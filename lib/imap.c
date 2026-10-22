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
#অন্তর্ভাৱ "curl_setup.h"
#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "imap.h"

#ifndef CURL_DISABLE_IMAP

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাৱ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাৱ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাৱ <netdb.h>
#endif
#ifdef __VMS
#অন্তর্ভাৱ <in.h>
#অন্তর্ভাৱ <inet.h>
#endif

#অন্তর্ভাৱ "curlx/dynbuf.h"
#অন্তর্ভাৱ "sendf.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "progress.h"
#অন্তর্ভাৱ "transfer.h"
#অন্তর্ভাৱ "escape.h"
#অন্তর্ভাৱ "pingpong.h"
#অন্তর্ভাৱ "mime.h"
#অন্তর্ভাৱ "curlx/strparse.h"
#অন্তর্ভাৱ "strcase.h"
#অন্তর্ভাৱ "vtls/vtls.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "select.h"
#অন্তর্ভাৱ "url.h"
#অন্তর্ভাৱ "bufref.h"
#অন্তর্ভাৱ "curl_sasl.h"
#অন্তর্ভাৱ "curlx/strcopy.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_IMAP_EASY   "meta:proto:imap:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_IMAP_CONN   "meta:proto:imap:conn"

প্রকারনির্দেশ গণনা {
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
  IMAP_অনুসন্ধান,
  IMAP_LOGOUT,
  IMAP_LAST          /* never used */
} imapstate;

/* imap_conn is used for struct connection-oriented data */
সংরচনা imap_conn {
  সংরচনা pingpong pp;
  সংরচনা SASL sasl;           /* SASL-related parameters */
  ৱর্ণ *mailbox;              /* The last selected mailbox */
  imapstate state;            /* Always use imap.c:state() to change state! */
  unsigned পূর্ণাঙ্ক mb_uidvalidity; /* UIDVALIDITY parsed from select response */
  ৱর্ণ resptag[5];            /* Response tag to wait for */
  unsigned ৱর্ণ preftype;     /* Preferred authentication type */
  unsigned ৱর্ণ cmdid;        /* Last used command ID */
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
সংরচনা IMAP {
  curl_pp_transfer transfer;
  ৱর্ণ *mailbox;          /* Mailbox to select */
  ৱর্ণ *uid;              /* Message UID to fetch */
  ৱর্ণ *mindex;           /* Index in mail box of mail to fetch */
  ৱর্ণ *section;          /* Message SECTION to fetch */
  ৱর্ণ *partial;          /* Message PARTIAL to fetch */
  ৱর্ণ *query;            /* Query to search for */
  ৱর্ণ *custom;           /* Custom request */
  ৱর্ণ *custom_params;    /* Parameters for the custom request */
  unsigned পূর্ণাঙ্ক uidvalidity; /* UIDVALIDITY to check in select */
  BIT(uidvalidity_set);
};

#define IMAP_RESP_OK       1
#define IMAP_RESP_NOT_OK   2
#define IMAP_RESP_PREAUTH  3

সংরচনা ulbits {
  পূর্ণাঙ্ক bit;
  ধ্রুৱ ৱর্ণ *flag;
};

/***********************************************************************
 *
 * imap_sendf()
 *
 * Sends the formatted string as an IMAP command to the server.
 *
 * Designed to never block.
 */
স্থির CURLcode imap_sendf(সংরচনা Curl_easy *data,
                           সংরচনা imap_conn *imapc,
                           ধ্রুৱ ৱর্ণ *fmt, ...) CURL_PRINTF(3, 4);
স্থির CURLcode imap_sendf(সংরচনা Curl_easy *data,
                           সংরচনা imap_conn *imapc,
                           ধ্রুৱ ৱর্ণ *fmt, ...)
{
  CURLcode result = CURLE_OK;
  va_list ap;

  DEBUGASSERT(fmt);

  /* Calculate the tag based on the connection ID and command ID */
  curl_msnprintf(imapc->resptag, পরিমাণ(imapc->resptag), "%c%03d",
                 'A' + curlx_sltosi((long)(data->conn->connection_id % 26)),
                 ++imapc->cmdid);

  va_start(ap, fmt);
  result = Curl_pp_vsendf(data, &imapc->pp, imapc->resptag, fmt, ap);
  va_end(ap);
  ঘূৰাই result;
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
স্থির ৱর্ণ *imap_atom(ধ্রুৱ ৱর্ণ *str, bool escape_only)
{
  সংরচনা dynbuf সাৰি;
  size_t nclean;
  size_t len;

  যদি(!str)
    ঘূৰাই রিক্ত;

  len = strlen(str);
  nclean = strcspn(str, "() {%*]\\\"");
  যদি(len == nclean)
    /* nothing to escape, return a strdup */
    ঘূৰাই curlx_strdup(str);

  curlx_dyn_init(&সাৰি, 2000);

  যদি(!escape_only && curlx_dyn_addn(&সাৰি, "\"", 1))
    ঘূৰাই রিক্ত;

  যেতিয়ালৈ(*str) {
    যদি((*str == '\\' || *str == '"') &&
       curlx_dyn_addn(&সাৰি, "\\", 1))
      ঘূৰাই রিক্ত;
    যদি(curlx_dyn_addn(&সাৰি, str, 1))
      ঘূৰাই রিক্ত;
    str++;
  }

  যদি(!escape_only && curlx_dyn_addn(&সাৰি, "\"", 1))
    ঘূৰাই রিক্ত;

  ঘূৰাই curlx_dyn_ptr(&সাৰি);
}

/*
 * Finds the start of a literal '{size}' in line, skipping over quoted strings.
 */
স্থির ধ্রুৱ ৱর্ণ *imap_find_literal(ধ্রুৱ ৱর্ণ *সাৰি, size_t len)
{
  ধ্রুৱ ৱর্ণ *end = সাৰি + len;
  bool in_quote = FALSE;

  যেতিয়ালৈ(সাৰি < end) {
    যদি(in_quote) {
      যদি(*সাৰি == '\\' && (সাৰি + 1) < end) {
        সাৰি += 2;
        অনুৱর্ত;
      }
      যদি(*সাৰি == '"')
        in_quote = FALSE;
    }
    অন্যথা {
      যদি(*সাৰি == '"')
        in_quote = TRUE;
      অন্যথা যদি(*সাৰি == '{')
        ঘূৰাই সাৰি;
    }
    সাৰি++;
  }
  ঘূৰাই রিক্ত;
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
স্থির bool imap_matchresp(ধ্রুৱ ৱর্ণ *সাৰি, size_t len, ধ্রুৱ ৱর্ণ *cmd)
{
  ধ্রুৱ ৱর্ণ *end = সাৰি + len;
  size_t cmd_len = strlen(cmd);

  /* Skip the untagged response marker */
  সাৰি += 2;

  /* Do we have a number after the marker? */
  যদি(সাৰি < end && ISDIGIT(*সাৰি)) {
    /* Skip the number */
    কৰা
      সাৰি++;
    যেতিয়ালৈ(সাৰি < end && ISDIGIT(*সাৰি));

    /* Do we have the space character? */
    যদি(সাৰি == end || *সাৰি != ' ')
      ঘূৰাই FALSE;

    সাৰি++;
  }

  /* Does the command name match and is it followed by a space character or at
     the end of line? */
  যদি(সাৰি + cmd_len <= end && curl_strnequal(সাৰি, cmd, cmd_len) &&
     (সাৰি[cmd_len] == ' ' || সাৰি + cmd_len + 2 == end))
    ঘূৰাই TRUE;

  ঘূৰাই FALSE;
}

/***********************************************************************
 *
 * imap_endofresp()
 *
 * Checks whether the given string is a valid tagged, untagged or continuation
 * response which can be processed by the response handler.
 */
স্থির bool imap_endofresp(সংরচনা Curl_easy *data, সংরচনা connectdata *conn,
                           ধ্রুৱ ৱর্ণ *সাৰি, size_t len, পূর্ণাঙ্ক *resp)
{
  সংরচনা imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  সংরচনা IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  ধ্রুৱ ৱর্ণ *id;
  size_t id_len;

  DEBUGASSERT(imapc);
  DEBUGASSERT(imap);
  যদি(!imapc || !imap)
    ঘূৰাই FALSE;

  /* Do we have a tagged command response? */
  id = imapc->resptag;
  id_len = strlen(id);
  যদি(len >= id_len + 1 && !memcmp(id, সাৰি, id_len) && সাৰি[id_len] == ' ') {
    সাৰি += id_len + 1;
    len -= id_len + 1;

    যদি(len >= 2 && !memcmp(সাৰি, "OK", 2))
      *resp = IMAP_RESP_OK;
    অন্যথা যদি(len >= 7 && !memcmp(সাৰি, "PREAUTH", 7))
      *resp = IMAP_RESP_PREAUTH;
    অন্যথা
      *resp = IMAP_RESP_NOT_OK;

    ঘূৰাই TRUE;
  }

  /* Do we have an untagged command response? */
  যদি(len >= 2 && !memcmp("* ", সাৰি, 2)) {
    ৱিকল্প(imapc->state) {
    /* States which are interested in untagged responses */
    পক্ষ IMAP_CAPABILITY:
      যদি(!imap_matchresp(সাৰি, len, "CAPABILITY"))
        ঘূৰাই FALSE;
      ভঙ্গ;

    পক্ষ IMAP_LIST:
      যদি((!imap->custom && !imap_matchresp(সাৰি, len, "LIST")) ||
         (imap->custom && !imap_matchresp(সাৰি, len, imap->custom) &&
          (!curl_strequal(imap->custom, "STORE") ||
           !imap_matchresp(সাৰি, len, "FETCH")) &&
          !curl_strequal(imap->custom, "SELECT") &&
          !curl_strequal(imap->custom, "EXAMINE") &&
          !curl_strequal(imap->custom, "SEARCH") &&
          !curl_strequal(imap->custom, "EXPUNGE") &&
          !curl_strequal(imap->custom, "LSUB") &&
          !curl_strequal(imap->custom, "UID") &&
          !curl_strequal(imap->custom, "GETQUOTAROOT") &&
          !curl_strequal(imap->custom, "NOOP")))
        ঘূৰাই FALSE;
      ভঙ্গ;

    পক্ষ IMAP_SELECT:
      /* SELECT is special in that its untagged responses do not have a
         common prefix so accept anything! */
      ভঙ্গ;

    পক্ষ IMAP_FETCH:
      যদি(!imap_matchresp(সাৰি, len, "FETCH"))
        ঘূৰাই FALSE;
      ভঙ্গ;

    পক্ষ IMAP_অনুসন্ধান:
      যদি(!imap_matchresp(সাৰি, len, "SEARCH"))
        ঘূৰাই FALSE;
      ভঙ্গ;

    /* Ignore other untagged responses */
    স্ৱভাৱ:
      ঘূৰাই FALSE;
    }

    *resp = '*';
    ঘূৰাই TRUE;
  }

  /* Do we have a continuation response? This should be a + symbol followed by
     a space and optionally some text as per RFC-3501 for the AUTHENTICATE and
     APPEND commands and as outlined in Section 4. Examples of RFC-4959 but
     some email servers ignore this and only send a single + instead. */
  যদি(!imap->custom && ((len == 3 && সাৰি[0] == '+') ||
                       (len >= 2 && !memcmp("+ ", সাৰি, 2)))) {
    ৱিকল্প(imapc->state) {
    /* States which are interested in continuation responses */
    পক্ষ IMAP_AUTHENTICATE:
    পক্ষ IMAP_APPEND:
      *resp = '+';
      ভঙ্গ;

    স্ৱভাৱ:
      failf(data, "Unexpected continuation response");
      *resp = -1;
      ভঙ্গ;
    }

    ঘূৰাই TRUE;
  }

  ঘূৰাই FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * imap_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
স্থির CURLcode imap_get_message(সংরচনা Curl_easy *data, সংরচনা bufref *out)
{
  সংরচনা imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  ৱর্ণ *message;
  size_t len;

  যদি(!imapc)
    ঘূৰাই CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&imapc->pp.recvbuf);
  len = imapc->pp.nfinal;
  যদি(len > 2) {
    /* Find the start of the message */
    len -= 2;
    বাবে(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    যেতিয়ালৈ(len--)
      যদি(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        ভঙ্গ;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, রিক্ত);
  }
  অন্যথা
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, রিক্ত);

  ঘূৰাই CURLE_OK;
}

/***********************************************************************
 *
 * imap_state()
 *
 * This is the ONLY way to change IMAP state!
 */
স্থির শূন্য imap_state(সংরচনা Curl_easy *data,
                       সংরচনা imap_conn *imapc,
                       imapstate newstate)
{
#যদি defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  স্থির ধ্রুৱ ৱর্ণ * ধ্রুৱ names[] = {
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

  যদি(imapc->state != newstate)
    infof(data, "IMAP %p state change from %s to %s",
          (শূন্য *)imapc, names[imapc->state], names[newstate]);
#অন্যথা
  (শূন্য)data;
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
স্থির CURLcode imap_perform_capability(সংরচনা Curl_easy *data,
                                        সংরচনা imap_conn *imapc)
{
  CURLcode result = CURLE_OK;

  imapc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  imapc->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  imapc->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPABILITY command */
  result = imap_sendf(data, imapc, "CAPABILITY");

  যদি(!result)
    imap_state(data, imapc, IMAP_CAPABILITY);

  ঘূৰাই result;
}

/***********************************************************************
 *
 * imap_perform_starttls()
 *
 * Sends the STARTTLS command to start the upgrade to TLS.
 */
স্থির CURLcode imap_perform_starttls(সংরচনা Curl_easy *data,
                                      সংরচনা imap_conn *imapc)
{
  /* Send the STARTTLS command */
  CURLcode result = imap_sendf(data, imapc, "STARTTLS");

  যদি(!result)
    imap_state(data, imapc, IMAP_STARTTLS);

  ঘূৰাই result;
}

/***********************************************************************
 *
 * imap_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
স্থির CURLcode imap_perform_upgrade_tls(সংরচনা Curl_easy *data,
                                         সংরচনা imap_conn *imapc,
                                         সংরচনা connectdata *conn)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  CURLcode result;
  bool ssldone = FALSE;

  যদি(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    যদি(result)
      গচ্ছ out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_imaps;
  }

  DEBUGASSERT(!imapc->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "imap_perform_upgrade_tls, connect -> %d, %d",
               (পূর্ণাঙ্ক)result, ssldone));
  যদি(!result && ssldone) {
    imapc->ssldone = ssldone;
    /* perform CAPA now, changes imapc->state out of IMAP_UPGRADETLS */
    result = imap_perform_capability(data, imapc);
  }
out:
  ঘূৰাই result;
#অন্যথা
  (শূন্য)data;
  (শূন্য)imapc;
  (শূন্য)conn;
  ঘূৰাই CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * imap_perform_login()
 *
 * Sends a clear text LOGIN command to authenticate with.
 */
স্থির CURLcode imap_perform_login(সংরচনা Curl_easy *data,
                                   সংরচনা imap_conn *imapc,
                                   সংরচনা connectdata *conn)
{
  CURLcode result = CURLE_OK;
  ৱর্ণ *user;
  ৱর্ণ *passwd;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  যদি(!conn->creds) {
    imap_state(data, imapc, IMAP_STOP);

    ঘূৰাই result;
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

  যদি(!result)
    imap_state(data, imapc, IMAP_LOGIN);

  ঘূৰাই result;
}

/***********************************************************************
 *
 * imap_perform_authenticate()
 *
 * Sends an AUTHENTICATE command allowing the client to login with the given
 * SASL authentication mechanism.
 */
স্থির CURLcode imap_perform_authenticate(সংরচনা Curl_easy *data,
                                          ধ্রুৱ ৱর্ণ *mech,
                                          ধ্রুৱ সংরচনা bufref *initresp)
{
  সংরচনা imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  CURLcode result = CURLE_OK;
  ধ্রুৱ ৱর্ণ *ir = Curl_bufref_ptr(initresp);

  যদি(!imapc)
    ঘূৰাই CURLE_FAILED_INIT;
  যদি(ir) {
    /* Send the AUTHENTICATE command with the initial response */
    result = imap_sendf(data, imapc, "AUTHENTICATE %s %s",
                        mech, *ir ? ir : "=");
  }
  অন্যথা {
    /* Send the AUTHENTICATE command */
    result = imap_sendf(data, imapc, "AUTHENTICATE %s", mech);
  }

  ঘূৰাই result;
}

/***********************************************************************
 *
 * imap_continue_authenticate()
 *
 * Sends SASL continuation data.
 */
স্থির CURLcode imap_continue_authenticate(সংরচনা Curl_easy *data,
                                           ধ্রুৱ ৱর্ণ *mech,
                                           ধ্রুৱ সংরচনা bufref *resp)
{
  সংরচনা imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  (শূন্য)mech;
  যদি(!imapc)
    ঘূৰাই CURLE_FAILED_INIT;
  ঘূৰাই Curl_pp_sendf(data, &imapc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * imap_cancel_authenticate()
 *
 * Sends SASL cancellation.
 */
স্থির CURLcode imap_cancel_authenticate(সংরচনা Curl_easy *data,
                                         ধ্রুৱ ৱর্ণ *mech)
{
  সংরচনা imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  (শূন্য)mech;
  যদি(!imapc)
    ঘূৰাই CURLE_FAILED_INIT;
  ঘূৰাই Curl_pp_sendf(data, &imapc->pp, "*");
}

/***********************************************************************
 *
 * imap_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to clear text should a common
 * mechanism not be available between the client and server.
 */
স্থির CURLcode imap_perform_authentication(সংরচনা Curl_easy *data,
                                            সংরচনা imap_conn *imapc)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  /* Check if already authenticated OR if there is enough data to authenticate
     with and end the connect phase if we do not */
  যদি(imapc->preauth ||
     !Curl_sasl_can_authenticate(&imapc->sasl, data)) {
    imap_state(data, imapc, IMAP_STOP);
    ঘূৰাই result;
  }

  /* Calculate the SASL login details */
  result = Curl_sasl_start(&imapc->sasl, data, (bool)imapc->ir_supported,
                           &progress);
  যদি(!result) {
    যদি(progress == SASL_INPROGRESS)
      imap_state(data, imapc, IMAP_AUTHENTICATE);
    অন্যথা যদি(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
      /* Perform clear text authentication */
      result = imap_perform_login(data, imapc, data->conn);
    অন্যথা
      result = Curl_sasl_is_blocked(&imapc->sasl, data);
  }

  ঘূৰাই result;
}

/***********************************************************************
 *
 * imap_perform_list()
 *
 * Sends a LIST command or an alternative custom request.
 */
স্থির CURLcode imap_perform_list(সংরচনা Curl_easy *data,
                                  সংরচনা imap_conn *imapc,
                                  সংরচনা IMAP *imap)
{
  CURLcode result = CURLE_OK;

  যদি(imap->custom)
    /* Send the custom request */
    result = imap_sendf(data, imapc, "%s%s", imap->custom,
                        imap->custom_params ? imap->custom_params : "");
  অন্যথা {
    /* Make sure the mailbox is in the correct atom format if necessary */
    ৱর্ণ *mailbox = imap->mailbox ? imap_atom(imap->mailbox, TRUE)
                                  : curlx_strdup("");
    যদি(!mailbox)
      ঘূৰাই CURLE_OUT_OF_MEMORY;

    /* Send the LIST command */
    result = imap_sendf(data, imapc, "LIST \"%s\" *", mailbox);

    curlx_free(mailbox);
  }

  যদি(!result)
    imap_state(data, imapc, IMAP_LIST);

  ঘূৰাই result;
}

/***********************************************************************
 *
 * imap_perform_select()
 *
 * Sends a SELECT command to ask the server to change the selected mailbox.
 */
স্থির CURLcode imap_perform_select(সংরচনা Curl_easy *data,
                                    সংরচনা imap_conn *imapc,
                                    সংরচনা IMAP *imap)
{
  CURLcode result = CURLE_OK;
  ৱর্ণ *mailbox;

  /* Invalidate old information as we are switching mailboxes */
  curlx_safefree(imapc->mailbox);
  imapc->mb_uidvalidity_set = FALSE;

  /* Check we have a mailbox */
  যদি(!imap->mailbox) {
    failf(data, "Cannot SELECT without a mailbox.");
    ঘূৰাই CURLE_URL_MALFORMAT;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  যদি(!mailbox)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  /* Send the SELECT command */
  result = imap_sendf(data, imapc, "SELECT %s", mailbox);

  curlx_free(mailbox);

  যদি(!result)
    imap_state(data, imapc, IMAP_SELECT);

  ঘূৰাই result;
}

/***********************************************************************
 *
 * imap_perform_fetch()
 *
 * Sends a FETCH command to initiate the download of a message.
 */
স্থির CURLcode imap_perform_fetch(সংরচনা Curl_easy *data,
                                   সংরচনা imap_conn *imapc,
                                   সংরচনা IMAP *imap)
{
  CURLcode result = CURLE_OK;
  /* Check we have a UID */
  যদি(imap->uid) {

    /* Send the FETCH command */
    যদি(imap->partial)
      result = imap_sendf(data, imapc, "UID FETCH %s BODY[%s]<%s>",
                          imap->uid, imap->section ? imap->section : "",
                          imap->partial);
    অন্যথা
      result = imap_sendf(data, imapc, "UID FETCH %s BODY[%s]",
                          imap->uid, imap->section ? imap->section : "");
  }
  অন্যথা যদি(imap->mindex) {
    /* Send the FETCH command */
    যদি(imap->partial)
      result = imap_sendf(data, imapc, "FETCH %s BODY[%s]<%s>",
                          imap->mindex, imap->section ? imap->section : "",
                          imap->partial);
    অন্যথা
      result = imap_sendf(data, imapc, "FETCH %s BODY[%s]",
                          imap->mindex, imap->section ? imap->section : "");
  }
  অন্যথা {
    failf(data, "Cannot FETCH without a UID.");
    ঘূৰাই CURLE_URL_MALFORMAT;
  }
  যদি(!result)
    imap_state(data, imapc, IMAP_FETCH);

  ঘূৰাই result;
}

/***********************************************************************
 *
 * imap_perform_append()
 *
 * Sends an APPEND command to initiate the upload of a message.
 */
স্থির CURLcode imap_perform_append(সংরচনা Curl_easy *data,
                                    সংরচনা imap_conn *imapc,
                                    সংরচনা IMAP *imap)
{
  CURLcode result = CURLE_OK;
  ৱর্ণ *mailbox;
  সংরচনা dynbuf flags;

  /* Check we have a mailbox */
  যদি(!imap->mailbox) {
    failf(data, "Cannot APPEND without a mailbox.");
    ঘূৰাই CURLE_URL_MALFORMAT;
  }

#ifndef CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  যদি(IS_MIME_POST(data)) {
    curl_mimepart *postp = data->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(unsigned পূর্ণাঙ্ক)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, data->set.headers, 0);
    result = Curl_mime_prepare_headers(data, postp, রিক্ত,
                                       রিক্ত, MIMESTRATEGY_MAIL);

    যদি(!result && !Curl_checkheaders(data, STRCONST("Mime-Version")))
      result = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    যদি(!result)
      result = Curl_creader_set_mime(data, postp);
    যদি(result)
      ঘূৰাই result;
  }
  অন্যথা
#endif
  {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    যদি(result)
      ঘূৰাই result;
  }

  /* Check we know the size of the upload. This takes all readers
   * into account. Especially crlf conversions which make the size
   * unpredictable, e.g. -1. */
  data->state.infilesize = Curl_creader_total_length(data);
  যদি(data->state.infilesize < 0) {
    যদি(data->set.crlf)
      failf(data, "Cannot APPEND with CRLF conversion making size unknown");
    অন্যথা
      failf(data, "Cannot APPEND with unknown input file size");
    ঘূৰাই CURLE_UPLOAD_FAILED;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  যদি(!mailbox)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  /* Generate flags string and send the APPEND command */
  curlx_dyn_init(&flags, 100);
  যদি(data->set.upload_flags) {
    পূর্ণাঙ্ক i;
    সংরচনা ulbits ulflag[] = {
      { CURLULFLAG_ANSWERED, "Answered" },
      { CURLULFLAG_DELETED, "Deleted" },
      { CURLULFLAG_DRAFT, "Draft" },
      { CURLULFLAG_FLAGGED, "Flagged" },
      { CURLULFLAG_SEEN, "Seen" },
      { 0, রিক্ত }
    };

    result = CURLE_OUT_OF_MEMORY;
    যদি(curlx_dyn_add(&flags, " (")) {
      গচ্ছ cleanup;
    }

    বাবে(i = 0; ulflag[i].bit; i++) {
      যদি(data->set.upload_flags & ulflag[i].bit &&
         ((curlx_dyn_len(&flags) > 2 && curlx_dyn_add(&flags, " ")) ||
          curlx_dyn_add(&flags, "\\") ||
          curlx_dyn_add(&flags, ulflag[i].flag)))
        গচ্ছ cleanup;
    }

    যদি(curlx_dyn_add(&flags, ")"))
      গচ্ছ cleanup;
  }
  অন্যথা যদি(curlx_dyn_add(&flags, ""))
    গচ্ছ cleanup;

  result = imap_sendf(data, imapc, "APPEND %s%s {%" FMT_OFF_T "}",
                      mailbox, curlx_dyn_ptr(&flags), data->state.infilesize);

cleanup:
  curlx_dyn_free(&flags);
  curlx_free(mailbox);

  যদি(!result)
    imap_state(data, imapc, IMAP_APPEND);

  ঘূৰাই result;
}

/***********************************************************************
 *
 * imap_perform_search()
 *
 * Sends a SEARCH command.
 */
স্থির CURLcode imap_perform_অনুসন্ধান(সংরচনা Curl_easy *data,
                                    সংরচনা imap_conn *imapc,
                                    সংরচনা IMAP *imap)
{
  CURLcode result = CURLE_OK;

  /* Check we have a query string */
  যদি(!imap->query) {
    failf(data, "Cannot SEARCH without a query string.");
    ঘূৰাই CURLE_URL_MALFORMAT;
  }

  /* Send the SEARCH command */
  result = imap_sendf(data, imapc, "SEARCH %s", imap->query);

  যদি(!result)
    imap_state(data, imapc, IMAP_অনুসন্ধান);

  ঘূৰাই result;
}

/***********************************************************************
 *
 * imap_perform_logout()
 *
 * Performs the logout action prior to sclose() being called.
 */
স্থির CURLcode imap_perform_logout(সংরচনা Curl_easy *data,
                                    সংরচনা imap_conn *imapc)
{
  /* Send the LOGOUT command */
  CURLcode result = imap_sendf(data, imapc, "LOGOUT");

  যদি(!result)
    imap_state(data, imapc, IMAP_LOGOUT);

  ঘূৰাই result;
}

/* For the initial server greeting */
স্থির CURLcode imap_state_servergreet_resp(সংরচনা Curl_easy *data,
                                            সংরচনা imap_conn *imapc,
                                            পূর্ণাঙ্ক imapcode,
                                            imapstate instate)
{
  (শূন্য)instate;

  যদি(imapcode == IMAP_RESP_PREAUTH) {
    /* PREAUTH */
    imapc->preauth = TRUE;
    infof(data, "PREAUTH connection, already authenticated");
  }
  অন্যথা যদি(imapcode != IMAP_RESP_OK) {
    failf(data, "Got unexpected imap-server response");
    ঘূৰাই CURLE_WEIRD_SERVER_REPLY;
  }

  ঘূৰাই imap_perform_capability(data, imapc);
}

/* For CAPABILITY responses */
স্থির CURLcode imap_state_capability_resp(সংরচনা Curl_easy *data,
                                           সংরচনা imap_conn *imapc,
                                           পূর্ণাঙ্ক imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  ধ্রুৱ ৱর্ণ *সাৰি = curlx_dyn_ptr(&imapc->pp.recvbuf);

  (শূন্য)instate;

  /* Do we have an untagged response? */
  যদি(imapcode == '*') {
    সাৰি += 2;

    /* Loop through the data line */
    বাবে(;;) {
      size_t wordlen;
      যেতিয়ালৈ(*সাৰি && (ISBLANK(*সাৰি) || ISNEWLINE(*সাৰি)))
        সাৰি++;

      যদি(!*সাৰি)
        ভঙ্গ;

      /* Extract the word */
      বাবে(wordlen = 0; সাৰি[wordlen] && !ISBLANK(সাৰি[wordlen]) &&
                       !ISNEWLINE(সাৰি[wordlen]);)
        wordlen++;

      /* Does the server support the STARTTLS capability? */
      যদি(wordlen == 8 && curl_strnequal(সাৰি, "STARTTLS", 8))
        imapc->tls_supported = TRUE;

      /* Has the server explicitly disabled clear text authentication? */
      অন্যথা যদি(wordlen == 13 && curl_strnequal(সাৰি, "LOGINDISABLED", 13))
        imapc->login_disabled = TRUE;

      /* Does the server support the SASL-IR capability? */
      অন্যথা যদি(wordlen == 7 && curl_strnequal(সাৰি, "SASL-IR", 7))
        imapc->ir_supported = TRUE;

      /* Do we have a SASL based authentication mechanism? */
      অন্যথা যদি(wordlen > 5 && curl_strnequal(সাৰি, "AUTH=", 5)) {
        size_t llen;
        unsigned short mechbit;

        সাৰি += 5;
        wordlen -= 5;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(সাৰি, wordlen, &llen);
        যদি(mechbit && llen == wordlen)
          imapc->sasl.authmechs |= mechbit;
      }

      সাৰি += wordlen;
    }
  }
  অন্যথা যদি(data->set.use_ssl && !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
    /* PREAUTH is not compatible with STARTTLS. */
    যদি(imapcode == IMAP_RESP_OK && imapc->tls_supported && !imapc->preauth) {
      /* Switch to TLS connection now */
      result = imap_perform_starttls(data, imapc);
    }
    অন্যথা যদি(data->set.use_ssl <= CURLUSESSL_TRY)
      result = imap_perform_authentication(data, imapc);
    অন্যথা {
      failf(data, "STARTTLS not available.");
      result = CURLE_USE_SSL_FAILED;
    }
  }
  অন্যথা
    result = imap_perform_authentication(data, imapc);

  ঘূৰাই result;
}

/* For STARTTLS responses */
স্থির CURLcode imap_state_starttls_resp(সংরচনা Curl_easy *data,
                                         সংরচনা imap_conn *imapc,
                                         পূর্ণাঙ্ক imapcode,
                                         imapstate instate)
{
  CURLcode result = CURLE_OK;

  (শূন্য)instate;

  /* Pipelining in response is forbidden. */
  যদি(imapc->pp.overflow)
    ঘূৰাই CURLE_WEIRD_SERVER_REPLY;

  যদি(imapcode != IMAP_RESP_OK) {
    যদি(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied");
      result = CURLE_USE_SSL_FAILED;
    }
    অন্যথা
      result = imap_perform_authentication(data, imapc);
  }
  অন্যথা
    imap_state(data, imapc, IMAP_UPGRADETLS);

  ঘূৰাই result;
}

/* For SASL authentication responses */
স্থির CURLcode imap_state_auth_resp(সংরচনা Curl_easy *data,
                                     সংরচনা imap_conn *imapc,
                                     পূর্ণাঙ্ক imapcode,
                                     imapstate instate)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  (শূন্য)instate;

  result = Curl_sasl_continue(&imapc->sasl, data, imapcode, &progress);
  যদি(!result)
    ৱিকল্প(progress) {
    পক্ষ SASL_DONE:
      imap_state(data, imapc, IMAP_STOP);  /* Authenticated */
      ভঙ্গ;
    পক্ষ SASL_IDLE:            /* No mechanism left after cancellation */
      যদি(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
        /* Perform clear text authentication */
        result = imap_perform_login(data, imapc, data->conn);
      অন্যথা {
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
      }
      ভঙ্গ;
    স্ৱভাৱ:
      ভঙ্গ;
    }

  ঘূৰাই result;
}

/* For LOGIN responses */
স্থির CURLcode imap_state_login_resp(সংরচনা Curl_easy *data,
                                      সংরচনা imap_conn *imapc,
                                      পূর্ণাঙ্ক imapcode,
                                      imapstate instate)
{
  CURLcode result = CURLE_OK;
  (শূন্য)instate;

  যদি(imapcode != IMAP_RESP_OK) {
    failf(data, "Access denied. %c", imapcode);
    result = CURLE_LOGIN_DENIED;
  }
  অন্যথা
    /* End of connect phase */
    imap_state(data, imapc, IMAP_STOP);

  ঘূৰাই result;
}

/* Detect IMAP listings vs. downloading a single email */
স্থির bool is_custom_fetch_listing_match(ধ্রুৱ ৱর্ণ *params)
{
  /* match " 1:* (FLAGS ..." or " 1,2,3 (FLAGS ..." */
  যদি(*params++ != ' ')
    ঘূৰাই FALSE;

  যেতিয়ালৈ(ISDIGIT(*params)) {
    params++;
    যদি(*params == 0)
      ঘূৰাই FALSE;
  }
  যদি(*params == ':')
    ঘূৰাই TRUE;
  যদি(*params == ',')
    ঘূৰাই TRUE;
  ঘূৰাই FALSE;
}

স্থির bool is_custom_fetch_listing(সংরচনা IMAP *imap)
{
  /* filter out "UID FETCH 1:* (FLAGS ..." queries to list emails */
  যদি(!imap->custom)
    ঘূৰাই FALSE;
  অন্যথা যদি(curl_strequal(imap->custom, "FETCH") && imap->custom_params) {
    ধ্রুৱ ৱর্ণ *p = imap->custom_params;
    ঘূৰাই is_custom_fetch_listing_match(p);
  }
  অন্যথা যদি(curl_strequal(imap->custom, "UID") && imap->custom_params &&
          curl_strnequal(imap->custom_params, " FETCH ", 7)) {
    ধ্রুৱ ৱর্ণ *p = imap->custom_params + 6;
    ঘূৰাই is_custom_fetch_listing_match(p);
  }
  ঘূৰাই FALSE;
}

/* For LIST and SEARCH responses */
স্থির CURLcode imap_state_listsearch_resp(সংরচনা Curl_easy *data,
                                           সংরচনা imap_conn *imapc,
                                           পূর্ণাঙ্ক imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  ধ্রুৱ ৱর্ণ *সাৰি = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  সংরচনা IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);

  DEBUGASSERT(imap);
  যদি(!imap)
    ঘূৰাই CURLE_FAILED_INIT;
  (শূন্য)instate;

  যদি(imapcode == '*' && is_custom_fetch_listing(imap)) {
    /* custom FETCH or UID FETCH for listing is not handled here */
  }
  অন্যথা যদি(imapcode == '*') {
    /* Check if this response contains a literal (e.g. FETCH responses with
       body data). Literal syntax is {size}\r\n */
    ধ্রুৱ ৱর্ণ *cr = memchr(সাৰি, '\r', len);
    size_t সাৰি_len = cr ? (size_t)(cr - সাৰি) : len;
    ধ্রুৱ ৱর্ণ *ptr = imap_find_literal(সাৰি, সাৰি_len);
    যদি(ptr) {
      curl_off_t size = 0;
      bool parsed = FALSE;
      ptr++;
      যদি(!curlx_str_সংখ্যা(&ptr, &size, CURL_OFF_T_MAX) &&
         !curlx_str_single(&ptr, '}'))
        parsed = TRUE;

      যদি(parsed) {
        সংরচনা pingpong *pp = &imapc->pp;
        size_t buffer_len = curlx_dyn_len(&pp->recvbuf);
        size_t after_header = buffer_len - pp->nfinal;

        /* This is a literal response, setup to receive the body data */
        infof(data, "Found %" FMT_OFF_T " bytes to download", size);

        /* First write the header line */
        result = Curl_client_write(data, CLIENTWRITE_BODY, সাৰি, len);
        যদি(result)
          ঘূৰাই result;

        /* Handle data already in buffer after the header line */
        যদি(after_header > 0) {
          /* There is already data in the buffer that is part of the literal
             body or subsequent responses */
          size_t chunk = after_header;

          /* Keep only the data after the header line */
          curlx_dyn_tail(&pp->recvbuf, chunk);
          pp->nfinal = 0; /* done */

          /* Limit chunk to the literal size */
          যদি((curl_off_t)chunk > size)
            chunk = (size_t)size;

          যদি(chunk) {
            /* Write the literal body data */
            result = Curl_client_write(data, CLIENTWRITE_BODY,
                                       curlx_dyn_ptr(&pp->recvbuf), chunk);
            যদি(result)
              ঘূৰাই result;
          }

          /* Handle remaining data in buffer (either more literal data or
             subsequent responses) */
          যদি(after_header > chunk) {
            /* Keep the data after the literal body */
            pp->overflow = after_header - chunk;
            curlx_dyn_tail(&pp->recvbuf, pp->overflow);
          }
          অন্যথা {
            pp->overflow = 0;
            curlx_dyn_reset(&pp->recvbuf);
          }
        }
        অন্যথা {
          /* No data in buffer yet, reset overflow */
          pp->overflow = 0;
        }

        যদি((CURL_OFF_T_MAX - size) < (curl_off_t)len)
          /* unlikely to actually be a transfer this big, but avoid integer
             overflow */
          size = CURL_OFF_T_MAX;
        অন্যথা
          size += len;

        /* Progress size includes both header line and literal body */
        Curl_pgrsSetDownloadSize(data, size);

        যদি(data->req.bytecount == size)
          /* All data already transferred (header + literal body) */
          Curl_xfer_setup_nop(data);
        অন্যথা {
          /* Setup to receive the literal body data.
             maxdownload and transfer size include both header line and
             literal body */
          data->req.maxdownload = size;
          Curl_xfer_setup_recv(data, FIRSTSOCKET, size);
        }
        /* End of DO phase */
        imap_state(data, imapc, IMAP_STOP);
      }
      অন্যথা {
        /* Failed to parse literal, write the line */
        result = Curl_client_write(data, CLIENTWRITE_BODY, সাৰি, len);
      }
    }
    অন্যথা {
      /* No literal, write the line as-is */
      result = Curl_client_write(data, CLIENTWRITE_BODY, সাৰি, len);
    }
  }
  অন্যথা যদি(imapcode != IMAP_RESP_OK)
    result = CURLE_QUOTE_ERROR;
  অন্যথা
    /* End of DO phase */
    imap_state(data, imapc, IMAP_STOP);

  ঘূৰাই result;
}

/* For SELECT responses */
স্থির CURLcode imap_state_select_resp(সংরচনা Curl_easy *data,
                                       সংরচনা imap_conn *imapc,
                                       সংরচনা IMAP *imap,
                                       পূর্ণাঙ্ক imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  (শূন্য)instate;

  যদি(imapcode == '*') {
    /* See if this is an UIDVALIDITY response */
    ধ্রুৱ ৱর্ণ *সাৰি = curlx_dyn_ptr(&imapc->pp.recvbuf);
    size_t len = curlx_dyn_len(&imapc->pp.recvbuf);
    যদি((len >= 18) && checkprefix("OK [UIDVALIDITY ", &সাৰি[2])) {
      curl_off_t মান;
      ধ্রুৱ ৱর্ণ *p = &সাৰি[2] + CURL_CSTRLEN("OK [UIDVALIDITY ");
      যদি(!curlx_str_সংখ্যা(&p, &মান, UINT_MAX)) {
        imapc->mb_uidvalidity = (unsigned পূর্ণাঙ্ক)মান;
        imapc->mb_uidvalidity_set = TRUE;
      }
    }
  }
  অন্যথা যদি(imapcode == IMAP_RESP_OK) {
    /* Check if the UIDVALIDITY has been specified and matches */
    যদি(imap->uidvalidity_set && imapc->mb_uidvalidity_set &&
       (imap->uidvalidity != imapc->mb_uidvalidity)) {
      failf(data, "Mailbox UIDVALIDITY has changed");
      result = CURLE_REMOTE_FILE_NOT_FOUND;
    }
    অন্যথা {
      /* Note the currently opened mailbox on this connection */
      DEBUGASSERT(!imapc->mailbox);
      imapc->mailbox = curlx_strdup(imap->mailbox);
      যদি(!imapc->mailbox)
        ঘূৰাই CURLE_OUT_OF_MEMORY;

      যদি(imap->custom)
        result = imap_perform_list(data, imapc, imap);
      অন্যথা যদি(imap->query)
        result = imap_perform_অনুসন্ধান(data, imapc, imap);
      অন্যথা
        result = imap_perform_fetch(data, imapc, imap);
    }
  }
  অন্যথা {
    failf(data, "Select failed");
    result = CURLE_LOGIN_DENIED;
  }

  ঘূৰাই result;
}

/* For the (first line of the) FETCH responses */
স্থির CURLcode imap_state_fetch_resp(সংরচনা Curl_easy *data,
                                      সংরচনা imap_conn *imapc,
                                      পূর্ণাঙ্ক imapcode,
                                      imapstate instate)
{
  CURLcode result = CURLE_OK;
  সংরচনা pingpong *pp = &imapc->pp;
  ধ্রুৱ ৱর্ণ *ptr = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  bool parsed = FALSE;
  curl_off_t size = 0;

  (শূন্য)instate;

  যদি(imapcode != '*') {
    Curl_pgrsSetDownloadSize(data, -1);
    imap_state(data, imapc, IMAP_STOP);
    ঘূৰাই CURLE_REMOTE_FILE_NOT_FOUND;
  }

  /* Something like this is received "* 1 FETCH (BODY[TEXT] {2021}\r" so parse
     the continuation data contained within the curly brackets */
  ptr = imap_find_literal(ptr, len);
  যদি(ptr) {
    ptr++;
    যদি(!curlx_str_সংখ্যা(&ptr, &size, CURL_OFF_T_MAX) &&
       !curlx_str_single(&ptr, '}'))
      parsed = TRUE;
  }

  যদি(parsed) {
    infof(data, "Found %" FMT_OFF_T " bytes to download", size);
    Curl_pgrsSetDownloadSize(data, size);

    যদি(pp->overflow) {
      /* At this point there is a data in the receive buffer that is body
         content, send it as body and then skip it. Do note that there may
         even be additional "headers" after the body. */
      size_t chunk = pp->overflow;

      /* keep only the overflow */
      curlx_dyn_tail(&pp->recvbuf, chunk);
      pp->nfinal = 0; /* done */

      যদি((curl_off_t)chunk > size)
        /* The conversion from curl_off_t to size_t is always fine here */
        chunk = (size_t)size;

      যদি(!chunk) {
        /* no size, we are done with the data */
        imap_state(data, imapc, IMAP_STOP);
        ঘূৰাই CURLE_OK;
      }
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 curlx_dyn_ptr(&pp->recvbuf), chunk);
      যদি(result)
        ঘূৰাই result;

      infof(data, "Written %zu bytes, %" FMT_OFF_T
            " bytes are left for transfer", chunk, (curl_off_t)(size - chunk));

      /* Have we used the entire overflow or part of it?*/
      যদি(pp->overflow > chunk) {
        /* remember the remaining trailing overflow data */
        pp->overflow -= chunk;
        curlx_dyn_tail(&pp->recvbuf, pp->overflow);
      }
      অন্যথা {
        pp->overflow = 0; /* handled */
        /* Free the cache */
        curlx_dyn_reset(&pp->recvbuf);
      }
    }

    যদি(data->req.bytecount == size)
      /* The entire data is already transferred! */
      Curl_xfer_setup_nop(data);
    অন্যথা {
      /* IMAP download */
      data->req.maxdownload = size;
      Curl_xfer_setup_recv(data, FIRSTSOCKET, size);
    }
  }
  অন্যথা {
    /* We do not know how to parse this line */
    failf(data, "Failed to parse FETCH response.");
    result = CURLE_WEIRD_SERVER_REPLY;
  }

  /* End of DO phase */
  imap_state(data, imapc, IMAP_STOP);

  ঘূৰাই result;
}

/* For final FETCH responses performed after the download */
স্থির CURLcode imap_state_fetch_final_resp(সংরচনা Curl_easy *data,
                                            সংরচনা imap_conn *imapc,
                                            পূর্ণাঙ্ক imapcode,
                                            imapstate instate)
{
  CURLcode result = CURLE_OK;

  (শূন্য)instate;

  যদি(imapcode != IMAP_RESP_OK)
    result = CURLE_WEIRD_SERVER_REPLY;
  অন্যথা
    /* End of DONE phase */
    imap_state(data, imapc, IMAP_STOP);

  ঘূৰাই result;
}

/* For APPEND responses */
স্থির CURLcode imap_state_append_resp(সংরচনা Curl_easy *data,
                                       সংরচনা imap_conn *imapc,
                                       পূর্ণাঙ্ক imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  (শূন্য)instate;

  যদি(imapcode != '+') {
    result = CURLE_UPLOAD_FAILED;
  }
  অন্যথা {
    /* Set the progress upload size */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* IMAP upload */
    Curl_xfer_setup_send(data, FIRSTSOCKET);

    /* End of DO phase */
    imap_state(data, imapc, IMAP_STOP);
  }

  ঘূৰাই result;
}

/* For final APPEND responses performed after the upload */
স্থির CURLcode imap_state_append_final_resp(সংরচনা Curl_easy *data,
                                             সংরচনা imap_conn *imapc,
                                             পূর্ণাঙ্ক imapcode,
                                             imapstate instate)
{
  CURLcode result = CURLE_OK;

  (শূন্য)instate;

  যদি(imapcode != IMAP_RESP_OK)
    result = CURLE_UPLOAD_FAILED;
  অন্যথা
    /* End of DONE phase */
    imap_state(data, imapc, IMAP_STOP);

  ঘূৰাই result;
}

স্থির CURLcode imap_pp_statemachine(সংরচনা Curl_easy *data,
                                     সংরচনা connectdata *conn)
{
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক imapcode;
  সংরচনা imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  সংরচনা IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  সংরচনা pingpong *pp;
  size_t nread = 0;

  যদি(!imapc || !imap)
    ঘূৰাই CURLE_FAILED_INIT;
  pp = &imapc->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not IMAP */
upgrade_tls:
  যদি(imapc->state == IMAP_UPGRADETLS) {
    result = imap_perform_upgrade_tls(data, imapc, conn);
    যদি(result || (imapc->state == IMAP_UPGRADETLS))
      ঘূৰাই result;
  }

  /* Flush any data that needs to be sent */
  যদি(pp->sendleft)
    ঘূৰাই Curl_pp_flushsend(data, pp);

  কৰা {
    /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, pp, &imapcode, &nread);
    যদি(result)
      ঘূৰাই result;

    /* Was there an error parsing the response line? */
    যদি(imapcode == -1)
      ঘূৰাই CURLE_WEIRD_SERVER_REPLY;

    যদি(!imapcode)
      ভঙ্গ;

    /* We have now received a full IMAP server response */
    ৱিকল্প(imapc->state) {
    পক্ষ IMAP_SERVERGREET:
      result = imap_state_servergreet_resp(data, imapc,
                                           imapcode, imapc->state);
      ভঙ্গ;

    পক্ষ IMAP_CAPABILITY:
      result = imap_state_capability_resp(data, imapc, imapcode, imapc->state);
      ভঙ্গ;

    পক্ষ IMAP_STARTTLS:
      result = imap_state_starttls_resp(data, imapc, imapcode, imapc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      যদি(!result && (imapc->state == IMAP_UPGRADETLS))
        গচ্ছ upgrade_tls;
      ভঙ্গ;

    পক্ষ IMAP_AUTHENTICATE:
      result = imap_state_auth_resp(data, imapc, imapcode, imapc->state);
      ভঙ্গ;

    পক্ষ IMAP_LOGIN:
      result = imap_state_login_resp(data, imapc, imapcode, imapc->state);
      ভঙ্গ;

    পক্ষ IMAP_LIST:
    পক্ষ IMAP_অনুসন্ধান:
      result = imap_state_listsearch_resp(data, imapc, imapcode, imapc->state);
      ভঙ্গ;

    পক্ষ IMAP_SELECT:
      result = imap_state_select_resp(data, imapc, imap,
                                      imapcode, imapc->state);
      ভঙ্গ;

    পক্ষ IMAP_FETCH:
      result = imap_state_fetch_resp(data, imapc, imapcode, imapc->state);
      ভঙ্গ;

    পক্ষ IMAP_FETCH_FINAL:
      result = imap_state_fetch_final_resp(data, imapc,
                                           imapcode, imapc->state);
      ভঙ্গ;

    পক্ষ IMAP_APPEND:
      result = imap_state_append_resp(data, imapc, imapcode, imapc->state);
      ভঙ্গ;

    পক্ষ IMAP_APPEND_FINAL:
      result = imap_state_append_final_resp(data, imapc,
                                            imapcode, imapc->state);
      ভঙ্গ;

    পক্ষ IMAP_LOGOUT:
    স্ৱভাৱ:
      /* internal error */
      imap_state(data, imapc, IMAP_STOP);
      ভঙ্গ;
    }
  } যেতিয়ালৈ(!result && imapc->state != IMAP_STOP && Curl_pp_moredata(pp));

  ঘূৰাই result;
}

/* Called repeatedly until done from multi.c */
স্থির CURLcode imap_multi_statemach(সংরচনা Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  সংরচনা imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  *done = FALSE;
  যদি(!imapc)
    ঘূৰাই CURLE_FAILED_INIT;
  result = Curl_pp_statemach(data, &imapc->pp, FALSE, FALSE);
  *done = (imapc->state == IMAP_STOP);

  ঘূৰাই result;
}

স্থির CURLcode imap_block_statemach(সংরচনা Curl_easy *data,
                                     সংরচনা imap_conn *imapc,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;

  যেতিয়ালৈ(imapc->state != IMAP_STOP && !result)
    result = Curl_pp_statemach(data, &imapc->pp, TRUE, disconnecting);

  ঘূৰাই result;
}

/* For the IMAP "protocol connect" and "doing" phases only */
স্থির CURLcode imap_pollset(সংরচনা Curl_easy *data,
                             সংরচনা easy_pollset *ps)
{
  সংরচনা imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  ঘূৰাই imapc ? Curl_pp_pollset(data, &imapc->pp, ps) : CURLE_OK;
}

স্থির শূন্য imap_easy_reset(সংরচনা IMAP *imap)
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
স্থির bool imap_is_bchar(ৱর্ণ ch)
{
  /* Performing the alnum check first with macro is faster because of ASCII
     arithmetic */
  ঘূৰাই ch && (ISALNUM(ch) || strchr(":@/&=-._~!$\'()*+,%", ch));
}

/***********************************************************************
 *
 * imap_parse_url_options()
 *
 * Parse the URL login options.
 */
স্থির CURLcode imap_parse_url_options(সংরচনা connectdata *conn,
                                       সংরচনা imap_conn *imapc)
{
  CURLcode result = CURLE_OK;
  ধ্রুৱ ৱর্ণ *ptr = conn->options;
  bool prefer_login = FALSE;

  যেতিয়ালৈ(!result && ptr && *ptr) {
    ধ্রুৱ ৱর্ণ *চাবি = ptr;
    ধ্রুৱ ৱর্ণ *মান;

    যেতিয়ালৈ(*ptr && *ptr != '=')
      ptr++;

    মান = ptr + 1;

    যেতিয়ালৈ(*ptr && *ptr != ';')
      ptr++;

    যদি(curl_strnequal(চাবি, "AUTH=+LOGIN", 11)) {
      /* User prefers plaintext LOGIN over any SASL, including SASL LOGIN */
      prefer_login = TRUE;
      imapc->sasl.prefmech = SASL_AUTH_NONE;
    }
    অন্যথা যদি(curl_strnequal(চাবি, "AUTH=", 5)) {
      prefer_login = FALSE;
      result = Curl_sasl_parse_url_auth_option(&imapc->sasl,
                                               মান, ptr - মান);
    }
    অন্যথা {
      prefer_login = FALSE;
      result = CURLE_URL_MALFORMAT;
    }

    যদি(*ptr == ';')
      ptr++;
  }

  যদি(prefer_login)
    imapc->preftype = IMAP_TYPE_CLEARTEXT;
  অন্যথা {
    ৱিকল্প(imapc->sasl.prefmech) {
    পক্ষ SASL_AUTH_NONE:
      imapc->preftype = IMAP_TYPE_NONE;
      ভঙ্গ;
    পক্ষ SASL_AUTH_DEFAULT:
      imapc->preftype = IMAP_TYPE_ANY;
      ভঙ্গ;
    স্ৱভাৱ:
      imapc->preftype = IMAP_TYPE_SASL;
      ভঙ্গ;
    }
  }

  ঘূৰাই result;
}

/***********************************************************************
 *
 * imap_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
স্থির CURLcode imap_parse_url_path(সংরচনা Curl_easy *data,
                                    সংরচনা IMAP *imap)
{
  /* The imap struct is already initialized in imap_connect() */
  CURLcode result = CURLE_OK;
  ধ্রুৱ ৱর্ণ *begin = &data->state.up.path[1]; /* skip leading slash */
  ধ্রুৱ ৱর্ণ *ptr = begin;

  /* See how much of the URL is a valid path and decode it */
  যেতিয়ালৈ(imap_is_bchar(*ptr))
    ptr++;

  যদি(ptr != begin) {
    /* Remove the trailing slash if present */
    ধ্রুৱ ৱর্ণ *end = ptr;
    যদি(end > begin && end[-1] == '/')
      end--;

    result = Curl_urldecode(begin, end - begin, &imap->mailbox, রিক্ত,
                            REJECT_CTRL);
    যদি(result)
      ঘূৰাই result;
  }
  অন্যথা
    imap->mailbox = রিক্ত;

  /* There can be any number of parameters in the form ";NAME=VALUE" */
  যেতিয়ালৈ(*ptr == ';') {
    ৱর্ণ *name;
    ৱর্ণ *মান;
    size_t valuelen;

    /* Find the length of the name parameter */
    begin = ++ptr;
    যেতিয়ালৈ(*ptr && *ptr != '=')
      ptr++;

    যদি(!*ptr)
      ঘূৰাই CURLE_URL_MALFORMAT;

    /* Decode the name parameter */
    result = Curl_urldecode(begin, ptr - begin, &name, রিক্ত,
                            REJECT_CTRL);
    যদি(result)
      ঘূৰাই result;

    /* Find the length of the value parameter */
    begin = ++ptr;
    যেতিয়ালৈ(imap_is_bchar(*ptr))
      ptr++;

    /* Decode the value parameter */
    result = Curl_urldecode(begin, ptr - begin, &মান, &valuelen,
                            REJECT_CTRL);
    যদি(result) {
      curlx_free(name);
      ঘূৰাই result;
    }

    DEBUGF(infof(data, "IMAP URL parameter '%s' = '%s'", name, মান));

    /* Process the known hierarchical parameters (UIDVALIDITY, UID, SECTION
       and PARTIAL) stripping of the trailing slash character if it is
       present.

       Note: Unknown parameters trigger a URL_MALFORMAT error. */
    যদি(valuelen > 0 && মান[valuelen - 1] == '/')
      মান[valuelen - 1] = '\0';
    যদি(valuelen) {
      যদি(curl_strequal(name, "UIDVALIDITY") && !imap->uidvalidity_set) {
        curl_off_t অঙ্ক;
        ধ্রুৱ ৱর্ণ *p = (ধ্রুৱ ৱর্ণ *)মান;
        যদি(!curlx_str_সংখ্যা(&p, &অঙ্ক, UINT_MAX)) {
          imap->uidvalidity = (unsigned পূর্ণাঙ্ক)অঙ্ক;
          imap->uidvalidity_set = TRUE;
        }
        curlx_free(মান);
      }
      অন্যথা যদি(curl_strequal(name, "UID") && !imap->uid) {
        imap->uid = মান;
      }
      অন্যথা যদি(curl_strequal(name, "MAILINDEX") && !imap->mindex) {
        imap->mindex = মান;
      }
      অন্যথা যদি(curl_strequal(name, "SECTION") && !imap->section) {
        imap->section = মান;
      }
      অন্যথা যদি(curl_strequal(name, "PARTIAL") && !imap->partial) {
        imap->partial = মান;
      }
      অন্যথা {
        curlx_free(name);
        curlx_free(মান);
        ঘূৰাই CURLE_URL_MALFORMAT;
      }
    }
    অন্যথা
      /* blank? */
      curlx_free(মান);
    curlx_free(name);
  }

  /* Does the URL contain a query parameter? Only valid when we have a mailbox
     and no UID as per RFC-5092 */
  যদি(imap->mailbox && !imap->uid && !imap->mindex) {
    /* Get the query parameter, URL decoded */
    CURLUcode uc = curl_url_get(data->state.uh, CURLUPART_QUERY, &imap->query,
                                CURLU_URLDECODE);
    যদি(uc == CURLUE_OUT_OF_MEMORY)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
  }

  /* Any extra stuff at the end of the URL is an error */
  যদি(*ptr)
    ঘূৰাই CURLE_URL_MALFORMAT;

  ঘূৰাই CURLE_OK;
}

/***********************************************************************
 *
 * imap_parse_custom_request()
 *
 * Parse the custom request.
 */
স্থির CURLcode imap_parse_custom_request(সংরচনা Curl_easy *data,
                                          সংরচনা IMAP *imap)
{
  CURLcode result = CURLE_OK;
  ধ্রুৱ ৱর্ণ *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  যদি(custom) {
    /* URL decode the custom request */
    result = Curl_urldecode(custom, 0, &imap->custom, রিক্ত, REJECT_CTRL);

    /* Extract the parameters if specified */
    যদি(!result) {
      ধ্রুৱ ৱর্ণ *params = imap->custom;

      যেতিয়ালৈ(*params && *params != ' ')
        params++;

      যদি(*params) {
        imap->custom_params = curlx_strdup(params);
        imap->custom[params - imap->custom] = '\0';

        যদি(!imap->custom_params)
          result = CURLE_OUT_OF_MEMORY;
      }
    }
  }

  ঘূৰাই result;
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
স্থির CURLcode imap_connect(সংরচনা Curl_easy *data, bool *done)
{
  সংরচনা imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  CURLcode result = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  যদি(!imapc)
    ঘূৰাই CURLE_FAILED_INIT;

  /* Parse the URL options */
  result = imap_parse_url_options(data->conn, imapc);
  যদি(result)
    ঘূৰাই result;

  /* Start off waiting for the server greeting response */
  imap_state(data, imapc, IMAP_SERVERGREET);

  /* Start off with an response id of '*' */
  curlx_strcopy(imapc->resptag, পরিমাণ(imapc->resptag), STRCONST("*"));

  result = imap_multi_statemach(data, done);

  ঘূৰাই result;
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
স্থির CURLcode imap_done(সংরচনা Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  সংরচনা IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);

  যদি(!imapc)
    ঘূৰাই CURLE_FAILED_INIT;
  যদি(!imap)
    ঘূৰাই CURLE_OK;

  যদি(status || premature) {
    CURL_TRC_M(data, "IMAP done, not finalizing transfer");
    connclose(conn); /* marked for closure */
    result = status;         /* use the already set error code */
  }
  অন্যথা যদি(!data->set.connect_only &&
          ((!imap->custom && (imap->uid || imap->mindex)) ||
           (imap->custom && data->req.maxdownload > 0) ||
           data->state.upload || IS_MIME_POST(data))) {
    /* Handle responses after FETCH or APPEND transfer has finished.
       For custom commands, check if we set up a download which indicates
       a FETCH-like command with literal data. */

    যদি(!data->state.upload && !IS_MIME_POST(data))
      imap_state(data, imapc, IMAP_FETCH_FINAL);
    অন্যথা {
      /* End the APPEND command first by sending an empty line */
      result = Curl_pp_sendf(data, &imapc->pp, "%s", "");
      যদি(!result)
        imap_state(data, imapc, IMAP_APPEND_FINAL);
    }

    /* Run the state-machine */
    যদি(!result)
      result = imap_block_statemach(data, imapc, FALSE);
  }

  imap_easy_reset(imap);
  ঘূৰাই result;
}

/***********************************************************************
 *
 * imap_perform()
 *
 * This is the actual DO function for IMAP. Fetch or append a message, or do
 * other things according to the options previously setup.
 */
স্থির CURLcode imap_perform(সংরচনা Curl_easy *data, bool *connected,
                             bool *dophase_done)
{
  /* This is IMAP and no proxy */
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  সংরচনা IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  bool selected = FALSE;

  DEBUGF(infof(data, "DO phase starts"));
  যদি(!imapc || !imap)
    ঘূৰাই CURLE_FAILED_INIT;

  যদি(data->req.no_body) {
    /* Requested no body means no transfer */
    imap->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* Determine if the requested mailbox (with the same UIDVALIDITY if set)
     has already been selected on this connection */
  যদি(imap->mailbox && imapc->mailbox &&
     (!strcmp(imap->mailbox, imapc->mailbox) ||
      (curl_strequal(imap->mailbox, "INBOX") &&
       curl_strequal(imapc->mailbox, "INBOX"))) &&
     (!imap->uidvalidity_set || !imapc->mb_uidvalidity_set ||
      (imap->uidvalidity == imapc->mb_uidvalidity)))
    selected = TRUE;

  /* Start the first command in the DO phase */
  যদি(data->state.upload || IS_MIME_POST(data))
    /* APPEND can be executed directly */
    result = imap_perform_append(data, imapc, imap);
  অন্যথা যদি(imap->custom && (selected || !imap->mailbox))
    /* Custom command using the same mailbox or no mailbox */
    result = imap_perform_list(data, imapc, imap);
  অন্যথা যদি(!imap->custom && selected && (imap->uid || imap->mindex))
    /* FETCH from the same mailbox */
    result = imap_perform_fetch(data, imapc, imap);
  অন্যথা যদি(!imap->custom && selected && imap->query)
    /* SEARCH the current mailbox */
    result = imap_perform_অনুসন্ধান(data, imapc, imap);
  অন্যথা যদি(imap->mailbox && !selected &&
          (imap->custom || imap->uid || imap->mindex || imap->query))
    /* SELECT the mailbox */
    result = imap_perform_select(data, imapc, imap);
  অন্যথা
    /* LIST */
    result = imap_perform_list(data, imapc, imap);

  যদি(result)
    ঘূৰাই result;

  /* Run the state-machine */
  result = imap_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(conn, FIRSTSOCKET);

  যদি(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  ঘূৰাই result;
}

/* Call this when the DO phase has completed */
স্থির CURLcode imap_dophase_done(সংরচনা Curl_easy *data,
                                  সংরচনা IMAP *imap,
                                  bool connected)
{
  (শূন্য)connected;

  যদি(imap->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);

  ঘূৰাই CURLE_OK;
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
স্থির CURLcode imap_regular_transfer(সংরচনা Curl_easy *data,
                                      সংরচনা IMAP *imap,
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
  যদি(!result && *dophase_done)
    result = imap_dophase_done(data, imap, connected);

  ঘূৰাই result;
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
স্থির CURLcode imap_do(সংরচনা Curl_easy *data, bool *done)
{
  সংরচনা IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* default to false */

  যদি(!imap)
    ঘূৰাই CURLE_FAILED_INIT;
  /* Parse the URL path */
  result = imap_parse_url_path(data, imap);
  যদি(result)
    ঘূৰাই result;

  /* Parse the custom request */
  result = imap_parse_custom_request(data, imap);
  যদি(result)
    ঘূৰাই result;

  result = imap_regular_transfer(data, imap, done);

  ঘূৰাই result;
}

/***********************************************************************
 *
 * imap_disconnect()
 *
 * Disconnect from an IMAP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
স্থির CURLcode imap_disconnect(সংরচনা Curl_easy *data,
                                সংরচনা connectdata *conn, bool dead_connection)
{
  সংরচনা imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);

  যদি(imapc &&
     /* We cannot send quit unconditionally. If this connection is stale or
        bad in any way (pingpong has pending data to send),
        sending quit and waiting around here will make the
        disconnect wait in vain and cause more problems than we need to. */
     !dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &imapc->pp) &&
     !imap_perform_logout(data, imapc))
    (শূন্য)imap_block_statemach(data, imapc, TRUE); /* ignore errors */

  ঘূৰাই CURLE_OK;
}

/* Called from multi.c while DOing */
স্থির CURLcode imap_doing(সংরচনা Curl_easy *data, bool *dophase_done)
{
  সংরচনা IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  CURLcode result;

  যদি(!imap)
    ঘূৰাই CURLE_FAILED_INIT;

  result = imap_multi_statemach(data, dophase_done);
  যদি(result)
    DEBUGF(infof(data, "DO phase failed"));
  অন্যথা যদি(*dophase_done) {
    result = imap_dophase_done(data, imap, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  ঘূৰাই result;
}

স্থির শূন্য imap_easy_dtor(ধ্রুৱ শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা IMAP *imap = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  imap_easy_reset(imap);
  curlx_free(imap);
}

স্থির শূন্য imap_conn_dtor(ধ্রুৱ শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা imap_conn *imapc = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  Curl_pp_disconnect(&imapc->pp);
  curlx_safefree(imapc->mailbox);
  curlx_free(imapc);
}

/* SASL parameters for the imap protocol */
স্থির ধ্রুৱ সংরচনা SASLproto saslimap = {
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

স্থির CURLcode imap_setup_connection(সংরচনা Curl_easy *data,
                                      সংরচনা connectdata *conn)
{
  সংরচনা imap_conn *imapc;
  সংরচনা pingpong *pp;
  সংরচনা IMAP *imap;

  imapc = curlx_calloc(1, পরিমাণ(*imapc));
  যদি(!imapc)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  pp = &imapc->pp;
  PINGPONG_SETUP(pp, imap_pp_statemachine, imap_endofresp);

  /* Set the default preferred authentication type and mechanism */
  imapc->preftype = IMAP_TYPE_ANY;
  Curl_sasl_init(&imapc->sasl, data, &saslimap);

  Curl_pp_init(pp, Curl_pgrs_now(data));

  যদি(Curl_conn_meta_set(conn, CURL_META_IMAP_CONN, imapc, imap_conn_dtor))
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  imap = curlx_calloc(1, পরিমাণ(সংরচনা IMAP));
  যদি(!imap ||
     Curl_meta_set(data, CURL_META_IMAP_EASY, imap, imap_easy_dtor))
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  ঘূৰাই CURLE_OK;
}

/*
 * IMAP protocol.
 */
ধ্রুৱ সংরচনা Curl_protocol Curl_protocol_imap = {
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
