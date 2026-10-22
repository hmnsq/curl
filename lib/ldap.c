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
#અન્તર્ભાવ "curl_setup.h"
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curl_ldap.h"

#જો !defined(CURL_DISABLE_LDAP) && !defined(USE_OPENLDAP)

#જો defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ધકેલો
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
#નહીંતર
#  define LDAP_DEPRECATED 1     /* Be sure ldap_init() is defined. */
#  ifdef HAVE_LBER_H
#    include <lber.h>
#  endif
#  include <ldap.h>
#  જો defined(HAVE_LDAP_SSL) && defined(HAVE_LDAP_SSL_H)
#    include <ldap_ssl.h>
#  endif /* HAVE_LDAP_SSL && HAVE_LDAP_SSL_H */
#endif

#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "escape.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "bufref.h"
#અન્તર્ભાવ "curlx/multibyte.h"
#અન્તર્ભાવ "curlx/base64.h"
#અન્તર્ભાવ "connect.h"

#ifdef USE_WIN32_LDAP
#define FREE_ON_WINLDAP(x) curlx_free(x)
#define curl_ldap_અંક_t    ULONG
#નહીંતર
#define FREE_ON_WINLDAP(x) કરો {} જ્યારે(0)
#define curl_ldap_અંક_t    પૂર્ણાંક
#endif

#ifndef HAVE_LDAP_URL_PARSE

/* Use our own implementation. */

સંરચના ldap_urldesc {
  અક્ષર   *lud_host;
  પૂર્ણાંક     lud_port;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_dn;
  TCHAR **lud_attrs;
#નહીંતર
  અક્ષર   *lud_dn;
  અક્ષર  **lud_attrs;
#endif
  પૂર્ણાંક     lud_scope;
#ifdef USE_WIN32_LDAP
  TCHAR  *lud_filter;
#નહીંતર
  અક્ષર   *lud_filter;
#endif
  અક્ષર  **lud_exts;
  size_t  lud_attrs_dups; /* how many were dup'ed, this field is not in the
                             "real" struct so can only be used in code
                             without HAVE_LDAP_URL_PARSE defined */
};

#undef LDAPURLDesc
#define LDAPURLDesc સંરચના ldap_urldesc

સ્થિર curl_ldap_અંક_t ldap_url_parse_નીચલું(સંરચના Curl_easy *data,
                                          અચલ સંરચના connectdata *conn,
                                          LDAPURLDesc **ludpp);
સ્થિર શૂન્ય ldap_free_urldesc_નીચલું(LDAPURLDesc *ludp);

#undef ldap_free_urldesc
#define ldap_free_urldesc       ldap_free_urldesc_નીચલું

#endif /* !HAVE_LDAP_URL_PARSE */

#ifdef DEBUG_LDAP
#define LDAP_TRACE(x)                 \
  કરો {                                \
    ldap_trace_નીચલું("%u: ", __LINE__); \
    ldap_trace_નીચલું x;                 \
  } જ્યારે(0)

સ્થિર શૂન્ય ldap_trace_નીચલું(અચલ અક્ષર *fmt, ...) CURL_PRINTF(1, 2);
#નહીંતર
#define LDAP_TRACE(x) Curl_nop_stmt
#endif

#જો defined(USE_WIN32_LDAP) && defined(ldap_err2string)
/* Use ANSI error strings in Unicode builds */
#undef ldap_err2string
#define ldap_err2string ldap_err2stringA
#endif

#જો defined(USE_WIN32_LDAP) && defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'void *' of greater size */
#undef LDAP_OPT_ON
#undef LDAP_OPT_OFF
#define LDAP_OPT_ON   ((શૂન્ય *)(size_t)1)
#define LDAP_OPT_OFF  ((શૂન્ય *)(size_t)0)
#endif

#ifdef USE_WIN32_LDAP

#ifdef USE_WINDOWS_SSPI
સ્થિર ULONG ldap_win_bind_auth(LDAP *server, અચલ અક્ષર *user,
                                અચલ અક્ષર *passwd, unsigned long authflags)
{
  ULONG method = 0;
  SEC_WINNT_AUTH_IDENTITY_EX cred;
  ULONG rc = LDAP_AUTH_METHOD_NOT_SUPPORTED;

  memset(&cred, 0, કદ(cred));

#ifdef USE_SPNEGO
  જો(authflags & CURLAUTH_NEGOTIATE) {
    method = LDAP_AUTH_NEGOTIATE;
  }
  નહીંતર
#endif
#ifdef USE_NTLM
  જો(authflags & CURLAUTH_NTLM) {
    method = LDAP_AUTH_NTLM;
  }
  નહીંતર
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  જો(authflags & CURLAUTH_DIGEST) {
    method = LDAP_AUTH_DIGEST;
  }
  નહીંતર
#endif
  {
    /* required anyway if one of upper preprocessor definitions enabled */
  }

  જો(method && user && passwd) {
    CURLcode result = Curl_create_sspi_identity(user, passwd, &cred);
    જો(!result) {
      rc = ldap_bind_s(server, ખાલી, (TCHAR *)&cred, method);
      Curl_sspi_free_identity(&cred);
    }
    નહીંતર {
      rc = LDAP_NO_MEMORY;
    }
  }
  નહીંતર {
    /* proceed with current user credentials */
    method = LDAP_AUTH_NEGOTIATE;
    rc = ldap_bind_s(server, ખાલી, ખાલી, method);
  }
  પાછા rc;
}
#endif /* USE_WINDOWS_SSPI */

સ્થિર ULONG ldap_win_bind(સંરચના Curl_easy *data, LDAP *server,
                           અચલ અક્ષર *user, અચલ અક્ષર *passwd)
{
  ULONG rc = LDAP_INVALID_CREDENTIALS;

  PTCHAR inuser = ખાલી;
  PTCHAR inpass = ખાલી;

  જો(user && passwd && (data->set.httpauth & CURLAUTH_BASIC)) {
    inuser = curlx_convert_UTF8_to_tchar(user);
    inpass = curlx_convert_UTF8_to_tchar(passwd);

    rc = ldap_simple_bind_s(server, inuser, inpass);

    curlx_free(inuser);
    curlx_free(inpass);
  }
#ifdef USE_WINDOWS_SSPI
  નહીંતર {
    rc = ldap_win_bind_auth(server, user, passwd, data->set.httpauth);
  }
#endif

  પાછા rc;
}
#endif /* USE_WIN32_LDAP */

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
સ્થિર bool ldap_str_needs_base64(અચલ અક્ષર *s, size_t len)
{
  size_t i;
  જો(!len)
    પાછા FALSE;
  જો((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    પાછા TRUE;
  માટે(i = 0; i < len; i++)
    જો(!ISPRINT(s[i]))
      પાછા TRUE;
  પાછા FALSE;
}

સ્થિર bool ldap_કિંમત_needs_base64(અચલ અક્ષર *attr, size_t attr_len,
                                    અચલ Ber_કિંમત *val)
{
  જો((attr_len > 7) && curl_strequal(";binary", attr + attr_len - 7))
    પાછા TRUE;

  પાછા ldap_str_needs_base64(val->bv_val, val->bv_len);
}

#ifdef USE_WIN32_LDAP
સ્થિર BOOLEAN bypass_cert_verify(PLDAP Connection,
                                  PCCERT_CONTEXT *ppServerCert)
{
  (શૂન્ય)Connection;
  CertFreeCertificateContext(*ppServerCert);
  /* approve any certificate since the verification is set to bypass */
  પાછા TRUE;
}
#endif

સ્થિર CURLcode show_vals(સંરચના Curl_easy *data, Ber_કિંમત **vals,
                          અચલ અક્ષર *attr)
{
  CURLcode result = CURLE_OK;
  પૂર્ણાંક i;
  size_t attr_len = strlen(attr);

  /* An attribute type has no base64 form in LDIF, so a type carrying control
     bytes (CR/LF etc.) would let a hostile server inject lines. A conformant
     type name never contains them, so reject it. */
  જો(ldap_str_needs_base64(attr, attr_len)) {
    failf(data, "LDAP local: attribute type contains illegal bytes");
    પાછા CURLE_WEIRD_SERVER_REPLY;
  }

  માટે(i = 0; vals[i] && !result; i++) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, "\t", 1);
    જો(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, attr, attr_len);
    જો(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, ":", 1);

    જો(result)
      તોડો;

    જો(ldap_કિંમત_needs_base64(attr, attr_len, vals[i])) {
      અક્ષર *val_b64 = ખાલી;
      size_t val_b64_sz = 0;

      /* Binary attribute, encode to base64. */
      જો(vals[i]->bv_len)
        result = curlx_base64_encode((uint8_t *)vals[i]->bv_val,
                                     vals[i]->bv_len,
                                     &val_b64, &val_b64_sz);
      જો(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, ": ", 2);
      જો(!result && val_b64_sz)
        result = Curl_client_write(data, CLIENTWRITE_BODY, val_b64,
                                   val_b64_sz);
      curlx_free(val_b64);
    }
    નહીંતર {
      result = Curl_client_write(data, CLIENTWRITE_BODY, " ", 1);
      જો(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   vals[i]->bv_val, vals[i]->bv_len);
    }

    જો(!result)
      result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
  }
  પાછા result;
}

સ્થિર CURLcode ldap_do(સંરચના Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  curl_ldap_અંક_t rc = LDAP_SUCCESS;
  LDAP *server = ખાલી;
  LDAPURLDesc *ludp = ખાલી;
  LDAPMessage *ldapmsg = ખાલી;
  LDAPMessage *entryIterator;
  પૂર્ણાંક અંક = 0;
  સંરચના connectdata *conn = data->conn;
  પૂર્ણાંક ldap_proto = LDAP_VERSION3;
  bool ldap_ssl = FALSE;
#ifdef LDAP_OPT_NETWORK_TIMEOUT
  સંરચના timeval ldap_timeout = { 10, 0 }; /* 10s connection/search timeout */
#endif
#ifdef USE_WIN32_LDAP
  TCHAR *host = ખાલી;
#નહીંતર
  અક્ષર *host = ખાલી;
#endif
  અચલ અક્ષર *user = data->state.creds ? data->state.creds->user : ખાલી;
  અચલ અક્ષર *passwd = data->state.creds ? data->state.creds->passwd : ખાલી;
  સંરચના ip_quadruple ipquad;
  bool is_ipv6;
  BerElement *ber = ખાલી;

  *done = TRUE; /* unconditionally */
  infof(data, "LDAP local: LDAP Vendor = %s ; LDAP Version = %d",
        LDAP_VENDOR_NAME, LDAP_VENDOR_VERSION);
  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

#ifdef HAVE_LDAP_URL_PARSE
  rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), &ludp);
#નહીંતર
  rc = ldap_url_parse_નીચલું(data, conn, &ludp);
#endif
  જો(rc) {
    failf(data, "Bad LDAP URL: %s", ldap_err2string(rc));
    result = CURLE_URL_MALFORMAT;
    જાઓ quit;
  }

  result = Curl_conn_get_ip_info(data, conn, FIRSTSOCKET, &is_ipv6, &ipquad);
  જો(result)
    જાઓ quit;

  /* Get the URL scheme (either ldap or ldaps) */
  ldap_ssl = Curl_conn_is_ssl(conn, FIRSTSOCKET);
  infof(data, "LDAP local: trying to establish %s connection",
        ldap_ssl ? "encrypted" : "cleartext");

#ifdef USE_WIN32_LDAP
  host = curlx_convert_UTF8_to_tchar(conn->origin->hostname);
  જો(!host) {
    result = CURLE_OUT_OF_MEMORY;

    જાઓ quit;
  }
#નહીંતર
  host = conn->origin->hostname;
#endif

#ifdef USE_WIN32_LDAP
  જો(ldap_ssl)
    server = ldap_sslinit(host, (curl_ldap_અંક_t)ipquad.remote_port, 1);
  નહીંતર
#endif
    server = ldap_init(host, (curl_ldap_અંક_t)ipquad.remote_port);
  જો(!server) {
    failf(data, "LDAP: cannot setup connect to %s:%u",
          conn->origin->user_hostname, ipquad.remote_port);
    result = CURLE_COULDNT_CONNECT;
    જાઓ quit;
  }

#ifdef LDAP_OPT_NETWORK_TIMEOUT
  ldap_set_option(server, LDAP_OPT_NETWORK_TIMEOUT, &ldap_timeout);
#endif
  ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);

  /* Do not chase referrals. */
  ldap_set_option(server, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  જો(ldap_ssl) {
#ifdef HAVE_LDAP_SSL
#ifdef USE_WIN32_LDAP
    /* Win32 LDAP uses the Windows CA store to verify certificates */
    ldap_set_option(server, LDAP_OPT_SSL, LDAP_OPT_ON);
    જો(!conn->ssl_config.verifypeer) {
      જો(conn->ssl_config.verifyhost) {
        failf(data, "LDAP local: host verification cannot be enabled when "
              "peer verification is disabled for Windows native LDAP");
        result = CURLE_NOT_BUILT_IN;
        જાઓ quit;
      }
      ldap_set_option(server, LDAP_OPT_SERVER_CERTIFICATE,
                      (શૂન્ય *)(uintptr_t)bypass_cert_verify);
    }
#અથવાજો defined(LDAP_OPT_X_TLS) /* !USE_WIN32_LDAP */
    પૂર્ણાંક ldap_option;
    અચલ અક્ષર *ldap_ca = conn->ssl_config.CAfile;
    જો(conn->ssl_config.verifypeer) {
      /* OpenLDAP SDK supports BASE64 files. */
      જો(conn->ssl_config.cert_type &&
         !curl_strequal(conn->ssl_config.cert_type, "PEM")) {
        failf(data, "LDAP local: ERROR OpenLDAP only supports PEM cert-type");
        result = CURLE_SSL_CERTPROBLEM;
        જાઓ quit;
      }
      જો(!ldap_ca) {
        failf(data, "LDAP local: ERROR PEM CA cert not set");
        result = CURLE_SSL_CERTPROBLEM;
        જાઓ quit;
      }
      infof(data, "LDAP local: using PEM CA cert: %s", ldap_ca);
      rc = ldap_set_option(server, LDAP_OPT_X_TLS_CACERTFILE, ldap_ca);
      જો(rc != LDAP_SUCCESS) {
        failf(data, "LDAP local: ERROR setting PEM CA cert: %s",
              ldap_err2string(rc));
        result = CURLE_SSL_CERTPROBLEM;
        જાઓ quit;
      }
      ldap_option = LDAP_OPT_X_TLS_DEMAND;
    }
    નહીંતર
      ldap_option = LDAP_OPT_X_TLS_NEVER;

    rc = ldap_set_option(server, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_option);
    જો(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting cert verify mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      જાઓ quit;
    }
    ldap_option = LDAP_OPT_X_TLS_HARD;
    rc = ldap_set_option(server, LDAP_OPT_X_TLS, &ldap_option);
    જો(rc != LDAP_SUCCESS) {
      failf(data, "LDAP local: ERROR setting SSL/TLS mode: %s",
            ldap_err2string(rc));
      result = CURLE_SSL_CERTPROBLEM;
      જાઓ quit;
    }
#નહીંતર /* !USE_WIN32_LDAP && !LDAP_OPT_X_TLS */
    /* we should probably never come up to here since configure
       should check in first place if we can support LDAP SSL/TLS */
    failf(data, "LDAP local: SSL/TLS not supported with this version "
          "of the OpenLDAP toolkit");
    result = CURLE_SSL_CERTPROBLEM;
    જાઓ quit;
#endif
#endif /* HAVE_LDAP_SSL */
  }
  નહીંતર જો(data->set.use_ssl > CURLUSESSL_TRY) {
    failf(data, "LDAP local: explicit TLS not supported");
    result = CURLE_NOT_BUILT_IN;
    જાઓ quit;
  }

#ifdef USE_WIN32_LDAP
  rc = ldap_win_bind(data, server, user, passwd);
#નહીંતર
  rc = ldap_simple_bind_s(server, user, passwd);
#endif
  જો(!ldap_ssl && rc != LDAP_SUCCESS) {
    ldap_proto = LDAP_VERSION2;
    ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);
#ifdef USE_WIN32_LDAP
    rc = ldap_win_bind(data, server, user, passwd);
#નહીંતર
    rc = ldap_simple_bind_s(server, user, passwd);
#endif
  }
  જો(rc != LDAP_SUCCESS) {
#ifdef USE_WIN32_LDAP
    failf(data, "LDAP local: bind via ldap_win_bind %s",
          ldap_err2string(rc));
#નહીંતર
    failf(data, "LDAP local: bind via ldap_simple_bind_s %s",
          ldap_err2string(rc));
#endif
    result = CURLE_LDAP_CANNOT_BIND;
    જાઓ quit;
  }

  Curl_pgrsReset(data);
  rc = ldap_શોધ_s(server, ludp->lud_dn,
                     ludp->lud_scope,
                     ludp->lud_filter, ludp->lud_attrs, 0, &ldapmsg);

  જો(rc != LDAP_SUCCESS && rc != LDAP_SIZELIMIT_EXCEEDED) {
    failf(data, "LDAP remote: %s", ldap_err2string(rc));
    result = CURLE_LDAP_શોધ_FAILED;
    જાઓ quit;
  }

  અંક = 0;
  માટે(entryIterator = ldap_first_entry(server, ldapmsg);
      entryIterator;
      entryIterator = ldap_next_entry(server, entryIterator), અંક++) {
#ifdef USE_WIN32_LDAP
    TCHAR *attribute;
#નહીંતર
    અક્ષર *attribute;
#endif

    /* Get the DN and write it to the client */
    {
      અક્ષર *name = ખાલી;
      size_t name_len = 0;
#ifdef USE_WIN32_LDAP
      TCHAR *dn = ldap_get_dn(server, entryIterator);
      જો(dn)
        name = curlx_convert_tchar_to_UTF8(dn);
#નહીંતર
      અક્ષર *dn = name = ldap_get_dn(server, entryIterator);
#endif
      જો(!name)
        result = dn ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
      નહીંતર {
        name_len = strlen(name);
        જો(ldap_str_needs_base64(name, name_len)) {
          અક્ષર *dn_b64 = ખાલી;
          size_t dn_b64_sz = 0;

          /* A DN with e.g. CR/LF would inject LDIF lines, so encode it */
          result = curlx_base64_encode((uint8_t *)name, name_len,
                                       &dn_b64, &dn_b64_sz);
          જો(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, "DN:: ", 5);
          જો(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, dn_b64,
                                       dn_b64_sz);
          curlx_free(dn_b64);
        }
        નહીંતર {
          result = Curl_client_write(data, CLIENTWRITE_BODY, "DN: ", 4);
          જો(!result)
            result = Curl_client_write(data, CLIENTWRITE_BODY, name, name_len);
        }
        જો(!result)
          result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      }
      FREE_ON_WINLDAP(name);
      ldap_memfree(dn);
      જો(result)
        જાઓ quit;
    }

    /* Get the attributes and write them to the client */
    માટે(attribute = ldap_first_attribute(server, entryIterator, &ber);
        attribute;
        attribute = ldap_next_attribute(server, entryIterator, ber)) {
      Ber_કિંમત **vals;
#ifdef USE_WIN32_LDAP
      અક્ષર *attr = curlx_convert_tchar_to_UTF8(attribute);
      જો(!attr) {
        ldap_memfree(attribute);
        result = CURLE_OUT_OF_MEMORY;
        જાઓ quit;
      }
#નહીંતર
      અક્ષર *attr = attribute;
#endif
      vals = ldap_get_કિંમતો_len(server, entryIterator, attribute);
      જો(vals) {
        result = show_vals(data, vals, attr);
        /* Free memory used to store values */
        ldap_કિંમત_free_len(vals);
      }

      /* Free the attribute as we are done with it */
      FREE_ON_WINLDAP(attr);
      ldap_memfree(attribute);

      જો(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      જો(result)
        જાઓ quit;
    }

    જો(ber) {
      ber_free(ber, 0);
      ber = ખાલી;
    }
  }

quit:
  જો(ber)
    ber_free(ber, 0);
  જો(ldapmsg) {
    ldap_msgfree(ldapmsg);
    LDAP_TRACE(("Received %d entries\n", અંક));
  }
  જો(rc == LDAP_SIZELIMIT_EXCEEDED)
    infof(data, "There are more than %d entries", અંક);
  જો(ludp)
    ldap_free_urldesc(ludp);
  જો(server)
    ldap_unbind_s(server);

  FREE_ON_WINLDAP(host);

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  connclose(conn);

  પાછા result;
}

#ifdef DEBUG_LDAP
સ્થિર શૂન્ય ldap_trace_નીચલું(અચલ અક્ષર *fmt, ...)
{
  સ્થિર પૂર્ણાંક do_trace = -1;
  va_list args;

  જો(do_trace == -1) {
    અચલ અક્ષર *env = getenv("CURL_TRACE");
    curl_off_t e = 0;
    જો(!curlx_str_સંખ્યા(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  જો(!do_trace)
    પાછા;

  va_start(args, fmt);
  curl_mvfprintf(stderr, fmt, args);
  va_end(args);
}
#endif /* DEBUG_LDAP */

#ifndef HAVE_LDAP_URL_PARSE

/*
 * Return scope-value for a scope-string.
 */
સ્થિર પૂર્ણાંક str2scope(અચલ અક્ષર *p)
{
  જો(curl_strequal(p, "one"))
    પાછા LDAP_SCOPE_ONELEVEL;
  જો(curl_strequal(p, "onetree"))
    પાછા LDAP_SCOPE_ONELEVEL;
  જો(curl_strequal(p, "base"))
    પાછા LDAP_SCOPE_BASE;
  જો(curl_strequal(p, "sub"))
    પાછા LDAP_SCOPE_SUBTREE;
  જો(curl_strequal(p, "subtree"))
    પાછા LDAP_SCOPE_SUBTREE;
  પાછા -1;
}

/* number of entries in the attributes list */
સ્થિર size_t અંક_entries(અચલ અક્ષર *s)
{
  size_t વસ્તુઓ = 1;

  s = strchr(s, ',');
  જ્યારે(s) {
    વસ્તુઓ++;
    s = strchr(s + 1, ',');
  }
  પાછા વસ્તુઓ;
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
સ્થિર curl_ldap_અંક_t ldap_url_parse2_નીચલું(સંરચના Curl_easy *data,
                                           અચલ સંરચના connectdata *conn,
                                           LDAPURLDesc *ludp)
{
  curl_ldap_અંક_t rc = LDAP_SUCCESS;
  અક્ષર *p;
  અક્ષર *path;
  અક્ષર *q = ખાલી;
  અક્ષર *query = ખાલી;
  size_t i;

  જો(!data ||
     !data->state.up.path ||
     data->state.up.path[0] != '/' ||
     ((data->state.origin->scheme != &Curl_scheme_ldap) &&
      (data->state.origin->scheme != &Curl_scheme_ldaps)))
    પાછા LDAP_INVALID_SYNTAX;

  ludp->lud_scope = LDAP_SCOPE_BASE;
  ludp->lud_port  = conn->origin->port;
  ludp->lud_host  = conn->origin->hostname;

  /* Duplicate the path */
  p = path = curlx_strdup(data->state.up.path + 1);
  જો(!path)
    પાછા LDAP_NO_MEMORY;

  /* Duplicate the query if present */
  જો(data->state.up.query) {
    q = query = curlx_strdup(data->state.up.query);
    જો(!query) {
      curlx_free(path);
      પાછા LDAP_NO_MEMORY;
    }
  }

  /* Parse the DN (Distinguished Name) */
  જો(*p) {
    અક્ષર *dn = p;
    અક્ષર *unescaped;
    CURLcode result;

    LDAP_TRACE(("DN '%s'\n", dn));

    /* Unescape the DN */
    result = Curl_urldecode(dn, 0, &unescaped, ખાલી, REJECT_ZERO);
    જો(result) {
      rc = LDAP_NO_MEMORY;

      જાઓ quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_dn = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    જો(!ludp->lud_dn) {
      rc = LDAP_NO_MEMORY;

      જાઓ quit;
    }
#નહીંતર
    ludp->lud_dn = unescaped;
#endif
  }

  p = q;
  જો(!p)
    જાઓ quit;

  /* Parse the attributes. skip "??" */
  q = strchr(p, '?');
  જો(q)
    *q++ = '\0';

  જો(*p) {
    size_t ગણતરી = અંક_entries(p); /* at least one */
    અચલ અક્ષર *atp = p;

    /* Allocate our array (+1 for the NULL entry) */
#ifdef USE_WIN32_LDAP
    ludp->lud_attrs = curlx_calloc(ગણતરી + 1, કદ(TCHAR *));
#નહીંતર
    ludp->lud_attrs = curlx_calloc(ગણતરી + 1, કદ(અક્ષર *));
#endif
    જો(!ludp->lud_attrs) {
      rc = LDAP_NO_MEMORY;
      જાઓ quit;
    }

    માટે(i = 0; i < ગણતરી; i++) {
      અક્ષર *unescaped;
      CURLcode result;
      સંરચના Curl_str out;

      જો(curlx_str_until(&atp, &out, 1024, ','))
        તોડો;

      LDAP_TRACE(("attr[%zu] '%.*s'\n", i, (પૂર્ણાંક)out.len, out.str));

      /* Unescape the attribute */
      result = Curl_urldecode(out.str, out.len, &unescaped, ખાલી, REJECT_ZERO);
      જો(result) {
        rc = LDAP_NO_MEMORY;
        જાઓ quit;
      }

#ifdef USE_WIN32_LDAP
      /* Convert the unescaped string to a tchar */
      ludp->lud_attrs[i] = curlx_convert_UTF8_to_tchar(unescaped);

      /* Free the unescaped string as we are done with it */
      curlx_free(unescaped);

      જો(!ludp->lud_attrs[i]) {
        rc = LDAP_NO_MEMORY;
        જાઓ quit;
      }
#નહીંતર
      ludp->lud_attrs[i] = unescaped;
#endif

      ludp->lud_attrs_dups++;
      જો(curlx_str_single(&atp, ','))
        તોડો;
    }
  }

  p = q;
  જો(!p)
    જાઓ quit;

  /* Parse the scope. skip "??" */
  q = strchr(p, '?');
  જો(q)
    *q++ = '\0';

  જો(*p) {
    ludp->lud_scope = str2scope(p);
    જો(ludp->lud_scope == -1) {
      rc = LDAP_INVALID_SYNTAX;

      જાઓ quit;
    }
    LDAP_TRACE(("scope %d\n", ludp->lud_scope));
  }

  p = q;
  જો(!p)
    જાઓ quit;

  /* Parse the filter */
  q = strchr(p, '?');
  જો(q)
    *q++ = '\0';

  જો(*p) {
    અક્ષર *filter = p;
    અક્ષર *unescaped;
    CURLcode result;

    LDAP_TRACE(("filter '%s'\n", filter));

    /* Unescape the filter */
    result = Curl_urldecode(filter, 0, &unescaped, ખાલી, REJECT_CTRL);
    જો(result) {
      rc = LDAP_NO_MEMORY;

      જાઓ quit;
    }

#ifdef USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_filter = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    જો(!ludp->lud_filter) {
      rc = LDAP_NO_MEMORY;

      જાઓ quit;
    }
#નહીંતર
    ludp->lud_filter = unescaped;
#endif
  }

  p = q;
  જો(p && !*p) {
    rc = LDAP_INVALID_SYNTAX;

    જાઓ quit;
  }

quit:
  curlx_free(path);
  curlx_free(query);

  પાછા rc;
}

સ્થિર curl_ldap_અંક_t ldap_url_parse_નીચલું(સંરચના Curl_easy *data,
                                          અચલ સંરચના connectdata *conn,
                                          LDAPURLDesc **ludpp)
{
  LDAPURLDesc *ludp = curlx_calloc(1, કદ(*ludp));
  curl_ldap_અંક_t rc;

  *ludpp = ખાલી;
  જો(!ludp)
    પાછા LDAP_NO_MEMORY;

  rc = ldap_url_parse2_નીચલું(data, conn, ludp);
  જો(rc != LDAP_SUCCESS) {
    ldap_free_urldesc_નીચલું(ludp);
    ludp = ખાલી;
  }
  *ludpp = ludp;
  પાછા rc;
}

સ્થિર શૂન્ય ldap_free_urldesc_નીચલું(LDAPURLDesc *ludp)
{
  જો(!ludp)
    પાછા;

  curlx_free(ludp->lud_dn);
  curlx_free(ludp->lud_filter);

  જો(ludp->lud_attrs) {
    size_t i;
    માટે(i = 0; i < ludp->lud_attrs_dups; i++) {
      curlx_free(ludp->lud_attrs[i]);
    }
    curlx_free(ludp->lud_attrs);
  }

  curlx_free(ludp);
}
#endif /* !HAVE_LDAP_URL_PARSE */

શૂન્ય Curl_ldap_version(અક્ષર *buf, size_t bufsz)
{
#ifdef USE_WIN32_LDAP
  curl_msnprintf(buf, bufsz, "WinLDAP");
#નહીંતર
#ifdef LDAP_OPT_X_TLS_PASSPHRASE
  સ્થિર અચલ અક્ષર flavor[] = "/Apple";
#નહીંતર
  સ્થિર અચલ અક્ષર flavor[] = "";
#endif
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  /* Comparing against 0, as different platforms
     disagree on the success define name */
  જો(ldap_get_option(ખાલી, LDAP_OPT_API_INFO, &api) == 0) {
    unsigned પૂર્ણાંક patch = (unsigned પૂર્ણાંક)(api.ldapai_vendor_version % 100);
    unsigned પૂર્ણાંક major = (unsigned પૂર્ણાંક)(api.ldapai_vendor_version / 10000);
    unsigned પૂર્ણાંક minor =
      (((unsigned પૂર્ણાંક)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
#ifdef __OS400__
    curl_msnprintf(buf, bufsz, "IBMLDAP/%u.%u.%u", major, minor, patch);
    ldap_કિંમત_free(api.ldapai_extensions);
    (શૂન્ય)flavor;
#નહીંતર
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u%s",
                   api.ldapai_vendor_name, major, minor, patch, flavor);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((શૂન્ય **)api.ldapai_extensions);
#endif
  }
  નહીંતર
    curl_msnprintf(buf, bufsz, "LDAP/1");
#endif /* USE_WIN32_LDAP */
}

/*
 * LDAP protocol handler.
 */
અચલ સંરચના Curl_protocol Curl_protocol_ldap = {
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

#જો defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic કાઢો
#endif

#endif /* !CURL_DISABLE_LDAP && !USE_OPENLDAP */
