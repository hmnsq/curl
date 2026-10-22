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
#pragma GCC diagnostic افزودن
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
#وگرنه
#  define LDAP_DEPRECATED 1     /* Be sure ldap_init() is defined. */
#  ifdef HAVE_LBER_H
#    include <lber.h>
#  endif
#  include <ldap.h>
#  اگر defined(HAVE_LDAP_SSL) && defined(HAVE_LDAP_SSL_H)
#    include <ldap_ssl.h>
#  endif /* HAVE_LDAP_SSL && HAVE_LDAP_SSL_H */
#endif

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

#ifdef USE_WIN32_LDAP
#define FREE_ON_WINLDAP(x) curlx_free(x)
#define curl_ldap_رقم_t    ULONG
#وگرنه
#define FREE_ON_WINLDAP(x) انجامبده {} وقتی(0)
#define curl_ldap_رقم_t    صحیح
#endif

#ifndef HAVE_LDAP_URL_PARSE

/* Use our own implementation. */

ساختار ldap_urldesc {
  نویسه   *lud_host;
  صحیح     lud_port;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_dn;
  TCHAR **lud_attrs;
#وگرنه
  نویسه   *lud_dn;
  نویسه  **lud_attrs;
#endif
  صحیح     lud_scope;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_filter;
#وگرنه
  نویسه   *lud_filter;
#endif
  نویسه  **lud_exts;
  size_t  lud_attrs_dups; /* how many were dup'ed, this field is not in the
                             "real" struct so can only be used in code
                             without HAVE_LDAP_URL_PARSE defined */
};

#undef LDAPURLDesc
#define LDAPURLDesc ساختار ldap_urldesc

ایستا curl_ldap_رقم_t ldap_url_parse_کم(ساختار Curl_easy *data,
                                          ثابت ساختار connectdata *conn,
                                          LDAPURLDesc **ludpp);
ایستا تهی ldap_free_urldesc_کم(LDAPURLDesc *ludp);

#undef ldap_free_urldesc
#define ldap_free_urldesc       ldap_free_urldesc_کم

#endif /* !HAVE_LDAP_URL_PARSE */

#ifdef DEBUG_LDAP
#define LDAP_TRACE(x)                 \
  انجامبده {                                \
    ldap_trace_کم("%u: ", __LINE__); \
    ldap_trace_کم x;                 \
  } وقتی(0)

ایستا تهی ldap_trace_کم(ثابت نویسه *fmt, ...) CURL_PRINTF(1, 2);
#وگرنه
#define LDAP_TRACE(x) Curl_nop_stmt
#endif

#اگر defined(USE_WIN32_LDAP) && defined(ldap_err2string)
/* Use ANSI error strings in Unicode builds */
#undef ldap_err2string
#define ldap_err2string ldap_err2stringA
#endif

#اگر defined(USE_WIN32_LDAP) && defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'void *' of greater size */
#undef LDAP_OPT_ON
#undef LDAP_OPT_OFF
#define LDAP_OPT_ON   ((تهی *)(size_t)1)
#define LDAP_OPT_OFF  ((تهی *)(size_t)0)
#endif

#ifdef USE_WIN32_LDAP

#ifdef USE_WINDOWS_SSPI
ایستا ULONG ldap_win_bind_auth(LDAP *server, ثابت نویسه *user,
                                ثابت نویسه *passwd, unsigned long authflags)
{
  ULONG method = 0;
  SEC_WINNT_AUTH_IDENTITY_EX cred;
  ULONG rc = LDAP_AUTH_METHOD_NOT_SUPPORTED;

  memset(&cred, 0, اندازه(cred));

#ifdef USE_SPNEGO
  اگر(authflags & CURLAUTH_NEGOTIATE) {
    method = LDAP_AUTH_NEGOTIATE;
  }
  وگرنه
#endif
#ifdef USE_NTLM
  اگر(authflags & CURLAUTH_NTLM) {
    method = LDAP_AUTH_NTLM;
  }
  وگرنه
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  اگر(authflags & CURLAUTH_DIGEST) {
    method = LDAP_AUTH_DIGEST;
  }
  وگرنه
#endif
  {
    /* required anyway if one of upper preprocessor definitions enabled */
  }

  اگر(method && user && passwd) {
    CURLcode result = Curl_create_sspi_identity(user, passwd, &cred);
    اگر(!result) {
      rc = ldap_bind_s(server, پوچ, (TCHAR *)&cred, method);
      Curl_sspi_free_identity(&cred);
    }
    وگرنه {
      rc = LDAP_NO_MEMORY;
    }
  }
  وگرنه {
    /* proceed with current user credentials */
    method = LDAP_AUTH_NEGOTIATE;
    rc = ldap_bind_s(server, پوچ, پوچ, method);
  }
  بازگشت rc;
}
#endif /* USE_WINDOWS_SSPI */

ایستا ULONG ldap_win_bind(ساختار Curl_easy *data, LDAP *server,
                           ثابت نویسه *user, ثابت نویسه *passwd)
{
  ULONG rc = LDAP_INVALID_CREDENTIALS;

  PTCHAR inuser = پوچ;
  PTCHAR inpass = پوچ;

  اگر(user && passwd && (data->set.httpauth & CURLAUTH_BASIC)) {
    inuser = curlx_convert_UTF8_to_tchar(user);
    inpass = curlx_convert_UTF8_to_tchar(passwd);

    rc = ldap_simple_bind_s(server, inuser, inpass);

    curlx_free(inuser);
    curlx_free(inpass);
  }
#ifdef USE_WINDOWS_SSPI
  وگرنه {
    rc = ldap_win_bind_auth(server, user, passwd, data->set.httpauth);
  }
#endif

  بازگشت rc;
}
#endif /* USE_WIN32_LDAP */

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
ایستا bool ldap_str_needs_base64(ثابت نویسه *s, size_t len)
{
  size_t i;
  اگر(!len)
    بازگشت FALSE;
  اگر((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    بازگشت TRUE;
  برای(i = 0; i < len; i++)
    اگر(!ISPRINT(s[i]))
      بازگشت TRUE;
  بازگشت FALSE;
}

ایستا bool ldap_مقدار_needs_base64(ثابت نویسه *attr, size_t attr_len,
                                    ثابت Ber_مقدار *val)
{
  اگر((attr_len > 7) && curl_strequal(";binary", attr + attr_len - 7))
    بازگشت TRUE;

  بازگشت ldap_str_needs_base64(val->bv_val, val->bv_len);
}

#ifdef USE_WIN32_LDAP
ایستا BOOLEAN bypass_cert_verify(PLDAP Connection,
                                  PCCERT_CONTEXT *ppServerCert)
{
  (تهی)Connection;
  CertFreeCertificateContext(*ppServerCert);
  /* approve any certificate since the verification is set to bypass */
  بازگشت TRUE;
}
#endif

ایستا CURLcode show_vals(ساختار Curl_easy *data, Ber_مقدار **vals,
                          ثابت نویسه *attr)
{
  CURLcode result = CURLE_OK;
  صحیح i;
  size_t attr_len = strlen(attr);

  /* An attribute type has no base64 form in LDIF, so a type carrying control
     bytes (CR/LF etc.) would let a hostile server inject lines. A conformant
     type name never contains them, so reject it. */
  اگر(ldap_str_needs_base64(attr, attr_len)) {
    failf(data, "LDAP local: attribute type contains illegal bytes");
    بازگشت CURLE_WEIRD_SERVER_REPLY;
  }

  برای(i = 0; vals[i] && !result; i++) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, "\t", 1);
    اگر(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, attr, attr_len);
    اگر(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, ":", 1);

    اگر(result)
      بشکن;

    اگر(ldap_مقدار_needs_base64(attr, attr_len, vals[i])) {
      نویسه *val_b64 = پوچ;
      size_t val_b64_sz = 0;

      /* Binary attribute, encode to base64. */
      اگر(vals[i]->bv_len)
        result = curlx_base64_encode((uint8_t *)vals[i]->bv_val,
                                     vals[i]->bv_len,
                                     &val_b64, &val_b64_sz);
      اگر(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, ": ", 2);
      اگر(!result && val_b64_sz)
        result = Curl_client_write(data, CLIENTWRITE_BODY, val_b64,
                                   val_b64_sz);
      curlx_free(val_b64);
    }
    وگرنه {
      result = Curl_client_write(data, CLIENTWRITE_BODY, " ", 1);
      اگر(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   vals[i]->bv_val, vals[i]->bv_len);
    }

    اگر(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
  }
  بازگشت result;
}

ایستا CURLcode ldap_do(ساختار Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  curl_ldap_رقم_t rc = LDAP_SUCCESS;
  LDAP *server = پوچ;
  LDAPURLDesc *ludp = پوچ;
  LDAPMessage *ldapmsg = پوچ;
  LDAPMessage *entryIterator;
  صحیح رقم = 0;
  ساختار connectdata *conn = data->conn;
  صحیح ldap_proto = LDAP_VERSION3;
  bool ldap_ssl = FALSE;
#ifdef LDAP_OPT_NETWORK_TIMEOUT
  ساختار timeval ldap_timeout = { 10, 0 }; /* 10s connection/search timeout */
#endif
#ifdef USE_WIN32_LDAP
  TCHAR *host = پوچ;
#وگرنه
  نویسه *host = پوچ;
#endif
  ثابت نویسه *user = data->state.creds ? data->state.creds->user : پوچ;
  ثابت نویسه *passwd = data->state.creds ? data->state.creds->passwd : پوچ;
  ساختار ip_quadruple ipquad;
  bool is_ipv6;
  BerElement *ber = پوچ;

  *done = TRUE; /* unconditionally */
  infof(data, "LDAP local: LDAP Vendor = %s ; LDAP Version = %d",
        LDAP_VENDOR_NAME, LDAP_VENDOR_VERSION);
  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

#ifdef HAVE_LDAP_URL_PARSE
  rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), &ludp);
#وگرنه
  rc = ldap_url_parse_کم(data, conn, &ludp);
#endif
  اگر(rc) {
    failf(data, "Bad LDAP URL: %s", ldap_err2string(rc));
    result = CURLE_URL_MALFORMAT;
    برو quit;
  }

  result = Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad);
  اگر(result)
    برو quit;

  /* Get the URL scheme (either ldap or ldaps) */
  ldap_ssl = Curl_conn_is_ssl(conn, FIRSTSOCKET);
  infof(data, "LDAP local: trying to establish %s connection",
        ldap_ssl ? "encrypted" : "cleartext");

#ifdef USE_WIN32_LDAP
  host = curlx_convert_UTF8_to_tchar(conn->origin->hostname);
  اگر(!host) {
    result = CURLE_OUT_OF_MEMORY;

    برو quit;
  }
#وگرنه
  host = conn->origin->hostname;
#endif

#ifdef USE_WIN32_LDAP
  اگر(ldap_ssl)
    server = ldap_sslinit(host, (curl_ldap_رقم_t)ipquad.remote_port, 1);
  وگرنه
#endif
    server = ldap_init(host, (curl_ldap_رقم_t)ipquad.remote_port);
  اگر(!server) {
    failf(data, "LDAP: cannot setup connect to %s:%u",
          conn->origin->user_hostname, ipquad.remote_port);
    result = CURLE_COULDNT_CONNECT;
    برو quit;
  }

#ifdef LDAP_OPT_NETWORK_TIMEOUT
  ldap_set_option(server, LDAP_OPT_NETWORK_TIMEOUT, &ldap_timeout);
#endif
  ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);

  /* Do not chase referrals. */
  ldap_set_option(server, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  اگر(ldap_ssl) {
#ifdef HAVE_LDAP_SSL
#ifdef USE_WIN32_LDAP
    /* Win32 LDAP uses the Windows CA store to verify certificates */
    ldap_set_option(server, LDAP_OPT_SSL, LDAP_OPT_ON);
    اگر(!conn->ssl_config.verifypeer) {
      اگر(conn->ssl_config.verifyhost) {
        failf(data, "LDAP local: host verification cannot be enabled when "
              "peer verification is disabled for Windows native LDAP");
        result = CURLE_NOT_BUILT_IN;
        برو quit;
      }
      ldap_set_option(server, LDAP_OPT_SERVER_CERTIFICATE,
                      (تهی *)(uintptr_t)bypass_cert_verify);
    }
#وگرنهاگر defined(LDAP_OPT_X_TLS) /* !USE_WIN32_LDAP */
    صحیح ldap_option;
    ثابت نویسه *ldap_ca = conn->ssl_config.CAfile;
    اگر(conn->ssl_config.verifypeer) {
      /* OpenLDAP SDK supports BASE64 files. */
      اگر(conn->ssl_config.cert_type &&
         !curl_strequal(conn->ssl_config.cert_type, "PEM")) {
        failf(data, "LDAP local: ERROR OpenLDAP only supports PEM cert-type");
        result = CURLE_SSL_CERTPROBLEM;
        برو quit;
      }
      اگر(!ldap_ca) {
        failf(data, "LDAP local: ERROR PEM CA cert not set");
        result = CURLE_SSL_CERTPROBLEM;
        برو quit;
      }
      infof(data, "LDAP local: using PEM CA cert: %s", ldap_ca);
      rc = ldap_set_option(server, LDAP_OPT_X_TLS_CACERTFILE, ldap_ca);
      اگر(rc != LDAP_SUCCESS) {
        failf(data, "LDAP local: ERROR setting PEM CA cert: %s",
              ldap_err2string(rc));
        result = CURLE_SSL_CERTPROBLEM;
        برو quit;
      }
      ldap_option = LDAP_OPT_X_TLS_DEMAND;
    }
    وگرنه
      ldap_option = LDAP_OPT_X_TLS_NEVER;

    rc = ldap_set_option(server, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_option);
    اگر(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting cert verify mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      برو quit;
    }
    ldap_option = LDAP_OPT_X_TLS_HARD;
    rc = ldap_set_option(server, LDAP_OPT_X_TLS, &ldap_option);
    اگر(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting SSL/TLS mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      برو quit;
    }
#وگرنه /* !USE_WIN32_LDAP && !LDAP_OPT_X_TLS */
    /* we should probably never come up to here since configure
       should check in first place if we can support LDAP SSL/TLS */
    failf(data, "LDAP local: SSL/TLS not supported with this version "
          "of the OpenLDAP toolkit");
    result = CURLE_SSL_CERTPROBLEM;
    برو quit;
#endif
#endif /* HAVE_LDAP_SSL */
  }
  وگرنه اگر(data->set.use_ssl > CURLUSESSL_TRY) {
    failf(data, "LDAP local: explicit TLS not supported");
    result = CURLE_NOT_BUILT_IN;
    برو quit;
  }

#ifdef USE_WIN32_LDAP
  rc = ldap_win_bind(data, server, user, passwd);
#وگرنه
  rc = ldap_simple_bind_s(server, user, passwd);
#endif
  اگر(!ldap_ssl && rc != LDAP_SUCCESS) {
    ldap_proto = LDAP_VERSION2;
    ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);
#ifdef USE_WIN32_LDAP
    rc = ldap_win_bind(data, server, user, passwd);
#وگرنه
    rc = ldap_simple_bind_s(server, user, passwd);
#endif
  }
  اگر(rc != LDAP_SUCCESS) {
#ifdef USE_WIN32_LDAP
    failf(data, "LDAP local: bind via ldap_win_bind %s",
          ldap_err2string(rc));
#وگرنه
    failf(data, "LDAP local: bind via ldap_simple_bind_s %s",
          ldap_err2string(rc));
#endif
    result = CURLE_LDAP_CANNOT_BIND;
    برو quit;
  }

  Curl_pgrsReset(data);
  rc = ldap_جستجو_s(server, ludp->lud_dn,
                     ludp->lud_scope,
                     ludp->lud_filter, ludp->lud_attrs, 0, &ldapmsg);

  اگر(rc != LDAP_SUCCESS && rc != LDAP_SIZELIMIT_EXCEEDED) {
    failf(data, "LDAP remote: %s", ldap_err2string(rc));
    result = CURLE_LDAP_جستجو_FAILED;
    برو quit;
  }

  رقم = 0;
  برای(entryIterator = ldap_first_entry(server, ldapmsg);
      entryIterator;
      entryIterator = ldap_next_entry(server, entryIterator), رقم++) {
#ifdef USE_WIN32_LDAP
    TCHAR *attribute;
#وگرنه
    نویسه *attribute;
#endif

    /* Get the DN and write it to the client */
    {
      نویسه *name = پوچ;
      size_t name_len = 0;
#ifdef USE_WIN32_LDAP
      TCHAR *dn = ldap_get_dn(server, entryIterator);
      اگر(dn)
        name = curlx_convert_tchar_to_UTF8(dn);
#وگرنه
      نویسه *dn = name = ldap_get_dn(server, entryIterator);
#endif
      اگر(!name)
        result = dn ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
      وگرنه {
        name_len = strlen(name);
        اگر(ldap_str_needs_base64(name, name_len)) {
          نویسه *dn_b64 = پوچ;
          size_t dn_b64_sz = 0;

          /* A DN with e.g. CR/LF would inject LDIF lines, so encode it */
          result = curlx_base64_encode((uint8_t *)name, name_len,
                                       &dn_b64, &dn_b64_sz);
          اگر(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, "DN:: ", 5);
          اگر(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, dn_b64,
                                       dn_b64_sz);
          curlx_free(dn_b64);
        }
        وگرنه {
          result = Curl_client_write(data, CLIENTWRITE_BODY, "DN: ", 4);
          اگر(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, name, name_len);
        }
        اگر(!result)
          result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      }
      FREE_ON_WINLDAP(name);
      ldap_memfree(dn);
      اگر(result)
        برو quit;
    }

    /* Get the attributes and write them to the client */
    برای(attribute = ldap_first_attribute(server, entryIterator, &ber);
        attribute;
        attribute = ldap_next_attribute(server, entryIterator, ber)) {
      Ber_مقدار **vals;
#ifdef USE_WIN32_LDAP
      نویسه *attr = curlx_convert_tchar_to_UTF8(attribute);
      اگر(!attr) {
        ldap_memfree(attribute);
        result = CURLE_OUT_OF_MEMORY;
        برو quit;
      }
#وگرنه
      نویسه *attr = attribute;
#endif
      vals = ldap_get_مقادیر_len(server, entryIterator, attribute);
      اگر(vals) {
        result = show_vals(data, vals, attr);
        /* Free memory used to store values */
        ldap_مقدار_free_len(vals);
      }

      /* Free the attribute as we are done with it */
      FREE_ON_WINLDAP(attr);
      ldap_memfree(attribute);

      اگر(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      اگر(result)
        برو quit;
    }

    اگر(ber) {
      ber_free(ber, 0);
      ber = پوچ;
    }
  }

quit:
  اگر(ber)
    ber_free(ber, 0);
  اگر(ldapmsg) {
    ldap_msgfree(ldapmsg);
    LDAP_TRACE(("Received %d entries\n", رقم));
  }
  اگر(rc == LDAP_SIZELIMIT_EXCEEDED)
    infof(data, "There are more than %d entries", رقم);
  اگر(ludp)
    ldap_free_urldesc(ludp);
  اگر(server)
    ldap_unbind_s(server);

  FREE_ON_WINLDAP(host);

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  connclose(conn);

  بازگشت result;
}

#ifdef DEBUG_LDAP
ایستا تهی ldap_trace_کم(ثابت نویسه *fmt, ...)
{
  ایستا صحیح do_trace = -1;
  va_list args;

  اگر(do_trace == -1) {
    ثابت نویسه *env = getenv("CURL_TRACE");
    curl_off_t e = 0;
    اگر(!curlx_str_عدد(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  اگر(!do_trace)
    بازگشت;

  va_start(args, fmt);
  curl_mvfprintf(stderr, fmt, args);
  va_end(args);
}
#endif /* DEBUG_LDAP */

#ifndef HAVE_LDAP_URL_PARSE

/*
 * Return scope-value for a scope-string.
 */
ایستا صحیح str2scope(ثابت نویسه *p)
{
  اگر(curl_strequal(p, "one"))
    بازگشت LDAP_SCOPE_ONELEVEL;
  اگر(curl_strequal(p, "onetree"))
    بازگشت LDAP_SCOPE_ONELEVEL;
  اگر(curl_strequal(p, "base"))
    بازگشت LDAP_SCOPE_BASE;
  اگر(curl_strequal(p, "sub"))
    بازگشت LDAP_SCOPE_SUBTREE;
  اگر(curl_strequal(p, "subtree"))
    بازگشت LDAP_SCOPE_SUBTREE;
  بازگشت -1;
}

/* number of entries in the attributes list */
ایستا size_t رقم_entries(ثابت نویسه *s)
{
  size_t اقلام = 1;

  s = strchr(s, ',');
  وقتی(s) {
    اقلام++;
    s = strchr(s + 1, ',');
  }
  بازگشت اقلام;
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
ایستا curl_ldap_رقم_t ldap_url_parse2_کم(ساختار Curl_easy *data,
                                           ثابت ساختار connectdata *conn,
                                           LDAPURLDesc *ludp)
{
  curl_ldap_رقم_t rc = LDAP_SUCCESS;
  نویسه *p;
  نویسه *path;
  نویسه *q = پوچ;
  نویسه *query = پوچ;
  size_t i;

  اگر(!data ||
     !data->state.up.path ||
     data->state.up.path[0] != '/' ||
     ((data->state.origin->scheme != &Curl_scheme_ldap) &&
      (data->state.origin->scheme != &Curl_scheme_ldaps)))
    بازگشت LDAP_INVALID_SYNTAX;

  ludp->lud_scope = LDAP_SCOPE_BASE;
  ludp->lud_port  = conn->origin->port;
  ludp->lud_host  = conn->origin->hostname;

  /* Duplicate the path */
  p = path = curlx_strdup(data->state.up.path + 1);
  اگر(!path)
    بازگشت LDAP_NO_MEMORY;

  /* Duplicate the query if present */
  اگر(data->state.up.query) {
    q = query = curlx_strdup(data->state.up.query);
    اگر(!query) {
      curlx_free(path);
      بازگشت LDAP_NO_MEMORY;
    }
  }

  /* Parse the DN (Distinguished Name) */
  اگر(*p) {
    نویسه *dn = p;
    نویسه *unescaped;
    CURLcode result;

    LDAP_TRACE(("DN '%s'\n", dn));

    /* Unescape the DN */
    result = Curl_urldecode(dn, 0, &unescaped, پوچ, REJECT_ZERO);
    اگر(result) {
      rc = LDAP_NO_MEMORY;

      برو quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_dn = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    اگر(!ludp->lud_dn) {
      rc = LDAP_NO_MEMORY;

      برو quit;
    }
#وگرنه
    ludp->lud_dn = unescaped;
#endif
  }

  p = q;
  اگر(!p)
    برو quit;

  /* Parse the attributes. skip "??" */
  q = strchr(p, '?');
  اگر(q)
    *q++ = '\0';

  اگر(*p) {
    size_t شمار = رقم_entries(p); /* at least one */
    ثابت نویسه *atp = p;

    /* Allocate our array (+1 for the NULL entry) */
#ifdef USE_WIN32_LDAP
    ludp->lud_attrs = curlx_calloc(شمار + 1, اندازه(TCHAR *));
#وگرنه
    ludp->lud_attrs = curlx_calloc(شمار + 1, اندازه(نویسه *));
#endif
    اگر(!ludp->lud_attrs) {
      rc = LDAP_NO_MEMORY;
      برو quit;
    }

    برای(i = 0; i < شمار; i++) {
      نویسه *unescaped;
      CURLcode result;
      ساختار Curl_str out;

      اگر(curlx_str_until(&atp, &out, 1024, ','))
        بشکن;

      LDAP_TRACE(("attr[%zu] '%.*s'\n", i, (صحیح)out.len, out.str));

      /* Unescape the attribute */
      result = Curl_urldecode(out.str, out.len, &unescaped, پوچ, REJECT_ZERO);
      اگر(result) {
        rc = LDAP_NO_MEMORY;
        برو quit;
      }

#ifdef USE_WIN32_LDAP
      /* Convert the unescaped string to a tchar */
      ludp->lud_attrs[i] = curlx_convert_UTF8_to_tchar(unescaped);

      /* Free the unescaped string as we are done with it */
      curlx_free(unescaped);

      اگر(!ludp->lud_attrs[i]) {
        rc = LDAP_NO_MEMORY;
        برو quit;
      }
#وگرنه
      ludp->lud_attrs[i] = unescaped;
#endif

      ludp->lud_attrs_dups++;
      اگر(curlx_str_single(&atp, ','))
        بشکن;
    }
  }

  p = q;
  اگر(!p)
    برو quit;

  /* Parse the scope. skip "??" */
  q = strchr(p, '?');
  اگر(q)
    *q++ = '\0';

  اگر(*p) {
    ludp->lud_scope = str2scope(p);
    اگر(ludp->lud_scope == -1) {
      rc = LDAP_INVALID_SYNTAX;

      برو quit;
    }
    LDAP_TRACE(("scope %d\n", ludp->lud_scope));
  }

  p = q;
  اگر(!p)
    برو quit;

  /* Parse the filter */
  q = strchr(p, '?');
  اگر(q)
    *q++ = '\0';

  اگر(*p) {
    نویسه *filter = p;
    نویسه *unescaped;
    CURLcode result;

    LDAP_TRACE(("filter '%s'\n", filter));

    /* Unescape the filter */
    result = Curl_urldecode(filter, 0, &unescaped, پوچ, REJECT_CTRL);
    اگر(result) {
      rc = LDAP_NO_MEMORY;

      برو quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_filter = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    اگر(!ludp->lud_filter) {
      rc = LDAP_NO_MEMORY;

      برو quit;
    }
#وگرنه
    ludp->lud_filter = unescaped;
#endif
  }

  p = q;
  اگر(p && !*p) {
    rc = LDAP_INVALID_SYNTAX;

    برو quit;
  }

quit:
  curlx_free(path);
  curlx_free(query);

  بازگشت rc;
}

ایستا curl_ldap_رقم_t ldap_url_parse_کم(ساختار Curl_easy *data,
                                          ثابت ساختار connectdata *conn,
                                          LDAPURLDesc **ludpp)
{
  LDAPURLDesc *ludp = curlx_calloc(1, اندازه(*ludp));
  curl_ldap_رقم_t rc;

  *ludpp = پوچ;
  اگر(!ludp)
    بازگشت LDAP_NO_MEMORY;

  rc = ldap_url_parse2_کم(data, conn, ludp);
  اگر(rc != LDAP_SUCCESS) {
    ldap_free_urldesc_کم(ludp);
    ludp = پوچ;
  }
  *ludpp = ludp;
  بازگشت rc;
}

ایستا تهی ldap_free_urldesc_کم(LDAPURLDesc *ludp)
{
  اگر(!ludp)
    بازگشت;

  curlx_free(ludp->lud_dn);
  curlx_free(ludp->lud_filter);

  اگر(ludp->lud_attrs) {
    size_t i;
    برای(i = 0; i < ludp->lud_attrs_dups; i++) {
      curlx_free(ludp->lud_attrs[i]);
    }
    curlx_free(ludp->lud_attrs);
  }

  curlx_free(ludp);
}
#endif /* !HAVE_LDAP_URL_PARSE */

تهی Curl_ldap_version(نویسه *buf, size_t bufsz)
{
#ifdef USE_WIN32_LDAP
  curl_msnprintf(buf, bufsz, "WinLDAP");
#وگرنه
#ifdef LDAP_OPT_X_TLS_PASSPHRASE
  ایستا ثابت نویسه flavor[] = "/Apple";
#وگرنه
  ایستا ثابت نویسه flavor[] = "";
#endif
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  /* Comparing against 0, as different platforms
     disagree on the success define name */
  اگر(ldap_get_option(پوچ, LDAP_OPT_API_INFO, &api) == 0) {
    unsigned صحیح patch = (unsigned صحیح)(api.ldapai_vendor_version % 100);
    unsigned صحیح major = (unsigned صحیح)(api.ldapai_vendor_version / 10000);
    unsigned صحیح minor =
      (((unsigned صحیح)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
#ifdef __OS400__
    curl_msnprintf(buf, bufsz, "IBMLDAP/%u.%u.%u", major, minor, patch);
    ldap_مقدار_free(api.ldapai_extensions);
    (تهی)flavor;
#وگرنه
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u%s",
                   api.ldapai_vendor_name, major, minor, patch, flavor);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((تهی **)api.ldapai_extensions);
#endif
  }
  وگرنه
    curl_msnprintf(buf, bufsz, "LDAP/1");
#endif /* USE_WIN32_LDAP */
}

/*
 * LDAP protocol handler.
 */
ثابت ساختار Curl_protocol Curl_protocol_ldap = {
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
#pragma GCC diagnostic برداشت
#endif

#endif /* !CURL_DISABLE_LDAP && !USE_OPENLDAP */
