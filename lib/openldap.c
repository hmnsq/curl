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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP)

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

#અન્તર્ભાવ <ldap.h>

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "curl_ldap.h"
#અન્તર્ભાવ "curlx/base64.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "curl_sasl.h"
#અન્તર્ભાવ "strcase.h"
#અન્તર્ભાવ "bufref.h"

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
પ્રકારવ્યાખ્યા ગણના {
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
બાહ્ય પૂર્ણાંક ldap_pvt_url_scheme2proto(અચલ અક્ષર *);
બાહ્ય પૂર્ણાંક ldap_init_fd(ber_socket_t fd, પૂર્ણાંક proto, અચલ અક્ષર *url,
                        LDAP **ld);
#endif

સ્થિર Curl_recv oldap_recv;

સંરચના ldapconninfo {
  સંરચના SASL sasl;          /* SASL-related parameters */
  LDAP *ld;                  /* Openldap connection handle. */
  Curl_recv *recv;           /* For stacking SSL handler */
  Curl_send *send;
  સંરચના berval *servercred; /* SASL data from server. */
  ldapstate state;           /* Current machine state. */
  curl_socket_t dupfd;       /* duplicated socket */
  પૂર્ણાંક proto;                 /* LDAP_PROTO_TCP/LDAP_PROTO_UDP/LDAP_PROTO_IPC */
  પૂર્ણાંક msgid;                 /* Current message id. */
};

સંરચના ldapreqinfo {
  પૂર્ણાંક msgid;
  પૂર્ણાંક nument;
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
સ્થિર શૂન્ય oldap_state(સંરચના Curl_easy *data, સંરચના ldapconninfo *li,
                        ldapstate newstate)
{
#જો defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  સ્થિર અચલ અક્ષર * અચલ names[] = {
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

  જો(li->state != newstate)
    infof(data, "LDAP %p state change from %s to %s",
          (શૂન્ય *)li, names[li->state], names[newstate]);
#નહીંતર
  (શૂન્ય)data;
#endif
  li->state = newstate;
}

/* Map some particular LDAP error codes to CURLcode values. */
સ્થિર CURLcode oldap_map_error(પૂર્ણાંક rc, CURLcode result)
{
  પસંદગી(rc) {
  સ્થિતિ LDAP_NO_MEMORY:
    પાછા CURLE_OUT_OF_MEMORY;
  સ્થિતિ LDAP_INVALID_CREDENTIALS:
    પાછા CURLE_LOGIN_DENIED;
  સ્થિતિ LDAP_PROTOCOL_ERROR:
    પાછા CURLE_UNSUPPORTED_PROTOCOL;
  સ્થિતિ LDAP_INSUFFICIENT_ACCESS:
    પાછા CURLE_REMOTE_ACCESS_DENIED;
  }
  પાછા result;
}

સ્થિર CURLcode oldap_url_parse(સંરચના Curl_easy *data, LDAPURLDesc **ludp)
{
  CURLcode result = CURLE_OK;
  પૂર્ણાંક rc;
  સ્થિર અચલ અક્ષર * અચલ url_errs[] = {
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

  *ludp = ખાલી;
  /* `ldap_url_parse() seems to be terrible with urls
   * that have user/pass/options in it. So when we have options or
   * creds from the url, fail without calling the function.
   * Yes, this is super-weird code and I do not like it. */
  જો((data->state.creds && (data->state.creds->source == CREDS_URL)) ||
     data->state.up.options)
    rc = LDAP_URL_ERR_BADURL;
  નહીંતર
    rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), ludp);

  જો(rc != LDAP_URL_SUCCESS) {
    અચલ અક્ષર *સંદેશ = "url parsing problem";

    result = rc == LDAP_URL_ERR_MEM ? CURLE_OUT_OF_MEMORY :
      CURLE_URL_MALFORMAT;
    rc -= LDAP_URL_SUCCESS;
    જો((size_t)rc < CURL_ARRAYSIZE(url_errs))
      સંદેશ = url_errs[rc];
    failf(data, "LDAP local: %s", સંદેશ);
  }
  પાછા result;
}

/* Parse the login options. */
સ્થિર CURLcode oldap_parse_login_options(સંરચના connectdata *conn)
{
  CURLcode result = CURLE_OK;
  સંરચના ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  અચલ અક્ષર *ptr = conn->options;

  DEBUGASSERT(li);
  જો(!li)
    પાછા CURLE_FAILED_INIT;

  જ્યારે(!result && ptr && *ptr) {
    અચલ અક્ષર *ચાવી = ptr;
    અચલ અક્ષર *કિંમત;

    જ્યારે(*ptr && *ptr != '=')
      ptr++;

    કિંમત = ptr + 1;

    જ્યારે(*ptr && *ptr != ';')
      ptr++;

    જો(checkprefix("AUTH=", ચાવી))
      result = Curl_sasl_parse_url_auth_option(&li->sasl, કિંમત, ptr - કિંમત);
    નહીંતર
      result = CURLE_SETOPT_OPTION_SYNTAX;

    જો(*ptr == ';')
      ptr++;
  }

  પાછા result == CURLE_URL_MALFORMAT ? CURLE_SETOPT_OPTION_SYNTAX : result;
}

સ્થિર CURLcode oldap_setup_connection(સંરચના Curl_easy *data,
                                       સંરચના connectdata *conn)
{
  CURLcode result;
  LDAPURLDesc *lud;
  (શૂન્ય)conn;

  /* Early URL syntax check. */
  result = oldap_url_parse(data, &lud);
  ldap_free_urldesc(lud);

  પાછા result;
}

/*
 * Get the SASL authentication challenge from the server credential buffer.
 */
સ્થિર CURLcode oldap_get_message(સંરચના Curl_easy *data, સંરચના bufref *out)
{
  સંરચના ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  સંરચના berval *servercred = li ? li->servercred : ખાલી;
  DEBUGASSERT(li);
  જો(!li)
    પાછા CURLE_FAILED_INIT;

  જો(!servercred || !servercred->bv_val)
    પાછા CURLE_WEIRD_SERVER_REPLY;
  Curl_bufref_set(out, servercred->bv_val, servercred->bv_len, ખાલી);
  પાછા CURLE_OK;
}

/*
 * Sends an initial SASL bind request to the server.
 */
સ્થિર CURLcode oldap_perform_auth(સંરચના Curl_easy *data, અચલ અક્ષર *mech,
                                   અચલ સંરચના bufref *initresp)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  સંરચના berval cred;
  સંરચના berval *pcred = &cred;
  પૂર્ણાંક rc;

  DEBUGASSERT(li);
  જો(!li)
    પાછા CURLE_FAILED_INIT;
  cred.bv_val = (અક્ષર *)CURL_UNCONST(Curl_bufref_ptr(initresp));
  cred.bv_len = Curl_bufref_len(initresp);
  જો(!cred.bv_val)
    pcred = ખાલી;
  rc = ldap_sasl_bind(li->ld, ખાલી, mech, pcred, ખાલી, ખાલી, &li->msgid);
  જો(rc != LDAP_SUCCESS)
    પાછા oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  પાછા CURLE_OK;
}

/*
 * Sends SASL continuation.
 */
સ્થિર CURLcode oldap_continue_auth(સંરચના Curl_easy *data, અચલ અક્ષર *mech,
                                    અચલ સંરચના bufref *resp)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  સંરચના berval cred;
  સંરચના berval *pcred = &cred;
  પૂર્ણાંક rc;

  જો(!li)
    પાછા CURLE_FAILED_INIT;
  cred.bv_val = (અક્ષર *)CURL_UNCONST(Curl_bufref_ptr(resp));
  cred.bv_len = Curl_bufref_len(resp);
  જો(!cred.bv_val)
    pcred = ખાલી;
  rc = ldap_sasl_bind(li->ld, ખાલી, mech, pcred, ખાલી, ખાલી, &li->msgid);
  જો(rc != LDAP_SUCCESS)
    પાછા oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  પાછા CURLE_OK;
}

/*
 * Sends SASL bind cancellation.
 */
સ્થિર CURLcode oldap_cancel_auth(સંરચના Curl_easy *data, અચલ અક્ષર *mech)
{
  સંરચના ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  પૂર્ણાંક rc;

  (શૂન્ય)mech;
  જો(!li)
    પાછા CURLE_FAILED_INIT;
  rc = ldap_sasl_bind(li->ld, ખાલી, LDAP_SASL_NULL, ખાલી, ખાલી, ખાલી,
                      &li->msgid);
  જો(rc != LDAP_SUCCESS)
    પાછા oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  પાછા CURLE_OK;
}

/* Starts LDAP simple bind. */
સ્થિર CURLcode oldap_perform_bind(સંરચના Curl_easy *data, ldapstate newstate)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  અચલ અક્ષર *binddn = ખાલી;
  સંરચના berval passwd;
  પૂર્ણાંક rc;

  જો(!li)
    પાછા CURLE_FAILED_INIT;
  passwd.bv_val = ખાલી;
  passwd.bv_len = 0;

  જો(conn->creds) {
    binddn = Curl_creds_user(conn->creds);
    passwd.bv_val = CURL_UNCONST(Curl_creds_passwd(conn->creds));
    passwd.bv_len = strlen(passwd.bv_val);
  }

  rc = ldap_sasl_bind(li->ld, binddn, LDAP_SASL_SIMPLE, &passwd,
                      ખાલી, ખાલી, &li->msgid);
  જો(rc != LDAP_SUCCESS)
    પાછા oldap_map_error(rc,
                           data->state.creds ?
                           CURLE_LOGIN_DENIED : CURLE_LDAP_CANNOT_BIND);
  oldap_state(data, li, newstate);
  પાછા CURLE_OK;
}

/* Query the supported SASL authentication mechanisms. */
સ્થિર CURLcode oldap_perform_mechs(સંરચના Curl_easy *data)
{
  સંરચના ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  પૂર્ણાંક rc;
  સ્થિર અચલ અક્ષર * અચલ supportedSASLMechanisms[] = {
    "supportedSASLMechanisms",
    ખાલી
  };

  જો(!li)
    પાછા CURLE_FAILED_INIT;
  /* Casting away the const for the 3rd parameter that the LDAP API expects as
     a non-const char ** is potentially unsafe but we believe the lack of
     const in the API was an oversight and that no LDAP implementation
     actually modifies the input. */
  rc = ldap_શોધ_ext(li->ld, "", LDAP_SCOPE_BASE, "(objectclass=*)",
                       (અક્ષર **)CURL_UNCONST(supportedSASLMechanisms), 0,
                       ખાલી, ખાલી, ખાલી, 0, &li->msgid);
  જો(rc != LDAP_SUCCESS)
    પાછા oldap_map_error(rc, CURLE_LOGIN_DENIED);
  oldap_state(data, li, OLDAP_MECHS);
  પાછા CURLE_OK;
}

/* Starts SASL bind. */
સ્થિર CURLcode oldap_perform_sasl(સંરચના Curl_easy *data)
{
  સંરચના ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  saslprogress progress = SASL_IDLE;
  CURLcode result;

  જો(!li)
    પાછા CURLE_FAILED_INIT;
  result = Curl_sasl_start(&li->sasl, data, TRUE, &progress);

  oldap_state(data, li, OLDAP_SASL);
  જો(!result && progress != SASL_INPROGRESS)
    result = Curl_sasl_is_blocked(&li->sasl, data);
  પાછા result;
}

#ifdef USE_SSL
સ્થિર પૂર્ણાંક ldapsb_tls_setup(Sockbuf_IO_Desc *sbiod, શૂન્ય *arg)
{
  sbiod->sbiod_pvt = arg;
  પાછા 0;
}

સ્થિર પૂર્ણાંક ldapsb_tls_remove(Sockbuf_IO_Desc *sbiod)
{
  sbiod->sbiod_pvt = ખાલી;
  પાછા 0;
}

/* if there is a duplicated socket to close, close it */
સ્થિર પૂર્ણાંક ldapsb_tls_close(Sockbuf_IO_Desc *sbiod)
{
  સંરચના Curl_easy *data = sbiod->sbiod_pvt;
  જો(data) {
    સંરચના connectdata *conn = data->conn;
    જો(conn) {
      સંરચના ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      જો(li && (li->dupfd != CURL_SOCKET_BAD)) {
        sclose(li->dupfd);
        li->dupfd = CURL_SOCKET_BAD;
      }
    }
  }

  પાછા 0;
}

સ્થિર પૂર્ણાંક ldapsb_tls_ctrl(Sockbuf_IO_Desc *sbiod, પૂર્ણાંક opt, શૂન્ય *arg)
{
  (શૂન્ય)arg;
  જો(opt == LBER_SB_OPT_DATA_READY) {
    સંરચના Curl_easy *data = sbiod->sbiod_pvt;
    પાછા Curl_conn_data_pending(data, FIRSTSOCKET);
  }
  પાછા 0;
}

સ્થિર ber_slen_t ldapsb_tls_read(Sockbuf_IO_Desc *sbiod, શૂન્ય *buf,
                                  ber_len_t len)
{
  સંરચના Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  જો(data) {
    સંરચના connectdata *conn = data->conn;
    જો(conn) {
      સંરચના ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_RECV_ERROR;
      size_t nread;

      જો(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        પાછા -1;
      }
      result = (li->recv)(data, FIRSTSOCKET, buf, len, &nread);
      જો(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nread;
    }
  }
  પાછા ret;
}

સ્થિર ber_slen_t ldapsb_tls_write(Sockbuf_IO_Desc *sbiod, શૂન્ય *buf,
                                   ber_len_t len)
{
  સંરચના Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  જો(data) {
    સંરચના connectdata *conn = data->conn;
    જો(conn) {
      સંરચના ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_SEND_ERROR;
      size_t nwritten;

      જો(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        પાછા -1;
      }
      result = (li->send)(data, FIRSTSOCKET, buf, len, FALSE, &nwritten);
      જો(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nwritten;
    }
  }
  પાછા ret;
}

સ્થિર Sockbuf_IO ldapsb_tls = {
  ldapsb_tls_setup,
  ldapsb_tls_remove,
  ldapsb_tls_ctrl,
  ldapsb_tls_read,
  ldapsb_tls_write,
  ldapsb_tls_close
};

સ્થિર bool ssl_installed(સંરચના connectdata *conn)
{
  સંરચના ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  પાછા li && li->recv;
}

સ્થિર CURLcode oldap_ssl_connect(સંરચના Curl_easy *data, ldapstate newstate)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  bool ssldone = FALSE;
  CURLcode result;

  જો(!li)
    પાછા CURLE_FAILED_INIT;
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  જો(result)
    પાછા result;
  oldap_state(data, li, newstate);

  જો(ssldone) {
    Sockbuf *sb;

    /* Install the libcurl SSL handlers into the sockbuf. */
    જો((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
      પાછા CURLE_FAILED_INIT;
    li->recv = conn->recv[FIRSTSOCKET];
    li->send = conn->send[FIRSTSOCKET];
  }

  પાછા result;
}

/* Send the STARTTLS request */
સ્થિર CURLcode oldap_perform_starttls(સંરચના Curl_easy *data)
{
  સંરચના ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  પૂર્ણાંક rc;

  જો(!li)
    પાછા CURLE_FAILED_INIT;
  rc = ldap_start_tls(li->ld, ખાલી, ખાલી, &li->msgid);
  જો(rc != LDAP_SUCCESS)
    પાછા oldap_map_error(rc, CURLE_USE_SSL_FAILED);
  oldap_state(data, li, OLDAP_STARTTLS);
  પાછા CURLE_OK;
}
#endif

સ્થિર શૂન્ય oldap_easy_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના ldapreqinfo *lr = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  curlx_free(lr);
}

સ્થિર શૂન્ય oldap_conn_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના ldapconninfo *li = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  જો(li->ld) {
    ldap_unbind_ext(li->ld, ખાલી, ખાલી);
    li->ld = ખાલી;
  }
  curlx_free(li);
}

/* SASL parameters for the ldap protocol */
સ્થિર અચલ સંરચના SASLproto saslldap = {
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

સ્થિર CURLcode oldap_connect(સંરચના Curl_easy *data, bool *done)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ldapconninfo *li;
  સ્થિર અચલ પૂર્ણાંક version = LDAP_VERSION3;
  અક્ષર *hosturl = ખાલી;
  CURLcode result;
  પૂર્ણાંક rc;
#ifdef CURL_OPENLDAP_DEBUG
  સ્થિર પૂર્ણાંક do_trace = -1;
#endif

  (શૂન્ય)done;

  li = curlx_calloc(1, કદ(સંરચના ldapconninfo));
  જો(!li) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  li->dupfd = CURL_SOCKET_BAD;

  result = Curl_conn_meta_set(conn, CURL_META_LDAP_CONN, li, oldap_conn_dtor);
  જો(result)
    જાઓ out;

  li->proto = ldap_pvt_url_scheme2proto(data->state.origin->scheme->name);

  /* Initialize the SASL storage */
  Curl_sasl_init(&li->sasl, data, &saslldap);

  result = oldap_parse_login_options(conn);
  જો(result)
    જાઓ out;

  hosturl = curl_maprintf("%s://%s:%u",
                          conn->scheme->name,
                          conn->origin->ipv6 ?
                          conn->origin->user_hostname :
                          conn->origin->hostname,
                          conn->origin->port);
  જો(!hosturl) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  {
#ifdef _WIN32
    SOCKET dupfd = INVALID_SOCKET;
    WSAPROTOCOL_INFO pi;
    જો(WSADuplicateSocket(conn->sock[FIRSTSOCKET], GetCurrentProcessId(),
                          &pi))
      ; /* error */
    નહીંતર
      dupfd = CURL_SOCKET(pi.iAddressFamily, pi.iSocketType, pi.iProtocol);
#નહીંતર
#ifdef F_DUPFD_CLOEXEC
    પૂર્ણાંક dupfd = fcntl(conn->sock[FIRSTSOCKET], F_DUPFD_CLOEXEC, 0);
#નહીંતર
    પૂર્ણાંક dupfd = dup(conn->sock[FIRSTSOCKET]);
#ifdef HAVE_FCNTL
    જો((dupfd != CURL_SOCKET_BAD) &&
       (fcntl(dupfd, F_SETFD, FD_CLOEXEC) < 0)) {
      sclose(dupfd);
      dupfd = CURL_SOCKET_BAD;
    }
#endif /* HAVE_FCNTL */
#endif /* F_DUPFD_CLOEXEC */
#endif /* _WIN32 */

    જો(dupfd == CURL_SOCKET_BAD) {
      result = CURLE_COULDNT_CONNECT;
      જાઓ out;
    }
    rc = ldap_init_fd((ber_socket_t)dupfd, li->proto, hosturl, &li->ld);
    જો(rc)
      sclose(dupfd);
    નહીંતર
      /* store the socket, as when this is used by TLS we close this ourselves
         and at this point the code does not yet know if this will use TLS or
         not */
      li->dupfd = dupfd;
  }
  જો(rc) {
    failf(data, "LDAP local: Cannot connect to %s, %s",
          hosturl, ldap_err2string(rc));
    result = CURLE_COULDNT_CONNECT;
    જાઓ out;
  }

#ifdef CURL_OPENLDAP_DEBUG
  જો(do_trace < 0) {
    અચલ અક્ષર *env = getenv("CURL_OPENLDAP_TRACE");
    curl_off_t e = 0;
    જો(!curlx_str_સંખ્યા(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  જો(do_trace)
    ldap_set_option(li->ld, LDAP_OPT_DEBUG_LEVEL, &do_trace);
#endif

  /* Try version 3 first. */
  ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);

  /* Do not chase referrals. */
  ldap_set_option(li->ld, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  {
    ber_len_t max = 256 * 1024;
    Sockbuf *sb;
    જો((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       /* Set the maximum allowed size of an incoming message, which to
          OpenLDAP means that it will malloc() memory up to this size. If not
          set, there is no limit and we instead risk a malloc() failure. */
       !ber_sockbuf_ctrl(sb, LBER_SB_OPT_SET_MAX_INCOMING, &max)) {
      result = CURLE_FAILED_INIT;
      જાઓ out;
    }
  }

#ifdef USE_SSL
  જો(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = oldap_ssl_connect(data, OLDAP_SSL);
    જાઓ out;
  }

  જો(data->set.use_ssl) {
    result = oldap_perform_starttls(data);
    જો(!result || data->set.use_ssl != CURLUSESSL_TRY)
      જાઓ out;
  }
#endif

  જો(li->sasl.prefmech != SASL_AUTH_NONE) {
    result = oldap_perform_mechs(data);
    જાઓ out;
  }

  /* Force bind even if anonymous bind is not needed in protocol version 3
     to detect missing version 3 support. */
  result = oldap_perform_bind(data, OLDAP_BIND);

out:
  curlx_free(hosturl);
  પાછા result;
}

/* Handle the supported SASL mechanisms query response */
સ્થિર CURLcode oldap_state_mechs_resp(સંરચના Curl_easy *data,
                                       LDAPMessage *સંદેશ, પૂર્ણાંક code)
{
  સંરચના connectdata *conn;
  સંરચના ldapconninfo *li;
  પૂર્ણાંક rc;
  BerElement *ber = ખાલી;
  CURLcode result = CURLE_OK;
  સંરચના berval bv, *bvals;

  જો(!data)
    પાછા CURLE_FAILED_INIT;

  conn = data->conn;
  li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);

  જો(!li)
    પાછા CURLE_FAILED_INIT;
  પસંદગી(ldap_msgtype(સંદેશ)) {
  સ્થિતિ LDAP_RES_શોધ_ENTRY:
    /* Got a list of supported SASL mechanisms. */
    જો(code != LDAP_SUCCESS && code != LDAP_NO_RESULTS_RETURNED)
      પાછા CURLE_LOGIN_DENIED;

    rc = ldap_get_dn_ber(li->ld, સંદેશ, &ber, &bv);
    જો(rc < 0)
      પાછા oldap_map_error(rc, CURLE_BAD_CONTENT_ENCODING);
    માટે(rc = ldap_get_attribute_ber(li->ld, સંદેશ, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, સંદેશ, ber, &bv, &bvals)) {
      પૂર્ણાંક i;

      જો(!bv.bv_val)
        તોડો;

      જો(bvals) {
        માટે(i = 0; bvals[i].bv_val; i++) {
          size_t llen;
          unsigned short mech =
            Curl_sasl_decode_mech((અચલ અક્ષર *)bvals[i].bv_val,
                                  bvals[i].bv_len, &llen);
          જો(bvals[i].bv_len == llen)
            li->sasl.authmechs |= mech;
        }
        ber_memfree(bvals);
      }
    }
    ber_free(ber, 0);
    તોડો;

  સ્થિતિ LDAP_RES_શોધ_RESULT:
    પસંદગી(code) {
    સ્થિતિ LDAP_SIZELIMIT_EXCEEDED:
      infof(data, "Too many authentication mechanisms");
      FALLTHROUGH();
    સ્થિતિ LDAP_SUCCESS:
    સ્થિતિ LDAP_NO_RESULTS_RETURNED:
      જો(Curl_sasl_can_authenticate(&li->sasl, data))
        result = oldap_perform_sasl(data);
      નહીંતર
        result = CURLE_LOGIN_DENIED;
      તોડો;
    મૂળભૂત:
      result = oldap_map_error(code, CURLE_LOGIN_DENIED);
      તોડો;
    }
    તોડો;
  મૂળભૂત:
    તોડો;
  }
  પાછા result;
}

/* Handle a SASL bind response. */
સ્થિર CURLcode oldap_state_sasl_resp(સંરચના Curl_easy *data,
                                      LDAPMessage *સંદેશ, પૂર્ણાંક code)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode result = CURLE_OK;
  saslprogress progress;
  પૂર્ણાંક rc;

  જો(!li)
    પાછા CURLE_FAILED_INIT;
  li->servercred = ખાલી;
  rc = ldap_parse_sasl_bind_result(li->ld, સંદેશ, &li->servercred, 0);
  જો(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: sasl ldap_parse_result %s", ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LOGIN_DENIED);
  }
  નહીંતર {
    result = Curl_sasl_continue(&li->sasl, data, code, &progress);
    જો(!result) {
      પસંદગી(progress) {
      સ્થિતિ SASL_DONE:
        oldap_state(data, li, OLDAP_STOP);   /* Authenticated */
        તોડો;
      સ્થિતિ SASL_IDLE:            /* No mechanism left after cancellation */
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
        તોડો;
      મૂળભૂત:
        તોડો;
      }
    }
  }

  જો(li->servercred)
    ber_bvfree(li->servercred);
  પાછા result;
}

/* Handle a simple bind response. */
સ્થિર CURLcode oldap_state_bind_resp(સંરચના Curl_easy *data, LDAPMessage *સંદેશ,
                                      પૂર્ણાંક code)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode result = CURLE_OK;
  સંરચના berval *bv = ખાલી;
  પૂર્ણાંક rc;

  જો(!li)
    પાછા CURLE_FAILED_INIT;

  જો(code != LDAP_SUCCESS)
    પાછા oldap_map_error(code, CURLE_LDAP_CANNOT_BIND);

  rc = ldap_parse_sasl_bind_result(li->ld, સંદેશ, &bv, 0);
  જો(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: bind ldap_parse_sasl_bind_result %s",
          ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  }
  નહીંતર
    oldap_state(data, li, OLDAP_STOP);

  જો(bv)
    ber_bvfree(bv);
  પાછા result;
}

સ્થિર CURLcode oldap_connecting(સંરચના Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;
  સંરચના ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  LDAPMessage *સંદેશ = ખાલી;
  સંરચના timeval tv = { 0, 0 };
  પૂર્ણાંક code = LDAP_SUCCESS;
  પૂર્ણાંક rc;

  જો(!li)
    પાછા CURLE_FAILED_INIT;

  જો(li->state != OLDAP_SSL && li->state != OLDAP_TLS) {
    /* Get response to last command. */
    rc = ldap_result(li->ld, li->msgid, LDAP_સંદેશ_ONE, &tv, &સંદેશ);
    પસંદગી(rc) {
    સ્થિતિ 0:                               /* Timed out. */
      પાછા CURLE_OK;
    સ્થિતિ LDAP_RES_શોધ_ENTRY:
    સ્થિતિ LDAP_RES_શોધ_REFERENCE:
      તોડો;
    મૂળભૂત:
      li->msgid = 0;                      /* Nothing to abandon upon error. */
      જો(rc < 0) {
        failf(data, "LDAP local: connecting ldap_result %s",
              ldap_err2string(rc));
        પાછા oldap_map_error(rc, CURLE_COULDNT_CONNECT);
      }
      તોડો;
    }

    /* Get error code from message. */
    rc = ldap_parse_result(li->ld, સંદેશ, &code, ખાલી, ખાલી, ખાલી, ખાલી, 0);
    જો(rc)
      code = rc;
    નહીંતર {
      /* store the latest code for later retrieval */
      data->info.httpcode = code;
    }

    /* If protocol version 3 is not supported, fallback to version 2. */
    જો(code == LDAP_PROTOCOL_ERROR && li->state != OLDAP_BINDV2 &&
#ifdef USE_SSL
       (ssl_installed(conn) || data->set.use_ssl <= CURLUSESSL_TRY) &&
#endif
       li->sasl.prefmech == SASL_AUTH_NONE) {
      સ્થિર અચલ પૂર્ણાંક version = LDAP_VERSION2;

      ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);
      ldap_msgfree(સંદેશ);
      પાછા oldap_perform_bind(data, OLDAP_BINDV2);
    }
  }

  /* Handle response message according to current state. */
  પસંદગી(li->state) {

#ifdef USE_SSL
  સ્થિતિ OLDAP_SSL:
    result = oldap_ssl_connect(data, OLDAP_SSL);
    જો(!result && ssl_installed(conn)) {
      જો(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      નહીંતર
        result = oldap_perform_bind(data, OLDAP_BIND);
    }
    તોડો;
  સ્થિતિ OLDAP_STARTTLS:
    જો(code != LDAP_SUCCESS) {
      જો(data->set.use_ssl != CURLUSESSL_TRY)
        result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
      નહીંતર જો(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      નહીંતર
        result = oldap_perform_bind(data, OLDAP_BIND);
      તોડો;
    }
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    જો(result)
      તોડો;
    FALLTHROUGH();
  સ્થિતિ OLDAP_TLS:
    result = oldap_ssl_connect(data, OLDAP_TLS);
    જો(result)
      result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
    નહીંતર જો(ssl_installed(conn)) {
      જો(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      નહીંતર જો(data->state.creds)
        result = oldap_perform_bind(data, OLDAP_BIND);
      નહીંતર {
        /* Version 3 supported: no bind required */
        oldap_state(data, li, OLDAP_STOP);
        result = CURLE_OK;
      }
    }
    તોડો;
#endif

  સ્થિતિ OLDAP_MECHS:
    result = oldap_state_mechs_resp(data, સંદેશ, code);
    તોડો;
  સ્થિતિ OLDAP_SASL:
    result = oldap_state_sasl_resp(data, સંદેશ, code);
    તોડો;
  સ્થિતિ OLDAP_BIND:
  સ્થિતિ OLDAP_BINDV2:
    result = oldap_state_bind_resp(data, સંદેશ, code);
    તોડો;
  મૂળભૂત:
    /* internal error */
    result = CURLE_COULDNT_CONNECT;
    તોડો;
  }

  ldap_msgfree(સંદેશ);

  *done = li->state == OLDAP_STOP;
  જો(*done)
    conn->recv[FIRSTSOCKET] = oldap_recv;

  જો(result && li->msgid) {
    ldap_abandon_ext(li->ld, li->msgid, ખાલી, ખાલી);
    li->msgid = 0;
  }
  પાછા result;
}

સ્થિર CURLcode oldap_disconnect(સંરચના Curl_easy *data,
                                 સંરચના connectdata *conn,
                                 bool dead_connection)
{
  સંરચના ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  (શૂન્ય)dead_connection;
#ifndef USE_SSL
  (શૂન્ય)data;
#endif

  જો(li && li->ld) {
#ifdef USE_SSL
    જો(ssl_installed(conn)) {
      Sockbuf *sb;
      જો(ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS ||
         ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
        પાછા CURLE_FAILED_INIT;
    }
#endif
    ldap_unbind_ext(li->ld, ખાલી, ખાલી);
    li->ld = ખાલી;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode oldap_do(સંરચના Curl_easy *data, bool *done)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  સંરચના ldapreqinfo *lr;
  CURLcode result;
  પૂર્ણાંક rc;
  LDAPURLDesc *lud;
  પૂર્ણાંક msgid;

  જો(!li)
    પાછા CURLE_FAILED_INIT;

  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

  result = oldap_url_parse(data, &lud);
  જો(result)
    જાઓ out;

#ifdef USE_SSL
  જો(ssl_installed(conn)) {
    Sockbuf *sb;
    /* re-install the libcurl SSL handlers into the sockbuf. */
    જો((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data)) {
      ldap_free_urldesc(lud);
      પાછા CURLE_FAILED_INIT;
    }
  }
#endif

  rc = ldap_શોધ_ext(li->ld, lud->lud_dn, lud->lud_scope,
                       lud->lud_filter, lud->lud_attrs, 0,
                       ખાલી, ખાલી, ખાલી, 0, &msgid);
  ldap_free_urldesc(lud);
  જો(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: ldap_search_ext %s", ldap_err2string(rc));
    result = CURLE_LDAP_શોધ_FAILED;
    જાઓ out;
  }

  lr = curlx_calloc(1, કદ(સંરચના ldapreqinfo));
  જો(!lr ||
     Curl_meta_set(data, CURL_META_LDAP_EASY, lr, oldap_easy_dtor)) {
    ldap_abandon_ext(li->ld, msgid, ખાલી, ખાલી);
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  lr->msgid = msgid;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  *done = TRUE;

out:
  પાછા result;
}

સ્થિર CURLcode oldap_done(સંરચના Curl_easy *data, CURLcode res,
                           bool premature)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);

  (શૂન્ય)res;
  (શૂન્ય)premature;

  જો(lr) {
    /* if there was a search in progress, abandon it */
    જો(lr->msgid) {
      સંરચના ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      જો(li && li->ld) {
        ldap_abandon_ext(li->ld, lr->msgid, ખાલી, ખાલી);
      }
      lr->msgid = 0;
    }
    Curl_meta_remove(data, CURL_META_LDAP_EASY);
  }

  પાછા CURLE_OK;
}

સ્થિર CURLcode client_write(સંરચના Curl_easy *data,
                             અચલ અક્ષર *prefix, size_t plen,
                             અચલ અક્ષર *કિંમત, size_t len,
                             અચલ અક્ષર *suffix, size_t slen)
{
  CURLcode result = CURLE_OK;

  જો(prefix) {
    /* If we have a zero-length value and the prefix ends with a space
       separator, drop the latter. */
    જો(!len && plen && prefix[plen - 1] == ' ')
      plen--;
    result = Curl_client_write(data, CLIENTWRITE_BODY, prefix, plen);
  }
  જો(!result && કિંમત) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, કિંમત, len);
  }
  જો(!result && suffix) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, suffix, slen);
  }
  પાછા result;
}

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
સ્થિર bool oldap_str_needs_base64(અચલ અક્ષર *s, size_t len)
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

સ્થિર CURLcode oldap_recv(સંરચના Curl_easy *data, int8_t sockindex, અક્ષર *buf,
                           size_t len, size_t *pnread)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  સંરચના ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);
  પૂર્ણાંક rc;
  LDAPMessage *સંદેશ = ખાલી;
  BerElement *ber = ખાલી;
  સંરચના timeval tv = { 0, 0 };
  સંરચના berval bv, *bvals;
  CURLcode result = CURLE_AGAIN;
  પૂર્ણાંક code;
  અક્ષર *info = ખાલી;

  (શૂન્ય)len;
  (શૂન્ય)buf;
  (શૂન્ય)sockindex;
  *pnread = 0;
  જો(!li || !lr)
    પાછા CURLE_FAILED_INIT;

  rc = ldap_result(li->ld, lr->msgid, LDAP_સંદેશ_ONE, &tv, &સંદેશ);
  જો(rc < 0) {
    failf(data, "LDAP local: search ldap_result %s", ldap_err2string(rc));
    result = CURLE_RECV_ERROR;
  }

  /* error or timed out */
  જો(!સંદેશ)
    પાછા result;

  result = CURLE_OK;

  પસંદગી(ldap_msgtype(સંદેશ)) {
  સ્થિતિ LDAP_RES_શોધ_RESULT:
    lr->msgid = 0;
    rc = ldap_parse_result(li->ld, સંદેશ, &code, ખાલી, &info, ખાલી, ખાલી, 0);
    જો(rc) {
      failf(data, "LDAP local: search ldap_parse_result %s",
            ldap_err2string(rc));
      result = CURLE_LDAP_શોધ_FAILED;
      તોડો;
    }

    /* store the latest code for later retrieval */
    data->info.httpcode = code;

    પસંદગી(code) {
    સ્થિતિ LDAP_SIZELIMIT_EXCEEDED:
      infof(data, "There are more than %d entries", lr->nument);
      FALLTHROUGH();
    સ્થિતિ LDAP_SUCCESS:
      data->req.size = data->req.bytecount;
      તોડો;
    મૂળભૂત:
      failf(data, "LDAP remote: search failed %s %s", ldap_err2string(code),
            info ? info : "");
      result = CURLE_LDAP_શોધ_FAILED;
      તોડો;
    }
    જો(info)
      ldap_memfree(info);
    તોડો;
  સ્થિતિ LDAP_RES_શોધ_ENTRY:
    lr->nument++;
    rc = ldap_get_dn_ber(li->ld, સંદેશ, &ber, &bv);
    જો(rc < 0) {
      result = CURLE_RECV_ERROR;
      તોડો;
    }

    જો(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
      અક્ષર *dn_b64 = ખાલી;
      size_t dn_b64_sz = 0;

      /* A DN with e.g. CR/LF would inject LDIF lines, so base64-encode it. */
      result = curlx_base64_encode((uint8_t *)bv.bv_val, bv.bv_len,
                                   &dn_b64, &dn_b64_sz);
      જો(!result)
        result = client_write(data, STRCONST("DN:: "), dn_b64, dn_b64_sz,
                              STRCONST("\n"));
      curlx_free(dn_b64);
    }
    નહીંતર
      result = client_write(data, STRCONST("DN: "), bv.bv_val, bv.bv_len,
                            STRCONST("\n"));
    જો(result)
      તોડો;

    માટે(rc = ldap_get_attribute_ber(li->ld, સંદેશ, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, સંદેશ, ber, &bv, &bvals)) {
      પૂર્ણાંક i;
      bool દ્વિઅંકી;

      જો(!bv.bv_val)
        તોડો;

      /* An attribute type has no base64 form in LDIF, so a type carrying
         control bytes (CR/LF etc.) would let a hostile server inject lines.
         A conformant type name never contains them, so reject it. */
      જો(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
        failf(data, "LDAP local: attribute type contains illegal bytes");
        જો(bvals)
          ber_memfree(bvals);
        result = CURLE_WEIRD_SERVER_REPLY;
        તોડો;
      }

      જો(!bvals) {
        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":\n"));
        જો(result)
          તોડો;
        ચાલુ;
      }

      દ્વિઅંકી = bv.bv_len > 7 &&
        curl_strnequal(bv.bv_val + bv.bv_len - 7, ";binary", 7);

      માટે(i = 0; bvals[i].bv_val; i++) {
        bool binval = FALSE;

        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":"));
        જો(result)
          તોડો;

        જો(!દ્વિઅંકી)
          binval = oldap_str_needs_base64(bvals[i].bv_val, bvals[i].bv_len);
        જો(દ્વિઅંકી || binval) {
          અક્ષર *val_b64 = ખાલી;
          size_t val_b64_sz = 0;

          /* Binary value, encode to base64. */
          જો(bvals[i].bv_len)
            result = curlx_base64_encode((uint8_t *)bvals[i].bv_val,
                                         bvals[i].bv_len,
                                         &val_b64, &val_b64_sz);
          જો(!result)
            result = client_write(data, STRCONST(": "), val_b64, val_b64_sz,
                                  STRCONST("\n"));
          curlx_free(val_b64);
        }
        નહીંતર
          result = client_write(data, STRCONST(" "),
                                bvals[i].bv_val, bvals[i].bv_len,
                                STRCONST("\n"));
        જો(result)
          તોડો;
      }

      ber_memfree(bvals);
      bvals = ખાલી;
      જો(!result)
        result = client_write(data, STRCONST("\n"), ખાલી, 0, ખાલી, 0);
      જો(result)
        તોડો;
    }

    જો(!result)
      result = client_write(data, STRCONST("\n"), ખાલી, 0, ખાલી, 0);
    જો(!result)
      result = CURLE_AGAIN;
    તોડો;
  }

  ber_free(ber, 0);
  ldap_msgfree(સંદેશ);
  પાછા result;
}

શૂન્ય Curl_ldap_version(અક્ષર *buf, size_t bufsz)
{
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  જો(ldap_get_option(ખાલી, LDAP_OPT_API_INFO, &api) == LDAP_OPT_SUCCESS) {
    unsigned પૂર્ણાંક patch = (unsigned પૂર્ણાંક)(api.ldapai_vendor_version % 100);
    unsigned પૂર્ણાંક major = (unsigned પૂર્ણાંક)(api.ldapai_vendor_version / 10000);
    unsigned પૂર્ણાંક minor =
      (((unsigned પૂર્ણાંક)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u",
                   api.ldapai_vendor_name, major, minor, patch);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((શૂન્ય **)api.ldapai_extensions);
  }
  નહીંતર
    curl_msnprintf(buf, bufsz, "OpenLDAP");
}

/*
 * LDAP protocol handler.
 */
અચલ સંરચના Curl_protocol Curl_protocol_ldap = {
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
