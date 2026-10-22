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
#অন্তর্ভাব "curl_setup.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_ldap.h"

#যদি !defined(CURL_DISABLE_LDAP) && !defined(USE_OPENLDAP)

#যদি defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ঠেলো
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
#অন্যথা
#  define LDAP_DEPRECATED 1     /* Be sure ldap_init() is defined. */
#  ifdef HAVE_LBER_H
#    include <lber.h>
#  endif
#  include <ldap.h>
#  যদি defined(HAVE_LDAP_SSL) && defined(HAVE_LDAP_SSL_H)
#    include <ldap_ssl.h>
#  endif /* HAVE_LDAP_SSL && HAVE_LDAP_SSL_H */
#endif

#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "bufref.h"
#অন্তর্ভাব "curlx/multibyte.h"
#অন্তর্ভাব "curlx/base64.h"
#অন্তর্ভাব "connect.h"

#ifdef USE_WIN32_LDAP
#define FREE_ON_WINLDAP(x) curlx_free(x)
#define curl_ldap_অঙ্ক_t    ULONG
#অন্যথা
#define FREE_ON_WINLDAP(x) করো {} যখন(0)
#define curl_ldap_অঙ্ক_t    পূর্ণসংখ্যা
#endif

#ifndef HAVE_LDAP_URL_PARSE

/* Use our own implementation. */

কাঠামো ldap_urldesc {
  অক্ষর   *lud_host;
  পূর্ণসংখ্যা     lud_port;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_dn;
  TCHAR **lud_attrs;
#অন্যথা
  অক্ষর   *lud_dn;
  অক্ষর  **lud_attrs;
#endif
  পূর্ণসংখ্যা     lud_scope;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_filter;
#অন্যথা
  অক্ষর   *lud_filter;
#endif
  অক্ষর  **lud_exts;
  size_t  lud_attrs_dups; /* how many were dup'ed, this field is not in the
                             "real" struct so can only be used in code
                             without HAVE_LDAP_URL_PARSE defined */
};

#undef LDAPURLDesc
#define LDAPURLDesc কাঠামো ldap_urldesc

স্থির curl_ldap_অঙ্ক_t ldap_url_parse_নিচের(কাঠামো Curl_easy *তথ্য,
                                          ধ্রুবক কাঠামো connectdata *conn,
                                          LDAPURLDesc **ludpp);
স্থির শূন্য ldap_free_urldesc_নিচের(LDAPURLDesc *ludp);

#undef ldap_free_urldesc
#define ldap_free_urldesc       ldap_free_urldesc_নিচের

#endif /* !HAVE_LDAP_URL_PARSE */

#ifdef DEBUG_LDAP
#define LDAP_TRACE(x)                 \
  করো {                                \
    ldap_trace_নিচের("%u: ", __LINE__); \
    ldap_trace_নিচের x;                 \
  } যখন(0)

স্থির শূন্য ldap_trace_নিচের(ধ্রুবক অক্ষর *fmt, ...) CURL_PRINTF(1, 2);
#অন্যথা
#define LDAP_TRACE(x) Curl_nop_stmt
#endif

#যদি defined(USE_WIN32_LDAP) && defined(ldap_err2string)
/* Use ANSI error strings in Unicode builds */
#undef ldap_err2string
#define ldap_err2string ldap_err2stringA
#endif

#যদি defined(USE_WIN32_LDAP) && defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'void *' of greater size */
#undef LDAP_OPT_ON
#undef LDAP_OPT_OFF
#define LDAP_OPT_ON   ((শূন্য *)(size_t)1)
#define LDAP_OPT_OFF  ((শূন্য *)(size_t)0)
#endif

#ifdef USE_WIN32_LDAP

#ifdef USE_WINDOWS_SSPI
স্থির ULONG ldap_win_bind_auth(LDAP *server, ধ্রুবক অক্ষর *user,
                                ধ্রুবক অক্ষর *passwd, unsigned long authflags)
{
  ULONG method = 0;
  SEC_WINNT_AUTH_IDENTITY_EX cred;
  ULONG rc = LDAP_AUTH_METHOD_NOT_SUPPORTED;

  memset(&cred, 0, আকার(cred));

#ifdef USE_SPNEGO
  যদি(authflags & CURLAUTH_NEGOTIATE) {
    method = LDAP_AUTH_NEGOTIATE;
  }
  অন্যথা
#endif
#ifdef USE_NTLM
  যদি(authflags & CURLAUTH_NTLM) {
    method = LDAP_AUTH_NTLM;
  }
  অন্যথা
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  যদি(authflags & CURLAUTH_DIGEST) {
    method = LDAP_AUTH_DIGEST;
  }
  অন্যথা
#endif
  {
    /* required anyway if one of upper preprocessor definitions enabled */
  }

  যদি(method && user && passwd) {
    CURLcode ফলাফল = Curl_create_sspi_identity(user, passwd, &cred);
    যদি(!ফলাফল) {
      rc = ldap_bind_s(server, ফাঁকা, (TCHAR *)&cred, method);
      Curl_sspi_free_identity(&cred);
    }
    অন্যথা {
      rc = LDAP_NO_MEMORY;
    }
  }
  অন্যথা {
    /* proceed with current user credentials */
    method = LDAP_AUTH_NEGOTIATE;
    rc = ldap_bind_s(server, ফাঁকা, ফাঁকা, method);
  }
  ফেরত rc;
}
#endif /* USE_WINDOWS_SSPI */

স্থির ULONG ldap_win_bind(কাঠামো Curl_easy *তথ্য, LDAP *server,
                           ধ্রুবক অক্ষর *user, ধ্রুবক অক্ষর *passwd)
{
  ULONG rc = LDAP_INVALID_CREDENTIALS;

  PTCHAR inuser = ফাঁকা;
  PTCHAR inpass = ফাঁকা;

  যদি(user && passwd && (তথ্য->set.httpauth & CURLAUTH_BASIC)) {
    inuser = curlx_convert_UTF8_to_tchar(user);
    inpass = curlx_convert_UTF8_to_tchar(passwd);

    rc = ldap_simple_bind_s(server, inuser, inpass);

    curlx_free(inuser);
    curlx_free(inpass);
  }
#ifdef USE_WINDOWS_SSPI
  অন্যথা {
    rc = ldap_win_bind_auth(server, user, passwd, তথ্য->set.httpauth);
  }
#endif

  ফেরত rc;
}
#endif /* USE_WIN32_LDAP */

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
স্থির bool ldap_str_needs_base64(ধ্রুবক অক্ষর *s, size_t len)
{
  size_t i;
  যদি(!len)
    ফেরত FALSE;
  যদি((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    ফেরত TRUE;
  জন্য(i = 0; i < len; i++)
    যদি(!ISPRINT(s[i]))
      ফেরত TRUE;
  ফেরত FALSE;
}

স্থির bool ldap_মান_needs_base64(ধ্রুবক অক্ষর *attr, size_t attr_len,
                                    ধ্রুবক Ber_মান *val)
{
  যদি((attr_len > 7) && curl_strequal(";binary", attr + attr_len - 7))
    ফেরত TRUE;

  ফেরত ldap_str_needs_base64(val->bv_val, val->bv_len);
}

#ifdef USE_WIN32_LDAP
স্থির BOOLEAN bypass_cert_verify(PLDAP Connection,
                                  PCCERT_CONTEXT *ppServerCert)
{
  (শূন্য)Connection;
  CertFreeCertificateContext(*ppServerCert);
  /* approve any certificate since the verification is set to bypass */
  ফেরত TRUE;
}
#endif

স্থির CURLcode show_vals(কাঠামো Curl_easy *তথ্য, Ber_মান **vals,
                          ধ্রুবক অক্ষর *attr)
{
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা i;
  size_t attr_len = strlen(attr);

  /* An attribute type has no base64 form in LDIF, so a type carrying control
     bytes (CR/LF etc.) would let a hostile server inject lines. A conformant
     type name never contains them, so reject it. */
  যদি(ldap_str_needs_base64(attr, attr_len)) {
    failf(তথ্য, "LDAP local: attribute type contains illegal bytes");
    ফেরত CURLE_WEIRD_SERVER_REPLY;
  }

  জন্য(i = 0; vals[i] && !ফলাফল; i++) {
    ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, "\t", 1);
    যদি(!ফলাফল)
      ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, attr, attr_len);
    যদি(!ফলাফল)
      ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, ":", 1);

    যদি(ফলাফল)
      বিরতি;

    যদি(ldap_মান_needs_base64(attr, attr_len, vals[i])) {
      অক্ষর *val_b64 = ফাঁকা;
      size_t val_b64_sz = 0;

      /* Binary attribute, encode to base64. */
      যদি(vals[i]->bv_len)
        ফলাফল = curlx_base64_encode((uint8_t *)vals[i]->bv_val,
                                     vals[i]->bv_len,
                                     &val_b64, &val_b64_sz);
      যদি(!ফলাফল)
        ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, ": ", 2);
      যদি(!ফলাফল && val_b64_sz)
        ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, val_b64,
                                   val_b64_sz);
      curlx_free(val_b64);
    }
    অন্যথা {
      ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, " ", 1);
      যদি(!ফলাফল)
        ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY,
                                   vals[i]->bv_val, vals[i]->bv_len);
    }

    যদি(!ফলাফল)
      ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, "\n", 1);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode ldap_do(কাঠামো Curl_easy *তথ্য, bool *done)
{
  CURLcode ফলাফল = CURLE_OK;
  curl_ldap_অঙ্ক_t rc = LDAP_SUCCESS;
  LDAP *server = ফাঁকা;
  LDAPURLDesc *ludp = ফাঁকা;
  LDAPMessage *ldapmsg = ফাঁকা;
  LDAPMessage *entryIterator;
  পূর্ণসংখ্যা অঙ্ক = 0;
  কাঠামো connectdata *conn = তথ্য->conn;
  পূর্ণসংখ্যা ldap_proto = LDAP_VERSION3;
  bool ldap_ssl = FALSE;
#ifdef LDAP_OPT_NETWORK_TIMEOUT
  কাঠামো timeval ldap_timeout = { 10, 0 }; /* 10s connection/search timeout */
#endif
#ifdef USE_WIN32_LDAP
  TCHAR *host = ফাঁকা;
#অন্যথা
  অক্ষর *host = ফাঁকা;
#endif
  ধ্রুবক অক্ষর *user = তথ্য->state.creds ? তথ্য->state.creds->user : ফাঁকা;
  ধ্রুবক অক্ষর *passwd = তথ্য->state.creds ? তথ্য->state.creds->passwd : ফাঁকা;
  কাঠামো ip_quadruple ipquad;
  bool is_ipv6;
  BerElement *ber = ফাঁকা;

  *done = TRUE; /* unconditionally */
  infof(তথ্য, "LDAP local: LDAP Vendor = %s ; LDAP Version = %d",
        LDAP_VENDOR_NAME, LDAP_VENDOR_VERSION);
  infof(তথ্য, "LDAP local: %s", Curl_bufref_ptr(&তথ্য->state.url));

#ifdef HAVE_LDAP_URL_PARSE
  rc = ldap_url_parse(Curl_bufref_ptr(&তথ্য->state.url), &ludp);
#অন্যথা
  rc = ldap_url_parse_নিচের(তথ্য, conn, &ludp);
#endif
  যদি(rc) {
    failf(তথ্য, "Bad LDAP URL: %s", ldap_err2string(rc));
    ফলাফল = CURLE_URL_MALFORMAT;
    যাও quit;
  }

  ফলাফল = Curl_conn_get_ip_info(তথ্য, conn, FIRSTSOCKET, &is_ipv6, &ipquad);
  যদি(ফলাফল)
    যাও quit;

  /* Get the URL scheme (either ldap or ldaps) */
  ldap_ssl = Curl_conn_is_ssl(conn, FIRSTSOCKET);
  infof(তথ্য, "LDAP local: trying to establish %s connection",
        ldap_ssl ? "encrypted" : "cleartext");

#ifdef USE_WIN32_LDAP
  host = curlx_convert_UTF8_to_tchar(conn->origin->hostname);
  যদি(!host) {
    ফলাফল = CURLE_OUT_OF_MEMORY;

    যাও quit;
  }
#অন্যথা
  host = conn->origin->hostname;
#endif

#ifdef USE_WIN32_LDAP
  যদি(ldap_ssl)
    server = ldap_sslinit(host, (curl_ldap_অঙ্ক_t)ipquad.remote_port, 1);
  অন্যথা
#endif
    server = ldap_init(host, (curl_ldap_অঙ্ক_t)ipquad.remote_port);
  যদি(!server) {
    failf(তথ্য, "LDAP: cannot setup connect to %s:%u",
          conn->origin->user_hostname, ipquad.remote_port);
    ফলাফল = CURLE_COULDNT_CONNECT;
    যাও quit;
  }

#ifdef LDAP_OPT_NETWORK_TIMEOUT
  ldap_set_option(server, LDAP_OPT_NETWORK_TIMEOUT, &ldap_timeout);
#endif
  ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);

  /* Do not chase referrals. */
  ldap_set_option(server, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  যদি(ldap_ssl) {
#ifdef HAVE_LDAP_SSL
#ifdef USE_WIN32_LDAP
    /* Win32 LDAP uses the Windows CA store to verify certificates */
    ldap_set_option(server, LDAP_OPT_SSL, LDAP_OPT_ON);
    যদি(!conn->ssl_config.verifypeer) {
      যদি(conn->ssl_config.verifyhost) {
        failf(তথ্য, "LDAP local: host verification cannot be enabled when "
              "peer verification is disabled for Windows native LDAP");
        ফলাফল = CURLE_NOT_BUILT_IN;
        যাও quit;
      }
      ldap_set_option(server, LDAP_OPT_SERVER_CERTIFICATE,
                      (শূন্য *)(uintptr_t)bypass_cert_verify);
    }
#নতুবা_যদি defined(LDAP_OPT_X_TLS) /* !USE_WIN32_LDAP */
    পূর্ণসংখ্যা ldap_option;
    ধ্রুবক অক্ষর *ldap_ca = conn->ssl_config.CAfile;
    যদি(conn->ssl_config.verifypeer) {
      /* OpenLDAP SDK supports BASE64 files. */
      যদি(conn->ssl_config.cert_type &&
         !curl_strequal(conn->ssl_config.cert_type, "PEM")) {
        failf(তথ্য, "LDAP local: ERROR OpenLDAP only supports PEM cert-type");
        ফলাফল = CURLE_SSL_CERTPROBLEM;
        যাও quit;
      }
      যদি(!ldap_ca) {
        failf(তথ্য, "LDAP local: ERROR PEM CA cert not set");
        ফলাফল = CURLE_SSL_CERTPROBLEM;
        যাও quit;
      }
      infof(তথ্য, "LDAP local: using PEM CA cert: %s", ldap_ca);
      rc = ldap_set_option(server, LDAP_OPT_X_TLS_CACERTFILE, ldap_ca);
      যদি(rc != LDAP_SUCCESS) {
        failf(তথ্য, "LDAP local: ERROR setting PEM CA cert: %s",
              ldap_err2string(rc));
        ফলাফল = CURLE_SSL_CERTPROBLEM;
        যাও quit;
      }
      ldap_option = LDAP_OPT_X_TLS_DEMAND;
    }
    অন্যথা
      ldap_option = LDAP_OPT_X_TLS_NEVER;

    rc = ldap_set_option(server, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_option);
    যদি(rc != LDAP_SUCCESS) {
      failf(তথ্য, "LDAP local: ERROR setting cert verify mode: %s",
            ldap_err2string(rc));
      ফলাফল = CURLE_SSL_CERTPROBLEM;
      যাও quit;
    }
    ldap_option = LDAP_OPT_X_TLS_HARD;
    rc = ldap_set_option(server, LDAP_OPT_X_TLS, &ldap_option);
    যদি(rc != LDAP_SUCCESS) {
      failf(তথ্য, "LDAP local: ERROR setting SSL/TLS mode: %s",
            ldap_err2string(rc));
      ফলাফল = CURLE_SSL_CERTPROBLEM;
      যাও quit;
    }
#অন্যথা /* !USE_WIN32_LDAP && !LDAP_OPT_X_TLS */
    /* we should probably never come up to here since configure
       should check in first place if we can support LDAP SSL/TLS */
    failf(তথ্য, "LDAP local: SSL/TLS not supported with this version "
          "of the OpenLDAP toolkit");
    ফলাফল = CURLE_SSL_CERTPROBLEM;
    যাও quit;
#endif
#endif /* HAVE_LDAP_SSL */
  }
  অন্যথা যদি(তথ্য->set.use_ssl > CURLUSESSL_TRY) {
    failf(তথ্য, "LDAP local: explicit TLS not supported");
    ফলাফল = CURLE_NOT_BUILT_IN;
    যাও quit;
  }

#ifdef USE_WIN32_LDAP
  rc = ldap_win_bind(তথ্য, server, user, passwd);
#অন্যথা
  rc = ldap_simple_bind_s(server, user, passwd);
#endif
  যদি(!ldap_ssl && rc != LDAP_SUCCESS) {
    ldap_proto = LDAP_VERSION2;
    ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);
#ifdef USE_WIN32_LDAP
    rc = ldap_win_bind(তথ্য, server, user, passwd);
#অন্যথা
    rc = ldap_simple_bind_s(server, user, passwd);
#endif
  }
  যদি(rc != LDAP_SUCCESS) {
#ifdef USE_WIN32_LDAP
    failf(তথ্য, "LDAP local: bind via ldap_win_bind %s",
          ldap_err2string(rc));
#অন্যথা
    failf(তথ্য, "LDAP local: bind via ldap_simple_bind_s %s",
          ldap_err2string(rc));
#endif
    ফলাফল = CURLE_LDAP_CANNOT_BIND;
    যাও quit;
  }

  Curl_pgrsReset(তথ্য);
  rc = ldap_অনুসন্ধান_s(server, ludp->lud_dn,
                     ludp->lud_scope,
                     ludp->lud_filter, ludp->lud_attrs, 0, &ldapmsg);

  যদি(rc != LDAP_SUCCESS && rc != LDAP_SIZELIMIT_EXCEEDED) {
    failf(তথ্য, "LDAP remote: %s", ldap_err2string(rc));
    ফলাফল = CURLE_LDAP_অনুসন্ধান_FAILED;
    যাও quit;
  }

  অঙ্ক = 0;
  জন্য(entryIterator = ldap_first_entry(server, ldapmsg);
      entryIterator;
      entryIterator = ldap_পরবর্তী_entry(server, entryIterator), অঙ্ক++) {
#ifdef USE_WIN32_LDAP
    TCHAR *attribute;
#অন্যথা
    অক্ষর *attribute;
#endif

    /* Get the DN and write it to the client */
    {
      অক্ষর *name = ফাঁকা;
      size_t name_len = 0;
#ifdef USE_WIN32_LDAP
      TCHAR *dn = ldap_get_dn(server, entryIterator);
      যদি(dn)
        name = curlx_convert_tchar_to_UTF8(dn);
#অন্যথা
      অক্ষর *dn = name = ldap_get_dn(server, entryIterator);
#endif
      যদি(!name)
        ফলাফল = dn ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
      অন্যথা {
        name_len = strlen(name);
        যদি(ldap_str_needs_base64(name, name_len)) {
          অক্ষর *dn_b64 = ফাঁকা;
          size_t dn_b64_sz = 0;

          /* A DN with e.g. CR/LF would inject LDIF lines, so encode it */
          ফলাফল = curlx_base64_encode((uint8_t *)name, name_len,
                                       &dn_b64, &dn_b64_sz);
          যদি(!ফলাফল)
            ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, "DN:: ", 5);
          যদি(!ফলাফল)
            ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, dn_b64,
                                       dn_b64_sz);
          curlx_free(dn_b64);
        }
        অন্যথা {
          ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, "DN: ", 4);
          যদি(!ফলাফল)
            ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, name, name_len);
        }
        যদি(!ফলাফল)
          ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, "\n", 1);
      }
      FREE_ON_WINLDAP(name);
      ldap_memfree(dn);
      যদি(ফলাফল)
        যাও quit;
    }

    /* Get the attributes and write them to the client */
    জন্য(attribute = ldap_first_attribute(server, entryIterator, &ber);
        attribute;
        attribute = ldap_পরবর্তী_attribute(server, entryIterator, ber)) {
      Ber_মান **vals;
#ifdef USE_WIN32_LDAP
      অক্ষর *attr = curlx_convert_tchar_to_UTF8(attribute);
      যদি(!attr) {
        ldap_memfree(attribute);
        ফলাফল = CURLE_OUT_OF_MEMORY;
        যাও quit;
      }
#অন্যথা
      অক্ষর *attr = attribute;
#endif
      vals = ldap_get_মানসমূহ_len(server, entryIterator, attribute);
      যদি(vals) {
        ফলাফল = show_vals(তথ্য, vals, attr);
        /* Free memory used to store values */
        ldap_মান_free_len(vals);
      }

      /* Free the attribute as we are done with it */
      FREE_ON_WINLDAP(attr);
      ldap_memfree(attribute);

      যদি(!ফলাফল)
        ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, "\n", 1);
      যদি(ফলাফল)
        যাও quit;
    }

    যদি(ber) {
      ber_free(ber, 0);
      ber = ফাঁকা;
    }
  }

quit:
  যদি(ber)
    ber_free(ber, 0);
  যদি(ldapmsg) {
    ldap_msgfree(ldapmsg);
    LDAP_TRACE(("Received %d entries\n", অঙ্ক));
  }
  যদি(rc == LDAP_SIZELIMIT_EXCEEDED)
    infof(তথ্য, "There are more than %d entries", অঙ্ক);
  যদি(ludp)
    ldap_free_urldesc(ludp);
  যদি(server)
    ldap_unbind_s(server);

  FREE_ON_WINLDAP(host);

  /* no data to transfer */
  Curl_xfer_setup_nop(তথ্য);
  connclose(conn);

  ফেরত ফলাফল;
}

#ifdef DEBUG_LDAP
স্থির শূন্য ldap_trace_নিচের(ধ্রুবক অক্ষর *fmt, ...)
{
  স্থির পূর্ণসংখ্যা do_trace = -1;
  va_list args;

  যদি(do_trace == -1) {
    ধ্রুবক অক্ষর *env = getenv("CURL_TRACE");
    curl_off_t e = 0;
    যদি(!curlx_str_সংখ্যা(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  যদি(!do_trace)
    ফেরত;

  va_start(args, fmt);
  curl_mvfprintf(stderr, fmt, args);
  va_end(args);
}
#endif /* DEBUG_LDAP */

#ifndef HAVE_LDAP_URL_PARSE

/*
 * Return scope-value for a scope-string.
 */
স্থির পূর্ণসংখ্যা str2scope(ধ্রুবক অক্ষর *p)
{
  যদি(curl_strequal(p, "one"))
    ফেরত LDAP_SCOPE_ONELEVEL;
  যদি(curl_strequal(p, "onetree"))
    ফেরত LDAP_SCOPE_ONELEVEL;
  যদি(curl_strequal(p, "base"))
    ফেরত LDAP_SCOPE_BASE;
  যদি(curl_strequal(p, "sub"))
    ফেরত LDAP_SCOPE_SUBTREE;
  যদি(curl_strequal(p, "subtree"))
    ফেরত LDAP_SCOPE_SUBTREE;
  ফেরত -1;
}

/* number of entries in the attributes list */
স্থির size_t অঙ্ক_entries(ধ্রুবক অক্ষর *s)
{
  size_t বস্তুসমূহ = 1;

  s = strchr(s, ',');
  যখন(s) {
    বস্তুসমূহ++;
    s = strchr(s + 1, ',');
  }
  ফেরত বস্তুসমূহ;
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
স্থির curl_ldap_অঙ্ক_t ldap_url_parse2_নিচের(কাঠামো Curl_easy *তথ্য,
                                           ধ্রুবক কাঠামো connectdata *conn,
                                           LDAPURLDesc *ludp)
{
  curl_ldap_অঙ্ক_t rc = LDAP_SUCCESS;
  অক্ষর *p;
  অক্ষর *path;
  অক্ষর *q = ফাঁকা;
  অক্ষর *query = ফাঁকা;
  size_t i;

  যদি(!তথ্য ||
     !তথ্য->state.up.path ||
     তথ্য->state.up.path[0] != '/' ||
     ((তথ্য->state.origin->scheme != &Curl_scheme_ldap) &&
      (তথ্য->state.origin->scheme != &Curl_scheme_ldaps)))
    ফেরত LDAP_INVALID_SYNTAX;

  ludp->lud_scope = LDAP_SCOPE_BASE;
  ludp->lud_port  = conn->origin->port;
  ludp->lud_host  = conn->origin->hostname;

  /* Duplicate the path */
  p = path = curlx_strdup(তথ্য->state.up.path + 1);
  যদি(!path)
    ফেরত LDAP_NO_MEMORY;

  /* Duplicate the query if present */
  যদি(তথ্য->state.up.query) {
    q = query = curlx_strdup(তথ্য->state.up.query);
    যদি(!query) {
      curlx_free(path);
      ফেরত LDAP_NO_MEMORY;
    }
  }

  /* Parse the DN (Distinguished Name) */
  যদি(*p) {
    অক্ষর *dn = p;
    অক্ষর *unescaped;
    CURLcode ফলাফল;

    LDAP_TRACE(("DN '%s'\n", dn));

    /* Unescape the DN */
    ফলাফল = Curl_urldecode(dn, 0, &unescaped, ফাঁকা, REJECT_ZERO);
    যদি(ফলাফল) {
      rc = LDAP_NO_MEMORY;

      যাও quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_dn = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    যদি(!ludp->lud_dn) {
      rc = LDAP_NO_MEMORY;

      যাও quit;
    }
#অন্যথা
    ludp->lud_dn = unescaped;
#endif
  }

  p = q;
  যদি(!p)
    যাও quit;

  /* Parse the attributes. skip "??" */
  q = strchr(p, '?');
  যদি(q)
    *q++ = '\0';

  যদি(*p) {
    size_t গুনতি = অঙ্ক_entries(p); /* at least one */
    ধ্রুবক অক্ষর *atp = p;

    /* Allocate our array (+1 for the NULL entry) */
#ifdef USE_WIN32_LDAP
    ludp->lud_attrs = curlx_calloc(গুনতি + 1, আকার(TCHAR *));
#অন্যথা
    ludp->lud_attrs = curlx_calloc(গুনতি + 1, আকার(অক্ষর *));
#endif
    যদি(!ludp->lud_attrs) {
      rc = LDAP_NO_MEMORY;
      যাও quit;
    }

    জন্য(i = 0; i < গুনতি; i++) {
      অক্ষর *unescaped;
      CURLcode ফলাফল;
      কাঠামো Curl_str out;

      যদি(curlx_str_until(&atp, &out, 1024, ','))
        বিরতি;

      LDAP_TRACE(("attr[%zu] '%.*s'\n", i, (পূর্ণসংখ্যা)out.len, out.str));

      /* Unescape the attribute */
      ফলাফল = Curl_urldecode(out.str, out.len, &unescaped, ফাঁকা, REJECT_ZERO);
      যদি(ফলাফল) {
        rc = LDAP_NO_MEMORY;
        যাও quit;
      }

#ifdef USE_WIN32_LDAP
      /* Convert the unescaped string to a tchar */
      ludp->lud_attrs[i] = curlx_convert_UTF8_to_tchar(unescaped);

      /* Free the unescaped string as we are done with it */
      curlx_free(unescaped);

      যদি(!ludp->lud_attrs[i]) {
        rc = LDAP_NO_MEMORY;
        যাও quit;
      }
#অন্যথা
      ludp->lud_attrs[i] = unescaped;
#endif

      ludp->lud_attrs_dups++;
      যদি(curlx_str_single(&atp, ','))
        বিরতি;
    }
  }

  p = q;
  যদি(!p)
    যাও quit;

  /* Parse the scope. skip "??" */
  q = strchr(p, '?');
  যদি(q)
    *q++ = '\0';

  যদি(*p) {
    ludp->lud_scope = str2scope(p);
    যদি(ludp->lud_scope == -1) {
      rc = LDAP_INVALID_SYNTAX;

      যাও quit;
    }
    LDAP_TRACE(("scope %d\n", ludp->lud_scope));
  }

  p = q;
  যদি(!p)
    যাও quit;

  /* Parse the filter */
  q = strchr(p, '?');
  যদি(q)
    *q++ = '\0';

  যদি(*p) {
    অক্ষর *filter = p;
    অক্ষর *unescaped;
    CURLcode ফলাফল;

    LDAP_TRACE(("filter '%s'\n", filter));

    /* Unescape the filter */
    ফলাফল = Curl_urldecode(filter, 0, &unescaped, ফাঁকা, REJECT_CTRL);
    যদি(ফলাফল) {
      rc = LDAP_NO_MEMORY;

      যাও quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_filter = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    যদি(!ludp->lud_filter) {
      rc = LDAP_NO_MEMORY;

      যাও quit;
    }
#অন্যথা
    ludp->lud_filter = unescaped;
#endif
  }

  p = q;
  যদি(p && !*p) {
    rc = LDAP_INVALID_SYNTAX;

    যাও quit;
  }

quit:
  curlx_free(path);
  curlx_free(query);

  ফেরত rc;
}

স্থির curl_ldap_অঙ্ক_t ldap_url_parse_নিচের(কাঠামো Curl_easy *তথ্য,
                                          ধ্রুবক কাঠামো connectdata *conn,
                                          LDAPURLDesc **ludpp)
{
  LDAPURLDesc *ludp = curlx_calloc(1, আকার(*ludp));
  curl_ldap_অঙ্ক_t rc;

  *ludpp = ফাঁকা;
  যদি(!ludp)
    ফেরত LDAP_NO_MEMORY;

  rc = ldap_url_parse2_নিচের(তথ্য, conn, ludp);
  যদি(rc != LDAP_SUCCESS) {
    ldap_free_urldesc_নিচের(ludp);
    ludp = ফাঁকা;
  }
  *ludpp = ludp;
  ফেরত rc;
}

স্থির শূন্য ldap_free_urldesc_নিচের(LDAPURLDesc *ludp)
{
  যদি(!ludp)
    ফেরত;

  curlx_free(ludp->lud_dn);
  curlx_free(ludp->lud_filter);

  যদি(ludp->lud_attrs) {
    size_t i;
    জন্য(i = 0; i < ludp->lud_attrs_dups; i++) {
      curlx_free(ludp->lud_attrs[i]);
    }
    curlx_free(ludp->lud_attrs);
  }

  curlx_free(ludp);
}
#endif /* !HAVE_LDAP_URL_PARSE */

শূন্য Curl_ldap_version(অক্ষর *buf, size_t bufsz)
{
#ifdef USE_WIN32_LDAP
  curl_msnprintf(buf, bufsz, "WinLDAP");
#অন্যথা
#ifdef LDAP_OPT_X_TLS_PASSPHRASE
  স্থির ধ্রুবক অক্ষর flavor[] = "/Apple";
#অন্যথা
  স্থির ধ্রুবক অক্ষর flavor[] = "";
#endif
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  /* Comparing against 0, as different platforms
     disagree on the success define name */
  যদি(ldap_get_option(ফাঁকা, LDAP_OPT_API_INFO, &api) == 0) {
    unsigned পূর্ণসংখ্যা patch = (unsigned পূর্ণসংখ্যা)(api.ldapai_vendor_version % 100);
    unsigned পূর্ণসংখ্যা major = (unsigned পূর্ণসংখ্যা)(api.ldapai_vendor_version / 10000);
    unsigned পূর্ণসংখ্যা minor =
      (((unsigned পূর্ণসংখ্যা)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
#ifdef __OS400__
    curl_msnprintf(buf, bufsz, "IBMLDAP/%u.%u.%u", major, minor, patch);
    ldap_মান_free(api.ldapai_extensions);
    (শূন্য)flavor;
#অন্যথা
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u%s",
                   api.ldapai_vendor_name, major, minor, patch, flavor);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((শূন্য **)api.ldapai_extensions);
#endif
  }
  অন্যথা
    curl_msnprintf(buf, bufsz, "LDAP/1");
#endif /* USE_WIN32_LDAP */
}

/*
 * LDAP protocol handler.
 */
ধ্রুবক কাঠামো Curl_protocol Curl_protocol_ldap = {
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

#যদি defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic বের_করো
#endif

#endif /* !CURL_DISABLE_LDAP && !USE_OPENLDAP */
