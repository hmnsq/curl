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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "imap.h"

#ifndef CURL_DISABLE_IMAP

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#endif
#ifdef __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#endif

#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "pingpong.h"
#ಅನ್ತರ್ಭಾವ "mime.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "curl_sasl.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_IMAP_EASY   "meta:proto:imap:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_IMAP_CONN   "meta:proto:imap:conn"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
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
  IMAP_ಹುಡುಕು,
  IMAP_LOGOUT,
  IMAP_LAST          /* never used */
} imapstate;

/* imap_conn is used for struct connection-oriented data */
ಸಂರಚನಾ imap_conn {
  ಸಂರಚನಾ pingpong pp;
  ಸಂರಚನಾ SASL sasl;           /* SASL-related parameters */
  ವರ್ಣ *mailbox;              /* The last selected mailbox */
  imapstate state;            /* Always use imap.c:state() to change state! */
  unsigned ಪೂರ್ಣಾಙ್ಕ mb_uidvalidity; /* UIDVALIDITY parsed from select response */
  ವರ್ಣ resptag[5];            /* Response tag to wait for */
  unsigned ವರ್ಣ preftype;     /* Preferred authentication type */
  unsigned ವರ್ಣ cmdid;        /* Last used command ID */
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
ಸಂರಚನಾ IMAP {
  curl_pp_transfer transfer;
  ವರ್ಣ *mailbox;          /* Mailbox to select */
  ವರ್ಣ *uid;              /* Message UID to fetch */
  ವರ್ಣ *mindex;           /* Index in mail box of mail to fetch */
  ವರ್ಣ *section;          /* Message SECTION to fetch */
  ವರ್ಣ *partial;          /* Message PARTIAL to fetch */
  ವರ್ಣ *query;            /* Query to search for */
  ವರ್ಣ *custom;           /* Custom request */
  ವರ್ಣ *custom_params;    /* Parameters for the custom request */
  unsigned ಪೂರ್ಣಾಙ್ಕ uidvalidity; /* UIDVALIDITY to check in select */
  BIT(uidvalidity_set);
};

#define IMAP_RESP_OK       1
#define IMAP_RESP_NOT_OK   2
#define IMAP_RESP_PREAUTH  3

ಸಂರಚನಾ ulbits {
  ಪೂರ್ಣಾಙ್ಕ bit;
  ಧ್ರುವ ವರ್ಣ *flag;
};

/***********************************************************************
 *
 * imap_sendf()
 *
 * Sends the formatted string as an IMAP command to the server.
 *
 * Designed to never block.
 */
ಸ್ಥಿರ CURLcode imap_sendf(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ imap_conn *imapc,
                           ಧ್ರುವ ವರ್ಣ *fmt, ...) CURL_PRINTF(3, 4);
ಸ್ಥಿರ CURLcode imap_sendf(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ imap_conn *imapc,
                           ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  CURLcode result = CURLE_OK;
  va_list ap;

  DEBUGASSERT(fmt);

  /* Calculate the tag based on the connection ID and command ID */
  curl_msnprintf(imapc->resptag, ಪರಿಮಾಣ(imapc->resptag), "%c%03d",
                 'A' + curlx_sltosi((long)(data->conn->connection_id % 26)),
                 ++imapc->cmdid);

  va_start(ap, fmt);
  result = Curl_pp_vsendf(data, &imapc->pp, imapc->resptag, fmt, ap);
  va_end(ap);
  ಪ್ರತ್ಯಾಗಮ result;
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
ಸ್ಥಿರ ವರ್ಣ *imap_atom(ಧ್ರುವ ವರ್ಣ *str, bool escape_only)
{
  ಸಂರಚನಾ dynbuf ಸಾಲು;
  size_t nclean;
  size_t len;

  ಯದಿ(!str)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  len = strlen(str);
  nclean = strcspn(str, "() {%*]\\\"");
  ಯದಿ(len == nclean)
    /* nothing to escape, return a strdup */
    ಪ್ರತ್ಯಾಗಮ curlx_strdup(str);

  curlx_dyn_init(&ಸಾಲು, 2000);

  ಯದಿ(!escape_only && curlx_dyn_addn(&ಸಾಲು, "\"", 1))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  ಯಾವತ್(*str) {
    ಯದಿ((*str == '\\' || *str == '"') &&
       curlx_dyn_addn(&ಸಾಲು, "\\", 1))
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ(curlx_dyn_addn(&ಸಾಲು, str, 1))
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    str++;
  }

  ಯದಿ(!escape_only && curlx_dyn_addn(&ಸಾಲು, "\"", 1))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  ಪ್ರತ್ಯಾಗಮ curlx_dyn_ptr(&ಸಾಲು);
}

/*
 * Finds the start of a literal '{size}' in line, skipping over quoted strings.
 */
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *imap_find_literal(ಧ್ರುವ ವರ್ಣ *ಸಾಲು, size_t len)
{
  ಧ್ರುವ ವರ್ಣ *end = ಸಾಲು + len;
  bool in_quote = FALSE;

  ಯಾವತ್(ಸಾಲು < end) {
    ಯದಿ(in_quote) {
      ಯದಿ(*ಸಾಲು == '\\' && (ಸಾಲು + 1) < end) {
        ಸಾಲು += 2;
        ಅನುವರ್ತ;
      }
      ಯದಿ(*ಸಾಲು == '"')
        in_quote = FALSE;
    }
    ಅನ್ಯಥಾ {
      ಯದಿ(*ಸಾಲು == '"')
        in_quote = TRUE;
      ಅನ್ಯಥಾ ಯದಿ(*ಸಾಲು == '{')
        ಪ್ರತ್ಯಾಗಮ ಸಾಲು;
    }
    ಸಾಲು++;
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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
ಸ್ಥಿರ bool imap_matchresp(ಧ್ರುವ ವರ್ಣ *ಸಾಲು, size_t len, ಧ್ರುವ ವರ್ಣ *cmd)
{
  ಧ್ರುವ ವರ್ಣ *end = ಸಾಲು + len;
  size_t cmd_len = strlen(cmd);

  /* Skip the untagged response marker */
  ಸಾಲು += 2;

  /* Do we have a number after the marker? */
  ಯದಿ(ಸಾಲು < end && ISDIGIT(*ಸಾಲು)) {
    /* Skip the number */
    ಕುರು
      ಸಾಲು++;
    ಯಾವತ್(ಸಾಲು < end && ISDIGIT(*ಸಾಲು));

    /* Do we have the space character? */
    ಯದಿ(ಸಾಲು == end || *ಸಾಲು != ' ')
      ಪ್ರತ್ಯಾಗಮ FALSE;

    ಸಾಲು++;
  }

  /* Does the command name match and is it followed by a space character or at
     the end of line? */
  ಯದಿ(ಸಾಲು + cmd_len <= end && curl_strnequal(ಸಾಲು, cmd, cmd_len) &&
     (ಸಾಲು[cmd_len] == ' ' || ಸಾಲು + cmd_len + 2 == end))
    ಪ್ರತ್ಯಾಗಮ TRUE;

  ಪ್ರತ್ಯಾಗಮ FALSE;
}

/***********************************************************************
 *
 * imap_endofresp()
 *
 * Checks whether the given string is a valid tagged, untagged or continuation
 * response which can be processed by the response handler.
 */
ಸ್ಥಿರ bool imap_endofresp(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ connectdata *conn,
                           ಧ್ರುವ ವರ್ಣ *ಸಾಲು, size_t len, ಪೂರ್ಣಾಙ್ಕ *resp)
{
  ಸಂರಚನಾ imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ಸಂರಚನಾ IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  ಧ್ರುವ ವರ್ಣ *id;
  size_t id_len;

  DEBUGASSERT(imapc);
  DEBUGASSERT(imap);
  ಯದಿ(!imapc || !imap)
    ಪ್ರತ್ಯಾಗಮ FALSE;

  /* Do we have a tagged command response? */
  id = imapc->resptag;
  id_len = strlen(id);
  ಯದಿ(len >= id_len + 1 && !memcmp(id, ಸಾಲು, id_len) && ಸಾಲು[id_len] == ' ') {
    ಸಾಲು += id_len + 1;
    len -= id_len + 1;

    ಯದಿ(len >= 2 && !memcmp(ಸಾಲು, "OK", 2))
      *resp = IMAP_RESP_OK;
    ಅನ್ಯಥಾ ಯದಿ(len >= 7 && !memcmp(ಸಾಲು, "PREAUTH", 7))
      *resp = IMAP_RESP_PREAUTH;
    ಅನ್ಯಥಾ
      *resp = IMAP_RESP_NOT_OK;

    ಪ್ರತ್ಯಾಗಮ TRUE;
  }

  /* Do we have an untagged command response? */
  ಯದಿ(len >= 2 && !memcmp("* ", ಸಾಲು, 2)) {
    ವಿಕಲ್ಪ(imapc->state) {
    /* States which are interested in untagged responses */
    ಪಕ್ಷ IMAP_CAPABILITY:
      ಯದಿ(!imap_matchresp(ಸಾಲು, len, "CAPABILITY"))
        ಪ್ರತ್ಯಾಗಮ FALSE;
      ಭಙ್ಗ;

    ಪಕ್ಷ IMAP_LIST:
      ಯದಿ((!imap->custom && !imap_matchresp(ಸಾಲು, len, "LIST")) ||
         (imap->custom && !imap_matchresp(ಸಾಲು, len, imap->custom) &&
          (!curl_strequal(imap->custom, "STORE") ||
           !imap_matchresp(ಸಾಲು, len, "FETCH")) &&
          !curl_strequal(imap->custom, "SELECT") &&
          !curl_strequal(imap->custom, "EXAMINE") &&
          !curl_strequal(imap->custom, "SEARCH") &&
          !curl_strequal(imap->custom, "EXPUNGE") &&
          !curl_strequal(imap->custom, "LSUB") &&
          !curl_strequal(imap->custom, "UID") &&
          !curl_strequal(imap->custom, "GETQUOTAROOT") &&
          !curl_strequal(imap->custom, "NOOP")))
        ಪ್ರತ್ಯಾಗಮ FALSE;
      ಭಙ್ಗ;

    ಪಕ್ಷ IMAP_SELECT:
      /* SELECT is special in that its untagged responses do not have a
         common prefix so accept anything! */
      ಭಙ್ಗ;

    ಪಕ್ಷ IMAP_FETCH:
      ಯದಿ(!imap_matchresp(ಸಾಲು, len, "FETCH"))
        ಪ್ರತ್ಯಾಗಮ FALSE;
      ಭಙ್ಗ;

    ಪಕ್ಷ IMAP_ಹುಡುಕು:
      ಯದಿ(!imap_matchresp(ಸಾಲು, len, "SEARCH"))
        ಪ್ರತ್ಯಾಗಮ FALSE;
      ಭಙ್ಗ;

    /* Ignore other untagged responses */
    ಸ್ವಭಾವ:
      ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    *resp = '*';
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }

  /* Do we have a continuation response? This should be a + symbol followed by
     a space and optionally some text as per RFC-3501 for the AUTHENTICATE and
     APPEND commands and as outlined in Section 4. Examples of RFC-4959 but
     some email servers ignore this and only send a single + instead. */
  ಯದಿ(!imap->custom && ((len == 3 && ಸಾಲು[0] == '+') ||
                       (len >= 2 && !memcmp("+ ", ಸಾಲು, 2)))) {
    ವಿಕಲ್ಪ(imapc->state) {
    /* States which are interested in continuation responses */
    ಪಕ್ಷ IMAP_AUTHENTICATE:
    ಪಕ್ಷ IMAP_APPEND:
      *resp = '+';
      ಭಙ್ಗ;

    ಸ್ವಭಾವ:
      failf(data, "Unexpected continuation response");
      *resp = -1;
      ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
  }

  ಪ್ರತ್ಯಾಗಮ FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * imap_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ಸ್ಥಿರ CURLcode imap_get_message(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ bufref *out)
{
  ಸಂರಚನಾ imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  ವರ್ಣ *message;
  size_t len;

  ಯದಿ(!imapc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&imapc->pp.recvbuf);
  len = imapc->pp.nfinal;
  ಯದಿ(len > 2) {
    /* Find the start of the message */
    len -= 2;
    ಪ್ರತಿ(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    ಯಾವತ್(len--)
      ಯದಿ(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        ಭಙ್ಗ;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, ರಿಕ್ತ);
  }
  ಅನ್ಯಥಾ
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, ರಿಕ್ತ);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/***********************************************************************
 *
 * imap_state()
 *
 * This is the ONLY way to change IMAP state!
 */
ಸ್ಥಿರ ಶೂನ್ಯ imap_state(ಸಂರಚನಾ Curl_easy *data,
                       ಸಂರಚನಾ imap_conn *imapc,
                       imapstate newstate)
{
#ಯದಿ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ names[] = {
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

  ಯದಿ(imapc->state != newstate)
    infof(data, "IMAP %p state change from %s to %s",
          (ಶೂನ್ಯ *)imapc, names[imapc->state], names[newstate]);
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
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
ಸ್ಥಿರ CURLcode imap_perform_capability(ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ imap_conn *imapc)
{
  CURLcode result = CURLE_OK;

  imapc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  imapc->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  imapc->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPABILITY command */
  result = imap_sendf(data, imapc, "CAPABILITY");

  ಯದಿ(!result)
    imap_state(data, imapc, IMAP_CAPABILITY);

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * imap_perform_starttls()
 *
 * Sends the STARTTLS command to start the upgrade to TLS.
 */
ಸ್ಥಿರ CURLcode imap_perform_starttls(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ imap_conn *imapc)
{
  /* Send the STARTTLS command */
  CURLcode result = imap_sendf(data, imapc, "STARTTLS");

  ಯದಿ(!result)
    imap_state(data, imapc, IMAP_STARTTLS);

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * imap_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ಸ್ಥಿರ CURLcode imap_perform_upgrade_tls(ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ imap_conn *imapc,
                                         ಸಂರಚನಾ connectdata *conn)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  CURLcode result;
  bool ssldone = FALSE;

  ಯದಿ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_imaps;
  }

  DEBUGASSERT(!imapc->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "imap_perform_upgrade_tls, connect -> %d, %d",
               (ಪೂರ್ಣಾಙ್ಕ)result, ssldone));
  ಯದಿ(!result && ssldone) {
    imapc->ssldone = ssldone;
    /* perform CAPA now, changes imapc->state out of IMAP_UPGRADETLS */
    result = imap_perform_capability(data, imapc);
  }
out:
  ಪ್ರತ್ಯಾಗಮ result;
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)imapc;
  (ಶೂನ್ಯ)conn;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * imap_perform_login()
 *
 * Sends a clear text LOGIN command to authenticate with.
 */
ಸ್ಥಿರ CURLcode imap_perform_login(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ imap_conn *imapc,
                                   ಸಂರಚನಾ connectdata *conn)
{
  CURLcode result = CURLE_OK;
  ವರ್ಣ *user;
  ವರ್ಣ *passwd;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  ಯದಿ(!conn->creds) {
    imap_state(data, imapc, IMAP_STOP);

    ಪ್ರತ್ಯಾಗಮ result;
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

  ಯದಿ(!result)
    imap_state(data, imapc, IMAP_LOGIN);

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * imap_perform_authenticate()
 *
 * Sends an AUTHENTICATE command allowing the client to login with the given
 * SASL authentication mechanism.
 */
ಸ್ಥಿರ CURLcode imap_perform_authenticate(ಸಂರಚನಾ Curl_easy *data,
                                          ಧ್ರುವ ವರ್ಣ *mech,
                                          ಧ್ರುವ ಸಂರಚನಾ bufref *initresp)
{
  ಸಂರಚನಾ imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ವರ್ಣ *ir = Curl_bufref_ptr(initresp);

  ಯದಿ(!imapc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಯದಿ(ir) {
    /* Send the AUTHENTICATE command with the initial response */
    result = imap_sendf(data, imapc, "AUTHENTICATE %s %s",
                        mech, *ir ? ir : "=");
  }
  ಅನ್ಯಥಾ {
    /* Send the AUTHENTICATE command */
    result = imap_sendf(data, imapc, "AUTHENTICATE %s", mech);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * imap_continue_authenticate()
 *
 * Sends SASL continuation data.
 */
ಸ್ಥಿರ CURLcode imap_continue_authenticate(ಸಂರಚನಾ Curl_easy *data,
                                           ಧ್ರುವ ವರ್ಣ *mech,
                                           ಧ್ರುವ ಸಂರಚನಾ bufref *resp)
{
  ಸಂರಚನಾ imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  (ಶೂನ್ಯ)mech;
  ಯದಿ(!imapc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಪ್ರತ್ಯಾಗಮ Curl_pp_sendf(data, &imapc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * imap_cancel_authenticate()
 *
 * Sends SASL cancellation.
 */
ಸ್ಥಿರ CURLcode imap_cancel_authenticate(ಸಂರಚನಾ Curl_easy *data,
                                         ಧ್ರುವ ವರ್ಣ *mech)
{
  ಸಂರಚನಾ imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  (ಶೂನ್ಯ)mech;
  ಯದಿ(!imapc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಪ್ರತ್ಯಾಗಮ Curl_pp_sendf(data, &imapc->pp, "*");
}

/***********************************************************************
 *
 * imap_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to clear text should a common
 * mechanism not be available between the client and server.
 */
ಸ್ಥಿರ CURLcode imap_perform_authentication(ಸಂರಚನಾ Curl_easy *data,
                                            ಸಂರಚನಾ imap_conn *imapc)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  /* Check if already authenticated OR if there is enough data to authenticate
     with and end the connect phase if we do not */
  ಯದಿ(imapc->preauth ||
     !Curl_sasl_can_authenticate(&imapc->sasl, data)) {
    imap_state(data, imapc, IMAP_STOP);
    ಪ್ರತ್ಯಾಗಮ result;
  }

  /* Calculate the SASL login details */
  result = Curl_sasl_start(&imapc->sasl, data, (bool)imapc->ir_supported,
                           &progress);
  ಯದಿ(!result) {
    ಯದಿ(progress == SASL_INPROGRESS)
      imap_state(data, imapc, IMAP_AUTHENTICATE);
    ಅನ್ಯಥಾ ಯದಿ(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
      /* Perform clear text authentication */
      result = imap_perform_login(data, imapc, data->conn);
    ಅನ್ಯಥಾ
      result = Curl_sasl_is_blocked(&imapc->sasl, data);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * imap_perform_list()
 *
 * Sends a LIST command or an alternative custom request.
 */
ಸ್ಥಿರ CURLcode imap_perform_list(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ imap_conn *imapc,
                                  ಸಂರಚನಾ IMAP *imap)
{
  CURLcode result = CURLE_OK;

  ಯದಿ(imap->custom)
    /* Send the custom request */
    result = imap_sendf(data, imapc, "%s%s", imap->custom,
                        imap->custom_params ? imap->custom_params : "");
  ಅನ್ಯಥಾ {
    /* Make sure the mailbox is in the correct atom format if necessary */
    ವರ್ಣ *mailbox = imap->mailbox ? imap_atom(imap->mailbox, TRUE)
                                  : curlx_strdup("");
    ಯದಿ(!mailbox)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

    /* Send the LIST command */
    result = imap_sendf(data, imapc, "LIST \"%s\" *", mailbox);

    curlx_free(mailbox);
  }

  ಯದಿ(!result)
    imap_state(data, imapc, IMAP_LIST);

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * imap_perform_select()
 *
 * Sends a SELECT command to ask the server to change the selected mailbox.
 */
ಸ್ಥಿರ CURLcode imap_perform_select(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ imap_conn *imapc,
                                    ಸಂರಚನಾ IMAP *imap)
{
  CURLcode result = CURLE_OK;
  ವರ್ಣ *mailbox;

  /* Invalidate old information as we are switching mailboxes */
  curlx_safefree(imapc->mailbox);
  imapc->mb_uidvalidity_set = FALSE;

  /* Check we have a mailbox */
  ಯದಿ(!imap->mailbox) {
    failf(data, "Cannot SELECT without a mailbox.");
    ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  ಯದಿ(!mailbox)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  /* Send the SELECT command */
  result = imap_sendf(data, imapc, "SELECT %s", mailbox);

  curlx_free(mailbox);

  ಯದಿ(!result)
    imap_state(data, imapc, IMAP_SELECT);

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * imap_perform_fetch()
 *
 * Sends a FETCH command to initiate the download of a message.
 */
ಸ್ಥಿರ CURLcode imap_perform_fetch(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ imap_conn *imapc,
                                   ಸಂರಚನಾ IMAP *imap)
{
  CURLcode result = CURLE_OK;
  /* Check we have a UID */
  ಯದಿ(imap->uid) {

    /* Send the FETCH command */
    ಯದಿ(imap->partial)
      result = imap_sendf(data, imapc, "UID FETCH %s BODY[%s]<%s>",
                          imap->uid, imap->section ? imap->section : "",
                          imap->partial);
    ಅನ್ಯಥಾ
      result = imap_sendf(data, imapc, "UID FETCH %s BODY[%s]",
                          imap->uid, imap->section ? imap->section : "");
  }
  ಅನ್ಯಥಾ ಯದಿ(imap->mindex) {
    /* Send the FETCH command */
    ಯದಿ(imap->partial)
      result = imap_sendf(data, imapc, "FETCH %s BODY[%s]<%s>",
                          imap->mindex, imap->section ? imap->section : "",
                          imap->partial);
    ಅನ್ಯಥಾ
      result = imap_sendf(data, imapc, "FETCH %s BODY[%s]",
                          imap->mindex, imap->section ? imap->section : "");
  }
  ಅನ್ಯಥಾ {
    failf(data, "Cannot FETCH without a UID.");
    ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;
  }
  ಯದಿ(!result)
    imap_state(data, imapc, IMAP_FETCH);

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * imap_perform_append()
 *
 * Sends an APPEND command to initiate the upload of a message.
 */
ಸ್ಥಿರ CURLcode imap_perform_append(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ imap_conn *imapc,
                                    ಸಂರಚನಾ IMAP *imap)
{
  CURLcode result = CURLE_OK;
  ವರ್ಣ *mailbox;
  ಸಂರಚನಾ dynbuf flags;

  /* Check we have a mailbox */
  ಯದಿ(!imap->mailbox) {
    failf(data, "Cannot APPEND without a mailbox.");
    ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;
  }

#ifndef CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  ಯದಿ(IS_MIME_POST(data)) {
    curl_mimepart *postp = data->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(unsigned ಪೂರ್ಣಾಙ್ಕ)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, data->set.headers, 0);
    result = Curl_mime_prepare_headers(data, postp, ರಿಕ್ತ,
                                       ರಿಕ್ತ, MIMESTRATEGY_MAIL);

    ಯದಿ(!result && !Curl_checkheaders(data, STRCONST("Mime-Version")))
      result = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    ಯದಿ(!result)
      result = Curl_creader_set_mime(data, postp);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಅನ್ಯಥಾ
#endif
  {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  /* Check we know the size of the upload. This takes all readers
   * into account. Especially crlf conversions which make the size
   * unpredictable, e.g. -1. */
  data->state.infilesize = Curl_creader_total_length(data);
  ಯದಿ(data->state.infilesize < 0) {
    ಯದಿ(data->set.crlf)
      failf(data, "Cannot APPEND with CRLF conversion making size unknown");
    ಅನ್ಯಥಾ
      failf(data, "Cannot APPEND with unknown input file size");
    ಪ್ರತ್ಯಾಗಮ CURLE_UPLOAD_FAILED;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  ಯದಿ(!mailbox)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  /* Generate flags string and send the APPEND command */
  curlx_dyn_init(&flags, 100);
  ಯದಿ(data->set.upload_flags) {
    ಪೂರ್ಣಾಙ್ಕ i;
    ಸಂರಚನಾ ulbits ulflag[] = {
      { CURLULFLAG_ANSWERED, "Answered" },
      { CURLULFLAG_DELETED, "Deleted" },
      { CURLULFLAG_DRAFT, "Draft" },
      { CURLULFLAG_FLAGGED, "Flagged" },
      { CURLULFLAG_SEEN, "Seen" },
      { 0, ರಿಕ್ತ }
    };

    result = CURLE_OUT_OF_MEMORY;
    ಯದಿ(curlx_dyn_add(&flags, " (")) {
      ಗಚ್ಛ cleanup;
    }

    ಪ್ರತಿ(i = 0; ulflag[i].bit; i++) {
      ಯದಿ(data->set.upload_flags & ulflag[i].bit &&
         ((curlx_dyn_len(&flags) > 2 && curlx_dyn_add(&flags, " ")) ||
          curlx_dyn_add(&flags, "\\") ||
          curlx_dyn_add(&flags, ulflag[i].flag)))
        ಗಚ್ಛ cleanup;
    }

    ಯದಿ(curlx_dyn_add(&flags, ")"))
      ಗಚ್ಛ cleanup;
  }
  ಅನ್ಯಥಾ ಯದಿ(curlx_dyn_add(&flags, ""))
    ಗಚ್ಛ cleanup;

  result = imap_sendf(data, imapc, "APPEND %s%s {%" FMT_OFF_T "}",
                      mailbox, curlx_dyn_ptr(&flags), data->state.infilesize);

cleanup:
  curlx_dyn_free(&flags);
  curlx_free(mailbox);

  ಯದಿ(!result)
    imap_state(data, imapc, IMAP_APPEND);

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * imap_perform_search()
 *
 * Sends a SEARCH command.
 */
ಸ್ಥಿರ CURLcode imap_perform_ಹುಡುಕು(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ imap_conn *imapc,
                                    ಸಂರಚನಾ IMAP *imap)
{
  CURLcode result = CURLE_OK;

  /* Check we have a query string */
  ಯದಿ(!imap->query) {
    failf(data, "Cannot SEARCH without a query string.");
    ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;
  }

  /* Send the SEARCH command */
  result = imap_sendf(data, imapc, "SEARCH %s", imap->query);

  ಯದಿ(!result)
    imap_state(data, imapc, IMAP_ಹುಡುಕು);

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * imap_perform_logout()
 *
 * Performs the logout action prior to sclose() being called.
 */
ಸ್ಥಿರ CURLcode imap_perform_logout(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ imap_conn *imapc)
{
  /* Send the LOGOUT command */
  CURLcode result = imap_sendf(data, imapc, "LOGOUT");

  ಯದಿ(!result)
    imap_state(data, imapc, IMAP_LOGOUT);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For the initial server greeting */
ಸ್ಥಿರ CURLcode imap_state_servergreet_resp(ಸಂರಚನಾ Curl_easy *data,
                                            ಸಂರಚನಾ imap_conn *imapc,
                                            ಪೂರ್ಣಾಙ್ಕ imapcode,
                                            imapstate instate)
{
  (ಶೂನ್ಯ)instate;

  ಯದಿ(imapcode == IMAP_RESP_PREAUTH) {
    /* PREAUTH */
    imapc->preauth = TRUE;
    infof(data, "PREAUTH connection, already authenticated");
  }
  ಅನ್ಯಥಾ ಯದಿ(imapcode != IMAP_RESP_OK) {
    failf(data, "Got unexpected imap-server response");
    ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
  }

  ಪ್ರತ್ಯಾಗಮ imap_perform_capability(data, imapc);
}

/* For CAPABILITY responses */
ಸ್ಥಿರ CURLcode imap_state_capability_resp(ಸಂರಚನಾ Curl_easy *data,
                                           ಸಂರಚನಾ imap_conn *imapc,
                                           ಪೂರ್ಣಾಙ್ಕ imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ವರ್ಣ *ಸಾಲು = curlx_dyn_ptr(&imapc->pp.recvbuf);

  (ಶೂನ್ಯ)instate;

  /* Do we have an untagged response? */
  ಯದಿ(imapcode == '*') {
    ಸಾಲು += 2;

    /* Loop through the data line */
    ಪ್ರತಿ(;;) {
      size_t wordlen;
      ಯಾವತ್(*ಸಾಲು && (ISBLANK(*ಸಾಲು) || ISNEWLINE(*ಸಾಲು)))
        ಸಾಲು++;

      ಯದಿ(!*ಸಾಲು)
        ಭಙ್ಗ;

      /* Extract the word */
      ಪ್ರತಿ(wordlen = 0; ಸಾಲು[wordlen] && !ISBLANK(ಸಾಲು[wordlen]) &&
                       !ISNEWLINE(ಸಾಲು[wordlen]);)
        wordlen++;

      /* Does the server support the STARTTLS capability? */
      ಯದಿ(wordlen == 8 && curl_strnequal(ಸಾಲು, "STARTTLS", 8))
        imapc->tls_supported = TRUE;

      /* Has the server explicitly disabled clear text authentication? */
      ಅನ್ಯಥಾ ಯದಿ(wordlen == 13 && curl_strnequal(ಸಾಲು, "LOGINDISABLED", 13))
        imapc->login_disabled = TRUE;

      /* Does the server support the SASL-IR capability? */
      ಅನ್ಯಥಾ ಯದಿ(wordlen == 7 && curl_strnequal(ಸಾಲು, "SASL-IR", 7))
        imapc->ir_supported = TRUE;

      /* Do we have a SASL based authentication mechanism? */
      ಅನ್ಯಥಾ ಯದಿ(wordlen > 5 && curl_strnequal(ಸಾಲು, "AUTH=", 5)) {
        size_t llen;
        unsigned short mechbit;

        ಸಾಲು += 5;
        wordlen -= 5;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(ಸಾಲು, wordlen, &llen);
        ಯದಿ(mechbit && llen == wordlen)
          imapc->sasl.authmechs |= mechbit;
      }

      ಸಾಲು += wordlen;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(data->set.use_ssl && !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
    /* PREAUTH is not compatible with STARTTLS. */
    ಯದಿ(imapcode == IMAP_RESP_OK && imapc->tls_supported && !imapc->preauth) {
      /* Switch to TLS connection now */
      result = imap_perform_starttls(data, imapc);
    }
    ಅನ್ಯಥಾ ಯದಿ(data->set.use_ssl <= CURLUSESSL_TRY)
      result = imap_perform_authentication(data, imapc);
    ಅನ್ಯಥಾ {
      failf(data, "STARTTLS not available.");
      result = CURLE_USE_SSL_FAILED;
    }
  }
  ಅನ್ಯಥಾ
    result = imap_perform_authentication(data, imapc);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For STARTTLS responses */
ಸ್ಥಿರ CURLcode imap_state_starttls_resp(ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ imap_conn *imapc,
                                         ಪೂರ್ಣಾಙ್ಕ imapcode,
                                         imapstate instate)
{
  CURLcode result = CURLE_OK;

  (ಶೂನ್ಯ)instate;

  /* Pipelining in response is forbidden. */
  ಯದಿ(imapc->pp.overflow)
    ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;

  ಯದಿ(imapcode != IMAP_RESP_OK) {
    ಯದಿ(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied");
      result = CURLE_USE_SSL_FAILED;
    }
    ಅನ್ಯಥಾ
      result = imap_perform_authentication(data, imapc);
  }
  ಅನ್ಯಥಾ
    imap_state(data, imapc, IMAP_UPGRADETLS);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For SASL authentication responses */
ಸ್ಥಿರ CURLcode imap_state_auth_resp(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ imap_conn *imapc,
                                     ಪೂರ್ಣಾಙ್ಕ imapcode,
                                     imapstate instate)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  (ಶೂನ್ಯ)instate;

  result = Curl_sasl_continue(&imapc->sasl, data, imapcode, &progress);
  ಯದಿ(!result)
    ವಿಕಲ್ಪ(progress) {
    ಪಕ್ಷ SASL_DONE:
      imap_state(data, imapc, IMAP_STOP);  /* Authenticated */
      ಭಙ್ಗ;
    ಪಕ್ಷ SASL_IDLE:            /* No mechanism left after cancellation */
      ಯದಿ(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
        /* Perform clear text authentication */
        result = imap_perform_login(data, imapc, data->conn);
      ಅನ್ಯಥಾ {
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
      }
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      ಭಙ್ಗ;
    }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For LOGIN responses */
ಸ್ಥಿರ CURLcode imap_state_login_resp(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ imap_conn *imapc,
                                      ಪೂರ್ಣಾಙ್ಕ imapcode,
                                      imapstate instate)
{
  CURLcode result = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  ಯದಿ(imapcode != IMAP_RESP_OK) {
    failf(data, "Access denied. %c", imapcode);
    result = CURLE_LOGIN_DENIED;
  }
  ಅನ್ಯಥಾ
    /* End of connect phase */
    imap_state(data, imapc, IMAP_STOP);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* Detect IMAP listings vs. downloading a single email */
ಸ್ಥಿರ bool is_custom_fetch_listing_match(ಧ್ರುವ ವರ್ಣ *params)
{
  /* match " 1:* (FLAGS ..." or " 1,2,3 (FLAGS ..." */
  ಯದಿ(*params++ != ' ')
    ಪ್ರತ್ಯಾಗಮ FALSE;

  ಯಾವತ್(ISDIGIT(*params)) {
    params++;
    ಯದಿ(*params == 0)
      ಪ್ರತ್ಯಾಗಮ FALSE;
  }
  ಯದಿ(*params == ':')
    ಪ್ರತ್ಯಾಗಮ TRUE;
  ಯದಿ(*params == ',')
    ಪ್ರತ್ಯಾಗಮ TRUE;
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ bool is_custom_fetch_listing(ಸಂರಚನಾ IMAP *imap)
{
  /* filter out "UID FETCH 1:* (FLAGS ..." queries to list emails */
  ಯದಿ(!imap->custom)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಅನ್ಯಥಾ ಯದಿ(curl_strequal(imap->custom, "FETCH") && imap->custom_params) {
    ಧ್ರುವ ವರ್ಣ *p = imap->custom_params;
    ಪ್ರತ್ಯಾಗಮ is_custom_fetch_listing_match(p);
  }
  ಅನ್ಯಥಾ ಯದಿ(curl_strequal(imap->custom, "UID") && imap->custom_params &&
          curl_strnequal(imap->custom_params, " FETCH ", 7)) {
    ಧ್ರುವ ವರ್ಣ *p = imap->custom_params + 6;
    ಪ್ರತ್ಯಾಗಮ is_custom_fetch_listing_match(p);
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

/* For LIST and SEARCH responses */
ಸ್ಥಿರ CURLcode imap_state_listsearch_resp(ಸಂರಚನಾ Curl_easy *data,
                                           ಸಂರಚನಾ imap_conn *imapc,
                                           ಪೂರ್ಣಾಙ್ಕ imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ವರ್ಣ *ಸಾಲು = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  ಸಂರಚನಾ IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);

  DEBUGASSERT(imap);
  ಯದಿ(!imap)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  (ಶೂನ್ಯ)instate;

  ಯದಿ(imapcode == '*' && is_custom_fetch_listing(imap)) {
    /* custom FETCH or UID FETCH for listing is not handled here */
  }
  ಅನ್ಯಥಾ ಯದಿ(imapcode == '*') {
    /* Check if this response contains a literal (e.g. FETCH responses with
       body data). Literal syntax is {size}\r\n */
    ಧ್ರುವ ವರ್ಣ *cr = memchr(ಸಾಲು, '\r', len);
    size_t ಸಾಲು_len = cr ? (size_t)(cr - ಸಾಲು) : len;
    ಧ್ರುವ ವರ್ಣ *ptr = imap_find_literal(ಸಾಲು, ಸಾಲು_len);
    ಯದಿ(ptr) {
      curl_off_t size = 0;
      bool parsed = FALSE;
      ptr++;
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&ptr, &size, CURL_OFF_T_MAX) &&
         !curlx_str_single(&ptr, '}'))
        parsed = TRUE;

      ಯದಿ(parsed) {
        ಸಂರಚನಾ pingpong *pp = &imapc->pp;
        size_t buffer_len = curlx_dyn_len(&pp->recvbuf);
        size_t after_header = buffer_len - pp->nfinal;

        /* This is a literal response, setup to receive the body data */
        infof(data, "Found %" FMT_OFF_T " bytes to download", size);

        /* First write the header line */
        result = Curl_client_write(data, CLIENTWRITE_BODY, ಸಾಲು, len);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;

        /* Handle data already in buffer after the header line */
        ಯದಿ(after_header > 0) {
          /* There is already data in the buffer that is part of the literal
             body or subsequent responses */
          size_t chunk = after_header;

          /* Keep only the data after the header line */
          curlx_dyn_tail(&pp->recvbuf, chunk);
          pp->nfinal = 0; /* done */

          /* Limit chunk to the literal size */
          ಯದಿ((curl_off_t)chunk > size)
            chunk = (size_t)size;

          ಯದಿ(chunk) {
            /* Write the literal body data */
            result = Curl_client_write(data, CLIENTWRITE_BODY,
                                       curlx_dyn_ptr(&pp->recvbuf), chunk);
            ಯದಿ(result)
              ಪ್ರತ್ಯಾಗಮ result;
          }

          /* Handle remaining data in buffer (either more literal data or
             subsequent responses) */
          ಯದಿ(after_header > chunk) {
            /* Keep the data after the literal body */
            pp->overflow = after_header - chunk;
            curlx_dyn_tail(&pp->recvbuf, pp->overflow);
          }
          ಅನ್ಯಥಾ {
            pp->overflow = 0;
            curlx_dyn_reset(&pp->recvbuf);
          }
        }
        ಅನ್ಯಥಾ {
          /* No data in buffer yet, reset overflow */
          pp->overflow = 0;
        }

        ಯದಿ((CURL_OFF_T_MAX - size) < (curl_off_t)len)
          /* unlikely to actually be a transfer this big, but avoid integer
             overflow */
          size = CURL_OFF_T_MAX;
        ಅನ್ಯಥಾ
          size += len;

        /* Progress size includes both header line and literal body */
        Curl_pgrsSetDownloadSize(data, size);

        ಯದಿ(data->req.bytecount == size)
          /* All data already transferred (header + literal body) */
          Curl_xfer_setup_nop(data);
        ಅನ್ಯಥಾ {
          /* Setup to receive the literal body data.
             maxdownload and transfer size include both header line and
             literal body */
          data->req.maxdownload = size;
          Curl_xfer_setup_recv(data, FIRSTSOCKET, size);
        }
        /* End of DO phase */
        imap_state(data, imapc, IMAP_STOP);
      }
      ಅನ್ಯಥಾ {
        /* Failed to parse literal, write the line */
        result = Curl_client_write(data, CLIENTWRITE_BODY, ಸಾಲು, len);
      }
    }
    ಅನ್ಯಥಾ {
      /* No literal, write the line as-is */
      result = Curl_client_write(data, CLIENTWRITE_BODY, ಸಾಲು, len);
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(imapcode != IMAP_RESP_OK)
    result = CURLE_QUOTE_ERROR;
  ಅನ್ಯಥಾ
    /* End of DO phase */
    imap_state(data, imapc, IMAP_STOP);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For SELECT responses */
ಸ್ಥಿರ CURLcode imap_state_select_resp(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ imap_conn *imapc,
                                       ಸಂರಚನಾ IMAP *imap,
                                       ಪೂರ್ಣಾಙ್ಕ imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  ಯದಿ(imapcode == '*') {
    /* See if this is an UIDVALIDITY response */
    ಧ್ರುವ ವರ್ಣ *ಸಾಲು = curlx_dyn_ptr(&imapc->pp.recvbuf);
    size_t len = curlx_dyn_len(&imapc->pp.recvbuf);
    ಯದಿ((len >= 18) && checkprefix("OK [UIDVALIDITY ", &ಸಾಲು[2])) {
      curl_off_t ಮೌಲ್ಯ;
      ಧ್ರುವ ವರ್ಣ *p = &ಸಾಲು[2] + CURL_CSTRLEN("OK [UIDVALIDITY ");
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &ಮೌಲ್ಯ, UINT_MAX)) {
        imapc->mb_uidvalidity = (unsigned ಪೂರ್ಣಾಙ್ಕ)ಮೌಲ್ಯ;
        imapc->mb_uidvalidity_set = TRUE;
      }
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(imapcode == IMAP_RESP_OK) {
    /* Check if the UIDVALIDITY has been specified and matches */
    ಯದಿ(imap->uidvalidity_set && imapc->mb_uidvalidity_set &&
       (imap->uidvalidity != imapc->mb_uidvalidity)) {
      failf(data, "Mailbox UIDVALIDITY has changed");
      result = CURLE_REMOTE_FILE_NOT_FOUND;
    }
    ಅನ್ಯಥಾ {
      /* Note the currently opened mailbox on this connection */
      DEBUGASSERT(!imapc->mailbox);
      imapc->mailbox = curlx_strdup(imap->mailbox);
      ಯದಿ(!imapc->mailbox)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

      ಯದಿ(imap->custom)
        result = imap_perform_list(data, imapc, imap);
      ಅನ್ಯಥಾ ಯದಿ(imap->query)
        result = imap_perform_ಹುಡುಕು(data, imapc, imap);
      ಅನ್ಯಥಾ
        result = imap_perform_fetch(data, imapc, imap);
    }
  }
  ಅನ್ಯಥಾ {
    failf(data, "Select failed");
    result = CURLE_LOGIN_DENIED;
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For the (first line of the) FETCH responses */
ಸ್ಥಿರ CURLcode imap_state_fetch_resp(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ imap_conn *imapc,
                                      ಪೂರ್ಣಾಙ್ಕ imapcode,
                                      imapstate instate)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ pingpong *pp = &imapc->pp;
  ಧ್ರುವ ವರ್ಣ *ptr = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  bool parsed = FALSE;
  curl_off_t size = 0;

  (ಶೂನ್ಯ)instate;

  ಯದಿ(imapcode != '*') {
    Curl_pgrsSetDownloadSize(data, -1);
    imap_state(data, imapc, IMAP_STOP);
    ಪ್ರತ್ಯಾಗಮ CURLE_REMOTE_FILE_NOT_FOUND;
  }

  /* Something like this is received "* 1 FETCH (BODY[TEXT] {2021}\r" so parse
     the continuation data contained within the curly brackets */
  ptr = imap_find_literal(ptr, len);
  ಯದಿ(ptr) {
    ptr++;
    ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&ptr, &size, CURL_OFF_T_MAX) &&
       !curlx_str_single(&ptr, '}'))
      parsed = TRUE;
  }

  ಯದಿ(parsed) {
    infof(data, "Found %" FMT_OFF_T " bytes to download", size);
    Curl_pgrsSetDownloadSize(data, size);

    ಯದಿ(pp->overflow) {
      /* At this point there is a data in the receive buffer that is body
         content, send it as body and then skip it. Do note that there may
         even be additional "headers" after the body. */
      size_t chunk = pp->overflow;

      /* keep only the overflow */
      curlx_dyn_tail(&pp->recvbuf, chunk);
      pp->nfinal = 0; /* done */

      ಯದಿ((curl_off_t)chunk > size)
        /* The conversion from curl_off_t to size_t is always fine here */
        chunk = (size_t)size;

      ಯದಿ(!chunk) {
        /* no size, we are done with the data */
        imap_state(data, imapc, IMAP_STOP);
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 curlx_dyn_ptr(&pp->recvbuf), chunk);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;

      infof(data, "Written %zu bytes, %" FMT_OFF_T
            " bytes are left for transfer", chunk, (curl_off_t)(size - chunk));

      /* Have we used the entire overflow or part of it?*/
      ಯದಿ(pp->overflow > chunk) {
        /* remember the remaining trailing overflow data */
        pp->overflow -= chunk;
        curlx_dyn_tail(&pp->recvbuf, pp->overflow);
      }
      ಅನ್ಯಥಾ {
        pp->overflow = 0; /* handled */
        /* Free the cache */
        curlx_dyn_reset(&pp->recvbuf);
      }
    }

    ಯದಿ(data->req.bytecount == size)
      /* The entire data is already transferred! */
      Curl_xfer_setup_nop(data);
    ಅನ್ಯಥಾ {
      /* IMAP download */
      data->req.maxdownload = size;
      Curl_xfer_setup_recv(data, FIRSTSOCKET, size);
    }
  }
  ಅನ್ಯಥಾ {
    /* We do not know how to parse this line */
    failf(data, "Failed to parse FETCH response.");
    result = CURLE_WEIRD_SERVER_REPLY;
  }

  /* End of DO phase */
  imap_state(data, imapc, IMAP_STOP);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For final FETCH responses performed after the download */
ಸ್ಥಿರ CURLcode imap_state_fetch_final_resp(ಸಂರಚನಾ Curl_easy *data,
                                            ಸಂರಚನಾ imap_conn *imapc,
                                            ಪೂರ್ಣಾಙ್ಕ imapcode,
                                            imapstate instate)
{
  CURLcode result = CURLE_OK;

  (ಶೂನ್ಯ)instate;

  ಯದಿ(imapcode != IMAP_RESP_OK)
    result = CURLE_WEIRD_SERVER_REPLY;
  ಅನ್ಯಥಾ
    /* End of DONE phase */
    imap_state(data, imapc, IMAP_STOP);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For APPEND responses */
ಸ್ಥಿರ CURLcode imap_state_append_resp(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ imap_conn *imapc,
                                       ಪೂರ್ಣಾಙ್ಕ imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  ಯದಿ(imapcode != '+') {
    result = CURLE_UPLOAD_FAILED;
  }
  ಅನ್ಯಥಾ {
    /* Set the progress upload size */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* IMAP upload */
    Curl_xfer_setup_send(data, FIRSTSOCKET);

    /* End of DO phase */
    imap_state(data, imapc, IMAP_STOP);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For final APPEND responses performed after the upload */
ಸ್ಥಿರ CURLcode imap_state_append_final_resp(ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ imap_conn *imapc,
                                             ಪೂರ್ಣಾಙ್ಕ imapcode,
                                             imapstate instate)
{
  CURLcode result = CURLE_OK;

  (ಶೂನ್ಯ)instate;

  ಯದಿ(imapcode != IMAP_RESP_OK)
    result = CURLE_UPLOAD_FAILED;
  ಅನ್ಯಥಾ
    /* End of DONE phase */
    imap_state(data, imapc, IMAP_STOP);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode imap_pp_statemachine(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ connectdata *conn)
{
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ imapcode;
  ಸಂರಚನಾ imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ಸಂರಚನಾ IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  ಸಂರಚನಾ pingpong *pp;
  size_t nread = 0;

  ಯದಿ(!imapc || !imap)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  pp = &imapc->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not IMAP */
upgrade_tls:
  ಯದಿ(imapc->state == IMAP_UPGRADETLS) {
    result = imap_perform_upgrade_tls(data, imapc, conn);
    ಯದಿ(result || (imapc->state == IMAP_UPGRADETLS))
      ಪ್ರತ್ಯಾಗಮ result;
  }

  /* Flush any data that needs to be sent */
  ಯದಿ(pp->sendleft)
    ಪ್ರತ್ಯಾಗಮ Curl_pp_flushsend(data, pp);

  ಕುರು {
    /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, pp, &imapcode, &nread);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    /* Was there an error parsing the response line? */
    ಯದಿ(imapcode == -1)
      ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;

    ಯದಿ(!imapcode)
      ಭಙ್ಗ;

    /* We have now received a full IMAP server response */
    ವಿಕಲ್ಪ(imapc->state) {
    ಪಕ್ಷ IMAP_SERVERGREET:
      result = imap_state_servergreet_resp(data, imapc,
                                           imapcode, imapc->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ IMAP_CAPABILITY:
      result = imap_state_capability_resp(data, imapc, imapcode, imapc->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ IMAP_STARTTLS:
      result = imap_state_starttls_resp(data, imapc, imapcode, imapc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      ಯದಿ(!result && (imapc->state == IMAP_UPGRADETLS))
        ಗಚ್ಛ upgrade_tls;
      ಭಙ್ಗ;

    ಪಕ್ಷ IMAP_AUTHENTICATE:
      result = imap_state_auth_resp(data, imapc, imapcode, imapc->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ IMAP_LOGIN:
      result = imap_state_login_resp(data, imapc, imapcode, imapc->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ IMAP_LIST:
    ಪಕ್ಷ IMAP_ಹುಡುಕು:
      result = imap_state_listsearch_resp(data, imapc, imapcode, imapc->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ IMAP_SELECT:
      result = imap_state_select_resp(data, imapc, imap,
                                      imapcode, imapc->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ IMAP_FETCH:
      result = imap_state_fetch_resp(data, imapc, imapcode, imapc->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ IMAP_FETCH_FINAL:
      result = imap_state_fetch_final_resp(data, imapc,
                                           imapcode, imapc->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ IMAP_APPEND:
      result = imap_state_append_resp(data, imapc, imapcode, imapc->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ IMAP_APPEND_FINAL:
      result = imap_state_append_final_resp(data, imapc,
                                            imapcode, imapc->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ IMAP_LOGOUT:
    ಸ್ವಭಾವ:
      /* internal error */
      imap_state(data, imapc, IMAP_STOP);
      ಭಙ್ಗ;
    }
  } ಯಾವತ್(!result && imapc->state != IMAP_STOP && Curl_pp_moredata(pp));

  ಪ್ರತ್ಯಾಗಮ result;
}

/* Called repeatedly until done from multi.c */
ಸ್ಥಿರ CURLcode imap_multi_statemach(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  *done = FALSE;
  ಯದಿ(!imapc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  result = Curl_pp_statemach(data, &imapc->pp, FALSE, FALSE);
  *done = (imapc->state == IMAP_STOP);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode imap_block_statemach(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ imap_conn *imapc,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;

  ಯಾವತ್(imapc->state != IMAP_STOP && !result)
    result = Curl_pp_statemach(data, &imapc->pp, TRUE, disconnecting);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For the IMAP "protocol connect" and "doing" phases only */
ಸ್ಥಿರ CURLcode imap_pollset(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  ಪ್ರತ್ಯಾಗಮ imapc ? Curl_pp_pollset(data, &imapc->pp, ps) : CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ imap_easy_reset(ಸಂರಚನಾ IMAP *imap)
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
ಸ್ಥಿರ bool imap_is_bchar(ವರ್ಣ ch)
{
  /* Performing the alnum check first with macro is faster because of ASCII
     arithmetic */
  ಪ್ರತ್ಯಾಗಮ ch && (ISALNUM(ch) || strchr(":@/&=-._~!$\'()*+,%", ch));
}

/***********************************************************************
 *
 * imap_parse_url_options()
 *
 * Parse the URL login options.
 */
ಸ್ಥಿರ CURLcode imap_parse_url_options(ಸಂರಚನಾ connectdata *conn,
                                       ಸಂರಚನಾ imap_conn *imapc)
{
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ವರ್ಣ *ptr = conn->options;
  bool prefer_login = FALSE;

  ಯಾವತ್(!result && ptr && *ptr) {
    ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ = ptr;
    ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ;

    ಯಾವತ್(*ptr && *ptr != '=')
      ptr++;

    ಮೌಲ್ಯ = ptr + 1;

    ಯಾವತ್(*ptr && *ptr != ';')
      ptr++;

    ಯದಿ(curl_strnequal(ಕೀಲಿ, "AUTH=+LOGIN", 11)) {
      /* User prefers plaintext LOGIN over any SASL, including SASL LOGIN */
      prefer_login = TRUE;
      imapc->sasl.prefmech = SASL_AUTH_NONE;
    }
    ಅನ್ಯಥಾ ಯದಿ(curl_strnequal(ಕೀಲಿ, "AUTH=", 5)) {
      prefer_login = FALSE;
      result = Curl_sasl_parse_url_auth_option(&imapc->sasl,
                                               ಮೌಲ್ಯ, ptr - ಮೌಲ್ಯ);
    }
    ಅನ್ಯಥಾ {
      prefer_login = FALSE;
      result = CURLE_URL_MALFORMAT;
    }

    ಯದಿ(*ptr == ';')
      ptr++;
  }

  ಯದಿ(prefer_login)
    imapc->preftype = IMAP_TYPE_CLEARTEXT;
  ಅನ್ಯಥಾ {
    ವಿಕಲ್ಪ(imapc->sasl.prefmech) {
    ಪಕ್ಷ SASL_AUTH_NONE:
      imapc->preftype = IMAP_TYPE_NONE;
      ಭಙ್ಗ;
    ಪಕ್ಷ SASL_AUTH_DEFAULT:
      imapc->preftype = IMAP_TYPE_ANY;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      imapc->preftype = IMAP_TYPE_SASL;
      ಭಙ್ಗ;
    }
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * imap_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
ಸ್ಥಿರ CURLcode imap_parse_url_path(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ IMAP *imap)
{
  /* The imap struct is already initialized in imap_connect() */
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ವರ್ಣ *begin = &data->state.up.path[1]; /* skip leading slash */
  ಧ್ರುವ ವರ್ಣ *ptr = begin;

  /* See how much of the URL is a valid path and decode it */
  ಯಾವತ್(imap_is_bchar(*ptr))
    ptr++;

  ಯದಿ(ptr != begin) {
    /* Remove the trailing slash if present */
    ಧ್ರುವ ವರ್ಣ *end = ptr;
    ಯದಿ(end > begin && end[-1] == '/')
      end--;

    result = Curl_urldecode(begin, end - begin, &imap->mailbox, ರಿಕ್ತ,
                            REJECT_CTRL);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಅನ್ಯಥಾ
    imap->mailbox = ರಿಕ್ತ;

  /* There can be any number of parameters in the form ";NAME=VALUE" */
  ಯಾವತ್(*ptr == ';') {
    ವರ್ಣ *name;
    ವರ್ಣ *ಮೌಲ್ಯ;
    size_t valuelen;

    /* Find the length of the name parameter */
    begin = ++ptr;
    ಯಾವತ್(*ptr && *ptr != '=')
      ptr++;

    ಯದಿ(!*ptr)
      ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;

    /* Decode the name parameter */
    result = Curl_urldecode(begin, ptr - begin, &name, ರಿಕ್ತ,
                            REJECT_CTRL);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    /* Find the length of the value parameter */
    begin = ++ptr;
    ಯಾವತ್(imap_is_bchar(*ptr))
      ptr++;

    /* Decode the value parameter */
    result = Curl_urldecode(begin, ptr - begin, &ಮೌಲ್ಯ, &valuelen,
                            REJECT_CTRL);
    ಯದಿ(result) {
      curlx_free(name);
      ಪ್ರತ್ಯಾಗಮ result;
    }

    DEBUGF(infof(data, "IMAP URL parameter '%s' = '%s'", name, ಮೌಲ್ಯ));

    /* Process the known hierarchical parameters (UIDVALIDITY, UID, SECTION
       and PARTIAL) stripping of the trailing slash character if it is
       present.

       Note: Unknown parameters trigger a URL_MALFORMAT error. */
    ಯದಿ(valuelen > 0 && ಮೌಲ್ಯ[valuelen - 1] == '/')
      ಮೌಲ್ಯ[valuelen - 1] = '\0';
    ಯದಿ(valuelen) {
      ಯದಿ(curl_strequal(name, "UIDVALIDITY") && !imap->uidvalidity_set) {
        curl_off_t ಅಂಕೆ;
        ಧ್ರುವ ವರ್ಣ *p = (ಧ್ರುವ ವರ್ಣ *)ಮೌಲ್ಯ;
        ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &ಅಂಕೆ, UINT_MAX)) {
          imap->uidvalidity = (unsigned ಪೂರ್ಣಾಙ್ಕ)ಅಂಕೆ;
          imap->uidvalidity_set = TRUE;
        }
        curlx_free(ಮೌಲ್ಯ);
      }
      ಅನ್ಯಥಾ ಯದಿ(curl_strequal(name, "UID") && !imap->uid) {
        imap->uid = ಮೌಲ್ಯ;
      }
      ಅನ್ಯಥಾ ಯದಿ(curl_strequal(name, "MAILINDEX") && !imap->mindex) {
        imap->mindex = ಮೌಲ್ಯ;
      }
      ಅನ್ಯಥಾ ಯದಿ(curl_strequal(name, "SECTION") && !imap->section) {
        imap->section = ಮೌಲ್ಯ;
      }
      ಅನ್ಯಥಾ ಯದಿ(curl_strequal(name, "PARTIAL") && !imap->partial) {
        imap->partial = ಮೌಲ್ಯ;
      }
      ಅನ್ಯಥಾ {
        curlx_free(name);
        curlx_free(ಮೌಲ್ಯ);
        ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;
      }
    }
    ಅನ್ಯಥಾ
      /* blank? */
      curlx_free(ಮೌಲ್ಯ);
    curlx_free(name);
  }

  /* Does the URL contain a query parameter? Only valid when we have a mailbox
     and no UID as per RFC-5092 */
  ಯದಿ(imap->mailbox && !imap->uid && !imap->mindex) {
    /* Get the query parameter, URL decoded */
    CURLUcode uc = curl_url_get(data->state.uh, CURLUPART_QUERY, &imap->query,
                                CURLU_URLDECODE);
    ಯದಿ(uc == CURLUE_OUT_OF_MEMORY)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  /* Any extra stuff at the end of the URL is an error */
  ಯದಿ(*ptr)
    ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/***********************************************************************
 *
 * imap_parse_custom_request()
 *
 * Parse the custom request.
 */
ಸ್ಥಿರ CURLcode imap_parse_custom_request(ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ IMAP *imap)
{
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ವರ್ಣ *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  ಯದಿ(custom) {
    /* URL decode the custom request */
    result = Curl_urldecode(custom, 0, &imap->custom, ರಿಕ್ತ, REJECT_CTRL);

    /* Extract the parameters if specified */
    ಯದಿ(!result) {
      ಧ್ರುವ ವರ್ಣ *params = imap->custom;

      ಯಾವತ್(*params && *params != ' ')
        params++;

      ಯದಿ(*params) {
        imap->custom_params = curlx_strdup(params);
        imap->custom[params - imap->custom] = '\0';

        ಯದಿ(!imap->custom_params)
          result = CURLE_OUT_OF_MEMORY;
      }
    }
  }

  ಪ್ರತ್ಯಾಗಮ result;
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
ಸ್ಥಿರ CURLcode imap_connect(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಸಂರಚನಾ imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  CURLcode result = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  ಯದಿ(!imapc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* Parse the URL options */
  result = imap_parse_url_options(data->conn, imapc);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* Start off waiting for the server greeting response */
  imap_state(data, imapc, IMAP_SERVERGREET);

  /* Start off with an response id of '*' */
  curlx_strcopy(imapc->resptag, ಪರಿಮಾಣ(imapc->resptag), STRCONST("*"));

  result = imap_multi_statemach(data, done);

  ಪ್ರತ್ಯಾಗಮ result;
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
ಸ್ಥಿರ CURLcode imap_done(ಸಂರಚನಾ Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ಸಂರಚನಾ IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);

  ಯದಿ(!imapc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಯದಿ(!imap)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಯದಿ(status || premature) {
    CURL_TRC_M(data, "IMAP done, not finalizing transfer");
    connclose(conn); /* marked for closure */
    result = status;         /* use the already set error code */
  }
  ಅನ್ಯಥಾ ಯದಿ(!data->set.connect_only &&
          ((!imap->custom && (imap->uid || imap->mindex)) ||
           (imap->custom && data->req.maxdownload > 0) ||
           data->state.upload || IS_MIME_POST(data))) {
    /* Handle responses after FETCH or APPEND transfer has finished.
       For custom commands, check if we set up a download which indicates
       a FETCH-like command with literal data. */

    ಯದಿ(!data->state.upload && !IS_MIME_POST(data))
      imap_state(data, imapc, IMAP_FETCH_FINAL);
    ಅನ್ಯಥಾ {
      /* End the APPEND command first by sending an empty line */
      result = Curl_pp_sendf(data, &imapc->pp, "%s", "");
      ಯದಿ(!result)
        imap_state(data, imapc, IMAP_APPEND_FINAL);
    }

    /* Run the state-machine */
    ಯದಿ(!result)
      result = imap_block_statemach(data, imapc, FALSE);
  }

  imap_easy_reset(imap);
  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * imap_perform()
 *
 * This is the actual DO function for IMAP. Fetch or append a message, or do
 * other things according to the options previously setup.
 */
ಸ್ಥಿರ CURLcode imap_perform(ಸಂರಚನಾ Curl_easy *data, bool *connected,
                             bool *dophase_done)
{
  /* This is IMAP and no proxy */
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ಸಂರಚನಾ IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  bool selected = FALSE;

  DEBUGF(infof(data, "DO phase starts"));
  ಯದಿ(!imapc || !imap)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯದಿ(data->req.no_body) {
    /* Requested no body means no transfer */
    imap->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* Determine if the requested mailbox (with the same UIDVALIDITY if set)
     has already been selected on this connection */
  ಯದಿ(imap->mailbox && imapc->mailbox &&
     (!strcmp(imap->mailbox, imapc->mailbox) ||
      (curl_strequal(imap->mailbox, "INBOX") &&
       curl_strequal(imapc->mailbox, "INBOX"))) &&
     (!imap->uidvalidity_set || !imapc->mb_uidvalidity_set ||
      (imap->uidvalidity == imapc->mb_uidvalidity)))
    selected = TRUE;

  /* Start the first command in the DO phase */
  ಯದಿ(data->state.upload || IS_MIME_POST(data))
    /* APPEND can be executed directly */
    result = imap_perform_append(data, imapc, imap);
  ಅನ್ಯಥಾ ಯದಿ(imap->custom && (selected || !imap->mailbox))
    /* Custom command using the same mailbox or no mailbox */
    result = imap_perform_list(data, imapc, imap);
  ಅನ್ಯಥಾ ಯದಿ(!imap->custom && selected && (imap->uid || imap->mindex))
    /* FETCH from the same mailbox */
    result = imap_perform_fetch(data, imapc, imap);
  ಅನ್ಯಥಾ ಯದಿ(!imap->custom && selected && imap->query)
    /* SEARCH the current mailbox */
    result = imap_perform_ಹುಡುಕು(data, imapc, imap);
  ಅನ್ಯಥಾ ಯದಿ(imap->mailbox && !selected &&
          (imap->custom || imap->uid || imap->mindex || imap->query))
    /* SELECT the mailbox */
    result = imap_perform_select(data, imapc, imap);
  ಅನ್ಯಥಾ
    /* LIST */
    result = imap_perform_list(data, imapc, imap);

  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* Run the state-machine */
  result = imap_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(conn, FIRSTSOCKET);

  ಯದಿ(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  ಪ್ರತ್ಯಾಗಮ result;
}

/* Call this when the DO phase has completed */
ಸ್ಥಿರ CURLcode imap_dophase_done(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ IMAP *imap,
                                  bool connected)
{
  (ಶೂನ್ಯ)connected;

  ಯದಿ(imap->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
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
ಸ್ಥಿರ CURLcode imap_regular_transfer(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ IMAP *imap,
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
  ಯದಿ(!result && *dophase_done)
    result = imap_dophase_done(data, imap, connected);

  ಪ್ರತ್ಯಾಗಮ result;
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
ಸ್ಥಿರ CURLcode imap_do(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಸಂರಚನಾ IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* default to false */

  ಯದಿ(!imap)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  /* Parse the URL path */
  result = imap_parse_url_path(data, imap);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* Parse the custom request */
  result = imap_parse_custom_request(data, imap);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  result = imap_regular_transfer(data, imap, done);

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * imap_disconnect()
 *
 * Disconnect from an IMAP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ಸ್ಥಿರ CURLcode imap_disconnect(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ connectdata *conn, bool dead_connection)
{
  ಸಂರಚನಾ imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);

  ಯದಿ(imapc &&
     /* We cannot send quit unconditionally. If this connection is stale or
        bad in any way (pingpong has pending data to send),
        sending quit and waiting around here will make the
        disconnect wait in vain and cause more problems than we need to. */
     !dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &imapc->pp) &&
     !imap_perform_logout(data, imapc))
    (ಶೂನ್ಯ)imap_block_statemach(data, imapc, TRUE); /* ignore errors */

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Called from multi.c while DOing */
ಸ್ಥಿರ CURLcode imap_doing(ಸಂರಚನಾ Curl_easy *data, bool *dophase_done)
{
  ಸಂರಚನಾ IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  CURLcode result;

  ಯದಿ(!imap)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  result = imap_multi_statemach(data, dophase_done);
  ಯದಿ(result)
    DEBUGF(infof(data, "DO phase failed"));
  ಅನ್ಯಥಾ ಯದಿ(*dophase_done) {
    result = imap_dophase_done(data, imap, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ imap_easy_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ IMAP *imap = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  imap_easy_reset(imap);
  curlx_free(imap);
}

ಸ್ಥಿರ ಶೂನ್ಯ imap_conn_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ imap_conn *imapc = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  Curl_pp_disconnect(&imapc->pp);
  curlx_safefree(imapc->mailbox);
  curlx_free(imapc);
}

/* SASL parameters for the imap protocol */
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ SASLproto saslimap = {
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

ಸ್ಥಿರ CURLcode imap_setup_connection(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ imap_conn *imapc;
  ಸಂರಚನಾ pingpong *pp;
  ಸಂರಚನಾ IMAP *imap;

  imapc = curlx_calloc(1, ಪರಿಮಾಣ(*imapc));
  ಯದಿ(!imapc)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  pp = &imapc->pp;
  PINGPONG_SETUP(pp, imap_pp_statemachine, imap_endofresp);

  /* Set the default preferred authentication type and mechanism */
  imapc->preftype = IMAP_TYPE_ANY;
  Curl_sasl_init(&imapc->sasl, data, &saslimap);

  Curl_pp_init(pp, Curl_pgrs_now(data));

  ಯದಿ(Curl_conn_meta_set(conn, CURL_META_IMAP_CONN, imapc, imap_conn_dtor))
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  imap = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ IMAP));
  ಯದಿ(!imap ||
     Curl_meta_set(data, CURL_META_IMAP_EASY, imap, imap_easy_dtor))
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * IMAP protocol.
 */
ಧ್ರುವ ಸಂರಚನಾ Curl_protocol Curl_protocol_imap = {
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
