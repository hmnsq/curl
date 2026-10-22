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

#यदि !defined(CURL_DISABLE_LDAP) && !defined(USE_OPENLDAP)

#यदि defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#निर्देशः GCC diagnostic प्रक्षिप
#निर्देशः GCC diagnostic ignored "-Wdeprecated-declarations"
#यदि_अन्तः

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

#यदि_परिभाषितम् USE_WIN32_LDAP           /* Use Windows LDAP implementation. */
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
#  यदि defined(HAVE_LDAP_SSL) && defined(HAVE_LDAP_SSL_H)
#    include <ldap_ssl.h>
#  endif /* HAVE_LDAP_SSL && HAVE_LDAP_SSL_H */
#यदि_अन्तः

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

#यदि_परिभाषितम् USE_WIN32_LDAP
#परिभाषय FREE_ON_WINLDAP(x) curlx_free(x)
#परिभाषय curl_ldap_अङ्कः_t    ULONG
#अन्यथा
#परिभाषय FREE_ON_WINLDAP(x) कुरु {} यावत्(0)
#परिभाषय curl_ldap_अङ्कः_t    पूर्णाङ्क
#यदि_अन्तः

#यदि_अपरिभाषितम् HAVE_LDAP_URL_PARSE

/* Use our own implementation. */

संरचना ldap_urldesc {
  वर्ण   *lud_host;
  पूर्णाङ्क     lud_पत्तनम्;
#यदि_परिभाषितम् USE_WIN32_LDAP
  TCHAR  *lud_dn;
  TCHAR **lud_attrs;
#अन्यथा
  वर्ण   *lud_dn;
  वर्ण  **lud_attrs;
#यदि_अन्तः
  पूर्णाङ्क     lud_scope;
#यदि_परिभाषितम् USE_WIN32_LDAP
  TCHAR  *lud_filter;
#अन्यथा
  वर्ण   *lud_filter;
#यदि_अन्तः
  वर्ण  **lud_exts;
  size_t  lud_attrs_dups; /* how many were dup'ed, this field is not in the
                             "real" struct so can only be used in code
                             without HAVE_LDAP_URL_PARSE defined */
};

#अपरिभाषय LDAPURLDesc
#परिभाषय LDAPURLDesc संरचना ldap_urldesc

स्थिर curl_ldap_अङ्कः_t ldap_url_parse_नीचः(संरचना Curl_easy *दत्त,
                                          ध्रुव संरचना connectdata *conn,
                                          LDAPURLDesc **ludpp);
स्थिर शून्य ldap_free_urldesc_नीचः(LDAPURLDesc *ludp);

#अपरिभाषय ldap_free_urldesc
#परिभाषय ldap_free_urldesc       ldap_free_urldesc_नीचः

#यदि_अन्तः /* !HAVE_LDAP_URL_PARSE */

#यदि_परिभाषितम् DEBUG_LDAP
#परिभाषय LDAP_TRACE(x)                 \
  कुरु {                                \
    ldap_trace_नीचः("%u: ", __LINE__); \
    ldap_trace_नीचः x;                 \
  } यावत्(0)

स्थिर शून्य ldap_trace_नीचः(ध्रुव वर्ण *fmt, ...) CURL_PRINTF(1, 2);
#अन्यथा
#परिभाषय LDAP_TRACE(x) Curl_nop_stmt
#यदि_अन्तः

#यदि defined(USE_WIN32_LDAP) && defined(ldap_err2string)
/* Use ANSI error strings in Unicode builds */
#अपरिभाषय ldap_err2string
#परिभाषय ldap_err2string ldap_err2stringA
#यदि_अन्तः

#यदि defined(USE_WIN32_LDAP) && defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'void *' of greater size */
#अपरिभाषय LDAP_OPT_ON
#अपरिभाषय LDAP_OPT_OFF
#परिभाषय LDAP_OPT_ON   ((शून्य *)(size_t)1)
#परिभाषय LDAP_OPT_OFF  ((शून्य *)(size_t)0)
#यदि_अन्तः

#यदि_परिभाषितम् USE_WIN32_LDAP

#यदि_परिभाषितम् USE_WINDOWS_SSPI
स्थिर ULONG ldap_win_bind_auth(LDAP *server, ध्रुव वर्ण *user,
                                ध्रुव वर्ण *passwd, अचिह्नित दीर्घ authflags)
{
  ULONG विधानम् = 0;
  SEC_WINNT_AUTH_तटस्थः_EX cred;
  ULONG rc = LDAP_AUTH_विधानम्_NOT_SUPPORTED;

  स्मृतिं_पूरय(&cred, 0, परिमाण(cred));

#यदि_परिभाषितम् USE_SPNEGO
  यदि(authflags & CURLAUTH_NEGOTIATE) {
    विधानम् = LDAP_AUTH_NEGOTIATE;
  }
  अन्यथा
#यदि_अन्तः
#यदि_परिभाषितम् USE_NTLM
  यदि(authflags & CURLAUTH_NTLM) {
    विधानम् = LDAP_AUTH_NTLM;
  }
  अन्यथा
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH
  यदि(authflags & CURLAUTH_DIGEST) {
    विधानम् = LDAP_AUTH_DIGEST;
  }
  अन्यथा
#यदि_अन्तः
  {
    /* required anyway if one of upper preprocessor definitions enabled */
  }

  यदि(विधानम् && user && passwd) {
    CURLcode परिणाम = Curl_create_sspi_तटस्थः(user, passwd, &cred);
    यदि(!परिणाम) {
      rc = ldap_bind_s(server, रिक्त, (TCHAR *)&cred, विधानम्);
      Curl_sspi_free_तटस्थः(&cred);
    }
    अन्यथा {
      rc = LDAP_NO_स्मृतिः;
    }
  }
  अन्यथा {
    /* proceed with current user credentials */
    विधानम् = LDAP_AUTH_NEGOTIATE;
    rc = ldap_bind_s(server, रिक्त, रिक्त, विधानम्);
  }
  प्रत्यागम rc;
}
#यदि_अन्तः /* USE_WINDOWS_SSPI */

स्थिर ULONG ldap_win_bind(संरचना Curl_easy *दत्त, LDAP *server,
                           ध्रुव वर्ण *user, ध्रुव वर्ण *passwd)
{
  ULONG rc = LDAP_INVALID_CREDENTIALS;

  PTCHAR inuser = रिक्त;
  PTCHAR inpass = रिक्त;

  यदि(user && passwd && (दत्त->समुच्चयः.httpauth & CURLAUTH_BASIC)) {
    inuser = curlx_convert_UTF8_गन्तव्यम्_tchar(user);
    inpass = curlx_convert_UTF8_गन्तव्यम्_tchar(passwd);

    rc = ldap_simple_bind_s(server, inuser, inpass);

    curlx_free(inuser);
    curlx_free(inpass);
  }
#यदि_परिभाषितम् USE_WINDOWS_SSPI
  अन्यथा {
    rc = ldap_win_bind_auth(server, user, passwd, दत्त->समुच्चयः.httpauth);
  }
#यदि_अन्तः

  प्रत्यागम rc;
}
#यदि_अन्तः /* USE_WIN32_LDAP */

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
स्थिर bool ldap_str_needs_base64(ध्रुव वर्ण *s, size_t len)
{
  size_t i;
  यदि(!len)
    प्रत्यागम FALSE;
  यदि((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    प्रत्यागम TRUE;
  प्रति(i = 0; i < len; i++)
    यदि(!ISPRINT(s[i]))
      प्रत्यागम TRUE;
  प्रत्यागम FALSE;
}

स्थिर bool ldap_मानम्_needs_base64(ध्रुव वर्ण *attr, size_t attr_len,
                                    ध्रुव Ber_मानम् *val)
{
  यदि((attr_len > 7) && curl_strequal(";binary", attr + attr_len - 7))
    प्रत्यागम TRUE;

  प्रत्यागम ldap_str_needs_base64(val->bv_val, val->bv_len);
}

#यदि_परिभाषितम् USE_WIN32_LDAP
स्थिर BOOLEAN bypass_cert_verify(PLDAP Connection,
                                  PCCERT_CONTEXT *ppServerCert)
{
  (शून्य)Connection;
  CertFreeCertificateContext(*ppServerCert);
  /* approve any certificate since the verification is set to bypass */
  प्रत्यागम TRUE;
}
#यदि_अन्तः

स्थिर CURLcode show_vals(संरचना Curl_easy *दत्त, Ber_मानम् **vals,
                          ध्रुव वर्ण *attr)
{
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क i;
  size_t attr_len = वाक्यदैर्घ्यम्(attr);

  /* An attribute type has no base64 form in LDIF, so a type carrying control
     bytes (CR/LF etc.) would let a hostile server inject lines. A conformant
     type name never contains them, so reject it. */
  यदि(ldap_str_needs_base64(attr, attr_len)) {
    failf(दत्त, "LDAP local: attribute type contains illegal bytes");
    प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  }

  प्रति(i = 0; vals[i] && !परिणाम; i++) {
    परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, "\t", 1);
    यदि(!परिणाम)
      परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, attr, attr_len);
    यदि(!परिणाम)
      परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, ":", 1);

    यदि(परिणाम)
      भङ्ग;

    यदि(ldap_मानम्_needs_base64(attr, attr_len, vals[i])) {
      वर्ण *val_b64 = रिक्त;
      size_t val_b64_sz = 0;

      /* Binary attribute, encode to base64. */
      यदि(vals[i]->bv_len)
        परिणाम = curlx_base64_encode((uint8_t *)vals[i]->bv_val,
                                     vals[i]->bv_len,
                                     &val_b64, &val_b64_sz);
      यदि(!परिणाम)
        परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, ": ", 2);
      यदि(!परिणाम && val_b64_sz)
        परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, val_b64,
                                   val_b64_sz);
      curlx_free(val_b64);
    }
    अन्यथा {
      परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, " ", 1);
      यदि(!परिणाम)
        परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY,
                                   vals[i]->bv_val, vals[i]->bv_len);
    }

    यदि(!परिणाम)
      परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, "\n", 1);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ldap_do(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  curl_ldap_अङ्कः_t rc = LDAP_SUCCESS;
  LDAP *server = रिक्त;
  LDAPURLDesc *ludp = रिक्त;
  LDAPMessage *ldapmsg = रिक्त;
  LDAPMessage *प्रविष्टिः_Iterator;
  पूर्णाङ्क अङ्कः = 0;
  संरचना connectdata *conn = दत्त->conn;
  पूर्णाङ्क ldap_proto = LDAP_VERSION3;
  bool ldap_ssl = FALSE;
#यदि_परिभाषितम् LDAP_OPT_जालम्_TIMEOUT
  संरचना timeval ldap_timeout = { 10, 0 }; /* 10s connection/search timeout */
#यदि_अन्तः
#यदि_परिभाषितम् USE_WIN32_LDAP
  TCHAR *host = रिक्त;
#अन्यथा
  वर्ण *host = रिक्त;
#यदि_अन्तः
  ध्रुव वर्ण *user = दत्त->state.creds ? दत्त->state.creds->user : रिक्त;
  ध्रुव वर्ण *passwd = दत्त->state.creds ? दत्त->state.creds->passwd : रिक्त;
  संरचना ip_quadruple ipquad;
  bool is_ipv6;
  BerElement *ber = रिक्त;

  *समाप्तम् = TRUE; /* unconditionally */
  infof(दत्त, "LDAP local: LDAP Vendor = %s ; LDAP Version = %d",
        LDAP_VENDOR_NAME, LDAP_VENDOR_संस्करणम्);
  infof(दत्त, "LDAP local: %s", Curl_bufref_ptr(&दत्त->state.url));

#यदि_परिभाषितम् HAVE_LDAP_URL_PARSE
  rc = ldap_url_parse(Curl_bufref_ptr(&दत्त->state.url), &ludp);
#अन्यथा
  rc = ldap_url_parse_नीचः(दत्त, conn, &ludp);
#यदि_अन्तः
  यदि(rc) {
    failf(दत्त, "Bad LDAP URL: %s", ldap_err2string(rc));
    परिणाम = CURLE_URL_MALFORMAT;
    गच्छ quit;
  }

  परिणाम = Curl_conn_get_ip_info(दत्त, conn, FIRSTSOCKET, &is_ipv6, &ipquad);
  यदि(परिणाम)
    गच्छ quit;

  /* Get the URL scheme (either ldap or ldaps) */
  ldap_ssl = Curl_conn_is_ssl(conn, FIRSTSOCKET);
  infof(दत्त, "LDAP local: trying to establish %s connection",
        ldap_ssl ? "encrypted" : "cleartext");

#यदि_परिभाषितम् USE_WIN32_LDAP
  host = curlx_convert_UTF8_गन्तव्यम्_tchar(conn->origin->hostname);
  यदि(!host) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    गच्छ quit;
  }
#अन्यथा
  host = conn->origin->hostname;
#यदि_अन्तः

#यदि_परिभाषितम् USE_WIN32_LDAP
  यदि(ldap_ssl)
    server = ldap_sslinit(host, (curl_ldap_अङ्कः_t)ipquad.remote_पत्तनम्, 1);
  अन्यथा
#यदि_अन्तः
    server = ldap_init(host, (curl_ldap_अङ्कः_t)ipquad.remote_पत्तनम्);
  यदि(!server) {
    failf(दत्त, "LDAP: cannot setup connect to %s:%u",
          conn->origin->user_hostname, ipquad.remote_पत्तनम्);
    परिणाम = CURLE_COULDNT_CONNECT;
    गच्छ quit;
  }

#यदि_परिभाषितम् LDAP_OPT_जालम्_TIMEOUT
  ldap_समुच्चयः_option(server, LDAP_OPT_जालम्_TIMEOUT, &ldap_timeout);
#यदि_अन्तः
  ldap_समुच्चयः_option(server, LDAP_OPT_PROTOCOL_संस्करणम्, &ldap_proto);

  /* Do not chase referrals. */
  ldap_समुच्चयः_option(server, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  यदि(ldap_ssl) {
#यदि_परिभाषितम् HAVE_LDAP_SSL
#यदि_परिभाषितम् USE_WIN32_LDAP
    /* Win32 LDAP uses the Windows CA store to verify certificates */
    ldap_समुच्चयः_option(server, LDAP_OPT_SSL, LDAP_OPT_ON);
    यदि(!conn->ssl_config.verifypeer) {
      यदि(conn->ssl_config.verifyhost) {
        failf(दत्त, "LDAP local: host verification cannot be enabled when "
              "peer verification is disabled for Windows native LDAP");
        परिणाम = CURLE_NOT_BUILT_अन्तर्;
        गच्छ quit;
      }
      ldap_समुच्चयः_option(server, LDAP_OPT_SERVER_CERTIFICATE,
                      (शून्य *)(uintptr_t)bypass_cert_verify);
    }
#नोचेद्यदि defined(LDAP_OPT_X_TLS) /* !USE_WIN32_LDAP */
    पूर्णाङ्क ldap_option;
    ध्रुव वर्ण *ldap_ca = conn->ssl_config.CAfile;
    यदि(conn->ssl_config.verifypeer) {
      /* OpenLDAP SDK supports BASE64 files. */
      यदि(conn->ssl_config.cert_type &&
         !curl_strequal(conn->ssl_config.cert_type, "PEM")) {
        failf(दत्त, "LDAP local: ERROR OpenLDAP only supports PEM cert-type");
        परिणाम = CURLE_SSL_CERTPROBLEM;
        गच्छ quit;
      }
      यदि(!ldap_ca) {
        failf(दत्त, "LDAP local: ERROR PEM CA cert not set");
        परिणाम = CURLE_SSL_CERTPROBLEM;
        गच्छ quit;
      }
      infof(दत्त, "LDAP local: using PEM CA cert: %s", ldap_ca);
      rc = ldap_समुच्चयः_option(server, LDAP_OPT_X_TLS_CACERTFILE, ldap_ca);
      यदि(rc != LDAP_SUCCESS) {
        failf(दत्त, "LDAP local: ERROR setting PEM CA cert: %s",
              ldap_err2string(rc));
        परिणाम = CURLE_SSL_CERTPROBLEM;
        गच्छ quit;
      }
      ldap_option = LDAP_OPT_X_TLS_DEMAND;
    }
    अन्यथा
      ldap_option = LDAP_OPT_X_TLS_NEVER;

    rc = ldap_समुच्चयः_option(server, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_option);
    यदि(rc != LDAP_SUCCESS) {
      failf(दत्त, "LDAP local: ERROR setting cert verify mode: %s",
            ldap_err2string(rc));
      परिणाम = CURLE_SSL_CERTPROBLEM;
      गच्छ quit;
    }
    ldap_option = LDAP_OPT_X_TLS_HARD;
    rc = ldap_समुच्चयः_option(server, LDAP_OPT_X_TLS, &ldap_option);
    यदि(rc != LDAP_SUCCESS) {
      failf(दत्त, "LDAP local: ERROR setting SSL/TLS mode: %s",
            ldap_err2string(rc));
      परिणाम = CURLE_SSL_CERTPROBLEM;
      गच्छ quit;
    }
#अन्यथा /* !USE_WIN32_LDAP && !LDAP_OPT_X_TLS */
    /* we should probably never come up to here since configure
       should check in first place if we can support LDAP SSL/TLS */
    failf(दत्त, "LDAP local: SSL/TLS not supported with this version "
          "of the OpenLDAP toolkit");
    परिणाम = CURLE_SSL_CERTPROBLEM;
    गच्छ quit;
#यदि_अन्तः
#यदि_अन्तः /* HAVE_LDAP_SSL */
  }
  अन्यथा यदि(दत्त->समुच्चयः.use_ssl > CURLUSESSL_TRY) {
    failf(दत्त, "LDAP local: explicit TLS not supported");
    परिणाम = CURLE_NOT_BUILT_अन्तर्;
    गच्छ quit;
  }

#यदि_परिभाषितम् USE_WIN32_LDAP
  rc = ldap_win_bind(दत्त, server, user, passwd);
#अन्यथा
  rc = ldap_simple_bind_s(server, user, passwd);
#यदि_अन्तः
  यदि(!ldap_ssl && rc != LDAP_SUCCESS) {
    ldap_proto = LDAP_VERSION2;
    ldap_समुच्चयः_option(server, LDAP_OPT_PROTOCOL_संस्करणम्, &ldap_proto);
#यदि_परिभाषितम् USE_WIN32_LDAP
    rc = ldap_win_bind(दत्त, server, user, passwd);
#अन्यथा
    rc = ldap_simple_bind_s(server, user, passwd);
#यदि_अन्तः
  }
  यदि(rc != LDAP_SUCCESS) {
#यदि_परिभाषितम् USE_WIN32_LDAP
    failf(दत्त, "LDAP local: bind via ldap_win_bind %s",
          ldap_err2string(rc));
#अन्यथा
    failf(दत्त, "LDAP local: bind via ldap_simple_bind_s %s",
          ldap_err2string(rc));
#यदि_अन्तः
    परिणाम = CURLE_LDAP_CANNOT_BIND;
    गच्छ quit;
  }

  Curl_pgrsReset(दत्त);
  rc = ldap_अन्वेषणम्_s(server, ludp->lud_dn,
                     ludp->lud_scope,
                     ludp->lud_filter, ludp->lud_attrs, 0, &ldapmsg);

  यदि(rc != LDAP_SUCCESS && rc != LDAP_SIZELIMIT_EXCEEDED) {
    failf(दत्त, "LDAP remote: %s", ldap_err2string(rc));
    परिणाम = CURLE_LDAP_अन्वेषणम्_FAILED;
    गच्छ quit;
  }

  अङ्कः = 0;
  प्रति(प्रविष्टिः_Iterator = ldap_first_प्रविष्टिः(server, ldapmsg);
      प्रविष्टिः_Iterator;
      प्रविष्टिः_Iterator = ldap_अग्र_प्रविष्टिः(server, प्रविष्टिः_Iterator), अङ्कः++) {
#यदि_परिभाषितम् USE_WIN32_LDAP
    TCHAR *attribute;
#अन्यथा
    वर्ण *attribute;
#यदि_अन्तः

    /* Get the DN and write it to the client */
    {
      वर्ण *name = रिक्त;
      size_t name_len = 0;
#यदि_परिभाषितम् USE_WIN32_LDAP
      TCHAR *dn = ldap_get_dn(server, प्रविष्टिः_Iterator);
      यदि(dn)
        name = curlx_convert_tchar_गन्तव्यम्_UTF8(dn);
#अन्यथा
      वर्ण *dn = name = ldap_get_dn(server, प्रविष्टिः_Iterator);
#यदि_अन्तः
      यदि(!name)
        परिणाम = dn ? CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः : CURLE_FAILED_INIT;
      अन्यथा {
        name_len = वाक्यदैर्घ्यम्(name);
        यदि(ldap_str_needs_base64(name, name_len)) {
          वर्ण *dn_b64 = रिक्त;
          size_t dn_b64_sz = 0;

          /* A DN with e.g. CR/LF would inject LDIF lines, so encode it */
          परिणाम = curlx_base64_encode((uint8_t *)name, name_len,
                                       &dn_b64, &dn_b64_sz);
          यदि(!परिणाम)
            परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, "DN:: ", 5);
          यदि(!परिणाम)
            परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, dn_b64,
                                       dn_b64_sz);
          curlx_free(dn_b64);
        }
        अन्यथा {
          परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, "DN: ", 4);
          यदि(!परिणाम)
            परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, name, name_len);
        }
        यदि(!परिणाम)
          परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, "\n", 1);
      }
      FREE_ON_WINLDAP(name);
      ldap_memfree(dn);
      यदि(परिणाम)
        गच्छ quit;
    }

    /* Get the attributes and write them to the client */
    प्रति(attribute = ldap_first_attribute(server, प्रविष्टिः_Iterator, &ber);
        attribute;
        attribute = ldap_अग्र_attribute(server, प्रविष्टिः_Iterator, ber)) {
      Ber_मानम् **vals;
#यदि_परिभाषितम् USE_WIN32_LDAP
      वर्ण *attr = curlx_convert_tchar_गन्तव्यम्_UTF8(attribute);
      यदि(!attr) {
        ldap_memfree(attribute);
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ quit;
      }
#अन्यथा
      वर्ण *attr = attribute;
#यदि_अन्तः
      vals = ldap_get_मानानि_len(server, प्रविष्टिः_Iterator, attribute);
      यदि(vals) {
        परिणाम = show_vals(दत्त, vals, attr);
        /* Free memory used to store values */
        ldap_मानम्_free_len(vals);
      }

      /* Free the attribute as we are done with it */
      FREE_ON_WINLDAP(attr);
      ldap_memfree(attribute);

      यदि(!परिणाम)
        परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, "\n", 1);
      यदि(परिणाम)
        गच्छ quit;
    }

    यदि(ber) {
      ber_free(ber, 0);
      ber = रिक्त;
    }
  }

quit:
  यदि(ber)
    ber_free(ber, 0);
  यदि(ldapmsg) {
    ldap_msgfree(ldapmsg);
    LDAP_TRACE(("Received %d entries\n", अङ्कः));
  }
  यदि(rc == LDAP_SIZELIMIT_EXCEEDED)
    infof(दत्त, "There are more than %d entries", अङ्कः);
  यदि(ludp)
    ldap_free_urldesc(ludp);
  यदि(server)
    ldap_unbind_s(server);

  FREE_ON_WINLDAP(host);

  /* no data to transfer */
  Curl_xfer_सज्जता_nop(दत्त);
  connclose(conn);

  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् DEBUG_LDAP
स्थिर शून्य ldap_trace_नीचः(ध्रुव वर्ण *fmt, ...)
{
  स्थिर पूर्णाङ्क do_trace = -1;
  va_list तर्काः;

  यदि(do_trace == -1) {
    ध्रुव वर्ण *env = getenv("CURL_TRACE");
    curl_off_t e = 0;
    यदि(!curlx_str_सङ्ख्या(&env, &e, अधिकतम_पूर्णाङ्कः))
      do_trace = e > 0;
  }
  यदि(!do_trace)
    प्रत्यागम;

  va_start(तर्काः, fmt);
  curl_mvfprintf(stderr, fmt, तर्काः);
  va_end(तर्काः);
}
#यदि_अन्तः /* DEBUG_LDAP */

#यदि_अपरिभाषितम् HAVE_LDAP_URL_PARSE

/*
 * Return scope-value for a scope-string.
 */
स्थिर पूर्णाङ्क str2scope(ध्रुव वर्ण *p)
{
  यदि(curl_strequal(p, "one"))
    प्रत्यागम LDAP_SCOPE_ONELEVEL;
  यदि(curl_strequal(p, "onetree"))
    प्रत्यागम LDAP_SCOPE_ONELEVEL;
  यदि(curl_strequal(p, "base"))
    प्रत्यागम LDAP_SCOPE_आधारः;
  यदि(curl_strequal(p, "sub"))
    प्रत्यागम LDAP_SCOPE_SUBTREE;
  यदि(curl_strequal(p, "subtree"))
    प्रत्यागम LDAP_SCOPE_SUBTREE;
  प्रत्यागम -1;
}

/* number of entries in the attributes list */
स्थिर size_t अङ्कः_entries(ध्रुव वर्ण *s)
{
  size_t वस्तूनि = 1;

  s = strchr(s, ',');
  यावत्(s) {
    वस्तूनि++;
    s = strchr(s + 1, ',');
  }
  प्रत्यागम वस्तूनि;
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
स्थिर curl_ldap_अङ्कः_t ldap_url_parse2_नीचः(संरचना Curl_easy *दत्त,
                                           ध्रुव संरचना connectdata *conn,
                                           LDAPURLDesc *ludp)
{
  curl_ldap_अङ्कः_t rc = LDAP_SUCCESS;
  वर्ण *p;
  वर्ण *मार्गः;
  वर्ण *q = रिक्त;
  वर्ण *query = रिक्त;
  size_t i;

  यदि(!दत्त ||
     !दत्त->state.ऊर्ध्वम्.मार्गः ||
     दत्त->state.ऊर्ध्वम्.मार्गः[0] != '/' ||
     ((दत्त->state.origin->scheme != &Curl_scheme_ldap) &&
      (दत्त->state.origin->scheme != &Curl_scheme_ldaps)))
    प्रत्यागम LDAP_INVALID_SYNTAX;

  ludp->lud_scope = LDAP_SCOPE_आधारः;
  ludp->lud_पत्तनम्  = conn->origin->पत्तनम्;
  ludp->lud_host  = conn->origin->hostname;

  /* Duplicate the path */
  p = मार्गः = curlx_strdup(दत्त->state.ऊर्ध्वम्.मार्गः + 1);
  यदि(!मार्गः)
    प्रत्यागम LDAP_NO_स्मृतिः;

  /* Duplicate the query if present */
  यदि(दत्त->state.ऊर्ध्वम्.query) {
    q = query = curlx_strdup(दत्त->state.ऊर्ध्वम्.query);
    यदि(!query) {
      curlx_free(मार्गः);
      प्रत्यागम LDAP_NO_स्मृतिः;
    }
  }

  /* Parse the DN (Distinguished Name) */
  यदि(*p) {
    वर्ण *dn = p;
    वर्ण *unescaped;
    CURLcode परिणाम;

    LDAP_TRACE(("DN '%s'\n", dn));

    /* Unescape the DN */
    परिणाम = Curl_urldecode(dn, 0, &unescaped, रिक्त, REJECT_ZERO);
    यदि(परिणाम) {
      rc = LDAP_NO_स्मृतिः;

      गच्छ quit;
    }

#यदि_परिभाषितम् USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_dn = curlx_convert_UTF8_गन्तव्यम्_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    यदि(!ludp->lud_dn) {
      rc = LDAP_NO_स्मृतिः;

      गच्छ quit;
    }
#अन्यथा
    ludp->lud_dn = unescaped;
#यदि_अन्तः
  }

  p = q;
  यदि(!p)
    गच्छ quit;

  /* Parse the attributes. skip "??" */
  q = strchr(p, '?');
  यदि(q)
    *q++ = '\0';

  यदि(*p) {
    size_t गणनम् = अङ्कः_entries(p); /* at least one */
    ध्रुव वर्ण *atp = p;

    /* Allocate our array (+1 for the NULL entry) */
#यदि_परिभाषितम् USE_WIN32_LDAP
    ludp->lud_attrs = curlx_calloc(गणनम् + 1, परिमाण(TCHAR *));
#अन्यथा
    ludp->lud_attrs = curlx_calloc(गणनम् + 1, परिमाण(वर्ण *));
#यदि_अन्तः
    यदि(!ludp->lud_attrs) {
      rc = LDAP_NO_स्मृतिः;
      गच्छ quit;
    }

    प्रति(i = 0; i < गणनम्; i++) {
      वर्ण *unescaped;
      CURLcode परिणाम;
      संरचना Curl_str बहिर्गमनम्;

      यदि(curlx_str_until(&atp, &बहिर्गमनम्, 1024, ','))
        भङ्ग;

      LDAP_TRACE(("attr[%zu] '%.*s'\n", i, (पूर्णाङ्क)बहिर्गमनम्.len, बहिर्गमनम्.str));

      /* Unescape the attribute */
      परिणाम = Curl_urldecode(बहिर्गमनम्.str, बहिर्गमनम्.len, &unescaped, रिक्त, REJECT_ZERO);
      यदि(परिणाम) {
        rc = LDAP_NO_स्मृतिः;
        गच्छ quit;
      }

#यदि_परिभाषितम् USE_WIN32_LDAP
      /* Convert the unescaped string to a tchar */
      ludp->lud_attrs[i] = curlx_convert_UTF8_गन्तव्यम्_tchar(unescaped);

      /* Free the unescaped string as we are done with it */
      curlx_free(unescaped);

      यदि(!ludp->lud_attrs[i]) {
        rc = LDAP_NO_स्मृतिः;
        गच्छ quit;
      }
#अन्यथा
      ludp->lud_attrs[i] = unescaped;
#यदि_अन्तः

      ludp->lud_attrs_dups++;
      यदि(curlx_str_single(&atp, ','))
        भङ्ग;
    }
  }

  p = q;
  यदि(!p)
    गच्छ quit;

  /* Parse the scope. skip "??" */
  q = strchr(p, '?');
  यदि(q)
    *q++ = '\0';

  यदि(*p) {
    ludp->lud_scope = str2scope(p);
    यदि(ludp->lud_scope == -1) {
      rc = LDAP_INVALID_SYNTAX;

      गच्छ quit;
    }
    LDAP_TRACE(("scope %d\n", ludp->lud_scope));
  }

  p = q;
  यदि(!p)
    गच्छ quit;

  /* Parse the filter */
  q = strchr(p, '?');
  यदि(q)
    *q++ = '\0';

  यदि(*p) {
    वर्ण *filter = p;
    वर्ण *unescaped;
    CURLcode परिणाम;

    LDAP_TRACE(("filter '%s'\n", filter));

    /* Unescape the filter */
    परिणाम = Curl_urldecode(filter, 0, &unescaped, रिक्त, REJECT_CTRL);
    यदि(परिणाम) {
      rc = LDAP_NO_स्मृतिः;

      गच्छ quit;
    }

#यदि_परिभाषितम् USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_filter = curlx_convert_UTF8_गन्तव्यम्_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    यदि(!ludp->lud_filter) {
      rc = LDAP_NO_स्मृतिः;

      गच्छ quit;
    }
#अन्यथा
    ludp->lud_filter = unescaped;
#यदि_अन्तः
  }

  p = q;
  यदि(p && !*p) {
    rc = LDAP_INVALID_SYNTAX;

    गच्छ quit;
  }

quit:
  curlx_free(मार्गः);
  curlx_free(query);

  प्रत्यागम rc;
}

स्थिर curl_ldap_अङ्कः_t ldap_url_parse_नीचः(संरचना Curl_easy *दत्त,
                                          ध्रुव संरचना connectdata *conn,
                                          LDAPURLDesc **ludpp)
{
  LDAPURLDesc *ludp = curlx_calloc(1, परिमाण(*ludp));
  curl_ldap_अङ्कः_t rc;

  *ludpp = रिक्त;
  यदि(!ludp)
    प्रत्यागम LDAP_NO_स्मृतिः;

  rc = ldap_url_parse2_नीचः(दत्त, conn, ludp);
  यदि(rc != LDAP_SUCCESS) {
    ldap_free_urldesc_नीचः(ludp);
    ludp = रिक्त;
  }
  *ludpp = ludp;
  प्रत्यागम rc;
}

स्थिर शून्य ldap_free_urldesc_नीचः(LDAPURLDesc *ludp)
{
  यदि(!ludp)
    प्रत्यागम;

  curlx_free(ludp->lud_dn);
  curlx_free(ludp->lud_filter);

  यदि(ludp->lud_attrs) {
    size_t i;
    प्रति(i = 0; i < ludp->lud_attrs_dups; i++) {
      curlx_free(ludp->lud_attrs[i]);
    }
    curlx_free(ludp->lud_attrs);
  }

  curlx_free(ludp);
}
#यदि_अन्तः /* !HAVE_LDAP_URL_PARSE */

शून्य Curl_ldap_संस्करणम्(वर्ण *buf, size_t bufsz)
{
#यदि_परिभाषितम् USE_WIN32_LDAP
  curl_msnprintf(buf, bufsz, "WinLDAP");
#अन्यथा
#यदि_परिभाषितम् LDAP_OPT_X_TLS_PASSPHRASE
  स्थिर ध्रुव वर्ण flavor[] = "/Apple";
#अन्यथा
  स्थिर ध्रुव वर्ण flavor[] = "";
#यदि_अन्तः
  LDAPAPIInfo api;
  api.ldapai_info_संस्करणम् = LDAP_API_INFO_संस्करणम्;

  /* Comparing against 0, as different platforms
     disagree on the success define name */
  यदि(ldap_get_option(रिक्त, LDAP_OPT_API_INFO, &api) == 0) {
    अचिह्नित पूर्णाङ्क patch = (अचिह्नित पूर्णाङ्क)(api.ldapai_vendor_संस्करणम् % 100);
    अचिह्नित पूर्णाङ्क major = (अचिह्नित पूर्णाङ्क)(api.ldapai_vendor_संस्करणम् / 10000);
    अचिह्नित पूर्णाङ्क minor =
      (((अचिह्नित पूर्णाङ्क)api.ldapai_vendor_संस्करणम् - (major * 10000))
       - patch) / 100;
#यदि_परिभाषितम् __OS400__
    curl_msnprintf(buf, bufsz, "IBMLDAP/%u.%u.%u", major, minor, patch);
    ldap_मानम्_free(api.ldapai_extensions);
    (शून्य)flavor;
#अन्यथा
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u%s",
                   api.ldapai_vendor_name, major, minor, patch, flavor);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((शून्य **)api.ldapai_extensions);
#यदि_अन्तः
  }
  अन्यथा
    curl_msnprintf(buf, bufsz, "LDAP/1");
#यदि_अन्तः /* USE_WIN32_LDAP */
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

#यदि defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#निर्देशः GCC diagnostic निष्कासय
#यदि_अन्तः

#यदि_अन्तः /* !CURL_DISABLE_LDAP && !USE_OPENLDAP */
