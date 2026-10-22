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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP)

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

#অন্তর্ভাব <ldap.h>

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "curl_ldap.h"
#অন্তর্ভাব "curlx/base64.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "curl_sasl.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "bufref.h"

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
প্রকারলেখ গণনা {
  OLDAP_STOP,           /* Do nothing state, stops the state machine */
  OLDAP_SSL,            /* Performing SSL handshake. */
  OLDAP_STARTTLS,       /* STARTTLS request sent. */
  OLDAP_TLS,            /* Performing TLS handshake. */
  OLDAP_MECHS,          /* Get SASL authentication mechanisms. */
  OLDAP_SASL,           /* SASL binding reply. */
  OLDAP_BIND,           /* Simple bind reply. */
  OLDAP_BINDV2,         /* Simple bind reply in protocol version 2. */
  OLDAP_LAST            /* Never used */
} ldapstate;

#ifndef _LDAP_PVT_H
বাহ্যিক পূর্ণসংখ্যা ldap_pvt_url_scheme2proto(ধ্রুবক অক্ষর *);
বাহ্যিক পূর্ণসংখ্যা ldap_init_fd(ber_socket_t fd, পূর্ণসংখ্যা proto, ধ্রুবক অক্ষর *url,
                        LDAP **ld);
#endif

স্থির Curl_recv oldap_recv;

কাঠামো ldapconninfo {
  কাঠামো SASL sasl;          /* SASL-related parameters */
  LDAP *ld;                  /* Openldap connection handle. */
  Curl_recv *recv;           /* For stacking SSL handler */
  Curl_send *send;
  কাঠামো berval *servercred; /* SASL data from server. */
  ldapstate state;           /* Current machine state. */
  curl_socket_t dupfd;       /* duplicated socket */
  পূর্ণসংখ্যা proto;                 /* LDAP_PROTO_TCP/LDAP_PROTO_UDP/LDAP_PROTO_IPC */
  পূর্ণসংখ্যা msgid;                 /* Current message id. */
};

কাঠামো ldapreqinfo {
  পূর্ণসংখ্যা msgid;
  পূর্ণসংখ্যা nument;
};

/* meta key for storing ldapconninfo at easy handle */
#define CURL_META_LDAP_EASY   "meta:proto:ldap:easy"
/* meta key for storing ldapconninfo at connection */
#define CURL_META_LDAP_CONN   "meta:proto:ldap:conn"

/*
 * oldap_state()
 *
 * This is the ONLY way to change LDAP state!
 */
স্থির শূন্য oldap_state(কাঠামো Curl_easy *তথ্য, কাঠামো ldapconninfo *li,
                        ldapstate newstate)
{
#যদি defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  স্থির ধ্রুবক অক্ষর * ধ্রুবক names[] = {
    "STOP",
    "SSL",
    "STARTTLS",
    "TLS",
    "MECHS",
    "SASL",
    "BIND",
    "BINDV2",
    /* LAST */
  };

  যদি(li->state != newstate)
    infof(তথ্য, "LDAP %p state change from %s to %s",
          (শূন্য *)li, names[li->state], names[newstate]);
#অন্যথা
  (শূন্য)তথ্য;
#endif
  li->state = newstate;
}

/* Map some particular LDAP error codes to CURLcode values. */
স্থির CURLcode oldap_map_error(পূর্ণসংখ্যা rc, CURLcode ফলাফল)
{
  নির্বাচন(rc) {
  ক্ষেত্র LDAP_NO_MEMORY:
    ফেরত CURLE_OUT_OF_MEMORY;
  ক্ষেত্র LDAP_INVALID_CREDENTIALS:
    ফেরত CURLE_LOGIN_DENIED;
  ক্ষেত্র LDAP_PROTOCOL_ERROR:
    ফেরত CURLE_UNSUPPORTED_PROTOCOL;
  ক্ষেত্র LDAP_INSUFFICIENT_ACCESS:
    ফেরত CURLE_REMOTE_ACCESS_DENIED;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode oldap_url_parse(কাঠামো Curl_easy *তথ্য, LDAPURLDesc **ludp)
{
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা rc;
  স্থির ধ্রুবক অক্ষর * ধ্রুবক url_errs[] = {
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

  *ludp = ফাঁকা;
  /* `ldap_url_parse() seems to be terrible with urls
   * that have user/pass/options in it. So when we have options or
   * creds from the url, fail without calling the function.
   * Yes, this is super-weird code and I do not like it. */
  যদি((তথ্য->state.creds && (তথ্য->state.creds->source == CREDS_URL)) ||
     তথ্য->state.up.options)
    rc = LDAP_URL_ERR_BADURL;
  অন্যথা
    rc = ldap_url_parse(Curl_bufref_ptr(&তথ্য->state.url), ludp);

  যদি(rc != LDAP_URL_SUCCESS) {
    ধ্রুবক অক্ষর *বার্তা = "url parsing problem";

    ফলাফল = rc == LDAP_URL_ERR_MEM ? CURLE_OUT_OF_MEMORY :
      CURLE_URL_MALFORMAT;
    rc -= LDAP_URL_SUCCESS;
    যদি((size_t)rc < CURL_ARRAYSIZE(url_errs))
      বার্তা = url_errs[rc];
    failf(তথ্য, "LDAP local: %s", বার্তা);
  }
  ফেরত ফলাফল;
}

/* Parse the login options. */
স্থির CURLcode oldap_parse_login_options(কাঠামো connectdata *conn)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ধ্রুবক অক্ষর *ptr = conn->options;

  DEBUGASSERT(li);
  যদি(!li)
    ফেরত CURLE_FAILED_INIT;

  যখন(!ফলাফল && ptr && *ptr) {
    ধ্রুবক অক্ষর *চাবি = ptr;
    ধ্রুবক অক্ষর *মান;

    যখন(*ptr && *ptr != '=')
      ptr++;

    মান = ptr + 1;

    যখন(*ptr && *ptr != ';')
      ptr++;

    যদি(checkprefix("AUTH=", চাবি))
      ফলাফল = Curl_sasl_parse_url_auth_option(&li->sasl, মান, ptr - মান);
    অন্যথা
      ফলাফল = CURLE_SETOPT_OPTION_SYNTAX;

    যদি(*ptr == ';')
      ptr++;
  }

  ফেরত ফলাফল == CURLE_URL_MALFORMAT ? CURLE_SETOPT_OPTION_SYNTAX : ফলাফল;
}

স্থির CURLcode oldap_setup_connection(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো connectdata *conn)
{
  CURLcode ফলাফল;
  LDAPURLDesc *lud;
  (শূন্য)conn;

  /* Early URL syntax check. */
  ফলাফল = oldap_url_parse(তথ্য, &lud);
  ldap_free_urldesc(lud);

  ফেরত ফলাফল;
}

/*
 * Get the SASL authentication challenge from the server credential buffer.
 */
স্থির CURLcode oldap_get_message(কাঠামো Curl_easy *তথ্য, কাঠামো bufref *out)
{
  কাঠামো ldapconninfo *li =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_LDAP_CONN);
  কাঠামো berval *servercred = li ? li->servercred : ফাঁকা;
  DEBUGASSERT(li);
  যদি(!li)
    ফেরত CURLE_FAILED_INIT;

  যদি(!servercred || !servercred->bv_val)
    ফেরত CURLE_WEIRD_SERVER_REPLY;
  Curl_bufref_set(out, servercred->bv_val, servercred->bv_len, ফাঁকা);
  ফেরত CURLE_OK;
}

/*
 * Sends an initial SASL bind request to the server.
 */
স্থির CURLcode oldap_perform_auth(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *mech,
                                   ধ্রুবক কাঠামো bufref *initresp)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  কাঠামো berval cred;
  কাঠামো berval *pcred = &cred;
  পূর্ণসংখ্যা rc;

  DEBUGASSERT(li);
  যদি(!li)
    ফেরত CURLE_FAILED_INIT;
  cred.bv_val = (অক্ষর *)CURL_UNCONST(Curl_bufref_ptr(initresp));
  cred.bv_len = Curl_bufref_len(initresp);
  যদি(!cred.bv_val)
    pcred = ফাঁকা;
  rc = ldap_sasl_bind(li->ld, ফাঁকা, mech, pcred, ফাঁকা, ফাঁকা, &li->msgid);
  যদি(rc != LDAP_SUCCESS)
    ফেরত oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ফেরত CURLE_OK;
}

/*
 * Sends SASL continuation.
 */
স্থির CURLcode oldap_continue_auth(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *mech,
                                    ধ্রুবক কাঠামো bufref *resp)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  কাঠামো berval cred;
  কাঠামো berval *pcred = &cred;
  পূর্ণসংখ্যা rc;

  যদি(!li)
    ফেরত CURLE_FAILED_INIT;
  cred.bv_val = (অক্ষর *)CURL_UNCONST(Curl_bufref_ptr(resp));
  cred.bv_len = Curl_bufref_len(resp);
  যদি(!cred.bv_val)
    pcred = ফাঁকা;
  rc = ldap_sasl_bind(li->ld, ফাঁকা, mech, pcred, ফাঁকা, ফাঁকা, &li->msgid);
  যদি(rc != LDAP_SUCCESS)
    ফেরত oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ফেরত CURLE_OK;
}

/*
 * Sends SASL bind cancellation.
 */
স্থির CURLcode oldap_cancel_auth(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *mech)
{
  কাঠামো ldapconninfo *li =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_LDAP_CONN);
  পূর্ণসংখ্যা rc;

  (শূন্য)mech;
  যদি(!li)
    ফেরত CURLE_FAILED_INIT;
  rc = ldap_sasl_bind(li->ld, ফাঁকা, LDAP_SASL_NULL, ফাঁকা, ফাঁকা, ফাঁকা,
                      &li->msgid);
  যদি(rc != LDAP_SUCCESS)
    ফেরত oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ফেরত CURLE_OK;
}

/* Starts LDAP simple bind. */
স্থির CURLcode oldap_perform_bind(কাঠামো Curl_easy *তথ্য, ldapstate newstate)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ধ্রুবক অক্ষর *binddn = ফাঁকা;
  কাঠামো berval passwd;
  পূর্ণসংখ্যা rc;

  যদি(!li)
    ফেরত CURLE_FAILED_INIT;
  passwd.bv_val = ফাঁকা;
  passwd.bv_len = 0;

  যদি(conn->creds) {
    binddn = Curl_creds_user(conn->creds);
    passwd.bv_val = CURL_UNCONST(Curl_creds_passwd(conn->creds));
    passwd.bv_len = strlen(passwd.bv_val);
  }

  rc = ldap_sasl_bind(li->ld, binddn, LDAP_SASL_SIMPLE, &passwd,
                      ফাঁকা, ফাঁকা, &li->msgid);
  যদি(rc != LDAP_SUCCESS)
    ফেরত oldap_map_error(rc,
                           তথ্য->state.creds ?
                           CURLE_LOGIN_DENIED : CURLE_LDAP_CANNOT_BIND);
  oldap_state(তথ্য, li, newstate);
  ফেরত CURLE_OK;
}

/* Query the supported SASL authentication mechanisms. */
স্থির CURLcode oldap_perform_mechs(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ldapconninfo *li =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_LDAP_CONN);
  পূর্ণসংখ্যা rc;
  স্থির ধ্রুবক অক্ষর * ধ্রুবক supportedSASLMechanisms[] = {
    "supportedSASLMechanisms",
    ফাঁকা
  };

  যদি(!li)
    ফেরত CURLE_FAILED_INIT;
  /* Casting away the const for the 3rd parameter that the LDAP API expects as
     a non-const char ** is potentially unsafe but we believe the lack of
     const in the API was an oversight and that no LDAP implementation
     actually modifies the input. */
  rc = ldap_অনুসন্ধান_ext(li->ld, "", LDAP_SCOPE_BASE, "(objectclass=*)",
                       (অক্ষর **)CURL_UNCONST(supportedSASLMechanisms), 0,
                       ফাঁকা, ফাঁকা, ফাঁকা, 0, &li->msgid);
  যদি(rc != LDAP_SUCCESS)
    ফেরত oldap_map_error(rc, CURLE_LOGIN_DENIED);
  oldap_state(তথ্য, li, OLDAP_MECHS);
  ফেরত CURLE_OK;
}

/* Starts SASL bind. */
স্থির CURLcode oldap_perform_sasl(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ldapconninfo *li =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_LDAP_CONN);
  saslprogress progress = SASL_IDLE;
  CURLcode ফলাফল;

  যদি(!li)
    ফেরত CURLE_FAILED_INIT;
  ফলাফল = Curl_sasl_start(&li->sasl, তথ্য, TRUE, &progress);

  oldap_state(তথ্য, li, OLDAP_SASL);
  যদি(!ফলাফল && progress != SASL_INPROGRESS)
    ফলাফল = Curl_sasl_is_blocked(&li->sasl, তথ্য);
  ফেরত ফলাফল;
}

#ifdef USE_SSL
স্থির পূর্ণসংখ্যা ldapsb_tls_setup(Sockbuf_IO_Desc *sbiod, শূন্য *arg)
{
  sbiod->sbiod_pvt = arg;
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা ldapsb_tls_remove(Sockbuf_IO_Desc *sbiod)
{
  sbiod->sbiod_pvt = ফাঁকা;
  ফেরত 0;
}

/* if there is a duplicated socket to close, close it */
স্থির পূর্ণসংখ্যা ldapsb_tls_close(Sockbuf_IO_Desc *sbiod)
{
  কাঠামো Curl_easy *তথ্য = sbiod->sbiod_pvt;
  যদি(তথ্য) {
    কাঠামো connectdata *conn = তথ্য->conn;
    যদি(conn) {
      কাঠামো ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      যদি(li && (li->dupfd != CURL_SOCKET_BAD)) {
        sclose(li->dupfd);
        li->dupfd = CURL_SOCKET_BAD;
      }
    }
  }

  ফেরত 0;
}

স্থির পূর্ণসংখ্যা ldapsb_tls_ctrl(Sockbuf_IO_Desc *sbiod, পূর্ণসংখ্যা opt, শূন্য *arg)
{
  (শূন্য)arg;
  যদি(opt == LBER_SB_OPT_তথ্য_READY) {
    কাঠামো Curl_easy *তথ্য = sbiod->sbiod_pvt;
    ফেরত Curl_conn_তথ্য_pending(তথ্য, FIRSTSOCKET);
  }
  ফেরত 0;
}

স্থির ber_slen_t ldapsb_tls_read(Sockbuf_IO_Desc *sbiod, শূন্য *buf,
                                  ber_len_t len)
{
  কাঠামো Curl_easy *তথ্য = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  যদি(তথ্য) {
    কাঠামো connectdata *conn = তথ্য->conn;
    যদি(conn) {
      কাঠামো ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode ফলাফল = CURLE_RECV_ERROR;
      size_t nread;

      যদি(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        ফেরত -1;
      }
      ফলাফল = (li->recv)(তথ্য, FIRSTSOCKET, buf, len, &nread);
      যদি(ফলাফল == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = ফলাফল ? -1 : (ber_slen_t)nread;
    }
  }
  ফেরত ret;
}

স্থির ber_slen_t ldapsb_tls_write(Sockbuf_IO_Desc *sbiod, শূন্য *buf,
                                   ber_len_t len)
{
  কাঠামো Curl_easy *তথ্য = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  যদি(তথ্য) {
    কাঠামো connectdata *conn = তথ্য->conn;
    যদি(conn) {
      কাঠামো ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode ফলাফল = CURLE_SEND_ERROR;
      size_t nwritten;

      যদি(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        ফেরত -1;
      }
      ফলাফল = (li->send)(তথ্য, FIRSTSOCKET, buf, len, FALSE, &nwritten);
      যদি(ফলাফল == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = ফলাফল ? -1 : (ber_slen_t)nwritten;
    }
  }
  ফেরত ret;
}

স্থির Sockbuf_IO ldapsb_tls = {
  ldapsb_tls_setup,
  ldapsb_tls_remove,
  ldapsb_tls_ctrl,
  ldapsb_tls_read,
  ldapsb_tls_write,
  ldapsb_tls_close
};

স্থির bool ssl_installed(কাঠামো connectdata *conn)
{
  কাঠামো ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ফেরত li && li->recv;
}

স্থির CURLcode oldap_ssl_connect(কাঠামো Curl_easy *তথ্য, ldapstate newstate)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  bool ssldone = FALSE;
  CURLcode ফলাফল;

  যদি(!li)
    ফেরত CURLE_FAILED_INIT;
  ফলাফল = Curl_conn_connect(তথ্য, FIRSTSOCKET, FALSE, &ssldone);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  oldap_state(তথ্য, li, newstate);

  যদি(ssldone) {
    Sockbuf *sb;

    /* Install the libcurl SSL handlers into the sockbuf. */
    যদি((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, তথ্য))
      ফেরত CURLE_FAILED_INIT;
    li->recv = conn->recv[FIRSTSOCKET];
    li->send = conn->send[FIRSTSOCKET];
  }

  ফেরত ফলাফল;
}

/* Send the STARTTLS request */
স্থির CURLcode oldap_perform_starttls(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ldapconninfo *li =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_LDAP_CONN);
  পূর্ণসংখ্যা rc;

  যদি(!li)
    ফেরত CURLE_FAILED_INIT;
  rc = ldap_start_tls(li->ld, ফাঁকা, ফাঁকা, &li->msgid);
  যদি(rc != LDAP_SUCCESS)
    ফেরত oldap_map_error(rc, CURLE_USE_SSL_FAILED);
  oldap_state(তথ্য, li, OLDAP_STARTTLS);
  ফেরত CURLE_OK;
}
#endif

স্থির শূন্য oldap_easy_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  কাঠামো ldapreqinfo *lr = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  curlx_free(lr);
}

স্থির শূন্য oldap_conn_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  কাঠামো ldapconninfo *li = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  যদি(li->ld) {
    ldap_unbind_ext(li->ld, ফাঁকা, ফাঁকা);
    li->ld = ফাঁকা;
  }
  curlx_free(li);
}

/* SASL parameters for the ldap protocol */
স্থির ধ্রুবক কাঠামো SASLproto saslldap = {
  "ldap",                     /* The service name */
  oldap_perform_auth,         /* Send authentication command */
  oldap_continue_auth,        /* Send authentication continuation */
  oldap_cancel_auth,          /* Send authentication cancellation */
  oldap_get_message,          /* Get SASL response message */
  0,                          /* Maximum initial response length (no max) */
  LDAP_SASL_BIND_IN_PROGRESS, /* Code received when continuation is expected */
  LDAP_SUCCESS,               /* Code to receive upon authentication success */
  SASL_AUTH_NONE,             /* Default mechanisms */
  0                           /* Configuration flags */
};

স্থির CURLcode oldap_connect(কাঠামো Curl_easy *তথ্য, bool *done)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ldapconninfo *li;
  স্থির ধ্রুবক পূর্ণসংখ্যা version = LDAP_VERSION3;
  অক্ষর *hosturl = ফাঁকা;
  CURLcode ফলাফল;
  পূর্ণসংখ্যা rc;
#ifdef CURL_OPENLDAP_DEBUG
  স্থির পূর্ণসংখ্যা do_trace = -1;
#endif

  (শূন্য)done;

  li = curlx_calloc(1, আকার(কাঠামো ldapconninfo));
  যদি(!li) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  li->dupfd = CURL_SOCKET_BAD;

  ফলাফল = Curl_conn_meta_set(conn, CURL_META_LDAP_CONN, li, oldap_conn_dtor);
  যদি(ফলাফল)
    যাও out;

  li->proto = ldap_pvt_url_scheme2proto(তথ্য->state.origin->scheme->name);

  /* Initialize the SASL storage */
  Curl_sasl_init(&li->sasl, তথ্য, &saslldap);

  ফলাফল = oldap_parse_login_options(conn);
  যদি(ফলাফল)
    যাও out;

  hosturl = curl_maprintf("%s://%s:%u",
                          conn->scheme->name,
                          conn->origin->ipv6 ?
                          conn->origin->user_hostname :
                          conn->origin->hostname,
                          conn->origin->port);
  যদি(!hosturl) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  {
#ifdef _WIN32
    SOCKET dupfd = INVALID_SOCKET;
    WSAPROTOCOL_INFO pi;
    যদি(WSADuplicateSocket(conn->sock[FIRSTSOCKET], GetCurrentProcessId(),
                          &pi))
      ; /* error */
    অন্যথা
      dupfd = CURL_SOCKET(pi.iAddressFamily, pi.iSocketType, pi.iProtocol);
#অন্যথা
#ifdef F_DUPFD_CLOEXEC
    পূর্ণসংখ্যা dupfd = fcntl(conn->sock[FIRSTSOCKET], F_DUPFD_CLOEXEC, 0);
#অন্যথা
    পূর্ণসংখ্যা dupfd = dup(conn->sock[FIRSTSOCKET]);
#ifdef HAVE_FCNTL
    যদি((dupfd != CURL_SOCKET_BAD) &&
       (fcntl(dupfd, F_SETFD, FD_CLOEXEC) < 0)) {
      sclose(dupfd);
      dupfd = CURL_SOCKET_BAD;
    }
#endif /* HAVE_FCNTL */
#endif /* F_DUPFD_CLOEXEC */
#endif /* _WIN32 */

    যদি(dupfd == CURL_SOCKET_BAD) {
      ফলাফল = CURLE_COULDNT_CONNECT;
      যাও out;
    }
    rc = ldap_init_fd((ber_socket_t)dupfd, li->proto, hosturl, &li->ld);
    যদি(rc)
      sclose(dupfd);
    অন্যথা
      /* store the socket, as when this is used by TLS we close this ourselves
         and at this point the code does not yet know if this will use TLS or
         not */
      li->dupfd = dupfd;
  }
  যদি(rc) {
    failf(তথ্য, "LDAP local: Cannot connect to %s, %s",
          hosturl, ldap_err2string(rc));
    ফলাফল = CURLE_COULDNT_CONNECT;
    যাও out;
  }

#ifdef CURL_OPENLDAP_DEBUG
  যদি(do_trace < 0) {
    ধ্রুবক অক্ষর *env = getenv("CURL_OPENLDAP_TRACE");
    curl_off_t e = 0;
    যদি(!curlx_str_সংখ্যা(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  যদি(do_trace)
    ldap_set_option(li->ld, LDAP_OPT_DEBUG_LEVEL, &do_trace);
#endif

  /* Try version 3 first. */
  ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);

  /* Do not chase referrals. */
  ldap_set_option(li->ld, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  {
    ber_len_t max = 256 * 1024;
    Sockbuf *sb;
    যদি((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       /* Set the maximum allowed size of an incoming message, which to
          OpenLDAP means that it will malloc() memory up to this size. If not
          set, there is no limit and we instead risk a malloc() failure. */
       !ber_sockbuf_ctrl(sb, LBER_SB_OPT_SET_MAX_INCOMING, &max)) {
      ফলাফল = CURLE_FAILED_INIT;
      যাও out;
    }
  }

#ifdef USE_SSL
  যদি(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    ফলাফল = oldap_ssl_connect(তথ্য, OLDAP_SSL);
    যাও out;
  }

  যদি(তথ্য->set.use_ssl) {
    ফলাফল = oldap_perform_starttls(তথ্য);
    যদি(!ফলাফল || তথ্য->set.use_ssl != CURLUSESSL_TRY)
      যাও out;
  }
#endif

  যদি(li->sasl.prefmech != SASL_AUTH_NONE) {
    ফলাফল = oldap_perform_mechs(তথ্য);
    যাও out;
  }

  /* Force bind even if anonymous bind is not needed in protocol version 3
     to detect missing version 3 support. */
  ফলাফল = oldap_perform_bind(তথ্য, OLDAP_BIND);

out:
  curlx_free(hosturl);
  ফেরত ফলাফল;
}

/* Handle the supported SASL mechanisms query response */
স্থির CURLcode oldap_state_mechs_resp(কাঠামো Curl_easy *তথ্য,
                                       LDAPMessage *বার্তা, পূর্ণসংখ্যা code)
{
  কাঠামো connectdata *conn;
  কাঠামো ldapconninfo *li;
  পূর্ণসংখ্যা rc;
  BerElement *ber = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো berval bv, *bvals;

  যদি(!তথ্য)
    ফেরত CURLE_FAILED_INIT;

  conn = তথ্য->conn;
  li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);

  যদি(!li)
    ফেরত CURLE_FAILED_INIT;
  নির্বাচন(ldap_msgtype(বার্তা)) {
  ক্ষেত্র LDAP_RES_অনুসন্ধান_ENTRY:
    /* Got a list of supported SASL mechanisms. */
    যদি(code != LDAP_SUCCESS && code != LDAP_NO_RESULTS_RETURNED)
      ফেরত CURLE_LOGIN_DENIED;

    rc = ldap_get_dn_ber(li->ld, বার্তা, &ber, &bv);
    যদি(rc < 0)
      ফেরত oldap_map_error(rc, CURLE_BAD_CONTENT_ENCODING);
    জন্য(rc = ldap_get_attribute_ber(li->ld, বার্তা, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, বার্তা, ber, &bv, &bvals)) {
      পূর্ণসংখ্যা i;

      যদি(!bv.bv_val)
        বিরতি;

      যদি(bvals) {
        জন্য(i = 0; bvals[i].bv_val; i++) {
          size_t llen;
          unsigned short mech =
            Curl_sasl_decode_mech((ধ্রুবক অক্ষর *)bvals[i].bv_val,
                                  bvals[i].bv_len, &llen);
          যদি(bvals[i].bv_len == llen)
            li->sasl.authmechs |= mech;
        }
        ber_memfree(bvals);
      }
    }
    ber_free(ber, 0);
    বিরতি;

  ক্ষেত্র LDAP_RES_অনুসন্ধান_ফলাফল:
    নির্বাচন(code) {
    ক্ষেত্র LDAP_SIZELIMIT_EXCEEDED:
      infof(তথ্য, "Too many authentication mechanisms");
      FALLTHROUGH();
    ক্ষেত্র LDAP_SUCCESS:
    ক্ষেত্র LDAP_NO_RESULTS_RETURNED:
      যদি(Curl_sasl_can_authenticate(&li->sasl, তথ্য))
        ফলাফল = oldap_perform_sasl(তথ্য);
      অন্যথা
        ফলাফল = CURLE_LOGIN_DENIED;
      বিরতি;
    পূর্বনির্ধারিত:
      ফলাফল = oldap_map_error(code, CURLE_LOGIN_DENIED);
      বিরতি;
    }
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত ফলাফল;
}

/* Handle a SASL bind response. */
স্থির CURLcode oldap_state_sasl_resp(কাঠামো Curl_easy *তথ্য,
                                      LDAPMessage *বার্তা, পূর্ণসংখ্যা code)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode ফলাফল = CURLE_OK;
  saslprogress progress;
  পূর্ণসংখ্যা rc;

  যদি(!li)
    ফেরত CURLE_FAILED_INIT;
  li->servercred = ফাঁকা;
  rc = ldap_parse_sasl_bind_ফলাফল(li->ld, বার্তা, &li->servercred, 0);
  যদি(rc != LDAP_SUCCESS) {
    failf(তথ্য, "LDAP local: sasl ldap_parse_result %s", ldap_err2string(rc));
    ফলাফল = oldap_map_error(rc, CURLE_LOGIN_DENIED);
  }
  অন্যথা {
    ফলাফল = Curl_sasl_continue(&li->sasl, তথ্য, code, &progress);
    যদি(!ফলাফল) {
      নির্বাচন(progress) {
      ক্ষেত্র SASL_DONE:
        oldap_state(তথ্য, li, OLDAP_STOP);   /* Authenticated */
        বিরতি;
      ক্ষেত্র SASL_IDLE:            /* No mechanism left after cancellation */
        failf(তথ্য, "Authentication cancelled");
        ফলাফল = CURLE_LOGIN_DENIED;
        বিরতি;
      পূর্বনির্ধারিত:
        বিরতি;
      }
    }
  }

  যদি(li->servercred)
    ber_bvfree(li->servercred);
  ফেরত ফলাফল;
}

/* Handle a simple bind response. */
স্থির CURLcode oldap_state_bind_resp(কাঠামো Curl_easy *তথ্য, LDAPMessage *বার্তা,
                                      পূর্ণসংখ্যা code)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো berval *bv = ফাঁকা;
  পূর্ণসংখ্যা rc;

  যদি(!li)
    ফেরত CURLE_FAILED_INIT;

  যদি(code != LDAP_SUCCESS)
    ফেরত oldap_map_error(code, CURLE_LDAP_CANNOT_BIND);

  rc = ldap_parse_sasl_bind_ফলাফল(li->ld, বার্তা, &bv, 0);
  যদি(rc != LDAP_SUCCESS) {
    failf(তথ্য, "LDAP local: bind ldap_parse_sasl_bind_result %s",
          ldap_err2string(rc));
    ফলাফল = oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  }
  অন্যথা
    oldap_state(তথ্য, li, OLDAP_STOP);

  যদি(bv)
    ber_bvfree(bv);
  ফেরত ফলাফল;
}

স্থির CURLcode oldap_connecting(কাঠামো Curl_easy *তথ্য, bool *done)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  LDAPMessage *বার্তা = ফাঁকা;
  কাঠামো timeval tv = { 0, 0 };
  পূর্ণসংখ্যা code = LDAP_SUCCESS;
  পূর্ণসংখ্যা rc;

  যদি(!li)
    ফেরত CURLE_FAILED_INIT;

  যদি(li->state != OLDAP_SSL && li->state != OLDAP_TLS) {
    /* Get response to last command. */
    rc = ldap_ফলাফল(li->ld, li->msgid, LDAP_বার্তা_ONE, &tv, &বার্তা);
    নির্বাচন(rc) {
    ক্ষেত্র 0:                               /* Timed out. */
      ফেরত CURLE_OK;
    ক্ষেত্র LDAP_RES_অনুসন্ধান_ENTRY:
    ক্ষেত্র LDAP_RES_অনুসন্ধান_REFERENCE:
      বিরতি;
    পূর্বনির্ধারিত:
      li->msgid = 0;                      /* Nothing to abandon upon error. */
      যদি(rc < 0) {
        failf(তথ্য, "LDAP local: connecting ldap_result %s",
              ldap_err2string(rc));
        ফেরত oldap_map_error(rc, CURLE_COULDNT_CONNECT);
      }
      বিরতি;
    }

    /* Get error code from message. */
    rc = ldap_parse_ফলাফল(li->ld, বার্তা, &code, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, 0);
    যদি(rc)
      code = rc;
    অন্যথা {
      /* store the latest code for later retrieval */
      তথ্য->info.httpcode = code;
    }

    /* If protocol version 3 is not supported, fallback to version 2. */
    যদি(code == LDAP_PROTOCOL_ERROR && li->state != OLDAP_BINDV2 &&
#ifdef USE_SSL
       (ssl_installed(conn) || তথ্য->set.use_ssl <= CURLUSESSL_TRY) &&
#endif
       li->sasl.prefmech == SASL_AUTH_NONE) {
      স্থির ধ্রুবক পূর্ণসংখ্যা version = LDAP_VERSION2;

      ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);
      ldap_msgfree(বার্তা);
      ফেরত oldap_perform_bind(তথ্য, OLDAP_BINDV2);
    }
  }

  /* Handle response message according to current state. */
  নির্বাচন(li->state) {

#ifdef USE_SSL
  ক্ষেত্র OLDAP_SSL:
    ফলাফল = oldap_ssl_connect(তথ্য, OLDAP_SSL);
    যদি(!ফলাফল && ssl_installed(conn)) {
      যদি(li->sasl.prefmech != SASL_AUTH_NONE)
        ফলাফল = oldap_perform_mechs(তথ্য);
      অন্যথা
        ফলাফল = oldap_perform_bind(তথ্য, OLDAP_BIND);
    }
    বিরতি;
  ক্ষেত্র OLDAP_STARTTLS:
    যদি(code != LDAP_SUCCESS) {
      যদি(তথ্য->set.use_ssl != CURLUSESSL_TRY)
        ফলাফল = oldap_map_error(code, CURLE_USE_SSL_FAILED);
      অন্যথা যদি(li->sasl.prefmech != SASL_AUTH_NONE)
        ফলাফল = oldap_perform_mechs(তথ্য);
      অন্যথা
        ফলাফল = oldap_perform_bind(তথ্য, OLDAP_BIND);
      বিরতি;
    }
    ফলাফল = Curl_ssl_cfilter_add(
      তথ্য, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    যদি(ফলাফল)
      বিরতি;
    FALLTHROUGH();
  ক্ষেত্র OLDAP_TLS:
    ফলাফল = oldap_ssl_connect(তথ্য, OLDAP_TLS);
    যদি(ফলাফল)
      ফলাফল = oldap_map_error(code, CURLE_USE_SSL_FAILED);
    অন্যথা যদি(ssl_installed(conn)) {
      যদি(li->sasl.prefmech != SASL_AUTH_NONE)
        ফলাফল = oldap_perform_mechs(তথ্য);
      অন্যথা যদি(তথ্য->state.creds)
        ফলাফল = oldap_perform_bind(তথ্য, OLDAP_BIND);
      অন্যথা {
        /* Version 3 supported: no bind required */
        oldap_state(তথ্য, li, OLDAP_STOP);
        ফলাফল = CURLE_OK;
      }
    }
    বিরতি;
#endif

  ক্ষেত্র OLDAP_MECHS:
    ফলাফল = oldap_state_mechs_resp(তথ্য, বার্তা, code);
    বিরতি;
  ক্ষেত্র OLDAP_SASL:
    ফলাফল = oldap_state_sasl_resp(তথ্য, বার্তা, code);
    বিরতি;
  ক্ষেত্র OLDAP_BIND:
  ক্ষেত্র OLDAP_BINDV2:
    ফলাফল = oldap_state_bind_resp(তথ্য, বার্তা, code);
    বিরতি;
  পূর্বনির্ধারিত:
    /* internal error */
    ফলাফল = CURLE_COULDNT_CONNECT;
    বিরতি;
  }

  ldap_msgfree(বার্তা);

  *done = li->state == OLDAP_STOP;
  যদি(*done)
    conn->recv[FIRSTSOCKET] = oldap_recv;

  যদি(ফলাফল && li->msgid) {
    ldap_abandon_ext(li->ld, li->msgid, ফাঁকা, ফাঁকা);
    li->msgid = 0;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode oldap_disconnect(কাঠামো Curl_easy *তথ্য,
                                 কাঠামো connectdata *conn,
                                 bool dead_connection)
{
  কাঠামো ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  (শূন্য)dead_connection;
#ifndef USE_SSL
  (শূন্য)তথ্য;
#endif

  যদি(li && li->ld) {
#ifdef USE_SSL
    যদি(ssl_installed(conn)) {
      Sockbuf *sb;
      যদি(ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS ||
         ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, তথ্য))
        ফেরত CURLE_FAILED_INIT;
    }
#endif
    ldap_unbind_ext(li->ld, ফাঁকা, ফাঁকা);
    li->ld = ফাঁকা;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode oldap_do(কাঠামো Curl_easy *তথ্য, bool *done)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  কাঠামো ldapreqinfo *lr;
  CURLcode ফলাফল;
  পূর্ণসংখ্যা rc;
  LDAPURLDesc *lud;
  পূর্ণসংখ্যা msgid;

  যদি(!li)
    ফেরত CURLE_FAILED_INIT;

  infof(তথ্য, "LDAP local: %s", Curl_bufref_ptr(&তথ্য->state.url));

  ফলাফল = oldap_url_parse(তথ্য, &lud);
  যদি(ফলাফল)
    যাও out;

#ifdef USE_SSL
  যদি(ssl_installed(conn)) {
    Sockbuf *sb;
    /* re-install the libcurl SSL handlers into the sockbuf. */
    যদি((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, তথ্য)) {
      ldap_free_urldesc(lud);
      ফেরত CURLE_FAILED_INIT;
    }
  }
#endif

  rc = ldap_অনুসন্ধান_ext(li->ld, lud->lud_dn, lud->lud_scope,
                       lud->lud_filter, lud->lud_attrs, 0,
                       ফাঁকা, ফাঁকা, ফাঁকা, 0, &msgid);
  ldap_free_urldesc(lud);
  যদি(rc != LDAP_SUCCESS) {
    failf(তথ্য, "LDAP local: ldap_search_ext %s", ldap_err2string(rc));
    ফলাফল = CURLE_LDAP_অনুসন্ধান_FAILED;
    যাও out;
  }

  lr = curlx_calloc(1, আকার(কাঠামো ldapreqinfo));
  যদি(!lr ||
     Curl_meta_set(তথ্য, CURL_META_LDAP_EASY, lr, oldap_easy_dtor)) {
    ldap_abandon_ext(li->ld, msgid, ফাঁকা, ফাঁকা);
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  lr->msgid = msgid;
  Curl_xfer_setup_recv(তথ্য, FIRSTSOCKET, -1);
  *done = TRUE;

out:
  ফেরত ফলাফল;
}

স্থির CURLcode oldap_done(কাঠামো Curl_easy *তথ্য, CURLcode res,
                           bool premature)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ldapreqinfo *lr = Curl_meta_get(তথ্য, CURL_META_LDAP_EASY);

  (শূন্য)res;
  (শূন্য)premature;

  যদি(lr) {
    /* if there was a search in progress, abandon it */
    যদি(lr->msgid) {
      কাঠামো ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      যদি(li && li->ld) {
        ldap_abandon_ext(li->ld, lr->msgid, ফাঁকা, ফাঁকা);
      }
      lr->msgid = 0;
    }
    Curl_meta_remove(তথ্য, CURL_META_LDAP_EASY);
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode client_write(কাঠামো Curl_easy *তথ্য,
                             ধ্রুবক অক্ষর *prefix, size_t plen,
                             ধ্রুবক অক্ষর *মান, size_t len,
                             ধ্রুবক অক্ষর *suffix, size_t slen)
{
  CURLcode ফলাফল = CURLE_OK;

  যদি(prefix) {
    /* If we have a zero-length value and the prefix ends with a space
       separator, drop the latter. */
    যদি(!len && plen && prefix[plen - 1] == ' ')
      plen--;
    ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, prefix, plen);
  }
  যদি(!ফলাফল && মান) {
    ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, মান, len);
  }
  যদি(!ফলাফল && suffix) {
    ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, suffix, slen);
  }
  ফেরত ফলাফল;
}

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
স্থির bool oldap_str_needs_base64(ধ্রুবক অক্ষর *s, size_t len)
{
  size_t i;
  যদি(!len)
    ফেরত FALSE;
  যদি((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    ফেরত TRUE;
  জন্য(i = 0; i < len; i++)
    যদি(!ISPRINT(s[i]))
      ফেরত TRUE;
  ফেরত FALSE;
}

স্থির CURLcode oldap_recv(কাঠামো Curl_easy *তথ্য, int8_t sockindex, অক্ষর *buf,
                           size_t len, size_t *pnread)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  কাঠামো ldapreqinfo *lr = Curl_meta_get(তথ্য, CURL_META_LDAP_EASY);
  পূর্ণসংখ্যা rc;
  LDAPMessage *বার্তা = ফাঁকা;
  BerElement *ber = ফাঁকা;
  কাঠামো timeval tv = { 0, 0 };
  কাঠামো berval bv, *bvals;
  CURLcode ফলাফল = CURLE_AGAIN;
  পূর্ণসংখ্যা code;
  অক্ষর *info = ফাঁকা;

  (শূন্য)len;
  (শূন্য)buf;
  (শূন্য)sockindex;
  *pnread = 0;
  যদি(!li || !lr)
    ফেরত CURLE_FAILED_INIT;

  rc = ldap_ফলাফল(li->ld, lr->msgid, LDAP_বার্তা_ONE, &tv, &বার্তা);
  যদি(rc < 0) {
    failf(তথ্য, "LDAP local: search ldap_result %s", ldap_err2string(rc));
    ফলাফল = CURLE_RECV_ERROR;
  }

  /* error or timed out */
  যদি(!বার্তা)
    ফেরত ফলাফল;

  ফলাফল = CURLE_OK;

  নির্বাচন(ldap_msgtype(বার্তা)) {
  ক্ষেত্র LDAP_RES_অনুসন্ধান_ফলাফল:
    lr->msgid = 0;
    rc = ldap_parse_ফলাফল(li->ld, বার্তা, &code, ফাঁকা, &info, ফাঁকা, ফাঁকা, 0);
    যদি(rc) {
      failf(তথ্য, "LDAP local: search ldap_parse_result %s",
            ldap_err2string(rc));
      ফলাফল = CURLE_LDAP_অনুসন্ধান_FAILED;
      বিরতি;
    }

    /* store the latest code for later retrieval */
    তথ্য->info.httpcode = code;

    নির্বাচন(code) {
    ক্ষেত্র LDAP_SIZELIMIT_EXCEEDED:
      infof(তথ্য, "There are more than %d entries", lr->nument);
      FALLTHROUGH();
    ক্ষেত্র LDAP_SUCCESS:
      তথ্য->req.আকার_2 = তথ্য->req.bytecount;
      বিরতি;
    পূর্বনির্ধারিত:
      failf(তথ্য, "LDAP remote: search failed %s %s", ldap_err2string(code),
            info ? info : "");
      ফলাফল = CURLE_LDAP_অনুসন্ধান_FAILED;
      বিরতি;
    }
    যদি(info)
      ldap_memfree(info);
    বিরতি;
  ক্ষেত্র LDAP_RES_অনুসন্ধান_ENTRY:
    lr->nument++;
    rc = ldap_get_dn_ber(li->ld, বার্তা, &ber, &bv);
    যদি(rc < 0) {
      ফলাফল = CURLE_RECV_ERROR;
      বিরতি;
    }

    যদি(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
      অক্ষর *dn_b64 = ফাঁকা;
      size_t dn_b64_sz = 0;

      /* A DN with e.g. CR/LF would inject LDIF lines, so base64-encode it. */
      ফলাফল = curlx_base64_encode((uint8_t *)bv.bv_val, bv.bv_len,
                                   &dn_b64, &dn_b64_sz);
      যদি(!ফলাফল)
        ফলাফল = client_write(তথ্য, STRCONST("DN:: "), dn_b64, dn_b64_sz,
                              STRCONST("\n"));
      curlx_free(dn_b64);
    }
    অন্যথা
      ফলাফল = client_write(তথ্য, STRCONST("DN: "), bv.bv_val, bv.bv_len,
                            STRCONST("\n"));
    যদি(ফলাফল)
      বিরতি;

    জন্য(rc = ldap_get_attribute_ber(li->ld, বার্তা, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, বার্তা, ber, &bv, &bvals)) {
      পূর্ণসংখ্যা i;
      bool দ্বিমিক;

      যদি(!bv.bv_val)
        বিরতি;

      /* An attribute type has no base64 form in LDIF, so a type carrying
         control bytes (CR/LF etc.) would let a hostile server inject lines.
         A conformant type name never contains them, so reject it. */
      যদি(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
        failf(তথ্য, "LDAP local: attribute type contains illegal bytes");
        যদি(bvals)
          ber_memfree(bvals);
        ফলাফল = CURLE_WEIRD_SERVER_REPLY;
        বিরতি;
      }

      যদি(!bvals) {
        ফলাফল = client_write(তথ্য, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":\n"));
        যদি(ফলাফল)
          বিরতি;
        চালু;
      }

      দ্বিমিক = bv.bv_len > 7 &&
        curl_strnequal(bv.bv_val + bv.bv_len - 7, ";binary", 7);

      জন্য(i = 0; bvals[i].bv_val; i++) {
        bool binval = FALSE;

        ফলাফল = client_write(তথ্য, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":"));
        যদি(ফলাফল)
          বিরতি;

        যদি(!দ্বিমিক)
          binval = oldap_str_needs_base64(bvals[i].bv_val, bvals[i].bv_len);
        যদি(দ্বিমিক || binval) {
          অক্ষর *val_b64 = ফাঁকা;
          size_t val_b64_sz = 0;

          /* Binary value, encode to base64. */
          যদি(bvals[i].bv_len)
            ফলাফল = curlx_base64_encode((uint8_t *)bvals[i].bv_val,
                                         bvals[i].bv_len,
                                         &val_b64, &val_b64_sz);
          যদি(!ফলাফল)
            ফলাফল = client_write(তথ্য, STRCONST(": "), val_b64, val_b64_sz,
                                  STRCONST("\n"));
          curlx_free(val_b64);
        }
        অন্যথা
          ফলাফল = client_write(তথ্য, STRCONST(" "),
                                bvals[i].bv_val, bvals[i].bv_len,
                                STRCONST("\n"));
        যদি(ফলাফল)
          বিরতি;
      }

      ber_memfree(bvals);
      bvals = ফাঁকা;
      যদি(!ফলাফল)
        ফলাফল = client_write(তথ্য, STRCONST("\n"), ফাঁকা, 0, ফাঁকা, 0);
      যদি(ফলাফল)
        বিরতি;
    }

    যদি(!ফলাফল)
      ফলাফল = client_write(তথ্য, STRCONST("\n"), ফাঁকা, 0, ফাঁকা, 0);
    যদি(!ফলাফল)
      ফলাফল = CURLE_AGAIN;
    বিরতি;
  }

  ber_free(ber, 0);
  ldap_msgfree(বার্তা);
  ফেরত ফলাফল;
}

শূন্য Curl_ldap_version(অক্ষর *buf, size_t bufsz)
{
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  যদি(ldap_get_option(ফাঁকা, LDAP_OPT_API_INFO, &api) == LDAP_OPT_SUCCESS) {
    unsigned পূর্ণসংখ্যা patch = (unsigned পূর্ণসংখ্যা)(api.ldapai_vendor_version % 100);
    unsigned পূর্ণসংখ্যা major = (unsigned পূর্ণসংখ্যা)(api.ldapai_vendor_version / 10000);
    unsigned পূর্ণসংখ্যা minor =
      (((unsigned পূর্ণসংখ্যা)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u",
                   api.ldapai_vendor_name, major, minor, patch);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((শূন্য **)api.ldapai_extensions);
  }
  অন্যথা
    curl_msnprintf(buf, bufsz, "OpenLDAP");
}

/*
 * LDAP protocol handler.
 */
ধ্রুবক কাঠামো Curl_protocol Curl_protocol_ldap = {
  oldap_setup_connection,               /* setup_connection */
  oldap_do,                             /* do_it */
  oldap_done,                           /* done */
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

#endif /* !CURL_DISABLE_LDAP && USE_OPENLDAP */

/* The LDAP scheme structs are in ldap.c */
