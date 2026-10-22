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
#include "curl_setup.h"
#include "urldata.h"
#include "curl_ldap.h"

#အကယ်၍ !defined(CURL_DISABLE_LDAP) && !defined(USE_OPENLDAP)

#အကယ်၍ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic push
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
#မဟုတ်ပါက
#  define LDAP_DEPRECATED 1     /* Be sure ldap_init() is defined. */
#  ifdef HAVE_LBER_H
#    include <lber.h>
#  endif
#  include <ldap.h>
#  အကယ်၍ defined(HAVE_LDAP_SSL) && defined(HAVE_LDAP_SSL_H)
#    include <ldap_ssl.h>
#  endif /* HAVE_LDAP_SSL && HAVE_LDAP_SSL_H */
#endif

#include "cfilters.h"
#include "sendf.h"
#include "curl_trc.h"
#include "escape.h"
#include "progress.h"
#include "transfer.h"
#include "curlx/strparse.h"
#include "bufref.h"
#include "curlx/multibyte.h"
#include "curlx/base64.h"
#include "connect.h"

#ifdef USE_WIN32_LDAP
#define FREE_ON_WINLDAP(x) curlx_free(x)
#define curl_ldap_num_t    ULONG
#မဟုတ်ပါက
#define FREE_ON_WINLDAP(x) do {} နေစဉ်(0)
#define curl_ldap_num_t    ကိန်းပြည့်
#endif

#ifndef HAVE_LDAP_URL_PARSE

/* Use our own implementation. */

sa_nracanaa ldap_urldesc {
  var_na   *lud_host;
  ကိန်းပြည့်     lud_port;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_dn;
  TCHAR **lud_attrs;
#မဟုတ်ပါက
  var_na   *lud_dn;
  var_na  **lud_attrs;
#endif
  ကိန်းပြည့်     lud_scope;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_filter;
#မဟုတ်ပါက
  var_na   *lud_filter;
#endif
  var_na  **lud_exts;
  size_t  lud_attrs_dups; /* how many were dup'ed, this field is not in the
                             "real" struct so can only be used in code
                             without HAVE_LDAP_URL_PARSE defined */
};

#undef LDAPURLDesc
#define LDAPURLDesc sa_nracanaa ldap_urldesc

static curl_ldap_num_t ldap_url_parse_low(sa_nracanaa Curl_easy *data,
                                          const sa_nracanaa connectdata *conn,
                                          LDAPURLDesc **ludpp);
static void ldap_free_urldesc_low(LDAPURLDesc *ludp);

#undef ldap_free_urldesc
#define ldap_free_urldesc       ldap_free_urldesc_low

#endif /* !HAVE_LDAP_URL_PARSE */

#ifdef DEBUG_LDAP
#define LDAP_TRACE(x)                 \
  do {                                \
    ldap_trace_low("%u: ", __LINE__); \
    ldap_trace_low x;                 \
  } နေစဉ်(0)

static void ldap_trace_low(const var_na *fmt, ...) CURL_PRINTF(1, 2);
#မဟုတ်ပါက
#define LDAP_TRACE(x) Curl_nop_stmt
#endif

#အကယ်၍ defined(USE_WIN32_LDAP) && defined(ldap_err2string)
/* Use ANSI error strings in Unicode builds */
#undef ldap_err2string
#define ldap_err2string ldap_err2stringA
#endif

#အကယ်၍ defined(USE_WIN32_LDAP) && defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'void *' of greater size */
#undef LDAP_OPT_ON
#undef LDAP_OPT_OFF
#define LDAP_OPT_ON   ((void *)(size_t)1)
#define LDAP_OPT_OFF  ((void *)(size_t)0)
#endif

#ifdef USE_WIN32_LDAP

#ifdef USE_WINDOWS_SSPI
static ULONG ldap_win_bind_auth(LDAP *server, const var_na *user,
                                const var_na *passwd, unsigned long authflags)
{
  ULONG method = 0;
  SEC_WINNT_AUTH_IDENTITY_EX cred;
  ULONG rc = LDAP_AUTH_METHOD_NOT_SUPPORTED;

  memset(&cred, 0, parimaa_na(cred));

#ifdef USE_SPNEGO
  အကယ်၍(authflags & CURLAUTH_NEGOTIATE) {
    method = LDAP_AUTH_NEGOTIATE;
  }
  မဟုတ်ပါက
#endif
#ifdef USE_NTLM
  အကယ်၍(authflags & CURLAUTH_NTLM) {
    method = LDAP_AUTH_NTLM;
  }
  မဟုတ်ပါက
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  အကယ်၍(authflags & CURLAUTH_DIGEST) {
    method = LDAP_AUTH_DIGEST;
  }
  မဟုတ်ပါက
#endif
  {
    /* required anyway if one of upper preprocessor definitions enabled */
  }

  အကယ်၍(method && user && passwd) {
    CURLcode result = Curl_create_sspi_identity(user, passwd, &cred);
    အကယ်၍(!result) {
      rc = ldap_bind_s(server, NULL, (TCHAR *)&cred, method);
      Curl_sspi_free_identity(&cred);
    }
    မဟုတ်ပါက {
      rc = LDAP_NO_MEMORY;
    }
  }
  မဟုတ်ပါက {
    /* proceed with current user credentials */
    method = LDAP_AUTH_NEGOTIATE;
    rc = ldap_bind_s(server, NULL, NULL, method);
  }
  ပြန်ပေး rc;
}
#endif /* USE_WINDOWS_SSPI */

static ULONG ldap_win_bind(sa_nracanaa Curl_easy *data, LDAP *server,
                           const var_na *user, const var_na *passwd)
{
  ULONG rc = LDAP_INVALID_CREDENTIALS;

  PTCHAR inuser = NULL;
  PTCHAR inpass = NULL;

  အကယ်၍(user && passwd && (data->set.httpauth & CURLAUTH_BASIC)) {
    inuser = curlx_convert_UTF8_to_tchar(user);
    inpass = curlx_convert_UTF8_to_tchar(passwd);

    rc = ldap_simple_bind_s(server, inuser, inpass);

    curlx_free(inuser);
    curlx_free(inpass);
  }
#ifdef USE_WINDOWS_SSPI
  မဟုတ်ပါက {
    rc = ldap_win_bind_auth(server, user, passwd, data->set.httpauth);
  }
#endif

  ပြန်ပေး rc;
}
#endif /* USE_WIN32_LDAP */

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
static bool ldap_str_needs_base64(const var_na *s, size_t len)
{
  size_t i;
  အကယ်၍(!len)
    ပြန်ပေး FALSE;
  အကယ်၍((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    ပြန်ပေး TRUE;
  အတွက်(i = 0; i < len; i++)
    အကယ်၍(!ISPRINT(s[i]))
      ပြန်ပေး TRUE;
  ပြန်ပေး FALSE;
}

static bool ldap_value_needs_base64(const var_na *attr, size_t attr_len,
                                    const BerValue *val)
{
  အကယ်၍((attr_len > 7) && curl_strequal(";binary", attr + attr_len - 7))
    ပြန်ပေး TRUE;

  ပြန်ပေး ldap_str_needs_base64(val->bv_val, val->bv_len);
}

#ifdef USE_WIN32_LDAP
static BOOLEAN bypass_cert_verify(PLDAP Connection,
                                  PCCERT_CONTEXT *ppServerCert)
{
  (void)Connection;
  CertFreeCertificateContext(*ppServerCert);
  /* approve any certificate since the verification is set to bypass */
  ပြန်ပေး TRUE;
}
#endif

static CURLcode show_vals(sa_nracanaa Curl_easy *data, BerValue **vals,
                          const var_na *attr)
{
  CURLcode result = CURLE_OK;
  ကိန်းပြည့် i;
  size_t attr_len = strlen(attr);

  /* An attribute type has no base64 form in LDIF, so a type carrying control
     bytes (CR/LF etc.) would let a hostile server inject lines. A conformant
     type name never contains them, so reject it. */
  အကယ်၍(ldap_str_needs_base64(attr, attr_len)) {
    failf(data, "LDAP local: attribute type contains illegal bytes");
    ပြန်ပေး CURLE_WEIRD_SERVER_REPLY;
  }

  အတွက်(i = 0; vals[i] && !result; i++) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, "\t", 1);
    အကယ်၍(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, attr, attr_len);
    အကယ်၍(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, ":", 1);

    အကယ်၍(result)
      bha_nga;

    အကယ်၍(ldap_value_needs_base64(attr, attr_len, vals[i])) {
      var_na *val_b64 = NULL;
      size_t val_b64_sz = 0;

      /* Binary attribute, encode to base64. */
      အကယ်၍(vals[i]->bv_len)
        result = curlx_base64_encode((uint8_t *)vals[i]->bv_val,
                                     vals[i]->bv_len,
                                     &val_b64, &val_b64_sz);
      အကယ်၍(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, ": ", 2);
      အကယ်၍(!result && val_b64_sz)
        result = Curl_client_write(data, CLIENTWRITE_BODY, val_b64,
                                   val_b64_sz);
      curlx_free(val_b64);
    }
    မဟုတ်ပါက {
      result = Curl_client_write(data, CLIENTWRITE_BODY, " ", 1);
      အကယ်၍(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   vals[i]->bv_val, vals[i]->bv_len);
    }

    အကယ်၍(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
  }
  ပြန်ပေး result;
}

static CURLcode ldap_do(sa_nracanaa Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  curl_ldap_num_t rc = LDAP_SUCCESS;
  LDAP *server = NULL;
  LDAPURLDesc *ludp = NULL;
  LDAPMessage *ldapmsg = NULL;
  LDAPMessage *entryIterator;
  ကိန်းပြည့် num = 0;
  sa_nracanaa connectdata *conn = data->conn;
  ကိန်းပြည့် ldap_proto = LDAP_VERSION3;
  bool ldap_ssl = FALSE;
#ifdef LDAP_OPT_NETWORK_TIMEOUT
  sa_nracanaa timeval ldap_timeout = { 10, 0 }; /* 10s connection/search timeout */
#endif
#ifdef USE_WIN32_LDAP
  TCHAR *host = NULL;
#မဟုတ်ပါက
  var_na *host = NULL;
#endif
  const var_na *user = data->state.creds ? data->state.creds->user : NULL;
  const var_na *passwd = data->state.creds ? data->state.creds->passwd : NULL;
  sa_nracanaa ip_quadruple ipquad;
  bool is_ipv6;
  BerElement *ber = NULL;

  *done = TRUE; /* unconditionally */
  infof(data, "LDAP local: LDAP Vendor = %s ; LDAP Version = %d",
        LDAP_VENDOR_NAME, LDAP_VENDOR_VERSION);
  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

#ifdef HAVE_LDAP_URL_PARSE
  rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), &ludp);
#မဟုတ်ပါက
  rc = ldap_url_parse_low(data, conn, &ludp);
#endif
  အကယ်၍(rc) {
    failf(data, "Bad LDAP URL: %s", ldap_err2string(rc));
    result = CURLE_URL_MALFORMAT;
    goto quit;
  }

  result = Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad);
  အကယ်၍(result)
    goto quit;

  /* Get the URL scheme (either ldap or ldaps) */
  ldap_ssl = Curl_conn_is_ssl(conn, FIRSTSOCKET);
  infof(data, "LDAP local: trying to establish %s connection",
        ldap_ssl ? "encrypted" : "cleartext");

#ifdef USE_WIN32_LDAP
  host = curlx_convert_UTF8_to_tchar(conn->origin->hostname);
  အကယ်၍(!host) {
    result = CURLE_OUT_OF_MEMORY;

    goto quit;
  }
#မဟုတ်ပါက
  host = conn->origin->hostname;
#endif

#ifdef USE_WIN32_LDAP
  အကယ်၍(ldap_ssl)
    server = ldap_sslinit(host, (curl_ldap_num_t)ipquad.remote_port, 1);
  မဟုတ်ပါက
#endif
    server = ldap_init(host, (curl_ldap_num_t)ipquad.remote_port);
  အကယ်၍(!server) {
    failf(data, "LDAP: cannot setup connect to %s:%u",
          conn->origin->user_hostname, ipquad.remote_port);
    result = CURLE_COULDNT_CONNECT;
    goto quit;
  }

#ifdef LDAP_OPT_NETWORK_TIMEOUT
  ldap_set_option(server, LDAP_OPT_NETWORK_TIMEOUT, &ldap_timeout);
#endif
  ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);

  /* Do not chase referrals. */
  ldap_set_option(server, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  အကယ်၍(ldap_ssl) {
#ifdef HAVE_LDAP_SSL
#ifdef USE_WIN32_LDAP
    /* Win32 LDAP uses the Windows CA store to verify certificates */
    ldap_set_option(server, LDAP_OPT_SSL, LDAP_OPT_ON);
    အကယ်၍(!conn->ssl_config.verifypeer) {
      အကယ်၍(conn->ssl_config.verifyhost) {
        failf(data, "LDAP local: host verification cannot be enabled when "
              "peer verification is disabled for Windows native LDAP");
        result = CURLE_NOT_BUILT_IN;
        goto quit;
      }
      ldap_set_option(server, LDAP_OPT_SERVER_CERTIFICATE,
                      (void *)(uintptr_t)bypass_cert_verify);
    }
#elif defined(LDAP_OPT_X_TLS) /* !USE_WIN32_LDAP */
    ကိန်းပြည့် ldap_option;
    const var_na *ldap_ca = conn->ssl_config.CAfile;
    အကယ်၍(conn->ssl_config.verifypeer) {
      /* OpenLDAP SDK supports BASE64 files. */
      အကယ်၍(conn->ssl_config.cert_type &&
         !curl_strequal(conn->ssl_config.cert_type, "PEM")) {
        failf(data, "LDAP local: ERROR OpenLDAP only supports PEM cert-type");
        result = CURLE_SSL_CERTPROBLEM;
        goto quit;
      }
      အကယ်၍(!ldap_ca) {
        failf(data, "LDAP local: ERROR PEM CA cert not set");
        result = CURLE_SSL_CERTPROBLEM;
        goto quit;
      }
      infof(data, "LDAP local: using PEM CA cert: %s", ldap_ca);
      rc = ldap_set_option(server, LDAP_OPT_X_TLS_CACERTFILE, ldap_ca);
      အကယ်၍(rc != LDAP_SUCCESS) {
        failf(data, "LDAP local: ERROR setting PEM CA cert: %s",
              ldap_err2string(rc));
        result = CURLE_SSL_CERTPROBLEM;
        goto quit;
      }
      ldap_option = LDAP_OPT_X_TLS_DEMAND;
    }
    မဟုတ်ပါက
      ldap_option = LDAP_OPT_X_TLS_NEVER;

    rc = ldap_set_option(server, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_option);
    အကယ်၍(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting cert verify mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      goto quit;
    }
    ldap_option = LDAP_OPT_X_TLS_HARD;
    rc = ldap_set_option(server, LDAP_OPT_X_TLS, &ldap_option);
    အကယ်၍(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting SSL/TLS mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      goto quit;
    }
#မဟုတ်ပါက /* !USE_WIN32_LDAP && !LDAP_OPT_X_TLS */
    /* we should probably never come up to here since configure
       should check in first place if we can support LDAP SSL/TLS */
    failf(data, "LDAP local: SSL/TLS not supported with this version "
          "of the OpenLDAP toolkit");
    result = CURLE_SSL_CERTPROBLEM;
    goto quit;
#endif
#endif /* HAVE_LDAP_SSL */
  }
  မဟုတ်ပါက အကယ်၍(data->set.use_ssl > CURLUSESSL_TRY) {
    failf(data, "LDAP local: explicit TLS not supported");
    result = CURLE_NOT_BUILT_IN;
    goto quit;
  }

#ifdef USE_WIN32_LDAP
  rc = ldap_win_bind(data, server, user, passwd);
#မဟုတ်ပါက
  rc = ldap_simple_bind_s(server, user, passwd);
#endif
  အကယ်၍(!ldap_ssl && rc != LDAP_SUCCESS) {
    ldap_proto = LDAP_VERSION2;
    ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);
#ifdef USE_WIN32_LDAP
    rc = ldap_win_bind(data, server, user, passwd);
#မဟုတ်ပါက
    rc = ldap_simple_bind_s(server, user, passwd);
#endif
  }
  အကယ်၍(rc != LDAP_SUCCESS) {
#ifdef USE_WIN32_LDAP
    failf(data, "LDAP local: bind via ldap_win_bind %s",
          ldap_err2string(rc));
#မဟုတ်ပါက
    failf(data, "LDAP local: bind via ldap_simple_bind_s %s",
          ldap_err2string(rc));
#endif
    result = CURLE_LDAP_CANNOT_BIND;
    goto quit;
  }

  Curl_pgrsReset(data);
  rc = ldap_search_s(server, ludp->lud_dn,
                     ludp->lud_scope,
                     ludp->lud_filter, ludp->lud_attrs, 0, &ldapmsg);

  အကယ်၍(rc != LDAP_SUCCESS && rc != LDAP_SIZELIMIT_EXCEEDED) {
    failf(data, "LDAP remote: %s", ldap_err2string(rc));
    result = CURLE_LDAP_SEARCH_FAILED;
    goto quit;
  }

  num = 0;
  အတွက်(entryIterator = ldap_first_entry(server, ldapmsg);
      entryIterator;
      entryIterator = ldap_next_entry(server, entryIterator), num++) {
#ifdef USE_WIN32_LDAP
    TCHAR *attribute;
#မဟုတ်ပါက
    var_na *attribute;
#endif

    /* Get the DN and write it to the client */
    {
      var_na *name = NULL;
      size_t name_len = 0;
#ifdef USE_WIN32_LDAP
      TCHAR *dn = ldap_get_dn(server, entryIterator);
      အကယ်၍(dn)
        name = curlx_convert_tchar_to_UTF8(dn);
#မဟုတ်ပါက
      var_na *dn = name = ldap_get_dn(server, entryIterator);
#endif
      အကယ်၍(!name)
        result = dn ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
      မဟုတ်ပါက {
        name_len = strlen(name);
        အကယ်၍(ldap_str_needs_base64(name, name_len)) {
          var_na *dn_b64 = NULL;
          size_t dn_b64_sz = 0;

          /* A DN with e.g. CR/LF would inject LDIF lines, so encode it */
          result = curlx_base64_encode((uint8_t *)name, name_len,
                                       &dn_b64, &dn_b64_sz);
          အကယ်၍(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, "DN:: ", 5);
          အကယ်၍(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, dn_b64,
                                       dn_b64_sz);
          curlx_free(dn_b64);
        }
        မဟုတ်ပါက {
          result = Curl_client_write(data, CLIENTWRITE_BODY, "DN: ", 4);
          အကယ်၍(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, name, name_len);
        }
        အကယ်၍(!result)
          result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      }
      FREE_ON_WINLDAP(name);
      ldap_memfree(dn);
      အကယ်၍(result)
        goto quit;
    }

    /* Get the attributes and write them to the client */
    အတွက်(attribute = ldap_first_attribute(server, entryIterator, &ber);
        attribute;
        attribute = ldap_next_attribute(server, entryIterator, ber)) {
      BerValue **vals;
#ifdef USE_WIN32_LDAP
      var_na *attr = curlx_convert_tchar_to_UTF8(attribute);
      အကယ်၍(!attr) {
        ldap_memfree(attribute);
        result = CURLE_OUT_OF_MEMORY;
        goto quit;
      }
#မဟုတ်ပါက
      var_na *attr = attribute;
#endif
      vals = ldap_get_values_len(server, entryIterator, attribute);
      အကယ်၍(vals) {
        result = show_vals(data, vals, attr);
        /* Free memory used to store values */
        ldap_value_free_len(vals);
      }

      /* Free the attribute as we are done with it */
      FREE_ON_WINLDAP(attr);
      ldap_memfree(attribute);

      အကယ်၍(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      အကယ်၍(result)
        goto quit;
    }

    အကယ်၍(ber) {
      ber_free(ber, 0);
      ber = NULL;
    }
  }

quit:
  အကယ်၍(ber)
    ber_free(ber, 0);
  အကယ်၍(ldapmsg) {
    ldap_msgfree(ldapmsg);
    LDAP_TRACE(("Received %d entries\n", num));
  }
  အကယ်၍(rc == LDAP_SIZELIMIT_EXCEEDED)
    infof(data, "There are more than %d entries", num);
  အကယ်၍(ludp)
    ldap_free_urldesc(ludp);
  အကယ်၍(server)
    ldap_unbind_s(server);

  FREE_ON_WINLDAP(host);

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  connclose(conn);

  ပြန်ပေး result;
}

#ifdef DEBUG_LDAP
static void ldap_trace_low(const var_na *fmt, ...)
{
  static ကိန်းပြည့် do_trace = -1;
  va_list args;

  အကယ်၍(do_trace == -1) {
    const var_na *env = getenv("CURL_TRACE");
    curl_off_t e = 0;
    အကယ်၍(!curlx_str_number(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  အကယ်၍(!do_trace)
    ပြန်ပေး;

  va_start(args, fmt);
  curl_mvfprintf(stderr, fmt, args);
  va_end(args);
}
#endif /* DEBUG_LDAP */

#ifndef HAVE_LDAP_URL_PARSE

/*
 * Return scope-value for a scope-string.
 */
static ကိန်းပြည့် str2scope(const var_na *p)
{
  အကယ်၍(curl_strequal(p, "one"))
    ပြန်ပေး LDAP_SCOPE_ONELEVEL;
  အကယ်၍(curl_strequal(p, "onetree"))
    ပြန်ပေး LDAP_SCOPE_ONELEVEL;
  အကယ်၍(curl_strequal(p, "base"))
    ပြန်ပေး LDAP_SCOPE_BASE;
  အကယ်၍(curl_strequal(p, "sub"))
    ပြန်ပေး LDAP_SCOPE_SUBTREE;
  အကယ်၍(curl_strequal(p, "subtree"))
    ပြန်ပေး LDAP_SCOPE_SUBTREE;
  ပြန်ပေး -1;
}

/* number of entries in the attributes list */
static size_t num_entries(const var_na *s)
{
  size_t items = 1;

  s = strchr(s, ',');
  နေစဉ်(s) {
    items++;
    s = strchr(s + 1, ',');
  }
  ပြန်ပေး items;
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
static curl_ldap_num_t ldap_url_parse2_low(sa_nracanaa Curl_easy *data,
                                           const sa_nracanaa connectdata *conn,
                                           LDAPURLDesc *ludp)
{
  curl_ldap_num_t rc = LDAP_SUCCESS;
  var_na *p;
  var_na *path;
  var_na *q = NULL;
  var_na *query = NULL;
  size_t i;

  အကယ်၍(!data ||
     !data->state.up.path ||
     data->state.up.path[0] != '/' ||
     ((data->state.origin->scheme != &Curl_scheme_ldap) &&
      (data->state.origin->scheme != &Curl_scheme_ldaps)))
    ပြန်ပေး LDAP_INVALID_SYNTAX;

  ludp->lud_scope = LDAP_SCOPE_BASE;
  ludp->lud_port  = conn->origin->port;
  ludp->lud_host  = conn->origin->hostname;

  /* Duplicate the path */
  p = path = curlx_strdup(data->state.up.path + 1);
  အကယ်၍(!path)
    ပြန်ပေး LDAP_NO_MEMORY;

  /* Duplicate the query if present */
  အကယ်၍(data->state.up.query) {
    q = query = curlx_strdup(data->state.up.query);
    အကယ်၍(!query) {
      curlx_free(path);
      ပြန်ပေး LDAP_NO_MEMORY;
    }
  }

  /* Parse the DN (Distinguished Name) */
  အကယ်၍(*p) {
    var_na *dn = p;
    var_na *unescaped;
    CURLcode result;

    LDAP_TRACE(("DN '%s'\n", dn));

    /* Unescape the DN */
    result = Curl_urldecode(dn, 0, &unescaped, NULL, REJECT_ZERO);
    အကယ်၍(result) {
      rc = LDAP_NO_MEMORY;

      goto quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_dn = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    အကယ်၍(!ludp->lud_dn) {
      rc = LDAP_NO_MEMORY;

      goto quit;
    }
#မဟုတ်ပါက
    ludp->lud_dn = unescaped;
#endif
  }

  p = q;
  အကယ်၍(!p)
    goto quit;

  /* Parse the attributes. skip "??" */
  q = strchr(p, '?');
  အကယ်၍(q)
    *q++ = '\0';

  အကယ်၍(*p) {
    size_t count = num_entries(p); /* at least one */
    const var_na *atp = p;

    /* Allocate our array (+1 for the NULL entry) */
#ifdef USE_WIN32_LDAP
    ludp->lud_attrs = curlx_calloc(count + 1, parimaa_na(TCHAR *));
#မဟုတ်ပါက
    ludp->lud_attrs = curlx_calloc(count + 1, parimaa_na(var_na *));
#endif
    အကယ်၍(!ludp->lud_attrs) {
      rc = LDAP_NO_MEMORY;
      goto quit;
    }

    အတွက်(i = 0; i < count; i++) {
      var_na *unescaped;
      CURLcode result;
      sa_nracanaa Curl_str out;

      အကယ်၍(curlx_str_until(&atp, &out, 1024, ','))
        bha_nga;

      LDAP_TRACE(("attr[%zu] '%.*s'\n", i, (ကိန်းပြည့်)out.len, out.str));

      /* Unescape the attribute */
      result = Curl_urldecode(out.str, out.len, &unescaped, NULL, REJECT_ZERO);
      အကယ်၍(result) {
        rc = LDAP_NO_MEMORY;
        goto quit;
      }

#ifdef USE_WIN32_LDAP
      /* Convert the unescaped string to a tchar */
      ludp->lud_attrs[i] = curlx_convert_UTF8_to_tchar(unescaped);

      /* Free the unescaped string as we are done with it */
      curlx_free(unescaped);

      အကယ်၍(!ludp->lud_attrs[i]) {
        rc = LDAP_NO_MEMORY;
        goto quit;
      }
#မဟုတ်ပါက
      ludp->lud_attrs[i] = unescaped;
#endif

      ludp->lud_attrs_dups++;
      အကယ်၍(curlx_str_single(&atp, ','))
        bha_nga;
    }
  }

  p = q;
  အကယ်၍(!p)
    goto quit;

  /* Parse the scope. skip "??" */
  q = strchr(p, '?');
  အကယ်၍(q)
    *q++ = '\0';

  အကယ်၍(*p) {
    ludp->lud_scope = str2scope(p);
    အကယ်၍(ludp->lud_scope == -1) {
      rc = LDAP_INVALID_SYNTAX;

      goto quit;
    }
    LDAP_TRACE(("scope %d\n", ludp->lud_scope));
  }

  p = q;
  အကယ်၍(!p)
    goto quit;

  /* Parse the filter */
  q = strchr(p, '?');
  အကယ်၍(q)
    *q++ = '\0';

  အကယ်၍(*p) {
    var_na *filter = p;
    var_na *unescaped;
    CURLcode result;

    LDAP_TRACE(("filter '%s'\n", filter));

    /* Unescape the filter */
    result = Curl_urldecode(filter, 0, &unescaped, NULL, REJECT_CTRL);
    အကယ်၍(result) {
      rc = LDAP_NO_MEMORY;

      goto quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_filter = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    အကယ်၍(!ludp->lud_filter) {
      rc = LDAP_NO_MEMORY;

      goto quit;
    }
#မဟုတ်ပါက
    ludp->lud_filter = unescaped;
#endif
  }

  p = q;
  အကယ်၍(p && !*p) {
    rc = LDAP_INVALID_SYNTAX;

    goto quit;
  }

quit:
  curlx_free(path);
  curlx_free(query);

  ပြန်ပေး rc;
}

static curl_ldap_num_t ldap_url_parse_low(sa_nracanaa Curl_easy *data,
                                          const sa_nracanaa connectdata *conn,
                                          LDAPURLDesc **ludpp)
{
  LDAPURLDesc *ludp = curlx_calloc(1, parimaa_na(*ludp));
  curl_ldap_num_t rc;

  *ludpp = NULL;
  အကယ်၍(!ludp)
    ပြန်ပေး LDAP_NO_MEMORY;

  rc = ldap_url_parse2_low(data, conn, ludp);
  အကယ်၍(rc != LDAP_SUCCESS) {
    ldap_free_urldesc_low(ludp);
    ludp = NULL;
  }
  *ludpp = ludp;
  ပြန်ပေး rc;
}

static void ldap_free_urldesc_low(LDAPURLDesc *ludp)
{
  အကယ်၍(!ludp)
    ပြန်ပေး;

  curlx_free(ludp->lud_dn);
  curlx_free(ludp->lud_filter);

  အကယ်၍(ludp->lud_attrs) {
    size_t i;
    အတွက်(i = 0; i < ludp->lud_attrs_dups; i++) {
      curlx_free(ludp->lud_attrs[i]);
    }
    curlx_free(ludp->lud_attrs);
  }

  curlx_free(ludp);
}
#endif /* !HAVE_LDAP_URL_PARSE */

void Curl_ldap_version(var_na *buf, size_t bufsz)
{
#ifdef USE_WIN32_LDAP
  curl_msnprintf(buf, bufsz, "WinLDAP");
#မဟုတ်ပါက
#ifdef LDAP_OPT_X_TLS_PASSPHRASE
  static const var_na flavor[] = "/Apple";
#မဟုတ်ပါက
  static const var_na flavor[] = "";
#endif
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  /* Comparing against 0, as different platforms
     disagree on the success define name */
  အကယ်၍(ldap_get_option(NULL, LDAP_OPT_API_INFO, &api) == 0) {
    unsigned ကိန်းပြည့် patch = (unsigned ကိန်းပြည့်)(api.ldapai_vendor_version % 100);
    unsigned ကိန်းပြည့် major = (unsigned ကိန်းပြည့်)(api.ldapai_vendor_version / 10000);
    unsigned ကိန်းပြည့် minor =
      (((unsigned ကိန်းပြည့်)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
#ifdef __OS400__
    curl_msnprintf(buf, bufsz, "IBMLDAP/%u.%u.%u", major, minor, patch);
    ldap_value_free(api.ldapai_extensions);
    (void)flavor;
#မဟုတ်ပါက
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u%s",
                   api.ldapai_vendor_name, major, minor, patch, flavor);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((void **)api.ldapai_extensions);
#endif
  }
  မဟုတ်ပါက
    curl_msnprintf(buf, bufsz, "LDAP/1");
#endif /* USE_WIN32_LDAP */
}

/*
 * LDAP protocol handler.
 */
const sa_nracanaa Curl_protocol Curl_protocol_ldap = {
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

#အကယ်၍ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic pop
#endif

#endif /* !CURL_DISABLE_LDAP && !USE_OPENLDAP */
