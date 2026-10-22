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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP)

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

#ਅਨ੍ਤਰ੍ਭਾਵ <ldap.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_ldap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/base64.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sasl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  OLDAP_STOP,           /* Do nothing state, stops the state machine */
  OLDAP_SSL,            /* Performing SSL handshake. */
  OLDAP_STARTTLS,       /* STARTTLS request sent. */
  OLDAP_TLS,            /* Performing TLS handshake. */
  OLDAP_MECHS,          /* Get SASL authentication mechanisms. */
  OLDAP_SASL,           /* SASL binding reply. */
  OLDAP_BIND,           /* Simple bind reply. */
  OLDAP_BINDV2,         /* Simple bind reply in protocol version 2. */
  OLDAP_ਆਖਰੀ            /* Never used */
} ldapstate;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _LDAP_PVT_H
ਬਾਹਰੀ ਪੂਰਨ ldap_pvt_url_scheme2proto(ਅਚਲ ਅੱਖਰ *);
ਬਾਹਰੀ ਪੂਰਨ ldap_init_fd(ber_socket_t fd, ਪੂਰਨ proto, ਅਚਲ ਅੱਖਰ *url,
                        LDAP **ld);
#ਜੇ_ਅੰਤ

ਸਥਿਰ Curl_recv oldap_recv;

ਬਣਤਰ ldapconninfo {
  ਬਣਤਰ SASL sasl;          /* SASL-related parameters */
  LDAP *ld;                  /* Openldap connection handle. */
  Curl_recv *recv;           /* For stacking SSL handler */
  Curl_send *send;
  ਬਣਤਰ berval *servercred; /* SASL data from server. */
  ldapstate state;           /* Current machine state. */
  curl_socket_t dupfd;       /* duplicated socket */
  ਪੂਰਨ proto;                 /* LDAP_PROTO_TCP/LDAP_PROTO_UDP/LDAP_PROTO_IPC */
  ਪੂਰਨ msgid;                 /* Current message id. */
};

ਬਣਤਰ ldapreqinfo {
  ਪੂਰਨ msgid;
  ਪੂਰਨ nument;
};

/* meta key for storing ldapconninfo at easy handle */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_LDAP_EASY   "meta:proto:ldap:easy"
/* meta key for storing ldapconninfo at connection */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_LDAP_CONN   "meta:proto:ldap:conn"

/*
 * oldap_state()
 *
 * This is the ONLY way to change LDAP state!
 */
ਸਥਿਰ ਖਾਲੀ oldap_state(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ ldapconninfo *li,
                        ldapstate newstate)
{
#ਜੇ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ ਨਾਂ[] = {
    "STOP",
    "SSL",
    "STARTTLS",
    "TLS",
    "MECHS",
    "SASL",
    "BIND",
    "BINDV2",
    /*ਆਖਰੀ*/
  };

  ਜੇ(li->state != newstate)
    infof(ਅੰਕੜੇ, "LDAP %p state change from %s to %s",
          (ਖਾਲੀ *)li, ਨਾਂ[li->state], ਨਾਂ[newstate]);
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
#ਜੇ_ਅੰਤ
  li->state = newstate;
}

/* Map some particular LDAP error codes to CURLcode values. */
ਸਥਿਰ CURLcode oldap_map_error(ਪੂਰਨ rc, CURLcode ਨਤੀਜਾ)
{
  ਚੋਣ(rc) {
  ਹਾਲਤ LDAP_NO_ਸਮ੍ਰਿਤੀ:
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਹਾਲਤ LDAP_INVALID_CREDENTIALS:
    ਵਾਪਸ CURLE_LOGIN_DENIED;
  ਹਾਲਤ LDAP_PROTOCOL_ERROR:
    ਵਾਪਸ CURLE_UNSUPPORTED_PROTOCOL;
  ਹਾਲਤ LDAP_INSUFFICIENT_ACCESS:
    ਵਾਪਸ CURLE_REMOTE_ACCESS_DENIED;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode oldap_url_parse(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, LDAPURLDesc **ludp)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ rc;
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ url_errs[] = {
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

  *ludp = ਨਿਰਾ;
  /* `ldap_url_parse() seems to be terrible with urls
   * that have user/pass/options in it. So when we have options or
   * creds from the url, fail without calling the function.
   * Yes, this is super-weird code and I do not like it. */
  ਜੇ((ਅੰਕੜੇ->state.creds && (ਅੰਕੜੇ->state.creds->ਸਰੋਤ == CREDS_URL)) ||
     ਅੰਕੜੇ->state.ਉੱਪਰ.options)
    rc = LDAP_URL_ERR_BADURL;
  ਨਹੀਂਤਾਂ
    rc = ldap_url_parse(Curl_bufref_ptr(&ਅੰਕੜੇ->state.url), ludp);

  ਜੇ(rc != LDAP_URL_SUCCESS) {
    ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ = "url parsing problem";

    ਨਤੀਜਾ = rc == LDAP_URL_ERR_MEM ? CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ :
      CURLE_URL_MALFORMAT;
    rc -= LDAP_URL_SUCCESS;
    ਜੇ((size_t)rc < CURL_ARRAYSIZE(url_errs))
      ਸੁਨੇਹਾ = url_errs[rc];
    failf(ਅੰਕੜੇ, "LDAP local: %s", ਸੁਨੇਹਾ);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Parse the login options. */
ਸਥਿਰ CURLcode oldap_parse_login_options(ਬਣਤਰ connectdata *conn)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ਅਚਲ ਅੱਖਰ *ptr = conn->options;

  DEBUGASSERT(li);
  ਜੇ(!li)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜਦੋਂ(!ਨਤੀਜਾ && ptr && *ptr) {
    ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ = ptr;
    ਅਚਲ ਅੱਖਰ *ਮੁੱਲ;

    ਜਦੋਂ(*ptr && *ptr != '=')
      ptr++;

    ਮੁੱਲ = ptr + 1;

    ਜਦੋਂ(*ptr && *ptr != ';')
      ptr++;

    ਜੇ(checkprefix("AUTH=", ਕੁੰਜੀ))
      ਨਤੀਜਾ = Curl_sasl_parse_url_auth_option(&li->sasl, ਮੁੱਲ, ptr - ਮੁੱਲ);
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = CURLE_SETOPT_OPTION_SYNTAX;

    ਜੇ(*ptr == ';')
      ptr++;
  }

  ਵਾਪਸ ਨਤੀਜਾ == CURLE_URL_MALFORMAT ? CURLE_SETOPT_OPTION_SYNTAX : ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode oldap_ਤਿਆਰੀ_connection(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ connectdata *conn)
{
  CURLcode ਨਤੀਜਾ;
  LDAPURLDesc *lud;
  (ਖਾਲੀ)conn;

  /* Early URL syntax check. */
  ਨਤੀਜਾ = oldap_url_parse(ਅੰਕੜੇ, &lud);
  ldap_free_urldesc(lud);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Get the SASL authentication challenge from the server credential buffer.
 */
ਸਥਿਰ CURLcode oldap_get_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ bufref *ਬਾਹਰਲਾ)
{
  ਬਣਤਰ ldapconninfo *li =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_LDAP_CONN);
  ਬਣਤਰ berval *servercred = li ? li->servercred : ਨਿਰਾ;
  DEBUGASSERT(li);
  ਜੇ(!li)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(!servercred || !servercred->bv_val)
    ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
  Curl_bufref_ਸੈੱਟ(ਬਾਹਰਲਾ, servercred->bv_val, servercred->bv_len, ਨਿਰਾ);
  ਵਾਪਸ CURLE_OK;
}

/*
 * Sends an initial SASL bind request to the server.
 */
ਸਥਿਰ CURLcode oldap_perform_auth(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *mech,
                                   ਅਚਲ ਬਣਤਰ bufref *initresp)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ਬਣਤਰ berval cred;
  ਬਣਤਰ berval *pcred = &cred;
  ਪੂਰਨ rc;

  DEBUGASSERT(li);
  ਜੇ(!li)
    ਵਾਪਸ CURLE_FAILED_INIT;
  cred.bv_val = (ਅੱਖਰ *)CURL_UNCONST(Curl_bufref_ptr(initresp));
  cred.bv_len = Curl_bufref_len(initresp);
  ਜੇ(!cred.bv_val)
    pcred = ਨਿਰਾ;
  rc = ldap_sasl_bind(li->ld, ਨਿਰਾ, mech, pcred, ਨਿਰਾ, ਨਿਰਾ, &li->msgid);
  ਜੇ(rc != LDAP_SUCCESS)
    ਵਾਪਸ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ਵਾਪਸ CURLE_OK;
}

/*
 * Sends SASL continuation.
 */
ਸਥਿਰ CURLcode oldap_continue_auth(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *mech,
                                    ਅਚਲ ਬਣਤਰ bufref *resp)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ਬਣਤਰ berval cred;
  ਬਣਤਰ berval *pcred = &cred;
  ਪੂਰਨ rc;

  ਜੇ(!li)
    ਵਾਪਸ CURLE_FAILED_INIT;
  cred.bv_val = (ਅੱਖਰ *)CURL_UNCONST(Curl_bufref_ptr(resp));
  cred.bv_len = Curl_bufref_len(resp);
  ਜੇ(!cred.bv_val)
    pcred = ਨਿਰਾ;
  rc = ldap_sasl_bind(li->ld, ਨਿਰਾ, mech, pcred, ਨਿਰਾ, ਨਿਰਾ, &li->msgid);
  ਜੇ(rc != LDAP_SUCCESS)
    ਵਾਪਸ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ਵਾਪਸ CURLE_OK;
}

/*
 * Sends SASL bind cancellation.
 */
ਸਥਿਰ CURLcode oldap_cancel_auth(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *mech)
{
  ਬਣਤਰ ldapconninfo *li =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_LDAP_CONN);
  ਪੂਰਨ rc;

  (ਖਾਲੀ)mech;
  ਜੇ(!li)
    ਵਾਪਸ CURLE_FAILED_INIT;
  rc = ldap_sasl_bind(li->ld, ਨਿਰਾ, LDAP_SASL_NULL, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ,
                      &li->msgid);
  ਜੇ(rc != LDAP_SUCCESS)
    ਵਾਪਸ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ਵਾਪਸ CURLE_OK;
}

/* Starts LDAP simple bind. */
ਸਥਿਰ CURLcode oldap_perform_bind(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ldapstate newstate)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ਅਚਲ ਅੱਖਰ *binddn = ਨਿਰਾ;
  ਬਣਤਰ berval passwd;
  ਪੂਰਨ rc;

  ਜੇ(!li)
    ਵਾਪਸ CURLE_FAILED_INIT;
  passwd.bv_val = ਨਿਰਾ;
  passwd.bv_len = 0;

  ਜੇ(conn->creds) {
    binddn = Curl_creds_user(conn->creds);
    passwd.bv_val = CURL_UNCONST(Curl_creds_passwd(conn->creds));
    passwd.bv_len = ਲੜੀ_ਲੰਬਾਈ(passwd.bv_val);
  }

  rc = ldap_sasl_bind(li->ld, binddn, LDAP_SASL_SIMPLE, &passwd,
                      ਨਿਰਾ, ਨਿਰਾ, &li->msgid);
  ਜੇ(rc != LDAP_SUCCESS)
    ਵਾਪਸ oldap_map_error(rc,
                           ਅੰਕੜੇ->state.creds ?
                           CURLE_LOGIN_DENIED : CURLE_LDAP_CANNOT_BIND);
  oldap_state(ਅੰਕੜੇ, li, newstate);
  ਵਾਪਸ CURLE_OK;
}

/* Query the supported SASL authentication mechanisms. */
ਸਥਿਰ CURLcode oldap_perform_mechs(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ldapconninfo *li =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_LDAP_CONN);
  ਪੂਰਨ rc;
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ supportedSASLMechanisms[] = {
    "supportedSASLMechanisms",
    ਨਿਰਾ
  };

  ਜੇ(!li)
    ਵਾਪਸ CURLE_FAILED_INIT;
  /* Casting away the const for the 3rd parameter that the LDAP API expects as
     a non-const char ** is potentially unsafe but we believe the lack of
     const in the API was an oversight and that no LDAP implementation
     actually modifies the input. */
  rc = ldap_ਖੋਜ_ext(li->ld, "", LDAP_SCOPE_ਅਧਾਰ, "(objectclass=*)",
                       (ਅੱਖਰ **)CURL_UNCONST(supportedSASLMechanisms), 0,
                       ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, 0, &li->msgid);
  ਜੇ(rc != LDAP_SUCCESS)
    ਵਾਪਸ oldap_map_error(rc, CURLE_LOGIN_DENIED);
  oldap_state(ਅੰਕੜੇ, li, OLDAP_MECHS);
  ਵਾਪਸ CURLE_OK;
}

/* Starts SASL bind. */
ਸਥਿਰ CURLcode oldap_perform_sasl(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ldapconninfo *li =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_LDAP_CONN);
  saslprogress ਤਰੱਕੀ = SASL_IDLE;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!li)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਨਤੀਜਾ = Curl_sasl_ਸ਼ੁਰੂ(&li->sasl, ਅੰਕੜੇ, TRUE, &ਤਰੱਕੀ);

  oldap_state(ਅੰਕੜੇ, li, OLDAP_SASL);
  ਜੇ(!ਨਤੀਜਾ && ਤਰੱਕੀ != SASL_INPROGRESS)
    ਨਤੀਜਾ = Curl_sasl_is_blocked(&li->sasl, ਅੰਕੜੇ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
ਸਥਿਰ ਪੂਰਨ ldapsb_tls_ਤਿਆਰੀ(Sockbuf_IO_Desc *sbiod, ਖਾਲੀ *arg)
{
  sbiod->sbiod_pvt = arg;
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ ldapsb_tls_remove(Sockbuf_IO_Desc *sbiod)
{
  sbiod->sbiod_pvt = ਨਿਰਾ;
  ਵਾਪਸ 0;
}

/* if there is a duplicated socket to close, close it */
ਸਥਿਰ ਪੂਰਨ ldapsb_tls_close(Sockbuf_IO_Desc *sbiod)
{
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = sbiod->sbiod_pvt;
  ਜੇ(ਅੰਕੜੇ) {
    ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
    ਜੇ(conn) {
      ਬਣਤਰ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      ਜੇ(li && (li->dupfd != CURL_SOCKET_BAD)) {
        sclose(li->dupfd);
        li->dupfd = CURL_SOCKET_BAD;
      }
    }
  }

  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ ldapsb_tls_ctrl(Sockbuf_IO_Desc *sbiod, ਪੂਰਨ opt, ਖਾਲੀ *arg)
{
  (ਖਾਲੀ)arg;
  ਜੇ(opt == LBER_SB_OPT_ਅੰਕੜੇ_READY) {
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = sbiod->sbiod_pvt;
    ਵਾਪਸ Curl_conn_ਅੰਕੜੇ_pending(ਅੰਕੜੇ, FIRSTSOCKET);
  }
  ਵਾਪਸ 0;
}

ਸਥਿਰ ber_slen_t ldapsb_tls_read(Sockbuf_IO_Desc *sbiod, ਖਾਲੀ *buf,
                                  ber_len_t len)
{
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  ਜੇ(ਅੰਕੜੇ) {
    ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
    ਜੇ(conn) {
      ਬਣਤਰ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode ਨਤੀਜਾ = CURLE_RECV_ERROR;
      size_t nread;

      ਜੇ(!li) {
        ਸੈੱਟ_SOCKERRNO(SOCKEINVAL);
        ਵਾਪਸ -1;
      }
      ਨਤੀਜਾ = (li->recv)(ਅੰਕੜੇ, FIRSTSOCKET, buf, len, &nread);
      ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
        ਸੈੱਟ_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = ਨਤੀਜਾ ? -1 : (ber_slen_t)nread;
    }
  }
  ਵਾਪਸ ret;
}

ਸਥਿਰ ber_slen_t ldapsb_tls_write(Sockbuf_IO_Desc *sbiod, ਖਾਲੀ *buf,
                                   ber_len_t len)
{
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  ਜੇ(ਅੰਕੜੇ) {
    ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
    ਜੇ(conn) {
      ਬਣਤਰ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode ਨਤੀਜਾ = CURLE_SEND_ERROR;
      size_t nwritten;

      ਜੇ(!li) {
        ਸੈੱਟ_SOCKERRNO(SOCKEINVAL);
        ਵਾਪਸ -1;
      }
      ਨਤੀਜਾ = (li->send)(ਅੰਕੜੇ, FIRSTSOCKET, buf, len, FALSE, &nwritten);
      ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
        ਸੈੱਟ_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = ਨਤੀਜਾ ? -1 : (ber_slen_t)nwritten;
    }
  }
  ਵਾਪਸ ret;
}

ਸਥਿਰ Sockbuf_IO ldapsb_tls = {
  ldapsb_tls_ਤਿਆਰੀ,
  ldapsb_tls_remove,
  ldapsb_tls_ctrl,
  ldapsb_tls_read,
  ldapsb_tls_write,
  ldapsb_tls_close
};

ਸਥਿਰ bool ssl_installed(ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ਵਾਪਸ li && li->recv;
}

ਸਥਿਰ CURLcode oldap_ssl_connect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ldapstate newstate)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  bool ssldone = FALSE;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!li)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਨਤੀਜਾ = Curl_conn_connect(ਅੰਕੜੇ, FIRSTSOCKET, FALSE, &ssldone);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  oldap_state(ਅੰਕੜੇ, li, newstate);

  ਜੇ(ssldone) {
    Sockbuf *sb;

    /* Install the libcurl SSL handlers into the sockbuf. */
    ਜੇ((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_ਜੋੜੋ_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, ਅੰਕੜੇ))
      ਵਾਪਸ CURLE_FAILED_INIT;
    li->recv = conn->recv[FIRSTSOCKET];
    li->send = conn->send[FIRSTSOCKET];
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Send the STARTTLS request */
ਸਥਿਰ CURLcode oldap_perform_starttls(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ldapconninfo *li =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_LDAP_CONN);
  ਪੂਰਨ rc;

  ਜੇ(!li)
    ਵਾਪਸ CURLE_FAILED_INIT;
  rc = ldap_ਸ਼ੁਰੂ_tls(li->ld, ਨਿਰਾ, ਨਿਰਾ, &li->msgid);
  ਜੇ(rc != LDAP_SUCCESS)
    ਵਾਪਸ oldap_map_error(rc, CURLE_USE_SSL_FAILED);
  oldap_state(ਅੰਕੜੇ, li, OLDAP_STARTTLS);
  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ oldap_easy_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ ldapreqinfo *lr = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  curlx_free(lr);
}

ਸਥਿਰ ਖਾਲੀ oldap_conn_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ ldapconninfo *li = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  ਜੇ(li->ld) {
    ldap_unbind_ext(li->ld, ਨਿਰਾ, ਨਿਰਾ);
    li->ld = ਨਿਰਾ;
  }
  curlx_free(li);
}

/* SASL parameters for the ldap protocol */
ਸਥਿਰ ਅਚਲ ਬਣਤਰ SASLproto saslldap = {
  "ldap",                     /* The service name */
  oldap_perform_auth,         /* Send authentication command */
  oldap_continue_auth,        /* Send authentication continuation */
  oldap_cancel_auth,          /* Send authentication cancellation */
  oldap_get_ਸੁਨੇਹਾ,          /* Get SASL response message */
  0,                          /* Maximum initial response length (no max) */
  LDAP_SASL_BIND_ਵਿੱਚ_ਤਰੱਕੀ, /* Code received when continuation is expected */
  LDAP_SUCCESS,               /* Code to receive upon authentication success */
  SASL_AUTH_NONE,             /* Default mechanisms */
  0                           /* Configuration flags */
};

ਸਥਿਰ CURLcode oldap_connect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ldapconninfo *li;
  ਸਥਿਰ ਅਚਲ ਪੂਰਨ ਸੰਸਕਰਣ_2 = LDAP_VERSION3;
  ਅੱਖਰ *hosturl = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ rc;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_OPENLDAP_DEBUG
  ਸਥਿਰ ਪੂਰਨ do_trace = -1;
#ਜੇ_ਅੰਤ

  (ਖਾਲੀ)ਪੂਰਾ;

  li = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ ldapconninfo));
  ਜੇ(!li) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  li->dupfd = CURL_SOCKET_BAD;

  ਨਤੀਜਾ = Curl_conn_meta_ਸੈੱਟ(conn, CURL_META_LDAP_CONN, li, oldap_conn_dtor);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  li->proto = ldap_pvt_url_scheme2proto(ਅੰਕੜੇ->state.origin->scheme->name);

  /* Initialize the SASL storage */
  Curl_sasl_init(&li->sasl, ਅੰਕੜੇ, &saslldap);

  ਨਤੀਜਾ = oldap_parse_login_options(conn);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  hosturl = curl_maprintf("%s://%s:%u",
                          conn->scheme->name,
                          conn->origin->ipv6 ?
                          conn->origin->user_hostname :
                          conn->origin->hostname,
                          conn->origin->ਪੋਰਟ);
  ਜੇ(!hosturl) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
    SOCKET dupfd = INVALID_SOCKET;
    WSAPROTOCOL_INFO pi;
    ਜੇ(WSADuplicateSocket(conn->sock[FIRSTSOCKET], GetCurrentProcessId(),
                          &pi))
      ; /* error */
    ਨਹੀਂਤਾਂ
      dupfd = CURL_SOCKET(pi.i_ਪਤਾ_Family, pi.iSocketType, pi.iProtocol);
#ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ F_DUPFD_CLOEXEC
    ਪੂਰਨ dupfd = fcntl(conn->sock[FIRSTSOCKET], F_DUPFD_CLOEXEC, 0);
#ਨਹੀਂਤਾਂ
    ਪੂਰਨ dupfd = dup(conn->sock[FIRSTSOCKET]);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FCNTL
    ਜੇ((dupfd != CURL_SOCKET_BAD) &&
       (fcntl(dupfd, F_SETFD, FD_CLOEXEC) < 0)) {
      sclose(dupfd);
      dupfd = CURL_SOCKET_BAD;
    }
#ਜੇ_ਅੰਤ /* HAVE_FCNTL */
#ਜੇ_ਅੰਤ /* F_DUPFD_CLOEXEC */
#ਜੇ_ਅੰਤ /* _WIN32 */

    ਜੇ(dupfd == CURL_SOCKET_BAD) {
      ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    rc = ldap_init_fd((ber_socket_t)dupfd, li->proto, hosturl, &li->ld);
    ਜੇ(rc)
      sclose(dupfd);
    ਨਹੀਂਤਾਂ
      /* store the socket, as when this is used by TLS we close this ourselves
         and at this point the code does not yet know if this will use TLS or
         not */
      li->dupfd = dupfd;
  }
  ਜੇ(rc) {
    failf(ਅੰਕੜੇ, "LDAP local: Cannot connect to %s, %s",
          hosturl, ldap_err2string(rc));
    ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_OPENLDAP_DEBUG
  ਜੇ(do_trace < 0) {
    ਅਚਲ ਅੱਖਰ *env = getenv("CURL_OPENLDAP_TRACE");
    curl_off_t e = 0;
    ਜੇ(!curlx_str_ਸੰਖਿਆ(&env, &e, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ))
      do_trace = e > 0;
  }
  ਜੇ(do_trace)
    ldap_ਸੈੱਟ_option(li->ld, LDAP_OPT_DEBUG_LEVEL, &do_trace);
#ਜੇ_ਅੰਤ

  /* Try version 3 first. */
  ldap_ਸੈੱਟ_option(li->ld, LDAP_OPT_PROTOCOL_ਸੰਸਕਰਣ, &ਸੰਸਕਰਣ_2);

  /* Do not chase referrals. */
  ldap_ਸੈੱਟ_option(li->ld, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  {
    ber_len_t max = 256 * 1024;
    Sockbuf *sb;
    ਜੇ((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       /* Set the maximum allowed size of an incoming message, which to
          OpenLDAP means that it will malloc() memory up to this size. If not
          set, there is no limit and we instead risk a malloc() failure. */
       !ber_sockbuf_ctrl(sb, LBER_SB_OPT_ਸੈੱਟ_MAX_INCOMING, &max)) {
      ਨਤੀਜਾ = CURLE_FAILED_INIT;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  ਜੇ(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    ਨਤੀਜਾ = oldap_ssl_connect(ਅੰਕੜੇ, OLDAP_SSL);
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.use_ssl) {
    ਨਤੀਜਾ = oldap_perform_starttls(ਅੰਕੜੇ);
    ਜੇ(!ਨਤੀਜਾ || ਅੰਕੜੇ->ਸੈੱਟ.use_ssl != CURLUSESSL_TRY)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
#ਜੇ_ਅੰਤ

  ਜੇ(li->sasl.prefmech != SASL_AUTH_NONE) {
    ਨਤੀਜਾ = oldap_perform_mechs(ਅੰਕੜੇ);
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* Force bind even if anonymous bind is not needed in protocol version 3
     to detect missing version 3 support. */
  ਨਤੀਜਾ = oldap_perform_bind(ਅੰਕੜੇ, OLDAP_BIND);

ਬਾਹਰਲਾ:
  curlx_free(hosturl);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Handle the supported SASL mechanisms query response */
ਸਥਿਰ CURLcode oldap_state_mechs_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       LDAPMessage *ਸੁਨੇਹਾ, ਪੂਰਨ ਕੋਡ)
{
  ਬਣਤਰ connectdata *conn;
  ਬਣਤਰ ldapconninfo *li;
  ਪੂਰਨ rc;
  BerElement *ber = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ berval bv, *bvals;

  ਜੇ(!ਅੰਕੜੇ)
    ਵਾਪਸ CURLE_FAILED_INIT;

  conn = ਅੰਕੜੇ->conn;
  li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);

  ਜੇ(!li)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਚੋਣ(ldap_msgtype(ਸੁਨੇਹਾ)) {
  ਹਾਲਤ LDAP_RES_ਖੋਜ_ਇੰਦਰਾਜ:
    /* Got a list of supported SASL mechanisms. */
    ਜੇ(ਕੋਡ != LDAP_SUCCESS && ਕੋਡ != LDAP_NO_RESULTS_RETURNED)
      ਵਾਪਸ CURLE_LOGIN_DENIED;

    rc = ldap_get_dn_ber(li->ld, ਸੁਨੇਹਾ, &ber, &bv);
    ਜੇ(rc < 0)
      ਵਾਪਸ oldap_map_error(rc, CURLE_BAD_CONTENT_ENCODING);
    ਲਈ(rc = ldap_get_attribute_ber(li->ld, ਸੁਨੇਹਾ, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, ਸੁਨੇਹਾ, ber, &bv, &bvals)) {
      ਪੂਰਨ i;

      ਜੇ(!bv.bv_val)
        ਰੋਕੋ;

      ਜੇ(bvals) {
        ਲਈ(i = 0; bvals[i].bv_val; i++) {
          size_t llen;
          ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ mech =
            Curl_sasl_decode_mech((ਅਚਲ ਅੱਖਰ *)bvals[i].bv_val,
                                  bvals[i].bv_len, &llen);
          ਜੇ(bvals[i].bv_len == llen)
            li->sasl.authmechs |= mech;
        }
        ber_memfree(bvals);
      }
    }
    ber_free(ber, 0);
    ਰੋਕੋ;

  ਹਾਲਤ LDAP_RES_ਖੋਜ_ਨਤੀਜਾ:
    ਚੋਣ(ਕੋਡ) {
    ਹਾਲਤ LDAP_SIZELIMIT_EXCEEDED:
      infof(ਅੰਕੜੇ, "Too many authentication mechanisms");
      FALLTHROUGH();
    ਹਾਲਤ LDAP_SUCCESS:
    ਹਾਲਤ LDAP_NO_RESULTS_RETURNED:
      ਜੇ(Curl_sasl_can_authenticate(&li->sasl, ਅੰਕੜੇ))
        ਨਤੀਜਾ = oldap_perform_sasl(ਅੰਕੜੇ);
      ਨਹੀਂਤਾਂ
        ਨਤੀਜਾ = CURLE_LOGIN_DENIED;
      ਰੋਕੋ;
    ਮੂਲ:
      ਨਤੀਜਾ = oldap_map_error(ਕੋਡ, CURLE_LOGIN_DENIED);
      ਰੋਕੋ;
    }
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Handle a SASL bind response. */
ਸਥਿਰ CURLcode oldap_state_sasl_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      LDAPMessage *ਸੁਨੇਹਾ, ਪੂਰਨ ਕੋਡ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  saslprogress ਤਰੱਕੀ;
  ਪੂਰਨ rc;

  ਜੇ(!li)
    ਵਾਪਸ CURLE_FAILED_INIT;
  li->servercred = ਨਿਰਾ;
  rc = ldap_parse_sasl_bind_ਨਤੀਜਾ(li->ld, ਸੁਨੇਹਾ, &li->servercred, 0);
  ਜੇ(rc != LDAP_SUCCESS) {
    failf(ਅੰਕੜੇ, "LDAP local: sasl ldap_parse_result %s", ldap_err2string(rc));
    ਨਤੀਜਾ = oldap_map_error(rc, CURLE_LOGIN_DENIED);
  }
  ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ = Curl_sasl_continue(&li->sasl, ਅੰਕੜੇ, ਕੋਡ, &ਤਰੱਕੀ);
    ਜੇ(!ਨਤੀਜਾ) {
      ਚੋਣ(ਤਰੱਕੀ) {
      ਹਾਲਤ SASL_ਪੂਰਾ:
        oldap_state(ਅੰਕੜੇ, li, OLDAP_STOP);   /* Authenticated */
        ਰੋਕੋ;
      ਹਾਲਤ SASL_IDLE:            /* No mechanism left after cancellation */
        failf(ਅੰਕੜੇ, "Authentication cancelled");
        ਨਤੀਜਾ = CURLE_LOGIN_DENIED;
        ਰੋਕੋ;
      ਮੂਲ:
        ਰੋਕੋ;
      }
    }
  }

  ਜੇ(li->servercred)
    ber_bvfree(li->servercred);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Handle a simple bind response. */
ਸਥਿਰ CURLcode oldap_state_bind_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, LDAPMessage *ਸੁਨੇਹਾ,
                                      ਪੂਰਨ ਕੋਡ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ berval *bv = ਨਿਰਾ;
  ਪੂਰਨ rc;

  ਜੇ(!li)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(ਕੋਡ != LDAP_SUCCESS)
    ਵਾਪਸ oldap_map_error(ਕੋਡ, CURLE_LDAP_CANNOT_BIND);

  rc = ldap_parse_sasl_bind_ਨਤੀਜਾ(li->ld, ਸੁਨੇਹਾ, &bv, 0);
  ਜੇ(rc != LDAP_SUCCESS) {
    failf(ਅੰਕੜੇ, "LDAP local: bind ldap_parse_sasl_bind_result %s",
          ldap_err2string(rc));
    ਨਤੀਜਾ = oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  }
  ਨਹੀਂਤਾਂ
    oldap_state(ਅੰਕੜੇ, li, OLDAP_STOP);

  ਜੇ(bv)
    ber_bvfree(bv);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode oldap_connecting(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  LDAPMessage *ਸੁਨੇਹਾ = ਨਿਰਾ;
  ਬਣਤਰ timeval tv = { 0, 0 };
  ਪੂਰਨ ਕੋਡ = LDAP_SUCCESS;
  ਪੂਰਨ rc;

  ਜੇ(!li)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(li->state != OLDAP_SSL && li->state != OLDAP_TLS) {
    /* Get response to last command. */
    rc = ldap_ਨਤੀਜਾ(li->ld, li->msgid, LDAP_ਸੁਨੇਹਾ_ONE, &tv, &ਸੁਨੇਹਾ);
    ਚੋਣ(rc) {
    ਹਾਲਤ 0:                               /* Timed out. */
      ਵਾਪਸ CURLE_OK;
    ਹਾਲਤ LDAP_RES_ਖੋਜ_ਇੰਦਰਾਜ:
    ਹਾਲਤ LDAP_RES_ਖੋਜ_REFERENCE:
      ਰੋਕੋ;
    ਮੂਲ:
      li->msgid = 0;                      /* Nothing to abandon upon error. */
      ਜੇ(rc < 0) {
        failf(ਅੰਕੜੇ, "LDAP local: connecting ldap_result %s",
              ldap_err2string(rc));
        ਵਾਪਸ oldap_map_error(rc, CURLE_COULDNT_CONNECT);
      }
      ਰੋਕੋ;
    }

    /* Get error code from message. */
    rc = ldap_parse_ਨਤੀਜਾ(li->ld, ਸੁਨੇਹਾ, &ਕੋਡ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, 0);
    ਜੇ(rc)
      ਕੋਡ = rc;
    ਨਹੀਂਤਾਂ {
      /* store the latest code for later retrieval */
      ਅੰਕੜੇ->info.httpcode = ਕੋਡ;
    }

    /* If protocol version 3 is not supported, fallback to version 2. */
    ਜੇ(ਕੋਡ == LDAP_PROTOCOL_ERROR && li->state != OLDAP_BINDV2 &&
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
       (ssl_installed(conn) || ਅੰਕੜੇ->ਸੈੱਟ.use_ssl <= CURLUSESSL_TRY) &&
#ਜੇ_ਅੰਤ
       li->sasl.prefmech == SASL_AUTH_NONE) {
      ਸਥਿਰ ਅਚਲ ਪੂਰਨ ਸੰਸਕਰਣ_2 = LDAP_VERSION2;

      ldap_ਸੈੱਟ_option(li->ld, LDAP_OPT_PROTOCOL_ਸੰਸਕਰਣ, &ਸੰਸਕਰਣ_2);
      ldap_msgfree(ਸੁਨੇਹਾ);
      ਵਾਪਸ oldap_perform_bind(ਅੰਕੜੇ, OLDAP_BINDV2);
    }
  }

  /* Handle response message according to current state. */
  ਚੋਣ(li->state) {

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  ਹਾਲਤ OLDAP_SSL:
    ਨਤੀਜਾ = oldap_ssl_connect(ਅੰਕੜੇ, OLDAP_SSL);
    ਜੇ(!ਨਤੀਜਾ && ssl_installed(conn)) {
      ਜੇ(li->sasl.prefmech != SASL_AUTH_NONE)
        ਨਤੀਜਾ = oldap_perform_mechs(ਅੰਕੜੇ);
      ਨਹੀਂਤਾਂ
        ਨਤੀਜਾ = oldap_perform_bind(ਅੰਕੜੇ, OLDAP_BIND);
    }
    ਰੋਕੋ;
  ਹਾਲਤ OLDAP_STARTTLS:
    ਜੇ(ਕੋਡ != LDAP_SUCCESS) {
      ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.use_ssl != CURLUSESSL_TRY)
        ਨਤੀਜਾ = oldap_map_error(ਕੋਡ, CURLE_USE_SSL_FAILED);
      ਨਹੀਂਤਾਂ ਜੇ(li->sasl.prefmech != SASL_AUTH_NONE)
        ਨਤੀਜਾ = oldap_perform_mechs(ਅੰਕੜੇ);
      ਨਹੀਂਤਾਂ
        ਨਤੀਜਾ = oldap_perform_bind(ਅੰਕੜੇ, OLDAP_BIND);
      ਰੋਕੋ;
    }
    ਨਤੀਜਾ = Curl_ssl_cfilter_ਜੋੜੋ(
      ਅੰਕੜੇ, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;
    FALLTHROUGH();
  ਹਾਲਤ OLDAP_TLS:
    ਨਤੀਜਾ = oldap_ssl_connect(ਅੰਕੜੇ, OLDAP_TLS);
    ਜੇ(ਨਤੀਜਾ)
      ਨਤੀਜਾ = oldap_map_error(ਕੋਡ, CURLE_USE_SSL_FAILED);
    ਨਹੀਂਤਾਂ ਜੇ(ssl_installed(conn)) {
      ਜੇ(li->sasl.prefmech != SASL_AUTH_NONE)
        ਨਤੀਜਾ = oldap_perform_mechs(ਅੰਕੜੇ);
      ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->state.creds)
        ਨਤੀਜਾ = oldap_perform_bind(ਅੰਕੜੇ, OLDAP_BIND);
      ਨਹੀਂਤਾਂ {
        /* Version 3 supported: no bind required */
        oldap_state(ਅੰਕੜੇ, li, OLDAP_STOP);
        ਨਤੀਜਾ = CURLE_OK;
      }
    }
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

  ਹਾਲਤ OLDAP_MECHS:
    ਨਤੀਜਾ = oldap_state_mechs_resp(ਅੰਕੜੇ, ਸੁਨੇਹਾ, ਕੋਡ);
    ਰੋਕੋ;
  ਹਾਲਤ OLDAP_SASL:
    ਨਤੀਜਾ = oldap_state_sasl_resp(ਅੰਕੜੇ, ਸੁਨੇਹਾ, ਕੋਡ);
    ਰੋਕੋ;
  ਹਾਲਤ OLDAP_BIND:
  ਹਾਲਤ OLDAP_BINDV2:
    ਨਤੀਜਾ = oldap_state_bind_resp(ਅੰਕੜੇ, ਸੁਨੇਹਾ, ਕੋਡ);
    ਰੋਕੋ;
  ਮੂਲ:
    /* internal error */
    ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
    ਰੋਕੋ;
  }

  ldap_msgfree(ਸੁਨੇਹਾ);

  *ਪੂਰਾ = li->state == OLDAP_STOP;
  ਜੇ(*ਪੂਰਾ)
    conn->recv[FIRSTSOCKET] = oldap_recv;

  ਜੇ(ਨਤੀਜਾ && li->msgid) {
    ldap_abandon_ext(li->ld, li->msgid, ਨਿਰਾ, ਨਿਰਾ);
    li->msgid = 0;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode oldap_disconnect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ connectdata *conn,
                                 bool dead_connection)
{
  ਬਣਤਰ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  (ਖਾਲੀ)dead_connection;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_SSL
  (ਖਾਲੀ)ਅੰਕੜੇ;
#ਜੇ_ਅੰਤ

  ਜੇ(li && li->ld) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
    ਜੇ(ssl_installed(conn)) {
      Sockbuf *sb;
      ਜੇ(ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS ||
         ber_sockbuf_ਜੋੜੋ_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, ਅੰਕੜੇ))
        ਵਾਪਸ CURLE_FAILED_INIT;
    }
#ਜੇ_ਅੰਤ
    ldap_unbind_ext(li->ld, ਨਿਰਾ, ਨਿਰਾ);
    li->ld = ਨਿਰਾ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode oldap_do(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ਬਣਤਰ ldapreqinfo *lr;
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ rc;
  LDAPURLDesc *lud;
  ਪੂਰਨ msgid;

  ਜੇ(!li)
    ਵਾਪਸ CURLE_FAILED_INIT;

  infof(ਅੰਕੜੇ, "LDAP local: %s", Curl_bufref_ptr(&ਅੰਕੜੇ->state.url));

  ਨਤੀਜਾ = oldap_url_parse(ਅੰਕੜੇ, &lud);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  ਜੇ(ssl_installed(conn)) {
    Sockbuf *sb;
    /* re-install the libcurl SSL handlers into the sockbuf. */
    ਜੇ((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_ਜੋੜੋ_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, ਅੰਕੜੇ)) {
      ldap_free_urldesc(lud);
      ਵਾਪਸ CURLE_FAILED_INIT;
    }
  }
#ਜੇ_ਅੰਤ

  rc = ldap_ਖੋਜ_ext(li->ld, lud->lud_dn, lud->lud_scope,
                       lud->lud_filter, lud->lud_attrs, 0,
                       ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, 0, &msgid);
  ldap_free_urldesc(lud);
  ਜੇ(rc != LDAP_SUCCESS) {
    failf(ਅੰਕੜੇ, "LDAP local: ldap_search_ext %s", ldap_err2string(rc));
    ਨਤੀਜਾ = CURLE_LDAP_ਖੋਜ_FAILED;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  lr = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ ldapreqinfo));
  ਜੇ(!lr ||
     Curl_meta_ਸੈੱਟ(ਅੰਕੜੇ, CURL_META_LDAP_EASY, lr, oldap_easy_dtor)) {
    ldap_abandon_ext(li->ld, msgid, ਨਿਰਾ, ਨਿਰਾ);
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  lr->msgid = msgid;
  Curl_xfer_ਤਿਆਰੀ_recv(ਅੰਕੜੇ, FIRSTSOCKET, -1);
  *ਪੂਰਾ = TRUE;

ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode oldap_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLcode res,
                           bool premature)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ldapreqinfo *lr = Curl_meta_get(ਅੰਕੜੇ, CURL_META_LDAP_EASY);

  (ਖਾਲੀ)res;
  (ਖਾਲੀ)premature;

  ਜੇ(lr) {
    /* if there was a search in progress, abandon it */
    ਜੇ(lr->msgid) {
      ਬਣਤਰ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      ਜੇ(li && li->ld) {
        ldap_abandon_ext(li->ld, lr->msgid, ਨਿਰਾ, ਨਿਰਾ);
      }
      lr->msgid = 0;
    }
    Curl_meta_remove(ਅੰਕੜੇ, CURL_META_LDAP_EASY);
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode client_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, size_t plen,
                             ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, size_t len,
                             ਅਚਲ ਅੱਖਰ *suffix, size_t slen)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(ਅਗੇਤਰ) {
    /* If we have a zero-length value and the prefix ends with a space
       separator, drop the latter. */
    ਜੇ(!len && plen && ਅਗੇਤਰ[plen - 1] == ' ')
      plen--;
    ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, ਅਗੇਤਰ, plen);
  }
  ਜੇ(!ਨਤੀਜਾ && ਮੁੱਲ) {
    ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, ਮੁੱਲ, len);
  }
  ਜੇ(!ਨਤੀਜਾ && suffix) {
    ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, suffix, slen);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
ਸਥਿਰ bool oldap_str_needs_base64(ਅਚਲ ਅੱਖਰ *s, size_t len)
{
  size_t i;
  ਜੇ(!len)
    ਵਾਪਸ FALSE;
  ਜੇ((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    ਵਾਪਸ TRUE;
  ਲਈ(i = 0; i < len; i++)
    ਜੇ(!ISPRINT(s[i]))
      ਵਾਪਸ TRUE;
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ CURLcode oldap_recv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex, ਅੱਖਰ *buf,
                           size_t len, size_t *pnread)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ਬਣਤਰ ldapreqinfo *lr = Curl_meta_get(ਅੰਕੜੇ, CURL_META_LDAP_EASY);
  ਪੂਰਨ rc;
  LDAPMessage *ਸੁਨੇਹਾ = ਨਿਰਾ;
  BerElement *ber = ਨਿਰਾ;
  ਬਣਤਰ timeval tv = { 0, 0 };
  ਬਣਤਰ berval bv, *bvals;
  CURLcode ਨਤੀਜਾ = CURLE_AGAIN;
  ਪੂਰਨ ਕੋਡ;
  ਅੱਖਰ *info = ਨਿਰਾ;

  (ਖਾਲੀ)len;
  (ਖਾਲੀ)buf;
  (ਖਾਲੀ)sockindex;
  *pnread = 0;
  ਜੇ(!li || !lr)
    ਵਾਪਸ CURLE_FAILED_INIT;

  rc = ldap_ਨਤੀਜਾ(li->ld, lr->msgid, LDAP_ਸੁਨੇਹਾ_ONE, &tv, &ਸੁਨੇਹਾ);
  ਜੇ(rc < 0) {
    failf(ਅੰਕੜੇ, "LDAP local: search ldap_result %s", ldap_err2string(rc));
    ਨਤੀਜਾ = CURLE_RECV_ERROR;
  }

  /* error or timed out */
  ਜੇ(!ਸੁਨੇਹਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਨਤੀਜਾ = CURLE_OK;

  ਚੋਣ(ldap_msgtype(ਸੁਨੇਹਾ)) {
  ਹਾਲਤ LDAP_RES_ਖੋਜ_ਨਤੀਜਾ:
    lr->msgid = 0;
    rc = ldap_parse_ਨਤੀਜਾ(li->ld, ਸੁਨੇਹਾ, &ਕੋਡ, ਨਿਰਾ, &info, ਨਿਰਾ, ਨਿਰਾ, 0);
    ਜੇ(rc) {
      failf(ਅੰਕੜੇ, "LDAP local: search ldap_parse_result %s",
            ldap_err2string(rc));
      ਨਤੀਜਾ = CURLE_LDAP_ਖੋਜ_FAILED;
      ਰੋਕੋ;
    }

    /* store the latest code for later retrieval */
    ਅੰਕੜੇ->info.httpcode = ਕੋਡ;

    ਚੋਣ(ਕੋਡ) {
    ਹਾਲਤ LDAP_SIZELIMIT_EXCEEDED:
      infof(ਅੰਕੜੇ, "There are more than %d entries", lr->nument);
      FALLTHROUGH();
    ਹਾਲਤ LDAP_SUCCESS:
      ਅੰਕੜੇ->req.ਮਾਪ = ਅੰਕੜੇ->req.bytecount;
      ਰੋਕੋ;
    ਮੂਲ:
      failf(ਅੰਕੜੇ, "LDAP remote: search failed %s %s", ldap_err2string(ਕੋਡ),
            info ? info : "");
      ਨਤੀਜਾ = CURLE_LDAP_ਖੋਜ_FAILED;
      ਰੋਕੋ;
    }
    ਜੇ(info)
      ldap_memfree(info);
    ਰੋਕੋ;
  ਹਾਲਤ LDAP_RES_ਖੋਜ_ਇੰਦਰਾਜ:
    lr->nument++;
    rc = ldap_get_dn_ber(li->ld, ਸੁਨੇਹਾ, &ber, &bv);
    ਜੇ(rc < 0) {
      ਨਤੀਜਾ = CURLE_RECV_ERROR;
      ਰੋਕੋ;
    }

    ਜੇ(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
      ਅੱਖਰ *dn_b64 = ਨਿਰਾ;
      size_t dn_b64_sz = 0;

      /* A DN with e.g. CR/LF would inject LDIF lines, so base64-encode it. */
      ਨਤੀਜਾ = curlx_base64_encode((uint8_t *)bv.bv_val, bv.bv_len,
                                   &dn_b64, &dn_b64_sz);
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = client_write(ਅੰਕੜੇ, STRCONST("DN:: "), dn_b64, dn_b64_sz,
                              STRCONST("\n"));
      curlx_free(dn_b64);
    }
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = client_write(ਅੰਕੜੇ, STRCONST("DN: "), bv.bv_val, bv.bv_len,
                            STRCONST("\n"));
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;

    ਲਈ(rc = ldap_get_attribute_ber(li->ld, ਸੁਨੇਹਾ, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, ਸੁਨੇਹਾ, ber, &bv, &bvals)) {
      ਪੂਰਨ i;
      bool ਦੁਆਧਾਰੀ;

      ਜੇ(!bv.bv_val)
        ਰੋਕੋ;

      /* An attribute type has no base64 form in LDIF, so a type carrying
         control bytes (CR/LF etc.) would let a hostile server inject lines.
         A conformant type name never contains them, so reject it. */
      ਜੇ(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
        failf(ਅੰਕੜੇ, "LDAP local: attribute type contains illegal bytes");
        ਜੇ(bvals)
          ber_memfree(bvals);
        ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;
        ਰੋਕੋ;
      }

      ਜੇ(!bvals) {
        ਨਤੀਜਾ = client_write(ਅੰਕੜੇ, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":\n"));
        ਜੇ(ਨਤੀਜਾ)
          ਰੋਕੋ;
        ਜਾਰੀ;
      }

      ਦੁਆਧਾਰੀ = bv.bv_len > 7 &&
        curl_strnequal(bv.bv_val + bv.bv_len - 7, ";binary", 7);

      ਲਈ(i = 0; bvals[i].bv_val; i++) {
        bool binval = FALSE;

        ਨਤੀਜਾ = client_write(ਅੰਕੜੇ, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":"));
        ਜੇ(ਨਤੀਜਾ)
          ਰੋਕੋ;

        ਜੇ(!ਦੁਆਧਾਰੀ)
          binval = oldap_str_needs_base64(bvals[i].bv_val, bvals[i].bv_len);
        ਜੇ(ਦੁਆਧਾਰੀ || binval) {
          ਅੱਖਰ *val_b64 = ਨਿਰਾ;
          size_t val_b64_sz = 0;

          /* Binary value, encode to base64. */
          ਜੇ(bvals[i].bv_len)
            ਨਤੀਜਾ = curlx_base64_encode((uint8_t *)bvals[i].bv_val,
                                         bvals[i].bv_len,
                                         &val_b64, &val_b64_sz);
          ਜੇ(!ਨਤੀਜਾ)
            ਨਤੀਜਾ = client_write(ਅੰਕੜੇ, STRCONST(": "), val_b64, val_b64_sz,
                                  STRCONST("\n"));
          curlx_free(val_b64);
        }
        ਨਹੀਂਤਾਂ
          ਨਤੀਜਾ = client_write(ਅੰਕੜੇ, STRCONST(" "),
                                bvals[i].bv_val, bvals[i].bv_len,
                                STRCONST("\n"));
        ਜੇ(ਨਤੀਜਾ)
          ਰੋਕੋ;
      }

      ber_memfree(bvals);
      bvals = ਨਿਰਾ;
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = client_write(ਅੰਕੜੇ, STRCONST("\n"), ਨਿਰਾ, 0, ਨਿਰਾ, 0);
      ਜੇ(ਨਤੀਜਾ)
        ਰੋਕੋ;
    }

    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = client_write(ਅੰਕੜੇ, STRCONST("\n"), ਨਿਰਾ, 0, ਨਿਰਾ, 0);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = CURLE_AGAIN;
    ਰੋਕੋ;
  }

  ber_free(ber, 0);
  ldap_msgfree(ਸੁਨੇਹਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ Curl_ldap_ਸੰਸਕਰਣ(ਅੱਖਰ *buf, size_t bufsz)
{
  LDAPAPIInfo api;
  api.ldapai_info_ਸੰਸਕਰਣ = LDAP_API_INFO_ਸੰਸਕਰਣ;

  ਜੇ(ldap_get_option(ਨਿਰਾ, LDAP_OPT_API_INFO, &api) == LDAP_OPT_SUCCESS) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ patch = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(api.ldapai_vendor_ਸੰਸਕਰਣ % 100);
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ major = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(api.ldapai_vendor_ਸੰਸਕਰਣ / 10000);
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ minor =
      (((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)api.ldapai_vendor_ਸੰਸਕਰਣ - (major * 10000))
       - patch) / 100;
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u",
                   api.ldapai_vendor_name, major, minor, patch);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((ਖਾਲੀ **)api.ldapai_extensions);
  }
  ਨਹੀਂਤਾਂ
    curl_msnprintf(buf, bufsz, "OpenLDAP");
}

/*
 * LDAP protocol handler.
 */
ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_ldap = {
  oldap_ਤਿਆਰੀ_connection,               /* setup_connection */
  oldap_do,                             /* do_it */
  oldap_ਪੂਰਾ,                           /* done */
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

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_LDAP && USE_OPENLDAP */

/* The LDAP scheme structs are in ldap.c */
