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
#ضمن "curl_setup.h"
#ضمن "urldata.h"
#ضمن "pop3.h"

#اگر_تعریف_نشده CURL_DISABLE_POP3

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده __VMS
#ضمن <in.h>
#ضمن <inet.h>
#پایان_اگر

#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "progress.h"
#ضمن "transfer.h"
#ضمن "escape.h"
#ضمن "pingpong.h"
#ضمن "vtls/vtls.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "url.h"
#ضمن "bufref.h"
#ضمن "curl_sasl.h"
#ضمن "curl_md5.h"
#ضمن "curlx/strdup.h"

/* Authentication type flags */
#تعریف_کن POP3_TYPE_CLEARTEXT (1 << 0)
#تعریف_کن POP3_TYPE_APOP      (1 << 1)
#تعریف_کن POP3_TYPE_SASL      (1 << 2)

/* Authentication type values */
#تعریف_کن POP3_TYPE_NONE 0
#تعریف_کن POP3_TYPE_ANY  (POP3_TYPE_CLEARTEXT | POP3_TYPE_APOP | POP3_TYPE_SASL)

/* This is the 5-bytes End-Of-Body marker for POP3 */
#تعریف_کن POP3_EOB     "\x0d\x0a\x2e\x0d\x0a"
#تعریف_کن POP3_EOB_LEN 5

/* meta key for storing protocol meta at easy handle */
#تعریف_کن CURL_META_POP3_EASY   "meta:proto:pop3:easy"
/* meta key for storing protocol meta at connection */
#تعریف_کن CURL_META_POP3_CONN   "meta:proto:pop3:conn"

/*
 * POP3 easy handle state
 */
بنية POP3 {
  curl_pp_transfer transfer;
  حرف *id;               /* Message ID */
  حرف *custom;           /* Custom Request */
};

/*
 * POP3 connection state
 */
تعريفنوع تعداد {
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
  POP3_آخرین          /* never used */
} pop3state;

بنية pop3_conn {
  بنية pingpong pp;
  pop3state state;        /* Always use pop3.c:state() to change state! */
  size_t eob;             /* Number of bytes of the EOB (End Of Body) that
                             have been received so far */
  size_t strip;           /* Number of bytes from the start to ignore as
                             non-body */
  بنية SASL sasl;       /* SASL-related storage */
  حرف *apoptimestamp;    /* APOP timestamp from the server greeting */
  بیعلامت حرف authtypes; /* Accepted authentication types */
  بیعلامت حرف preftype;  /* Preferred authentication type */
  بیت(ssldone);           /* Is connect() over SSL done? */
  بیت(tls_supported);     /* StartTLS capability supported by server */
};

بنية pop3_cmd {
  ثابت حرف *name;
  بیعلامت کوتاه nlen;
  بیت(multiline); /* response is multi-line with last '.' line */
  بیت(multiline_with_آرگومانها); /* is multi-line when command has args */
};

ساكن ثابت بنية pop3_cmd pop3cmds[] = {
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
ساكن CURLcode pop3_parse_url_options(بنية connectdata *conn)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتیجه = CURLE_OK;
  ثابت حرف *ptr = conn->options;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  طالما(!نتیجه && ptr && *ptr) {
    ثابت حرف *کلید = ptr;
    ثابت حرف *مقدار;

    طالما(*ptr && *ptr != '=')
      ptr++;

    مقدار = ptr + 1;

    طالما(*ptr && *ptr != ';')
      ptr++;

    إذا(curl_strnequal(کلید, "AUTH=", 5)) {
      نتیجه = Curl_sasl_parse_url_auth_option(&pop3c->sasl,
                                               مقدار, ptr - مقدار);

      إذا(نتیجه && curl_strnequal(مقدار, "+APOP", ptr - مقدار)) {
        pop3c->preftype = POP3_TYPE_APOP;
        pop3c->sasl.prefmech = SASL_AUTH_NONE;
        نتیجه = CURLE_OK;
      }
    }
    وإلا
      نتیجه = CURLE_URL_MALFORMAT;

    إذا(*ptr == ';')
      ptr++;
  }

  إذا(pop3c->preftype != POP3_TYPE_APOP)
    بدل(pop3c->sasl.prefmech) {
    حالة SASL_AUTH_NONE:
      pop3c->preftype = POP3_TYPE_NONE;
      اكسر;
    حالة SASL_AUTH_DEFAULT:
      pop3c->preftype = POP3_TYPE_ANY;
      اكسر;
    افتراضي:
      pop3c->preftype = POP3_TYPE_SASL;
      اكسر;
    }

  أرجع نتیجه;
}

/***********************************************************************
 *
 * pop3_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
ساكن CURLcode pop3_parse_url_مسیر(بنية Curl_easy *داده)
{
  /* The POP3 struct is already initialized in pop3_connect() */
  بنية POP3 *pop3 = Curl_meta_get(داده, CURL_META_POP3_EASY);
  ثابت حرف *مسیر = &داده->state.به_بالا.مسیر[1]; /* skip leading path */

  إذا(!pop3)
    أرجع CURLE_FAILED_INIT;
  /* URL decode the path for the message ID */
  أرجع Curl_urldecode(مسیر, 0, &pop3->id, عدم, REJECT_CTRL);
}

/***********************************************************************
 *
 * pop3_parse_custom_request()
 *
 * Parse the custom request.
 */
ساكن CURLcode pop3_parse_custom_request(بنية Curl_easy *داده)
{
  CURLcode نتیجه = CURLE_OK;
  بنية POP3 *pop3 = Curl_meta_get(داده, CURL_META_POP3_EASY);
  ثابت حرف *custom = CURL_EASY_STR(داده, STRING_CUSTOMREQUEST);

  إذا(!pop3)
    أرجع CURLE_FAILED_INIT;
  /* URL decode the custom request */
  إذا(custom)
    نتیجه = Curl_urldecode(custom, 0, &pop3->custom, عدم, REJECT_CTRL);

  أرجع نتیجه;
}

/* Return iff a command is defined as "multi-line" (RFC 1939),
 * has a response terminated by a last line with a '.'.
 */
ساكن bool pop3_is_multiline(ثابت حرف *cmdline)
{
  size_t i;
  لكل(i = 0; i < CURL_ARRAYSIZE(pop3cmds); ++i) {
    إذا(curl_strnequal(pop3cmds[i].name, cmdline, pop3cmds[i].nlen)) {
      إذا(!cmdline[pop3cmds[i].nlen])
        أرجع (bool)pop3cmds[i].multiline;
      وإلا إذا(cmdline[pop3cmds[i].nlen] == ' ')
        أرجع (bool)pop3cmds[i].multiline_with_آرگومانها;
    }
  }
  /* Unknown command, assume multi-line for backward compatibility with
   * earlier curl versions that only could do multi-line responses. */
  أرجع TRUE;
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
ساكن bool pop3_endofresp(بنية Curl_easy *داده, بنية connectdata *conn,
                           ثابت حرف *سطر, size_t len, صحيح *resp)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (فراغ)داده;
  DEBUGASSERT(pop3c);
  إذا(!pop3c) /* internal error */
    أرجع TRUE;

  /* Do we have an error response? */
  إذا(len >= 4 && !memcmp("-ERR", سطر, 4)) {
    *resp = '-';

    أرجع TRUE;
  }

  /* Are we processing CAPA command responses? */
  إذا(pop3c->state == POP3_CAPA) {
    /* Do we have the terminating line? Per RFC 2449 this is a line
       containing only a single dot */
    إذا((len == 3 && سطر[0] == '.' && سطر[1] == '\r') ||
       (len == 2 && سطر[0] == '.' && سطر[1] == '\n'))
      /* Treat the response as a success */
      *resp = '+';
    وإلا
      /* Treat the response as an untagged continuation */
      *resp = '*';

    أرجع TRUE;
  }

  /* Do we have a success response? */
  إذا(len >= 3 && !memcmp("+OK", سطر, 3)) {
    *resp = '+';

    أرجع TRUE;
  }

  /* Do we have a continuation response? */
  إذا(len >= 1 && سطر[0] == '+') {
    *resp = '*';

    أرجع TRUE;
  }

  أرجع FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * pop3_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ساكن CURLcode pop3_get_پیام(بنية Curl_easy *داده, بنية bufref *بیرون)
{
  بنية pop3_conn *pop3c =
    Curl_conn_meta_get(داده->conn, CURL_META_POP3_CONN);
  حرف *پیام_2;
  size_t len;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;
  پیام_2 = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;
  إذا(len > 2) {
    /* Find the start of the message */
    len -= 2;
    لكل(پیام_2 += 2; ISBLANK(*پیام_2); پیام_2++, len--)
      ;

    /* Find the end of the message */
    طالما(len--)
      إذا(!ISBLANK(پیام_2[len]) && !ISNEWLINE(پیام_2[len]))
        اكسر;

    /* Terminate the message */
    پیام_2[++len] = '\0';
    Curl_bufref_مجموعه(بیرون, پیام_2, len, عدم);
  }
  وإلا
    /* junk input => zero length output */
    Curl_bufref_مجموعه(بیرون, "", 0, عدم);

  أرجع CURLE_OK;
}

/***********************************************************************
 *
 * pop3_state()
 *
 * This is the ONLY way to change POP3 state!
 */
ساكن فراغ pop3_state(بنية Curl_easy *داده, pop3state newstate)
{
  بنية pop3_conn *pop3c =
    Curl_conn_meta_get(داده->conn, CURL_META_POP3_CONN);
  إذا(pop3c) {
#إذا defined(DEBUGBUILD) && defined(CURLVERBOSE)
    /* for debug purposes */
    ساكن ثابت حرف * ثابت نامها[] = {
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
      /*آخرین*/
    };

    إذا(pop3c->state != newstate)
      infof(داده, "POP3 %p state change from %s to %s",
            (فراغ *)pop3c, نامها[pop3c->state], نامها[newstate]);
#پایان_اگر

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
ساكن CURLcode pop3_perform_capa(بنية Curl_easy *داده,
                                  بنية connectdata *conn)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتیجه = CURLE_OK;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  pop3c->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  pop3c->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  pop3c->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPA command */
  نتیجه = Curl_pp_sendf(داده, &pop3c->pp, "%s", "CAPA");

  إذا(!نتیجه)
    pop3_state(داده, POP3_CAPA);

  أرجع نتیجه;
}

/***********************************************************************
 *
 * pop3_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
ساكن CURLcode pop3_perform_starttls(بنية Curl_easy *داده,
                                      بنية connectdata *conn)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتیجه;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  /* Send the STLS command */
  نتیجه = Curl_pp_sendf(داده, &pop3c->pp, "%s", "STLS");
  إذا(!نتیجه)
    pop3_state(داده, POP3_STARTTLS);

  أرجع نتیجه;
}

/***********************************************************************
 *
 * pop3_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ساكن CURLcode pop3_perform_upgrade_tls(بنية Curl_easy *داده,
                                         بنية connectdata *conn)
{
#اگر_تعریف_شده USE_SSL
  /* Start the SSL connection */
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتیجه;
  bool ssldone = FALSE;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  إذا(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    نتیجه = Curl_ssl_cfilter_جمع_کن(
      داده, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    إذا(نتیجه)
      اذهب بیرون;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_pop3s;
  }

  DEBUGASSERT(!pop3c->ssldone);
  نتیجه = Curl_conn_connect(داده, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(داده, "pop3_perform_upgrade_tls, connect -> %d, %d",
               (صحيح)نتیجه, ssldone));
  إذا(!نتیجه && ssldone) {
    pop3c->ssldone = ssldone;
    /* perform CAPA now, changes pop3c->state out of POP3_UPGRADETLS */
    نتیجه = pop3_perform_capa(داده, conn);
  }
بیرون:
  أرجع نتیجه;
#وإلا
  (فراغ)داده;
  (فراغ)conn;
  أرجع CURLE_NOT_BUILT_درون;
#پایان_اگر
}

/***********************************************************************
 *
 * pop3_perform_user()
 *
 * Sends a clear text USER command to authenticate with.
 */
ساكن CURLcode pop3_perform_user(بنية Curl_easy *داده,
                                  بنية connectdata *conn)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتیجه = CURLE_OK;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  إذا(!conn->creds) {
    pop3_state(داده, POP3_STOP);

    أرجع نتیجه;
  }

  /* Send the USER command */
  نتیجه = Curl_pp_sendf(داده, &pop3c->pp, "USER %s",
                         Curl_creds_user(conn->creds));
  إذا(!نتیجه)
    pop3_state(داده, POP3_USER);

  أرجع نتیجه;
}

#اگر_تعریف_نشده CURL_DISABLE_DIGEST_AUTH
/***********************************************************************
 *
 * pop3_perform_apop()
 *
 * Sends an APOP command to authenticate with.
 */
ساكن CURLcode pop3_perform_apop(بنية Curl_easy *داده,
                                  بنية connectdata *conn)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتیجه = CURLE_OK;
  size_t i;
  بنية MD5_context *ctxt;
  بیعلامت حرف digest[MD5_DIGEST_LEN];
  حرف secret[(2 * MD5_DIGEST_LEN) + 1];

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  إذا(!داده->state.creds) {
    pop3_state(داده, POP3_STOP);

    أرجع نتیجه;
  }

  /* Create the digest */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  إذا(!ctxt)
    أرجع CURLE_بیرون_از_حافظه;

  Curl_MD5_update(ctxt, (ثابت بیعلامت حرف *)pop3c->apoptimestamp,
                  curlx_uztoui(طول_رشته(pop3c->apoptimestamp)));

  Curl_MD5_update(ctxt, (ثابت بیعلامت حرف *)Curl_creds_passwd(conn->creds),
                  curlx_uztoui(طول_رشته(Curl_creds_passwd(conn->creds))));

  /* Finalise the digest */
  Curl_MD5_final(ctxt, digest);

  /* Convert the calculated 16 octet digest into a 32-byte hex string */
  لكل(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&secret[2 * i], 3, "%02x", digest[i]);

  نتیجه = Curl_pp_sendf(داده, &pop3c->pp, "APOP %s %s",
                         Curl_creds_user(conn->creds), secret);

  إذا(!نتیجه)
    pop3_state(داده, POP3_APOP);

  أرجع نتیجه;
}
#پایان_اگر

/***********************************************************************
 *
 * pop3_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
ساكن CURLcode pop3_perform_auth(بنية Curl_easy *داده,
                                  ثابت حرف *mech,
                                  ثابت بنية bufref *initresp)
{
  بنية pop3_conn *pop3c =
    Curl_conn_meta_get(داده->conn, CURL_META_POP3_CONN);
  CURLcode نتیجه = CURLE_OK;
  ثابت حرف *ir = Curl_bufref_ptr(initresp);

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  إذا(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    نتیجه = Curl_pp_sendf(داده, &pop3c->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  وإلا {
    /* Send the AUTH command */
    نتیجه = Curl_pp_sendf(داده, &pop3c->pp, "AUTH %s", mech);
  }

  أرجع نتیجه;
}

/***********************************************************************
 *
 * pop3_continue_auth()
 *
 * Sends SASL continuation data.
 */
ساكن CURLcode pop3_continue_auth(بنية Curl_easy *داده,
                                   ثابت حرف *mech,
                                   ثابت بنية bufref *resp)
{
  بنية pop3_conn *pop3c =
    Curl_conn_meta_get(داده->conn, CURL_META_POP3_CONN);

  (فراغ)mech;
  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  أرجع Curl_pp_sendf(داده, &pop3c->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * pop3_cancel_auth()
 *
 * Sends SASL cancellation.
 */
ساكن CURLcode pop3_cancel_auth(بنية Curl_easy *داده, ثابت حرف *mech)
{
  بنية pop3_conn *pop3c =
    Curl_conn_meta_get(داده->conn, CURL_META_POP3_CONN);

  (فراغ)mech;
  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  أرجع Curl_pp_sendf(داده, &pop3c->pp, "*");
}

/***********************************************************************
 *
 * pop3_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to APOP and clear text should a
 * common mechanism not be available between the client and server.
 */
ساكن CURLcode pop3_perform_authentication(بنية Curl_easy *داده,
                                            بنية connectdata *conn)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتیجه = CURLE_OK;
  saslprogress پیشرفت = SASL_IDLE;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  /* Check we have enough data to authenticate with and end the
     connect phase if we do not */
  إذا(!Curl_sasl_can_authenticate(&pop3c->sasl, داده)) {
    pop3_state(داده, POP3_STOP);
    أرجع نتیجه;
  }

  إذا(pop3c->authtypes & pop3c->preftype & POP3_TYPE_SASL) {
    /* Calculate the SASL login details */
    نتیجه = Curl_sasl_شروع(&pop3c->sasl, داده, FALSE, &پیشرفت);

    إذا(!نتیجه && پیشرفت == SASL_INPROGRESS)
      pop3_state(داده, POP3_AUTH);
  }

  إذا(!نتیجه && پیشرفت == SASL_IDLE) {
#اگر_تعریف_نشده CURL_DISABLE_DIGEST_AUTH
    إذا(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
      /* Perform APOP authentication */
      نتیجه = pop3_perform_apop(داده, conn);
    وإلا
#پایان_اگر
    إذا(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
      /* Perform clear text authentication */
      نتیجه = pop3_perform_user(داده, conn);
    وإلا
      نتیجه = Curl_sasl_is_blocked(&pop3c->sasl, داده);
  }

  أرجع نتیجه;
}

/***********************************************************************
 *
 * pop3_perform_command()
 *
 * Sends a POP3 based command.
 */
ساكن CURLcode pop3_perform_command(بنية Curl_easy *داده)
{
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;
  بنية POP3 *pop3 = Curl_meta_get(داده, CURL_META_POP3_EASY);
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ثابت حرف *command = عدم;

  إذا(!pop3 || !pop3c)
    أرجع CURLE_FAILED_INIT;

  /* Calculate the default command */
  إذا(pop3->id[0] == '\0' || داده->مجموعه.list_only) {
    command = "LIST";

    إذا(pop3->id[0] != '\0')
      /* Message specific LIST so skip the BODY transfer */
      pop3->transfer = PPTRANSFER_INFO;
  }
  وإلا
    command = "RETR";

  إذا(pop3->custom && pop3->custom[0] != '\0')
    command = pop3->custom;

  /* Send the command */
  إذا(pop3->id[0] != '\0')
    نتیجه = Curl_pp_sendf(داده, &pop3c->pp, "%s %s", command, pop3->id);
  وإلا
    نتیجه = Curl_pp_sendf(داده, &pop3c->pp, "%s", command);

  إذا(!نتیجه) {
    pop3_state(داده, POP3_COMMAND);
    داده->req.no_body = !pop3_is_multiline(command);
  }

  أرجع نتیجه;
}

/***********************************************************************
 *
 * pop3_perform_quit()
 *
 * Performs the quit action prior to sclose() be called.
 */
ساكن CURLcode pop3_perform_quit(بنية Curl_easy *داده,
                                  بنية connectdata *conn)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتیجه;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  /* Send the QUIT command */
  نتیجه = Curl_pp_sendf(داده, &pop3c->pp, "%s", "QUIT");
  إذا(!نتیجه)
    pop3_state(داده, POP3_QUIT);

  أرجع نتیجه;
}

/* For the initial server greeting */
ساكن CURLcode pop3_state_servergreet_resp(بنية Curl_easy *داده,
                                            صحيح pop3code,
                                            pop3state instate)
{
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ثابت حرف *سطر;
  size_t len;

  (فراغ)instate;
  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  سطر = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  إذا(pop3code != '+') {
    failf(داده, "Got unexpected pop3-server response");
    نتیجه = CURLE_WEIRD_SERVER_پاسخ;
  }
  وإلا إذا(len > 3) {
    /* Does the server support APOP authentication? */
    ثابت حرف *lt;
    ثابت حرف *gt = عدم;

    /* Look for the APOP timestamp */
    lt = memchr(سطر, '<', len);
    إذا(lt)
      /* search the remainder for '>' */
      gt = memchr(lt, '>', len - (lt - سطر));
    إذا(gt) {
      /* the length of the timestamp, including the brackets */
      size_t timestamplen = gt - lt + 1;
      ثابت حرف *at = memchr(lt, '@', timestamplen);
      /* If the timestamp does not contain '@' it is not (as required by
         RFC-1939) conformant to the RFC-822 message id syntax, and we
         therefore do not use APOP authentication. */
      إذا(at) {
        /* dupe the timestamp */
        pop3c->apoptimestamp = curlx_memdup0(lt, timestamplen);
        إذا(!pop3c->apoptimestamp)
          أرجع CURLE_بیرون_از_حافظه;
        /* Store the APOP capability */
        pop3c->authtypes |= POP3_TYPE_APOP;
      }
    }

    إذا(!نتیجه)
      نتیجه = pop3_perform_capa(داده, conn);
  }

  أرجع نتیجه;
}

/* For CAPA responses */
ساكن CURLcode pop3_state_capa_resp(بنية Curl_easy *داده, صحيح pop3code,
                                     pop3state instate)
{
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ثابت حرف *سطر;
  size_t len;

  (فراغ)instate;
  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  سطر = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  /* Do we have an untagged continuation response? */
  إذا(pop3code == '*') {
    /* Does the server support the STLS capability? */
    إذا(len >= 4 && curl_strnequal(سطر, "STLS", 4))
      pop3c->tls_supported = TRUE;

    /* Does the server support clear text authentication? */
    وإلا إذا(len >= 4 && curl_strnequal(سطر, "USER", 4))
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    /* Does the server support SASL based authentication? */
    وإلا إذا(len >= 5 && curl_strnequal(سطر, "SASL ", 5)) {
      pop3c->authtypes |= POP3_TYPE_SASL;

      /* Advance past the SASL keyword */
      سطر += 5;
      len -= 5;

      /* Loop through the data line */
      لكل(;;) {
        size_t llen;
        size_t wordlen = 0;
        بیعلامت کوتاه mechbit;

        طالما(len && (ISBLANK(*سطر) || ISNEWLINE(*سطر))) {
          سطر++;
          len--;
        }

        إذا(!len)
          اكسر;

        /* Extract the word */
        طالما(wordlen < len && !ISBLANK(سطر[wordlen]) &&
              !ISNEWLINE(سطر[wordlen]))
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(سطر, wordlen, &llen);
        إذا(mechbit && llen == wordlen)
          pop3c->sasl.authmechs |= mechbit;

        سطر += wordlen;
        len -= wordlen;
      }
    }
  }
  وإلا {
    /* Clear text is supported when CAPA is not recognised */
    إذا(pop3code != '+')
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    إذا(!داده->مجموعه.use_ssl || Curl_conn_is_ssl(conn, FIRSTSOCKET))
      نتیجه = pop3_perform_authentication(داده, conn);
    وإلا إذا(pop3code == '+' && pop3c->tls_supported)
      /* Switch to TLS connection now */
      نتیجه = pop3_perform_starttls(داده, conn);
    وإلا إذا(داده->مجموعه.use_ssl <= CURLUSESSL_TRY)
      /* Fallback and carry on with authentication */
      نتیجه = pop3_perform_authentication(داده, conn);
    وإلا {
      failf(داده, "STLS not supported.");
      نتیجه = CURLE_USE_SSL_FAILED;
    }
  }

  أرجع نتیجه;
}

/* For STARTTLS responses */
ساكن CURLcode pop3_state_starttls_resp(بنية Curl_easy *داده,
                                         بنية connectdata *conn,
                                         صحيح pop3code,
                                         pop3state instate)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتیجه = CURLE_OK;
  (فراغ)instate;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  /* Pipelining in response is forbidden. */
  إذا(pop3c->pp.سرریز)
    أرجع CURLE_WEIRD_SERVER_پاسخ;

  إذا(pop3code != '+') {
    إذا(داده->مجموعه.use_ssl != CURLUSESSL_TRY) {
      failf(داده, "STARTTLS denied");
      نتیجه = CURLE_USE_SSL_FAILED;
    }
    وإلا
      نتیجه = pop3_perform_authentication(داده, conn);
  }
  وإلا
    pop3_state(داده, POP3_UPGRADETLS);

  أرجع نتیجه;
}

/* For SASL authentication responses */
ساكن CURLcode pop3_state_auth_resp(بنية Curl_easy *داده,
                                     صحيح pop3code,
                                     pop3state instate)
{
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  saslprogress پیشرفت;

  (فراغ)instate;
  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  نتیجه = Curl_sasl_continue(&pop3c->sasl, داده, pop3code, &پیشرفت);
  إذا(!نتیجه)
    بدل(پیشرفت) {
    حالة SASL_انجامشده:
      pop3_state(داده, POP3_STOP);  /* Authenticated */
      اكسر;
    حالة SASL_IDLE:            /* No mechanism left after cancellation */
#اگر_تعریف_نشده CURL_DISABLE_DIGEST_AUTH
      إذا(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
        /* Perform APOP authentication */
        نتیجه = pop3_perform_apop(داده, conn);
      وإلا
#پایان_اگر
      إذا(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
        /* Perform clear text authentication */
        نتیجه = pop3_perform_user(داده, conn);
      وإلا {
        failf(داده, "Authentication cancelled");
        نتیجه = CURLE_LOGIN_DENIED;
      }
      اكسر;
    افتراضي:
      اكسر;
    }

  أرجع نتیجه;
}

#اگر_تعریف_نشده CURL_DISABLE_DIGEST_AUTH
/* For APOP responses */
ساكن CURLcode pop3_state_apop_resp(بنية Curl_easy *داده, صحيح pop3code,
                                     pop3state instate)
{
  CURLcode نتیجه = CURLE_OK;
  (فراغ)instate;

  إذا(pop3code != '+') {
    failf(داده, "Authentication failed: %d", pop3code);
    نتیجه = CURLE_LOGIN_DENIED;
  }
  وإلا
    /* End of connect phase */
    pop3_state(داده, POP3_STOP);

  أرجع نتیجه;
}
#پایان_اگر

/* For USER responses */
ساكن CURLcode pop3_state_user_resp(بنية Curl_easy *داده, صحيح pop3code,
                                     pop3state instate)
{
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (فراغ)instate;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  إذا(pop3code != '+') {
    failf(داده, "Access denied. %c", pop3code);
    نتیجه = CURLE_LOGIN_DENIED;
  }
  وإلا
    /* Send the PASS command */
    نتیجه = Curl_pp_sendf(داده, &pop3c->pp, "PASS %s",
                           Curl_creds_passwd(conn->creds));
  إذا(!نتیجه)
    pop3_state(داده, POP3_PASS);

  أرجع نتیجه;
}

/* For PASS responses */
ساكن CURLcode pop3_state_pass_resp(بنية Curl_easy *داده, صحيح pop3code,
                                     pop3state instate)
{
  CURLcode نتیجه = CURLE_OK;
  (فراغ)instate;

  إذا(pop3code != '+') {
    failf(داده, "Access denied. %c", pop3code);
    نتیجه = CURLE_LOGIN_DENIED;
  }
  وإلا
    /* End of connect phase */
    pop3_state(داده, POP3_STOP);

  أرجع نتیجه;
}

/***********************************************************************
 *
 * pop3_write()
 *
 * This function scans the body after the end-of-body and writes everything
 * until the end is found.
 */
ساكن CURLcode pop3_write(بنية Curl_easy *داده, ثابت حرف *str,
                           size_t nread, bool is_eos)
{
  /* This code could be made into a special function in the handler struct */
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  bool strip_dot = FALSE;
  size_t آخرین = 0;
  size_t i;
  (فراغ)is_eos;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  /* Search through the buffer looking for the end-of-body marker which is
     5 bytes (0d 0a 2e 0d 0a). Note that a line starting with a dot matches
     the eob so the server will have prefixed it with an extra dot which we
     need to strip out. Additionally the marker could of course be spread out
     over 5 different data chunks. */
  لكل(i = 0; i < nread; i++) {
    size_t prev = pop3c->eob;

    بدل(str[i]) {
    حالة 0x0d:
      إذا(pop3c->eob == 0) {
        pop3c->eob++;

        إذا(i) {
          /* Write out the body part that did not match */
          نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, &str[آخرین],
                                     i - آخرین);

          إذا(نتیجه)
            أرجع نتیجه;

          آخرین = i;
        }
      }
      وإلا إذا(pop3c->eob == 3)
        pop3c->eob++;
      وإلا
        /* If the character match was not at position 0 or 3 then restart the
           pattern matching */
        pop3c->eob = 1;
      اكسر;

    حالة 0x0a:
      إذا(pop3c->eob == 1 || pop3c->eob == 4)
        pop3c->eob++;
      وإلا
        /* If the character match was not at position 1 or 4 then start the
           search again */
        pop3c->eob = 0;
      اكسر;

    حالة 0x2e:
      إذا(pop3c->eob == 2)
        pop3c->eob++;
      وإلا إذا(pop3c->eob == 3) {
        /* We have an extra dot after the CRLF which we need to strip off */
        strip_dot = TRUE;
        pop3c->eob = 0;
      }
      وإلا
        /* If the character match was not at position 2 then start the search
           again */
        pop3c->eob = 0;
      اكسر;

    افتراضي:
      pop3c->eob = 0;
      اكسر;
    }

    /* Did we have a partial match which has subsequently failed? */
    إذا(prev && prev >= pop3c->eob) {
      /* Strip can only be non-zero for the first mismatch after CRLF and
         then both prev and strip are equal and nothing will be output below */
      طالما(prev && pop3c->strip) {
        prev--;
        pop3c->strip--;
      }

      إذا(prev) {
        /* If the partial match was the CRLF and dot then only write the CRLF
           as the server would have inserted the dot */
        إذا(strip_dot && prev - 1 > 0) {
          نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, POP3_EOB,
                                     prev - 1);
        }
        وإلا إذا(!strip_dot) {
          نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, POP3_EOB,
                                     prev);
        }
        وإلا {
          نتیجه = CURLE_OK;
        }

        إذا(نتیجه)
          أرجع نتیجه;

        آخرین = i;
        strip_dot = FALSE;
      }
    }
  }

  إذا(pop3c->eob == POP3_EOB_LEN) {
    /* We have a full match so the transfer is done, however we must transfer
    the CRLF at the start of the EOB as this is considered to be part of the
    message as per RFC-1939, sect. 3 */
    نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, POP3_EOB, 2);

    CURL_REQ_CLEAR_RECV(داده);
    pop3c->eob = 0;

    أرجع نتیجه;
  }

  إذا(pop3c->eob)
    /* While EOB is matching nothing should be output */
    أرجع CURLE_OK;

  إذا(nread - آخرین) {
    نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, &str[آخرین],
                               nread - آخرین);
  }

  أرجع نتیجه;
}

/* For command responses */
ساكن CURLcode pop3_state_command_resp(بنية Curl_easy *داده,
                                        صحيح pop3code,
                                        pop3state instate)
{
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;
  بنية POP3 *pop3 = Curl_meta_get(داده, CURL_META_POP3_EASY);
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  بنية pingpong *pp;

  (فراغ)instate;
  إذا(!pop3 || !pop3c)
    أرجع CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  إذا(pop3code != '+') {
    pop3_state(داده, POP3_STOP);
    أرجع CURLE_WEIRD_SERVER_پاسخ;
  }

  /* This 'OK' line ends with a CR LF pair which is the two first bytes of the
     EOB string so count this is two matching bytes. This is necessary to make
     the code detect the EOB if the only data than comes now is %2e CR LF like
     when there is no body to return. */
  pop3c->eob = 2;

  /* Since this initial CR LF pair is not part of the actual body, we set
     the strip counter here so that these bytes will not be delivered. */
  pop3c->strip = 2;

  إذا(pop3->transfer == PPTRANSFER_BODY) {
    /* POP3 download */
    Curl_xfer_راهاندازی_recv(داده, FIRSTSOCKET, -1);

    إذا(pp->سرریز) {
      /* The recv buffer contains data that is actually body content so send
         it as such. Note that there may even be additional "headers" after
         the body */

      /* keep only the overflow */
      curlx_dyn_دم(&pp->recvbuf, pp->سرریز);
      pp->nfinal = 0; /* done */

      إذا(!داده->req.no_body) {
        نتیجه = pop3_write(داده, curlx_dyn_ptr(&pp->recvbuf),
                            curlx_dyn_len(&pp->recvbuf), FALSE);
        إذا(نتیجه)
          أرجع نتیجه;
      }

      /* reset the buffer */
      curlx_dyn_reset(&pp->recvbuf);
      pp->سرریز = 0;
    }
  }
  وإلا
    pp->سرریز = 0;

  /* End of DO phase */
  pop3_state(داده, POP3_STOP);

  أرجع نتیجه;
}

ساكن CURLcode pop3_statemachine(بنية Curl_easy *داده,
                                  بنية connectdata *conn)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتیجه = CURLE_OK;
  صحيح pop3code;
  بنية pingpong *pp;
  size_t nread = 0;
  (فراغ)داده;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not POP3 */
upgrade_tls:
  إذا(pop3c->state == POP3_UPGRADETLS) {
    نتیجه = pop3_perform_upgrade_tls(داده, conn);
    إذا(نتیجه || (pop3c->state == POP3_UPGRADETLS))
      أرجع نتیجه;
  }

  /* Flush any data that needs to be sent */
  إذا(pp->sendleft)
    أرجع Curl_pp_flushsend(داده, pp);

  افعل {
     /* Read the response from the server */
    نتیجه = Curl_pp_readresp(داده, FIRSTSOCKET, pp, &pop3code, &nread);
    إذا(نتیجه)
      أرجع نتیجه;

    إذا(!pop3code)
      اكسر;

    /* We have now received a full POP3 server response */
    بدل(pop3c->state) {
    حالة POP3_SERVERGREET:
      نتیجه = pop3_state_servergreet_resp(داده, pop3code, pop3c->state);
      اكسر;

    حالة POP3_CAPA:
      نتیجه = pop3_state_capa_resp(داده, pop3code, pop3c->state);
      اكسر;

    حالة POP3_STARTTLS:
      نتیجه = pop3_state_starttls_resp(داده, conn, pop3code, pop3c->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      إذا(!نتیجه && (pop3c->state == POP3_UPGRADETLS))
        اذهب upgrade_tls;
      اكسر;

    حالة POP3_AUTH:
      نتیجه = pop3_state_auth_resp(داده, pop3code, pop3c->state);
      اكسر;

#اگر_تعریف_نشده CURL_DISABLE_DIGEST_AUTH
    حالة POP3_APOP:
      نتیجه = pop3_state_apop_resp(داده, pop3code, pop3c->state);
      اكسر;
#پایان_اگر

    حالة POP3_USER:
      نتیجه = pop3_state_user_resp(داده, pop3code, pop3c->state);
      اكسر;

    حالة POP3_PASS:
      نتیجه = pop3_state_pass_resp(داده, pop3code, pop3c->state);
      اكسر;

    حالة POP3_COMMAND:
      نتیجه = pop3_state_command_resp(داده, pop3code, pop3c->state);
      اكسر;

    حالة POP3_QUIT:
      pop3_state(داده, POP3_STOP);
      اكسر;

    افتراضي:
      /* internal error */
      pop3_state(داده, POP3_STOP);
      اكسر;
    }
  } طالما(!نتیجه && pop3c->state != POP3_STOP && Curl_pp_moredata(pp));

  أرجع نتیجه;
}

/* Called repeatedly until done from multi.c */
ساكن CURLcode pop3_multi_statemach(بنية Curl_easy *داده, bool *انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;
  نتیجه = Curl_pp_statemach(داده, &pop3c->pp, FALSE, FALSE);
  *انجامشده = (pop3c->state == POP3_STOP);

  أرجع نتیجه;
}

ساكن CURLcode pop3_بلوک_statemach(بنية Curl_easy *داده,
                                     بنية connectdata *conn,
                                     bool disconnecting)
{
  CURLcode نتیجه = CURLE_OK;
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  طالما(pop3c->state != POP3_STOP && !نتیجه)
    نتیجه = Curl_pp_statemach(داده, &pop3c->pp, TRUE, disconnecting);

  أرجع نتیجه;
}

/* For the POP3 "protocol connect" and "doing" phases only */
ساكن CURLcode pop3_pollset(بنية Curl_easy *داده,
                             بنية easy_pollset *ps)
{
  بنية pop3_conn *pop3c =
    Curl_conn_meta_get(داده->conn, CURL_META_POP3_CONN);
  أرجع pop3c ? Curl_pp_pollset(داده, &pop3c->pp, ps) : CURLE_OK;
}

/* SASL parameters for the pop3 protocol */
ساكن ثابت بنية SASLproto saslpop3 = {
  "pop",                /* The service name */
  pop3_perform_auth,    /* Send authentication command */
  pop3_continue_auth,   /* Send authentication continuation */
  pop3_cancel_auth,     /* Send authentication cancellation */
  pop3_get_پیام,     /* Get SASL response message */
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
ساكن CURLcode pop3_connect(بنية Curl_easy *داده, bool *انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  بنية pingpong *pp = pop3c ? &pop3c->pp : عدم;

  *انجامشده = FALSE; /* default to not done yet */
  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  PINGPONG_راهاندازی(pp, pop3_statemachine, pop3_endofresp);

  /* Set the default preferred authentication type and mechanism */
  pop3c->preftype = POP3_TYPE_ANY;
  Curl_sasl_init(&pop3c->sasl, داده, &saslpop3);

  /* Initialize the pingpong layer */
  Curl_pp_init(pp, Curl_pgrs_now(داده));

  /* Parse the URL options */
  نتیجه = pop3_parse_url_options(conn);
  إذا(نتیجه)
    أرجع نتیجه;

  /* Start off waiting for the server greeting response */
  pop3_state(داده, POP3_SERVERGREET);

  نتیجه = pop3_multi_statemach(داده, انجامشده);

  أرجع نتیجه;
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
ساكن CURLcode pop3_انجامشده(بنية Curl_easy *داده, CURLcode وضعیت,
                          bool premature)
{
  CURLcode نتیجه = CURLE_OK;
  بنية POP3 *pop3 = Curl_meta_get(داده, CURL_META_POP3_EASY);

  (فراغ)premature;

  إذا(!pop3)
    أرجع CURLE_OK;

  إذا(وضعیت) {
    CURL_TRC_M(داده, "POP3 done with bad status");
    connclose(داده->conn);
    نتیجه = وضعیت;         /* use the already set error code */
  }

  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);

  /* Clear the transfer mode for the next request */
  pop3->transfer = PPTRANSFER_BODY;

  أرجع نتیجه;
}

/***********************************************************************
 *
 * pop3_perform()
 *
 * This is the actual DO function for POP3. Get a message/listing according to
 * the options previously setup.
 */
ساكن CURLcode pop3_perform(بنية Curl_easy *داده, bool *connected,
                             bool *dophase_انجامشده)
{
  /* This is POP3 and no proxy */
  CURLcode نتیجه = CURLE_OK;
  بنية POP3 *pop3 = Curl_meta_get(داده, CURL_META_POP3_EASY);

  إذا(!pop3)
    أرجع CURLE_FAILED_INIT;

  DEBUGF(infof(داده, "DO phase starts"));

  /* Start the first command in the DO phase, may alter data->req.no_body */
  نتیجه = pop3_perform_command(داده);
  إذا(نتیجه)
    أرجع نتیجه;

  إذا(داده->req.no_body)
    /* Requested no body means no transfer */
    pop3->transfer = PPTRANSFER_INFO;

  *dophase_انجامشده = FALSE; /* not done yet */

  /* Run the state-machine */
  نتیجه = pop3_multi_statemach(داده, dophase_انجامشده);
  *connected = Curl_conn_is_connected(داده->conn, FIRSTSOCKET);

  إذا(*dophase_انجامشده)
    DEBUGF(infof(داده, "DO phase is complete"));

  أرجع نتیجه;
}

/* Call this when the DO phase has completed */
ساكن CURLcode pop3_dophase_انجامشده(بنية Curl_easy *داده, bool connected)
{
  (فراغ)داده;
  (فراغ)connected;

  أرجع CURLE_OK;
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
ساكن CURLcode pop3_regular_transfer(بنية Curl_easy *داده,
                                      bool *dophase_انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  داده->req.اندازه = -1;

  /* Set the progress data */
  Curl_pgrsReset(داده);

  /* Carry out the perform */
  نتیجه = pop3_perform(داده, &connected, dophase_انجامشده);

  /* Perform post DO phase operations if necessary */
  إذا(!نتیجه && *dophase_انجامشده)
    نتیجه = pop3_dophase_انجامشده(داده, connected);

  أرجع نتیجه;
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
ساكن CURLcode pop3_do(بنية Curl_easy *داده, bool *انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  *انجامشده = FALSE; /* default to false */

  /* Parse the URL path */
  نتیجه = pop3_parse_url_مسیر(داده);
  إذا(نتیجه)
    أرجع نتیجه;

  /* Parse the custom request */
  نتیجه = pop3_parse_custom_request(داده);
  إذا(نتیجه)
    أرجع نتیجه;

  نتیجه = pop3_regular_transfer(داده, انجامشده);

  أرجع نتیجه;
}

/***********************************************************************
 *
 * pop3_disconnect()
 *
 * Disconnect from an POP3 server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ساكن CURLcode pop3_disconnect(بنية Curl_easy *داده,
                                بنية connectdata *conn, bool dead_connection)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (فراغ)داده;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  إذا(!dead_connection && conn->بیتها.protoconnstart &&
     !Curl_pp_needs_flush(داده, &pop3c->pp) &&
     !pop3_perform_quit(داده, conn))
    (فراغ)pop3_بلوک_statemach(داده, conn, TRUE); /* ignore errors on QUIT */

  /* Disconnect from the server */
  Curl_pp_disconnect(&pop3c->pp);

  /* Cleanup our connection based variables */
  curlx_safefree(pop3c->apoptimestamp);

  أرجع CURLE_OK;
}

/* Called from multi.c while DOing */
ساكن CURLcode pop3_doing(بنية Curl_easy *داده, bool *dophase_انجامشده)
{
  CURLcode نتیجه = pop3_multi_statemach(داده, dophase_انجامشده);

  إذا(نتیجه)
    DEBUGF(infof(داده, "DO phase failed"));
  وإلا إذا(*dophase_انجامشده) {
    نتیجه = pop3_dophase_انجامشده(داده, FALSE /* not connected */);

    DEBUGF(infof(داده, "DO phase is complete"));
  }

  أرجع نتیجه;
}

ساكن فراغ pop3_easy_dtor(ثابت فراغ *کلید, size_t klen, فراغ *مدخل)
{
  بنية POP3 *pop3 = مدخل;
  (فراغ)کلید;
  (فراغ)klen;
  DEBUGASSERT(pop3);
  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);
  curlx_free(pop3);
}

ساكن فراغ pop3_conn_dtor(ثابت فراغ *کلید, size_t klen, فراغ *مدخل)
{
  بنية pop3_conn *pop3c = مدخل;
  (فراغ)کلید;
  (فراغ)klen;
  DEBUGASSERT(pop3c);
  Curl_pp_disconnect(&pop3c->pp);
  curlx_safefree(pop3c->apoptimestamp);
  curlx_free(pop3c);
}

ساكن CURLcode pop3_راهاندازی_connection(بنية Curl_easy *داده,
                                      بنية connectdata *conn)
{
  بنية pop3_conn *pop3c;
  بنية POP3 *pop3 = curlx_calloc(1, حجم(*pop3));
  إذا(!pop3 ||
     Curl_meta_مجموعه(داده, CURL_META_POP3_EASY, pop3, pop3_easy_dtor))
    أرجع CURLE_بیرون_از_حافظه;

  pop3c = curlx_calloc(1, حجم(*pop3c));
  إذا(!pop3c ||
     Curl_conn_meta_مجموعه(conn, CURL_META_POP3_CONN, pop3c, pop3_conn_dtor))
    أرجع CURLE_بیرون_از_حافظه;

  أرجع CURLE_OK;
}

/*
 * POP3 protocol.
 */
ثابت بنية Curl_protocol Curl_protocol_pop3 = {
  pop3_راهاندازی_connection,            /* setup_connection */
  pop3_do,                          /* do_it */
  pop3_انجامشده,                        /* done */
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

#پایان_اگر /* CURL_DISABLE_POP3 */
