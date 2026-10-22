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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_ldap.h"

#ਜੇ !defined(CURL_DISABLE_LDAP) && !defined(USE_OPENLDAP)

#ਜੇ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Wdeprecated-declarations"
#ਜੇ_ਅੰਤ

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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP           /* Use Windows LDAP implementation. */
#  include <winldap.h>
/* Undefine indirect <wincrypt.h> symbols conflicting with AWS-LC/BoringSSL. */
#  undef X509_NAME
#  undef X509_EXTENSIONS
#  undef PKCS7_ISSUER_AND_SERIAL
#  undef PKCS7_SIGNER_INFO
#  undef OCSP_REQUEST
#  undef OCSP_RESPONSE
#  include <winber.h>
#ਨਹੀਂਤਾਂ
#  define LDAP_DEPRECATED 1     /* Be sure ldap_init() is defined. */
#  ifdef HAVE_LBER_H
#    include <lber.h>
#  endif
#  include <ldap.h>
#  ਜੇ defined(HAVE_LDAP_SSL) && defined(HAVE_LDAP_SSL_H)
#    include <ldap_ssl.h>
#  endif /* HAVE_LDAP_SSL && HAVE_LDAP_SSL_H */
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/multibyte.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/base64.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FREE_ON_WINLDAP(x) curlx_free(x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curl_ldap_ਅੰਕ_t    ULONG
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FREE_ON_WINLDAP(x) ਕਰੋ {} ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curl_ldap_ਅੰਕ_t    ਪੂਰਨ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_LDAP_URL_PARSE

/* Use our own implementation. */

ਬਣਤਰ ldap_urldesc {
  ਅੱਖਰ   *lud_host;
  ਪੂਰਨ     lud_ਪੋਰਟ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP
  TCHAR  *lud_dn;
  TCHAR **lud_attrs;
#ਨਹੀਂਤਾਂ
  ਅੱਖਰ   *lud_dn;
  ਅੱਖਰ  **lud_attrs;
#ਜੇ_ਅੰਤ
  ਪੂਰਨ     lud_scope;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP
  TCHAR  *lud_filter;
#ਨਹੀਂਤਾਂ
  ਅੱਖਰ   *lud_filter;
#ਜੇ_ਅੰਤ
  ਅੱਖਰ  **lud_exts;
  size_t  lud_attrs_dups; /* how many were dup'ed, this field is not in the
                             "real" struct so can only be used in code
                             without HAVE_LDAP_URL_PARSE defined */
};

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ LDAPURLDesc
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LDAPURLDesc ਬਣਤਰ ldap_urldesc

ਸਥਿਰ curl_ldap_ਅੰਕ_t ldap_url_parse_ਨੀਵਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਅਚਲ ਬਣਤਰ connectdata *conn,
                                          LDAPURLDesc **ludpp);
ਸਥਿਰ ਖਾਲੀ ldap_free_urldesc_ਨੀਵਾਂ(LDAPURLDesc *ludp);

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ldap_free_urldesc
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ldap_free_urldesc       ldap_free_urldesc_ਨੀਵਾਂ

#ਜੇ_ਅੰਤ /* !HAVE_LDAP_URL_PARSE */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_LDAP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LDAP_TRACE(x)                 \
  ਕਰੋ {                                \
    ldap_trace_ਨੀਵਾਂ("%u: ", __LINE__); \
    ldap_trace_ਨੀਵਾਂ x;                 \
  } ਜਦੋਂ(0)

ਸਥਿਰ ਖਾਲੀ ldap_trace_ਨੀਵਾਂ(ਅਚਲ ਅੱਖਰ *fmt, ...) CURL_PRINTF(1, 2);
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LDAP_TRACE(x) Curl_nop_stmt
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_WIN32_LDAP) && defined(ldap_err2string)
/* Use ANSI error strings in Unicode builds */
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ldap_err2string
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ldap_err2string ldap_err2stringA
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_WIN32_LDAP) && defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'void *' of greater size */
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ LDAP_OPT_ON
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ LDAP_OPT_OFF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LDAP_OPT_ON   ((ਖਾਲੀ *)(size_t)1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LDAP_OPT_OFF  ((ਖਾਲੀ *)(size_t)0)
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
ਸਥਿਰ ULONG ldap_win_bind_auth(LDAP *server, ਅਚਲ ਅੱਖਰ *user,
                                ਅਚਲ ਅੱਖਰ *passwd, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ authflags)
{
  ULONG ਵਿਧੀ = 0;
  SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_EX cred;
  ULONG rc = LDAP_AUTH_ਵਿਧੀ_NOT_SUPPORTED;

  memset(&cred, 0, ਆਕਾਰ(cred));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SPNEGO
  ਜੇ(authflags & CURLAUTH_NEGOTIATE) {
    ਵਿਧੀ = LDAP_AUTH_NEGOTIATE;
  }
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NTLM
  ਜੇ(authflags & CURLAUTH_NTLM) {
    ਵਿਧੀ = LDAP_AUTH_NTLM;
  }
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
  ਜੇ(authflags & CURLAUTH_DIGEST) {
    ਵਿਧੀ = LDAP_AUTH_DIGEST;
  }
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
  {
    /* required anyway if one of upper preprocessor definitions enabled */
  }

  ਜੇ(ਵਿਧੀ && user && passwd) {
    CURLcode ਨਤੀਜਾ = Curl_create_sspi_ਪਛਾਣ_ਤੱਤ(user, passwd, &cred);
    ਜੇ(!ਨਤੀਜਾ) {
      rc = ldap_bind_s(server, ਨਿਰਾ, (TCHAR *)&cred, ਵਿਧੀ);
      Curl_sspi_free_ਪਛਾਣ_ਤੱਤ(&cred);
    }
    ਨਹੀਂਤਾਂ {
      rc = LDAP_NO_ਸਮ੍ਰਿਤੀ;
    }
  }
  ਨਹੀਂਤਾਂ {
    /* proceed with current user credentials */
    ਵਿਧੀ = LDAP_AUTH_NEGOTIATE;
    rc = ldap_bind_s(server, ਨਿਰਾ, ਨਿਰਾ, ਵਿਧੀ);
  }
  ਵਾਪਸ rc;
}
#ਜੇ_ਅੰਤ /* USE_WINDOWS_SSPI */

ਸਥਿਰ ULONG ldap_win_bind(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, LDAP *server,
                           ਅਚਲ ਅੱਖਰ *user, ਅਚਲ ਅੱਖਰ *passwd)
{
  ULONG rc = LDAP_INVALID_CREDENTIALS;

  PTCHAR inuser = ਨਿਰਾ;
  PTCHAR inpass = ਨਿਰਾ;

  ਜੇ(user && passwd && (ਅੰਕੜੇ->ਸੈੱਟ.httpauth & CURLAUTH_BASIC)) {
    inuser = curlx_convert_UTF8_ਮੰਜ਼ਿਲ_tchar(user);
    inpass = curlx_convert_UTF8_ਮੰਜ਼ਿਲ_tchar(passwd);

    rc = ldap_simple_bind_s(server, inuser, inpass);

    curlx_free(inuser);
    curlx_free(inpass);
  }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
  ਨਹੀਂਤਾਂ {
    rc = ldap_win_bind_auth(server, user, passwd, ਅੰਕੜੇ->ਸੈੱਟ.httpauth);
  }
#ਜੇ_ਅੰਤ

  ਵਾਪਸ rc;
}
#ਜੇ_ਅੰਤ /* USE_WIN32_LDAP */

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
ਸਥਿਰ bool ldap_str_needs_base64(ਅਚਲ ਅੱਖਰ *s, size_t len)
{
  size_t i;
  ਜੇ(!len)
    ਵਾਪਸ FALSE;
  ਜੇ((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    ਵਾਪਸ TRUE;
  ਲਈ(i = 0; i < len; i++)
    ਜੇ(!ISPRINT(s[i]))
      ਵਾਪਸ TRUE;
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ bool ldap_ਮੁੱਲ_needs_base64(ਅਚਲ ਅੱਖਰ *attr, size_t attr_len,
                                    ਅਚਲ Ber_ਮੁੱਲ *val)
{
  ਜੇ((attr_len > 7) && curl_strequal(";binary", attr + attr_len - 7))
    ਵਾਪਸ TRUE;

  ਵਾਪਸ ldap_str_needs_base64(val->bv_val, val->bv_len);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP
ਸਥਿਰ BOOLEAN bypass_cert_verify(PLDAP Connection,
                                  PCCERT_CONTEXT *ppServerCert)
{
  (ਖਾਲੀ)Connection;
  CertFreeCertificateContext(*ppServerCert);
  /* approve any certificate since the verification is set to bypass */
  ਵਾਪਸ TRUE;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode show_vals(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, Ber_ਮੁੱਲ **vals,
                          ਅਚਲ ਅੱਖਰ *attr)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ i;
  size_t attr_len = ਲੜੀ_ਲੰਬਾਈ(attr);

  /* An attribute type has no base64 form in LDIF, so a type carrying control
     bytes (CR/LF etc.) would let a hostile server inject lines. A conformant
     type name never contains them, so reject it. */
  ਜੇ(ldap_str_needs_base64(attr, attr_len)) {
    failf(ਅੰਕੜੇ, "LDAP local: attribute type contains illegal bytes");
    ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
  }

  ਲਈ(i = 0; vals[i] && !ਨਤੀਜਾ; i++) {
    ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, "\t", 1);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, attr, attr_len);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, ":", 1);

    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;

    ਜੇ(ldap_ਮੁੱਲ_needs_base64(attr, attr_len, vals[i])) {
      ਅੱਖਰ *val_b64 = ਨਿਰਾ;
      size_t val_b64_sz = 0;

      /* Binary attribute, encode to base64. */
      ਜੇ(vals[i]->bv_len)
        ਨਤੀਜਾ = curlx_base64_encode((uint8_t *)vals[i]->bv_val,
                                     vals[i]->bv_len,
                                     &val_b64, &val_b64_sz);
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, ": ", 2);
      ਜੇ(!ਨਤੀਜਾ && val_b64_sz)
        ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, val_b64,
                                   val_b64_sz);
      curlx_free(val_b64);
    }
    ਨਹੀਂਤਾਂ {
      ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, " ", 1);
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY,
                                   vals[i]->bv_val, vals[i]->bv_len);
    }

    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, "\n", 1);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ldap_do(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  curl_ldap_ਅੰਕ_t rc = LDAP_SUCCESS;
  LDAP *server = ਨਿਰਾ;
  LDAPURLDesc *ludp = ਨਿਰਾ;
  LDAPMessage *ldapmsg = ਨਿਰਾ;
  LDAPMessage *ਇੰਦਰਾਜ_Iterator;
  ਪੂਰਨ ਅੰਕ = 0;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਪੂਰਨ ldap_proto = LDAP_VERSION3;
  bool ldap_ssl = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LDAP_OPT_ਜਾਲ_TIMEOUT
  ਬਣਤਰ timeval ldap_timeout = { 10, 0 }; /* 10s connection/search timeout */
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP
  TCHAR *host = ਨਿਰਾ;
#ਨਹੀਂਤਾਂ
  ਅੱਖਰ *host = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
  ਅਚਲ ਅੱਖਰ *user = ਅੰਕੜੇ->state.creds ? ਅੰਕੜੇ->state.creds->user : ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *passwd = ਅੰਕੜੇ->state.creds ? ਅੰਕੜੇ->state.creds->passwd : ਨਿਰਾ;
  ਬਣਤਰ ip_quadruple ipquad;
  bool is_ipv6;
  BerElement *ber = ਨਿਰਾ;

  *ਪੂਰਾ = TRUE; /* unconditionally */
  infof(ਅੰਕੜੇ, "LDAP local: LDAP Vendor = %s ; LDAP Version = %d",
        LDAP_VENDOR_NAME, LDAP_VENDOR_ਸੰਸਕਰਣ);
  infof(ਅੰਕੜੇ, "LDAP local: %s", Curl_bufref_ptr(&ਅੰਕੜੇ->state.url));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LDAP_URL_PARSE
  rc = ldap_url_parse(Curl_bufref_ptr(&ਅੰਕੜੇ->state.url), &ludp);
#ਨਹੀਂਤਾਂ
  rc = ldap_url_parse_ਨੀਵਾਂ(ਅੰਕੜੇ, conn, &ludp);
#ਜੇ_ਅੰਤ
  ਜੇ(rc) {
    failf(ਅੰਕੜੇ, "Bad LDAP URL: %s", ldap_err2string(rc));
    ਨਤੀਜਾ = CURLE_URL_MALFORMAT;
    ਜਾਓ quit;
  }

  ਨਤੀਜਾ = Curl_conn_get_ip_info(ਅੰਕੜੇ, conn, FIRSTSOCKET, &is_ipv6, &ipquad);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ quit;

  /* Get the URL scheme (either ldap or ldaps) */
  ldap_ssl = Curl_conn_is_ssl(conn, FIRSTSOCKET);
  infof(ਅੰਕੜੇ, "LDAP local: trying to establish %s connection",
        ldap_ssl ? "encrypted" : "cleartext");

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP
  host = curlx_convert_UTF8_ਮੰਜ਼ਿਲ_tchar(conn->origin->hostname);
  ਜੇ(!host) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    ਜਾਓ quit;
  }
#ਨਹੀਂਤਾਂ
  host = conn->origin->hostname;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP
  ਜੇ(ldap_ssl)
    server = ldap_sslinit(host, (curl_ldap_ਅੰਕ_t)ipquad.remote_ਪੋਰਟ, 1);
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    server = ldap_init(host, (curl_ldap_ਅੰਕ_t)ipquad.remote_ਪੋਰਟ);
  ਜੇ(!server) {
    failf(ਅੰਕੜੇ, "LDAP: cannot setup connect to %s:%u",
          conn->origin->user_hostname, ipquad.remote_ਪੋਰਟ);
    ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
    ਜਾਓ quit;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LDAP_OPT_ਜਾਲ_TIMEOUT
  ldap_ਸੈੱਟ_option(server, LDAP_OPT_ਜਾਲ_TIMEOUT, &ldap_timeout);
#ਜੇ_ਅੰਤ
  ldap_ਸੈੱਟ_option(server, LDAP_OPT_PROTOCOL_ਸੰਸਕਰਣ, &ldap_proto);

  /* Do not chase referrals. */
  ldap_ਸੈੱਟ_option(server, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  ਜੇ(ldap_ssl) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LDAP_SSL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP
    /* Win32 LDAP uses the Windows CA store to verify certificates */
    ldap_ਸੈੱਟ_option(server, LDAP_OPT_SSL, LDAP_OPT_ON);
    ਜੇ(!conn->ssl_config.verifypeer) {
      ਜੇ(conn->ssl_config.verifyhost) {
        failf(ਅੰਕੜੇ, "LDAP local: host verification cannot be enabled when "
              "peer verification is disabled for Windows native LDAP");
        ਨਤੀਜਾ = CURLE_NOT_BUILT_ਵਿੱਚ;
        ਜਾਓ quit;
      }
      ldap_ਸੈੱਟ_option(server, LDAP_OPT_SERVER_CERTIFICATE,
                      (ਖਾਲੀ *)(uintptr_t)bypass_cert_verify);
    }
#ਜਾਂਜੇ defined(LDAP_OPT_X_TLS) /* !USE_WIN32_LDAP */
    ਪੂਰਨ ldap_option;
    ਅਚਲ ਅੱਖਰ *ldap_ca = conn->ssl_config.CAfile;
    ਜੇ(conn->ssl_config.verifypeer) {
      /* OpenLDAP SDK supports BASE64 files. */
      ਜੇ(conn->ssl_config.cert_type &&
         !curl_strequal(conn->ssl_config.cert_type, "PEM")) {
        failf(ਅੰਕੜੇ, "LDAP local: ERROR OpenLDAP only supports PEM cert-type");
        ਨਤੀਜਾ = CURLE_SSL_CERTPROBLEM;
        ਜਾਓ quit;
      }
      ਜੇ(!ldap_ca) {
        failf(ਅੰਕੜੇ, "LDAP local: ERROR PEM CA cert not set");
        ਨਤੀਜਾ = CURLE_SSL_CERTPROBLEM;
        ਜਾਓ quit;
      }
      infof(ਅੰਕੜੇ, "LDAP local: using PEM CA cert: %s", ldap_ca);
      rc = ldap_ਸੈੱਟ_option(server, LDAP_OPT_X_TLS_CACERTFILE, ldap_ca);
      ਜੇ(rc != LDAP_SUCCESS) {
        failf(ਅੰਕੜੇ, "LDAP local: ERROR setting PEM CA cert: %s",
              ldap_err2string(rc));
        ਨਤੀਜਾ = CURLE_SSL_CERTPROBLEM;
        ਜਾਓ quit;
      }
      ldap_option = LDAP_OPT_X_TLS_DEMAND;
    }
    ਨਹੀਂਤਾਂ
      ldap_option = LDAP_OPT_X_TLS_NEVER;

    rc = ldap_ਸੈੱਟ_option(server, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_option);
    ਜੇ(rc != LDAP_SUCCESS) {
      failf(ਅੰਕੜੇ, "LDAP local: ERROR setting cert verify mode: %s",
            ldap_err2string(rc));
      ਨਤੀਜਾ = CURLE_SSL_CERTPROBLEM;
      ਜਾਓ quit;
    }
    ldap_option = LDAP_OPT_X_TLS_HARD;
    rc = ldap_ਸੈੱਟ_option(server, LDAP_OPT_X_TLS, &ldap_option);
    ਜੇ(rc != LDAP_SUCCESS) {
      failf(ਅੰਕੜੇ, "LDAP local: ERROR setting SSL/TLS mode: %s",
            ldap_err2string(rc));
      ਨਤੀਜਾ = CURLE_SSL_CERTPROBLEM;
      ਜਾਓ quit;
    }
#ਨਹੀਂਤਾਂ /* !USE_WIN32_LDAP && !LDAP_OPT_X_TLS */
    /* we should probably never come up to here since configure
       should check in first place if we can support LDAP SSL/TLS */
    failf(ਅੰਕੜੇ, "LDAP local: SSL/TLS not supported with this version "
          "of the OpenLDAP toolkit");
    ਨਤੀਜਾ = CURLE_SSL_CERTPROBLEM;
    ਜਾਓ quit;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* HAVE_LDAP_SSL */
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.use_ssl > CURLUSESSL_TRY) {
    failf(ਅੰਕੜੇ, "LDAP local: explicit TLS not supported");
    ਨਤੀਜਾ = CURLE_NOT_BUILT_ਵਿੱਚ;
    ਜਾਓ quit;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP
  rc = ldap_win_bind(ਅੰਕੜੇ, server, user, passwd);
#ਨਹੀਂਤਾਂ
  rc = ldap_simple_bind_s(server, user, passwd);
#ਜੇ_ਅੰਤ
  ਜੇ(!ldap_ssl && rc != LDAP_SUCCESS) {
    ldap_proto = LDAP_VERSION2;
    ldap_ਸੈੱਟ_option(server, LDAP_OPT_PROTOCOL_ਸੰਸਕਰਣ, &ldap_proto);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP
    rc = ldap_win_bind(ਅੰਕੜੇ, server, user, passwd);
#ਨਹੀਂਤਾਂ
    rc = ldap_simple_bind_s(server, user, passwd);
#ਜੇ_ਅੰਤ
  }
  ਜੇ(rc != LDAP_SUCCESS) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP
    failf(ਅੰਕੜੇ, "LDAP local: bind via ldap_win_bind %s",
          ldap_err2string(rc));
#ਨਹੀਂਤਾਂ
    failf(ਅੰਕੜੇ, "LDAP local: bind via ldap_simple_bind_s %s",
          ldap_err2string(rc));
#ਜੇ_ਅੰਤ
    ਨਤੀਜਾ = CURLE_LDAP_CANNOT_BIND;
    ਜਾਓ quit;
  }

  Curl_pgrsReset(ਅੰਕੜੇ);
  rc = ldap_ਖੋਜ_s(server, ludp->lud_dn,
                     ludp->lud_scope,
                     ludp->lud_filter, ludp->lud_attrs, 0, &ldapmsg);

  ਜੇ(rc != LDAP_SUCCESS && rc != LDAP_SIZELIMIT_EXCEEDED) {
    failf(ਅੰਕੜੇ, "LDAP remote: %s", ldap_err2string(rc));
    ਨਤੀਜਾ = CURLE_LDAP_ਖੋਜ_FAILED;
    ਜਾਓ quit;
  }

  ਅੰਕ = 0;
  ਲਈ(ਇੰਦਰਾਜ_Iterator = ldap_ਪਹਿਲਾ_ਇੰਦਰਾਜ(server, ldapmsg);
      ਇੰਦਰਾਜ_Iterator;
      ਇੰਦਰਾਜ_Iterator = ldap_ਅਗਲਾ_ਇੰਦਰਾਜ(server, ਇੰਦਰਾਜ_Iterator), ਅੰਕ++) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP
    TCHAR *attribute;
#ਨਹੀਂਤਾਂ
    ਅੱਖਰ *attribute;
#ਜੇ_ਅੰਤ

    /* Get the DN and write it to the client */
    {
      ਅੱਖਰ *name = ਨਿਰਾ;
      size_t name_len = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP
      TCHAR *dn = ldap_get_dn(server, ਇੰਦਰਾਜ_Iterator);
      ਜੇ(dn)
        name = curlx_convert_tchar_ਮੰਜ਼ਿਲ_UTF8(dn);
#ਨਹੀਂਤਾਂ
      ਅੱਖਰ *dn = name = ldap_get_dn(server, ਇੰਦਰਾਜ_Iterator);
#ਜੇ_ਅੰਤ
      ਜੇ(!name)
        ਨਤੀਜਾ = dn ? CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ : CURLE_FAILED_INIT;
      ਨਹੀਂਤਾਂ {
        name_len = ਲੜੀ_ਲੰਬਾਈ(name);
        ਜੇ(ldap_str_needs_base64(name, name_len)) {
          ਅੱਖਰ *dn_b64 = ਨਿਰਾ;
          size_t dn_b64_sz = 0;

          /* A DN with e.g. CR/LF would inject LDIF lines, so encode it */
          ਨਤੀਜਾ = curlx_base64_encode((uint8_t *)name, name_len,
                                       &dn_b64, &dn_b64_sz);
          ਜੇ(!ਨਤੀਜਾ)
            ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, "DN:: ", 5);
          ਜੇ(!ਨਤੀਜਾ)
            ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, dn_b64,
                                       dn_b64_sz);
          curlx_free(dn_b64);
        }
        ਨਹੀਂਤਾਂ {
          ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, "DN: ", 4);
          ਜੇ(!ਨਤੀਜਾ)
            ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, name, name_len);
        }
        ਜੇ(!ਨਤੀਜਾ)
          ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, "\n", 1);
      }
      FREE_ON_WINLDAP(name);
      ldap_memfree(dn);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ quit;
    }

    /* Get the attributes and write them to the client */
    ਲਈ(attribute = ldap_ਪਹਿਲਾ_attribute(server, ਇੰਦਰਾਜ_Iterator, &ber);
        attribute;
        attribute = ldap_ਅਗਲਾ_attribute(server, ਇੰਦਰਾਜ_Iterator, ber)) {
      Ber_ਮੁੱਲ **vals;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP
      ਅੱਖਰ *attr = curlx_convert_tchar_ਮੰਜ਼ਿਲ_UTF8(attribute);
      ਜੇ(!attr) {
        ldap_memfree(attribute);
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ quit;
      }
#ਨਹੀਂਤਾਂ
      ਅੱਖਰ *attr = attribute;
#ਜੇ_ਅੰਤ
      vals = ldap_get_ਮੁੱਲਾਂ_len(server, ਇੰਦਰਾਜ_Iterator, attribute);
      ਜੇ(vals) {
        ਨਤੀਜਾ = show_vals(ਅੰਕੜੇ, vals, attr);
        /* Free memory used to store values */
        ldap_ਮੁੱਲ_free_len(vals);
      }

      /* Free the attribute as we are done with it */
      FREE_ON_WINLDAP(attr);
      ldap_memfree(attribute);

      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, "\n", 1);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ quit;
    }

    ਜੇ(ber) {
      ber_free(ber, 0);
      ber = ਨਿਰਾ;
    }
  }

quit:
  ਜੇ(ber)
    ber_free(ber, 0);
  ਜੇ(ldapmsg) {
    ldap_msgfree(ldapmsg);
    LDAP_TRACE(("Received %d entries\n", ਅੰਕ));
  }
  ਜੇ(rc == LDAP_SIZELIMIT_EXCEEDED)
    infof(ਅੰਕੜੇ, "There are more than %d entries", ਅੰਕ);
  ਜੇ(ludp)
    ldap_free_urldesc(ludp);
  ਜੇ(server)
    ldap_unbind_s(server);

  FREE_ON_WINLDAP(host);

  /* no data to transfer */
  Curl_xfer_ਤਿਆਰੀ_nop(ਅੰਕੜੇ);
  connclose(conn);

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_LDAP
ਸਥਿਰ ਖਾਲੀ ldap_trace_ਨੀਵਾਂ(ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  ਸਥਿਰ ਪੂਰਨ do_trace = -1;
  va_list ਦਲੀਲਾਂ;

  ਜੇ(do_trace == -1) {
    ਅਚਲ ਅੱਖਰ *env = getenv("CURL_TRACE");
    curl_off_t e = 0;
    ਜੇ(!curlx_str_ਸੰਖਿਆ(&env, &e, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ))
      do_trace = e > 0;
  }
  ਜੇ(!do_trace)
    ਵਾਪਸ;

  va_start(ਦਲੀਲਾਂ, fmt);
  curl_mvfprintf(stderr, fmt, ਦਲੀਲਾਂ);
  va_end(ਦਲੀਲਾਂ);
}
#ਜੇ_ਅੰਤ /* DEBUG_LDAP */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_LDAP_URL_PARSE

/*
 * Return scope-value for a scope-string.
 */
ਸਥਿਰ ਪੂਰਨ str2scope(ਅਚਲ ਅੱਖਰ *p)
{
  ਜੇ(curl_strequal(p, "one"))
    ਵਾਪਸ LDAP_SCOPE_ONELEVEL;
  ਜੇ(curl_strequal(p, "onetree"))
    ਵਾਪਸ LDAP_SCOPE_ONELEVEL;
  ਜੇ(curl_strequal(p, "base"))
    ਵਾਪਸ LDAP_SCOPE_ਅਧਾਰ;
  ਜੇ(curl_strequal(p, "sub"))
    ਵਾਪਸ LDAP_SCOPE_SUBTREE;
  ਜੇ(curl_strequal(p, "subtree"))
    ਵਾਪਸ LDAP_SCOPE_SUBTREE;
  ਵਾਪਸ -1;
}

/* number of entries in the attributes list */
ਸਥਿਰ size_t ਅੰਕ_entries(ਅਚਲ ਅੱਖਰ *s)
{
  size_t ਚੀਜ਼ਾਂ = 1;

  s = strchr(s, ',');
  ਜਦੋਂ(s) {
    ਚੀਜ਼ਾਂ++;
    s = strchr(s + 1, ',');
  }
  ਵਾਪਸ ਚੀਜ਼ਾਂ;
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
ਸਥਿਰ curl_ldap_ਅੰਕ_t ldap_url_parse2_ਨੀਵਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਅਚਲ ਬਣਤਰ connectdata *conn,
                                           LDAPURLDesc *ludp)
{
  curl_ldap_ਅੰਕ_t rc = LDAP_SUCCESS;
  ਅੱਖਰ *p;
  ਅੱਖਰ *ਰਸਤਾ;
  ਅੱਖਰ *q = ਨਿਰਾ;
  ਅੱਖਰ *query = ਨਿਰਾ;
  size_t i;

  ਜੇ(!ਅੰਕੜੇ ||
     !ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ ||
     ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ[0] != '/' ||
     ((ਅੰਕੜੇ->state.origin->scheme != &Curl_scheme_ldap) &&
      (ਅੰਕੜੇ->state.origin->scheme != &Curl_scheme_ldaps)))
    ਵਾਪਸ LDAP_INVALID_SYNTAX;

  ludp->lud_scope = LDAP_SCOPE_ਅਧਾਰ;
  ludp->lud_ਪੋਰਟ  = conn->origin->ਪੋਰਟ;
  ludp->lud_host  = conn->origin->hostname;

  /* Duplicate the path */
  p = ਰਸਤਾ = curlx_strdup(ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ + 1);
  ਜੇ(!ਰਸਤਾ)
    ਵਾਪਸ LDAP_NO_ਸਮ੍ਰਿਤੀ;

  /* Duplicate the query if present */
  ਜੇ(ਅੰਕੜੇ->state.ਉੱਪਰ.query) {
    q = query = curlx_strdup(ਅੰਕੜੇ->state.ਉੱਪਰ.query);
    ਜੇ(!query) {
      curlx_free(ਰਸਤਾ);
      ਵਾਪਸ LDAP_NO_ਸਮ੍ਰਿਤੀ;
    }
  }

  /* Parse the DN (Distinguished Name) */
  ਜੇ(*p) {
    ਅੱਖਰ *dn = p;
    ਅੱਖਰ *unescaped;
    CURLcode ਨਤੀਜਾ;

    LDAP_TRACE(("DN '%s'\n", dn));

    /* Unescape the DN */
    ਨਤੀਜਾ = Curl_urldecode(dn, 0, &unescaped, ਨਿਰਾ, REJECT_ZERO);
    ਜੇ(ਨਤੀਜਾ) {
      rc = LDAP_NO_ਸਮ੍ਰਿਤੀ;

      ਜਾਓ quit;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_dn = curlx_convert_UTF8_ਮੰਜ਼ਿਲ_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    ਜੇ(!ludp->lud_dn) {
      rc = LDAP_NO_ਸਮ੍ਰਿਤੀ;

      ਜਾਓ quit;
    }
#ਨਹੀਂਤਾਂ
    ludp->lud_dn = unescaped;
#ਜੇ_ਅੰਤ
  }

  p = q;
  ਜੇ(!p)
    ਜਾਓ quit;

  /* Parse the attributes. skip "??" */
  q = strchr(p, '?');
  ਜੇ(q)
    *q++ = '\0';

  ਜੇ(*p) {
    size_t ਗਿਣਤੀ_2 = ਅੰਕ_entries(p); /* at least one */
    ਅਚਲ ਅੱਖਰ *atp = p;

    /* Allocate our array (+1 for the NULL entry) */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP
    ludp->lud_attrs = curlx_calloc(ਗਿਣਤੀ_2 + 1, ਆਕਾਰ(TCHAR *));
#ਨਹੀਂਤਾਂ
    ludp->lud_attrs = curlx_calloc(ਗਿਣਤੀ_2 + 1, ਆਕਾਰ(ਅੱਖਰ *));
#ਜੇ_ਅੰਤ
    ਜੇ(!ludp->lud_attrs) {
      rc = LDAP_NO_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ quit;
    }

    ਲਈ(i = 0; i < ਗਿਣਤੀ_2; i++) {
      ਅੱਖਰ *unescaped;
      CURLcode ਨਤੀਜਾ;
      ਬਣਤਰ Curl_str ਬਾਹਰਲਾ;

      ਜੇ(curlx_str_until(&atp, &ਬਾਹਰਲਾ, 1024, ','))
        ਰੋਕੋ;

      LDAP_TRACE(("attr[%zu] '%.*s'\n", i, (ਪੂਰਨ)ਬਾਹਰਲਾ.len, ਬਾਹਰਲਾ.str));

      /* Unescape the attribute */
      ਨਤੀਜਾ = Curl_urldecode(ਬਾਹਰਲਾ.str, ਬਾਹਰਲਾ.len, &unescaped, ਨਿਰਾ, REJECT_ZERO);
      ਜੇ(ਨਤੀਜਾ) {
        rc = LDAP_NO_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ quit;
      }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP
      /* Convert the unescaped string to a tchar */
      ludp->lud_attrs[i] = curlx_convert_UTF8_ਮੰਜ਼ਿਲ_tchar(unescaped);

      /* Free the unescaped string as we are done with it */
      curlx_free(unescaped);

      ਜੇ(!ludp->lud_attrs[i]) {
        rc = LDAP_NO_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ quit;
      }
#ਨਹੀਂਤਾਂ
      ludp->lud_attrs[i] = unescaped;
#ਜੇ_ਅੰਤ

      ludp->lud_attrs_dups++;
      ਜੇ(curlx_str_single(&atp, ','))
        ਰੋਕੋ;
    }
  }

  p = q;
  ਜੇ(!p)
    ਜਾਓ quit;

  /* Parse the scope. skip "??" */
  q = strchr(p, '?');
  ਜੇ(q)
    *q++ = '\0';

  ਜੇ(*p) {
    ludp->lud_scope = str2scope(p);
    ਜੇ(ludp->lud_scope == -1) {
      rc = LDAP_INVALID_SYNTAX;

      ਜਾਓ quit;
    }
    LDAP_TRACE(("scope %d\n", ludp->lud_scope));
  }

  p = q;
  ਜੇ(!p)
    ਜਾਓ quit;

  /* Parse the filter */
  q = strchr(p, '?');
  ਜੇ(q)
    *q++ = '\0';

  ਜੇ(*p) {
    ਅੱਖਰ *filter = p;
    ਅੱਖਰ *unescaped;
    CURLcode ਨਤੀਜਾ;

    LDAP_TRACE(("filter '%s'\n", filter));

    /* Unescape the filter */
    ਨਤੀਜਾ = Curl_urldecode(filter, 0, &unescaped, ਨਿਰਾ, REJECT_CTRL);
    ਜੇ(ਨਤੀਜਾ) {
      rc = LDAP_NO_ਸਮ੍ਰਿਤੀ;

      ਜਾਓ quit;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_filter = curlx_convert_UTF8_ਮੰਜ਼ਿਲ_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    ਜੇ(!ludp->lud_filter) {
      rc = LDAP_NO_ਸਮ੍ਰਿਤੀ;

      ਜਾਓ quit;
    }
#ਨਹੀਂਤਾਂ
    ludp->lud_filter = unescaped;
#ਜੇ_ਅੰਤ
  }

  p = q;
  ਜੇ(p && !*p) {
    rc = LDAP_INVALID_SYNTAX;

    ਜਾਓ quit;
  }

quit:
  curlx_free(ਰਸਤਾ);
  curlx_free(query);

  ਵਾਪਸ rc;
}

ਸਥਿਰ curl_ldap_ਅੰਕ_t ldap_url_parse_ਨੀਵਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਅਚਲ ਬਣਤਰ connectdata *conn,
                                          LDAPURLDesc **ludpp)
{
  LDAPURLDesc *ludp = curlx_calloc(1, ਆਕਾਰ(*ludp));
  curl_ldap_ਅੰਕ_t rc;

  *ludpp = ਨਿਰਾ;
  ਜੇ(!ludp)
    ਵਾਪਸ LDAP_NO_ਸਮ੍ਰਿਤੀ;

  rc = ldap_url_parse2_ਨੀਵਾਂ(ਅੰਕੜੇ, conn, ludp);
  ਜੇ(rc != LDAP_SUCCESS) {
    ldap_free_urldesc_ਨੀਵਾਂ(ludp);
    ludp = ਨਿਰਾ;
  }
  *ludpp = ludp;
  ਵਾਪਸ rc;
}

ਸਥਿਰ ਖਾਲੀ ldap_free_urldesc_ਨੀਵਾਂ(LDAPURLDesc *ludp)
{
  ਜੇ(!ludp)
    ਵਾਪਸ;

  curlx_free(ludp->lud_dn);
  curlx_free(ludp->lud_filter);

  ਜੇ(ludp->lud_attrs) {
    size_t i;
    ਲਈ(i = 0; i < ludp->lud_attrs_dups; i++) {
      curlx_free(ludp->lud_attrs[i]);
    }
    curlx_free(ludp->lud_attrs);
  }

  curlx_free(ludp);
}
#ਜੇ_ਅੰਤ /* !HAVE_LDAP_URL_PARSE */

ਖਾਲੀ Curl_ldap_ਸੰਸਕਰਣ(ਅੱਖਰ *buf, size_t bufsz)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_LDAP
  curl_msnprintf(buf, bufsz, "WinLDAP");
#ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LDAP_OPT_X_TLS_PASSPHRASE
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ flavor[] = "/Apple";
#ਨਹੀਂਤਾਂ
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ flavor[] = "";
#ਜੇ_ਅੰਤ
  LDAPAPIInfo api;
  api.ldapai_info_ਸੰਸਕਰਣ = LDAP_API_INFO_ਸੰਸਕਰਣ;

  /* Comparing against 0, as different platforms
     disagree on the success define name */
  ਜੇ(ldap_get_option(ਨਿਰਾ, LDAP_OPT_API_INFO, &api) == 0) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ patch = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(api.ldapai_vendor_ਸੰਸਕਰਣ % 100);
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ major = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(api.ldapai_vendor_ਸੰਸਕਰਣ / 10000);
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ minor =
      (((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)api.ldapai_vendor_ਸੰਸਕਰਣ - (major * 10000))
       - patch) / 100;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __OS400__
    curl_msnprintf(buf, bufsz, "IBMLDAP/%u.%u.%u", major, minor, patch);
    ldap_ਮੁੱਲ_free(api.ldapai_extensions);
    (ਖਾਲੀ)flavor;
#ਨਹੀਂਤਾਂ
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u%s",
                   api.ldapai_vendor_name, major, minor, patch, flavor);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((ਖਾਲੀ **)api.ldapai_extensions);
#ਜੇ_ਅੰਤ
  }
  ਨਹੀਂਤਾਂ
    curl_msnprintf(buf, bufsz, "LDAP/1");
#ਜੇ_ਅੰਤ /* USE_WIN32_LDAP */
}

/*
 * LDAP protocol handler.
 */
ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_ldap = {
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

#ਜੇ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_LDAP && !USE_OPENLDAP */
