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

#إذا !defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP)

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
  OLDAP_آخری            /* Never used */
} ldapstate;

#جے_بنا_تعریف _LDAP_PVT_H
خارجي صحيح ldap_pvt_url_scheme2proto(ثابت حرف *);
خارجي صحيح ldap_init_fd(ber_socket_t fd, صحيح proto, ثابت حرف *url,
                        LDAP **ld);
#جے_انت

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
#تعریف_کرو CURL_META_LDAP_EASY   "meta:proto:ldap:easy"
/* meta key for storing ldapconninfo at connection */
#تعریف_کرو CURL_META_LDAP_CONN   "meta:proto:ldap:conn"

/*
 * oldap_state()
 *
 * This is the ONLY way to change LDAP state!
 */
ساكن فراغ oldap_state(بنية Curl_easy *انکڑے, بنية ldapconninfo *li,
                        ldapstate newstate)
{
#إذا defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ساكن ثابت حرف * ثابت ناں[] = {
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

  إذا(li->state != newstate)
    infof(انکڑے, "LDAP %p state change from %s to %s",
          (فراغ *)li, ناں[li->state], ناں[newstate]);
#وإلا
  (فراغ)انکڑے;
#جے_انت
  li->state = newstate;
}

/* Map some particular LDAP error codes to CURLcode values. */
ساكن CURLcode oldap_map_error(صحيح rc, CURLcode نتیجہ)
{
  بدل(rc) {
  حالة LDAP_NO_حافظہ:
    أرجع CURLE_باہرلا_دا_حافظہ;
  حالة LDAP_INVALID_CREDENTIALS:
    أرجع CURLE_LOGIN_DENIED;
  حالة LDAP_PROTOCOL_ERROR:
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  حالة LDAP_INSUFFICIENT_ACCESS:
    أرجع CURLE_REMOTE_ACCESS_DENIED;
  }
  أرجع نتیجہ;
}

ساكن CURLcode oldap_url_parse(بنية Curl_easy *انکڑے, LDAPURLDesc **ludp)
{
  CURLcode نتیجہ = CURLE_OK;
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
  إذا((انکڑے->state.creds && (انکڑے->state.creds->سروت == CREDS_URL)) ||
     انکڑے->state.اتے.options)
    rc = LDAP_URL_ERR_BADURL;
  وإلا
    rc = ldap_url_parse(Curl_bufref_ptr(&انکڑے->state.url), ludp);

  إذا(rc != LDAP_URL_SUCCESS) {
    ثابت حرف *پیغام = "url parsing problem";

    نتیجہ = rc == LDAP_URL_ERR_MEM ? CURLE_باہرلا_دا_حافظہ :
      CURLE_URL_MALFORMAT;
    rc -= LDAP_URL_SUCCESS;
    إذا((size_t)rc < CURL_ARRAYSIZE(url_errs))
      پیغام = url_errs[rc];
    failf(انکڑے, "LDAP local: %s", پیغام);
  }
  أرجع نتیجہ;
}

/* Parse the login options. */
ساكن CURLcode oldap_parse_login_options(بنية connectdata *conn)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ثابت حرف *ptr = conn->options;

  DEBUGASSERT(li);
  إذا(!li)
    أرجع CURLE_FAILED_INIT;

  طالما(!نتیجہ && ptr && *ptr) {
    ثابت حرف *کلید = ptr;
    ثابت حرف *قدر;

    طالما(*ptr && *ptr != '=')
      ptr++;

    قدر = ptr + 1;

    طالما(*ptr && *ptr != ';')
      ptr++;

    إذا(checkprefix("AUTH=", کلید))
      نتیجہ = Curl_sasl_parse_url_auth_option(&li->sasl, قدر, ptr - قدر);
    وإلا
      نتیجہ = CURLE_SETOPT_OPTION_SYNTAX;

    إذا(*ptr == ';')
      ptr++;
  }

  أرجع نتیجہ == CURLE_URL_MALFORMAT ? CURLE_SETOPT_OPTION_SYNTAX : نتیجہ;
}

ساكن CURLcode oldap_تیاری_connection(بنية Curl_easy *انکڑے,
                                       بنية connectdata *conn)
{
  CURLcode نتیجہ;
  LDAPURLDesc *lud;
  (فراغ)conn;

  /* Early URL syntax check. */
  نتیجہ = oldap_url_parse(انکڑے, &lud);
  ldap_free_urldesc(lud);

  أرجع نتیجہ;
}

/*
 * Get the SASL authentication challenge from the server credential buffer.
 */
ساكن CURLcode oldap_get_سنیہا(بنية Curl_easy *انکڑے, بنية bufref *باہرلا)
{
  بنية ldapconninfo *li =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_LDAP_CONN);
  بنية berval *servercred = li ? li->servercred : عدم;
  DEBUGASSERT(li);
  إذا(!li)
    أرجع CURLE_FAILED_INIT;

  إذا(!servercred || !servercred->bv_val)
    أرجع CURLE_WEIRD_SERVER_جواب;
  Curl_bufref_سیٹ(باہرلا, servercred->bv_val, servercred->bv_len, عدم);
  أرجع CURLE_OK;
}

/*
 * Sends an initial SASL bind request to the server.
 */
ساكن CURLcode oldap_perform_auth(بنية Curl_easy *انکڑے, ثابت حرف *mech,
                                   ثابت بنية bufref *initresp)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  بنية berval cred;
  بنية berval *pcred = &cred;
  صحيح rc;

  DEBUGASSERT(li);
  إذا(!li)
    أرجع CURLE_FAILED_INIT;
  cred.bv_val = (حرف *)CURL_UNCONST(Curl_bufref_ptr(initresp));
  cred.bv_len = Curl_bufref_len(initresp);
  إذا(!cred.bv_val)
    pcred = عدم;
  rc = ldap_sasl_bind(li->ld, عدم, mech, pcred, عدم, عدم, &li->msgid);
  إذا(rc != LDAP_SUCCESS)
    أرجع oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  أرجع CURLE_OK;
}

/*
 * Sends SASL continuation.
 */
ساكن CURLcode oldap_continue_auth(بنية Curl_easy *انکڑے, ثابت حرف *mech,
                                    ثابت بنية bufref *resp)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  بنية berval cred;
  بنية berval *pcred = &cred;
  صحيح rc;

  إذا(!li)
    أرجع CURLE_FAILED_INIT;
  cred.bv_val = (حرف *)CURL_UNCONST(Curl_bufref_ptr(resp));
  cred.bv_len = Curl_bufref_len(resp);
  إذا(!cred.bv_val)
    pcred = عدم;
  rc = ldap_sasl_bind(li->ld, عدم, mech, pcred, عدم, عدم, &li->msgid);
  إذا(rc != LDAP_SUCCESS)
    أرجع oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  أرجع CURLE_OK;
}

/*
 * Sends SASL bind cancellation.
 */
ساكن CURLcode oldap_cancel_auth(بنية Curl_easy *انکڑے, ثابت حرف *mech)
{
  بنية ldapconninfo *li =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_LDAP_CONN);
  صحيح rc;

  (فراغ)mech;
  إذا(!li)
    أرجع CURLE_FAILED_INIT;
  rc = ldap_sasl_bind(li->ld, عدم, LDAP_SASL_NULL, عدم, عدم, عدم,
                      &li->msgid);
  إذا(rc != LDAP_SUCCESS)
    أرجع oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  أرجع CURLE_OK;
}

/* Starts LDAP simple bind. */
ساكن CURLcode oldap_perform_bind(بنية Curl_easy *انکڑے, ldapstate newstate)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  ثابت حرف *binddn = عدم;
  بنية berval passwd;
  صحيح rc;

  إذا(!li)
    أرجع CURLE_FAILED_INIT;
  passwd.bv_val = عدم;
  passwd.bv_len = 0;

  إذا(conn->creds) {
    binddn = Curl_creds_user(conn->creds);
    passwd.bv_val = CURL_UNCONST(Curl_creds_passwd(conn->creds));
    passwd.bv_len = لڑی_لمبائی(passwd.bv_val);
  }

  rc = ldap_sasl_bind(li->ld, binddn, LDAP_SASL_SIMPLE, &passwd,
                      عدم, عدم, &li->msgid);
  إذا(rc != LDAP_SUCCESS)
    أرجع oldap_map_error(rc,
                           انکڑے->state.creds ?
                           CURLE_LOGIN_DENIED : CURLE_LDAP_CANNOT_BIND);
  oldap_state(انکڑے, li, newstate);
  أرجع CURLE_OK;
}

/* Query the supported SASL authentication mechanisms. */
ساكن CURLcode oldap_perform_mechs(بنية Curl_easy *انکڑے)
{
  بنية ldapconninfo *li =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_LDAP_CONN);
  صحيح rc;
  ساكن ثابت حرف * ثابت supportedSASLMechanisms[] = {
    "supportedSASLMechanisms",
    عدم
  };

  إذا(!li)
    أرجع CURLE_FAILED_INIT;
  /* Casting away the const for the 3rd parameter that the LDAP API expects as
     a non-const char ** is potentially unsafe but we believe the lack of
     const in the API was an oversight and that no LDAP implementation
     actually modifies the input. */
  rc = ldap_تلاش_ext(li->ld, "", LDAP_SCOPE_ادھار, "(objectclass=*)",
                       (حرف **)CURL_UNCONST(supportedSASLMechanisms), 0,
                       عدم, عدم, عدم, 0, &li->msgid);
  إذا(rc != LDAP_SUCCESS)
    أرجع oldap_map_error(rc, CURLE_LOGIN_DENIED);
  oldap_state(انکڑے, li, OLDAP_MECHS);
  أرجع CURLE_OK;
}

/* Starts SASL bind. */
ساكن CURLcode oldap_perform_sasl(بنية Curl_easy *انکڑے)
{
  بنية ldapconninfo *li =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_LDAP_CONN);
  saslprogress ترقی = SASL_IDLE;
  CURLcode نتیجہ;

  إذا(!li)
    أرجع CURLE_FAILED_INIT;
  نتیجہ = Curl_sasl_شروع(&li->sasl, انکڑے, TRUE, &ترقی);

  oldap_state(انکڑے, li, OLDAP_SASL);
  إذا(!نتیجہ && ترقی != SASL_INPROGRESS)
    نتیجہ = Curl_sasl_is_blocked(&li->sasl, انکڑے);
  أرجع نتیجہ;
}

#جے_تعریف_شدہ USE_SSL
ساكن صحيح ldapsb_tls_تیاری(Sockbuf_IO_Desc *sbiod, فراغ *arg)
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
  بنية Curl_easy *انکڑے = sbiod->sbiod_pvt;
  إذا(انکڑے) {
    بنية connectdata *conn = انکڑے->conn;
    إذا(conn) {
      بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      إذا(li && (li->dupfd != CURL_SOCKET_BAD)) {
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
  إذا(opt == LBER_SB_OPT_انکڑے_READY) {
    بنية Curl_easy *انکڑے = sbiod->sbiod_pvt;
    أرجع Curl_conn_انکڑے_pending(انکڑے, FIRSTSOCKET);
  }
  أرجع 0;
}

ساكن ber_slen_t ldapsb_tls_read(Sockbuf_IO_Desc *sbiod, فراغ *buf,
                                  ber_len_t len)
{
  بنية Curl_easy *انکڑے = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  إذا(انکڑے) {
    بنية connectdata *conn = انکڑے->conn;
    إذا(conn) {
      بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode نتیجہ = CURLE_RECV_ERROR;
      size_t nread;

      إذا(!li) {
        سیٹ_SOCKERRNO(SOCKEINVAL);
        أرجع -1;
      }
      نتیجہ = (li->recv)(انکڑے, FIRSTSOCKET, buf, len, &nread);
      إذا(نتیجہ == CURLE_AGAIN) {
        سیٹ_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = نتیجہ ? -1 : (ber_slen_t)nread;
    }
  }
  أرجع ret;
}

ساكن ber_slen_t ldapsb_tls_write(Sockbuf_IO_Desc *sbiod, فراغ *buf,
                                   ber_len_t len)
{
  بنية Curl_easy *انکڑے = sbiod->sbiod_pvt;
  ber_slen_t ret = 0;
  إذا(انکڑے) {
    بنية connectdata *conn = انکڑے->conn;
    إذا(conn) {
      بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      CURLcode نتیجہ = CURLE_SEND_ERROR;
      size_t nwritten;

      إذا(!li) {
        سیٹ_SOCKERRNO(SOCKEINVAL);
        أرجع -1;
      }
      نتیجہ = (li->send)(انکڑے, FIRSTSOCKET, buf, len, FALSE, &nwritten);
      إذا(نتیجہ == CURLE_AGAIN) {
        سیٹ_SOCKERRNO(SOCKEWOULDBLOCK);
      }
      ret = نتیجہ ? -1 : (ber_slen_t)nwritten;
    }
  }
  أرجع ret;
}

ساكن Sockbuf_IO ldapsb_tls = {
  ldapsb_tls_تیاری,
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

ساكن CURLcode oldap_ssl_connect(بنية Curl_easy *انکڑے, ldapstate newstate)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  bool ssldone = FALSE;
  CURLcode نتیجہ;

  إذا(!li)
    أرجع CURLE_FAILED_INIT;
  نتیجہ = Curl_conn_connect(انکڑے, FIRSTSOCKET, FALSE, &ssldone);
  إذا(نتیجہ)
    أرجع نتیجہ;
  oldap_state(انکڑے, li, newstate);

  إذا(ssldone) {
    Sockbuf *sb;

    /* Install the libcurl SSL handlers into the sockbuf. */
    إذا((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_جوڑو_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, انکڑے))
      أرجع CURLE_FAILED_INIT;
    li->recv = conn->recv[FIRSTSOCKET];
    li->send = conn->send[FIRSTSOCKET];
  }

  أرجع نتیجہ;
}

/* Send the STARTTLS request */
ساكن CURLcode oldap_perform_starttls(بنية Curl_easy *انکڑے)
{
  بنية ldapconninfo *li =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_LDAP_CONN);
  صحيح rc;

  إذا(!li)
    أرجع CURLE_FAILED_INIT;
  rc = ldap_شروع_tls(li->ld, عدم, عدم, &li->msgid);
  إذا(rc != LDAP_SUCCESS)
    أرجع oldap_map_error(rc, CURLE_USE_SSL_FAILED);
  oldap_state(انکڑے, li, OLDAP_STARTTLS);
  أرجع CURLE_OK;
}
#جے_انت

ساكن فراغ oldap_easy_dtor(ثابت فراغ *کلید, size_t klen, فراغ *اندراج)
{
  بنية ldapreqinfo *lr = اندراج;
  (فراغ)کلید;
  (فراغ)klen;
  curlx_free(lr);
}

ساكن فراغ oldap_conn_dtor(ثابت فراغ *کلید, size_t klen, فراغ *اندراج)
{
  بنية ldapconninfo *li = اندراج;
  (فراغ)کلید;
  (فراغ)klen;
  إذا(li->ld) {
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
  oldap_get_سنیہا,          /* Get SASL response message */
  0,                          /* Maximum initial response length (no max) */
  LDAP_SASL_BIND_وچ_ترقی, /* Code received when continuation is expected */
  LDAP_SUCCESS,               /* Code to receive upon authentication success */
  SASL_AUTH_NONE,             /* Default mechanisms */
  0                           /* Configuration flags */
};

ساكن CURLcode oldap_connect(بنية Curl_easy *انکڑے, bool *پورا)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية ldapconninfo *li;
  ساكن ثابت صحيح نسخہ_2 = LDAP_VERSION3;
  حرف *hosturl = عدم;
  CURLcode نتیجہ;
  صحيح rc;
#جے_تعریف_شدہ CURL_OPENLDAP_DEBUG
  ساكن صحيح do_trace = -1;
#جے_انت

  (فراغ)پورا;

  li = curlx_calloc(1, حجم(بنية ldapconninfo));
  إذا(!li) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }
  li->dupfd = CURL_SOCKET_BAD;

  نتیجہ = Curl_conn_meta_سیٹ(conn, CURL_META_LDAP_CONN, li, oldap_conn_dtor);
  إذا(نتیجہ)
    اذهب باہرلا;

  li->proto = ldap_pvt_url_scheme2proto(انکڑے->state.origin->scheme->name);

  /* Initialize the SASL storage */
  Curl_sasl_init(&li->sasl, انکڑے, &saslldap);

  نتیجہ = oldap_parse_login_options(conn);
  إذا(نتیجہ)
    اذهب باہرلا;

  hosturl = curl_maprintf("%s://%s:%u",
                          conn->scheme->name,
                          conn->origin->ipv6 ?
                          conn->origin->user_hostname :
                          conn->origin->hostname,
                          conn->origin->پورٹ);
  إذا(!hosturl) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  {
#جے_تعریف_شدہ _WIN32
    SOCKET dupfd = INVALID_SOCKET;
    WSAPROTOCOL_INFO pi;
    إذا(WSADuplicateSocket(conn->sock[FIRSTSOCKET], GetCurrentProcessId(),
                          &pi))
      ; /* error */
    وإلا
      dupfd = CURL_SOCKET(pi.i_پتہ_Family, pi.iSocketType, pi.iProtocol);
#وإلا
#جے_تعریف_شدہ F_DUPFD_CLOEXEC
    صحيح dupfd = fcntl(conn->sock[FIRSTSOCKET], F_DUPFD_CLOEXEC, 0);
#وإلا
    صحيح dupfd = dup(conn->sock[FIRSTSOCKET]);
#جے_تعریف_شدہ HAVE_FCNTL
    إذا((dupfd != CURL_SOCKET_BAD) &&
       (fcntl(dupfd, F_SETFD, FD_CLOEXEC) < 0)) {
      sclose(dupfd);
      dupfd = CURL_SOCKET_BAD;
    }
#جے_انت /* HAVE_FCNTL */
#جے_انت /* F_DUPFD_CLOEXEC */
#جے_انت /* _WIN32 */

    إذا(dupfd == CURL_SOCKET_BAD) {
      نتیجہ = CURLE_COULDNT_CONNECT;
      اذهب باہرلا;
    }
    rc = ldap_init_fd((ber_socket_t)dupfd, li->proto, hosturl, &li->ld);
    إذا(rc)
      sclose(dupfd);
    وإلا
      /* store the socket, as when this is used by TLS we close this ourselves
         and at this point the code does not yet know if this will use TLS or
         not */
      li->dupfd = dupfd;
  }
  إذا(rc) {
    failf(انکڑے, "LDAP local: Cannot connect to %s, %s",
          hosturl, ldap_err2string(rc));
    نتیجہ = CURLE_COULDNT_CONNECT;
    اذهب باہرلا;
  }

#جے_تعریف_شدہ CURL_OPENLDAP_DEBUG
  إذا(do_trace < 0) {
    ثابت حرف *env = getenv("CURL_OPENLDAP_TRACE");
    curl_off_t e = 0;
    إذا(!curlx_str_نمبر(&env, &e, ودھ_توں_ودھ_عدد))
      do_trace = e > 0;
  }
  إذا(do_trace)
    ldap_سیٹ_option(li->ld, LDAP_OPT_DEBUG_LEVEL, &do_trace);
#جے_انت

  /* Try version 3 first. */
  ldap_سیٹ_option(li->ld, LDAP_OPT_PROTOCOL_نسخہ, &نسخہ_2);

  /* Do not chase referrals. */
  ldap_سیٹ_option(li->ld, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);

  {
    ber_len_t max = 256 * 1024;
    Sockbuf *sb;
    إذا((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       /* Set the maximum allowed size of an incoming message, which to
          OpenLDAP means that it will malloc() memory up to this size. If not
          set, there is no limit and we instead risk a malloc() failure. */
       !ber_sockbuf_ctrl(sb, LBER_SB_OPT_سیٹ_MAX_INCOMING, &max)) {
      نتیجہ = CURLE_FAILED_INIT;
      اذهب باہرلا;
    }
  }

#جے_تعریف_شدہ USE_SSL
  إذا(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    نتیجہ = oldap_ssl_connect(انکڑے, OLDAP_SSL);
    اذهب باہرلا;
  }

  إذا(انکڑے->سیٹ.use_ssl) {
    نتیجہ = oldap_perform_starttls(انکڑے);
    إذا(!نتیجہ || انکڑے->سیٹ.use_ssl != CURLUSESSL_TRY)
      اذهب باہرلا;
  }
#جے_انت

  إذا(li->sasl.prefmech != SASL_AUTH_NONE) {
    نتیجہ = oldap_perform_mechs(انکڑے);
    اذهب باہرلا;
  }

  /* Force bind even if anonymous bind is not needed in protocol version 3
     to detect missing version 3 support. */
  نتیجہ = oldap_perform_bind(انکڑے, OLDAP_BIND);

باہرلا:
  curlx_free(hosturl);
  أرجع نتیجہ;
}

/* Handle the supported SASL mechanisms query response */
ساكن CURLcode oldap_state_mechs_resp(بنية Curl_easy *انکڑے,
                                       LDAPMessage *پیغام, صحيح کوڈ)
{
  بنية connectdata *conn;
  بنية ldapconninfo *li;
  صحيح rc;
  BerElement *ber = عدم;
  CURLcode نتیجہ = CURLE_OK;
  بنية berval bv, *bvals;

  إذا(!انکڑے)
    أرجع CURLE_FAILED_INIT;

  conn = انکڑے->conn;
  li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);

  إذا(!li)
    أرجع CURLE_FAILED_INIT;
  بدل(ldap_msgtype(پیغام)) {
  حالة LDAP_RES_تلاش_اندراج:
    /* Got a list of supported SASL mechanisms. */
    إذا(کوڈ != LDAP_SUCCESS && کوڈ != LDAP_NO_RESULTS_RETURNED)
      أرجع CURLE_LOGIN_DENIED;

    rc = ldap_get_dn_ber(li->ld, پیغام, &ber, &bv);
    إذا(rc < 0)
      أرجع oldap_map_error(rc, CURLE_BAD_CONTENT_ENCODING);
    لكل(rc = ldap_get_attribute_ber(li->ld, پیغام, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, پیغام, ber, &bv, &bvals)) {
      صحيح i;

      إذا(!bv.bv_val)
        اكسر;

      إذا(bvals) {
        لكل(i = 0; bvals[i].bv_val; i++) {
          size_t llen;
          بنانشانی چھوٹا mech =
            Curl_sasl_decode_mech((ثابت حرف *)bvals[i].bv_val,
                                  bvals[i].bv_len, &llen);
          إذا(bvals[i].bv_len == llen)
            li->sasl.authmechs |= mech;
        }
        ber_memfree(bvals);
      }
    }
    ber_free(ber, 0);
    اكسر;

  حالة LDAP_RES_تلاش_نتیجہ:
    بدل(کوڈ) {
    حالة LDAP_SIZELIMIT_EXCEEDED:
      infof(انکڑے, "Too many authentication mechanisms");
      FALLTHROUGH();
    حالة LDAP_SUCCESS:
    حالة LDAP_NO_RESULTS_RETURNED:
      إذا(Curl_sasl_can_authenticate(&li->sasl, انکڑے))
        نتیجہ = oldap_perform_sasl(انکڑے);
      وإلا
        نتیجہ = CURLE_LOGIN_DENIED;
      اكسر;
    افتراضي:
      نتیجہ = oldap_map_error(کوڈ, CURLE_LOGIN_DENIED);
      اكسر;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع نتیجہ;
}

/* Handle a SASL bind response. */
ساكن CURLcode oldap_state_sasl_resp(بنية Curl_easy *انکڑے,
                                      LDAPMessage *پیغام, صحيح کوڈ)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode نتیجہ = CURLE_OK;
  saslprogress ترقی;
  صحيح rc;

  إذا(!li)
    أرجع CURLE_FAILED_INIT;
  li->servercred = عدم;
  rc = ldap_parse_sasl_bind_نتیجہ(li->ld, پیغام, &li->servercred, 0);
  إذا(rc != LDAP_SUCCESS) {
    failf(انکڑے, "LDAP local: sasl ldap_parse_result %s", ldap_err2string(rc));
    نتیجہ = oldap_map_error(rc, CURLE_LOGIN_DENIED);
  }
  وإلا {
    نتیجہ = Curl_sasl_continue(&li->sasl, انکڑے, کوڈ, &ترقی);
    إذا(!نتیجہ) {
      بدل(ترقی) {
      حالة SASL_پورا:
        oldap_state(انکڑے, li, OLDAP_STOP);   /* Authenticated */
        اكسر;
      حالة SASL_IDLE:            /* No mechanism left after cancellation */
        failf(انکڑے, "Authentication cancelled");
        نتیجہ = CURLE_LOGIN_DENIED;
        اكسر;
      افتراضي:
        اكسر;
      }
    }
  }

  إذا(li->servercred)
    ber_bvfree(li->servercred);
  أرجع نتیجہ;
}

/* Handle a simple bind response. */
ساكن CURLcode oldap_state_bind_resp(بنية Curl_easy *انکڑے, LDAPMessage *پیغام,
                                      صحيح کوڈ)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  CURLcode نتیجہ = CURLE_OK;
  بنية berval *bv = عدم;
  صحيح rc;

  إذا(!li)
    أرجع CURLE_FAILED_INIT;

  إذا(کوڈ != LDAP_SUCCESS)
    أرجع oldap_map_error(کوڈ, CURLE_LDAP_CANNOT_BIND);

  rc = ldap_parse_sasl_bind_نتیجہ(li->ld, پیغام, &bv, 0);
  إذا(rc != LDAP_SUCCESS) {
    failf(انکڑے, "LDAP local: bind ldap_parse_sasl_bind_result %s",
          ldap_err2string(rc));
    نتیجہ = oldap_map_error(rc, CURLE_LDAP_CANNOT_BIND);
  }
  وإلا
    oldap_state(انکڑے, li, OLDAP_STOP);

  إذا(bv)
    ber_bvfree(bv);
  أرجع نتیجہ;
}

ساكن CURLcode oldap_connecting(بنية Curl_easy *انکڑے, bool *پورا)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية connectdata *conn = انکڑے->conn;
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  LDAPMessage *پیغام = عدم;
  بنية timeval tv = { 0, 0 };
  صحيح کوڈ = LDAP_SUCCESS;
  صحيح rc;

  إذا(!li)
    أرجع CURLE_FAILED_INIT;

  إذا(li->state != OLDAP_SSL && li->state != OLDAP_TLS) {
    /* Get response to last command. */
    rc = ldap_نتیجہ(li->ld, li->msgid, LDAP_پیغام_ONE, &tv, &پیغام);
    بدل(rc) {
    حالة 0:                               /* Timed out. */
      أرجع CURLE_OK;
    حالة LDAP_RES_تلاش_اندراج:
    حالة LDAP_RES_تلاش_REFERENCE:
      اكسر;
    افتراضي:
      li->msgid = 0;                      /* Nothing to abandon upon error. */
      إذا(rc < 0) {
        failf(انکڑے, "LDAP local: connecting ldap_result %s",
              ldap_err2string(rc));
        أرجع oldap_map_error(rc, CURLE_COULDNT_CONNECT);
      }
      اكسر;
    }

    /* Get error code from message. */
    rc = ldap_parse_نتیجہ(li->ld, پیغام, &کوڈ, عدم, عدم, عدم, عدم, 0);
    إذا(rc)
      کوڈ = rc;
    وإلا {
      /* store the latest code for later retrieval */
      انکڑے->info.httpcode = کوڈ;
    }

    /* If protocol version 3 is not supported, fallback to version 2. */
    إذا(کوڈ == LDAP_PROTOCOL_ERROR && li->state != OLDAP_BINDV2 &&
#جے_تعریف_شدہ USE_SSL
       (ssl_installed(conn) || انکڑے->سیٹ.use_ssl <= CURLUSESSL_TRY) &&
#جے_انت
       li->sasl.prefmech == SASL_AUTH_NONE) {
      ساكن ثابت صحيح نسخہ_2 = LDAP_VERSION2;

      ldap_سیٹ_option(li->ld, LDAP_OPT_PROTOCOL_نسخہ, &نسخہ_2);
      ldap_msgfree(پیغام);
      أرجع oldap_perform_bind(انکڑے, OLDAP_BINDV2);
    }
  }

  /* Handle response message according to current state. */
  بدل(li->state) {

#جے_تعریف_شدہ USE_SSL
  حالة OLDAP_SSL:
    نتیجہ = oldap_ssl_connect(انکڑے, OLDAP_SSL);
    إذا(!نتیجہ && ssl_installed(conn)) {
      إذا(li->sasl.prefmech != SASL_AUTH_NONE)
        نتیجہ = oldap_perform_mechs(انکڑے);
      وإلا
        نتیجہ = oldap_perform_bind(انکڑے, OLDAP_BIND);
    }
    اكسر;
  حالة OLDAP_STARTTLS:
    إذا(کوڈ != LDAP_SUCCESS) {
      إذا(انکڑے->سیٹ.use_ssl != CURLUSESSL_TRY)
        نتیجہ = oldap_map_error(کوڈ, CURLE_USE_SSL_FAILED);
      وإلا إذا(li->sasl.prefmech != SASL_AUTH_NONE)
        نتیجہ = oldap_perform_mechs(انکڑے);
      وإلا
        نتیجہ = oldap_perform_bind(انکڑے, OLDAP_BIND);
      اكسر;
    }
    نتیجہ = Curl_ssl_cfilter_جوڑو(
      انکڑے, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    إذا(نتیجہ)
      اكسر;
    FALLTHROUGH();
  حالة OLDAP_TLS:
    نتیجہ = oldap_ssl_connect(انکڑے, OLDAP_TLS);
    إذا(نتیجہ)
      نتیجہ = oldap_map_error(کوڈ, CURLE_USE_SSL_FAILED);
    وإلا إذا(ssl_installed(conn)) {
      إذا(li->sasl.prefmech != SASL_AUTH_NONE)
        نتیجہ = oldap_perform_mechs(انکڑے);
      وإلا إذا(انکڑے->state.creds)
        نتیجہ = oldap_perform_bind(انکڑے, OLDAP_BIND);
      وإلا {
        /* Version 3 supported: no bind required */
        oldap_state(انکڑے, li, OLDAP_STOP);
        نتیجہ = CURLE_OK;
      }
    }
    اكسر;
#جے_انت

  حالة OLDAP_MECHS:
    نتیجہ = oldap_state_mechs_resp(انکڑے, پیغام, کوڈ);
    اكسر;
  حالة OLDAP_SASL:
    نتیجہ = oldap_state_sasl_resp(انکڑے, پیغام, کوڈ);
    اكسر;
  حالة OLDAP_BIND:
  حالة OLDAP_BINDV2:
    نتیجہ = oldap_state_bind_resp(انکڑے, پیغام, کوڈ);
    اكسر;
  افتراضي:
    /* internal error */
    نتیجہ = CURLE_COULDNT_CONNECT;
    اكسر;
  }

  ldap_msgfree(پیغام);

  *پورا = li->state == OLDAP_STOP;
  إذا(*پورا)
    conn->recv[FIRSTSOCKET] = oldap_recv;

  إذا(نتیجہ && li->msgid) {
    ldap_abandon_ext(li->ld, li->msgid, عدم, عدم);
    li->msgid = 0;
  }
  أرجع نتیجہ;
}

ساكن CURLcode oldap_disconnect(بنية Curl_easy *انکڑے,
                                 بنية connectdata *conn,
                                 bool dead_connection)
{
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  (فراغ)dead_connection;
#جے_بنا_تعریف USE_SSL
  (فراغ)انکڑے;
#جے_انت

  إذا(li && li->ld) {
#جے_تعریف_شدہ USE_SSL
    إذا(ssl_installed(conn)) {
      Sockbuf *sb;
      إذا(ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS ||
         ber_sockbuf_جوڑو_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, انکڑے))
        أرجع CURLE_FAILED_INIT;
    }
#جے_انت
    ldap_unbind_ext(li->ld, عدم, عدم);
    li->ld = عدم;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode oldap_do(بنية Curl_easy *انکڑے, bool *پورا)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  بنية ldapreqinfo *lr;
  CURLcode نتیجہ;
  صحيح rc;
  LDAPURLDesc *lud;
  صحيح msgid;

  إذا(!li)
    أرجع CURLE_FAILED_INIT;

  infof(انکڑے, "LDAP local: %s", Curl_bufref_ptr(&انکڑے->state.url));

  نتیجہ = oldap_url_parse(انکڑے, &lud);
  إذا(نتیجہ)
    اذهب باہرلا;

#جے_تعریف_شدہ USE_SSL
  إذا(ssl_installed(conn)) {
    Sockbuf *sb;
    /* re-install the libcurl SSL handlers into the sockbuf. */
    إذا((ldap_get_option(li->ld, LDAP_OPT_SOCKBUF, &sb) != LDAP_OPT_SUCCESS) ||
       ber_sockbuf_جوڑو_io(sb, &ldapsb_tls, LBER_SBIOD_LEVEL_TRANSPORT, انکڑے)) {
      ldap_free_urldesc(lud);
      أرجع CURLE_FAILED_INIT;
    }
  }
#جے_انت

  rc = ldap_تلاش_ext(li->ld, lud->lud_dn, lud->lud_scope,
                       lud->lud_filter, lud->lud_attrs, 0,
                       عدم, عدم, عدم, 0, &msgid);
  ldap_free_urldesc(lud);
  إذا(rc != LDAP_SUCCESS) {
    failf(انکڑے, "LDAP local: ldap_search_ext %s", ldap_err2string(rc));
    نتیجہ = CURLE_LDAP_تلاش_FAILED;
    اذهب باہرلا;
  }

  lr = curlx_calloc(1, حجم(بنية ldapreqinfo));
  إذا(!lr ||
     Curl_meta_سیٹ(انکڑے, CURL_META_LDAP_EASY, lr, oldap_easy_dtor)) {
    ldap_abandon_ext(li->ld, msgid, عدم, عدم);
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  lr->msgid = msgid;
  Curl_xfer_تیاری_recv(انکڑے, FIRSTSOCKET, -1);
  *پورا = TRUE;

باہرلا:
  أرجع نتیجہ;
}

ساكن CURLcode oldap_پورا(بنية Curl_easy *انکڑے, CURLcode res,
                           bool premature)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية ldapreqinfo *lr = Curl_meta_get(انکڑے, CURL_META_LDAP_EASY);

  (فراغ)res;
  (فراغ)premature;

  إذا(lr) {
    /* if there was a search in progress, abandon it */
    إذا(lr->msgid) {
      بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
      إذا(li && li->ld) {
        ldap_abandon_ext(li->ld, lr->msgid, عدم, عدم);
      }
      lr->msgid = 0;
    }
    Curl_meta_remove(انکڑے, CURL_META_LDAP_EASY);
  }

  أرجع CURLE_OK;
}

ساكن CURLcode client_write(بنية Curl_easy *انکڑے,
                             ثابت حرف *اگیتر, size_t plen,
                             ثابت حرف *قدر, size_t len,
                             ثابت حرف *suffix, size_t slen)
{
  CURLcode نتیجہ = CURLE_OK;

  إذا(اگیتر) {
    /* If we have a zero-length value and the prefix ends with a space
       separator, drop the latter. */
    إذا(!len && plen && اگیتر[plen - 1] == ' ')
      plen--;
    نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY, اگیتر, plen);
  }
  إذا(!نتیجہ && قدر) {
    نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY, قدر, len);
  }
  إذا(!نتیجہ && suffix) {
    نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY, suffix, slen);
  }
  أرجع نتیجہ;
}

/* RFC 2849: a string that is not a SAFE-STRING (leading ':' or '<', leading or
   trailing space, or any non-printable byte such as CR/LF) cannot go into LDIF
   as-is. DNs and values get base64-encoded instead; an attribute type has no
   base64 form in LDIF and is rejected by the caller. */
ساكن bool oldap_str_needs_base64(ثابت حرف *s, size_t len)
{
  size_t i;
  إذا(!len)
    أرجع FALSE;
  إذا((s[0] == ':') || (s[0] == '<') || ISBLANK(s[0]) || ISBLANK(s[len - 1]))
    أرجع TRUE;
  لكل(i = 0; i < len; i++)
    إذا(!ISPRINT(s[i]))
      أرجع TRUE;
  أرجع FALSE;
}

ساكن CURLcode oldap_recv(بنية Curl_easy *انکڑے, int8_t sockindex, حرف *buf,
                           size_t len, size_t *pnread)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية ldapconninfo *li = Curl_conn_meta_get(conn, CURL_META_LDAP_CONN);
  بنية ldapreqinfo *lr = Curl_meta_get(انکڑے, CURL_META_LDAP_EASY);
  صحيح rc;
  LDAPMessage *پیغام = عدم;
  BerElement *ber = عدم;
  بنية timeval tv = { 0, 0 };
  بنية berval bv, *bvals;
  CURLcode نتیجہ = CURLE_AGAIN;
  صحيح کوڈ;
  حرف *info = عدم;

  (فراغ)len;
  (فراغ)buf;
  (فراغ)sockindex;
  *pnread = 0;
  إذا(!li || !lr)
    أرجع CURLE_FAILED_INIT;

  rc = ldap_نتیجہ(li->ld, lr->msgid, LDAP_پیغام_ONE, &tv, &پیغام);
  إذا(rc < 0) {
    failf(انکڑے, "LDAP local: search ldap_result %s", ldap_err2string(rc));
    نتیجہ = CURLE_RECV_ERROR;
  }

  /* error or timed out */
  إذا(!پیغام)
    أرجع نتیجہ;

  نتیجہ = CURLE_OK;

  بدل(ldap_msgtype(پیغام)) {
  حالة LDAP_RES_تلاش_نتیجہ:
    lr->msgid = 0;
    rc = ldap_parse_نتیجہ(li->ld, پیغام, &کوڈ, عدم, &info, عدم, عدم, 0);
    إذا(rc) {
      failf(انکڑے, "LDAP local: search ldap_parse_result %s",
            ldap_err2string(rc));
      نتیجہ = CURLE_LDAP_تلاش_FAILED;
      اكسر;
    }

    /* store the latest code for later retrieval */
    انکڑے->info.httpcode = کوڈ;

    بدل(کوڈ) {
    حالة LDAP_SIZELIMIT_EXCEEDED:
      infof(انکڑے, "There are more than %d entries", lr->nument);
      FALLTHROUGH();
    حالة LDAP_SUCCESS:
      انکڑے->req.ناپ = انکڑے->req.bytecount;
      اكسر;
    افتراضي:
      failf(انکڑے, "LDAP remote: search failed %s %s", ldap_err2string(کوڈ),
            info ? info : "");
      نتیجہ = CURLE_LDAP_تلاش_FAILED;
      اكسر;
    }
    إذا(info)
      ldap_memfree(info);
    اكسر;
  حالة LDAP_RES_تلاش_اندراج:
    lr->nument++;
    rc = ldap_get_dn_ber(li->ld, پیغام, &ber, &bv);
    إذا(rc < 0) {
      نتیجہ = CURLE_RECV_ERROR;
      اكسر;
    }

    إذا(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
      حرف *dn_b64 = عدم;
      size_t dn_b64_sz = 0;

      /* A DN with e.g. CR/LF would inject LDIF lines, so base64-encode it. */
      نتیجہ = curlx_base64_encode((uint8_t *)bv.bv_val, bv.bv_len,
                                   &dn_b64, &dn_b64_sz);
      إذا(!نتیجہ)
        نتیجہ = client_write(انکڑے, STRCONST("DN:: "), dn_b64, dn_b64_sz,
                              STRCONST("\n"));
      curlx_free(dn_b64);
    }
    وإلا
      نتیجہ = client_write(انکڑے, STRCONST("DN: "), bv.bv_val, bv.bv_len,
                            STRCONST("\n"));
    إذا(نتیجہ)
      اكسر;

    لكل(rc = ldap_get_attribute_ber(li->ld, پیغام, ber, &bv, &bvals);
        rc == LDAP_SUCCESS;
        rc = ldap_get_attribute_ber(li->ld, پیغام, ber, &bv, &bvals)) {
      صحيح i;
      bool ثنائی;

      إذا(!bv.bv_val)
        اكسر;

      /* An attribute type has no base64 form in LDIF, so a type carrying
         control bytes (CR/LF etc.) would let a hostile server inject lines.
         A conformant type name never contains them, so reject it. */
      إذا(oldap_str_needs_base64(bv.bv_val, bv.bv_len)) {
        failf(انکڑے, "LDAP local: attribute type contains illegal bytes");
        إذا(bvals)
          ber_memfree(bvals);
        نتیجہ = CURLE_WEIRD_SERVER_جواب;
        اكسر;
      }

      إذا(!bvals) {
        نتیجہ = client_write(انکڑے, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":\n"));
        إذا(نتیجہ)
          اكسر;
        استمر;
      }

      ثنائی = bv.bv_len > 7 &&
        curl_strnequal(bv.bv_val + bv.bv_len - 7, ";binary", 7);

      لكل(i = 0; bvals[i].bv_val; i++) {
        bool binval = FALSE;

        نتیجہ = client_write(انکڑے, STRCONST("\t"), bv.bv_val, bv.bv_len,
                              STRCONST(":"));
        إذا(نتیجہ)
          اكسر;

        إذا(!ثنائی)
          binval = oldap_str_needs_base64(bvals[i].bv_val, bvals[i].bv_len);
        إذا(ثنائی || binval) {
          حرف *val_b64 = عدم;
          size_t val_b64_sz = 0;

          /* Binary value, encode to base64. */
          إذا(bvals[i].bv_len)
            نتیجہ = curlx_base64_encode((uint8_t *)bvals[i].bv_val,
                                         bvals[i].bv_len,
                                         &val_b64, &val_b64_sz);
          إذا(!نتیجہ)
            نتیجہ = client_write(انکڑے, STRCONST(": "), val_b64, val_b64_sz,
                                  STRCONST("\n"));
          curlx_free(val_b64);
        }
        وإلا
          نتیجہ = client_write(انکڑے, STRCONST(" "),
                                bvals[i].bv_val, bvals[i].bv_len,
                                STRCONST("\n"));
        إذا(نتیجہ)
          اكسر;
      }

      ber_memfree(bvals);
      bvals = عدم;
      إذا(!نتیجہ)
        نتیجہ = client_write(انکڑے, STRCONST("\n"), عدم, 0, عدم, 0);
      إذا(نتیجہ)
        اكسر;
    }

    إذا(!نتیجہ)
      نتیجہ = client_write(انکڑے, STRCONST("\n"), عدم, 0, عدم, 0);
    إذا(!نتیجہ)
      نتیجہ = CURLE_AGAIN;
    اكسر;
  }

  ber_free(ber, 0);
  ldap_msgfree(پیغام);
  أرجع نتیجہ;
}

فراغ Curl_ldap_نسخہ(حرف *buf, size_t bufsz)
{
  LDAPAPIInfo api;
  api.ldapai_info_نسخہ = LDAP_API_INFO_نسخہ;

  إذا(ldap_get_option(عدم, LDAP_OPT_API_INFO, &api) == LDAP_OPT_SUCCESS) {
    بنانشانی صحيح patch = (بنانشانی صحيح)(api.ldapai_vendor_نسخہ % 100);
    بنانشانی صحيح major = (بنانشانی صحيح)(api.ldapai_vendor_نسخہ / 10000);
    بنانشانی صحيح minor =
      (((بنانشانی صحيح)api.ldapai_vendor_نسخہ - (major * 10000))
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
  oldap_تیاری_connection,               /* setup_connection */
  oldap_do,                             /* do_it */
  oldap_پورا,                           /* done */
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

#جے_انت /* !CURL_DISABLE_LDAP && USE_OPENLDAP */

/* The LDAP scheme structs are in ldap.c */
