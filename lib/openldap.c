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

#ಯದಿ !defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP)

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
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
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
ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ ldap_pvt_url_scheme2proto(ಧ್ರುವ ವರ್ಣ *);
ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ ldap_init_fd(ber_socket_t fd, ಪೂರ್ಣಾಙ್ಕ proto, ಧ್ರುವ ವರ್ಣ *url,
                        LDAP **ld);
#endif

ಸ್ಥಿರ Curl_recv oldap_recv;

ಸಂರಚನಾ ldapconninfo {
  ಸಂರಚನಾ SASL sasl;          /* SASL-related parameters */
  LDAP *ld;                  /* Openldap connection handle. */
  Curl_recv *recv;           /* For stacking SSL handler */
  Curl_send *send;
  ಸಂರಚನಾ berval *servercred; /* SASL data from server. */
  ldapstate state;           /* Current machine state. */
  curl_socket_t dupfd;       /* duplicated socket */
  ಪೂರ್ಣಾಙ್ಕ proto;                 /* LDAP_PROTO_TCP/LDAP_PROTO_UDP/LDAP_PROTO_IPC */
  ಪೂರ್ಣಾಙ್ಕ msgid;                 /* Current message id. */
};

ಸಂರಚನಾ ldapreqinfo {
  ಪೂರ್ಣಾಙ್ಕ msgid;
  ಪೂರ್ಣಾಙ್ಕ nument;
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
ಸ್ಥಿರ ಶೂನ್ಯ oldap_state(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ ldapconninfo *li,
                        ldapstate newstate)
{
#ಯದಿ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ names[] = {
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

  ಯದಿ(li->state != newstate)
    infof(data, "LDAP %p state change from %s to %s",
          (ಶೂನ್ಯ *)li, names[li->state], names[newstate]);
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
#endif
  li->state = newstate;
}

/* Map some particular LDAP error codes to CURLcode values. */
ಸ್ಥಿರ CURLcode oldap_map_error(ಪೂರ್ಣಾಙ್ಕ rc, CURLcode result)
{
  ವಿಕಲ್ಪ(rc) {
  ಪಕ್ಷ LDAP_NO_MEMORY:
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  ಪಕ್ಷ LDAP_INVALID_CREDENTIALS:
    ಪ್ರತ್ಯಾಗಮ CURLE_LOGIN_DENIED;
  ಪಕ್ಷ LDAP_PROTOCOL_ERROR:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNSUPPORTED_PROTOCOL;
  ಪಕ್ಷ LDAP_INSUFFICIENT_ACCESS:
    ಪ್ರತ್ಯಾಗಮ CURLE_REMOTE_ACCESS_DENIED;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode oldap_url_parse(ಸಂರಚನಾ Curl_easy *data, LDAPURLDesc **ludp)
{
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ rc;
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ url_errs[] = {
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

  *ludp = ರಿಕ್ತ;
  /* `ldap_url_parse() seems to be terrible with urls
   * that have user/pass/options in it. So when we have options or
   * creds from the url, fail without calling the function.
   * Yes, this is super-weird code and I do not like it. */
  ಯದಿ((data->state.creds && (data->state.creds->source == CREDS_URL)) ||
     data->state.up.options)
    rc = LDAP_URL_ERR_BADURL;
  ಅನ್ಯಥಾ
    rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), ludp);

  ಯದಿ(rc != LDAP_URL_SUCCESS) {
    ಧ್ರುವ ವರ್ಣ *ಸಂದೇಶ = "url parsing problem";

    result = rc == LDAP_URL_ERR_MEM ? CURLE_OUT_OF_MEMORY :
      CURLE_URL_MALFORMAT;
    rc -= LDAP_URL_SUCCESS;
    ಯದಿ((size_t)rc < CURL_ARRAYSIZE(url_errs))
      ಸಂದೇಶ = url_errs[rc];
    failf(data, "LDAP local: %s", ಸಂದೇಶ);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/* Parse the login options. */
ಸ್ಥಿರ CURLcode oldap_parse_login_options(ಸಂರಚನಾ connectdata *conn)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ಧ್ರುವ ವರ್ಣ *ptr = conn->options;

  DEBUGASSERT(li);
  ಯದಿ(!li)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯಾವತ್(!result && ptr && *ptr) {
    ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ = ptr;
    ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ;

    ಯಾವತ್(*ptr && *ptr != '=')
      ptr++;

    ಮೌಲ್ಯ = ptr + 1;

    ಯಾವತ್(*ptr && *ptr != ';')
      ptr++;

    ಯದಿ(checkprefix("AUTH=", ಕೀಲಿ))
      result = Curl_sasl_parse_url_auth_option(&li->sasl, ಮೌಲ್ಯ, ptr - ಮೌಲ್ಯ);
    ಅನ್ಯಥಾ
      result = CURLE_SETOPT_OPTION_SYNTAX;

    ಯದಿ(*ptr == ';')
      ptr++;
  }

  ಪ್ರತ್ಯಾಗಮ result == CURLE_URL_MALFORMAT ? CURLE_SETOPT_OPTION_SYNTAX : result;
}

ಸ್ಥಿರ CURLcode oldap_setup_connection(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ connectdata *conn)
{
  CURLcode result;
  LDAPURLDesc *lud;
  (ಶೂನ್ಯ)conn;

  /* Early URL syntax check. */
  result = oldap_url_parse(data, &lud);
  ldap_free_urldesc(lud);

  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Get the SASL authentication challenge from the server credential buffer.
 */
ಸ್ಥಿರ CURLcode oldap_get_message(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ bufref *out)
{
  ಸಂರಚನಾ ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  ಸಂರಚನಾ berval *servercred = li ? li->servercred : ರಿಕ್ತ;
  DEBUGASSERT(li);
  ಯದಿ(!li)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯದಿ(!servercred || !servercred->bv_val)
    ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
  Curl_bufref_set(out, servercred->bv_val, servercred->bv_len, ರಿಕ್ತ);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Sends an initial SASL bind request to the server.
 */
ಸ್ಥಿರ CURLcode oldap_perform_auth(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *mech,
                                   ಧ್ರುವ ಸಂರಚನಾ bufref *initresp)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ಸಂರಚನಾ berval cred;
  ಸಂರಚನಾ berval *pcred = &cred;
  ಪೂರ್ಣಾಙ್ಕ rc;

  DEBUGASSERT(li);
  ಯದಿ(!li)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  cred.bv_val = (ವರ್ಣ *)CURL_UNCONST(Curl_bufref_ptr(initresp));
  cred.bv_len = Curl_bufref_len(initresp);
  ಯದಿ(!cred.bv_val)
    pcred = ರಿಕ್ತ;
  rc = ldap_sasl_bind(li->ld, ರಿಕ್ತ, mech, pcred, ರಿಕ್ತ, ರಿಕ್ತ, &li->msgid);
  ಯದಿ(rc != LDAP_SUCCESS)
    ಪ್ರತ್ಯಾಗಮ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Sends SASL continuation.
 */
ಸ್ಥಿರ CURLcode oldap_continue_auth(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *mech,
                                    ಧ್ರುವ ಸಂರಚನಾ bufref *resp)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ಸಂರಚನಾ berval cred;
  ಸಂರಚನಾ berval *pcred = &cred;
  ಪೂರ್ಣಾಙ್ಕ rc;

  ಯದಿ(!li)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  cred.bv_val = (ವರ್ಣ *)CURL_UNCONST(Curl_bufref_ptr(resp));
  cred.bv_len = Curl_bufref_len(resp);
  ಯದಿ(!cred.bv_val)
    pcred = ರಿಕ್ತ;
  rc = ldap_sasl_bind(li->ld, ರಿಕ್ತ, mech, pcred, ರಿಕ್ತ, ರಿಕ್ತ, &li->msgid);
  ಯದಿ(rc != LDAP_SUCCESS)
    ಪ್ರತ್ಯಾಗಮ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Sends SASL bind cancellation.
 */
ಸ್ಥಿರ CURLcode oldap_cancel_auth(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *mech)
{
  ಸಂರಚನಾ ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  ಪೂರ್ಣಾಙ್ಕ rc;

  (ಶೂನ್ಯ)mech;
  ಯದಿ(!li)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  rc = ldap_sasl_bind(li->ld, ರಿಕ್ತ, LDAP_SASL_NULL, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ,
                      &li->msgid);
  ಯದಿ(rc != LDAP_SUCCESS)
    ಪ್ರತ್ಯಾಗಮ oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Starts LDAP simple bind. */
ಸ್ಥಿರ CURLcode oldap_perform_bind(ಸಂರಚನಾ Curl_easy *data, ldapstate newstate)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ಧ್ರುವ ವರ್ಣ *binddn = ರಿಕ್ತ;
  ಸಂರಚನಾ berval passwd;
  ಪೂರ್ಣಾಙ್ಕ rc;

  ಯದಿ(!li)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  passwd.bv_val = ರಿಕ್ತ;
  passwd.bv_len = 0;

  ಯದಿ(conn->creds) {
    binddn = Curl_creds_user(conn->creds);
    passwd.bv_val = CURL_UNCONST(Curl_creds_passwd(conn->creds));
    passwd.bv_len = strlen(passwd.bv_val);
  }

  rc = ldap_sasl_bind(li->ld, binddn, LDAP_SASL_SIMPLE, &passwd,
                      ರಿಕ್ತ, ರಿಕ್ತ, &li->msgid);
  ಯದಿ(rc != LDAP_SUCCESS)
    ಪ್ರತ್ಯಾಗಮ oldap_map_error(rc,
                           data->state.creds ?
                           CURLE_LOGIN_DENIED : CURLE_LDAP_CANNOT_BIND);
  oldap_state(data, li, newstate);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Query the supported SASL authentication mechanisms. */
ಸ್ಥಿರ CURLcode oldap_perform_mechs(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  ಪೂರ್ಣಾಙ್ಕ rc;
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ supportedSASLMechanisms[] = {
    "supportedSASLMechanisms",
    ರಿಕ್ತ
  };

  ಯದಿ(!li)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  /* Casting away the const for the 3rd parameter that the LDAP API expects as
     a non-const char ** is potentially unsafe but we believe the lack of
     const in the API was an oversight and that no LDAP implementation
     actually modifies the input. */
  rc = ldap_ಹುಡುಕು_ext(li->ld, "", LDAP_SCOPE_BASE, "(objectclass=*)",
                       (ವರ್ಣ **)CURL_UNCONST(supportedSASLMechanisms), 0,
                       ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, 0, &li->msgid);
  ಯದಿ(rc != LDAP_SUCCESS)
    ಪ್ರತ್ಯಾಗಮ oldap_map_error(rc, CURLE_LOGIN_DENIED);
  oldap_state(data, li, OLDAP_MECHS);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Starts SASL bind. */
ಸ್ಥಿರ CURLcode oldap_perform_sasl(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  saslprogress progress = SASL_IDLE;
  CURLcode result;

  ಯದಿ(!li)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  result = Curl_sasl_start(&li->sasl, data, TRUE, &progress);

  oldap_state(data, li, OLDAP_SASL);
  ಯದಿ(!result && progress != SASL_INPROGRESS)
    result = Curl_sasl_is_blocked(&li->sasl, data);
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef USE_SSL
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ldapsb_tls_setup(Sockbuf_IO_Desc *sbiod, ಶೂನ್ಯ *arg)
{
  sbiod->sbiod_pvt = arg;
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ldapsb_tls_remove(Sockbuf_IO_Desc *sbiod)
{
  sbiod->sbiod_pvt = ರಿಕ್ತ;
  ಪ್ರತ್ಯಾಗಮ 0;
}

/* if there is a duplicated socket to close, close it */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ldapsb_tls_close(Sockbuf_IO_Desc *sbiod)
{
  ಸಂರಚನಾ Curl_easy *data = sbiod->sbiod_pvt;
  ಯದಿ(data) {
    ಸಂರಚನಾ connectdata *conn = data->conn;
    ಯದಿ(conn) {
      ಸಂರಚನಾ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      ಯದಿ(li && (li->dupfd != CURL_SOCKET_BAD)) {
        sclose(li->dupfd);
        li->dupfd = CURL_SOCKET_BAD;
      }
    }
  }

  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ldapsb_tls_ctrl(Sockbuf_IO_Desc *sbiod, ಪೂರ್ಣಾಙ್ಕ opt, ಶೂನ್ಯ *arg)
{
  (ಶೂನ್ಯ)arg;
  ಯದಿ(opt == LBER_SB_OPT_DATA_READY) {
    ಸಂರಚನಾ Curl_easy *data = sbiod->sbiod_pvt;
    ಪ್ರತ್ಯಾಗಮ Curl_conn_data_pending(data, FIRSTSOCKET);
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ber_slen_t ldapsb_tls_read(Sockbuf_IO_Desc *sbiod, ಶೂನ್ಯ *buf,
                                  ber_len_t len)
{
  ಸಂರಚನಾ Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  ಯದಿ(data) {
    ಸಂರಚನಾ connectdata *conn = data->conn;
    ಯದಿ(conn) {
      ಸಂರಚನಾ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_RECV_ERROR;
      size_t nread;

      ಯದಿ(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        ಪ್ರತ್ಯಾಗಮ -1;
      }
      result = (li->recv)(data, FIRSTSOCKET, buf, len, &nread);
      ಯದಿ(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nread;
    }
  }
  ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ber_slen_t ldapsb_tls_write(Sockbuf_IO_Desc *sbiod, ಶೂನ್ಯ *buf,
                                   ber_len_t len)
{
  ಸಂರಚನಾ Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  ಯದಿ(data) {
    ಸಂರಚನಾ connectdata *conn = data->conn;
    ಯದಿ(conn) {
      ಸಂರಚನಾ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_SEND_ERROR;
      size_t nwritten;

      ಯದಿ(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        ಪ್ರತ್ಯಾಗಮ -1;
      }
      result = (li->send)(data, FIRSTSOCKET, buf, len, FALSE, &nwritten);
      ಯದಿ(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nwritten;
    }
  }
  ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ Sockbuf_IO ldapsb_tls = {
  ldapsb_tls_setup,
  ldapsb_tls_remove,
  ldapsb_tls_ctrl,
  ldapsb_tls_read,
  ldapsb_tls_write,
  ldapsb_tls_close
};

ಸ್ಥಿರ bool ssl_installed(ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ಪ್ರತ್ಯಾಗಮ li && li->recv;
}

ಸ್ಥಿರ CURLcode oldap_ssl_connect(ಸಂರಚನಾ Curl_easy *data, ldapstate newstate)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  bool ssldone = FALSE;
  CURLcode result;

  ಯದಿ(!li)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  oldap_state(data, li, newstate);

  ಯದಿ(ssldone) {
    Sockbuf *sb;

    /* Install the libcurl SSL handlers into the sockbuf. */
    ಯದಿ((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
    li->recv = conn->recv[FIRSTSOCKET];
    li->send = conn->send[FIRSTSOCKET];
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* Send the STARTTLS request */
ಸ್ಥಿರ CURLcode oldap_perform_starttls(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  ಪೂರ್ಣಾಙ್ಕ rc;

  ಯದಿ(!li)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  rc = ldap_start_tls(li->ld, ರಿಕ್ತ, ರಿಕ್ತ, &li->msgid);
  ಯದಿ(rc != LDAP_SUCCESS)
    ಪ್ರತ್ಯಾಗಮ oldap_map_error(rc, CURLE_USE_SSL_FAILED);
  oldap_state(data, li, OLDAP_STARTTLS);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif

ಸ್ಥಿರ ಶೂನ್ಯ oldap_easy_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ ldapreqinfo *lr = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  curlx_free(lr);
}

ಸ್ಥಿರ ಶೂನ್ಯ oldap_conn_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ ldapconninfo *li = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  ಯದಿ(li->ld) {
    ldap_unbind_ext(li->ld, ರಿಕ್ತ, ರಿಕ್ತ);
    li->ld = ರಿಕ್ತ;
  }
  curlx_free(li);
}

/* SASL parameters for the ldap protocol */
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ SASLproto saslldap = {
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

ಸ್ಥಿರ CURLcode oldap_connect(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ldapconninfo *li;
  ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ version = LDAP_VERSION3;
  ವರ್ಣ *hosturl = ರಿಕ್ತ;
  CURLcode result;
  ಪೂರ್ಣಾಙ್ಕ rc;
#ifdef CURL_OPENLDAP_DEBUG
  ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ do_trace = -1;
#endif

  (ಶೂನ್ಯ)done;

  li = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ ldapconninfo));
  ಯದಿ(!li) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  li->dupfd = CURL_SOCKET_BAD;

  result = Curl_conn_meta_set(conn, CURL_META_LDAP_CONN, li, oldap_conn_dtor);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  li->proto = ldap_pvt_url_scheme2proto(data->state.origin->scheme->name);

  /* Initialize the SASL storage */
  Curl_sasl_init(&li->sasl, data, &saslldap);

  result = oldap_parse_login_options(conn);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  hosturl = curl_maprintf("%s://%s:%u",
                          conn->scheme->name,
                          conn->origin->ipv6 ?
                          conn->origin->user_hostname :
                          conn->origin->hostname,
                          conn->origin->port);
  ಯದಿ(!hosturl) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  {
#ifdef _WIN32
    SOCKET dupfd = INVALID_SOCKET;
    WSAPROTOCOL_INFO pi;
    ಯದಿ(WSADuplicateSocket(conn->sock[FIRSTSOCKET], GetCurrentProcessId(),
                          &pi))
      ; /* error */
    ಅನ್ಯಥಾ
      dupfd = CURL_SOCKET(pi.iAddressFamily, pi.iSocketType, pi.iProtocol);
#ಅನ್ಯಥಾ
#ifdef F_DUPFD_CLOEXEC
    ಪೂರ್ಣಾಙ್ಕ dupfd = fcntl(conn->sock[FIRSTSOCKET], F_DUPFD_CLOEXEC, 0);
#ಅನ್ಯಥಾ
    ಪೂರ್ಣಾಙ್ಕ dupfd = dup(conn->sock[FIRSTSOCKET]);
#ifdef HAVE_FCNTL
    ಯದಿ((dupfd != CURL_SOCKET_BAD) &&
       (fcntl(dupfd, F_SETFD, FD_CLOEXEC) < 0)) {
      sclose(dupfd);
      dupfd = CURL_SOCKET_BAD;
    }
#endif /* HAVE_FCNTL */
#endif /* F_DUPFD_CLOEXEC */
#endif /* _WIN32 */

    ಯದಿ(dupfd == CURL_SOCKET_BAD) {
      result = CURLE_COULDNT_CONNECT;
      ಗಚ್ಛ out;
    }
    rc = ldap_init_fd((ber_socket_t)dupfd, li->proto, hosturl, &li->ld);
    ಯದಿ(rc)
      sclose(dupfd);
    ಅನ್ಯಥಾ
      /* store the socket, as when this is used by TLS we close this ourselves
         and at this point the code does not yet know if this will use TLS or
         not */
      li->dupfd = dupfd;
  }
  ಯದಿ(rc) {
    failf(data, "LDAP local: Cannot connect to %s, %s",
          hosturl, ldap_err2string(rc));
    result = CURLE_COULDNT_CONNECT;
    ಗಚ್ಛ out;
  }

#ifdef CURL_OPENLDAP_DEBUG
  ಯದಿ(do_trace < 0) {
    ಧ್ರುವ ವರ್ಣ *env = getenv("CURL_OPENLDAP_TRACE");
    curl_off_t e = 0;
    ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  ಯದಿ(do_trace)
    ldap_set_option(li->ld, LDAP_OPT_DEBUG_LEVEL, &do_trace);
#endif

  /* Try version 3 first. */
  ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);

  /* Do not chase referrals. */
  ldap_set_option(li->ld, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  {
    ber_len_t max = 256 * 1024;
    Sockbuf *sb;
    ಯದಿ((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       /* Set the maximum allowed size of an incoming message, which to
          OpenLDAP means that it will malloc() memory up to this size. If not
          set, there is no limit and we instead risk a malloc() failure. */
       !ber_sockbuf_ctrl(sb, LBER_SB_OPT_SET_MAX_INCOMING, &max)) {
      result = CURLE_FAILED_INIT;
      ಗಚ್ಛ out;
    }
  }

#ifdef USE_SSL
  ಯದಿ(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = oldap_ssl_connect(data, OLDAP_SSL);
    ಗಚ್ಛ out;
  }

  ಯದಿ(data->set.use_ssl) {
    result = oldap_perform_starttls(data);
    ಯದಿ(!result || data->set.use_ssl != CURLUSESSL_TRY)
      ಗಚ್ಛ out;
  }
#endif

  ಯದಿ(li->sasl.prefmech != SASL_AUTH_NONE) {
    result = oldap_perform_mechs(data);
    ಗಚ್ಛ out;
  }

  /* Force bind even if anonymous bind is not needed in protocol version 3
     to detect missing version 3 support. */
  result = oldap_perform_bind(data, OLDAP_BIND);

out:
  curlx_free(hosturl);
  ಪ್ರತ್ಯಾಗಮ result;
}

/* Handle the supported SASL mechanisms query response */
ಸ್ಥಿರ CURLcode oldap_state_mechs_resp(ಸಂರಚನಾ Curl_easy *data,
                                       LDAPMessage *ಸಂದೇಶ, ಪೂರ್ಣಾಙ್ಕ code)
{
  ಸಂರಚನಾ connectdata *conn;
  ಸಂರಚನಾ ldapconninfo *li;
  ಪೂರ್ಣಾಙ್ಕ rc;
  BerElement *ber = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ berval bv, *bvals;

  ಯದಿ(!data)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  conn = data->conn;
  li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);

  ಯದಿ(!li)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ವಿಕಲ್ಪ(ldap_msgtype(ಸಂದೇಶ)) {
  ಪಕ್ಷ LDAP_RES_ಹುಡುಕು_ENTRY:
    /* Got a list of supported SASL mechanisms. */
    ಯದಿ(code != LDAP_SUCCESS && code != LDAP_NO_RESULTS_RETURNED)
      ಪ್ರತ್ಯಾಗಮ CURLE_LOGIN_DENIED;

    rc = ldap_get_dn_ber(li->ld, ಸಂದೇಶ, &ber, &bv);
    ಯದಿ(rc < 0)
      ಪ್ರತ್ಯಾಗಮ oldap_map_error(rc, CURLE_BAD_CONTENT_ENCODING);
    ಪ್ರತಿ(rc = ldap_get_attribute_ber(li->ld, ಸಂದೇಶ, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, ಸಂದೇಶ, ber, &bv, &bvals)) {
      ಪೂರ್ಣಾಙ್ಕ i;

      ಯದಿ(!bv.bv_val)
        ಭಙ್ಗ;

      ಯದಿ(bvals) {
        ಪ್ರತಿ(i = 0; bvals[i].bv_val; i++) {
          size_t llen;
          unsigned short mech =
            Curl_sasl_decode_mech((ಧ್ರುವ ವರ್ಣ *)bvals[i].bv_val,
                                  bvals[i].bv_len, &llen);
          ಯದಿ(bvals[i].bv_len == llen)
            li->sasl.authmechs |= mech;
        }
        ber_memfree(bvals);
      }
    }
    ber_free(ber, 0);
    ಭಙ್ಗ;

  ಪಕ್ಷ LDAP_RES_ಹುಡುಕು_RESULT:
    ವಿಕಲ್ಪ(code) {
    ಪಕ್ಷ LDAP_SIZELIMIT_EXCEEDED:
      infof(data, "Too many authentication mechanisms");
      FALLTHROUGH();
    ಪಕ್ಷ LDAP_SUCCESS:
    ಪಕ್ಷ LDAP_NO_RESULTS_RETURNED:
      ಯದಿ(Curl_sasl_can_authenticate(&li->sasl, data))
        result = oldap_perform_sasl(data);
      ಅನ್ಯಥಾ
        result = CURLE_LOGIN_DENIED;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      result = oldap_map_error(code, CURLE_LOGIN_DENIED);
      ಭಙ್ಗ;
    }
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/* Handle a SASL bind response. */
ಸ್ಥಿರ CURLcode oldap_state_sasl_resp(ಸಂರಚನಾ Curl_easy *data,
                                      LDAPMessage *ಸಂದೇಶ, ಪೂರ್ಣಾಙ್ಕ code)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode result = CURLE_OK;
  saslprogress progress;
  ಪೂರ್ಣಾಙ್ಕ rc;

  ಯದಿ(!li)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  li->servercred = ರಿಕ್ತ;
  rc = ldap_parse_sasl_bind_result(li->ld, ಸಂದೇಶ, &li->servercred, 0);
  ಯದಿ(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: sasl ldap_parse_result %s", ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LOGIN_DENIED);
  }
  ಅನ್ಯಥಾ {
    result = Curl_sasl_continue(&li->sasl, data, code, &progress);
    ಯದಿ(!result) {
      ವಿಕಲ್ಪ(progress) {
      ಪಕ್ಷ SASL_DONE:
        oldap_state(data, li, OLDAP_STOP);   /* Authenticated */
        ಭಙ್ಗ;
      ಪಕ್ಷ SASL_IDLE:            /* No mechanism left after cancellation */
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
        ಭಙ್ಗ;
      ಸ್ವಭಾವ:
        ಭಙ್ಗ;
      }
    }
  }

  ಯದಿ(li->servercred)
    ber_bvfree(li->servercred);
  ಪ್ರತ್ಯಾಗಮ result;
}

/* Handle a simple bind response. */
ಸ್ಥಿರ CURLcode oldap_state_bind_resp(ಸಂರಚನಾ Curl_easy *data, LDAPMessage *ಸಂದೇಶ,
                                      ಪೂರ್ಣಾಙ್ಕ code)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ berval *bv = ರಿಕ್ತ;
  ಪೂರ್ಣಾಙ್ಕ rc;

  ಯದಿ(!li)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯದಿ(code != LDAP_SUCCESS)
    ಪ್ರತ್ಯಾಗಮ oldap_map_error(code, CURLE_LDAP_CANNOT_BIND);

  rc = ldap_parse_sasl_bind_result(li->ld, ಸಂದೇಶ, &bv, 0);
  ಯದಿ(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: bind ldap_parse_sasl_bind_result %s",
          ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  }
  ಅನ್ಯಥಾ
    oldap_state(data, li, OLDAP_STOP);

  ಯದಿ(bv)
    ber_bvfree(bv);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode oldap_connecting(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  LDAPMessage *ಸಂದೇಶ = ರಿಕ್ತ;
  ಸಂರಚನಾ timeval tv = { 0, 0 };
  ಪೂರ್ಣಾಙ್ಕ code = LDAP_SUCCESS;
  ಪೂರ್ಣಾಙ್ಕ rc;

  ಯದಿ(!li)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯದಿ(li->state != OLDAP_SSL && li->state != OLDAP_TLS) {
    /* Get response to last command. */
    rc = ldap_result(li->ld, li->msgid, LDAP_ಸಂದೇಶ_ONE, &tv, &ಸಂದೇಶ);
    ವಿಕಲ್ಪ(rc) {
    ಪಕ್ಷ 0:                               /* Timed out. */
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    ಪಕ್ಷ LDAP_RES_ಹುಡುಕು_ENTRY:
    ಪಕ್ಷ LDAP_RES_ಹುಡುಕು_REFERENCE:
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      li->msgid = 0;                      /* Nothing to abandon upon error. */
      ಯದಿ(rc < 0) {
        failf(data, "LDAP local: connecting ldap_result %s",
              ldap_err2string(rc));
        ಪ್ರತ್ಯಾಗಮ oldap_map_error(rc, CURLE_COULDNT_CONNECT);
      }
      ಭಙ್ಗ;
    }

    /* Get error code from message. */
    rc = ldap_parse_result(li->ld, ಸಂದೇಶ, &code, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, 0);
    ಯದಿ(rc)
      code = rc;
    ಅನ್ಯಥಾ {
      /* store the latest code for later retrieval */
      data->info.httpcode = code;
    }

    /* If protocol version 3 is not supported, fallback to version 2. */
    ಯದಿ(code == LDAP_PROTOCOL_ERROR && li->state != OLDAP_BINDV2 &&
#ifdef USE_SSL
       (ssl_installed(conn) || data->set.use_ssl <= CURLUSESSL_TRY) &&
#endif
       li->sasl.prefmech == SASL_AUTH_NONE) {
      ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ version = LDAP_VERSION2;

      ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);
      ldap_msgfree(ಸಂದೇಶ);
      ಪ್ರತ್ಯಾಗಮ oldap_perform_bind(data, OLDAP_BINDV2);
    }
  }

  /* Handle response message according to current state. */
  ವಿಕಲ್ಪ(li->state) {

#ifdef USE_SSL
  ಪಕ್ಷ OLDAP_SSL:
    result = oldap_ssl_connect(data, OLDAP_SSL);
    ಯದಿ(!result && ssl_installed(conn)) {
      ಯದಿ(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      ಅನ್ಯಥಾ
        result = oldap_perform_bind(data, OLDAP_BIND);
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ OLDAP_STARTTLS:
    ಯದಿ(code != LDAP_SUCCESS) {
      ಯದಿ(data->set.use_ssl != CURLUSESSL_TRY)
        result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
      ಅನ್ಯಥಾ ಯದಿ(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      ಅನ್ಯಥಾ
        result = oldap_perform_bind(data, OLDAP_BIND);
      ಭಙ್ಗ;
    }
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ಯದಿ(result)
      ಭಙ್ಗ;
    FALLTHROUGH();
  ಪಕ್ಷ OLDAP_TLS:
    result = oldap_ssl_connect(data, OLDAP_TLS);
    ಯದಿ(result)
      result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
    ಅನ್ಯಥಾ ಯದಿ(ssl_installed(conn)) {
      ಯದಿ(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      ಅನ್ಯಥಾ ಯದಿ(data->state.creds)
        result = oldap_perform_bind(data, OLDAP_BIND);
      ಅನ್ಯಥಾ {
        /* Version 3 supported: no bind required */
        oldap_state(data, li, OLDAP_STOP);
        result = CURLE_OK;
      }
    }
    ಭಙ್ಗ;
#endif

  ಪಕ್ಷ OLDAP_MECHS:
    result = oldap_state_mechs_resp(data, ಸಂದೇಶ, code);
    ಭಙ್ಗ;
  ಪಕ್ಷ OLDAP_SASL:
    result = oldap_state_sasl_resp(data, ಸಂದೇಶ, code);
    ಭಙ್ಗ;
  ಪಕ್ಷ OLDAP_BIND:
  ಪಕ್ಷ OLDAP_BINDV2:
    result = oldap_state_bind_resp(data, ಸಂದೇಶ, code);
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    /* internal error */
    result = CURLE_COULDNT_CONNECT;
    ಭಙ್ಗ;
  }

  ldap_msgfree(ಸಂದೇಶ);

  *done = li->state == OLDAP_STOP;
  ಯದಿ(*done)
    conn->recv[FIRSTSOCKET] = oldap_recv;

  ಯದಿ(result && li->msgid) {
    ldap_abandon_ext(li->ld, li->msgid, ರಿಕ್ತ, ರಿಕ್ತ);
    li->msgid = 0;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode oldap_disconnect(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ connectdata *conn,
                                 bool dead_connection)
{
  ಸಂರಚನಾ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  (ಶೂನ್ಯ)dead_connection;
#ifndef USE_SSL
  (ಶೂನ್ಯ)data;
#endif

  ಯದಿ(li && li->ld) {
#ifdef USE_SSL
    ಯದಿ(ssl_installed(conn)) {
      Sockbuf *sb;
      ಯದಿ(ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS ||
         ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
        ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
    }
#endif
    ldap_unbind_ext(li->ld, ರಿಕ್ತ, ರಿಕ್ತ);
    li->ld = ರಿಕ್ತ;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode oldap_do(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ಸಂರಚನಾ ldapreqinfo *lr;
  CURLcode result;
  ಪೂರ್ಣಾಙ್ಕ rc;
  LDAPURLDesc *lud;
  ಪೂರ್ಣಾಙ್ಕ msgid;

  ಯದಿ(!li)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

  result = oldap_url_parse(data, &lud);
  ಯದಿ(result)
    ಗಚ್ಛ out;

#ifdef USE_SSL
  ಯದಿ(ssl_installed(conn)) {
    Sockbuf *sb;
    /* re-install the libcurl SSL handlers into the sockbuf. */
    ಯದಿ((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data)) {
      ldap_free_urldesc(lud);
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
    }
  }
#endif

  rc = ldap_ಹುಡುಕು_ext(li->ld, lud->lud_dn, lud->lud_scope,
                       lud->lud_filter, lud->lud_attrs, 0,
                       ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, 0, &msgid);
  ldap_free_urldesc(lud);
  ಯದಿ(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: ldap_search_ext %s", ldap_err2string(rc));
    result = CURLE_LDAP_ಹುಡುಕು_FAILED;
    ಗಚ್ಛ out;
  }

  lr = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ ldapreqinfo));
  ಯದಿ(!lr ||
     Curl_meta_set(data, CURL_META_LDAP_EASY, lr, oldap_easy_dtor)) {
    ldap_abandon_ext(li->ld, msgid, ರಿಕ್ತ, ರಿಕ್ತ);
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  lr->msgid = msgid;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  *done = TRUE;

out:
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode oldap_done(ಸಂರಚನಾ Curl_easy *data, CURLcode res,
                           bool premature)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);

  (ಶೂನ್ಯ)res;
  (ಶೂನ್ಯ)premature;

  ಯದಿ(lr) {
    /* if there was a search in progress, abandon it */
    ಯದಿ(lr->msgid) {
      ಸಂರಚನಾ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      ಯದಿ(li && li->ld) {
        ldap_abandon_ext(li->ld, lr->msgid, ರಿಕ್ತ, ರಿಕ್ತ);
      }
      lr->msgid = 0;
    }
    Curl_meta_remove(data, CURL_META_LDAP_EASY);
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode client_write(ಸಂರಚನಾ Curl_easy *data,
                             ಧ್ರುವ ವರ್ಣ *prefix, size_t plen,
                             ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, size_t len,
                             ಧ್ರುವ ವರ್ಣ *suffix, size_t slen)
{
  CURLcode result = CURLE_OK;

  ಯದಿ(prefix) {
    /* If we have a zero-length value and the prefix ends with a space
       separator, drop the latter. */
    ಯದಿ(!len && plen && prefix[plen - 1] == ' ')
      plen--;
    result = Curl_client_write(data, CLIENTWRITE_BODY, prefix, plen);
  }
  ಯದಿ(!result && ಮೌಲ್ಯ) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, ಮೌಲ್ಯ, len);
  }
  ಯದಿ(!result && suffix) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, suffix, slen);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
ಸ್ಥಿರ bool oldap_str_needs_base64(ಧ್ರುವ ವರ್ಣ *s, size_t len)
{
  size_t i;
  ಯದಿ(!len)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಯದಿ((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    ಪ್ರತ್ಯಾಗಮ TRUE;
  ಪ್ರತಿ(i = 0; i < len; i++)
    ಯದಿ(!ISPRINT(s[i]))
      ಪ್ರತ್ಯಾಗಮ TRUE;
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ CURLcode oldap_recv(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex, ವರ್ಣ *buf,
                           size_t len, size_t *pnread)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ಸಂರಚನಾ ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);
  ಪೂರ್ಣಾಙ್ಕ rc;
  LDAPMessage *ಸಂದೇಶ = ರಿಕ್ತ;
  BerElement *ber = ರಿಕ್ತ;
  ಸಂರಚನಾ timeval tv = { 0, 0 };
  ಸಂರಚನಾ berval bv, *bvals;
  CURLcode result = CURLE_AGAIN;
  ಪೂರ್ಣಾಙ್ಕ code;
  ವರ್ಣ *info = ರಿಕ್ತ;

  (ಶೂನ್ಯ)len;
  (ಶೂನ್ಯ)buf;
  (ಶೂನ್ಯ)sockindex;
  *pnread = 0;
  ಯದಿ(!li || !lr)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  rc = ldap_result(li->ld, lr->msgid, LDAP_ಸಂದೇಶ_ONE, &tv, &ಸಂದೇಶ);
  ಯದಿ(rc < 0) {
    failf(data, "LDAP local: search ldap_result %s", ldap_err2string(rc));
    result = CURLE_RECV_ERROR;
  }

  /* error or timed out */
  ಯದಿ(!ಸಂದೇಶ)
    ಪ್ರತ್ಯಾಗಮ result;

  result = CURLE_OK;

  ವಿಕಲ್ಪ(ldap_msgtype(ಸಂದೇಶ)) {
  ಪಕ್ಷ LDAP_RES_ಹುಡುಕು_RESULT:
    lr->msgid = 0;
    rc = ldap_parse_result(li->ld, ಸಂದೇಶ, &code, ರಿಕ್ತ, &info, ರಿಕ್ತ, ರಿಕ್ತ, 0);
    ಯದಿ(rc) {
      failf(data, "LDAP local: search ldap_parse_result %s",
            ldap_err2string(rc));
      result = CURLE_LDAP_ಹುಡುಕು_FAILED;
      ಭಙ್ಗ;
    }

    /* store the latest code for later retrieval */
    data->info.httpcode = code;

    ವಿಕಲ್ಪ(code) {
    ಪಕ್ಷ LDAP_SIZELIMIT_EXCEEDED:
      infof(data, "There are more than %d entries", lr->nument);
      FALLTHROUGH();
    ಪಕ್ಷ LDAP_SUCCESS:
      data->req.size = data->req.bytecount;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      failf(data, "LDAP remote: search failed %s %s", ldap_err2string(code),
            info ? info : "");
      result = CURLE_LDAP_ಹುಡುಕು_FAILED;
      ಭಙ್ಗ;
    }
    ಯದಿ(info)
      ldap_memfree(info);
    ಭಙ್ಗ;
  ಪಕ್ಷ LDAP_RES_ಹುಡುಕು_ENTRY:
    lr->nument++;
    rc = ldap_get_dn_ber(li->ld, ಸಂದೇಶ, &ber, &bv);
    ಯದಿ(rc < 0) {
      result = CURLE_RECV_ERROR;
      ಭಙ್ಗ;
    }

    ಯದಿ(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
      ವರ್ಣ *dn_b64 = ರಿಕ್ತ;
      size_t dn_b64_sz = 0;

      /* A DN with e.g. CR/LF would inject LDIF lines, so base64-encode it. */
      result = curlx_base64_encode((uint8_t *)bv.bv_val, bv.bv_len,
                                   &dn_b64, &dn_b64_sz);
      ಯದಿ(!result)
        result = client_write(data, STRCONST("DN:: "), dn_b64, dn_b64_sz,
                              STRCONST("\n"));
      curlx_free(dn_b64);
    }
    ಅನ್ಯಥಾ
      result = client_write(data, STRCONST("DN: "), bv.bv_val, bv.bv_len,
                            STRCONST("\n"));
    ಯದಿ(result)
      ಭಙ್ಗ;

    ಪ್ರತಿ(rc = ldap_get_attribute_ber(li->ld, ಸಂದೇಶ, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, ಸಂದೇಶ, ber, &bv, &bvals)) {
      ಪೂರ್ಣಾಙ್ಕ i;
      bool ದ್ವಿಮಾನ;

      ಯದಿ(!bv.bv_val)
        ಭಙ್ಗ;

      /* An attribute type has no base64 form in LDIF, so a type carrying
         control bytes (CR/LF etc.) would let a hostile server inject lines.
         A conformant type name never contains them, so reject it. */
      ಯದಿ(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
        failf(data, "LDAP local: attribute type contains illegal bytes");
        ಯದಿ(bvals)
          ber_memfree(bvals);
        result = CURLE_WEIRD_SERVER_REPLY;
        ಭಙ್ಗ;
      }

      ಯದಿ(!bvals) {
        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":\n"));
        ಯದಿ(result)
          ಭಙ್ಗ;
        ಅನುವರ್ತ;
      }

      ದ್ವಿಮಾನ = bv.bv_len > 7 &&
        curl_strnequal(bv.bv_val + bv.bv_len - 7, ";binary", 7);

      ಪ್ರತಿ(i = 0; bvals[i].bv_val; i++) {
        bool binval = FALSE;

        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":"));
        ಯದಿ(result)
          ಭಙ್ಗ;

        ಯದಿ(!ದ್ವಿಮಾನ)
          binval = oldap_str_needs_base64(bvals[i].bv_val, bvals[i].bv_len);
        ಯದಿ(ದ್ವಿಮಾನ || binval) {
          ವರ್ಣ *val_b64 = ರಿಕ್ತ;
          size_t val_b64_sz = 0;

          /* Binary value, encode to base64. */
          ಯದಿ(bvals[i].bv_len)
            result = curlx_base64_encode((uint8_t *)bvals[i].bv_val,
                                         bvals[i].bv_len,
                                         &val_b64, &val_b64_sz);
          ಯದಿ(!result)
            result = client_write(data, STRCONST(": "), val_b64, val_b64_sz,
                                  STRCONST("\n"));
          curlx_free(val_b64);
        }
        ಅನ್ಯಥಾ
          result = client_write(data, STRCONST(" "),
                                bvals[i].bv_val, bvals[i].bv_len,
                                STRCONST("\n"));
        ಯದಿ(result)
          ಭಙ್ಗ;
      }

      ber_memfree(bvals);
      bvals = ರಿಕ್ತ;
      ಯದಿ(!result)
        result = client_write(data, STRCONST("\n"), ರಿಕ್ತ, 0, ರಿಕ್ತ, 0);
      ಯದಿ(result)
        ಭಙ್ಗ;
    }

    ಯದಿ(!result)
      result = client_write(data, STRCONST("\n"), ರಿಕ್ತ, 0, ರಿಕ್ತ, 0);
    ಯದಿ(!result)
      result = CURLE_AGAIN;
    ಭಙ್ಗ;
  }

  ber_free(ber, 0);
  ldap_msgfree(ಸಂದೇಶ);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ Curl_ldap_version(ವರ್ಣ *buf, size_t bufsz)
{
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  ಯದಿ(ldap_get_option(ರಿಕ್ತ, LDAP_OPT_API_INFO, &api) == LDAP_OPT_SUCCESS) {
    unsigned ಪೂರ್ಣಾಙ್ಕ patch = (unsigned ಪೂರ್ಣಾಙ್ಕ)(api.ldapai_vendor_version % 100);
    unsigned ಪೂರ್ಣಾಙ್ಕ major = (unsigned ಪೂರ್ಣಾಙ್ಕ)(api.ldapai_vendor_version / 10000);
    unsigned ಪೂರ್ಣಾಙ್ಕ minor =
      (((unsigned ಪೂರ್ಣಾಙ್ಕ)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u",
                   api.ldapai_vendor_name, major, minor, patch);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((ಶೂನ್ಯ **)api.ldapai_extensions);
  }
  ಅನ್ಯಥಾ
    curl_msnprintf(buf, bufsz, "OpenLDAP");
}

/*
 * LDAP protocol handler.
 */
ಧ್ರುವ ಸಂರಚನಾ Curl_protocol Curl_protocol_ldap = {
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
