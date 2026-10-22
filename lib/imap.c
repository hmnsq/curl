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

#ifndef CURL_DISABLE_IMAP

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef __VMS
#شامل <in.h>
#شامل <inet.h>
#endif

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
#define CURL_META_IMAP_EASY   "meta:proto:imap:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_IMAP_CONN   "meta:proto:imap:conn"

تعریفنوع شمارش {
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
  IMAP_LAST          /* never used */
} imapstate;

/* imap_conn is used for struct connection-oriented data */
ساختار imap_conn {
  ساختار pingpong pp;
  ساختار SASL sasl;           /* SASL-related parameters */
  نویسه *mailbox;              /* The last selected mailbox */
  imapstate state;            /* Always use imap.c:state() to change state! */
  unsigned صحیح mb_uidvalidity; /* UIDVALIDITY parsed from select response */
  نویسه resptag[5];            /* Response tag to wait for */
  unsigned نویسه preftype;     /* Preferred authentication type */
  unsigned نویسه cmdid;        /* Last used command ID */
  BIT(ssldone);               /* Is connect() over SSL done? */
  BIT(preauth);               /* Is this connection PREAUTH? */
  BIT(tls_supported);         /* StartTLS capability supported by server */
  BIT(login_disabled);        /* LOGIN command disabled by server */
  BIT(ir_supported);          /* Initial response supported by server */
  BIT(mb_uidvalidity_set);
};

/* This IMAP struct is used in the Curl_easy. All IMAP data that is
   connection-oriented must be in imap_conn to properly deal with the fact that
   perhaps the Curl_easy is changed between the times the connection is
   used. */
ساختار IMAP {
  curl_pp_transfer transfer;
  نویسه *mailbox;          /* Mailbox to select */
  نویسه *uid;              /* Message UID to fetch */
  نویسه *mindex;           /* Index in mail box of mail to fetch */
  نویسه *section;          /* Message SECTION to fetch */
  نویسه *partial;          /* Message PARTIAL to fetch */
  نویسه *query;            /* Query to search for */
  نویسه *custom;           /* Custom request */
  نویسه *custom_params;    /* Parameters for the custom request */
  unsigned صحیح uidvalidity; /* UIDVALIDITY to check in select */
  BIT(uidvalidity_set);
};

#define IMAP_RESP_OK       1
#define IMAP_RESP_NOT_OK   2
#define IMAP_RESP_PREAUTH  3

ساختار ulbits {
  صحیح bit;
  ثابت نویسه *flag;
};

/***********************************************************************
 *
 * imap_sendf()
 *
 * Sends the formatted string as an IMAP command to the server.
 *
 * Designed to never block.
 */
ایستا CURLcode imap_sendf(ساختار Curl_easy *data,
                           ساختار imap_conn *imapc,
                           ثابت نویسه *fmt, ...) CURL_PRINTF(3, 4);
ایستا CURLcode imap_sendf(ساختار Curl_easy *data,
                           ساختار imap_conn *imapc,
                           ثابت نویسه *fmt, ...)
{
  CURLcode result = CURLE_OK;
  va_list ap;

  DEBUGASSERT(fmt);

  /* Calculate the tag based on the connection ID and command ID */
  curl_msnprintf(imapc->resptag, اندازه(imapc->resptag), "%c%03d",
                 'A' + curlx_sltosi((long)(data->conn->connection_id % 26)),
                 ++imapc->cmdid);

  va_start(ap, fmt);
  result = Curl_pp_vsendf(data, &imapc->pp, imapc->resptag, fmt, ap);
  va_end(ap);
  بازگشت result;
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
ایستا نویسه *imap_atom(ثابت نویسه *str, bool escape_only)
{
  ساختار dynbuf سطر;
  size_t nclean;
  size_t len;

  اگر(!str)
    بازگشت پوچ;

  len = strlen(str);
  nclean = strcspn(str, "() {%*]\\\"");
  اگر(len == nclean)
    /* nothing to escape, return a strdup */
    بازگشت curlx_strdup(str);

  curlx_dyn_init(&سطر, 2000);

  اگر(!escape_only && curlx_dyn_addn(&سطر, "\"", 1))
    بازگشت پوچ;

  وقتی(*str) {
    اگر((*str == '\\' || *str == '"') &&
       curlx_dyn_addn(&سطر, "\\", 1))
      بازگشت پوچ;
    اگر(curlx_dyn_addn(&سطر, str, 1))
      بازگشت پوچ;
    str++;
  }

  اگر(!escape_only && curlx_dyn_addn(&سطر, "\"", 1))
    بازگشت پوچ;

  بازگشت curlx_dyn_ptr(&سطر);
}

/*
 * Finds the start of a literal '{size}' in line, skipping over quoted strings.
 */
ایستا ثابت نویسه *imap_find_literal(ثابت نویسه *سطر, size_t len)
{
  ثابت نویسه *end = سطر + len;
  bool in_quote = FALSE;

  وقتی(سطر < end) {
    اگر(in_quote) {
      اگر(*سطر == '\\' && (سطر + 1) < end) {
        سطر += 2;
        ادامه;
      }
      اگر(*سطر == '"')
        in_quote = FALSE;
    }
    وگرنه {
      اگر(*سطر == '"')
        in_quote = TRUE;
      وگرنه اگر(*سطر == '{')
        بازگشت سطر;
    }
    سطر++;
  }
  بازگشت پوچ;
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
ایستا bool imap_matchresp(ثابت نویسه *سطر, size_t len, ثابت نویسه *cmd)
{
  ثابت نویسه *end = سطر + len;
  size_t cmd_len = strlen(cmd);

  /* Skip the untagged response marker */
  سطر += 2;

  /* Do we have a number after the marker? */
  اگر(سطر < end && ISDIGIT(*سطر)) {
    /* Skip the number */
    انجامبده
      سطر++;
    وقتی(سطر < end && ISDIGIT(*سطر));

    /* Do we have the space character? */
    اگر(سطر == end || *سطر != ' ')
      بازگشت FALSE;

    سطر++;
  }

  /* Does the command name match and is it followed by a space character or at
     the end of line? */
  اگر(سطر + cmd_len <= end && curl_strnequal(سطر, cmd, cmd_len) &&
     (سطر[cmd_len] == ' ' || سطر + cmd_len + 2 == end))
    بازگشت TRUE;

  بازگشت FALSE;
}

/***********************************************************************
 *
 * imap_endofresp()
 *
 * Checks whether the given string is a valid tagged, untagged or continuation
 * response which can be processed by the response handler.
 */
ایستا bool imap_endofresp(ساختار Curl_easy *data, ساختار connectdata *conn,
                           ثابت نویسه *سطر, size_t len, صحیح *resp)
{
  ساختار imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ساختار IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  ثابت نویسه *id;
  size_t id_len;

  DEBUGASSERT(imapc);
  DEBUGASSERT(imap);
  اگر(!imapc || !imap)
    بازگشت FALSE;

  /* Do we have a tagged command response? */
  id = imapc->resptag;
  id_len = strlen(id);
  اگر(len >= id_len + 1 && !memcmp(id, سطر, id_len) && سطر[id_len] == ' ') {
    سطر += id_len + 1;
    len -= id_len + 1;

    اگر(len >= 2 && !memcmp(سطر, "OK", 2))
      *resp = IMAP_RESP_OK;
    وگرنه اگر(len >= 7 && !memcmp(سطر, "PREAUTH", 7))
      *resp = IMAP_RESP_PREAUTH;
    وگرنه
      *resp = IMAP_RESP_NOT_OK;

    بازگشت TRUE;
  }

  /* Do we have an untagged command response? */
  اگر(len >= 2 && !memcmp("* ", سطر, 2)) {
    انتخاب(imapc->state) {
    /* States which are interested in untagged responses */
    حالت IMAP_CAPABILITY:
      اگر(!imap_matchresp(سطر, len, "CAPABILITY"))
        بازگشت FALSE;
      بشکن;

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
        بازگشت FALSE;
      بشکن;

    حالت IMAP_SELECT:
      /* SELECT is special in that its untagged responses do not have a
         common prefix so accept anything! */
      بشکن;

    حالت IMAP_FETCH:
      اگر(!imap_matchresp(سطر, len, "FETCH"))
        بازگشت FALSE;
      بشکن;

    حالت IMAP_جستجو:
      اگر(!imap_matchresp(سطر, len, "SEARCH"))
        بازگشت FALSE;
      بشکن;

    /* Ignore other untagged responses */
    پیشفرض:
      بازگشت FALSE;
    }

    *resp = '*';
    بازگشت TRUE;
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
      بشکن;

    پیشفرض:
      failf(data, "Unexpected continuation response");
      *resp = -1;
      بشکن;
    }

    بازگشت TRUE;
  }

  بازگشت FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * imap_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ایستا CURLcode imap_get_message(ساختار Curl_easy *data, ساختار bufref *out)
{
  ساختار imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  نویسه *message;
  size_t len;

  اگر(!imapc)
    بازگشت CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&imapc->pp.recvbuf);
  len = imapc->pp.nfinal;
  اگر(len > 2) {
    /* Find the start of the message */
    len -= 2;
    برای(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    وقتی(len--)
      اگر(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        بشکن;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, پوچ);
  }
  وگرنه
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, پوچ);

  بازگشت CURLE_OK;
}

/***********************************************************************
 *
 * imap_state()
 *
 * This is the ONLY way to change IMAP state!
 */
ایستا تهی imap_state(ساختار Curl_easy *data,
                       ساختار imap_conn *imapc,
                       imapstate newstate)
{
#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ایستا ثابت نویسه * ثابت names[] = {
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
    /* LAST */
  };

  اگر(imapc->state != newstate)
    infof(data, "IMAP %p state change from %s to %s",
          (تهی *)imapc, names[imapc->state], names[newstate]);
#وگرنه
  (تهی)data;
#endif
  imapc->state = newstate;
}

/***********************************************************************
 *
 * imap_perform_capability()
 *
 * Sends the CAPABILITY command in order to obtain a list of server side
 * supported capabilities.
 */
ایستا CURLcode imap_perform_capability(ساختار Curl_easy *data,
                                        ساختار imap_conn *imapc)
{
  CURLcode result = CURLE_OK;

  imapc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  imapc->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  imapc->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPABILITY command */
  result = imap_sendf(data, imapc, "CAPABILITY");

  اگر(!result)
    imap_state(data, imapc, IMAP_CAPABILITY);

  بازگشت result;
}

/***********************************************************************
 *
 * imap_perform_starttls()
 *
 * Sends the STARTTLS command to start the upgrade to TLS.
 */
ایستا CURLcode imap_perform_starttls(ساختار Curl_easy *data,
                                      ساختار imap_conn *imapc)
{
  /* Send the STARTTLS command */
  CURLcode result = imap_sendf(data, imapc, "STARTTLS");

  اگر(!result)
    imap_state(data, imapc, IMAP_STARTTLS);

  بازگشت result;
}

/***********************************************************************
 *
 * imap_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ایستا CURLcode imap_perform_upgrade_tls(ساختار Curl_easy *data,
                                         ساختار imap_conn *imapc,
                                         ساختار connectdata *conn)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  CURLcode result;
  bool ssldone = FALSE;

  اگر(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    اگر(result)
      برو out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_imaps;
  }

  DEBUGASSERT(!imapc->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "imap_perform_upgrade_tls, connect -> %d, %d",
               (صحیح)result, ssldone));
  اگر(!result && ssldone) {
    imapc->ssldone = ssldone;
    /* perform CAPA now, changes imapc->state out of IMAP_UPGRADETLS */
    result = imap_perform_capability(data, imapc);
  }
out:
  بازگشت result;
#وگرنه
  (تهی)data;
  (تهی)imapc;
  (تهی)conn;
  بازگشت CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * imap_perform_login()
 *
 * Sends a clear text LOGIN command to authenticate with.
 */
ایستا CURLcode imap_perform_login(ساختار Curl_easy *data,
                                   ساختار imap_conn *imapc,
                                   ساختار connectdata *conn)
{
  CURLcode result = CURLE_OK;
  نویسه *user;
  نویسه *passwd;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  اگر(!conn->creds) {
    imap_state(data, imapc, IMAP_STOP);

    بازگشت result;
  }

  /* Make sure the username and password are in the correct atom format */
  user = imap_atom(Curl_creds_user(conn->creds), FALSE);
  passwd = imap_atom(Curl_creds_passwd(conn->creds), FALSE);

  /* Send the LOGIN command */
  result = imap_sendf(data, imapc, "LOGIN %s %s", user ? user : "",
                      passwd ? passwd : "");

  curlx_free(user);
  curlx_strzero(passwd);
  curlx_free(passwd);

  اگر(!result)
    imap_state(data, imapc, IMAP_LOGIN);

  بازگشت result;
}

/***********************************************************************
 *
 * imap_perform_authenticate()
 *
 * Sends an AUTHENTICATE command allowing the client to login with the given
 * SASL authentication mechanism.
 */
ایستا CURLcode imap_perform_authenticate(ساختار Curl_easy *data,
                                          ثابت نویسه *mech,
                                          ثابت ساختار bufref *initresp)
{
  ساختار imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  CURLcode result = CURLE_OK;
  ثابت نویسه *ir = Curl_bufref_ptr(initresp);

  اگر(!imapc)
    بازگشت CURLE_FAILED_INIT;
  اگر(ir) {
    /* Send the AUTHENTICATE command with the initial response */
    result = imap_sendf(data, imapc, "AUTHENTICATE %s %s",
                        mech, *ir ? ir : "=");
  }
  وگرنه {
    /* Send the AUTHENTICATE command */
    result = imap_sendf(data, imapc, "AUTHENTICATE %s", mech);
  }

  بازگشت result;
}

/***********************************************************************
 *
 * imap_continue_authenticate()
 *
 * Sends SASL continuation data.
 */
ایستا CURLcode imap_continue_authenticate(ساختار Curl_easy *data,
                                           ثابت نویسه *mech,
                                           ثابت ساختار bufref *resp)
{
  ساختار imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  (تهی)mech;
  اگر(!imapc)
    بازگشت CURLE_FAILED_INIT;
  بازگشت Curl_pp_sendf(data, &imapc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * imap_cancel_authenticate()
 *
 * Sends SASL cancellation.
 */
ایستا CURLcode imap_cancel_authenticate(ساختار Curl_easy *data,
                                         ثابت نویسه *mech)
{
  ساختار imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  (تهی)mech;
  اگر(!imapc)
    بازگشت CURLE_FAILED_INIT;
  بازگشت Curl_pp_sendf(data, &imapc->pp, "*");
}

/***********************************************************************
 *
 * imap_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to clear text should a common
 * mechanism not be available between the client and server.
 */
ایستا CURLcode imap_perform_authentication(ساختار Curl_easy *data,
                                            ساختار imap_conn *imapc)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  /* Check if already authenticated OR if there is enough data to authenticate
     with and end the connect phase if we do not */
  اگر(imapc->preauth ||
     !Curl_sasl_can_authenticate(&imapc->sasl, data)) {
    imap_state(data, imapc, IMAP_STOP);
    بازگشت result;
  }

  /* Calculate the SASL login details */
  result = Curl_sasl_start(&imapc->sasl, data, (bool)imapc->ir_supported,
                           &progress);
  اگر(!result) {
    اگر(progress == SASL_INPROGRESS)
      imap_state(data, imapc, IMAP_AUTHENTICATE);
    وگرنه اگر(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
      /* Perform clear text authentication */
      result = imap_perform_login(data, imapc, data->conn);
    وگرنه
      result = Curl_sasl_is_blocked(&imapc->sasl, data);
  }

  بازگشت result;
}

/***********************************************************************
 *
 * imap_perform_list()
 *
 * Sends a LIST command or an alternative custom request.
 */
ایستا CURLcode imap_perform_list(ساختار Curl_easy *data,
                                  ساختار imap_conn *imapc,
                                  ساختار IMAP *imap)
{
  CURLcode result = CURLE_OK;

  اگر(imap->custom)
    /* Send the custom request */
    result = imap_sendf(data, imapc, "%s%s", imap->custom,
                        imap->custom_params ? imap->custom_params : "");
  وگرنه {
    /* Make sure the mailbox is in the correct atom format if necessary */
    نویسه *mailbox = imap->mailbox ? imap_atom(imap->mailbox, TRUE)
                                  : curlx_strdup("");
    اگر(!mailbox)
      بازگشت CURLE_OUT_OF_MEMORY;

    /* Send the LIST command */
    result = imap_sendf(data, imapc, "LIST \"%s\" *", mailbox);

    curlx_free(mailbox);
  }

  اگر(!result)
    imap_state(data, imapc, IMAP_LIST);

  بازگشت result;
}

/***********************************************************************
 *
 * imap_perform_select()
 *
 * Sends a SELECT command to ask the server to change the selected mailbox.
 */
ایستا CURLcode imap_perform_select(ساختار Curl_easy *data,
                                    ساختار imap_conn *imapc,
                                    ساختار IMAP *imap)
{
  CURLcode result = CURLE_OK;
  نویسه *mailbox;

  /* Invalidate old information as we are switching mailboxes */
  curlx_safefree(imapc->mailbox);
  imapc->mb_uidvalidity_set = FALSE;

  /* Check we have a mailbox */
  اگر(!imap->mailbox) {
    failf(data, "Cannot SELECT without a mailbox.");
    بازگشت CURLE_URL_MALFORMAT;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  اگر(!mailbox)
    بازگشت CURLE_OUT_OF_MEMORY;

  /* Send the SELECT command */
  result = imap_sendf(data, imapc, "SELECT %s", mailbox);

  curlx_free(mailbox);

  اگر(!result)
    imap_state(data, imapc, IMAP_SELECT);

  بازگشت result;
}

/***********************************************************************
 *
 * imap_perform_fetch()
 *
 * Sends a FETCH command to initiate the download of a message.
 */
ایستا CURLcode imap_perform_fetch(ساختار Curl_easy *data,
                                   ساختار imap_conn *imapc,
                                   ساختار IMAP *imap)
{
  CURLcode result = CURLE_OK;
  /* Check we have a UID */
  اگر(imap->uid) {

    /* Send the FETCH command */
    اگر(imap->partial)
      result = imap_sendf(data, imapc, "UID FETCH %s BODY[%s]<%s>",
                          imap->uid, imap->section ? imap->section : "",
                          imap->partial);
    وگرنه
      result = imap_sendf(data, imapc, "UID FETCH %s BODY[%s]",
                          imap->uid, imap->section ? imap->section : "");
  }
  وگرنه اگر(imap->mindex) {
    /* Send the FETCH command */
    اگر(imap->partial)
      result = imap_sendf(data, imapc, "FETCH %s BODY[%s]<%s>",
                          imap->mindex, imap->section ? imap->section : "",
                          imap->partial);
    وگرنه
      result = imap_sendf(data, imapc, "FETCH %s BODY[%s]",
                          imap->mindex, imap->section ? imap->section : "");
  }
  وگرنه {
    failf(data, "Cannot FETCH without a UID.");
    بازگشت CURLE_URL_MALFORMAT;
  }
  اگر(!result)
    imap_state(data, imapc, IMAP_FETCH);

  بازگشت result;
}

/***********************************************************************
 *
 * imap_perform_append()
 *
 * Sends an APPEND command to initiate the upload of a message.
 */
ایستا CURLcode imap_perform_append(ساختار Curl_easy *data,
                                    ساختار imap_conn *imapc,
                                    ساختار IMAP *imap)
{
  CURLcode result = CURLE_OK;
  نویسه *mailbox;
  ساختار dynbuf flags;

  /* Check we have a mailbox */
  اگر(!imap->mailbox) {
    failf(data, "Cannot APPEND without a mailbox.");
    بازگشت CURLE_URL_MALFORMAT;
  }

#ifndef CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  اگر(IS_MIME_POST(data)) {
    curl_mimepart *postp = data->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(unsigned صحیح)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, data->set.headers, 0);
    result = Curl_mime_prepare_headers(data, postp, پوچ,
                                       پوچ, MIMESTRATEGY_MAIL);

    اگر(!result && !Curl_checkheaders(data, STRCONST("Mime-Version")))
      result = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    اگر(!result)
      result = Curl_creader_set_mime(data, postp);
    اگر(result)
      بازگشت result;
  }
  وگرنه
#endif
  {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    اگر(result)
      بازگشت result;
  }

  /* Check we know the size of the upload. This takes all readers
   * into account. Especially crlf conversions which make the size
   * unpredictable, e.g. -1. */
  data->state.infilesize = Curl_creader_total_length(data);
  اگر(data->state.infilesize < 0) {
    اگر(data->set.crlf)
      failf(data, "Cannot APPEND with CRLF conversion making size unknown");
    وگرنه
      failf(data, "Cannot APPEND with unknown input file size");
    بازگشت CURLE_UPLOAD_FAILED;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  اگر(!mailbox)
    بازگشت CURLE_OUT_OF_MEMORY;

  /* Generate flags string and send the APPEND command */
  curlx_dyn_init(&flags, 100);
  اگر(data->set.upload_flags) {
    صحیح i;
    ساختار ulbits ulflag[] = {
      { CURLULFLAG_ANSWERED, "Answered" },
      { CURLULFLAG_DELETED, "Deleted" },
      { CURLULFLAG_DRAFT, "Draft" },
      { CURLULFLAG_FLAGGED, "Flagged" },
      { CURLULFLAG_SEEN, "Seen" },
      { 0, پوچ }
    };

    result = CURLE_OUT_OF_MEMORY;
    اگر(curlx_dyn_add(&flags, " (")) {
      برو cleanup;
    }

    برای(i = 0; ulflag[i].bit; i++) {
      اگر(data->set.upload_flags & ulflag[i].bit &&
         ((curlx_dyn_len(&flags) > 2 && curlx_dyn_add(&flags, " ")) ||
          curlx_dyn_add(&flags, "\\") ||
          curlx_dyn_add(&flags, ulflag[i].flag)))
        برو cleanup;
    }

    اگر(curlx_dyn_add(&flags, ")"))
      برو cleanup;
  }
  وگرنه اگر(curlx_dyn_add(&flags, ""))
    برو cleanup;

  result = imap_sendf(data, imapc, "APPEND %s%s {%" FMT_OFF_T "}",
                      mailbox, curlx_dyn_ptr(&flags), data->state.infilesize);

cleanup:
  curlx_dyn_free(&flags);
  curlx_free(mailbox);

  اگر(!result)
    imap_state(data, imapc, IMAP_APPEND);

  بازگشت result;
}

/***********************************************************************
 *
 * imap_perform_search()
 *
 * Sends a SEARCH command.
 */
ایستا CURLcode imap_perform_جستجو(ساختار Curl_easy *data,
                                    ساختار imap_conn *imapc,
                                    ساختار IMAP *imap)
{
  CURLcode result = CURLE_OK;

  /* Check we have a query string */
  اگر(!imap->query) {
    failf(data, "Cannot SEARCH without a query string.");
    بازگشت CURLE_URL_MALFORMAT;
  }

  /* Send the SEARCH command */
  result = imap_sendf(data, imapc, "SEARCH %s", imap->query);

  اگر(!result)
    imap_state(data, imapc, IMAP_جستجو);

  بازگشت result;
}

/***********************************************************************
 *
 * imap_perform_logout()
 *
 * Performs the logout action prior to sclose() being called.
 */
ایستا CURLcode imap_perform_logout(ساختار Curl_easy *data,
                                    ساختار imap_conn *imapc)
{
  /* Send the LOGOUT command */
  CURLcode result = imap_sendf(data, imapc, "LOGOUT");

  اگر(!result)
    imap_state(data, imapc, IMAP_LOGOUT);

  بازگشت result;
}

/* For the initial server greeting */
ایستا CURLcode imap_state_servergreet_resp(ساختار Curl_easy *data,
                                            ساختار imap_conn *imapc,
                                            صحیح imapcode,
                                            imapstate instate)
{
  (تهی)instate;

  اگر(imapcode == IMAP_RESP_PREAUTH) {
    /* PREAUTH */
    imapc->preauth = TRUE;
    infof(data, "PREAUTH connection, already authenticated");
  }
  وگرنه اگر(imapcode != IMAP_RESP_OK) {
    failf(data, "Got unexpected imap-server response");
    بازگشت CURLE_WEIRD_SERVER_REPLY;
  }

  بازگشت imap_perform_capability(data, imapc);
}

/* For CAPABILITY responses */
ایستا CURLcode imap_state_capability_resp(ساختار Curl_easy *data,
                                           ساختار imap_conn *imapc,
                                           صحیح imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  ثابت نویسه *سطر = curlx_dyn_ptr(&imapc->pp.recvbuf);

  (تهی)instate;

  /* Do we have an untagged response? */
  اگر(imapcode == '*') {
    سطر += 2;

    /* Loop through the data line */
    برای(;;) {
      size_t wordlen;
      وقتی(*سطر && (ISBLANK(*سطر) || ISNEWLINE(*سطر)))
        سطر++;

      اگر(!*سطر)
        بشکن;

      /* Extract the word */
      برای(wordlen = 0; سطر[wordlen] && !ISBLANK(سطر[wordlen]) &&
                       !ISNEWLINE(سطر[wordlen]);)
        wordlen++;

      /* Does the server support the STARTTLS capability? */
      اگر(wordlen == 8 && curl_strnequal(سطر, "STARTTLS", 8))
        imapc->tls_supported = TRUE;

      /* Has the server explicitly disabled clear text authentication? */
      وگرنه اگر(wordlen == 13 && curl_strnequal(سطر, "LOGINDISABLED", 13))
        imapc->login_disabled = TRUE;

      /* Does the server support the SASL-IR capability? */
      وگرنه اگر(wordlen == 7 && curl_strnequal(سطر, "SASL-IR", 7))
        imapc->ir_supported = TRUE;

      /* Do we have a SASL based authentication mechanism? */
      وگرنه اگر(wordlen > 5 && curl_strnequal(سطر, "AUTH=", 5)) {
        size_t llen;
        unsigned short mechbit;

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
  وگرنه اگر(data->set.use_ssl && !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
    /* PREAUTH is not compatible with STARTTLS. */
    اگر(imapcode == IMAP_RESP_OK && imapc->tls_supported && !imapc->preauth) {
      /* Switch to TLS connection now */
      result = imap_perform_starttls(data, imapc);
    }
    وگرنه اگر(data->set.use_ssl <= CURLUSESSL_TRY)
      result = imap_perform_authentication(data, imapc);
    وگرنه {
      failf(data, "STARTTLS not available.");
      result = CURLE_USE_SSL_FAILED;
    }
  }
  وگرنه
    result = imap_perform_authentication(data, imapc);

  بازگشت result;
}

/* For STARTTLS responses */
ایستا CURLcode imap_state_starttls_resp(ساختار Curl_easy *data,
                                         ساختار imap_conn *imapc,
                                         صحیح imapcode,
                                         imapstate instate)
{
  CURLcode result = CURLE_OK;

  (تهی)instate;

  /* Pipelining in response is forbidden. */
  اگر(imapc->pp.overflow)
    بازگشت CURLE_WEIRD_SERVER_REPLY;

  اگر(imapcode != IMAP_RESP_OK) {
    اگر(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied");
      result = CURLE_USE_SSL_FAILED;
    }
    وگرنه
      result = imap_perform_authentication(data, imapc);
  }
  وگرنه
    imap_state(data, imapc, IMAP_UPGRADETLS);

  بازگشت result;
}

/* For SASL authentication responses */
ایستا CURLcode imap_state_auth_resp(ساختار Curl_easy *data,
                                     ساختار imap_conn *imapc,
                                     صحیح imapcode,
                                     imapstate instate)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  (تهی)instate;

  result = Curl_sasl_continue(&imapc->sasl, data, imapcode, &progress);
  اگر(!result)
    انتخاب(progress) {
    حالت SASL_DONE:
      imap_state(data, imapc, IMAP_STOP);  /* Authenticated */
      بشکن;
    حالت SASL_IDLE:            /* No mechanism left after cancellation */
      اگر(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
        /* Perform clear text authentication */
        result = imap_perform_login(data, imapc, data->conn);
      وگرنه {
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
      }
      بشکن;
    پیشفرض:
      بشکن;
    }

  بازگشت result;
}

/* For LOGIN responses */
ایستا CURLcode imap_state_login_resp(ساختار Curl_easy *data,
                                      ساختار imap_conn *imapc,
                                      صحیح imapcode,
                                      imapstate instate)
{
  CURLcode result = CURLE_OK;
  (تهی)instate;

  اگر(imapcode != IMAP_RESP_OK) {
    failf(data, "Access denied. %c", imapcode);
    result = CURLE_LOGIN_DENIED;
  }
  وگرنه
    /* End of connect phase */
    imap_state(data, imapc, IMAP_STOP);

  بازگشت result;
}

/* Detect IMAP listings vs. downloading a single email */
ایستا bool is_custom_fetch_listing_match(ثابت نویسه *params)
{
  /* match " 1:* (FLAGS ..." or " 1,2,3 (FLAGS ..." */
  اگر(*params++ != ' ')
    بازگشت FALSE;

  وقتی(ISDIGIT(*params)) {
    params++;
    اگر(*params == 0)
      بازگشت FALSE;
  }
  اگر(*params == ':')
    بازگشت TRUE;
  اگر(*params == ',')
    بازگشت TRUE;
  بازگشت FALSE;
}

ایستا bool is_custom_fetch_listing(ساختار IMAP *imap)
{
  /* filter out "UID FETCH 1:* (FLAGS ..." queries to list emails */
  اگر(!imap->custom)
    بازگشت FALSE;
  وگرنه اگر(curl_strequal(imap->custom, "FETCH") && imap->custom_params) {
    ثابت نویسه *p = imap->custom_params;
    بازگشت is_custom_fetch_listing_match(p);
  }
  وگرنه اگر(curl_strequal(imap->custom, "UID") && imap->custom_params &&
          curl_strnequal(imap->custom_params, " FETCH ", 7)) {
    ثابت نویسه *p = imap->custom_params + 6;
    بازگشت is_custom_fetch_listing_match(p);
  }
  بازگشت FALSE;
}

/* For LIST and SEARCH responses */
ایستا CURLcode imap_state_listsearch_resp(ساختار Curl_easy *data,
                                           ساختار imap_conn *imapc,
                                           صحیح imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  ثابت نویسه *سطر = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  ساختار IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);

  DEBUGASSERT(imap);
  اگر(!imap)
    بازگشت CURLE_FAILED_INIT;
  (تهی)instate;

  اگر(imapcode == '*' && is_custom_fetch_listing(imap)) {
    /* custom FETCH or UID FETCH for listing is not handled here */
  }
  وگرنه اگر(imapcode == '*') {
    /* Check if this response contains a literal (e.g. FETCH responses with
       body data). Literal syntax is {size}\r\n */
    ثابت نویسه *cr = memchr(سطر, '\r', len);
    size_t سطر_len = cr ? (size_t)(cr - سطر) : len;
    ثابت نویسه *ptr = imap_find_literal(سطر, سطر_len);
    اگر(ptr) {
      curl_off_t size = 0;
      bool parsed = FALSE;
      ptr++;
      اگر(!curlx_str_عدد(&ptr, &size, CURL_OFF_T_MAX) &&
         !curlx_str_single(&ptr, '}'))
        parsed = TRUE;

      اگر(parsed) {
        ساختار pingpong *pp = &imapc->pp;
        size_t buffer_len = curlx_dyn_len(&pp->recvbuf);
        size_t after_header = buffer_len - pp->nfinal;

        /* This is a literal response, setup to receive the body data */
        infof(data, "Found %" FMT_OFF_T " bytes to download", size);

        /* First write the header line */
        result = Curl_client_write(data, CLIENTWRITE_BODY, سطر, len);
        اگر(result)
          بازگشت result;

        /* Handle data already in buffer after the header line */
        اگر(after_header > 0) {
          /* There is already data in the buffer that is part of the literal
             body or subsequent responses */
          size_t chunk = after_header;

          /* Keep only the data after the header line */
          curlx_dyn_tail(&pp->recvbuf, chunk);
          pp->nfinal = 0; /* done */

          /* Limit chunk to the literal size */
          اگر((curl_off_t)chunk > size)
            chunk = (size_t)size;

          اگر(chunk) {
            /* Write the literal body data */
            result = Curl_client_write(data, CLIENTWRITE_BODY,
                                       curlx_dyn_ptr(&pp->recvbuf), chunk);
            اگر(result)
              بازگشت result;
          }

          /* Handle remaining data in buffer (either more literal data or
             subsequent responses) */
          اگر(after_header > chunk) {
            /* Keep the data after the literal body */
            pp->overflow = after_header - chunk;
            curlx_dyn_tail(&pp->recvbuf, pp->overflow);
          }
          وگرنه {
            pp->overflow = 0;
            curlx_dyn_reset(&pp->recvbuf);
          }
        }
        وگرنه {
          /* No data in buffer yet, reset overflow */
          pp->overflow = 0;
        }

        اگر((CURL_OFF_T_MAX - size) < (curl_off_t)len)
          /* unlikely to actually be a transfer this big, but avoid integer
             overflow */
          size = CURL_OFF_T_MAX;
        وگرنه
          size += len;

        /* Progress size includes both header line and literal body */
        Curl_pgrsSetDownloadSize(data, size);

        اگر(data->req.bytecount == size)
          /* All data already transferred (header + literal body) */
          Curl_xfer_setup_nop(data);
        وگرنه {
          /* Setup to receive the literal body data.
             maxdownload and transfer size include both header line and
             literal body */
          data->req.maxdownload = size;
          Curl_xfer_setup_recv(data, FIRSTSOCKET, size);
        }
        /* End of DO phase */
        imap_state(data, imapc, IMAP_STOP);
      }
      وگرنه {
        /* Failed to parse literal, write the line */
        result = Curl_client_write(data, CLIENTWRITE_BODY, سطر, len);
      }
    }
    وگرنه {
      /* No literal, write the line as-is */
      result = Curl_client_write(data, CLIENTWRITE_BODY, سطر, len);
    }
  }
  وگرنه اگر(imapcode != IMAP_RESP_OK)
    result = CURLE_QUOTE_ERROR;
  وگرنه
    /* End of DO phase */
    imap_state(data, imapc, IMAP_STOP);

  بازگشت result;
}

/* For SELECT responses */
ایستا CURLcode imap_state_select_resp(ساختار Curl_easy *data,
                                       ساختار imap_conn *imapc,
                                       ساختار IMAP *imap,
                                       صحیح imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  (تهی)instate;

  اگر(imapcode == '*') {
    /* See if this is an UIDVALIDITY response */
    ثابت نویسه *سطر = curlx_dyn_ptr(&imapc->pp.recvbuf);
    size_t len = curlx_dyn_len(&imapc->pp.recvbuf);
    اگر((len >= 18) && checkprefix("OK [UIDVALIDITY ", &سطر[2])) {
      curl_off_t مقدار;
      ثابت نویسه *p = &سطر[2] + CURL_CSTRLEN("OK [UIDVALIDITY ");
      اگر(!curlx_str_عدد(&p, &مقدار, UINT_MAX)) {
        imapc->mb_uidvalidity = (unsigned صحیح)مقدار;
        imapc->mb_uidvalidity_set = TRUE;
      }
    }
  }
  وگرنه اگر(imapcode == IMAP_RESP_OK) {
    /* Check if the UIDVALIDITY has been specified and matches */
    اگر(imap->uidvalidity_set && imapc->mb_uidvalidity_set &&
       (imap->uidvalidity != imapc->mb_uidvalidity)) {
      failf(data, "Mailbox UIDVALIDITY has changed");
      result = CURLE_REMOTE_FILE_NOT_FOUND;
    }
    وگرنه {
      /* Note the currently opened mailbox on this connection */
      DEBUGASSERT(!imapc->mailbox);
      imapc->mailbox = curlx_strdup(imap->mailbox);
      اگر(!imapc->mailbox)
        بازگشت CURLE_OUT_OF_MEMORY;

      اگر(imap->custom)
        result = imap_perform_list(data, imapc, imap);
      وگرنه اگر(imap->query)
        result = imap_perform_جستجو(data, imapc, imap);
      وگرنه
        result = imap_perform_fetch(data, imapc, imap);
    }
  }
  وگرنه {
    failf(data, "Select failed");
    result = CURLE_LOGIN_DENIED;
  }

  بازگشت result;
}

/* For the (first line of the) FETCH responses */
ایستا CURLcode imap_state_fetch_resp(ساختار Curl_easy *data,
                                      ساختار imap_conn *imapc,
                                      صحیح imapcode,
                                      imapstate instate)
{
  CURLcode result = CURLE_OK;
  ساختار pingpong *pp = &imapc->pp;
  ثابت نویسه *ptr = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  bool parsed = FALSE;
  curl_off_t size = 0;

  (تهی)instate;

  اگر(imapcode != '*') {
    Curl_pgrsSetDownloadSize(data, -1);
    imap_state(data, imapc, IMAP_STOP);
    بازگشت CURLE_REMOTE_FILE_NOT_FOUND;
  }

  /* Something like this is received "* 1 FETCH (BODY[TEXT] {2021}\r" so parse
     the continuation data contained within the curly brackets */
  ptr = imap_find_literal(ptr, len);
  اگر(ptr) {
    ptr++;
    اگر(!curlx_str_عدد(&ptr, &size, CURL_OFF_T_MAX) &&
       !curlx_str_single(&ptr, '}'))
      parsed = TRUE;
  }

  اگر(parsed) {
    infof(data, "Found %" FMT_OFF_T " bytes to download", size);
    Curl_pgrsSetDownloadSize(data, size);

    اگر(pp->overflow) {
      /* At this point there is a data in the receive buffer that is body
         content, send it as body and then skip it. Do note that there may
         even be additional "headers" after the body. */
      size_t chunk = pp->overflow;

      /* keep only the overflow */
      curlx_dyn_tail(&pp->recvbuf, chunk);
      pp->nfinal = 0; /* done */

      اگر((curl_off_t)chunk > size)
        /* The conversion from curl_off_t to size_t is always fine here */
        chunk = (size_t)size;

      اگر(!chunk) {
        /* no size, we are done with the data */
        imap_state(data, imapc, IMAP_STOP);
        بازگشت CURLE_OK;
      }
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 curlx_dyn_ptr(&pp->recvbuf), chunk);
      اگر(result)
        بازگشت result;

      infof(data, "Written %zu bytes, %" FMT_OFF_T
            " bytes are left for transfer", chunk, (curl_off_t)(size - chunk));

      /* Have we used the entire overflow or part of it?*/
      اگر(pp->overflow > chunk) {
        /* remember the remaining trailing overflow data */
        pp->overflow -= chunk;
        curlx_dyn_tail(&pp->recvbuf, pp->overflow);
      }
      وگرنه {
        pp->overflow = 0; /* handled */
        /* Free the cache */
        curlx_dyn_reset(&pp->recvbuf);
      }
    }

    اگر(data->req.bytecount == size)
      /* The entire data is already transferred! */
      Curl_xfer_setup_nop(data);
    وگرنه {
      /* IMAP download */
      data->req.maxdownload = size;
      Curl_xfer_setup_recv(data, FIRSTSOCKET, size);
    }
  }
  وگرنه {
    /* We do not know how to parse this line */
    failf(data, "Failed to parse FETCH response.");
    result = CURLE_WEIRD_SERVER_REPLY;
  }

  /* End of DO phase */
  imap_state(data, imapc, IMAP_STOP);

  بازگشت result;
}

/* For final FETCH responses performed after the download */
ایستا CURLcode imap_state_fetch_final_resp(ساختار Curl_easy *data,
                                            ساختار imap_conn *imapc,
                                            صحیح imapcode,
                                            imapstate instate)
{
  CURLcode result = CURLE_OK;

  (تهی)instate;

  اگر(imapcode != IMAP_RESP_OK)
    result = CURLE_WEIRD_SERVER_REPLY;
  وگرنه
    /* End of DONE phase */
    imap_state(data, imapc, IMAP_STOP);

  بازگشت result;
}

/* For APPEND responses */
ایستا CURLcode imap_state_append_resp(ساختار Curl_easy *data,
                                       ساختار imap_conn *imapc,
                                       صحیح imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  (تهی)instate;

  اگر(imapcode != '+') {
    result = CURLE_UPLOAD_FAILED;
  }
  وگرنه {
    /* Set the progress upload size */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* IMAP upload */
    Curl_xfer_setup_send(data, FIRSTSOCKET);

    /* End of DO phase */
    imap_state(data, imapc, IMAP_STOP);
  }

  بازگشت result;
}

/* For final APPEND responses performed after the upload */
ایستا CURLcode imap_state_append_final_resp(ساختار Curl_easy *data,
                                             ساختار imap_conn *imapc,
                                             صحیح imapcode,
                                             imapstate instate)
{
  CURLcode result = CURLE_OK;

  (تهی)instate;

  اگر(imapcode != IMAP_RESP_OK)
    result = CURLE_UPLOAD_FAILED;
  وگرنه
    /* End of DONE phase */
    imap_state(data, imapc, IMAP_STOP);

  بازگشت result;
}

ایستا CURLcode imap_pp_statemachine(ساختار Curl_easy *data,
                                     ساختار connectdata *conn)
{
  CURLcode result = CURLE_OK;
  صحیح imapcode;
  ساختار imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ساختار IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  ساختار pingpong *pp;
  size_t nread = 0;

  اگر(!imapc || !imap)
    بازگشت CURLE_FAILED_INIT;
  pp = &imapc->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not IMAP */
upgrade_tls:
  اگر(imapc->state == IMAP_UPGRADETLS) {
    result = imap_perform_upgrade_tls(data, imapc, conn);
    اگر(result || (imapc->state == IMAP_UPGRADETLS))
      بازگشت result;
  }

  /* Flush any data that needs to be sent */
  اگر(pp->sendleft)
    بازگشت Curl_pp_flushsend(data, pp);

  انجامبده {
    /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, pp, &imapcode, &nread);
    اگر(result)
      بازگشت result;

    /* Was there an error parsing the response line? */
    اگر(imapcode == -1)
      بازگشت CURLE_WEIRD_SERVER_REPLY;

    اگر(!imapcode)
      بشکن;

    /* We have now received a full IMAP server response */
    انتخاب(imapc->state) {
    حالت IMAP_SERVERGREET:
      result = imap_state_servergreet_resp(data, imapc,
                                           imapcode, imapc->state);
      بشکن;

    حالت IMAP_CAPABILITY:
      result = imap_state_capability_resp(data, imapc, imapcode, imapc->state);
      بشکن;

    حالت IMAP_STARTTLS:
      result = imap_state_starttls_resp(data, imapc, imapcode, imapc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      اگر(!result && (imapc->state == IMAP_UPGRADETLS))
        برو upgrade_tls;
      بشکن;

    حالت IMAP_AUTHENTICATE:
      result = imap_state_auth_resp(data, imapc, imapcode, imapc->state);
      بشکن;

    حالت IMAP_LOGIN:
      result = imap_state_login_resp(data, imapc, imapcode, imapc->state);
      بشکن;

    حالت IMAP_LIST:
    حالت IMAP_جستجو:
      result = imap_state_listsearch_resp(data, imapc, imapcode, imapc->state);
      بشکن;

    حالت IMAP_SELECT:
      result = imap_state_select_resp(data, imapc, imap,
                                      imapcode, imapc->state);
      بشکن;

    حالت IMAP_FETCH:
      result = imap_state_fetch_resp(data, imapc, imapcode, imapc->state);
      بشکن;

    حالت IMAP_FETCH_FINAL:
      result = imap_state_fetch_final_resp(data, imapc,
                                           imapcode, imapc->state);
      بشکن;

    حالت IMAP_APPEND:
      result = imap_state_append_resp(data, imapc, imapcode, imapc->state);
      بشکن;

    حالت IMAP_APPEND_FINAL:
      result = imap_state_append_final_resp(data, imapc,
                                            imapcode, imapc->state);
      بشکن;

    حالت IMAP_LOGOUT:
    پیشفرض:
      /* internal error */
      imap_state(data, imapc, IMAP_STOP);
      بشکن;
    }
  } وقتی(!result && imapc->state != IMAP_STOP && Curl_pp_moredata(pp));

  بازگشت result;
}

/* Called repeatedly until done from multi.c */
ایستا CURLcode imap_multi_statemach(ساختار Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ساختار imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  *done = FALSE;
  اگر(!imapc)
    بازگشت CURLE_FAILED_INIT;
  result = Curl_pp_statemach(data, &imapc->pp, FALSE, FALSE);
  *done = (imapc->state == IMAP_STOP);

  بازگشت result;
}

ایستا CURLcode imap_block_statemach(ساختار Curl_easy *data,
                                     ساختار imap_conn *imapc,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;

  وقتی(imapc->state != IMAP_STOP && !result)
    result = Curl_pp_statemach(data, &imapc->pp, TRUE, disconnecting);

  بازگشت result;
}

/* For the IMAP "protocol connect" and "doing" phases only */
ایستا CURLcode imap_pollset(ساختار Curl_easy *data,
                             ساختار easy_pollset *ps)
{
  ساختار imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  بازگشت imapc ? Curl_pp_pollset(data, &imapc->pp, ps) : CURLE_OK;
}

ایستا تهی imap_easy_reset(ساختار IMAP *imap)
{
  curlx_safefree(imap->mailbox);
  curlx_safefree(imap->uid);
  curlx_safefree(imap->mindex);
  curlx_safefree(imap->section);
  curlx_safefree(imap->partial);
  curlx_safefree(imap->query);
  curlx_safefree(imap->custom);
  curlx_safefree(imap->custom_params);
  imap->uidvalidity_set = FALSE;
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
ایستا bool imap_is_bchar(نویسه ch)
{
  /* Performing the alnum check first with macro is faster because of ASCII
     arithmetic */
  بازگشت ch && (ISALNUM(ch) || strchr(":@/&=-._~!$\'()*+,%", ch));
}

/***********************************************************************
 *
 * imap_parse_url_options()
 *
 * Parse the URL login options.
 */
ایستا CURLcode imap_parse_url_options(ساختار connectdata *conn,
                                       ساختار imap_conn *imapc)
{
  CURLcode result = CURLE_OK;
  ثابت نویسه *ptr = conn->options;
  bool prefer_login = FALSE;

  وقتی(!result && ptr && *ptr) {
    ثابت نویسه *کلید = ptr;
    ثابت نویسه *مقدار;

    وقتی(*ptr && *ptr != '=')
      ptr++;

    مقدار = ptr + 1;

    وقتی(*ptr && *ptr != ';')
      ptr++;

    اگر(curl_strnequal(کلید, "AUTH=+LOGIN", 11)) {
      /* User prefers plaintext LOGIN over any SASL, including SASL LOGIN */
      prefer_login = TRUE;
      imapc->sasl.prefmech = SASL_AUTH_NONE;
    }
    وگرنه اگر(curl_strnequal(کلید, "AUTH=", 5)) {
      prefer_login = FALSE;
      result = Curl_sasl_parse_url_auth_option(&imapc->sasl,
                                               مقدار, ptr - مقدار);
    }
    وگرنه {
      prefer_login = FALSE;
      result = CURLE_URL_MALFORMAT;
    }

    اگر(*ptr == ';')
      ptr++;
  }

  اگر(prefer_login)
    imapc->preftype = IMAP_TYPE_CLEARTEXT;
  وگرنه {
    انتخاب(imapc->sasl.prefmech) {
    حالت SASL_AUTH_NONE:
      imapc->preftype = IMAP_TYPE_NONE;
      بشکن;
    حالت SASL_AUTH_DEFAULT:
      imapc->preftype = IMAP_TYPE_ANY;
      بشکن;
    پیشفرض:
      imapc->preftype = IMAP_TYPE_SASL;
      بشکن;
    }
  }

  بازگشت result;
}

/***********************************************************************
 *
 * imap_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
ایستا CURLcode imap_parse_url_path(ساختار Curl_easy *data,
                                    ساختار IMAP *imap)
{
  /* The imap struct is already initialized in imap_connect() */
  CURLcode result = CURLE_OK;
  ثابت نویسه *begin = &data->state.up.path[1]; /* skip leading slash */
  ثابت نویسه *ptr = begin;

  /* See how much of the URL is a valid path and decode it */
  وقتی(imap_is_bchar(*ptr))
    ptr++;

  اگر(ptr != begin) {
    /* Remove the trailing slash if present */
    ثابت نویسه *end = ptr;
    اگر(end > begin && end[-1] == '/')
      end--;

    result = Curl_urldecode(begin, end - begin, &imap->mailbox, پوچ,
                            REJECT_CTRL);
    اگر(result)
      بازگشت result;
  }
  وگرنه
    imap->mailbox = پوچ;

  /* There can be any number of parameters in the form ";NAME=VALUE" */
  وقتی(*ptr == ';') {
    نویسه *name;
    نویسه *مقدار;
    size_t valuelen;

    /* Find the length of the name parameter */
    begin = ++ptr;
    وقتی(*ptr && *ptr != '=')
      ptr++;

    اگر(!*ptr)
      بازگشت CURLE_URL_MALFORMAT;

    /* Decode the name parameter */
    result = Curl_urldecode(begin, ptr - begin, &name, پوچ,
                            REJECT_CTRL);
    اگر(result)
      بازگشت result;

    /* Find the length of the value parameter */
    begin = ++ptr;
    وقتی(imap_is_bchar(*ptr))
      ptr++;

    /* Decode the value parameter */
    result = Curl_urldecode(begin, ptr - begin, &مقدار, &valuelen,
                            REJECT_CTRL);
    اگر(result) {
      curlx_free(name);
      بازگشت result;
    }

    DEBUGF(infof(data, "IMAP URL parameter '%s' = '%s'", name, مقدار));

    /* Process the known hierarchical parameters (UIDVALIDITY, UID, SECTION
       and PARTIAL) stripping of the trailing slash character if it is
       present.

       Note: Unknown parameters trigger a URL_MALFORMAT error. */
    اگر(valuelen > 0 && مقدار[valuelen - 1] == '/')
      مقدار[valuelen - 1] = '\0';
    اگر(valuelen) {
      اگر(curl_strequal(name, "UIDVALIDITY") && !imap->uidvalidity_set) {
        curl_off_t رقم;
        ثابت نویسه *p = (ثابت نویسه *)مقدار;
        اگر(!curlx_str_عدد(&p, &رقم, UINT_MAX)) {
          imap->uidvalidity = (unsigned صحیح)رقم;
          imap->uidvalidity_set = TRUE;
        }
        curlx_free(مقدار);
      }
      وگرنه اگر(curl_strequal(name, "UID") && !imap->uid) {
        imap->uid = مقدار;
      }
      وگرنه اگر(curl_strequal(name, "MAILINDEX") && !imap->mindex) {
        imap->mindex = مقدار;
      }
      وگرنه اگر(curl_strequal(name, "SECTION") && !imap->section) {
        imap->section = مقدار;
      }
      وگرنه اگر(curl_strequal(name, "PARTIAL") && !imap->partial) {
        imap->partial = مقدار;
      }
      وگرنه {
        curlx_free(name);
        curlx_free(مقدار);
        بازگشت CURLE_URL_MALFORMAT;
      }
    }
    وگرنه
      /* blank? */
      curlx_free(مقدار);
    curlx_free(name);
  }

  /* Does the URL contain a query parameter? Only valid when we have a mailbox
     and no UID as per RFC-5092 */
  اگر(imap->mailbox && !imap->uid && !imap->mindex) {
    /* Get the query parameter, URL decoded */
    CURLUcode uc = curl_url_get(data->state.uh, CURLUPART_QUERY, &imap->query,
                                CURLU_URLDECODE);
    اگر(uc == CURLUE_OUT_OF_MEMORY)
      بازگشت CURLE_OUT_OF_MEMORY;
  }

  /* Any extra stuff at the end of the URL is an error */
  اگر(*ptr)
    بازگشت CURLE_URL_MALFORMAT;

  بازگشت CURLE_OK;
}

/***********************************************************************
 *
 * imap_parse_custom_request()
 *
 * Parse the custom request.
 */
ایستا CURLcode imap_parse_custom_request(ساختار Curl_easy *data,
                                          ساختار IMAP *imap)
{
  CURLcode result = CURLE_OK;
  ثابت نویسه *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  اگر(custom) {
    /* URL decode the custom request */
    result = Curl_urldecode(custom, 0, &imap->custom, پوچ, REJECT_CTRL);

    /* Extract the parameters if specified */
    اگر(!result) {
      ثابت نویسه *params = imap->custom;

      وقتی(*params && *params != ' ')
        params++;

      اگر(*params) {
        imap->custom_params = curlx_strdup(params);
        imap->custom[params - imap->custom] = '\0';

        اگر(!imap->custom_params)
          result = CURLE_OUT_OF_MEMORY;
      }
    }
  }

  بازگشت result;
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
ایستا CURLcode imap_connect(ساختار Curl_easy *data, bool *done)
{
  ساختار imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  CURLcode result = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  اگر(!imapc)
    بازگشت CURLE_FAILED_INIT;

  /* Parse the URL options */
  result = imap_parse_url_options(data->conn, imapc);
  اگر(result)
    بازگشت result;

  /* Start off waiting for the server greeting response */
  imap_state(data, imapc, IMAP_SERVERGREET);

  /* Start off with an response id of '*' */
  curlx_strcopy(imapc->resptag, اندازه(imapc->resptag), STRCONST("*"));

  result = imap_multi_statemach(data, done);

  بازگشت result;
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
ایستا CURLcode imap_done(ساختار Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;
  ساختار imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ساختار IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);

  اگر(!imapc)
    بازگشت CURLE_FAILED_INIT;
  اگر(!imap)
    بازگشت CURLE_OK;

  اگر(status || premature) {
    CURL_TRC_M(data, "IMAP done, not finalizing transfer");
    connclose(conn); /* marked for closure */
    result = status;         /* use the already set error code */
  }
  وگرنه اگر(!data->set.connect_only &&
          ((!imap->custom && (imap->uid || imap->mindex)) ||
           (imap->custom && data->req.maxdownload > 0) ||
           data->state.upload || IS_MIME_POST(data))) {
    /* Handle responses after FETCH or APPEND transfer has finished.
       For custom commands, check if we set up a download which indicates
       a FETCH-like command with literal data. */

    اگر(!data->state.upload && !IS_MIME_POST(data))
      imap_state(data, imapc, IMAP_FETCH_FINAL);
    وگرنه {
      /* End the APPEND command first by sending an empty line */
      result = Curl_pp_sendf(data, &imapc->pp, "%s", "");
      اگر(!result)
        imap_state(data, imapc, IMAP_APPEND_FINAL);
    }

    /* Run the state-machine */
    اگر(!result)
      result = imap_block_statemach(data, imapc, FALSE);
  }

  imap_easy_reset(imap);
  بازگشت result;
}

/***********************************************************************
 *
 * imap_perform()
 *
 * This is the actual DO function for IMAP. Fetch or append a message, or do
 * other things according to the options previously setup.
 */
ایستا CURLcode imap_perform(ساختار Curl_easy *data, bool *connected,
                             bool *dophase_done)
{
  /* This is IMAP and no proxy */
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;
  ساختار imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ساختار IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  bool selected = FALSE;

  DEBUGF(infof(data, "DO phase starts"));
  اگر(!imapc || !imap)
    بازگشت CURLE_FAILED_INIT;

  اگر(data->req.no_body) {
    /* Requested no body means no transfer */
    imap->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* Determine if the requested mailbox (with the same UIDVALIDITY if set)
     has already been selected on this connection */
  اگر(imap->mailbox && imapc->mailbox &&
     (!strcmp(imap->mailbox, imapc->mailbox) ||
      (curl_strequal(imap->mailbox, "INBOX") &&
       curl_strequal(imapc->mailbox, "INBOX"))) &&
     (!imap->uidvalidity_set || !imapc->mb_uidvalidity_set ||
      (imap->uidvalidity == imapc->mb_uidvalidity)))
    selected = TRUE;

  /* Start the first command in the DO phase */
  اگر(data->state.upload || IS_MIME_POST(data))
    /* APPEND can be executed directly */
    result = imap_perform_append(data, imapc, imap);
  وگرنه اگر(imap->custom && (selected || !imap->mailbox))
    /* Custom command using the same mailbox or no mailbox */
    result = imap_perform_list(data, imapc, imap);
  وگرنه اگر(!imap->custom && selected && (imap->uid || imap->mindex))
    /* FETCH from the same mailbox */
    result = imap_perform_fetch(data, imapc, imap);
  وگرنه اگر(!imap->custom && selected && imap->query)
    /* SEARCH the current mailbox */
    result = imap_perform_جستجو(data, imapc, imap);
  وگرنه اگر(imap->mailbox && !selected &&
          (imap->custom || imap->uid || imap->mindex || imap->query))
    /* SELECT the mailbox */
    result = imap_perform_select(data, imapc, imap);
  وگرنه
    /* LIST */
    result = imap_perform_list(data, imapc, imap);

  اگر(result)
    بازگشت result;

  /* Run the state-machine */
  result = imap_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(conn, FIRSTSOCKET);

  اگر(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  بازگشت result;
}

/* Call this when the DO phase has completed */
ایستا CURLcode imap_dophase_done(ساختار Curl_easy *data,
                                  ساختار IMAP *imap,
                                  bool connected)
{
  (تهی)connected;

  اگر(imap->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);

  بازگشت CURLE_OK;
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
ایستا CURLcode imap_regular_transfer(ساختار Curl_easy *data,
                                      ساختار IMAP *imap,
                                      bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  data->req.size = -1;

  /* Set the progress data */
  Curl_pgrsReset(data);

  /* Carry out the perform */
  result = imap_perform(data, &connected, dophase_done);

  /* Perform post DO phase operations if necessary */
  اگر(!result && *dophase_done)
    result = imap_dophase_done(data, imap, connected);

  بازگشت result;
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
ایستا CURLcode imap_do(ساختار Curl_easy *data, bool *done)
{
  ساختار IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* default to false */

  اگر(!imap)
    بازگشت CURLE_FAILED_INIT;
  /* Parse the URL path */
  result = imap_parse_url_path(data, imap);
  اگر(result)
    بازگشت result;

  /* Parse the custom request */
  result = imap_parse_custom_request(data, imap);
  اگر(result)
    بازگشت result;

  result = imap_regular_transfer(data, imap, done);

  بازگشت result;
}

/***********************************************************************
 *
 * imap_disconnect()
 *
 * Disconnect from an IMAP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ایستا CURLcode imap_disconnect(ساختار Curl_easy *data,
                                ساختار connectdata *conn, bool dead_connection)
{
  ساختار imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);

  اگر(imapc &&
     /* We cannot send quit unconditionally. If this connection is stale or
        bad in any way (pingpong has pending data to send),
        sending quit and waiting around here will make the
        disconnect wait in vain and cause more problems than we need to. */
     !dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &imapc->pp) &&
     !imap_perform_logout(data, imapc))
    (تهی)imap_block_statemach(data, imapc, TRUE); /* ignore errors */

  بازگشت CURLE_OK;
}

/* Called from multi.c while DOing */
ایستا CURLcode imap_doing(ساختار Curl_easy *data, bool *dophase_done)
{
  ساختار IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  CURLcode result;

  اگر(!imap)
    بازگشت CURLE_FAILED_INIT;

  result = imap_multi_statemach(data, dophase_done);
  اگر(result)
    DEBUGF(infof(data, "DO phase failed"));
  وگرنه اگر(*dophase_done) {
    result = imap_dophase_done(data, imap, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  بازگشت result;
}

ایستا تهی imap_easy_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار IMAP *imap = entry;
  (تهی)کلید;
  (تهی)klen;
  imap_easy_reset(imap);
  curlx_free(imap);
}

ایستا تهی imap_conn_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار imap_conn *imapc = entry;
  (تهی)کلید;
  (تهی)klen;
  Curl_pp_disconnect(&imapc->pp);
  curlx_safefree(imapc->mailbox);
  curlx_free(imapc);
}

/* SASL parameters for the imap protocol */
ایستا ثابت ساختار SASLproto saslimap = {
  "imap",                     /* The service name */
  imap_perform_authenticate,  /* Send authentication command */
  imap_continue_authenticate, /* Send authentication continuation */
  imap_cancel_authenticate,   /* Send authentication cancellation */
  imap_get_message,           /* Get SASL response message */
  0,                          /* No maximum initial response length */
  '+',                        /* Code received when continuation is expected */
  IMAP_RESP_OK,               /* Code to receive upon authentication success */
  SASL_AUTH_DEFAULT,          /* Default mechanisms */
  SASL_FLAG_BASE64            /* Configuration flags */
};

ایستا CURLcode imap_setup_connection(ساختار Curl_easy *data,
                                      ساختار connectdata *conn)
{
  ساختار imap_conn *imapc;
  ساختار pingpong *pp;
  ساختار IMAP *imap;

  imapc = curlx_calloc(1, اندازه(*imapc));
  اگر(!imapc)
    بازگشت CURLE_OUT_OF_MEMORY;

  pp = &imapc->pp;
  PINGPONG_SETUP(pp, imap_pp_statemachine, imap_endofresp);

  /* Set the default preferred authentication type and mechanism */
  imapc->preftype = IMAP_TYPE_ANY;
  Curl_sasl_init(&imapc->sasl, data, &saslimap);

  Curl_pp_init(pp, Curl_pgrs_now(data));

  اگر(Curl_conn_meta_set(conn, CURL_META_IMAP_CONN, imapc, imap_conn_dtor))
    بازگشت CURLE_OUT_OF_MEMORY;

  imap = curlx_calloc(1, اندازه(ساختار IMAP));
  اگر(!imap ||
     Curl_meta_set(data, CURL_META_IMAP_EASY, imap, imap_easy_dtor))
    بازگشت CURLE_OUT_OF_MEMORY;

  بازگشت CURLE_OK;
}

/*
 * IMAP protocol.
 */
ثابت ساختار Curl_protocol Curl_protocol_imap = {
  imap_setup_connection,            /* setup_connection */
  imap_do,                          /* do_it */
  imap_done,                        /* done */
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

#endif /* CURL_DISABLE_IMAP */
