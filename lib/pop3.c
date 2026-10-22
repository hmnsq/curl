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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_POP3

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#ಆದರೆ_ಅಂತ್ಯ

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
#ವ್ಯಾಖ್ಯಾನಿಸು POP3_TYPE_CLEARTEXT (1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು POP3_TYPE_APOP      (1 << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು POP3_TYPE_SASL      (1 << 2)

/* Authentication type values */
#ವ್ಯಾಖ್ಯಾನಿಸು POP3_TYPE_NONE 0
#ವ್ಯಾಖ್ಯಾನಿಸು POP3_TYPE_ANY  (POP3_TYPE_CLEARTEXT | POP3_TYPE_APOP | POP3_TYPE_SASL)

/* This is the 5-bytes End-Of-Body marker for POP3 */
#ವ್ಯಾಖ್ಯಾನಿಸು POP3_EOB     "\x0d\x0a\x2e\x0d\x0a"
#ವ್ಯಾಖ್ಯಾನಿಸು POP3_EOB_LEN 5

/* meta key for storing protocol meta at easy handle */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_POP3_EASY   "meta:proto:pop3:easy"
/* meta key for storing protocol meta at connection */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_POP3_CONN   "meta:proto:pop3:conn"

/*
 * POP3 easy handle state
 */
ರಚನೆ POP3 {
  curl_pp_transfer transfer;
  ಅಕ್ಷರ *id;               /* Message ID */
  ಅಕ್ಷರ *custom;           /* Custom Request */
};

/*
 * POP3 connection state
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
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
  POP3_ಕೊನೆಯ          /* never used */
} pop3state;

ರಚನೆ pop3_conn {
  ರಚನೆ pingpong pp;
  pop3state state;        /* Always use pop3.c:state() to change state! */
  size_t eob;             /* Number of bytes of the EOB (End Of Body) that
                             have been received so far */
  size_t strip;           /* Number of bytes from the start to ignore as
                             non-body */
  ರಚನೆ SASL sasl;       /* SASL-related storage */
  ಅಕ್ಷರ *apoptimestamp;    /* APOP timestamp from the server greeting */
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ authtypes; /* Accepted authentication types */
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ preftype;  /* Preferred authentication type */
  ಬಿಟ್(ssldone);           /* Is connect() over SSL done? */
  ಬಿಟ್(tls_supported);     /* StartTLS capability supported by server */
};

ರಚನೆ pop3_cmd {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ nlen;
  ಬಿಟ್(multiline); /* response is multi-line with last '.' line */
  ಬಿಟ್(multiline_with_ವಾದಗಳು); /* is multi-line when command has args */
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ pop3_cmd pop3cmds[] = {
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
ಸ್ಥಿರ CURLcode pop3_parse_url_options(ರಚನೆ connectdata *conn)
{
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr = conn->options;

  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆಗ(!ಫಲಿತಾಂಶ && ptr && *ptr) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ = ptr;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ;

    ಆಗ(*ptr && *ptr != '=')
      ptr++;

    ಮೌಲ್ಯ = ptr + 1;

    ಆಗ(*ptr && *ptr != ';')
      ptr++;

    ಆದರೆ(curl_strnequal(ಕೀಲಿ, "AUTH=", 5)) {
      ಫಲಿತಾಂಶ = Curl_sasl_parse_url_auth_option(&pop3c->sasl,
                                               ಮೌಲ್ಯ, ptr - ಮೌಲ್ಯ);

      ಆದರೆ(ಫಲಿತಾಂಶ && curl_strnequal(ಮೌಲ್ಯ, "+APOP", ptr - ಮೌಲ್ಯ)) {
        pop3c->preftype = POP3_TYPE_APOP;
        pop3c->sasl.prefmech = SASL_AUTH_NONE;
        ಫಲಿತಾಂಶ = CURLE_OK;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = CURLE_URL_MALFORMAT;

    ಆದರೆ(*ptr == ';')
      ptr++;
  }

  ಆದರೆ(pop3c->preftype != POP3_TYPE_APOP)
    ಆಯ್ಕೆ(pop3c->sasl.prefmech) {
    ಸ್ಥಿತಿ SASL_AUTH_NONE:
      pop3c->preftype = POP3_TYPE_NONE;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SASL_AUTH_DEFAULT:
      pop3c->preftype = POP3_TYPE_ANY;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      pop3c->preftype = POP3_TYPE_SASL;
      ನಿಲ್ಲಿಸು;
    }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * pop3_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
ಸ್ಥಿರ CURLcode pop3_parse_url_ಮಾರ್ಗ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  /* The POP3 struct is already initialized in pop3_connect() */
  ರಚನೆ POP3 *pop3 = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_POP3_EASY);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ = &ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ[1]; /* skip leading path */

  ಆದರೆ(!pop3)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  /* URL decode the path for the message ID */
  ಹಿಂತಿರುಗಿ Curl_urldecode(ಮಾರ್ಗ, 0, &pop3->id, ಖಾಲಿ, REJECT_CTRL);
}

/***********************************************************************
 *
 * pop3_parse_custom_request()
 *
 * Parse the custom request.
 */
ಸ್ಥಿರ CURLcode pop3_parse_custom_request(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ POP3 *pop3 = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_POP3_EASY);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *custom = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_CUSTOMREQUEST);

  ಆದರೆ(!pop3)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  /* URL decode the custom request */
  ಆದರೆ(custom)
    ಫಲಿತಾಂಶ = Curl_urldecode(custom, 0, &pop3->custom, ಖಾಲಿ, REJECT_CTRL);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Return iff a command is defined as "multi-line" (RFC 1939),
 * has a response terminated by a last line with a '.'.
 */
ಸ್ಥಿರ bool pop3_is_multiline(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cmdline)
{
  size_t i;
  ಗಾಗಿ(i = 0; i < CURL_ARRAYSIZE(pop3cmds); ++i) {
    ಆದರೆ(curl_strnequal(pop3cmds[i].name, cmdline, pop3cmds[i].nlen)) {
      ಆದರೆ(!cmdline[pop3cmds[i].nlen])
        ಹಿಂತಿರುಗಿ (bool)pop3cmds[i].multiline;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cmdline[pop3cmds[i].nlen] == ' ')
        ಹಿಂತಿರುಗಿ (bool)pop3cmds[i].multiline_with_ವಾದಗಳು;
    }
  }
  /* Unknown command, assume multi-line for backward compatibility with
   * earlier curl versions that only could do multi-line responses. */
  ಹಿಂತಿರುಗಿ TRUE;
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
ಸ್ಥಿರ bool pop3_endofresp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ connectdata *conn,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, size_t len, ಪೂರ್ಣಾಂಕ *resp)
{
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  DEBUGASSERT(pop3c);
  ಆದರೆ(!pop3c) /* internal error */
    ಹಿಂತಿರುಗಿ TRUE;

  /* Do we have an error response? */
  ಆದರೆ(len >= 4 && !memcmp("-ERR", ಸಾಲು, 4)) {
    *resp = '-';

    ಹಿಂತಿರುಗಿ TRUE;
  }

  /* Are we processing CAPA command responses? */
  ಆದರೆ(pop3c->state == POP3_CAPA) {
    /* Do we have the terminating line? Per RFC 2449 this is a line
       containing only a single dot */
    ಆದರೆ((len == 3 && ಸಾಲು[0] == '.' && ಸಾಲು[1] == '\r') ||
       (len == 2 && ಸಾಲು[0] == '.' && ಸಾಲು[1] == '\n'))
      /* Treat the response as a success */
      *resp = '+';
    ಇಲ್ಲದಿದ್ದರೆ
      /* Treat the response as an untagged continuation */
      *resp = '*';

    ಹಿಂತಿರುಗಿ TRUE;
  }

  /* Do we have a success response? */
  ಆದರೆ(len >= 3 && !memcmp("+OK", ಸಾಲು, 3)) {
    *resp = '+';

    ಹಿಂತಿರುಗಿ TRUE;
  }

  /* Do we have a continuation response? */
  ಆದರೆ(len >= 1 && ಸಾಲು[0] == '+') {
    *resp = '*';

    ಹಿಂತಿರುಗಿ TRUE;
  }

  ಹಿಂತಿರುಗಿ FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * pop3_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ಸ್ಥಿರ CURLcode pop3_get_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ bufref *ಹೊರಹೋಗು)
{
  ರಚನೆ pop3_conn *pop3c =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_POP3_CONN);
  ಅಕ್ಷರ *ಸಂದೇಶ_2;
  size_t len;

  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಸಂದೇಶ_2 = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;
  ಆದರೆ(len > 2) {
    /* Find the start of the message */
    len -= 2;
    ಗಾಗಿ(ಸಂದೇಶ_2 += 2; ISBLANK(*ಸಂದೇಶ_2); ಸಂದೇಶ_2++, len--)
      ;

    /* Find the end of the message */
    ಆಗ(len--)
      ಆದರೆ(!ISBLANK(ಸಂದೇಶ_2[len]) && !ISNEWLINE(ಸಂದೇಶ_2[len]))
        ನಿಲ್ಲಿಸು;

    /* Terminate the message */
    ಸಂದೇಶ_2[++len] = '\0';
    Curl_bufref_ಗಣ(ಹೊರಹೋಗು, ಸಂದೇಶ_2, len, ಖಾಲಿ);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    /* junk input => zero length output */
    Curl_bufref_ಗಣ(ಹೊರಹೋಗು, "", 0, ಖಾಲಿ);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/***********************************************************************
 *
 * pop3_state()
 *
 * This is the ONLY way to change POP3 state!
 */
ಸ್ಥಿರ ಶೂನ್ಯ pop3_state(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, pop3state newstate)
{
  ರಚನೆ pop3_conn *pop3c =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_POP3_CONN);
  ಆದರೆ(pop3c) {
#ಆದರೆ defined(DEBUGBUILD) && defined(CURLVERBOSE)
    /* for debug purposes */
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ಹೆಸರುಗಳು[] = {
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
      /*ಕೊನೆಯ*/
    };

    ಆದರೆ(pop3c->state != newstate)
      infof(ದತ್ತಾಂಶ, "POP3 %p state change from %s to %s",
            (ಶೂನ್ಯ *)pop3c, ಹೆಸರುಗಳು[pop3c->state], ಹೆಸರುಗಳು[newstate]);
#ಆದರೆ_ಅಂತ್ಯ

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
ಸ್ಥಿರ CURLcode pop3_perform_capa(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata *conn)
{
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  pop3c->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  pop3c->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  pop3c->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPA command */
  ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &pop3c->pp, "%s", "CAPA");

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    pop3_state(ದತ್ತಾಂಶ, POP3_CAPA);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * pop3_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
ಸ್ಥಿರ CURLcode pop3_perform_starttls(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ connectdata *conn)
{
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* Send the STLS command */
  ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &pop3c->pp, "%s", "STLS");
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    pop3_state(ದತ್ತಾಂಶ, POP3_STARTTLS);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * pop3_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ಸ್ಥಿರ CURLcode pop3_perform_upgrade_tls(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ connectdata *conn)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  /* Start the SSL connection */
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ಫಲಿತಾಂಶ;
  bool ssldone = FALSE;

  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    ಫಲಿತಾಂಶ = Curl_ssl_cfilter_ಕೂಡಿಸು(
      ದತ್ತಾಂಶ, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_pop3s;
  }

  DEBUGASSERT(!pop3c->ssldone);
  ಫಲಿತಾಂಶ = Curl_conn_connect(ದತ್ತಾಂಶ, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(ದತ್ತಾಂಶ, "pop3_perform_upgrade_tls, connect -> %d, %d",
               (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, ssldone));
  ಆದರೆ(!ಫಲಿತಾಂಶ && ssldone) {
    pop3c->ssldone = ssldone;
    /* perform CAPA now, changes pop3c->state out of POP3_UPGRADETLS */
    ಫಲಿತಾಂಶ = pop3_perform_capa(ದತ್ತಾಂಶ, conn);
  }
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)conn;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
}

/***********************************************************************
 *
 * pop3_perform_user()
 *
 * Sends a clear text USER command to authenticate with.
 */
ಸ್ಥಿರ CURLcode pop3_perform_user(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata *conn)
{
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  ಆದರೆ(!conn->creds) {
    pop3_state(ದತ್ತಾಂಶ, POP3_STOP);

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* Send the USER command */
  ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &pop3c->pp, "USER %s",
                         Curl_creds_user(conn->creds));
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    pop3_state(ದತ್ತಾಂಶ, POP3_USER);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH
/***********************************************************************
 *
 * pop3_perform_apop()
 *
 * Sends an APOP command to authenticate with.
 */
ಸ್ಥಿರ CURLcode pop3_perform_apop(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata *conn)
{
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t i;
  ರಚನೆ MD5_context *ctxt;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ digest[MD5_DIGEST_LEN];
  ಅಕ್ಷರ secret[(2 * MD5_DIGEST_LEN) + 1];

  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  ಆದರೆ(!ದತ್ತಾಂಶ->state.creds) {
    pop3_state(ದತ್ತಾಂಶ, POP3_STOP);

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* Create the digest */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ಆದರೆ(!ctxt)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)pop3c->apoptimestamp,
                  curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(pop3c->apoptimestamp)));

  Curl_MD5_update(ctxt, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)Curl_creds_passwd(conn->creds),
                  curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(Curl_creds_passwd(conn->creds))));

  /* Finalise the digest */
  Curl_MD5_final(ctxt, digest);

  /* Convert the calculated 16 octet digest into a 32-byte hex string */
  ಗಾಗಿ(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&secret[2 * i], 3, "%02x", digest[i]);

  ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &pop3c->pp, "APOP %s %s",
                         Curl_creds_user(conn->creds), secret);

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    pop3_state(ದತ್ತಾಂಶ, POP3_APOP);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

/***********************************************************************
 *
 * pop3_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
ಸ್ಥಿರ CURLcode pop3_perform_auth(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mech,
                                  ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *initresp)
{
  ರಚನೆ pop3_conn *pop3c =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_POP3_CONN);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ir = Curl_bufref_ptr(initresp);

  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &pop3c->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Send the AUTH command */
    ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &pop3c->pp, "AUTH %s", mech);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * pop3_continue_auth()
 *
 * Sends SASL continuation data.
 */
ಸ್ಥಿರ CURLcode pop3_continue_auth(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mech,
                                   ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *resp)
{
  ರಚನೆ pop3_conn *pop3c =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_POP3_CONN);

  (ಶೂನ್ಯ)mech;
  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಹಿಂತಿರುಗಿ Curl_pp_sendf(ದತ್ತಾಂಶ, &pop3c->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * pop3_cancel_auth()
 *
 * Sends SASL cancellation.
 */
ಸ್ಥಿರ CURLcode pop3_cancel_auth(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mech)
{
  ರಚನೆ pop3_conn *pop3c =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_POP3_CONN);

  (ಶೂನ್ಯ)mech;
  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಹಿಂತಿರುಗಿ Curl_pp_sendf(ದತ್ತಾಂಶ, &pop3c->pp, "*");
}

/***********************************************************************
 *
 * pop3_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to APOP and clear text should a
 * common mechanism not be available between the client and server.
 */
ಸ್ಥಿರ CURLcode pop3_perform_authentication(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                            ರಚನೆ connectdata *conn)
{
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  saslprogress ಪ್ರಗತಿ = SASL_IDLE;

  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* Check we have enough data to authenticate with and end the
     connect phase if we do not */
  ಆದರೆ(!Curl_sasl_can_authenticate(&pop3c->sasl, ದತ್ತಾಂಶ)) {
    pop3_state(ದತ್ತಾಂಶ, POP3_STOP);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_SASL) {
    /* Calculate the SASL login details */
    ಫಲಿತಾಂಶ = Curl_sasl_ಆರಂಭ(&pop3c->sasl, ದತ್ತಾಂಶ, FALSE, &ಪ್ರಗತಿ);

    ಆದರೆ(!ಫಲಿತಾಂಶ && ಪ್ರಗತಿ == SASL_INPROGRESS)
      pop3_state(ದತ್ತಾಂಶ, POP3_AUTH);
  }

  ಆದರೆ(!ಫಲಿತಾಂಶ && ಪ್ರಗತಿ == SASL_IDLE) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH
    ಆದರೆ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
      /* Perform APOP authentication */
      ಫಲಿತಾಂಶ = pop3_perform_apop(ದತ್ತಾಂಶ, conn);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
      /* Perform clear text authentication */
      ಫಲಿತಾಂಶ = pop3_perform_user(ದತ್ತಾಂಶ, conn);
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = Curl_sasl_is_blocked(&pop3c->sasl, ದತ್ತಾಂಶ);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * pop3_perform_command()
 *
 * Sends a POP3 based command.
 */
ಸ್ಥಿರ CURLcode pop3_perform_command(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ POP3 *pop3 = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_POP3_EASY);
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *command = ಖಾಲಿ;

  ಆದರೆ(!pop3 || !pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* Calculate the default command */
  ಆದರೆ(pop3->id[0] == '\0' || ದತ್ತಾಂಶ->ಗಣ.list_only) {
    command = "LIST";

    ಆದರೆ(pop3->id[0] != '\0')
      /* Message specific LIST so skip the BODY transfer */
      pop3->transfer = PPTRANSFER_INFO;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    command = "RETR";

  ಆದರೆ(pop3->custom && pop3->custom[0] != '\0')
    command = pop3->custom;

  /* Send the command */
  ಆದರೆ(pop3->id[0] != '\0')
    ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &pop3c->pp, "%s %s", command, pop3->id);
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &pop3c->pp, "%s", command);

  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    pop3_state(ದತ್ತಾಂಶ, POP3_COMMAND);
    ದತ್ತಾಂಶ->req.no_body = !pop3_is_multiline(command);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * pop3_perform_quit()
 *
 * Performs the quit action prior to sclose() be called.
 */
ಸ್ಥಿರ CURLcode pop3_perform_quit(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata *conn)
{
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* Send the QUIT command */
  ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &pop3c->pp, "%s", "QUIT");
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    pop3_state(ದತ್ತಾಂಶ, POP3_QUIT);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For the initial server greeting */
ಸ್ಥಿರ CURLcode pop3_state_servergreet_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                            ಪೂರ್ಣಾಂಕ pop3code,
                                            pop3state instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು;
  size_t len;

  (ಶೂನ್ಯ)instate;
  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಸಾಲು = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  ಆದರೆ(pop3code != '+') {
    failf(ದತ್ತಾಂಶ, "Got unexpected pop3-server response");
    ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(len > 3) {
    /* Does the server support APOP authentication? */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *lt;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *gt = ಖಾಲಿ;

    /* Look for the APOP timestamp */
    lt = memchr(ಸಾಲು, '<', len);
    ಆದರೆ(lt)
      /* search the remainder for '>' */
      gt = memchr(lt, '>', len - (lt - ಸಾಲು));
    ಆದರೆ(gt) {
      /* the length of the timestamp, including the brackets */
      size_t timestamplen = gt - lt + 1;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *at = memchr(lt, '@', timestamplen);
      /* If the timestamp does not contain '@' it is not (as required by
         RFC-1939) conformant to the RFC-822 message id syntax, and we
         therefore do not use APOP authentication. */
      ಆದರೆ(at) {
        /* dupe the timestamp */
        pop3c->apoptimestamp = curlx_memdup0(lt, timestamplen);
        ಆದರೆ(!pop3c->apoptimestamp)
          ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        /* Store the APOP capability */
        pop3c->authtypes |= POP3_TYPE_APOP;
      }
    }

    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = pop3_perform_capa(ದತ್ತಾಂಶ, conn);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For CAPA responses */
ಸ್ಥಿರ CURLcode pop3_state_capa_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ pop3code,
                                     pop3state instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು;
  size_t len;

  (ಶೂನ್ಯ)instate;
  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಸಾಲು = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  /* Do we have an untagged continuation response? */
  ಆದರೆ(pop3code == '*') {
    /* Does the server support the STLS capability? */
    ಆದರೆ(len >= 4 && curl_strnequal(ಸಾಲು, "STLS", 4))
      pop3c->tls_supported = TRUE;

    /* Does the server support clear text authentication? */
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(len >= 4 && curl_strnequal(ಸಾಲು, "USER", 4))
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    /* Does the server support SASL based authentication? */
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(len >= 5 && curl_strnequal(ಸಾಲು, "SASL ", 5)) {
      pop3c->authtypes |= POP3_TYPE_SASL;

      /* Advance past the SASL keyword */
      ಸಾಲು += 5;
      len -= 5;

      /* Loop through the data line */
      ಗಾಗಿ(;;) {
        size_t llen;
        size_t wordlen = 0;
        ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ mechbit;

        ಆಗ(len && (ISBLANK(*ಸಾಲು) || ISNEWLINE(*ಸಾಲು))) {
          ಸಾಲು++;
          len--;
        }

        ಆದರೆ(!len)
          ನಿಲ್ಲಿಸು;

        /* Extract the word */
        ಆಗ(wordlen < len && !ISBLANK(ಸಾಲು[wordlen]) &&
              !ISNEWLINE(ಸಾಲು[wordlen]))
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(ಸಾಲು, wordlen, &llen);
        ಆದರೆ(mechbit && llen == wordlen)
          pop3c->sasl.authmechs |= mechbit;

        ಸಾಲು += wordlen;
        len -= wordlen;
      }
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Clear text is supported when CAPA is not recognised */
    ಆದರೆ(pop3code != '+')
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.use_ssl || Curl_conn_is_ssl(conn, FIRSTSOCKET))
      ಫಲಿತಾಂಶ = pop3_perform_authentication(ದತ್ತಾಂಶ, conn);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pop3code == '+' && pop3c->tls_supported)
      /* Switch to TLS connection now */
      ಫಲಿತಾಂಶ = pop3_perform_starttls(ದತ್ತಾಂಶ, conn);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.use_ssl <= CURLUSESSL_TRY)
      /* Fallback and carry on with authentication */
      ಫಲಿತಾಂಶ = pop3_perform_authentication(ದತ್ತಾಂಶ, conn);
    ಇಲ್ಲದಿದ್ದರೆ {
      failf(ದತ್ತಾಂಶ, "STLS not supported.");
      ಫಲಿತಾಂಶ = CURLE_USE_SSL_FAILED;
    }
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For STARTTLS responses */
ಸ್ಥಿರ CURLcode pop3_state_starttls_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ connectdata *conn,
                                         ಪೂರ್ಣಾಂಕ pop3code,
                                         pop3state instate)
{
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* Pipelining in response is forbidden. */
  ಆದರೆ(pop3c->pp.ಅತಿಪ್ರವಾಹ)
    ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;

  ಆದರೆ(pop3code != '+') {
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.use_ssl != CURLUSESSL_TRY) {
      failf(ದತ್ತಾಂಶ, "STARTTLS denied");
      ಫಲಿತಾಂಶ = CURLE_USE_SSL_FAILED;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = pop3_perform_authentication(ದತ್ತಾಂಶ, conn);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    pop3_state(ದತ್ತಾಂಶ, POP3_UPGRADETLS);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For SASL authentication responses */
ಸ್ಥಿರ CURLcode pop3_state_auth_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ಪೂರ್ಣಾಂಕ pop3code,
                                     pop3state instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  saslprogress ಪ್ರಗತಿ;

  (ಶೂನ್ಯ)instate;
  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಫಲಿತಾಂಶ = Curl_sasl_continue(&pop3c->sasl, ದತ್ತಾಂಶ, pop3code, &ಪ್ರಗತಿ);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಆಯ್ಕೆ(ಪ್ರಗತಿ) {
    ಸ್ಥಿತಿ SASL_ಮುಗಿದ:
      pop3_state(ದತ್ತಾಂಶ, POP3_STOP);  /* Authenticated */
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SASL_IDLE:            /* No mechanism left after cancellation */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH
      ಆದರೆ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
        /* Perform APOP authentication */
        ಫಲಿತಾಂಶ = pop3_perform_apop(ದತ್ತಾಂಶ, conn);
      ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
      ಆದರೆ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
        /* Perform clear text authentication */
        ಫಲಿತಾಂಶ = pop3_perform_user(ದತ್ತಾಂಶ, conn);
      ಇಲ್ಲದಿದ್ದರೆ {
        failf(ದತ್ತಾಂಶ, "Authentication cancelled");
        ಫಲಿತಾಂಶ = CURLE_LOGIN_DENIED;
      }
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ನಿಲ್ಲಿಸು;
    }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH
/* For APOP responses */
ಸ್ಥಿರ CURLcode pop3_state_apop_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ pop3code,
                                     pop3state instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  ಆದರೆ(pop3code != '+') {
    failf(ದತ್ತಾಂಶ, "Authentication failed: %d", pop3code);
    ಫಲಿತಾಂಶ = CURLE_LOGIN_DENIED;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    /* End of connect phase */
    pop3_state(ದತ್ತಾಂಶ, POP3_STOP);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

/* For USER responses */
ಸ್ಥಿರ CURLcode pop3_state_user_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ pop3code,
                                     pop3state instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (ಶೂನ್ಯ)instate;

  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(pop3code != '+') {
    failf(ದತ್ತಾಂಶ, "Access denied. %c", pop3code);
    ಫಲಿತಾಂಶ = CURLE_LOGIN_DENIED;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    /* Send the PASS command */
    ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &pop3c->pp, "PASS %s",
                           Curl_creds_passwd(conn->creds));
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    pop3_state(ದತ್ತಾಂಶ, POP3_PASS);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For PASS responses */
ಸ್ಥಿರ CURLcode pop3_state_pass_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ pop3code,
                                     pop3state instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  ಆದರೆ(pop3code != '+') {
    failf(ದತ್ತಾಂಶ, "Access denied. %c", pop3code);
    ಫಲಿತಾಂಶ = CURLE_LOGIN_DENIED;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    /* End of connect phase */
    pop3_state(ದತ್ತಾಂಶ, POP3_STOP);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * pop3_write()
 *
 * This function scans the body after the end-of-body and writes everything
 * until the end is found.
 */
ಸ್ಥಿರ CURLcode pop3_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str,
                           size_t nread, bool is_eos)
{
  /* This code could be made into a special function in the handler struct */
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  bool strip_dot = FALSE;
  size_t ಕೊನೆಯ = 0;
  size_t i;
  (ಶೂನ್ಯ)is_eos;

  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* Search through the buffer looking for the end-of-body marker which is
     5 bytes (0d 0a 2e 0d 0a). Note that a line starting with a dot matches
     the eob so the server will have prefixed it with an extra dot which we
     need to strip out. Additionally the marker could of course be spread out
     over 5 different data chunks. */
  ಗಾಗಿ(i = 0; i < nread; i++) {
    size_t prev = pop3c->eob;

    ಆಯ್ಕೆ(str[i]) {
    ಸ್ಥಿತಿ 0x0d:
      ಆದರೆ(pop3c->eob == 0) {
        pop3c->eob++;

        ಆದರೆ(i) {
          /* Write out the body part that did not match */
          ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, &str[ಕೊನೆಯ],
                                     i - ಕೊನೆಯ);

          ಆದರೆ(ಫಲಿತಾಂಶ)
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

          ಕೊನೆಯ = i;
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pop3c->eob == 3)
        pop3c->eob++;
      ಇಲ್ಲದಿದ್ದರೆ
        /* If the character match was not at position 0 or 3 then restart the
           pattern matching */
        pop3c->eob = 1;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 0x0a:
      ಆದರೆ(pop3c->eob == 1 || pop3c->eob == 4)
        pop3c->eob++;
      ಇಲ್ಲದಿದ್ದರೆ
        /* If the character match was not at position 1 or 4 then start the
           search again */
        pop3c->eob = 0;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 0x2e:
      ಆದರೆ(pop3c->eob == 2)
        pop3c->eob++;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pop3c->eob == 3) {
        /* We have an extra dot after the CRLF which we need to strip off */
        strip_dot = TRUE;
        pop3c->eob = 0;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        /* If the character match was not at position 2 then start the search
           again */
        pop3c->eob = 0;
      ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
      pop3c->eob = 0;
      ನಿಲ್ಲಿಸು;
    }

    /* Did we have a partial match which has subsequently failed? */
    ಆದರೆ(prev && prev >= pop3c->eob) {
      /* Strip can only be non-zero for the first mismatch after CRLF and
         then both prev and strip are equal and nothing will be output below */
      ಆಗ(prev && pop3c->strip) {
        prev--;
        pop3c->strip--;
      }

      ಆದರೆ(prev) {
        /* If the partial match was the CRLF and dot then only write the CRLF
           as the server would have inserted the dot */
        ಆದರೆ(strip_dot && prev - 1 > 0) {
          ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, POP3_EOB,
                                     prev - 1);
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strip_dot) {
          ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, POP3_EOB,
                                     prev);
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          ಫಲಿತಾಂಶ = CURLE_OK;
        }

        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

        ಕೊನೆಯ = i;
        strip_dot = FALSE;
      }
    }
  }

  ಆದರೆ(pop3c->eob == POP3_EOB_LEN) {
    /* We have a full match so the transfer is done, however we must transfer
    the CRLF at the start of the EOB as this is considered to be part of the
    message as per RFC-1939, sect. 3 */
    ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, POP3_EOB, 2);

    CURL_REQ_CLEAR_RECV(ದತ್ತಾಂಶ);
    pop3c->eob = 0;

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(pop3c->eob)
    /* While EOB is matching nothing should be output */
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಆದರೆ(nread - ಕೊನೆಯ) {
    ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, &str[ಕೊನೆಯ],
                               nread - ಕೊನೆಯ);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For command responses */
ಸ್ಥಿರ CURLcode pop3_state_command_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ಪೂರ್ಣಾಂಕ pop3code,
                                        pop3state instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ POP3 *pop3 = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_POP3_EASY);
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ರಚನೆ pingpong *pp;

  (ಶೂನ್ಯ)instate;
  ಆದರೆ(!pop3 || !pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  ಆದರೆ(pop3code != '+') {
    pop3_state(ದತ್ತಾಂಶ, POP3_STOP);
    ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
  }

  /* This 'OK' line ends with a CR LF pair which is the two first bytes of the
     EOB string so count this is two matching bytes. This is necessary to make
     the code detect the EOB if the only data than comes now is %2e CR LF like
     when there is no body to return. */
  pop3c->eob = 2;

  /* Since this initial CR LF pair is not part of the actual body, we set
     the strip counter here so that these bytes will not be delivered. */
  pop3c->strip = 2;

  ಆದರೆ(pop3->transfer == PPTRANSFER_BODY) {
    /* POP3 download */
    Curl_xfer_ಸಿದ್ಧತೆ_recv(ದತ್ತಾಂಶ, FIRSTSOCKET, -1);

    ಆದರೆ(pp->ಅತಿಪ್ರವಾಹ) {
      /* The recv buffer contains data that is actually body content so send
         it as such. Note that there may even be additional "headers" after
         the body */

      /* keep only the overflow */
      curlx_dyn_ಬಾಲ(&pp->recvbuf, pp->ಅತಿಪ್ರವಾಹ);
      pp->nfinal = 0; /* done */

      ಆದರೆ(!ದತ್ತಾಂಶ->req.no_body) {
        ಫಲಿತಾಂಶ = pop3_write(ದತ್ತಾಂಶ, curlx_dyn_ptr(&pp->recvbuf),
                            curlx_dyn_len(&pp->recvbuf), FALSE);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }

      /* reset the buffer */
      curlx_dyn_reset(&pp->recvbuf);
      pp->ಅತಿಪ್ರವಾಹ = 0;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ
    pp->ಅತಿಪ್ರವಾಹ = 0;

  /* End of DO phase */
  pop3_state(ದತ್ತಾಂಶ, POP3_STOP);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode pop3_statemachine(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata *conn)
{
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ pop3code;
  ರಚನೆ pingpong *pp;
  size_t nread = 0;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;

  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not POP3 */
upgrade_tls:
  ಆದರೆ(pop3c->state == POP3_UPGRADETLS) {
    ಫಲಿತಾಂಶ = pop3_perform_upgrade_tls(ದತ್ತಾಂಶ, conn);
    ಆದರೆ(ಫಲಿತಾಂಶ || (pop3c->state == POP3_UPGRADETLS))
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* Flush any data that needs to be sent */
  ಆದರೆ(pp->sendleft)
    ಹಿಂತಿರುಗಿ Curl_pp_flushsend(ದತ್ತಾಂಶ, pp);

  ಮಾಡು {
     /* Read the response from the server */
    ಫಲಿತಾಂಶ = Curl_pp_readresp(ದತ್ತಾಂಶ, FIRSTSOCKET, pp, &pop3code, &nread);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    ಆದರೆ(!pop3code)
      ನಿಲ್ಲಿಸು;

    /* We have now received a full POP3 server response */
    ಆಯ್ಕೆ(pop3c->state) {
    ಸ್ಥಿತಿ POP3_SERVERGREET:
      ಫಲಿತಾಂಶ = pop3_state_servergreet_resp(ದತ್ತಾಂಶ, pop3code, pop3c->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ POP3_CAPA:
      ಫಲಿತಾಂಶ = pop3_state_capa_resp(ದತ್ತಾಂಶ, pop3code, pop3c->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ POP3_STARTTLS:
      ಫಲಿತಾಂಶ = pop3_state_starttls_resp(ದತ್ತಾಂಶ, conn, pop3code, pop3c->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      ಆದರೆ(!ಫಲಿತಾಂಶ && (pop3c->state == POP3_UPGRADETLS))
        ಹೋಗು upgrade_tls;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ POP3_AUTH:
      ಫಲಿತಾಂಶ = pop3_state_auth_resp(ದತ್ತಾಂಶ, pop3code, pop3c->state);
      ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH
    ಸ್ಥಿತಿ POP3_APOP:
      ಫಲಿತಾಂಶ = pop3_state_apop_resp(ದತ್ತಾಂಶ, pop3code, pop3c->state);
      ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿತಿ POP3_USER:
      ಫಲಿತಾಂಶ = pop3_state_user_resp(ದತ್ತಾಂಶ, pop3code, pop3c->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ POP3_PASS:
      ಫಲಿತಾಂಶ = pop3_state_pass_resp(ದತ್ತಾಂಶ, pop3code, pop3c->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ POP3_COMMAND:
      ಫಲಿತಾಂಶ = pop3_state_command_resp(ದತ್ತಾಂಶ, pop3code, pop3c->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ POP3_QUIT:
      pop3_state(ದತ್ತಾಂಶ, POP3_STOP);
      ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
      /* internal error */
      pop3_state(ದತ್ತಾಂಶ, POP3_STOP);
      ನಿಲ್ಲಿಸು;
    }
  } ಆಗ(!ಫಲಿತಾಂಶ && pop3c->state != POP3_STOP && Curl_pp_moredata(pp));

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Called repeatedly until done from multi.c */
ಸ್ಥಿರ CURLcode pop3_multi_statemach(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಫಲಿತಾಂಶ = Curl_pp_statemach(ದತ್ತಾಂಶ, &pop3c->pp, FALSE, FALSE);
  *ಮುಗಿದ = (pop3c->state == POP3_STOP);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode pop3_ಖಂಡ_statemach(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ connectdata *conn,
                                     bool disconnecting)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆಗ(pop3c->state != POP3_STOP && !ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_pp_statemach(ದತ್ತಾಂಶ, &pop3c->pp, TRUE, disconnecting);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For the POP3 "protocol connect" and "doing" phases only */
ಸ್ಥಿರ CURLcode pop3_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ pop3_conn *pop3c =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_POP3_CONN);
  ಹಿಂತಿರುಗಿ pop3c ? Curl_pp_pollset(ದತ್ತಾಂಶ, &pop3c->pp, ps) : CURLE_OK;
}

/* SASL parameters for the pop3 protocol */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ SASLproto saslpop3 = {
  "pop",                /* The service name */
  pop3_perform_auth,    /* Send authentication command */
  pop3_continue_auth,   /* Send authentication continuation */
  pop3_cancel_auth,     /* Send authentication cancellation */
  pop3_get_ಸಂದೇಶ,     /* Get SASL response message */
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
ಸ್ಥಿರ CURLcode pop3_connect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ರಚನೆ pingpong *pp = pop3c ? &pop3c->pp : ಖಾಲಿ;

  *ಮುಗಿದ = FALSE; /* default to not done yet */
  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  PINGPONG_ಸಿದ್ಧತೆ(pp, pop3_statemachine, pop3_endofresp);

  /* Set the default preferred authentication type and mechanism */
  pop3c->preftype = POP3_TYPE_ANY;
  Curl_sasl_init(&pop3c->sasl, ದತ್ತಾಂಶ, &saslpop3);

  /* Initialize the pingpong layer */
  Curl_pp_init(pp, Curl_pgrs_now(ದತ್ತಾಂಶ));

  /* Parse the URL options */
  ಫಲಿತಾಂಶ = pop3_parse_url_options(conn);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* Start off waiting for the server greeting response */
  pop3_state(ದತ್ತಾಂಶ, POP3_SERVERGREET);

  ಫಲಿತಾಂಶ = pop3_multi_statemach(ದತ್ತಾಂಶ, ಮುಗಿದ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ಸ್ಥಿರ CURLcode pop3_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLcode ಸ್ಥಿತಿ_2,
                          bool premature)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ POP3 *pop3 = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_POP3_EASY);

  (ಶೂನ್ಯ)premature;

  ಆದರೆ(!pop3)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಆದರೆ(ಸ್ಥಿತಿ_2) {
    CURL_TRC_M(ದತ್ತಾಂಶ, "POP3 done with bad status");
    connclose(ದತ್ತಾಂಶ->conn);
    ಫಲಿತಾಂಶ = ಸ್ಥಿತಿ_2;         /* use the already set error code */
  }

  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);

  /* Clear the transfer mode for the next request */
  pop3->transfer = PPTRANSFER_BODY;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * pop3_perform()
 *
 * This is the actual DO function for POP3. Get a message/listing according to
 * the options previously setup.
 */
ಸ್ಥಿರ CURLcode pop3_perform(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *connected,
                             bool *dophase_ಮುಗಿದ)
{
  /* This is POP3 and no proxy */
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ POP3 *pop3 = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_POP3_EASY);

  ಆದರೆ(!pop3)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  DEBUGF(infof(ದತ್ತಾಂಶ, "DO phase starts"));

  /* Start the first command in the DO phase, may alter data->req.no_body */
  ಫಲಿತಾಂಶ = pop3_perform_command(ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಆದರೆ(ದತ್ತಾಂಶ->req.no_body)
    /* Requested no body means no transfer */
    pop3->transfer = PPTRANSFER_INFO;

  *dophase_ಮುಗಿದ = FALSE; /* not done yet */

  /* Run the state-machine */
  ಫಲಿತಾಂಶ = pop3_multi_statemach(ದತ್ತಾಂಶ, dophase_ಮುಗಿದ);
  *connected = Curl_conn_is_connected(ದತ್ತಾಂಶ->conn, FIRSTSOCKET);

  ಆದರೆ(*dophase_ಮುಗಿದ)
    DEBUGF(infof(ದತ್ತಾಂಶ, "DO phase is complete"));

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Call this when the DO phase has completed */
ಸ್ಥಿರ CURLcode pop3_dophase_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool connected)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)connected;

  ಹಿಂತಿರುಗಿ CURLE_OK;
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
ಸ್ಥಿರ CURLcode pop3_regular_transfer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      bool *dophase_ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = -1;

  /* Set the progress data */
  Curl_pgrsReset(ದತ್ತಾಂಶ);

  /* Carry out the perform */
  ಫಲಿತಾಂಶ = pop3_perform(ದತ್ತಾಂಶ, &connected, dophase_ಮುಗಿದ);

  /* Perform post DO phase operations if necessary */
  ಆದರೆ(!ಫಲಿತಾಂಶ && *dophase_ಮುಗಿದ)
    ಫಲಿತಾಂಶ = pop3_dophase_ಮುಗಿದ(ದತ್ತಾಂಶ, connected);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ಸ್ಥಿರ CURLcode pop3_do(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  *ಮುಗಿದ = FALSE; /* default to false */

  /* Parse the URL path */
  ಫಲಿತಾಂಶ = pop3_parse_url_ಮಾರ್ಗ(ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* Parse the custom request */
  ಫಲಿತಾಂಶ = pop3_parse_custom_request(ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = pop3_regular_transfer(ದತ್ತಾಂಶ, ಮುಗಿದ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * pop3_disconnect()
 *
 * Disconnect from an POP3 server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ಸ್ಥಿರ CURLcode pop3_disconnect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ connectdata *conn, bool dead_connection)
{
  ರಚನೆ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;

  ಆದರೆ(!pop3c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  ಆದರೆ(!dead_connection && conn->ಬಿಟ್ಟುಗಳು.protoconnstart &&
     !Curl_pp_needs_flush(ದತ್ತಾಂಶ, &pop3c->pp) &&
     !pop3_perform_quit(ದತ್ತಾಂಶ, conn))
    (ಶೂನ್ಯ)pop3_ಖಂಡ_statemach(ದತ್ತಾಂಶ, conn, TRUE); /* ignore errors on QUIT */

  /* Disconnect from the server */
  Curl_pp_disconnect(&pop3c->pp);

  /* Cleanup our connection based variables */
  curlx_safefree(pop3c->apoptimestamp);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Called from multi.c while DOing */
ಸ್ಥಿರ CURLcode pop3_doing(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *dophase_ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = pop3_multi_statemach(ದತ್ತಾಂಶ, dophase_ಮುಗಿದ);

  ಆದರೆ(ಫಲಿತಾಂಶ)
    DEBUGF(infof(ದತ್ತಾಂಶ, "DO phase failed"));
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*dophase_ಮುಗಿದ) {
    ಫಲಿತಾಂಶ = pop3_dophase_ಮುಗಿದ(ದತ್ತಾಂಶ, FALSE /* not connected */);

    DEBUGF(infof(ದತ್ತಾಂಶ, "DO phase is complete"));
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ pop3_easy_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ POP3 *pop3 = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  DEBUGASSERT(pop3);
  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);
  curlx_free(pop3);
}

ಸ್ಥಿರ ಶೂನ್ಯ pop3_conn_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ pop3_conn *pop3c = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  DEBUGASSERT(pop3c);
  Curl_pp_disconnect(&pop3c->pp);
  curlx_safefree(pop3c->apoptimestamp);
  curlx_free(pop3c);
}

ಸ್ಥಿರ CURLcode pop3_ಸಿದ್ಧತೆ_connection(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ connectdata *conn)
{
  ರಚನೆ pop3_conn *pop3c;
  ರಚನೆ POP3 *pop3 = curlx_calloc(1, ಗಾತ್ರ(*pop3));
  ಆದರೆ(!pop3 ||
     Curl_meta_ಗಣ(ದತ್ತಾಂಶ, CURL_META_POP3_EASY, pop3, pop3_easy_dtor))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  pop3c = curlx_calloc(1, ಗಾತ್ರ(*pop3c));
  ಆದರೆ(!pop3c ||
     Curl_conn_meta_ಗಣ(conn, CURL_META_POP3_CONN, pop3c, pop3_conn_dtor))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * POP3 protocol.
 */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_pop3 = {
  pop3_ಸಿದ್ಧತೆ_connection,            /* setup_connection */
  pop3_do,                          /* do_it */
  pop3_ಮುಗಿದ,                        /* done */
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

#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_POP3 */
