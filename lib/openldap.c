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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP)

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

#അന്തര്ഭാവ <ldap.h>

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "curl_ldap.h"
#അന്തര്ഭാവ "curlx/base64.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "curl_sasl.h"
#അന്തര്ഭാവ "strcase.h"
#അന്തര്ഭാവ "bufref.h"

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
തരനിർവചനം എണ്ണൽ {
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
ബാഹ്യ പൂർണ്ണസംഖ്യ ldap_pvt_url_scheme2proto(സ്ഥിരം അക്ഷരം *);
ബാഹ്യ പൂർണ്ണസംഖ്യ ldap_init_fd(ber_socket_t fd, പൂർണ്ണസംഖ്യ proto, സ്ഥിരം അക്ഷരം *url,
                        LDAP **ld);
#endif

സ്ഥിതം Curl_recv oldap_recv;

ഘടന ldapconninfo {
  ഘടന SASL sasl;          /* SASL-related parameters */
  LDAP *ld;                  /* Openldap connection handle. */
  Curl_recv *recv;           /* For stacking SSL handler */
  Curl_send *send;
  ഘടന berval *servercred; /* SASL data from server. */
  ldapstate state;           /* Current machine state. */
  curl_socket_t dupfd;       /* duplicated socket */
  പൂർണ്ണസംഖ്യ proto;                 /* LDAP_PROTO_TCP/LDAP_PROTO_UDP/LDAP_PROTO_IPC */
  പൂർണ്ണസംഖ്യ msgid;                 /* Current message id. */
};

ഘടന ldapreqinfo {
  പൂർണ്ണസംഖ്യ msgid;
  പൂർണ്ണസംഖ്യ nument;
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
സ്ഥിതം ശൂന്യം oldap_state(ഘടന Curl_easy *data, ഘടന ldapconninfo *li,
                        ldapstate newstate)
{
#എങ്കിൽ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം names[] = {
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

  എങ്കിൽ(li->state != newstate)
    infof(data, "LDAP %p state change from %s to %s",
          (ശൂന്യം *)li, names[li->state], names[newstate]);
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
#endif
  li->state = newstate;
}

/* Map some particular LDAP error codes to CURLcode values. */
സ്ഥിതം CURLcode oldap_map_error(പൂർണ്ണസംഖ്യ rc, CURLcode result)
{
  തിരഞ്ഞെടുപ്പ്(rc) {
  അവസ്ഥ LDAP_NO_MEMORY:
    തിരികെ CURLE_OUT_OF_MEMORY;
  അവസ്ഥ LDAP_INVALID_CREDENTIALS:
    തിരികെ CURLE_LOGIN_DENIED;
  അവസ്ഥ LDAP_PROTOCOL_ERROR:
    തിരികെ CURLE_UNSUPPORTED_PROTOCOL;
  അവസ്ഥ LDAP_INSUFFICIENT_ACCESS:
    തിരികെ CURLE_REMOTE_ACCESS_DENIED;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode oldap_url_parse(ഘടന Curl_easy *data, LDAPURLDesc **ludp)
{
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ rc;
  സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം url_errs[] = {
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

  *ludp = ശൂന്യ;
  /* `ldap_url_parse() seems to be terrible with urls
   * that have user/pass/options in it. So when we have options or
   * creds from the url, fail without calling the function.
   * Yes, this is super-weird code and I do not like it. */
  എങ്കിൽ((data->state.creds && (data->state.creds->source == CREDS_URL)) ||
     data->state.up.options)
    rc = LDAP_URL_ERR_BADURL;
  അല്ലെങ്കിൽ
    rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), ludp);

  എങ്കിൽ(rc != LDAP_URL_SUCCESS) {
    സ്ഥിരം അക്ഷരം *സന്ദേശം = "url parsing problem";

    result = rc == LDAP_URL_ERR_MEM ? CURLE_OUT_OF_MEMORY :
      CURLE_URL_MALFORMAT;
    rc -= LDAP_URL_SUCCESS;
    എങ്കിൽ((size_t)rc < CURL_ARRAYSIZE(url_errs))
      സന്ദേശം = url_errs[rc];
    failf(data, "LDAP local: %s", സന്ദേശം);
  }
  തിരികെ result;
}

/* Parse the login options. */
സ്ഥിതം CURLcode oldap_parse_login_options(ഘടന connectdata *conn)
{
  CURLcode result = CURLE_OK;
  ഘടന ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  സ്ഥിരം അക്ഷരം *ptr = conn->options;

  DEBUGASSERT(li);
  എങ്കിൽ(!li)
    തിരികെ CURLE_FAILED_INIT;

  ആകുമ്പോൾ(!result && ptr && *ptr) {
    സ്ഥിരം അക്ഷരം *താക്കോൽ = ptr;
    സ്ഥിരം അക്ഷരം *മൂല്യം;

    ആകുമ്പോൾ(*ptr && *ptr != '=')
      ptr++;

    മൂല്യം = ptr + 1;

    ആകുമ്പോൾ(*ptr && *ptr != ';')
      ptr++;

    എങ്കിൽ(checkprefix("AUTH=", താക്കോൽ))
      result = Curl_sasl_parse_url_auth_option(&li->sasl, മൂല്യം, ptr - മൂല്യം);
    അല്ലെങ്കിൽ
      result = CURLE_SETOPT_OPTION_SYNTAX;

    എങ്കിൽ(*ptr == ';')
      ptr++;
  }

  തിരികെ result == CURLE_URL_MALFORMAT ? CURLE_SETOPT_OPTION_SYNTAX : result;
}

സ്ഥിതം CURLcode oldap_setup_connection(ഘടന Curl_easy *data,
                                       ഘടന connectdata *conn)
{
  CURLcode result;
  LDAPURLDesc *lud;
  (ശൂന്യം)conn;

  /* Early URL syntax check. */
  result = oldap_url_parse(data, &lud);
  ldap_free_urldesc(lud);

  തിരികെ result;
}

/*
 * Get the SASL authentication challenge from the server credential buffer.
 */
സ്ഥിതം CURLcode oldap_get_message(ഘടന Curl_easy *data, ഘടന bufref *out)
{
  ഘടന ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  ഘടന berval *servercred = li ? li->servercred : ശൂന്യ;
  DEBUGASSERT(li);
  എങ്കിൽ(!li)
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ(!servercred || !servercred->bv_val)
    തിരികെ CURLE_WEIRD_SERVER_REPLY;
  Curl_bufref_set(out, servercred->bv_val, servercred->bv_len, ശൂന്യ);
  തിരികെ CURLE_OK;
}

/*
 * Sends an initial SASL bind request to the server.
 */
സ്ഥിതം CURLcode oldap_perform_auth(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *mech,
                                   സ്ഥിരം ഘടന bufref *initresp)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ഘടന berval cred;
  ഘടന berval *pcred = &cred;
  പൂർണ്ണസംഖ്യ rc;

  DEBUGASSERT(li);
  എങ്കിൽ(!li)
    തിരികെ CURLE_FAILED_INIT;
  cred.bv_val = (അക്ഷരം *)CURL_UNCONST(Curl_bufref_ptr(initresp));
  cred.bv_len = Curl_bufref_len(initresp);
  എങ്കിൽ(!cred.bv_val)
    pcred = ശൂന്യ;
  rc = ldap_sasl_bind(li->ld, ശൂന്യ, mech, pcred, ശൂന്യ, ശൂന്യ, &li->msgid);
  എങ്കിൽ(rc != LDAP_SUCCESS)
    തിരികെ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  തിരികെ CURLE_OK;
}

/*
 * Sends SASL continuation.
 */
സ്ഥിതം CURLcode oldap_continue_auth(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *mech,
                                    സ്ഥിരം ഘടന bufref *resp)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ഘടന berval cred;
  ഘടന berval *pcred = &cred;
  പൂർണ്ണസംഖ്യ rc;

  എങ്കിൽ(!li)
    തിരികെ CURLE_FAILED_INIT;
  cred.bv_val = (അക്ഷരം *)CURL_UNCONST(Curl_bufref_ptr(resp));
  cred.bv_len = Curl_bufref_len(resp);
  എങ്കിൽ(!cred.bv_val)
    pcred = ശൂന്യ;
  rc = ldap_sasl_bind(li->ld, ശൂന്യ, mech, pcred, ശൂന്യ, ശൂന്യ, &li->msgid);
  എങ്കിൽ(rc != LDAP_SUCCESS)
    തിരികെ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  തിരികെ CURLE_OK;
}

/*
 * Sends SASL bind cancellation.
 */
സ്ഥിതം CURLcode oldap_cancel_auth(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *mech)
{
  ഘടന ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  പൂർണ്ണസംഖ്യ rc;

  (ശൂന്യം)mech;
  എങ്കിൽ(!li)
    തിരികെ CURLE_FAILED_INIT;
  rc = ldap_sasl_bind(li->ld, ശൂന്യ, LDAP_SASL_NULL, ശൂന്യ, ശൂന്യ, ശൂന്യ,
                      &li->msgid);
  എങ്കിൽ(rc != LDAP_SUCCESS)
    തിരികെ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  തിരികെ CURLE_OK;
}

/* Starts LDAP simple bind. */
സ്ഥിതം CURLcode oldap_perform_bind(ഘടന Curl_easy *data, ldapstate newstate)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  സ്ഥിരം അക്ഷരം *binddn = ശൂന്യ;
  ഘടന berval passwd;
  പൂർണ്ണസംഖ്യ rc;

  എങ്കിൽ(!li)
    തിരികെ CURLE_FAILED_INIT;
  passwd.bv_val = ശൂന്യ;
  passwd.bv_len = 0;

  എങ്കിൽ(conn->creds) {
    binddn = Curl_creds_user(conn->creds);
    passwd.bv_val = CURL_UNCONST(Curl_creds_passwd(conn->creds));
    passwd.bv_len = strlen(passwd.bv_val);
  }

  rc = ldap_sasl_bind(li->ld, binddn, LDAP_SASL_SIMPLE, &passwd,
                      ശൂന്യ, ശൂന്യ, &li->msgid);
  എങ്കിൽ(rc != LDAP_SUCCESS)
    തിരികെ oldap_map_error(rc,
                           data->state.creds ?
                           CURLE_LOGIN_DENIED : CURLE_LDAP_CANNOT_BIND);
  oldap_state(data, li, newstate);
  തിരികെ CURLE_OK;
}

/* Query the supported SASL authentication mechanisms. */
സ്ഥിതം CURLcode oldap_perform_mechs(ഘടന Curl_easy *data)
{
  ഘടന ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  പൂർണ്ണസംഖ്യ rc;
  സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം supportedSASLMechanisms[] = {
    "supportedSASLMechanisms",
    ശൂന്യ
  };

  എങ്കിൽ(!li)
    തിരികെ CURLE_FAILED_INIT;
  /* Casting away the const for the 3rd parameter that the LDAP API expects as
     a non-const char ** is potentially unsafe but we believe the lack of
     const in the API was an oversight and that no LDAP implementation
     actually modifies the input. */
  rc = ldap_തിരയൽ_ext(li->ld, "", LDAP_SCOPE_BASE, "(objectclass=*)",
                       (അക്ഷരം **)CURL_UNCONST(supportedSASLMechanisms), 0,
                       ശൂന്യ, ശൂന്യ, ശൂന്യ, 0, &li->msgid);
  എങ്കിൽ(rc != LDAP_SUCCESS)
    തിരികെ oldap_map_error(rc, CURLE_LOGIN_DENIED);
  oldap_state(data, li, OLDAP_MECHS);
  തിരികെ CURLE_OK;
}

/* Starts SASL bind. */
സ്ഥിതം CURLcode oldap_perform_sasl(ഘടന Curl_easy *data)
{
  ഘടന ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  saslprogress progress = SASL_IDLE;
  CURLcode result;

  എങ്കിൽ(!li)
    തിരികെ CURLE_FAILED_INIT;
  result = Curl_sasl_start(&li->sasl, data, TRUE, &progress);

  oldap_state(data, li, OLDAP_SASL);
  എങ്കിൽ(!result && progress != SASL_INPROGRESS)
    result = Curl_sasl_is_blocked(&li->sasl, data);
  തിരികെ result;
}

#ifdef USE_SSL
സ്ഥിതം പൂർണ്ണസംഖ്യ ldapsb_tls_setup(Sockbuf_IO_Desc *sbiod, ശൂന്യം *arg)
{
  sbiod->sbiod_pvt = arg;
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ ldapsb_tls_remove(Sockbuf_IO_Desc *sbiod)
{
  sbiod->sbiod_pvt = ശൂന്യ;
  തിരികെ 0;
}

/* if there is a duplicated socket to close, close it */
സ്ഥിതം പൂർണ്ണസംഖ്യ ldapsb_tls_close(Sockbuf_IO_Desc *sbiod)
{
  ഘടന Curl_easy *data = sbiod->sbiod_pvt;
  എങ്കിൽ(data) {
    ഘടന connectdata *conn = data->conn;
    എങ്കിൽ(conn) {
      ഘടന ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      എങ്കിൽ(li && (li->dupfd != CURL_SOCKET_BAD)) {
        sclose(li->dupfd);
        li->dupfd = CURL_SOCKET_BAD;
      }
    }
  }

  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ ldapsb_tls_ctrl(Sockbuf_IO_Desc *sbiod, പൂർണ്ണസംഖ്യ opt, ശൂന്യം *arg)
{
  (ശൂന്യം)arg;
  എങ്കിൽ(opt == LBER_SB_OPT_DATA_READY) {
    ഘടന Curl_easy *data = sbiod->sbiod_pvt;
    തിരികെ Curl_conn_data_pending(data, FIRSTSOCKET);
  }
  തിരികെ 0;
}

സ്ഥിതം ber_slen_t ldapsb_tls_read(Sockbuf_IO_Desc *sbiod, ശൂന്യം *buf,
                                  ber_len_t len)
{
  ഘടന Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  എങ്കിൽ(data) {
    ഘടന connectdata *conn = data->conn;
    എങ്കിൽ(conn) {
      ഘടന ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_RECV_ERROR;
      size_t nread;

      എങ്കിൽ(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        തിരികെ -1;
      }
      result = (li->recv)(data, FIRSTSOCKET, buf, len, &nread);
      എങ്കിൽ(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nread;
    }
  }
  തിരികെ ret;
}

സ്ഥിതം ber_slen_t ldapsb_tls_write(Sockbuf_IO_Desc *sbiod, ശൂന്യം *buf,
                                   ber_len_t len)
{
  ഘടന Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  എങ്കിൽ(data) {
    ഘടന connectdata *conn = data->conn;
    എങ്കിൽ(conn) {
      ഘടന ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_SEND_ERROR;
      size_t nwritten;

      എങ്കിൽ(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        തിരികെ -1;
      }
      result = (li->send)(data, FIRSTSOCKET, buf, len, FALSE, &nwritten);
      എങ്കിൽ(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nwritten;
    }
  }
  തിരികെ ret;
}

സ്ഥിതം Sockbuf_IO ldapsb_tls = {
  ldapsb_tls_setup,
  ldapsb_tls_remove,
  ldapsb_tls_ctrl,
  ldapsb_tls_read,
  ldapsb_tls_write,
  ldapsb_tls_close
};

സ്ഥിതം bool ssl_installed(ഘടന connectdata *conn)
{
  ഘടന ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  തിരികെ li && li->recv;
}

സ്ഥിതം CURLcode oldap_ssl_connect(ഘടന Curl_easy *data, ldapstate newstate)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  bool ssldone = FALSE;
  CURLcode result;

  എങ്കിൽ(!li)
    തിരികെ CURLE_FAILED_INIT;
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  എങ്കിൽ(result)
    തിരികെ result;
  oldap_state(data, li, newstate);

  എങ്കിൽ(ssldone) {
    Sockbuf *sb;

    /* Install the libcurl SSL handlers into the sockbuf. */
    എങ്കിൽ((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
      തിരികെ CURLE_FAILED_INIT;
    li->recv = conn->recv[FIRSTSOCKET];
    li->send = conn->send[FIRSTSOCKET];
  }

  തിരികെ result;
}

/* Send the STARTTLS request */
സ്ഥിതം CURLcode oldap_perform_starttls(ഘടന Curl_easy *data)
{
  ഘടന ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  പൂർണ്ണസംഖ്യ rc;

  എങ്കിൽ(!li)
    തിരികെ CURLE_FAILED_INIT;
  rc = ldap_start_tls(li->ld, ശൂന്യ, ശൂന്യ, &li->msgid);
  എങ്കിൽ(rc != LDAP_SUCCESS)
    തിരികെ oldap_map_error(rc, CURLE_USE_SSL_FAILED);
  oldap_state(data, li, OLDAP_STARTTLS);
  തിരികെ CURLE_OK;
}
#endif

സ്ഥിതം ശൂന്യം oldap_easy_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന ldapreqinfo *lr = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  curlx_free(lr);
}

സ്ഥിതം ശൂന്യം oldap_conn_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന ldapconninfo *li = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  എങ്കിൽ(li->ld) {
    ldap_unbind_ext(li->ld, ശൂന്യ, ശൂന്യ);
    li->ld = ശൂന്യ;
  }
  curlx_free(li);
}

/* SASL parameters for the ldap protocol */
സ്ഥിതം സ്ഥിരം ഘടന SASLproto saslldap = {
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

സ്ഥിതം CURLcode oldap_connect(ഘടന Curl_easy *data, bool *done)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ldapconninfo *li;
  സ്ഥിതം സ്ഥിരം പൂർണ്ണസംഖ്യ version = LDAP_VERSION3;
  അക്ഷരം *hosturl = ശൂന്യ;
  CURLcode result;
  പൂർണ്ണസംഖ്യ rc;
#ifdef CURL_OPENLDAP_DEBUG
  സ്ഥിതം പൂർണ്ണസംഖ്യ do_trace = -1;
#endif

  (ശൂന്യം)done;

  li = curlx_calloc(1, വലുപ്പം(ഘടന ldapconninfo));
  എങ്കിൽ(!li) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  li->dupfd = CURL_SOCKET_BAD;

  result = Curl_conn_meta_set(conn, CURL_META_LDAP_CONN, li, oldap_conn_dtor);
  എങ്കിൽ(result)
    പോകുക out;

  li->proto = ldap_pvt_url_scheme2proto(data->state.origin->scheme->name);

  /* Initialize the SASL storage */
  Curl_sasl_init(&li->sasl, data, &saslldap);

  result = oldap_parse_login_options(conn);
  എങ്കിൽ(result)
    പോകുക out;

  hosturl = curl_maprintf("%s://%s:%u",
                          conn->scheme->name,
                          conn->origin->ipv6 ?
                          conn->origin->user_hostname :
                          conn->origin->hostname,
                          conn->origin->port);
  എങ്കിൽ(!hosturl) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  {
#ifdef _WIN32
    SOCKET dupfd = INVALID_SOCKET;
    WSAPROTOCOL_INFO pi;
    എങ്കിൽ(WSADuplicateSocket(conn->sock[FIRSTSOCKET], GetCurrentProcessId(),
                          &pi))
      ; /* error */
    അല്ലെങ്കിൽ
      dupfd = CURL_SOCKET(pi.iAddressFamily, pi.iSocketType, pi.iProtocol);
#അല്ലെങ്കിൽ
#ifdef F_DUPFD_CLOEXEC
    പൂർണ്ണസംഖ്യ dupfd = fcntl(conn->sock[FIRSTSOCKET], F_DUPFD_CLOEXEC, 0);
#അല്ലെങ്കിൽ
    പൂർണ്ണസംഖ്യ dupfd = dup(conn->sock[FIRSTSOCKET]);
#ifdef HAVE_FCNTL
    എങ്കിൽ((dupfd != CURL_SOCKET_BAD) &&
       (fcntl(dupfd, F_SETFD, FD_CLOEXEC) < 0)) {
      sclose(dupfd);
      dupfd = CURL_SOCKET_BAD;
    }
#endif /* HAVE_FCNTL */
#endif /* F_DUPFD_CLOEXEC */
#endif /* _WIN32 */

    എങ്കിൽ(dupfd == CURL_SOCKET_BAD) {
      result = CURLE_COULDNT_CONNECT;
      പോകുക out;
    }
    rc = ldap_init_fd((ber_socket_t)dupfd, li->proto, hosturl, &li->ld);
    എങ്കിൽ(rc)
      sclose(dupfd);
    അല്ലെങ്കിൽ
      /* store the socket, as when this is used by TLS we close this ourselves
         and at this point the code does not yet know if this will use TLS or
         not */
      li->dupfd = dupfd;
  }
  എങ്കിൽ(rc) {
    failf(data, "LDAP local: Cannot connect to %s, %s",
          hosturl, ldap_err2string(rc));
    result = CURLE_COULDNT_CONNECT;
    പോകുക out;
  }

#ifdef CURL_OPENLDAP_DEBUG
  എങ്കിൽ(do_trace < 0) {
    സ്ഥിരം അക്ഷരം *env = getenv("CURL_OPENLDAP_TRACE");
    curl_off_t e = 0;
    എങ്കിൽ(!curlx_str_സംഖ്യ(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  എങ്കിൽ(do_trace)
    ldap_set_option(li->ld, LDAP_OPT_DEBUG_LEVEL, &do_trace);
#endif

  /* Try version 3 first. */
  ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);

  /* Do not chase referrals. */
  ldap_set_option(li->ld, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  {
    ber_len_t max = 256 * 1024;
    Sockbuf *sb;
    എങ്കിൽ((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       /* Set the maximum allowed size of an incoming message, which to
          OpenLDAP means that it will malloc() memory up to this size. If not
          set, there is no limit and we instead risk a malloc() failure. */
       !ber_sockbuf_ctrl(sb, LBER_SB_OPT_SET_MAX_INCOMING, &max)) {
      result = CURLE_FAILED_INIT;
      പോകുക out;
    }
  }

#ifdef USE_SSL
  എങ്കിൽ(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = oldap_ssl_connect(data, OLDAP_SSL);
    പോകുക out;
  }

  എങ്കിൽ(data->set.use_ssl) {
    result = oldap_perform_starttls(data);
    എങ്കിൽ(!result || data->set.use_ssl != CURLUSESSL_TRY)
      പോകുക out;
  }
#endif

  എങ്കിൽ(li->sasl.prefmech != SASL_AUTH_NONE) {
    result = oldap_perform_mechs(data);
    പോകുക out;
  }

  /* Force bind even if anonymous bind is not needed in protocol version 3
     to detect missing version 3 support. */
  result = oldap_perform_bind(data, OLDAP_BIND);

out:
  curlx_free(hosturl);
  തിരികെ result;
}

/* Handle the supported SASL mechanisms query response */
സ്ഥിതം CURLcode oldap_state_mechs_resp(ഘടന Curl_easy *data,
                                       LDAPMessage *സന്ദേശം, പൂർണ്ണസംഖ്യ code)
{
  ഘടന connectdata *conn;
  ഘടന ldapconninfo *li;
  പൂർണ്ണസംഖ്യ rc;
  BerElement *ber = ശൂന്യ;
  CURLcode result = CURLE_OK;
  ഘടന berval bv, *bvals;

  എങ്കിൽ(!data)
    തിരികെ CURLE_FAILED_INIT;

  conn = data->conn;
  li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);

  എങ്കിൽ(!li)
    തിരികെ CURLE_FAILED_INIT;
  തിരഞ്ഞെടുപ്പ്(ldap_msgtype(സന്ദേശം)) {
  അവസ്ഥ LDAP_RES_തിരയൽ_ENTRY:
    /* Got a list of supported SASL mechanisms. */
    എങ്കിൽ(code != LDAP_SUCCESS && code != LDAP_NO_RESULTS_RETURNED)
      തിരികെ CURLE_LOGIN_DENIED;

    rc = ldap_get_dn_ber(li->ld, സന്ദേശം, &ber, &bv);
    എങ്കിൽ(rc < 0)
      തിരികെ oldap_map_error(rc, CURLE_BAD_CONTENT_ENCODING);
    വേണ്ടി(rc = ldap_get_attribute_ber(li->ld, സന്ദേശം, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, സന്ദേശം, ber, &bv, &bvals)) {
      പൂർണ്ണസംഖ്യ i;

      എങ്കിൽ(!bv.bv_val)
        നിർത്തുക;

      എങ്കിൽ(bvals) {
        വേണ്ടി(i = 0; bvals[i].bv_val; i++) {
          size_t llen;
          unsigned short mech =
            Curl_sasl_decode_mech((സ്ഥിരം അക്ഷരം *)bvals[i].bv_val,
                                  bvals[i].bv_len, &llen);
          എങ്കിൽ(bvals[i].bv_len == llen)
            li->sasl.authmechs |= mech;
        }
        ber_memfree(bvals);
      }
    }
    ber_free(ber, 0);
    നിർത്തുക;

  അവസ്ഥ LDAP_RES_തിരയൽ_RESULT:
    തിരഞ്ഞെടുപ്പ്(code) {
    അവസ്ഥ LDAP_SIZELIMIT_EXCEEDED:
      infof(data, "Too many authentication mechanisms");
      FALLTHROUGH();
    അവസ്ഥ LDAP_SUCCESS:
    അവസ്ഥ LDAP_NO_RESULTS_RETURNED:
      എങ്കിൽ(Curl_sasl_can_authenticate(&li->sasl, data))
        result = oldap_perform_sasl(data);
      അല്ലെങ്കിൽ
        result = CURLE_LOGIN_DENIED;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      result = oldap_map_error(code, CURLE_LOGIN_DENIED);
      നിർത്തുക;
    }
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ result;
}

/* Handle a SASL bind response. */
സ്ഥിതം CURLcode oldap_state_sasl_resp(ഘടന Curl_easy *data,
                                      LDAPMessage *സന്ദേശം, പൂർണ്ണസംഖ്യ code)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode result = CURLE_OK;
  saslprogress progress;
  പൂർണ്ണസംഖ്യ rc;

  എങ്കിൽ(!li)
    തിരികെ CURLE_FAILED_INIT;
  li->servercred = ശൂന്യ;
  rc = ldap_parse_sasl_bind_result(li->ld, സന്ദേശം, &li->servercred, 0);
  എങ്കിൽ(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: sasl ldap_parse_result %s", ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LOGIN_DENIED);
  }
  അല്ലെങ്കിൽ {
    result = Curl_sasl_continue(&li->sasl, data, code, &progress);
    എങ്കിൽ(!result) {
      തിരഞ്ഞെടുപ്പ്(progress) {
      അവസ്ഥ SASL_DONE:
        oldap_state(data, li, OLDAP_STOP);   /* Authenticated */
        നിർത്തുക;
      അവസ്ഥ SASL_IDLE:            /* No mechanism left after cancellation */
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി:
        നിർത്തുക;
      }
    }
  }

  എങ്കിൽ(li->servercred)
    ber_bvfree(li->servercred);
  തിരികെ result;
}

/* Handle a simple bind response. */
സ്ഥിതം CURLcode oldap_state_bind_resp(ഘടന Curl_easy *data, LDAPMessage *സന്ദേശം,
                                      പൂർണ്ണസംഖ്യ code)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode result = CURLE_OK;
  ഘടന berval *bv = ശൂന്യ;
  പൂർണ്ണസംഖ്യ rc;

  എങ്കിൽ(!li)
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ(code != LDAP_SUCCESS)
    തിരികെ oldap_map_error(code, CURLE_LDAP_CANNOT_BIND);

  rc = ldap_parse_sasl_bind_result(li->ld, സന്ദേശം, &bv, 0);
  എങ്കിൽ(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: bind ldap_parse_sasl_bind_result %s",
          ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  }
  അല്ലെങ്കിൽ
    oldap_state(data, li, OLDAP_STOP);

  എങ്കിൽ(bv)
    ber_bvfree(bv);
  തിരികെ result;
}

സ്ഥിതം CURLcode oldap_connecting(ഘടന Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;
  ഘടന ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  LDAPMessage *സന്ദേശം = ശൂന്യ;
  ഘടന timeval tv = { 0, 0 };
  പൂർണ്ണസംഖ്യ code = LDAP_SUCCESS;
  പൂർണ്ണസംഖ്യ rc;

  എങ്കിൽ(!li)
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ(li->state != OLDAP_SSL && li->state != OLDAP_TLS) {
    /* Get response to last command. */
    rc = ldap_result(li->ld, li->msgid, LDAP_സന്ദേശം_ONE, &tv, &സന്ദേശം);
    തിരഞ്ഞെടുപ്പ്(rc) {
    അവസ്ഥ 0:                               /* Timed out. */
      തിരികെ CURLE_OK;
    അവസ്ഥ LDAP_RES_തിരയൽ_ENTRY:
    അവസ്ഥ LDAP_RES_തിരയൽ_REFERENCE:
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      li->msgid = 0;                      /* Nothing to abandon upon error. */
      എങ്കിൽ(rc < 0) {
        failf(data, "LDAP local: connecting ldap_result %s",
              ldap_err2string(rc));
        തിരികെ oldap_map_error(rc, CURLE_COULDNT_CONNECT);
      }
      നിർത്തുക;
    }

    /* Get error code from message. */
    rc = ldap_parse_result(li->ld, സന്ദേശം, &code, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, 0);
    എങ്കിൽ(rc)
      code = rc;
    അല്ലെങ്കിൽ {
      /* store the latest code for later retrieval */
      data->info.httpcode = code;
    }

    /* If protocol version 3 is not supported, fallback to version 2. */
    എങ്കിൽ(code == LDAP_PROTOCOL_ERROR && li->state != OLDAP_BINDV2 &&
#ifdef USE_SSL
       (ssl_installed(conn) || data->set.use_ssl <= CURLUSESSL_TRY) &&
#endif
       li->sasl.prefmech == SASL_AUTH_NONE) {
      സ്ഥിതം സ്ഥിരം പൂർണ്ണസംഖ്യ version = LDAP_VERSION2;

      ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);
      ldap_msgfree(സന്ദേശം);
      തിരികെ oldap_perform_bind(data, OLDAP_BINDV2);
    }
  }

  /* Handle response message according to current state. */
  തിരഞ്ഞെടുപ്പ്(li->state) {

#ifdef USE_SSL
  അവസ്ഥ OLDAP_SSL:
    result = oldap_ssl_connect(data, OLDAP_SSL);
    എങ്കിൽ(!result && ssl_installed(conn)) {
      എങ്കിൽ(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      അല്ലെങ്കിൽ
        result = oldap_perform_bind(data, OLDAP_BIND);
    }
    നിർത്തുക;
  അവസ്ഥ OLDAP_STARTTLS:
    എങ്കിൽ(code != LDAP_SUCCESS) {
      എങ്കിൽ(data->set.use_ssl != CURLUSESSL_TRY)
        result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
      അല്ലെങ്കിൽ എങ്കിൽ(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      അല്ലെങ്കിൽ
        result = oldap_perform_bind(data, OLDAP_BIND);
      നിർത്തുക;
    }
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    എങ്കിൽ(result)
      നിർത്തുക;
    FALLTHROUGH();
  അവസ്ഥ OLDAP_TLS:
    result = oldap_ssl_connect(data, OLDAP_TLS);
    എങ്കിൽ(result)
      result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
    അല്ലെങ്കിൽ എങ്കിൽ(ssl_installed(conn)) {
      എങ്കിൽ(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      അല്ലെങ്കിൽ എങ്കിൽ(data->state.creds)
        result = oldap_perform_bind(data, OLDAP_BIND);
      അല്ലെങ്കിൽ {
        /* Version 3 supported: no bind required */
        oldap_state(data, li, OLDAP_STOP);
        result = CURLE_OK;
      }
    }
    നിർത്തുക;
#endif

  അവസ്ഥ OLDAP_MECHS:
    result = oldap_state_mechs_resp(data, സന്ദേശം, code);
    നിർത്തുക;
  അവസ്ഥ OLDAP_SASL:
    result = oldap_state_sasl_resp(data, സന്ദേശം, code);
    നിർത്തുക;
  അവസ്ഥ OLDAP_BIND:
  അവസ്ഥ OLDAP_BINDV2:
    result = oldap_state_bind_resp(data, സന്ദേശം, code);
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    /* internal error */
    result = CURLE_COULDNT_CONNECT;
    നിർത്തുക;
  }

  ldap_msgfree(സന്ദേശം);

  *done = li->state == OLDAP_STOP;
  എങ്കിൽ(*done)
    conn->recv[FIRSTSOCKET] = oldap_recv;

  എങ്കിൽ(result && li->msgid) {
    ldap_abandon_ext(li->ld, li->msgid, ശൂന്യ, ശൂന്യ);
    li->msgid = 0;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode oldap_disconnect(ഘടന Curl_easy *data,
                                 ഘടന connectdata *conn,
                                 bool dead_connection)
{
  ഘടന ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  (ശൂന്യം)dead_connection;
#ifndef USE_SSL
  (ശൂന്യം)data;
#endif

  എങ്കിൽ(li && li->ld) {
#ifdef USE_SSL
    എങ്കിൽ(ssl_installed(conn)) {
      Sockbuf *sb;
      എങ്കിൽ(ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS ||
         ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
        തിരികെ CURLE_FAILED_INIT;
    }
#endif
    ldap_unbind_ext(li->ld, ശൂന്യ, ശൂന്യ);
    li->ld = ശൂന്യ;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode oldap_do(ഘടന Curl_easy *data, bool *done)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ഘടന ldapreqinfo *lr;
  CURLcode result;
  പൂർണ്ണസംഖ്യ rc;
  LDAPURLDesc *lud;
  പൂർണ്ണസംഖ്യ msgid;

  എങ്കിൽ(!li)
    തിരികെ CURLE_FAILED_INIT;

  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

  result = oldap_url_parse(data, &lud);
  എങ്കിൽ(result)
    പോകുക out;

#ifdef USE_SSL
  എങ്കിൽ(ssl_installed(conn)) {
    Sockbuf *sb;
    /* re-install the libcurl SSL handlers into the sockbuf. */
    എങ്കിൽ((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data)) {
      ldap_free_urldesc(lud);
      തിരികെ CURLE_FAILED_INIT;
    }
  }
#endif

  rc = ldap_തിരയൽ_ext(li->ld, lud->lud_dn, lud->lud_scope,
                       lud->lud_filter, lud->lud_attrs, 0,
                       ശൂന്യ, ശൂന്യ, ശൂന്യ, 0, &msgid);
  ldap_free_urldesc(lud);
  എങ്കിൽ(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: ldap_search_ext %s", ldap_err2string(rc));
    result = CURLE_LDAP_തിരയൽ_FAILED;
    പോകുക out;
  }

  lr = curlx_calloc(1, വലുപ്പം(ഘടന ldapreqinfo));
  എങ്കിൽ(!lr ||
     Curl_meta_set(data, CURL_META_LDAP_EASY, lr, oldap_easy_dtor)) {
    ldap_abandon_ext(li->ld, msgid, ശൂന്യ, ശൂന്യ);
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  lr->msgid = msgid;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  *done = TRUE;

out:
  തിരികെ result;
}

സ്ഥിതം CURLcode oldap_done(ഘടന Curl_easy *data, CURLcode res,
                           bool premature)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);

  (ശൂന്യം)res;
  (ശൂന്യം)premature;

  എങ്കിൽ(lr) {
    /* if there was a search in progress, abandon it */
    എങ്കിൽ(lr->msgid) {
      ഘടന ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      എങ്കിൽ(li && li->ld) {
        ldap_abandon_ext(li->ld, lr->msgid, ശൂന്യ, ശൂന്യ);
      }
      lr->msgid = 0;
    }
    Curl_meta_remove(data, CURL_META_LDAP_EASY);
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode client_write(ഘടന Curl_easy *data,
                             സ്ഥിരം അക്ഷരം *prefix, size_t plen,
                             സ്ഥിരം അക്ഷരം *മൂല്യം, size_t len,
                             സ്ഥിരം അക്ഷരം *suffix, size_t slen)
{
  CURLcode result = CURLE_OK;

  എങ്കിൽ(prefix) {
    /* If we have a zero-length value and the prefix ends with a space
       separator, drop the latter. */
    എങ്കിൽ(!len && plen && prefix[plen - 1] == ' ')
      plen--;
    result = Curl_client_write(data, CLIENTWRITE_BODY, prefix, plen);
  }
  എങ്കിൽ(!result && മൂല്യം) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, മൂല്യം, len);
  }
  എങ്കിൽ(!result && suffix) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, suffix, slen);
  }
  തിരികെ result;
}

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
സ്ഥിതം bool oldap_str_needs_base64(സ്ഥിരം അക്ഷരം *s, size_t len)
{
  size_t i;
  എങ്കിൽ(!len)
    തിരികെ FALSE;
  എങ്കിൽ((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    തിരികെ TRUE;
  വേണ്ടി(i = 0; i < len; i++)
    എങ്കിൽ(!ISPRINT(s[i]))
      തിരികെ TRUE;
  തിരികെ FALSE;
}

സ്ഥിതം CURLcode oldap_recv(ഘടന Curl_easy *data, int8_t sockindex, അക്ഷരം *buf,
                           size_t len, size_t *pnread)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ഘടന ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);
  പൂർണ്ണസംഖ്യ rc;
  LDAPMessage *സന്ദേശം = ശൂന്യ;
  BerElement *ber = ശൂന്യ;
  ഘടന timeval tv = { 0, 0 };
  ഘടന berval bv, *bvals;
  CURLcode result = CURLE_AGAIN;
  പൂർണ്ണസംഖ്യ code;
  അക്ഷരം *info = ശൂന്യ;

  (ശൂന്യം)len;
  (ശൂന്യം)buf;
  (ശൂന്യം)sockindex;
  *pnread = 0;
  എങ്കിൽ(!li || !lr)
    തിരികെ CURLE_FAILED_INIT;

  rc = ldap_result(li->ld, lr->msgid, LDAP_സന്ദേശം_ONE, &tv, &സന്ദേശം);
  എങ്കിൽ(rc < 0) {
    failf(data, "LDAP local: search ldap_result %s", ldap_err2string(rc));
    result = CURLE_RECV_ERROR;
  }

  /* error or timed out */
  എങ്കിൽ(!സന്ദേശം)
    തിരികെ result;

  result = CURLE_OK;

  തിരഞ്ഞെടുപ്പ്(ldap_msgtype(സന്ദേശം)) {
  അവസ്ഥ LDAP_RES_തിരയൽ_RESULT:
    lr->msgid = 0;
    rc = ldap_parse_result(li->ld, സന്ദേശം, &code, ശൂന്യ, &info, ശൂന്യ, ശൂന്യ, 0);
    എങ്കിൽ(rc) {
      failf(data, "LDAP local: search ldap_parse_result %s",
            ldap_err2string(rc));
      result = CURLE_LDAP_തിരയൽ_FAILED;
      നിർത്തുക;
    }

    /* store the latest code for later retrieval */
    data->info.httpcode = code;

    തിരഞ്ഞെടുപ്പ്(code) {
    അവസ്ഥ LDAP_SIZELIMIT_EXCEEDED:
      infof(data, "There are more than %d entries", lr->nument);
      FALLTHROUGH();
    അവസ്ഥ LDAP_SUCCESS:
      data->req.size = data->req.bytecount;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      failf(data, "LDAP remote: search failed %s %s", ldap_err2string(code),
            info ? info : "");
      result = CURLE_LDAP_തിരയൽ_FAILED;
      നിർത്തുക;
    }
    എങ്കിൽ(info)
      ldap_memfree(info);
    നിർത്തുക;
  അവസ്ഥ LDAP_RES_തിരയൽ_ENTRY:
    lr->nument++;
    rc = ldap_get_dn_ber(li->ld, സന്ദേശം, &ber, &bv);
    എങ്കിൽ(rc < 0) {
      result = CURLE_RECV_ERROR;
      നിർത്തുക;
    }

    എങ്കിൽ(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
      അക്ഷരം *dn_b64 = ശൂന്യ;
      size_t dn_b64_sz = 0;

      /* A DN with e.g. CR/LF would inject LDIF lines, so base64-encode it. */
      result = curlx_base64_encode((uint8_t *)bv.bv_val, bv.bv_len,
                                   &dn_b64, &dn_b64_sz);
      എങ്കിൽ(!result)
        result = client_write(data, STRCONST("DN:: "), dn_b64, dn_b64_sz,
                              STRCONST("\n"));
      curlx_free(dn_b64);
    }
    അല്ലെങ്കിൽ
      result = client_write(data, STRCONST("DN: "), bv.bv_val, bv.bv_len,
                            STRCONST("\n"));
    എങ്കിൽ(result)
      നിർത്തുക;

    വേണ്ടി(rc = ldap_get_attribute_ber(li->ld, സന്ദേശം, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, സന്ദേശം, ber, &bv, &bvals)) {
      പൂർണ്ണസംഖ്യ i;
      bool ദ്വിമാന;

      എങ്കിൽ(!bv.bv_val)
        നിർത്തുക;

      /* An attribute type has no base64 form in LDIF, so a type carrying
         control bytes (CR/LF etc.) would let a hostile server inject lines.
         A conformant type name never contains them, so reject it. */
      എങ്കിൽ(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
        failf(data, "LDAP local: attribute type contains illegal bytes");
        എങ്കിൽ(bvals)
          ber_memfree(bvals);
        result = CURLE_WEIRD_SERVER_REPLY;
        നിർത്തുക;
      }

      എങ്കിൽ(!bvals) {
        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":\n"));
        എങ്കിൽ(result)
          നിർത്തുക;
        തുടരുക;
      }

      ദ്വിമാന = bv.bv_len > 7 &&
        curl_strnequal(bv.bv_val + bv.bv_len - 7, ";binary", 7);

      വേണ്ടി(i = 0; bvals[i].bv_val; i++) {
        bool binval = FALSE;

        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":"));
        എങ്കിൽ(result)
          നിർത്തുക;

        എങ്കിൽ(!ദ്വിമാന)
          binval = oldap_str_needs_base64(bvals[i].bv_val, bvals[i].bv_len);
        എങ്കിൽ(ദ്വിമാന || binval) {
          അക്ഷരം *val_b64 = ശൂന്യ;
          size_t val_b64_sz = 0;

          /* Binary value, encode to base64. */
          എങ്കിൽ(bvals[i].bv_len)
            result = curlx_base64_encode((uint8_t *)bvals[i].bv_val,
                                         bvals[i].bv_len,
                                         &val_b64, &val_b64_sz);
          എങ്കിൽ(!result)
            result = client_write(data, STRCONST(": "), val_b64, val_b64_sz,
                                  STRCONST("\n"));
          curlx_free(val_b64);
        }
        അല്ലെങ്കിൽ
          result = client_write(data, STRCONST(" "),
                                bvals[i].bv_val, bvals[i].bv_len,
                                STRCONST("\n"));
        എങ്കിൽ(result)
          നിർത്തുക;
      }

      ber_memfree(bvals);
      bvals = ശൂന്യ;
      എങ്കിൽ(!result)
        result = client_write(data, STRCONST("\n"), ശൂന്യ, 0, ശൂന്യ, 0);
      എങ്കിൽ(result)
        നിർത്തുക;
    }

    എങ്കിൽ(!result)
      result = client_write(data, STRCONST("\n"), ശൂന്യ, 0, ശൂന്യ, 0);
    എങ്കിൽ(!result)
      result = CURLE_AGAIN;
    നിർത്തുക;
  }

  ber_free(ber, 0);
  ldap_msgfree(സന്ദേശം);
  തിരികെ result;
}

ശൂന്യം Curl_ldap_version(അക്ഷരം *buf, size_t bufsz)
{
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  എങ്കിൽ(ldap_get_option(ശൂന്യ, LDAP_OPT_API_INFO, &api) == LDAP_OPT_SUCCESS) {
    unsigned പൂർണ്ണസംഖ്യ patch = (unsigned പൂർണ്ണസംഖ്യ)(api.ldapai_vendor_version % 100);
    unsigned പൂർണ്ണസംഖ്യ major = (unsigned പൂർണ്ണസംഖ്യ)(api.ldapai_vendor_version / 10000);
    unsigned പൂർണ്ണസംഖ്യ minor =
      (((unsigned പൂർണ്ണസംഖ്യ)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u",
                   api.ldapai_vendor_name, major, minor, patch);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((ശൂന്യം **)api.ldapai_extensions);
  }
  അല്ലെങ്കിൽ
    curl_msnprintf(buf, bufsz, "OpenLDAP");
}

/*
 * LDAP protocol handler.
 */
സ്ഥിരം ഘടന Curl_protocol Curl_protocol_ldap = {
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
