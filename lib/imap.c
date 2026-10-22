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
#অন্তর্ভাব "curl_setup.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "imap.h"

#ifndef CURL_DISABLE_IMAP

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef __VMS
#অন্তর্ভাব <in.h>
#অন্তর্ভাব <inet.h>
#endif

#অন্তর্ভাব "curlx/dynbuf.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "pingpong.h"
#অন্তর্ভাব "mime.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "bufref.h"
#অন্তর্ভাব "curl_sasl.h"
#অন্তর্ভাব "curlx/strcopy.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_IMAP_EASY   "meta:proto:imap:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_IMAP_CONN   "meta:proto:imap:conn"

প্রকারলেখ গণনা {
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
কাঠামো imap_conn {
  কাঠামো pingpong pp;
  কাঠামো SASL sasl;           /* SASL-related parameters */
  অক্ষর *mailbox;              /* The last selected mailbox */
  imapstate state;            /* Always use imap.c:state() to change state! */
  unsigned পূর্ণসংখ্যা mb_uidvalidity; /* UIDVALIDITY parsed from select response */
  অক্ষর resptag[5];            /* Response tag to wait for */
  unsigned অক্ষর preftype;     /* Preferred authentication type */
  unsigned অক্ষর cmdid;        /* Last used command ID */
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
কাঠামো IMAP {
  curl_pp_transfer transfer;
  অক্ষর *mailbox;          /* Mailbox to select */
  অক্ষর *uid;              /* Message UID to fetch */
  অক্ষর *mindex;           /* Index in mail box of mail to fetch */
  অক্ষর *section;          /* Message SECTION to fetch */
  অক্ষর *partial;          /* Message PARTIAL to fetch */
  অক্ষর *query;            /* Query to search for */
  অক্ষর *custom;           /* Custom request */
  অক্ষর *custom_params;    /* Parameters for the custom request */
  unsigned পূর্ণসংখ্যা uidvalidity; /* UIDVALIDITY to check in select */
  BIT(uidvalidity_set);
};

#define IMAP_RESP_OK       1
#define IMAP_RESP_NOT_OK   2
#define IMAP_RESP_PREAUTH  3

কাঠামো ulbits {
  পূর্ণসংখ্যা bit;
  ধ্রুবক অক্ষর *flag;
};

/***********************************************************************
 *
 * imap_sendf()
 *
 * Sends the formatted string as an IMAP command to the server.
 *
 * Designed to never block.
 */
স্থির CURLcode imap_sendf(কাঠামো Curl_easy *তথ্য,
                           কাঠামো imap_conn *imapc,
                           ধ্রুবক অক্ষর *fmt, ...) CURL_PRINTF(3, 4);
স্থির CURLcode imap_sendf(কাঠামো Curl_easy *তথ্য,
                           কাঠামো imap_conn *imapc,
                           ধ্রুবক অক্ষর *fmt, ...)
{
  CURLcode ফলাফল = CURLE_OK;
  va_list ap;

  DEBUGASSERT(fmt);

  /* Calculate the tag based on the connection ID and command ID */
  curl_msnprintf(imapc->resptag, আকার(imapc->resptag), "%c%03d",
                 'A' + curlx_sltosi((long)(তথ্য->conn->connection_id % 26)),
                 ++imapc->cmdid);

  va_start(ap, fmt);
  ফলাফল = Curl_pp_vsendf(তথ্য, &imapc->pp, imapc->resptag, fmt, ap);
  va_end(ap);
  ফেরত ফলাফল;
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
স্থির অক্ষর *imap_atom(ধ্রুবক অক্ষর *str, bool escape_only)
{
  কাঠামো dynbuf সারি;
  size_t nclean;
  size_t len;

  যদি(!str)
    ফেরত ফাঁকা;

  len = strlen(str);
  nclean = strcspn(str, "() {%*]\\\"");
  যদি(len == nclean)
    /* nothing to escape, return a strdup */
    ফেরত curlx_strdup(str);

  curlx_dyn_init(&সারি, 2000);

  যদি(!escape_only && curlx_dyn_addn(&সারি, "\"", 1))
    ফেরত ফাঁকা;

  যখন(*str) {
    যদি((*str == '\\' || *str == '"') &&
       curlx_dyn_addn(&সারি, "\\", 1))
      ফেরত ফাঁকা;
    যদি(curlx_dyn_addn(&সারি, str, 1))
      ফেরত ফাঁকা;
    str++;
  }

  যদি(!escape_only && curlx_dyn_addn(&সারি, "\"", 1))
    ফেরত ফাঁকা;

  ফেরত curlx_dyn_ptr(&সারি);
}

/*
 * Finds the start of a literal '{size}' in line, skipping over quoted strings.
 */
স্থির ধ্রুবক অক্ষর *imap_find_literal(ধ্রুবক অক্ষর *সারি, size_t len)
{
  ধ্রুবক অক্ষর *end = সারি + len;
  bool in_quote = FALSE;

  যখন(সারি < end) {
    যদি(in_quote) {
      যদি(*সারি == '\\' && (সারি + 1) < end) {
        সারি += 2;
        চালু;
      }
      যদি(*সারি == '"')
        in_quote = FALSE;
    }
    অন্যথা {
      যদি(*সারি == '"')
        in_quote = TRUE;
      অন্যথা যদি(*সারি == '{')
        ফেরত সারি;
    }
    সারি++;
  }
  ফেরত ফাঁকা;
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
স্থির bool imap_matchresp(ধ্রুবক অক্ষর *সারি, size_t len, ধ্রুবক অক্ষর *cmd)
{
  ধ্রুবক অক্ষর *end = সারি + len;
  size_t cmd_len = strlen(cmd);

  /* Skip the untagged response marker */
  সারি += 2;

  /* Do we have a number after the marker? */
  যদি(সারি < end && ISDIGIT(*সারি)) {
    /* Skip the number */
    করো
      সারি++;
    যখন(সারি < end && ISDIGIT(*সারি));

    /* Do we have the space character? */
    যদি(সারি == end || *সারি != ' ')
      ফেরত FALSE;

    সারি++;
  }

  /* Does the command name match and is it followed by a space character or at
     the end of line? */
  যদি(সারি + cmd_len <= end && curl_strnequal(সারি, cmd, cmd_len) &&
     (সারি[cmd_len] == ' ' || সারি + cmd_len + 2 == end))
    ফেরত TRUE;

  ফেরত FALSE;
}

/***********************************************************************
 *
 * imap_endofresp()
 *
 * Checks whether the given string is a valid tagged, untagged or continuation
 * response which can be processed by the response handler.
 */
স্থির bool imap_endofresp(কাঠামো Curl_easy *তথ্য, কাঠামো connectdata *conn,
                           ধ্রুবক অক্ষর *সারি, size_t len, পূর্ণসংখ্যা *resp)
{
  কাঠামো imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  কাঠামো IMAP *imap = Curl_meta_get(তথ্য, CURL_META_IMAP_EASY);
  ধ্রুবক অক্ষর *id;
  size_t id_len;

  DEBUGASSERT(imapc);
  DEBUGASSERT(imap);
  যদি(!imapc || !imap)
    ফেরত FALSE;

  /* Do we have a tagged command response? */
  id = imapc->resptag;
  id_len = strlen(id);
  যদি(len >= id_len + 1 && !memcmp(id, সারি, id_len) && সারি[id_len] == ' ') {
    সারি += id_len + 1;
    len -= id_len + 1;

    যদি(len >= 2 && !memcmp(সারি, "OK", 2))
      *resp = IMAP_RESP_OK;
    অন্যথা যদি(len >= 7 && !memcmp(সারি, "PREAUTH", 7))
      *resp = IMAP_RESP_PREAUTH;
    অন্যথা
      *resp = IMAP_RESP_NOT_OK;

    ফেরত TRUE;
  }

  /* Do we have an untagged command response? */
  যদি(len >= 2 && !memcmp("* ", সারি, 2)) {
    নির্বাচন(imapc->state) {
    /* States which are interested in untagged responses */
    ক্ষেত্র IMAP_CAPABILITY:
      যদি(!imap_matchresp(সারি, len, "CAPABILITY"))
        ফেরত FALSE;
      বিরতি;

    ক্ষেত্র IMAP_LIST:
      যদি((!imap->custom && !imap_matchresp(সারি, len, "LIST")) ||
         (imap->custom && !imap_matchresp(সারি, len, imap->custom) &&
          (!curl_strequal(imap->custom, "STORE") ||
           !imap_matchresp(সারি, len, "FETCH")) &&
          !curl_strequal(imap->custom, "SELECT") &&
          !curl_strequal(imap->custom, "EXAMINE") &&
          !curl_strequal(imap->custom, "SEARCH") &&
          !curl_strequal(imap->custom, "EXPUNGE") &&
          !curl_strequal(imap->custom, "LSUB") &&
          !curl_strequal(imap->custom, "UID") &&
          !curl_strequal(imap->custom, "GETQUOTAROOT") &&
          !curl_strequal(imap->custom, "NOOP")))
        ফেরত FALSE;
      বিরতি;

    ক্ষেত্র IMAP_SELECT:
      /* SELECT is special in that its untagged responses do not have a
         common prefix so accept anything! */
      বিরতি;

    ক্ষেত্র IMAP_FETCH:
      যদি(!imap_matchresp(সারি, len, "FETCH"))
        ফেরত FALSE;
      বিরতি;

    ক্ষেত্র IMAP_অনুসন্ধান:
      যদি(!imap_matchresp(সারি, len, "SEARCH"))
        ফেরত FALSE;
      বিরতি;

    /* Ignore other untagged responses */
    পূর্বনির্ধারিত:
      ফেরত FALSE;
    }

    *resp = '*';
    ফেরত TRUE;
  }

  /* Do we have a continuation response? This should be a + symbol followed by
     a space and optionally some text as per RFC-3501 for the AUTHENTICATE and
     APPEND commands and as outlined in Section 4. Examples of RFC-4959 but
     some email servers ignore this and only send a single + instead. */
  যদি(!imap->custom && ((len == 3 && সারি[0] == '+') ||
                       (len >= 2 && !memcmp("+ ", সারি, 2)))) {
    নির্বাচন(imapc->state) {
    /* States which are interested in continuation responses */
    ক্ষেত্র IMAP_AUTHENTICATE:
    ক্ষেত্র IMAP_APPEND:
      *resp = '+';
      বিরতি;

    পূর্বনির্ধারিত:
      failf(তথ্য, "Unexpected continuation response");
      *resp = -1;
      বিরতি;
    }

    ফেরত TRUE;
  }

  ফেরত FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * imap_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
স্থির CURLcode imap_get_message(কাঠামো Curl_easy *তথ্য, কাঠামো bufref *out)
{
  কাঠামো imap_conn *imapc =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_IMAP_CONN);
  অক্ষর *message;
  size_t len;

  যদি(!imapc)
    ফেরত CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&imapc->pp.recvbuf);
  len = imapc->pp.nfinal;
  যদি(len > 2) {
    /* Find the start of the message */
    len -= 2;
    জন্য(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    যখন(len--)
      যদি(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        বিরতি;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, ফাঁকা);
  }
  অন্যথা
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, ফাঁকা);

  ফেরত CURLE_OK;
}

/***********************************************************************
 *
 * imap_state()
 *
 * This is the ONLY way to change IMAP state!
 */
স্থির শূন্য imap_state(কাঠামো Curl_easy *তথ্য,
                       কাঠামো imap_conn *imapc,
                       imapstate newstate)
{
#যদি defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  স্থির ধ্রুবক অক্ষর * ধ্রুবক names[] = {
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
    infof(তথ্য, "IMAP %p state change from %s to %s",
          (শূন্য *)imapc, names[imapc->state], names[newstate]);
#অন্যথা
  (শূন্য)তথ্য;
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
স্থির CURLcode imap_perform_capability(কাঠামো Curl_easy *তথ্য,
                                        কাঠামো imap_conn *imapc)
{
  CURLcode ফলাফল = CURLE_OK;

  imapc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  imapc->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  imapc->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPABILITY command */
  ফলাফল = imap_sendf(তথ্য, imapc, "CAPABILITY");

  যদি(!ফলাফল)
    imap_state(তথ্য, imapc, IMAP_CAPABILITY);

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * imap_perform_starttls()
 *
 * Sends the STARTTLS command to start the upgrade to TLS.
 */
স্থির CURLcode imap_perform_starttls(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো imap_conn *imapc)
{
  /* Send the STARTTLS command */
  CURLcode ফলাফল = imap_sendf(তথ্য, imapc, "STARTTLS");

  যদি(!ফলাফল)
    imap_state(তথ্য, imapc, IMAP_STARTTLS);

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * imap_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
স্থির CURLcode imap_perform_upgrade_tls(কাঠামো Curl_easy *তথ্য,
                                         কাঠামো imap_conn *imapc,
                                         কাঠামো connectdata *conn)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  CURLcode ফলাফল;
  bool ssldone = FALSE;

  যদি(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    ফলাফল = Curl_ssl_cfilter_add(
      তথ্য, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    যদি(ফলাফল)
      যাও out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_imaps;
  }

  DEBUGASSERT(!imapc->ssldone);
  ফলাফল = Curl_conn_connect(তথ্য, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(তথ্য, "imap_perform_upgrade_tls, connect -> %d, %d",
               (পূর্ণসংখ্যা)ফলাফল, ssldone));
  যদি(!ফলাফল && ssldone) {
    imapc->ssldone = ssldone;
    /* perform CAPA now, changes imapc->state out of IMAP_UPGRADETLS */
    ফলাফল = imap_perform_capability(তথ্য, imapc);
  }
out:
  ফেরত ফলাফল;
#অন্যথা
  (শূন্য)তথ্য;
  (শূন্য)imapc;
  (শূন্য)conn;
  ফেরত CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * imap_perform_login()
 *
 * Sends a clear text LOGIN command to authenticate with.
 */
স্থির CURLcode imap_perform_login(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো imap_conn *imapc,
                                   কাঠামো connectdata *conn)
{
  CURLcode ফলাফল = CURLE_OK;
  অক্ষর *user;
  অক্ষর *passwd;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  যদি(!conn->creds) {
    imap_state(তথ্য, imapc, IMAP_STOP);

    ফেরত ফলাফল;
  }

  /* Make sure the username and password are in the correct atom format */
  user = imap_atom(Curl_creds_user(conn->creds), FALSE);
  passwd = imap_atom(Curl_creds_passwd(conn->creds), FALSE);

  /* Send the LOGIN command */
  ফলাফল = imap_sendf(তথ্য, imapc, "LOGIN %s %s", user ? user : "",
                      passwd ? passwd : "");

  curlx_free(user);
  curlx_strzero(passwd);
  curlx_free(passwd);

  যদি(!ফলাফল)
    imap_state(তথ্য, imapc, IMAP_LOGIN);

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * imap_perform_authenticate()
 *
 * Sends an AUTHENTICATE command allowing the client to login with the given
 * SASL authentication mechanism.
 */
স্থির CURLcode imap_perform_authenticate(কাঠামো Curl_easy *তথ্য,
                                          ধ্রুবক অক্ষর *mech,
                                          ধ্রুবক কাঠামো bufref *initresp)
{
  কাঠামো imap_conn *imapc =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_IMAP_CONN);
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক অক্ষর *ir = Curl_bufref_ptr(initresp);

  যদি(!imapc)
    ফেরত CURLE_FAILED_INIT;
  যদি(ir) {
    /* Send the AUTHENTICATE command with the initial response */
    ফলাফল = imap_sendf(তথ্য, imapc, "AUTHENTICATE %s %s",
                        mech, *ir ? ir : "=");
  }
  অন্যথা {
    /* Send the AUTHENTICATE command */
    ফলাফল = imap_sendf(তথ্য, imapc, "AUTHENTICATE %s", mech);
  }

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * imap_continue_authenticate()
 *
 * Sends SASL continuation data.
 */
স্থির CURLcode imap_continue_authenticate(কাঠামো Curl_easy *তথ্য,
                                           ধ্রুবক অক্ষর *mech,
                                           ধ্রুবক কাঠামো bufref *resp)
{
  কাঠামো imap_conn *imapc =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_IMAP_CONN);

  (শূন্য)mech;
  যদি(!imapc)
    ফেরত CURLE_FAILED_INIT;
  ফেরত Curl_pp_sendf(তথ্য, &imapc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * imap_cancel_authenticate()
 *
 * Sends SASL cancellation.
 */
স্থির CURLcode imap_cancel_authenticate(কাঠামো Curl_easy *তথ্য,
                                         ধ্রুবক অক্ষর *mech)
{
  কাঠামো imap_conn *imapc =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_IMAP_CONN);

  (শূন্য)mech;
  যদি(!imapc)
    ফেরত CURLE_FAILED_INIT;
  ফেরত Curl_pp_sendf(তথ্য, &imapc->pp, "*");
}

/***********************************************************************
 *
 * imap_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to clear text should a common
 * mechanism not be available between the client and server.
 */
স্থির CURLcode imap_perform_authentication(কাঠামো Curl_easy *তথ্য,
                                            কাঠামো imap_conn *imapc)
{
  CURLcode ফলাফল = CURLE_OK;
  saslprogress progress;

  /* Check if already authenticated OR if there is enough data to authenticate
     with and end the connect phase if we do not */
  যদি(imapc->preauth ||
     !Curl_sasl_can_authenticate(&imapc->sasl, তথ্য)) {
    imap_state(তথ্য, imapc, IMAP_STOP);
    ফেরত ফলাফল;
  }

  /* Calculate the SASL login details */
  ফলাফল = Curl_sasl_start(&imapc->sasl, তথ্য, (bool)imapc->ir_supported,
                           &progress);
  যদি(!ফলাফল) {
    যদি(progress == SASL_INPROGRESS)
      imap_state(তথ্য, imapc, IMAP_AUTHENTICATE);
    অন্যথা যদি(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
      /* Perform clear text authentication */
      ফলাফল = imap_perform_login(তথ্য, imapc, তথ্য->conn);
    অন্যথা
      ফলাফল = Curl_sasl_is_blocked(&imapc->sasl, তথ্য);
  }

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * imap_perform_list()
 *
 * Sends a LIST command or an alternative custom request.
 */
স্থির CURLcode imap_perform_list(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো imap_conn *imapc,
                                  কাঠামো IMAP *imap)
{
  CURLcode ফলাফল = CURLE_OK;

  যদি(imap->custom)
    /* Send the custom request */
    ফলাফল = imap_sendf(তথ্য, imapc, "%s%s", imap->custom,
                        imap->custom_params ? imap->custom_params : "");
  অন্যথা {
    /* Make sure the mailbox is in the correct atom format if necessary */
    অক্ষর *mailbox = imap->mailbox ? imap_atom(imap->mailbox, TRUE)
                                  : curlx_strdup("");
    যদি(!mailbox)
      ফেরত CURLE_OUT_OF_MEMORY;

    /* Send the LIST command */
    ফলাফল = imap_sendf(তথ্য, imapc, "LIST \"%s\" *", mailbox);

    curlx_free(mailbox);
  }

  যদি(!ফলাফল)
    imap_state(তথ্য, imapc, IMAP_LIST);

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * imap_perform_select()
 *
 * Sends a SELECT command to ask the server to change the selected mailbox.
 */
স্থির CURLcode imap_perform_select(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো imap_conn *imapc,
                                    কাঠামো IMAP *imap)
{
  CURLcode ফলাফল = CURLE_OK;
  অক্ষর *mailbox;

  /* Invalidate old information as we are switching mailboxes */
  curlx_safefree(imapc->mailbox);
  imapc->mb_uidvalidity_set = FALSE;

  /* Check we have a mailbox */
  যদি(!imap->mailbox) {
    failf(তথ্য, "Cannot SELECT without a mailbox.");
    ফেরত CURLE_URL_MALFORMAT;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  যদি(!mailbox)
    ফেরত CURLE_OUT_OF_MEMORY;

  /* Send the SELECT command */
  ফলাফল = imap_sendf(তথ্য, imapc, "SELECT %s", mailbox);

  curlx_free(mailbox);

  যদি(!ফলাফল)
    imap_state(তথ্য, imapc, IMAP_SELECT);

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * imap_perform_fetch()
 *
 * Sends a FETCH command to initiate the download of a message.
 */
স্থির CURLcode imap_perform_fetch(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো imap_conn *imapc,
                                   কাঠামো IMAP *imap)
{
  CURLcode ফলাফল = CURLE_OK;
  /* Check we have a UID */
  যদি(imap->uid) {

    /* Send the FETCH command */
    যদি(imap->partial)
      ফলাফল = imap_sendf(তথ্য, imapc, "UID FETCH %s BODY[%s]<%s>",
                          imap->uid, imap->section ? imap->section : "",
                          imap->partial);
    অন্যথা
      ফলাফল = imap_sendf(তথ্য, imapc, "UID FETCH %s BODY[%s]",
                          imap->uid, imap->section ? imap->section : "");
  }
  অন্যথা যদি(imap->mindex) {
    /* Send the FETCH command */
    যদি(imap->partial)
      ফলাফল = imap_sendf(তথ্য, imapc, "FETCH %s BODY[%s]<%s>",
                          imap->mindex, imap->section ? imap->section : "",
                          imap->partial);
    অন্যথা
      ফলাফল = imap_sendf(তথ্য, imapc, "FETCH %s BODY[%s]",
                          imap->mindex, imap->section ? imap->section : "");
  }
  অন্যথা {
    failf(তথ্য, "Cannot FETCH without a UID.");
    ফেরত CURLE_URL_MALFORMAT;
  }
  যদি(!ফলাফল)
    imap_state(তথ্য, imapc, IMAP_FETCH);

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * imap_perform_append()
 *
 * Sends an APPEND command to initiate the upload of a message.
 */
স্থির CURLcode imap_perform_append(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো imap_conn *imapc,
                                    কাঠামো IMAP *imap)
{
  CURLcode ফলাফল = CURLE_OK;
  অক্ষর *mailbox;
  কাঠামো dynbuf flags;

  /* Check we have a mailbox */
  যদি(!imap->mailbox) {
    failf(তথ্য, "Cannot APPEND without a mailbox.");
    ফেরত CURLE_URL_MALFORMAT;
  }

#ifndef CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  যদি(IS_MIME_POST(তথ্য)) {
    curl_mimepart *postp = তথ্য->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(unsigned পূর্ণসংখ্যা)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, তথ্য->set.headers, 0);
    ফলাফল = Curl_mime_prepare_headers(তথ্য, postp, ফাঁকা,
                                       ফাঁকা, MIMESTRATEGY_MAIL);

    যদি(!ফলাফল && !Curl_checkheaders(তথ্য, STRCONST("Mime-Version")))
      ফলাফল = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    যদি(!ফলাফল)
      ফলাফল = Curl_creader_set_mime(তথ্য, postp);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  অন্যথা
#endif
  {
    ফলাফল = Curl_creader_set_fread(তথ্য, তথ্য->state.infilesize);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  /* Check we know the size of the upload. This takes all readers
   * into account. Especially crlf conversions which make the size
   * unpredictable, e.g. -1. */
  তথ্য->state.infilesize = Curl_creader_যোগ_দৈর্ঘ্য(তথ্য);
  যদি(তথ্য->state.infilesize < 0) {
    যদি(তথ্য->set.crlf)
      failf(তথ্য, "Cannot APPEND with CRLF conversion making size unknown");
    অন্যথা
      failf(তথ্য, "Cannot APPEND with unknown input file size");
    ফেরত CURLE_UPLOAD_FAILED;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  যদি(!mailbox)
    ফেরত CURLE_OUT_OF_MEMORY;

  /* Generate flags string and send the APPEND command */
  curlx_dyn_init(&flags, 100);
  যদি(তথ্য->set.upload_flags) {
    পূর্ণসংখ্যা i;
    কাঠামো ulbits ulflag[] = {
      { CURLULFLAG_ANSWERED, "Answered" },
      { CURLULFLAG_DELETED, "Deleted" },
      { CURLULFLAG_DRAFT, "Draft" },
      { CURLULFLAG_FLAGGED, "Flagged" },
      { CURLULFLAG_SEEN, "Seen" },
      { 0, ফাঁকা }
    };

    ফলাফল = CURLE_OUT_OF_MEMORY;
    যদি(curlx_dyn_add(&flags, " (")) {
      যাও cleanup;
    }

    জন্য(i = 0; ulflag[i].bit; i++) {
      যদি(তথ্য->set.upload_flags & ulflag[i].bit &&
         ((curlx_dyn_len(&flags) > 2 && curlx_dyn_add(&flags, " ")) ||
          curlx_dyn_add(&flags, "\\") ||
          curlx_dyn_add(&flags, ulflag[i].flag)))
        যাও cleanup;
    }

    যদি(curlx_dyn_add(&flags, ")"))
      যাও cleanup;
  }
  অন্যথা যদি(curlx_dyn_add(&flags, ""))
    যাও cleanup;

  ফলাফল = imap_sendf(তথ্য, imapc, "APPEND %s%s {%" FMT_OFF_T "}",
                      mailbox, curlx_dyn_ptr(&flags), তথ্য->state.infilesize);

cleanup:
  curlx_dyn_free(&flags);
  curlx_free(mailbox);

  যদি(!ফলাফল)
    imap_state(তথ্য, imapc, IMAP_APPEND);

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * imap_perform_search()
 *
 * Sends a SEARCH command.
 */
স্থির CURLcode imap_perform_অনুসন্ধান(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো imap_conn *imapc,
                                    কাঠামো IMAP *imap)
{
  CURLcode ফলাফল = CURLE_OK;

  /* Check we have a query string */
  যদি(!imap->query) {
    failf(তথ্য, "Cannot SEARCH without a query string.");
    ফেরত CURLE_URL_MALFORMAT;
  }

  /* Send the SEARCH command */
  ফলাফল = imap_sendf(তথ্য, imapc, "SEARCH %s", imap->query);

  যদি(!ফলাফল)
    imap_state(তথ্য, imapc, IMAP_অনুসন্ধান);

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * imap_perform_logout()
 *
 * Performs the logout action prior to sclose() being called.
 */
স্থির CURLcode imap_perform_logout(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো imap_conn *imapc)
{
  /* Send the LOGOUT command */
  CURLcode ফলাফল = imap_sendf(তথ্য, imapc, "LOGOUT");

  যদি(!ফলাফল)
    imap_state(তথ্য, imapc, IMAP_LOGOUT);

  ফেরত ফলাফল;
}

/* For the initial server greeting */
স্থির CURLcode imap_state_servergreet_resp(কাঠামো Curl_easy *তথ্য,
                                            কাঠামো imap_conn *imapc,
                                            পূর্ণসংখ্যা imapcode,
                                            imapstate instate)
{
  (শূন্য)instate;

  যদি(imapcode == IMAP_RESP_PREAUTH) {
    /* PREAUTH */
    imapc->preauth = TRUE;
    infof(তথ্য, "PREAUTH connection, already authenticated");
  }
  অন্যথা যদি(imapcode != IMAP_RESP_OK) {
    failf(তথ্য, "Got unexpected imap-server response");
    ফেরত CURLE_WEIRD_SERVER_REPLY;
  }

  ফেরত imap_perform_capability(তথ্য, imapc);
}

/* For CAPABILITY responses */
স্থির CURLcode imap_state_capability_resp(কাঠামো Curl_easy *তথ্য,
                                           কাঠামো imap_conn *imapc,
                                           পূর্ণসংখ্যা imapcode,
                                           imapstate instate)
{
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক অক্ষর *সারি = curlx_dyn_ptr(&imapc->pp.recvbuf);

  (শূন্য)instate;

  /* Do we have an untagged response? */
  যদি(imapcode == '*') {
    সারি += 2;

    /* Loop through the data line */
    জন্য(;;) {
      size_t wordlen;
      যখন(*সারি && (ISBLANK(*সারি) || ISNEWLINE(*সারি)))
        সারি++;

      যদি(!*সারি)
        বিরতি;

      /* Extract the word */
      জন্য(wordlen = 0; সারি[wordlen] && !ISBLANK(সারি[wordlen]) &&
                       !ISNEWLINE(সারি[wordlen]);)
        wordlen++;

      /* Does the server support the STARTTLS capability? */
      যদি(wordlen == 8 && curl_strnequal(সারি, "STARTTLS", 8))
        imapc->tls_supported = TRUE;

      /* Has the server explicitly disabled clear text authentication? */
      অন্যথা যদি(wordlen == 13 && curl_strnequal(সারি, "LOGINDISABLED", 13))
        imapc->login_disabled = TRUE;

      /* Does the server support the SASL-IR capability? */
      অন্যথা যদি(wordlen == 7 && curl_strnequal(সারি, "SASL-IR", 7))
        imapc->ir_supported = TRUE;

      /* Do we have a SASL based authentication mechanism? */
      অন্যথা যদি(wordlen > 5 && curl_strnequal(সারি, "AUTH=", 5)) {
        size_t llen;
        unsigned short mechbit;

        সারি += 5;
        wordlen -= 5;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(সারি, wordlen, &llen);
        যদি(mechbit && llen == wordlen)
          imapc->sasl.authmechs |= mechbit;
      }

      সারি += wordlen;
    }
  }
  অন্যথা যদি(তথ্য->set.use_ssl && !Curl_conn_is_ssl(তথ্য->conn, FIRSTSOCKET)) {
    /* PREAUTH is not compatible with STARTTLS. */
    যদি(imapcode == IMAP_RESP_OK && imapc->tls_supported && !imapc->preauth) {
      /* Switch to TLS connection now */
      ফলাফল = imap_perform_starttls(তথ্য, imapc);
    }
    অন্যথা যদি(তথ্য->set.use_ssl <= CURLUSESSL_TRY)
      ফলাফল = imap_perform_authentication(তথ্য, imapc);
    অন্যথা {
      failf(তথ্য, "STARTTLS not available.");
      ফলাফল = CURLE_USE_SSL_FAILED;
    }
  }
  অন্যথা
    ফলাফল = imap_perform_authentication(তথ্য, imapc);

  ফেরত ফলাফল;
}

/* For STARTTLS responses */
স্থির CURLcode imap_state_starttls_resp(কাঠামো Curl_easy *তথ্য,
                                         কাঠামো imap_conn *imapc,
                                         পূর্ণসংখ্যা imapcode,
                                         imapstate instate)
{
  CURLcode ফলাফল = CURLE_OK;

  (শূন্য)instate;

  /* Pipelining in response is forbidden. */
  যদি(imapc->pp.overflow)
    ফেরত CURLE_WEIRD_SERVER_REPLY;

  যদি(imapcode != IMAP_RESP_OK) {
    যদি(তথ্য->set.use_ssl != CURLUSESSL_TRY) {
      failf(তথ্য, "STARTTLS denied");
      ফলাফল = CURLE_USE_SSL_FAILED;
    }
    অন্যথা
      ফলাফল = imap_perform_authentication(তথ্য, imapc);
  }
  অন্যথা
    imap_state(তথ্য, imapc, IMAP_UPGRADETLS);

  ফেরত ফলাফল;
}

/* For SASL authentication responses */
স্থির CURLcode imap_state_auth_resp(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো imap_conn *imapc,
                                     পূর্ণসংখ্যা imapcode,
                                     imapstate instate)
{
  CURLcode ফলাফল = CURLE_OK;
  saslprogress progress;

  (শূন্য)instate;

  ফলাফল = Curl_sasl_continue(&imapc->sasl, তথ্য, imapcode, &progress);
  যদি(!ফলাফল)
    নির্বাচন(progress) {
    ক্ষেত্র SASL_DONE:
      imap_state(তথ্য, imapc, IMAP_STOP);  /* Authenticated */
      বিরতি;
    ক্ষেত্র SASL_IDLE:            /* No mechanism left after cancellation */
      যদি(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
        /* Perform clear text authentication */
        ফলাফল = imap_perform_login(তথ্য, imapc, তথ্য->conn);
      অন্যথা {
        failf(তথ্য, "Authentication cancelled");
        ফলাফল = CURLE_LOGIN_DENIED;
      }
      বিরতি;
    পূর্বনির্ধারিত:
      বিরতি;
    }

  ফেরত ফলাফল;
}

/* For LOGIN responses */
স্থির CURLcode imap_state_login_resp(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো imap_conn *imapc,
                                      পূর্ণসংখ্যা imapcode,
                                      imapstate instate)
{
  CURLcode ফলাফল = CURLE_OK;
  (শূন্য)instate;

  যদি(imapcode != IMAP_RESP_OK) {
    failf(তথ্য, "Access denied. %c", imapcode);
    ফলাফল = CURLE_LOGIN_DENIED;
  }
  অন্যথা
    /* End of connect phase */
    imap_state(তথ্য, imapc, IMAP_STOP);

  ফেরত ফলাফল;
}

/* Detect IMAP listings vs. downloading a single email */
স্থির bool is_custom_fetch_listing_match(ধ্রুবক অক্ষর *params)
{
  /* match " 1:* (FLAGS ..." or " 1,2,3 (FLAGS ..." */
  যদি(*params++ != ' ')
    ফেরত FALSE;

  যখন(ISDIGIT(*params)) {
    params++;
    যদি(*params == 0)
      ফেরত FALSE;
  }
  যদি(*params == ':')
    ফেরত TRUE;
  যদি(*params == ',')
    ফেরত TRUE;
  ফেরত FALSE;
}

স্থির bool is_custom_fetch_listing(কাঠামো IMAP *imap)
{
  /* filter out "UID FETCH 1:* (FLAGS ..." queries to list emails */
  যদি(!imap->custom)
    ফেরত FALSE;
  অন্যথা যদি(curl_strequal(imap->custom, "FETCH") && imap->custom_params) {
    ধ্রুবক অক্ষর *p = imap->custom_params;
    ফেরত is_custom_fetch_listing_match(p);
  }
  অন্যথা যদি(curl_strequal(imap->custom, "UID") && imap->custom_params &&
          curl_strnequal(imap->custom_params, " FETCH ", 7)) {
    ধ্রুবক অক্ষর *p = imap->custom_params + 6;
    ফেরত is_custom_fetch_listing_match(p);
  }
  ফেরত FALSE;
}

/* For LIST and SEARCH responses */
স্থির CURLcode imap_state_listsearch_resp(কাঠামো Curl_easy *তথ্য,
                                           কাঠামো imap_conn *imapc,
                                           পূর্ণসংখ্যা imapcode,
                                           imapstate instate)
{
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক অক্ষর *সারি = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  কাঠামো IMAP *imap = Curl_meta_get(তথ্য, CURL_META_IMAP_EASY);

  DEBUGASSERT(imap);
  যদি(!imap)
    ফেরত CURLE_FAILED_INIT;
  (শূন্য)instate;

  যদি(imapcode == '*' && is_custom_fetch_listing(imap)) {
    /* custom FETCH or UID FETCH for listing is not handled here */
  }
  অন্যথা যদি(imapcode == '*') {
    /* Check if this response contains a literal (e.g. FETCH responses with
       body data). Literal syntax is {size}\r\n */
    ধ্রুবক অক্ষর *cr = memchr(সারি, '\r', len);
    size_t সারি_len = cr ? (size_t)(cr - সারি) : len;
    ধ্রুবক অক্ষর *ptr = imap_find_literal(সারি, সারি_len);
    যদি(ptr) {
      curl_off_t আকার_2 = 0;
      bool parsed = FALSE;
      ptr++;
      যদি(!curlx_str_সংখ্যা(&ptr, &আকার_2, CURL_OFF_T_MAX) &&
         !curlx_str_single(&ptr, '}'))
        parsed = TRUE;

      যদি(parsed) {
        কাঠামো pingpong *pp = &imapc->pp;
        size_t বাফার_len = curlx_dyn_len(&pp->recvbuf);
        size_t after_header = বাফার_len - pp->nfinal;

        /* This is a literal response, setup to receive the body data */
        infof(তথ্য, "Found %" FMT_OFF_T " bytes to download", আকার_2);

        /* First write the header line */
        ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, সারি, len);
        যদি(ফলাফল)
          ফেরত ফলাফল;

        /* Handle data already in buffer after the header line */
        যদি(after_header > 0) {
          /* There is already data in the buffer that is part of the literal
             body or subsequent responses */
          size_t chunk = after_header;

          /* Keep only the data after the header line */
          curlx_dyn_লেজ(&pp->recvbuf, chunk);
          pp->nfinal = 0; /* done */

          /* Limit chunk to the literal size */
          যদি((curl_off_t)chunk > আকার_2)
            chunk = (size_t)আকার_2;

          যদি(chunk) {
            /* Write the literal body data */
            ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY,
                                       curlx_dyn_ptr(&pp->recvbuf), chunk);
            যদি(ফলাফল)
              ফেরত ফলাফল;
          }

          /* Handle remaining data in buffer (either more literal data or
             subsequent responses) */
          যদি(after_header > chunk) {
            /* Keep the data after the literal body */
            pp->overflow = after_header - chunk;
            curlx_dyn_লেজ(&pp->recvbuf, pp->overflow);
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

        যদি((CURL_OFF_T_MAX - আকার_2) < (curl_off_t)len)
          /* unlikely to actually be a transfer this big, but avoid integer
             overflow */
          আকার_2 = CURL_OFF_T_MAX;
        অন্যথা
          আকার_2 += len;

        /* Progress size includes both header line and literal body */
        Curl_pgrs_Set_Download_আকার(তথ্য, আকার_2);

        যদি(তথ্য->req.bytecount == আকার_2)
          /* All data already transferred (header + literal body) */
          Curl_xfer_setup_nop(তথ্য);
        অন্যথা {
          /* Setup to receive the literal body data.
             maxdownload and transfer size include both header line and
             literal body */
          তথ্য->req.maxdownload = আকার_2;
          Curl_xfer_setup_recv(তথ্য, FIRSTSOCKET, আকার_2);
        }
        /* End of DO phase */
        imap_state(তথ্য, imapc, IMAP_STOP);
      }
      অন্যথা {
        /* Failed to parse literal, write the line */
        ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, সারি, len);
      }
    }
    অন্যথা {
      /* No literal, write the line as-is */
      ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, সারি, len);
    }
  }
  অন্যথা যদি(imapcode != IMAP_RESP_OK)
    ফলাফল = CURLE_QUOTE_ERROR;
  অন্যথা
    /* End of DO phase */
    imap_state(তথ্য, imapc, IMAP_STOP);

  ফেরত ফলাফল;
}

/* For SELECT responses */
স্থির CURLcode imap_state_select_resp(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো imap_conn *imapc,
                                       কাঠামো IMAP *imap,
                                       পূর্ণসংখ্যা imapcode,
                                       imapstate instate)
{
  CURLcode ফলাফল = CURLE_OK;
  (শূন্য)instate;

  যদি(imapcode == '*') {
    /* See if this is an UIDVALIDITY response */
    ধ্রুবক অক্ষর *সারি = curlx_dyn_ptr(&imapc->pp.recvbuf);
    size_t len = curlx_dyn_len(&imapc->pp.recvbuf);
    যদি((len >= 18) && checkprefix("OK [UIDVALIDITY ", &সারি[2])) {
      curl_off_t মান;
      ধ্রুবক অক্ষর *p = &সারি[2] + CURL_CSTRLEN("OK [UIDVALIDITY ");
      যদি(!curlx_str_সংখ্যা(&p, &মান, UINT_MAX)) {
        imapc->mb_uidvalidity = (unsigned পূর্ণসংখ্যা)মান;
        imapc->mb_uidvalidity_set = TRUE;
      }
    }
  }
  অন্যথা যদি(imapcode == IMAP_RESP_OK) {
    /* Check if the UIDVALIDITY has been specified and matches */
    যদি(imap->uidvalidity_set && imapc->mb_uidvalidity_set &&
       (imap->uidvalidity != imapc->mb_uidvalidity)) {
      failf(তথ্য, "Mailbox UIDVALIDITY has changed");
      ফলাফল = CURLE_REMOTE_FILE_NOT_FOUND;
    }
    অন্যথা {
      /* Note the currently opened mailbox on this connection */
      DEBUGASSERT(!imapc->mailbox);
      imapc->mailbox = curlx_strdup(imap->mailbox);
      যদি(!imapc->mailbox)
        ফেরত CURLE_OUT_OF_MEMORY;

      যদি(imap->custom)
        ফলাফল = imap_perform_list(তথ্য, imapc, imap);
      অন্যথা যদি(imap->query)
        ফলাফল = imap_perform_অনুসন্ধান(তথ্য, imapc, imap);
      অন্যথা
        ফলাফল = imap_perform_fetch(তথ্য, imapc, imap);
    }
  }
  অন্যথা {
    failf(তথ্য, "Select failed");
    ফলাফল = CURLE_LOGIN_DENIED;
  }

  ফেরত ফলাফল;
}

/* For the (first line of the) FETCH responses */
স্থির CURLcode imap_state_fetch_resp(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো imap_conn *imapc,
                                      পূর্ণসংখ্যা imapcode,
                                      imapstate instate)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো pingpong *pp = &imapc->pp;
  ধ্রুবক অক্ষর *ptr = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  bool parsed = FALSE;
  curl_off_t আকার_2 = 0;

  (শূন্য)instate;

  যদি(imapcode != '*') {
    Curl_pgrs_Set_Download_আকার(তথ্য, -1);
    imap_state(তথ্য, imapc, IMAP_STOP);
    ফেরত CURLE_REMOTE_FILE_NOT_FOUND;
  }

  /* Something like this is received "* 1 FETCH (BODY[TEXT] {2021}\r" so parse
     the continuation data contained within the curly brackets */
  ptr = imap_find_literal(ptr, len);
  যদি(ptr) {
    ptr++;
    যদি(!curlx_str_সংখ্যা(&ptr, &আকার_2, CURL_OFF_T_MAX) &&
       !curlx_str_single(&ptr, '}'))
      parsed = TRUE;
  }

  যদি(parsed) {
    infof(তথ্য, "Found %" FMT_OFF_T " bytes to download", আকার_2);
    Curl_pgrs_Set_Download_আকার(তথ্য, আকার_2);

    যদি(pp->overflow) {
      /* At this point there is a data in the receive buffer that is body
         content, send it as body and then skip it. Do note that there may
         even be additional "headers" after the body. */
      size_t chunk = pp->overflow;

      /* keep only the overflow */
      curlx_dyn_লেজ(&pp->recvbuf, chunk);
      pp->nfinal = 0; /* done */

      যদি((curl_off_t)chunk > আকার_2)
        /* The conversion from curl_off_t to size_t is always fine here */
        chunk = (size_t)আকার_2;

      যদি(!chunk) {
        /* no size, we are done with the data */
        imap_state(তথ্য, imapc, IMAP_STOP);
        ফেরত CURLE_OK;
      }
      ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY,
                                 curlx_dyn_ptr(&pp->recvbuf), chunk);
      যদি(ফলাফল)
        ফেরত ফলাফল;

      infof(তথ্য, "Written %zu bytes, %" FMT_OFF_T
            " bytes are left for transfer", chunk, (curl_off_t)(আকার_2 - chunk));

      /* Have we used the entire overflow or part of it?*/
      যদি(pp->overflow > chunk) {
        /* remember the remaining trailing overflow data */
        pp->overflow -= chunk;
        curlx_dyn_লেজ(&pp->recvbuf, pp->overflow);
      }
      অন্যথা {
        pp->overflow = 0; /* handled */
        /* Free the cache */
        curlx_dyn_reset(&pp->recvbuf);
      }
    }

    যদি(তথ্য->req.bytecount == আকার_2)
      /* The entire data is already transferred! */
      Curl_xfer_setup_nop(তথ্য);
    অন্যথা {
      /* IMAP download */
      তথ্য->req.maxdownload = আকার_2;
      Curl_xfer_setup_recv(তথ্য, FIRSTSOCKET, আকার_2);
    }
  }
  অন্যথা {
    /* We do not know how to parse this line */
    failf(তথ্য, "Failed to parse FETCH response.");
    ফলাফল = CURLE_WEIRD_SERVER_REPLY;
  }

  /* End of DO phase */
  imap_state(তথ্য, imapc, IMAP_STOP);

  ফেরত ফলাফল;
}

/* For final FETCH responses performed after the download */
স্থির CURLcode imap_state_fetch_final_resp(কাঠামো Curl_easy *তথ্য,
                                            কাঠামো imap_conn *imapc,
                                            পূর্ণসংখ্যা imapcode,
                                            imapstate instate)
{
  CURLcode ফলাফল = CURLE_OK;

  (শূন্য)instate;

  যদি(imapcode != IMAP_RESP_OK)
    ফলাফল = CURLE_WEIRD_SERVER_REPLY;
  অন্যথা
    /* End of DONE phase */
    imap_state(তথ্য, imapc, IMAP_STOP);

  ফেরত ফলাফল;
}

/* For APPEND responses */
স্থির CURLcode imap_state_append_resp(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো imap_conn *imapc,
                                       পূর্ণসংখ্যা imapcode,
                                       imapstate instate)
{
  CURLcode ফলাফল = CURLE_OK;
  (শূন্য)instate;

  যদি(imapcode != '+') {
    ফলাফল = CURLE_UPLOAD_FAILED;
  }
  অন্যথা {
    /* Set the progress upload size */
    Curl_pgrs_Set_Upload_আকার(তথ্য, তথ্য->state.infilesize);

    /* IMAP upload */
    Curl_xfer_setup_send(তথ্য, FIRSTSOCKET);

    /* End of DO phase */
    imap_state(তথ্য, imapc, IMAP_STOP);
  }

  ফেরত ফলাফল;
}

/* For final APPEND responses performed after the upload */
স্থির CURLcode imap_state_append_final_resp(কাঠামো Curl_easy *তথ্য,
                                             কাঠামো imap_conn *imapc,
                                             পূর্ণসংখ্যা imapcode,
                                             imapstate instate)
{
  CURLcode ফলাফল = CURLE_OK;

  (শূন্য)instate;

  যদি(imapcode != IMAP_RESP_OK)
    ফলাফল = CURLE_UPLOAD_FAILED;
  অন্যথা
    /* End of DONE phase */
    imap_state(তথ্য, imapc, IMAP_STOP);

  ফেরত ফলাফল;
}

স্থির CURLcode imap_pp_statemachine(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো connectdata *conn)
{
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা imapcode;
  কাঠামো imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  কাঠামো IMAP *imap = Curl_meta_get(তথ্য, CURL_META_IMAP_EASY);
  কাঠামো pingpong *pp;
  size_t nread = 0;

  যদি(!imapc || !imap)
    ফেরত CURLE_FAILED_INIT;
  pp = &imapc->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not IMAP */
upgrade_tls:
  যদি(imapc->state == IMAP_UPGRADETLS) {
    ফলাফল = imap_perform_upgrade_tls(তথ্য, imapc, conn);
    যদি(ফলাফল || (imapc->state == IMAP_UPGRADETLS))
      ফেরত ফলাফল;
  }

  /* Flush any data that needs to be sent */
  যদি(pp->sendleft)
    ফেরত Curl_pp_flushsend(তথ্য, pp);

  করো {
    /* Read the response from the server */
    ফলাফল = Curl_pp_readresp(তথ্য, FIRSTSOCKET, pp, &imapcode, &nread);
    যদি(ফলাফল)
      ফেরত ফলাফল;

    /* Was there an error parsing the response line? */
    যদি(imapcode == -1)
      ফেরত CURLE_WEIRD_SERVER_REPLY;

    যদি(!imapcode)
      বিরতি;

    /* We have now received a full IMAP server response */
    নির্বাচন(imapc->state) {
    ক্ষেত্র IMAP_SERVERGREET:
      ফলাফল = imap_state_servergreet_resp(তথ্য, imapc,
                                           imapcode, imapc->state);
      বিরতি;

    ক্ষেত্র IMAP_CAPABILITY:
      ফলাফল = imap_state_capability_resp(তথ্য, imapc, imapcode, imapc->state);
      বিরতি;

    ক্ষেত্র IMAP_STARTTLS:
      ফলাফল = imap_state_starttls_resp(তথ্য, imapc, imapcode, imapc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      যদি(!ফলাফল && (imapc->state == IMAP_UPGRADETLS))
        যাও upgrade_tls;
      বিরতি;

    ক্ষেত্র IMAP_AUTHENTICATE:
      ফলাফল = imap_state_auth_resp(তথ্য, imapc, imapcode, imapc->state);
      বিরতি;

    ক্ষেত্র IMAP_LOGIN:
      ফলাফল = imap_state_login_resp(তথ্য, imapc, imapcode, imapc->state);
      বিরতি;

    ক্ষেত্র IMAP_LIST:
    ক্ষেত্র IMAP_অনুসন্ধান:
      ফলাফল = imap_state_listsearch_resp(তথ্য, imapc, imapcode, imapc->state);
      বিরতি;

    ক্ষেত্র IMAP_SELECT:
      ফলাফল = imap_state_select_resp(তথ্য, imapc, imap,
                                      imapcode, imapc->state);
      বিরতি;

    ক্ষেত্র IMAP_FETCH:
      ফলাফল = imap_state_fetch_resp(তথ্য, imapc, imapcode, imapc->state);
      বিরতি;

    ক্ষেত্র IMAP_FETCH_FINAL:
      ফলাফল = imap_state_fetch_final_resp(তথ্য, imapc,
                                           imapcode, imapc->state);
      বিরতি;

    ক্ষেত্র IMAP_APPEND:
      ফলাফল = imap_state_append_resp(তথ্য, imapc, imapcode, imapc->state);
      বিরতি;

    ক্ষেত্র IMAP_APPEND_FINAL:
      ফলাফল = imap_state_append_final_resp(তথ্য, imapc,
                                            imapcode, imapc->state);
      বিরতি;

    ক্ষেত্র IMAP_LOGOUT:
    পূর্বনির্ধারিত:
      /* internal error */
      imap_state(তথ্য, imapc, IMAP_STOP);
      বিরতি;
    }
  } যখন(!ফলাফল && imapc->state != IMAP_STOP && Curl_pp_moredata(pp));

  ফেরত ফলাফল;
}

/* Called repeatedly until done from multi.c */
স্থির CURLcode imap_multi_statemach(কাঠামো Curl_easy *তথ্য, bool *done)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো imap_conn *imapc =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_IMAP_CONN);

  *done = FALSE;
  যদি(!imapc)
    ফেরত CURLE_FAILED_INIT;
  ফলাফল = Curl_pp_statemach(তথ্য, &imapc->pp, FALSE, FALSE);
  *done = (imapc->state == IMAP_STOP);

  ফেরত ফলাফল;
}

স্থির CURLcode imap_block_statemach(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো imap_conn *imapc,
                                     bool disconnecting)
{
  CURLcode ফলাফল = CURLE_OK;

  যখন(imapc->state != IMAP_STOP && !ফলাফল)
    ফলাফল = Curl_pp_statemach(তথ্য, &imapc->pp, TRUE, disconnecting);

  ফেরত ফলাফল;
}

/* For the IMAP "protocol connect" and "doing" phases only */
স্থির CURLcode imap_pollset(কাঠামো Curl_easy *তথ্য,
                             কাঠামো easy_pollset *ps)
{
  কাঠামো imap_conn *imapc =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_IMAP_CONN);
  ফেরত imapc ? Curl_pp_pollset(তথ্য, &imapc->pp, ps) : CURLE_OK;
}

স্থির শূন্য imap_easy_reset(কাঠামো IMAP *imap)
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
স্থির bool imap_is_bchar(অক্ষর ch)
{
  /* Performing the alnum check first with macro is faster because of ASCII
     arithmetic */
  ফেরত ch && (ISALNUM(ch) || strchr(":@/&=-._~!$\'()*+,%", ch));
}

/***********************************************************************
 *
 * imap_parse_url_options()
 *
 * Parse the URL login options.
 */
স্থির CURLcode imap_parse_url_options(কাঠামো connectdata *conn,
                                       কাঠামো imap_conn *imapc)
{
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক অক্ষর *ptr = conn->options;
  bool prefer_login = FALSE;

  যখন(!ফলাফল && ptr && *ptr) {
    ধ্রুবক অক্ষর *চাবি = ptr;
    ধ্রুবক অক্ষর *মান;

    যখন(*ptr && *ptr != '=')
      ptr++;

    মান = ptr + 1;

    যখন(*ptr && *ptr != ';')
      ptr++;

    যদি(curl_strnequal(চাবি, "AUTH=+LOGIN", 11)) {
      /* User prefers plaintext LOGIN over any SASL, including SASL LOGIN */
      prefer_login = TRUE;
      imapc->sasl.prefmech = SASL_AUTH_NONE;
    }
    অন্যথা যদি(curl_strnequal(চাবি, "AUTH=", 5)) {
      prefer_login = FALSE;
      ফলাফল = Curl_sasl_parse_url_auth_option(&imapc->sasl,
                                               মান, ptr - মান);
    }
    অন্যথা {
      prefer_login = FALSE;
      ফলাফল = CURLE_URL_MALFORMAT;
    }

    যদি(*ptr == ';')
      ptr++;
  }

  যদি(prefer_login)
    imapc->preftype = IMAP_TYPE_CLEARTEXT;
  অন্যথা {
    নির্বাচন(imapc->sasl.prefmech) {
    ক্ষেত্র SASL_AUTH_NONE:
      imapc->preftype = IMAP_TYPE_NONE;
      বিরতি;
    ক্ষেত্র SASL_AUTH_DEFAULT:
      imapc->preftype = IMAP_TYPE_ANY;
      বিরতি;
    পূর্বনির্ধারিত:
      imapc->preftype = IMAP_TYPE_SASL;
      বিরতি;
    }
  }

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * imap_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
স্থির CURLcode imap_parse_url_path(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো IMAP *imap)
{
  /* The imap struct is already initialized in imap_connect() */
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক অক্ষর *begin = &তথ্য->state.up.path[1]; /* skip leading slash */
  ধ্রুবক অক্ষর *ptr = begin;

  /* See how much of the URL is a valid path and decode it */
  যখন(imap_is_bchar(*ptr))
    ptr++;

  যদি(ptr != begin) {
    /* Remove the trailing slash if present */
    ধ্রুবক অক্ষর *end = ptr;
    যদি(end > begin && end[-1] == '/')
      end--;

    ফলাফল = Curl_urldecode(begin, end - begin, &imap->mailbox, ফাঁকা,
                            REJECT_CTRL);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  অন্যথা
    imap->mailbox = ফাঁকা;

  /* There can be any number of parameters in the form ";NAME=VALUE" */
  যখন(*ptr == ';') {
    অক্ষর *name;
    অক্ষর *মান;
    size_t valuelen;

    /* Find the length of the name parameter */
    begin = ++ptr;
    যখন(*ptr && *ptr != '=')
      ptr++;

    যদি(!*ptr)
      ফেরত CURLE_URL_MALFORMAT;

    /* Decode the name parameter */
    ফলাফল = Curl_urldecode(begin, ptr - begin, &name, ফাঁকা,
                            REJECT_CTRL);
    যদি(ফলাফল)
      ফেরত ফলাফল;

    /* Find the length of the value parameter */
    begin = ++ptr;
    যখন(imap_is_bchar(*ptr))
      ptr++;

    /* Decode the value parameter */
    ফলাফল = Curl_urldecode(begin, ptr - begin, &মান, &valuelen,
                            REJECT_CTRL);
    যদি(ফলাফল) {
      curlx_free(name);
      ফেরত ফলাফল;
    }

    DEBUGF(infof(তথ্য, "IMAP URL parameter '%s' = '%s'", name, মান));

    /* Process the known hierarchical parameters (UIDVALIDITY, UID, SECTION
       and PARTIAL) stripping of the trailing slash character if it is
       present.

       Note: Unknown parameters trigger a URL_MALFORMAT error. */
    যদি(valuelen > 0 && মান[valuelen - 1] == '/')
      মান[valuelen - 1] = '\0';
    যদি(valuelen) {
      যদি(curl_strequal(name, "UIDVALIDITY") && !imap->uidvalidity_set) {
        curl_off_t অঙ্ক;
        ধ্রুবক অক্ষর *p = (ধ্রুবক অক্ষর *)মান;
        যদি(!curlx_str_সংখ্যা(&p, &অঙ্ক, UINT_MAX)) {
          imap->uidvalidity = (unsigned পূর্ণসংখ্যা)অঙ্ক;
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
        ফেরত CURLE_URL_MALFORMAT;
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
    CURLUcode uc = curl_url_get(তথ্য->state.uh, CURLUPART_QUERY, &imap->query,
                                CURLU_URLDECODE);
    যদি(uc == CURLUE_OUT_OF_MEMORY)
      ফেরত CURLE_OUT_OF_MEMORY;
  }

  /* Any extra stuff at the end of the URL is an error */
  যদি(*ptr)
    ফেরত CURLE_URL_MALFORMAT;

  ফেরত CURLE_OK;
}

/***********************************************************************
 *
 * imap_parse_custom_request()
 *
 * Parse the custom request.
 */
স্থির CURLcode imap_parse_custom_request(কাঠামো Curl_easy *তথ্য,
                                          কাঠামো IMAP *imap)
{
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক অক্ষর *custom = CURL_EASY_STR(তথ্য, STRING_CUSTOMREQUEST);

  যদি(custom) {
    /* URL decode the custom request */
    ফলাফল = Curl_urldecode(custom, 0, &imap->custom, ফাঁকা, REJECT_CTRL);

    /* Extract the parameters if specified */
    যদি(!ফলাফল) {
      ধ্রুবক অক্ষর *params = imap->custom;

      যখন(*params && *params != ' ')
        params++;

      যদি(*params) {
        imap->custom_params = curlx_strdup(params);
        imap->custom[params - imap->custom] = '\0';

        যদি(!imap->custom_params)
          ফলাফল = CURLE_OUT_OF_MEMORY;
      }
    }
  }

  ফেরত ফলাফল;
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
স্থির CURLcode imap_connect(কাঠামো Curl_easy *তথ্য, bool *done)
{
  কাঠামো imap_conn *imapc =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_IMAP_CONN);
  CURLcode ফলাফল = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  যদি(!imapc)
    ফেরত CURLE_FAILED_INIT;

  /* Parse the URL options */
  ফলাফল = imap_parse_url_options(তথ্য->conn, imapc);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* Start off waiting for the server greeting response */
  imap_state(তথ্য, imapc, IMAP_SERVERGREET);

  /* Start off with an response id of '*' */
  curlx_strcopy(imapc->resptag, আকার(imapc->resptag), STRCONST("*"));

  ফলাফল = imap_multi_statemach(তথ্য, done);

  ফেরত ফলাফল;
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
স্থির CURLcode imap_done(কাঠামো Curl_easy *তথ্য, CURLcode status,
                          bool premature)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  কাঠামো IMAP *imap = Curl_meta_get(তথ্য, CURL_META_IMAP_EASY);

  যদি(!imapc)
    ফেরত CURLE_FAILED_INIT;
  যদি(!imap)
    ফেরত CURLE_OK;

  যদি(status || premature) {
    CURL_TRC_M(তথ্য, "IMAP done, not finalizing transfer");
    connclose(conn); /* marked for closure */
    ফলাফল = status;         /* use the already set error code */
  }
  অন্যথা যদি(!তথ্য->set.connect_only &&
          ((!imap->custom && (imap->uid || imap->mindex)) ||
           (imap->custom && তথ্য->req.maxdownload > 0) ||
           তথ্য->state.upload || IS_MIME_POST(তথ্য))) {
    /* Handle responses after FETCH or APPEND transfer has finished.
       For custom commands, check if we set up a download which indicates
       a FETCH-like command with literal data. */

    যদি(!তথ্য->state.upload && !IS_MIME_POST(তথ্য))
      imap_state(তথ্য, imapc, IMAP_FETCH_FINAL);
    অন্যথা {
      /* End the APPEND command first by sending an empty line */
      ফলাফল = Curl_pp_sendf(তথ্য, &imapc->pp, "%s", "");
      যদি(!ফলাফল)
        imap_state(তথ্য, imapc, IMAP_APPEND_FINAL);
    }

    /* Run the state-machine */
    যদি(!ফলাফল)
      ফলাফল = imap_block_statemach(তথ্য, imapc, FALSE);
  }

  imap_easy_reset(imap);
  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * imap_perform()
 *
 * This is the actual DO function for IMAP. Fetch or append a message, or do
 * other things according to the options previously setup.
 */
স্থির CURLcode imap_perform(কাঠামো Curl_easy *তথ্য, bool *connected,
                             bool *dophase_done)
{
  /* This is IMAP and no proxy */
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  কাঠামো IMAP *imap = Curl_meta_get(তথ্য, CURL_META_IMAP_EASY);
  bool selected = FALSE;

  DEBUGF(infof(তথ্য, "DO phase starts"));
  যদি(!imapc || !imap)
    ফেরত CURLE_FAILED_INIT;

  যদি(তথ্য->req.no_body) {
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
  যদি(তথ্য->state.upload || IS_MIME_POST(তথ্য))
    /* APPEND can be executed directly */
    ফলাফল = imap_perform_append(তথ্য, imapc, imap);
  অন্যথা যদি(imap->custom && (selected || !imap->mailbox))
    /* Custom command using the same mailbox or no mailbox */
    ফলাফল = imap_perform_list(তথ্য, imapc, imap);
  অন্যথা যদি(!imap->custom && selected && (imap->uid || imap->mindex))
    /* FETCH from the same mailbox */
    ফলাফল = imap_perform_fetch(তথ্য, imapc, imap);
  অন্যথা যদি(!imap->custom && selected && imap->query)
    /* SEARCH the current mailbox */
    ফলাফল = imap_perform_অনুসন্ধান(তথ্য, imapc, imap);
  অন্যথা যদি(imap->mailbox && !selected &&
          (imap->custom || imap->uid || imap->mindex || imap->query))
    /* SELECT the mailbox */
    ফলাফল = imap_perform_select(তথ্য, imapc, imap);
  অন্যথা
    /* LIST */
    ফলাফল = imap_perform_list(তথ্য, imapc, imap);

  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* Run the state-machine */
  ফলাফল = imap_multi_statemach(তথ্য, dophase_done);

  *connected = Curl_conn_is_connected(conn, FIRSTSOCKET);

  যদি(*dophase_done)
    DEBUGF(infof(তথ্য, "DO phase is complete"));

  ফেরত ফলাফল;
}

/* Call this when the DO phase has completed */
স্থির CURLcode imap_dophase_done(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো IMAP *imap,
                                  bool connected)
{
  (শূন্য)connected;

  যদি(imap->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(তথ্য);

  ফেরত CURLE_OK;
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
স্থির CURLcode imap_regular_transfer(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো IMAP *imap,
                                      bool *dophase_done)
{
  CURLcode ফলাফল = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  তথ্য->req.আকার_2 = -1;

  /* Set the progress data */
  Curl_pgrsReset(তথ্য);

  /* Carry out the perform */
  ফলাফল = imap_perform(তথ্য, &connected, dophase_done);

  /* Perform post DO phase operations if necessary */
  যদি(!ফলাফল && *dophase_done)
    ফলাফল = imap_dophase_done(তথ্য, imap, connected);

  ফেরত ফলাফল;
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
স্থির CURLcode imap_do(কাঠামো Curl_easy *তথ্য, bool *done)
{
  কাঠামো IMAP *imap = Curl_meta_get(তথ্য, CURL_META_IMAP_EASY);
  CURLcode ফলাফল = CURLE_OK;
  *done = FALSE; /* default to false */

  যদি(!imap)
    ফেরত CURLE_FAILED_INIT;
  /* Parse the URL path */
  ফলাফল = imap_parse_url_path(তথ্য, imap);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* Parse the custom request */
  ফলাফল = imap_parse_custom_request(তথ্য, imap);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  ফলাফল = imap_regular_transfer(তথ্য, imap, done);

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * imap_disconnect()
 *
 * Disconnect from an IMAP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
স্থির CURLcode imap_disconnect(কাঠামো Curl_easy *তথ্য,
                                কাঠামো connectdata *conn, bool dead_connection)
{
  কাঠামো imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);

  যদি(imapc &&
     /* We cannot send quit unconditionally. If this connection is stale or
        bad in any way (pingpong has pending data to send),
        sending quit and waiting around here will make the
        disconnect wait in vain and cause more problems than we need to. */
     !dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(তথ্য, &imapc->pp) &&
     !imap_perform_logout(তথ্য, imapc))
    (শূন্য)imap_block_statemach(তথ্য, imapc, TRUE); /* ignore errors */

  ফেরত CURLE_OK;
}

/* Called from multi.c while DOing */
স্থির CURLcode imap_doing(কাঠামো Curl_easy *তথ্য, bool *dophase_done)
{
  কাঠামো IMAP *imap = Curl_meta_get(তথ্য, CURL_META_IMAP_EASY);
  CURLcode ফলাফল;

  যদি(!imap)
    ফেরত CURLE_FAILED_INIT;

  ফলাফল = imap_multi_statemach(তথ্য, dophase_done);
  যদি(ফলাফল)
    DEBUGF(infof(তথ্য, "DO phase failed"));
  অন্যথা যদি(*dophase_done) {
    ফলাফল = imap_dophase_done(তথ্য, imap, FALSE /* not connected */);

    DEBUGF(infof(তথ্য, "DO phase is complete"));
  }

  ফেরত ফলাফল;
}

স্থির শূন্য imap_easy_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  কাঠামো IMAP *imap = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  imap_easy_reset(imap);
  curlx_free(imap);
}

স্থির শূন্য imap_conn_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  কাঠামো imap_conn *imapc = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  Curl_pp_disconnect(&imapc->pp);
  curlx_safefree(imapc->mailbox);
  curlx_free(imapc);
}

/* SASL parameters for the imap protocol */
স্থির ধ্রুবক কাঠামো SASLproto saslimap = {
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

স্থির CURLcode imap_setup_connection(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো connectdata *conn)
{
  কাঠামো imap_conn *imapc;
  কাঠামো pingpong *pp;
  কাঠামো IMAP *imap;

  imapc = curlx_calloc(1, আকার(*imapc));
  যদি(!imapc)
    ফেরত CURLE_OUT_OF_MEMORY;

  pp = &imapc->pp;
  PINGPONG_SETUP(pp, imap_pp_statemachine, imap_endofresp);

  /* Set the default preferred authentication type and mechanism */
  imapc->preftype = IMAP_TYPE_ANY;
  Curl_sasl_init(&imapc->sasl, তথ্য, &saslimap);

  Curl_pp_init(pp, Curl_pgrs_now(তথ্য));

  যদি(Curl_conn_meta_set(conn, CURL_META_IMAP_CONN, imapc, imap_conn_dtor))
    ফেরত CURLE_OUT_OF_MEMORY;

  imap = curlx_calloc(1, আকার(কাঠামো IMAP));
  যদি(!imap ||
     Curl_meta_set(তথ্য, CURL_META_IMAP_EASY, imap, imap_easy_dtor))
    ফেরত CURLE_OUT_OF_MEMORY;

  ফেরত CURLE_OK;
}

/*
 * IMAP protocol.
 */
ধ্রুবক কাঠামো Curl_protocol Curl_protocol_imap = {
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
