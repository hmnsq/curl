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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curl_ldap.h"

#ଯଦି !defined(CURL_DISABLE_LDAP) && !defined(USE_OPENLDAP)

#ଯଦି defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ଠେଲ
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
#ଅନ୍ୟଥା
#  define LDAP_DEPRECATED 1     /* Be sure ldap_init() is defined. */
#  ifdef HAVE_LBER_H
#    include <lber.h>
#  endif
#  include <ldap.h>
#  ଯଦି defined(HAVE_LDAP_SSL) && defined(HAVE_LDAP_SSL_H)
#    include <ldap_ssl.h>
#  endif /* HAVE_LDAP_SSL && HAVE_LDAP_SSL_H */
#endif

#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "bufref.h"
#ଅନ୍ତର୍ଭାଵ "curlx/multibyte.h"
#ଅନ୍ତର୍ଭାଵ "curlx/base64.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"

#ifdef USE_WIN32_LDAP
#define FREE_ON_WINLDAP(x) curlx_free(x)
#define curl_ldap_ଅଙ୍କ_t    ULONG
#ଅନ୍ୟଥା
#define FREE_ON_WINLDAP(x) କର {} ଯେତେବେଳେ(0)
#define curl_ldap_ଅଙ୍କ_t    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
#endif

#ifndef HAVE_LDAP_URL_PARSE

/* Use our own implementation. */

ସଂରଚନା ldap_urldesc {
  ଅକ୍ଷର   *lud_host;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା     lud_port;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_dn;
  TCHAR **lud_attrs;
#ଅନ୍ୟଥା
  ଅକ୍ଷର   *lud_dn;
  ଅକ୍ଷର  **lud_attrs;
#endif
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା     lud_scope;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_filter;
#ଅନ୍ୟଥା
  ଅକ୍ଷର   *lud_filter;
#endif
  ଅକ୍ଷର  **lud_exts;
  size_t  lud_attrs_dups; /* how many were dup'ed, this field is not in the
                             "real" struct so can only be used in code
                             without HAVE_LDAP_URL_PARSE defined */
};

#undef LDAPURLDesc
#define LDAPURLDesc ସଂରଚନା ldap_urldesc

ସ୍ଥିର curl_ldap_ଅଙ୍କ_t ldap_url_parse_ତଳ(ସଂରଚନା Curl_easy *data,
                                          ଅଚଳ ସଂରଚନା connectdata *conn,
                                          LDAPURLDesc **ludpp);
ସ୍ଥିର ଶୂନ୍ୟ ldap_free_urldesc_ତଳ(LDAPURLDesc *ludp);

#undef ldap_free_urldesc
#define ldap_free_urldesc       ldap_free_urldesc_ତଳ

#endif /* !HAVE_LDAP_URL_PARSE */

#ifdef DEBUG_LDAP
#define LDAP_TRACE(x)                 \
  କର {                                \
    ldap_trace_ତଳ("%u: ", __LINE__); \
    ldap_trace_ତଳ x;                 \
  } ଯେତେବେଳେ(0)

ସ୍ଥିର ଶୂନ୍ୟ ldap_trace_ତଳ(ଅଚଳ ଅକ୍ଷର *fmt, ...) CURL_PRINTF(1, 2);
#ଅନ୍ୟଥା
#define LDAP_TRACE(x) Curl_nop_stmt
#endif

#ଯଦି defined(USE_WIN32_LDAP) && defined(ldap_err2string)
/* Use ANSI error strings in Unicode builds */
#undef ldap_err2string
#define ldap_err2string ldap_err2stringA
#endif

#ଯଦି defined(USE_WIN32_LDAP) && defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'void *' of greater size */
#undef LDAP_OPT_ON
#undef LDAP_OPT_OFF
#define LDAP_OPT_ON   ((ଶୂନ୍ୟ *)(size_t)1)
#define LDAP_OPT_OFF  ((ଶୂନ୍ୟ *)(size_t)0)
#endif

#ifdef USE_WIN32_LDAP

#ifdef USE_WINDOWS_SSPI
ସ୍ଥିର ULONG ldap_win_bind_auth(LDAP *server, ଅଚଳ ଅକ୍ଷର *user,
                                ଅଚଳ ଅକ୍ଷର *passwd, unsigned long authflags)
{
  ULONG method = 0;
  SEC_WINNT_AUTH_IDENTITY_EX cred;
  ULONG rc = LDAP_AUTH_METHOD_NOT_SUPPORTED;

  memset(&cred, 0, ଆକାର(cred));

#ifdef USE_SPNEGO
  ଯଦି(authflags & CURLAUTH_NEGOTIATE) {
    method = LDAP_AUTH_NEGOTIATE;
  }
  ଅନ୍ୟଥା
#endif
#ifdef USE_NTLM
  ଯଦି(authflags & CURLAUTH_NTLM) {
    method = LDAP_AUTH_NTLM;
  }
  ଅନ୍ୟଥା
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  ଯଦି(authflags & CURLAUTH_DIGEST) {
    method = LDAP_AUTH_DIGEST;
  }
  ଅନ୍ୟଥା
#endif
  {
    /* required anyway if one of upper preprocessor definitions enabled */
  }

  ଯଦି(method && user && passwd) {
    CURLcode result = Curl_create_sspi_identity(user, passwd, &cred);
    ଯଦି(!result) {
      rc = ldap_bind_s(server, ଖାଲି, (TCHAR *)&cred, method);
      Curl_sspi_free_identity(&cred);
    }
    ଅନ୍ୟଥା {
      rc = LDAP_NO_MEMORY;
    }
  }
  ଅନ୍ୟଥା {
    /* proceed with current user credentials */
    method = LDAP_AUTH_NEGOTIATE;
    rc = ldap_bind_s(server, ଖାଲି, ଖାଲି, method);
  }
  ଫେରନ୍ତୁ rc;
}
#endif /* USE_WINDOWS_SSPI */

ସ୍ଥିର ULONG ldap_win_bind(ସଂରଚନା Curl_easy *data, LDAP *server,
                           ଅଚଳ ଅକ୍ଷର *user, ଅଚଳ ଅକ୍ଷର *passwd)
{
  ULONG rc = LDAP_INVALID_CREDENTIALS;

  PTCHAR inuser = ଖାଲି;
  PTCHAR inpass = ଖାଲି;

  ଯଦି(user && passwd && (data->set.httpauth & CURLAUTH_BASIC)) {
    inuser = curlx_convert_UTF8_to_tchar(user);
    inpass = curlx_convert_UTF8_to_tchar(passwd);

    rc = ldap_simple_bind_s(server, inuser, inpass);

    curlx_free(inuser);
    curlx_free(inpass);
  }
#ifdef USE_WINDOWS_SSPI
  ଅନ୍ୟଥା {
    rc = ldap_win_bind_auth(server, user, passwd, data->set.httpauth);
  }
#endif

  ଫେରନ୍ତୁ rc;
}
#endif /* USE_WIN32_LDAP */

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
ସ୍ଥିର bool ldap_str_needs_base64(ଅଚଳ ଅକ୍ଷର *s, size_t len)
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

ସ୍ଥିର bool ldap_ମୂଲ୍ୟ_needs_base64(ଅଚଳ ଅକ୍ଷର *attr, size_t attr_len,
                                    ଅଚଳ Ber_ମୂଲ୍ୟ *val)
{
  ଯଦି((attr_len > 7) && curl_strequal(";binary", attr + attr_len - 7))
    ଫେରନ୍ତୁ TRUE;

  ଫେରନ୍ତୁ ldap_str_needs_base64(val->bv_val, val->bv_len);
}

#ifdef USE_WIN32_LDAP
ସ୍ଥିର BOOLEAN bypass_cert_verify(PLDAP Connection,
                                  PCCERT_CONTEXT *ppServerCert)
{
  (ଶୂନ୍ୟ)Connection;
  CertFreeCertificateContext(*ppServerCert);
  /* approve any certificate since the verification is set to bypass */
  ଫେରନ୍ତୁ TRUE;
}
#endif

ସ୍ଥିର CURLcode show_vals(ସଂରଚନା Curl_easy *data, Ber_ମୂଲ୍ୟ **vals,
                          ଅଚଳ ଅକ୍ଷର *attr)
{
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  size_t attr_len = strlen(attr);

  /* An attribute type has no base64 form in LDIF, so a type carrying control
     bytes (CR/LF etc.) would let a hostile server inject lines. A conformant
     type name never contains them, so reject it. */
  ଯଦି(ldap_str_needs_base64(attr, attr_len)) {
    failf(data, "LDAP local: attribute type contains illegal bytes");
    ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
  }

  ପାଇଁ(i = 0; vals[i] && !result; i++) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, "\t", 1);
    ଯଦି(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, attr, attr_len);
    ଯଦି(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, ":", 1);

    ଯଦି(result)
      ଭାଙ୍ଗ;

    ଯଦି(ldap_ମୂଲ୍ୟ_needs_base64(attr, attr_len, vals[i])) {
      ଅକ୍ଷର *val_b64 = ଖାଲି;
      size_t val_b64_sz = 0;

      /* Binary attribute, encode to base64. */
      ଯଦି(vals[i]->bv_len)
        result = curlx_base64_encode((uint8_t *)vals[i]->bv_val,
                                     vals[i]->bv_len,
                                     &val_b64, &val_b64_sz);
      ଯଦି(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, ": ", 2);
      ଯଦି(!result && val_b64_sz)
        result = Curl_client_write(data, CLIENTWRITE_BODY, val_b64,
                                   val_b64_sz);
      curlx_free(val_b64);
    }
    ଅନ୍ୟଥା {
      result = Curl_client_write(data, CLIENTWRITE_BODY, " ", 1);
      ଯଦି(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   vals[i]->bv_val, vals[i]->bv_len);
    }

    ଯଦି(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ldap_do(ସଂରଚନା Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  curl_ldap_ଅଙ୍କ_t rc = LDAP_SUCCESS;
  LDAP *server = ଖାଲି;
  LDAPURLDesc *ludp = ଖାଲି;
  LDAPMessage *ldapmsg = ଖାଲି;
  LDAPMessage *entryIterator;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ = 0;
  ସଂରଚନା connectdata *conn = data->conn;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ldap_proto = LDAP_VERSION3;
  bool ldap_ssl = FALSE;
#ifdef LDAP_OPT_NETWORK_TIMEOUT
  ସଂରଚନା timeval ldap_timeout = { 10, 0 }; /* 10s connection/search timeout */
#endif
#ifdef USE_WIN32_LDAP
  TCHAR *host = ଖାଲି;
#ଅନ୍ୟଥା
  ଅକ୍ଷର *host = ଖାଲି;
#endif
  ଅଚଳ ଅକ୍ଷର *user = data->state.creds ? data->state.creds->user : ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *passwd = data->state.creds ? data->state.creds->passwd : ଖାଲି;
  ସଂରଚନା ip_quadruple ipquad;
  bool is_ipv6;
  BerElement *ber = ଖାଲି;

  *done = TRUE; /* unconditionally */
  infof(data, "LDAP local: LDAP Vendor = %s ; LDAP Version = %d",
        LDAP_VENDOR_NAME, LDAP_VENDOR_VERSION);
  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

#ifdef HAVE_LDAP_URL_PARSE
  rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), &ludp);
#ଅନ୍ୟଥା
  rc = ldap_url_parse_ତଳ(data, conn, &ludp);
#endif
  ଯଦି(rc) {
    failf(data, "Bad LDAP URL: %s", ldap_err2string(rc));
    result = CURLE_URL_MALFORMAT;
    ଯାଅ quit;
  }

  result = Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad);
  ଯଦି(result)
    ଯାଅ quit;

  /* Get the URL scheme (either ldap or ldaps) */
  ldap_ssl = Curl_conn_is_ssl(conn, FIRSTSOCKET);
  infof(data, "LDAP local: trying to establish %s connection",
        ldap_ssl ? "encrypted" : "cleartext");

#ifdef USE_WIN32_LDAP
  host = curlx_convert_UTF8_to_tchar(conn->origin->hostname);
  ଯଦି(!host) {
    result = CURLE_OUT_OF_MEMORY;

    ଯାଅ quit;
  }
#ଅନ୍ୟଥା
  host = conn->origin->hostname;
#endif

#ifdef USE_WIN32_LDAP
  ଯଦି(ldap_ssl)
    server = ldap_sslinit(host, (curl_ldap_ଅଙ୍କ_t)ipquad.remote_port, 1);
  ଅନ୍ୟଥା
#endif
    server = ldap_init(host, (curl_ldap_ଅଙ୍କ_t)ipquad.remote_port);
  ଯଦି(!server) {
    failf(data, "LDAP: cannot setup connect to %s:%u",
          conn->origin->user_hostname, ipquad.remote_port);
    result = CURLE_COULDNT_CONNECT;
    ଯାଅ quit;
  }

#ifdef LDAP_OPT_NETWORK_TIMEOUT
  ldap_set_option(server, LDAP_OPT_NETWORK_TIMEOUT, &ldap_timeout);
#endif
  ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);

  /* Do not chase referrals. */
  ldap_set_option(server, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  ଯଦି(ldap_ssl) {
#ifdef HAVE_LDAP_SSL
#ifdef USE_WIN32_LDAP
    /* Win32 LDAP uses the Windows CA store to verify certificates */
    ldap_set_option(server, LDAP_OPT_SSL, LDAP_OPT_ON);
    ଯଦି(!conn->ssl_config.verifypeer) {
      ଯଦି(conn->ssl_config.verifyhost) {
        failf(data, "LDAP local: host verification cannot be enabled when "
              "peer verification is disabled for Windows native LDAP");
        result = CURLE_NOT_BUILT_IN;
        ଯାଅ quit;
      }
      ldap_set_option(server, LDAP_OPT_SERVER_CERTIFICATE,
                      (ଶୂନ୍ୟ *)(uintptr_t)bypass_cert_verify);
    }
#ନଚେତଯଦି defined(LDAP_OPT_X_TLS) /* !USE_WIN32_LDAP */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ldap_option;
    ଅଚଳ ଅକ୍ଷର *ldap_ca = conn->ssl_config.CAfile;
    ଯଦି(conn->ssl_config.verifypeer) {
      /* OpenLDAP SDK supports BASE64 files. */
      ଯଦି(conn->ssl_config.cert_type &&
         !curl_strequal(conn->ssl_config.cert_type, "PEM")) {
        failf(data, "LDAP local: ERROR OpenLDAP only supports PEM cert-type");
        result = CURLE_SSL_CERTPROBLEM;
        ଯାଅ quit;
      }
      ଯଦି(!ldap_ca) {
        failf(data, "LDAP local: ERROR PEM CA cert not set");
        result = CURLE_SSL_CERTPROBLEM;
        ଯାଅ quit;
      }
      infof(data, "LDAP local: using PEM CA cert: %s", ldap_ca);
      rc = ldap_set_option(server, LDAP_OPT_X_TLS_CACERTFILE, ldap_ca);
      ଯଦି(rc != LDAP_SUCCESS) {
        failf(data, "LDAP local: ERROR setting PEM CA cert: %s",
              ldap_err2string(rc));
        result = CURLE_SSL_CERTPROBLEM;
        ଯାଅ quit;
      }
      ldap_option = LDAP_OPT_X_TLS_DEMAND;
    }
    ଅନ୍ୟଥା
      ldap_option = LDAP_OPT_X_TLS_NEVER;

    rc = ldap_set_option(server, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_option);
    ଯଦି(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting cert verify mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      ଯାଅ quit;
    }
    ldap_option = LDAP_OPT_X_TLS_HARD;
    rc = ldap_set_option(server, LDAP_OPT_X_TLS, &ldap_option);
    ଯଦି(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting SSL/TLS mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      ଯାଅ quit;
    }
#ଅନ୍ୟଥା /* !USE_WIN32_LDAP && !LDAP_OPT_X_TLS */
    /* we should probably never come up to here since configure
       should check in first place if we can support LDAP SSL/TLS */
    failf(data, "LDAP local: SSL/TLS not supported with this version "
          "of the OpenLDAP toolkit");
    result = CURLE_SSL_CERTPROBLEM;
    ଯାଅ quit;
#endif
#endif /* HAVE_LDAP_SSL */
  }
  ଅନ୍ୟଥା ଯଦି(data->set.use_ssl > CURLUSESSL_TRY) {
    failf(data, "LDAP local: explicit TLS not supported");
    result = CURLE_NOT_BUILT_IN;
    ଯାଅ quit;
  }

#ifdef USE_WIN32_LDAP
  rc = ldap_win_bind(data, server, user, passwd);
#ଅନ୍ୟଥା
  rc = ldap_simple_bind_s(server, user, passwd);
#endif
  ଯଦି(!ldap_ssl && rc != LDAP_SUCCESS) {
    ldap_proto = LDAP_VERSION2;
    ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);
#ifdef USE_WIN32_LDAP
    rc = ldap_win_bind(data, server, user, passwd);
#ଅନ୍ୟଥା
    rc = ldap_simple_bind_s(server, user, passwd);
#endif
  }
  ଯଦି(rc != LDAP_SUCCESS) {
#ifdef USE_WIN32_LDAP
    failf(data, "LDAP local: bind via ldap_win_bind %s",
          ldap_err2string(rc));
#ଅନ୍ୟଥା
    failf(data, "LDAP local: bind via ldap_simple_bind_s %s",
          ldap_err2string(rc));
#endif
    result = CURLE_LDAP_CANNOT_BIND;
    ଯାଅ quit;
  }

  Curl_pgrsReset(data);
  rc = ldap_ସନ୍ଧାନ_s(server, ludp->lud_dn,
                     ludp->lud_scope,
                     ludp->lud_filter, ludp->lud_attrs, 0, &ldapmsg);

  ଯଦି(rc != LDAP_SUCCESS && rc != LDAP_SIZELIMIT_EXCEEDED) {
    failf(data, "LDAP remote: %s", ldap_err2string(rc));
    result = CURLE_LDAP_ସନ୍ଧାନ_FAILED;
    ଯାଅ quit;
  }

  ଅଙ୍କ = 0;
  ପାଇଁ(entryIterator = ldap_first_entry(server, ldapmsg);
      entryIterator;
      entryIterator = ldap_next_entry(server, entryIterator), ଅଙ୍କ++) {
#ifdef USE_WIN32_LDAP
    TCHAR *attribute;
#ଅନ୍ୟଥା
    ଅକ୍ଷର *attribute;
#endif

    /* Get the DN and write it to the client */
    {
      ଅକ୍ଷର *name = ଖାଲି;
      size_t name_len = 0;
#ifdef USE_WIN32_LDAP
      TCHAR *dn = ldap_get_dn(server, entryIterator);
      ଯଦି(dn)
        name = curlx_convert_tchar_to_UTF8(dn);
#ଅନ୍ୟଥା
      ଅକ୍ଷର *dn = name = ldap_get_dn(server, entryIterator);
#endif
      ଯଦି(!name)
        result = dn ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
      ଅନ୍ୟଥା {
        name_len = strlen(name);
        ଯଦି(ldap_str_needs_base64(name, name_len)) {
          ଅକ୍ଷର *dn_b64 = ଖାଲି;
          size_t dn_b64_sz = 0;

          /* A DN with e.g. CR/LF would inject LDIF lines, so encode it */
          result = curlx_base64_encode((uint8_t *)name, name_len,
                                       &dn_b64, &dn_b64_sz);
          ଯଦି(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, "DN:: ", 5);
          ଯଦି(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, dn_b64,
                                       dn_b64_sz);
          curlx_free(dn_b64);
        }
        ଅନ୍ୟଥା {
          result = Curl_client_write(data, CLIENTWRITE_BODY, "DN: ", 4);
          ଯଦି(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, name, name_len);
        }
        ଯଦି(!result)
          result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      }
      FREE_ON_WINLDAP(name);
      ldap_memfree(dn);
      ଯଦି(result)
        ଯାଅ quit;
    }

    /* Get the attributes and write them to the client */
    ପାଇଁ(attribute = ldap_first_attribute(server, entryIterator, &ber);
        attribute;
        attribute = ldap_next_attribute(server, entryIterator, ber)) {
      Ber_ମୂଲ୍ୟ **vals;
#ifdef USE_WIN32_LDAP
      ଅକ୍ଷର *attr = curlx_convert_tchar_to_UTF8(attribute);
      ଯଦି(!attr) {
        ldap_memfree(attribute);
        result = CURLE_OUT_OF_MEMORY;
        ଯାଅ quit;
      }
#ଅନ୍ୟଥା
      ଅକ୍ଷର *attr = attribute;
#endif
      vals = ldap_get_ମୂଲ୍ୟଗୁଡ଼ିକ_len(server, entryIterator, attribute);
      ଯଦି(vals) {
        result = show_vals(data, vals, attr);
        /* Free memory used to store values */
        ldap_ମୂଲ୍ୟ_free_len(vals);
      }

      /* Free the attribute as we are done with it */
      FREE_ON_WINLDAP(attr);
      ldap_memfree(attribute);

      ଯଦି(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      ଯଦି(result)
        ଯାଅ quit;
    }

    ଯଦି(ber) {
      ber_free(ber, 0);
      ber = ଖାଲି;
    }
  }

quit:
  ଯଦି(ber)
    ber_free(ber, 0);
  ଯଦି(ldapmsg) {
    ldap_msgfree(ldapmsg);
    LDAP_TRACE(("Received %d entries\n", ଅଙ୍କ));
  }
  ଯଦି(rc == LDAP_SIZELIMIT_EXCEEDED)
    infof(data, "There are more than %d entries", ଅଙ୍କ);
  ଯଦି(ludp)
    ldap_free_urldesc(ludp);
  ଯଦି(server)
    ldap_unbind_s(server);

  FREE_ON_WINLDAP(host);

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  connclose(conn);

  ଫେରନ୍ତୁ result;
}

#ifdef DEBUG_LDAP
ସ୍ଥିର ଶୂନ୍ୟ ldap_trace_ତଳ(ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_trace = -1;
  va_list args;

  ଯଦି(do_trace == -1) {
    ଅଚଳ ଅକ୍ଷର *env = getenv("CURL_TRACE");
    curl_off_t e = 0;
    ଯଦି(!curlx_str_ସଂଖ୍ୟା(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  ଯଦି(!do_trace)
    ଫେରନ୍ତୁ;

  va_start(args, fmt);
  curl_mvfprintf(stderr, fmt, args);
  va_end(args);
}
#endif /* DEBUG_LDAP */

#ifndef HAVE_LDAP_URL_PARSE

/*
 * Return scope-value for a scope-string.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା str2scope(ଅଚଳ ଅକ୍ଷର *p)
{
  ଯଦି(curl_strequal(p, "one"))
    ଫେରନ୍ତୁ LDAP_SCOPE_ONELEVEL;
  ଯଦି(curl_strequal(p, "onetree"))
    ଫେରନ୍ତୁ LDAP_SCOPE_ONELEVEL;
  ଯଦି(curl_strequal(p, "base"))
    ଫେରନ୍ତୁ LDAP_SCOPE_BASE;
  ଯଦି(curl_strequal(p, "sub"))
    ଫେରନ୍ତୁ LDAP_SCOPE_SUBTREE;
  ଯଦି(curl_strequal(p, "subtree"))
    ଫେରନ୍ତୁ LDAP_SCOPE_SUBTREE;
  ଫେରନ୍ତୁ -1;
}

/* number of entries in the attributes list */
ସ୍ଥିର size_t ଅଙ୍କ_entries(ଅଚଳ ଅକ୍ଷର *s)
{
  size_t ବସ୍ତୁଗୁଡ଼ିକ = 1;

  s = strchr(s, ',');
  ଯେତେବେଳେ(s) {
    ବସ୍ତୁଗୁଡ଼ିକ++;
    s = strchr(s + 1, ',');
  }
  ଫେରନ୍ତୁ ବସ୍ତୁଗୁଡ଼ିକ;
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
ସ୍ଥିର curl_ldap_ଅଙ୍କ_t ldap_url_parse2_ତଳ(ସଂରଚନା Curl_easy *data,
                                           ଅଚଳ ସଂରଚନା connectdata *conn,
                                           LDAPURLDesc *ludp)
{
  curl_ldap_ଅଙ୍କ_t rc = LDAP_SUCCESS;
  ଅକ୍ଷର *p;
  ଅକ୍ଷର *path;
  ଅକ୍ଷର *q = ଖାଲି;
  ଅକ୍ଷର *query = ଖାଲି;
  size_t i;

  ଯଦି(!data ||
     !data->state.up.path ||
     data->state.up.path[0] != '/' ||
     ((data->state.origin->scheme != &Curl_scheme_ldap) &&
      (data->state.origin->scheme != &Curl_scheme_ldaps)))
    ଫେରନ୍ତୁ LDAP_INVALID_SYNTAX;

  ludp->lud_scope = LDAP_SCOPE_BASE;
  ludp->lud_port  = conn->origin->port;
  ludp->lud_host  = conn->origin->hostname;

  /* Duplicate the path */
  p = path = curlx_strdup(data->state.up.path + 1);
  ଯଦି(!path)
    ଫେରନ୍ତୁ LDAP_NO_MEMORY;

  /* Duplicate the query if present */
  ଯଦି(data->state.up.query) {
    q = query = curlx_strdup(data->state.up.query);
    ଯଦି(!query) {
      curlx_free(path);
      ଫେରନ୍ତୁ LDAP_NO_MEMORY;
    }
  }

  /* Parse the DN (Distinguished Name) */
  ଯଦି(*p) {
    ଅକ୍ଷର *dn = p;
    ଅକ୍ଷର *unescaped;
    CURLcode result;

    LDAP_TRACE(("DN '%s'\n", dn));

    /* Unescape the DN */
    result = Curl_urldecode(dn, 0, &unescaped, ଖାଲି, REJECT_ZERO);
    ଯଦି(result) {
      rc = LDAP_NO_MEMORY;

      ଯାଅ quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_dn = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    ଯଦି(!ludp->lud_dn) {
      rc = LDAP_NO_MEMORY;

      ଯାଅ quit;
    }
#ଅନ୍ୟଥା
    ludp->lud_dn = unescaped;
#endif
  }

  p = q;
  ଯଦି(!p)
    ଯାଅ quit;

  /* Parse the attributes. skip "??" */
  q = strchr(p, '?');
  ଯଦି(q)
    *q++ = '\0';

  ଯଦି(*p) {
    size_t ଗଣନା_2 = ଅଙ୍କ_entries(p); /* at least one */
    ଅଚଳ ଅକ୍ଷର *atp = p;

    /* Allocate our array (+1 for the NULL entry) */
#ifdef USE_WIN32_LDAP
    ludp->lud_attrs = curlx_calloc(ଗଣନା_2 + 1, ଆକାର(TCHAR *));
#ଅନ୍ୟଥା
    ludp->lud_attrs = curlx_calloc(ଗଣନା_2 + 1, ଆକାର(ଅକ୍ଷର *));
#endif
    ଯଦି(!ludp->lud_attrs) {
      rc = LDAP_NO_MEMORY;
      ଯାଅ quit;
    }

    ପାଇଁ(i = 0; i < ଗଣନା_2; i++) {
      ଅକ୍ଷର *unescaped;
      CURLcode result;
      ସଂରଚନା Curl_str out;

      ଯଦି(curlx_str_until(&atp, &out, 1024, ','))
        ଭାଙ୍ଗ;

      LDAP_TRACE(("attr[%zu] '%.*s'\n", i, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)out.len, out.str));

      /* Unescape the attribute */
      result = Curl_urldecode(out.str, out.len, &unescaped, ଖାଲି, REJECT_ZERO);
      ଯଦି(result) {
        rc = LDAP_NO_MEMORY;
        ଯାଅ quit;
      }

#ifdef USE_WIN32_LDAP
      /* Convert the unescaped string to a tchar */
      ludp->lud_attrs[i] = curlx_convert_UTF8_to_tchar(unescaped);

      /* Free the unescaped string as we are done with it */
      curlx_free(unescaped);

      ଯଦି(!ludp->lud_attrs[i]) {
        rc = LDAP_NO_MEMORY;
        ଯାଅ quit;
      }
#ଅନ୍ୟଥା
      ludp->lud_attrs[i] = unescaped;
#endif

      ludp->lud_attrs_dups++;
      ଯଦି(curlx_str_single(&atp, ','))
        ଭାଙ୍ଗ;
    }
  }

  p = q;
  ଯଦି(!p)
    ଯାଅ quit;

  /* Parse the scope. skip "??" */
  q = strchr(p, '?');
  ଯଦି(q)
    *q++ = '\0';

  ଯଦି(*p) {
    ludp->lud_scope = str2scope(p);
    ଯଦି(ludp->lud_scope == -1) {
      rc = LDAP_INVALID_SYNTAX;

      ଯାଅ quit;
    }
    LDAP_TRACE(("scope %d\n", ludp->lud_scope));
  }

  p = q;
  ଯଦି(!p)
    ଯାଅ quit;

  /* Parse the filter */
  q = strchr(p, '?');
  ଯଦି(q)
    *q++ = '\0';

  ଯଦି(*p) {
    ଅକ୍ଷର *filter = p;
    ଅକ୍ଷର *unescaped;
    CURLcode result;

    LDAP_TRACE(("filter '%s'\n", filter));

    /* Unescape the filter */
    result = Curl_urldecode(filter, 0, &unescaped, ଖାଲି, REJECT_CTRL);
    ଯଦି(result) {
      rc = LDAP_NO_MEMORY;

      ଯାଅ quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_filter = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    ଯଦି(!ludp->lud_filter) {
      rc = LDAP_NO_MEMORY;

      ଯାଅ quit;
    }
#ଅନ୍ୟଥା
    ludp->lud_filter = unescaped;
#endif
  }

  p = q;
  ଯଦି(p && !*p) {
    rc = LDAP_INVALID_SYNTAX;

    ଯାଅ quit;
  }

quit:
  curlx_free(path);
  curlx_free(query);

  ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର curl_ldap_ଅଙ୍କ_t ldap_url_parse_ତଳ(ସଂରଚନା Curl_easy *data,
                                          ଅଚଳ ସଂରଚନା connectdata *conn,
                                          LDAPURLDesc **ludpp)
{
  LDAPURLDesc *ludp = curlx_calloc(1, ଆକାର(*ludp));
  curl_ldap_ଅଙ୍କ_t rc;

  *ludpp = ଖାଲି;
  ଯଦି(!ludp)
    ଫେରନ୍ତୁ LDAP_NO_MEMORY;

  rc = ldap_url_parse2_ତଳ(data, conn, ludp);
  ଯଦି(rc != LDAP_SUCCESS) {
    ldap_free_urldesc_ତଳ(ludp);
    ludp = ଖାଲି;
  }
  *ludpp = ludp;
  ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ଶୂନ୍ୟ ldap_free_urldesc_ତଳ(LDAPURLDesc *ludp)
{
  ଯଦି(!ludp)
    ଫେରନ୍ତୁ;

  curlx_free(ludp->lud_dn);
  curlx_free(ludp->lud_filter);

  ଯଦି(ludp->lud_attrs) {
    size_t i;
    ପାଇଁ(i = 0; i < ludp->lud_attrs_dups; i++) {
      curlx_free(ludp->lud_attrs[i]);
    }
    curlx_free(ludp->lud_attrs);
  }

  curlx_free(ludp);
}
#endif /* !HAVE_LDAP_URL_PARSE */

ଶୂନ୍ୟ Curl_ldap_version(ଅକ୍ଷର *buf, size_t bufsz)
{
#ifdef USE_WIN32_LDAP
  curl_msnprintf(buf, bufsz, "WinLDAP");
#ଅନ୍ୟଥା
#ifdef LDAP_OPT_X_TLS_PASSPHRASE
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର flavor[] = "/Apple";
#ଅନ୍ୟଥା
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର flavor[] = "";
#endif
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  /* Comparing against 0, as different platforms
     disagree on the success define name */
  ଯଦି(ldap_get_option(ଖାଲି, LDAP_OPT_API_INFO, &api) == 0) {
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା patch = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(api.ldapai_vendor_version % 100);
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା major = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(api.ldapai_vendor_version / 10000);
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା minor =
      (((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
#ifdef __OS400__
    curl_msnprintf(buf, bufsz, "IBMLDAP/%u.%u.%u", major, minor, patch);
    ldap_ମୂଲ୍ୟ_free(api.ldapai_extensions);
    (ଶୂନ୍ୟ)flavor;
#ଅନ୍ୟଥା
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u%s",
                   api.ldapai_vendor_name, major, minor, patch, flavor);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((ଶୂନ୍ୟ **)api.ldapai_extensions);
#endif
  }
  ଅନ୍ୟଥା
    curl_msnprintf(buf, bufsz, "LDAP/1");
#endif /* USE_WIN32_LDAP */
}

/*
 * LDAP protocol handler.
 */
ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_ldap = {
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

#ଯଦି defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic କାଢ଼
#endif

#endif /* !CURL_DISABLE_LDAP && !USE_OPENLDAP */
