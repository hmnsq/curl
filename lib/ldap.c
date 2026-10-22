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
#شامل "curl_setup.h"
#شامل "urldata.h"
#شامل "curl_ldap.h"

#اگر !defined(CURL_DISABLE_LDAP) && !defined(USE_OPENLDAP)

#اگر defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wdeprecated-declarations"
#اگر_ختم

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

#اگر_تعریف_شدہ USE_WIN32_LDAP           /* Use Windows LDAP implementation. */
#  include <winldap.h>
/* Undefine indirect <wincrypt.h> symbols conflicting with AWS-LC/BoringSSL. */
#  undef X509_NAME
#  undef X509_EXTENSIONS
#  undef PKCS7_ISSUER_AND_SERIAL
#  undef PKCS7_SIGNER_INFO
#  undef OCSP_REQUEST
#  undef OCSP_RESPONSE
#  include <winber.h>
#ورنہ
#  define LDAP_DEPRECATED 1     /* Be sure ldap_init() is defined. */
#  ifdef HAVE_LBER_H
#    include <lber.h>
#  endif
#  include <ldap.h>
#  اگر defined(HAVE_LDAP_SSL) && defined(HAVE_LDAP_SSL_H)
#    include <ldap_ssl.h>
#  endif /* HAVE_LDAP_SSL && HAVE_LDAP_SSL_H */
#اگر_ختم

#شامل "cfilters.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "escape.h"
#شامل "progress.h"
#شامل "transfer.h"
#شامل "curlx/strparse.h"
#شامل "bufref.h"
#شامل "curlx/multibyte.h"
#شامل "curlx/base64.h"
#شامل "connect.h"

#اگر_تعریف_شدہ USE_WIN32_LDAP
#تعریف_کرو FREE_ON_WINLDAP(x) curlx_free(x)
#تعریف_کرو curl_ldap_عدد_t    ULONG
#ورنہ
#تعریف_کرو FREE_ON_WINLDAP(x) کرو {} جب(0)
#تعریف_کرو curl_ldap_عدد_t    صحیح
#اگر_ختم

#اگر_غیر_تعریف_شدہ HAVE_LDAP_URL_PARSE

/* Use our own implementation. */

ساخت ldap_urldesc {
  حرف   *lud_host;
  صحیح     lud_پورٹ;
#اگر_تعریف_شدہ USE_WIN32_LDAP
  TCHAR  *lud_dn;
  TCHAR **lud_attrs;
#ورنہ
  حرف   *lud_dn;
  حرف  **lud_attrs;
#اگر_ختم
  صحیح     lud_scope;
#اگر_تعریف_شدہ USE_WIN32_LDAP
  TCHAR  *lud_filter;
#ورنہ
  حرف   *lud_filter;
#اگر_ختم
  حرف  **lud_exts;
  size_t  lud_attrs_dups; /* how many were dup'ed, this field is not in the
                             "real" struct so can only be used in code
                             without HAVE_LDAP_URL_PARSE defined */
};

#تعریف_منسوخ LDAPURLDesc
#تعریف_کرو LDAPURLDesc ساخت ldap_urldesc

ساکن curl_ldap_عدد_t ldap_url_parse_کم(ساخت Curl_easy *ڈیٹا,
                                          ثابت ساخت connectdata *conn,
                                          LDAPURLDesc **ludpp);
ساکن خالی ldap_free_urldesc_کم(LDAPURLDesc *ludp);

#تعریف_منسوخ ldap_free_urldesc
#تعریف_کرو ldap_free_urldesc       ldap_free_urldesc_کم

#اگر_ختم /* !HAVE_LDAP_URL_PARSE */

#اگر_تعریف_شدہ DEBUG_LDAP
#تعریف_کرو LDAP_TRACE(x)                 \
  کرو {                                \
    ldap_trace_کم("%u: ", __LINE__); \
    ldap_trace_کم x;                 \
  } جب(0)

ساکن خالی ldap_trace_کم(ثابت حرف *fmt, ...) CURL_PRINTF(1, 2);
#ورنہ
#تعریف_کرو LDAP_TRACE(x) Curl_nop_stmt
#اگر_ختم

#اگر defined(USE_WIN32_LDAP) && defined(ldap_err2string)
/* Use ANSI error strings in Unicode builds */
#تعریف_منسوخ ldap_err2string
#تعریف_کرو ldap_err2string ldap_err2stringA
#اگر_ختم

#اگر defined(USE_WIN32_LDAP) && defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'void *' of greater size */
#تعریف_منسوخ LDAP_OPT_ON
#تعریف_منسوخ LDAP_OPT_OFF
#تعریف_کرو LDAP_OPT_ON   ((خالی *)(size_t)1)
#تعریف_کرو LDAP_OPT_OFF  ((خالی *)(size_t)0)
#اگر_ختم

#اگر_تعریف_شدہ USE_WIN32_LDAP

#اگر_تعریف_شدہ USE_WINDOWS_SSPI
ساکن ULONG ldap_win_bind_auth(LDAP *server, ثابت حرف *user,
                                ثابت حرف *passwd, غیرعلامتی طویل authflags)
{
  ULONG طریقہ = 0;
  SEC_WINNT_AUTH_شناختی_عنصر_EX cred;
  ULONG rc = LDAP_AUTH_طریقہ_NOT_SUPPORTED;

  memset(&cred, 0, حجم(cred));

#اگر_تعریف_شدہ USE_SPNEGO
  اگر(authflags & CURLAUTH_NEGOTIATE) {
    طریقہ = LDAP_AUTH_NEGOTIATE;
  }
  ورنہ
#اگر_ختم
#اگر_تعریف_شدہ USE_NTLM
  اگر(authflags & CURLAUTH_NTLM) {
    طریقہ = LDAP_AUTH_NTLM;
  }
  ورنہ
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
  اگر(authflags & CURLAUTH_DIGEST) {
    طریقہ = LDAP_AUTH_DIGEST;
  }
  ورنہ
#اگر_ختم
  {
    /* required anyway if one of upper preprocessor definitions enabled */
  }

  اگر(طریقہ && user && passwd) {
    CURLcode نتیجہ = Curl_create_sspi_شناختی_عنصر(user, passwd, &cred);
    اگر(!نتیجہ) {
      rc = ldap_bind_s(server, تہی, (TCHAR *)&cred, طریقہ);
      Curl_sspi_free_شناختی_عنصر(&cred);
    }
    ورنہ {
      rc = LDAP_NO_حافظہ;
    }
  }
  ورنہ {
    /* proceed with current user credentials */
    طریقہ = LDAP_AUTH_NEGOTIATE;
    rc = ldap_bind_s(server, تہی, تہی, طریقہ);
  }
  واپس rc;
}
#اگر_ختم /* USE_WINDOWS_SSPI */

ساکن ULONG ldap_win_bind(ساخت Curl_easy *ڈیٹا, LDAP *server,
                           ثابت حرف *user, ثابت حرف *passwd)
{
  ULONG rc = LDAP_INVALID_CREDENTIALS;

  PTCHAR inuser = تہی;
  PTCHAR inpass = تہی;

  اگر(user && passwd && (ڈیٹا->سیٹ.httpauth & CURLAUTH_BASIC)) {
    inuser = curlx_convert_UTF8_مقصد_tchar(user);
    inpass = curlx_convert_UTF8_مقصد_tchar(passwd);

    rc = ldap_simple_bind_s(server, inuser, inpass);

    curlx_free(inuser);
    curlx_free(inpass);
  }
#اگر_تعریف_شدہ USE_WINDOWS_SSPI
  ورنہ {
    rc = ldap_win_bind_auth(server, user, passwd, ڈیٹا->سیٹ.httpauth);
  }
#اگر_ختم

  واپس rc;
}
#اگر_ختم /* USE_WIN32_LDAP */

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
ساکن bool ldap_str_needs_base64(ثابت حرف *s, size_t len)
{
  size_t i;
  اگر(!len)
    واپس FALSE;
  اگر((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    واپس TRUE;
  برائے(i = 0; i < len; i++)
    اگر(!ISPRINT(s[i]))
      واپس TRUE;
  واپس FALSE;
}

ساکن bool ldap_قدر_needs_base64(ثابت حرف *attr, size_t attr_len,
                                    ثابت Ber_قدر *val)
{
  اگر((attr_len > 7) && curl_strequal(";binary", attr + attr_len - 7))
    واپس TRUE;

  واپس ldap_str_needs_base64(val->bv_val, val->bv_len);
}

#اگر_تعریف_شدہ USE_WIN32_LDAP
ساکن BOOLEAN bypass_cert_verify(PLDAP Connection,
                                  PCCERT_CONTEXT *ppServerCert)
{
  (خالی)Connection;
  CertFreeCertificateContext(*ppServerCert);
  /* approve any certificate since the verification is set to bypass */
  واپس TRUE;
}
#اگر_ختم

ساکن CURLcode show_vals(ساخت Curl_easy *ڈیٹا, Ber_قدر **vals,
                          ثابت حرف *attr)
{
  CURLcode نتیجہ = CURLE_OK;
  صحیح i;
  size_t attr_len = لڑی_لمبائی(attr);

  /* An attribute type has no base64 form in LDIF, so a type carrying control
     bytes (CR/LF etc.) would let a hostile server inject lines. A conformant
     type name never contains them, so reject it. */
  اگر(ldap_str_needs_base64(attr, attr_len)) {
    failf(ڈیٹا, "LDAP local: attribute type contains illegal bytes");
    واپس CURLE_WEIRD_SERVER_جواب;
  }

  برائے(i = 0; vals[i] && !نتیجہ; i++) {
    نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, "\t", 1);
    اگر(!نتیجہ)
      نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, attr, attr_len);
    اگر(!نتیجہ)
      نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, ":", 1);

    اگر(نتیجہ)
      توڑو;

    اگر(ldap_قدر_needs_base64(attr, attr_len, vals[i])) {
      حرف *val_b64 = تہی;
      size_t val_b64_sz = 0;

      /* Binary attribute, encode to base64. */
      اگر(vals[i]->bv_len)
        نتیجہ = curlx_base64_encode((uint8_t *)vals[i]->bv_val,
                                     vals[i]->bv_len,
                                     &val_b64, &val_b64_sz);
      اگر(!نتیجہ)
        نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, ": ", 2);
      اگر(!نتیجہ && val_b64_sz)
        نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, val_b64,
                                   val_b64_sz);
      curlx_free(val_b64);
    }
    ورنہ {
      نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, " ", 1);
      اگر(!نتیجہ)
        نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY,
                                   vals[i]->bv_val, vals[i]->bv_len);
    }

    اگر(!نتیجہ)
      نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, "\n", 1);
  }
  واپس نتیجہ;
}

ساکن CURLcode ldap_do(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  curl_ldap_عدد_t rc = LDAP_SUCCESS;
  LDAP *server = تہی;
  LDAPURLDesc *ludp = تہی;
  LDAPMessage *ldapmsg = تہی;
  LDAPMessage *اندراج_Iterator;
  صحیح عدد = 0;
  ساخت connectdata *conn = ڈیٹا->conn;
  صحیح ldap_proto = LDAP_VERSION3;
  bool ldap_ssl = FALSE;
#اگر_تعریف_شدہ LDAP_OPT_جال_TIMEOUT
  ساخت timeval ldap_timeout = { 10, 0 }; /* 10s connection/search timeout */
#اگر_ختم
#اگر_تعریف_شدہ USE_WIN32_LDAP
  TCHAR *host = تہی;
#ورنہ
  حرف *host = تہی;
#اگر_ختم
  ثابت حرف *user = ڈیٹا->state.creds ? ڈیٹا->state.creds->user : تہی;
  ثابت حرف *passwd = ڈیٹا->state.creds ? ڈیٹا->state.creds->passwd : تہی;
  ساخت ip_quadruple ipquad;
  bool is_ipv6;
  BerElement *ber = تہی;

  *مکمل = TRUE; /* unconditionally */
  infof(ڈیٹا, "LDAP local: LDAP Vendor = %s ; LDAP Version = %d",
        LDAP_VENDOR_NAME, LDAP_VENDOR_نسخہ);
  infof(ڈیٹا, "LDAP local: %s", Curl_bufref_ptr(&ڈیٹا->state.url));

#اگر_تعریف_شدہ HAVE_LDAP_URL_PARSE
  rc = ldap_url_parse(Curl_bufref_ptr(&ڈیٹا->state.url), &ludp);
#ورنہ
  rc = ldap_url_parse_کم(ڈیٹا, conn, &ludp);
#اگر_ختم
  اگر(rc) {
    failf(ڈیٹا, "Bad LDAP URL: %s", ldap_err2string(rc));
    نتیجہ = CURLE_URL_MALFORMAT;
    جاؤ quit;
  }

  نتیجہ = Curl_conn_get_ip_info(ڈیٹا, conn, FIRSTSOCKET, &is_ipv6, &ipquad);
  اگر(نتیجہ)
    جاؤ quit;

  /* Get the URL scheme (either ldap or ldaps) */
  ldap_ssl = Curl_conn_is_ssl(conn, FIRSTSOCKET);
  infof(ڈیٹا, "LDAP local: trying to establish %s connection",
        ldap_ssl ? "encrypted" : "cleartext");

#اگر_تعریف_شدہ USE_WIN32_LDAP
  host = curlx_convert_UTF8_مقصد_tchar(conn->origin->hostname);
  اگر(!host) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;

    جاؤ quit;
  }
#ورنہ
  host = conn->origin->hostname;
#اگر_ختم

#اگر_تعریف_شدہ USE_WIN32_LDAP
  اگر(ldap_ssl)
    server = ldap_sslinit(host, (curl_ldap_عدد_t)ipquad.remote_پورٹ, 1);
  ورنہ
#اگر_ختم
    server = ldap_init(host, (curl_ldap_عدد_t)ipquad.remote_پورٹ);
  اگر(!server) {
    failf(ڈیٹا, "LDAP: cannot setup connect to %s:%u",
          conn->origin->user_hostname, ipquad.remote_پورٹ);
    نتیجہ = CURLE_COULDNT_CONNECT;
    جاؤ quit;
  }

#اگر_تعریف_شدہ LDAP_OPT_جال_TIMEOUT
  ldap_سیٹ_option(server, LDAP_OPT_جال_TIMEOUT, &ldap_timeout);
#اگر_ختم
  ldap_سیٹ_option(server, LDAP_OPT_PROTOCOL_نسخہ, &ldap_proto);

  /* Do not chase referrals. */
  ldap_سیٹ_option(server, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  اگر(ldap_ssl) {
#اگر_تعریف_شدہ HAVE_LDAP_SSL
#اگر_تعریف_شدہ USE_WIN32_LDAP
    /* Win32 LDAP uses the Windows CA store to verify certificates */
    ldap_سیٹ_option(server, LDAP_OPT_SSL, LDAP_OPT_ON);
    اگر(!conn->ssl_config.verifypeer) {
      اگر(conn->ssl_config.verifyhost) {
        failf(ڈیٹا, "LDAP local: host verification cannot be enabled when "
              "peer verification is disabled for Windows native LDAP");
        نتیجہ = CURLE_NOT_BUILT_اندر;
        جاؤ quit;
      }
      ldap_سیٹ_option(server, LDAP_OPT_SERVER_CERTIFICATE,
                      (خالی *)(uintptr_t)bypass_cert_verify);
    }
#وگرنہ_اگر defined(LDAP_OPT_X_TLS) /* !USE_WIN32_LDAP */
    صحیح ldap_option;
    ثابت حرف *ldap_ca = conn->ssl_config.CAfile;
    اگر(conn->ssl_config.verifypeer) {
      /* OpenLDAP SDK supports BASE64 files. */
      اگر(conn->ssl_config.cert_type &&
         !curl_strequal(conn->ssl_config.cert_type, "PEM")) {
        failf(ڈیٹا, "LDAP local: ERROR OpenLDAP only supports PEM cert-type");
        نتیجہ = CURLE_SSL_CERTPROBLEM;
        جاؤ quit;
      }
      اگر(!ldap_ca) {
        failf(ڈیٹا, "LDAP local: ERROR PEM CA cert not set");
        نتیجہ = CURLE_SSL_CERTPROBLEM;
        جاؤ quit;
      }
      infof(ڈیٹا, "LDAP local: using PEM CA cert: %s", ldap_ca);
      rc = ldap_سیٹ_option(server, LDAP_OPT_X_TLS_CACERTFILE, ldap_ca);
      اگر(rc != LDAP_SUCCESS) {
        failf(ڈیٹا, "LDAP local: ERROR setting PEM CA cert: %s",
              ldap_err2string(rc));
        نتیجہ = CURLE_SSL_CERTPROBLEM;
        جاؤ quit;
      }
      ldap_option = LDAP_OPT_X_TLS_DEMAND;
    }
    ورنہ
      ldap_option = LDAP_OPT_X_TLS_NEVER;

    rc = ldap_سیٹ_option(server, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_option);
    اگر(rc != LDAP_SUCCESS) {
      failf(ڈیٹا, "LDAP local: ERROR setting cert verify mode: %s",
            ldap_err2string(rc));
      نتیجہ = CURLE_SSL_CERTPROBLEM;
      جاؤ quit;
    }
    ldap_option = LDAP_OPT_X_TLS_HARD;
    rc = ldap_سیٹ_option(server, LDAP_OPT_X_TLS, &ldap_option);
    اگر(rc != LDAP_SUCCESS) {
      failf(ڈیٹا, "LDAP local: ERROR setting SSL/TLS mode: %s",
            ldap_err2string(rc));
      نتیجہ = CURLE_SSL_CERTPROBLEM;
      جاؤ quit;
    }
#ورنہ /* !USE_WIN32_LDAP && !LDAP_OPT_X_TLS */
    /* we should probably never come up to here since configure
       should check in first place if we can support LDAP SSL/TLS */
    failf(ڈیٹا, "LDAP local: SSL/TLS not supported with this version "
          "of the OpenLDAP toolkit");
    نتیجہ = CURLE_SSL_CERTPROBLEM;
    جاؤ quit;
#اگر_ختم
#اگر_ختم /* HAVE_LDAP_SSL */
  }
  ورنہ اگر(ڈیٹا->سیٹ.use_ssl > CURLUSESSL_TRY) {
    failf(ڈیٹا, "LDAP local: explicit TLS not supported");
    نتیجہ = CURLE_NOT_BUILT_اندر;
    جاؤ quit;
  }

#اگر_تعریف_شدہ USE_WIN32_LDAP
  rc = ldap_win_bind(ڈیٹا, server, user, passwd);
#ورنہ
  rc = ldap_simple_bind_s(server, user, passwd);
#اگر_ختم
  اگر(!ldap_ssl && rc != LDAP_SUCCESS) {
    ldap_proto = LDAP_VERSION2;
    ldap_سیٹ_option(server, LDAP_OPT_PROTOCOL_نسخہ, &ldap_proto);
#اگر_تعریف_شدہ USE_WIN32_LDAP
    rc = ldap_win_bind(ڈیٹا, server, user, passwd);
#ورنہ
    rc = ldap_simple_bind_s(server, user, passwd);
#اگر_ختم
  }
  اگر(rc != LDAP_SUCCESS) {
#اگر_تعریف_شدہ USE_WIN32_LDAP
    failf(ڈیٹا, "LDAP local: bind via ldap_win_bind %s",
          ldap_err2string(rc));
#ورنہ
    failf(ڈیٹا, "LDAP local: bind via ldap_simple_bind_s %s",
          ldap_err2string(rc));
#اگر_ختم
    نتیجہ = CURLE_LDAP_CANNOT_BIND;
    جاؤ quit;
  }

  Curl_pgrsReset(ڈیٹا);
  rc = ldap_تلاش_s(server, ludp->lud_dn,
                     ludp->lud_scope,
                     ludp->lud_filter, ludp->lud_attrs, 0, &ldapmsg);

  اگر(rc != LDAP_SUCCESS && rc != LDAP_SIZELIMIT_EXCEEDED) {
    failf(ڈیٹا, "LDAP remote: %s", ldap_err2string(rc));
    نتیجہ = CURLE_LDAP_تلاش_FAILED;
    جاؤ quit;
  }

  عدد = 0;
  برائے(اندراج_Iterator = ldap_پہلا_اندراج(server, ldapmsg);
      اندراج_Iterator;
      اندراج_Iterator = ldap_اگلا_اندراج(server, اندراج_Iterator), عدد++) {
#اگر_تعریف_شدہ USE_WIN32_LDAP
    TCHAR *attribute;
#ورنہ
    حرف *attribute;
#اگر_ختم

    /* Get the DN and write it to the client */
    {
      حرف *name = تہی;
      size_t name_len = 0;
#اگر_تعریف_شدہ USE_WIN32_LDAP
      TCHAR *dn = ldap_get_dn(server, اندراج_Iterator);
      اگر(dn)
        name = curlx_convert_tchar_مقصد_UTF8(dn);
#ورنہ
      حرف *dn = name = ldap_get_dn(server, اندراج_Iterator);
#اگر_ختم
      اگر(!name)
        نتیجہ = dn ? CURLE_برآمد_کا_حافظہ : CURLE_FAILED_INIT;
      ورنہ {
        name_len = لڑی_لمبائی(name);
        اگر(ldap_str_needs_base64(name, name_len)) {
          حرف *dn_b64 = تہی;
          size_t dn_b64_sz = 0;

          /* A DN with e.g. CR/LF would inject LDIF lines, so encode it */
          نتیجہ = curlx_base64_encode((uint8_t *)name, name_len,
                                       &dn_b64, &dn_b64_sz);
          اگر(!نتیجہ)
            نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, "DN:: ", 5);
          اگر(!نتیجہ)
            نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, dn_b64,
                                       dn_b64_sz);
          curlx_free(dn_b64);
        }
        ورنہ {
          نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, "DN: ", 4);
          اگر(!نتیجہ)
            نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, name, name_len);
        }
        اگر(!نتیجہ)
          نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, "\n", 1);
      }
      FREE_ON_WINLDAP(name);
      ldap_memfree(dn);
      اگر(نتیجہ)
        جاؤ quit;
    }

    /* Get the attributes and write them to the client */
    برائے(attribute = ldap_پہلا_attribute(server, اندراج_Iterator, &ber);
        attribute;
        attribute = ldap_اگلا_attribute(server, اندراج_Iterator, ber)) {
      Ber_قدر **vals;
#اگر_تعریف_شدہ USE_WIN32_LDAP
      حرف *attr = curlx_convert_tchar_مقصد_UTF8(attribute);
      اگر(!attr) {
        ldap_memfree(attribute);
        نتیجہ = CURLE_برآمد_کا_حافظہ;
        جاؤ quit;
      }
#ورنہ
      حرف *attr = attribute;
#اگر_ختم
      vals = ldap_get_اقدار_len(server, اندراج_Iterator, attribute);
      اگر(vals) {
        نتیجہ = show_vals(ڈیٹا, vals, attr);
        /* Free memory used to store values */
        ldap_قدر_free_len(vals);
      }

      /* Free the attribute as we are done with it */
      FREE_ON_WINLDAP(attr);
      ldap_memfree(attribute);

      اگر(!نتیجہ)
        نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, "\n", 1);
      اگر(نتیجہ)
        جاؤ quit;
    }

    اگر(ber) {
      ber_free(ber, 0);
      ber = تہی;
    }
  }

quit:
  اگر(ber)
    ber_free(ber, 0);
  اگر(ldapmsg) {
    ldap_msgfree(ldapmsg);
    LDAP_TRACE(("Received %d entries\n", عدد));
  }
  اگر(rc == LDAP_SIZELIMIT_EXCEEDED)
    infof(ڈیٹا, "There are more than %d entries", عدد);
  اگر(ludp)
    ldap_free_urldesc(ludp);
  اگر(server)
    ldap_unbind_s(server);

  FREE_ON_WINLDAP(host);

  /* no data to transfer */
  Curl_xfer_تیاری_nop(ڈیٹا);
  connclose(conn);

  واپس نتیجہ;
}

#اگر_تعریف_شدہ DEBUG_LDAP
ساکن خالی ldap_trace_کم(ثابت حرف *fmt, ...)
{
  ساکن صحیح do_trace = -1;
  va_list دلائل;

  اگر(do_trace == -1) {
    ثابت حرف *env = getenv("CURL_TRACE");
    curl_off_t e = 0;
    اگر(!curlx_str_نمبر(&env, &e, زیادہ_سے_زیادہ_عدد))
      do_trace = e > 0;
  }
  اگر(!do_trace)
    واپس;

  va_start(دلائل, fmt);
  curl_mvfprintf(stderr, fmt, دلائل);
  va_end(دلائل);
}
#اگر_ختم /* DEBUG_LDAP */

#اگر_غیر_تعریف_شدہ HAVE_LDAP_URL_PARSE

/*
 * Return scope-value for a scope-string.
 */
ساکن صحیح str2scope(ثابت حرف *p)
{
  اگر(curl_strequal(p, "one"))
    واپس LDAP_SCOPE_ONELEVEL;
  اگر(curl_strequal(p, "onetree"))
    واپس LDAP_SCOPE_ONELEVEL;
  اگر(curl_strequal(p, "base"))
    واپس LDAP_SCOPE_بنیاد;
  اگر(curl_strequal(p, "sub"))
    واپس LDAP_SCOPE_SUBTREE;
  اگر(curl_strequal(p, "subtree"))
    واپس LDAP_SCOPE_SUBTREE;
  واپس -1;
}

/* number of entries in the attributes list */
ساکن size_t عدد_entries(ثابت حرف *s)
{
  size_t اشیاء = 1;

  s = strchr(s, ',');
  جب(s) {
    اشیاء++;
    s = strchr(s + 1, ',');
  }
  واپس اشیاء;
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
ساکن curl_ldap_عدد_t ldap_url_parse2_کم(ساخت Curl_easy *ڈیٹا,
                                           ثابت ساخت connectdata *conn,
                                           LDAPURLDesc *ludp)
{
  curl_ldap_عدد_t rc = LDAP_SUCCESS;
  حرف *p;
  حرف *راستہ;
  حرف *q = تہی;
  حرف *query = تہی;
  size_t i;

  اگر(!ڈیٹا ||
     !ڈیٹا->state.اوپر.راستہ ||
     ڈیٹا->state.اوپر.راستہ[0] != '/' ||
     ((ڈیٹا->state.origin->scheme != &Curl_scheme_ldap) &&
      (ڈیٹا->state.origin->scheme != &Curl_scheme_ldaps)))
    واپس LDAP_INVALID_SYNTAX;

  ludp->lud_scope = LDAP_SCOPE_بنیاد;
  ludp->lud_پورٹ  = conn->origin->پورٹ;
  ludp->lud_host  = conn->origin->hostname;

  /* Duplicate the path */
  p = راستہ = curlx_strdup(ڈیٹا->state.اوپر.راستہ + 1);
  اگر(!راستہ)
    واپس LDAP_NO_حافظہ;

  /* Duplicate the query if present */
  اگر(ڈیٹا->state.اوپر.query) {
    q = query = curlx_strdup(ڈیٹا->state.اوپر.query);
    اگر(!query) {
      curlx_free(راستہ);
      واپس LDAP_NO_حافظہ;
    }
  }

  /* Parse the DN (Distinguished Name) */
  اگر(*p) {
    حرف *dn = p;
    حرف *unescaped;
    CURLcode نتیجہ;

    LDAP_TRACE(("DN '%s'\n", dn));

    /* Unescape the DN */
    نتیجہ = Curl_urldecode(dn, 0, &unescaped, تہی, REJECT_ZERO);
    اگر(نتیجہ) {
      rc = LDAP_NO_حافظہ;

      جاؤ quit;
    }

#اگر_تعریف_شدہ USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_dn = curlx_convert_UTF8_مقصد_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    اگر(!ludp->lud_dn) {
      rc = LDAP_NO_حافظہ;

      جاؤ quit;
    }
#ورنہ
    ludp->lud_dn = unescaped;
#اگر_ختم
  }

  p = q;
  اگر(!p)
    جاؤ quit;

  /* Parse the attributes. skip "??" */
  q = strchr(p, '?');
  اگر(q)
    *q++ = '\0';

  اگر(*p) {
    size_t گنتی = عدد_entries(p); /* at least one */
    ثابت حرف *atp = p;

    /* Allocate our array (+1 for the NULL entry) */
#اگر_تعریف_شدہ USE_WIN32_LDAP
    ludp->lud_attrs = curlx_calloc(گنتی + 1, حجم(TCHAR *));
#ورنہ
    ludp->lud_attrs = curlx_calloc(گنتی + 1, حجم(حرف *));
#اگر_ختم
    اگر(!ludp->lud_attrs) {
      rc = LDAP_NO_حافظہ;
      جاؤ quit;
    }

    برائے(i = 0; i < گنتی; i++) {
      حرف *unescaped;
      CURLcode نتیجہ;
      ساخت Curl_str برآمد;

      اگر(curlx_str_until(&atp, &برآمد, 1024, ','))
        توڑو;

      LDAP_TRACE(("attr[%zu] '%.*s'\n", i, (صحیح)برآمد.len, برآمد.str));

      /* Unescape the attribute */
      نتیجہ = Curl_urldecode(برآمد.str, برآمد.len, &unescaped, تہی, REJECT_ZERO);
      اگر(نتیجہ) {
        rc = LDAP_NO_حافظہ;
        جاؤ quit;
      }

#اگر_تعریف_شدہ USE_WIN32_LDAP
      /* Convert the unescaped string to a tchar */
      ludp->lud_attrs[i] = curlx_convert_UTF8_مقصد_tchar(unescaped);

      /* Free the unescaped string as we are done with it */
      curlx_free(unescaped);

      اگر(!ludp->lud_attrs[i]) {
        rc = LDAP_NO_حافظہ;
        جاؤ quit;
      }
#ورنہ
      ludp->lud_attrs[i] = unescaped;
#اگر_ختم

      ludp->lud_attrs_dups++;
      اگر(curlx_str_single(&atp, ','))
        توڑو;
    }
  }

  p = q;
  اگر(!p)
    جاؤ quit;

  /* Parse the scope. skip "??" */
  q = strchr(p, '?');
  اگر(q)
    *q++ = '\0';

  اگر(*p) {
    ludp->lud_scope = str2scope(p);
    اگر(ludp->lud_scope == -1) {
      rc = LDAP_INVALID_SYNTAX;

      جاؤ quit;
    }
    LDAP_TRACE(("scope %d\n", ludp->lud_scope));
  }

  p = q;
  اگر(!p)
    جاؤ quit;

  /* Parse the filter */
  q = strchr(p, '?');
  اگر(q)
    *q++ = '\0';

  اگر(*p) {
    حرف *filter = p;
    حرف *unescaped;
    CURLcode نتیجہ;

    LDAP_TRACE(("filter '%s'\n", filter));

    /* Unescape the filter */
    نتیجہ = Curl_urldecode(filter, 0, &unescaped, تہی, REJECT_CTRL);
    اگر(نتیجہ) {
      rc = LDAP_NO_حافظہ;

      جاؤ quit;
    }

#اگر_تعریف_شدہ USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_filter = curlx_convert_UTF8_مقصد_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    اگر(!ludp->lud_filter) {
      rc = LDAP_NO_حافظہ;

      جاؤ quit;
    }
#ورنہ
    ludp->lud_filter = unescaped;
#اگر_ختم
  }

  p = q;
  اگر(p && !*p) {
    rc = LDAP_INVALID_SYNTAX;

    جاؤ quit;
  }

quit:
  curlx_free(راستہ);
  curlx_free(query);

  واپس rc;
}

ساکن curl_ldap_عدد_t ldap_url_parse_کم(ساخت Curl_easy *ڈیٹا,
                                          ثابت ساخت connectdata *conn,
                                          LDAPURLDesc **ludpp)
{
  LDAPURLDesc *ludp = curlx_calloc(1, حجم(*ludp));
  curl_ldap_عدد_t rc;

  *ludpp = تہی;
  اگر(!ludp)
    واپس LDAP_NO_حافظہ;

  rc = ldap_url_parse2_کم(ڈیٹا, conn, ludp);
  اگر(rc != LDAP_SUCCESS) {
    ldap_free_urldesc_کم(ludp);
    ludp = تہی;
  }
  *ludpp = ludp;
  واپس rc;
}

ساکن خالی ldap_free_urldesc_کم(LDAPURLDesc *ludp)
{
  اگر(!ludp)
    واپس;

  curlx_free(ludp->lud_dn);
  curlx_free(ludp->lud_filter);

  اگر(ludp->lud_attrs) {
    size_t i;
    برائے(i = 0; i < ludp->lud_attrs_dups; i++) {
      curlx_free(ludp->lud_attrs[i]);
    }
    curlx_free(ludp->lud_attrs);
  }

  curlx_free(ludp);
}
#اگر_ختم /* !HAVE_LDAP_URL_PARSE */

خالی Curl_ldap_نسخہ(حرف *buf, size_t bufsz)
{
#اگر_تعریف_شدہ USE_WIN32_LDAP
  curl_msnprintf(buf, bufsz, "WinLDAP");
#ورنہ
#اگر_تعریف_شدہ LDAP_OPT_X_TLS_PASSPHRASE
  ساکن ثابت حرف flavor[] = "/Apple";
#ورنہ
  ساکن ثابت حرف flavor[] = "";
#اگر_ختم
  LDAPAPIInfo api;
  api.ldapai_info_نسخہ = LDAP_API_INFO_نسخہ;

  /* Comparing against 0, as different platforms
     disagree on the success define name */
  اگر(ldap_get_option(تہی, LDAP_OPT_API_INFO, &api) == 0) {
    غیرعلامتی صحیح patch = (غیرعلامتی صحیح)(api.ldapai_vendor_نسخہ % 100);
    غیرعلامتی صحیح major = (غیرعلامتی صحیح)(api.ldapai_vendor_نسخہ / 10000);
    غیرعلامتی صحیح minor =
      (((غیرعلامتی صحیح)api.ldapai_vendor_نسخہ - (major * 10000))
       - patch) / 100;
#اگر_تعریف_شدہ __OS400__
    curl_msnprintf(buf, bufsz, "IBMLDAP/%u.%u.%u", major, minor, patch);
    ldap_قدر_free(api.ldapai_extensions);
    (خالی)flavor;
#ورنہ
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u%s",
                   api.ldapai_vendor_name, major, minor, patch, flavor);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((خالی **)api.ldapai_extensions);
#اگر_ختم
  }
  ورنہ
    curl_msnprintf(buf, bufsz, "LDAP/1");
#اگر_ختم /* USE_WIN32_LDAP */
}

/*
 * LDAP protocol handler.
 */
ثابت ساخت Curl_protocol Curl_protocol_ldap = {
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

#اگر defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#ہدایت GCC diagnostic نکالو
#اگر_ختم

#اگر_ختم /* !CURL_DISABLE_LDAP && !USE_OPENLDAP */
