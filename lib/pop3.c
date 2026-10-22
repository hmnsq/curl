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
#شامل "curl_setup.h"
#شامل "urldata.h"
#شامل "pop3.h"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_POP3

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ __VMS
#شامل <in.h>
#شامل <inet.h>
#اگر_ختم

#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "progress.h"
#شامل "transfer.h"
#شامل "escape.h"
#شامل "pingpong.h"
#شامل "vtls/vtls.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "select.h"
#شامل "url.h"
#شامل "bufref.h"
#شامل "curl_sasl.h"
#شامل "curl_md5.h"
#شامل "curlx/strdup.h"

/* Authentication type flags */
#تعریف_کرو POP3_TYPE_CLEARTEXT (1 << 0)
#تعریف_کرو POP3_TYPE_APOP      (1 << 1)
#تعریف_کرو POP3_TYPE_SASL      (1 << 2)

/* Authentication type values */
#تعریف_کرو POP3_TYPE_NONE 0
#تعریف_کرو POP3_TYPE_ANY  (POP3_TYPE_CLEARTEXT | POP3_TYPE_APOP | POP3_TYPE_SASL)

/* This is the 5-bytes End-Of-Body marker for POP3 */
#تعریف_کرو POP3_EOB     "\x0d\x0a\x2e\x0d\x0a"
#تعریف_کرو POP3_EOB_LEN 5

/* meta key for storing protocol meta at easy handle */
#تعریف_کرو CURL_META_POP3_EASY   "meta:proto:pop3:easy"
/* meta key for storing protocol meta at connection */
#تعریف_کرو CURL_META_POP3_CONN   "meta:proto:pop3:conn"

/*
 * POP3 easy handle state
 */
ساخت POP3 {
  curl_pp_transfer transfer;
  حرف *id;               /* Message ID */
  حرف *custom;           /* Custom Request */
};

/*
 * POP3 connection state
 */
قسمتعریف شمار {
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
  POP3_آخری          /* never used */
} pop3state;

ساخت pop3_conn {
  ساخت pingpong pp;
  pop3state state;        /* Always use pop3.c:state() to change state! */
  size_t eob;             /* Number of bytes of the EOB (End Of Body) that
                             have been received so far */
  size_t strip;           /* Number of bytes from the start to ignore as
                             non-body */
  ساخت SASL sasl;       /* SASL-related storage */
  حرف *apoptimestamp;    /* APOP timestamp from the server greeting */
  غیرعلامتی حرف authtypes; /* Accepted authentication types */
  غیرعلامتی حرف preftype;  /* Preferred authentication type */
  ثنائی_ہندسہ(ssldone);           /* Is connect() over SSL done? */
  ثنائی_ہندسہ(tls_supported);     /* StartTLS capability supported by server */
};

ساخت pop3_cmd {
  ثابت حرف *name;
  غیرعلامتی مختصر nlen;
  ثنائی_ہندسہ(multiline); /* response is multi-line with last '.' line */
  ثنائی_ہندسہ(multiline_with_دلائل); /* is multi-line when command has args */
};

ساکن ثابت ساخت pop3_cmd pop3cmds[] = {
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
ساکن CURLcode pop3_parse_url_options(ساخت connectdata *conn)
{
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *ptr = conn->options;

  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  جب(!نتیجہ && ptr && *ptr) {
    ثابت حرف *کلید = ptr;
    ثابت حرف *قدر;

    جب(*ptr && *ptr != '=')
      ptr++;

    قدر = ptr + 1;

    جب(*ptr && *ptr != ';')
      ptr++;

    اگر(curl_strnequal(کلید, "AUTH=", 5)) {
      نتیجہ = Curl_sasl_parse_url_auth_option(&pop3c->sasl,
                                               قدر, ptr - قدر);

      اگر(نتیجہ && curl_strnequal(قدر, "+APOP", ptr - قدر)) {
        pop3c->preftype = POP3_TYPE_APOP;
        pop3c->sasl.prefmech = SASL_AUTH_NONE;
        نتیجہ = CURLE_OK;
      }
    }
    ورنہ
      نتیجہ = CURLE_URL_MALFORMAT;

    اگر(*ptr == ';')
      ptr++;
  }

  اگر(pop3c->preftype != POP3_TYPE_APOP)
    انتخاب(pop3c->sasl.prefmech) {
    حالت SASL_AUTH_NONE:
      pop3c->preftype = POP3_TYPE_NONE;
      توڑو;
    حالت SASL_AUTH_DEFAULT:
      pop3c->preftype = POP3_TYPE_ANY;
      توڑو;
    طےشدہ:
      pop3c->preftype = POP3_TYPE_SASL;
      توڑو;
    }

  واپس نتیجہ;
}

/***********************************************************************
 *
 * pop3_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
ساکن CURLcode pop3_parse_url_راستہ(ساخت Curl_easy *ڈیٹا)
{
  /* The POP3 struct is already initialized in pop3_connect() */
  ساخت POP3 *pop3 = Curl_meta_get(ڈیٹا, CURL_META_POP3_EASY);
  ثابت حرف *راستہ = &ڈیٹا->state.اوپر.راستہ[1]; /* skip leading path */

  اگر(!pop3)
    واپس CURLE_FAILED_INIT;
  /* URL decode the path for the message ID */
  واپس Curl_urldecode(راستہ, 0, &pop3->id, تہی, REJECT_CTRL);
}

/***********************************************************************
 *
 * pop3_parse_custom_request()
 *
 * Parse the custom request.
 */
ساکن CURLcode pop3_parse_custom_request(ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت POP3 *pop3 = Curl_meta_get(ڈیٹا, CURL_META_POP3_EASY);
  ثابت حرف *custom = CURL_EASY_STR(ڈیٹا, STRING_CUSTOMREQUEST);

  اگر(!pop3)
    واپس CURLE_FAILED_INIT;
  /* URL decode the custom request */
  اگر(custom)
    نتیجہ = Curl_urldecode(custom, 0, &pop3->custom, تہی, REJECT_CTRL);

  واپس نتیجہ;
}

/* Return iff a command is defined as "multi-line" (RFC 1939),
 * has a response terminated by a last line with a '.'.
 */
ساکن bool pop3_is_multiline(ثابت حرف *cmdline)
{
  size_t i;
  برائے(i = 0; i < CURL_ARRAYSIZE(pop3cmds); ++i) {
    اگر(curl_strnequal(pop3cmds[i].name, cmdline, pop3cmds[i].nlen)) {
      اگر(!cmdline[pop3cmds[i].nlen])
        واپس (bool)pop3cmds[i].multiline;
      ورنہ اگر(cmdline[pop3cmds[i].nlen] == ' ')
        واپس (bool)pop3cmds[i].multiline_with_دلائل;
    }
  }
  /* Unknown command, assume multi-line for backward compatibility with
   * earlier curl versions that only could do multi-line responses. */
  واپس TRUE;
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
ساکن bool pop3_endofresp(ساخت Curl_easy *ڈیٹا, ساخت connectdata *conn,
                           ثابت حرف *سطر, size_t len, صحیح *resp)
{
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (خالی)ڈیٹا;
  DEBUGASSERT(pop3c);
  اگر(!pop3c) /* internal error */
    واپس TRUE;

  /* Do we have an error response? */
  اگر(len >= 4 && !memcmp("-ERR", سطر, 4)) {
    *resp = '-';

    واپس TRUE;
  }

  /* Are we processing CAPA command responses? */
  اگر(pop3c->state == POP3_CAPA) {
    /* Do we have the terminating line? Per RFC 2449 this is a line
       containing only a single dot */
    اگر((len == 3 && سطر[0] == '.' && سطر[1] == '\r') ||
       (len == 2 && سطر[0] == '.' && سطر[1] == '\n'))
      /* Treat the response as a success */
      *resp = '+';
    ورنہ
      /* Treat the response as an untagged continuation */
      *resp = '*';

    واپس TRUE;
  }

  /* Do we have a success response? */
  اگر(len >= 3 && !memcmp("+OK", سطر, 3)) {
    *resp = '+';

    واپس TRUE;
  }

  /* Do we have a continuation response? */
  اگر(len >= 1 && سطر[0] == '+') {
    *resp = '*';

    واپس TRUE;
  }

  واپس FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * pop3_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ساکن CURLcode pop3_get_پیغام(ساخت Curl_easy *ڈیٹا, ساخت bufref *برآمد)
{
  ساخت pop3_conn *pop3c =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_POP3_CONN);
  حرف *پیغام_2;
  size_t len;

  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;
  پیغام_2 = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;
  اگر(len > 2) {
    /* Find the start of the message */
    len -= 2;
    برائے(پیغام_2 += 2; ISBLANK(*پیغام_2); پیغام_2++, len--)
      ;

    /* Find the end of the message */
    جب(len--)
      اگر(!ISBLANK(پیغام_2[len]) && !ISNEWLINE(پیغام_2[len]))
        توڑو;

    /* Terminate the message */
    پیغام_2[++len] = '\0';
    Curl_bufref_سیٹ(برآمد, پیغام_2, len, تہی);
  }
  ورنہ
    /* junk input => zero length output */
    Curl_bufref_سیٹ(برآمد, "", 0, تہی);

  واپس CURLE_OK;
}

/***********************************************************************
 *
 * pop3_state()
 *
 * This is the ONLY way to change POP3 state!
 */
ساکن خالی pop3_state(ساخت Curl_easy *ڈیٹا, pop3state newstate)
{
  ساخت pop3_conn *pop3c =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_POP3_CONN);
  اگر(pop3c) {
#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
    /* for debug purposes */
    ساکن ثابت حرف * ثابت نام[] = {
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
      /*آخری*/
    };

    اگر(pop3c->state != newstate)
      infof(ڈیٹا, "POP3 %p state change from %s to %s",
            (خالی *)pop3c, نام[pop3c->state], نام[newstate]);
#اگر_ختم

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
ساکن CURLcode pop3_perform_capa(ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata *conn)
{
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتیجہ = CURLE_OK;

  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  pop3c->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  pop3c->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  pop3c->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPA command */
  نتیجہ = Curl_pp_sendf(ڈیٹا, &pop3c->pp, "%s", "CAPA");

  اگر(!نتیجہ)
    pop3_state(ڈیٹا, POP3_CAPA);

  واپس نتیجہ;
}

/***********************************************************************
 *
 * pop3_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
ساکن CURLcode pop3_perform_starttls(ساخت Curl_easy *ڈیٹا,
                                      ساخت connectdata *conn)
{
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتیجہ;

  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  /* Send the STLS command */
  نتیجہ = Curl_pp_sendf(ڈیٹا, &pop3c->pp, "%s", "STLS");
  اگر(!نتیجہ)
    pop3_state(ڈیٹا, POP3_STARTTLS);

  واپس نتیجہ;
}

/***********************************************************************
 *
 * pop3_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ساکن CURLcode pop3_perform_upgrade_tls(ساخت Curl_easy *ڈیٹا,
                                         ساخت connectdata *conn)
{
#اگر_تعریف_شدہ USE_SSL
  /* Start the SSL connection */
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتیجہ;
  bool ssldone = FALSE;

  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  اگر(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    نتیجہ = Curl_ssl_cfilter_جوڑو(
      ڈیٹا, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    اگر(نتیجہ)
      جاؤ برآمد;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_pop3s;
  }

  DEBUGASSERT(!pop3c->ssldone);
  نتیجہ = Curl_conn_connect(ڈیٹا, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(ڈیٹا, "pop3_perform_upgrade_tls, connect -> %d, %d",
               (صحیح)نتیجہ, ssldone));
  اگر(!نتیجہ && ssldone) {
    pop3c->ssldone = ssldone;
    /* perform CAPA now, changes pop3c->state out of POP3_UPGRADETLS */
    نتیجہ = pop3_perform_capa(ڈیٹا, conn);
  }
برآمد:
  واپس نتیجہ;
#ورنہ
  (خالی)ڈیٹا;
  (خالی)conn;
  واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
}

/***********************************************************************
 *
 * pop3_perform_user()
 *
 * Sends a clear text USER command to authenticate with.
 */
ساکن CURLcode pop3_perform_user(ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata *conn)
{
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتیجہ = CURLE_OK;

  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  اگر(!conn->creds) {
    pop3_state(ڈیٹا, POP3_STOP);

    واپس نتیجہ;
  }

  /* Send the USER command */
  نتیجہ = Curl_pp_sendf(ڈیٹا, &pop3c->pp, "USER %s",
                         Curl_creds_user(conn->creds));
  اگر(!نتیجہ)
    pop3_state(ڈیٹا, POP3_USER);

  واپس نتیجہ;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
/***********************************************************************
 *
 * pop3_perform_apop()
 *
 * Sends an APOP command to authenticate with.
 */
ساکن CURLcode pop3_perform_apop(ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata *conn)
{
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتیجہ = CURLE_OK;
  size_t i;
  ساخت MD5_context *ctxt;
  غیرعلامتی حرف digest[MD5_DIGEST_LEN];
  حرف secret[(2 * MD5_DIGEST_LEN) + 1];

  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  اگر(!ڈیٹا->state.creds) {
    pop3_state(ڈیٹا, POP3_STOP);

    واپس نتیجہ;
  }

  /* Create the digest */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  اگر(!ctxt)
    واپس CURLE_برآمد_کا_حافظہ;

  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)pop3c->apoptimestamp,
                  curlx_uztoui(لڑی_لمبائی(pop3c->apoptimestamp)));

  Curl_MD5_update(ctxt, (ثابت غیرعلامتی حرف *)Curl_creds_passwd(conn->creds),
                  curlx_uztoui(لڑی_لمبائی(Curl_creds_passwd(conn->creds))));

  /* Finalise the digest */
  Curl_MD5_final(ctxt, digest);

  /* Convert the calculated 16 octet digest into a 32-byte hex string */
  برائے(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&secret[2 * i], 3, "%02x", digest[i]);

  نتیجہ = Curl_pp_sendf(ڈیٹا, &pop3c->pp, "APOP %s %s",
                         Curl_creds_user(conn->creds), secret);

  اگر(!نتیجہ)
    pop3_state(ڈیٹا, POP3_APOP);

  واپس نتیجہ;
}
#اگر_ختم

/***********************************************************************
 *
 * pop3_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
ساکن CURLcode pop3_perform_auth(ساخت Curl_easy *ڈیٹا,
                                  ثابت حرف *mech,
                                  ثابت ساخت bufref *initresp)
{
  ساخت pop3_conn *pop3c =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_POP3_CONN);
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *ir = Curl_bufref_ptr(initresp);

  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  اگر(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    نتیجہ = Curl_pp_sendf(ڈیٹا, &pop3c->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  ورنہ {
    /* Send the AUTH command */
    نتیجہ = Curl_pp_sendf(ڈیٹا, &pop3c->pp, "AUTH %s", mech);
  }

  واپس نتیجہ;
}

/***********************************************************************
 *
 * pop3_continue_auth()
 *
 * Sends SASL continuation data.
 */
ساکن CURLcode pop3_continue_auth(ساخت Curl_easy *ڈیٹا,
                                   ثابت حرف *mech,
                                   ثابت ساخت bufref *resp)
{
  ساخت pop3_conn *pop3c =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_POP3_CONN);

  (خالی)mech;
  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  واپس Curl_pp_sendf(ڈیٹا, &pop3c->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * pop3_cancel_auth()
 *
 * Sends SASL cancellation.
 */
ساکن CURLcode pop3_cancel_auth(ساخت Curl_easy *ڈیٹا, ثابت حرف *mech)
{
  ساخت pop3_conn *pop3c =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_POP3_CONN);

  (خالی)mech;
  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  واپس Curl_pp_sendf(ڈیٹا, &pop3c->pp, "*");
}

/***********************************************************************
 *
 * pop3_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to APOP and clear text should a
 * common mechanism not be available between the client and server.
 */
ساکن CURLcode pop3_perform_authentication(ساخت Curl_easy *ڈیٹا,
                                            ساخت connectdata *conn)
{
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتیجہ = CURLE_OK;
  saslprogress پیش_رفت = SASL_IDLE;

  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  /* Check we have enough data to authenticate with and end the
     connect phase if we do not */
  اگر(!Curl_sasl_can_authenticate(&pop3c->sasl, ڈیٹا)) {
    pop3_state(ڈیٹا, POP3_STOP);
    واپس نتیجہ;
  }

  اگر(pop3c->authtypes & pop3c->preftype & POP3_TYPE_SASL) {
    /* Calculate the SASL login details */
    نتیجہ = Curl_sasl_آغاز(&pop3c->sasl, ڈیٹا, FALSE, &پیش_رفت);

    اگر(!نتیجہ && پیش_رفت == SASL_INPROGRESS)
      pop3_state(ڈیٹا, POP3_AUTH);
  }

  اگر(!نتیجہ && پیش_رفت == SASL_IDLE) {
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
    اگر(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
      /* Perform APOP authentication */
      نتیجہ = pop3_perform_apop(ڈیٹا, conn);
    ورنہ
#اگر_ختم
    اگر(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
      /* Perform clear text authentication */
      نتیجہ = pop3_perform_user(ڈیٹا, conn);
    ورنہ
      نتیجہ = Curl_sasl_is_blocked(&pop3c->sasl, ڈیٹا);
  }

  واپس نتیجہ;
}

/***********************************************************************
 *
 * pop3_perform_command()
 *
 * Sends a POP3 based command.
 */
ساکن CURLcode pop3_perform_command(ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت POP3 *pop3 = Curl_meta_get(ڈیٹا, CURL_META_POP3_EASY);
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ثابت حرف *command = تہی;

  اگر(!pop3 || !pop3c)
    واپس CURLE_FAILED_INIT;

  /* Calculate the default command */
  اگر(pop3->id[0] == '\0' || ڈیٹا->سیٹ.list_only) {
    command = "LIST";

    اگر(pop3->id[0] != '\0')
      /* Message specific LIST so skip the BODY transfer */
      pop3->transfer = PPTRANSFER_INFO;
  }
  ورنہ
    command = "RETR";

  اگر(pop3->custom && pop3->custom[0] != '\0')
    command = pop3->custom;

  /* Send the command */
  اگر(pop3->id[0] != '\0')
    نتیجہ = Curl_pp_sendf(ڈیٹا, &pop3c->pp, "%s %s", command, pop3->id);
  ورنہ
    نتیجہ = Curl_pp_sendf(ڈیٹا, &pop3c->pp, "%s", command);

  اگر(!نتیجہ) {
    pop3_state(ڈیٹا, POP3_COMMAND);
    ڈیٹا->req.no_body = !pop3_is_multiline(command);
  }

  واپس نتیجہ;
}

/***********************************************************************
 *
 * pop3_perform_quit()
 *
 * Performs the quit action prior to sclose() be called.
 */
ساکن CURLcode pop3_perform_quit(ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata *conn)
{
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتیجہ;

  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  /* Send the QUIT command */
  نتیجہ = Curl_pp_sendf(ڈیٹا, &pop3c->pp, "%s", "QUIT");
  اگر(!نتیجہ)
    pop3_state(ڈیٹا, POP3_QUIT);

  واپس نتیجہ;
}

/* For the initial server greeting */
ساکن CURLcode pop3_state_servergreet_resp(ساخت Curl_easy *ڈیٹا,
                                            صحیح pop3code,
                                            pop3state instate)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ثابت حرف *سطر;
  size_t len;

  (خالی)instate;
  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  سطر = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  اگر(pop3code != '+') {
    failf(ڈیٹا, "Got unexpected pop3-server response");
    نتیجہ = CURLE_WEIRD_SERVER_جواب;
  }
  ورنہ اگر(len > 3) {
    /* Does the server support APOP authentication? */
    ثابت حرف *lt;
    ثابت حرف *gt = تہی;

    /* Look for the APOP timestamp */
    lt = memchr(سطر, '<', len);
    اگر(lt)
      /* search the remainder for '>' */
      gt = memchr(lt, '>', len - (lt - سطر));
    اگر(gt) {
      /* the length of the timestamp, including the brackets */
      size_t timestamplen = gt - lt + 1;
      ثابت حرف *at = memchr(lt, '@', timestamplen);
      /* If the timestamp does not contain '@' it is not (as required by
         RFC-1939) conformant to the RFC-822 message id syntax, and we
         therefore do not use APOP authentication. */
      اگر(at) {
        /* dupe the timestamp */
        pop3c->apoptimestamp = curlx_memdup0(lt, timestamplen);
        اگر(!pop3c->apoptimestamp)
          واپس CURLE_برآمد_کا_حافظہ;
        /* Store the APOP capability */
        pop3c->authtypes |= POP3_TYPE_APOP;
      }
    }

    اگر(!نتیجہ)
      نتیجہ = pop3_perform_capa(ڈیٹا, conn);
  }

  واپس نتیجہ;
}

/* For CAPA responses */
ساکن CURLcode pop3_state_capa_resp(ساخت Curl_easy *ڈیٹا, صحیح pop3code,
                                     pop3state instate)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ثابت حرف *سطر;
  size_t len;

  (خالی)instate;
  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  سطر = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  /* Do we have an untagged continuation response? */
  اگر(pop3code == '*') {
    /* Does the server support the STLS capability? */
    اگر(len >= 4 && curl_strnequal(سطر, "STLS", 4))
      pop3c->tls_supported = TRUE;

    /* Does the server support clear text authentication? */
    ورنہ اگر(len >= 4 && curl_strnequal(سطر, "USER", 4))
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    /* Does the server support SASL based authentication? */
    ورنہ اگر(len >= 5 && curl_strnequal(سطر, "SASL ", 5)) {
      pop3c->authtypes |= POP3_TYPE_SASL;

      /* Advance past the SASL keyword */
      سطر += 5;
      len -= 5;

      /* Loop through the data line */
      برائے(;;) {
        size_t llen;
        size_t wordlen = 0;
        غیرعلامتی مختصر mechbit;

        جب(len && (ISBLANK(*سطر) || ISNEWLINE(*سطر))) {
          سطر++;
          len--;
        }

        اگر(!len)
          توڑو;

        /* Extract the word */
        جب(wordlen < len && !ISBLANK(سطر[wordlen]) &&
              !ISNEWLINE(سطر[wordlen]))
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(سطر, wordlen, &llen);
        اگر(mechbit && llen == wordlen)
          pop3c->sasl.authmechs |= mechbit;

        سطر += wordlen;
        len -= wordlen;
      }
    }
  }
  ورنہ {
    /* Clear text is supported when CAPA is not recognised */
    اگر(pop3code != '+')
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    اگر(!ڈیٹا->سیٹ.use_ssl || Curl_conn_is_ssl(conn, FIRSTSOCKET))
      نتیجہ = pop3_perform_authentication(ڈیٹا, conn);
    ورنہ اگر(pop3code == '+' && pop3c->tls_supported)
      /* Switch to TLS connection now */
      نتیجہ = pop3_perform_starttls(ڈیٹا, conn);
    ورنہ اگر(ڈیٹا->سیٹ.use_ssl <= CURLUSESSL_TRY)
      /* Fallback and carry on with authentication */
      نتیجہ = pop3_perform_authentication(ڈیٹا, conn);
    ورنہ {
      failf(ڈیٹا, "STLS not supported.");
      نتیجہ = CURLE_USE_SSL_FAILED;
    }
  }

  واپس نتیجہ;
}

/* For STARTTLS responses */
ساکن CURLcode pop3_state_starttls_resp(ساخت Curl_easy *ڈیٹا,
                                         ساخت connectdata *conn,
                                         صحیح pop3code,
                                         pop3state instate)
{
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتیجہ = CURLE_OK;
  (خالی)instate;

  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  /* Pipelining in response is forbidden. */
  اگر(pop3c->pp.حد_سے_تجاوز)
    واپس CURLE_WEIRD_SERVER_جواب;

  اگر(pop3code != '+') {
    اگر(ڈیٹا->سیٹ.use_ssl != CURLUSESSL_TRY) {
      failf(ڈیٹا, "STARTTLS denied");
      نتیجہ = CURLE_USE_SSL_FAILED;
    }
    ورنہ
      نتیجہ = pop3_perform_authentication(ڈیٹا, conn);
  }
  ورنہ
    pop3_state(ڈیٹا, POP3_UPGRADETLS);

  واپس نتیجہ;
}

/* For SASL authentication responses */
ساکن CURLcode pop3_state_auth_resp(ساخت Curl_easy *ڈیٹا,
                                     صحیح pop3code,
                                     pop3state instate)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  saslprogress پیش_رفت;

  (خالی)instate;
  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  نتیجہ = Curl_sasl_continue(&pop3c->sasl, ڈیٹا, pop3code, &پیش_رفت);
  اگر(!نتیجہ)
    انتخاب(پیش_رفت) {
    حالت SASL_مکمل:
      pop3_state(ڈیٹا, POP3_STOP);  /* Authenticated */
      توڑو;
    حالت SASL_IDLE:            /* No mechanism left after cancellation */
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
      اگر(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
        /* Perform APOP authentication */
        نتیجہ = pop3_perform_apop(ڈیٹا, conn);
      ورنہ
#اگر_ختم
      اگر(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
        /* Perform clear text authentication */
        نتیجہ = pop3_perform_user(ڈیٹا, conn);
      ورنہ {
        failf(ڈیٹا, "Authentication cancelled");
        نتیجہ = CURLE_LOGIN_DENIED;
      }
      توڑو;
    طےشدہ:
      توڑو;
    }

  واپس نتیجہ;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
/* For APOP responses */
ساکن CURLcode pop3_state_apop_resp(ساخت Curl_easy *ڈیٹا, صحیح pop3code,
                                     pop3state instate)
{
  CURLcode نتیجہ = CURLE_OK;
  (خالی)instate;

  اگر(pop3code != '+') {
    failf(ڈیٹا, "Authentication failed: %d", pop3code);
    نتیجہ = CURLE_LOGIN_DENIED;
  }
  ورنہ
    /* End of connect phase */
    pop3_state(ڈیٹا, POP3_STOP);

  واپس نتیجہ;
}
#اگر_ختم

/* For USER responses */
ساکن CURLcode pop3_state_user_resp(ساخت Curl_easy *ڈیٹا, صحیح pop3code,
                                     pop3state instate)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (خالی)instate;

  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  اگر(pop3code != '+') {
    failf(ڈیٹا, "Access denied. %c", pop3code);
    نتیجہ = CURLE_LOGIN_DENIED;
  }
  ورنہ
    /* Send the PASS command */
    نتیجہ = Curl_pp_sendf(ڈیٹا, &pop3c->pp, "PASS %s",
                           Curl_creds_passwd(conn->creds));
  اگر(!نتیجہ)
    pop3_state(ڈیٹا, POP3_PASS);

  واپس نتیجہ;
}

/* For PASS responses */
ساکن CURLcode pop3_state_pass_resp(ساخت Curl_easy *ڈیٹا, صحیح pop3code,
                                     pop3state instate)
{
  CURLcode نتیجہ = CURLE_OK;
  (خالی)instate;

  اگر(pop3code != '+') {
    failf(ڈیٹا, "Access denied. %c", pop3code);
    نتیجہ = CURLE_LOGIN_DENIED;
  }
  ورنہ
    /* End of connect phase */
    pop3_state(ڈیٹا, POP3_STOP);

  واپس نتیجہ;
}

/***********************************************************************
 *
 * pop3_write()
 *
 * This function scans the body after the end-of-body and writes everything
 * until the end is found.
 */
ساکن CURLcode pop3_write(ساخت Curl_easy *ڈیٹا, ثابت حرف *str,
                           size_t nread, bool is_eos)
{
  /* This code could be made into a special function in the handler struct */
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  bool strip_dot = FALSE;
  size_t آخری = 0;
  size_t i;
  (خالی)is_eos;

  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  /* Search through the buffer looking for the end-of-body marker which is
     5 bytes (0d 0a 2e 0d 0a). Note that a line starting with a dot matches
     the eob so the server will have prefixed it with an extra dot which we
     need to strip out. Additionally the marker could of course be spread out
     over 5 different data chunks. */
  برائے(i = 0; i < nread; i++) {
    size_t پچھلا = pop3c->eob;

    انتخاب(str[i]) {
    حالت 0x0d:
      اگر(pop3c->eob == 0) {
        pop3c->eob++;

        اگر(i) {
          /* Write out the body part that did not match */
          نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, &str[آخری],
                                     i - آخری);

          اگر(نتیجہ)
            واپس نتیجہ;

          آخری = i;
        }
      }
      ورنہ اگر(pop3c->eob == 3)
        pop3c->eob++;
      ورنہ
        /* If the character match was not at position 0 or 3 then restart the
           pattern matching */
        pop3c->eob = 1;
      توڑو;

    حالت 0x0a:
      اگر(pop3c->eob == 1 || pop3c->eob == 4)
        pop3c->eob++;
      ورنہ
        /* If the character match was not at position 1 or 4 then start the
           search again */
        pop3c->eob = 0;
      توڑو;

    حالت 0x2e:
      اگر(pop3c->eob == 2)
        pop3c->eob++;
      ورنہ اگر(pop3c->eob == 3) {
        /* We have an extra dot after the CRLF which we need to strip off */
        strip_dot = TRUE;
        pop3c->eob = 0;
      }
      ورنہ
        /* If the character match was not at position 2 then start the search
           again */
        pop3c->eob = 0;
      توڑو;

    طےشدہ:
      pop3c->eob = 0;
      توڑو;
    }

    /* Did we have a partial match which has subsequently failed? */
    اگر(پچھلا && پچھلا >= pop3c->eob) {
      /* Strip can only be non-zero for the first mismatch after CRLF and
         then both prev and strip are equal and nothing will be output below */
      جب(پچھلا && pop3c->strip) {
        پچھلا--;
        pop3c->strip--;
      }

      اگر(پچھلا) {
        /* If the partial match was the CRLF and dot then only write the CRLF
           as the server would have inserted the dot */
        اگر(strip_dot && پچھلا - 1 > 0) {
          نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, POP3_EOB,
                                     پچھلا - 1);
        }
        ورنہ اگر(!strip_dot) {
          نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, POP3_EOB,
                                     پچھلا);
        }
        ورنہ {
          نتیجہ = CURLE_OK;
        }

        اگر(نتیجہ)
          واپس نتیجہ;

        آخری = i;
        strip_dot = FALSE;
      }
    }
  }

  اگر(pop3c->eob == POP3_EOB_LEN) {
    /* We have a full match so the transfer is done, however we must transfer
    the CRLF at the start of the EOB as this is considered to be part of the
    message as per RFC-1939, sect. 3 */
    نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, POP3_EOB, 2);

    CURL_REQ_CLEAR_RECV(ڈیٹا);
    pop3c->eob = 0;

    واپس نتیجہ;
  }

  اگر(pop3c->eob)
    /* While EOB is matching nothing should be output */
    واپس CURLE_OK;

  اگر(nread - آخری) {
    نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, &str[آخری],
                               nread - آخری);
  }

  واپس نتیجہ;
}

/* For command responses */
ساکن CURLcode pop3_state_command_resp(ساخت Curl_easy *ڈیٹا,
                                        صحیح pop3code,
                                        pop3state instate)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت POP3 *pop3 = Curl_meta_get(ڈیٹا, CURL_META_POP3_EASY);
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ساخت pingpong *pp;

  (خالی)instate;
  اگر(!pop3 || !pop3c)
    واپس CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  اگر(pop3code != '+') {
    pop3_state(ڈیٹا, POP3_STOP);
    واپس CURLE_WEIRD_SERVER_جواب;
  }

  /* This 'OK' line ends with a CR LF pair which is the two first bytes of the
     EOB string so count this is two matching bytes. This is necessary to make
     the code detect the EOB if the only data than comes now is %2e CR LF like
     when there is no body to return. */
  pop3c->eob = 2;

  /* Since this initial CR LF pair is not part of the actual body, we set
     the strip counter here so that these bytes will not be delivered. */
  pop3c->strip = 2;

  اگر(pop3->transfer == PPTRANSFER_BODY) {
    /* POP3 download */
    Curl_xfer_تیاری_recv(ڈیٹا, FIRSTSOCKET, -1);

    اگر(pp->حد_سے_تجاوز) {
      /* The recv buffer contains data that is actually body content so send
         it as such. Note that there may even be additional "headers" after
         the body */

      /* keep only the overflow */
      curlx_dyn_دم(&pp->recvbuf, pp->حد_سے_تجاوز);
      pp->nfinal = 0; /* done */

      اگر(!ڈیٹا->req.no_body) {
        نتیجہ = pop3_write(ڈیٹا, curlx_dyn_ptr(&pp->recvbuf),
                            curlx_dyn_len(&pp->recvbuf), FALSE);
        اگر(نتیجہ)
          واپس نتیجہ;
      }

      /* reset the buffer */
      curlx_dyn_reset(&pp->recvbuf);
      pp->حد_سے_تجاوز = 0;
    }
  }
  ورنہ
    pp->حد_سے_تجاوز = 0;

  /* End of DO phase */
  pop3_state(ڈیٹا, POP3_STOP);

  واپس نتیجہ;
}

ساکن CURLcode pop3_statemachine(ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata *conn)
{
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتیجہ = CURLE_OK;
  صحیح pop3code;
  ساخت pingpong *pp;
  size_t nread = 0;
  (خالی)ڈیٹا;

  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not POP3 */
upgrade_tls:
  اگر(pop3c->state == POP3_UPGRADETLS) {
    نتیجہ = pop3_perform_upgrade_tls(ڈیٹا, conn);
    اگر(نتیجہ || (pop3c->state == POP3_UPGRADETLS))
      واپس نتیجہ;
  }

  /* Flush any data that needs to be sent */
  اگر(pp->sendleft)
    واپس Curl_pp_flushsend(ڈیٹا, pp);

  کرو {
     /* Read the response from the server */
    نتیجہ = Curl_pp_readresp(ڈیٹا, FIRSTSOCKET, pp, &pop3code, &nread);
    اگر(نتیجہ)
      واپس نتیجہ;

    اگر(!pop3code)
      توڑو;

    /* We have now received a full POP3 server response */
    انتخاب(pop3c->state) {
    حالت POP3_SERVERGREET:
      نتیجہ = pop3_state_servergreet_resp(ڈیٹا, pop3code, pop3c->state);
      توڑو;

    حالت POP3_CAPA:
      نتیجہ = pop3_state_capa_resp(ڈیٹا, pop3code, pop3c->state);
      توڑو;

    حالت POP3_STARTTLS:
      نتیجہ = pop3_state_starttls_resp(ڈیٹا, conn, pop3code, pop3c->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      اگر(!نتیجہ && (pop3c->state == POP3_UPGRADETLS))
        جاؤ upgrade_tls;
      توڑو;

    حالت POP3_AUTH:
      نتیجہ = pop3_state_auth_resp(ڈیٹا, pop3code, pop3c->state);
      توڑو;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
    حالت POP3_APOP:
      نتیجہ = pop3_state_apop_resp(ڈیٹا, pop3code, pop3c->state);
      توڑو;
#اگر_ختم

    حالت POP3_USER:
      نتیجہ = pop3_state_user_resp(ڈیٹا, pop3code, pop3c->state);
      توڑو;

    حالت POP3_PASS:
      نتیجہ = pop3_state_pass_resp(ڈیٹا, pop3code, pop3c->state);
      توڑو;

    حالت POP3_COMMAND:
      نتیجہ = pop3_state_command_resp(ڈیٹا, pop3code, pop3c->state);
      توڑو;

    حالت POP3_QUIT:
      pop3_state(ڈیٹا, POP3_STOP);
      توڑو;

    طےشدہ:
      /* internal error */
      pop3_state(ڈیٹا, POP3_STOP);
      توڑو;
    }
  } جب(!نتیجہ && pop3c->state != POP3_STOP && Curl_pp_moredata(pp));

  واپس نتیجہ;
}

/* Called repeatedly until done from multi.c */
ساکن CURLcode pop3_multi_statemach(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;
  نتیجہ = Curl_pp_statemach(ڈیٹا, &pop3c->pp, FALSE, FALSE);
  *مکمل = (pop3c->state == POP3_STOP);

  واپس نتیجہ;
}

ساکن CURLcode pop3_بلاک_statemach(ساخت Curl_easy *ڈیٹا,
                                     ساخت connectdata *conn,
                                     bool disconnecting)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  جب(pop3c->state != POP3_STOP && !نتیجہ)
    نتیجہ = Curl_pp_statemach(ڈیٹا, &pop3c->pp, TRUE, disconnecting);

  واپس نتیجہ;
}

/* For the POP3 "protocol connect" and "doing" phases only */
ساکن CURLcode pop3_pollset(ساخت Curl_easy *ڈیٹا,
                             ساخت easy_pollset *ps)
{
  ساخت pop3_conn *pop3c =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_POP3_CONN);
  واپس pop3c ? Curl_pp_pollset(ڈیٹا, &pop3c->pp, ps) : CURLE_OK;
}

/* SASL parameters for the pop3 protocol */
ساکن ثابت ساخت SASLproto saslpop3 = {
  "pop",                /* The service name */
  pop3_perform_auth,    /* Send authentication command */
  pop3_continue_auth,   /* Send authentication continuation */
  pop3_cancel_auth,     /* Send authentication cancellation */
  pop3_get_پیغام,     /* Get SASL response message */
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
ساکن CURLcode pop3_connect(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ساخت pingpong *pp = pop3c ? &pop3c->pp : تہی;

  *مکمل = FALSE; /* default to not done yet */
  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  PINGPONG_تیاری(pp, pop3_statemachine, pop3_endofresp);

  /* Set the default preferred authentication type and mechanism */
  pop3c->preftype = POP3_TYPE_ANY;
  Curl_sasl_init(&pop3c->sasl, ڈیٹا, &saslpop3);

  /* Initialize the pingpong layer */
  Curl_pp_init(pp, Curl_pgrs_now(ڈیٹا));

  /* Parse the URL options */
  نتیجہ = pop3_parse_url_options(conn);
  اگر(نتیجہ)
    واپس نتیجہ;

  /* Start off waiting for the server greeting response */
  pop3_state(ڈیٹا, POP3_SERVERGREET);

  نتیجہ = pop3_multi_statemach(ڈیٹا, مکمل);

  واپس نتیجہ;
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
ساکن CURLcode pop3_مکمل(ساخت Curl_easy *ڈیٹا, CURLcode حالت_2,
                          bool premature)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت POP3 *pop3 = Curl_meta_get(ڈیٹا, CURL_META_POP3_EASY);

  (خالی)premature;

  اگر(!pop3)
    واپس CURLE_OK;

  اگر(حالت_2) {
    CURL_TRC_M(ڈیٹا, "POP3 done with bad status");
    connclose(ڈیٹا->conn);
    نتیجہ = حالت_2;         /* use the already set error code */
  }

  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);

  /* Clear the transfer mode for the next request */
  pop3->transfer = PPTRANSFER_BODY;

  واپس نتیجہ;
}

/***********************************************************************
 *
 * pop3_perform()
 *
 * This is the actual DO function for POP3. Get a message/listing according to
 * the options previously setup.
 */
ساکن CURLcode pop3_perform(ساخت Curl_easy *ڈیٹا, bool *connected,
                             bool *dophase_مکمل)
{
  /* This is POP3 and no proxy */
  CURLcode نتیجہ = CURLE_OK;
  ساخت POP3 *pop3 = Curl_meta_get(ڈیٹا, CURL_META_POP3_EASY);

  اگر(!pop3)
    واپس CURLE_FAILED_INIT;

  DEBUGF(infof(ڈیٹا, "DO phase starts"));

  /* Start the first command in the DO phase, may alter data->req.no_body */
  نتیجہ = pop3_perform_command(ڈیٹا);
  اگر(نتیجہ)
    واپس نتیجہ;

  اگر(ڈیٹا->req.no_body)
    /* Requested no body means no transfer */
    pop3->transfer = PPTRANSFER_INFO;

  *dophase_مکمل = FALSE; /* not done yet */

  /* Run the state-machine */
  نتیجہ = pop3_multi_statemach(ڈیٹا, dophase_مکمل);
  *connected = Curl_conn_is_connected(ڈیٹا->conn, FIRSTSOCKET);

  اگر(*dophase_مکمل)
    DEBUGF(infof(ڈیٹا, "DO phase is complete"));

  واپس نتیجہ;
}

/* Call this when the DO phase has completed */
ساکن CURLcode pop3_dophase_مکمل(ساخت Curl_easy *ڈیٹا, bool connected)
{
  (خالی)ڈیٹا;
  (خالی)connected;

  واپس CURLE_OK;
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
ساکن CURLcode pop3_regular_transfer(ساخت Curl_easy *ڈیٹا,
                                      bool *dophase_مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  ڈیٹا->req.سائز = -1;

  /* Set the progress data */
  Curl_pgrsReset(ڈیٹا);

  /* Carry out the perform */
  نتیجہ = pop3_perform(ڈیٹا, &connected, dophase_مکمل);

  /* Perform post DO phase operations if necessary */
  اگر(!نتیجہ && *dophase_مکمل)
    نتیجہ = pop3_dophase_مکمل(ڈیٹا, connected);

  واپس نتیجہ;
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
ساکن CURLcode pop3_do(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  *مکمل = FALSE; /* default to false */

  /* Parse the URL path */
  نتیجہ = pop3_parse_url_راستہ(ڈیٹا);
  اگر(نتیجہ)
    واپس نتیجہ;

  /* Parse the custom request */
  نتیجہ = pop3_parse_custom_request(ڈیٹا);
  اگر(نتیجہ)
    واپس نتیجہ;

  نتیجہ = pop3_regular_transfer(ڈیٹا, مکمل);

  واپس نتیجہ;
}

/***********************************************************************
 *
 * pop3_disconnect()
 *
 * Disconnect from an POP3 server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ساکن CURLcode pop3_disconnect(ساخت Curl_easy *ڈیٹا,
                                ساخت connectdata *conn, bool dead_connection)
{
  ساخت pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (خالی)ڈیٹا;

  اگر(!pop3c)
    واپس CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  اگر(!dead_connection && conn->بٹ.protoconnstart &&
     !Curl_pp_needs_flush(ڈیٹا, &pop3c->pp) &&
     !pop3_perform_quit(ڈیٹا, conn))
    (خالی)pop3_بلاک_statemach(ڈیٹا, conn, TRUE); /* ignore errors on QUIT */

  /* Disconnect from the server */
  Curl_pp_disconnect(&pop3c->pp);

  /* Cleanup our connection based variables */
  curlx_safefree(pop3c->apoptimestamp);

  واپس CURLE_OK;
}

/* Called from multi.c while DOing */
ساکن CURLcode pop3_doing(ساخت Curl_easy *ڈیٹا, bool *dophase_مکمل)
{
  CURLcode نتیجہ = pop3_multi_statemach(ڈیٹا, dophase_مکمل);

  اگر(نتیجہ)
    DEBUGF(infof(ڈیٹا, "DO phase failed"));
  ورنہ اگر(*dophase_مکمل) {
    نتیجہ = pop3_dophase_مکمل(ڈیٹا, FALSE /* not connected */);

    DEBUGF(infof(ڈیٹا, "DO phase is complete"));
  }

  واپس نتیجہ;
}

ساکن خالی pop3_easy_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت POP3 *pop3 = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  DEBUGASSERT(pop3);
  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);
  curlx_free(pop3);
}

ساکن خالی pop3_conn_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت pop3_conn *pop3c = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  DEBUGASSERT(pop3c);
  Curl_pp_disconnect(&pop3c->pp);
  curlx_safefree(pop3c->apoptimestamp);
  curlx_free(pop3c);
}

ساکن CURLcode pop3_تیاری_connection(ساخت Curl_easy *ڈیٹا,
                                      ساخت connectdata *conn)
{
  ساخت pop3_conn *pop3c;
  ساخت POP3 *pop3 = curlx_calloc(1, حجم(*pop3));
  اگر(!pop3 ||
     Curl_meta_سیٹ(ڈیٹا, CURL_META_POP3_EASY, pop3, pop3_easy_dtor))
    واپس CURLE_برآمد_کا_حافظہ;

  pop3c = curlx_calloc(1, حجم(*pop3c));
  اگر(!pop3c ||
     Curl_conn_meta_سیٹ(conn, CURL_META_POP3_CONN, pop3c, pop3_conn_dtor))
    واپس CURLE_برآمد_کا_حافظہ;

  واپس CURLE_OK;
}

/*
 * POP3 protocol.
 */
ثابت ساخت Curl_protocol Curl_protocol_pop3 = {
  pop3_تیاری_connection,            /* setup_connection */
  pop3_do,                          /* do_it */
  pop3_مکمل,                        /* done */
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

#اگر_ختم /* CURL_DISABLE_POP3 */
