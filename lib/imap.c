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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "imap.h"

#ifndef CURL_DISABLE_IMAP

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef __VMS
#ଅନ୍ତର୍ଭାଵ <in.h>
#ଅନ୍ତର୍ଭାଵ <inet.h>
#endif

#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"
#ଅନ୍ତର୍ଭାଵ "pingpong.h"
#ଅନ୍ତର୍ଭାଵ "mime.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "bufref.h"
#ଅନ୍ତର୍ଭାଵ "curl_sasl.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strcopy.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_IMAP_EASY   "meta:proto:imap:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_IMAP_CONN   "meta:proto:imap:conn"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
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
  IMAP_ସନ୍ଧାନ,
  IMAP_LOGOUT,
  IMAP_LAST          /* never used */
} imapstate;

/* imap_conn is used for struct connection-oriented data */
ସଂରଚନା imap_conn {
  ସଂରଚନା pingpong pp;
  ସଂରଚନା SASL sasl;           /* SASL-related parameters */
  ଅକ୍ଷର *mailbox;              /* The last selected mailbox */
  imapstate state;            /* Always use imap.c:state() to change state! */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା mb_uidvalidity; /* UIDVALIDITY parsed from select response */
  ଅକ୍ଷର resptag[5];            /* Response tag to wait for */
  unsigned ଅକ୍ଷର preftype;     /* Preferred authentication type */
  unsigned ଅକ୍ଷର cmdid;        /* Last used command ID */
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
ସଂରଚନା IMAP {
  curl_pp_transfer transfer;
  ଅକ୍ଷର *mailbox;          /* Mailbox to select */
  ଅକ୍ଷର *uid;              /* Message UID to fetch */
  ଅକ୍ଷର *mindex;           /* Index in mail box of mail to fetch */
  ଅକ୍ଷର *section;          /* Message SECTION to fetch */
  ଅକ୍ଷର *partial;          /* Message PARTIAL to fetch */
  ଅକ୍ଷର *query;            /* Query to search for */
  ଅକ୍ଷର *custom;           /* Custom request */
  ଅକ୍ଷର *custom_params;    /* Parameters for the custom request */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା uidvalidity; /* UIDVALIDITY to check in select */
  BIT(uidvalidity_set);
};

#define IMAP_RESP_OK       1
#define IMAP_RESP_NOT_OK   2
#define IMAP_RESP_PREAUTH  3

ସଂରଚନା ulbits {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା bit;
  ଅଚଳ ଅକ୍ଷର *flag;
};

/***********************************************************************
 *
 * imap_sendf()
 *
 * Sends the formatted string as an IMAP command to the server.
 *
 * Designed to never block.
 */
ସ୍ଥିର CURLcode imap_sendf(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା imap_conn *imapc,
                           ଅଚଳ ଅକ୍ଷର *fmt, ...) CURL_PRINTF(3, 4);
ସ୍ଥିର CURLcode imap_sendf(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା imap_conn *imapc,
                           ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  CURLcode result = CURLE_OK;
  va_list ap;

  DEBUGASSERT(fmt);

  /* Calculate the tag based on the connection ID and command ID */
  curl_msnprintf(imapc->resptag, ଆକାର(imapc->resptag), "%c%03d",
                 'A' + curlx_sltosi((long)(data->conn->connection_id % 26)),
                 ++imapc->cmdid);

  va_start(ap, fmt);
  result = Curl_pp_vsendf(data, &imapc->pp, imapc->resptag, fmt, ap);
  va_end(ap);
  ଫେରନ୍ତୁ result;
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
ସ୍ଥିର ଅକ୍ଷର *imap_atom(ଅଚଳ ଅକ୍ଷର *str, bool escape_only)
{
  ସଂରଚନା dynbuf ଧାଡ଼ି;
  size_t nclean;
  size_t len;

  ଯଦି(!str)
    ଫେରନ୍ତୁ ଖାଲି;

  len = strlen(str);
  nclean = strcspn(str, "() {%*]\\\"");
  ଯଦି(len == nclean)
    /* nothing to escape, return a strdup */
    ଫେରନ୍ତୁ curlx_strdup(str);

  curlx_dyn_init(&ଧାଡ଼ି, 2000);

  ଯଦି(!escape_only && curlx_dyn_addn(&ଧାଡ଼ି, "\"", 1))
    ଫେରନ୍ତୁ ଖାଲି;

  ଯେତେବେଳେ(*str) {
    ଯଦି((*str == '\\' || *str == '"') &&
       curlx_dyn_addn(&ଧାଡ଼ି, "\\", 1))
      ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି(curlx_dyn_addn(&ଧାଡ଼ି, str, 1))
      ଫେରନ୍ତୁ ଖାଲି;
    str++;
  }

  ଯଦି(!escape_only && curlx_dyn_addn(&ଧାଡ଼ି, "\"", 1))
    ଫେରନ୍ତୁ ଖାଲି;

  ଫେରନ୍ତୁ curlx_dyn_ptr(&ଧାଡ଼ି);
}

/*
 * Finds the start of a literal '{size}' in line, skipping over quoted strings.
 */
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *imap_find_literal(ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି, size_t len)
{
  ଅଚଳ ଅକ୍ଷର *end = ଧାଡ଼ି + len;
  bool in_quote = FALSE;

  ଯେତେବେଳେ(ଧାଡ଼ି < end) {
    ଯଦି(in_quote) {
      ଯଦି(*ଧାଡ଼ି == '\\' && (ଧାଡ଼ି + 1) < end) {
        ଧାଡ଼ି += 2;
        ଜାରି;
      }
      ଯଦି(*ଧାଡ଼ି == '"')
        in_quote = FALSE;
    }
    ଅନ୍ୟଥା {
      ଯଦି(*ଧାଡ଼ି == '"')
        in_quote = TRUE;
      ଅନ୍ୟଥା ଯଦି(*ଧାଡ଼ି == '{')
        ଫେରନ୍ତୁ ଧାଡ଼ି;
    }
    ଧାଡ଼ି++;
  }
  ଫେରନ୍ତୁ ଖାଲି;
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
ସ୍ଥିର bool imap_matchresp(ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି, size_t len, ଅଚଳ ଅକ୍ଷର *cmd)
{
  ଅଚଳ ଅକ୍ଷର *end = ଧାଡ଼ି + len;
  size_t cmd_len = strlen(cmd);

  /* Skip the untagged response marker */
  ଧାଡ଼ି += 2;

  /* Do we have a number after the marker? */
  ଯଦି(ଧାଡ଼ି < end && ISDIGIT(*ଧାଡ଼ି)) {
    /* Skip the number */
    କର
      ଧାଡ଼ି++;
    ଯେତେବେଳେ(ଧାଡ଼ି < end && ISDIGIT(*ଧାଡ଼ି));

    /* Do we have the space character? */
    ଯଦି(ଧାଡ଼ି == end || *ଧାଡ଼ି != ' ')
      ଫେରନ୍ତୁ FALSE;

    ଧାଡ଼ି++;
  }

  /* Does the command name match and is it followed by a space character or at
     the end of line? */
  ଯଦି(ଧାଡ଼ି + cmd_len <= end && curl_strnequal(ଧାଡ଼ି, cmd, cmd_len) &&
     (ଧାଡ଼ି[cmd_len] == ' ' || ଧାଡ଼ି + cmd_len + 2 == end))
    ଫେରନ୍ତୁ TRUE;

  ଫେରନ୍ତୁ FALSE;
}

/***********************************************************************
 *
 * imap_endofresp()
 *
 * Checks whether the given string is a valid tagged, untagged or continuation
 * response which can be processed by the response handler.
 */
ସ୍ଥିର bool imap_endofresp(ସଂରଚନା Curl_easy *data, ସଂରଚନା connectdata *conn,
                           ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି, size_t len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *resp)
{
  ସଂରଚନା imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ସଂରଚନା IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  ଅଚଳ ଅକ୍ଷର *id;
  size_t id_len;

  DEBUGASSERT(imapc);
  DEBUGASSERT(imap);
  ଯଦି(!imapc || !imap)
    ଫେରନ୍ତୁ FALSE;

  /* Do we have a tagged command response? */
  id = imapc->resptag;
  id_len = strlen(id);
  ଯଦି(len >= id_len + 1 && !memcmp(id, ଧାଡ଼ି, id_len) && ଧାଡ଼ି[id_len] == ' ') {
    ଧାଡ଼ି += id_len + 1;
    len -= id_len + 1;

    ଯଦି(len >= 2 && !memcmp(ଧାଡ଼ି, "OK", 2))
      *resp = IMAP_RESP_OK;
    ଅନ୍ୟଥା ଯଦି(len >= 7 && !memcmp(ଧାଡ଼ି, "PREAUTH", 7))
      *resp = IMAP_RESP_PREAUTH;
    ଅନ୍ୟଥା
      *resp = IMAP_RESP_NOT_OK;

    ଫେରନ୍ତୁ TRUE;
  }

  /* Do we have an untagged command response? */
  ଯଦି(len >= 2 && !memcmp("* ", ଧାଡ଼ି, 2)) {
    ବାଛ(imapc->state) {
    /* States which are interested in untagged responses */
    ସ୍ଥିତି IMAP_CAPABILITY:
      ଯଦି(!imap_matchresp(ଧାଡ଼ି, len, "CAPABILITY"))
        ଫେରନ୍ତୁ FALSE;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି IMAP_LIST:
      ଯଦି((!imap->custom && !imap_matchresp(ଧାଡ଼ି, len, "LIST")) ||
         (imap->custom && !imap_matchresp(ଧାଡ଼ି, len, imap->custom) &&
          (!curl_strequal(imap->custom, "STORE") ||
           !imap_matchresp(ଧାଡ଼ି, len, "FETCH")) &&
          !curl_strequal(imap->custom, "SELECT") &&
          !curl_strequal(imap->custom, "EXAMINE") &&
          !curl_strequal(imap->custom, "SEARCH") &&
          !curl_strequal(imap->custom, "EXPUNGE") &&
          !curl_strequal(imap->custom, "LSUB") &&
          !curl_strequal(imap->custom, "UID") &&
          !curl_strequal(imap->custom, "GETQUOTAROOT") &&
          !curl_strequal(imap->custom, "NOOP")))
        ଫେରନ୍ତୁ FALSE;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି IMAP_SELECT:
      /* SELECT is special in that its untagged responses do not have a
         common prefix so accept anything! */
      ଭାଙ୍ଗ;

    ସ୍ଥିତି IMAP_FETCH:
      ଯଦି(!imap_matchresp(ଧାଡ଼ି, len, "FETCH"))
        ଫେରନ୍ତୁ FALSE;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି IMAP_ସନ୍ଧାନ:
      ଯଦି(!imap_matchresp(ଧାଡ଼ି, len, "SEARCH"))
        ଫେରନ୍ତୁ FALSE;
      ଭାଙ୍ଗ;

    /* Ignore other untagged responses */
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଫେରନ୍ତୁ FALSE;
    }

    *resp = '*';
    ଫେରନ୍ତୁ TRUE;
  }

  /* Do we have a continuation response? This should be a + symbol followed by
     a space and optionally some text as per RFC-3501 for the AUTHENTICATE and
     APPEND commands and as outlined in Section 4. Examples of RFC-4959 but
     some email servers ignore this and only send a single + instead. */
  ଯଦି(!imap->custom && ((len == 3 && ଧାଡ଼ି[0] == '+') ||
                       (len >= 2 && !memcmp("+ ", ଧାଡ଼ି, 2)))) {
    ବାଛ(imapc->state) {
    /* States which are interested in continuation responses */
    ସ୍ଥିତି IMAP_AUTHENTICATE:
    ସ୍ଥିତି IMAP_APPEND:
      *resp = '+';
      ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      failf(data, "Unexpected continuation response");
      *resp = -1;
      ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ TRUE;
  }

  ଫେରନ୍ତୁ FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * imap_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ସ୍ଥିର CURLcode imap_get_message(ସଂରଚନା Curl_easy *data, ସଂରଚନା bufref *out)
{
  ସଂରଚନା imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  ଅକ୍ଷର *message;
  size_t len;

  ଯଦି(!imapc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&imapc->pp.recvbuf);
  len = imapc->pp.nfinal;
  ଯଦି(len > 2) {
    /* Find the start of the message */
    len -= 2;
    ପାଇଁ(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    ଯେତେବେଳେ(len--)
      ଯଦି(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        ଭାଙ୍ଗ;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, ଖାଲି);
  }
  ଅନ୍ୟଥା
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, ଖାଲି);

  ଫେରନ୍ତୁ CURLE_OK;
}

/***********************************************************************
 *
 * imap_state()
 *
 * This is the ONLY way to change IMAP state!
 */
ସ୍ଥିର ଶୂନ୍ୟ imap_state(ସଂରଚନା Curl_easy *data,
                       ସଂରଚନା imap_conn *imapc,
                       imapstate newstate)
{
#ଯଦି defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ names[] = {
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

  ଯଦି(imapc->state != newstate)
    infof(data, "IMAP %p state change from %s to %s",
          (ଶୂନ୍ୟ *)imapc, names[imapc->state], names[newstate]);
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)data;
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
ସ୍ଥିର CURLcode imap_perform_capability(ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା imap_conn *imapc)
{
  CURLcode result = CURLE_OK;

  imapc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  imapc->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  imapc->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPABILITY command */
  result = imap_sendf(data, imapc, "CAPABILITY");

  ଯଦି(!result)
    imap_state(data, imapc, IMAP_CAPABILITY);

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * imap_perform_starttls()
 *
 * Sends the STARTTLS command to start the upgrade to TLS.
 */
ସ୍ଥିର CURLcode imap_perform_starttls(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା imap_conn *imapc)
{
  /* Send the STARTTLS command */
  CURLcode result = imap_sendf(data, imapc, "STARTTLS");

  ଯଦି(!result)
    imap_state(data, imapc, IMAP_STARTTLS);

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * imap_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ସ୍ଥିର CURLcode imap_perform_upgrade_tls(ସଂରଚନା Curl_easy *data,
                                         ସଂରଚନା imap_conn *imapc,
                                         ସଂରଚନା connectdata *conn)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  CURLcode result;
  bool ssldone = FALSE;

  ଯଦି(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ଯଦି(result)
      ଯାଅ out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_imaps;
  }

  DEBUGASSERT(!imapc->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "imap_perform_upgrade_tls, connect -> %d, %d",
               (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, ssldone));
  ଯଦି(!result && ssldone) {
    imapc->ssldone = ssldone;
    /* perform CAPA now, changes imapc->state out of IMAP_UPGRADETLS */
    result = imap_perform_capability(data, imapc);
  }
out:
  ଫେରନ୍ତୁ result;
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)imapc;
  (ଶୂନ୍ୟ)conn;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * imap_perform_login()
 *
 * Sends a clear text LOGIN command to authenticate with.
 */
ସ୍ଥିର CURLcode imap_perform_login(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା imap_conn *imapc,
                                   ସଂରଚନା connectdata *conn)
{
  CURLcode result = CURLE_OK;
  ଅକ୍ଷର *user;
  ଅକ୍ଷର *passwd;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  ଯଦି(!conn->creds) {
    imap_state(data, imapc, IMAP_STOP);

    ଫେରନ୍ତୁ result;
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

  ଯଦି(!result)
    imap_state(data, imapc, IMAP_LOGIN);

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * imap_perform_authenticate()
 *
 * Sends an AUTHENTICATE command allowing the client to login with the given
 * SASL authentication mechanism.
 */
ସ୍ଥିର CURLcode imap_perform_authenticate(ସଂରଚନା Curl_easy *data,
                                          ଅଚଳ ଅକ୍ଷର *mech,
                                          ଅଚଳ ସଂରଚନା bufref *initresp)
{
  ସଂରଚନା imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  CURLcode result = CURLE_OK;
  ଅଚଳ ଅକ୍ଷର *ir = Curl_bufref_ptr(initresp);

  ଯଦି(!imapc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଯଦି(ir) {
    /* Send the AUTHENTICATE command with the initial response */
    result = imap_sendf(data, imapc, "AUTHENTICATE %s %s",
                        mech, *ir ? ir : "=");
  }
  ଅନ୍ୟଥା {
    /* Send the AUTHENTICATE command */
    result = imap_sendf(data, imapc, "AUTHENTICATE %s", mech);
  }

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * imap_continue_authenticate()
 *
 * Sends SASL continuation data.
 */
ସ୍ଥିର CURLcode imap_continue_authenticate(ସଂରଚନା Curl_easy *data,
                                           ଅଚଳ ଅକ୍ଷର *mech,
                                           ଅଚଳ ସଂରଚନା bufref *resp)
{
  ସଂରଚନା imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  (ଶୂନ୍ୟ)mech;
  ଯଦି(!imapc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଫେରନ୍ତୁ Curl_pp_sendf(data, &imapc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * imap_cancel_authenticate()
 *
 * Sends SASL cancellation.
 */
ସ୍ଥିର CURLcode imap_cancel_authenticate(ସଂରଚନା Curl_easy *data,
                                         ଅଚଳ ଅକ୍ଷର *mech)
{
  ସଂରଚନା imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  (ଶୂନ୍ୟ)mech;
  ଯଦି(!imapc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଫେରନ୍ତୁ Curl_pp_sendf(data, &imapc->pp, "*");
}

/***********************************************************************
 *
 * imap_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to clear text should a common
 * mechanism not be available between the client and server.
 */
ସ୍ଥିର CURLcode imap_perform_authentication(ସଂରଚନା Curl_easy *data,
                                            ସଂରଚନା imap_conn *imapc)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  /* Check if already authenticated OR if there is enough data to authenticate
     with and end the connect phase if we do not */
  ଯଦି(imapc->preauth ||
     !Curl_sasl_can_authenticate(&imapc->sasl, data)) {
    imap_state(data, imapc, IMAP_STOP);
    ଫେରନ୍ତୁ result;
  }

  /* Calculate the SASL login details */
  result = Curl_sasl_start(&imapc->sasl, data, (bool)imapc->ir_supported,
                           &progress);
  ଯଦି(!result) {
    ଯଦି(progress == SASL_INPROGRESS)
      imap_state(data, imapc, IMAP_AUTHENTICATE);
    ଅନ୍ୟଥା ଯଦି(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
      /* Perform clear text authentication */
      result = imap_perform_login(data, imapc, data->conn);
    ଅନ୍ୟଥା
      result = Curl_sasl_is_blocked(&imapc->sasl, data);
  }

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * imap_perform_list()
 *
 * Sends a LIST command or an alternative custom request.
 */
ସ୍ଥିର CURLcode imap_perform_list(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା imap_conn *imapc,
                                  ସଂରଚନା IMAP *imap)
{
  CURLcode result = CURLE_OK;

  ଯଦି(imap->custom)
    /* Send the custom request */
    result = imap_sendf(data, imapc, "%s%s", imap->custom,
                        imap->custom_params ? imap->custom_params : "");
  ଅନ୍ୟଥା {
    /* Make sure the mailbox is in the correct atom format if necessary */
    ଅକ୍ଷର *mailbox = imap->mailbox ? imap_atom(imap->mailbox, TRUE)
                                  : curlx_strdup("");
    ଯଦି(!mailbox)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

    /* Send the LIST command */
    result = imap_sendf(data, imapc, "LIST \"%s\" *", mailbox);

    curlx_free(mailbox);
  }

  ଯଦି(!result)
    imap_state(data, imapc, IMAP_LIST);

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * imap_perform_select()
 *
 * Sends a SELECT command to ask the server to change the selected mailbox.
 */
ସ୍ଥିର CURLcode imap_perform_select(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା imap_conn *imapc,
                                    ସଂରଚନା IMAP *imap)
{
  CURLcode result = CURLE_OK;
  ଅକ୍ଷର *mailbox;

  /* Invalidate old information as we are switching mailboxes */
  curlx_safefree(imapc->mailbox);
  imapc->mb_uidvalidity_set = FALSE;

  /* Check we have a mailbox */
  ଯଦି(!imap->mailbox) {
    failf(data, "Cannot SELECT without a mailbox.");
    ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  ଯଦି(!mailbox)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  /* Send the SELECT command */
  result = imap_sendf(data, imapc, "SELECT %s", mailbox);

  curlx_free(mailbox);

  ଯଦି(!result)
    imap_state(data, imapc, IMAP_SELECT);

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * imap_perform_fetch()
 *
 * Sends a FETCH command to initiate the download of a message.
 */
ସ୍ଥିର CURLcode imap_perform_fetch(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା imap_conn *imapc,
                                   ସଂରଚନା IMAP *imap)
{
  CURLcode result = CURLE_OK;
  /* Check we have a UID */
  ଯଦି(imap->uid) {

    /* Send the FETCH command */
    ଯଦି(imap->partial)
      result = imap_sendf(data, imapc, "UID FETCH %s BODY[%s]<%s>",
                          imap->uid, imap->section ? imap->section : "",
                          imap->partial);
    ଅନ୍ୟଥା
      result = imap_sendf(data, imapc, "UID FETCH %s BODY[%s]",
                          imap->uid, imap->section ? imap->section : "");
  }
  ଅନ୍ୟଥା ଯଦି(imap->mindex) {
    /* Send the FETCH command */
    ଯଦି(imap->partial)
      result = imap_sendf(data, imapc, "FETCH %s BODY[%s]<%s>",
                          imap->mindex, imap->section ? imap->section : "",
                          imap->partial);
    ଅନ୍ୟଥା
      result = imap_sendf(data, imapc, "FETCH %s BODY[%s]",
                          imap->mindex, imap->section ? imap->section : "");
  }
  ଅନ୍ୟଥା {
    failf(data, "Cannot FETCH without a UID.");
    ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
  }
  ଯଦି(!result)
    imap_state(data, imapc, IMAP_FETCH);

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * imap_perform_append()
 *
 * Sends an APPEND command to initiate the upload of a message.
 */
ସ୍ଥିର CURLcode imap_perform_append(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା imap_conn *imapc,
                                    ସଂରଚନା IMAP *imap)
{
  CURLcode result = CURLE_OK;
  ଅକ୍ଷର *mailbox;
  ସଂରଚନା dynbuf flags;

  /* Check we have a mailbox */
  ଯଦି(!imap->mailbox) {
    failf(data, "Cannot APPEND without a mailbox.");
    ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
  }

#ifndef CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  ଯଦି(IS_MIME_POST(data)) {
    curl_mimepart *postp = data->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, data->set.headers, 0);
    result = Curl_mime_prepare_headers(data, postp, ଖାଲି,
                                       ଖାଲି, MIMESTRATEGY_MAIL);

    ଯଦି(!result && !Curl_checkheaders(data, STRCONST("Mime-Version")))
      result = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    ଯଦି(!result)
      result = Curl_creader_set_mime(data, postp);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଅନ୍ୟଥା
#endif
  {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  /* Check we know the size of the upload. This takes all readers
   * into account. Especially crlf conversions which make the size
   * unpredictable, e.g. -1. */
  data->state.infilesize = Curl_creader_total_length(data);
  ଯଦି(data->state.infilesize < 0) {
    ଯଦି(data->set.crlf)
      failf(data, "Cannot APPEND with CRLF conversion making size unknown");
    ଅନ୍ୟଥା
      failf(data, "Cannot APPEND with unknown input file size");
    ଫେରନ୍ତୁ CURLE_UPLOAD_FAILED;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  ଯଦି(!mailbox)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  /* Generate flags string and send the APPEND command */
  curlx_dyn_init(&flags, 100);
  ଯଦି(data->set.upload_flags) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ସଂରଚନା ulbits ulflag[] = {
      { CURLULFLAG_ANSWERED, "Answered" },
      { CURLULFLAG_DELETED, "Deleted" },
      { CURLULFLAG_DRAFT, "Draft" },
      { CURLULFLAG_FLAGGED, "Flagged" },
      { CURLULFLAG_SEEN, "Seen" },
      { 0, ଖାଲି }
    };

    result = CURLE_OUT_OF_MEMORY;
    ଯଦି(curlx_dyn_add(&flags, " (")) {
      ଯାଅ cleanup;
    }

    ପାଇଁ(i = 0; ulflag[i].bit; i++) {
      ଯଦି(data->set.upload_flags & ulflag[i].bit &&
         ((curlx_dyn_len(&flags) > 2 && curlx_dyn_add(&flags, " ")) ||
          curlx_dyn_add(&flags, "\\") ||
          curlx_dyn_add(&flags, ulflag[i].flag)))
        ଯାଅ cleanup;
    }

    ଯଦି(curlx_dyn_add(&flags, ")"))
      ଯାଅ cleanup;
  }
  ଅନ୍ୟଥା ଯଦି(curlx_dyn_add(&flags, ""))
    ଯାଅ cleanup;

  result = imap_sendf(data, imapc, "APPEND %s%s {%" FMT_OFF_T "}",
                      mailbox, curlx_dyn_ptr(&flags), data->state.infilesize);

cleanup:
  curlx_dyn_free(&flags);
  curlx_free(mailbox);

  ଯଦି(!result)
    imap_state(data, imapc, IMAP_APPEND);

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * imap_perform_search()
 *
 * Sends a SEARCH command.
 */
ସ୍ଥିର CURLcode imap_perform_ସନ୍ଧାନ(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା imap_conn *imapc,
                                    ସଂରଚନା IMAP *imap)
{
  CURLcode result = CURLE_OK;

  /* Check we have a query string */
  ଯଦି(!imap->query) {
    failf(data, "Cannot SEARCH without a query string.");
    ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
  }

  /* Send the SEARCH command */
  result = imap_sendf(data, imapc, "SEARCH %s", imap->query);

  ଯଦି(!result)
    imap_state(data, imapc, IMAP_ସନ୍ଧାନ);

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * imap_perform_logout()
 *
 * Performs the logout action prior to sclose() being called.
 */
ସ୍ଥିର CURLcode imap_perform_logout(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା imap_conn *imapc)
{
  /* Send the LOGOUT command */
  CURLcode result = imap_sendf(data, imapc, "LOGOUT");

  ଯଦି(!result)
    imap_state(data, imapc, IMAP_LOGOUT);

  ଫେରନ୍ତୁ result;
}

/* For the initial server greeting */
ସ୍ଥିର CURLcode imap_state_servergreet_resp(ସଂରଚନା Curl_easy *data,
                                            ସଂରଚନା imap_conn *imapc,
                                            ପୂର୍ଣ୍ଣସଂଖ୍ୟା imapcode,
                                            imapstate instate)
{
  (ଶୂନ୍ୟ)instate;

  ଯଦି(imapcode == IMAP_RESP_PREAUTH) {
    /* PREAUTH */
    imapc->preauth = TRUE;
    infof(data, "PREAUTH connection, already authenticated");
  }
  ଅନ୍ୟଥା ଯଦି(imapcode != IMAP_RESP_OK) {
    failf(data, "Got unexpected imap-server response");
    ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
  }

  ଫେରନ୍ତୁ imap_perform_capability(data, imapc);
}

/* For CAPABILITY responses */
ସ୍ଥିର CURLcode imap_state_capability_resp(ସଂରଚନା Curl_easy *data,
                                           ସଂରଚନା imap_conn *imapc,
                                           ପୂର୍ଣ୍ଣସଂଖ୍ୟା imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି = curlx_dyn_ptr(&imapc->pp.recvbuf);

  (ଶୂନ୍ୟ)instate;

  /* Do we have an untagged response? */
  ଯଦି(imapcode == '*') {
    ଧାଡ଼ି += 2;

    /* Loop through the data line */
    ପାଇଁ(;;) {
      size_t wordlen;
      ଯେତେବେଳେ(*ଧାଡ଼ି && (ISBLANK(*ଧାଡ଼ି) || ISNEWLINE(*ଧାଡ଼ି)))
        ଧାଡ଼ି++;

      ଯଦି(!*ଧାଡ଼ି)
        ଭାଙ୍ଗ;

      /* Extract the word */
      ପାଇଁ(wordlen = 0; ଧାଡ଼ି[wordlen] && !ISBLANK(ଧାଡ଼ି[wordlen]) &&
                       !ISNEWLINE(ଧାଡ଼ି[wordlen]);)
        wordlen++;

      /* Does the server support the STARTTLS capability? */
      ଯଦି(wordlen == 8 && curl_strnequal(ଧାଡ଼ି, "STARTTLS", 8))
        imapc->tls_supported = TRUE;

      /* Has the server explicitly disabled clear text authentication? */
      ଅନ୍ୟଥା ଯଦି(wordlen == 13 && curl_strnequal(ଧାଡ଼ି, "LOGINDISABLED", 13))
        imapc->login_disabled = TRUE;

      /* Does the server support the SASL-IR capability? */
      ଅନ୍ୟଥା ଯଦି(wordlen == 7 && curl_strnequal(ଧାଡ଼ି, "SASL-IR", 7))
        imapc->ir_supported = TRUE;

      /* Do we have a SASL based authentication mechanism? */
      ଅନ୍ୟଥା ଯଦି(wordlen > 5 && curl_strnequal(ଧାଡ଼ି, "AUTH=", 5)) {
        size_t llen;
        unsigned short mechbit;

        ଧାଡ଼ି += 5;
        wordlen -= 5;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(ଧାଡ଼ି, wordlen, &llen);
        ଯଦି(mechbit && llen == wordlen)
          imapc->sasl.authmechs |= mechbit;
      }

      ଧାଡ଼ି += wordlen;
    }
  }
  ଅନ୍ୟଥା ଯଦି(data->set.use_ssl && !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
    /* PREAUTH is not compatible with STARTTLS. */
    ଯଦି(imapcode == IMAP_RESP_OK && imapc->tls_supported && !imapc->preauth) {
      /* Switch to TLS connection now */
      result = imap_perform_starttls(data, imapc);
    }
    ଅନ୍ୟଥା ଯଦି(data->set.use_ssl <= CURLUSESSL_TRY)
      result = imap_perform_authentication(data, imapc);
    ଅନ୍ୟଥା {
      failf(data, "STARTTLS not available.");
      result = CURLE_USE_SSL_FAILED;
    }
  }
  ଅନ୍ୟଥା
    result = imap_perform_authentication(data, imapc);

  ଫେରନ୍ତୁ result;
}

/* For STARTTLS responses */
ସ୍ଥିର CURLcode imap_state_starttls_resp(ସଂରଚନା Curl_easy *data,
                                         ସଂରଚନା imap_conn *imapc,
                                         ପୂର୍ଣ୍ଣସଂଖ୍ୟା imapcode,
                                         imapstate instate)
{
  CURLcode result = CURLE_OK;

  (ଶୂନ୍ୟ)instate;

  /* Pipelining in response is forbidden. */
  ଯଦି(imapc->pp.overflow)
    ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;

  ଯଦି(imapcode != IMAP_RESP_OK) {
    ଯଦି(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied");
      result = CURLE_USE_SSL_FAILED;
    }
    ଅନ୍ୟଥା
      result = imap_perform_authentication(data, imapc);
  }
  ଅନ୍ୟଥା
    imap_state(data, imapc, IMAP_UPGRADETLS);

  ଫେରନ୍ତୁ result;
}

/* For SASL authentication responses */
ସ୍ଥିର CURLcode imap_state_auth_resp(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା imap_conn *imapc,
                                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା imapcode,
                                     imapstate instate)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  (ଶୂନ୍ୟ)instate;

  result = Curl_sasl_continue(&imapc->sasl, data, imapcode, &progress);
  ଯଦି(!result)
    ବାଛ(progress) {
    ସ୍ଥିତି SASL_DONE:
      imap_state(data, imapc, IMAP_STOP);  /* Authenticated */
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SASL_IDLE:            /* No mechanism left after cancellation */
      ଯଦି(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
        /* Perform clear text authentication */
        result = imap_perform_login(data, imapc, data->conn);
      ଅନ୍ୟଥା {
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
      }
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଭାଙ୍ଗ;
    }

  ଫେରନ୍ତୁ result;
}

/* For LOGIN responses */
ସ୍ଥିର CURLcode imap_state_login_resp(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା imap_conn *imapc,
                                      ପୂର୍ଣ୍ଣସଂଖ୍ୟା imapcode,
                                      imapstate instate)
{
  CURLcode result = CURLE_OK;
  (ଶୂନ୍ୟ)instate;

  ଯଦି(imapcode != IMAP_RESP_OK) {
    failf(data, "Access denied. %c", imapcode);
    result = CURLE_LOGIN_DENIED;
  }
  ଅନ୍ୟଥା
    /* End of connect phase */
    imap_state(data, imapc, IMAP_STOP);

  ଫେରନ୍ତୁ result;
}

/* Detect IMAP listings vs. downloading a single email */
ସ୍ଥିର bool is_custom_fetch_listing_match(ଅଚଳ ଅକ୍ଷର *params)
{
  /* match " 1:* (FLAGS ..." or " 1,2,3 (FLAGS ..." */
  ଯଦି(*params++ != ' ')
    ଫେରନ୍ତୁ FALSE;

  ଯେତେବେଳେ(ISDIGIT(*params)) {
    params++;
    ଯଦି(*params == 0)
      ଫେରନ୍ତୁ FALSE;
  }
  ଯଦି(*params == ':')
    ଫେରନ୍ତୁ TRUE;
  ଯଦି(*params == ',')
    ଫେରନ୍ତୁ TRUE;
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର bool is_custom_fetch_listing(ସଂରଚନା IMAP *imap)
{
  /* filter out "UID FETCH 1:* (FLAGS ..." queries to list emails */
  ଯଦି(!imap->custom)
    ଫେରନ୍ତୁ FALSE;
  ଅନ୍ୟଥା ଯଦି(curl_strequal(imap->custom, "FETCH") && imap->custom_params) {
    ଅଚଳ ଅକ୍ଷର *p = imap->custom_params;
    ଫେରନ୍ତୁ is_custom_fetch_listing_match(p);
  }
  ଅନ୍ୟଥା ଯଦି(curl_strequal(imap->custom, "UID") && imap->custom_params &&
          curl_strnequal(imap->custom_params, " FETCH ", 7)) {
    ଅଚଳ ଅକ୍ଷର *p = imap->custom_params + 6;
    ଫେରନ୍ତୁ is_custom_fetch_listing_match(p);
  }
  ଫେରନ୍ତୁ FALSE;
}

/* For LIST and SEARCH responses */
ସ୍ଥିର CURLcode imap_state_listsearch_resp(ସଂରଚନା Curl_easy *data,
                                           ସଂରଚନା imap_conn *imapc,
                                           ପୂର୍ଣ୍ଣସଂଖ୍ୟା imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  ସଂରଚନା IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);

  DEBUGASSERT(imap);
  ଯଦି(!imap)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  (ଶୂନ୍ୟ)instate;

  ଯଦି(imapcode == '*' && is_custom_fetch_listing(imap)) {
    /* custom FETCH or UID FETCH for listing is not handled here */
  }
  ଅନ୍ୟଥା ଯଦି(imapcode == '*') {
    /* Check if this response contains a literal (e.g. FETCH responses with
       body data). Literal syntax is {size}\r\n */
    ଅଚଳ ଅକ୍ଷର *cr = memchr(ଧାଡ଼ି, '\r', len);
    size_t ଧାଡ଼ି_len = cr ? (size_t)(cr - ଧାଡ଼ି) : len;
    ଅଚଳ ଅକ୍ଷର *ptr = imap_find_literal(ଧାଡ଼ି, ଧାଡ଼ି_len);
    ଯଦି(ptr) {
      curl_off_t size = 0;
      bool parsed = FALSE;
      ptr++;
      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&ptr, &size, CURL_OFF_T_MAX) &&
         !curlx_str_single(&ptr, '}'))
        parsed = TRUE;

      ଯଦି(parsed) {
        ସଂରଚନା pingpong *pp = &imapc->pp;
        size_t buffer_len = curlx_dyn_len(&pp->recvbuf);
        size_t after_header = buffer_len - pp->nfinal;

        /* This is a literal response, setup to receive the body data */
        infof(data, "Found %" FMT_OFF_T " bytes to download", size);

        /* First write the header line */
        result = Curl_client_write(data, CLIENTWRITE_BODY, ଧାଡ଼ି, len);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;

        /* Handle data already in buffer after the header line */
        ଯଦି(after_header > 0) {
          /* There is already data in the buffer that is part of the literal
             body or subsequent responses */
          size_t chunk = after_header;

          /* Keep only the data after the header line */
          curlx_dyn_tail(&pp->recvbuf, chunk);
          pp->nfinal = 0; /* done */

          /* Limit chunk to the literal size */
          ଯଦି((curl_off_t)chunk > size)
            chunk = (size_t)size;

          ଯଦି(chunk) {
            /* Write the literal body data */
            result = Curl_client_write(data, CLIENTWRITE_BODY,
                                       curlx_dyn_ptr(&pp->recvbuf), chunk);
            ଯଦି(result)
              ଫେରନ୍ତୁ result;
          }

          /* Handle remaining data in buffer (either more literal data or
             subsequent responses) */
          ଯଦି(after_header > chunk) {
            /* Keep the data after the literal body */
            pp->overflow = after_header - chunk;
            curlx_dyn_tail(&pp->recvbuf, pp->overflow);
          }
          ଅନ୍ୟଥା {
            pp->overflow = 0;
            curlx_dyn_reset(&pp->recvbuf);
          }
        }
        ଅନ୍ୟଥା {
          /* No data in buffer yet, reset overflow */
          pp->overflow = 0;
        }

        ଯଦି((CURL_OFF_T_MAX - size) < (curl_off_t)len)
          /* unlikely to actually be a transfer this big, but avoid integer
             overflow */
          size = CURL_OFF_T_MAX;
        ଅନ୍ୟଥା
          size += len;

        /* Progress size includes both header line and literal body */
        Curl_pgrsSetDownloadSize(data, size);

        ଯଦି(data->req.bytecount == size)
          /* All data already transferred (header + literal body) */
          Curl_xfer_setup_nop(data);
        ଅନ୍ୟଥା {
          /* Setup to receive the literal body data.
             maxdownload and transfer size include both header line and
             literal body */
          data->req.maxdownload = size;
          Curl_xfer_setup_recv(data, FIRSTSOCKET, size);
        }
        /* End of DO phase */
        imap_state(data, imapc, IMAP_STOP);
      }
      ଅନ୍ୟଥା {
        /* Failed to parse literal, write the line */
        result = Curl_client_write(data, CLIENTWRITE_BODY, ଧାଡ଼ି, len);
      }
    }
    ଅନ୍ୟଥା {
      /* No literal, write the line as-is */
      result = Curl_client_write(data, CLIENTWRITE_BODY, ଧାଡ଼ି, len);
    }
  }
  ଅନ୍ୟଥା ଯଦି(imapcode != IMAP_RESP_OK)
    result = CURLE_QUOTE_ERROR;
  ଅନ୍ୟଥା
    /* End of DO phase */
    imap_state(data, imapc, IMAP_STOP);

  ଫେରନ୍ତୁ result;
}

/* For SELECT responses */
ସ୍ଥିର CURLcode imap_state_select_resp(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା imap_conn *imapc,
                                       ସଂରଚନା IMAP *imap,
                                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  (ଶୂନ୍ୟ)instate;

  ଯଦି(imapcode == '*') {
    /* See if this is an UIDVALIDITY response */
    ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି = curlx_dyn_ptr(&imapc->pp.recvbuf);
    size_t len = curlx_dyn_len(&imapc->pp.recvbuf);
    ଯଦି((len >= 18) && checkprefix("OK [UIDVALIDITY ", &ଧାଡ଼ି[2])) {
      curl_off_t ମୂଲ୍ୟ;
      ଅଚଳ ଅକ୍ଷର *p = &ଧାଡ଼ି[2] + CURL_CSTRLEN("OK [UIDVALIDITY ");
      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &ମୂଲ୍ୟ, UINT_MAX)) {
        imapc->mb_uidvalidity = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ମୂଲ୍ୟ;
        imapc->mb_uidvalidity_set = TRUE;
      }
    }
  }
  ଅନ୍ୟଥା ଯଦି(imapcode == IMAP_RESP_OK) {
    /* Check if the UIDVALIDITY has been specified and matches */
    ଯଦି(imap->uidvalidity_set && imapc->mb_uidvalidity_set &&
       (imap->uidvalidity != imapc->mb_uidvalidity)) {
      failf(data, "Mailbox UIDVALIDITY has changed");
      result = CURLE_REMOTE_FILE_NOT_FOUND;
    }
    ଅନ୍ୟଥା {
      /* Note the currently opened mailbox on this connection */
      DEBUGASSERT(!imapc->mailbox);
      imapc->mailbox = curlx_strdup(imap->mailbox);
      ଯଦି(!imapc->mailbox)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

      ଯଦି(imap->custom)
        result = imap_perform_list(data, imapc, imap);
      ଅନ୍ୟଥା ଯଦି(imap->query)
        result = imap_perform_ସନ୍ଧାନ(data, imapc, imap);
      ଅନ୍ୟଥା
        result = imap_perform_fetch(data, imapc, imap);
    }
  }
  ଅନ୍ୟଥା {
    failf(data, "Select failed");
    result = CURLE_LOGIN_DENIED;
  }

  ଫେରନ୍ତୁ result;
}

/* For the (first line of the) FETCH responses */
ସ୍ଥିର CURLcode imap_state_fetch_resp(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା imap_conn *imapc,
                                      ପୂର୍ଣ୍ଣସଂଖ୍ୟା imapcode,
                                      imapstate instate)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା pingpong *pp = &imapc->pp;
  ଅଚଳ ଅକ୍ଷର *ptr = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  bool parsed = FALSE;
  curl_off_t size = 0;

  (ଶୂନ୍ୟ)instate;

  ଯଦି(imapcode != '*') {
    Curl_pgrsSetDownloadSize(data, -1);
    imap_state(data, imapc, IMAP_STOP);
    ଫେରନ୍ତୁ CURLE_REMOTE_FILE_NOT_FOUND;
  }

  /* Something like this is received "* 1 FETCH (BODY[TEXT] {2021}\r" so parse
     the continuation data contained within the curly brackets */
  ptr = imap_find_literal(ptr, len);
  ଯଦି(ptr) {
    ptr++;
    ଯଦି(!curlx_str_ସଂଖ୍ୟା(&ptr, &size, CURL_OFF_T_MAX) &&
       !curlx_str_single(&ptr, '}'))
      parsed = TRUE;
  }

  ଯଦି(parsed) {
    infof(data, "Found %" FMT_OFF_T " bytes to download", size);
    Curl_pgrsSetDownloadSize(data, size);

    ଯଦି(pp->overflow) {
      /* At this point there is a data in the receive buffer that is body
         content, send it as body and then skip it. Do note that there may
         even be additional "headers" after the body. */
      size_t chunk = pp->overflow;

      /* keep only the overflow */
      curlx_dyn_tail(&pp->recvbuf, chunk);
      pp->nfinal = 0; /* done */

      ଯଦି((curl_off_t)chunk > size)
        /* The conversion from curl_off_t to size_t is always fine here */
        chunk = (size_t)size;

      ଯଦି(!chunk) {
        /* no size, we are done with the data */
        imap_state(data, imapc, IMAP_STOP);
        ଫେରନ୍ତୁ CURLE_OK;
      }
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 curlx_dyn_ptr(&pp->recvbuf), chunk);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;

      infof(data, "Written %zu bytes, %" FMT_OFF_T
            " bytes are left for transfer", chunk, (curl_off_t)(size - chunk));

      /* Have we used the entire overflow or part of it?*/
      ଯଦି(pp->overflow > chunk) {
        /* remember the remaining trailing overflow data */
        pp->overflow -= chunk;
        curlx_dyn_tail(&pp->recvbuf, pp->overflow);
      }
      ଅନ୍ୟଥା {
        pp->overflow = 0; /* handled */
        /* Free the cache */
        curlx_dyn_reset(&pp->recvbuf);
      }
    }

    ଯଦି(data->req.bytecount == size)
      /* The entire data is already transferred! */
      Curl_xfer_setup_nop(data);
    ଅନ୍ୟଥା {
      /* IMAP download */
      data->req.maxdownload = size;
      Curl_xfer_setup_recv(data, FIRSTSOCKET, size);
    }
  }
  ଅନ୍ୟଥା {
    /* We do not know how to parse this line */
    failf(data, "Failed to parse FETCH response.");
    result = CURLE_WEIRD_SERVER_REPLY;
  }

  /* End of DO phase */
  imap_state(data, imapc, IMAP_STOP);

  ଫେରନ୍ତୁ result;
}

/* For final FETCH responses performed after the download */
ସ୍ଥିର CURLcode imap_state_fetch_final_resp(ସଂରଚନା Curl_easy *data,
                                            ସଂରଚନା imap_conn *imapc,
                                            ପୂର୍ଣ୍ଣସଂଖ୍ୟା imapcode,
                                            imapstate instate)
{
  CURLcode result = CURLE_OK;

  (ଶୂନ୍ୟ)instate;

  ଯଦି(imapcode != IMAP_RESP_OK)
    result = CURLE_WEIRD_SERVER_REPLY;
  ଅନ୍ୟଥା
    /* End of DONE phase */
    imap_state(data, imapc, IMAP_STOP);

  ଫେରନ୍ତୁ result;
}

/* For APPEND responses */
ସ୍ଥିର CURLcode imap_state_append_resp(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା imap_conn *imapc,
                                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  (ଶୂନ୍ୟ)instate;

  ଯଦି(imapcode != '+') {
    result = CURLE_UPLOAD_FAILED;
  }
  ଅନ୍ୟଥା {
    /* Set the progress upload size */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* IMAP upload */
    Curl_xfer_setup_send(data, FIRSTSOCKET);

    /* End of DO phase */
    imap_state(data, imapc, IMAP_STOP);
  }

  ଫେରନ୍ତୁ result;
}

/* For final APPEND responses performed after the upload */
ସ୍ଥିର CURLcode imap_state_append_final_resp(ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା imap_conn *imapc,
                                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା imapcode,
                                             imapstate instate)
{
  CURLcode result = CURLE_OK;

  (ଶୂନ୍ୟ)instate;

  ଯଦି(imapcode != IMAP_RESP_OK)
    result = CURLE_UPLOAD_FAILED;
  ଅନ୍ୟଥା
    /* End of DONE phase */
    imap_state(data, imapc, IMAP_STOP);

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode imap_pp_statemachine(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା connectdata *conn)
{
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା imapcode;
  ସଂରଚନା imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ସଂରଚନା IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  ସଂରଚନା pingpong *pp;
  size_t nread = 0;

  ଯଦି(!imapc || !imap)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  pp = &imapc->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not IMAP */
upgrade_tls:
  ଯଦି(imapc->state == IMAP_UPGRADETLS) {
    result = imap_perform_upgrade_tls(data, imapc, conn);
    ଯଦି(result || (imapc->state == IMAP_UPGRADETLS))
      ଫେରନ୍ତୁ result;
  }

  /* Flush any data that needs to be sent */
  ଯଦି(pp->sendleft)
    ଫେରନ୍ତୁ Curl_pp_flushsend(data, pp);

  କର {
    /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, pp, &imapcode, &nread);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;

    /* Was there an error parsing the response line? */
    ଯଦି(imapcode == -1)
      ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;

    ଯଦି(!imapcode)
      ଭାଙ୍ଗ;

    /* We have now received a full IMAP server response */
    ବାଛ(imapc->state) {
    ସ୍ଥିତି IMAP_SERVERGREET:
      result = imap_state_servergreet_resp(data, imapc,
                                           imapcode, imapc->state);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି IMAP_CAPABILITY:
      result = imap_state_capability_resp(data, imapc, imapcode, imapc->state);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି IMAP_STARTTLS:
      result = imap_state_starttls_resp(data, imapc, imapcode, imapc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      ଯଦି(!result && (imapc->state == IMAP_UPGRADETLS))
        ଯାଅ upgrade_tls;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି IMAP_AUTHENTICATE:
      result = imap_state_auth_resp(data, imapc, imapcode, imapc->state);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି IMAP_LOGIN:
      result = imap_state_login_resp(data, imapc, imapcode, imapc->state);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି IMAP_LIST:
    ସ୍ଥିତି IMAP_ସନ୍ଧାନ:
      result = imap_state_listsearch_resp(data, imapc, imapcode, imapc->state);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି IMAP_SELECT:
      result = imap_state_select_resp(data, imapc, imap,
                                      imapcode, imapc->state);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି IMAP_FETCH:
      result = imap_state_fetch_resp(data, imapc, imapcode, imapc->state);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି IMAP_FETCH_FINAL:
      result = imap_state_fetch_final_resp(data, imapc,
                                           imapcode, imapc->state);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି IMAP_APPEND:
      result = imap_state_append_resp(data, imapc, imapcode, imapc->state);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି IMAP_APPEND_FINAL:
      result = imap_state_append_final_resp(data, imapc,
                                            imapcode, imapc->state);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି IMAP_LOGOUT:
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      /* internal error */
      imap_state(data, imapc, IMAP_STOP);
      ଭାଙ୍ଗ;
    }
  } ଯେତେବେଳେ(!result && imapc->state != IMAP_STOP && Curl_pp_moredata(pp));

  ଫେରନ୍ତୁ result;
}

/* Called repeatedly until done from multi.c */
ସ୍ଥିର CURLcode imap_multi_statemach(ସଂରଚନା Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  *done = FALSE;
  ଯଦି(!imapc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  result = Curl_pp_statemach(data, &imapc->pp, FALSE, FALSE);
  *done = (imapc->state == IMAP_STOP);

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode imap_block_statemach(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା imap_conn *imapc,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;

  ଯେତେବେଳେ(imapc->state != IMAP_STOP && !result)
    result = Curl_pp_statemach(data, &imapc->pp, TRUE, disconnecting);

  ଫେରନ୍ତୁ result;
}

/* For the IMAP "protocol connect" and "doing" phases only */
ସ୍ଥିର CURLcode imap_pollset(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା easy_pollset *ps)
{
  ସଂରଚନା imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  ଫେରନ୍ତୁ imapc ? Curl_pp_pollset(data, &imapc->pp, ps) : CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ imap_easy_reset(ସଂରଚନା IMAP *imap)
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
ସ୍ଥିର bool imap_is_bchar(ଅକ୍ଷର ch)
{
  /* Performing the alnum check first with macro is faster because of ASCII
     arithmetic */
  ଫେରନ୍ତୁ ch && (ISALNUM(ch) || strchr(":@/&=-._~!$\'()*+,%", ch));
}

/***********************************************************************
 *
 * imap_parse_url_options()
 *
 * Parse the URL login options.
 */
ସ୍ଥିର CURLcode imap_parse_url_options(ସଂରଚନା connectdata *conn,
                                       ସଂରଚନା imap_conn *imapc)
{
  CURLcode result = CURLE_OK;
  ଅଚଳ ଅକ୍ଷର *ptr = conn->options;
  bool prefer_login = FALSE;

  ଯେତେବେଳେ(!result && ptr && *ptr) {
    ଅଚଳ ଅକ୍ଷର *ଚାବି = ptr;
    ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ;

    ଯେତେବେଳେ(*ptr && *ptr != '=')
      ptr++;

    ମୂଲ୍ୟ = ptr + 1;

    ଯେତେବେଳେ(*ptr && *ptr != ';')
      ptr++;

    ଯଦି(curl_strnequal(ଚାବି, "AUTH=+LOGIN", 11)) {
      /* User prefers plaintext LOGIN over any SASL, including SASL LOGIN */
      prefer_login = TRUE;
      imapc->sasl.prefmech = SASL_AUTH_NONE;
    }
    ଅନ୍ୟଥା ଯଦି(curl_strnequal(ଚାବି, "AUTH=", 5)) {
      prefer_login = FALSE;
      result = Curl_sasl_parse_url_auth_option(&imapc->sasl,
                                               ମୂଲ୍ୟ, ptr - ମୂଲ୍ୟ);
    }
    ଅନ୍ୟଥା {
      prefer_login = FALSE;
      result = CURLE_URL_MALFORMAT;
    }

    ଯଦି(*ptr == ';')
      ptr++;
  }

  ଯଦି(prefer_login)
    imapc->preftype = IMAP_TYPE_CLEARTEXT;
  ଅନ୍ୟଥା {
    ବାଛ(imapc->sasl.prefmech) {
    ସ୍ଥିତି SASL_AUTH_NONE:
      imapc->preftype = IMAP_TYPE_NONE;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SASL_AUTH_DEFAULT:
      imapc->preftype = IMAP_TYPE_ANY;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      imapc->preftype = IMAP_TYPE_SASL;
      ଭାଙ୍ଗ;
    }
  }

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * imap_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
ସ୍ଥିର CURLcode imap_parse_url_path(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା IMAP *imap)
{
  /* The imap struct is already initialized in imap_connect() */
  CURLcode result = CURLE_OK;
  ଅଚଳ ଅକ୍ଷର *begin = &data->state.up.path[1]; /* skip leading slash */
  ଅଚଳ ଅକ୍ଷର *ptr = begin;

  /* See how much of the URL is a valid path and decode it */
  ଯେତେବେଳେ(imap_is_bchar(*ptr))
    ptr++;

  ଯଦି(ptr != begin) {
    /* Remove the trailing slash if present */
    ଅଚଳ ଅକ୍ଷର *end = ptr;
    ଯଦି(end > begin && end[-1] == '/')
      end--;

    result = Curl_urldecode(begin, end - begin, &imap->mailbox, ଖାଲି,
                            REJECT_CTRL);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଅନ୍ୟଥା
    imap->mailbox = ଖାଲି;

  /* There can be any number of parameters in the form ";NAME=VALUE" */
  ଯେତେବେଳେ(*ptr == ';') {
    ଅକ୍ଷର *name;
    ଅକ୍ଷର *ମୂଲ୍ୟ;
    size_t valuelen;

    /* Find the length of the name parameter */
    begin = ++ptr;
    ଯେତେବେଳେ(*ptr && *ptr != '=')
      ptr++;

    ଯଦି(!*ptr)
      ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;

    /* Decode the name parameter */
    result = Curl_urldecode(begin, ptr - begin, &name, ଖାଲି,
                            REJECT_CTRL);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;

    /* Find the length of the value parameter */
    begin = ++ptr;
    ଯେତେବେଳେ(imap_is_bchar(*ptr))
      ptr++;

    /* Decode the value parameter */
    result = Curl_urldecode(begin, ptr - begin, &ମୂଲ୍ୟ, &valuelen,
                            REJECT_CTRL);
    ଯଦି(result) {
      curlx_free(name);
      ଫେରନ୍ତୁ result;
    }

    DEBUGF(infof(data, "IMAP URL parameter '%s' = '%s'", name, ମୂଲ୍ୟ));

    /* Process the known hierarchical parameters (UIDVALIDITY, UID, SECTION
       and PARTIAL) stripping of the trailing slash character if it is
       present.

       Note: Unknown parameters trigger a URL_MALFORMAT error. */
    ଯଦି(valuelen > 0 && ମୂଲ୍ୟ[valuelen - 1] == '/')
      ମୂଲ୍ୟ[valuelen - 1] = '\0';
    ଯଦି(valuelen) {
      ଯଦି(curl_strequal(name, "UIDVALIDITY") && !imap->uidvalidity_set) {
        curl_off_t ଅଙ୍କ;
        ଅଚଳ ଅକ୍ଷର *p = (ଅଚଳ ଅକ୍ଷର *)ମୂଲ୍ୟ;
        ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &ଅଙ୍କ, UINT_MAX)) {
          imap->uidvalidity = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଅଙ୍କ;
          imap->uidvalidity_set = TRUE;
        }
        curlx_free(ମୂଲ୍ୟ);
      }
      ଅନ୍ୟଥା ଯଦି(curl_strequal(name, "UID") && !imap->uid) {
        imap->uid = ମୂଲ୍ୟ;
      }
      ଅନ୍ୟଥା ଯଦି(curl_strequal(name, "MAILINDEX") && !imap->mindex) {
        imap->mindex = ମୂଲ୍ୟ;
      }
      ଅନ୍ୟଥା ଯଦି(curl_strequal(name, "SECTION") && !imap->section) {
        imap->section = ମୂଲ୍ୟ;
      }
      ଅନ୍ୟଥା ଯଦି(curl_strequal(name, "PARTIAL") && !imap->partial) {
        imap->partial = ମୂଲ୍ୟ;
      }
      ଅନ୍ୟଥା {
        curlx_free(name);
        curlx_free(ମୂଲ୍ୟ);
        ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
      }
    }
    ଅନ୍ୟଥା
      /* blank? */
      curlx_free(ମୂଲ୍ୟ);
    curlx_free(name);
  }

  /* Does the URL contain a query parameter? Only valid when we have a mailbox
     and no UID as per RFC-5092 */
  ଯଦି(imap->mailbox && !imap->uid && !imap->mindex) {
    /* Get the query parameter, URL decoded */
    CURLUcode uc = curl_url_get(data->state.uh, CURLUPART_QUERY, &imap->query,
                                CURLU_URLDECODE);
    ଯଦି(uc == CURLUE_OUT_OF_MEMORY)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  /* Any extra stuff at the end of the URL is an error */
  ଯଦି(*ptr)
    ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;

  ଫେରନ୍ତୁ CURLE_OK;
}

/***********************************************************************
 *
 * imap_parse_custom_request()
 *
 * Parse the custom request.
 */
ସ୍ଥିର CURLcode imap_parse_custom_request(ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା IMAP *imap)
{
  CURLcode result = CURLE_OK;
  ଅଚଳ ଅକ୍ଷର *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  ଯଦି(custom) {
    /* URL decode the custom request */
    result = Curl_urldecode(custom, 0, &imap->custom, ଖାଲି, REJECT_CTRL);

    /* Extract the parameters if specified */
    ଯଦି(!result) {
      ଅଚଳ ଅକ୍ଷର *params = imap->custom;

      ଯେତେବେଳେ(*params && *params != ' ')
        params++;

      ଯଦି(*params) {
        imap->custom_params = curlx_strdup(params);
        imap->custom[params - imap->custom] = '\0';

        ଯଦି(!imap->custom_params)
          result = CURLE_OUT_OF_MEMORY;
      }
    }
  }

  ଫେରନ୍ତୁ result;
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
ସ୍ଥିର CURLcode imap_connect(ସଂରଚନା Curl_easy *data, bool *done)
{
  ସଂରଚନା imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  CURLcode result = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  ଯଦି(!imapc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  /* Parse the URL options */
  result = imap_parse_url_options(data->conn, imapc);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* Start off waiting for the server greeting response */
  imap_state(data, imapc, IMAP_SERVERGREET);

  /* Start off with an response id of '*' */
  curlx_strcopy(imapc->resptag, ଆକାର(imapc->resptag), STRCONST("*"));

  result = imap_multi_statemach(data, done);

  ଫେରନ୍ତୁ result;
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
ସ୍ଥିର CURLcode imap_done(ସଂରଚନା Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ସଂରଚନା IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);

  ଯଦି(!imapc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଯଦି(!imap)
    ଫେରନ୍ତୁ CURLE_OK;

  ଯଦି(status || premature) {
    CURL_TRC_M(data, "IMAP done, not finalizing transfer");
    connclose(conn); /* marked for closure */
    result = status;         /* use the already set error code */
  }
  ଅନ୍ୟଥା ଯଦି(!data->set.connect_only &&
          ((!imap->custom && (imap->uid || imap->mindex)) ||
           (imap->custom && data->req.maxdownload > 0) ||
           data->state.upload || IS_MIME_POST(data))) {
    /* Handle responses after FETCH or APPEND transfer has finished.
       For custom commands, check if we set up a download which indicates
       a FETCH-like command with literal data. */

    ଯଦି(!data->state.upload && !IS_MIME_POST(data))
      imap_state(data, imapc, IMAP_FETCH_FINAL);
    ଅନ୍ୟଥା {
      /* End the APPEND command first by sending an empty line */
      result = Curl_pp_sendf(data, &imapc->pp, "%s", "");
      ଯଦି(!result)
        imap_state(data, imapc, IMAP_APPEND_FINAL);
    }

    /* Run the state-machine */
    ଯଦି(!result)
      result = imap_block_statemach(data, imapc, FALSE);
  }

  imap_easy_reset(imap);
  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * imap_perform()
 *
 * This is the actual DO function for IMAP. Fetch or append a message, or do
 * other things according to the options previously setup.
 */
ସ୍ଥିର CURLcode imap_perform(ସଂରଚନା Curl_easy *data, bool *connected,
                             bool *dophase_done)
{
  /* This is IMAP and no proxy */
  CURLcode result = CURLE_OK;
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ସଂରଚନା IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  bool selected = FALSE;

  DEBUGF(infof(data, "DO phase starts"));
  ଯଦି(!imapc || !imap)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯଦି(data->req.no_body) {
    /* Requested no body means no transfer */
    imap->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* Determine if the requested mailbox (with the same UIDVALIDITY if set)
     has already been selected on this connection */
  ଯଦି(imap->mailbox && imapc->mailbox &&
     (!strcmp(imap->mailbox, imapc->mailbox) ||
      (curl_strequal(imap->mailbox, "INBOX") &&
       curl_strequal(imapc->mailbox, "INBOX"))) &&
     (!imap->uidvalidity_set || !imapc->mb_uidvalidity_set ||
      (imap->uidvalidity == imapc->mb_uidvalidity)))
    selected = TRUE;

  /* Start the first command in the DO phase */
  ଯଦି(data->state.upload || IS_MIME_POST(data))
    /* APPEND can be executed directly */
    result = imap_perform_append(data, imapc, imap);
  ଅନ୍ୟଥା ଯଦି(imap->custom && (selected || !imap->mailbox))
    /* Custom command using the same mailbox or no mailbox */
    result = imap_perform_list(data, imapc, imap);
  ଅନ୍ୟଥା ଯଦି(!imap->custom && selected && (imap->uid || imap->mindex))
    /* FETCH from the same mailbox */
    result = imap_perform_fetch(data, imapc, imap);
  ଅନ୍ୟଥା ଯଦି(!imap->custom && selected && imap->query)
    /* SEARCH the current mailbox */
    result = imap_perform_ସନ୍ଧାନ(data, imapc, imap);
  ଅନ୍ୟଥା ଯଦି(imap->mailbox && !selected &&
          (imap->custom || imap->uid || imap->mindex || imap->query))
    /* SELECT the mailbox */
    result = imap_perform_select(data, imapc, imap);
  ଅନ୍ୟଥା
    /* LIST */
    result = imap_perform_list(data, imapc, imap);

  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* Run the state-machine */
  result = imap_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(conn, FIRSTSOCKET);

  ଯଦି(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  ଫେରନ୍ତୁ result;
}

/* Call this when the DO phase has completed */
ସ୍ଥିର CURLcode imap_dophase_done(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା IMAP *imap,
                                  bool connected)
{
  (ଶୂନ୍ୟ)connected;

  ଯଦି(imap->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);

  ଫେରନ୍ତୁ CURLE_OK;
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
ସ୍ଥିର CURLcode imap_regular_transfer(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା IMAP *imap,
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
  ଯଦି(!result && *dophase_done)
    result = imap_dophase_done(data, imap, connected);

  ଫେରନ୍ତୁ result;
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
ସ୍ଥିର CURLcode imap_do(ସଂରଚନା Curl_easy *data, bool *done)
{
  ସଂରଚନା IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* default to false */

  ଯଦି(!imap)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  /* Parse the URL path */
  result = imap_parse_url_path(data, imap);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* Parse the custom request */
  result = imap_parse_custom_request(data, imap);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  result = imap_regular_transfer(data, imap, done);

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * imap_disconnect()
 *
 * Disconnect from an IMAP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ସ୍ଥିର CURLcode imap_disconnect(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା connectdata *conn, bool dead_connection)
{
  ସଂରଚନା imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);

  ଯଦି(imapc &&
     /* We cannot send quit unconditionally. If this connection is stale or
        bad in any way (pingpong has pending data to send),
        sending quit and waiting around here will make the
        disconnect wait in vain and cause more problems than we need to. */
     !dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &imapc->pp) &&
     !imap_perform_logout(data, imapc))
    (ଶୂନ୍ୟ)imap_block_statemach(data, imapc, TRUE); /* ignore errors */

  ଫେରନ୍ତୁ CURLE_OK;
}

/* Called from multi.c while DOing */
ସ୍ଥିର CURLcode imap_doing(ସଂରଚନା Curl_easy *data, bool *dophase_done)
{
  ସଂରଚନା IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  CURLcode result;

  ଯଦି(!imap)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  result = imap_multi_statemach(data, dophase_done);
  ଯଦି(result)
    DEBUGF(infof(data, "DO phase failed"));
  ଅନ୍ୟଥା ଯଦି(*dophase_done) {
    result = imap_dophase_done(data, imap, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ imap_easy_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା IMAP *imap = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  imap_easy_reset(imap);
  curlx_free(imap);
}

ସ୍ଥିର ଶୂନ୍ୟ imap_conn_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା imap_conn *imapc = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  Curl_pp_disconnect(&imapc->pp);
  curlx_safefree(imapc->mailbox);
  curlx_free(imapc);
}

/* SASL parameters for the imap protocol */
ସ୍ଥିର ଅଚଳ ସଂରଚନା SASLproto saslimap = {
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

ସ୍ଥିର CURLcode imap_setup_connection(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା connectdata *conn)
{
  ସଂରଚନା imap_conn *imapc;
  ସଂରଚନା pingpong *pp;
  ସଂରଚନା IMAP *imap;

  imapc = curlx_calloc(1, ଆକାର(*imapc));
  ଯଦି(!imapc)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  pp = &imapc->pp;
  PINGPONG_SETUP(pp, imap_pp_statemachine, imap_endofresp);

  /* Set the default preferred authentication type and mechanism */
  imapc->preftype = IMAP_TYPE_ANY;
  Curl_sasl_init(&imapc->sasl, data, &saslimap);

  Curl_pp_init(pp, Curl_pgrs_now(data));

  ଯଦି(Curl_conn_meta_set(conn, CURL_META_IMAP_CONN, imapc, imap_conn_dtor))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  imap = curlx_calloc(1, ଆକାର(ସଂରଚନା IMAP));
  ଯଦି(!imap ||
     Curl_meta_set(data, CURL_META_IMAP_EASY, imap, imap_easy_dtor))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * IMAP protocol.
 */
ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_imap = {
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
