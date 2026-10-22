/***************************************************************************
 *                      _   _ ____  _
 *  Project         ___| | | |  _ \| |
 *                 / __| | | | |_) | |
 *                | (__| |_| |  _ <| |___
 *                 \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
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
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_ldap.h"

#ಯದಿ !defined(CURL_DISABLE_LDAP) && !defined(USE_OPENLDAP)

#ಯದಿ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ತಳ್ಳು
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

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

#ifdef USE_WIN32_LDAP           /* Use Windows LDAP implementation. */
#  include <winldap.h>
/* Undefine indirect <wincrypt.h> symbols conflicting with AWS-LC/BoringSSL. */
#  undef X509_NAME
#  undef X509_EXTENSIONS
#  undef PKCS7_ISSUER_AND_SERIAL
#  undef PKCS7_SIGNER_INFO
#  undef OCSP_REQUEST
#  undef OCSP_RESPONSE
#  include <winber.h>
#ಅನ್ಯಥಾ
#  define LDAP_DEPRECATED 1     /* Be sure ldap_init() is defined. */
#  ifdef HAVE_LBER_H
#    include <lber.h>
#  endif
#  include <ldap.h>
#  ಯದಿ defined(HAVE_LDAP_SSL) && defined(HAVE_LDAP_SSL_H)
#    include <ldap_ssl.h>
#  endif /* HAVE_LDAP_SSL && HAVE_LDAP_SSL_H */
#endif

#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "curlx/multibyte.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#ಅನ್ತರ್ಭಾವ "connect.h"

#ifdef USE_WIN32_LDAP
#define FREE_ON_WINLDAP(x) curlx_free(x)
#define curl_ldap_ಅಂಕೆ_t    ULONG
#ಅನ್ಯಥಾ
#define FREE_ON_WINLDAP(x) ಕುರು {} ಯಾವತ್(0)
#define curl_ldap_ಅಂಕೆ_t    ಪೂರ್ಣಾಙ್ಕ
#endif

#ifndef HAVE_LDAP_URL_PARSE

/* Use our own implementation. */

ಸಂರಚನಾ ldap_urldesc {
  ವರ್ಣ   *lud_host;
  ಪೂರ್ಣಾಙ್ಕ     lud_port;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_dn;
  TCHAR **lud_attrs;
#ಅನ್ಯಥಾ
  ವರ್ಣ   *lud_dn;
  ವರ್ಣ  **lud_attrs;
#endif
  ಪೂರ್ಣಾಙ್ಕ     lud_scope;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_filter;
#ಅನ್ಯಥಾ
  ವರ್ಣ   *lud_filter;
#endif
  ವರ್ಣ  **lud_exts;
  size_t  lud_attrs_dups; /* how many were dup'ed, this field is not in the
                             "real" struct so can only be used in code
                             without HAVE_LDAP_URL_PARSE defined */
};

#undef LDAPURLDesc
#define LDAPURLDesc ಸಂರಚನಾ ldap_urldesc

ಸ್ಥಿರ curl_ldap_ಅಂಕೆ_t ldap_url_parse_ಕೆಳಗಿನ(ಸಂರಚನಾ Curl_easy *data,
                                          ಧ್ರುವ ಸಂರಚನಾ connectdata *conn,
                                          LDAPURLDesc **ludpp);
ಸ್ಥಿರ ಶೂನ್ಯ ldap_free_urldesc_ಕೆಳಗಿನ(LDAPURLDesc *ludp);

#undef ldap_free_urldesc
#define ldap_free_urldesc       ldap_free_urldesc_ಕೆಳಗಿನ

#endif /* !HAVE_LDAP_URL_PARSE */

#ifdef DEBUG_LDAP
#define LDAP_TRACE(x)                 \
  ಕುರು {                                \
    ldap_trace_ಕೆಳಗಿನ("%u: ", __LINE__); \
    ldap_trace_ಕೆಳಗಿನ x;                 \
  } ಯಾವತ್(0)

ಸ್ಥಿರ ಶೂನ್ಯ ldap_trace_ಕೆಳಗಿನ(ಧ್ರುವ ವರ್ಣ *fmt, ...) CURL_PRINTF(1, 2);
#ಅನ್ಯಥಾ
#define LDAP_TRACE(x) Curl_nop_stmt
#endif

#ಯದಿ defined(USE_WIN32_LDAP) && defined(ldap_err2string)
/* Use ANSI error strings in Unicode builds */
#undef ldap_err2string
#define ldap_err2string ldap_err2stringA
#endif

#ಯದಿ defined(USE_WIN32_LDAP) && defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'void *' of greater size */
#undef LDAP_OPT_ON
#undef LDAP_OPT_OFF
#define LDAP_OPT_ON   ((ಶೂನ್ಯ *)(size_t)1)
#define LDAP_OPT_OFF  ((ಶೂನ್ಯ *)(size_t)0)
#endif

#ifdef USE_WIN32_LDAP

#ifdef USE_WINDOWS_SSPI
ಸ್ಥಿರ ULONG ldap_win_bind_auth(LDAP *server, ಧ್ರುವ ವರ್ಣ *user,
                                ಧ್ರುವ ವರ್ಣ *passwd, unsigned long authflags)
{
  ULONG method = 0;
  SEC_WINNT_AUTH_IDENTITY_EX cred;
  ULONG rc = LDAP_AUTH_METHOD_NOT_SUPPORTED;

  memset(&cred, 0, ಪರಿಮಾಣ(cred));

#ifdef USE_SPNEGO
  ಯದಿ(authflags & CURLAUTH_NEGOTIATE) {
    method = LDAP_AUTH_NEGOTIATE;
  }
  ಅನ್ಯಥಾ
#endif
#ifdef USE_NTLM
  ಯದಿ(authflags & CURLAUTH_NTLM) {
    method = LDAP_AUTH_NTLM;
  }
  ಅನ್ಯಥಾ
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  ಯದಿ(authflags & CURLAUTH_DIGEST) {
    method = LDAP_AUTH_DIGEST;
  }
  ಅನ್ಯಥಾ
#endif
  {
    /* required anyway if one of upper preprocessor definitions enabled */
  }

  ಯದಿ(method && user && passwd) {
    CURLcode result = Curl_create_sspi_identity(user, passwd, &cred);
    ಯದಿ(!result) {
      rc = ldap_bind_s(server, ರಿಕ್ತ, (TCHAR *)&cred, method);
      Curl_sspi_free_identity(&cred);
    }
    ಅನ್ಯಥಾ {
      rc = LDAP_NO_MEMORY;
    }
  }
  ಅನ್ಯಥಾ {
    /* proceed with current user credentials */
    method = LDAP_AUTH_NEGOTIATE;
    rc = ldap_bind_s(server, ರಿಕ್ತ, ರಿಕ್ತ, method);
  }
  ಪ್ರತ್ಯಾಗಮ rc;
}
#endif /* USE_WINDOWS_SSPI */

ಸ್ಥಿರ ULONG ldap_win_bind(ಸಂರಚನಾ Curl_easy *data, LDAP *server,
                           ಧ್ರುವ ವರ್ಣ *user, ಧ್ರುವ ವರ್ಣ *passwd)
{
  ULONG rc = LDAP_INVALID_CREDENTIALS;

  PTCHAR inuser = ರಿಕ್ತ;
  PTCHAR inpass = ರಿಕ್ತ;

  ಯದಿ(user && passwd && (data->set.httpauth & CURLAUTH_BASIC)) {
    inuser = curlx_convert_UTF8_to_tchar(user);
    inpass = curlx_convert_UTF8_to_tchar(passwd);

    rc = ldap_simple_bind_s(server, inuser, inpass);

    curlx_free(inuser);
    curlx_free(inpass);
  }
#ifdef USE_WINDOWS_SSPI
  ಅನ್ಯಥಾ {
    rc = ldap_win_bind_auth(server, user, passwd, data->set.httpauth);
  }
#endif

  ಪ್ರತ್ಯಾಗಮ rc;
}
#endif /* USE_WIN32_LDAP */

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
ಸ್ಥಿರ bool ldap_str_needs_base64(ಧ್ರುವ ವರ್ಣ *s, size_t len)
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

ಸ್ಥಿರ bool ldap_ಮೌಲ್ಯ_needs_base64(ಧ್ರುವ ವರ್ಣ *attr, size_t attr_len,
                                    ಧ್ರುವ Ber_ಮೌಲ್ಯ *val)
{
  ಯದಿ((attr_len > 7) && curl_strequal(";binary", attr + attr_len - 7))
    ಪ್ರತ್ಯಾಗಮ TRUE;

  ಪ್ರತ್ಯಾಗಮ ldap_str_needs_base64(val->bv_val, val->bv_len);
}

#ifdef USE_WIN32_LDAP
ಸ್ಥಿರ BOOLEAN bypass_cert_verify(PLDAP Connection,
                                  PCCERT_CONTEXT *ppServerCert)
{
  (ಶೂನ್ಯ)Connection;
  CertFreeCertificateContext(*ppServerCert);
  /* approve any certificate since the verification is set to bypass */
  ಪ್ರತ್ಯಾಗಮ TRUE;
}
#endif

ಸ್ಥಿರ CURLcode show_vals(ಸಂರಚನಾ Curl_easy *data, Ber_ಮೌಲ್ಯ **vals,
                          ಧ್ರುವ ವರ್ಣ *attr)
{
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ i;
  size_t attr_len = strlen(attr);

  /* An attribute type has no base64 form in LDIF, so a type carrying control
     bytes (CR/LF etc.) would let a hostile server inject lines. A conformant
     type name never contains them, so reject it. */
  ಯದಿ(ldap_str_needs_base64(attr, attr_len)) {
    failf(data, "LDAP local: attribute type contains illegal bytes");
    ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
  }

  ಪ್ರತಿ(i = 0; vals[i] && !result; i++) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, "\t", 1);
    ಯದಿ(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, attr, attr_len);
    ಯದಿ(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, ":", 1);

    ಯದಿ(result)
      ಭಙ್ಗ;

    ಯದಿ(ldap_ಮೌಲ್ಯ_needs_base64(attr, attr_len, vals[i])) {
      ವರ್ಣ *val_b64 = ರಿಕ್ತ;
      size_t val_b64_sz = 0;

      /* Binary attribute, encode to base64. */
      ಯದಿ(vals[i]->bv_len)
        result = curlx_base64_encode((uint8_t *)vals[i]->bv_val,
                                     vals[i]->bv_len,
                                     &val_b64, &val_b64_sz);
      ಯದಿ(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, ": ", 2);
      ಯದಿ(!result && val_b64_sz)
        result = Curl_client_write(data, CLIENTWRITE_BODY, val_b64,
                                   val_b64_sz);
      curlx_free(val_b64);
    }
    ಅನ್ಯಥಾ {
      result = Curl_client_write(data, CLIENTWRITE_BODY, " ", 1);
      ಯದಿ(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   vals[i]->bv_val, vals[i]->bv_len);
    }

    ಯದಿ(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ldap_do(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  curl_ldap_ಅಂಕೆ_t rc = LDAP_SUCCESS;
  LDAP *server = ರಿಕ್ತ;
  LDAPURLDesc *ludp = ರಿಕ್ತ;
  LDAPMessage *ldapmsg = ರಿಕ್ತ;
  LDAPMessage *entryIterator;
  ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ = 0;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಪೂರ್ಣಾಙ್ಕ ldap_proto = LDAP_VERSION3;
  bool ldap_ssl = FALSE;
#ifdef LDAP_OPT_NETWORK_TIMEOUT
  ಸಂರಚನಾ timeval ldap_timeout = { 10, 0 }; /* 10s connection/search timeout */
#endif
#ifdef USE_WIN32_LDAP
  TCHAR *host = ರಿಕ್ತ;
#ಅನ್ಯಥಾ
  ವರ್ಣ *host = ರಿಕ್ತ;
#endif
  ಧ್ರುವ ವರ್ಣ *user = data->state.creds ? data->state.creds->user : ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *passwd = data->state.creds ? data->state.creds->passwd : ರಿಕ್ತ;
  ಸಂರಚನಾ ip_quadruple ipquad;
  bool is_ipv6;
  BerElement *ber = ರಿಕ್ತ;

  *done = TRUE; /* unconditionally */
  infof(data, "LDAP local: LDAP Vendor = %s ; LDAP Version = %d",
        LDAP_VENDOR_NAME, LDAP_VENDOR_VERSION);
  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

#ifdef HAVE_LDAP_URL_PARSE
  rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), &ludp);
#ಅನ್ಯಥಾ
  rc = ldap_url_parse_ಕೆಳಗಿನ(data, conn, &ludp);
#endif
  ಯದಿ(rc) {
    failf(data, "Bad LDAP URL: %s", ldap_err2string(rc));
    result = CURLE_URL_MALFORMAT;
    ಗಚ್ಛ quit;
  }

  result = Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad);
  ಯದಿ(result)
    ಗಚ್ಛ quit;

  /* Get the URL scheme (either ldap or ldaps) */
  ldap_ssl = Curl_conn_is_ssl(conn, FIRSTSOCKET);
  infof(data, "LDAP local: trying to establish %s connection",
        ldap_ssl ? "encrypted" : "cleartext");

#ifdef USE_WIN32_LDAP
  host = curlx_convert_UTF8_to_tchar(conn->origin->hostname);
  ಯದಿ(!host) {
    result = CURLE_OUT_OF_MEMORY;

    ಗಚ್ಛ quit;
  }
#ಅನ್ಯಥಾ
  host = conn->origin->hostname;
#endif

#ifdef USE_WIN32_LDAP
  ಯದಿ(ldap_ssl)
    server = ldap_sslinit(host, (curl_ldap_ಅಂಕೆ_t)ipquad.remote_port, 1);
  ಅನ್ಯಥಾ
#endif
    server = ldap_init(host, (curl_ldap_ಅಂಕೆ_t)ipquad.remote_port);
  ಯದಿ(!server) {
    failf(data, "LDAP: cannot setup connect to %s:%u",
          conn->origin->user_hostname, ipquad.remote_port);
    result = CURLE_COULDNT_CONNECT;
    ಗಚ್ಛ quit;
  }

#ifdef LDAP_OPT_NETWORK_TIMEOUT
  ldap_set_option(server, LDAP_OPT_NETWORK_TIMEOUT, &ldap_timeout);
#endif
  ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);

  /* Do not chase referrals. */
  ldap_set_option(server, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  ಯದಿ(ldap_ssl) {
#ifdef HAVE_LDAP_SSL
#ifdef USE_WIN32_LDAP
    /* Win32 LDAP uses the Windows CA store to verify certificates */
    ldap_set_option(server, LDAP_OPT_SSL, LDAP_OPT_ON);
    ಯದಿ(!conn->ssl_config.verifypeer) {
      ಯದಿ(conn->ssl_config.verifyhost) {
        failf(data, "LDAP local: host verification cannot be enabled when "
              "peer verification is disabled for Windows native LDAP");
        result = CURLE_NOT_BUILT_IN;
        ಗಚ್ಛ quit;
      }
      ldap_set_option(server, LDAP_OPT_SERVER_CERTIFICATE,
                      (ಶೂನ್ಯ *)(uintptr_t)bypass_cert_verify);
    }
#ನೋಚೇದ್ಯದಿ defined(LDAP_OPT_X_TLS) /* !USE_WIN32_LDAP */
    ಪೂರ್ಣಾಙ್ಕ ldap_option;
    ಧ್ರುವ ವರ್ಣ *ldap_ca = conn->ssl_config.CAfile;
    ಯದಿ(conn->ssl_config.verifypeer) {
      /* OpenLDAP SDK supports BASE64 files. */
      ಯದಿ(conn->ssl_config.cert_type &&
         !curl_strequal(conn->ssl_config.cert_type, "PEM")) {
        failf(data, "LDAP local: ERROR OpenLDAP only supports PEM cert-type");
        result = CURLE_SSL_CERTPROBLEM;
        ಗಚ್ಛ quit;
      }
      ಯದಿ(!ldap_ca) {
        failf(data, "LDAP local: ERROR PEM CA cert not set");
        result = CURLE_SSL_CERTPROBLEM;
        ಗಚ್ಛ quit;
      }
      infof(data, "LDAP local: using PEM CA cert: %s", ldap_ca);
      rc = ldap_set_option(server, LDAP_OPT_X_TLS_CACERTFILE, ldap_ca);
      ಯದಿ(rc != LDAP_SUCCESS) {
        failf(data, "LDAP local: ERROR setting PEM CA cert: %s",
              ldap_err2string(rc));
        result = CURLE_SSL_CERTPROBLEM;
        ಗಚ್ಛ quit;
      }
      ldap_option = LDAP_OPT_X_TLS_DEMAND;
    }
    ಅನ್ಯಥಾ
      ldap_option = LDAP_OPT_X_TLS_NEVER;

    rc = ldap_set_option(server, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_option);
    ಯದಿ(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting cert verify mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      ಗಚ್ಛ quit;
    }
    ldap_option = LDAP_OPT_X_TLS_HARD;
    rc = ldap_set_option(server, LDAP_OPT_X_TLS, &ldap_option);
    ಯದಿ(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting SSL/TLS mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      ಗಚ್ಛ quit;
    }
#ಅನ್ಯಥಾ /* !USE_WIN32_LDAP && !LDAP_OPT_X_TLS */
    /* we should probably never come up to here since configure
       should check in first place if we can support LDAP SSL/TLS */
    failf(data, "LDAP local: SSL/TLS not supported with this version "
          "of the OpenLDAP toolkit");
    result = CURLE_SSL_CERTPROBLEM;
    ಗಚ್ಛ quit;
#endif
#endif /* HAVE_LDAP_SSL */
  }
  ಅನ್ಯಥಾ ಯದಿ(data->set.use_ssl > CURLUSESSL_TRY) {
    failf(data, "LDAP local: explicit TLS not supported");
    result = CURLE_NOT_BUILT_IN;
    ಗಚ್ಛ quit;
  }

#ifdef USE_WIN32_LDAP
  rc = ldap_win_bind(data, server, user, passwd);
#ಅನ್ಯಥಾ
  rc = ldap_simple_bind_s(server, user, passwd);
#endif
  ಯದಿ(!ldap_ssl && rc != LDAP_SUCCESS) {
    ldap_proto = LDAP_VERSION2;
    ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);
#ifdef USE_WIN32_LDAP
    rc = ldap_win_bind(data, server, user, passwd);
#ಅನ್ಯಥಾ
    rc = ldap_simple_bind_s(server, user, passwd);
#endif
  }
  ಯದಿ(rc != LDAP_SUCCESS) {
#ifdef USE_WIN32_LDAP
    failf(data, "LDAP local: bind via ldap_win_bind %s",
          ldap_err2string(rc));
#ಅನ್ಯಥಾ
    failf(data, "LDAP local: bind via ldap_simple_bind_s %s",
          ldap_err2string(rc));
#endif
    result = CURLE_LDAP_CANNOT_BIND;
    ಗಚ್ಛ quit;
  }

  Curl_pgrsReset(data);
  rc = ldap_ಹುಡುಕು_s(server, ludp->lud_dn,
                     ludp->lud_scope,
                     ludp->lud_filter, ludp->lud_attrs, 0, &ldapmsg);

  ಯದಿ(rc != LDAP_SUCCESS && rc != LDAP_SIZELIMIT_EXCEEDED) {
    failf(data, "LDAP remote: %s", ldap_err2string(rc));
    result = CURLE_LDAP_ಹುಡುಕು_FAILED;
    ಗಚ್ಛ quit;
  }

  ಅಂಕೆ = 0;
  ಪ್ರತಿ(entryIterator = ldap_first_entry(server, ldapmsg);
      entryIterator;
      entryIterator = ldap_next_entry(server, entryIterator), ಅಂಕೆ++) {
#ifdef USE_WIN32_LDAP
    TCHAR *attribute;
#ಅನ್ಯಥಾ
    ವರ್ಣ *attribute;
#endif

    /* Get the DN and write it to the client */
    {
      ವರ್ಣ *name = ರಿಕ್ತ;
      size_t name_len = 0;
#ifdef USE_WIN32_LDAP
      TCHAR *dn = ldap_get_dn(server, entryIterator);
      ಯದಿ(dn)
        name = curlx_convert_tchar_to_UTF8(dn);
#ಅನ್ಯಥಾ
      ವರ್ಣ *dn = name = ldap_get_dn(server, entryIterator);
#endif
      ಯದಿ(!name)
        result = dn ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
      ಅನ್ಯಥಾ {
        name_len = strlen(name);
        ಯದಿ(ldap_str_needs_base64(name, name_len)) {
          ವರ್ಣ *dn_b64 = ರಿಕ್ತ;
          size_t dn_b64_sz = 0;

          /* A DN with e.g. CR/LF would inject LDIF lines, so encode it */
          result = curlx_base64_encode((uint8_t *)name, name_len,
                                       &dn_b64, &dn_b64_sz);
          ಯದಿ(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, "DN:: ", 5);
          ಯದಿ(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, dn_b64,
                                       dn_b64_sz);
          curlx_free(dn_b64);
        }
        ಅನ್ಯಥಾ {
          result = Curl_client_write(data, CLIENTWRITE_BODY, "DN: ", 4);
          ಯದಿ(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, name, name_len);
        }
        ಯದಿ(!result)
          result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      }
      FREE_ON_WINLDAP(name);
      ldap_memfree(dn);
      ಯದಿ(result)
        ಗಚ್ಛ quit;
    }

    /* Get the attributes and write them to the client */
    ಪ್ರತಿ(attribute = ldap_first_attribute(server, entryIterator, &ber);
        attribute;
        attribute = ldap_next_attribute(server, entryIterator, ber)) {
      Ber_ಮೌಲ್ಯ **vals;
#ifdef USE_WIN32_LDAP
      ವರ್ಣ *attr = curlx_convert_tchar_to_UTF8(attribute);
      ಯದಿ(!attr) {
        ldap_memfree(attribute);
        result = CURLE_OUT_OF_MEMORY;
        ಗಚ್ಛ quit;
      }
#ಅನ್ಯಥಾ
      ವರ್ಣ *attr = attribute;
#endif
      vals = ldap_get_ಮೌಲ್ಯಗಳು_len(server, entryIterator, attribute);
      ಯದಿ(vals) {
        result = show_vals(data, vals, attr);
        /* Free memory used to store values */
        ldap_ಮೌಲ್ಯ_free_len(vals);
      }

      /* Free the attribute as we are done with it */
      FREE_ON_WINLDAP(attr);
      ldap_memfree(attribute);

      ಯದಿ(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      ಯದಿ(result)
        ಗಚ್ಛ quit;
    }

    ಯದಿ(ber) {
      ber_free(ber, 0);
      ber = ರಿಕ್ತ;
    }
  }

quit:
  ಯದಿ(ber)
    ber_free(ber, 0);
  ಯದಿ(ldapmsg) {
    ldap_msgfree(ldapmsg);
    LDAP_TRACE(("Received %d entries\n", ಅಂಕೆ));
  }
  ಯದಿ(rc == LDAP_SIZELIMIT_EXCEEDED)
    infof(data, "There are more than %d entries", ಅಂಕೆ);
  ಯದಿ(ludp)
    ldap_free_urldesc(ludp);
  ಯದಿ(server)
    ldap_unbind_s(server);

  FREE_ON_WINLDAP(host);

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  connclose(conn);

  ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef DEBUG_LDAP
ಸ್ಥಿರ ಶೂನ್ಯ ldap_trace_ಕೆಳಗಿನ(ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ do_trace = -1;
  va_list args;

  ಯದಿ(do_trace == -1) {
    ಧ್ರುವ ವರ್ಣ *env = getenv("CURL_TRACE");
    curl_off_t e = 0;
    ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  ಯದಿ(!do_trace)
    ಪ್ರತ್ಯಾಗಮ;

  va_start(args, fmt);
  curl_mvfprintf(stderr, fmt, args);
  va_end(args);
}
#endif /* DEBUG_LDAP */

#ifndef HAVE_LDAP_URL_PARSE

/*
 * Return scope-value for a scope-string.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ str2scope(ಧ್ರುವ ವರ್ಣ *p)
{
  ಯದಿ(curl_strequal(p, "one"))
    ಪ್ರತ್ಯಾಗಮ LDAP_SCOPE_ONELEVEL;
  ಯದಿ(curl_strequal(p, "onetree"))
    ಪ್ರತ್ಯಾಗಮ LDAP_SCOPE_ONELEVEL;
  ಯದಿ(curl_strequal(p, "base"))
    ಪ್ರತ್ಯಾಗಮ LDAP_SCOPE_BASE;
  ಯದಿ(curl_strequal(p, "sub"))
    ಪ್ರತ್ಯಾಗಮ LDAP_SCOPE_SUBTREE;
  ಯದಿ(curl_strequal(p, "subtree"))
    ಪ್ರತ್ಯಾಗಮ LDAP_SCOPE_SUBTREE;
  ಪ್ರತ್ಯಾಗಮ -1;
}

/* number of entries in the attributes list */
ಸ್ಥಿರ size_t ಅಂಕೆ_entries(ಧ್ರುವ ವರ್ಣ *s)
{
  size_t ವಸ್ತುಗಳು = 1;

  s = strchr(s, ',');
  ಯಾವತ್(s) {
    ವಸ್ತುಗಳು++;
    s = strchr(s + 1, ',');
  }
  ಪ್ರತ್ಯಾಗಮ ವಸ್ತುಗಳು;
}

/*
 * Break apart the pieces of an LDAP URL.
 * Syntax:
 *   ldap://<hostname>:<port>/<base_dn>?<attributes>?<scope>?<filter>?<ext>
 *
 * <hostname> already known from 'conn->origin->hostname'.
 * <port>     already known from 'conn->origin->port'.
 * extract the rest from 'data->state.path+1'. All fields are optional.
 * e.g.
 *   ldap://<hostname>:<port>/?<attributes>?<scope>?<filter>
 * yields ludp->lud_dn = "".
 *
 * Defined in RFC4516 section 2.
 */
ಸ್ಥಿರ curl_ldap_ಅಂಕೆ_t ldap_url_parse2_ಕೆಳಗಿನ(ಸಂರಚನಾ Curl_easy *data,
                                           ಧ್ರುವ ಸಂರಚನಾ connectdata *conn,
                                           LDAPURLDesc *ludp)
{
  curl_ldap_ಅಂಕೆ_t rc = LDAP_SUCCESS;
  ವರ್ಣ *p;
  ವರ್ಣ *path;
  ವರ್ಣ *q = ರಿಕ್ತ;
  ವರ್ಣ *query = ರಿಕ್ತ;
  size_t i;

  ಯದಿ(!data ||
     !data->state.up.path ||
     data->state.up.path[0] != '/' ||
     ((data->state.origin->scheme != &Curl_scheme_ldap) &&
      (data->state.origin->scheme != &Curl_scheme_ldaps)))
    ಪ್ರತ್ಯಾಗಮ LDAP_INVALID_SYNTAX;

  ludp->lud_scope = LDAP_SCOPE_BASE;
  ludp->lud_port  = conn->origin->port;
  ludp->lud_host  = conn->origin->hostname;

  /* Duplicate the path */
  p = path = curlx_strdup(data->state.up.path + 1);
  ಯದಿ(!path)
    ಪ್ರತ್ಯಾಗಮ LDAP_NO_MEMORY;

  /* Duplicate the query if present */
  ಯದಿ(data->state.up.query) {
    q = query = curlx_strdup(data->state.up.query);
    ಯದಿ(!query) {
      curlx_free(path);
      ಪ್ರತ್ಯಾಗಮ LDAP_NO_MEMORY;
    }
  }

  /* Parse the DN (Distinguished Name) */
  ಯದಿ(*p) {
    ವರ್ಣ *dn = p;
    ವರ್ಣ *unescaped;
    CURLcode result;

    LDAP_TRACE(("DN '%s'\n", dn));

    /* Unescape the DN */
    result = Curl_urldecode(dn, 0, &unescaped, ರಿಕ್ತ, REJECT_ZERO);
    ಯದಿ(result) {
      rc = LDAP_NO_MEMORY;

      ಗಚ್ಛ quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_dn = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    ಯದಿ(!ludp->lud_dn) {
      rc = LDAP_NO_MEMORY;

      ಗಚ್ಛ quit;
    }
#ಅನ್ಯಥಾ
    ludp->lud_dn = unescaped;
#endif
  }

  p = q;
  ಯದಿ(!p)
    ಗಚ್ಛ quit;

  /* Parse the attributes. skip "??" */
  q = strchr(p, '?');
  ಯದಿ(q)
    *q++ = '\0';

  ಯದಿ(*p) {
    size_t ಎಣಿಕೆ = ಅಂಕೆ_entries(p); /* at least one */
    ಧ್ರುವ ವರ್ಣ *atp = p;

    /* Allocate our array (+1 for the NULL entry) */
#ifdef USE_WIN32_LDAP
    ludp->lud_attrs = curlx_calloc(ಎಣಿಕೆ + 1, ಪರಿಮಾಣ(TCHAR *));
#ಅನ್ಯಥಾ
    ludp->lud_attrs = curlx_calloc(ಎಣಿಕೆ + 1, ಪರಿಮಾಣ(ವರ್ಣ *));
#endif
    ಯದಿ(!ludp->lud_attrs) {
      rc = LDAP_NO_MEMORY;
      ಗಚ್ಛ quit;
    }

    ಪ್ರತಿ(i = 0; i < ಎಣಿಕೆ; i++) {
      ವರ್ಣ *unescaped;
      CURLcode result;
      ಸಂರಚನಾ Curl_str out;

      ಯದಿ(curlx_str_until(&atp, &out, 1024, ','))
        ಭಙ್ಗ;

      LDAP_TRACE(("attr[%zu] '%.*s'\n", i, (ಪೂರ್ಣಾಙ್ಕ)out.len, out.str));

      /* Unescape the attribute */
      result = Curl_urldecode(out.str, out.len, &unescaped, ರಿಕ್ತ, REJECT_ZERO);
      ಯದಿ(result) {
        rc = LDAP_NO_MEMORY;
        ಗಚ್ಛ quit;
      }

#ifdef USE_WIN32_LDAP
      /* Convert the unescaped string to a tchar */
      ludp->lud_attrs[i] = curlx_convert_UTF8_to_tchar(unescaped);

      /* Free the unescaped string as we are done with it */
      curlx_free(unescaped);

      ಯದಿ(!ludp->lud_attrs[i]) {
        rc = LDAP_NO_MEMORY;
        ಗಚ್ಛ quit;
      }
#ಅನ್ಯಥಾ
      ludp->lud_attrs[i] = unescaped;
#endif

      ludp->lud_attrs_dups++;
      ಯದಿ(curlx_str_single(&atp, ','))
        ಭಙ್ಗ;
    }
  }

  p = q;
  ಯದಿ(!p)
    ಗಚ್ಛ quit;

  /* Parse the scope. skip "??" */
  q = strchr(p, '?');
  ಯದಿ(q)
    *q++ = '\0';

  ಯದಿ(*p) {
    ludp->lud_scope = str2scope(p);
    ಯದಿ(ludp->lud_scope == -1) {
      rc = LDAP_INVALID_SYNTAX;

      ಗಚ್ಛ quit;
    }
    LDAP_TRACE(("scope %d\n", ludp->lud_scope));
  }

  p = q;
  ಯದಿ(!p)
    ಗಚ್ಛ quit;

  /* Parse the filter */
  q = strchr(p, '?');
  ಯದಿ(q)
    *q++ = '\0';

  ಯದಿ(*p) {
    ವರ್ಣ *filter = p;
    ವರ್ಣ *unescaped;
    CURLcode result;

    LDAP_TRACE(("filter '%s'\n", filter));

    /* Unescape the filter */
    result = Curl_urldecode(filter, 0, &unescaped, ರಿಕ್ತ, REJECT_CTRL);
    ಯದಿ(result) {
      rc = LDAP_NO_MEMORY;

      ಗಚ್ಛ quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_filter = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    ಯದಿ(!ludp->lud_filter) {
      rc = LDAP_NO_MEMORY;

      ಗಚ್ಛ quit;
    }
#ಅನ್ಯಥಾ
    ludp->lud_filter = unescaped;
#endif
  }

  p = q;
  ಯದಿ(p && !*p) {
    rc = LDAP_INVALID_SYNTAX;

    ಗಚ್ಛ quit;
  }

quit:
  curlx_free(path);
  curlx_free(query);

  ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ curl_ldap_ಅಂಕೆ_t ldap_url_parse_ಕೆಳಗಿನ(ಸಂರಚನಾ Curl_easy *data,
                                          ಧ್ರುವ ಸಂರಚನಾ connectdata *conn,
                                          LDAPURLDesc **ludpp)
{
  LDAPURLDesc *ludp = curlx_calloc(1, ಪರಿಮಾಣ(*ludp));
  curl_ldap_ಅಂಕೆ_t rc;

  *ludpp = ರಿಕ್ತ;
  ಯದಿ(!ludp)
    ಪ್ರತ್ಯಾಗಮ LDAP_NO_MEMORY;

  rc = ldap_url_parse2_ಕೆಳಗಿನ(data, conn, ludp);
  ಯದಿ(rc != LDAP_SUCCESS) {
    ldap_free_urldesc_ಕೆಳಗಿನ(ludp);
    ludp = ರಿಕ್ತ;
  }
  *ludpp = ludp;
  ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಶೂನ್ಯ ldap_free_urldesc_ಕೆಳಗಿನ(LDAPURLDesc *ludp)
{
  ಯದಿ(!ludp)
    ಪ್ರತ್ಯಾಗಮ;

  curlx_free(ludp->lud_dn);
  curlx_free(ludp->lud_filter);

  ಯದಿ(ludp->lud_attrs) {
    size_t i;
    ಪ್ರತಿ(i = 0; i < ludp->lud_attrs_dups; i++) {
      curlx_free(ludp->lud_attrs[i]);
    }
    curlx_free(ludp->lud_attrs);
  }

  curlx_free(ludp);
}
#endif /* !HAVE_LDAP_URL_PARSE */

ಶೂನ್ಯ Curl_ldap_version(ವರ್ಣ *buf, size_t bufsz)
{
#ifdef USE_WIN32_LDAP
  curl_msnprintf(buf, bufsz, "WinLDAP");
#ಅನ್ಯಥಾ
#ifdef LDAP_OPT_X_TLS_PASSPHRASE
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ flavor[] = "/Apple";
#ಅನ್ಯಥಾ
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ flavor[] = "";
#endif
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  /* Comparing against 0, as different platforms
     disagree on the success define name */
  ಯದಿ(ldap_get_option(ರಿಕ್ತ, LDAP_OPT_API_INFO, &api) == 0) {
    unsigned ಪೂರ್ಣಾಙ್ಕ patch = (unsigned ಪೂರ್ಣಾಙ್ಕ)(api.ldapai_vendor_version % 100);
    unsigned ಪೂರ್ಣಾಙ್ಕ major = (unsigned ಪೂರ್ಣಾಙ್ಕ)(api.ldapai_vendor_version / 10000);
    unsigned ಪೂರ್ಣಾಙ್ಕ minor =
      (((unsigned ಪೂರ್ಣಾಙ್ಕ)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
#ifdef __OS400__
    curl_msnprintf(buf, bufsz, "IBMLDAP/%u.%u.%u", major, minor, patch);
    ldap_ಮೌಲ್ಯ_free(api.ldapai_extensions);
    (ಶೂನ್ಯ)flavor;
#ಅನ್ಯಥಾ
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u%s",
                   api.ldapai_vendor_name, major, minor, patch, flavor);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((ಶೂನ್ಯ **)api.ldapai_extensions);
#endif
  }
  ಅನ್ಯಥಾ
    curl_msnprintf(buf, bufsz, "LDAP/1");
#endif /* USE_WIN32_LDAP */
}

/*
 * LDAP protocol handler.
 */
ಧ್ರುವ ಸಂರಚನಾ Curl_protocol Curl_protocol_ldap = {
  ZERO_NULL,                            /* setup_connection */
  ldap_do,                              /* do_it */
  ZERO_NULL,                            /* done */
  ZERO_NULL,                            /* do_more */
  ZERO_NULL,                            /* connect_it */
  ZERO_NULL,                            /* connecting */
  ZERO_NULL,                            /* doing */
  ZERO_NULL,                            /* proto_pollset */
  ZERO_NULL,                            /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  ZERO_NULL,                            /* perform_pollset */
  ZERO_NULL,                            /* disconnect */
  ZERO_NULL,                            /* write_resp */
  ZERO_NULL,                            /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ZERO_NULL,                            /* attach connection */
  ZERO_NULL,                            /* follow */
};

#ಯದಿ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ತೆಗೆ
#endif

#endif /* !CURL_DISABLE_LDAP && !USE_OPENLDAP */
