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
#അന്തര്ഭാവ "curl_setup.h"
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "pop3.h"

#ifndef CURL_DISABLE_POP3

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef __VMS
#അന്തര്ഭാവ <in.h>
#അന്തര്ഭാവ <inet.h>
#endif

#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "pingpong.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "bufref.h"
#അന്തര്ഭാവ "curl_sasl.h"
#അന്തര്ഭാവ "curl_md5.h"
#അന്തര്ഭാവ "curlx/strdup.h"

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
ഘടന POP3 {
  curl_pp_transfer transfer;
  അക്ഷരം *id;               /* Message ID */
  അക്ഷരം *custom;           /* Custom Request */
};

/*
 * POP3 connection state
 */
തരനിർവചനം എണ്ണൽ {
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

ഘടന pop3_conn {
  ഘടന pingpong pp;
  pop3state state;        /* Always use pop3.c:state() to change state! */
  size_t eob;             /* Number of bytes of the EOB (End Of Body) that
                             have been received so far */
  size_t strip;           /* Number of bytes from the start to ignore as
                             non-body */
  ഘടന SASL sasl;       /* SASL-related storage */
  അക്ഷരം *apoptimestamp;    /* APOP timestamp from the server greeting */
  unsigned അക്ഷരം authtypes; /* Accepted authentication types */
  unsigned അക്ഷരം preftype;  /* Preferred authentication type */
  BIT(ssldone);           /* Is connect() over SSL done? */
  BIT(tls_supported);     /* StartTLS capability supported by server */
};

ഘടന pop3_cmd {
  സ്ഥിരം അക്ഷരം *name;
  unsigned short nlen;
  BIT(multiline); /* response is multi-line with last '.' line */
  BIT(multiline_with_args); /* is multi-line when command has args */
};

സ്ഥിതം സ്ഥിരം ഘടന pop3_cmd pop3cmds[] = {
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
സ്ഥിതം CURLcode pop3_parse_url_options(ഘടന connectdata *conn)
{
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  സ്ഥിരം അക്ഷരം *ptr = conn->options;

  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  ആകുമ്പോൾ(!result && ptr && *ptr) {
    സ്ഥിരം അക്ഷരം *താക്കോൽ = ptr;
    സ്ഥിരം അക്ഷരം *മൂല്യം;

    ആകുമ്പോൾ(*ptr && *ptr != '=')
      ptr++;

    മൂല്യം = ptr + 1;

    ആകുമ്പോൾ(*ptr && *ptr != ';')
      ptr++;

    എങ്കിൽ(curl_strnequal(താക്കോൽ, "AUTH=", 5)) {
      result = Curl_sasl_parse_url_auth_option(&pop3c->sasl,
                                               മൂല്യം, ptr - മൂല്യം);

      എങ്കിൽ(result && curl_strnequal(മൂല്യം, "+APOP", ptr - മൂല്യം)) {
        pop3c->preftype = POP3_TYPE_APOP;
        pop3c->sasl.prefmech = SASL_AUTH_NONE;
        result = CURLE_OK;
      }
    }
    അല്ലെങ്കിൽ
      result = CURLE_URL_MALFORMAT;

    എങ്കിൽ(*ptr == ';')
      ptr++;
  }

  എങ്കിൽ(pop3c->preftype != POP3_TYPE_APOP)
    തിരഞ്ഞെടുപ്പ്(pop3c->sasl.prefmech) {
    അവസ്ഥ SASL_AUTH_NONE:
      pop3c->preftype = POP3_TYPE_NONE;
      നിർത്തുക;
    അവസ്ഥ SASL_AUTH_DEFAULT:
      pop3c->preftype = POP3_TYPE_ANY;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      pop3c->preftype = POP3_TYPE_SASL;
      നിർത്തുക;
    }

  തിരികെ result;
}

/***********************************************************************
 *
 * pop3_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
സ്ഥിതം CURLcode pop3_parse_url_path(ഘടന Curl_easy *data)
{
  /* The POP3 struct is already initialized in pop3_connect() */
  ഘടന POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  സ്ഥിരം അക്ഷരം *path = &data->state.up.path[1]; /* skip leading path */

  എങ്കിൽ(!pop3)
    തിരികെ CURLE_FAILED_INIT;
  /* URL decode the path for the message ID */
  തിരികെ Curl_urldecode(path, 0, &pop3->id, ശൂന്യ, REJECT_CTRL);
}

/***********************************************************************
 *
 * pop3_parse_custom_request()
 *
 * Parse the custom request.
 */
സ്ഥിതം CURLcode pop3_parse_custom_request(ഘടന Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ഘടന POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  സ്ഥിരം അക്ഷരം *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  എങ്കിൽ(!pop3)
    തിരികെ CURLE_FAILED_INIT;
  /* URL decode the custom request */
  എങ്കിൽ(custom)
    result = Curl_urldecode(custom, 0, &pop3->custom, ശൂന്യ, REJECT_CTRL);

  തിരികെ result;
}

/* Return iff a command is defined as "multi-line" (RFC 1939),
 * has a response terminated by a last line with a '.'.
 */
സ്ഥിതം bool pop3_is_multiline(സ്ഥിരം അക്ഷരം *cmdline)
{
  size_t i;
  വേണ്ടി(i = 0; i < CURL_ARRAYSIZE(pop3cmds); ++i) {
    എങ്കിൽ(curl_strnequal(pop3cmds[i].name, cmdline, pop3cmds[i].nlen)) {
      എങ്കിൽ(!cmdline[pop3cmds[i].nlen])
        തിരികെ (bool)pop3cmds[i].multiline;
      അല്ലെങ്കിൽ എങ്കിൽ(cmdline[pop3cmds[i].nlen] == ' ')
        തിരികെ (bool)pop3cmds[i].multiline_with_args;
    }
  }
  /* Unknown command, assume multi-line for backward compatibility with
   * earlier curl versions that only could do multi-line responses. */
  തിരികെ TRUE;
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
സ്ഥിതം bool pop3_endofresp(ഘടന Curl_easy *data, ഘടന connectdata *conn,
                           സ്ഥിരം അക്ഷരം *വരി, size_t len, പൂർണ്ണസംഖ്യ *resp)
{
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (ശൂന്യം)data;
  DEBUGASSERT(pop3c);
  എങ്കിൽ(!pop3c) /* internal error */
    തിരികെ TRUE;

  /* Do we have an error response? */
  എങ്കിൽ(len >= 4 && !memcmp("-ERR", വരി, 4)) {
    *resp = '-';

    തിരികെ TRUE;
  }

  /* Are we processing CAPA command responses? */
  എങ്കിൽ(pop3c->state == POP3_CAPA) {
    /* Do we have the terminating line? Per RFC 2449 this is a line
       containing only a single dot */
    എങ്കിൽ((len == 3 && വരി[0] == '.' && വരി[1] == '\r') ||
       (len == 2 && വരി[0] == '.' && വരി[1] == '\n'))
      /* Treat the response as a success */
      *resp = '+';
    അല്ലെങ്കിൽ
      /* Treat the response as an untagged continuation */
      *resp = '*';

    തിരികെ TRUE;
  }

  /* Do we have a success response? */
  എങ്കിൽ(len >= 3 && !memcmp("+OK", വരി, 3)) {
    *resp = '+';

    തിരികെ TRUE;
  }

  /* Do we have a continuation response? */
  എങ്കിൽ(len >= 1 && വരി[0] == '+') {
    *resp = '*';

    തിരികെ TRUE;
  }

  തിരികെ FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * pop3_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
സ്ഥിതം CURLcode pop3_get_message(ഘടന Curl_easy *data, ഘടന bufref *out)
{
  ഘടന pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  അക്ഷരം *message;
  size_t len;

  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;
  message = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;
  എങ്കിൽ(len > 2) {
    /* Find the start of the message */
    len -= 2;
    വേണ്ടി(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    ആകുമ്പോൾ(len--)
      എങ്കിൽ(!ISBLANK(message[len]) && !ISNEWLINE(message[len]))
        നിർത്തുക;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, ശൂന്യ);
  }
  അല്ലെങ്കിൽ
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, ശൂന്യ);

  തിരികെ CURLE_OK;
}

/***********************************************************************
 *
 * pop3_state()
 *
 * This is the ONLY way to change POP3 state!
 */
സ്ഥിതം ശൂന്യം pop3_state(ഘടന Curl_easy *data, pop3state newstate)
{
  ഘടന pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  എങ്കിൽ(pop3c) {
#എങ്കിൽ defined(DEBUGBUILD) && defined(CURLVERBOSE)
    /* for debug purposes */
    സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം names[] = {
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

    എങ്കിൽ(pop3c->state != newstate)
      infof(data, "POP3 %p state change from %s to %s",
            (ശൂന്യം *)pop3c, names[pop3c->state], names[newstate]);
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
സ്ഥിതം CURLcode pop3_perform_capa(ഘടന Curl_easy *data,
                                  ഘടന connectdata *conn)
{
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  pop3c->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  pop3c->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  pop3c->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPA command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "CAPA");

  എങ്കിൽ(!result)
    pop3_state(data, POP3_CAPA);

  തിരികെ result;
}

/***********************************************************************
 *
 * pop3_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
സ്ഥിതം CURLcode pop3_perform_starttls(ഘടന Curl_easy *data,
                                      ഘടന connectdata *conn)
{
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;

  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  /* Send the STLS command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "STLS");
  എങ്കിൽ(!result)
    pop3_state(data, POP3_STARTTLS);

  തിരികെ result;
}

/***********************************************************************
 *
 * pop3_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
സ്ഥിതം CURLcode pop3_perform_upgrade_tls(ഘടന Curl_easy *data,
                                         ഘടന connectdata *conn)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;
  bool ssldone = FALSE;

  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    എങ്കിൽ(result)
      പോകുക out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_pop3s;
  }

  DEBUGASSERT(!pop3c->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "pop3_perform_upgrade_tls, connect -> %d, %d",
               (പൂർണ്ണസംഖ്യ)result, ssldone));
  എങ്കിൽ(!result && ssldone) {
    pop3c->ssldone = ssldone;
    /* perform CAPA now, changes pop3c->state out of POP3_UPGRADETLS */
    result = pop3_perform_capa(data, conn);
  }
out:
  തിരികെ result;
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
  (ശൂന്യം)conn;
  തിരികെ CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * pop3_perform_user()
 *
 * Sends a clear text USER command to authenticate with.
 */
സ്ഥിതം CURLcode pop3_perform_user(ഘടന Curl_easy *data,
                                  ഘടന connectdata *conn)
{
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  എങ്കിൽ(!conn->creds) {
    pop3_state(data, POP3_STOP);

    തിരികെ result;
  }

  /* Send the USER command */
  result = Curl_pp_sendf(data, &pop3c->pp, "USER %s",
                         Curl_creds_user(conn->creds));
  എങ്കിൽ(!result)
    pop3_state(data, POP3_USER);

  തിരികെ result;
}

#ifndef CURL_DISABLE_DIGEST_AUTH
/***********************************************************************
 *
 * pop3_perform_apop()
 *
 * Sends an APOP command to authenticate with.
 */
സ്ഥിതം CURLcode pop3_perform_apop(ഘടന Curl_easy *data,
                                  ഘടന connectdata *conn)
{
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  size_t i;
  ഘടന MD5_context *ctxt;
  unsigned അക്ഷരം digest[MD5_DIGEST_LEN];
  അക്ഷരം secret[(2 * MD5_DIGEST_LEN) + 1];

  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  എങ്കിൽ(!data->state.creds) {
    pop3_state(data, POP3_STOP);

    തിരികെ result;
  }

  /* Create the digest */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  എങ്കിൽ(!ctxt)
    തിരികെ CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)pop3c->apoptimestamp,
                  curlx_uztoui(strlen(pop3c->apoptimestamp)));

  Curl_MD5_update(ctxt, (സ്ഥിരം unsigned അക്ഷരം *)Curl_creds_passwd(conn->creds),
                  curlx_uztoui(strlen(Curl_creds_passwd(conn->creds))));

  /* Finalise the digest */
  Curl_MD5_final(ctxt, digest);

  /* Convert the calculated 16 octet digest into a 32-byte hex string */
  വേണ്ടി(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&secret[2 * i], 3, "%02x", digest[i]);

  result = Curl_pp_sendf(data, &pop3c->pp, "APOP %s %s",
                         Curl_creds_user(conn->creds), secret);

  എങ്കിൽ(!result)
    pop3_state(data, POP3_APOP);

  തിരികെ result;
}
#endif

/***********************************************************************
 *
 * pop3_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
സ്ഥിതം CURLcode pop3_perform_auth(ഘടന Curl_easy *data,
                                  സ്ഥിരം അക്ഷരം *mech,
                                  സ്ഥിരം ഘടന bufref *initresp)
{
  ഘടന pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  സ്ഥിരം അക്ഷരം *ir = Curl_bufref_ptr(initresp);

  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    result = Curl_pp_sendf(data, &pop3c->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  അല്ലെങ്കിൽ {
    /* Send the AUTH command */
    result = Curl_pp_sendf(data, &pop3c->pp, "AUTH %s", mech);
  }

  തിരികെ result;
}

/***********************************************************************
 *
 * pop3_continue_auth()
 *
 * Sends SASL continuation data.
 */
സ്ഥിതം CURLcode pop3_continue_auth(ഘടന Curl_easy *data,
                                   സ്ഥിരം അക്ഷരം *mech,
                                   സ്ഥിരം ഘടന bufref *resp)
{
  ഘടന pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);

  (ശൂന്യം)mech;
  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  തിരികെ Curl_pp_sendf(data, &pop3c->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * pop3_cancel_auth()
 *
 * Sends SASL cancellation.
 */
സ്ഥിതം CURLcode pop3_cancel_auth(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *mech)
{
  ഘടന pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);

  (ശൂന്യം)mech;
  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  തിരികെ Curl_pp_sendf(data, &pop3c->pp, "*");
}

/***********************************************************************
 *
 * pop3_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to APOP and clear text should a
 * common mechanism not be available between the client and server.
 */
സ്ഥിതം CURLcode pop3_perform_authentication(ഘടന Curl_easy *data,
                                            ഘടന connectdata *conn)
{
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  saslprogress progress = SASL_IDLE;

  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  /* Check we have enough data to authenticate with and end the
     connect phase if we do not */
  എങ്കിൽ(!Curl_sasl_can_authenticate(&pop3c->sasl, data)) {
    pop3_state(data, POP3_STOP);
    തിരികെ result;
  }

  എങ്കിൽ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_SASL) {
    /* Calculate the SASL login details */
    result = Curl_sasl_start(&pop3c->sasl, data, FALSE, &progress);

    എങ്കിൽ(!result && progress == SASL_INPROGRESS)
      pop3_state(data, POP3_AUTH);
  }

  എങ്കിൽ(!result && progress == SASL_IDLE) {
#ifndef CURL_DISABLE_DIGEST_AUTH
    എങ്കിൽ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
      /* Perform APOP authentication */
      result = pop3_perform_apop(data, conn);
    അല്ലെങ്കിൽ
#endif
    എങ്കിൽ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
      /* Perform clear text authentication */
      result = pop3_perform_user(data, conn);
    അല്ലെങ്കിൽ
      result = Curl_sasl_is_blocked(&pop3c->sasl, data);
  }

  തിരികെ result;
}

/***********************************************************************
 *
 * pop3_perform_command()
 *
 * Sends a POP3 based command.
 */
സ്ഥിതം CURLcode pop3_perform_command(ഘടന Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;
  ഘടന POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  സ്ഥിരം അക്ഷരം *command = ശൂന്യ;

  എങ്കിൽ(!pop3 || !pop3c)
    തിരികെ CURLE_FAILED_INIT;

  /* Calculate the default command */
  എങ്കിൽ(pop3->id[0] == '\0' || data->set.list_only) {
    command = "LIST";

    എങ്കിൽ(pop3->id[0] != '\0')
      /* Message specific LIST so skip the BODY transfer */
      pop3->transfer = PPTRANSFER_INFO;
  }
  അല്ലെങ്കിൽ
    command = "RETR";

  എങ്കിൽ(pop3->custom && pop3->custom[0] != '\0')
    command = pop3->custom;

  /* Send the command */
  എങ്കിൽ(pop3->id[0] != '\0')
    result = Curl_pp_sendf(data, &pop3c->pp, "%s %s", command, pop3->id);
  അല്ലെങ്കിൽ
    result = Curl_pp_sendf(data, &pop3c->pp, "%s", command);

  എങ്കിൽ(!result) {
    pop3_state(data, POP3_COMMAND);
    data->req.no_body = !pop3_is_multiline(command);
  }

  തിരികെ result;
}

/***********************************************************************
 *
 * pop3_perform_quit()
 *
 * Performs the quit action prior to sclose() be called.
 */
സ്ഥിതം CURLcode pop3_perform_quit(ഘടന Curl_easy *data,
                                  ഘടന connectdata *conn)
{
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;

  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  /* Send the QUIT command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "QUIT");
  എങ്കിൽ(!result)
    pop3_state(data, POP3_QUIT);

  തിരികെ result;
}

/* For the initial server greeting */
സ്ഥിതം CURLcode pop3_state_servergreet_resp(ഘടന Curl_easy *data,
                                            പൂർണ്ണസംഖ്യ pop3code,
                                            pop3state instate)
{
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  സ്ഥിരം അക്ഷരം *വരി;
  size_t len;

  (ശൂന്യം)instate;
  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  വരി = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  എങ്കിൽ(pop3code != '+') {
    failf(data, "Got unexpected pop3-server response");
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(len > 3) {
    /* Does the server support APOP authentication? */
    സ്ഥിരം അക്ഷരം *lt;
    സ്ഥിരം അക്ഷരം *gt = ശൂന്യ;

    /* Look for the APOP timestamp */
    lt = memchr(വരി, '<', len);
    എങ്കിൽ(lt)
      /* search the remainder for '>' */
      gt = memchr(lt, '>', len - (lt - വരി));
    എങ്കിൽ(gt) {
      /* the length of the timestamp, including the brackets */
      size_t timestamplen = gt - lt + 1;
      സ്ഥിരം അക്ഷരം *at = memchr(lt, '@', timestamplen);
      /* If the timestamp does not contain '@' it is not (as required by
         RFC-1939) conformant to the RFC-822 message id syntax, and we
         therefore do not use APOP authentication. */
      എങ്കിൽ(at) {
        /* dupe the timestamp */
        pop3c->apoptimestamp = curlx_memdup0(lt, timestamplen);
        എങ്കിൽ(!pop3c->apoptimestamp)
          തിരികെ CURLE_OUT_OF_MEMORY;
        /* Store the APOP capability */
        pop3c->authtypes |= POP3_TYPE_APOP;
      }
    }

    എങ്കിൽ(!result)
      result = pop3_perform_capa(data, conn);
  }

  തിരികെ result;
}

/* For CAPA responses */
സ്ഥിതം CURLcode pop3_state_capa_resp(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  സ്ഥിരം അക്ഷരം *വരി;
  size_t len;

  (ശൂന്യം)instate;
  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  വരി = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  /* Do we have an untagged continuation response? */
  എങ്കിൽ(pop3code == '*') {
    /* Does the server support the STLS capability? */
    എങ്കിൽ(len >= 4 && curl_strnequal(വരി, "STLS", 4))
      pop3c->tls_supported = TRUE;

    /* Does the server support clear text authentication? */
    അല്ലെങ്കിൽ എങ്കിൽ(len >= 4 && curl_strnequal(വരി, "USER", 4))
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    /* Does the server support SASL based authentication? */
    അല്ലെങ്കിൽ എങ്കിൽ(len >= 5 && curl_strnequal(വരി, "SASL ", 5)) {
      pop3c->authtypes |= POP3_TYPE_SASL;

      /* Advance past the SASL keyword */
      വരി += 5;
      len -= 5;

      /* Loop through the data line */
      വേണ്ടി(;;) {
        size_t llen;
        size_t wordlen = 0;
        unsigned short mechbit;

        ആകുമ്പോൾ(len && (ISBLANK(*വരി) || ISNEWLINE(*വരി))) {
          വരി++;
          len--;
        }

        എങ്കിൽ(!len)
          നിർത്തുക;

        /* Extract the word */
        ആകുമ്പോൾ(wordlen < len && !ISBLANK(വരി[wordlen]) &&
              !ISNEWLINE(വരി[wordlen]))
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(വരി, wordlen, &llen);
        എങ്കിൽ(mechbit && llen == wordlen)
          pop3c->sasl.authmechs |= mechbit;

        വരി += wordlen;
        len -= wordlen;
      }
    }
  }
  അല്ലെങ്കിൽ {
    /* Clear text is supported when CAPA is not recognised */
    എങ്കിൽ(pop3code != '+')
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    എങ്കിൽ(!data->set.use_ssl || Curl_conn_is_ssl(conn, FIRSTSOCKET))
      result = pop3_perform_authentication(data, conn);
    അല്ലെങ്കിൽ എങ്കിൽ(pop3code == '+' && pop3c->tls_supported)
      /* Switch to TLS connection now */
      result = pop3_perform_starttls(data, conn);
    അല്ലെങ്കിൽ എങ്കിൽ(data->set.use_ssl <= CURLUSESSL_TRY)
      /* Fallback and carry on with authentication */
      result = pop3_perform_authentication(data, conn);
    അല്ലെങ്കിൽ {
      failf(data, "STLS not supported.");
      result = CURLE_USE_SSL_FAILED;
    }
  }

  തിരികെ result;
}

/* For STARTTLS responses */
സ്ഥിതം CURLcode pop3_state_starttls_resp(ഘടന Curl_easy *data,
                                         ഘടന connectdata *conn,
                                         പൂർണ്ണസംഖ്യ pop3code,
                                         pop3state instate)
{
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  (ശൂന്യം)instate;

  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  /* Pipelining in response is forbidden. */
  എങ്കിൽ(pop3c->pp.overflow)
    തിരികെ CURLE_WEIRD_SERVER_REPLY;

  എങ്കിൽ(pop3code != '+') {
    എങ്കിൽ(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied");
      result = CURLE_USE_SSL_FAILED;
    }
    അല്ലെങ്കിൽ
      result = pop3_perform_authentication(data, conn);
  }
  അല്ലെങ്കിൽ
    pop3_state(data, POP3_UPGRADETLS);

  തിരികെ result;
}

/* For SASL authentication responses */
സ്ഥിതം CURLcode pop3_state_auth_resp(ഘടന Curl_easy *data,
                                     പൂർണ്ണസംഖ്യ pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  saslprogress progress;

  (ശൂന്യം)instate;
  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  result = Curl_sasl_continue(&pop3c->sasl, data, pop3code, &progress);
  എങ്കിൽ(!result)
    തിരഞ്ഞെടുപ്പ്(progress) {
    അവസ്ഥ SASL_DONE:
      pop3_state(data, POP3_STOP);  /* Authenticated */
      നിർത്തുക;
    അവസ്ഥ SASL_IDLE:            /* No mechanism left after cancellation */
#ifndef CURL_DISABLE_DIGEST_AUTH
      എങ്കിൽ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
        /* Perform APOP authentication */
        result = pop3_perform_apop(data, conn);
      അല്ലെങ്കിൽ
#endif
      എങ്കിൽ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
        /* Perform clear text authentication */
        result = pop3_perform_user(data, conn);
      അല്ലെങ്കിൽ {
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
      }
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      നിർത്തുക;
    }

  തിരികെ result;
}

#ifndef CURL_DISABLE_DIGEST_AUTH
/* For APOP responses */
സ്ഥിതം CURLcode pop3_state_apop_resp(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  (ശൂന്യം)instate;

  എങ്കിൽ(pop3code != '+') {
    failf(data, "Authentication failed: %d", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  അല്ലെങ്കിൽ
    /* End of connect phase */
    pop3_state(data, POP3_STOP);

  തിരികെ result;
}
#endif

/* For USER responses */
സ്ഥിതം CURLcode pop3_state_user_resp(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (ശൂന്യം)instate;

  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ(pop3code != '+') {
    failf(data, "Access denied. %c", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  അല്ലെങ്കിൽ
    /* Send the PASS command */
    result = Curl_pp_sendf(data, &pop3c->pp, "PASS %s",
                           Curl_creds_passwd(conn->creds));
  എങ്കിൽ(!result)
    pop3_state(data, POP3_PASS);

  തിരികെ result;
}

/* For PASS responses */
സ്ഥിതം CURLcode pop3_state_pass_resp(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  (ശൂന്യം)instate;

  എങ്കിൽ(pop3code != '+') {
    failf(data, "Access denied. %c", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  അല്ലെങ്കിൽ
    /* End of connect phase */
    pop3_state(data, POP3_STOP);

  തിരികെ result;
}

/***********************************************************************
 *
 * pop3_write()
 *
 * This function scans the body after the end-of-body and writes everything
 * until the end is found.
 */
സ്ഥിതം CURLcode pop3_write(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *str,
                           size_t nread, bool is_eos)
{
  /* This code could be made into a special function in the handler struct */
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  bool strip_dot = FALSE;
  size_t last = 0;
  size_t i;
  (ശൂന്യം)is_eos;

  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  /* Search through the buffer looking for the end-of-body marker which is
     5 bytes (0d 0a 2e 0d 0a). Note that a line starting with a dot matches
     the eob so the server will have prefixed it with an extra dot which we
     need to strip out. Additionally the marker could of course be spread out
     over 5 different data chunks. */
  വേണ്ടി(i = 0; i < nread; i++) {
    size_t prev = pop3c->eob;

    തിരഞ്ഞെടുപ്പ്(str[i]) {
    അവസ്ഥ 0x0d:
      എങ്കിൽ(pop3c->eob == 0) {
        pop3c->eob++;

        എങ്കിൽ(i) {
          /* Write out the body part that did not match */
          result = Curl_client_write(data, CLIENTWRITE_BODY, &str[last],
                                     i - last);

          എങ്കിൽ(result)
            തിരികെ result;

          last = i;
        }
      }
      അല്ലെങ്കിൽ എങ്കിൽ(pop3c->eob == 3)
        pop3c->eob++;
      അല്ലെങ്കിൽ
        /* If the character match was not at position 0 or 3 then restart the
           pattern matching */
        pop3c->eob = 1;
      നിർത്തുക;

    അവസ്ഥ 0x0a:
      എങ്കിൽ(pop3c->eob == 1 || pop3c->eob == 4)
        pop3c->eob++;
      അല്ലെങ്കിൽ
        /* If the character match was not at position 1 or 4 then start the
           search again */
        pop3c->eob = 0;
      നിർത്തുക;

    അവസ്ഥ 0x2e:
      എങ്കിൽ(pop3c->eob == 2)
        pop3c->eob++;
      അല്ലെങ്കിൽ എങ്കിൽ(pop3c->eob == 3) {
        /* We have an extra dot after the CRLF which we need to strip off */
        strip_dot = TRUE;
        pop3c->eob = 0;
      }
      അല്ലെങ്കിൽ
        /* If the character match was not at position 2 then start the search
           again */
        pop3c->eob = 0;
      നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
      pop3c->eob = 0;
      നിർത്തുക;
    }

    /* Did we have a partial match which has subsequently failed? */
    എങ്കിൽ(prev && prev >= pop3c->eob) {
      /* Strip can only be non-zero for the first mismatch after CRLF and
         then both prev and strip are equal and nothing will be output below */
      ആകുമ്പോൾ(prev && pop3c->strip) {
        prev--;
        pop3c->strip--;
      }

      എങ്കിൽ(prev) {
        /* If the partial match was the CRLF and dot then only write the CRLF
           as the server would have inserted the dot */
        എങ്കിൽ(strip_dot && prev - 1 > 0) {
          result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB,
                                     prev - 1);
        }
        അല്ലെങ്കിൽ എങ്കിൽ(!strip_dot) {
          result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB,
                                     prev);
        }
        അല്ലെങ്കിൽ {
          result = CURLE_OK;
        }

        എങ്കിൽ(result)
          തിരികെ result;

        last = i;
        strip_dot = FALSE;
      }
    }
  }

  എങ്കിൽ(pop3c->eob == POP3_EOB_LEN) {
    /* We have a full match so the transfer is done, however we must transfer
    the CRLF at the start of the EOB as this is considered to be part of the
    message as per RFC-1939, sect. 3 */
    result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB, 2);

    CURL_REQ_CLEAR_RECV(data);
    pop3c->eob = 0;

    തിരികെ result;
  }

  എങ്കിൽ(pop3c->eob)
    /* While EOB is matching nothing should be output */
    തിരികെ CURLE_OK;

  എങ്കിൽ(nread - last) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, &str[last],
                               nread - last);
  }

  തിരികെ result;
}

/* For command responses */
സ്ഥിതം CURLcode pop3_state_command_resp(ഘടന Curl_easy *data,
                                        പൂർണ്ണസംഖ്യ pop3code,
                                        pop3state instate)
{
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;
  ഘടന POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ഘടന pingpong *pp;

  (ശൂന്യം)instate;
  എങ്കിൽ(!pop3 || !pop3c)
    തിരികെ CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  എങ്കിൽ(pop3code != '+') {
    pop3_state(data, POP3_STOP);
    തിരികെ CURLE_WEIRD_SERVER_REPLY;
  }

  /* This 'OK' line ends with a CR LF pair which is the two first bytes of the
     EOB string so count this is two matching bytes. This is necessary to make
     the code detect the EOB if the only data than comes now is %2e CR LF like
     when there is no body to return. */
  pop3c->eob = 2;

  /* Since this initial CR LF pair is not part of the actual body, we set
     the strip counter here so that these bytes will not be delivered. */
  pop3c->strip = 2;

  എങ്കിൽ(pop3->transfer == PPTRANSFER_BODY) {
    /* POP3 download */
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);

    എങ്കിൽ(pp->overflow) {
      /* The recv buffer contains data that is actually body content so send
         it as such. Note that there may even be additional "headers" after
         the body */

      /* keep only the overflow */
      curlx_dyn_tail(&pp->recvbuf, pp->overflow);
      pp->nfinal = 0; /* done */

      എങ്കിൽ(!data->req.no_body) {
        result = pop3_write(data, curlx_dyn_ptr(&pp->recvbuf),
                            curlx_dyn_len(&pp->recvbuf), FALSE);
        എങ്കിൽ(result)
          തിരികെ result;
      }

      /* reset the buffer */
      curlx_dyn_reset(&pp->recvbuf);
      pp->overflow = 0;
    }
  }
  അല്ലെങ്കിൽ
    pp->overflow = 0;

  /* End of DO phase */
  pop3_state(data, POP3_STOP);

  തിരികെ result;
}

സ്ഥിതം CURLcode pop3_statemachine(ഘടന Curl_easy *data,
                                  ഘടന connectdata *conn)
{
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ pop3code;
  ഘടന pingpong *pp;
  size_t nread = 0;
  (ശൂന്യം)data;

  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not POP3 */
upgrade_tls:
  എങ്കിൽ(pop3c->state == POP3_UPGRADETLS) {
    result = pop3_perform_upgrade_tls(data, conn);
    എങ്കിൽ(result || (pop3c->state == POP3_UPGRADETLS))
      തിരികെ result;
  }

  /* Flush any data that needs to be sent */
  എങ്കിൽ(pp->sendleft)
    തിരികെ Curl_pp_flushsend(data, pp);

  ചെയ്യുക {
     /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, pp, &pop3code, &nread);
    എങ്കിൽ(result)
      തിരികെ result;

    എങ്കിൽ(!pop3code)
      നിർത്തുക;

    /* We have now received a full POP3 server response */
    തിരഞ്ഞെടുപ്പ്(pop3c->state) {
    അവസ്ഥ POP3_SERVERGREET:
      result = pop3_state_servergreet_resp(data, pop3code, pop3c->state);
      നിർത്തുക;

    അവസ്ഥ POP3_CAPA:
      result = pop3_state_capa_resp(data, pop3code, pop3c->state);
      നിർത്തുക;

    അവസ്ഥ POP3_STARTTLS:
      result = pop3_state_starttls_resp(data, conn, pop3code, pop3c->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      എങ്കിൽ(!result && (pop3c->state == POP3_UPGRADETLS))
        പോകുക upgrade_tls;
      നിർത്തുക;

    അവസ്ഥ POP3_AUTH:
      result = pop3_state_auth_resp(data, pop3code, pop3c->state);
      നിർത്തുക;

#ifndef CURL_DISABLE_DIGEST_AUTH
    അവസ്ഥ POP3_APOP:
      result = pop3_state_apop_resp(data, pop3code, pop3c->state);
      നിർത്തുക;
#endif

    അവസ്ഥ POP3_USER:
      result = pop3_state_user_resp(data, pop3code, pop3c->state);
      നിർത്തുക;

    അവസ്ഥ POP3_PASS:
      result = pop3_state_pass_resp(data, pop3code, pop3c->state);
      നിർത്തുക;

    അവസ്ഥ POP3_COMMAND:
      result = pop3_state_command_resp(data, pop3code, pop3c->state);
      നിർത്തുക;

    അവസ്ഥ POP3_QUIT:
      pop3_state(data, POP3_STOP);
      നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
      /* internal error */
      pop3_state(data, POP3_STOP);
      നിർത്തുക;
    }
  } ആകുമ്പോൾ(!result && pop3c->state != POP3_STOP && Curl_pp_moredata(pp));

  തിരികെ result;
}

/* Called repeatedly until done from multi.c */
സ്ഥിതം CURLcode pop3_multi_statemach(ഘടന Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;
  result = Curl_pp_statemach(data, &pop3c->pp, FALSE, FALSE);
  *done = (pop3c->state == POP3_STOP);

  തിരികെ result;
}

സ്ഥിതം CURLcode pop3_block_statemach(ഘടന Curl_easy *data,
                                     ഘടന connectdata *conn,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  ആകുമ്പോൾ(pop3c->state != POP3_STOP && !result)
    result = Curl_pp_statemach(data, &pop3c->pp, TRUE, disconnecting);

  തിരികെ result;
}

/* For the POP3 "protocol connect" and "doing" phases only */
സ്ഥിതം CURLcode pop3_pollset(ഘടന Curl_easy *data,
                             ഘടന easy_pollset *ps)
{
  ഘടന pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  തിരികെ pop3c ? Curl_pp_pollset(data, &pop3c->pp, ps) : CURLE_OK;
}

/* SASL parameters for the pop3 protocol */
സ്ഥിതം സ്ഥിരം ഘടന SASLproto saslpop3 = {
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
സ്ഥിതം CURLcode pop3_connect(ഘടന Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ഘടന pingpong *pp = pop3c ? &pop3c->pp : ശൂന്യ;

  *done = FALSE; /* default to not done yet */
  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  PINGPONG_SETUP(pp, pop3_statemachine, pop3_endofresp);

  /* Set the default preferred authentication type and mechanism */
  pop3c->preftype = POP3_TYPE_ANY;
  Curl_sasl_init(&pop3c->sasl, data, &saslpop3);

  /* Initialize the pingpong layer */
  Curl_pp_init(pp, Curl_pgrs_now(data));

  /* Parse the URL options */
  result = pop3_parse_url_options(conn);
  എങ്കിൽ(result)
    തിരികെ result;

  /* Start off waiting for the server greeting response */
  pop3_state(data, POP3_SERVERGREET);

  result = pop3_multi_statemach(data, done);

  തിരികെ result;
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
സ്ഥിതം CURLcode pop3_done(ഘടന Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  ഘടന POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);

  (ശൂന്യം)premature;

  എങ്കിൽ(!pop3)
    തിരികെ CURLE_OK;

  എങ്കിൽ(status) {
    CURL_TRC_M(data, "POP3 done with bad status");
    connclose(data->conn);
    result = status;         /* use the already set error code */
  }

  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);

  /* Clear the transfer mode for the next request */
  pop3->transfer = PPTRANSFER_BODY;

  തിരികെ result;
}

/***********************************************************************
 *
 * pop3_perform()
 *
 * This is the actual DO function for POP3. Get a message/listing according to
 * the options previously setup.
 */
സ്ഥിതം CURLcode pop3_perform(ഘടന Curl_easy *data, bool *connected,
                             bool *dophase_done)
{
  /* This is POP3 and no proxy */
  CURLcode result = CURLE_OK;
  ഘടന POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);

  എങ്കിൽ(!pop3)
    തിരികെ CURLE_FAILED_INIT;

  DEBUGF(infof(data, "DO phase starts"));

  /* Start the first command in the DO phase, may alter data->req.no_body */
  result = pop3_perform_command(data);
  എങ്കിൽ(result)
    തിരികെ result;

  എങ്കിൽ(data->req.no_body)
    /* Requested no body means no transfer */
    pop3->transfer = PPTRANSFER_INFO;

  *dophase_done = FALSE; /* not done yet */

  /* Run the state-machine */
  result = pop3_multi_statemach(data, dophase_done);
  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  എങ്കിൽ(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  തിരികെ result;
}

/* Call this when the DO phase has completed */
സ്ഥിതം CURLcode pop3_dophase_done(ഘടന Curl_easy *data, bool connected)
{
  (ശൂന്യം)data;
  (ശൂന്യം)connected;

  തിരികെ CURLE_OK;
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
സ്ഥിതം CURLcode pop3_regular_transfer(ഘടന Curl_easy *data,
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
  എങ്കിൽ(!result && *dophase_done)
    result = pop3_dophase_done(data, connected);

  തിരികെ result;
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
സ്ഥിതം CURLcode pop3_do(ഘടന Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  *done = FALSE; /* default to false */

  /* Parse the URL path */
  result = pop3_parse_url_path(data);
  എങ്കിൽ(result)
    തിരികെ result;

  /* Parse the custom request */
  result = pop3_parse_custom_request(data);
  എങ്കിൽ(result)
    തിരികെ result;

  result = pop3_regular_transfer(data, done);

  തിരികെ result;
}

/***********************************************************************
 *
 * pop3_disconnect()
 *
 * Disconnect from an POP3 server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
സ്ഥിതം CURLcode pop3_disconnect(ഘടന Curl_easy *data,
                                ഘടന connectdata *conn, bool dead_connection)
{
  ഘടന pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (ശൂന്യം)data;

  എങ്കിൽ(!pop3c)
    തിരികെ CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  എങ്കിൽ(!dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &pop3c->pp) &&
     !pop3_perform_quit(data, conn))
    (ശൂന്യം)pop3_block_statemach(data, conn, TRUE); /* ignore errors on QUIT */

  /* Disconnect from the server */
  Curl_pp_disconnect(&pop3c->pp);

  /* Cleanup our connection based variables */
  curlx_safefree(pop3c->apoptimestamp);

  തിരികെ CURLE_OK;
}

/* Called from multi.c while DOing */
സ്ഥിതം CURLcode pop3_doing(ഘടന Curl_easy *data, bool *dophase_done)
{
  CURLcode result = pop3_multi_statemach(data, dophase_done);

  എങ്കിൽ(result)
    DEBUGF(infof(data, "DO phase failed"));
  അല്ലെങ്കിൽ എങ്കിൽ(*dophase_done) {
    result = pop3_dophase_done(data, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  തിരികെ result;
}

സ്ഥിതം ശൂന്യം pop3_easy_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന POP3 *pop3 = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  DEBUGASSERT(pop3);
  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);
  curlx_free(pop3);
}

സ്ഥിതം ശൂന്യം pop3_conn_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന pop3_conn *pop3c = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  DEBUGASSERT(pop3c);
  Curl_pp_disconnect(&pop3c->pp);
  curlx_safefree(pop3c->apoptimestamp);
  curlx_free(pop3c);
}

സ്ഥിതം CURLcode pop3_setup_connection(ഘടന Curl_easy *data,
                                      ഘടന connectdata *conn)
{
  ഘടന pop3_conn *pop3c;
  ഘടന POP3 *pop3 = curlx_calloc(1, വലുപ്പം(*pop3));
  എങ്കിൽ(!pop3 ||
     Curl_meta_set(data, CURL_META_POP3_EASY, pop3, pop3_easy_dtor))
    തിരികെ CURLE_OUT_OF_MEMORY;

  pop3c = curlx_calloc(1, വലുപ്പം(*pop3c));
  എങ്കിൽ(!pop3c ||
     Curl_conn_meta_set(conn, CURL_META_POP3_CONN, pop3c, pop3_conn_dtor))
    തിരികെ CURLE_OUT_OF_MEMORY;

  തിരികെ CURLE_OK;
}

/*
 * POP3 protocol.
 */
സ്ഥിരം ഘടന Curl_protocol Curl_protocol_pop3 = {
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
