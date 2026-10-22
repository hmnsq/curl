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
#رهنمود GCC diagnostic افزودن
#رهنمود GCC diagnostic ignored "-Wdeprecated-declarations"
#پایان_اگر

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

#اگر_تعریف_شده USE_WIN32_LDAP           /* Use Windows LDAP implementation. */
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
#پایان_اگر

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

#اگر_تعریف_شده USE_WIN32_LDAP
#تعریف_کن FREE_ON_WINLDAP(x) curlx_free(x)
#تعریف_کن curl_ldap_رقم_t    ULONG
#وإلا
#تعریف_کن FREE_ON_WINLDAP(x) افعل {} طالما(0)
#تعریف_کن curl_ldap_رقم_t    صحيح
#پایان_اگر

#اگر_تعریف_نشده HAVE_LDAP_URL_PARSE

/* Use our own implementation. */

بنية ldap_urldesc {
  حرف   *lud_host;
  صحيح     lud_پورت;
#اگر_تعریف_شده USE_WIN32_LDAP
  TCHAR  *lud_dn;
  TCHAR **lud_attrs;
#وإلا
  حرف   *lud_dn;
  حرف  **lud_attrs;
#پایان_اگر
  صحيح     lud_scope;
#اگر_تعریف_شده USE_WIN32_LDAP
  TCHAR  *lud_filter;
#وإلا
  حرف   *lud_filter;
#پایان_اگر
  حرف  **lud_exts;
  size_t  lud_attrs_dups; /* how many were dup'ed, this field is not in the
                             "real" struct so can only be used in code
                             without HAVE_LDAP_URL_PARSE defined */
};

#لغو_تعریف LDAPURLDesc
#تعریف_کن LDAPURLDesc بنية ldap_urldesc

ساكن curl_ldap_رقم_t ldap_url_parse_کم(بنية Curl_easy *داده,
                                          ثابت بنية connectdata *conn,
                                          LDAPURLDesc **ludpp);
ساكن فراغ ldap_free_urldesc_کم(LDAPURLDesc *ludp);

#لغو_تعریف ldap_free_urldesc
#تعریف_کن ldap_free_urldesc       ldap_free_urldesc_کم

#پایان_اگر /* !HAVE_LDAP_URL_PARSE */

#اگر_تعریف_شده DEBUG_LDAP
#تعریف_کن LDAP_TRACE(x)                 \
  افعل {                                \
    ldap_trace_کم("%u: ", __LINE__); \
    ldap_trace_کم x;                 \
  } طالما(0)

ساكن فراغ ldap_trace_کم(ثابت حرف *fmt, ...) CURL_PRINTF(1, 2);
#وإلا
#تعریف_کن LDAP_TRACE(x) Curl_nop_stmt
#پایان_اگر

#إذا defined(USE_WIN32_LDAP) && defined(ldap_err2string)
/* Use ANSI error strings in Unicode builds */
#لغو_تعریف ldap_err2string
#تعریف_کن ldap_err2string ldap_err2stringA
#پایان_اگر

#إذا defined(USE_WIN32_LDAP) && defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'void *' of greater size */
#لغو_تعریف LDAP_OPT_ON
#لغو_تعریف LDAP_OPT_OFF
#تعریف_کن LDAP_OPT_ON   ((فراغ *)(size_t)1)
#تعریف_کن LDAP_OPT_OFF  ((فراغ *)(size_t)0)
#پایان_اگر

#اگر_تعریف_شده USE_WIN32_LDAP

#اگر_تعریف_شده USE_WINDOWS_SSPI
ساكن ULONG ldap_win_bind_auth(LDAP *server, ثابت حرف *user,
                                ثابت حرف *passwd, بیعلامت دراز authflags)
{
  ULONG روش = 0;
  SEC_WINNT_AUTH_عنصر_خنثی_EX cred;
  ULONG rc = LDAP_AUTH_روش_NOT_SUPPORTED;

  memset(&cred, 0, حجم(cred));

#اگر_تعریف_شده USE_SPNEGO
  إذا(authflags & CURLAUTH_NEGOTIATE) {
    روش = LDAP_AUTH_NEGOTIATE;
  }
  وإلا
#پایان_اگر
#اگر_تعریف_شده USE_NTLM
  إذا(authflags & CURLAUTH_NTLM) {
    روش = LDAP_AUTH_NTLM;
  }
  وإلا
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_DIGEST_AUTH
  إذا(authflags & CURLAUTH_DIGEST) {
    روش = LDAP_AUTH_DIGEST;
  }
  وإلا
#پایان_اگر
  {
    /* required anyway if one of upper preprocessor definitions enabled */
  }

  إذا(روش && user && passwd) {
    CURLcode نتیجه = Curl_create_sspi_عنصر_خنثی(user, passwd, &cred);
    إذا(!نتیجه) {
      rc = ldap_bind_s(server, عدم, (TCHAR *)&cred, روش);
      Curl_sspi_free_عنصر_خنثی(&cred);
    }
    وإلا {
      rc = LDAP_NO_حافظه;
    }
  }
  وإلا {
    /* proceed with current user credentials */
    روش = LDAP_AUTH_NEGOTIATE;
    rc = ldap_bind_s(server, عدم, عدم, روش);
  }
  أرجع rc;
}
#پایان_اگر /* USE_WINDOWS_SSPI */

ساكن ULONG ldap_win_bind(بنية Curl_easy *داده, LDAP *server,
                           ثابت حرف *user, ثابت حرف *passwd)
{
  ULONG rc = LDAP_INVALID_CREDENTIALS;

  PTCHAR inuser = عدم;
  PTCHAR inpass = عدم;

  إذا(user && passwd && (داده->مجموعه.httpauth & CURLAUTH_BASIC)) {
    inuser = curlx_convert_UTF8_مقصد_tchar(user);
    inpass = curlx_convert_UTF8_مقصد_tchar(passwd);

    rc = ldap_simple_bind_s(server, inuser, inpass);

    curlx_free(inuser);
    curlx_free(inpass);
  }
#اگر_تعریف_شده USE_WINDOWS_SSPI
  وإلا {
    rc = ldap_win_bind_auth(server, user, passwd, داده->مجموعه.httpauth);
  }
#پایان_اگر

  أرجع rc;
}
#پایان_اگر /* USE_WIN32_LDAP */

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

ساكن bool ldap_مقدار_needs_base64(ثابت حرف *attr, size_t attr_len,
                                    ثابت Ber_مقدار *val)
{
  إذا((attr_len > 7) && curl_strequal(";binary", attr + attr_len - 7))
    أرجع TRUE;

  أرجع ldap_str_needs_base64(val->bv_val, val->bv_len);
}

#اگر_تعریف_شده USE_WIN32_LDAP
ساكن BOOLEAN bypass_cert_verify(PLDAP Connection,
                                  PCCERT_CONTEXT *ppServerCert)
{
  (فراغ)Connection;
  CertFreeCertificateContext(*ppServerCert);
  /* approve any certificate since the verification is set to bypass */
  أرجع TRUE;
}
#پایان_اگر

ساكن CURLcode show_vals(بنية Curl_easy *داده, Ber_مقدار **vals,
                          ثابت حرف *attr)
{
  CURLcode نتیجه = CURLE_OK;
  صحيح i;
  size_t attr_len = طول_رشته(attr);

  /* An attribute type has no base64 form in LDIF, so a type carrying control
     bytes (CR/LF etc.) would let a hostile server inject lines. A conformant
     type name never contains them, so reject it. */
  إذا(ldap_str_needs_base64(attr, attr_len)) {
    failf(داده, "LDAP local: attribute type contains illegal bytes");
    أرجع CURLE_WEIRD_SERVER_پاسخ;
  }

  لكل(i = 0; vals[i] && !نتیجه; i++) {
    نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, "\t", 1);
    إذا(!نتیجه)
      نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, attr, attr_len);
    إذا(!نتیجه)
      نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, ":", 1);

    إذا(نتیجه)
      اكسر;

    إذا(ldap_مقدار_needs_base64(attr, attr_len, vals[i])) {
      حرف *val_b64 = عدم;
      size_t val_b64_sz = 0;

      /* Binary attribute, encode to base64. */
      إذا(vals[i]->bv_len)
        نتیجه = curlx_base64_encode((uint8_t *)vals[i]->bv_val,
                                     vals[i]->bv_len,
                                     &val_b64, &val_b64_sz);
      إذا(!نتیجه)
        نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, ": ", 2);
      إذا(!نتیجه && val_b64_sz)
        نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, val_b64,
                                   val_b64_sz);
      curlx_free(val_b64);
    }
    وإلا {
      نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, " ", 1);
      إذا(!نتیجه)
        نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY,
                                   vals[i]->bv_val, vals[i]->bv_len);
    }

    إذا(!نتیجه)
      نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, "\n", 1);
  }
  أرجع نتیجه;
}

ساكن CURLcode ldap_do(بنية Curl_easy *داده, bool *انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  curl_ldap_رقم_t rc = LDAP_SUCCESS;
  LDAP *server = عدم;
  LDAPURLDesc *ludp = عدم;
  LDAPMessage *ldapmsg = عدم;
  LDAPMessage *مدخل_Iterator;
  صحيح رقم = 0;
  بنية connectdata *conn = داده->conn;
  صحيح ldap_proto = LDAP_VERSION3;
  bool ldap_ssl = FALSE;
#اگر_تعریف_شده LDAP_OPT_شبکه_TIMEOUT
  بنية timeval ldap_timeout = { 10, 0 }; /* 10s connection/search timeout */
#پایان_اگر
#اگر_تعریف_شده USE_WIN32_LDAP
  TCHAR *host = عدم;
#وإلا
  حرف *host = عدم;
#پایان_اگر
  ثابت حرف *user = داده->state.creds ? داده->state.creds->user : عدم;
  ثابت حرف *passwd = داده->state.creds ? داده->state.creds->passwd : عدم;
  بنية ip_quadruple ipquad;
  bool is_ipv6;
  BerElement *ber = عدم;

  *انجامشده = TRUE; /* unconditionally */
  infof(داده, "LDAP local: LDAP Vendor = %s ; LDAP Version = %d",
        LDAP_VENDOR_NAME, LDAP_VENDOR_نسخه);
  infof(داده, "LDAP local: %s", Curl_bufref_ptr(&داده->state.url));

#اگر_تعریف_شده HAVE_LDAP_URL_PARSE
  rc = ldap_url_parse(Curl_bufref_ptr(&داده->state.url), &ludp);
#وإلا
  rc = ldap_url_parse_کم(داده, conn, &ludp);
#پایان_اگر
  إذا(rc) {
    failf(داده, "Bad LDAP URL: %s", ldap_err2string(rc));
    نتیجه = CURLE_URL_MALFORMAT;
    اذهب quit;
  }

  نتیجه = Curl_conn_get_ip_info(داده, conn, FIRSTSOCKET, &is_ipv6, &ipquad);
  إذا(نتیجه)
    اذهب quit;

  /* Get the URL scheme (either ldap or ldaps) */
  ldap_ssl = Curl_conn_is_ssl(conn, FIRSTSOCKET);
  infof(داده, "LDAP local: trying to establish %s connection",
        ldap_ssl ? "encrypted" : "cleartext");

#اگر_تعریف_شده USE_WIN32_LDAP
  host = curlx_convert_UTF8_مقصد_tchar(conn->origin->hostname);
  إذا(!host) {
    نتیجه = CURLE_بیرون_از_حافظه;

    اذهب quit;
  }
#وإلا
  host = conn->origin->hostname;
#پایان_اگر

#اگر_تعریف_شده USE_WIN32_LDAP
  إذا(ldap_ssl)
    server = ldap_sslinit(host, (curl_ldap_رقم_t)ipquad.remote_پورت, 1);
  وإلا
#پایان_اگر
    server = ldap_init(host, (curl_ldap_رقم_t)ipquad.remote_پورت);
  إذا(!server) {
    failf(داده, "LDAP: cannot setup connect to %s:%u",
          conn->origin->user_hostname, ipquad.remote_پورت);
    نتیجه = CURLE_COULDNT_CONNECT;
    اذهب quit;
  }

#اگر_تعریف_شده LDAP_OPT_شبکه_TIMEOUT
  ldap_مجموعه_option(server, LDAP_OPT_شبکه_TIMEOUT, &ldap_timeout);
#پایان_اگر
  ldap_مجموعه_option(server, LDAP_OPT_PROTOCOL_نسخه, &ldap_proto);

  /* Do not chase referrals. */
  ldap_مجموعه_option(server, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  إذا(ldap_ssl) {
#اگر_تعریف_شده HAVE_LDAP_SSL
#اگر_تعریف_شده USE_WIN32_LDAP
    /* Win32 LDAP uses the Windows CA store to verify certificates */
    ldap_مجموعه_option(server, LDAP_OPT_SSL, LDAP_OPT_ON);
    إذا(!conn->ssl_config.verifypeer) {
      إذا(conn->ssl_config.verifyhost) {
        failf(داده, "LDAP local: host verification cannot be enabled when "
              "peer verification is disabled for Windows native LDAP");
        نتیجه = CURLE_NOT_BUILT_درون;
        اذهب quit;
      }
      ldap_مجموعه_option(server, LDAP_OPT_SERVER_CERTIFICATE,
                      (فراغ *)(uintptr_t)bypass_cert_verify);
    }
#وإلاإذا defined(LDAP_OPT_X_TLS) /* !USE_WIN32_LDAP */
    صحيح ldap_option;
    ثابت حرف *ldap_ca = conn->ssl_config.CAfile;
    إذا(conn->ssl_config.verifypeer) {
      /* OpenLDAP SDK supports BASE64 files. */
      إذا(conn->ssl_config.cert_type &&
         !curl_strequal(conn->ssl_config.cert_type, "PEM")) {
        failf(داده, "LDAP local: ERROR OpenLDAP only supports PEM cert-type");
        نتیجه = CURLE_SSL_CERTPROBLEM;
        اذهب quit;
      }
      إذا(!ldap_ca) {
        failf(داده, "LDAP local: ERROR PEM CA cert not set");
        نتیجه = CURLE_SSL_CERTPROBLEM;
        اذهب quit;
      }
      infof(داده, "LDAP local: using PEM CA cert: %s", ldap_ca);
      rc = ldap_مجموعه_option(server, LDAP_OPT_X_TLS_CACERTFILE, ldap_ca);
      إذا(rc != LDAP_SUCCESS) {
        failf(داده, "LDAP local: ERROR setting PEM CA cert: %s",
              ldap_err2string(rc));
        نتیجه = CURLE_SSL_CERTPROBLEM;
        اذهب quit;
      }
      ldap_option = LDAP_OPT_X_TLS_DEMAND;
    }
    وإلا
      ldap_option = LDAP_OPT_X_TLS_NEVER;

    rc = ldap_مجموعه_option(server, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_option);
    إذا(rc != LDAP_SUCCESS) {
      failf(داده, "LDAP local: ERROR setting cert verify mode: %s",
            ldap_err2string(rc));
      نتیجه = CURLE_SSL_CERTPROBLEM;
      اذهب quit;
    }
    ldap_option = LDAP_OPT_X_TLS_HARD;
    rc = ldap_مجموعه_option(server, LDAP_OPT_X_TLS, &ldap_option);
    إذا(rc != LDAP_SUCCESS) {
      failf(داده, "LDAP local: ERROR setting SSL/TLS mode: %s",
            ldap_err2string(rc));
      نتیجه = CURLE_SSL_CERTPROBLEM;
      اذهب quit;
    }
#وإلا /* !USE_WIN32_LDAP && !LDAP_OPT_X_TLS */
    /* we should probably never come up to here since configure
       should check in first place if we can support LDAP SSL/TLS */
    failf(داده, "LDAP local: SSL/TLS not supported with this version "
          "of the OpenLDAP toolkit");
    نتیجه = CURLE_SSL_CERTPROBLEM;
    اذهب quit;
#پایان_اگر
#پایان_اگر /* HAVE_LDAP_SSL */
  }
  وإلا إذا(داده->مجموعه.use_ssl > CURLUSESSL_TRY) {
    failf(داده, "LDAP local: explicit TLS not supported");
    نتیجه = CURLE_NOT_BUILT_درون;
    اذهب quit;
  }

#اگر_تعریف_شده USE_WIN32_LDAP
  rc = ldap_win_bind(داده, server, user, passwd);
#وإلا
  rc = ldap_simple_bind_s(server, user, passwd);
#پایان_اگر
  إذا(!ldap_ssl && rc != LDAP_SUCCESS) {
    ldap_proto = LDAP_VERSION2;
    ldap_مجموعه_option(server, LDAP_OPT_PROTOCOL_نسخه, &ldap_proto);
#اگر_تعریف_شده USE_WIN32_LDAP
    rc = ldap_win_bind(داده, server, user, passwd);
#وإلا
    rc = ldap_simple_bind_s(server, user, passwd);
#پایان_اگر
  }
  إذا(rc != LDAP_SUCCESS) {
#اگر_تعریف_شده USE_WIN32_LDAP
    failf(داده, "LDAP local: bind via ldap_win_bind %s",
          ldap_err2string(rc));
#وإلا
    failf(داده, "LDAP local: bind via ldap_simple_bind_s %s",
          ldap_err2string(rc));
#پایان_اگر
    نتیجه = CURLE_LDAP_CANNOT_BIND;
    اذهب quit;
  }

  Curl_pgrsReset(داده);
  rc = ldap_جستجو_s(server, ludp->lud_dn,
                     ludp->lud_scope,
                     ludp->lud_filter, ludp->lud_attrs, 0, &ldapmsg);

  إذا(rc != LDAP_SUCCESS && rc != LDAP_SIZELIMIT_EXCEEDED) {
    failf(داده, "LDAP remote: %s", ldap_err2string(rc));
    نتیجه = CURLE_LDAP_جستجو_FAILED;
    اذهب quit;
  }

  رقم = 0;
  لكل(مدخل_Iterator = ldap_اولی_مدخل(server, ldapmsg);
      مدخل_Iterator;
      مدخل_Iterator = ldap_بعدی_مدخل(server, مدخل_Iterator), رقم++) {
#اگر_تعریف_شده USE_WIN32_LDAP
    TCHAR *attribute;
#وإلا
    حرف *attribute;
#پایان_اگر

    /* Get the DN and write it to the client */
    {
      حرف *name = عدم;
      size_t name_len = 0;
#اگر_تعریف_شده USE_WIN32_LDAP
      TCHAR *dn = ldap_get_dn(server, مدخل_Iterator);
      إذا(dn)
        name = curlx_convert_tchar_مقصد_UTF8(dn);
#وإلا
      حرف *dn = name = ldap_get_dn(server, مدخل_Iterator);
#پایان_اگر
      إذا(!name)
        نتیجه = dn ? CURLE_بیرون_از_حافظه : CURLE_FAILED_INIT;
      وإلا {
        name_len = طول_رشته(name);
        إذا(ldap_str_needs_base64(name, name_len)) {
          حرف *dn_b64 = عدم;
          size_t dn_b64_sz = 0;

          /* A DN with e.g. CR/LF would inject LDIF lines, so encode it */
          نتیجه = curlx_base64_encode((uint8_t *)name, name_len,
                                       &dn_b64, &dn_b64_sz);
          إذا(!نتیجه)
            نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, "DN:: ", 5);
          إذا(!نتیجه)
            نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, dn_b64,
                                       dn_b64_sz);
          curlx_free(dn_b64);
        }
        وإلا {
          نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, "DN: ", 4);
          إذا(!نتیجه)
            نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, name, name_len);
        }
        إذا(!نتیجه)
          نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, "\n", 1);
      }
      FREE_ON_WINLDAP(name);
      ldap_memfree(dn);
      إذا(نتیجه)
        اذهب quit;
    }

    /* Get the attributes and write them to the client */
    لكل(attribute = ldap_اولی_attribute(server, مدخل_Iterator, &ber);
        attribute;
        attribute = ldap_بعدی_attribute(server, مدخل_Iterator, ber)) {
      Ber_مقدار **vals;
#اگر_تعریف_شده USE_WIN32_LDAP
      حرف *attr = curlx_convert_tchar_مقصد_UTF8(attribute);
      إذا(!attr) {
        ldap_memfree(attribute);
        نتیجه = CURLE_بیرون_از_حافظه;
        اذهب quit;
      }
#وإلا
      حرف *attr = attribute;
#پایان_اگر
      vals = ldap_get_مقادیر_len(server, مدخل_Iterator, attribute);
      إذا(vals) {
        نتیجه = show_vals(داده, vals, attr);
        /* Free memory used to store values */
        ldap_مقدار_free_len(vals);
      }

      /* Free the attribute as we are done with it */
      FREE_ON_WINLDAP(attr);
      ldap_memfree(attribute);

      إذا(!نتیجه)
        نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, "\n", 1);
      إذا(نتیجه)
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
    LDAP_TRACE(("Received %d entries\n", رقم));
  }
  إذا(rc == LDAP_SIZELIMIT_EXCEEDED)
    infof(داده, "There are more than %d entries", رقم);
  إذا(ludp)
    ldap_free_urldesc(ludp);
  إذا(server)
    ldap_unbind_s(server);

  FREE_ON_WINLDAP(host);

  /* no data to transfer */
  Curl_xfer_راهاندازی_nop(داده);
  connclose(conn);

  أرجع نتیجه;
}

#اگر_تعریف_شده DEBUG_LDAP
ساكن فراغ ldap_trace_کم(ثابت حرف *fmt, ...)
{
  ساكن صحيح do_trace = -1;
  va_list آرگومانها;

  إذا(do_trace == -1) {
    ثابت حرف *env = getenv("CURL_TRACE");
    curl_off_t e = 0;
    إذا(!curlx_str_عدد(&env, &e, بیشترین_صحیح))
      do_trace = e > 0;
  }
  إذا(!do_trace)
    أرجع;

  va_start(آرگومانها, fmt);
  curl_mvfprintf(stderr, fmt, آرگومانها);
  va_end(آرگومانها);
}
#پایان_اگر /* DEBUG_LDAP */

#اگر_تعریف_نشده HAVE_LDAP_URL_PARSE

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
    أرجع LDAP_SCOPE_پایه;
  إذا(curl_strequal(p, "sub"))
    أرجع LDAP_SCOPE_SUBTREE;
  إذا(curl_strequal(p, "subtree"))
    أرجع LDAP_SCOPE_SUBTREE;
  أرجع -1;
}

/* number of entries in the attributes list */
ساكن size_t رقم_entries(ثابت حرف *s)
{
  size_t اقلام = 1;

  s = strchr(s, ',');
  طالما(s) {
    اقلام++;
    s = strchr(s + 1, ',');
  }
  أرجع اقلام;
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
ساكن curl_ldap_رقم_t ldap_url_parse2_کم(بنية Curl_easy *داده,
                                           ثابت بنية connectdata *conn,
                                           LDAPURLDesc *ludp)
{
  curl_ldap_رقم_t rc = LDAP_SUCCESS;
  حرف *p;
  حرف *مسیر;
  حرف *q = عدم;
  حرف *query = عدم;
  size_t i;

  إذا(!داده ||
     !داده->state.به_بالا.مسیر ||
     داده->state.به_بالا.مسیر[0] != '/' ||
     ((داده->state.origin->scheme != &Curl_scheme_ldap) &&
      (داده->state.origin->scheme != &Curl_scheme_ldaps)))
    أرجع LDAP_INVALID_SYNTAX;

  ludp->lud_scope = LDAP_SCOPE_پایه;
  ludp->lud_پورت  = conn->origin->پورت;
  ludp->lud_host  = conn->origin->hostname;

  /* Duplicate the path */
  p = مسیر = curlx_strdup(داده->state.به_بالا.مسیر + 1);
  إذا(!مسیر)
    أرجع LDAP_NO_حافظه;

  /* Duplicate the query if present */
  إذا(داده->state.به_بالا.query) {
    q = query = curlx_strdup(داده->state.به_بالا.query);
    إذا(!query) {
      curlx_free(مسیر);
      أرجع LDAP_NO_حافظه;
    }
  }

  /* Parse the DN (Distinguished Name) */
  إذا(*p) {
    حرف *dn = p;
    حرف *unescaped;
    CURLcode نتیجه;

    LDAP_TRACE(("DN '%s'\n", dn));

    /* Unescape the DN */
    نتیجه = Curl_urldecode(dn, 0, &unescaped, عدم, REJECT_ZERO);
    إذا(نتیجه) {
      rc = LDAP_NO_حافظه;

      اذهب quit;
    }

#اگر_تعریف_شده USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_dn = curlx_convert_UTF8_مقصد_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    إذا(!ludp->lud_dn) {
      rc = LDAP_NO_حافظه;

      اذهب quit;
    }
#وإلا
    ludp->lud_dn = unescaped;
#پایان_اگر
  }

  p = q;
  إذا(!p)
    اذهب quit;

  /* Parse the attributes. skip "??" */
  q = strchr(p, '?');
  إذا(q)
    *q++ = '\0';

  إذا(*p) {
    size_t شمار = رقم_entries(p); /* at least one */
    ثابت حرف *atp = p;

    /* Allocate our array (+1 for the NULL entry) */
#اگر_تعریف_شده USE_WIN32_LDAP
    ludp->lud_attrs = curlx_calloc(شمار + 1, حجم(TCHAR *));
#وإلا
    ludp->lud_attrs = curlx_calloc(شمار + 1, حجم(حرف *));
#پایان_اگر
    إذا(!ludp->lud_attrs) {
      rc = LDAP_NO_حافظه;
      اذهب quit;
    }

    لكل(i = 0; i < شمار; i++) {
      حرف *unescaped;
      CURLcode نتیجه;
      بنية Curl_str بیرون;

      إذا(curlx_str_until(&atp, &بیرون, 1024, ','))
        اكسر;

      LDAP_TRACE(("attr[%zu] '%.*s'\n", i, (صحيح)بیرون.len, بیرون.str));

      /* Unescape the attribute */
      نتیجه = Curl_urldecode(بیرون.str, بیرون.len, &unescaped, عدم, REJECT_ZERO);
      إذا(نتیجه) {
        rc = LDAP_NO_حافظه;
        اذهب quit;
      }

#اگر_تعریف_شده USE_WIN32_LDAP
      /* Convert the unescaped string to a tchar */
      ludp->lud_attrs[i] = curlx_convert_UTF8_مقصد_tchar(unescaped);

      /* Free the unescaped string as we are done with it */
      curlx_free(unescaped);

      إذا(!ludp->lud_attrs[i]) {
        rc = LDAP_NO_حافظه;
        اذهب quit;
      }
#وإلا
      ludp->lud_attrs[i] = unescaped;
#پایان_اگر

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
    CURLcode نتیجه;

    LDAP_TRACE(("filter '%s'\n", filter));

    /* Unescape the filter */
    نتیجه = Curl_urldecode(filter, 0, &unescaped, عدم, REJECT_CTRL);
    إذا(نتیجه) {
      rc = LDAP_NO_حافظه;

      اذهب quit;
    }

#اگر_تعریف_شده USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_filter = curlx_convert_UTF8_مقصد_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    إذا(!ludp->lud_filter) {
      rc = LDAP_NO_حافظه;

      اذهب quit;
    }
#وإلا
    ludp->lud_filter = unescaped;
#پایان_اگر
  }

  p = q;
  إذا(p && !*p) {
    rc = LDAP_INVALID_SYNTAX;

    اذهب quit;
  }

quit:
  curlx_free(مسیر);
  curlx_free(query);

  أرجع rc;
}

ساكن curl_ldap_رقم_t ldap_url_parse_کم(بنية Curl_easy *داده,
                                          ثابت بنية connectdata *conn,
                                          LDAPURLDesc **ludpp)
{
  LDAPURLDesc *ludp = curlx_calloc(1, حجم(*ludp));
  curl_ldap_رقم_t rc;

  *ludpp = عدم;
  إذا(!ludp)
    أرجع LDAP_NO_حافظه;

  rc = ldap_url_parse2_کم(داده, conn, ludp);
  إذا(rc != LDAP_SUCCESS) {
    ldap_free_urldesc_کم(ludp);
    ludp = عدم;
  }
  *ludpp = ludp;
  أرجع rc;
}

ساكن فراغ ldap_free_urldesc_کم(LDAPURLDesc *ludp)
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
#پایان_اگر /* !HAVE_LDAP_URL_PARSE */

فراغ Curl_ldap_نسخه(حرف *buf, size_t bufsz)
{
#اگر_تعریف_شده USE_WIN32_LDAP
  curl_msnprintf(buf, bufsz, "WinLDAP");
#وإلا
#اگر_تعریف_شده LDAP_OPT_X_TLS_PASSPHRASE
  ساكن ثابت حرف flavor[] = "/Apple";
#وإلا
  ساكن ثابت حرف flavor[] = "";
#پایان_اگر
  LDAPAPIInfo api;
  api.ldapai_info_نسخه = LDAP_API_INFO_نسخه;

  /* Comparing against 0, as different platforms
     disagree on the success define name */
  إذا(ldap_get_option(عدم, LDAP_OPT_API_INFO, &api) == 0) {
    بیعلامت صحيح patch = (بیعلامت صحيح)(api.ldapai_vendor_نسخه % 100);
    بیعلامت صحيح major = (بیعلامت صحيح)(api.ldapai_vendor_نسخه / 10000);
    بیعلامت صحيح minor =
      (((بیعلامت صحيح)api.ldapai_vendor_نسخه - (major * 10000))
       - patch) / 100;
#اگر_تعریف_شده __OS400__
    curl_msnprintf(buf, bufsz, "IBMLDAP/%u.%u.%u", major, minor, patch);
    ldap_مقدار_free(api.ldapai_extensions);
    (فراغ)flavor;
#وإلا
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u%s",
                   api.ldapai_vendor_name, major, minor, patch, flavor);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((فراغ **)api.ldapai_extensions);
#پایان_اگر
  }
  وإلا
    curl_msnprintf(buf, bufsz, "LDAP/1");
#پایان_اگر /* USE_WIN32_LDAP */
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
#رهنمود GCC diagnostic برداشت
#پایان_اگر

#پایان_اگر /* !CURL_DISABLE_LDAP && !USE_OPENLDAP */
