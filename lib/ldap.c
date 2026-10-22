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
#అన్తర్భావ "curl_setup.h"
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curl_ldap.h"

#ఒకవేళ !defined(CURL_DISABLE_LDAP) && !defined(USE_OPENLDAP)

#ఒకవేళ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Wdeprecated-declarations"
#అయితే_ముగింపు

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

#నిర్వచితమైతే USE_WIN32_LDAP           /* Use Windows LDAP implementation. */
#  include <winldap.h>
/* Undefine indirect <wincrypt.h> symbols conflicting with AWS-LC/BoringSSL. */
#  undef X509_NAME
#  undef X509_EXTENSIONS
#  undef PKCS7_ISSUER_AND_SERIAL
#  undef PKCS7_SIGNER_INFO
#  undef OCSP_REQUEST
#  undef OCSP_RESPONSE
#  include <winber.h>
#లేకపోతే
#  define LDAP_DEPRECATED 1     /* Be sure ldap_init() is defined. */
#  ifdef HAVE_LBER_H
#    include <lber.h>
#  endif
#  include <ldap.h>
#  ఒకవేళ defined(HAVE_LDAP_SSL) && defined(HAVE_LDAP_SSL_H)
#    include <ldap_ssl.h>
#  endif /* HAVE_LDAP_SSL && HAVE_LDAP_SSL_H */
#అయితే_ముగింపు

#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "bufref.h"
#అన్తర్భావ "curlx/multibyte.h"
#అన్తర్భావ "curlx/base64.h"
#అన్తర్భావ "connect.h"

#నిర్వచితమైతే USE_WIN32_LDAP
#నిర్వచించు FREE_ON_WINLDAP(x) curlx_free(x)
#నిర్వచించు curl_ldap_అంకె_t    ULONG
#లేకపోతే
#నిర్వచించు FREE_ON_WINLDAP(x) చేయి {} అయితే(0)
#నిర్వచించు curl_ldap_అంకె_t    పూర్ణాంకం
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే HAVE_LDAP_URL_PARSE

/* Use our own implementation. */

నిర్మాణం ldap_urldesc {
  అక్షరం   *lud_host;
  పూర్ణాంకం     lud_పోర్టు;
#నిర్వచితమైతే USE_WIN32_LDAP
  TCHAR  *lud_dn;
  TCHAR **lud_attrs;
#లేకపోతే
  అక్షరం   *lud_dn;
  అక్షరం  **lud_attrs;
#అయితే_ముగింపు
  పూర్ణాంకం     lud_scope;
#నిర్వచితమైతే USE_WIN32_LDAP
  TCHAR  *lud_filter;
#లేకపోతే
  అక్షరం   *lud_filter;
#అయితే_ముగింపు
  అక్షరం  **lud_exts;
  size_t  lud_attrs_dups; /* how many were dup'ed, this field is not in the
                             "real" struct so can only be used in code
                             without HAVE_LDAP_URL_PARSE defined */
};

#నిర్వచనం_రద్దు LDAPURLDesc
#నిర్వచించు LDAPURLDesc నిర్మాణం ldap_urldesc

స్థిర curl_ldap_అంకె_t ldap_url_parse_దిగువ(నిర్మాణం Curl_easy *దత్తాంశం,
                                          స్థిరరాశి నిర్మాణం connectdata *conn,
                                          LDAPURLDesc **ludpp);
స్థిర శూన్య ldap_free_urldesc_దిగువ(LDAPURLDesc *ludp);

#నిర్వచనం_రద్దు ldap_free_urldesc
#నిర్వచించు ldap_free_urldesc       ldap_free_urldesc_దిగువ

#అయితే_ముగింపు /* !HAVE_LDAP_URL_PARSE */

#నిర్వచితమైతే DEBUG_LDAP
#నిర్వచించు LDAP_TRACE(x)                 \
  చేయి {                                \
    ldap_trace_దిగువ("%u: ", __LINE__); \
    ldap_trace_దిగువ x;                 \
  } అయితే(0)

స్థిర శూన్య ldap_trace_దిగువ(స్థిరరాశి అక్షరం *fmt, ...) CURL_PRINTF(1, 2);
#లేకపోతే
#నిర్వచించు LDAP_TRACE(x) Curl_nop_stmt
#అయితే_ముగింపు

#ఒకవేళ defined(USE_WIN32_LDAP) && defined(ldap_err2string)
/* Use ANSI error strings in Unicode builds */
#నిర్వచనం_రద్దు ldap_err2string
#నిర్వచించు ldap_err2string ldap_err2stringA
#అయితే_ముగింపు

#ఒకవేళ defined(USE_WIN32_LDAP) && defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'void *' of greater size */
#నిర్వచనం_రద్దు LDAP_OPT_ON
#నిర్వచనం_రద్దు LDAP_OPT_OFF
#నిర్వచించు LDAP_OPT_ON   ((శూన్య *)(size_t)1)
#నిర్వచించు LDAP_OPT_OFF  ((శూన్య *)(size_t)0)
#అయితే_ముగింపు

#నిర్వచితమైతే USE_WIN32_LDAP

#నిర్వచితమైతే USE_WINDOWS_SSPI
స్థిర ULONG ldap_win_bind_auth(LDAP *server, స్థిరరాశి అక్షరం *user,
                                స్థిరరాశి అక్షరం *passwd, గుర్తులేని దీర్ఘ authflags)
{
  ULONG పద్ధతి = 0;
  SEC_WINNT_AUTH_తటస్థం_EX cred;
  ULONG rc = LDAP_AUTH_పద్ధతి_NOT_SUPPORTED;

  memset(&cred, 0, పరిమాణం(cred));

#నిర్వచితమైతే USE_SPNEGO
  ఒకవేళ(authflags & CURLAUTH_NEGOTIATE) {
    పద్ధతి = LDAP_AUTH_NEGOTIATE;
  }
  లేకపోతే
#అయితే_ముగింపు
#నిర్వచితమైతే USE_NTLM
  ఒకవేళ(authflags & CURLAUTH_NTLM) {
    పద్ధతి = LDAP_AUTH_NTLM;
  }
  లేకపోతే
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH
  ఒకవేళ(authflags & CURLAUTH_DIGEST) {
    పద్ధతి = LDAP_AUTH_DIGEST;
  }
  లేకపోతే
#అయితే_ముగింపు
  {
    /* required anyway if one of upper preprocessor definitions enabled */
  }

  ఒకవేళ(పద్ధతి && user && passwd) {
    CURLcode ఫలితం = Curl_create_sspi_తటస్థం(user, passwd, &cred);
    ఒకవేళ(!ఫలితం) {
      rc = ldap_bind_s(server, ఖాళీ, (TCHAR *)&cred, పద్ధతి);
      Curl_sspi_free_తటస్థం(&cred);
    }
    లేకపోతే {
      rc = LDAP_NO_జ్ఞాపకం;
    }
  }
  లేకపోతే {
    /* proceed with current user credentials */
    పద్ధతి = LDAP_AUTH_NEGOTIATE;
    rc = ldap_bind_s(server, ఖాళీ, ఖాళీ, పద్ధతి);
  }
  తిరిగి rc;
}
#అయితే_ముగింపు /* USE_WINDOWS_SSPI */

స్థిర ULONG ldap_win_bind(నిర్మాణం Curl_easy *దత్తాంశం, LDAP *server,
                           స్థిరరాశి అక్షరం *user, స్థిరరాశి అక్షరం *passwd)
{
  ULONG rc = LDAP_INVALID_CREDENTIALS;

  PTCHAR inuser = ఖాళీ;
  PTCHAR inpass = ఖాళీ;

  ఒకవేళ(user && passwd && (దత్తాంశం->సమితి.httpauth & CURLAUTH_BASIC)) {
    inuser = curlx_convert_UTF8_గమ్యం_tchar(user);
    inpass = curlx_convert_UTF8_గమ్యం_tchar(passwd);

    rc = ldap_simple_bind_s(server, inuser, inpass);

    curlx_free(inuser);
    curlx_free(inpass);
  }
#నిర్వచితమైతే USE_WINDOWS_SSPI
  లేకపోతే {
    rc = ldap_win_bind_auth(server, user, passwd, దత్తాంశం->సమితి.httpauth);
  }
#అయితే_ముగింపు

  తిరిగి rc;
}
#అయితే_ముగింపు /* USE_WIN32_LDAP */

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
స్థిర bool ldap_str_needs_base64(స్థిరరాశి అక్షరం *s, size_t len)
{
  size_t i;
  ఒకవేళ(!len)
    తిరిగి FALSE;
  ఒకవేళ((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    తిరిగి TRUE;
  కొరకు(i = 0; i < len; i++)
    ఒకవేళ(!ISPRINT(s[i]))
      తిరిగి TRUE;
  తిరిగి FALSE;
}

స్థిర bool ldap_విలువ_needs_base64(స్థిరరాశి అక్షరం *attr, size_t attr_len,
                                    స్థిరరాశి Ber_విలువ *val)
{
  ఒకవేళ((attr_len > 7) && curl_strequal(";binary", attr + attr_len - 7))
    తిరిగి TRUE;

  తిరిగి ldap_str_needs_base64(val->bv_val, val->bv_len);
}

#నిర్వచితమైతే USE_WIN32_LDAP
స్థిర BOOLEAN bypass_cert_verify(PLDAP Connection,
                                  PCCERT_CONTEXT *ppServerCert)
{
  (శూన్య)Connection;
  CertFreeCertificateContext(*ppServerCert);
  /* approve any certificate since the verification is set to bypass */
  తిరిగి TRUE;
}
#అయితే_ముగింపు

స్థిర CURLcode show_vals(నిర్మాణం Curl_easy *దత్తాంశం, Ber_విలువ **vals,
                          స్థిరరాశి అక్షరం *attr)
{
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం i;
  size_t attr_len = అక్షరశ్రేణి_పొడవు(attr);

  /* An attribute type has no base64 form in LDIF, so a type carrying control
     bytes (CR/LF etc.) would let a hostile server inject lines. A conformant
     type name never contains them, so reject it. */
  ఒకవేళ(ldap_str_needs_base64(attr, attr_len)) {
    failf(దత్తాంశం, "LDAP local: attribute type contains illegal bytes");
    తిరిగి CURLE_WEIRD_SERVER_జవాబు;
  }

  కొరకు(i = 0; vals[i] && !ఫలితం; i++) {
    ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, "\t", 1);
    ఒకవేళ(!ఫలితం)
      ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, attr, attr_len);
    ఒకవేళ(!ఫలితం)
      ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, ":", 1);

    ఒకవేళ(ఫలితం)
      ఆపు;

    ఒకవేళ(ldap_విలువ_needs_base64(attr, attr_len, vals[i])) {
      అక్షరం *val_b64 = ఖాళీ;
      size_t val_b64_sz = 0;

      /* Binary attribute, encode to base64. */
      ఒకవేళ(vals[i]->bv_len)
        ఫలితం = curlx_base64_encode((uint8_t *)vals[i]->bv_val,
                                     vals[i]->bv_len,
                                     &val_b64, &val_b64_sz);
      ఒకవేళ(!ఫలితం)
        ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, ": ", 2);
      ఒకవేళ(!ఫలితం && val_b64_sz)
        ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, val_b64,
                                   val_b64_sz);
      curlx_free(val_b64);
    }
    లేకపోతే {
      ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, " ", 1);
      ఒకవేళ(!ఫలితం)
        ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY,
                                   vals[i]->bv_val, vals[i]->bv_len);
    }

    ఒకవేళ(!ఫలితం)
      ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, "\n", 1);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode ldap_do(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  curl_ldap_అంకె_t rc = LDAP_SUCCESS;
  LDAP *server = ఖాళీ;
  LDAPURLDesc *ludp = ఖాళీ;
  LDAPMessage *ldapmsg = ఖాళీ;
  LDAPMessage *నమోదు_Iterator;
  పూర్ణాంకం అంకె = 0;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  పూర్ణాంకం ldap_proto = LDAP_VERSION3;
  bool ldap_ssl = FALSE;
#నిర్వచితమైతే LDAP_OPT_జాలం_TIMEOUT
  నిర్మాణం timeval ldap_timeout = { 10, 0 }; /* 10s connection/search timeout */
#అయితే_ముగింపు
#నిర్వచితమైతే USE_WIN32_LDAP
  TCHAR *host = ఖాళీ;
#లేకపోతే
  అక్షరం *host = ఖాళీ;
#అయితే_ముగింపు
  స్థిరరాశి అక్షరం *user = దత్తాంశం->state.creds ? దత్తాంశం->state.creds->user : ఖాళీ;
  స్థిరరాశి అక్షరం *passwd = దత్తాంశం->state.creds ? దత్తాంశం->state.creds->passwd : ఖాళీ;
  నిర్మాణం ip_quadruple ipquad;
  bool is_ipv6;
  BerElement *ber = ఖాళీ;

  *పూర్తి = TRUE; /* unconditionally */
  infof(దత్తాంశం, "LDAP local: LDAP Vendor = %s ; LDAP Version = %d",
        LDAP_VENDOR_NAME, LDAP_VENDOR_సంస్కరణ);
  infof(దత్తాంశం, "LDAP local: %s", Curl_bufref_ptr(&దత్తాంశం->state.url));

#నిర్వచితమైతే HAVE_LDAP_URL_PARSE
  rc = ldap_url_parse(Curl_bufref_ptr(&దత్తాంశం->state.url), &ludp);
#లేకపోతే
  rc = ldap_url_parse_దిగువ(దత్తాంశం, conn, &ludp);
#అయితే_ముగింపు
  ఒకవేళ(rc) {
    failf(దత్తాంశం, "Bad LDAP URL: %s", ldap_err2string(rc));
    ఫలితం = CURLE_URL_MALFORMAT;
    వెళ్ళు quit;
  }

  ఫలితం = Curl_conn_get_ip_info(దత్తాంశం, conn, FIRSTSOCKET, &is_ipv6, &ipquad);
  ఒకవేళ(ఫలితం)
    వెళ్ళు quit;

  /* Get the URL scheme (either ldap or ldaps) */
  ldap_ssl = Curl_conn_is_ssl(conn, FIRSTSOCKET);
  infof(దత్తాంశం, "LDAP local: trying to establish %s connection",
        ldap_ssl ? "encrypted" : "cleartext");

#నిర్వచితమైతే USE_WIN32_LDAP
  host = curlx_convert_UTF8_గమ్యం_tchar(conn->origin->hostname);
  ఒకవేళ(!host) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    వెళ్ళు quit;
  }
#లేకపోతే
  host = conn->origin->hostname;
#అయితే_ముగింపు

#నిర్వచితమైతే USE_WIN32_LDAP
  ఒకవేళ(ldap_ssl)
    server = ldap_sslinit(host, (curl_ldap_అంకె_t)ipquad.remote_పోర్టు, 1);
  లేకపోతే
#అయితే_ముగింపు
    server = ldap_init(host, (curl_ldap_అంకె_t)ipquad.remote_పోర్టు);
  ఒకవేళ(!server) {
    failf(దత్తాంశం, "LDAP: cannot setup connect to %s:%u",
          conn->origin->user_hostname, ipquad.remote_పోర్టు);
    ఫలితం = CURLE_COULDNT_CONNECT;
    వెళ్ళు quit;
  }

#నిర్వచితమైతే LDAP_OPT_జాలం_TIMEOUT
  ldap_సమితి_option(server, LDAP_OPT_జాలం_TIMEOUT, &ldap_timeout);
#అయితే_ముగింపు
  ldap_సమితి_option(server, LDAP_OPT_PROTOCOL_సంస్కరణ, &ldap_proto);

  /* Do not chase referrals. */
  ldap_సమితి_option(server, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  ఒకవేళ(ldap_ssl) {
#నిర్వచితమైతే HAVE_LDAP_SSL
#నిర్వచితమైతే USE_WIN32_LDAP
    /* Win32 LDAP uses the Windows CA store to verify certificates */
    ldap_సమితి_option(server, LDAP_OPT_SSL, LDAP_OPT_ON);
    ఒకవేళ(!conn->ssl_config.verifypeer) {
      ఒకవేళ(conn->ssl_config.verifyhost) {
        failf(దత్తాంశం, "LDAP local: host verification cannot be enabled when "
              "peer verification is disabled for Windows native LDAP");
        ఫలితం = CURLE_NOT_BUILT_లోపల;
        వెళ్ళు quit;
      }
      ldap_సమితి_option(server, LDAP_OPT_SERVER_CERTIFICATE,
                      (శూన్య *)(uintptr_t)bypass_cert_verify);
    }
#లేదావేళ defined(LDAP_OPT_X_TLS) /* !USE_WIN32_LDAP */
    పూర్ణాంకం ldap_option;
    స్థిరరాశి అక్షరం *ldap_ca = conn->ssl_config.CAfile;
    ఒకవేళ(conn->ssl_config.verifypeer) {
      /* OpenLDAP SDK supports BASE64 files. */
      ఒకవేళ(conn->ssl_config.cert_type &&
         !curl_strequal(conn->ssl_config.cert_type, "PEM")) {
        failf(దత్తాంశం, "LDAP local: ERROR OpenLDAP only supports PEM cert-type");
        ఫలితం = CURLE_SSL_CERTPROBLEM;
        వెళ్ళు quit;
      }
      ఒకవేళ(!ldap_ca) {
        failf(దత్తాంశం, "LDAP local: ERROR PEM CA cert not set");
        ఫలితం = CURLE_SSL_CERTPROBLEM;
        వెళ్ళు quit;
      }
      infof(దత్తాంశం, "LDAP local: using PEM CA cert: %s", ldap_ca);
      rc = ldap_సమితి_option(server, LDAP_OPT_X_TLS_CACERTFILE, ldap_ca);
      ఒకవేళ(rc != LDAP_SUCCESS) {
        failf(దత్తాంశం, "LDAP local: ERROR setting PEM CA cert: %s",
              ldap_err2string(rc));
        ఫలితం = CURLE_SSL_CERTPROBLEM;
        వెళ్ళు quit;
      }
      ldap_option = LDAP_OPT_X_TLS_DEMAND;
    }
    లేకపోతే
      ldap_option = LDAP_OPT_X_TLS_NEVER;

    rc = ldap_సమితి_option(server, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_option);
    ఒకవేళ(rc != LDAP_SUCCESS) {
      failf(దత్తాంశం, "LDAP local: ERROR setting cert verify mode: %s",
            ldap_err2string(rc));
      ఫలితం = CURLE_SSL_CERTPROBLEM;
      వెళ్ళు quit;
    }
    ldap_option = LDAP_OPT_X_TLS_HARD;
    rc = ldap_సమితి_option(server, LDAP_OPT_X_TLS, &ldap_option);
    ఒకవేళ(rc != LDAP_SUCCESS) {
      failf(దత్తాంశం, "LDAP local: ERROR setting SSL/TLS mode: %s",
            ldap_err2string(rc));
      ఫలితం = CURLE_SSL_CERTPROBLEM;
      వెళ్ళు quit;
    }
#లేకపోతే /* !USE_WIN32_LDAP && !LDAP_OPT_X_TLS */
    /* we should probably never come up to here since configure
       should check in first place if we can support LDAP SSL/TLS */
    failf(దత్తాంశం, "LDAP local: SSL/TLS not supported with this version "
          "of the OpenLDAP toolkit");
    ఫలితం = CURLE_SSL_CERTPROBLEM;
    వెళ్ళు quit;
#అయితే_ముగింపు
#అయితే_ముగింపు /* HAVE_LDAP_SSL */
  }
  లేకపోతే ఒకవేళ(దత్తాంశం->సమితి.use_ssl > CURLUSESSL_TRY) {
    failf(దత్తాంశం, "LDAP local: explicit TLS not supported");
    ఫలితం = CURLE_NOT_BUILT_లోపల;
    వెళ్ళు quit;
  }

#నిర్వచితమైతే USE_WIN32_LDAP
  rc = ldap_win_bind(దత్తాంశం, server, user, passwd);
#లేకపోతే
  rc = ldap_simple_bind_s(server, user, passwd);
#అయితే_ముగింపు
  ఒకవేళ(!ldap_ssl && rc != LDAP_SUCCESS) {
    ldap_proto = LDAP_VERSION2;
    ldap_సమితి_option(server, LDAP_OPT_PROTOCOL_సంస్కరణ, &ldap_proto);
#నిర్వచితమైతే USE_WIN32_LDAP
    rc = ldap_win_bind(దత్తాంశం, server, user, passwd);
#లేకపోతే
    rc = ldap_simple_bind_s(server, user, passwd);
#అయితే_ముగింపు
  }
  ఒకవేళ(rc != LDAP_SUCCESS) {
#నిర్వచితమైతే USE_WIN32_LDAP
    failf(దత్తాంశం, "LDAP local: bind via ldap_win_bind %s",
          ldap_err2string(rc));
#లేకపోతే
    failf(దత్తాంశం, "LDAP local: bind via ldap_simple_bind_s %s",
          ldap_err2string(rc));
#అయితే_ముగింపు
    ఫలితం = CURLE_LDAP_CANNOT_BIND;
    వెళ్ళు quit;
  }

  Curl_pgrsReset(దత్తాంశం);
  rc = ldap_శోధన_s(server, ludp->lud_dn,
                     ludp->lud_scope,
                     ludp->lud_filter, ludp->lud_attrs, 0, &ldapmsg);

  ఒకవేళ(rc != LDAP_SUCCESS && rc != LDAP_SIZELIMIT_EXCEEDED) {
    failf(దత్తాంశం, "LDAP remote: %s", ldap_err2string(rc));
    ఫలితం = CURLE_LDAP_శోధన_FAILED;
    వెళ్ళు quit;
  }

  అంకె = 0;
  కొరకు(నమోదు_Iterator = ldap_మొదటి_నమోదు(server, ldapmsg);
      నమోదు_Iterator;
      నమోదు_Iterator = ldap_తదుపరి_నమోదు(server, నమోదు_Iterator), అంకె++) {
#నిర్వచితమైతే USE_WIN32_LDAP
    TCHAR *attribute;
#లేకపోతే
    అక్షరం *attribute;
#అయితే_ముగింపు

    /* Get the DN and write it to the client */
    {
      అక్షరం *name = ఖాళీ;
      size_t name_len = 0;
#నిర్వచితమైతే USE_WIN32_LDAP
      TCHAR *dn = ldap_get_dn(server, నమోదు_Iterator);
      ఒకవేళ(dn)
        name = curlx_convert_tchar_గమ్యం_UTF8(dn);
#లేకపోతే
      అక్షరం *dn = name = ldap_get_dn(server, నమోదు_Iterator);
#అయితే_ముగింపు
      ఒకవేళ(!name)
        ఫలితం = dn ? CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం : CURLE_FAILED_INIT;
      లేకపోతే {
        name_len = అక్షరశ్రేణి_పొడవు(name);
        ఒకవేళ(ldap_str_needs_base64(name, name_len)) {
          అక్షరం *dn_b64 = ఖాళీ;
          size_t dn_b64_sz = 0;

          /* A DN with e.g. CR/LF would inject LDIF lines, so encode it */
          ఫలితం = curlx_base64_encode((uint8_t *)name, name_len,
                                       &dn_b64, &dn_b64_sz);
          ఒకవేళ(!ఫలితం)
            ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, "DN:: ", 5);
          ఒకవేళ(!ఫలితం)
            ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, dn_b64,
                                       dn_b64_sz);
          curlx_free(dn_b64);
        }
        లేకపోతే {
          ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, "DN: ", 4);
          ఒకవేళ(!ఫలితం)
            ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, name, name_len);
        }
        ఒకవేళ(!ఫలితం)
          ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, "\n", 1);
      }
      FREE_ON_WINLDAP(name);
      ldap_memfree(dn);
      ఒకవేళ(ఫలితం)
        వెళ్ళు quit;
    }

    /* Get the attributes and write them to the client */
    కొరకు(attribute = ldap_మొదటి_attribute(server, నమోదు_Iterator, &ber);
        attribute;
        attribute = ldap_తదుపరి_attribute(server, నమోదు_Iterator, ber)) {
      Ber_విలువ **vals;
#నిర్వచితమైతే USE_WIN32_LDAP
      అక్షరం *attr = curlx_convert_tchar_గమ్యం_UTF8(attribute);
      ఒకవేళ(!attr) {
        ldap_memfree(attribute);
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు quit;
      }
#లేకపోతే
      అక్షరం *attr = attribute;
#అయితే_ముగింపు
      vals = ldap_get_విలువలు_len(server, నమోదు_Iterator, attribute);
      ఒకవేళ(vals) {
        ఫలితం = show_vals(దత్తాంశం, vals, attr);
        /* Free memory used to store values */
        ldap_విలువ_free_len(vals);
      }

      /* Free the attribute as we are done with it */
      FREE_ON_WINLDAP(attr);
      ldap_memfree(attribute);

      ఒకవేళ(!ఫలితం)
        ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, "\n", 1);
      ఒకవేళ(ఫలితం)
        వెళ్ళు quit;
    }

    ఒకవేళ(ber) {
      ber_free(ber, 0);
      ber = ఖాళీ;
    }
  }

quit:
  ఒకవేళ(ber)
    ber_free(ber, 0);
  ఒకవేళ(ldapmsg) {
    ldap_msgfree(ldapmsg);
    LDAP_TRACE(("Received %d entries\n", అంకె));
  }
  ఒకవేళ(rc == LDAP_SIZELIMIT_EXCEEDED)
    infof(దత్తాంశం, "There are more than %d entries", అంకె);
  ఒకవేళ(ludp)
    ldap_free_urldesc(ludp);
  ఒకవేళ(server)
    ldap_unbind_s(server);

  FREE_ON_WINLDAP(host);

  /* no data to transfer */
  Curl_xfer_అమరిక_nop(దత్తాంశం);
  connclose(conn);

  తిరిగి ఫలితం;
}

#నిర్వచితమైతే DEBUG_LDAP
స్థిర శూన్య ldap_trace_దిగువ(స్థిరరాశి అక్షరం *fmt, ...)
{
  స్థిర పూర్ణాంకం do_trace = -1;
  va_list వాదనలు;

  ఒకవేళ(do_trace == -1) {
    స్థిరరాశి అక్షరం *env = getenv("CURL_TRACE");
    curl_off_t e = 0;
    ఒకవేళ(!curlx_str_సంఖ్య(&env, &e, గరిష్ఠ_పూర్ణాంకం))
      do_trace = e > 0;
  }
  ఒకవేళ(!do_trace)
    తిరిగి;

  va_start(వాదనలు, fmt);
  curl_mvfprintf(stderr, fmt, వాదనలు);
  va_end(వాదనలు);
}
#అయితే_ముగింపు /* DEBUG_LDAP */

#నిర్వచితం_కాకపోతే HAVE_LDAP_URL_PARSE

/*
 * Return scope-value for a scope-string.
 */
స్థిర పూర్ణాంకం str2scope(స్థిరరాశి అక్షరం *p)
{
  ఒకవేళ(curl_strequal(p, "one"))
    తిరిగి LDAP_SCOPE_ONELEVEL;
  ఒకవేళ(curl_strequal(p, "onetree"))
    తిరిగి LDAP_SCOPE_ONELEVEL;
  ఒకవేళ(curl_strequal(p, "base"))
    తిరిగి LDAP_SCOPE_పునాది;
  ఒకవేళ(curl_strequal(p, "sub"))
    తిరిగి LDAP_SCOPE_SUBTREE;
  ఒకవేళ(curl_strequal(p, "subtree"))
    తిరిగి LDAP_SCOPE_SUBTREE;
  తిరిగి -1;
}

/* number of entries in the attributes list */
స్థిర size_t అంకె_entries(స్థిరరాశి అక్షరం *s)
{
  size_t వస్తువులు = 1;

  s = strchr(s, ',');
  అయితే(s) {
    వస్తువులు++;
    s = strchr(s + 1, ',');
  }
  తిరిగి వస్తువులు;
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
స్థిర curl_ldap_అంకె_t ldap_url_parse2_దిగువ(నిర్మాణం Curl_easy *దత్తాంశం,
                                           స్థిరరాశి నిర్మాణం connectdata *conn,
                                           LDAPURLDesc *ludp)
{
  curl_ldap_అంకె_t rc = LDAP_SUCCESS;
  అక్షరం *p;
  అక్షరం *మార్గం;
  అక్షరం *q = ఖాళీ;
  అక్షరం *query = ఖాళీ;
  size_t i;

  ఒకవేళ(!దత్తాంశం ||
     !దత్తాంశం->state.పైకి.మార్గం ||
     దత్తాంశం->state.పైకి.మార్గం[0] != '/' ||
     ((దత్తాంశం->state.origin->scheme != &Curl_scheme_ldap) &&
      (దత్తాంశం->state.origin->scheme != &Curl_scheme_ldaps)))
    తిరిగి LDAP_INVALID_SYNTAX;

  ludp->lud_scope = LDAP_SCOPE_పునాది;
  ludp->lud_పోర్టు  = conn->origin->పోర్టు;
  ludp->lud_host  = conn->origin->hostname;

  /* Duplicate the path */
  p = మార్గం = curlx_strdup(దత్తాంశం->state.పైకి.మార్గం + 1);
  ఒకవేళ(!మార్గం)
    తిరిగి LDAP_NO_జ్ఞాపకం;

  /* Duplicate the query if present */
  ఒకవేళ(దత్తాంశం->state.పైకి.query) {
    q = query = curlx_strdup(దత్తాంశం->state.పైకి.query);
    ఒకవేళ(!query) {
      curlx_free(మార్గం);
      తిరిగి LDAP_NO_జ్ఞాపకం;
    }
  }

  /* Parse the DN (Distinguished Name) */
  ఒకవేళ(*p) {
    అక్షరం *dn = p;
    అక్షరం *unescaped;
    CURLcode ఫలితం;

    LDAP_TRACE(("DN '%s'\n", dn));

    /* Unescape the DN */
    ఫలితం = Curl_urldecode(dn, 0, &unescaped, ఖాళీ, REJECT_ZERO);
    ఒకవేళ(ఫలితం) {
      rc = LDAP_NO_జ్ఞాపకం;

      వెళ్ళు quit;
    }

#నిర్వచితమైతే USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_dn = curlx_convert_UTF8_గమ్యం_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    ఒకవేళ(!ludp->lud_dn) {
      rc = LDAP_NO_జ్ఞాపకం;

      వెళ్ళు quit;
    }
#లేకపోతే
    ludp->lud_dn = unescaped;
#అయితే_ముగింపు
  }

  p = q;
  ఒకవేళ(!p)
    వెళ్ళు quit;

  /* Parse the attributes. skip "??" */
  q = strchr(p, '?');
  ఒకవేళ(q)
    *q++ = '\0';

  ఒకవేళ(*p) {
    size_t లెక్క = అంకె_entries(p); /* at least one */
    స్థిరరాశి అక్షరం *atp = p;

    /* Allocate our array (+1 for the NULL entry) */
#నిర్వచితమైతే USE_WIN32_LDAP
    ludp->lud_attrs = curlx_calloc(లెక్క + 1, పరిమాణం(TCHAR *));
#లేకపోతే
    ludp->lud_attrs = curlx_calloc(లెక్క + 1, పరిమాణం(అక్షరం *));
#అయితే_ముగింపు
    ఒకవేళ(!ludp->lud_attrs) {
      rc = LDAP_NO_జ్ఞాపకం;
      వెళ్ళు quit;
    }

    కొరకు(i = 0; i < లెక్క; i++) {
      అక్షరం *unescaped;
      CURLcode ఫలితం;
      నిర్మాణం Curl_str బహిర్గతం;

      ఒకవేళ(curlx_str_until(&atp, &బహిర్గతం, 1024, ','))
        ఆపు;

      LDAP_TRACE(("attr[%zu] '%.*s'\n", i, (పూర్ణాంకం)బహిర్గతం.len, బహిర్గతం.str));

      /* Unescape the attribute */
      ఫలితం = Curl_urldecode(బహిర్గతం.str, బహిర్గతం.len, &unescaped, ఖాళీ, REJECT_ZERO);
      ఒకవేళ(ఫలితం) {
        rc = LDAP_NO_జ్ఞాపకం;
        వెళ్ళు quit;
      }

#నిర్వచితమైతే USE_WIN32_LDAP
      /* Convert the unescaped string to a tchar */
      ludp->lud_attrs[i] = curlx_convert_UTF8_గమ్యం_tchar(unescaped);

      /* Free the unescaped string as we are done with it */
      curlx_free(unescaped);

      ఒకవేళ(!ludp->lud_attrs[i]) {
        rc = LDAP_NO_జ్ఞాపకం;
        వెళ్ళు quit;
      }
#లేకపోతే
      ludp->lud_attrs[i] = unescaped;
#అయితే_ముగింపు

      ludp->lud_attrs_dups++;
      ఒకవేళ(curlx_str_single(&atp, ','))
        ఆపు;
    }
  }

  p = q;
  ఒకవేళ(!p)
    వెళ్ళు quit;

  /* Parse the scope. skip "??" */
  q = strchr(p, '?');
  ఒకవేళ(q)
    *q++ = '\0';

  ఒకవేళ(*p) {
    ludp->lud_scope = str2scope(p);
    ఒకవేళ(ludp->lud_scope == -1) {
      rc = LDAP_INVALID_SYNTAX;

      వెళ్ళు quit;
    }
    LDAP_TRACE(("scope %d\n", ludp->lud_scope));
  }

  p = q;
  ఒకవేళ(!p)
    వెళ్ళు quit;

  /* Parse the filter */
  q = strchr(p, '?');
  ఒకవేళ(q)
    *q++ = '\0';

  ఒకవేళ(*p) {
    అక్షరం *filter = p;
    అక్షరం *unescaped;
    CURLcode ఫలితం;

    LDAP_TRACE(("filter '%s'\n", filter));

    /* Unescape the filter */
    ఫలితం = Curl_urldecode(filter, 0, &unescaped, ఖాళీ, REJECT_CTRL);
    ఒకవేళ(ఫలితం) {
      rc = LDAP_NO_జ్ఞాపకం;

      వెళ్ళు quit;
    }

#నిర్వచితమైతే USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_filter = curlx_convert_UTF8_గమ్యం_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    ఒకవేళ(!ludp->lud_filter) {
      rc = LDAP_NO_జ్ఞాపకం;

      వెళ్ళు quit;
    }
#లేకపోతే
    ludp->lud_filter = unescaped;
#అయితే_ముగింపు
  }

  p = q;
  ఒకవేళ(p && !*p) {
    rc = LDAP_INVALID_SYNTAX;

    వెళ్ళు quit;
  }

quit:
  curlx_free(మార్గం);
  curlx_free(query);

  తిరిగి rc;
}

స్థిర curl_ldap_అంకె_t ldap_url_parse_దిగువ(నిర్మాణం Curl_easy *దత్తాంశం,
                                          స్థిరరాశి నిర్మాణం connectdata *conn,
                                          LDAPURLDesc **ludpp)
{
  LDAPURLDesc *ludp = curlx_calloc(1, పరిమాణం(*ludp));
  curl_ldap_అంకె_t rc;

  *ludpp = ఖాళీ;
  ఒకవేళ(!ludp)
    తిరిగి LDAP_NO_జ్ఞాపకం;

  rc = ldap_url_parse2_దిగువ(దత్తాంశం, conn, ludp);
  ఒకవేళ(rc != LDAP_SUCCESS) {
    ldap_free_urldesc_దిగువ(ludp);
    ludp = ఖాళీ;
  }
  *ludpp = ludp;
  తిరిగి rc;
}

స్థిర శూన్య ldap_free_urldesc_దిగువ(LDAPURLDesc *ludp)
{
  ఒకవేళ(!ludp)
    తిరిగి;

  curlx_free(ludp->lud_dn);
  curlx_free(ludp->lud_filter);

  ఒకవేళ(ludp->lud_attrs) {
    size_t i;
    కొరకు(i = 0; i < ludp->lud_attrs_dups; i++) {
      curlx_free(ludp->lud_attrs[i]);
    }
    curlx_free(ludp->lud_attrs);
  }

  curlx_free(ludp);
}
#అయితే_ముగింపు /* !HAVE_LDAP_URL_PARSE */

శూన్య Curl_ldap_సంస్కరణ(అక్షరం *buf, size_t bufsz)
{
#నిర్వచితమైతే USE_WIN32_LDAP
  curl_msnprintf(buf, bufsz, "WinLDAP");
#లేకపోతే
#నిర్వచితమైతే LDAP_OPT_X_TLS_PASSPHRASE
  స్థిర స్థిరరాశి అక్షరం flavor[] = "/Apple";
#లేకపోతే
  స్థిర స్థిరరాశి అక్షరం flavor[] = "";
#అయితే_ముగింపు
  LDAPAPIInfo api;
  api.ldapai_info_సంస్కరణ = LDAP_API_INFO_సంస్కరణ;

  /* Comparing against 0, as different platforms
     disagree on the success define name */
  ఒకవేళ(ldap_get_option(ఖాళీ, LDAP_OPT_API_INFO, &api) == 0) {
    గుర్తులేని పూర్ణాంకం patch = (గుర్తులేని పూర్ణాంకం)(api.ldapai_vendor_సంస్కరణ % 100);
    గుర్తులేని పూర్ణాంకం major = (గుర్తులేని పూర్ణాంకం)(api.ldapai_vendor_సంస్కరణ / 10000);
    గుర్తులేని పూర్ణాంకం minor =
      (((గుర్తులేని పూర్ణాంకం)api.ldapai_vendor_సంస్కరణ - (major * 10000))
       - patch) / 100;
#నిర్వచితమైతే __OS400__
    curl_msnprintf(buf, bufsz, "IBMLDAP/%u.%u.%u", major, minor, patch);
    ldap_విలువ_free(api.ldapai_extensions);
    (శూన్య)flavor;
#లేకపోతే
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u%s",
                   api.ldapai_vendor_name, major, minor, patch, flavor);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((శూన్య **)api.ldapai_extensions);
#అయితే_ముగింపు
  }
  లేకపోతే
    curl_msnprintf(buf, bufsz, "LDAP/1");
#అయితే_ముగింపు /* USE_WIN32_LDAP */
}

/*
 * LDAP protocol handler.
 */
స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_ldap = {
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

#ఒకవేళ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు

#అయితే_ముగింపు /* !CURL_DISABLE_LDAP && !USE_OPENLDAP */
