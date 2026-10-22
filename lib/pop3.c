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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "pop3.h"

#यदि_अपरिभाषितम् CURL_DISABLE_POP3

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#यदि_अन्तः

#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "pingpong.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "curl_sasl.h"
#अन्तर्भाव "curl_md5.h"
#अन्तर्भाव "curlx/strdup.h"

/* Authentication type flags */
#परिभाषय POP3_TYPE_CLEARTEXT (1 << 0)
#परिभाषय POP3_TYPE_APOP      (1 << 1)
#परिभाषय POP3_TYPE_SASL      (1 << 2)

/* Authentication type values */
#परिभाषय POP3_TYPE_NONE 0
#परिभाषय POP3_TYPE_ANY  (POP3_TYPE_CLEARTEXT | POP3_TYPE_APOP | POP3_TYPE_SASL)

/* This is the 5-bytes End-Of-Body marker for POP3 */
#परिभाषय POP3_EOB     "\x0d\x0a\x2e\x0d\x0a"
#परिभाषय POP3_EOB_LEN 5

/* meta key for storing protocol meta at easy handle */
#परिभाषय CURL_META_POP3_EASY   "meta:proto:pop3:easy"
/* meta key for storing protocol meta at connection */
#परिभाषय CURL_META_POP3_CONN   "meta:proto:pop3:conn"

/*
 * POP3 easy handle state
 */
संरचना POP3 {
  curl_pp_transfer transfer;
  वर्ण *id;               /* Message ID */
  वर्ण *custom;           /* Custom Request */
};

/*
 * POP3 connection state
 */
प्रकारनिर्देश गणना {
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
  POP3_चरमः          /* never used */
} pop3state;

संरचना pop3_conn {
  संरचना pingpong pp;
  pop3state state;        /* Always use pop3.c:state() to change state! */
  size_t eob;             /* Number of bytes of the EOB (End Of Body) that
                             have been received so far */
  size_t strip;           /* Number of bytes from the start to ignore as
                             non-body */
  संरचना SASL sasl;       /* SASL-related storage */
  वर्ण *apoptimestamp;    /* APOP timestamp from the server greeting */
  अचिह्नित वर्ण authtypes; /* Accepted authentication types */
  अचिह्नित वर्ण preftype;  /* Preferred authentication type */
  BIT(ssldone);           /* Is connect() over SSL done? */
  BIT(tls_supported);     /* StartTLS capability supported by server */
};

संरचना pop3_cmd {
  ध्रुव वर्ण *name;
  अचिह्नित ह्रस्व nlen;
  BIT(multiline); /* response is multi-line with last '.' line */
  BIT(multiline_with_तर्काः); /* is multi-line when command has args */
};

स्थिर ध्रुव संरचना pop3_cmd pop3cmds[] = {
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
स्थिर CURLcode pop3_parse_url_options(संरचना connectdata *conn)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण *ptr = conn->options;

  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  यावत्(!परिणाम && ptr && *ptr) {
    ध्रुव वर्ण *कुञ्चिका = ptr;
    ध्रुव वर्ण *मानम्;

    यावत्(*ptr && *ptr != '=')
      ptr++;

    मानम् = ptr + 1;

    यावत्(*ptr && *ptr != ';')
      ptr++;

    यदि(curl_strnequal(कुञ्चिका, "AUTH=", 5)) {
      परिणाम = Curl_sasl_parse_url_auth_option(&pop3c->sasl,
                                               मानम्, ptr - मानम्);

      यदि(परिणाम && curl_strnequal(मानम्, "+APOP", ptr - मानम्)) {
        pop3c->preftype = POP3_TYPE_APOP;
        pop3c->sasl.prefmech = SASL_AUTH_NONE;
        परिणाम = CURLE_OK;
      }
    }
    अन्यथा
      परिणाम = CURLE_URL_MALFORMAT;

    यदि(*ptr == ';')
      ptr++;
  }

  यदि(pop3c->preftype != POP3_TYPE_APOP)
    विकल्प(pop3c->sasl.prefmech) {
    पक्ष SASL_AUTH_NONE:
      pop3c->preftype = POP3_TYPE_NONE;
      भङ्ग;
    पक्ष SASL_AUTH_DEFAULT:
      pop3c->preftype = POP3_TYPE_ANY;
      भङ्ग;
    स्वभाव:
      pop3c->preftype = POP3_TYPE_SASL;
      भङ्ग;
    }

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * pop3_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
स्थिर CURLcode pop3_parse_url_मार्गः(संरचना Curl_easy *दत्त)
{
  /* The POP3 struct is already initialized in pop3_connect() */
  संरचना POP3 *pop3 = Curl_meta_get(दत्त, CURL_META_POP3_EASY);
  ध्रुव वर्ण *मार्गः = &दत्त->state.ऊर्ध्वम्.मार्गः[1]; /* skip leading path */

  यदि(!pop3)
    प्रत्यागम CURLE_FAILED_INIT;
  /* URL decode the path for the message ID */
  प्रत्यागम Curl_urldecode(मार्गः, 0, &pop3->id, रिक्त, REJECT_CTRL);
}

/***********************************************************************
 *
 * pop3_parse_custom_request()
 *
 * Parse the custom request.
 */
स्थिर CURLcode pop3_parse_custom_request(संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना POP3 *pop3 = Curl_meta_get(दत्त, CURL_META_POP3_EASY);
  ध्रुव वर्ण *custom = CURL_EASY_STR(दत्त, STRING_CUSTOMREQUEST);

  यदि(!pop3)
    प्रत्यागम CURLE_FAILED_INIT;
  /* URL decode the custom request */
  यदि(custom)
    परिणाम = Curl_urldecode(custom, 0, &pop3->custom, रिक्त, REJECT_CTRL);

  प्रत्यागम परिणाम;
}

/* Return iff a command is defined as "multi-line" (RFC 1939),
 * has a response terminated by a last line with a '.'.
 */
स्थिर bool pop3_is_multiline(ध्रुव वर्ण *cmdline)
{
  size_t i;
  प्रति(i = 0; i < CURL_ARRAYSIZE(pop3cmds); ++i) {
    यदि(curl_strnequal(pop3cmds[i].name, cmdline, pop3cmds[i].nlen)) {
      यदि(!cmdline[pop3cmds[i].nlen])
        प्रत्यागम (bool)pop3cmds[i].multiline;
      अन्यथा यदि(cmdline[pop3cmds[i].nlen] == ' ')
        प्रत्यागम (bool)pop3cmds[i].multiline_with_तर्काः;
    }
  }
  /* Unknown command, assume multi-line for backward compatibility with
   * earlier curl versions that only could do multi-line responses. */
  प्रत्यागम TRUE;
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
स्थिर bool pop3_endofresp(संरचना Curl_easy *दत्त, संरचना connectdata *conn,
                           ध्रुव वर्ण *पङ्क्तिः, size_t len, पूर्णाङ्क *resp)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (शून्य)दत्त;
  DEBUGASSERT(pop3c);
  यदि(!pop3c) /* internal error */
    प्रत्यागम TRUE;

  /* Do we have an error response? */
  यदि(len >= 4 && !memcmp("-ERR", पङ्क्तिः, 4)) {
    *resp = '-';

    प्रत्यागम TRUE;
  }

  /* Are we processing CAPA command responses? */
  यदि(pop3c->state == POP3_CAPA) {
    /* Do we have the terminating line? Per RFC 2449 this is a line
       containing only a single dot */
    यदि((len == 3 && पङ्क्तिः[0] == '.' && पङ्क्तिः[1] == '\r') ||
       (len == 2 && पङ्क्तिः[0] == '.' && पङ्क्तिः[1] == '\n'))
      /* Treat the response as a success */
      *resp = '+';
    अन्यथा
      /* Treat the response as an untagged continuation */
      *resp = '*';

    प्रत्यागम TRUE;
  }

  /* Do we have a success response? */
  यदि(len >= 3 && !memcmp("+OK", पङ्क्तिः, 3)) {
    *resp = '+';

    प्रत्यागम TRUE;
  }

  /* Do we have a continuation response? */
  यदि(len >= 1 && पङ्क्तिः[0] == '+') {
    *resp = '*';

    प्रत्यागम TRUE;
  }

  प्रत्यागम FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * pop3_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
स्थिर CURLcode pop3_get_सन्देशः(संरचना Curl_easy *दत्त, संरचना bufref *बहिर्गमनम्)
{
  संरचना pop3_conn *pop3c =
    Curl_conn_meta_get(दत्त->conn, CURL_META_POP3_CONN);
  वर्ण *सन्देशः_2;
  size_t len;

  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;
  सन्देशः_2 = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;
  यदि(len > 2) {
    /* Find the start of the message */
    len -= 2;
    प्रति(सन्देशः_2 += 2; ISBLANK(*सन्देशः_2); सन्देशः_2++, len--)
      ;

    /* Find the end of the message */
    यावत्(len--)
      यदि(!ISBLANK(सन्देशः_2[len]) && !ISNEWLINE(सन्देशः_2[len]))
        भङ्ग;

    /* Terminate the message */
    सन्देशः_2[++len] = '\0';
    Curl_bufref_समुच्चयः(बहिर्गमनम्, सन्देशः_2, len, रिक्त);
  }
  अन्यथा
    /* junk input => zero length output */
    Curl_bufref_समुच्चयः(बहिर्गमनम्, "", 0, रिक्त);

  प्रत्यागम CURLE_OK;
}

/***********************************************************************
 *
 * pop3_state()
 *
 * This is the ONLY way to change POP3 state!
 */
स्थिर शून्य pop3_state(संरचना Curl_easy *दत्त, pop3state newstate)
{
  संरचना pop3_conn *pop3c =
    Curl_conn_meta_get(दत्त->conn, CURL_META_POP3_CONN);
  यदि(pop3c) {
#यदि defined(DEBUGBUILD) && defined(CURLVERBOSE)
    /* for debug purposes */
    स्थिर ध्रुव वर्ण * ध्रुव नामानि[] = {
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
      /*चरमः*/
    };

    यदि(pop3c->state != newstate)
      infof(दत्त, "POP3 %p state change from %s to %s",
            (शून्य *)pop3c, नामानि[pop3c->state], नामानि[newstate]);
#यदि_अन्तः

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
स्थिर CURLcode pop3_perform_capa(संरचना Curl_easy *दत्त,
                                  संरचना connectdata *conn)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode परिणाम = CURLE_OK;

  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  pop3c->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  pop3c->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  pop3c->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPA command */
  परिणाम = Curl_pp_sendf(दत्त, &pop3c->pp, "%s", "CAPA");

  यदि(!परिणाम)
    pop3_state(दत्त, POP3_CAPA);

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * pop3_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
स्थिर CURLcode pop3_perform_starttls(संरचना Curl_easy *दत्त,
                                      संरचना connectdata *conn)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode परिणाम;

  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  /* Send the STLS command */
  परिणाम = Curl_pp_sendf(दत्त, &pop3c->pp, "%s", "STLS");
  यदि(!परिणाम)
    pop3_state(दत्त, POP3_STARTTLS);

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * pop3_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
स्थिर CURLcode pop3_perform_upgrade_tls(संरचना Curl_easy *दत्त,
                                         संरचना connectdata *conn)
{
#यदि_परिभाषितम् USE_SSL
  /* Start the SSL connection */
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode परिणाम;
  bool ssldone = FALSE;

  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    परिणाम = Curl_ssl_cfilter_सङ्कलय(
      दत्त, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_pop3s;
  }

  DEBUGASSERT(!pop3c->ssldone);
  परिणाम = Curl_conn_connect(दत्त, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(दत्त, "pop3_perform_upgrade_tls, connect -> %d, %d",
               (पूर्णाङ्क)परिणाम, ssldone));
  यदि(!परिणाम && ssldone) {
    pop3c->ssldone = ssldone;
    /* perform CAPA now, changes pop3c->state out of POP3_UPGRADETLS */
    परिणाम = pop3_perform_capa(दत्त, conn);
  }
बहिर्गमनम्:
  प्रत्यागम परिणाम;
#अन्यथा
  (शून्य)दत्त;
  (शून्य)conn;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
}

/***********************************************************************
 *
 * pop3_perform_user()
 *
 * Sends a clear text USER command to authenticate with.
 */
स्थिर CURLcode pop3_perform_user(संरचना Curl_easy *दत्त,
                                  संरचना connectdata *conn)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode परिणाम = CURLE_OK;

  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  यदि(!conn->creds) {
    pop3_state(दत्त, POP3_STOP);

    प्रत्यागम परिणाम;
  }

  /* Send the USER command */
  परिणाम = Curl_pp_sendf(दत्त, &pop3c->pp, "USER %s",
                         Curl_creds_user(conn->creds));
  यदि(!परिणाम)
    pop3_state(दत्त, POP3_USER);

  प्रत्यागम परिणाम;
}

#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH
/***********************************************************************
 *
 * pop3_perform_apop()
 *
 * Sends an APOP command to authenticate with.
 */
स्थिर CURLcode pop3_perform_apop(संरचना Curl_easy *दत्त,
                                  संरचना connectdata *conn)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode परिणाम = CURLE_OK;
  size_t i;
  संरचना MD5_context *ctxt;
  अचिह्नित वर्ण digest[MD5_DIGEST_LEN];
  वर्ण secret[(2 * MD5_DIGEST_LEN) + 1];

  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  यदि(!दत्त->state.creds) {
    pop3_state(दत्त, POP3_STOP);

    प्रत्यागम परिणाम;
  }

  /* Create the digest */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  यदि(!ctxt)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)pop3c->apoptimestamp,
                  curlx_uztoui(वाक्यदैर्घ्यम्(pop3c->apoptimestamp)));

  Curl_MD5_update(ctxt, (ध्रुव अचिह्नित वर्ण *)Curl_creds_passwd(conn->creds),
                  curlx_uztoui(वाक्यदैर्घ्यम्(Curl_creds_passwd(conn->creds))));

  /* Finalise the digest */
  Curl_MD5_final(ctxt, digest);

  /* Convert the calculated 16 octet digest into a 32-byte hex string */
  प्रति(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&secret[2 * i], 3, "%02x", digest[i]);

  परिणाम = Curl_pp_sendf(दत्त, &pop3c->pp, "APOP %s %s",
                         Curl_creds_user(conn->creds), secret);

  यदि(!परिणाम)
    pop3_state(दत्त, POP3_APOP);

  प्रत्यागम परिणाम;
}
#यदि_अन्तः

/***********************************************************************
 *
 * pop3_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
स्थिर CURLcode pop3_perform_auth(संरचना Curl_easy *दत्त,
                                  ध्रुव वर्ण *mech,
                                  ध्रुव संरचना bufref *initresp)
{
  संरचना pop3_conn *pop3c =
    Curl_conn_meta_get(दत्त->conn, CURL_META_POP3_CONN);
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण *ir = Curl_bufref_ptr(initresp);

  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    परिणाम = Curl_pp_sendf(दत्त, &pop3c->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  अन्यथा {
    /* Send the AUTH command */
    परिणाम = Curl_pp_sendf(दत्त, &pop3c->pp, "AUTH %s", mech);
  }

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * pop3_continue_auth()
 *
 * Sends SASL continuation data.
 */
स्थिर CURLcode pop3_continue_auth(संरचना Curl_easy *दत्त,
                                   ध्रुव वर्ण *mech,
                                   ध्रुव संरचना bufref *resp)
{
  संरचना pop3_conn *pop3c =
    Curl_conn_meta_get(दत्त->conn, CURL_META_POP3_CONN);

  (शून्य)mech;
  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  प्रत्यागम Curl_pp_sendf(दत्त, &pop3c->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * pop3_cancel_auth()
 *
 * Sends SASL cancellation.
 */
स्थिर CURLcode pop3_cancel_auth(संरचना Curl_easy *दत्त, ध्रुव वर्ण *mech)
{
  संरचना pop3_conn *pop3c =
    Curl_conn_meta_get(दत्त->conn, CURL_META_POP3_CONN);

  (शून्य)mech;
  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  प्रत्यागम Curl_pp_sendf(दत्त, &pop3c->pp, "*");
}

/***********************************************************************
 *
 * pop3_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to APOP and clear text should a
 * common mechanism not be available between the client and server.
 */
स्थिर CURLcode pop3_perform_authentication(संरचना Curl_easy *दत्त,
                                            संरचना connectdata *conn)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode परिणाम = CURLE_OK;
  saslprogress प्रगतिः = SASL_IDLE;

  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  /* Check we have enough data to authenticate with and end the
     connect phase if we do not */
  यदि(!Curl_sasl_can_authenticate(&pop3c->sasl, दत्त)) {
    pop3_state(दत्त, POP3_STOP);
    प्रत्यागम परिणाम;
  }

  यदि(pop3c->authtypes & pop3c->preftype & POP3_TYPE_SASL) {
    /* Calculate the SASL login details */
    परिणाम = Curl_sasl_start(&pop3c->sasl, दत्त, FALSE, &प्रगतिः);

    यदि(!परिणाम && प्रगतिः == SASL_INPROGRESS)
      pop3_state(दत्त, POP3_AUTH);
  }

  यदि(!परिणाम && प्रगतिः == SASL_IDLE) {
#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH
    यदि(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
      /* Perform APOP authentication */
      परिणाम = pop3_perform_apop(दत्त, conn);
    अन्यथा
#यदि_अन्तः
    यदि(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
      /* Perform clear text authentication */
      परिणाम = pop3_perform_user(दत्त, conn);
    अन्यथा
      परिणाम = Curl_sasl_is_blocked(&pop3c->sasl, दत्त);
  }

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * pop3_perform_command()
 *
 * Sends a POP3 based command.
 */
स्थिर CURLcode pop3_perform_command(संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;
  संरचना POP3 *pop3 = Curl_meta_get(दत्त, CURL_META_POP3_EASY);
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ध्रुव वर्ण *command = रिक्त;

  यदि(!pop3 || !pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  /* Calculate the default command */
  यदि(pop3->id[0] == '\0' || दत्त->समुच्चयः.list_only) {
    command = "LIST";

    यदि(pop3->id[0] != '\0')
      /* Message specific LIST so skip the BODY transfer */
      pop3->transfer = PPTRANSFER_INFO;
  }
  अन्यथा
    command = "RETR";

  यदि(pop3->custom && pop3->custom[0] != '\0')
    command = pop3->custom;

  /* Send the command */
  यदि(pop3->id[0] != '\0')
    परिणाम = Curl_pp_sendf(दत्त, &pop3c->pp, "%s %s", command, pop3->id);
  अन्यथा
    परिणाम = Curl_pp_sendf(दत्त, &pop3c->pp, "%s", command);

  यदि(!परिणाम) {
    pop3_state(दत्त, POP3_COMMAND);
    दत्त->req.no_body = !pop3_is_multiline(command);
  }

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * pop3_perform_quit()
 *
 * Performs the quit action prior to sclose() be called.
 */
स्थिर CURLcode pop3_perform_quit(संरचना Curl_easy *दत्त,
                                  संरचना connectdata *conn)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode परिणाम;

  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  /* Send the QUIT command */
  परिणाम = Curl_pp_sendf(दत्त, &pop3c->pp, "%s", "QUIT");
  यदि(!परिणाम)
    pop3_state(दत्त, POP3_QUIT);

  प्रत्यागम परिणाम;
}

/* For the initial server greeting */
स्थिर CURLcode pop3_state_servergreet_resp(संरचना Curl_easy *दत्त,
                                            पूर्णाङ्क pop3code,
                                            pop3state instate)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ध्रुव वर्ण *पङ्क्तिः;
  size_t len;

  (शून्य)instate;
  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  पङ्क्तिः = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  यदि(pop3code != '+') {
    failf(दत्त, "Got unexpected pop3-server response");
    परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  }
  अन्यथा यदि(len > 3) {
    /* Does the server support APOP authentication? */
    ध्रुव वर्ण *lt;
    ध्रुव वर्ण *gt = रिक्त;

    /* Look for the APOP timestamp */
    lt = memchr(पङ्क्तिः, '<', len);
    यदि(lt)
      /* search the remainder for '>' */
      gt = memchr(lt, '>', len - (lt - पङ्क्तिः));
    यदि(gt) {
      /* the length of the timestamp, including the brackets */
      size_t timestamplen = gt - lt + 1;
      ध्रुव वर्ण *at = memchr(lt, '@', timestamplen);
      /* If the timestamp does not contain '@' it is not (as required by
         RFC-1939) conformant to the RFC-822 message id syntax, and we
         therefore do not use APOP authentication. */
      यदि(at) {
        /* dupe the timestamp */
        pop3c->apoptimestamp = curlx_memdup0(lt, timestamplen);
        यदि(!pop3c->apoptimestamp)
          प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        /* Store the APOP capability */
        pop3c->authtypes |= POP3_TYPE_APOP;
      }
    }

    यदि(!परिणाम)
      परिणाम = pop3_perform_capa(दत्त, conn);
  }

  प्रत्यागम परिणाम;
}

/* For CAPA responses */
स्थिर CURLcode pop3_state_capa_resp(संरचना Curl_easy *दत्त, पूर्णाङ्क pop3code,
                                     pop3state instate)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  ध्रुव वर्ण *पङ्क्तिः;
  size_t len;

  (शून्य)instate;
  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  पङ्क्तिः = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  /* Do we have an untagged continuation response? */
  यदि(pop3code == '*') {
    /* Does the server support the STLS capability? */
    यदि(len >= 4 && curl_strnequal(पङ्क्तिः, "STLS", 4))
      pop3c->tls_supported = TRUE;

    /* Does the server support clear text authentication? */
    अन्यथा यदि(len >= 4 && curl_strnequal(पङ्क्तिः, "USER", 4))
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    /* Does the server support SASL based authentication? */
    अन्यथा यदि(len >= 5 && curl_strnequal(पङ्क्तिः, "SASL ", 5)) {
      pop3c->authtypes |= POP3_TYPE_SASL;

      /* Advance past the SASL keyword */
      पङ्क्तिः += 5;
      len -= 5;

      /* Loop through the data line */
      प्रति(;;) {
        size_t llen;
        size_t wordlen = 0;
        अचिह्नित ह्रस्व mechbit;

        यावत्(len && (ISBLANK(*पङ्क्तिः) || ISNEWLINE(*पङ्क्तिः))) {
          पङ्क्तिः++;
          len--;
        }

        यदि(!len)
          भङ्ग;

        /* Extract the word */
        यावत्(wordlen < len && !ISBLANK(पङ्क्तिः[wordlen]) &&
              !ISNEWLINE(पङ्क्तिः[wordlen]))
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(पङ्क्तिः, wordlen, &llen);
        यदि(mechbit && llen == wordlen)
          pop3c->sasl.authmechs |= mechbit;

        पङ्क्तिः += wordlen;
        len -= wordlen;
      }
    }
  }
  अन्यथा {
    /* Clear text is supported when CAPA is not recognised */
    यदि(pop3code != '+')
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    यदि(!दत्त->समुच्चयः.use_ssl || Curl_conn_is_ssl(conn, FIRSTSOCKET))
      परिणाम = pop3_perform_authentication(दत्त, conn);
    अन्यथा यदि(pop3code == '+' && pop3c->tls_supported)
      /* Switch to TLS connection now */
      परिणाम = pop3_perform_starttls(दत्त, conn);
    अन्यथा यदि(दत्त->समुच्चयः.use_ssl <= CURLUSESSL_TRY)
      /* Fallback and carry on with authentication */
      परिणाम = pop3_perform_authentication(दत्त, conn);
    अन्यथा {
      failf(दत्त, "STLS not supported.");
      परिणाम = CURLE_USE_SSL_FAILED;
    }
  }

  प्रत्यागम परिणाम;
}

/* For STARTTLS responses */
स्थिर CURLcode pop3_state_starttls_resp(संरचना Curl_easy *दत्त,
                                         संरचना connectdata *conn,
                                         पूर्णाङ्क pop3code,
                                         pop3state instate)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode परिणाम = CURLE_OK;
  (शून्य)instate;

  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  /* Pipelining in response is forbidden. */
  यदि(pop3c->pp.अतिप्रवाहः)
    प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;

  यदि(pop3code != '+') {
    यदि(दत्त->समुच्चयः.use_ssl != CURLUSESSL_TRY) {
      failf(दत्त, "STARTTLS denied");
      परिणाम = CURLE_USE_SSL_FAILED;
    }
    अन्यथा
      परिणाम = pop3_perform_authentication(दत्त, conn);
  }
  अन्यथा
    pop3_state(दत्त, POP3_UPGRADETLS);

  प्रत्यागम परिणाम;
}

/* For SASL authentication responses */
स्थिर CURLcode pop3_state_auth_resp(संरचना Curl_easy *दत्त,
                                     पूर्णाङ्क pop3code,
                                     pop3state instate)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  saslprogress प्रगतिः;

  (शून्य)instate;
  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  परिणाम = Curl_sasl_continue(&pop3c->sasl, दत्त, pop3code, &प्रगतिः);
  यदि(!परिणाम)
    विकल्प(प्रगतिः) {
    पक्ष SASL_समाप्तम्:
      pop3_state(दत्त, POP3_STOP);  /* Authenticated */
      भङ्ग;
    पक्ष SASL_IDLE:            /* No mechanism left after cancellation */
#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH
      यदि(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
        /* Perform APOP authentication */
        परिणाम = pop3_perform_apop(दत्त, conn);
      अन्यथा
#यदि_अन्तः
      यदि(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
        /* Perform clear text authentication */
        परिणाम = pop3_perform_user(दत्त, conn);
      अन्यथा {
        failf(दत्त, "Authentication cancelled");
        परिणाम = CURLE_LOGIN_DENIED;
      }
      भङ्ग;
    स्वभाव:
      भङ्ग;
    }

  प्रत्यागम परिणाम;
}

#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH
/* For APOP responses */
स्थिर CURLcode pop3_state_apop_resp(संरचना Curl_easy *दत्त, पूर्णाङ्क pop3code,
                                     pop3state instate)
{
  CURLcode परिणाम = CURLE_OK;
  (शून्य)instate;

  यदि(pop3code != '+') {
    failf(दत्त, "Authentication failed: %d", pop3code);
    परिणाम = CURLE_LOGIN_DENIED;
  }
  अन्यथा
    /* End of connect phase */
    pop3_state(दत्त, POP3_STOP);

  प्रत्यागम परिणाम;
}
#यदि_अन्तः

/* For USER responses */
स्थिर CURLcode pop3_state_user_resp(संरचना Curl_easy *दत्त, पूर्णाङ्क pop3code,
                                     pop3state instate)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (शून्य)instate;

  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(pop3code != '+') {
    failf(दत्त, "Access denied. %c", pop3code);
    परिणाम = CURLE_LOGIN_DENIED;
  }
  अन्यथा
    /* Send the PASS command */
    परिणाम = Curl_pp_sendf(दत्त, &pop3c->pp, "PASS %s",
                           Curl_creds_passwd(conn->creds));
  यदि(!परिणाम)
    pop3_state(दत्त, POP3_PASS);

  प्रत्यागम परिणाम;
}

/* For PASS responses */
स्थिर CURLcode pop3_state_pass_resp(संरचना Curl_easy *दत्त, पूर्णाङ्क pop3code,
                                     pop3state instate)
{
  CURLcode परिणाम = CURLE_OK;
  (शून्य)instate;

  यदि(pop3code != '+') {
    failf(दत्त, "Access denied. %c", pop3code);
    परिणाम = CURLE_LOGIN_DENIED;
  }
  अन्यथा
    /* End of connect phase */
    pop3_state(दत्त, POP3_STOP);

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * pop3_write()
 *
 * This function scans the body after the end-of-body and writes everything
 * until the end is found.
 */
स्थिर CURLcode pop3_write(संरचना Curl_easy *दत्त, ध्रुव वर्ण *str,
                           size_t nread, bool is_eos)
{
  /* This code could be made into a special function in the handler struct */
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  bool strip_dot = FALSE;
  size_t चरमः = 0;
  size_t i;
  (शून्य)is_eos;

  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  /* Search through the buffer looking for the end-of-body marker which is
     5 bytes (0d 0a 2e 0d 0a). Note that a line starting with a dot matches
     the eob so the server will have prefixed it with an extra dot which we
     need to strip out. Additionally the marker could of course be spread out
     over 5 different data chunks. */
  प्रति(i = 0; i < nread; i++) {
    size_t पूर्व = pop3c->eob;

    विकल्प(str[i]) {
    पक्ष 0x0d:
      यदि(pop3c->eob == 0) {
        pop3c->eob++;

        यदि(i) {
          /* Write out the body part that did not match */
          परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, &str[चरमः],
                                     i - चरमः);

          यदि(परिणाम)
            प्रत्यागम परिणाम;

          चरमः = i;
        }
      }
      अन्यथा यदि(pop3c->eob == 3)
        pop3c->eob++;
      अन्यथा
        /* If the character match was not at position 0 or 3 then restart the
           pattern matching */
        pop3c->eob = 1;
      भङ्ग;

    पक्ष 0x0a:
      यदि(pop3c->eob == 1 || pop3c->eob == 4)
        pop3c->eob++;
      अन्यथा
        /* If the character match was not at position 1 or 4 then start the
           search again */
        pop3c->eob = 0;
      भङ्ग;

    पक्ष 0x2e:
      यदि(pop3c->eob == 2)
        pop3c->eob++;
      अन्यथा यदि(pop3c->eob == 3) {
        /* We have an extra dot after the CRLF which we need to strip off */
        strip_dot = TRUE;
        pop3c->eob = 0;
      }
      अन्यथा
        /* If the character match was not at position 2 then start the search
           again */
        pop3c->eob = 0;
      भङ्ग;

    स्वभाव:
      pop3c->eob = 0;
      भङ्ग;
    }

    /* Did we have a partial match which has subsequently failed? */
    यदि(पूर्व && पूर्व >= pop3c->eob) {
      /* Strip can only be non-zero for the first mismatch after CRLF and
         then both prev and strip are equal and nothing will be output below */
      यावत्(पूर्व && pop3c->strip) {
        पूर्व--;
        pop3c->strip--;
      }

      यदि(पूर्व) {
        /* If the partial match was the CRLF and dot then only write the CRLF
           as the server would have inserted the dot */
        यदि(strip_dot && पूर्व - 1 > 0) {
          परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, POP3_EOB,
                                     पूर्व - 1);
        }
        अन्यथा यदि(!strip_dot) {
          परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, POP3_EOB,
                                     पूर्व);
        }
        अन्यथा {
          परिणाम = CURLE_OK;
        }

        यदि(परिणाम)
          प्रत्यागम परिणाम;

        चरमः = i;
        strip_dot = FALSE;
      }
    }
  }

  यदि(pop3c->eob == POP3_EOB_LEN) {
    /* We have a full match so the transfer is done, however we must transfer
    the CRLF at the start of the EOB as this is considered to be part of the
    message as per RFC-1939, sect. 3 */
    परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, POP3_EOB, 2);

    CURL_REQ_CLEAR_RECV(दत्त);
    pop3c->eob = 0;

    प्रत्यागम परिणाम;
  }

  यदि(pop3c->eob)
    /* While EOB is matching nothing should be output */
    प्रत्यागम CURLE_OK;

  यदि(nread - चरमः) {
    परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, &str[चरमः],
                               nread - चरमः);
  }

  प्रत्यागम परिणाम;
}

/* For command responses */
स्थिर CURLcode pop3_state_command_resp(संरचना Curl_easy *दत्त,
                                        पूर्णाङ्क pop3code,
                                        pop3state instate)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;
  संरचना POP3 *pop3 = Curl_meta_get(दत्त, CURL_META_POP3_EASY);
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  संरचना pingpong *pp;

  (शून्य)instate;
  यदि(!pop3 || !pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  यदि(pop3code != '+') {
    pop3_state(दत्त, POP3_STOP);
    प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  }

  /* This 'OK' line ends with a CR LF pair which is the two first bytes of the
     EOB string so count this is two matching bytes. This is necessary to make
     the code detect the EOB if the only data than comes now is %2e CR LF like
     when there is no body to return. */
  pop3c->eob = 2;

  /* Since this initial CR LF pair is not part of the actual body, we set
     the strip counter here so that these bytes will not be delivered. */
  pop3c->strip = 2;

  यदि(pop3->transfer == PPTRANSFER_BODY) {
    /* POP3 download */
    Curl_xfer_सज्जता_recv(दत्त, FIRSTSOCKET, -1);

    यदि(pp->अतिप्रवाहः) {
      /* The recv buffer contains data that is actually body content so send
         it as such. Note that there may even be additional "headers" after
         the body */

      /* keep only the overflow */
      curlx_dyn_पुच्छ(&pp->recvbuf, pp->अतिप्रवाहः);
      pp->nfinal = 0; /* done */

      यदि(!दत्त->req.no_body) {
        परिणाम = pop3_write(दत्त, curlx_dyn_ptr(&pp->recvbuf),
                            curlx_dyn_len(&pp->recvbuf), FALSE);
        यदि(परिणाम)
          प्रत्यागम परिणाम;
      }

      /* reset the buffer */
      curlx_dyn_reset(&pp->recvbuf);
      pp->अतिप्रवाहः = 0;
    }
  }
  अन्यथा
    pp->अतिप्रवाहः = 0;

  /* End of DO phase */
  pop3_state(दत्त, POP3_STOP);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode pop3_statemachine(संरचना Curl_easy *दत्त,
                                  संरचना connectdata *conn)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क pop3code;
  संरचना pingpong *pp;
  size_t nread = 0;
  (शून्य)दत्त;

  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not POP3 */
upgrade_tls:
  यदि(pop3c->state == POP3_UPGRADETLS) {
    परिणाम = pop3_perform_upgrade_tls(दत्त, conn);
    यदि(परिणाम || (pop3c->state == POP3_UPGRADETLS))
      प्रत्यागम परिणाम;
  }

  /* Flush any data that needs to be sent */
  यदि(pp->sendleft)
    प्रत्यागम Curl_pp_flushsend(दत्त, pp);

  कुरु {
     /* Read the response from the server */
    परिणाम = Curl_pp_readresp(दत्त, FIRSTSOCKET, pp, &pop3code, &nread);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    यदि(!pop3code)
      भङ्ग;

    /* We have now received a full POP3 server response */
    विकल्प(pop3c->state) {
    पक्ष POP3_SERVERGREET:
      परिणाम = pop3_state_servergreet_resp(दत्त, pop3code, pop3c->state);
      भङ्ग;

    पक्ष POP3_CAPA:
      परिणाम = pop3_state_capa_resp(दत्त, pop3code, pop3c->state);
      भङ्ग;

    पक्ष POP3_STARTTLS:
      परिणाम = pop3_state_starttls_resp(दत्त, conn, pop3code, pop3c->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      यदि(!परिणाम && (pop3c->state == POP3_UPGRADETLS))
        गच्छ upgrade_tls;
      भङ्ग;

    पक्ष POP3_AUTH:
      परिणाम = pop3_state_auth_resp(दत्त, pop3code, pop3c->state);
      भङ्ग;

#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH
    पक्ष POP3_APOP:
      परिणाम = pop3_state_apop_resp(दत्त, pop3code, pop3c->state);
      भङ्ग;
#यदि_अन्तः

    पक्ष POP3_USER:
      परिणाम = pop3_state_user_resp(दत्त, pop3code, pop3c->state);
      भङ्ग;

    पक्ष POP3_PASS:
      परिणाम = pop3_state_pass_resp(दत्त, pop3code, pop3c->state);
      भङ्ग;

    पक्ष POP3_COMMAND:
      परिणाम = pop3_state_command_resp(दत्त, pop3code, pop3c->state);
      भङ्ग;

    पक्ष POP3_QUIT:
      pop3_state(दत्त, POP3_STOP);
      भङ्ग;

    स्वभाव:
      /* internal error */
      pop3_state(दत्त, POP3_STOP);
      भङ्ग;
    }
  } यावत्(!परिणाम && pop3c->state != POP3_STOP && Curl_pp_moredata(pp));

  प्रत्यागम परिणाम;
}

/* Called repeatedly until done from multi.c */
स्थिर CURLcode pop3_multi_statemach(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;
  परिणाम = Curl_pp_statemach(दत्त, &pop3c->pp, FALSE, FALSE);
  *समाप्तम् = (pop3c->state == POP3_STOP);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode pop3_खण्डम्_statemach(संरचना Curl_easy *दत्त,
                                     संरचना connectdata *conn,
                                     bool disconnecting)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  यावत्(pop3c->state != POP3_STOP && !परिणाम)
    परिणाम = Curl_pp_statemach(दत्त, &pop3c->pp, TRUE, disconnecting);

  प्रत्यागम परिणाम;
}

/* For the POP3 "protocol connect" and "doing" phases only */
स्थिर CURLcode pop3_pollset(संरचना Curl_easy *दत्त,
                             संरचना easy_pollset *ps)
{
  संरचना pop3_conn *pop3c =
    Curl_conn_meta_get(दत्त->conn, CURL_META_POP3_CONN);
  प्रत्यागम pop3c ? Curl_pp_pollset(दत्त, &pop3c->pp, ps) : CURLE_OK;
}

/* SASL parameters for the pop3 protocol */
स्थिर ध्रुव संरचना SASLproto saslpop3 = {
  "pop",                /* The service name */
  pop3_perform_auth,    /* Send authentication command */
  pop3_continue_auth,   /* Send authentication continuation */
  pop3_cancel_auth,     /* Send authentication cancellation */
  pop3_get_सन्देशः,     /* Get SASL response message */
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
स्थिर CURLcode pop3_connect(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  संरचना pingpong *pp = pop3c ? &pop3c->pp : रिक्त;

  *समाप्तम् = FALSE; /* default to not done yet */
  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  PINGPONG_सज्जता(pp, pop3_statemachine, pop3_endofresp);

  /* Set the default preferred authentication type and mechanism */
  pop3c->preftype = POP3_TYPE_ANY;
  Curl_sasl_init(&pop3c->sasl, दत्त, &saslpop3);

  /* Initialize the pingpong layer */
  Curl_pp_init(pp, Curl_pgrs_now(दत्त));

  /* Parse the URL options */
  परिणाम = pop3_parse_url_options(conn);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* Start off waiting for the server greeting response */
  pop3_state(दत्त, POP3_SERVERGREET);

  परिणाम = pop3_multi_statemach(दत्त, समाप्तम्);

  प्रत्यागम परिणाम;
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
स्थिर CURLcode pop3_समाप्तम्(संरचना Curl_easy *दत्त, CURLcode स्थितिः,
                          bool premature)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना POP3 *pop3 = Curl_meta_get(दत्त, CURL_META_POP3_EASY);

  (शून्य)premature;

  यदि(!pop3)
    प्रत्यागम CURLE_OK;

  यदि(स्थितिः) {
    CURL_TRC_M(दत्त, "POP3 done with bad status");
    connclose(दत्त->conn);
    परिणाम = स्थितिः;         /* use the already set error code */
  }

  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);

  /* Clear the transfer mode for the next request */
  pop3->transfer = PPTRANSFER_BODY;

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * pop3_perform()
 *
 * This is the actual DO function for POP3. Get a message/listing according to
 * the options previously setup.
 */
स्थिर CURLcode pop3_perform(संरचना Curl_easy *दत्त, bool *connected,
                             bool *dophase_समाप्तम्)
{
  /* This is POP3 and no proxy */
  CURLcode परिणाम = CURLE_OK;
  संरचना POP3 *pop3 = Curl_meta_get(दत्त, CURL_META_POP3_EASY);

  यदि(!pop3)
    प्रत्यागम CURLE_FAILED_INIT;

  DEBUGF(infof(दत्त, "DO phase starts"));

  /* Start the first command in the DO phase, may alter data->req.no_body */
  परिणाम = pop3_perform_command(दत्त);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  यदि(दत्त->req.no_body)
    /* Requested no body means no transfer */
    pop3->transfer = PPTRANSFER_INFO;

  *dophase_समाप्तम् = FALSE; /* not done yet */

  /* Run the state-machine */
  परिणाम = pop3_multi_statemach(दत्त, dophase_समाप्तम्);
  *connected = Curl_conn_is_connected(दत्त->conn, FIRSTSOCKET);

  यदि(*dophase_समाप्तम्)
    DEBUGF(infof(दत्त, "DO phase is complete"));

  प्रत्यागम परिणाम;
}

/* Call this when the DO phase has completed */
स्थिर CURLcode pop3_dophase_समाप्तम्(संरचना Curl_easy *दत्त, bool connected)
{
  (शून्य)दत्त;
  (शून्य)connected;

  प्रत्यागम CURLE_OK;
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
स्थिर CURLcode pop3_regular_transfer(संरचना Curl_easy *दत्त,
                                      bool *dophase_समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  दत्त->req.आकार = -1;

  /* Set the progress data */
  Curl_pgrsReset(दत्त);

  /* Carry out the perform */
  परिणाम = pop3_perform(दत्त, &connected, dophase_समाप्तम्);

  /* Perform post DO phase operations if necessary */
  यदि(!परिणाम && *dophase_समाप्तम्)
    परिणाम = pop3_dophase_समाप्तम्(दत्त, connected);

  प्रत्यागम परिणाम;
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
स्थिर CURLcode pop3_do(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  *समाप्तम् = FALSE; /* default to false */

  /* Parse the URL path */
  परिणाम = pop3_parse_url_मार्गः(दत्त);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* Parse the custom request */
  परिणाम = pop3_parse_custom_request(दत्त);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  परिणाम = pop3_regular_transfer(दत्त, समाप्तम्);

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * pop3_disconnect()
 *
 * Disconnect from an POP3 server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
स्थिर CURLcode pop3_disconnect(संरचना Curl_easy *दत्त,
                                संरचना connectdata *conn, bool dead_connection)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (शून्य)दत्त;

  यदि(!pop3c)
    प्रत्यागम CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  यदि(!dead_connection && conn->द्वयङ्काः.protoconnstart &&
     !Curl_pp_needs_flush(दत्त, &pop3c->pp) &&
     !pop3_perform_quit(दत्त, conn))
    (शून्य)pop3_खण्डम्_statemach(दत्त, conn, TRUE); /* ignore errors on QUIT */

  /* Disconnect from the server */
  Curl_pp_disconnect(&pop3c->pp);

  /* Cleanup our connection based variables */
  curlx_safefree(pop3c->apoptimestamp);

  प्रत्यागम CURLE_OK;
}

/* Called from multi.c while DOing */
स्थिर CURLcode pop3_doing(संरचना Curl_easy *दत्त, bool *dophase_समाप्तम्)
{
  CURLcode परिणाम = pop3_multi_statemach(दत्त, dophase_समाप्तम्);

  यदि(परिणाम)
    DEBUGF(infof(दत्त, "DO phase failed"));
  अन्यथा यदि(*dophase_समाप्तम्) {
    परिणाम = pop3_dophase_समाप्तम्(दत्त, FALSE /* not connected */);

    DEBUGF(infof(दत्त, "DO phase is complete"));
  }

  प्रत्यागम परिणाम;
}

स्थिर शून्य pop3_easy_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना POP3 *pop3 = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  DEBUGASSERT(pop3);
  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);
  curlx_free(pop3);
}

स्थिर शून्य pop3_conn_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना pop3_conn *pop3c = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  DEBUGASSERT(pop3c);
  Curl_pp_disconnect(&pop3c->pp);
  curlx_safefree(pop3c->apoptimestamp);
  curlx_free(pop3c);
}

स्थिर CURLcode pop3_सज्जता_connection(संरचना Curl_easy *दत्त,
                                      संरचना connectdata *conn)
{
  संरचना pop3_conn *pop3c;
  संरचना POP3 *pop3 = curlx_calloc(1, परिमाण(*pop3));
  यदि(!pop3 ||
     Curl_meta_समुच्चयः(दत्त, CURL_META_POP3_EASY, pop3, pop3_easy_dtor))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  pop3c = curlx_calloc(1, परिमाण(*pop3c));
  यदि(!pop3c ||
     Curl_conn_meta_समुच्चयः(conn, CURL_META_POP3_CONN, pop3c, pop3_conn_dtor))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  प्रत्यागम CURLE_OK;
}

/*
 * POP3 protocol.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_pop3 = {
  pop3_सज्जता_connection,            /* setup_connection */
  pop3_do,                          /* do_it */
  pop3_समाप्तम्,                        /* done */
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

#यदि_अन्तः /* CURL_DISABLE_POP3 */
