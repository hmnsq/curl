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

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_POP3

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#ضمن <netinet/in.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NETDB_H
#ضمن <netdb.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جيڪڏهن_پورو

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
#وصف_ڪريو POP3_TYPE_CLEARTEXT (1 << 0)
#وصف_ڪريو POP3_TYPE_APOP      (1 << 1)
#وصف_ڪريو POP3_TYPE_SASL      (1 << 2)

/* Authentication type values */
#وصف_ڪريو POP3_TYPE_NONE 0
#وصف_ڪريو POP3_TYPE_ANY  (POP3_TYPE_CLEARTEXT | POP3_TYPE_APOP | POP3_TYPE_SASL)

/* This is the 5-bytes End-Of-Body marker for POP3 */
#وصف_ڪريو POP3_EOB     "\x0d\x0a\x2e\x0d\x0a"
#وصف_ڪريو POP3_EOB_LEN 5

/* meta key for storing protocol meta at easy handle */
#وصف_ڪريو CURL_META_POP3_EASY   "meta:proto:pop3:easy"
/* meta key for storing protocol meta at connection */
#وصف_ڪريو CURL_META_POP3_CONN   "meta:proto:pop3:conn"

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
  POP3_آخري          /* never used */
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
  بنانشان حرف authtypes; /* Accepted authentication types */
  بنانشان حرف preftype;  /* Preferred authentication type */
  ٻٽو_انگ(ssldone);           /* Is connect() over SSL done? */
  ٻٽو_انگ(tls_supported);     /* StartTLS capability supported by server */
};

بنية pop3_cmd {
  ثابت حرف *name;
  بنانشان ننڍو nlen;
  ٻٽو_انگ(multiline); /* response is multi-line with last '.' line */
  ٻٽو_انگ(multiline_with_دليلون); /* is multi-line when command has args */
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
  CURLcode نتيجو = CURLE_OK;
  ثابت حرف *ptr = conn->options;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  طالما(!نتيجو && ptr && *ptr) {
    ثابت حرف *ڪنجي = ptr;
    ثابت حرف *قدر;

    طالما(*ptr && *ptr != '=')
      ptr++;

    قدر = ptr + 1;

    طالما(*ptr && *ptr != ';')
      ptr++;

    إذا(curl_strnequal(ڪنجي, "AUTH=", 5)) {
      نتيجو = Curl_sasl_parse_url_auth_option(&pop3c->sasl,
                                               قدر, ptr - قدر);

      إذا(نتيجو && curl_strnequal(قدر, "+APOP", ptr - قدر)) {
        pop3c->preftype = POP3_TYPE_APOP;
        pop3c->sasl.prefmech = SASL_AUTH_NONE;
        نتيجو = CURLE_OK;
      }
    }
    وإلا
      نتيجو = CURLE_URL_MALFORMAT;

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

  أرجع نتيجو;
}

/***********************************************************************
 *
 * pop3_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
ساكن CURLcode pop3_parse_url_رستو(بنية Curl_easy *معلومات)
{
  /* The POP3 struct is already initialized in pop3_connect() */
  بنية POP3 *pop3 = Curl_meta_get(معلومات, CURL_META_POP3_EASY);
  ثابت حرف *رستو = &معلومات->state.مٿي.رستو[1]; /* skip leading path */

  إذا(!pop3)
    أرجع CURLE_FAILED_INIT;
  /* URL decode the path for the message ID */
  أرجع Curl_urldecode(رستو, 0, &pop3->id, عدم, REJECT_CTRL);
}

/***********************************************************************
 *
 * pop3_parse_custom_request()
 *
 * Parse the custom request.
 */
ساكن CURLcode pop3_parse_custom_request(بنية Curl_easy *معلومات)
{
  CURLcode نتيجو = CURLE_OK;
  بنية POP3 *pop3 = Curl_meta_get(معلومات, CURL_META_POP3_EASY);
  ثابت حرف *custom = CURL_EASY_STR(معلومات, STRING_CUSTOMREQUEST);

  إذا(!pop3)
    أرجع CURLE_FAILED_INIT;
  /* URL decode the custom request */
  إذا(custom)
    نتيجو = Curl_urldecode(custom, 0, &pop3->custom, عدم, REJECT_CTRL);

  أرجع نتيجو;
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
        أرجع (bool)pop3cmds[i].multiline_with_دليلون;
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
ساكن bool pop3_endofresp(بنية Curl_easy *معلومات, بنية connectdata *conn,
                           ثابت حرف *سٽ, size_t len, صحيح *resp)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (فراغ)معلومات;
  DEBUGASSERT(pop3c);
  إذا(!pop3c) /* internal error */
    أرجع TRUE;

  /* Do we have an error response? */
  إذا(len >= 4 && !memcmp("-ERR", سٽ, 4)) {
    *resp = '-';

    أرجع TRUE;
  }

  /* Are we processing CAPA command responses? */
  إذا(pop3c->state == POP3_CAPA) {
    /* Do we have the terminating line? Per RFC 2449 this is a line
       containing only a single dot */
    إذا((len == 3 && سٽ[0] == '.' && سٽ[1] == '\r') ||
       (len == 2 && سٽ[0] == '.' && سٽ[1] == '\n'))
      /* Treat the response as a success */
      *resp = '+';
    وإلا
      /* Treat the response as an untagged continuation */
      *resp = '*';

    أرجع TRUE;
  }

  /* Do we have a success response? */
  إذا(len >= 3 && !memcmp("+OK", سٽ, 3)) {
    *resp = '+';

    أرجع TRUE;
  }

  /* Do we have a continuation response? */
  إذا(len >= 1 && سٽ[0] == '+') {
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
ساكن CURLcode pop3_get_پيغام(بنية Curl_easy *معلومات, بنية bufref *ٻاهريون)
{
  بنية pop3_conn *pop3c =
    Curl_conn_meta_get(معلومات->conn, CURL_META_POP3_CONN);
  حرف *پيغام;
  size_t len;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;
  پيغام = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;
  إذا(len > 2) {
    /* Find the start of the message */
    len -= 2;
    لكل(پيغام += 2; ISBLANK(*پيغام); پيغام++, len--)
      ;

    /* Find the end of the message */
    طالما(len--)
      إذا(!ISBLANK(پيغام[len]) && !ISNEWLINE(پيغام[len]))
        اكسر;

    /* Terminate the message */
    پيغام[++len] = '\0';
    Curl_bufref_سيٽ(ٻاهريون, پيغام, len, عدم);
  }
  وإلا
    /* junk input => zero length output */
    Curl_bufref_سيٽ(ٻاهريون, "", 0, عدم);

  أرجع CURLE_OK;
}

/***********************************************************************
 *
 * pop3_state()
 *
 * This is the ONLY way to change POP3 state!
 */
ساكن فراغ pop3_state(بنية Curl_easy *معلومات, pop3state newstate)
{
  بنية pop3_conn *pop3c =
    Curl_conn_meta_get(معلومات->conn, CURL_META_POP3_CONN);
  إذا(pop3c) {
#إذا defined(DEBUGBUILD) && defined(CURLVERBOSE)
    /* for debug purposes */
    ساكن ثابت حرف * ثابت نالا[] = {
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
      /*آخري*/
    };

    إذا(pop3c->state != newstate)
      infof(معلومات, "POP3 %p state change from %s to %s",
            (فراغ *)pop3c, نالا[pop3c->state], نالا[newstate]);
#جيڪڏهن_پورو

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
ساكن CURLcode pop3_perform_capa(بنية Curl_easy *معلومات,
                                  بنية connectdata *conn)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتيجو = CURLE_OK;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  pop3c->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  pop3c->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  pop3c->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPA command */
  نتيجو = Curl_pp_sendf(معلومات, &pop3c->pp, "%s", "CAPA");

  إذا(!نتيجو)
    pop3_state(معلومات, POP3_CAPA);

  أرجع نتيجو;
}

/***********************************************************************
 *
 * pop3_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
ساكن CURLcode pop3_perform_starttls(بنية Curl_easy *معلومات,
                                      بنية connectdata *conn)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتيجو;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  /* Send the STLS command */
  نتيجو = Curl_pp_sendf(معلومات, &pop3c->pp, "%s", "STLS");
  إذا(!نتيجو)
    pop3_state(معلومات, POP3_STARTTLS);

  أرجع نتيجو;
}

/***********************************************************************
 *
 * pop3_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ساكن CURLcode pop3_perform_upgrade_tls(بنية Curl_easy *معلومات,
                                         بنية connectdata *conn)
{
#جيڪڏهن_وصف_ٿيل USE_SSL
  /* Start the SSL connection */
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتيجو;
  bool ssldone = FALSE;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  إذا(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    نتيجو = Curl_ssl_cfilter_جوڙيو(
      معلومات, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    إذا(نتيجو)
      اذهب ٻاهريون;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_pop3s;
  }

  DEBUGASSERT(!pop3c->ssldone);
  نتيجو = Curl_conn_connect(معلومات, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(معلومات, "pop3_perform_upgrade_tls, connect -> %d, %d",
               (صحيح)نتيجو, ssldone));
  إذا(!نتيجو && ssldone) {
    pop3c->ssldone = ssldone;
    /* perform CAPA now, changes pop3c->state out of POP3_UPGRADETLS */
    نتيجو = pop3_perform_capa(معلومات, conn);
  }
ٻاهريون:
  أرجع نتيجو;
#وإلا
  (فراغ)معلومات;
  (فراغ)conn;
  أرجع CURLE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
}

/***********************************************************************
 *
 * pop3_perform_user()
 *
 * Sends a clear text USER command to authenticate with.
 */
ساكن CURLcode pop3_perform_user(بنية Curl_easy *معلومات,
                                  بنية connectdata *conn)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتيجو = CURLE_OK;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  إذا(!conn->creds) {
    pop3_state(معلومات, POP3_STOP);

    أرجع نتيجو;
  }

  /* Send the USER command */
  نتيجو = Curl_pp_sendf(معلومات, &pop3c->pp, "USER %s",
                         Curl_creds_user(conn->creds));
  إذا(!نتيجو)
    pop3_state(معلومات, POP3_USER);

  أرجع نتيجو;
}

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DIGEST_AUTH
/***********************************************************************
 *
 * pop3_perform_apop()
 *
 * Sends an APOP command to authenticate with.
 */
ساكن CURLcode pop3_perform_apop(بنية Curl_easy *معلومات,
                                  بنية connectdata *conn)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتيجو = CURLE_OK;
  size_t i;
  بنية MD5_context *ctxt;
  بنانشان حرف digest[MD5_DIGEST_LEN];
  حرف secret[(2 * MD5_DIGEST_LEN) + 1];

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  إذا(!معلومات->state.creds) {
    pop3_state(معلومات, POP3_STOP);

    أرجع نتيجو;
  }

  /* Create the digest */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  إذا(!ctxt)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  Curl_MD5_update(ctxt, (ثابت بنانشان حرف *)pop3c->apoptimestamp,
                  curlx_uztoui(لڙي_ڊيگهه(pop3c->apoptimestamp)));

  Curl_MD5_update(ctxt, (ثابت بنانشان حرف *)Curl_creds_passwd(conn->creds),
                  curlx_uztoui(لڙي_ڊيگهه(Curl_creds_passwd(conn->creds))));

  /* Finalise the digest */
  Curl_MD5_final(ctxt, digest);

  /* Convert the calculated 16 octet digest into a 32-byte hex string */
  لكل(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&secret[2 * i], 3, "%02x", digest[i]);

  نتيجو = Curl_pp_sendf(معلومات, &pop3c->pp, "APOP %s %s",
                         Curl_creds_user(conn->creds), secret);

  إذا(!نتيجو)
    pop3_state(معلومات, POP3_APOP);

  أرجع نتيجو;
}
#جيڪڏهن_پورو

/***********************************************************************
 *
 * pop3_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
ساكن CURLcode pop3_perform_auth(بنية Curl_easy *معلومات,
                                  ثابت حرف *mech,
                                  ثابت بنية bufref *initresp)
{
  بنية pop3_conn *pop3c =
    Curl_conn_meta_get(معلومات->conn, CURL_META_POP3_CONN);
  CURLcode نتيجو = CURLE_OK;
  ثابت حرف *ir = Curl_bufref_ptr(initresp);

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  إذا(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    نتيجو = Curl_pp_sendf(معلومات, &pop3c->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  وإلا {
    /* Send the AUTH command */
    نتيجو = Curl_pp_sendf(معلومات, &pop3c->pp, "AUTH %s", mech);
  }

  أرجع نتيجو;
}

/***********************************************************************
 *
 * pop3_continue_auth()
 *
 * Sends SASL continuation data.
 */
ساكن CURLcode pop3_continue_auth(بنية Curl_easy *معلومات,
                                   ثابت حرف *mech,
                                   ثابت بنية bufref *resp)
{
  بنية pop3_conn *pop3c =
    Curl_conn_meta_get(معلومات->conn, CURL_META_POP3_CONN);

  (فراغ)mech;
  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  أرجع Curl_pp_sendf(معلومات, &pop3c->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * pop3_cancel_auth()
 *
 * Sends SASL cancellation.
 */
ساكن CURLcode pop3_cancel_auth(بنية Curl_easy *معلومات, ثابت حرف *mech)
{
  بنية pop3_conn *pop3c =
    Curl_conn_meta_get(معلومات->conn, CURL_META_POP3_CONN);

  (فراغ)mech;
  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  أرجع Curl_pp_sendf(معلومات, &pop3c->pp, "*");
}

/***********************************************************************
 *
 * pop3_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to APOP and clear text should a
 * common mechanism not be available between the client and server.
 */
ساكن CURLcode pop3_perform_authentication(بنية Curl_easy *معلومات,
                                            بنية connectdata *conn)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتيجو = CURLE_OK;
  saslprogress ترقي = SASL_IDLE;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  /* Check we have enough data to authenticate with and end the
     connect phase if we do not */
  إذا(!Curl_sasl_can_authenticate(&pop3c->sasl, معلومات)) {
    pop3_state(معلومات, POP3_STOP);
    أرجع نتيجو;
  }

  إذا(pop3c->authtypes & pop3c->preftype & POP3_TYPE_SASL) {
    /* Calculate the SASL login details */
    نتيجو = Curl_sasl_شروعات(&pop3c->sasl, معلومات, FALSE, &ترقي);

    إذا(!نتيجو && ترقي == SASL_INPROGRESS)
      pop3_state(معلومات, POP3_AUTH);
  }

  إذا(!نتيجو && ترقي == SASL_IDLE) {
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DIGEST_AUTH
    إذا(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
      /* Perform APOP authentication */
      نتيجو = pop3_perform_apop(معلومات, conn);
    وإلا
#جيڪڏهن_پورو
    إذا(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
      /* Perform clear text authentication */
      نتيجو = pop3_perform_user(معلومات, conn);
    وإلا
      نتيجو = Curl_sasl_is_blocked(&pop3c->sasl, معلومات);
  }

  أرجع نتيجو;
}

/***********************************************************************
 *
 * pop3_perform_command()
 *
 * Sends a POP3 based command.
 */
ساكن CURLcode pop3_perform_command(بنية Curl_easy *معلومات)
{
  CURLcode نتيجو = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;
  بنية POP3 *pop3 = Curl_meta_get(معلومات, CURL_META_POP3_EASY);
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ثابت حرف *command = عدم;

  إذا(!pop3 || !pop3c)
    أرجع CURLE_FAILED_INIT;

  /* Calculate the default command */
  إذا(pop3->id[0] == '\0' || معلومات->سيٽ.list_only) {
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
    نتيجو = Curl_pp_sendf(معلومات, &pop3c->pp, "%s %s", command, pop3->id);
  وإلا
    نتيجو = Curl_pp_sendf(معلومات, &pop3c->pp, "%s", command);

  إذا(!نتيجو) {
    pop3_state(معلومات, POP3_COMMAND);
    معلومات->req.no_body = !pop3_is_multiline(command);
  }

  أرجع نتيجو;
}

/***********************************************************************
 *
 * pop3_perform_quit()
 *
 * Performs the quit action prior to sclose() be called.
 */
ساكن CURLcode pop3_perform_quit(بنية Curl_easy *معلومات,
                                  بنية connectdata *conn)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتيجو;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  /* Send the QUIT command */
  نتيجو = Curl_pp_sendf(معلومات, &pop3c->pp, "%s", "QUIT");
  إذا(!نتيجو)
    pop3_state(معلومات, POP3_QUIT);

  أرجع نتيجو;
}

/* For the initial server greeting */
ساكن CURLcode pop3_state_servergreet_resp(بنية Curl_easy *معلومات,
                                            صحيح pop3code,
                                            pop3state instate)
{
  CURLcode نتيجو = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ثابت حرف *سٽ;
  size_t len;

  (فراغ)instate;
  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  سٽ = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  إذا(pop3code != '+') {
    failf(معلومات, "Got unexpected pop3-server response");
    نتيجو = CURLE_WEIRD_SERVER_جواب;
  }
  وإلا إذا(len > 3) {
    /* Does the server support APOP authentication? */
    ثابت حرف *lt;
    ثابت حرف *gt = عدم;

    /* Look for the APOP timestamp */
    lt = memchr(سٽ, '<', len);
    إذا(lt)
      /* search the remainder for '>' */
      gt = memchr(lt, '>', len - (lt - سٽ));
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
          أرجع CURLE_ٻاهريون_جو_يادگيري;
        /* Store the APOP capability */
        pop3c->authtypes |= POP3_TYPE_APOP;
      }
    }

    إذا(!نتيجو)
      نتيجو = pop3_perform_capa(معلومات, conn);
  }

  أرجع نتيجو;
}

/* For CAPA responses */
ساكن CURLcode pop3_state_capa_resp(بنية Curl_easy *معلومات, صحيح pop3code,
                                     pop3state instate)
{
  CURLcode نتيجو = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ثابت حرف *سٽ;
  size_t len;

  (فراغ)instate;
  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  سٽ = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  /* Do we have an untagged continuation response? */
  إذا(pop3code == '*') {
    /* Does the server support the STLS capability? */
    إذا(len >= 4 && curl_strnequal(سٽ, "STLS", 4))
      pop3c->tls_supported = TRUE;

    /* Does the server support clear text authentication? */
    وإلا إذا(len >= 4 && curl_strnequal(سٽ, "USER", 4))
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    /* Does the server support SASL based authentication? */
    وإلا إذا(len >= 5 && curl_strnequal(سٽ, "SASL ", 5)) {
      pop3c->authtypes |= POP3_TYPE_SASL;

      /* Advance past the SASL keyword */
      سٽ += 5;
      len -= 5;

      /* Loop through the data line */
      لكل(;;) {
        size_t llen;
        size_t wordlen = 0;
        بنانشان ننڍو mechbit;

        طالما(len && (ISBLANK(*سٽ) || ISNEWLINE(*سٽ))) {
          سٽ++;
          len--;
        }

        إذا(!len)
          اكسر;

        /* Extract the word */
        طالما(wordlen < len && !ISBLANK(سٽ[wordlen]) &&
              !ISNEWLINE(سٽ[wordlen]))
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(سٽ, wordlen, &llen);
        إذا(mechbit && llen == wordlen)
          pop3c->sasl.authmechs |= mechbit;

        سٽ += wordlen;
        len -= wordlen;
      }
    }
  }
  وإلا {
    /* Clear text is supported when CAPA is not recognised */
    إذا(pop3code != '+')
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    إذا(!معلومات->سيٽ.use_ssl || Curl_conn_is_ssl(conn, FIRSTSOCKET))
      نتيجو = pop3_perform_authentication(معلومات, conn);
    وإلا إذا(pop3code == '+' && pop3c->tls_supported)
      /* Switch to TLS connection now */
      نتيجو = pop3_perform_starttls(معلومات, conn);
    وإلا إذا(معلومات->سيٽ.use_ssl <= CURLUSESSL_TRY)
      /* Fallback and carry on with authentication */
      نتيجو = pop3_perform_authentication(معلومات, conn);
    وإلا {
      failf(معلومات, "STLS not supported.");
      نتيجو = CURLE_USE_SSL_FAILED;
    }
  }

  أرجع نتيجو;
}

/* For STARTTLS responses */
ساكن CURLcode pop3_state_starttls_resp(بنية Curl_easy *معلومات,
                                         بنية connectdata *conn,
                                         صحيح pop3code,
                                         pop3state instate)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتيجو = CURLE_OK;
  (فراغ)instate;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  /* Pipelining in response is forbidden. */
  إذا(pop3c->pp.حد_کان_مٿي)
    أرجع CURLE_WEIRD_SERVER_جواب;

  إذا(pop3code != '+') {
    إذا(معلومات->سيٽ.use_ssl != CURLUSESSL_TRY) {
      failf(معلومات, "STARTTLS denied");
      نتيجو = CURLE_USE_SSL_FAILED;
    }
    وإلا
      نتيجو = pop3_perform_authentication(معلومات, conn);
  }
  وإلا
    pop3_state(معلومات, POP3_UPGRADETLS);

  أرجع نتيجو;
}

/* For SASL authentication responses */
ساكن CURLcode pop3_state_auth_resp(بنية Curl_easy *معلومات,
                                     صحيح pop3code,
                                     pop3state instate)
{
  CURLcode نتيجو = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  saslprogress ترقي;

  (فراغ)instate;
  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  نتيجو = Curl_sasl_continue(&pop3c->sasl, معلومات, pop3code, &ترقي);
  إذا(!نتيجو)
    بدل(ترقي) {
    حالة SASL_پورو:
      pop3_state(معلومات, POP3_STOP);  /* Authenticated */
      اكسر;
    حالة SASL_IDLE:            /* No mechanism left after cancellation */
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DIGEST_AUTH
      إذا(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
        /* Perform APOP authentication */
        نتيجو = pop3_perform_apop(معلومات, conn);
      وإلا
#جيڪڏهن_پورو
      إذا(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
        /* Perform clear text authentication */
        نتيجو = pop3_perform_user(معلومات, conn);
      وإلا {
        failf(معلومات, "Authentication cancelled");
        نتيجو = CURLE_LOGIN_DENIED;
      }
      اكسر;
    افتراضي:
      اكسر;
    }

  أرجع نتيجو;
}

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DIGEST_AUTH
/* For APOP responses */
ساكن CURLcode pop3_state_apop_resp(بنية Curl_easy *معلومات, صحيح pop3code,
                                     pop3state instate)
{
  CURLcode نتيجو = CURLE_OK;
  (فراغ)instate;

  إذا(pop3code != '+') {
    failf(معلومات, "Authentication failed: %d", pop3code);
    نتيجو = CURLE_LOGIN_DENIED;
  }
  وإلا
    /* End of connect phase */
    pop3_state(معلومات, POP3_STOP);

  أرجع نتيجو;
}
#جيڪڏهن_پورو

/* For USER responses */
ساكن CURLcode pop3_state_user_resp(بنية Curl_easy *معلومات, صحيح pop3code,
                                     pop3state instate)
{
  CURLcode نتيجو = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (فراغ)instate;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  إذا(pop3code != '+') {
    failf(معلومات, "Access denied. %c", pop3code);
    نتيجو = CURLE_LOGIN_DENIED;
  }
  وإلا
    /* Send the PASS command */
    نتيجو = Curl_pp_sendf(معلومات, &pop3c->pp, "PASS %s",
                           Curl_creds_passwd(conn->creds));
  إذا(!نتيجو)
    pop3_state(معلومات, POP3_PASS);

  أرجع نتيجو;
}

/* For PASS responses */
ساكن CURLcode pop3_state_pass_resp(بنية Curl_easy *معلومات, صحيح pop3code,
                                     pop3state instate)
{
  CURLcode نتيجو = CURLE_OK;
  (فراغ)instate;

  إذا(pop3code != '+') {
    failf(معلومات, "Access denied. %c", pop3code);
    نتيجو = CURLE_LOGIN_DENIED;
  }
  وإلا
    /* End of connect phase */
    pop3_state(معلومات, POP3_STOP);

  أرجع نتيجو;
}

/***********************************************************************
 *
 * pop3_write()
 *
 * This function scans the body after the end-of-body and writes everything
 * until the end is found.
 */
ساكن CURLcode pop3_write(بنية Curl_easy *معلومات, ثابت حرف *str,
                           size_t nread, bool is_eos)
{
  /* This code could be made into a special function in the handler struct */
  CURLcode نتيجو = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  bool strip_dot = FALSE;
  size_t آخري = 0;
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
          نتيجو = Curl_client_write(معلومات, CLIENTWRITE_BODY, &str[آخري],
                                     i - آخري);

          إذا(نتيجو)
            أرجع نتيجو;

          آخري = i;
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
          نتيجو = Curl_client_write(معلومات, CLIENTWRITE_BODY, POP3_EOB,
                                     prev - 1);
        }
        وإلا إذا(!strip_dot) {
          نتيجو = Curl_client_write(معلومات, CLIENTWRITE_BODY, POP3_EOB,
                                     prev);
        }
        وإلا {
          نتيجو = CURLE_OK;
        }

        إذا(نتيجو)
          أرجع نتيجو;

        آخري = i;
        strip_dot = FALSE;
      }
    }
  }

  إذا(pop3c->eob == POP3_EOB_LEN) {
    /* We have a full match so the transfer is done, however we must transfer
    the CRLF at the start of the EOB as this is considered to be part of the
    message as per RFC-1939, sect. 3 */
    نتيجو = Curl_client_write(معلومات, CLIENTWRITE_BODY, POP3_EOB, 2);

    CURL_REQ_CLEAR_RECV(معلومات);
    pop3c->eob = 0;

    أرجع نتيجو;
  }

  إذا(pop3c->eob)
    /* While EOB is matching nothing should be output */
    أرجع CURLE_OK;

  إذا(nread - آخري) {
    نتيجو = Curl_client_write(معلومات, CLIENTWRITE_BODY, &str[آخري],
                               nread - آخري);
  }

  أرجع نتيجو;
}

/* For command responses */
ساكن CURLcode pop3_state_command_resp(بنية Curl_easy *معلومات,
                                        صحيح pop3code,
                                        pop3state instate)
{
  CURLcode نتيجو = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;
  بنية POP3 *pop3 = Curl_meta_get(معلومات, CURL_META_POP3_EASY);
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  بنية pingpong *pp;

  (فراغ)instate;
  إذا(!pop3 || !pop3c)
    أرجع CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  إذا(pop3code != '+') {
    pop3_state(معلومات, POP3_STOP);
    أرجع CURLE_WEIRD_SERVER_جواب;
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
    Curl_xfer_تياري_recv(معلومات, FIRSTSOCKET, -1);

    إذا(pp->حد_کان_مٿي) {
      /* The recv buffer contains data that is actually body content so send
         it as such. Note that there may even be additional "headers" after
         the body */

      /* keep only the overflow */
      curlx_dyn_پڇ(&pp->recvbuf, pp->حد_کان_مٿي);
      pp->nfinal = 0; /* done */

      إذا(!معلومات->req.no_body) {
        نتيجو = pop3_write(معلومات, curlx_dyn_ptr(&pp->recvbuf),
                            curlx_dyn_len(&pp->recvbuf), FALSE);
        إذا(نتيجو)
          أرجع نتيجو;
      }

      /* reset the buffer */
      curlx_dyn_reset(&pp->recvbuf);
      pp->حد_کان_مٿي = 0;
    }
  }
  وإلا
    pp->حد_کان_مٿي = 0;

  /* End of DO phase */
  pop3_state(معلومات, POP3_STOP);

  أرجع نتيجو;
}

ساكن CURLcode pop3_statemachine(بنية Curl_easy *معلومات,
                                  بنية connectdata *conn)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode نتيجو = CURLE_OK;
  صحيح pop3code;
  بنية pingpong *pp;
  size_t nread = 0;
  (فراغ)معلومات;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not POP3 */
upgrade_tls:
  إذا(pop3c->state == POP3_UPGRADETLS) {
    نتيجو = pop3_perform_upgrade_tls(معلومات, conn);
    إذا(نتيجو || (pop3c->state == POP3_UPGRADETLS))
      أرجع نتيجو;
  }

  /* Flush any data that needs to be sent */
  إذا(pp->sendleft)
    أرجع Curl_pp_flushsend(معلومات, pp);

  افعل {
     /* Read the response from the server */
    نتيجو = Curl_pp_readresp(معلومات, FIRSTSOCKET, pp, &pop3code, &nread);
    إذا(نتيجو)
      أرجع نتيجو;

    إذا(!pop3code)
      اكسر;

    /* We have now received a full POP3 server response */
    بدل(pop3c->state) {
    حالة POP3_SERVERGREET:
      نتيجو = pop3_state_servergreet_resp(معلومات, pop3code, pop3c->state);
      اكسر;

    حالة POP3_CAPA:
      نتيجو = pop3_state_capa_resp(معلومات, pop3code, pop3c->state);
      اكسر;

    حالة POP3_STARTTLS:
      نتيجو = pop3_state_starttls_resp(معلومات, conn, pop3code, pop3c->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      إذا(!نتيجو && (pop3c->state == POP3_UPGRADETLS))
        اذهب upgrade_tls;
      اكسر;

    حالة POP3_AUTH:
      نتيجو = pop3_state_auth_resp(معلومات, pop3code, pop3c->state);
      اكسر;

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DIGEST_AUTH
    حالة POP3_APOP:
      نتيجو = pop3_state_apop_resp(معلومات, pop3code, pop3c->state);
      اكسر;
#جيڪڏهن_پورو

    حالة POP3_USER:
      نتيجو = pop3_state_user_resp(معلومات, pop3code, pop3c->state);
      اكسر;

    حالة POP3_PASS:
      نتيجو = pop3_state_pass_resp(معلومات, pop3code, pop3c->state);
      اكسر;

    حالة POP3_COMMAND:
      نتيجو = pop3_state_command_resp(معلومات, pop3code, pop3c->state);
      اكسر;

    حالة POP3_QUIT:
      pop3_state(معلومات, POP3_STOP);
      اكسر;

    افتراضي:
      /* internal error */
      pop3_state(معلومات, POP3_STOP);
      اكسر;
    }
  } طالما(!نتيجو && pop3c->state != POP3_STOP && Curl_pp_moredata(pp));

  أرجع نتيجو;
}

/* Called repeatedly until done from multi.c */
ساكن CURLcode pop3_multi_statemach(بنية Curl_easy *معلومات, bool *پورو)
{
  CURLcode نتيجو = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;
  نتيجو = Curl_pp_statemach(معلومات, &pop3c->pp, FALSE, FALSE);
  *پورو = (pop3c->state == POP3_STOP);

  أرجع نتيجو;
}

ساكن CURLcode pop3_بلاڪ_statemach(بنية Curl_easy *معلومات,
                                     بنية connectdata *conn,
                                     bool disconnecting)
{
  CURLcode نتيجو = CURLE_OK;
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  طالما(pop3c->state != POP3_STOP && !نتيجو)
    نتيجو = Curl_pp_statemach(معلومات, &pop3c->pp, TRUE, disconnecting);

  أرجع نتيجو;
}

/* For the POP3 "protocol connect" and "doing" phases only */
ساكن CURLcode pop3_pollset(بنية Curl_easy *معلومات,
                             بنية easy_pollset *ps)
{
  بنية pop3_conn *pop3c =
    Curl_conn_meta_get(معلومات->conn, CURL_META_POP3_CONN);
  أرجع pop3c ? Curl_pp_pollset(معلومات, &pop3c->pp, ps) : CURLE_OK;
}

/* SASL parameters for the pop3 protocol */
ساكن ثابت بنية SASLproto saslpop3 = {
  "pop",                /* The service name */
  pop3_perform_auth,    /* Send authentication command */
  pop3_continue_auth,   /* Send authentication continuation */
  pop3_cancel_auth,     /* Send authentication cancellation */
  pop3_get_پيغام,     /* Get SASL response message */
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
ساكن CURLcode pop3_connect(بنية Curl_easy *معلومات, bool *پورو)
{
  CURLcode نتيجو = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  بنية pingpong *pp = pop3c ? &pop3c->pp : عدم;

  *پورو = FALSE; /* default to not done yet */
  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  PINGPONG_تياري(pp, pop3_statemachine, pop3_endofresp);

  /* Set the default preferred authentication type and mechanism */
  pop3c->preftype = POP3_TYPE_ANY;
  Curl_sasl_init(&pop3c->sasl, معلومات, &saslpop3);

  /* Initialize the pingpong layer */
  Curl_pp_init(pp, Curl_pgrs_now(معلومات));

  /* Parse the URL options */
  نتيجو = pop3_parse_url_options(conn);
  إذا(نتيجو)
    أرجع نتيجو;

  /* Start off waiting for the server greeting response */
  pop3_state(معلومات, POP3_SERVERGREET);

  نتيجو = pop3_multi_statemach(معلومات, پورو);

  أرجع نتيجو;
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
ساكن CURLcode pop3_پورو(بنية Curl_easy *معلومات, CURLcode حالت,
                          bool premature)
{
  CURLcode نتيجو = CURLE_OK;
  بنية POP3 *pop3 = Curl_meta_get(معلومات, CURL_META_POP3_EASY);

  (فراغ)premature;

  إذا(!pop3)
    أرجع CURLE_OK;

  إذا(حالت) {
    CURL_TRC_M(معلومات, "POP3 done with bad status");
    connclose(معلومات->conn);
    نتيجو = حالت;         /* use the already set error code */
  }

  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);

  /* Clear the transfer mode for the next request */
  pop3->transfer = PPTRANSFER_BODY;

  أرجع نتيجو;
}

/***********************************************************************
 *
 * pop3_perform()
 *
 * This is the actual DO function for POP3. Get a message/listing according to
 * the options previously setup.
 */
ساكن CURLcode pop3_perform(بنية Curl_easy *معلومات, bool *connected,
                             bool *dophase_پورو)
{
  /* This is POP3 and no proxy */
  CURLcode نتيجو = CURLE_OK;
  بنية POP3 *pop3 = Curl_meta_get(معلومات, CURL_META_POP3_EASY);

  إذا(!pop3)
    أرجع CURLE_FAILED_INIT;

  DEBUGF(infof(معلومات, "DO phase starts"));

  /* Start the first command in the DO phase, may alter data->req.no_body */
  نتيجو = pop3_perform_command(معلومات);
  إذا(نتيجو)
    أرجع نتيجو;

  إذا(معلومات->req.no_body)
    /* Requested no body means no transfer */
    pop3->transfer = PPTRANSFER_INFO;

  *dophase_پورو = FALSE; /* not done yet */

  /* Run the state-machine */
  نتيجو = pop3_multi_statemach(معلومات, dophase_پورو);
  *connected = Curl_conn_is_connected(معلومات->conn, FIRSTSOCKET);

  إذا(*dophase_پورو)
    DEBUGF(infof(معلومات, "DO phase is complete"));

  أرجع نتيجو;
}

/* Call this when the DO phase has completed */
ساكن CURLcode pop3_dophase_پورو(بنية Curl_easy *معلومات, bool connected)
{
  (فراغ)معلومات;
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
ساكن CURLcode pop3_regular_transfer(بنية Curl_easy *معلومات,
                                      bool *dophase_پورو)
{
  CURLcode نتيجو = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  معلومات->req.سائيز = -1;

  /* Set the progress data */
  Curl_pgrsReset(معلومات);

  /* Carry out the perform */
  نتيجو = pop3_perform(معلومات, &connected, dophase_پورو);

  /* Perform post DO phase operations if necessary */
  إذا(!نتيجو && *dophase_پورو)
    نتيجو = pop3_dophase_پورو(معلومات, connected);

  أرجع نتيجو;
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
ساكن CURLcode pop3_do(بنية Curl_easy *معلومات, bool *پورو)
{
  CURLcode نتيجو = CURLE_OK;
  *پورو = FALSE; /* default to false */

  /* Parse the URL path */
  نتيجو = pop3_parse_url_رستو(معلومات);
  إذا(نتيجو)
    أرجع نتيجو;

  /* Parse the custom request */
  نتيجو = pop3_parse_custom_request(معلومات);
  إذا(نتيجو)
    أرجع نتيجو;

  نتيجو = pop3_regular_transfer(معلومات, پورو);

  أرجع نتيجو;
}

/***********************************************************************
 *
 * pop3_disconnect()
 *
 * Disconnect from an POP3 server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ساكن CURLcode pop3_disconnect(بنية Curl_easy *معلومات,
                                بنية connectdata *conn, bool dead_connection)
{
  بنية pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (فراغ)معلومات;

  إذا(!pop3c)
    أرجع CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  إذا(!dead_connection && conn->بٽ.protoconnstart &&
     !Curl_pp_needs_flush(معلومات, &pop3c->pp) &&
     !pop3_perform_quit(معلومات, conn))
    (فراغ)pop3_بلاڪ_statemach(معلومات, conn, TRUE); /* ignore errors on QUIT */

  /* Disconnect from the server */
  Curl_pp_disconnect(&pop3c->pp);

  /* Cleanup our connection based variables */
  curlx_safefree(pop3c->apoptimestamp);

  أرجع CURLE_OK;
}

/* Called from multi.c while DOing */
ساكن CURLcode pop3_doing(بنية Curl_easy *معلومات, bool *dophase_پورو)
{
  CURLcode نتيجو = pop3_multi_statemach(معلومات, dophase_پورو);

  إذا(نتيجو)
    DEBUGF(infof(معلومات, "DO phase failed"));
  وإلا إذا(*dophase_پورو) {
    نتيجو = pop3_dophase_پورو(معلومات, FALSE /* not connected */);

    DEBUGF(infof(معلومات, "DO phase is complete"));
  }

  أرجع نتيجو;
}

ساكن فراغ pop3_easy_dtor(ثابت فراغ *ڪنجي, size_t klen, فراغ *داخلا_2)
{
  بنية POP3 *pop3 = داخلا_2;
  (فراغ)ڪنجي;
  (فراغ)klen;
  DEBUGASSERT(pop3);
  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);
  curlx_free(pop3);
}

ساكن فراغ pop3_conn_dtor(ثابت فراغ *ڪنجي, size_t klen, فراغ *داخلا_2)
{
  بنية pop3_conn *pop3c = داخلا_2;
  (فراغ)ڪنجي;
  (فراغ)klen;
  DEBUGASSERT(pop3c);
  Curl_pp_disconnect(&pop3c->pp);
  curlx_safefree(pop3c->apoptimestamp);
  curlx_free(pop3c);
}

ساكن CURLcode pop3_تياري_connection(بنية Curl_easy *معلومات,
                                      بنية connectdata *conn)
{
  بنية pop3_conn *pop3c;
  بنية POP3 *pop3 = curlx_calloc(1, حجم(*pop3));
  إذا(!pop3 ||
     Curl_meta_سيٽ(معلومات, CURL_META_POP3_EASY, pop3, pop3_easy_dtor))
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  pop3c = curlx_calloc(1, حجم(*pop3c));
  إذا(!pop3c ||
     Curl_conn_meta_سيٽ(conn, CURL_META_POP3_CONN, pop3c, pop3_conn_dtor))
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  أرجع CURLE_OK;
}

/*
 * POP3 protocol.
 */
ثابت بنية Curl_protocol Curl_protocol_pop3 = {
  pop3_تياري_connection,            /* setup_connection */
  pop3_do,                          /* do_it */
  pop3_پورو,                        /* done */
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

#جيڪڏهن_پورو /* CURL_DISABLE_POP3 */
