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

#जर !defined(CURL_DISABLE_LDAP) && !defined(USE_OPENLDAP)

#जर defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ढकला
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
#अन्यथा
#  define LDAP_DEPRECATED 1     /* Be sure ldap_init() is defined. */
#  ifdef HAVE_LBER_H
#    include <lber.h>
#  endif
#  include <ldap.h>
#  जर defined(HAVE_LDAP_SSL) && defined(HAVE_LDAP_SSL_H)
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
#define curl_ldap_अंक_t    ULONG
#अन्यथा
#define FREE_ON_WINLDAP(x) करा {} पर्यंत(0)
#define curl_ldap_अंक_t    पूर्णांक
#endif

#ifndef HAVE_LDAP_URL_PARSE

/* Use our own implementation. */

संरचना ldap_urldesc {
  अक्षर   *lud_host;
  पूर्णांक     lud_port;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_dn;
  TCHAR **lud_attrs;
#अन्यथा
  अक्षर   *lud_dn;
  अक्षर  **lud_attrs;
#endif
  पूर्णांक     lud_scope;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_filter;
#अन्यथा
  अक्षर   *lud_filter;
#endif
  अक्षर  **lud_exts;
  माप_प्रकार  lud_attrs_dups; /* how many were dup'ed, this field is not in the
                             "real" struct so can only be used in code
                             without HAVE_LDAP_URL_PARSE defined */
};

#undef LDAPURLDesc
#define LDAPURLDesc संरचना ldap_urldesc

स्थिर curl_ldap_अंक_t ldap_url_parse_खालचा(संरचना Curl_easy *data,
                                          स्थिरांक संरचना connectdata *conn,
                                          LDAPURLDesc **ludpp);
स्थिर शून्य ldap_free_urldesc_खालचा(LDAPURLDesc *ludp);

#undef ldap_free_urldesc
#define ldap_free_urldesc       ldap_free_urldesc_खालचा

#endif /* !HAVE_LDAP_URL_PARSE */

#ifdef DEBUG_LDAP
#define LDAP_TRACE(x)                 \
  करा {                                \
    ldap_trace_खालचा("%u: ", __LINE__); \
    ldap_trace_खालचा x;                 \
  } पर्यंत(0)

स्थिर शून्य ldap_trace_खालचा(स्थिरांक अक्षर *fmt, ...) CURL_PRINTF(1, 2);
#अन्यथा
#define LDAP_TRACE(x) Curl_nop_stmt
#endif

#जर defined(USE_WIN32_LDAP) && defined(ldap_err2string)
/* Use ANSI error strings in Unicode builds */
#undef ldap_err2string
#define ldap_err2string ldap_err2stringA
#endif

#जर defined(USE_WIN32_LDAP) && defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'void *' of greater size */
#undef LDAP_OPT_ON
#undef LDAP_OPT_OFF
#define LDAP_OPT_ON   ((शून्य *)(माप_प्रकार)1)
#define LDAP_OPT_OFF  ((शून्य *)(माप_प्रकार)0)
#endif

#ifdef USE_WIN32_LDAP

#ifdef USE_WINDOWS_SSPI
स्थिर ULONG ldap_win_bind_auth(LDAP *server, स्थिरांक अक्षर *user,
                                स्थिरांक अक्षर *passwd, अचिन्हित दीर्घ authflags)
{
  ULONG method = 0;
  SEC_WINNT_AUTH_IDENTITY_EX cred;
  ULONG rc = LDAP_AUTH_METHOD_NOT_SUPPORTED;

  स_रखो(&cred, 0, आकार(cred));

#ifdef USE_SPNEGO
  जर(authflags & CURLAUTH_NEGOTIATE) {
    method = LDAP_AUTH_NEGOTIATE;
  }
  अन्यथा
#endif
#ifdef USE_NTLM
  जर(authflags & CURLAUTH_NTLM) {
    method = LDAP_AUTH_NTLM;
  }
  अन्यथा
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  जर(authflags & CURLAUTH_DIGEST) {
    method = LDAP_AUTH_DIGEST;
  }
  अन्यथा
#endif
  {
    /* required anyway if one of upper preprocessor definitions enabled */
  }

  जर(method && user && passwd) {
    CURLcode result = Curl_create_sspi_identity(user, passwd, &cred);
    जर(!result) {
      rc = ldap_bind_s(server, रिक्त, (TCHAR *)&cred, method);
      Curl_sspi_free_identity(&cred);
    }
    अन्यथा {
      rc = LDAP_NO_MEMORY;
    }
  }
  अन्यथा {
    /* proceed with current user credentials */
    method = LDAP_AUTH_NEGOTIATE;
    rc = ldap_bind_s(server, रिक्त, रिक्त, method);
  }
  परत rc;
}
#endif /* USE_WINDOWS_SSPI */

स्थिर ULONG ldap_win_bind(संरचना Curl_easy *data, LDAP *server,
                           स्थिरांक अक्षर *user, स्थिरांक अक्षर *passwd)
{
  ULONG rc = LDAP_INVALID_CREDENTIALS;

  PTCHAR inuser = रिक्त;
  PTCHAR inpass = रिक्त;

  जर(user && passwd && (data->set.httpauth & CURLAUTH_BASIC)) {
    inuser = curlx_convert_UTF8_to_tchar(user);
    inpass = curlx_convert_UTF8_to_tchar(passwd);

    rc = ldap_simple_bind_s(server, inuser, inpass);

    curlx_free(inuser);
    curlx_free(inpass);
  }
#ifdef USE_WINDOWS_SSPI
  अन्यथा {
    rc = ldap_win_bind_auth(server, user, passwd, data->set.httpauth);
  }
#endif

  परत rc;
}
#endif /* USE_WIN32_LDAP */

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
स्थिर bool ldap_str_needs_base64(स्थिरांक अक्षर *s, माप_प्रकार len)
{
  माप_प्रकार i;
  जर(!len)
    परत FALSE;
  जर((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    परत TRUE;
  साठी(i = 0; i < len; i++)
    जर(!ISPRINT(s[i]))
      परत TRUE;
  परत FALSE;
}

स्थिर bool ldap_मूल्य_needs_base64(स्थिरांक अक्षर *attr, माप_प्रकार attr_len,
                                    स्थिरांक Ber_मूल्य *val)
{
  जर((attr_len > 7) && curl_strequal(";binary", attr + attr_len - 7))
    परत TRUE;

  परत ldap_str_needs_base64(val->bv_val, val->bv_len);
}

#ifdef USE_WIN32_LDAP
स्थिर BOOLEAN bypass_cert_verify(PLDAP Connection,
                                  PCCERT_CONTEXT *ppServerCert)
{
  (शून्य)Connection;
  CertFreeCertificateContext(*ppServerCert);
  /* approve any certificate since the verification is set to bypass */
  परत TRUE;
}
#endif

स्थिर CURLcode show_vals(संरचना Curl_easy *data, Ber_मूल्य **vals,
                          स्थिरांक अक्षर *attr)
{
  CURLcode result = CURLE_OK;
  पूर्णांक i;
  माप_प्रकार attr_len = म_माप(attr);

  /* An attribute type has no base64 form in LDIF, so a type carrying control
     bytes (CR/LF etc.) would let a hostile server inject lines. A conformant
     type name never contains them, so reject it. */
  जर(ldap_str_needs_base64(attr, attr_len)) {
    failf(data, "LDAP local: attribute type contains illegal bytes");
    परत CURLE_WEIRD_SERVER_REPLY;
  }

  साठी(i = 0; vals[i] && !result; i++) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, "\t", 1);
    जर(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, attr, attr_len);
    जर(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, ":", 1);

    जर(result)
      थांबा;

    जर(ldap_मूल्य_needs_base64(attr, attr_len, vals[i])) {
      अक्षर *val_b64 = रिक्त;
      माप_प्रकार val_b64_sz = 0;

      /* Binary attribute, encode to base64. */
      जर(vals[i]->bv_len)
        result = curlx_base64_encode((uint8_t *)vals[i]->bv_val,
                                     vals[i]->bv_len,
                                     &val_b64, &val_b64_sz);
      जर(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, ": ", 2);
      जर(!result && val_b64_sz)
        result = Curl_client_write(data, CLIENTWRITE_BODY, val_b64,
                                   val_b64_sz);
      curlx_free(val_b64);
    }
    अन्यथा {
      result = Curl_client_write(data, CLIENTWRITE_BODY, " ", 1);
      जर(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   vals[i]->bv_val, vals[i]->bv_len);
    }

    जर(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
  }
  परत result;
}

स्थिर CURLcode ldap_do(संरचना Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  curl_ldap_अंक_t rc = LDAP_SUCCESS;
  LDAP *server = रिक्त;
  LDAPURLDesc *ludp = रिक्त;
  LDAPMessage *ldapmsg = रिक्त;
  LDAPMessage *entryIterator;
  पूर्णांक अंक = 0;
  संरचना connectdata *conn = data->conn;
  पूर्णांक ldap_proto = LDAP_VERSION3;
  bool ldap_ssl = FALSE;
#ifdef LDAP_OPT_NETWORK_TIMEOUT
  संरचना timeval ldap_timeout = { 10, 0 }; /* 10s connection/search timeout */
#endif
#ifdef USE_WIN32_LDAP
  TCHAR *host = रिक्त;
#अन्यथा
  अक्षर *host = रिक्त;
#endif
  स्थिरांक अक्षर *user = data->state.creds ? data->state.creds->user : रिक्त;
  स्थिरांक अक्षर *passwd = data->state.creds ? data->state.creds->passwd : रिक्त;
  संरचना ip_quadruple ipquad;
  bool is_ipv6;
  BerElement *ber = रिक्त;

  *done = TRUE; /* unconditionally */
  infof(data, "LDAP local: LDAP Vendor = %s ; LDAP Version = %d",
        LDAP_VENDOR_NAME, LDAP_VENDOR_VERSION);
  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

#ifdef HAVE_LDAP_URL_PARSE
  rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), &ludp);
#अन्यथा
  rc = ldap_url_parse_खालचा(data, conn, &ludp);
#endif
  जर(rc) {
    failf(data, "Bad LDAP URL: %s", ldap_err2string(rc));
    result = CURLE_URL_MALFORMAT;
    जा quit;
  }

  result = Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad);
  जर(result)
    जा quit;

  /* Get the URL scheme (either ldap or ldaps) */
  ldap_ssl = Curl_conn_is_ssl(conn, FIRSTSOCKET);
  infof(data, "LDAP local: trying to establish %s connection",
        ldap_ssl ? "encrypted" : "cleartext");

#ifdef USE_WIN32_LDAP
  host = curlx_convert_UTF8_to_tchar(conn->origin->hostname);
  जर(!host) {
    result = CURLE_OUT_OF_MEMORY;

    जा quit;
  }
#अन्यथा
  host = conn->origin->hostname;
#endif

#ifdef USE_WIN32_LDAP
  जर(ldap_ssl)
    server = ldap_sslinit(host, (curl_ldap_अंक_t)ipquad.remote_port, 1);
  अन्यथा
#endif
    server = ldap_init(host, (curl_ldap_अंक_t)ipquad.remote_port);
  जर(!server) {
    failf(data, "LDAP: cannot setup connect to %s:%u",
          conn->origin->user_hostname, ipquad.remote_port);
    result = CURLE_COULDNT_CONNECT;
    जा quit;
  }

#ifdef LDAP_OPT_NETWORK_TIMEOUT
  ldap_set_option(server, LDAP_OPT_NETWORK_TIMEOUT, &ldap_timeout);
#endif
  ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);

  /* Do not chase referrals. */
  ldap_set_option(server, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  जर(ldap_ssl) {
#ifdef HAVE_LDAP_SSL
#ifdef USE_WIN32_LDAP
    /* Win32 LDAP uses the Windows CA store to verify certificates */
    ldap_set_option(server, LDAP_OPT_SSL, LDAP_OPT_ON);
    जर(!conn->ssl_config.verifypeer) {
      जर(conn->ssl_config.verifyhost) {
        failf(data, "LDAP local: host verification cannot be enabled when "
              "peer verification is disabled for Windows native LDAP");
        result = CURLE_NOT_BUILT_IN;
        जा quit;
      }
      ldap_set_option(server, LDAP_OPT_SERVER_CERTIFICATE,
                      (शून्य *)(uintptr_t)bypass_cert_verify);
    }
#किंवाजर defined(LDAP_OPT_X_TLS) /* !USE_WIN32_LDAP */
    पूर्णांक ldap_option;
    स्थिरांक अक्षर *ldap_ca = conn->ssl_config.CAfile;
    जर(conn->ssl_config.verifypeer) {
      /* OpenLDAP SDK supports BASE64 files. */
      जर(conn->ssl_config.cert_type &&
         !curl_strequal(conn->ssl_config.cert_type, "PEM")) {
        failf(data, "LDAP local: ERROR OpenLDAP only supports PEM cert-type");
        result = CURLE_SSL_CERTPROBLEM;
        जा quit;
      }
      जर(!ldap_ca) {
        failf(data, "LDAP local: ERROR PEM CA cert not set");
        result = CURLE_SSL_CERTPROBLEM;
        जा quit;
      }
      infof(data, "LDAP local: using PEM CA cert: %s", ldap_ca);
      rc = ldap_set_option(server, LDAP_OPT_X_TLS_CACERTFILE, ldap_ca);
      जर(rc != LDAP_SUCCESS) {
        failf(data, "LDAP local: ERROR setting PEM CA cert: %s",
              ldap_err2string(rc));
        result = CURLE_SSL_CERTPROBLEM;
        जा quit;
      }
      ldap_option = LDAP_OPT_X_TLS_DEMAND;
    }
    अन्यथा
      ldap_option = LDAP_OPT_X_TLS_NEVER;

    rc = ldap_set_option(server, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_option);
    जर(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting cert verify mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      जा quit;
    }
    ldap_option = LDAP_OPT_X_TLS_HARD;
    rc = ldap_set_option(server, LDAP_OPT_X_TLS, &ldap_option);
    जर(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting SSL/TLS mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      जा quit;
    }
#अन्यथा /* !USE_WIN32_LDAP && !LDAP_OPT_X_TLS */
    /* we should probably never come up to here since configure
       should check in first place if we can support LDAP SSL/TLS */
    failf(data, "LDAP local: SSL/TLS not supported with this version "
          "of the OpenLDAP toolkit");
    result = CURLE_SSL_CERTPROBLEM;
    जा quit;
#endif
#endif /* HAVE_LDAP_SSL */
  }
  अन्यथा जर(data->set.use_ssl > CURLUSESSL_TRY) {
    failf(data, "LDAP local: explicit TLS not supported");
    result = CURLE_NOT_BUILT_IN;
    जा quit;
  }

#ifdef USE_WIN32_LDAP
  rc = ldap_win_bind(data, server, user, passwd);
#अन्यथा
  rc = ldap_simple_bind_s(server, user, passwd);
#endif
  जर(!ldap_ssl && rc != LDAP_SUCCESS) {
    ldap_proto = LDAP_VERSION2;
    ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);
#ifdef USE_WIN32_LDAP
    rc = ldap_win_bind(data, server, user, passwd);
#अन्यथा
    rc = ldap_simple_bind_s(server, user, passwd);
#endif
  }
  जर(rc != LDAP_SUCCESS) {
#ifdef USE_WIN32_LDAP
    failf(data, "LDAP local: bind via ldap_win_bind %s",
          ldap_err2string(rc));
#अन्यथा
    failf(data, "LDAP local: bind via ldap_simple_bind_s %s",
          ldap_err2string(rc));
#endif
    result = CURLE_LDAP_CANNOT_BIND;
    जा quit;
  }

  Curl_pgrsReset(data);
  rc = ldap_शोध_s(server, ludp->lud_dn,
                     ludp->lud_scope,
                     ludp->lud_filter, ludp->lud_attrs, 0, &ldapmsg);

  जर(rc != LDAP_SUCCESS && rc != LDAP_SIZELIMIT_EXCEEDED) {
    failf(data, "LDAP remote: %s", ldap_err2string(rc));
    result = CURLE_LDAP_शोध_FAILED;
    जा quit;
  }

  अंक = 0;
  साठी(entryIterator = ldap_first_entry(server, ldapmsg);
      entryIterator;
      entryIterator = ldap_next_entry(server, entryIterator), अंक++) {
#ifdef USE_WIN32_LDAP
    TCHAR *attribute;
#अन्यथा
    अक्षर *attribute;
#endif

    /* Get the DN and write it to the client */
    {
      अक्षर *name = रिक्त;
      माप_प्रकार name_len = 0;
#ifdef USE_WIN32_LDAP
      TCHAR *dn = ldap_get_dn(server, entryIterator);
      जर(dn)
        name = curlx_convert_tchar_to_UTF8(dn);
#अन्यथा
      अक्षर *dn = name = ldap_get_dn(server, entryIterator);
#endif
      जर(!name)
        result = dn ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
      अन्यथा {
        name_len = म_माप(name);
        जर(ldap_str_needs_base64(name, name_len)) {
          अक्षर *dn_b64 = रिक्त;
          माप_प्रकार dn_b64_sz = 0;

          /* A DN with e.g. CR/LF would inject LDIF lines, so encode it */
          result = curlx_base64_encode((uint8_t *)name, name_len,
                                       &dn_b64, &dn_b64_sz);
          जर(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, "DN:: ", 5);
          जर(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, dn_b64,
                                       dn_b64_sz);
          curlx_free(dn_b64);
        }
        अन्यथा {
          result = Curl_client_write(data, CLIENTWRITE_BODY, "DN: ", 4);
          जर(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, name, name_len);
        }
        जर(!result)
          result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      }
      FREE_ON_WINLDAP(name);
      ldap_memfree(dn);
      जर(result)
        जा quit;
    }

    /* Get the attributes and write them to the client */
    साठी(attribute = ldap_first_attribute(server, entryIterator, &ber);
        attribute;
        attribute = ldap_next_attribute(server, entryIterator, ber)) {
      Ber_मूल्य **vals;
#ifdef USE_WIN32_LDAP
      अक्षर *attr = curlx_convert_tchar_to_UTF8(attribute);
      जर(!attr) {
        ldap_memfree(attribute);
        result = CURLE_OUT_OF_MEMORY;
        जा quit;
      }
#अन्यथा
      अक्षर *attr = attribute;
#endif
      vals = ldap_get_मूल्ये_len(server, entryIterator, attribute);
      जर(vals) {
        result = show_vals(data, vals, attr);
        /* Free memory used to store values */
        ldap_मूल्य_free_len(vals);
      }

      /* Free the attribute as we are done with it */
      FREE_ON_WINLDAP(attr);
      ldap_memfree(attribute);

      जर(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      जर(result)
        जा quit;
    }

    जर(ber) {
      ber_free(ber, 0);
      ber = रिक्त;
    }
  }

quit:
  जर(ber)
    ber_free(ber, 0);
  जर(ldapmsg) {
    ldap_msgfree(ldapmsg);
    LDAP_TRACE(("Received %d entries\n", अंक));
  }
  जर(rc == LDAP_SIZELIMIT_EXCEEDED)
    infof(data, "There are more than %d entries", अंक);
  जर(ludp)
    ldap_free_urldesc(ludp);
  जर(server)
    ldap_unbind_s(server);

  FREE_ON_WINLDAP(host);

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  connclose(conn);

  परत result;
}

#ifdef DEBUG_LDAP
स्थिर शून्य ldap_trace_खालचा(स्थिरांक अक्षर *fmt, ...)
{
  स्थिर पूर्णांक do_trace = -1;
  बहु_सूची args;

  जर(do_trace == -1) {
    स्थिरांक अक्षर *env = दो_पर्या("CURL_TRACE");
    curl_off_t e = 0;
    जर(!curlx_str_संख्या(&env, &e, पूर्णांक_उच्च))
      do_trace = e > 0;
  }
  जर(!do_trace)
    परत;

  बहु_शुरू(args, fmt);
  curl_mvfprintf(मानक_त्रुटि, fmt, args);
  बहु_पूर्ण(args);
}
#endif /* DEBUG_LDAP */

#ifndef HAVE_LDAP_URL_PARSE

/*
 * Return scope-value for a scope-string.
 */
स्थिर पूर्णांक str2scope(स्थिरांक अक्षर *p)
{
  जर(curl_strequal(p, "one"))
    परत LDAP_SCOPE_ONELEVEL;
  जर(curl_strequal(p, "onetree"))
    परत LDAP_SCOPE_ONELEVEL;
  जर(curl_strequal(p, "base"))
    परत LDAP_SCOPE_BASE;
  जर(curl_strequal(p, "sub"))
    परत LDAP_SCOPE_SUBTREE;
  जर(curl_strequal(p, "subtree"))
    परत LDAP_SCOPE_SUBTREE;
  परत -1;
}

/* number of entries in the attributes list */
स्थिर माप_प्रकार अंक_entries(स्थिरांक अक्षर *s)
{
  माप_प्रकार वस्तुसंच = 1;

  s = म_अक्षर(s, ',');
  पर्यंत(s) {
    वस्तुसंच++;
    s = म_अक्षर(s + 1, ',');
  }
  परत वस्तुसंच;
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
स्थिर curl_ldap_अंक_t ldap_url_parse2_खालचा(संरचना Curl_easy *data,
                                           स्थिरांक संरचना connectdata *conn,
                                           LDAPURLDesc *ludp)
{
  curl_ldap_अंक_t rc = LDAP_SUCCESS;
  अक्षर *p;
  अक्षर *path;
  अक्षर *q = रिक्त;
  अक्षर *query = रिक्त;
  माप_प्रकार i;

  जर(!data ||
     !data->state.up.path ||
     data->state.up.path[0] != '/' ||
     ((data->state.origin->scheme != &Curl_scheme_ldap) &&
      (data->state.origin->scheme != &Curl_scheme_ldaps)))
    परत LDAP_INVALID_SYNTAX;

  ludp->lud_scope = LDAP_SCOPE_BASE;
  ludp->lud_port  = conn->origin->port;
  ludp->lud_host  = conn->origin->hostname;

  /* Duplicate the path */
  p = path = curlx_strdup(data->state.up.path + 1);
  जर(!path)
    परत LDAP_NO_MEMORY;

  /* Duplicate the query if present */
  जर(data->state.up.query) {
    q = query = curlx_strdup(data->state.up.query);
    जर(!query) {
      curlx_free(path);
      परत LDAP_NO_MEMORY;
    }
  }

  /* Parse the DN (Distinguished Name) */
  जर(*p) {
    अक्षर *dn = p;
    अक्षर *unescaped;
    CURLcode result;

    LDAP_TRACE(("DN '%s'\n", dn));

    /* Unescape the DN */
    result = Curl_urldecode(dn, 0, &unescaped, रिक्त, REJECT_ZERO);
    जर(result) {
      rc = LDAP_NO_MEMORY;

      जा quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_dn = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    जर(!ludp->lud_dn) {
      rc = LDAP_NO_MEMORY;

      जा quit;
    }
#अन्यथा
    ludp->lud_dn = unescaped;
#endif
  }

  p = q;
  जर(!p)
    जा quit;

  /* Parse the attributes. skip "??" */
  q = म_अक्षर(p, '?');
  जर(q)
    *q++ = '\0';

  जर(*p) {
    माप_प्रकार मोजणी = अंक_entries(p); /* at least one */
    स्थिरांक अक्षर *atp = p;

    /* Allocate our array (+1 for the NULL entry) */
#ifdef USE_WIN32_LDAP
    ludp->lud_attrs = curlx_calloc(मोजणी + 1, आकार(TCHAR *));
#अन्यथा
    ludp->lud_attrs = curlx_calloc(मोजणी + 1, आकार(अक्षर *));
#endif
    जर(!ludp->lud_attrs) {
      rc = LDAP_NO_MEMORY;
      जा quit;
    }

    साठी(i = 0; i < मोजणी; i++) {
      अक्षर *unescaped;
      CURLcode result;
      संरचना Curl_str out;

      जर(curlx_str_until(&atp, &out, 1024, ','))
        थांबा;

      LDAP_TRACE(("attr[%zu] '%.*s'\n", i, (पूर्णांक)out.len, out.str));

      /* Unescape the attribute */
      result = Curl_urldecode(out.str, out.len, &unescaped, रिक्त, REJECT_ZERO);
      जर(result) {
        rc = LDAP_NO_MEMORY;
        जा quit;
      }

#ifdef USE_WIN32_LDAP
      /* Convert the unescaped string to a tchar */
      ludp->lud_attrs[i] = curlx_convert_UTF8_to_tchar(unescaped);

      /* Free the unescaped string as we are done with it */
      curlx_free(unescaped);

      जर(!ludp->lud_attrs[i]) {
        rc = LDAP_NO_MEMORY;
        जा quit;
      }
#अन्यथा
      ludp->lud_attrs[i] = unescaped;
#endif

      ludp->lud_attrs_dups++;
      जर(curlx_str_single(&atp, ','))
        थांबा;
    }
  }

  p = q;
  जर(!p)
    जा quit;

  /* Parse the scope. skip "??" */
  q = म_अक्षर(p, '?');
  जर(q)
    *q++ = '\0';

  जर(*p) {
    ludp->lud_scope = str2scope(p);
    जर(ludp->lud_scope == -1) {
      rc = LDAP_INVALID_SYNTAX;

      जा quit;
    }
    LDAP_TRACE(("scope %d\n", ludp->lud_scope));
  }

  p = q;
  जर(!p)
    जा quit;

  /* Parse the filter */
  q = म_अक्षर(p, '?');
  जर(q)
    *q++ = '\0';

  जर(*p) {
    अक्षर *filter = p;
    अक्षर *unescaped;
    CURLcode result;

    LDAP_TRACE(("filter '%s'\n", filter));

    /* Unescape the filter */
    result = Curl_urldecode(filter, 0, &unescaped, रिक्त, REJECT_CTRL);
    जर(result) {
      rc = LDAP_NO_MEMORY;

      जा quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_filter = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    जर(!ludp->lud_filter) {
      rc = LDAP_NO_MEMORY;

      जा quit;
    }
#अन्यथा
    ludp->lud_filter = unescaped;
#endif
  }

  p = q;
  जर(p && !*p) {
    rc = LDAP_INVALID_SYNTAX;

    जा quit;
  }

quit:
  curlx_free(path);
  curlx_free(query);

  परत rc;
}

स्थिर curl_ldap_अंक_t ldap_url_parse_खालचा(संरचना Curl_easy *data,
                                          स्थिरांक संरचना connectdata *conn,
                                          LDAPURLDesc **ludpp)
{
  LDAPURLDesc *ludp = curlx_calloc(1, आकार(*ludp));
  curl_ldap_अंक_t rc;

  *ludpp = रिक्त;
  जर(!ludp)
    परत LDAP_NO_MEMORY;

  rc = ldap_url_parse2_खालचा(data, conn, ludp);
  जर(rc != LDAP_SUCCESS) {
    ldap_free_urldesc_खालचा(ludp);
    ludp = रिक्त;
  }
  *ludpp = ludp;
  परत rc;
}

स्थिर शून्य ldap_free_urldesc_खालचा(LDAPURLDesc *ludp)
{
  जर(!ludp)
    परत;

  curlx_free(ludp->lud_dn);
  curlx_free(ludp->lud_filter);

  जर(ludp->lud_attrs) {
    माप_प्रकार i;
    साठी(i = 0; i < ludp->lud_attrs_dups; i++) {
      curlx_free(ludp->lud_attrs[i]);
    }
    curlx_free(ludp->lud_attrs);
  }

  curlx_free(ludp);
}
#endif /* !HAVE_LDAP_URL_PARSE */

शून्य Curl_ldap_version(अक्षर *buf, माप_प्रकार bufsz)
{
#ifdef USE_WIN32_LDAP
  curl_msnprintf(buf, bufsz, "WinLDAP");
#अन्यथा
#ifdef LDAP_OPT_X_TLS_PASSPHRASE
  स्थिर स्थिरांक अक्षर flavor[] = "/Apple";
#अन्यथा
  स्थिर स्थिरांक अक्षर flavor[] = "";
#endif
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  /* Comparing against 0, as different platforms
     disagree on the success define name */
  जर(ldap_get_option(रिक्त, LDAP_OPT_API_INFO, &api) == 0) {
    अचिन्हित पूर्णांक patch = (अचिन्हित पूर्णांक)(api.ldapai_vendor_version % 100);
    अचिन्हित पूर्णांक major = (अचिन्हित पूर्णांक)(api.ldapai_vendor_version / 10000);
    अचिन्हित पूर्णांक minor =
      (((अचिन्हित पूर्णांक)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
#ifdef __OS400__
    curl_msnprintf(buf, bufsz, "IBMLDAP/%u.%u.%u", major, minor, patch);
    ldap_मूल्य_free(api.ldapai_extensions);
    (शून्य)flavor;
#अन्यथा
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u%s",
                   api.ldapai_vendor_name, major, minor, patch, flavor);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((शून्य **)api.ldapai_extensions);
#endif
  }
  अन्यथा
    curl_msnprintf(buf, bufsz, "LDAP/1");
#endif /* USE_WIN32_LDAP */
}

/*
 * LDAP protocol handler.
 */
स्थिरांक संरचना Curl_protocol Curl_protocol_ldap = {
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

#जर defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic काढा
#endif

#endif /* !CURL_DISABLE_LDAP && !USE_OPENLDAP */
