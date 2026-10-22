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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ !defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP)

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

#ಅನ್ತರ್ಭಾವ <ldap.h>

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "curl_ldap.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "curl_sasl.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  OLDAP_STOP,           /* Do nothing state, stops the state machine */
  OLDAP_SSL,            /* Performing SSL handshake. */
  OLDAP_STARTTLS,       /* STARTTLS request sent. */
  OLDAP_TLS,            /* Performing TLS handshake. */
  OLDAP_MECHS,          /* Get SASL authentication mechanisms. */
  OLDAP_SASL,           /* SASL binding reply. */
  OLDAP_BIND,           /* Simple bind reply. */
  OLDAP_BINDV2,         /* Simple bind reply in protocol version 2. */
  OLDAP_ಕೊನೆಯ            /* Never used */
} ldapstate;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _LDAP_PVT_H
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ ldap_pvt_url_scheme2proto(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ ldap_init_fd(ber_socket_t fd, ಪೂರ್ಣಾಂಕ proto, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url,
                        LDAP **ld);
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ Curl_recv oldap_recv;

ರಚನೆ ldapconninfo {
  ರಚನೆ SASL sasl;          /* SASL-related parameters */
  LDAP *ld;                  /* Openldap connection handle. */
  Curl_recv *recv;           /* For stacking SSL handler */
  Curl_send *send;
  ರಚನೆ berval *servercred; /* SASL data from server. */
  ldapstate state;           /* Current machine state. */
  curl_socket_t dupfd;       /* duplicated socket */
  ಪೂರ್ಣಾಂಕ proto;                 /* LDAP_PROTO_TCP/LDAP_PROTO_UDP/LDAP_PROTO_IPC */
  ಪೂರ್ಣಾಂಕ msgid;                 /* Current message id. */
};

ರಚನೆ ldapreqinfo {
  ಪೂರ್ಣಾಂಕ msgid;
  ಪೂರ್ಣಾಂಕ nument;
};

/* meta key for storing ldapconninfo at easy handle */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_LDAP_EASY   "meta:proto:ldap:easy"
/* meta key for storing ldapconninfo at connection */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_LDAP_CONN   "meta:proto:ldap:conn"

/*
 * oldap_state()
 *
 * This is the ONLY way to change LDAP state!
 */
ಸ್ಥಿರ ಶೂನ್ಯ oldap_state(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ ldapconninfo *li,
                        ldapstate newstate)
{
#ಆದರೆ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ಹೆಸರುಗಳು[] = {
    "STOP",
    "SSL",
    "STARTTLS",
    "TLS",
    "MECHS",
    "SASL",
    "BIND",
    "BINDV2",
    /*ಕೊನೆಯ*/
  };

  ಆದರೆ(li->state != newstate)
    infof(ದತ್ತಾಂಶ, "LDAP %p state change from %s to %s",
          (ಶೂನ್ಯ *)li, ಹೆಸರುಗಳು[li->state], ಹೆಸರುಗಳು[newstate]);
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ
  li->state = newstate;
}

/* Map some particular LDAP error codes to CURLcode values. */
ಸ್ಥಿರ CURLcode oldap_map_error(ಪೂರ್ಣಾಂಕ rc, CURLcode ಫಲಿತಾಂಶ)
{
  ಆಯ್ಕೆ(rc) {
  ಸ್ಥಿತಿ LDAP_NO_ಸ್ಮರಣೆ:
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಸ್ಥಿತಿ LDAP_INVALID_CREDENTIALS:
    ಹಿಂತಿರುಗಿ CURLE_LOGIN_DENIED;
  ಸ್ಥಿತಿ LDAP_PROTOCOL_ERROR:
    ಹಿಂತಿರುಗಿ CURLE_UNSUPPORTED_PROTOCOL;
  ಸ್ಥಿತಿ LDAP_INSUFFICIENT_ACCESS:
    ಹಿಂತಿರುಗಿ CURLE_REMOTE_ACCESS_DENIED;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode oldap_url_parse(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, LDAPURLDesc **ludp)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ rc;
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ url_errs[] = {
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

  *ludp = ಖಾಲಿ;
  /* `ldap_url_parse() seems to be terrible with urls
   * that have user/pass/options in it. So when we have options or
   * creds from the url, fail without calling the function.
   * Yes, this is super-weird code and I do not like it. */
  ಆದರೆ((ದತ್ತಾಂಶ->state.creds && (ದತ್ತಾಂಶ->state.creds->ಮೂಲ == CREDS_URL)) ||
     ದತ್ತಾಂಶ->state.ಮೇಲೆ.options)
    rc = LDAP_URL_ERR_BADURL;
  ಇಲ್ಲದಿದ್ದರೆ
    rc = ldap_url_parse(Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.url), ludp);

  ಆದರೆ(rc != LDAP_URL_SUCCESS) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ = "url parsing problem";

    ಫಲಿತಾಂಶ = rc == LDAP_URL_ERR_MEM ? CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ :
      CURLE_URL_MALFORMAT;
    rc -= LDAP_URL_SUCCESS;
    ಆದರೆ((size_t)rc < CURL_ARRAYSIZE(url_errs))
      ಸಂದೇಶ = url_errs[rc];
    failf(ದತ್ತಾಂಶ, "LDAP local: %s", ಸಂದೇಶ);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Parse the login options. */
ಸ್ಥಿರ CURLcode oldap_parse_login_options(ರಚನೆ connectdata *conn)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr = conn->options;

  DEBUGASSERT(li);
  ಆದರೆ(!li)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆಗ(!ಫಲಿತಾಂಶ && ptr && *ptr) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ = ptr;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ;

    ಆಗ(*ptr && *ptr != '=')
      ptr++;

    ಮೌಲ್ಯ = ptr + 1;

    ಆಗ(*ptr && *ptr != ';')
      ptr++;

    ಆದರೆ(checkprefix("AUTH=", ಕೀಲಿ))
      ಫಲಿತಾಂಶ = Curl_sasl_parse_url_auth_option(&li->sasl, ಮೌಲ್ಯ, ptr - ಮೌಲ್ಯ);
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = CURLE_SETOPT_OPTION_SYNTAX;

    ಆದರೆ(*ptr == ';')
      ptr++;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ == CURLE_URL_MALFORMAT ? CURLE_SETOPT_OPTION_SYNTAX : ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode oldap_ಸಿದ್ಧತೆ_connection(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ connectdata *conn)
{
  CURLcode ಫಲಿತಾಂಶ;
  LDAPURLDesc *lud;
  (ಶೂನ್ಯ)conn;

  /* Early URL syntax check. */
  ಫಲಿತಾಂಶ = oldap_url_parse(ದತ್ತಾಂಶ, &lud);
  ldap_free_urldesc(lud);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Get the SASL authentication challenge from the server credential buffer.
 */
ಸ್ಥಿರ CURLcode oldap_get_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ bufref *ಹೊರಹೋಗು)
{
  ರಚನೆ ldapconninfo *li =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_LDAP_CONN);
  ರಚನೆ berval *servercred = li ? li->servercred : ಖಾಲಿ;
  DEBUGASSERT(li);
  ಆದರೆ(!li)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(!servercred || !servercred->bv_val)
    ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
  Curl_bufref_ಗಣ(ಹೊರಹೋಗು, servercred->bv_val, servercred->bv_len, ಖಾಲಿ);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Sends an initial SASL bind request to the server.
 */
ಸ್ಥಿರ CURLcode oldap_perform_auth(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mech,
                                   ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *initresp)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ರಚನೆ berval cred;
  ರಚನೆ berval *pcred = &cred;
  ಪೂರ್ಣಾಂಕ rc;

  DEBUGASSERT(li);
  ಆದರೆ(!li)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  cred.bv_val = (ಅಕ್ಷರ *)CURL_UNCONST(Curl_bufref_ptr(initresp));
  cred.bv_len = Curl_bufref_len(initresp);
  ಆದರೆ(!cred.bv_val)
    pcred = ಖಾಲಿ;
  rc = ldap_sasl_bind(li->ld, ಖಾಲಿ, mech, pcred, ಖಾಲಿ, ಖಾಲಿ, &li->msgid);
  ಆದರೆ(rc != LDAP_SUCCESS)
    ಹಿಂತಿರುಗಿ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Sends SASL continuation.
 */
ಸ್ಥಿರ CURLcode oldap_continue_auth(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mech,
                                    ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *resp)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ರಚನೆ berval cred;
  ರಚನೆ berval *pcred = &cred;
  ಪೂರ್ಣಾಂಕ rc;

  ಆದರೆ(!li)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  cred.bv_val = (ಅಕ್ಷರ *)CURL_UNCONST(Curl_bufref_ptr(resp));
  cred.bv_len = Curl_bufref_len(resp);
  ಆದರೆ(!cred.bv_val)
    pcred = ಖಾಲಿ;
  rc = ldap_sasl_bind(li->ld, ಖಾಲಿ, mech, pcred, ಖಾಲಿ, ಖಾಲಿ, &li->msgid);
  ಆದರೆ(rc != LDAP_SUCCESS)
    ಹಿಂತಿರುಗಿ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Sends SASL bind cancellation.
 */
ಸ್ಥಿರ CURLcode oldap_cancel_auth(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mech)
{
  ರಚನೆ ldapconninfo *li =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_LDAP_CONN);
  ಪೂರ್ಣಾಂಕ rc;

  (ಶೂನ್ಯ)mech;
  ಆದರೆ(!li)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  rc = ldap_sasl_bind(li->ld, ಖಾಲಿ, LDAP_SASL_NULL, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ,
                      &li->msgid);
  ಆದರೆ(rc != LDAP_SUCCESS)
    ಹಿಂತಿರುಗಿ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Starts LDAP simple bind. */
ಸ್ಥಿರ CURLcode oldap_perform_bind(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ldapstate newstate)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *binddn = ಖಾಲಿ;
  ರಚನೆ berval passwd;
  ಪೂರ್ಣಾಂಕ rc;

  ಆದರೆ(!li)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  passwd.bv_val = ಖಾಲಿ;
  passwd.bv_len = 0;

  ಆದರೆ(conn->creds) {
    binddn = Curl_creds_user(conn->creds);
    passwd.bv_val = CURL_UNCONST(Curl_creds_passwd(conn->creds));
    passwd.bv_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(passwd.bv_val);
  }

  rc = ldap_sasl_bind(li->ld, binddn, LDAP_SASL_SIMPLE, &passwd,
                      ಖಾಲಿ, ಖಾಲಿ, &li->msgid);
  ಆದರೆ(rc != LDAP_SUCCESS)
    ಹಿಂತಿರುಗಿ oldap_map_error(rc,
                           ದತ್ತಾಂಶ->state.creds ?
                           CURLE_LOGIN_DENIED : CURLE_LDAP_CANNOT_BIND);
  oldap_state(ದತ್ತಾಂಶ, li, newstate);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Query the supported SASL authentication mechanisms. */
ಸ್ಥಿರ CURLcode oldap_perform_mechs(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ldapconninfo *li =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_LDAP_CONN);
  ಪೂರ್ಣಾಂಕ rc;
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ supportedSASLMechanisms[] = {
    "supportedSASLMechanisms",
    ಖಾಲಿ
  };

  ಆದರೆ(!li)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  /* Casting away the const for the 3rd parameter that the LDAP API expects as
     a non-const char ** is potentially unsafe but we believe the lack of
     const in the API was an oversight and that no LDAP implementation
     actually modifies the input. */
  rc = ldap_ಹುಡುಕು_ext(li->ld, "", LDAP_SCOPE_ತಳ, "(objectclass=*)",
                       (ಅಕ್ಷರ **)CURL_UNCONST(supportedSASLMechanisms), 0,
                       ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, 0, &li->msgid);
  ಆದರೆ(rc != LDAP_SUCCESS)
    ಹಿಂತಿರುಗಿ oldap_map_error(rc, CURLE_LOGIN_DENIED);
  oldap_state(ದತ್ತಾಂಶ, li, OLDAP_MECHS);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Starts SASL bind. */
ಸ್ಥಿರ CURLcode oldap_perform_sasl(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ldapconninfo *li =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_LDAP_CONN);
  saslprogress ಪ್ರಗತಿ = SASL_IDLE;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!li)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಫಲಿತಾಂಶ = Curl_sasl_ಆರಂಭ(&li->sasl, ದತ್ತಾಂಶ, TRUE, &ಪ್ರಗತಿ);

  oldap_state(ದತ್ತಾಂಶ, li, OLDAP_SASL);
  ಆದರೆ(!ಫಲಿತಾಂಶ && ಪ್ರಗತಿ != SASL_INPROGRESS)
    ಫಲಿತಾಂಶ = Curl_sasl_is_blocked(&li->sasl, ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ldapsb_tls_ಸಿದ್ಧತೆ(Sockbuf_IO_Desc *sbiod, ಶೂನ್ಯ *arg)
{
  sbiod->sbiod_pvt = arg;
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ldapsb_tls_remove(Sockbuf_IO_Desc *sbiod)
{
  sbiod->sbiod_pvt = ಖಾಲಿ;
  ಹಿಂತಿರುಗಿ 0;
}

/* if there is a duplicated socket to close, close it */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ldapsb_tls_close(Sockbuf_IO_Desc *sbiod)
{
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = sbiod->sbiod_pvt;
  ಆದರೆ(ದತ್ತಾಂಶ) {
    ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
    ಆದರೆ(conn) {
      ರಚನೆ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      ಆದರೆ(li && (li->dupfd != CURL_SOCKET_BAD)) {
        sclose(li->dupfd);
        li->dupfd = CURL_SOCKET_BAD;
      }
    }
  }

  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ldapsb_tls_ctrl(Sockbuf_IO_Desc *sbiod, ಪೂರ್ಣಾಂಕ opt, ಶೂನ್ಯ *arg)
{
  (ಶೂನ್ಯ)arg;
  ಆದರೆ(opt == LBER_SB_OPT_ದತ್ತಾಂಶ_READY) {
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = sbiod->sbiod_pvt;
    ಹಿಂತಿರುಗಿ Curl_conn_ದತ್ತಾಂಶ_pending(ದತ್ತಾಂಶ, FIRSTSOCKET);
  }
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ber_slen_t ldapsb_tls_read(Sockbuf_IO_Desc *sbiod, ಶೂನ್ಯ *buf,
                                  ber_len_t len)
{
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  ಆದರೆ(ದತ್ತಾಂಶ) {
    ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
    ಆದರೆ(conn) {
      ರಚನೆ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
      size_t nread;

      ಆದರೆ(!li) {
        ಗಣ_SOCKERRNO(SOCKEINVAL);
        ಹಿಂತಿರುಗಿ -1;
      }
      ಫಲಿತಾಂಶ = (li->recv)(ದತ್ತಾಂಶ, FIRSTSOCKET, buf, len, &nread);
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
        ಗಣ_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = ಫಲಿತಾಂಶ ? -1 : (ber_slen_t)nread;
    }
  }
  ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ber_slen_t ldapsb_tls_write(Sockbuf_IO_Desc *sbiod, ಶೂನ್ಯ *buf,
                                   ber_len_t len)
{
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  ಆದರೆ(ದತ್ತಾಂಶ) {
    ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
    ಆದರೆ(conn) {
      ರಚನೆ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      size_t nwritten;

      ಆದರೆ(!li) {
        ಗಣ_SOCKERRNO(SOCKEINVAL);
        ಹಿಂತಿರುಗಿ -1;
      }
      ಫಲಿತಾಂಶ = (li->send)(ದತ್ತಾಂಶ, FIRSTSOCKET, buf, len, FALSE, &nwritten);
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
        ಗಣ_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = ಫಲಿತಾಂಶ ? -1 : (ber_slen_t)nwritten;
    }
  }
  ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ Sockbuf_IO ldapsb_tls = {
  ldapsb_tls_ಸಿದ್ಧತೆ,
  ldapsb_tls_remove,
  ldapsb_tls_ctrl,
  ldapsb_tls_read,
  ldapsb_tls_write,
  ldapsb_tls_close
};

ಸ್ಥಿರ bool ssl_installed(ರಚನೆ connectdata *conn)
{
  ರಚನೆ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ಹಿಂತಿರುಗಿ li && li->recv;
}

ಸ್ಥಿರ CURLcode oldap_ssl_connect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ldapstate newstate)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  bool ssldone = FALSE;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!li)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಫಲಿತಾಂಶ = Curl_conn_connect(ದತ್ತಾಂಶ, FIRSTSOCKET, FALSE, &ssldone);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  oldap_state(ದತ್ತಾಂಶ, li, newstate);

  ಆದರೆ(ssldone) {
    Sockbuf *sb;

    /* Install the libcurl SSL handlers into the sockbuf. */
    ಆದರೆ((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_ಕೂಡಿಸು_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, ದತ್ತಾಂಶ))
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
    li->recv = conn->recv[FIRSTSOCKET];
    li->send = conn->send[FIRSTSOCKET];
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Send the STARTTLS request */
ಸ್ಥಿರ CURLcode oldap_perform_starttls(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ldapconninfo *li =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_LDAP_CONN);
  ಪೂರ್ಣಾಂಕ rc;

  ಆದರೆ(!li)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  rc = ldap_ಆರಂಭ_tls(li->ld, ಖಾಲಿ, ಖಾಲಿ, &li->msgid);
  ಆದರೆ(rc != LDAP_SUCCESS)
    ಹಿಂತಿರುಗಿ oldap_map_error(rc, CURLE_USE_SSL_FAILED);
  oldap_state(ದತ್ತಾಂಶ, li, OLDAP_STARTTLS);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ oldap_easy_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ ldapreqinfo *lr = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  curlx_free(lr);
}

ಸ್ಥಿರ ಶೂನ್ಯ oldap_conn_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ ldapconninfo *li = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  ಆದರೆ(li->ld) {
    ldap_unbind_ext(li->ld, ಖಾಲಿ, ಖಾಲಿ);
    li->ld = ಖಾಲಿ;
  }
  curlx_free(li);
}

/* SASL parameters for the ldap protocol */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ SASLproto saslldap = {
  "ldap",                     /* The service name */
  oldap_perform_auth,         /* Send authentication command */
  oldap_continue_auth,        /* Send authentication continuation */
  oldap_cancel_auth,          /* Send authentication cancellation */
  oldap_get_ಸಂದೇಶ,          /* Get SASL response message */
  0,                          /* Maximum initial response length (no max) */
  LDAP_SASL_BIND_ಒಳ_ಪ್ರಗತಿ, /* Code received when continuation is expected */
  LDAP_SUCCESS,               /* Code to receive upon authentication success */
  SASL_AUTH_NONE,             /* Default mechanisms */
  0                           /* Configuration flags */
};

ಸ್ಥಿರ CURLcode oldap_connect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ldapconninfo *li;
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಆವೃತ್ತಿ_2 = LDAP_VERSION3;
  ಅಕ್ಷರ *hosturl = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ rc;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_OPENLDAP_DEBUG
  ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ do_trace = -1;
#ಆದರೆ_ಅಂತ್ಯ

  (ಶೂನ್ಯ)ಮುಗಿದ;

  li = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ ldapconninfo));
  ಆದರೆ(!li) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  li->dupfd = CURL_SOCKET_BAD;

  ಫಲಿತಾಂಶ = Curl_conn_meta_ಗಣ(conn, CURL_META_LDAP_CONN, li, oldap_conn_dtor);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  li->proto = ldap_pvt_url_scheme2proto(ದತ್ತಾಂಶ->state.origin->scheme->name);

  /* Initialize the SASL storage */
  Curl_sasl_init(&li->sasl, ದತ್ತಾಂಶ, &saslldap);

  ಫಲಿತಾಂಶ = oldap_parse_login_options(conn);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  hosturl = curl_maprintf("%s://%s:%u",
                          conn->scheme->name,
                          conn->origin->ipv6 ?
                          conn->origin->user_hostname :
                          conn->origin->hostname,
                          conn->origin->ಪೋರ್ಟ್);
  ಆದರೆ(!hosturl) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
    SOCKET dupfd = INVALID_SOCKET;
    WSAPROTOCOL_INFO pi;
    ಆದರೆ(WSADuplicateSocket(conn->sock[FIRSTSOCKET], GetCurrentProcessId(),
                          &pi))
      ; /* error */
    ಇಲ್ಲದಿದ್ದರೆ
      dupfd = CURL_SOCKET(pi.i_ವಿಳಾಸ_Family, pi.iSocketType, pi.iProtocol);
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ F_DUPFD_CLOEXEC
    ಪೂರ್ಣಾಂಕ dupfd = fcntl(conn->sock[FIRSTSOCKET], F_DUPFD_CLOEXEC, 0);
#ಇಲ್ಲದಿದ್ದರೆ
    ಪೂರ್ಣಾಂಕ dupfd = dup(conn->sock[FIRSTSOCKET]);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FCNTL
    ಆದರೆ((dupfd != CURL_SOCKET_BAD) &&
       (fcntl(dupfd, F_SETFD, FD_CLOEXEC) < 0)) {
      sclose(dupfd);
      dupfd = CURL_SOCKET_BAD;
    }
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_FCNTL */
#ಆದರೆ_ಅಂತ್ಯ /* F_DUPFD_CLOEXEC */
#ಆದರೆ_ಅಂತ್ಯ /* _WIN32 */

    ಆದರೆ(dupfd == CURL_SOCKET_BAD) {
      ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    rc = ldap_init_fd((ber_socket_t)dupfd, li->proto, hosturl, &li->ld);
    ಆದರೆ(rc)
      sclose(dupfd);
    ಇಲ್ಲದಿದ್ದರೆ
      /* store the socket, as when this is used by TLS we close this ourselves
         and at this point the code does not yet know if this will use TLS or
         not */
      li->dupfd = dupfd;
  }
  ಆದರೆ(rc) {
    failf(ದತ್ತಾಂಶ, "LDAP local: Cannot connect to %s, %s",
          hosturl, ldap_err2string(rc));
    ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
    ಹೋಗು ಹೊರಹೋಗು;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_OPENLDAP_DEBUG
  ಆದರೆ(do_trace < 0) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *env = getenv("CURL_OPENLDAP_TRACE");
    curl_off_t e = 0;
    ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&env, &e, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ))
      do_trace = e > 0;
  }
  ಆದರೆ(do_trace)
    ldap_ಗಣ_option(li->ld, LDAP_OPT_DEBUG_LEVEL, &do_trace);
#ಆದರೆ_ಅಂತ್ಯ

  /* Try version 3 first. */
  ldap_ಗಣ_option(li->ld, LDAP_OPT_PROTOCOL_ಆವೃತ್ತಿ, &ಆವೃತ್ತಿ_2);

  /* Do not chase referrals. */
  ldap_ಗಣ_option(li->ld, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  {
    ber_len_t max = 256 * 1024;
    Sockbuf *sb;
    ಆದರೆ((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       /* Set the maximum allowed size of an incoming message, which to
          OpenLDAP means that it will malloc() memory up to this size. If not
          set, there is no limit and we instead risk a malloc() failure. */
       !ber_sockbuf_ctrl(sb, LBER_SB_OPT_ಗಣ_MAX_INCOMING, &max)) {
      ಫಲಿತಾಂಶ = CURLE_FAILED_INIT;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  ಆದರೆ(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    ಫಲಿತಾಂಶ = oldap_ssl_connect(ದತ್ತಾಂಶ, OLDAP_SSL);
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.use_ssl) {
    ಫಲಿತಾಂಶ = oldap_perform_starttls(ದತ್ತಾಂಶ);
    ಆದರೆ(!ಫಲಿತಾಂಶ || ದತ್ತಾಂಶ->ಗಣ.use_ssl != CURLUSESSL_TRY)
      ಹೋಗು ಹೊರಹೋಗು;
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(li->sasl.prefmech != SASL_AUTH_NONE) {
    ಫಲಿತಾಂಶ = oldap_perform_mechs(ದತ್ತಾಂಶ);
    ಹೋಗು ಹೊರಹೋಗು;
  }

  /* Force bind even if anonymous bind is not needed in protocol version 3
     to detect missing version 3 support. */
  ಫಲಿತಾಂಶ = oldap_perform_bind(ದತ್ತಾಂಶ, OLDAP_BIND);

ಹೊರಹೋಗು:
  curlx_free(hosturl);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Handle the supported SASL mechanisms query response */
ಸ್ಥಿರ CURLcode oldap_state_mechs_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       LDAPMessage *ಸಂದೇಶ, ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2)
{
  ರಚನೆ connectdata *conn;
  ರಚನೆ ldapconninfo *li;
  ಪೂರ್ಣಾಂಕ rc;
  BerElement *ber = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ berval bv, *bvals;

  ಆದರೆ(!ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  conn = ದತ್ತಾಂಶ->conn;
  li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);

  ಆದರೆ(!li)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಆಯ್ಕೆ(ldap_msgtype(ಸಂದೇಶ)) {
  ಸ್ಥಿತಿ LDAP_RES_ಹುಡುಕು_ನಮೂದು:
    /* Got a list of supported SASL mechanisms. */
    ಆದರೆ(ಸಂಕೇತ_2 != LDAP_SUCCESS && ಸಂಕೇತ_2 != LDAP_NO_RESULTS_RETURNED)
      ಹಿಂತಿರುಗಿ CURLE_LOGIN_DENIED;

    rc = ldap_get_dn_ber(li->ld, ಸಂದೇಶ, &ber, &bv);
    ಆದರೆ(rc < 0)
      ಹಿಂತಿರುಗಿ oldap_map_error(rc, CURLE_BAD_CONTENT_ENCODING);
    ಗಾಗಿ(rc = ldap_get_attribute_ber(li->ld, ಸಂದೇಶ, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, ಸಂದೇಶ, ber, &bv, &bvals)) {
      ಪೂರ್ಣಾಂಕ i;

      ಆದರೆ(!bv.bv_val)
        ನಿಲ್ಲಿಸು;

      ಆದರೆ(bvals) {
        ಗಾಗಿ(i = 0; bvals[i].bv_val; i++) {
          size_t llen;
          ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ mech =
            Curl_sasl_decode_mech((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)bvals[i].bv_val,
                                  bvals[i].bv_len, &llen);
          ಆದರೆ(bvals[i].bv_len == llen)
            li->sasl.authmechs |= mech;
        }
        ber_memfree(bvals);
      }
    }
    ber_free(ber, 0);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ LDAP_RES_ಹುಡುಕು_ಫಲಿತಾಂಶ:
    ಆಯ್ಕೆ(ಸಂಕೇತ_2) {
    ಸ್ಥಿತಿ LDAP_SIZELIMIT_EXCEEDED:
      infof(ದತ್ತಾಂಶ, "Too many authentication mechanisms");
      FALLTHROUGH();
    ಸ್ಥಿತಿ LDAP_SUCCESS:
    ಸ್ಥಿತಿ LDAP_NO_RESULTS_RETURNED:
      ಆದರೆ(Curl_sasl_can_authenticate(&li->sasl, ದತ್ತಾಂಶ))
        ಫಲಿತಾಂಶ = oldap_perform_sasl(ದತ್ತಾಂಶ);
      ಇಲ್ಲದಿದ್ದರೆ
        ಫಲಿತಾಂಶ = CURLE_LOGIN_DENIED;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಫಲಿತಾಂಶ = oldap_map_error(ಸಂಕೇತ_2, CURLE_LOGIN_DENIED);
      ನಿಲ್ಲಿಸು;
    }
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Handle a SASL bind response. */
ಸ್ಥಿರ CURLcode oldap_state_sasl_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      LDAPMessage *ಸಂದೇಶ, ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  saslprogress ಪ್ರಗತಿ;
  ಪೂರ್ಣಾಂಕ rc;

  ಆದರೆ(!li)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  li->servercred = ಖಾಲಿ;
  rc = ldap_parse_sasl_bind_ಫಲಿತಾಂಶ(li->ld, ಸಂದೇಶ, &li->servercred, 0);
  ಆದರೆ(rc != LDAP_SUCCESS) {
    failf(ದತ್ತಾಂಶ, "LDAP local: sasl ldap_parse_result %s", ldap_err2string(rc));
    ಫಲಿತಾಂಶ = oldap_map_error(rc, CURLE_LOGIN_DENIED);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ = Curl_sasl_continue(&li->sasl, ದತ್ತಾಂಶ, ಸಂಕೇತ_2, &ಪ್ರಗತಿ);
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      ಆಯ್ಕೆ(ಪ್ರಗತಿ) {
      ಸ್ಥಿತಿ SASL_ಮುಗಿದ:
        oldap_state(ದತ್ತಾಂಶ, li, OLDAP_STOP);   /* Authenticated */
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SASL_IDLE:            /* No mechanism left after cancellation */
        failf(ದತ್ತಾಂಶ, "Authentication cancelled");
        ಫಲಿತಾಂಶ = CURLE_LOGIN_DENIED;
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು;
      }
    }
  }

  ಆದರೆ(li->servercred)
    ber_bvfree(li->servercred);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Handle a simple bind response. */
ಸ್ಥಿರ CURLcode oldap_state_bind_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, LDAPMessage *ಸಂದೇಶ,
                                      ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ berval *bv = ಖಾಲಿ;
  ಪೂರ್ಣಾಂಕ rc;

  ಆದರೆ(!li)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(ಸಂಕೇತ_2 != LDAP_SUCCESS)
    ಹಿಂತಿರುಗಿ oldap_map_error(ಸಂಕೇತ_2, CURLE_LDAP_CANNOT_BIND);

  rc = ldap_parse_sasl_bind_ಫಲಿತಾಂಶ(li->ld, ಸಂದೇಶ, &bv, 0);
  ಆದರೆ(rc != LDAP_SUCCESS) {
    failf(ದತ್ತಾಂಶ, "LDAP local: bind ldap_parse_sasl_bind_result %s",
          ldap_err2string(rc));
    ಫಲಿತಾಂಶ = oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    oldap_state(ದತ್ತಾಂಶ, li, OLDAP_STOP);

  ಆದರೆ(bv)
    ber_bvfree(bv);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode oldap_connecting(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  LDAPMessage *ಸಂದೇಶ = ಖಾಲಿ;
  ರಚನೆ timeval tv = { 0, 0 };
  ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2 = LDAP_SUCCESS;
  ಪೂರ್ಣಾಂಕ rc;

  ಆದರೆ(!li)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(li->state != OLDAP_SSL && li->state != OLDAP_TLS) {
    /* Get response to last command. */
    rc = ldap_ಫಲಿತಾಂಶ(li->ld, li->msgid, LDAP_ಸಂದೇಶ_ONE, &tv, &ಸಂದೇಶ);
    ಆಯ್ಕೆ(rc) {
    ಸ್ಥಿತಿ 0:                               /* Timed out. */
      ಹಿಂತಿರುಗಿ CURLE_OK;
    ಸ್ಥಿತಿ LDAP_RES_ಹುಡುಕು_ನಮೂದು:
    ಸ್ಥಿತಿ LDAP_RES_ಹುಡುಕು_REFERENCE:
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      li->msgid = 0;                      /* Nothing to abandon upon error. */
      ಆದರೆ(rc < 0) {
        failf(ದತ್ತಾಂಶ, "LDAP local: connecting ldap_result %s",
              ldap_err2string(rc));
        ಹಿಂತಿರುಗಿ oldap_map_error(rc, CURLE_COULDNT_CONNECT);
      }
      ನಿಲ್ಲಿಸು;
    }

    /* Get error code from message. */
    rc = ldap_parse_ಫಲಿತಾಂಶ(li->ld, ಸಂದೇಶ, &ಸಂಕೇತ_2, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, 0);
    ಆದರೆ(rc)
      ಸಂಕೇತ_2 = rc;
    ಇಲ್ಲದಿದ್ದರೆ {
      /* store the latest code for later retrieval */
      ದತ್ತಾಂಶ->info.httpcode = ಸಂಕೇತ_2;
    }

    /* If protocol version 3 is not supported, fallback to version 2. */
    ಆದರೆ(ಸಂಕೇತ_2 == LDAP_PROTOCOL_ERROR && li->state != OLDAP_BINDV2 &&
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
       (ssl_installed(conn) || ದತ್ತಾಂಶ->ಗಣ.use_ssl <= CURLUSESSL_TRY) &&
#ಆದರೆ_ಅಂತ್ಯ
       li->sasl.prefmech == SASL_AUTH_NONE) {
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಆವೃತ್ತಿ_2 = LDAP_VERSION2;

      ldap_ಗಣ_option(li->ld, LDAP_OPT_PROTOCOL_ಆವೃತ್ತಿ, &ಆವೃತ್ತಿ_2);
      ldap_msgfree(ಸಂದೇಶ);
      ಹಿಂತಿರುಗಿ oldap_perform_bind(ದತ್ತಾಂಶ, OLDAP_BINDV2);
    }
  }

  /* Handle response message according to current state. */
  ಆಯ್ಕೆ(li->state) {

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  ಸ್ಥಿತಿ OLDAP_SSL:
    ಫಲಿತಾಂಶ = oldap_ssl_connect(ದತ್ತಾಂಶ, OLDAP_SSL);
    ಆದರೆ(!ಫಲಿತಾಂಶ && ssl_installed(conn)) {
      ಆದರೆ(li->sasl.prefmech != SASL_AUTH_NONE)
        ಫಲಿತಾಂಶ = oldap_perform_mechs(ದತ್ತಾಂಶ);
      ಇಲ್ಲದಿದ್ದರೆ
        ಫಲಿತಾಂಶ = oldap_perform_bind(ದತ್ತಾಂಶ, OLDAP_BIND);
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ OLDAP_STARTTLS:
    ಆದರೆ(ಸಂಕೇತ_2 != LDAP_SUCCESS) {
      ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.use_ssl != CURLUSESSL_TRY)
        ಫಲಿತಾಂಶ = oldap_map_error(ಸಂಕೇತ_2, CURLE_USE_SSL_FAILED);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(li->sasl.prefmech != SASL_AUTH_NONE)
        ಫಲಿತಾಂಶ = oldap_perform_mechs(ದತ್ತಾಂಶ);
      ಇಲ್ಲದಿದ್ದರೆ
        ಫಲಿತಾಂಶ = oldap_perform_bind(ದತ್ತಾಂಶ, OLDAP_BIND);
      ನಿಲ್ಲಿಸು;
    }
    ಫಲಿತಾಂಶ = Curl_ssl_cfilter_ಕೂಡಿಸು(
      ದತ್ತಾಂಶ, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;
    FALLTHROUGH();
  ಸ್ಥಿತಿ OLDAP_TLS:
    ಫಲಿತಾಂಶ = oldap_ssl_connect(ದತ್ತಾಂಶ, OLDAP_TLS);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = oldap_map_error(ಸಂಕೇತ_2, CURLE_USE_SSL_FAILED);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ssl_installed(conn)) {
      ಆದರೆ(li->sasl.prefmech != SASL_AUTH_NONE)
        ಫಲಿತಾಂಶ = oldap_perform_mechs(ದತ್ತಾಂಶ);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->state.creds)
        ಫಲಿತಾಂಶ = oldap_perform_bind(ದತ್ತಾಂಶ, OLDAP_BIND);
      ಇಲ್ಲದಿದ್ದರೆ {
        /* Version 3 supported: no bind required */
        oldap_state(ದತ್ತಾಂಶ, li, OLDAP_STOP);
        ಫಲಿತಾಂಶ = CURLE_OK;
      }
    }
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

  ಸ್ಥಿತಿ OLDAP_MECHS:
    ಫಲಿತಾಂಶ = oldap_state_mechs_resp(ದತ್ತಾಂಶ, ಸಂದೇಶ, ಸಂಕೇತ_2);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ OLDAP_SASL:
    ಫಲಿತಾಂಶ = oldap_state_sasl_resp(ದತ್ತಾಂಶ, ಸಂದೇಶ, ಸಂಕೇತ_2);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ OLDAP_BIND:
  ಸ್ಥಿತಿ OLDAP_BINDV2:
    ಫಲಿತಾಂಶ = oldap_state_bind_resp(ದತ್ತಾಂಶ, ಸಂದೇಶ, ಸಂಕೇತ_2);
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    /* internal error */
    ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
    ನಿಲ್ಲಿಸು;
  }

  ldap_msgfree(ಸಂದೇಶ);

  *ಮುಗಿದ = li->state == OLDAP_STOP;
  ಆದರೆ(*ಮುಗಿದ)
    conn->recv[FIRSTSOCKET] = oldap_recv;

  ಆದರೆ(ಫಲಿತಾಂಶ && li->msgid) {
    ldap_abandon_ext(li->ld, li->msgid, ಖಾಲಿ, ಖಾಲಿ);
    li->msgid = 0;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode oldap_disconnect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ connectdata *conn,
                                 bool dead_connection)
{
  ರಚನೆ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  (ಶೂನ್ಯ)dead_connection;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_SSL
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(li && li->ld) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
    ಆದರೆ(ssl_installed(conn)) {
      Sockbuf *sb;
      ಆದರೆ(ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS ||
         ber_sockbuf_ಕೂಡಿಸು_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, ದತ್ತಾಂಶ))
        ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ldap_unbind_ext(li->ld, ಖಾಲಿ, ಖಾಲಿ);
    li->ld = ಖಾಲಿ;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode oldap_do(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ರಚನೆ ldapreqinfo *lr;
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ rc;
  LDAPURLDesc *lud;
  ಪೂರ್ಣಾಂಕ msgid;

  ಆದರೆ(!li)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  infof(ದತ್ತಾಂಶ, "LDAP local: %s", Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.url));

  ಫಲಿತಾಂಶ = oldap_url_parse(ದತ್ತಾಂಶ, &lud);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  ಆದರೆ(ssl_installed(conn)) {
    Sockbuf *sb;
    /* re-install the libcurl SSL handlers into the sockbuf. */
    ಆದರೆ((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_ಕೂಡಿಸು_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, ದತ್ತಾಂಶ)) {
      ldap_free_urldesc(lud);
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ

  rc = ldap_ಹುಡುಕು_ext(li->ld, lud->lud_dn, lud->lud_scope,
                       lud->lud_filter, lud->lud_attrs, 0,
                       ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, 0, &msgid);
  ldap_free_urldesc(lud);
  ಆದರೆ(rc != LDAP_SUCCESS) {
    failf(ದತ್ತಾಂಶ, "LDAP local: ldap_search_ext %s", ldap_err2string(rc));
    ಫಲಿತಾಂಶ = CURLE_LDAP_ಹುಡುಕು_FAILED;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  lr = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ ldapreqinfo));
  ಆದರೆ(!lr ||
     Curl_meta_ಗಣ(ದತ್ತಾಂಶ, CURL_META_LDAP_EASY, lr, oldap_easy_dtor)) {
    ldap_abandon_ext(li->ld, msgid, ಖಾಲಿ, ಖಾಲಿ);
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  lr->msgid = msgid;
  Curl_xfer_ಸಿದ್ಧತೆ_recv(ದತ್ತಾಂಶ, FIRSTSOCKET, -1);
  *ಮುಗಿದ = TRUE;

ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode oldap_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLcode res,
                           bool premature)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ldapreqinfo *lr = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_LDAP_EASY);

  (ಶೂನ್ಯ)res;
  (ಶೂನ್ಯ)premature;

  ಆದರೆ(lr) {
    /* if there was a search in progress, abandon it */
    ಆದರೆ(lr->msgid) {
      ರಚನೆ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      ಆದರೆ(li && li->ld) {
        ldap_abandon_ext(li->ld, lr->msgid, ಖಾಲಿ, ಖಾಲಿ);
      }
      lr->msgid = 0;
    }
    Curl_meta_remove(ದತ್ತಾಂಶ, CURL_META_LDAP_EASY);
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode client_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, size_t plen,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, size_t len,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, size_t slen)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(ಪೂರ್ವಪ್ರತ್ಯಯ) {
    /* If we have a zero-length value and the prefix ends with a space
       separator, drop the latter. */
    ಆದರೆ(!len && plen && ಪೂರ್ವಪ್ರತ್ಯಯ[plen - 1] == ' ')
      plen--;
    ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, ಪೂರ್ವಪ್ರತ್ಯಯ, plen);
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ && ಮೌಲ್ಯ) {
    ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, ಮೌಲ್ಯ, len);
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ && suffix) {
    ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, suffix, slen);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
ಸ್ಥಿರ bool oldap_str_needs_base64(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len)
{
  size_t i;
  ಆದರೆ(!len)
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    ಹಿಂತಿರುಗಿ TRUE;
  ಗಾಗಿ(i = 0; i < len; i++)
    ಆದರೆ(!ISPRINT(s[i]))
      ಹಿಂತಿರುಗಿ TRUE;
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ CURLcode oldap_recv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex, ಅಕ್ಷರ *buf,
                           size_t len, size_t *pnread)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ರಚನೆ ldapreqinfo *lr = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_LDAP_EASY);
  ಪೂರ್ಣಾಂಕ rc;
  LDAPMessage *ಸಂದೇಶ = ಖಾಲಿ;
  BerElement *ber = ಖಾಲಿ;
  ರಚನೆ timeval tv = { 0, 0 };
  ರಚನೆ berval bv, *bvals;
  CURLcode ಫಲಿತಾಂಶ = CURLE_AGAIN;
  ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2;
  ಅಕ್ಷರ *info = ಖಾಲಿ;

  (ಶೂನ್ಯ)len;
  (ಶೂನ್ಯ)buf;
  (ಶೂನ್ಯ)sockindex;
  *pnread = 0;
  ಆದರೆ(!li || !lr)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  rc = ldap_ಫಲಿತಾಂಶ(li->ld, lr->msgid, LDAP_ಸಂದೇಶ_ONE, &tv, &ಸಂದೇಶ);
  ಆದರೆ(rc < 0) {
    failf(ದತ್ತಾಂಶ, "LDAP local: search ldap_result %s", ldap_err2string(rc));
    ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
  }

  /* error or timed out */
  ಆದರೆ(!ಸಂದೇಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = CURLE_OK;

  ಆಯ್ಕೆ(ldap_msgtype(ಸಂದೇಶ)) {
  ಸ್ಥಿತಿ LDAP_RES_ಹುಡುಕು_ಫಲಿತಾಂಶ:
    lr->msgid = 0;
    rc = ldap_parse_ಫಲಿತಾಂಶ(li->ld, ಸಂದೇಶ, &ಸಂಕೇತ_2, ಖಾಲಿ, &info, ಖಾಲಿ, ಖಾಲಿ, 0);
    ಆದರೆ(rc) {
      failf(ದತ್ತಾಂಶ, "LDAP local: search ldap_parse_result %s",
            ldap_err2string(rc));
      ಫಲಿತಾಂಶ = CURLE_LDAP_ಹುಡುಕು_FAILED;
      ನಿಲ್ಲಿಸು;
    }

    /* store the latest code for later retrieval */
    ದತ್ತಾಂಶ->info.httpcode = ಸಂಕೇತ_2;

    ಆಯ್ಕೆ(ಸಂಕೇತ_2) {
    ಸ್ಥಿತಿ LDAP_SIZELIMIT_EXCEEDED:
      infof(ದತ್ತಾಂಶ, "There are more than %d entries", lr->nument);
      FALLTHROUGH();
    ಸ್ಥಿತಿ LDAP_SUCCESS:
      ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = ದತ್ತಾಂಶ->req.bytecount;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      failf(ದತ್ತಾಂಶ, "LDAP remote: search failed %s %s", ldap_err2string(ಸಂಕೇತ_2),
            info ? info : "");
      ಫಲಿತಾಂಶ = CURLE_LDAP_ಹುಡುಕು_FAILED;
      ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ(info)
      ldap_memfree(info);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ LDAP_RES_ಹುಡುಕು_ನಮೂದು:
    lr->nument++;
    rc = ldap_get_dn_ber(li->ld, ಸಂದೇಶ, &ber, &bv);
    ಆದರೆ(rc < 0) {
      ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
      ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
      ಅಕ್ಷರ *dn_b64 = ಖಾಲಿ;
      size_t dn_b64_sz = 0;

      /* A DN with e.g. CR/LF would inject LDIF lines, so base64-encode it. */
      ಫಲಿತಾಂಶ = curlx_base64_encode((uint8_t *)bv.bv_val, bv.bv_len,
                                   &dn_b64, &dn_b64_sz);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = client_write(ದತ್ತಾಂಶ, STRCONST("DN:: "), dn_b64, dn_b64_sz,
                              STRCONST("\n"));
      curlx_free(dn_b64);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = client_write(ದತ್ತಾಂಶ, STRCONST("DN: "), bv.bv_val, bv.bv_len,
                            STRCONST("\n"));
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;

    ಗಾಗಿ(rc = ldap_get_attribute_ber(li->ld, ಸಂದೇಶ, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, ಸಂದೇಶ, ber, &bv, &bvals)) {
      ಪೂರ್ಣಾಂಕ i;
      bool ದ್ವಿಮಾನ;

      ಆದರೆ(!bv.bv_val)
        ನಿಲ್ಲಿಸು;

      /* An attribute type has no base64 form in LDIF, so a type carrying
         control bytes (CR/LF etc.) would let a hostile server inject lines.
         A conformant type name never contains them, so reject it. */
      ಆದರೆ(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
        failf(ದತ್ತಾಂಶ, "LDAP local: attribute type contains illegal bytes");
        ಆದರೆ(bvals)
          ber_memfree(bvals);
        ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;
        ನಿಲ್ಲಿಸು;
      }

      ಆದರೆ(!bvals) {
        ಫಲಿತಾಂಶ = client_write(ದತ್ತಾಂಶ, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":\n"));
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ನಿಲ್ಲಿಸು;
        ಮುಂದುವರಿ;
      }

      ದ್ವಿಮಾನ = bv.bv_len > 7 &&
        curl_strnequal(bv.bv_val + bv.bv_len - 7, ";binary", 7);

      ಗಾಗಿ(i = 0; bvals[i].bv_val; i++) {
        bool binval = FALSE;

        ಫಲಿತಾಂಶ = client_write(ದತ್ತಾಂಶ, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":"));
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ನಿಲ್ಲಿಸು;

        ಆದರೆ(!ದ್ವಿಮಾನ)
          binval = oldap_str_needs_base64(bvals[i].bv_val, bvals[i].bv_len);
        ಆದರೆ(ದ್ವಿಮಾನ || binval) {
          ಅಕ್ಷರ *val_b64 = ಖಾಲಿ;
          size_t val_b64_sz = 0;

          /* Binary value, encode to base64. */
          ಆದರೆ(bvals[i].bv_len)
            ಫಲಿತಾಂಶ = curlx_base64_encode((uint8_t *)bvals[i].bv_val,
                                         bvals[i].bv_len,
                                         &val_b64, &val_b64_sz);
          ಆದರೆ(!ಫಲಿತಾಂಶ)
            ಫಲಿತಾಂಶ = client_write(ದತ್ತಾಂಶ, STRCONST(": "), val_b64, val_b64_sz,
                                  STRCONST("\n"));
          curlx_free(val_b64);
        }
        ಇಲ್ಲದಿದ್ದರೆ
          ಫಲಿತಾಂಶ = client_write(ದತ್ತಾಂಶ, STRCONST(" "),
                                bvals[i].bv_val, bvals[i].bv_len,
                                STRCONST("\n"));
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ನಿಲ್ಲಿಸು;
      }

      ber_memfree(bvals);
      bvals = ಖಾಲಿ;
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = client_write(ದತ್ತಾಂಶ, STRCONST("\n"), ಖಾಲಿ, 0, ಖಾಲಿ, 0);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = client_write(ದತ್ತಾಂಶ, STRCONST("\n"), ಖಾಲಿ, 0, ಖಾಲಿ, 0);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = CURLE_AGAIN;
    ನಿಲ್ಲಿಸು;
  }

  ber_free(ber, 0);
  ldap_msgfree(ಸಂದೇಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ Curl_ldap_ಆವೃತ್ತಿ(ಅಕ್ಷರ *buf, size_t bufsz)
{
  LDAPAPIInfo api;
  api.ldapai_info_ಆವೃತ್ತಿ = LDAP_API_INFO_ಆವೃತ್ತಿ;

  ಆದರೆ(ldap_get_option(ಖಾಲಿ, LDAP_OPT_API_INFO, &api) == LDAP_OPT_SUCCESS) {
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ patch = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(api.ldapai_vendor_ಆವೃತ್ತಿ % 100);
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ major = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(api.ldapai_vendor_ಆವೃತ್ತಿ / 10000);
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ minor =
      (((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)api.ldapai_vendor_ಆವೃತ್ತಿ - (major * 10000))
       - patch) / 100;
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u",
                   api.ldapai_vendor_name, major, minor, patch);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((ಶೂನ್ಯ **)api.ldapai_extensions);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    curl_msnprintf(buf, bufsz, "OpenLDAP");
}

/*
 * LDAP protocol handler.
 */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_ldap = {
  oldap_ಸಿದ್ಧತೆ_connection,               /* setup_connection */
  oldap_do,                             /* do_it */
  oldap_ಮುಗಿದ,                           /* done */
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

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_LDAP && USE_OPENLDAP */

/* The LDAP scheme structs are in ldap.c */
