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
#অন্তর্ভাব "curl_setup.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "pop3.h"

#ifndef CURL_DISABLE_POP3

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef __VMS
#অন্তর্ভাব <in.h>
#অন্তর্ভাব <inet.h>
#endif

#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "pingpong.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "bufref.h"
#অন্তর্ভাব "curl_sasl.h"
#অন্তর্ভাব "curl_md5.h"
#অন্তর্ভাব "curlx/strdup.h"

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
সংরচনা POP3 {
  curl_pp_transfer transfer;
  বর্ণ *id;               /* Message ID */
  বর্ণ *custom;           /* Custom Request */
};

/*
 * POP3 connection state
 */
প্রকারনির্দেশ গণনা {
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

সংরচনা pop3_conn {
  সংরচনা pingpong pp;
  pop3state state;        /* Always use pop3.c:state() to change state! */
  size_t eob;             /* Number of bytes of the EOB (End Of Body) that
                             have been received so far */
  size_t strip;           /* Number of bytes from the start to ignore as
                             non-body */
  সংরচনা SASL sasl;       /* SASL-related storage */
  বর্ণ *apoptimestamp;    /* APOP timestamp from the server greeting */
  unsigned বর্ণ authtypes; /* Accepted authentication types */
  unsigned বর্ণ preftype;  /* Preferred authentication type */
  BIT(ssldone);           /* Is connect() over SSL done? */
  BIT(tls_supported);     /* StartTLS capability supported by server */
};

সংরচনা pop3_cmd {
  ধ্রুব বর্ণ *name;
  unsigned short nlen;
  BIT(multiline); /* response is multi-line with last '.' line */
  BIT(multiline_with_args); /* is multi-line when command has args */
};

স্থির ধ্রুব সংরচনা pop3_cmd pop3cmds[] = {
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
স্থির CURLcode pop3_parse_url_options(সংরচনা connectdata *conn)
{
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  ধ্রুব বর্ণ *ptr = conn->options;

  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  যাবত্(!result && ptr && *ptr) {
    ধ্রুব বর্ণ *চাবি = ptr;
    ধ্রুব বর্ণ *মান;

    যাবত্(*ptr && *ptr != '=')
      ptr++;

    মান = ptr + 1;

    যাবত্(*ptr && *ptr != ';')
      ptr++;

    যদি(curl_strnequal(চাবি, "AUTH=", 5)) {
      result = Curl_sasl_parse_url_auth_option(&pop3c->sasl,
                                               মান, ptr - মান);

      যদি(result && curl_strnequal(মান, "+APOP", ptr - মান)) {
        pop3c->preftype = POP3_TYPE_APOP;
        pop3c->sasl.prefmech = SASL_AUTH_NONE;
        result = CURLE_OK;
      }
    }
    অন্যথা
      result = CURLE_URL_MALFORMAT;

    যদি(*ptr == ';')
      ptr++;
  }

  যদি(pop3c->preftype != POP3_TYPE_APOP)
    বিকল্প(pop3c->sasl.prefmech) {
    পক্ষ SASL_AUTH_NONE:
      pop3c->preftype = POP3_TYPE_NONE;
      ভঙ্গ;
    পক্ষ SASL_AUTH_DEFAULT:
      pop3c->preftype = POP3_TYPE_ANY;
      ভঙ্গ;
    স্বভাব:
      pop3c->preftype = POP3_TYPE_SASL;
      ভঙ্গ;
    }

  প্রত্যাগম result;
}

/***********************************************************************
 *
 * pop3_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
স্থির CURLcode pop3_parse_url_path(সংরচনা Curl_easy *data)
{
  /* The POP3 struct is already initialized in pop3_connect() */
  সংরচনা POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  ধ্রুব বর্ণ *path = &data->state.up.path[1]; /* skip leading path */

  যদি(!pop3)
    প্রত্যাগম CURLE_FAILED_INIT;
  /* URL decode the path for the message ID */
  প্রত্যাগম Curl_urldecode(path, 0, &pop3->id, রিক্ত, REJECT_CTRL);
}

/***********************************************************************
 *
 * pop3_parse_custom_request()
 *
 * Parse the custom request.
 */
স্থির CURLcode pop3_parse_custom_request(সংরচনা Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  সংরচনা POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  ধ্রুব বর্ণ *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  যদি(!pop3)
    প্রত্যাগম CURLE_FAILED_INIT;
  /* URL decode the custom request */
  যদি(custom)
    result = Curl_urldecode(custom, 0, &pop3->custom, রিক্ত, REJECT_CTRL);

  প্রত্যাগম result;
}

/* Return iff a command is defined as "multi-line" (RFC 1939),
 * has a response terminated by a last line with a '.'.
 */
স্থির bool pop3_is_multiline(ধ্রুব বর্ণ *cmdline)
{
  size_t i;
  প্রতি(i = 0; i < CURL_ARRAYSIZE(pop3cmds); ++i) {
    যদি(curl_strnequal(pop3cmds[i].name, cmdline, pop3cmds[i].nlen)) {
      যদি(!cmdline[pop3cmds[i].nlen])
        প্রত্যাগম (bool)pop3cmds[i].multiline;
      অন্যথা যদি(cmdline[pop3cmds[i].nlen] == ' ')
        প্রত্যাগম (bool)pop3cmds[i].multiline_with_args;
    }
  }
  /* Unknown command, assume multi-line for backward compatibility with
   * earlier curl versions that only could do multi-line responses. */
  প্রত্যাগম TRUE;
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
স্থির bool pop3_endofresp(সংরচনা Curl_easy *data, সংরচনা connectdata *conn,
                           ধ্রুব বর্ণ *সারি, size_t len, পূর্ণাঙ্ক *resp)
{
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (শূন্য)data;
  DEBUGASSERT(pop3c);
  যদি(!pop3c) /* internal error */
    প্রত্যাগম TRUE;

  /* Do we have an error response? */
  যদি(len >= 4 && !memcmp("-ERR", সারি, 4)) {
    *resp = '-';

    প্রত্যাগম TRUE;
  }

  /* Are we processing CAPA command responses? */
  যদি(pop3c->state == POP3_CAPA) {
    /* Do we have the terminating line? Per RFC 2449 this is a line
       containing only a single dot */
    যদি((len == 3 && সারি[0] == '.' && সারি[1] == '\r') ||
       (len == 2 && সারি[0] == '.' && সারি[1] == '\n'))
      /* Treat the response as a success */
      *resp = '+';
    অন্যথা
      /* Treat the response as an untagged continuation */
      *resp = '*';

    প্রত্যাগম TRUE;
  }

  /* Do we have a success response? */
  যদি(len >= 3 && !memcmp("+OK", সারি, 3)) {
    *resp = '+';

    প্রত্যাগম TRUE;
  }

  /* Do we have a continuation response? */
  যদি(len >= 1 && সারি[0] == '+') {
    *resp = '*';

    প্রত্যাগম TRUE;
  }

  প্রত্যাগম FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * pop3_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
স্থির CURLcode pop3_get_message(সংরচনা Curl_easy *data, সংরচনা bufref *out)
{
  সংরচনা pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  বর্ণ *message;
  size_t len;

  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;
  message = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;
  যদি(len > 2) {
    /* Find the start of the message */
    len -= 2;
    প্রতি(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    যাবত্(len--)
      যদি(!ISBLANK(message[len]) && !ISNEWLINE(message[len]))
        ভঙ্গ;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, রিক্ত);
  }
  অন্যথা
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, রিক্ত);

  প্রত্যাগম CURLE_OK;
}

/***********************************************************************
 *
 * pop3_state()
 *
 * This is the ONLY way to change POP3 state!
 */
স্থির শূন্য pop3_state(সংরচনা Curl_easy *data, pop3state newstate)
{
  সংরচনা pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  যদি(pop3c) {
#যদি defined(DEBUGBUILD) && defined(CURLVERBOSE)
    /* for debug purposes */
    স্থির ধ্রুব বর্ণ * ধ্রুব names[] = {
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

    যদি(pop3c->state != newstate)
      infof(data, "POP3 %p state change from %s to %s",
            (শূন্য *)pop3c, names[pop3c->state], names[newstate]);
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
স্থির CURLcode pop3_perform_capa(সংরচনা Curl_easy *data,
                                  সংরচনা connectdata *conn)
{
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;

  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  pop3c->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  pop3c->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  pop3c->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPA command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "CAPA");

  যদি(!result)
    pop3_state(data, POP3_CAPA);

  প্রত্যাগম result;
}

/***********************************************************************
 *
 * pop3_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
স্থির CURLcode pop3_perform_starttls(সংরচনা Curl_easy *data,
                                      সংরচনা connectdata *conn)
{
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;

  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  /* Send the STLS command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "STLS");
  যদি(!result)
    pop3_state(data, POP3_STARTTLS);

  প্রত্যাগম result;
}

/***********************************************************************
 *
 * pop3_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
স্থির CURLcode pop3_perform_upgrade_tls(সংরচনা Curl_easy *data,
                                         সংরচনা connectdata *conn)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;
  bool ssldone = FALSE;

  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  যদি(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    যদি(result)
      গচ্ছ out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_pop3s;
  }

  DEBUGASSERT(!pop3c->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "pop3_perform_upgrade_tls, connect -> %d, %d",
               (পূর্ণাঙ্ক)result, ssldone));
  যদি(!result && ssldone) {
    pop3c->ssldone = ssldone;
    /* perform CAPA now, changes pop3c->state out of POP3_UPGRADETLS */
    result = pop3_perform_capa(data, conn);
  }
out:
  প্রত্যাগম result;
#অন্যথা
  (শূন্য)data;
  (শূন্য)conn;
  প্রত্যাগম CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * pop3_perform_user()
 *
 * Sends a clear text USER command to authenticate with.
 */
স্থির CURLcode pop3_perform_user(সংরচনা Curl_easy *data,
                                  সংরচনা connectdata *conn)
{
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;

  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  যদি(!conn->creds) {
    pop3_state(data, POP3_STOP);

    প্রত্যাগম result;
  }

  /* Send the USER command */
  result = Curl_pp_sendf(data, &pop3c->pp, "USER %s",
                         Curl_creds_user(conn->creds));
  যদি(!result)
    pop3_state(data, POP3_USER);

  প্রত্যাগম result;
}

#ifndef CURL_DISABLE_DIGEST_AUTH
/***********************************************************************
 *
 * pop3_perform_apop()
 *
 * Sends an APOP command to authenticate with.
 */
স্থির CURLcode pop3_perform_apop(সংরচনা Curl_easy *data,
                                  সংরচনা connectdata *conn)
{
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  size_t i;
  সংরচনা MD5_context *ctxt;
  unsigned বর্ণ digest[MD5_DIGEST_LEN];
  বর্ণ secret[(2 * MD5_DIGEST_LEN) + 1];

  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  যদি(!data->state.creds) {
    pop3_state(data, POP3_STOP);

    প্রত্যাগম result;
  }

  /* Create the digest */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  যদি(!ctxt)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (ধ্রুব unsigned বর্ণ *)pop3c->apoptimestamp,
                  curlx_uztoui(strlen(pop3c->apoptimestamp)));

  Curl_MD5_update(ctxt, (ধ্রুব unsigned বর্ণ *)Curl_creds_passwd(conn->creds),
                  curlx_uztoui(strlen(Curl_creds_passwd(conn->creds))));

  /* Finalise the digest */
  Curl_MD5_final(ctxt, digest);

  /* Convert the calculated 16 octet digest into a 32-byte hex string */
  প্রতি(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&secret[2 * i], 3, "%02x", digest[i]);

  result = Curl_pp_sendf(data, &pop3c->pp, "APOP %s %s",
                         Curl_creds_user(conn->creds), secret);

  যদি(!result)
    pop3_state(data, POP3_APOP);

  প্রত্যাগম result;
}
#endif

/***********************************************************************
 *
 * pop3_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
স্থির CURLcode pop3_perform_auth(সংরচনা Curl_easy *data,
                                  ধ্রুব বর্ণ *mech,
                                  ধ্রুব সংরচনা bufref *initresp)
{
  সংরচনা pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  ধ্রুব বর্ণ *ir = Curl_bufref_ptr(initresp);

  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  যদি(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    result = Curl_pp_sendf(data, &pop3c->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  অন্যথা {
    /* Send the AUTH command */
    result = Curl_pp_sendf(data, &pop3c->pp, "AUTH %s", mech);
  }

  প্রত্যাগম result;
}

/***********************************************************************
 *
 * pop3_continue_auth()
 *
 * Sends SASL continuation data.
 */
স্থির CURLcode pop3_continue_auth(সংরচনা Curl_easy *data,
                                   ধ্রুব বর্ণ *mech,
                                   ধ্রুব সংরচনা bufref *resp)
{
  সংরচনা pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);

  (শূন্য)mech;
  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  প্রত্যাগম Curl_pp_sendf(data, &pop3c->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * pop3_cancel_auth()
 *
 * Sends SASL cancellation.
 */
স্থির CURLcode pop3_cancel_auth(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *mech)
{
  সংরচনা pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);

  (শূন্য)mech;
  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  প্রত্যাগম Curl_pp_sendf(data, &pop3c->pp, "*");
}

/***********************************************************************
 *
 * pop3_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to APOP and clear text should a
 * common mechanism not be available between the client and server.
 */
স্থির CURLcode pop3_perform_authentication(সংরচনা Curl_easy *data,
                                            সংরচনা connectdata *conn)
{
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  saslprogress progress = SASL_IDLE;

  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  /* Check we have enough data to authenticate with and end the
     connect phase if we do not */
  যদি(!Curl_sasl_can_authenticate(&pop3c->sasl, data)) {
    pop3_state(data, POP3_STOP);
    প্রত্যাগম result;
  }

  যদি(pop3c->authtypes & pop3c->preftype & POP3_TYPE_SASL) {
    /* Calculate the SASL login details */
    result = Curl_sasl_start(&pop3c->sasl, data, FALSE, &progress);

    যদি(!result && progress == SASL_INPROGRESS)
      pop3_state(data, POP3_AUTH);
  }

  যদি(!result && progress == SASL_IDLE) {
#ifndef CURL_DISABLE_DIGEST_AUTH
    যদি(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
      /* Perform APOP authentication */
      result = pop3_perform_apop(data, conn);
    অন্যথা
#endif
    যদি(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
      /* Perform clear text authentication */
      result = pop3_perform_user(data, conn);
    অন্যথা
      result = Curl_sasl_is_blocked(&pop3c->sasl, data);
  }

  প্রত্যাগম result;
}

/***********************************************************************
 *
 * pop3_perform_command()
 *
 * Sends a POP3 based command.
 */
স্থির CURLcode pop3_perform_command(সংরচনা Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ধ্রুব বর্ণ *command = রিক্ত;

  যদি(!pop3 || !pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  /* Calculate the default command */
  যদি(pop3->id[0] == '\0' || data->set.list_only) {
    command = "LIST";

    যদি(pop3->id[0] != '\0')
      /* Message specific LIST so skip the BODY transfer */
      pop3->transfer = PPTRANSFER_INFO;
  }
  অন্যথা
    command = "RETR";

  যদি(pop3->custom && pop3->custom[0] != '\0')
    command = pop3->custom;

  /* Send the command */
  যদি(pop3->id[0] != '\0')
    result = Curl_pp_sendf(data, &pop3c->pp, "%s %s", command, pop3->id);
  অন্যথা
    result = Curl_pp_sendf(data, &pop3c->pp, "%s", command);

  যদি(!result) {
    pop3_state(data, POP3_COMMAND);
    data->req.no_body = !pop3_is_multiline(command);
  }

  প্রত্যাগম result;
}

/***********************************************************************
 *
 * pop3_perform_quit()
 *
 * Performs the quit action prior to sclose() be called.
 */
স্থির CURLcode pop3_perform_quit(সংরচনা Curl_easy *data,
                                  সংরচনা connectdata *conn)
{
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;

  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  /* Send the QUIT command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "QUIT");
  যদি(!result)
    pop3_state(data, POP3_QUIT);

  প্রত্যাগম result;
}

/* For the initial server greeting */
স্থির CURLcode pop3_state_servergreet_resp(সংরচনা Curl_easy *data,
                                            পূর্ণাঙ্ক pop3code,
                                            pop3state instate)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ধ্রুব বর্ণ *সারি;
  size_t len;

  (শূন্য)instate;
  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  সারি = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  যদি(pop3code != '+') {
    failf(data, "Got unexpected pop3-server response");
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  অন্যথা যদি(len > 3) {
    /* Does the server support APOP authentication? */
    ধ্রুব বর্ণ *lt;
    ধ্রুব বর্ণ *gt = রিক্ত;

    /* Look for the APOP timestamp */
    lt = memchr(সারি, '<', len);
    যদি(lt)
      /* search the remainder for '>' */
      gt = memchr(lt, '>', len - (lt - সারি));
    যদি(gt) {
      /* the length of the timestamp, including the brackets */
      size_t timestamplen = gt - lt + 1;
      ধ্রুব বর্ণ *at = memchr(lt, '@', timestamplen);
      /* If the timestamp does not contain '@' it is not (as required by
         RFC-1939) conformant to the RFC-822 message id syntax, and we
         therefore do not use APOP authentication. */
      যদি(at) {
        /* dupe the timestamp */
        pop3c->apoptimestamp = curlx_memdup0(lt, timestamplen);
        যদি(!pop3c->apoptimestamp)
          প্রত্যাগম CURLE_OUT_OF_MEMORY;
        /* Store the APOP capability */
        pop3c->authtypes |= POP3_TYPE_APOP;
      }
    }

    যদি(!result)
      result = pop3_perform_capa(data, conn);
  }

  প্রত্যাগম result;
}

/* For CAPA responses */
স্থির CURLcode pop3_state_capa_resp(সংরচনা Curl_easy *data, পূর্ণাঙ্ক pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ধ্রুব বর্ণ *সারি;
  size_t len;

  (শূন্য)instate;
  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  সারি = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  /* Do we have an untagged continuation response? */
  যদি(pop3code == '*') {
    /* Does the server support the STLS capability? */
    যদি(len >= 4 && curl_strnequal(সারি, "STLS", 4))
      pop3c->tls_supported = TRUE;

    /* Does the server support clear text authentication? */
    অন্যথা যদি(len >= 4 && curl_strnequal(সারি, "USER", 4))
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    /* Does the server support SASL based authentication? */
    অন্যথা যদি(len >= 5 && curl_strnequal(সারি, "SASL ", 5)) {
      pop3c->authtypes |= POP3_TYPE_SASL;

      /* Advance past the SASL keyword */
      সারি += 5;
      len -= 5;

      /* Loop through the data line */
      প্রতি(;;) {
        size_t llen;
        size_t wordlen = 0;
        unsigned short mechbit;

        যাবত্(len && (ISBLANK(*সারি) || ISNEWLINE(*সারি))) {
          সারি++;
          len--;
        }

        যদি(!len)
          ভঙ্গ;

        /* Extract the word */
        যাবত্(wordlen < len && !ISBLANK(সারি[wordlen]) &&
              !ISNEWLINE(সারি[wordlen]))
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(সারি, wordlen, &llen);
        যদি(mechbit && llen == wordlen)
          pop3c->sasl.authmechs |= mechbit;

        সারি += wordlen;
        len -= wordlen;
      }
    }
  }
  অন্যথা {
    /* Clear text is supported when CAPA is not recognised */
    যদি(pop3code != '+')
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    যদি(!data->set.use_ssl || Curl_conn_is_ssl(conn, FIRSTSOCKET))
      result = pop3_perform_authentication(data, conn);
    অন্যথা যদি(pop3code == '+' && pop3c->tls_supported)
      /* Switch to TLS connection now */
      result = pop3_perform_starttls(data, conn);
    অন্যথা যদি(data->set.use_ssl <= CURLUSESSL_TRY)
      /* Fallback and carry on with authentication */
      result = pop3_perform_authentication(data, conn);
    অন্যথা {
      failf(data, "STLS not supported.");
      result = CURLE_USE_SSL_FAILED;
    }
  }

  প্রত্যাগম result;
}

/* For STARTTLS responses */
স্থির CURLcode pop3_state_starttls_resp(সংরচনা Curl_easy *data,
                                         সংরচনা connectdata *conn,
                                         পূর্ণাঙ্ক pop3code,
                                         pop3state instate)
{
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  (শূন্য)instate;

  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  /* Pipelining in response is forbidden. */
  যদি(pop3c->pp.overflow)
    প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;

  যদি(pop3code != '+') {
    যদি(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied");
      result = CURLE_USE_SSL_FAILED;
    }
    অন্যথা
      result = pop3_perform_authentication(data, conn);
  }
  অন্যথা
    pop3_state(data, POP3_UPGRADETLS);

  প্রত্যাগম result;
}

/* For SASL authentication responses */
স্থির CURLcode pop3_state_auth_resp(সংরচনা Curl_easy *data,
                                     পূর্ণাঙ্ক pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  saslprogress progress;

  (শূন্য)instate;
  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  result = Curl_sasl_continue(&pop3c->sasl, data, pop3code, &progress);
  যদি(!result)
    বিকল্প(progress) {
    পক্ষ SASL_DONE:
      pop3_state(data, POP3_STOP);  /* Authenticated */
      ভঙ্গ;
    পক্ষ SASL_IDLE:            /* No mechanism left after cancellation */
#ifndef CURL_DISABLE_DIGEST_AUTH
      যদি(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
        /* Perform APOP authentication */
        result = pop3_perform_apop(data, conn);
      অন্যথা
#endif
      যদি(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
        /* Perform clear text authentication */
        result = pop3_perform_user(data, conn);
      অন্যথা {
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
      }
      ভঙ্গ;
    স্বভাব:
      ভঙ্গ;
    }

  প্রত্যাগম result;
}

#ifndef CURL_DISABLE_DIGEST_AUTH
/* For APOP responses */
স্থির CURLcode pop3_state_apop_resp(সংরচনা Curl_easy *data, পূর্ণাঙ্ক pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  (শূন্য)instate;

  যদি(pop3code != '+') {
    failf(data, "Authentication failed: %d", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  অন্যথা
    /* End of connect phase */
    pop3_state(data, POP3_STOP);

  প্রত্যাগম result;
}
#endif

/* For USER responses */
স্থির CURLcode pop3_state_user_resp(সংরচনা Curl_easy *data, পূর্ণাঙ্ক pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (শূন্য)instate;

  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  যদি(pop3code != '+') {
    failf(data, "Access denied. %c", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  অন্যথা
    /* Send the PASS command */
    result = Curl_pp_sendf(data, &pop3c->pp, "PASS %s",
                           Curl_creds_passwd(conn->creds));
  যদি(!result)
    pop3_state(data, POP3_PASS);

  প্রত্যাগম result;
}

/* For PASS responses */
স্থির CURLcode pop3_state_pass_resp(সংরচনা Curl_easy *data, পূর্ণাঙ্ক pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  (শূন্য)instate;

  যদি(pop3code != '+') {
    failf(data, "Access denied. %c", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  অন্যথা
    /* End of connect phase */
    pop3_state(data, POP3_STOP);

  প্রত্যাগম result;
}

/***********************************************************************
 *
 * pop3_write()
 *
 * This function scans the body after the end-of-body and writes everything
 * until the end is found.
 */
স্থির CURLcode pop3_write(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *str,
                           size_t nread, bool is_eos)
{
  /* This code could be made into a special function in the handler struct */
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  bool strip_dot = FALSE;
  size_t last = 0;
  size_t i;
  (শূন্য)is_eos;

  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  /* Search through the buffer looking for the end-of-body marker which is
     5 bytes (0d 0a 2e 0d 0a). Note that a line starting with a dot matches
     the eob so the server will have prefixed it with an extra dot which we
     need to strip out. Additionally the marker could of course be spread out
     over 5 different data chunks. */
  প্রতি(i = 0; i < nread; i++) {
    size_t prev = pop3c->eob;

    বিকল্প(str[i]) {
    পক্ষ 0x0d:
      যদি(pop3c->eob == 0) {
        pop3c->eob++;

        যদি(i) {
          /* Write out the body part that did not match */
          result = Curl_client_write(data, CLIENTWRITE_BODY, &str[last],
                                     i - last);

          যদি(result)
            প্রত্যাগম result;

          last = i;
        }
      }
      অন্যথা যদি(pop3c->eob == 3)
        pop3c->eob++;
      অন্যথা
        /* If the character match was not at position 0 or 3 then restart the
           pattern matching */
        pop3c->eob = 1;
      ভঙ্গ;

    পক্ষ 0x0a:
      যদি(pop3c->eob == 1 || pop3c->eob == 4)
        pop3c->eob++;
      অন্যথা
        /* If the character match was not at position 1 or 4 then start the
           search again */
        pop3c->eob = 0;
      ভঙ্গ;

    পক্ষ 0x2e:
      যদি(pop3c->eob == 2)
        pop3c->eob++;
      অন্যথা যদি(pop3c->eob == 3) {
        /* We have an extra dot after the CRLF which we need to strip off */
        strip_dot = TRUE;
        pop3c->eob = 0;
      }
      অন্যথা
        /* If the character match was not at position 2 then start the search
           again */
        pop3c->eob = 0;
      ভঙ্গ;

    স্বভাব:
      pop3c->eob = 0;
      ভঙ্গ;
    }

    /* Did we have a partial match which has subsequently failed? */
    যদি(prev && prev >= pop3c->eob) {
      /* Strip can only be non-zero for the first mismatch after CRLF and
         then both prev and strip are equal and nothing will be output below */
      যাবত্(prev && pop3c->strip) {
        prev--;
        pop3c->strip--;
      }

      যদি(prev) {
        /* If the partial match was the CRLF and dot then only write the CRLF
           as the server would have inserted the dot */
        যদি(strip_dot && prev - 1 > 0) {
          result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB,
                                     prev - 1);
        }
        অন্যথা যদি(!strip_dot) {
          result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB,
                                     prev);
        }
        অন্যথা {
          result = CURLE_OK;
        }

        যদি(result)
          প্রত্যাগম result;

        last = i;
        strip_dot = FALSE;
      }
    }
  }

  যদি(pop3c->eob == POP3_EOB_LEN) {
    /* We have a full match so the transfer is done, however we must transfer
    the CRLF at the start of the EOB as this is considered to be part of the
    message as per RFC-1939, sect. 3 */
    result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB, 2);

    CURL_REQ_CLEAR_RECV(data);
    pop3c->eob = 0;

    প্রত্যাগম result;
  }

  যদি(pop3c->eob)
    /* While EOB is matching nothing should be output */
    প্রত্যাগম CURLE_OK;

  যদি(nread - last) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, &str[last],
                               nread - last);
  }

  প্রত্যাগম result;
}

/* For command responses */
স্থির CURLcode pop3_state_command_resp(সংরচনা Curl_easy *data,
                                        পূর্ণাঙ্ক pop3code,
                                        pop3state instate)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  সংরচনা pingpong *pp;

  (শূন্য)instate;
  যদি(!pop3 || !pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  যদি(pop3code != '+') {
    pop3_state(data, POP3_STOP);
    প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;
  }

  /* This 'OK' line ends with a CR LF pair which is the two first bytes of the
     EOB string so count this is two matching bytes. This is necessary to make
     the code detect the EOB if the only data than comes now is %2e CR LF like
     when there is no body to return. */
  pop3c->eob = 2;

  /* Since this initial CR LF pair is not part of the actual body, we set
     the strip counter here so that these bytes will not be delivered. */
  pop3c->strip = 2;

  যদি(pop3->transfer == PPTRANSFER_BODY) {
    /* POP3 download */
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);

    যদি(pp->overflow) {
      /* The recv buffer contains data that is actually body content so send
         it as such. Note that there may even be additional "headers" after
         the body */

      /* keep only the overflow */
      curlx_dyn_tail(&pp->recvbuf, pp->overflow);
      pp->nfinal = 0; /* done */

      যদি(!data->req.no_body) {
        result = pop3_write(data, curlx_dyn_ptr(&pp->recvbuf),
                            curlx_dyn_len(&pp->recvbuf), FALSE);
        যদি(result)
          প্রত্যাগম result;
      }

      /* reset the buffer */
      curlx_dyn_reset(&pp->recvbuf);
      pp->overflow = 0;
    }
  }
  অন্যথা
    pp->overflow = 0;

  /* End of DO phase */
  pop3_state(data, POP3_STOP);

  প্রত্যাগম result;
}

স্থির CURLcode pop3_statemachine(সংরচনা Curl_easy *data,
                                  সংরচনা connectdata *conn)
{
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক pop3code;
  সংরচনা pingpong *pp;
  size_t nread = 0;
  (শূন্য)data;

  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not POP3 */
upgrade_tls:
  যদি(pop3c->state == POP3_UPGRADETLS) {
    result = pop3_perform_upgrade_tls(data, conn);
    যদি(result || (pop3c->state == POP3_UPGRADETLS))
      প্রত্যাগম result;
  }

  /* Flush any data that needs to be sent */
  যদি(pp->sendleft)
    প্রত্যাগম Curl_pp_flushsend(data, pp);

  কুরু {
     /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, pp, &pop3code, &nread);
    যদি(result)
      প্রত্যাগম result;

    যদি(!pop3code)
      ভঙ্গ;

    /* We have now received a full POP3 server response */
    বিকল্প(pop3c->state) {
    পক্ষ POP3_SERVERGREET:
      result = pop3_state_servergreet_resp(data, pop3code, pop3c->state);
      ভঙ্গ;

    পক্ষ POP3_CAPA:
      result = pop3_state_capa_resp(data, pop3code, pop3c->state);
      ভঙ্গ;

    পক্ষ POP3_STARTTLS:
      result = pop3_state_starttls_resp(data, conn, pop3code, pop3c->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      যদি(!result && (pop3c->state == POP3_UPGRADETLS))
        গচ্ছ upgrade_tls;
      ভঙ্গ;

    পক্ষ POP3_AUTH:
      result = pop3_state_auth_resp(data, pop3code, pop3c->state);
      ভঙ্গ;

#ifndef CURL_DISABLE_DIGEST_AUTH
    পক্ষ POP3_APOP:
      result = pop3_state_apop_resp(data, pop3code, pop3c->state);
      ভঙ্গ;
#endif

    পক্ষ POP3_USER:
      result = pop3_state_user_resp(data, pop3code, pop3c->state);
      ভঙ্গ;

    পক্ষ POP3_PASS:
      result = pop3_state_pass_resp(data, pop3code, pop3c->state);
      ভঙ্গ;

    পক্ষ POP3_COMMAND:
      result = pop3_state_command_resp(data, pop3code, pop3c->state);
      ভঙ্গ;

    পক্ষ POP3_QUIT:
      pop3_state(data, POP3_STOP);
      ভঙ্গ;

    স্বভাব:
      /* internal error */
      pop3_state(data, POP3_STOP);
      ভঙ্গ;
    }
  } যাবত্(!result && pop3c->state != POP3_STOP && Curl_pp_moredata(pp));

  প্রত্যাগম result;
}

/* Called repeatedly until done from multi.c */
স্থির CURLcode pop3_multi_statemach(সংরচনা Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;
  result = Curl_pp_statemach(data, &pop3c->pp, FALSE, FALSE);
  *done = (pop3c->state == POP3_STOP);

  প্রত্যাগম result;
}

স্থির CURLcode pop3_block_statemach(সংরচনা Curl_easy *data,
                                     সংরচনা connectdata *conn,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  যাবত্(pop3c->state != POP3_STOP && !result)
    result = Curl_pp_statemach(data, &pop3c->pp, TRUE, disconnecting);

  প্রত্যাগম result;
}

/* For the POP3 "protocol connect" and "doing" phases only */
স্থির CURLcode pop3_pollset(সংরচনা Curl_easy *data,
                             সংরচনা easy_pollset *ps)
{
  সংরচনা pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  প্রত্যাগম pop3c ? Curl_pp_pollset(data, &pop3c->pp, ps) : CURLE_OK;
}

/* SASL parameters for the pop3 protocol */
স্থির ধ্রুব সংরচনা SASLproto saslpop3 = {
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
স্থির CURLcode pop3_connect(সংরচনা Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  সংরচনা pingpong *pp = pop3c ? &pop3c->pp : রিক্ত;

  *done = FALSE; /* default to not done yet */
  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  PINGPONG_SETUP(pp, pop3_statemachine, pop3_endofresp);

  /* Set the default preferred authentication type and mechanism */
  pop3c->preftype = POP3_TYPE_ANY;
  Curl_sasl_init(&pop3c->sasl, data, &saslpop3);

  /* Initialize the pingpong layer */
  Curl_pp_init(pp, Curl_pgrs_now(data));

  /* Parse the URL options */
  result = pop3_parse_url_options(conn);
  যদি(result)
    প্রত্যাগম result;

  /* Start off waiting for the server greeting response */
  pop3_state(data, POP3_SERVERGREET);

  result = pop3_multi_statemach(data, done);

  প্রত্যাগম result;
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
স্থির CURLcode pop3_done(সংরচনা Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  সংরচনা POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);

  (শূন্য)premature;

  যদি(!pop3)
    প্রত্যাগম CURLE_OK;

  যদি(status) {
    CURL_TRC_M(data, "POP3 done with bad status");
    connclose(data->conn);
    result = status;         /* use the already set error code */
  }

  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);

  /* Clear the transfer mode for the next request */
  pop3->transfer = PPTRANSFER_BODY;

  প্রত্যাগম result;
}

/***********************************************************************
 *
 * pop3_perform()
 *
 * This is the actual DO function for POP3. Get a message/listing according to
 * the options previously setup.
 */
স্থির CURLcode pop3_perform(সংরচনা Curl_easy *data, bool *connected,
                             bool *dophase_done)
{
  /* This is POP3 and no proxy */
  CURLcode result = CURLE_OK;
  সংরচনা POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);

  যদি(!pop3)
    প্রত্যাগম CURLE_FAILED_INIT;

  DEBUGF(infof(data, "DO phase starts"));

  /* Start the first command in the DO phase, may alter data->req.no_body */
  result = pop3_perform_command(data);
  যদি(result)
    প্রত্যাগম result;

  যদি(data->req.no_body)
    /* Requested no body means no transfer */
    pop3->transfer = PPTRANSFER_INFO;

  *dophase_done = FALSE; /* not done yet */

  /* Run the state-machine */
  result = pop3_multi_statemach(data, dophase_done);
  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  যদি(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  প্রত্যাগম result;
}

/* Call this when the DO phase has completed */
স্থির CURLcode pop3_dophase_done(সংরচনা Curl_easy *data, bool connected)
{
  (শূন্য)data;
  (শূন্য)connected;

  প্রত্যাগম CURLE_OK;
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
স্থির CURLcode pop3_regular_transfer(সংরচনা Curl_easy *data,
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
  যদি(!result && *dophase_done)
    result = pop3_dophase_done(data, connected);

  প্রত্যাগম result;
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
স্থির CURLcode pop3_do(সংরচনা Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  *done = FALSE; /* default to false */

  /* Parse the URL path */
  result = pop3_parse_url_path(data);
  যদি(result)
    প্রত্যাগম result;

  /* Parse the custom request */
  result = pop3_parse_custom_request(data);
  যদি(result)
    প্রত্যাগম result;

  result = pop3_regular_transfer(data, done);

  প্রত্যাগম result;
}

/***********************************************************************
 *
 * pop3_disconnect()
 *
 * Disconnect from an POP3 server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
স্থির CURLcode pop3_disconnect(সংরচনা Curl_easy *data,
                                সংরচনা connectdata *conn, bool dead_connection)
{
  সংরচনা pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (শূন্য)data;

  যদি(!pop3c)
    প্রত্যাগম CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  যদি(!dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &pop3c->pp) &&
     !pop3_perform_quit(data, conn))
    (শূন্য)pop3_block_statemach(data, conn, TRUE); /* ignore errors on QUIT */

  /* Disconnect from the server */
  Curl_pp_disconnect(&pop3c->pp);

  /* Cleanup our connection based variables */
  curlx_safefree(pop3c->apoptimestamp);

  প্রত্যাগম CURLE_OK;
}

/* Called from multi.c while DOing */
স্থির CURLcode pop3_doing(সংরচনা Curl_easy *data, bool *dophase_done)
{
  CURLcode result = pop3_multi_statemach(data, dophase_done);

  যদি(result)
    DEBUGF(infof(data, "DO phase failed"));
  অন্যথা যদি(*dophase_done) {
    result = pop3_dophase_done(data, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  প্রত্যাগম result;
}

স্থির শূন্য pop3_easy_dtor(ধ্রুব শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা POP3 *pop3 = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  DEBUGASSERT(pop3);
  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);
  curlx_free(pop3);
}

স্থির শূন্য pop3_conn_dtor(ধ্রুব শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা pop3_conn *pop3c = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  DEBUGASSERT(pop3c);
  Curl_pp_disconnect(&pop3c->pp);
  curlx_safefree(pop3c->apoptimestamp);
  curlx_free(pop3c);
}

স্থির CURLcode pop3_setup_connection(সংরচনা Curl_easy *data,
                                      সংরচনা connectdata *conn)
{
  সংরচনা pop3_conn *pop3c;
  সংরচনা POP3 *pop3 = curlx_calloc(1, পরিমাণ(*pop3));
  যদি(!pop3 ||
     Curl_meta_set(data, CURL_META_POP3_EASY, pop3, pop3_easy_dtor))
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  pop3c = curlx_calloc(1, পরিমাণ(*pop3c));
  যদি(!pop3c ||
     Curl_conn_meta_set(conn, CURL_META_POP3_CONN, pop3c, pop3_conn_dtor))
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  প্রত্যাগম CURLE_OK;
}

/*
 * POP3 protocol.
 */
ধ্রুব সংরচনা Curl_protocol Curl_protocol_pop3 = {
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
