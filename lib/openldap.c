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
#ضمن "curl_setup.h"

#𐤀𐤌 !defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP)

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

#ضمن <ldap.h>

#ضمن "urldata.h"
#ضمن "url.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "vtls/vtls.h"
#ضمن "transfer.h"
#ضمن "curl_ldap.h"
#ضمن "curlx/base64.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "curl_sasl.h"
#ضمن "strcase.h"
#ضمن "bufref.h"

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
تعريفنوع تعداد {
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
خارجي صحيح ldap_pvt_url_scheme2proto(ثابت حرف *);
خارجي صحيح ldap_init_fd(ber_socket_t fd, صحيح proto, ثابت حرف *url,
                        LDAP **ld);
#endif

ساكن Curl_recv oldap_recv;

بنية ldapconninfo {
  بنية SASL sasl;          /* SASL-related parameters */
  LDAP *ld;                  /* Openldap connection handle. */
  Curl_recv *recv;           /* For stacking SSL handler */
  Curl_send *send;
  بنية berval *servercred; /* SASL data from server. */
  ldapstate state;           /* Current machine state. */
  curl_socket_t dupfd;       /* duplicated socket */
  صحيح proto;                 /* LDAP_PROTO_TCP/LDAP_PROTO_UDP/LDAP_PROTO_IPC */
  صحيح msgid;                 /* Current message id. */
};

بنية ldapreqinfo {
  صحيح msgid;
  صحيح nument;
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
ساكن فراغ oldap_state(بنية Curl_easy *data, بنية ldapconninfo *li,
                        ldapstate newstate)
{
#𐤀𐤌 defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ساكن ثابت حرف * ثابت names[] = {
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

  𐤀𐤌(li->state != newstate)
    infof(data, "LDAP %p state change from %s to %s",
          (فراغ *)li, names[li->state], names[newstate]);
#وإلا
  (فراغ)data;
#endif
  li->state = newstate;
}

/* Map some particular LDAP error codes to CURLcode values. */
ساكن CURLcode oldap_map_error(صحيح rc, CURLcode result)
{
  بدل(rc) {
  حالة LDAP_NO_MEMORY:
    أرجع CURLE_OUT_OF_MEMORY;
  حالة LDAP_INVALID_CREDENTIALS:
    أرجع CURLE_LOGIN_DENIED;
  حالة LDAP_PROTOCOL_ERROR:
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  حالة LDAP_INSUFFICIENT_ACCESS:
    أرجع CURLE_REMOTE_ACCESS_DENIED;
  }
  أرجع result;
}

ساكن CURLcode oldap_url_parse(بنية Curl_easy *data, LDAPURLDesc **ludp)
{
  CURLcode result = CURLE_OK;
  صحيح rc;
  ساكن ثابت حرف * ثابت url_errs[] = {
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

  *ludp = عدم;
  /* `ldap_url_parse() seems to be terrible with urls
   * that have user/pass/options in it. So when we have options or
   * creds from the url, fail without calling the function.
   * Yes, this is super-weird code and I do not like it. */
  𐤀𐤌((data->state.creds && (data->state.creds->source == CREDS_URL)) ||
     data->state.up.options)
    rc = LDAP_URL_ERR_BADURL;
  وإلا
    rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), ludp);

  𐤀𐤌(rc != LDAP_URL_SUCCESS) {
    ثابت حرف *msg = "url parsing problem";

    result = rc == LDAP_URL_ERR_MEM ? CURLE_OUT_OF_MEMORY :
      CURLE_URL_MALFORMAT;
    rc -= LDAP_URL_SUCCESS;
    𐤀𐤌((size_t)rc < CURL_ARRAYSIZE(url_errs))
      msg = url_errs[rc];
    failf(data, "LDAP local: %s", msg);
  }
  أرجع result;
}

/* Parse the login options. */
ساكن CURLcode oldap_parse_login_options(بنية connectdata *conn)
{
  CURLcode result = CURLE_OK;
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ثابت حرف *ptr = conn->options;

  DEBUGASSERT(li);
  𐤀𐤌(!li)
    أرجع CURLE_FAILED_INIT;

  طالما(!result && ptr && *ptr) {
    ثابت حرف *key = ptr;
    ثابت حرف *value;

    طالما(*ptr && *ptr != '=')
      ptr++;

    value = ptr + 1;

    طالما(*ptr && *ptr != ';')
      ptr++;

    𐤀𐤌(checkprefix("AUTH=", key))
      result = Curl_sasl_parse_url_auth_option(&li->sasl, value, ptr - value);
    وإلا
      result = CURLE_SETOPT_OPTION_SYNTAX;

    𐤀𐤌(*ptr == ';')
      ptr++;
  }

  أرجع result == CURLE_URL_MALFORMAT ? CURLE_SETOPT_OPTION_SYNTAX : result;
}

ساكن CURLcode oldap_setup_connection(بنية Curl_easy *data,
                                       بنية connectdata *conn)
{
  CURLcode result;
  LDAPURLDesc *lud;
  (فراغ)conn;

  /* Early URL syntax check. */
  result = oldap_url_parse(data, &lud);
  ldap_free_urldesc(lud);

  أرجع result;
}

/*
 * Get the SASL authentication challenge from the server credential buffer.
 */
ساكن CURLcode oldap_get_message(بنية Curl_easy *data, بنية bufref *out)
{
  بنية ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  بنية berval *servercred = li ? li->servercred : عدم;
  DEBUGASSERT(li);
  𐤀𐤌(!li)
    أرجع CURLE_FAILED_INIT;

  𐤀𐤌(!servercred || !servercred->bv_val)
    أرجع CURLE_WEIRD_SERVER_REPLY;
  Curl_bufref_set(out, servercred->bv_val, servercred->bv_len, عدم);
  أرجع CURLE_OK;
}

/*
 * Sends an initial SASL bind request to the server.
 */
ساكن CURLcode oldap_perform_auth(بنية Curl_easy *data, ثابت حرف *mech,
                                   ثابت بنية bufref *initresp)
{
  بنية connectdata *conn = data->conn;
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  بنية berval cred;
  بنية berval *pcred = &cred;
  صحيح rc;

  DEBUGASSERT(li);
  𐤀𐤌(!li)
    أرجع CURLE_FAILED_INIT;
  cred.bv_val = (حرف *)CURL_UNCONST(Curl_bufref_ptr(initresp));
  cred.bv_len = Curl_bufref_len(initresp);
  𐤀𐤌(!cred.bv_val)
    pcred = عدم;
  rc = ldap_sasl_bind(li->ld, عدم, mech, pcred, عدم, عدم, &li->msgid);
  𐤀𐤌(rc != LDAP_SUCCESS)
    أرجع oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  أرجع CURLE_OK;
}

/*
 * Sends SASL continuation.
 */
ساكن CURLcode oldap_continue_auth(بنية Curl_easy *data, ثابت حرف *mech,
                                    ثابت بنية bufref *resp)
{
  بنية connectdata *conn = data->conn;
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  بنية berval cred;
  بنية berval *pcred = &cred;
  صحيح rc;

  𐤀𐤌(!li)
    أرجع CURLE_FAILED_INIT;
  cred.bv_val = (حرف *)CURL_UNCONST(Curl_bufref_ptr(resp));
  cred.bv_len = Curl_bufref_len(resp);
  𐤀𐤌(!cred.bv_val)
    pcred = عدم;
  rc = ldap_sasl_bind(li->ld, عدم, mech, pcred, عدم, عدم, &li->msgid);
  𐤀𐤌(rc != LDAP_SUCCESS)
    أرجع oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  أرجع CURLE_OK;
}

/*
 * Sends SASL bind cancellation.
 */
ساكن CURLcode oldap_cancel_auth(بنية Curl_easy *data, ثابت حرف *mech)
{
  بنية ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  صحيح rc;

  (فراغ)mech;
  𐤀𐤌(!li)
    أرجع CURLE_FAILED_INIT;
  rc = ldap_sasl_bind(li->ld, عدم, LDAP_SASL_NULL, عدم, عدم, عدم,
                      &li->msgid);
  𐤀𐤌(rc != LDAP_SUCCESS)
    أرجع oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  أرجع CURLE_OK;
}

/* Starts LDAP simple bind. */
ساكن CURLcode oldap_perform_bind(بنية Curl_easy *data, ldapstate newstate)
{
  بنية connectdata *conn = data->conn;
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ثابت حرف *binddn = عدم;
  بنية berval passwd;
  صحيح rc;

  𐤀𐤌(!li)
    أرجع CURLE_FAILED_INIT;
  passwd.bv_val = عدم;
  passwd.bv_len = 0;

  𐤀𐤌(conn->creds) {
    binddn = Curl_creds_user(conn->creds);
    passwd.bv_val = CURL_UNCONST(Curl_creds_passwd(conn->creds));
    passwd.bv_len = strlen(passwd.bv_val);
  }

  rc = ldap_sasl_bind(li->ld, binddn, LDAP_SASL_SIMPLE, &passwd,
                      عدم, عدم, &li->msgid);
  𐤀𐤌(rc != LDAP_SUCCESS)
    أرجع oldap_map_error(rc,
                           data->state.creds ?
                           CURLE_LOGIN_DENIED : CURLE_LDAP_CANNOT_BIND);
  oldap_state(data, li, newstate);
  أرجع CURLE_OK;
}

/* Query the supported SASL authentication mechanisms. */
ساكن CURLcode oldap_perform_mechs(بنية Curl_easy *data)
{
  بنية ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  صحيح rc;
  ساكن ثابت حرف * ثابت supportedSASLMechanisms[] = {
    "supportedSASLMechanisms",
    عدم
  };

  𐤀𐤌(!li)
    أرجع CURLE_FAILED_INIT;
  /* Casting away the const for the 3rd parameter that the LDAP API expects as
     a non-const char ** is potentially unsafe but we believe the lack of
     const in the API was an oversight and that no LDAP implementation
     actually modifies the input. */
  rc = ldap_search_ext(li->ld, "", LDAP_SCOPE_BASE, "(objectclass=*)",
                       (حرف **)CURL_UNCONST(supportedSASLMechanisms), 0,
                       عدم, عدم, عدم, 0, &li->msgid);
  𐤀𐤌(rc != LDAP_SUCCESS)
    أرجع oldap_map_error(rc, CURLE_LOGIN_DENIED);
  oldap_state(data, li, OLDAP_MECHS);
  أرجع CURLE_OK;
}

/* Starts SASL bind. */
ساكن CURLcode oldap_perform_sasl(بنية Curl_easy *data)
{
  بنية ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  saslprogress progress = SASL_IDLE;
  CURLcode result;

  𐤀𐤌(!li)
    أرجع CURLE_FAILED_INIT;
  result = Curl_sasl_start(&li->sasl, data, TRUE, &progress);

  oldap_state(data, li, OLDAP_SASL);
  𐤀𐤌(!result && progress != SASL_INPROGRESS)
    result = Curl_sasl_is_blocked(&li->sasl, data);
  أرجع result;
}

#ifdef USE_SSL
ساكن صحيح ldapsb_tls_setup(Sockbuf_IO_Desc *sbiod, فراغ *arg)
{
  sbiod->sbiod_pvt = arg;
  أرجع 0;
}

ساكن صحيح ldapsb_tls_remove(Sockbuf_IO_Desc *sbiod)
{
  sbiod->sbiod_pvt = عدم;
  أرجع 0;
}

/* if there is a duplicated socket to close, close it */
ساكن صحيح ldapsb_tls_close(Sockbuf_IO_Desc *sbiod)
{
  بنية Curl_easy *data = sbiod->sbiod_pvt;
  𐤀𐤌(data) {
    بنية connectdata *conn = data->conn;
    𐤀𐤌(conn) {
      بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      𐤀𐤌(li && (li->dupfd != CURL_SOCKET_BAD)) {
        sclose(li->dupfd);
        li->dupfd = CURL_SOCKET_BAD;
      }
    }
  }

  أرجع 0;
}

ساكن صحيح ldapsb_tls_ctrl(Sockbuf_IO_Desc *sbiod, صحيح opt, فراغ *arg)
{
  (فراغ)arg;
  𐤀𐤌(opt == LBER_SB_OPT_DATA_READY) {
    بنية Curl_easy *data = sbiod->sbiod_pvt;
    أرجع Curl_conn_data_pending(data, FIRSTSOCKET);
  }
  أرجع 0;
}

ساكن ber_slen_t ldapsb_tls_read(Sockbuf_IO_Desc *sbiod, فراغ *buf,
                                  ber_len_t len)
{
  بنية Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  𐤀𐤌(data) {
    بنية connectdata *conn = data->conn;
    𐤀𐤌(conn) {
      بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_RECV_ERROR;
      size_t nread;

      𐤀𐤌(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        أرجع -1;
      }
      result = (li->recv)(data, FIRSTSOCKET, buf, len, &nread);
      𐤀𐤌(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nread;
    }
  }
  أرجع ret;
}

ساكن ber_slen_t ldapsb_tls_write(Sockbuf_IO_Desc *sbiod, فراغ *buf,
                                   ber_len_t len)
{
  بنية Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  𐤀𐤌(data) {
    بنية connectdata *conn = data->conn;
    𐤀𐤌(conn) {
      بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_SEND_ERROR;
      size_t nwritten;

      𐤀𐤌(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        أرجع -1;
      }
      result = (li->send)(data, FIRSTSOCKET, buf, len, FALSE, &nwritten);
      𐤀𐤌(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nwritten;
    }
  }
  أرجع ret;
}

ساكن Sockbuf_IO ldapsb_tls = {
  ldapsb_tls_setup,
  ldapsb_tls_remove,
  ldapsb_tls_ctrl,
  ldapsb_tls_read,
  ldapsb_tls_write,
  ldapsb_tls_close
};

ساكن bool ssl_installed(بنية connectdata *conn)
{
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  أرجع li && li->recv;
}

ساكن CURLcode oldap_ssl_connect(بنية Curl_easy *data, ldapstate newstate)
{
  بنية connectdata *conn = data->conn;
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  bool ssldone = FALSE;
  CURLcode result;

  𐤀𐤌(!li)
    أرجع CURLE_FAILED_INIT;
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  𐤀𐤌(result)
    أرجع result;
  oldap_state(data, li, newstate);

  𐤀𐤌(ssldone) {
    Sockbuf *sb;

    /* Install the libcurl SSL handlers into the sockbuf. */
    𐤀𐤌((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
      أرجع CURLE_FAILED_INIT;
    li->recv = conn->recv[FIRSTSOCKET];
    li->send = conn->send[FIRSTSOCKET];
  }

  أرجع result;
}

/* Send the STARTTLS request */
ساكن CURLcode oldap_perform_starttls(بنية Curl_easy *data)
{
  بنية ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  صحيح rc;

  𐤀𐤌(!li)
    أرجع CURLE_FAILED_INIT;
  rc = ldap_start_tls(li->ld, عدم, عدم, &li->msgid);
  𐤀𐤌(rc != LDAP_SUCCESS)
    أرجع oldap_map_error(rc, CURLE_USE_SSL_FAILED);
  oldap_state(data, li, OLDAP_STARTTLS);
  أرجع CURLE_OK;
}
#endif

ساكن فراغ oldap_easy_dtor(ثابت فراغ *key, size_t klen, فراغ *entry)
{
  بنية ldapreqinfo *lr = entry;
  (فراغ)key;
  (فراغ)klen;
  curlx_free(lr);
}

ساكن فراغ oldap_conn_dtor(ثابت فراغ *key, size_t klen, فراغ *entry)
{
  بنية ldapconninfo *li = entry;
  (فراغ)key;
  (فراغ)klen;
  𐤀𐤌(li->ld) {
    ldap_unbind_ext(li->ld, عدم, عدم);
    li->ld = عدم;
  }
  curlx_free(li);
}

/* SASL parameters for the ldap protocol */
ساكن ثابت بنية SASLproto saslldap = {
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

ساكن CURLcode oldap_connect(بنية Curl_easy *data, bool *done)
{
  بنية connectdata *conn = data->conn;
  بنية ldapconninfo *li;
  ساكن ثابت صحيح version = LDAP_VERSION3;
  حرف *hosturl = عدم;
  CURLcode result;
  صحيح rc;
#ifdef CURL_OPENLDAP_DEBUG
  ساكن صحيح do_trace = -1;
#endif

  (فراغ)done;

  li = curlx_calloc(1, حجم(بنية ldapconninfo));
  𐤀𐤌(!li) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  li->dupfd = CURL_SOCKET_BAD;

  result = Curl_conn_meta_set(conn, CURL_META_LDAP_CONN, li, oldap_conn_dtor);
  𐤀𐤌(result)
    اذهب out;

  li->proto = ldap_pvt_url_scheme2proto(data->state.origin->scheme->name);

  /* Initialize the SASL storage */
  Curl_sasl_init(&li->sasl, data, &saslldap);

  result = oldap_parse_login_options(conn);
  𐤀𐤌(result)
    اذهب out;

  hosturl = curl_maprintf("%s://%s:%u",
                          conn->scheme->name,
                          conn->origin->ipv6 ?
                          conn->origin->user_hostname :
                          conn->origin->hostname,
                          conn->origin->port);
  𐤀𐤌(!hosturl) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  {
#ifdef _WIN32
    SOCKET dupfd = INVALID_SOCKET;
    WSAPROTOCOL_INFO pi;
    𐤀𐤌(WSADuplicateSocket(conn->sock[FIRSTSOCKET], GetCurrentProcessId(),
                          &pi))
      ; /* error */
    وإلا
      dupfd = CURL_SOCKET(pi.iAddressFamily, pi.iSocketType, pi.iProtocol);
#وإلا
#ifdef F_DUPFD_CLOEXEC
    صحيح dupfd = fcntl(conn->sock[FIRSTSOCKET], F_DUPFD_CLOEXEC, 0);
#وإلا
    صحيح dupfd = dup(conn->sock[FIRSTSOCKET]);
#ifdef HAVE_FCNTL
    𐤀𐤌((dupfd != CURL_SOCKET_BAD) &&
       (fcntl(dupfd, F_SETFD, FD_CLOEXEC) < 0)) {
      sclose(dupfd);
      dupfd = CURL_SOCKET_BAD;
    }
#endif /* HAVE_FCNTL */
#endif /* F_DUPFD_CLOEXEC */
#endif /* _WIN32 */

    𐤀𐤌(dupfd == CURL_SOCKET_BAD) {
      result = CURLE_COULDNT_CONNECT;
      اذهب out;
    }
    rc = ldap_init_fd((ber_socket_t)dupfd, li->proto, hosturl, &li->ld);
    𐤀𐤌(rc)
      sclose(dupfd);
    وإلا
      /* store the socket, as when this is used by TLS we close this ourselves
         and at this point the code does not yet know if this will use TLS or
         not */
      li->dupfd = dupfd;
  }
  𐤀𐤌(rc) {
    failf(data, "LDAP local: Cannot connect to %s, %s",
          hosturl, ldap_err2string(rc));
    result = CURLE_COULDNT_CONNECT;
    اذهب out;
  }

#ifdef CURL_OPENLDAP_DEBUG
  𐤀𐤌(do_trace < 0) {
    ثابت حرف *env = getenv("CURL_OPENLDAP_TRACE");
    curl_off_t e = 0;
    𐤀𐤌(!curlx_str_number(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  𐤀𐤌(do_trace)
    ldap_set_option(li->ld, LDAP_OPT_DEBUG_LEVEL, &do_trace);
#endif

  /* Try version 3 first. */
  ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);

  /* Do not chase referrals. */
  ldap_set_option(li->ld, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  {
    ber_len_t max = 256 * 1024;
    Sockbuf *sb;
    𐤀𐤌((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       /* Set the maximum allowed size of an incoming message, which to
          OpenLDAP means that it will malloc() memory up to this size. If not
          set, there is no limit and we instead risk a malloc() failure. */
       !ber_sockbuf_ctrl(sb, LBER_SB_OPT_SET_MAX_INCOMING, &max)) {
      result = CURLE_FAILED_INIT;
      اذهب out;
    }
  }

#ifdef USE_SSL
  𐤀𐤌(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = oldap_ssl_connect(data, OLDAP_SSL);
    اذهب out;
  }

  𐤀𐤌(data->set.use_ssl) {
    result = oldap_perform_starttls(data);
    𐤀𐤌(!result || data->set.use_ssl != CURLUSESSL_TRY)
      اذهب out;
  }
#endif

  𐤀𐤌(li->sasl.prefmech != SASL_AUTH_NONE) {
    result = oldap_perform_mechs(data);
    اذهب out;
  }

  /* Force bind even if anonymous bind is not needed in protocol version 3
     to detect missing version 3 support. */
  result = oldap_perform_bind(data, OLDAP_BIND);

out:
  curlx_free(hosturl);
  أرجع result;
}

/* Handle the supported SASL mechanisms query response */
ساكن CURLcode oldap_state_mechs_resp(بنية Curl_easy *data,
                                       LDAPMessage *msg, صحيح code)
{
  بنية connectdata *conn;
  بنية ldapconninfo *li;
  صحيح rc;
  BerElement *ber = عدم;
  CURLcode result = CURLE_OK;
  بنية berval bv, *bvals;

  𐤀𐤌(!data)
    أرجع CURLE_FAILED_INIT;

  conn = data->conn;
  li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);

  𐤀𐤌(!li)
    أرجع CURLE_FAILED_INIT;
  بدل(ldap_msgtype(msg)) {
  حالة LDAP_RES_SEARCH_ENTRY:
    /* Got a list of supported SASL mechanisms. */
    𐤀𐤌(code != LDAP_SUCCESS && code != LDAP_NO_RESULTS_RETURNED)
      أرجع CURLE_LOGIN_DENIED;

    rc = ldap_get_dn_ber(li->ld, msg, &ber, &bv);
    𐤀𐤌(rc < 0)
      أرجع oldap_map_error(rc, CURLE_BAD_CONTENT_ENCODING);
    لكل(rc = ldap_get_attribute_ber(li->ld, msg, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, msg, ber, &bv, &bvals)) {
      صحيح i;

      𐤀𐤌(!bv.bv_val)
        اكسر;

      𐤀𐤌(bvals) {
        لكل(i = 0; bvals[i].bv_val; i++) {
          size_t llen;
          unsigned short mech =
            Curl_sasl_decode_mech((ثابت حرف *)bvals[i].bv_val,
                                  bvals[i].bv_len, &llen);
          𐤀𐤌(bvals[i].bv_len == llen)
            li->sasl.authmechs |= mech;
        }
        ber_memfree(bvals);
      }
    }
    ber_free(ber, 0);
    اكسر;

  حالة LDAP_RES_SEARCH_RESULT:
    بدل(code) {
    حالة LDAP_SIZELIMIT_EXCEEDED:
      infof(data, "Too many authentication mechanisms");
      FALLTHROUGH();
    حالة LDAP_SUCCESS:
    حالة LDAP_NO_RESULTS_RETURNED:
      𐤀𐤌(Curl_sasl_can_authenticate(&li->sasl, data))
        result = oldap_perform_sasl(data);
      وإلا
        result = CURLE_LOGIN_DENIED;
      اكسر;
    افتراضي:
      result = oldap_map_error(code, CURLE_LOGIN_DENIED);
      اكسر;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع result;
}

/* Handle a SASL bind response. */
ساكن CURLcode oldap_state_sasl_resp(بنية Curl_easy *data,
                                      LDAPMessage *msg, صحيح code)
{
  بنية connectdata *conn = data->conn;
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode result = CURLE_OK;
  saslprogress progress;
  صحيح rc;

  𐤀𐤌(!li)
    أرجع CURLE_FAILED_INIT;
  li->servercred = عدم;
  rc = ldap_parse_sasl_bind_result(li->ld, msg, &li->servercred, 0);
  𐤀𐤌(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: sasl ldap_parse_result %s", ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LOGIN_DENIED);
  }
  وإلا {
    result = Curl_sasl_continue(&li->sasl, data, code, &progress);
    𐤀𐤌(!result) {
      بدل(progress) {
      حالة SASL_DONE:
        oldap_state(data, li, OLDAP_STOP);   /* Authenticated */
        اكسر;
      حالة SASL_IDLE:            /* No mechanism left after cancellation */
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
        اكسر;
      افتراضي:
        اكسر;
      }
    }
  }

  𐤀𐤌(li->servercred)
    ber_bvfree(li->servercred);
  أرجع result;
}

/* Handle a simple bind response. */
ساكن CURLcode oldap_state_bind_resp(بنية Curl_easy *data, LDAPMessage *msg,
                                      صحيح code)
{
  بنية connectdata *conn = data->conn;
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode result = CURLE_OK;
  بنية berval *bv = عدم;
  صحيح rc;

  𐤀𐤌(!li)
    أرجع CURLE_FAILED_INIT;

  𐤀𐤌(code != LDAP_SUCCESS)
    أرجع oldap_map_error(code, CURLE_LDAP_CANNOT_BIND);

  rc = ldap_parse_sasl_bind_result(li->ld, msg, &bv, 0);
  𐤀𐤌(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: bind ldap_parse_sasl_bind_result %s",
          ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  }
  وإلا
    oldap_state(data, li, OLDAP_STOP);

  𐤀𐤌(bv)
    ber_bvfree(bv);
  أرجع result;
}

ساكن CURLcode oldap_connecting(بنية Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  بنية connectdata *conn = data->conn;
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  LDAPMessage *msg = عدم;
  بنية timeval tv = { 0, 0 };
  صحيح code = LDAP_SUCCESS;
  صحيح rc;

  𐤀𐤌(!li)
    أرجع CURLE_FAILED_INIT;

  𐤀𐤌(li->state != OLDAP_SSL && li->state != OLDAP_TLS) {
    /* Get response to last command. */
    rc = ldap_result(li->ld, li->msgid, LDAP_MSG_ONE, &tv, &msg);
    بدل(rc) {
    حالة 0:                               /* Timed out. */
      أرجع CURLE_OK;
    حالة LDAP_RES_SEARCH_ENTRY:
    حالة LDAP_RES_SEARCH_REFERENCE:
      اكسر;
    افتراضي:
      li->msgid = 0;                      /* Nothing to abandon upon error. */
      𐤀𐤌(rc < 0) {
        failf(data, "LDAP local: connecting ldap_result %s",
              ldap_err2string(rc));
        أرجع oldap_map_error(rc, CURLE_COULDNT_CONNECT);
      }
      اكسر;
    }

    /* Get error code from message. */
    rc = ldap_parse_result(li->ld, msg, &code, عدم, عدم, عدم, عدم, 0);
    𐤀𐤌(rc)
      code = rc;
    وإلا {
      /* store the latest code for later retrieval */
      data->info.httpcode = code;
    }

    /* If protocol version 3 is not supported, fallback to version 2. */
    𐤀𐤌(code == LDAP_PROTOCOL_ERROR && li->state != OLDAP_BINDV2 &&
#ifdef USE_SSL
       (ssl_installed(conn) || data->set.use_ssl <= CURLUSESSL_TRY) &&
#endif
       li->sasl.prefmech == SASL_AUTH_NONE) {
      ساكن ثابت صحيح version = LDAP_VERSION2;

      ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);
      ldap_msgfree(msg);
      أرجع oldap_perform_bind(data, OLDAP_BINDV2);
    }
  }

  /* Handle response message according to current state. */
  بدل(li->state) {

#ifdef USE_SSL
  حالة OLDAP_SSL:
    result = oldap_ssl_connect(data, OLDAP_SSL);
    𐤀𐤌(!result && ssl_installed(conn)) {
      𐤀𐤌(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      وإلا
        result = oldap_perform_bind(data, OLDAP_BIND);
    }
    اكسر;
  حالة OLDAP_STARTTLS:
    𐤀𐤌(code != LDAP_SUCCESS) {
      𐤀𐤌(data->set.use_ssl != CURLUSESSL_TRY)
        result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
      وإلا 𐤀𐤌(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      وإلا
        result = oldap_perform_bind(data, OLDAP_BIND);
      اكسر;
    }
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    𐤀𐤌(result)
      اكسر;
    FALLTHROUGH();
  حالة OLDAP_TLS:
    result = oldap_ssl_connect(data, OLDAP_TLS);
    𐤀𐤌(result)
      result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
    وإلا 𐤀𐤌(ssl_installed(conn)) {
      𐤀𐤌(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      وإلا 𐤀𐤌(data->state.creds)
        result = oldap_perform_bind(data, OLDAP_BIND);
      وإلا {
        /* Version 3 supported: no bind required */
        oldap_state(data, li, OLDAP_STOP);
        result = CURLE_OK;
      }
    }
    اكسر;
#endif

  حالة OLDAP_MECHS:
    result = oldap_state_mechs_resp(data, msg, code);
    اكسر;
  حالة OLDAP_SASL:
    result = oldap_state_sasl_resp(data, msg, code);
    اكسر;
  حالة OLDAP_BIND:
  حالة OLDAP_BINDV2:
    result = oldap_state_bind_resp(data, msg, code);
    اكسر;
  افتراضي:
    /* internal error */
    result = CURLE_COULDNT_CONNECT;
    اكسر;
  }

  ldap_msgfree(msg);

  *done = li->state == OLDAP_STOP;
  𐤀𐤌(*done)
    conn->recv[FIRSTSOCKET] = oldap_recv;

  𐤀𐤌(result && li->msgid) {
    ldap_abandon_ext(li->ld, li->msgid, عدم, عدم);
    li->msgid = 0;
  }
  أرجع result;
}

ساكن CURLcode oldap_disconnect(بنية Curl_easy *data,
                                 بنية connectdata *conn,
                                 bool dead_connection)
{
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  (فراغ)dead_connection;
#ifndef USE_SSL
  (فراغ)data;
#endif

  𐤀𐤌(li && li->ld) {
#ifdef USE_SSL
    𐤀𐤌(ssl_installed(conn)) {
      Sockbuf *sb;
      𐤀𐤌(ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS ||
         ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
        أرجع CURLE_FAILED_INIT;
    }
#endif
    ldap_unbind_ext(li->ld, عدم, عدم);
    li->ld = عدم;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode oldap_do(بنية Curl_easy *data, bool *done)
{
  بنية connectdata *conn = data->conn;
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  بنية ldapreqinfo *lr;
  CURLcode result;
  صحيح rc;
  LDAPURLDesc *lud;
  صحيح msgid;

  𐤀𐤌(!li)
    أرجع CURLE_FAILED_INIT;

  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

  result = oldap_url_parse(data, &lud);
  𐤀𐤌(result)
    اذهب out;

#ifdef USE_SSL
  𐤀𐤌(ssl_installed(conn)) {
    Sockbuf *sb;
    /* re-install the libcurl SSL handlers into the sockbuf. */
    𐤀𐤌((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data)) {
      ldap_free_urldesc(lud);
      أرجع CURLE_FAILED_INIT;
    }
  }
#endif

  rc = ldap_search_ext(li->ld, lud->lud_dn, lud->lud_scope,
                       lud->lud_filter, lud->lud_attrs, 0,
                       عدم, عدم, عدم, 0, &msgid);
  ldap_free_urldesc(lud);
  𐤀𐤌(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: ldap_search_ext %s", ldap_err2string(rc));
    result = CURLE_LDAP_SEARCH_FAILED;
    اذهب out;
  }

  lr = curlx_calloc(1, حجم(بنية ldapreqinfo));
  𐤀𐤌(!lr ||
     Curl_meta_set(data, CURL_META_LDAP_EASY, lr, oldap_easy_dtor)) {
    ldap_abandon_ext(li->ld, msgid, عدم, عدم);
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  lr->msgid = msgid;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  *done = TRUE;

out:
  أرجع result;
}

ساكن CURLcode oldap_done(بنية Curl_easy *data, CURLcode res,
                           bool premature)
{
  بنية connectdata *conn = data->conn;
  بنية ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);

  (فراغ)res;
  (فراغ)premature;

  𐤀𐤌(lr) {
    /* if there was a search in progress, abandon it */
    𐤀𐤌(lr->msgid) {
      بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      𐤀𐤌(li && li->ld) {
        ldap_abandon_ext(li->ld, lr->msgid, عدم, عدم);
      }
      lr->msgid = 0;
    }
    Curl_meta_remove(data, CURL_META_LDAP_EASY);
  }

  أرجع CURLE_OK;
}

ساكن CURLcode client_write(بنية Curl_easy *data,
                             ثابت حرف *prefix, size_t plen,
                             ثابت حرف *value, size_t len,
                             ثابت حرف *suffix, size_t slen)
{
  CURLcode result = CURLE_OK;

  𐤀𐤌(prefix) {
    /* If we have a zero-length value and the prefix ends with a space
       separator, drop the latter. */
    𐤀𐤌(!len && plen && prefix[plen - 1] == ' ')
      plen--;
    result = Curl_client_write(data, CLIENTWRITE_BODY, prefix, plen);
  }
  𐤀𐤌(!result && value) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, value, len);
  }
  𐤀𐤌(!result && suffix) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, suffix, slen);
  }
  أرجع result;
}

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
ساكن bool oldap_str_needs_base64(ثابت حرف *s, size_t len)
{
  size_t i;
  𐤀𐤌(!len)
    أرجع FALSE;
  𐤀𐤌((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    أرجع TRUE;
  لكل(i = 0; i < len; i++)
    𐤀𐤌(!ISPRINT(s[i]))
      أرجع TRUE;
  أرجع FALSE;
}

ساكن CURLcode oldap_recv(بنية Curl_easy *data, int8_t sockindex, حرف *buf,
                           size_t len, size_t *pnread)
{
  بنية connectdata *conn = data->conn;
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  بنية ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);
  صحيح rc;
  LDAPMessage *msg = عدم;
  BerElement *ber = عدم;
  بنية timeval tv = { 0, 0 };
  بنية berval bv, *bvals;
  CURLcode result = CURLE_AGAIN;
  صحيح code;
  حرف *info = عدم;

  (فراغ)len;
  (فراغ)buf;
  (فراغ)sockindex;
  *pnread = 0;
  𐤀𐤌(!li || !lr)
    أرجع CURLE_FAILED_INIT;

  rc = ldap_result(li->ld, lr->msgid, LDAP_MSG_ONE, &tv, &msg);
  𐤀𐤌(rc < 0) {
    failf(data, "LDAP local: search ldap_result %s", ldap_err2string(rc));
    result = CURLE_RECV_ERROR;
  }

  /* error or timed out */
  𐤀𐤌(!msg)
    أرجع result;

  result = CURLE_OK;

  بدل(ldap_msgtype(msg)) {
  حالة LDAP_RES_SEARCH_RESULT:
    lr->msgid = 0;
    rc = ldap_parse_result(li->ld, msg, &code, عدم, &info, عدم, عدم, 0);
    𐤀𐤌(rc) {
      failf(data, "LDAP local: search ldap_parse_result %s",
            ldap_err2string(rc));
      result = CURLE_LDAP_SEARCH_FAILED;
      اكسر;
    }

    /* store the latest code for later retrieval */
    data->info.httpcode = code;

    بدل(code) {
    حالة LDAP_SIZELIMIT_EXCEEDED:
      infof(data, "There are more than %d entries", lr->nument);
      FALLTHROUGH();
    حالة LDAP_SUCCESS:
      data->req.size = data->req.bytecount;
      اكسر;
    افتراضي:
      failf(data, "LDAP remote: search failed %s %s", ldap_err2string(code),
            info ? info : "");
      result = CURLE_LDAP_SEARCH_FAILED;
      اكسر;
    }
    𐤀𐤌(info)
      ldap_memfree(info);
    اكسر;
  حالة LDAP_RES_SEARCH_ENTRY:
    lr->nument++;
    rc = ldap_get_dn_ber(li->ld, msg, &ber, &bv);
    𐤀𐤌(rc < 0) {
      result = CURLE_RECV_ERROR;
      اكسر;
    }

    𐤀𐤌(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
      حرف *dn_b64 = عدم;
      size_t dn_b64_sz = 0;

      /* A DN with e.g. CR/LF would inject LDIF lines, so base64-encode it. */
      result = curlx_base64_encode((uint8_t *)bv.bv_val, bv.bv_len,
                                   &dn_b64, &dn_b64_sz);
      𐤀𐤌(!result)
        result = client_write(data, STRCONST("DN:: "), dn_b64, dn_b64_sz,
                              STRCONST("\n"));
      curlx_free(dn_b64);
    }
    وإلا
      result = client_write(data, STRCONST("DN: "), bv.bv_val, bv.bv_len,
                            STRCONST("\n"));
    𐤀𐤌(result)
      اكسر;

    لكل(rc = ldap_get_attribute_ber(li->ld, msg, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, msg, ber, &bv, &bvals)) {
      صحيح i;
      bool binary;

      𐤀𐤌(!bv.bv_val)
        اكسر;

      /* An attribute type has no base64 form in LDIF, so a type carrying
         control bytes (CR/LF etc.) would let a hostile server inject lines.
         A conformant type name never contains them, so reject it. */
      𐤀𐤌(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
        failf(data, "LDAP local: attribute type contains illegal bytes");
        𐤀𐤌(bvals)
          ber_memfree(bvals);
        result = CURLE_WEIRD_SERVER_REPLY;
        اكسر;
      }

      𐤀𐤌(!bvals) {
        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":\n"));
        𐤀𐤌(result)
          اكسر;
        استمر;
      }

      binary = bv.bv_len > 7 &&
        curl_strnequal(bv.bv_val + bv.bv_len - 7, ";binary", 7);

      لكل(i = 0; bvals[i].bv_val; i++) {
        bool binval = FALSE;

        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":"));
        𐤀𐤌(result)
          اكسر;

        𐤀𐤌(!binary)
          binval = oldap_str_needs_base64(bvals[i].bv_val, bvals[i].bv_len);
        𐤀𐤌(binary || binval) {
          حرف *val_b64 = عدم;
          size_t val_b64_sz = 0;

          /* Binary value, encode to base64. */
          𐤀𐤌(bvals[i].bv_len)
            result = curlx_base64_encode((uint8_t *)bvals[i].bv_val,
                                         bvals[i].bv_len,
                                         &val_b64, &val_b64_sz);
          𐤀𐤌(!result)
            result = client_write(data, STRCONST(": "), val_b64, val_b64_sz,
                                  STRCONST("\n"));
          curlx_free(val_b64);
        }
        وإلا
          result = client_write(data, STRCONST(" "),
                                bvals[i].bv_val, bvals[i].bv_len,
                                STRCONST("\n"));
        𐤀𐤌(result)
          اكسر;
      }

      ber_memfree(bvals);
      bvals = عدم;
      𐤀𐤌(!result)
        result = client_write(data, STRCONST("\n"), عدم, 0, عدم, 0);
      𐤀𐤌(result)
        اكسر;
    }

    𐤀𐤌(!result)
      result = client_write(data, STRCONST("\n"), عدم, 0, عدم, 0);
    𐤀𐤌(!result)
      result = CURLE_AGAIN;
    اكسر;
  }

  ber_free(ber, 0);
  ldap_msgfree(msg);
  أرجع result;
}

فراغ Curl_ldap_version(حرف *buf, size_t bufsz)
{
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  𐤀𐤌(ldap_get_option(عدم, LDAP_OPT_API_INFO, &api) == LDAP_OPT_SUCCESS) {
    unsigned صحيح patch = (unsigned صحيح)(api.ldapai_vendor_version % 100);
    unsigned صحيح major = (unsigned صحيح)(api.ldapai_vendor_version / 10000);
    unsigned صحيح minor =
      (((unsigned صحيح)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u",
                   api.ldapai_vendor_name, major, minor, patch);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((فراغ **)api.ldapai_extensions);
  }
  وإلا
    curl_msnprintf(buf, bufsz, "OpenLDAP");
}

/*
 * LDAP protocol handler.
 */
ثابت بنية Curl_protocol Curl_protocol_ldap = {
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
