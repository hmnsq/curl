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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_IMAP

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#ಆದರೆ_ಅಂತ್ಯ

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
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_IMAP_EASY   "meta:proto:imap:easy"
/* meta key for storing protocol meta at connection */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_IMAP_CONN   "meta:proto:imap:conn"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
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
  IMAP_ಕೊನೆಯ          /* never used */
} imapstate;

/* imap_conn is used for struct connection-oriented data */
ರಚನೆ imap_conn {
  ರಚನೆ pingpong pp;
  ರಚನೆ SASL sasl;           /* SASL-related parameters */
  ಅಕ್ಷರ *mailbox;              /* The last selected mailbox */
  imapstate state;            /* Always use imap.c:state() to change state! */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ mb_uidvalidity; /* UIDVALIDITY parsed from select response */
  ಅಕ್ಷರ resptag[5];            /* Response tag to wait for */
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ preftype;     /* Preferred authentication type */
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ cmdid;        /* Last used command ID */
  ಬಿಟ್(ssldone);               /* Is connect() over SSL done? */
  ಬಿಟ್(preauth);               /* Is this connection PREAUTH? */
  ಬಿಟ್(tls_supported);         /* StartTLS capability supported by server */
  ಬಿಟ್(login_disabled);        /* LOGIN command disabled by server */
  ಬಿಟ್(ir_supported);          /* Initial response supported by server */
  ಬಿಟ್(mb_uidvalidity_ಗಣ);
};

/* This IMAP struct is used in the Curl_easy. All IMAP data that is
   connection-oriented must be in imap_conn to properly deal with the fact that
   perhaps the Curl_easy is changed between the times the connection is
   used. */
ರಚನೆ IMAP {
  curl_pp_transfer transfer;
  ಅಕ್ಷರ *mailbox;          /* Mailbox to select */
  ಅಕ್ಷರ *uid;              /* Message UID to fetch */
  ಅಕ್ಷರ *mindex;           /* Index in mail box of mail to fetch */
  ಅಕ್ಷರ *section;          /* Message SECTION to fetch */
  ಅಕ್ಷರ *partial;          /* Message PARTIAL to fetch */
  ಅಕ್ಷರ *query;            /* Query to search for */
  ಅಕ್ಷರ *custom;           /* Custom request */
  ಅಕ್ಷರ *custom_params;    /* Parameters for the custom request */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ uidvalidity; /* UIDVALIDITY to check in select */
  ಬಿಟ್(uidvalidity_ಗಣ);
};

#ವ್ಯಾಖ್ಯಾನಿಸು IMAP_RESP_OK       1
#ವ್ಯಾಖ್ಯಾನಿಸು IMAP_RESP_NOT_OK   2
#ವ್ಯಾಖ್ಯಾನಿಸು IMAP_RESP_PREAUTH  3

ರಚನೆ ulbits {
  ಪೂರ್ಣಾಂಕ ಬಿಟ್_2;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *flag;
};

/***********************************************************************
 *
 * imap_sendf()
 *
 * Sends the formatted string as an IMAP command to the server.
 *
 * Designed to never block.
 */
ಸ್ಥಿರ CURLcode imap_sendf(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ imap_conn *imapc,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) CURL_PRINTF(3, 4);
ಸ್ಥಿರ CURLcode imap_sendf(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ imap_conn *imapc,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  va_list ap;

  DEBUGASSERT(fmt);

  /* Calculate the tag based on the connection ID and command ID */
  curl_msnprintf(imapc->resptag, ಗಾತ್ರ(imapc->resptag), "%c%03d",
                 'A' + curlx_sltosi((ದೀರ್ಘ)(ದತ್ತಾಂಶ->conn->connection_id % 26)),
                 ++imapc->cmdid);

  va_start(ap, fmt);
  ಫಲಿತಾಂಶ = Curl_pp_vsendf(ದತ್ತಾಂಶ, &imapc->pp, imapc->resptag, fmt, ap);
  va_end(ap);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ಸ್ಥಿರ ಅಕ್ಷರ *imap_atom(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, bool escape_only)
{
  ರಚನೆ dynbuf ಸಾಲು;
  size_t nclean;
  size_t len;

  ಆದರೆ(!str)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str);
  nclean = strcspn(str, "() {%*]\\\"");
  ಆದರೆ(len == nclean)
    /* nothing to escape, return a strdup */
    ಹಿಂತಿರುಗಿ curlx_strdup(str);

  curlx_dyn_init(&ಸಾಲು, 2000);

  ಆದರೆ(!escape_only && curlx_dyn_addn(&ಸಾಲು, "\"", 1))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  ಆಗ(*str) {
    ಆದರೆ((*str == '\\' || *str == '"') &&
       curlx_dyn_addn(&ಸಾಲು, "\\", 1))
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ(curlx_dyn_addn(&ಸಾಲು, str, 1))
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    str++;
  }

  ಆದರೆ(!escape_only && curlx_dyn_addn(&ಸಾಲು, "\"", 1))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  ಹಿಂತಿರುಗಿ curlx_dyn_ptr(&ಸಾಲು);
}

/*
 * Finds the start of a literal '{size}' in line, skipping over quoted strings.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *imap_ಕಂಡುಹಿಡಿ_literal(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, size_t len)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ = ಸಾಲು + len;
  bool ಒಳ_quote = FALSE;

  ಆಗ(ಸಾಲು < ಕೊನೆ) {
    ಆದರೆ(ಒಳ_quote) {
      ಆದರೆ(*ಸಾಲು == '\\' && (ಸಾಲು + 1) < ಕೊನೆ) {
        ಸಾಲು += 2;
        ಮುಂದುವರಿ;
      }
      ಆದರೆ(*ಸಾಲು == '"')
        ಒಳ_quote = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(*ಸಾಲು == '"')
        ಒಳ_quote = TRUE;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*ಸಾಲು == '{')
        ಹಿಂತಿರುಗಿ ಸಾಲು;
    }
    ಸಾಲು++;
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
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
ಸ್ಥಿರ bool imap_matchresp(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, size_t len, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cmd)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ = ಸಾಲು + len;
  size_t cmd_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(cmd);

  /* Skip the untagged response marker */
  ಸಾಲು += 2;

  /* Do we have a number after the marker? */
  ಆದರೆ(ಸಾಲು < ಕೊನೆ && ISDIGIT(*ಸಾಲು)) {
    /* Skip the number */
    ಮಾಡು
      ಸಾಲು++;
    ಆಗ(ಸಾಲು < ಕೊನೆ && ISDIGIT(*ಸಾಲು));

    /* Do we have the space character? */
    ಆದರೆ(ಸಾಲು == ಕೊನೆ || *ಸಾಲು != ' ')
      ಹಿಂತಿರುಗಿ FALSE;

    ಸಾಲು++;
  }

  /* Does the command name match and is it followed by a space character or at
     the end of line? */
  ಆದರೆ(ಸಾಲು + cmd_len <= ಕೊನೆ && curl_strnequal(ಸಾಲು, cmd, cmd_len) &&
     (ಸಾಲು[cmd_len] == ' ' || ಸಾಲು + cmd_len + 2 == ಕೊನೆ))
    ಹಿಂತಿರುಗಿ TRUE;

  ಹಿಂತಿರುಗಿ FALSE;
}

/***********************************************************************
 *
 * imap_endofresp()
 *
 * Checks whether the given string is a valid tagged, untagged or continuation
 * response which can be processed by the response handler.
 */
ಸ್ಥಿರ bool imap_endofresp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ connectdata *conn,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, size_t len, ಪೂರ್ಣಾಂಕ *resp)
{
  ರಚನೆ imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ರಚನೆ IMAP *imap = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_IMAP_EASY);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *id;
  size_t id_len;

  DEBUGASSERT(imapc);
  DEBUGASSERT(imap);
  ಆದರೆ(!imapc || !imap)
    ಹಿಂತಿರುಗಿ FALSE;

  /* Do we have a tagged command response? */
  id = imapc->resptag;
  id_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(id);
  ಆದರೆ(len >= id_len + 1 && !memcmp(id, ಸಾಲು, id_len) && ಸಾಲು[id_len] == ' ') {
    ಸಾಲು += id_len + 1;
    len -= id_len + 1;

    ಆದರೆ(len >= 2 && !memcmp(ಸಾಲು, "OK", 2))
      *resp = IMAP_RESP_OK;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(len >= 7 && !memcmp(ಸಾಲು, "PREAUTH", 7))
      *resp = IMAP_RESP_PREAUTH;
    ಇಲ್ಲದಿದ್ದರೆ
      *resp = IMAP_RESP_NOT_OK;

    ಹಿಂತಿರುಗಿ TRUE;
  }

  /* Do we have an untagged command response? */
  ಆದರೆ(len >= 2 && !memcmp("* ", ಸಾಲು, 2)) {
    ಆಯ್ಕೆ(imapc->state) {
    /* States which are interested in untagged responses */
    ಸ್ಥಿತಿ IMAP_CAPABILITY:
      ಆದರೆ(!imap_matchresp(ಸಾಲು, len, "CAPABILITY"))
        ಹಿಂತಿರುಗಿ FALSE;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ IMAP_LIST:
      ಆದರೆ((!imap->custom && !imap_matchresp(ಸಾಲು, len, "LIST")) ||
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
        ಹಿಂತಿರುಗಿ FALSE;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ IMAP_SELECT:
      /* SELECT is special in that its untagged responses do not have a
         common prefix so accept anything! */
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ IMAP_FETCH:
      ಆದರೆ(!imap_matchresp(ಸಾಲು, len, "FETCH"))
        ಹಿಂತಿರುಗಿ FALSE;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ IMAP_ಹುಡುಕು:
      ಆದರೆ(!imap_matchresp(ಸಾಲು, len, "SEARCH"))
        ಹಿಂತಿರುಗಿ FALSE;
      ನಿಲ್ಲಿಸು;

    /* Ignore other untagged responses */
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಹಿಂತಿರುಗಿ FALSE;
    }

    *resp = '*';
    ಹಿಂತಿರುಗಿ TRUE;
  }

  /* Do we have a continuation response? This should be a + symbol followed by
     a space and optionally some text as per RFC-3501 for the AUTHENTICATE and
     APPEND commands and as outlined in Section 4. Examples of RFC-4959 but
     some email servers ignore this and only send a single + instead. */
  ಆದರೆ(!imap->custom && ((len == 3 && ಸಾಲು[0] == '+') ||
                       (len >= 2 && !memcmp("+ ", ಸಾಲು, 2)))) {
    ಆಯ್ಕೆ(imapc->state) {
    /* States which are interested in continuation responses */
    ಸ್ಥಿತಿ IMAP_AUTHENTICATE:
    ಸ್ಥಿತಿ IMAP_APPEND:
      *resp = '+';
      ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
      failf(ದತ್ತಾಂಶ, "Unexpected continuation response");
      *resp = -1;
      ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ TRUE;
  }

  ಹಿಂತಿರುಗಿ FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * imap_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ಸ್ಥಿರ CURLcode imap_get_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ bufref *ಹೊರಹೋಗು)
{
  ರಚನೆ imap_conn *imapc =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_IMAP_CONN);
  ಅಕ್ಷರ *ಸಂದೇಶ_2;
  size_t len;

  ಆದರೆ(!imapc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಸಂದೇಶ_2 = curlx_dyn_ptr(&imapc->pp.recvbuf);
  len = imapc->pp.nfinal;
  ಆದರೆ(len > 2) {
    /* Find the start of the message */
    len -= 2;
    ಗಾಗಿ(ಸಂದೇಶ_2 += 2; ISBLANK(*ಸಂದೇಶ_2); ಸಂದೇಶ_2++, len--)
      ;

    /* Find the end of the message */
    ಆಗ(len--)
      ಆದರೆ(!ISNEWLINE(ಸಂದೇಶ_2[len]) && !ISBLANK(ಸಂದೇಶ_2[len]))
        ನಿಲ್ಲಿಸು;

    /* Terminate the message */
    ಸಂದೇಶ_2[++len] = '\0';
    Curl_bufref_ಗಣ(ಹೊರಹೋಗು, ಸಂದೇಶ_2, len, ಖಾಲಿ);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    /* junk input => zero length output */
    Curl_bufref_ಗಣ(ಹೊರಹೋಗು, "", 0, ಖಾಲಿ);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/***********************************************************************
 *
 * imap_state()
 *
 * This is the ONLY way to change IMAP state!
 */
ಸ್ಥಿರ ಶೂನ್ಯ imap_state(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                       ರಚನೆ imap_conn *imapc,
                       imapstate newstate)
{
#ಆದರೆ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ಹೆಸರುಗಳು[] = {
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
    /*ಕೊನೆಯ*/
  };

  ಆದರೆ(imapc->state != newstate)
    infof(ದತ್ತಾಂಶ, "IMAP %p state change from %s to %s",
          (ಶೂನ್ಯ *)imapc, ಹೆಸರುಗಳು[imapc->state], ಹೆಸರುಗಳು[newstate]);
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ
  imapc->state = newstate;
}

/***********************************************************************
 *
 * imap_perform_capability()
 *
 * Sends the CAPABILITY command in order to obtain a list of server side
 * supported capabilities.
 */
ಸ್ಥಿರ CURLcode imap_perform_capability(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ imap_conn *imapc)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  imapc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  imapc->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  imapc->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPABILITY command */
  ಫಲಿತಾಂಶ = imap_sendf(ದತ್ತಾಂಶ, imapc, "CAPABILITY");

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    imap_state(ದತ್ತಾಂಶ, imapc, IMAP_CAPABILITY);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * imap_perform_starttls()
 *
 * Sends the STARTTLS command to start the upgrade to TLS.
 */
ಸ್ಥಿರ CURLcode imap_perform_starttls(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ imap_conn *imapc)
{
  /* Send the STARTTLS command */
  CURLcode ಫಲಿತಾಂಶ = imap_sendf(ದತ್ತಾಂಶ, imapc, "STARTTLS");

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    imap_state(ದತ್ತಾಂಶ, imapc, IMAP_STARTTLS);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * imap_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ಸ್ಥಿರ CURLcode imap_perform_upgrade_tls(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ imap_conn *imapc,
                                         ರಚನೆ connectdata *conn)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  /* Start the SSL connection */
  CURLcode ಫಲಿತಾಂಶ;
  bool ssldone = FALSE;

  ಆದರೆ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    ಫಲಿತಾಂಶ = Curl_ssl_cfilter_ಕೂಡಿಸು(
      ದತ್ತಾಂಶ, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_imaps;
  }

  DEBUGASSERT(!imapc->ssldone);
  ಫಲಿತಾಂಶ = Curl_conn_connect(ದತ್ತಾಂಶ, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(ದತ್ತಾಂಶ, "imap_perform_upgrade_tls, connect -> %d, %d",
               (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, ssldone));
  ಆದರೆ(!ಫಲಿತಾಂಶ && ssldone) {
    imapc->ssldone = ssldone;
    /* perform CAPA now, changes imapc->state out of IMAP_UPGRADETLS */
    ಫಲಿತಾಂಶ = imap_perform_capability(ದತ್ತಾಂಶ, imapc);
  }
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)imapc;
  (ಶೂನ್ಯ)conn;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
}

/***********************************************************************
 *
 * imap_perform_login()
 *
 * Sends a clear text LOGIN command to authenticate with.
 */
ಸ್ಥಿರ CURLcode imap_perform_login(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ imap_conn *imapc,
                                   ರಚನೆ connectdata *conn)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಅಕ್ಷರ *user;
  ಅಕ್ಷರ *passwd;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  ಆದರೆ(!conn->creds) {
    imap_state(ದತ್ತಾಂಶ, imapc, IMAP_STOP);

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* Make sure the username and password are in the correct atom format */
  user = imap_atom(Curl_creds_user(conn->creds), FALSE);
  passwd = imap_atom(Curl_creds_passwd(conn->creds), FALSE);

  /* Send the LOGIN command */
  ಫಲಿತಾಂಶ = imap_sendf(ದತ್ತಾಂಶ, imapc, "LOGIN %s %s", user ? user : "",
                      passwd ? passwd : "");

  curlx_free(user);
  curlx_strzero(passwd);
  curlx_free(passwd);

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    imap_state(ದತ್ತಾಂಶ, imapc, IMAP_LOGIN);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * imap_perform_authenticate()
 *
 * Sends an AUTHENTICATE command allowing the client to login with the given
 * SASL authentication mechanism.
 */
ಸ್ಥಿರ CURLcode imap_perform_authenticate(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mech,
                                          ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *initresp)
{
  ರಚನೆ imap_conn *imapc =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_IMAP_CONN);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ir = Curl_bufref_ptr(initresp);

  ಆದರೆ(!imapc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಆದರೆ(ir) {
    /* Send the AUTHENTICATE command with the initial response */
    ಫಲಿತಾಂಶ = imap_sendf(ದತ್ತಾಂಶ, imapc, "AUTHENTICATE %s %s",
                        mech, *ir ? ir : "=");
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Send the AUTHENTICATE command */
    ಫಲಿತಾಂಶ = imap_sendf(ದತ್ತಾಂಶ, imapc, "AUTHENTICATE %s", mech);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * imap_continue_authenticate()
 *
 * Sends SASL continuation data.
 */
ಸ್ಥಿರ CURLcode imap_continue_authenticate(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mech,
                                           ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *resp)
{
  ರಚನೆ imap_conn *imapc =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_IMAP_CONN);

  (ಶೂನ್ಯ)mech;
  ಆದರೆ(!imapc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಹಿಂತಿರುಗಿ Curl_pp_sendf(ದತ್ತಾಂಶ, &imapc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * imap_cancel_authenticate()
 *
 * Sends SASL cancellation.
 */
ಸ್ಥಿರ CURLcode imap_cancel_authenticate(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mech)
{
  ರಚನೆ imap_conn *imapc =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_IMAP_CONN);

  (ಶೂನ್ಯ)mech;
  ಆದರೆ(!imapc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಹಿಂತಿರುಗಿ Curl_pp_sendf(ದತ್ತಾಂಶ, &imapc->pp, "*");
}

/***********************************************************************
 *
 * imap_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to clear text should a common
 * mechanism not be available between the client and server.
 */
ಸ್ಥಿರ CURLcode imap_perform_authentication(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                            ರಚನೆ imap_conn *imapc)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  saslprogress ಪ್ರಗತಿ;

  /* Check if already authenticated OR if there is enough data to authenticate
     with and end the connect phase if we do not */
  ಆದರೆ(imapc->preauth ||
     !Curl_sasl_can_authenticate(&imapc->sasl, ದತ್ತಾಂಶ)) {
    imap_state(ದತ್ತಾಂಶ, imapc, IMAP_STOP);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* Calculate the SASL login details */
  ಫಲಿತಾಂಶ = Curl_sasl_ಆರಂಭ(&imapc->sasl, ದತ್ತಾಂಶ, (bool)imapc->ir_supported,
                           &ಪ್ರಗತಿ);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಪ್ರಗತಿ == SASL_INPROGRESS)
      imap_state(ದತ್ತಾಂಶ, imapc, IMAP_AUTHENTICATE);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
      /* Perform clear text authentication */
      ಫಲಿತಾಂಶ = imap_perform_login(ದತ್ತಾಂಶ, imapc, ದತ್ತಾಂಶ->conn);
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = Curl_sasl_is_blocked(&imapc->sasl, ದತ್ತಾಂಶ);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * imap_perform_list()
 *
 * Sends a LIST command or an alternative custom request.
 */
ಸ್ಥಿರ CURLcode imap_perform_list(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ imap_conn *imapc,
                                  ರಚನೆ IMAP *imap)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(imap->custom)
    /* Send the custom request */
    ಫಲಿತಾಂಶ = imap_sendf(ದತ್ತಾಂಶ, imapc, "%s%s", imap->custom,
                        imap->custom_params ? imap->custom_params : "");
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Make sure the mailbox is in the correct atom format if necessary */
    ಅಕ್ಷರ *mailbox = imap->mailbox ? imap_atom(imap->mailbox, TRUE)
                                  : curlx_strdup("");
    ಆದರೆ(!mailbox)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    /* Send the LIST command */
    ಫಲಿತಾಂಶ = imap_sendf(ದತ್ತಾಂಶ, imapc, "LIST \"%s\" *", mailbox);

    curlx_free(mailbox);
  }

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    imap_state(ದತ್ತಾಂಶ, imapc, IMAP_LIST);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * imap_perform_select()
 *
 * Sends a SELECT command to ask the server to change the selected mailbox.
 */
ಸ್ಥಿರ CURLcode imap_perform_select(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ imap_conn *imapc,
                                    ರಚನೆ IMAP *imap)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಅಕ್ಷರ *mailbox;

  /* Invalidate old information as we are switching mailboxes */
  curlx_safefree(imapc->mailbox);
  imapc->mb_uidvalidity_ಗಣ = FALSE;

  /* Check we have a mailbox */
  ಆದರೆ(!imap->mailbox) {
    failf(ದತ್ತಾಂಶ, "Cannot SELECT without a mailbox.");
    ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  ಆದರೆ(!mailbox)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* Send the SELECT command */
  ಫಲಿತಾಂಶ = imap_sendf(ದತ್ತಾಂಶ, imapc, "SELECT %s", mailbox);

  curlx_free(mailbox);

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    imap_state(ದತ್ತಾಂಶ, imapc, IMAP_SELECT);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * imap_perform_fetch()
 *
 * Sends a FETCH command to initiate the download of a message.
 */
ಸ್ಥಿರ CURLcode imap_perform_fetch(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ imap_conn *imapc,
                                   ರಚನೆ IMAP *imap)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  /* Check we have a UID */
  ಆದರೆ(imap->uid) {

    /* Send the FETCH command */
    ಆದರೆ(imap->partial)
      ಫಲಿತಾಂಶ = imap_sendf(ದತ್ತಾಂಶ, imapc, "UID FETCH %s BODY[%s]<%s>",
                          imap->uid, imap->section ? imap->section : "",
                          imap->partial);
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = imap_sendf(ದತ್ತಾಂಶ, imapc, "UID FETCH %s BODY[%s]",
                          imap->uid, imap->section ? imap->section : "");
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(imap->mindex) {
    /* Send the FETCH command */
    ಆದರೆ(imap->partial)
      ಫಲಿತಾಂಶ = imap_sendf(ದತ್ತಾಂಶ, imapc, "FETCH %s BODY[%s]<%s>",
                          imap->mindex, imap->section ? imap->section : "",
                          imap->partial);
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = imap_sendf(ದತ್ತಾಂಶ, imapc, "FETCH %s BODY[%s]",
                          imap->mindex, imap->section ? imap->section : "");
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    failf(ದತ್ತಾಂಶ, "Cannot FETCH without a UID.");
    ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    imap_state(ದತ್ತಾಂಶ, imapc, IMAP_FETCH);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * imap_perform_append()
 *
 * Sends an APPEND command to initiate the upload of a message.
 */
ಸ್ಥಿರ CURLcode imap_perform_append(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ imap_conn *imapc,
                                    ರಚನೆ IMAP *imap)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಅಕ್ಷರ *mailbox;
  ರಚನೆ dynbuf flags;

  /* Check we have a mailbox */
  ಆದರೆ(!imap->mailbox) {
    failf(ದತ್ತಾಂಶ, "Cannot APPEND without a mailbox.");
    ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  ಆದರೆ(IS_MIME_POST(ದತ್ತಾಂಶ)) {
    curl_mimepart *postp = ದತ್ತಾಂಶ->ಗಣ.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_ಶೀರ್ಷಿಕೆಗಳು(postp, ದತ್ತಾಂಶ->ಗಣ.ಶೀರ್ಷಿಕೆಗಳು, 0);
    ಫಲಿತಾಂಶ = Curl_mime_prepare_ಶೀರ್ಷಿಕೆಗಳು(ದತ್ತಾಂಶ, postp, ಖಾಲಿ,
                                       ಖಾಲಿ, MIMESTRATEGY_MAIL);

    ಆದರೆ(!ಫಲಿತಾಂಶ && !Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Mime-Version")))
      ಫಲಿತಾಂಶ = Curl_mime_ಕೂಡಿಸು_ಶೀರ್ಷಿಕೆ(&postp->curlheaders, "Mime-Version: 1.0");
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_mime(ದತ್ತಾಂಶ, postp);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
  {
    ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_fread(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.infilesize);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* Check we know the size of the upload. This takes all readers
   * into account. Especially crlf conversions which make the size
   * unpredictable, e.g. -1. */
  ದತ್ತಾಂಶ->state.infilesize = Curl_creader_ಒಟ್ಟು_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ);
  ಆದರೆ(ದತ್ತಾಂಶ->state.infilesize < 0) {
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.crlf)
      failf(ದತ್ತಾಂಶ, "Cannot APPEND with CRLF conversion making size unknown");
    ಇಲ್ಲದಿದ್ದರೆ
      failf(ದತ್ತಾಂಶ, "Cannot APPEND with unknown input file size");
    ಹಿಂತಿರುಗಿ CURLE_UPLOAD_FAILED;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  ಆದರೆ(!mailbox)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* Generate flags string and send the APPEND command */
  curlx_dyn_init(&flags, 100);
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.upload_flags) {
    ಪೂರ್ಣಾಂಕ i;
    ರಚನೆ ulbits ulflag[] = {
      { CURLULFLAG_ANSWERED, "Answered" },
      { CURLULFLAG_DELETED, "Deleted" },
      { CURLULFLAG_DRAFT, "Draft" },
      { CURLULFLAG_FLAGGED, "Flagged" },
      { CURLULFLAG_ನೋಡಿದ, "Seen" },
      { 0, ಖಾಲಿ }
    };

    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಆದರೆ(curlx_dyn_ಕೂಡಿಸು(&flags, " (")) {
      ಹೋಗು cleanup;
    }

    ಗಾಗಿ(i = 0; ulflag[i].ಬಿಟ್_2; i++) {
      ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.upload_flags & ulflag[i].ಬಿಟ್_2 &&
         ((curlx_dyn_len(&flags) > 2 && curlx_dyn_ಕೂಡಿಸು(&flags, " ")) ||
          curlx_dyn_ಕೂಡಿಸು(&flags, "\\") ||
          curlx_dyn_ಕೂಡಿಸು(&flags, ulflag[i].flag)))
        ಹೋಗು cleanup;
    }

    ಆದರೆ(curlx_dyn_ಕೂಡಿಸು(&flags, ")"))
      ಹೋಗು cleanup;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_dyn_ಕೂಡಿಸು(&flags, ""))
    ಹೋಗು cleanup;

  ಫಲಿತಾಂಶ = imap_sendf(ದತ್ತಾಂಶ, imapc, "APPEND %s%s {%" FMT_OFF_T "}",
                      mailbox, curlx_dyn_ptr(&flags), ದತ್ತಾಂಶ->state.infilesize);

cleanup:
  curlx_dyn_free(&flags);
  curlx_free(mailbox);

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    imap_state(ದತ್ತಾಂಶ, imapc, IMAP_APPEND);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * imap_perform_search()
 *
 * Sends a SEARCH command.
 */
ಸ್ಥಿರ CURLcode imap_perform_ಹುಡುಕು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ imap_conn *imapc,
                                    ರಚನೆ IMAP *imap)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  /* Check we have a query string */
  ಆದರೆ(!imap->query) {
    failf(ದತ್ತಾಂಶ, "Cannot SEARCH without a query string.");
    ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
  }

  /* Send the SEARCH command */
  ಫಲಿತಾಂಶ = imap_sendf(ದತ್ತಾಂಶ, imapc, "SEARCH %s", imap->query);

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    imap_state(ದತ್ತಾಂಶ, imapc, IMAP_ಹುಡುಕು);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * imap_perform_logout()
 *
 * Performs the logout action prior to sclose() being called.
 */
ಸ್ಥಿರ CURLcode imap_perform_logout(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ imap_conn *imapc)
{
  /* Send the LOGOUT command */
  CURLcode ಫಲಿತಾಂಶ = imap_sendf(ದತ್ತಾಂಶ, imapc, "LOGOUT");

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    imap_state(ದತ್ತಾಂಶ, imapc, IMAP_LOGOUT);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For the initial server greeting */
ಸ್ಥಿರ CURLcode imap_state_servergreet_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                            ರಚನೆ imap_conn *imapc,
                                            ಪೂರ್ಣಾಂಕ imapcode,
                                            imapstate instate)
{
  (ಶೂನ್ಯ)instate;

  ಆದರೆ(imapcode == IMAP_RESP_PREAUTH) {
    /* PREAUTH */
    imapc->preauth = TRUE;
    infof(ದತ್ತಾಂಶ, "PREAUTH connection, already authenticated");
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(imapcode != IMAP_RESP_OK) {
    failf(ದತ್ತಾಂಶ, "Got unexpected imap-server response");
    ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
  }

  ಹಿಂತಿರುಗಿ imap_perform_capability(ದತ್ತಾಂಶ, imapc);
}

/* For CAPABILITY responses */
ಸ್ಥಿರ CURLcode imap_state_capability_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ರಚನೆ imap_conn *imapc,
                                           ಪೂರ್ಣಾಂಕ imapcode,
                                           imapstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು = curlx_dyn_ptr(&imapc->pp.recvbuf);

  (ಶೂನ್ಯ)instate;

  /* Do we have an untagged response? */
  ಆದರೆ(imapcode == '*') {
    ಸಾಲು += 2;

    /* Loop through the data line */
    ಗಾಗಿ(;;) {
      size_t wordlen;
      ಆಗ(*ಸಾಲು && (ISBLANK(*ಸಾಲು) || ISNEWLINE(*ಸಾಲು)))
        ಸಾಲು++;

      ಆದರೆ(!*ಸಾಲು)
        ನಿಲ್ಲಿಸು;

      /* Extract the word */
      ಗಾಗಿ(wordlen = 0; ಸಾಲು[wordlen] && !ISBLANK(ಸಾಲು[wordlen]) &&
                       !ISNEWLINE(ಸಾಲು[wordlen]);)
        wordlen++;

      /* Does the server support the STARTTLS capability? */
      ಆದರೆ(wordlen == 8 && curl_strnequal(ಸಾಲು, "STARTTLS", 8))
        imapc->tls_supported = TRUE;

      /* Has the server explicitly disabled clear text authentication? */
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(wordlen == 13 && curl_strnequal(ಸಾಲು, "LOGINDISABLED", 13))
        imapc->login_disabled = TRUE;

      /* Does the server support the SASL-IR capability? */
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(wordlen == 7 && curl_strnequal(ಸಾಲು, "SASL-IR", 7))
        imapc->ir_supported = TRUE;

      /* Do we have a SASL based authentication mechanism? */
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(wordlen > 5 && curl_strnequal(ಸಾಲು, "AUTH=", 5)) {
        size_t llen;
        ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ mechbit;

        ಸಾಲು += 5;
        wordlen -= 5;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(ಸಾಲು, wordlen, &llen);
        ಆದರೆ(mechbit && llen == wordlen)
          imapc->sasl.authmechs |= mechbit;
      }

      ಸಾಲು += wordlen;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.use_ssl && !Curl_conn_is_ssl(ದತ್ತಾಂಶ->conn, FIRSTSOCKET)) {
    /* PREAUTH is not compatible with STARTTLS. */
    ಆದರೆ(imapcode == IMAP_RESP_OK && imapc->tls_supported && !imapc->preauth) {
      /* Switch to TLS connection now */
      ಫಲಿತಾಂಶ = imap_perform_starttls(ದತ್ತಾಂಶ, imapc);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.use_ssl <= CURLUSESSL_TRY)
      ಫಲಿತಾಂಶ = imap_perform_authentication(ದತ್ತಾಂಶ, imapc);
    ಇಲ್ಲದಿದ್ದರೆ {
      failf(ದತ್ತಾಂಶ, "STARTTLS not available.");
      ಫಲಿತಾಂಶ = CURLE_USE_SSL_FAILED;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = imap_perform_authentication(ದತ್ತಾಂಶ, imapc);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For STARTTLS responses */
ಸ್ಥಿರ CURLcode imap_state_starttls_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ imap_conn *imapc,
                                         ಪೂರ್ಣಾಂಕ imapcode,
                                         imapstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  (ಶೂನ್ಯ)instate;

  /* Pipelining in response is forbidden. */
  ಆದರೆ(imapc->pp.ಅತಿಪ್ರವಾಹ)
    ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;

  ಆದರೆ(imapcode != IMAP_RESP_OK) {
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.use_ssl != CURLUSESSL_TRY) {
      failf(ದತ್ತಾಂಶ, "STARTTLS denied");
      ಫಲಿತಾಂಶ = CURLE_USE_SSL_FAILED;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = imap_perform_authentication(ದತ್ತಾಂಶ, imapc);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    imap_state(ದತ್ತಾಂಶ, imapc, IMAP_UPGRADETLS);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For SASL authentication responses */
ಸ್ಥಿರ CURLcode imap_state_auth_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ imap_conn *imapc,
                                     ಪೂರ್ಣಾಂಕ imapcode,
                                     imapstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  saslprogress ಪ್ರಗತಿ;

  (ಶೂನ್ಯ)instate;

  ಫಲಿತಾಂಶ = Curl_sasl_continue(&imapc->sasl, ದತ್ತಾಂಶ, imapcode, &ಪ್ರಗತಿ);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಆಯ್ಕೆ(ಪ್ರಗತಿ) {
    ಸ್ಥಿತಿ SASL_ಮುಗಿದ:
      imap_state(ದತ್ತಾಂಶ, imapc, IMAP_STOP);  /* Authenticated */
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SASL_IDLE:            /* No mechanism left after cancellation */
      ಆದರೆ(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
        /* Perform clear text authentication */
        ಫಲಿತಾಂಶ = imap_perform_login(ದತ್ತಾಂಶ, imapc, ದತ್ತಾಂಶ->conn);
      ಇಲ್ಲದಿದ್ದರೆ {
        failf(ದತ್ತಾಂಶ, "Authentication cancelled");
        ಫಲಿತಾಂಶ = CURLE_LOGIN_DENIED;
      }
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ನಿಲ್ಲಿಸು;
    }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For LOGIN responses */
ಸ್ಥಿರ CURLcode imap_state_login_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ imap_conn *imapc,
                                      ಪೂರ್ಣಾಂಕ imapcode,
                                      imapstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  ಆದರೆ(imapcode != IMAP_RESP_OK) {
    failf(ದತ್ತಾಂಶ, "Access denied. %c", imapcode);
    ಫಲಿತಾಂಶ = CURLE_LOGIN_DENIED;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    /* End of connect phase */
    imap_state(ದತ್ತಾಂಶ, imapc, IMAP_STOP);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Detect IMAP listings vs. downloading a single email */
ಸ್ಥಿರ bool is_custom_fetch_listing_ಹೊಂದಿಕೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *params)
{
  /* match " 1:* (FLAGS ..." or " 1,2,3 (FLAGS ..." */
  ಆದರೆ(*params++ != ' ')
    ಹಿಂತಿರುಗಿ FALSE;

  ಆಗ(ISDIGIT(*params)) {
    params++;
    ಆದರೆ(*params == 0)
      ಹಿಂತಿರುಗಿ FALSE;
  }
  ಆದರೆ(*params == ':')
    ಹಿಂತಿರುಗಿ TRUE;
  ಆದರೆ(*params == ',')
    ಹಿಂತಿರುಗಿ TRUE;
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ bool is_custom_fetch_listing(ರಚನೆ IMAP *imap)
{
  /* filter out "UID FETCH 1:* (FLAGS ..." queries to list emails */
  ಆದರೆ(!imap->custom)
    ಹಿಂತಿರುಗಿ FALSE;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal(imap->custom, "FETCH") && imap->custom_params) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = imap->custom_params;
    ಹಿಂತಿರುಗಿ is_custom_fetch_listing_ಹೊಂದಿಕೆ(p);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal(imap->custom, "UID") && imap->custom_params &&
          curl_strnequal(imap->custom_params, " FETCH ", 7)) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = imap->custom_params + 6;
    ಹಿಂತಿರುಗಿ is_custom_fetch_listing_ಹೊಂದಿಕೆ(p);
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

/* For LIST and SEARCH responses */
ಸ್ಥಿರ CURLcode imap_state_listsearch_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ರಚನೆ imap_conn *imapc,
                                           ಪೂರ್ಣಾಂಕ imapcode,
                                           imapstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  ರಚನೆ IMAP *imap = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_IMAP_EASY);

  DEBUGASSERT(imap);
  ಆದರೆ(!imap)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  (ಶೂನ್ಯ)instate;

  ಆದರೆ(imapcode == '*' && is_custom_fetch_listing(imap)) {
    /* custom FETCH or UID FETCH for listing is not handled here */
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(imapcode == '*') {
    /* Check if this response contains a literal (e.g. FETCH responses with
       body data). Literal syntax is {size}\r\n */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cr = memchr(ಸಾಲು, '\r', len);
    size_t ಸಾಲು_len = cr ? (size_t)(cr - ಸಾಲು) : len;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr = imap_ಕಂಡುಹಿಡಿ_literal(ಸಾಲು, ಸಾಲು_len);
    ಆದರೆ(ptr) {
      curl_off_t ಗಾತ್ರ_2 = 0;
      bool parsed = FALSE;
      ptr++;
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&ptr, &ಗಾತ್ರ_2, CURL_OFF_T_MAX) &&
         !curlx_str_single(&ptr, '}'))
        parsed = TRUE;

      ಆದರೆ(parsed) {
        ರಚನೆ pingpong *pp = &imapc->pp;
        size_t ಬಫರ್_len = curlx_dyn_len(&pp->recvbuf);
        size_t after_ಶೀರ್ಷಿಕೆ = ಬಫರ್_len - pp->nfinal;

        /* This is a literal response, setup to receive the body data */
        infof(ದತ್ತಾಂಶ, "Found %" FMT_OFF_T " bytes to download", ಗಾತ್ರ_2);

        /* First write the header line */
        ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, ಸಾಲು, len);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

        /* Handle data already in buffer after the header line */
        ಆದರೆ(after_ಶೀರ್ಷಿಕೆ > 0) {
          /* There is already data in the buffer that is part of the literal
             body or subsequent responses */
          size_t chunk = after_ಶೀರ್ಷಿಕೆ;

          /* Keep only the data after the header line */
          curlx_dyn_ಬಾಲ(&pp->recvbuf, chunk);
          pp->nfinal = 0; /* done */

          /* Limit chunk to the literal size */
          ಆದರೆ((curl_off_t)chunk > ಗಾತ್ರ_2)
            chunk = (size_t)ಗಾತ್ರ_2;

          ಆದರೆ(chunk) {
            /* Write the literal body data */
            ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY,
                                       curlx_dyn_ptr(&pp->recvbuf), chunk);
            ಆದರೆ(ಫಲಿತಾಂಶ)
              ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
          }

          /* Handle remaining data in buffer (either more literal data or
             subsequent responses) */
          ಆದರೆ(after_ಶೀರ್ಷಿಕೆ > chunk) {
            /* Keep the data after the literal body */
            pp->ಅತಿಪ್ರವಾಹ = after_ಶೀರ್ಷಿಕೆ - chunk;
            curlx_dyn_ಬಾಲ(&pp->recvbuf, pp->ಅತಿಪ್ರವಾಹ);
          }
          ಇಲ್ಲದಿದ್ದರೆ {
            pp->ಅತಿಪ್ರವಾಹ = 0;
            curlx_dyn_reset(&pp->recvbuf);
          }
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          /* No data in buffer yet, reset overflow */
          pp->ಅತಿಪ್ರವಾಹ = 0;
        }

        ಆದರೆ((CURL_OFF_T_MAX - ಗಾತ್ರ_2) < (curl_off_t)len)
          /* unlikely to actually be a transfer this big, but avoid integer
ಅತಿಪ್ರವಾಹ*/
          ಗಾತ್ರ_2 = CURL_OFF_T_MAX;
        ಇಲ್ಲದಿದ್ದರೆ
          ಗಾತ್ರ_2 += len;

        /* Progress size includes both header line and literal body */
        Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, ಗಾತ್ರ_2);

        ಆದರೆ(ದತ್ತಾಂಶ->req.bytecount == ಗಾತ್ರ_2)
          /* All data already transferred (header + literal body) */
          Curl_xfer_ಸಿದ್ಧತೆ_nop(ದತ್ತಾಂಶ);
        ಇಲ್ಲದಿದ್ದರೆ {
          /* Setup to receive the literal body data.
             maxdownload and transfer size include both header line and
             literal body */
          ದತ್ತಾಂಶ->req.maxdownload = ಗಾತ್ರ_2;
          Curl_xfer_ಸಿದ್ಧತೆ_recv(ದತ್ತಾಂಶ, FIRSTSOCKET, ಗಾತ್ರ_2);
        }
        /* End of DO phase */
        imap_state(ದತ್ತಾಂಶ, imapc, IMAP_STOP);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* Failed to parse literal, write the line */
        ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, ಸಾಲು, len);
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* No literal, write the line as-is */
      ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, ಸಾಲು, len);
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(imapcode != IMAP_RESP_OK)
    ಫಲಿತಾಂಶ = CURLE_QUOTE_ERROR;
  ಇಲ್ಲದಿದ್ದರೆ
    /* End of DO phase */
    imap_state(ದತ್ತಾಂಶ, imapc, IMAP_STOP);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For SELECT responses */
ಸ್ಥಿರ CURLcode imap_state_select_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ imap_conn *imapc,
                                       ರಚನೆ IMAP *imap,
                                       ಪೂರ್ಣಾಂಕ imapcode,
                                       imapstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  ಆದರೆ(imapcode == '*') {
    /* See if this is an UIDVALIDITY response */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು = curlx_dyn_ptr(&imapc->pp.recvbuf);
    size_t len = curlx_dyn_len(&imapc->pp.recvbuf);
    ಆದರೆ((len >= 18) && checkprefix("OK [UIDVALIDITY ", &ಸಾಲು[2])) {
      curl_off_t ಮೌಲ್ಯ;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = &ಸಾಲು[2] + CURL_CSTRLEN("OK [UIDVALIDITY ");
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &ಮೌಲ್ಯ, UINT_MAX)) {
        imapc->mb_uidvalidity = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಮೌಲ್ಯ;
        imapc->mb_uidvalidity_ಗಣ = TRUE;
      }
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(imapcode == IMAP_RESP_OK) {
    /* Check if the UIDVALIDITY has been specified and matches */
    ಆದರೆ(imap->uidvalidity_ಗಣ && imapc->mb_uidvalidity_ಗಣ &&
       (imap->uidvalidity != imapc->mb_uidvalidity)) {
      failf(ದತ್ತಾಂಶ, "Mailbox UIDVALIDITY has changed");
      ಫಲಿತಾಂಶ = CURLE_REMOTE_FILE_NOT_ಸಿಕ್ಕಿತು;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* Note the currently opened mailbox on this connection */
      DEBUGASSERT(!imapc->mailbox);
      imapc->mailbox = curlx_strdup(imap->mailbox);
      ಆದರೆ(!imapc->mailbox)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

      ಆದರೆ(imap->custom)
        ಫಲಿತಾಂಶ = imap_perform_list(ದತ್ತಾಂಶ, imapc, imap);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(imap->query)
        ಫಲಿತಾಂಶ = imap_perform_ಹುಡುಕು(ದತ್ತಾಂಶ, imapc, imap);
      ಇಲ್ಲದಿದ್ದರೆ
        ಫಲಿತಾಂಶ = imap_perform_fetch(ದತ್ತಾಂಶ, imapc, imap);
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    failf(ದತ್ತಾಂಶ, "Select failed");
    ಫಲಿತಾಂಶ = CURLE_LOGIN_DENIED;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For the (first line of the) FETCH responses */
ಸ್ಥಿರ CURLcode imap_state_fetch_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ imap_conn *imapc,
                                      ಪೂರ್ಣಾಂಕ imapcode,
                                      imapstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ pingpong *pp = &imapc->pp;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  bool parsed = FALSE;
  curl_off_t ಗಾತ್ರ_2 = 0;

  (ಶೂನ್ಯ)instate;

  ಆದರೆ(imapcode != '*') {
    Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, -1);
    imap_state(ದತ್ತಾಂಶ, imapc, IMAP_STOP);
    ಹಿಂತಿರುಗಿ CURLE_REMOTE_FILE_NOT_ಸಿಕ್ಕಿತು;
  }

  /* Something like this is received "* 1 FETCH (BODY[TEXT] {2021}\r" so parse
     the continuation data contained within the curly brackets */
  ptr = imap_ಕಂಡುಹಿಡಿ_literal(ptr, len);
  ಆದರೆ(ptr) {
    ptr++;
    ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&ptr, &ಗಾತ್ರ_2, CURL_OFF_T_MAX) &&
       !curlx_str_single(&ptr, '}'))
      parsed = TRUE;
  }

  ಆದರೆ(parsed) {
    infof(ದತ್ತಾಂಶ, "Found %" FMT_OFF_T " bytes to download", ಗಾತ್ರ_2);
    Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, ಗಾತ್ರ_2);

    ಆದರೆ(pp->ಅತಿಪ್ರವಾಹ) {
      /* At this point there is a data in the receive buffer that is body
         content, send it as body and then skip it. Do note that there may
         even be additional "headers" after the body. */
      size_t chunk = pp->ಅತಿಪ್ರವಾಹ;

      /* keep only the overflow */
      curlx_dyn_ಬಾಲ(&pp->recvbuf, chunk);
      pp->nfinal = 0; /* done */

      ಆದರೆ((curl_off_t)chunk > ಗಾತ್ರ_2)
        /* The conversion from curl_off_t to size_t is always fine here */
        chunk = (size_t)ಗಾತ್ರ_2;

      ಆದರೆ(!chunk) {
        /* no size, we are done with the data */
        imap_state(ದತ್ತಾಂಶ, imapc, IMAP_STOP);
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
      ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY,
                                 curlx_dyn_ptr(&pp->recvbuf), chunk);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

      infof(ದತ್ತಾಂಶ, "Written %zu bytes, %" FMT_OFF_T
            " bytes are left for transfer", chunk, (curl_off_t)(ಗಾತ್ರ_2 - chunk));

      /* Have we used the entire overflow or part of it?*/
      ಆದರೆ(pp->ಅತಿಪ್ರವಾಹ > chunk) {
        /* remember the remaining trailing overflow data */
        pp->ಅತಿಪ್ರವಾಹ -= chunk;
        curlx_dyn_ಬಾಲ(&pp->recvbuf, pp->ಅತಿಪ್ರವಾಹ);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        pp->ಅತಿಪ್ರವಾಹ = 0; /* handled */
        /* Free the cache */
        curlx_dyn_reset(&pp->recvbuf);
      }
    }

    ಆದರೆ(ದತ್ತಾಂಶ->req.bytecount == ಗಾತ್ರ_2)
      /* The entire data is already transferred! */
      Curl_xfer_ಸಿದ್ಧತೆ_nop(ದತ್ತಾಂಶ);
    ಇಲ್ಲದಿದ್ದರೆ {
      /* IMAP download */
      ದತ್ತಾಂಶ->req.maxdownload = ಗಾತ್ರ_2;
      Curl_xfer_ಸಿದ್ಧತೆ_recv(ದತ್ತಾಂಶ, FIRSTSOCKET, ಗಾತ್ರ_2);
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* We do not know how to parse this line */
    failf(ದತ್ತಾಂಶ, "Failed to parse FETCH response.");
    ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;
  }

  /* End of DO phase */
  imap_state(ದತ್ತಾಂಶ, imapc, IMAP_STOP);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For final FETCH responses performed after the download */
ಸ್ಥಿರ CURLcode imap_state_fetch_final_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                            ರಚನೆ imap_conn *imapc,
                                            ಪೂರ್ಣಾಂಕ imapcode,
                                            imapstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  (ಶೂನ್ಯ)instate;

  ಆದರೆ(imapcode != IMAP_RESP_OK)
    ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;
  ಇಲ್ಲದಿದ್ದರೆ
    /* End of DONE phase */
    imap_state(ದತ್ತಾಂಶ, imapc, IMAP_STOP);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For APPEND responses */
ಸ್ಥಿರ CURLcode imap_state_append_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ imap_conn *imapc,
                                       ಪೂರ್ಣಾಂಕ imapcode,
                                       imapstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  ಆದರೆ(imapcode != '+') {
    ಫಲಿತಾಂಶ = CURLE_UPLOAD_FAILED;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Set the progress upload size */
    Curl_pgrs_ಗಣ_Upload_ಗಾತ್ರ(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.infilesize);

    /* IMAP upload */
    Curl_xfer_ಸಿದ್ಧತೆ_send(ದತ್ತಾಂಶ, FIRSTSOCKET);

    /* End of DO phase */
    imap_state(ದತ್ತಾಂಶ, imapc, IMAP_STOP);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For final APPEND responses performed after the upload */
ಸ್ಥಿರ CURLcode imap_state_append_final_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ imap_conn *imapc,
                                             ಪೂರ್ಣಾಂಕ imapcode,
                                             imapstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  (ಶೂನ್ಯ)instate;

  ಆದರೆ(imapcode != IMAP_RESP_OK)
    ಫಲಿತಾಂಶ = CURLE_UPLOAD_FAILED;
  ಇಲ್ಲದಿದ್ದರೆ
    /* End of DONE phase */
    imap_state(ದತ್ತಾಂಶ, imapc, IMAP_STOP);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode imap_pp_statemachine(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ connectdata *conn)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ imapcode;
  ರಚನೆ imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ರಚನೆ IMAP *imap = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_IMAP_EASY);
  ರಚನೆ pingpong *pp;
  size_t nread = 0;

  ಆದರೆ(!imapc || !imap)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  pp = &imapc->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not IMAP */
upgrade_tls:
  ಆದರೆ(imapc->state == IMAP_UPGRADETLS) {
    ಫಲಿತಾಂಶ = imap_perform_upgrade_tls(ದತ್ತಾಂಶ, imapc, conn);
    ಆದರೆ(ಫಲಿತಾಂಶ || (imapc->state == IMAP_UPGRADETLS))
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* Flush any data that needs to be sent */
  ಆದರೆ(pp->sendleft)
    ಹಿಂತಿರುಗಿ Curl_pp_flushsend(ದತ್ತಾಂಶ, pp);

  ಮಾಡು {
    /* Read the response from the server */
    ಫಲಿತಾಂಶ = Curl_pp_readresp(ದತ್ತಾಂಶ, FIRSTSOCKET, pp, &imapcode, &nread);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    /* Was there an error parsing the response line? */
    ಆದರೆ(imapcode == -1)
      ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;

    ಆದರೆ(!imapcode)
      ನಿಲ್ಲಿಸು;

    /* We have now received a full IMAP server response */
    ಆಯ್ಕೆ(imapc->state) {
    ಸ್ಥಿತಿ IMAP_SERVERGREET:
      ಫಲಿತಾಂಶ = imap_state_servergreet_resp(ದತ್ತಾಂಶ, imapc,
                                           imapcode, imapc->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ IMAP_CAPABILITY:
      ಫಲಿತಾಂಶ = imap_state_capability_resp(ದತ್ತಾಂಶ, imapc, imapcode, imapc->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ IMAP_STARTTLS:
      ಫಲಿತಾಂಶ = imap_state_starttls_resp(ದತ್ತಾಂಶ, imapc, imapcode, imapc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      ಆದರೆ(!ಫಲಿತಾಂಶ && (imapc->state == IMAP_UPGRADETLS))
        ಹೋಗು upgrade_tls;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ IMAP_AUTHENTICATE:
      ಫಲಿತಾಂಶ = imap_state_auth_resp(ದತ್ತಾಂಶ, imapc, imapcode, imapc->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ IMAP_LOGIN:
      ಫಲಿತಾಂಶ = imap_state_login_resp(ದತ್ತಾಂಶ, imapc, imapcode, imapc->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ IMAP_LIST:
    ಸ್ಥಿತಿ IMAP_ಹುಡುಕು:
      ಫಲಿತಾಂಶ = imap_state_listsearch_resp(ದತ್ತಾಂಶ, imapc, imapcode, imapc->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ IMAP_SELECT:
      ಫಲಿತಾಂಶ = imap_state_select_resp(ದತ್ತಾಂಶ, imapc, imap,
                                      imapcode, imapc->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ IMAP_FETCH:
      ಫಲಿತಾಂಶ = imap_state_fetch_resp(ದತ್ತಾಂಶ, imapc, imapcode, imapc->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ IMAP_FETCH_FINAL:
      ಫಲಿತಾಂಶ = imap_state_fetch_final_resp(ದತ್ತಾಂಶ, imapc,
                                           imapcode, imapc->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ IMAP_APPEND:
      ಫಲಿತಾಂಶ = imap_state_append_resp(ದತ್ತಾಂಶ, imapc, imapcode, imapc->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ IMAP_APPEND_FINAL:
      ಫಲಿತಾಂಶ = imap_state_append_final_resp(ದತ್ತಾಂಶ, imapc,
                                            imapcode, imapc->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ IMAP_LOGOUT:
    ಪೂರ್ವನಿಯೋಜಿತ:
      /* internal error */
      imap_state(ದತ್ತಾಂಶ, imapc, IMAP_STOP);
      ನಿಲ್ಲಿಸು;
    }
  } ಆಗ(!ಫಲಿತಾಂಶ && imapc->state != IMAP_STOP && Curl_pp_moredata(pp));

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Called repeatedly until done from multi.c */
ಸ್ಥಿರ CURLcode imap_multi_statemach(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ imap_conn *imapc =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_IMAP_CONN);

  *ಮುಗಿದ = FALSE;
  ಆದರೆ(!imapc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಫಲಿತಾಂಶ = Curl_pp_statemach(ದತ್ತಾಂಶ, &imapc->pp, FALSE, FALSE);
  *ಮುಗಿದ = (imapc->state == IMAP_STOP);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode imap_ಖಂಡ_statemach(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ imap_conn *imapc,
                                     bool disconnecting)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆಗ(imapc->state != IMAP_STOP && !ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_pp_statemach(ದತ್ತಾಂಶ, &imapc->pp, TRUE, disconnecting);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For the IMAP "protocol connect" and "doing" phases only */
ಸ್ಥಿರ CURLcode imap_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ imap_conn *imapc =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_IMAP_CONN);
  ಹಿಂತಿರುಗಿ imapc ? Curl_pp_pollset(ದತ್ತಾಂಶ, &imapc->pp, ps) : CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ imap_easy_reset(ರಚನೆ IMAP *imap)
{
  curlx_safefree(imap->mailbox);
  curlx_safefree(imap->uid);
  curlx_safefree(imap->mindex);
  curlx_safefree(imap->section);
  curlx_safefree(imap->partial);
  curlx_safefree(imap->query);
  curlx_safefree(imap->custom);
  curlx_safefree(imap->custom_params);
  imap->uidvalidity_ಗಣ = FALSE;
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
ಸ್ಥಿರ bool imap_is_bchar(ಅಕ್ಷರ ch)
{
  /* Performing the alnum check first with macro is faster because of ASCII
     arithmetic */
  ಹಿಂತಿರುಗಿ ch && (ISALNUM(ch) || strchr(":@/&=-._~!$\'()*+,%", ch));
}

/***********************************************************************
 *
 * imap_parse_url_options()
 *
 * Parse the URL login options.
 */
ಸ್ಥಿರ CURLcode imap_parse_url_options(ರಚನೆ connectdata *conn,
                                       ರಚನೆ imap_conn *imapc)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr = conn->options;
  bool prefer_login = FALSE;

  ಆಗ(!ಫಲಿತಾಂಶ && ptr && *ptr) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ = ptr;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ;

    ಆಗ(*ptr && *ptr != '=')
      ptr++;

    ಮೌಲ್ಯ = ptr + 1;

    ಆಗ(*ptr && *ptr != ';')
      ptr++;

    ಆದರೆ(curl_strnequal(ಕೀಲಿ, "AUTH=+LOGIN", 11)) {
      /* User prefers plaintext LOGIN over any SASL, including SASL LOGIN */
      prefer_login = TRUE;
      imapc->sasl.prefmech = SASL_AUTH_NONE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strnequal(ಕೀಲಿ, "AUTH=", 5)) {
      prefer_login = FALSE;
      ಫಲಿತಾಂಶ = Curl_sasl_parse_url_auth_option(&imapc->sasl,
                                               ಮೌಲ್ಯ, ptr - ಮೌಲ್ಯ);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      prefer_login = FALSE;
      ಫಲಿತಾಂಶ = CURLE_URL_MALFORMAT;
    }

    ಆದರೆ(*ptr == ';')
      ptr++;
  }

  ಆದರೆ(prefer_login)
    imapc->preftype = IMAP_TYPE_CLEARTEXT;
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆಯ್ಕೆ(imapc->sasl.prefmech) {
    ಸ್ಥಿತಿ SASL_AUTH_NONE:
      imapc->preftype = IMAP_TYPE_NONE;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SASL_AUTH_DEFAULT:
      imapc->preftype = IMAP_TYPE_ANY;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      imapc->preftype = IMAP_TYPE_SASL;
      ನಿಲ್ಲಿಸು;
    }
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * imap_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
ಸ್ಥಿರ CURLcode imap_parse_url_ಮಾರ್ಗ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ IMAP *imap)
{
  /* The imap struct is already initialized in imap_connect() */
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *begin = &ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ[1]; /* skip leading slash */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr = begin;

  /* See how much of the URL is a valid path and decode it */
  ಆಗ(imap_is_bchar(*ptr))
    ptr++;

  ಆದರೆ(ptr != begin) {
    /* Remove the trailing slash if present */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ = ptr;
    ಆದರೆ(ಕೊನೆ > begin && ಕೊನೆ[-1] == '/')
      ಕೊನೆ--;

    ಫಲಿತಾಂಶ = Curl_urldecode(begin, ಕೊನೆ - begin, &imap->mailbox, ಖಾಲಿ,
                            REJECT_CTRL);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    imap->mailbox = ಖಾಲಿ;

  /* There can be any number of parameters in the form ";NAME=VALUE" */
  ಆಗ(*ptr == ';') {
    ಅಕ್ಷರ *name;
    ಅಕ್ಷರ *ಮೌಲ್ಯ;
    size_t valuelen;

    /* Find the length of the name parameter */
    begin = ++ptr;
    ಆಗ(*ptr && *ptr != '=')
      ptr++;

    ಆದರೆ(!*ptr)
      ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;

    /* Decode the name parameter */
    ಫಲಿತಾಂಶ = Curl_urldecode(begin, ptr - begin, &name, ಖಾಲಿ,
                            REJECT_CTRL);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    /* Find the length of the value parameter */
    begin = ++ptr;
    ಆಗ(imap_is_bchar(*ptr))
      ptr++;

    /* Decode the value parameter */
    ಫಲಿತಾಂಶ = Curl_urldecode(begin, ptr - begin, &ಮೌಲ್ಯ, &valuelen,
                            REJECT_CTRL);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      curlx_free(name);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    DEBUGF(infof(ದತ್ತಾಂಶ, "IMAP URL parameter '%s' = '%s'", name, ಮೌಲ್ಯ));

    /* Process the known hierarchical parameters (UIDVALIDITY, UID, SECTION
       and PARTIAL) stripping of the trailing slash character if it is
       present.

       Note: Unknown parameters trigger a URL_MALFORMAT error. */
    ಆದರೆ(valuelen > 0 && ಮೌಲ್ಯ[valuelen - 1] == '/')
      ಮೌಲ್ಯ[valuelen - 1] = '\0';
    ಆದರೆ(valuelen) {
      ಆದರೆ(curl_strequal(name, "UIDVALIDITY") && !imap->uidvalidity_ಗಣ) {
        curl_off_t ಅಂಕೆ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೌಲ್ಯ;
        ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &ಅಂಕೆ, UINT_MAX)) {
          imap->uidvalidity = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಅಂಕೆ;
          imap->uidvalidity_ಗಣ = TRUE;
        }
        curlx_free(ಮೌಲ್ಯ);
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal(name, "UID") && !imap->uid) {
        imap->uid = ಮೌಲ್ಯ;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal(name, "MAILINDEX") && !imap->mindex) {
        imap->mindex = ಮೌಲ್ಯ;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal(name, "SECTION") && !imap->section) {
        imap->section = ಮೌಲ್ಯ;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal(name, "PARTIAL") && !imap->partial) {
        imap->partial = ಮೌಲ್ಯ;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        curlx_free(name);
        curlx_free(ಮೌಲ್ಯ);
        ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ
      /* blank? */
      curlx_free(ಮೌಲ್ಯ);
    curlx_free(name);
  }

  /* Does the URL contain a query parameter? Only valid when we have a mailbox
     and no UID as per RFC-5092 */
  ಆದರೆ(imap->mailbox && !imap->uid && !imap->mindex) {
    /* Get the query parameter, URL decoded */
    CURLUcode uc = curl_url_get(ದತ್ತಾಂಶ->state.uh, CURLUPART_QUERY, &imap->query,
                                CURLU_URLDECODE);
    ಆದರೆ(uc == CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  /* Any extra stuff at the end of the URL is an error */
  ಆದರೆ(*ptr)
    ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/***********************************************************************
 *
 * imap_parse_custom_request()
 *
 * Parse the custom request.
 */
ಸ್ಥಿರ CURLcode imap_parse_custom_request(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ IMAP *imap)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *custom = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_CUSTOMREQUEST);

  ಆದರೆ(custom) {
    /* URL decode the custom request */
    ಫಲಿತಾಂಶ = Curl_urldecode(custom, 0, &imap->custom, ಖಾಲಿ, REJECT_CTRL);

    /* Extract the parameters if specified */
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *params = imap->custom;

      ಆಗ(*params && *params != ' ')
        params++;

      ಆದರೆ(*params) {
        imap->custom_params = curlx_strdup(params);
        imap->custom[params - imap->custom] = '\0';

        ಆದರೆ(!imap->custom_params)
          ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
    }
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ಸ್ಥಿರ CURLcode imap_connect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ರಚನೆ imap_conn *imapc =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_IMAP_CONN);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  *ಮುಗಿದ = FALSE; /* default to not done yet */
  ಆದರೆ(!imapc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* Parse the URL options */
  ಫಲಿತಾಂಶ = imap_parse_url_options(ದತ್ತಾಂಶ->conn, imapc);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* Start off waiting for the server greeting response */
  imap_state(ದತ್ತಾಂಶ, imapc, IMAP_SERVERGREET);

  /* Start off with an response id of '*' */
  curlx_strcopy(imapc->resptag, ಗಾತ್ರ(imapc->resptag), STRCONST("*"));

  ಫಲಿತಾಂಶ = imap_multi_statemach(ದತ್ತಾಂಶ, ಮುಗಿದ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ಸ್ಥಿರ CURLcode imap_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLcode ಸ್ಥಿತಿ_2,
                          bool premature)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ರಚನೆ IMAP *imap = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_IMAP_EASY);

  ಆದರೆ(!imapc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಆದರೆ(!imap)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಆದರೆ(ಸ್ಥಿತಿ_2 || premature) {
    CURL_TRC_M(ದತ್ತಾಂಶ, "IMAP done, not finalizing transfer");
    connclose(conn); /* marked for closure */
    ಫಲಿತಾಂಶ = ಸ್ಥಿತಿ_2;         /* use the already set error code */
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.connect_only &&
          ((!imap->custom && (imap->uid || imap->mindex)) ||
           (imap->custom && ದತ್ತಾಂಶ->req.maxdownload > 0) ||
           ದತ್ತಾಂಶ->state.upload || IS_MIME_POST(ದತ್ತಾಂಶ))) {
    /* Handle responses after FETCH or APPEND transfer has finished.
       For custom commands, check if we set up a download which indicates
       a FETCH-like command with literal data. */

    ಆದರೆ(!ದತ್ತಾಂಶ->state.upload && !IS_MIME_POST(ದತ್ತಾಂಶ))
      imap_state(ದತ್ತಾಂಶ, imapc, IMAP_FETCH_FINAL);
    ಇಲ್ಲದಿದ್ದರೆ {
      /* End the APPEND command first by sending an empty line */
      ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &imapc->pp, "%s", "");
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        imap_state(ದತ್ತಾಂಶ, imapc, IMAP_APPEND_FINAL);
    }

    /* Run the state-machine */
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = imap_ಖಂಡ_statemach(ದತ್ತಾಂಶ, imapc, FALSE);
  }

  imap_easy_reset(imap);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * imap_perform()
 *
 * This is the actual DO function for IMAP. Fetch or append a message, or do
 * other things according to the options previously setup.
 */
ಸ್ಥಿರ CURLcode imap_perform(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *connected,
                             bool *dophase_ಮುಗಿದ)
{
  /* This is IMAP and no proxy */
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ರಚನೆ IMAP *imap = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_IMAP_EASY);
  bool selected = FALSE;

  DEBUGF(infof(ದತ್ತಾಂಶ, "DO phase starts"));
  ಆದರೆ(!imapc || !imap)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(ದತ್ತಾಂಶ->req.no_body) {
    /* Requested no body means no transfer */
    imap->transfer = PPTRANSFER_INFO;
  }

  *dophase_ಮುಗಿದ = FALSE; /* not done yet */

  /* Determine if the requested mailbox (with the same UIDVALIDITY if set)
     has already been selected on this connection */
  ಆದರೆ(imap->mailbox && imapc->mailbox &&
     (!strcmp(imap->mailbox, imapc->mailbox) ||
      (curl_strequal(imap->mailbox, "INBOX") &&
       curl_strequal(imapc->mailbox, "INBOX"))) &&
     (!imap->uidvalidity_ಗಣ || !imapc->mb_uidvalidity_ಗಣ ||
      (imap->uidvalidity == imapc->mb_uidvalidity)))
    selected = TRUE;

  /* Start the first command in the DO phase */
  ಆದರೆ(ದತ್ತಾಂಶ->state.upload || IS_MIME_POST(ದತ್ತಾಂಶ))
    /* APPEND can be executed directly */
    ಫಲಿತಾಂಶ = imap_perform_append(ದತ್ತಾಂಶ, imapc, imap);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(imap->custom && (selected || !imap->mailbox))
    /* Custom command using the same mailbox or no mailbox */
    ಫಲಿತಾಂಶ = imap_perform_list(ದತ್ತಾಂಶ, imapc, imap);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!imap->custom && selected && (imap->uid || imap->mindex))
    /* FETCH from the same mailbox */
    ಫಲಿತಾಂಶ = imap_perform_fetch(ದತ್ತಾಂಶ, imapc, imap);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!imap->custom && selected && imap->query)
    /* SEARCH the current mailbox */
    ಫಲಿತಾಂಶ = imap_perform_ಹುಡುಕು(ದತ್ತಾಂಶ, imapc, imap);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(imap->mailbox && !selected &&
          (imap->custom || imap->uid || imap->mindex || imap->query))
    /* SELECT the mailbox */
    ಫಲಿತಾಂಶ = imap_perform_select(ದತ್ತಾಂಶ, imapc, imap);
  ಇಲ್ಲದಿದ್ದರೆ
    /* LIST */
    ಫಲಿತಾಂಶ = imap_perform_list(ದತ್ತಾಂಶ, imapc, imap);

  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* Run the state-machine */
  ಫಲಿತಾಂಶ = imap_multi_statemach(ದತ್ತಾಂಶ, dophase_ಮುಗಿದ);

  *connected = Curl_conn_is_connected(conn, FIRSTSOCKET);

  ಆದರೆ(*dophase_ಮುಗಿದ)
    DEBUGF(infof(ದತ್ತಾಂಶ, "DO phase is complete"));

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Call this when the DO phase has completed */
ಸ್ಥಿರ CURLcode imap_dophase_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ IMAP *imap,
                                  bool connected)
{
  (ಶೂನ್ಯ)connected;

  ಆದರೆ(imap->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_ಸಿದ್ಧತೆ_nop(ದತ್ತಾಂಶ);

  ಹಿಂತಿರುಗಿ CURLE_OK;
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
ಸ್ಥಿರ CURLcode imap_regular_transfer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ IMAP *imap,
                                      bool *dophase_ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = -1;

  /* Set the progress data */
  Curl_pgrsReset(ದತ್ತಾಂಶ);

  /* Carry out the perform */
  ಫಲಿತಾಂಶ = imap_perform(ದತ್ತಾಂಶ, &connected, dophase_ಮುಗಿದ);

  /* Perform post DO phase operations if necessary */
  ಆದರೆ(!ಫಲಿತಾಂಶ && *dophase_ಮುಗಿದ)
    ಫಲಿತಾಂಶ = imap_dophase_ಮುಗಿದ(ದತ್ತಾಂಶ, imap, connected);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ಸ್ಥಿರ CURLcode imap_do(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ರಚನೆ IMAP *imap = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_IMAP_EASY);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  *ಮುಗಿದ = FALSE; /* default to false */

  ಆದರೆ(!imap)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  /* Parse the URL path */
  ಫಲಿತಾಂಶ = imap_parse_url_ಮಾರ್ಗ(ದತ್ತಾಂಶ, imap);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* Parse the custom request */
  ಫಲಿತಾಂಶ = imap_parse_custom_request(ದತ್ತಾಂಶ, imap);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = imap_regular_transfer(ದತ್ತಾಂಶ, imap, ಮುಗಿದ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * imap_disconnect()
 *
 * Disconnect from an IMAP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ಸ್ಥಿರ CURLcode imap_disconnect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ connectdata *conn, bool dead_connection)
{
  ರಚನೆ imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);

  ಆದರೆ(imapc &&
     /* We cannot send quit unconditionally. If this connection is stale or
        bad in any way (pingpong has pending data to send),
        sending quit and waiting around here will make the
        disconnect wait in vain and cause more problems than we need to. */
     !dead_connection && conn->ಬಿಟ್ಟುಗಳು.protoconnstart &&
     !Curl_pp_needs_flush(ದತ್ತಾಂಶ, &imapc->pp) &&
     !imap_perform_logout(ದತ್ತಾಂಶ, imapc))
    (ಶೂನ್ಯ)imap_ಖಂಡ_statemach(ದತ್ತಾಂಶ, imapc, TRUE); /* ignore errors */

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Called from multi.c while DOing */
ಸ್ಥಿರ CURLcode imap_doing(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *dophase_ಮುಗಿದ)
{
  ರಚನೆ IMAP *imap = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_IMAP_EASY);
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!imap)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಫಲಿತಾಂಶ = imap_multi_statemach(ದತ್ತಾಂಶ, dophase_ಮುಗಿದ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    DEBUGF(infof(ದತ್ತಾಂಶ, "DO phase failed"));
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*dophase_ಮುಗಿದ) {
    ಫಲಿತಾಂಶ = imap_dophase_ಮುಗಿದ(ದತ್ತಾಂಶ, imap, FALSE /* not connected */);

    DEBUGF(infof(ದತ್ತಾಂಶ, "DO phase is complete"));
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ imap_easy_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ IMAP *imap = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  imap_easy_reset(imap);
  curlx_free(imap);
}

ಸ್ಥಿರ ಶೂನ್ಯ imap_conn_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ imap_conn *imapc = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  Curl_pp_disconnect(&imapc->pp);
  curlx_safefree(imapc->mailbox);
  curlx_free(imapc);
}

/* SASL parameters for the imap protocol */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ SASLproto saslimap = {
  "imap",                     /* The service name */
  imap_perform_authenticate,  /* Send authentication command */
  imap_continue_authenticate, /* Send authentication continuation */
  imap_cancel_authenticate,   /* Send authentication cancellation */
  imap_get_ಸಂದೇಶ,           /* Get SASL response message */
  0,                          /* No maximum initial response length */
  '+',                        /* Code received when continuation is expected */
  IMAP_RESP_OK,               /* Code to receive upon authentication success */
  SASL_AUTH_DEFAULT,          /* Default mechanisms */
  SASL_FLAG_BASE64            /* Configuration flags */
};

ಸ್ಥಿರ CURLcode imap_ಸಿದ್ಧತೆ_connection(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ connectdata *conn)
{
  ರಚನೆ imap_conn *imapc;
  ರಚನೆ pingpong *pp;
  ರಚನೆ IMAP *imap;

  imapc = curlx_calloc(1, ಗಾತ್ರ(*imapc));
  ಆದರೆ(!imapc)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  pp = &imapc->pp;
  PINGPONG_ಸಿದ್ಧತೆ(pp, imap_pp_statemachine, imap_endofresp);

  /* Set the default preferred authentication type and mechanism */
  imapc->preftype = IMAP_TYPE_ANY;
  Curl_sasl_init(&imapc->sasl, ದತ್ತಾಂಶ, &saslimap);

  Curl_pp_init(pp, Curl_pgrs_now(ದತ್ತಾಂಶ));

  ಆದರೆ(Curl_conn_meta_ಗಣ(conn, CURL_META_IMAP_CONN, imapc, imap_conn_dtor))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  imap = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ IMAP));
  ಆದರೆ(!imap ||
     Curl_meta_ಗಣ(ದತ್ತಾಂಶ, CURL_META_IMAP_EASY, imap, imap_easy_dtor))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * IMAP protocol.
 */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_imap = {
  imap_ಸಿದ್ಧತೆ_connection,            /* setup_connection */
  imap_do,                          /* do_it */
  imap_ಮುಗಿದ,                        /* done */
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

#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_IMAP */
