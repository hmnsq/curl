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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "pop3.h"

#ifndef CURL_DISABLE_POP3

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#endif
#ifdef __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#endif

#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "pingpong.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "curl_sasl.h"
#ಅನ್ತರ್ಭಾವ "curl_md5.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"

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
ಸಂರಚನಾ POP3 {
  curl_pp_transfer transfer;
  ವರ್ಣ *id;               /* Message ID */
  ವರ್ಣ *custom;           /* Custom Request */
};

/*
 * POP3 connection state
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
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

ಸಂರಚನಾ pop3_conn {
  ಸಂರಚನಾ pingpong pp;
  pop3state state;        /* Always use pop3.c:state() to change state! */
  size_t eob;             /* Number of bytes of the EOB (End Of Body) that
                             have been received so far */
  size_t strip;           /* Number of bytes from the start to ignore as
                             non-body */
  ಸಂರಚನಾ SASL sasl;       /* SASL-related storage */
  ವರ್ಣ *apoptimestamp;    /* APOP timestamp from the server greeting */
  unsigned ವರ್ಣ authtypes; /* Accepted authentication types */
  unsigned ವರ್ಣ preftype;  /* Preferred authentication type */
  BIT(ssldone);           /* Is connect() over SSL done? */
  BIT(tls_supported);     /* StartTLS capability supported by server */
};

ಸಂರಚನಾ pop3_cmd {
  ಧ್ರುವ ವರ್ಣ *name;
  unsigned short nlen;
  BIT(multiline); /* response is multi-line with last '.' line */
  BIT(multiline_with_args); /* is multi-line when command has args */
};

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ pop3_cmd pop3cmds[] = {
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
ಸ್ಥಿರ CURLcode pop3_parse_url_options(ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ವರ್ಣ *ptr = conn->options;

  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯಾವತ್(!result && ptr && *ptr) {
    ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ = ptr;
    ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ;

    ಯಾವತ್(*ptr && *ptr != '=')
      ptr++;

    ಮೌಲ್ಯ = ptr + 1;

    ಯಾವತ್(*ptr && *ptr != ';')
      ptr++;

    ಯದಿ(curl_strnequal(ಕೀಲಿ, "AUTH=", 5)) {
      result = Curl_sasl_parse_url_auth_option(&pop3c->sasl,
                                               ಮೌಲ್ಯ, ptr - ಮೌಲ್ಯ);

      ಯದಿ(result && curl_strnequal(ಮೌಲ್ಯ, "+APOP", ptr - ಮೌಲ್ಯ)) {
        pop3c->preftype = POP3_TYPE_APOP;
        pop3c->sasl.prefmech = SASL_AUTH_NONE;
        result = CURLE_OK;
      }
    }
    ಅನ್ಯಥಾ
      result = CURLE_URL_MALFORMAT;

    ಯದಿ(*ptr == ';')
      ptr++;
  }

  ಯದಿ(pop3c->preftype != POP3_TYPE_APOP)
    ವಿಕಲ್ಪ(pop3c->sasl.prefmech) {
    ಪಕ್ಷ SASL_AUTH_NONE:
      pop3c->preftype = POP3_TYPE_NONE;
      ಭಙ್ಗ;
    ಪಕ್ಷ SASL_AUTH_DEFAULT:
      pop3c->preftype = POP3_TYPE_ANY;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      pop3c->preftype = POP3_TYPE_SASL;
      ಭಙ್ಗ;
    }

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * pop3_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
ಸ್ಥಿರ CURLcode pop3_parse_url_path(ಸಂರಚನಾ Curl_easy *data)
{
  /* The POP3 struct is already initialized in pop3_connect() */
  ಸಂರಚನಾ POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  ಧ್ರುವ ವರ್ಣ *path = &data->state.up.path[1]; /* skip leading path */

  ಯದಿ(!pop3)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  /* URL decode the path for the message ID */
  ಪ್ರತ್ಯಾಗಮ Curl_urldecode(path, 0, &pop3->id, ರಿಕ್ತ, REJECT_CTRL);
}

/***********************************************************************
 *
 * pop3_parse_custom_request()
 *
 * Parse the custom request.
 */
ಸ್ಥಿರ CURLcode pop3_parse_custom_request(ಸಂರಚನಾ Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  ಧ್ರುವ ವರ್ಣ *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  ಯದಿ(!pop3)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  /* URL decode the custom request */
  ಯದಿ(custom)
    result = Curl_urldecode(custom, 0, &pop3->custom, ರಿಕ್ತ, REJECT_CTRL);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* Return iff a command is defined as "multi-line" (RFC 1939),
 * has a response terminated by a last line with a '.'.
 */
ಸ್ಥಿರ bool pop3_is_multiline(ಧ್ರುವ ವರ್ಣ *cmdline)
{
  size_t i;
  ಪ್ರತಿ(i = 0; i < CURL_ARRAYSIZE(pop3cmds); ++i) {
    ಯದಿ(curl_strnequal(pop3cmds[i].name, cmdline, pop3cmds[i].nlen)) {
      ಯದಿ(!cmdline[pop3cmds[i].nlen])
        ಪ್ರತ್ಯಾಗಮ (bool)pop3cmds[i].multiline;
      ಅನ್ಯಥಾ ಯದಿ(cmdline[pop3cmds[i].nlen] == ' ')
        ಪ್ರತ್ಯಾಗಮ (bool)pop3cmds[i].multiline_with_args;
    }
  }
  /* Unknown command, assume multi-line for backward compatibility with
   * earlier curl versions that only could do multi-line responses. */
  ಪ್ರತ್ಯಾಗಮ TRUE;
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
ಸ್ಥಿರ bool pop3_endofresp(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ connectdata *conn,
                           ಧ್ರುವ ವರ್ಣ *ಸಾಲು, size_t len, ಪೂರ್ಣಾಙ್ಕ *resp)
{
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (ಶೂನ್ಯ)data;
  DEBUGASSERT(pop3c);
  ಯದಿ(!pop3c) /* internal error */
    ಪ್ರತ್ಯಾಗಮ TRUE;

  /* Do we have an error response? */
  ಯದಿ(len >= 4 && !memcmp("-ERR", ಸಾಲು, 4)) {
    *resp = '-';

    ಪ್ರತ್ಯಾಗಮ TRUE;
  }

  /* Are we processing CAPA command responses? */
  ಯದಿ(pop3c->state == POP3_CAPA) {
    /* Do we have the terminating line? Per RFC 2449 this is a line
       containing only a single dot */
    ಯದಿ((len == 3 && ಸಾಲು[0] == '.' && ಸಾಲು[1] == '\r') ||
       (len == 2 && ಸಾಲು[0] == '.' && ಸಾಲು[1] == '\n'))
      /* Treat the response as a success */
      *resp = '+';
    ಅನ್ಯಥಾ
      /* Treat the response as an untagged continuation */
      *resp = '*';

    ಪ್ರತ್ಯಾಗಮ TRUE;
  }

  /* Do we have a success response? */
  ಯದಿ(len >= 3 && !memcmp("+OK", ಸಾಲು, 3)) {
    *resp = '+';

    ಪ್ರತ್ಯಾಗಮ TRUE;
  }

  /* Do we have a continuation response? */
  ಯದಿ(len >= 1 && ಸಾಲು[0] == '+') {
    *resp = '*';

    ಪ್ರತ್ಯಾಗಮ TRUE;
  }

  ಪ್ರತ್ಯಾಗಮ FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * pop3_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ಸ್ಥಿರ CURLcode pop3_get_message(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ bufref *out)
{
  ಸಂರಚನಾ pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  ವರ್ಣ *message;
  size_t len;

  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  message = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;
  ಯದಿ(len > 2) {
    /* Find the start of the message */
    len -= 2;
    ಪ್ರತಿ(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    ಯಾವತ್(len--)
      ಯದಿ(!ISBLANK(message[len]) && !ISNEWLINE(message[len]))
        ಭಙ್ಗ;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, ರಿಕ್ತ);
  }
  ಅನ್ಯಥಾ
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, ರಿಕ್ತ);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/***********************************************************************
 *
 * pop3_state()
 *
 * This is the ONLY way to change POP3 state!
 */
ಸ್ಥಿರ ಶೂನ್ಯ pop3_state(ಸಂರಚನಾ Curl_easy *data, pop3state newstate)
{
  ಸಂರಚನಾ pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  ಯದಿ(pop3c) {
#ಯದಿ defined(DEBUGBUILD) && defined(CURLVERBOSE)
    /* for debug purposes */
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ names[] = {
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

    ಯದಿ(pop3c->state != newstate)
      infof(data, "POP3 %p state change from %s to %s",
            (ಶೂನ್ಯ *)pop3c, names[pop3c->state], names[newstate]);
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
ಸ್ಥಿರ CURLcode pop3_perform_capa(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;

  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  pop3c->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  pop3c->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  pop3c->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPA command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "CAPA");

  ಯದಿ(!result)
    pop3_state(data, POP3_CAPA);

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * pop3_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
ಸ್ಥಿರ CURLcode pop3_perform_starttls(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;

  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* Send the STLS command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "STLS");
  ಯದಿ(!result)
    pop3_state(data, POP3_STARTTLS);

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * pop3_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ಸ್ಥಿರ CURLcode pop3_perform_upgrade_tls(ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ connectdata *conn)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;
  bool ssldone = FALSE;

  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯದಿ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_pop3s;
  }

  DEBUGASSERT(!pop3c->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "pop3_perform_upgrade_tls, connect -> %d, %d",
               (ಪೂರ್ಣಾಙ್ಕ)result, ssldone));
  ಯದಿ(!result && ssldone) {
    pop3c->ssldone = ssldone;
    /* perform CAPA now, changes pop3c->state out of POP3_UPGRADETLS */
    result = pop3_perform_capa(data, conn);
  }
out:
  ಪ್ರತ್ಯಾಗಮ result;
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)conn;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * pop3_perform_user()
 *
 * Sends a clear text USER command to authenticate with.
 */
ಸ್ಥಿರ CURLcode pop3_perform_user(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;

  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  ಯದಿ(!conn->creds) {
    pop3_state(data, POP3_STOP);

    ಪ್ರತ್ಯಾಗಮ result;
  }

  /* Send the USER command */
  result = Curl_pp_sendf(data, &pop3c->pp, "USER %s",
                         Curl_creds_user(conn->creds));
  ಯದಿ(!result)
    pop3_state(data, POP3_USER);

  ಪ್ರತ್ಯಾಗಮ result;
}

#ifndef CURL_DISABLE_DIGEST_AUTH
/***********************************************************************
 *
 * pop3_perform_apop()
 *
 * Sends an APOP command to authenticate with.
 */
ಸ್ಥಿರ CURLcode pop3_perform_apop(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  size_t i;
  ಸಂರಚನಾ MD5_context *ctxt;
  unsigned ವರ್ಣ digest[MD5_DIGEST_LEN];
  ವರ್ಣ secret[(2 * MD5_DIGEST_LEN) + 1];

  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  ಯದಿ(!data->state.creds) {
    pop3_state(data, POP3_STOP);

    ಪ್ರತ್ಯಾಗಮ result;
  }

  /* Create the digest */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ಯದಿ(!ctxt)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)pop3c->apoptimestamp,
                  curlx_uztoui(strlen(pop3c->apoptimestamp)));

  Curl_MD5_update(ctxt, (ಧ್ರುವ unsigned ವರ್ಣ *)Curl_creds_passwd(conn->creds),
                  curlx_uztoui(strlen(Curl_creds_passwd(conn->creds))));

  /* Finalise the digest */
  Curl_MD5_final(ctxt, digest);

  /* Convert the calculated 16 octet digest into a 32-byte hex string */
  ಪ್ರತಿ(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&secret[2 * i], 3, "%02x", digest[i]);

  result = Curl_pp_sendf(data, &pop3c->pp, "APOP %s %s",
                         Curl_creds_user(conn->creds), secret);

  ಯದಿ(!result)
    pop3_state(data, POP3_APOP);

  ಪ್ರತ್ಯಾಗಮ result;
}
#endif

/***********************************************************************
 *
 * pop3_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
ಸ್ಥಿರ CURLcode pop3_perform_auth(ಸಂರಚನಾ Curl_easy *data,
                                  ಧ್ರುವ ವರ್ಣ *mech,
                                  ಧ್ರುವ ಸಂರಚನಾ bufref *initresp)
{
  ಸಂರಚನಾ pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ವರ್ಣ *ir = Curl_bufref_ptr(initresp);

  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯದಿ(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    result = Curl_pp_sendf(data, &pop3c->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  ಅನ್ಯಥಾ {
    /* Send the AUTH command */
    result = Curl_pp_sendf(data, &pop3c->pp, "AUTH %s", mech);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * pop3_continue_auth()
 *
 * Sends SASL continuation data.
 */
ಸ್ಥಿರ CURLcode pop3_continue_auth(ಸಂರಚನಾ Curl_easy *data,
                                   ಧ್ರುವ ವರ್ಣ *mech,
                                   ಧ್ರುವ ಸಂರಚನಾ bufref *resp)
{
  ಸಂರಚನಾ pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);

  (ಶೂನ್ಯ)mech;
  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಪ್ರತ್ಯಾಗಮ Curl_pp_sendf(data, &pop3c->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * pop3_cancel_auth()
 *
 * Sends SASL cancellation.
 */
ಸ್ಥಿರ CURLcode pop3_cancel_auth(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *mech)
{
  ಸಂರಚನಾ pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);

  (ಶೂನ್ಯ)mech;
  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಪ್ರತ್ಯಾಗಮ Curl_pp_sendf(data, &pop3c->pp, "*");
}

/***********************************************************************
 *
 * pop3_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to APOP and clear text should a
 * common mechanism not be available between the client and server.
 */
ಸ್ಥಿರ CURLcode pop3_perform_authentication(ಸಂರಚನಾ Curl_easy *data,
                                            ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  saslprogress progress = SASL_IDLE;

  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* Check we have enough data to authenticate with and end the
     connect phase if we do not */
  ಯದಿ(!Curl_sasl_can_authenticate(&pop3c->sasl, data)) {
    pop3_state(data, POP3_STOP);
    ಪ್ರತ್ಯಾಗಮ result;
  }

  ಯದಿ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_SASL) {
    /* Calculate the SASL login details */
    result = Curl_sasl_start(&pop3c->sasl, data, FALSE, &progress);

    ಯದಿ(!result && progress == SASL_INPROGRESS)
      pop3_state(data, POP3_AUTH);
  }

  ಯದಿ(!result && progress == SASL_IDLE) {
#ifndef CURL_DISABLE_DIGEST_AUTH
    ಯದಿ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
      /* Perform APOP authentication */
      result = pop3_perform_apop(data, conn);
    ಅನ್ಯಥಾ
#endif
    ಯದಿ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
      /* Perform clear text authentication */
      result = pop3_perform_user(data, conn);
    ಅನ್ಯಥಾ
      result = Curl_sasl_is_blocked(&pop3c->sasl, data);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * pop3_perform_command()
 *
 * Sends a POP3 based command.
 */
ಸ್ಥಿರ CURLcode pop3_perform_command(ಸಂರಚನಾ Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ಧ್ರುವ ವರ್ಣ *command = ರಿಕ್ತ;

  ಯದಿ(!pop3 || !pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* Calculate the default command */
  ಯದಿ(pop3->id[0] == '\0' || data->set.list_only) {
    command = "LIST";

    ಯದಿ(pop3->id[0] != '\0')
      /* Message specific LIST so skip the BODY transfer */
      pop3->transfer = PPTRANSFER_INFO;
  }
  ಅನ್ಯಥಾ
    command = "RETR";

  ಯದಿ(pop3->custom && pop3->custom[0] != '\0')
    command = pop3->custom;

  /* Send the command */
  ಯದಿ(pop3->id[0] != '\0')
    result = Curl_pp_sendf(data, &pop3c->pp, "%s %s", command, pop3->id);
  ಅನ್ಯಥಾ
    result = Curl_pp_sendf(data, &pop3c->pp, "%s", command);

  ಯದಿ(!result) {
    pop3_state(data, POP3_COMMAND);
    data->req.no_body = !pop3_is_multiline(command);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * pop3_perform_quit()
 *
 * Performs the quit action prior to sclose() be called.
 */
ಸ್ಥಿರ CURLcode pop3_perform_quit(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;

  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* Send the QUIT command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "QUIT");
  ಯದಿ(!result)
    pop3_state(data, POP3_QUIT);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For the initial server greeting */
ಸ್ಥಿರ CURLcode pop3_state_servergreet_resp(ಸಂರಚನಾ Curl_easy *data,
                                            ಪೂರ್ಣಾಙ್ಕ pop3code,
                                            pop3state instate)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ಧ್ರುವ ವರ್ಣ *ಸಾಲು;
  size_t len;

  (ಶೂನ್ಯ)instate;
  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಸಾಲು = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  ಯದಿ(pop3code != '+') {
    failf(data, "Got unexpected pop3-server response");
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  ಅನ್ಯಥಾ ಯದಿ(len > 3) {
    /* Does the server support APOP authentication? */
    ಧ್ರುವ ವರ್ಣ *lt;
    ಧ್ರುವ ವರ್ಣ *gt = ರಿಕ್ತ;

    /* Look for the APOP timestamp */
    lt = memchr(ಸಾಲು, '<', len);
    ಯದಿ(lt)
      /* search the remainder for '>' */
      gt = memchr(lt, '>', len - (lt - ಸಾಲು));
    ಯದಿ(gt) {
      /* the length of the timestamp, including the brackets */
      size_t timestamplen = gt - lt + 1;
      ಧ್ರುವ ವರ್ಣ *at = memchr(lt, '@', timestamplen);
      /* If the timestamp does not contain '@' it is not (as required by
         RFC-1939) conformant to the RFC-822 message id syntax, and we
         therefore do not use APOP authentication. */
      ಯದಿ(at) {
        /* dupe the timestamp */
        pop3c->apoptimestamp = curlx_memdup0(lt, timestamplen);
        ಯದಿ(!pop3c->apoptimestamp)
          ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
        /* Store the APOP capability */
        pop3c->authtypes |= POP3_TYPE_APOP;
      }
    }

    ಯದಿ(!result)
      result = pop3_perform_capa(data, conn);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For CAPA responses */
ಸ್ಥಿರ CURLcode pop3_state_capa_resp(ಸಂರಚನಾ Curl_easy *data, ಪೂರ್ಣಾಙ್ಕ pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ಧ್ರುವ ವರ್ಣ *ಸಾಲು;
  size_t len;

  (ಶೂನ್ಯ)instate;
  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಸಾಲು = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  /* Do we have an untagged continuation response? */
  ಯದಿ(pop3code == '*') {
    /* Does the server support the STLS capability? */
    ಯದಿ(len >= 4 && curl_strnequal(ಸಾಲು, "STLS", 4))
      pop3c->tls_supported = TRUE;

    /* Does the server support clear text authentication? */
    ಅನ್ಯಥಾ ಯದಿ(len >= 4 && curl_strnequal(ಸಾಲು, "USER", 4))
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    /* Does the server support SASL based authentication? */
    ಅನ್ಯಥಾ ಯದಿ(len >= 5 && curl_strnequal(ಸಾಲು, "SASL ", 5)) {
      pop3c->authtypes |= POP3_TYPE_SASL;

      /* Advance past the SASL keyword */
      ಸಾಲು += 5;
      len -= 5;

      /* Loop through the data line */
      ಪ್ರತಿ(;;) {
        size_t llen;
        size_t wordlen = 0;
        unsigned short mechbit;

        ಯಾವತ್(len && (ISBLANK(*ಸಾಲು) || ISNEWLINE(*ಸಾಲು))) {
          ಸಾಲು++;
          len--;
        }

        ಯದಿ(!len)
          ಭಙ್ಗ;

        /* Extract the word */
        ಯಾವತ್(wordlen < len && !ISBLANK(ಸಾಲು[wordlen]) &&
              !ISNEWLINE(ಸಾಲು[wordlen]))
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(ಸಾಲು, wordlen, &llen);
        ಯದಿ(mechbit && llen == wordlen)
          pop3c->sasl.authmechs |= mechbit;

        ಸಾಲು += wordlen;
        len -= wordlen;
      }
    }
  }
  ಅನ್ಯಥಾ {
    /* Clear text is supported when CAPA is not recognised */
    ಯದಿ(pop3code != '+')
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    ಯದಿ(!data->set.use_ssl || Curl_conn_is_ssl(conn, FIRSTSOCKET))
      result = pop3_perform_authentication(data, conn);
    ಅನ್ಯಥಾ ಯದಿ(pop3code == '+' && pop3c->tls_supported)
      /* Switch to TLS connection now */
      result = pop3_perform_starttls(data, conn);
    ಅನ್ಯಥಾ ಯದಿ(data->set.use_ssl <= CURLUSESSL_TRY)
      /* Fallback and carry on with authentication */
      result = pop3_perform_authentication(data, conn);
    ಅನ್ಯಥಾ {
      failf(data, "STLS not supported.");
      result = CURLE_USE_SSL_FAILED;
    }
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For STARTTLS responses */
ಸ್ಥಿರ CURLcode pop3_state_starttls_resp(ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ connectdata *conn,
                                         ಪೂರ್ಣಾಙ್ಕ pop3code,
                                         pop3state instate)
{
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* Pipelining in response is forbidden. */
  ಯದಿ(pop3c->pp.overflow)
    ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;

  ಯದಿ(pop3code != '+') {
    ಯದಿ(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied");
      result = CURLE_USE_SSL_FAILED;
    }
    ಅನ್ಯಥಾ
      result = pop3_perform_authentication(data, conn);
  }
  ಅನ್ಯಥಾ
    pop3_state(data, POP3_UPGRADETLS);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For SASL authentication responses */
ಸ್ಥಿರ CURLcode pop3_state_auth_resp(ಸಂರಚನಾ Curl_easy *data,
                                     ಪೂರ್ಣಾಙ್ಕ pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  saslprogress progress;

  (ಶೂನ್ಯ)instate;
  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  result = Curl_sasl_continue(&pop3c->sasl, data, pop3code, &progress);
  ಯದಿ(!result)
    ವಿಕಲ್ಪ(progress) {
    ಪಕ್ಷ SASL_DONE:
      pop3_state(data, POP3_STOP);  /* Authenticated */
      ಭಙ್ಗ;
    ಪಕ್ಷ SASL_IDLE:            /* No mechanism left after cancellation */
#ifndef CURL_DISABLE_DIGEST_AUTH
      ಯದಿ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
        /* Perform APOP authentication */
        result = pop3_perform_apop(data, conn);
      ಅನ್ಯಥಾ
#endif
      ಯದಿ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
        /* Perform clear text authentication */
        result = pop3_perform_user(data, conn);
      ಅನ್ಯಥಾ {
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
      }
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      ಭಙ್ಗ;
    }

  ಪ್ರತ್ಯಾಗಮ result;
}

#ifndef CURL_DISABLE_DIGEST_AUTH
/* For APOP responses */
ಸ್ಥಿರ CURLcode pop3_state_apop_resp(ಸಂರಚನಾ Curl_easy *data, ಪೂರ್ಣಾಙ್ಕ pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  ಯದಿ(pop3code != '+') {
    failf(data, "Authentication failed: %d", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  ಅನ್ಯಥಾ
    /* End of connect phase */
    pop3_state(data, POP3_STOP);

  ಪ್ರತ್ಯಾಗಮ result;
}
#endif

/* For USER responses */
ಸ್ಥಿರ CURLcode pop3_state_user_resp(ಸಂರಚನಾ Curl_easy *data, ಪೂರ್ಣಾಙ್ಕ pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (ಶೂನ್ಯ)instate;

  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯದಿ(pop3code != '+') {
    failf(data, "Access denied. %c", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  ಅನ್ಯಥಾ
    /* Send the PASS command */
    result = Curl_pp_sendf(data, &pop3c->pp, "PASS %s",
                           Curl_creds_passwd(conn->creds));
  ಯದಿ(!result)
    pop3_state(data, POP3_PASS);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For PASS responses */
ಸ್ಥಿರ CURLcode pop3_state_pass_resp(ಸಂರಚನಾ Curl_easy *data, ಪೂರ್ಣಾಙ್ಕ pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  ಯದಿ(pop3code != '+') {
    failf(data, "Access denied. %c", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  ಅನ್ಯಥಾ
    /* End of connect phase */
    pop3_state(data, POP3_STOP);

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * pop3_write()
 *
 * This function scans the body after the end-of-body and writes everything
 * until the end is found.
 */
ಸ್ಥಿರ CURLcode pop3_write(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *str,
                           size_t nread, bool is_eos)
{
  /* This code could be made into a special function in the handler struct */
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  bool strip_dot = FALSE;
  size_t last = 0;
  size_t i;
  (ಶೂನ್ಯ)is_eos;

  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* Search through the buffer looking for the end-of-body marker which is
     5 bytes (0d 0a 2e 0d 0a). Note that a line starting with a dot matches
     the eob so the server will have prefixed it with an extra dot which we
     need to strip out. Additionally the marker could of course be spread out
     over 5 different data chunks. */
  ಪ್ರತಿ(i = 0; i < nread; i++) {
    size_t prev = pop3c->eob;

    ವಿಕಲ್ಪ(str[i]) {
    ಪಕ್ಷ 0x0d:
      ಯದಿ(pop3c->eob == 0) {
        pop3c->eob++;

        ಯದಿ(i) {
          /* Write out the body part that did not match */
          result = Curl_client_write(data, CLIENTWRITE_BODY, &str[last],
                                     i - last);

          ಯದಿ(result)
            ಪ್ರತ್ಯಾಗಮ result;

          last = i;
        }
      }
      ಅನ್ಯಥಾ ಯದಿ(pop3c->eob == 3)
        pop3c->eob++;
      ಅನ್ಯಥಾ
        /* If the character match was not at position 0 or 3 then restart the
           pattern matching */
        pop3c->eob = 1;
      ಭಙ್ಗ;

    ಪಕ್ಷ 0x0a:
      ಯದಿ(pop3c->eob == 1 || pop3c->eob == 4)
        pop3c->eob++;
      ಅನ್ಯಥಾ
        /* If the character match was not at position 1 or 4 then start the
           search again */
        pop3c->eob = 0;
      ಭಙ್ಗ;

    ಪಕ್ಷ 0x2e:
      ಯದಿ(pop3c->eob == 2)
        pop3c->eob++;
      ಅನ್ಯಥಾ ಯದಿ(pop3c->eob == 3) {
        /* We have an extra dot after the CRLF which we need to strip off */
        strip_dot = TRUE;
        pop3c->eob = 0;
      }
      ಅನ್ಯಥಾ
        /* If the character match was not at position 2 then start the search
           again */
        pop3c->eob = 0;
      ಭಙ್ಗ;

    ಸ್ವಭಾವ:
      pop3c->eob = 0;
      ಭಙ್ಗ;
    }

    /* Did we have a partial match which has subsequently failed? */
    ಯದಿ(prev && prev >= pop3c->eob) {
      /* Strip can only be non-zero for the first mismatch after CRLF and
         then both prev and strip are equal and nothing will be output below */
      ಯಾವತ್(prev && pop3c->strip) {
        prev--;
        pop3c->strip--;
      }

      ಯದಿ(prev) {
        /* If the partial match was the CRLF and dot then only write the CRLF
           as the server would have inserted the dot */
        ಯದಿ(strip_dot && prev - 1 > 0) {
          result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB,
                                     prev - 1);
        }
        ಅನ್ಯಥಾ ಯದಿ(!strip_dot) {
          result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB,
                                     prev);
        }
        ಅನ್ಯಥಾ {
          result = CURLE_OK;
        }

        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;

        last = i;
        strip_dot = FALSE;
      }
    }
  }

  ಯದಿ(pop3c->eob == POP3_EOB_LEN) {
    /* We have a full match so the transfer is done, however we must transfer
    the CRLF at the start of the EOB as this is considered to be part of the
    message as per RFC-1939, sect. 3 */
    result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB, 2);

    CURL_REQ_CLEAR_RECV(data);
    pop3c->eob = 0;

    ಪ್ರತ್ಯಾಗಮ result;
  }

  ಯದಿ(pop3c->eob)
    /* While EOB is matching nothing should be output */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಯದಿ(nread - last) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, &str[last],
                               nread - last);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For command responses */
ಸ್ಥಿರ CURLcode pop3_state_command_resp(ಸಂರಚನಾ Curl_easy *data,
                                        ಪೂರ್ಣಾಙ್ಕ pop3code,
                                        pop3state instate)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ಸಂರಚನಾ pingpong *pp;

  (ಶೂನ್ಯ)instate;
  ಯದಿ(!pop3 || !pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  ಯದಿ(pop3code != '+') {
    pop3_state(data, POP3_STOP);
    ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
  }

  /* This 'OK' line ends with a CR LF pair which is the two first bytes of the
     EOB string so count this is two matching bytes. This is necessary to make
     the code detect the EOB if the only data than comes now is %2e CR LF like
     when there is no body to return. */
  pop3c->eob = 2;

  /* Since this initial CR LF pair is not part of the actual body, we set
     the strip counter here so that these bytes will not be delivered. */
  pop3c->strip = 2;

  ಯದಿ(pop3->transfer == PPTRANSFER_BODY) {
    /* POP3 download */
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);

    ಯದಿ(pp->overflow) {
      /* The recv buffer contains data that is actually body content so send
         it as such. Note that there may even be additional "headers" after
         the body */

      /* keep only the overflow */
      curlx_dyn_tail(&pp->recvbuf, pp->overflow);
      pp->nfinal = 0; /* done */

      ಯದಿ(!data->req.no_body) {
        result = pop3_write(data, curlx_dyn_ptr(&pp->recvbuf),
                            curlx_dyn_len(&pp->recvbuf), FALSE);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;
      }

      /* reset the buffer */
      curlx_dyn_reset(&pp->recvbuf);
      pp->overflow = 0;
    }
  }
  ಅನ್ಯಥಾ
    pp->overflow = 0;

  /* End of DO phase */
  pop3_state(data, POP3_STOP);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode pop3_statemachine(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ pop3code;
  ಸಂರಚನಾ pingpong *pp;
  size_t nread = 0;
  (ಶೂನ್ಯ)data;

  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not POP3 */
upgrade_tls:
  ಯದಿ(pop3c->state == POP3_UPGRADETLS) {
    result = pop3_perform_upgrade_tls(data, conn);
    ಯದಿ(result || (pop3c->state == POP3_UPGRADETLS))
      ಪ್ರತ್ಯಾಗಮ result;
  }

  /* Flush any data that needs to be sent */
  ಯದಿ(pp->sendleft)
    ಪ್ರತ್ಯಾಗಮ Curl_pp_flushsend(data, pp);

  ಕುರು {
     /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, pp, &pop3code, &nread);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    ಯದಿ(!pop3code)
      ಭಙ್ಗ;

    /* We have now received a full POP3 server response */
    ವಿಕಲ್ಪ(pop3c->state) {
    ಪಕ್ಷ POP3_SERVERGREET:
      result = pop3_state_servergreet_resp(data, pop3code, pop3c->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ POP3_CAPA:
      result = pop3_state_capa_resp(data, pop3code, pop3c->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ POP3_STARTTLS:
      result = pop3_state_starttls_resp(data, conn, pop3code, pop3c->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      ಯದಿ(!result && (pop3c->state == POP3_UPGRADETLS))
        ಗಚ್ಛ upgrade_tls;
      ಭಙ್ಗ;

    ಪಕ್ಷ POP3_AUTH:
      result = pop3_state_auth_resp(data, pop3code, pop3c->state);
      ಭಙ್ಗ;

#ifndef CURL_DISABLE_DIGEST_AUTH
    ಪಕ್ಷ POP3_APOP:
      result = pop3_state_apop_resp(data, pop3code, pop3c->state);
      ಭಙ್ಗ;
#endif

    ಪಕ್ಷ POP3_USER:
      result = pop3_state_user_resp(data, pop3code, pop3c->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ POP3_PASS:
      result = pop3_state_pass_resp(data, pop3code, pop3c->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ POP3_COMMAND:
      result = pop3_state_command_resp(data, pop3code, pop3c->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ POP3_QUIT:
      pop3_state(data, POP3_STOP);
      ಭಙ್ಗ;

    ಸ್ವಭಾವ:
      /* internal error */
      pop3_state(data, POP3_STOP);
      ಭಙ್ಗ;
    }
  } ಯಾವತ್(!result && pop3c->state != POP3_STOP && Curl_pp_moredata(pp));

  ಪ್ರತ್ಯಾಗಮ result;
}

/* Called repeatedly until done from multi.c */
ಸ್ಥಿರ CURLcode pop3_multi_statemach(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  result = Curl_pp_statemach(data, &pop3c->pp, FALSE, FALSE);
  *done = (pop3c->state == POP3_STOP);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode pop3_block_statemach(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ connectdata *conn,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯಾವತ್(pop3c->state != POP3_STOP && !result)
    result = Curl_pp_statemach(data, &pop3c->pp, TRUE, disconnecting);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For the POP3 "protocol connect" and "doing" phases only */
ಸ್ಥಿರ CURLcode pop3_pollset(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  ಪ್ರತ್ಯಾಗಮ pop3c ? Curl_pp_pollset(data, &pop3c->pp, ps) : CURLE_OK;
}

/* SASL parameters for the pop3 protocol */
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ SASLproto saslpop3 = {
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
ಸ್ಥಿರ CURLcode pop3_connect(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ಸಂರಚನಾ pingpong *pp = pop3c ? &pop3c->pp : ರಿಕ್ತ;

  *done = FALSE; /* default to not done yet */
  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  PINGPONG_SETUP(pp, pop3_statemachine, pop3_endofresp);

  /* Set the default preferred authentication type and mechanism */
  pop3c->preftype = POP3_TYPE_ANY;
  Curl_sasl_init(&pop3c->sasl, data, &saslpop3);

  /* Initialize the pingpong layer */
  Curl_pp_init(pp, Curl_pgrs_now(data));

  /* Parse the URL options */
  result = pop3_parse_url_options(conn);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* Start off waiting for the server greeting response */
  pop3_state(data, POP3_SERVERGREET);

  result = pop3_multi_statemach(data, done);

  ಪ್ರತ್ಯಾಗಮ result;
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
ಸ್ಥಿರ CURLcode pop3_done(ಸಂರಚನಾ Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);

  (ಶೂನ್ಯ)premature;

  ಯದಿ(!pop3)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಯದಿ(status) {
    CURL_TRC_M(data, "POP3 done with bad status");
    connclose(data->conn);
    result = status;         /* use the already set error code */
  }

  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);

  /* Clear the transfer mode for the next request */
  pop3->transfer = PPTRANSFER_BODY;

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * pop3_perform()
 *
 * This is the actual DO function for POP3. Get a message/listing according to
 * the options previously setup.
 */
ಸ್ಥಿರ CURLcode pop3_perform(ಸಂರಚನಾ Curl_easy *data, bool *connected,
                             bool *dophase_done)
{
  /* This is POP3 and no proxy */
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);

  ಯದಿ(!pop3)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  DEBUGF(infof(data, "DO phase starts"));

  /* Start the first command in the DO phase, may alter data->req.no_body */
  result = pop3_perform_command(data);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  ಯದಿ(data->req.no_body)
    /* Requested no body means no transfer */
    pop3->transfer = PPTRANSFER_INFO;

  *dophase_done = FALSE; /* not done yet */

  /* Run the state-machine */
  result = pop3_multi_statemach(data, dophase_done);
  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  ಯದಿ(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  ಪ್ರತ್ಯಾಗಮ result;
}

/* Call this when the DO phase has completed */
ಸ್ಥಿರ CURLcode pop3_dophase_done(ಸಂರಚನಾ Curl_easy *data, bool connected)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)connected;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
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
ಸ್ಥಿರ CURLcode pop3_regular_transfer(ಸಂರಚನಾ Curl_easy *data,
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
  ಯದಿ(!result && *dophase_done)
    result = pop3_dophase_done(data, connected);

  ಪ್ರತ್ಯಾಗಮ result;
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
ಸ್ಥಿರ CURLcode pop3_do(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  *done = FALSE; /* default to false */

  /* Parse the URL path */
  result = pop3_parse_url_path(data);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* Parse the custom request */
  result = pop3_parse_custom_request(data);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  result = pop3_regular_transfer(data, done);

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * pop3_disconnect()
 *
 * Disconnect from an POP3 server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ಸ್ಥಿರ CURLcode pop3_disconnect(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ connectdata *conn, bool dead_connection)
{
  ಸಂರಚನಾ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (ಶೂನ್ಯ)data;

  ಯದಿ(!pop3c)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  ಯದಿ(!dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &pop3c->pp) &&
     !pop3_perform_quit(data, conn))
    (ಶೂನ್ಯ)pop3_block_statemach(data, conn, TRUE); /* ignore errors on QUIT */

  /* Disconnect from the server */
  Curl_pp_disconnect(&pop3c->pp);

  /* Cleanup our connection based variables */
  curlx_safefree(pop3c->apoptimestamp);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Called from multi.c while DOing */
ಸ್ಥಿರ CURLcode pop3_doing(ಸಂರಚನಾ Curl_easy *data, bool *dophase_done)
{
  CURLcode result = pop3_multi_statemach(data, dophase_done);

  ಯದಿ(result)
    DEBUGF(infof(data, "DO phase failed"));
  ಅನ್ಯಥಾ ಯದಿ(*dophase_done) {
    result = pop3_dophase_done(data, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ pop3_easy_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ POP3 *pop3 = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  DEBUGASSERT(pop3);
  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);
  curlx_free(pop3);
}

ಸ್ಥಿರ ಶೂನ್ಯ pop3_conn_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ pop3_conn *pop3c = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  DEBUGASSERT(pop3c);
  Curl_pp_disconnect(&pop3c->pp);
  curlx_safefree(pop3c->apoptimestamp);
  curlx_free(pop3c);
}

ಸ್ಥಿರ CURLcode pop3_setup_connection(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ pop3_conn *pop3c;
  ಸಂರಚನಾ POP3 *pop3 = curlx_calloc(1, ಪರಿಮಾಣ(*pop3));
  ಯದಿ(!pop3 ||
     Curl_meta_set(data, CURL_META_POP3_EASY, pop3, pop3_easy_dtor))
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  pop3c = curlx_calloc(1, ಪರಿಮಾಣ(*pop3c));
  ಯದಿ(!pop3c ||
     Curl_conn_meta_set(conn, CURL_META_POP3_CONN, pop3c, pop3_conn_dtor))
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * POP3 protocol.
 */
ಧ್ರುವ ಸಂರಚನಾ Curl_protocol Curl_protocol_pop3 = {
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
