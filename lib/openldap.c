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
#अन्तर्भाव "curl_setup.h"

#जर !defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP)

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

#अन्तर्भाव <ldap.h>

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "curl_ldap.h"
#अन्तर्भाव "curlx/base64.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "curl_sasl.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "bufref.h"

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
प्रकारव्याख्या गणना {
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

#ifndef _LDAP_PVT_H
बाह्य पूर्णांक ldap_pvt_url_scheme2proto(स्थिरांक अक्षर *);
बाह्य पूर्णांक ldap_init_fd(ber_socket_t fd, पूर्णांक proto, स्थिरांक अक्षर *url,
                        LDAP **ld);
#endif

स्थिर Curl_recv oldap_recv;

संरचना ldapconninfo {
  संरचना SASL sasl;          /* SASL-related parameters */
  LDAP *ld;                  /* Openldap connection handle. */
  Curl_recv *recv;           /* For stacking SSL handler */
  Curl_send *send;
  संरचना berval *servercred; /* SASL data from server. */
  ldapstate state;           /* Current machine state. */
  curl_socket_t dupfd;       /* duplicated socket */
  पूर्णांक proto;                 /* LDAP_PROTO_TCP/LDAP_PROTO_UDP/LDAP_PROTO_IPC */
  पूर्णांक msgid;                 /* Current message id. */
};

संरचना ldapreqinfo {
  पूर्णांक msgid;
  पूर्णांक nument;
};

/* meta key for storing ldapconninfo at easy handle */
#define CURL_META_LDAP_EASY   "meta:proto:ldap:easy"
/* meta key for storing ldapconninfo at connection */
#define CURL_META_LDAP_CONN   "meta:proto:ldap:conn"

/*
 * oldap_state()
 *
 * This is the ONLY way to change LDAP state!
 */
स्थिर शून्य oldap_state(संरचना Curl_easy *data, संरचना ldapconninfo *li,
                        ldapstate newstate)
{
#जर defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  स्थिर स्थिरांक अक्षर * स्थिरांक names[] = {
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

  जर(li->state != newstate)
    infof(data, "LDAP %p state change from %s to %s",
          (शून्य *)li, names[li->state], names[newstate]);
#अन्यथा
  (शून्य)data;
#endif
  li->state = newstate;
}

/* Map some particular LDAP error codes to CURLcode values. */
स्थिर CURLcode oldap_map_error(पूर्णांक rc, CURLcode result)
{
  निवड(rc) {
  स्थिती LDAP_NO_MEMORY:
    परत CURLE_OUT_OF_MEMORY;
  स्थिती LDAP_INVALID_CREDENTIALS:
    परत CURLE_LOGIN_DENIED;
  स्थिती LDAP_PROTOCOL_ERROR:
    परत CURLE_UNSUPPORTED_PROTOCOL;
  स्थिती LDAP_INSUFFICIENT_ACCESS:
    परत CURLE_REMOTE_ACCESS_DENIED;
  }
  परत result;
}

स्थिर CURLcode oldap_url_parse(संरचना Curl_easy *data, LDAPURLDesc **ludp)
{
  CURLcode result = CURLE_OK;
  पूर्णांक rc;
  स्थिर स्थिरांक अक्षर * स्थिरांक url_errs[] = {
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

  *ludp = रिक्त;
  /* `ldap_url_parse() seems to be terrible with urls
   * that have user/pass/options in it. So when we have options or
   * creds from the url, fail without calling the function.
   * Yes, this is super-weird code and I do not like it. */
  जर((data->state.creds && (data->state.creds->source == CREDS_URL)) ||
     data->state.up.options)
    rc = LDAP_URL_ERR_BADURL;
  अन्यथा
    rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), ludp);

  जर(rc != LDAP_URL_SUCCESS) {
    स्थिरांक अक्षर *संदेश = "url parsing problem";

    result = rc == LDAP_URL_ERR_MEM ? CURLE_OUT_OF_MEMORY :
      CURLE_URL_MALFORMAT;
    rc -= LDAP_URL_SUCCESS;
    जर((माप_प्रकार)rc < CURL_ARRAYSIZE(url_errs))
      संदेश = url_errs[rc];
    failf(data, "LDAP local: %s", संदेश);
  }
  परत result;
}

/* Parse the login options. */
स्थिर CURLcode oldap_parse_login_options(संरचना connectdata *conn)
{
  CURLcode result = CURLE_OK;
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  स्थिरांक अक्षर *ptr = conn->options;

  DEBUGASSERT(li);
  जर(!li)
    परत CURLE_FAILED_INIT;

  पर्यंत(!result && ptr && *ptr) {
    स्थिरांक अक्षर *किल्ली = ptr;
    स्थिरांक अक्षर *मूल्य;

    पर्यंत(*ptr && *ptr != '=')
      ptr++;

    मूल्य = ptr + 1;

    पर्यंत(*ptr && *ptr != ';')
      ptr++;

    जर(checkprefix("AUTH=", किल्ली))
      result = Curl_sasl_parse_url_auth_option(&li->sasl, मूल्य, ptr - मूल्य);
    अन्यथा
      result = CURLE_SETOPT_OPTION_SYNTAX;

    जर(*ptr == ';')
      ptr++;
  }

  परत result == CURLE_URL_MALFORMAT ? CURLE_SETOPT_OPTION_SYNTAX : result;
}

स्थिर CURLcode oldap_setup_connection(संरचना Curl_easy *data,
                                       संरचना connectdata *conn)
{
  CURLcode result;
  LDAPURLDesc *lud;
  (शून्य)conn;

  /* Early URL syntax check. */
  result = oldap_url_parse(data, &lud);
  ldap_free_urldesc(lud);

  परत result;
}

/*
 * Get the SASL authentication challenge from the server credential buffer.
 */
स्थिर CURLcode oldap_get_message(संरचना Curl_easy *data, संरचना bufref *out)
{
  संरचना ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  संरचना berval *servercred = li ? li->servercred : रिक्त;
  DEBUGASSERT(li);
  जर(!li)
    परत CURLE_FAILED_INIT;

  जर(!servercred || !servercred->bv_val)
    परत CURLE_WEIRD_SERVER_REPLY;
  Curl_bufref_set(out, servercred->bv_val, servercred->bv_len, रिक्त);
  परत CURLE_OK;
}

/*
 * Sends an initial SASL bind request to the server.
 */
स्थिर CURLcode oldap_perform_auth(संरचना Curl_easy *data, स्थिरांक अक्षर *mech,
                                   स्थिरांक संरचना bufref *initresp)
{
  संरचना connectdata *conn = data->conn;
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  संरचना berval cred;
  संरचना berval *pcred = &cred;
  पूर्णांक rc;

  DEBUGASSERT(li);
  जर(!li)
    परत CURLE_FAILED_INIT;
  cred.bv_val = (अक्षर *)CURL_UNCONST(Curl_bufref_ptr(initresp));
  cred.bv_len = Curl_bufref_len(initresp);
  जर(!cred.bv_val)
    pcred = रिक्त;
  rc = ldap_sasl_bind(li->ld, रिक्त, mech, pcred, रिक्त, रिक्त, &li->msgid);
  जर(rc != LDAP_SUCCESS)
    परत oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  परत CURLE_OK;
}

/*
 * Sends SASL continuation.
 */
स्थिर CURLcode oldap_continue_auth(संरचना Curl_easy *data, स्थिरांक अक्षर *mech,
                                    स्थिरांक संरचना bufref *resp)
{
  संरचना connectdata *conn = data->conn;
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  संरचना berval cred;
  संरचना berval *pcred = &cred;
  पूर्णांक rc;

  जर(!li)
    परत CURLE_FAILED_INIT;
  cred.bv_val = (अक्षर *)CURL_UNCONST(Curl_bufref_ptr(resp));
  cred.bv_len = Curl_bufref_len(resp);
  जर(!cred.bv_val)
    pcred = रिक्त;
  rc = ldap_sasl_bind(li->ld, रिक्त, mech, pcred, रिक्त, रिक्त, &li->msgid);
  जर(rc != LDAP_SUCCESS)
    परत oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  परत CURLE_OK;
}

/*
 * Sends SASL bind cancellation.
 */
स्थिर CURLcode oldap_cancel_auth(संरचना Curl_easy *data, स्थिरांक अक्षर *mech)
{
  संरचना ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  पूर्णांक rc;

  (शून्य)mech;
  जर(!li)
    परत CURLE_FAILED_INIT;
  rc = ldap_sasl_bind(li->ld, रिक्त, LDAP_SASL_NULL, रिक्त, रिक्त, रिक्त,
                      &li->msgid);
  जर(rc != LDAP_SUCCESS)
    परत oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  परत CURLE_OK;
}

/* Starts LDAP simple bind. */
स्थिर CURLcode oldap_perform_bind(संरचना Curl_easy *data, ldapstate newstate)
{
  संरचना connectdata *conn = data->conn;
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  स्थिरांक अक्षर *binddn = रिक्त;
  संरचना berval passwd;
  पूर्णांक rc;

  जर(!li)
    परत CURLE_FAILED_INIT;
  passwd.bv_val = रिक्त;
  passwd.bv_len = 0;

  जर(conn->creds) {
    binddn = Curl_creds_user(conn->creds);
    passwd.bv_val = CURL_UNCONST(Curl_creds_passwd(conn->creds));
    passwd.bv_len = म_माप(passwd.bv_val);
  }

  rc = ldap_sasl_bind(li->ld, binddn, LDAP_SASL_SIMPLE, &passwd,
                      रिक्त, रिक्त, &li->msgid);
  जर(rc != LDAP_SUCCESS)
    परत oldap_map_error(rc,
                           data->state.creds ?
                           CURLE_LOGIN_DENIED : CURLE_LDAP_CANNOT_BIND);
  oldap_state(data, li, newstate);
  परत CURLE_OK;
}

/* Query the supported SASL authentication mechanisms. */
स्थिर CURLcode oldap_perform_mechs(संरचना Curl_easy *data)
{
  संरचना ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  पूर्णांक rc;
  स्थिर स्थिरांक अक्षर * स्थिरांक supportedSASLMechanisms[] = {
    "supportedSASLMechanisms",
    रिक्त
  };

  जर(!li)
    परत CURLE_FAILED_INIT;
  /* Casting away the const for the 3rd parameter that the LDAP API expects as
     a non-const char ** is potentially unsafe but we believe the lack of
     const in the API was an oversight and that no LDAP implementation
     actually modifies the input. */
  rc = ldap_शोध_ext(li->ld, "", LDAP_SCOPE_BASE, "(objectclass=*)",
                       (अक्षर **)CURL_UNCONST(supportedSASLMechanisms), 0,
                       रिक्त, रिक्त, रिक्त, 0, &li->msgid);
  जर(rc != LDAP_SUCCESS)
    परत oldap_map_error(rc, CURLE_LOGIN_DENIED);
  oldap_state(data, li, OLDAP_MECHS);
  परत CURLE_OK;
}

/* Starts SASL bind. */
स्थिर CURLcode oldap_perform_sasl(संरचना Curl_easy *data)
{
  संरचना ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  saslprogress progress = SASL_IDLE;
  CURLcode result;

  जर(!li)
    परत CURLE_FAILED_INIT;
  result = Curl_sasl_start(&li->sasl, data, TRUE, &progress);

  oldap_state(data, li, OLDAP_SASL);
  जर(!result && progress != SASL_INPROGRESS)
    result = Curl_sasl_is_blocked(&li->sasl, data);
  परत result;
}

#ifdef USE_SSL
स्थिर पूर्णांक ldapsb_tls_setup(Sockbuf_IO_Desc *sbiod, शून्य *arg)
{
  sbiod->sbiod_pvt = arg;
  परत 0;
}

स्थिर पूर्णांक ldapsb_tls_remove(Sockbuf_IO_Desc *sbiod)
{
  sbiod->sbiod_pvt = रिक्त;
  परत 0;
}

/* if there is a duplicated socket to close, close it */
स्थिर पूर्णांक ldapsb_tls_close(Sockbuf_IO_Desc *sbiod)
{
  संरचना Curl_easy *data = sbiod->sbiod_pvt;
  जर(data) {
    संरचना connectdata *conn = data->conn;
    जर(conn) {
      संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      जर(li && (li->dupfd != CURL_SOCKET_BAD)) {
        sclose(li->dupfd);
        li->dupfd = CURL_SOCKET_BAD;
      }
    }
  }

  परत 0;
}

स्थिर पूर्णांक ldapsb_tls_ctrl(Sockbuf_IO_Desc *sbiod, पूर्णांक opt, शून्य *arg)
{
  (शून्य)arg;
  जर(opt == LBER_SB_OPT_DATA_READY) {
    संरचना Curl_easy *data = sbiod->sbiod_pvt;
    परत Curl_conn_data_pending(data, FIRSTSOCKET);
  }
  परत 0;
}

स्थिर ber_slen_t ldapsb_tls_read(Sockbuf_IO_Desc *sbiod, शून्य *buf,
                                  ber_len_t len)
{
  संरचना Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  जर(data) {
    संरचना connectdata *conn = data->conn;
    जर(conn) {
      संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_RECV_ERROR;
      माप_प्रकार nread;

      जर(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        परत -1;
      }
      result = (li->recv)(data, FIRSTSOCKET, buf, len, &nread);
      जर(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nread;
    }
  }
  परत ret;
}

स्थिर ber_slen_t ldapsb_tls_write(Sockbuf_IO_Desc *sbiod, शून्य *buf,
                                   ber_len_t len)
{
  संरचना Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  जर(data) {
    संरचना connectdata *conn = data->conn;
    जर(conn) {
      संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_SEND_ERROR;
      माप_प्रकार nwritten;

      जर(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        परत -1;
      }
      result = (li->send)(data, FIRSTSOCKET, buf, len, FALSE, &nwritten);
      जर(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nwritten;
    }
  }
  परत ret;
}

स्थिर Sockbuf_IO ldapsb_tls = {
  ldapsb_tls_setup,
  ldapsb_tls_remove,
  ldapsb_tls_ctrl,
  ldapsb_tls_read,
  ldapsb_tls_write,
  ldapsb_tls_close
};

स्थिर bool ssl_installed(संरचना connectdata *conn)
{
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  परत li && li->recv;
}

स्थिर CURLcode oldap_ssl_connect(संरचना Curl_easy *data, ldapstate newstate)
{
  संरचना connectdata *conn = data->conn;
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  bool ssldone = FALSE;
  CURLcode result;

  जर(!li)
    परत CURLE_FAILED_INIT;
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  जर(result)
    परत result;
  oldap_state(data, li, newstate);

  जर(ssldone) {
    Sockbuf *sb;

    /* Install the libcurl SSL handlers into the sockbuf. */
    जर((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
      परत CURLE_FAILED_INIT;
    li->recv = conn->recv[FIRSTSOCKET];
    li->send = conn->send[FIRSTSOCKET];
  }

  परत result;
}

/* Send the STARTTLS request */
स्थिर CURLcode oldap_perform_starttls(संरचना Curl_easy *data)
{
  संरचना ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  पूर्णांक rc;

  जर(!li)
    परत CURLE_FAILED_INIT;
  rc = ldap_start_tls(li->ld, रिक्त, रिक्त, &li->msgid);
  जर(rc != LDAP_SUCCESS)
    परत oldap_map_error(rc, CURLE_USE_SSL_FAILED);
  oldap_state(data, li, OLDAP_STARTTLS);
  परत CURLE_OK;
}
#endif

स्थिर शून्य oldap_easy_dtor(स्थिरांक शून्य *किल्ली, माप_प्रकार klen, शून्य *entry)
{
  संरचना ldapreqinfo *lr = entry;
  (शून्य)किल्ली;
  (शून्य)klen;
  curlx_free(lr);
}

स्थिर शून्य oldap_conn_dtor(स्थिरांक शून्य *किल्ली, माप_प्रकार klen, शून्य *entry)
{
  संरचना ldapconninfo *li = entry;
  (शून्य)किल्ली;
  (शून्य)klen;
  जर(li->ld) {
    ldap_unbind_ext(li->ld, रिक्त, रिक्त);
    li->ld = रिक्त;
  }
  curlx_free(li);
}

/* SASL parameters for the ldap protocol */
स्थिर स्थिरांक संरचना SASLproto saslldap = {
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

स्थिर CURLcode oldap_connect(संरचना Curl_easy *data, bool *done)
{
  संरचना connectdata *conn = data->conn;
  संरचना ldapconninfo *li;
  स्थिर स्थिरांक पूर्णांक version = LDAP_VERSION3;
  अक्षर *hosturl = रिक्त;
  CURLcode result;
  पूर्णांक rc;
#ifdef CURL_OPENLDAP_DEBUG
  स्थिर पूर्णांक do_trace = -1;
#endif

  (शून्य)done;

  li = curlx_calloc(1, आकार(संरचना ldapconninfo));
  जर(!li) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }
  li->dupfd = CURL_SOCKET_BAD;

  result = Curl_conn_meta_set(conn, CURL_META_LDAP_CONN, li, oldap_conn_dtor);
  जर(result)
    जा out;

  li->proto = ldap_pvt_url_scheme2proto(data->state.origin->scheme->name);

  /* Initialize the SASL storage */
  Curl_sasl_init(&li->sasl, data, &saslldap);

  result = oldap_parse_login_options(conn);
  जर(result)
    जा out;

  hosturl = curl_maprintf("%s://%s:%u",
                          conn->scheme->name,
                          conn->origin->ipv6 ?
                          conn->origin->user_hostname :
                          conn->origin->hostname,
                          conn->origin->port);
  जर(!hosturl) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }

  {
#ifdef _WIN32
    SOCKET dupfd = INVALID_SOCKET;
    WSAPROTOCOL_INFO pi;
    जर(WSADuplicateSocket(conn->sock[FIRSTSOCKET], GetCurrentProcessId(),
                          &pi))
      ; /* error */
    अन्यथा
      dupfd = CURL_SOCKET(pi.iAddressFamily, pi.iSocketType, pi.iProtocol);
#अन्यथा
#ifdef F_DUPFD_CLOEXEC
    पूर्णांक dupfd = fcntl(conn->sock[FIRSTSOCKET], F_DUPFD_CLOEXEC, 0);
#अन्यथा
    पूर्णांक dupfd = dup(conn->sock[FIRSTSOCKET]);
#ifdef HAVE_FCNTL
    जर((dupfd != CURL_SOCKET_BAD) &&
       (fcntl(dupfd, F_SETFD, FD_CLOEXEC) < 0)) {
      sclose(dupfd);
      dupfd = CURL_SOCKET_BAD;
    }
#endif /* HAVE_FCNTL */
#endif /* F_DUPFD_CLOEXEC */
#endif /* _WIN32 */

    जर(dupfd == CURL_SOCKET_BAD) {
      result = CURLE_COULDNT_CONNECT;
      जा out;
    }
    rc = ldap_init_fd((ber_socket_t)dupfd, li->proto, hosturl, &li->ld);
    जर(rc)
      sclose(dupfd);
    अन्यथा
      /* store the socket, as when this is used by TLS we close this ourselves
         and at this point the code does not yet know if this will use TLS or
         not */
      li->dupfd = dupfd;
  }
  जर(rc) {
    failf(data, "LDAP local: Cannot connect to %s, %s",
          hosturl, ldap_err2string(rc));
    result = CURLE_COULDNT_CONNECT;
    जा out;
  }

#ifdef CURL_OPENLDAP_DEBUG
  जर(do_trace < 0) {
    स्थिरांक अक्षर *env = दो_पर्या("CURL_OPENLDAP_TRACE");
    curl_off_t e = 0;
    जर(!curlx_str_संख्या(&env, &e, पूर्णांक_उच्च))
      do_trace = e > 0;
  }
  जर(do_trace)
    ldap_set_option(li->ld, LDAP_OPT_DEBUG_LEVEL, &do_trace);
#endif

  /* Try version 3 first. */
  ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);

  /* Do not chase referrals. */
  ldap_set_option(li->ld, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  {
    ber_len_t max = 256 * 1024;
    Sockbuf *sb;
    जर((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       /* Set the maximum allowed size of an incoming message, which to
          OpenLDAP means that it will malloc() memory up to this size. If not
          set, there is no limit and we instead risk a malloc() failure. */
       !ber_sockbuf_ctrl(sb, LBER_SB_OPT_SET_MAX_INCOMING, &max)) {
      result = CURLE_FAILED_INIT;
      जा out;
    }
  }

#ifdef USE_SSL
  जर(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = oldap_ssl_connect(data, OLDAP_SSL);
    जा out;
  }

  जर(data->set.use_ssl) {
    result = oldap_perform_starttls(data);
    जर(!result || data->set.use_ssl != CURLUSESSL_TRY)
      जा out;
  }
#endif

  जर(li->sasl.prefmech != SASL_AUTH_NONE) {
    result = oldap_perform_mechs(data);
    जा out;
  }

  /* Force bind even if anonymous bind is not needed in protocol version 3
     to detect missing version 3 support. */
  result = oldap_perform_bind(data, OLDAP_BIND);

out:
  curlx_free(hosturl);
  परत result;
}

/* Handle the supported SASL mechanisms query response */
स्थिर CURLcode oldap_state_mechs_resp(संरचना Curl_easy *data,
                                       LDAPMessage *संदेश, पूर्णांक code)
{
  संरचना connectdata *conn;
  संरचना ldapconninfo *li;
  पूर्णांक rc;
  BerElement *ber = रिक्त;
  CURLcode result = CURLE_OK;
  संरचना berval bv, *bvals;

  जर(!data)
    परत CURLE_FAILED_INIT;

  conn = data->conn;
  li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);

  जर(!li)
    परत CURLE_FAILED_INIT;
  निवड(ldap_msgtype(संदेश)) {
  स्थिती LDAP_RES_शोध_ENTRY:
    /* Got a list of supported SASL mechanisms. */
    जर(code != LDAP_SUCCESS && code != LDAP_NO_RESULTS_RETURNED)
      परत CURLE_LOGIN_DENIED;

    rc = ldap_get_dn_ber(li->ld, संदेश, &ber, &bv);
    जर(rc < 0)
      परत oldap_map_error(rc, CURLE_BAD_CONTENT_ENCODING);
    साठी(rc = ldap_get_attribute_ber(li->ld, संदेश, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, संदेश, ber, &bv, &bvals)) {
      पूर्णांक i;

      जर(!bv.bv_val)
        थांबा;

      जर(bvals) {
        साठी(i = 0; bvals[i].bv_val; i++) {
          माप_प्रकार llen;
          अचिन्हित लघु mech =
            Curl_sasl_decode_mech((स्थिरांक अक्षर *)bvals[i].bv_val,
                                  bvals[i].bv_len, &llen);
          जर(bvals[i].bv_len == llen)
            li->sasl.authmechs |= mech;
        }
        ber_memfree(bvals);
      }
    }
    ber_free(ber, 0);
    थांबा;

  स्थिती LDAP_RES_शोध_RESULT:
    निवड(code) {
    स्थिती LDAP_SIZELIMIT_EXCEEDED:
      infof(data, "Too many authentication mechanisms");
      FALLTHROUGH();
    स्थिती LDAP_SUCCESS:
    स्थिती LDAP_NO_RESULTS_RETURNED:
      जर(Curl_sasl_can_authenticate(&li->sasl, data))
        result = oldap_perform_sasl(data);
      अन्यथा
        result = CURLE_LOGIN_DENIED;
      थांबा;
    पूर्वनिर्धारित:
      result = oldap_map_error(code, CURLE_LOGIN_DENIED);
      थांबा;
    }
    थांबा;
  पूर्वनिर्धारित:
    थांबा;
  }
  परत result;
}

/* Handle a SASL bind response. */
स्थिर CURLcode oldap_state_sasl_resp(संरचना Curl_easy *data,
                                      LDAPMessage *संदेश, पूर्णांक code)
{
  संरचना connectdata *conn = data->conn;
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode result = CURLE_OK;
  saslprogress progress;
  पूर्णांक rc;

  जर(!li)
    परत CURLE_FAILED_INIT;
  li->servercred = रिक्त;
  rc = ldap_parse_sasl_bind_result(li->ld, संदेश, &li->servercred, 0);
  जर(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: sasl ldap_parse_result %s", ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LOGIN_DENIED);
  }
  अन्यथा {
    result = Curl_sasl_continue(&li->sasl, data, code, &progress);
    जर(!result) {
      निवड(progress) {
      स्थिती SASL_DONE:
        oldap_state(data, li, OLDAP_STOP);   /* Authenticated */
        थांबा;
      स्थिती SASL_IDLE:            /* No mechanism left after cancellation */
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
        थांबा;
      पूर्वनिर्धारित:
        थांबा;
      }
    }
  }

  जर(li->servercred)
    ber_bvfree(li->servercred);
  परत result;
}

/* Handle a simple bind response. */
स्थिर CURLcode oldap_state_bind_resp(संरचना Curl_easy *data, LDAPMessage *संदेश,
                                      पूर्णांक code)
{
  संरचना connectdata *conn = data->conn;
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode result = CURLE_OK;
  संरचना berval *bv = रिक्त;
  पूर्णांक rc;

  जर(!li)
    परत CURLE_FAILED_INIT;

  जर(code != LDAP_SUCCESS)
    परत oldap_map_error(code, CURLE_LDAP_CANNOT_BIND);

  rc = ldap_parse_sasl_bind_result(li->ld, संदेश, &bv, 0);
  जर(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: bind ldap_parse_sasl_bind_result %s",
          ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  }
  अन्यथा
    oldap_state(data, li, OLDAP_STOP);

  जर(bv)
    ber_bvfree(bv);
  परत result;
}

स्थिर CURLcode oldap_connecting(संरचना Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  LDAPMessage *संदेश = रिक्त;
  संरचना timeval tv = { 0, 0 };
  पूर्णांक code = LDAP_SUCCESS;
  पूर्णांक rc;

  जर(!li)
    परत CURLE_FAILED_INIT;

  जर(li->state != OLDAP_SSL && li->state != OLDAP_TLS) {
    /* Get response to last command. */
    rc = ldap_result(li->ld, li->msgid, LDAP_संदेश_ONE, &tv, &संदेश);
    निवड(rc) {
    स्थिती 0:                               /* Timed out. */
      परत CURLE_OK;
    स्थिती LDAP_RES_शोध_ENTRY:
    स्थिती LDAP_RES_शोध_REFERENCE:
      थांबा;
    पूर्वनिर्धारित:
      li->msgid = 0;                      /* Nothing to abandon upon error. */
      जर(rc < 0) {
        failf(data, "LDAP local: connecting ldap_result %s",
              ldap_err2string(rc));
        परत oldap_map_error(rc, CURLE_COULDNT_CONNECT);
      }
      थांबा;
    }

    /* Get error code from message. */
    rc = ldap_parse_result(li->ld, संदेश, &code, रिक्त, रिक्त, रिक्त, रिक्त, 0);
    जर(rc)
      code = rc;
    अन्यथा {
      /* store the latest code for later retrieval */
      data->info.httpcode = code;
    }

    /* If protocol version 3 is not supported, fallback to version 2. */
    जर(code == LDAP_PROTOCOL_ERROR && li->state != OLDAP_BINDV2 &&
#ifdef USE_SSL
       (ssl_installed(conn) || data->set.use_ssl <= CURLUSESSL_TRY) &&
#endif
       li->sasl.prefmech == SASL_AUTH_NONE) {
      स्थिर स्थिरांक पूर्णांक version = LDAP_VERSION2;

      ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);
      ldap_msgfree(संदेश);
      परत oldap_perform_bind(data, OLDAP_BINDV2);
    }
  }

  /* Handle response message according to current state. */
  निवड(li->state) {

#ifdef USE_SSL
  स्थिती OLDAP_SSL:
    result = oldap_ssl_connect(data, OLDAP_SSL);
    जर(!result && ssl_installed(conn)) {
      जर(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      अन्यथा
        result = oldap_perform_bind(data, OLDAP_BIND);
    }
    थांबा;
  स्थिती OLDAP_STARTTLS:
    जर(code != LDAP_SUCCESS) {
      जर(data->set.use_ssl != CURLUSESSL_TRY)
        result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
      अन्यथा जर(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      अन्यथा
        result = oldap_perform_bind(data, OLDAP_BIND);
      थांबा;
    }
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    जर(result)
      थांबा;
    FALLTHROUGH();
  स्थिती OLDAP_TLS:
    result = oldap_ssl_connect(data, OLDAP_TLS);
    जर(result)
      result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
    अन्यथा जर(ssl_installed(conn)) {
      जर(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      अन्यथा जर(data->state.creds)
        result = oldap_perform_bind(data, OLDAP_BIND);
      अन्यथा {
        /* Version 3 supported: no bind required */
        oldap_state(data, li, OLDAP_STOP);
        result = CURLE_OK;
      }
    }
    थांबा;
#endif

  स्थिती OLDAP_MECHS:
    result = oldap_state_mechs_resp(data, संदेश, code);
    थांबा;
  स्थिती OLDAP_SASL:
    result = oldap_state_sasl_resp(data, संदेश, code);
    थांबा;
  स्थिती OLDAP_BIND:
  स्थिती OLDAP_BINDV2:
    result = oldap_state_bind_resp(data, संदेश, code);
    थांबा;
  पूर्वनिर्धारित:
    /* internal error */
    result = CURLE_COULDNT_CONNECT;
    थांबा;
  }

  ldap_msgfree(संदेश);

  *done = li->state == OLDAP_STOP;
  जर(*done)
    conn->recv[FIRSTSOCKET] = oldap_recv;

  जर(result && li->msgid) {
    ldap_abandon_ext(li->ld, li->msgid, रिक्त, रिक्त);
    li->msgid = 0;
  }
  परत result;
}

स्थिर CURLcode oldap_disconnect(संरचना Curl_easy *data,
                                 संरचना connectdata *conn,
                                 bool dead_connection)
{
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  (शून्य)dead_connection;
#ifndef USE_SSL
  (शून्य)data;
#endif

  जर(li && li->ld) {
#ifdef USE_SSL
    जर(ssl_installed(conn)) {
      Sockbuf *sb;
      जर(ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS ||
         ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
        परत CURLE_FAILED_INIT;
    }
#endif
    ldap_unbind_ext(li->ld, रिक्त, रिक्त);
    li->ld = रिक्त;
  }
  परत CURLE_OK;
}

स्थिर CURLcode oldap_do(संरचना Curl_easy *data, bool *done)
{
  संरचना connectdata *conn = data->conn;
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  संरचना ldapreqinfo *lr;
  CURLcode result;
  पूर्णांक rc;
  LDAPURLDesc *lud;
  पूर्णांक msgid;

  जर(!li)
    परत CURLE_FAILED_INIT;

  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

  result = oldap_url_parse(data, &lud);
  जर(result)
    जा out;

#ifdef USE_SSL
  जर(ssl_installed(conn)) {
    Sockbuf *sb;
    /* re-install the libcurl SSL handlers into the sockbuf. */
    जर((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data)) {
      ldap_free_urldesc(lud);
      परत CURLE_FAILED_INIT;
    }
  }
#endif

  rc = ldap_शोध_ext(li->ld, lud->lud_dn, lud->lud_scope,
                       lud->lud_filter, lud->lud_attrs, 0,
                       रिक्त, रिक्त, रिक्त, 0, &msgid);
  ldap_free_urldesc(lud);
  जर(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: ldap_search_ext %s", ldap_err2string(rc));
    result = CURLE_LDAP_शोध_FAILED;
    जा out;
  }

  lr = curlx_calloc(1, आकार(संरचना ldapreqinfo));
  जर(!lr ||
     Curl_meta_set(data, CURL_META_LDAP_EASY, lr, oldap_easy_dtor)) {
    ldap_abandon_ext(li->ld, msgid, रिक्त, रिक्त);
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }

  lr->msgid = msgid;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  *done = TRUE;

out:
  परत result;
}

स्थिर CURLcode oldap_done(संरचना Curl_easy *data, CURLcode res,
                           bool premature)
{
  संरचना connectdata *conn = data->conn;
  संरचना ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);

  (शून्य)res;
  (शून्य)premature;

  जर(lr) {
    /* if there was a search in progress, abandon it */
    जर(lr->msgid) {
      संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      जर(li && li->ld) {
        ldap_abandon_ext(li->ld, lr->msgid, रिक्त, रिक्त);
      }
      lr->msgid = 0;
    }
    Curl_meta_remove(data, CURL_META_LDAP_EASY);
  }

  परत CURLE_OK;
}

स्थिर CURLcode client_write(संरचना Curl_easy *data,
                             स्थिरांक अक्षर *prefix, माप_प्रकार plen,
                             स्थिरांक अक्षर *मूल्य, माप_प्रकार len,
                             स्थिरांक अक्षर *suffix, माप_प्रकार slen)
{
  CURLcode result = CURLE_OK;

  जर(prefix) {
    /* If we have a zero-length value and the prefix ends with a space
       separator, drop the latter. */
    जर(!len && plen && prefix[plen - 1] == ' ')
      plen--;
    result = Curl_client_write(data, CLIENTWRITE_BODY, prefix, plen);
  }
  जर(!result && मूल्य) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, मूल्य, len);
  }
  जर(!result && suffix) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, suffix, slen);
  }
  परत result;
}

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
स्थिर bool oldap_str_needs_base64(स्थिरांक अक्षर *s, माप_प्रकार len)
{
  माप_प्रकार i;
  जर(!len)
    परत FALSE;
  जर((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    परत TRUE;
  साठी(i = 0; i < len; i++)
    जर(!ISPRINT(s[i]))
      परत TRUE;
  परत FALSE;
}

स्थिर CURLcode oldap_recv(संरचना Curl_easy *data, int8_t sockindex, अक्षर *buf,
                           माप_प्रकार len, माप_प्रकार *pnread)
{
  संरचना connectdata *conn = data->conn;
  संरचना ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  संरचना ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);
  पूर्णांक rc;
  LDAPMessage *संदेश = रिक्त;
  BerElement *ber = रिक्त;
  संरचना timeval tv = { 0, 0 };
  संरचना berval bv, *bvals;
  CURLcode result = CURLE_AGAIN;
  पूर्णांक code;
  अक्षर *info = रिक्त;

  (शून्य)len;
  (शून्य)buf;
  (शून्य)sockindex;
  *pnread = 0;
  जर(!li || !lr)
    परत CURLE_FAILED_INIT;

  rc = ldap_result(li->ld, lr->msgid, LDAP_संदेश_ONE, &tv, &संदेश);
  जर(rc < 0) {
    failf(data, "LDAP local: search ldap_result %s", ldap_err2string(rc));
    result = CURLE_RECV_ERROR;
  }

  /* error or timed out */
  जर(!संदेश)
    परत result;

  result = CURLE_OK;

  निवड(ldap_msgtype(संदेश)) {
  स्थिती LDAP_RES_शोध_RESULT:
    lr->msgid = 0;
    rc = ldap_parse_result(li->ld, संदेश, &code, रिक्त, &info, रिक्त, रिक्त, 0);
    जर(rc) {
      failf(data, "LDAP local: search ldap_parse_result %s",
            ldap_err2string(rc));
      result = CURLE_LDAP_शोध_FAILED;
      थांबा;
    }

    /* store the latest code for later retrieval */
    data->info.httpcode = code;

    निवड(code) {
    स्थिती LDAP_SIZELIMIT_EXCEEDED:
      infof(data, "There are more than %d entries", lr->nument);
      FALLTHROUGH();
    स्थिती LDAP_SUCCESS:
      data->req.size = data->req.bytecount;
      थांबा;
    पूर्वनिर्धारित:
      failf(data, "LDAP remote: search failed %s %s", ldap_err2string(code),
            info ? info : "");
      result = CURLE_LDAP_शोध_FAILED;
      थांबा;
    }
    जर(info)
      ldap_memfree(info);
    थांबा;
  स्थिती LDAP_RES_शोध_ENTRY:
    lr->nument++;
    rc = ldap_get_dn_ber(li->ld, संदेश, &ber, &bv);
    जर(rc < 0) {
      result = CURLE_RECV_ERROR;
      थांबा;
    }

    जर(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
      अक्षर *dn_b64 = रिक्त;
      माप_प्रकार dn_b64_sz = 0;

      /* A DN with e.g. CR/LF would inject LDIF lines, so base64-encode it. */
      result = curlx_base64_encode((uint8_t *)bv.bv_val, bv.bv_len,
                                   &dn_b64, &dn_b64_sz);
      जर(!result)
        result = client_write(data, STRCONST("DN:: "), dn_b64, dn_b64_sz,
                              STRCONST("\n"));
      curlx_free(dn_b64);
    }
    अन्यथा
      result = client_write(data, STRCONST("DN: "), bv.bv_val, bv.bv_len,
                            STRCONST("\n"));
    जर(result)
      थांबा;

    साठी(rc = ldap_get_attribute_ber(li->ld, संदेश, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, संदेश, ber, &bv, &bvals)) {
      पूर्णांक i;
      bool द्विमान;

      जर(!bv.bv_val)
        थांबा;

      /* An attribute type has no base64 form in LDIF, so a type carrying
         control bytes (CR/LF etc.) would let a hostile server inject lines.
         A conformant type name never contains them, so reject it. */
      जर(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
        failf(data, "LDAP local: attribute type contains illegal bytes");
        जर(bvals)
          ber_memfree(bvals);
        result = CURLE_WEIRD_SERVER_REPLY;
        थांबा;
      }

      जर(!bvals) {
        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":\n"));
        जर(result)
          थांबा;
        सुरूठेवा;
      }

      द्विमान = bv.bv_len > 7 &&
        curl_strnequal(bv.bv_val + bv.bv_len - 7, ";binary", 7);

      साठी(i = 0; bvals[i].bv_val; i++) {
        bool binval = FALSE;

        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":"));
        जर(result)
          थांबा;

        जर(!द्विमान)
          binval = oldap_str_needs_base64(bvals[i].bv_val, bvals[i].bv_len);
        जर(द्विमान || binval) {
          अक्षर *val_b64 = रिक्त;
          माप_प्रकार val_b64_sz = 0;

          /* Binary value, encode to base64. */
          जर(bvals[i].bv_len)
            result = curlx_base64_encode((uint8_t *)bvals[i].bv_val,
                                         bvals[i].bv_len,
                                         &val_b64, &val_b64_sz);
          जर(!result)
            result = client_write(data, STRCONST(": "), val_b64, val_b64_sz,
                                  STRCONST("\n"));
          curlx_free(val_b64);
        }
        अन्यथा
          result = client_write(data, STRCONST(" "),
                                bvals[i].bv_val, bvals[i].bv_len,
                                STRCONST("\n"));
        जर(result)
          थांबा;
      }

      ber_memfree(bvals);
      bvals = रिक्त;
      जर(!result)
        result = client_write(data, STRCONST("\n"), रिक्त, 0, रिक्त, 0);
      जर(result)
        थांबा;
    }

    जर(!result)
      result = client_write(data, STRCONST("\n"), रिक्त, 0, रिक्त, 0);
    जर(!result)
      result = CURLE_AGAIN;
    थांबा;
  }

  ber_free(ber, 0);
  ldap_msgfree(संदेश);
  परत result;
}

शून्य Curl_ldap_version(अक्षर *buf, माप_प्रकार bufsz)
{
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  जर(ldap_get_option(रिक्त, LDAP_OPT_API_INFO, &api) == LDAP_OPT_SUCCESS) {
    अचिन्हित पूर्णांक patch = (अचिन्हित पूर्णांक)(api.ldapai_vendor_version % 100);
    अचिन्हित पूर्णांक major = (अचिन्हित पूर्णांक)(api.ldapai_vendor_version / 10000);
    अचिन्हित पूर्णांक minor =
      (((अचिन्हित पूर्णांक)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u",
                   api.ldapai_vendor_name, major, minor, patch);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((शून्य **)api.ldapai_extensions);
  }
  अन्यथा
    curl_msnprintf(buf, bufsz, "OpenLDAP");
}

/*
 * LDAP protocol handler.
 */
स्थिरांक संरचना Curl_protocol Curl_protocol_ldap = {
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

#endif /* !CURL_DISABLE_LDAP && USE_OPENLDAP */

/* The LDAP scheme structs are in ldap.c */
