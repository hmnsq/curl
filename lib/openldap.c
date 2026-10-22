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
قسمتعریف شمار {
  OLDAP_STOP,           /* Do nothing state, stops the state machine */
  OLDAP_SSL,            /* Performing SSL handshake. */
  OLDAP_STARTTLS,       /* STARTTLS request sent. */
  OLDAP_TLS,            /* Performing TLS handshake. */
  OLDAP_MECHS,          /* Get SASL authentication mechanisms. */
  OLDAP_SASL,           /* SASL binding reply. */
  OLDAP_BIND,           /* Simple bind reply. */
  OLDAP_BINDV2,         /* Simple bind reply in protocol version 2. */
  OLDAP_آخری            /* Never used */
} ldapstate;

#اگر_غیر_تعریف_شدہ _LDAP_PVT_H
خارجی صحیح ldap_pvt_url_scheme2proto(ثابت حرف *);
خارجی صحیح ldap_init_fd(ber_socket_t fd, صحیح proto, ثابت حرف *url,
                        LDAP **ld);
#اگر_ختم

ساکن Curl_recv oldap_recv;

ساخت ldapconninfo {
  ساخت SASL sasl;          /* SASL-related parameters */
  LDAP *ld;                  /* Openldap connection handle. */
  Curl_recv *recv;           /* For stacking SSL handler */
  Curl_send *send;
  ساخت berval *servercred; /* SASL data from server. */
  ldapstate state;           /* Current machine state. */
  curl_socket_t dupfd;       /* duplicated socket */
  صحیح proto;                 /* LDAP_PROTO_TCP/LDAP_PROTO_UDP/LDAP_PROTO_IPC */
  صحیح msgid;                 /* Current message id. */
};

ساخت ldapreqinfo {
  صحیح msgid;
  صحیح nument;
};

/* meta key for storing ldapconninfo at easy handle */
#تعریف_کرو CURL_META_LDAP_EASY   "meta:proto:ldap:easy"
/* meta key for storing ldapconninfo at connection */
#تعریف_کرو CURL_META_LDAP_CONN   "meta:proto:ldap:conn"

/*
 * oldap_state()
 *
 * This is the ONLY way to change LDAP state!
 */
ساکن خالی oldap_state(ساخت Curl_easy *ڈیٹا, ساخت ldapconninfo *li,
                        ldapstate newstate)
{
#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ساکن ثابت حرف * ثابت نام[] = {
    "STOP",
    "SSL",
    "STARTTLS",
    "TLS",
    "MECHS",
    "SASL",
    "BIND",
    "BINDV2",
    /*آخری*/
  };

  اگر(li->state != newstate)
    infof(ڈیٹا, "LDAP %p state change from %s to %s",
          (خالی *)li, نام[li->state], نام[newstate]);
#ورنہ
  (خالی)ڈیٹا;
#اگر_ختم
  li->state = newstate;
}

/* Map some particular LDAP error codes to CURLcode values. */
ساکن CURLcode oldap_map_error(صحیح rc, CURLcode نتیجہ)
{
  انتخاب(rc) {
  حالت LDAP_NO_حافظہ:
    واپس CURLE_برآمد_کا_حافظہ;
  حالت LDAP_INVALID_CREDENTIALS:
    واپس CURLE_LOGIN_DENIED;
  حالت LDAP_PROTOCOL_ERROR:
    واپس CURLE_UNSUPPORTED_PROTOCOL;
  حالت LDAP_INSUFFICIENT_ACCESS:
    واپس CURLE_REMOTE_ACCESS_DENIED;
  }
  واپس نتیجہ;
}

ساکن CURLcode oldap_url_parse(ساخت Curl_easy *ڈیٹا, LDAPURLDesc **ludp)
{
  CURLcode نتیجہ = CURLE_OK;
  صحیح rc;
  ساکن ثابت حرف * ثابت url_errs[] = {
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

  *ludp = تہی;
  /* `ldap_url_parse() seems to be terrible with urls
   * that have user/pass/options in it. So when we have options or
   * creds from the url, fail without calling the function.
   * Yes, this is super-weird code and I do not like it. */
  اگر((ڈیٹا->state.creds && (ڈیٹا->state.creds->ماخذ == CREDS_URL)) ||
     ڈیٹا->state.اوپر.options)
    rc = LDAP_URL_ERR_BADURL;
  ورنہ
    rc = ldap_url_parse(Curl_bufref_ptr(&ڈیٹا->state.url), ludp);

  اگر(rc != LDAP_URL_SUCCESS) {
    ثابت حرف *پیغام = "url parsing problem";

    نتیجہ = rc == LDAP_URL_ERR_MEM ? CURLE_برآمد_کا_حافظہ :
      CURLE_URL_MALFORMAT;
    rc -= LDAP_URL_SUCCESS;
    اگر((size_t)rc < CURL_ARRAYSIZE(url_errs))
      پیغام = url_errs[rc];
    failf(ڈیٹا, "LDAP local: %s", پیغام);
  }
  واپس نتیجہ;
}

/* Parse the login options. */
ساکن CURLcode oldap_parse_login_options(ساخت connectdata *conn)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ثابت حرف *ptr = conn->options;

  DEBUGASSERT(li);
  اگر(!li)
    واپس CURLE_FAILED_INIT;

  جب(!نتیجہ && ptr && *ptr) {
    ثابت حرف *کلید = ptr;
    ثابت حرف *قدر;

    جب(*ptr && *ptr != '=')
      ptr++;

    قدر = ptr + 1;

    جب(*ptr && *ptr != ';')
      ptr++;

    اگر(checkprefix("AUTH=", کلید))
      نتیجہ = Curl_sasl_parse_url_auth_option(&li->sasl, قدر, ptr - قدر);
    ورنہ
      نتیجہ = CURLE_SETOPT_OPTION_SYNTAX;

    اگر(*ptr == ';')
      ptr++;
  }

  واپس نتیجہ == CURLE_URL_MALFORMAT ? CURLE_SETOPT_OPTION_SYNTAX : نتیجہ;
}

ساکن CURLcode oldap_تیاری_connection(ساخت Curl_easy *ڈیٹا,
                                       ساخت connectdata *conn)
{
  CURLcode نتیجہ;
  LDAPURLDesc *lud;
  (خالی)conn;

  /* Early URL syntax check. */
  نتیجہ = oldap_url_parse(ڈیٹا, &lud);
  ldap_free_urldesc(lud);

  واپس نتیجہ;
}

/*
 * Get the SASL authentication challenge from the server credential buffer.
 */
ساکن CURLcode oldap_get_پیغام(ساخت Curl_easy *ڈیٹا, ساخت bufref *برآمد)
{
  ساخت ldapconninfo *li =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_LDAP_CONN);
  ساخت berval *servercred = li ? li->servercred : تہی;
  DEBUGASSERT(li);
  اگر(!li)
    واپس CURLE_FAILED_INIT;

  اگر(!servercred || !servercred->bv_val)
    واپس CURLE_WEIRD_SERVER_جواب;
  Curl_bufref_سیٹ(برآمد, servercred->bv_val, servercred->bv_len, تہی);
  واپس CURLE_OK;
}

/*
 * Sends an initial SASL bind request to the server.
 */
ساکن CURLcode oldap_perform_auth(ساخت Curl_easy *ڈیٹا, ثابت حرف *mech,
                                   ثابت ساخت bufref *initresp)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ساخت berval cred;
  ساخت berval *pcred = &cred;
  صحیح rc;

  DEBUGASSERT(li);
  اگر(!li)
    واپس CURLE_FAILED_INIT;
  cred.bv_val = (حرف *)CURL_UNCONST(Curl_bufref_ptr(initresp));
  cred.bv_len = Curl_bufref_len(initresp);
  اگر(!cred.bv_val)
    pcred = تہی;
  rc = ldap_sasl_bind(li->ld, تہی, mech, pcred, تہی, تہی, &li->msgid);
  اگر(rc != LDAP_SUCCESS)
    واپس oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  واپس CURLE_OK;
}

/*
 * Sends SASL continuation.
 */
ساکن CURLcode oldap_continue_auth(ساخت Curl_easy *ڈیٹا, ثابت حرف *mech,
                                    ثابت ساخت bufref *resp)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ساخت berval cred;
  ساخت berval *pcred = &cred;
  صحیح rc;

  اگر(!li)
    واپس CURLE_FAILED_INIT;
  cred.bv_val = (حرف *)CURL_UNCONST(Curl_bufref_ptr(resp));
  cred.bv_len = Curl_bufref_len(resp);
  اگر(!cred.bv_val)
    pcred = تہی;
  rc = ldap_sasl_bind(li->ld, تہی, mech, pcred, تہی, تہی, &li->msgid);
  اگر(rc != LDAP_SUCCESS)
    واپس oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  واپس CURLE_OK;
}

/*
 * Sends SASL bind cancellation.
 */
ساکن CURLcode oldap_cancel_auth(ساخت Curl_easy *ڈیٹا, ثابت حرف *mech)
{
  ساخت ldapconninfo *li =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_LDAP_CONN);
  صحیح rc;

  (خالی)mech;
  اگر(!li)
    واپس CURLE_FAILED_INIT;
  rc = ldap_sasl_bind(li->ld, تہی, LDAP_SASL_NULL, تہی, تہی, تہی,
                      &li->msgid);
  اگر(rc != LDAP_SUCCESS)
    واپس oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  واپس CURLE_OK;
}

/* Starts LDAP simple bind. */
ساکن CURLcode oldap_perform_bind(ساخت Curl_easy *ڈیٹا, ldapstate newstate)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ثابت حرف *binddn = تہی;
  ساخت berval passwd;
  صحیح rc;

  اگر(!li)
    واپس CURLE_FAILED_INIT;
  passwd.bv_val = تہی;
  passwd.bv_len = 0;

  اگر(conn->creds) {
    binddn = Curl_creds_user(conn->creds);
    passwd.bv_val = CURL_UNCONST(Curl_creds_passwd(conn->creds));
    passwd.bv_len = لڑی_لمبائی(passwd.bv_val);
  }

  rc = ldap_sasl_bind(li->ld, binddn, LDAP_SASL_SIMPLE, &passwd,
                      تہی, تہی, &li->msgid);
  اگر(rc != LDAP_SUCCESS)
    واپس oldap_map_error(rc,
                           ڈیٹا->state.creds ?
                           CURLE_LOGIN_DENIED : CURLE_LDAP_CANNOT_BIND);
  oldap_state(ڈیٹا, li, newstate);
  واپس CURLE_OK;
}

/* Query the supported SASL authentication mechanisms. */
ساکن CURLcode oldap_perform_mechs(ساخت Curl_easy *ڈیٹا)
{
  ساخت ldapconninfo *li =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_LDAP_CONN);
  صحیح rc;
  ساکن ثابت حرف * ثابت supportedSASLMechanisms[] = {
    "supportedSASLMechanisms",
    تہی
  };

  اگر(!li)
    واپس CURLE_FAILED_INIT;
  /* Casting away the const for the 3rd parameter that the LDAP API expects as
     a non-const char ** is potentially unsafe but we believe the lack of
     const in the API was an oversight and that no LDAP implementation
     actually modifies the input. */
  rc = ldap_تلاش_ext(li->ld, "", LDAP_SCOPE_بنیاد, "(objectclass=*)",
                       (حرف **)CURL_UNCONST(supportedSASLMechanisms), 0,
                       تہی, تہی, تہی, 0, &li->msgid);
  اگر(rc != LDAP_SUCCESS)
    واپس oldap_map_error(rc, CURLE_LOGIN_DENIED);
  oldap_state(ڈیٹا, li, OLDAP_MECHS);
  واپس CURLE_OK;
}

/* Starts SASL bind. */
ساکن CURLcode oldap_perform_sasl(ساخت Curl_easy *ڈیٹا)
{
  ساخت ldapconninfo *li =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_LDAP_CONN);
  saslprogress پیش_رفت = SASL_IDLE;
  CURLcode نتیجہ;

  اگر(!li)
    واپس CURLE_FAILED_INIT;
  نتیجہ = Curl_sasl_آغاز(&li->sasl, ڈیٹا, TRUE, &پیش_رفت);

  oldap_state(ڈیٹا, li, OLDAP_SASL);
  اگر(!نتیجہ && پیش_رفت != SASL_INPROGRESS)
    نتیجہ = Curl_sasl_is_blocked(&li->sasl, ڈیٹا);
  واپس نتیجہ;
}

#اگر_تعریف_شدہ USE_SSL
ساکن صحیح ldapsb_tls_تیاری(Sockbuf_IO_Desc *sbiod, خالی *arg)
{
  sbiod->sbiod_pvt = arg;
  واپس 0;
}

ساکن صحیح ldapsb_tls_remove(Sockbuf_IO_Desc *sbiod)
{
  sbiod->sbiod_pvt = تہی;
  واپس 0;
}

/* if there is a duplicated socket to close, close it */
ساکن صحیح ldapsb_tls_close(Sockbuf_IO_Desc *sbiod)
{
  ساخت Curl_easy *ڈیٹا = sbiod->sbiod_pvt;
  اگر(ڈیٹا) {
    ساخت connectdata *conn = ڈیٹا->conn;
    اگر(conn) {
      ساخت ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      اگر(li && (li->dupfd != CURL_SOCKET_BAD)) {
        sclose(li->dupfd);
        li->dupfd = CURL_SOCKET_BAD;
      }
    }
  }

  واپس 0;
}

ساکن صحیح ldapsb_tls_ctrl(Sockbuf_IO_Desc *sbiod, صحیح opt, خالی *arg)
{
  (خالی)arg;
  اگر(opt == LBER_SB_OPT_ڈیٹا_READY) {
    ساخت Curl_easy *ڈیٹا = sbiod->sbiod_pvt;
    واپس Curl_conn_ڈیٹا_pending(ڈیٹا, FIRSTSOCKET);
  }
  واپس 0;
}

ساکن ber_slen_t ldapsb_tls_read(Sockbuf_IO_Desc *sbiod, خالی *buf,
                                  ber_len_t len)
{
  ساخت Curl_easy *ڈیٹا = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  اگر(ڈیٹا) {
    ساخت connectdata *conn = ڈیٹا->conn;
    اگر(conn) {
      ساخت ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode نتیجہ = CURLE_RECV_ERROR;
      size_t nread;

      اگر(!li) {
        سیٹ_SOCKERRNO(SOCKEINVAL);
        واپس -1;
      }
      نتیجہ = (li->recv)(ڈیٹا, FIRSTSOCKET, buf, len, &nread);
      اگر(نتیجہ == CURLE_AGAIN) {
        سیٹ_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = نتیجہ ? -1 : (ber_slen_t)nread;
    }
  }
  واپس ret;
}

ساکن ber_slen_t ldapsb_tls_write(Sockbuf_IO_Desc *sbiod, خالی *buf,
                                   ber_len_t len)
{
  ساخت Curl_easy *ڈیٹا = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  اگر(ڈیٹا) {
    ساخت connectdata *conn = ڈیٹا->conn;
    اگر(conn) {
      ساخت ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode نتیجہ = CURLE_SEND_ERROR;
      size_t nwritten;

      اگر(!li) {
        سیٹ_SOCKERRNO(SOCKEINVAL);
        واپس -1;
      }
      نتیجہ = (li->send)(ڈیٹا, FIRSTSOCKET, buf, len, FALSE, &nwritten);
      اگر(نتیجہ == CURLE_AGAIN) {
        سیٹ_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = نتیجہ ? -1 : (ber_slen_t)nwritten;
    }
  }
  واپس ret;
}

ساکن Sockbuf_IO ldapsb_tls = {
  ldapsb_tls_تیاری,
  ldapsb_tls_remove,
  ldapsb_tls_ctrl,
  ldapsb_tls_read,
  ldapsb_tls_write,
  ldapsb_tls_close
};

ساکن bool ssl_installed(ساخت connectdata *conn)
{
  ساخت ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  واپس li && li->recv;
}

ساکن CURLcode oldap_ssl_connect(ساخت Curl_easy *ڈیٹا, ldapstate newstate)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  bool ssldone = FALSE;
  CURLcode نتیجہ;

  اگر(!li)
    واپس CURLE_FAILED_INIT;
  نتیجہ = Curl_conn_connect(ڈیٹا, FIRSTSOCKET, FALSE, &ssldone);
  اگر(نتیجہ)
    واپس نتیجہ;
  oldap_state(ڈیٹا, li, newstate);

  اگر(ssldone) {
    Sockbuf *sb;

    /* Install the libcurl SSL handlers into the sockbuf. */
    اگر((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_جوڑو_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, ڈیٹا))
      واپس CURLE_FAILED_INIT;
    li->recv = conn->recv[FIRSTSOCKET];
    li->send = conn->send[FIRSTSOCKET];
  }

  واپس نتیجہ;
}

/* Send the STARTTLS request */
ساکن CURLcode oldap_perform_starttls(ساخت Curl_easy *ڈیٹا)
{
  ساخت ldapconninfo *li =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_LDAP_CONN);
  صحیح rc;

  اگر(!li)
    واپس CURLE_FAILED_INIT;
  rc = ldap_آغاز_tls(li->ld, تہی, تہی, &li->msgid);
  اگر(rc != LDAP_SUCCESS)
    واپس oldap_map_error(rc, CURLE_USE_SSL_FAILED);
  oldap_state(ڈیٹا, li, OLDAP_STARTTLS);
  واپس CURLE_OK;
}
#اگر_ختم

ساکن خالی oldap_easy_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت ldapreqinfo *lr = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  curlx_free(lr);
}

ساکن خالی oldap_conn_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت ldapconninfo *li = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  اگر(li->ld) {
    ldap_unbind_ext(li->ld, تہی, تہی);
    li->ld = تہی;
  }
  curlx_free(li);
}

/* SASL parameters for the ldap protocol */
ساکن ثابت ساخت SASLproto saslldap = {
  "ldap",                     /* The service name */
  oldap_perform_auth,         /* Send authentication command */
  oldap_continue_auth,        /* Send authentication continuation */
  oldap_cancel_auth,          /* Send authentication cancellation */
  oldap_get_پیغام,          /* Get SASL response message */
  0,                          /* Maximum initial response length (no max) */
  LDAP_SASL_BIND_اندر_پیش_رفت, /* Code received when continuation is expected */
  LDAP_SUCCESS,               /* Code to receive upon authentication success */
  SASL_AUTH_NONE,             /* Default mechanisms */
  0                           /* Configuration flags */
};

ساکن CURLcode oldap_connect(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ldapconninfo *li;
  ساکن ثابت صحیح نسخہ_2 = LDAP_VERSION3;
  حرف *hosturl = تہی;
  CURLcode نتیجہ;
  صحیح rc;
#اگر_تعریف_شدہ CURL_OPENLDAP_DEBUG
  ساکن صحیح do_trace = -1;
#اگر_ختم

  (خالی)مکمل;

  li = curlx_calloc(1, حجم(ساخت ldapconninfo));
  اگر(!li) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  li->dupfd = CURL_SOCKET_BAD;

  نتیجہ = Curl_conn_meta_سیٹ(conn, CURL_META_LDAP_CONN, li, oldap_conn_dtor);
  اگر(نتیجہ)
    جاؤ برآمد;

  li->proto = ldap_pvt_url_scheme2proto(ڈیٹا->state.origin->scheme->name);

  /* Initialize the SASL storage */
  Curl_sasl_init(&li->sasl, ڈیٹا, &saslldap);

  نتیجہ = oldap_parse_login_options(conn);
  اگر(نتیجہ)
    جاؤ برآمد;

  hosturl = curl_maprintf("%s://%s:%u",
                          conn->scheme->name,
                          conn->origin->ipv6 ?
                          conn->origin->user_hostname :
                          conn->origin->hostname,
                          conn->origin->پورٹ);
  اگر(!hosturl) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  {
#اگر_تعریف_شدہ _WIN32
    SOCKET dupfd = INVALID_SOCKET;
    WSAPROTOCOL_INFO pi;
    اگر(WSADuplicateSocket(conn->sock[FIRSTSOCKET], GetCurrentProcessId(),
                          &pi))
      ; /* error */
    ورنہ
      dupfd = CURL_SOCKET(pi.i_پتہ_Family, pi.iSocketType, pi.iProtocol);
#ورنہ
#اگر_تعریف_شدہ F_DUPFD_CLOEXEC
    صحیح dupfd = fcntl(conn->sock[FIRSTSOCKET], F_DUPFD_CLOEXEC, 0);
#ورنہ
    صحیح dupfd = dup(conn->sock[FIRSTSOCKET]);
#اگر_تعریف_شدہ HAVE_FCNTL
    اگر((dupfd != CURL_SOCKET_BAD) &&
       (fcntl(dupfd, F_SETFD, FD_CLOEXEC) < 0)) {
      sclose(dupfd);
      dupfd = CURL_SOCKET_BAD;
    }
#اگر_ختم /* HAVE_FCNTL */
#اگر_ختم /* F_DUPFD_CLOEXEC */
#اگر_ختم /* _WIN32 */

    اگر(dupfd == CURL_SOCKET_BAD) {
      نتیجہ = CURLE_COULDNT_CONNECT;
      جاؤ برآمد;
    }
    rc = ldap_init_fd((ber_socket_t)dupfd, li->proto, hosturl, &li->ld);
    اگر(rc)
      sclose(dupfd);
    ورنہ
      /* store the socket, as when this is used by TLS we close this ourselves
         and at this point the code does not yet know if this will use TLS or
         not */
      li->dupfd = dupfd;
  }
  اگر(rc) {
    failf(ڈیٹا, "LDAP local: Cannot connect to %s, %s",
          hosturl, ldap_err2string(rc));
    نتیجہ = CURLE_COULDNT_CONNECT;
    جاؤ برآمد;
  }

#اگر_تعریف_شدہ CURL_OPENLDAP_DEBUG
  اگر(do_trace < 0) {
    ثابت حرف *env = getenv("CURL_OPENLDAP_TRACE");
    curl_off_t e = 0;
    اگر(!curlx_str_نمبر(&env, &e, زیادہ_سے_زیادہ_عدد))
      do_trace = e > 0;
  }
  اگر(do_trace)
    ldap_سیٹ_option(li->ld, LDAP_OPT_DEBUG_LEVEL, &do_trace);
#اگر_ختم

  /* Try version 3 first. */
  ldap_سیٹ_option(li->ld, LDAP_OPT_PROTOCOL_نسخہ, &نسخہ_2);

  /* Do not chase referrals. */
  ldap_سیٹ_option(li->ld, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  {
    ber_len_t max = 256 * 1024;
    Sockbuf *sb;
    اگر((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       /* Set the maximum allowed size of an incoming message, which to
          OpenLDAP means that it will malloc() memory up to this size. If not
          set, there is no limit and we instead risk a malloc() failure. */
       !ber_sockbuf_ctrl(sb, LBER_SB_OPT_سیٹ_MAX_INCOMING, &max)) {
      نتیجہ = CURLE_FAILED_INIT;
      جاؤ برآمد;
    }
  }

#اگر_تعریف_شدہ USE_SSL
  اگر(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    نتیجہ = oldap_ssl_connect(ڈیٹا, OLDAP_SSL);
    جاؤ برآمد;
  }

  اگر(ڈیٹا->سیٹ.use_ssl) {
    نتیجہ = oldap_perform_starttls(ڈیٹا);
    اگر(!نتیجہ || ڈیٹا->سیٹ.use_ssl != CURLUSESSL_TRY)
      جاؤ برآمد;
  }
#اگر_ختم

  اگر(li->sasl.prefmech != SASL_AUTH_NONE) {
    نتیجہ = oldap_perform_mechs(ڈیٹا);
    جاؤ برآمد;
  }

  /* Force bind even if anonymous bind is not needed in protocol version 3
     to detect missing version 3 support. */
  نتیجہ = oldap_perform_bind(ڈیٹا, OLDAP_BIND);

برآمد:
  curlx_free(hosturl);
  واپس نتیجہ;
}

/* Handle the supported SASL mechanisms query response */
ساکن CURLcode oldap_state_mechs_resp(ساخت Curl_easy *ڈیٹا,
                                       LDAPMessage *پیغام, صحیح رمز)
{
  ساخت connectdata *conn;
  ساخت ldapconninfo *li;
  صحیح rc;
  BerElement *ber = تہی;
  CURLcode نتیجہ = CURLE_OK;
  ساخت berval bv, *bvals;

  اگر(!ڈیٹا)
    واپس CURLE_FAILED_INIT;

  conn = ڈیٹا->conn;
  li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);

  اگر(!li)
    واپس CURLE_FAILED_INIT;
  انتخاب(ldap_msgtype(پیغام)) {
  حالت LDAP_RES_تلاش_اندراج:
    /* Got a list of supported SASL mechanisms. */
    اگر(رمز != LDAP_SUCCESS && رمز != LDAP_NO_RESULTS_RETURNED)
      واپس CURLE_LOGIN_DENIED;

    rc = ldap_get_dn_ber(li->ld, پیغام, &ber, &bv);
    اگر(rc < 0)
      واپس oldap_map_error(rc, CURLE_BAD_CONTENT_ENCODING);
    برائے(rc = ldap_get_attribute_ber(li->ld, پیغام, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, پیغام, ber, &bv, &bvals)) {
      صحیح i;

      اگر(!bv.bv_val)
        توڑو;

      اگر(bvals) {
        برائے(i = 0; bvals[i].bv_val; i++) {
          size_t llen;
          غیرعلامتی مختصر mech =
            Curl_sasl_decode_mech((ثابت حرف *)bvals[i].bv_val,
                                  bvals[i].bv_len, &llen);
          اگر(bvals[i].bv_len == llen)
            li->sasl.authmechs |= mech;
        }
        ber_memfree(bvals);
      }
    }
    ber_free(ber, 0);
    توڑو;

  حالت LDAP_RES_تلاش_نتیجہ:
    انتخاب(رمز) {
    حالت LDAP_SIZELIMIT_EXCEEDED:
      infof(ڈیٹا, "Too many authentication mechanisms");
      FALLTHROUGH();
    حالت LDAP_SUCCESS:
    حالت LDAP_NO_RESULTS_RETURNED:
      اگر(Curl_sasl_can_authenticate(&li->sasl, ڈیٹا))
        نتیجہ = oldap_perform_sasl(ڈیٹا);
      ورنہ
        نتیجہ = CURLE_LOGIN_DENIED;
      توڑو;
    طےشدہ:
      نتیجہ = oldap_map_error(رمز, CURLE_LOGIN_DENIED);
      توڑو;
    }
    توڑو;
  طےشدہ:
    توڑو;
  }
  واپس نتیجہ;
}

/* Handle a SASL bind response. */
ساکن CURLcode oldap_state_sasl_resp(ساخت Curl_easy *ڈیٹا,
                                      LDAPMessage *پیغام, صحیح رمز)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode نتیجہ = CURLE_OK;
  saslprogress پیش_رفت;
  صحیح rc;

  اگر(!li)
    واپس CURLE_FAILED_INIT;
  li->servercred = تہی;
  rc = ldap_parse_sasl_bind_نتیجہ(li->ld, پیغام, &li->servercred, 0);
  اگر(rc != LDAP_SUCCESS) {
    failf(ڈیٹا, "LDAP local: sasl ldap_parse_result %s", ldap_err2string(rc));
    نتیجہ = oldap_map_error(rc, CURLE_LOGIN_DENIED);
  }
  ورنہ {
    نتیجہ = Curl_sasl_continue(&li->sasl, ڈیٹا, رمز, &پیش_رفت);
    اگر(!نتیجہ) {
      انتخاب(پیش_رفت) {
      حالت SASL_مکمل:
        oldap_state(ڈیٹا, li, OLDAP_STOP);   /* Authenticated */
        توڑو;
      حالت SASL_IDLE:            /* No mechanism left after cancellation */
        failf(ڈیٹا, "Authentication cancelled");
        نتیجہ = CURLE_LOGIN_DENIED;
        توڑو;
      طےشدہ:
        توڑو;
      }
    }
  }

  اگر(li->servercred)
    ber_bvfree(li->servercred);
  واپس نتیجہ;
}

/* Handle a simple bind response. */
ساکن CURLcode oldap_state_bind_resp(ساخت Curl_easy *ڈیٹا, LDAPMessage *پیغام,
                                      صحیح رمز)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode نتیجہ = CURLE_OK;
  ساخت berval *bv = تہی;
  صحیح rc;

  اگر(!li)
    واپس CURLE_FAILED_INIT;

  اگر(رمز != LDAP_SUCCESS)
    واپس oldap_map_error(رمز, CURLE_LDAP_CANNOT_BIND);

  rc = ldap_parse_sasl_bind_نتیجہ(li->ld, پیغام, &bv, 0);
  اگر(rc != LDAP_SUCCESS) {
    failf(ڈیٹا, "LDAP local: bind ldap_parse_sasl_bind_result %s",
          ldap_err2string(rc));
    نتیجہ = oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  }
  ورنہ
    oldap_state(ڈیٹا, li, OLDAP_STOP);

  اگر(bv)
    ber_bvfree(bv);
  واپس نتیجہ;
}

ساکن CURLcode oldap_connecting(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  LDAPMessage *پیغام = تہی;
  ساخت timeval tv = { 0, 0 };
  صحیح رمز = LDAP_SUCCESS;
  صحیح rc;

  اگر(!li)
    واپس CURLE_FAILED_INIT;

  اگر(li->state != OLDAP_SSL && li->state != OLDAP_TLS) {
    /* Get response to last command. */
    rc = ldap_نتیجہ(li->ld, li->msgid, LDAP_پیغام_ONE, &tv, &پیغام);
    انتخاب(rc) {
    حالت 0:                               /* Timed out. */
      واپس CURLE_OK;
    حالت LDAP_RES_تلاش_اندراج:
    حالت LDAP_RES_تلاش_REFERENCE:
      توڑو;
    طےشدہ:
      li->msgid = 0;                      /* Nothing to abandon upon error. */
      اگر(rc < 0) {
        failf(ڈیٹا, "LDAP local: connecting ldap_result %s",
              ldap_err2string(rc));
        واپس oldap_map_error(rc, CURLE_COULDNT_CONNECT);
      }
      توڑو;
    }

    /* Get error code from message. */
    rc = ldap_parse_نتیجہ(li->ld, پیغام, &رمز, تہی, تہی, تہی, تہی, 0);
    اگر(rc)
      رمز = rc;
    ورنہ {
      /* store the latest code for later retrieval */
      ڈیٹا->info.httpcode = رمز;
    }

    /* If protocol version 3 is not supported, fallback to version 2. */
    اگر(رمز == LDAP_PROTOCOL_ERROR && li->state != OLDAP_BINDV2 &&
#اگر_تعریف_شدہ USE_SSL
       (ssl_installed(conn) || ڈیٹا->سیٹ.use_ssl <= CURLUSESSL_TRY) &&
#اگر_ختم
       li->sasl.prefmech == SASL_AUTH_NONE) {
      ساکن ثابت صحیح نسخہ_2 = LDAP_VERSION2;

      ldap_سیٹ_option(li->ld, LDAP_OPT_PROTOCOL_نسخہ, &نسخہ_2);
      ldap_msgfree(پیغام);
      واپس oldap_perform_bind(ڈیٹا, OLDAP_BINDV2);
    }
  }

  /* Handle response message according to current state. */
  انتخاب(li->state) {

#اگر_تعریف_شدہ USE_SSL
  حالت OLDAP_SSL:
    نتیجہ = oldap_ssl_connect(ڈیٹا, OLDAP_SSL);
    اگر(!نتیجہ && ssl_installed(conn)) {
      اگر(li->sasl.prefmech != SASL_AUTH_NONE)
        نتیجہ = oldap_perform_mechs(ڈیٹا);
      ورنہ
        نتیجہ = oldap_perform_bind(ڈیٹا, OLDAP_BIND);
    }
    توڑو;
  حالت OLDAP_STARTTLS:
    اگر(رمز != LDAP_SUCCESS) {
      اگر(ڈیٹا->سیٹ.use_ssl != CURLUSESSL_TRY)
        نتیجہ = oldap_map_error(رمز, CURLE_USE_SSL_FAILED);
      ورنہ اگر(li->sasl.prefmech != SASL_AUTH_NONE)
        نتیجہ = oldap_perform_mechs(ڈیٹا);
      ورنہ
        نتیجہ = oldap_perform_bind(ڈیٹا, OLDAP_BIND);
      توڑو;
    }
    نتیجہ = Curl_ssl_cfilter_جوڑو(
      ڈیٹا, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    اگر(نتیجہ)
      توڑو;
    FALLTHROUGH();
  حالت OLDAP_TLS:
    نتیجہ = oldap_ssl_connect(ڈیٹا, OLDAP_TLS);
    اگر(نتیجہ)
      نتیجہ = oldap_map_error(رمز, CURLE_USE_SSL_FAILED);
    ورنہ اگر(ssl_installed(conn)) {
      اگر(li->sasl.prefmech != SASL_AUTH_NONE)
        نتیجہ = oldap_perform_mechs(ڈیٹا);
      ورنہ اگر(ڈیٹا->state.creds)
        نتیجہ = oldap_perform_bind(ڈیٹا, OLDAP_BIND);
      ورنہ {
        /* Version 3 supported: no bind required */
        oldap_state(ڈیٹا, li, OLDAP_STOP);
        نتیجہ = CURLE_OK;
      }
    }
    توڑو;
#اگر_ختم

  حالت OLDAP_MECHS:
    نتیجہ = oldap_state_mechs_resp(ڈیٹا, پیغام, رمز);
    توڑو;
  حالت OLDAP_SASL:
    نتیجہ = oldap_state_sasl_resp(ڈیٹا, پیغام, رمز);
    توڑو;
  حالت OLDAP_BIND:
  حالت OLDAP_BINDV2:
    نتیجہ = oldap_state_bind_resp(ڈیٹا, پیغام, رمز);
    توڑو;
  طےشدہ:
    /* internal error */
    نتیجہ = CURLE_COULDNT_CONNECT;
    توڑو;
  }

  ldap_msgfree(پیغام);

  *مکمل = li->state == OLDAP_STOP;
  اگر(*مکمل)
    conn->recv[FIRSTSOCKET] = oldap_recv;

  اگر(نتیجہ && li->msgid) {
    ldap_abandon_ext(li->ld, li->msgid, تہی, تہی);
    li->msgid = 0;
  }
  واپس نتیجہ;
}

ساکن CURLcode oldap_disconnect(ساخت Curl_easy *ڈیٹا,
                                 ساخت connectdata *conn,
                                 bool dead_connection)
{
  ساخت ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  (خالی)dead_connection;
#اگر_غیر_تعریف_شدہ USE_SSL
  (خالی)ڈیٹا;
#اگر_ختم

  اگر(li && li->ld) {
#اگر_تعریف_شدہ USE_SSL
    اگر(ssl_installed(conn)) {
      Sockbuf *sb;
      اگر(ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS ||
         ber_sockbuf_جوڑو_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, ڈیٹا))
        واپس CURLE_FAILED_INIT;
    }
#اگر_ختم
    ldap_unbind_ext(li->ld, تہی, تہی);
    li->ld = تہی;
  }
  واپس CURLE_OK;
}

ساکن CURLcode oldap_do(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ساخت ldapreqinfo *lr;
  CURLcode نتیجہ;
  صحیح rc;
  LDAPURLDesc *lud;
  صحیح msgid;

  اگر(!li)
    واپس CURLE_FAILED_INIT;

  infof(ڈیٹا, "LDAP local: %s", Curl_bufref_ptr(&ڈیٹا->state.url));

  نتیجہ = oldap_url_parse(ڈیٹا, &lud);
  اگر(نتیجہ)
    جاؤ برآمد;

#اگر_تعریف_شدہ USE_SSL
  اگر(ssl_installed(conn)) {
    Sockbuf *sb;
    /* re-install the libcurl SSL handlers into the sockbuf. */
    اگر((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_جوڑو_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, ڈیٹا)) {
      ldap_free_urldesc(lud);
      واپس CURLE_FAILED_INIT;
    }
  }
#اگر_ختم

  rc = ldap_تلاش_ext(li->ld, lud->lud_dn, lud->lud_scope,
                       lud->lud_filter, lud->lud_attrs, 0,
                       تہی, تہی, تہی, 0, &msgid);
  ldap_free_urldesc(lud);
  اگر(rc != LDAP_SUCCESS) {
    failf(ڈیٹا, "LDAP local: ldap_search_ext %s", ldap_err2string(rc));
    نتیجہ = CURLE_LDAP_تلاش_FAILED;
    جاؤ برآمد;
  }

  lr = curlx_calloc(1, حجم(ساخت ldapreqinfo));
  اگر(!lr ||
     Curl_meta_سیٹ(ڈیٹا, CURL_META_LDAP_EASY, lr, oldap_easy_dtor)) {
    ldap_abandon_ext(li->ld, msgid, تہی, تہی);
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  lr->msgid = msgid;
  Curl_xfer_تیاری_recv(ڈیٹا, FIRSTSOCKET, -1);
  *مکمل = TRUE;

برآمد:
  واپس نتیجہ;
}

ساکن CURLcode oldap_مکمل(ساخت Curl_easy *ڈیٹا, CURLcode res,
                           bool premature)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ldapreqinfo *lr = Curl_meta_get(ڈیٹا, CURL_META_LDAP_EASY);

  (خالی)res;
  (خالی)premature;

  اگر(lr) {
    /* if there was a search in progress, abandon it */
    اگر(lr->msgid) {
      ساخت ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      اگر(li && li->ld) {
        ldap_abandon_ext(li->ld, lr->msgid, تہی, تہی);
      }
      lr->msgid = 0;
    }
    Curl_meta_remove(ڈیٹا, CURL_META_LDAP_EASY);
  }

  واپس CURLE_OK;
}

ساکن CURLcode client_write(ساخت Curl_easy *ڈیٹا,
                             ثابت حرف *سابقہ, size_t plen,
                             ثابت حرف *قدر, size_t len,
                             ثابت حرف *suffix, size_t slen)
{
  CURLcode نتیجہ = CURLE_OK;

  اگر(سابقہ) {
    /* If we have a zero-length value and the prefix ends with a space
       separator, drop the latter. */
    اگر(!len && plen && سابقہ[plen - 1] == ' ')
      plen--;
    نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, سابقہ, plen);
  }
  اگر(!نتیجہ && قدر) {
    نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, قدر, len);
  }
  اگر(!نتیجہ && suffix) {
    نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, suffix, slen);
  }
  واپس نتیجہ;
}

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
ساکن bool oldap_str_needs_base64(ثابت حرف *s, size_t len)
{
  size_t i;
  اگر(!len)
    واپس FALSE;
  اگر((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    واپس TRUE;
  برائے(i = 0; i < len; i++)
    اگر(!ISPRINT(s[i]))
      واپس TRUE;
  واپس FALSE;
}

ساکن CURLcode oldap_recv(ساخت Curl_easy *ڈیٹا, int8_t sockindex, حرف *buf,
                           size_t len, size_t *pnread)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ساخت ldapreqinfo *lr = Curl_meta_get(ڈیٹا, CURL_META_LDAP_EASY);
  صحیح rc;
  LDAPMessage *پیغام = تہی;
  BerElement *ber = تہی;
  ساخت timeval tv = { 0, 0 };
  ساخت berval bv, *bvals;
  CURLcode نتیجہ = CURLE_AGAIN;
  صحیح رمز;
  حرف *info = تہی;

  (خالی)len;
  (خالی)buf;
  (خالی)sockindex;
  *pnread = 0;
  اگر(!li || !lr)
    واپس CURLE_FAILED_INIT;

  rc = ldap_نتیجہ(li->ld, lr->msgid, LDAP_پیغام_ONE, &tv, &پیغام);
  اگر(rc < 0) {
    failf(ڈیٹا, "LDAP local: search ldap_result %s", ldap_err2string(rc));
    نتیجہ = CURLE_RECV_ERROR;
  }

  /* error or timed out */
  اگر(!پیغام)
    واپس نتیجہ;

  نتیجہ = CURLE_OK;

  انتخاب(ldap_msgtype(پیغام)) {
  حالت LDAP_RES_تلاش_نتیجہ:
    lr->msgid = 0;
    rc = ldap_parse_نتیجہ(li->ld, پیغام, &رمز, تہی, &info, تہی, تہی, 0);
    اگر(rc) {
      failf(ڈیٹا, "LDAP local: search ldap_parse_result %s",
            ldap_err2string(rc));
      نتیجہ = CURLE_LDAP_تلاش_FAILED;
      توڑو;
    }

    /* store the latest code for later retrieval */
    ڈیٹا->info.httpcode = رمز;

    انتخاب(رمز) {
    حالت LDAP_SIZELIMIT_EXCEEDED:
      infof(ڈیٹا, "There are more than %d entries", lr->nument);
      FALLTHROUGH();
    حالت LDAP_SUCCESS:
      ڈیٹا->req.سائز = ڈیٹا->req.bytecount;
      توڑو;
    طےشدہ:
      failf(ڈیٹا, "LDAP remote: search failed %s %s", ldap_err2string(رمز),
            info ? info : "");
      نتیجہ = CURLE_LDAP_تلاش_FAILED;
      توڑو;
    }
    اگر(info)
      ldap_memfree(info);
    توڑو;
  حالت LDAP_RES_تلاش_اندراج:
    lr->nument++;
    rc = ldap_get_dn_ber(li->ld, پیغام, &ber, &bv);
    اگر(rc < 0) {
      نتیجہ = CURLE_RECV_ERROR;
      توڑو;
    }

    اگر(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
      حرف *dn_b64 = تہی;
      size_t dn_b64_sz = 0;

      /* A DN with e.g. CR/LF would inject LDIF lines, so base64-encode it. */
      نتیجہ = curlx_base64_encode((uint8_t *)bv.bv_val, bv.bv_len,
                                   &dn_b64, &dn_b64_sz);
      اگر(!نتیجہ)
        نتیجہ = client_write(ڈیٹا, STRCONST("DN:: "), dn_b64, dn_b64_sz,
                              STRCONST("\n"));
      curlx_free(dn_b64);
    }
    ورنہ
      نتیجہ = client_write(ڈیٹا, STRCONST("DN: "), bv.bv_val, bv.bv_len,
                            STRCONST("\n"));
    اگر(نتیجہ)
      توڑو;

    برائے(rc = ldap_get_attribute_ber(li->ld, پیغام, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, پیغام, ber, &bv, &bvals)) {
      صحیح i;
      bool ثنائی;

      اگر(!bv.bv_val)
        توڑو;

      /* An attribute type has no base64 form in LDIF, so a type carrying
         control bytes (CR/LF etc.) would let a hostile server inject lines.
         A conformant type name never contains them, so reject it. */
      اگر(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
        failf(ڈیٹا, "LDAP local: attribute type contains illegal bytes");
        اگر(bvals)
          ber_memfree(bvals);
        نتیجہ = CURLE_WEIRD_SERVER_جواب;
        توڑو;
      }

      اگر(!bvals) {
        نتیجہ = client_write(ڈیٹا, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":\n"));
        اگر(نتیجہ)
          توڑو;
        جاری;
      }

      ثنائی = bv.bv_len > 7 &&
        curl_strnequal(bv.bv_val + bv.bv_len - 7, ";binary", 7);

      برائے(i = 0; bvals[i].bv_val; i++) {
        bool binval = FALSE;

        نتیجہ = client_write(ڈیٹا, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":"));
        اگر(نتیجہ)
          توڑو;

        اگر(!ثنائی)
          binval = oldap_str_needs_base64(bvals[i].bv_val, bvals[i].bv_len);
        اگر(ثنائی || binval) {
          حرف *val_b64 = تہی;
          size_t val_b64_sz = 0;

          /* Binary value, encode to base64. */
          اگر(bvals[i].bv_len)
            نتیجہ = curlx_base64_encode((uint8_t *)bvals[i].bv_val,
                                         bvals[i].bv_len,
                                         &val_b64, &val_b64_sz);
          اگر(!نتیجہ)
            نتیجہ = client_write(ڈیٹا, STRCONST(": "), val_b64, val_b64_sz,
                                  STRCONST("\n"));
          curlx_free(val_b64);
        }
        ورنہ
          نتیجہ = client_write(ڈیٹا, STRCONST(" "),
                                bvals[i].bv_val, bvals[i].bv_len,
                                STRCONST("\n"));
        اگر(نتیجہ)
          توڑو;
      }

      ber_memfree(bvals);
      bvals = تہی;
      اگر(!نتیجہ)
        نتیجہ = client_write(ڈیٹا, STRCONST("\n"), تہی, 0, تہی, 0);
      اگر(نتیجہ)
        توڑو;
    }

    اگر(!نتیجہ)
      نتیجہ = client_write(ڈیٹا, STRCONST("\n"), تہی, 0, تہی, 0);
    اگر(!نتیجہ)
      نتیجہ = CURLE_AGAIN;
    توڑو;
  }

  ber_free(ber, 0);
  ldap_msgfree(پیغام);
  واپس نتیجہ;
}

خالی Curl_ldap_نسخہ(حرف *buf, size_t bufsz)
{
  LDAPAPIInfo api;
  api.ldapai_info_نسخہ = LDAP_API_INFO_نسخہ;

  اگر(ldap_get_option(تہی, LDAP_OPT_API_INFO, &api) == LDAP_OPT_SUCCESS) {
    غیرعلامتی صحیح patch = (غیرعلامتی صحیح)(api.ldapai_vendor_نسخہ % 100);
    غیرعلامتی صحیح major = (غیرعلامتی صحیح)(api.ldapai_vendor_نسخہ / 10000);
    غیرعلامتی صحیح minor =
      (((غیرعلامتی صحیح)api.ldapai_vendor_نسخہ - (major * 10000))
       - patch) / 100;
    curl_msnprintf(buf, bufsz, "%s/%u.%u.%u",
                   api.ldapai_vendor_name, major, minor, patch);
    ldap_memfree(api.ldapai_vendor_name);
    ber_memvfree((خالی **)api.ldapai_extensions);
  }
  ورنہ
    curl_msnprintf(buf, bufsz, "OpenLDAP");
}

/*
 * LDAP protocol handler.
 */
ثابت ساخت Curl_protocol Curl_protocol_ldap = {
  oldap_تیاری_connection,               /* setup_connection */
  oldap_do,                             /* do_it */
  oldap_مکمل,                           /* done */
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

#اگر_ختم /* !CURL_DISABLE_LDAP && USE_OPENLDAP */

/* The LDAP scheme structs are in ldap.c */
