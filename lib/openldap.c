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
#अन्तर्भाव "curl_setup.h"

#यदि !defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP)

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

#अन्तर्भाव <ldap.h>

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "curl_ldap.h"
#अन्तर्भाव "curlx/base64.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "curl_sasl.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "bufref.h"

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
प्रकारनिर्देश गणना {
  OLDAP_STOP,           /* Do nothing state, stops the state machine */
  OLDAP_SSL,            /* Performing SSL handshake. */
  OLDAP_STARTTLS,       /* STARTTLS request sent. */
  OLDAP_TLS,            /* Performing TLS handshake. */
  OLDAP_MECHS,          /* Get SASL authentication mechanisms. */
  OLDAP_SASL,           /* SASL binding reply. */
  OLDAP_BIND,           /* Simple bind reply. */
  OLDAP_BINDV2,         /* Simple bind reply in protocol version 2. */
  OLDAP_चरमः            /* Never used */
} ldapstate;

#यदि_अपरिभाषितम् _LDAP_PVT_H
बाह्य पूर्णाङ्क ldap_pvt_url_scheme2proto(ध्रुव वर्ण *);
बाह्य पूर्णाङ्क ldap_init_fd(ber_socket_t fd, पूर्णाङ्क proto, ध्रुव वर्ण *url,
                        LDAP **ld);
#यदि_अन्तः

स्थिर Curl_recv oldap_recv;

संरचना ldapconninfo {
  संरचना SASL sasl;          /* SASL-related parameters */
  LDAP *ld;                  /* Openldap connection handle. */
  Curl_recv *recv;           /* For stacking SSL handler */
  Curl_send *send;
  संरचना berval *servercred; /* SASL data from server. */
  ldapstate state;           /* Current machine state. */
  curl_socket_t dupfd;       /* duplicated socket */
  पूर्णाङ्क proto;                 /* LDAP_PROTO_TCP/LDAP_PROTO_UDP/LDAP_PROTO_IPC */
  पूर्णाङ्क msgid;                 /* Current message id. */
};

संरचना ldapreqinfo {
  पूर्णाङ्क msgid;
  पूर्णाङ्क nument;
};

/* meta key for storing ldapconninfo at easy handle */
#परिभाषय CURL_META_LDAP_EASY   "meta:proto:ldap:easy"
/* meta key for storing ldapconninfo at connection */
#परिभाषय CURL_META_LDAP_CONN   "meta:proto:ldap:conn"

/*
 * oldap_state()
 *
 * This is the ONLY way to change LDAP state!
 */
स्थिर शून्य oldap_state(संरचना Curl_easy *दत्त, संरचना ldapconninfo *li,
                        ldapstate newstate)
{
#यदि defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  स्थिर ध्रुव वर्ण * ध्रुव नामानि[] = {
    "STOP",
    "SSL",
    "STARTTLS",
    "TLS",
    "MECHS",
    "SASL",
    "BIND",
    "BINDV2",
    /*चरमः*/
  };

  यदि(li->state != newstate)
    infof(दत्त, "LDAP %p state change from %s to %s",
          (शून्य *)li, नामानि[li->state], नामानि[newstate]);
#अन्यथा
  (शून्य)दत्त;
#यदि_अन्तः
  li->state = newstate;
}

/* Map some particular LDAP error codes to CURLcode values. */
स्थिर CURLcode oldap_map_error(पूर्णाङ्क rc, CURLcode परिणाम)
{
  विकल्प(rc) {
  पक्ष LDAP_NO_स्मृतिः:
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  पक्ष LDAP_INVALID_CREDENTIALS:
    प्रत्यागम CURLE_LOGIN_DENIED;
  पक्ष LDAP_PROTOCOL_ERROR:
    प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
  पक्ष LDAP_INSUFFICIENT_ACCESS:
    प्रत्यागम CURLE_REMOTE_ACCESS_DENIED;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode oldap_url_parse(संरचना Curl_easy *दत्त, LDAPURLDesc **ludp)
{
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क rc;
  स्थिर ध्रुव वर्ण * ध्रुव url_errs[] = {
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

  *ludp = रिक्त;
  /* `ldap_url_parse() seems to be terrible with urls
   * that have user/pass/options in it. So when we have options or
   * creds from the url, fail without calling the function.
   * Yes, this is super-weird code and I do not like it. */
  यदि((दत्त->state.creds && (दत्त->state.creds->source == CREDS_URL)) ||
     दत्त->state.ऊर्ध्वम्.options)
    rc = LDAP_URL_ERR_BADURL;
  अन्यथा
    rc = ldap_url_parse(Curl_bufref_ptr(&दत्त->state.url), ludp);

  यदि(rc != LDAP_URL_SUCCESS) {
    ध्रुव वर्ण *सन्देशः = "url parsing problem";

    परिणाम = rc == LDAP_URL_ERR_MEM ? CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः :
      CURLE_URL_MALFORMAT;
    rc -= LDAP_URL_SUCCESS;
    यदि((size_t)rc < CURL_ARRAYSIZE(url_errs))
      सन्देशः = url_errs[rc];
    failf(दत्त, "LDAP local: %s", सन्देशः);
  }
  प्रत्यागम परिणाम;
}

/* Parse the login options. */
स्थिर CURLcode oldap_parse_login_options(संरचना connectdata *conn)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ध्रुव वर्ण *ptr = conn->options;

  DEBUGASSERT(li);
  यदि(!li)
    प्रत्यागम CURLE_FAILED_INIT;

  यावत्(!परिणाम && ptr && *ptr) {
    ध्रुव वर्ण *कुञ्चिका = ptr;
    ध्रुव वर्ण *मानम्;

    यावत्(*ptr && *ptr != '=')
      ptr++;

    मानम् = ptr + 1;

    यावत्(*ptr && *ptr != ';')
      ptr++;

    यदि(checkprefix("AUTH=", कुञ्चिका))
      परिणाम = Curl_sasl_parse_url_auth_option(&li->sasl, मानम्, ptr - मानम्);
    अन्यथा
      परिणाम = CURLE_SETOPT_OPTION_SYNTAX;

    यदि(*ptr == ';')
      ptr++;
  }

  प्रत्यागम परिणाम == CURLE_URL_MALFORMAT ? CURLE_SETOPT_OPTION_SYNTAX : परिणाम;
}

स्थिर CURLcode oldap_सज्जता_connection(संरचना Curl_easy *दत्त,
                                       संरचना connectdata *conn)
{
  CURLcode परिणाम;
  LDAPURLDesc *lud;
  (शून्य)conn;

  /* Early URL syntax check. */
  परिणाम = oldap_url_parse(दत्त, &lud);
  ldap_free_urldesc(lud);

  प्रत्यागम परिणाम;
}

/*
 * Get the SASL authentication challenge from the server credential buffer.
 */
स्थिर CURLcode oldap_get_सन्देशः(संरचना Curl_easy *दत्त, संरचना bufref *बहिर्गमनम्)
{
  संरचना ldapconninfo *li =
    Curl_conn_meta_get(दत्त->conn, CURL_META_LDAP_CONN);
  संरचना berval *servercred = li ? li->servercred : रिक्त;
  DEBUGASSERT(li);
  यदि(!li)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(!servercred || !servercred->bv_val)
    प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  Curl_bufref_समुच्चयः(बहिर्गमनम्, servercred->bv_val, servercred->bv_len, रिक्त);
  प्रत्यागम CURLE_OK;
}

/*
 * Sends an initial SASL bind request to the server.
 */
स्थिर CURLcode oldap_perform_auth(संरचना Curl_easy *दत्त, ध्रुव वर्ण *mech,
                                   ध्रुव संरचना bufref *initresp)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  संरचना berval cred;
  संरचना berval *pcred = &cred;
  पूर्णाङ्क rc;

  DEBUGASSERT(li);
  यदि(!li)
    प्रत्यागम CURLE_FAILED_INIT;
  cred.bv_val = (वर्ण *)CURL_UNCONST(Curl_bufref_ptr(initresp));
  cred.bv_len = Curl_bufref_len(initresp);
  यदि(!cred.bv_val)
    pcred = रिक्त;
  rc = ldap_sasl_bind(li->ld, रिक्त, mech, pcred, रिक्त, रिक्त, &li->msgid);
  यदि(rc != LDAP_SUCCESS)
    प्रत्यागम oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  प्रत्यागम CURLE_OK;
}

/*
 * Sends SASL continuation.
 */
स्थिर CURLcode oldap_continue_auth(संरचना Curl_easy *दत्त, ध्रुव वर्ण *mech,
                                    ध्रुव संरचना bufref *resp)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  संरचना berval cred;
  संरचना berval *pcred = &cred;
  पूर्णाङ्क rc;

  यदि(!li)
    प्रत्यागम CURLE_FAILED_INIT;
  cred.bv_val = (वर्ण *)CURL_UNCONST(Curl_bufref_ptr(resp));
  cred.bv_len = Curl_bufref_len(resp);
  यदि(!cred.bv_val)
    pcred = रिक्त;
  rc = ldap_sasl_bind(li->ld, रिक्त, mech, pcred, रिक्त, रिक्त, &li->msgid);
  यदि(rc != LDAP_SUCCESS)
    प्रत्यागम oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  प्रत्यागम CURLE_OK;
}

/*
 * Sends SASL bind cancellation.
 */
स्थिर CURLcode oldap_cancel_auth(संरचना Curl_easy *दत्त, ध्रुव वर्ण *mech)
{
  संरचना ldapconninfo *li =
    Curl_conn_meta_get(दत्त->conn, CURL_META_LDAP_CONN);
  पूर्णाङ्क rc;

  (शून्य)mech;
  यदि(!li)
    प्रत्यागम CURLE_FAILED_INIT;
  rc = ldap_sasl_bind(li->ld, रिक्त, LDAP_SASL_NULL, रिक्त, रिक्त, रिक्त,
                      &li->msgid);
  यदि(rc != LDAP_SUCCESS)
    प्रत्यागम oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  प्रत्यागम CURLE_OK;
}

/* Starts LDAP simple bind. */
स्थिर CURLcode oldap_perform_bind(संरचना Curl_easy *दत्त, ldapstate newstate)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ध्रुव वर्ण *binddn = रिक्त;
  संरचना berval passwd;
  पूर्णाङ्क rc;

  यदि(!li)
    प्रत्यागम CURLE_FAILED_INIT;
  passwd.bv_val = रिक्त;
  passwd.bv_len = 0;

  यदि(conn->creds) {
    binddn = Curl_creds_user(conn->creds);
    passwd.bv_val = CURL_UNCONST(Curl_creds_passwd(conn->creds));
    passwd.bv_len = वाक्यदैर्घ्यम्(passwd.bv_val);
  }

  rc = ldap_sasl_bind(li->ld, binddn, LDAP_SASL_SIMPLE, &passwd,
                      रिक्त, रिक्त, &li->msgid);
  यदि(rc != LDAP_SUCCESS)
    प्रत्यागम oldap_map_error(rc,
                           दत्त->state.creds ?
                           CURLE_LOGIN_DENIED : CURLE_LDAP_CANNOT_BIND);
  oldap_state(दत्त, li, newstate);
  प्रत्यागम CURLE_OK;
}

/* Query the supported SASL authentication mechanisms. */
स्थिर CURLcode oldap_perform_mechs(संरचना Curl_easy *दत्त)
{
  संरचना ldapconninfo *li =
    Curl_conn_meta_get(दत्त->conn, CURL_META_LDAP_CONN);
  पूर्णाङ्क rc;
  स्थिर ध्रुव वर्ण * ध्रुव supportedSASLMechanisms[] = {
    "supportedSASLMechanisms",
    रिक्त
  };

  यदि(!li)
    प्रत्यागम CURLE_FAILED_INIT;
  /* Casting away the const for the 3rd parameter that the LDAP API expects as
     a non-const char ** is potentially unsafe but we believe the lack of
     const in the API was an oversight and that no LDAP implementation
     actually modifies the input. */
  rc = ldap_अन्वेषणम्_ext(li->ld, "", LDAP_SCOPE_आधारः, "(objectclass=*)",
                       (वर्ण **)CURL_UNCONST(supportedSASLMechanisms), 0,
                       रिक्त, रिक्त, रिक्त, 0, &li->msgid);
  यदि(rc != LDAP_SUCCESS)
    प्रत्यागम oldap_map_error(rc, CURLE_LOGIN_DENIED);
  oldap_state(दत्त, li, OLDAP_MECHS);
  प्रत्यागम CURLE_OK;
}

/* Starts SASL bind. */
स्थिर CURLcode oldap_perform_sasl(संरचना Curl_easy *दत्त)
{
  संरचना ldapconninfo *li =
    Curl_conn_meta_get(दत्त->conn, CURL_META_LDAP_CONN);
  saslprogress प्रगतिः = SASL_IDLE;
  CURLcode परिणाम;

  यदि(!li)
    प्रत्यागम CURLE_FAILED_INIT;
  परिणाम = Curl_sasl_start(&li->sasl, दत्त, TRUE, &प्रगतिः);

  oldap_state(दत्त, li, OLDAP_SASL);
  यदि(!परिणाम && प्रगतिः != SASL_INPROGRESS)
    परिणाम = Curl_sasl_is_blocked(&li->sasl, दत्त);
  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् USE_SSL
स्थिर पूर्णाङ्क ldapsb_tls_सज्जता(Sockbuf_IO_Desc *sbiod, शून्य *arg)
{
  sbiod->sbiod_pvt = arg;
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क ldapsb_tls_remove(Sockbuf_IO_Desc *sbiod)
{
  sbiod->sbiod_pvt = रिक्त;
  प्रत्यागम 0;
}

/* if there is a duplicated socket to close, close it */
स्थिर पूर्णाङ्क ldapsb_tls_close(Sockbuf_IO_Desc *sbiod)
{
  संरचना Curl_easy *दत्त = sbiod->sbiod_pvt;
  यदि(दत्त) {
    संरचना connectdata *conn = दत्त->conn;
    यदि(conn) {
      संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      यदि(li && (li->dupfd != CURL_SOCKET_BAD)) {
        sclose(li->dupfd);
        li->dupfd = CURL_SOCKET_BAD;
      }
    }
  }

  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क ldapsb_tls_ctrl(Sockbuf_IO_Desc *sbiod, पूर्णाङ्क opt, शून्य *arg)
{
  (शून्य)arg;
  यदि(opt == LBER_SB_OPT_दत्त_READY) {
    संरचना Curl_easy *दत्त = sbiod->sbiod_pvt;
    प्रत्यागम Curl_conn_दत्त_pending(दत्त, FIRSTSOCKET);
  }
  प्रत्यागम 0;
}

स्थिर ber_slen_t ldapsb_tls_read(Sockbuf_IO_Desc *sbiod, शून्य *buf,
                                  ber_len_t len)
{
  संरचना Curl_easy *दत्त = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  यदि(दत्त) {
    संरचना connectdata *conn = दत्त->conn;
    यदि(conn) {
      संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode परिणाम = CURLE_RECV_ERROR;
      size_t nread;

      यदि(!li) {
        समुच्चयः_SOCKERRNO(SOCKEINVAL);
        प्रत्यागम -1;
      }
      परिणाम = (li->recv)(दत्त, FIRSTSOCKET, buf, len, &nread);
      यदि(परिणाम == CURLE_AGAIN) {
        समुच्चयः_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = परिणाम ? -1 : (ber_slen_t)nread;
    }
  }
  प्रत्यागम ret;
}

स्थिर ber_slen_t ldapsb_tls_write(Sockbuf_IO_Desc *sbiod, शून्य *buf,
                                   ber_len_t len)
{
  संरचना Curl_easy *दत्त = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  यदि(दत्त) {
    संरचना connectdata *conn = दत्त->conn;
    यदि(conn) {
      संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode परिणाम = CURLE_SEND_ERROR;
      size_t nwritten;

      यदि(!li) {
        समुच्चयः_SOCKERRNO(SOCKEINVAL);
        प्रत्यागम -1;
      }
      परिणाम = (li->send)(दत्त, FIRSTSOCKET, buf, len, FALSE, &nwritten);
      यदि(परिणाम == CURLE_AGAIN) {
        समुच्चयः_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = परिणाम ? -1 : (ber_slen_t)nwritten;
    }
  }
  प्रत्यागम ret;
}

स्थिर Sockbuf_IO ldapsb_tls = {
  ldapsb_tls_सज्जता,
  ldapsb_tls_remove,
  ldapsb_tls_ctrl,
  ldapsb_tls_read,
  ldapsb_tls_write,
  ldapsb_tls_close
};

स्थिर bool ssl_installed(संरचना connectdata *conn)
{
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  प्रत्यागम li && li->recv;
}

स्थिर CURLcode oldap_ssl_connect(संरचना Curl_easy *दत्त, ldapstate newstate)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  bool ssldone = FALSE;
  CURLcode परिणाम;

  यदि(!li)
    प्रत्यागम CURLE_FAILED_INIT;
  परिणाम = Curl_conn_connect(दत्त, FIRSTSOCKET, FALSE, &ssldone);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  oldap_state(दत्त, li, newstate);

  यदि(ssldone) {
    Sockbuf *sb;

    /* Install the libcurl SSL handlers into the sockbuf. */
    यदि((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_सङ्कलय_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, दत्त))
      प्रत्यागम CURLE_FAILED_INIT;
    li->recv = conn->recv[FIRSTSOCKET];
    li->send = conn->send[FIRSTSOCKET];
  }

  प्रत्यागम परिणाम;
}

/* Send the STARTTLS request */
स्थिर CURLcode oldap_perform_starttls(संरचना Curl_easy *दत्त)
{
  संरचना ldapconninfo *li =
    Curl_conn_meta_get(दत्त->conn, CURL_META_LDAP_CONN);
  पूर्णाङ्क rc;

  यदि(!li)
    प्रत्यागम CURLE_FAILED_INIT;
  rc = ldap_start_tls(li->ld, रिक्त, रिक्त, &li->msgid);
  यदि(rc != LDAP_SUCCESS)
    प्रत्यागम oldap_map_error(rc, CURLE_USE_SSL_FAILED);
  oldap_state(दत्त, li, OLDAP_STARTTLS);
  प्रत्यागम CURLE_OK;
}
#यदि_अन्तः

स्थिर शून्य oldap_easy_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना ldapreqinfo *lr = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  curlx_free(lr);
}

स्थिर शून्य oldap_conn_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना ldapconninfo *li = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  यदि(li->ld) {
    ldap_unbind_ext(li->ld, रिक्त, रिक्त);
    li->ld = रिक्त;
  }
  curlx_free(li);
}

/* SASL parameters for the ldap protocol */
स्थिर ध्रुव संरचना SASLproto saslldap = {
  "ldap",                     /* The service name */
  oldap_perform_auth,         /* Send authentication command */
  oldap_continue_auth,        /* Send authentication continuation */
  oldap_cancel_auth,          /* Send authentication cancellation */
  oldap_get_सन्देशः,          /* Get SASL response message */
  0,                          /* Maximum initial response length (no max) */
  LDAP_SASL_BIND_अन्तर्_प्रगतिः, /* Code received when continuation is expected */
  LDAP_SUCCESS,               /* Code to receive upon authentication success */
  SASL_AUTH_NONE,             /* Default mechanisms */
  0                           /* Configuration flags */
};

स्थिर CURLcode oldap_connect(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ldapconninfo *li;
  स्थिर ध्रुव पूर्णाङ्क संस्करणम्_2 = LDAP_VERSION3;
  वर्ण *hosturl = रिक्त;
  CURLcode परिणाम;
  पूर्णाङ्क rc;
#यदि_परिभाषितम् CURL_OPENLDAP_DEBUG
  स्थिर पूर्णाङ्क do_trace = -1;
#यदि_अन्तः

  (शून्य)समाप्तम्;

  li = curlx_calloc(1, परिमाण(संरचना ldapconninfo));
  यदि(!li) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  li->dupfd = CURL_SOCKET_BAD;

  परिणाम = Curl_conn_meta_समुच्चयः(conn, CURL_META_LDAP_CONN, li, oldap_conn_dtor);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  li->proto = ldap_pvt_url_scheme2proto(दत्त->state.origin->scheme->name);

  /* Initialize the SASL storage */
  Curl_sasl_init(&li->sasl, दत्त, &saslldap);

  परिणाम = oldap_parse_login_options(conn);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  hosturl = curl_maprintf("%s://%s:%u",
                          conn->scheme->name,
                          conn->origin->ipv6 ?
                          conn->origin->user_hostname :
                          conn->origin->hostname,
                          conn->origin->पत्तनम्);
  यदि(!hosturl) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  {
#यदि_परिभाषितम् _WIN32
    SOCKET dupfd = INVALID_SOCKET;
    WSAPROTOCOL_INFO pi;
    यदि(WSADuplicateSocket(conn->sock[FIRSTSOCKET], GetCurrentProcessId(),
                          &pi))
      ; /* error */
    अन्यथा
      dupfd = CURL_SOCKET(pi.i_स्थानाङ्कः_Family, pi.iSocketType, pi.iProtocol);
#अन्यथा
#यदि_परिभाषितम् F_DUPFD_CLOEXEC
    पूर्णाङ्क dupfd = fcntl(conn->sock[FIRSTSOCKET], F_DUPFD_CLOEXEC, 0);
#अन्यथा
    पूर्णाङ्क dupfd = dup(conn->sock[FIRSTSOCKET]);
#यदि_परिभाषितम् HAVE_FCNTL
    यदि((dupfd != CURL_SOCKET_BAD) &&
       (fcntl(dupfd, F_SETFD, FD_CLOEXEC) < 0)) {
      sclose(dupfd);
      dupfd = CURL_SOCKET_BAD;
    }
#यदि_अन्तः /* HAVE_FCNTL */
#यदि_अन्तः /* F_DUPFD_CLOEXEC */
#यदि_अन्तः /* _WIN32 */

    यदि(dupfd == CURL_SOCKET_BAD) {
      परिणाम = CURLE_COULDNT_CONNECT;
      गच्छ बहिर्गमनम्;
    }
    rc = ldap_init_fd((ber_socket_t)dupfd, li->proto, hosturl, &li->ld);
    यदि(rc)
      sclose(dupfd);
    अन्यथा
      /* store the socket, as when this is used by TLS we close this ourselves
         and at this point the code does not yet know if this will use TLS or
         not */
      li->dupfd = dupfd;
  }
  यदि(rc) {
    failf(दत्त, "LDAP local: Cannot connect to %s, %s",
          hosturl, ldap_err2string(rc));
    परिणाम = CURLE_COULDNT_CONNECT;
    गच्छ बहिर्गमनम्;
  }

#यदि_परिभाषितम् CURL_OPENLDAP_DEBUG
  यदि(do_trace < 0) {
    ध्रुव वर्ण *env = getenv("CURL_OPENLDAP_TRACE");
    curl_off_t e = 0;
    यदि(!curlx_str_सङ्ख्या(&env, &e, अधिकतम_पूर्णाङ्कः))
      do_trace = e > 0;
  }
  यदि(do_trace)
    ldap_समुच्चयः_option(li->ld, LDAP_OPT_DEBUG_LEVEL, &do_trace);
#यदि_अन्तः

  /* Try version 3 first. */
  ldap_समुच्चयः_option(li->ld, LDAP_OPT_PROTOCOL_संस्करणम्, &संस्करणम्_2);

  /* Do not chase referrals. */
  ldap_समुच्चयः_option(li->ld, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  {
    ber_len_t max = 256 * 1024;
    Sockbuf *sb;
    यदि((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       /* Set the maximum allowed size of an incoming message, which to
          OpenLDAP means that it will malloc() memory up to this size. If not
          set, there is no limit and we instead risk a malloc() failure. */
       !ber_sockbuf_ctrl(sb, LBER_SB_OPT_समुच्चयः_MAX_INCOMING, &max)) {
      परिणाम = CURLE_FAILED_INIT;
      गच्छ बहिर्गमनम्;
    }
  }

#यदि_परिभाषितम् USE_SSL
  यदि(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    परिणाम = oldap_ssl_connect(दत्त, OLDAP_SSL);
    गच्छ बहिर्गमनम्;
  }

  यदि(दत्त->समुच्चयः.use_ssl) {
    परिणाम = oldap_perform_starttls(दत्त);
    यदि(!परिणाम || दत्त->समुच्चयः.use_ssl != CURLUSESSL_TRY)
      गच्छ बहिर्गमनम्;
  }
#यदि_अन्तः

  यदि(li->sasl.prefmech != SASL_AUTH_NONE) {
    परिणाम = oldap_perform_mechs(दत्त);
    गच्छ बहिर्गमनम्;
  }

  /* Force bind even if anonymous bind is not needed in protocol version 3
     to detect missing version 3 support. */
  परिणाम = oldap_perform_bind(दत्त, OLDAP_BIND);

बहिर्गमनम्:
  curlx_free(hosturl);
  प्रत्यागम परिणाम;
}

/* Handle the supported SASL mechanisms query response */
स्थिर CURLcode oldap_state_mechs_resp(संरचना Curl_easy *दत्त,
                                       LDAPMessage *सन्देशः, पूर्णाङ्क संहिता)
{
  संरचना connectdata *conn;
  संरचना ldapconninfo *li;
  पूर्णाङ्क rc;
  BerElement *ber = रिक्त;
  CURLcode परिणाम = CURLE_OK;
  संरचना berval bv, *bvals;

  यदि(!दत्त)
    प्रत्यागम CURLE_FAILED_INIT;

  conn = दत्त->conn;
  li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);

  यदि(!li)
    प्रत्यागम CURLE_FAILED_INIT;
  विकल्प(ldap_msgtype(सन्देशः)) {
  पक्ष LDAP_RES_अन्वेषणम्_प्रविष्टिः:
    /* Got a list of supported SASL mechanisms. */
    यदि(संहिता != LDAP_SUCCESS && संहिता != LDAP_NO_RESULTS_RETURNED)
      प्रत्यागम CURLE_LOGIN_DENIED;

    rc = ldap_get_dn_ber(li->ld, सन्देशः, &ber, &bv);
    यदि(rc < 0)
      प्रत्यागम oldap_map_error(rc, CURLE_BAD_CONTENT_ENCODING);
    प्रति(rc = ldap_get_attribute_ber(li->ld, सन्देशः, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, सन्देशः, ber, &bv, &bvals)) {
      पूर्णाङ्क i;

      यदि(!bv.bv_val)
        भङ्ग;

      यदि(bvals) {
        प्रति(i = 0; bvals[i].bv_val; i++) {
          size_t llen;
          अचिह्नित ह्रस्व mech =
            Curl_sasl_decode_mech((ध्रुव वर्ण *)bvals[i].bv_val,
                                  bvals[i].bv_len, &llen);
          यदि(bvals[i].bv_len == llen)
            li->sasl.authmechs |= mech;
        }
        ber_memfree(bvals);
      }
    }
    ber_free(ber, 0);
    भङ्ग;

  पक्ष LDAP_RES_अन्वेषणम्_परिणाम:
    विकल्प(संहिता) {
    पक्ष LDAP_SIZELIMIT_EXCEEDED:
      infof(दत्त, "Too many authentication mechanisms");
      FALLTHROUGH();
    पक्ष LDAP_SUCCESS:
    पक्ष LDAP_NO_RESULTS_RETURNED:
      यदि(Curl_sasl_can_authenticate(&li->sasl, दत्त))
        परिणाम = oldap_perform_sasl(दत्त);
      अन्यथा
        परिणाम = CURLE_LOGIN_DENIED;
      भङ्ग;
    स्वभाव:
      परिणाम = oldap_map_error(संहिता, CURLE_LOGIN_DENIED);
      भङ्ग;
    }
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम परिणाम;
}

/* Handle a SASL bind response. */
स्थिर CURLcode oldap_state_sasl_resp(संरचना Curl_easy *दत्त,
                                      LDAPMessage *सन्देशः, पूर्णाङ्क संहिता)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode परिणाम = CURLE_OK;
  saslprogress प्रगतिः;
  पूर्णाङ्क rc;

  यदि(!li)
    प्रत्यागम CURLE_FAILED_INIT;
  li->servercred = रिक्त;
  rc = ldap_parse_sasl_bind_परिणाम(li->ld, सन्देशः, &li->servercred, 0);
  यदि(rc != LDAP_SUCCESS) {
    failf(दत्त, "LDAP local: sasl ldap_parse_result %s", ldap_err2string(rc));
    परिणाम = oldap_map_error(rc, CURLE_LOGIN_DENIED);
  }
  अन्यथा {
    परिणाम = Curl_sasl_continue(&li->sasl, दत्त, संहिता, &प्रगतिः);
    यदि(!परिणाम) {
      विकल्प(प्रगतिः) {
      पक्ष SASL_समाप्तम्:
        oldap_state(दत्त, li, OLDAP_STOP);   /* Authenticated */
        भङ्ग;
      पक्ष SASL_IDLE:            /* No mechanism left after cancellation */
        failf(दत्त, "Authentication cancelled");
        परिणाम = CURLE_LOGIN_DENIED;
        भङ्ग;
      स्वभाव:
        भङ्ग;
      }
    }
  }

  यदि(li->servercred)
    ber_bvfree(li->servercred);
  प्रत्यागम परिणाम;
}

/* Handle a simple bind response. */
स्थिर CURLcode oldap_state_bind_resp(संरचना Curl_easy *दत्त, LDAPMessage *सन्देशः,
                                      पूर्णाङ्क संहिता)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode परिणाम = CURLE_OK;
  संरचना berval *bv = रिक्त;
  पूर्णाङ्क rc;

  यदि(!li)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(संहिता != LDAP_SUCCESS)
    प्रत्यागम oldap_map_error(संहिता, CURLE_LDAP_CANNOT_BIND);

  rc = ldap_parse_sasl_bind_परिणाम(li->ld, सन्देशः, &bv, 0);
  यदि(rc != LDAP_SUCCESS) {
    failf(दत्त, "LDAP local: bind ldap_parse_sasl_bind_result %s",
          ldap_err2string(rc));
    परिणाम = oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  }
  अन्यथा
    oldap_state(दत्त, li, OLDAP_STOP);

  यदि(bv)
    ber_bvfree(bv);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode oldap_connecting(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  LDAPMessage *सन्देशः = रिक्त;
  संरचना timeval tv = { 0, 0 };
  पूर्णाङ्क संहिता = LDAP_SUCCESS;
  पूर्णाङ्क rc;

  यदि(!li)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(li->state != OLDAP_SSL && li->state != OLDAP_TLS) {
    /* Get response to last command. */
    rc = ldap_परिणाम(li->ld, li->msgid, LDAP_सन्देशः_ONE, &tv, &सन्देशः);
    विकल्प(rc) {
    पक्ष 0:                               /* Timed out. */
      प्रत्यागम CURLE_OK;
    पक्ष LDAP_RES_अन्वेषणम्_प्रविष्टिः:
    पक्ष LDAP_RES_अन्वेषणम्_REFERENCE:
      भङ्ग;
    स्वभाव:
      li->msgid = 0;                      /* Nothing to abandon upon error. */
      यदि(rc < 0) {
        failf(दत्त, "LDAP local: connecting ldap_result %s",
              ldap_err2string(rc));
        प्रत्यागम oldap_map_error(rc, CURLE_COULDNT_CONNECT);
      }
      भङ्ग;
    }

    /* Get error code from message. */
    rc = ldap_parse_परिणाम(li->ld, सन्देशः, &संहिता, रिक्त, रिक्त, रिक्त, रिक्त, 0);
    यदि(rc)
      संहिता = rc;
    अन्यथा {
      /* store the latest code for later retrieval */
      दत्त->info.httpcode = संहिता;
    }

    /* If protocol version 3 is not supported, fallback to version 2. */
    यदि(संहिता == LDAP_PROTOCOL_ERROR && li->state != OLDAP_BINDV2 &&
#यदि_परिभाषितम् USE_SSL
       (ssl_installed(conn) || दत्त->समुच्चयः.use_ssl <= CURLUSESSL_TRY) &&
#यदि_अन्तः
       li->sasl.prefmech == SASL_AUTH_NONE) {
      स्थिर ध्रुव पूर्णाङ्क संस्करणम्_2 = LDAP_VERSION2;

      ldap_समुच्चयः_option(li->ld, LDAP_OPT_PROTOCOL_संस्करणम्, &संस्करणम्_2);
      ldap_msgfree(सन्देशः);
      प्रत्यागम oldap_perform_bind(दत्त, OLDAP_BINDV2);
    }
  }

  /* Handle response message according to current state. */
  विकल्प(li->state) {

#यदि_परिभाषितम् USE_SSL
  पक्ष OLDAP_SSL:
    परिणाम = oldap_ssl_connect(दत्त, OLDAP_SSL);
    यदि(!परिणाम && ssl_installed(conn)) {
      यदि(li->sasl.prefmech != SASL_AUTH_NONE)
        परिणाम = oldap_perform_mechs(दत्त);
      अन्यथा
        परिणाम = oldap_perform_bind(दत्त, OLDAP_BIND);
    }
    भङ्ग;
  पक्ष OLDAP_STARTTLS:
    यदि(संहिता != LDAP_SUCCESS) {
      यदि(दत्त->समुच्चयः.use_ssl != CURLUSESSL_TRY)
        परिणाम = oldap_map_error(संहिता, CURLE_USE_SSL_FAILED);
      अन्यथा यदि(li->sasl.prefmech != SASL_AUTH_NONE)
        परिणाम = oldap_perform_mechs(दत्त);
      अन्यथा
        परिणाम = oldap_perform_bind(दत्त, OLDAP_BIND);
      भङ्ग;
    }
    परिणाम = Curl_ssl_cfilter_सङ्कलय(
      दत्त, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    यदि(परिणाम)
      भङ्ग;
    FALLTHROUGH();
  पक्ष OLDAP_TLS:
    परिणाम = oldap_ssl_connect(दत्त, OLDAP_TLS);
    यदि(परिणाम)
      परिणाम = oldap_map_error(संहिता, CURLE_USE_SSL_FAILED);
    अन्यथा यदि(ssl_installed(conn)) {
      यदि(li->sasl.prefmech != SASL_AUTH_NONE)
        परिणाम = oldap_perform_mechs(दत्त);
      अन्यथा यदि(दत्त->state.creds)
        परिणाम = oldap_perform_bind(दत्त, OLDAP_BIND);
      अन्यथा {
        /* Version 3 supported: no bind required */
        oldap_state(दत्त, li, OLDAP_STOP);
        परिणाम = CURLE_OK;
      }
    }
    भङ्ग;
#यदि_अन्तः

  पक्ष OLDAP_MECHS:
    परिणाम = oldap_state_mechs_resp(दत्त, सन्देशः, संहिता);
    भङ्ग;
  पक्ष OLDAP_SASL:
    परिणाम = oldap_state_sasl_resp(दत्त, सन्देशः, संहिता);
    भङ्ग;
  पक्ष OLDAP_BIND:
  पक्ष OLDAP_BINDV2:
    परिणाम = oldap_state_bind_resp(दत्त, सन्देशः, संहिता);
    भङ्ग;
  स्वभाव:
    /* internal error */
    परिणाम = CURLE_COULDNT_CONNECT;
    भङ्ग;
  }

  ldap_msgfree(सन्देशः);

  *समाप्तम् = li->state == OLDAP_STOP;
  यदि(*समाप्तम्)
    conn->recv[FIRSTSOCKET] = oldap_recv;

  यदि(परिणाम && li->msgid) {
    ldap_abandon_ext(li->ld, li->msgid, रिक्त, रिक्त);
    li->msgid = 0;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode oldap_disconnect(संरचना Curl_easy *दत्त,
                                 संरचना connectdata *conn,
                                 bool dead_connection)
{
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  (शून्य)dead_connection;
#यदि_अपरिभाषितम् USE_SSL
  (शून्य)दत्त;
#यदि_अन्तः

  यदि(li && li->ld) {
#यदि_परिभाषितम् USE_SSL
    यदि(ssl_installed(conn)) {
      Sockbuf *sb;
      यदि(ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS ||
         ber_sockbuf_सङ्कलय_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, दत्त))
        प्रत्यागम CURLE_FAILED_INIT;
    }
#यदि_अन्तः
    ldap_unbind_ext(li->ld, रिक्त, रिक्त);
    li->ld = रिक्त;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode oldap_do(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  संरचना ldapreqinfo *lr;
  CURLcode परिणाम;
  पूर्णाङ्क rc;
  LDAPURLDesc *lud;
  पूर्णाङ्क msgid;

  यदि(!li)
    प्रत्यागम CURLE_FAILED_INIT;

  infof(दत्त, "LDAP local: %s", Curl_bufref_ptr(&दत्त->state.url));

  परिणाम = oldap_url_parse(दत्त, &lud);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

#यदि_परिभाषितम् USE_SSL
  यदि(ssl_installed(conn)) {
    Sockbuf *sb;
    /* re-install the libcurl SSL handlers into the sockbuf. */
    यदि((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_सङ्कलय_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, दत्त)) {
      ldap_free_urldesc(lud);
      प्रत्यागम CURLE_FAILED_INIT;
    }
  }
#यदि_अन्तः

  rc = ldap_अन्वेषणम्_ext(li->ld, lud->lud_dn, lud->lud_scope,
                       lud->lud_filter, lud->lud_attrs, 0,
                       रिक्त, रिक्त, रिक्त, 0, &msgid);
  ldap_free_urldesc(lud);
  यदि(rc != LDAP_SUCCESS) {
    failf(दत्त, "LDAP local: ldap_search_ext %s", ldap_err2string(rc));
    परिणाम = CURLE_LDAP_अन्वेषणम्_FAILED;
    गच्छ बहिर्गमनम्;
  }

  lr = curlx_calloc(1, परिमाण(संरचना ldapreqinfo));
  यदि(!lr ||
     Curl_meta_समुच्चयः(दत्त, CURL_META_LDAP_EASY, lr, oldap_easy_dtor)) {
    ldap_abandon_ext(li->ld, msgid, रिक्त, रिक्त);
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  lr->msgid = msgid;
  Curl_xfer_सज्जता_recv(दत्त, FIRSTSOCKET, -1);
  *समाप्तम् = TRUE;

बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

स्थिर CURLcode oldap_समाप्तम्(संरचना Curl_easy *दत्त, CURLcode res,
                           bool premature)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ldapreqinfo *lr = Curl_meta_get(दत्त, CURL_META_LDAP_EASY);

  (शून्य)res;
  (शून्य)premature;

  यदि(lr) {
    /* if there was a search in progress, abandon it */
    यदि(lr->msgid) {
      संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      यदि(li && li->ld) {
        ldap_abandon_ext(li->ld, lr->msgid, रिक्त, रिक्त);
      }
      lr->msgid = 0;
    }
    Curl_meta_remove(दत्त, CURL_META_LDAP_EASY);
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode client_write(संरचना Curl_easy *दत्त,
                             ध्रुव वर्ण *उपसर्गः, size_t plen,
                             ध्रुव वर्ण *मानम्, size_t len,
                             ध्रुव वर्ण *suffix, size_t slen)
{
  CURLcode परिणाम = CURLE_OK;

  यदि(उपसर्गः) {
    /* If we have a zero-length value and the prefix ends with a space
       separator, drop the latter. */
    यदि(!len && plen && उपसर्गः[plen - 1] == ' ')
      plen--;
    परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, उपसर्गः, plen);
  }
  यदि(!परिणाम && मानम्) {
    परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, मानम्, len);
  }
  यदि(!परिणाम && suffix) {
    परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, suffix, slen);
  }
  प्रत्यागम परिणाम;
}

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
स्थिर bool oldap_str_needs_base64(ध्रुव वर्ण *s, size_t len)
{
  size_t i;
  यदि(!len)
    प्रत्यागम FALSE;
  यदि((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    प्रत्यागम TRUE;
  प्रति(i = 0; i < len; i++)
    यदि(!ISPRINT(s[i]))
      प्रत्यागम TRUE;
  प्रत्यागम FALSE;
}

स्थिर CURLcode oldap_recv(संरचना Curl_easy *दत्त, int8_t sockindex, वर्ण *buf,
                           size_t len, size_t *pnread)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  संरचना ldapreqinfo *lr = Curl_meta_get(दत्त, CURL_META_LDAP_EASY);
  पूर्णाङ्क rc;
  LDAPMessage *सन्देशः = रिक्त;
  BerElement *ber = रिक्त;
  संरचना timeval tv = { 0, 0 };
  संरचना berval bv, *bvals;
  CURLcode परिणाम = CURLE_AGAIN;
  पूर्णाङ्क संहिता;
  वर्ण *info = रिक्त;

  (शून्य)len;
  (शून्य)buf;
  (शून्य)sockindex;
  *pnread = 0;
  यदि(!li || !lr)
    प्रत्यागम CURLE_FAILED_INIT;

  rc = ldap_परिणाम(li->ld, lr->msgid, LDAP_सन्देशः_ONE, &tv, &सन्देशः);
  यदि(rc < 0) {
    failf(दत्त, "LDAP local: search ldap_result %s", ldap_err2string(rc));
    परिणाम = CURLE_RECV_ERROR;
  }

  /* error or timed out */
  यदि(!सन्देशः)
    प्रत्यागम परिणाम;

  परिणाम = CURLE_OK;

  विकल्प(ldap_msgtype(सन्देशः)) {
  पक्ष LDAP_RES_अन्वेषणम्_परिणाम:
    lr->msgid = 0;
    rc = ldap_parse_परिणाम(li->ld, सन्देशः, &संहिता, रिक्त, &info, रिक्त, रिक्त, 0);
    यदि(rc) {
      failf(दत्त, "LDAP local: search ldap_parse_result %s",
            ldap_err2string(rc));
      परिणाम = CURLE_LDAP_अन्वेषणम्_FAILED;
      भङ्ग;
    }

    /* store the latest code for later retrieval */
    दत्त->info.httpcode = संहिता;

    विकल्प(संहिता) {
    पक्ष LDAP_SIZELIMIT_EXCEEDED:
      infof(दत्त, "There are more than %d entries", lr->nument);
      FALLTHROUGH();
    पक्ष LDAP_SUCCESS:
      दत्त->req.आकार = दत्त->req.bytecount;
      भङ्ग;
    स्वभाव:
      failf(दत्त, "LDAP remote: search failed %s %s", ldap_err2string(संहिता),
            info ? info : "");
      परिणाम = CURLE_LDAP_अन्वेषणम्_FAILED;
      भङ्ग;
    }
    यदि(info)
      ldap_memfree(info);
    भङ्ग;
  पक्ष LDAP_RES_अन्वेषणम्_प्रविष्टिः:
    lr->nument++;
    rc = ldap_get_dn_ber(li->ld, सन्देशः, &ber, &bv);
    यदि(rc < 0) {
      परिणाम = CURLE_RECV_ERROR;
      भङ्ग;
    }

    यदि(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
      वर्ण *dn_b64 = रिक्त;
      size_t dn_b64_sz = 0;

      /* A DN with e.g. CR/LF would inject LDIF lines, so base64-encode it. */
      परिणाम = curlx_base64_encode((uint8_t *)bv.bv_val, bv.bv_len,
                                   &dn_b64, &dn_b64_sz);
      यदि(!परिणाम)
        परिणाम = client_write(दत्त, STRCONST("DN:: "), dn_b64, dn_b64_sz,
                              STRCONST("\n"));
      curlx_free(dn_b64);
    }
    अन्यथा
      परिणाम = client_write(दत्त, STRCONST("DN: "), bv.bv_val, bv.bv_len,
                            STRCONST("\n"));
    यदि(परिणाम)
      भङ्ग;

    प्रति(rc = ldap_get_attribute_ber(li->ld, सन्देशः, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, सन्देशः, ber, &bv, &bvals)) {
      पूर्णाङ्क i;
      bool द्विमान;

      यदि(!bv.bv_val)
        भङ्ग;

      /* An attribute type has no base64 form in LDIF, so a type carrying
         control bytes (CR/LF etc.) would let a hostile server inject lines.
         A conformant type name never contains them, so reject it. */
      यदि(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
        failf(दत्त, "LDAP local: attribute type contains illegal bytes");
        यदि(bvals)
          ber_memfree(bvals);
        परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
        भङ्ग;
      }

      यदि(!bvals) {
        परिणाम = client_write(दत्त, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":\n"));
        यदि(परिणाम)
          भङ्ग;
        अनुवर्त;
      }

      द्विमान = bv.bv_len > 7 &&
        curl_strnequal(bv.bv_val + bv.bv_len - 7, ";binary", 7);

      प्रति(i = 0; bvals[i].bv_val; i++) {
        bool binval = FALSE;

        परिणाम = client_write(दत्त, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":"));
        यदि(परिणाम)
          भङ्ग;

        यदि(!द्विमान)
          binval = oldap_str_needs_base64(bvals[i].bv_val, bvals[i].bv_len);
        यदि(द्विमान || binval) {
          वर्ण *val_b64 = रिक्त;
          size_t val_b64_sz = 0;

          /* Binary value, encode to base64. */
          यदि(bvals[i].bv_len)
            परिणाम = curlx_base64_encode((uint8_t *)bvals[i].bv_val,
                                         bvals[i].bv_len,
                                         &val_b64, &val_b64_sz);
          यदि(!परिणाम)
            परिणाम = client_write(दत्त, STRCONST(": "), val_b64, val_b64_sz,
                                  STRCONST("\n"));
          curlx_free(val_b64);
        }
        अन्यथा
          परिणाम = client_write(दत्त, STRCONST(" "),
                                bvals[i].bv_val, bvals[i].bv_len,
                                STRCONST("\n"));
        यदि(परिणाम)
          भङ्ग;
      }

      ber_memfree(bvals);
      bvals = रिक्त;
      यदि(!परिणाम)
        परिणाम = client_write(दत्त, STRCONST("\n"), रिक्त, 0, रिक्त, 0);
      यदि(परिणाम)
        भङ्ग;
    }

    यदि(!परिणाम)
      परिणाम = client_write(दत्त, STRCONST("\n"), रिक्त, 0, रिक्त, 0);
    यदि(!परिणाम)
      परिणाम = CURLE_AGAIN;
    भङ्ग;
  }

  ber_free(ber, 0);
  ldap_msgfree(सन्देशः);
  प्रत्यागम परिणाम;
}

शून्य Curl_ldap_संस्करणम्(वर्ण *buf, size_t bufsz)
{
  LDAPAPIInfo api;
  api.ldapai_info_संस्करणम् = LDAP_API_INFO_संस्करणम्;

  यदि(ldap_get_option(रिक्त, LDAP_OPT_API_INFO, &api) == LDAP_OPT_SUCCESS) {
    अचिह्नित पूर्णाङ्क patch = (अचिह्नित पूर्णाङ्क)(api.ldapai_vendor_संस्करणम् % 100);
    अचिह्नित पूर्णाङ्क major = (अचिह्नित पूर्णाङ्क)(api.ldapai_vendor_संस्करणम् / 10000);
    अचिह्नित पूर्णाङ्क minor =
      (((अचिह्नित पूर्णाङ्क)api.ldapai_vendor_संस्करणम् - (major * 10000))
       - patch) / 100;
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u",
                   api.ldapai_vendor_name, major, minor, patch);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((शून्य **)api.ldapai_extensions);
  }
  अन्यथा
    curl_msnprintf(buf, bufsz, "OpenLDAP");
}

/*
 * LDAP protocol handler.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_ldap = {
  oldap_सज्जता_connection,               /* setup_connection */
  oldap_do,                             /* do_it */
  oldap_समाप्तम्,                           /* done */
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

#यदि_अन्तः /* !CURL_DISABLE_LDAP && USE_OPENLDAP */

/* The LDAP scheme structs are in ldap.c */
