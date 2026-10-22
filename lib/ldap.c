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
#അന്തര്ഭാവ "curl_setup.h"
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curl_ldap.h"

#എങ്കിൽ !defined(CURL_DISABLE_LDAP) && !defined(USE_OPENLDAP)

#എങ്കിൽ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic തള്ളുക
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
#അല്ലെങ്കിൽ
#  define LDAP_DEPRECATED 1     /* Be sure ldap_init() is defined. */
#  ifdef HAVE_LBER_H
#    include <lber.h>
#  endif
#  include <ldap.h>
#  എങ്കിൽ defined(HAVE_LDAP_SSL) && defined(HAVE_LDAP_SSL_H)
#    include <ldap_ssl.h>
#  endif /* HAVE_LDAP_SSL && HAVE_LDAP_SSL_H */
#endif

#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "bufref.h"
#അന്തര്ഭാവ "curlx/multibyte.h"
#അന്തര്ഭാവ "curlx/base64.h"
#അന്തര്ഭാവ "connect.h"

#ifdef USE_WIN32_LDAP
#define FREE_ON_WINLDAP(x) curlx_free(x)
#define curl_ldap_അക്കം_t    ULONG
#അല്ലെങ്കിൽ
#define FREE_ON_WINLDAP(x) ചെയ്യുക {} ആകുമ്പോൾ(0)
#define curl_ldap_അക്കം_t    പൂർണ്ണസംഖ്യ
#endif

#ifndef HAVE_LDAP_URL_PARSE

/* Use our own implementation. */

ഘടന ldap_urldesc {
  അക്ഷരം   *lud_host;
  പൂർണ്ണസംഖ്യ     lud_port;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_dn;
  TCHAR **lud_attrs;
#അല്ലെങ്കിൽ
  അക്ഷരം   *lud_dn;
  അക്ഷരം  **lud_attrs;
#endif
  പൂർണ്ണസംഖ്യ     lud_scope;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_filter;
#അല്ലെങ്കിൽ
  അക്ഷരം   *lud_filter;
#endif
  അക്ഷരം  **lud_exts;
  size_t  lud_attrs_dups; /* how many were dup'ed, this field is not in the
                             "real" struct so can only be used in code
                             without HAVE_LDAP_URL_PARSE defined */
};

#undef LDAPURLDesc
#define LDAPURLDesc ഘടന ldap_urldesc

സ്ഥിതം curl_ldap_അക്കം_t ldap_url_parse_താഴെയുള്ള(ഘടന Curl_easy *data,
                                          സ്ഥിരം ഘടന connectdata *conn,
                                          LDAPURLDesc **ludpp);
സ്ഥിതം ശൂന്യം ldap_free_urldesc_താഴെയുള്ള(LDAPURLDesc *ludp);

#undef ldap_free_urldesc
#define ldap_free_urldesc       ldap_free_urldesc_താഴെയുള്ള

#endif /* !HAVE_LDAP_URL_PARSE */

#ifdef DEBUG_LDAP
#define LDAP_TRACE(x)                 \
  ചെയ്യുക {                                \
    ldap_trace_താഴെയുള്ള("%u: ", __LINE__); \
    ldap_trace_താഴെയുള്ള x;                 \
  } ആകുമ്പോൾ(0)

സ്ഥിതം ശൂന്യം ldap_trace_താഴെയുള്ള(സ്ഥിരം അക്ഷരം *fmt, ...) CURL_PRINTF(1, 2);
#അല്ലെങ്കിൽ
#define LDAP_TRACE(x) Curl_nop_stmt
#endif

#എങ്കിൽ defined(USE_WIN32_LDAP) && defined(ldap_err2string)
/* Use ANSI error strings in Unicode builds */
#undef ldap_err2string
#define ldap_err2string ldap_err2stringA
#endif

#എങ്കിൽ defined(USE_WIN32_LDAP) && defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'void *' of greater size */
#undef LDAP_OPT_ON
#undef LDAP_OPT_OFF
#define LDAP_OPT_ON   ((ശൂന്യം *)(size_t)1)
#define LDAP_OPT_OFF  ((ശൂന്യം *)(size_t)0)
#endif

#ifdef USE_WIN32_LDAP

#ifdef USE_WINDOWS_SSPI
സ്ഥിതം ULONG ldap_win_bind_auth(LDAP *server, സ്ഥിരം അക്ഷരം *user,
                                സ്ഥിരം അക്ഷരം *passwd, unsigned long authflags)
{
  ULONG method = 0;
  SEC_WINNT_AUTH_IDENTITY_EX cred;
  ULONG rc = LDAP_AUTH_METHOD_NOT_SUPPORTED;

  memset(&cred, 0, വലുപ്പം(cred));

#ifdef USE_SPNEGO
  എങ്കിൽ(authflags & CURLAUTH_NEGOTIATE) {
    method = LDAP_AUTH_NEGOTIATE;
  }
  അല്ലെങ്കിൽ
#endif
#ifdef USE_NTLM
  എങ്കിൽ(authflags & CURLAUTH_NTLM) {
    method = LDAP_AUTH_NTLM;
  }
  അല്ലെങ്കിൽ
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  എങ്കിൽ(authflags & CURLAUTH_DIGEST) {
    method = LDAP_AUTH_DIGEST;
  }
  അല്ലെങ്കിൽ
#endif
  {
    /* required anyway if one of upper preprocessor definitions enabled */
  }

  എങ്കിൽ(method && user && passwd) {
    CURLcode result = Curl_create_sspi_identity(user, passwd, &cred);
    എങ്കിൽ(!result) {
      rc = ldap_bind_s(server, ശൂന്യ, (TCHAR *)&cred, method);
      Curl_sspi_free_identity(&cred);
    }
    അല്ലെങ്കിൽ {
      rc = LDAP_NO_MEMORY;
    }
  }
  അല്ലെങ്കിൽ {
    /* proceed with current user credentials */
    method = LDAP_AUTH_NEGOTIATE;
    rc = ldap_bind_s(server, ശൂന്യ, ശൂന്യ, method);
  }
  തിരികെ rc;
}
#endif /* USE_WINDOWS_SSPI */

സ്ഥിതം ULONG ldap_win_bind(ഘടന Curl_easy *data, LDAP *server,
                           സ്ഥിരം അക്ഷരം *user, സ്ഥിരം അക്ഷരം *passwd)
{
  ULONG rc = LDAP_INVALID_CREDENTIALS;

  PTCHAR inuser = ശൂന്യ;
  PTCHAR inpass = ശൂന്യ;

  എങ്കിൽ(user && passwd && (data->set.httpauth & CURLAUTH_BASIC)) {
    inuser = curlx_convert_UTF8_to_tchar(user);
    inpass = curlx_convert_UTF8_to_tchar(passwd);

    rc = ldap_simple_bind_s(server, inuser, inpass);

    curlx_free(inuser);
    curlx_free(inpass);
  }
#ifdef USE_WINDOWS_SSPI
  അല്ലെങ്കിൽ {
    rc = ldap_win_bind_auth(server, user, passwd, data->set.httpauth);
  }
#endif

  തിരികെ rc;
}
#endif /* USE_WIN32_LDAP */

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
സ്ഥിതം bool ldap_str_needs_base64(സ്ഥിരം അക്ഷരം *s, size_t len)
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

സ്ഥിതം bool ldap_മൂല്യം_needs_base64(സ്ഥിരം അക്ഷരം *attr, size_t attr_len,
                                    സ്ഥിരം Ber_മൂല്യം *val)
{
  എങ്കിൽ((attr_len > 7) && curl_strequal(";binary", attr + attr_len - 7))
    തിരികെ TRUE;

  തിരികെ ldap_str_needs_base64(val->bv_val, val->bv_len);
}

#ifdef USE_WIN32_LDAP
സ്ഥിതം BOOLEAN bypass_cert_verify(PLDAP Connection,
                                  PCCERT_CONTEXT *ppServerCert)
{
  (ശൂന്യം)Connection;
  CertFreeCertificateContext(*ppServerCert);
  /* approve any certificate since the verification is set to bypass */
  തിരികെ TRUE;
}
#endif

സ്ഥിതം CURLcode show_vals(ഘടന Curl_easy *data, Ber_മൂല്യം **vals,
                          സ്ഥിരം അക്ഷരം *attr)
{
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ i;
  size_t attr_len = strlen(attr);

  /* An attribute type has no base64 form in LDIF, so a type carrying control
     bytes (CR/LF etc.) would let a hostile server inject lines. A conformant
     type name never contains them, so reject it. */
  എങ്കിൽ(ldap_str_needs_base64(attr, attr_len)) {
    failf(data, "LDAP local: attribute type contains illegal bytes");
    തിരികെ CURLE_WEIRD_SERVER_REPLY;
  }

  വേണ്ടി(i = 0; vals[i] && !result; i++) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, "\t", 1);
    എങ്കിൽ(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, attr, attr_len);
    എങ്കിൽ(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, ":", 1);

    എങ്കിൽ(result)
      നിർത്തുക;

    എങ്കിൽ(ldap_മൂല്യം_needs_base64(attr, attr_len, vals[i])) {
      അക്ഷരം *val_b64 = ശൂന്യ;
      size_t val_b64_sz = 0;

      /* Binary attribute, encode to base64. */
      എങ്കിൽ(vals[i]->bv_len)
        result = curlx_base64_encode((uint8_t *)vals[i]->bv_val,
                                     vals[i]->bv_len,
                                     &val_b64, &val_b64_sz);
      എങ്കിൽ(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, ": ", 2);
      എങ്കിൽ(!result && val_b64_sz)
        result = Curl_client_write(data, CLIENTWRITE_BODY, val_b64,
                                   val_b64_sz);
      curlx_free(val_b64);
    }
    അല്ലെങ്കിൽ {
      result = Curl_client_write(data, CLIENTWRITE_BODY, " ", 1);
      എങ്കിൽ(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   vals[i]->bv_val, vals[i]->bv_len);
    }

    എങ്കിൽ(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode ldap_do(ഘടന Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  curl_ldap_അക്കം_t rc = LDAP_SUCCESS;
  LDAP *server = ശൂന്യ;
  LDAPURLDesc *ludp = ശൂന്യ;
  LDAPMessage *ldapmsg = ശൂന്യ;
  LDAPMessage *entryIterator;
  പൂർണ്ണസംഖ്യ അക്കം = 0;
  ഘടന connectdata *conn = data->conn;
  പൂർണ്ണസംഖ്യ ldap_proto = LDAP_VERSION3;
  bool ldap_ssl = FALSE;
#ifdef LDAP_OPT_NETWORK_TIMEOUT
  ഘടന timeval ldap_timeout = { 10, 0 }; /* 10s connection/search timeout */
#endif
#ifdef USE_WIN32_LDAP
  TCHAR *host = ശൂന്യ;
#അല്ലെങ്കിൽ
  അക്ഷരം *host = ശൂന്യ;
#endif
  സ്ഥിരം അക്ഷരം *user = data->state.creds ? data->state.creds->user : ശൂന്യ;
  സ്ഥിരം അക്ഷരം *passwd = data->state.creds ? data->state.creds->passwd : ശൂന്യ;
  ഘടന ip_quadruple ipquad;
  bool is_ipv6;
  BerElement *ber = ശൂന്യ;

  *done = TRUE; /* unconditionally */
  infof(data, "LDAP local: LDAP Vendor = %s ; LDAP Version = %d",
        LDAP_VENDOR_NAME, LDAP_VENDOR_VERSION);
  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

#ifdef HAVE_LDAP_URL_PARSE
  rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), &ludp);
#അല്ലെങ്കിൽ
  rc = ldap_url_parse_താഴെയുള്ള(data, conn, &ludp);
#endif
  എങ്കിൽ(rc) {
    failf(data, "Bad LDAP URL: %s", ldap_err2string(rc));
    result = CURLE_URL_MALFORMAT;
    പോകുക quit;
  }

  result = Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad);
  എങ്കിൽ(result)
    പോകുക quit;

  /* Get the URL scheme (either ldap or ldaps) */
  ldap_ssl = Curl_conn_is_ssl(conn, FIRSTSOCKET);
  infof(data, "LDAP local: trying to establish %s connection",
        ldap_ssl ? "encrypted" : "cleartext");

#ifdef USE_WIN32_LDAP
  host = curlx_convert_UTF8_to_tchar(conn->origin->hostname);
  എങ്കിൽ(!host) {
    result = CURLE_OUT_OF_MEMORY;

    പോകുക quit;
  }
#അല്ലെങ്കിൽ
  host = conn->origin->hostname;
#endif

#ifdef USE_WIN32_LDAP
  എങ്കിൽ(ldap_ssl)
    server = ldap_sslinit(host, (curl_ldap_അക്കം_t)ipquad.remote_port, 1);
  അല്ലെങ്കിൽ
#endif
    server = ldap_init(host, (curl_ldap_അക്കം_t)ipquad.remote_port);
  എങ്കിൽ(!server) {
    failf(data, "LDAP: cannot setup connect to %s:%u",
          conn->origin->user_hostname, ipquad.remote_port);
    result = CURLE_COULDNT_CONNECT;
    പോകുക quit;
  }

#ifdef LDAP_OPT_NETWORK_TIMEOUT
  ldap_set_option(server, LDAP_OPT_NETWORK_TIMEOUT, &ldap_timeout);
#endif
  ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);

  /* Do not chase referrals. */
  ldap_set_option(server, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  എങ്കിൽ(ldap_ssl) {
#ifdef HAVE_LDAP_SSL
#ifdef USE_WIN32_LDAP
    /* Win32 LDAP uses the Windows CA store to verify certificates */
    ldap_set_option(server, LDAP_OPT_SSL, LDAP_OPT_ON);
    എങ്കിൽ(!conn->ssl_config.verifypeer) {
      എങ്കിൽ(conn->ssl_config.verifyhost) {
        failf(data, "LDAP local: host verification cannot be enabled when "
              "peer verification is disabled for Windows native LDAP");
        result = CURLE_NOT_BUILT_IN;
        പോകുക quit;
      }
      ldap_set_option(server, LDAP_OPT_SERVER_CERTIFICATE,
                      (ശൂന്യം *)(uintptr_t)bypass_cert_verify);
    }
#അല്ലെങ്കിലെങ്കിൽ defined(LDAP_OPT_X_TLS) /* !USE_WIN32_LDAP */
    പൂർണ്ണസംഖ്യ ldap_option;
    സ്ഥിരം അക്ഷരം *ldap_ca = conn->ssl_config.CAfile;
    എങ്കിൽ(conn->ssl_config.verifypeer) {
      /* OpenLDAP SDK supports BASE64 files. */
      എങ്കിൽ(conn->ssl_config.cert_type &&
         !curl_strequal(conn->ssl_config.cert_type, "PEM")) {
        failf(data, "LDAP local: ERROR OpenLDAP only supports PEM cert-type");
        result = CURLE_SSL_CERTPROBLEM;
        പോകുക quit;
      }
      എങ്കിൽ(!ldap_ca) {
        failf(data, "LDAP local: ERROR PEM CA cert not set");
        result = CURLE_SSL_CERTPROBLEM;
        പോകുക quit;
      }
      infof(data, "LDAP local: using PEM CA cert: %s", ldap_ca);
      rc = ldap_set_option(server, LDAP_OPT_X_TLS_CACERTFILE, ldap_ca);
      എങ്കിൽ(rc != LDAP_SUCCESS) {
        failf(data, "LDAP local: ERROR setting PEM CA cert: %s",
              ldap_err2string(rc));
        result = CURLE_SSL_CERTPROBLEM;
        പോകുക quit;
      }
      ldap_option = LDAP_OPT_X_TLS_DEMAND;
    }
    അല്ലെങ്കിൽ
      ldap_option = LDAP_OPT_X_TLS_NEVER;

    rc = ldap_set_option(server, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_option);
    എങ്കിൽ(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting cert verify mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      പോകുക quit;
    }
    ldap_option = LDAP_OPT_X_TLS_HARD;
    rc = ldap_set_option(server, LDAP_OPT_X_TLS, &ldap_option);
    എങ്കിൽ(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting SSL/TLS mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      പോകുക quit;
    }
#അല്ലെങ്കിൽ /* !USE_WIN32_LDAP && !LDAP_OPT_X_TLS */
    /* we should probably never come up to here since configure
       should check in first place if we can support LDAP SSL/TLS */
    failf(data, "LDAP local: SSL/TLS not supported with this version "
          "of the OpenLDAP toolkit");
    result = CURLE_SSL_CERTPROBLEM;
    പോകുക quit;
#endif
#endif /* HAVE_LDAP_SSL */
  }
  അല്ലെങ്കിൽ എങ്കിൽ(data->set.use_ssl > CURLUSESSL_TRY) {
    failf(data, "LDAP local: explicit TLS not supported");
    result = CURLE_NOT_BUILT_IN;
    പോകുക quit;
  }

#ifdef USE_WIN32_LDAP
  rc = ldap_win_bind(data, server, user, passwd);
#അല്ലെങ്കിൽ
  rc = ldap_simple_bind_s(server, user, passwd);
#endif
  എങ്കിൽ(!ldap_ssl && rc != LDAP_SUCCESS) {
    ldap_proto = LDAP_VERSION2;
    ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);
#ifdef USE_WIN32_LDAP
    rc = ldap_win_bind(data, server, user, passwd);
#അല്ലെങ്കിൽ
    rc = ldap_simple_bind_s(server, user, passwd);
#endif
  }
  എങ്കിൽ(rc != LDAP_SUCCESS) {
#ifdef USE_WIN32_LDAP
    failf(data, "LDAP local: bind via ldap_win_bind %s",
          ldap_err2string(rc));
#അല്ലെങ്കിൽ
    failf(data, "LDAP local: bind via ldap_simple_bind_s %s",
          ldap_err2string(rc));
#endif
    result = CURLE_LDAP_CANNOT_BIND;
    പോകുക quit;
  }

  Curl_pgrsReset(data);
  rc = ldap_തിരയൽ_s(server, ludp->lud_dn,
                     ludp->lud_scope,
                     ludp->lud_filter, ludp->lud_attrs, 0, &ldapmsg);

  എങ്കിൽ(rc != LDAP_SUCCESS && rc != LDAP_SIZELIMIT_EXCEEDED) {
    failf(data, "LDAP remote: %s", ldap_err2string(rc));
    result = CURLE_LDAP_തിരയൽ_FAILED;
    പോകുക quit;
  }

  അക്കം = 0;
  വേണ്ടി(entryIterator = ldap_first_entry(server, ldapmsg);
      entryIterator;
      entryIterator = ldap_next_entry(server, entryIterator), അക്കം++) {
#ifdef USE_WIN32_LDAP
    TCHAR *attribute;
#അല്ലെങ്കിൽ
    അക്ഷരം *attribute;
#endif

    /* Get the DN and write it to the client */
    {
      അക്ഷരം *name = ശൂന്യ;
      size_t name_len = 0;
#ifdef USE_WIN32_LDAP
      TCHAR *dn = ldap_get_dn(server, entryIterator);
      എങ്കിൽ(dn)
        name = curlx_convert_tchar_to_UTF8(dn);
#അല്ലെങ്കിൽ
      അക്ഷരം *dn = name = ldap_get_dn(server, entryIterator);
#endif
      എങ്കിൽ(!name)
        result = dn ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
      അല്ലെങ്കിൽ {
        name_len = strlen(name);
        എങ്കിൽ(ldap_str_needs_base64(name, name_len)) {
          അക്ഷരം *dn_b64 = ശൂന്യ;
          size_t dn_b64_sz = 0;

          /* A DN with e.g. CR/LF would inject LDIF lines, so encode it */
          result = curlx_base64_encode((uint8_t *)name, name_len,
                                       &dn_b64, &dn_b64_sz);
          എങ്കിൽ(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, "DN:: ", 5);
          എങ്കിൽ(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, dn_b64,
                                       dn_b64_sz);
          curlx_free(dn_b64);
        }
        അല്ലെങ്കിൽ {
          result = Curl_client_write(data, CLIENTWRITE_BODY, "DN: ", 4);
          എങ്കിൽ(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, name, name_len);
        }
        എങ്കിൽ(!result)
          result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      }
      FREE_ON_WINLDAP(name);
      ldap_memfree(dn);
      എങ്കിൽ(result)
        പോകുക quit;
    }

    /* Get the attributes and write them to the client */
    വേണ്ടി(attribute = ldap_first_attribute(server, entryIterator, &ber);
        attribute;
        attribute = ldap_next_attribute(server, entryIterator, ber)) {
      Ber_മൂല്യം **vals;
#ifdef USE_WIN32_LDAP
      അക്ഷരം *attr = curlx_convert_tchar_to_UTF8(attribute);
      എങ്കിൽ(!attr) {
        ldap_memfree(attribute);
        result = CURLE_OUT_OF_MEMORY;
        പോകുക quit;
      }
#അല്ലെങ്കിൽ
      അക്ഷരം *attr = attribute;
#endif
      vals = ldap_get_മൂല്യങ്ങൾ_len(server, entryIterator, attribute);
      എങ്കിൽ(vals) {
        result = show_vals(data, vals, attr);
        /* Free memory used to store values */
        ldap_മൂല്യം_free_len(vals);
      }

      /* Free the attribute as we are done with it */
      FREE_ON_WINLDAP(attr);
      ldap_memfree(attribute);

      എങ്കിൽ(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      എങ്കിൽ(result)
        പോകുക quit;
    }

    എങ്കിൽ(ber) {
      ber_free(ber, 0);
      ber = ശൂന്യ;
    }
  }

quit:
  എങ്കിൽ(ber)
    ber_free(ber, 0);
  എങ്കിൽ(ldapmsg) {
    ldap_msgfree(ldapmsg);
    LDAP_TRACE(("Received %d entries\n", അക്കം));
  }
  എങ്കിൽ(rc == LDAP_SIZELIMIT_EXCEEDED)
    infof(data, "There are more than %d entries", അക്കം);
  എങ്കിൽ(ludp)
    ldap_free_urldesc(ludp);
  എങ്കിൽ(server)
    ldap_unbind_s(server);

  FREE_ON_WINLDAP(host);

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  connclose(conn);

  തിരികെ result;
}

#ifdef DEBUG_LDAP
സ്ഥിതം ശൂന്യം ldap_trace_താഴെയുള്ള(സ്ഥിരം അക്ഷരം *fmt, ...)
{
  സ്ഥിതം പൂർണ്ണസംഖ്യ do_trace = -1;
  va_list args;

  എങ്കിൽ(do_trace == -1) {
    സ്ഥിരം അക്ഷരം *env = getenv("CURL_TRACE");
    curl_off_t e = 0;
    എങ്കിൽ(!curlx_str_സംഖ്യ(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  എങ്കിൽ(!do_trace)
    തിരികെ;

  va_start(args, fmt);
  curl_mvfprintf(stderr, fmt, args);
  va_end(args);
}
#endif /* DEBUG_LDAP */

#ifndef HAVE_LDAP_URL_PARSE

/*
 * Return scope-value for a scope-string.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ str2scope(സ്ഥിരം അക്ഷരം *p)
{
  എങ്കിൽ(curl_strequal(p, "one"))
    തിരികെ LDAP_SCOPE_ONELEVEL;
  എങ്കിൽ(curl_strequal(p, "onetree"))
    തിരികെ LDAP_SCOPE_ONELEVEL;
  എങ്കിൽ(curl_strequal(p, "base"))
    തിരികെ LDAP_SCOPE_BASE;
  എങ്കിൽ(curl_strequal(p, "sub"))
    തിരികെ LDAP_SCOPE_SUBTREE;
  എങ്കിൽ(curl_strequal(p, "subtree"))
    തിരികെ LDAP_SCOPE_SUBTREE;
  തിരികെ -1;
}

/* number of entries in the attributes list */
സ്ഥിതം size_t അക്കം_entries(സ്ഥിരം അക്ഷരം *s)
{
  size_t ഇനങ്ങൾ = 1;

  s = strchr(s, ',');
  ആകുമ്പോൾ(s) {
    ഇനങ്ങൾ++;
    s = strchr(s + 1, ',');
  }
  തിരികെ ഇനങ്ങൾ;
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
സ്ഥിതം curl_ldap_അക്കം_t ldap_url_parse2_താഴെയുള്ള(ഘടന Curl_easy *data,
                                           സ്ഥിരം ഘടന connectdata *conn,
                                           LDAPURLDesc *ludp)
{
  curl_ldap_അക്കം_t rc = LDAP_SUCCESS;
  അക്ഷരം *p;
  അക്ഷരം *path;
  അക്ഷരം *q = ശൂന്യ;
  അക്ഷരം *query = ശൂന്യ;
  size_t i;

  എങ്കിൽ(!data ||
     !data->state.up.path ||
     data->state.up.path[0] != '/' ||
     ((data->state.origin->scheme != &Curl_scheme_ldap) &&
      (data->state.origin->scheme != &Curl_scheme_ldaps)))
    തിരികെ LDAP_INVALID_SYNTAX;

  ludp->lud_scope = LDAP_SCOPE_BASE;
  ludp->lud_port  = conn->origin->port;
  ludp->lud_host  = conn->origin->hostname;

  /* Duplicate the path */
  p = path = curlx_strdup(data->state.up.path + 1);
  എങ്കിൽ(!path)
    തിരികെ LDAP_NO_MEMORY;

  /* Duplicate the query if present */
  എങ്കിൽ(data->state.up.query) {
    q = query = curlx_strdup(data->state.up.query);
    എങ്കിൽ(!query) {
      curlx_free(path);
      തിരികെ LDAP_NO_MEMORY;
    }
  }

  /* Parse the DN (Distinguished Name) */
  എങ്കിൽ(*p) {
    അക്ഷരം *dn = p;
    അക്ഷരം *unescaped;
    CURLcode result;

    LDAP_TRACE(("DN '%s'\n", dn));

    /* Unescape the DN */
    result = Curl_urldecode(dn, 0, &unescaped, ശൂന്യ, REJECT_ZERO);
    എങ്കിൽ(result) {
      rc = LDAP_NO_MEMORY;

      പോകുക quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_dn = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    എങ്കിൽ(!ludp->lud_dn) {
      rc = LDAP_NO_MEMORY;

      പോകുക quit;
    }
#അല്ലെങ്കിൽ
    ludp->lud_dn = unescaped;
#endif
  }

  p = q;
  എങ്കിൽ(!p)
    പോകുക quit;

  /* Parse the attributes. skip "??" */
  q = strchr(p, '?');
  എങ്കിൽ(q)
    *q++ = '\0';

  എങ്കിൽ(*p) {
    size_t എണ്ണം = അക്കം_entries(p); /* at least one */
    സ്ഥിരം അക്ഷരം *atp = p;

    /* Allocate our array (+1 for the NULL entry) */
#ifdef USE_WIN32_LDAP
    ludp->lud_attrs = curlx_calloc(എണ്ണം + 1, വലുപ്പം(TCHAR *));
#അല്ലെങ്കിൽ
    ludp->lud_attrs = curlx_calloc(എണ്ണം + 1, വലുപ്പം(അക്ഷരം *));
#endif
    എങ്കിൽ(!ludp->lud_attrs) {
      rc = LDAP_NO_MEMORY;
      പോകുക quit;
    }

    വേണ്ടി(i = 0; i < എണ്ണം; i++) {
      അക്ഷരം *unescaped;
      CURLcode result;
      ഘടന Curl_str out;

      എങ്കിൽ(curlx_str_until(&atp, &out, 1024, ','))
        നിർത്തുക;

      LDAP_TRACE(("attr[%zu] '%.*s'\n", i, (പൂർണ്ണസംഖ്യ)out.len, out.str));

      /* Unescape the attribute */
      result = Curl_urldecode(out.str, out.len, &unescaped, ശൂന്യ, REJECT_ZERO);
      എങ്കിൽ(result) {
        rc = LDAP_NO_MEMORY;
        പോകുക quit;
      }

#ifdef USE_WIN32_LDAP
      /* Convert the unescaped string to a tchar */
      ludp->lud_attrs[i] = curlx_convert_UTF8_to_tchar(unescaped);

      /* Free the unescaped string as we are done with it */
      curlx_free(unescaped);

      എങ്കിൽ(!ludp->lud_attrs[i]) {
        rc = LDAP_NO_MEMORY;
        പോകുക quit;
      }
#അല്ലെങ്കിൽ
      ludp->lud_attrs[i] = unescaped;
#endif

      ludp->lud_attrs_dups++;
      എങ്കിൽ(curlx_str_single(&atp, ','))
        നിർത്തുക;
    }
  }

  p = q;
  എങ്കിൽ(!p)
    പോകുക quit;

  /* Parse the scope. skip "??" */
  q = strchr(p, '?');
  എങ്കിൽ(q)
    *q++ = '\0';

  എങ്കിൽ(*p) {
    ludp->lud_scope = str2scope(p);
    എങ്കിൽ(ludp->lud_scope == -1) {
      rc = LDAP_INVALID_SYNTAX;

      പോകുക quit;
    }
    LDAP_TRACE(("scope %d\n", ludp->lud_scope));
  }

  p = q;
  എങ്കിൽ(!p)
    പോകുക quit;

  /* Parse the filter */
  q = strchr(p, '?');
  എങ്കിൽ(q)
    *q++ = '\0';

  എങ്കിൽ(*p) {
    അക്ഷരം *filter = p;
    അക്ഷരം *unescaped;
    CURLcode result;

    LDAP_TRACE(("filter '%s'\n", filter));

    /* Unescape the filter */
    result = Curl_urldecode(filter, 0, &unescaped, ശൂന്യ, REJECT_CTRL);
    എങ്കിൽ(result) {
      rc = LDAP_NO_MEMORY;

      പോകുക quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_filter = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    എങ്കിൽ(!ludp->lud_filter) {
      rc = LDAP_NO_MEMORY;

      പോകുക quit;
    }
#അല്ലെങ്കിൽ
    ludp->lud_filter = unescaped;
#endif
  }

  p = q;
  എങ്കിൽ(p && !*p) {
    rc = LDAP_INVALID_SYNTAX;

    പോകുക quit;
  }

quit:
  curlx_free(path);
  curlx_free(query);

  തിരികെ rc;
}

സ്ഥിതം curl_ldap_അക്കം_t ldap_url_parse_താഴെയുള്ള(ഘടന Curl_easy *data,
                                          സ്ഥിരം ഘടന connectdata *conn,
                                          LDAPURLDesc **ludpp)
{
  LDAPURLDesc *ludp = curlx_calloc(1, വലുപ്പം(*ludp));
  curl_ldap_അക്കം_t rc;

  *ludpp = ശൂന്യ;
  എങ്കിൽ(!ludp)
    തിരികെ LDAP_NO_MEMORY;

  rc = ldap_url_parse2_താഴെയുള്ള(data, conn, ludp);
  എങ്കിൽ(rc != LDAP_SUCCESS) {
    ldap_free_urldesc_താഴെയുള്ള(ludp);
    ludp = ശൂന്യ;
  }
  *ludpp = ludp;
  തിരികെ rc;
}

സ്ഥിതം ശൂന്യം ldap_free_urldesc_താഴെയുള്ള(LDAPURLDesc *ludp)
{
  എങ്കിൽ(!ludp)
    തിരികെ;

  curlx_free(ludp->lud_dn);
  curlx_free(ludp->lud_filter);

  എങ്കിൽ(ludp->lud_attrs) {
    size_t i;
    വേണ്ടി(i = 0; i < ludp->lud_attrs_dups; i++) {
      curlx_free(ludp->lud_attrs[i]);
    }
    curlx_free(ludp->lud_attrs);
  }

  curlx_free(ludp);
}
#endif /* !HAVE_LDAP_URL_PARSE */

ശൂന്യം Curl_ldap_version(അക്ഷരം *buf, size_t bufsz)
{
#ifdef USE_WIN32_LDAP
  curl_msnprintf(buf, bufsz, "WinLDAP");
#അല്ലെങ്കിൽ
#ifdef LDAP_OPT_X_TLS_PASSPHRASE
  സ്ഥിതം സ്ഥിരം അക്ഷരം flavor[] = "/Apple";
#അല്ലെങ്കിൽ
  സ്ഥിതം സ്ഥിരം അക്ഷരം flavor[] = "";
#endif
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  /* Comparing against 0, as different platforms
     disagree on the success define name */
  എങ്കിൽ(ldap_get_option(ശൂന്യ, LDAP_OPT_API_INFO, &api) == 0) {
    unsigned പൂർണ്ണസംഖ്യ patch = (unsigned പൂർണ്ണസംഖ്യ)(api.ldapai_vendor_version % 100);
    unsigned പൂർണ്ണസംഖ്യ major = (unsigned പൂർണ്ണസംഖ്യ)(api.ldapai_vendor_version / 10000);
    unsigned പൂർണ്ണസംഖ്യ minor =
      (((unsigned പൂർണ്ണസംഖ്യ)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
#ifdef __OS400__
    curl_msnprintf(buf, bufsz, "IBMLDAP/%u.%u.%u", major, minor, patch);
    ldap_മൂല്യം_free(api.ldapai_extensions);
    (ശൂന്യം)flavor;
#അല്ലെങ്കിൽ
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u%s",
                   api.ldapai_vendor_name, major, minor, patch, flavor);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((ശൂന്യം **)api.ldapai_extensions);
#endif
  }
  അല്ലെങ്കിൽ
    curl_msnprintf(buf, bufsz, "LDAP/1");
#endif /* USE_WIN32_LDAP */
}

/*
 * LDAP protocol handler.
 */
സ്ഥിരം ഘടന Curl_protocol Curl_protocol_ldap = {
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

#എങ്കിൽ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic എടുക്കുക
#endif

#endif /* !CURL_DISABLE_LDAP && !USE_OPENLDAP */
