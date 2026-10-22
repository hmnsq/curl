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
#ضمن "curl_setup.h"
#ضمن "urldata.h"
#ضمن "curl_ldap.h"

#إذا !defined(CURL_DISABLE_LDAP) && !defined(USE_OPENLDAP)

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#لارښوونه GCC diagnostic ټېل
#لارښوونه GCC diagnostic ignored "-Wdeprecated-declarations"
#د_که_پای

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

#که_تعریف_شوی USE_WIN32_LDAP           /* Use Windows LDAP implementation. */
#  include <winldap.h>
/* Undefine indirect <wincrypt.h> symbols conflicting with AWS-LC/BoringSSL. */
#  undef X509_NAME
#  undef X509_EXTENSIONS
#  undef PKCS7_ISSUER_AND_SERIAL
#  undef PKCS7_SIGNER_INFO
#  undef OCSP_REQUEST
#  undef OCSP_RESPONSE
#  include <winber.h>
#وإلا
#  define LDAP_DEPRECATED 1     /* Be sure ldap_init() is defined. */
#  ifdef HAVE_LBER_H
#    include <lber.h>
#  endif
#  include <ldap.h>
#  إذا defined(HAVE_LDAP_SSL) && defined(HAVE_LDAP_SSL_H)
#    include <ldap_ssl.h>
#  endif /* HAVE_LDAP_SSL && HAVE_LDAP_SSL_H */
#د_که_پای

#ضمن "cfilters.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "escape.h"
#ضمن "progress.h"
#ضمن "transfer.h"
#ضمن "curlx/strparse.h"
#ضمن "bufref.h"
#ضمن "curlx/multibyte.h"
#ضمن "curlx/base64.h"
#ضمن "connect.h"

#که_تعریف_شوی USE_WIN32_LDAP
#تعریف_کړه FREE_ON_WINLDAP(x) curlx_free(x)
#تعریف_کړه curl_ldap_عدد_t    ULONG
#وإلا
#تعریف_کړه FREE_ON_WINLDAP(x) افعل {} طالما(0)
#تعریف_کړه curl_ldap_عدد_t    صحيح
#د_که_پای

#که_نه_تعریف_شوی HAVE_LDAP_URL_PARSE

/* Use our own implementation. */

بنية ldap_urldesc {
  حرف   *lud_host;
  صحيح     lud_پورټ;
#که_تعریف_شوی USE_WIN32_LDAP
  TCHAR  *lud_dn;
  TCHAR **lud_attrs;
#وإلا
  حرف   *lud_dn;
  حرف  **lud_attrs;
#د_که_پای
  صحيح     lud_scope;
#که_تعریف_شوی USE_WIN32_LDAP
  TCHAR  *lud_filter;
#وإلا
  حرف   *lud_filter;
#د_که_پای
  حرف  **lud_exts;
  size_t  lud_attrs_dups; /* how many were dup'ed, this field is not in the
                             "real" struct so can only be used in code
                             without HAVE_LDAP_URL_PARSE defined */
};

#تعریف_لغوه LDAPURLDesc
#تعریف_کړه LDAPURLDesc بنية ldap_urldesc

ساكن curl_ldap_عدد_t ldap_url_parse_ټیټ(بنية Curl_easy *معلومات,
                                          ثابت بنية connectdata *conn,
                                          LDAPURLDesc **ludpp);
ساكن فراغ ldap_free_urldesc_ټیټ(LDAPURLDesc *ludp);

#تعریف_لغوه ldap_free_urldesc
#تعریف_کړه ldap_free_urldesc       ldap_free_urldesc_ټیټ

#د_که_پای /* !HAVE_LDAP_URL_PARSE */

#که_تعریف_شوی DEBUG_LDAP
#تعریف_کړه LDAP_TRACE(x)                 \
  افعل {                                \
    ldap_trace_ټیټ("%u: ", __LINE__); \
    ldap_trace_ټیټ x;                 \
  } طالما(0)

ساكن فراغ ldap_trace_ټیټ(ثابت حرف *fmt, ...) CURL_PRINTF(1, 2);
#وإلا
#تعریف_کړه LDAP_TRACE(x) Curl_nop_stmt
#د_که_پای

#إذا defined(USE_WIN32_LDAP) && defined(ldap_err2string)
/* Use ANSI error strings in Unicode builds */
#تعریف_لغوه ldap_err2string
#تعریف_کړه ldap_err2string ldap_err2stringA
#د_که_پای

#إذا defined(USE_WIN32_LDAP) && defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'void *' of greater size */
#تعریف_لغوه LDAP_OPT_ON
#تعریف_لغوه LDAP_OPT_OFF
#تعریف_کړه LDAP_OPT_ON   ((فراغ *)(size_t)1)
#تعریف_کړه LDAP_OPT_OFF  ((فراغ *)(size_t)0)
#د_که_پای

#که_تعریف_شوی USE_WIN32_LDAP

#که_تعریف_شوی USE_WINDOWS_SSPI
ساكن ULONG ldap_win_bind_auth(LDAP *server, ثابت حرف *user,
                                ثابت حرف *passwd, بینښه اوږد authflags)
{
  ULONG لاره = 0;
  SEC_WINNT_AUTH_بې_اغېزه_عنصر_EX cred;
  ULONG rc = LDAP_AUTH_لاره_NOT_SUPPORTED;

  memset(&cred, 0, حجم(cred));

#که_تعریف_شوی USE_SPNEGO
  إذا(authflags & CURLAUTH_NEGOTIATE) {
    لاره = LDAP_AUTH_NEGOTIATE;
  }
  وإلا
#د_که_پای
#که_تعریف_شوی USE_NTLM
  إذا(authflags & CURLAUTH_NTLM) {
    لاره = LDAP_AUTH_NTLM;
  }
  وإلا
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_DIGEST_AUTH
  إذا(authflags & CURLAUTH_DIGEST) {
    لاره = LDAP_AUTH_DIGEST;
  }
  وإلا
#د_که_پای
  {
    /* required anyway if one of upper preprocessor definitions enabled */
  }

  إذا(لاره && user && passwd) {
    CURLcode پایله = Curl_create_sspi_بې_اغېزه_عنصر(user, passwd, &cred);
    إذا(!پایله) {
      rc = ldap_bind_s(server, عدم, (TCHAR *)&cred, لاره);
      Curl_sspi_free_بې_اغېزه_عنصر(&cred);
    }
    وإلا {
      rc = LDAP_NO_حافظه;
    }
  }
  وإلا {
    /* proceed with current user credentials */
    لاره = LDAP_AUTH_NEGOTIATE;
    rc = ldap_bind_s(server, عدم, عدم, لاره);
  }
  أرجع rc;
}
#د_که_پای /* USE_WINDOWS_SSPI */

ساكن ULONG ldap_win_bind(بنية Curl_easy *معلومات, LDAP *server,
                           ثابت حرف *user, ثابت حرف *passwd)
{
  ULONG rc = LDAP_INVALID_CREDENTIALS;

  PTCHAR inuser = عدم;
  PTCHAR inpass = عدم;

  إذا(user && passwd && (معلومات->سېټ.httpauth & CURLAUTH_BASIC)) {
    inuser = curlx_convert_UTF8_منزل_tchar(user);
    inpass = curlx_convert_UTF8_منزل_tchar(passwd);

    rc = ldap_simple_bind_s(server, inuser, inpass);

    curlx_free(inuser);
    curlx_free(inpass);
  }
#که_تعریف_شوی USE_WINDOWS_SSPI
  وإلا {
    rc = ldap_win_bind_auth(server, user, passwd, معلومات->سېټ.httpauth);
  }
#د_که_پای

  أرجع rc;
}
#د_که_پای /* USE_WIN32_LDAP */

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
ساكن bool ldap_str_needs_base64(ثابت حرف *s, size_t len)
{
  size_t i;
  إذا(!len)
    أرجع FALSE;
  إذا((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    أرجع TRUE;
  لكل(i = 0; i < len; i++)
    إذا(!ISPRINT(s[i]))
      أرجع TRUE;
  أرجع FALSE;
}

ساكن bool ldap_ارزښت_needs_base64(ثابت حرف *attr, size_t attr_len,
                                    ثابت Ber_ارزښت *val)
{
  إذا((attr_len > 7) && curl_strequal(";binary", attr + attr_len - 7))
    أرجع TRUE;

  أرجع ldap_str_needs_base64(val->bv_val, val->bv_len);
}

#که_تعریف_شوی USE_WIN32_LDAP
ساكن BOOLEAN bypass_cert_verify(PLDAP Connection,
                                  PCCERT_CONTEXT *ppServerCert)
{
  (فراغ)Connection;
  CertFreeCertificateContext(*ppServerCert);
  /* approve any certificate since the verification is set to bypass */
  أرجع TRUE;
}
#د_که_پای

ساكن CURLcode show_vals(بنية Curl_easy *معلومات, Ber_ارزښت **vals,
                          ثابت حرف *attr)
{
  CURLcode پایله = CURLE_OK;
  صحيح i;
  size_t attr_len = مزي_اوږدوالی(attr);

  /* An attribute type has no base64 form in LDIF, so a type carrying control
     bytes (CR/LF etc.) would let a hostile server inject lines. A conformant
     type name never contains them, so reject it. */
  إذا(ldap_str_needs_base64(attr, attr_len)) {
    failf(معلومات, "LDAP local: attribute type contains illegal bytes");
    أرجع CURLE_WEIRD_SERVER_ځواب;
  }

  لكل(i = 0; vals[i] && !پایله; i++) {
    پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY, "\t", 1);
    إذا(!پایله)
      پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY, attr, attr_len);
    إذا(!پایله)
      پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY, ":", 1);

    إذا(پایله)
      اكسر;

    إذا(ldap_ارزښت_needs_base64(attr, attr_len, vals[i])) {
      حرف *val_b64 = عدم;
      size_t val_b64_sz = 0;

      /* Binary attribute, encode to base64. */
      إذا(vals[i]->bv_len)
        پایله = curlx_base64_encode((uint8_t *)vals[i]->bv_val,
                                     vals[i]->bv_len,
                                     &val_b64, &val_b64_sz);
      إذا(!پایله)
        پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY, ": ", 2);
      إذا(!پایله && val_b64_sz)
        پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY, val_b64,
                                   val_b64_sz);
      curlx_free(val_b64);
    }
    وإلا {
      پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY, " ", 1);
      إذا(!پایله)
        پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY,
                                   vals[i]->bv_val, vals[i]->bv_len);
    }

    إذا(!پایله)
      پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY, "\n", 1);
  }
  أرجع پایله;
}

ساكن CURLcode ldap_do(بنية Curl_easy *معلومات, bool *بشپړ)
{
  CURLcode پایله = CURLE_OK;
  curl_ldap_عدد_t rc = LDAP_SUCCESS;
  LDAP *server = عدم;
  LDAPURLDesc *ludp = عدم;
  LDAPMessage *ldapmsg = عدم;
  LDAPMessage *ثبت_Iterator;
  صحيح عدد = 0;
  بنية connectdata *conn = معلومات->conn;
  صحيح ldap_proto = LDAP_VERSION3;
  bool ldap_ssl = FALSE;
#که_تعریف_شوی LDAP_OPT_شبکه_TIMEOUT
  بنية timeval ldap_timeout = { 10, 0 }; /* 10s connection/search timeout */
#د_که_پای
#که_تعریف_شوی USE_WIN32_LDAP
  TCHAR *host = عدم;
#وإلا
  حرف *host = عدم;
#د_که_پای
  ثابت حرف *user = معلومات->state.creds ? معلومات->state.creds->user : عدم;
  ثابت حرف *passwd = معلومات->state.creds ? معلومات->state.creds->passwd : عدم;
  بنية ip_quadruple ipquad;
  bool is_ipv6;
  BerElement *ber = عدم;

  *بشپړ = TRUE; /* unconditionally */
  infof(معلومات, "LDAP local: LDAP Vendor = %s ; LDAP Version = %d",
        LDAP_VENDOR_NAME, LDAP_VENDOR_نسخه);
  infof(معلومات, "LDAP local: %s", Curl_bufref_ptr(&معلومات->state.url));

#که_تعریف_شوی HAVE_LDAP_URL_PARSE
  rc = ldap_url_parse(Curl_bufref_ptr(&معلومات->state.url), &ludp);
#وإلا
  rc = ldap_url_parse_ټیټ(معلومات, conn, &ludp);
#د_که_پای
  إذا(rc) {
    failf(معلومات, "Bad LDAP URL: %s", ldap_err2string(rc));
    پایله = CURLE_URL_MALFORMAT;
    اذهب quit;
  }

  پایله = Curl_conn_get_ip_info(معلومات, conn, FIRSTSOCKET, &is_ipv6, &ipquad);
  إذا(پایله)
    اذهب quit;

  /* Get the URL scheme (either ldap or ldaps) */
  ldap_ssl = Curl_conn_is_ssl(conn, FIRSTSOCKET);
  infof(معلومات, "LDAP local: trying to establish %s connection",
        ldap_ssl ? "encrypted" : "cleartext");

#که_تعریف_شوی USE_WIN32_LDAP
  host = curlx_convert_UTF8_منزل_tchar(conn->origin->hostname);
  إذا(!host) {
    پایله = CURLE_بهر_د_حافظه;

    اذهب quit;
  }
#وإلا
  host = conn->origin->hostname;
#د_که_پای

#که_تعریف_شوی USE_WIN32_LDAP
  إذا(ldap_ssl)
    server = ldap_sslinit(host, (curl_ldap_عدد_t)ipquad.remote_پورټ, 1);
  وإلا
#د_که_پای
    server = ldap_init(host, (curl_ldap_عدد_t)ipquad.remote_پورټ);
  إذا(!server) {
    failf(معلومات, "LDAP: cannot setup connect to %s:%u",
          conn->origin->user_hostname, ipquad.remote_پورټ);
    پایله = CURLE_COULDNT_CONNECT;
    اذهب quit;
  }

#که_تعریف_شوی LDAP_OPT_شبکه_TIMEOUT
  ldap_سېټ_option(server, LDAP_OPT_شبکه_TIMEOUT, &ldap_timeout);
#د_که_پای
  ldap_سېټ_option(server, LDAP_OPT_PROTOCOL_نسخه, &ldap_proto);

  /* Do not chase referrals. */
  ldap_سېټ_option(server, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  إذا(ldap_ssl) {
#که_تعریف_شوی HAVE_LDAP_SSL
#که_تعریف_شوی USE_WIN32_LDAP
    /* Win32 LDAP uses the Windows CA store to verify certificates */
    ldap_سېټ_option(server, LDAP_OPT_SSL, LDAP_OPT_ON);
    إذا(!conn->ssl_config.verifypeer) {
      إذا(conn->ssl_config.verifyhost) {
        failf(معلومات, "LDAP local: host verification cannot be enabled when "
              "peer verification is disabled for Windows native LDAP");
        پایله = CURLE_NOT_BUILT_دننه;
        اذهب quit;
      }
      ldap_سېټ_option(server, LDAP_OPT_SERVER_CERTIFICATE,
                      (فراغ *)(uintptr_t)bypass_cert_verify);
    }
#وإلاإذا defined(LDAP_OPT_X_TLS) /* !USE_WIN32_LDAP */
    صحيح ldap_option;
    ثابت حرف *ldap_ca = conn->ssl_config.CAfile;
    إذا(conn->ssl_config.verifypeer) {
      /* OpenLDAP SDK supports BASE64 files. */
      إذا(conn->ssl_config.cert_type &&
         !curl_strequal(conn->ssl_config.cert_type, "PEM")) {
        failf(معلومات, "LDAP local: ERROR OpenLDAP only supports PEM cert-type");
        پایله = CURLE_SSL_CERTPROBLEM;
        اذهب quit;
      }
      إذا(!ldap_ca) {
        failf(معلومات, "LDAP local: ERROR PEM CA cert not set");
        پایله = CURLE_SSL_CERTPROBLEM;
        اذهب quit;
      }
      infof(معلومات, "LDAP local: using PEM CA cert: %s", ldap_ca);
      rc = ldap_سېټ_option(server, LDAP_OPT_X_TLS_CACERTFILE, ldap_ca);
      إذا(rc != LDAP_SUCCESS) {
        failf(معلومات, "LDAP local: ERROR setting PEM CA cert: %s",
              ldap_err2string(rc));
        پایله = CURLE_SSL_CERTPROBLEM;
        اذهب quit;
      }
      ldap_option = LDAP_OPT_X_TLS_DEMAND;
    }
    وإلا
      ldap_option = LDAP_OPT_X_TLS_NEVER;

    rc = ldap_سېټ_option(server, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_option);
    إذا(rc != LDAP_SUCCESS) {
      failf(معلومات, "LDAP local: ERROR setting cert verify mode: %s",
            ldap_err2string(rc));
      پایله = CURLE_SSL_CERTPROBLEM;
      اذهب quit;
    }
    ldap_option = LDAP_OPT_X_TLS_HARD;
    rc = ldap_سېټ_option(server, LDAP_OPT_X_TLS, &ldap_option);
    إذا(rc != LDAP_SUCCESS) {
      failf(معلومات, "LDAP local: ERROR setting SSL/TLS mode: %s",
            ldap_err2string(rc));
      پایله = CURLE_SSL_CERTPROBLEM;
      اذهب quit;
    }
#وإلا /* !USE_WIN32_LDAP && !LDAP_OPT_X_TLS */
    /* we should probably never come up to here since configure
       should check in first place if we can support LDAP SSL/TLS */
    failf(معلومات, "LDAP local: SSL/TLS not supported with this version "
          "of the OpenLDAP toolkit");
    پایله = CURLE_SSL_CERTPROBLEM;
    اذهب quit;
#د_که_پای
#د_که_پای /* HAVE_LDAP_SSL */
  }
  وإلا إذا(معلومات->سېټ.use_ssl > CURLUSESSL_TRY) {
    failf(معلومات, "LDAP local: explicit TLS not supported");
    پایله = CURLE_NOT_BUILT_دننه;
    اذهب quit;
  }

#که_تعریف_شوی USE_WIN32_LDAP
  rc = ldap_win_bind(معلومات, server, user, passwd);
#وإلا
  rc = ldap_simple_bind_s(server, user, passwd);
#د_که_پای
  إذا(!ldap_ssl && rc != LDAP_SUCCESS) {
    ldap_proto = LDAP_VERSION2;
    ldap_سېټ_option(server, LDAP_OPT_PROTOCOL_نسخه, &ldap_proto);
#که_تعریف_شوی USE_WIN32_LDAP
    rc = ldap_win_bind(معلومات, server, user, passwd);
#وإلا
    rc = ldap_simple_bind_s(server, user, passwd);
#د_که_پای
  }
  إذا(rc != LDAP_SUCCESS) {
#که_تعریف_شوی USE_WIN32_LDAP
    failf(معلومات, "LDAP local: bind via ldap_win_bind %s",
          ldap_err2string(rc));
#وإلا
    failf(معلومات, "LDAP local: bind via ldap_simple_bind_s %s",
          ldap_err2string(rc));
#د_که_پای
    پایله = CURLE_LDAP_CANNOT_BIND;
    اذهب quit;
  }

  Curl_pgrsReset(معلومات);
  rc = ldap_پلټنه_s(server, ludp->lud_dn,
                     ludp->lud_scope,
                     ludp->lud_filter, ludp->lud_attrs, 0, &ldapmsg);

  إذا(rc != LDAP_SUCCESS && rc != LDAP_SIZELIMIT_EXCEEDED) {
    failf(معلومات, "LDAP remote: %s", ldap_err2string(rc));
    پایله = CURLE_LDAP_پلټنه_FAILED;
    اذهب quit;
  }

  عدد = 0;
  لكل(ثبت_Iterator = ldap_لومړی_ثبت(server, ldapmsg);
      ثبت_Iterator;
      ثبت_Iterator = ldap_بل_ثبت(server, ثبت_Iterator), عدد++) {
#که_تعریف_شوی USE_WIN32_LDAP
    TCHAR *attribute;
#وإلا
    حرف *attribute;
#د_که_پای

    /* Get the DN and write it to the client */
    {
      حرف *name = عدم;
      size_t name_len = 0;
#که_تعریف_شوی USE_WIN32_LDAP
      TCHAR *dn = ldap_get_dn(server, ثبت_Iterator);
      إذا(dn)
        name = curlx_convert_tchar_منزل_UTF8(dn);
#وإلا
      حرف *dn = name = ldap_get_dn(server, ثبت_Iterator);
#د_که_پای
      إذا(!name)
        پایله = dn ? CURLE_بهر_د_حافظه : CURLE_FAILED_INIT;
      وإلا {
        name_len = مزي_اوږدوالی(name);
        إذا(ldap_str_needs_base64(name, name_len)) {
          حرف *dn_b64 = عدم;
          size_t dn_b64_sz = 0;

          /* A DN with e.g. CR/LF would inject LDIF lines, so encode it */
          پایله = curlx_base64_encode((uint8_t *)name, name_len,
                                       &dn_b64, &dn_b64_sz);
          إذا(!پایله)
            پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY, "DN:: ", 5);
          إذا(!پایله)
            پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY, dn_b64,
                                       dn_b64_sz);
          curlx_free(dn_b64);
        }
        وإلا {
          پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY, "DN: ", 4);
          إذا(!پایله)
            پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY, name, name_len);
        }
        إذا(!پایله)
          پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY, "\n", 1);
      }
      FREE_ON_WINLDAP(name);
      ldap_memfree(dn);
      إذا(پایله)
        اذهب quit;
    }

    /* Get the attributes and write them to the client */
    لكل(attribute = ldap_لومړی_attribute(server, ثبت_Iterator, &ber);
        attribute;
        attribute = ldap_بل_attribute(server, ثبت_Iterator, ber)) {
      Ber_ارزښت **vals;
#که_تعریف_شوی USE_WIN32_LDAP
      حرف *attr = curlx_convert_tchar_منزل_UTF8(attribute);
      إذا(!attr) {
        ldap_memfree(attribute);
        پایله = CURLE_بهر_د_حافظه;
        اذهب quit;
      }
#وإلا
      حرف *attr = attribute;
#د_که_پای
      vals = ldap_get_ارزښتونه_len(server, ثبت_Iterator, attribute);
      إذا(vals) {
        پایله = show_vals(معلومات, vals, attr);
        /* Free memory used to store values */
        ldap_ارزښت_free_len(vals);
      }

      /* Free the attribute as we are done with it */
      FREE_ON_WINLDAP(attr);
      ldap_memfree(attribute);

      إذا(!پایله)
        پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY, "\n", 1);
      إذا(پایله)
        اذهب quit;
    }

    إذا(ber) {
      ber_free(ber, 0);
      ber = عدم;
    }
  }

quit:
  إذا(ber)
    ber_free(ber, 0);
  إذا(ldapmsg) {
    ldap_msgfree(ldapmsg);
    LDAP_TRACE(("Received %d entries\n", عدد));
  }
  إذا(rc == LDAP_SIZELIMIT_EXCEEDED)
    infof(معلومات, "There are more than %d entries", عدد);
  إذا(ludp)
    ldap_free_urldesc(ludp);
  إذا(server)
    ldap_unbind_s(server);

  FREE_ON_WINLDAP(host);

  /* no data to transfer */
  Curl_xfer_تیاری_nop(معلومات);
  connclose(conn);

  أرجع پایله;
}

#که_تعریف_شوی DEBUG_LDAP
ساكن فراغ ldap_trace_ټیټ(ثابت حرف *fmt, ...)
{
  ساكن صحيح do_trace = -1;
  va_list دلیلونه;

  إذا(do_trace == -1) {
    ثابت حرف *env = getenv("CURL_TRACE");
    curl_off_t e = 0;
    إذا(!curlx_str_شمېره(&env, &e, لوی_عدد))
      do_trace = e > 0;
  }
  إذا(!do_trace)
    أرجع;

  va_start(دلیلونه, fmt);
  curl_mvfprintf(stderr, fmt, دلیلونه);
  va_end(دلیلونه);
}
#د_که_پای /* DEBUG_LDAP */

#که_نه_تعریف_شوی HAVE_LDAP_URL_PARSE

/*
 * Return scope-value for a scope-string.
 */
ساكن صحيح str2scope(ثابت حرف *p)
{
  إذا(curl_strequal(p, "one"))
    أرجع LDAP_SCOPE_ONELEVEL;
  إذا(curl_strequal(p, "onetree"))
    أرجع LDAP_SCOPE_ONELEVEL;
  إذا(curl_strequal(p, "base"))
    أرجع LDAP_SCOPE_بنسټ;
  إذا(curl_strequal(p, "sub"))
    أرجع LDAP_SCOPE_SUBTREE;
  إذا(curl_strequal(p, "subtree"))
    أرجع LDAP_SCOPE_SUBTREE;
  أرجع -1;
}

/* number of entries in the attributes list */
ساكن size_t عدد_entries(ثابت حرف *s)
{
  size_t توکي = 1;

  s = strchr(s, ',');
  طالما(s) {
    توکي++;
    s = strchr(s + 1, ',');
  }
  أرجع توکي;
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
ساكن curl_ldap_عدد_t ldap_url_parse2_ټیټ(بنية Curl_easy *معلومات,
                                           ثابت بنية connectdata *conn,
                                           LDAPURLDesc *ludp)
{
  curl_ldap_عدد_t rc = LDAP_SUCCESS;
  حرف *p;
  حرف *لار;
  حرف *q = عدم;
  حرف *query = عدم;
  size_t i;

  إذا(!معلومات ||
     !معلومات->state.پورته.لار ||
     معلومات->state.پورته.لار[0] != '/' ||
     ((معلومات->state.origin->scheme != &Curl_scheme_ldap) &&
      (معلومات->state.origin->scheme != &Curl_scheme_ldaps)))
    أرجع LDAP_INVALID_SYNTAX;

  ludp->lud_scope = LDAP_SCOPE_بنسټ;
  ludp->lud_پورټ  = conn->origin->پورټ;
  ludp->lud_host  = conn->origin->hostname;

  /* Duplicate the path */
  p = لار = curlx_strdup(معلومات->state.پورته.لار + 1);
  إذا(!لار)
    أرجع LDAP_NO_حافظه;

  /* Duplicate the query if present */
  إذا(معلومات->state.پورته.query) {
    q = query = curlx_strdup(معلومات->state.پورته.query);
    إذا(!query) {
      curlx_free(لار);
      أرجع LDAP_NO_حافظه;
    }
  }

  /* Parse the DN (Distinguished Name) */
  إذا(*p) {
    حرف *dn = p;
    حرف *unescaped;
    CURLcode پایله;

    LDAP_TRACE(("DN '%s'\n", dn));

    /* Unescape the DN */
    پایله = Curl_urldecode(dn, 0, &unescaped, عدم, REJECT_ZERO);
    إذا(پایله) {
      rc = LDAP_NO_حافظه;

      اذهب quit;
    }

#که_تعریف_شوی USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_dn = curlx_convert_UTF8_منزل_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    إذا(!ludp->lud_dn) {
      rc = LDAP_NO_حافظه;

      اذهب quit;
    }
#وإلا
    ludp->lud_dn = unescaped;
#د_که_پای
  }

  p = q;
  إذا(!p)
    اذهب quit;

  /* Parse the attributes. skip "??" */
  q = strchr(p, '?');
  إذا(q)
    *q++ = '\0';

  إذا(*p) {
    size_t شمېر = عدد_entries(p); /* at least one */
    ثابت حرف *atp = p;

    /* Allocate our array (+1 for the NULL entry) */
#که_تعریف_شوی USE_WIN32_LDAP
    ludp->lud_attrs = curlx_calloc(شمېر + 1, حجم(TCHAR *));
#وإلا
    ludp->lud_attrs = curlx_calloc(شمېر + 1, حجم(حرف *));
#د_که_پای
    إذا(!ludp->lud_attrs) {
      rc = LDAP_NO_حافظه;
      اذهب quit;
    }

    لكل(i = 0; i < شمېر; i++) {
      حرف *unescaped;
      CURLcode پایله;
      بنية Curl_str بهر;

      إذا(curlx_str_until(&atp, &بهر, 1024, ','))
        اكسر;

      LDAP_TRACE(("attr[%zu] '%.*s'\n", i, (صحيح)بهر.len, بهر.str));

      /* Unescape the attribute */
      پایله = Curl_urldecode(بهر.str, بهر.len, &unescaped, عدم, REJECT_ZERO);
      إذا(پایله) {
        rc = LDAP_NO_حافظه;
        اذهب quit;
      }

#که_تعریف_شوی USE_WIN32_LDAP
      /* Convert the unescaped string to a tchar */
      ludp->lud_attrs[i] = curlx_convert_UTF8_منزل_tchar(unescaped);

      /* Free the unescaped string as we are done with it */
      curlx_free(unescaped);

      إذا(!ludp->lud_attrs[i]) {
        rc = LDAP_NO_حافظه;
        اذهب quit;
      }
#وإلا
      ludp->lud_attrs[i] = unescaped;
#د_که_پای

      ludp->lud_attrs_dups++;
      إذا(curlx_str_single(&atp, ','))
        اكسر;
    }
  }

  p = q;
  إذا(!p)
    اذهب quit;

  /* Parse the scope. skip "??" */
  q = strchr(p, '?');
  إذا(q)
    *q++ = '\0';

  إذا(*p) {
    ludp->lud_scope = str2scope(p);
    إذا(ludp->lud_scope == -1) {
      rc = LDAP_INVALID_SYNTAX;

      اذهب quit;
    }
    LDAP_TRACE(("scope %d\n", ludp->lud_scope));
  }

  p = q;
  إذا(!p)
    اذهب quit;

  /* Parse the filter */
  q = strchr(p, '?');
  إذا(q)
    *q++ = '\0';

  إذا(*p) {
    حرف *filter = p;
    حرف *unescaped;
    CURLcode پایله;

    LDAP_TRACE(("filter '%s'\n", filter));

    /* Unescape the filter */
    پایله = Curl_urldecode(filter, 0, &unescaped, عدم, REJECT_CTRL);
    إذا(پایله) {
      rc = LDAP_NO_حافظه;

      اذهب quit;
    }

#که_تعریف_شوی USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_filter = curlx_convert_UTF8_منزل_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    إذا(!ludp->lud_filter) {
      rc = LDAP_NO_حافظه;

      اذهب quit;
    }
#وإلا
    ludp->lud_filter = unescaped;
#د_که_پای
  }

  p = q;
  إذا(p && !*p) {
    rc = LDAP_INVALID_SYNTAX;

    اذهب quit;
  }

quit:
  curlx_free(لار);
  curlx_free(query);

  أرجع rc;
}

ساكن curl_ldap_عدد_t ldap_url_parse_ټیټ(بنية Curl_easy *معلومات,
                                          ثابت بنية connectdata *conn,
                                          LDAPURLDesc **ludpp)
{
  LDAPURLDesc *ludp = curlx_calloc(1, حجم(*ludp));
  curl_ldap_عدد_t rc;

  *ludpp = عدم;
  إذا(!ludp)
    أرجع LDAP_NO_حافظه;

  rc = ldap_url_parse2_ټیټ(معلومات, conn, ludp);
  إذا(rc != LDAP_SUCCESS) {
    ldap_free_urldesc_ټیټ(ludp);
    ludp = عدم;
  }
  *ludpp = ludp;
  أرجع rc;
}

ساكن فراغ ldap_free_urldesc_ټیټ(LDAPURLDesc *ludp)
{
  إذا(!ludp)
    أرجع;

  curlx_free(ludp->lud_dn);
  curlx_free(ludp->lud_filter);

  إذا(ludp->lud_attrs) {
    size_t i;
    لكل(i = 0; i < ludp->lud_attrs_dups; i++) {
      curlx_free(ludp->lud_attrs[i]);
    }
    curlx_free(ludp->lud_attrs);
  }

  curlx_free(ludp);
}
#د_که_پای /* !HAVE_LDAP_URL_PARSE */

فراغ Curl_ldap_نسخه(حرف *buf, size_t bufsz)
{
#که_تعریف_شوی USE_WIN32_LDAP
  curl_msnprintf(buf, bufsz, "WinLDAP");
#وإلا
#که_تعریف_شوی LDAP_OPT_X_TLS_PASSPHRASE
  ساكن ثابت حرف flavor[] = "/Apple";
#وإلا
  ساكن ثابت حرف flavor[] = "";
#د_که_پای
  LDAPAPIInfo api;
  api.ldapai_info_نسخه = LDAP_API_INFO_نسخه;

  /* Comparing against 0, as different platforms
     disagree on the success define name */
  إذا(ldap_get_option(عدم, LDAP_OPT_API_INFO, &api) == 0) {
    بینښه صحيح patch = (بینښه صحيح)(api.ldapai_vendor_نسخه % 100);
    بینښه صحيح major = (بینښه صحيح)(api.ldapai_vendor_نسخه / 10000);
    بینښه صحيح minor =
      (((بینښه صحيح)api.ldapai_vendor_نسخه - (major * 10000))
       - patch) / 100;
#که_تعریف_شوی __OS400__
    curl_msnprintf(buf, bufsz, "IBMLDAP/%u.%u.%u", major, minor, patch);
    ldap_ارزښت_free(api.ldapai_extensions);
    (فراغ)flavor;
#وإلا
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u%s",
                   api.ldapai_vendor_name, major, minor, patch, flavor);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((فراغ **)api.ldapai_extensions);
#د_که_پای
  }
  وإلا
    curl_msnprintf(buf, bufsz, "LDAP/1");
#د_که_پای /* USE_WIN32_LDAP */
}

/*
 * LDAP protocol handler.
 */
ثابت بنية Curl_protocol Curl_protocol_ldap = {
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

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#لارښوونه GCC diagnostic راوباسه
#د_که_پای

#د_که_پای /* !CURL_DISABLE_LDAP && !USE_OPENLDAP */
