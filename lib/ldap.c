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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_ldap.h"

#ಆದರೆ !defined(CURL_DISABLE_LDAP) && !defined(USE_OPENLDAP)

#ಆದರೆ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Wdeprecated-declarations"
#ಆದರೆ_ಅಂತ್ಯ

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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP           /* Use Windows LDAP implementation. */
#  include <winldap.h>
/* Undefine indirect <wincrypt.h> symbols conflicting with AWS-LC/BoringSSL. */
#  undef X509_NAME
#  undef X509_EXTENSIONS
#  undef PKCS7_ISSUER_AND_SERIAL
#  undef PKCS7_SIGNER_INFO
#  undef OCSP_REQUEST
#  undef OCSP_RESPONSE
#  include <winber.h>
#ಇಲ್ಲದಿದ್ದರೆ
#  define LDAP_DEPRECATED 1     /* Be sure ldap_init() is defined. */
#  ifdef HAVE_LBER_H
#    include <lber.h>
#  endif
#  include <ldap.h>
#  ಆದರೆ defined(HAVE_LDAP_SSL) && defined(HAVE_LDAP_SSL_H)
#    include <ldap_ssl.h>
#  endif /* HAVE_LDAP_SSL && HAVE_LDAP_SSL_H */
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "curlx/multibyte.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#ಅನ್ತರ್ಭಾವ "connect.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP
#ವ್ಯಾಖ್ಯಾನಿಸು FREE_ON_WINLDAP(x) curlx_free(x)
#ವ್ಯಾಖ್ಯಾನಿಸು curl_ldap_ಅಂಕೆ_t    ULONG
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು FREE_ON_WINLDAP(x) ಮಾಡು {} ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು curl_ldap_ಅಂಕೆ_t    ಪೂರ್ಣಾಂಕ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_LDAP_URL_PARSE

/* Use our own implementation. */

ರಚನೆ ldap_urldesc {
  ಅಕ್ಷರ   *lud_host;
  ಪೂರ್ಣಾಂಕ     lud_ಪೋರ್ಟ್;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP
  TCHAR  *lud_dn;
  TCHAR **lud_attrs;
#ಇಲ್ಲದಿದ್ದರೆ
  ಅಕ್ಷರ   *lud_dn;
  ಅಕ್ಷರ  **lud_attrs;
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ಣಾಂಕ     lud_scope;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP
  TCHAR  *lud_filter;
#ಇಲ್ಲದಿದ್ದರೆ
  ಅಕ್ಷರ   *lud_filter;
#ಆದರೆ_ಅಂತ್ಯ
  ಅಕ್ಷರ  **lud_exts;
  size_t  lud_attrs_dups; /* how many were dup'ed, this field is not in the
                             "real" struct so can only be used in code
                             without HAVE_LDAP_URL_PARSE defined */
};

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು LDAPURLDesc
#ವ್ಯಾಖ್ಯಾನಿಸು LDAPURLDesc ರಚನೆ ldap_urldesc

ಸ್ಥಿರ curl_ldap_ಅಂಕೆ_t ldap_url_parse_ಕೆಳಗಿನ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ಸ್ಥಿರಾಂಕ ರಚನೆ connectdata *conn,
                                          LDAPURLDesc **ludpp);
ಸ್ಥಿರ ಶೂನ್ಯ ldap_free_urldesc_ಕೆಳಗಿನ(LDAPURLDesc *ludp);

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ldap_free_urldesc
#ವ್ಯಾಖ್ಯಾನಿಸು ldap_free_urldesc       ldap_free_urldesc_ಕೆಳಗಿನ

#ಆದರೆ_ಅಂತ್ಯ /* !HAVE_LDAP_URL_PARSE */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_LDAP
#ವ್ಯಾಖ್ಯಾನಿಸು LDAP_TRACE(x)                 \
  ಮಾಡು {                                \
    ldap_trace_ಕೆಳಗಿನ("%u: ", __LINE__); \
    ldap_trace_ಕೆಳಗಿನ x;                 \
  } ಆಗ(0)

ಸ್ಥಿರ ಶೂನ್ಯ ldap_trace_ಕೆಳಗಿನ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) CURL_PRINTF(1, 2);
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು LDAP_TRACE(x) Curl_nop_stmt
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_WIN32_LDAP) && defined(ldap_err2string)
/* Use ANSI error strings in Unicode builds */
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ldap_err2string
#ವ್ಯಾಖ್ಯಾನಿಸು ldap_err2string ldap_err2stringA
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_WIN32_LDAP) && defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'void *' of greater size */
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು LDAP_OPT_ON
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು LDAP_OPT_OFF
#ವ್ಯಾಖ್ಯಾನಿಸು LDAP_OPT_ON   ((ಶೂನ್ಯ *)(size_t)1)
#ವ್ಯಾಖ್ಯಾನಿಸು LDAP_OPT_OFF  ((ಶೂನ್ಯ *)(size_t)0)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI
ಸ್ಥಿರ ULONG ldap_win_bind_auth(LDAP *server, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *passwd, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ authflags)
{
  ULONG ವಿಧಾನ = 0;
  SEC_WINNT_AUTH_ತಟಸ್ಥ_EX cred;
  ULONG rc = LDAP_AUTH_ವಿಧಾನ_NOT_SUPPORTED;

  memset(&cred, 0, ಗಾತ್ರ(cred));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SPNEGO
  ಆದರೆ(authflags & CURLAUTH_NEGOTIATE) {
    ವಿಧಾನ = LDAP_AUTH_NEGOTIATE;
  }
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NTLM
  ಆದರೆ(authflags & CURLAUTH_NTLM) {
    ವಿಧಾನ = LDAP_AUTH_NTLM;
  }
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH
  ಆದರೆ(authflags & CURLAUTH_DIGEST) {
    ವಿಧಾನ = LDAP_AUTH_DIGEST;
  }
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
  {
    /* required anyway if one of upper preprocessor definitions enabled */
  }

  ಆದರೆ(ವಿಧಾನ && user && passwd) {
    CURLcode ಫಲಿತಾಂಶ = Curl_create_sspi_ತಟಸ್ಥ(user, passwd, &cred);
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      rc = ldap_bind_s(server, ಖಾಲಿ, (TCHAR *)&cred, ವಿಧಾನ);
      Curl_sspi_free_ತಟಸ್ಥ(&cred);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      rc = LDAP_NO_ಸ್ಮರಣೆ;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* proceed with current user credentials */
    ವಿಧಾನ = LDAP_AUTH_NEGOTIATE;
    rc = ldap_bind_s(server, ಖಾಲಿ, ಖಾಲಿ, ವಿಧಾನ);
  }
  ಹಿಂತಿರುಗಿ rc;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_WINDOWS_SSPI */

ಸ್ಥಿರ ULONG ldap_win_bind(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, LDAP *server,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *passwd)
{
  ULONG rc = LDAP_INVALID_CREDENTIALS;

  PTCHAR inuser = ಖಾಲಿ;
  PTCHAR inpass = ಖಾಲಿ;

  ಆದರೆ(user && passwd && (ದತ್ತಾಂಶ->ಗಣ.httpauth & CURLAUTH_BASIC)) {
    inuser = curlx_convert_UTF8_ಗಮ್ಯ_tchar(user);
    inpass = curlx_convert_UTF8_ಗಮ್ಯ_tchar(passwd);

    rc = ldap_simple_bind_s(server, inuser, inpass);

    curlx_free(inuser);
    curlx_free(inpass);
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI
  ಇಲ್ಲದಿದ್ದರೆ {
    rc = ldap_win_bind_auth(server, user, passwd, ದತ್ತಾಂಶ->ಗಣ.httpauth);
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ rc;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_WIN32_LDAP */

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
ಸ್ಥಿರ bool ldap_str_needs_base64(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len)
{
  size_t i;
  ಆದರೆ(!len)
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    ಹಿಂತಿರುಗಿ TRUE;
  ಗಾಗಿ(i = 0; i < len; i++)
    ಆದರೆ(!ISPRINT(s[i]))
      ಹಿಂತಿರುಗಿ TRUE;
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ bool ldap_ಮೌಲ್ಯ_needs_base64(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *attr, size_t attr_len,
                                    ಸ್ಥಿರಾಂಕ Ber_ಮೌಲ್ಯ *val)
{
  ಆದರೆ((attr_len > 7) && curl_strequal(";binary", attr + attr_len - 7))
    ಹಿಂತಿರುಗಿ TRUE;

  ಹಿಂತಿರುಗಿ ldap_str_needs_base64(val->bv_val, val->bv_len);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP
ಸ್ಥಿರ BOOLEAN bypass_cert_verify(PLDAP Connection,
                                  PCCERT_CONTEXT *ppServerCert)
{
  (ಶೂನ್ಯ)Connection;
  CertFreeCertificateContext(*ppServerCert);
  /* approve any certificate since the verification is set to bypass */
  ಹಿಂತಿರುಗಿ TRUE;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode show_vals(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, Ber_ಮೌಲ್ಯ **vals,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *attr)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ i;
  size_t attr_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(attr);

  /* An attribute type has no base64 form in LDIF, so a type carrying control
     bytes (CR/LF etc.) would let a hostile server inject lines. A conformant
     type name never contains them, so reject it. */
  ಆದರೆ(ldap_str_needs_base64(attr, attr_len)) {
    failf(ದತ್ತಾಂಶ, "LDAP local: attribute type contains illegal bytes");
    ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
  }

  ಗಾಗಿ(i = 0; vals[i] && !ಫಲಿತಾಂಶ; i++) {
    ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, "\t", 1);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, attr, attr_len);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, ":", 1);

    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;

    ಆದರೆ(ldap_ಮೌಲ್ಯ_needs_base64(attr, attr_len, vals[i])) {
      ಅಕ್ಷರ *val_b64 = ಖಾಲಿ;
      size_t val_b64_sz = 0;

      /* Binary attribute, encode to base64. */
      ಆದರೆ(vals[i]->bv_len)
        ಫಲಿತಾಂಶ = curlx_base64_encode((uint8_t *)vals[i]->bv_val,
                                     vals[i]->bv_len,
                                     &val_b64, &val_b64_sz);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, ": ", 2);
      ಆದರೆ(!ಫಲಿತಾಂಶ && val_b64_sz)
        ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, val_b64,
                                   val_b64_sz);
      curlx_free(val_b64);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, " ", 1);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY,
                                   vals[i]->bv_val, vals[i]->bv_len);
    }

    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, "\n", 1);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ldap_do(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  curl_ldap_ಅಂಕೆ_t rc = LDAP_SUCCESS;
  LDAP *server = ಖಾಲಿ;
  LDAPURLDesc *ludp = ಖಾಲಿ;
  LDAPMessage *ldapmsg = ಖಾಲಿ;
  LDAPMessage *ನಮೂದು_Iterator;
  ಪೂರ್ಣಾಂಕ ಅಂಕೆ = 0;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ಪೂರ್ಣಾಂಕ ldap_proto = LDAP_VERSION3;
  bool ldap_ssl = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LDAP_OPT_ಜಾಲ_TIMEOUT
  ರಚನೆ timeval ldap_timeout = { 10, 0 }; /* 10s connection/search timeout */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP
  TCHAR *host = ಖಾಲಿ;
#ಇಲ್ಲದಿದ್ದರೆ
  ಅಕ್ಷರ *host = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user = ದತ್ತಾಂಶ->state.creds ? ದತ್ತಾಂಶ->state.creds->user : ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *passwd = ದತ್ತಾಂಶ->state.creds ? ದತ್ತಾಂಶ->state.creds->passwd : ಖಾಲಿ;
  ರಚನೆ ip_quadruple ipquad;
  bool is_ipv6;
  BerElement *ber = ಖಾಲಿ;

  *ಮುಗಿದ = TRUE; /* unconditionally */
  infof(ದತ್ತಾಂಶ, "LDAP local: LDAP Vendor = %s ; LDAP Version = %d",
        LDAP_VENDOR_NAME, LDAP_VENDOR_ಆವೃತ್ತಿ);
  infof(ದತ್ತಾಂಶ, "LDAP local: %s", Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.url));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LDAP_URL_PARSE
  rc = ldap_url_parse(Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.url), &ludp);
#ಇಲ್ಲದಿದ್ದರೆ
  rc = ldap_url_parse_ಕೆಳಗಿನ(ದತ್ತಾಂಶ, conn, &ludp);
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(rc) {
    failf(ದತ್ತಾಂಶ, "Bad LDAP URL: %s", ldap_err2string(rc));
    ಫಲಿತಾಂಶ = CURLE_URL_MALFORMAT;
    ಹೋಗು quit;
  }

  ಫಲಿತಾಂಶ = Curl_conn_get_ip_info(ದತ್ತಾಂಶ, conn, FIRSTSOCKET, &is_ipv6, &ipquad);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು quit;

  /* Get the URL scheme (either ldap or ldaps) */
  ldap_ssl = Curl_conn_is_ssl(conn, FIRSTSOCKET);
  infof(ದತ್ತಾಂಶ, "LDAP local: trying to establish %s connection",
        ldap_ssl ? "encrypted" : "cleartext");

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP
  host = curlx_convert_UTF8_ಗಮ್ಯ_tchar(conn->origin->hostname);
  ಆದರೆ(!host) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    ಹೋಗು quit;
  }
#ಇಲ್ಲದಿದ್ದರೆ
  host = conn->origin->hostname;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP
  ಆದರೆ(ldap_ssl)
    server = ldap_sslinit(host, (curl_ldap_ಅಂಕೆ_t)ipquad.remote_ಪೋರ್ಟ್, 1);
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    server = ldap_init(host, (curl_ldap_ಅಂಕೆ_t)ipquad.remote_ಪೋರ್ಟ್);
  ಆದರೆ(!server) {
    failf(ದತ್ತಾಂಶ, "LDAP: cannot setup connect to %s:%u",
          conn->origin->user_hostname, ipquad.remote_ಪೋರ್ಟ್);
    ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
    ಹೋಗು quit;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LDAP_OPT_ಜಾಲ_TIMEOUT
  ldap_ಗಣ_option(server, LDAP_OPT_ಜಾಲ_TIMEOUT, &ldap_timeout);
#ಆದರೆ_ಅಂತ್ಯ
  ldap_ಗಣ_option(server, LDAP_OPT_PROTOCOL_ಆವೃತ್ತಿ, &ldap_proto);

  /* Do not chase referrals. */
  ldap_ಗಣ_option(server, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  ಆದರೆ(ldap_ssl) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LDAP_SSL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP
    /* Win32 LDAP uses the Windows CA store to verify certificates */
    ldap_ಗಣ_option(server, LDAP_OPT_SSL, LDAP_OPT_ON);
    ಆದರೆ(!conn->ssl_config.verifypeer) {
      ಆದರೆ(conn->ssl_config.verifyhost) {
        failf(ದತ್ತಾಂಶ, "LDAP local: host verification cannot be enabled when "
              "peer verification is disabled for Windows native LDAP");
        ಫಲಿತಾಂಶ = CURLE_NOT_BUILT_ಒಳ;
        ಹೋಗು quit;
      }
      ldap_ಗಣ_option(server, LDAP_OPT_SERVER_CERTIFICATE,
                      (ಶೂನ್ಯ *)(uintptr_t)bypass_cert_verify);
    }
#ಅಥವಾಆದರೆ defined(LDAP_OPT_X_TLS) /* !USE_WIN32_LDAP */
    ಪೂರ್ಣಾಂಕ ldap_option;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ldap_ca = conn->ssl_config.CAfile;
    ಆದರೆ(conn->ssl_config.verifypeer) {
      /* OpenLDAP SDK supports BASE64 files. */
      ಆದರೆ(conn->ssl_config.cert_type &&
         !curl_strequal(conn->ssl_config.cert_type, "PEM")) {
        failf(ದತ್ತಾಂಶ, "LDAP local: ERROR OpenLDAP only supports PEM cert-type");
        ಫಲಿತಾಂಶ = CURLE_SSL_CERTPROBLEM;
        ಹೋಗು quit;
      }
      ಆದರೆ(!ldap_ca) {
        failf(ದತ್ತಾಂಶ, "LDAP local: ERROR PEM CA cert not set");
        ಫಲಿತಾಂಶ = CURLE_SSL_CERTPROBLEM;
        ಹೋಗು quit;
      }
      infof(ದತ್ತಾಂಶ, "LDAP local: using PEM CA cert: %s", ldap_ca);
      rc = ldap_ಗಣ_option(server, LDAP_OPT_X_TLS_CACERTFILE, ldap_ca);
      ಆದರೆ(rc != LDAP_SUCCESS) {
        failf(ದತ್ತಾಂಶ, "LDAP local: ERROR setting PEM CA cert: %s",
              ldap_err2string(rc));
        ಫಲಿತಾಂಶ = CURLE_SSL_CERTPROBLEM;
        ಹೋಗು quit;
      }
      ldap_option = LDAP_OPT_X_TLS_DEMAND;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ldap_option = LDAP_OPT_X_TLS_NEVER;

    rc = ldap_ಗಣ_option(server, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_option);
    ಆದರೆ(rc != LDAP_SUCCESS) {
      failf(ದತ್ತಾಂಶ, "LDAP local: ERROR setting cert verify mode: %s",
            ldap_err2string(rc));
      ಫಲಿತಾಂಶ = CURLE_SSL_CERTPROBLEM;
      ಹೋಗು quit;
    }
    ldap_option = LDAP_OPT_X_TLS_HARD;
    rc = ldap_ಗಣ_option(server, LDAP_OPT_X_TLS, &ldap_option);
    ಆದರೆ(rc != LDAP_SUCCESS) {
      failf(ದತ್ತಾಂಶ, "LDAP local: ERROR setting SSL/TLS mode: %s",
            ldap_err2string(rc));
      ಫಲಿತಾಂಶ = CURLE_SSL_CERTPROBLEM;
      ಹೋಗು quit;
    }
#ಇಲ್ಲದಿದ್ದರೆ /* !USE_WIN32_LDAP && !LDAP_OPT_X_TLS */
    /* we should probably never come up to here since configure
       should check in first place if we can support LDAP SSL/TLS */
    failf(ದತ್ತಾಂಶ, "LDAP local: SSL/TLS not supported with this version "
          "of the OpenLDAP toolkit");
    ಫಲಿತಾಂಶ = CURLE_SSL_CERTPROBLEM;
    ಹೋಗು quit;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_LDAP_SSL */
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.use_ssl > CURLUSESSL_TRY) {
    failf(ದತ್ತಾಂಶ, "LDAP local: explicit TLS not supported");
    ಫಲಿತಾಂಶ = CURLE_NOT_BUILT_ಒಳ;
    ಹೋಗು quit;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP
  rc = ldap_win_bind(ದತ್ತಾಂಶ, server, user, passwd);
#ಇಲ್ಲದಿದ್ದರೆ
  rc = ldap_simple_bind_s(server, user, passwd);
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(!ldap_ssl && rc != LDAP_SUCCESS) {
    ldap_proto = LDAP_VERSION2;
    ldap_ಗಣ_option(server, LDAP_OPT_PROTOCOL_ಆವೃತ್ತಿ, &ldap_proto);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP
    rc = ldap_win_bind(ದತ್ತಾಂಶ, server, user, passwd);
#ಇಲ್ಲದಿದ್ದರೆ
    rc = ldap_simple_bind_s(server, user, passwd);
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಆದರೆ(rc != LDAP_SUCCESS) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP
    failf(ದತ್ತಾಂಶ, "LDAP local: bind via ldap_win_bind %s",
          ldap_err2string(rc));
#ಇಲ್ಲದಿದ್ದರೆ
    failf(ದತ್ತಾಂಶ, "LDAP local: bind via ldap_simple_bind_s %s",
          ldap_err2string(rc));
#ಆದರೆ_ಅಂತ್ಯ
    ಫಲಿತಾಂಶ = CURLE_LDAP_CANNOT_BIND;
    ಹೋಗು quit;
  }

  Curl_pgrsReset(ದತ್ತಾಂಶ);
  rc = ldap_ಹುಡುಕು_s(server, ludp->lud_dn,
                     ludp->lud_scope,
                     ludp->lud_filter, ludp->lud_attrs, 0, &ldapmsg);

  ಆದರೆ(rc != LDAP_SUCCESS && rc != LDAP_SIZELIMIT_EXCEEDED) {
    failf(ದತ್ತಾಂಶ, "LDAP remote: %s", ldap_err2string(rc));
    ಫಲಿತಾಂಶ = CURLE_LDAP_ಹುಡುಕು_FAILED;
    ಹೋಗು quit;
  }

  ಅಂಕೆ = 0;
  ಗಾಗಿ(ನಮೂದು_Iterator = ldap_ಮೊದಲ_ನಮೂದು(server, ldapmsg);
      ನಮೂದು_Iterator;
      ನಮೂದು_Iterator = ldap_ಮುಂದಿನ_ನಮೂದು(server, ನಮೂದು_Iterator), ಅಂಕೆ++) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP
    TCHAR *attribute;
#ಇಲ್ಲದಿದ್ದರೆ
    ಅಕ್ಷರ *attribute;
#ಆದರೆ_ಅಂತ್ಯ

    /* Get the DN and write it to the client */
    {
      ಅಕ್ಷರ *name = ಖಾಲಿ;
      size_t name_len = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP
      TCHAR *dn = ldap_get_dn(server, ನಮೂದು_Iterator);
      ಆದರೆ(dn)
        name = curlx_convert_tchar_ಗಮ್ಯ_UTF8(dn);
#ಇಲ್ಲದಿದ್ದರೆ
      ಅಕ್ಷರ *dn = name = ldap_get_dn(server, ನಮೂದು_Iterator);
#ಆದರೆ_ಅಂತ್ಯ
      ಆದರೆ(!name)
        ಫಲಿತಾಂಶ = dn ? CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ : CURLE_FAILED_INIT;
      ಇಲ್ಲದಿದ್ದರೆ {
        name_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name);
        ಆದರೆ(ldap_str_needs_base64(name, name_len)) {
          ಅಕ್ಷರ *dn_b64 = ಖಾಲಿ;
          size_t dn_b64_sz = 0;

          /* A DN with e.g. CR/LF would inject LDIF lines, so encode it */
          ಫಲಿತಾಂಶ = curlx_base64_encode((uint8_t *)name, name_len,
                                       &dn_b64, &dn_b64_sz);
          ಆದರೆ(!ಫಲಿತಾಂಶ)
            ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, "DN:: ", 5);
          ಆದರೆ(!ಫಲಿತಾಂಶ)
            ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, dn_b64,
                                       dn_b64_sz);
          curlx_free(dn_b64);
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, "DN: ", 4);
          ಆದರೆ(!ಫಲಿತಾಂಶ)
            ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, name, name_len);
        }
        ಆದರೆ(!ಫಲಿತಾಂಶ)
          ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, "\n", 1);
      }
      FREE_ON_WINLDAP(name);
      ldap_memfree(dn);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು quit;
    }

    /* Get the attributes and write them to the client */
    ಗಾಗಿ(attribute = ldap_ಮೊದಲ_attribute(server, ನಮೂದು_Iterator, &ber);
        attribute;
        attribute = ldap_ಮುಂದಿನ_attribute(server, ನಮೂದು_Iterator, ber)) {
      Ber_ಮೌಲ್ಯ **vals;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP
      ಅಕ್ಷರ *attr = curlx_convert_tchar_ಗಮ್ಯ_UTF8(attribute);
      ಆದರೆ(!attr) {
        ldap_memfree(attribute);
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು quit;
      }
#ಇಲ್ಲದಿದ್ದರೆ
      ಅಕ್ಷರ *attr = attribute;
#ಆದರೆ_ಅಂತ್ಯ
      vals = ldap_get_ಮೌಲ್ಯಗಳು_len(server, ನಮೂದು_Iterator, attribute);
      ಆದರೆ(vals) {
        ಫಲಿತಾಂಶ = show_vals(ದತ್ತಾಂಶ, vals, attr);
        /* Free memory used to store values */
        ldap_ಮೌಲ್ಯ_free_len(vals);
      }

      /* Free the attribute as we are done with it */
      FREE_ON_WINLDAP(attr);
      ldap_memfree(attribute);

      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, "\n", 1);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು quit;
    }

    ಆದರೆ(ber) {
      ber_free(ber, 0);
      ber = ಖಾಲಿ;
    }
  }

quit:
  ಆದರೆ(ber)
    ber_free(ber, 0);
  ಆದರೆ(ldapmsg) {
    ldap_msgfree(ldapmsg);
    LDAP_TRACE(("Received %d entries\n", ಅಂಕೆ));
  }
  ಆದರೆ(rc == LDAP_SIZELIMIT_EXCEEDED)
    infof(ದತ್ತಾಂಶ, "There are more than %d entries", ಅಂಕೆ);
  ಆದರೆ(ludp)
    ldap_free_urldesc(ludp);
  ಆದರೆ(server)
    ldap_unbind_s(server);

  FREE_ON_WINLDAP(host);

  /* no data to transfer */
  Curl_xfer_ಸಿದ್ಧತೆ_nop(ದತ್ತಾಂಶ);
  connclose(conn);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_LDAP
ಸ್ಥಿರ ಶೂನ್ಯ ldap_trace_ಕೆಳಗಿನ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ do_trace = -1;
  va_list ವಾದಗಳು;

  ಆದರೆ(do_trace == -1) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *env = getenv("CURL_TRACE");
    curl_off_t e = 0;
    ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&env, &e, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ))
      do_trace = e > 0;
  }
  ಆದರೆ(!do_trace)
    ಹಿಂತಿರುಗಿ;

  va_start(ವಾದಗಳು, fmt);
  curl_mvfprintf(stderr, fmt, ವಾದಗಳು);
  va_end(ವಾದಗಳು);
}
#ಆದರೆ_ಅಂತ್ಯ /* DEBUG_LDAP */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_LDAP_URL_PARSE

/*
 * Return scope-value for a scope-string.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ str2scope(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p)
{
  ಆದರೆ(curl_strequal(p, "one"))
    ಹಿಂತಿರುಗಿ LDAP_SCOPE_ONELEVEL;
  ಆದರೆ(curl_strequal(p, "onetree"))
    ಹಿಂತಿರುಗಿ LDAP_SCOPE_ONELEVEL;
  ಆದರೆ(curl_strequal(p, "base"))
    ಹಿಂತಿರುಗಿ LDAP_SCOPE_ತಳ;
  ಆದರೆ(curl_strequal(p, "sub"))
    ಹಿಂತಿರುಗಿ LDAP_SCOPE_SUBTREE;
  ಆದರೆ(curl_strequal(p, "subtree"))
    ಹಿಂತಿರುಗಿ LDAP_SCOPE_SUBTREE;
  ಹಿಂತಿರುಗಿ -1;
}

/* number of entries in the attributes list */
ಸ್ಥಿರ size_t ಅಂಕೆ_entries(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s)
{
  size_t ವಸ್ತುಗಳು = 1;

  s = strchr(s, ',');
  ಆಗ(s) {
    ವಸ್ತುಗಳು++;
    s = strchr(s + 1, ',');
  }
  ಹಿಂತಿರುಗಿ ವಸ್ತುಗಳು;
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
ಸ್ಥಿರ curl_ldap_ಅಂಕೆ_t ldap_url_parse2_ಕೆಳಗಿನ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ಸ್ಥಿರಾಂಕ ರಚನೆ connectdata *conn,
                                           LDAPURLDesc *ludp)
{
  curl_ldap_ಅಂಕೆ_t rc = LDAP_SUCCESS;
  ಅಕ್ಷರ *p;
  ಅಕ್ಷರ *ಮಾರ್ಗ;
  ಅಕ್ಷರ *q = ಖಾಲಿ;
  ಅಕ್ಷರ *query = ಖಾಲಿ;
  size_t i;

  ಆದರೆ(!ದತ್ತಾಂಶ ||
     !ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ ||
     ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ[0] != '/' ||
     ((ದತ್ತಾಂಶ->state.origin->scheme != &Curl_scheme_ldap) &&
      (ದತ್ತಾಂಶ->state.origin->scheme != &Curl_scheme_ldaps)))
    ಹಿಂತಿರುಗಿ LDAP_INVALID_SYNTAX;

  ludp->lud_scope = LDAP_SCOPE_ತಳ;
  ludp->lud_ಪೋರ್ಟ್  = conn->origin->ಪೋರ್ಟ್;
  ludp->lud_host  = conn->origin->hostname;

  /* Duplicate the path */
  p = ಮಾರ್ಗ = curlx_strdup(ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ + 1);
  ಆದರೆ(!ಮಾರ್ಗ)
    ಹಿಂತಿರುಗಿ LDAP_NO_ಸ್ಮರಣೆ;

  /* Duplicate the query if present */
  ಆದರೆ(ದತ್ತಾಂಶ->state.ಮೇಲೆ.query) {
    q = query = curlx_strdup(ದತ್ತಾಂಶ->state.ಮೇಲೆ.query);
    ಆದರೆ(!query) {
      curlx_free(ಮಾರ್ಗ);
      ಹಿಂತಿರುಗಿ LDAP_NO_ಸ್ಮರಣೆ;
    }
  }

  /* Parse the DN (Distinguished Name) */
  ಆದರೆ(*p) {
    ಅಕ್ಷರ *dn = p;
    ಅಕ್ಷರ *unescaped;
    CURLcode ಫಲಿತಾಂಶ;

    LDAP_TRACE(("DN '%s'\n", dn));

    /* Unescape the DN */
    ಫಲಿತಾಂಶ = Curl_urldecode(dn, 0, &unescaped, ಖಾಲಿ, REJECT_ZERO);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      rc = LDAP_NO_ಸ್ಮರಣೆ;

      ಹೋಗು quit;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_dn = curlx_convert_UTF8_ಗಮ್ಯ_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    ಆದರೆ(!ludp->lud_dn) {
      rc = LDAP_NO_ಸ್ಮರಣೆ;

      ಹೋಗು quit;
    }
#ಇಲ್ಲದಿದ್ದರೆ
    ludp->lud_dn = unescaped;
#ಆದರೆ_ಅಂತ್ಯ
  }

  p = q;
  ಆದರೆ(!p)
    ಹೋಗು quit;

  /* Parse the attributes. skip "??" */
  q = strchr(p, '?');
  ಆದರೆ(q)
    *q++ = '\0';

  ಆದರೆ(*p) {
    size_t ಎಣಿಕೆ_2 = ಅಂಕೆ_entries(p); /* at least one */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *atp = p;

    /* Allocate our array (+1 for the NULL entry) */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP
    ludp->lud_attrs = curlx_calloc(ಎಣಿಕೆ_2 + 1, ಗಾತ್ರ(TCHAR *));
#ಇಲ್ಲದಿದ್ದರೆ
    ludp->lud_attrs = curlx_calloc(ಎಣಿಕೆ_2 + 1, ಗಾತ್ರ(ಅಕ್ಷರ *));
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(!ludp->lud_attrs) {
      rc = LDAP_NO_ಸ್ಮರಣೆ;
      ಹೋಗು quit;
    }

    ಗಾಗಿ(i = 0; i < ಎಣಿಕೆ_2; i++) {
      ಅಕ್ಷರ *unescaped;
      CURLcode ಫಲಿತಾಂಶ;
      ರಚನೆ Curl_str ಹೊರಹೋಗು;

      ಆದರೆ(curlx_str_until(&atp, &ಹೊರಹೋಗು, 1024, ','))
        ನಿಲ್ಲಿಸು;

      LDAP_TRACE(("attr[%zu] '%.*s'\n", i, (ಪೂರ್ಣಾಂಕ)ಹೊರಹೋಗು.len, ಹೊರಹೋಗು.str));

      /* Unescape the attribute */
      ಫಲಿತಾಂಶ = Curl_urldecode(ಹೊರಹೋಗು.str, ಹೊರಹೋಗು.len, &unescaped, ಖಾಲಿ, REJECT_ZERO);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        rc = LDAP_NO_ಸ್ಮರಣೆ;
        ಹೋಗು quit;
      }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP
      /* Convert the unescaped string to a tchar */
      ludp->lud_attrs[i] = curlx_convert_UTF8_ಗಮ್ಯ_tchar(unescaped);

      /* Free the unescaped string as we are done with it */
      curlx_free(unescaped);

      ಆದರೆ(!ludp->lud_attrs[i]) {
        rc = LDAP_NO_ಸ್ಮರಣೆ;
        ಹೋಗು quit;
      }
#ಇಲ್ಲದಿದ್ದರೆ
      ludp->lud_attrs[i] = unescaped;
#ಆದರೆ_ಅಂತ್ಯ

      ludp->lud_attrs_dups++;
      ಆದರೆ(curlx_str_single(&atp, ','))
        ನಿಲ್ಲಿಸು;
    }
  }

  p = q;
  ಆದರೆ(!p)
    ಹೋಗು quit;

  /* Parse the scope. skip "??" */
  q = strchr(p, '?');
  ಆದರೆ(q)
    *q++ = '\0';

  ಆದರೆ(*p) {
    ludp->lud_scope = str2scope(p);
    ಆದರೆ(ludp->lud_scope == -1) {
      rc = LDAP_INVALID_SYNTAX;

      ಹೋಗು quit;
    }
    LDAP_TRACE(("scope %d\n", ludp->lud_scope));
  }

  p = q;
  ಆದರೆ(!p)
    ಹೋಗು quit;

  /* Parse the filter */
  q = strchr(p, '?');
  ಆದರೆ(q)
    *q++ = '\0';

  ಆದರೆ(*p) {
    ಅಕ್ಷರ *filter = p;
    ಅಕ್ಷರ *unescaped;
    CURLcode ಫಲಿತಾಂಶ;

    LDAP_TRACE(("filter '%s'\n", filter));

    /* Unescape the filter */
    ಫಲಿತಾಂಶ = Curl_urldecode(filter, 0, &unescaped, ಖಾಲಿ, REJECT_CTRL);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      rc = LDAP_NO_ಸ್ಮರಣೆ;

      ಹೋಗು quit;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_filter = curlx_convert_UTF8_ಗಮ್ಯ_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    ಆದರೆ(!ludp->lud_filter) {
      rc = LDAP_NO_ಸ್ಮರಣೆ;

      ಹೋಗು quit;
    }
#ಇಲ್ಲದಿದ್ದರೆ
    ludp->lud_filter = unescaped;
#ಆದರೆ_ಅಂತ್ಯ
  }

  p = q;
  ಆದರೆ(p && !*p) {
    rc = LDAP_INVALID_SYNTAX;

    ಹೋಗು quit;
  }

quit:
  curlx_free(ಮಾರ್ಗ);
  curlx_free(query);

  ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ curl_ldap_ಅಂಕೆ_t ldap_url_parse_ಕೆಳಗಿನ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ಸ್ಥಿರಾಂಕ ರಚನೆ connectdata *conn,
                                          LDAPURLDesc **ludpp)
{
  LDAPURLDesc *ludp = curlx_calloc(1, ಗಾತ್ರ(*ludp));
  curl_ldap_ಅಂಕೆ_t rc;

  *ludpp = ಖಾಲಿ;
  ಆದರೆ(!ludp)
    ಹಿಂತಿರುಗಿ LDAP_NO_ಸ್ಮರಣೆ;

  rc = ldap_url_parse2_ಕೆಳಗಿನ(ದತ್ತಾಂಶ, conn, ludp);
  ಆದರೆ(rc != LDAP_SUCCESS) {
    ldap_free_urldesc_ಕೆಳಗಿನ(ludp);
    ludp = ಖಾಲಿ;
  }
  *ludpp = ludp;
  ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಶೂನ್ಯ ldap_free_urldesc_ಕೆಳಗಿನ(LDAPURLDesc *ludp)
{
  ಆದರೆ(!ludp)
    ಹಿಂತಿರುಗಿ;

  curlx_free(ludp->lud_dn);
  curlx_free(ludp->lud_filter);

  ಆದರೆ(ludp->lud_attrs) {
    size_t i;
    ಗಾಗಿ(i = 0; i < ludp->lud_attrs_dups; i++) {
      curlx_free(ludp->lud_attrs[i]);
    }
    curlx_free(ludp->lud_attrs);
  }

  curlx_free(ludp);
}
#ಆದರೆ_ಅಂತ್ಯ /* !HAVE_LDAP_URL_PARSE */

ಶೂನ್ಯ Curl_ldap_ಆವೃತ್ತಿ(ಅಕ್ಷರ *buf, size_t bufsz)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_LDAP
  curl_msnprintf(buf, bufsz, "WinLDAP");
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LDAP_OPT_X_TLS_PASSPHRASE
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ flavor[] = "/Apple";
#ಇಲ್ಲದಿದ್ದರೆ
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ flavor[] = "";
#ಆದರೆ_ಅಂತ್ಯ
  LDAPAPIInfo api;
  api.ldapai_info_ಆವೃತ್ತಿ = LDAP_API_INFO_ಆವೃತ್ತಿ;

  /* Comparing against 0, as different platforms
     disagree on the success define name */
  ಆದರೆ(ldap_get_option(ಖಾಲಿ, LDAP_OPT_API_INFO, &api) == 0) {
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ patch = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(api.ldapai_vendor_ಆವೃತ್ತಿ % 100);
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ major = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(api.ldapai_vendor_ಆವೃತ್ತಿ / 10000);
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ minor =
      (((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)api.ldapai_vendor_ಆವೃತ್ತಿ - (major * 10000))
       - patch) / 100;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __OS400__
    curl_msnprintf(buf, bufsz, "IBMLDAP/%u.%u.%u", major, minor, patch);
    ldap_ಮೌಲ್ಯ_free(api.ldapai_extensions);
    (ಶೂನ್ಯ)flavor;
#ಇಲ್ಲದಿದ್ದರೆ
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u%s",
                   api.ldapai_vendor_name, major, minor, patch, flavor);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((ಶೂನ್ಯ **)api.ldapai_extensions);
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಇಲ್ಲದಿದ್ದರೆ
    curl_msnprintf(buf, bufsz, "LDAP/1");
#ಆದರೆ_ಅಂತ್ಯ /* USE_WIN32_LDAP */
}

/*
 * LDAP protocol handler.
 */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_ldap = {
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

#ಆದರೆ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_LDAP && !USE_OPENLDAP */
