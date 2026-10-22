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

#اگر_تعریف_نشده CURL_DISABLE_IMAP

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده __VMS
#ضمن <in.h>
#ضمن <inet.h>
#پایان_اگر

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
#تعریف_کن CURL_META_IMAP_EASY   "meta:proto:imap:easy"
/* meta key for storing protocol meta at connection */
#تعریف_کن CURL_META_IMAP_CONN   "meta:proto:imap:conn"

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
  IMAP_جستجو,
  IMAP_LOGOUT,
  IMAP_آخرین          /* never used */
} imapstate;

/* imap_conn is used for struct connection-oriented data */
بنية imap_conn {
  بنية pingpong pp;
  بنية SASL sasl;           /* SASL-related parameters */
  حرف *mailbox;              /* The last selected mailbox */
  imapstate state;            /* Always use imap.c:state() to change state! */
  بیعلامت صحيح mb_uidvalidity; /* UIDVALIDITY parsed from select response */
  حرف resptag[5];            /* Response tag to wait for */
  بیعلامت حرف preftype;     /* Preferred authentication type */
  بیعلامت حرف cmdid;        /* Last used command ID */
  بیت(ssldone);               /* Is connect() over SSL done? */
  بیت(preauth);               /* Is this connection PREAUTH? */
  بیت(tls_supported);         /* StartTLS capability supported by server */
  بیت(login_disabled);        /* LOGIN command disabled by server */
  بیت(ir_supported);          /* Initial response supported by server */
  بیت(mb_uidvalidity_مجموعه);
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
  بیعلامت صحيح uidvalidity; /* UIDVALIDITY to check in select */
  بیت(uidvalidity_مجموعه);
};

#تعریف_کن IMAP_RESP_OK       1
#تعریف_کن IMAP_RESP_NOT_OK   2
#تعریف_کن IMAP_RESP_PREAUTH  3

بنية ulbits {
  صحيح بیت_2;
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
ساكن CURLcode imap_sendf(بنية Curl_easy *داده,
                           بنية imap_conn *imapc,
                           ثابت حرف *fmt, ...) CURL_PRINTF(3, 4);
ساكن CURLcode imap_sendf(بنية Curl_easy *داده,
                           بنية imap_conn *imapc,
                           ثابت حرف *fmt, ...)
{
  CURLcode نتیجه = CURLE_OK;
  va_list ap;

  DEBUGASSERT(fmt);

  /* Calculate the tag based on the connection ID and command ID */
  curl_msnprintf(imapc->resptag, حجم(imapc->resptag), "%c%03d",
                 'A' + curlx_sltosi((دراز)(داده->conn->connection_id % 26)),
                 ++imapc->cmdid);

  va_start(ap, fmt);
  نتیجه = Curl_pp_vsendf(داده, &imapc->pp, imapc->resptag, fmt, ap);
  va_end(ap);
  أرجع نتیجه;
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

  len = طول_رشته(str);
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
ساكن ثابت حرف *imap_پیدا_کن_literal(ثابت حرف *سطر, size_t len)
{
  ثابت حرف *انتها = سطر + len;
  bool درون_quote = FALSE;

  طالما(سطر < انتها) {
    إذا(درون_quote) {
      إذا(*سطر == '\\' && (سطر + 1) < انتها) {
        سطر += 2;
        استمر;
      }
      إذا(*سطر == '"')
        درون_quote = FALSE;
    }
    وإلا {
      إذا(*سطر == '"')
        درون_quote = TRUE;
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
  ثابت حرف *انتها = سطر + len;
  size_t cmd_len = طول_رشته(cmd);

  /* Skip the untagged response marker */
  سطر += 2;

  /* Do we have a number after the marker? */
  إذا(سطر < انتها && ISDIGIT(*سطر)) {
    /* Skip the number */
    افعل
      سطر++;
    طالما(سطر < انتها && ISDIGIT(*سطر));

    /* Do we have the space character? */
    إذا(سطر == انتها || *سطر != ' ')
      أرجع FALSE;

    سطر++;
  }

  /* Does the command name match and is it followed by a space character or at
     the end of line? */
  إذا(سطر + cmd_len <= انتها && curl_strnequal(سطر, cmd, cmd_len) &&
     (سطر[cmd_len] == ' ' || سطر + cmd_len + 2 == انتها))
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
ساكن bool imap_endofresp(بنية Curl_easy *داده, بنية connectdata *conn,
                           ثابت حرف *سطر, size_t len, صحيح *resp)
{
  بنية imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  بنية IMAP *imap = Curl_meta_get(داده, CURL_META_IMAP_EASY);
  ثابت حرف *id;
  size_t id_len;

  DEBUGASSERT(imapc);
  DEBUGASSERT(imap);
  إذا(!imapc || !imap)
    أرجع FALSE;

  /* Do we have a tagged command response? */
  id = imapc->resptag;
  id_len = طول_رشته(id);
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

    حالة IMAP_جستجو:
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
      failf(داده, "Unexpected continuation response");
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
ساكن CURLcode imap_get_پیام(بنية Curl_easy *داده, بنية bufref *بیرون)
{
  بنية imap_conn *imapc =
    Curl_conn_meta_get(داده->conn, CURL_META_IMAP_CONN);
  حرف *پیام_2;
  size_t len;

  إذا(!imapc)
    أرجع CURLE_FAILED_INIT;

  پیام_2 = curlx_dyn_ptr(&imapc->pp.recvbuf);
  len = imapc->pp.nfinal;
  إذا(len > 2) {
    /* Find the start of the message */
    len -= 2;
    لكل(پیام_2 += 2; ISBLANK(*پیام_2); پیام_2++, len--)
      ;

    /* Find the end of the message */
    طالما(len--)
      إذا(!ISNEWLINE(پیام_2[len]) && !ISBLANK(پیام_2[len]))
        اكسر;

    /* Terminate the message */
    پیام_2[++len] = '\0';
    Curl_bufref_مجموعه(بیرون, پیام_2, len, عدم);
  }
  وإلا
    /* junk input => zero length output */
    Curl_bufref_مجموعه(بیرون, "", 0, عدم);

  أرجع CURLE_OK;
}

/***********************************************************************
 *
 * imap_state()
 *
 * This is the ONLY way to change IMAP state!
 */
ساكن فراغ imap_state(بنية Curl_easy *داده,
                       بنية imap_conn *imapc,
                       imapstate newstate)
{
#إذا defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ساكن ثابت حرف * ثابت نامها[] = {
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
    /*آخرین*/
  };

  إذا(imapc->state != newstate)
    infof(داده, "IMAP %p state change from %s to %s",
          (فراغ *)imapc, نامها[imapc->state], نامها[newstate]);
#وإلا
  (فراغ)داده;
#پایان_اگر
  imapc->state = newstate;
}

/***********************************************************************
 *
 * imap_perform_capability()
 *
 * Sends the CAPABILITY command in order to obtain a list of server side
 * supported capabilities.
 */
ساكن CURLcode imap_perform_capability(بنية Curl_easy *داده,
                                        بنية imap_conn *imapc)
{
  CURLcode نتیجه = CURLE_OK;

  imapc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  imapc->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  imapc->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPABILITY command */
  نتیجه = imap_sendf(داده, imapc, "CAPABILITY");

  إذا(!نتیجه)
    imap_state(داده, imapc, IMAP_CAPABILITY);

  أرجع نتیجه;
}

/***********************************************************************
 *
 * imap_perform_starttls()
 *
 * Sends the STARTTLS command to start the upgrade to TLS.
 */
ساكن CURLcode imap_perform_starttls(بنية Curl_easy *داده,
                                      بنية imap_conn *imapc)
{
  /* Send the STARTTLS command */
  CURLcode نتیجه = imap_sendf(داده, imapc, "STARTTLS");

  إذا(!نتیجه)
    imap_state(داده, imapc, IMAP_STARTTLS);

  أرجع نتیجه;
}

/***********************************************************************
 *
 * imap_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ساكن CURLcode imap_perform_upgrade_tls(بنية Curl_easy *داده,
                                         بنية imap_conn *imapc,
                                         بنية connectdata *conn)
{
#اگر_تعریف_شده USE_SSL
  /* Start the SSL connection */
  CURLcode نتیجه;
  bool ssldone = FALSE;

  إذا(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    نتیجه = Curl_ssl_cfilter_جمع_کن(
      داده, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    إذا(نتیجه)
      اذهب بیرون;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_imaps;
  }

  DEBUGASSERT(!imapc->ssldone);
  نتیجه = Curl_conn_connect(داده, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(داده, "imap_perform_upgrade_tls, connect -> %d, %d",
               (صحيح)نتیجه, ssldone));
  إذا(!نتیجه && ssldone) {
    imapc->ssldone = ssldone;
    /* perform CAPA now, changes imapc->state out of IMAP_UPGRADETLS */
    نتیجه = imap_perform_capability(داده, imapc);
  }
بیرون:
  أرجع نتیجه;
#وإلا
  (فراغ)داده;
  (فراغ)imapc;
  (فراغ)conn;
  أرجع CURLE_NOT_BUILT_درون;
#پایان_اگر
}

/***********************************************************************
 *
 * imap_perform_login()
 *
 * Sends a clear text LOGIN command to authenticate with.
 */
ساكن CURLcode imap_perform_login(بنية Curl_easy *داده,
                                   بنية imap_conn *imapc,
                                   بنية connectdata *conn)
{
  CURLcode نتیجه = CURLE_OK;
  حرف *user;
  حرف *passwd;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  إذا(!conn->creds) {
    imap_state(داده, imapc, IMAP_STOP);

    أرجع نتیجه;
  }

  /* Make sure the username and password are in the correct atom format */
  user = imap_atom(Curl_creds_user(conn->creds), FALSE);
  passwd = imap_atom(Curl_creds_passwd(conn->creds), FALSE);

  /* Send the LOGIN command */
  نتیجه = imap_sendf(داده, imapc, "LOGIN %s %s", user ? user : "",
                      passwd ? passwd : "");

  curlx_free(user);
  curlx_strzero(passwd);
  curlx_free(passwd);

  إذا(!نتیجه)
    imap_state(داده, imapc, IMAP_LOGIN);

  أرجع نتیجه;
}

/***********************************************************************
 *
 * imap_perform_authenticate()
 *
 * Sends an AUTHENTICATE command allowing the client to login with the given
 * SASL authentication mechanism.
 */
ساكن CURLcode imap_perform_authenticate(بنية Curl_easy *داده,
                                          ثابت حرف *mech,
                                          ثابت بنية bufref *initresp)
{
  بنية imap_conn *imapc =
    Curl_conn_meta_get(داده->conn, CURL_META_IMAP_CONN);
  CURLcode نتیجه = CURLE_OK;
  ثابت حرف *ir = Curl_bufref_ptr(initresp);

  إذا(!imapc)
    أرجع CURLE_FAILED_INIT;
  إذا(ir) {
    /* Send the AUTHENTICATE command with the initial response */
    نتیجه = imap_sendf(داده, imapc, "AUTHENTICATE %s %s",
                        mech, *ir ? ir : "=");
  }
  وإلا {
    /* Send the AUTHENTICATE command */
    نتیجه = imap_sendf(داده, imapc, "AUTHENTICATE %s", mech);
  }

  أرجع نتیجه;
}

/***********************************************************************
 *
 * imap_continue_authenticate()
 *
 * Sends SASL continuation data.
 */
ساكن CURLcode imap_continue_authenticate(بنية Curl_easy *داده,
                                           ثابت حرف *mech,
                                           ثابت بنية bufref *resp)
{
  بنية imap_conn *imapc =
    Curl_conn_meta_get(داده->conn, CURL_META_IMAP_CONN);

  (فراغ)mech;
  إذا(!imapc)
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_pp_sendf(داده, &imapc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * imap_cancel_authenticate()
 *
 * Sends SASL cancellation.
 */
ساكن CURLcode imap_cancel_authenticate(بنية Curl_easy *داده,
                                         ثابت حرف *mech)
{
  بنية imap_conn *imapc =
    Curl_conn_meta_get(داده->conn, CURL_META_IMAP_CONN);

  (فراغ)mech;
  إذا(!imapc)
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_pp_sendf(داده, &imapc->pp, "*");
}

/***********************************************************************
 *
 * imap_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to clear text should a common
 * mechanism not be available between the client and server.
 */
ساكن CURLcode imap_perform_authentication(بنية Curl_easy *داده,
                                            بنية imap_conn *imapc)
{
  CURLcode نتیجه = CURLE_OK;
  saslprogress پیشرفت;

  /* Check if already authenticated OR if there is enough data to authenticate
     with and end the connect phase if we do not */
  إذا(imapc->preauth ||
     !Curl_sasl_can_authenticate(&imapc->sasl, داده)) {
    imap_state(داده, imapc, IMAP_STOP);
    أرجع نتیجه;
  }

  /* Calculate the SASL login details */
  نتیجه = Curl_sasl_شروع(&imapc->sasl, داده, (bool)imapc->ir_supported,
                           &پیشرفت);
  إذا(!نتیجه) {
    إذا(پیشرفت == SASL_INPROGRESS)
      imap_state(داده, imapc, IMAP_AUTHENTICATE);
    وإلا إذا(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
      /* Perform clear text authentication */
      نتیجه = imap_perform_login(داده, imapc, داده->conn);
    وإلا
      نتیجه = Curl_sasl_is_blocked(&imapc->sasl, داده);
  }

  أرجع نتیجه;
}

/***********************************************************************
 *
 * imap_perform_list()
 *
 * Sends a LIST command or an alternative custom request.
 */
ساكن CURLcode imap_perform_list(بنية Curl_easy *داده,
                                  بنية imap_conn *imapc,
                                  بنية IMAP *imap)
{
  CURLcode نتیجه = CURLE_OK;

  إذا(imap->custom)
    /* Send the custom request */
    نتیجه = imap_sendf(داده, imapc, "%s%s", imap->custom,
                        imap->custom_params ? imap->custom_params : "");
  وإلا {
    /* Make sure the mailbox is in the correct atom format if necessary */
    حرف *mailbox = imap->mailbox ? imap_atom(imap->mailbox, TRUE)
                                  : curlx_strdup("");
    إذا(!mailbox)
      أرجع CURLE_بیرون_از_حافظه;

    /* Send the LIST command */
    نتیجه = imap_sendf(داده, imapc, "LIST \"%s\" *", mailbox);

    curlx_free(mailbox);
  }

  إذا(!نتیجه)
    imap_state(داده, imapc, IMAP_LIST);

  أرجع نتیجه;
}

/***********************************************************************
 *
 * imap_perform_select()
 *
 * Sends a SELECT command to ask the server to change the selected mailbox.
 */
ساكن CURLcode imap_perform_select(بنية Curl_easy *داده,
                                    بنية imap_conn *imapc,
                                    بنية IMAP *imap)
{
  CURLcode نتیجه = CURLE_OK;
  حرف *mailbox;

  /* Invalidate old information as we are switching mailboxes */
  curlx_safefree(imapc->mailbox);
  imapc->mb_uidvalidity_مجموعه = FALSE;

  /* Check we have a mailbox */
  إذا(!imap->mailbox) {
    failf(داده, "Cannot SELECT without a mailbox.");
    أرجع CURLE_URL_MALFORMAT;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  إذا(!mailbox)
    أرجع CURLE_بیرون_از_حافظه;

  /* Send the SELECT command */
  نتیجه = imap_sendf(داده, imapc, "SELECT %s", mailbox);

  curlx_free(mailbox);

  إذا(!نتیجه)
    imap_state(داده, imapc, IMAP_SELECT);

  أرجع نتیجه;
}

/***********************************************************************
 *
 * imap_perform_fetch()
 *
 * Sends a FETCH command to initiate the download of a message.
 */
ساكن CURLcode imap_perform_fetch(بنية Curl_easy *داده,
                                   بنية imap_conn *imapc,
                                   بنية IMAP *imap)
{
  CURLcode نتیجه = CURLE_OK;
  /* Check we have a UID */
  إذا(imap->uid) {

    /* Send the FETCH command */
    إذا(imap->partial)
      نتیجه = imap_sendf(داده, imapc, "UID FETCH %s BODY[%s]<%s>",
                          imap->uid, imap->section ? imap->section : "",
                          imap->partial);
    وإلا
      نتیجه = imap_sendf(داده, imapc, "UID FETCH %s BODY[%s]",
                          imap->uid, imap->section ? imap->section : "");
  }
  وإلا إذا(imap->mindex) {
    /* Send the FETCH command */
    إذا(imap->partial)
      نتیجه = imap_sendf(داده, imapc, "FETCH %s BODY[%s]<%s>",
                          imap->mindex, imap->section ? imap->section : "",
                          imap->partial);
    وإلا
      نتیجه = imap_sendf(داده, imapc, "FETCH %s BODY[%s]",
                          imap->mindex, imap->section ? imap->section : "");
  }
  وإلا {
    failf(داده, "Cannot FETCH without a UID.");
    أرجع CURLE_URL_MALFORMAT;
  }
  إذا(!نتیجه)
    imap_state(داده, imapc, IMAP_FETCH);

  أرجع نتیجه;
}

/***********************************************************************
 *
 * imap_perform_append()
 *
 * Sends an APPEND command to initiate the upload of a message.
 */
ساكن CURLcode imap_perform_append(بنية Curl_easy *داده,
                                    بنية imap_conn *imapc,
                                    بنية IMAP *imap)
{
  CURLcode نتیجه = CURLE_OK;
  حرف *mailbox;
  بنية dynbuf flags;

  /* Check we have a mailbox */
  إذا(!imap->mailbox) {
    failf(داده, "Cannot APPEND without a mailbox.");
    أرجع CURLE_URL_MALFORMAT;
  }

#اگر_تعریف_نشده CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  إذا(IS_MIME_POST(داده)) {
    curl_mimepart *postp = داده->مجموعه.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(بیعلامت صحيح)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_سرآیندها(postp, داده->مجموعه.سرآیندها, 0);
    نتیجه = Curl_mime_prepare_سرآیندها(داده, postp, عدم,
                                       عدم, MIMESTRATEGY_MAIL);

    إذا(!نتیجه && !Curl_checkheaders(داده, STRCONST("Mime-Version")))
      نتیجه = Curl_mime_جمع_کن_سرآیند(&postp->curlheaders, "Mime-Version: 1.0");
    إذا(!نتیجه)
      نتیجه = Curl_creader_مجموعه_mime(داده, postp);
    إذا(نتیجه)
      أرجع نتیجه;
  }
  وإلا
#پایان_اگر
  {
    نتیجه = Curl_creader_مجموعه_fread(داده, داده->state.infilesize);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  /* Check we know the size of the upload. This takes all readers
   * into account. Especially crlf conversions which make the size
   * unpredictable, e.g. -1. */
  داده->state.infilesize = Curl_creader_مجموع_درازی(داده);
  إذا(داده->state.infilesize < 0) {
    إذا(داده->مجموعه.crlf)
      failf(داده, "Cannot APPEND with CRLF conversion making size unknown");
    وإلا
      failf(داده, "Cannot APPEND with unknown input file size");
    أرجع CURLE_UPLOAD_FAILED;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  إذا(!mailbox)
    أرجع CURLE_بیرون_از_حافظه;

  /* Generate flags string and send the APPEND command */
  curlx_dyn_init(&flags, 100);
  إذا(داده->مجموعه.upload_flags) {
    صحيح i;
    بنية ulbits ulflag[] = {
      { CURLULFLAG_ANSWERED, "Answered" },
      { CURLULFLAG_DELETED, "Deleted" },
      { CURLULFLAG_DRAFT, "Draft" },
      { CURLULFLAG_FLAGGED, "Flagged" },
      { CURLULFLAG_دیده, "Seen" },
      { 0, عدم }
    };

    نتیجه = CURLE_بیرون_از_حافظه;
    إذا(curlx_dyn_جمع_کن(&flags, " (")) {
      اذهب cleanup;
    }

    لكل(i = 0; ulflag[i].بیت_2; i++) {
      إذا(داده->مجموعه.upload_flags & ulflag[i].بیت_2 &&
         ((curlx_dyn_len(&flags) > 2 && curlx_dyn_جمع_کن(&flags, " ")) ||
          curlx_dyn_جمع_کن(&flags, "\\") ||
          curlx_dyn_جمع_کن(&flags, ulflag[i].flag)))
        اذهب cleanup;
    }

    إذا(curlx_dyn_جمع_کن(&flags, ")"))
      اذهب cleanup;
  }
  وإلا إذا(curlx_dyn_جمع_کن(&flags, ""))
    اذهب cleanup;

  نتیجه = imap_sendf(داده, imapc, "APPEND %s%s {%" FMT_OFF_T "}",
                      mailbox, curlx_dyn_ptr(&flags), داده->state.infilesize);

cleanup:
  curlx_dyn_free(&flags);
  curlx_free(mailbox);

  إذا(!نتیجه)
    imap_state(داده, imapc, IMAP_APPEND);

  أرجع نتیجه;
}

/***********************************************************************
 *
 * imap_perform_search()
 *
 * Sends a SEARCH command.
 */
ساكن CURLcode imap_perform_جستجو(بنية Curl_easy *داده,
                                    بنية imap_conn *imapc,
                                    بنية IMAP *imap)
{
  CURLcode نتیجه = CURLE_OK;

  /* Check we have a query string */
  إذا(!imap->query) {
    failf(داده, "Cannot SEARCH without a query string.");
    أرجع CURLE_URL_MALFORMAT;
  }

  /* Send the SEARCH command */
  نتیجه = imap_sendf(داده, imapc, "SEARCH %s", imap->query);

  إذا(!نتیجه)
    imap_state(داده, imapc, IMAP_جستجو);

  أرجع نتیجه;
}

/***********************************************************************
 *
 * imap_perform_logout()
 *
 * Performs the logout action prior to sclose() being called.
 */
ساكن CURLcode imap_perform_logout(بنية Curl_easy *داده,
                                    بنية imap_conn *imapc)
{
  /* Send the LOGOUT command */
  CURLcode نتیجه = imap_sendf(داده, imapc, "LOGOUT");

  إذا(!نتیجه)
    imap_state(داده, imapc, IMAP_LOGOUT);

  أرجع نتیجه;
}

/* For the initial server greeting */
ساكن CURLcode imap_state_servergreet_resp(بنية Curl_easy *داده,
                                            بنية imap_conn *imapc,
                                            صحيح imapcode,
                                            imapstate instate)
{
  (فراغ)instate;

  إذا(imapcode == IMAP_RESP_PREAUTH) {
    /* PREAUTH */
    imapc->preauth = TRUE;
    infof(داده, "PREAUTH connection, already authenticated");
  }
  وإلا إذا(imapcode != IMAP_RESP_OK) {
    failf(داده, "Got unexpected imap-server response");
    أرجع CURLE_WEIRD_SERVER_پاسخ;
  }

  أرجع imap_perform_capability(داده, imapc);
}

/* For CAPABILITY responses */
ساكن CURLcode imap_state_capability_resp(بنية Curl_easy *داده,
                                           بنية imap_conn *imapc,
                                           صحيح imapcode,
                                           imapstate instate)
{
  CURLcode نتیجه = CURLE_OK;
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
        بیعلامت کوتاه mechbit;

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
  وإلا إذا(داده->مجموعه.use_ssl && !Curl_conn_is_ssl(داده->conn, FIRSTSOCKET)) {
    /* PREAUTH is not compatible with STARTTLS. */
    إذا(imapcode == IMAP_RESP_OK && imapc->tls_supported && !imapc->preauth) {
      /* Switch to TLS connection now */
      نتیجه = imap_perform_starttls(داده, imapc);
    }
    وإلا إذا(داده->مجموعه.use_ssl <= CURLUSESSL_TRY)
      نتیجه = imap_perform_authentication(داده, imapc);
    وإلا {
      failf(داده, "STARTTLS not available.");
      نتیجه = CURLE_USE_SSL_FAILED;
    }
  }
  وإلا
    نتیجه = imap_perform_authentication(داده, imapc);

  أرجع نتیجه;
}

/* For STARTTLS responses */
ساكن CURLcode imap_state_starttls_resp(بنية Curl_easy *داده,
                                         بنية imap_conn *imapc,
                                         صحيح imapcode,
                                         imapstate instate)
{
  CURLcode نتیجه = CURLE_OK;

  (فراغ)instate;

  /* Pipelining in response is forbidden. */
  إذا(imapc->pp.سرریز)
    أرجع CURLE_WEIRD_SERVER_پاسخ;

  إذا(imapcode != IMAP_RESP_OK) {
    إذا(داده->مجموعه.use_ssl != CURLUSESSL_TRY) {
      failf(داده, "STARTTLS denied");
      نتیجه = CURLE_USE_SSL_FAILED;
    }
    وإلا
      نتیجه = imap_perform_authentication(داده, imapc);
  }
  وإلا
    imap_state(داده, imapc, IMAP_UPGRADETLS);

  أرجع نتیجه;
}

/* For SASL authentication responses */
ساكن CURLcode imap_state_auth_resp(بنية Curl_easy *داده,
                                     بنية imap_conn *imapc,
                                     صحيح imapcode,
                                     imapstate instate)
{
  CURLcode نتیجه = CURLE_OK;
  saslprogress پیشرفت;

  (فراغ)instate;

  نتیجه = Curl_sasl_continue(&imapc->sasl, داده, imapcode, &پیشرفت);
  إذا(!نتیجه)
    بدل(پیشرفت) {
    حالة SASL_انجامشده:
      imap_state(داده, imapc, IMAP_STOP);  /* Authenticated */
      اكسر;
    حالة SASL_IDLE:            /* No mechanism left after cancellation */
      إذا(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
        /* Perform clear text authentication */
        نتیجه = imap_perform_login(داده, imapc, داده->conn);
      وإلا {
        failf(داده, "Authentication cancelled");
        نتیجه = CURLE_LOGIN_DENIED;
      }
      اكسر;
    افتراضي:
      اكسر;
    }

  أرجع نتیجه;
}

/* For LOGIN responses */
ساكن CURLcode imap_state_login_resp(بنية Curl_easy *داده,
                                      بنية imap_conn *imapc,
                                      صحيح imapcode,
                                      imapstate instate)
{
  CURLcode نتیجه = CURLE_OK;
  (فراغ)instate;

  إذا(imapcode != IMAP_RESP_OK) {
    failf(داده, "Access denied. %c", imapcode);
    نتیجه = CURLE_LOGIN_DENIED;
  }
  وإلا
    /* End of connect phase */
    imap_state(داده, imapc, IMAP_STOP);

  أرجع نتیجه;
}

/* Detect IMAP listings vs. downloading a single email */
ساكن bool is_custom_fetch_listing_تطابق(ثابت حرف *params)
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
    أرجع is_custom_fetch_listing_تطابق(p);
  }
  وإلا إذا(curl_strequal(imap->custom, "UID") && imap->custom_params &&
          curl_strnequal(imap->custom_params, " FETCH ", 7)) {
    ثابت حرف *p = imap->custom_params + 6;
    أرجع is_custom_fetch_listing_تطابق(p);
  }
  أرجع FALSE;
}

/* For LIST and SEARCH responses */
ساكن CURLcode imap_state_listsearch_resp(بنية Curl_easy *داده,
                                           بنية imap_conn *imapc,
                                           صحيح imapcode,
                                           imapstate instate)
{
  CURLcode نتیجه = CURLE_OK;
  ثابت حرف *سطر = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  بنية IMAP *imap = Curl_meta_get(داده, CURL_META_IMAP_EASY);

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
    ثابت حرف *ptr = imap_پیدا_کن_literal(سطر, سطر_len);
    إذا(ptr) {
      curl_off_t اندازه = 0;
      bool parsed = FALSE;
      ptr++;
      إذا(!curlx_str_عدد(&ptr, &اندازه, CURL_OFF_T_MAX) &&
         !curlx_str_single(&ptr, '}'))
        parsed = TRUE;

      إذا(parsed) {
        بنية pingpong *pp = &imapc->pp;
        size_t میانگیر_len = curlx_dyn_len(&pp->recvbuf);
        size_t after_سرآیند = میانگیر_len - pp->nfinal;

        /* This is a literal response, setup to receive the body data */
        infof(داده, "Found %" FMT_OFF_T " bytes to download", اندازه);

        /* First write the header line */
        نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, سطر, len);
        إذا(نتیجه)
          أرجع نتیجه;

        /* Handle data already in buffer after the header line */
        إذا(after_سرآیند > 0) {
          /* There is already data in the buffer that is part of the literal
             body or subsequent responses */
          size_t chunk = after_سرآیند;

          /* Keep only the data after the header line */
          curlx_dyn_دم(&pp->recvbuf, chunk);
          pp->nfinal = 0; /* done */

          /* Limit chunk to the literal size */
          إذا((curl_off_t)chunk > اندازه)
            chunk = (size_t)اندازه;

          إذا(chunk) {
            /* Write the literal body data */
            نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY,
                                       curlx_dyn_ptr(&pp->recvbuf), chunk);
            إذا(نتیجه)
              أرجع نتیجه;
          }

          /* Handle remaining data in buffer (either more literal data or
             subsequent responses) */
          إذا(after_سرآیند > chunk) {
            /* Keep the data after the literal body */
            pp->سرریز = after_سرآیند - chunk;
            curlx_dyn_دم(&pp->recvbuf, pp->سرریز);
          }
          وإلا {
            pp->سرریز = 0;
            curlx_dyn_reset(&pp->recvbuf);
          }
        }
        وإلا {
          /* No data in buffer yet, reset overflow */
          pp->سرریز = 0;
        }

        إذا((CURL_OFF_T_MAX - اندازه) < (curl_off_t)len)
          /* unlikely to actually be a transfer this big, but avoid integer
سرریز*/
          اندازه = CURL_OFF_T_MAX;
        وإلا
          اندازه += len;

        /* Progress size includes both header line and literal body */
        Curl_pgrs_مجموعه_Download_اندازه(داده, اندازه);

        إذا(داده->req.bytecount == اندازه)
          /* All data already transferred (header + literal body) */
          Curl_xfer_راهاندازی_nop(داده);
        وإلا {
          /* Setup to receive the literal body data.
             maxdownload and transfer size include both header line and
             literal body */
          داده->req.maxdownload = اندازه;
          Curl_xfer_راهاندازی_recv(داده, FIRSTSOCKET, اندازه);
        }
        /* End of DO phase */
        imap_state(داده, imapc, IMAP_STOP);
      }
      وإلا {
        /* Failed to parse literal, write the line */
        نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, سطر, len);
      }
    }
    وإلا {
      /* No literal, write the line as-is */
      نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, سطر, len);
    }
  }
  وإلا إذا(imapcode != IMAP_RESP_OK)
    نتیجه = CURLE_QUOTE_ERROR;
  وإلا
    /* End of DO phase */
    imap_state(داده, imapc, IMAP_STOP);

  أرجع نتیجه;
}

/* For SELECT responses */
ساكن CURLcode imap_state_select_resp(بنية Curl_easy *داده,
                                       بنية imap_conn *imapc,
                                       بنية IMAP *imap,
                                       صحيح imapcode,
                                       imapstate instate)
{
  CURLcode نتیجه = CURLE_OK;
  (فراغ)instate;

  إذا(imapcode == '*') {
    /* See if this is an UIDVALIDITY response */
    ثابت حرف *سطر = curlx_dyn_ptr(&imapc->pp.recvbuf);
    size_t len = curlx_dyn_len(&imapc->pp.recvbuf);
    إذا((len >= 18) && checkprefix("OK [UIDVALIDITY ", &سطر[2])) {
      curl_off_t مقدار;
      ثابت حرف *p = &سطر[2] + CURL_CSTRLEN("OK [UIDVALIDITY ");
      إذا(!curlx_str_عدد(&p, &مقدار, UINT_MAX)) {
        imapc->mb_uidvalidity = (بیعلامت صحيح)مقدار;
        imapc->mb_uidvalidity_مجموعه = TRUE;
      }
    }
  }
  وإلا إذا(imapcode == IMAP_RESP_OK) {
    /* Check if the UIDVALIDITY has been specified and matches */
    إذا(imap->uidvalidity_مجموعه && imapc->mb_uidvalidity_مجموعه &&
       (imap->uidvalidity != imapc->mb_uidvalidity)) {
      failf(داده, "Mailbox UIDVALIDITY has changed");
      نتیجه = CURLE_REMOTE_FILE_NOT_یافت;
    }
    وإلا {
      /* Note the currently opened mailbox on this connection */
      DEBUGASSERT(!imapc->mailbox);
      imapc->mailbox = curlx_strdup(imap->mailbox);
      إذا(!imapc->mailbox)
        أرجع CURLE_بیرون_از_حافظه;

      إذا(imap->custom)
        نتیجه = imap_perform_list(داده, imapc, imap);
      وإلا إذا(imap->query)
        نتیجه = imap_perform_جستجو(داده, imapc, imap);
      وإلا
        نتیجه = imap_perform_fetch(داده, imapc, imap);
    }
  }
  وإلا {
    failf(داده, "Select failed");
    نتیجه = CURLE_LOGIN_DENIED;
  }

  أرجع نتیجه;
}

/* For the (first line of the) FETCH responses */
ساكن CURLcode imap_state_fetch_resp(بنية Curl_easy *داده,
                                      بنية imap_conn *imapc,
                                      صحيح imapcode,
                                      imapstate instate)
{
  CURLcode نتیجه = CURLE_OK;
  بنية pingpong *pp = &imapc->pp;
  ثابت حرف *ptr = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  bool parsed = FALSE;
  curl_off_t اندازه = 0;

  (فراغ)instate;

  إذا(imapcode != '*') {
    Curl_pgrs_مجموعه_Download_اندازه(داده, -1);
    imap_state(داده, imapc, IMAP_STOP);
    أرجع CURLE_REMOTE_FILE_NOT_یافت;
  }

  /* Something like this is received "* 1 FETCH (BODY[TEXT] {2021}\r" so parse
     the continuation data contained within the curly brackets */
  ptr = imap_پیدا_کن_literal(ptr, len);
  إذا(ptr) {
    ptr++;
    إذا(!curlx_str_عدد(&ptr, &اندازه, CURL_OFF_T_MAX) &&
       !curlx_str_single(&ptr, '}'))
      parsed = TRUE;
  }

  إذا(parsed) {
    infof(داده, "Found %" FMT_OFF_T " bytes to download", اندازه);
    Curl_pgrs_مجموعه_Download_اندازه(داده, اندازه);

    إذا(pp->سرریز) {
      /* At this point there is a data in the receive buffer that is body
         content, send it as body and then skip it. Do note that there may
         even be additional "headers" after the body. */
      size_t chunk = pp->سرریز;

      /* keep only the overflow */
      curlx_dyn_دم(&pp->recvbuf, chunk);
      pp->nfinal = 0; /* done */

      إذا((curl_off_t)chunk > اندازه)
        /* The conversion from curl_off_t to size_t is always fine here */
        chunk = (size_t)اندازه;

      إذا(!chunk) {
        /* no size, we are done with the data */
        imap_state(داده, imapc, IMAP_STOP);
        أرجع CURLE_OK;
      }
      نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY,
                                 curlx_dyn_ptr(&pp->recvbuf), chunk);
      إذا(نتیجه)
        أرجع نتیجه;

      infof(داده, "Written %zu bytes, %" FMT_OFF_T
            " bytes are left for transfer", chunk, (curl_off_t)(اندازه - chunk));

      /* Have we used the entire overflow or part of it?*/
      إذا(pp->سرریز > chunk) {
        /* remember the remaining trailing overflow data */
        pp->سرریز -= chunk;
        curlx_dyn_دم(&pp->recvbuf, pp->سرریز);
      }
      وإلا {
        pp->سرریز = 0; /* handled */
        /* Free the cache */
        curlx_dyn_reset(&pp->recvbuf);
      }
    }

    إذا(داده->req.bytecount == اندازه)
      /* The entire data is already transferred! */
      Curl_xfer_راهاندازی_nop(داده);
    وإلا {
      /* IMAP download */
      داده->req.maxdownload = اندازه;
      Curl_xfer_راهاندازی_recv(داده, FIRSTSOCKET, اندازه);
    }
  }
  وإلا {
    /* We do not know how to parse this line */
    failf(داده, "Failed to parse FETCH response.");
    نتیجه = CURLE_WEIRD_SERVER_پاسخ;
  }

  /* End of DO phase */
  imap_state(داده, imapc, IMAP_STOP);

  أرجع نتیجه;
}

/* For final FETCH responses performed after the download */
ساكن CURLcode imap_state_fetch_final_resp(بنية Curl_easy *داده,
                                            بنية imap_conn *imapc,
                                            صحيح imapcode,
                                            imapstate instate)
{
  CURLcode نتیجه = CURLE_OK;

  (فراغ)instate;

  إذا(imapcode != IMAP_RESP_OK)
    نتیجه = CURLE_WEIRD_SERVER_پاسخ;
  وإلا
    /* End of DONE phase */
    imap_state(داده, imapc, IMAP_STOP);

  أرجع نتیجه;
}

/* For APPEND responses */
ساكن CURLcode imap_state_append_resp(بنية Curl_easy *داده,
                                       بنية imap_conn *imapc,
                                       صحيح imapcode,
                                       imapstate instate)
{
  CURLcode نتیجه = CURLE_OK;
  (فراغ)instate;

  إذا(imapcode != '+') {
    نتیجه = CURLE_UPLOAD_FAILED;
  }
  وإلا {
    /* Set the progress upload size */
    Curl_pgrs_مجموعه_Upload_اندازه(داده, داده->state.infilesize);

    /* IMAP upload */
    Curl_xfer_راهاندازی_send(داده, FIRSTSOCKET);

    /* End of DO phase */
    imap_state(داده, imapc, IMAP_STOP);
  }

  أرجع نتیجه;
}

/* For final APPEND responses performed after the upload */
ساكن CURLcode imap_state_append_final_resp(بنية Curl_easy *داده,
                                             بنية imap_conn *imapc,
                                             صحيح imapcode,
                                             imapstate instate)
{
  CURLcode نتیجه = CURLE_OK;

  (فراغ)instate;

  إذا(imapcode != IMAP_RESP_OK)
    نتیجه = CURLE_UPLOAD_FAILED;
  وإلا
    /* End of DONE phase */
    imap_state(داده, imapc, IMAP_STOP);

  أرجع نتیجه;
}

ساكن CURLcode imap_pp_statemachine(بنية Curl_easy *داده,
                                     بنية connectdata *conn)
{
  CURLcode نتیجه = CURLE_OK;
  صحيح imapcode;
  بنية imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  بنية IMAP *imap = Curl_meta_get(داده, CURL_META_IMAP_EASY);
  بنية pingpong *pp;
  size_t nread = 0;

  إذا(!imapc || !imap)
    أرجع CURLE_FAILED_INIT;
  pp = &imapc->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not IMAP */
upgrade_tls:
  إذا(imapc->state == IMAP_UPGRADETLS) {
    نتیجه = imap_perform_upgrade_tls(داده, imapc, conn);
    إذا(نتیجه || (imapc->state == IMAP_UPGRADETLS))
      أرجع نتیجه;
  }

  /* Flush any data that needs to be sent */
  إذا(pp->sendleft)
    أرجع Curl_pp_flushsend(داده, pp);

  افعل {
    /* Read the response from the server */
    نتیجه = Curl_pp_readresp(داده, FIRSTSOCKET, pp, &imapcode, &nread);
    إذا(نتیجه)
      أرجع نتیجه;

    /* Was there an error parsing the response line? */
    إذا(imapcode == -1)
      أرجع CURLE_WEIRD_SERVER_پاسخ;

    إذا(!imapcode)
      اكسر;

    /* We have now received a full IMAP server response */
    بدل(imapc->state) {
    حالة IMAP_SERVERGREET:
      نتیجه = imap_state_servergreet_resp(داده, imapc,
                                           imapcode, imapc->state);
      اكسر;

    حالة IMAP_CAPABILITY:
      نتیجه = imap_state_capability_resp(داده, imapc, imapcode, imapc->state);
      اكسر;

    حالة IMAP_STARTTLS:
      نتیجه = imap_state_starttls_resp(داده, imapc, imapcode, imapc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      إذا(!نتیجه && (imapc->state == IMAP_UPGRADETLS))
        اذهب upgrade_tls;
      اكسر;

    حالة IMAP_AUTHENTICATE:
      نتیجه = imap_state_auth_resp(داده, imapc, imapcode, imapc->state);
      اكسر;

    حالة IMAP_LOGIN:
      نتیجه = imap_state_login_resp(داده, imapc, imapcode, imapc->state);
      اكسر;

    حالة IMAP_LIST:
    حالة IMAP_جستجو:
      نتیجه = imap_state_listsearch_resp(داده, imapc, imapcode, imapc->state);
      اكسر;

    حالة IMAP_SELECT:
      نتیجه = imap_state_select_resp(داده, imapc, imap,
                                      imapcode, imapc->state);
      اكسر;

    حالة IMAP_FETCH:
      نتیجه = imap_state_fetch_resp(داده, imapc, imapcode, imapc->state);
      اكسر;

    حالة IMAP_FETCH_FINAL:
      نتیجه = imap_state_fetch_final_resp(داده, imapc,
                                           imapcode, imapc->state);
      اكسر;

    حالة IMAP_APPEND:
      نتیجه = imap_state_append_resp(داده, imapc, imapcode, imapc->state);
      اكسر;

    حالة IMAP_APPEND_FINAL:
      نتیجه = imap_state_append_final_resp(داده, imapc,
                                            imapcode, imapc->state);
      اكسر;

    حالة IMAP_LOGOUT:
    افتراضي:
      /* internal error */
      imap_state(داده, imapc, IMAP_STOP);
      اكسر;
    }
  } طالما(!نتیجه && imapc->state != IMAP_STOP && Curl_pp_moredata(pp));

  أرجع نتیجه;
}

/* Called repeatedly until done from multi.c */
ساكن CURLcode imap_multi_statemach(بنية Curl_easy *داده, bool *انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  بنية imap_conn *imapc =
    Curl_conn_meta_get(داده->conn, CURL_META_IMAP_CONN);

  *انجامشده = FALSE;
  إذا(!imapc)
    أرجع CURLE_FAILED_INIT;
  نتیجه = Curl_pp_statemach(داده, &imapc->pp, FALSE, FALSE);
  *انجامشده = (imapc->state == IMAP_STOP);

  أرجع نتیجه;
}

ساكن CURLcode imap_بلوک_statemach(بنية Curl_easy *داده,
                                     بنية imap_conn *imapc,
                                     bool disconnecting)
{
  CURLcode نتیجه = CURLE_OK;

  طالما(imapc->state != IMAP_STOP && !نتیجه)
    نتیجه = Curl_pp_statemach(داده, &imapc->pp, TRUE, disconnecting);

  أرجع نتیجه;
}

/* For the IMAP "protocol connect" and "doing" phases only */
ساكن CURLcode imap_pollset(بنية Curl_easy *داده,
                             بنية easy_pollset *ps)
{
  بنية imap_conn *imapc =
    Curl_conn_meta_get(داده->conn, CURL_META_IMAP_CONN);
  أرجع imapc ? Curl_pp_pollset(داده, &imapc->pp, ps) : CURLE_OK;
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
  imap->uidvalidity_مجموعه = FALSE;
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
  CURLcode نتیجه = CURLE_OK;
  ثابت حرف *ptr = conn->options;
  bool prefer_login = FALSE;

  طالما(!نتیجه && ptr && *ptr) {
    ثابت حرف *کلید = ptr;
    ثابت حرف *مقدار;

    طالما(*ptr && *ptr != '=')
      ptr++;

    مقدار = ptr + 1;

    طالما(*ptr && *ptr != ';')
      ptr++;

    إذا(curl_strnequal(کلید, "AUTH=+LOGIN", 11)) {
      /* User prefers plaintext LOGIN over any SASL, including SASL LOGIN */
      prefer_login = TRUE;
      imapc->sasl.prefmech = SASL_AUTH_NONE;
    }
    وإلا إذا(curl_strnequal(کلید, "AUTH=", 5)) {
      prefer_login = FALSE;
      نتیجه = Curl_sasl_parse_url_auth_option(&imapc->sasl,
                                               مقدار, ptr - مقدار);
    }
    وإلا {
      prefer_login = FALSE;
      نتیجه = CURLE_URL_MALFORMAT;
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

  أرجع نتیجه;
}

/***********************************************************************
 *
 * imap_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
ساكن CURLcode imap_parse_url_مسیر(بنية Curl_easy *داده,
                                    بنية IMAP *imap)
{
  /* The imap struct is already initialized in imap_connect() */
  CURLcode نتیجه = CURLE_OK;
  ثابت حرف *begin = &داده->state.به_بالا.مسیر[1]; /* skip leading slash */
  ثابت حرف *ptr = begin;

  /* See how much of the URL is a valid path and decode it */
  طالما(imap_is_bchar(*ptr))
    ptr++;

  إذا(ptr != begin) {
    /* Remove the trailing slash if present */
    ثابت حرف *انتها = ptr;
    إذا(انتها > begin && انتها[-1] == '/')
      انتها--;

    نتیجه = Curl_urldecode(begin, انتها - begin, &imap->mailbox, عدم,
                            REJECT_CTRL);
    إذا(نتیجه)
      أرجع نتیجه;
  }
  وإلا
    imap->mailbox = عدم;

  /* There can be any number of parameters in the form ";NAME=VALUE" */
  طالما(*ptr == ';') {
    حرف *name;
    حرف *مقدار;
    size_t valuelen;

    /* Find the length of the name parameter */
    begin = ++ptr;
    طالما(*ptr && *ptr != '=')
      ptr++;

    إذا(!*ptr)
      أرجع CURLE_URL_MALFORMAT;

    /* Decode the name parameter */
    نتیجه = Curl_urldecode(begin, ptr - begin, &name, عدم,
                            REJECT_CTRL);
    إذا(نتیجه)
      أرجع نتیجه;

    /* Find the length of the value parameter */
    begin = ++ptr;
    طالما(imap_is_bchar(*ptr))
      ptr++;

    /* Decode the value parameter */
    نتیجه = Curl_urldecode(begin, ptr - begin, &مقدار, &valuelen,
                            REJECT_CTRL);
    إذا(نتیجه) {
      curlx_free(name);
      أرجع نتیجه;
    }

    DEBUGF(infof(داده, "IMAP URL parameter '%s' = '%s'", name, مقدار));

    /* Process the known hierarchical parameters (UIDVALIDITY, UID, SECTION
       and PARTIAL) stripping of the trailing slash character if it is
       present.

       Note: Unknown parameters trigger a URL_MALFORMAT error. */
    إذا(valuelen > 0 && مقدار[valuelen - 1] == '/')
      مقدار[valuelen - 1] = '\0';
    إذا(valuelen) {
      إذا(curl_strequal(name, "UIDVALIDITY") && !imap->uidvalidity_مجموعه) {
        curl_off_t رقم;
        ثابت حرف *p = (ثابت حرف *)مقدار;
        إذا(!curlx_str_عدد(&p, &رقم, UINT_MAX)) {
          imap->uidvalidity = (بیعلامت صحيح)رقم;
          imap->uidvalidity_مجموعه = TRUE;
        }
        curlx_free(مقدار);
      }
      وإلا إذا(curl_strequal(name, "UID") && !imap->uid) {
        imap->uid = مقدار;
      }
      وإلا إذا(curl_strequal(name, "MAILINDEX") && !imap->mindex) {
        imap->mindex = مقدار;
      }
      وإلا إذا(curl_strequal(name, "SECTION") && !imap->section) {
        imap->section = مقدار;
      }
      وإلا إذا(curl_strequal(name, "PARTIAL") && !imap->partial) {
        imap->partial = مقدار;
      }
      وإلا {
        curlx_free(name);
        curlx_free(مقدار);
        أرجع CURLE_URL_MALFORMAT;
      }
    }
    وإلا
      /* blank? */
      curlx_free(مقدار);
    curlx_free(name);
  }

  /* Does the URL contain a query parameter? Only valid when we have a mailbox
     and no UID as per RFC-5092 */
  إذا(imap->mailbox && !imap->uid && !imap->mindex) {
    /* Get the query parameter, URL decoded */
    CURLUcode uc = curl_url_get(داده->state.uh, CURLUPART_QUERY, &imap->query,
                                CURLU_URLDECODE);
    إذا(uc == CURLUE_بیرون_از_حافظه)
      أرجع CURLE_بیرون_از_حافظه;
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
ساكن CURLcode imap_parse_custom_request(بنية Curl_easy *داده,
                                          بنية IMAP *imap)
{
  CURLcode نتیجه = CURLE_OK;
  ثابت حرف *custom = CURL_EASY_STR(داده, STRING_CUSTOMREQUEST);

  إذا(custom) {
    /* URL decode the custom request */
    نتیجه = Curl_urldecode(custom, 0, &imap->custom, عدم, REJECT_CTRL);

    /* Extract the parameters if specified */
    إذا(!نتیجه) {
      ثابت حرف *params = imap->custom;

      طالما(*params && *params != ' ')
        params++;

      إذا(*params) {
        imap->custom_params = curlx_strdup(params);
        imap->custom[params - imap->custom] = '\0';

        إذا(!imap->custom_params)
          نتیجه = CURLE_بیرون_از_حافظه;
      }
    }
  }

  أرجع نتیجه;
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
ساكن CURLcode imap_connect(بنية Curl_easy *داده, bool *انجامشده)
{
  بنية imap_conn *imapc =
    Curl_conn_meta_get(داده->conn, CURL_META_IMAP_CONN);
  CURLcode نتیجه = CURLE_OK;

  *انجامشده = FALSE; /* default to not done yet */
  إذا(!imapc)
    أرجع CURLE_FAILED_INIT;

  /* Parse the URL options */
  نتیجه = imap_parse_url_options(داده->conn, imapc);
  إذا(نتیجه)
    أرجع نتیجه;

  /* Start off waiting for the server greeting response */
  imap_state(داده, imapc, IMAP_SERVERGREET);

  /* Start off with an response id of '*' */
  curlx_strcopy(imapc->resptag, حجم(imapc->resptag), STRCONST("*"));

  نتیجه = imap_multi_statemach(داده, انجامشده);

  أرجع نتیجه;
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
ساكن CURLcode imap_انجامشده(بنية Curl_easy *داده, CURLcode وضعیت,
                          bool premature)
{
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;
  بنية imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  بنية IMAP *imap = Curl_meta_get(داده, CURL_META_IMAP_EASY);

  إذا(!imapc)
    أرجع CURLE_FAILED_INIT;
  إذا(!imap)
    أرجع CURLE_OK;

  إذا(وضعیت || premature) {
    CURL_TRC_M(داده, "IMAP done, not finalizing transfer");
    connclose(conn); /* marked for closure */
    نتیجه = وضعیت;         /* use the already set error code */
  }
  وإلا إذا(!داده->مجموعه.connect_only &&
          ((!imap->custom && (imap->uid || imap->mindex)) ||
           (imap->custom && داده->req.maxdownload > 0) ||
           داده->state.upload || IS_MIME_POST(داده))) {
    /* Handle responses after FETCH or APPEND transfer has finished.
       For custom commands, check if we set up a download which indicates
       a FETCH-like command with literal data. */

    إذا(!داده->state.upload && !IS_MIME_POST(داده))
      imap_state(داده, imapc, IMAP_FETCH_FINAL);
    وإلا {
      /* End the APPEND command first by sending an empty line */
      نتیجه = Curl_pp_sendf(داده, &imapc->pp, "%s", "");
      إذا(!نتیجه)
        imap_state(داده, imapc, IMAP_APPEND_FINAL);
    }

    /* Run the state-machine */
    إذا(!نتیجه)
      نتیجه = imap_بلوک_statemach(داده, imapc, FALSE);
  }

  imap_easy_reset(imap);
  أرجع نتیجه;
}

/***********************************************************************
 *
 * imap_perform()
 *
 * This is the actual DO function for IMAP. Fetch or append a message, or do
 * other things according to the options previously setup.
 */
ساكن CURLcode imap_perform(بنية Curl_easy *داده, bool *connected,
                             bool *dophase_انجامشده)
{
  /* This is IMAP and no proxy */
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;
  بنية imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  بنية IMAP *imap = Curl_meta_get(داده, CURL_META_IMAP_EASY);
  bool selected = FALSE;

  DEBUGF(infof(داده, "DO phase starts"));
  إذا(!imapc || !imap)
    أرجع CURLE_FAILED_INIT;

  إذا(داده->req.no_body) {
    /* Requested no body means no transfer */
    imap->transfer = PPTRANSFER_INFO;
  }

  *dophase_انجامشده = FALSE; /* not done yet */

  /* Determine if the requested mailbox (with the same UIDVALIDITY if set)
     has already been selected on this connection */
  إذا(imap->mailbox && imapc->mailbox &&
     (!strcmp(imap->mailbox, imapc->mailbox) ||
      (curl_strequal(imap->mailbox, "INBOX") &&
       curl_strequal(imapc->mailbox, "INBOX"))) &&
     (!imap->uidvalidity_مجموعه || !imapc->mb_uidvalidity_مجموعه ||
      (imap->uidvalidity == imapc->mb_uidvalidity)))
    selected = TRUE;

  /* Start the first command in the DO phase */
  إذا(داده->state.upload || IS_MIME_POST(داده))
    /* APPEND can be executed directly */
    نتیجه = imap_perform_append(داده, imapc, imap);
  وإلا إذا(imap->custom && (selected || !imap->mailbox))
    /* Custom command using the same mailbox or no mailbox */
    نتیجه = imap_perform_list(داده, imapc, imap);
  وإلا إذا(!imap->custom && selected && (imap->uid || imap->mindex))
    /* FETCH from the same mailbox */
    نتیجه = imap_perform_fetch(داده, imapc, imap);
  وإلا إذا(!imap->custom && selected && imap->query)
    /* SEARCH the current mailbox */
    نتیجه = imap_perform_جستجو(داده, imapc, imap);
  وإلا إذا(imap->mailbox && !selected &&
          (imap->custom || imap->uid || imap->mindex || imap->query))
    /* SELECT the mailbox */
    نتیجه = imap_perform_select(داده, imapc, imap);
  وإلا
    /* LIST */
    نتیجه = imap_perform_list(داده, imapc, imap);

  إذا(نتیجه)
    أرجع نتیجه;

  /* Run the state-machine */
  نتیجه = imap_multi_statemach(داده, dophase_انجامشده);

  *connected = Curl_conn_is_connected(conn, FIRSTSOCKET);

  إذا(*dophase_انجامشده)
    DEBUGF(infof(داده, "DO phase is complete"));

  أرجع نتیجه;
}

/* Call this when the DO phase has completed */
ساكن CURLcode imap_dophase_انجامشده(بنية Curl_easy *داده,
                                  بنية IMAP *imap,
                                  bool connected)
{
  (فراغ)connected;

  إذا(imap->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_راهاندازی_nop(داده);

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
ساكن CURLcode imap_regular_transfer(بنية Curl_easy *داده,
                                      بنية IMAP *imap,
                                      bool *dophase_انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  داده->req.اندازه = -1;

  /* Set the progress data */
  Curl_pgrsReset(داده);

  /* Carry out the perform */
  نتیجه = imap_perform(داده, &connected, dophase_انجامشده);

  /* Perform post DO phase operations if necessary */
  إذا(!نتیجه && *dophase_انجامشده)
    نتیجه = imap_dophase_انجامشده(داده, imap, connected);

  أرجع نتیجه;
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
ساكن CURLcode imap_do(بنية Curl_easy *داده, bool *انجامشده)
{
  بنية IMAP *imap = Curl_meta_get(داده, CURL_META_IMAP_EASY);
  CURLcode نتیجه = CURLE_OK;
  *انجامشده = FALSE; /* default to false */

  إذا(!imap)
    أرجع CURLE_FAILED_INIT;
  /* Parse the URL path */
  نتیجه = imap_parse_url_مسیر(داده, imap);
  إذا(نتیجه)
    أرجع نتیجه;

  /* Parse the custom request */
  نتیجه = imap_parse_custom_request(داده, imap);
  إذا(نتیجه)
    أرجع نتیجه;

  نتیجه = imap_regular_transfer(داده, imap, انجامشده);

  أرجع نتیجه;
}

/***********************************************************************
 *
 * imap_disconnect()
 *
 * Disconnect from an IMAP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ساكن CURLcode imap_disconnect(بنية Curl_easy *داده,
                                بنية connectdata *conn, bool dead_connection)
{
  بنية imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);

  إذا(imapc &&
     /* We cannot send quit unconditionally. If this connection is stale or
        bad in any way (pingpong has pending data to send),
        sending quit and waiting around here will make the
        disconnect wait in vain and cause more problems than we need to. */
     !dead_connection && conn->بیتها.protoconnstart &&
     !Curl_pp_needs_flush(داده, &imapc->pp) &&
     !imap_perform_logout(داده, imapc))
    (فراغ)imap_بلوک_statemach(داده, imapc, TRUE); /* ignore errors */

  أرجع CURLE_OK;
}

/* Called from multi.c while DOing */
ساكن CURLcode imap_doing(بنية Curl_easy *داده, bool *dophase_انجامشده)
{
  بنية IMAP *imap = Curl_meta_get(داده, CURL_META_IMAP_EASY);
  CURLcode نتیجه;

  إذا(!imap)
    أرجع CURLE_FAILED_INIT;

  نتیجه = imap_multi_statemach(داده, dophase_انجامشده);
  إذا(نتیجه)
    DEBUGF(infof(داده, "DO phase failed"));
  وإلا إذا(*dophase_انجامشده) {
    نتیجه = imap_dophase_انجامشده(داده, imap, FALSE /* not connected */);

    DEBUGF(infof(داده, "DO phase is complete"));
  }

  أرجع نتیجه;
}

ساكن فراغ imap_easy_dtor(ثابت فراغ *کلید, size_t klen, فراغ *مدخل)
{
  بنية IMAP *imap = مدخل;
  (فراغ)کلید;
  (فراغ)klen;
  imap_easy_reset(imap);
  curlx_free(imap);
}

ساكن فراغ imap_conn_dtor(ثابت فراغ *کلید, size_t klen, فراغ *مدخل)
{
  بنية imap_conn *imapc = مدخل;
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
  imap_get_پیام,           /* Get SASL response message */
  0,                          /* No maximum initial response length */
  '+',                        /* Code received when continuation is expected */
  IMAP_RESP_OK,               /* Code to receive upon authentication success */
  SASL_AUTH_DEFAULT,          /* Default mechanisms */
  SASL_FLAG_BASE64            /* Configuration flags */
};

ساكن CURLcode imap_راهاندازی_connection(بنية Curl_easy *داده,
                                      بنية connectdata *conn)
{
  بنية imap_conn *imapc;
  بنية pingpong *pp;
  بنية IMAP *imap;

  imapc = curlx_calloc(1, حجم(*imapc));
  إذا(!imapc)
    أرجع CURLE_بیرون_از_حافظه;

  pp = &imapc->pp;
  PINGPONG_راهاندازی(pp, imap_pp_statemachine, imap_endofresp);

  /* Set the default preferred authentication type and mechanism */
  imapc->preftype = IMAP_TYPE_ANY;
  Curl_sasl_init(&imapc->sasl, داده, &saslimap);

  Curl_pp_init(pp, Curl_pgrs_now(داده));

  إذا(Curl_conn_meta_مجموعه(conn, CURL_META_IMAP_CONN, imapc, imap_conn_dtor))
    أرجع CURLE_بیرون_از_حافظه;

  imap = curlx_calloc(1, حجم(بنية IMAP));
  إذا(!imap ||
     Curl_meta_مجموعه(داده, CURL_META_IMAP_EASY, imap, imap_easy_dtor))
    أرجع CURLE_بیرون_از_حافظه;

  أرجع CURLE_OK;
}

/*
 * IMAP protocol.
 */
ثابت بنية Curl_protocol Curl_protocol_imap = {
  imap_راهاندازی_connection,            /* setup_connection */
  imap_do,                          /* do_it */
  imap_انجامشده,                        /* done */
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

#پایان_اگر /* CURL_DISABLE_IMAP */
