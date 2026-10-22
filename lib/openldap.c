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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP)

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

#অন্তর্ভাব <ldap.h>

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "curl_ldap.h"
#অন্তর্ভাব "curlx/base64.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "curl_sasl.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "bufref.h"

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
প্রকারনির্দেশ গণনা {
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
বাহ্য পূর্ণাঙ্ক ldap_pvt_url_scheme2proto(ধ্রুব বর্ণ *);
বাহ্য পূর্ণাঙ্ক ldap_init_fd(ber_socket_t fd, পূর্ণাঙ্ক proto, ধ্রুব বর্ণ *url,
                        LDAP **ld);
#endif

স্থির Curl_recv oldap_recv;

সংরচনা ldapconninfo {
  সংরচনা SASL sasl;          /* SASL-related parameters */
  LDAP *ld;                  /* Openldap connection handle. */
  Curl_recv *recv;           /* For stacking SSL handler */
  Curl_send *send;
  সংরচনা berval *servercred; /* SASL data from server. */
  ldapstate state;           /* Current machine state. */
  curl_socket_t dupfd;       /* duplicated socket */
  পূর্ণাঙ্ক proto;                 /* LDAP_PROTO_TCP/LDAP_PROTO_UDP/LDAP_PROTO_IPC */
  পূর্ণাঙ্ক msgid;                 /* Current message id. */
};

সংরচনা ldapreqinfo {
  পূর্ণাঙ্ক msgid;
  পূর্ণাঙ্ক nument;
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
স্থির শূন্য oldap_state(সংরচনা Curl_easy *data, সংরচনা ldapconninfo *li,
                        ldapstate newstate)
{
#যদি defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  স্থির ধ্রুব বর্ণ * ধ্রুব names[] = {
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

  যদি(li->state != newstate)
    infof(data, "LDAP %p state change from %s to %s",
          (শূন্য *)li, names[li->state], names[newstate]);
#অন্যথা
  (শূন্য)data;
#endif
  li->state = newstate;
}

/* Map some particular LDAP error codes to CURLcode values. */
স্থির CURLcode oldap_map_error(পূর্ণাঙ্ক rc, CURLcode result)
{
  বিকল্প(rc) {
  পক্ষ LDAP_NO_MEMORY:
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  পক্ষ LDAP_INVALID_CREDENTIALS:
    প্রত্যাগম CURLE_LOGIN_DENIED;
  পক্ষ LDAP_PROTOCOL_ERROR:
    প্রত্যাগম CURLE_UNSUPPORTED_PROTOCOL;
  পক্ষ LDAP_INSUFFICIENT_ACCESS:
    প্রত্যাগম CURLE_REMOTE_ACCESS_DENIED;
  }
  প্রত্যাগম result;
}

স্থির CURLcode oldap_url_parse(সংরচনা Curl_easy *data, LDAPURLDesc **ludp)
{
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক rc;
  স্থির ধ্রুব বর্ণ * ধ্রুব url_errs[] = {
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

  *ludp = রিক্ত;
  /* `ldap_url_parse() seems to be terrible with urls
   * that have user/pass/options in it. So when we have options or
   * creds from the url, fail without calling the function.
   * Yes, this is super-weird code and I do not like it. */
  যদি((data->state.creds && (data->state.creds->source == CREDS_URL)) ||
     data->state.up.options)
    rc = LDAP_URL_ERR_BADURL;
  অন্যথা
    rc = ldap_url_parse(Curl_bufref_ptr(&data->state.url), ludp);

  যদি(rc != LDAP_URL_SUCCESS) {
    ধ্রুব বর্ণ *বার্তা = "url parsing problem";

    result = rc == LDAP_URL_ERR_MEM ? CURLE_OUT_OF_MEMORY :
      CURLE_URL_MALFORMAT;
    rc -= LDAP_URL_SUCCESS;
    যদি((size_t)rc < CURL_ARRAYSIZE(url_errs))
      বার্তা = url_errs[rc];
    failf(data, "LDAP local: %s", বার্তা);
  }
  প্রত্যাগম result;
}

/* Parse the login options. */
স্থির CURLcode oldap_parse_login_options(সংরচনা connectdata *conn)
{
  CURLcode result = CURLE_OK;
  সংরচনা ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ধ্রুব বর্ণ *ptr = conn->options;

  DEBUGASSERT(li);
  যদি(!li)
    প্রত্যাগম CURLE_FAILED_INIT;

  যাবত্(!result && ptr && *ptr) {
    ধ্রুব বর্ণ *চাবি = ptr;
    ধ্রুব বর্ণ *মান;

    যাবত্(*ptr && *ptr != '=')
      ptr++;

    মান = ptr + 1;

    যাবত্(*ptr && *ptr != ';')
      ptr++;

    যদি(checkprefix("AUTH=", চাবি))
      result = Curl_sasl_parse_url_auth_option(&li->sasl, মান, ptr - মান);
    অন্যথা
      result = CURLE_SETOPT_OPTION_SYNTAX;

    যদি(*ptr == ';')
      ptr++;
  }

  প্রত্যাগম result == CURLE_URL_MALFORMAT ? CURLE_SETOPT_OPTION_SYNTAX : result;
}

স্থির CURLcode oldap_setup_connection(সংরচনা Curl_easy *data,
                                       সংরচনা connectdata *conn)
{
  CURLcode result;
  LDAPURLDesc *lud;
  (শূন্য)conn;

  /* Early URL syntax check. */
  result = oldap_url_parse(data, &lud);
  ldap_free_urldesc(lud);

  প্রত্যাগম result;
}

/*
 * Get the SASL authentication challenge from the server credential buffer.
 */
স্থির CURLcode oldap_get_message(সংরচনা Curl_easy *data, সংরচনা bufref *out)
{
  সংরচনা ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  সংরচনা berval *servercred = li ? li->servercred : রিক্ত;
  DEBUGASSERT(li);
  যদি(!li)
    প্রত্যাগম CURLE_FAILED_INIT;

  যদি(!servercred || !servercred->bv_val)
    প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;
  Curl_bufref_set(out, servercred->bv_val, servercred->bv_len, রিক্ত);
  প্রত্যাগম CURLE_OK;
}

/*
 * Sends an initial SASL bind request to the server.
 */
স্থির CURLcode oldap_perform_auth(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *mech,
                                   ধ্রুব সংরচনা bufref *initresp)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  সংরচনা berval cred;
  সংরচনা berval *pcred = &cred;
  পূর্ণাঙ্ক rc;

  DEBUGASSERT(li);
  যদি(!li)
    প্রত্যাগম CURLE_FAILED_INIT;
  cred.bv_val = (বর্ণ *)CURL_UNCONST(Curl_bufref_ptr(initresp));
  cred.bv_len = Curl_bufref_len(initresp);
  যদি(!cred.bv_val)
    pcred = রিক্ত;
  rc = ldap_sasl_bind(li->ld, রিক্ত, mech, pcred, রিক্ত, রিক্ত, &li->msgid);
  যদি(rc != LDAP_SUCCESS)
    প্রত্যাগম oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  প্রত্যাগম CURLE_OK;
}

/*
 * Sends SASL continuation.
 */
স্থির CURLcode oldap_continue_auth(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *mech,
                                    ধ্রুব সংরচনা bufref *resp)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  সংরচনা berval cred;
  সংরচনা berval *pcred = &cred;
  পূর্ণাঙ্ক rc;

  যদি(!li)
    প্রত্যাগম CURLE_FAILED_INIT;
  cred.bv_val = (বর্ণ *)CURL_UNCONST(Curl_bufref_ptr(resp));
  cred.bv_len = Curl_bufref_len(resp);
  যদি(!cred.bv_val)
    pcred = রিক্ত;
  rc = ldap_sasl_bind(li->ld, রিক্ত, mech, pcred, রিক্ত, রিক্ত, &li->msgid);
  যদি(rc != LDAP_SUCCESS)
    প্রত্যাগম oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  প্রত্যাগম CURLE_OK;
}

/*
 * Sends SASL bind cancellation.
 */
স্থির CURLcode oldap_cancel_auth(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *mech)
{
  সংরচনা ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  পূর্ণাঙ্ক rc;

  (শূন্য)mech;
  যদি(!li)
    প্রত্যাগম CURLE_FAILED_INIT;
  rc = ldap_sasl_bind(li->ld, রিক্ত, LDAP_SASL_NULL, রিক্ত, রিক্ত, রিক্ত,
                      &li->msgid);
  যদি(rc != LDAP_SUCCESS)
    প্রত্যাগম oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  প্রত্যাগম CURLE_OK;
}

/* Starts LDAP simple bind. */
স্থির CURLcode oldap_perform_bind(সংরচনা Curl_easy *data, ldapstate newstate)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ধ্রুব বর্ণ *binddn = রিক্ত;
  সংরচনা berval passwd;
  পূর্ণাঙ্ক rc;

  যদি(!li)
    প্রত্যাগম CURLE_FAILED_INIT;
  passwd.bv_val = রিক্ত;
  passwd.bv_len = 0;

  যদি(conn->creds) {
    binddn = Curl_creds_user(conn->creds);
    passwd.bv_val = CURL_UNCONST(Curl_creds_passwd(conn->creds));
    passwd.bv_len = strlen(passwd.bv_val);
  }

  rc = ldap_sasl_bind(li->ld, binddn, LDAP_SASL_SIMPLE, &passwd,
                      রিক্ত, রিক্ত, &li->msgid);
  যদি(rc != LDAP_SUCCESS)
    প্রত্যাগম oldap_map_error(rc,
                           data->state.creds ?
                           CURLE_LOGIN_DENIED : CURLE_LDAP_CANNOT_BIND);
  oldap_state(data, li, newstate);
  প্রত্যাগম CURLE_OK;
}

/* Query the supported SASL authentication mechanisms. */
স্থির CURLcode oldap_perform_mechs(সংরচনা Curl_easy *data)
{
  সংরচনা ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  পূর্ণাঙ্ক rc;
  স্থির ধ্রুব বর্ণ * ধ্রুব supportedSASLMechanisms[] = {
    "supportedSASLMechanisms",
    রিক্ত
  };

  যদি(!li)
    প্রত্যাগম CURLE_FAILED_INIT;
  /* Casting away the const for the 3rd parameter that the LDAP API expects as
     a non-const char ** is potentially unsafe but we believe the lack of
     const in the API was an oversight and that no LDAP implementation
     actually modifies the input. */
  rc = ldap_অনুসন্ধান_ext(li->ld, "", LDAP_SCOPE_BASE, "(objectclass=*)",
                       (বর্ণ **)CURL_UNCONST(supportedSASLMechanisms), 0,
                       রিক্ত, রিক্ত, রিক্ত, 0, &li->msgid);
  যদি(rc != LDAP_SUCCESS)
    প্রত্যাগম oldap_map_error(rc, CURLE_LOGIN_DENIED);
  oldap_state(data, li, OLDAP_MECHS);
  প্রত্যাগম CURLE_OK;
}

/* Starts SASL bind. */
স্থির CURLcode oldap_perform_sasl(সংরচনা Curl_easy *data)
{
  সংরচনা ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  saslprogress progress = SASL_IDLE;
  CURLcode result;

  যদি(!li)
    প্রত্যাগম CURLE_FAILED_INIT;
  result = Curl_sasl_start(&li->sasl, data, TRUE, &progress);

  oldap_state(data, li, OLDAP_SASL);
  যদি(!result && progress != SASL_INPROGRESS)
    result = Curl_sasl_is_blocked(&li->sasl, data);
  প্রত্যাগম result;
}

#ifdef USE_SSL
স্থির পূর্ণাঙ্ক ldapsb_tls_setup(Sockbuf_IO_Desc *sbiod, শূন্য *arg)
{
  sbiod->sbiod_pvt = arg;
  প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক ldapsb_tls_remove(Sockbuf_IO_Desc *sbiod)
{
  sbiod->sbiod_pvt = রিক্ত;
  প্রত্যাগম 0;
}

/* if there is a duplicated socket to close, close it */
স্থির পূর্ণাঙ্ক ldapsb_tls_close(Sockbuf_IO_Desc *sbiod)
{
  সংরচনা Curl_easy *data = sbiod->sbiod_pvt;
  যদি(data) {
    সংরচনা connectdata *conn = data->conn;
    যদি(conn) {
      সংরচনা ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      যদি(li && (li->dupfd != CURL_SOCKET_BAD)) {
        sclose(li->dupfd);
        li->dupfd = CURL_SOCKET_BAD;
      }
    }
  }

  প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক ldapsb_tls_ctrl(Sockbuf_IO_Desc *sbiod, পূর্ণাঙ্ক opt, শূন্য *arg)
{
  (শূন্য)arg;
  যদি(opt == LBER_SB_OPT_DATA_READY) {
    সংরচনা Curl_easy *data = sbiod->sbiod_pvt;
    প্রত্যাগম Curl_conn_data_pending(data, FIRSTSOCKET);
  }
  প্রত্যাগম 0;
}

স্থির ber_slen_t ldapsb_tls_read(Sockbuf_IO_Desc *sbiod, শূন্য *buf,
                                  ber_len_t len)
{
  সংরচনা Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  যদি(data) {
    সংরচনা connectdata *conn = data->conn;
    যদি(conn) {
      সংরচনা ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_RECV_ERROR;
      size_t nread;

      যদি(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        প্রত্যাগম -1;
      }
      result = (li->recv)(data, FIRSTSOCKET, buf, len, &nread);
      যদি(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nread;
    }
  }
  প্রত্যাগম ret;
}

স্থির ber_slen_t ldapsb_tls_write(Sockbuf_IO_Desc *sbiod, শূন্য *buf,
                                   ber_len_t len)
{
  সংরচনা Curl_easy *data = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  যদি(data) {
    সংরচনা connectdata *conn = data->conn;
    যদি(conn) {
      সংরচনা ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode result = CURLE_SEND_ERROR;
      size_t nwritten;

      যদি(!li) {
        SET_SOCKERRNO(SOCKEINVAL);
        প্রত্যাগম -1;
      }
      result = (li->send)(data, FIRSTSOCKET, buf, len, FALSE, &nwritten);
      যদি(result == CURLE_AGAIN) {
        SET_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = result ? -1 : (ber_slen_t)nwritten;
    }
  }
  প্রত্যাগম ret;
}

স্থির Sockbuf_IO ldapsb_tls = {
  ldapsb_tls_setup,
  ldapsb_tls_remove,
  ldapsb_tls_ctrl,
  ldapsb_tls_read,
  ldapsb_tls_write,
  ldapsb_tls_close
};

স্থির bool ssl_installed(সংরচনা connectdata *conn)
{
  সংরচনা ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  প্রত্যাগম li && li->recv;
}

স্থির CURLcode oldap_ssl_connect(সংরচনা Curl_easy *data, ldapstate newstate)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  bool ssldone = FALSE;
  CURLcode result;

  যদি(!li)
    প্রত্যাগম CURLE_FAILED_INIT;
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  যদি(result)
    প্রত্যাগম result;
  oldap_state(data, li, newstate);

  যদি(ssldone) {
    Sockbuf *sb;

    /* Install the libcurl SSL handlers into the sockbuf. */
    যদি((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
      প্রত্যাগম CURLE_FAILED_INIT;
    li->recv = conn->recv[FIRSTSOCKET];
    li->send = conn->send[FIRSTSOCKET];
  }

  প্রত্যাগম result;
}

/* Send the STARTTLS request */
স্থির CURLcode oldap_perform_starttls(সংরচনা Curl_easy *data)
{
  সংরচনা ldapconninfo *li =
    Curl_conn_meta_get(data->conn, CURL_META_LDAP_CONN);
  পূর্ণাঙ্ক rc;

  যদি(!li)
    প্রত্যাগম CURLE_FAILED_INIT;
  rc = ldap_start_tls(li->ld, রিক্ত, রিক্ত, &li->msgid);
  যদি(rc != LDAP_SUCCESS)
    প্রত্যাগম oldap_map_error(rc, CURLE_USE_SSL_FAILED);
  oldap_state(data, li, OLDAP_STARTTLS);
  প্রত্যাগম CURLE_OK;
}
#endif

স্থির শূন্য oldap_easy_dtor(ধ্রুব শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা ldapreqinfo *lr = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  curlx_free(lr);
}

স্থির শূন্য oldap_conn_dtor(ধ্রুব শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা ldapconninfo *li = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  যদি(li->ld) {
    ldap_unbind_ext(li->ld, রিক্ত, রিক্ত);
    li->ld = রিক্ত;
  }
  curlx_free(li);
}

/* SASL parameters for the ldap protocol */
স্থির ধ্রুব সংরচনা SASLproto saslldap = {
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

স্থির CURLcode oldap_connect(সংরচনা Curl_easy *data, bool *done)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ldapconninfo *li;
  স্থির ধ্রুব পূর্ণাঙ্ক version = LDAP_VERSION3;
  বর্ণ *hosturl = রিক্ত;
  CURLcode result;
  পূর্ণাঙ্ক rc;
#ifdef CURL_OPENLDAP_DEBUG
  স্থির পূর্ণাঙ্ক do_trace = -1;
#endif

  (শূন্য)done;

  li = curlx_calloc(1, পরিমাণ(সংরচনা ldapconninfo));
  যদি(!li) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }
  li->dupfd = CURL_SOCKET_BAD;

  result = Curl_conn_meta_set(conn, CURL_META_LDAP_CONN, li, oldap_conn_dtor);
  যদি(result)
    গচ্ছ out;

  li->proto = ldap_pvt_url_scheme2proto(data->state.origin->scheme->name);

  /* Initialize the SASL storage */
  Curl_sasl_init(&li->sasl, data, &saslldap);

  result = oldap_parse_login_options(conn);
  যদি(result)
    গচ্ছ out;

  hosturl = curl_maprintf("%s://%s:%u",
                          conn->scheme->name,
                          conn->origin->ipv6 ?
                          conn->origin->user_hostname :
                          conn->origin->hostname,
                          conn->origin->port);
  যদি(!hosturl) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  {
#ifdef _WIN32
    SOCKET dupfd = INVALID_SOCKET;
    WSAPROTOCOL_INFO pi;
    যদি(WSADuplicateSocket(conn->sock[FIRSTSOCKET], GetCurrentProcessId(),
                          &pi))
      ; /* error */
    অন্যথা
      dupfd = CURL_SOCKET(pi.iAddressFamily, pi.iSocketType, pi.iProtocol);
#অন্যথা
#ifdef F_DUPFD_CLOEXEC
    পূর্ণাঙ্ক dupfd = fcntl(conn->sock[FIRSTSOCKET], F_DUPFD_CLOEXEC, 0);
#অন্যথা
    পূর্ণাঙ্ক dupfd = dup(conn->sock[FIRSTSOCKET]);
#ifdef HAVE_FCNTL
    যদি((dupfd != CURL_SOCKET_BAD) &&
       (fcntl(dupfd, F_SETFD, FD_CLOEXEC) < 0)) {
      sclose(dupfd);
      dupfd = CURL_SOCKET_BAD;
    }
#endif /* HAVE_FCNTL */
#endif /* F_DUPFD_CLOEXEC */
#endif /* _WIN32 */

    যদি(dupfd == CURL_SOCKET_BAD) {
      result = CURLE_COULDNT_CONNECT;
      গচ্ছ out;
    }
    rc = ldap_init_fd((ber_socket_t)dupfd, li->proto, hosturl, &li->ld);
    যদি(rc)
      sclose(dupfd);
    অন্যথা
      /* store the socket, as when this is used by TLS we close this ourselves
         and at this point the code does not yet know if this will use TLS or
         not */
      li->dupfd = dupfd;
  }
  যদি(rc) {
    failf(data, "LDAP local: Cannot connect to %s, %s",
          hosturl, ldap_err2string(rc));
    result = CURLE_COULDNT_CONNECT;
    গচ্ছ out;
  }

#ifdef CURL_OPENLDAP_DEBUG
  যদি(do_trace < 0) {
    ধ্রুব বর্ণ *env = getenv("CURL_OPENLDAP_TRACE");
    curl_off_t e = 0;
    যদি(!curlx_str_সংখ্যা(&env, &e, INT_MAX))
      do_trace = e > 0;
  }
  যদি(do_trace)
    ldap_set_option(li->ld, LDAP_OPT_DEBUG_LEVEL, &do_trace);
#endif

  /* Try version 3 first. */
  ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);

  /* Do not chase referrals. */
  ldap_set_option(li->ld, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  {
    ber_len_t max = 256 * 1024;
    Sockbuf *sb;
    যদি((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       /* Set the maximum allowed size of an incoming message, which to
          OpenLDAP means that it will malloc() memory up to this size. If not
          set, there is no limit and we instead risk a malloc() failure. */
       !ber_sockbuf_ctrl(sb, LBER_SB_OPT_SET_MAX_INCOMING, &max)) {
      result = CURLE_FAILED_INIT;
      গচ্ছ out;
    }
  }

#ifdef USE_SSL
  যদি(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = oldap_ssl_connect(data, OLDAP_SSL);
    গচ্ছ out;
  }

  যদি(data->set.use_ssl) {
    result = oldap_perform_starttls(data);
    যদি(!result || data->set.use_ssl != CURLUSESSL_TRY)
      গচ্ছ out;
  }
#endif

  যদি(li->sasl.prefmech != SASL_AUTH_NONE) {
    result = oldap_perform_mechs(data);
    গচ্ছ out;
  }

  /* Force bind even if anonymous bind is not needed in protocol version 3
     to detect missing version 3 support. */
  result = oldap_perform_bind(data, OLDAP_BIND);

out:
  curlx_free(hosturl);
  প্রত্যাগম result;
}

/* Handle the supported SASL mechanisms query response */
স্থির CURLcode oldap_state_mechs_resp(সংরচনা Curl_easy *data,
                                       LDAPMessage *বার্তা, পূর্ণাঙ্ক code)
{
  সংরচনা connectdata *conn;
  সংরচনা ldapconninfo *li;
  পূর্ণাঙ্ক rc;
  BerElement *ber = রিক্ত;
  CURLcode result = CURLE_OK;
  সংরচনা berval bv, *bvals;

  যদি(!data)
    প্রত্যাগম CURLE_FAILED_INIT;

  conn = data->conn;
  li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);

  যদি(!li)
    প্রত্যাগম CURLE_FAILED_INIT;
  বিকল্প(ldap_msgtype(বার্তা)) {
  পক্ষ LDAP_RES_অনুসন্ধান_ENTRY:
    /* Got a list of supported SASL mechanisms. */
    যদি(code != LDAP_SUCCESS && code != LDAP_NO_RESULTS_RETURNED)
      প্রত্যাগম CURLE_LOGIN_DENIED;

    rc = ldap_get_dn_ber(li->ld, বার্তা, &ber, &bv);
    যদি(rc < 0)
      প্রত্যাগম oldap_map_error(rc, CURLE_BAD_CONTENT_ENCODING);
    প্রতি(rc = ldap_get_attribute_ber(li->ld, বার্তা, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, বার্তা, ber, &bv, &bvals)) {
      পূর্ণাঙ্ক i;

      যদি(!bv.bv_val)
        ভঙ্গ;

      যদি(bvals) {
        প্রতি(i = 0; bvals[i].bv_val; i++) {
          size_t llen;
          unsigned short mech =
            Curl_sasl_decode_mech((ধ্রুব বর্ণ *)bvals[i].bv_val,
                                  bvals[i].bv_len, &llen);
          যদি(bvals[i].bv_len == llen)
            li->sasl.authmechs |= mech;
        }
        ber_memfree(bvals);
      }
    }
    ber_free(ber, 0);
    ভঙ্গ;

  পক্ষ LDAP_RES_অনুসন্ধান_RESULT:
    বিকল্প(code) {
    পক্ষ LDAP_SIZELIMIT_EXCEEDED:
      infof(data, "Too many authentication mechanisms");
      FALLTHROUGH();
    পক্ষ LDAP_SUCCESS:
    পক্ষ LDAP_NO_RESULTS_RETURNED:
      যদি(Curl_sasl_can_authenticate(&li->sasl, data))
        result = oldap_perform_sasl(data);
      অন্যথা
        result = CURLE_LOGIN_DENIED;
      ভঙ্গ;
    স্বভাব:
      result = oldap_map_error(code, CURLE_LOGIN_DENIED);
      ভঙ্গ;
    }
    ভঙ্গ;
  স্বভাব:
    ভঙ্গ;
  }
  প্রত্যাগম result;
}

/* Handle a SASL bind response. */
স্থির CURLcode oldap_state_sasl_resp(সংরচনা Curl_easy *data,
                                      LDAPMessage *বার্তা, পূর্ণাঙ্ক code)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode result = CURLE_OK;
  saslprogress progress;
  পূর্ণাঙ্ক rc;

  যদি(!li)
    প্রত্যাগম CURLE_FAILED_INIT;
  li->servercred = রিক্ত;
  rc = ldap_parse_sasl_bind_result(li->ld, বার্তা, &li->servercred, 0);
  যদি(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: sasl ldap_parse_result %s", ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LOGIN_DENIED);
  }
  অন্যথা {
    result = Curl_sasl_continue(&li->sasl, data, code, &progress);
    যদি(!result) {
      বিকল্প(progress) {
      পক্ষ SASL_DONE:
        oldap_state(data, li, OLDAP_STOP);   /* Authenticated */
        ভঙ্গ;
      পক্ষ SASL_IDLE:            /* No mechanism left after cancellation */
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
        ভঙ্গ;
      স্বভাব:
        ভঙ্গ;
      }
    }
  }

  যদি(li->servercred)
    ber_bvfree(li->servercred);
  প্রত্যাগম result;
}

/* Handle a simple bind response. */
স্থির CURLcode oldap_state_bind_resp(সংরচনা Curl_easy *data, LDAPMessage *বার্তা,
                                      পূর্ণাঙ্ক code)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode result = CURLE_OK;
  সংরচনা berval *bv = রিক্ত;
  পূর্ণাঙ্ক rc;

  যদি(!li)
    প্রত্যাগম CURLE_FAILED_INIT;

  যদি(code != LDAP_SUCCESS)
    প্রত্যাগম oldap_map_error(code, CURLE_LDAP_CANNOT_BIND);

  rc = ldap_parse_sasl_bind_result(li->ld, বার্তা, &bv, 0);
  যদি(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: bind ldap_parse_sasl_bind_result %s",
          ldap_err2string(rc));
    result = oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  }
  অন্যথা
    oldap_state(data, li, OLDAP_STOP);

  যদি(bv)
    ber_bvfree(bv);
  প্রত্যাগম result;
}

স্থির CURLcode oldap_connecting(সংরচনা Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  LDAPMessage *বার্তা = রিক্ত;
  সংরচনা timeval tv = { 0, 0 };
  পূর্ণাঙ্ক code = LDAP_SUCCESS;
  পূর্ণাঙ্ক rc;

  যদি(!li)
    প্রত্যাগম CURLE_FAILED_INIT;

  যদি(li->state != OLDAP_SSL && li->state != OLDAP_TLS) {
    /* Get response to last command. */
    rc = ldap_result(li->ld, li->msgid, LDAP_বার্তা_ONE, &tv, &বার্তা);
    বিকল্প(rc) {
    পক্ষ 0:                               /* Timed out. */
      প্রত্যাগম CURLE_OK;
    পক্ষ LDAP_RES_অনুসন্ধান_ENTRY:
    পক্ষ LDAP_RES_অনুসন্ধান_REFERENCE:
      ভঙ্গ;
    স্বভাব:
      li->msgid = 0;                      /* Nothing to abandon upon error. */
      যদি(rc < 0) {
        failf(data, "LDAP local: connecting ldap_result %s",
              ldap_err2string(rc));
        প্রত্যাগম oldap_map_error(rc, CURLE_COULDNT_CONNECT);
      }
      ভঙ্গ;
    }

    /* Get error code from message. */
    rc = ldap_parse_result(li->ld, বার্তা, &code, রিক্ত, রিক্ত, রিক্ত, রিক্ত, 0);
    যদি(rc)
      code = rc;
    অন্যথা {
      /* store the latest code for later retrieval */
      data->info.httpcode = code;
    }

    /* If protocol version 3 is not supported, fallback to version 2. */
    যদি(code == LDAP_PROTOCOL_ERROR && li->state != OLDAP_BINDV2 &&
#ifdef USE_SSL
       (ssl_installed(conn) || data->set.use_ssl <= CURLUSESSL_TRY) &&
#endif
       li->sasl.prefmech == SASL_AUTH_NONE) {
      স্থির ধ্রুব পূর্ণাঙ্ক version = LDAP_VERSION2;

      ldap_set_option(li->ld, LDAP_OPT_PROTOCOL_VERSION, &version);
      ldap_msgfree(বার্তা);
      প্রত্যাগম oldap_perform_bind(data, OLDAP_BINDV2);
    }
  }

  /* Handle response message according to current state. */
  বিকল্প(li->state) {

#ifdef USE_SSL
  পক্ষ OLDAP_SSL:
    result = oldap_ssl_connect(data, OLDAP_SSL);
    যদি(!result && ssl_installed(conn)) {
      যদি(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      অন্যথা
        result = oldap_perform_bind(data, OLDAP_BIND);
    }
    ভঙ্গ;
  পক্ষ OLDAP_STARTTLS:
    যদি(code != LDAP_SUCCESS) {
      যদি(data->set.use_ssl != CURLUSESSL_TRY)
        result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
      অন্যথা যদি(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      অন্যথা
        result = oldap_perform_bind(data, OLDAP_BIND);
      ভঙ্গ;
    }
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    যদি(result)
      ভঙ্গ;
    FALLTHROUGH();
  পক্ষ OLDAP_TLS:
    result = oldap_ssl_connect(data, OLDAP_TLS);
    যদি(result)
      result = oldap_map_error(code, CURLE_USE_SSL_FAILED);
    অন্যথা যদি(ssl_installed(conn)) {
      যদি(li->sasl.prefmech != SASL_AUTH_NONE)
        result = oldap_perform_mechs(data);
      অন্যথা যদি(data->state.creds)
        result = oldap_perform_bind(data, OLDAP_BIND);
      অন্যথা {
        /* Version 3 supported: no bind required */
        oldap_state(data, li, OLDAP_STOP);
        result = CURLE_OK;
      }
    }
    ভঙ্গ;
#endif

  পক্ষ OLDAP_MECHS:
    result = oldap_state_mechs_resp(data, বার্তা, code);
    ভঙ্গ;
  পক্ষ OLDAP_SASL:
    result = oldap_state_sasl_resp(data, বার্তা, code);
    ভঙ্গ;
  পক্ষ OLDAP_BIND:
  পক্ষ OLDAP_BINDV2:
    result = oldap_state_bind_resp(data, বার্তা, code);
    ভঙ্গ;
  স্বভাব:
    /* internal error */
    result = CURLE_COULDNT_CONNECT;
    ভঙ্গ;
  }

  ldap_msgfree(বার্তা);

  *done = li->state == OLDAP_STOP;
  যদি(*done)
    conn->recv[FIRSTSOCKET] = oldap_recv;

  যদি(result && li->msgid) {
    ldap_abandon_ext(li->ld, li->msgid, রিক্ত, রিক্ত);
    li->msgid = 0;
  }
  প্রত্যাগম result;
}

স্থির CURLcode oldap_disconnect(সংরচনা Curl_easy *data,
                                 সংরচনা connectdata *conn,
                                 bool dead_connection)
{
  সংরচনা ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  (শূন্য)dead_connection;
#ifndef USE_SSL
  (শূন্য)data;
#endif

  যদি(li && li->ld) {
#ifdef USE_SSL
    যদি(ssl_installed(conn)) {
      Sockbuf *sb;
      যদি(ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS ||
         ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data))
        প্রত্যাগম CURLE_FAILED_INIT;
    }
#endif
    ldap_unbind_ext(li->ld, রিক্ত, রিক্ত);
    li->ld = রিক্ত;
  }
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode oldap_do(সংরচনা Curl_easy *data, bool *done)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  সংরচনা ldapreqinfo *lr;
  CURLcode result;
  পূর্ণাঙ্ক rc;
  LDAPURLDesc *lud;
  পূর্ণাঙ্ক msgid;

  যদি(!li)
    প্রত্যাগম CURLE_FAILED_INIT;

  infof(data, "LDAP local: %s", Curl_bufref_ptr(&data->state.url));

  result = oldap_url_parse(data, &lud);
  যদি(result)
    গচ্ছ out;

#ifdef USE_SSL
  যদি(ssl_installed(conn)) {
    Sockbuf *sb;
    /* re-install the libcurl SSL handlers into the sockbuf. */
    যদি((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_add_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, data)) {
      ldap_free_urldesc(lud);
      প্রত্যাগম CURLE_FAILED_INIT;
    }
  }
#endif

  rc = ldap_অনুসন্ধান_ext(li->ld, lud->lud_dn, lud->lud_scope,
                       lud->lud_filter, lud->lud_attrs, 0,
                       রিক্ত, রিক্ত, রিক্ত, 0, &msgid);
  ldap_free_urldesc(lud);
  যদি(rc != LDAP_SUCCESS) {
    failf(data, "LDAP local: ldap_search_ext %s", ldap_err2string(rc));
    result = CURLE_LDAP_অনুসন্ধান_FAILED;
    গচ্ছ out;
  }

  lr = curlx_calloc(1, পরিমাণ(সংরচনা ldapreqinfo));
  যদি(!lr ||
     Curl_meta_set(data, CURL_META_LDAP_EASY, lr, oldap_easy_dtor)) {
    ldap_abandon_ext(li->ld, msgid, রিক্ত, রিক্ত);
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  lr->msgid = msgid;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  *done = TRUE;

out:
  প্রত্যাগম result;
}

স্থির CURLcode oldap_done(সংরচনা Curl_easy *data, CURLcode res,
                           bool premature)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);

  (শূন্য)res;
  (শূন্য)premature;

  যদি(lr) {
    /* if there was a search in progress, abandon it */
    যদি(lr->msgid) {
      সংরচনা ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      যদি(li && li->ld) {
        ldap_abandon_ext(li->ld, lr->msgid, রিক্ত, রিক্ত);
      }
      lr->msgid = 0;
    }
    Curl_meta_remove(data, CURL_META_LDAP_EASY);
  }

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode client_write(সংরচনা Curl_easy *data,
                             ধ্রুব বর্ণ *prefix, size_t plen,
                             ধ্রুব বর্ণ *মান, size_t len,
                             ধ্রুব বর্ণ *suffix, size_t slen)
{
  CURLcode result = CURLE_OK;

  যদি(prefix) {
    /* If we have a zero-length value and the prefix ends with a space
       separator, drop the latter. */
    যদি(!len && plen && prefix[plen - 1] == ' ')
      plen--;
    result = Curl_client_write(data, CLIENTWRITE_BODY, prefix, plen);
  }
  যদি(!result && মান) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, মান, len);
  }
  যদি(!result && suffix) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, suffix, slen);
  }
  প্রত্যাগম result;
}

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
স্থির bool oldap_str_needs_base64(ধ্রুব বর্ণ *s, size_t len)
{
  size_t i;
  যদি(!len)
    প্রত্যাগম FALSE;
  যদি((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    প্রত্যাগম TRUE;
  প্রতি(i = 0; i < len; i++)
    যদি(!ISPRINT(s[i]))
      প্রত্যাগম TRUE;
  প্রত্যাগম FALSE;
}

স্থির CURLcode oldap_recv(সংরচনা Curl_easy *data, int8_t sockindex, বর্ণ *buf,
                           size_t len, size_t *pnread)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  সংরচনা ldapreqinfo *lr = Curl_meta_get(data, CURL_META_LDAP_EASY);
  পূর্ণাঙ্ক rc;
  LDAPMessage *বার্তা = রিক্ত;
  BerElement *ber = রিক্ত;
  সংরচনা timeval tv = { 0, 0 };
  সংরচনা berval bv, *bvals;
  CURLcode result = CURLE_AGAIN;
  পূর্ণাঙ্ক code;
  বর্ণ *info = রিক্ত;

  (শূন্য)len;
  (শূন্য)buf;
  (শূন্য)sockindex;
  *pnread = 0;
  যদি(!li || !lr)
    প্রত্যাগম CURLE_FAILED_INIT;

  rc = ldap_result(li->ld, lr->msgid, LDAP_বার্তা_ONE, &tv, &বার্তা);
  যদি(rc < 0) {
    failf(data, "LDAP local: search ldap_result %s", ldap_err2string(rc));
    result = CURLE_RECV_ERROR;
  }

  /* error or timed out */
  যদি(!বার্তা)
    প্রত্যাগম result;

  result = CURLE_OK;

  বিকল্প(ldap_msgtype(বার্তা)) {
  পক্ষ LDAP_RES_অনুসন্ধান_RESULT:
    lr->msgid = 0;
    rc = ldap_parse_result(li->ld, বার্তা, &code, রিক্ত, &info, রিক্ত, রিক্ত, 0);
    যদি(rc) {
      failf(data, "LDAP local: search ldap_parse_result %s",
            ldap_err2string(rc));
      result = CURLE_LDAP_অনুসন্ধান_FAILED;
      ভঙ্গ;
    }

    /* store the latest code for later retrieval */
    data->info.httpcode = code;

    বিকল্প(code) {
    পক্ষ LDAP_SIZELIMIT_EXCEEDED:
      infof(data, "There are more than %d entries", lr->nument);
      FALLTHROUGH();
    পক্ষ LDAP_SUCCESS:
      data->req.size = data->req.bytecount;
      ভঙ্গ;
    স্বভাব:
      failf(data, "LDAP remote: search failed %s %s", ldap_err2string(code),
            info ? info : "");
      result = CURLE_LDAP_অনুসন্ধান_FAILED;
      ভঙ্গ;
    }
    যদি(info)
      ldap_memfree(info);
    ভঙ্গ;
  পক্ষ LDAP_RES_অনুসন্ধান_ENTRY:
    lr->nument++;
    rc = ldap_get_dn_ber(li->ld, বার্তা, &ber, &bv);
    যদি(rc < 0) {
      result = CURLE_RECV_ERROR;
      ভঙ্গ;
    }

    যদি(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
      বর্ণ *dn_b64 = রিক্ত;
      size_t dn_b64_sz = 0;

      /* A DN with e.g. CR/LF would inject LDIF lines, so base64-encode it. */
      result = curlx_base64_encode((uint8_t *)bv.bv_val, bv.bv_len,
                                   &dn_b64, &dn_b64_sz);
      যদি(!result)
        result = client_write(data, STRCONST("DN:: "), dn_b64, dn_b64_sz,
                              STRCONST("\n"));
      curlx_free(dn_b64);
    }
    অন্যথা
      result = client_write(data, STRCONST("DN: "), bv.bv_val, bv.bv_len,
                            STRCONST("\n"));
    যদি(result)
      ভঙ্গ;

    প্রতি(rc = ldap_get_attribute_ber(li->ld, বার্তা, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, বার্তা, ber, &bv, &bvals)) {
      পূর্ণাঙ্ক i;
      bool দ্বিমিক;

      যদি(!bv.bv_val)
        ভঙ্গ;

      /* An attribute type has no base64 form in LDIF, so a type carrying
         control bytes (CR/LF etc.) would let a hostile server inject lines.
         A conformant type name never contains them, so reject it. */
      যদি(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
        failf(data, "LDAP local: attribute type contains illegal bytes");
        যদি(bvals)
          ber_memfree(bvals);
        result = CURLE_WEIRD_SERVER_REPLY;
        ভঙ্গ;
      }

      যদি(!bvals) {
        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":\n"));
        যদি(result)
          ভঙ্গ;
        অনুবর্ত;
      }

      দ্বিমিক = bv.bv_len > 7 &&
        curl_strnequal(bv.bv_val + bv.bv_len - 7, ";binary", 7);

      প্রতি(i = 0; bvals[i].bv_val; i++) {
        bool binval = FALSE;

        result = client_write(data, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":"));
        যদি(result)
          ভঙ্গ;

        যদি(!দ্বিমিক)
          binval = oldap_str_needs_base64(bvals[i].bv_val, bvals[i].bv_len);
        যদি(দ্বিমিক || binval) {
          বর্ণ *val_b64 = রিক্ত;
          size_t val_b64_sz = 0;

          /* Binary value, encode to base64. */
          যদি(bvals[i].bv_len)
            result = curlx_base64_encode((uint8_t *)bvals[i].bv_val,
                                         bvals[i].bv_len,
                                         &val_b64, &val_b64_sz);
          যদি(!result)
            result = client_write(data, STRCONST(": "), val_b64, val_b64_sz,
                                  STRCONST("\n"));
          curlx_free(val_b64);
        }
        অন্যথা
          result = client_write(data, STRCONST(" "),
                                bvals[i].bv_val, bvals[i].bv_len,
                                STRCONST("\n"));
        যদি(result)
          ভঙ্গ;
      }

      ber_memfree(bvals);
      bvals = রিক্ত;
      যদি(!result)
        result = client_write(data, STRCONST("\n"), রিক্ত, 0, রিক্ত, 0);
      যদি(result)
        ভঙ্গ;
    }

    যদি(!result)
      result = client_write(data, STRCONST("\n"), রিক্ত, 0, রিক্ত, 0);
    যদি(!result)
      result = CURLE_AGAIN;
    ভঙ্গ;
  }

  ber_free(ber, 0);
  ldap_msgfree(বার্তা);
  প্রত্যাগম result;
}

শূন্য Curl_ldap_version(বর্ণ *buf, size_t bufsz)
{
  LDAPAPIInfo api;
  api.ldapai_info_version = LDAP_API_INFO_VERSION;

  যদি(ldap_get_option(রিক্ত, LDAP_OPT_API_INFO, &api) == LDAP_OPT_SUCCESS) {
    unsigned পূর্ণাঙ্ক patch = (unsigned পূর্ণাঙ্ক)(api.ldapai_vendor_version % 100);
    unsigned পূর্ণাঙ্ক major = (unsigned পূর্ণাঙ্ক)(api.ldapai_vendor_version / 10000);
    unsigned পূর্ণাঙ্ক minor =
      (((unsigned পূর্ণাঙ্ক)api.ldapai_vendor_version - (major * 10000))
       - patch) / 100;
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u",
                   api.ldapai_vendor_name, major, minor, patch);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((শূন্য **)api.ldapai_extensions);
  }
  অন্যথা
    curl_msnprintf(buf, bufsz, "OpenLDAP");
}

/*
 * LDAP protocol handler.
 */
ধ্রুব সংরচনা Curl_protocol Curl_protocol_ldap = {
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
