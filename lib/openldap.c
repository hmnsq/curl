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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP)

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

#ଅନ୍ତର୍ଭାଵ <ldap.h>

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "curl_ldap.h"
#ଅନ୍ତର୍ଭାଵ "curlx/base64.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "curl_sasl.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "bufref.h"

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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
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
ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ldap_pvt_url_scheme2proto(ଅଚଳ ଅକ୍ଷର *);
ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ldap_init_fd(ber_socket_t fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା proto, ଅଚଳ ଅକ୍ଷର *url,
                        LDAP **ld);
#endif

ସ୍ଥିର Curl_recv oldap_recv;

ସଂରଚନା ldapconninfo {
  ସଂରଚନା SASL sasl;          /* SASL-related parameters */
  LDAP *ld;                  /* Openldap connection handle. */
  Curl_recv *recv;           /* For stacking SSL handler */
  Curl_send *send;
  ସଂରଚନା berval *servercred; /* SASL data from server. */
  ldapstate state;           /* Current machine state. */
  curl_socket_t dupfd;       /* duplicated socket */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା proto;                 /* LDAP_PROTO_TCP/LDAP_PROTO_UDP/LDAP_PROTO_IPC */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା msgid;                 /* Current message id. */
};

ସଂରଚନା ldapreqinfo {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା msgid;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା nument;
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
ସ୍ଥିର ଶୂନ୍ୟ oldap_state(ସଂରଚନା Curl_easy *data, ସଂରଚନା ldapconninfo *li,
                        ldapstate newstate)
{
#ଯଦି defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ names[] = {
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

  ଯଦି(li->state != newstate)
    infof(data, "LDAP %p state change from %s to %s",
          (ଶୂନ୍ୟ *)li, names[li->state], names[newstate]);
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)data;
#endif
  li->state = newstate;
}

/* Map some particular LDAP error codes to CURLcode values. */
ସ୍ଥିର CURLcode oldap_map_error(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, CURLcode result)
{
  ବାଛ(rc) {
  ସ୍ଥିତି LDAP_NO_MEMORY:
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  ସ୍ଥିତି LDAP_INVALID_CREDENTIALS:
    ଫେରନ୍ତୁ CURLE_LOGIN_DENIED;
  ସ୍ଥିତି LDAP_PROTOCOL_ERROR:
    ଫେରନ୍ତୁ CURLE_UNSUPPORTED_PROTOCOL;
  ସ୍ଥିତି LDAP_INSUFFICIENT_ACCESS:
    ଫେରନ୍ତୁ CURLE_REMOTE_ACCESS_DENIED;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode oldap_url_parse(ସଂରଚନା Curl_easy *data, LDAPURLDesc **ludp)
{
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ url_errs[] = {
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

  *ludp = ଖାଲି;
  /* `ldap_url_parse() seems to be terrible with urls
   * that have user/pass/options in it. So when we have options or
   * creds from the url, fail without calling the function.
   * Yes, this is super-weird code and I do not like it. */
  ଯଦି((data->state.creds && (data->state.creds->source == CREDS_URL)) ||
     data->state.up.options)
    rc = LDAP_URL_ERR_BADURL;
  ଅନ୍ୟଥା
    rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), ludp);

  ଯଦି(rc != LDAP_URL_SUCCESS) {
    ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ = "url parsing problem";

    result = rc == LDAP_URL_ERR_MEM ? CURLE_OUT_OF_MEMORY :
      CURLE_URL_MALFORMAT;
    rc -= LDAP_URL_SUCCESS;
    ଯଦି((size_t)rc < CURL_ARRAYSIZE(url_errs))
      ସନ୍ଦେଶ = url_errs[rc];
    failf(data, "LDAP local: %s", ସନ୍ଦେଶ);
  }
  ଫେରନ୍ତୁ result;
}

/* Parse the login options. */
ସ୍ଥିର CURLcode oldap_parse_login_options(ସଂରଚନା connectdata *conn)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ଅଚଳ ଅକ୍ଷର *ptr = conn->options;

  DEBUGASSERT(li);
  ଯଦି(!li)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯେତେବେଳେ(!result && ptr && *ptr) {
    ଅଚଳ ଅକ୍ଷର *ଚାବି = ptr;
    ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ;

    ଯେତେବେଳେ(*ptr && *ptr != '=')
      ptr++;

    ମୂଲ୍ୟ = ptr + 1;

    ଯେତେବେଳେ(*ptr && *ptr != ';')
      ptr++;

    ଯଦି(checkprefix("AUTH=", ଚାବି))
      result = Curl_sasl_parse_url_auth_option(&li->sasl, ମୂଲ୍ୟ, ptr - ମୂଲ୍ୟ);
    ଅନ୍ୟଥା
      result = CURLE_SETOPT_OPTION_SYNTAX;

    ଯଦି(*ptr == ';')
      ptr++;
  }

  ଫେରନ୍ତୁ result == CURLE_URL_MALFORMAT ? CURLE_SETOPT_OPTION_SYNTAX : result;
}

ସ୍ଥିର CURLcode oldap_setup_connection(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା connectdata *conn)
{
  CURLcode result;
  LDAPURLDesc *lud;
  (ଶୂନ୍ୟ)conn;

  /* Early URL syntax check. */
  result = oldap_url_parse(data, &lud);
  ldap_free_urldesc(lud);

  ଫେରନ୍ତୁ result;
}

/*
 * Get the SASL authentication challenge from the server credential buffer.
 */
ସ୍ଥିର CURLcode oldap_get_message(ସଂରଚନା Curl_easy *data, ସଂରଚନା bufref *out)
{
  ସଂରଚନା ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  ସଂରଚନା berval *servercred = li ? li->servercred : ଖାଲି;
  DEBUGASSERT(li);
  ଯଦି(!li)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯଦି(!servercred || !servercred->bv_val)
    ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
  Curl_bufref_set(out, servercred->bv_val, servercred->bv_len, ଖାଲି);
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Sends an initial SASL bind request to the server.
 */
ସ୍ଥିର CURLcode oldap_perform_auth(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *mech,
                                   ଅଚଳ ସଂରଚନା bufref *initresp)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ସଂରଚନା berval cred;
  ସଂରଚନା berval *pcred = &cred;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  DEBUGASSERT(li);
  ଯଦି(!li)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  cred.bv_val = (ଅକ୍ଷର *)CURL_UNCONST(Curl_bufref_ptr(initresp));
  cred.bv_len = Curl_bufref_len(initresp);
  ଯଦି(!cred.bv_val)
    pcred = ଖାଲି;
  rc = ldap_sasl_bind(li->ld, ଖାଲି, mech, pcred, ଖାଲି, ଖାଲି, &li->msgid);
  ଯଦି(rc != LDAP_SUCCESS)
    ଫେରନ୍ତୁ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Sends SASL continuation.
 */
ସ୍ଥିର CURLcode oldap_continue_auth(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *mech,
                                    ଅଚଳ ସଂରଚନା bufref *resp)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ସଂରଚନା berval cred;
  ସଂରଚନା berval *pcred = &cred;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  ଯଦି(!li)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  cred.bv_val = (ଅକ୍ଷର *)CURL_UNCONST(Curl_bufref_ptr(resp));
  cred.bv_len = Curl_bufref_len(resp);
  ଯଦି(!cred.bv_val)
    pcred = ଖାଲି;
  rc = ldap_sasl_bind(li->ld, ଖାଲି, mech, pcred, ଖାଲି, ଖାଲି, &li->msgid);
  ଯଦି(rc != LDAP_SUCCESS)
    ଫେରନ୍ତୁ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Sends SASL bind cancellation.
 */
ସ୍ଥିର CURLcode oldap_cancel_auth(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *mech)
{
  ସଂରଚନା ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  (ଶୂନ୍ୟ)mech;
  ଯଦି(!li)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  rc = ldap_sasl_bind(li->ld, ଖାଲି, LDAP_SASL_NULL, ଖାଲି, ଖାଲି, ଖାଲି,
                      &li->msgid);
  ଯଦି(rc != LDAP_SUCCESS)
    ଫେରନ୍ତୁ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ଫେରନ୍ତୁ CURLE_OK;
}

/* Starts LDAP simple bind. */
ସ୍ଥିର CURLcode oldap_perform_bind(ସଂରଚନା Curl_easy *data, ldapstate newstate)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ଅଚଳ ଅକ୍ଷର *binddn = ଖାଲି;
  ସଂରଚନା berval passwd;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  ଯଦି(!li)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  passwd.bv_val = ଖାଲି;
  passwd.bv_len = 0;

  ଯଦି(conn->creds) {
    binddn = Curl_creds_user(conn->creds);
    passwd.bv_val = CURL_UNCONST(Curl_creds_passwd(conn->creds));
    passwd.bv_len = strlen(passwd.bv_val);
  }

  rc = ldap_sasl_bind(li->ld, binddn, LDAP_SASL_SIMPLE, &passwd,
                      ଖାଲି, ଖାଲି, &li->msgid);
  ଯଦି(rc != LDAP_SUCCESS)
    ଫେରନ୍ତୁ oldap_map_error(rc,
                           data->state.creds ?
                           CURLE_LOGIN_DENIED : CURLE_LDAP_CANNOT_BIND);
  oldap_state(data, li, newstate);
  ଫେରନ୍ତୁ CURLE_OK;
}

/* Query the supported SASL authentication mechanisms. */
ସ୍ଥିର CURLcode oldap_perform_mechs(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ supportedSASLMechanisms[] = {
    "supportedSASLMechanisms",
    ଖାଲି
  };

  ଯଦି(!li)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  /* Casting away the const for the 3rd parameter that the LDAP API expects as
     a non-const char ** is potentially unsafe but we believe the lack of
     const in the API was an oversight and that no LDAP implementation
     actually modifies the input. */
  rc = ldap_ସନ୍ଧାନ_ext(li->ld, "", LDAP_SCOPE_BASE, "(objectclass=*)",
                       (ଅକ୍ଷର **)CURL_UNCONST(supportedSASLMechanisms), 0,
                       ଖାଲି, ଖାଲି, ଖାଲି, 0, &li->msgid);
  ଯଦି(rc != LDAP_SUCCESS)
    ଫେରନ୍ତୁ oldap_map_error(rc, CURLE_LOGIN_DENIED);
  oldap_state(data, li, OLDAP_MECHS);
  ଫେରନ୍ତୁ CURLE_OK;
}

/* Starts SASL bind. */
ସ୍ଥିର CURLcode oldap_perform_sasl(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  saslprogress progress = SASL_IDLE;
  CURLcode result;

  ଯଦି(!li)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  result = Curl_sasl_start(&li->sasl, data, TRUE, &progress);

  oldap_state(data, li, OLDAP_SASL);
  ଯଦି(!result && progress != SASL_INPROGRESS)
    result = Curl_sasl_is_blocked(&li->sasl, data);
  ଫେରନ୍ତୁ result;
}

#ifdef USE_SSL
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ldapsb_tls_setup(Sockbuf_IO_Desc *sbiod, ଶୂନ୍ୟ *arg)
{
  sbiod->sbiod_pvt = arg;
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ldapsb_tls_remove(Sockbuf_IO_Desc *sbiod)
{
  sbiod->sbiod_pvt = ଖାଲି;
  ଫେରନ୍ତୁ 0;
}

/* if there is a duplicated socket to close, close it */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ldapsb_tls_close(Sockbuf_IO_Desc *sbiod)
{
  ସଂରଚନା Curl_easy *data = sbiod->sbiod_pvt;
  ଯଦି(data) {
    ସଂରଚନା connectdata *conn = data->conn;
    ଯଦି(conn) {
      ସଂରଚନା ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      ଯଦି(li && (li->dupfd != CURL_SOCKET_BAD)) {
        sclose(li->dupfd);
        li->dupfd = CURL_SOCKET_BAD;
      }
    }
  }

  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ldapsb_tls_ctrl(Sockbuf_IO_Desc *sbiod, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt, ଶୂନ୍ୟ *arg)
{
  (ଶୂନ୍ୟ)arg;
  ଯଦି(opt == LBER_SB_OPT_DATA_READY) {
    ସଂରଚନା Curl_easy *data = sbiod->sbiod_pvt;
    ଫେରନ୍ତୁ Curl_conn_data_pending(data, FIRSTSOCKET);
  }
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ber_slen_t ldapsb_tls_read(Sockbuf_IO_Desc *sbiod, ଶୂନ୍ୟ *buf,
                                  ber_len_t len)
{
  ସଂରଚନା Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  ଯଦି(data) {
    ସଂରଚନା connectdata *conn = data->conn;
    ଯଦି(conn) {
      ସଂରଚନା ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_RECV_ERROR;
      size_t nread;

      ଯଦି(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        ଫେରନ୍ତୁ -1;
      }
      result = (li->recv)(data, FIRSTSOCKET, buf, len, &nread);
      ଯଦି(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nread;
    }
  }
  ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ber_slen_t ldapsb_tls_write(Sockbuf_IO_Desc *sbiod, ଶୂନ୍ୟ *buf,
                                   ber_len_t len)
{
  ସଂରଚନା Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  ଯଦି(data) {
    ସଂରଚନା connectdata *conn = data->conn;
    ଯଦି(conn) {
      ସଂରଚନା ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_SEND_ERROR;
      size_t nwritten;

      ଯଦି(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        ଫେରନ୍ତୁ -1;
      }
      result = (li->send)(data, FIRSTSOCKET, buf, len, FALSE, &nwritten);
      ଯଦି(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nwritten;
    }
  }
  ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର Sockbuf_IO ldapsb_tls = {
  ldapsb_tls_setup,
  ldapsb_tls_remove,
  ldapsb_tls_ctrl,
  ldapsb_tls_read,
  ldapsb_tls_write,
  ldapsb_tls_close
};

ସ୍ଥିର bool ssl_installed(ସଂରଚନା connectdata *conn)
{
  ସଂରଚନା ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ଫେରନ୍ତୁ li && li->recv;
}

ସ୍ଥିର CURLcode oldap_ssl_connect(ସଂରଚନା Curl_easy *data, ldapstate newstate)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  bool ssldone = FALSE;
  CURLcode result;

  ଯଦି(!li)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  oldap_state(data, li, newstate);

  ଯଦି(ssldone) {
    Sockbuf *sb;

    /* Install the libcurl SSL handlers into the sockbuf. */
    ଯଦି((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;
    li->recv = conn->recv[FIRSTSOCKET];
    li->send = conn->send[FIRSTSOCKET];
  }

  ଫେରନ୍ତୁ result;
}

/* Send the STARTTLS request */
ସ୍ଥିର CURLcode oldap_perform_starttls(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  ଯଦି(!li)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  rc = ldap_start_tls(li->ld, ଖାଲି, ଖାଲି, &li->msgid);
  ଯଦି(rc != LDAP_SUCCESS)
    ଫେରନ୍ତୁ oldap_map_error(rc, CURLE_USE_SSL_FAILED);
  oldap_state(data, li, OLDAP_STARTTLS);
  ଫେରନ୍ତୁ CURLE_OK;
}
#endif

ସ୍ଥିର ଶୂନ୍ୟ oldap_easy_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା ldapreqinfo *lr = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  curlx_free(lr);
}

ସ୍ଥିର ଶୂନ୍ୟ oldap_conn_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା ldapconninfo *li = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  ଯଦି(li->ld) {
    ldap_unbind_ext(li->ld, ଖାଲି, ଖାଲି);
    li->ld = ଖାଲି;
  }
  curlx_free(li);
}

/* SASL parameters for the ldap protocol */
ସ୍ଥିର ଅଚଳ ସଂରଚନା SASLproto saslldap = {
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

ସ୍ଥିର CURLcode oldap_connect(ସଂରଚନା Curl_easy *data, bool *done)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ldapconninfo *li;
  ସ୍ଥିର ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା version = LDAP_VERSION3;
  ଅକ୍ଷର *hosturl = ଖାଲି;
  CURLcode result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
#ifdef CURL_OPENLDAP_DEBUG
  ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_trace = -1;
#endif

  (ଶୂନ୍ୟ)done;

  li = curlx_calloc(1, ଆକାର(ସଂରଚନା ldapconninfo));
  ଯଦି(!li) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }
  li->dupfd = CURL_SOCKET_BAD;

  result = Curl_conn_meta_set(conn, CURL_META_LDAP_CONN, li, oldap_conn_dtor);
  ଯଦି(result)
    ଯାଅ out;

  li->proto = ldap_pvt_url_scheme2proto(data->state.origin->scheme->name);

  /* Initialize the SASL storage */
  Curl_sasl_init(&li->sasl, data, &saslldap);

  result = oldap_parse_login_options(conn);
  ଯଦି(result)
    ଯାଅ out;

  hosturl = curl_maprintf("%s://%s:%u",
                          conn->scheme->name,
                          conn->origin->ipv6 ?
                          conn->origin->user_hostname :
                          conn->origin->hostname,
                          conn->origin->port);
  ଯଦି(!hosturl) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  {
#ifdef _WIN32
    SOCKET dupfd = INVALID_SOCKET;
    WSAPROTOCOL_INFO pi;
    ଯଦି(WSADuplicateSocket(conn->sock[FIRSTSOCKET], GetCurrentProcessId(),
                          &pi))
      ; /* error */
    ଅନ୍ୟଥା
      dupfd = CURL_SOCKET(pi.iAddressFamily, pi.iSocketType, pi.iProtocol);
#ଅନ୍ୟଥା
#ifdef F_DUPFD_CLOEXEC
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା dupfd = fcntl(conn->sock[FIRSTSOCKET], F_DUPFD_CLOEXEC, 0);
#ଅନ୍ୟଥା
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା dupfd = dup(conn->sock[FIRSTSOCKET]);
#ifdef HAVE_FCNTL
    ଯଦି((dupfd != CURL_SOCKET_BAD) &&
       (fcntl(dupfd, F_SETFD, FD_CLOEXEC) < 0)) {
      sclose(dupfd);
      dupfd = CURL_SOCKET_BAD;
    }
#endif /* HAVE_FCNTL */
#endif /* F_DUPFD_CLOEXEC */
#endif /* _WIN32 */

    ଯଦି(dupfd == CURL_SOCKET_BAD) {
      result = CURLE_COULDNT_CONNECT;
      ଯାଅ out;
    }
    rc = ldap_init_fd((ber_socket_t)dupfd, li->proto, hosturl, &li->ld);
    ଯଦି(rc)
      sclose(dupfd);
    ଅନ୍ୟଥା
      /* store the socket, as when this is used by TLS we close this ourselves
         and at this point the code does not yet know if this will use TLS or
         not */
      li->dupfd = dupfd;
  }
  ଯଦି(rc) {
    failf(data, "LDAP local: Cannot connect to %s, %s",
          hosturl, ldap_err2string(rc));
    result = CURLE_COULDNT_CONNECT;
    ଯାଅ out;
  }

#ifdef CURL_OPENLDAP_DEBUG
  ଯଦି(do_trace < 0) {
    ଅଚଳ ଅକ୍ଷର *env = getenv("CURL_OPENLDAP_TRACE");
    curl_off_t e = 0;
    ଯଦି(!curlx_str_ସଂଖ୍ୟା(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  ଯଦି(do_trace)
    ldap_set_option(li->ld, LDAP_OPT_DEBUG_LEVEL, &do_trace);
#endif

  /* Try version 3 first. */
  ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);

  /* Do not chase referrals. */
  ldap_set_option(li->ld, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  {
    ber_len_t max = 256 * 1024;
    Sockbuf *sb;
    ଯଦି((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       /* Set the maximum allowed size of an incoming message, which to
          OpenLDAP means that it will malloc() memory up to this size. If not
          set, there is no limit and we instead risk a malloc() failure. */
       !ber_sockbuf_ctrl(sb, LBER_SB_OPT_SET_MAX_INCOMING, &max)) {
      result = CURLE_FAILED_INIT;
      ଯାଅ out;
    }
  }

#ifdef USE_SSL
  ଯଦି(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = oldap_ssl_connect(data, OLDAP_SSL);
    ଯାଅ out;
  }

  ଯଦି(data->set.use_ssl) {
    result = oldap_perform_starttls(data);
    ଯଦି(!result || data->set.use_ssl != CURLUSESSL_TRY)
      ଯାଅ out;
  }
#endif

  ଯଦି(li->sasl.prefmech != SASL_AUTH_NONE) {
    result = oldap_perform_mechs(data);
    ଯାଅ out;
  }

  /* Force bind even if anonymous bind is not needed in protocol version 3
     to detect missing version 3 support. */
  result = oldap_perform_bind(data, OLDAP_BIND);

out:
  curlx_free(hosturl);
  ଫେରନ୍ତୁ result;
}

/* Handle the supported SASL mechanisms query response */
ସ୍ଥିର CURLcode oldap_state_mechs_resp(ସଂରଚନା Curl_easy *data,
                                       LDAPMessage *ସନ୍ଦେଶ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
  ସଂରଚନା connectdata *conn;
  ସଂରଚନା ldapconninfo *li;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  BerElement *ber = ଖାଲି;
  CURLcode result = CURLE_OK;
  ସଂରଚନା berval bv, *bvals;

  ଯଦି(!data)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  conn = data->conn;
  li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);

  ଯଦି(!li)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ବାଛ(ldap_msgtype(ସନ୍ଦେଶ)) {
  ସ୍ଥିତି LDAP_RES_ସନ୍ଧାନ_ENTRY:
    /* Got a list of supported SASL mechanisms. */
    ଯଦି(code != LDAP_SUCCESS && code != LDAP_NO_RESULTS_RETURNED)
      ଫେରନ୍ତୁ CURLE_LOGIN_DENIED;

    rc = ldap_get_dn_ber(li->ld, ସନ୍ଦେଶ, &ber, &bv);
    ଯଦି(rc < 0)
      ଫେରନ୍ତୁ oldap_map_error(rc, CURLE_BAD_CONTENT_ENCODING);
    ପାଇଁ(rc = ldap_get_attribute_ber(li->ld, ସନ୍ଦେଶ, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, ସନ୍ଦେଶ, ber, &bv, &bvals)) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

      ଯଦି(!bv.bv_val)
        ଭାଙ୍ଗ;

      ଯଦି(bvals) {
        ପାଇଁ(i = 0; bvals[i].bv_val; i++) {
          size_t llen;
          unsigned short mech =
            Curl_sasl_decode_mech((ଅଚଳ ଅକ୍ଷର *)bvals[i].bv_val,
                                  bvals[i].bv_len, &llen);
          ଯଦି(bvals[i].bv_len == llen)
            li->sasl.authmechs |= mech;
        }
        ber_memfree(bvals);
      }
    }
    ber_free(ber, 0);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି LDAP_RES_ସନ୍ଧାନ_RESULT:
    ବାଛ(code) {
    ସ୍ଥିତି LDAP_SIZELIMIT_EXCEEDED:
      infof(data, "Too many authentication mechanisms");
      FALLTHROUGH();
    ସ୍ଥିତି LDAP_SUCCESS:
    ସ୍ଥିତି LDAP_NO_RESULTS_RETURNED:
      ଯଦି(Curl_sasl_can_authenticate(&li->sasl, data))
        result = oldap_perform_sasl(data);
      ଅନ୍ୟଥା
        result = CURLE_LOGIN_DENIED;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      result = oldap_map_error(code, CURLE_LOGIN_DENIED);
      ଭାଙ୍ଗ;
    }
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ result;
}

/* Handle a SASL bind response. */
ସ୍ଥିର CURLcode oldap_state_sasl_resp(ସଂରଚନା Curl_easy *data,
                                      LDAPMessage *ସନ୍ଦେଶ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode result = CURLE_OK;
  saslprogress progress;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  ଯଦି(!li)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  li->servercred = ଖାଲି;
  rc = ldap_parse_sasl_bind_result(li->ld, ସନ୍ଦେଶ, &li->servercred, 0);
  ଯଦି(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: sasl ldap_parse_result %s", ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LOGIN_DENIED);
  }
  ଅନ୍ୟଥା {
    result = Curl_sasl_continue(&li->sasl, data, code, &progress);
    ଯଦି(!result) {
      ବାଛ(progress) {
      ସ୍ଥିତି SASL_DONE:
        oldap_state(data, li, OLDAP_STOP);   /* Authenticated */
        ଭାଙ୍ଗ;
      ସ୍ଥିତି SASL_IDLE:            /* No mechanism left after cancellation */
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଭାଙ୍ଗ;
      }
    }
  }

  ଯଦି(li->servercred)
    ber_bvfree(li->servercred);
  ଫେରନ୍ତୁ result;
}

/* Handle a simple bind response. */
ସ୍ଥିର CURLcode oldap_state_bind_resp(ସଂରଚନା Curl_easy *data, LDAPMessage *ସନ୍ଦେଶ,
                                      ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode result = CURLE_OK;
  ସଂରଚନା berval *bv = ଖାଲି;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  ଯଦି(!li)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯଦି(code != LDAP_SUCCESS)
    ଫେରନ୍ତୁ oldap_map_error(code, CURLE_LDAP_CANNOT_BIND);

  rc = ldap_parse_sasl_bind_result(li->ld, ସନ୍ଦେଶ, &bv, 0);
  ଯଦି(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: bind ldap_parse_sasl_bind_result %s",
          ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  }
  ଅନ୍ୟଥା
    oldap_state(data, li, OLDAP_STOP);

  ଯଦି(bv)
    ber_bvfree(bv);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode oldap_connecting(ସଂରଚନା Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  LDAPMessage *ସନ୍ଦେଶ = ଖାଲି;
  ସଂରଚନା timeval tv = { 0, 0 };
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା code = LDAP_SUCCESS;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  ଯଦି(!li)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯଦି(li->state != OLDAP_SSL && li->state != OLDAP_TLS) {
    /* Get response to last command. */
    rc = ldap_result(li->ld, li->msgid, LDAP_ସନ୍ଦେଶ_ONE, &tv, &ସନ୍ଦେଶ);
    ବାଛ(rc) {
    ସ୍ଥିତି 0:                               /* Timed out. */
      ଫେରନ୍ତୁ CURLE_OK;
    ସ୍ଥିତି LDAP_RES_ସନ୍ଧାନ_ENTRY:
    ସ୍ଥିତି LDAP_RES_ସନ୍ଧାନ_REFERENCE:
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      li->msgid = 0;                      /* Nothing to abandon upon error. */
      ଯଦି(rc < 0) {
        failf(data, "LDAP local: connecting ldap_result %s",
              ldap_err2string(rc));
        ଫେରନ୍ତୁ oldap_map_error(rc, CURLE_COULDNT_CONNECT);
      }
      ଭାଙ୍ଗ;
    }

    /* Get error code from message. */
    rc = ldap_parse_result(li->ld, ସନ୍ଦେଶ, &code, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, 0);
    ଯଦି(rc)
      code = rc;
    ଅନ୍ୟଥା {
      /* store the latest code for later retrieval */
      data->info.httpcode = code;
    }

    /* If protocol version 3 is not supported, fallback to version 2. */
    ଯଦି(code == LDAP_PROTOCOL_ERROR && li->state != OLDAP_BINDV2 &&
#ifdef USE_SSL
       (ssl_installed(conn) || data->set.use_ssl <= CURLUSESSL_TRY) &&
#endif
       li->sasl.prefmech == SASL_AUTH_NONE) {
      ସ୍ଥିର ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା version = LDAP_VERSION2;

      ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);
      ldap_msgfree(ସନ୍ଦେଶ);
      ଫେରନ୍ତୁ oldap_perform_bind(data, OLDAP_BINDV2);
    }
  }

  /* Handle response message according to current state. */
  ବାଛ(li->state) {

#ifdef USE_SSL
  ସ୍ଥିତି OLDAP_SSL:
    result = oldap_ssl_connect(data, OLDAP_SSL);
    ଯଦି(!result && ssl_installed(conn)) {
      ଯଦି(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      ଅନ୍ୟଥା
        result = oldap_perform_bind(data, OLDAP_BIND);
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି OLDAP_STARTTLS:
    ଯଦି(code != LDAP_SUCCESS) {
      ଯଦି(data->set.use_ssl != CURLUSESSL_TRY)
        result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
      ଅନ୍ୟଥା ଯଦି(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      ଅନ୍ୟଥା
        result = oldap_perform_bind(data, OLDAP_BIND);
      ଭାଙ୍ଗ;
    }
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ଯଦି(result)
      ଭାଙ୍ଗ;
    FALLTHROUGH();
  ସ୍ଥିତି OLDAP_TLS:
    result = oldap_ssl_connect(data, OLDAP_TLS);
    ଯଦି(result)
      result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
    ଅନ୍ୟଥା ଯଦି(ssl_installed(conn)) {
      ଯଦି(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      ଅନ୍ୟଥା ଯଦି(data->state.creds)
        result = oldap_perform_bind(data, OLDAP_BIND);
      ଅନ୍ୟଥା {
        /* Version 3 supported: no bind required */
        oldap_state(data, li, OLDAP_STOP);
        result = CURLE_OK;
      }
    }
    ଭାଙ୍ଗ;
#endif

  ସ୍ଥିତି OLDAP_MECHS:
    result = oldap_state_mechs_resp(data, ସନ୍ଦେଶ, code);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି OLDAP_SASL:
    result = oldap_state_sasl_resp(data, ସନ୍ଦେଶ, code);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି OLDAP_BIND:
  ସ୍ଥିତି OLDAP_BINDV2:
    result = oldap_state_bind_resp(data, ସନ୍ଦେଶ, code);
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    /* internal error */
    result = CURLE_COULDNT_CONNECT;
    ଭାଙ୍ଗ;
  }

  ldap_msgfree(ସନ୍ଦେଶ);

  *done = li->state == OLDAP_STOP;
  ଯଦି(*done)
    conn->recv[FIRSTSOCKET] = oldap_recv;

  ଯଦି(result && li->msgid) {
    ldap_abandon_ext(li->ld, li->msgid, ଖାଲି, ଖାଲି);
    li->msgid = 0;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode oldap_disconnect(ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା connectdata *conn,
                                 bool dead_connection)
{
  ସଂରଚନା ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  (ଶୂନ୍ୟ)dead_connection;
#ifndef USE_SSL
  (ଶୂନ୍ୟ)data;
#endif

  ଯଦି(li && li->ld) {
#ifdef USE_SSL
    ଯଦି(ssl_installed(conn)) {
      Sockbuf *sb;
      ଯଦି(ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS ||
         ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
        ଫେରନ୍ତୁ CURLE_FAILED_INIT;
    }
#endif
    ldap_unbind_ext(li->ld, ଖାଲି, ଖାଲି);
    li->ld = ଖାଲି;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode oldap_do(ସଂରଚନା Curl_easy *data, bool *done)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ସଂରଚନା ldapreqinfo *lr;
  CURLcode result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  LDAPURLDesc *lud;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା msgid;

  ଯଦି(!li)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

  result = oldap_url_parse(data, &lud);
  ଯଦି(result)
    ଯାଅ out;

#ifdef USE_SSL
  ଯଦି(ssl_installed(conn)) {
    Sockbuf *sb;
    /* re-install the libcurl SSL handlers into the sockbuf. */
    ଯଦି((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data)) {
      ldap_free_urldesc(lud);
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;
    }
  }
#endif

  rc = ldap_ସନ୍ଧାନ_ext(li->ld, lud->lud_dn, lud->lud_scope,
                       lud->lud_filter, lud->lud_attrs, 0,
                       ଖାଲି, ଖାଲି, ଖାଲି, 0, &msgid);
  ldap_free_urldesc(lud);
  ଯଦି(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: ldap_search_ext %s", ldap_err2string(rc));
    result = CURLE_LDAP_ସନ୍ଧାନ_FAILED;
    ଯାଅ out;
  }

  lr = curlx_calloc(1, ଆକାର(ସଂରଚନା ldapreqinfo));
  ଯଦି(!lr ||
     Curl_meta_set(data, CURL_META_LDAP_EASY, lr, oldap_easy_dtor)) {
    ldap_abandon_ext(li->ld, msgid, ଖାଲି, ଖାଲି);
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  lr->msgid = msgid;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  *done = TRUE;

out:
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode oldap_done(ସଂରଚନା Curl_easy *data, CURLcode res,
                           bool premature)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);

  (ଶୂନ୍ୟ)res;
  (ଶୂନ୍ୟ)premature;

  ଯଦି(lr) {
    /* if there was a search in progress, abandon it */
    ଯଦି(lr->msgid) {
      ସଂରଚନା ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      ଯଦି(li && li->ld) {
        ldap_abandon_ext(li->ld, lr->msgid, ଖାଲି, ଖାଲି);
      }
      lr->msgid = 0;
    }
    Curl_meta_remove(data, CURL_META_LDAP_EASY);
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode client_write(ସଂରଚନା Curl_easy *data,
                             ଅଚଳ ଅକ୍ଷର *prefix, size_t plen,
                             ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, size_t len,
                             ଅଚଳ ଅକ୍ଷର *suffix, size_t slen)
{
  CURLcode result = CURLE_OK;

  ଯଦି(prefix) {
    /* If we have a zero-length value and the prefix ends with a space
       separator, drop the latter. */
    ଯଦି(!len && plen && prefix[plen - 1] == ' ')
      plen--;
    result = Curl_client_write(data, CLIENTWRITE_BODY, prefix, plen);
  }
  ଯଦି(!result && ମୂଲ୍ୟ) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, ମୂଲ୍ୟ, len);
  }
  ଯଦି(!result && suffix) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, suffix, slen);
  }
  ଫେରନ୍ତୁ result;
}

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
ସ୍ଥିର bool oldap_str_needs_base64(ଅଚଳ ଅକ୍ଷର *s, size_t len)
{
  size_t i;
  ଯଦି(!len)
    ଫେରନ୍ତୁ FALSE;
  ଯଦି((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    ଫେରନ୍ତୁ TRUE;
  ପାଇଁ(i = 0; i < len; i++)
    ଯଦି(!ISPRINT(s[i]))
      ଫେରନ୍ତୁ TRUE;
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର CURLcode oldap_recv(ସଂରଚନା Curl_easy *data, int8_t sockindex, ଅକ୍ଷର *buf,
                           size_t len, size_t *pnread)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ସଂରଚନା ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  LDAPMessage *ସନ୍ଦେଶ = ଖାଲି;
  BerElement *ber = ଖାଲି;
  ସଂରଚନା timeval tv = { 0, 0 };
  ସଂରଚନା berval bv, *bvals;
  CURLcode result = CURLE_AGAIN;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା code;
  ଅକ୍ଷର *info = ଖାଲି;

  (ଶୂନ୍ୟ)len;
  (ଶୂନ୍ୟ)buf;
  (ଶୂନ୍ୟ)sockindex;
  *pnread = 0;
  ଯଦି(!li || !lr)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  rc = ldap_result(li->ld, lr->msgid, LDAP_ସନ୍ଦେଶ_ONE, &tv, &ସନ୍ଦେଶ);
  ଯଦି(rc < 0) {
    failf(data, "LDAP local: search ldap_result %s", ldap_err2string(rc));
    result = CURLE_RECV_ERROR;
  }

  /* error or timed out */
  ଯଦି(!ସନ୍ଦେଶ)
    ଫେରନ୍ତୁ result;

  result = CURLE_OK;

  ବାଛ(ldap_msgtype(ସନ୍ଦେଶ)) {
  ସ୍ଥିତି LDAP_RES_ସନ୍ଧାନ_RESULT:
    lr->msgid = 0;
    rc = ldap_parse_result(li->ld, ସନ୍ଦେଶ, &code, ଖାଲି, &info, ଖାଲି, ଖାଲି, 0);
    ଯଦି(rc) {
      failf(data, "LDAP local: search ldap_parse_result %s",
            ldap_err2string(rc));
      result = CURLE_LDAP_ସନ୍ଧାନ_FAILED;
      ଭାଙ୍ଗ;
    }

    /* store the latest code for later retrieval */
    data->info.httpcode = code;

    ବାଛ(code) {
    ସ୍ଥିତି LDAP_SIZELIMIT_EXCEEDED:
      infof(data, "There are more than %d entries", lr->nument);
      FALLTHROUGH();
    ସ୍ଥିତି LDAP_SUCCESS:
      data->req.size = data->req.bytecount;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      failf(data, "LDAP remote: search failed %s %s", ldap_err2string(code),
            info ? info : "");
      result = CURLE_LDAP_ସନ୍ଧାନ_FAILED;
      ଭାଙ୍ଗ;
    }
    ଯଦି(info)
      ldap_memfree(info);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି LDAP_RES_ସନ୍ଧାନ_ENTRY:
    lr->nument++;
    rc = ldap_get_dn_ber(li->ld, ସନ୍ଦେଶ, &ber, &bv);
    ଯଦି(rc < 0) {
      result = CURLE_RECV_ERROR;
      ଭାଙ୍ଗ;
    }

    ଯଦି(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
      ଅକ୍ଷର *dn_b64 = ଖାଲି;
      size_t dn_b64_sz = 0;

      /* A DN with e.g. CR/LF would inject LDIF lines, so base64-encode it. */
      result = curlx_base64_encode((uint8_t *)bv.bv_val, bv.bv_len,
                                   &dn_b64, &dn_b64_sz);
      ଯଦି(!result)
        result = client_write(data, STRCONST("DN:: "), dn_b64, dn_b64_sz,
                              STRCONST("\n"));
      curlx_free(dn_b64);
    }
    ଅନ୍ୟଥା
      result = client_write(data, STRCONST("DN: "), bv.bv_val, bv.bv_len,
                            STRCONST("\n"));
    ଯଦି(result)
      ଭାଙ୍ଗ;

    ପାଇଁ(rc = ldap_get_attribute_ber(li->ld, ସନ୍ଦେଶ, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, ସନ୍ଦେଶ, ber, &bv, &bvals)) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
      bool ଦ୍ୱିମିକ;

      ଯଦି(!bv.bv_val)
        ଭାଙ୍ଗ;

      /* An attribute type has no base64 form in LDIF, so a type carrying
         control bytes (CR/LF etc.) would let a hostile server inject lines.
         A conformant type name never contains them, so reject it. */
      ଯଦି(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
        failf(data, "LDAP local: attribute type contains illegal bytes");
        ଯଦି(bvals)
          ber_memfree(bvals);
        result = CURLE_WEIRD_SERVER_REPLY;
        ଭାଙ୍ଗ;
      }

      ଯଦି(!bvals) {
        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":\n"));
        ଯଦି(result)
          ଭାଙ୍ଗ;
        ଜାରି;
      }

      ଦ୍ୱିମିକ = bv.bv_len > 7 &&
        curl_strnequal(bv.bv_val + bv.bv_len - 7, ";binary", 7);

      ପାଇଁ(i = 0; bvals[i].bv_val; i++) {
        bool binval = FALSE;

        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":"));
        ଯଦି(result)
          ଭାଙ୍ଗ;

        ଯଦି(!ଦ୍ୱିମିକ)
          binval = oldap_str_needs_base64(bvals[i].bv_val, bvals[i].bv_len);
        ଯଦି(ଦ୍ୱିମିକ || binval) {
          ଅକ୍ଷର *val_b64 = ଖାଲି;
          size_t val_b64_sz = 0;

          /* Binary value, encode to base64. */
          ଯଦି(bvals[i].bv_len)
            result = curlx_base64_encode((uint8_t *)bvals[i].bv_val,
                                         bvals[i].bv_len,
                                         &val_b64, &val_b64_sz);
          ଯଦି(!result)
            result = client_write(data, STRCONST(": "), val_b64, val_b64_sz,
                                  STRCONST("\n"));
          curlx_free(val_b64);
        }
        ଅନ୍ୟଥା
          result = client_write(data, STRCONST(" "),
                                bvals[i].bv_val, bvals[i].bv_len,
                                STRCONST("\n"));
        ଯଦି(result)
          ଭାଙ୍ଗ;
      }

      ber_memfree(bvals);
      bvals = ଖାଲି;
      ଯଦି(!result)
        result = client_write(data, STRCONST("\n"), ଖାଲି, 0, ଖାଲି, 0);
      ଯଦି(result)
        ଭାଙ୍ଗ;
    }

    ଯଦି(!result)
      result = client_write(data, STRCONST("\n"), ଖାଲି, 0, ଖାଲି, 0);
    ଯଦି(!result)
      result = CURLE_AGAIN;
    ଭାଙ୍ଗ;
  }

  ber_free(ber, 0);
  ldap_msgfree(ସନ୍ଦେଶ);
  ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ Curl_ldap_version(ଅକ୍ଷର *buf, size_t bufsz)
{
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  ଯଦି(ldap_get_option(ଖାଲି, LDAP_OPT_API_INFO, &api) == LDAP_OPT_SUCCESS) {
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା patch = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(api.ldapai_vendor_version % 100);
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା major = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(api.ldapai_vendor_version / 10000);
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା minor =
      (((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u",
                   api.ldapai_vendor_name, major, minor, patch);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((ଶୂନ୍ୟ **)api.ldapai_extensions);
  }
  ଅନ୍ୟଥା
    curl_msnprintf(buf, bufsz, "OpenLDAP");
}

/*
 * LDAP protocol handler.
 */
ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_ldap = {
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
