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
#شامل "curl_setup.h"
#شامل "urldata.h"
#شامل "imap.h"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_IMAP

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ __VMS
#شامل <in.h>
#شامل <inet.h>
#اگر_ختم

#شامل "curlx/dynbuf.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "progress.h"
#شامل "transfer.h"
#شامل "escape.h"
#شامل "pingpong.h"
#شامل "mime.h"
#شامل "curlx/strparse.h"
#شامل "strcase.h"
#شامل "vtls/vtls.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "select.h"
#شامل "url.h"
#شامل "bufref.h"
#شامل "curl_sasl.h"
#شامل "curlx/strcopy.h"

/* meta key for storing protocol meta at easy handle */
#تعریف_کرو CURL_META_IMAP_EASY   "meta:proto:imap:easy"
/* meta key for storing protocol meta at connection */
#تعریف_کرو CURL_META_IMAP_CONN   "meta:proto:imap:conn"

قسمتعریف شمار {
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
ساخت imap_conn {
  ساخت pingpong pp;
  ساخت SASL sasl;           /* SASL-related parameters */
  حرف *mailbox;              /* The last selected mailbox */
  imapstate state;            /* Always use imap.c:state() to change state! */
  غیرعلامتی صحیح mb_uidvalidity; /* UIDVALIDITY parsed from select response */
  حرف resptag[5];            /* Response tag to wait for */
  غیرعلامتی حرف preftype;     /* Preferred authentication type */
  غیرعلامتی حرف cmdid;        /* Last used command ID */
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
ساخت IMAP {
  curl_pp_transfer transfer;
  حرف *mailbox;          /* Mailbox to select */
  حرف *uid;              /* Message UID to fetch */
  حرف *mindex;           /* Index in mail box of mail to fetch */
  حرف *section;          /* Message SECTION to fetch */
  حرف *partial;          /* Message PARTIAL to fetch */
  حرف *query;            /* Query to search for */
  حرف *custom;           /* Custom request */
  حرف *custom_params;    /* Parameters for the custom request */
  غیرعلامتی صحیح uidvalidity; /* UIDVALIDITY to check in select */
  ثنائی_ہندسہ(uidvalidity_سیٹ);
};

#تعریف_کرو IMAP_RESP_OK       1
#تعریف_کرو IMAP_RESP_NOT_OK   2
#تعریف_کرو IMAP_RESP_PREAUTH  3

ساخت ulbits {
  صحیح ثنائی_ہندسہ_2;
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
ساکن CURLcode imap_sendf(ساخت Curl_easy *ڈیٹا,
                           ساخت imap_conn *imapc,
                           ثابت حرف *fmt, ...) CURL_PRINTF(3, 4);
ساکن CURLcode imap_sendf(ساخت Curl_easy *ڈیٹا,
                           ساخت imap_conn *imapc,
                           ثابت حرف *fmt, ...)
{
  CURLcode نتیجہ = CURLE_OK;
  va_list ap;

  DEBUGASSERT(fmt);

  /* Calculate the tag based on the connection ID and command ID */
  curl_msnprintf(imapc->resptag, حجم(imapc->resptag), "%c%03d",
                 'A' + curlx_sltosi((طویل)(ڈیٹا->conn->connection_id % 26)),
                 ++imapc->cmdid);

  va_start(ap, fmt);
  نتیجہ = Curl_pp_vsendf(ڈیٹا, &imapc->pp, imapc->resptag, fmt, ap);
  va_end(ap);
  واپس نتیجہ;
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
ساکن حرف *imap_atom(ثابت حرف *str, bool escape_only)
{
  ساخت dynbuf سطر;
  size_t nclean;
  size_t len;

  اگر(!str)
    واپس تہی;

  len = لڑی_لمبائی(str);
  nclean = strcspn(str, "() {%*]\\\"");
  اگر(len == nclean)
    /* nothing to escape, return a strdup */
    واپس curlx_strdup(str);

  curlx_dyn_init(&سطر, 2000);

  اگر(!escape_only && curlx_dyn_addn(&سطر, "\"", 1))
    واپس تہی;

  جب(*str) {
    اگر((*str == '\\' || *str == '"') &&
       curlx_dyn_addn(&سطر, "\\", 1))
      واپس تہی;
    اگر(curlx_dyn_addn(&سطر, str, 1))
      واپس تہی;
    str++;
  }

  اگر(!escape_only && curlx_dyn_addn(&سطر, "\"", 1))
    واپس تہی;

  واپس curlx_dyn_ptr(&سطر);
}

/*
 * Finds the start of a literal '{size}' in line, skipping over quoted strings.
 */
ساکن ثابت حرف *imap_ڈھونڈو_literal(ثابت حرف *سطر, size_t len)
{
  ثابت حرف *انتہا = سطر + len;
  bool اندر_quote = FALSE;

  جب(سطر < انتہا) {
    اگر(اندر_quote) {
      اگر(*سطر == '\\' && (سطر + 1) < انتہا) {
        سطر += 2;
        جاری;
      }
      اگر(*سطر == '"')
        اندر_quote = FALSE;
    }
    ورنہ {
      اگر(*سطر == '"')
        اندر_quote = TRUE;
      ورنہ اگر(*سطر == '{')
        واپس سطر;
    }
    سطر++;
  }
  واپس تہی;
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
ساکن bool imap_matchresp(ثابت حرف *سطر, size_t len, ثابت حرف *cmd)
{
  ثابت حرف *انتہا = سطر + len;
  size_t cmd_len = لڑی_لمبائی(cmd);

  /* Skip the untagged response marker */
  سطر += 2;

  /* Do we have a number after the marker? */
  اگر(سطر < انتہا && ISDIGIT(*سطر)) {
    /* Skip the number */
    کرو
      سطر++;
    جب(سطر < انتہا && ISDIGIT(*سطر));

    /* Do we have the space character? */
    اگر(سطر == انتہا || *سطر != ' ')
      واپس FALSE;

    سطر++;
  }

  /* Does the command name match and is it followed by a space character or at
     the end of line? */
  اگر(سطر + cmd_len <= انتہا && curl_strnequal(سطر, cmd, cmd_len) &&
     (سطر[cmd_len] == ' ' || سطر + cmd_len + 2 == انتہا))
    واپس TRUE;

  واپس FALSE;
}

/***********************************************************************
 *
 * imap_endofresp()
 *
 * Checks whether the given string is a valid tagged, untagged or continuation
 * response which can be processed by the response handler.
 */
ساکن bool imap_endofresp(ساخت Curl_easy *ڈیٹا, ساخت connectdata *conn,
                           ثابت حرف *سطر, size_t len, صحیح *resp)
{
  ساخت imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ساخت IMAP *imap = Curl_meta_get(ڈیٹا, CURL_META_IMAP_EASY);
  ثابت حرف *id;
  size_t id_len;

  DEBUGASSERT(imapc);
  DEBUGASSERT(imap);
  اگر(!imapc || !imap)
    واپس FALSE;

  /* Do we have a tagged command response? */
  id = imapc->resptag;
  id_len = لڑی_لمبائی(id);
  اگر(len >= id_len + 1 && !memcmp(id, سطر, id_len) && سطر[id_len] == ' ') {
    سطر += id_len + 1;
    len -= id_len + 1;

    اگر(len >= 2 && !memcmp(سطر, "OK", 2))
      *resp = IMAP_RESP_OK;
    ورنہ اگر(len >= 7 && !memcmp(سطر, "PREAUTH", 7))
      *resp = IMAP_RESP_PREAUTH;
    ورنہ
      *resp = IMAP_RESP_NOT_OK;

    واپس TRUE;
  }

  /* Do we have an untagged command response? */
  اگر(len >= 2 && !memcmp("* ", سطر, 2)) {
    انتخاب(imapc->state) {
    /* States which are interested in untagged responses */
    حالت IMAP_CAPABILITY:
      اگر(!imap_matchresp(سطر, len, "CAPABILITY"))
        واپس FALSE;
      توڑو;

    حالت IMAP_LIST:
      اگر((!imap->custom && !imap_matchresp(سطر, len, "LIST")) ||
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
        واپس FALSE;
      توڑو;

    حالت IMAP_SELECT:
      /* SELECT is special in that its untagged responses do not have a
         common prefix so accept anything! */
      توڑو;

    حالت IMAP_FETCH:
      اگر(!imap_matchresp(سطر, len, "FETCH"))
        واپس FALSE;
      توڑو;

    حالت IMAP_تلاش:
      اگر(!imap_matchresp(سطر, len, "SEARCH"))
        واپس FALSE;
      توڑو;

    /* Ignore other untagged responses */
    طےشدہ:
      واپس FALSE;
    }

    *resp = '*';
    واپس TRUE;
  }

  /* Do we have a continuation response? This should be a + symbol followed by
     a space and optionally some text as per RFC-3501 for the AUTHENTICATE and
     APPEND commands and as outlined in Section 4. Examples of RFC-4959 but
     some email servers ignore this and only send a single + instead. */
  اگر(!imap->custom && ((len == 3 && سطر[0] == '+') ||
                       (len >= 2 && !memcmp("+ ", سطر, 2)))) {
    انتخاب(imapc->state) {
    /* States which are interested in continuation responses */
    حالت IMAP_AUTHENTICATE:
    حالت IMAP_APPEND:
      *resp = '+';
      توڑو;

    طےشدہ:
      failf(ڈیٹا, "Unexpected continuation response");
      *resp = -1;
      توڑو;
    }

    واپس TRUE;
  }

  واپس FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * imap_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ساکن CURLcode imap_get_پیغام(ساخت Curl_easy *ڈیٹا, ساخت bufref *برآمد)
{
  ساخت imap_conn *imapc =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_IMAP_CONN);
  حرف *پیغام_2;
  size_t len;

  اگر(!imapc)
    واپس CURLE_FAILED_INIT;

  پیغام_2 = curlx_dyn_ptr(&imapc->pp.recvbuf);
  len = imapc->pp.nfinal;
  اگر(len > 2) {
    /* Find the start of the message */
    len -= 2;
    برائے(پیغام_2 += 2; ISBLANK(*پیغام_2); پیغام_2++, len--)
      ;

    /* Find the end of the message */
    جب(len--)
      اگر(!ISNEWLINE(پیغام_2[len]) && !ISBLANK(پیغام_2[len]))
        توڑو;

    /* Terminate the message */
    پیغام_2[++len] = '\0';
    Curl_bufref_سیٹ(برآمد, پیغام_2, len, تہی);
  }
  ورنہ
    /* junk input => zero length output */
    Curl_bufref_سیٹ(برآمد, "", 0, تہی);

  واپس CURLE_OK;
}

/***********************************************************************
 *
 * imap_state()
 *
 * This is the ONLY way to change IMAP state!
 */
ساکن خالی imap_state(ساخت Curl_easy *ڈیٹا,
                       ساخت imap_conn *imapc,
                       imapstate newstate)
{
#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ساکن ثابت حرف * ثابت نام[] = {
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

  اگر(imapc->state != newstate)
    infof(ڈیٹا, "IMAP %p state change from %s to %s",
          (خالی *)imapc, نام[imapc->state], نام[newstate]);
#ورنہ
  (خالی)ڈیٹا;
#اگر_ختم
  imapc->state = newstate;
}

/***********************************************************************
 *
 * imap_perform_capability()
 *
 * Sends the CAPABILITY command in order to obtain a list of server side
 * supported capabilities.
 */
ساکن CURLcode imap_perform_capability(ساخت Curl_easy *ڈیٹا,
                                        ساخت imap_conn *imapc)
{
  CURLcode نتیجہ = CURLE_OK;

  imapc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  imapc->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  imapc->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPABILITY command */
  نتیجہ = imap_sendf(ڈیٹا, imapc, "CAPABILITY");

  اگر(!نتیجہ)
    imap_state(ڈیٹا, imapc, IMAP_CAPABILITY);

  واپس نتیجہ;
}

/***********************************************************************
 *
 * imap_perform_starttls()
 *
 * Sends the STARTTLS command to start the upgrade to TLS.
 */
ساکن CURLcode imap_perform_starttls(ساخت Curl_easy *ڈیٹا,
                                      ساخت imap_conn *imapc)
{
  /* Send the STARTTLS command */
  CURLcode نتیجہ = imap_sendf(ڈیٹا, imapc, "STARTTLS");

  اگر(!نتیجہ)
    imap_state(ڈیٹا, imapc, IMAP_STARTTLS);

  واپس نتیجہ;
}

/***********************************************************************
 *
 * imap_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ساکن CURLcode imap_perform_upgrade_tls(ساخت Curl_easy *ڈیٹا,
                                         ساخت imap_conn *imapc,
                                         ساخت connectdata *conn)
{
#اگر_تعریف_شدہ USE_SSL
  /* Start the SSL connection */
  CURLcode نتیجہ;
  bool ssldone = FALSE;

  اگر(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    نتیجہ = Curl_ssl_cfilter_جوڑو(
      ڈیٹا, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    اگر(نتیجہ)
      جاؤ برآمد;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_imaps;
  }

  DEBUGASSERT(!imapc->ssldone);
  نتیجہ = Curl_conn_connect(ڈیٹا, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(ڈیٹا, "imap_perform_upgrade_tls, connect -> %d, %d",
               (صحیح)نتیجہ, ssldone));
  اگر(!نتیجہ && ssldone) {
    imapc->ssldone = ssldone;
    /* perform CAPA now, changes imapc->state out of IMAP_UPGRADETLS */
    نتیجہ = imap_perform_capability(ڈیٹا, imapc);
  }
برآمد:
  واپس نتیجہ;
#ورنہ
  (خالی)ڈیٹا;
  (خالی)imapc;
  (خالی)conn;
  واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
}

/***********************************************************************
 *
 * imap_perform_login()
 *
 * Sends a clear text LOGIN command to authenticate with.
 */
ساکن CURLcode imap_perform_login(ساخت Curl_easy *ڈیٹا,
                                   ساخت imap_conn *imapc,
                                   ساخت connectdata *conn)
{
  CURLcode نتیجہ = CURLE_OK;
  حرف *user;
  حرف *passwd;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  اگر(!conn->creds) {
    imap_state(ڈیٹا, imapc, IMAP_STOP);

    واپس نتیجہ;
  }

  /* Make sure the username and password are in the correct atom format */
  user = imap_atom(Curl_creds_user(conn->creds), FALSE);
  passwd = imap_atom(Curl_creds_passwd(conn->creds), FALSE);

  /* Send the LOGIN command */
  نتیجہ = imap_sendf(ڈیٹا, imapc, "LOGIN %s %s", user ? user : "",
                      passwd ? passwd : "");

  curlx_free(user);
  curlx_strzero(passwd);
  curlx_free(passwd);

  اگر(!نتیجہ)
    imap_state(ڈیٹا, imapc, IMAP_LOGIN);

  واپس نتیجہ;
}

/***********************************************************************
 *
 * imap_perform_authenticate()
 *
 * Sends an AUTHENTICATE command allowing the client to login with the given
 * SASL authentication mechanism.
 */
ساکن CURLcode imap_perform_authenticate(ساخت Curl_easy *ڈیٹا,
                                          ثابت حرف *mech,
                                          ثابت ساخت bufref *initresp)
{
  ساخت imap_conn *imapc =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_IMAP_CONN);
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *ir = Curl_bufref_ptr(initresp);

  اگر(!imapc)
    واپس CURLE_FAILED_INIT;
  اگر(ir) {
    /* Send the AUTHENTICATE command with the initial response */
    نتیجہ = imap_sendf(ڈیٹا, imapc, "AUTHENTICATE %s %s",
                        mech, *ir ? ir : "=");
  }
  ورنہ {
    /* Send the AUTHENTICATE command */
    نتیجہ = imap_sendf(ڈیٹا, imapc, "AUTHENTICATE %s", mech);
  }

  واپس نتیجہ;
}

/***********************************************************************
 *
 * imap_continue_authenticate()
 *
 * Sends SASL continuation data.
 */
ساکن CURLcode imap_continue_authenticate(ساخت Curl_easy *ڈیٹا,
                                           ثابت حرف *mech,
                                           ثابت ساخت bufref *resp)
{
  ساخت imap_conn *imapc =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_IMAP_CONN);

  (خالی)mech;
  اگر(!imapc)
    واپس CURLE_FAILED_INIT;
  واپس Curl_pp_sendf(ڈیٹا, &imapc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * imap_cancel_authenticate()
 *
 * Sends SASL cancellation.
 */
ساکن CURLcode imap_cancel_authenticate(ساخت Curl_easy *ڈیٹا,
                                         ثابت حرف *mech)
{
  ساخت imap_conn *imapc =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_IMAP_CONN);

  (خالی)mech;
  اگر(!imapc)
    واپس CURLE_FAILED_INIT;
  واپس Curl_pp_sendf(ڈیٹا, &imapc->pp, "*");
}

/***********************************************************************
 *
 * imap_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to clear text should a common
 * mechanism not be available between the client and server.
 */
ساکن CURLcode imap_perform_authentication(ساخت Curl_easy *ڈیٹا,
                                            ساخت imap_conn *imapc)
{
  CURLcode نتیجہ = CURLE_OK;
  saslprogress پیش_رفت;

  /* Check if already authenticated OR if there is enough data to authenticate
     with and end the connect phase if we do not */
  اگر(imapc->preauth ||
     !Curl_sasl_can_authenticate(&imapc->sasl, ڈیٹا)) {
    imap_state(ڈیٹا, imapc, IMAP_STOP);
    واپس نتیجہ;
  }

  /* Calculate the SASL login details */
  نتیجہ = Curl_sasl_آغاز(&imapc->sasl, ڈیٹا, (bool)imapc->ir_supported,
                           &پیش_رفت);
  اگر(!نتیجہ) {
    اگر(پیش_رفت == SASL_INPROGRESS)
      imap_state(ڈیٹا, imapc, IMAP_AUTHENTICATE);
    ورنہ اگر(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
      /* Perform clear text authentication */
      نتیجہ = imap_perform_login(ڈیٹا, imapc, ڈیٹا->conn);
    ورنہ
      نتیجہ = Curl_sasl_is_blocked(&imapc->sasl, ڈیٹا);
  }

  واپس نتیجہ;
}

/***********************************************************************
 *
 * imap_perform_list()
 *
 * Sends a LIST command or an alternative custom request.
 */
ساکن CURLcode imap_perform_list(ساخت Curl_easy *ڈیٹا,
                                  ساخت imap_conn *imapc,
                                  ساخت IMAP *imap)
{
  CURLcode نتیجہ = CURLE_OK;

  اگر(imap->custom)
    /* Send the custom request */
    نتیجہ = imap_sendf(ڈیٹا, imapc, "%s%s", imap->custom,
                        imap->custom_params ? imap->custom_params : "");
  ورنہ {
    /* Make sure the mailbox is in the correct atom format if necessary */
    حرف *mailbox = imap->mailbox ? imap_atom(imap->mailbox, TRUE)
                                  : curlx_strdup("");
    اگر(!mailbox)
      واپس CURLE_برآمد_کا_حافظہ;

    /* Send the LIST command */
    نتیجہ = imap_sendf(ڈیٹا, imapc, "LIST \"%s\" *", mailbox);

    curlx_free(mailbox);
  }

  اگر(!نتیجہ)
    imap_state(ڈیٹا, imapc, IMAP_LIST);

  واپس نتیجہ;
}

/***********************************************************************
 *
 * imap_perform_select()
 *
 * Sends a SELECT command to ask the server to change the selected mailbox.
 */
ساکن CURLcode imap_perform_select(ساخت Curl_easy *ڈیٹا,
                                    ساخت imap_conn *imapc,
                                    ساخت IMAP *imap)
{
  CURLcode نتیجہ = CURLE_OK;
  حرف *mailbox;

  /* Invalidate old information as we are switching mailboxes */
  curlx_safefree(imapc->mailbox);
  imapc->mb_uidvalidity_سیٹ = FALSE;

  /* Check we have a mailbox */
  اگر(!imap->mailbox) {
    failf(ڈیٹا, "Cannot SELECT without a mailbox.");
    واپس CURLE_URL_MALFORMAT;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  اگر(!mailbox)
    واپس CURLE_برآمد_کا_حافظہ;

  /* Send the SELECT command */
  نتیجہ = imap_sendf(ڈیٹا, imapc, "SELECT %s", mailbox);

  curlx_free(mailbox);

  اگر(!نتیجہ)
    imap_state(ڈیٹا, imapc, IMAP_SELECT);

  واپس نتیجہ;
}

/***********************************************************************
 *
 * imap_perform_fetch()
 *
 * Sends a FETCH command to initiate the download of a message.
 */
ساکن CURLcode imap_perform_fetch(ساخت Curl_easy *ڈیٹا,
                                   ساخت imap_conn *imapc,
                                   ساخت IMAP *imap)
{
  CURLcode نتیجہ = CURLE_OK;
  /* Check we have a UID */
  اگر(imap->uid) {

    /* Send the FETCH command */
    اگر(imap->partial)
      نتیجہ = imap_sendf(ڈیٹا, imapc, "UID FETCH %s BODY[%s]<%s>",
                          imap->uid, imap->section ? imap->section : "",
                          imap->partial);
    ورنہ
      نتیجہ = imap_sendf(ڈیٹا, imapc, "UID FETCH %s BODY[%s]",
                          imap->uid, imap->section ? imap->section : "");
  }
  ورنہ اگر(imap->mindex) {
    /* Send the FETCH command */
    اگر(imap->partial)
      نتیجہ = imap_sendf(ڈیٹا, imapc, "FETCH %s BODY[%s]<%s>",
                          imap->mindex, imap->section ? imap->section : "",
                          imap->partial);
    ورنہ
      نتیجہ = imap_sendf(ڈیٹا, imapc, "FETCH %s BODY[%s]",
                          imap->mindex, imap->section ? imap->section : "");
  }
  ورنہ {
    failf(ڈیٹا, "Cannot FETCH without a UID.");
    واپس CURLE_URL_MALFORMAT;
  }
  اگر(!نتیجہ)
    imap_state(ڈیٹا, imapc, IMAP_FETCH);

  واپس نتیجہ;
}

/***********************************************************************
 *
 * imap_perform_append()
 *
 * Sends an APPEND command to initiate the upload of a message.
 */
ساکن CURLcode imap_perform_append(ساخت Curl_easy *ڈیٹا,
                                    ساخت imap_conn *imapc,
                                    ساخت IMAP *imap)
{
  CURLcode نتیجہ = CURLE_OK;
  حرف *mailbox;
  ساخت dynbuf flags;

  /* Check we have a mailbox */
  اگر(!imap->mailbox) {
    failf(ڈیٹا, "Cannot APPEND without a mailbox.");
    واپس CURLE_URL_MALFORMAT;
  }

#اگر_غیر_تعریف_شدہ CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  اگر(IS_MIME_POST(ڈیٹا)) {
    curl_mimepart *postp = ڈیٹا->سیٹ.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(غیرعلامتی صحیح)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_سرخیاں(postp, ڈیٹا->سیٹ.سرخیاں, 0);
    نتیجہ = Curl_mime_prepare_سرخیاں(ڈیٹا, postp, تہی,
                                       تہی, MIMESTRATEGY_MAIL);

    اگر(!نتیجہ && !Curl_checkheaders(ڈیٹا, STRCONST("Mime-Version")))
      نتیجہ = Curl_mime_جوڑو_سرخی(&postp->curlheaders, "Mime-Version: 1.0");
    اگر(!نتیجہ)
      نتیجہ = Curl_creader_سیٹ_mime(ڈیٹا, postp);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
  ورنہ
#اگر_ختم
  {
    نتیجہ = Curl_creader_سیٹ_fread(ڈیٹا, ڈیٹا->state.infilesize);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  /* Check we know the size of the upload. This takes all readers
   * into account. Especially crlf conversions which make the size
   * unpredictable, e.g. -1. */
  ڈیٹا->state.infilesize = Curl_creader_کل_لمبائی(ڈیٹا);
  اگر(ڈیٹا->state.infilesize < 0) {
    اگر(ڈیٹا->سیٹ.crlf)
      failf(ڈیٹا, "Cannot APPEND with CRLF conversion making size unknown");
    ورنہ
      failf(ڈیٹا, "Cannot APPEND with unknown input file size");
    واپس CURLE_UPLOAD_FAILED;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  اگر(!mailbox)
    واپس CURLE_برآمد_کا_حافظہ;

  /* Generate flags string and send the APPEND command */
  curlx_dyn_init(&flags, 100);
  اگر(ڈیٹا->سیٹ.upload_flags) {
    صحیح i;
    ساخت ulbits ulflag[] = {
      { CURLULFLAG_ANSWERED, "Answered" },
      { CURLULFLAG_DELETED, "Deleted" },
      { CURLULFLAG_DRAFT, "Draft" },
      { CURLULFLAG_FLAGGED, "Flagged" },
      { CURLULFLAG_نظر_آیا, "Seen" },
      { 0, تہی }
    };

    نتیجہ = CURLE_برآمد_کا_حافظہ;
    اگر(curlx_dyn_جوڑو(&flags, " (")) {
      جاؤ cleanup;
    }

    برائے(i = 0; ulflag[i].ثنائی_ہندسہ_2; i++) {
      اگر(ڈیٹا->سیٹ.upload_flags & ulflag[i].ثنائی_ہندسہ_2 &&
         ((curlx_dyn_len(&flags) > 2 && curlx_dyn_جوڑو(&flags, " ")) ||
          curlx_dyn_جوڑو(&flags, "\\") ||
          curlx_dyn_جوڑو(&flags, ulflag[i].flag)))
        جاؤ cleanup;
    }

    اگر(curlx_dyn_جوڑو(&flags, ")"))
      جاؤ cleanup;
  }
  ورنہ اگر(curlx_dyn_جوڑو(&flags, ""))
    جاؤ cleanup;

  نتیجہ = imap_sendf(ڈیٹا, imapc, "APPEND %s%s {%" FMT_OFF_T "}",
                      mailbox, curlx_dyn_ptr(&flags), ڈیٹا->state.infilesize);

cleanup:
  curlx_dyn_free(&flags);
  curlx_free(mailbox);

  اگر(!نتیجہ)
    imap_state(ڈیٹا, imapc, IMAP_APPEND);

  واپس نتیجہ;
}

/***********************************************************************
 *
 * imap_perform_search()
 *
 * Sends a SEARCH command.
 */
ساکن CURLcode imap_perform_تلاش(ساخت Curl_easy *ڈیٹا,
                                    ساخت imap_conn *imapc,
                                    ساخت IMAP *imap)
{
  CURLcode نتیجہ = CURLE_OK;

  /* Check we have a query string */
  اگر(!imap->query) {
    failf(ڈیٹا, "Cannot SEARCH without a query string.");
    واپس CURLE_URL_MALFORMAT;
  }

  /* Send the SEARCH command */
  نتیجہ = imap_sendf(ڈیٹا, imapc, "SEARCH %s", imap->query);

  اگر(!نتیجہ)
    imap_state(ڈیٹا, imapc, IMAP_تلاش);

  واپس نتیجہ;
}

/***********************************************************************
 *
 * imap_perform_logout()
 *
 * Performs the logout action prior to sclose() being called.
 */
ساکن CURLcode imap_perform_logout(ساخت Curl_easy *ڈیٹا,
                                    ساخت imap_conn *imapc)
{
  /* Send the LOGOUT command */
  CURLcode نتیجہ = imap_sendf(ڈیٹا, imapc, "LOGOUT");

  اگر(!نتیجہ)
    imap_state(ڈیٹا, imapc, IMAP_LOGOUT);

  واپس نتیجہ;
}

/* For the initial server greeting */
ساکن CURLcode imap_state_servergreet_resp(ساخت Curl_easy *ڈیٹا,
                                            ساخت imap_conn *imapc,
                                            صحیح imapcode,
                                            imapstate instate)
{
  (خالی)instate;

  اگر(imapcode == IMAP_RESP_PREAUTH) {
    /* PREAUTH */
    imapc->preauth = TRUE;
    infof(ڈیٹا, "PREAUTH connection, already authenticated");
  }
  ورنہ اگر(imapcode != IMAP_RESP_OK) {
    failf(ڈیٹا, "Got unexpected imap-server response");
    واپس CURLE_WEIRD_SERVER_جواب;
  }

  واپس imap_perform_capability(ڈیٹا, imapc);
}

/* For CAPABILITY responses */
ساکن CURLcode imap_state_capability_resp(ساخت Curl_easy *ڈیٹا,
                                           ساخت imap_conn *imapc,
                                           صحیح imapcode,
                                           imapstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *سطر = curlx_dyn_ptr(&imapc->pp.recvbuf);

  (خالی)instate;

  /* Do we have an untagged response? */
  اگر(imapcode == '*') {
    سطر += 2;

    /* Loop through the data line */
    برائے(;;) {
      size_t wordlen;
      جب(*سطر && (ISBLANK(*سطر) || ISNEWLINE(*سطر)))
        سطر++;

      اگر(!*سطر)
        توڑو;

      /* Extract the word */
      برائے(wordlen = 0; سطر[wordlen] && !ISBLANK(سطر[wordlen]) &&
                       !ISNEWLINE(سطر[wordlen]);)
        wordlen++;

      /* Does the server support the STARTTLS capability? */
      اگر(wordlen == 8 && curl_strnequal(سطر, "STARTTLS", 8))
        imapc->tls_supported = TRUE;

      /* Has the server explicitly disabled clear text authentication? */
      ورنہ اگر(wordlen == 13 && curl_strnequal(سطر, "LOGINDISABLED", 13))
        imapc->login_disabled = TRUE;

      /* Does the server support the SASL-IR capability? */
      ورنہ اگر(wordlen == 7 && curl_strnequal(سطر, "SASL-IR", 7))
        imapc->ir_supported = TRUE;

      /* Do we have a SASL based authentication mechanism? */
      ورنہ اگر(wordlen > 5 && curl_strnequal(سطر, "AUTH=", 5)) {
        size_t llen;
        غیرعلامتی مختصر mechbit;

        سطر += 5;
        wordlen -= 5;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(سطر, wordlen, &llen);
        اگر(mechbit && llen == wordlen)
          imapc->sasl.authmechs |= mechbit;
      }

      سطر += wordlen;
    }
  }
  ورنہ اگر(ڈیٹا->سیٹ.use_ssl && !Curl_conn_is_ssl(ڈیٹا->conn, FIRSTSOCKET)) {
    /* PREAUTH is not compatible with STARTTLS. */
    اگر(imapcode == IMAP_RESP_OK && imapc->tls_supported && !imapc->preauth) {
      /* Switch to TLS connection now */
      نتیجہ = imap_perform_starttls(ڈیٹا, imapc);
    }
    ورنہ اگر(ڈیٹا->سیٹ.use_ssl <= CURLUSESSL_TRY)
      نتیجہ = imap_perform_authentication(ڈیٹا, imapc);
    ورنہ {
      failf(ڈیٹا, "STARTTLS not available.");
      نتیجہ = CURLE_USE_SSL_FAILED;
    }
  }
  ورنہ
    نتیجہ = imap_perform_authentication(ڈیٹا, imapc);

  واپس نتیجہ;
}

/* For STARTTLS responses */
ساکن CURLcode imap_state_starttls_resp(ساخت Curl_easy *ڈیٹا,
                                         ساخت imap_conn *imapc,
                                         صحیح imapcode,
                                         imapstate instate)
{
  CURLcode نتیجہ = CURLE_OK;

  (خالی)instate;

  /* Pipelining in response is forbidden. */
  اگر(imapc->pp.حد_سے_تجاوز)
    واپس CURLE_WEIRD_SERVER_جواب;

  اگر(imapcode != IMAP_RESP_OK) {
    اگر(ڈیٹا->سیٹ.use_ssl != CURLUSESSL_TRY) {
      failf(ڈیٹا, "STARTTLS denied");
      نتیجہ = CURLE_USE_SSL_FAILED;
    }
    ورنہ
      نتیجہ = imap_perform_authentication(ڈیٹا, imapc);
  }
  ورنہ
    imap_state(ڈیٹا, imapc, IMAP_UPGRADETLS);

  واپس نتیجہ;
}

/* For SASL authentication responses */
ساکن CURLcode imap_state_auth_resp(ساخت Curl_easy *ڈیٹا,
                                     ساخت imap_conn *imapc,
                                     صحیح imapcode,
                                     imapstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  saslprogress پیش_رفت;

  (خالی)instate;

  نتیجہ = Curl_sasl_continue(&imapc->sasl, ڈیٹا, imapcode, &پیش_رفت);
  اگر(!نتیجہ)
    انتخاب(پیش_رفت) {
    حالت SASL_مکمل:
      imap_state(ڈیٹا, imapc, IMAP_STOP);  /* Authenticated */
      توڑو;
    حالت SASL_IDLE:            /* No mechanism left after cancellation */
      اگر(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
        /* Perform clear text authentication */
        نتیجہ = imap_perform_login(ڈیٹا, imapc, ڈیٹا->conn);
      ورنہ {
        failf(ڈیٹا, "Authentication cancelled");
        نتیجہ = CURLE_LOGIN_DENIED;
      }
      توڑو;
    طےشدہ:
      توڑو;
    }

  واپس نتیجہ;
}

/* For LOGIN responses */
ساکن CURLcode imap_state_login_resp(ساخت Curl_easy *ڈیٹا,
                                      ساخت imap_conn *imapc,
                                      صحیح imapcode,
                                      imapstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  (خالی)instate;

  اگر(imapcode != IMAP_RESP_OK) {
    failf(ڈیٹا, "Access denied. %c", imapcode);
    نتیجہ = CURLE_LOGIN_DENIED;
  }
  ورنہ
    /* End of connect phase */
    imap_state(ڈیٹا, imapc, IMAP_STOP);

  واپس نتیجہ;
}

/* Detect IMAP listings vs. downloading a single email */
ساکن bool is_custom_fetch_listing_مطابقت(ثابت حرف *params)
{
  /* match " 1:* (FLAGS ..." or " 1,2,3 (FLAGS ..." */
  اگر(*params++ != ' ')
    واپس FALSE;

  جب(ISDIGIT(*params)) {
    params++;
    اگر(*params == 0)
      واپس FALSE;
  }
  اگر(*params == ':')
    واپس TRUE;
  اگر(*params == ',')
    واپس TRUE;
  واپس FALSE;
}

ساکن bool is_custom_fetch_listing(ساخت IMAP *imap)
{
  /* filter out "UID FETCH 1:* (FLAGS ..." queries to list emails */
  اگر(!imap->custom)
    واپس FALSE;
  ورنہ اگر(curl_strequal(imap->custom, "FETCH") && imap->custom_params) {
    ثابت حرف *p = imap->custom_params;
    واپس is_custom_fetch_listing_مطابقت(p);
  }
  ورنہ اگر(curl_strequal(imap->custom, "UID") && imap->custom_params &&
          curl_strnequal(imap->custom_params, " FETCH ", 7)) {
    ثابت حرف *p = imap->custom_params + 6;
    واپس is_custom_fetch_listing_مطابقت(p);
  }
  واپس FALSE;
}

/* For LIST and SEARCH responses */
ساکن CURLcode imap_state_listsearch_resp(ساخت Curl_easy *ڈیٹا,
                                           ساخت imap_conn *imapc,
                                           صحیح imapcode,
                                           imapstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *سطر = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  ساخت IMAP *imap = Curl_meta_get(ڈیٹا, CURL_META_IMAP_EASY);

  DEBUGASSERT(imap);
  اگر(!imap)
    واپس CURLE_FAILED_INIT;
  (خالی)instate;

  اگر(imapcode == '*' && is_custom_fetch_listing(imap)) {
    /* custom FETCH or UID FETCH for listing is not handled here */
  }
  ورنہ اگر(imapcode == '*') {
    /* Check if this response contains a literal (e.g. FETCH responses with
       body data). Literal syntax is {size}\r\n */
    ثابت حرف *cr = memchr(سطر, '\r', len);
    size_t سطر_len = cr ? (size_t)(cr - سطر) : len;
    ثابت حرف *ptr = imap_ڈھونڈو_literal(سطر, سطر_len);
    اگر(ptr) {
      curl_off_t سائز = 0;
      bool parsed = FALSE;
      ptr++;
      اگر(!curlx_str_نمبر(&ptr, &سائز, CURL_OFF_T_MAX) &&
         !curlx_str_single(&ptr, '}'))
        parsed = TRUE;

      اگر(parsed) {
        ساخت pingpong *pp = &imapc->pp;
        size_t بفر_len = curlx_dyn_len(&pp->recvbuf);
        size_t after_سرخی = بفر_len - pp->nfinal;

        /* This is a literal response, setup to receive the body data */
        infof(ڈیٹا, "Found %" FMT_OFF_T " bytes to download", سائز);

        /* First write the header line */
        نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, سطر, len);
        اگر(نتیجہ)
          واپس نتیجہ;

        /* Handle data already in buffer after the header line */
        اگر(after_سرخی > 0) {
          /* There is already data in the buffer that is part of the literal
             body or subsequent responses */
          size_t chunk = after_سرخی;

          /* Keep only the data after the header line */
          curlx_dyn_دم(&pp->recvbuf, chunk);
          pp->nfinal = 0; /* done */

          /* Limit chunk to the literal size */
          اگر((curl_off_t)chunk > سائز)
            chunk = (size_t)سائز;

          اگر(chunk) {
            /* Write the literal body data */
            نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY,
                                       curlx_dyn_ptr(&pp->recvbuf), chunk);
            اگر(نتیجہ)
              واپس نتیجہ;
          }

          /* Handle remaining data in buffer (either more literal data or
             subsequent responses) */
          اگر(after_سرخی > chunk) {
            /* Keep the data after the literal body */
            pp->حد_سے_تجاوز = after_سرخی - chunk;
            curlx_dyn_دم(&pp->recvbuf, pp->حد_سے_تجاوز);
          }
          ورنہ {
            pp->حد_سے_تجاوز = 0;
            curlx_dyn_reset(&pp->recvbuf);
          }
        }
        ورنہ {
          /* No data in buffer yet, reset overflow */
          pp->حد_سے_تجاوز = 0;
        }

        اگر((CURL_OFF_T_MAX - سائز) < (curl_off_t)len)
          /* unlikely to actually be a transfer this big, but avoid integer
حد_سے_تجاوز*/
          سائز = CURL_OFF_T_MAX;
        ورنہ
          سائز += len;

        /* Progress size includes both header line and literal body */
        Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, سائز);

        اگر(ڈیٹا->req.bytecount == سائز)
          /* All data already transferred (header + literal body) */
          Curl_xfer_تیاری_nop(ڈیٹا);
        ورنہ {
          /* Setup to receive the literal body data.
             maxdownload and transfer size include both header line and
             literal body */
          ڈیٹا->req.maxdownload = سائز;
          Curl_xfer_تیاری_recv(ڈیٹا, FIRSTSOCKET, سائز);
        }
        /* End of DO phase */
        imap_state(ڈیٹا, imapc, IMAP_STOP);
      }
      ورنہ {
        /* Failed to parse literal, write the line */
        نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, سطر, len);
      }
    }
    ورنہ {
      /* No literal, write the line as-is */
      نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, سطر, len);
    }
  }
  ورنہ اگر(imapcode != IMAP_RESP_OK)
    نتیجہ = CURLE_QUOTE_ERROR;
  ورنہ
    /* End of DO phase */
    imap_state(ڈیٹا, imapc, IMAP_STOP);

  واپس نتیجہ;
}

/* For SELECT responses */
ساکن CURLcode imap_state_select_resp(ساخت Curl_easy *ڈیٹا,
                                       ساخت imap_conn *imapc,
                                       ساخت IMAP *imap,
                                       صحیح imapcode,
                                       imapstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  (خالی)instate;

  اگر(imapcode == '*') {
    /* See if this is an UIDVALIDITY response */
    ثابت حرف *سطر = curlx_dyn_ptr(&imapc->pp.recvbuf);
    size_t len = curlx_dyn_len(&imapc->pp.recvbuf);
    اگر((len >= 18) && checkprefix("OK [UIDVALIDITY ", &سطر[2])) {
      curl_off_t قدر;
      ثابت حرف *p = &سطر[2] + CURL_CSTRLEN("OK [UIDVALIDITY ");
      اگر(!curlx_str_نمبر(&p, &قدر, UINT_MAX)) {
        imapc->mb_uidvalidity = (غیرعلامتی صحیح)قدر;
        imapc->mb_uidvalidity_سیٹ = TRUE;
      }
    }
  }
  ورنہ اگر(imapcode == IMAP_RESP_OK) {
    /* Check if the UIDVALIDITY has been specified and matches */
    اگر(imap->uidvalidity_سیٹ && imapc->mb_uidvalidity_سیٹ &&
       (imap->uidvalidity != imapc->mb_uidvalidity)) {
      failf(ڈیٹا, "Mailbox UIDVALIDITY has changed");
      نتیجہ = CURLE_REMOTE_FILE_NOT_ملا;
    }
    ورنہ {
      /* Note the currently opened mailbox on this connection */
      DEBUGASSERT(!imapc->mailbox);
      imapc->mailbox = curlx_strdup(imap->mailbox);
      اگر(!imapc->mailbox)
        واپس CURLE_برآمد_کا_حافظہ;

      اگر(imap->custom)
        نتیجہ = imap_perform_list(ڈیٹا, imapc, imap);
      ورنہ اگر(imap->query)
        نتیجہ = imap_perform_تلاش(ڈیٹا, imapc, imap);
      ورنہ
        نتیجہ = imap_perform_fetch(ڈیٹا, imapc, imap);
    }
  }
  ورنہ {
    failf(ڈیٹا, "Select failed");
    نتیجہ = CURLE_LOGIN_DENIED;
  }

  واپس نتیجہ;
}

/* For the (first line of the) FETCH responses */
ساکن CURLcode imap_state_fetch_resp(ساخت Curl_easy *ڈیٹا,
                                      ساخت imap_conn *imapc,
                                      صحیح imapcode,
                                      imapstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت pingpong *pp = &imapc->pp;
  ثابت حرف *ptr = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  bool parsed = FALSE;
  curl_off_t سائز = 0;

  (خالی)instate;

  اگر(imapcode != '*') {
    Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, -1);
    imap_state(ڈیٹا, imapc, IMAP_STOP);
    واپس CURLE_REMOTE_FILE_NOT_ملا;
  }

  /* Something like this is received "* 1 FETCH (BODY[TEXT] {2021}\r" so parse
     the continuation data contained within the curly brackets */
  ptr = imap_ڈھونڈو_literal(ptr, len);
  اگر(ptr) {
    ptr++;
    اگر(!curlx_str_نمبر(&ptr, &سائز, CURL_OFF_T_MAX) &&
       !curlx_str_single(&ptr, '}'))
      parsed = TRUE;
  }

  اگر(parsed) {
    infof(ڈیٹا, "Found %" FMT_OFF_T " bytes to download", سائز);
    Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, سائز);

    اگر(pp->حد_سے_تجاوز) {
      /* At this point there is a data in the receive buffer that is body
         content, send it as body and then skip it. Do note that there may
         even be additional "headers" after the body. */
      size_t chunk = pp->حد_سے_تجاوز;

      /* keep only the overflow */
      curlx_dyn_دم(&pp->recvbuf, chunk);
      pp->nfinal = 0; /* done */

      اگر((curl_off_t)chunk > سائز)
        /* The conversion from curl_off_t to size_t is always fine here */
        chunk = (size_t)سائز;

      اگر(!chunk) {
        /* no size, we are done with the data */
        imap_state(ڈیٹا, imapc, IMAP_STOP);
        واپس CURLE_OK;
      }
      نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY,
                                 curlx_dyn_ptr(&pp->recvbuf), chunk);
      اگر(نتیجہ)
        واپس نتیجہ;

      infof(ڈیٹا, "Written %zu bytes, %" FMT_OFF_T
            " bytes are left for transfer", chunk, (curl_off_t)(سائز - chunk));

      /* Have we used the entire overflow or part of it?*/
      اگر(pp->حد_سے_تجاوز > chunk) {
        /* remember the remaining trailing overflow data */
        pp->حد_سے_تجاوز -= chunk;
        curlx_dyn_دم(&pp->recvbuf, pp->حد_سے_تجاوز);
      }
      ورنہ {
        pp->حد_سے_تجاوز = 0; /* handled */
        /* Free the cache */
        curlx_dyn_reset(&pp->recvbuf);
      }
    }

    اگر(ڈیٹا->req.bytecount == سائز)
      /* The entire data is already transferred! */
      Curl_xfer_تیاری_nop(ڈیٹا);
    ورنہ {
      /* IMAP download */
      ڈیٹا->req.maxdownload = سائز;
      Curl_xfer_تیاری_recv(ڈیٹا, FIRSTSOCKET, سائز);
    }
  }
  ورنہ {
    /* We do not know how to parse this line */
    failf(ڈیٹا, "Failed to parse FETCH response.");
    نتیجہ = CURLE_WEIRD_SERVER_جواب;
  }

  /* End of DO phase */
  imap_state(ڈیٹا, imapc, IMAP_STOP);

  واپس نتیجہ;
}

/* For final FETCH responses performed after the download */
ساکن CURLcode imap_state_fetch_final_resp(ساخت Curl_easy *ڈیٹا,
                                            ساخت imap_conn *imapc,
                                            صحیح imapcode,
                                            imapstate instate)
{
  CURLcode نتیجہ = CURLE_OK;

  (خالی)instate;

  اگر(imapcode != IMAP_RESP_OK)
    نتیجہ = CURLE_WEIRD_SERVER_جواب;
  ورنہ
    /* End of DONE phase */
    imap_state(ڈیٹا, imapc, IMAP_STOP);

  واپس نتیجہ;
}

/* For APPEND responses */
ساکن CURLcode imap_state_append_resp(ساخت Curl_easy *ڈیٹا,
                                       ساخت imap_conn *imapc,
                                       صحیح imapcode,
                                       imapstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  (خالی)instate;

  اگر(imapcode != '+') {
    نتیجہ = CURLE_UPLOAD_FAILED;
  }
  ورنہ {
    /* Set the progress upload size */
    Curl_pgrs_سیٹ_Upload_سائز(ڈیٹا, ڈیٹا->state.infilesize);

    /* IMAP upload */
    Curl_xfer_تیاری_send(ڈیٹا, FIRSTSOCKET);

    /* End of DO phase */
    imap_state(ڈیٹا, imapc, IMAP_STOP);
  }

  واپس نتیجہ;
}

/* For final APPEND responses performed after the upload */
ساکن CURLcode imap_state_append_final_resp(ساخت Curl_easy *ڈیٹا,
                                             ساخت imap_conn *imapc,
                                             صحیح imapcode,
                                             imapstate instate)
{
  CURLcode نتیجہ = CURLE_OK;

  (خالی)instate;

  اگر(imapcode != IMAP_RESP_OK)
    نتیجہ = CURLE_UPLOAD_FAILED;
  ورنہ
    /* End of DONE phase */
    imap_state(ڈیٹا, imapc, IMAP_STOP);

  واپس نتیجہ;
}

ساکن CURLcode imap_pp_statemachine(ساخت Curl_easy *ڈیٹا,
                                     ساخت connectdata *conn)
{
  CURLcode نتیجہ = CURLE_OK;
  صحیح imapcode;
  ساخت imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ساخت IMAP *imap = Curl_meta_get(ڈیٹا, CURL_META_IMAP_EASY);
  ساخت pingpong *pp;
  size_t nread = 0;

  اگر(!imapc || !imap)
    واپس CURLE_FAILED_INIT;
  pp = &imapc->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not IMAP */
upgrade_tls:
  اگر(imapc->state == IMAP_UPGRADETLS) {
    نتیجہ = imap_perform_upgrade_tls(ڈیٹا, imapc, conn);
    اگر(نتیجہ || (imapc->state == IMAP_UPGRADETLS))
      واپس نتیجہ;
  }

  /* Flush any data that needs to be sent */
  اگر(pp->sendleft)
    واپس Curl_pp_flushsend(ڈیٹا, pp);

  کرو {
    /* Read the response from the server */
    نتیجہ = Curl_pp_readresp(ڈیٹا, FIRSTSOCKET, pp, &imapcode, &nread);
    اگر(نتیجہ)
      واپس نتیجہ;

    /* Was there an error parsing the response line? */
    اگر(imapcode == -1)
      واپس CURLE_WEIRD_SERVER_جواب;

    اگر(!imapcode)
      توڑو;

    /* We have now received a full IMAP server response */
    انتخاب(imapc->state) {
    حالت IMAP_SERVERGREET:
      نتیجہ = imap_state_servergreet_resp(ڈیٹا, imapc,
                                           imapcode, imapc->state);
      توڑو;

    حالت IMAP_CAPABILITY:
      نتیجہ = imap_state_capability_resp(ڈیٹا, imapc, imapcode, imapc->state);
      توڑو;

    حالت IMAP_STARTTLS:
      نتیجہ = imap_state_starttls_resp(ڈیٹا, imapc, imapcode, imapc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      اگر(!نتیجہ && (imapc->state == IMAP_UPGRADETLS))
        جاؤ upgrade_tls;
      توڑو;

    حالت IMAP_AUTHENTICATE:
      نتیجہ = imap_state_auth_resp(ڈیٹا, imapc, imapcode, imapc->state);
      توڑو;

    حالت IMAP_LOGIN:
      نتیجہ = imap_state_login_resp(ڈیٹا, imapc, imapcode, imapc->state);
      توڑو;

    حالت IMAP_LIST:
    حالت IMAP_تلاش:
      نتیجہ = imap_state_listsearch_resp(ڈیٹا, imapc, imapcode, imapc->state);
      توڑو;

    حالت IMAP_SELECT:
      نتیجہ = imap_state_select_resp(ڈیٹا, imapc, imap,
                                      imapcode, imapc->state);
      توڑو;

    حالت IMAP_FETCH:
      نتیجہ = imap_state_fetch_resp(ڈیٹا, imapc, imapcode, imapc->state);
      توڑو;

    حالت IMAP_FETCH_FINAL:
      نتیجہ = imap_state_fetch_final_resp(ڈیٹا, imapc,
                                           imapcode, imapc->state);
      توڑو;

    حالت IMAP_APPEND:
      نتیجہ = imap_state_append_resp(ڈیٹا, imapc, imapcode, imapc->state);
      توڑو;

    حالت IMAP_APPEND_FINAL:
      نتیجہ = imap_state_append_final_resp(ڈیٹا, imapc,
                                            imapcode, imapc->state);
      توڑو;

    حالت IMAP_LOGOUT:
    طےشدہ:
      /* internal error */
      imap_state(ڈیٹا, imapc, IMAP_STOP);
      توڑو;
    }
  } جب(!نتیجہ && imapc->state != IMAP_STOP && Curl_pp_moredata(pp));

  واپس نتیجہ;
}

/* Called repeatedly until done from multi.c */
ساکن CURLcode imap_multi_statemach(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت imap_conn *imapc =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_IMAP_CONN);

  *مکمل = FALSE;
  اگر(!imapc)
    واپس CURLE_FAILED_INIT;
  نتیجہ = Curl_pp_statemach(ڈیٹا, &imapc->pp, FALSE, FALSE);
  *مکمل = (imapc->state == IMAP_STOP);

  واپس نتیجہ;
}

ساکن CURLcode imap_بلاک_statemach(ساخت Curl_easy *ڈیٹا,
                                     ساخت imap_conn *imapc,
                                     bool disconnecting)
{
  CURLcode نتیجہ = CURLE_OK;

  جب(imapc->state != IMAP_STOP && !نتیجہ)
    نتیجہ = Curl_pp_statemach(ڈیٹا, &imapc->pp, TRUE, disconnecting);

  واپس نتیجہ;
}

/* For the IMAP "protocol connect" and "doing" phases only */
ساکن CURLcode imap_pollset(ساخت Curl_easy *ڈیٹا,
                             ساخت easy_pollset *ps)
{
  ساخت imap_conn *imapc =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_IMAP_CONN);
  واپس imapc ? Curl_pp_pollset(ڈیٹا, &imapc->pp, ps) : CURLE_OK;
}

ساکن خالی imap_easy_reset(ساخت IMAP *imap)
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
ساکن bool imap_is_bchar(حرف ch)
{
  /* Performing the alnum check first with macro is faster because of ASCII
     arithmetic */
  واپس ch && (ISALNUM(ch) || strchr(":@/&=-._~!$\'()*+,%", ch));
}

/***********************************************************************
 *
 * imap_parse_url_options()
 *
 * Parse the URL login options.
 */
ساکن CURLcode imap_parse_url_options(ساخت connectdata *conn,
                                       ساخت imap_conn *imapc)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *ptr = conn->options;
  bool prefer_login = FALSE;

  جب(!نتیجہ && ptr && *ptr) {
    ثابت حرف *کلید = ptr;
    ثابت حرف *قدر;

    جب(*ptr && *ptr != '=')
      ptr++;

    قدر = ptr + 1;

    جب(*ptr && *ptr != ';')
      ptr++;

    اگر(curl_strnequal(کلید, "AUTH=+LOGIN", 11)) {
      /* User prefers plaintext LOGIN over any SASL, including SASL LOGIN */
      prefer_login = TRUE;
      imapc->sasl.prefmech = SASL_AUTH_NONE;
    }
    ورنہ اگر(curl_strnequal(کلید, "AUTH=", 5)) {
      prefer_login = FALSE;
      نتیجہ = Curl_sasl_parse_url_auth_option(&imapc->sasl,
                                               قدر, ptr - قدر);
    }
    ورنہ {
      prefer_login = FALSE;
      نتیجہ = CURLE_URL_MALFORMAT;
    }

    اگر(*ptr == ';')
      ptr++;
  }

  اگر(prefer_login)
    imapc->preftype = IMAP_TYPE_CLEARTEXT;
  ورنہ {
    انتخاب(imapc->sasl.prefmech) {
    حالت SASL_AUTH_NONE:
      imapc->preftype = IMAP_TYPE_NONE;
      توڑو;
    حالت SASL_AUTH_DEFAULT:
      imapc->preftype = IMAP_TYPE_ANY;
      توڑو;
    طےشدہ:
      imapc->preftype = IMAP_TYPE_SASL;
      توڑو;
    }
  }

  واپس نتیجہ;
}

/***********************************************************************
 *
 * imap_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
ساکن CURLcode imap_parse_url_راستہ(ساخت Curl_easy *ڈیٹا,
                                    ساخت IMAP *imap)
{
  /* The imap struct is already initialized in imap_connect() */
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *begin = &ڈیٹا->state.اوپر.راستہ[1]; /* skip leading slash */
  ثابت حرف *ptr = begin;

  /* See how much of the URL is a valid path and decode it */
  جب(imap_is_bchar(*ptr))
    ptr++;

  اگر(ptr != begin) {
    /* Remove the trailing slash if present */
    ثابت حرف *انتہا = ptr;
    اگر(انتہا > begin && انتہا[-1] == '/')
      انتہا--;

    نتیجہ = Curl_urldecode(begin, انتہا - begin, &imap->mailbox, تہی,
                            REJECT_CTRL);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
  ورنہ
    imap->mailbox = تہی;

  /* There can be any number of parameters in the form ";NAME=VALUE" */
  جب(*ptr == ';') {
    حرف *name;
    حرف *قدر;
    size_t valuelen;

    /* Find the length of the name parameter */
    begin = ++ptr;
    جب(*ptr && *ptr != '=')
      ptr++;

    اگر(!*ptr)
      واپس CURLE_URL_MALFORMAT;

    /* Decode the name parameter */
    نتیجہ = Curl_urldecode(begin, ptr - begin, &name, تہی,
                            REJECT_CTRL);
    اگر(نتیجہ)
      واپس نتیجہ;

    /* Find the length of the value parameter */
    begin = ++ptr;
    جب(imap_is_bchar(*ptr))
      ptr++;

    /* Decode the value parameter */
    نتیجہ = Curl_urldecode(begin, ptr - begin, &قدر, &valuelen,
                            REJECT_CTRL);
    اگر(نتیجہ) {
      curlx_free(name);
      واپس نتیجہ;
    }

    DEBUGF(infof(ڈیٹا, "IMAP URL parameter '%s' = '%s'", name, قدر));

    /* Process the known hierarchical parameters (UIDVALIDITY, UID, SECTION
       and PARTIAL) stripping of the trailing slash character if it is
       present.

       Note: Unknown parameters trigger a URL_MALFORMAT error. */
    اگر(valuelen > 0 && قدر[valuelen - 1] == '/')
      قدر[valuelen - 1] = '\0';
    اگر(valuelen) {
      اگر(curl_strequal(name, "UIDVALIDITY") && !imap->uidvalidity_سیٹ) {
        curl_off_t عدد;
        ثابت حرف *p = (ثابت حرف *)قدر;
        اگر(!curlx_str_نمبر(&p, &عدد, UINT_MAX)) {
          imap->uidvalidity = (غیرعلامتی صحیح)عدد;
          imap->uidvalidity_سیٹ = TRUE;
        }
        curlx_free(قدر);
      }
      ورنہ اگر(curl_strequal(name, "UID") && !imap->uid) {
        imap->uid = قدر;
      }
      ورنہ اگر(curl_strequal(name, "MAILINDEX") && !imap->mindex) {
        imap->mindex = قدر;
      }
      ورنہ اگر(curl_strequal(name, "SECTION") && !imap->section) {
        imap->section = قدر;
      }
      ورنہ اگر(curl_strequal(name, "PARTIAL") && !imap->partial) {
        imap->partial = قدر;
      }
      ورنہ {
        curlx_free(name);
        curlx_free(قدر);
        واپس CURLE_URL_MALFORMAT;
      }
    }
    ورنہ
      /* blank? */
      curlx_free(قدر);
    curlx_free(name);
  }

  /* Does the URL contain a query parameter? Only valid when we have a mailbox
     and no UID as per RFC-5092 */
  اگر(imap->mailbox && !imap->uid && !imap->mindex) {
    /* Get the query parameter, URL decoded */
    CURLUcode uc = curl_url_get(ڈیٹا->state.uh, CURLUPART_QUERY, &imap->query,
                                CURLU_URLDECODE);
    اگر(uc == CURLUE_برآمد_کا_حافظہ)
      واپس CURLE_برآمد_کا_حافظہ;
  }

  /* Any extra stuff at the end of the URL is an error */
  اگر(*ptr)
    واپس CURLE_URL_MALFORMAT;

  واپس CURLE_OK;
}

/***********************************************************************
 *
 * imap_parse_custom_request()
 *
 * Parse the custom request.
 */
ساکن CURLcode imap_parse_custom_request(ساخت Curl_easy *ڈیٹا,
                                          ساخت IMAP *imap)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *custom = CURL_EASY_STR(ڈیٹا, STRING_CUSTOMREQUEST);

  اگر(custom) {
    /* URL decode the custom request */
    نتیجہ = Curl_urldecode(custom, 0, &imap->custom, تہی, REJECT_CTRL);

    /* Extract the parameters if specified */
    اگر(!نتیجہ) {
      ثابت حرف *params = imap->custom;

      جب(*params && *params != ' ')
        params++;

      اگر(*params) {
        imap->custom_params = curlx_strdup(params);
        imap->custom[params - imap->custom] = '\0';

        اگر(!imap->custom_params)
          نتیجہ = CURLE_برآمد_کا_حافظہ;
      }
    }
  }

  واپس نتیجہ;
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
ساکن CURLcode imap_connect(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  ساخت imap_conn *imapc =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_IMAP_CONN);
  CURLcode نتیجہ = CURLE_OK;

  *مکمل = FALSE; /* default to not done yet */
  اگر(!imapc)
    واپس CURLE_FAILED_INIT;

  /* Parse the URL options */
  نتیجہ = imap_parse_url_options(ڈیٹا->conn, imapc);
  اگر(نتیجہ)
    واپس نتیجہ;

  /* Start off waiting for the server greeting response */
  imap_state(ڈیٹا, imapc, IMAP_SERVERGREET);

  /* Start off with an response id of '*' */
  curlx_strcopy(imapc->resptag, حجم(imapc->resptag), STRCONST("*"));

  نتیجہ = imap_multi_statemach(ڈیٹا, مکمل);

  واپس نتیجہ;
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
ساکن CURLcode imap_مکمل(ساخت Curl_easy *ڈیٹا, CURLcode حالت_2,
                          bool premature)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ساخت IMAP *imap = Curl_meta_get(ڈیٹا, CURL_META_IMAP_EASY);

  اگر(!imapc)
    واپس CURLE_FAILED_INIT;
  اگر(!imap)
    واپس CURLE_OK;

  اگر(حالت_2 || premature) {
    CURL_TRC_M(ڈیٹا, "IMAP done, not finalizing transfer");
    connclose(conn); /* marked for closure */
    نتیجہ = حالت_2;         /* use the already set error code */
  }
  ورنہ اگر(!ڈیٹا->سیٹ.connect_only &&
          ((!imap->custom && (imap->uid || imap->mindex)) ||
           (imap->custom && ڈیٹا->req.maxdownload > 0) ||
           ڈیٹا->state.upload || IS_MIME_POST(ڈیٹا))) {
    /* Handle responses after FETCH or APPEND transfer has finished.
       For custom commands, check if we set up a download which indicates
       a FETCH-like command with literal data. */

    اگر(!ڈیٹا->state.upload && !IS_MIME_POST(ڈیٹا))
      imap_state(ڈیٹا, imapc, IMAP_FETCH_FINAL);
    ورنہ {
      /* End the APPEND command first by sending an empty line */
      نتیجہ = Curl_pp_sendf(ڈیٹا, &imapc->pp, "%s", "");
      اگر(!نتیجہ)
        imap_state(ڈیٹا, imapc, IMAP_APPEND_FINAL);
    }

    /* Run the state-machine */
    اگر(!نتیجہ)
      نتیجہ = imap_بلاک_statemach(ڈیٹا, imapc, FALSE);
  }

  imap_easy_reset(imap);
  واپس نتیجہ;
}

/***********************************************************************
 *
 * imap_perform()
 *
 * This is the actual DO function for IMAP. Fetch or append a message, or do
 * other things according to the options previously setup.
 */
ساکن CURLcode imap_perform(ساخت Curl_easy *ڈیٹا, bool *connected,
                             bool *dophase_مکمل)
{
  /* This is IMAP and no proxy */
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ساخت IMAP *imap = Curl_meta_get(ڈیٹا, CURL_META_IMAP_EASY);
  bool selected = FALSE;

  DEBUGF(infof(ڈیٹا, "DO phase starts"));
  اگر(!imapc || !imap)
    واپس CURLE_FAILED_INIT;

  اگر(ڈیٹا->req.no_body) {
    /* Requested no body means no transfer */
    imap->transfer = PPTRANSFER_INFO;
  }

  *dophase_مکمل = FALSE; /* not done yet */

  /* Determine if the requested mailbox (with the same UIDVALIDITY if set)
     has already been selected on this connection */
  اگر(imap->mailbox && imapc->mailbox &&
     (!strcmp(imap->mailbox, imapc->mailbox) ||
      (curl_strequal(imap->mailbox, "INBOX") &&
       curl_strequal(imapc->mailbox, "INBOX"))) &&
     (!imap->uidvalidity_سیٹ || !imapc->mb_uidvalidity_سیٹ ||
      (imap->uidvalidity == imapc->mb_uidvalidity)))
    selected = TRUE;

  /* Start the first command in the DO phase */
  اگر(ڈیٹا->state.upload || IS_MIME_POST(ڈیٹا))
    /* APPEND can be executed directly */
    نتیجہ = imap_perform_append(ڈیٹا, imapc, imap);
  ورنہ اگر(imap->custom && (selected || !imap->mailbox))
    /* Custom command using the same mailbox or no mailbox */
    نتیجہ = imap_perform_list(ڈیٹا, imapc, imap);
  ورنہ اگر(!imap->custom && selected && (imap->uid || imap->mindex))
    /* FETCH from the same mailbox */
    نتیجہ = imap_perform_fetch(ڈیٹا, imapc, imap);
  ورنہ اگر(!imap->custom && selected && imap->query)
    /* SEARCH the current mailbox */
    نتیجہ = imap_perform_تلاش(ڈیٹا, imapc, imap);
  ورنہ اگر(imap->mailbox && !selected &&
          (imap->custom || imap->uid || imap->mindex || imap->query))
    /* SELECT the mailbox */
    نتیجہ = imap_perform_select(ڈیٹا, imapc, imap);
  ورنہ
    /* LIST */
    نتیجہ = imap_perform_list(ڈیٹا, imapc, imap);

  اگر(نتیجہ)
    واپس نتیجہ;

  /* Run the state-machine */
  نتیجہ = imap_multi_statemach(ڈیٹا, dophase_مکمل);

  *connected = Curl_conn_is_connected(conn, FIRSTSOCKET);

  اگر(*dophase_مکمل)
    DEBUGF(infof(ڈیٹا, "DO phase is complete"));

  واپس نتیجہ;
}

/* Call this when the DO phase has completed */
ساکن CURLcode imap_dophase_مکمل(ساخت Curl_easy *ڈیٹا,
                                  ساخت IMAP *imap,
                                  bool connected)
{
  (خالی)connected;

  اگر(imap->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_تیاری_nop(ڈیٹا);

  واپس CURLE_OK;
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
ساکن CURLcode imap_regular_transfer(ساخت Curl_easy *ڈیٹا,
                                      ساخت IMAP *imap,
                                      bool *dophase_مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  ڈیٹا->req.سائز = -1;

  /* Set the progress data */
  Curl_pgrsReset(ڈیٹا);

  /* Carry out the perform */
  نتیجہ = imap_perform(ڈیٹا, &connected, dophase_مکمل);

  /* Perform post DO phase operations if necessary */
  اگر(!نتیجہ && *dophase_مکمل)
    نتیجہ = imap_dophase_مکمل(ڈیٹا, imap, connected);

  واپس نتیجہ;
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
ساکن CURLcode imap_do(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  ساخت IMAP *imap = Curl_meta_get(ڈیٹا, CURL_META_IMAP_EASY);
  CURLcode نتیجہ = CURLE_OK;
  *مکمل = FALSE; /* default to false */

  اگر(!imap)
    واپس CURLE_FAILED_INIT;
  /* Parse the URL path */
  نتیجہ = imap_parse_url_راستہ(ڈیٹا, imap);
  اگر(نتیجہ)
    واپس نتیجہ;

  /* Parse the custom request */
  نتیجہ = imap_parse_custom_request(ڈیٹا, imap);
  اگر(نتیجہ)
    واپس نتیجہ;

  نتیجہ = imap_regular_transfer(ڈیٹا, imap, مکمل);

  واپس نتیجہ;
}

/***********************************************************************
 *
 * imap_disconnect()
 *
 * Disconnect from an IMAP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ساکن CURLcode imap_disconnect(ساخت Curl_easy *ڈیٹا,
                                ساخت connectdata *conn, bool dead_connection)
{
  ساخت imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);

  اگر(imapc &&
     /* We cannot send quit unconditionally. If this connection is stale or
        bad in any way (pingpong has pending data to send),
        sending quit and waiting around here will make the
        disconnect wait in vain and cause more problems than we need to. */
     !dead_connection && conn->بٹ.protoconnstart &&
     !Curl_pp_needs_flush(ڈیٹا, &imapc->pp) &&
     !imap_perform_logout(ڈیٹا, imapc))
    (خالی)imap_بلاک_statemach(ڈیٹا, imapc, TRUE); /* ignore errors */

  واپس CURLE_OK;
}

/* Called from multi.c while DOing */
ساکن CURLcode imap_doing(ساخت Curl_easy *ڈیٹا, bool *dophase_مکمل)
{
  ساخت IMAP *imap = Curl_meta_get(ڈیٹا, CURL_META_IMAP_EASY);
  CURLcode نتیجہ;

  اگر(!imap)
    واپس CURLE_FAILED_INIT;

  نتیجہ = imap_multi_statemach(ڈیٹا, dophase_مکمل);
  اگر(نتیجہ)
    DEBUGF(infof(ڈیٹا, "DO phase failed"));
  ورنہ اگر(*dophase_مکمل) {
    نتیجہ = imap_dophase_مکمل(ڈیٹا, imap, FALSE /* not connected */);

    DEBUGF(infof(ڈیٹا, "DO phase is complete"));
  }

  واپس نتیجہ;
}

ساکن خالی imap_easy_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت IMAP *imap = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  imap_easy_reset(imap);
  curlx_free(imap);
}

ساکن خالی imap_conn_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت imap_conn *imapc = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  Curl_pp_disconnect(&imapc->pp);
  curlx_safefree(imapc->mailbox);
  curlx_free(imapc);
}

/* SASL parameters for the imap protocol */
ساکن ثابت ساخت SASLproto saslimap = {
  "imap",                     /* The service name */
  imap_perform_authenticate,  /* Send authentication command */
  imap_continue_authenticate, /* Send authentication continuation */
  imap_cancel_authenticate,   /* Send authentication cancellation */
  imap_get_پیغام,           /* Get SASL response message */
  0,                          /* No maximum initial response length */
  '+',                        /* Code received when continuation is expected */
  IMAP_RESP_OK,               /* Code to receive upon authentication success */
  SASL_AUTH_DEFAULT,          /* Default mechanisms */
  SASL_FLAG_BASE64            /* Configuration flags */
};

ساکن CURLcode imap_تیاری_connection(ساخت Curl_easy *ڈیٹا,
                                      ساخت connectdata *conn)
{
  ساخت imap_conn *imapc;
  ساخت pingpong *pp;
  ساخت IMAP *imap;

  imapc = curlx_calloc(1, حجم(*imapc));
  اگر(!imapc)
    واپس CURLE_برآمد_کا_حافظہ;

  pp = &imapc->pp;
  PINGPONG_تیاری(pp, imap_pp_statemachine, imap_endofresp);

  /* Set the default preferred authentication type and mechanism */
  imapc->preftype = IMAP_TYPE_ANY;
  Curl_sasl_init(&imapc->sasl, ڈیٹا, &saslimap);

  Curl_pp_init(pp, Curl_pgrs_now(ڈیٹا));

  اگر(Curl_conn_meta_سیٹ(conn, CURL_META_IMAP_CONN, imapc, imap_conn_dtor))
    واپس CURLE_برآمد_کا_حافظہ;

  imap = curlx_calloc(1, حجم(ساخت IMAP));
  اگر(!imap ||
     Curl_meta_سیٹ(ڈیٹا, CURL_META_IMAP_EASY, imap, imap_easy_dtor))
    واپس CURLE_برآمد_کا_حافظہ;

  واپس CURLE_OK;
}

/*
 * IMAP protocol.
 */
ثابت ساخت Curl_protocol Curl_protocol_imap = {
  imap_تیاری_connection,            /* setup_connection */
  imap_do,                          /* do_it */
  imap_مکمل,                        /* done */
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

#اگر_ختم /* CURL_DISABLE_IMAP */
