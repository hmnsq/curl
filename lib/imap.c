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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "imap.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_IMAP

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#ਅਨ੍ਤਰ੍ਭਾਵ <in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <inet.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pingpong.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mime.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sasl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strcopy.h"

/* meta key for storing protocol meta at easy handle */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_IMAP_EASY   "meta:proto:imap:easy"
/* meta key for storing protocol meta at connection */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_IMAP_CONN   "meta:proto:imap:conn"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
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
  IMAP_ਖੋਜ,
  IMAP_LOGOUT,
  IMAP_ਆਖਰੀ          /* never used */
} imapstate;

/* imap_conn is used for struct connection-oriented data */
ਬਣਤਰ imap_conn {
  ਬਣਤਰ pingpong pp;
  ਬਣਤਰ SASL sasl;           /* SASL-related parameters */
  ਅੱਖਰ *mailbox;              /* The last selected mailbox */
  imapstate state;            /* Always use imap.c:state() to change state! */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ mb_uidvalidity; /* UIDVALIDITY parsed from select response */
  ਅੱਖਰ resptag[5];            /* Response tag to wait for */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ preftype;     /* Preferred authentication type */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ cmdid;        /* Last used command ID */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ssldone);               /* Is connect() over SSL done? */
  ਦੋਆਧਾਰੀ_ਅੰਕ(preauth);               /* Is this connection PREAUTH? */
  ਦੋਆਧਾਰੀ_ਅੰਕ(tls_supported);         /* StartTLS capability supported by server */
  ਦੋਆਧਾਰੀ_ਅੰਕ(login_disabled);        /* LOGIN command disabled by server */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ir_supported);          /* Initial response supported by server */
  ਦੋਆਧਾਰੀ_ਅੰਕ(mb_uidvalidity_ਸੈੱਟ);
};

/* This IMAP struct is used in the Curl_easy. All IMAP data that is
   connection-oriented must be in imap_conn to properly deal with the fact that
   perhaps the Curl_easy is changed between the times the connection is
   used. */
ਬਣਤਰ IMAP {
  curl_pp_transfer transfer;
  ਅੱਖਰ *mailbox;          /* Mailbox to select */
  ਅੱਖਰ *uid;              /* Message UID to fetch */
  ਅੱਖਰ *mindex;           /* Index in mail box of mail to fetch */
  ਅੱਖਰ *section;          /* Message SECTION to fetch */
  ਅੱਖਰ *partial;          /* Message PARTIAL to fetch */
  ਅੱਖਰ *query;            /* Query to search for */
  ਅੱਖਰ *custom;           /* Custom request */
  ਅੱਖਰ *custom_params;    /* Parameters for the custom request */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ uidvalidity; /* UIDVALIDITY to check in select */
  ਦੋਆਧਾਰੀ_ਅੰਕ(uidvalidity_ਸੈੱਟ);
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMAP_RESP_OK       1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMAP_RESP_NOT_OK   2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMAP_RESP_PREAUTH  3

ਬਣਤਰ ulbits {
  ਪੂਰਨ ਦੋਆਧਾਰੀ_ਅੰਕ_2;
  ਅਚਲ ਅੱਖਰ *flag;
};

/***********************************************************************
 *
 * imap_sendf()
 *
 * Sends the formatted string as an IMAP command to the server.
 *
 * Designed to never block.
 */
ਸਥਿਰ CURLcode imap_sendf(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ imap_conn *imapc,
                           ਅਚਲ ਅੱਖਰ *fmt, ...) CURL_PRINTF(3, 4);
ਸਥਿਰ CURLcode imap_sendf(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ imap_conn *imapc,
                           ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  va_list ap;

  DEBUGASSERT(fmt);

  /* Calculate the tag based on the connection ID and command ID */
  curl_msnprintf(imapc->resptag, ਆਕਾਰ(imapc->resptag), "%c%03d",
                 'A' + curlx_sltosi((ਲੰਮਾ)(ਅੰਕੜੇ->conn->connection_id % 26)),
                 ++imapc->cmdid);

  va_start(ap, fmt);
  ਨਤੀਜਾ = Curl_pp_vsendf(ਅੰਕੜੇ, &imapc->pp, imapc->resptag, fmt, ap);
  va_end(ap);
  ਵਾਪਸ ਨਤੀਜਾ;
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
ਸਥਿਰ ਅੱਖਰ *imap_atom(ਅਚਲ ਅੱਖਰ *str, bool escape_only)
{
  ਬਣਤਰ dynbuf ਸਤਰ;
  size_t nclean;
  size_t len;

  ਜੇ(!str)
    ਵਾਪਸ ਨਿਰਾ;

  len = ਲੜੀ_ਲੰਬਾਈ(str);
  nclean = strcspn(str, "() {%*]\\\"");
  ਜੇ(len == nclean)
    /* nothing to escape, return a strdup */
    ਵਾਪਸ curlx_strdup(str);

  curlx_dyn_init(&ਸਤਰ, 2000);

  ਜੇ(!escape_only && curlx_dyn_addn(&ਸਤਰ, "\"", 1))
    ਵਾਪਸ ਨਿਰਾ;

  ਜਦੋਂ(*str) {
    ਜੇ((*str == '\\' || *str == '"') &&
       curlx_dyn_addn(&ਸਤਰ, "\\", 1))
      ਵਾਪਸ ਨਿਰਾ;
    ਜੇ(curlx_dyn_addn(&ਸਤਰ, str, 1))
      ਵਾਪਸ ਨਿਰਾ;
    str++;
  }

  ਜੇ(!escape_only && curlx_dyn_addn(&ਸਤਰ, "\"", 1))
    ਵਾਪਸ ਨਿਰਾ;

  ਵਾਪਸ curlx_dyn_ptr(&ਸਤਰ);
}

/*
 * Finds the start of a literal '{size}' in line, skipping over quoted strings.
 */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *imap_ਲੱਭੋ_literal(ਅਚਲ ਅੱਖਰ *ਸਤਰ, size_t len)
{
  ਅਚਲ ਅੱਖਰ *ਸਿਰਾ = ਸਤਰ + len;
  bool ਵਿੱਚ_quote = FALSE;

  ਜਦੋਂ(ਸਤਰ < ਸਿਰਾ) {
    ਜੇ(ਵਿੱਚ_quote) {
      ਜੇ(*ਸਤਰ == '\\' && (ਸਤਰ + 1) < ਸਿਰਾ) {
        ਸਤਰ += 2;
        ਜਾਰੀ;
      }
      ਜੇ(*ਸਤਰ == '"')
        ਵਿੱਚ_quote = FALSE;
    }
    ਨਹੀਂਤਾਂ {
      ਜੇ(*ਸਤਰ == '"')
        ਵਿੱਚ_quote = TRUE;
      ਨਹੀਂਤਾਂ ਜੇ(*ਸਤਰ == '{')
        ਵਾਪਸ ਸਤਰ;
    }
    ਸਤਰ++;
  }
  ਵਾਪਸ ਨਿਰਾ;
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
ਸਥਿਰ bool imap_matchresp(ਅਚਲ ਅੱਖਰ *ਸਤਰ, size_t len, ਅਚਲ ਅੱਖਰ *cmd)
{
  ਅਚਲ ਅੱਖਰ *ਸਿਰਾ = ਸਤਰ + len;
  size_t cmd_len = ਲੜੀ_ਲੰਬਾਈ(cmd);

  /* Skip the untagged response marker */
  ਸਤਰ += 2;

  /* Do we have a number after the marker? */
  ਜੇ(ਸਤਰ < ਸਿਰਾ && ISDIGIT(*ਸਤਰ)) {
    /* Skip the number */
    ਕਰੋ
      ਸਤਰ++;
    ਜਦੋਂ(ਸਤਰ < ਸਿਰਾ && ISDIGIT(*ਸਤਰ));

    /* Do we have the space character? */
    ਜੇ(ਸਤਰ == ਸਿਰਾ || *ਸਤਰ != ' ')
      ਵਾਪਸ FALSE;

    ਸਤਰ++;
  }

  /* Does the command name match and is it followed by a space character or at
     the end of line? */
  ਜੇ(ਸਤਰ + cmd_len <= ਸਿਰਾ && curl_strnequal(ਸਤਰ, cmd, cmd_len) &&
     (ਸਤਰ[cmd_len] == ' ' || ਸਤਰ + cmd_len + 2 == ਸਿਰਾ))
    ਵਾਪਸ TRUE;

  ਵਾਪਸ FALSE;
}

/***********************************************************************
 *
 * imap_endofresp()
 *
 * Checks whether the given string is a valid tagged, untagged or continuation
 * response which can be processed by the response handler.
 */
ਸਥਿਰ bool imap_endofresp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ connectdata *conn,
                           ਅਚਲ ਅੱਖਰ *ਸਤਰ, size_t len, ਪੂਰਨ *resp)
{
  ਬਣਤਰ imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ਬਣਤਰ IMAP *imap = Curl_meta_get(ਅੰਕੜੇ, CURL_META_IMAP_EASY);
  ਅਚਲ ਅੱਖਰ *id;
  size_t id_len;

  DEBUGASSERT(imapc);
  DEBUGASSERT(imap);
  ਜੇ(!imapc || !imap)
    ਵਾਪਸ FALSE;

  /* Do we have a tagged command response? */
  id = imapc->resptag;
  id_len = ਲੜੀ_ਲੰਬਾਈ(id);
  ਜੇ(len >= id_len + 1 && !memcmp(id, ਸਤਰ, id_len) && ਸਤਰ[id_len] == ' ') {
    ਸਤਰ += id_len + 1;
    len -= id_len + 1;

    ਜੇ(len >= 2 && !memcmp(ਸਤਰ, "OK", 2))
      *resp = IMAP_RESP_OK;
    ਨਹੀਂਤਾਂ ਜੇ(len >= 7 && !memcmp(ਸਤਰ, "PREAUTH", 7))
      *resp = IMAP_RESP_PREAUTH;
    ਨਹੀਂਤਾਂ
      *resp = IMAP_RESP_NOT_OK;

    ਵਾਪਸ TRUE;
  }

  /* Do we have an untagged command response? */
  ਜੇ(len >= 2 && !memcmp("* ", ਸਤਰ, 2)) {
    ਚੋਣ(imapc->state) {
    /* States which are interested in untagged responses */
    ਹਾਲਤ IMAP_CAPABILITY:
      ਜੇ(!imap_matchresp(ਸਤਰ, len, "CAPABILITY"))
        ਵਾਪਸ FALSE;
      ਰੋਕੋ;

    ਹਾਲਤ IMAP_LIST:
      ਜੇ((!imap->custom && !imap_matchresp(ਸਤਰ, len, "LIST")) ||
         (imap->custom && !imap_matchresp(ਸਤਰ, len, imap->custom) &&
          (!curl_strequal(imap->custom, "STORE") ||
           !imap_matchresp(ਸਤਰ, len, "FETCH")) &&
          !curl_strequal(imap->custom, "SELECT") &&
          !curl_strequal(imap->custom, "EXAMINE") &&
          !curl_strequal(imap->custom, "SEARCH") &&
          !curl_strequal(imap->custom, "EXPUNGE") &&
          !curl_strequal(imap->custom, "LSUB") &&
          !curl_strequal(imap->custom, "UID") &&
          !curl_strequal(imap->custom, "GETQUOTAROOT") &&
          !curl_strequal(imap->custom, "NOOP")))
        ਵਾਪਸ FALSE;
      ਰੋਕੋ;

    ਹਾਲਤ IMAP_SELECT:
      /* SELECT is special in that its untagged responses do not have a
         common prefix so accept anything! */
      ਰੋਕੋ;

    ਹਾਲਤ IMAP_FETCH:
      ਜੇ(!imap_matchresp(ਸਤਰ, len, "FETCH"))
        ਵਾਪਸ FALSE;
      ਰੋਕੋ;

    ਹਾਲਤ IMAP_ਖੋਜ:
      ਜੇ(!imap_matchresp(ਸਤਰ, len, "SEARCH"))
        ਵਾਪਸ FALSE;
      ਰੋਕੋ;

    /* Ignore other untagged responses */
    ਮੂਲ:
      ਵਾਪਸ FALSE;
    }

    *resp = '*';
    ਵਾਪਸ TRUE;
  }

  /* Do we have a continuation response? This should be a + symbol followed by
     a space and optionally some text as per RFC-3501 for the AUTHENTICATE and
     APPEND commands and as outlined in Section 4. Examples of RFC-4959 but
     some email servers ignore this and only send a single + instead. */
  ਜੇ(!imap->custom && ((len == 3 && ਸਤਰ[0] == '+') ||
                       (len >= 2 && !memcmp("+ ", ਸਤਰ, 2)))) {
    ਚੋਣ(imapc->state) {
    /* States which are interested in continuation responses */
    ਹਾਲਤ IMAP_AUTHENTICATE:
    ਹਾਲਤ IMAP_APPEND:
      *resp = '+';
      ਰੋਕੋ;

    ਮੂਲ:
      failf(ਅੰਕੜੇ, "Unexpected continuation response");
      *resp = -1;
      ਰੋਕੋ;
    }

    ਵਾਪਸ TRUE;
  }

  ਵਾਪਸ FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * imap_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ਸਥਿਰ CURLcode imap_get_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ bufref *ਬਾਹਰਲਾ)
{
  ਬਣਤਰ imap_conn *imapc =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_IMAP_CONN);
  ਅੱਖਰ *ਸੁਨੇਹਾ_2;
  size_t len;

  ਜੇ(!imapc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਸੁਨੇਹਾ_2 = curlx_dyn_ptr(&imapc->pp.recvbuf);
  len = imapc->pp.nfinal;
  ਜੇ(len > 2) {
    /* Find the start of the message */
    len -= 2;
    ਲਈ(ਸੁਨੇਹਾ_2 += 2; ISBLANK(*ਸੁਨੇਹਾ_2); ਸੁਨੇਹਾ_2++, len--)
      ;

    /* Find the end of the message */
    ਜਦੋਂ(len--)
      ਜੇ(!ISNEWLINE(ਸੁਨੇਹਾ_2[len]) && !ISBLANK(ਸੁਨੇਹਾ_2[len]))
        ਰੋਕੋ;

    /* Terminate the message */
    ਸੁਨੇਹਾ_2[++len] = '\0';
    Curl_bufref_ਸੈੱਟ(ਬਾਹਰਲਾ, ਸੁਨੇਹਾ_2, len, ਨਿਰਾ);
  }
  ਨਹੀਂਤਾਂ
    /* junk input => zero length output */
    Curl_bufref_ਸੈੱਟ(ਬਾਹਰਲਾ, "", 0, ਨਿਰਾ);

  ਵਾਪਸ CURLE_OK;
}

/***********************************************************************
 *
 * imap_state()
 *
 * This is the ONLY way to change IMAP state!
 */
ਸਥਿਰ ਖਾਲੀ imap_state(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                       ਬਣਤਰ imap_conn *imapc,
                       imapstate newstate)
{
#ਜੇ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ ਨਾਂ[] = {
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
    /*ਆਖਰੀ*/
  };

  ਜੇ(imapc->state != newstate)
    infof(ਅੰਕੜੇ, "IMAP %p state change from %s to %s",
          (ਖਾਲੀ *)imapc, ਨਾਂ[imapc->state], ਨਾਂ[newstate]);
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
#ਜੇ_ਅੰਤ
  imapc->state = newstate;
}

/***********************************************************************
 *
 * imap_perform_capability()
 *
 * Sends the CAPABILITY command in order to obtain a list of server side
 * supported capabilities.
 */
ਸਥਿਰ CURLcode imap_perform_capability(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ imap_conn *imapc)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  imapc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  imapc->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  imapc->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPABILITY command */
  ਨਤੀਜਾ = imap_sendf(ਅੰਕੜੇ, imapc, "CAPABILITY");

  ਜੇ(!ਨਤੀਜਾ)
    imap_state(ਅੰਕੜੇ, imapc, IMAP_CAPABILITY);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * imap_perform_starttls()
 *
 * Sends the STARTTLS command to start the upgrade to TLS.
 */
ਸਥਿਰ CURLcode imap_perform_starttls(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ imap_conn *imapc)
{
  /* Send the STARTTLS command */
  CURLcode ਨਤੀਜਾ = imap_sendf(ਅੰਕੜੇ, imapc, "STARTTLS");

  ਜੇ(!ਨਤੀਜਾ)
    imap_state(ਅੰਕੜੇ, imapc, IMAP_STARTTLS);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * imap_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ਸਥਿਰ CURLcode imap_perform_upgrade_tls(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ imap_conn *imapc,
                                         ਬਣਤਰ connectdata *conn)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  /* Start the SSL connection */
  CURLcode ਨਤੀਜਾ;
  bool ssldone = FALSE;

  ਜੇ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    ਨਤੀਜਾ = Curl_ssl_cfilter_ਜੋੜੋ(
      ਅੰਕੜੇ, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_imaps;
  }

  DEBUGASSERT(!imapc->ssldone);
  ਨਤੀਜਾ = Curl_conn_connect(ਅੰਕੜੇ, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(ਅੰਕੜੇ, "imap_perform_upgrade_tls, connect -> %d, %d",
               (ਪੂਰਨ)ਨਤੀਜਾ, ssldone));
  ਜੇ(!ਨਤੀਜਾ && ssldone) {
    imapc->ssldone = ssldone;
    /* perform CAPA now, changes imapc->state out of IMAP_UPGRADETLS */
    ਨਤੀਜਾ = imap_perform_capability(ਅੰਕੜੇ, imapc);
  }
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)imapc;
  (ਖਾਲੀ)conn;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
}

/***********************************************************************
 *
 * imap_perform_login()
 *
 * Sends a clear text LOGIN command to authenticate with.
 */
ਸਥਿਰ CURLcode imap_perform_login(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ imap_conn *imapc,
                                   ਬਣਤਰ connectdata *conn)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅੱਖਰ *user;
  ਅੱਖਰ *passwd;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  ਜੇ(!conn->creds) {
    imap_state(ਅੰਕੜੇ, imapc, IMAP_STOP);

    ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* Make sure the username and password are in the correct atom format */
  user = imap_atom(Curl_creds_user(conn->creds), FALSE);
  passwd = imap_atom(Curl_creds_passwd(conn->creds), FALSE);

  /* Send the LOGIN command */
  ਨਤੀਜਾ = imap_sendf(ਅੰਕੜੇ, imapc, "LOGIN %s %s", user ? user : "",
                      passwd ? passwd : "");

  curlx_free(user);
  curlx_strzero(passwd);
  curlx_free(passwd);

  ਜੇ(!ਨਤੀਜਾ)
    imap_state(ਅੰਕੜੇ, imapc, IMAP_LOGIN);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * imap_perform_authenticate()
 *
 * Sends an AUTHENTICATE command allowing the client to login with the given
 * SASL authentication mechanism.
 */
ਸਥਿਰ CURLcode imap_perform_authenticate(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਅਚਲ ਅੱਖਰ *mech,
                                          ਅਚਲ ਬਣਤਰ bufref *initresp)
{
  ਬਣਤਰ imap_conn *imapc =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_IMAP_CONN);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ *ir = Curl_bufref_ptr(initresp);

  ਜੇ(!imapc)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਜੇ(ir) {
    /* Send the AUTHENTICATE command with the initial response */
    ਨਤੀਜਾ = imap_sendf(ਅੰਕੜੇ, imapc, "AUTHENTICATE %s %s",
                        mech, *ir ? ir : "=");
  }
  ਨਹੀਂਤਾਂ {
    /* Send the AUTHENTICATE command */
    ਨਤੀਜਾ = imap_sendf(ਅੰਕੜੇ, imapc, "AUTHENTICATE %s", mech);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * imap_continue_authenticate()
 *
 * Sends SASL continuation data.
 */
ਸਥਿਰ CURLcode imap_continue_authenticate(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਅਚਲ ਅੱਖਰ *mech,
                                           ਅਚਲ ਬਣਤਰ bufref *resp)
{
  ਬਣਤਰ imap_conn *imapc =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_IMAP_CONN);

  (ਖਾਲੀ)mech;
  ਜੇ(!imapc)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਵਾਪਸ Curl_pp_sendf(ਅੰਕੜੇ, &imapc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * imap_cancel_authenticate()
 *
 * Sends SASL cancellation.
 */
ਸਥਿਰ CURLcode imap_cancel_authenticate(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਅਚਲ ਅੱਖਰ *mech)
{
  ਬਣਤਰ imap_conn *imapc =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_IMAP_CONN);

  (ਖਾਲੀ)mech;
  ਜੇ(!imapc)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਵਾਪਸ Curl_pp_sendf(ਅੰਕੜੇ, &imapc->pp, "*");
}

/***********************************************************************
 *
 * imap_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to clear text should a common
 * mechanism not be available between the client and server.
 */
ਸਥਿਰ CURLcode imap_perform_authentication(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                            ਬਣਤਰ imap_conn *imapc)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  saslprogress ਤਰੱਕੀ;

  /* Check if already authenticated OR if there is enough data to authenticate
     with and end the connect phase if we do not */
  ਜੇ(imapc->preauth ||
     !Curl_sasl_can_authenticate(&imapc->sasl, ਅੰਕੜੇ)) {
    imap_state(ਅੰਕੜੇ, imapc, IMAP_STOP);
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* Calculate the SASL login details */
  ਨਤੀਜਾ = Curl_sasl_ਸ਼ੁਰੂ(&imapc->sasl, ਅੰਕੜੇ, (bool)imapc->ir_supported,
                           &ਤਰੱਕੀ);
  ਜੇ(!ਨਤੀਜਾ) {
    ਜੇ(ਤਰੱਕੀ == SASL_INPROGRESS)
      imap_state(ਅੰਕੜੇ, imapc, IMAP_AUTHENTICATE);
    ਨਹੀਂਤਾਂ ਜੇ(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
      /* Perform clear text authentication */
      ਨਤੀਜਾ = imap_perform_login(ਅੰਕੜੇ, imapc, ਅੰਕੜੇ->conn);
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = Curl_sasl_is_blocked(&imapc->sasl, ਅੰਕੜੇ);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * imap_perform_list()
 *
 * Sends a LIST command or an alternative custom request.
 */
ਸਥਿਰ CURLcode imap_perform_list(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ imap_conn *imapc,
                                  ਬਣਤਰ IMAP *imap)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(imap->custom)
    /* Send the custom request */
    ਨਤੀਜਾ = imap_sendf(ਅੰਕੜੇ, imapc, "%s%s", imap->custom,
                        imap->custom_params ? imap->custom_params : "");
  ਨਹੀਂਤਾਂ {
    /* Make sure the mailbox is in the correct atom format if necessary */
    ਅੱਖਰ *mailbox = imap->mailbox ? imap_atom(imap->mailbox, TRUE)
                                  : curlx_strdup("");
    ਜੇ(!mailbox)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    /* Send the LIST command */
    ਨਤੀਜਾ = imap_sendf(ਅੰਕੜੇ, imapc, "LIST \"%s\" *", mailbox);

    curlx_free(mailbox);
  }

  ਜੇ(!ਨਤੀਜਾ)
    imap_state(ਅੰਕੜੇ, imapc, IMAP_LIST);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * imap_perform_select()
 *
 * Sends a SELECT command to ask the server to change the selected mailbox.
 */
ਸਥਿਰ CURLcode imap_perform_select(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ imap_conn *imapc,
                                    ਬਣਤਰ IMAP *imap)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅੱਖਰ *mailbox;

  /* Invalidate old information as we are switching mailboxes */
  curlx_safefree(imapc->mailbox);
  imapc->mb_uidvalidity_ਸੈੱਟ = FALSE;

  /* Check we have a mailbox */
  ਜੇ(!imap->mailbox) {
    failf(ਅੰਕੜੇ, "Cannot SELECT without a mailbox.");
    ਵਾਪਸ CURLE_URL_MALFORMAT;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  ਜੇ(!mailbox)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* Send the SELECT command */
  ਨਤੀਜਾ = imap_sendf(ਅੰਕੜੇ, imapc, "SELECT %s", mailbox);

  curlx_free(mailbox);

  ਜੇ(!ਨਤੀਜਾ)
    imap_state(ਅੰਕੜੇ, imapc, IMAP_SELECT);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * imap_perform_fetch()
 *
 * Sends a FETCH command to initiate the download of a message.
 */
ਸਥਿਰ CURLcode imap_perform_fetch(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ imap_conn *imapc,
                                   ਬਣਤਰ IMAP *imap)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  /* Check we have a UID */
  ਜੇ(imap->uid) {

    /* Send the FETCH command */
    ਜੇ(imap->partial)
      ਨਤੀਜਾ = imap_sendf(ਅੰਕੜੇ, imapc, "UID FETCH %s BODY[%s]<%s>",
                          imap->uid, imap->section ? imap->section : "",
                          imap->partial);
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = imap_sendf(ਅੰਕੜੇ, imapc, "UID FETCH %s BODY[%s]",
                          imap->uid, imap->section ? imap->section : "");
  }
  ਨਹੀਂਤਾਂ ਜੇ(imap->mindex) {
    /* Send the FETCH command */
    ਜੇ(imap->partial)
      ਨਤੀਜਾ = imap_sendf(ਅੰਕੜੇ, imapc, "FETCH %s BODY[%s]<%s>",
                          imap->mindex, imap->section ? imap->section : "",
                          imap->partial);
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = imap_sendf(ਅੰਕੜੇ, imapc, "FETCH %s BODY[%s]",
                          imap->mindex, imap->section ? imap->section : "");
  }
  ਨਹੀਂਤਾਂ {
    failf(ਅੰਕੜੇ, "Cannot FETCH without a UID.");
    ਵਾਪਸ CURLE_URL_MALFORMAT;
  }
  ਜੇ(!ਨਤੀਜਾ)
    imap_state(ਅੰਕੜੇ, imapc, IMAP_FETCH);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * imap_perform_append()
 *
 * Sends an APPEND command to initiate the upload of a message.
 */
ਸਥਿਰ CURLcode imap_perform_append(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ imap_conn *imapc,
                                    ਬਣਤਰ IMAP *imap)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅੱਖਰ *mailbox;
  ਬਣਤਰ dynbuf flags;

  /* Check we have a mailbox */
  ਜੇ(!imap->mailbox) {
    failf(ਅੰਕੜੇ, "Cannot APPEND without a mailbox.");
    ਵਾਪਸ CURLE_URL_MALFORMAT;
  }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  ਜੇ(IS_MIME_POST(ਅੰਕੜੇ)) {
    curl_mimepart *postp = ਅੰਕੜੇ->ਸੈੱਟ.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_ਸਿਰਲੇਖਾਂ(postp, ਅੰਕੜੇ->ਸੈੱਟ.ਸਿਰਲੇਖਾਂ, 0);
    ਨਤੀਜਾ = Curl_mime_prepare_ਸਿਰਲੇਖਾਂ(ਅੰਕੜੇ, postp, ਨਿਰਾ,
                                       ਨਿਰਾ, MIMESTRATEGY_MAIL);

    ਜੇ(!ਨਤੀਜਾ && !Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Mime-Version")))
      ਨਤੀਜਾ = Curl_mime_ਜੋੜੋ_ਸਿਰਲੇਖ(&postp->curlheaders, "Mime-Version: 1.0");
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_mime(ਅੰਕੜੇ, postp);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
  {
    ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_fread(ਅੰਕੜੇ, ਅੰਕੜੇ->state.infilesize);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* Check we know the size of the upload. This takes all readers
   * into account. Especially crlf conversions which make the size
   * unpredictable, e.g. -1. */
  ਅੰਕੜੇ->state.infilesize = Curl_creader_ਕੁੱਲ_ਲੰਮਾਈ(ਅੰਕੜੇ);
  ਜੇ(ਅੰਕੜੇ->state.infilesize < 0) {
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.crlf)
      failf(ਅੰਕੜੇ, "Cannot APPEND with CRLF conversion making size unknown");
    ਨਹੀਂਤਾਂ
      failf(ਅੰਕੜੇ, "Cannot APPEND with unknown input file size");
    ਵਾਪਸ CURLE_UPLOAD_FAILED;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  ਜੇ(!mailbox)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* Generate flags string and send the APPEND command */
  curlx_dyn_init(&flags, 100);
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.upload_flags) {
    ਪੂਰਨ i;
    ਬਣਤਰ ulbits ulflag[] = {
      { CURLULFLAG_ANSWERED, "Answered" },
      { CURLULFLAG_DELETED, "Deleted" },
      { CURLULFLAG_DRAFT, "Draft" },
      { CURLULFLAG_FLAGGED, "Flagged" },
      { CURLULFLAG_ਵੇਖਿਆ, "Seen" },
      { 0, ਨਿਰਾ }
    };

    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜੇ(curlx_dyn_ਜੋੜੋ(&flags, " (")) {
      ਜਾਓ cleanup;
    }

    ਲਈ(i = 0; ulflag[i].ਦੋਆਧਾਰੀ_ਅੰਕ_2; i++) {
      ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.upload_flags & ulflag[i].ਦੋਆਧਾਰੀ_ਅੰਕ_2 &&
         ((curlx_dyn_len(&flags) > 2 && curlx_dyn_ਜੋੜੋ(&flags, " ")) ||
          curlx_dyn_ਜੋੜੋ(&flags, "\\") ||
          curlx_dyn_ਜੋੜੋ(&flags, ulflag[i].flag)))
        ਜਾਓ cleanup;
    }

    ਜੇ(curlx_dyn_ਜੋੜੋ(&flags, ")"))
      ਜਾਓ cleanup;
  }
  ਨਹੀਂਤਾਂ ਜੇ(curlx_dyn_ਜੋੜੋ(&flags, ""))
    ਜਾਓ cleanup;

  ਨਤੀਜਾ = imap_sendf(ਅੰਕੜੇ, imapc, "APPEND %s%s {%" FMT_OFF_T "}",
                      mailbox, curlx_dyn_ptr(&flags), ਅੰਕੜੇ->state.infilesize);

cleanup:
  curlx_dyn_free(&flags);
  curlx_free(mailbox);

  ਜੇ(!ਨਤੀਜਾ)
    imap_state(ਅੰਕੜੇ, imapc, IMAP_APPEND);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * imap_perform_search()
 *
 * Sends a SEARCH command.
 */
ਸਥਿਰ CURLcode imap_perform_ਖੋਜ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ imap_conn *imapc,
                                    ਬਣਤਰ IMAP *imap)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  /* Check we have a query string */
  ਜੇ(!imap->query) {
    failf(ਅੰਕੜੇ, "Cannot SEARCH without a query string.");
    ਵਾਪਸ CURLE_URL_MALFORMAT;
  }

  /* Send the SEARCH command */
  ਨਤੀਜਾ = imap_sendf(ਅੰਕੜੇ, imapc, "SEARCH %s", imap->query);

  ਜੇ(!ਨਤੀਜਾ)
    imap_state(ਅੰਕੜੇ, imapc, IMAP_ਖੋਜ);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * imap_perform_logout()
 *
 * Performs the logout action prior to sclose() being called.
 */
ਸਥਿਰ CURLcode imap_perform_logout(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ imap_conn *imapc)
{
  /* Send the LOGOUT command */
  CURLcode ਨਤੀਜਾ = imap_sendf(ਅੰਕੜੇ, imapc, "LOGOUT");

  ਜੇ(!ਨਤੀਜਾ)
    imap_state(ਅੰਕੜੇ, imapc, IMAP_LOGOUT);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For the initial server greeting */
ਸਥਿਰ CURLcode imap_state_servergreet_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                            ਬਣਤਰ imap_conn *imapc,
                                            ਪੂਰਨ imapcode,
                                            imapstate instate)
{
  (ਖਾਲੀ)instate;

  ਜੇ(imapcode == IMAP_RESP_PREAUTH) {
    /* PREAUTH */
    imapc->preauth = TRUE;
    infof(ਅੰਕੜੇ, "PREAUTH connection, already authenticated");
  }
  ਨਹੀਂਤਾਂ ਜੇ(imapcode != IMAP_RESP_OK) {
    failf(ਅੰਕੜੇ, "Got unexpected imap-server response");
    ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
  }

  ਵਾਪਸ imap_perform_capability(ਅੰਕੜੇ, imapc);
}

/* For CAPABILITY responses */
ਸਥਿਰ CURLcode imap_state_capability_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਬਣਤਰ imap_conn *imapc,
                                           ਪੂਰਨ imapcode,
                                           imapstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ *ਸਤਰ = curlx_dyn_ptr(&imapc->pp.recvbuf);

  (ਖਾਲੀ)instate;

  /* Do we have an untagged response? */
  ਜੇ(imapcode == '*') {
    ਸਤਰ += 2;

    /* Loop through the data line */
    ਲਈ(;;) {
      size_t wordlen;
      ਜਦੋਂ(*ਸਤਰ && (ISBLANK(*ਸਤਰ) || ISNEWLINE(*ਸਤਰ)))
        ਸਤਰ++;

      ਜੇ(!*ਸਤਰ)
        ਰੋਕੋ;

      /* Extract the word */
      ਲਈ(wordlen = 0; ਸਤਰ[wordlen] && !ISBLANK(ਸਤਰ[wordlen]) &&
                       !ISNEWLINE(ਸਤਰ[wordlen]);)
        wordlen++;

      /* Does the server support the STARTTLS capability? */
      ਜੇ(wordlen == 8 && curl_strnequal(ਸਤਰ, "STARTTLS", 8))
        imapc->tls_supported = TRUE;

      /* Has the server explicitly disabled clear text authentication? */
      ਨਹੀਂਤਾਂ ਜੇ(wordlen == 13 && curl_strnequal(ਸਤਰ, "LOGINDISABLED", 13))
        imapc->login_disabled = TRUE;

      /* Does the server support the SASL-IR capability? */
      ਨਹੀਂਤਾਂ ਜੇ(wordlen == 7 && curl_strnequal(ਸਤਰ, "SASL-IR", 7))
        imapc->ir_supported = TRUE;

      /* Do we have a SASL based authentication mechanism? */
      ਨਹੀਂਤਾਂ ਜੇ(wordlen > 5 && curl_strnequal(ਸਤਰ, "AUTH=", 5)) {
        size_t llen;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ mechbit;

        ਸਤਰ += 5;
        wordlen -= 5;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(ਸਤਰ, wordlen, &llen);
        ਜੇ(mechbit && llen == wordlen)
          imapc->sasl.authmechs |= mechbit;
      }

      ਸਤਰ += wordlen;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.use_ssl && !Curl_conn_is_ssl(ਅੰਕੜੇ->conn, FIRSTSOCKET)) {
    /* PREAUTH is not compatible with STARTTLS. */
    ਜੇ(imapcode == IMAP_RESP_OK && imapc->tls_supported && !imapc->preauth) {
      /* Switch to TLS connection now */
      ਨਤੀਜਾ = imap_perform_starttls(ਅੰਕੜੇ, imapc);
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.use_ssl <= CURLUSESSL_TRY)
      ਨਤੀਜਾ = imap_perform_authentication(ਅੰਕੜੇ, imapc);
    ਨਹੀਂਤਾਂ {
      failf(ਅੰਕੜੇ, "STARTTLS not available.");
      ਨਤੀਜਾ = CURLE_USE_SSL_FAILED;
    }
  }
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = imap_perform_authentication(ਅੰਕੜੇ, imapc);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For STARTTLS responses */
ਸਥਿਰ CURLcode imap_state_starttls_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ imap_conn *imapc,
                                         ਪੂਰਨ imapcode,
                                         imapstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  (ਖਾਲੀ)instate;

  /* Pipelining in response is forbidden. */
  ਜੇ(imapc->pp.ਉਛਾਲ)
    ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;

  ਜੇ(imapcode != IMAP_RESP_OK) {
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.use_ssl != CURLUSESSL_TRY) {
      failf(ਅੰਕੜੇ, "STARTTLS denied");
      ਨਤੀਜਾ = CURLE_USE_SSL_FAILED;
    }
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = imap_perform_authentication(ਅੰਕੜੇ, imapc);
  }
  ਨਹੀਂਤਾਂ
    imap_state(ਅੰਕੜੇ, imapc, IMAP_UPGRADETLS);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For SASL authentication responses */
ਸਥਿਰ CURLcode imap_state_auth_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ imap_conn *imapc,
                                     ਪੂਰਨ imapcode,
                                     imapstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  saslprogress ਤਰੱਕੀ;

  (ਖਾਲੀ)instate;

  ਨਤੀਜਾ = Curl_sasl_continue(&imapc->sasl, ਅੰਕੜੇ, imapcode, &ਤਰੱਕੀ);
  ਜੇ(!ਨਤੀਜਾ)
    ਚੋਣ(ਤਰੱਕੀ) {
    ਹਾਲਤ SASL_ਪੂਰਾ:
      imap_state(ਅੰਕੜੇ, imapc, IMAP_STOP);  /* Authenticated */
      ਰੋਕੋ;
    ਹਾਲਤ SASL_IDLE:            /* No mechanism left after cancellation */
      ਜੇ(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
        /* Perform clear text authentication */
        ਨਤੀਜਾ = imap_perform_login(ਅੰਕੜੇ, imapc, ਅੰਕੜੇ->conn);
      ਨਹੀਂਤਾਂ {
        failf(ਅੰਕੜੇ, "Authentication cancelled");
        ਨਤੀਜਾ = CURLE_LOGIN_DENIED;
      }
      ਰੋਕੋ;
    ਮੂਲ:
      ਰੋਕੋ;
    }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For LOGIN responses */
ਸਥਿਰ CURLcode imap_state_login_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ imap_conn *imapc,
                                      ਪੂਰਨ imapcode,
                                      imapstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  (ਖਾਲੀ)instate;

  ਜੇ(imapcode != IMAP_RESP_OK) {
    failf(ਅੰਕੜੇ, "Access denied. %c", imapcode);
    ਨਤੀਜਾ = CURLE_LOGIN_DENIED;
  }
  ਨਹੀਂਤਾਂ
    /* End of connect phase */
    imap_state(ਅੰਕੜੇ, imapc, IMAP_STOP);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Detect IMAP listings vs. downloading a single email */
ਸਥਿਰ bool is_custom_fetch_listing_ਮੇਲ(ਅਚਲ ਅੱਖਰ *params)
{
  /* match " 1:* (FLAGS ..." or " 1,2,3 (FLAGS ..." */
  ਜੇ(*params++ != ' ')
    ਵਾਪਸ FALSE;

  ਜਦੋਂ(ISDIGIT(*params)) {
    params++;
    ਜੇ(*params == 0)
      ਵਾਪਸ FALSE;
  }
  ਜੇ(*params == ':')
    ਵਾਪਸ TRUE;
  ਜੇ(*params == ',')
    ਵਾਪਸ TRUE;
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ bool is_custom_fetch_listing(ਬਣਤਰ IMAP *imap)
{
  /* filter out "UID FETCH 1:* (FLAGS ..." queries to list emails */
  ਜੇ(!imap->custom)
    ਵਾਪਸ FALSE;
  ਨਹੀਂਤਾਂ ਜੇ(curl_strequal(imap->custom, "FETCH") && imap->custom_params) {
    ਅਚਲ ਅੱਖਰ *p = imap->custom_params;
    ਵਾਪਸ is_custom_fetch_listing_ਮੇਲ(p);
  }
  ਨਹੀਂਤਾਂ ਜੇ(curl_strequal(imap->custom, "UID") && imap->custom_params &&
          curl_strnequal(imap->custom_params, " FETCH ", 7)) {
    ਅਚਲ ਅੱਖਰ *p = imap->custom_params + 6;
    ਵਾਪਸ is_custom_fetch_listing_ਮੇਲ(p);
  }
  ਵਾਪਸ FALSE;
}

/* For LIST and SEARCH responses */
ਸਥਿਰ CURLcode imap_state_listsearch_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਬਣਤਰ imap_conn *imapc,
                                           ਪੂਰਨ imapcode,
                                           imapstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ *ਸਤਰ = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  ਬਣਤਰ IMAP *imap = Curl_meta_get(ਅੰਕੜੇ, CURL_META_IMAP_EASY);

  DEBUGASSERT(imap);
  ਜੇ(!imap)
    ਵਾਪਸ CURLE_FAILED_INIT;
  (ਖਾਲੀ)instate;

  ਜੇ(imapcode == '*' && is_custom_fetch_listing(imap)) {
    /* custom FETCH or UID FETCH for listing is not handled here */
  }
  ਨਹੀਂਤਾਂ ਜੇ(imapcode == '*') {
    /* Check if this response contains a literal (e.g. FETCH responses with
       body data). Literal syntax is {size}\r\n */
    ਅਚਲ ਅੱਖਰ *cr = memchr(ਸਤਰ, '\r', len);
    size_t ਸਤਰ_len = cr ? (size_t)(cr - ਸਤਰ) : len;
    ਅਚਲ ਅੱਖਰ *ptr = imap_ਲੱਭੋ_literal(ਸਤਰ, ਸਤਰ_len);
    ਜੇ(ptr) {
      curl_off_t ਮਾਪ = 0;
      bool parsed = FALSE;
      ptr++;
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&ptr, &ਮਾਪ, CURL_OFF_T_MAX) &&
         !curlx_str_single(&ptr, '}'))
        parsed = TRUE;

      ਜੇ(parsed) {
        ਬਣਤਰ pingpong *pp = &imapc->pp;
        size_t ਬਫਰ_len = curlx_dyn_len(&pp->recvbuf);
        size_t after_ਸਿਰਲੇਖ = ਬਫਰ_len - pp->nfinal;

        /* This is a literal response, setup to receive the body data */
        infof(ਅੰਕੜੇ, "Found %" FMT_OFF_T " bytes to download", ਮਾਪ);

        /* First write the header line */
        ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, ਸਤਰ, len);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;

        /* Handle data already in buffer after the header line */
        ਜੇ(after_ਸਿਰਲੇਖ > 0) {
          /* There is already data in the buffer that is part of the literal
             body or subsequent responses */
          size_t chunk = after_ਸਿਰਲੇਖ;

          /* Keep only the data after the header line */
          curlx_dyn_ਪੂਛ(&pp->recvbuf, chunk);
          pp->nfinal = 0; /* done */

          /* Limit chunk to the literal size */
          ਜੇ((curl_off_t)chunk > ਮਾਪ)
            chunk = (size_t)ਮਾਪ;

          ਜੇ(chunk) {
            /* Write the literal body data */
            ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY,
                                       curlx_dyn_ptr(&pp->recvbuf), chunk);
            ਜੇ(ਨਤੀਜਾ)
              ਵਾਪਸ ਨਤੀਜਾ;
          }

          /* Handle remaining data in buffer (either more literal data or
             subsequent responses) */
          ਜੇ(after_ਸਿਰਲੇਖ > chunk) {
            /* Keep the data after the literal body */
            pp->ਉਛਾਲ = after_ਸਿਰਲੇਖ - chunk;
            curlx_dyn_ਪੂਛ(&pp->recvbuf, pp->ਉਛਾਲ);
          }
          ਨਹੀਂਤਾਂ {
            pp->ਉਛਾਲ = 0;
            curlx_dyn_reset(&pp->recvbuf);
          }
        }
        ਨਹੀਂਤਾਂ {
          /* No data in buffer yet, reset overflow */
          pp->ਉਛਾਲ = 0;
        }

        ਜੇ((CURL_OFF_T_MAX - ਮਾਪ) < (curl_off_t)len)
          /* unlikely to actually be a transfer this big, but avoid integer
ਉਛਾਲ*/
          ਮਾਪ = CURL_OFF_T_MAX;
        ਨਹੀਂਤਾਂ
          ਮਾਪ += len;

        /* Progress size includes both header line and literal body */
        Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, ਮਾਪ);

        ਜੇ(ਅੰਕੜੇ->req.bytecount == ਮਾਪ)
          /* All data already transferred (header + literal body) */
          Curl_xfer_ਤਿਆਰੀ_nop(ਅੰਕੜੇ);
        ਨਹੀਂਤਾਂ {
          /* Setup to receive the literal body data.
             maxdownload and transfer size include both header line and
             literal body */
          ਅੰਕੜੇ->req.maxdownload = ਮਾਪ;
          Curl_xfer_ਤਿਆਰੀ_recv(ਅੰਕੜੇ, FIRSTSOCKET, ਮਾਪ);
        }
        /* End of DO phase */
        imap_state(ਅੰਕੜੇ, imapc, IMAP_STOP);
      }
      ਨਹੀਂਤਾਂ {
        /* Failed to parse literal, write the line */
        ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, ਸਤਰ, len);
      }
    }
    ਨਹੀਂਤਾਂ {
      /* No literal, write the line as-is */
      ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, ਸਤਰ, len);
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(imapcode != IMAP_RESP_OK)
    ਨਤੀਜਾ = CURLE_QUOTE_ERROR;
  ਨਹੀਂਤਾਂ
    /* End of DO phase */
    imap_state(ਅੰਕੜੇ, imapc, IMAP_STOP);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For SELECT responses */
ਸਥਿਰ CURLcode imap_state_select_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ imap_conn *imapc,
                                       ਬਣਤਰ IMAP *imap,
                                       ਪੂਰਨ imapcode,
                                       imapstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  (ਖਾਲੀ)instate;

  ਜੇ(imapcode == '*') {
    /* See if this is an UIDVALIDITY response */
    ਅਚਲ ਅੱਖਰ *ਸਤਰ = curlx_dyn_ptr(&imapc->pp.recvbuf);
    size_t len = curlx_dyn_len(&imapc->pp.recvbuf);
    ਜੇ((len >= 18) && checkprefix("OK [UIDVALIDITY ", &ਸਤਰ[2])) {
      curl_off_t ਮੁੱਲ;
      ਅਚਲ ਅੱਖਰ *p = &ਸਤਰ[2] + CURL_CSTRLEN("OK [UIDVALIDITY ");
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &ਮੁੱਲ, UINT_MAX)) {
        imapc->mb_uidvalidity = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਮੁੱਲ;
        imapc->mb_uidvalidity_ਸੈੱਟ = TRUE;
      }
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(imapcode == IMAP_RESP_OK) {
    /* Check if the UIDVALIDITY has been specified and matches */
    ਜੇ(imap->uidvalidity_ਸੈੱਟ && imapc->mb_uidvalidity_ਸੈੱਟ &&
       (imap->uidvalidity != imapc->mb_uidvalidity)) {
      failf(ਅੰਕੜੇ, "Mailbox UIDVALIDITY has changed");
      ਨਤੀਜਾ = CURLE_REMOTE_FILE_NOT_ਮਿਲਿਆ;
    }
    ਨਹੀਂਤਾਂ {
      /* Note the currently opened mailbox on this connection */
      DEBUGASSERT(!imapc->mailbox);
      imapc->mailbox = curlx_strdup(imap->mailbox);
      ਜੇ(!imapc->mailbox)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

      ਜੇ(imap->custom)
        ਨਤੀਜਾ = imap_perform_list(ਅੰਕੜੇ, imapc, imap);
      ਨਹੀਂਤਾਂ ਜੇ(imap->query)
        ਨਤੀਜਾ = imap_perform_ਖੋਜ(ਅੰਕੜੇ, imapc, imap);
      ਨਹੀਂਤਾਂ
        ਨਤੀਜਾ = imap_perform_fetch(ਅੰਕੜੇ, imapc, imap);
    }
  }
  ਨਹੀਂਤਾਂ {
    failf(ਅੰਕੜੇ, "Select failed");
    ਨਤੀਜਾ = CURLE_LOGIN_DENIED;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For the (first line of the) FETCH responses */
ਸਥਿਰ CURLcode imap_state_fetch_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ imap_conn *imapc,
                                      ਪੂਰਨ imapcode,
                                      imapstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ pingpong *pp = &imapc->pp;
  ਅਚਲ ਅੱਖਰ *ptr = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  bool parsed = FALSE;
  curl_off_t ਮਾਪ = 0;

  (ਖਾਲੀ)instate;

  ਜੇ(imapcode != '*') {
    Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, -1);
    imap_state(ਅੰਕੜੇ, imapc, IMAP_STOP);
    ਵਾਪਸ CURLE_REMOTE_FILE_NOT_ਮਿਲਿਆ;
  }

  /* Something like this is received "* 1 FETCH (BODY[TEXT] {2021}\r" so parse
     the continuation data contained within the curly brackets */
  ptr = imap_ਲੱਭੋ_literal(ptr, len);
  ਜੇ(ptr) {
    ptr++;
    ਜੇ(!curlx_str_ਸੰਖਿਆ(&ptr, &ਮਾਪ, CURL_OFF_T_MAX) &&
       !curlx_str_single(&ptr, '}'))
      parsed = TRUE;
  }

  ਜੇ(parsed) {
    infof(ਅੰਕੜੇ, "Found %" FMT_OFF_T " bytes to download", ਮਾਪ);
    Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, ਮਾਪ);

    ਜੇ(pp->ਉਛਾਲ) {
      /* At this point there is a data in the receive buffer that is body
         content, send it as body and then skip it. Do note that there may
         even be additional "headers" after the body. */
      size_t chunk = pp->ਉਛਾਲ;

      /* keep only the overflow */
      curlx_dyn_ਪੂਛ(&pp->recvbuf, chunk);
      pp->nfinal = 0; /* done */

      ਜੇ((curl_off_t)chunk > ਮਾਪ)
        /* The conversion from curl_off_t to size_t is always fine here */
        chunk = (size_t)ਮਾਪ;

      ਜੇ(!chunk) {
        /* no size, we are done with the data */
        imap_state(ਅੰਕੜੇ, imapc, IMAP_STOP);
        ਵਾਪਸ CURLE_OK;
      }
      ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY,
                                 curlx_dyn_ptr(&pp->recvbuf), chunk);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;

      infof(ਅੰਕੜੇ, "Written %zu bytes, %" FMT_OFF_T
            " bytes are left for transfer", chunk, (curl_off_t)(ਮਾਪ - chunk));

      /* Have we used the entire overflow or part of it?*/
      ਜੇ(pp->ਉਛਾਲ > chunk) {
        /* remember the remaining trailing overflow data */
        pp->ਉਛਾਲ -= chunk;
        curlx_dyn_ਪੂਛ(&pp->recvbuf, pp->ਉਛਾਲ);
      }
      ਨਹੀਂਤਾਂ {
        pp->ਉਛਾਲ = 0; /* handled */
        /* Free the cache */
        curlx_dyn_reset(&pp->recvbuf);
      }
    }

    ਜੇ(ਅੰਕੜੇ->req.bytecount == ਮਾਪ)
      /* The entire data is already transferred! */
      Curl_xfer_ਤਿਆਰੀ_nop(ਅੰਕੜੇ);
    ਨਹੀਂਤਾਂ {
      /* IMAP download */
      ਅੰਕੜੇ->req.maxdownload = ਮਾਪ;
      Curl_xfer_ਤਿਆਰੀ_recv(ਅੰਕੜੇ, FIRSTSOCKET, ਮਾਪ);
    }
  }
  ਨਹੀਂਤਾਂ {
    /* We do not know how to parse this line */
    failf(ਅੰਕੜੇ, "Failed to parse FETCH response.");
    ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;
  }

  /* End of DO phase */
  imap_state(ਅੰਕੜੇ, imapc, IMAP_STOP);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For final FETCH responses performed after the download */
ਸਥਿਰ CURLcode imap_state_fetch_final_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                            ਬਣਤਰ imap_conn *imapc,
                                            ਪੂਰਨ imapcode,
                                            imapstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  (ਖਾਲੀ)instate;

  ਜੇ(imapcode != IMAP_RESP_OK)
    ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;
  ਨਹੀਂਤਾਂ
    /* End of DONE phase */
    imap_state(ਅੰਕੜੇ, imapc, IMAP_STOP);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For APPEND responses */
ਸਥਿਰ CURLcode imap_state_append_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ imap_conn *imapc,
                                       ਪੂਰਨ imapcode,
                                       imapstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  (ਖਾਲੀ)instate;

  ਜੇ(imapcode != '+') {
    ਨਤੀਜਾ = CURLE_UPLOAD_FAILED;
  }
  ਨਹੀਂਤਾਂ {
    /* Set the progress upload size */
    Curl_pgrs_ਸੈੱਟ_Upload_ਮਾਪ(ਅੰਕੜੇ, ਅੰਕੜੇ->state.infilesize);

    /* IMAP upload */
    Curl_xfer_ਤਿਆਰੀ_send(ਅੰਕੜੇ, FIRSTSOCKET);

    /* End of DO phase */
    imap_state(ਅੰਕੜੇ, imapc, IMAP_STOP);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For final APPEND responses performed after the upload */
ਸਥਿਰ CURLcode imap_state_append_final_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ imap_conn *imapc,
                                             ਪੂਰਨ imapcode,
                                             imapstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  (ਖਾਲੀ)instate;

  ਜੇ(imapcode != IMAP_RESP_OK)
    ਨਤੀਜਾ = CURLE_UPLOAD_FAILED;
  ਨਹੀਂਤਾਂ
    /* End of DONE phase */
    imap_state(ਅੰਕੜੇ, imapc, IMAP_STOP);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode imap_pp_statemachine(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ connectdata *conn)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ imapcode;
  ਬਣਤਰ imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ਬਣਤਰ IMAP *imap = Curl_meta_get(ਅੰਕੜੇ, CURL_META_IMAP_EASY);
  ਬਣਤਰ pingpong *pp;
  size_t nread = 0;

  ਜੇ(!imapc || !imap)
    ਵਾਪਸ CURLE_FAILED_INIT;
  pp = &imapc->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not IMAP */
upgrade_tls:
  ਜੇ(imapc->state == IMAP_UPGRADETLS) {
    ਨਤੀਜਾ = imap_perform_upgrade_tls(ਅੰਕੜੇ, imapc, conn);
    ਜੇ(ਨਤੀਜਾ || (imapc->state == IMAP_UPGRADETLS))
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* Flush any data that needs to be sent */
  ਜੇ(pp->sendleft)
    ਵਾਪਸ Curl_pp_flushsend(ਅੰਕੜੇ, pp);

  ਕਰੋ {
    /* Read the response from the server */
    ਨਤੀਜਾ = Curl_pp_readresp(ਅੰਕੜੇ, FIRSTSOCKET, pp, &imapcode, &nread);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    /* Was there an error parsing the response line? */
    ਜੇ(imapcode == -1)
      ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;

    ਜੇ(!imapcode)
      ਰੋਕੋ;

    /* We have now received a full IMAP server response */
    ਚੋਣ(imapc->state) {
    ਹਾਲਤ IMAP_SERVERGREET:
      ਨਤੀਜਾ = imap_state_servergreet_resp(ਅੰਕੜੇ, imapc,
                                           imapcode, imapc->state);
      ਰੋਕੋ;

    ਹਾਲਤ IMAP_CAPABILITY:
      ਨਤੀਜਾ = imap_state_capability_resp(ਅੰਕੜੇ, imapc, imapcode, imapc->state);
      ਰੋਕੋ;

    ਹਾਲਤ IMAP_STARTTLS:
      ਨਤੀਜਾ = imap_state_starttls_resp(ਅੰਕੜੇ, imapc, imapcode, imapc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      ਜੇ(!ਨਤੀਜਾ && (imapc->state == IMAP_UPGRADETLS))
        ਜਾਓ upgrade_tls;
      ਰੋਕੋ;

    ਹਾਲਤ IMAP_AUTHENTICATE:
      ਨਤੀਜਾ = imap_state_auth_resp(ਅੰਕੜੇ, imapc, imapcode, imapc->state);
      ਰੋਕੋ;

    ਹਾਲਤ IMAP_LOGIN:
      ਨਤੀਜਾ = imap_state_login_resp(ਅੰਕੜੇ, imapc, imapcode, imapc->state);
      ਰੋਕੋ;

    ਹਾਲਤ IMAP_LIST:
    ਹਾਲਤ IMAP_ਖੋਜ:
      ਨਤੀਜਾ = imap_state_listsearch_resp(ਅੰਕੜੇ, imapc, imapcode, imapc->state);
      ਰੋਕੋ;

    ਹਾਲਤ IMAP_SELECT:
      ਨਤੀਜਾ = imap_state_select_resp(ਅੰਕੜੇ, imapc, imap,
                                      imapcode, imapc->state);
      ਰੋਕੋ;

    ਹਾਲਤ IMAP_FETCH:
      ਨਤੀਜਾ = imap_state_fetch_resp(ਅੰਕੜੇ, imapc, imapcode, imapc->state);
      ਰੋਕੋ;

    ਹਾਲਤ IMAP_FETCH_FINAL:
      ਨਤੀਜਾ = imap_state_fetch_final_resp(ਅੰਕੜੇ, imapc,
                                           imapcode, imapc->state);
      ਰੋਕੋ;

    ਹਾਲਤ IMAP_APPEND:
      ਨਤੀਜਾ = imap_state_append_resp(ਅੰਕੜੇ, imapc, imapcode, imapc->state);
      ਰੋਕੋ;

    ਹਾਲਤ IMAP_APPEND_FINAL:
      ਨਤੀਜਾ = imap_state_append_final_resp(ਅੰਕੜੇ, imapc,
                                            imapcode, imapc->state);
      ਰੋਕੋ;

    ਹਾਲਤ IMAP_LOGOUT:
    ਮੂਲ:
      /* internal error */
      imap_state(ਅੰਕੜੇ, imapc, IMAP_STOP);
      ਰੋਕੋ;
    }
  } ਜਦੋਂ(!ਨਤੀਜਾ && imapc->state != IMAP_STOP && Curl_pp_moredata(pp));

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Called repeatedly until done from multi.c */
ਸਥਿਰ CURLcode imap_multi_statemach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ imap_conn *imapc =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_IMAP_CONN);

  *ਪੂਰਾ = FALSE;
  ਜੇ(!imapc)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਨਤੀਜਾ = Curl_pp_statemach(ਅੰਕੜੇ, &imapc->pp, FALSE, FALSE);
  *ਪੂਰਾ = (imapc->state == IMAP_STOP);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode imap_ਖੰਡ_statemach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ imap_conn *imapc,
                                     bool disconnecting)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜਦੋਂ(imapc->state != IMAP_STOP && !ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_pp_statemach(ਅੰਕੜੇ, &imapc->pp, TRUE, disconnecting);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For the IMAP "protocol connect" and "doing" phases only */
ਸਥਿਰ CURLcode imap_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ imap_conn *imapc =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_IMAP_CONN);
  ਵਾਪਸ imapc ? Curl_pp_pollset(ਅੰਕੜੇ, &imapc->pp, ps) : CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ imap_easy_reset(ਬਣਤਰ IMAP *imap)
{
  curlx_safefree(imap->mailbox);
  curlx_safefree(imap->uid);
  curlx_safefree(imap->mindex);
  curlx_safefree(imap->section);
  curlx_safefree(imap->partial);
  curlx_safefree(imap->query);
  curlx_safefree(imap->custom);
  curlx_safefree(imap->custom_params);
  imap->uidvalidity_ਸੈੱਟ = FALSE;
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
ਸਥਿਰ bool imap_is_bchar(ਅੱਖਰ ch)
{
  /* Performing the alnum check first with macro is faster because of ASCII
     arithmetic */
  ਵਾਪਸ ch && (ISALNUM(ch) || strchr(":@/&=-._~!$\'()*+,%", ch));
}

/***********************************************************************
 *
 * imap_parse_url_options()
 *
 * Parse the URL login options.
 */
ਸਥਿਰ CURLcode imap_parse_url_options(ਬਣਤਰ connectdata *conn,
                                       ਬਣਤਰ imap_conn *imapc)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ *ptr = conn->options;
  bool prefer_login = FALSE;

  ਜਦੋਂ(!ਨਤੀਜਾ && ptr && *ptr) {
    ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ = ptr;
    ਅਚਲ ਅੱਖਰ *ਮੁੱਲ;

    ਜਦੋਂ(*ptr && *ptr != '=')
      ptr++;

    ਮੁੱਲ = ptr + 1;

    ਜਦੋਂ(*ptr && *ptr != ';')
      ptr++;

    ਜੇ(curl_strnequal(ਕੁੰਜੀ, "AUTH=+LOGIN", 11)) {
      /* User prefers plaintext LOGIN over any SASL, including SASL LOGIN */
      prefer_login = TRUE;
      imapc->sasl.prefmech = SASL_AUTH_NONE;
    }
    ਨਹੀਂਤਾਂ ਜੇ(curl_strnequal(ਕੁੰਜੀ, "AUTH=", 5)) {
      prefer_login = FALSE;
      ਨਤੀਜਾ = Curl_sasl_parse_url_auth_option(&imapc->sasl,
                                               ਮੁੱਲ, ptr - ਮੁੱਲ);
    }
    ਨਹੀਂਤਾਂ {
      prefer_login = FALSE;
      ਨਤੀਜਾ = CURLE_URL_MALFORMAT;
    }

    ਜੇ(*ptr == ';')
      ptr++;
  }

  ਜੇ(prefer_login)
    imapc->preftype = IMAP_TYPE_CLEARTEXT;
  ਨਹੀਂਤਾਂ {
    ਚੋਣ(imapc->sasl.prefmech) {
    ਹਾਲਤ SASL_AUTH_NONE:
      imapc->preftype = IMAP_TYPE_NONE;
      ਰੋਕੋ;
    ਹਾਲਤ SASL_AUTH_DEFAULT:
      imapc->preftype = IMAP_TYPE_ANY;
      ਰੋਕੋ;
    ਮੂਲ:
      imapc->preftype = IMAP_TYPE_SASL;
      ਰੋਕੋ;
    }
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * imap_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
ਸਥਿਰ CURLcode imap_parse_url_ਰਸਤਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ IMAP *imap)
{
  /* The imap struct is already initialized in imap_connect() */
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ *begin = &ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ[1]; /* skip leading slash */
  ਅਚਲ ਅੱਖਰ *ptr = begin;

  /* See how much of the URL is a valid path and decode it */
  ਜਦੋਂ(imap_is_bchar(*ptr))
    ptr++;

  ਜੇ(ptr != begin) {
    /* Remove the trailing slash if present */
    ਅਚਲ ਅੱਖਰ *ਸਿਰਾ = ptr;
    ਜੇ(ਸਿਰਾ > begin && ਸਿਰਾ[-1] == '/')
      ਸਿਰਾ--;

    ਨਤੀਜਾ = Curl_urldecode(begin, ਸਿਰਾ - begin, &imap->mailbox, ਨਿਰਾ,
                            REJECT_CTRL);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਨਹੀਂਤਾਂ
    imap->mailbox = ਨਿਰਾ;

  /* There can be any number of parameters in the form ";NAME=VALUE" */
  ਜਦੋਂ(*ptr == ';') {
    ਅੱਖਰ *name;
    ਅੱਖਰ *ਮੁੱਲ;
    size_t valuelen;

    /* Find the length of the name parameter */
    begin = ++ptr;
    ਜਦੋਂ(*ptr && *ptr != '=')
      ptr++;

    ਜੇ(!*ptr)
      ਵਾਪਸ CURLE_URL_MALFORMAT;

    /* Decode the name parameter */
    ਨਤੀਜਾ = Curl_urldecode(begin, ptr - begin, &name, ਨਿਰਾ,
                            REJECT_CTRL);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    /* Find the length of the value parameter */
    begin = ++ptr;
    ਜਦੋਂ(imap_is_bchar(*ptr))
      ptr++;

    /* Decode the value parameter */
    ਨਤੀਜਾ = Curl_urldecode(begin, ptr - begin, &ਮੁੱਲ, &valuelen,
                            REJECT_CTRL);
    ਜੇ(ਨਤੀਜਾ) {
      curlx_free(name);
      ਵਾਪਸ ਨਤੀਜਾ;
    }

    DEBUGF(infof(ਅੰਕੜੇ, "IMAP URL parameter '%s' = '%s'", name, ਮੁੱਲ));

    /* Process the known hierarchical parameters (UIDVALIDITY, UID, SECTION
       and PARTIAL) stripping of the trailing slash character if it is
       present.

       Note: Unknown parameters trigger a URL_MALFORMAT error. */
    ਜੇ(valuelen > 0 && ਮੁੱਲ[valuelen - 1] == '/')
      ਮੁੱਲ[valuelen - 1] = '\0';
    ਜੇ(valuelen) {
      ਜੇ(curl_strequal(name, "UIDVALIDITY") && !imap->uidvalidity_ਸੈੱਟ) {
        curl_off_t ਅੰਕ;
        ਅਚਲ ਅੱਖਰ *p = (ਅਚਲ ਅੱਖਰ *)ਮੁੱਲ;
        ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &ਅੰਕ, UINT_MAX)) {
          imap->uidvalidity = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਅੰਕ;
          imap->uidvalidity_ਸੈੱਟ = TRUE;
        }
        curlx_free(ਮੁੱਲ);
      }
      ਨਹੀਂਤਾਂ ਜੇ(curl_strequal(name, "UID") && !imap->uid) {
        imap->uid = ਮੁੱਲ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(curl_strequal(name, "MAILINDEX") && !imap->mindex) {
        imap->mindex = ਮੁੱਲ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(curl_strequal(name, "SECTION") && !imap->section) {
        imap->section = ਮੁੱਲ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(curl_strequal(name, "PARTIAL") && !imap->partial) {
        imap->partial = ਮੁੱਲ;
      }
      ਨਹੀਂਤਾਂ {
        curlx_free(name);
        curlx_free(ਮੁੱਲ);
        ਵਾਪਸ CURLE_URL_MALFORMAT;
      }
    }
    ਨਹੀਂਤਾਂ
      /* blank? */
      curlx_free(ਮੁੱਲ);
    curlx_free(name);
  }

  /* Does the URL contain a query parameter? Only valid when we have a mailbox
     and no UID as per RFC-5092 */
  ਜੇ(imap->mailbox && !imap->uid && !imap->mindex) {
    /* Get the query parameter, URL decoded */
    CURLUcode uc = curl_url_get(ਅੰਕੜੇ->state.uh, CURLUPART_QUERY, &imap->query,
                                CURLU_URLDECODE);
    ਜੇ(uc == CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  /* Any extra stuff at the end of the URL is an error */
  ਜੇ(*ptr)
    ਵਾਪਸ CURLE_URL_MALFORMAT;

  ਵਾਪਸ CURLE_OK;
}

/***********************************************************************
 *
 * imap_parse_custom_request()
 *
 * Parse the custom request.
 */
ਸਥਿਰ CURLcode imap_parse_custom_request(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ IMAP *imap)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ *custom = CURL_EASY_STR(ਅੰਕੜੇ, STRING_CUSTOMREQUEST);

  ਜੇ(custom) {
    /* URL decode the custom request */
    ਨਤੀਜਾ = Curl_urldecode(custom, 0, &imap->custom, ਨਿਰਾ, REJECT_CTRL);

    /* Extract the parameters if specified */
    ਜੇ(!ਨਤੀਜਾ) {
      ਅਚਲ ਅੱਖਰ *params = imap->custom;

      ਜਦੋਂ(*params && *params != ' ')
        params++;

      ਜੇ(*params) {
        imap->custom_params = curlx_strdup(params);
        imap->custom[params - imap->custom] = '\0';

        ਜੇ(!imap->custom_params)
          ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
    }
  }

  ਵਾਪਸ ਨਤੀਜਾ;
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
ਸਥਿਰ CURLcode imap_connect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਬਣਤਰ imap_conn *imapc =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_IMAP_CONN);
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  *ਪੂਰਾ = FALSE; /* default to not done yet */
  ਜੇ(!imapc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* Parse the URL options */
  ਨਤੀਜਾ = imap_parse_url_options(ਅੰਕੜੇ->conn, imapc);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* Start off waiting for the server greeting response */
  imap_state(ਅੰਕੜੇ, imapc, IMAP_SERVERGREET);

  /* Start off with an response id of '*' */
  curlx_strcopy(imapc->resptag, ਆਕਾਰ(imapc->resptag), STRCONST("*"));

  ਨਤੀਜਾ = imap_multi_statemach(ਅੰਕੜੇ, ਪੂਰਾ);

  ਵਾਪਸ ਨਤੀਜਾ;
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
ਸਥਿਰ CURLcode imap_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLcode ਸਥਿਤੀ,
                          bool premature)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ਬਣਤਰ IMAP *imap = Curl_meta_get(ਅੰਕੜੇ, CURL_META_IMAP_EASY);

  ਜੇ(!imapc)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਜੇ(!imap)
    ਵਾਪਸ CURLE_OK;

  ਜੇ(ਸਥਿਤੀ || premature) {
    CURL_TRC_M(ਅੰਕੜੇ, "IMAP done, not finalizing transfer");
    connclose(conn); /* marked for closure */
    ਨਤੀਜਾ = ਸਥਿਤੀ;         /* use the already set error code */
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.connect_only &&
          ((!imap->custom && (imap->uid || imap->mindex)) ||
           (imap->custom && ਅੰਕੜੇ->req.maxdownload > 0) ||
           ਅੰਕੜੇ->state.upload || IS_MIME_POST(ਅੰਕੜੇ))) {
    /* Handle responses after FETCH or APPEND transfer has finished.
       For custom commands, check if we set up a download which indicates
       a FETCH-like command with literal data. */

    ਜੇ(!ਅੰਕੜੇ->state.upload && !IS_MIME_POST(ਅੰਕੜੇ))
      imap_state(ਅੰਕੜੇ, imapc, IMAP_FETCH_FINAL);
    ਨਹੀਂਤਾਂ {
      /* End the APPEND command first by sending an empty line */
      ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &imapc->pp, "%s", "");
      ਜੇ(!ਨਤੀਜਾ)
        imap_state(ਅੰਕੜੇ, imapc, IMAP_APPEND_FINAL);
    }

    /* Run the state-machine */
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = imap_ਖੰਡ_statemach(ਅੰਕੜੇ, imapc, FALSE);
  }

  imap_easy_reset(imap);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * imap_perform()
 *
 * This is the actual DO function for IMAP. Fetch or append a message, or do
 * other things according to the options previously setup.
 */
ਸਥਿਰ CURLcode imap_perform(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *connected,
                             bool *dophase_ਪੂਰਾ)
{
  /* This is IMAP and no proxy */
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ਬਣਤਰ IMAP *imap = Curl_meta_get(ਅੰਕੜੇ, CURL_META_IMAP_EASY);
  bool selected = FALSE;

  DEBUGF(infof(ਅੰਕੜੇ, "DO phase starts"));
  ਜੇ(!imapc || !imap)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(ਅੰਕੜੇ->req.no_body) {
    /* Requested no body means no transfer */
    imap->transfer = PPTRANSFER_INFO;
  }

  *dophase_ਪੂਰਾ = FALSE; /* not done yet */

  /* Determine if the requested mailbox (with the same UIDVALIDITY if set)
     has already been selected on this connection */
  ਜੇ(imap->mailbox && imapc->mailbox &&
     (!strcmp(imap->mailbox, imapc->mailbox) ||
      (curl_strequal(imap->mailbox, "INBOX") &&
       curl_strequal(imapc->mailbox, "INBOX"))) &&
     (!imap->uidvalidity_ਸੈੱਟ || !imapc->mb_uidvalidity_ਸੈੱਟ ||
      (imap->uidvalidity == imapc->mb_uidvalidity)))
    selected = TRUE;

  /* Start the first command in the DO phase */
  ਜੇ(ਅੰਕੜੇ->state.upload || IS_MIME_POST(ਅੰਕੜੇ))
    /* APPEND can be executed directly */
    ਨਤੀਜਾ = imap_perform_append(ਅੰਕੜੇ, imapc, imap);
  ਨਹੀਂਤਾਂ ਜੇ(imap->custom && (selected || !imap->mailbox))
    /* Custom command using the same mailbox or no mailbox */
    ਨਤੀਜਾ = imap_perform_list(ਅੰਕੜੇ, imapc, imap);
  ਨਹੀਂਤਾਂ ਜੇ(!imap->custom && selected && (imap->uid || imap->mindex))
    /* FETCH from the same mailbox */
    ਨਤੀਜਾ = imap_perform_fetch(ਅੰਕੜੇ, imapc, imap);
  ਨਹੀਂਤਾਂ ਜੇ(!imap->custom && selected && imap->query)
    /* SEARCH the current mailbox */
    ਨਤੀਜਾ = imap_perform_ਖੋਜ(ਅੰਕੜੇ, imapc, imap);
  ਨਹੀਂਤਾਂ ਜੇ(imap->mailbox && !selected &&
          (imap->custom || imap->uid || imap->mindex || imap->query))
    /* SELECT the mailbox */
    ਨਤੀਜਾ = imap_perform_select(ਅੰਕੜੇ, imapc, imap);
  ਨਹੀਂਤਾਂ
    /* LIST */
    ਨਤੀਜਾ = imap_perform_list(ਅੰਕੜੇ, imapc, imap);

  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* Run the state-machine */
  ਨਤੀਜਾ = imap_multi_statemach(ਅੰਕੜੇ, dophase_ਪੂਰਾ);

  *connected = Curl_conn_is_connected(conn, FIRSTSOCKET);

  ਜੇ(*dophase_ਪੂਰਾ)
    DEBUGF(infof(ਅੰਕੜੇ, "DO phase is complete"));

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Call this when the DO phase has completed */
ਸਥਿਰ CURLcode imap_dophase_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ IMAP *imap,
                                  bool connected)
{
  (ਖਾਲੀ)connected;

  ਜੇ(imap->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_ਤਿਆਰੀ_nop(ਅੰਕੜੇ);

  ਵਾਪਸ CURLE_OK;
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
ਸਥਿਰ CURLcode imap_regular_transfer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ IMAP *imap,
                                      bool *dophase_ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  ਅੰਕੜੇ->req.ਮਾਪ = -1;

  /* Set the progress data */
  Curl_pgrsReset(ਅੰਕੜੇ);

  /* Carry out the perform */
  ਨਤੀਜਾ = imap_perform(ਅੰਕੜੇ, &connected, dophase_ਪੂਰਾ);

  /* Perform post DO phase operations if necessary */
  ਜੇ(!ਨਤੀਜਾ && *dophase_ਪੂਰਾ)
    ਨਤੀਜਾ = imap_dophase_ਪੂਰਾ(ਅੰਕੜੇ, imap, connected);

  ਵਾਪਸ ਨਤੀਜਾ;
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
ਸਥਿਰ CURLcode imap_do(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਬਣਤਰ IMAP *imap = Curl_meta_get(ਅੰਕੜੇ, CURL_META_IMAP_EASY);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  *ਪੂਰਾ = FALSE; /* default to false */

  ਜੇ(!imap)
    ਵਾਪਸ CURLE_FAILED_INIT;
  /* Parse the URL path */
  ਨਤੀਜਾ = imap_parse_url_ਰਸਤਾ(ਅੰਕੜੇ, imap);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* Parse the custom request */
  ਨਤੀਜਾ = imap_parse_custom_request(ਅੰਕੜੇ, imap);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਨਤੀਜਾ = imap_regular_transfer(ਅੰਕੜੇ, imap, ਪੂਰਾ);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * imap_disconnect()
 *
 * Disconnect from an IMAP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ਸਥਿਰ CURLcode imap_disconnect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ connectdata *conn, bool dead_connection)
{
  ਬਣਤਰ imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);

  ਜੇ(imapc &&
     /* We cannot send quit unconditionally. If this connection is stale or
        bad in any way (pingpong has pending data to send),
        sending quit and waiting around here will make the
        disconnect wait in vain and cause more problems than we need to. */
     !dead_connection && conn->ਬਿੱਟ.protoconnstart &&
     !Curl_pp_needs_flush(ਅੰਕੜੇ, &imapc->pp) &&
     !imap_perform_logout(ਅੰਕੜੇ, imapc))
    (ਖਾਲੀ)imap_ਖੰਡ_statemach(ਅੰਕੜੇ, imapc, TRUE); /* ignore errors */

  ਵਾਪਸ CURLE_OK;
}

/* Called from multi.c while DOing */
ਸਥਿਰ CURLcode imap_doing(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *dophase_ਪੂਰਾ)
{
  ਬਣਤਰ IMAP *imap = Curl_meta_get(ਅੰਕੜੇ, CURL_META_IMAP_EASY);
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!imap)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਨਤੀਜਾ = imap_multi_statemach(ਅੰਕੜੇ, dophase_ਪੂਰਾ);
  ਜੇ(ਨਤੀਜਾ)
    DEBUGF(infof(ਅੰਕੜੇ, "DO phase failed"));
  ਨਹੀਂਤਾਂ ਜੇ(*dophase_ਪੂਰਾ) {
    ਨਤੀਜਾ = imap_dophase_ਪੂਰਾ(ਅੰਕੜੇ, imap, FALSE /* not connected */);

    DEBUGF(infof(ਅੰਕੜੇ, "DO phase is complete"));
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ imap_easy_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ IMAP *imap = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  imap_easy_reset(imap);
  curlx_free(imap);
}

ਸਥਿਰ ਖਾਲੀ imap_conn_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ imap_conn *imapc = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  Curl_pp_disconnect(&imapc->pp);
  curlx_safefree(imapc->mailbox);
  curlx_free(imapc);
}

/* SASL parameters for the imap protocol */
ਸਥਿਰ ਅਚਲ ਬਣਤਰ SASLproto saslimap = {
  "imap",                     /* The service name */
  imap_perform_authenticate,  /* Send authentication command */
  imap_continue_authenticate, /* Send authentication continuation */
  imap_cancel_authenticate,   /* Send authentication cancellation */
  imap_get_ਸੁਨੇਹਾ,           /* Get SASL response message */
  0,                          /* No maximum initial response length */
  '+',                        /* Code received when continuation is expected */
  IMAP_RESP_OK,               /* Code to receive upon authentication success */
  SASL_AUTH_DEFAULT,          /* Default mechanisms */
  SASL_FLAG_BASE64            /* Configuration flags */
};

ਸਥਿਰ CURLcode imap_ਤਿਆਰੀ_connection(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ imap_conn *imapc;
  ਬਣਤਰ pingpong *pp;
  ਬਣਤਰ IMAP *imap;

  imapc = curlx_calloc(1, ਆਕਾਰ(*imapc));
  ਜੇ(!imapc)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  pp = &imapc->pp;
  PINGPONG_ਤਿਆਰੀ(pp, imap_pp_statemachine, imap_endofresp);

  /* Set the default preferred authentication type and mechanism */
  imapc->preftype = IMAP_TYPE_ANY;
  Curl_sasl_init(&imapc->sasl, ਅੰਕੜੇ, &saslimap);

  Curl_pp_init(pp, Curl_pgrs_now(ਅੰਕੜੇ));

  ਜੇ(Curl_conn_meta_ਸੈੱਟ(conn, CURL_META_IMAP_CONN, imapc, imap_conn_dtor))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  imap = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ IMAP));
  ਜੇ(!imap ||
     Curl_meta_ਸੈੱਟ(ਅੰਕੜੇ, CURL_META_IMAP_EASY, imap, imap_easy_dtor))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਵਾਪਸ CURLE_OK;
}

/*
 * IMAP protocol.
 */
ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_imap = {
  imap_ਤਿਆਰੀ_connection,            /* setup_connection */
  imap_do,                          /* do_it */
  imap_ਪੂਰਾ,                        /* done */
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

#ਜੇ_ਅੰਤ /* CURL_DISABLE_IMAP */
