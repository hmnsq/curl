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
#અન્તર્ભાવ "curl_setup.h"
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "pop3.h"

#ifndef CURL_DISABLE_POP3

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#અન્તર્ભાવ <netdb.h>
#endif
#ifdef __VMS
#અન્તર્ભાવ <in.h>
#અન્તર્ભાવ <inet.h>
#endif

#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "escape.h"
#અન્તર્ભાવ "pingpong.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "bufref.h"
#અન્તર્ભાવ "curl_sasl.h"
#અન્તર્ભાવ "curl_md5.h"
#અન્તર્ભાવ "curlx/strdup.h"

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
સંરચના POP3 {
  curl_pp_transfer transfer;
  અક્ષર *id;               /* Message ID */
  અક્ષર *custom;           /* Custom Request */
};

/*
 * POP3 connection state
 */
પ્રકારવ્યાખ્યા ગણના {
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

સંરચના pop3_conn {
  સંરચના pingpong pp;
  pop3state state;        /* Always use pop3.c:state() to change state! */
  size_t eob;             /* Number of bytes of the EOB (End Of Body) that
                             have been received so far */
  size_t strip;           /* Number of bytes from the start to ignore as
                             non-body */
  સંરચના SASL sasl;       /* SASL-related storage */
  અક્ષર *apoptimestamp;    /* APOP timestamp from the server greeting */
  unsigned અક્ષર authtypes; /* Accepted authentication types */
  unsigned અક્ષર preftype;  /* Preferred authentication type */
  BIT(ssldone);           /* Is connect() over SSL done? */
  BIT(tls_supported);     /* StartTLS capability supported by server */
};

સંરચના pop3_cmd {
  અચલ અક્ષર *name;
  unsigned short nlen;
  BIT(multiline); /* response is multi-line with last '.' line */
  BIT(multiline_with_args); /* is multi-line when command has args */
};

સ્થિર અચલ સંરચના pop3_cmd pop3cmds[] = {
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
સ્થિર CURLcode pop3_parse_url_options(સંરચના connectdata *conn)
{
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  અચલ અક્ષર *ptr = conn->options;

  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  જ્યારે(!result && ptr && *ptr) {
    અચલ અક્ષર *ચાવી = ptr;
    અચલ અક્ષર *કિંમત;

    જ્યારે(*ptr && *ptr != '=')
      ptr++;

    કિંમત = ptr + 1;

    જ્યારે(*ptr && *ptr != ';')
      ptr++;

    જો(curl_strnequal(ચાવી, "AUTH=", 5)) {
      result = Curl_sasl_parse_url_auth_option(&pop3c->sasl,
                                               કિંમત, ptr - કિંમત);

      જો(result && curl_strnequal(કિંમત, "+APOP", ptr - કિંમત)) {
        pop3c->preftype = POP3_TYPE_APOP;
        pop3c->sasl.prefmech = SASL_AUTH_NONE;
        result = CURLE_OK;
      }
    }
    નહીંતર
      result = CURLE_URL_MALFORMAT;

    જો(*ptr == ';')
      ptr++;
  }

  જો(pop3c->preftype != POP3_TYPE_APOP)
    પસંદગી(pop3c->sasl.prefmech) {
    સ્થિતિ SASL_AUTH_NONE:
      pop3c->preftype = POP3_TYPE_NONE;
      તોડો;
    સ્થિતિ SASL_AUTH_DEFAULT:
      pop3c->preftype = POP3_TYPE_ANY;
      તોડો;
    મૂળભૂત:
      pop3c->preftype = POP3_TYPE_SASL;
      તોડો;
    }

  પાછા result;
}

/***********************************************************************
 *
 * pop3_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
સ્થિર CURLcode pop3_parse_url_path(સંરચના Curl_easy *data)
{
  /* The POP3 struct is already initialized in pop3_connect() */
  સંરચના POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  અચલ અક્ષર *path = &data->state.up.path[1]; /* skip leading path */

  જો(!pop3)
    પાછા CURLE_FAILED_INIT;
  /* URL decode the path for the message ID */
  પાછા Curl_urldecode(path, 0, &pop3->id, ખાલી, REJECT_CTRL);
}

/***********************************************************************
 *
 * pop3_parse_custom_request()
 *
 * Parse the custom request.
 */
સ્થિર CURLcode pop3_parse_custom_request(સંરચના Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  સંરચના POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  અચલ અક્ષર *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  જો(!pop3)
    પાછા CURLE_FAILED_INIT;
  /* URL decode the custom request */
  જો(custom)
    result = Curl_urldecode(custom, 0, &pop3->custom, ખાલી, REJECT_CTRL);

  પાછા result;
}

/* Return iff a command is defined as "multi-line" (RFC 1939),
 * has a response terminated by a last line with a '.'.
 */
સ્થિર bool pop3_is_multiline(અચલ અક્ષર *cmdline)
{
  size_t i;
  માટે(i = 0; i < CURL_ARRAYSIZE(pop3cmds); ++i) {
    જો(curl_strnequal(pop3cmds[i].name, cmdline, pop3cmds[i].nlen)) {
      જો(!cmdline[pop3cmds[i].nlen])
        પાછા (bool)pop3cmds[i].multiline;
      નહીંતર જો(cmdline[pop3cmds[i].nlen] == ' ')
        પાછા (bool)pop3cmds[i].multiline_with_args;
    }
  }
  /* Unknown command, assume multi-line for backward compatibility with
   * earlier curl versions that only could do multi-line responses. */
  પાછા TRUE;
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
સ્થિર bool pop3_endofresp(સંરચના Curl_easy *data, સંરચના connectdata *conn,
                           અચલ અક્ષર *લીટી, size_t len, પૂર્ણાંક *resp)
{
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (શૂન્ય)data;
  DEBUGASSERT(pop3c);
  જો(!pop3c) /* internal error */
    પાછા TRUE;

  /* Do we have an error response? */
  જો(len >= 4 && !memcmp("-ERR", લીટી, 4)) {
    *resp = '-';

    પાછા TRUE;
  }

  /* Are we processing CAPA command responses? */
  જો(pop3c->state == POP3_CAPA) {
    /* Do we have the terminating line? Per RFC 2449 this is a line
       containing only a single dot */
    જો((len == 3 && લીટી[0] == '.' && લીટી[1] == '\r') ||
       (len == 2 && લીટી[0] == '.' && લીટી[1] == '\n'))
      /* Treat the response as a success */
      *resp = '+';
    નહીંતર
      /* Treat the response as an untagged continuation */
      *resp = '*';

    પાછા TRUE;
  }

  /* Do we have a success response? */
  જો(len >= 3 && !memcmp("+OK", લીટી, 3)) {
    *resp = '+';

    પાછા TRUE;
  }

  /* Do we have a continuation response? */
  જો(len >= 1 && લીટી[0] == '+') {
    *resp = '*';

    પાછા TRUE;
  }

  પાછા FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * pop3_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
સ્થિર CURLcode pop3_get_message(સંરચના Curl_easy *data, સંરચના bufref *out)
{
  સંરચના pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  અક્ષર *message;
  size_t len;

  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;
  message = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;
  જો(len > 2) {
    /* Find the start of the message */
    len -= 2;
    માટે(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    જ્યારે(len--)
      જો(!ISBLANK(message[len]) && !ISNEWLINE(message[len]))
        તોડો;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, ખાલી);
  }
  નહીંતર
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, ખાલી);

  પાછા CURLE_OK;
}

/***********************************************************************
 *
 * pop3_state()
 *
 * This is the ONLY way to change POP3 state!
 */
સ્થિર શૂન્ય pop3_state(સંરચના Curl_easy *data, pop3state newstate)
{
  સંરચના pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  જો(pop3c) {
#જો defined(DEBUGBUILD) && defined(CURLVERBOSE)
    /* for debug purposes */
    સ્થિર અચલ અક્ષર * અચલ names[] = {
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

    જો(pop3c->state != newstate)
      infof(data, "POP3 %p state change from %s to %s",
            (શૂન્ય *)pop3c, names[pop3c->state], names[newstate]);
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
સ્થિર CURLcode pop3_perform_capa(સંરચના Curl_easy *data,
                                  સંરચના connectdata *conn)
{
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;

  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  pop3c->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  pop3c->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  pop3c->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPA command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "CAPA");

  જો(!result)
    pop3_state(data, POP3_CAPA);

  પાછા result;
}

/***********************************************************************
 *
 * pop3_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
સ્થિર CURLcode pop3_perform_starttls(સંરચના Curl_easy *data,
                                      સંરચના connectdata *conn)
{
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;

  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  /* Send the STLS command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "STLS");
  જો(!result)
    pop3_state(data, POP3_STARTTLS);

  પાછા result;
}

/***********************************************************************
 *
 * pop3_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
સ્થિર CURLcode pop3_perform_upgrade_tls(સંરચના Curl_easy *data,
                                         સંરચના connectdata *conn)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;
  bool ssldone = FALSE;

  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  જો(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    જો(result)
      જાઓ out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_pop3s;
  }

  DEBUGASSERT(!pop3c->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "pop3_perform_upgrade_tls, connect -> %d, %d",
               (પૂર્ણાંક)result, ssldone));
  જો(!result && ssldone) {
    pop3c->ssldone = ssldone;
    /* perform CAPA now, changes pop3c->state out of POP3_UPGRADETLS */
    result = pop3_perform_capa(data, conn);
  }
out:
  પાછા result;
#નહીંતર
  (શૂન્ય)data;
  (શૂન્ય)conn;
  પાછા CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * pop3_perform_user()
 *
 * Sends a clear text USER command to authenticate with.
 */
સ્થિર CURLcode pop3_perform_user(સંરચના Curl_easy *data,
                                  સંરચના connectdata *conn)
{
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;

  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  જો(!conn->creds) {
    pop3_state(data, POP3_STOP);

    પાછા result;
  }

  /* Send the USER command */
  result = Curl_pp_sendf(data, &pop3c->pp, "USER %s",
                         Curl_creds_user(conn->creds));
  જો(!result)
    pop3_state(data, POP3_USER);

  પાછા result;
}

#ifndef CURL_DISABLE_DIGEST_AUTH
/***********************************************************************
 *
 * pop3_perform_apop()
 *
 * Sends an APOP command to authenticate with.
 */
સ્થિર CURLcode pop3_perform_apop(સંરચના Curl_easy *data,
                                  સંરચના connectdata *conn)
{
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  size_t i;
  સંરચના MD5_context *ctxt;
  unsigned અક્ષર digest[MD5_DIGEST_LEN];
  અક્ષર secret[(2 * MD5_DIGEST_LEN) + 1];

  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  જો(!data->state.creds) {
    pop3_state(data, POP3_STOP);

    પાછા result;
  }

  /* Create the digest */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  જો(!ctxt)
    પાછા CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)pop3c->apoptimestamp,
                  curlx_uztoui(strlen(pop3c->apoptimestamp)));

  Curl_MD5_update(ctxt, (અચલ unsigned અક્ષર *)Curl_creds_passwd(conn->creds),
                  curlx_uztoui(strlen(Curl_creds_passwd(conn->creds))));

  /* Finalise the digest */
  Curl_MD5_final(ctxt, digest);

  /* Convert the calculated 16 octet digest into a 32-byte hex string */
  માટે(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&secret[2 * i], 3, "%02x", digest[i]);

  result = Curl_pp_sendf(data, &pop3c->pp, "APOP %s %s",
                         Curl_creds_user(conn->creds), secret);

  જો(!result)
    pop3_state(data, POP3_APOP);

  પાછા result;
}
#endif

/***********************************************************************
 *
 * pop3_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
સ્થિર CURLcode pop3_perform_auth(સંરચના Curl_easy *data,
                                  અચલ અક્ષર *mech,
                                  અચલ સંરચના bufref *initresp)
{
  સંરચના pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  અચલ અક્ષર *ir = Curl_bufref_ptr(initresp);

  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  જો(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    result = Curl_pp_sendf(data, &pop3c->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  નહીંતર {
    /* Send the AUTH command */
    result = Curl_pp_sendf(data, &pop3c->pp, "AUTH %s", mech);
  }

  પાછા result;
}

/***********************************************************************
 *
 * pop3_continue_auth()
 *
 * Sends SASL continuation data.
 */
સ્થિર CURLcode pop3_continue_auth(સંરચના Curl_easy *data,
                                   અચલ અક્ષર *mech,
                                   અચલ સંરચના bufref *resp)
{
  સંરચના pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);

  (શૂન્ય)mech;
  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  પાછા Curl_pp_sendf(data, &pop3c->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * pop3_cancel_auth()
 *
 * Sends SASL cancellation.
 */
સ્થિર CURLcode pop3_cancel_auth(સંરચના Curl_easy *data, અચલ અક્ષર *mech)
{
  સંરચના pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);

  (શૂન્ય)mech;
  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  પાછા Curl_pp_sendf(data, &pop3c->pp, "*");
}

/***********************************************************************
 *
 * pop3_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to APOP and clear text should a
 * common mechanism not be available between the client and server.
 */
સ્થિર CURLcode pop3_perform_authentication(સંરચના Curl_easy *data,
                                            સંરચના connectdata *conn)
{
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  saslprogress progress = SASL_IDLE;

  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  /* Check we have enough data to authenticate with and end the
     connect phase if we do not */
  જો(!Curl_sasl_can_authenticate(&pop3c->sasl, data)) {
    pop3_state(data, POP3_STOP);
    પાછા result;
  }

  જો(pop3c->authtypes & pop3c->preftype & POP3_TYPE_SASL) {
    /* Calculate the SASL login details */
    result = Curl_sasl_start(&pop3c->sasl, data, FALSE, &progress);

    જો(!result && progress == SASL_INPROGRESS)
      pop3_state(data, POP3_AUTH);
  }

  જો(!result && progress == SASL_IDLE) {
#ifndef CURL_DISABLE_DIGEST_AUTH
    જો(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
      /* Perform APOP authentication */
      result = pop3_perform_apop(data, conn);
    નહીંતર
#endif
    જો(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
      /* Perform clear text authentication */
      result = pop3_perform_user(data, conn);
    નહીંતર
      result = Curl_sasl_is_blocked(&pop3c->sasl, data);
  }

  પાછા result;
}

/***********************************************************************
 *
 * pop3_perform_command()
 *
 * Sends a POP3 based command.
 */
સ્થિર CURLcode pop3_perform_command(સંરચના Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;
  સંરચના POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  અચલ અક્ષર *command = ખાલી;

  જો(!pop3 || !pop3c)
    પાછા CURLE_FAILED_INIT;

  /* Calculate the default command */
  જો(pop3->id[0] == '\0' || data->set.list_only) {
    command = "LIST";

    જો(pop3->id[0] != '\0')
      /* Message specific LIST so skip the BODY transfer */
      pop3->transfer = PPTRANSFER_INFO;
  }
  નહીંતર
    command = "RETR";

  જો(pop3->custom && pop3->custom[0] != '\0')
    command = pop3->custom;

  /* Send the command */
  જો(pop3->id[0] != '\0')
    result = Curl_pp_sendf(data, &pop3c->pp, "%s %s", command, pop3->id);
  નહીંતર
    result = Curl_pp_sendf(data, &pop3c->pp, "%s", command);

  જો(!result) {
    pop3_state(data, POP3_COMMAND);
    data->req.no_body = !pop3_is_multiline(command);
  }

  પાછા result;
}

/***********************************************************************
 *
 * pop3_perform_quit()
 *
 * Performs the quit action prior to sclose() be called.
 */
સ્થિર CURLcode pop3_perform_quit(સંરચના Curl_easy *data,
                                  સંરચના connectdata *conn)
{
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;

  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  /* Send the QUIT command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "QUIT");
  જો(!result)
    pop3_state(data, POP3_QUIT);

  પાછા result;
}

/* For the initial server greeting */
સ્થિર CURLcode pop3_state_servergreet_resp(સંરચના Curl_easy *data,
                                            પૂર્ણાંક pop3code,
                                            pop3state instate)
{
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  અચલ અક્ષર *લીટી;
  size_t len;

  (શૂન્ય)instate;
  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  લીટી = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  જો(pop3code != '+') {
    failf(data, "Got unexpected pop3-server response");
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  નહીંતર જો(len > 3) {
    /* Does the server support APOP authentication? */
    અચલ અક્ષર *lt;
    અચલ અક્ષર *gt = ખાલી;

    /* Look for the APOP timestamp */
    lt = memchr(લીટી, '<', len);
    જો(lt)
      /* search the remainder for '>' */
      gt = memchr(lt, '>', len - (lt - લીટી));
    જો(gt) {
      /* the length of the timestamp, including the brackets */
      size_t timestamplen = gt - lt + 1;
      અચલ અક્ષર *at = memchr(lt, '@', timestamplen);
      /* If the timestamp does not contain '@' it is not (as required by
         RFC-1939) conformant to the RFC-822 message id syntax, and we
         therefore do not use APOP authentication. */
      જો(at) {
        /* dupe the timestamp */
        pop3c->apoptimestamp = curlx_memdup0(lt, timestamplen);
        જો(!pop3c->apoptimestamp)
          પાછા CURLE_OUT_OF_MEMORY;
        /* Store the APOP capability */
        pop3c->authtypes |= POP3_TYPE_APOP;
      }
    }

    જો(!result)
      result = pop3_perform_capa(data, conn);
  }

  પાછા result;
}

/* For CAPA responses */
સ્થિર CURLcode pop3_state_capa_resp(સંરચના Curl_easy *data, પૂર્ણાંક pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  અચલ અક્ષર *લીટી;
  size_t len;

  (શૂન્ય)instate;
  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  લીટી = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  /* Do we have an untagged continuation response? */
  જો(pop3code == '*') {
    /* Does the server support the STLS capability? */
    જો(len >= 4 && curl_strnequal(લીટી, "STLS", 4))
      pop3c->tls_supported = TRUE;

    /* Does the server support clear text authentication? */
    નહીંતર જો(len >= 4 && curl_strnequal(લીટી, "USER", 4))
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    /* Does the server support SASL based authentication? */
    નહીંતર જો(len >= 5 && curl_strnequal(લીટી, "SASL ", 5)) {
      pop3c->authtypes |= POP3_TYPE_SASL;

      /* Advance past the SASL keyword */
      લીટી += 5;
      len -= 5;

      /* Loop through the data line */
      માટે(;;) {
        size_t llen;
        size_t wordlen = 0;
        unsigned short mechbit;

        જ્યારે(len && (ISBLANK(*લીટી) || ISNEWLINE(*લીટી))) {
          લીટી++;
          len--;
        }

        જો(!len)
          તોડો;

        /* Extract the word */
        જ્યારે(wordlen < len && !ISBLANK(લીટી[wordlen]) &&
              !ISNEWLINE(લીટી[wordlen]))
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(લીટી, wordlen, &llen);
        જો(mechbit && llen == wordlen)
          pop3c->sasl.authmechs |= mechbit;

        લીટી += wordlen;
        len -= wordlen;
      }
    }
  }
  નહીંતર {
    /* Clear text is supported when CAPA is not recognised */
    જો(pop3code != '+')
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    જો(!data->set.use_ssl || Curl_conn_is_ssl(conn, FIRSTSOCKET))
      result = pop3_perform_authentication(data, conn);
    નહીંતર જો(pop3code == '+' && pop3c->tls_supported)
      /* Switch to TLS connection now */
      result = pop3_perform_starttls(data, conn);
    નહીંતર જો(data->set.use_ssl <= CURLUSESSL_TRY)
      /* Fallback and carry on with authentication */
      result = pop3_perform_authentication(data, conn);
    નહીંતર {
      failf(data, "STLS not supported.");
      result = CURLE_USE_SSL_FAILED;
    }
  }

  પાછા result;
}

/* For STARTTLS responses */
સ્થિર CURLcode pop3_state_starttls_resp(સંરચના Curl_easy *data,
                                         સંરચના connectdata *conn,
                                         પૂર્ણાંક pop3code,
                                         pop3state instate)
{
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  (શૂન્ય)instate;

  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  /* Pipelining in response is forbidden. */
  જો(pop3c->pp.overflow)
    પાછા CURLE_WEIRD_SERVER_REPLY;

  જો(pop3code != '+') {
    જો(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied");
      result = CURLE_USE_SSL_FAILED;
    }
    નહીંતર
      result = pop3_perform_authentication(data, conn);
  }
  નહીંતર
    pop3_state(data, POP3_UPGRADETLS);

  પાછા result;
}

/* For SASL authentication responses */
સ્થિર CURLcode pop3_state_auth_resp(સંરચના Curl_easy *data,
                                     પૂર્ણાંક pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  saslprogress progress;

  (શૂન્ય)instate;
  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  result = Curl_sasl_continue(&pop3c->sasl, data, pop3code, &progress);
  જો(!result)
    પસંદગી(progress) {
    સ્થિતિ SASL_DONE:
      pop3_state(data, POP3_STOP);  /* Authenticated */
      તોડો;
    સ્થિતિ SASL_IDLE:            /* No mechanism left after cancellation */
#ifndef CURL_DISABLE_DIGEST_AUTH
      જો(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
        /* Perform APOP authentication */
        result = pop3_perform_apop(data, conn);
      નહીંતર
#endif
      જો(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
        /* Perform clear text authentication */
        result = pop3_perform_user(data, conn);
      નહીંતર {
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
      }
      તોડો;
    મૂળભૂત:
      તોડો;
    }

  પાછા result;
}

#ifndef CURL_DISABLE_DIGEST_AUTH
/* For APOP responses */
સ્થિર CURLcode pop3_state_apop_resp(સંરચના Curl_easy *data, પૂર્ણાંક pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  (શૂન્ય)instate;

  જો(pop3code != '+') {
    failf(data, "Authentication failed: %d", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  નહીંતર
    /* End of connect phase */
    pop3_state(data, POP3_STOP);

  પાછા result;
}
#endif

/* For USER responses */
સ્થિર CURLcode pop3_state_user_resp(સંરચના Curl_easy *data, પૂર્ણાંક pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (શૂન્ય)instate;

  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  જો(pop3code != '+') {
    failf(data, "Access denied. %c", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  નહીંતર
    /* Send the PASS command */
    result = Curl_pp_sendf(data, &pop3c->pp, "PASS %s",
                           Curl_creds_passwd(conn->creds));
  જો(!result)
    pop3_state(data, POP3_PASS);

  પાછા result;
}

/* For PASS responses */
સ્થિર CURLcode pop3_state_pass_resp(સંરચના Curl_easy *data, પૂર્ણાંક pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  (શૂન્ય)instate;

  જો(pop3code != '+') {
    failf(data, "Access denied. %c", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  નહીંતર
    /* End of connect phase */
    pop3_state(data, POP3_STOP);

  પાછા result;
}

/***********************************************************************
 *
 * pop3_write()
 *
 * This function scans the body after the end-of-body and writes everything
 * until the end is found.
 */
સ્થિર CURLcode pop3_write(સંરચના Curl_easy *data, અચલ અક્ષર *str,
                           size_t nread, bool is_eos)
{
  /* This code could be made into a special function in the handler struct */
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  bool strip_dot = FALSE;
  size_t last = 0;
  size_t i;
  (શૂન્ય)is_eos;

  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  /* Search through the buffer looking for the end-of-body marker which is
     5 bytes (0d 0a 2e 0d 0a). Note that a line starting with a dot matches
     the eob so the server will have prefixed it with an extra dot which we
     need to strip out. Additionally the marker could of course be spread out
     over 5 different data chunks. */
  માટે(i = 0; i < nread; i++) {
    size_t prev = pop3c->eob;

    પસંદગી(str[i]) {
    સ્થિતિ 0x0d:
      જો(pop3c->eob == 0) {
        pop3c->eob++;

        જો(i) {
          /* Write out the body part that did not match */
          result = Curl_client_write(data, CLIENTWRITE_BODY, &str[last],
                                     i - last);

          જો(result)
            પાછા result;

          last = i;
        }
      }
      નહીંતર જો(pop3c->eob == 3)
        pop3c->eob++;
      નહીંતર
        /* If the character match was not at position 0 or 3 then restart the
           pattern matching */
        pop3c->eob = 1;
      તોડો;

    સ્થિતિ 0x0a:
      જો(pop3c->eob == 1 || pop3c->eob == 4)
        pop3c->eob++;
      નહીંતર
        /* If the character match was not at position 1 or 4 then start the
           search again */
        pop3c->eob = 0;
      તોડો;

    સ્થિતિ 0x2e:
      જો(pop3c->eob == 2)
        pop3c->eob++;
      નહીંતર જો(pop3c->eob == 3) {
        /* We have an extra dot after the CRLF which we need to strip off */
        strip_dot = TRUE;
        pop3c->eob = 0;
      }
      નહીંતર
        /* If the character match was not at position 2 then start the search
           again */
        pop3c->eob = 0;
      તોડો;

    મૂળભૂત:
      pop3c->eob = 0;
      તોડો;
    }

    /* Did we have a partial match which has subsequently failed? */
    જો(prev && prev >= pop3c->eob) {
      /* Strip can only be non-zero for the first mismatch after CRLF and
         then both prev and strip are equal and nothing will be output below */
      જ્યારે(prev && pop3c->strip) {
        prev--;
        pop3c->strip--;
      }

      જો(prev) {
        /* If the partial match was the CRLF and dot then only write the CRLF
           as the server would have inserted the dot */
        જો(strip_dot && prev - 1 > 0) {
          result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB,
                                     prev - 1);
        }
        નહીંતર જો(!strip_dot) {
          result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB,
                                     prev);
        }
        નહીંતર {
          result = CURLE_OK;
        }

        જો(result)
          પાછા result;

        last = i;
        strip_dot = FALSE;
      }
    }
  }

  જો(pop3c->eob == POP3_EOB_LEN) {
    /* We have a full match so the transfer is done, however we must transfer
    the CRLF at the start of the EOB as this is considered to be part of the
    message as per RFC-1939, sect. 3 */
    result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB, 2);

    CURL_REQ_CLEAR_RECV(data);
    pop3c->eob = 0;

    પાછા result;
  }

  જો(pop3c->eob)
    /* While EOB is matching nothing should be output */
    પાછા CURLE_OK;

  જો(nread - last) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, &str[last],
                               nread - last);
  }

  પાછા result;
}

/* For command responses */
સ્થિર CURLcode pop3_state_command_resp(સંરચના Curl_easy *data,
                                        પૂર્ણાંક pop3code,
                                        pop3state instate)
{
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;
  સંરચના POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  સંરચના pingpong *pp;

  (શૂન્ય)instate;
  જો(!pop3 || !pop3c)
    પાછા CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  જો(pop3code != '+') {
    pop3_state(data, POP3_STOP);
    પાછા CURLE_WEIRD_SERVER_REPLY;
  }

  /* This 'OK' line ends with a CR LF pair which is the two first bytes of the
     EOB string so count this is two matching bytes. This is necessary to make
     the code detect the EOB if the only data than comes now is %2e CR LF like
     when there is no body to return. */
  pop3c->eob = 2;

  /* Since this initial CR LF pair is not part of the actual body, we set
     the strip counter here so that these bytes will not be delivered. */
  pop3c->strip = 2;

  જો(pop3->transfer == PPTRANSFER_BODY) {
    /* POP3 download */
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);

    જો(pp->overflow) {
      /* The recv buffer contains data that is actually body content so send
         it as such. Note that there may even be additional "headers" after
         the body */

      /* keep only the overflow */
      curlx_dyn_tail(&pp->recvbuf, pp->overflow);
      pp->nfinal = 0; /* done */

      જો(!data->req.no_body) {
        result = pop3_write(data, curlx_dyn_ptr(&pp->recvbuf),
                            curlx_dyn_len(&pp->recvbuf), FALSE);
        જો(result)
          પાછા result;
      }

      /* reset the buffer */
      curlx_dyn_reset(&pp->recvbuf);
      pp->overflow = 0;
    }
  }
  નહીંતર
    pp->overflow = 0;

  /* End of DO phase */
  pop3_state(data, POP3_STOP);

  પાછા result;
}

સ્થિર CURLcode pop3_statemachine(સંરચના Curl_easy *data,
                                  સંરચના connectdata *conn)
{
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  પૂર્ણાંક pop3code;
  સંરચના pingpong *pp;
  size_t nread = 0;
  (શૂન્ય)data;

  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not POP3 */
upgrade_tls:
  જો(pop3c->state == POP3_UPGRADETLS) {
    result = pop3_perform_upgrade_tls(data, conn);
    જો(result || (pop3c->state == POP3_UPGRADETLS))
      પાછા result;
  }

  /* Flush any data that needs to be sent */
  જો(pp->sendleft)
    પાછા Curl_pp_flushsend(data, pp);

  કરો {
     /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, pp, &pop3code, &nread);
    જો(result)
      પાછા result;

    જો(!pop3code)
      તોડો;

    /* We have now received a full POP3 server response */
    પસંદગી(pop3c->state) {
    સ્થિતિ POP3_SERVERGREET:
      result = pop3_state_servergreet_resp(data, pop3code, pop3c->state);
      તોડો;

    સ્થિતિ POP3_CAPA:
      result = pop3_state_capa_resp(data, pop3code, pop3c->state);
      તોડો;

    સ્થિતિ POP3_STARTTLS:
      result = pop3_state_starttls_resp(data, conn, pop3code, pop3c->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      જો(!result && (pop3c->state == POP3_UPGRADETLS))
        જાઓ upgrade_tls;
      તોડો;

    સ્થિતિ POP3_AUTH:
      result = pop3_state_auth_resp(data, pop3code, pop3c->state);
      તોડો;

#ifndef CURL_DISABLE_DIGEST_AUTH
    સ્થિતિ POP3_APOP:
      result = pop3_state_apop_resp(data, pop3code, pop3c->state);
      તોડો;
#endif

    સ્થિતિ POP3_USER:
      result = pop3_state_user_resp(data, pop3code, pop3c->state);
      તોડો;

    સ્થિતિ POP3_PASS:
      result = pop3_state_pass_resp(data, pop3code, pop3c->state);
      તોડો;

    સ્થિતિ POP3_COMMAND:
      result = pop3_state_command_resp(data, pop3code, pop3c->state);
      તોડો;

    સ્થિતિ POP3_QUIT:
      pop3_state(data, POP3_STOP);
      તોડો;

    મૂળભૂત:
      /* internal error */
      pop3_state(data, POP3_STOP);
      તોડો;
    }
  } જ્યારે(!result && pop3c->state != POP3_STOP && Curl_pp_moredata(pp));

  પાછા result;
}

/* Called repeatedly until done from multi.c */
સ્થિર CURLcode pop3_multi_statemach(સંરચના Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;
  result = Curl_pp_statemach(data, &pop3c->pp, FALSE, FALSE);
  *done = (pop3c->state == POP3_STOP);

  પાછા result;
}

સ્થિર CURLcode pop3_block_statemach(સંરચના Curl_easy *data,
                                     સંરચના connectdata *conn,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  જ્યારે(pop3c->state != POP3_STOP && !result)
    result = Curl_pp_statemach(data, &pop3c->pp, TRUE, disconnecting);

  પાછા result;
}

/* For the POP3 "protocol connect" and "doing" phases only */
સ્થિર CURLcode pop3_pollset(સંરચના Curl_easy *data,
                             સંરચના easy_pollset *ps)
{
  સંરચના pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  પાછા pop3c ? Curl_pp_pollset(data, &pop3c->pp, ps) : CURLE_OK;
}

/* SASL parameters for the pop3 protocol */
સ્થિર અચલ સંરચના SASLproto saslpop3 = {
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
સ્થિર CURLcode pop3_connect(સંરચના Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  સંરચના pingpong *pp = pop3c ? &pop3c->pp : ખાલી;

  *done = FALSE; /* default to not done yet */
  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  PINGPONG_SETUP(pp, pop3_statemachine, pop3_endofresp);

  /* Set the default preferred authentication type and mechanism */
  pop3c->preftype = POP3_TYPE_ANY;
  Curl_sasl_init(&pop3c->sasl, data, &saslpop3);

  /* Initialize the pingpong layer */
  Curl_pp_init(pp, Curl_pgrs_now(data));

  /* Parse the URL options */
  result = pop3_parse_url_options(conn);
  જો(result)
    પાછા result;

  /* Start off waiting for the server greeting response */
  pop3_state(data, POP3_SERVERGREET);

  result = pop3_multi_statemach(data, done);

  પાછા result;
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
સ્થિર CURLcode pop3_done(સંરચના Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  સંરચના POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);

  (શૂન્ય)premature;

  જો(!pop3)
    પાછા CURLE_OK;

  જો(status) {
    CURL_TRC_M(data, "POP3 done with bad status");
    connclose(data->conn);
    result = status;         /* use the already set error code */
  }

  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);

  /* Clear the transfer mode for the next request */
  pop3->transfer = PPTRANSFER_BODY;

  પાછા result;
}

/***********************************************************************
 *
 * pop3_perform()
 *
 * This is the actual DO function for POP3. Get a message/listing according to
 * the options previously setup.
 */
સ્થિર CURLcode pop3_perform(સંરચના Curl_easy *data, bool *connected,
                             bool *dophase_done)
{
  /* This is POP3 and no proxy */
  CURLcode result = CURLE_OK;
  સંરચના POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);

  જો(!pop3)
    પાછા CURLE_FAILED_INIT;

  DEBUGF(infof(data, "DO phase starts"));

  /* Start the first command in the DO phase, may alter data->req.no_body */
  result = pop3_perform_command(data);
  જો(result)
    પાછા result;

  જો(data->req.no_body)
    /* Requested no body means no transfer */
    pop3->transfer = PPTRANSFER_INFO;

  *dophase_done = FALSE; /* not done yet */

  /* Run the state-machine */
  result = pop3_multi_statemach(data, dophase_done);
  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  જો(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  પાછા result;
}

/* Call this when the DO phase has completed */
સ્થિર CURLcode pop3_dophase_done(સંરચના Curl_easy *data, bool connected)
{
  (શૂન્ય)data;
  (શૂન્ય)connected;

  પાછા CURLE_OK;
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
સ્થિર CURLcode pop3_regular_transfer(સંરચના Curl_easy *data,
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
  જો(!result && *dophase_done)
    result = pop3_dophase_done(data, connected);

  પાછા result;
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
સ્થિર CURLcode pop3_do(સંરચના Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  *done = FALSE; /* default to false */

  /* Parse the URL path */
  result = pop3_parse_url_path(data);
  જો(result)
    પાછા result;

  /* Parse the custom request */
  result = pop3_parse_custom_request(data);
  જો(result)
    પાછા result;

  result = pop3_regular_transfer(data, done);

  પાછા result;
}

/***********************************************************************
 *
 * pop3_disconnect()
 *
 * Disconnect from an POP3 server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
સ્થિર CURLcode pop3_disconnect(સંરચના Curl_easy *data,
                                સંરચના connectdata *conn, bool dead_connection)
{
  સંરચના pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (શૂન્ય)data;

  જો(!pop3c)
    પાછા CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  જો(!dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &pop3c->pp) &&
     !pop3_perform_quit(data, conn))
    (શૂન્ય)pop3_block_statemach(data, conn, TRUE); /* ignore errors on QUIT */

  /* Disconnect from the server */
  Curl_pp_disconnect(&pop3c->pp);

  /* Cleanup our connection based variables */
  curlx_safefree(pop3c->apoptimestamp);

  પાછા CURLE_OK;
}

/* Called from multi.c while DOing */
સ્થિર CURLcode pop3_doing(સંરચના Curl_easy *data, bool *dophase_done)
{
  CURLcode result = pop3_multi_statemach(data, dophase_done);

  જો(result)
    DEBUGF(infof(data, "DO phase failed"));
  નહીંતર જો(*dophase_done) {
    result = pop3_dophase_done(data, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  પાછા result;
}

સ્થિર શૂન્ય pop3_easy_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના POP3 *pop3 = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  DEBUGASSERT(pop3);
  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);
  curlx_free(pop3);
}

સ્થિર શૂન્ય pop3_conn_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના pop3_conn *pop3c = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  DEBUGASSERT(pop3c);
  Curl_pp_disconnect(&pop3c->pp);
  curlx_safefree(pop3c->apoptimestamp);
  curlx_free(pop3c);
}

સ્થિર CURLcode pop3_setup_connection(સંરચના Curl_easy *data,
                                      સંરચના connectdata *conn)
{
  સંરચના pop3_conn *pop3c;
  સંરચના POP3 *pop3 = curlx_calloc(1, કદ(*pop3));
  જો(!pop3 ||
     Curl_meta_set(data, CURL_META_POP3_EASY, pop3, pop3_easy_dtor))
    પાછા CURLE_OUT_OF_MEMORY;

  pop3c = curlx_calloc(1, કદ(*pop3c));
  જો(!pop3c ||
     Curl_conn_meta_set(conn, CURL_META_POP3_CONN, pop3c, pop3_conn_dtor))
    પાછા CURLE_OUT_OF_MEMORY;

  પાછા CURLE_OK;
}

/*
 * POP3 protocol.
 */
અચલ સંરચના Curl_protocol Curl_protocol_pop3 = {
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
