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
#அந்தர்பாவ "curl_setup.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curl_ldap.h"

#எனில் !defined(CURL_DISABLE_LDAP) && !defined(USE_OPENLDAP)

#எனில் defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic தள்ளு
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
#இல்லையெனில்
#  define LDAP_DEPRECATED 1     /* Be sure ldap_init() is defined. */
#  ifdef HAVE_LBER_H
#    include <lber.h>
#  endif
#  include <ldap.h>
#  எனில் defined(HAVE_LDAP_SSL) && defined(HAVE_LDAP_SSL_H)
#    include <ldap_ssl.h>
#  endif /* HAVE_LDAP_SSL && HAVE_LDAP_SSL_H */
#endif

#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "escape.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "bufref.h"
#அந்தர்பாவ "curlx/multibyte.h"
#அந்தர்பாவ "curlx/base64.h"
#அந்தர்பாவ "connect.h"

#ifdef USE_WIN32_LDAP
#define FREE_ON_WINLDAP(x) curlx_free(x)
#define curl_ldap_இலக்கம்_t    ULONG
#இல்லையெனில்
#define FREE_ON_WINLDAP(x) செய் {} போது(0)
#define curl_ldap_இலக்கம்_t    முழு
#endif

#ifndef HAVE_LDAP_URL_PARSE

/* Use our own implementation. */

கட்டமைப்பு ldap_urldesc {
  எழுத்து   *lud_host;
  முழு     lud_port;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_dn;
  TCHAR **lud_attrs;
#இல்லையெனில்
  எழுத்து   *lud_dn;
  எழுத்து  **lud_attrs;
#endif
  முழு     lud_scope;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_filter;
#இல்லையெனில்
  எழுத்து   *lud_filter;
#endif
  எழுத்து  **lud_exts;
  size_t  lud_attrs_dups; /* how many were dup'ed, this field is not in the
                             "real" struct so can only be used in code
                             without HAVE_LDAP_URL_PARSE defined */
};

#undef LDAPURLDesc
#define LDAPURLDesc கட்டமைப்பு ldap_urldesc

நிலையான curl_ldap_இலக்கம்_t ldap_url_parse_கீழ்(கட்டமைப்பு Curl_easy *data,
                                          மாறிலி கட்டமைப்பு connectdata *conn,
                                          LDAPURLDesc **ludpp);
நிலையான வெறுமை ldap_free_urldesc_கீழ்(LDAPURLDesc *ludp);

#undef ldap_free_urldesc
#define ldap_free_urldesc       ldap_free_urldesc_கீழ்

#endif /* !HAVE_LDAP_URL_PARSE */

#ifdef DEBUG_LDAP
#define LDAP_TRACE(x)                 \
  செய் {                                \
    ldap_trace_கீழ்("%u: ", __LINE__); \
    ldap_trace_கீழ் x;                 \
  } போது(0)

நிலையான வெறுமை ldap_trace_கீழ்(மாறிலி எழுத்து *fmt, ...) CURL_PRINTF(1, 2);
#இல்லையெனில்
#define LDAP_TRACE(x) Curl_nop_stmt
#endif

#எனில் defined(USE_WIN32_LDAP) && defined(ldap_err2string)
/* Use ANSI error strings in Unicode builds */
#undef ldap_err2string
#define ldap_err2string ldap_err2stringA
#endif

#எனில் defined(USE_WIN32_LDAP) && defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'void *' of greater size */
#undef LDAP_OPT_ON
#undef LDAP_OPT_OFF
#define LDAP_OPT_ON   ((வெறுமை *)(size_t)1)
#define LDAP_OPT_OFF  ((வெறுமை *)(size_t)0)
#endif

#ifdef USE_WIN32_LDAP

#ifdef USE_WINDOWS_SSPI
நிலையான ULONG ldap_win_bind_auth(LDAP *server, மாறிலி எழுத்து *user,
                                மாறிலி எழுத்து *passwd, unsigned long authflags)
{
  ULONG method = 0;
  SEC_WINNT_AUTH_IDENTITY_EX cred;
  ULONG rc = LDAP_AUTH_METHOD_NOT_SUPPORTED;

  memset(&cred, 0, அளவு(cred));

#ifdef USE_SPNEGO
  எனில்(authflags & CURLAUTH_NEGOTIATE) {
    method = LDAP_AUTH_NEGOTIATE;
  }
  இல்லையெனில்
#endif
#ifdef USE_NTLM
  எனில்(authflags & CURLAUTH_NTLM) {
    method = LDAP_AUTH_NTLM;
  }
  இல்லையெனில்
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  எனில்(authflags & CURLAUTH_DIGEST) {
    method = LDAP_AUTH_DIGEST;
  }
  இல்லையெனில்
#endif
  {
    /* required anyway if one of upper preprocessor definitions enabled */
  }

  எனில்(method && user && passwd) {
    CURLcode result = Curl_create_sspi_identity(user, passwd, &cred);
    எனில்(!result) {
      rc = ldap_bind_s(server, வெற்று, (TCHAR *)&cred, method);
      Curl_sspi_free_identity(&cred);
    }
    இல்லையெனில் {
      rc = LDAP_NO_MEMORY;
    }
  }
  இல்லையெனில் {
    /* proceed with current user credentials */
    method = LDAP_AUTH_NEGOTIATE;
    rc = ldap_bind_s(server, வெற்று, வெற்று, method);
  }
  திரும்பு rc;
}
#endif /* USE_WINDOWS_SSPI */

நிலையான ULONG ldap_win_bind(கட்டமைப்பு Curl_easy *data, LDAP *server,
                           மாறிலி எழுத்து *user, மாறிலி எழுத்து *passwd)
{
  ULONG rc = LDAP_INVALID_CREDENTIALS;

  PTCHAR inuser = வெற்று;
  PTCHAR inpass = வெற்று;

  எனில்(user && passwd && (data->set.httpauth & CURLAUTH_BASIC)) {
    inuser = curlx_convert_UTF8_to_tchar(user);
    inpass = curlx_convert_UTF8_to_tchar(passwd);

    rc = ldap_simple_bind_s(server, inuser, inpass);

    curlx_free(inuser);
    curlx_free(inpass);
  }
#ifdef USE_WINDOWS_SSPI
  இல்லையெனில் {
    rc = ldap_win_bind_auth(server, user, passwd, data->set.httpauth);
  }
#endif

  திரும்பு rc;
}
#endif /* USE_WIN32_LDAP */

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
நிலையான bool ldap_str_needs_base64(மாறிலி எழுத்து *s, size_t len)
{
  size_t i;
  எனில்(!len)
    திரும்பு FALSE;
  எனில்((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    திரும்பு TRUE;
  க்கு(i = 0; i < len; i++)
    எனில்(!ISPRINT(s[i]))
      திரும்பு TRUE;
  திரும்பு FALSE;
}

நிலையான bool ldap_மதிப்பு_needs_base64(மாறிலி எழுத்து *attr, size_t attr_len,
                                    மாறிலி Ber_மதிப்பு *val)
{
  எனில்((attr_len > 7) && curl_strequal(";binary", attr + attr_len - 7))
    திரும்பு TRUE;

  திரும்பு ldap_str_needs_base64(val->bv_val, val->bv_len);
}

#ifdef USE_WIN32_LDAP
நிலையான BOOLEAN bypass_cert_verify(PLDAP Connection,
                                  PCCERT_CONTEXT *ppServerCert)
{
  (வெறுமை)Connection;
  CertFreeCertificateContext(*ppServerCert);
  /* approve any certificate since the verification is set to bypass */
  திரும்பு TRUE;
}
#endif

நிலையான CURLcode show_vals(கட்டமைப்பு Curl_easy *data, Ber_மதிப்பு **vals,
                          மாறிலி எழுத்து *attr)
{
  CURLcode result = CURLE_OK;
  முழு i;
  size_t attr_len = strlen(attr);

  /* An attribute type has no base64 form in LDIF, so a type carrying control
     bytes (CR/LF etc.) would let a hostile server inject lines. A conformant
     type name never contains them, so reject it. */
  எனில்(ldap_str_needs_base64(attr, attr_len)) {
    failf(data, "LDAP local: attribute type contains illegal bytes");
    திரும்பு CURLE_WEIRD_SERVER_REPLY;
  }

  க்கு(i = 0; vals[i] && !result; i++) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, "\t", 1);
    எனில்(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, attr, attr_len);
    எனில்(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, ":", 1);

    எனில்(result)
      நிறுத்து;

    எனில்(ldap_மதிப்பு_needs_base64(attr, attr_len, vals[i])) {
      எழுத்து *val_b64 = வெற்று;
      size_t val_b64_sz = 0;

      /* Binary attribute, encode to base64. */
      எனில்(vals[i]->bv_len)
        result = curlx_base64_encode((uint8_t *)vals[i]->bv_val,
                                     vals[i]->bv_len,
                                     &val_b64, &val_b64_sz);
      எனில்(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, ": ", 2);
      எனில்(!result && val_b64_sz)
        result = Curl_client_write(data, CLIENTWRITE_BODY, val_b64,
                                   val_b64_sz);
      curlx_free(val_b64);
    }
    இல்லையெனில் {
      result = Curl_client_write(data, CLIENTWRITE_BODY, " ", 1);
      எனில்(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   vals[i]->bv_val, vals[i]->bv_len);
    }

    எனில்(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
  }
  திரும்பு result;
}

நிலையான CURLcode ldap_do(கட்டமைப்பு Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  curl_ldap_இலக்கம்_t rc = LDAP_SUCCESS;
  LDAP *server = வெற்று;
  LDAPURLDesc *ludp = வெற்று;
  LDAPMessage *ldapmsg = வெற்று;
  LDAPMessage *entryIterator;
  முழு இலக்கம் = 0;
  கட்டமைப்பு connectdata *conn = data->conn;
  முழு ldap_proto = LDAP_VERSION3;
  bool ldap_ssl = FALSE;
#ifdef LDAP_OPT_NETWORK_TIMEOUT
  கட்டமைப்பு timeval ldap_timeout = { 10, 0 }; /* 10s connection/search timeout */
#endif
#ifdef USE_WIN32_LDAP
  TCHAR *host = வெற்று;
#இல்லையெனில்
  எழுத்து *host = வெற்று;
#endif
  மாறிலி எழுத்து *user = data->state.creds ? data->state.creds->user : வெற்று;
  மாறிலி எழுத்து *passwd = data->state.creds ? data->state.creds->passwd : வெற்று;
  கட்டமைப்பு ip_quadruple ipquad;
  bool is_ipv6;
  BerElement *ber = வெற்று;

  *done = TRUE; /* unconditionally */
  infof(data, "LDAP local: LDAP Vendor = %s ; LDAP Version = %d",
        LDAP_VENDOR_NAME, LDAP_VENDOR_VERSION);
  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

#ifdef HAVE_LDAP_URL_PARSE
  rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), &ludp);
#இல்லையெனில்
  rc = ldap_url_parse_கீழ்(data, conn, &ludp);
#endif
  எனில்(rc) {
    failf(data, "Bad LDAP URL: %s", ldap_err2string(rc));
    result = CURLE_URL_MALFORMAT;
    செல் quit;
  }

  result = Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad);
  எனில்(result)
    செல் quit;

  /* Get the URL scheme (either ldap or ldaps) */
  ldap_ssl = Curl_conn_is_ssl(conn, FIRSTSOCKET);
  infof(data, "LDAP local: trying to establish %s connection",
        ldap_ssl ? "encrypted" : "cleartext");

#ifdef USE_WIN32_LDAP
  host = curlx_convert_UTF8_to_tchar(conn->origin->hostname);
  எனில்(!host) {
    result = CURLE_OUT_OF_MEMORY;

    செல் quit;
  }
#இல்லையெனில்
  host = conn->origin->hostname;
#endif

#ifdef USE_WIN32_LDAP
  எனில்(ldap_ssl)
    server = ldap_sslinit(host, (curl_ldap_இலக்கம்_t)ipquad.remote_port, 1);
  இல்லையெனில்
#endif
    server = ldap_init(host, (curl_ldap_இலக்கம்_t)ipquad.remote_port);
  எனில்(!server) {
    failf(data, "LDAP: cannot setup connect to %s:%u",
          conn->origin->user_hostname, ipquad.remote_port);
    result = CURLE_COULDNT_CONNECT;
    செல் quit;
  }

#ifdef LDAP_OPT_NETWORK_TIMEOUT
  ldap_set_option(server, LDAP_OPT_NETWORK_TIMEOUT, &ldap_timeout);
#endif
  ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);

  /* Do not chase referrals. */
  ldap_set_option(server, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  எனில்(ldap_ssl) {
#ifdef HAVE_LDAP_SSL
#ifdef USE_WIN32_LDAP
    /* Win32 LDAP uses the Windows CA store to verify certificates */
    ldap_set_option(server, LDAP_OPT_SSL, LDAP_OPT_ON);
    எனில்(!conn->ssl_config.verifypeer) {
      எனில்(conn->ssl_config.verifyhost) {
        failf(data, "LDAP local: host verification cannot be enabled when "
              "peer verification is disabled for Windows native LDAP");
        result = CURLE_NOT_BUILT_IN;
        செல் quit;
      }
      ldap_set_option(server, LDAP_OPT_SERVER_CERTIFICATE,
                      (வெறுமை *)(uintptr_t)bypass_cert_verify);
    }
#அல்லதெனில் defined(LDAP_OPT_X_TLS) /* !USE_WIN32_LDAP */
    முழு ldap_option;
    மாறிலி எழுத்து *ldap_ca = conn->ssl_config.CAfile;
    எனில்(conn->ssl_config.verifypeer) {
      /* OpenLDAP SDK supports BASE64 files. */
      எனில்(conn->ssl_config.cert_type &&
         !curl_strequal(conn->ssl_config.cert_type, "PEM")) {
        failf(data, "LDAP local: ERROR OpenLDAP only supports PEM cert-type");
        result = CURLE_SSL_CERTPROBLEM;
        செல் quit;
      }
      எனில்(!ldap_ca) {
        failf(data, "LDAP local: ERROR PEM CA cert not set");
        result = CURLE_SSL_CERTPROBLEM;
        செல் quit;
      }
      infof(data, "LDAP local: using PEM CA cert: %s", ldap_ca);
      rc = ldap_set_option(server, LDAP_OPT_X_TLS_CACERTFILE, ldap_ca);
      எனில்(rc != LDAP_SUCCESS) {
        failf(data, "LDAP local: ERROR setting PEM CA cert: %s",
              ldap_err2string(rc));
        result = CURLE_SSL_CERTPROBLEM;
        செல் quit;
      }
      ldap_option = LDAP_OPT_X_TLS_DEMAND;
    }
    இல்லையெனில்
      ldap_option = LDAP_OPT_X_TLS_NEVER;

    rc = ldap_set_option(server, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_option);
    எனில்(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting cert verify mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      செல் quit;
    }
    ldap_option = LDAP_OPT_X_TLS_HARD;
    rc = ldap_set_option(server, LDAP_OPT_X_TLS, &ldap_option);
    எனில்(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting SSL/TLS mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      செல் quit;
    }
#இல்லையெனில் /* !USE_WIN32_LDAP && !LDAP_OPT_X_TLS */
    /* we should probably never come up to here since configure
       should check in first place if we can support LDAP SSL/TLS */
    failf(data, "LDAP local: SSL/TLS not supported with this version "
          "of the OpenLDAP toolkit");
    result = CURLE_SSL_CERTPROBLEM;
    செல் quit;
#endif
#endif /* HAVE_LDAP_SSL */
  }
  இல்லையெனில் எனில்(data->set.use_ssl > CURLUSESSL_TRY) {
    failf(data, "LDAP local: explicit TLS not supported");
    result = CURLE_NOT_BUILT_IN;
    செல் quit;
  }

#ifdef USE_WIN32_LDAP
  rc = ldap_win_bind(data, server, user, passwd);
#இல்லையெனில்
  rc = ldap_simple_bind_s(server, user, passwd);
#endif
  எனில்(!ldap_ssl && rc != LDAP_SUCCESS) {
    ldap_proto = LDAP_VERSION2;
    ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);
#ifdef USE_WIN32_LDAP
    rc = ldap_win_bind(data, server, user, passwd);
#இல்லையெனில்
    rc = ldap_simple_bind_s(server, user, passwd);
#endif
  }
  எனில்(rc != LDAP_SUCCESS) {
#ifdef USE_WIN32_LDAP
    failf(data, "LDAP local: bind via ldap_win_bind %s",
          ldap_err2string(rc));
#இல்லையெனில்
    failf(data, "LDAP local: bind via ldap_simple_bind_s %s",
          ldap_err2string(rc));
#endif
    result = CURLE_LDAP_CANNOT_BIND;
    செல் quit;
  }

  Curl_pgrsReset(data);
  rc = ldap_தேடல்_s(server, ludp->lud_dn,
                     ludp->lud_scope,
                     ludp->lud_filter, ludp->lud_attrs, 0, &ldapmsg);

  எனில்(rc != LDAP_SUCCESS && rc != LDAP_SIZELIMIT_EXCEEDED) {
    failf(data, "LDAP remote: %s", ldap_err2string(rc));
    result = CURLE_LDAP_தேடல்_FAILED;
    செல் quit;
  }

  இலக்கம் = 0;
  க்கு(entryIterator = ldap_first_entry(server, ldapmsg);
      entryIterator;
      entryIterator = ldap_next_entry(server, entryIterator), இலக்கம்++) {
#ifdef USE_WIN32_LDAP
    TCHAR *attribute;
#இல்லையெனில்
    எழுத்து *attribute;
#endif

    /* Get the DN and write it to the client */
    {
      எழுத்து *name = வெற்று;
      size_t name_len = 0;
#ifdef USE_WIN32_LDAP
      TCHAR *dn = ldap_get_dn(server, entryIterator);
      எனில்(dn)
        name = curlx_convert_tchar_to_UTF8(dn);
#இல்லையெனில்
      எழுத்து *dn = name = ldap_get_dn(server, entryIterator);
#endif
      எனில்(!name)
        result = dn ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
      இல்லையெனில் {
        name_len = strlen(name);
        எனில்(ldap_str_needs_base64(name, name_len)) {
          எழுத்து *dn_b64 = வெற்று;
          size_t dn_b64_sz = 0;

          /* A DN with e.g. CR/LF would inject LDIF lines, so encode it */
          result = curlx_base64_encode((uint8_t *)name, name_len,
                                       &dn_b64, &dn_b64_sz);
          எனில்(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, "DN:: ", 5);
          எனில்(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, dn_b64,
                                       dn_b64_sz);
          curlx_free(dn_b64);
        }
        இல்லையெனில் {
          result = Curl_client_write(data, CLIENTWRITE_BODY, "DN: ", 4);
          எனில்(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, name, name_len);
        }
        எனில்(!result)
          result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      }
      FREE_ON_WINLDAP(name);
      ldap_memfree(dn);
      எனில்(result)
        செல் quit;
    }

    /* Get the attributes and write them to the client */
    க்கு(attribute = ldap_first_attribute(server, entryIterator, &ber);
        attribute;
        attribute = ldap_next_attribute(server, entryIterator, ber)) {
      Ber_மதிப்பு **vals;
#ifdef USE_WIN32_LDAP
      எழுத்து *attr = curlx_convert_tchar_to_UTF8(attribute);
      எனில்(!attr) {
        ldap_memfree(attribute);
        result = CURLE_OUT_OF_MEMORY;
        செல் quit;
      }
#இல்லையெனில்
      எழுத்து *attr = attribute;
#endif
      vals = ldap_get_மதிப்புகள்_len(server, entryIterator, attribute);
      எனில்(vals) {
        result = show_vals(data, vals, attr);
        /* Free memory used to store values */
        ldap_மதிப்பு_free_len(vals);
      }

      /* Free the attribute as we are done with it */
      FREE_ON_WINLDAP(attr);
      ldap_memfree(attribute);

      எனில்(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      எனில்(result)
        செல் quit;
    }

    எனில்(ber) {
      ber_free(ber, 0);
      ber = வெற்று;
    }
  }

quit:
  எனில்(ber)
    ber_free(ber, 0);
  எனில்(ldapmsg) {
    ldap_msgfree(ldapmsg);
    LDAP_TRACE(("Received %d entries\n", இலக்கம்));
  }
  எனில்(rc == LDAP_SIZELIMIT_EXCEEDED)
    infof(data, "There are more than %d entries", இலக்கம்);
  எனில்(ludp)
    ldap_free_urldesc(ludp);
  எனில்(server)
    ldap_unbind_s(server);

  FREE_ON_WINLDAP(host);

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  connclose(conn);

  திரும்பு result;
}

#ifdef DEBUG_LDAP
நிலையான வெறுமை ldap_trace_கீழ்(மாறிலி எழுத்து *fmt, ...)
{
  நிலையான முழு do_trace = -1;
  va_list args;

  எனில்(do_trace == -1) {
    மாறிலி எழுத்து *env = getenv("CURL_TRACE");
    curl_off_t e = 0;
    எனில்(!curlx_str_எண்(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  எனில்(!do_trace)
    திரும்பு;

  va_start(args, fmt);
  curl_mvfprintf(stderr, fmt, args);
  va_end(args);
}
#endif /* DEBUG_LDAP */

#ifndef HAVE_LDAP_URL_PARSE

/*
 * Return scope-value for a scope-string.
 */
நிலையான முழு str2scope(மாறிலி எழுத்து *p)
{
  எனில்(curl_strequal(p, "one"))
    திரும்பு LDAP_SCOPE_ONELEVEL;
  எனில்(curl_strequal(p, "onetree"))
    திரும்பு LDAP_SCOPE_ONELEVEL;
  எனில்(curl_strequal(p, "base"))
    திரும்பு LDAP_SCOPE_BASE;
  எனில்(curl_strequal(p, "sub"))
    திரும்பு LDAP_SCOPE_SUBTREE;
  எனில்(curl_strequal(p, "subtree"))
    திரும்பு LDAP_SCOPE_SUBTREE;
  திரும்பு -1;
}

/* number of entries in the attributes list */
நிலையான size_t இலக்கம்_entries(மாறிலி எழுத்து *s)
{
  size_t பொருட்கள் = 1;

  s = strchr(s, ',');
  போது(s) {
    பொருட்கள்++;
    s = strchr(s + 1, ',');
  }
  திரும்பு பொருட்கள்;
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
நிலையான curl_ldap_இலக்கம்_t ldap_url_parse2_கீழ்(கட்டமைப்பு Curl_easy *data,
                                           மாறிலி கட்டமைப்பு connectdata *conn,
                                           LDAPURLDesc *ludp)
{
  curl_ldap_இலக்கம்_t rc = LDAP_SUCCESS;
  எழுத்து *p;
  எழுத்து *path;
  எழுத்து *q = வெற்று;
  எழுத்து *query = வெற்று;
  size_t i;

  எனில்(!data ||
     !data->state.up.path ||
     data->state.up.path[0] != '/' ||
     ((data->state.origin->scheme != &Curl_scheme_ldap) &&
      (data->state.origin->scheme != &Curl_scheme_ldaps)))
    திரும்பு LDAP_INVALID_SYNTAX;

  ludp->lud_scope = LDAP_SCOPE_BASE;
  ludp->lud_port  = conn->origin->port;
  ludp->lud_host  = conn->origin->hostname;

  /* Duplicate the path */
  p = path = curlx_strdup(data->state.up.path + 1);
  எனில்(!path)
    திரும்பு LDAP_NO_MEMORY;

  /* Duplicate the query if present */
  எனில்(data->state.up.query) {
    q = query = curlx_strdup(data->state.up.query);
    எனில்(!query) {
      curlx_free(path);
      திரும்பு LDAP_NO_MEMORY;
    }
  }

  /* Parse the DN (Distinguished Name) */
  எனில்(*p) {
    எழுத்து *dn = p;
    எழுத்து *unescaped;
    CURLcode result;

    LDAP_TRACE(("DN '%s'\n", dn));

    /* Unescape the DN */
    result = Curl_urldecode(dn, 0, &unescaped, வெற்று, REJECT_ZERO);
    எனில்(result) {
      rc = LDAP_NO_MEMORY;

      செல் quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_dn = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    எனில்(!ludp->lud_dn) {
      rc = LDAP_NO_MEMORY;

      செல் quit;
    }
#இல்லையெனில்
    ludp->lud_dn = unescaped;
#endif
  }

  p = q;
  எனில்(!p)
    செல் quit;

  /* Parse the attributes. skip "??" */
  q = strchr(p, '?');
  எனில்(q)
    *q++ = '\0';

  எனில்(*p) {
    size_t மொத்தம் = இலக்கம்_entries(p); /* at least one */
    மாறிலி எழுத்து *atp = p;

    /* Allocate our array (+1 for the NULL entry) */
#ifdef USE_WIN32_LDAP
    ludp->lud_attrs = curlx_calloc(மொத்தம் + 1, அளவு(TCHAR *));
#இல்லையெனில்
    ludp->lud_attrs = curlx_calloc(மொத்தம் + 1, அளவு(எழுத்து *));
#endif
    எனில்(!ludp->lud_attrs) {
      rc = LDAP_NO_MEMORY;
      செல் quit;
    }

    க்கு(i = 0; i < மொத்தம்; i++) {
      எழுத்து *unescaped;
      CURLcode result;
      கட்டமைப்பு Curl_str out;

      எனில்(curlx_str_until(&atp, &out, 1024, ','))
        நிறுத்து;

      LDAP_TRACE(("attr[%zu] '%.*s'\n", i, (முழு)out.len, out.str));

      /* Unescape the attribute */
      result = Curl_urldecode(out.str, out.len, &unescaped, வெற்று, REJECT_ZERO);
      எனில்(result) {
        rc = LDAP_NO_MEMORY;
        செல் quit;
      }

#ifdef USE_WIN32_LDAP
      /* Convert the unescaped string to a tchar */
      ludp->lud_attrs[i] = curlx_convert_UTF8_to_tchar(unescaped);

      /* Free the unescaped string as we are done with it */
      curlx_free(unescaped);

      எனில்(!ludp->lud_attrs[i]) {
        rc = LDAP_NO_MEMORY;
        செல் quit;
      }
#இல்லையெனில்
      ludp->lud_attrs[i] = unescaped;
#endif

      ludp->lud_attrs_dups++;
      எனில்(curlx_str_single(&atp, ','))
        நிறுத்து;
    }
  }

  p = q;
  எனில்(!p)
    செல் quit;

  /* Parse the scope. skip "??" */
  q = strchr(p, '?');
  எனில்(q)
    *q++ = '\0';

  எனில்(*p) {
    ludp->lud_scope = str2scope(p);
    எனில்(ludp->lud_scope == -1) {
      rc = LDAP_INVALID_SYNTAX;

      செல் quit;
    }
    LDAP_TRACE(("scope %d\n", ludp->lud_scope));
  }

  p = q;
  எனில்(!p)
    செல் quit;

  /* Parse the filter */
  q = strchr(p, '?');
  எனில்(q)
    *q++ = '\0';

  எனில்(*p) {
    எழுத்து *filter = p;
    எழுத்து *unescaped;
    CURLcode result;

    LDAP_TRACE(("filter '%s'\n", filter));

    /* Unescape the filter */
    result = Curl_urldecode(filter, 0, &unescaped, வெற்று, REJECT_CTRL);
    எனில்(result) {
      rc = LDAP_NO_MEMORY;

      செல் quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_filter = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    எனில்(!ludp->lud_filter) {
      rc = LDAP_NO_MEMORY;

      செல் quit;
    }
#இல்லையெனில்
    ludp->lud_filter = unescaped;
#endif
  }

  p = q;
  எனில்(p && !*p) {
    rc = LDAP_INVALID_SYNTAX;

    செல் quit;
  }

quit:
  curlx_free(path);
  curlx_free(query);

  திரும்பு rc;
}

நிலையான curl_ldap_இலக்கம்_t ldap_url_parse_கீழ்(கட்டமைப்பு Curl_easy *data,
                                          மாறிலி கட்டமைப்பு connectdata *conn,
                                          LDAPURLDesc **ludpp)
{
  LDAPURLDesc *ludp = curlx_calloc(1, அளவு(*ludp));
  curl_ldap_இலக்கம்_t rc;

  *ludpp = வெற்று;
  எனில்(!ludp)
    திரும்பு LDAP_NO_MEMORY;

  rc = ldap_url_parse2_கீழ்(data, conn, ludp);
  எனில்(rc != LDAP_SUCCESS) {
    ldap_free_urldesc_கீழ்(ludp);
    ludp = வெற்று;
  }
  *ludpp = ludp;
  திரும்பு rc;
}

நிலையான வெறுமை ldap_free_urldesc_கீழ்(LDAPURLDesc *ludp)
{
  எனில்(!ludp)
    திரும்பு;

  curlx_free(ludp->lud_dn);
  curlx_free(ludp->lud_filter);

  எனில்(ludp->lud_attrs) {
    size_t i;
    க்கு(i = 0; i < ludp->lud_attrs_dups; i++) {
      curlx_free(ludp->lud_attrs[i]);
    }
    curlx_free(ludp->lud_attrs);
  }

  curlx_free(ludp);
}
#endif /* !HAVE_LDAP_URL_PARSE */

வெறுமை Curl_ldap_version(எழுத்து *buf, size_t bufsz)
{
#ifdef USE_WIN32_LDAP
  curl_msnprintf(buf, bufsz, "WinLDAP");
#இல்லையெனில்
#ifdef LDAP_OPT_X_TLS_PASSPHRASE
  நிலையான மாறிலி எழுத்து flavor[] = "/Apple";
#இல்லையெனில்
  நிலையான மாறிலி எழுத்து flavor[] = "";
#endif
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  /* Comparing against 0, as different platforms
     disagree on the success define name */
  எனில்(ldap_get_option(வெற்று, LDAP_OPT_API_INFO, &api) == 0) {
    unsigned முழு patch = (unsigned முழு)(api.ldapai_vendor_version % 100);
    unsigned முழு major = (unsigned முழு)(api.ldapai_vendor_version / 10000);
    unsigned முழு minor =
      (((unsigned முழு)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
#ifdef __OS400__
    curl_msnprintf(buf, bufsz, "IBMLDAP/%u.%u.%u", major, minor, patch);
    ldap_மதிப்பு_free(api.ldapai_extensions);
    (வெறுமை)flavor;
#இல்லையெனில்
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u%s",
                   api.ldapai_vendor_name, major, minor, patch, flavor);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((வெறுமை **)api.ldapai_extensions);
#endif
  }
  இல்லையெனில்
    curl_msnprintf(buf, bufsz, "LDAP/1");
#endif /* USE_WIN32_LDAP */
}

/*
 * LDAP protocol handler.
 */
மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_ldap = {
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

#எனில் defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic எடு
#endif

#endif /* !CURL_DISABLE_LDAP && !USE_OPENLDAP */
