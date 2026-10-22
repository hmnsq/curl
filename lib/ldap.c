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
#pragma GCC diagnostic ئىتتىرىش
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
#وإلا
#  define LDAP_DEPRECATED 1     /* Be sure ldap_init() is defined. */
#  ifdef HAVE_LBER_H
#    include <lber.h>
#  endif
#  include <ldap.h>
#  إذا defined(HAVE_LDAP_SSL) && defined(HAVE_LDAP_SSL_H)
#    include <ldap_ssl.h>
#  endif /* HAVE_LDAP_SSL && HAVE_LDAP_SSL_H */
#endif

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

#ifdef USE_WIN32_LDAP
#define FREE_ON_WINLDAP(x) curlx_free(x)
#define curl_ldap_رەقەم_t    ULONG
#وإلا
#define FREE_ON_WINLDAP(x) افعل {} طالما(0)
#define curl_ldap_رەقەم_t    صحيح
#endif

#ifndef HAVE_LDAP_URL_PARSE

/* Use our own implementation. */

بنية ldap_urldesc {
  حرف   *lud_host;
  صحيح     lud_port;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_dn;
  TCHAR **lud_attrs;
#وإلا
  حرف   *lud_dn;
  حرف  **lud_attrs;
#endif
  صحيح     lud_scope;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_filter;
#وإلا
  حرف   *lud_filter;
#endif
  حرف  **lud_exts;
  size_t  lud_attrs_dups; /* how many were dup'ed, this field is not in the
                             "real" struct so can only be used in code
                             without HAVE_LDAP_URL_PARSE defined */
};

#undef LDAPURLDesc
#define LDAPURLDesc بنية ldap_urldesc

ساكن curl_ldap_رەقەم_t ldap_url_parse_تۆۋەن(بنية Curl_easy *data,
                                          ثابت بنية connectdata *conn,
                                          LDAPURLDesc **ludpp);
ساكن فراغ ldap_free_urldesc_تۆۋەن(LDAPURLDesc *ludp);

#undef ldap_free_urldesc
#define ldap_free_urldesc       ldap_free_urldesc_تۆۋەن

#endif /* !HAVE_LDAP_URL_PARSE */

#ifdef DEBUG_LDAP
#define LDAP_TRACE(x)                 \
  افعل {                                \
    ldap_trace_تۆۋەن("%u: ", __LINE__); \
    ldap_trace_تۆۋەن x;                 \
  } طالما(0)

ساكن فراغ ldap_trace_تۆۋەن(ثابت حرف *fmt, ...) CURL_PRINTF(1, 2);
#وإلا
#define LDAP_TRACE(x) Curl_nop_stmt
#endif

#إذا defined(USE_WIN32_LDAP) && defined(ldap_err2string)
/* Use ANSI error strings in Unicode builds */
#undef ldap_err2string
#define ldap_err2string ldap_err2stringA
#endif

#إذا defined(USE_WIN32_LDAP) && defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'void *' of greater size */
#undef LDAP_OPT_ON
#undef LDAP_OPT_OFF
#define LDAP_OPT_ON   ((فراغ *)(size_t)1)
#define LDAP_OPT_OFF  ((فراغ *)(size_t)0)
#endif

#ifdef USE_WIN32_LDAP

#ifdef USE_WINDOWS_SSPI
ساكن ULONG ldap_win_bind_auth(LDAP *server, ثابت حرف *user,
                                ثابت حرف *passwd, unsigned long authflags)
{
  ULONG method = 0;
  SEC_WINNT_AUTH_IDENTITY_EX cred;
  ULONG rc = LDAP_AUTH_METHOD_NOT_SUPPORTED;

  memset(&cred, 0, حجم(cred));

#ifdef USE_SPNEGO
  إذا(authflags & CURLAUTH_NEGOTIATE) {
    method = LDAP_AUTH_NEGOTIATE;
  }
  وإلا
#endif
#ifdef USE_NTLM
  إذا(authflags & CURLAUTH_NTLM) {
    method = LDAP_AUTH_NTLM;
  }
  وإلا
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  إذا(authflags & CURLAUTH_DIGEST) {
    method = LDAP_AUTH_DIGEST;
  }
  وإلا
#endif
  {
    /* required anyway if one of upper preprocessor definitions enabled */
  }

  إذا(method && user && passwd) {
    CURLcode result = Curl_create_sspi_identity(user, passwd, &cred);
    إذا(!result) {
      rc = ldap_bind_s(server, عدم, (TCHAR *)&cred, method);
      Curl_sspi_free_identity(&cred);
    }
    وإلا {
      rc = LDAP_NO_MEMORY;
    }
  }
  وإلا {
    /* proceed with current user credentials */
    method = LDAP_AUTH_NEGOTIATE;
    rc = ldap_bind_s(server, عدم, عدم, method);
  }
  أرجع rc;
}
#endif /* USE_WINDOWS_SSPI */

ساكن ULONG ldap_win_bind(بنية Curl_easy *data, LDAP *server,
                           ثابت حرف *user, ثابت حرف *passwd)
{
  ULONG rc = LDAP_INVALID_CREDENTIALS;

  PTCHAR inuser = عدم;
  PTCHAR inpass = عدم;

  إذا(user && passwd && (data->set.httpauth & CURLAUTH_BASIC)) {
    inuser = curlx_convert_UTF8_to_tchar(user);
    inpass = curlx_convert_UTF8_to_tchar(passwd);

    rc = ldap_simple_bind_s(server, inuser, inpass);

    curlx_free(inuser);
    curlx_free(inpass);
  }
#ifdef USE_WINDOWS_SSPI
  وإلا {
    rc = ldap_win_bind_auth(server, user, passwd, data->set.httpauth);
  }
#endif

  أرجع rc;
}
#endif /* USE_WIN32_LDAP */

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

ساكن bool ldap_قىممەت_needs_base64(ثابت حرف *attr, size_t attr_len,
                                    ثابت Ber_قىممەت *val)
{
  إذا((attr_len > 7) && curl_strequal(";binary", attr + attr_len - 7))
    أرجع TRUE;

  أرجع ldap_str_needs_base64(val->bv_val, val->bv_len);
}

#ifdef USE_WIN32_LDAP
ساكن BOOLEAN bypass_cert_verify(PLDAP Connection,
                                  PCCERT_CONTEXT *ppServerCert)
{
  (فراغ)Connection;
  CertFreeCertificateContext(*ppServerCert);
  /* approve any certificate since the verification is set to bypass */
  أرجع TRUE;
}
#endif

ساكن CURLcode show_vals(بنية Curl_easy *data, Ber_قىممەت **vals,
                          ثابت حرف *attr)
{
  CURLcode result = CURLE_OK;
  صحيح i;
  size_t attr_len = strlen(attr);

  /* An attribute type has no base64 form in LDIF, so a type carrying control
     bytes (CR/LF etc.) would let a hostile server inject lines. A conformant
     type name never contains them, so reject it. */
  إذا(ldap_str_needs_base64(attr, attr_len)) {
    failf(data, "LDAP local: attribute type contains illegal bytes");
    أرجع CURLE_WEIRD_SERVER_REPLY;
  }

  لكل(i = 0; vals[i] && !result; i++) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, "\t", 1);
    إذا(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, attr, attr_len);
    إذا(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, ":", 1);

    إذا(result)
      اكسر;

    إذا(ldap_قىممەت_needs_base64(attr, attr_len, vals[i])) {
      حرف *val_b64 = عدم;
      size_t val_b64_sz = 0;

      /* Binary attribute, encode to base64. */
      إذا(vals[i]->bv_len)
        result = curlx_base64_encode((uint8_t *)vals[i]->bv_val,
                                     vals[i]->bv_len,
                                     &val_b64, &val_b64_sz);
      إذا(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, ": ", 2);
      إذا(!result && val_b64_sz)
        result = Curl_client_write(data, CLIENTWRITE_BODY, val_b64,
                                   val_b64_sz);
      curlx_free(val_b64);
    }
    وإلا {
      result = Curl_client_write(data, CLIENTWRITE_BODY, " ", 1);
      إذا(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   vals[i]->bv_val, vals[i]->bv_len);
    }

    إذا(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
  }
  أرجع result;
}

ساكن CURLcode ldap_do(بنية Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  curl_ldap_رەقەم_t rc = LDAP_SUCCESS;
  LDAP *server = عدم;
  LDAPURLDesc *ludp = عدم;
  LDAPMessage *ldapmsg = عدم;
  LDAPMessage *entryIterator;
  صحيح رەقەم = 0;
  بنية connectdata *conn = data->conn;
  صحيح ldap_proto = LDAP_VERSION3;
  bool ldap_ssl = FALSE;
#ifdef LDAP_OPT_NETWORK_TIMEOUT
  بنية timeval ldap_timeout = { 10, 0 }; /* 10s connection/search timeout */
#endif
#ifdef USE_WIN32_LDAP
  TCHAR *host = عدم;
#وإلا
  حرف *host = عدم;
#endif
  ثابت حرف *user = data->state.creds ? data->state.creds->user : عدم;
  ثابت حرف *passwd = data->state.creds ? data->state.creds->passwd : عدم;
  بنية ip_quadruple ipquad;
  bool is_ipv6;
  BerElement *ber = عدم;

  *done = TRUE; /* unconditionally */
  infof(data, "LDAP local: LDAP Vendor = %s ; LDAP Version = %d",
        LDAP_VENDOR_NAME, LDAP_VENDOR_VERSION);
  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

#ifdef HAVE_LDAP_URL_PARSE
  rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), &ludp);
#وإلا
  rc = ldap_url_parse_تۆۋەن(data, conn, &ludp);
#endif
  إذا(rc) {
    failf(data, "Bad LDAP URL: %s", ldap_err2string(rc));
    result = CURLE_URL_MALFORMAT;
    اذهب quit;
  }

  result = Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad);
  إذا(result)
    اذهب quit;

  /* Get the URL scheme (either ldap or ldaps) */
  ldap_ssl = Curl_conn_is_ssl(conn, FIRSTSOCKET);
  infof(data, "LDAP local: trying to establish %s connection",
        ldap_ssl ? "encrypted" : "cleartext");

#ifdef USE_WIN32_LDAP
  host = curlx_convert_UTF8_to_tchar(conn->origin->hostname);
  إذا(!host) {
    result = CURLE_OUT_OF_MEMORY;

    اذهب quit;
  }
#وإلا
  host = conn->origin->hostname;
#endif

#ifdef USE_WIN32_LDAP
  إذا(ldap_ssl)
    server = ldap_sslinit(host, (curl_ldap_رەقەم_t)ipquad.remote_port, 1);
  وإلا
#endif
    server = ldap_init(host, (curl_ldap_رەقەم_t)ipquad.remote_port);
  إذا(!server) {
    failf(data, "LDAP: cannot setup connect to %s:%u",
          conn->origin->user_hostname, ipquad.remote_port);
    result = CURLE_COULDNT_CONNECT;
    اذهب quit;
  }

#ifdef LDAP_OPT_NETWORK_TIMEOUT
  ldap_set_option(server, LDAP_OPT_NETWORK_TIMEOUT, &ldap_timeout);
#endif
  ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);

  /* Do not chase referrals. */
  ldap_set_option(server, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  إذا(ldap_ssl) {
#ifdef HAVE_LDAP_SSL
#ifdef USE_WIN32_LDAP
    /* Win32 LDAP uses the Windows CA store to verify certificates */
    ldap_set_option(server, LDAP_OPT_SSL, LDAP_OPT_ON);
    إذا(!conn->ssl_config.verifypeer) {
      إذا(conn->ssl_config.verifyhost) {
        failf(data, "LDAP local: host verification cannot be enabled when "
              "peer verification is disabled for Windows native LDAP");
        result = CURLE_NOT_BUILT_IN;
        اذهب quit;
      }
      ldap_set_option(server, LDAP_OPT_SERVER_CERTIFICATE,
                      (فراغ *)(uintptr_t)bypass_cert_verify);
    }
#وإلاإذا defined(LDAP_OPT_X_TLS) /* !USE_WIN32_LDAP */
    صحيح ldap_option;
    ثابت حرف *ldap_ca = conn->ssl_config.CAfile;
    إذا(conn->ssl_config.verifypeer) {
      /* OpenLDAP SDK supports BASE64 files. */
      إذا(conn->ssl_config.cert_type &&
         !curl_strequal(conn->ssl_config.cert_type, "PEM")) {
        failf(data, "LDAP local: ERROR OpenLDAP only supports PEM cert-type");
        result = CURLE_SSL_CERTPROBLEM;
        اذهب quit;
      }
      إذا(!ldap_ca) {
        failf(data, "LDAP local: ERROR PEM CA cert not set");
        result = CURLE_SSL_CERTPROBLEM;
        اذهب quit;
      }
      infof(data, "LDAP local: using PEM CA cert: %s", ldap_ca);
      rc = ldap_set_option(server, LDAP_OPT_X_TLS_CACERTFILE, ldap_ca);
      إذا(rc != LDAP_SUCCESS) {
        failf(data, "LDAP local: ERROR setting PEM CA cert: %s",
              ldap_err2string(rc));
        result = CURLE_SSL_CERTPROBLEM;
        اذهب quit;
      }
      ldap_option = LDAP_OPT_X_TLS_DEMAND;
    }
    وإلا
      ldap_option = LDAP_OPT_X_TLS_NEVER;

    rc = ldap_set_option(server, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_option);
    إذا(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting cert verify mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      اذهب quit;
    }
    ldap_option = LDAP_OPT_X_TLS_HARD;
    rc = ldap_set_option(server, LDAP_OPT_X_TLS, &ldap_option);
    إذا(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting SSL/TLS mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      اذهب quit;
    }
#وإلا /* !USE_WIN32_LDAP && !LDAP_OPT_X_TLS */
    /* we should probably never come up to here since configure
       should check in first place if we can support LDAP SSL/TLS */
    failf(data, "LDAP local: SSL/TLS not supported with this version "
          "of the OpenLDAP toolkit");
    result = CURLE_SSL_CERTPROBLEM;
    اذهب quit;
#endif
#endif /* HAVE_LDAP_SSL */
  }
  وإلا إذا(data->set.use_ssl > CURLUSESSL_TRY) {
    failf(data, "LDAP local: explicit TLS not supported");
    result = CURLE_NOT_BUILT_IN;
    اذهب quit;
  }

#ifdef USE_WIN32_LDAP
  rc = ldap_win_bind(data, server, user, passwd);
#وإلا
  rc = ldap_simple_bind_s(server, user, passwd);
#endif
  إذا(!ldap_ssl && rc != LDAP_SUCCESS) {
    ldap_proto = LDAP_VERSION2;
    ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);
#ifdef USE_WIN32_LDAP
    rc = ldap_win_bind(data, server, user, passwd);
#وإلا
    rc = ldap_simple_bind_s(server, user, passwd);
#endif
  }
  إذا(rc != LDAP_SUCCESS) {
#ifdef USE_WIN32_LDAP
    failf(data, "LDAP local: bind via ldap_win_bind %s",
          ldap_err2string(rc));
#وإلا
    failf(data, "LDAP local: bind via ldap_simple_bind_s %s",
          ldap_err2string(rc));
#endif
    result = CURLE_LDAP_CANNOT_BIND;
    اذهب quit;
  }

  Curl_pgrsReset(data);
  rc = ldap_ئىزدەش_s(server, ludp->lud_dn,
                     ludp->lud_scope,
                     ludp->lud_filter, ludp->lud_attrs, 0, &ldapmsg);

  إذا(rc != LDAP_SUCCESS && rc != LDAP_SIZELIMIT_EXCEEDED) {
    failf(data, "LDAP remote: %s", ldap_err2string(rc));
    result = CURLE_LDAP_ئىزدەش_FAILED;
    اذهب quit;
  }

  رەقەم = 0;
  لكل(entryIterator = ldap_first_entry(server, ldapmsg);
      entryIterator;
      entryIterator = ldap_next_entry(server, entryIterator), رەقەم++) {
#ifdef USE_WIN32_LDAP
    TCHAR *attribute;
#وإلا
    حرف *attribute;
#endif

    /* Get the DN and write it to the client */
    {
      حرف *name = عدم;
      size_t name_len = 0;
#ifdef USE_WIN32_LDAP
      TCHAR *dn = ldap_get_dn(server, entryIterator);
      إذا(dn)
        name = curlx_convert_tchar_to_UTF8(dn);
#وإلا
      حرف *dn = name = ldap_get_dn(server, entryIterator);
#endif
      إذا(!name)
        result = dn ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
      وإلا {
        name_len = strlen(name);
        إذا(ldap_str_needs_base64(name, name_len)) {
          حرف *dn_b64 = عدم;
          size_t dn_b64_sz = 0;

          /* A DN with e.g. CR/LF would inject LDIF lines, so encode it */
          result = curlx_base64_encode((uint8_t *)name, name_len,
                                       &dn_b64, &dn_b64_sz);
          إذا(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, "DN:: ", 5);
          إذا(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, dn_b64,
                                       dn_b64_sz);
          curlx_free(dn_b64);
        }
        وإلا {
          result = Curl_client_write(data, CLIENTWRITE_BODY, "DN: ", 4);
          إذا(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, name, name_len);
        }
        إذا(!result)
          result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      }
      FREE_ON_WINLDAP(name);
      ldap_memfree(dn);
      إذا(result)
        اذهب quit;
    }

    /* Get the attributes and write them to the client */
    لكل(attribute = ldap_first_attribute(server, entryIterator, &ber);
        attribute;
        attribute = ldap_next_attribute(server, entryIterator, ber)) {
      Ber_قىممەت **vals;
#ifdef USE_WIN32_LDAP
      حرف *attr = curlx_convert_tchar_to_UTF8(attribute);
      إذا(!attr) {
        ldap_memfree(attribute);
        result = CURLE_OUT_OF_MEMORY;
        اذهب quit;
      }
#وإلا
      حرف *attr = attribute;
#endif
      vals = ldap_get_قىممەتلەر_len(server, entryIterator, attribute);
      إذا(vals) {
        result = show_vals(data, vals, attr);
        /* Free memory used to store values */
        ldap_قىممەت_free_len(vals);
      }

      /* Free the attribute as we are done with it */
      FREE_ON_WINLDAP(attr);
      ldap_memfree(attribute);

      إذا(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      إذا(result)
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
    LDAP_TRACE(("Received %d entries\n", رەقەم));
  }
  إذا(rc == LDAP_SIZELIMIT_EXCEEDED)
    infof(data, "There are more than %d entries", رەقەم);
  إذا(ludp)
    ldap_free_urldesc(ludp);
  إذا(server)
    ldap_unbind_s(server);

  FREE_ON_WINLDAP(host);

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  connclose(conn);

  أرجع result;
}

#ifdef DEBUG_LDAP
ساكن فراغ ldap_trace_تۆۋەن(ثابت حرف *fmt, ...)
{
  ساكن صحيح do_trace = -1;
  va_list args;

  إذا(do_trace == -1) {
    ثابت حرف *env = getenv("CURL_TRACE");
    curl_off_t e = 0;
    إذا(!curlx_str_سان(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  إذا(!do_trace)
    أرجع;

  va_start(args, fmt);
  curl_mvfprintf(stderr, fmt, args);
  va_end(args);
}
#endif /* DEBUG_LDAP */

#ifndef HAVE_LDAP_URL_PARSE

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
    أرجع LDAP_SCOPE_BASE;
  إذا(curl_strequal(p, "sub"))
    أرجع LDAP_SCOPE_SUBTREE;
  إذا(curl_strequal(p, "subtree"))
    أرجع LDAP_SCOPE_SUBTREE;
  أرجع -1;
}

/* number of entries in the attributes list */
ساكن size_t رەقەم_entries(ثابت حرف *s)
{
  size_t بۇيۇملار = 1;

  s = strchr(s, ',');
  طالما(s) {
    بۇيۇملار++;
    s = strchr(s + 1, ',');
  }
  أرجع بۇيۇملار;
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
ساكن curl_ldap_رەقەم_t ldap_url_parse2_تۆۋەن(بنية Curl_easy *data,
                                           ثابت بنية connectdata *conn,
                                           LDAPURLDesc *ludp)
{
  curl_ldap_رەقەم_t rc = LDAP_SUCCESS;
  حرف *p;
  حرف *path;
  حرف *q = عدم;
  حرف *query = عدم;
  size_t i;

  إذا(!data ||
     !data->state.up.path ||
     data->state.up.path[0] != '/' ||
     ((data->state.origin->scheme != &Curl_scheme_ldap) &&
      (data->state.origin->scheme != &Curl_scheme_ldaps)))
    أرجع LDAP_INVALID_SYNTAX;

  ludp->lud_scope = LDAP_SCOPE_BASE;
  ludp->lud_port  = conn->origin->port;
  ludp->lud_host  = conn->origin->hostname;

  /* Duplicate the path */
  p = path = curlx_strdup(data->state.up.path + 1);
  إذا(!path)
    أرجع LDAP_NO_MEMORY;

  /* Duplicate the query if present */
  إذا(data->state.up.query) {
    q = query = curlx_strdup(data->state.up.query);
    إذا(!query) {
      curlx_free(path);
      أرجع LDAP_NO_MEMORY;
    }
  }

  /* Parse the DN (Distinguished Name) */
  إذا(*p) {
    حرف *dn = p;
    حرف *unescaped;
    CURLcode result;

    LDAP_TRACE(("DN '%s'\n", dn));

    /* Unescape the DN */
    result = Curl_urldecode(dn, 0, &unescaped, عدم, REJECT_ZERO);
    إذا(result) {
      rc = LDAP_NO_MEMORY;

      اذهب quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_dn = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    إذا(!ludp->lud_dn) {
      rc = LDAP_NO_MEMORY;

      اذهب quit;
    }
#وإلا
    ludp->lud_dn = unescaped;
#endif
  }

  p = q;
  إذا(!p)
    اذهب quit;

  /* Parse the attributes. skip "??" */
  q = strchr(p, '?');
  إذا(q)
    *q++ = '\0';

  إذا(*p) {
    size_t سانى = رەقەم_entries(p); /* at least one */
    ثابت حرف *atp = p;

    /* Allocate our array (+1 for the NULL entry) */
#ifdef USE_WIN32_LDAP
    ludp->lud_attrs = curlx_calloc(سانى + 1, حجم(TCHAR *));
#وإلا
    ludp->lud_attrs = curlx_calloc(سانى + 1, حجم(حرف *));
#endif
    إذا(!ludp->lud_attrs) {
      rc = LDAP_NO_MEMORY;
      اذهب quit;
    }

    لكل(i = 0; i < سانى; i++) {
      حرف *unescaped;
      CURLcode result;
      بنية Curl_str out;

      إذا(curlx_str_until(&atp, &out, 1024, ','))
        اكسر;

      LDAP_TRACE(("attr[%zu] '%.*s'\n", i, (صحيح)out.len, out.str));

      /* Unescape the attribute */
      result = Curl_urldecode(out.str, out.len, &unescaped, عدم, REJECT_ZERO);
      إذا(result) {
        rc = LDAP_NO_MEMORY;
        اذهب quit;
      }

#ifdef USE_WIN32_LDAP
      /* Convert the unescaped string to a tchar */
      ludp->lud_attrs[i] = curlx_convert_UTF8_to_tchar(unescaped);

      /* Free the unescaped string as we are done with it */
      curlx_free(unescaped);

      إذا(!ludp->lud_attrs[i]) {
        rc = LDAP_NO_MEMORY;
        اذهب quit;
      }
#وإلا
      ludp->lud_attrs[i] = unescaped;
#endif

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
    CURLcode result;

    LDAP_TRACE(("filter '%s'\n", filter));

    /* Unescape the filter */
    result = Curl_urldecode(filter, 0, &unescaped, عدم, REJECT_CTRL);
    إذا(result) {
      rc = LDAP_NO_MEMORY;

      اذهب quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_filter = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    إذا(!ludp->lud_filter) {
      rc = LDAP_NO_MEMORY;

      اذهب quit;
    }
#وإلا
    ludp->lud_filter = unescaped;
#endif
  }

  p = q;
  إذا(p && !*p) {
    rc = LDAP_INVALID_SYNTAX;

    اذهب quit;
  }

quit:
  curlx_free(path);
  curlx_free(query);

  أرجع rc;
}

ساكن curl_ldap_رەقەم_t ldap_url_parse_تۆۋەن(بنية Curl_easy *data,
                                          ثابت بنية connectdata *conn,
                                          LDAPURLDesc **ludpp)
{
  LDAPURLDesc *ludp = curlx_calloc(1, حجم(*ludp));
  curl_ldap_رەقەم_t rc;

  *ludpp = عدم;
  إذا(!ludp)
    أرجع LDAP_NO_MEMORY;

  rc = ldap_url_parse2_تۆۋەن(data, conn, ludp);
  إذا(rc != LDAP_SUCCESS) {
    ldap_free_urldesc_تۆۋەن(ludp);
    ludp = عدم;
  }
  *ludpp = ludp;
  أرجع rc;
}

ساكن فراغ ldap_free_urldesc_تۆۋەن(LDAPURLDesc *ludp)
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
#endif /* !HAVE_LDAP_URL_PARSE */

فراغ Curl_ldap_version(حرف *buf, size_t bufsz)
{
#ifdef USE_WIN32_LDAP
  curl_msnprintf(buf, bufsz, "WinLDAP");
#وإلا
#ifdef LDAP_OPT_X_TLS_PASSPHRASE
  ساكن ثابت حرف flavor[] = "/Apple";
#وإلا
  ساكن ثابت حرف flavor[] = "";
#endif
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  /* Comparing against 0, as different platforms
     disagree on the success define name */
  إذا(ldap_get_option(عدم, LDAP_OPT_API_INFO, &api) == 0) {
    unsigned صحيح patch = (unsigned صحيح)(api.ldapai_vendor_version % 100);
    unsigned صحيح major = (unsigned صحيح)(api.ldapai_vendor_version / 10000);
    unsigned صحيح minor =
      (((unsigned صحيح)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
#ifdef __OS400__
    curl_msnprintf(buf, bufsz, "IBMLDAP/%u.%u.%u", major, minor, patch);
    ldap_قىممەت_free(api.ldapai_extensions);
    (فراغ)flavor;
#وإلا
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u%s",
                   api.ldapai_vendor_name, major, minor, patch, flavor);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((فراغ **)api.ldapai_extensions);
#endif
  }
  وإلا
    curl_msnprintf(buf, bufsz, "LDAP/1");
#endif /* USE_WIN32_LDAP */
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
#pragma GCC diagnostic چىقىرىش
#endif

#endif /* !CURL_DISABLE_LDAP && !USE_OPENLDAP */
