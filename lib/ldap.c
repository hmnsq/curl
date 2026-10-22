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
#समावेश "curl_setup.h"
#समावेश "urldata.h"
#समावेश "curl_ldap.h"

#अगर !defined(CURL_DISABLE_LDAP) && !defined(USE_OPENLDAP)

#अगर defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#आशय GCC diagnostic धकेलो
#आशय GCC diagnostic ignored "-Wdeprecated-declarations"
#पूर्ण_अगर

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

#अगर_घोषित USE_WIN32_LDAP           /* Use Windows LDAP implementation. */
#  include <winldap.h>
/* Undefine indirect <wincrypt.h> symbols conflicting with AWS-LC/BoringSSL. */
#  undef X509_NAME
#  undef X509_EXTENSIONS
#  undef PKCS7_ISSUER_AND_SERIAL
#  undef PKCS7_SIGNER_INFO
#  undef OCSP_REQUEST
#  undef OCSP_RESPONSE
#  include <winber.h>
#अन्यथा
#  define LDAP_DEPRECATED 1     /* Be sure ldap_init() is defined. */
#  ifdef HAVE_LBER_H
#    include <lber.h>
#  endif
#  include <ldap.h>
#  अगर defined(HAVE_LDAP_SSL) && defined(HAVE_LDAP_SSL_H)
#    include <ldap_ssl.h>
#  endif /* HAVE_LDAP_SSL && HAVE_LDAP_SSL_H */
#पूर्ण_अगर

#समावेश "cfilters.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "escape.h"
#समावेश "progress.h"
#समावेश "transfer.h"
#समावेश "curlx/strparse.h"
#समावेश "bufref.h"
#समावेश "curlx/multibyte.h"
#समावेश "curlx/base64.h"
#समावेश "connect.h"

#अगर_घोषित USE_WIN32_LDAP
#घोषणा FREE_ON_WINLDAP(x) curlx_free(x)
#घोषणा curl_ldap_अंक_t    ULONG
#अन्यथा
#घोषणा FREE_ON_WINLDAP(x) करो {} जबतक(0)
#घोषणा curl_ldap_अंक_t    पूर्णांक
#पूर्ण_अगर

#अगर_अघोषित HAVE_LDAP_URL_PARSE

/* Use our own implementation. */

काष्ठा ldap_urldesc {
  अक्षर   *lud_host;
  पूर्णांक     lud_port;
#अगर_घोषित USE_WIN32_LDAP
  TCHAR  *lud_dn;
  TCHAR **lud_attrs;
#अन्यथा
  अक्षर   *lud_dn;
  अक्षर  **lud_attrs;
#पूर्ण_अगर
  पूर्णांक     lud_scope;
#अगर_घोषित USE_WIN32_LDAP
  TCHAR  *lud_filter;
#अन्यथा
  अक्षर   *lud_filter;
#पूर्ण_अगर
  अक्षर  **lud_exts;
  माप_प्रकार  lud_attrs_dups; /* how many were dup'ed, this field is not in the
                             "real" struct so can only be used in code
                             without HAVE_LDAP_URL_PARSE defined */
};

#अघोषित LDAPURLDesc
#घोषणा LDAPURLDesc काष्ठा ldap_urldesc

अटल curl_ldap_अंक_t ldap_url_parse_निचला(काष्ठा Curl_easy *तथ्य,
                                          स्थिर काष्ठा connectdata *conn,
                                          LDAPURLDesc **ludpp);
अटल व्योम ldap_free_urldesc_निचला(LDAPURLDesc *ludp);

#अघोषित ldap_free_urldesc
#घोषणा ldap_free_urldesc       ldap_free_urldesc_निचला

#पूर्ण_अगर /* !HAVE_LDAP_URL_PARSE */

#अगर_घोषित DEBUG_LDAP
#घोषणा LDAP_TRACE(x)                 \
  करो {                                \
    ldap_trace_निचला("%u: ", __LINE__); \
    ldap_trace_निचला x;                 \
  } जबतक(0)

अटल व्योम ldap_trace_निचला(स्थिर अक्षर *fmt, ...) CURL_PRINTF(1, 2);
#अन्यथा
#घोषणा LDAP_TRACE(x) Curl_nop_stmt
#पूर्ण_अगर

#अगर defined(USE_WIN32_LDAP) && defined(ldap_err2string)
/* Use ANSI error strings in Unicode builds */
#अघोषित ldap_err2string
#घोषणा ldap_err2string ldap_err2stringA
#पूर्ण_अगर

#अगर defined(USE_WIN32_LDAP) && defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'void *' of greater size */
#अघोषित LDAP_OPT_ON
#अघोषित LDAP_OPT_OFF
#घोषणा LDAP_OPT_ON   ((व्योम *)(माप_प्रकार)1)
#घोषणा LDAP_OPT_OFF  ((व्योम *)(माप_प्रकार)0)
#पूर्ण_अगर

#अगर_घोषित USE_WIN32_LDAP

#अगर_घोषित USE_WINDOWS_SSPI
अटल ULONG ldap_win_bind_auth(LDAP *server, स्थिर अक्षर *user,
                                स्थिर अक्षर *passwd, अचिन्हित दीर्घ authflags)
{
  ULONG method = 0;
  SEC_WINNT_AUTH_IDENTITY_EX cred;
  ULONG rc = LDAP_AUTH_METHOD_NOT_SUPPORTED;

  स_रखो(&cred, 0, माप(cred));

#अगर_घोषित USE_SPNEGO
  अगर(authflags & CURLAUTH_NEGOTIATE) {
    method = LDAP_AUTH_NEGOTIATE;
  }
  अन्यथा
#पूर्ण_अगर
#अगर_घोषित USE_NTLM
  अगर(authflags & CURLAUTH_NTLM) {
    method = LDAP_AUTH_NTLM;
  }
  अन्यथा
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH
  अगर(authflags & CURLAUTH_DIGEST) {
    method = LDAP_AUTH_DIGEST;
  }
  अन्यथा
#पूर्ण_अगर
  {
    /* required anyway if one of upper preprocessor definitions enabled */
  }

  अगर(method && user && passwd) {
    CURLcode परिणाम = Curl_create_sspi_identity(user, passwd, &cred);
    अगर(!परिणाम) {
      rc = ldap_bind_s(server, शून्य, (TCHAR *)&cred, method);
      Curl_sspi_free_identity(&cred);
    }
    अन्यथा {
      rc = LDAP_NO_MEMORY;
    }
  }
  अन्यथा {
    /* proceed with current user credentials */
    method = LDAP_AUTH_NEGOTIATE;
    rc = ldap_bind_s(server, शून्य, शून्य, method);
  }
  वापस rc;
}
#पूर्ण_अगर /* USE_WINDOWS_SSPI */

अटल ULONG ldap_win_bind(काष्ठा Curl_easy *तथ्य, LDAP *server,
                           स्थिर अक्षर *user, स्थिर अक्षर *passwd)
{
  ULONG rc = LDAP_INVALID_CREDENTIALS;

  PTCHAR inuser = शून्य;
  PTCHAR inpass = शून्य;

  अगर(user && passwd && (तथ्य->set.httpauth & CURLAUTH_BASIC)) {
    inuser = curlx_convert_UTF8_to_tchar(user);
    inpass = curlx_convert_UTF8_to_tchar(passwd);

    rc = ldap_simple_bind_s(server, inuser, inpass);

    curlx_free(inuser);
    curlx_free(inpass);
  }
#अगर_घोषित USE_WINDOWS_SSPI
  अन्यथा {
    rc = ldap_win_bind_auth(server, user, passwd, तथ्य->set.httpauth);
  }
#पूर्ण_अगर

  वापस rc;
}
#पूर्ण_अगर /* USE_WIN32_LDAP */

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
अटल bool ldap_str_needs_base64(स्थिर अक्षर *s, माप_प्रकार len)
{
  माप_प्रकार i;
  अगर(!len)
    वापस FALSE;
  अगर((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    वापस TRUE;
  क्रम(i = 0; i < len; i++)
    अगर(!ISPRINT(s[i]))
      वापस TRUE;
  वापस FALSE;
}

अटल bool ldap_मूल्य_needs_base64(स्थिर अक्षर *attr, माप_प्रकार attr_len,
                                    स्थिर Ber_मूल्य *val)
{
  अगर((attr_len > 7) && curl_strequal(";binary", attr + attr_len - 7))
    वापस TRUE;

  वापस ldap_str_needs_base64(val->bv_val, val->bv_len);
}

#अगर_घोषित USE_WIN32_LDAP
अटल BOOLEAN bypass_cert_verify(PLDAP Connection,
                                  PCCERT_CONTEXT *ppServerCert)
{
  (व्योम)Connection;
  CertFreeCertificateContext(*ppServerCert);
  /* approve any certificate since the verification is set to bypass */
  वापस TRUE;
}
#पूर्ण_अगर

अटल CURLcode show_vals(काष्ठा Curl_easy *तथ्य, Ber_मूल्य **vals,
                          स्थिर अक्षर *attr)
{
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक i;
  माप_प्रकार attr_len = म_माप(attr);

  /* An attribute type has no base64 form in LDIF, so a type carrying control
     bytes (CR/LF etc.) would let a hostile server inject lines. A conformant
     type name never contains them, so reject it. */
  अगर(ldap_str_needs_base64(attr, attr_len)) {
    failf(तथ्य, "LDAP local: attribute type contains illegal bytes");
    वापस CURLE_WEIRD_SERVER_REPLY;
  }

  क्रम(i = 0; vals[i] && !परिणाम; i++) {
    परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, "\t", 1);
    अगर(!परिणाम)
      परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, attr, attr_len);
    अगर(!परिणाम)
      परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, ":", 1);

    अगर(परिणाम)
      अवरोध;

    अगर(ldap_मूल्य_needs_base64(attr, attr_len, vals[i])) {
      अक्षर *val_b64 = शून्य;
      माप_प्रकार val_b64_sz = 0;

      /* Binary attribute, encode to base64. */
      अगर(vals[i]->bv_len)
        परिणाम = curlx_base64_encode((uint8_t *)vals[i]->bv_val,
                                     vals[i]->bv_len,
                                     &val_b64, &val_b64_sz);
      अगर(!परिणाम)
        परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, ": ", 2);
      अगर(!परिणाम && val_b64_sz)
        परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, val_b64,
                                   val_b64_sz);
      curlx_free(val_b64);
    }
    अन्यथा {
      परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, " ", 1);
      अगर(!परिणाम)
        परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY,
                                   vals[i]->bv_val, vals[i]->bv_len);
    }

    अगर(!परिणाम)
      परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, "\n", 1);
  }
  वापस परिणाम;
}

अटल CURLcode ldap_do(काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम = CURLE_OK;
  curl_ldap_अंक_t rc = LDAP_SUCCESS;
  LDAP *server = शून्य;
  LDAPURLDesc *ludp = शून्य;
  LDAPMessage *ldapmsg = शून्य;
  LDAPMessage *entryIterator;
  पूर्णांक अंक = 0;
  काष्ठा connectdata *conn = तथ्य->conn;
  पूर्णांक ldap_proto = LDAP_VERSION3;
  bool ldap_ssl = FALSE;
#अगर_घोषित LDAP_OPT_NETWORK_TIMEOUT
  काष्ठा timeval ldap_timeout = { 10, 0 }; /* 10s connection/search timeout */
#पूर्ण_अगर
#अगर_घोषित USE_WIN32_LDAP
  TCHAR *host = शून्य;
#अन्यथा
  अक्षर *host = शून्य;
#पूर्ण_अगर
  स्थिर अक्षर *user = तथ्य->state.creds ? तथ्य->state.creds->user : शून्य;
  स्थिर अक्षर *passwd = तथ्य->state.creds ? तथ्य->state.creds->passwd : शून्य;
  काष्ठा ip_quadruple ipquad;
  bool is_ipv6;
  BerElement *ber = शून्य;

  *done = TRUE; /* unconditionally */
  infof(तथ्य, "LDAP local: LDAP Vendor = %s ; LDAP Version = %d",
        LDAP_VENDOR_NAME, LDAP_VENDOR_VERSION);
  infof(तथ्य, "LDAP local: %s", Curl_bufref_ptr(&तथ्य->state.url));

#अगर_घोषित HAVE_LDAP_URL_PARSE
  rc = ldap_url_parse(Curl_bufref_ptr(&तथ्य->state.url), &ludp);
#अन्यथा
  rc = ldap_url_parse_निचला(तथ्य, conn, &ludp);
#पूर्ण_अगर
  अगर(rc) {
    failf(तथ्य, "Bad LDAP URL: %s", ldap_err2string(rc));
    परिणाम = CURLE_URL_MALFORMAT;
    जाओ quit;
  }

  परिणाम = Curl_conn_get_ip_info(तथ्य, conn, FIRSTSOCKET, &is_ipv6, &ipquad);
  अगर(परिणाम)
    जाओ quit;

  /* Get the URL scheme (either ldap or ldaps) */
  ldap_ssl = Curl_conn_is_ssl(conn, FIRSTSOCKET);
  infof(तथ्य, "LDAP local: trying to establish %s connection",
        ldap_ssl ? "encrypted" : "cleartext");

#अगर_घोषित USE_WIN32_LDAP
  host = curlx_convert_UTF8_to_tchar(conn->origin->hostname);
  अगर(!host) {
    परिणाम = CURLE_OUT_OF_MEMORY;

    जाओ quit;
  }
#अन्यथा
  host = conn->origin->hostname;
#पूर्ण_अगर

#अगर_घोषित USE_WIN32_LDAP
  अगर(ldap_ssl)
    server = ldap_sslinit(host, (curl_ldap_अंक_t)ipquad.remote_port, 1);
  अन्यथा
#पूर्ण_अगर
    server = ldap_init(host, (curl_ldap_अंक_t)ipquad.remote_port);
  अगर(!server) {
    failf(तथ्य, "LDAP: cannot setup connect to %s:%u",
          conn->origin->user_hostname, ipquad.remote_port);
    परिणाम = CURLE_COULDNT_CONNECT;
    जाओ quit;
  }

#अगर_घोषित LDAP_OPT_NETWORK_TIMEOUT
  ldap_set_option(server, LDAP_OPT_NETWORK_TIMEOUT, &ldap_timeout);
#पूर्ण_अगर
  ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);

  /* Do not chase referrals. */
  ldap_set_option(server, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  अगर(ldap_ssl) {
#अगर_घोषित HAVE_LDAP_SSL
#अगर_घोषित USE_WIN32_LDAP
    /* Win32 LDAP uses the Windows CA store to verify certificates */
    ldap_set_option(server, LDAP_OPT_SSL, LDAP_OPT_ON);
    अगर(!conn->ssl_config.verifypeer) {
      अगर(conn->ssl_config.verifyhost) {
        failf(तथ्य, "LDAP local: host verification cannot be enabled when "
              "peer verification is disabled for Windows native LDAP");
        परिणाम = CURLE_NOT_BUILT_IN;
        जाओ quit;
      }
      ldap_set_option(server, LDAP_OPT_SERVER_CERTIFICATE,
                      (व्योम *)(uintptr_t)bypass_cert_verify);
    }
#या_अगर defined(LDAP_OPT_X_TLS) /* !USE_WIN32_LDAP */
    पूर्णांक ldap_option;
    स्थिर अक्षर *ldap_ca = conn->ssl_config.CAfile;
    अगर(conn->ssl_config.verifypeer) {
      /* OpenLDAP SDK supports BASE64 files. */
      अगर(conn->ssl_config.cert_type &&
         !curl_strequal(conn->ssl_config.cert_type, "PEM")) {
        failf(तथ्य, "LDAP local: ERROR OpenLDAP only supports PEM cert-type");
        परिणाम = CURLE_SSL_CERTPROBLEM;
        जाओ quit;
      }
      अगर(!ldap_ca) {
        failf(तथ्य, "LDAP local: ERROR PEM CA cert not set");
        परिणाम = CURLE_SSL_CERTPROBLEM;
        जाओ quit;
      }
      infof(तथ्य, "LDAP local: using PEM CA cert: %s", ldap_ca);
      rc = ldap_set_option(server, LDAP_OPT_X_TLS_CACERTFILE, ldap_ca);
      अगर(rc != LDAP_SUCCESS) {
        failf(तथ्य, "LDAP local: ERROR setting PEM CA cert: %s",
              ldap_err2string(rc));
        परिणाम = CURLE_SSL_CERTPROBLEM;
        जाओ quit;
      }
      ldap_option = LDAP_OPT_X_TLS_DEMAND;
    }
    अन्यथा
      ldap_option = LDAP_OPT_X_TLS_NEVER;

    rc = ldap_set_option(server, LDAP_OPT_X_TLS_REQUIRE_CERT, &ldap_option);
    अगर(rc != LDAP_SUCCESS) {
      failf(तथ्य, "LDAP local: ERROR setting cert verify mode: %s",
            ldap_err2string(rc));
      परिणाम = CURLE_SSL_CERTPROBLEM;
      जाओ quit;
    }
    ldap_option = LDAP_OPT_X_TLS_HARD;
    rc = ldap_set_option(server, LDAP_OPT_X_TLS, &ldap_option);
    अगर(rc != LDAP_SUCCESS) {
      failf(तथ्य, "LDAP local: ERROR setting SSL/TLS mode: %s",
            ldap_err2string(rc));
      परिणाम = CURLE_SSL_CERTPROBLEM;
      जाओ quit;
    }
#अन्यथा /* !USE_WIN32_LDAP && !LDAP_OPT_X_TLS */
    /* we should probably never come up to here since configure
       should check in first place if we can support LDAP SSL/TLS */
    failf(तथ्य, "LDAP local: SSL/TLS not supported with this version "
          "of the OpenLDAP toolkit");
    परिणाम = CURLE_SSL_CERTPROBLEM;
    जाओ quit;
#पूर्ण_अगर
#पूर्ण_अगर /* HAVE_LDAP_SSL */
  }
  अन्यथा अगर(तथ्य->set.use_ssl > CURLUSESSL_TRY) {
    failf(तथ्य, "LDAP local: explicit TLS not supported");
    परिणाम = CURLE_NOT_BUILT_IN;
    जाओ quit;
  }

#अगर_घोषित USE_WIN32_LDAP
  rc = ldap_win_bind(तथ्य, server, user, passwd);
#अन्यथा
  rc = ldap_simple_bind_s(server, user, passwd);
#पूर्ण_अगर
  अगर(!ldap_ssl && rc != LDAP_SUCCESS) {
    ldap_proto = LDAP_VERSION2;
    ldap_set_option(server, LDAP_OPT_PROTOCOL_VERSION, &ldap_proto);
#अगर_घोषित USE_WIN32_LDAP
    rc = ldap_win_bind(तथ्य, server, user, passwd);
#अन्यथा
    rc = ldap_simple_bind_s(server, user, passwd);
#पूर्ण_अगर
  }
  अगर(rc != LDAP_SUCCESS) {
#अगर_घोषित USE_WIN32_LDAP
    failf(तथ्य, "LDAP local: bind via ldap_win_bind %s",
          ldap_err2string(rc));
#अन्यथा
    failf(तथ्य, "LDAP local: bind via ldap_simple_bind_s %s",
          ldap_err2string(rc));
#पूर्ण_अगर
    परिणाम = CURLE_LDAP_CANNOT_BIND;
    जाओ quit;
  }

  Curl_pgrsReset(तथ्य);
  rc = ldap_खोज_s(server, ludp->lud_dn,
                     ludp->lud_scope,
                     ludp->lud_filter, ludp->lud_attrs, 0, &ldapmsg);

  अगर(rc != LDAP_SUCCESS && rc != LDAP_SIZELIMIT_EXCEEDED) {
    failf(तथ्य, "LDAP remote: %s", ldap_err2string(rc));
    परिणाम = CURLE_LDAP_खोज_FAILED;
    जाओ quit;
  }

  अंक = 0;
  क्रम(entryIterator = ldap_first_entry(server, ldapmsg);
      entryIterator;
      entryIterator = ldap_अग्र_entry(server, entryIterator), अंक++) {
#अगर_घोषित USE_WIN32_LDAP
    TCHAR *attribute;
#अन्यथा
    अक्षर *attribute;
#पूर्ण_अगर

    /* Get the DN and write it to the client */
    {
      अक्षर *name = शून्य;
      माप_प्रकार name_len = 0;
#अगर_घोषित USE_WIN32_LDAP
      TCHAR *dn = ldap_get_dn(server, entryIterator);
      अगर(dn)
        name = curlx_convert_tchar_to_UTF8(dn);
#अन्यथा
      अक्षर *dn = name = ldap_get_dn(server, entryIterator);
#पूर्ण_अगर
      अगर(!name)
        परिणाम = dn ? CURLE_OUT_OF_MEMORY : CURLE_FAILED_INIT;
      अन्यथा {
        name_len = म_माप(name);
        अगर(ldap_str_needs_base64(name, name_len)) {
          अक्षर *dn_b64 = शून्य;
          माप_प्रकार dn_b64_sz = 0;

          /* A DN with e.g. CR/LF would inject LDIF lines, so encode it */
          परिणाम = curlx_base64_encode((uint8_t *)name, name_len,
                                       &dn_b64, &dn_b64_sz);
          अगर(!परिणाम)
            परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, "DN:: ", 5);
          अगर(!परिणाम)
            परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, dn_b64,
                                       dn_b64_sz);
          curlx_free(dn_b64);
        }
        अन्यथा {
          परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, "DN: ", 4);
          अगर(!परिणाम)
            परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, name, name_len);
        }
        अगर(!परिणाम)
          परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, "\n", 1);
      }
      FREE_ON_WINLDAP(name);
      ldap_memfree(dn);
      अगर(परिणाम)
        जाओ quit;
    }

    /* Get the attributes and write them to the client */
    क्रम(attribute = ldap_first_attribute(server, entryIterator, &ber);
        attribute;
        attribute = ldap_अग्र_attribute(server, entryIterator, ber)) {
      Ber_मूल्य **vals;
#अगर_घोषित USE_WIN32_LDAP
      अक्षर *attr = curlx_convert_tchar_to_UTF8(attribute);
      अगर(!attr) {
        ldap_memfree(attribute);
        परिणाम = CURLE_OUT_OF_MEMORY;
        जाओ quit;
      }
#अन्यथा
      अक्षर *attr = attribute;
#पूर्ण_अगर
      vals = ldap_get_मूल्यावली_len(server, entryIterator, attribute);
      अगर(vals) {
        परिणाम = show_vals(तथ्य, vals, attr);
        /* Free memory used to store values */
        ldap_मूल्य_free_len(vals);
      }

      /* Free the attribute as we are done with it */
      FREE_ON_WINLDAP(attr);
      ldap_memfree(attribute);

      अगर(!परिणाम)
        परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, "\n", 1);
      अगर(परिणाम)
        जाओ quit;
    }

    अगर(ber) {
      ber_free(ber, 0);
      ber = शून्य;
    }
  }

quit:
  अगर(ber)
    ber_free(ber, 0);
  अगर(ldapmsg) {
    ldap_msgfree(ldapmsg);
    LDAP_TRACE(("Received %d entries\n", अंक));
  }
  अगर(rc == LDAP_SIZELIMIT_EXCEEDED)
    infof(तथ्य, "There are more than %d entries", अंक);
  अगर(ludp)
    ldap_free_urldesc(ludp);
  अगर(server)
    ldap_unbind_s(server);

  FREE_ON_WINLDAP(host);

  /* no data to transfer */
  Curl_xfer_setup_nop(तथ्य);
  connclose(conn);

  वापस परिणाम;
}

#अगर_घोषित DEBUG_LDAP
अटल व्योम ldap_trace_निचला(स्थिर अक्षर *fmt, ...)
{
  अटल पूर्णांक do_trace = -1;
  बहु_सूची args;

  अगर(do_trace == -1) {
    स्थिर अक्षर *env = दो_पर्या("CURL_TRACE");
    curl_off_t e = 0;
    अगर(!curlx_str_संख्या(&env, &e, पूर्णांक_उच्च))
      do_trace = e > 0;
  }
  अगर(!do_trace)
    वापस;

  बहु_शुरू(args, fmt);
  curl_mvfprintf(मानक_त्रुटि, fmt, args);
  बहु_पूर्ण(args);
}
#पूर्ण_अगर /* DEBUG_LDAP */

#अगर_अघोषित HAVE_LDAP_URL_PARSE

/*
 * Return scope-value for a scope-string.
 */
अटल पूर्णांक str2scope(स्थिर अक्षर *p)
{
  अगर(curl_strequal(p, "one"))
    वापस LDAP_SCOPE_ONELEVEL;
  अगर(curl_strequal(p, "onetree"))
    वापस LDAP_SCOPE_ONELEVEL;
  अगर(curl_strequal(p, "base"))
    वापस LDAP_SCOPE_BASE;
  अगर(curl_strequal(p, "sub"))
    वापस LDAP_SCOPE_SUBTREE;
  अगर(curl_strequal(p, "subtree"))
    वापस LDAP_SCOPE_SUBTREE;
  वापस -1;
}

/* number of entries in the attributes list */
अटल माप_प्रकार अंक_entries(स्थिर अक्षर *s)
{
  माप_प्रकार वस्तुएँ = 1;

  s = म_अक्षर(s, ',');
  जबतक(s) {
    वस्तुएँ++;
    s = म_अक्षर(s + 1, ',');
  }
  वापस वस्तुएँ;
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
अटल curl_ldap_अंक_t ldap_url_parse2_निचला(काष्ठा Curl_easy *तथ्य,
                                           स्थिर काष्ठा connectdata *conn,
                                           LDAPURLDesc *ludp)
{
  curl_ldap_अंक_t rc = LDAP_SUCCESS;
  अक्षर *p;
  अक्षर *path;
  अक्षर *q = शून्य;
  अक्षर *query = शून्य;
  माप_प्रकार i;

  अगर(!तथ्य ||
     !तथ्य->state.up.path ||
     तथ्य->state.up.path[0] != '/' ||
     ((तथ्य->state.origin->scheme != &Curl_scheme_ldap) &&
      (तथ्य->state.origin->scheme != &Curl_scheme_ldaps)))
    वापस LDAP_INVALID_SYNTAX;

  ludp->lud_scope = LDAP_SCOPE_BASE;
  ludp->lud_port  = conn->origin->port;
  ludp->lud_host  = conn->origin->hostname;

  /* Duplicate the path */
  p = path = curlx_strdup(तथ्य->state.up.path + 1);
  अगर(!path)
    वापस LDAP_NO_MEMORY;

  /* Duplicate the query if present */
  अगर(तथ्य->state.up.query) {
    q = query = curlx_strdup(तथ्य->state.up.query);
    अगर(!query) {
      curlx_free(path);
      वापस LDAP_NO_MEMORY;
    }
  }

  /* Parse the DN (Distinguished Name) */
  अगर(*p) {
    अक्षर *dn = p;
    अक्षर *unescaped;
    CURLcode परिणाम;

    LDAP_TRACE(("DN '%s'\n", dn));

    /* Unescape the DN */
    परिणाम = Curl_urldecode(dn, 0, &unescaped, शून्य, REJECT_ZERO);
    अगर(परिणाम) {
      rc = LDAP_NO_MEMORY;

      जाओ quit;
    }

#अगर_घोषित USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_dn = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    अगर(!ludp->lud_dn) {
      rc = LDAP_NO_MEMORY;

      जाओ quit;
    }
#अन्यथा
    ludp->lud_dn = unescaped;
#पूर्ण_अगर
  }

  p = q;
  अगर(!p)
    जाओ quit;

  /* Parse the attributes. skip "??" */
  q = म_अक्षर(p, '?');
  अगर(q)
    *q++ = '\0';

  अगर(*p) {
    माप_प्रकार गिनती = अंक_entries(p); /* at least one */
    स्थिर अक्षर *atp = p;

    /* Allocate our array (+1 for the NULL entry) */
#अगर_घोषित USE_WIN32_LDAP
    ludp->lud_attrs = curlx_calloc(गिनती + 1, माप(TCHAR *));
#अन्यथा
    ludp->lud_attrs = curlx_calloc(गिनती + 1, माप(अक्षर *));
#पूर्ण_अगर
    अगर(!ludp->lud_attrs) {
      rc = LDAP_NO_MEMORY;
      जाओ quit;
    }

    क्रम(i = 0; i < गिनती; i++) {
      अक्षर *unescaped;
      CURLcode परिणाम;
      काष्ठा Curl_str out;

      अगर(curlx_str_until(&atp, &out, 1024, ','))
        अवरोध;

      LDAP_TRACE(("attr[%zu] '%.*s'\n", i, (पूर्णांक)out.len, out.str));

      /* Unescape the attribute */
      परिणाम = Curl_urldecode(out.str, out.len, &unescaped, शून्य, REJECT_ZERO);
      अगर(परिणाम) {
        rc = LDAP_NO_MEMORY;
        जाओ quit;
      }

#अगर_घोषित USE_WIN32_LDAP
      /* Convert the unescaped string to a tchar */
      ludp->lud_attrs[i] = curlx_convert_UTF8_to_tchar(unescaped);

      /* Free the unescaped string as we are done with it */
      curlx_free(unescaped);

      अगर(!ludp->lud_attrs[i]) {
        rc = LDAP_NO_MEMORY;
        जाओ quit;
      }
#अन्यथा
      ludp->lud_attrs[i] = unescaped;
#पूर्ण_अगर

      ludp->lud_attrs_dups++;
      अगर(curlx_str_single(&atp, ','))
        अवरोध;
    }
  }

  p = q;
  अगर(!p)
    जाओ quit;

  /* Parse the scope. skip "??" */
  q = म_अक्षर(p, '?');
  अगर(q)
    *q++ = '\0';

  अगर(*p) {
    ludp->lud_scope = str2scope(p);
    अगर(ludp->lud_scope == -1) {
      rc = LDAP_INVALID_SYNTAX;

      जाओ quit;
    }
    LDAP_TRACE(("scope %d\n", ludp->lud_scope));
  }

  p = q;
  अगर(!p)
    जाओ quit;

  /* Parse the filter */
  q = म_अक्षर(p, '?');
  अगर(q)
    *q++ = '\0';

  अगर(*p) {
    अक्षर *filter = p;
    अक्षर *unescaped;
    CURLcode परिणाम;

    LDAP_TRACE(("filter '%s'\n", filter));

    /* Unescape the filter */
    परिणाम = Curl_urldecode(filter, 0, &unescaped, शून्य, REJECT_CTRL);
    अगर(परिणाम) {
      rc = LDAP_NO_MEMORY;

      जाओ quit;
    }

#अगर_घोषित USE_WIN32_LDAP
    /* Convert the unescaped string to a tchar */
    ludp->lud_filter = curlx_convert_UTF8_to_tchar(unescaped);

    /* Free the unescaped string as we are done with it */
    curlx_free(unescaped);

    अगर(!ludp->lud_filter) {
      rc = LDAP_NO_MEMORY;

      जाओ quit;
    }
#अन्यथा
    ludp->lud_filter = unescaped;
#पूर्ण_अगर
  }

  p = q;
  अगर(p && !*p) {
    rc = LDAP_INVALID_SYNTAX;

    जाओ quit;
  }

quit:
  curlx_free(path);
  curlx_free(query);

  वापस rc;
}

अटल curl_ldap_अंक_t ldap_url_parse_निचला(काष्ठा Curl_easy *तथ्य,
                                          स्थिर काष्ठा connectdata *conn,
                                          LDAPURLDesc **ludpp)
{
  LDAPURLDesc *ludp = curlx_calloc(1, माप(*ludp));
  curl_ldap_अंक_t rc;

  *ludpp = शून्य;
  अगर(!ludp)
    वापस LDAP_NO_MEMORY;

  rc = ldap_url_parse2_निचला(तथ्य, conn, ludp);
  अगर(rc != LDAP_SUCCESS) {
    ldap_free_urldesc_निचला(ludp);
    ludp = शून्य;
  }
  *ludpp = ludp;
  वापस rc;
}

अटल व्योम ldap_free_urldesc_निचला(LDAPURLDesc *ludp)
{
  अगर(!ludp)
    वापस;

  curlx_free(ludp->lud_dn);
  curlx_free(ludp->lud_filter);

  अगर(ludp->lud_attrs) {
    माप_प्रकार i;
    क्रम(i = 0; i < ludp->lud_attrs_dups; i++) {
      curlx_free(ludp->lud_attrs[i]);
    }
    curlx_free(ludp->lud_attrs);
  }

  curlx_free(ludp);
}
#पूर्ण_अगर /* !HAVE_LDAP_URL_PARSE */

व्योम Curl_ldap_version(अक्षर *buf, माप_प्रकार bufsz)
{
#अगर_घोषित USE_WIN32_LDAP
  curl_msnprintf(buf, bufsz, "WinLDAP");
#अन्यथा
#अगर_घोषित LDAP_OPT_X_TLS_PASSPHRASE
  अटल स्थिर अक्षर flavor[] = "/Apple";
#अन्यथा
  अटल स्थिर अक्षर flavor[] = "";
#पूर्ण_अगर
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  /* Comparing against 0, as different platforms
     disagree on the success define name */
  अगर(ldap_get_option(शून्य, LDAP_OPT_API_INFO, &api) == 0) {
    अचिन्हित पूर्णांक patch = (अचिन्हित पूर्णांक)(api.ldapai_vendor_version % 100);
    अचिन्हित पूर्णांक major = (अचिन्हित पूर्णांक)(api.ldapai_vendor_version / 10000);
    अचिन्हित पूर्णांक minor =
      (((अचिन्हित पूर्णांक)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
#अगर_घोषित __OS400__
    curl_msnprintf(buf, bufsz, "IBMLDAP/%u.%u.%u", major, minor, patch);
    ldap_मूल्य_free(api.ldapai_extensions);
    (व्योम)flavor;
#अन्यथा
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u%s",
                   api.ldapai_vendor_name, major, minor, patch, flavor);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((व्योम **)api.ldapai_extensions);
#पूर्ण_अगर
  }
  अन्यथा
    curl_msnprintf(buf, bufsz, "LDAP/1");
#पूर्ण_अगर /* USE_WIN32_LDAP */
}

/*
 * LDAP protocol handler.
 */
स्थिर काष्ठा Curl_protocol Curl_protocol_ldap = {
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

#अगर defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#आशय GCC diagnostic निकालो
#पूर्ण_अगर

#पूर्ण_अगर /* !CURL_DISABLE_LDAP && !USE_OPENLDAP */
