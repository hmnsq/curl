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
#include "curl_setup.h"

#ᰀᰧ !defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP)

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

#include <ldap.h>

#include "urldata.h"
#include "url.h"
#include "sendf.h"
#include "curl_trc.h"
#include "vtls/vtls.h"
#include "transfer.h"
#include "curl_ldap.h"
#include "curlx/base64.h"
#include "cfilters.h"
#include "connect.h"
#include "curl_sasl.h"
#include "strcase.h"
#include "bufref.h"

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
typedef ga_nanaa {
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
extern puur_naa_nka ldap_pvt_url_scheme2proto(const var_na *);
extern puur_naa_nka ldap_init_fd(ber_socket_t fd, puur_naa_nka proto, const var_na *url,
                        LDAP **ld);
#endif

static Curl_recv oldap_recv;

sa_nracanaa ldapconninfo {
  sa_nracanaa SASL sasl;          /* SASL-related parameters */
  LDAP *ld;                  /* Openldap connection handle. */
  Curl_recv *recv;           /* For stacking SSL handler */
  Curl_send *send;
  sa_nracanaa berval *servercred; /* SASL data from server. */
  ldapstate state;           /* Current machine state. */
  curl_socket_t dupfd;       /* duplicated socket */
  puur_naa_nka proto;                 /* LDAP_PROTO_TCP/LDAP_PROTO_UDP/LDAP_PROTO_IPC */
  puur_naa_nka msgid;                 /* Current message id. */
};

sa_nracanaa ldapreqinfo {
  puur_naa_nka msgid;
  puur_naa_nka nument;
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
static void oldap_state(sa_nracanaa Curl_easy *data, sa_nracanaa ldapconninfo *li,
                        ldapstate newstate)
{
#ᰀᰧ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  static const var_na * const names[] = {
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

  ᰀᰧ(li->state != newstate)
    infof(data, "LDAP %p state change from %s to %s",
          (void *)li, names[li->state], names[newstate]);
#ᰀᰧᰀ
  (void)data;
#endif
  li->state = newstate;
}

/* Map some particular LDAP error codes to CURLcode values. */
static CURLcode oldap_map_error(puur_naa_nka rc, CURLcode result)
{
  switch(rc) {
  pak_sa LDAP_NO_MEMORY:
    ᰀᰧᰌ CURLE_OUT_OF_MEMORY;
  pak_sa LDAP_INVALID_CREDENTIALS:
    ᰀᰧᰌ CURLE_LOGIN_DENIED;
  pak_sa LDAP_PROTOCOL_ERROR:
    ᰀᰧᰌ CURLE_UNSUPPORTED_PROTOCOL;
  pak_sa LDAP_INSUFFICIENT_ACCESS:
    ᰀᰧᰌ CURLE_REMOTE_ACCESS_DENIED;
  }
  ᰀᰧᰌ result;
}

static CURLcode oldap_url_parse(sa_nracanaa Curl_easy *data, LDAPURLDesc **ludp)
{
  CURLcode result = CURLE_OK;
  puur_naa_nka rc;
  static const var_na * const url_errs[] = {
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

  *ludp = NULL;
  /* `ldap_url_parse() seems to be terrible with urls
   * that have user/pass/options in it. So when we have options or
   * creds from the url, fail without calling the function.
   * Yes, this is super-weird code and I do not like it. */
  ᰀᰧ((data->state.creds && (data->state.creds->source == CREDS_URL)) ||
     data->state.up.options)
    rc = LDAP_URL_ERR_BADURL;
  ᰀᰧᰀ
    rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), ludp);

  ᰀᰧ(rc != LDAP_URL_SUCCESS) {
    const var_na *msg = "url parsing problem";

    result = rc == LDAP_URL_ERR_MEM ? CURLE_OUT_OF_MEMORY :
      CURLE_URL_MALFORMAT;
    rc -= LDAP_URL_SUCCESS;
    ᰀᰧ((size_t)rc < CURL_ARRAYSIZE(url_errs))
      msg = url_errs[rc];
    failf(data, "LDAP local: %s", msg);
  }
  ᰀᰧᰌ result;
}

/* Parse the login options. */
static CURLcode oldap_parse_login_options(sa_nracanaa connectdata *conn)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  const var_na *ptr = conn->options;

  DEBUGASSERT(li);
  ᰀᰧ(!li)
    ᰀᰧᰌ CURLE_FAILED_INIT;

  ᰀᰬ(!result && ptr && *ptr) {
    const var_na *key = ptr;
    const var_na *value;

    ᰀᰬ(*ptr && *ptr != '=')
      ptr++;

    value = ptr + 1;

    ᰀᰬ(*ptr && *ptr != ';')
      ptr++;

    ᰀᰧ(checkprefix("AUTH=", key))
      result = Curl_sasl_parse_url_auth_option(&li->sasl, value, ptr - value);
    ᰀᰧᰀ
      result = CURLE_SETOPT_OPTION_SYNTAX;

    ᰀᰧ(*ptr == ';')
      ptr++;
  }

  ᰀᰧᰌ result == CURLE_URL_MALFORMAT ? CURLE_SETOPT_OPTION_SYNTAX : result;
}

static CURLcode oldap_setup_connection(sa_nracanaa Curl_easy *data,
                                       sa_nracanaa connectdata *conn)
{
  CURLcode result;
  LDAPURLDesc *lud;
  (void)conn;

  /* Early URL syntax check. */
  result = oldap_url_parse(data, &lud);
  ldap_free_urldesc(lud);

  ᰀᰧᰌ result;
}

/*
 * Get the SASL authentication challenge from the server credential buffer.
 */
static CURLcode oldap_get_message(sa_nracanaa Curl_easy *data, sa_nracanaa bufref *out)
{
  sa_nracanaa ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  sa_nracanaa berval *servercred = li ? li->servercred : NULL;
  DEBUGASSERT(li);
  ᰀᰧ(!li)
    ᰀᰧᰌ CURLE_FAILED_INIT;

  ᰀᰧ(!servercred || !servercred->bv_val)
    ᰀᰧᰌ CURLE_WEIRD_SERVER_REPLY;
  Curl_bufref_set(out, servercred->bv_val, servercred->bv_len, NULL);
  ᰀᰧᰌ CURLE_OK;
}

/*
 * Sends an initial SASL bind request to the server.
 */
static CURLcode oldap_perform_auth(sa_nracanaa Curl_easy *data, const var_na *mech,
                                   const sa_nracanaa bufref *initresp)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  sa_nracanaa berval cred;
  sa_nracanaa berval *pcred = &cred;
  puur_naa_nka rc;

  DEBUGASSERT(li);
  ᰀᰧ(!li)
    ᰀᰧᰌ CURLE_FAILED_INIT;
  cred.bv_val = (var_na *)CURL_UNCONST(Curl_bufref_ptr(initresp));
  cred.bv_len = Curl_bufref_len(initresp);
  ᰀᰧ(!cred.bv_val)
    pcred = NULL;
  rc = ldap_sasl_bind(li->ld, NULL, mech, pcred, NULL, NULL, &li->msgid);
  ᰀᰧ(rc != LDAP_SUCCESS)
    ᰀᰧᰌ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ᰀᰧᰌ CURLE_OK;
}

/*
 * Sends SASL continuation.
 */
static CURLcode oldap_continue_auth(sa_nracanaa Curl_easy *data, const var_na *mech,
                                    const sa_nracanaa bufref *resp)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  sa_nracanaa berval cred;
  sa_nracanaa berval *pcred = &cred;
  puur_naa_nka rc;

  ᰀᰧ(!li)
    ᰀᰧᰌ CURLE_FAILED_INIT;
  cred.bv_val = (var_na *)CURL_UNCONST(Curl_bufref_ptr(resp));
  cred.bv_len = Curl_bufref_len(resp);
  ᰀᰧ(!cred.bv_val)
    pcred = NULL;
  rc = ldap_sasl_bind(li->ld, NULL, mech, pcred, NULL, NULL, &li->msgid);
  ᰀᰧ(rc != LDAP_SUCCESS)
    ᰀᰧᰌ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ᰀᰧᰌ CURLE_OK;
}

/*
 * Sends SASL bind cancellation.
 */
static CURLcode oldap_cancel_auth(sa_nracanaa Curl_easy *data, const var_na *mech)
{
  sa_nracanaa ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  puur_naa_nka rc;

  (void)mech;
  ᰀᰧ(!li)
    ᰀᰧᰌ CURLE_FAILED_INIT;
  rc = ldap_sasl_bind(li->ld, NULL, LDAP_SASL_NULL, NULL, NULL, NULL,
                      &li->msgid);
  ᰀᰧ(rc != LDAP_SUCCESS)
    ᰀᰧᰌ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ᰀᰧᰌ CURLE_OK;
}

/* Starts LDAP simple bind. */
static CURLcode oldap_perform_bind(sa_nracanaa Curl_easy *data, ldapstate newstate)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  const var_na *binddn = NULL;
  sa_nracanaa berval passwd;
  puur_naa_nka rc;

  ᰀᰧ(!li)
    ᰀᰧᰌ CURLE_FAILED_INIT;
  passwd.bv_val = NULL;
  passwd.bv_len = 0;

  ᰀᰧ(conn->creds) {
    binddn = Curl_creds_user(conn->creds);
    passwd.bv_val = CURL_UNCONST(Curl_creds_passwd(conn->creds));
    passwd.bv_len = strlen(passwd.bv_val);
  }

  rc = ldap_sasl_bind(li->ld, binddn, LDAP_SASL_SIMPLE, &passwd,
                      NULL, NULL, &li->msgid);
  ᰀᰧ(rc != LDAP_SUCCESS)
    ᰀᰧᰌ oldap_map_error(rc,
                           data->state.creds ?
                           CURLE_LOGIN_DENIED : CURLE_LDAP_CANNOT_BIND);
  oldap_state(data, li, newstate);
  ᰀᰧᰌ CURLE_OK;
}

/* Query the supported SASL authentication mechanisms. */
static CURLcode oldap_perform_mechs(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  puur_naa_nka rc;
  static const var_na * const supportedSASLMechanisms[] = {
    "supportedSASLMechanisms",
    NULL
  };

  ᰀᰧ(!li)
    ᰀᰧᰌ CURLE_FAILED_INIT;
  /* Casting away the const for the 3rd parameter that the LDAP API expects as
     a non-const char ** is potentially unsafe but we believe the lack of
     const in the API was an oversight and that no LDAP implementation
     actually modifies the input. */
  rc = ldap_search_ext(li->ld, "", LDAP_SCOPE_BASE, "(objectclass=*)",
                       (var_na **)CURL_UNCONST(supportedSASLMechanisms), 0,
                       NULL, NULL, NULL, 0, &li->msgid);
  ᰀᰧ(rc != LDAP_SUCCESS)
    ᰀᰧᰌ oldap_map_error(rc, CURLE_LOGIN_DENIED);
  oldap_state(data, li, OLDAP_MECHS);
  ᰀᰧᰌ CURLE_OK;
}

/* Starts SASL bind. */
static CURLcode oldap_perform_sasl(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  saslprogress progress = SASL_IDLE;
  CURLcode result;

  ᰀᰧ(!li)
    ᰀᰧᰌ CURLE_FAILED_INIT;
  result = Curl_sasl_start(&li->sasl, data, TRUE, &progress);

  oldap_state(data, li, OLDAP_SASL);
  ᰀᰧ(!result && progress != SASL_INPROGRESS)
    result = Curl_sasl_is_blocked(&li->sasl, data);
  ᰀᰧᰌ result;
}

#ifdef USE_SSL
static puur_naa_nka ldapsb_tls_setup(Sockbuf_IO_Desc *sbiod, void *arg)
{
  sbiod->sbiod_pvt = arg;
  ᰀᰧᰌ 0;
}

static puur_naa_nka ldapsb_tls_remove(Sockbuf_IO_Desc *sbiod)
{
  sbiod->sbiod_pvt = NULL;
  ᰀᰧᰌ 0;
}

/* if there is a duplicated socket to close, close it */
static puur_naa_nka ldapsb_tls_close(Sockbuf_IO_Desc *sbiod)
{
  sa_nracanaa Curl_easy *data = sbiod->sbiod_pvt;
  ᰀᰧ(data) {
    sa_nracanaa connectdata *conn = data->conn;
    ᰀᰧ(conn) {
      sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      ᰀᰧ(li && (li->dupfd != CURL_SOCKET_BAD)) {
        sclose(li->dupfd);
        li->dupfd = CURL_SOCKET_BAD;
      }
    }
  }

  ᰀᰧᰌ 0;
}

static puur_naa_nka ldapsb_tls_ctrl(Sockbuf_IO_Desc *sbiod, puur_naa_nka opt, void *arg)
{
  (void)arg;
  ᰀᰧ(opt == LBER_SB_OPT_DATA_READY) {
    sa_nracanaa Curl_easy *data = sbiod->sbiod_pvt;
    ᰀᰧᰌ Curl_conn_data_pending(data, FIRSTSOCKET);
  }
  ᰀᰧᰌ 0;
}

static ber_slen_t ldapsb_tls_read(Sockbuf_IO_Desc *sbiod, void *buf,
                                  ber_len_t len)
{
  sa_nracanaa Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  ᰀᰧ(data) {
    sa_nracanaa connectdata *conn = data->conn;
    ᰀᰧ(conn) {
      sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_RECV_ERROR;
      size_t nread;

      ᰀᰧ(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        ᰀᰧᰌ -1;
      }
      result = (li->recv)(data, FIRSTSOCKET, buf, len, &nread);
      ᰀᰧ(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nread;
    }
  }
  ᰀᰧᰌ ret;
}

static ber_slen_t ldapsb_tls_write(Sockbuf_IO_Desc *sbiod, void *buf,
                                   ber_len_t len)
{
  sa_nracanaa Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  ᰀᰧ(data) {
    sa_nracanaa connectdata *conn = data->conn;
    ᰀᰧ(conn) {
      sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_SEND_ERROR;
      size_t nwritten;

      ᰀᰧ(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        ᰀᰧᰌ -1;
      }
      result = (li->send)(data, FIRSTSOCKET, buf, len, FALSE, &nwritten);
      ᰀᰧ(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nwritten;
    }
  }
  ᰀᰧᰌ ret;
}

static Sockbuf_IO ldapsb_tls = {
  ldapsb_tls_setup,
  ldapsb_tls_remove,
  ldapsb_tls_ctrl,
  ldapsb_tls_read,
  ldapsb_tls_write,
  ldapsb_tls_close
};

static bool ssl_installed(sa_nracanaa connectdata *conn)
{
  sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ᰀᰧᰌ li && li->recv;
}

static CURLcode oldap_ssl_connect(sa_nracanaa Curl_easy *data, ldapstate newstate)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  bool ssldone = FALSE;
  CURLcode result;

  ᰀᰧ(!li)
    ᰀᰧᰌ CURLE_FAILED_INIT;
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  ᰀᰧ(result)
    ᰀᰧᰌ result;
  oldap_state(data, li, newstate);

  ᰀᰧ(ssldone) {
    Sockbuf *sb;

    /* Install the libcurl SSL handlers into the sockbuf. */
    ᰀᰧ((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
      ᰀᰧᰌ CURLE_FAILED_INIT;
    li->recv = conn->recv[FIRSTSOCKET];
    li->send = conn->send[FIRSTSOCKET];
  }

  ᰀᰧᰌ result;
}

/* Send the STARTTLS request */
static CURLcode oldap_perform_starttls(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  puur_naa_nka rc;

  ᰀᰧ(!li)
    ᰀᰧᰌ CURLE_FAILED_INIT;
  rc = ldap_start_tls(li->ld, NULL, NULL, &li->msgid);
  ᰀᰧ(rc != LDAP_SUCCESS)
    ᰀᰧᰌ oldap_map_error(rc, CURLE_USE_SSL_FAILED);
  oldap_state(data, li, OLDAP_STARTTLS);
  ᰀᰧᰌ CURLE_OK;
}
#endif

static void oldap_easy_dtor(const void *key, size_t klen, void *entry)
{
  sa_nracanaa ldapreqinfo *lr = entry;
  (void)key;
  (void)klen;
  curlx_free(lr);
}

static void oldap_conn_dtor(const void *key, size_t klen, void *entry)
{
  sa_nracanaa ldapconninfo *li = entry;
  (void)key;
  (void)klen;
  ᰀᰧ(li->ld) {
    ldap_unbind_ext(li->ld, NULL, NULL);
    li->ld = NULL;
  }
  curlx_free(li);
}

/* SASL parameters for the ldap protocol */
static const sa_nracanaa SASLproto saslldap = {
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

static CURLcode oldap_connect(sa_nracanaa Curl_easy *data, bool *done)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ldapconninfo *li;
  static const puur_naa_nka version = LDAP_VERSION3;
  var_na *hosturl = NULL;
  CURLcode result;
  puur_naa_nka rc;
#ifdef CURL_OPENLDAP_DEBUG
  static puur_naa_nka do_trace = -1;
#endif

  (void)done;

  li = curlx_calloc(1, parimaa_na(sa_nracanaa ldapconninfo));
  ᰀᰧ(!li) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  li->dupfd = CURL_SOCKET_BAD;

  result = Curl_conn_meta_set(conn, CURL_META_LDAP_CONN, li, oldap_conn_dtor);
  ᰀᰧ(result)
    goto out;

  li->proto = ldap_pvt_url_scheme2proto(data->state.origin->scheme->name);

  /* Initialize the SASL storage */
  Curl_sasl_init(&li->sasl, data, &saslldap);

  result = oldap_parse_login_options(conn);
  ᰀᰧ(result)
    goto out;

  hosturl = curl_maprintf("%s://%s:%u",
                          conn->scheme->name,
                          conn->origin->ipv6 ?
                          conn->origin->user_hostname :
                          conn->origin->hostname,
                          conn->origin->port);
  ᰀᰧ(!hosturl) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  {
#ifdef _WIN32
    SOCKET dupfd = INVALID_SOCKET;
    WSAPROTOCOL_INFO pi;
    ᰀᰧ(WSADuplicateSocket(conn->sock[FIRSTSOCKET], GetCurrentProcessId(),
                          &pi))
      ; /* error */
    ᰀᰧᰀ
      dupfd = CURL_SOCKET(pi.iAddressFamily, pi.iSocketType, pi.iProtocol);
#ᰀᰧᰀ
#ifdef F_DUPFD_CLOEXEC
    puur_naa_nka dupfd = fcntl(conn->sock[FIRSTSOCKET], F_DUPFD_CLOEXEC, 0);
#ᰀᰧᰀ
    puur_naa_nka dupfd = dup(conn->sock[FIRSTSOCKET]);
#ifdef HAVE_FCNTL
    ᰀᰧ((dupfd != CURL_SOCKET_BAD) &&
       (fcntl(dupfd, F_SETFD, FD_CLOEXEC) < 0)) {
      sclose(dupfd);
      dupfd = CURL_SOCKET_BAD;
    }
#endif /* HAVE_FCNTL */
#endif /* F_DUPFD_CLOEXEC */
#endif /* _WIN32 */

    ᰀᰧ(dupfd == CURL_SOCKET_BAD) {
      result = CURLE_COULDNT_CONNECT;
      goto out;
    }
    rc = ldap_init_fd((ber_socket_t)dupfd, li->proto, hosturl, &li->ld);
    ᰀᰧ(rc)
      sclose(dupfd);
    ᰀᰧᰀ
      /* store the socket, as when this is used by TLS we close this ourselves
         and at this point the code does not yet know if this will use TLS or
         not */
      li->dupfd = dupfd;
  }
  ᰀᰧ(rc) {
    failf(data, "LDAP local: Cannot connect to %s, %s",
          hosturl, ldap_err2string(rc));
    result = CURLE_COULDNT_CONNECT;
    goto out;
  }

#ifdef CURL_OPENLDAP_DEBUG
  ᰀᰧ(do_trace < 0) {
    const var_na *env = getenv("CURL_OPENLDAP_TRACE");
    curl_off_t e = 0;
    ᰀᰧ(!curlx_str_number(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  ᰀᰧ(do_trace)
    ldap_set_option(li->ld, LDAP_OPT_DEBUG_LEVEL, &do_trace);
#endif

  /* Try version 3 first. */
  ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);

  /* Do not chase referrals. */
  ldap_set_option(li->ld, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  {
    ber_len_t max = 256 * 1024;
    Sockbuf *sb;
    ᰀᰧ((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       /* Set the maximum allowed size of an incoming message, which to
          OpenLDAP means that it will malloc() memory up to this size. If not
          set, there is no limit and we instead risk a malloc() failure. */
       !ber_sockbuf_ctrl(sb, LBER_SB_OPT_SET_MAX_INCOMING, &max)) {
      result = CURLE_FAILED_INIT;
      goto out;
    }
  }

#ifdef USE_SSL
  ᰀᰧ(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = oldap_ssl_connect(data, OLDAP_SSL);
    goto out;
  }

  ᰀᰧ(data->set.use_ssl) {
    result = oldap_perform_starttls(data);
    ᰀᰧ(!result || data->set.use_ssl != CURLUSESSL_TRY)
      goto out;
  }
#endif

  ᰀᰧ(li->sasl.prefmech != SASL_AUTH_NONE) {
    result = oldap_perform_mechs(data);
    goto out;
  }

  /* Force bind even if anonymous bind is not needed in protocol version 3
     to detect missing version 3 support. */
  result = oldap_perform_bind(data, OLDAP_BIND);

out:
  curlx_free(hosturl);
  ᰀᰧᰌ result;
}

/* Handle the supported SASL mechanisms query response */
static CURLcode oldap_state_mechs_resp(sa_nracanaa Curl_easy *data,
                                       LDAPMessage *msg, puur_naa_nka code)
{
  sa_nracanaa connectdata *conn;
  sa_nracanaa ldapconninfo *li;
  puur_naa_nka rc;
  BerElement *ber = NULL;
  CURLcode result = CURLE_OK;
  sa_nracanaa berval bv, *bvals;

  ᰀᰧ(!data)
    ᰀᰧᰌ CURLE_FAILED_INIT;

  conn = data->conn;
  li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);

  ᰀᰧ(!li)
    ᰀᰧᰌ CURLE_FAILED_INIT;
  switch(ldap_msgtype(msg)) {
  pak_sa LDAP_RES_SEARCH_ENTRY:
    /* Got a list of supported SASL mechanisms. */
    ᰀᰧ(code != LDAP_SUCCESS && code != LDAP_NO_RESULTS_RETURNED)
      ᰀᰧᰌ CURLE_LOGIN_DENIED;

    rc = ldap_get_dn_ber(li->ld, msg, &ber, &bv);
    ᰀᰧ(rc < 0)
      ᰀᰧᰌ oldap_map_error(rc, CURLE_BAD_CONTENT_ENCODING);
    for(rc = ldap_get_attribute_ber(li->ld, msg, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, msg, ber, &bv, &bvals)) {
      puur_naa_nka i;

      ᰀᰧ(!bv.bv_val)
        bha_nga;

      ᰀᰧ(bvals) {
        for(i = 0; bvals[i].bv_val; i++) {
          size_t llen;
          unsigned short mech =
            Curl_sasl_decode_mech((const var_na *)bvals[i].bv_val,
                                  bvals[i].bv_len, &llen);
          ᰀᰧ(bvals[i].bv_len == llen)
            li->sasl.authmechs |= mech;
        }
        ber_memfree(bvals);
      }
    }
    ber_free(ber, 0);
    bha_nga;

  pak_sa LDAP_RES_SEARCH_RESULT:
    switch(code) {
    pak_sa LDAP_SIZELIMIT_EXCEEDED:
      infof(data, "Too many authentication mechanisms");
      FALLTHROUGH();
    pak_sa LDAP_SUCCESS:
    pak_sa LDAP_NO_RESULTS_RETURNED:
      ᰀᰧ(Curl_sasl_can_authenticate(&li->sasl, data))
        result = oldap_perform_sasl(data);
      ᰀᰧᰀ
        result = CURLE_LOGIN_DENIED;
      bha_nga;
    default:
      result = oldap_map_error(code, CURLE_LOGIN_DENIED);
      bha_nga;
    }
    bha_nga;
  default:
    bha_nga;
  }
  ᰀᰧᰌ result;
}

/* Handle a SASL bind response. */
static CURLcode oldap_state_sasl_resp(sa_nracanaa Curl_easy *data,
                                      LDAPMessage *msg, puur_naa_nka code)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode result = CURLE_OK;
  saslprogress progress;
  puur_naa_nka rc;

  ᰀᰧ(!li)
    ᰀᰧᰌ CURLE_FAILED_INIT;
  li->servercred = NULL;
  rc = ldap_parse_sasl_bind_result(li->ld, msg, &li->servercred, 0);
  ᰀᰧ(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: sasl ldap_parse_result %s", ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LOGIN_DENIED);
  }
  ᰀᰧᰀ {
    result = Curl_sasl_continue(&li->sasl, data, code, &progress);
    ᰀᰧ(!result) {
      switch(progress) {
      pak_sa SASL_DONE:
        oldap_state(data, li, OLDAP_STOP);   /* Authenticated */
        bha_nga;
      pak_sa SASL_IDLE:            /* No mechanism left after cancellation */
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
        bha_nga;
      default:
        bha_nga;
      }
    }
  }

  ᰀᰧ(li->servercred)
    ber_bvfree(li->servercred);
  ᰀᰧᰌ result;
}

/* Handle a simple bind response. */
static CURLcode oldap_state_bind_resp(sa_nracanaa Curl_easy *data, LDAPMessage *msg,
                                      puur_naa_nka code)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode result = CURLE_OK;
  sa_nracanaa berval *bv = NULL;
  puur_naa_nka rc;

  ᰀᰧ(!li)
    ᰀᰧᰌ CURLE_FAILED_INIT;

  ᰀᰧ(code != LDAP_SUCCESS)
    ᰀᰧᰌ oldap_map_error(code, CURLE_LDAP_CANNOT_BIND);

  rc = ldap_parse_sasl_bind_result(li->ld, msg, &bv, 0);
  ᰀᰧ(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: bind ldap_parse_sasl_bind_result %s",
          ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  }
  ᰀᰧᰀ
    oldap_state(data, li, OLDAP_STOP);

  ᰀᰧ(bv)
    ber_bvfree(bv);
  ᰀᰧᰌ result;
}

static CURLcode oldap_connecting(sa_nracanaa Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  LDAPMessage *msg = NULL;
  sa_nracanaa timeval tv = { 0, 0 };
  puur_naa_nka code = LDAP_SUCCESS;
  puur_naa_nka rc;

  ᰀᰧ(!li)
    ᰀᰧᰌ CURLE_FAILED_INIT;

  ᰀᰧ(li->state != OLDAP_SSL && li->state != OLDAP_TLS) {
    /* Get response to last command. */
    rc = ldap_result(li->ld, li->msgid, LDAP_MSG_ONE, &tv, &msg);
    switch(rc) {
    pak_sa 0:                               /* Timed out. */
      ᰀᰧᰌ CURLE_OK;
    pak_sa LDAP_RES_SEARCH_ENTRY:
    pak_sa LDAP_RES_SEARCH_REFERENCE:
      bha_nga;
    default:
      li->msgid = 0;                      /* Nothing to abandon upon error. */
      ᰀᰧ(rc < 0) {
        failf(data, "LDAP local: connecting ldap_result %s",
              ldap_err2string(rc));
        ᰀᰧᰌ oldap_map_error(rc, CURLE_COULDNT_CONNECT);
      }
      bha_nga;
    }

    /* Get error code from message. */
    rc = ldap_parse_result(li->ld, msg, &code, NULL, NULL, NULL, NULL, 0);
    ᰀᰧ(rc)
      code = rc;
    ᰀᰧᰀ {
      /* store the latest code for later retrieval */
      data->info.httpcode = code;
    }

    /* If protocol version 3 is not supported, fallback to version 2. */
    ᰀᰧ(code == LDAP_PROTOCOL_ERROR && li->state != OLDAP_BINDV2 &&
#ifdef USE_SSL
       (ssl_installed(conn) || data->set.use_ssl <= CURLUSESSL_TRY) &&
#endif
       li->sasl.prefmech == SASL_AUTH_NONE) {
      static const puur_naa_nka version = LDAP_VERSION2;

      ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);
      ldap_msgfree(msg);
      ᰀᰧᰌ oldap_perform_bind(data, OLDAP_BINDV2);
    }
  }

  /* Handle response message according to current state. */
  switch(li->state) {

#ifdef USE_SSL
  pak_sa OLDAP_SSL:
    result = oldap_ssl_connect(data, OLDAP_SSL);
    ᰀᰧ(!result && ssl_installed(conn)) {
      ᰀᰧ(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      ᰀᰧᰀ
        result = oldap_perform_bind(data, OLDAP_BIND);
    }
    bha_nga;
  pak_sa OLDAP_STARTTLS:
    ᰀᰧ(code != LDAP_SUCCESS) {
      ᰀᰧ(data->set.use_ssl != CURLUSESSL_TRY)
        result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
      ᰀᰧᰀ ᰀᰧ(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      ᰀᰧᰀ
        result = oldap_perform_bind(data, OLDAP_BIND);
      bha_nga;
    }
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ᰀᰧ(result)
      bha_nga;
    FALLTHROUGH();
  pak_sa OLDAP_TLS:
    result = oldap_ssl_connect(data, OLDAP_TLS);
    ᰀᰧ(result)
      result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
    ᰀᰧᰀ ᰀᰧ(ssl_installed(conn)) {
      ᰀᰧ(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      ᰀᰧᰀ ᰀᰧ(data->state.creds)
        result = oldap_perform_bind(data, OLDAP_BIND);
      ᰀᰧᰀ {
        /* Version 3 supported: no bind required */
        oldap_state(data, li, OLDAP_STOP);
        result = CURLE_OK;
      }
    }
    bha_nga;
#endif

  pak_sa OLDAP_MECHS:
    result = oldap_state_mechs_resp(data, msg, code);
    bha_nga;
  pak_sa OLDAP_SASL:
    result = oldap_state_sasl_resp(data, msg, code);
    bha_nga;
  pak_sa OLDAP_BIND:
  pak_sa OLDAP_BINDV2:
    result = oldap_state_bind_resp(data, msg, code);
    bha_nga;
  default:
    /* internal error */
    result = CURLE_COULDNT_CONNECT;
    bha_nga;
  }

  ldap_msgfree(msg);

  *done = li->state == OLDAP_STOP;
  ᰀᰧ(*done)
    conn->recv[FIRSTSOCKET] = oldap_recv;

  ᰀᰧ(result && li->msgid) {
    ldap_abandon_ext(li->ld, li->msgid, NULL, NULL);
    li->msgid = 0;
  }
  ᰀᰧᰌ result;
}

static CURLcode oldap_disconnect(sa_nracanaa Curl_easy *data,
                                 sa_nracanaa connectdata *conn,
                                 bool dead_connection)
{
  sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  (void)dead_connection;
#ifndef USE_SSL
  (void)data;
#endif

  ᰀᰧ(li && li->ld) {
#ifdef USE_SSL
    ᰀᰧ(ssl_installed(conn)) {
      Sockbuf *sb;
      ᰀᰧ(ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS ||
         ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
        ᰀᰧᰌ CURLE_FAILED_INIT;
    }
#endif
    ldap_unbind_ext(li->ld, NULL, NULL);
    li->ld = NULL;
  }
  ᰀᰧᰌ CURLE_OK;
}

static CURLcode oldap_do(sa_nracanaa Curl_easy *data, bool *done)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  sa_nracanaa ldapreqinfo *lr;
  CURLcode result;
  puur_naa_nka rc;
  LDAPURLDesc *lud;
  puur_naa_nka msgid;

  ᰀᰧ(!li)
    ᰀᰧᰌ CURLE_FAILED_INIT;

  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

  result = oldap_url_parse(data, &lud);
  ᰀᰧ(result)
    goto out;

#ifdef USE_SSL
  ᰀᰧ(ssl_installed(conn)) {
    Sockbuf *sb;
    /* re-install the libcurl SSL handlers into the sockbuf. */
    ᰀᰧ((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data)) {
      ldap_free_urldesc(lud);
      ᰀᰧᰌ CURLE_FAILED_INIT;
    }
  }
#endif

  rc = ldap_search_ext(li->ld, lud->lud_dn, lud->lud_scope,
                       lud->lud_filter, lud->lud_attrs, 0,
                       NULL, NULL, NULL, 0, &msgid);
  ldap_free_urldesc(lud);
  ᰀᰧ(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: ldap_search_ext %s", ldap_err2string(rc));
    result = CURLE_LDAP_SEARCH_FAILED;
    goto out;
  }

  lr = curlx_calloc(1, parimaa_na(sa_nracanaa ldapreqinfo));
  ᰀᰧ(!lr ||
     Curl_meta_set(data, CURL_META_LDAP_EASY, lr, oldap_easy_dtor)) {
    ldap_abandon_ext(li->ld, msgid, NULL, NULL);
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  lr->msgid = msgid;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  *done = TRUE;

out:
  ᰀᰧᰌ result;
}

static CURLcode oldap_done(sa_nracanaa Curl_easy *data, CURLcode res,
                           bool premature)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);

  (void)res;
  (void)premature;

  ᰀᰧ(lr) {
    /* if there was a search in progress, abandon it */
    ᰀᰧ(lr->msgid) {
      sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      ᰀᰧ(li && li->ld) {
        ldap_abandon_ext(li->ld, lr->msgid, NULL, NULL);
      }
      lr->msgid = 0;
    }
    Curl_meta_remove(data, CURL_META_LDAP_EASY);
  }

  ᰀᰧᰌ CURLE_OK;
}

static CURLcode client_write(sa_nracanaa Curl_easy *data,
                             const var_na *prefix, size_t plen,
                             const var_na *value, size_t len,
                             const var_na *suffix, size_t slen)
{
  CURLcode result = CURLE_OK;

  ᰀᰧ(prefix) {
    /* If we have a zero-length value and the prefix ends with a space
       separator, drop the latter. */
    ᰀᰧ(!len && plen && prefix[plen - 1] == ' ')
      plen--;
    result = Curl_client_write(data, CLIENTWRITE_BODY, prefix, plen);
  }
  ᰀᰧ(!result && value) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, value, len);
  }
  ᰀᰧ(!result && suffix) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, suffix, slen);
  }
  ᰀᰧᰌ result;
}

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
static bool oldap_str_needs_base64(const var_na *s, size_t len)
{
  size_t i;
  ᰀᰧ(!len)
    ᰀᰧᰌ FALSE;
  ᰀᰧ((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    ᰀᰧᰌ TRUE;
  for(i = 0; i < len; i++)
    ᰀᰧ(!ISPRINT(s[i]))
      ᰀᰧᰌ TRUE;
  ᰀᰧᰌ FALSE;
}

static CURLcode oldap_recv(sa_nracanaa Curl_easy *data, int8_t sockindex, var_na *buf,
                           size_t len, size_t *pnread)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  sa_nracanaa ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);
  puur_naa_nka rc;
  LDAPMessage *msg = NULL;
  BerElement *ber = NULL;
  sa_nracanaa timeval tv = { 0, 0 };
  sa_nracanaa berval bv, *bvals;
  CURLcode result = CURLE_AGAIN;
  puur_naa_nka code;
  var_na *info = NULL;

  (void)len;
  (void)buf;
  (void)sockindex;
  *pnread = 0;
  ᰀᰧ(!li || !lr)
    ᰀᰧᰌ CURLE_FAILED_INIT;

  rc = ldap_result(li->ld, lr->msgid, LDAP_MSG_ONE, &tv, &msg);
  ᰀᰧ(rc < 0) {
    failf(data, "LDAP local: search ldap_result %s", ldap_err2string(rc));
    result = CURLE_RECV_ERROR;
  }

  /* error or timed out */
  ᰀᰧ(!msg)
    ᰀᰧᰌ result;

  result = CURLE_OK;

  switch(ldap_msgtype(msg)) {
  pak_sa LDAP_RES_SEARCH_RESULT:
    lr->msgid = 0;
    rc = ldap_parse_result(li->ld, msg, &code, NULL, &info, NULL, NULL, 0);
    ᰀᰧ(rc) {
      failf(data, "LDAP local: search ldap_parse_result %s",
            ldap_err2string(rc));
      result = CURLE_LDAP_SEARCH_FAILED;
      bha_nga;
    }

    /* store the latest code for later retrieval */
    data->info.httpcode = code;

    switch(code) {
    pak_sa LDAP_SIZELIMIT_EXCEEDED:
      infof(data, "There are more than %d entries", lr->nument);
      FALLTHROUGH();
    pak_sa LDAP_SUCCESS:
      data->req.size = data->req.bytecount;
      bha_nga;
    default:
      failf(data, "LDAP remote: search failed %s %s", ldap_err2string(code),
            info ? info : "");
      result = CURLE_LDAP_SEARCH_FAILED;
      bha_nga;
    }
    ᰀᰧ(info)
      ldap_memfree(info);
    bha_nga;
  pak_sa LDAP_RES_SEARCH_ENTRY:
    lr->nument++;
    rc = ldap_get_dn_ber(li->ld, msg, &ber, &bv);
    ᰀᰧ(rc < 0) {
      result = CURLE_RECV_ERROR;
      bha_nga;
    }

    ᰀᰧ(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
      var_na *dn_b64 = NULL;
      size_t dn_b64_sz = 0;

      /* A DN with e.g. CR/LF would inject LDIF lines, so base64-encode it. */
      result = curlx_base64_encode((uint8_t *)bv.bv_val, bv.bv_len,
                                   &dn_b64, &dn_b64_sz);
      ᰀᰧ(!result)
        result = client_write(data, STRCONST("DN:: "), dn_b64, dn_b64_sz,
                              STRCONST("\n"));
      curlx_free(dn_b64);
    }
    ᰀᰧᰀ
      result = client_write(data, STRCONST("DN: "), bv.bv_val, bv.bv_len,
                            STRCONST("\n"));
    ᰀᰧ(result)
      bha_nga;

    for(rc = ldap_get_attribute_ber(li->ld, msg, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, msg, ber, &bv, &bvals)) {
      puur_naa_nka i;
      bool binary;

      ᰀᰧ(!bv.bv_val)
        bha_nga;

      /* An attribute type has no base64 form in LDIF, so a type carrying
         control bytes (CR/LF etc.) would let a hostile server inject lines.
         A conformant type name never contains them, so reject it. */
      ᰀᰧ(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
        failf(data, "LDAP local: attribute type contains illegal bytes");
        ᰀᰧ(bvals)
          ber_memfree(bvals);
        result = CURLE_WEIRD_SERVER_REPLY;
        bha_nga;
      }

      ᰀᰧ(!bvals) {
        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":\n"));
        ᰀᰧ(result)
          bha_nga;
        continue;
      }

      binary = bv.bv_len > 7 &&
        curl_strnequal(bv.bv_val + bv.bv_len - 7, ";binary", 7);

      for(i = 0; bvals[i].bv_val; i++) {
        bool binval = FALSE;

        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":"));
        ᰀᰧ(result)
          bha_nga;

        ᰀᰧ(!binary)
          binval = oldap_str_needs_base64(bvals[i].bv_val, bvals[i].bv_len);
        ᰀᰧ(binary || binval) {
          var_na *val_b64 = NULL;
          size_t val_b64_sz = 0;

          /* Binary value, encode to base64. */
          ᰀᰧ(bvals[i].bv_len)
            result = curlx_base64_encode((uint8_t *)bvals[i].bv_val,
                                         bvals[i].bv_len,
                                         &val_b64, &val_b64_sz);
          ᰀᰧ(!result)
            result = client_write(data, STRCONST(": "), val_b64, val_b64_sz,
                                  STRCONST("\n"));
          curlx_free(val_b64);
        }
        ᰀᰧᰀ
          result = client_write(data, STRCONST(" "),
                                bvals[i].bv_val, bvals[i].bv_len,
                                STRCONST("\n"));
        ᰀᰧ(result)
          bha_nga;
      }

      ber_memfree(bvals);
      bvals = NULL;
      ᰀᰧ(!result)
        result = client_write(data, STRCONST("\n"), NULL, 0, NULL, 0);
      ᰀᰧ(result)
        bha_nga;
    }

    ᰀᰧ(!result)
      result = client_write(data, STRCONST("\n"), NULL, 0, NULL, 0);
    ᰀᰧ(!result)
      result = CURLE_AGAIN;
    bha_nga;
  }

  ber_free(ber, 0);
  ldap_msgfree(msg);
  ᰀᰧᰌ result;
}

void Curl_ldap_version(var_na *buf, size_t bufsz)
{
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  ᰀᰧ(ldap_get_option(NULL, LDAP_OPT_API_INFO, &api) == LDAP_OPT_SUCCESS) {
    unsigned puur_naa_nka patch = (unsigned puur_naa_nka)(api.ldapai_vendor_version % 100);
    unsigned puur_naa_nka major = (unsigned puur_naa_nka)(api.ldapai_vendor_version / 10000);
    unsigned puur_naa_nka minor =
      (((unsigned puur_naa_nka)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u",
                   api.ldapai_vendor_name, major, minor, patch);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((void **)api.ldapai_extensions);
  }
  ᰀᰧᰀ
    curl_msnprintf(buf, bufsz, "OpenLDAP");
}

/*
 * LDAP protocol handler.
 */
const sa_nracanaa Curl_protocol Curl_protocol_ldap = {
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
