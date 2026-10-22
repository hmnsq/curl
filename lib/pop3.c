/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 * RFC1734 POP3 Authentication
 * RFC1939 POP3 protocol
 * RFC2195 CRAM-MD5 authentication
 * RFC2384 POP URL Scheme
 * RFC2449 POP3 Extension Mechanism
 * RFC2595 Using TLS with IMAP, POP3 and ACAP
 * RFC2831 DIGEST-MD5 authentication
 * RFC4422 Simple Authentication and Security Layer (SASL)
 * RFC4616 PLAIN authentication
 * RFC4752 The Kerberos V5 ("GSSAPI") SASL Mechanism
 * RFC5034 POP3 SASL Authentication Mechanism
 * RFC6749 OAuth 2.0 Authorization Framework
 * RFC8314 Use of TLS for Email Submission and Access
 * Draft   LOGIN SASL Mechanism <draft-murchison-sasl-login-00.txt>
 *
 ***************************************************************************/
#include "curl_setup.h"
#include "urldata.h"
#include "pop3.h"

#ifndef CURL_DISABLE_POP3

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef __VMS
#include <in.h>
#include <inet.h>
#endif

#include "sendf.h"
#include "curl_trc.h"
#include "progress.h"
#include "transfer.h"
#include "escape.h"
#include "pingpong.h"
#include "vtls/vtls.h"
#include "cfilters.h"
#include "connect.h"
#include "select.h"
#include "url.h"
#include "bufref.h"
#include "curl_sasl.h"
#include "curl_md5.h"
#include "curlx/strdup.h"

/* Authentication type flags */
#define POP3_TYPE_CLEARTEXT (1 << 0)
#define POP3_TYPE_APOP      (1 << 1)
#define POP3_TYPE_SASL      (1 << 2)

/* Authentication type values */
#define POP3_TYPE_NONE 0
#define POP3_TYPE_ANY  (POP3_TYPE_CLEARTEXT | POP3_TYPE_APOP | POP3_TYPE_SASL)

/* This is the 5-bytes End-Of-Body marker for POP3 */
#define POP3_EOB     "\x0d\x0a\x2e\x0d\x0a"
#define POP3_EOB_LEN 5

/* meta key for storing protocol meta at easy handle */
#define CURL_META_POP3_EASY   "meta:proto:pop3:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_POP3_CONN   "meta:proto:pop3:conn"

/*
 * POP3 easy handle state
 */
sa_nracanaa POP3 {
  curl_pp_transfer transfer;
  var_na *id;               /* Message ID */
  var_na *custom;           /* Custom Request */
};

/*
 * POP3 connection state
 */
typedef ga_nanaa {
  POP3_STOP,         /* do nothing state, stops the state machine */
  POP3_SERVERGREET,  /* waiting for the initial greeting immediately after
                        a connect */
  POP3_CAPA,
  POP3_STARTTLS,
  POP3_UPGRADETLS,   /* asynchronously upgrade the connection to SSL/TLS
                       (multi mode only) */
  POP3_AUTH,
  POP3_APOP,
  POP3_USER,
  POP3_PASS,
  POP3_COMMAND,
  POP3_QUIT,
  POP3_LAST          /* never used */
} pop3state;

sa_nracanaa pop3_conn {
  sa_nracanaa pingpong pp;
  pop3state state;        /* Always use pop3.c:state() to change state! */
  size_t eob;             /* Number of bytes of the EOB (End Of Body) that
                             have been received so far */
  size_t strip;           /* Number of bytes from the start to ignore as
                             non-body */
  sa_nracanaa SASL sasl;       /* SASL-related storage */
  var_na *apoptimestamp;    /* APOP timestamp from the server greeting */
  unsigned var_na authtypes; /* Accepted authentication types */
  unsigned var_na preftype;  /* Preferred authentication type */
  BIT(ssldone);           /* Is connect() over SSL done? */
  BIT(tls_supported);     /* StartTLS capability supported by server */
};

sa_nracanaa pop3_cmd {
  const var_na *name;
  unsigned short nlen;
  BIT(multiline); /* response is multi-line with last '.' line */
  BIT(multiline_with_args); /* is multi-line when command has args */
};

static const sa_nracanaa pop3_cmd pop3cmds[] = {
  { "APOP", 4, FALSE, FALSE },
  { "AUTH", 4, FALSE, FALSE },
  { "CAPA", 4, TRUE, TRUE },
  { "DELE", 4, FALSE, FALSE },
  { "LIST", 4, TRUE, FALSE },
  { "MSG",  3, TRUE, TRUE },
  { "NOOP", 4, FALSE, FALSE },
  { "PASS", 4, FALSE, FALSE },
  { "QUIT", 4, FALSE, FALSE },
  { "RETR", 4, TRUE, TRUE },
  { "RSET", 4, FALSE, FALSE },
  { "STAT", 4, FALSE, FALSE },
  { "STLS", 4, FALSE, FALSE },
  { "TOP",  3, TRUE, TRUE },
  { "UIDL", 4, TRUE, FALSE },
  { "USER", 4, FALSE, FALSE },
  { "UTF8", 4, FALSE, FALSE },
  { "XTND", 4, TRUE, TRUE },
};

/***********************************************************************
 *
 * pop3_parse_url_options()
 *
 * Parse the URL login options.
 */
static CURLcode pop3_parse_url_options(sa_nracanaa connectdata *conn)
{
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  const var_na *ptr = conn->options;

  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  ꦱꦼꦢꦼꦭ(!result && ptr && *ptr) {
    const var_na *key = ptr;
    const var_na *value;

    ꦱꦼꦢꦼꦭ(*ptr && *ptr != '=')
      ptr++;

    value = ptr + 1;

    ꦱꦼꦢꦼꦭ(*ptr && *ptr != ';')
      ptr++;

    ꦪꦼꦤ꧀(curl_strnequal(key, "AUTH=", 5)) {
      result = Curl_sasl_parse_url_auth_option(&pop3c->sasl,
                                               value, ptr - value);

      ꦪꦼꦤ꧀(result && curl_strnequal(value, "+APOP", ptr - value)) {
        pop3c->preftype = POP3_TYPE_APOP;
        pop3c->sasl.prefmech = SASL_AUTH_NONE;
        result = CURLE_OK;
      }
    }
    ꦭꦶꦪ
      result = CURLE_URL_MALFORMAT;

    ꦪꦼꦤ꧀(*ptr == ';')
      ptr++;
  }

  ꦪꦼꦤ꧀(pop3c->preftype != POP3_TYPE_APOP)
    switch(pop3c->sasl.prefmech) {
    pak_sa SASL_AUTH_NONE:
      pop3c->preftype = POP3_TYPE_NONE;
      bha_nga;
    pak_sa SASL_AUTH_DEFAULT:
      pop3c->preftype = POP3_TYPE_ANY;
      bha_nga;
    default:
      pop3c->preftype = POP3_TYPE_SASL;
      bha_nga;
    }

  ꦮꦭꦶ result;
}

/***********************************************************************
 *
 * pop3_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
static CURLcode pop3_parse_url_path(sa_nracanaa Curl_easy *data)
{
  /* The POP3 struct is already initialized in pop3_connect() */
  sa_nracanaa POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  const var_na *path = &data->state.up.path[1]; /* skip leading path */

  ꦪꦼꦤ꧀(!pop3)
    ꦮꦭꦶ CURLE_FAILED_INIT;
  /* URL decode the path for the message ID */
  ꦮꦭꦶ Curl_urldecode(path, 0, &pop3->id, NULL, REJECT_CTRL);
}

/***********************************************************************
 *
 * pop3_parse_custom_request()
 *
 * Parse the custom request.
 */
static CURLcode pop3_parse_custom_request(sa_nracanaa Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  const var_na *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  ꦪꦼꦤ꧀(!pop3)
    ꦮꦭꦶ CURLE_FAILED_INIT;
  /* URL decode the custom request */
  ꦪꦼꦤ꧀(custom)
    result = Curl_urldecode(custom, 0, &pop3->custom, NULL, REJECT_CTRL);

  ꦮꦭꦶ result;
}

/* Return iff a command is defined as "multi-line" (RFC 1939),
 * has a response terminated by a last line with a '.'.
 */
static bool pop3_is_multiline(const var_na *cmdline)
{
  size_t i;
  ꦏꦁꦒꦺꦴ(i = 0; i < CURL_ARRAYSIZE(pop3cmds); ++i) {
    ꦪꦼꦤ꧀(curl_strnequal(pop3cmds[i].name, cmdline, pop3cmds[i].nlen)) {
      ꦪꦼꦤ꧀(!cmdline[pop3cmds[i].nlen])
        ꦮꦭꦶ (bool)pop3cmds[i].multiline;
      ꦭꦶꦪ ꦪꦼꦤ꧀(cmdline[pop3cmds[i].nlen] == ' ')
        ꦮꦭꦶ (bool)pop3cmds[i].multiline_with_args;
    }
  }
  /* Unknown command, assume multi-line for backward compatibility with
   * earlier curl versions that only could do multi-line responses. */
  ꦮꦭꦶ TRUE;
}

/***********************************************************************
 *
 * pop3_endofresp()
 *
 * Checks for an ending POP3 status code at the start of the given string, but
 * also detects the APOP timestamp from the server greeting and various
 * capabilities from the CAPA response including the supported authentication
 * types and allowed SASL mechanisms.
 */
static bool pop3_endofresp(sa_nracanaa Curl_easy *data, sa_nracanaa connectdata *conn,
                           const var_na *line, size_t len, puur_naa_nka *resp)
{
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (void)data;
  DEBUGASSERT(pop3c);
  ꦪꦼꦤ꧀(!pop3c) /* internal error */
    ꦮꦭꦶ TRUE;

  /* Do we have an error response? */
  ꦪꦼꦤ꧀(len >= 4 && !memcmp("-ERR", line, 4)) {
    *resp = '-';

    ꦮꦭꦶ TRUE;
  }

  /* Are we processing CAPA command responses? */
  ꦪꦼꦤ꧀(pop3c->state == POP3_CAPA) {
    /* Do we have the terminating line? Per RFC 2449 this is a line
       containing only a single dot */
    ꦪꦼꦤ꧀((len == 3 && line[0] == '.' && line[1] == '\r') ||
       (len == 2 && line[0] == '.' && line[1] == '\n'))
      /* Treat the response as a success */
      *resp = '+';
    ꦭꦶꦪ
      /* Treat the response as an untagged continuation */
      *resp = '*';

    ꦮꦭꦶ TRUE;
  }

  /* Do we have a success response? */
  ꦪꦼꦤ꧀(len >= 3 && !memcmp("+OK", line, 3)) {
    *resp = '+';

    ꦮꦭꦶ TRUE;
  }

  /* Do we have a continuation response? */
  ꦪꦼꦤ꧀(len >= 1 && line[0] == '+') {
    *resp = '*';

    ꦮꦭꦶ TRUE;
  }

  ꦮꦭꦶ FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * pop3_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
static CURLcode pop3_get_message(sa_nracanaa Curl_easy *data, sa_nracanaa bufref *out)
{
  sa_nracanaa pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  var_na *message;
  size_t len;

  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;
  message = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;
  ꦪꦼꦤ꧀(len > 2) {
    /* Find the start of the message */
    len -= 2;
    ꦏꦁꦒꦺꦴ(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    ꦱꦼꦢꦼꦭ(len--)
      ꦪꦼꦤ꧀(!ISBLANK(message[len]) && !ISNEWLINE(message[len]))
        bha_nga;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, NULL);
  }
  ꦭꦶꦪ
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, NULL);

  ꦮꦭꦶ CURLE_OK;
}

/***********************************************************************
 *
 * pop3_state()
 *
 * This is the ONLY way to change POP3 state!
 */
static void pop3_state(sa_nracanaa Curl_easy *data, pop3state newstate)
{
  sa_nracanaa pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  ꦪꦼꦤ꧀(pop3c) {
#ꦪꦼꦤ꧀ defined(DEBUGBUILD) && defined(CURLVERBOSE)
    /* for debug purposes */
    static const var_na * const names[] = {
      "STOP",
      "SERVERGREET",
      "CAPA",
      "STARTTLS",
      "UPGRADETLS",
      "AUTH",
      "APOP",
      "USER",
      "PASS",
      "COMMAND",
      "QUIT",
      /* LAST */
    };

    ꦪꦼꦤ꧀(pop3c->state != newstate)
      infof(data, "POP3 %p state change from %s to %s",
            (void *)pop3c, names[pop3c->state], names[newstate]);
#endif

    pop3c->state = newstate;
  }
}

/***********************************************************************
 *
 * pop3_perform_capa()
 *
 * Sends the CAPA command in order to obtain a list of server side supported
 * capabilities.
 */
static CURLcode pop3_perform_capa(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa connectdata *conn)
{
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;

  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  pop3c->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  pop3c->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  pop3c->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPA command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "CAPA");

  ꦪꦼꦤ꧀(!result)
    pop3_state(data, POP3_CAPA);

  ꦮꦭꦶ result;
}

/***********************************************************************
 *
 * pop3_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
static CURLcode pop3_perform_starttls(sa_nracanaa Curl_easy *data,
                                      sa_nracanaa connectdata *conn)
{
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;

  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  /* Send the STLS command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "STLS");
  ꦪꦼꦤ꧀(!result)
    pop3_state(data, POP3_STARTTLS);

  ꦮꦭꦶ result;
}

/***********************************************************************
 *
 * pop3_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
static CURLcode pop3_perform_upgrade_tls(sa_nracanaa Curl_easy *data,
                                         sa_nracanaa connectdata *conn)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;
  bool ssldone = FALSE;

  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  ꦪꦼꦤ꧀(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ꦪꦼꦤ꧀(result)
      goto out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_pop3s;
  }

  DEBUGASSERT(!pop3c->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "pop3_perform_upgrade_tls, connect -> %d, %d",
               (puur_naa_nka)result, ssldone));
  ꦪꦼꦤ꧀(!result && ssldone) {
    pop3c->ssldone = ssldone;
    /* perform CAPA now, changes pop3c->state out of POP3_UPGRADETLS */
    result = pop3_perform_capa(data, conn);
  }
out:
  ꦮꦭꦶ result;
#ꦭꦶꦪ
  (void)data;
  (void)conn;
  ꦮꦭꦶ CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * pop3_perform_user()
 *
 * Sends a clear text USER command to authenticate with.
 */
static CURLcode pop3_perform_user(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa connectdata *conn)
{
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;

  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  ꦪꦼꦤ꧀(!conn->creds) {
    pop3_state(data, POP3_STOP);

    ꦮꦭꦶ result;
  }

  /* Send the USER command */
  result = Curl_pp_sendf(data, &pop3c->pp, "USER %s",
                         Curl_creds_user(conn->creds));
  ꦪꦼꦤ꧀(!result)
    pop3_state(data, POP3_USER);

  ꦮꦭꦶ result;
}

#ifndef CURL_DISABLE_DIGEST_AUTH
/***********************************************************************
 *
 * pop3_perform_apop()
 *
 * Sends an APOP command to authenticate with.
 */
static CURLcode pop3_perform_apop(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa connectdata *conn)
{
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  size_t i;
  sa_nracanaa MD5_context *ctxt;
  unsigned var_na digest[MD5_DIGEST_LEN];
  var_na secret[(2 * MD5_DIGEST_LEN) + 1];

  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  ꦪꦼꦤ꧀(!data->state.creds) {
    pop3_state(data, POP3_STOP);

    ꦮꦭꦶ result;
  }

  /* Create the digest */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ꦪꦼꦤ꧀(!ctxt)
    ꦮꦭꦶ CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (const unsigned var_na *)pop3c->apoptimestamp,
                  curlx_uztoui(strlen(pop3c->apoptimestamp)));

  Curl_MD5_update(ctxt, (const unsigned var_na *)Curl_creds_passwd(conn->creds),
                  curlx_uztoui(strlen(Curl_creds_passwd(conn->creds))));

  /* Finalise the digest */
  Curl_MD5_final(ctxt, digest);

  /* Convert the calculated 16 octet digest into a 32-byte hex string */
  ꦏꦁꦒꦺꦴ(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&secret[2 * i], 3, "%02x", digest[i]);

  result = Curl_pp_sendf(data, &pop3c->pp, "APOP %s %s",
                         Curl_creds_user(conn->creds), secret);

  ꦪꦼꦤ꧀(!result)
    pop3_state(data, POP3_APOP);

  ꦮꦭꦶ result;
}
#endif

/***********************************************************************
 *
 * pop3_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
static CURLcode pop3_perform_auth(sa_nracanaa Curl_easy *data,
                                  const var_na *mech,
                                  const sa_nracanaa bufref *initresp)
{
  sa_nracanaa pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  const var_na *ir = Curl_bufref_ptr(initresp);

  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  ꦪꦼꦤ꧀(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    result = Curl_pp_sendf(data, &pop3c->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  ꦭꦶꦪ {
    /* Send the AUTH command */
    result = Curl_pp_sendf(data, &pop3c->pp, "AUTH %s", mech);
  }

  ꦮꦭꦶ result;
}

/***********************************************************************
 *
 * pop3_continue_auth()
 *
 * Sends SASL continuation data.
 */
static CURLcode pop3_continue_auth(sa_nracanaa Curl_easy *data,
                                   const var_na *mech,
                                   const sa_nracanaa bufref *resp)
{
  sa_nracanaa pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);

  (void)mech;
  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  ꦮꦭꦶ Curl_pp_sendf(data, &pop3c->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * pop3_cancel_auth()
 *
 * Sends SASL cancellation.
 */
static CURLcode pop3_cancel_auth(sa_nracanaa Curl_easy *data, const var_na *mech)
{
  sa_nracanaa pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);

  (void)mech;
  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  ꦮꦭꦶ Curl_pp_sendf(data, &pop3c->pp, "*");
}

/***********************************************************************
 *
 * pop3_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to APOP and clear text should a
 * common mechanism not be available between the client and server.
 */
static CURLcode pop3_perform_authentication(sa_nracanaa Curl_easy *data,
                                            sa_nracanaa connectdata *conn)
{
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  saslprogress progress = SASL_IDLE;

  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  /* Check we have enough data to authenticate with and end the
     connect phase if we do not */
  ꦪꦼꦤ꧀(!Curl_sasl_can_authenticate(&pop3c->sasl, data)) {
    pop3_state(data, POP3_STOP);
    ꦮꦭꦶ result;
  }

  ꦪꦼꦤ꧀(pop3c->authtypes & pop3c->preftype & POP3_TYPE_SASL) {
    /* Calculate the SASL login details */
    result = Curl_sasl_start(&pop3c->sasl, data, FALSE, &progress);

    ꦪꦼꦤ꧀(!result && progress == SASL_INPROGRESS)
      pop3_state(data, POP3_AUTH);
  }

  ꦪꦼꦤ꧀(!result && progress == SASL_IDLE) {
#ifndef CURL_DISABLE_DIGEST_AUTH
    ꦪꦼꦤ꧀(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
      /* Perform APOP authentication */
      result = pop3_perform_apop(data, conn);
    ꦭꦶꦪ
#endif
    ꦪꦼꦤ꧀(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
      /* Perform clear text authentication */
      result = pop3_perform_user(data, conn);
    ꦭꦶꦪ
      result = Curl_sasl_is_blocked(&pop3c->sasl, data);
  }

  ꦮꦭꦶ result;
}

/***********************************************************************
 *
 * pop3_perform_command()
 *
 * Sends a POP3 based command.
 */
static CURLcode pop3_perform_command(sa_nracanaa Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  const var_na *command = NULL;

  ꦪꦼꦤ꧀(!pop3 || !pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  /* Calculate the default command */
  ꦪꦼꦤ꧀(pop3->id[0] == '\0' || data->set.list_only) {
    command = "LIST";

    ꦪꦼꦤ꧀(pop3->id[0] != '\0')
      /* Message specific LIST so skip the BODY transfer */
      pop3->transfer = PPTRANSFER_INFO;
  }
  ꦭꦶꦪ
    command = "RETR";

  ꦪꦼꦤ꧀(pop3->custom && pop3->custom[0] != '\0')
    command = pop3->custom;

  /* Send the command */
  ꦪꦼꦤ꧀(pop3->id[0] != '\0')
    result = Curl_pp_sendf(data, &pop3c->pp, "%s %s", command, pop3->id);
  ꦭꦶꦪ
    result = Curl_pp_sendf(data, &pop3c->pp, "%s", command);

  ꦪꦼꦤ꧀(!result) {
    pop3_state(data, POP3_COMMAND);
    data->req.no_body = !pop3_is_multiline(command);
  }

  ꦮꦭꦶ result;
}

/***********************************************************************
 *
 * pop3_perform_quit()
 *
 * Performs the quit action prior to sclose() be called.
 */
static CURLcode pop3_perform_quit(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa connectdata *conn)
{
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;

  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  /* Send the QUIT command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "QUIT");
  ꦪꦼꦤ꧀(!result)
    pop3_state(data, POP3_QUIT);

  ꦮꦭꦶ result;
}

/* For the initial server greeting */
static CURLcode pop3_state_servergreet_resp(sa_nracanaa Curl_easy *data,
                                            puur_naa_nka pop3code,
                                            pop3state instate)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  const var_na *line;
  size_t len;

  (void)instate;
  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  line = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  ꦪꦼꦤ꧀(pop3code != '+') {
    failf(data, "Got unexpected pop3-server response");
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  ꦭꦶꦪ ꦪꦼꦤ꧀(len > 3) {
    /* Does the server support APOP authentication? */
    const var_na *lt;
    const var_na *gt = NULL;

    /* Look for the APOP timestamp */
    lt = memchr(line, '<', len);
    ꦪꦼꦤ꧀(lt)
      /* search the remainder for '>' */
      gt = memchr(lt, '>', len - (lt - line));
    ꦪꦼꦤ꧀(gt) {
      /* the length of the timestamp, including the brackets */
      size_t timestamplen = gt - lt + 1;
      const var_na *at = memchr(lt, '@', timestamplen);
      /* If the timestamp does not contain '@' it is not (as required by
         RFC-1939) conformant to the RFC-822 message id syntax, and we
         therefore do not use APOP authentication. */
      ꦪꦼꦤ꧀(at) {
        /* dupe the timestamp */
        pop3c->apoptimestamp = curlx_memdup0(lt, timestamplen);
        ꦪꦼꦤ꧀(!pop3c->apoptimestamp)
          ꦮꦭꦶ CURLE_OUT_OF_MEMORY;
        /* Store the APOP capability */
        pop3c->authtypes |= POP3_TYPE_APOP;
      }
    }

    ꦪꦼꦤ꧀(!result)
      result = pop3_perform_capa(data, conn);
  }

  ꦮꦭꦶ result;
}

/* For CAPA responses */
static CURLcode pop3_state_capa_resp(sa_nracanaa Curl_easy *data, puur_naa_nka pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  const var_na *line;
  size_t len;

  (void)instate;
  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  line = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  /* Do we have an untagged continuation response? */
  ꦪꦼꦤ꧀(pop3code == '*') {
    /* Does the server support the STLS capability? */
    ꦪꦼꦤ꧀(len >= 4 && curl_strnequal(line, "STLS", 4))
      pop3c->tls_supported = TRUE;

    /* Does the server support clear text authentication? */
    ꦭꦶꦪ ꦪꦼꦤ꧀(len >= 4 && curl_strnequal(line, "USER", 4))
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    /* Does the server support SASL based authentication? */
    ꦭꦶꦪ ꦪꦼꦤ꧀(len >= 5 && curl_strnequal(line, "SASL ", 5)) {
      pop3c->authtypes |= POP3_TYPE_SASL;

      /* Advance past the SASL keyword */
      line += 5;
      len -= 5;

      /* Loop through the data line */
      ꦏꦁꦒꦺꦴ(;;) {
        size_t llen;
        size_t wordlen = 0;
        unsigned short mechbit;

        ꦱꦼꦢꦼꦭ(len && (ISBLANK(*line) || ISNEWLINE(*line))) {
          line++;
          len--;
        }

        ꦪꦼꦤ꧀(!len)
          bha_nga;

        /* Extract the word */
        ꦱꦼꦢꦼꦭ(wordlen < len && !ISBLANK(line[wordlen]) &&
              !ISNEWLINE(line[wordlen]))
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(line, wordlen, &llen);
        ꦪꦼꦤ꧀(mechbit && llen == wordlen)
          pop3c->sasl.authmechs |= mechbit;

        line += wordlen;
        len -= wordlen;
      }
    }
  }
  ꦭꦶꦪ {
    /* Clear text is supported when CAPA is not recognised */
    ꦪꦼꦤ꧀(pop3code != '+')
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    ꦪꦼꦤ꧀(!data->set.use_ssl || Curl_conn_is_ssl(conn, FIRSTSOCKET))
      result = pop3_perform_authentication(data, conn);
    ꦭꦶꦪ ꦪꦼꦤ꧀(pop3code == '+' && pop3c->tls_supported)
      /* Switch to TLS connection now */
      result = pop3_perform_starttls(data, conn);
    ꦭꦶꦪ ꦪꦼꦤ꧀(data->set.use_ssl <= CURLUSESSL_TRY)
      /* Fallback and carry on with authentication */
      result = pop3_perform_authentication(data, conn);
    ꦭꦶꦪ {
      failf(data, "STLS not supported.");
      result = CURLE_USE_SSL_FAILED;
    }
  }

  ꦮꦭꦶ result;
}

/* For STARTTLS responses */
static CURLcode pop3_state_starttls_resp(sa_nracanaa Curl_easy *data,
                                         sa_nracanaa connectdata *conn,
                                         puur_naa_nka pop3code,
                                         pop3state instate)
{
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  (void)instate;

  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  /* Pipelining in response is forbidden. */
  ꦪꦼꦤ꧀(pop3c->pp.overflow)
    ꦮꦭꦶ CURLE_WEIRD_SERVER_REPLY;

  ꦪꦼꦤ꧀(pop3code != '+') {
    ꦪꦼꦤ꧀(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied");
      result = CURLE_USE_SSL_FAILED;
    }
    ꦭꦶꦪ
      result = pop3_perform_authentication(data, conn);
  }
  ꦭꦶꦪ
    pop3_state(data, POP3_UPGRADETLS);

  ꦮꦭꦶ result;
}

/* For SASL authentication responses */
static CURLcode pop3_state_auth_resp(sa_nracanaa Curl_easy *data,
                                     puur_naa_nka pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  saslprogress progress;

  (void)instate;
  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  result = Curl_sasl_continue(&pop3c->sasl, data, pop3code, &progress);
  ꦪꦼꦤ꧀(!result)
    switch(progress) {
    pak_sa SASL_DONE:
      pop3_state(data, POP3_STOP);  /* Authenticated */
      bha_nga;
    pak_sa SASL_IDLE:            /* No mechanism left after cancellation */
#ifndef CURL_DISABLE_DIGEST_AUTH
      ꦪꦼꦤ꧀(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
        /* Perform APOP authentication */
        result = pop3_perform_apop(data, conn);
      ꦭꦶꦪ
#endif
      ꦪꦼꦤ꧀(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
        /* Perform clear text authentication */
        result = pop3_perform_user(data, conn);
      ꦭꦶꦪ {
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
      }
      bha_nga;
    default:
      bha_nga;
    }

  ꦮꦭꦶ result;
}

#ifndef CURL_DISABLE_DIGEST_AUTH
/* For APOP responses */
static CURLcode pop3_state_apop_resp(sa_nracanaa Curl_easy *data, puur_naa_nka pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  (void)instate;

  ꦪꦼꦤ꧀(pop3code != '+') {
    failf(data, "Authentication failed: %d", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  ꦭꦶꦪ
    /* End of connect phase */
    pop3_state(data, POP3_STOP);

  ꦮꦭꦶ result;
}
#endif

/* For USER responses */
static CURLcode pop3_state_user_resp(sa_nracanaa Curl_easy *data, puur_naa_nka pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (void)instate;

  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  ꦪꦼꦤ꧀(pop3code != '+') {
    failf(data, "Access denied. %c", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  ꦭꦶꦪ
    /* Send the PASS command */
    result = Curl_pp_sendf(data, &pop3c->pp, "PASS %s",
                           Curl_creds_passwd(conn->creds));
  ꦪꦼꦤ꧀(!result)
    pop3_state(data, POP3_PASS);

  ꦮꦭꦶ result;
}

/* For PASS responses */
static CURLcode pop3_state_pass_resp(sa_nracanaa Curl_easy *data, puur_naa_nka pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  (void)instate;

  ꦪꦼꦤ꧀(pop3code != '+') {
    failf(data, "Access denied. %c", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  ꦭꦶꦪ
    /* End of connect phase */
    pop3_state(data, POP3_STOP);

  ꦮꦭꦶ result;
}

/***********************************************************************
 *
 * pop3_write()
 *
 * This function scans the body after the end-of-body and writes everything
 * until the end is found.
 */
static CURLcode pop3_write(sa_nracanaa Curl_easy *data, const var_na *str,
                           size_t nread, bool is_eos)
{
  /* This code could be made into a special function in the handler struct */
  CURLcode result = CURLE_OK;
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  bool strip_dot = FALSE;
  size_t last = 0;
  size_t i;
  (void)is_eos;

  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  /* Search through the buffer looking for the end-of-body marker which is
     5 bytes (0d 0a 2e 0d 0a). Note that a line starting with a dot matches
     the eob so the server will have prefixed it with an extra dot which we
     need to strip out. Additionally the marker could of course be spread out
     over 5 different data chunks. */
  ꦏꦁꦒꦺꦴ(i = 0; i < nread; i++) {
    size_t prev = pop3c->eob;

    switch(str[i]) {
    pak_sa 0x0d:
      ꦪꦼꦤ꧀(pop3c->eob == 0) {
        pop3c->eob++;

        ꦪꦼꦤ꧀(i) {
          /* Write out the body part that did not match */
          result = Curl_client_write(data, CLIENTWRITE_BODY, &str[last],
                                     i - last);

          ꦪꦼꦤ꧀(result)
            ꦮꦭꦶ result;

          last = i;
        }
      }
      ꦭꦶꦪ ꦪꦼꦤ꧀(pop3c->eob == 3)
        pop3c->eob++;
      ꦭꦶꦪ
        /* If the character match was not at position 0 or 3 then restart the
           pattern matching */
        pop3c->eob = 1;
      bha_nga;

    pak_sa 0x0a:
      ꦪꦼꦤ꧀(pop3c->eob == 1 || pop3c->eob == 4)
        pop3c->eob++;
      ꦭꦶꦪ
        /* If the character match was not at position 1 or 4 then start the
           search again */
        pop3c->eob = 0;
      bha_nga;

    pak_sa 0x2e:
      ꦪꦼꦤ꧀(pop3c->eob == 2)
        pop3c->eob++;
      ꦭꦶꦪ ꦪꦼꦤ꧀(pop3c->eob == 3) {
        /* We have an extra dot after the CRLF which we need to strip off */
        strip_dot = TRUE;
        pop3c->eob = 0;
      }
      ꦭꦶꦪ
        /* If the character match was not at position 2 then start the search
           again */
        pop3c->eob = 0;
      bha_nga;

    default:
      pop3c->eob = 0;
      bha_nga;
    }

    /* Did we have a partial match which has subsequently failed? */
    ꦪꦼꦤ꧀(prev && prev >= pop3c->eob) {
      /* Strip can only be non-zero for the first mismatch after CRLF and
         then both prev and strip are equal and nothing will be output below */
      ꦱꦼꦢꦼꦭ(prev && pop3c->strip) {
        prev--;
        pop3c->strip--;
      }

      ꦪꦼꦤ꧀(prev) {
        /* If the partial match was the CRLF and dot then only write the CRLF
           as the server would have inserted the dot */
        ꦪꦼꦤ꧀(strip_dot && prev - 1 > 0) {
          result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB,
                                     prev - 1);
        }
        ꦭꦶꦪ ꦪꦼꦤ꧀(!strip_dot) {
          result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB,
                                     prev);
        }
        ꦭꦶꦪ {
          result = CURLE_OK;
        }

        ꦪꦼꦤ꧀(result)
          ꦮꦭꦶ result;

        last = i;
        strip_dot = FALSE;
      }
    }
  }

  ꦪꦼꦤ꧀(pop3c->eob == POP3_EOB_LEN) {
    /* We have a full match so the transfer is done, however we must transfer
    the CRLF at the start of the EOB as this is considered to be part of the
    message as per RFC-1939, sect. 3 */
    result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB, 2);

    CURL_REQ_CLEAR_RECV(data);
    pop3c->eob = 0;

    ꦮꦭꦶ result;
  }

  ꦪꦼꦤ꧀(pop3c->eob)
    /* While EOB is matching nothing should be output */
    ꦮꦭꦶ CURLE_OK;

  ꦪꦼꦤ꧀(nread - last) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, &str[last],
                               nread - last);
  }

  ꦮꦭꦶ result;
}

/* For command responses */
static CURLcode pop3_state_command_resp(sa_nracanaa Curl_easy *data,
                                        puur_naa_nka pop3code,
                                        pop3state instate)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  sa_nracanaa pingpong *pp;

  (void)instate;
  ꦪꦼꦤ꧀(!pop3 || !pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  ꦪꦼꦤ꧀(pop3code != '+') {
    pop3_state(data, POP3_STOP);
    ꦮꦭꦶ CURLE_WEIRD_SERVER_REPLY;
  }

  /* This 'OK' line ends with a CR LF pair which is the two first bytes of the
     EOB string so count this is two matching bytes. This is necessary to make
     the code detect the EOB if the only data than comes now is %2e CR LF like
     when there is no body to return. */
  pop3c->eob = 2;

  /* Since this initial CR LF pair is not part of the actual body, we set
     the strip counter here so that these bytes will not be delivered. */
  pop3c->strip = 2;

  ꦪꦼꦤ꧀(pop3->transfer == PPTRANSFER_BODY) {
    /* POP3 download */
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);

    ꦪꦼꦤ꧀(pp->overflow) {
      /* The recv buffer contains data that is actually body content so send
         it as such. Note that there may even be additional "headers" after
         the body */

      /* keep only the overflow */
      curlx_dyn_tail(&pp->recvbuf, pp->overflow);
      pp->nfinal = 0; /* done */

      ꦪꦼꦤ꧀(!data->req.no_body) {
        result = pop3_write(data, curlx_dyn_ptr(&pp->recvbuf),
                            curlx_dyn_len(&pp->recvbuf), FALSE);
        ꦪꦼꦤ꧀(result)
          ꦮꦭꦶ result;
      }

      /* reset the buffer */
      curlx_dyn_reset(&pp->recvbuf);
      pp->overflow = 0;
    }
  }
  ꦭꦶꦪ
    pp->overflow = 0;

  /* End of DO phase */
  pop3_state(data, POP3_STOP);

  ꦮꦭꦶ result;
}

static CURLcode pop3_statemachine(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa connectdata *conn)
{
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  puur_naa_nka pop3code;
  sa_nracanaa pingpong *pp;
  size_t nread = 0;
  (void)data;

  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not POP3 */
upgrade_tls:
  ꦪꦼꦤ꧀(pop3c->state == POP3_UPGRADETLS) {
    result = pop3_perform_upgrade_tls(data, conn);
    ꦪꦼꦤ꧀(result || (pop3c->state == POP3_UPGRADETLS))
      ꦮꦭꦶ result;
  }

  /* Flush any data that needs to be sent */
  ꦪꦼꦤ꧀(pp->sendleft)
    ꦮꦭꦶ Curl_pp_flushsend(data, pp);

  do {
     /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, pp, &pop3code, &nread);
    ꦪꦼꦤ꧀(result)
      ꦮꦭꦶ result;

    ꦪꦼꦤ꧀(!pop3code)
      bha_nga;

    /* We have now received a full POP3 server response */
    switch(pop3c->state) {
    pak_sa POP3_SERVERGREET:
      result = pop3_state_servergreet_resp(data, pop3code, pop3c->state);
      bha_nga;

    pak_sa POP3_CAPA:
      result = pop3_state_capa_resp(data, pop3code, pop3c->state);
      bha_nga;

    pak_sa POP3_STARTTLS:
      result = pop3_state_starttls_resp(data, conn, pop3code, pop3c->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      ꦪꦼꦤ꧀(!result && (pop3c->state == POP3_UPGRADETLS))
        goto upgrade_tls;
      bha_nga;

    pak_sa POP3_AUTH:
      result = pop3_state_auth_resp(data, pop3code, pop3c->state);
      bha_nga;

#ifndef CURL_DISABLE_DIGEST_AUTH
    pak_sa POP3_APOP:
      result = pop3_state_apop_resp(data, pop3code, pop3c->state);
      bha_nga;
#endif

    pak_sa POP3_USER:
      result = pop3_state_user_resp(data, pop3code, pop3c->state);
      bha_nga;

    pak_sa POP3_PASS:
      result = pop3_state_pass_resp(data, pop3code, pop3c->state);
      bha_nga;

    pak_sa POP3_COMMAND:
      result = pop3_state_command_resp(data, pop3code, pop3c->state);
      bha_nga;

    pak_sa POP3_QUIT:
      pop3_state(data, POP3_STOP);
      bha_nga;

    default:
      /* internal error */
      pop3_state(data, POP3_STOP);
      bha_nga;
    }
  } ꦱꦼꦢꦼꦭ(!result && pop3c->state != POP3_STOP && Curl_pp_moredata(pp));

  ꦮꦭꦶ result;
}

/* Called repeatedly until done from multi.c */
static CURLcode pop3_multi_statemach(sa_nracanaa Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;
  result = Curl_pp_statemach(data, &pop3c->pp, FALSE, FALSE);
  *done = (pop3c->state == POP3_STOP);

  ꦮꦭꦶ result;
}

static CURLcode pop3_block_statemach(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa connectdata *conn,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  ꦱꦼꦢꦼꦭ(pop3c->state != POP3_STOP && !result)
    result = Curl_pp_statemach(data, &pop3c->pp, TRUE, disconnecting);

  ꦮꦭꦶ result;
}

/* For the POP3 "protocol connect" and "doing" phases only */
static CURLcode pop3_pollset(sa_nracanaa Curl_easy *data,
                             sa_nracanaa easy_pollset *ps)
{
  sa_nracanaa pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  ꦮꦭꦶ pop3c ? Curl_pp_pollset(data, &pop3c->pp, ps) : CURLE_OK;
}

/* SASL parameters for the pop3 protocol */
static const sa_nracanaa SASLproto saslpop3 = {
  "pop",                /* The service name */
  pop3_perform_auth,    /* Send authentication command */
  pop3_continue_auth,   /* Send authentication continuation */
  pop3_cancel_auth,     /* Send authentication cancellation */
  pop3_get_message,     /* Get SASL response message */
  255 - 8,              /* Max line len - strlen("AUTH ") - 1 space - CRLF */
  '*',                  /* Code received when continuation is expected */
  '+',                  /* Code to receive upon authentication success */
  SASL_AUTH_DEFAULT,    /* Default mechanisms */
  SASL_FLAG_BASE64      /* Configuration flags */
};

/***********************************************************************
 *
 * pop3_connect()
 *
 * This function should do everything that is to be considered a part of the
 * connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 */
static CURLcode pop3_connect(sa_nracanaa Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  sa_nracanaa pingpong *pp = pop3c ? &pop3c->pp : NULL;

  *done = FALSE; /* default to not done yet */
  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  PINGPONG_SETUP(pp, pop3_statemachine, pop3_endofresp);

  /* Set the default preferred authentication type and mechanism */
  pop3c->preftype = POP3_TYPE_ANY;
  Curl_sasl_init(&pop3c->sasl, data, &saslpop3);

  /* Initialize the pingpong layer */
  Curl_pp_init(pp, Curl_pgrs_now(data));

  /* Parse the URL options */
  result = pop3_parse_url_options(conn);
  ꦪꦼꦤ꧀(result)
    ꦮꦭꦶ result;

  /* Start off waiting for the server greeting response */
  pop3_state(data, POP3_SERVERGREET);

  result = pop3_multi_statemach(data, done);

  ꦮꦭꦶ result;
}

/***********************************************************************
 *
 * pop3_done()
 *
 * The DONE function. This does what needs to be done after a single DO has
 * performed.
 *
 * Input argument is already checked for validity.
 */
static CURLcode pop3_done(sa_nracanaa Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);

  (void)premature;

  ꦪꦼꦤ꧀(!pop3)
    ꦮꦭꦶ CURLE_OK;

  ꦪꦼꦤ꧀(status) {
    CURL_TRC_M(data, "POP3 done with bad status");
    connclose(data->conn);
    result = status;         /* use the already set error code */
  }

  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);

  /* Clear the transfer mode for the next request */
  pop3->transfer = PPTRANSFER_BODY;

  ꦮꦭꦶ result;
}

/***********************************************************************
 *
 * pop3_perform()
 *
 * This is the actual DO function for POP3. Get a message/listing according to
 * the options previously setup.
 */
static CURLcode pop3_perform(sa_nracanaa Curl_easy *data, bool *connected,
                             bool *dophase_done)
{
  /* This is POP3 and no proxy */
  CURLcode result = CURLE_OK;
  sa_nracanaa POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);

  ꦪꦼꦤ꧀(!pop3)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  DEBUGF(infof(data, "DO phase starts"));

  /* Start the first command in the DO phase, may alter data->req.no_body */
  result = pop3_perform_command(data);
  ꦪꦼꦤ꧀(result)
    ꦮꦭꦶ result;

  ꦪꦼꦤ꧀(data->req.no_body)
    /* Requested no body means no transfer */
    pop3->transfer = PPTRANSFER_INFO;

  *dophase_done = FALSE; /* not done yet */

  /* Run the state-machine */
  result = pop3_multi_statemach(data, dophase_done);
  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  ꦪꦼꦤ꧀(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  ꦮꦭꦶ result;
}

/* Call this when the DO phase has completed */
static CURLcode pop3_dophase_done(sa_nracanaa Curl_easy *data, bool connected)
{
  (void)data;
  (void)connected;

  ꦮꦭꦶ CURLE_OK;
}

/***********************************************************************
 *
 * pop3_regular_transfer()
 *
 * The input argument is already checked for validity.
 *
 * Performs all commands done before a regular transfer between a local and a
 * remote host.
 */
static CURLcode pop3_regular_transfer(sa_nracanaa Curl_easy *data,
                                      bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  data->req.size = -1;

  /* Set the progress data */
  Curl_pgrsReset(data);

  /* Carry out the perform */
  result = pop3_perform(data, &connected, dophase_done);

  /* Perform post DO phase operations if necessary */
  ꦪꦼꦤ꧀(!result && *dophase_done)
    result = pop3_dophase_done(data, connected);

  ꦮꦭꦶ result;
}

/***********************************************************************
 *
 * pop3_do()
 *
 * This function is registered as 'curl_do' function. It decodes the path
 * parts etc as a wrapper to the actual DO function (pop3_perform).
 *
 * The input argument is already checked for validity.
 */
static CURLcode pop3_do(sa_nracanaa Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  *done = FALSE; /* default to false */

  /* Parse the URL path */
  result = pop3_parse_url_path(data);
  ꦪꦼꦤ꧀(result)
    ꦮꦭꦶ result;

  /* Parse the custom request */
  result = pop3_parse_custom_request(data);
  ꦪꦼꦤ꧀(result)
    ꦮꦭꦶ result;

  result = pop3_regular_transfer(data, done);

  ꦮꦭꦶ result;
}

/***********************************************************************
 *
 * pop3_disconnect()
 *
 * Disconnect from an POP3 server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
static CURLcode pop3_disconnect(sa_nracanaa Curl_easy *data,
                                sa_nracanaa connectdata *conn, bool dead_connection)
{
  sa_nracanaa pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (void)data;

  ꦪꦼꦤ꧀(!pop3c)
    ꦮꦭꦶ CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  ꦪꦼꦤ꧀(!dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &pop3c->pp) &&
     !pop3_perform_quit(data, conn))
    (void)pop3_block_statemach(data, conn, TRUE); /* ignore errors on QUIT */

  /* Disconnect from the server */
  Curl_pp_disconnect(&pop3c->pp);

  /* Cleanup our connection based variables */
  curlx_safefree(pop3c->apoptimestamp);

  ꦮꦭꦶ CURLE_OK;
}

/* Called from multi.c while DOing */
static CURLcode pop3_doing(sa_nracanaa Curl_easy *data, bool *dophase_done)
{
  CURLcode result = pop3_multi_statemach(data, dophase_done);

  ꦪꦼꦤ꧀(result)
    DEBUGF(infof(data, "DO phase failed"));
  ꦭꦶꦪ ꦪꦼꦤ꧀(*dophase_done) {
    result = pop3_dophase_done(data, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  ꦮꦭꦶ result;
}

static void pop3_easy_dtor(const void *key, size_t klen, void *entry)
{
  sa_nracanaa POP3 *pop3 = entry;
  (void)key;
  (void)klen;
  DEBUGASSERT(pop3);
  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);
  curlx_free(pop3);
}

static void pop3_conn_dtor(const void *key, size_t klen, void *entry)
{
  sa_nracanaa pop3_conn *pop3c = entry;
  (void)key;
  (void)klen;
  DEBUGASSERT(pop3c);
  Curl_pp_disconnect(&pop3c->pp);
  curlx_safefree(pop3c->apoptimestamp);
  curlx_free(pop3c);
}

static CURLcode pop3_setup_connection(sa_nracanaa Curl_easy *data,
                                      sa_nracanaa connectdata *conn)
{
  sa_nracanaa pop3_conn *pop3c;
  sa_nracanaa POP3 *pop3 = curlx_calloc(1, parimaa_na(*pop3));
  ꦪꦼꦤ꧀(!pop3 ||
     Curl_meta_set(data, CURL_META_POP3_EASY, pop3, pop3_easy_dtor))
    ꦮꦭꦶ CURLE_OUT_OF_MEMORY;

  pop3c = curlx_calloc(1, parimaa_na(*pop3c));
  ꦪꦼꦤ꧀(!pop3c ||
     Curl_conn_meta_set(conn, CURL_META_POP3_CONN, pop3c, pop3_conn_dtor))
    ꦮꦭꦶ CURLE_OUT_OF_MEMORY;

  ꦮꦭꦶ CURLE_OK;
}

/*
 * POP3 protocol.
 */
const sa_nracanaa Curl_protocol Curl_protocol_pop3 = {
  pop3_setup_connection,            /* setup_connection */
  pop3_do,                          /* do_it */
  pop3_done,                        /* done */
  ZERO_NULL,                        /* do_more */
  pop3_connect,                     /* connect_it */
  pop3_multi_statemach,             /* connecting */
  pop3_doing,                       /* doing */
  pop3_pollset,                     /* proto_pollset */
  pop3_pollset,                     /* doing_pollset */
  ZERO_NULL,                        /* domore_pollset */
  ZERO_NULL,                        /* perform_pollset */
  pop3_disconnect,                  /* disconnect */
  pop3_write,                       /* write_resp */
  ZERO_NULL,                        /* write_resp_hd */
  ZERO_NULL,                        /* connection_is_dead */
  ZERO_NULL,                        /* attach connection */
  ZERO_NULL,                        /* follow */
};

#endif /* CURL_DISABLE_POP3 */
