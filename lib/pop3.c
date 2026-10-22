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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pop3.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_POP3

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#ਅਨ੍ਤਰ੍ਭਾਵ <in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <inet.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pingpong.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sasl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_md5.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"

/* Authentication type flags */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POP3_TYPE_CLEARTEXT (1 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POP3_TYPE_APOP      (1 << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POP3_TYPE_SASL      (1 << 2)

/* Authentication type values */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POP3_TYPE_NONE 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POP3_TYPE_ANY  (POP3_TYPE_CLEARTEXT | POP3_TYPE_APOP | POP3_TYPE_SASL)

/* This is the 5-bytes End-Of-Body marker for POP3 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POP3_EOB     "\x0d\x0a\x2e\x0d\x0a"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POP3_EOB_LEN 5

/* meta key for storing protocol meta at easy handle */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_POP3_EASY   "meta:proto:pop3:easy"
/* meta key for storing protocol meta at connection */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_POP3_CONN   "meta:proto:pop3:conn"

/*
 * POP3 easy handle state
 */
ਬਣਤਰ POP3 {
  curl_pp_transfer transfer;
  ਅੱਖਰ *id;               /* Message ID */
  ਅੱਖਰ *custom;           /* Custom Request */
};

/*
 * POP3 connection state
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
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
  POP3_ਆਖਰੀ          /* never used */
} pop3state;

ਬਣਤਰ pop3_conn {
  ਬਣਤਰ pingpong pp;
  pop3state state;        /* Always use pop3.c:state() to change state! */
  size_t eob;             /* Number of bytes of the EOB (End Of Body) that
                             have been received so far */
  size_t strip;           /* Number of bytes from the start to ignore as
                             non-body */
  ਬਣਤਰ SASL sasl;       /* SASL-related storage */
  ਅੱਖਰ *apoptimestamp;    /* APOP timestamp from the server greeting */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ authtypes; /* Accepted authentication types */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ preftype;  /* Preferred authentication type */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ssldone);           /* Is connect() over SSL done? */
  ਦੋਆਧਾਰੀ_ਅੰਕ(tls_supported);     /* StartTLS capability supported by server */
};

ਬਣਤਰ pop3_cmd {
  ਅਚਲ ਅੱਖਰ *name;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ nlen;
  ਦੋਆਧਾਰੀ_ਅੰਕ(multiline); /* response is multi-line with last '.' line */
  ਦੋਆਧਾਰੀ_ਅੰਕ(multiline_with_ਦਲੀਲਾਂ); /* is multi-line when command has args */
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ pop3_cmd pop3cmds[] = {
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
ਸਥਿਰ CURLcode pop3_parse_url_options(ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ *ptr = conn->options;

  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜਦੋਂ(!ਨਤੀਜਾ && ptr && *ptr) {
    ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ = ptr;
    ਅਚਲ ਅੱਖਰ *ਮੁੱਲ;

    ਜਦੋਂ(*ptr && *ptr != '=')
      ptr++;

    ਮੁੱਲ = ptr + 1;

    ਜਦੋਂ(*ptr && *ptr != ';')
      ptr++;

    ਜੇ(curl_strnequal(ਕੁੰਜੀ, "AUTH=", 5)) {
      ਨਤੀਜਾ = Curl_sasl_parse_url_auth_option(&pop3c->sasl,
                                               ਮੁੱਲ, ptr - ਮੁੱਲ);

      ਜੇ(ਨਤੀਜਾ && curl_strnequal(ਮੁੱਲ, "+APOP", ptr - ਮੁੱਲ)) {
        pop3c->preftype = POP3_TYPE_APOP;
        pop3c->sasl.prefmech = SASL_AUTH_NONE;
        ਨਤੀਜਾ = CURLE_OK;
      }
    }
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = CURLE_URL_MALFORMAT;

    ਜੇ(*ptr == ';')
      ptr++;
  }

  ਜੇ(pop3c->preftype != POP3_TYPE_APOP)
    ਚੋਣ(pop3c->sasl.prefmech) {
    ਹਾਲਤ SASL_AUTH_NONE:
      pop3c->preftype = POP3_TYPE_NONE;
      ਰੋਕੋ;
    ਹਾਲਤ SASL_AUTH_DEFAULT:
      pop3c->preftype = POP3_TYPE_ANY;
      ਰੋਕੋ;
    ਮੂਲ:
      pop3c->preftype = POP3_TYPE_SASL;
      ਰੋਕੋ;
    }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * pop3_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
ਸਥਿਰ CURLcode pop3_parse_url_ਰਸਤਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  /* The POP3 struct is already initialized in pop3_connect() */
  ਬਣਤਰ POP3 *pop3 = Curl_meta_get(ਅੰਕੜੇ, CURL_META_POP3_EASY);
  ਅਚਲ ਅੱਖਰ *ਰਸਤਾ = &ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ[1]; /* skip leading path */

  ਜੇ(!pop3)
    ਵਾਪਸ CURLE_FAILED_INIT;
  /* URL decode the path for the message ID */
  ਵਾਪਸ Curl_urldecode(ਰਸਤਾ, 0, &pop3->id, ਨਿਰਾ, REJECT_CTRL);
}

/***********************************************************************
 *
 * pop3_parse_custom_request()
 *
 * Parse the custom request.
 */
ਸਥਿਰ CURLcode pop3_parse_custom_request(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ POP3 *pop3 = Curl_meta_get(ਅੰਕੜੇ, CURL_META_POP3_EASY);
  ਅਚਲ ਅੱਖਰ *custom = CURL_EASY_STR(ਅੰਕੜੇ, STRING_CUSTOMREQUEST);

  ਜੇ(!pop3)
    ਵਾਪਸ CURLE_FAILED_INIT;
  /* URL decode the custom request */
  ਜੇ(custom)
    ਨਤੀਜਾ = Curl_urldecode(custom, 0, &pop3->custom, ਨਿਰਾ, REJECT_CTRL);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Return iff a command is defined as "multi-line" (RFC 1939),
 * has a response terminated by a last line with a '.'.
 */
ਸਥਿਰ bool pop3_is_multiline(ਅਚਲ ਅੱਖਰ *cmdline)
{
  size_t i;
  ਲਈ(i = 0; i < CURL_ARRAYSIZE(pop3cmds); ++i) {
    ਜੇ(curl_strnequal(pop3cmds[i].name, cmdline, pop3cmds[i].nlen)) {
      ਜੇ(!cmdline[pop3cmds[i].nlen])
        ਵਾਪਸ (bool)pop3cmds[i].multiline;
      ਨਹੀਂਤਾਂ ਜੇ(cmdline[pop3cmds[i].nlen] == ' ')
        ਵਾਪਸ (bool)pop3cmds[i].multiline_with_ਦਲੀਲਾਂ;
    }
  }
  /* Unknown command, assume multi-line for backward compatibility with
   * earlier curl versions that only could do multi-line responses. */
  ਵਾਪਸ TRUE;
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
ਸਥਿਰ bool pop3_endofresp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ connectdata *conn,
                           ਅਚਲ ਅੱਖਰ *ਸਤਰ, size_t len, ਪੂਰਨ *resp)
{
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (ਖਾਲੀ)ਅੰਕੜੇ;
  DEBUGASSERT(pop3c);
  ਜੇ(!pop3c) /* internal error */
    ਵਾਪਸ TRUE;

  /* Do we have an error response? */
  ਜੇ(len >= 4 && !memcmp("-ERR", ਸਤਰ, 4)) {
    *resp = '-';

    ਵਾਪਸ TRUE;
  }

  /* Are we processing CAPA command responses? */
  ਜੇ(pop3c->state == POP3_CAPA) {
    /* Do we have the terminating line? Per RFC 2449 this is a line
       containing only a single dot */
    ਜੇ((len == 3 && ਸਤਰ[0] == '.' && ਸਤਰ[1] == '\r') ||
       (len == 2 && ਸਤਰ[0] == '.' && ਸਤਰ[1] == '\n'))
      /* Treat the response as a success */
      *resp = '+';
    ਨਹੀਂਤਾਂ
      /* Treat the response as an untagged continuation */
      *resp = '*';

    ਵਾਪਸ TRUE;
  }

  /* Do we have a success response? */
  ਜੇ(len >= 3 && !memcmp("+OK", ਸਤਰ, 3)) {
    *resp = '+';

    ਵਾਪਸ TRUE;
  }

  /* Do we have a continuation response? */
  ਜੇ(len >= 1 && ਸਤਰ[0] == '+') {
    *resp = '*';

    ਵਾਪਸ TRUE;
  }

  ਵਾਪਸ FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * pop3_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ਸਥਿਰ CURLcode pop3_get_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ bufref *ਬਾਹਰਲਾ)
{
  ਬਣਤਰ pop3_conn *pop3c =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_POP3_CONN);
  ਅੱਖਰ *ਸੁਨੇਹਾ_2;
  size_t len;

  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਸੁਨੇਹਾ_2 = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;
  ਜੇ(len > 2) {
    /* Find the start of the message */
    len -= 2;
    ਲਈ(ਸੁਨੇਹਾ_2 += 2; ISBLANK(*ਸੁਨੇਹਾ_2); ਸੁਨੇਹਾ_2++, len--)
      ;

    /* Find the end of the message */
    ਜਦੋਂ(len--)
      ਜੇ(!ISBLANK(ਸੁਨੇਹਾ_2[len]) && !ISNEWLINE(ਸੁਨੇਹਾ_2[len]))
        ਰੋਕੋ;

    /* Terminate the message */
    ਸੁਨੇਹਾ_2[++len] = '\0';
    Curl_bufref_ਸੈੱਟ(ਬਾਹਰਲਾ, ਸੁਨੇਹਾ_2, len, ਨਿਰਾ);
  }
  ਨਹੀਂਤਾਂ
    /* junk input => zero length output */
    Curl_bufref_ਸੈੱਟ(ਬਾਹਰਲਾ, "", 0, ਨਿਰਾ);

  ਵਾਪਸ CURLE_OK;
}

/***********************************************************************
 *
 * pop3_state()
 *
 * This is the ONLY way to change POP3 state!
 */
ਸਥਿਰ ਖਾਲੀ pop3_state(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, pop3state newstate)
{
  ਬਣਤਰ pop3_conn *pop3c =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_POP3_CONN);
  ਜੇ(pop3c) {
#ਜੇ defined(DEBUGBUILD) && defined(CURLVERBOSE)
    /* for debug purposes */
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ ਨਾਂ[] = {
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
      /*ਆਖਰੀ*/
    };

    ਜੇ(pop3c->state != newstate)
      infof(ਅੰਕੜੇ, "POP3 %p state change from %s to %s",
            (ਖਾਲੀ *)pop3c, ਨਾਂ[pop3c->state], ਨਾਂ[newstate]);
#ਜੇ_ਅੰਤ

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
ਸਥਿਰ CURLcode pop3_perform_capa(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  pop3c->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  pop3c->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  pop3c->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPA command */
  ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &pop3c->pp, "%s", "CAPA");

  ਜੇ(!ਨਤੀਜਾ)
    pop3_state(ਅੰਕੜੇ, POP3_CAPA);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * pop3_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
ਸਥਿਰ CURLcode pop3_perform_starttls(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* Send the STLS command */
  ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &pop3c->pp, "%s", "STLS");
  ਜੇ(!ਨਤੀਜਾ)
    pop3_state(ਅੰਕੜੇ, POP3_STARTTLS);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * pop3_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ਸਥਿਰ CURLcode pop3_perform_upgrade_tls(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ connectdata *conn)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  /* Start the SSL connection */
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ਨਤੀਜਾ;
  bool ssldone = FALSE;

  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    ਨਤੀਜਾ = Curl_ssl_cfilter_ਜੋੜੋ(
      ਅੰਕੜੇ, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_pop3s;
  }

  DEBUGASSERT(!pop3c->ssldone);
  ਨਤੀਜਾ = Curl_conn_connect(ਅੰਕੜੇ, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(ਅੰਕੜੇ, "pop3_perform_upgrade_tls, connect -> %d, %d",
               (ਪੂਰਨ)ਨਤੀਜਾ, ssldone));
  ਜੇ(!ਨਤੀਜਾ && ssldone) {
    pop3c->ssldone = ssldone;
    /* perform CAPA now, changes pop3c->state out of POP3_UPGRADETLS */
    ਨਤੀਜਾ = pop3_perform_capa(ਅੰਕੜੇ, conn);
  }
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)conn;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
}

/***********************************************************************
 *
 * pop3_perform_user()
 *
 * Sends a clear text USER command to authenticate with.
 */
ਸਥਿਰ CURLcode pop3_perform_user(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  ਜੇ(!conn->creds) {
    pop3_state(ਅੰਕੜੇ, POP3_STOP);

    ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* Send the USER command */
  ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &pop3c->pp, "USER %s",
                         Curl_creds_user(conn->creds));
  ਜੇ(!ਨਤੀਜਾ)
    pop3_state(ਅੰਕੜੇ, POP3_USER);

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
/***********************************************************************
 *
 * pop3_perform_apop()
 *
 * Sends an APOP command to authenticate with.
 */
ਸਥਿਰ CURLcode pop3_perform_apop(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t i;
  ਬਣਤਰ MD5_context *ctxt;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ digest[MD5_DIGEST_LEN];
  ਅੱਖਰ secret[(2 * MD5_DIGEST_LEN) + 1];

  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  ਜੇ(!ਅੰਕੜੇ->state.creds) {
    pop3_state(ਅੰਕੜੇ, POP3_STOP);

    ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* Create the digest */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ਜੇ(!ctxt)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)pop3c->apoptimestamp,
                  curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(pop3c->apoptimestamp)));

  Curl_MD5_update(ctxt, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)Curl_creds_passwd(conn->creds),
                  curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(Curl_creds_passwd(conn->creds))));

  /* Finalise the digest */
  Curl_MD5_final(ctxt, digest);

  /* Convert the calculated 16 octet digest into a 32-byte hex string */
  ਲਈ(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&secret[2 * i], 3, "%02x", digest[i]);

  ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &pop3c->pp, "APOP %s %s",
                         Curl_creds_user(conn->creds), secret);

  ਜੇ(!ਨਤੀਜਾ)
    pop3_state(ਅੰਕੜੇ, POP3_APOP);

  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

/***********************************************************************
 *
 * pop3_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
ਸਥਿਰ CURLcode pop3_perform_auth(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਅਚਲ ਅੱਖਰ *mech,
                                  ਅਚਲ ਬਣਤਰ bufref *initresp)
{
  ਬਣਤਰ pop3_conn *pop3c =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_POP3_CONN);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ *ir = Curl_bufref_ptr(initresp);

  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &pop3c->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  ਨਹੀਂਤਾਂ {
    /* Send the AUTH command */
    ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &pop3c->pp, "AUTH %s", mech);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * pop3_continue_auth()
 *
 * Sends SASL continuation data.
 */
ਸਥਿਰ CURLcode pop3_continue_auth(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਅਚਲ ਅੱਖਰ *mech,
                                   ਅਚਲ ਬਣਤਰ bufref *resp)
{
  ਬਣਤਰ pop3_conn *pop3c =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_POP3_CONN);

  (ਖਾਲੀ)mech;
  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਵਾਪਸ Curl_pp_sendf(ਅੰਕੜੇ, &pop3c->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * pop3_cancel_auth()
 *
 * Sends SASL cancellation.
 */
ਸਥਿਰ CURLcode pop3_cancel_auth(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *mech)
{
  ਬਣਤਰ pop3_conn *pop3c =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_POP3_CONN);

  (ਖਾਲੀ)mech;
  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਵਾਪਸ Curl_pp_sendf(ਅੰਕੜੇ, &pop3c->pp, "*");
}

/***********************************************************************
 *
 * pop3_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to APOP and clear text should a
 * common mechanism not be available between the client and server.
 */
ਸਥਿਰ CURLcode pop3_perform_authentication(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                            ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  saslprogress ਤਰੱਕੀ = SASL_IDLE;

  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* Check we have enough data to authenticate with and end the
     connect phase if we do not */
  ਜੇ(!Curl_sasl_can_authenticate(&pop3c->sasl, ਅੰਕੜੇ)) {
    pop3_state(ਅੰਕੜੇ, POP3_STOP);
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_SASL) {
    /* Calculate the SASL login details */
    ਨਤੀਜਾ = Curl_sasl_ਸ਼ੁਰੂ(&pop3c->sasl, ਅੰਕੜੇ, FALSE, &ਤਰੱਕੀ);

    ਜੇ(!ਨਤੀਜਾ && ਤਰੱਕੀ == SASL_INPROGRESS)
      pop3_state(ਅੰਕੜੇ, POP3_AUTH);
  }

  ਜੇ(!ਨਤੀਜਾ && ਤਰੱਕੀ == SASL_IDLE) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
    ਜੇ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
      /* Perform APOP authentication */
      ਨਤੀਜਾ = pop3_perform_apop(ਅੰਕੜੇ, conn);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    ਜੇ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
      /* Perform clear text authentication */
      ਨਤੀਜਾ = pop3_perform_user(ਅੰਕੜੇ, conn);
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = Curl_sasl_is_blocked(&pop3c->sasl, ਅੰਕੜੇ);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * pop3_perform_command()
 *
 * Sends a POP3 based command.
 */
ਸਥਿਰ CURLcode pop3_perform_command(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ POP3 *pop3 = Curl_meta_get(ਅੰਕੜੇ, CURL_META_POP3_EASY);
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ਅਚਲ ਅੱਖਰ *command = ਨਿਰਾ;

  ਜੇ(!pop3 || !pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* Calculate the default command */
  ਜੇ(pop3->id[0] == '\0' || ਅੰਕੜੇ->ਸੈੱਟ.list_only) {
    command = "LIST";

    ਜੇ(pop3->id[0] != '\0')
      /* Message specific LIST so skip the BODY transfer */
      pop3->transfer = PPTRANSFER_INFO;
  }
  ਨਹੀਂਤਾਂ
    command = "RETR";

  ਜੇ(pop3->custom && pop3->custom[0] != '\0')
    command = pop3->custom;

  /* Send the command */
  ਜੇ(pop3->id[0] != '\0')
    ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &pop3c->pp, "%s %s", command, pop3->id);
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &pop3c->pp, "%s", command);

  ਜੇ(!ਨਤੀਜਾ) {
    pop3_state(ਅੰਕੜੇ, POP3_COMMAND);
    ਅੰਕੜੇ->req.no_body = !pop3_is_multiline(command);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * pop3_perform_quit()
 *
 * Performs the quit action prior to sclose() be called.
 */
ਸਥਿਰ CURLcode pop3_perform_quit(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* Send the QUIT command */
  ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &pop3c->pp, "%s", "QUIT");
  ਜੇ(!ਨਤੀਜਾ)
    pop3_state(ਅੰਕੜੇ, POP3_QUIT);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For the initial server greeting */
ਸਥਿਰ CURLcode pop3_state_servergreet_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                            ਪੂਰਨ pop3code,
                                            pop3state instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ਅਚਲ ਅੱਖਰ *ਸਤਰ;
  size_t len;

  (ਖਾਲੀ)instate;
  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਸਤਰ = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  ਜੇ(pop3code != '+') {
    failf(ਅੰਕੜੇ, "Got unexpected pop3-server response");
    ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(len > 3) {
    /* Does the server support APOP authentication? */
    ਅਚਲ ਅੱਖਰ *lt;
    ਅਚਲ ਅੱਖਰ *gt = ਨਿਰਾ;

    /* Look for the APOP timestamp */
    lt = memchr(ਸਤਰ, '<', len);
    ਜੇ(lt)
      /* search the remainder for '>' */
      gt = memchr(lt, '>', len - (lt - ਸਤਰ));
    ਜੇ(gt) {
      /* the length of the timestamp, including the brackets */
      size_t timestamplen = gt - lt + 1;
      ਅਚਲ ਅੱਖਰ *at = memchr(lt, '@', timestamplen);
      /* If the timestamp does not contain '@' it is not (as required by
         RFC-1939) conformant to the RFC-822 message id syntax, and we
         therefore do not use APOP authentication. */
      ਜੇ(at) {
        /* dupe the timestamp */
        pop3c->apoptimestamp = curlx_memdup0(lt, timestamplen);
        ਜੇ(!pop3c->apoptimestamp)
          ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        /* Store the APOP capability */
        pop3c->authtypes |= POP3_TYPE_APOP;
      }
    }

    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = pop3_perform_capa(ਅੰਕੜੇ, conn);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For CAPA responses */
ਸਥਿਰ CURLcode pop3_state_capa_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ pop3code,
                                     pop3state instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ਅਚਲ ਅੱਖਰ *ਸਤਰ;
  size_t len;

  (ਖਾਲੀ)instate;
  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਸਤਰ = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  /* Do we have an untagged continuation response? */
  ਜੇ(pop3code == '*') {
    /* Does the server support the STLS capability? */
    ਜੇ(len >= 4 && curl_strnequal(ਸਤਰ, "STLS", 4))
      pop3c->tls_supported = TRUE;

    /* Does the server support clear text authentication? */
    ਨਹੀਂਤਾਂ ਜੇ(len >= 4 && curl_strnequal(ਸਤਰ, "USER", 4))
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    /* Does the server support SASL based authentication? */
    ਨਹੀਂਤਾਂ ਜੇ(len >= 5 && curl_strnequal(ਸਤਰ, "SASL ", 5)) {
      pop3c->authtypes |= POP3_TYPE_SASL;

      /* Advance past the SASL keyword */
      ਸਤਰ += 5;
      len -= 5;

      /* Loop through the data line */
      ਲਈ(;;) {
        size_t llen;
        size_t wordlen = 0;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ mechbit;

        ਜਦੋਂ(len && (ISBLANK(*ਸਤਰ) || ISNEWLINE(*ਸਤਰ))) {
          ਸਤਰ++;
          len--;
        }

        ਜੇ(!len)
          ਰੋਕੋ;

        /* Extract the word */
        ਜਦੋਂ(wordlen < len && !ISBLANK(ਸਤਰ[wordlen]) &&
              !ISNEWLINE(ਸਤਰ[wordlen]))
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(ਸਤਰ, wordlen, &llen);
        ਜੇ(mechbit && llen == wordlen)
          pop3c->sasl.authmechs |= mechbit;

        ਸਤਰ += wordlen;
        len -= wordlen;
      }
    }
  }
  ਨਹੀਂਤਾਂ {
    /* Clear text is supported when CAPA is not recognised */
    ਜੇ(pop3code != '+')
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.use_ssl || Curl_conn_is_ssl(conn, FIRSTSOCKET))
      ਨਤੀਜਾ = pop3_perform_authentication(ਅੰਕੜੇ, conn);
    ਨਹੀਂਤਾਂ ਜੇ(pop3code == '+' && pop3c->tls_supported)
      /* Switch to TLS connection now */
      ਨਤੀਜਾ = pop3_perform_starttls(ਅੰਕੜੇ, conn);
    ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.use_ssl <= CURLUSESSL_TRY)
      /* Fallback and carry on with authentication */
      ਨਤੀਜਾ = pop3_perform_authentication(ਅੰਕੜੇ, conn);
    ਨਹੀਂਤਾਂ {
      failf(ਅੰਕੜੇ, "STLS not supported.");
      ਨਤੀਜਾ = CURLE_USE_SSL_FAILED;
    }
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For STARTTLS responses */
ਸਥਿਰ CURLcode pop3_state_starttls_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ connectdata *conn,
                                         ਪੂਰਨ pop3code,
                                         pop3state instate)
{
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  (ਖਾਲੀ)instate;

  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* Pipelining in response is forbidden. */
  ਜੇ(pop3c->pp.ਉਛਾਲ)
    ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;

  ਜੇ(pop3code != '+') {
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.use_ssl != CURLUSESSL_TRY) {
      failf(ਅੰਕੜੇ, "STARTTLS denied");
      ਨਤੀਜਾ = CURLE_USE_SSL_FAILED;
    }
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = pop3_perform_authentication(ਅੰਕੜੇ, conn);
  }
  ਨਹੀਂਤਾਂ
    pop3_state(ਅੰਕੜੇ, POP3_UPGRADETLS);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For SASL authentication responses */
ਸਥਿਰ CURLcode pop3_state_auth_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਪੂਰਨ pop3code,
                                     pop3state instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  saslprogress ਤਰੱਕੀ;

  (ਖਾਲੀ)instate;
  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਨਤੀਜਾ = Curl_sasl_continue(&pop3c->sasl, ਅੰਕੜੇ, pop3code, &ਤਰੱਕੀ);
  ਜੇ(!ਨਤੀਜਾ)
    ਚੋਣ(ਤਰੱਕੀ) {
    ਹਾਲਤ SASL_ਪੂਰਾ:
      pop3_state(ਅੰਕੜੇ, POP3_STOP);  /* Authenticated */
      ਰੋਕੋ;
    ਹਾਲਤ SASL_IDLE:            /* No mechanism left after cancellation */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
      ਜੇ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
        /* Perform APOP authentication */
        ਨਤੀਜਾ = pop3_perform_apop(ਅੰਕੜੇ, conn);
      ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
      ਜੇ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
        /* Perform clear text authentication */
        ਨਤੀਜਾ = pop3_perform_user(ਅੰਕੜੇ, conn);
      ਨਹੀਂਤਾਂ {
        failf(ਅੰਕੜੇ, "Authentication cancelled");
        ਨਤੀਜਾ = CURLE_LOGIN_DENIED;
      }
      ਰੋਕੋ;
    ਮੂਲ:
      ਰੋਕੋ;
    }

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
/* For APOP responses */
ਸਥਿਰ CURLcode pop3_state_apop_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ pop3code,
                                     pop3state instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  (ਖਾਲੀ)instate;

  ਜੇ(pop3code != '+') {
    failf(ਅੰਕੜੇ, "Authentication failed: %d", pop3code);
    ਨਤੀਜਾ = CURLE_LOGIN_DENIED;
  }
  ਨਹੀਂਤਾਂ
    /* End of connect phase */
    pop3_state(ਅੰਕੜੇ, POP3_STOP);

  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

/* For USER responses */
ਸਥਿਰ CURLcode pop3_state_user_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ pop3code,
                                     pop3state instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (ਖਾਲੀ)instate;

  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(pop3code != '+') {
    failf(ਅੰਕੜੇ, "Access denied. %c", pop3code);
    ਨਤੀਜਾ = CURLE_LOGIN_DENIED;
  }
  ਨਹੀਂਤਾਂ
    /* Send the PASS command */
    ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &pop3c->pp, "PASS %s",
                           Curl_creds_passwd(conn->creds));
  ਜੇ(!ਨਤੀਜਾ)
    pop3_state(ਅੰਕੜੇ, POP3_PASS);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For PASS responses */
ਸਥਿਰ CURLcode pop3_state_pass_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ pop3code,
                                     pop3state instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  (ਖਾਲੀ)instate;

  ਜੇ(pop3code != '+') {
    failf(ਅੰਕੜੇ, "Access denied. %c", pop3code);
    ਨਤੀਜਾ = CURLE_LOGIN_DENIED;
  }
  ਨਹੀਂਤਾਂ
    /* End of connect phase */
    pop3_state(ਅੰਕੜੇ, POP3_STOP);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * pop3_write()
 *
 * This function scans the body after the end-of-body and writes everything
 * until the end is found.
 */
ਸਥਿਰ CURLcode pop3_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *str,
                           size_t nread, bool is_eos)
{
  /* This code could be made into a special function in the handler struct */
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  bool strip_dot = FALSE;
  size_t ਆਖਰੀ = 0;
  size_t i;
  (ਖਾਲੀ)is_eos;

  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* Search through the buffer looking for the end-of-body marker which is
     5 bytes (0d 0a 2e 0d 0a). Note that a line starting with a dot matches
     the eob so the server will have prefixed it with an extra dot which we
     need to strip out. Additionally the marker could of course be spread out
     over 5 different data chunks. */
  ਲਈ(i = 0; i < nread; i++) {
    size_t prev = pop3c->eob;

    ਚੋਣ(str[i]) {
    ਹਾਲਤ 0x0d:
      ਜੇ(pop3c->eob == 0) {
        pop3c->eob++;

        ਜੇ(i) {
          /* Write out the body part that did not match */
          ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, &str[ਆਖਰੀ],
                                     i - ਆਖਰੀ);

          ਜੇ(ਨਤੀਜਾ)
            ਵਾਪਸ ਨਤੀਜਾ;

          ਆਖਰੀ = i;
        }
      }
      ਨਹੀਂਤਾਂ ਜੇ(pop3c->eob == 3)
        pop3c->eob++;
      ਨਹੀਂਤਾਂ
        /* If the character match was not at position 0 or 3 then restart the
           pattern matching */
        pop3c->eob = 1;
      ਰੋਕੋ;

    ਹਾਲਤ 0x0a:
      ਜੇ(pop3c->eob == 1 || pop3c->eob == 4)
        pop3c->eob++;
      ਨਹੀਂਤਾਂ
        /* If the character match was not at position 1 or 4 then start the
           search again */
        pop3c->eob = 0;
      ਰੋਕੋ;

    ਹਾਲਤ 0x2e:
      ਜੇ(pop3c->eob == 2)
        pop3c->eob++;
      ਨਹੀਂਤਾਂ ਜੇ(pop3c->eob == 3) {
        /* We have an extra dot after the CRLF which we need to strip off */
        strip_dot = TRUE;
        pop3c->eob = 0;
      }
      ਨਹੀਂਤਾਂ
        /* If the character match was not at position 2 then start the search
           again */
        pop3c->eob = 0;
      ਰੋਕੋ;

    ਮੂਲ:
      pop3c->eob = 0;
      ਰੋਕੋ;
    }

    /* Did we have a partial match which has subsequently failed? */
    ਜੇ(prev && prev >= pop3c->eob) {
      /* Strip can only be non-zero for the first mismatch after CRLF and
         then both prev and strip are equal and nothing will be output below */
      ਜਦੋਂ(prev && pop3c->strip) {
        prev--;
        pop3c->strip--;
      }

      ਜੇ(prev) {
        /* If the partial match was the CRLF and dot then only write the CRLF
           as the server would have inserted the dot */
        ਜੇ(strip_dot && prev - 1 > 0) {
          ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, POP3_EOB,
                                     prev - 1);
        }
        ਨਹੀਂਤਾਂ ਜੇ(!strip_dot) {
          ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, POP3_EOB,
                                     prev);
        }
        ਨਹੀਂਤਾਂ {
          ਨਤੀਜਾ = CURLE_OK;
        }

        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;

        ਆਖਰੀ = i;
        strip_dot = FALSE;
      }
    }
  }

  ਜੇ(pop3c->eob == POP3_EOB_LEN) {
    /* We have a full match so the transfer is done, however we must transfer
    the CRLF at the start of the EOB as this is considered to be part of the
    message as per RFC-1939, sect. 3 */
    ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, POP3_EOB, 2);

    CURL_REQ_CLEAR_RECV(ਅੰਕੜੇ);
    pop3c->eob = 0;

    ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(pop3c->eob)
    /* While EOB is matching nothing should be output */
    ਵਾਪਸ CURLE_OK;

  ਜੇ(nread - ਆਖਰੀ) {
    ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, &str[ਆਖਰੀ],
                               nread - ਆਖਰੀ);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For command responses */
ਸਥਿਰ CURLcode pop3_state_command_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਪੂਰਨ pop3code,
                                        pop3state instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ POP3 *pop3 = Curl_meta_get(ਅੰਕੜੇ, CURL_META_POP3_EASY);
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ਬਣਤਰ pingpong *pp;

  (ਖਾਲੀ)instate;
  ਜੇ(!pop3 || !pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  ਜੇ(pop3code != '+') {
    pop3_state(ਅੰਕੜੇ, POP3_STOP);
    ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
  }

  /* This 'OK' line ends with a CR LF pair which is the two first bytes of the
     EOB string so count this is two matching bytes. This is necessary to make
     the code detect the EOB if the only data than comes now is %2e CR LF like
     when there is no body to return. */
  pop3c->eob = 2;

  /* Since this initial CR LF pair is not part of the actual body, we set
     the strip counter here so that these bytes will not be delivered. */
  pop3c->strip = 2;

  ਜੇ(pop3->transfer == PPTRANSFER_BODY) {
    /* POP3 download */
    Curl_xfer_ਤਿਆਰੀ_recv(ਅੰਕੜੇ, FIRSTSOCKET, -1);

    ਜੇ(pp->ਉਛਾਲ) {
      /* The recv buffer contains data that is actually body content so send
         it as such. Note that there may even be additional "headers" after
         the body */

      /* keep only the overflow */
      curlx_dyn_ਪੂਛ(&pp->recvbuf, pp->ਉਛਾਲ);
      pp->nfinal = 0; /* done */

      ਜੇ(!ਅੰਕੜੇ->req.no_body) {
        ਨਤੀਜਾ = pop3_write(ਅੰਕੜੇ, curlx_dyn_ptr(&pp->recvbuf),
                            curlx_dyn_len(&pp->recvbuf), FALSE);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;
      }

      /* reset the buffer */
      curlx_dyn_reset(&pp->recvbuf);
      pp->ਉਛਾਲ = 0;
    }
  }
  ਨਹੀਂਤਾਂ
    pp->ਉਛਾਲ = 0;

  /* End of DO phase */
  pop3_state(ਅੰਕੜੇ, POP3_STOP);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode pop3_statemachine(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ pop3code;
  ਬਣਤਰ pingpong *pp;
  size_t nread = 0;
  (ਖਾਲੀ)ਅੰਕੜੇ;

  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not POP3 */
upgrade_tls:
  ਜੇ(pop3c->state == POP3_UPGRADETLS) {
    ਨਤੀਜਾ = pop3_perform_upgrade_tls(ਅੰਕੜੇ, conn);
    ਜੇ(ਨਤੀਜਾ || (pop3c->state == POP3_UPGRADETLS))
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* Flush any data that needs to be sent */
  ਜੇ(pp->sendleft)
    ਵਾਪਸ Curl_pp_flushsend(ਅੰਕੜੇ, pp);

  ਕਰੋ {
     /* Read the response from the server */
    ਨਤੀਜਾ = Curl_pp_readresp(ਅੰਕੜੇ, FIRSTSOCKET, pp, &pop3code, &nread);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    ਜੇ(!pop3code)
      ਰੋਕੋ;

    /* We have now received a full POP3 server response */
    ਚੋਣ(pop3c->state) {
    ਹਾਲਤ POP3_SERVERGREET:
      ਨਤੀਜਾ = pop3_state_servergreet_resp(ਅੰਕੜੇ, pop3code, pop3c->state);
      ਰੋਕੋ;

    ਹਾਲਤ POP3_CAPA:
      ਨਤੀਜਾ = pop3_state_capa_resp(ਅੰਕੜੇ, pop3code, pop3c->state);
      ਰੋਕੋ;

    ਹਾਲਤ POP3_STARTTLS:
      ਨਤੀਜਾ = pop3_state_starttls_resp(ਅੰਕੜੇ, conn, pop3code, pop3c->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      ਜੇ(!ਨਤੀਜਾ && (pop3c->state == POP3_UPGRADETLS))
        ਜਾਓ upgrade_tls;
      ਰੋਕੋ;

    ਹਾਲਤ POP3_AUTH:
      ਨਤੀਜਾ = pop3_state_auth_resp(ਅੰਕੜੇ, pop3code, pop3c->state);
      ਰੋਕੋ;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
    ਹਾਲਤ POP3_APOP:
      ਨਤੀਜਾ = pop3_state_apop_resp(ਅੰਕੜੇ, pop3code, pop3c->state);
      ਰੋਕੋ;
#ਜੇ_ਅੰਤ

    ਹਾਲਤ POP3_USER:
      ਨਤੀਜਾ = pop3_state_user_resp(ਅੰਕੜੇ, pop3code, pop3c->state);
      ਰੋਕੋ;

    ਹਾਲਤ POP3_PASS:
      ਨਤੀਜਾ = pop3_state_pass_resp(ਅੰਕੜੇ, pop3code, pop3c->state);
      ਰੋਕੋ;

    ਹਾਲਤ POP3_COMMAND:
      ਨਤੀਜਾ = pop3_state_command_resp(ਅੰਕੜੇ, pop3code, pop3c->state);
      ਰੋਕੋ;

    ਹਾਲਤ POP3_QUIT:
      pop3_state(ਅੰਕੜੇ, POP3_STOP);
      ਰੋਕੋ;

    ਮੂਲ:
      /* internal error */
      pop3_state(ਅੰਕੜੇ, POP3_STOP);
      ਰੋਕੋ;
    }
  } ਜਦੋਂ(!ਨਤੀਜਾ && pop3c->state != POP3_STOP && Curl_pp_moredata(pp));

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Called repeatedly until done from multi.c */
ਸਥਿਰ CURLcode pop3_multi_statemach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਨਤੀਜਾ = Curl_pp_statemach(ਅੰਕੜੇ, &pop3c->pp, FALSE, FALSE);
  *ਪੂਰਾ = (pop3c->state == POP3_STOP);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode pop3_ਖੰਡ_statemach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ connectdata *conn,
                                     bool disconnecting)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜਦੋਂ(pop3c->state != POP3_STOP && !ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_pp_statemach(ਅੰਕੜੇ, &pop3c->pp, TRUE, disconnecting);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For the POP3 "protocol connect" and "doing" phases only */
ਸਥਿਰ CURLcode pop3_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ pop3_conn *pop3c =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_POP3_CONN);
  ਵਾਪਸ pop3c ? Curl_pp_pollset(ਅੰਕੜੇ, &pop3c->pp, ps) : CURLE_OK;
}

/* SASL parameters for the pop3 protocol */
ਸਥਿਰ ਅਚਲ ਬਣਤਰ SASLproto saslpop3 = {
  "pop",                /* The service name */
  pop3_perform_auth,    /* Send authentication command */
  pop3_continue_auth,   /* Send authentication continuation */
  pop3_cancel_auth,     /* Send authentication cancellation */
  pop3_get_ਸੁਨੇਹਾ,     /* Get SASL response message */
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
ਸਥਿਰ CURLcode pop3_connect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ਬਣਤਰ pingpong *pp = pop3c ? &pop3c->pp : ਨਿਰਾ;

  *ਪੂਰਾ = FALSE; /* default to not done yet */
  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  PINGPONG_ਤਿਆਰੀ(pp, pop3_statemachine, pop3_endofresp);

  /* Set the default preferred authentication type and mechanism */
  pop3c->preftype = POP3_TYPE_ANY;
  Curl_sasl_init(&pop3c->sasl, ਅੰਕੜੇ, &saslpop3);

  /* Initialize the pingpong layer */
  Curl_pp_init(pp, Curl_pgrs_now(ਅੰਕੜੇ));

  /* Parse the URL options */
  ਨਤੀਜਾ = pop3_parse_url_options(conn);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* Start off waiting for the server greeting response */
  pop3_state(ਅੰਕੜੇ, POP3_SERVERGREET);

  ਨਤੀਜਾ = pop3_multi_statemach(ਅੰਕੜੇ, ਪੂਰਾ);

  ਵਾਪਸ ਨਤੀਜਾ;
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
ਸਥਿਰ CURLcode pop3_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLcode ਸਥਿਤੀ,
                          bool premature)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ POP3 *pop3 = Curl_meta_get(ਅੰਕੜੇ, CURL_META_POP3_EASY);

  (ਖਾਲੀ)premature;

  ਜੇ(!pop3)
    ਵਾਪਸ CURLE_OK;

  ਜੇ(ਸਥਿਤੀ) {
    CURL_TRC_M(ਅੰਕੜੇ, "POP3 done with bad status");
    connclose(ਅੰਕੜੇ->conn);
    ਨਤੀਜਾ = ਸਥਿਤੀ;         /* use the already set error code */
  }

  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);

  /* Clear the transfer mode for the next request */
  pop3->transfer = PPTRANSFER_BODY;

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * pop3_perform()
 *
 * This is the actual DO function for POP3. Get a message/listing according to
 * the options previously setup.
 */
ਸਥਿਰ CURLcode pop3_perform(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *connected,
                             bool *dophase_ਪੂਰਾ)
{
  /* This is POP3 and no proxy */
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ POP3 *pop3 = Curl_meta_get(ਅੰਕੜੇ, CURL_META_POP3_EASY);

  ਜੇ(!pop3)
    ਵਾਪਸ CURLE_FAILED_INIT;

  DEBUGF(infof(ਅੰਕੜੇ, "DO phase starts"));

  /* Start the first command in the DO phase, may alter data->req.no_body */
  ਨਤੀਜਾ = pop3_perform_command(ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਜੇ(ਅੰਕੜੇ->req.no_body)
    /* Requested no body means no transfer */
    pop3->transfer = PPTRANSFER_INFO;

  *dophase_ਪੂਰਾ = FALSE; /* not done yet */

  /* Run the state-machine */
  ਨਤੀਜਾ = pop3_multi_statemach(ਅੰਕੜੇ, dophase_ਪੂਰਾ);
  *connected = Curl_conn_is_connected(ਅੰਕੜੇ->conn, FIRSTSOCKET);

  ਜੇ(*dophase_ਪੂਰਾ)
    DEBUGF(infof(ਅੰਕੜੇ, "DO phase is complete"));

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Call this when the DO phase has completed */
ਸਥਿਰ CURLcode pop3_dophase_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool connected)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)connected;

  ਵਾਪਸ CURLE_OK;
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
ਸਥਿਰ CURLcode pop3_regular_transfer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      bool *dophase_ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  ਅੰਕੜੇ->req.ਮਾਪ = -1;

  /* Set the progress data */
  Curl_pgrsReset(ਅੰਕੜੇ);

  /* Carry out the perform */
  ਨਤੀਜਾ = pop3_perform(ਅੰਕੜੇ, &connected, dophase_ਪੂਰਾ);

  /* Perform post DO phase operations if necessary */
  ਜੇ(!ਨਤੀਜਾ && *dophase_ਪੂਰਾ)
    ਨਤੀਜਾ = pop3_dophase_ਪੂਰਾ(ਅੰਕੜੇ, connected);

  ਵਾਪਸ ਨਤੀਜਾ;
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
ਸਥਿਰ CURLcode pop3_do(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  *ਪੂਰਾ = FALSE; /* default to false */

  /* Parse the URL path */
  ਨਤੀਜਾ = pop3_parse_url_ਰਸਤਾ(ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* Parse the custom request */
  ਨਤੀਜਾ = pop3_parse_custom_request(ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਨਤੀਜਾ = pop3_regular_transfer(ਅੰਕੜੇ, ਪੂਰਾ);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * pop3_disconnect()
 *
 * Disconnect from an POP3 server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ਸਥਿਰ CURLcode pop3_disconnect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ connectdata *conn, bool dead_connection)
{
  ਬਣਤਰ pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (ਖਾਲੀ)ਅੰਕੜੇ;

  ਜੇ(!pop3c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  ਜੇ(!dead_connection && conn->ਬਿੱਟ.protoconnstart &&
     !Curl_pp_needs_flush(ਅੰਕੜੇ, &pop3c->pp) &&
     !pop3_perform_quit(ਅੰਕੜੇ, conn))
    (ਖਾਲੀ)pop3_ਖੰਡ_statemach(ਅੰਕੜੇ, conn, TRUE); /* ignore errors on QUIT */

  /* Disconnect from the server */
  Curl_pp_disconnect(&pop3c->pp);

  /* Cleanup our connection based variables */
  curlx_safefree(pop3c->apoptimestamp);

  ਵਾਪਸ CURLE_OK;
}

/* Called from multi.c while DOing */
ਸਥਿਰ CURLcode pop3_doing(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *dophase_ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = pop3_multi_statemach(ਅੰਕੜੇ, dophase_ਪੂਰਾ);

  ਜੇ(ਨਤੀਜਾ)
    DEBUGF(infof(ਅੰਕੜੇ, "DO phase failed"));
  ਨਹੀਂਤਾਂ ਜੇ(*dophase_ਪੂਰਾ) {
    ਨਤੀਜਾ = pop3_dophase_ਪੂਰਾ(ਅੰਕੜੇ, FALSE /* not connected */);

    DEBUGF(infof(ਅੰਕੜੇ, "DO phase is complete"));
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ pop3_easy_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ POP3 *pop3 = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  DEBUGASSERT(pop3);
  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);
  curlx_free(pop3);
}

ਸਥਿਰ ਖਾਲੀ pop3_conn_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ pop3_conn *pop3c = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  DEBUGASSERT(pop3c);
  Curl_pp_disconnect(&pop3c->pp);
  curlx_safefree(pop3c->apoptimestamp);
  curlx_free(pop3c);
}

ਸਥਿਰ CURLcode pop3_ਤਿਆਰੀ_connection(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ pop3_conn *pop3c;
  ਬਣਤਰ POP3 *pop3 = curlx_calloc(1, ਆਕਾਰ(*pop3));
  ਜੇ(!pop3 ||
     Curl_meta_ਸੈੱਟ(ਅੰਕੜੇ, CURL_META_POP3_EASY, pop3, pop3_easy_dtor))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  pop3c = curlx_calloc(1, ਆਕਾਰ(*pop3c));
  ਜੇ(!pop3c ||
     Curl_conn_meta_ਸੈੱਟ(conn, CURL_META_POP3_CONN, pop3c, pop3_conn_dtor))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਵਾਪਸ CURLE_OK;
}

/*
 * POP3 protocol.
 */
ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_pop3 = {
  pop3_ਤਿਆਰੀ_connection,            /* setup_connection */
  pop3_do,                          /* do_it */
  pop3_ਪੂਰਾ,                        /* done */
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

#ਜੇ_ਅੰਤ /* CURL_DISABLE_POP3 */
