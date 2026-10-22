/***************************************************************************
 *                      _   _ ____  _
 *  Project         ___| | | |  _ \| |
 *                 / __| | | | |_) | |
 *                | (__| |_| |  _ <| |___
 *                 \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 * Copyright (C) Howard Chu, <hyc@openldap.org>
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

#अगर !defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP)

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

#समावेश <ldap.h>

#समावेश "urldata.h"
#समावेश "url.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "vtls/vtls.h"
#समावेश "transfer.h"
#समावेश "curl_ldap.h"
#समावेश "curlx/base64.h"
#समावेश "cfilters.h"
#समावेश "connect.h"
#समावेश "curl_sasl.h"
#समावेश "strcase.h"
#समावेश "bufref.h"

/*
 * Uncommenting this will enable the built-in debug logging of the openldap
 * library. The debug log level can be set using the CURL_OPENLDAP_TRACE
 * environment variable. The debug output is written to stderr.
 *
 * The library supports the following debug flags:
 * LDAP_DEBUG_NONE         0x0000
 * LDAP_DEBUG_TRACE        0x0001
 * LDAP_DEBUG_CONSTRUCT    0x0002
 * LDAP_DEBUG_DESTROY      0x0004
 * LDAP_DEBUG_PARAMETER    0x0008
 * LDAP_DEBUG_ANY          0xffff
 *
 * For example, use CURL_OPENLDAP_TRACE=0 for no debug,
 * CURL_OPENLDAP_TRACE=2 for LDAP_DEBUG_CONSTRUCT messages only,
 * CURL_OPENLDAP_TRACE=65535 for all debug message levels.
 */
/* #define CURL_OPENLDAP_DEBUG */

/* Machine states. */
प्रकार क्रमागत {
  OLDAP_STOP,           /* Do nothing state, stops the state machine */
  OLDAP_SSL,            /* Performing SSL handshake. */
  OLDAP_STARTTLS,       /* STARTTLS request sent. */
  OLDAP_TLS,            /* Performing TLS handshake. */
  OLDAP_MECHS,          /* Get SASL authentication mechanisms. */
  OLDAP_SASL,           /* SASL binding reply. */
  OLDAP_BIND,           /* Simple bind reply. */
  OLDAP_BINDV2,         /* Simple bind reply in protocol version 2. */
  OLDAP_LAST            /* Never used */
} ldapstate;

#अगर_अघोषित _LDAP_PVT_H
बाह्य पूर्णांक ldap_pvt_url_scheme2proto(स्थिर अक्षर *);
बाह्य पूर्णांक ldap_init_fd(ber_socket_t fd, पूर्णांक proto, स्थिर अक्षर *url,
                        LDAP **ld);
#पूर्ण_अगर

अटल Curl_recv oldap_recv;

काष्ठा ldapconninfo {
  काष्ठा SASL sasl;          /* SASL-related parameters */
  LDAP *ld;                  /* Openldap connection handle. */
  Curl_recv *recv;           /* For stacking SSL handler */
  Curl_send *send;
  काष्ठा berval *servercred; /* SASL data from server. */
  ldapstate state;           /* Current machine state. */
  curl_socket_t dupfd;       /* duplicated socket */
  पूर्णांक proto;                 /* LDAP_PROTO_TCP/LDAP_PROTO_UDP/LDAP_PROTO_IPC */
  पूर्णांक msgid;                 /* Current message id. */
};

काष्ठा ldapreqinfo {
  पूर्णांक msgid;
  पूर्णांक nument;
};

/* meta key for storing ldapconninfo at easy handle */
#घोषणा CURL_META_LDAP_EASY   "meta:proto:ldap:easy"
/* meta key for storing ldapconninfo at connection */
#घोषणा CURL_META_LDAP_CONN   "meta:proto:ldap:conn"

/*
 * oldap_state()
 *
 * This is the ONLY way to change LDAP state!
 */
अटल व्योम oldap_state(काष्ठा Curl_easy *तथ्य, काष्ठा ldapconninfo *li,
                        ldapstate newstate)
{
#अगर defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  अटल स्थिर अक्षर * स्थिर names[] = {
    "STOP",
    "SSL",
    "STARTTLS",
    "TLS",
    "MECHS",
    "SASL",
    "BIND",
    "BINDV2",
    /* LAST */
  };

  अगर(li->state != newstate)
    infof(तथ्य, "LDAP %p state change from %s to %s",
          (व्योम *)li, names[li->state], names[newstate]);
#अन्यथा
  (व्योम)तथ्य;
#पूर्ण_अगर
  li->state = newstate;
}

/* Map some particular LDAP error codes to CURLcode values. */
अटल CURLcode oldap_map_error(पूर्णांक rc, CURLcode परिणाम)
{
  चयन(rc) {
  हाल LDAP_NO_MEMORY:
    वापस CURLE_OUT_OF_MEMORY;
  हाल LDAP_INVALID_CREDENTIALS:
    वापस CURLE_LOGIN_DENIED;
  हाल LDAP_PROTOCOL_ERROR:
    वापस CURLE_UNSUPPORTED_PROTOCOL;
  हाल LDAP_INSUFFICIENT_ACCESS:
    वापस CURLE_REMOTE_ACCESS_DENIED;
  }
  वापस परिणाम;
}

अटल CURLcode oldap_url_parse(काष्ठा Curl_easy *तथ्य, LDAPURLDesc **ludp)
{
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक rc;
  अटल स्थिर अक्षर * स्थिर url_errs[] = {
    "success",
    "out of memory",
    "bad parameter",
    "unrecognized scheme",
    "unbalanced delimiter",
    "bad URL",
    "bad host or port",
    "bad or missing attributes",
    "bad or missing scope",
    "bad or missing filter",
    "bad or missing extensions"
  };

  *ludp = शून्य;
  /* `ldap_url_parse() seems to be terrible with urls
   * that have user/pass/options in it. So when we have options or
   * creds from the url, fail without calling the function.
   * Yes, this is super-weird code and I do not like it. */
  अगर((तथ्य->state.creds && (तथ्य->state.creds->source == CREDS_URL)) ||
     तथ्य->state.up.options)
    rc = LDAP_URL_ERR_BADURL;
  अन्यथा
    rc = ldap_url_parse(Curl_bufref_ptr(&तथ्य->state.url), ludp);

  अगर(rc != LDAP_URL_SUCCESS) {
    स्थिर अक्षर *संदेश = "url parsing problem";

    परिणाम = rc == LDAP_URL_ERR_MEM ? CURLE_OUT_OF_MEMORY :
      CURLE_URL_MALFORMAT;
    rc -= LDAP_URL_SUCCESS;
    अगर((माप_प्रकार)rc < CURL_ARRAYSIZE(url_errs))
      संदेश = url_errs[rc];
    failf(तथ्य, "LDAP local: %s", संदेश);
  }
  वापस परिणाम;
}

/* Parse the login options. */
अटल CURLcode oldap_parse_login_options(काष्ठा connectdata *conn)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  स्थिर अक्षर *ptr = conn->options;

  DEBUGASSERT(li);
  अगर(!li)
    वापस CURLE_FAILED_INIT;

  जबतक(!परिणाम && ptr && *ptr) {
    स्थिर अक्षर *कुंजी = ptr;
    स्थिर अक्षर *मूल्य;

    जबतक(*ptr && *ptr != '=')
      ptr++;

    मूल्य = ptr + 1;

    जबतक(*ptr && *ptr != ';')
      ptr++;

    अगर(checkprefix("AUTH=", कुंजी))
      परिणाम = Curl_sasl_parse_url_auth_option(&li->sasl, मूल्य, ptr - मूल्य);
    अन्यथा
      परिणाम = CURLE_SETOPT_OPTION_SYNTAX;

    अगर(*ptr == ';')
      ptr++;
  }

  वापस परिणाम == CURLE_URL_MALFORMAT ? CURLE_SETOPT_OPTION_SYNTAX : परिणाम;
}

अटल CURLcode oldap_setup_connection(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा connectdata *conn)
{
  CURLcode परिणाम;
  LDAPURLDesc *lud;
  (व्योम)conn;

  /* Early URL syntax check. */
  परिणाम = oldap_url_parse(तथ्य, &lud);
  ldap_free_urldesc(lud);

  वापस परिणाम;
}

/*
 * Get the SASL authentication challenge from the server credential buffer.
 */
अटल CURLcode oldap_get_message(काष्ठा Curl_easy *तथ्य, काष्ठा bufref *out)
{
  काष्ठा ldapconninfo *li =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_LDAP_CONN);
  काष्ठा berval *servercred = li ? li->servercred : शून्य;
  DEBUGASSERT(li);
  अगर(!li)
    वापस CURLE_FAILED_INIT;

  अगर(!servercred || !servercred->bv_val)
    वापस CURLE_WEIRD_SERVER_REPLY;
  Curl_bufref_set(out, servercred->bv_val, servercred->bv_len, शून्य);
  वापस CURLE_OK;
}

/*
 * Sends an initial SASL bind request to the server.
 */
अटल CURLcode oldap_perform_auth(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *mech,
                                   स्थिर काष्ठा bufref *initresp)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  काष्ठा berval cred;
  काष्ठा berval *pcred = &cred;
  पूर्णांक rc;

  DEBUGASSERT(li);
  अगर(!li)
    वापस CURLE_FAILED_INIT;
  cred.bv_val = (अक्षर *)CURL_UNCONST(Curl_bufref_ptr(initresp));
  cred.bv_len = Curl_bufref_len(initresp);
  अगर(!cred.bv_val)
    pcred = शून्य;
  rc = ldap_sasl_bind(li->ld, शून्य, mech, pcred, शून्य, शून्य, &li->msgid);
  अगर(rc != LDAP_SUCCESS)
    वापस oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  वापस CURLE_OK;
}

/*
 * Sends SASL continuation.
 */
अटल CURLcode oldap_continue_auth(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *mech,
                                    स्थिर काष्ठा bufref *resp)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  काष्ठा berval cred;
  काष्ठा berval *pcred = &cred;
  पूर्णांक rc;

  अगर(!li)
    वापस CURLE_FAILED_INIT;
  cred.bv_val = (अक्षर *)CURL_UNCONST(Curl_bufref_ptr(resp));
  cred.bv_len = Curl_bufref_len(resp);
  अगर(!cred.bv_val)
    pcred = शून्य;
  rc = ldap_sasl_bind(li->ld, शून्य, mech, pcred, शून्य, शून्य, &li->msgid);
  अगर(rc != LDAP_SUCCESS)
    वापस oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  वापस CURLE_OK;
}

/*
 * Sends SASL bind cancellation.
 */
अटल CURLcode oldap_cancel_auth(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *mech)
{
  काष्ठा ldapconninfo *li =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_LDAP_CONN);
  पूर्णांक rc;

  (व्योम)mech;
  अगर(!li)
    वापस CURLE_FAILED_INIT;
  rc = ldap_sasl_bind(li->ld, शून्य, LDAP_SASL_NULL, शून्य, शून्य, शून्य,
                      &li->msgid);
  अगर(rc != LDAP_SUCCESS)
    वापस oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  वापस CURLE_OK;
}

/* Starts LDAP simple bind. */
अटल CURLcode oldap_perform_bind(काष्ठा Curl_easy *तथ्य, ldapstate newstate)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  स्थिर अक्षर *binddn = शून्य;
  काष्ठा berval passwd;
  पूर्णांक rc;

  अगर(!li)
    वापस CURLE_FAILED_INIT;
  passwd.bv_val = शून्य;
  passwd.bv_len = 0;

  अगर(conn->creds) {
    binddn = Curl_creds_user(conn->creds);
    passwd.bv_val = CURL_UNCONST(Curl_creds_passwd(conn->creds));
    passwd.bv_len = म_माप(passwd.bv_val);
  }

  rc = ldap_sasl_bind(li->ld, binddn, LDAP_SASL_SIMPLE, &passwd,
                      शून्य, शून्य, &li->msgid);
  अगर(rc != LDAP_SUCCESS)
    वापस oldap_map_error(rc,
                           तथ्य->state.creds ?
                           CURLE_LOGIN_DENIED : CURLE_LDAP_CANNOT_BIND);
  oldap_state(तथ्य, li, newstate);
  वापस CURLE_OK;
}

/* Query the supported SASL authentication mechanisms. */
अटल CURLcode oldap_perform_mechs(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ldapconninfo *li =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_LDAP_CONN);
  पूर्णांक rc;
  अटल स्थिर अक्षर * स्थिर supportedSASLMechanisms[] = {
    "supportedSASLMechanisms",
    शून्य
  };

  अगर(!li)
    वापस CURLE_FAILED_INIT;
  /* Casting away the const for the 3rd parameter that the LDAP API expects as
     a non-const char ** is potentially unsafe but we believe the lack of
     const in the API was an oversight and that no LDAP implementation
     actually modifies the input. */
  rc = ldap_खोज_ext(li->ld, "", LDAP_SCOPE_BASE, "(objectclass=*)",
                       (अक्षर **)CURL_UNCONST(supportedSASLMechanisms), 0,
                       शून्य, शून्य, शून्य, 0, &li->msgid);
  अगर(rc != LDAP_SUCCESS)
    वापस oldap_map_error(rc, CURLE_LOGIN_DENIED);
  oldap_state(तथ्य, li, OLDAP_MECHS);
  वापस CURLE_OK;
}

/* Starts SASL bind. */
अटल CURLcode oldap_perform_sasl(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ldapconninfo *li =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_LDAP_CONN);
  saslprogress progress = SASL_IDLE;
  CURLcode परिणाम;

  अगर(!li)
    वापस CURLE_FAILED_INIT;
  परिणाम = Curl_sasl_start(&li->sasl, तथ्य, TRUE, &progress);

  oldap_state(तथ्य, li, OLDAP_SASL);
  अगर(!परिणाम && progress != SASL_INPROGRESS)
    परिणाम = Curl_sasl_is_blocked(&li->sasl, तथ्य);
  वापस परिणाम;
}

#अगर_घोषित USE_SSL
अटल पूर्णांक ldapsb_tls_setup(Sockbuf_IO_Desc *sbiod, व्योम *arg)
{
  sbiod->sbiod_pvt = arg;
  वापस 0;
}

अटल पूर्णांक ldapsb_tls_remove(Sockbuf_IO_Desc *sbiod)
{
  sbiod->sbiod_pvt = शून्य;
  वापस 0;
}

/* if there is a duplicated socket to close, close it */
अटल पूर्णांक ldapsb_tls_close(Sockbuf_IO_Desc *sbiod)
{
  काष्ठा Curl_easy *तथ्य = sbiod->sbiod_pvt;
  अगर(तथ्य) {
    काष्ठा connectdata *conn = तथ्य->conn;
    अगर(conn) {
      काष्ठा ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      अगर(li && (li->dupfd != CURL_SOCKET_BAD)) {
        sclose(li->dupfd);
        li->dupfd = CURL_SOCKET_BAD;
      }
    }
  }

  वापस 0;
}

अटल पूर्णांक ldapsb_tls_ctrl(Sockbuf_IO_Desc *sbiod, पूर्णांक opt, व्योम *arg)
{
  (व्योम)arg;
  अगर(opt == LBER_SB_OPT_तथ्य_READY) {
    काष्ठा Curl_easy *तथ्य = sbiod->sbiod_pvt;
    वापस Curl_conn_तथ्य_pending(तथ्य, FIRSTSOCKET);
  }
  वापस 0;
}

अटल ber_slen_t ldapsb_tls_read(Sockbuf_IO_Desc *sbiod, व्योम *buf,
                                  ber_len_t len)
{
  काष्ठा Curl_easy *तथ्य = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  अगर(तथ्य) {
    काष्ठा connectdata *conn = तथ्य->conn;
    अगर(conn) {
      काष्ठा ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode परिणाम = CURLE_RECV_ERROR;
      माप_प्रकार nread;

      अगर(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        वापस -1;
      }
      परिणाम = (li->recv)(तथ्य, FIRSTSOCKET, buf, len, &nread);
      अगर(परिणाम == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = परिणाम ? -1 : (ber_slen_t)nread;
    }
  }
  वापस ret;
}

अटल ber_slen_t ldapsb_tls_write(Sockbuf_IO_Desc *sbiod, व्योम *buf,
                                   ber_len_t len)
{
  काष्ठा Curl_easy *तथ्य = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  अगर(तथ्य) {
    काष्ठा connectdata *conn = तथ्य->conn;
    अगर(conn) {
      काष्ठा ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode परिणाम = CURLE_SEND_ERROR;
      माप_प्रकार nwritten;

      अगर(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        वापस -1;
      }
      परिणाम = (li->send)(तथ्य, FIRSTSOCKET, buf, len, FALSE, &nwritten);
      अगर(परिणाम == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = परिणाम ? -1 : (ber_slen_t)nwritten;
    }
  }
  वापस ret;
}

अटल Sockbuf_IO ldapsb_tls = {
  ldapsb_tls_setup,
  ldapsb_tls_remove,
  ldapsb_tls_ctrl,
  ldapsb_tls_read,
  ldapsb_tls_write,
  ldapsb_tls_close
};

अटल bool ssl_installed(काष्ठा connectdata *conn)
{
  काष्ठा ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  वापस li && li->recv;
}

अटल CURLcode oldap_ssl_connect(काष्ठा Curl_easy *तथ्य, ldapstate newstate)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  bool ssldone = FALSE;
  CURLcode परिणाम;

  अगर(!li)
    वापस CURLE_FAILED_INIT;
  परिणाम = Curl_conn_connect(तथ्य, FIRSTSOCKET, FALSE, &ssldone);
  अगर(परिणाम)
    वापस परिणाम;
  oldap_state(तथ्य, li, newstate);

  अगर(ssldone) {
    Sockbuf *sb;

    /* Install the libcurl SSL handlers into the sockbuf. */
    अगर((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, तथ्य))
      वापस CURLE_FAILED_INIT;
    li->recv = conn->recv[FIRSTSOCKET];
    li->send = conn->send[FIRSTSOCKET];
  }

  वापस परिणाम;
}

/* Send the STARTTLS request */
अटल CURLcode oldap_perform_starttls(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ldapconninfo *li =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_LDAP_CONN);
  पूर्णांक rc;

  अगर(!li)
    वापस CURLE_FAILED_INIT;
  rc = ldap_start_tls(li->ld, शून्य, शून्य, &li->msgid);
  अगर(rc != LDAP_SUCCESS)
    वापस oldap_map_error(rc, CURLE_USE_SSL_FAILED);
  oldap_state(तथ्य, li, OLDAP_STARTTLS);
  वापस CURLE_OK;
}
#पूर्ण_अगर

अटल व्योम oldap_easy_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा ldapreqinfo *lr = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  curlx_free(lr);
}

अटल व्योम oldap_conn_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा ldapconninfo *li = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  अगर(li->ld) {
    ldap_unbind_ext(li->ld, शून्य, शून्य);
    li->ld = शून्य;
  }
  curlx_free(li);
}

/* SASL parameters for the ldap protocol */
अटल स्थिर काष्ठा SASLproto saslldap = {
  "ldap",                     /* The service name */
  oldap_perform_auth,         /* Send authentication command */
  oldap_continue_auth,        /* Send authentication continuation */
  oldap_cancel_auth,          /* Send authentication cancellation */
  oldap_get_message,          /* Get SASL response message */
  0,                          /* Maximum initial response length (no max) */
  LDAP_SASL_BIND_IN_PROGRESS, /* Code received when continuation is expected */
  LDAP_SUCCESS,               /* Code to receive upon authentication success */
  SASL_AUTH_NONE,             /* Default mechanisms */
  0                           /* Configuration flags */
};

अटल CURLcode oldap_connect(काष्ठा Curl_easy *तथ्य, bool *done)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ldapconninfo *li;
  अटल स्थिर पूर्णांक version = LDAP_VERSION3;
  अक्षर *hosturl = शून्य;
  CURLcode परिणाम;
  पूर्णांक rc;
#अगर_घोषित CURL_OPENLDAP_DEBUG
  अटल पूर्णांक do_trace = -1;
#पूर्ण_अगर

  (व्योम)done;

  li = curlx_calloc(1, माप(काष्ठा ldapconninfo));
  अगर(!li) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  li->dupfd = CURL_SOCKET_BAD;

  परिणाम = Curl_conn_meta_set(conn, CURL_META_LDAP_CONN, li, oldap_conn_dtor);
  अगर(परिणाम)
    जाओ out;

  li->proto = ldap_pvt_url_scheme2proto(तथ्य->state.origin->scheme->name);

  /* Initialize the SASL storage */
  Curl_sasl_init(&li->sasl, तथ्य, &saslldap);

  परिणाम = oldap_parse_login_options(conn);
  अगर(परिणाम)
    जाओ out;

  hosturl = curl_maprintf("%s://%s:%u",
                          conn->scheme->name,
                          conn->origin->ipv6 ?
                          conn->origin->user_hostname :
                          conn->origin->hostname,
                          conn->origin->port);
  अगर(!hosturl) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  {
#अगर_घोषित _WIN32
    SOCKET dupfd = INVALID_SOCKET;
    WSAPROTOCOL_INFO pi;
    अगर(WSADuplicateSocket(conn->sock[FIRSTSOCKET], GetCurrentProcessId(),
                          &pi))
      ; /* error */
    अन्यथा
      dupfd = CURL_SOCKET(pi.iAddressFamily, pi.iSocketType, pi.iProtocol);
#अन्यथा
#अगर_घोषित F_DUPFD_CLOEXEC
    पूर्णांक dupfd = fcntl(conn->sock[FIRSTSOCKET], F_DUPFD_CLOEXEC, 0);
#अन्यथा
    पूर्णांक dupfd = dup(conn->sock[FIRSTSOCKET]);
#अगर_घोषित HAVE_FCNTL
    अगर((dupfd != CURL_SOCKET_BAD) &&
       (fcntl(dupfd, F_SETFD, FD_CLOEXEC) < 0)) {
      sclose(dupfd);
      dupfd = CURL_SOCKET_BAD;
    }
#पूर्ण_अगर /* HAVE_FCNTL */
#पूर्ण_अगर /* F_DUPFD_CLOEXEC */
#पूर्ण_अगर /* _WIN32 */

    अगर(dupfd == CURL_SOCKET_BAD) {
      परिणाम = CURLE_COULDNT_CONNECT;
      जाओ out;
    }
    rc = ldap_init_fd((ber_socket_t)dupfd, li->proto, hosturl, &li->ld);
    अगर(rc)
      sclose(dupfd);
    अन्यथा
      /* store the socket, as when this is used by TLS we close this ourselves
         and at this point the code does not yet know if this will use TLS or
         not */
      li->dupfd = dupfd;
  }
  अगर(rc) {
    failf(तथ्य, "LDAP local: Cannot connect to %s, %s",
          hosturl, ldap_err2string(rc));
    परिणाम = CURLE_COULDNT_CONNECT;
    जाओ out;
  }

#अगर_घोषित CURL_OPENLDAP_DEBUG
  अगर(do_trace < 0) {
    स्थिर अक्षर *env = दो_पर्या("CURL_OPENLDAP_TRACE");
    curl_off_t e = 0;
    अगर(!curlx_str_संख्या(&env, &e, पूर्णांक_उच्च))
      do_trace = e > 0;
  }
  अगर(do_trace)
    ldap_set_option(li->ld, LDAP_OPT_DEBUG_LEVEL, &do_trace);
#पूर्ण_अगर

  /* Try version 3 first. */
  ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);

  /* Do not chase referrals. */
  ldap_set_option(li->ld, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  {
    ber_len_t max = 256 * 1024;
    Sockbuf *sb;
    अगर((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       /* Set the maximum allowed size of an incoming message, which to
          OpenLDAP means that it will malloc() memory up to this size. If not
          set, there is no limit and we instead risk a malloc() failure. */
       !ber_sockbuf_ctrl(sb, LBER_SB_OPT_SET_MAX_INCOMING, &max)) {
      परिणाम = CURLE_FAILED_INIT;
      जाओ out;
    }
  }

#अगर_घोषित USE_SSL
  अगर(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    परिणाम = oldap_ssl_connect(तथ्य, OLDAP_SSL);
    जाओ out;
  }

  अगर(तथ्य->set.use_ssl) {
    परिणाम = oldap_perform_starttls(तथ्य);
    अगर(!परिणाम || तथ्य->set.use_ssl != CURLUSESSL_TRY)
      जाओ out;
  }
#पूर्ण_अगर

  अगर(li->sasl.prefmech != SASL_AUTH_NONE) {
    परिणाम = oldap_perform_mechs(तथ्य);
    जाओ out;
  }

  /* Force bind even if anonymous bind is not needed in protocol version 3
     to detect missing version 3 support. */
  परिणाम = oldap_perform_bind(तथ्य, OLDAP_BIND);

out:
  curlx_free(hosturl);
  वापस परिणाम;
}

/* Handle the supported SASL mechanisms query response */
अटल CURLcode oldap_state_mechs_resp(काष्ठा Curl_easy *तथ्य,
                                       LDAPMessage *संदेश, पूर्णांक code)
{
  काष्ठा connectdata *conn;
  काष्ठा ldapconninfo *li;
  पूर्णांक rc;
  BerElement *ber = शून्य;
  CURLcode परिणाम = CURLE_OK;
  काष्ठा berval bv, *bvals;

  अगर(!तथ्य)
    वापस CURLE_FAILED_INIT;

  conn = तथ्य->conn;
  li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);

  अगर(!li)
    वापस CURLE_FAILED_INIT;
  चयन(ldap_msgtype(संदेश)) {
  हाल LDAP_RES_खोज_ENTRY:
    /* Got a list of supported SASL mechanisms. */
    अगर(code != LDAP_SUCCESS && code != LDAP_NO_RESULTS_RETURNED)
      वापस CURLE_LOGIN_DENIED;

    rc = ldap_get_dn_ber(li->ld, संदेश, &ber, &bv);
    अगर(rc < 0)
      वापस oldap_map_error(rc, CURLE_BAD_CONTENT_ENCODING);
    क्रम(rc = ldap_get_attribute_ber(li->ld, संदेश, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, संदेश, ber, &bv, &bvals)) {
      पूर्णांक i;

      अगर(!bv.bv_val)
        अवरोध;

      अगर(bvals) {
        क्रम(i = 0; bvals[i].bv_val; i++) {
          माप_प्रकार llen;
          अचिन्हित लघु mech =
            Curl_sasl_decode_mech((स्थिर अक्षर *)bvals[i].bv_val,
                                  bvals[i].bv_len, &llen);
          अगर(bvals[i].bv_len == llen)
            li->sasl.authmechs |= mech;
        }
        ber_memfree(bvals);
      }
    }
    ber_free(ber, 0);
    अवरोध;

  हाल LDAP_RES_खोज_परिणाम:
    चयन(code) {
    हाल LDAP_SIZELIMIT_EXCEEDED:
      infof(तथ्य, "Too many authentication mechanisms");
      FALLTHROUGH();
    हाल LDAP_SUCCESS:
    हाल LDAP_NO_RESULTS_RETURNED:
      अगर(Curl_sasl_can_authenticate(&li->sasl, तथ्य))
        परिणाम = oldap_perform_sasl(तथ्य);
      अन्यथा
        परिणाम = CURLE_LOGIN_DENIED;
      अवरोध;
    शेष:
      परिणाम = oldap_map_error(code, CURLE_LOGIN_DENIED);
      अवरोध;
    }
    अवरोध;
  शेष:
    अवरोध;
  }
  वापस परिणाम;
}

/* Handle a SASL bind response. */
अटल CURLcode oldap_state_sasl_resp(काष्ठा Curl_easy *तथ्य,
                                      LDAPMessage *संदेश, पूर्णांक code)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode परिणाम = CURLE_OK;
  saslprogress progress;
  पूर्णांक rc;

  अगर(!li)
    वापस CURLE_FAILED_INIT;
  li->servercred = शून्य;
  rc = ldap_parse_sasl_bind_परिणाम(li->ld, संदेश, &li->servercred, 0);
  अगर(rc != LDAP_SUCCESS) {
    failf(तथ्य, "LDAP local: sasl ldap_parse_result %s", ldap_err2string(rc));
    परिणाम = oldap_map_error(rc, CURLE_LOGIN_DENIED);
  }
  अन्यथा {
    परिणाम = Curl_sasl_continue(&li->sasl, तथ्य, code, &progress);
    अगर(!परिणाम) {
      चयन(progress) {
      हाल SASL_DONE:
        oldap_state(तथ्य, li, OLDAP_STOP);   /* Authenticated */
        अवरोध;
      हाल SASL_IDLE:            /* No mechanism left after cancellation */
        failf(तथ्य, "Authentication cancelled");
        परिणाम = CURLE_LOGIN_DENIED;
        अवरोध;
      शेष:
        अवरोध;
      }
    }
  }

  अगर(li->servercred)
    ber_bvfree(li->servercred);
  वापस परिणाम;
}

/* Handle a simple bind response. */
अटल CURLcode oldap_state_bind_resp(काष्ठा Curl_easy *तथ्य, LDAPMessage *संदेश,
                                      पूर्णांक code)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode परिणाम = CURLE_OK;
  काष्ठा berval *bv = शून्य;
  पूर्णांक rc;

  अगर(!li)
    वापस CURLE_FAILED_INIT;

  अगर(code != LDAP_SUCCESS)
    वापस oldap_map_error(code, CURLE_LDAP_CANNOT_BIND);

  rc = ldap_parse_sasl_bind_परिणाम(li->ld, संदेश, &bv, 0);
  अगर(rc != LDAP_SUCCESS) {
    failf(तथ्य, "LDAP local: bind ldap_parse_sasl_bind_result %s",
          ldap_err2string(rc));
    परिणाम = oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  }
  अन्यथा
    oldap_state(तथ्य, li, OLDAP_STOP);

  अगर(bv)
    ber_bvfree(bv);
  वापस परिणाम;
}

अटल CURLcode oldap_connecting(काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  LDAPMessage *संदेश = शून्य;
  काष्ठा timeval tv = { 0, 0 };
  पूर्णांक code = LDAP_SUCCESS;
  पूर्णांक rc;

  अगर(!li)
    वापस CURLE_FAILED_INIT;

  अगर(li->state != OLDAP_SSL && li->state != OLDAP_TLS) {
    /* Get response to last command. */
    rc = ldap_परिणाम(li->ld, li->msgid, LDAP_संदेश_ONE, &tv, &संदेश);
    चयन(rc) {
    हाल 0:                               /* Timed out. */
      वापस CURLE_OK;
    हाल LDAP_RES_खोज_ENTRY:
    हाल LDAP_RES_खोज_REFERENCE:
      अवरोध;
    शेष:
      li->msgid = 0;                      /* Nothing to abandon upon error. */
      अगर(rc < 0) {
        failf(तथ्य, "LDAP local: connecting ldap_result %s",
              ldap_err2string(rc));
        वापस oldap_map_error(rc, CURLE_COULDNT_CONNECT);
      }
      अवरोध;
    }

    /* Get error code from message. */
    rc = ldap_parse_परिणाम(li->ld, संदेश, &code, शून्य, शून्य, शून्य, शून्य, 0);
    अगर(rc)
      code = rc;
    अन्यथा {
      /* store the latest code for later retrieval */
      तथ्य->info.httpcode = code;
    }

    /* If protocol version 3 is not supported, fallback to version 2. */
    अगर(code == LDAP_PROTOCOL_ERROR && li->state != OLDAP_BINDV2 &&
#अगर_घोषित USE_SSL
       (ssl_installed(conn) || तथ्य->set.use_ssl <= CURLUSESSL_TRY) &&
#पूर्ण_अगर
       li->sasl.prefmech == SASL_AUTH_NONE) {
      अटल स्थिर पूर्णांक version = LDAP_VERSION2;

      ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);
      ldap_msgfree(संदेश);
      वापस oldap_perform_bind(तथ्य, OLDAP_BINDV2);
    }
  }

  /* Handle response message according to current state. */
  चयन(li->state) {

#अगर_घोषित USE_SSL
  हाल OLDAP_SSL:
    परिणाम = oldap_ssl_connect(तथ्य, OLDAP_SSL);
    अगर(!परिणाम && ssl_installed(conn)) {
      अगर(li->sasl.prefmech != SASL_AUTH_NONE)
        परिणाम = oldap_perform_mechs(तथ्य);
      अन्यथा
        परिणाम = oldap_perform_bind(तथ्य, OLDAP_BIND);
    }
    अवरोध;
  हाल OLDAP_STARTTLS:
    अगर(code != LDAP_SUCCESS) {
      अगर(तथ्य->set.use_ssl != CURLUSESSL_TRY)
        परिणाम = oldap_map_error(code, CURLE_USE_SSL_FAILED);
      अन्यथा अगर(li->sasl.prefmech != SASL_AUTH_NONE)
        परिणाम = oldap_perform_mechs(तथ्य);
      अन्यथा
        परिणाम = oldap_perform_bind(तथ्य, OLDAP_BIND);
      अवरोध;
    }
    परिणाम = Curl_ssl_cfilter_add(
      तथ्य, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    अगर(परिणाम)
      अवरोध;
    FALLTHROUGH();
  हाल OLDAP_TLS:
    परिणाम = oldap_ssl_connect(तथ्य, OLDAP_TLS);
    अगर(परिणाम)
      परिणाम = oldap_map_error(code, CURLE_USE_SSL_FAILED);
    अन्यथा अगर(ssl_installed(conn)) {
      अगर(li->sasl.prefmech != SASL_AUTH_NONE)
        परिणाम = oldap_perform_mechs(तथ्य);
      अन्यथा अगर(तथ्य->state.creds)
        परिणाम = oldap_perform_bind(तथ्य, OLDAP_BIND);
      अन्यथा {
        /* Version 3 supported: no bind required */
        oldap_state(तथ्य, li, OLDAP_STOP);
        परिणाम = CURLE_OK;
      }
    }
    अवरोध;
#पूर्ण_अगर

  हाल OLDAP_MECHS:
    परिणाम = oldap_state_mechs_resp(तथ्य, संदेश, code);
    अवरोध;
  हाल OLDAP_SASL:
    परिणाम = oldap_state_sasl_resp(तथ्य, संदेश, code);
    अवरोध;
  हाल OLDAP_BIND:
  हाल OLDAP_BINDV2:
    परिणाम = oldap_state_bind_resp(तथ्य, संदेश, code);
    अवरोध;
  शेष:
    /* internal error */
    परिणाम = CURLE_COULDNT_CONNECT;
    अवरोध;
  }

  ldap_msgfree(संदेश);

  *done = li->state == OLDAP_STOP;
  अगर(*done)
    conn->recv[FIRSTSOCKET] = oldap_recv;

  अगर(परिणाम && li->msgid) {
    ldap_abandon_ext(li->ld, li->msgid, शून्य, शून्य);
    li->msgid = 0;
  }
  वापस परिणाम;
}

अटल CURLcode oldap_disconnect(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा connectdata *conn,
                                 bool dead_connection)
{
  काष्ठा ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  (व्योम)dead_connection;
#अगर_अघोषित USE_SSL
  (व्योम)तथ्य;
#पूर्ण_अगर

  अगर(li && li->ld) {
#अगर_घोषित USE_SSL
    अगर(ssl_installed(conn)) {
      Sockbuf *sb;
      अगर(ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS ||
         ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, तथ्य))
        वापस CURLE_FAILED_INIT;
    }
#पूर्ण_अगर
    ldap_unbind_ext(li->ld, शून्य, शून्य);
    li->ld = शून्य;
  }
  वापस CURLE_OK;
}

अटल CURLcode oldap_do(काष्ठा Curl_easy *तथ्य, bool *done)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  काष्ठा ldapreqinfo *lr;
  CURLcode परिणाम;
  पूर्णांक rc;
  LDAPURLDesc *lud;
  पूर्णांक msgid;

  अगर(!li)
    वापस CURLE_FAILED_INIT;

  infof(तथ्य, "LDAP local: %s", Curl_bufref_ptr(&तथ्य->state.url));

  परिणाम = oldap_url_parse(तथ्य, &lud);
  अगर(परिणाम)
    जाओ out;

#अगर_घोषित USE_SSL
  अगर(ssl_installed(conn)) {
    Sockbuf *sb;
    /* re-install the libcurl SSL handlers into the sockbuf. */
    अगर((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, तथ्य)) {
      ldap_free_urldesc(lud);
      वापस CURLE_FAILED_INIT;
    }
  }
#पूर्ण_अगर

  rc = ldap_खोज_ext(li->ld, lud->lud_dn, lud->lud_scope,
                       lud->lud_filter, lud->lud_attrs, 0,
                       शून्य, शून्य, शून्य, 0, &msgid);
  ldap_free_urldesc(lud);
  अगर(rc != LDAP_SUCCESS) {
    failf(तथ्य, "LDAP local: ldap_search_ext %s", ldap_err2string(rc));
    परिणाम = CURLE_LDAP_खोज_FAILED;
    जाओ out;
  }

  lr = curlx_calloc(1, माप(काष्ठा ldapreqinfo));
  अगर(!lr ||
     Curl_meta_set(तथ्य, CURL_META_LDAP_EASY, lr, oldap_easy_dtor)) {
    ldap_abandon_ext(li->ld, msgid, शून्य, शून्य);
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  lr->msgid = msgid;
  Curl_xfer_setup_recv(तथ्य, FIRSTSOCKET, -1);
  *done = TRUE;

out:
  वापस परिणाम;
}

अटल CURLcode oldap_done(काष्ठा Curl_easy *तथ्य, CURLcode res,
                           bool premature)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ldapreqinfo *lr = Curl_meta_get(तथ्य, CURL_META_LDAP_EASY);

  (व्योम)res;
  (व्योम)premature;

  अगर(lr) {
    /* if there was a search in progress, abandon it */
    अगर(lr->msgid) {
      काष्ठा ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      अगर(li && li->ld) {
        ldap_abandon_ext(li->ld, lr->msgid, शून्य, शून्य);
      }
      lr->msgid = 0;
    }
    Curl_meta_remove(तथ्य, CURL_META_LDAP_EASY);
  }

  वापस CURLE_OK;
}

अटल CURLcode client_write(काष्ठा Curl_easy *तथ्य,
                             स्थिर अक्षर *prefix, माप_प्रकार plen,
                             स्थिर अक्षर *मूल्य, माप_प्रकार len,
                             स्थिर अक्षर *suffix, माप_प्रकार slen)
{
  CURLcode परिणाम = CURLE_OK;

  अगर(prefix) {
    /* If we have a zero-length value and the prefix ends with a space
       separator, drop the latter. */
    अगर(!len && plen && prefix[plen - 1] == ' ')
      plen--;
    परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, prefix, plen);
  }
  अगर(!परिणाम && मूल्य) {
    परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, मूल्य, len);
  }
  अगर(!परिणाम && suffix) {
    परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, suffix, slen);
  }
  वापस परिणाम;
}

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
अटल bool oldap_str_needs_base64(स्थिर अक्षर *s, माप_प्रकार len)
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

अटल CURLcode oldap_recv(काष्ठा Curl_easy *तथ्य, int8_t sockindex, अक्षर *buf,
                           माप_प्रकार len, माप_प्रकार *pnread)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  काष्ठा ldapreqinfo *lr = Curl_meta_get(तथ्य, CURL_META_LDAP_EASY);
  पूर्णांक rc;
  LDAPMessage *संदेश = शून्य;
  BerElement *ber = शून्य;
  काष्ठा timeval tv = { 0, 0 };
  काष्ठा berval bv, *bvals;
  CURLcode परिणाम = CURLE_AGAIN;
  पूर्णांक code;
  अक्षर *info = शून्य;

  (व्योम)len;
  (व्योम)buf;
  (व्योम)sockindex;
  *pnread = 0;
  अगर(!li || !lr)
    वापस CURLE_FAILED_INIT;

  rc = ldap_परिणाम(li->ld, lr->msgid, LDAP_संदेश_ONE, &tv, &संदेश);
  अगर(rc < 0) {
    failf(तथ्य, "LDAP local: search ldap_result %s", ldap_err2string(rc));
    परिणाम = CURLE_RECV_ERROR;
  }

  /* error or timed out */
  अगर(!संदेश)
    वापस परिणाम;

  परिणाम = CURLE_OK;

  चयन(ldap_msgtype(संदेश)) {
  हाल LDAP_RES_खोज_परिणाम:
    lr->msgid = 0;
    rc = ldap_parse_परिणाम(li->ld, संदेश, &code, शून्य, &info, शून्य, शून्य, 0);
    अगर(rc) {
      failf(तथ्य, "LDAP local: search ldap_parse_result %s",
            ldap_err2string(rc));
      परिणाम = CURLE_LDAP_खोज_FAILED;
      अवरोध;
    }

    /* store the latest code for later retrieval */
    तथ्य->info.httpcode = code;

    चयन(code) {
    हाल LDAP_SIZELIMIT_EXCEEDED:
      infof(तथ्य, "There are more than %d entries", lr->nument);
      FALLTHROUGH();
    हाल LDAP_SUCCESS:
      तथ्य->req.आकार = तथ्य->req.bytecount;
      अवरोध;
    शेष:
      failf(तथ्य, "LDAP remote: search failed %s %s", ldap_err2string(code),
            info ? info : "");
      परिणाम = CURLE_LDAP_खोज_FAILED;
      अवरोध;
    }
    अगर(info)
      ldap_memfree(info);
    अवरोध;
  हाल LDAP_RES_खोज_ENTRY:
    lr->nument++;
    rc = ldap_get_dn_ber(li->ld, संदेश, &ber, &bv);
    अगर(rc < 0) {
      परिणाम = CURLE_RECV_ERROR;
      अवरोध;
    }

    अगर(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
      अक्षर *dn_b64 = शून्य;
      माप_प्रकार dn_b64_sz = 0;

      /* A DN with e.g. CR/LF would inject LDIF lines, so base64-encode it. */
      परिणाम = curlx_base64_encode((uint8_t *)bv.bv_val, bv.bv_len,
                                   &dn_b64, &dn_b64_sz);
      अगर(!परिणाम)
        परिणाम = client_write(तथ्य, STRCONST("DN:: "), dn_b64, dn_b64_sz,
                              STRCONST("\n"));
      curlx_free(dn_b64);
    }
    अन्यथा
      परिणाम = client_write(तथ्य, STRCONST("DN: "), bv.bv_val, bv.bv_len,
                            STRCONST("\n"));
    अगर(परिणाम)
      अवरोध;

    क्रम(rc = ldap_get_attribute_ber(li->ld, संदेश, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, संदेश, ber, &bv, &bvals)) {
      पूर्णांक i;
      bool द्विआधारी;

      अगर(!bv.bv_val)
        अवरोध;

      /* An attribute type has no base64 form in LDIF, so a type carrying
         control bytes (CR/LF etc.) would let a hostile server inject lines.
         A conformant type name never contains them, so reject it. */
      अगर(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
        failf(तथ्य, "LDAP local: attribute type contains illegal bytes");
        अगर(bvals)
          ber_memfree(bvals);
        परिणाम = CURLE_WEIRD_SERVER_REPLY;
        अवरोध;
      }

      अगर(!bvals) {
        परिणाम = client_write(तथ्य, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":\n"));
        अगर(परिणाम)
          अवरोध;
        जारी;
      }

      द्विआधारी = bv.bv_len > 7 &&
        curl_strnequal(bv.bv_val + bv.bv_len - 7, ";binary", 7);

      क्रम(i = 0; bvals[i].bv_val; i++) {
        bool binval = FALSE;

        परिणाम = client_write(तथ्य, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":"));
        अगर(परिणाम)
          अवरोध;

        अगर(!द्विआधारी)
          binval = oldap_str_needs_base64(bvals[i].bv_val, bvals[i].bv_len);
        अगर(द्विआधारी || binval) {
          अक्षर *val_b64 = शून्य;
          माप_प्रकार val_b64_sz = 0;

          /* Binary value, encode to base64. */
          अगर(bvals[i].bv_len)
            परिणाम = curlx_base64_encode((uint8_t *)bvals[i].bv_val,
                                         bvals[i].bv_len,
                                         &val_b64, &val_b64_sz);
          अगर(!परिणाम)
            परिणाम = client_write(तथ्य, STRCONST(": "), val_b64, val_b64_sz,
                                  STRCONST("\n"));
          curlx_free(val_b64);
        }
        अन्यथा
          परिणाम = client_write(तथ्य, STRCONST(" "),
                                bvals[i].bv_val, bvals[i].bv_len,
                                STRCONST("\n"));
        अगर(परिणाम)
          अवरोध;
      }

      ber_memfree(bvals);
      bvals = शून्य;
      अगर(!परिणाम)
        परिणाम = client_write(तथ्य, STRCONST("\n"), शून्य, 0, शून्य, 0);
      अगर(परिणाम)
        अवरोध;
    }

    अगर(!परिणाम)
      परिणाम = client_write(तथ्य, STRCONST("\n"), शून्य, 0, शून्य, 0);
    अगर(!परिणाम)
      परिणाम = CURLE_AGAIN;
    अवरोध;
  }

  ber_free(ber, 0);
  ldap_msgfree(संदेश);
  वापस परिणाम;
}

व्योम Curl_ldap_version(अक्षर *buf, माप_प्रकार bufsz)
{
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  अगर(ldap_get_option(शून्य, LDAP_OPT_API_INFO, &api) == LDAP_OPT_SUCCESS) {
    अचिन्हित पूर्णांक patch = (अचिन्हित पूर्णांक)(api.ldapai_vendor_version % 100);
    अचिन्हित पूर्णांक major = (अचिन्हित पूर्णांक)(api.ldapai_vendor_version / 10000);
    अचिन्हित पूर्णांक minor =
      (((अचिन्हित पूर्णांक)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u",
                   api.ldapai_vendor_name, major, minor, patch);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((व्योम **)api.ldapai_extensions);
  }
  अन्यथा
    curl_msnprintf(buf, bufsz, "OpenLDAP");
}

/*
 * LDAP protocol handler.
 */
स्थिर काष्ठा Curl_protocol Curl_protocol_ldap = {
  oldap_setup_connection,               /* setup_connection */
  oldap_do,                             /* do_it */
  oldap_done,                           /* done */
  ZERO_NULL,                            /* do_more */
  oldap_connect,                        /* connect_it */
  oldap_connecting,                     /* connecting */
  ZERO_NULL,                            /* doing */
  ZERO_NULL,                            /* proto_pollset */
  ZERO_NULL,                            /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  ZERO_NULL,                            /* perform_pollset */
  oldap_disconnect,                     /* disconnect */
  ZERO_NULL,                            /* write_resp */
  ZERO_NULL,                            /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ZERO_NULL,                            /* attach connection */
  ZERO_NULL,                            /* follow */
};

#पूर्ण_अगर /* !CURL_DISABLE_LDAP && USE_OPENLDAP */

/* The LDAP scheme structs are in ldap.c */
