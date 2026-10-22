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
#define FREE_ON_WINLDAP(x) কুরু {} যাবত্(0)
#define curl_ldap_অঙ্ক_t    পূর্ণাঙ্ক
#endif

#ifndef HAVE_LDAP_URL_PARSE

/* Use our own implementation. */

সংরচনা ldap_urldesc {
  বর্ণ   *lud_host;
  পূর্ণাঙ্ক     lud_port;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_dn;
  TCHAR **lud_attrs;
#অন্যথা
  বর্ণ   *lud_dn;
  বর্ণ  **lud_attrs;
#endif
  পূর্ণাঙ্ক     lud_scope;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_filter;
#অন্যথা
  বর্ণ   *lud_filter;
#endif
  বর্ণ  **lud_exts;
  size_t  lud_attrs_dups; /* how many were dup'ed, this field is not in the
                             "real" struct so can only be used in code
                             without HAVE_LDAP_URL_PARSE defined */
};

#undef LDAPURLDesc
#define LDAPURLDesc সংরচনা ldap_urldesc

স্থির curl_ldap_অঙ্ক_t ldap_url_parse_নিচের(সংরচনা Curl_easy *data,
                                          ধ্রুব সংরচনা connectdata *conn,
                                          LDAPURLDesc **ludpp);
স্থির শূন্য ldap_free_urldesc_নিচের(LDAPURLDesc *ludp);

#undef ldap_free_urldesc
#define ldap_free_urldesc       ldap_free_urldesc_নিচের

#endif /* !HAVE_LDAP_URL_PARSE */

#ifdef DEBUG_LDAP
#define LDAP_TRACE(x)                 \
  কুরু {                                \
    ldap_trace_নিচের("%u: ", __LINE__); \
    ldap_trace_নিচের x;                 \
  } যাবত্(0)

স্থির শূন্য ldap_trace_নিচের(ধ্রুব বর্ণ *fmt, ...) CURL_PRINTF(1, 2);
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
স্থির ULONG ldap_win_bind_auth(LDAP *server, ধ্রুব বর্ণ *user,
                                ধ্রুব বর্ণ *passwd, unsigned long authflags)
{
  ULONG method = 0;
  SEC_WINNT_AUTH_IDENTITY_EX cred;
  ULONG rc = LDAP_AUTH_METHOD_NOT_SUPPORTED;

  memset(&cred, 0, পরিমাণ(cred));

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
    CURLcode result = Curl_create_sspi_identity(user, passwd, &cred);
    যদি(!result) {
      rc = ldap_bind_s(server, রিক্ত, (TCHAR *)&cred, method);
      Curl_sspi_free_identity(&cred);
    }
    অন্যথা {
      rc = LDAP_NO_MEMORY;
    }
  }
  অন্যথা {
    /* proceed with current user credentials */
    method = LDAP_AUTH_NEGOTIATE;
    rc = ldap_bind_s(server, রিক্ত, রিক্ত, method);
  }
  প্রত্যাগম rc;
}
#endif /* USE_WINDOWS_SSPI */

স্থির ULONG ldap_win_bind(সংরচনা Curl_easy *data, LDAP *server,
                           ধ্রুব বর্ণ *user, ধ্রুব বর্ণ *passwd)
{
  ULONG rc = LDAP_INVALID_CREDENTIALS;

  PTCHAR inuser = রিক্ত;
  PTCHAR inpass = রিক্ত;

  যদি(user && passwd && (data->set.httpauth & CURLAUTH_BASIC)) {
    inuser = curlx_convert_UTF8_to_tchar(user);
    inpass = curlx_convert_UTF8_to_tchar(passwd);

    rc = ldap_simple_bind_s(server, inuser, inpass);

    curlx_free(inuser);
    curlx_free(inpass);
  }
#ifdef USE_WINDOWS_SSPI
  অন্যথা {
    rc = ldap_win_bind_auth(server, user, passwd, data->set.httpauth);
  }
#endif

  প্রত্যাগম rc;
}
#endif /* USE_WIN32_LDAP */

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
স্থির bool ldap_str_needs_base64(ধ্রুব বর্ণ *s, size_t len)
{
  size_t i;
  যদি(!len)
    প্রত্যাগম FALSE;
  যদি((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    প্রত্যাগম TRUE;
  প্রতি(i = 0; i < len; i++)
    যদি(!ISPRINT(s[i]))
      প্রত্যাগম TRUE;
  প্রত্যাগম FALSE;
}

স্থির bool ldap_মান_needs_base64(ধ্রুব বর্ণ *attr, size_t attr_len,
                                    ধ্রুব Ber_মান *val)
{
  যদি((attr_len > 7) && curl_strequal(";binary", attr + attr_len - 7))
    প্রত্যাগম TRUE;

  প্রত্যাগম ldap_str_needs_base64(val->bv_val, val->bv_len);
}

#ifdef USE_WIN32_LDAP
স্থির BOOLEAN bypass_cert_verify(PLDAP Connection,
                                  PCCERT_CONTEXT *ppServerCert)
{
  (শূন্য)Connection;
  CertFreeCertificateContext(*ppServerCert);
  /* approve any certificate since the verification is set to bypass */
  প্রত্যাগম TRUE;
}
#endif

স্থির CURLcode show_vals(সংরচনা Curl_easy *data, Ber_মান **vals,
                          ধ্রুব বর্ণ *attr)
{
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক i;
  size_t attr_len = strlen(attr);

  /* An attribute type has no base64 form in LDIF, so a type carrying control
     bytes (CR/LF etc.) would let a hostile server inject lines. A conformant
     type name never contains them, so reject it. */
  যদি(ldap_str_needs_base64(attr, attr_len)) {
    failf(data, "LDAP local: attribute type contains illegal bytes");
    প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;
  }

  প্রতি(i = 0; vals[i] && !result; i++) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, "\t", 1);
    যদি(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, attr, attr_len);
    যদি(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, ":", 1);

    যদি(result)
      ভঙ্গ;

    যদি(ldap_মান_needs_base64(attr, attr_len, vals[i])) {
      বর্ণ *val_b64 = রিক্ত;
      size_t val_b64_sz = 0;

      /* Binary attribute, encode to base64. */
      যদি(vals[i]->bv_len)
        result = curlx_base64_encode((uint8_t *)vals[i]->bv_val,
                                     vals[i]->bv_len,
                                     &val_b64, &val_b64_sz);
      যদি(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, ": ", 2);
      যদি(!result && val_b64_sz)
        result = Curl_client_write(data, CLIENTWRITE_BODY, val_b64,
                                   val_b64_sz);
      curlx_free(val_b64);
    }
    অন্যথা {
      result = Curl_client_write(data, CLIENTWRITE_BODY, " ", 1);
      যদি(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   vals[i]->bv_val, vals[i]->bv_len);
    }

    যদি(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
  }
  প্রত্যাগম result;
}

স্থির CURLcode ldap_do(সংরচনা Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  curl_ldap_অঙ্ক_t rc = LDAP_SUCCESS;
  LDAP *server = রিক্ত;
  LDAPURLDesc *ludp = রিক্ত;
  LDAPMessage *ldapmsg = রিক্ত;
  LDAPMessage *entryIterator;
  পূর্ণাঙ্ক অঙ্ক = 0;
  সংরচনা connectdata *conn = data->conn;
  পূর্ণাঙ্ক ldap_proto = LDAP_VERSION3;
  bool ldap_ssl = FALSE;
#ifdef LDAP_OPT_NETWORK_TIMEOUT
  সংরচনা timeval ldap_timeout = { 10, 0 }; /* 10s connection/search timeout */
#endif
#ifdef USE_WIN32_LDAP
  TCHAR *host = রিক্ত;
#অন্যথা
  বর্ণ *host = রিক্ত;
#endif
  ধ্রুব বর্ণ *user = data->state.creds ? data->state.creds->user : রিক্ত;
  ধ্রুব বর্ণ *passwd = data->state.creds ? data->state.creds->passwd : রিক্ত;
  সংরচনা ip_quadruple ipquad;
  bool is_ipv6;
  BerElement *ber = রিক্ত;

  *done = TRUE; /* unconditionally */
  infof(data, "LDAP local: LDAP Vendor = %s ; LDAP Version = %d",
        LDAP_VENDOR_NAME, LDAP_VENDOR_VERSION);
  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

#ifdef HAVE_LDAP_URL_PARSE
  rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), &ludp);
#অন্যথা
  rc = ldap_url_parse_নিচের(data, conn, &ludp);
#endif
  যদি(rc) {
    failf(data, "Bad LDAP URL: %s", ldap_err2string(rc));
    result = CURLE_URL_MALFORMAT;
    গচ্ছ quit;
  }

  result = Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad);
  যদি(result)
    গচ্ছ quit;

  /* Get the URL scheme (either ldap or ldaps) */
  ldap_ssl = Curl_conn_is_ssl(conn, FIRSTSOCKET);
  infof(data, "LDAP local: trying to establish %s connection",
        ldap_ssl ? "encrypted" : "cleartext");

#ifdef USE_WIN32_LDAP
  host = curlx_convert_UTF8_to_tchar(conn->origin->hostname);
  যদি(!host) {
    result = CURLE_OUT_OF_MEMORY;

    গচ্ছ quit;
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
    failf(data, "LDAP: cannot setup connect to %s:%u",
          conn->origin->user_hostname, ipquad.remote_port);
    result = CURLE_COULDNT_CONNECT;
    গচ্ছ quit;
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
        failf(data, "LDAP local: host verification cannot be enabled when "
              "peer verification is disabled for Windows native LDAP");
        result = CURLE_NOT_BUILT_IN;
        গচ্ছ quit;
      }
      ldap_set_option(server, LDAP_OPT_SERVER_CERTIFICATE,
                      (শূন্য *)(uintptr_t)bypass_cert_verify);
    }
#নোচেদ্যদি defined(LDAP_OPT_X_TLS) /* !USE_WIN32_LDAP */
    পূর্ণাঙ্ক ldap_option;
    ধ্রুব বর্ণ *ldap_ca = conn->ssl_config.CAfile;
    যদি(conn->ssl_config.verifypeer) {
      /* OpenLDAP SDK supports BASE64 files. */
      যদি(conn->ssl_config.cert_type &&
         !curl_strequal(conn->ssl_config.cert_type, "PEM")) {
        failf(data, "LDAP local: ERROR OpenLDAP only supports PEM cert-type");
        result = CURLE_SSL_CERTPROBLEM;
        গচ্ছ quit;
      }
      যদি(!ldap_ca) {
        failf(data, "LDAP local: ERROR PEM CA cert not set");
        result = CURLE_SSL_CERTPROBLEM;
        গচ্ছ quit;
      }
      infof(data, "LDAP local: using PEM CA cert: %s", ldap_ca);
      rc = ldap_set_option(server, LDAP_OPT_X_TLS_CACERTFILE, ldap_ca);
      যদি(rc != LDAP_SUCCESS) {
        failf(data, "LDAP local: ERROR setting PEM CA cert: %s",
              ldap_err2string(rc));
        result = CURLE_SSL_CERTPROBLEM;
        গচ্ছ quit;
      }
      ldap_option = LDAP_OPT_X_TLS_DEMAND;
    }
    অন্যথা
      ldap_option = LDAP_OPT_X_TLS_NEVER;

    rc = ldap_set_option(server, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_option);
    যদি(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting cert verify mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      গচ্ছ quit;
    }
    ldap_option = LDAP_OPT_X_TLS_HARD;
    rc = ldap_set_option(server, LDAP_OPT_X_TLS, &ldap_option);
    যদি(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting SSL/TLS mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      গচ্ছ quit;
    }
#অন্যথা /* !USE_WIN32_LDAP && !LDAP_OPT_X_TLS */
    /* we should probably never come up to here since configure
       should check in first place if we can support LDAP SSL/TLS */
    failf(data, "LDAP local: SSL/TLS not supported with this version "
          "of the OpenLDAP toolkit");
    result = CURLE_SSL_CERTPROBLEM;
    গচ্ছ quit;
#endif
#endif /* HAVE_LDAP_SSL */
  }
  অন্যথা যদি(data->set.use_ssl > CURLUSESSL_TRY) {
    failf(data, "LDAP local: explicit TLS not supported");
    result = CURLE_NOT_BUILT_IN;
    গচ্ছ quit;
  }

#ifdef USE_WIN32_LDAP
  rc = ldap_win_bind(data, server, user, passwd);
#অন্যথা
  rc = ldap_simple_bind_s(server, user, passwd);
#endif
  যদি(!ldap_ssl && rc != LDAP_SUCCESS) {
    ldap_proto = LDAP_VERSION2;
    ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);
#ifdef USE_WIN32_LDAP
    rc = ldap_win_bind(data, server, user, passwd);
#অন্যথা
    rc = ldap_simple_bind_s(server, user, passwd);
#endif
  }
  যদি(rc != LDAP_SUCCESS) {
#ifdef USE_WIN32_LDAP
    failf(data, "LDAP local: bind via ldap_win_bind %s",
          ldap_err2string(rc));
#অন্যথা
    failf(data, "LDAP local: bind via ldap_simple_bind_s %s",
          ldap_err2string(rc));
#endif
    result = CURLE_LDAP_CANNOT_BIND;
    গচ্ছ quit;
  }

  Curl_pgrsReset(data);
  rc = ldap_অনুসন্ধান_s(server, ludp->lud_dn,
                     ludp->lud_scope,
                     ludp->lud_filter, ludp->lud_attrs, 0, &ldapmsg);

  যদি(rc != LDAP_SUCCESS && rc != LDAP_SIZELIMIT_EXCEEDED) {
    failf(data, "LDAP remote: %s", ldap_err2string(rc));
    result = CURLE_LDAP_অনুসন্ধান_FAILED;
    গচ্ছ quit;
  }

  অঙ্ক = 0;
  প্রতি(entryIterator = ldap_first_entry(server, ldapmsg);
      entryIterator;
      entryIterator = ldap_next_entry(server, entryIterator), অঙ্ক++) {
#ifdef USE_WIN32_LDAP
    TCHAR *attribute;
#অন্যথা
    বর্ণ *attribute;
#endif

    /* Get the DN and write it to the client */
    {
      বর্ণ *name = রিক্ত;
      size_t name_len = 0;
#ifdef USE_WIN32_LDAP
      TCHAR *dn = ldap_get_dn(server, entryIterator);
      যদি(dn)
        name = curlx_convert_tchar_to_UTF8(dn);
#অন্যথা
      বর্ণ *dn = name = ldap_get_dn(server, entryIterator);
#endif
      যদি(!name)
        result = dn ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
      অন্যথা {
        name_len = strlen(name);
        যদি(ldap_str_needs_base64(name, name_len)) {
          বর্ণ *dn_b64 = রিক্ত;
          size_t dn_b64_sz = 0;

          /* A DN with e.g. CR/LF would inject LDIF lines, so encode it */
          result = curlx_base64_encode((uint8_t *)name, name_len,
                                       &dn_b64, &dn_b64_sz);
          যদি(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, "DN:: ", 5);
          যদি(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, dn_b64,
                                       dn_b64_sz);
          curlx_free(dn_b64);
        }
        অন্যথা {
          result = Curl_client_write(data, CLIENTWRITE_BODY, "DN: ", 4);
          যদি(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, name, name_len);
        }
        যদি(!result)
          result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      }
      FREE_ON_WINLDAP(name);
      ldap_memfree(dn);
      যদি(result)
        গচ্ছ quit;
    }

    /* Get the attributes and write them to the client */
    প্রতি(attribute = ldap_first_attribute(server, entryIterator, &ber);
        attribute;
        attribute = ldap_next_attribute(server, entryIterator, ber)) {
      Ber_মান **vals;
#ifdef USE_WIN32_LDAP
      বর্ণ *attr = curlx_convert_tchar_to_UTF8(attribute);
      যদি(!attr) {
        ldap_memfree(attribute);
        result = CURLE_OUT_OF_MEMORY;
        গচ্ছ quit;
      }
#অন্যথা
      বর্ণ *attr = attribute;
#endif
      vals = ldap_get_মানসমূহ_len(server, entryIterator, attribute);
      যদি(vals) {
        result = show_vals(data, vals, attr);
        /* Free memory used to store values */
        ldap_মান_free_len(vals);
      }

      /* Free the attribute as we are done with it */
      FREE_ON_WINLDAP(attr);
      ldap_memfree(attribute);

      যদি(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      যদি(result)
        গচ্ছ quit;
    }

    যদি(ber) {
      ber_free(ber, 0);
      ber = রিক্ত;
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
    infof(data, "There are more than %d entries", অঙ্ক);
  যদি(ludp)
    ldap_free_urldesc(ludp);
  যদি(server)
    ldap_unbind_s(server);

  FREE_ON_WINLDAP(host);

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  connclose(conn);

  প্রত্যাগম result;
}

#ifdef DEBUG_LDAP
স্থির শূন্য ldap_trace_নিচের(ধ্রুব বর্ণ *fmt, ...)
{
  স্থির পূর্ণাঙ্ক do_trace = -1;
  va_list args;

  যদি(do_trace == -1) {
    ধ্রুব বর্ণ *env = getenv("CURL_TRACE");
    curl_off_t e = 0;
    যদি(!curlx_str_সংখ্যা(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  যদি(!do_trace)
    প্রত্যাগম;

  va_start(args, fmt);
  curl_mvfprintf(stderr, fmt, args);
  va_end(args);
}
#endif /* DEBUG_LDAP */

#ifndef HAVE_LDAP_URL_PARSE

/*
 * Return scope-value for a scope-string.
 */
স্থির পূর্ণাঙ্ক str2scope(ধ্রুব বর্ণ *p)
{
  যদি(curl_strequal(p, "one"))
    প্রত্যাগম LDAP_SCOPE_ONELEVEL;
  যদি(curl_strequal(p, "onetree"))
    প্রত্যাগম LDAP_SCOPE_ONELEVEL;
  যদি(curl_strequal(p, "base"))
    প্রত্যাগম LDAP_SCOPE_BASE;
  যদি(curl_strequal(p, "sub"))
    প্রত্যাগম LDAP_SCOPE_SUBTREE;
  যদি(curl_strequal(p, "subtree"))
    প্রত্যাগম LDAP_SCOPE_SUBTREE;
  প্রত্যাগম -1;
}

/* number of entries in the attributes list */
স্থির size_t অঙ্ক_entries(ধ্রুব বর্ণ *s)
{
  size_t বস্তুসমূহ = 1;

  s = strchr(s, ',');
  যাবত্(s) {
    বস্তুসমূহ++;
    s = strchr(s + 1, ',');
  }
  প্রত্যাগম বস্তুসমূহ;
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
স্থির curl_ldap_অঙ্ক_t ldap_url_parse2_নিচের(সংরচনা Curl_easy *data,
                                           ধ্রুব সংরচনা connectdata *conn,
                                           LDAPURLDesc *ludp)
{
  curl_ldap_অঙ্ক_t rc = LDAP_SUCCESS;
  বর্ণ *p;
  বর্ণ *path;
  বর্ণ *q = রিক্ত;
  বর্ণ *query = রিক্ত;
  size_t i;

  যদি(!data ||
     !data->state.up.path ||
     data->state.up.path[0] != '/' ||
     ((data->state.origin->scheme != &Curl_scheme_ldap) &&
      (data->state.origin->scheme != &Curl_scheme_ldaps)))
    প্রত্যাগম LDAP_INVALID_SYNTAX;

  ludp->lud_scope = LDAP_SCOPE_BASE;
  ludp->lud_port  = conn->origin->port;
  ludp->lud_host  = conn->origin->hostname;

  /* Duplicate the path */
  p = path = curlx_strdup(data->state.up.path + 1);
  যদি(!path)
    প্রত্যাগম LDAP_NO_MEMORY;

  /* Duplicate the query if present */
  যদি(data->state.up.query) {
    q = query = curlx_strdup(data->state.up.query);
    যদি(!query) {
      curlx_free(path);
      প্রত্যাগম LDAP_NO_MEMORY;
    }
  }

  /* Parse the DN (Distinguished Name) */
  যদি(*p) {
    বর্ণ *dn = p;
    বর্ণ *unescaped;
    CURLcode result;

    LDAP_TRACE(("DN '%s'\n", dn));

    /* Unescape the DN */
    result = Curl_urldecode(dn, 0, &unescaped, রিক্ত, REJECT_ZERO);
    যদি(result) {
      rc = LDAP_NO_MEMORY;

      গচ্ছ quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_dn = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    যদি(!ludp->lud_dn) {
      rc = LDAP_NO_MEMORY;

      গচ্ছ quit;
    }
#অন্যথা
    ludp->lud_dn = unescaped;
#endif
  }

  p = q;
  যদি(!p)
    গচ্ছ quit;

  /* Parse the attributes. skip "??" */
  q = strchr(p, '?');
  যদি(q)
    *q++ = '\0';

  যদি(*p) {
    size_t গুনতি = অঙ্ক_entries(p); /* at least one */
    ধ্রুব বর্ণ *atp = p;

    /* Allocate our array (+1 for the NULL entry) */
#ifdef USE_WIN32_LDAP
    ludp->lud_attrs = curlx_calloc(গুনতি + 1, পরিমাণ(TCHAR *));
#অন্যথা
    ludp->lud_attrs = curlx_calloc(গুনতি + 1, পরিমাণ(বর্ণ *));
#endif
    যদি(!ludp->lud_attrs) {
      rc = LDAP_NO_MEMORY;
      গচ্ছ quit;
    }

    প্রতি(i = 0; i < গুনতি; i++) {
      বর্ণ *unescaped;
      CURLcode result;
      সংরচনা Curl_str out;

      যদি(curlx_str_until(&atp, &out, 1024, ','))
        ভঙ্গ;

      LDAP_TRACE(("attr[%zu] '%.*s'\n", i, (পূর্ণাঙ্ক)out.len, out.str));

      /* Unescape the attribute */
      result = Curl_urldecode(out.str, out.len, &unescaped, রিক্ত, REJECT_ZERO);
      যদি(result) {
        rc = LDAP_NO_MEMORY;
        গচ্ছ quit;
      }

#ifdef USE_WIN32_LDAP
      /* Convert the unescaped string to a tchar */
      ludp->lud_attrs[i] = curlx_convert_UTF8_to_tchar(unescaped);

      /* Free the unescaped string as we are done with it */
      curlx_free(unescaped);

      যদি(!ludp->lud_attrs[i]) {
        rc = LDAP_NO_MEMORY;
        গচ্ছ quit;
      }
#অন্যথা
      ludp->lud_attrs[i] = unescaped;
#endif

      ludp->lud_attrs_dups++;
      যদি(curlx_str_single(&atp, ','))
        ভঙ্গ;
    }
  }

  p = q;
  যদি(!p)
    গচ্ছ quit;

  /* Parse the scope. skip "??" */
  q = strchr(p, '?');
  যদি(q)
    *q++ = '\0';

  যদি(*p) {
    ludp->lud_scope = str2scope(p);
    যদি(ludp->lud_scope == -1) {
      rc = LDAP_INVALID_SYNTAX;

      গচ্ছ quit;
    }
    LDAP_TRACE(("scope %d\n", ludp->lud_scope));
  }

  p = q;
  যদি(!p)
    গচ্ছ quit;

  /* Parse the filter */
  q = strchr(p, '?');
  যদি(q)
    *q++ = '\0';

  যদি(*p) {
    বর্ণ *filter = p;
    বর্ণ *unescaped;
    CURLcode result;

    LDAP_TRACE(("filter '%s'\n", filter));

    /* Unescape the filter */
    result = Curl_urldecode(filter, 0, &unescaped, রিক্ত, REJECT_CTRL);
    যদি(result) {
      rc = LDAP_NO_MEMORY;

      গচ্ছ quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_filter = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    যদি(!ludp->lud_filter) {
      rc = LDAP_NO_MEMORY;

      গচ্ছ quit;
    }
#অন্যথা
    ludp->lud_filter = unescaped;
#endif
  }

  p = q;
  যদি(p && !*p) {
    rc = LDAP_INVALID_SYNTAX;

    গচ্ছ quit;
  }

quit:
  curlx_free(path);
  curlx_free(query);

  প্রত্যাগম rc;
}

স্থির curl_ldap_অঙ্ক_t ldap_url_parse_নিচের(সংরচনা Curl_easy *data,
                                          ধ্রুব সংরচনা connectdata *conn,
                                          LDAPURLDesc **ludpp)
{
  LDAPURLDesc *ludp = curlx_calloc(1, পরিমাণ(*ludp));
  curl_ldap_অঙ্ক_t rc;

  *ludpp = রিক্ত;
  যদি(!ludp)
    প্রত্যাগম LDAP_NO_MEMORY;

  rc = ldap_url_parse2_নিচের(data, conn, ludp);
  যদি(rc != LDAP_SUCCESS) {
    ldap_free_urldesc_নিচের(ludp);
    ludp = রিক্ত;
  }
  *ludpp = ludp;
  প্রত্যাগম rc;
}

স্থির শূন্য ldap_free_urldesc_নিচের(LDAPURLDesc *ludp)
{
  যদি(!ludp)
    প্রত্যাগম;

  curlx_free(ludp->lud_dn);
  curlx_free(ludp->lud_filter);

  যদি(ludp->lud_attrs) {
    size_t i;
    প্রতি(i = 0; i < ludp->lud_attrs_dups; i++) {
      curlx_free(ludp->lud_attrs[i]);
    }
    curlx_free(ludp->lud_attrs);
  }

  curlx_free(ludp);
}
#endif /* !HAVE_LDAP_URL_PARSE */

শূন্য Curl_ldap_version(বর্ণ *buf, size_t bufsz)
{
#ifdef USE_WIN32_LDAP
  curl_msnprintf(buf, bufsz, "WinLDAP");
#অন্যথা
#ifdef LDAP_OPT_X_TLS_PASSPHRASE
  স্থির ধ্রুব বর্ণ flavor[] = "/Apple";
#অন্যথা
  স্থির ধ্রুব বর্ণ flavor[] = "";
#endif
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  /* Comparing against 0, as different platforms
     disagree on the success define name */
  যদি(ldap_get_option(রিক্ত, LDAP_OPT_API_INFO, &api) == 0) {
    unsigned পূর্ণাঙ্ক patch = (unsigned পূর্ণাঙ্ক)(api.ldapai_vendor_version % 100);
    unsigned পূর্ণাঙ্ক major = (unsigned পূর্ণাঙ্ক)(api.ldapai_vendor_version / 10000);
    unsigned পূর্ণাঙ্ক minor =
      (((unsigned পূর্ণাঙ্ক)api.ldapai_vendor_version - (major * 10000))
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
ধ্রুব সংরচনা Curl_protocol Curl_protocol_ldap = {
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
