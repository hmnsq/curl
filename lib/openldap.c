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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP)

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

#அந்தர்பாவ <ldap.h>

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "curl_ldap.h"
#அந்தர்பாவ "curlx/base64.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "curl_sasl.h"
#அந்தர்பாவ "strcase.h"
#அந்தர்பாவ "bufref.h"

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
வகைவரை எண்ணிக்கை {
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
வெளி முழு ldap_pvt_url_scheme2proto(மாறிலி எழுத்து *);
வெளி முழு ldap_init_fd(ber_socket_t fd, முழு proto, மாறிலி எழுத்து *url,
                        LDAP **ld);
#endif

நிலையான Curl_recv oldap_recv;

கட்டமைப்பு ldapconninfo {
  கட்டமைப்பு SASL sasl;          /* SASL-related parameters */
  LDAP *ld;                  /* Openldap connection handle. */
  Curl_recv *recv;           /* For stacking SSL handler */
  Curl_send *send;
  கட்டமைப்பு berval *servercred; /* SASL data from server. */
  ldapstate state;           /* Current machine state. */
  curl_socket_t dupfd;       /* duplicated socket */
  முழு proto;                 /* LDAP_PROTO_TCP/LDAP_PROTO_UDP/LDAP_PROTO_IPC */
  முழு msgid;                 /* Current message id. */
};

கட்டமைப்பு ldapreqinfo {
  முழு msgid;
  முழு nument;
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
நிலையான வெறுமை oldap_state(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு ldapconninfo *li,
                        ldapstate newstate)
{
#எனில் defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  நிலையான மாறிலி எழுத்து * மாறிலி names[] = {
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

  எனில்(li->state != newstate)
    infof(data, "LDAP %p state change from %s to %s",
          (வெறுமை *)li, names[li->state], names[newstate]);
#இல்லையெனில்
  (வெறுமை)data;
#endif
  li->state = newstate;
}

/* Map some particular LDAP error codes to CURLcode values. */
நிலையான CURLcode oldap_map_error(முழு rc, CURLcode result)
{
  தேர்வு(rc) {
  நிலை LDAP_NO_MEMORY:
    திரும்பு CURLE_OUT_OF_MEMORY;
  நிலை LDAP_INVALID_CREDENTIALS:
    திரும்பு CURLE_LOGIN_DENIED;
  நிலை LDAP_PROTOCOL_ERROR:
    திரும்பு CURLE_UNSUPPORTED_PROTOCOL;
  நிலை LDAP_INSUFFICIENT_ACCESS:
    திரும்பு CURLE_REMOTE_ACCESS_DENIED;
  }
  திரும்பு result;
}

நிலையான CURLcode oldap_url_parse(கட்டமைப்பு Curl_easy *data, LDAPURLDesc **ludp)
{
  CURLcode result = CURLE_OK;
  முழு rc;
  நிலையான மாறிலி எழுத்து * மாறிலி url_errs[] = {
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

  *ludp = வெற்று;
  /* `ldap_url_parse() seems to be terrible with urls
   * that have user/pass/options in it. So when we have options or
   * creds from the url, fail without calling the function.
   * Yes, this is super-weird code and I do not like it. */
  எனில்((data->state.creds && (data->state.creds->source == CREDS_URL)) ||
     data->state.up.options)
    rc = LDAP_URL_ERR_BADURL;
  இல்லையெனில்
    rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), ludp);

  எனில்(rc != LDAP_URL_SUCCESS) {
    மாறிலி எழுத்து *செய்தி = "url parsing problem";

    result = rc == LDAP_URL_ERR_MEM ? CURLE_OUT_OF_MEMORY :
      CURLE_URL_MALFORMAT;
    rc -= LDAP_URL_SUCCESS;
    எனில்((size_t)rc < CURL_ARRAYSIZE(url_errs))
      செய்தி = url_errs[rc];
    failf(data, "LDAP local: %s", செய்தி);
  }
  திரும்பு result;
}

/* Parse the login options. */
நிலையான CURLcode oldap_parse_login_options(கட்டமைப்பு connectdata *conn)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  மாறிலி எழுத்து *ptr = conn->options;

  DEBUGASSERT(li);
  எனில்(!li)
    திரும்பு CURLE_FAILED_INIT;

  போது(!result && ptr && *ptr) {
    மாறிலி எழுத்து *சாவி = ptr;
    மாறிலி எழுத்து *மதிப்பு;

    போது(*ptr && *ptr != '=')
      ptr++;

    மதிப்பு = ptr + 1;

    போது(*ptr && *ptr != ';')
      ptr++;

    எனில்(checkprefix("AUTH=", சாவி))
      result = Curl_sasl_parse_url_auth_option(&li->sasl, மதிப்பு, ptr - மதிப்பு);
    இல்லையெனில்
      result = CURLE_SETOPT_OPTION_SYNTAX;

    எனில்(*ptr == ';')
      ptr++;
  }

  திரும்பு result == CURLE_URL_MALFORMAT ? CURLE_SETOPT_OPTION_SYNTAX : result;
}

நிலையான CURLcode oldap_setup_connection(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு connectdata *conn)
{
  CURLcode result;
  LDAPURLDesc *lud;
  (வெறுமை)conn;

  /* Early URL syntax check. */
  result = oldap_url_parse(data, &lud);
  ldap_free_urldesc(lud);

  திரும்பு result;
}

/*
 * Get the SASL authentication challenge from the server credential buffer.
 */
நிலையான CURLcode oldap_get_message(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு bufref *out)
{
  கட்டமைப்பு ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  கட்டமைப்பு berval *servercred = li ? li->servercred : வெற்று;
  DEBUGASSERT(li);
  எனில்(!li)
    திரும்பு CURLE_FAILED_INIT;

  எனில்(!servercred || !servercred->bv_val)
    திரும்பு CURLE_WEIRD_SERVER_REPLY;
  Curl_bufref_set(out, servercred->bv_val, servercred->bv_len, வெற்று);
  திரும்பு CURLE_OK;
}

/*
 * Sends an initial SASL bind request to the server.
 */
நிலையான CURLcode oldap_perform_auth(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *mech,
                                   மாறிலி கட்டமைப்பு bufref *initresp)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  கட்டமைப்பு berval cred;
  கட்டமைப்பு berval *pcred = &cred;
  முழு rc;

  DEBUGASSERT(li);
  எனில்(!li)
    திரும்பு CURLE_FAILED_INIT;
  cred.bv_val = (எழுத்து *)CURL_UNCONST(Curl_bufref_ptr(initresp));
  cred.bv_len = Curl_bufref_len(initresp);
  எனில்(!cred.bv_val)
    pcred = வெற்று;
  rc = ldap_sasl_bind(li->ld, வெற்று, mech, pcred, வெற்று, வெற்று, &li->msgid);
  எனில்(rc != LDAP_SUCCESS)
    திரும்பு oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  திரும்பு CURLE_OK;
}

/*
 * Sends SASL continuation.
 */
நிலையான CURLcode oldap_continue_auth(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *mech,
                                    மாறிலி கட்டமைப்பு bufref *resp)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  கட்டமைப்பு berval cred;
  கட்டமைப்பு berval *pcred = &cred;
  முழு rc;

  எனில்(!li)
    திரும்பு CURLE_FAILED_INIT;
  cred.bv_val = (எழுத்து *)CURL_UNCONST(Curl_bufref_ptr(resp));
  cred.bv_len = Curl_bufref_len(resp);
  எனில்(!cred.bv_val)
    pcred = வெற்று;
  rc = ldap_sasl_bind(li->ld, வெற்று, mech, pcred, வெற்று, வெற்று, &li->msgid);
  எனில்(rc != LDAP_SUCCESS)
    திரும்பு oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  திரும்பு CURLE_OK;
}

/*
 * Sends SASL bind cancellation.
 */
நிலையான CURLcode oldap_cancel_auth(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *mech)
{
  கட்டமைப்பு ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  முழு rc;

  (வெறுமை)mech;
  எனில்(!li)
    திரும்பு CURLE_FAILED_INIT;
  rc = ldap_sasl_bind(li->ld, வெற்று, LDAP_SASL_NULL, வெற்று, வெற்று, வெற்று,
                      &li->msgid);
  எனில்(rc != LDAP_SUCCESS)
    திரும்பு oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  திரும்பு CURLE_OK;
}

/* Starts LDAP simple bind. */
நிலையான CURLcode oldap_perform_bind(கட்டமைப்பு Curl_easy *data, ldapstate newstate)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  மாறிலி எழுத்து *binddn = வெற்று;
  கட்டமைப்பு berval passwd;
  முழு rc;

  எனில்(!li)
    திரும்பு CURLE_FAILED_INIT;
  passwd.bv_val = வெற்று;
  passwd.bv_len = 0;

  எனில்(conn->creds) {
    binddn = Curl_creds_user(conn->creds);
    passwd.bv_val = CURL_UNCONST(Curl_creds_passwd(conn->creds));
    passwd.bv_len = strlen(passwd.bv_val);
  }

  rc = ldap_sasl_bind(li->ld, binddn, LDAP_SASL_SIMPLE, &passwd,
                      வெற்று, வெற்று, &li->msgid);
  எனில்(rc != LDAP_SUCCESS)
    திரும்பு oldap_map_error(rc,
                           data->state.creds ?
                           CURLE_LOGIN_DENIED : CURLE_LDAP_CANNOT_BIND);
  oldap_state(data, li, newstate);
  திரும்பு CURLE_OK;
}

/* Query the supported SASL authentication mechanisms. */
நிலையான CURLcode oldap_perform_mechs(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  முழு rc;
  நிலையான மாறிலி எழுத்து * மாறிலி supportedSASLMechanisms[] = {
    "supportedSASLMechanisms",
    வெற்று
  };

  எனில்(!li)
    திரும்பு CURLE_FAILED_INIT;
  /* Casting away the const for the 3rd parameter that the LDAP API expects as
     a non-const char ** is potentially unsafe but we believe the lack of
     const in the API was an oversight and that no LDAP implementation
     actually modifies the input. */
  rc = ldap_தேடல்_ext(li->ld, "", LDAP_SCOPE_BASE, "(objectclass=*)",
                       (எழுத்து **)CURL_UNCONST(supportedSASLMechanisms), 0,
                       வெற்று, வெற்று, வெற்று, 0, &li->msgid);
  எனில்(rc != LDAP_SUCCESS)
    திரும்பு oldap_map_error(rc, CURLE_LOGIN_DENIED);
  oldap_state(data, li, OLDAP_MECHS);
  திரும்பு CURLE_OK;
}

/* Starts SASL bind. */
நிலையான CURLcode oldap_perform_sasl(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  saslprogress progress = SASL_IDLE;
  CURLcode result;

  எனில்(!li)
    திரும்பு CURLE_FAILED_INIT;
  result = Curl_sasl_start(&li->sasl, data, TRUE, &progress);

  oldap_state(data, li, OLDAP_SASL);
  எனில்(!result && progress != SASL_INPROGRESS)
    result = Curl_sasl_is_blocked(&li->sasl, data);
  திரும்பு result;
}

#ifdef USE_SSL
நிலையான முழு ldapsb_tls_setup(Sockbuf_IO_Desc *sbiod, வெறுமை *arg)
{
  sbiod->sbiod_pvt = arg;
  திரும்பு 0;
}

நிலையான முழு ldapsb_tls_remove(Sockbuf_IO_Desc *sbiod)
{
  sbiod->sbiod_pvt = வெற்று;
  திரும்பு 0;
}

/* if there is a duplicated socket to close, close it */
நிலையான முழு ldapsb_tls_close(Sockbuf_IO_Desc *sbiod)
{
  கட்டமைப்பு Curl_easy *data = sbiod->sbiod_pvt;
  எனில்(data) {
    கட்டமைப்பு connectdata *conn = data->conn;
    எனில்(conn) {
      கட்டமைப்பு ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      எனில்(li && (li->dupfd != CURL_SOCKET_BAD)) {
        sclose(li->dupfd);
        li->dupfd = CURL_SOCKET_BAD;
      }
    }
  }

  திரும்பு 0;
}

நிலையான முழு ldapsb_tls_ctrl(Sockbuf_IO_Desc *sbiod, முழு opt, வெறுமை *arg)
{
  (வெறுமை)arg;
  எனில்(opt == LBER_SB_OPT_DATA_READY) {
    கட்டமைப்பு Curl_easy *data = sbiod->sbiod_pvt;
    திரும்பு Curl_conn_data_pending(data, FIRSTSOCKET);
  }
  திரும்பு 0;
}

நிலையான ber_slen_t ldapsb_tls_read(Sockbuf_IO_Desc *sbiod, வெறுமை *buf,
                                  ber_len_t len)
{
  கட்டமைப்பு Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  எனில்(data) {
    கட்டமைப்பு connectdata *conn = data->conn;
    எனில்(conn) {
      கட்டமைப்பு ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_RECV_ERROR;
      size_t nread;

      எனில்(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        திரும்பு -1;
      }
      result = (li->recv)(data, FIRSTSOCKET, buf, len, &nread);
      எனில்(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nread;
    }
  }
  திரும்பு ret;
}

நிலையான ber_slen_t ldapsb_tls_write(Sockbuf_IO_Desc *sbiod, வெறுமை *buf,
                                   ber_len_t len)
{
  கட்டமைப்பு Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  எனில்(data) {
    கட்டமைப்பு connectdata *conn = data->conn;
    எனில்(conn) {
      கட்டமைப்பு ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_SEND_ERROR;
      size_t nwritten;

      எனில்(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        திரும்பு -1;
      }
      result = (li->send)(data, FIRSTSOCKET, buf, len, FALSE, &nwritten);
      எனில்(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nwritten;
    }
  }
  திரும்பு ret;
}

நிலையான Sockbuf_IO ldapsb_tls = {
  ldapsb_tls_setup,
  ldapsb_tls_remove,
  ldapsb_tls_ctrl,
  ldapsb_tls_read,
  ldapsb_tls_write,
  ldapsb_tls_close
};

நிலையான bool ssl_installed(கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  திரும்பு li && li->recv;
}

நிலையான CURLcode oldap_ssl_connect(கட்டமைப்பு Curl_easy *data, ldapstate newstate)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  bool ssldone = FALSE;
  CURLcode result;

  எனில்(!li)
    திரும்பு CURLE_FAILED_INIT;
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  எனில்(result)
    திரும்பு result;
  oldap_state(data, li, newstate);

  எனில்(ssldone) {
    Sockbuf *sb;

    /* Install the libcurl SSL handlers into the sockbuf. */
    எனில்((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
      திரும்பு CURLE_FAILED_INIT;
    li->recv = conn->recv[FIRSTSOCKET];
    li->send = conn->send[FIRSTSOCKET];
  }

  திரும்பு result;
}

/* Send the STARTTLS request */
நிலையான CURLcode oldap_perform_starttls(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  முழு rc;

  எனில்(!li)
    திரும்பு CURLE_FAILED_INIT;
  rc = ldap_start_tls(li->ld, வெற்று, வெற்று, &li->msgid);
  எனில்(rc != LDAP_SUCCESS)
    திரும்பு oldap_map_error(rc, CURLE_USE_SSL_FAILED);
  oldap_state(data, li, OLDAP_STARTTLS);
  திரும்பு CURLE_OK;
}
#endif

நிலையான வெறுமை oldap_easy_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு ldapreqinfo *lr = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  curlx_free(lr);
}

நிலையான வெறுமை oldap_conn_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு ldapconninfo *li = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  எனில்(li->ld) {
    ldap_unbind_ext(li->ld, வெற்று, வெற்று);
    li->ld = வெற்று;
  }
  curlx_free(li);
}

/* SASL parameters for the ldap protocol */
நிலையான மாறிலி கட்டமைப்பு SASLproto saslldap = {
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

நிலையான CURLcode oldap_connect(கட்டமைப்பு Curl_easy *data, bool *done)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ldapconninfo *li;
  நிலையான மாறிலி முழு version = LDAP_VERSION3;
  எழுத்து *hosturl = வெற்று;
  CURLcode result;
  முழு rc;
#ifdef CURL_OPENLDAP_DEBUG
  நிலையான முழு do_trace = -1;
#endif

  (வெறுமை)done;

  li = curlx_calloc(1, அளவு(கட்டமைப்பு ldapconninfo));
  எனில்(!li) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  li->dupfd = CURL_SOCKET_BAD;

  result = Curl_conn_meta_set(conn, CURL_META_LDAP_CONN, li, oldap_conn_dtor);
  எனில்(result)
    செல் out;

  li->proto = ldap_pvt_url_scheme2proto(data->state.origin->scheme->name);

  /* Initialize the SASL storage */
  Curl_sasl_init(&li->sasl, data, &saslldap);

  result = oldap_parse_login_options(conn);
  எனில்(result)
    செல் out;

  hosturl = curl_maprintf("%s://%s:%u",
                          conn->scheme->name,
                          conn->origin->ipv6 ?
                          conn->origin->user_hostname :
                          conn->origin->hostname,
                          conn->origin->port);
  எனில்(!hosturl) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  {
#ifdef _WIN32
    SOCKET dupfd = INVALID_SOCKET;
    WSAPROTOCOL_INFO pi;
    எனில்(WSADuplicateSocket(conn->sock[FIRSTSOCKET], GetCurrentProcessId(),
                          &pi))
      ; /* error */
    இல்லையெனில்
      dupfd = CURL_SOCKET(pi.iAddressFamily, pi.iSocketType, pi.iProtocol);
#இல்லையெனில்
#ifdef F_DUPFD_CLOEXEC
    முழு dupfd = fcntl(conn->sock[FIRSTSOCKET], F_DUPFD_CLOEXEC, 0);
#இல்லையெனில்
    முழு dupfd = dup(conn->sock[FIRSTSOCKET]);
#ifdef HAVE_FCNTL
    எனில்((dupfd != CURL_SOCKET_BAD) &&
       (fcntl(dupfd, F_SETFD, FD_CLOEXEC) < 0)) {
      sclose(dupfd);
      dupfd = CURL_SOCKET_BAD;
    }
#endif /* HAVE_FCNTL */
#endif /* F_DUPFD_CLOEXEC */
#endif /* _WIN32 */

    எனில்(dupfd == CURL_SOCKET_BAD) {
      result = CURLE_COULDNT_CONNECT;
      செல் out;
    }
    rc = ldap_init_fd((ber_socket_t)dupfd, li->proto, hosturl, &li->ld);
    எனில்(rc)
      sclose(dupfd);
    இல்லையெனில்
      /* store the socket, as when this is used by TLS we close this ourselves
         and at this point the code does not yet know if this will use TLS or
         not */
      li->dupfd = dupfd;
  }
  எனில்(rc) {
    failf(data, "LDAP local: Cannot connect to %s, %s",
          hosturl, ldap_err2string(rc));
    result = CURLE_COULDNT_CONNECT;
    செல் out;
  }

#ifdef CURL_OPENLDAP_DEBUG
  எனில்(do_trace < 0) {
    மாறிலி எழுத்து *env = getenv("CURL_OPENLDAP_TRACE");
    curl_off_t e = 0;
    எனில்(!curlx_str_எண்(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  எனில்(do_trace)
    ldap_set_option(li->ld, LDAP_OPT_DEBUG_LEVEL, &do_trace);
#endif

  /* Try version 3 first. */
  ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);

  /* Do not chase referrals. */
  ldap_set_option(li->ld, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  {
    ber_len_t max = 256 * 1024;
    Sockbuf *sb;
    எனில்((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       /* Set the maximum allowed size of an incoming message, which to
          OpenLDAP means that it will malloc() memory up to this size. If not
          set, there is no limit and we instead risk a malloc() failure. */
       !ber_sockbuf_ctrl(sb, LBER_SB_OPT_SET_MAX_INCOMING, &max)) {
      result = CURLE_FAILED_INIT;
      செல் out;
    }
  }

#ifdef USE_SSL
  எனில்(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = oldap_ssl_connect(data, OLDAP_SSL);
    செல் out;
  }

  எனில்(data->set.use_ssl) {
    result = oldap_perform_starttls(data);
    எனில்(!result || data->set.use_ssl != CURLUSESSL_TRY)
      செல் out;
  }
#endif

  எனில்(li->sasl.prefmech != SASL_AUTH_NONE) {
    result = oldap_perform_mechs(data);
    செல் out;
  }

  /* Force bind even if anonymous bind is not needed in protocol version 3
     to detect missing version 3 support. */
  result = oldap_perform_bind(data, OLDAP_BIND);

out:
  curlx_free(hosturl);
  திரும்பு result;
}

/* Handle the supported SASL mechanisms query response */
நிலையான CURLcode oldap_state_mechs_resp(கட்டமைப்பு Curl_easy *data,
                                       LDAPMessage *செய்தி, முழு code)
{
  கட்டமைப்பு connectdata *conn;
  கட்டமைப்பு ldapconninfo *li;
  முழு rc;
  BerElement *ber = வெற்று;
  CURLcode result = CURLE_OK;
  கட்டமைப்பு berval bv, *bvals;

  எனில்(!data)
    திரும்பு CURLE_FAILED_INIT;

  conn = data->conn;
  li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);

  எனில்(!li)
    திரும்பு CURLE_FAILED_INIT;
  தேர்வு(ldap_msgtype(செய்தி)) {
  நிலை LDAP_RES_தேடல்_ENTRY:
    /* Got a list of supported SASL mechanisms. */
    எனில்(code != LDAP_SUCCESS && code != LDAP_NO_RESULTS_RETURNED)
      திரும்பு CURLE_LOGIN_DENIED;

    rc = ldap_get_dn_ber(li->ld, செய்தி, &ber, &bv);
    எனில்(rc < 0)
      திரும்பு oldap_map_error(rc, CURLE_BAD_CONTENT_ENCODING);
    க்கு(rc = ldap_get_attribute_ber(li->ld, செய்தி, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, செய்தி, ber, &bv, &bvals)) {
      முழு i;

      எனில்(!bv.bv_val)
        நிறுத்து;

      எனில்(bvals) {
        க்கு(i = 0; bvals[i].bv_val; i++) {
          size_t llen;
          unsigned short mech =
            Curl_sasl_decode_mech((மாறிலி எழுத்து *)bvals[i].bv_val,
                                  bvals[i].bv_len, &llen);
          எனில்(bvals[i].bv_len == llen)
            li->sasl.authmechs |= mech;
        }
        ber_memfree(bvals);
      }
    }
    ber_free(ber, 0);
    நிறுத்து;

  நிலை LDAP_RES_தேடல்_RESULT:
    தேர்வு(code) {
    நிலை LDAP_SIZELIMIT_EXCEEDED:
      infof(data, "Too many authentication mechanisms");
      FALLTHROUGH();
    நிலை LDAP_SUCCESS:
    நிலை LDAP_NO_RESULTS_RETURNED:
      எனில்(Curl_sasl_can_authenticate(&li->sasl, data))
        result = oldap_perform_sasl(data);
      இல்லையெனில்
        result = CURLE_LOGIN_DENIED;
      நிறுத்து;
    இயல்பு:
      result = oldap_map_error(code, CURLE_LOGIN_DENIED);
      நிறுத்து;
    }
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு result;
}

/* Handle a SASL bind response. */
நிலையான CURLcode oldap_state_sasl_resp(கட்டமைப்பு Curl_easy *data,
                                      LDAPMessage *செய்தி, முழு code)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode result = CURLE_OK;
  saslprogress progress;
  முழு rc;

  எனில்(!li)
    திரும்பு CURLE_FAILED_INIT;
  li->servercred = வெற்று;
  rc = ldap_parse_sasl_bind_result(li->ld, செய்தி, &li->servercred, 0);
  எனில்(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: sasl ldap_parse_result %s", ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LOGIN_DENIED);
  }
  இல்லையெனில் {
    result = Curl_sasl_continue(&li->sasl, data, code, &progress);
    எனில்(!result) {
      தேர்வு(progress) {
      நிலை SASL_DONE:
        oldap_state(data, li, OLDAP_STOP);   /* Authenticated */
        நிறுத்து;
      நிலை SASL_IDLE:            /* No mechanism left after cancellation */
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
        நிறுத்து;
      இயல்பு:
        நிறுத்து;
      }
    }
  }

  எனில்(li->servercred)
    ber_bvfree(li->servercred);
  திரும்பு result;
}

/* Handle a simple bind response. */
நிலையான CURLcode oldap_state_bind_resp(கட்டமைப்பு Curl_easy *data, LDAPMessage *செய்தி,
                                      முழு code)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode result = CURLE_OK;
  கட்டமைப்பு berval *bv = வெற்று;
  முழு rc;

  எனில்(!li)
    திரும்பு CURLE_FAILED_INIT;

  எனில்(code != LDAP_SUCCESS)
    திரும்பு oldap_map_error(code, CURLE_LDAP_CANNOT_BIND);

  rc = ldap_parse_sasl_bind_result(li->ld, செய்தி, &bv, 0);
  எனில்(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: bind ldap_parse_sasl_bind_result %s",
          ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  }
  இல்லையெனில்
    oldap_state(data, li, OLDAP_STOP);

  எனில்(bv)
    ber_bvfree(bv);
  திரும்பு result;
}

நிலையான CURLcode oldap_connecting(கட்டமைப்பு Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  LDAPMessage *செய்தி = வெற்று;
  கட்டமைப்பு timeval tv = { 0, 0 };
  முழு code = LDAP_SUCCESS;
  முழு rc;

  எனில்(!li)
    திரும்பு CURLE_FAILED_INIT;

  எனில்(li->state != OLDAP_SSL && li->state != OLDAP_TLS) {
    /* Get response to last command. */
    rc = ldap_result(li->ld, li->msgid, LDAP_செய்தி_ONE, &tv, &செய்தி);
    தேர்வு(rc) {
    நிலை 0:                               /* Timed out. */
      திரும்பு CURLE_OK;
    நிலை LDAP_RES_தேடல்_ENTRY:
    நிலை LDAP_RES_தேடல்_REFERENCE:
      நிறுத்து;
    இயல்பு:
      li->msgid = 0;                      /* Nothing to abandon upon error. */
      எனில்(rc < 0) {
        failf(data, "LDAP local: connecting ldap_result %s",
              ldap_err2string(rc));
        திரும்பு oldap_map_error(rc, CURLE_COULDNT_CONNECT);
      }
      நிறுத்து;
    }

    /* Get error code from message. */
    rc = ldap_parse_result(li->ld, செய்தி, &code, வெற்று, வெற்று, வெற்று, வெற்று, 0);
    எனில்(rc)
      code = rc;
    இல்லையெனில் {
      /* store the latest code for later retrieval */
      data->info.httpcode = code;
    }

    /* If protocol version 3 is not supported, fallback to version 2. */
    எனில்(code == LDAP_PROTOCOL_ERROR && li->state != OLDAP_BINDV2 &&
#ifdef USE_SSL
       (ssl_installed(conn) || data->set.use_ssl <= CURLUSESSL_TRY) &&
#endif
       li->sasl.prefmech == SASL_AUTH_NONE) {
      நிலையான மாறிலி முழு version = LDAP_VERSION2;

      ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);
      ldap_msgfree(செய்தி);
      திரும்பு oldap_perform_bind(data, OLDAP_BINDV2);
    }
  }

  /* Handle response message according to current state. */
  தேர்வு(li->state) {

#ifdef USE_SSL
  நிலை OLDAP_SSL:
    result = oldap_ssl_connect(data, OLDAP_SSL);
    எனில்(!result && ssl_installed(conn)) {
      எனில்(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      இல்லையெனில்
        result = oldap_perform_bind(data, OLDAP_BIND);
    }
    நிறுத்து;
  நிலை OLDAP_STARTTLS:
    எனில்(code != LDAP_SUCCESS) {
      எனில்(data->set.use_ssl != CURLUSESSL_TRY)
        result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
      இல்லையெனில் எனில்(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      இல்லையெனில்
        result = oldap_perform_bind(data, OLDAP_BIND);
      நிறுத்து;
    }
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    எனில்(result)
      நிறுத்து;
    FALLTHROUGH();
  நிலை OLDAP_TLS:
    result = oldap_ssl_connect(data, OLDAP_TLS);
    எனில்(result)
      result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
    இல்லையெனில் எனில்(ssl_installed(conn)) {
      எனில்(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      இல்லையெனில் எனில்(data->state.creds)
        result = oldap_perform_bind(data, OLDAP_BIND);
      இல்லையெனில் {
        /* Version 3 supported: no bind required */
        oldap_state(data, li, OLDAP_STOP);
        result = CURLE_OK;
      }
    }
    நிறுத்து;
#endif

  நிலை OLDAP_MECHS:
    result = oldap_state_mechs_resp(data, செய்தி, code);
    நிறுத்து;
  நிலை OLDAP_SASL:
    result = oldap_state_sasl_resp(data, செய்தி, code);
    நிறுத்து;
  நிலை OLDAP_BIND:
  நிலை OLDAP_BINDV2:
    result = oldap_state_bind_resp(data, செய்தி, code);
    நிறுத்து;
  இயல்பு:
    /* internal error */
    result = CURLE_COULDNT_CONNECT;
    நிறுத்து;
  }

  ldap_msgfree(செய்தி);

  *done = li->state == OLDAP_STOP;
  எனில்(*done)
    conn->recv[FIRSTSOCKET] = oldap_recv;

  எனில்(result && li->msgid) {
    ldap_abandon_ext(li->ld, li->msgid, வெற்று, வெற்று);
    li->msgid = 0;
  }
  திரும்பு result;
}

நிலையான CURLcode oldap_disconnect(கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு connectdata *conn,
                                 bool dead_connection)
{
  கட்டமைப்பு ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  (வெறுமை)dead_connection;
#ifndef USE_SSL
  (வெறுமை)data;
#endif

  எனில்(li && li->ld) {
#ifdef USE_SSL
    எனில்(ssl_installed(conn)) {
      Sockbuf *sb;
      எனில்(ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS ||
         ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
        திரும்பு CURLE_FAILED_INIT;
    }
#endif
    ldap_unbind_ext(li->ld, வெற்று, வெற்று);
    li->ld = வெற்று;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode oldap_do(கட்டமைப்பு Curl_easy *data, bool *done)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  கட்டமைப்பு ldapreqinfo *lr;
  CURLcode result;
  முழு rc;
  LDAPURLDesc *lud;
  முழு msgid;

  எனில்(!li)
    திரும்பு CURLE_FAILED_INIT;

  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

  result = oldap_url_parse(data, &lud);
  எனில்(result)
    செல் out;

#ifdef USE_SSL
  எனில்(ssl_installed(conn)) {
    Sockbuf *sb;
    /* re-install the libcurl SSL handlers into the sockbuf. */
    எனில்((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data)) {
      ldap_free_urldesc(lud);
      திரும்பு CURLE_FAILED_INIT;
    }
  }
#endif

  rc = ldap_தேடல்_ext(li->ld, lud->lud_dn, lud->lud_scope,
                       lud->lud_filter, lud->lud_attrs, 0,
                       வெற்று, வெற்று, வெற்று, 0, &msgid);
  ldap_free_urldesc(lud);
  எனில்(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: ldap_search_ext %s", ldap_err2string(rc));
    result = CURLE_LDAP_தேடல்_FAILED;
    செல் out;
  }

  lr = curlx_calloc(1, அளவு(கட்டமைப்பு ldapreqinfo));
  எனில்(!lr ||
     Curl_meta_set(data, CURL_META_LDAP_EASY, lr, oldap_easy_dtor)) {
    ldap_abandon_ext(li->ld, msgid, வெற்று, வெற்று);
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  lr->msgid = msgid;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  *done = TRUE;

out:
  திரும்பு result;
}

நிலையான CURLcode oldap_done(கட்டமைப்பு Curl_easy *data, CURLcode res,
                           bool premature)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);

  (வெறுமை)res;
  (வெறுமை)premature;

  எனில்(lr) {
    /* if there was a search in progress, abandon it */
    எனில்(lr->msgid) {
      கட்டமைப்பு ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      எனில்(li && li->ld) {
        ldap_abandon_ext(li->ld, lr->msgid, வெற்று, வெற்று);
      }
      lr->msgid = 0;
    }
    Curl_meta_remove(data, CURL_META_LDAP_EASY);
  }

  திரும்பு CURLE_OK;
}

நிலையான CURLcode client_write(கட்டமைப்பு Curl_easy *data,
                             மாறிலி எழுத்து *prefix, size_t plen,
                             மாறிலி எழுத்து *மதிப்பு, size_t len,
                             மாறிலி எழுத்து *suffix, size_t slen)
{
  CURLcode result = CURLE_OK;

  எனில்(prefix) {
    /* If we have a zero-length value and the prefix ends with a space
       separator, drop the latter. */
    எனில்(!len && plen && prefix[plen - 1] == ' ')
      plen--;
    result = Curl_client_write(data, CLIENTWRITE_BODY, prefix, plen);
  }
  எனில்(!result && மதிப்பு) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, மதிப்பு, len);
  }
  எனில்(!result && suffix) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, suffix, slen);
  }
  திரும்பு result;
}

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
நிலையான bool oldap_str_needs_base64(மாறிலி எழுத்து *s, size_t len)
{
  size_t i;
  எனில்(!len)
    திரும்பு FALSE;
  எனில்((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    திரும்பு TRUE;
  க்கு(i = 0; i < len; i++)
    எனில்(!ISPRINT(s[i]))
      திரும்பு TRUE;
  திரும்பு FALSE;
}

நிலையான CURLcode oldap_recv(கட்டமைப்பு Curl_easy *data, int8_t sockindex, எழுத்து *buf,
                           size_t len, size_t *pnread)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  கட்டமைப்பு ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);
  முழு rc;
  LDAPMessage *செய்தி = வெற்று;
  BerElement *ber = வெற்று;
  கட்டமைப்பு timeval tv = { 0, 0 };
  கட்டமைப்பு berval bv, *bvals;
  CURLcode result = CURLE_AGAIN;
  முழு code;
  எழுத்து *info = வெற்று;

  (வெறுமை)len;
  (வெறுமை)buf;
  (வெறுமை)sockindex;
  *pnread = 0;
  எனில்(!li || !lr)
    திரும்பு CURLE_FAILED_INIT;

  rc = ldap_result(li->ld, lr->msgid, LDAP_செய்தி_ONE, &tv, &செய்தி);
  எனில்(rc < 0) {
    failf(data, "LDAP local: search ldap_result %s", ldap_err2string(rc));
    result = CURLE_RECV_ERROR;
  }

  /* error or timed out */
  எனில்(!செய்தி)
    திரும்பு result;

  result = CURLE_OK;

  தேர்வு(ldap_msgtype(செய்தி)) {
  நிலை LDAP_RES_தேடல்_RESULT:
    lr->msgid = 0;
    rc = ldap_parse_result(li->ld, செய்தி, &code, வெற்று, &info, வெற்று, வெற்று, 0);
    எனில்(rc) {
      failf(data, "LDAP local: search ldap_parse_result %s",
            ldap_err2string(rc));
      result = CURLE_LDAP_தேடல்_FAILED;
      நிறுத்து;
    }

    /* store the latest code for later retrieval */
    data->info.httpcode = code;

    தேர்வு(code) {
    நிலை LDAP_SIZELIMIT_EXCEEDED:
      infof(data, "There are more than %d entries", lr->nument);
      FALLTHROUGH();
    நிலை LDAP_SUCCESS:
      data->req.size = data->req.bytecount;
      நிறுத்து;
    இயல்பு:
      failf(data, "LDAP remote: search failed %s %s", ldap_err2string(code),
            info ? info : "");
      result = CURLE_LDAP_தேடல்_FAILED;
      நிறுத்து;
    }
    எனில்(info)
      ldap_memfree(info);
    நிறுத்து;
  நிலை LDAP_RES_தேடல்_ENTRY:
    lr->nument++;
    rc = ldap_get_dn_ber(li->ld, செய்தி, &ber, &bv);
    எனில்(rc < 0) {
      result = CURLE_RECV_ERROR;
      நிறுத்து;
    }

    எனில்(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
      எழுத்து *dn_b64 = வெற்று;
      size_t dn_b64_sz = 0;

      /* A DN with e.g. CR/LF would inject LDIF lines, so base64-encode it. */
      result = curlx_base64_encode((uint8_t *)bv.bv_val, bv.bv_len,
                                   &dn_b64, &dn_b64_sz);
      எனில்(!result)
        result = client_write(data, STRCONST("DN:: "), dn_b64, dn_b64_sz,
                              STRCONST("\n"));
      curlx_free(dn_b64);
    }
    இல்லையெனில்
      result = client_write(data, STRCONST("DN: "), bv.bv_val, bv.bv_len,
                            STRCONST("\n"));
    எனில்(result)
      நிறுத்து;

    க்கு(rc = ldap_get_attribute_ber(li->ld, செய்தி, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, செய்தி, ber, &bv, &bvals)) {
      முழு i;
      bool இருமம்;

      எனில்(!bv.bv_val)
        நிறுத்து;

      /* An attribute type has no base64 form in LDIF, so a type carrying
         control bytes (CR/LF etc.) would let a hostile server inject lines.
         A conformant type name never contains them, so reject it. */
      எனில்(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
        failf(data, "LDAP local: attribute type contains illegal bytes");
        எனில்(bvals)
          ber_memfree(bvals);
        result = CURLE_WEIRD_SERVER_REPLY;
        நிறுத்து;
      }

      எனில்(!bvals) {
        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":\n"));
        எனில்(result)
          நிறுத்து;
        தொடர்;
      }

      இருமம் = bv.bv_len > 7 &&
        curl_strnequal(bv.bv_val + bv.bv_len - 7, ";binary", 7);

      க்கு(i = 0; bvals[i].bv_val; i++) {
        bool binval = FALSE;

        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":"));
        எனில்(result)
          நிறுத்து;

        எனில்(!இருமம்)
          binval = oldap_str_needs_base64(bvals[i].bv_val, bvals[i].bv_len);
        எனில்(இருமம் || binval) {
          எழுத்து *val_b64 = வெற்று;
          size_t val_b64_sz = 0;

          /* Binary value, encode to base64. */
          எனில்(bvals[i].bv_len)
            result = curlx_base64_encode((uint8_t *)bvals[i].bv_val,
                                         bvals[i].bv_len,
                                         &val_b64, &val_b64_sz);
          எனில்(!result)
            result = client_write(data, STRCONST(": "), val_b64, val_b64_sz,
                                  STRCONST("\n"));
          curlx_free(val_b64);
        }
        இல்லையெனில்
          result = client_write(data, STRCONST(" "),
                                bvals[i].bv_val, bvals[i].bv_len,
                                STRCONST("\n"));
        எனில்(result)
          நிறுத்து;
      }

      ber_memfree(bvals);
      bvals = வெற்று;
      எனில்(!result)
        result = client_write(data, STRCONST("\n"), வெற்று, 0, வெற்று, 0);
      எனில்(result)
        நிறுத்து;
    }

    எனில்(!result)
      result = client_write(data, STRCONST("\n"), வெற்று, 0, வெற்று, 0);
    எனில்(!result)
      result = CURLE_AGAIN;
    நிறுத்து;
  }

  ber_free(ber, 0);
  ldap_msgfree(செய்தி);
  திரும்பு result;
}

வெறுமை Curl_ldap_version(எழுத்து *buf, size_t bufsz)
{
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  எனில்(ldap_get_option(வெற்று, LDAP_OPT_API_INFO, &api) == LDAP_OPT_SUCCESS) {
    unsigned முழு patch = (unsigned முழு)(api.ldapai_vendor_version % 100);
    unsigned முழு major = (unsigned முழு)(api.ldapai_vendor_version / 10000);
    unsigned முழு minor =
      (((unsigned முழு)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u",
                   api.ldapai_vendor_name, major, minor, patch);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((வெறுமை **)api.ldapai_extensions);
  }
  இல்லையெனில்
    curl_msnprintf(buf, bufsz, "OpenLDAP");
}

/*
 * LDAP protocol handler.
 */
மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_ldap = {
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
