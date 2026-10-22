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

#ᬬᬾᬦ᭄ !defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP)

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
#ᬬᬾᬦ᭄ defined(DEBUGBUILD) && defined(CURLVERBOSE)
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

  ᬬᬾᬦ᭄(li->state != newstate)
    infof(data, "LDAP %p state change from %s to %s",
          (void *)li, names[li->state], names[newstate]);
#ᬮᬶᬬ
  (void)data;
#endif
  li->state = newstate;
}

/* Map some particular LDAP error codes to CURLcode values. */
static CURLcode oldap_map_error(puur_naa_nka rc, CURLcode result)
{
  switch(rc) {
  pak_sa LDAP_NO_MEMORY:
    ᬯᬮᬶ CURLE_OUT_OF_MEMORY;
  pak_sa LDAP_INVALID_CREDENTIALS:
    ᬯᬮᬶ CURLE_LOGIN_DENIED;
  pak_sa LDAP_PROTOCOL_ERROR:
    ᬯᬮᬶ CURLE_UNSUPPORTED_PROTOCOL;
  pak_sa LDAP_INSUFFICIENT_ACCESS:
    ᬯᬮᬶ CURLE_REMOTE_ACCESS_DENIED;
  }
  ᬯᬮᬶ result;
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
  ᬬᬾᬦ᭄((data->state.creds && (data->state.creds->source == CREDS_URL)) ||
     data->state.up.options)
    rc = LDAP_URL_ERR_BADURL;
  ᬮᬶᬬ
    rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), ludp);

  ᬬᬾᬦ᭄(rc != LDAP_URL_SUCCESS) {
    const var_na *msg = "url parsing problem";

    result = rc == LDAP_URL_ERR_MEM ? CURLE_OUT_OF_MEMORY :
      CURLE_URL_MALFORMAT;
    rc -= LDAP_URL_SUCCESS;
    ᬬᬾᬦ᭄((size_t)rc < CURL_ARRAYSIZE(url_errs))
      msg = url_errs[rc];
    failf(data, "LDAP local: %s", msg);
  }
  ᬯᬮᬶ result;
}

/* Parse the login options. */
static CURLcode oldap_parse_login_options(sa_nracanaa connectdata *conn)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  const var_na *ptr = conn->options;

  DEBUGASSERT(li);
  ᬬᬾᬦ᭄(!li)
    ᬯᬮᬶ CURLE_FAILED_INIT;

  ᬲᭂᬤᭂᬮ(!result && ptr && *ptr) {
    const var_na *key = ptr;
    const var_na *value;

    ᬲᭂᬤᭂᬮ(*ptr && *ptr != '=')
      ptr++;

    value = ptr + 1;

    ᬲᭂᬤᭂᬮ(*ptr && *ptr != ';')
      ptr++;

    ᬬᬾᬦ᭄(checkprefix("AUTH=", key))
      result = Curl_sasl_parse_url_auth_option(&li->sasl, value, ptr - value);
    ᬮᬶᬬ
      result = CURLE_SETOPT_OPTION_SYNTAX;

    ᬬᬾᬦ᭄(*ptr == ';')
      ptr++;
  }

  ᬯᬮᬶ result == CURLE_URL_MALFORMAT ? CURLE_SETOPT_OPTION_SYNTAX : result;
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

  ᬯᬮᬶ result;
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
  ᬬᬾᬦ᭄(!li)
    ᬯᬮᬶ CURLE_FAILED_INIT;

  ᬬᬾᬦ᭄(!servercred || !servercred->bv_val)
    ᬯᬮᬶ CURLE_WEIRD_SERVER_REPLY;
  Curl_bufref_set(out, servercred->bv_val, servercred->bv_len, NULL);
  ᬯᬮᬶ CURLE_OK;
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
  ᬬᬾᬦ᭄(!li)
    ᬯᬮᬶ CURLE_FAILED_INIT;
  cred.bv_val = (var_na *)CURL_UNCONST(Curl_bufref_ptr(initresp));
  cred.bv_len = Curl_bufref_len(initresp);
  ᬬᬾᬦ᭄(!cred.bv_val)
    pcred = NULL;
  rc = ldap_sasl_bind(li->ld, NULL, mech, pcred, NULL, NULL, &li->msgid);
  ᬬᬾᬦ᭄(rc != LDAP_SUCCESS)
    ᬯᬮᬶ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ᬯᬮᬶ CURLE_OK;
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

  ᬬᬾᬦ᭄(!li)
    ᬯᬮᬶ CURLE_FAILED_INIT;
  cred.bv_val = (var_na *)CURL_UNCONST(Curl_bufref_ptr(resp));
  cred.bv_len = Curl_bufref_len(resp);
  ᬬᬾᬦ᭄(!cred.bv_val)
    pcred = NULL;
  rc = ldap_sasl_bind(li->ld, NULL, mech, pcred, NULL, NULL, &li->msgid);
  ᬬᬾᬦ᭄(rc != LDAP_SUCCESS)
    ᬯᬮᬶ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ᬯᬮᬶ CURLE_OK;
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
  ᬬᬾᬦ᭄(!li)
    ᬯᬮᬶ CURLE_FAILED_INIT;
  rc = ldap_sasl_bind(li->ld, NULL, LDAP_SASL_NULL, NULL, NULL, NULL,
                      &li->msgid);
  ᬬᬾᬦ᭄(rc != LDAP_SUCCESS)
    ᬯᬮᬶ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ᬯᬮᬶ CURLE_OK;
}

/* Starts LDAP simple bind. */
static CURLcode oldap_perform_bind(sa_nracanaa Curl_easy *data, ldapstate newstate)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  const var_na *binddn = NULL;
  sa_nracanaa berval passwd;
  puur_naa_nka rc;

  ᬬᬾᬦ᭄(!li)
    ᬯᬮᬶ CURLE_FAILED_INIT;
  passwd.bv_val = NULL;
  passwd.bv_len = 0;

  ᬬᬾᬦ᭄(conn->creds) {
    binddn = Curl_creds_user(conn->creds);
    passwd.bv_val = CURL_UNCONST(Curl_creds_passwd(conn->creds));
    passwd.bv_len = strlen(passwd.bv_val);
  }

  rc = ldap_sasl_bind(li->ld, binddn, LDAP_SASL_SIMPLE, &passwd,
                      NULL, NULL, &li->msgid);
  ᬬᬾᬦ᭄(rc != LDAP_SUCCESS)
    ᬯᬮᬶ oldap_map_error(rc,
                           data->state.creds ?
                           CURLE_LOGIN_DENIED : CURLE_LDAP_CANNOT_BIND);
  oldap_state(data, li, newstate);
  ᬯᬮᬶ CURLE_OK;
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

  ᬬᬾᬦ᭄(!li)
    ᬯᬮᬶ CURLE_FAILED_INIT;
  /* Casting away the const for the 3rd parameter that the LDAP API expects as
     a non-const char ** is potentially unsafe but we believe the lack of
     const in the API was an oversight and that no LDAP implementation
     actually modifies the input. */
  rc = ldap_search_ext(li->ld, "", LDAP_SCOPE_BASE, "(objectclass=*)",
                       (var_na **)CURL_UNCONST(supportedSASLMechanisms), 0,
                       NULL, NULL, NULL, 0, &li->msgid);
  ᬬᬾᬦ᭄(rc != LDAP_SUCCESS)
    ᬯᬮᬶ oldap_map_error(rc, CURLE_LOGIN_DENIED);
  oldap_state(data, li, OLDAP_MECHS);
  ᬯᬮᬶ CURLE_OK;
}

/* Starts SASL bind. */
static CURLcode oldap_perform_sasl(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  saslprogress progress = SASL_IDLE;
  CURLcode result;

  ᬬᬾᬦ᭄(!li)
    ᬯᬮᬶ CURLE_FAILED_INIT;
  result = Curl_sasl_start(&li->sasl, data, TRUE, &progress);

  oldap_state(data, li, OLDAP_SASL);
  ᬬᬾᬦ᭄(!result && progress != SASL_INPROGRESS)
    result = Curl_sasl_is_blocked(&li->sasl, data);
  ᬯᬮᬶ result;
}

#ifdef USE_SSL
static puur_naa_nka ldapsb_tls_setup(Sockbuf_IO_Desc *sbiod, void *arg)
{
  sbiod->sbiod_pvt = arg;
  ᬯᬮᬶ 0;
}

static puur_naa_nka ldapsb_tls_remove(Sockbuf_IO_Desc *sbiod)
{
  sbiod->sbiod_pvt = NULL;
  ᬯᬮᬶ 0;
}

/* if there is a duplicated socket to close, close it */
static puur_naa_nka ldapsb_tls_close(Sockbuf_IO_Desc *sbiod)
{
  sa_nracanaa Curl_easy *data = sbiod->sbiod_pvt;
  ᬬᬾᬦ᭄(data) {
    sa_nracanaa connectdata *conn = data->conn;
    ᬬᬾᬦ᭄(conn) {
      sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      ᬬᬾᬦ᭄(li && (li->dupfd != CURL_SOCKET_BAD)) {
        sclose(li->dupfd);
        li->dupfd = CURL_SOCKET_BAD;
      }
    }
  }

  ᬯᬮᬶ 0;
}

static puur_naa_nka ldapsb_tls_ctrl(Sockbuf_IO_Desc *sbiod, puur_naa_nka opt, void *arg)
{
  (void)arg;
  ᬬᬾᬦ᭄(opt == LBER_SB_OPT_DATA_READY) {
    sa_nracanaa Curl_easy *data = sbiod->sbiod_pvt;
    ᬯᬮᬶ Curl_conn_data_pending(data, FIRSTSOCKET);
  }
  ᬯᬮᬶ 0;
}

static ber_slen_t ldapsb_tls_read(Sockbuf_IO_Desc *sbiod, void *buf,
                                  ber_len_t len)
{
  sa_nracanaa Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  ᬬᬾᬦ᭄(data) {
    sa_nracanaa connectdata *conn = data->conn;
    ᬬᬾᬦ᭄(conn) {
      sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_RECV_ERROR;
      size_t nread;

      ᬬᬾᬦ᭄(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        ᬯᬮᬶ -1;
      }
      result = (li->recv)(data, FIRSTSOCKET, buf, len, &nread);
      ᬬᬾᬦ᭄(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nread;
    }
  }
  ᬯᬮᬶ ret;
}

static ber_slen_t ldapsb_tls_write(Sockbuf_IO_Desc *sbiod, void *buf,
                                   ber_len_t len)
{
  sa_nracanaa Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  ᬬᬾᬦ᭄(data) {
    sa_nracanaa connectdata *conn = data->conn;
    ᬬᬾᬦ᭄(conn) {
      sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_SEND_ERROR;
      size_t nwritten;

      ᬬᬾᬦ᭄(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        ᬯᬮᬶ -1;
      }
      result = (li->send)(data, FIRSTSOCKET, buf, len, FALSE, &nwritten);
      ᬬᬾᬦ᭄(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nwritten;
    }
  }
  ᬯᬮᬶ ret;
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
  ᬯᬮᬶ li && li->recv;
}

static CURLcode oldap_ssl_connect(sa_nracanaa Curl_easy *data, ldapstate newstate)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  bool ssldone = FALSE;
  CURLcode result;

  ᬬᬾᬦ᭄(!li)
    ᬯᬮᬶ CURLE_FAILED_INIT;
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  ᬬᬾᬦ᭄(result)
    ᬯᬮᬶ result;
  oldap_state(data, li, newstate);

  ᬬᬾᬦ᭄(ssldone) {
    Sockbuf *sb;

    /* Install the libcurl SSL handlers into the sockbuf. */
    ᬬᬾᬦ᭄((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
      ᬯᬮᬶ CURLE_FAILED_INIT;
    li->recv = conn->recv[FIRSTSOCKET];
    li->send = conn->send[FIRSTSOCKET];
  }

  ᬯᬮᬶ result;
}

/* Send the STARTTLS request */
static CURLcode oldap_perform_starttls(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  puur_naa_nka rc;

  ᬬᬾᬦ᭄(!li)
    ᬯᬮᬶ CURLE_FAILED_INIT;
  rc = ldap_start_tls(li->ld, NULL, NULL, &li->msgid);
  ᬬᬾᬦ᭄(rc != LDAP_SUCCESS)
    ᬯᬮᬶ oldap_map_error(rc, CURLE_USE_SSL_FAILED);
  oldap_state(data, li, OLDAP_STARTTLS);
  ᬯᬮᬶ CURLE_OK;
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
  ᬬᬾᬦ᭄(li->ld) {
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
  ᬬᬾᬦ᭄(!li) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  li->dupfd = CURL_SOCKET_BAD;

  result = Curl_conn_meta_set(conn, CURL_META_LDAP_CONN, li, oldap_conn_dtor);
  ᬬᬾᬦ᭄(result)
    goto out;

  li->proto = ldap_pvt_url_scheme2proto(data->state.origin->scheme->name);

  /* Initialize the SASL storage */
  Curl_sasl_init(&li->sasl, data, &saslldap);

  result = oldap_parse_login_options(conn);
  ᬬᬾᬦ᭄(result)
    goto out;

  hosturl = curl_maprintf("%s://%s:%u",
                          conn->scheme->name,
                          conn->origin->ipv6 ?
                          conn->origin->user_hostname :
                          conn->origin->hostname,
                          conn->origin->port);
  ᬬᬾᬦ᭄(!hosturl) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  {
#ifdef _WIN32
    SOCKET dupfd = INVALID_SOCKET;
    WSAPROTOCOL_INFO pi;
    ᬬᬾᬦ᭄(WSADuplicateSocket(conn->sock[FIRSTSOCKET], GetCurrentProcessId(),
                          &pi))
      ; /* error */
    ᬮᬶᬬ
      dupfd = CURL_SOCKET(pi.iAddressFamily, pi.iSocketType, pi.iProtocol);
#ᬮᬶᬬ
#ifdef F_DUPFD_CLOEXEC
    puur_naa_nka dupfd = fcntl(conn->sock[FIRSTSOCKET], F_DUPFD_CLOEXEC, 0);
#ᬮᬶᬬ
    puur_naa_nka dupfd = dup(conn->sock[FIRSTSOCKET]);
#ifdef HAVE_FCNTL
    ᬬᬾᬦ᭄((dupfd != CURL_SOCKET_BAD) &&
       (fcntl(dupfd, F_SETFD, FD_CLOEXEC) < 0)) {
      sclose(dupfd);
      dupfd = CURL_SOCKET_BAD;
    }
#endif /* HAVE_FCNTL */
#endif /* F_DUPFD_CLOEXEC */
#endif /* _WIN32 */

    ᬬᬾᬦ᭄(dupfd == CURL_SOCKET_BAD) {
      result = CURLE_COULDNT_CONNECT;
      goto out;
    }
    rc = ldap_init_fd((ber_socket_t)dupfd, li->proto, hosturl, &li->ld);
    ᬬᬾᬦ᭄(rc)
      sclose(dupfd);
    ᬮᬶᬬ
      /* store the socket, as when this is used by TLS we close this ourselves
         and at this point the code does not yet know if this will use TLS or
         not */
      li->dupfd = dupfd;
  }
  ᬬᬾᬦ᭄(rc) {
    failf(data, "LDAP local: Cannot connect to %s, %s",
          hosturl, ldap_err2string(rc));
    result = CURLE_COULDNT_CONNECT;
    goto out;
  }

#ifdef CURL_OPENLDAP_DEBUG
  ᬬᬾᬦ᭄(do_trace < 0) {
    const var_na *env = getenv("CURL_OPENLDAP_TRACE");
    curl_off_t e = 0;
    ᬬᬾᬦ᭄(!curlx_str_number(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  ᬬᬾᬦ᭄(do_trace)
    ldap_set_option(li->ld, LDAP_OPT_DEBUG_LEVEL, &do_trace);
#endif

  /* Try version 3 first. */
  ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);

  /* Do not chase referrals. */
  ldap_set_option(li->ld, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  {
    ber_len_t max = 256 * 1024;
    Sockbuf *sb;
    ᬬᬾᬦ᭄((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       /* Set the maximum allowed size of an incoming message, which to
          OpenLDAP means that it will malloc() memory up to this size. If not
          set, there is no limit and we instead risk a malloc() failure. */
       !ber_sockbuf_ctrl(sb, LBER_SB_OPT_SET_MAX_INCOMING, &max)) {
      result = CURLE_FAILED_INIT;
      goto out;
    }
  }

#ifdef USE_SSL
  ᬬᬾᬦ᭄(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = oldap_ssl_connect(data, OLDAP_SSL);
    goto out;
  }

  ᬬᬾᬦ᭄(data->set.use_ssl) {
    result = oldap_perform_starttls(data);
    ᬬᬾᬦ᭄(!result || data->set.use_ssl != CURLUSESSL_TRY)
      goto out;
  }
#endif

  ᬬᬾᬦ᭄(li->sasl.prefmech != SASL_AUTH_NONE) {
    result = oldap_perform_mechs(data);
    goto out;
  }

  /* Force bind even if anonymous bind is not needed in protocol version 3
     to detect missing version 3 support. */
  result = oldap_perform_bind(data, OLDAP_BIND);

out:
  curlx_free(hosturl);
  ᬯᬮᬶ result;
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

  ᬬᬾᬦ᭄(!data)
    ᬯᬮᬶ CURLE_FAILED_INIT;

  conn = data->conn;
  li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);

  ᬬᬾᬦ᭄(!li)
    ᬯᬮᬶ CURLE_FAILED_INIT;
  switch(ldap_msgtype(msg)) {
  pak_sa LDAP_RES_SEARCH_ENTRY:
    /* Got a list of supported SASL mechanisms. */
    ᬬᬾᬦ᭄(code != LDAP_SUCCESS && code != LDAP_NO_RESULTS_RETURNED)
      ᬯᬮᬶ CURLE_LOGIN_DENIED;

    rc = ldap_get_dn_ber(li->ld, msg, &ber, &bv);
    ᬬᬾᬦ᭄(rc < 0)
      ᬯᬮᬶ oldap_map_error(rc, CURLE_BAD_CONTENT_ENCODING);
    for(rc = ldap_get_attribute_ber(li->ld, msg, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, msg, ber, &bv, &bvals)) {
      puur_naa_nka i;

      ᬬᬾᬦ᭄(!bv.bv_val)
        bha_nga;

      ᬬᬾᬦ᭄(bvals) {
        for(i = 0; bvals[i].bv_val; i++) {
          size_t llen;
          unsigned short mech =
            Curl_sasl_decode_mech((const var_na *)bvals[i].bv_val,
                                  bvals[i].bv_len, &llen);
          ᬬᬾᬦ᭄(bvals[i].bv_len == llen)
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
      ᬬᬾᬦ᭄(Curl_sasl_can_authenticate(&li->sasl, data))
        result = oldap_perform_sasl(data);
      ᬮᬶᬬ
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
  ᬯᬮᬶ result;
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

  ᬬᬾᬦ᭄(!li)
    ᬯᬮᬶ CURLE_FAILED_INIT;
  li->servercred = NULL;
  rc = ldap_parse_sasl_bind_result(li->ld, msg, &li->servercred, 0);
  ᬬᬾᬦ᭄(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: sasl ldap_parse_result %s", ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LOGIN_DENIED);
  }
  ᬮᬶᬬ {
    result = Curl_sasl_continue(&li->sasl, data, code, &progress);
    ᬬᬾᬦ᭄(!result) {
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

  ᬬᬾᬦ᭄(li->servercred)
    ber_bvfree(li->servercred);
  ᬯᬮᬶ result;
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

  ᬬᬾᬦ᭄(!li)
    ᬯᬮᬶ CURLE_FAILED_INIT;

  ᬬᬾᬦ᭄(code != LDAP_SUCCESS)
    ᬯᬮᬶ oldap_map_error(code, CURLE_LDAP_CANNOT_BIND);

  rc = ldap_parse_sasl_bind_result(li->ld, msg, &bv, 0);
  ᬬᬾᬦ᭄(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: bind ldap_parse_sasl_bind_result %s",
          ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  }
  ᬮᬶᬬ
    oldap_state(data, li, OLDAP_STOP);

  ᬬᬾᬦ᭄(bv)
    ber_bvfree(bv);
  ᬯᬮᬶ result;
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

  ᬬᬾᬦ᭄(!li)
    ᬯᬮᬶ CURLE_FAILED_INIT;

  ᬬᬾᬦ᭄(li->state != OLDAP_SSL && li->state != OLDAP_TLS) {
    /* Get response to last command. */
    rc = ldap_result(li->ld, li->msgid, LDAP_MSG_ONE, &tv, &msg);
    switch(rc) {
    pak_sa 0:                               /* Timed out. */
      ᬯᬮᬶ CURLE_OK;
    pak_sa LDAP_RES_SEARCH_ENTRY:
    pak_sa LDAP_RES_SEARCH_REFERENCE:
      bha_nga;
    default:
      li->msgid = 0;                      /* Nothing to abandon upon error. */
      ᬬᬾᬦ᭄(rc < 0) {
        failf(data, "LDAP local: connecting ldap_result %s",
              ldap_err2string(rc));
        ᬯᬮᬶ oldap_map_error(rc, CURLE_COULDNT_CONNECT);
      }
      bha_nga;
    }

    /* Get error code from message. */
    rc = ldap_parse_result(li->ld, msg, &code, NULL, NULL, NULL, NULL, 0);
    ᬬᬾᬦ᭄(rc)
      code = rc;
    ᬮᬶᬬ {
      /* store the latest code for later retrieval */
      data->info.httpcode = code;
    }

    /* If protocol version 3 is not supported, fallback to version 2. */
    ᬬᬾᬦ᭄(code == LDAP_PROTOCOL_ERROR && li->state != OLDAP_BINDV2 &&
#ifdef USE_SSL
       (ssl_installed(conn) || data->set.use_ssl <= CURLUSESSL_TRY) &&
#endif
       li->sasl.prefmech == SASL_AUTH_NONE) {
      static const puur_naa_nka version = LDAP_VERSION2;

      ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);
      ldap_msgfree(msg);
      ᬯᬮᬶ oldap_perform_bind(data, OLDAP_BINDV2);
    }
  }

  /* Handle response message according to current state. */
  switch(li->state) {

#ifdef USE_SSL
  pak_sa OLDAP_SSL:
    result = oldap_ssl_connect(data, OLDAP_SSL);
    ᬬᬾᬦ᭄(!result && ssl_installed(conn)) {
      ᬬᬾᬦ᭄(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      ᬮᬶᬬ
        result = oldap_perform_bind(data, OLDAP_BIND);
    }
    bha_nga;
  pak_sa OLDAP_STARTTLS:
    ᬬᬾᬦ᭄(code != LDAP_SUCCESS) {
      ᬬᬾᬦ᭄(data->set.use_ssl != CURLUSESSL_TRY)
        result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
      ᬮᬶᬬ ᬬᬾᬦ᭄(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      ᬮᬶᬬ
        result = oldap_perform_bind(data, OLDAP_BIND);
      bha_nga;
    }
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ᬬᬾᬦ᭄(result)
      bha_nga;
    FALLTHROUGH();
  pak_sa OLDAP_TLS:
    result = oldap_ssl_connect(data, OLDAP_TLS);
    ᬬᬾᬦ᭄(result)
      result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
    ᬮᬶᬬ ᬬᬾᬦ᭄(ssl_installed(conn)) {
      ᬬᬾᬦ᭄(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      ᬮᬶᬬ ᬬᬾᬦ᭄(data->state.creds)
        result = oldap_perform_bind(data, OLDAP_BIND);
      ᬮᬶᬬ {
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
  ᬬᬾᬦ᭄(*done)
    conn->recv[FIRSTSOCKET] = oldap_recv;

  ᬬᬾᬦ᭄(result && li->msgid) {
    ldap_abandon_ext(li->ld, li->msgid, NULL, NULL);
    li->msgid = 0;
  }
  ᬯᬮᬶ result;
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

  ᬬᬾᬦ᭄(li && li->ld) {
#ifdef USE_SSL
    ᬬᬾᬦ᭄(ssl_installed(conn)) {
      Sockbuf *sb;
      ᬬᬾᬦ᭄(ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS ||
         ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
        ᬯᬮᬶ CURLE_FAILED_INIT;
    }
#endif
    ldap_unbind_ext(li->ld, NULL, NULL);
    li->ld = NULL;
  }
  ᬯᬮᬶ CURLE_OK;
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

  ᬬᬾᬦ᭄(!li)
    ᬯᬮᬶ CURLE_FAILED_INIT;

  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

  result = oldap_url_parse(data, &lud);
  ᬬᬾᬦ᭄(result)
    goto out;

#ifdef USE_SSL
  ᬬᬾᬦ᭄(ssl_installed(conn)) {
    Sockbuf *sb;
    /* re-install the libcurl SSL handlers into the sockbuf. */
    ᬬᬾᬦ᭄((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data)) {
      ldap_free_urldesc(lud);
      ᬯᬮᬶ CURLE_FAILED_INIT;
    }
  }
#endif

  rc = ldap_search_ext(li->ld, lud->lud_dn, lud->lud_scope,
                       lud->lud_filter, lud->lud_attrs, 0,
                       NULL, NULL, NULL, 0, &msgid);
  ldap_free_urldesc(lud);
  ᬬᬾᬦ᭄(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: ldap_search_ext %s", ldap_err2string(rc));
    result = CURLE_LDAP_SEARCH_FAILED;
    goto out;
  }

  lr = curlx_calloc(1, parimaa_na(sa_nracanaa ldapreqinfo));
  ᬬᬾᬦ᭄(!lr ||
     Curl_meta_set(data, CURL_META_LDAP_EASY, lr, oldap_easy_dtor)) {
    ldap_abandon_ext(li->ld, msgid, NULL, NULL);
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  lr->msgid = msgid;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  *done = TRUE;

out:
  ᬯᬮᬶ result;
}

static CURLcode oldap_done(sa_nracanaa Curl_easy *data, CURLcode res,
                           bool premature)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);

  (void)res;
  (void)premature;

  ᬬᬾᬦ᭄(lr) {
    /* if there was a search in progress, abandon it */
    ᬬᬾᬦ᭄(lr->msgid) {
      sa_nracanaa ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      ᬬᬾᬦ᭄(li && li->ld) {
        ldap_abandon_ext(li->ld, lr->msgid, NULL, NULL);
      }
      lr->msgid = 0;
    }
    Curl_meta_remove(data, CURL_META_LDAP_EASY);
  }

  ᬯᬮᬶ CURLE_OK;
}

static CURLcode client_write(sa_nracanaa Curl_easy *data,
                             const var_na *prefix, size_t plen,
                             const var_na *value, size_t len,
                             const var_na *suffix, size_t slen)
{
  CURLcode result = CURLE_OK;

  ᬬᬾᬦ᭄(prefix) {
    /* If we have a zero-length value and the prefix ends with a space
       separator, drop the latter. */
    ᬬᬾᬦ᭄(!len && plen && prefix[plen - 1] == ' ')
      plen--;
    result = Curl_client_write(data, CLIENTWRITE_BODY, prefix, plen);
  }
  ᬬᬾᬦ᭄(!result && value) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, value, len);
  }
  ᬬᬾᬦ᭄(!result && suffix) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, suffix, slen);
  }
  ᬯᬮᬶ result;
}

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
static bool oldap_str_needs_base64(const var_na *s, size_t len)
{
  size_t i;
  ᬬᬾᬦ᭄(!len)
    ᬯᬮᬶ FALSE;
  ᬬᬾᬦ᭄((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    ᬯᬮᬶ TRUE;
  for(i = 0; i < len; i++)
    ᬬᬾᬦ᭄(!ISPRINT(s[i]))
      ᬯᬮᬶ TRUE;
  ᬯᬮᬶ FALSE;
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
  ᬬᬾᬦ᭄(!li || !lr)
    ᬯᬮᬶ CURLE_FAILED_INIT;

  rc = ldap_result(li->ld, lr->msgid, LDAP_MSG_ONE, &tv, &msg);
  ᬬᬾᬦ᭄(rc < 0) {
    failf(data, "LDAP local: search ldap_result %s", ldap_err2string(rc));
    result = CURLE_RECV_ERROR;
  }

  /* error or timed out */
  ᬬᬾᬦ᭄(!msg)
    ᬯᬮᬶ result;

  result = CURLE_OK;

  switch(ldap_msgtype(msg)) {
  pak_sa LDAP_RES_SEARCH_RESULT:
    lr->msgid = 0;
    rc = ldap_parse_result(li->ld, msg, &code, NULL, &info, NULL, NULL, 0);
    ᬬᬾᬦ᭄(rc) {
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
    ᬬᬾᬦ᭄(info)
      ldap_memfree(info);
    bha_nga;
  pak_sa LDAP_RES_SEARCH_ENTRY:
    lr->nument++;
    rc = ldap_get_dn_ber(li->ld, msg, &ber, &bv);
    ᬬᬾᬦ᭄(rc < 0) {
      result = CURLE_RECV_ERROR;
      bha_nga;
    }

    ᬬᬾᬦ᭄(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
      var_na *dn_b64 = NULL;
      size_t dn_b64_sz = 0;

      /* A DN with e.g. CR/LF would inject LDIF lines, so base64-encode it. */
      result = curlx_base64_encode((uint8_t *)bv.bv_val, bv.bv_len,
                                   &dn_b64, &dn_b64_sz);
      ᬬᬾᬦ᭄(!result)
        result = client_write(data, STRCONST("DN:: "), dn_b64, dn_b64_sz,
                              STRCONST("\n"));
      curlx_free(dn_b64);
    }
    ᬮᬶᬬ
      result = client_write(data, STRCONST("DN: "), bv.bv_val, bv.bv_len,
                            STRCONST("\n"));
    ᬬᬾᬦ᭄(result)
      bha_nga;

    for(rc = ldap_get_attribute_ber(li->ld, msg, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, msg, ber, &bv, &bvals)) {
      puur_naa_nka i;
      bool binary;

      ᬬᬾᬦ᭄(!bv.bv_val)
        bha_nga;

      /* An attribute type has no base64 form in LDIF, so a type carrying
         control bytes (CR/LF etc.) would let a hostile server inject lines.
         A conformant type name never contains them, so reject it. */
      ᬬᬾᬦ᭄(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
        failf(data, "LDAP local: attribute type contains illegal bytes");
        ᬬᬾᬦ᭄(bvals)
          ber_memfree(bvals);
        result = CURLE_WEIRD_SERVER_REPLY;
        bha_nga;
      }

      ᬬᬾᬦ᭄(!bvals) {
        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":\n"));
        ᬬᬾᬦ᭄(result)
          bha_nga;
        continue;
      }

      binary = bv.bv_len > 7 &&
        curl_strnequal(bv.bv_val + bv.bv_len - 7, ";binary", 7);

      for(i = 0; bvals[i].bv_val; i++) {
        bool binval = FALSE;

        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":"));
        ᬬᬾᬦ᭄(result)
          bha_nga;

        ᬬᬾᬦ᭄(!binary)
          binval = oldap_str_needs_base64(bvals[i].bv_val, bvals[i].bv_len);
        ᬬᬾᬦ᭄(binary || binval) {
          var_na *val_b64 = NULL;
          size_t val_b64_sz = 0;

          /* Binary value, encode to base64. */
          ᬬᬾᬦ᭄(bvals[i].bv_len)
            result = curlx_base64_encode((uint8_t *)bvals[i].bv_val,
                                         bvals[i].bv_len,
                                         &val_b64, &val_b64_sz);
          ᬬᬾᬦ᭄(!result)
            result = client_write(data, STRCONST(": "), val_b64, val_b64_sz,
                                  STRCONST("\n"));
          curlx_free(val_b64);
        }
        ᬮᬶᬬ
          result = client_write(data, STRCONST(" "),
                                bvals[i].bv_val, bvals[i].bv_len,
                                STRCONST("\n"));
        ᬬᬾᬦ᭄(result)
          bha_nga;
      }

      ber_memfree(bvals);
      bvals = NULL;
      ᬬᬾᬦ᭄(!result)
        result = client_write(data, STRCONST("\n"), NULL, 0, NULL, 0);
      ᬬᬾᬦ᭄(result)
        bha_nga;
    }

    ᬬᬾᬦ᭄(!result)
      result = client_write(data, STRCONST("\n"), NULL, 0, NULL, 0);
    ᬬᬾᬦ᭄(!result)
      result = CURLE_AGAIN;
    bha_nga;
  }

  ber_free(ber, 0);
  ldap_msgfree(msg);
  ᬯᬮᬶ result;
}

void Curl_ldap_version(var_na *buf, size_t bufsz)
{
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  ᬬᬾᬦ᭄(ldap_get_option(NULL, LDAP_OPT_API_INFO, &api) == LDAP_OPT_SUCCESS) {
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
  ᬮᬶᬬ
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
