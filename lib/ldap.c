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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_ldap.h"

#सचे !defined(CURL_DISABLE_LDAP) && !defined(USE_OPENLDAP)

#सचे defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic प्रक्षिप
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
#अञ्ञथा
#  define LDAP_DEPRECATED 1     /* Be sure ldap_init() is defined. */
#  ifdef HAVE_LBER_H
#    include <lber.h>
#  endif
#  include <ldap.h>
#  सचे defined(HAVE_LDAP_SSL) && defined(HAVE_LDAP_SSL_H)
#    include <ldap_ssl.h>
#  endif /* HAVE_LDAP_SSL && HAVE_LDAP_SSL_H */
#endif

#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "curlx/multibyte.h"
#अन्तर्भाव "curlx/base64.h"
#अन्तर्भाव "connect.h"

#ifdef USE_WIN32_LDAP
#define FREE_ON_WINLDAP(x) curlx_free(x)
#define curl_ldap_अङ्कः_t    ULONG
#अञ्ञथा
#define FREE_ON_WINLDAP(x) करोहि {} याव(0)
#define curl_ldap_अङ्कः_t    पूर्णाङ्क
#endif

#ifndef HAVE_LDAP_URL_PARSE

/* Use our own implementation. */

संरचना ldap_urldesc {
  वर्ण   *lud_host;
  पूर्णाङ्क     lud_port;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_dn;
  TCHAR **lud_attrs;
#अञ्ञथा
  वर्ण   *lud_dn;
  वर्ण  **lud_attrs;
#endif
  पूर्णाङ्क     lud_scope;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_filter;
#अञ्ञथा
  वर्ण   *lud_filter;
#endif
  वर्ण  **lud_exts;
  size_t  lud_attrs_dups; /* how many were dup'ed, this field is not in the
                             "real" struct so can only be used in code
                             without HAVE_LDAP_URL_PARSE defined */
};

#undef LDAPURLDesc
#define LDAPURLDesc संरचना ldap_urldesc

स्थिर curl_ldap_अङ्कः_t ldap_url_parse_नीचः(संरचना Curl_easy *data,
                                          ध्रुव संरचना connectdata *conn,
                                          LDAPURLDesc **ludpp);
स्थिर शून्य ldap_free_urldesc_नीचः(LDAPURLDesc *ludp);

#undef ldap_free_urldesc
#define ldap_free_urldesc       ldap_free_urldesc_नीचः

#endif /* !HAVE_LDAP_URL_PARSE */

#ifdef DEBUG_LDAP
#define LDAP_TRACE(x)                 \
  करोहि {                                \
    ldap_trace_नीचः("%u: ", __LINE__); \
    ldap_trace_नीचः x;                 \
  } याव(0)

स्थिर शून्य ldap_trace_नीचः(ध्रुव वर्ण *fmt, ...) CURL_PRINTF(1, 2);
#अञ्ञथा
#define LDAP_TRACE(x) Curl_nop_stmt
#endif

#सचे defined(USE_WIN32_LDAP) && defined(ldap_err2string)
/* Use ANSI error strings in Unicode builds */
#undef ldap_err2string
#define ldap_err2string ldap_err2stringA
#endif

#सचे defined(USE_WIN32_LDAP) && defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'void *' of greater size */
#undef LDAP_OPT_ON
#undef LDAP_OPT_OFF
#define LDAP_OPT_ON   ((शून्य *)(size_t)1)
#define LDAP_OPT_OFF  ((शून्य *)(size_t)0)
#endif

#ifdef USE_WIN32_LDAP

#ifdef USE_WINDOWS_SSPI
स्थिर ULONG ldap_win_bind_auth(LDAP *server, ध्रुव वर्ण *user,
                                ध्रुव वर्ण *passwd, unsigned long authflags)
{
  ULONG method = 0;
  SEC_WINNT_AUTH_IDENTITY_EX cred;
  ULONG rc = LDAP_AUTH_METHOD_NOT_SUPPORTED;

  memset(&cred, 0, परिमाण(cred));

#ifdef USE_SPNEGO
  सचे(authflags & CURLAUTH_NEGOTIATE) {
    method = LDAP_AUTH_NEGOTIATE;
  }
  अञ्ञथा
#endif
#ifdef USE_NTLM
  सचे(authflags & CURLAUTH_NTLM) {
    method = LDAP_AUTH_NTLM;
  }
  अञ्ञथा
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  सचे(authflags & CURLAUTH_DIGEST) {
    method = LDAP_AUTH_DIGEST;
  }
  अञ्ञथा
#endif
  {
    /* required anyway if one of upper preprocessor definitions enabled */
  }

  सचे(method && user && passwd) {
    CURLcode result = Curl_create_sspi_identity(user, passwd, &cred);
    सचे(!result) {
      rc = ldap_bind_s(server, रिक्त, (TCHAR *)&cred, method);
      Curl_sspi_free_identity(&cred);
    }
    अञ्ञथा {
      rc = LDAP_NO_MEMORY;
    }
  }
  अञ्ञथा {
    /* proceed with current user credentials */
    method = LDAP_AUTH_NEGOTIATE;
    rc = ldap_bind_s(server, रिक्त, रिक्त, method);
  }
  पटिनिवत्त rc;
}
#endif /* USE_WINDOWS_SSPI */

स्थिर ULONG ldap_win_bind(संरचना Curl_easy *data, LDAP *server,
                           ध्रुव वर्ण *user, ध्रुव वर्ण *passwd)
{
  ULONG rc = LDAP_INVALID_CREDENTIALS;

  PTCHAR inuser = रिक्त;
  PTCHAR inpass = रिक्त;

  सचे(user && passwd && (data->set.httpauth & CURLAUTH_BASIC)) {
    inuser = curlx_convert_UTF8_to_tchar(user);
    inpass = curlx_convert_UTF8_to_tchar(passwd);

    rc = ldap_simple_bind_s(server, inuser, inpass);

    curlx_free(inuser);
    curlx_free(inpass);
  }
#ifdef USE_WINDOWS_SSPI
  अञ्ञथा {
    rc = ldap_win_bind_auth(server, user, passwd, data->set.httpauth);
  }
#endif

  पटिनिवत्त rc;
}
#endif /* USE_WIN32_LDAP */

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
स्थिर bool ldap_str_needs_base64(ध्रुव वर्ण *s, size_t len)
{
  size_t i;
  सचे(!len)
    पटिनिवत्त FALSE;
  सचे((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    पटिनिवत्त TRUE;
  अत्थाय(i = 0; i < len; i++)
    सचे(!ISPRINT(s[i]))
      पटिनिवत्त TRUE;
  पटिनिवत्त FALSE;
}

स्थिर bool ldap_मानम्_needs_base64(ध्रुव वर्ण *attr, size_t attr_len,
                                    ध्रुव Ber_मानम् *val)
{
  सचे((attr_len > 7) && curl_strequal(";binary", attr + attr_len - 7))
    पटिनिवत्त TRUE;

  पटिनिवत्त ldap_str_needs_base64(val->bv_val, val->bv_len);
}

#ifdef USE_WIN32_LDAP
स्थिर BOOLEAN bypass_cert_verify(PLDAP Connection,
                                  PCCERT_CONTEXT *ppServerCert)
{
  (शून्य)Connection;
  CertFreeCertificateContext(*ppServerCert);
  /* approve any certificate since the verification is set to bypass */
  पटिनिवत्त TRUE;
}
#endif

स्थिर CURLcode show_vals(संरचना Curl_easy *data, Ber_मानम् **vals,
                          ध्रुव वर्ण *attr)
{
  CURLcode result = CURLE_OK;
  पूर्णाङ्क i;
  size_t attr_len = strlen(attr);

  /* An attribute type has no base64 form in LDIF, so a type carrying control
     bytes (CR/LF etc.) would let a hostile server inject lines. A conformant
     type name never contains them, so reject it. */
  सचे(ldap_str_needs_base64(attr, attr_len)) {
    failf(data, "LDAP local: attribute type contains illegal bytes");
    पटिनिवत्त CURLE_WEIRD_SERVER_REPLY;
  }

  अत्थाय(i = 0; vals[i] && !result; i++) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, "\t", 1);
    सचे(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, attr, attr_len);
    सचे(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, ":", 1);

    सचे(result)
      भङ्ग;

    सचे(ldap_मानम्_needs_base64(attr, attr_len, vals[i])) {
      वर्ण *val_b64 = रिक्त;
      size_t val_b64_sz = 0;

      /* Binary attribute, encode to base64. */
      सचे(vals[i]->bv_len)
        result = curlx_base64_encode((uint8_t *)vals[i]->bv_val,
                                     vals[i]->bv_len,
                                     &val_b64, &val_b64_sz);
      सचे(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, ": ", 2);
      सचे(!result && val_b64_sz)
        result = Curl_client_write(data, CLIENTWRITE_BODY, val_b64,
                                   val_b64_sz);
      curlx_free(val_b64);
    }
    अञ्ञथा {
      result = Curl_client_write(data, CLIENTWRITE_BODY, " ", 1);
      सचे(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   vals[i]->bv_val, vals[i]->bv_len);
    }

    सचे(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
  }
  पटिनिवत्त result;
}

स्थिर CURLcode ldap_do(संरचना Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  curl_ldap_अङ्कः_t rc = LDAP_SUCCESS;
  LDAP *server = रिक्त;
  LDAPURLDesc *ludp = रिक्त;
  LDAPMessage *ldapmsg = रिक्त;
  LDAPMessage *entryIterator;
  पूर्णाङ्क अङ्कः = 0;
  संरचना connectdata *conn = data->conn;
  पूर्णाङ्क ldap_proto = LDAP_VERSION3;
  bool ldap_ssl = FALSE;
#ifdef LDAP_OPT_NETWORK_TIMEOUT
  संरचना timeval ldap_timeout = { 10, 0 }; /* 10s connection/search timeout */
#endif
#ifdef USE_WIN32_LDAP
  TCHAR *host = रिक्त;
#अञ्ञथा
  वर्ण *host = रिक्त;
#endif
  ध्रुव वर्ण *user = data->state.creds ? data->state.creds->user : रिक्त;
  ध्रुव वर्ण *passwd = data->state.creds ? data->state.creds->passwd : रिक्त;
  संरचना ip_quadruple ipquad;
  bool is_ipv6;
  BerElement *ber = रिक्त;

  *done = TRUE; /* unconditionally */
  infof(data, "LDAP local: LDAP Vendor = %s ; LDAP Version = %d",
        LDAP_VENDOR_NAME, LDAP_VENDOR_VERSION);
  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

#ifdef HAVE_LDAP_URL_PARSE
  rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), &ludp);
#अञ्ञथा
  rc = ldap_url_parse_नीचः(data, conn, &ludp);
#endif
  सचे(rc) {
    failf(data, "Bad LDAP URL: %s", ldap_err2string(rc));
    result = CURLE_URL_MALFORMAT;
    गच्छ quit;
  }

  result = Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad);
  सचे(result)
    गच्छ quit;

  /* Get the URL scheme (either ldap or ldaps) */
  ldap_ssl = Curl_conn_is_ssl(conn, FIRSTSOCKET);
  infof(data, "LDAP local: trying to establish %s connection",
        ldap_ssl ? "encrypted" : "cleartext");

#ifdef USE_WIN32_LDAP
  host = curlx_convert_UTF8_to_tchar(conn->origin->hostname);
  सचे(!host) {
    result = CURLE_OUT_OF_MEMORY;

    गच्छ quit;
  }
#अञ्ञथा
  host = conn->origin->hostname;
#endif

#ifdef USE_WIN32_LDAP
  सचे(ldap_ssl)
    server = ldap_sslinit(host, (curl_ldap_अङ्कः_t)ipquad.remote_port, 1);
  अञ्ञथा
#endif
    server = ldap_init(host, (curl_ldap_अङ्कः_t)ipquad.remote_port);
  सचे(!server) {
    failf(data, "LDAP: cannot setup connect to %s:%u",
          conn->origin->user_hostname, ipquad.remote_port);
    result = CURLE_COULDNT_CONNECT;
    गच्छ quit;
  }

#ifdef LDAP_OPT_NETWORK_TIMEOUT
  ldap_set_option(server, LDAP_OPT_NETWORK_TIMEOUT, &ldap_timeout);
#endif
  ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);

  /* Do not chase referrals. */
  ldap_set_option(server, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  सचे(ldap_ssl) {
#ifdef HAVE_LDAP_SSL
#ifdef USE_WIN32_LDAP
    /* Win32 LDAP uses the Windows CA store to verify certificates */
    ldap_set_option(server, LDAP_OPT_SSL, LDAP_OPT_ON);
    सचे(!conn->ssl_config.verifypeer) {
      सचे(conn->ssl_config.verifyhost) {
        failf(data, "LDAP local: host verification cannot be enabled when "
              "peer verification is disabled for Windows native LDAP");
        result = CURLE_NOT_BUILT_IN;
        गच्छ quit;
      }
      ldap_set_option(server, LDAP_OPT_SERVER_CERTIFICATE,
                      (शून्य *)(uintptr_t)bypass_cert_verify);
    }
#नोचेद्यदि defined(LDAP_OPT_X_TLS) /* !USE_WIN32_LDAP */
    पूर्णाङ्क ldap_option;
    ध्रुव वर्ण *ldap_ca = conn->ssl_config.CAfile;
    सचे(conn->ssl_config.verifypeer) {
      /* OpenLDAP SDK supports BASE64 files. */
      सचे(conn->ssl_config.cert_type &&
         !curl_strequal(conn->ssl_config.cert_type, "PEM")) {
        failf(data, "LDAP local: ERROR OpenLDAP only supports PEM cert-type");
        result = CURLE_SSL_CERTPROBLEM;
        गच्छ quit;
      }
      सचे(!ldap_ca) {
        failf(data, "LDAP local: ERROR PEM CA cert not set");
        result = CURLE_SSL_CERTPROBLEM;
        गच्छ quit;
      }
      infof(data, "LDAP local: using PEM CA cert: %s", ldap_ca);
      rc = ldap_set_option(server, LDAP_OPT_X_TLS_CACERTFILE, ldap_ca);
      सचे(rc != LDAP_SUCCESS) {
        failf(data, "LDAP local: ERROR setting PEM CA cert: %s",
              ldap_err2string(rc));
        result = CURLE_SSL_CERTPROBLEM;
        गच्छ quit;
      }
      ldap_option = LDAP_OPT_X_TLS_DEMAND;
    }
    अञ्ञथा
      ldap_option = LDAP_OPT_X_TLS_NEVER;

    rc = ldap_set_option(server, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_option);
    सचे(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting cert verify mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      गच्छ quit;
    }
    ldap_option = LDAP_OPT_X_TLS_HARD;
    rc = ldap_set_option(server, LDAP_OPT_X_TLS, &ldap_option);
    सचे(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting SSL/TLS mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      गच्छ quit;
    }
#अञ्ञथा /* !USE_WIN32_LDAP && !LDAP_OPT_X_TLS */
    /* we should probably never come up to here since configure
       should check in first place if we can support LDAP SSL/TLS */
    failf(data, "LDAP local: SSL/TLS not supported with this version "
          "of the OpenLDAP toolkit");
    result = CURLE_SSL_CERTPROBLEM;
    गच्छ quit;
#endif
#endif /* HAVE_LDAP_SSL */
  }
  अञ्ञथा सचे(data->set.use_ssl > CURLUSESSL_TRY) {
    failf(data, "LDAP local: explicit TLS not supported");
    result = CURLE_NOT_BUILT_IN;
    गच्छ quit;
  }

#ifdef USE_WIN32_LDAP
  rc = ldap_win_bind(data, server, user, passwd);
#अञ्ञथा
  rc = ldap_simple_bind_s(server, user, passwd);
#endif
  सचे(!ldap_ssl && rc != LDAP_SUCCESS) {
    ldap_proto = LDAP_VERSION2;
    ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);
#ifdef USE_WIN32_LDAP
    rc = ldap_win_bind(data, server, user, passwd);
#अञ्ञथा
    rc = ldap_simple_bind_s(server, user, passwd);
#endif
  }
  सचे(rc != LDAP_SUCCESS) {
#ifdef USE_WIN32_LDAP
    failf(data, "LDAP local: bind via ldap_win_bind %s",
          ldap_err2string(rc));
#अञ्ञथा
    failf(data, "LDAP local: bind via ldap_simple_bind_s %s",
          ldap_err2string(rc));
#endif
    result = CURLE_LDAP_CANNOT_BIND;
    गच्छ quit;
  }

  Curl_pgrsReset(data);
  rc = ldap_अन्वेषणम्_s(server, ludp->lud_dn,
                     ludp->lud_scope,
                     ludp->lud_filter, ludp->lud_attrs, 0, &ldapmsg);

  सचे(rc != LDAP_SUCCESS && rc != LDAP_SIZELIMIT_EXCEEDED) {
    failf(data, "LDAP remote: %s", ldap_err2string(rc));
    result = CURLE_LDAP_अन्वेषणम्_FAILED;
    गच्छ quit;
  }

  अङ्कः = 0;
  अत्थाय(entryIterator = ldap_first_entry(server, ldapmsg);
      entryIterator;
      entryIterator = ldap_next_entry(server, entryIterator), अङ्कः++) {
#ifdef USE_WIN32_LDAP
    TCHAR *attribute;
#अञ्ञथा
    वर्ण *attribute;
#endif

    /* Get the DN and write it to the client */
    {
      वर्ण *name = रिक्त;
      size_t name_len = 0;
#ifdef USE_WIN32_LDAP
      TCHAR *dn = ldap_get_dn(server, entryIterator);
      सचे(dn)
        name = curlx_convert_tchar_to_UTF8(dn);
#अञ्ञथा
      वर्ण *dn = name = ldap_get_dn(server, entryIterator);
#endif
      सचे(!name)
        result = dn ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
      अञ्ञथा {
        name_len = strlen(name);
        सचे(ldap_str_needs_base64(name, name_len)) {
          वर्ण *dn_b64 = रिक्त;
          size_t dn_b64_sz = 0;

          /* A DN with e.g. CR/LF would inject LDIF lines, so encode it */
          result = curlx_base64_encode((uint8_t *)name, name_len,
                                       &dn_b64, &dn_b64_sz);
          सचे(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, "DN:: ", 5);
          सचे(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, dn_b64,
                                       dn_b64_sz);
          curlx_free(dn_b64);
        }
        अञ्ञथा {
          result = Curl_client_write(data, CLIENTWRITE_BODY, "DN: ", 4);
          सचे(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, name, name_len);
        }
        सचे(!result)
          result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      }
      FREE_ON_WINLDAP(name);
      ldap_memfree(dn);
      सचे(result)
        गच्छ quit;
    }

    /* Get the attributes and write them to the client */
    अत्थाय(attribute = ldap_first_attribute(server, entryIterator, &ber);
        attribute;
        attribute = ldap_next_attribute(server, entryIterator, ber)) {
      Ber_मानम् **vals;
#ifdef USE_WIN32_LDAP
      वर्ण *attr = curlx_convert_tchar_to_UTF8(attribute);
      सचे(!attr) {
        ldap_memfree(attribute);
        result = CURLE_OUT_OF_MEMORY;
        गच्छ quit;
      }
#अञ्ञथा
      वर्ण *attr = attribute;
#endif
      vals = ldap_get_मानानि_len(server, entryIterator, attribute);
      सचे(vals) {
        result = show_vals(data, vals, attr);
        /* Free memory used to store values */
        ldap_मानम्_free_len(vals);
      }

      /* Free the attribute as we are done with it */
      FREE_ON_WINLDAP(attr);
      ldap_memfree(attribute);

      सचे(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      सचे(result)
        गच्छ quit;
    }

    सचे(ber) {
      ber_free(ber, 0);
      ber = रिक्त;
    }
  }

quit:
  सचे(ber)
    ber_free(ber, 0);
  सचे(ldapmsg) {
    ldap_msgfree(ldapmsg);
    LDAP_TRACE(("Received %d entries\n", अङ्कः));
  }
  सचे(rc == LDAP_SIZELIMIT_EXCEEDED)
    infof(data, "There are more than %d entries", अङ्कः);
  सचे(ludp)
    ldap_free_urldesc(ludp);
  सचे(server)
    ldap_unbind_s(server);

  FREE_ON_WINLDAP(host);

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  connclose(conn);

  पटिनिवत्त result;
}

#ifdef DEBUG_LDAP
स्थिर शून्य ldap_trace_नीचः(ध्रुव वर्ण *fmt, ...)
{
  स्थिर पूर्णाङ्क do_trace = -1;
  va_list args;

  सचे(do_trace == -1) {
    ध्रुव वर्ण *env = getenv("CURL_TRACE");
    curl_off_t e = 0;
    सचे(!curlx_str_सङ्ख्या(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  सचे(!do_trace)
    पटिनिवत्त;

  va_start(args, fmt);
  curl_mvfprintf(stderr, fmt, args);
  va_end(args);
}
#endif /* DEBUG_LDAP */

#ifndef HAVE_LDAP_URL_PARSE

/*
 * Return scope-value for a scope-string.
 */
स्थिर पूर्णाङ्क str2scope(ध्रुव वर्ण *p)
{
  सचे(curl_strequal(p, "one"))
    पटिनिवत्त LDAP_SCOPE_ONELEVEL;
  सचे(curl_strequal(p, "onetree"))
    पटिनिवत्त LDAP_SCOPE_ONELEVEL;
  सचे(curl_strequal(p, "base"))
    पटिनिवत्त LDAP_SCOPE_BASE;
  सचे(curl_strequal(p, "sub"))
    पटिनिवत्त LDAP_SCOPE_SUBTREE;
  सचे(curl_strequal(p, "subtree"))
    पटिनिवत्त LDAP_SCOPE_SUBTREE;
  पटिनिवत्त -1;
}

/* number of entries in the attributes list */
स्थिर size_t अङ्कः_entries(ध्रुव वर्ण *s)
{
  size_t वस्तूनि = 1;

  s = strchr(s, ',');
  याव(s) {
    वस्तूनि++;
    s = strchr(s + 1, ',');
  }
  पटिनिवत्त वस्तूनि;
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
स्थिर curl_ldap_अङ्कः_t ldap_url_parse2_नीचः(संरचना Curl_easy *data,
                                           ध्रुव संरचना connectdata *conn,
                                           LDAPURLDesc *ludp)
{
  curl_ldap_अङ्कः_t rc = LDAP_SUCCESS;
  वर्ण *p;
  वर्ण *path;
  वर्ण *q = रिक्त;
  वर्ण *query = रिक्त;
  size_t i;

  सचे(!data ||
     !data->state.up.path ||
     data->state.up.path[0] != '/' ||
     ((data->state.origin->scheme != &Curl_scheme_ldap) &&
      (data->state.origin->scheme != &Curl_scheme_ldaps)))
    पटिनिवत्त LDAP_INVALID_SYNTAX;

  ludp->lud_scope = LDAP_SCOPE_BASE;
  ludp->lud_port  = conn->origin->port;
  ludp->lud_host  = conn->origin->hostname;

  /* Duplicate the path */
  p = path = curlx_strdup(data->state.up.path + 1);
  सचे(!path)
    पटिनिवत्त LDAP_NO_MEMORY;

  /* Duplicate the query if present */
  सचे(data->state.up.query) {
    q = query = curlx_strdup(data->state.up.query);
    सचे(!query) {
      curlx_free(path);
      पटिनिवत्त LDAP_NO_MEMORY;
    }
  }

  /* Parse the DN (Distinguished Name) */
  सचे(*p) {
    वर्ण *dn = p;
    वर्ण *unescaped;
    CURLcode result;

    LDAP_TRACE(("DN '%s'\n", dn));

    /* Unescape the DN */
    result = Curl_urldecode(dn, 0, &unescaped, रिक्त, REJECT_ZERO);
    सचे(result) {
      rc = LDAP_NO_MEMORY;

      गच्छ quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_dn = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    सचे(!ludp->lud_dn) {
      rc = LDAP_NO_MEMORY;

      गच्छ quit;
    }
#अञ्ञथा
    ludp->lud_dn = unescaped;
#endif
  }

  p = q;
  सचे(!p)
    गच्छ quit;

  /* Parse the attributes. skip "??" */
  q = strchr(p, '?');
  सचे(q)
    *q++ = '\0';

  सचे(*p) {
    size_t गणनम् = अङ्कः_entries(p); /* at least one */
    ध्रुव वर्ण *atp = p;

    /* Allocate our array (+1 for the NULL entry) */
#ifdef USE_WIN32_LDAP
    ludp->lud_attrs = curlx_calloc(गणनम् + 1, परिमाण(TCHAR *));
#अञ्ञथा
    ludp->lud_attrs = curlx_calloc(गणनम् + 1, परिमाण(वर्ण *));
#endif
    सचे(!ludp->lud_attrs) {
      rc = LDAP_NO_MEMORY;
      गच्छ quit;
    }

    अत्थाय(i = 0; i < गणनम्; i++) {
      वर्ण *unescaped;
      CURLcode result;
      संरचना Curl_str out;

      सचे(curlx_str_until(&atp, &out, 1024, ','))
        भङ्ग;

      LDAP_TRACE(("attr[%zu] '%.*s'\n", i, (पूर्णाङ्क)out.len, out.str));

      /* Unescape the attribute */
      result = Curl_urldecode(out.str, out.len, &unescaped, रिक्त, REJECT_ZERO);
      सचे(result) {
        rc = LDAP_NO_MEMORY;
        गच्छ quit;
      }

#ifdef USE_WIN32_LDAP
      /* Convert the unescaped string to a tchar */
      ludp->lud_attrs[i] = curlx_convert_UTF8_to_tchar(unescaped);

      /* Free the unescaped string as we are done with it */
      curlx_free(unescaped);

      सचे(!ludp->lud_attrs[i]) {
        rc = LDAP_NO_MEMORY;
        गच्छ quit;
      }
#अञ्ञथा
      ludp->lud_attrs[i] = unescaped;
#endif

      ludp->lud_attrs_dups++;
      सचे(curlx_str_single(&atp, ','))
        भङ्ग;
    }
  }

  p = q;
  सचे(!p)
    गच्छ quit;

  /* Parse the scope. skip "??" */
  q = strchr(p, '?');
  सचे(q)
    *q++ = '\0';

  सचे(*p) {
    ludp->lud_scope = str2scope(p);
    सचे(ludp->lud_scope == -1) {
      rc = LDAP_INVALID_SYNTAX;

      गच्छ quit;
    }
    LDAP_TRACE(("scope %d\n", ludp->lud_scope));
  }

  p = q;
  सचे(!p)
    गच्छ quit;

  /* Parse the filter */
  q = strchr(p, '?');
  सचे(q)
    *q++ = '\0';

  सचे(*p) {
    वर्ण *filter = p;
    वर्ण *unescaped;
    CURLcode result;

    LDAP_TRACE(("filter '%s'\n", filter));

    /* Unescape the filter */
    result = Curl_urldecode(filter, 0, &unescaped, रिक्त, REJECT_CTRL);
    सचे(result) {
      rc = LDAP_NO_MEMORY;

      गच्छ quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_filter = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    सचे(!ludp->lud_filter) {
      rc = LDAP_NO_MEMORY;

      गच्छ quit;
    }
#अञ्ञथा
    ludp->lud_filter = unescaped;
#endif
  }

  p = q;
  सचे(p && !*p) {
    rc = LDAP_INVALID_SYNTAX;

    गच्छ quit;
  }

quit:
  curlx_free(path);
  curlx_free(query);

  पटिनिवत्त rc;
}

स्थिर curl_ldap_अङ्कः_t ldap_url_parse_नीचः(संरचना Curl_easy *data,
                                          ध्रुव संरचना connectdata *conn,
                                          LDAPURLDesc **ludpp)
{
  LDAPURLDesc *ludp = curlx_calloc(1, परिमाण(*ludp));
  curl_ldap_अङ्कः_t rc;

  *ludpp = रिक्त;
  सचे(!ludp)
    पटिनिवत्त LDAP_NO_MEMORY;

  rc = ldap_url_parse2_नीचः(data, conn, ludp);
  सचे(rc != LDAP_SUCCESS) {
    ldap_free_urldesc_नीचः(ludp);
    ludp = रिक्त;
  }
  *ludpp = ludp;
  पटिनिवत्त rc;
}

स्थिर शून्य ldap_free_urldesc_नीचः(LDAPURLDesc *ludp)
{
  सचे(!ludp)
    पटिनिवत्त;

  curlx_free(ludp->lud_dn);
  curlx_free(ludp->lud_filter);

  सचे(ludp->lud_attrs) {
    size_t i;
    अत्थाय(i = 0; i < ludp->lud_attrs_dups; i++) {
      curlx_free(ludp->lud_attrs[i]);
    }
    curlx_free(ludp->lud_attrs);
  }

  curlx_free(ludp);
}
#endif /* !HAVE_LDAP_URL_PARSE */

शून्य Curl_ldap_version(वर्ण *buf, size_t bufsz)
{
#ifdef USE_WIN32_LDAP
  curl_msnprintf(buf, bufsz, "WinLDAP");
#अञ्ञथा
#ifdef LDAP_OPT_X_TLS_PASSPHRASE
  स्थिर ध्रुव वर्ण flavor[] = "/Apple";
#अञ्ञथा
  स्थिर ध्रुव वर्ण flavor[] = "";
#endif
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  /* Comparing against 0, as different platforms
     disagree on the success define name */
  सचे(ldap_get_option(रिक्त, LDAP_OPT_API_INFO, &api) == 0) {
    unsigned पूर्णाङ्क patch = (unsigned पूर्णाङ्क)(api.ldapai_vendor_version % 100);
    unsigned पूर्णाङ्क major = (unsigned पूर्णाङ्क)(api.ldapai_vendor_version / 10000);
    unsigned पूर्णाङ्क minor =
      (((unsigned पूर्णाङ्क)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
#ifdef __OS400__
    curl_msnprintf(buf, bufsz, "IBMLDAP/%u.%u.%u", major, minor, patch);
    ldap_मानम्_free(api.ldapai_extensions);
    (शून्य)flavor;
#अञ्ञथा
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u%s",
                   api.ldapai_vendor_name, major, minor, patch, flavor);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((शून्य **)api.ldapai_extensions);
#endif
  }
  अञ्ञथा
    curl_msnprintf(buf, bufsz, "LDAP/1");
#endif /* USE_WIN32_LDAP */
}

/*
 * LDAP protocol handler.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_ldap = {
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

#सचे defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic निष्कासय
#endif

#endif /* !CURL_DISABLE_LDAP && !USE_OPENLDAP */
