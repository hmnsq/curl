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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP)

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

#شامل <ldap.h>

#شامل "urldata.h"
#شامل "url.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "vtls/vtls.h"
#شامل "transfer.h"
#شامل "curl_ldap.h"
#شامل "curlx/base64.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "curl_sasl.h"
#شامل "strcase.h"
#شامل "bufref.h"

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
تعریفنوع شمارش {
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
بیرونی صحیح ldap_pvt_url_scheme2proto(ثابت نویسه *);
بیرونی صحیح ldap_init_fd(ber_socket_t fd, صحیح proto, ثابت نویسه *url,
                        LDAP **ld);
#endif

ایستا Curl_recv oldap_recv;

ساختار ldapconninfo {
  ساختار SASL sasl;          /* SASL-related parameters */
  LDAP *ld;                  /* Openldap connection handle. */
  Curl_recv *recv;           /* For stacking SSL handler */
  Curl_send *send;
  ساختار berval *servercred; /* SASL data from server. */
  ldapstate state;           /* Current machine state. */
  curl_socket_t dupfd;       /* duplicated socket */
  صحیح proto;                 /* LDAP_PROTO_TCP/LDAP_PROTO_UDP/LDAP_PROTO_IPC */
  صحیح msgid;                 /* Current message id. */
};

ساختار ldapreqinfo {
  صحیح msgid;
  صحیح nument;
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
ایستا تهی oldap_state(ساختار Curl_easy *data, ساختار ldapconninfo *li,
                        ldapstate newstate)
{
#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ایستا ثابت نویسه * ثابت names[] = {
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

  اگر(li->state != newstate)
    infof(data, "LDAP %p state change from %s to %s",
          (تهی *)li, names[li->state], names[newstate]);
#وگرنه
  (تهی)data;
#endif
  li->state = newstate;
}

/* Map some particular LDAP error codes to CURLcode values. */
ایستا CURLcode oldap_map_error(صحیح rc, CURLcode result)
{
  انتخاب(rc) {
  حالت LDAP_NO_MEMORY:
    بازگشت CURLE_OUT_OF_MEMORY;
  حالت LDAP_INVALID_CREDENTIALS:
    بازگشت CURLE_LOGIN_DENIED;
  حالت LDAP_PROTOCOL_ERROR:
    بازگشت CURLE_UNSUPPORTED_PROTOCOL;
  حالت LDAP_INSUFFICIENT_ACCESS:
    بازگشت CURLE_REMOTE_ACCESS_DENIED;
  }
  بازگشت result;
}

ایستا CURLcode oldap_url_parse(ساختار Curl_easy *data, LDAPURLDesc **ludp)
{
  CURLcode result = CURLE_OK;
  صحیح rc;
  ایستا ثابت نویسه * ثابت url_errs[] = {
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

  *ludp = پوچ;
  /* `ldap_url_parse() seems to be terrible with urls
   * that have user/pass/options in it. So when we have options or
   * creds from the url, fail without calling the function.
   * Yes, this is super-weird code and I do not like it. */
  اگر((data->state.creds && (data->state.creds->source == CREDS_URL)) ||
     data->state.up.options)
    rc = LDAP_URL_ERR_BADURL;
  وگرنه
    rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), ludp);

  اگر(rc != LDAP_URL_SUCCESS) {
    ثابت نویسه *پیام = "url parsing problem";

    result = rc == LDAP_URL_ERR_MEM ? CURLE_OUT_OF_MEMORY :
      CURLE_URL_MALFORMAT;
    rc -= LDAP_URL_SUCCESS;
    اگر((size_t)rc < CURL_ARRAYSIZE(url_errs))
      پیام = url_errs[rc];
    failf(data, "LDAP local: %s", پیام);
  }
  بازگشت result;
}

/* Parse the login options. */
ایستا CURLcode oldap_parse_login_options(ساختار connectdata *conn)
{
  CURLcode result = CURLE_OK;
  ساختار ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ثابت نویسه *ptr = conn->options;

  DEBUGASSERT(li);
  اگر(!li)
    بازگشت CURLE_FAILED_INIT;

  وقتی(!result && ptr && *ptr) {
    ثابت نویسه *کلید = ptr;
    ثابت نویسه *مقدار;

    وقتی(*ptr && *ptr != '=')
      ptr++;

    مقدار = ptr + 1;

    وقتی(*ptr && *ptr != ';')
      ptr++;

    اگر(checkprefix("AUTH=", کلید))
      result = Curl_sasl_parse_url_auth_option(&li->sasl, مقدار, ptr - مقدار);
    وگرنه
      result = CURLE_SETOPT_OPTION_SYNTAX;

    اگر(*ptr == ';')
      ptr++;
  }

  بازگشت result == CURLE_URL_MALFORMAT ? CURLE_SETOPT_OPTION_SYNTAX : result;
}

ایستا CURLcode oldap_setup_connection(ساختار Curl_easy *data,
                                       ساختار connectdata *conn)
{
  CURLcode result;
  LDAPURLDesc *lud;
  (تهی)conn;

  /* Early URL syntax check. */
  result = oldap_url_parse(data, &lud);
  ldap_free_urldesc(lud);

  بازگشت result;
}

/*
 * Get the SASL authentication challenge from the server credential buffer.
 */
ایستا CURLcode oldap_get_message(ساختار Curl_easy *data, ساختار bufref *out)
{
  ساختار ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  ساختار berval *servercred = li ? li->servercred : پوچ;
  DEBUGASSERT(li);
  اگر(!li)
    بازگشت CURLE_FAILED_INIT;

  اگر(!servercred || !servercred->bv_val)
    بازگشت CURLE_WEIRD_SERVER_REPLY;
  Curl_bufref_set(out, servercred->bv_val, servercred->bv_len, پوچ);
  بازگشت CURLE_OK;
}

/*
 * Sends an initial SASL bind request to the server.
 */
ایستا CURLcode oldap_perform_auth(ساختار Curl_easy *data, ثابت نویسه *mech,
                                   ثابت ساختار bufref *initresp)
{
  ساختار connectdata *conn = data->conn;
  ساختار ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ساختار berval cred;
  ساختار berval *pcred = &cred;
  صحیح rc;

  DEBUGASSERT(li);
  اگر(!li)
    بازگشت CURLE_FAILED_INIT;
  cred.bv_val = (نویسه *)CURL_UNCONST(Curl_bufref_ptr(initresp));
  cred.bv_len = Curl_bufref_len(initresp);
  اگر(!cred.bv_val)
    pcred = پوچ;
  rc = ldap_sasl_bind(li->ld, پوچ, mech, pcred, پوچ, پوچ, &li->msgid);
  اگر(rc != LDAP_SUCCESS)
    بازگشت oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  بازگشت CURLE_OK;
}

/*
 * Sends SASL continuation.
 */
ایستا CURLcode oldap_continue_auth(ساختار Curl_easy *data, ثابت نویسه *mech,
                                    ثابت ساختار bufref *resp)
{
  ساختار connectdata *conn = data->conn;
  ساختار ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ساختار berval cred;
  ساختار berval *pcred = &cred;
  صحیح rc;

  اگر(!li)
    بازگشت CURLE_FAILED_INIT;
  cred.bv_val = (نویسه *)CURL_UNCONST(Curl_bufref_ptr(resp));
  cred.bv_len = Curl_bufref_len(resp);
  اگر(!cred.bv_val)
    pcred = پوچ;
  rc = ldap_sasl_bind(li->ld, پوچ, mech, pcred, پوچ, پوچ, &li->msgid);
  اگر(rc != LDAP_SUCCESS)
    بازگشت oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  بازگشت CURLE_OK;
}

/*
 * Sends SASL bind cancellation.
 */
ایستا CURLcode oldap_cancel_auth(ساختار Curl_easy *data, ثابت نویسه *mech)
{
  ساختار ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  صحیح rc;

  (تهی)mech;
  اگر(!li)
    بازگشت CURLE_FAILED_INIT;
  rc = ldap_sasl_bind(li->ld, پوچ, LDAP_SASL_NULL, پوچ, پوچ, پوچ,
                      &li->msgid);
  اگر(rc != LDAP_SUCCESS)
    بازگشت oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  بازگشت CURLE_OK;
}

/* Starts LDAP simple bind. */
ایستا CURLcode oldap_perform_bind(ساختار Curl_easy *data, ldapstate newstate)
{
  ساختار connectdata *conn = data->conn;
  ساختار ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ثابت نویسه *binddn = پوچ;
  ساختار berval passwd;
  صحیح rc;

  اگر(!li)
    بازگشت CURLE_FAILED_INIT;
  passwd.bv_val = پوچ;
  passwd.bv_len = 0;

  اگر(conn->creds) {
    binddn = Curl_creds_user(conn->creds);
    passwd.bv_val = CURL_UNCONST(Curl_creds_passwd(conn->creds));
    passwd.bv_len = strlen(passwd.bv_val);
  }

  rc = ldap_sasl_bind(li->ld, binddn, LDAP_SASL_SIMPLE, &passwd,
                      پوچ, پوچ, &li->msgid);
  اگر(rc != LDAP_SUCCESS)
    بازگشت oldap_map_error(rc,
                           data->state.creds ?
                           CURLE_LOGIN_DENIED : CURLE_LDAP_CANNOT_BIND);
  oldap_state(data, li, newstate);
  بازگشت CURLE_OK;
}

/* Query the supported SASL authentication mechanisms. */
ایستا CURLcode oldap_perform_mechs(ساختار Curl_easy *data)
{
  ساختار ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  صحیح rc;
  ایستا ثابت نویسه * ثابت supportedSASLMechanisms[] = {
    "supportedSASLMechanisms",
    پوچ
  };

  اگر(!li)
    بازگشت CURLE_FAILED_INIT;
  /* Casting away the const for the 3rd parameter that the LDAP API expects as
     a non-const char ** is potentially unsafe but we believe the lack of
     const in the API was an oversight and that no LDAP implementation
     actually modifies the input. */
  rc = ldap_جستجو_ext(li->ld, "", LDAP_SCOPE_BASE, "(objectclass=*)",
                       (نویسه **)CURL_UNCONST(supportedSASLMechanisms), 0,
                       پوچ, پوچ, پوچ, 0, &li->msgid);
  اگر(rc != LDAP_SUCCESS)
    بازگشت oldap_map_error(rc, CURLE_LOGIN_DENIED);
  oldap_state(data, li, OLDAP_MECHS);
  بازگشت CURLE_OK;
}

/* Starts SASL bind. */
ایستا CURLcode oldap_perform_sasl(ساختار Curl_easy *data)
{
  ساختار ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  saslprogress progress = SASL_IDLE;
  CURLcode result;

  اگر(!li)
    بازگشت CURLE_FAILED_INIT;
  result = Curl_sasl_start(&li->sasl, data, TRUE, &progress);

  oldap_state(data, li, OLDAP_SASL);
  اگر(!result && progress != SASL_INPROGRESS)
    result = Curl_sasl_is_blocked(&li->sasl, data);
  بازگشت result;
}

#ifdef USE_SSL
ایستا صحیح ldapsb_tls_setup(Sockbuf_IO_Desc *sbiod, تهی *arg)
{
  sbiod->sbiod_pvt = arg;
  بازگشت 0;
}

ایستا صحیح ldapsb_tls_remove(Sockbuf_IO_Desc *sbiod)
{
  sbiod->sbiod_pvt = پوچ;
  بازگشت 0;
}

/* if there is a duplicated socket to close, close it */
ایستا صحیح ldapsb_tls_close(Sockbuf_IO_Desc *sbiod)
{
  ساختار Curl_easy *data = sbiod->sbiod_pvt;
  اگر(data) {
    ساختار connectdata *conn = data->conn;
    اگر(conn) {
      ساختار ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      اگر(li && (li->dupfd != CURL_SOCKET_BAD)) {
        sclose(li->dupfd);
        li->dupfd = CURL_SOCKET_BAD;
      }
    }
  }

  بازگشت 0;
}

ایستا صحیح ldapsb_tls_ctrl(Sockbuf_IO_Desc *sbiod, صحیح opt, تهی *arg)
{
  (تهی)arg;
  اگر(opt == LBER_SB_OPT_DATA_READY) {
    ساختار Curl_easy *data = sbiod->sbiod_pvt;
    بازگشت Curl_conn_data_pending(data, FIRSTSOCKET);
  }
  بازگشت 0;
}

ایستا ber_slen_t ldapsb_tls_read(Sockbuf_IO_Desc *sbiod, تهی *buf,
                                  ber_len_t len)
{
  ساختار Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  اگر(data) {
    ساختار connectdata *conn = data->conn;
    اگر(conn) {
      ساختار ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_RECV_ERROR;
      size_t nread;

      اگر(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        بازگشت -1;
      }
      result = (li->recv)(data, FIRSTSOCKET, buf, len, &nread);
      اگر(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nread;
    }
  }
  بازگشت ret;
}

ایستا ber_slen_t ldapsb_tls_write(Sockbuf_IO_Desc *sbiod, تهی *buf,
                                   ber_len_t len)
{
  ساختار Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  اگر(data) {
    ساختار connectdata *conn = data->conn;
    اگر(conn) {
      ساختار ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_SEND_ERROR;
      size_t nwritten;

      اگر(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        بازگشت -1;
      }
      result = (li->send)(data, FIRSTSOCKET, buf, len, FALSE, &nwritten);
      اگر(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nwritten;
    }
  }
  بازگشت ret;
}

ایستا Sockbuf_IO ldapsb_tls = {
  ldapsb_tls_setup,
  ldapsb_tls_remove,
  ldapsb_tls_ctrl,
  ldapsb_tls_read,
  ldapsb_tls_write,
  ldapsb_tls_close
};

ایستا bool ssl_installed(ساختار connectdata *conn)
{
  ساختار ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  بازگشت li && li->recv;
}

ایستا CURLcode oldap_ssl_connect(ساختار Curl_easy *data, ldapstate newstate)
{
  ساختار connectdata *conn = data->conn;
  ساختار ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  bool ssldone = FALSE;
  CURLcode result;

  اگر(!li)
    بازگشت CURLE_FAILED_INIT;
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  اگر(result)
    بازگشت result;
  oldap_state(data, li, newstate);

  اگر(ssldone) {
    Sockbuf *sb;

    /* Install the libcurl SSL handlers into the sockbuf. */
    اگر((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
      بازگشت CURLE_FAILED_INIT;
    li->recv = conn->recv[FIRSTSOCKET];
    li->send = conn->send[FIRSTSOCKET];
  }

  بازگشت result;
}

/* Send the STARTTLS request */
ایستا CURLcode oldap_perform_starttls(ساختار Curl_easy *data)
{
  ساختار ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  صحیح rc;

  اگر(!li)
    بازگشت CURLE_FAILED_INIT;
  rc = ldap_start_tls(li->ld, پوچ, پوچ, &li->msgid);
  اگر(rc != LDAP_SUCCESS)
    بازگشت oldap_map_error(rc, CURLE_USE_SSL_FAILED);
  oldap_state(data, li, OLDAP_STARTTLS);
  بازگشت CURLE_OK;
}
#endif

ایستا تهی oldap_easy_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار ldapreqinfo *lr = entry;
  (تهی)کلید;
  (تهی)klen;
  curlx_free(lr);
}

ایستا تهی oldap_conn_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار ldapconninfo *li = entry;
  (تهی)کلید;
  (تهی)klen;
  اگر(li->ld) {
    ldap_unbind_ext(li->ld, پوچ, پوچ);
    li->ld = پوچ;
  }
  curlx_free(li);
}

/* SASL parameters for the ldap protocol */
ایستا ثابت ساختار SASLproto saslldap = {
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

ایستا CURLcode oldap_connect(ساختار Curl_easy *data, bool *done)
{
  ساختار connectdata *conn = data->conn;
  ساختار ldapconninfo *li;
  ایستا ثابت صحیح version = LDAP_VERSION3;
  نویسه *hosturl = پوچ;
  CURLcode result;
  صحیح rc;
#ifdef CURL_OPENLDAP_DEBUG
  ایستا صحیح do_trace = -1;
#endif

  (تهی)done;

  li = curlx_calloc(1, اندازه(ساختار ldapconninfo));
  اگر(!li) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  li->dupfd = CURL_SOCKET_BAD;

  result = Curl_conn_meta_set(conn, CURL_META_LDAP_CONN, li, oldap_conn_dtor);
  اگر(result)
    برو out;

  li->proto = ldap_pvt_url_scheme2proto(data->state.origin->scheme->name);

  /* Initialize the SASL storage */
  Curl_sasl_init(&li->sasl, data, &saslldap);

  result = oldap_parse_login_options(conn);
  اگر(result)
    برو out;

  hosturl = curl_maprintf("%s://%s:%u",
                          conn->scheme->name,
                          conn->origin->ipv6 ?
                          conn->origin->user_hostname :
                          conn->origin->hostname,
                          conn->origin->port);
  اگر(!hosturl) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  {
#ifdef _WIN32
    SOCKET dupfd = INVALID_SOCKET;
    WSAPROTOCOL_INFO pi;
    اگر(WSADuplicateSocket(conn->sock[FIRSTSOCKET], GetCurrentProcessId(),
                          &pi))
      ; /* error */
    وگرنه
      dupfd = CURL_SOCKET(pi.iAddressFamily, pi.iSocketType, pi.iProtocol);
#وگرنه
#ifdef F_DUPFD_CLOEXEC
    صحیح dupfd = fcntl(conn->sock[FIRSTSOCKET], F_DUPFD_CLOEXEC, 0);
#وگرنه
    صحیح dupfd = dup(conn->sock[FIRSTSOCKET]);
#ifdef HAVE_FCNTL
    اگر((dupfd != CURL_SOCKET_BAD) &&
       (fcntl(dupfd, F_SETFD, FD_CLOEXEC) < 0)) {
      sclose(dupfd);
      dupfd = CURL_SOCKET_BAD;
    }
#endif /* HAVE_FCNTL */
#endif /* F_DUPFD_CLOEXEC */
#endif /* _WIN32 */

    اگر(dupfd == CURL_SOCKET_BAD) {
      result = CURLE_COULDNT_CONNECT;
      برو out;
    }
    rc = ldap_init_fd((ber_socket_t)dupfd, li->proto, hosturl, &li->ld);
    اگر(rc)
      sclose(dupfd);
    وگرنه
      /* store the socket, as when this is used by TLS we close this ourselves
         and at this point the code does not yet know if this will use TLS or
         not */
      li->dupfd = dupfd;
  }
  اگر(rc) {
    failf(data, "LDAP local: Cannot connect to %s, %s",
          hosturl, ldap_err2string(rc));
    result = CURLE_COULDNT_CONNECT;
    برو out;
  }

#ifdef CURL_OPENLDAP_DEBUG
  اگر(do_trace < 0) {
    ثابت نویسه *env = getenv("CURL_OPENLDAP_TRACE");
    curl_off_t e = 0;
    اگر(!curlx_str_عدد(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  اگر(do_trace)
    ldap_set_option(li->ld, LDAP_OPT_DEBUG_LEVEL, &do_trace);
#endif

  /* Try version 3 first. */
  ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);

  /* Do not chase referrals. */
  ldap_set_option(li->ld, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  {
    ber_len_t max = 256 * 1024;
    Sockbuf *sb;
    اگر((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       /* Set the maximum allowed size of an incoming message, which to
          OpenLDAP means that it will malloc() memory up to this size. If not
          set, there is no limit and we instead risk a malloc() failure. */
       !ber_sockbuf_ctrl(sb, LBER_SB_OPT_SET_MAX_INCOMING, &max)) {
      result = CURLE_FAILED_INIT;
      برو out;
    }
  }

#ifdef USE_SSL
  اگر(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = oldap_ssl_connect(data, OLDAP_SSL);
    برو out;
  }

  اگر(data->set.use_ssl) {
    result = oldap_perform_starttls(data);
    اگر(!result || data->set.use_ssl != CURLUSESSL_TRY)
      برو out;
  }
#endif

  اگر(li->sasl.prefmech != SASL_AUTH_NONE) {
    result = oldap_perform_mechs(data);
    برو out;
  }

  /* Force bind even if anonymous bind is not needed in protocol version 3
     to detect missing version 3 support. */
  result = oldap_perform_bind(data, OLDAP_BIND);

out:
  curlx_free(hosturl);
  بازگشت result;
}

/* Handle the supported SASL mechanisms query response */
ایستا CURLcode oldap_state_mechs_resp(ساختار Curl_easy *data,
                                       LDAPMessage *پیام, صحیح code)
{
  ساختار connectdata *conn;
  ساختار ldapconninfo *li;
  صحیح rc;
  BerElement *ber = پوچ;
  CURLcode result = CURLE_OK;
  ساختار berval bv, *bvals;

  اگر(!data)
    بازگشت CURLE_FAILED_INIT;

  conn = data->conn;
  li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);

  اگر(!li)
    بازگشت CURLE_FAILED_INIT;
  انتخاب(ldap_msgtype(پیام)) {
  حالت LDAP_RES_جستجو_ENTRY:
    /* Got a list of supported SASL mechanisms. */
    اگر(code != LDAP_SUCCESS && code != LDAP_NO_RESULTS_RETURNED)
      بازگشت CURLE_LOGIN_DENIED;

    rc = ldap_get_dn_ber(li->ld, پیام, &ber, &bv);
    اگر(rc < 0)
      بازگشت oldap_map_error(rc, CURLE_BAD_CONTENT_ENCODING);
    برای(rc = ldap_get_attribute_ber(li->ld, پیام, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, پیام, ber, &bv, &bvals)) {
      صحیح i;

      اگر(!bv.bv_val)
        بشکن;

      اگر(bvals) {
        برای(i = 0; bvals[i].bv_val; i++) {
          size_t llen;
          unsigned short mech =
            Curl_sasl_decode_mech((ثابت نویسه *)bvals[i].bv_val,
                                  bvals[i].bv_len, &llen);
          اگر(bvals[i].bv_len == llen)
            li->sasl.authmechs |= mech;
        }
        ber_memfree(bvals);
      }
    }
    ber_free(ber, 0);
    بشکن;

  حالت LDAP_RES_جستجو_RESULT:
    انتخاب(code) {
    حالت LDAP_SIZELIMIT_EXCEEDED:
      infof(data, "Too many authentication mechanisms");
      FALLTHROUGH();
    حالت LDAP_SUCCESS:
    حالت LDAP_NO_RESULTS_RETURNED:
      اگر(Curl_sasl_can_authenticate(&li->sasl, data))
        result = oldap_perform_sasl(data);
      وگرنه
        result = CURLE_LOGIN_DENIED;
      بشکن;
    پیشفرض:
      result = oldap_map_error(code, CURLE_LOGIN_DENIED);
      بشکن;
    }
    بشکن;
  پیشفرض:
    بشکن;
  }
  بازگشت result;
}

/* Handle a SASL bind response. */
ایستا CURLcode oldap_state_sasl_resp(ساختار Curl_easy *data,
                                      LDAPMessage *پیام, صحیح code)
{
  ساختار connectdata *conn = data->conn;
  ساختار ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode result = CURLE_OK;
  saslprogress progress;
  صحیح rc;

  اگر(!li)
    بازگشت CURLE_FAILED_INIT;
  li->servercred = پوچ;
  rc = ldap_parse_sasl_bind_result(li->ld, پیام, &li->servercred, 0);
  اگر(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: sasl ldap_parse_result %s", ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LOGIN_DENIED);
  }
  وگرنه {
    result = Curl_sasl_continue(&li->sasl, data, code, &progress);
    اگر(!result) {
      انتخاب(progress) {
      حالت SASL_DONE:
        oldap_state(data, li, OLDAP_STOP);   /* Authenticated */
        بشکن;
      حالت SASL_IDLE:            /* No mechanism left after cancellation */
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
        بشکن;
      پیشفرض:
        بشکن;
      }
    }
  }

  اگر(li->servercred)
    ber_bvfree(li->servercred);
  بازگشت result;
}

/* Handle a simple bind response. */
ایستا CURLcode oldap_state_bind_resp(ساختار Curl_easy *data, LDAPMessage *پیام,
                                      صحیح code)
{
  ساختار connectdata *conn = data->conn;
  ساختار ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode result = CURLE_OK;
  ساختار berval *bv = پوچ;
  صحیح rc;

  اگر(!li)
    بازگشت CURLE_FAILED_INIT;

  اگر(code != LDAP_SUCCESS)
    بازگشت oldap_map_error(code, CURLE_LDAP_CANNOT_BIND);

  rc = ldap_parse_sasl_bind_result(li->ld, پیام, &bv, 0);
  اگر(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: bind ldap_parse_sasl_bind_result %s",
          ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  }
  وگرنه
    oldap_state(data, li, OLDAP_STOP);

  اگر(bv)
    ber_bvfree(bv);
  بازگشت result;
}

ایستا CURLcode oldap_connecting(ساختار Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;
  ساختار ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  LDAPMessage *پیام = پوچ;
  ساختار timeval tv = { 0, 0 };
  صحیح code = LDAP_SUCCESS;
  صحیح rc;

  اگر(!li)
    بازگشت CURLE_FAILED_INIT;

  اگر(li->state != OLDAP_SSL && li->state != OLDAP_TLS) {
    /* Get response to last command. */
    rc = ldap_result(li->ld, li->msgid, LDAP_پیام_ONE, &tv, &پیام);
    انتخاب(rc) {
    حالت 0:                               /* Timed out. */
      بازگشت CURLE_OK;
    حالت LDAP_RES_جستجو_ENTRY:
    حالت LDAP_RES_جستجو_REFERENCE:
      بشکن;
    پیشفرض:
      li->msgid = 0;                      /* Nothing to abandon upon error. */
      اگر(rc < 0) {
        failf(data, "LDAP local: connecting ldap_result %s",
              ldap_err2string(rc));
        بازگشت oldap_map_error(rc, CURLE_COULDNT_CONNECT);
      }
      بشکن;
    }

    /* Get error code from message. */
    rc = ldap_parse_result(li->ld, پیام, &code, پوچ, پوچ, پوچ, پوچ, 0);
    اگر(rc)
      code = rc;
    وگرنه {
      /* store the latest code for later retrieval */
      data->info.httpcode = code;
    }

    /* If protocol version 3 is not supported, fallback to version 2. */
    اگر(code == LDAP_PROTOCOL_ERROR && li->state != OLDAP_BINDV2 &&
#ifdef USE_SSL
       (ssl_installed(conn) || data->set.use_ssl <= CURLUSESSL_TRY) &&
#endif
       li->sasl.prefmech == SASL_AUTH_NONE) {
      ایستا ثابت صحیح version = LDAP_VERSION2;

      ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);
      ldap_msgfree(پیام);
      بازگشت oldap_perform_bind(data, OLDAP_BINDV2);
    }
  }

  /* Handle response message according to current state. */
  انتخاب(li->state) {

#ifdef USE_SSL
  حالت OLDAP_SSL:
    result = oldap_ssl_connect(data, OLDAP_SSL);
    اگر(!result && ssl_installed(conn)) {
      اگر(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      وگرنه
        result = oldap_perform_bind(data, OLDAP_BIND);
    }
    بشکن;
  حالت OLDAP_STARTTLS:
    اگر(code != LDAP_SUCCESS) {
      اگر(data->set.use_ssl != CURLUSESSL_TRY)
        result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
      وگرنه اگر(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      وگرنه
        result = oldap_perform_bind(data, OLDAP_BIND);
      بشکن;
    }
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    اگر(result)
      بشکن;
    FALLTHROUGH();
  حالت OLDAP_TLS:
    result = oldap_ssl_connect(data, OLDAP_TLS);
    اگر(result)
      result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
    وگرنه اگر(ssl_installed(conn)) {
      اگر(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      وگرنه اگر(data->state.creds)
        result = oldap_perform_bind(data, OLDAP_BIND);
      وگرنه {
        /* Version 3 supported: no bind required */
        oldap_state(data, li, OLDAP_STOP);
        result = CURLE_OK;
      }
    }
    بشکن;
#endif

  حالت OLDAP_MECHS:
    result = oldap_state_mechs_resp(data, پیام, code);
    بشکن;
  حالت OLDAP_SASL:
    result = oldap_state_sasl_resp(data, پیام, code);
    بشکن;
  حالت OLDAP_BIND:
  حالت OLDAP_BINDV2:
    result = oldap_state_bind_resp(data, پیام, code);
    بشکن;
  پیشفرض:
    /* internal error */
    result = CURLE_COULDNT_CONNECT;
    بشکن;
  }

  ldap_msgfree(پیام);

  *done = li->state == OLDAP_STOP;
  اگر(*done)
    conn->recv[FIRSTSOCKET] = oldap_recv;

  اگر(result && li->msgid) {
    ldap_abandon_ext(li->ld, li->msgid, پوچ, پوچ);
    li->msgid = 0;
  }
  بازگشت result;
}

ایستا CURLcode oldap_disconnect(ساختار Curl_easy *data,
                                 ساختار connectdata *conn,
                                 bool dead_connection)
{
  ساختار ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  (تهی)dead_connection;
#ifndef USE_SSL
  (تهی)data;
#endif

  اگر(li && li->ld) {
#ifdef USE_SSL
    اگر(ssl_installed(conn)) {
      Sockbuf *sb;
      اگر(ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS ||
         ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
        بازگشت CURLE_FAILED_INIT;
    }
#endif
    ldap_unbind_ext(li->ld, پوچ, پوچ);
    li->ld = پوچ;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode oldap_do(ساختار Curl_easy *data, bool *done)
{
  ساختار connectdata *conn = data->conn;
  ساختار ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ساختار ldapreqinfo *lr;
  CURLcode result;
  صحیح rc;
  LDAPURLDesc *lud;
  صحیح msgid;

  اگر(!li)
    بازگشت CURLE_FAILED_INIT;

  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

  result = oldap_url_parse(data, &lud);
  اگر(result)
    برو out;

#ifdef USE_SSL
  اگر(ssl_installed(conn)) {
    Sockbuf *sb;
    /* re-install the libcurl SSL handlers into the sockbuf. */
    اگر((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data)) {
      ldap_free_urldesc(lud);
      بازگشت CURLE_FAILED_INIT;
    }
  }
#endif

  rc = ldap_جستجو_ext(li->ld, lud->lud_dn, lud->lud_scope,
                       lud->lud_filter, lud->lud_attrs, 0,
                       پوچ, پوچ, پوچ, 0, &msgid);
  ldap_free_urldesc(lud);
  اگر(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: ldap_search_ext %s", ldap_err2string(rc));
    result = CURLE_LDAP_جستجو_FAILED;
    برو out;
  }

  lr = curlx_calloc(1, اندازه(ساختار ldapreqinfo));
  اگر(!lr ||
     Curl_meta_set(data, CURL_META_LDAP_EASY, lr, oldap_easy_dtor)) {
    ldap_abandon_ext(li->ld, msgid, پوچ, پوچ);
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  lr->msgid = msgid;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  *done = TRUE;

out:
  بازگشت result;
}

ایستا CURLcode oldap_done(ساختار Curl_easy *data, CURLcode res,
                           bool premature)
{
  ساختار connectdata *conn = data->conn;
  ساختار ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);

  (تهی)res;
  (تهی)premature;

  اگر(lr) {
    /* if there was a search in progress, abandon it */
    اگر(lr->msgid) {
      ساختار ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      اگر(li && li->ld) {
        ldap_abandon_ext(li->ld, lr->msgid, پوچ, پوچ);
      }
      lr->msgid = 0;
    }
    Curl_meta_remove(data, CURL_META_LDAP_EASY);
  }

  بازگشت CURLE_OK;
}

ایستا CURLcode client_write(ساختار Curl_easy *data,
                             ثابت نویسه *prefix, size_t plen,
                             ثابت نویسه *مقدار, size_t len,
                             ثابت نویسه *suffix, size_t slen)
{
  CURLcode result = CURLE_OK;

  اگر(prefix) {
    /* If we have a zero-length value and the prefix ends with a space
       separator, drop the latter. */
    اگر(!len && plen && prefix[plen - 1] == ' ')
      plen--;
    result = Curl_client_write(data, CLIENTWRITE_BODY, prefix, plen);
  }
  اگر(!result && مقدار) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, مقدار, len);
  }
  اگر(!result && suffix) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, suffix, slen);
  }
  بازگشت result;
}

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
ایستا bool oldap_str_needs_base64(ثابت نویسه *s, size_t len)
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

ایستا CURLcode oldap_recv(ساختار Curl_easy *data, int8_t sockindex, نویسه *buf,
                           size_t len, size_t *pnread)
{
  ساختار connectdata *conn = data->conn;
  ساختار ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ساختار ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);
  صحیح rc;
  LDAPMessage *پیام = پوچ;
  BerElement *ber = پوچ;
  ساختار timeval tv = { 0, 0 };
  ساختار berval bv, *bvals;
  CURLcode result = CURLE_AGAIN;
  صحیح code;
  نویسه *info = پوچ;

  (تهی)len;
  (تهی)buf;
  (تهی)sockindex;
  *pnread = 0;
  اگر(!li || !lr)
    بازگشت CURLE_FAILED_INIT;

  rc = ldap_result(li->ld, lr->msgid, LDAP_پیام_ONE, &tv, &پیام);
  اگر(rc < 0) {
    failf(data, "LDAP local: search ldap_result %s", ldap_err2string(rc));
    result = CURLE_RECV_ERROR;
  }

  /* error or timed out */
  اگر(!پیام)
    بازگشت result;

  result = CURLE_OK;

  انتخاب(ldap_msgtype(پیام)) {
  حالت LDAP_RES_جستجو_RESULT:
    lr->msgid = 0;
    rc = ldap_parse_result(li->ld, پیام, &code, پوچ, &info, پوچ, پوچ, 0);
    اگر(rc) {
      failf(data, "LDAP local: search ldap_parse_result %s",
            ldap_err2string(rc));
      result = CURLE_LDAP_جستجو_FAILED;
      بشکن;
    }

    /* store the latest code for later retrieval */
    data->info.httpcode = code;

    انتخاب(code) {
    حالت LDAP_SIZELIMIT_EXCEEDED:
      infof(data, "There are more than %d entries", lr->nument);
      FALLTHROUGH();
    حالت LDAP_SUCCESS:
      data->req.size = data->req.bytecount;
      بشکن;
    پیشفرض:
      failf(data, "LDAP remote: search failed %s %s", ldap_err2string(code),
            info ? info : "");
      result = CURLE_LDAP_جستجو_FAILED;
      بشکن;
    }
    اگر(info)
      ldap_memfree(info);
    بشکن;
  حالت LDAP_RES_جستجو_ENTRY:
    lr->nument++;
    rc = ldap_get_dn_ber(li->ld, پیام, &ber, &bv);
    اگر(rc < 0) {
      result = CURLE_RECV_ERROR;
      بشکن;
    }

    اگر(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
      نویسه *dn_b64 = پوچ;
      size_t dn_b64_sz = 0;

      /* A DN with e.g. CR/LF would inject LDIF lines, so base64-encode it. */
      result = curlx_base64_encode((uint8_t *)bv.bv_val, bv.bv_len,
                                   &dn_b64, &dn_b64_sz);
      اگر(!result)
        result = client_write(data, STRCONST("DN:: "), dn_b64, dn_b64_sz,
                              STRCONST("\n"));
      curlx_free(dn_b64);
    }
    وگرنه
      result = client_write(data, STRCONST("DN: "), bv.bv_val, bv.bv_len,
                            STRCONST("\n"));
    اگر(result)
      بشکن;

    برای(rc = ldap_get_attribute_ber(li->ld, پیام, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, پیام, ber, &bv, &bvals)) {
      صحیح i;
      bool دودویی;

      اگر(!bv.bv_val)
        بشکن;

      /* An attribute type has no base64 form in LDIF, so a type carrying
         control bytes (CR/LF etc.) would let a hostile server inject lines.
         A conformant type name never contains them, so reject it. */
      اگر(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
        failf(data, "LDAP local: attribute type contains illegal bytes");
        اگر(bvals)
          ber_memfree(bvals);
        result = CURLE_WEIRD_SERVER_REPLY;
        بشکن;
      }

      اگر(!bvals) {
        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":\n"));
        اگر(result)
          بشکن;
        ادامه;
      }

      دودویی = bv.bv_len > 7 &&
        curl_strnequal(bv.bv_val + bv.bv_len - 7, ";binary", 7);

      برای(i = 0; bvals[i].bv_val; i++) {
        bool binval = FALSE;

        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":"));
        اگر(result)
          بشکن;

        اگر(!دودویی)
          binval = oldap_str_needs_base64(bvals[i].bv_val, bvals[i].bv_len);
        اگر(دودویی || binval) {
          نویسه *val_b64 = پوچ;
          size_t val_b64_sz = 0;

          /* Binary value, encode to base64. */
          اگر(bvals[i].bv_len)
            result = curlx_base64_encode((uint8_t *)bvals[i].bv_val,
                                         bvals[i].bv_len,
                                         &val_b64, &val_b64_sz);
          اگر(!result)
            result = client_write(data, STRCONST(": "), val_b64, val_b64_sz,
                                  STRCONST("\n"));
          curlx_free(val_b64);
        }
        وگرنه
          result = client_write(data, STRCONST(" "),
                                bvals[i].bv_val, bvals[i].bv_len,
                                STRCONST("\n"));
        اگر(result)
          بشکن;
      }

      ber_memfree(bvals);
      bvals = پوچ;
      اگر(!result)
        result = client_write(data, STRCONST("\n"), پوچ, 0, پوچ, 0);
      اگر(result)
        بشکن;
    }

    اگر(!result)
      result = client_write(data, STRCONST("\n"), پوچ, 0, پوچ, 0);
    اگر(!result)
      result = CURLE_AGAIN;
    بشکن;
  }

  ber_free(ber, 0);
  ldap_msgfree(پیام);
  بازگشت result;
}

تهی Curl_ldap_version(نویسه *buf, size_t bufsz)
{
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  اگر(ldap_get_option(پوچ, LDAP_OPT_API_INFO, &api) == LDAP_OPT_SUCCESS) {
    unsigned صحیح patch = (unsigned صحیح)(api.ldapai_vendor_version % 100);
    unsigned صحیح major = (unsigned صحیح)(api.ldapai_vendor_version / 10000);
    unsigned صحیح minor =
      (((unsigned صحیح)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u",
                   api.ldapai_vendor_name, major, minor, patch);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((تهی **)api.ldapai_extensions);
  }
  وگرنه
    curl_msnprintf(buf, bufsz, "OpenLDAP");
}

/*
 * LDAP protocol handler.
 */
ثابت ساختار Curl_protocol Curl_protocol_ldap = {
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
