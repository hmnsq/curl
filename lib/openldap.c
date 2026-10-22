/***************************************************************************
 *                      _   _ ____  _
 *  Project         ___| | | |  _ \| |
 *                 / __| | | | |_) | |
 *                | (__| |_| |  _ <| |___
 *                 \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 * Copyright (C) Howard Chu, <hyc@openldap.org>
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
 ***************************************************************************/
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP)

/*
 * Notice that USE_OPENLDAP is only a source code selection switch. When
 * libcurl is built with USE_OPENLDAP defined the libcurl source code that
 * gets compiled is the code from openldap.c, otherwise the code that gets
 * compiled is the code from ldap.c.
 *
 * When USE_OPENLDAP is defined a recent version of the OpenLDAP library
 * might be required for compilation and runtime. In order to use ancient
 * OpenLDAP library versions, USE_OPENLDAP shall not be defined.
 */

#అన్తర్భావ <ldap.h>

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "curl_ldap.h"
#అన్తర్భావ "curlx/base64.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "curl_sasl.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "bufref.h"

/*
 * Uncommenting this will enable the built-in debug logging of the openldap
 * library. The debug log level can be set using the CURL_OPENLDAP_TRACE
 * environment variable. The debug output is written to stderr.
 *
 * The library supports the following debug flags:
 * LDAP_DEBUG_NONE         0x0000
 * LDAP_DEBUG_TRACE        0x0001
 * LDAP_DEBUG_CONSTRUCT    0x0002
 * LDAP_DEBUG_DESTROY      0x0004
 * LDAP_DEBUG_PARAMETER    0x0008
 * LDAP_DEBUG_ANY          0xffff
 *
 * For example, use CURL_OPENLDAP_TRACE=0 for no debug,
 * CURL_OPENLDAP_TRACE=2 for LDAP_DEBUG_CONSTRUCT messages only,
 * CURL_OPENLDAP_TRACE=65535 for all debug message levels.
 */
/* #define CURL_OPENLDAP_DEBUG */

/* Machine states. */
రకనిర్వచనం గణన {
  OLDAP_STOP,           /* Do nothing state, stops the state machine */
  OLDAP_SSL,            /* Performing SSL handshake. */
  OLDAP_STARTTLS,       /* STARTTLS request sent. */
  OLDAP_TLS,            /* Performing TLS handshake. */
  OLDAP_MECHS,          /* Get SASL authentication mechanisms. */
  OLDAP_SASL,           /* SASL binding reply. */
  OLDAP_BIND,           /* Simple bind reply. */
  OLDAP_BINDV2,         /* Simple bind reply in protocol version 2. */
  OLDAP_చివరి            /* Never used */
} ldapstate;

#నిర్వచితం_కాకపోతే _LDAP_PVT_H
బాహ్య పూర్ణాంకం ldap_pvt_url_scheme2proto(స్థిరరాశి అక్షరం *);
బాహ్య పూర్ణాంకం ldap_init_fd(ber_socket_t fd, పూర్ణాంకం proto, స్థిరరాశి అక్షరం *url,
                        LDAP **ld);
#అయితే_ముగింపు

స్థిర Curl_recv oldap_recv;

నిర్మాణం ldapconninfo {
  నిర్మాణం SASL sasl;          /* SASL-related parameters */
  LDAP *ld;                  /* Openldap connection handle. */
  Curl_recv *recv;           /* For stacking SSL handler */
  Curl_send *send;
  నిర్మాణం berval *servercred; /* SASL data from server. */
  ldapstate state;           /* Current machine state. */
  curl_socket_t dupfd;       /* duplicated socket */
  పూర్ణాంకం proto;                 /* LDAP_PROTO_TCP/LDAP_PROTO_UDP/LDAP_PROTO_IPC */
  పూర్ణాంకం msgid;                 /* Current message id. */
};

నిర్మాణం ldapreqinfo {
  పూర్ణాంకం msgid;
  పూర్ణాంకం nument;
};

/* meta key for storing ldapconninfo at easy handle */
#నిర్వచించు CURL_META_LDAP_EASY   "meta:proto:ldap:easy"
/* meta key for storing ldapconninfo at connection */
#నిర్వచించు CURL_META_LDAP_CONN   "meta:proto:ldap:conn"

/*
 * oldap_state()
 *
 * This is the ONLY way to change LDAP state!
 */
స్థిర శూన్య oldap_state(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం ldapconninfo *li,
                        ldapstate newstate)
{
#ఒకవేళ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి పేర్లు[] = {
    "STOP",
    "SSL",
    "STARTTLS",
    "TLS",
    "MECHS",
    "SASL",
    "BIND",
    "BINDV2",
    /*చివరి*/
  };

  ఒకవేళ(li->state != newstate)
    infof(దత్తాంశం, "LDAP %p state change from %s to %s",
          (శూన్య *)li, పేర్లు[li->state], పేర్లు[newstate]);
#లేకపోతే
  (శూన్య)దత్తాంశం;
#అయితే_ముగింపు
  li->state = newstate;
}

/* Map some particular LDAP error codes to CURLcode values. */
స్థిర CURLcode oldap_map_error(పూర్ణాంకం rc, CURLcode ఫలితం)
{
  ఎంపిక(rc) {
  స్థితి LDAP_NO_జ్ఞాపకం:
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  స్థితి LDAP_INVALID_CREDENTIALS:
    తిరిగి CURLE_LOGIN_DENIED;
  స్థితి LDAP_PROTOCOL_ERROR:
    తిరిగి CURLE_UNSUPPORTED_PROTOCOL;
  స్థితి LDAP_INSUFFICIENT_ACCESS:
    తిరిగి CURLE_REMOTE_ACCESS_DENIED;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode oldap_url_parse(నిర్మాణం Curl_easy *దత్తాంశం, LDAPURLDesc **ludp)
{
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం rc;
  స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి url_errs[] = {
    "success",
    "out of memory",
    "bad parameter",
    "unrecognized scheme",
    "unbalanced delimiter",
    "bad URL",
    "bad host or port",
    "bad or missing attributes",
    "bad or missing scope",
    "bad or missing filter",
    "bad or missing extensions"
  };

  *ludp = ఖాళీ;
  /* `ldap_url_parse() seems to be terrible with urls
   * that have user/pass/options in it. So when we have options or
   * creds from the url, fail without calling the function.
   * Yes, this is super-weird code and I do not like it. */
  ఒకవేళ((దత్తాంశం->state.creds && (దత్తాంశం->state.creds->ఆధారం == CREDS_URL)) ||
     దత్తాంశం->state.పైకి.options)
    rc = LDAP_URL_ERR_BADURL;
  లేకపోతే
    rc = ldap_url_parse(Curl_bufref_ptr(&దత్తాంశం->state.url), ludp);

  ఒకవేళ(rc != LDAP_URL_SUCCESS) {
    స్థిరరాశి అక్షరం *సందేశం = "url parsing problem";

    ఫలితం = rc == LDAP_URL_ERR_MEM ? CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం :
      CURLE_URL_MALFORMAT;
    rc -= LDAP_URL_SUCCESS;
    ఒకవేళ((size_t)rc < CURL_ARRAYSIZE(url_errs))
      సందేశం = url_errs[rc];
    failf(దత్తాంశం, "LDAP local: %s", సందేశం);
  }
  తిరిగి ఫలితం;
}

/* Parse the login options. */
స్థిర CURLcode oldap_parse_login_options(నిర్మాణం connectdata *conn)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  స్థిరరాశి అక్షరం *ptr = conn->options;

  DEBUGASSERT(li);
  ఒకవేళ(!li)
    తిరిగి CURLE_FAILED_INIT;

  అయితే(!ఫలితం && ptr && *ptr) {
    స్థిరరాశి అక్షరం *కీలకం = ptr;
    స్థిరరాశి అక్షరం *విలువ;

    అయితే(*ptr && *ptr != '=')
      ptr++;

    విలువ = ptr + 1;

    అయితే(*ptr && *ptr != ';')
      ptr++;

    ఒకవేళ(checkprefix("AUTH=", కీలకం))
      ఫలితం = Curl_sasl_parse_url_auth_option(&li->sasl, విలువ, ptr - విలువ);
    లేకపోతే
      ఫలితం = CURLE_SETOPT_OPTION_SYNTAX;

    ఒకవేళ(*ptr == ';')
      ptr++;
  }

  తిరిగి ఫలితం == CURLE_URL_MALFORMAT ? CURLE_SETOPT_OPTION_SYNTAX : ఫలితం;
}

స్థిర CURLcode oldap_అమరిక_connection(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం connectdata *conn)
{
  CURLcode ఫలితం;
  LDAPURLDesc *lud;
  (శూన్య)conn;

  /* Early URL syntax check. */
  ఫలితం = oldap_url_parse(దత్తాంశం, &lud);
  ldap_free_urldesc(lud);

  తిరిగి ఫలితం;
}

/*
 * Get the SASL authentication challenge from the server credential buffer.
 */
స్థిర CURLcode oldap_get_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం bufref *బహిర్గతం)
{
  నిర్మాణం ldapconninfo *li =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_LDAP_CONN);
  నిర్మాణం berval *servercred = li ? li->servercred : ఖాళీ;
  DEBUGASSERT(li);
  ఒకవేళ(!li)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(!servercred || !servercred->bv_val)
    తిరిగి CURLE_WEIRD_SERVER_జవాబు;
  Curl_bufref_సమితి(బహిర్గతం, servercred->bv_val, servercred->bv_len, ఖాళీ);
  తిరిగి CURLE_OK;
}

/*
 * Sends an initial SASL bind request to the server.
 */
స్థిర CURLcode oldap_perform_auth(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *mech,
                                   స్థిరరాశి నిర్మాణం bufref *initresp)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  నిర్మాణం berval cred;
  నిర్మాణం berval *pcred = &cred;
  పూర్ణాంకం rc;

  DEBUGASSERT(li);
  ఒకవేళ(!li)
    తిరిగి CURLE_FAILED_INIT;
  cred.bv_val = (అక్షరం *)CURL_UNCONST(Curl_bufref_ptr(initresp));
  cred.bv_len = Curl_bufref_len(initresp);
  ఒకవేళ(!cred.bv_val)
    pcred = ఖాళీ;
  rc = ldap_sasl_bind(li->ld, ఖాళీ, mech, pcred, ఖాళీ, ఖాళీ, &li->msgid);
  ఒకవేళ(rc != LDAP_SUCCESS)
    తిరిగి oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  తిరిగి CURLE_OK;
}

/*
 * Sends SASL continuation.
 */
స్థిర CURLcode oldap_continue_auth(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *mech,
                                    స్థిరరాశి నిర్మాణం bufref *resp)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  నిర్మాణం berval cred;
  నిర్మాణం berval *pcred = &cred;
  పూర్ణాంకం rc;

  ఒకవేళ(!li)
    తిరిగి CURLE_FAILED_INIT;
  cred.bv_val = (అక్షరం *)CURL_UNCONST(Curl_bufref_ptr(resp));
  cred.bv_len = Curl_bufref_len(resp);
  ఒకవేళ(!cred.bv_val)
    pcred = ఖాళీ;
  rc = ldap_sasl_bind(li->ld, ఖాళీ, mech, pcred, ఖాళీ, ఖాళీ, &li->msgid);
  ఒకవేళ(rc != LDAP_SUCCESS)
    తిరిగి oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  తిరిగి CURLE_OK;
}

/*
 * Sends SASL bind cancellation.
 */
స్థిర CURLcode oldap_cancel_auth(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *mech)
{
  నిర్మాణం ldapconninfo *li =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_LDAP_CONN);
  పూర్ణాంకం rc;

  (శూన్య)mech;
  ఒకవేళ(!li)
    తిరిగి CURLE_FAILED_INIT;
  rc = ldap_sasl_bind(li->ld, ఖాళీ, LDAP_SASL_NULL, ఖాళీ, ఖాళీ, ఖాళీ,
                      &li->msgid);
  ఒకవేళ(rc != LDAP_SUCCESS)
    తిరిగి oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  తిరిగి CURLE_OK;
}

/* Starts LDAP simple bind. */
స్థిర CURLcode oldap_perform_bind(నిర్మాణం Curl_easy *దత్తాంశం, ldapstate newstate)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  స్థిరరాశి అక్షరం *binddn = ఖాళీ;
  నిర్మాణం berval passwd;
  పూర్ణాంకం rc;

  ఒకవేళ(!li)
    తిరిగి CURLE_FAILED_INIT;
  passwd.bv_val = ఖాళీ;
  passwd.bv_len = 0;

  ఒకవేళ(conn->creds) {
    binddn = Curl_creds_user(conn->creds);
    passwd.bv_val = CURL_UNCONST(Curl_creds_passwd(conn->creds));
    passwd.bv_len = అక్షరశ్రేణి_పొడవు(passwd.bv_val);
  }

  rc = ldap_sasl_bind(li->ld, binddn, LDAP_SASL_SIMPLE, &passwd,
                      ఖాళీ, ఖాళీ, &li->msgid);
  ఒకవేళ(rc != LDAP_SUCCESS)
    తిరిగి oldap_map_error(rc,
                           దత్తాంశం->state.creds ?
                           CURLE_LOGIN_DENIED : CURLE_LDAP_CANNOT_BIND);
  oldap_state(దత్తాంశం, li, newstate);
  తిరిగి CURLE_OK;
}

/* Query the supported SASL authentication mechanisms. */
స్థిర CURLcode oldap_perform_mechs(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ldapconninfo *li =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_LDAP_CONN);
  పూర్ణాంకం rc;
  స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి supportedSASLMechanisms[] = {
    "supportedSASLMechanisms",
    ఖాళీ
  };

  ఒకవేళ(!li)
    తిరిగి CURLE_FAILED_INIT;
  /* Casting away the const for the 3rd parameter that the LDAP API expects as
     a non-const char ** is potentially unsafe but we believe the lack of
     const in the API was an oversight and that no LDAP implementation
     actually modifies the input. */
  rc = ldap_శోధన_ext(li->ld, "", LDAP_SCOPE_పునాది, "(objectclass=*)",
                       (అక్షరం **)CURL_UNCONST(supportedSASLMechanisms), 0,
                       ఖాళీ, ఖాళీ, ఖాళీ, 0, &li->msgid);
  ఒకవేళ(rc != LDAP_SUCCESS)
    తిరిగి oldap_map_error(rc, CURLE_LOGIN_DENIED);
  oldap_state(దత్తాంశం, li, OLDAP_MECHS);
  తిరిగి CURLE_OK;
}

/* Starts SASL bind. */
స్థిర CURLcode oldap_perform_sasl(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ldapconninfo *li =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_LDAP_CONN);
  saslprogress పురోగతి = SASL_IDLE;
  CURLcode ఫలితం;

  ఒకవేళ(!li)
    తిరిగి CURLE_FAILED_INIT;
  ఫలితం = Curl_sasl_ప్రారంభం(&li->sasl, దత్తాంశం, TRUE, &పురోగతి);

  oldap_state(దత్తాంశం, li, OLDAP_SASL);
  ఒకవేళ(!ఫలితం && పురోగతి != SASL_INPROGRESS)
    ఫలితం = Curl_sasl_is_blocked(&li->sasl, దత్తాంశం);
  తిరిగి ఫలితం;
}

#నిర్వచితమైతే USE_SSL
స్థిర పూర్ణాంకం ldapsb_tls_అమరిక(Sockbuf_IO_Desc *sbiod, శూన్య *arg)
{
  sbiod->sbiod_pvt = arg;
  తిరిగి 0;
}

స్థిర పూర్ణాంకం ldapsb_tls_remove(Sockbuf_IO_Desc *sbiod)
{
  sbiod->sbiod_pvt = ఖాళీ;
  తిరిగి 0;
}

/* if there is a duplicated socket to close, close it */
స్థిర పూర్ణాంకం ldapsb_tls_close(Sockbuf_IO_Desc *sbiod)
{
  నిర్మాణం Curl_easy *దత్తాంశం = sbiod->sbiod_pvt;
  ఒకవేళ(దత్తాంశం) {
    నిర్మాణం connectdata *conn = దత్తాంశం->conn;
    ఒకవేళ(conn) {
      నిర్మాణం ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      ఒకవేళ(li && (li->dupfd != CURL_SOCKET_BAD)) {
        sclose(li->dupfd);
        li->dupfd = CURL_SOCKET_BAD;
      }
    }
  }

  తిరిగి 0;
}

స్థిర పూర్ణాంకం ldapsb_tls_ctrl(Sockbuf_IO_Desc *sbiod, పూర్ణాంకం opt, శూన్య *arg)
{
  (శూన్య)arg;
  ఒకవేళ(opt == LBER_SB_OPT_దత్తాంశం_READY) {
    నిర్మాణం Curl_easy *దత్తాంశం = sbiod->sbiod_pvt;
    తిరిగి Curl_conn_దత్తాంశం_pending(దత్తాంశం, FIRSTSOCKET);
  }
  తిరిగి 0;
}

స్థిర ber_slen_t ldapsb_tls_read(Sockbuf_IO_Desc *sbiod, శూన్య *buf,
                                  ber_len_t len)
{
  నిర్మాణం Curl_easy *దత్తాంశం = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  ఒకవేళ(దత్తాంశం) {
    నిర్మాణం connectdata *conn = దత్తాంశం->conn;
    ఒకవేళ(conn) {
      నిర్మాణం ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode ఫలితం = CURLE_RECV_ERROR;
      size_t nread;

      ఒకవేళ(!li) {
        సమితి_SOCKERRNO(SOCKEINVAL);
        తిరిగి -1;
      }
      ఫలితం = (li->recv)(దత్తాంశం, FIRSTSOCKET, buf, len, &nread);
      ఒకవేళ(ఫలితం == CURLE_AGAIN) {
        సమితి_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = ఫలితం ? -1 : (ber_slen_t)nread;
    }
  }
  తిరిగి ret;
}

స్థిర ber_slen_t ldapsb_tls_write(Sockbuf_IO_Desc *sbiod, శూన్య *buf,
                                   ber_len_t len)
{
  నిర్మాణం Curl_easy *దత్తాంశం = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  ఒకవేళ(దత్తాంశం) {
    నిర్మాణం connectdata *conn = దత్తాంశం->conn;
    ఒకవేళ(conn) {
      నిర్మాణం ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode ఫలితం = CURLE_SEND_ERROR;
      size_t nwritten;

      ఒకవేళ(!li) {
        సమితి_SOCKERRNO(SOCKEINVAL);
        తిరిగి -1;
      }
      ఫలితం = (li->send)(దత్తాంశం, FIRSTSOCKET, buf, len, FALSE, &nwritten);
      ఒకవేళ(ఫలితం == CURLE_AGAIN) {
        సమితి_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = ఫలితం ? -1 : (ber_slen_t)nwritten;
    }
  }
  తిరిగి ret;
}

స్థిర Sockbuf_IO ldapsb_tls = {
  ldapsb_tls_అమరిక,
  ldapsb_tls_remove,
  ldapsb_tls_ctrl,
  ldapsb_tls_read,
  ldapsb_tls_write,
  ldapsb_tls_close
};

స్థిర bool ssl_installed(నిర్మాణం connectdata *conn)
{
  నిర్మాణం ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  తిరిగి li && li->recv;
}

స్థిర CURLcode oldap_ssl_connect(నిర్మాణం Curl_easy *దత్తాంశం, ldapstate newstate)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  bool ssldone = FALSE;
  CURLcode ఫలితం;

  ఒకవేళ(!li)
    తిరిగి CURLE_FAILED_INIT;
  ఫలితం = Curl_conn_connect(దత్తాంశం, FIRSTSOCKET, FALSE, &ssldone);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  oldap_state(దత్తాంశం, li, newstate);

  ఒకవేళ(ssldone) {
    Sockbuf *sb;

    /* Install the libcurl SSL handlers into the sockbuf. */
    ఒకవేళ((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_కూడు_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, దత్తాంశం))
      తిరిగి CURLE_FAILED_INIT;
    li->recv = conn->recv[FIRSTSOCKET];
    li->send = conn->send[FIRSTSOCKET];
  }

  తిరిగి ఫలితం;
}

/* Send the STARTTLS request */
స్థిర CURLcode oldap_perform_starttls(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ldapconninfo *li =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_LDAP_CONN);
  పూర్ణాంకం rc;

  ఒకవేళ(!li)
    తిరిగి CURLE_FAILED_INIT;
  rc = ldap_ప్రారంభం_tls(li->ld, ఖాళీ, ఖాళీ, &li->msgid);
  ఒకవేళ(rc != LDAP_SUCCESS)
    తిరిగి oldap_map_error(rc, CURLE_USE_SSL_FAILED);
  oldap_state(దత్తాంశం, li, OLDAP_STARTTLS);
  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు

స్థిర శూన్య oldap_easy_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం ldapreqinfo *lr = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  curlx_free(lr);
}

స్థిర శూన్య oldap_conn_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం ldapconninfo *li = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  ఒకవేళ(li->ld) {
    ldap_unbind_ext(li->ld, ఖాళీ, ఖాళీ);
    li->ld = ఖాళీ;
  }
  curlx_free(li);
}

/* SASL parameters for the ldap protocol */
స్థిర స్థిరరాశి నిర్మాణం SASLproto saslldap = {
  "ldap",                     /* The service name */
  oldap_perform_auth,         /* Send authentication command */
  oldap_continue_auth,        /* Send authentication continuation */
  oldap_cancel_auth,          /* Send authentication cancellation */
  oldap_get_సందేశం,          /* Get SASL response message */
  0,                          /* Maximum initial response length (no max) */
  LDAP_SASL_BIND_లోపల_పురోగతి, /* Code received when continuation is expected */
  LDAP_SUCCESS,               /* Code to receive upon authentication success */
  SASL_AUTH_NONE,             /* Default mechanisms */
  0                           /* Configuration flags */
};

స్థిర CURLcode oldap_connect(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ldapconninfo *li;
  స్థిర స్థిరరాశి పూర్ణాంకం సంస్కరణ_2 = LDAP_VERSION3;
  అక్షరం *hosturl = ఖాళీ;
  CURLcode ఫలితం;
  పూర్ణాంకం rc;
#నిర్వచితమైతే CURL_OPENLDAP_DEBUG
  స్థిర పూర్ణాంకం do_trace = -1;
#అయితే_ముగింపు

  (శూన్య)పూర్తి;

  li = curlx_calloc(1, పరిమాణం(నిర్మాణం ldapconninfo));
  ఒకవేళ(!li) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  li->dupfd = CURL_SOCKET_BAD;

  ఫలితం = Curl_conn_meta_సమితి(conn, CURL_META_LDAP_CONN, li, oldap_conn_dtor);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  li->proto = ldap_pvt_url_scheme2proto(దత్తాంశం->state.origin->scheme->name);

  /* Initialize the SASL storage */
  Curl_sasl_init(&li->sasl, దత్తాంశం, &saslldap);

  ఫలితం = oldap_parse_login_options(conn);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  hosturl = curl_maprintf("%s://%s:%u",
                          conn->scheme->name,
                          conn->origin->ipv6 ?
                          conn->origin->user_hostname :
                          conn->origin->hostname,
                          conn->origin->పోర్టు);
  ఒకవేళ(!hosturl) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  {
#నిర్వచితమైతే _WIN32
    SOCKET dupfd = INVALID_SOCKET;
    WSAPROTOCOL_INFO pi;
    ఒకవేళ(WSADuplicateSocket(conn->sock[FIRSTSOCKET], GetCurrentProcessId(),
                          &pi))
      ; /* error */
    లేకపోతే
      dupfd = CURL_SOCKET(pi.i_చిరునామా_Family, pi.iSocketType, pi.iProtocol);
#లేకపోతే
#నిర్వచితమైతే F_DUPFD_CLOEXEC
    పూర్ణాంకం dupfd = fcntl(conn->sock[FIRSTSOCKET], F_DUPFD_CLOEXEC, 0);
#లేకపోతే
    పూర్ణాంకం dupfd = dup(conn->sock[FIRSTSOCKET]);
#నిర్వచితమైతే HAVE_FCNTL
    ఒకవేళ((dupfd != CURL_SOCKET_BAD) &&
       (fcntl(dupfd, F_SETFD, FD_CLOEXEC) < 0)) {
      sclose(dupfd);
      dupfd = CURL_SOCKET_BAD;
    }
#అయితే_ముగింపు /* HAVE_FCNTL */
#అయితే_ముగింపు /* F_DUPFD_CLOEXEC */
#అయితే_ముగింపు /* _WIN32 */

    ఒకవేళ(dupfd == CURL_SOCKET_BAD) {
      ఫలితం = CURLE_COULDNT_CONNECT;
      వెళ్ళు బహిర్గతం;
    }
    rc = ldap_init_fd((ber_socket_t)dupfd, li->proto, hosturl, &li->ld);
    ఒకవేళ(rc)
      sclose(dupfd);
    లేకపోతే
      /* store the socket, as when this is used by TLS we close this ourselves
         and at this point the code does not yet know if this will use TLS or
         not */
      li->dupfd = dupfd;
  }
  ఒకవేళ(rc) {
    failf(దత్తాంశం, "LDAP local: Cannot connect to %s, %s",
          hosturl, ldap_err2string(rc));
    ఫలితం = CURLE_COULDNT_CONNECT;
    వెళ్ళు బహిర్గతం;
  }

#నిర్వచితమైతే CURL_OPENLDAP_DEBUG
  ఒకవేళ(do_trace < 0) {
    స్థిరరాశి అక్షరం *env = getenv("CURL_OPENLDAP_TRACE");
    curl_off_t e = 0;
    ఒకవేళ(!curlx_str_సంఖ్య(&env, &e, గరిష్ఠ_పూర్ణాంకం))
      do_trace = e > 0;
  }
  ఒకవేళ(do_trace)
    ldap_సమితి_option(li->ld, LDAP_OPT_DEBUG_LEVEL, &do_trace);
#అయితే_ముగింపు

  /* Try version 3 first. */
  ldap_సమితి_option(li->ld, LDAP_OPT_PROTOCOL_సంస్కరణ, &సంస్కరణ_2);

  /* Do not chase referrals. */
  ldap_సమితి_option(li->ld, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  {
    ber_len_t max = 256 * 1024;
    Sockbuf *sb;
    ఒకవేళ((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       /* Set the maximum allowed size of an incoming message, which to
          OpenLDAP means that it will malloc() memory up to this size. If not
          set, there is no limit and we instead risk a malloc() failure. */
       !ber_sockbuf_ctrl(sb, LBER_SB_OPT_సమితి_MAX_INCOMING, &max)) {
      ఫలితం = CURLE_FAILED_INIT;
      వెళ్ళు బహిర్గతం;
    }
  }

#నిర్వచితమైతే USE_SSL
  ఒకవేళ(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    ఫలితం = oldap_ssl_connect(దత్తాంశం, OLDAP_SSL);
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(దత్తాంశం->సమితి.use_ssl) {
    ఫలితం = oldap_perform_starttls(దత్తాంశం);
    ఒకవేళ(!ఫలితం || దత్తాంశం->సమితి.use_ssl != CURLUSESSL_TRY)
      వెళ్ళు బహిర్గతం;
  }
#అయితే_ముగింపు

  ఒకవేళ(li->sasl.prefmech != SASL_AUTH_NONE) {
    ఫలితం = oldap_perform_mechs(దత్తాంశం);
    వెళ్ళు బహిర్గతం;
  }

  /* Force bind even if anonymous bind is not needed in protocol version 3
     to detect missing version 3 support. */
  ఫలితం = oldap_perform_bind(దత్తాంశం, OLDAP_BIND);

బహిర్గతం:
  curlx_free(hosturl);
  తిరిగి ఫలితం;
}

/* Handle the supported SASL mechanisms query response */
స్థిర CURLcode oldap_state_mechs_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                       LDAPMessage *సందేశం, పూర్ణాంకం సంకేతం_2)
{
  నిర్మాణం connectdata *conn;
  నిర్మాణం ldapconninfo *li;
  పూర్ణాంకం rc;
  BerElement *ber = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం berval bv, *bvals;

  ఒకవేళ(!దత్తాంశం)
    తిరిగి CURLE_FAILED_INIT;

  conn = దత్తాంశం->conn;
  li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);

  ఒకవేళ(!li)
    తిరిగి CURLE_FAILED_INIT;
  ఎంపిక(ldap_msgtype(సందేశం)) {
  స్థితి LDAP_RES_శోధన_నమోదు:
    /* Got a list of supported SASL mechanisms. */
    ఒకవేళ(సంకేతం_2 != LDAP_SUCCESS && సంకేతం_2 != LDAP_NO_RESULTS_RETURNED)
      తిరిగి CURLE_LOGIN_DENIED;

    rc = ldap_get_dn_ber(li->ld, సందేశం, &ber, &bv);
    ఒకవేళ(rc < 0)
      తిరిగి oldap_map_error(rc, CURLE_BAD_CONTENT_ENCODING);
    కొరకు(rc = ldap_get_attribute_ber(li->ld, సందేశం, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, సందేశం, ber, &bv, &bvals)) {
      పూర్ణాంకం i;

      ఒకవేళ(!bv.bv_val)
        ఆపు;

      ఒకవేళ(bvals) {
        కొరకు(i = 0; bvals[i].bv_val; i++) {
          size_t llen;
          గుర్తులేని పొట్టి mech =
            Curl_sasl_decode_mech((స్థిరరాశి అక్షరం *)bvals[i].bv_val,
                                  bvals[i].bv_len, &llen);
          ఒకవేళ(bvals[i].bv_len == llen)
            li->sasl.authmechs |= mech;
        }
        ber_memfree(bvals);
      }
    }
    ber_free(ber, 0);
    ఆపు;

  స్థితి LDAP_RES_శోధన_ఫలితం:
    ఎంపిక(సంకేతం_2) {
    స్థితి LDAP_SIZELIMIT_EXCEEDED:
      infof(దత్తాంశం, "Too many authentication mechanisms");
      FALLTHROUGH();
    స్థితి LDAP_SUCCESS:
    స్థితి LDAP_NO_RESULTS_RETURNED:
      ఒకవేళ(Curl_sasl_can_authenticate(&li->sasl, దత్తాంశం))
        ఫలితం = oldap_perform_sasl(దత్తాంశం);
      లేకపోతే
        ఫలితం = CURLE_LOGIN_DENIED;
      ఆపు;
    అప్రమేయ:
      ఫలితం = oldap_map_error(సంకేతం_2, CURLE_LOGIN_DENIED);
      ఆపు;
    }
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి ఫలితం;
}

/* Handle a SASL bind response. */
స్థిర CURLcode oldap_state_sasl_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                      LDAPMessage *సందేశం, పూర్ణాంకం సంకేతం_2)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode ఫలితం = CURLE_OK;
  saslprogress పురోగతి;
  పూర్ణాంకం rc;

  ఒకవేళ(!li)
    తిరిగి CURLE_FAILED_INIT;
  li->servercred = ఖాళీ;
  rc = ldap_parse_sasl_bind_ఫలితం(li->ld, సందేశం, &li->servercred, 0);
  ఒకవేళ(rc != LDAP_SUCCESS) {
    failf(దత్తాంశం, "LDAP local: sasl ldap_parse_result %s", ldap_err2string(rc));
    ఫలితం = oldap_map_error(rc, CURLE_LOGIN_DENIED);
  }
  లేకపోతే {
    ఫలితం = Curl_sasl_continue(&li->sasl, దత్తాంశం, సంకేతం_2, &పురోగతి);
    ఒకవేళ(!ఫలితం) {
      ఎంపిక(పురోగతి) {
      స్థితి SASL_పూర్తి:
        oldap_state(దత్తాంశం, li, OLDAP_STOP);   /* Authenticated */
        ఆపు;
      స్థితి SASL_IDLE:            /* No mechanism left after cancellation */
        failf(దత్తాంశం, "Authentication cancelled");
        ఫలితం = CURLE_LOGIN_DENIED;
        ఆపు;
      అప్రమేయ:
        ఆపు;
      }
    }
  }

  ఒకవేళ(li->servercred)
    ber_bvfree(li->servercred);
  తిరిగి ఫలితం;
}

/* Handle a simple bind response. */
స్థిర CURLcode oldap_state_bind_resp(నిర్మాణం Curl_easy *దత్తాంశం, LDAPMessage *సందేశం,
                                      పూర్ణాంకం సంకేతం_2)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం berval *bv = ఖాళీ;
  పూర్ణాంకం rc;

  ఒకవేళ(!li)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(సంకేతం_2 != LDAP_SUCCESS)
    తిరిగి oldap_map_error(సంకేతం_2, CURLE_LDAP_CANNOT_BIND);

  rc = ldap_parse_sasl_bind_ఫలితం(li->ld, సందేశం, &bv, 0);
  ఒకవేళ(rc != LDAP_SUCCESS) {
    failf(దత్తాంశం, "LDAP local: bind ldap_parse_sasl_bind_result %s",
          ldap_err2string(rc));
    ఫలితం = oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  }
  లేకపోతే
    oldap_state(దత్తాంశం, li, OLDAP_STOP);

  ఒకవేళ(bv)
    ber_bvfree(bv);
  తిరిగి ఫలితం;
}

స్థిర CURLcode oldap_connecting(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  LDAPMessage *సందేశం = ఖాళీ;
  నిర్మాణం timeval tv = { 0, 0 };
  పూర్ణాంకం సంకేతం_2 = LDAP_SUCCESS;
  పూర్ణాంకం rc;

  ఒకవేళ(!li)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(li->state != OLDAP_SSL && li->state != OLDAP_TLS) {
    /* Get response to last command. */
    rc = ldap_ఫలితం(li->ld, li->msgid, LDAP_సందేశం_ONE, &tv, &సందేశం);
    ఎంపిక(rc) {
    స్థితి 0:                               /* Timed out. */
      తిరిగి CURLE_OK;
    స్థితి LDAP_RES_శోధన_నమోదు:
    స్థితి LDAP_RES_శోధన_REFERENCE:
      ఆపు;
    అప్రమేయ:
      li->msgid = 0;                      /* Nothing to abandon upon error. */
      ఒకవేళ(rc < 0) {
        failf(దత్తాంశం, "LDAP local: connecting ldap_result %s",
              ldap_err2string(rc));
        తిరిగి oldap_map_error(rc, CURLE_COULDNT_CONNECT);
      }
      ఆపు;
    }

    /* Get error code from message. */
    rc = ldap_parse_ఫలితం(li->ld, సందేశం, &సంకేతం_2, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, 0);
    ఒకవేళ(rc)
      సంకేతం_2 = rc;
    లేకపోతే {
      /* store the latest code for later retrieval */
      దత్తాంశం->info.httpcode = సంకేతం_2;
    }

    /* If protocol version 3 is not supported, fallback to version 2. */
    ఒకవేళ(సంకేతం_2 == LDAP_PROTOCOL_ERROR && li->state != OLDAP_BINDV2 &&
#నిర్వచితమైతే USE_SSL
       (ssl_installed(conn) || దత్తాంశం->సమితి.use_ssl <= CURLUSESSL_TRY) &&
#అయితే_ముగింపు
       li->sasl.prefmech == SASL_AUTH_NONE) {
      స్థిర స్థిరరాశి పూర్ణాంకం సంస్కరణ_2 = LDAP_VERSION2;

      ldap_సమితి_option(li->ld, LDAP_OPT_PROTOCOL_సంస్కరణ, &సంస్కరణ_2);
      ldap_msgfree(సందేశం);
      తిరిగి oldap_perform_bind(దత్తాంశం, OLDAP_BINDV2);
    }
  }

  /* Handle response message according to current state. */
  ఎంపిక(li->state) {

#నిర్వచితమైతే USE_SSL
  స్థితి OLDAP_SSL:
    ఫలితం = oldap_ssl_connect(దత్తాంశం, OLDAP_SSL);
    ఒకవేళ(!ఫలితం && ssl_installed(conn)) {
      ఒకవేళ(li->sasl.prefmech != SASL_AUTH_NONE)
        ఫలితం = oldap_perform_mechs(దత్తాంశం);
      లేకపోతే
        ఫలితం = oldap_perform_bind(దత్తాంశం, OLDAP_BIND);
    }
    ఆపు;
  స్థితి OLDAP_STARTTLS:
    ఒకవేళ(సంకేతం_2 != LDAP_SUCCESS) {
      ఒకవేళ(దత్తాంశం->సమితి.use_ssl != CURLUSESSL_TRY)
        ఫలితం = oldap_map_error(సంకేతం_2, CURLE_USE_SSL_FAILED);
      లేకపోతే ఒకవేళ(li->sasl.prefmech != SASL_AUTH_NONE)
        ఫలితం = oldap_perform_mechs(దత్తాంశం);
      లేకపోతే
        ఫలితం = oldap_perform_bind(దత్తాంశం, OLDAP_BIND);
      ఆపు;
    }
    ఫలితం = Curl_ssl_cfilter_కూడు(
      దత్తాంశం, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ఒకవేళ(ఫలితం)
      ఆపు;
    FALLTHROUGH();
  స్థితి OLDAP_TLS:
    ఫలితం = oldap_ssl_connect(దత్తాంశం, OLDAP_TLS);
    ఒకవేళ(ఫలితం)
      ఫలితం = oldap_map_error(సంకేతం_2, CURLE_USE_SSL_FAILED);
    లేకపోతే ఒకవేళ(ssl_installed(conn)) {
      ఒకవేళ(li->sasl.prefmech != SASL_AUTH_NONE)
        ఫలితం = oldap_perform_mechs(దత్తాంశం);
      లేకపోతే ఒకవేళ(దత్తాంశం->state.creds)
        ఫలితం = oldap_perform_bind(దత్తాంశం, OLDAP_BIND);
      లేకపోతే {
        /* Version 3 supported: no bind required */
        oldap_state(దత్తాంశం, li, OLDAP_STOP);
        ఫలితం = CURLE_OK;
      }
    }
    ఆపు;
#అయితే_ముగింపు

  స్థితి OLDAP_MECHS:
    ఫలితం = oldap_state_mechs_resp(దత్తాంశం, సందేశం, సంకేతం_2);
    ఆపు;
  స్థితి OLDAP_SASL:
    ఫలితం = oldap_state_sasl_resp(దత్తాంశం, సందేశం, సంకేతం_2);
    ఆపు;
  స్థితి OLDAP_BIND:
  స్థితి OLDAP_BINDV2:
    ఫలితం = oldap_state_bind_resp(దత్తాంశం, సందేశం, సంకేతం_2);
    ఆపు;
  అప్రమేయ:
    /* internal error */
    ఫలితం = CURLE_COULDNT_CONNECT;
    ఆపు;
  }

  ldap_msgfree(సందేశం);

  *పూర్తి = li->state == OLDAP_STOP;
  ఒకవేళ(*పూర్తి)
    conn->recv[FIRSTSOCKET] = oldap_recv;

  ఒకవేళ(ఫలితం && li->msgid) {
    ldap_abandon_ext(li->ld, li->msgid, ఖాళీ, ఖాళీ);
    li->msgid = 0;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode oldap_disconnect(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం connectdata *conn,
                                 bool dead_connection)
{
  నిర్మాణం ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  (శూన్య)dead_connection;
#నిర్వచితం_కాకపోతే USE_SSL
  (శూన్య)దత్తాంశం;
#అయితే_ముగింపు

  ఒకవేళ(li && li->ld) {
#నిర్వచితమైతే USE_SSL
    ఒకవేళ(ssl_installed(conn)) {
      Sockbuf *sb;
      ఒకవేళ(ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS ||
         ber_sockbuf_కూడు_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, దత్తాంశం))
        తిరిగి CURLE_FAILED_INIT;
    }
#అయితే_ముగింపు
    ldap_unbind_ext(li->ld, ఖాళీ, ఖాళీ);
    li->ld = ఖాళీ;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode oldap_do(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  నిర్మాణం ldapreqinfo *lr;
  CURLcode ఫలితం;
  పూర్ణాంకం rc;
  LDAPURLDesc *lud;
  పూర్ణాంకం msgid;

  ఒకవేళ(!li)
    తిరిగి CURLE_FAILED_INIT;

  infof(దత్తాంశం, "LDAP local: %s", Curl_bufref_ptr(&దత్తాంశం->state.url));

  ఫలితం = oldap_url_parse(దత్తాంశం, &lud);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

#నిర్వచితమైతే USE_SSL
  ఒకవేళ(ssl_installed(conn)) {
    Sockbuf *sb;
    /* re-install the libcurl SSL handlers into the sockbuf. */
    ఒకవేళ((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_కూడు_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, దత్తాంశం)) {
      ldap_free_urldesc(lud);
      తిరిగి CURLE_FAILED_INIT;
    }
  }
#అయితే_ముగింపు

  rc = ldap_శోధన_ext(li->ld, lud->lud_dn, lud->lud_scope,
                       lud->lud_filter, lud->lud_attrs, 0,
                       ఖాళీ, ఖాళీ, ఖాళీ, 0, &msgid);
  ldap_free_urldesc(lud);
  ఒకవేళ(rc != LDAP_SUCCESS) {
    failf(దత్తాంశం, "LDAP local: ldap_search_ext %s", ldap_err2string(rc));
    ఫలితం = CURLE_LDAP_శోధన_FAILED;
    వెళ్ళు బహిర్గతం;
  }

  lr = curlx_calloc(1, పరిమాణం(నిర్మాణం ldapreqinfo));
  ఒకవేళ(!lr ||
     Curl_meta_సమితి(దత్తాంశం, CURL_META_LDAP_EASY, lr, oldap_easy_dtor)) {
    ldap_abandon_ext(li->ld, msgid, ఖాళీ, ఖాళీ);
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  lr->msgid = msgid;
  Curl_xfer_అమరిక_recv(దత్తాంశం, FIRSTSOCKET, -1);
  *పూర్తి = TRUE;

బహిర్గతం:
  తిరిగి ఫలితం;
}

స్థిర CURLcode oldap_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, CURLcode res,
                           bool premature)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ldapreqinfo *lr = Curl_meta_get(దత్తాంశం, CURL_META_LDAP_EASY);

  (శూన్య)res;
  (శూన్య)premature;

  ఒకవేళ(lr) {
    /* if there was a search in progress, abandon it */
    ఒకవేళ(lr->msgid) {
      నిర్మాణం ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      ఒకవేళ(li && li->ld) {
        ldap_abandon_ext(li->ld, lr->msgid, ఖాళీ, ఖాళీ);
      }
      lr->msgid = 0;
    }
    Curl_meta_remove(దత్తాంశం, CURL_META_LDAP_EASY);
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode client_write(నిర్మాణం Curl_easy *దత్తాంశం,
                             స్థిరరాశి అక్షరం *ఉపసర్గ, size_t plen,
                             స్థిరరాశి అక్షరం *విలువ, size_t len,
                             స్థిరరాశి అక్షరం *suffix, size_t slen)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(ఉపసర్గ) {
    /* If we have a zero-length value and the prefix ends with a space
       separator, drop the latter. */
    ఒకవేళ(!len && plen && ఉపసర్గ[plen - 1] == ' ')
      plen--;
    ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, ఉపసర్గ, plen);
  }
  ఒకవేళ(!ఫలితం && విలువ) {
    ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, విలువ, len);
  }
  ఒకవేళ(!ఫలితం && suffix) {
    ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, suffix, slen);
  }
  తిరిగి ఫలితం;
}

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
స్థిర bool oldap_str_needs_base64(స్థిరరాశి అక్షరం *s, size_t len)
{
  size_t i;
  ఒకవేళ(!len)
    తిరిగి FALSE;
  ఒకవేళ((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    తిరిగి TRUE;
  కొరకు(i = 0; i < len; i++)
    ఒకవేళ(!ISPRINT(s[i]))
      తిరిగి TRUE;
  తిరిగి FALSE;
}

స్థిర CURLcode oldap_recv(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex, అక్షరం *buf,
                           size_t len, size_t *pnread)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  నిర్మాణం ldapreqinfo *lr = Curl_meta_get(దత్తాంశం, CURL_META_LDAP_EASY);
  పూర్ణాంకం rc;
  LDAPMessage *సందేశం = ఖాళీ;
  BerElement *ber = ఖాళీ;
  నిర్మాణం timeval tv = { 0, 0 };
  నిర్మాణం berval bv, *bvals;
  CURLcode ఫలితం = CURLE_AGAIN;
  పూర్ణాంకం సంకేతం_2;
  అక్షరం *info = ఖాళీ;

  (శూన్య)len;
  (శూన్య)buf;
  (శూన్య)sockindex;
  *pnread = 0;
  ఒకవేళ(!li || !lr)
    తిరిగి CURLE_FAILED_INIT;

  rc = ldap_ఫలితం(li->ld, lr->msgid, LDAP_సందేశం_ONE, &tv, &సందేశం);
  ఒకవేళ(rc < 0) {
    failf(దత్తాంశం, "LDAP local: search ldap_result %s", ldap_err2string(rc));
    ఫలితం = CURLE_RECV_ERROR;
  }

  /* error or timed out */
  ఒకవేళ(!సందేశం)
    తిరిగి ఫలితం;

  ఫలితం = CURLE_OK;

  ఎంపిక(ldap_msgtype(సందేశం)) {
  స్థితి LDAP_RES_శోధన_ఫలితం:
    lr->msgid = 0;
    rc = ldap_parse_ఫలితం(li->ld, సందేశం, &సంకేతం_2, ఖాళీ, &info, ఖాళీ, ఖాళీ, 0);
    ఒకవేళ(rc) {
      failf(దత్తాంశం, "LDAP local: search ldap_parse_result %s",
            ldap_err2string(rc));
      ఫలితం = CURLE_LDAP_శోధన_FAILED;
      ఆపు;
    }

    /* store the latest code for later retrieval */
    దత్తాంశం->info.httpcode = సంకేతం_2;

    ఎంపిక(సంకేతం_2) {
    స్థితి LDAP_SIZELIMIT_EXCEEDED:
      infof(దత్తాంశం, "There are more than %d entries", lr->nument);
      FALLTHROUGH();
    స్థితి LDAP_SUCCESS:
      దత్తాంశం->req.కొలత = దత్తాంశం->req.bytecount;
      ఆపు;
    అప్రమేయ:
      failf(దత్తాంశం, "LDAP remote: search failed %s %s", ldap_err2string(సంకేతం_2),
            info ? info : "");
      ఫలితం = CURLE_LDAP_శోధన_FAILED;
      ఆపు;
    }
    ఒకవేళ(info)
      ldap_memfree(info);
    ఆపు;
  స్థితి LDAP_RES_శోధన_నమోదు:
    lr->nument++;
    rc = ldap_get_dn_ber(li->ld, సందేశం, &ber, &bv);
    ఒకవేళ(rc < 0) {
      ఫలితం = CURLE_RECV_ERROR;
      ఆపు;
    }

    ఒకవేళ(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
      అక్షరం *dn_b64 = ఖాళీ;
      size_t dn_b64_sz = 0;

      /* A DN with e.g. CR/LF would inject LDIF lines, so base64-encode it. */
      ఫలితం = curlx_base64_encode((uint8_t *)bv.bv_val, bv.bv_len,
                                   &dn_b64, &dn_b64_sz);
      ఒకవేళ(!ఫలితం)
        ఫలితం = client_write(దత్తాంశం, STRCONST("DN:: "), dn_b64, dn_b64_sz,
                              STRCONST("\n"));
      curlx_free(dn_b64);
    }
    లేకపోతే
      ఫలితం = client_write(దత్తాంశం, STRCONST("DN: "), bv.bv_val, bv.bv_len,
                            STRCONST("\n"));
    ఒకవేళ(ఫలితం)
      ఆపు;

    కొరకు(rc = ldap_get_attribute_ber(li->ld, సందేశం, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, సందేశం, ber, &bv, &bvals)) {
      పూర్ణాంకం i;
      bool ద్విమాన;

      ఒకవేళ(!bv.bv_val)
        ఆపు;

      /* An attribute type has no base64 form in LDIF, so a type carrying
         control bytes (CR/LF etc.) would let a hostile server inject lines.
         A conformant type name never contains them, so reject it. */
      ఒకవేళ(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
        failf(దత్తాంశం, "LDAP local: attribute type contains illegal bytes");
        ఒకవేళ(bvals)
          ber_memfree(bvals);
        ఫలితం = CURLE_WEIRD_SERVER_జవాబు;
        ఆపు;
      }

      ఒకవేళ(!bvals) {
        ఫలితం = client_write(దత్తాంశం, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":\n"));
        ఒకవేళ(ఫలితం)
          ఆపు;
        కొనసాగు;
      }

      ద్విమాన = bv.bv_len > 7 &&
        curl_strnequal(bv.bv_val + bv.bv_len - 7, ";binary", 7);

      కొరకు(i = 0; bvals[i].bv_val; i++) {
        bool binval = FALSE;

        ఫలితం = client_write(దత్తాంశం, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":"));
        ఒకవేళ(ఫలితం)
          ఆపు;

        ఒకవేళ(!ద్విమాన)
          binval = oldap_str_needs_base64(bvals[i].bv_val, bvals[i].bv_len);
        ఒకవేళ(ద్విమాన || binval) {
          అక్షరం *val_b64 = ఖాళీ;
          size_t val_b64_sz = 0;

          /* Binary value, encode to base64. */
          ఒకవేళ(bvals[i].bv_len)
            ఫలితం = curlx_base64_encode((uint8_t *)bvals[i].bv_val,
                                         bvals[i].bv_len,
                                         &val_b64, &val_b64_sz);
          ఒకవేళ(!ఫలితం)
            ఫలితం = client_write(దత్తాంశం, STRCONST(": "), val_b64, val_b64_sz,
                                  STRCONST("\n"));
          curlx_free(val_b64);
        }
        లేకపోతే
          ఫలితం = client_write(దత్తాంశం, STRCONST(" "),
                                bvals[i].bv_val, bvals[i].bv_len,
                                STRCONST("\n"));
        ఒకవేళ(ఫలితం)
          ఆపు;
      }

      ber_memfree(bvals);
      bvals = ఖాళీ;
      ఒకవేళ(!ఫలితం)
        ఫలితం = client_write(దత్తాంశం, STRCONST("\n"), ఖాళీ, 0, ఖాళీ, 0);
      ఒకవేళ(ఫలితం)
        ఆపు;
    }

    ఒకవేళ(!ఫలితం)
      ఫలితం = client_write(దత్తాంశం, STRCONST("\n"), ఖాళీ, 0, ఖాళీ, 0);
    ఒకవేళ(!ఫలితం)
      ఫలితం = CURLE_AGAIN;
    ఆపు;
  }

  ber_free(ber, 0);
  ldap_msgfree(సందేశం);
  తిరిగి ఫలితం;
}

శూన్య Curl_ldap_సంస్కరణ(అక్షరం *buf, size_t bufsz)
{
  LDAPAPIInfo api;
  api.ldapai_info_సంస్కరణ = LDAP_API_INFO_సంస్కరణ;

  ఒకవేళ(ldap_get_option(ఖాళీ, LDAP_OPT_API_INFO, &api) == LDAP_OPT_SUCCESS) {
    గుర్తులేని పూర్ణాంకం patch = (గుర్తులేని పూర్ణాంకం)(api.ldapai_vendor_సంస్కరణ % 100);
    గుర్తులేని పూర్ణాంకం major = (గుర్తులేని పూర్ణాంకం)(api.ldapai_vendor_సంస్కరణ / 10000);
    గుర్తులేని పూర్ణాంకం minor =
      (((గుర్తులేని పూర్ణాంకం)api.ldapai_vendor_సంస్కరణ - (major * 10000))
       - patch) / 100;
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u",
                   api.ldapai_vendor_name, major, minor, patch);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((శూన్య **)api.ldapai_extensions);
  }
  లేకపోతే
    curl_msnprintf(buf, bufsz, "OpenLDAP");
}

/*
 * LDAP protocol handler.
 */
స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_ldap = {
  oldap_అమరిక_connection,               /* setup_connection */
  oldap_do,                             /* do_it */
  oldap_పూర్తి,                           /* done */
  ZERO_NULL,                            /* do_more */
  oldap_connect,                        /* connect_it */
  oldap_connecting,                     /* connecting */
  ZERO_NULL,                            /* doing */
  ZERO_NULL,                            /* proto_pollset */
  ZERO_NULL,                            /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  ZERO_NULL,                            /* perform_pollset */
  oldap_disconnect,                     /* disconnect */
  ZERO_NULL,                            /* write_resp */
  ZERO_NULL,                            /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ZERO_NULL,                            /* attach connection */
  ZERO_NULL,                            /* follow */
};

#అయితే_ముగింపు /* !CURL_DISABLE_LDAP && USE_OPENLDAP */

/* The LDAP scheme structs are in ldap.c */
