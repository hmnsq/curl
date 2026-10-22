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
struct POP3 {
  curl_pp_transfer transfer;
  char *id;               /* Message ID */
  char *custom;           /* Custom Request */
};

/*
 * POP3 connection state
 */
typedef enum {
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

struct pop3_conn {
  struct pingpong pp;
  pop3state state;        /* Always use pop3.c:state() to change state! */
  size_t eob;             /* Number of bytes of the EOB (End Of Body) that
                             have been received so far */
  size_t strip;           /* Number of bytes from the start to ignore as
                             non-body */
  struct SASL sasl;       /* SASL-related storage */
  char *apoptimestamp;    /* APOP timestamp from the server greeting */
  unsigned char authtypes; /* Accepted authentication types */
  unsigned char preftype;  /* Preferred authentication type */
  BIT(ssldone);           /* Is connect() over SSL done? */
  BIT(tls_supported);     /* StartTLS capability supported by server */
};

struct pop3_cmd {
  const char *name;
  unsigned short nlen;
  BIT(multiline); /* response is multi-line with last '.' line */
  BIT(multiline_with_args); /* is multi-line when command has args */
};

static const struct pop3_cmd pop3cmds[] = {
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
static CURLcode pop3_parse_url_options(struct connectdata *conn)
{
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  const char *ptr = conn->options;

  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  док(!result && ptr && *ptr) {
    const char *key = ptr;
    const char *value;

    док(*ptr && *ptr != '=')
      ptr++;

    value = ptr + 1;

    док(*ptr && *ptr != ';')
      ptr++;

    ако(curl_strnequal(key, "AUTH=", 5)) {
      result = Curl_sasl_parse_url_auth_option(&pop3c->sasl,
                                               value, ptr - value);

      ако(result && curl_strnequal(value, "+APOP", ptr - value)) {
        pop3c->preftype = POP3_TYPE_APOP;
        pop3c->sasl.prefmech = SASL_AUTH_NONE;
        result = CURLE_OK;
      }
    }
    инако
      result = CURLE_URL_MALFORMAT;

    ако(*ptr == ';')
      ptr++;
  }

  ако(pop3c->preftype != POP3_TYPE_APOP)
    switch(pop3c->sasl.prefmech) {
    case SASL_AUTH_NONE:
      pop3c->preftype = POP3_TYPE_NONE;
      break;
    case SASL_AUTH_DEFAULT:
      pop3c->preftype = POP3_TYPE_ANY;
      break;
    default:
      pop3c->preftype = POP3_TYPE_SASL;
      break;
    }

  врати result;
}

/***********************************************************************
 *
 * pop3_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
static CURLcode pop3_parse_url_path(struct Curl_easy *data)
{
  /* The POP3 struct is already initialized in pop3_connect() */
  struct POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  const char *path = &data->state.up.path[1]; /* skip leading path */

  ако(!pop3)
    врати CURLE_FAILED_INIT;
  /* URL decode the path for the message ID */
  врати Curl_urldecode(path, 0, &pop3->id, NULL, REJECT_CTRL);
}

/***********************************************************************
 *
 * pop3_parse_custom_request()
 *
 * Parse the custom request.
 */
static CURLcode pop3_parse_custom_request(struct Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  struct POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  const char *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  ако(!pop3)
    врати CURLE_FAILED_INIT;
  /* URL decode the custom request */
  ако(custom)
    result = Curl_urldecode(custom, 0, &pop3->custom, NULL, REJECT_CTRL);

  врати result;
}

/* Return iff a command is defined as "multi-line" (RFC 1939),
 * has a response terminated by a last line with a '.'.
 */
static bool pop3_is_multiline(const char *cmdline)
{
  size_t i;
  за(i = 0; i < CURL_ARRAYSIZE(pop3cmds); ++i) {
    ако(curl_strnequal(pop3cmds[i].name, cmdline, pop3cmds[i].nlen)) {
      ако(!cmdline[pop3cmds[i].nlen])
        врати (bool)pop3cmds[i].multiline;
      инако ако(cmdline[pop3cmds[i].nlen] == ' ')
        врати (bool)pop3cmds[i].multiline_with_args;
    }
  }
  /* Unknown command, assume multi-line for backward compatibility with
   * earlier curl versions that only could do multi-line responses. */
  врати TRUE;
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
static bool pop3_endofresp(struct Curl_easy *data, struct connectdata *conn,
                           const char *line, size_t len, цео *resp)
{
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (void)data;
  DEBUGASSERT(pop3c);
  ако(!pop3c) /* internal error */
    врати TRUE;

  /* Do we have an error response? */
  ако(len >= 4 && !memcmp("-ERR", line, 4)) {
    *resp = '-';

    врати TRUE;
  }

  /* Are we processing CAPA command responses? */
  ако(pop3c->state == POP3_CAPA) {
    /* Do we have the terminating line? Per RFC 2449 this is a line
       containing only a single dot */
    ако((len == 3 && line[0] == '.' && line[1] == '\r') ||
       (len == 2 && line[0] == '.' && line[1] == '\n'))
      /* Treat the response as a success */
      *resp = '+';
    инако
      /* Treat the response as an untagged continuation */
      *resp = '*';

    врати TRUE;
  }

  /* Do we have a success response? */
  ако(len >= 3 && !memcmp("+OK", line, 3)) {
    *resp = '+';

    врати TRUE;
  }

  /* Do we have a continuation response? */
  ако(len >= 1 && line[0] == '+') {
    *resp = '*';

    врати TRUE;
  }

  врати FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * pop3_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
static CURLcode pop3_get_message(struct Curl_easy *data, struct bufref *out)
{
  struct pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  char *message;
  size_t len;

  ако(!pop3c)
    врати CURLE_FAILED_INIT;
  message = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;
  ако(len > 2) {
    /* Find the start of the message */
    len -= 2;
    за(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    док(len--)
      ако(!ISBLANK(message[len]) && !ISNEWLINE(message[len]))
        break;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, NULL);
  }
  инако
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, NULL);

  врати CURLE_OK;
}

/***********************************************************************
 *
 * pop3_state()
 *
 * This is the ONLY way to change POP3 state!
 */
static void pop3_state(struct Curl_easy *data, pop3state newstate)
{
  struct pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  ако(pop3c) {
#ако defined(DEBUGBUILD) && defined(CURLVERBOSE)
    /* for debug purposes */
    static const char * const names[] = {
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

    ако(pop3c->state != newstate)
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
static CURLcode pop3_perform_capa(struct Curl_easy *data,
                                  struct connectdata *conn)
{
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;

  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  pop3c->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  pop3c->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  pop3c->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPA command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "CAPA");

  ако(!result)
    pop3_state(data, POP3_CAPA);

  врати result;
}

/***********************************************************************
 *
 * pop3_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
static CURLcode pop3_perform_starttls(struct Curl_easy *data,
                                      struct connectdata *conn)
{
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;

  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  /* Send the STLS command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "STLS");
  ако(!result)
    pop3_state(data, POP3_STARTTLS);

  врати result;
}

/***********************************************************************
 *
 * pop3_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
static CURLcode pop3_perform_upgrade_tls(struct Curl_easy *data,
                                         struct connectdata *conn)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;
  bool ssldone = FALSE;

  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  ако(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ако(result)
      goto out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_pop3s;
  }

  DEBUGASSERT(!pop3c->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "pop3_perform_upgrade_tls, connect -> %d, %d",
               (цео)result, ssldone));
  ако(!result && ssldone) {
    pop3c->ssldone = ssldone;
    /* perform CAPA now, changes pop3c->state out of POP3_UPGRADETLS */
    result = pop3_perform_capa(data, conn);
  }
out:
  врати result;
#инако
  (void)data;
  (void)conn;
  врати CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * pop3_perform_user()
 *
 * Sends a clear text USER command to authenticate with.
 */
static CURLcode pop3_perform_user(struct Curl_easy *data,
                                  struct connectdata *conn)
{
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;

  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  ако(!conn->creds) {
    pop3_state(data, POP3_STOP);

    врати result;
  }

  /* Send the USER command */
  result = Curl_pp_sendf(data, &pop3c->pp, "USER %s",
                         Curl_creds_user(conn->creds));
  ако(!result)
    pop3_state(data, POP3_USER);

  врати result;
}

#ifndef CURL_DISABLE_DIGEST_AUTH
/***********************************************************************
 *
 * pop3_perform_apop()
 *
 * Sends an APOP command to authenticate with.
 */
static CURLcode pop3_perform_apop(struct Curl_easy *data,
                                  struct connectdata *conn)
{
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  size_t i;
  struct MD5_context *ctxt;
  unsigned char digest[MD5_DIGEST_LEN];
  char secret[(2 * MD5_DIGEST_LEN) + 1];

  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  ако(!data->state.creds) {
    pop3_state(data, POP3_STOP);

    врати result;
  }

  /* Create the digest */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ако(!ctxt)
    врати CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (const unsigned char *)pop3c->apoptimestamp,
                  curlx_uztoui(strlen(pop3c->apoptimestamp)));

  Curl_MD5_update(ctxt, (const unsigned char *)Curl_creds_passwd(conn->creds),
                  curlx_uztoui(strlen(Curl_creds_passwd(conn->creds))));

  /* Finalise the digest */
  Curl_MD5_final(ctxt, digest);

  /* Convert the calculated 16 octet digest into a 32-byte hex string */
  за(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&secret[2 * i], 3, "%02x", digest[i]);

  result = Curl_pp_sendf(data, &pop3c->pp, "APOP %s %s",
                         Curl_creds_user(conn->creds), secret);

  ако(!result)
    pop3_state(data, POP3_APOP);

  врати result;
}
#endif

/***********************************************************************
 *
 * pop3_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
static CURLcode pop3_perform_auth(struct Curl_easy *data,
                                  const char *mech,
                                  const struct bufref *initresp)
{
  struct pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  const char *ir = Curl_bufref_ptr(initresp);

  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  ако(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    result = Curl_pp_sendf(data, &pop3c->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  инако {
    /* Send the AUTH command */
    result = Curl_pp_sendf(data, &pop3c->pp, "AUTH %s", mech);
  }

  врати result;
}

/***********************************************************************
 *
 * pop3_continue_auth()
 *
 * Sends SASL continuation data.
 */
static CURLcode pop3_continue_auth(struct Curl_easy *data,
                                   const char *mech,
                                   const struct bufref *resp)
{
  struct pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);

  (void)mech;
  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  врати Curl_pp_sendf(data, &pop3c->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * pop3_cancel_auth()
 *
 * Sends SASL cancellation.
 */
static CURLcode pop3_cancel_auth(struct Curl_easy *data, const char *mech)
{
  struct pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);

  (void)mech;
  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  врати Curl_pp_sendf(data, &pop3c->pp, "*");
}

/***********************************************************************
 *
 * pop3_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to APOP and clear text should a
 * common mechanism not be available between the client and server.
 */
static CURLcode pop3_perform_authentication(struct Curl_easy *data,
                                            struct connectdata *conn)
{
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  saslprogress progress = SASL_IDLE;

  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  /* Check we have enough data to authenticate with and end the
     connect phase if we do not */
  ако(!Curl_sasl_can_authenticate(&pop3c->sasl, data)) {
    pop3_state(data, POP3_STOP);
    врати result;
  }

  ако(pop3c->authtypes & pop3c->preftype & POP3_TYPE_SASL) {
    /* Calculate the SASL login details */
    result = Curl_sasl_start(&pop3c->sasl, data, FALSE, &progress);

    ако(!result && progress == SASL_INPROGRESS)
      pop3_state(data, POP3_AUTH);
  }

  ако(!result && progress == SASL_IDLE) {
#ifndef CURL_DISABLE_DIGEST_AUTH
    ако(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
      /* Perform APOP authentication */
      result = pop3_perform_apop(data, conn);
    инако
#endif
    ако(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
      /* Perform clear text authentication */
      result = pop3_perform_user(data, conn);
    инако
      result = Curl_sasl_is_blocked(&pop3c->sasl, data);
  }

  врати result;
}

/***********************************************************************
 *
 * pop3_perform_command()
 *
 * Sends a POP3 based command.
 */
static CURLcode pop3_perform_command(struct Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  struct connectdata *conn = data->conn;
  struct POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  const char *command = NULL;

  ако(!pop3 || !pop3c)
    врати CURLE_FAILED_INIT;

  /* Calculate the default command */
  ако(pop3->id[0] == '\0' || data->set.list_only) {
    command = "LIST";

    ако(pop3->id[0] != '\0')
      /* Message specific LIST so skip the BODY transfer */
      pop3->transfer = PPTRANSFER_INFO;
  }
  инако
    command = "RETR";

  ако(pop3->custom && pop3->custom[0] != '\0')
    command = pop3->custom;

  /* Send the command */
  ако(pop3->id[0] != '\0')
    result = Curl_pp_sendf(data, &pop3c->pp, "%s %s", command, pop3->id);
  инако
    result = Curl_pp_sendf(data, &pop3c->pp, "%s", command);

  ако(!result) {
    pop3_state(data, POP3_COMMAND);
    data->req.no_body = !pop3_is_multiline(command);
  }

  врати result;
}

/***********************************************************************
 *
 * pop3_perform_quit()
 *
 * Performs the quit action prior to sclose() be called.
 */
static CURLcode pop3_perform_quit(struct Curl_easy *data,
                                  struct connectdata *conn)
{
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;

  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  /* Send the QUIT command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "QUIT");
  ако(!result)
    pop3_state(data, POP3_QUIT);

  врати result;
}

/* For the initial server greeting */
static CURLcode pop3_state_servergreet_resp(struct Curl_easy *data,
                                            цео pop3code,
                                            pop3state instate)
{
  CURLcode result = CURLE_OK;
  struct connectdata *conn = data->conn;
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  const char *line;
  size_t len;

  (void)instate;
  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  line = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  ако(pop3code != '+') {
    failf(data, "Got unexpected pop3-server response");
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  инако ако(len > 3) {
    /* Does the server support APOP authentication? */
    const char *lt;
    const char *gt = NULL;

    /* Look for the APOP timestamp */
    lt = memchr(line, '<', len);
    ако(lt)
      /* search the remainder for '>' */
      gt = memchr(lt, '>', len - (lt - line));
    ако(gt) {
      /* the length of the timestamp, including the brackets */
      size_t timestamplen = gt - lt + 1;
      const char *at = memchr(lt, '@', timestamplen);
      /* If the timestamp does not contain '@' it is not (as required by
         RFC-1939) conformant to the RFC-822 message id syntax, and we
         therefore do not use APOP authentication. */
      ако(at) {
        /* dupe the timestamp */
        pop3c->apoptimestamp = curlx_memdup0(lt, timestamplen);
        ако(!pop3c->apoptimestamp)
          врати CURLE_OUT_OF_MEMORY;
        /* Store the APOP capability */
        pop3c->authtypes |= POP3_TYPE_APOP;
      }
    }

    ако(!result)
      result = pop3_perform_capa(data, conn);
  }

  врати result;
}

/* For CAPA responses */
static CURLcode pop3_state_capa_resp(struct Curl_easy *data, цео pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  struct connectdata *conn = data->conn;
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  const char *line;
  size_t len;

  (void)instate;
  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  line = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  /* Do we have an untagged continuation response? */
  ако(pop3code == '*') {
    /* Does the server support the STLS capability? */
    ако(len >= 4 && curl_strnequal(line, "STLS", 4))
      pop3c->tls_supported = TRUE;

    /* Does the server support clear text authentication? */
    инако ако(len >= 4 && curl_strnequal(line, "USER", 4))
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    /* Does the server support SASL based authentication? */
    инако ако(len >= 5 && curl_strnequal(line, "SASL ", 5)) {
      pop3c->authtypes |= POP3_TYPE_SASL;

      /* Advance past the SASL keyword */
      line += 5;
      len -= 5;

      /* Loop through the data line */
      за(;;) {
        size_t llen;
        size_t wordlen = 0;
        unsigned short mechbit;

        док(len && (ISBLANK(*line) || ISNEWLINE(*line))) {
          line++;
          len--;
        }

        ако(!len)
          break;

        /* Extract the word */
        док(wordlen < len && !ISBLANK(line[wordlen]) &&
              !ISNEWLINE(line[wordlen]))
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(line, wordlen, &llen);
        ако(mechbit && llen == wordlen)
          pop3c->sasl.authmechs |= mechbit;

        line += wordlen;
        len -= wordlen;
      }
    }
  }
  инако {
    /* Clear text is supported when CAPA is not recognised */
    ако(pop3code != '+')
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    ако(!data->set.use_ssl || Curl_conn_is_ssl(conn, FIRSTSOCKET))
      result = pop3_perform_authentication(data, conn);
    инако ако(pop3code == '+' && pop3c->tls_supported)
      /* Switch to TLS connection now */
      result = pop3_perform_starttls(data, conn);
    инако ако(data->set.use_ssl <= CURLUSESSL_TRY)
      /* Fallback and carry on with authentication */
      result = pop3_perform_authentication(data, conn);
    инако {
      failf(data, "STLS not supported.");
      result = CURLE_USE_SSL_FAILED;
    }
  }

  врати result;
}

/* For STARTTLS responses */
static CURLcode pop3_state_starttls_resp(struct Curl_easy *data,
                                         struct connectdata *conn,
                                         цео pop3code,
                                         pop3state instate)
{
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  (void)instate;

  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  /* Pipelining in response is forbidden. */
  ако(pop3c->pp.overflow)
    врати CURLE_WEIRD_SERVER_REPLY;

  ако(pop3code != '+') {
    ако(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied");
      result = CURLE_USE_SSL_FAILED;
    }
    инако
      result = pop3_perform_authentication(data, conn);
  }
  инако
    pop3_state(data, POP3_UPGRADETLS);

  врати result;
}

/* For SASL authentication responses */
static CURLcode pop3_state_auth_resp(struct Curl_easy *data,
                                     цео pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  struct connectdata *conn = data->conn;
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  saslprogress progress;

  (void)instate;
  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  result = Curl_sasl_continue(&pop3c->sasl, data, pop3code, &progress);
  ако(!result)
    switch(progress) {
    case SASL_DONE:
      pop3_state(data, POP3_STOP);  /* Authenticated */
      break;
    case SASL_IDLE:            /* No mechanism left after cancellation */
#ifndef CURL_DISABLE_DIGEST_AUTH
      ако(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
        /* Perform APOP authentication */
        result = pop3_perform_apop(data, conn);
      инако
#endif
      ако(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
        /* Perform clear text authentication */
        result = pop3_perform_user(data, conn);
      инако {
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
      }
      break;
    default:
      break;
    }

  врати result;
}

#ifndef CURL_DISABLE_DIGEST_AUTH
/* For APOP responses */
static CURLcode pop3_state_apop_resp(struct Curl_easy *data, цео pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  (void)instate;

  ако(pop3code != '+') {
    failf(data, "Authentication failed: %d", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  инако
    /* End of connect phase */
    pop3_state(data, POP3_STOP);

  врати result;
}
#endif

/* For USER responses */
static CURLcode pop3_state_user_resp(struct Curl_easy *data, цео pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  struct connectdata *conn = data->conn;
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (void)instate;

  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  ако(pop3code != '+') {
    failf(data, "Access denied. %c", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  инако
    /* Send the PASS command */
    result = Curl_pp_sendf(data, &pop3c->pp, "PASS %s",
                           Curl_creds_passwd(conn->creds));
  ако(!result)
    pop3_state(data, POP3_PASS);

  врати result;
}

/* For PASS responses */
static CURLcode pop3_state_pass_resp(struct Curl_easy *data, цео pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  (void)instate;

  ако(pop3code != '+') {
    failf(data, "Access denied. %c", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  инако
    /* End of connect phase */
    pop3_state(data, POP3_STOP);

  врати result;
}

/***********************************************************************
 *
 * pop3_write()
 *
 * This function scans the body after the end-of-body and writes everything
 * until the end is found.
 */
static CURLcode pop3_write(struct Curl_easy *data, const char *str,
                           size_t nread, bool is_eos)
{
  /* This code could be made into a special function in the handler struct */
  CURLcode result = CURLE_OK;
  struct connectdata *conn = data->conn;
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  bool strip_dot = FALSE;
  size_t last = 0;
  size_t i;
  (void)is_eos;

  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  /* Search through the buffer looking for the end-of-body marker which is
     5 bytes (0d 0a 2e 0d 0a). Note that a line starting with a dot matches
     the eob so the server will have prefixed it with an extra dot which we
     need to strip out. Additionally the marker could of course be spread out
     over 5 different data chunks. */
  за(i = 0; i < nread; i++) {
    size_t prev = pop3c->eob;

    switch(str[i]) {
    case 0x0d:
      ако(pop3c->eob == 0) {
        pop3c->eob++;

        ако(i) {
          /* Write out the body part that did not match */
          result = Curl_client_write(data, CLIENTWRITE_BODY, &str[last],
                                     i - last);

          ако(result)
            врати result;

          last = i;
        }
      }
      инако ако(pop3c->eob == 3)
        pop3c->eob++;
      инако
        /* If the character match was not at position 0 or 3 then restart the
           pattern matching */
        pop3c->eob = 1;
      break;

    case 0x0a:
      ако(pop3c->eob == 1 || pop3c->eob == 4)
        pop3c->eob++;
      инако
        /* If the character match was not at position 1 or 4 then start the
           search again */
        pop3c->eob = 0;
      break;

    case 0x2e:
      ако(pop3c->eob == 2)
        pop3c->eob++;
      инако ако(pop3c->eob == 3) {
        /* We have an extra dot after the CRLF which we need to strip off */
        strip_dot = TRUE;
        pop3c->eob = 0;
      }
      инако
        /* If the character match was not at position 2 then start the search
           again */
        pop3c->eob = 0;
      break;

    default:
      pop3c->eob = 0;
      break;
    }

    /* Did we have a partial match which has subsequently failed? */
    ако(prev && prev >= pop3c->eob) {
      /* Strip can only be non-zero for the first mismatch after CRLF and
         then both prev and strip are equal and nothing will be output below */
      док(prev && pop3c->strip) {
        prev--;
        pop3c->strip--;
      }

      ако(prev) {
        /* If the partial match was the CRLF and dot then only write the CRLF
           as the server would have inserted the dot */
        ако(strip_dot && prev - 1 > 0) {
          result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB,
                                     prev - 1);
        }
        инако ако(!strip_dot) {
          result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB,
                                     prev);
        }
        инако {
          result = CURLE_OK;
        }

        ако(result)
          врати result;

        last = i;
        strip_dot = FALSE;
      }
    }
  }

  ако(pop3c->eob == POP3_EOB_LEN) {
    /* We have a full match so the transfer is done, however we must transfer
    the CRLF at the start of the EOB as this is considered to be part of the
    message as per RFC-1939, sect. 3 */
    result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB, 2);

    CURL_REQ_CLEAR_RECV(data);
    pop3c->eob = 0;

    врати result;
  }

  ако(pop3c->eob)
    /* While EOB is matching nothing should be output */
    врати CURLE_OK;

  ако(nread - last) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, &str[last],
                               nread - last);
  }

  врати result;
}

/* For command responses */
static CURLcode pop3_state_command_resp(struct Curl_easy *data,
                                        цео pop3code,
                                        pop3state instate)
{
  CURLcode result = CURLE_OK;
  struct connectdata *conn = data->conn;
  struct POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  struct pingpong *pp;

  (void)instate;
  ако(!pop3 || !pop3c)
    врати CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  ако(pop3code != '+') {
    pop3_state(data, POP3_STOP);
    врати CURLE_WEIRD_SERVER_REPLY;
  }

  /* This 'OK' line ends with a CR LF pair which is the two first bytes of the
     EOB string so count this is two matching bytes. This is necessary to make
     the code detect the EOB if the only data than comes now is %2e CR LF like
     when there is no body to return. */
  pop3c->eob = 2;

  /* Since this initial CR LF pair is not part of the actual body, we set
     the strip counter here so that these bytes will not be delivered. */
  pop3c->strip = 2;

  ако(pop3->transfer == PPTRANSFER_BODY) {
    /* POP3 download */
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);

    ако(pp->overflow) {
      /* The recv buffer contains data that is actually body content so send
         it as such. Note that there may even be additional "headers" after
         the body */

      /* keep only the overflow */
      curlx_dyn_tail(&pp->recvbuf, pp->overflow);
      pp->nfinal = 0; /* done */

      ако(!data->req.no_body) {
        result = pop3_write(data, curlx_dyn_ptr(&pp->recvbuf),
                            curlx_dyn_len(&pp->recvbuf), FALSE);
        ако(result)
          врати result;
      }

      /* reset the buffer */
      curlx_dyn_reset(&pp->recvbuf);
      pp->overflow = 0;
    }
  }
  инако
    pp->overflow = 0;

  /* End of DO phase */
  pop3_state(data, POP3_STOP);

  врати result;
}

static CURLcode pop3_statemachine(struct Curl_easy *data,
                                  struct connectdata *conn)
{
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  цео pop3code;
  struct pingpong *pp;
  size_t nread = 0;
  (void)data;

  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not POP3 */
upgrade_tls:
  ако(pop3c->state == POP3_UPGRADETLS) {
    result = pop3_perform_upgrade_tls(data, conn);
    ако(result || (pop3c->state == POP3_UPGRADETLS))
      врати result;
  }

  /* Flush any data that needs to be sent */
  ако(pp->sendleft)
    врати Curl_pp_flushsend(data, pp);

  do {
     /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, pp, &pop3code, &nread);
    ако(result)
      врати result;

    ако(!pop3code)
      break;

    /* We have now received a full POP3 server response */
    switch(pop3c->state) {
    case POP3_SERVERGREET:
      result = pop3_state_servergreet_resp(data, pop3code, pop3c->state);
      break;

    case POP3_CAPA:
      result = pop3_state_capa_resp(data, pop3code, pop3c->state);
      break;

    case POP3_STARTTLS:
      result = pop3_state_starttls_resp(data, conn, pop3code, pop3c->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      ако(!result && (pop3c->state == POP3_UPGRADETLS))
        goto upgrade_tls;
      break;

    case POP3_AUTH:
      result = pop3_state_auth_resp(data, pop3code, pop3c->state);
      break;

#ifndef CURL_DISABLE_DIGEST_AUTH
    case POP3_APOP:
      result = pop3_state_apop_resp(data, pop3code, pop3c->state);
      break;
#endif

    case POP3_USER:
      result = pop3_state_user_resp(data, pop3code, pop3c->state);
      break;

    case POP3_PASS:
      result = pop3_state_pass_resp(data, pop3code, pop3c->state);
      break;

    case POP3_COMMAND:
      result = pop3_state_command_resp(data, pop3code, pop3c->state);
      break;

    case POP3_QUIT:
      pop3_state(data, POP3_STOP);
      break;

    default:
      /* internal error */
      pop3_state(data, POP3_STOP);
      break;
    }
  } док(!result && pop3c->state != POP3_STOP && Curl_pp_moredata(pp));

  врати result;
}

/* Called repeatedly until done from multi.c */
static CURLcode pop3_multi_statemach(struct Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  struct connectdata *conn = data->conn;
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  ако(!pop3c)
    врати CURLE_FAILED_INIT;
  result = Curl_pp_statemach(data, &pop3c->pp, FALSE, FALSE);
  *done = (pop3c->state == POP3_STOP);

  врати result;
}

static CURLcode pop3_block_statemach(struct Curl_easy *data,
                                     struct connectdata *conn,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  док(pop3c->state != POP3_STOP && !result)
    result = Curl_pp_statemach(data, &pop3c->pp, TRUE, disconnecting);

  врати result;
}

/* For the POP3 "protocol connect" and "doing" phases only */
static CURLcode pop3_pollset(struct Curl_easy *data,
                             struct easy_pollset *ps)
{
  struct pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  врати pop3c ? Curl_pp_pollset(data, &pop3c->pp, ps) : CURLE_OK;
}

/* SASL parameters for the pop3 protocol */
static const struct SASLproto saslpop3 = {
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
static CURLcode pop3_connect(struct Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  struct connectdata *conn = data->conn;
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  struct pingpong *pp = pop3c ? &pop3c->pp : NULL;

  *done = FALSE; /* default to not done yet */
  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  PINGPONG_SETUP(pp, pop3_statemachine, pop3_endofresp);

  /* Set the default preferred authentication type and mechanism */
  pop3c->preftype = POP3_TYPE_ANY;
  Curl_sasl_init(&pop3c->sasl, data, &saslpop3);

  /* Initialize the pingpong layer */
  Curl_pp_init(pp, Curl_pgrs_now(data));

  /* Parse the URL options */
  result = pop3_parse_url_options(conn);
  ако(result)
    врати result;

  /* Start off waiting for the server greeting response */
  pop3_state(data, POP3_SERVERGREET);

  result = pop3_multi_statemach(data, done);

  врати result;
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
static CURLcode pop3_done(struct Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  struct POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);

  (void)premature;

  ако(!pop3)
    врати CURLE_OK;

  ако(status) {
    CURL_TRC_M(data, "POP3 done with bad status");
    connclose(data->conn);
    result = status;         /* use the already set error code */
  }

  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);

  /* Clear the transfer mode for the next request */
  pop3->transfer = PPTRANSFER_BODY;

  врати result;
}

/***********************************************************************
 *
 * pop3_perform()
 *
 * This is the actual DO function for POP3. Get a message/listing according to
 * the options previously setup.
 */
static CURLcode pop3_perform(struct Curl_easy *data, bool *connected,
                             bool *dophase_done)
{
  /* This is POP3 and no proxy */
  CURLcode result = CURLE_OK;
  struct POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);

  ако(!pop3)
    врати CURLE_FAILED_INIT;

  DEBUGF(infof(data, "DO phase starts"));

  /* Start the first command in the DO phase, may alter data->req.no_body */
  result = pop3_perform_command(data);
  ако(result)
    врати result;

  ако(data->req.no_body)
    /* Requested no body means no transfer */
    pop3->transfer = PPTRANSFER_INFO;

  *dophase_done = FALSE; /* not done yet */

  /* Run the state-machine */
  result = pop3_multi_statemach(data, dophase_done);
  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  ако(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  врати result;
}

/* Call this when the DO phase has completed */
static CURLcode pop3_dophase_done(struct Curl_easy *data, bool connected)
{
  (void)data;
  (void)connected;

  врати CURLE_OK;
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
static CURLcode pop3_regular_transfer(struct Curl_easy *data,
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
  ако(!result && *dophase_done)
    result = pop3_dophase_done(data, connected);

  врати result;
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
static CURLcode pop3_do(struct Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  *done = FALSE; /* default to false */

  /* Parse the URL path */
  result = pop3_parse_url_path(data);
  ако(result)
    врати result;

  /* Parse the custom request */
  result = pop3_parse_custom_request(data);
  ако(result)
    врати result;

  result = pop3_regular_transfer(data, done);

  врати result;
}

/***********************************************************************
 *
 * pop3_disconnect()
 *
 * Disconnect from an POP3 server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
static CURLcode pop3_disconnect(struct Curl_easy *data,
                                struct connectdata *conn, bool dead_connection)
{
  struct pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (void)data;

  ако(!pop3c)
    врати CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  ако(!dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &pop3c->pp) &&
     !pop3_perform_quit(data, conn))
    (void)pop3_block_statemach(data, conn, TRUE); /* ignore errors on QUIT */

  /* Disconnect from the server */
  Curl_pp_disconnect(&pop3c->pp);

  /* Cleanup our connection based variables */
  curlx_safefree(pop3c->apoptimestamp);

  врати CURLE_OK;
}

/* Called from multi.c while DOing */
static CURLcode pop3_doing(struct Curl_easy *data, bool *dophase_done)
{
  CURLcode result = pop3_multi_statemach(data, dophase_done);

  ако(result)
    DEBUGF(infof(data, "DO phase failed"));
  инако ако(*dophase_done) {
    result = pop3_dophase_done(data, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  врати result;
}

static void pop3_easy_dtor(const void *key, size_t klen, void *entry)
{
  struct POP3 *pop3 = entry;
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
  struct pop3_conn *pop3c = entry;
  (void)key;
  (void)klen;
  DEBUGASSERT(pop3c);
  Curl_pp_disconnect(&pop3c->pp);
  curlx_safefree(pop3c->apoptimestamp);
  curlx_free(pop3c);
}

static CURLcode pop3_setup_connection(struct Curl_easy *data,
                                      struct connectdata *conn)
{
  struct pop3_conn *pop3c;
  struct POP3 *pop3 = curlx_calloc(1, sizeof(*pop3));
  ако(!pop3 ||
     Curl_meta_set(data, CURL_META_POP3_EASY, pop3, pop3_easy_dtor))
    врати CURLE_OUT_OF_MEMORY;

  pop3c = curlx_calloc(1, sizeof(*pop3c));
  ако(!pop3c ||
     Curl_conn_meta_set(conn, CURL_META_POP3_CONN, pop3c, pop3_conn_dtor))
    врати CURLE_OUT_OF_MEMORY;

  врати CURLE_OK;
}

/*
 * POP3 protocol.
 */
const struct Curl_protocol Curl_protocol_pop3 = {
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
