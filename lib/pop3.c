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
#అన్తర్భావ "curl_setup.h"
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "pop3.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_POP3

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే __VMS
#అన్తర్భావ <in.h>
#అన్తర్భావ <inet.h>
#అయితే_ముగింపు

#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "pingpong.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "bufref.h"
#అన్తర్భావ "curl_sasl.h"
#అన్తర్భావ "curl_md5.h"
#అన్తర్భావ "curlx/strdup.h"

/* Authentication type flags */
#నిర్వచించు POP3_TYPE_CLEARTEXT (1 << 0)
#నిర్వచించు POP3_TYPE_APOP      (1 << 1)
#నిర్వచించు POP3_TYPE_SASL      (1 << 2)

/* Authentication type values */
#నిర్వచించు POP3_TYPE_NONE 0
#నిర్వచించు POP3_TYPE_ANY  (POP3_TYPE_CLEARTEXT | POP3_TYPE_APOP | POP3_TYPE_SASL)

/* This is the 5-bytes End-Of-Body marker for POP3 */
#నిర్వచించు POP3_EOB     "\x0d\x0a\x2e\x0d\x0a"
#నిర్వచించు POP3_EOB_LEN 5

/* meta key for storing protocol meta at easy handle */
#నిర్వచించు CURL_META_POP3_EASY   "meta:proto:pop3:easy"
/* meta key for storing protocol meta at connection */
#నిర్వచించు CURL_META_POP3_CONN   "meta:proto:pop3:conn"

/*
 * POP3 easy handle state
 */
నిర్మాణం POP3 {
  curl_pp_transfer transfer;
  అక్షరం *id;               /* Message ID */
  అక్షరం *custom;           /* Custom Request */
};

/*
 * POP3 connection state
 */
రకనిర్వచనం గణన {
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
  POP3_చివరి          /* never used */
} pop3state;

నిర్మాణం pop3_conn {
  నిర్మాణం pingpong pp;
  pop3state state;        /* Always use pop3.c:state() to change state! */
  size_t eob;             /* Number of bytes of the EOB (End Of Body) that
                             have been received so far */
  size_t strip;           /* Number of bytes from the start to ignore as
                             non-body */
  నిర్మాణం SASL sasl;       /* SASL-related storage */
  అక్షరం *apoptimestamp;    /* APOP timestamp from the server greeting */
  గుర్తులేని అక్షరం authtypes; /* Accepted authentication types */
  గుర్తులేని అక్షరం preftype;  /* Preferred authentication type */
  బిట్టు(ssldone);           /* Is connect() over SSL done? */
  బిట్టు(tls_supported);     /* StartTLS capability supported by server */
};

నిర్మాణం pop3_cmd {
  స్థిరరాశి అక్షరం *name;
  గుర్తులేని పొట్టి nlen;
  బిట్టు(multiline); /* response is multi-line with last '.' line */
  బిట్టు(multiline_with_వాదనలు); /* is multi-line when command has args */
};

స్థిర స్థిరరాశి నిర్మాణం pop3_cmd pop3cmds[] = {
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
స్థిర CURLcode pop3_parse_url_options(నిర్మాణం connectdata *conn)
{
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం *ptr = conn->options;

  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  అయితే(!ఫలితం && ptr && *ptr) {
    స్థిరరాశి అక్షరం *కీలకం = ptr;
    స్థిరరాశి అక్షరం *విలువ;

    అయితే(*ptr && *ptr != '=')
      ptr++;

    విలువ = ptr + 1;

    అయితే(*ptr && *ptr != ';')
      ptr++;

    ఒకవేళ(curl_strnequal(కీలకం, "AUTH=", 5)) {
      ఫలితం = Curl_sasl_parse_url_auth_option(&pop3c->sasl,
                                               విలువ, ptr - విలువ);

      ఒకవేళ(ఫలితం && curl_strnequal(విలువ, "+APOP", ptr - విలువ)) {
        pop3c->preftype = POP3_TYPE_APOP;
        pop3c->sasl.prefmech = SASL_AUTH_NONE;
        ఫలితం = CURLE_OK;
      }
    }
    లేకపోతే
      ఫలితం = CURLE_URL_MALFORMAT;

    ఒకవేళ(*ptr == ';')
      ptr++;
  }

  ఒకవేళ(pop3c->preftype != POP3_TYPE_APOP)
    ఎంపిక(pop3c->sasl.prefmech) {
    స్థితి SASL_AUTH_NONE:
      pop3c->preftype = POP3_TYPE_NONE;
      ఆపు;
    స్థితి SASL_AUTH_DEFAULT:
      pop3c->preftype = POP3_TYPE_ANY;
      ఆపు;
    అప్రమేయ:
      pop3c->preftype = POP3_TYPE_SASL;
      ఆపు;
    }

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * pop3_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
స్థిర CURLcode pop3_parse_url_మార్గం(నిర్మాణం Curl_easy *దత్తాంశం)
{
  /* The POP3 struct is already initialized in pop3_connect() */
  నిర్మాణం POP3 *pop3 = Curl_meta_get(దత్తాంశం, CURL_META_POP3_EASY);
  స్థిరరాశి అక్షరం *మార్గం = &దత్తాంశం->state.పైకి.మార్గం[1]; /* skip leading path */

  ఒకవేళ(!pop3)
    తిరిగి CURLE_FAILED_INIT;
  /* URL decode the path for the message ID */
  తిరిగి Curl_urldecode(మార్గం, 0, &pop3->id, ఖాళీ, REJECT_CTRL);
}

/***********************************************************************
 *
 * pop3_parse_custom_request()
 *
 * Parse the custom request.
 */
స్థిర CURLcode pop3_parse_custom_request(నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం POP3 *pop3 = Curl_meta_get(దత్తాంశం, CURL_META_POP3_EASY);
  స్థిరరాశి అక్షరం *custom = CURL_EASY_STR(దత్తాంశం, STRING_CUSTOMREQUEST);

  ఒకవేళ(!pop3)
    తిరిగి CURLE_FAILED_INIT;
  /* URL decode the custom request */
  ఒకవేళ(custom)
    ఫలితం = Curl_urldecode(custom, 0, &pop3->custom, ఖాళీ, REJECT_CTRL);

  తిరిగి ఫలితం;
}

/* Return iff a command is defined as "multi-line" (RFC 1939),
 * has a response terminated by a last line with a '.'.
 */
స్థిర bool pop3_is_multiline(స్థిరరాశి అక్షరం *cmdline)
{
  size_t i;
  కొరకు(i = 0; i < CURL_ARRAYSIZE(pop3cmds); ++i) {
    ఒకవేళ(curl_strnequal(pop3cmds[i].name, cmdline, pop3cmds[i].nlen)) {
      ఒకవేళ(!cmdline[pop3cmds[i].nlen])
        తిరిగి (bool)pop3cmds[i].multiline;
      లేకపోతే ఒకవేళ(cmdline[pop3cmds[i].nlen] == ' ')
        తిరిగి (bool)pop3cmds[i].multiline_with_వాదనలు;
    }
  }
  /* Unknown command, assume multi-line for backward compatibility with
   * earlier curl versions that only could do multi-line responses. */
  తిరిగి TRUE;
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
స్థిర bool pop3_endofresp(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం connectdata *conn,
                           స్థిరరాశి అక్షరం *పంక్తి, size_t len, పూర్ణాంకం *resp)
{
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (శూన్య)దత్తాంశం;
  DEBUGASSERT(pop3c);
  ఒకవేళ(!pop3c) /* internal error */
    తిరిగి TRUE;

  /* Do we have an error response? */
  ఒకవేళ(len >= 4 && !memcmp("-ERR", పంక్తి, 4)) {
    *resp = '-';

    తిరిగి TRUE;
  }

  /* Are we processing CAPA command responses? */
  ఒకవేళ(pop3c->state == POP3_CAPA) {
    /* Do we have the terminating line? Per RFC 2449 this is a line
       containing only a single dot */
    ఒకవేళ((len == 3 && పంక్తి[0] == '.' && పంక్తి[1] == '\r') ||
       (len == 2 && పంక్తి[0] == '.' && పంక్తి[1] == '\n'))
      /* Treat the response as a success */
      *resp = '+';
    లేకపోతే
      /* Treat the response as an untagged continuation */
      *resp = '*';

    తిరిగి TRUE;
  }

  /* Do we have a success response? */
  ఒకవేళ(len >= 3 && !memcmp("+OK", పంక్తి, 3)) {
    *resp = '+';

    తిరిగి TRUE;
  }

  /* Do we have a continuation response? */
  ఒకవేళ(len >= 1 && పంక్తి[0] == '+') {
    *resp = '*';

    తిరిగి TRUE;
  }

  తిరిగి FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * pop3_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
స్థిర CURLcode pop3_get_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం bufref *బహిర్గతం)
{
  నిర్మాణం pop3_conn *pop3c =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_POP3_CONN);
  అక్షరం *సందేశం_2;
  size_t len;

  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;
  సందేశం_2 = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;
  ఒకవేళ(len > 2) {
    /* Find the start of the message */
    len -= 2;
    కొరకు(సందేశం_2 += 2; ISBLANK(*సందేశం_2); సందేశం_2++, len--)
      ;

    /* Find the end of the message */
    అయితే(len--)
      ఒకవేళ(!ISBLANK(సందేశం_2[len]) && !ISNEWLINE(సందేశం_2[len]))
        ఆపు;

    /* Terminate the message */
    సందేశం_2[++len] = '\0';
    Curl_bufref_సమితి(బహిర్గతం, సందేశం_2, len, ఖాళీ);
  }
  లేకపోతే
    /* junk input => zero length output */
    Curl_bufref_సమితి(బహిర్గతం, "", 0, ఖాళీ);

  తిరిగి CURLE_OK;
}

/***********************************************************************
 *
 * pop3_state()
 *
 * This is the ONLY way to change POP3 state!
 */
స్థిర శూన్య pop3_state(నిర్మాణం Curl_easy *దత్తాంశం, pop3state newstate)
{
  నిర్మాణం pop3_conn *pop3c =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_POP3_CONN);
  ఒకవేళ(pop3c) {
#ఒకవేళ defined(DEBUGBUILD) && defined(CURLVERBOSE)
    /* for debug purposes */
    స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి పేర్లు[] = {
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
      /*చివరి*/
    };

    ఒకవేళ(pop3c->state != newstate)
      infof(దత్తాంశం, "POP3 %p state change from %s to %s",
            (శూన్య *)pop3c, పేర్లు[pop3c->state], పేర్లు[newstate]);
#అయితే_ముగింపు

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
స్థిర CURLcode pop3_perform_capa(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata *conn)
{
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  pop3c->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  pop3c->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  pop3c->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPA command */
  ఫలితం = Curl_pp_sendf(దత్తాంశం, &pop3c->pp, "%s", "CAPA");

  ఒకవేళ(!ఫలితం)
    pop3_state(దత్తాంశం, POP3_CAPA);

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * pop3_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
స్థిర CURLcode pop3_perform_starttls(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం connectdata *conn)
{
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ఫలితం;

  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  /* Send the STLS command */
  ఫలితం = Curl_pp_sendf(దత్తాంశం, &pop3c->pp, "%s", "STLS");
  ఒకవేళ(!ఫలితం)
    pop3_state(దత్తాంశం, POP3_STARTTLS);

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * pop3_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
స్థిర CURLcode pop3_perform_upgrade_tls(నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం connectdata *conn)
{
#నిర్వచితమైతే USE_SSL
  /* Start the SSL connection */
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ఫలితం;
  bool ssldone = FALSE;

  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    ఫలితం = Curl_ssl_cfilter_కూడు(
      దత్తాంశం, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_pop3s;
  }

  DEBUGASSERT(!pop3c->ssldone);
  ఫలితం = Curl_conn_connect(దత్తాంశం, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(దత్తాంశం, "pop3_perform_upgrade_tls, connect -> %d, %d",
               (పూర్ణాంకం)ఫలితం, ssldone));
  ఒకవేళ(!ఫలితం && ssldone) {
    pop3c->ssldone = ssldone;
    /* perform CAPA now, changes pop3c->state out of POP3_UPGRADETLS */
    ఫలితం = pop3_perform_capa(దత్తాంశం, conn);
  }
బహిర్గతం:
  తిరిగి ఫలితం;
#లేకపోతే
  (శూన్య)దత్తాంశం;
  (శూన్య)conn;
  తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
}

/***********************************************************************
 *
 * pop3_perform_user()
 *
 * Sends a clear text USER command to authenticate with.
 */
స్థిర CURLcode pop3_perform_user(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata *conn)
{
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  ఒకవేళ(!conn->creds) {
    pop3_state(దత్తాంశం, POP3_STOP);

    తిరిగి ఫలితం;
  }

  /* Send the USER command */
  ఫలితం = Curl_pp_sendf(దత్తాంశం, &pop3c->pp, "USER %s",
                         Curl_creds_user(conn->creds));
  ఒకవేళ(!ఫలితం)
    pop3_state(దత్తాంశం, POP3_USER);

  తిరిగి ఫలితం;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH
/***********************************************************************
 *
 * pop3_perform_apop()
 *
 * Sends an APOP command to authenticate with.
 */
స్థిర CURLcode pop3_perform_apop(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata *conn)
{
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ఫలితం = CURLE_OK;
  size_t i;
  నిర్మాణం MD5_context *ctxt;
  గుర్తులేని అక్షరం digest[MD5_DIGEST_LEN];
  అక్షరం secret[(2 * MD5_DIGEST_LEN) + 1];

  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  ఒకవేళ(!దత్తాంశం->state.creds) {
    pop3_state(దత్తాంశం, POP3_STOP);

    తిరిగి ఫలితం;
  }

  /* Create the digest */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  ఒకవేళ(!ctxt)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)pop3c->apoptimestamp,
                  curlx_uztoui(అక్షరశ్రేణి_పొడవు(pop3c->apoptimestamp)));

  Curl_MD5_update(ctxt, (స్థిరరాశి గుర్తులేని అక్షరం *)Curl_creds_passwd(conn->creds),
                  curlx_uztoui(అక్షరశ్రేణి_పొడవు(Curl_creds_passwd(conn->creds))));

  /* Finalise the digest */
  Curl_MD5_final(ctxt, digest);

  /* Convert the calculated 16 octet digest into a 32-byte hex string */
  కొరకు(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&secret[2 * i], 3, "%02x", digest[i]);

  ఫలితం = Curl_pp_sendf(దత్తాంశం, &pop3c->pp, "APOP %s %s",
                         Curl_creds_user(conn->creds), secret);

  ఒకవేళ(!ఫలితం)
    pop3_state(దత్తాంశం, POP3_APOP);

  తిరిగి ఫలితం;
}
#అయితే_ముగింపు

/***********************************************************************
 *
 * pop3_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
స్థిర CURLcode pop3_perform_auth(నిర్మాణం Curl_easy *దత్తాంశం,
                                  స్థిరరాశి అక్షరం *mech,
                                  స్థిరరాశి నిర్మాణం bufref *initresp)
{
  నిర్మాణం pop3_conn *pop3c =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_POP3_CONN);
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం *ir = Curl_bufref_ptr(initresp);

  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    ఫలితం = Curl_pp_sendf(దత్తాంశం, &pop3c->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  లేకపోతే {
    /* Send the AUTH command */
    ఫలితం = Curl_pp_sendf(దత్తాంశం, &pop3c->pp, "AUTH %s", mech);
  }

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * pop3_continue_auth()
 *
 * Sends SASL continuation data.
 */
స్థిర CURLcode pop3_continue_auth(నిర్మాణం Curl_easy *దత్తాంశం,
                                   స్థిరరాశి అక్షరం *mech,
                                   స్థిరరాశి నిర్మాణం bufref *resp)
{
  నిర్మాణం pop3_conn *pop3c =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_POP3_CONN);

  (శూన్య)mech;
  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  తిరిగి Curl_pp_sendf(దత్తాంశం, &pop3c->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * pop3_cancel_auth()
 *
 * Sends SASL cancellation.
 */
స్థిర CURLcode pop3_cancel_auth(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *mech)
{
  నిర్మాణం pop3_conn *pop3c =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_POP3_CONN);

  (శూన్య)mech;
  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  తిరిగి Curl_pp_sendf(దత్తాంశం, &pop3c->pp, "*");
}

/***********************************************************************
 *
 * pop3_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to APOP and clear text should a
 * common mechanism not be available between the client and server.
 */
స్థిర CURLcode pop3_perform_authentication(నిర్మాణం Curl_easy *దత్తాంశం,
                                            నిర్మాణం connectdata *conn)
{
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ఫలితం = CURLE_OK;
  saslprogress పురోగతి = SASL_IDLE;

  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  /* Check we have enough data to authenticate with and end the
     connect phase if we do not */
  ఒకవేళ(!Curl_sasl_can_authenticate(&pop3c->sasl, దత్తాంశం)) {
    pop3_state(దత్తాంశం, POP3_STOP);
    తిరిగి ఫలితం;
  }

  ఒకవేళ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_SASL) {
    /* Calculate the SASL login details */
    ఫలితం = Curl_sasl_ప్రారంభం(&pop3c->sasl, దత్తాంశం, FALSE, &పురోగతి);

    ఒకవేళ(!ఫలితం && పురోగతి == SASL_INPROGRESS)
      pop3_state(దత్తాంశం, POP3_AUTH);
  }

  ఒకవేళ(!ఫలితం && పురోగతి == SASL_IDLE) {
#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH
    ఒకవేళ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
      /* Perform APOP authentication */
      ఫలితం = pop3_perform_apop(దత్తాంశం, conn);
    లేకపోతే
#అయితే_ముగింపు
    ఒకవేళ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
      /* Perform clear text authentication */
      ఫలితం = pop3_perform_user(దత్తాంశం, conn);
    లేకపోతే
      ఫలితం = Curl_sasl_is_blocked(&pop3c->sasl, దత్తాంశం);
  }

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * pop3_perform_command()
 *
 * Sends a POP3 based command.
 */
స్థిర CURLcode pop3_perform_command(నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం POP3 *pop3 = Curl_meta_get(దత్తాంశం, CURL_META_POP3_EASY);
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  స్థిరరాశి అక్షరం *command = ఖాళీ;

  ఒకవేళ(!pop3 || !pop3c)
    తిరిగి CURLE_FAILED_INIT;

  /* Calculate the default command */
  ఒకవేళ(pop3->id[0] == '\0' || దత్తాంశం->సమితి.list_only) {
    command = "LIST";

    ఒకవేళ(pop3->id[0] != '\0')
      /* Message specific LIST so skip the BODY transfer */
      pop3->transfer = PPTRANSFER_INFO;
  }
  లేకపోతే
    command = "RETR";

  ఒకవేళ(pop3->custom && pop3->custom[0] != '\0')
    command = pop3->custom;

  /* Send the command */
  ఒకవేళ(pop3->id[0] != '\0')
    ఫలితం = Curl_pp_sendf(దత్తాంశం, &pop3c->pp, "%s %s", command, pop3->id);
  లేకపోతే
    ఫలితం = Curl_pp_sendf(దత్తాంశం, &pop3c->pp, "%s", command);

  ఒకవేళ(!ఫలితం) {
    pop3_state(దత్తాంశం, POP3_COMMAND);
    దత్తాంశం->req.no_body = !pop3_is_multiline(command);
  }

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * pop3_perform_quit()
 *
 * Performs the quit action prior to sclose() be called.
 */
స్థిర CURLcode pop3_perform_quit(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata *conn)
{
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ఫలితం;

  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  /* Send the QUIT command */
  ఫలితం = Curl_pp_sendf(దత్తాంశం, &pop3c->pp, "%s", "QUIT");
  ఒకవేళ(!ఫలితం)
    pop3_state(దత్తాంశం, POP3_QUIT);

  తిరిగి ఫలితం;
}

/* For the initial server greeting */
స్థిర CURLcode pop3_state_servergreet_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                            పూర్ణాంకం pop3code,
                                            pop3state instate)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  స్థిరరాశి అక్షరం *పంక్తి;
  size_t len;

  (శూన్య)instate;
  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  పంక్తి = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  ఒకవేళ(pop3code != '+') {
    failf(దత్తాంశం, "Got unexpected pop3-server response");
    ఫలితం = CURLE_WEIRD_SERVER_జవాబు;
  }
  లేకపోతే ఒకవేళ(len > 3) {
    /* Does the server support APOP authentication? */
    స్థిరరాశి అక్షరం *lt;
    స్థిరరాశి అక్షరం *gt = ఖాళీ;

    /* Look for the APOP timestamp */
    lt = memchr(పంక్తి, '<', len);
    ఒకవేళ(lt)
      /* search the remainder for '>' */
      gt = memchr(lt, '>', len - (lt - పంక్తి));
    ఒకవేళ(gt) {
      /* the length of the timestamp, including the brackets */
      size_t timestamplen = gt - lt + 1;
      స్థిరరాశి అక్షరం *at = memchr(lt, '@', timestamplen);
      /* If the timestamp does not contain '@' it is not (as required by
         RFC-1939) conformant to the RFC-822 message id syntax, and we
         therefore do not use APOP authentication. */
      ఒకవేళ(at) {
        /* dupe the timestamp */
        pop3c->apoptimestamp = curlx_memdup0(lt, timestamplen);
        ఒకవేళ(!pop3c->apoptimestamp)
          తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        /* Store the APOP capability */
        pop3c->authtypes |= POP3_TYPE_APOP;
      }
    }

    ఒకవేళ(!ఫలితం)
      ఫలితం = pop3_perform_capa(దత్తాంశం, conn);
  }

  తిరిగి ఫలితం;
}

/* For CAPA responses */
స్థిర CURLcode pop3_state_capa_resp(నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం pop3code,
                                     pop3state instate)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  స్థిరరాశి అక్షరం *పంక్తి;
  size_t len;

  (శూన్య)instate;
  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  పంక్తి = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  /* Do we have an untagged continuation response? */
  ఒకవేళ(pop3code == '*') {
    /* Does the server support the STLS capability? */
    ఒకవేళ(len >= 4 && curl_strnequal(పంక్తి, "STLS", 4))
      pop3c->tls_supported = TRUE;

    /* Does the server support clear text authentication? */
    లేకపోతే ఒకవేళ(len >= 4 && curl_strnequal(పంక్తి, "USER", 4))
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    /* Does the server support SASL based authentication? */
    లేకపోతే ఒకవేళ(len >= 5 && curl_strnequal(పంక్తి, "SASL ", 5)) {
      pop3c->authtypes |= POP3_TYPE_SASL;

      /* Advance past the SASL keyword */
      పంక్తి += 5;
      len -= 5;

      /* Loop through the data line */
      కొరకు(;;) {
        size_t llen;
        size_t wordlen = 0;
        గుర్తులేని పొట్టి mechbit;

        అయితే(len && (ISBLANK(*పంక్తి) || ISNEWLINE(*పంక్తి))) {
          పంక్తి++;
          len--;
        }

        ఒకవేళ(!len)
          ఆపు;

        /* Extract the word */
        అయితే(wordlen < len && !ISBLANK(పంక్తి[wordlen]) &&
              !ISNEWLINE(పంక్తి[wordlen]))
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(పంక్తి, wordlen, &llen);
        ఒకవేళ(mechbit && llen == wordlen)
          pop3c->sasl.authmechs |= mechbit;

        పంక్తి += wordlen;
        len -= wordlen;
      }
    }
  }
  లేకపోతే {
    /* Clear text is supported when CAPA is not recognised */
    ఒకవేళ(pop3code != '+')
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    ఒకవేళ(!దత్తాంశం->సమితి.use_ssl || Curl_conn_is_ssl(conn, FIRSTSOCKET))
      ఫలితం = pop3_perform_authentication(దత్తాంశం, conn);
    లేకపోతే ఒకవేళ(pop3code == '+' && pop3c->tls_supported)
      /* Switch to TLS connection now */
      ఫలితం = pop3_perform_starttls(దత్తాంశం, conn);
    లేకపోతే ఒకవేళ(దత్తాంశం->సమితి.use_ssl <= CURLUSESSL_TRY)
      /* Fallback and carry on with authentication */
      ఫలితం = pop3_perform_authentication(దత్తాంశం, conn);
    లేకపోతే {
      failf(దత్తాంశం, "STLS not supported.");
      ఫలితం = CURLE_USE_SSL_FAILED;
    }
  }

  తిరిగి ఫలితం;
}

/* For STARTTLS responses */
స్థిర CURLcode pop3_state_starttls_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం connectdata *conn,
                                         పూర్ణాంకం pop3code,
                                         pop3state instate)
{
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ఫలితం = CURLE_OK;
  (శూన్య)instate;

  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  /* Pipelining in response is forbidden. */
  ఒకవేళ(pop3c->pp.అధికప్రవాహం)
    తిరిగి CURLE_WEIRD_SERVER_జవాబు;

  ఒకవేళ(pop3code != '+') {
    ఒకవేళ(దత్తాంశం->సమితి.use_ssl != CURLUSESSL_TRY) {
      failf(దత్తాంశం, "STARTTLS denied");
      ఫలితం = CURLE_USE_SSL_FAILED;
    }
    లేకపోతే
      ఫలితం = pop3_perform_authentication(దత్తాంశం, conn);
  }
  లేకపోతే
    pop3_state(దత్తాంశం, POP3_UPGRADETLS);

  తిరిగి ఫలితం;
}

/* For SASL authentication responses */
స్థిర CURLcode pop3_state_auth_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                     పూర్ణాంకం pop3code,
                                     pop3state instate)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  saslprogress పురోగతి;

  (శూన్య)instate;
  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  ఫలితం = Curl_sasl_continue(&pop3c->sasl, దత్తాంశం, pop3code, &పురోగతి);
  ఒకవేళ(!ఫలితం)
    ఎంపిక(పురోగతి) {
    స్థితి SASL_పూర్తి:
      pop3_state(దత్తాంశం, POP3_STOP);  /* Authenticated */
      ఆపు;
    స్థితి SASL_IDLE:            /* No mechanism left after cancellation */
#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH
      ఒకవేళ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
        /* Perform APOP authentication */
        ఫలితం = pop3_perform_apop(దత్తాంశం, conn);
      లేకపోతే
#అయితే_ముగింపు
      ఒకవేళ(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
        /* Perform clear text authentication */
        ఫలితం = pop3_perform_user(దత్తాంశం, conn);
      లేకపోతే {
        failf(దత్తాంశం, "Authentication cancelled");
        ఫలితం = CURLE_LOGIN_DENIED;
      }
      ఆపు;
    అప్రమేయ:
      ఆపు;
    }

  తిరిగి ఫలితం;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH
/* For APOP responses */
స్థిర CURLcode pop3_state_apop_resp(నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం pop3code,
                                     pop3state instate)
{
  CURLcode ఫలితం = CURLE_OK;
  (శూన్య)instate;

  ఒకవేళ(pop3code != '+') {
    failf(దత్తాంశం, "Authentication failed: %d", pop3code);
    ఫలితం = CURLE_LOGIN_DENIED;
  }
  లేకపోతే
    /* End of connect phase */
    pop3_state(దత్తాంశం, POP3_STOP);

  తిరిగి ఫలితం;
}
#అయితే_ముగింపు

/* For USER responses */
స్థిర CURLcode pop3_state_user_resp(నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం pop3code,
                                     pop3state instate)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (శూన్య)instate;

  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(pop3code != '+') {
    failf(దత్తాంశం, "Access denied. %c", pop3code);
    ఫలితం = CURLE_LOGIN_DENIED;
  }
  లేకపోతే
    /* Send the PASS command */
    ఫలితం = Curl_pp_sendf(దత్తాంశం, &pop3c->pp, "PASS %s",
                           Curl_creds_passwd(conn->creds));
  ఒకవేళ(!ఫలితం)
    pop3_state(దత్తాంశం, POP3_PASS);

  తిరిగి ఫలితం;
}

/* For PASS responses */
స్థిర CURLcode pop3_state_pass_resp(నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం pop3code,
                                     pop3state instate)
{
  CURLcode ఫలితం = CURLE_OK;
  (శూన్య)instate;

  ఒకవేళ(pop3code != '+') {
    failf(దత్తాంశం, "Access denied. %c", pop3code);
    ఫలితం = CURLE_LOGIN_DENIED;
  }
  లేకపోతే
    /* End of connect phase */
    pop3_state(దత్తాంశం, POP3_STOP);

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * pop3_write()
 *
 * This function scans the body after the end-of-body and writes everything
 * until the end is found.
 */
స్థిర CURLcode pop3_write(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *str,
                           size_t nread, bool is_eos)
{
  /* This code could be made into a special function in the handler struct */
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  bool strip_dot = FALSE;
  size_t చివరి = 0;
  size_t i;
  (శూన్య)is_eos;

  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  /* Search through the buffer looking for the end-of-body marker which is
     5 bytes (0d 0a 2e 0d 0a). Note that a line starting with a dot matches
     the eob so the server will have prefixed it with an extra dot which we
     need to strip out. Additionally the marker could of course be spread out
     over 5 different data chunks. */
  కొరకు(i = 0; i < nread; i++) {
    size_t prev = pop3c->eob;

    ఎంపిక(str[i]) {
    స్థితి 0x0d:
      ఒకవేళ(pop3c->eob == 0) {
        pop3c->eob++;

        ఒకవేళ(i) {
          /* Write out the body part that did not match */
          ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, &str[చివరి],
                                     i - చివరి);

          ఒకవేళ(ఫలితం)
            తిరిగి ఫలితం;

          చివరి = i;
        }
      }
      లేకపోతే ఒకవేళ(pop3c->eob == 3)
        pop3c->eob++;
      లేకపోతే
        /* If the character match was not at position 0 or 3 then restart the
           pattern matching */
        pop3c->eob = 1;
      ఆపు;

    స్థితి 0x0a:
      ఒకవేళ(pop3c->eob == 1 || pop3c->eob == 4)
        pop3c->eob++;
      లేకపోతే
        /* If the character match was not at position 1 or 4 then start the
           search again */
        pop3c->eob = 0;
      ఆపు;

    స్థితి 0x2e:
      ఒకవేళ(pop3c->eob == 2)
        pop3c->eob++;
      లేకపోతే ఒకవేళ(pop3c->eob == 3) {
        /* We have an extra dot after the CRLF which we need to strip off */
        strip_dot = TRUE;
        pop3c->eob = 0;
      }
      లేకపోతే
        /* If the character match was not at position 2 then start the search
           again */
        pop3c->eob = 0;
      ఆపు;

    అప్రమేయ:
      pop3c->eob = 0;
      ఆపు;
    }

    /* Did we have a partial match which has subsequently failed? */
    ఒకవేళ(prev && prev >= pop3c->eob) {
      /* Strip can only be non-zero for the first mismatch after CRLF and
         then both prev and strip are equal and nothing will be output below */
      అయితే(prev && pop3c->strip) {
        prev--;
        pop3c->strip--;
      }

      ఒకవేళ(prev) {
        /* If the partial match was the CRLF and dot then only write the CRLF
           as the server would have inserted the dot */
        ఒకవేళ(strip_dot && prev - 1 > 0) {
          ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, POP3_EOB,
                                     prev - 1);
        }
        లేకపోతే ఒకవేళ(!strip_dot) {
          ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, POP3_EOB,
                                     prev);
        }
        లేకపోతే {
          ఫలితం = CURLE_OK;
        }

        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;

        చివరి = i;
        strip_dot = FALSE;
      }
    }
  }

  ఒకవేళ(pop3c->eob == POP3_EOB_LEN) {
    /* We have a full match so the transfer is done, however we must transfer
    the CRLF at the start of the EOB as this is considered to be part of the
    message as per RFC-1939, sect. 3 */
    ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, POP3_EOB, 2);

    CURL_REQ_CLEAR_RECV(దత్తాంశం);
    pop3c->eob = 0;

    తిరిగి ఫలితం;
  }

  ఒకవేళ(pop3c->eob)
    /* While EOB is matching nothing should be output */
    తిరిగి CURLE_OK;

  ఒకవేళ(nread - చివరి) {
    ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, &str[చివరి],
                               nread - చివరి);
  }

  తిరిగి ఫలితం;
}

/* For command responses */
స్థిర CURLcode pop3_state_command_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                        పూర్ణాంకం pop3code,
                                        pop3state instate)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం POP3 *pop3 = Curl_meta_get(దత్తాంశం, CURL_META_POP3_EASY);
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  నిర్మాణం pingpong *pp;

  (శూన్య)instate;
  ఒకవేళ(!pop3 || !pop3c)
    తిరిగి CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  ఒకవేళ(pop3code != '+') {
    pop3_state(దత్తాంశం, POP3_STOP);
    తిరిగి CURLE_WEIRD_SERVER_జవాబు;
  }

  /* This 'OK' line ends with a CR LF pair which is the two first bytes of the
     EOB string so count this is two matching bytes. This is necessary to make
     the code detect the EOB if the only data than comes now is %2e CR LF like
     when there is no body to return. */
  pop3c->eob = 2;

  /* Since this initial CR LF pair is not part of the actual body, we set
     the strip counter here so that these bytes will not be delivered. */
  pop3c->strip = 2;

  ఒకవేళ(pop3->transfer == PPTRANSFER_BODY) {
    /* POP3 download */
    Curl_xfer_అమరిక_recv(దత్తాంశం, FIRSTSOCKET, -1);

    ఒకవేళ(pp->అధికప్రవాహం) {
      /* The recv buffer contains data that is actually body content so send
         it as such. Note that there may even be additional "headers" after
         the body */

      /* keep only the overflow */
      curlx_dyn_తోక(&pp->recvbuf, pp->అధికప్రవాహం);
      pp->nfinal = 0; /* done */

      ఒకవేళ(!దత్తాంశం->req.no_body) {
        ఫలితం = pop3_write(దత్తాంశం, curlx_dyn_ptr(&pp->recvbuf),
                            curlx_dyn_len(&pp->recvbuf), FALSE);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;
      }

      /* reset the buffer */
      curlx_dyn_reset(&pp->recvbuf);
      pp->అధికప్రవాహం = 0;
    }
  }
  లేకపోతే
    pp->అధికప్రవాహం = 0;

  /* End of DO phase */
  pop3_state(దత్తాంశం, POP3_STOP);

  తిరిగి ఫలితం;
}

స్థిర CURLcode pop3_statemachine(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata *conn)
{
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం pop3code;
  నిర్మాణం pingpong *pp;
  size_t nread = 0;
  (శూన్య)దత్తాంశం;

  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not POP3 */
upgrade_tls:
  ఒకవేళ(pop3c->state == POP3_UPGRADETLS) {
    ఫలితం = pop3_perform_upgrade_tls(దత్తాంశం, conn);
    ఒకవేళ(ఫలితం || (pop3c->state == POP3_UPGRADETLS))
      తిరిగి ఫలితం;
  }

  /* Flush any data that needs to be sent */
  ఒకవేళ(pp->sendleft)
    తిరిగి Curl_pp_flushsend(దత్తాంశం, pp);

  చేయి {
     /* Read the response from the server */
    ఫలితం = Curl_pp_readresp(దత్తాంశం, FIRSTSOCKET, pp, &pop3code, &nread);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    ఒకవేళ(!pop3code)
      ఆపు;

    /* We have now received a full POP3 server response */
    ఎంపిక(pop3c->state) {
    స్థితి POP3_SERVERGREET:
      ఫలితం = pop3_state_servergreet_resp(దత్తాంశం, pop3code, pop3c->state);
      ఆపు;

    స్థితి POP3_CAPA:
      ఫలితం = pop3_state_capa_resp(దత్తాంశం, pop3code, pop3c->state);
      ఆపు;

    స్థితి POP3_STARTTLS:
      ఫలితం = pop3_state_starttls_resp(దత్తాంశం, conn, pop3code, pop3c->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      ఒకవేళ(!ఫలితం && (pop3c->state == POP3_UPGRADETLS))
        వెళ్ళు upgrade_tls;
      ఆపు;

    స్థితి POP3_AUTH:
      ఫలితం = pop3_state_auth_resp(దత్తాంశం, pop3code, pop3c->state);
      ఆపు;

#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH
    స్థితి POP3_APOP:
      ఫలితం = pop3_state_apop_resp(దత్తాంశం, pop3code, pop3c->state);
      ఆపు;
#అయితే_ముగింపు

    స్థితి POP3_USER:
      ఫలితం = pop3_state_user_resp(దత్తాంశం, pop3code, pop3c->state);
      ఆపు;

    స్థితి POP3_PASS:
      ఫలితం = pop3_state_pass_resp(దత్తాంశం, pop3code, pop3c->state);
      ఆపు;

    స్థితి POP3_COMMAND:
      ఫలితం = pop3_state_command_resp(దత్తాంశం, pop3code, pop3c->state);
      ఆపు;

    స్థితి POP3_QUIT:
      pop3_state(దత్తాంశం, POP3_STOP);
      ఆపు;

    అప్రమేయ:
      /* internal error */
      pop3_state(దత్తాంశం, POP3_STOP);
      ఆపు;
    }
  } అయితే(!ఫలితం && pop3c->state != POP3_STOP && Curl_pp_moredata(pp));

  తిరిగి ఫలితం;
}

/* Called repeatedly until done from multi.c */
స్థిర CURLcode pop3_multi_statemach(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;
  ఫలితం = Curl_pp_statemach(దత్తాంశం, &pop3c->pp, FALSE, FALSE);
  *పూర్తి = (pop3c->state == POP3_STOP);

  తిరిగి ఫలితం;
}

స్థిర CURLcode pop3_ఖండం_statemach(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం connectdata *conn,
                                     bool disconnecting)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  అయితే(pop3c->state != POP3_STOP && !ఫలితం)
    ఫలితం = Curl_pp_statemach(దత్తాంశం, &pop3c->pp, TRUE, disconnecting);

  తిరిగి ఫలితం;
}

/* For the POP3 "protocol connect" and "doing" phases only */
స్థిర CURLcode pop3_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం pop3_conn *pop3c =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_POP3_CONN);
  తిరిగి pop3c ? Curl_pp_pollset(దత్తాంశం, &pop3c->pp, ps) : CURLE_OK;
}

/* SASL parameters for the pop3 protocol */
స్థిర స్థిరరాశి నిర్మాణం SASLproto saslpop3 = {
  "pop",                /* The service name */
  pop3_perform_auth,    /* Send authentication command */
  pop3_continue_auth,   /* Send authentication continuation */
  pop3_cancel_auth,     /* Send authentication cancellation */
  pop3_get_సందేశం,     /* Get SASL response message */
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
స్థిర CURLcode pop3_connect(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  నిర్మాణం pingpong *pp = pop3c ? &pop3c->pp : ఖాళీ;

  *పూర్తి = FALSE; /* default to not done yet */
  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  PINGPONG_అమరిక(pp, pop3_statemachine, pop3_endofresp);

  /* Set the default preferred authentication type and mechanism */
  pop3c->preftype = POP3_TYPE_ANY;
  Curl_sasl_init(&pop3c->sasl, దత్తాంశం, &saslpop3);

  /* Initialize the pingpong layer */
  Curl_pp_init(pp, Curl_pgrs_now(దత్తాంశం));

  /* Parse the URL options */
  ఫలితం = pop3_parse_url_options(conn);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* Start off waiting for the server greeting response */
  pop3_state(దత్తాంశం, POP3_SERVERGREET);

  ఫలితం = pop3_multi_statemach(దత్తాంశం, పూర్తి);

  తిరిగి ఫలితం;
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
స్థిర CURLcode pop3_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, CURLcode స్థితి_2,
                          bool premature)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం POP3 *pop3 = Curl_meta_get(దత్తాంశం, CURL_META_POP3_EASY);

  (శూన్య)premature;

  ఒకవేళ(!pop3)
    తిరిగి CURLE_OK;

  ఒకవేళ(స్థితి_2) {
    CURL_TRC_M(దత్తాంశం, "POP3 done with bad status");
    connclose(దత్తాంశం->conn);
    ఫలితం = స్థితి_2;         /* use the already set error code */
  }

  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);

  /* Clear the transfer mode for the next request */
  pop3->transfer = PPTRANSFER_BODY;

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * pop3_perform()
 *
 * This is the actual DO function for POP3. Get a message/listing according to
 * the options previously setup.
 */
స్థిర CURLcode pop3_perform(నిర్మాణం Curl_easy *దత్తాంశం, bool *connected,
                             bool *dophase_పూర్తి)
{
  /* This is POP3 and no proxy */
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం POP3 *pop3 = Curl_meta_get(దత్తాంశం, CURL_META_POP3_EASY);

  ఒకవేళ(!pop3)
    తిరిగి CURLE_FAILED_INIT;

  DEBUGF(infof(దత్తాంశం, "DO phase starts"));

  /* Start the first command in the DO phase, may alter data->req.no_body */
  ఫలితం = pop3_perform_command(దత్తాంశం);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఒకవేళ(దత్తాంశం->req.no_body)
    /* Requested no body means no transfer */
    pop3->transfer = PPTRANSFER_INFO;

  *dophase_పూర్తి = FALSE; /* not done yet */

  /* Run the state-machine */
  ఫలితం = pop3_multi_statemach(దత్తాంశం, dophase_పూర్తి);
  *connected = Curl_conn_is_connected(దత్తాంశం->conn, FIRSTSOCKET);

  ఒకవేళ(*dophase_పూర్తి)
    DEBUGF(infof(దత్తాంశం, "DO phase is complete"));

  తిరిగి ఫలితం;
}

/* Call this when the DO phase has completed */
స్థిర CURLcode pop3_dophase_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, bool connected)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)connected;

  తిరిగి CURLE_OK;
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
స్థిర CURLcode pop3_regular_transfer(నిర్మాణం Curl_easy *దత్తాంశం,
                                      bool *dophase_పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  దత్తాంశం->req.కొలత = -1;

  /* Set the progress data */
  Curl_pgrsReset(దత్తాంశం);

  /* Carry out the perform */
  ఫలితం = pop3_perform(దత్తాంశం, &connected, dophase_పూర్తి);

  /* Perform post DO phase operations if necessary */
  ఒకవేళ(!ఫలితం && *dophase_పూర్తి)
    ఫలితం = pop3_dophase_పూర్తి(దత్తాంశం, connected);

  తిరిగి ఫలితం;
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
స్థిర CURLcode pop3_do(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  *పూర్తి = FALSE; /* default to false */

  /* Parse the URL path */
  ఫలితం = pop3_parse_url_మార్గం(దత్తాంశం);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* Parse the custom request */
  ఫలితం = pop3_parse_custom_request(దత్తాంశం);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఫలితం = pop3_regular_transfer(దత్తాంశం, పూర్తి);

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * pop3_disconnect()
 *
 * Disconnect from an POP3 server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
స్థిర CURLcode pop3_disconnect(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం connectdata *conn, bool dead_connection)
{
  నిర్మాణం pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (శూన్య)దత్తాంశం;

  ఒకవేళ(!pop3c)
    తిరిగి CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  ఒకవేళ(!dead_connection && conn->బిట్లు.protoconnstart &&
     !Curl_pp_needs_flush(దత్తాంశం, &pop3c->pp) &&
     !pop3_perform_quit(దత్తాంశం, conn))
    (శూన్య)pop3_ఖండం_statemach(దత్తాంశం, conn, TRUE); /* ignore errors on QUIT */

  /* Disconnect from the server */
  Curl_pp_disconnect(&pop3c->pp);

  /* Cleanup our connection based variables */
  curlx_safefree(pop3c->apoptimestamp);

  తిరిగి CURLE_OK;
}

/* Called from multi.c while DOing */
స్థిర CURLcode pop3_doing(నిర్మాణం Curl_easy *దత్తాంశం, bool *dophase_పూర్తి)
{
  CURLcode ఫలితం = pop3_multi_statemach(దత్తాంశం, dophase_పూర్తి);

  ఒకవేళ(ఫలితం)
    DEBUGF(infof(దత్తాంశం, "DO phase failed"));
  లేకపోతే ఒకవేళ(*dophase_పూర్తి) {
    ఫలితం = pop3_dophase_పూర్తి(దత్తాంశం, FALSE /* not connected */);

    DEBUGF(infof(దత్తాంశం, "DO phase is complete"));
  }

  తిరిగి ఫలితం;
}

స్థిర శూన్య pop3_easy_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం POP3 *pop3 = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  DEBUGASSERT(pop3);
  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);
  curlx_free(pop3);
}

స్థిర శూన్య pop3_conn_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం pop3_conn *pop3c = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  DEBUGASSERT(pop3c);
  Curl_pp_disconnect(&pop3c->pp);
  curlx_safefree(pop3c->apoptimestamp);
  curlx_free(pop3c);
}

స్థిర CURLcode pop3_అమరిక_connection(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం connectdata *conn)
{
  నిర్మాణం pop3_conn *pop3c;
  నిర్మాణం POP3 *pop3 = curlx_calloc(1, పరిమాణం(*pop3));
  ఒకవేళ(!pop3 ||
     Curl_meta_సమితి(దత్తాంశం, CURL_META_POP3_EASY, pop3, pop3_easy_dtor))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  pop3c = curlx_calloc(1, పరిమాణం(*pop3c));
  ఒకవేళ(!pop3c ||
     Curl_conn_meta_సమితి(conn, CURL_META_POP3_CONN, pop3c, pop3_conn_dtor))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  తిరిగి CURLE_OK;
}

/*
 * POP3 protocol.
 */
స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_pop3 = {
  pop3_అమరిక_connection,            /* setup_connection */
  pop3_do,                          /* do_it */
  pop3_పూర్తి,                        /* done */
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

#అయితే_ముగింపు /* CURL_DISABLE_POP3 */
