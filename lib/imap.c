/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
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
 * RFC2195 CRAM-MD5 authentication
 * RFC2595 Using TLS with IMAP, POP3 and ACAP
 * RFC2831 DIGEST-MD5 authentication
 * RFC3501 IMAPv4 protocol
 * RFC4422 Simple Authentication and Security Layer (SASL)
 * RFC4616 PLAIN authentication
 * RFC4752 The Kerberos V5 ("GSSAPI") SASL Mechanism
 * RFC4959 IMAP Extension for SASL Initial Client Response
 * RFC5092 IMAP URL Scheme
 * RFC6749 OAuth 2.0 Authorization Framework
 * RFC8314 Use of TLS for Email Submission and Access
 * Draft   LOGIN SASL Mechanism <draft-murchison-sasl-login-00.txt>
 *
 ***************************************************************************/
#ضمن "curl_setup.h"
#ضمن "urldata.h"
#ضمن "imap.h"

#جے_بنا_تعریف CURL_DISABLE_IMAP

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETDB_H
#ضمن <netdb.h>
#جے_انت
#جے_تعریف_شدہ __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جے_انت

#ضمن "curlx/dynbuf.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "progress.h"
#ضمن "transfer.h"
#ضمن "escape.h"
#ضمن "pingpong.h"
#ضمن "mime.h"
#ضمن "curlx/strparse.h"
#ضمن "strcase.h"
#ضمن "vtls/vtls.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "url.h"
#ضمن "bufref.h"
#ضمن "curl_sasl.h"
#ضمن "curlx/strcopy.h"

/* meta key for storing protocol meta at easy handle */
#تعریف_کرو CURL_META_IMAP_EASY   "meta:proto:imap:easy"
/* meta key for storing protocol meta at connection */
#تعریف_کرو CURL_META_IMAP_CONN   "meta:proto:imap:conn"

تعريفنوع تعداد {
  IMAP_STOP,         /* do nothing state, stops the state machine */
  IMAP_SERVERGREET,  /* waiting for the initial greeting immediately after
                        a connect */
  IMAP_CAPABILITY,
  IMAP_STARTTLS,
  IMAP_UPGRADETLS,   /* asynchronously upgrade the connection to SSL/TLS
                       (multi mode only) */
  IMAP_AUTHENTICATE,
  IMAP_LOGIN,
  IMAP_LIST,
  IMAP_SELECT,
  IMAP_FETCH,
  IMAP_FETCH_FINAL,
  IMAP_APPEND,
  IMAP_APPEND_FINAL,
  IMAP_تلاش,
  IMAP_LOGOUT,
  IMAP_آخری          /* never used */
} imapstate;

/* imap_conn is used for struct connection-oriented data */
بنية imap_conn {
  بنية pingpong pp;
  بنية SASL sasl;           /* SASL-related parameters */
  حرف *mailbox;              /* The last selected mailbox */
  imapstate state;            /* Always use imap.c:state() to change state! */
  بنانشانی صحيح mb_uidvalidity; /* UIDVALIDITY parsed from select response */
  حرف resptag[5];            /* Response tag to wait for */
  بنانشانی حرف preftype;     /* Preferred authentication type */
  بنانشانی حرف cmdid;        /* Last used command ID */
  ثنائی_ہندسہ(ssldone);               /* Is connect() over SSL done? */
  ثنائی_ہندسہ(preauth);               /* Is this connection PREAUTH? */
  ثنائی_ہندسہ(tls_supported);         /* StartTLS capability supported by server */
  ثنائی_ہندسہ(login_disabled);        /* LOGIN command disabled by server */
  ثنائی_ہندسہ(ir_supported);          /* Initial response supported by server */
  ثنائی_ہندسہ(mb_uidvalidity_سیٹ);
};

/* This IMAP struct is used in the Curl_easy. All IMAP data that is
   connection-oriented must be in imap_conn to properly deal with the fact that
   perhaps the Curl_easy is changed between the times the connection is
   used. */
بنية IMAP {
  curl_pp_transfer transfer;
  حرف *mailbox;          /* Mailbox to select */
  حرف *uid;              /* Message UID to fetch */
  حرف *mindex;           /* Index in mail box of mail to fetch */
  حرف *section;          /* Message SECTION to fetch */
  حرف *partial;          /* Message PARTIAL to fetch */
  حرف *query;            /* Query to search for */
  حرف *custom;           /* Custom request */
  حرف *custom_params;    /* Parameters for the custom request */
  بنانشانی صحيح uidvalidity; /* UIDVALIDITY to check in select */
  ثنائی_ہندسہ(uidvalidity_سیٹ);
};

#تعریف_کرو IMAP_RESP_OK       1
#تعریف_کرو IMAP_RESP_NOT_OK   2
#تعریف_کرو IMAP_RESP_PREAUTH  3

بنية ulbits {
  صحيح ثنائی_ہندسہ_2;
  ثابت حرف *flag;
};

/***********************************************************************
 *
 * imap_sendf()
 *
 * Sends the formatted string as an IMAP command to the server.
 *
 * Designed to never block.
 */
ساكن CURLcode imap_sendf(بنية Curl_easy *انکڑے,
                           بنية imap_conn *imapc,
                           ثابت حرف *fmt, ...) CURL_PRINTF(3, 4);
ساكن CURLcode imap_sendf(بنية Curl_easy *انکڑے,
                           بنية imap_conn *imapc,
                           ثابت حرف *fmt, ...)
{
  CURLcode نتیجہ = CURLE_OK;
  va_list ap;

  DEBUGASSERT(fmt);

  /* Calculate the tag based on the connection ID and command ID */
  curl_msnprintf(imapc->resptag, حجم(imapc->resptag), "%c%03d",
                 'A' + curlx_sltosi((لما)(انکڑے->conn->connection_id % 26)),
                 ++imapc->cmdid);

  va_start(ap, fmt);
  نتیجہ = Curl_pp_vsendf(انکڑے, &imapc->pp, imapc->resptag, fmt, ap);
  va_end(ap);
  أرجع نتیجہ;
}

/***********************************************************************
 *
 * imap_atom()
 *
 * Checks the input string for characters that need escaping and returns an
 * atom ready for sending to the server.
 *
 * The returned string needs to be freed.
 *
 */
ساكن حرف *imap_atom(ثابت حرف *str, bool escape_only)
{
  بنية dynbuf سطر;
  size_t nclean;
  size_t len;

  إذا(!str)
    أرجع عدم;

  len = لڑی_لمبائی(str);
  nclean = strcspn(str, "() {%*]\\\"");
  إذا(len == nclean)
    /* nothing to escape, return a strdup */
    أرجع curlx_strdup(str);

  curlx_dyn_init(&سطر, 2000);

  إذا(!escape_only && curlx_dyn_addn(&سطر, "\"", 1))
    أرجع عدم;

  طالما(*str) {
    إذا((*str == '\\' || *str == '"') &&
       curlx_dyn_addn(&سطر, "\\", 1))
      أرجع عدم;
    إذا(curlx_dyn_addn(&سطر, str, 1))
      أرجع عدم;
    str++;
  }

  إذا(!escape_only && curlx_dyn_addn(&سطر, "\"", 1))
    أرجع عدم;

  أرجع curlx_dyn_ptr(&سطر);
}

/*
 * Finds the start of a literal '{size}' in line, skipping over quoted strings.
 */
ساكن ثابت حرف *imap_لبھو_literal(ثابت حرف *سطر, size_t len)
{
  ثابت حرف *سرا = سطر + len;
  bool وچ_quote = FALSE;

  طالما(سطر < سرا) {
    إذا(وچ_quote) {
      إذا(*سطر == '\\' && (سطر + 1) < سرا) {
        سطر += 2;
        استمر;
      }
      إذا(*سطر == '"')
        وچ_quote = FALSE;
    }
    وإلا {
      إذا(*سطر == '"')
        وچ_quote = TRUE;
      وإلا إذا(*سطر == '{')
        أرجع سطر;
    }
    سطر++;
  }
  أرجع عدم;
}

/***********************************************************************
 *
 * imap_matchresp()
 *
 * Determines whether the untagged response is related to the specified
 * command by checking if it is in format "* <command-name> ..." or
 * "* <number> <command-name> ...".
 *
 * The "* " marker is assumed to have already been checked by the caller.
 */
ساكن bool imap_matchresp(ثابت حرف *سطر, size_t len, ثابت حرف *cmd)
{
  ثابت حرف *سرا = سطر + len;
  size_t cmd_len = لڑی_لمبائی(cmd);

  /* Skip the untagged response marker */
  سطر += 2;

  /* Do we have a number after the marker? */
  إذا(سطر < سرا && ISDIGIT(*سطر)) {
    /* Skip the number */
    افعل
      سطر++;
    طالما(سطر < سرا && ISDIGIT(*سطر));

    /* Do we have the space character? */
    إذا(سطر == سرا || *سطر != ' ')
      أرجع FALSE;

    سطر++;
  }

  /* Does the command name match and is it followed by a space character or at
     the end of line? */
  إذا(سطر + cmd_len <= سرا && curl_strnequal(سطر, cmd, cmd_len) &&
     (سطر[cmd_len] == ' ' || سطر + cmd_len + 2 == سرا))
    أرجع TRUE;

  أرجع FALSE;
}

/***********************************************************************
 *
 * imap_endofresp()
 *
 * Checks whether the given string is a valid tagged, untagged or continuation
 * response which can be processed by the response handler.
 */
ساكن bool imap_endofresp(بنية Curl_easy *انکڑے, بنية connectdata *conn,
                           ثابت حرف *سطر, size_t len, صحيح *resp)
{
  بنية imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  بنية IMAP *imap = Curl_meta_get(انکڑے, CURL_META_IMAP_EASY);
  ثابت حرف *id;
  size_t id_len;

  DEBUGASSERT(imapc);
  DEBUGASSERT(imap);
  إذا(!imapc || !imap)
    أرجع FALSE;

  /* Do we have a tagged command response? */
  id = imapc->resptag;
  id_len = لڑی_لمبائی(id);
  إذا(len >= id_len + 1 && !memcmp(id, سطر, id_len) && سطر[id_len] == ' ') {
    سطر += id_len + 1;
    len -= id_len + 1;

    إذا(len >= 2 && !memcmp(سطر, "OK", 2))
      *resp = IMAP_RESP_OK;
    وإلا إذا(len >= 7 && !memcmp(سطر, "PREAUTH", 7))
      *resp = IMAP_RESP_PREAUTH;
    وإلا
      *resp = IMAP_RESP_NOT_OK;

    أرجع TRUE;
  }

  /* Do we have an untagged command response? */
  إذا(len >= 2 && !memcmp("* ", سطر, 2)) {
    بدل(imapc->state) {
    /* States which are interested in untagged responses */
    حالة IMAP_CAPABILITY:
      إذا(!imap_matchresp(سطر, len, "CAPABILITY"))
        أرجع FALSE;
      اكسر;

    حالة IMAP_LIST:
      إذا((!imap->custom && !imap_matchresp(سطر, len, "LIST")) ||
         (imap->custom && !imap_matchresp(سطر, len, imap->custom) &&
          (!curl_strequal(imap->custom, "STORE") ||
           !imap_matchresp(سطر, len, "FETCH")) &&
          !curl_strequal(imap->custom, "SELECT") &&
          !curl_strequal(imap->custom, "EXAMINE") &&
          !curl_strequal(imap->custom, "SEARCH") &&
          !curl_strequal(imap->custom, "EXPUNGE") &&
          !curl_strequal(imap->custom, "LSUB") &&
          !curl_strequal(imap->custom, "UID") &&
          !curl_strequal(imap->custom, "GETQUOTAROOT") &&
          !curl_strequal(imap->custom, "NOOP")))
        أرجع FALSE;
      اكسر;

    حالة IMAP_SELECT:
      /* SELECT is special in that its untagged responses do not have a
         common prefix so accept anything! */
      اكسر;

    حالة IMAP_FETCH:
      إذا(!imap_matchresp(سطر, len, "FETCH"))
        أرجع FALSE;
      اكسر;

    حالة IMAP_تلاش:
      إذا(!imap_matchresp(سطر, len, "SEARCH"))
        أرجع FALSE;
      اكسر;

    /* Ignore other untagged responses */
    افتراضي:
      أرجع FALSE;
    }

    *resp = '*';
    أرجع TRUE;
  }

  /* Do we have a continuation response? This should be a + symbol followed by
     a space and optionally some text as per RFC-3501 for the AUTHENTICATE and
     APPEND commands and as outlined in Section 4. Examples of RFC-4959 but
     some email servers ignore this and only send a single + instead. */
  إذا(!imap->custom && ((len == 3 && سطر[0] == '+') ||
                       (len >= 2 && !memcmp("+ ", سطر, 2)))) {
    بدل(imapc->state) {
    /* States which are interested in continuation responses */
    حالة IMAP_AUTHENTICATE:
    حالة IMAP_APPEND:
      *resp = '+';
      اكسر;

    افتراضي:
      failf(انکڑے, "Unexpected continuation response");
      *resp = -1;
      اكسر;
    }

    أرجع TRUE;
  }

  أرجع FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * imap_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ساكن CURLcode imap_get_سنیہا(بنية Curl_easy *انکڑے, بنية bufref *باہرلا)
{
  بنية imap_conn *imapc =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_IMAP_CONN);
  حرف *سنیہا;
  size_t len;

  إذا(!imapc)
    أرجع CURLE_FAILED_INIT;

  سنیہا = curlx_dyn_ptr(&imapc->pp.recvbuf);
  len = imapc->pp.nfinal;
  إذا(len > 2) {
    /* Find the start of the message */
    len -= 2;
    لكل(سنیہا += 2; ISBLANK(*سنیہا); سنیہا++, len--)
      ;

    /* Find the end of the message */
    طالما(len--)
      إذا(!ISNEWLINE(سنیہا[len]) && !ISBLANK(سنیہا[len]))
        اكسر;

    /* Terminate the message */
    سنیہا[++len] = '\0';
    Curl_bufref_سیٹ(باہرلا, سنیہا, len, عدم);
  }
  وإلا
    /* junk input => zero length output */
    Curl_bufref_سیٹ(باہرلا, "", 0, عدم);

  أرجع CURLE_OK;
}

/***********************************************************************
 *
 * imap_state()
 *
 * This is the ONLY way to change IMAP state!
 */
ساكن فراغ imap_state(بنية Curl_easy *انکڑے,
                       بنية imap_conn *imapc,
                       imapstate newstate)
{
#إذا defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ساكن ثابت حرف * ثابت ناں[] = {
    "STOP",
    "SERVERGREET",
    "CAPABILITY",
    "STARTTLS",
    "UPGRADETLS",
    "AUTHENTICATE",
    "LOGIN",
    "LIST",
    "SELECT",
    "FETCH",
    "FETCH_FINAL",
    "APPEND",
    "APPEND_FINAL",
    "SEARCH",
    "LOGOUT",
    /*آخری*/
  };

  إذا(imapc->state != newstate)
    infof(انکڑے, "IMAP %p state change from %s to %s",
          (فراغ *)imapc, ناں[imapc->state], ناں[newstate]);
#وإلا
  (فراغ)انکڑے;
#جے_انت
  imapc->state = newstate;
}

/***********************************************************************
 *
 * imap_perform_capability()
 *
 * Sends the CAPABILITY command in order to obtain a list of server side
 * supported capabilities.
 */
ساكن CURLcode imap_perform_capability(بنية Curl_easy *انکڑے,
                                        بنية imap_conn *imapc)
{
  CURLcode نتیجہ = CURLE_OK;

  imapc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  imapc->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  imapc->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPABILITY command */
  نتیجہ = imap_sendf(انکڑے, imapc, "CAPABILITY");

  إذا(!نتیجہ)
    imap_state(انکڑے, imapc, IMAP_CAPABILITY);

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * imap_perform_starttls()
 *
 * Sends the STARTTLS command to start the upgrade to TLS.
 */
ساكن CURLcode imap_perform_starttls(بنية Curl_easy *انکڑے,
                                      بنية imap_conn *imapc)
{
  /* Send the STARTTLS command */
  CURLcode نتیجہ = imap_sendf(انکڑے, imapc, "STARTTLS");

  إذا(!نتیجہ)
    imap_state(انکڑے, imapc, IMAP_STARTTLS);

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * imap_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ساكن CURLcode imap_perform_upgrade_tls(بنية Curl_easy *انکڑے,
                                         بنية imap_conn *imapc,
                                         بنية connectdata *conn)
{
#جے_تعریف_شدہ USE_SSL
  /* Start the SSL connection */
  CURLcode نتیجہ;
  bool ssldone = FALSE;

  إذا(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    نتیجہ = Curl_ssl_cfilter_جوڑو(
      انکڑے, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    إذا(نتیجہ)
      اذهب باہرلا;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_imaps;
  }

  DEBUGASSERT(!imapc->ssldone);
  نتیجہ = Curl_conn_connect(انکڑے, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(انکڑے, "imap_perform_upgrade_tls, connect -> %d, %d",
               (صحيح)نتیجہ, ssldone));
  إذا(!نتیجہ && ssldone) {
    imapc->ssldone = ssldone;
    /* perform CAPA now, changes imapc->state out of IMAP_UPGRADETLS */
    نتیجہ = imap_perform_capability(انکڑے, imapc);
  }
باہرلا:
  أرجع نتیجہ;
#وإلا
  (فراغ)انکڑے;
  (فراغ)imapc;
  (فراغ)conn;
  أرجع CURLE_NOT_BUILT_وچ;
#جے_انت
}

/***********************************************************************
 *
 * imap_perform_login()
 *
 * Sends a clear text LOGIN command to authenticate with.
 */
ساكن CURLcode imap_perform_login(بنية Curl_easy *انکڑے,
                                   بنية imap_conn *imapc,
                                   بنية connectdata *conn)
{
  CURLcode نتیجہ = CURLE_OK;
  حرف *user;
  حرف *passwd;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  إذا(!conn->creds) {
    imap_state(انکڑے, imapc, IMAP_STOP);

    أرجع نتیجہ;
  }

  /* Make sure the username and password are in the correct atom format */
  user = imap_atom(Curl_creds_user(conn->creds), FALSE);
  passwd = imap_atom(Curl_creds_passwd(conn->creds), FALSE);

  /* Send the LOGIN command */
  نتیجہ = imap_sendf(انکڑے, imapc, "LOGIN %s %s", user ? user : "",
                      passwd ? passwd : "");

  curlx_free(user);
  curlx_strzero(passwd);
  curlx_free(passwd);

  إذا(!نتیجہ)
    imap_state(انکڑے, imapc, IMAP_LOGIN);

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * imap_perform_authenticate()
 *
 * Sends an AUTHENTICATE command allowing the client to login with the given
 * SASL authentication mechanism.
 */
ساكن CURLcode imap_perform_authenticate(بنية Curl_easy *انکڑے,
                                          ثابت حرف *mech,
                                          ثابت بنية bufref *initresp)
{
  بنية imap_conn *imapc =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_IMAP_CONN);
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *ir = Curl_bufref_ptr(initresp);

  إذا(!imapc)
    أرجع CURLE_FAILED_INIT;
  إذا(ir) {
    /* Send the AUTHENTICATE command with the initial response */
    نتیجہ = imap_sendf(انکڑے, imapc, "AUTHENTICATE %s %s",
                        mech, *ir ? ir : "=");
  }
  وإلا {
    /* Send the AUTHENTICATE command */
    نتیجہ = imap_sendf(انکڑے, imapc, "AUTHENTICATE %s", mech);
  }

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * imap_continue_authenticate()
 *
 * Sends SASL continuation data.
 */
ساكن CURLcode imap_continue_authenticate(بنية Curl_easy *انکڑے,
                                           ثابت حرف *mech,
                                           ثابت بنية bufref *resp)
{
  بنية imap_conn *imapc =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_IMAP_CONN);

  (فراغ)mech;
  إذا(!imapc)
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_pp_sendf(انکڑے, &imapc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * imap_cancel_authenticate()
 *
 * Sends SASL cancellation.
 */
ساكن CURLcode imap_cancel_authenticate(بنية Curl_easy *انکڑے,
                                         ثابت حرف *mech)
{
  بنية imap_conn *imapc =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_IMAP_CONN);

  (فراغ)mech;
  إذا(!imapc)
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_pp_sendf(انکڑے, &imapc->pp, "*");
}

/***********************************************************************
 *
 * imap_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to clear text should a common
 * mechanism not be available between the client and server.
 */
ساكن CURLcode imap_perform_authentication(بنية Curl_easy *انکڑے,
                                            بنية imap_conn *imapc)
{
  CURLcode نتیجہ = CURLE_OK;
  saslprogress ترقی;

  /* Check if already authenticated OR if there is enough data to authenticate
     with and end the connect phase if we do not */
  إذا(imapc->preauth ||
     !Curl_sasl_can_authenticate(&imapc->sasl, انکڑے)) {
    imap_state(انکڑے, imapc, IMAP_STOP);
    أرجع نتیجہ;
  }

  /* Calculate the SASL login details */
  نتیجہ = Curl_sasl_شروع(&imapc->sasl, انکڑے, (bool)imapc->ir_supported,
                           &ترقی);
  إذا(!نتیجہ) {
    إذا(ترقی == SASL_INPROGRESS)
      imap_state(انکڑے, imapc, IMAP_AUTHENTICATE);
    وإلا إذا(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
      /* Perform clear text authentication */
      نتیجہ = imap_perform_login(انکڑے, imapc, انکڑے->conn);
    وإلا
      نتیجہ = Curl_sasl_is_blocked(&imapc->sasl, انکڑے);
  }

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * imap_perform_list()
 *
 * Sends a LIST command or an alternative custom request.
 */
ساكن CURLcode imap_perform_list(بنية Curl_easy *انکڑے,
                                  بنية imap_conn *imapc,
                                  بنية IMAP *imap)
{
  CURLcode نتیجہ = CURLE_OK;

  إذا(imap->custom)
    /* Send the custom request */
    نتیجہ = imap_sendf(انکڑے, imapc, "%s%s", imap->custom,
                        imap->custom_params ? imap->custom_params : "");
  وإلا {
    /* Make sure the mailbox is in the correct atom format if necessary */
    حرف *mailbox = imap->mailbox ? imap_atom(imap->mailbox, TRUE)
                                  : curlx_strdup("");
    إذا(!mailbox)
      أرجع CURLE_باہرلا_دا_حافظہ;

    /* Send the LIST command */
    نتیجہ = imap_sendf(انکڑے, imapc, "LIST \"%s\" *", mailbox);

    curlx_free(mailbox);
  }

  إذا(!نتیجہ)
    imap_state(انکڑے, imapc, IMAP_LIST);

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * imap_perform_select()
 *
 * Sends a SELECT command to ask the server to change the selected mailbox.
 */
ساكن CURLcode imap_perform_select(بنية Curl_easy *انکڑے,
                                    بنية imap_conn *imapc,
                                    بنية IMAP *imap)
{
  CURLcode نتیجہ = CURLE_OK;
  حرف *mailbox;

  /* Invalidate old information as we are switching mailboxes */
  curlx_safefree(imapc->mailbox);
  imapc->mb_uidvalidity_سیٹ = FALSE;

  /* Check we have a mailbox */
  إذا(!imap->mailbox) {
    failf(انکڑے, "Cannot SELECT without a mailbox.");
    أرجع CURLE_URL_MALFORMAT;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  إذا(!mailbox)
    أرجع CURLE_باہرلا_دا_حافظہ;

  /* Send the SELECT command */
  نتیجہ = imap_sendf(انکڑے, imapc, "SELECT %s", mailbox);

  curlx_free(mailbox);

  إذا(!نتیجہ)
    imap_state(انکڑے, imapc, IMAP_SELECT);

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * imap_perform_fetch()
 *
 * Sends a FETCH command to initiate the download of a message.
 */
ساكن CURLcode imap_perform_fetch(بنية Curl_easy *انکڑے,
                                   بنية imap_conn *imapc,
                                   بنية IMAP *imap)
{
  CURLcode نتیجہ = CURLE_OK;
  /* Check we have a UID */
  إذا(imap->uid) {

    /* Send the FETCH command */
    إذا(imap->partial)
      نتیجہ = imap_sendf(انکڑے, imapc, "UID FETCH %s BODY[%s]<%s>",
                          imap->uid, imap->section ? imap->section : "",
                          imap->partial);
    وإلا
      نتیجہ = imap_sendf(انکڑے, imapc, "UID FETCH %s BODY[%s]",
                          imap->uid, imap->section ? imap->section : "");
  }
  وإلا إذا(imap->mindex) {
    /* Send the FETCH command */
    إذا(imap->partial)
      نتیجہ = imap_sendf(انکڑے, imapc, "FETCH %s BODY[%s]<%s>",
                          imap->mindex, imap->section ? imap->section : "",
                          imap->partial);
    وإلا
      نتیجہ = imap_sendf(انکڑے, imapc, "FETCH %s BODY[%s]",
                          imap->mindex, imap->section ? imap->section : "");
  }
  وإلا {
    failf(انکڑے, "Cannot FETCH without a UID.");
    أرجع CURLE_URL_MALFORMAT;
  }
  إذا(!نتیجہ)
    imap_state(انکڑے, imapc, IMAP_FETCH);

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * imap_perform_append()
 *
 * Sends an APPEND command to initiate the upload of a message.
 */
ساكن CURLcode imap_perform_append(بنية Curl_easy *انکڑے,
                                    بنية imap_conn *imapc,
                                    بنية IMAP *imap)
{
  CURLcode نتیجہ = CURLE_OK;
  حرف *mailbox;
  بنية dynbuf flags;

  /* Check we have a mailbox */
  إذا(!imap->mailbox) {
    failf(انکڑے, "Cannot APPEND without a mailbox.");
    أرجع CURLE_URL_MALFORMAT;
  }

#جے_بنا_تعریف CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  إذا(IS_MIME_POST(انکڑے)) {
    curl_mimepart *postp = انکڑے->سیٹ.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(بنانشانی صحيح)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_سرخیاں(postp, انکڑے->سیٹ.سرخیاں, 0);
    نتیجہ = Curl_mime_prepare_سرخیاں(انکڑے, postp, عدم,
                                       عدم, MIMESTRATEGY_MAIL);

    إذا(!نتیجہ && !Curl_checkheaders(انکڑے, STRCONST("Mime-Version")))
      نتیجہ = Curl_mime_جوڑو_سرخی(&postp->curlheaders, "Mime-Version: 1.0");
    إذا(!نتیجہ)
      نتیجہ = Curl_creader_سیٹ_mime(انکڑے, postp);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }
  وإلا
#جے_انت
  {
    نتیجہ = Curl_creader_سیٹ_fread(انکڑے, انکڑے->state.infilesize);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  /* Check we know the size of the upload. This takes all readers
   * into account. Especially crlf conversions which make the size
   * unpredictable, e.g. -1. */
  انکڑے->state.infilesize = Curl_creader_کل_لمائی(انکڑے);
  إذا(انکڑے->state.infilesize < 0) {
    إذا(انکڑے->سیٹ.crlf)
      failf(انکڑے, "Cannot APPEND with CRLF conversion making size unknown");
    وإلا
      failf(انکڑے, "Cannot APPEND with unknown input file size");
    أرجع CURLE_UPLOAD_FAILED;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  إذا(!mailbox)
    أرجع CURLE_باہرلا_دا_حافظہ;

  /* Generate flags string and send the APPEND command */
  curlx_dyn_init(&flags, 100);
  إذا(انکڑے->سیٹ.upload_flags) {
    صحيح i;
    بنية ulbits ulflag[] = {
      { CURLULFLAG_ANSWERED, "Answered" },
      { CURLULFLAG_DELETED, "Deleted" },
      { CURLULFLAG_DRAFT, "Draft" },
      { CURLULFLAG_FLAGGED, "Flagged" },
      { CURLULFLAG_تکیا, "Seen" },
      { 0, عدم }
    };

    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    إذا(curlx_dyn_جوڑو(&flags, " (")) {
      اذهب cleanup;
    }

    لكل(i = 0; ulflag[i].ثنائی_ہندسہ_2; i++) {
      إذا(انکڑے->سیٹ.upload_flags & ulflag[i].ثنائی_ہندسہ_2 &&
         ((curlx_dyn_len(&flags) > 2 && curlx_dyn_جوڑو(&flags, " ")) ||
          curlx_dyn_جوڑو(&flags, "\\") ||
          curlx_dyn_جوڑو(&flags, ulflag[i].flag)))
        اذهب cleanup;
    }

    إذا(curlx_dyn_جوڑو(&flags, ")"))
      اذهب cleanup;
  }
  وإلا إذا(curlx_dyn_جوڑو(&flags, ""))
    اذهب cleanup;

  نتیجہ = imap_sendf(انکڑے, imapc, "APPEND %s%s {%" FMT_OFF_T "}",
                      mailbox, curlx_dyn_ptr(&flags), انکڑے->state.infilesize);

cleanup:
  curlx_dyn_free(&flags);
  curlx_free(mailbox);

  إذا(!نتیجہ)
    imap_state(انکڑے, imapc, IMAP_APPEND);

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * imap_perform_search()
 *
 * Sends a SEARCH command.
 */
ساكن CURLcode imap_perform_تلاش(بنية Curl_easy *انکڑے,
                                    بنية imap_conn *imapc,
                                    بنية IMAP *imap)
{
  CURLcode نتیجہ = CURLE_OK;

  /* Check we have a query string */
  إذا(!imap->query) {
    failf(انکڑے, "Cannot SEARCH without a query string.");
    أرجع CURLE_URL_MALFORMAT;
  }

  /* Send the SEARCH command */
  نتیجہ = imap_sendf(انکڑے, imapc, "SEARCH %s", imap->query);

  إذا(!نتیجہ)
    imap_state(انکڑے, imapc, IMAP_تلاش);

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * imap_perform_logout()
 *
 * Performs the logout action prior to sclose() being called.
 */
ساكن CURLcode imap_perform_logout(بنية Curl_easy *انکڑے,
                                    بنية imap_conn *imapc)
{
  /* Send the LOGOUT command */
  CURLcode نتیجہ = imap_sendf(انکڑے, imapc, "LOGOUT");

  إذا(!نتیجہ)
    imap_state(انکڑے, imapc, IMAP_LOGOUT);

  أرجع نتیجہ;
}

/* For the initial server greeting */
ساكن CURLcode imap_state_servergreet_resp(بنية Curl_easy *انکڑے,
                                            بنية imap_conn *imapc,
                                            صحيح imapcode,
                                            imapstate instate)
{
  (فراغ)instate;

  إذا(imapcode == IMAP_RESP_PREAUTH) {
    /* PREAUTH */
    imapc->preauth = TRUE;
    infof(انکڑے, "PREAUTH connection, already authenticated");
  }
  وإلا إذا(imapcode != IMAP_RESP_OK) {
    failf(انکڑے, "Got unexpected imap-server response");
    أرجع CURLE_WEIRD_SERVER_جواب;
  }

  أرجع imap_perform_capability(انکڑے, imapc);
}

/* For CAPABILITY responses */
ساكن CURLcode imap_state_capability_resp(بنية Curl_easy *انکڑے,
                                           بنية imap_conn *imapc,
                                           صحيح imapcode,
                                           imapstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *سطر = curlx_dyn_ptr(&imapc->pp.recvbuf);

  (فراغ)instate;

  /* Do we have an untagged response? */
  إذا(imapcode == '*') {
    سطر += 2;

    /* Loop through the data line */
    لكل(;;) {
      size_t wordlen;
      طالما(*سطر && (ISBLANK(*سطر) || ISNEWLINE(*سطر)))
        سطر++;

      إذا(!*سطر)
        اكسر;

      /* Extract the word */
      لكل(wordlen = 0; سطر[wordlen] && !ISBLANK(سطر[wordlen]) &&
                       !ISNEWLINE(سطر[wordlen]);)
        wordlen++;

      /* Does the server support the STARTTLS capability? */
      إذا(wordlen == 8 && curl_strnequal(سطر, "STARTTLS", 8))
        imapc->tls_supported = TRUE;

      /* Has the server explicitly disabled clear text authentication? */
      وإلا إذا(wordlen == 13 && curl_strnequal(سطر, "LOGINDISABLED", 13))
        imapc->login_disabled = TRUE;

      /* Does the server support the SASL-IR capability? */
      وإلا إذا(wordlen == 7 && curl_strnequal(سطر, "SASL-IR", 7))
        imapc->ir_supported = TRUE;

      /* Do we have a SASL based authentication mechanism? */
      وإلا إذا(wordlen > 5 && curl_strnequal(سطر, "AUTH=", 5)) {
        size_t llen;
        بنانشانی چھوٹا mechbit;

        سطر += 5;
        wordlen -= 5;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(سطر, wordlen, &llen);
        إذا(mechbit && llen == wordlen)
          imapc->sasl.authmechs |= mechbit;
      }

      سطر += wordlen;
    }
  }
  وإلا إذا(انکڑے->سیٹ.use_ssl && !Curl_conn_is_ssl(انکڑے->conn, FIRSTSOCKET)) {
    /* PREAUTH is not compatible with STARTTLS. */
    إذا(imapcode == IMAP_RESP_OK && imapc->tls_supported && !imapc->preauth) {
      /* Switch to TLS connection now */
      نتیجہ = imap_perform_starttls(انکڑے, imapc);
    }
    وإلا إذا(انکڑے->سیٹ.use_ssl <= CURLUSESSL_TRY)
      نتیجہ = imap_perform_authentication(انکڑے, imapc);
    وإلا {
      failf(انکڑے, "STARTTLS not available.");
      نتیجہ = CURLE_USE_SSL_FAILED;
    }
  }
  وإلا
    نتیجہ = imap_perform_authentication(انکڑے, imapc);

  أرجع نتیجہ;
}

/* For STARTTLS responses */
ساكن CURLcode imap_state_starttls_resp(بنية Curl_easy *انکڑے,
                                         بنية imap_conn *imapc,
                                         صحيح imapcode,
                                         imapstate instate)
{
  CURLcode نتیجہ = CURLE_OK;

  (فراغ)instate;

  /* Pipelining in response is forbidden. */
  إذا(imapc->pp.حد_توں_ودھ)
    أرجع CURLE_WEIRD_SERVER_جواب;

  إذا(imapcode != IMAP_RESP_OK) {
    إذا(انکڑے->سیٹ.use_ssl != CURLUSESSL_TRY) {
      failf(انکڑے, "STARTTLS denied");
      نتیجہ = CURLE_USE_SSL_FAILED;
    }
    وإلا
      نتیجہ = imap_perform_authentication(انکڑے, imapc);
  }
  وإلا
    imap_state(انکڑے, imapc, IMAP_UPGRADETLS);

  أرجع نتیجہ;
}

/* For SASL authentication responses */
ساكن CURLcode imap_state_auth_resp(بنية Curl_easy *انکڑے,
                                     بنية imap_conn *imapc,
                                     صحيح imapcode,
                                     imapstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  saslprogress ترقی;

  (فراغ)instate;

  نتیجہ = Curl_sasl_continue(&imapc->sasl, انکڑے, imapcode, &ترقی);
  إذا(!نتیجہ)
    بدل(ترقی) {
    حالة SASL_پورا:
      imap_state(انکڑے, imapc, IMAP_STOP);  /* Authenticated */
      اكسر;
    حالة SASL_IDLE:            /* No mechanism left after cancellation */
      إذا(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
        /* Perform clear text authentication */
        نتیجہ = imap_perform_login(انکڑے, imapc, انکڑے->conn);
      وإلا {
        failf(انکڑے, "Authentication cancelled");
        نتیجہ = CURLE_LOGIN_DENIED;
      }
      اكسر;
    افتراضي:
      اكسر;
    }

  أرجع نتیجہ;
}

/* For LOGIN responses */
ساكن CURLcode imap_state_login_resp(بنية Curl_easy *انکڑے,
                                      بنية imap_conn *imapc,
                                      صحيح imapcode,
                                      imapstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  (فراغ)instate;

  إذا(imapcode != IMAP_RESP_OK) {
    failf(انکڑے, "Access denied. %c", imapcode);
    نتیجہ = CURLE_LOGIN_DENIED;
  }
  وإلا
    /* End of connect phase */
    imap_state(انکڑے, imapc, IMAP_STOP);

  أرجع نتیجہ;
}

/* Detect IMAP listings vs. downloading a single email */
ساكن bool is_custom_fetch_listing_میل(ثابت حرف *params)
{
  /* match " 1:* (FLAGS ..." or " 1,2,3 (FLAGS ..." */
  إذا(*params++ != ' ')
    أرجع FALSE;

  طالما(ISDIGIT(*params)) {
    params++;
    إذا(*params == 0)
      أرجع FALSE;
  }
  إذا(*params == ':')
    أرجع TRUE;
  إذا(*params == ',')
    أرجع TRUE;
  أرجع FALSE;
}

ساكن bool is_custom_fetch_listing(بنية IMAP *imap)
{
  /* filter out "UID FETCH 1:* (FLAGS ..." queries to list emails */
  إذا(!imap->custom)
    أرجع FALSE;
  وإلا إذا(curl_strequal(imap->custom, "FETCH") && imap->custom_params) {
    ثابت حرف *p = imap->custom_params;
    أرجع is_custom_fetch_listing_میل(p);
  }
  وإلا إذا(curl_strequal(imap->custom, "UID") && imap->custom_params &&
          curl_strnequal(imap->custom_params, " FETCH ", 7)) {
    ثابت حرف *p = imap->custom_params + 6;
    أرجع is_custom_fetch_listing_میل(p);
  }
  أرجع FALSE;
}

/* For LIST and SEARCH responses */
ساكن CURLcode imap_state_listsearch_resp(بنية Curl_easy *انکڑے,
                                           بنية imap_conn *imapc,
                                           صحيح imapcode,
                                           imapstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *سطر = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  بنية IMAP *imap = Curl_meta_get(انکڑے, CURL_META_IMAP_EASY);

  DEBUGASSERT(imap);
  إذا(!imap)
    أرجع CURLE_FAILED_INIT;
  (فراغ)instate;

  إذا(imapcode == '*' && is_custom_fetch_listing(imap)) {
    /* custom FETCH or UID FETCH for listing is not handled here */
  }
  وإلا إذا(imapcode == '*') {
    /* Check if this response contains a literal (e.g. FETCH responses with
       body data). Literal syntax is {size}\r\n */
    ثابت حرف *cr = memchr(سطر, '\r', len);
    size_t سطر_len = cr ? (size_t)(cr - سطر) : len;
    ثابت حرف *ptr = imap_لبھو_literal(سطر, سطر_len);
    إذا(ptr) {
      curl_off_t ناپ = 0;
      bool parsed = FALSE;
      ptr++;
      إذا(!curlx_str_نمبر(&ptr, &ناپ, CURL_OFF_T_MAX) &&
         !curlx_str_single(&ptr, '}'))
        parsed = TRUE;

      إذا(parsed) {
        بنية pingpong *pp = &imapc->pp;
        size_t بفر_len = curlx_dyn_len(&pp->recvbuf);
        size_t after_سرخی = بفر_len - pp->nfinal;

        /* This is a literal response, setup to receive the body data */
        infof(انکڑے, "Found %" FMT_OFF_T " bytes to download", ناپ);

        /* First write the header line */
        نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY, سطر, len);
        إذا(نتیجہ)
          أرجع نتیجہ;

        /* Handle data already in buffer after the header line */
        إذا(after_سرخی > 0) {
          /* There is already data in the buffer that is part of the literal
             body or subsequent responses */
          size_t chunk = after_سرخی;

          /* Keep only the data after the header line */
          curlx_dyn_پوچھ(&pp->recvbuf, chunk);
          pp->nfinal = 0; /* done */

          /* Limit chunk to the literal size */
          إذا((curl_off_t)chunk > ناپ)
            chunk = (size_t)ناپ;

          إذا(chunk) {
            /* Write the literal body data */
            نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY,
                                       curlx_dyn_ptr(&pp->recvbuf), chunk);
            إذا(نتیجہ)
              أرجع نتیجہ;
          }

          /* Handle remaining data in buffer (either more literal data or
             subsequent responses) */
          إذا(after_سرخی > chunk) {
            /* Keep the data after the literal body */
            pp->حد_توں_ودھ = after_سرخی - chunk;
            curlx_dyn_پوچھ(&pp->recvbuf, pp->حد_توں_ودھ);
          }
          وإلا {
            pp->حد_توں_ودھ = 0;
            curlx_dyn_reset(&pp->recvbuf);
          }
        }
        وإلا {
          /* No data in buffer yet, reset overflow */
          pp->حد_توں_ودھ = 0;
        }

        إذا((CURL_OFF_T_MAX - ناپ) < (curl_off_t)len)
          /* unlikely to actually be a transfer this big, but avoid integer
حد_توں_ودھ*/
          ناپ = CURL_OFF_T_MAX;
        وإلا
          ناپ += len;

        /* Progress size includes both header line and literal body */
        Curl_pgrs_سیٹ_Download_ناپ(انکڑے, ناپ);

        إذا(انکڑے->req.bytecount == ناپ)
          /* All data already transferred (header + literal body) */
          Curl_xfer_تیاری_nop(انکڑے);
        وإلا {
          /* Setup to receive the literal body data.
             maxdownload and transfer size include both header line and
             literal body */
          انکڑے->req.maxdownload = ناپ;
          Curl_xfer_تیاری_recv(انکڑے, FIRSTSOCKET, ناپ);
        }
        /* End of DO phase */
        imap_state(انکڑے, imapc, IMAP_STOP);
      }
      وإلا {
        /* Failed to parse literal, write the line */
        نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY, سطر, len);
      }
    }
    وإلا {
      /* No literal, write the line as-is */
      نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY, سطر, len);
    }
  }
  وإلا إذا(imapcode != IMAP_RESP_OK)
    نتیجہ = CURLE_QUOTE_ERROR;
  وإلا
    /* End of DO phase */
    imap_state(انکڑے, imapc, IMAP_STOP);

  أرجع نتیجہ;
}

/* For SELECT responses */
ساكن CURLcode imap_state_select_resp(بنية Curl_easy *انکڑے,
                                       بنية imap_conn *imapc,
                                       بنية IMAP *imap,
                                       صحيح imapcode,
                                       imapstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  (فراغ)instate;

  إذا(imapcode == '*') {
    /* See if this is an UIDVALIDITY response */
    ثابت حرف *سطر = curlx_dyn_ptr(&imapc->pp.recvbuf);
    size_t len = curlx_dyn_len(&imapc->pp.recvbuf);
    إذا((len >= 18) && checkprefix("OK [UIDVALIDITY ", &سطر[2])) {
      curl_off_t قدر;
      ثابت حرف *p = &سطر[2] + CURL_CSTRLEN("OK [UIDVALIDITY ");
      إذا(!curlx_str_نمبر(&p, &قدر, UINT_MAX)) {
        imapc->mb_uidvalidity = (بنانشانی صحيح)قدر;
        imapc->mb_uidvalidity_سیٹ = TRUE;
      }
    }
  }
  وإلا إذا(imapcode == IMAP_RESP_OK) {
    /* Check if the UIDVALIDITY has been specified and matches */
    إذا(imap->uidvalidity_سیٹ && imapc->mb_uidvalidity_سیٹ &&
       (imap->uidvalidity != imapc->mb_uidvalidity)) {
      failf(انکڑے, "Mailbox UIDVALIDITY has changed");
      نتیجہ = CURLE_REMOTE_FILE_NOT_لبھیا;
    }
    وإلا {
      /* Note the currently opened mailbox on this connection */
      DEBUGASSERT(!imapc->mailbox);
      imapc->mailbox = curlx_strdup(imap->mailbox);
      إذا(!imapc->mailbox)
        أرجع CURLE_باہرلا_دا_حافظہ;

      إذا(imap->custom)
        نتیجہ = imap_perform_list(انکڑے, imapc, imap);
      وإلا إذا(imap->query)
        نتیجہ = imap_perform_تلاش(انکڑے, imapc, imap);
      وإلا
        نتیجہ = imap_perform_fetch(انکڑے, imapc, imap);
    }
  }
  وإلا {
    failf(انکڑے, "Select failed");
    نتیجہ = CURLE_LOGIN_DENIED;
  }

  أرجع نتیجہ;
}

/* For the (first line of the) FETCH responses */
ساكن CURLcode imap_state_fetch_resp(بنية Curl_easy *انکڑے,
                                      بنية imap_conn *imapc,
                                      صحيح imapcode,
                                      imapstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية pingpong *pp = &imapc->pp;
  ثابت حرف *ptr = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  bool parsed = FALSE;
  curl_off_t ناپ = 0;

  (فراغ)instate;

  إذا(imapcode != '*') {
    Curl_pgrs_سیٹ_Download_ناپ(انکڑے, -1);
    imap_state(انکڑے, imapc, IMAP_STOP);
    أرجع CURLE_REMOTE_FILE_NOT_لبھیا;
  }

  /* Something like this is received "* 1 FETCH (BODY[TEXT] {2021}\r" so parse
     the continuation data contained within the curly brackets */
  ptr = imap_لبھو_literal(ptr, len);
  إذا(ptr) {
    ptr++;
    إذا(!curlx_str_نمبر(&ptr, &ناپ, CURL_OFF_T_MAX) &&
       !curlx_str_single(&ptr, '}'))
      parsed = TRUE;
  }

  إذا(parsed) {
    infof(انکڑے, "Found %" FMT_OFF_T " bytes to download", ناپ);
    Curl_pgrs_سیٹ_Download_ناپ(انکڑے, ناپ);

    إذا(pp->حد_توں_ودھ) {
      /* At this point there is a data in the receive buffer that is body
         content, send it as body and then skip it. Do note that there may
         even be additional "headers" after the body. */
      size_t chunk = pp->حد_توں_ودھ;

      /* keep only the overflow */
      curlx_dyn_پوچھ(&pp->recvbuf, chunk);
      pp->nfinal = 0; /* done */

      إذا((curl_off_t)chunk > ناپ)
        /* The conversion from curl_off_t to size_t is always fine here */
        chunk = (size_t)ناپ;

      إذا(!chunk) {
        /* no size, we are done with the data */
        imap_state(انکڑے, imapc, IMAP_STOP);
        أرجع CURLE_OK;
      }
      نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY,
                                 curlx_dyn_ptr(&pp->recvbuf), chunk);
      إذا(نتیجہ)
        أرجع نتیجہ;

      infof(انکڑے, "Written %zu bytes, %" FMT_OFF_T
            " bytes are left for transfer", chunk, (curl_off_t)(ناپ - chunk));

      /* Have we used the entire overflow or part of it?*/
      إذا(pp->حد_توں_ودھ > chunk) {
        /* remember the remaining trailing overflow data */
        pp->حد_توں_ودھ -= chunk;
        curlx_dyn_پوچھ(&pp->recvbuf, pp->حد_توں_ودھ);
      }
      وإلا {
        pp->حد_توں_ودھ = 0; /* handled */
        /* Free the cache */
        curlx_dyn_reset(&pp->recvbuf);
      }
    }

    إذا(انکڑے->req.bytecount == ناپ)
      /* The entire data is already transferred! */
      Curl_xfer_تیاری_nop(انکڑے);
    وإلا {
      /* IMAP download */
      انکڑے->req.maxdownload = ناپ;
      Curl_xfer_تیاری_recv(انکڑے, FIRSTSOCKET, ناپ);
    }
  }
  وإلا {
    /* We do not know how to parse this line */
    failf(انکڑے, "Failed to parse FETCH response.");
    نتیجہ = CURLE_WEIRD_SERVER_جواب;
  }

  /* End of DO phase */
  imap_state(انکڑے, imapc, IMAP_STOP);

  أرجع نتیجہ;
}

/* For final FETCH responses performed after the download */
ساكن CURLcode imap_state_fetch_final_resp(بنية Curl_easy *انکڑے,
                                            بنية imap_conn *imapc,
                                            صحيح imapcode,
                                            imapstate instate)
{
  CURLcode نتیجہ = CURLE_OK;

  (فراغ)instate;

  إذا(imapcode != IMAP_RESP_OK)
    نتیجہ = CURLE_WEIRD_SERVER_جواب;
  وإلا
    /* End of DONE phase */
    imap_state(انکڑے, imapc, IMAP_STOP);

  أرجع نتیجہ;
}

/* For APPEND responses */
ساكن CURLcode imap_state_append_resp(بنية Curl_easy *انکڑے,
                                       بنية imap_conn *imapc,
                                       صحيح imapcode,
                                       imapstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  (فراغ)instate;

  إذا(imapcode != '+') {
    نتیجہ = CURLE_UPLOAD_FAILED;
  }
  وإلا {
    /* Set the progress upload size */
    Curl_pgrs_سیٹ_Upload_ناپ(انکڑے, انکڑے->state.infilesize);

    /* IMAP upload */
    Curl_xfer_تیاری_send(انکڑے, FIRSTSOCKET);

    /* End of DO phase */
    imap_state(انکڑے, imapc, IMAP_STOP);
  }

  أرجع نتیجہ;
}

/* For final APPEND responses performed after the upload */
ساكن CURLcode imap_state_append_final_resp(بنية Curl_easy *انکڑے,
                                             بنية imap_conn *imapc,
                                             صحيح imapcode,
                                             imapstate instate)
{
  CURLcode نتیجہ = CURLE_OK;

  (فراغ)instate;

  إذا(imapcode != IMAP_RESP_OK)
    نتیجہ = CURLE_UPLOAD_FAILED;
  وإلا
    /* End of DONE phase */
    imap_state(انکڑے, imapc, IMAP_STOP);

  أرجع نتیجہ;
}

ساكن CURLcode imap_pp_statemachine(بنية Curl_easy *انکڑے,
                                     بنية connectdata *conn)
{
  CURLcode نتیجہ = CURLE_OK;
  صحيح imapcode;
  بنية imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  بنية IMAP *imap = Curl_meta_get(انکڑے, CURL_META_IMAP_EASY);
  بنية pingpong *pp;
  size_t nread = 0;

  إذا(!imapc || !imap)
    أرجع CURLE_FAILED_INIT;
  pp = &imapc->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not IMAP */
upgrade_tls:
  إذا(imapc->state == IMAP_UPGRADETLS) {
    نتیجہ = imap_perform_upgrade_tls(انکڑے, imapc, conn);
    إذا(نتیجہ || (imapc->state == IMAP_UPGRADETLS))
      أرجع نتیجہ;
  }

  /* Flush any data that needs to be sent */
  إذا(pp->sendleft)
    أرجع Curl_pp_flushsend(انکڑے, pp);

  افعل {
    /* Read the response from the server */
    نتیجہ = Curl_pp_readresp(انکڑے, FIRSTSOCKET, pp, &imapcode, &nread);
    إذا(نتیجہ)
      أرجع نتیجہ;

    /* Was there an error parsing the response line? */
    إذا(imapcode == -1)
      أرجع CURLE_WEIRD_SERVER_جواب;

    إذا(!imapcode)
      اكسر;

    /* We have now received a full IMAP server response */
    بدل(imapc->state) {
    حالة IMAP_SERVERGREET:
      نتیجہ = imap_state_servergreet_resp(انکڑے, imapc,
                                           imapcode, imapc->state);
      اكسر;

    حالة IMAP_CAPABILITY:
      نتیجہ = imap_state_capability_resp(انکڑے, imapc, imapcode, imapc->state);
      اكسر;

    حالة IMAP_STARTTLS:
      نتیجہ = imap_state_starttls_resp(انکڑے, imapc, imapcode, imapc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      إذا(!نتیجہ && (imapc->state == IMAP_UPGRADETLS))
        اذهب upgrade_tls;
      اكسر;

    حالة IMAP_AUTHENTICATE:
      نتیجہ = imap_state_auth_resp(انکڑے, imapc, imapcode, imapc->state);
      اكسر;

    حالة IMAP_LOGIN:
      نتیجہ = imap_state_login_resp(انکڑے, imapc, imapcode, imapc->state);
      اكسر;

    حالة IMAP_LIST:
    حالة IMAP_تلاش:
      نتیجہ = imap_state_listsearch_resp(انکڑے, imapc, imapcode, imapc->state);
      اكسر;

    حالة IMAP_SELECT:
      نتیجہ = imap_state_select_resp(انکڑے, imapc, imap,
                                      imapcode, imapc->state);
      اكسر;

    حالة IMAP_FETCH:
      نتیجہ = imap_state_fetch_resp(انکڑے, imapc, imapcode, imapc->state);
      اكسر;

    حالة IMAP_FETCH_FINAL:
      نتیجہ = imap_state_fetch_final_resp(انکڑے, imapc,
                                           imapcode, imapc->state);
      اكسر;

    حالة IMAP_APPEND:
      نتیجہ = imap_state_append_resp(انکڑے, imapc, imapcode, imapc->state);
      اكسر;

    حالة IMAP_APPEND_FINAL:
      نتیجہ = imap_state_append_final_resp(انکڑے, imapc,
                                            imapcode, imapc->state);
      اكسر;

    حالة IMAP_LOGOUT:
    افتراضي:
      /* internal error */
      imap_state(انکڑے, imapc, IMAP_STOP);
      اكسر;
    }
  } طالما(!نتیجہ && imapc->state != IMAP_STOP && Curl_pp_moredata(pp));

  أرجع نتیجہ;
}

/* Called repeatedly until done from multi.c */
ساكن CURLcode imap_multi_statemach(بنية Curl_easy *انکڑے, bool *پورا)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية imap_conn *imapc =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_IMAP_CONN);

  *پورا = FALSE;
  إذا(!imapc)
    أرجع CURLE_FAILED_INIT;
  نتیجہ = Curl_pp_statemach(انکڑے, &imapc->pp, FALSE, FALSE);
  *پورا = (imapc->state == IMAP_STOP);

  أرجع نتیجہ;
}

ساكن CURLcode imap_بلاک_statemach(بنية Curl_easy *انکڑے,
                                     بنية imap_conn *imapc,
                                     bool disconnecting)
{
  CURLcode نتیجہ = CURLE_OK;

  طالما(imapc->state != IMAP_STOP && !نتیجہ)
    نتیجہ = Curl_pp_statemach(انکڑے, &imapc->pp, TRUE, disconnecting);

  أرجع نتیجہ;
}

/* For the IMAP "protocol connect" and "doing" phases only */
ساكن CURLcode imap_pollset(بنية Curl_easy *انکڑے,
                             بنية easy_pollset *ps)
{
  بنية imap_conn *imapc =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_IMAP_CONN);
  أرجع imapc ? Curl_pp_pollset(انکڑے, &imapc->pp, ps) : CURLE_OK;
}

ساكن فراغ imap_easy_reset(بنية IMAP *imap)
{
  curlx_safefree(imap->mailbox);
  curlx_safefree(imap->uid);
  curlx_safefree(imap->mindex);
  curlx_safefree(imap->section);
  curlx_safefree(imap->partial);
  curlx_safefree(imap->query);
  curlx_safefree(imap->custom);
  curlx_safefree(imap->custom_params);
  imap->uidvalidity_سیٹ = FALSE;
  /* Clear the transfer mode for the next request */
  imap->transfer = PPTRANSFER_BODY;
}

/***********************************************************************
 *
 * imap_is_bchar()
 *
 * Portable test of whether the specified char is a "bchar" as defined in the
 * grammar of RFC-5092.
 */
ساكن bool imap_is_bchar(حرف ch)
{
  /* Performing the alnum check first with macro is faster because of ASCII
     arithmetic */
  أرجع ch && (ISALNUM(ch) || strchr(":@/&=-._~!$\'()*+,%", ch));
}

/***********************************************************************
 *
 * imap_parse_url_options()
 *
 * Parse the URL login options.
 */
ساكن CURLcode imap_parse_url_options(بنية connectdata *conn,
                                       بنية imap_conn *imapc)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *ptr = conn->options;
  bool prefer_login = FALSE;

  طالما(!نتیجہ && ptr && *ptr) {
    ثابت حرف *کلید = ptr;
    ثابت حرف *قدر;

    طالما(*ptr && *ptr != '=')
      ptr++;

    قدر = ptr + 1;

    طالما(*ptr && *ptr != ';')
      ptr++;

    إذا(curl_strnequal(کلید, "AUTH=+LOGIN", 11)) {
      /* User prefers plaintext LOGIN over any SASL, including SASL LOGIN */
      prefer_login = TRUE;
      imapc->sasl.prefmech = SASL_AUTH_NONE;
    }
    وإلا إذا(curl_strnequal(کلید, "AUTH=", 5)) {
      prefer_login = FALSE;
      نتیجہ = Curl_sasl_parse_url_auth_option(&imapc->sasl,
                                               قدر, ptr - قدر);
    }
    وإلا {
      prefer_login = FALSE;
      نتیجہ = CURLE_URL_MALFORMAT;
    }

    إذا(*ptr == ';')
      ptr++;
  }

  إذا(prefer_login)
    imapc->preftype = IMAP_TYPE_CLEARTEXT;
  وإلا {
    بدل(imapc->sasl.prefmech) {
    حالة SASL_AUTH_NONE:
      imapc->preftype = IMAP_TYPE_NONE;
      اكسر;
    حالة SASL_AUTH_DEFAULT:
      imapc->preftype = IMAP_TYPE_ANY;
      اكسر;
    افتراضي:
      imapc->preftype = IMAP_TYPE_SASL;
      اكسر;
    }
  }

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * imap_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
ساكن CURLcode imap_parse_url_رستہ(بنية Curl_easy *انکڑے,
                                    بنية IMAP *imap)
{
  /* The imap struct is already initialized in imap_connect() */
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *begin = &انکڑے->state.اتے.رستہ[1]; /* skip leading slash */
  ثابت حرف *ptr = begin;

  /* See how much of the URL is a valid path and decode it */
  طالما(imap_is_bchar(*ptr))
    ptr++;

  إذا(ptr != begin) {
    /* Remove the trailing slash if present */
    ثابت حرف *سرا = ptr;
    إذا(سرا > begin && سرا[-1] == '/')
      سرا--;

    نتیجہ = Curl_urldecode(begin, سرا - begin, &imap->mailbox, عدم,
                            REJECT_CTRL);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }
  وإلا
    imap->mailbox = عدم;

  /* There can be any number of parameters in the form ";NAME=VALUE" */
  طالما(*ptr == ';') {
    حرف *name;
    حرف *قدر;
    size_t valuelen;

    /* Find the length of the name parameter */
    begin = ++ptr;
    طالما(*ptr && *ptr != '=')
      ptr++;

    إذا(!*ptr)
      أرجع CURLE_URL_MALFORMAT;

    /* Decode the name parameter */
    نتیجہ = Curl_urldecode(begin, ptr - begin, &name, عدم,
                            REJECT_CTRL);
    إذا(نتیجہ)
      أرجع نتیجہ;

    /* Find the length of the value parameter */
    begin = ++ptr;
    طالما(imap_is_bchar(*ptr))
      ptr++;

    /* Decode the value parameter */
    نتیجہ = Curl_urldecode(begin, ptr - begin, &قدر, &valuelen,
                            REJECT_CTRL);
    إذا(نتیجہ) {
      curlx_free(name);
      أرجع نتیجہ;
    }

    DEBUGF(infof(انکڑے, "IMAP URL parameter '%s' = '%s'", name, قدر));

    /* Process the known hierarchical parameters (UIDVALIDITY, UID, SECTION
       and PARTIAL) stripping of the trailing slash character if it is
       present.

       Note: Unknown parameters trigger a URL_MALFORMAT error. */
    إذا(valuelen > 0 && قدر[valuelen - 1] == '/')
      قدر[valuelen - 1] = '\0';
    إذا(valuelen) {
      إذا(curl_strequal(name, "UIDVALIDITY") && !imap->uidvalidity_سیٹ) {
        curl_off_t عدد;
        ثابت حرف *p = (ثابت حرف *)قدر;
        إذا(!curlx_str_نمبر(&p, &عدد, UINT_MAX)) {
          imap->uidvalidity = (بنانشانی صحيح)عدد;
          imap->uidvalidity_سیٹ = TRUE;
        }
        curlx_free(قدر);
      }
      وإلا إذا(curl_strequal(name, "UID") && !imap->uid) {
        imap->uid = قدر;
      }
      وإلا إذا(curl_strequal(name, "MAILINDEX") && !imap->mindex) {
        imap->mindex = قدر;
      }
      وإلا إذا(curl_strequal(name, "SECTION") && !imap->section) {
        imap->section = قدر;
      }
      وإلا إذا(curl_strequal(name, "PARTIAL") && !imap->partial) {
        imap->partial = قدر;
      }
      وإلا {
        curlx_free(name);
        curlx_free(قدر);
        أرجع CURLE_URL_MALFORMAT;
      }
    }
    وإلا
      /* blank? */
      curlx_free(قدر);
    curlx_free(name);
  }

  /* Does the URL contain a query parameter? Only valid when we have a mailbox
     and no UID as per RFC-5092 */
  إذا(imap->mailbox && !imap->uid && !imap->mindex) {
    /* Get the query parameter, URL decoded */
    CURLUcode uc = curl_url_get(انکڑے->state.uh, CURLUPART_QUERY, &imap->query,
                                CURLU_URLDECODE);
    إذا(uc == CURLUE_باہرلا_دا_حافظہ)
      أرجع CURLE_باہرلا_دا_حافظہ;
  }

  /* Any extra stuff at the end of the URL is an error */
  إذا(*ptr)
    أرجع CURLE_URL_MALFORMAT;

  أرجع CURLE_OK;
}

/***********************************************************************
 *
 * imap_parse_custom_request()
 *
 * Parse the custom request.
 */
ساكن CURLcode imap_parse_custom_request(بنية Curl_easy *انکڑے,
                                          بنية IMAP *imap)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *custom = CURL_EASY_STR(انکڑے, STRING_CUSTOMREQUEST);

  إذا(custom) {
    /* URL decode the custom request */
    نتیجہ = Curl_urldecode(custom, 0, &imap->custom, عدم, REJECT_CTRL);

    /* Extract the parameters if specified */
    إذا(!نتیجہ) {
      ثابت حرف *params = imap->custom;

      طالما(*params && *params != ' ')
        params++;

      إذا(*params) {
        imap->custom_params = curlx_strdup(params);
        imap->custom[params - imap->custom] = '\0';

        إذا(!imap->custom_params)
          نتیجہ = CURLE_باہرلا_دا_حافظہ;
      }
    }
  }

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * imap_connect()
 *
 * This function should do everything that is to be considered a part of the
 * connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 */
ساكن CURLcode imap_connect(بنية Curl_easy *انکڑے, bool *پورا)
{
  بنية imap_conn *imapc =
    Curl_conn_meta_get(انکڑے->conn, CURL_META_IMAP_CONN);
  CURLcode نتیجہ = CURLE_OK;

  *پورا = FALSE; /* default to not done yet */
  إذا(!imapc)
    أرجع CURLE_FAILED_INIT;

  /* Parse the URL options */
  نتیجہ = imap_parse_url_options(انکڑے->conn, imapc);
  إذا(نتیجہ)
    أرجع نتیجہ;

  /* Start off waiting for the server greeting response */
  imap_state(انکڑے, imapc, IMAP_SERVERGREET);

  /* Start off with an response id of '*' */
  curlx_strcopy(imapc->resptag, حجم(imapc->resptag), STRCONST("*"));

  نتیجہ = imap_multi_statemach(انکڑے, پورا);

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * imap_done()
 *
 * The DONE function. This does what needs to be done after a single DO has
 * performed.
 *
 * Input argument is already checked for validity.
 */
ساكن CURLcode imap_پورا(بنية Curl_easy *انکڑے, CURLcode حالت,
                          bool premature)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية connectdata *conn = انکڑے->conn;
  بنية imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  بنية IMAP *imap = Curl_meta_get(انکڑے, CURL_META_IMAP_EASY);

  إذا(!imapc)
    أرجع CURLE_FAILED_INIT;
  إذا(!imap)
    أرجع CURLE_OK;

  إذا(حالت || premature) {
    CURL_TRC_M(انکڑے, "IMAP done, not finalizing transfer");
    connclose(conn); /* marked for closure */
    نتیجہ = حالت;         /* use the already set error code */
  }
  وإلا إذا(!انکڑے->سیٹ.connect_only &&
          ((!imap->custom && (imap->uid || imap->mindex)) ||
           (imap->custom && انکڑے->req.maxdownload > 0) ||
           انکڑے->state.upload || IS_MIME_POST(انکڑے))) {
    /* Handle responses after FETCH or APPEND transfer has finished.
       For custom commands, check if we set up a download which indicates
       a FETCH-like command with literal data. */

    إذا(!انکڑے->state.upload && !IS_MIME_POST(انکڑے))
      imap_state(انکڑے, imapc, IMAP_FETCH_FINAL);
    وإلا {
      /* End the APPEND command first by sending an empty line */
      نتیجہ = Curl_pp_sendf(انکڑے, &imapc->pp, "%s", "");
      إذا(!نتیجہ)
        imap_state(انکڑے, imapc, IMAP_APPEND_FINAL);
    }

    /* Run the state-machine */
    إذا(!نتیجہ)
      نتیجہ = imap_بلاک_statemach(انکڑے, imapc, FALSE);
  }

  imap_easy_reset(imap);
  أرجع نتیجہ;
}

/***********************************************************************
 *
 * imap_perform()
 *
 * This is the actual DO function for IMAP. Fetch or append a message, or do
 * other things according to the options previously setup.
 */
ساكن CURLcode imap_perform(بنية Curl_easy *انکڑے, bool *connected,
                             bool *dophase_پورا)
{
  /* This is IMAP and no proxy */
  CURLcode نتیجہ = CURLE_OK;
  بنية connectdata *conn = انکڑے->conn;
  بنية imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  بنية IMAP *imap = Curl_meta_get(انکڑے, CURL_META_IMAP_EASY);
  bool selected = FALSE;

  DEBUGF(infof(انکڑے, "DO phase starts"));
  إذا(!imapc || !imap)
    أرجع CURLE_FAILED_INIT;

  إذا(انکڑے->req.no_body) {
    /* Requested no body means no transfer */
    imap->transfer = PPTRANSFER_INFO;
  }

  *dophase_پورا = FALSE; /* not done yet */

  /* Determine if the requested mailbox (with the same UIDVALIDITY if set)
     has already been selected on this connection */
  إذا(imap->mailbox && imapc->mailbox &&
     (!strcmp(imap->mailbox, imapc->mailbox) ||
      (curl_strequal(imap->mailbox, "INBOX") &&
       curl_strequal(imapc->mailbox, "INBOX"))) &&
     (!imap->uidvalidity_سیٹ || !imapc->mb_uidvalidity_سیٹ ||
      (imap->uidvalidity == imapc->mb_uidvalidity)))
    selected = TRUE;

  /* Start the first command in the DO phase */
  إذا(انکڑے->state.upload || IS_MIME_POST(انکڑے))
    /* APPEND can be executed directly */
    نتیجہ = imap_perform_append(انکڑے, imapc, imap);
  وإلا إذا(imap->custom && (selected || !imap->mailbox))
    /* Custom command using the same mailbox or no mailbox */
    نتیجہ = imap_perform_list(انکڑے, imapc, imap);
  وإلا إذا(!imap->custom && selected && (imap->uid || imap->mindex))
    /* FETCH from the same mailbox */
    نتیجہ = imap_perform_fetch(انکڑے, imapc, imap);
  وإلا إذا(!imap->custom && selected && imap->query)
    /* SEARCH the current mailbox */
    نتیجہ = imap_perform_تلاش(انکڑے, imapc, imap);
  وإلا إذا(imap->mailbox && !selected &&
          (imap->custom || imap->uid || imap->mindex || imap->query))
    /* SELECT the mailbox */
    نتیجہ = imap_perform_select(انکڑے, imapc, imap);
  وإلا
    /* LIST */
    نتیجہ = imap_perform_list(انکڑے, imapc, imap);

  إذا(نتیجہ)
    أرجع نتیجہ;

  /* Run the state-machine */
  نتیجہ = imap_multi_statemach(انکڑے, dophase_پورا);

  *connected = Curl_conn_is_connected(conn, FIRSTSOCKET);

  إذا(*dophase_پورا)
    DEBUGF(infof(انکڑے, "DO phase is complete"));

  أرجع نتیجہ;
}

/* Call this when the DO phase has completed */
ساكن CURLcode imap_dophase_پورا(بنية Curl_easy *انکڑے,
                                  بنية IMAP *imap,
                                  bool connected)
{
  (فراغ)connected;

  إذا(imap->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_تیاری_nop(انکڑے);

  أرجع CURLE_OK;
}

/***********************************************************************
 *
 * imap_regular_transfer()
 *
 * The input argument is already checked for validity.
 *
 * Performs all commands done before a regular transfer between a local and a
 * remote host.
 */
ساكن CURLcode imap_regular_transfer(بنية Curl_easy *انکڑے,
                                      بنية IMAP *imap,
                                      bool *dophase_پورا)
{
  CURLcode نتیجہ = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  انکڑے->req.ناپ = -1;

  /* Set the progress data */
  Curl_pgrsReset(انکڑے);

  /* Carry out the perform */
  نتیجہ = imap_perform(انکڑے, &connected, dophase_پورا);

  /* Perform post DO phase operations if necessary */
  إذا(!نتیجہ && *dophase_پورا)
    نتیجہ = imap_dophase_پورا(انکڑے, imap, connected);

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * imap_do()
 *
 * This function is registered as 'curl_do' function. It decodes the path
 * parts etc as a wrapper to the actual DO function (imap_perform).
 *
 * The input argument is already checked for validity.
 */
ساكن CURLcode imap_do(بنية Curl_easy *انکڑے, bool *پورا)
{
  بنية IMAP *imap = Curl_meta_get(انکڑے, CURL_META_IMAP_EASY);
  CURLcode نتیجہ = CURLE_OK;
  *پورا = FALSE; /* default to false */

  إذا(!imap)
    أرجع CURLE_FAILED_INIT;
  /* Parse the URL path */
  نتیجہ = imap_parse_url_رستہ(انکڑے, imap);
  إذا(نتیجہ)
    أرجع نتیجہ;

  /* Parse the custom request */
  نتیجہ = imap_parse_custom_request(انکڑے, imap);
  إذا(نتیجہ)
    أرجع نتیجہ;

  نتیجہ = imap_regular_transfer(انکڑے, imap, پورا);

  أرجع نتیجہ;
}

/***********************************************************************
 *
 * imap_disconnect()
 *
 * Disconnect from an IMAP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ساكن CURLcode imap_disconnect(بنية Curl_easy *انکڑے,
                                بنية connectdata *conn, bool dead_connection)
{
  بنية imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);

  إذا(imapc &&
     /* We cannot send quit unconditionally. If this connection is stale or
        bad in any way (pingpong has pending data to send),
        sending quit and waiting around here will make the
        disconnect wait in vain and cause more problems than we need to. */
     !dead_connection && conn->بٹ.protoconnstart &&
     !Curl_pp_needs_flush(انکڑے, &imapc->pp) &&
     !imap_perform_logout(انکڑے, imapc))
    (فراغ)imap_بلاک_statemach(انکڑے, imapc, TRUE); /* ignore errors */

  أرجع CURLE_OK;
}

/* Called from multi.c while DOing */
ساكن CURLcode imap_doing(بنية Curl_easy *انکڑے, bool *dophase_پورا)
{
  بنية IMAP *imap = Curl_meta_get(انکڑے, CURL_META_IMAP_EASY);
  CURLcode نتیجہ;

  إذا(!imap)
    أرجع CURLE_FAILED_INIT;

  نتیجہ = imap_multi_statemach(انکڑے, dophase_پورا);
  إذا(نتیجہ)
    DEBUGF(infof(انکڑے, "DO phase failed"));
  وإلا إذا(*dophase_پورا) {
    نتیجہ = imap_dophase_پورا(انکڑے, imap, FALSE /* not connected */);

    DEBUGF(infof(انکڑے, "DO phase is complete"));
  }

  أرجع نتیجہ;
}

ساكن فراغ imap_easy_dtor(ثابت فراغ *کلید, size_t klen, فراغ *اندراج)
{
  بنية IMAP *imap = اندراج;
  (فراغ)کلید;
  (فراغ)klen;
  imap_easy_reset(imap);
  curlx_free(imap);
}

ساكن فراغ imap_conn_dtor(ثابت فراغ *کلید, size_t klen, فراغ *اندراج)
{
  بنية imap_conn *imapc = اندراج;
  (فراغ)کلید;
  (فراغ)klen;
  Curl_pp_disconnect(&imapc->pp);
  curlx_safefree(imapc->mailbox);
  curlx_free(imapc);
}

/* SASL parameters for the imap protocol */
ساكن ثابت بنية SASLproto saslimap = {
  "imap",                     /* The service name */
  imap_perform_authenticate,  /* Send authentication command */
  imap_continue_authenticate, /* Send authentication continuation */
  imap_cancel_authenticate,   /* Send authentication cancellation */
  imap_get_سنیہا,           /* Get SASL response message */
  0,                          /* No maximum initial response length */
  '+',                        /* Code received when continuation is expected */
  IMAP_RESP_OK,               /* Code to receive upon authentication success */
  SASL_AUTH_DEFAULT,          /* Default mechanisms */
  SASL_FLAG_BASE64            /* Configuration flags */
};

ساكن CURLcode imap_تیاری_connection(بنية Curl_easy *انکڑے,
                                      بنية connectdata *conn)
{
  بنية imap_conn *imapc;
  بنية pingpong *pp;
  بنية IMAP *imap;

  imapc = curlx_calloc(1, حجم(*imapc));
  إذا(!imapc)
    أرجع CURLE_باہرلا_دا_حافظہ;

  pp = &imapc->pp;
  PINGPONG_تیاری(pp, imap_pp_statemachine, imap_endofresp);

  /* Set the default preferred authentication type and mechanism */
  imapc->preftype = IMAP_TYPE_ANY;
  Curl_sasl_init(&imapc->sasl, انکڑے, &saslimap);

  Curl_pp_init(pp, Curl_pgrs_now(انکڑے));

  إذا(Curl_conn_meta_سیٹ(conn, CURL_META_IMAP_CONN, imapc, imap_conn_dtor))
    أرجع CURLE_باہرلا_دا_حافظہ;

  imap = curlx_calloc(1, حجم(بنية IMAP));
  إذا(!imap ||
     Curl_meta_سیٹ(انکڑے, CURL_META_IMAP_EASY, imap, imap_easy_dtor))
    أرجع CURLE_باہرلا_دا_حافظہ;

  أرجع CURLE_OK;
}

/*
 * IMAP protocol.
 */
ثابت بنية Curl_protocol Curl_protocol_imap = {
  imap_تیاری_connection,            /* setup_connection */
  imap_do,                          /* do_it */
  imap_پورا,                        /* done */
  ZERO_NULL,                        /* do_more */
  imap_connect,                     /* connect_it */
  imap_multi_statemach,             /* connecting */
  imap_doing,                       /* doing */
  imap_pollset,                     /* proto_pollset */
  imap_pollset,                     /* doing_pollset */
  ZERO_NULL,                        /* domore_pollset */
  ZERO_NULL,                        /* perform_pollset */
  imap_disconnect,                  /* disconnect */
  ZERO_NULL,                        /* write_resp */
  ZERO_NULL,                        /* write_resp_hd */
  ZERO_NULL,                        /* connection_is_dead */
  ZERO_NULL,                        /* attach connection */
  ZERO_NULL,                        /* follow */
};

#جے_انت /* CURL_DISABLE_IMAP */
