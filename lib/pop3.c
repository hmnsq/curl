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

#ifndef CURL_DISABLE_POP3

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef __VMS
#شامل <in.h>
#شامل <inet.h>
#endif

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
ساختار POP3 {
  curl_pp_transfer transfer;
  نویسه *id;               /* Message ID */
  نویسه *custom;           /* Custom Request */
};

/*
 * POP3 connection state
 */
تعریفنوع شمارش {
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

ساختار pop3_conn {
  ساختار pingpong pp;
  pop3state state;        /* Always use pop3.c:state() to change state! */
  size_t eob;             /* Number of bytes of the EOB (End Of Body) that
                             have been received so far */
  size_t strip;           /* Number of bytes from the start to ignore as
                             non-body */
  ساختار SASL sasl;       /* SASL-related storage */
  نویسه *apoptimestamp;    /* APOP timestamp from the server greeting */
  unsigned نویسه authtypes; /* Accepted authentication types */
  unsigned نویسه preftype;  /* Preferred authentication type */
  BIT(ssldone);           /* Is connect() over SSL done? */
  BIT(tls_supported);     /* StartTLS capability supported by server */
};

ساختار pop3_cmd {
  ثابت نویسه *name;
  unsigned short nlen;
  BIT(multiline); /* response is multi-line with last '.' line */
  BIT(multiline_with_args); /* is multi-line when command has args */
};

ایستا ثابت ساختار pop3_cmd pop3cmds[] = {
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
ایستا CURLcode pop3_parse_url_options(ساختار connectdata *conn)
{
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  ثابت نویسه *ptr = conn->options;

  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  وقتی(!result && ptr && *ptr) {
    ثابت نویسه *کلید = ptr;
    ثابت نویسه *مقدار;

    وقتی(*ptr && *ptr != '=')
      ptr++;

    مقدار = ptr + 1;

    وقتی(*ptr && *ptr != ';')
      ptr++;

    اگر(curl_strnequal(کلید, "AUTH=", 5)) {
      result = Curl_sasl_parse_url_auth_option(&pop3c->sasl,
                                               مقدار, ptr - مقدار);

      اگر(result && curl_strnequal(مقدار, "+APOP", ptr - مقدار)) {
        pop3c->preftype = POP3_TYPE_APOP;
        pop3c->sasl.prefmech = SASL_AUTH_NONE;
        result = CURLE_OK;
      }
    }
    وگرنه
      result = CURLE_URL_MALFORMAT;

    اگر(*ptr == ';')
      ptr++;
  }

  اگر(pop3c->preftype != POP3_TYPE_APOP)
    انتخاب(pop3c->sasl.prefmech) {
    حالت SASL_AUTH_NONE:
      pop3c->preftype = POP3_TYPE_NONE;
      بشکن;
    حالت SASL_AUTH_DEFAULT:
      pop3c->preftype = POP3_TYPE_ANY;
      بشکن;
    پیشفرض:
      pop3c->preftype = POP3_TYPE_SASL;
      بشکن;
    }

  بازگشت result;
}

/***********************************************************************
 *
 * pop3_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
ایستا CURLcode pop3_parse_url_path(ساختار Curl_easy *data)
{
  /* The POP3 struct is already initialized in pop3_connect() */
  ساختار POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  ثابت نویسه *path = &data->state.up.path[1]; /* skip leading path */

  اگر(!pop3)
    بازگشت CURLE_FAILED_INIT;
  /* URL decode the path for the message ID */
  بازگشت Curl_urldecode(path, 0, &pop3->id, پوچ, REJECT_CTRL);
}

/***********************************************************************
 *
 * pop3_parse_custom_request()
 *
 * Parse the custom request.
 */
ایستا CURLcode pop3_parse_custom_request(ساختار Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ساختار POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  ثابت نویسه *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  اگر(!pop3)
    بازگشت CURLE_FAILED_INIT;
  /* URL decode the custom request */
  اگر(custom)
    result = Curl_urldecode(custom, 0, &pop3->custom, پوچ, REJECT_CTRL);

  بازگشت result;
}

/* Return iff a command is defined as "multi-line" (RFC 1939),
 * has a response terminated by a last line with a '.'.
 */
ایستا bool pop3_is_multiline(ثابت نویسه *cmdline)
{
  size_t i;
  برای(i = 0; i < CURL_ARRAYSIZE(pop3cmds); ++i) {
    اگر(curl_strnequal(pop3cmds[i].name, cmdline, pop3cmds[i].nlen)) {
      اگر(!cmdline[pop3cmds[i].nlen])
        بازگشت (bool)pop3cmds[i].multiline;
      وگرنه اگر(cmdline[pop3cmds[i].nlen] == ' ')
        بازگشت (bool)pop3cmds[i].multiline_with_args;
    }
  }
  /* Unknown command, assume multi-line for backward compatibility with
   * earlier curl versions that only could do multi-line responses. */
  بازگشت TRUE;
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
ایستا bool pop3_endofresp(ساختار Curl_easy *data, ساختار connectdata *conn,
                           ثابت نویسه *سطر, size_t len, صحیح *resp)
{
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (تهی)data;
  DEBUGASSERT(pop3c);
  اگر(!pop3c) /* internal error */
    بازگشت TRUE;

  /* Do we have an error response? */
  اگر(len >= 4 && !memcmp("-ERR", سطر, 4)) {
    *resp = '-';

    بازگشت TRUE;
  }

  /* Are we processing CAPA command responses? */
  اگر(pop3c->state == POP3_CAPA) {
    /* Do we have the terminating line? Per RFC 2449 this is a line
       containing only a single dot */
    اگر((len == 3 && سطر[0] == '.' && سطر[1] == '\r') ||
       (len == 2 && سطر[0] == '.' && سطر[1] == '\n'))
      /* Treat the response as a success */
      *resp = '+';
    وگرنه
      /* Treat the response as an untagged continuation */
      *resp = '*';

    بازگشت TRUE;
  }

  /* Do we have a success response? */
  اگر(len >= 3 && !memcmp("+OK", سطر, 3)) {
    *resp = '+';

    بازگشت TRUE;
  }

  /* Do we have a continuation response? */
  اگر(len >= 1 && سطر[0] == '+') {
    *resp = '*';

    بازگشت TRUE;
  }

  بازگشت FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * pop3_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ایستا CURLcode pop3_get_message(ساختار Curl_easy *data, ساختار bufref *out)
{
  ساختار pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  نویسه *message;
  size_t len;

  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;
  message = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;
  اگر(len > 2) {
    /* Find the start of the message */
    len -= 2;
    برای(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    وقتی(len--)
      اگر(!ISBLANK(message[len]) && !ISNEWLINE(message[len]))
        بشکن;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, پوچ);
  }
  وگرنه
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, پوچ);

  بازگشت CURLE_OK;
}

/***********************************************************************
 *
 * pop3_state()
 *
 * This is the ONLY way to change POP3 state!
 */
ایستا تهی pop3_state(ساختار Curl_easy *data, pop3state newstate)
{
  ساختار pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  اگر(pop3c) {
#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
    /* for debug purposes */
    ایستا ثابت نویسه * ثابت names[] = {
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

    اگر(pop3c->state != newstate)
      infof(data, "POP3 %p state change from %s to %s",
            (تهی *)pop3c, names[pop3c->state], names[newstate]);
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
ایستا CURLcode pop3_perform_capa(ساختار Curl_easy *data,
                                  ساختار connectdata *conn)
{
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;

  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  pop3c->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  pop3c->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  pop3c->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPA command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "CAPA");

  اگر(!result)
    pop3_state(data, POP3_CAPA);

  بازگشت result;
}

/***********************************************************************
 *
 * pop3_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
ایستا CURLcode pop3_perform_starttls(ساختار Curl_easy *data,
                                      ساختار connectdata *conn)
{
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;

  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  /* Send the STLS command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "STLS");
  اگر(!result)
    pop3_state(data, POP3_STARTTLS);

  بازگشت result;
}

/***********************************************************************
 *
 * pop3_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ایستا CURLcode pop3_perform_upgrade_tls(ساختار Curl_easy *data,
                                         ساختار connectdata *conn)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;
  bool ssldone = FALSE;

  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  اگر(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    اگر(result)
      برو out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_pop3s;
  }

  DEBUGASSERT(!pop3c->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "pop3_perform_upgrade_tls, connect -> %d, %d",
               (صحیح)result, ssldone));
  اگر(!result && ssldone) {
    pop3c->ssldone = ssldone;
    /* perform CAPA now, changes pop3c->state out of POP3_UPGRADETLS */
    result = pop3_perform_capa(data, conn);
  }
out:
  بازگشت result;
#وگرنه
  (تهی)data;
  (تهی)conn;
  بازگشت CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * pop3_perform_user()
 *
 * Sends a clear text USER command to authenticate with.
 */
ایستا CURLcode pop3_perform_user(ساختار Curl_easy *data,
                                  ساختار connectdata *conn)
{
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;

  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  اگر(!conn->creds) {
    pop3_state(data, POP3_STOP);

    بازگشت result;
  }

  /* Send the USER command */
  result = Curl_pp_sendf(data, &pop3c->pp, "USER %s",
                         Curl_creds_user(conn->creds));
  اگر(!result)
    pop3_state(data, POP3_USER);

  بازگشت result;
}

#ifndef CURL_DISABLE_DIGEST_AUTH
/***********************************************************************
 *
 * pop3_perform_apop()
 *
 * Sends an APOP command to authenticate with.
 */
ایستا CURLcode pop3_perform_apop(ساختار Curl_easy *data,
                                  ساختار connectdata *conn)
{
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  size_t i;
  ساختار MD5_context *ctxt;
  unsigned نویسه digest[MD5_DIGEST_LEN];
  نویسه secret[(2 * MD5_DIGEST_LEN) + 1];

  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  اگر(!data->state.creds) {
    pop3_state(data, POP3_STOP);

    بازگشت result;
  }

  /* Create the digest */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  اگر(!ctxt)
    بازگشت CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)pop3c->apoptimestamp,
                  curlx_uztoui(strlen(pop3c->apoptimestamp)));

  Curl_MD5_update(ctxt, (ثابت unsigned نویسه *)Curl_creds_passwd(conn->creds),
                  curlx_uztoui(strlen(Curl_creds_passwd(conn->creds))));

  /* Finalise the digest */
  Curl_MD5_final(ctxt, digest);

  /* Convert the calculated 16 octet digest into a 32-byte hex string */
  برای(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&secret[2 * i], 3, "%02x", digest[i]);

  result = Curl_pp_sendf(data, &pop3c->pp, "APOP %s %s",
                         Curl_creds_user(conn->creds), secret);

  اگر(!result)
    pop3_state(data, POP3_APOP);

  بازگشت result;
}
#endif

/***********************************************************************
 *
 * pop3_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
ایستا CURLcode pop3_perform_auth(ساختار Curl_easy *data,
                                  ثابت نویسه *mech,
                                  ثابت ساختار bufref *initresp)
{
  ساختار pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  ثابت نویسه *ir = Curl_bufref_ptr(initresp);

  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  اگر(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    result = Curl_pp_sendf(data, &pop3c->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  وگرنه {
    /* Send the AUTH command */
    result = Curl_pp_sendf(data, &pop3c->pp, "AUTH %s", mech);
  }

  بازگشت result;
}

/***********************************************************************
 *
 * pop3_continue_auth()
 *
 * Sends SASL continuation data.
 */
ایستا CURLcode pop3_continue_auth(ساختار Curl_easy *data,
                                   ثابت نویسه *mech,
                                   ثابت ساختار bufref *resp)
{
  ساختار pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);

  (تهی)mech;
  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  بازگشت Curl_pp_sendf(data, &pop3c->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * pop3_cancel_auth()
 *
 * Sends SASL cancellation.
 */
ایستا CURLcode pop3_cancel_auth(ساختار Curl_easy *data, ثابت نویسه *mech)
{
  ساختار pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);

  (تهی)mech;
  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  بازگشت Curl_pp_sendf(data, &pop3c->pp, "*");
}

/***********************************************************************
 *
 * pop3_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to APOP and clear text should a
 * common mechanism not be available between the client and server.
 */
ایستا CURLcode pop3_perform_authentication(ساختار Curl_easy *data,
                                            ساختار connectdata *conn)
{
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  saslprogress progress = SASL_IDLE;

  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  /* Check we have enough data to authenticate with and end the
     connect phase if we do not */
  اگر(!Curl_sasl_can_authenticate(&pop3c->sasl, data)) {
    pop3_state(data, POP3_STOP);
    بازگشت result;
  }

  اگر(pop3c->authtypes & pop3c->preftype & POP3_TYPE_SASL) {
    /* Calculate the SASL login details */
    result = Curl_sasl_start(&pop3c->sasl, data, FALSE, &progress);

    اگر(!result && progress == SASL_INPROGRESS)
      pop3_state(data, POP3_AUTH);
  }

  اگر(!result && progress == SASL_IDLE) {
#ifndef CURL_DISABLE_DIGEST_AUTH
    اگر(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
      /* Perform APOP authentication */
      result = pop3_perform_apop(data, conn);
    وگرنه
#endif
    اگر(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
      /* Perform clear text authentication */
      result = pop3_perform_user(data, conn);
    وگرنه
      result = Curl_sasl_is_blocked(&pop3c->sasl, data);
  }

  بازگشت result;
}

/***********************************************************************
 *
 * pop3_perform_command()
 *
 * Sends a POP3 based command.
 */
ایستا CURLcode pop3_perform_command(ساختار Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;
  ساختار POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ثابت نویسه *command = پوچ;

  اگر(!pop3 || !pop3c)
    بازگشت CURLE_FAILED_INIT;

  /* Calculate the default command */
  اگر(pop3->id[0] == '\0' || data->set.list_only) {
    command = "LIST";

    اگر(pop3->id[0] != '\0')
      /* Message specific LIST so skip the BODY transfer */
      pop3->transfer = PPTRANSFER_INFO;
  }
  وگرنه
    command = "RETR";

  اگر(pop3->custom && pop3->custom[0] != '\0')
    command = pop3->custom;

  /* Send the command */
  اگر(pop3->id[0] != '\0')
    result = Curl_pp_sendf(data, &pop3c->pp, "%s %s", command, pop3->id);
  وگرنه
    result = Curl_pp_sendf(data, &pop3c->pp, "%s", command);

  اگر(!result) {
    pop3_state(data, POP3_COMMAND);
    data->req.no_body = !pop3_is_multiline(command);
  }

  بازگشت result;
}

/***********************************************************************
 *
 * pop3_perform_quit()
 *
 * Performs the quit action prior to sclose() be called.
 */
ایستا CURLcode pop3_perform_quit(ساختار Curl_easy *data,
                                  ساختار connectdata *conn)
{
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;

  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  /* Send the QUIT command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "QUIT");
  اگر(!result)
    pop3_state(data, POP3_QUIT);

  بازگشت result;
}

/* For the initial server greeting */
ایستا CURLcode pop3_state_servergreet_resp(ساختار Curl_easy *data,
                                            صحیح pop3code,
                                            pop3state instate)
{
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ثابت نویسه *سطر;
  size_t len;

  (تهی)instate;
  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  سطر = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  اگر(pop3code != '+') {
    failf(data, "Got unexpected pop3-server response");
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  وگرنه اگر(len > 3) {
    /* Does the server support APOP authentication? */
    ثابت نویسه *lt;
    ثابت نویسه *gt = پوچ;

    /* Look for the APOP timestamp */
    lt = memchr(سطر, '<', len);
    اگر(lt)
      /* search the remainder for '>' */
      gt = memchr(lt, '>', len - (lt - سطر));
    اگر(gt) {
      /* the length of the timestamp, including the brackets */
      size_t timestamplen = gt - lt + 1;
      ثابت نویسه *at = memchr(lt, '@', timestamplen);
      /* If the timestamp does not contain '@' it is not (as required by
         RFC-1939) conformant to the RFC-822 message id syntax, and we
         therefore do not use APOP authentication. */
      اگر(at) {
        /* dupe the timestamp */
        pop3c->apoptimestamp = curlx_memdup0(lt, timestamplen);
        اگر(!pop3c->apoptimestamp)
          بازگشت CURLE_OUT_OF_MEMORY;
        /* Store the APOP capability */
        pop3c->authtypes |= POP3_TYPE_APOP;
      }
    }

    اگر(!result)
      result = pop3_perform_capa(data, conn);
  }

  بازگشت result;
}

/* For CAPA responses */
ایستا CURLcode pop3_state_capa_resp(ساختار Curl_easy *data, صحیح pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ثابت نویسه *سطر;
  size_t len;

  (تهی)instate;
  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  سطر = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  /* Do we have an untagged continuation response? */
  اگر(pop3code == '*') {
    /* Does the server support the STLS capability? */
    اگر(len >= 4 && curl_strnequal(سطر, "STLS", 4))
      pop3c->tls_supported = TRUE;

    /* Does the server support clear text authentication? */
    وگرنه اگر(len >= 4 && curl_strnequal(سطر, "USER", 4))
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    /* Does the server support SASL based authentication? */
    وگرنه اگر(len >= 5 && curl_strnequal(سطر, "SASL ", 5)) {
      pop3c->authtypes |= POP3_TYPE_SASL;

      /* Advance past the SASL keyword */
      سطر += 5;
      len -= 5;

      /* Loop through the data line */
      برای(;;) {
        size_t llen;
        size_t wordlen = 0;
        unsigned short mechbit;

        وقتی(len && (ISBLANK(*سطر) || ISNEWLINE(*سطر))) {
          سطر++;
          len--;
        }

        اگر(!len)
          بشکن;

        /* Extract the word */
        وقتی(wordlen < len && !ISBLANK(سطر[wordlen]) &&
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
  وگرنه {
    /* Clear text is supported when CAPA is not recognised */
    اگر(pop3code != '+')
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    اگر(!data->set.use_ssl || Curl_conn_is_ssl(conn, FIRSTSOCKET))
      result = pop3_perform_authentication(data, conn);
    وگرنه اگر(pop3code == '+' && pop3c->tls_supported)
      /* Switch to TLS connection now */
      result = pop3_perform_starttls(data, conn);
    وگرنه اگر(data->set.use_ssl <= CURLUSESSL_TRY)
      /* Fallback and carry on with authentication */
      result = pop3_perform_authentication(data, conn);
    وگرنه {
      failf(data, "STLS not supported.");
      result = CURLE_USE_SSL_FAILED;
    }
  }

  بازگشت result;
}

/* For STARTTLS responses */
ایستا CURLcode pop3_state_starttls_resp(ساختار Curl_easy *data,
                                         ساختار connectdata *conn,
                                         صحیح pop3code,
                                         pop3state instate)
{
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  (تهی)instate;

  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  /* Pipelining in response is forbidden. */
  اگر(pop3c->pp.overflow)
    بازگشت CURLE_WEIRD_SERVER_REPLY;

  اگر(pop3code != '+') {
    اگر(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied");
      result = CURLE_USE_SSL_FAILED;
    }
    وگرنه
      result = pop3_perform_authentication(data, conn);
  }
  وگرنه
    pop3_state(data, POP3_UPGRADETLS);

  بازگشت result;
}

/* For SASL authentication responses */
ایستا CURLcode pop3_state_auth_resp(ساختار Curl_easy *data,
                                     صحیح pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  saslprogress progress;

  (تهی)instate;
  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  result = Curl_sasl_continue(&pop3c->sasl, data, pop3code, &progress);
  اگر(!result)
    انتخاب(progress) {
    حالت SASL_DONE:
      pop3_state(data, POP3_STOP);  /* Authenticated */
      بشکن;
    حالت SASL_IDLE:            /* No mechanism left after cancellation */
#ifndef CURL_DISABLE_DIGEST_AUTH
      اگر(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
        /* Perform APOP authentication */
        result = pop3_perform_apop(data, conn);
      وگرنه
#endif
      اگر(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
        /* Perform clear text authentication */
        result = pop3_perform_user(data, conn);
      وگرنه {
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
      }
      بشکن;
    پیشفرض:
      بشکن;
    }

  بازگشت result;
}

#ifndef CURL_DISABLE_DIGEST_AUTH
/* For APOP responses */
ایستا CURLcode pop3_state_apop_resp(ساختار Curl_easy *data, صحیح pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  (تهی)instate;

  اگر(pop3code != '+') {
    failf(data, "Authentication failed: %d", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  وگرنه
    /* End of connect phase */
    pop3_state(data, POP3_STOP);

  بازگشت result;
}
#endif

/* For USER responses */
ایستا CURLcode pop3_state_user_resp(ساختار Curl_easy *data, صحیح pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (تهی)instate;

  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  اگر(pop3code != '+') {
    failf(data, "Access denied. %c", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  وگرنه
    /* Send the PASS command */
    result = Curl_pp_sendf(data, &pop3c->pp, "PASS %s",
                           Curl_creds_passwd(conn->creds));
  اگر(!result)
    pop3_state(data, POP3_PASS);

  بازگشت result;
}

/* For PASS responses */
ایستا CURLcode pop3_state_pass_resp(ساختار Curl_easy *data, صحیح pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  (تهی)instate;

  اگر(pop3code != '+') {
    failf(data, "Access denied. %c", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  وگرنه
    /* End of connect phase */
    pop3_state(data, POP3_STOP);

  بازگشت result;
}

/***********************************************************************
 *
 * pop3_write()
 *
 * This function scans the body after the end-of-body and writes everything
 * until the end is found.
 */
ایستا CURLcode pop3_write(ساختار Curl_easy *data, ثابت نویسه *str,
                           size_t nread, bool is_eos)
{
  /* This code could be made into a special function in the handler struct */
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  bool strip_dot = FALSE;
  size_t last = 0;
  size_t i;
  (تهی)is_eos;

  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  /* Search through the buffer looking for the end-of-body marker which is
     5 bytes (0d 0a 2e 0d 0a). Note that a line starting with a dot matches
     the eob so the server will have prefixed it with an extra dot which we
     need to strip out. Additionally the marker could of course be spread out
     over 5 different data chunks. */
  برای(i = 0; i < nread; i++) {
    size_t prev = pop3c->eob;

    انتخاب(str[i]) {
    حالت 0x0d:
      اگر(pop3c->eob == 0) {
        pop3c->eob++;

        اگر(i) {
          /* Write out the body part that did not match */
          result = Curl_client_write(data, CLIENTWRITE_BODY, &str[last],
                                     i - last);

          اگر(result)
            بازگشت result;

          last = i;
        }
      }
      وگرنه اگر(pop3c->eob == 3)
        pop3c->eob++;
      وگرنه
        /* If the character match was not at position 0 or 3 then restart the
           pattern matching */
        pop3c->eob = 1;
      بشکن;

    حالت 0x0a:
      اگر(pop3c->eob == 1 || pop3c->eob == 4)
        pop3c->eob++;
      وگرنه
        /* If the character match was not at position 1 or 4 then start the
           search again */
        pop3c->eob = 0;
      بشکن;

    حالت 0x2e:
      اگر(pop3c->eob == 2)
        pop3c->eob++;
      وگرنه اگر(pop3c->eob == 3) {
        /* We have an extra dot after the CRLF which we need to strip off */
        strip_dot = TRUE;
        pop3c->eob = 0;
      }
      وگرنه
        /* If the character match was not at position 2 then start the search
           again */
        pop3c->eob = 0;
      بشکن;

    پیشفرض:
      pop3c->eob = 0;
      بشکن;
    }

    /* Did we have a partial match which has subsequently failed? */
    اگر(prev && prev >= pop3c->eob) {
      /* Strip can only be non-zero for the first mismatch after CRLF and
         then both prev and strip are equal and nothing will be output below */
      وقتی(prev && pop3c->strip) {
        prev--;
        pop3c->strip--;
      }

      اگر(prev) {
        /* If the partial match was the CRLF and dot then only write the CRLF
           as the server would have inserted the dot */
        اگر(strip_dot && prev - 1 > 0) {
          result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB,
                                     prev - 1);
        }
        وگرنه اگر(!strip_dot) {
          result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB,
                                     prev);
        }
        وگرنه {
          result = CURLE_OK;
        }

        اگر(result)
          بازگشت result;

        last = i;
        strip_dot = FALSE;
      }
    }
  }

  اگر(pop3c->eob == POP3_EOB_LEN) {
    /* We have a full match so the transfer is done, however we must transfer
    the CRLF at the start of the EOB as this is considered to be part of the
    message as per RFC-1939, sect. 3 */
    result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB, 2);

    CURL_REQ_CLEAR_RECV(data);
    pop3c->eob = 0;

    بازگشت result;
  }

  اگر(pop3c->eob)
    /* While EOB is matching nothing should be output */
    بازگشت CURLE_OK;

  اگر(nread - last) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, &str[last],
                               nread - last);
  }

  بازگشت result;
}

/* For command responses */
ایستا CURLcode pop3_state_command_resp(ساختار Curl_easy *data,
                                        صحیح pop3code,
                                        pop3state instate)
{
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;
  ساختار POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ساختار pingpong *pp;

  (تهی)instate;
  اگر(!pop3 || !pop3c)
    بازگشت CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  اگر(pop3code != '+') {
    pop3_state(data, POP3_STOP);
    بازگشت CURLE_WEIRD_SERVER_REPLY;
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
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);

    اگر(pp->overflow) {
      /* The recv buffer contains data that is actually body content so send
         it as such. Note that there may even be additional "headers" after
         the body */

      /* keep only the overflow */
      curlx_dyn_tail(&pp->recvbuf, pp->overflow);
      pp->nfinal = 0; /* done */

      اگر(!data->req.no_body) {
        result = pop3_write(data, curlx_dyn_ptr(&pp->recvbuf),
                            curlx_dyn_len(&pp->recvbuf), FALSE);
        اگر(result)
          بازگشت result;
      }

      /* reset the buffer */
      curlx_dyn_reset(&pp->recvbuf);
      pp->overflow = 0;
    }
  }
  وگرنه
    pp->overflow = 0;

  /* End of DO phase */
  pop3_state(data, POP3_STOP);

  بازگشت result;
}

ایستا CURLcode pop3_statemachine(ساختار Curl_easy *data,
                                  ساختار connectdata *conn)
{
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  صحیح pop3code;
  ساختار pingpong *pp;
  size_t nread = 0;
  (تهی)data;

  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not POP3 */
upgrade_tls:
  اگر(pop3c->state == POP3_UPGRADETLS) {
    result = pop3_perform_upgrade_tls(data, conn);
    اگر(result || (pop3c->state == POP3_UPGRADETLS))
      بازگشت result;
  }

  /* Flush any data that needs to be sent */
  اگر(pp->sendleft)
    بازگشت Curl_pp_flushsend(data, pp);

  انجامبده {
     /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, pp, &pop3code, &nread);
    اگر(result)
      بازگشت result;

    اگر(!pop3code)
      بشکن;

    /* We have now received a full POP3 server response */
    انتخاب(pop3c->state) {
    حالت POP3_SERVERGREET:
      result = pop3_state_servergreet_resp(data, pop3code, pop3c->state);
      بشکن;

    حالت POP3_CAPA:
      result = pop3_state_capa_resp(data, pop3code, pop3c->state);
      بشکن;

    حالت POP3_STARTTLS:
      result = pop3_state_starttls_resp(data, conn, pop3code, pop3c->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      اگر(!result && (pop3c->state == POP3_UPGRADETLS))
        برو upgrade_tls;
      بشکن;

    حالت POP3_AUTH:
      result = pop3_state_auth_resp(data, pop3code, pop3c->state);
      بشکن;

#ifndef CURL_DISABLE_DIGEST_AUTH
    حالت POP3_APOP:
      result = pop3_state_apop_resp(data, pop3code, pop3c->state);
      بشکن;
#endif

    حالت POP3_USER:
      result = pop3_state_user_resp(data, pop3code, pop3c->state);
      بشکن;

    حالت POP3_PASS:
      result = pop3_state_pass_resp(data, pop3code, pop3c->state);
      بشکن;

    حالت POP3_COMMAND:
      result = pop3_state_command_resp(data, pop3code, pop3c->state);
      بشکن;

    حالت POP3_QUIT:
      pop3_state(data, POP3_STOP);
      بشکن;

    پیشفرض:
      /* internal error */
      pop3_state(data, POP3_STOP);
      بشکن;
    }
  } وقتی(!result && pop3c->state != POP3_STOP && Curl_pp_moredata(pp));

  بازگشت result;
}

/* Called repeatedly until done from multi.c */
ایستا CURLcode pop3_multi_statemach(ساختار Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;
  result = Curl_pp_statemach(data, &pop3c->pp, FALSE, FALSE);
  *done = (pop3c->state == POP3_STOP);

  بازگشت result;
}

ایستا CURLcode pop3_block_statemach(ساختار Curl_easy *data,
                                     ساختار connectdata *conn,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  وقتی(pop3c->state != POP3_STOP && !result)
    result = Curl_pp_statemach(data, &pop3c->pp, TRUE, disconnecting);

  بازگشت result;
}

/* For the POP3 "protocol connect" and "doing" phases only */
ایستا CURLcode pop3_pollset(ساختار Curl_easy *data,
                             ساختار easy_pollset *ps)
{
  ساختار pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  بازگشت pop3c ? Curl_pp_pollset(data, &pop3c->pp, ps) : CURLE_OK;
}

/* SASL parameters for the pop3 protocol */
ایستا ثابت ساختار SASLproto saslpop3 = {
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
ایستا CURLcode pop3_connect(ساختار Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ساختار pingpong *pp = pop3c ? &pop3c->pp : پوچ;

  *done = FALSE; /* default to not done yet */
  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  PINGPONG_SETUP(pp, pop3_statemachine, pop3_endofresp);

  /* Set the default preferred authentication type and mechanism */
  pop3c->preftype = POP3_TYPE_ANY;
  Curl_sasl_init(&pop3c->sasl, data, &saslpop3);

  /* Initialize the pingpong layer */
  Curl_pp_init(pp, Curl_pgrs_now(data));

  /* Parse the URL options */
  result = pop3_parse_url_options(conn);
  اگر(result)
    بازگشت result;

  /* Start off waiting for the server greeting response */
  pop3_state(data, POP3_SERVERGREET);

  result = pop3_multi_statemach(data, done);

  بازگشت result;
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
ایستا CURLcode pop3_done(ساختار Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  ساختار POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);

  (تهی)premature;

  اگر(!pop3)
    بازگشت CURLE_OK;

  اگر(status) {
    CURL_TRC_M(data, "POP3 done with bad status");
    connclose(data->conn);
    result = status;         /* use the already set error code */
  }

  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);

  /* Clear the transfer mode for the next request */
  pop3->transfer = PPTRANSFER_BODY;

  بازگشت result;
}

/***********************************************************************
 *
 * pop3_perform()
 *
 * This is the actual DO function for POP3. Get a message/listing according to
 * the options previously setup.
 */
ایستا CURLcode pop3_perform(ساختار Curl_easy *data, bool *connected,
                             bool *dophase_done)
{
  /* This is POP3 and no proxy */
  CURLcode result = CURLE_OK;
  ساختار POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);

  اگر(!pop3)
    بازگشت CURLE_FAILED_INIT;

  DEBUGF(infof(data, "DO phase starts"));

  /* Start the first command in the DO phase, may alter data->req.no_body */
  result = pop3_perform_command(data);
  اگر(result)
    بازگشت result;

  اگر(data->req.no_body)
    /* Requested no body means no transfer */
    pop3->transfer = PPTRANSFER_INFO;

  *dophase_done = FALSE; /* not done yet */

  /* Run the state-machine */
  result = pop3_multi_statemach(data, dophase_done);
  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  اگر(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  بازگشت result;
}

/* Call this when the DO phase has completed */
ایستا CURLcode pop3_dophase_done(ساختار Curl_easy *data, bool connected)
{
  (تهی)data;
  (تهی)connected;

  بازگشت CURLE_OK;
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
ایستا CURLcode pop3_regular_transfer(ساختار Curl_easy *data,
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
  اگر(!result && *dophase_done)
    result = pop3_dophase_done(data, connected);

  بازگشت result;
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
ایستا CURLcode pop3_do(ساختار Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  *done = FALSE; /* default to false */

  /* Parse the URL path */
  result = pop3_parse_url_path(data);
  اگر(result)
    بازگشت result;

  /* Parse the custom request */
  result = pop3_parse_custom_request(data);
  اگر(result)
    بازگشت result;

  result = pop3_regular_transfer(data, done);

  بازگشت result;
}

/***********************************************************************
 *
 * pop3_disconnect()
 *
 * Disconnect from an POP3 server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ایستا CURLcode pop3_disconnect(ساختار Curl_easy *data,
                                ساختار connectdata *conn, bool dead_connection)
{
  ساختار pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (تهی)data;

  اگر(!pop3c)
    بازگشت CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  اگر(!dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &pop3c->pp) &&
     !pop3_perform_quit(data, conn))
    (تهی)pop3_block_statemach(data, conn, TRUE); /* ignore errors on QUIT */

  /* Disconnect from the server */
  Curl_pp_disconnect(&pop3c->pp);

  /* Cleanup our connection based variables */
  curlx_safefree(pop3c->apoptimestamp);

  بازگشت CURLE_OK;
}

/* Called from multi.c while DOing */
ایستا CURLcode pop3_doing(ساختار Curl_easy *data, bool *dophase_done)
{
  CURLcode result = pop3_multi_statemach(data, dophase_done);

  اگر(result)
    DEBUGF(infof(data, "DO phase failed"));
  وگرنه اگر(*dophase_done) {
    result = pop3_dophase_done(data, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  بازگشت result;
}

ایستا تهی pop3_easy_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار POP3 *pop3 = entry;
  (تهی)کلید;
  (تهی)klen;
  DEBUGASSERT(pop3);
  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);
  curlx_free(pop3);
}

ایستا تهی pop3_conn_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار pop3_conn *pop3c = entry;
  (تهی)کلید;
  (تهی)klen;
  DEBUGASSERT(pop3c);
  Curl_pp_disconnect(&pop3c->pp);
  curlx_safefree(pop3c->apoptimestamp);
  curlx_free(pop3c);
}

ایستا CURLcode pop3_setup_connection(ساختار Curl_easy *data,
                                      ساختار connectdata *conn)
{
  ساختار pop3_conn *pop3c;
  ساختار POP3 *pop3 = curlx_calloc(1, اندازه(*pop3));
  اگر(!pop3 ||
     Curl_meta_set(data, CURL_META_POP3_EASY, pop3, pop3_easy_dtor))
    بازگشت CURLE_OUT_OF_MEMORY;

  pop3c = curlx_calloc(1, اندازه(*pop3c));
  اگر(!pop3c ||
     Curl_conn_meta_set(conn, CURL_META_POP3_CONN, pop3c, pop3_conn_dtor))
    بازگشت CURLE_OUT_OF_MEMORY;

  بازگشت CURLE_OK;
}

/*
 * POP3 protocol.
 */
ثابت ساختار Curl_protocol Curl_protocol_pop3 = {
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
