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
#അന്തര്ഭാവ "curl_setup.h"
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "imap.h"

#ifndef CURL_DISABLE_IMAP

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef __VMS
#അന്തര്ഭാവ <in.h>
#അന്തര്ഭാവ <inet.h>
#endif

#അന്തര്ഭാവ "curlx/dynbuf.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "pingpong.h"
#അന്തര്ഭാവ "mime.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "strcase.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "bufref.h"
#അന്തര്ഭാവ "curl_sasl.h"
#അന്തര്ഭാവ "curlx/strcopy.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_IMAP_EASY   "meta:proto:imap:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_IMAP_CONN   "meta:proto:imap:conn"

തരനിർവചനം എണ്ണൽ {
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
  IMAP_തിരയൽ,
  IMAP_LOGOUT,
  IMAP_LAST          /* never used */
} imapstate;

/* imap_conn is used for struct connection-oriented data */
ഘടന imap_conn {
  ഘടന pingpong pp;
  ഘടന SASL sasl;           /* SASL-related parameters */
  അക്ഷരം *mailbox;              /* The last selected mailbox */
  imapstate state;            /* Always use imap.c:state() to change state! */
  unsigned പൂർണ്ണസംഖ്യ mb_uidvalidity; /* UIDVALIDITY parsed from select response */
  അക്ഷരം resptag[5];            /* Response tag to wait for */
  unsigned അക്ഷരം preftype;     /* Preferred authentication type */
  unsigned അക്ഷരം cmdid;        /* Last used command ID */
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
ഘടന IMAP {
  curl_pp_transfer transfer;
  അക്ഷരം *mailbox;          /* Mailbox to select */
  അക്ഷരം *uid;              /* Message UID to fetch */
  അക്ഷരം *mindex;           /* Index in mail box of mail to fetch */
  അക്ഷരം *section;          /* Message SECTION to fetch */
  അക്ഷരം *partial;          /* Message PARTIAL to fetch */
  അക്ഷരം *query;            /* Query to search for */
  അക്ഷരം *custom;           /* Custom request */
  അക്ഷരം *custom_params;    /* Parameters for the custom request */
  unsigned പൂർണ്ണസംഖ്യ uidvalidity; /* UIDVALIDITY to check in select */
  BIT(uidvalidity_set);
};

#define IMAP_RESP_OK       1
#define IMAP_RESP_NOT_OK   2
#define IMAP_RESP_PREAUTH  3

ഘടന ulbits {
  പൂർണ്ണസംഖ്യ bit;
  സ്ഥിരം അക്ഷരം *flag;
};

/***********************************************************************
 *
 * imap_sendf()
 *
 * Sends the formatted string as an IMAP command to the server.
 *
 * Designed to never block.
 */
സ്ഥിതം CURLcode imap_sendf(ഘടന Curl_easy *data,
                           ഘടന imap_conn *imapc,
                           സ്ഥിരം അക്ഷരം *fmt, ...) CURL_PRINTF(3, 4);
സ്ഥിതം CURLcode imap_sendf(ഘടന Curl_easy *data,
                           ഘടന imap_conn *imapc,
                           സ്ഥിരം അക്ഷരം *fmt, ...)
{
  CURLcode result = CURLE_OK;
  va_list ap;

  DEBUGASSERT(fmt);

  /* Calculate the tag based on the connection ID and command ID */
  curl_msnprintf(imapc->resptag, വലുപ്പം(imapc->resptag), "%c%03d",
                 'A' + curlx_sltosi((long)(data->conn->connection_id % 26)),
                 ++imapc->cmdid);

  va_start(ap, fmt);
  result = Curl_pp_vsendf(data, &imapc->pp, imapc->resptag, fmt, ap);
  va_end(ap);
  തിരികെ result;
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
സ്ഥിതം അക്ഷരം *imap_atom(സ്ഥിരം അക്ഷരം *str, bool escape_only)
{
  ഘടന dynbuf വരി;
  size_t nclean;
  size_t len;

  എങ്കിൽ(!str)
    തിരികെ ശൂന്യ;

  len = strlen(str);
  nclean = strcspn(str, "() {%*]\\\"");
  എങ്കിൽ(len == nclean)
    /* nothing to escape, return a strdup */
    തിരികെ curlx_strdup(str);

  curlx_dyn_init(&വരി, 2000);

  എങ്കിൽ(!escape_only && curlx_dyn_addn(&വരി, "\"", 1))
    തിരികെ ശൂന്യ;

  ആകുമ്പോൾ(*str) {
    എങ്കിൽ((*str == '\\' || *str == '"') &&
       curlx_dyn_addn(&വരി, "\\", 1))
      തിരികെ ശൂന്യ;
    എങ്കിൽ(curlx_dyn_addn(&വരി, str, 1))
      തിരികെ ശൂന്യ;
    str++;
  }

  എങ്കിൽ(!escape_only && curlx_dyn_addn(&വരി, "\"", 1))
    തിരികെ ശൂന്യ;

  തിരികെ curlx_dyn_ptr(&വരി);
}

/*
 * Finds the start of a literal '{size}' in line, skipping over quoted strings.
 */
സ്ഥിതം സ്ഥിരം അക്ഷരം *imap_find_literal(സ്ഥിരം അക്ഷരം *വരി, size_t len)
{
  സ്ഥിരം അക്ഷരം *end = വരി + len;
  bool in_quote = FALSE;

  ആകുമ്പോൾ(വരി < end) {
    എങ്കിൽ(in_quote) {
      എങ്കിൽ(*വരി == '\\' && (വരി + 1) < end) {
        വരി += 2;
        തുടരുക;
      }
      എങ്കിൽ(*വരി == '"')
        in_quote = FALSE;
    }
    അല്ലെങ്കിൽ {
      എങ്കിൽ(*വരി == '"')
        in_quote = TRUE;
      അല്ലെങ്കിൽ എങ്കിൽ(*വരി == '{')
        തിരികെ വരി;
    }
    വരി++;
  }
  തിരികെ ശൂന്യ;
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
സ്ഥിതം bool imap_matchresp(സ്ഥിരം അക്ഷരം *വരി, size_t len, സ്ഥിരം അക്ഷരം *cmd)
{
  സ്ഥിരം അക്ഷരം *end = വരി + len;
  size_t cmd_len = strlen(cmd);

  /* Skip the untagged response marker */
  വരി += 2;

  /* Do we have a number after the marker? */
  എങ്കിൽ(വരി < end && ISDIGIT(*വരി)) {
    /* Skip the number */
    ചെയ്യുക
      വരി++;
    ആകുമ്പോൾ(വരി < end && ISDIGIT(*വരി));

    /* Do we have the space character? */
    എങ്കിൽ(വരി == end || *വരി != ' ')
      തിരികെ FALSE;

    വരി++;
  }

  /* Does the command name match and is it followed by a space character or at
     the end of line? */
  എങ്കിൽ(വരി + cmd_len <= end && curl_strnequal(വരി, cmd, cmd_len) &&
     (വരി[cmd_len] == ' ' || വരി + cmd_len + 2 == end))
    തിരികെ TRUE;

  തിരികെ FALSE;
}

/***********************************************************************
 *
 * imap_endofresp()
 *
 * Checks whether the given string is a valid tagged, untagged or continuation
 * response which can be processed by the response handler.
 */
സ്ഥിതം bool imap_endofresp(ഘടന Curl_easy *data, ഘടന connectdata *conn,
                           സ്ഥിരം അക്ഷരം *വരി, size_t len, പൂർണ്ണസംഖ്യ *resp)
{
  ഘടന imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ഘടന IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  സ്ഥിരം അക്ഷരം *id;
  size_t id_len;

  DEBUGASSERT(imapc);
  DEBUGASSERT(imap);
  എങ്കിൽ(!imapc || !imap)
    തിരികെ FALSE;

  /* Do we have a tagged command response? */
  id = imapc->resptag;
  id_len = strlen(id);
  എങ്കിൽ(len >= id_len + 1 && !memcmp(id, വരി, id_len) && വരി[id_len] == ' ') {
    വരി += id_len + 1;
    len -= id_len + 1;

    എങ്കിൽ(len >= 2 && !memcmp(വരി, "OK", 2))
      *resp = IMAP_RESP_OK;
    അല്ലെങ്കിൽ എങ്കിൽ(len >= 7 && !memcmp(വരി, "PREAUTH", 7))
      *resp = IMAP_RESP_PREAUTH;
    അല്ലെങ്കിൽ
      *resp = IMAP_RESP_NOT_OK;

    തിരികെ TRUE;
  }

  /* Do we have an untagged command response? */
  എങ്കിൽ(len >= 2 && !memcmp("* ", വരി, 2)) {
    തിരഞ്ഞെടുപ്പ്(imapc->state) {
    /* States which are interested in untagged responses */
    അവസ്ഥ IMAP_CAPABILITY:
      എങ്കിൽ(!imap_matchresp(വരി, len, "CAPABILITY"))
        തിരികെ FALSE;
      നിർത്തുക;

    അവസ്ഥ IMAP_LIST:
      എങ്കിൽ((!imap->custom && !imap_matchresp(വരി, len, "LIST")) ||
         (imap->custom && !imap_matchresp(വരി, len, imap->custom) &&
          (!curl_strequal(imap->custom, "STORE") ||
           !imap_matchresp(വരി, len, "FETCH")) &&
          !curl_strequal(imap->custom, "SELECT") &&
          !curl_strequal(imap->custom, "EXAMINE") &&
          !curl_strequal(imap->custom, "SEARCH") &&
          !curl_strequal(imap->custom, "EXPUNGE") &&
          !curl_strequal(imap->custom, "LSUB") &&
          !curl_strequal(imap->custom, "UID") &&
          !curl_strequal(imap->custom, "GETQUOTAROOT") &&
          !curl_strequal(imap->custom, "NOOP")))
        തിരികെ FALSE;
      നിർത്തുക;

    അവസ്ഥ IMAP_SELECT:
      /* SELECT is special in that its untagged responses do not have a
         common prefix so accept anything! */
      നിർത്തുക;

    അവസ്ഥ IMAP_FETCH:
      എങ്കിൽ(!imap_matchresp(വരി, len, "FETCH"))
        തിരികെ FALSE;
      നിർത്തുക;

    അവസ്ഥ IMAP_തിരയൽ:
      എങ്കിൽ(!imap_matchresp(വരി, len, "SEARCH"))
        തിരികെ FALSE;
      നിർത്തുക;

    /* Ignore other untagged responses */
    സ്ഥിരസ്ഥിതി:
      തിരികെ FALSE;
    }

    *resp = '*';
    തിരികെ TRUE;
  }

  /* Do we have a continuation response? This should be a + symbol followed by
     a space and optionally some text as per RFC-3501 for the AUTHENTICATE and
     APPEND commands and as outlined in Section 4. Examples of RFC-4959 but
     some email servers ignore this and only send a single + instead. */
  എങ്കിൽ(!imap->custom && ((len == 3 && വരി[0] == '+') ||
                       (len >= 2 && !memcmp("+ ", വരി, 2)))) {
    തിരഞ്ഞെടുപ്പ്(imapc->state) {
    /* States which are interested in continuation responses */
    അവസ്ഥ IMAP_AUTHENTICATE:
    അവസ്ഥ IMAP_APPEND:
      *resp = '+';
      നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
      failf(data, "Unexpected continuation response");
      *resp = -1;
      നിർത്തുക;
    }

    തിരികെ TRUE;
  }

  തിരികെ FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * imap_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
സ്ഥിതം CURLcode imap_get_message(ഘടന Curl_easy *data, ഘടന bufref *out)
{
  ഘടന imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  അക്ഷരം *message;
  size_t len;

  എങ്കിൽ(!imapc)
    തിരികെ CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&imapc->pp.recvbuf);
  len = imapc->pp.nfinal;
  എങ്കിൽ(len > 2) {
    /* Find the start of the message */
    len -= 2;
    വേണ്ടി(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    ആകുമ്പോൾ(len--)
      എങ്കിൽ(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        നിർത്തുക;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, ശൂന്യ);
  }
  അല്ലെങ്കിൽ
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, ശൂന്യ);

  തിരികെ CURLE_OK;
}

/***********************************************************************
 *
 * imap_state()
 *
 * This is the ONLY way to change IMAP state!
 */
സ്ഥിതം ശൂന്യം imap_state(ഘടന Curl_easy *data,
                       ഘടന imap_conn *imapc,
                       imapstate newstate)
{
#എങ്കിൽ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം names[] = {
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

  എങ്കിൽ(imapc->state != newstate)
    infof(data, "IMAP %p state change from %s to %s",
          (ശൂന്യം *)imapc, names[imapc->state], names[newstate]);
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
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
സ്ഥിതം CURLcode imap_perform_capability(ഘടന Curl_easy *data,
                                        ഘടന imap_conn *imapc)
{
  CURLcode result = CURLE_OK;

  imapc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  imapc->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  imapc->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPABILITY command */
  result = imap_sendf(data, imapc, "CAPABILITY");

  എങ്കിൽ(!result)
    imap_state(data, imapc, IMAP_CAPABILITY);

  തിരികെ result;
}

/***********************************************************************
 *
 * imap_perform_starttls()
 *
 * Sends the STARTTLS command to start the upgrade to TLS.
 */
സ്ഥിതം CURLcode imap_perform_starttls(ഘടന Curl_easy *data,
                                      ഘടന imap_conn *imapc)
{
  /* Send the STARTTLS command */
  CURLcode result = imap_sendf(data, imapc, "STARTTLS");

  എങ്കിൽ(!result)
    imap_state(data, imapc, IMAP_STARTTLS);

  തിരികെ result;
}

/***********************************************************************
 *
 * imap_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
സ്ഥിതം CURLcode imap_perform_upgrade_tls(ഘടന Curl_easy *data,
                                         ഘടന imap_conn *imapc,
                                         ഘടന connectdata *conn)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  CURLcode result;
  bool ssldone = FALSE;

  എങ്കിൽ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    എങ്കിൽ(result)
      പോകുക out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_imaps;
  }

  DEBUGASSERT(!imapc->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "imap_perform_upgrade_tls, connect -> %d, %d",
               (പൂർണ്ണസംഖ്യ)result, ssldone));
  എങ്കിൽ(!result && ssldone) {
    imapc->ssldone = ssldone;
    /* perform CAPA now, changes imapc->state out of IMAP_UPGRADETLS */
    result = imap_perform_capability(data, imapc);
  }
out:
  തിരികെ result;
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
  (ശൂന്യം)imapc;
  (ശൂന്യം)conn;
  തിരികെ CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * imap_perform_login()
 *
 * Sends a clear text LOGIN command to authenticate with.
 */
സ്ഥിതം CURLcode imap_perform_login(ഘടന Curl_easy *data,
                                   ഘടന imap_conn *imapc,
                                   ഘടന connectdata *conn)
{
  CURLcode result = CURLE_OK;
  അക്ഷരം *user;
  അക്ഷരം *passwd;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  എങ്കിൽ(!conn->creds) {
    imap_state(data, imapc, IMAP_STOP);

    തിരികെ result;
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

  എങ്കിൽ(!result)
    imap_state(data, imapc, IMAP_LOGIN);

  തിരികെ result;
}

/***********************************************************************
 *
 * imap_perform_authenticate()
 *
 * Sends an AUTHENTICATE command allowing the client to login with the given
 * SASL authentication mechanism.
 */
സ്ഥിതം CURLcode imap_perform_authenticate(ഘടന Curl_easy *data,
                                          സ്ഥിരം അക്ഷരം *mech,
                                          സ്ഥിരം ഘടന bufref *initresp)
{
  ഘടന imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  CURLcode result = CURLE_OK;
  സ്ഥിരം അക്ഷരം *ir = Curl_bufref_ptr(initresp);

  എങ്കിൽ(!imapc)
    തിരികെ CURLE_FAILED_INIT;
  എങ്കിൽ(ir) {
    /* Send the AUTHENTICATE command with the initial response */
    result = imap_sendf(data, imapc, "AUTHENTICATE %s %s",
                        mech, *ir ? ir : "=");
  }
  അല്ലെങ്കിൽ {
    /* Send the AUTHENTICATE command */
    result = imap_sendf(data, imapc, "AUTHENTICATE %s", mech);
  }

  തിരികെ result;
}

/***********************************************************************
 *
 * imap_continue_authenticate()
 *
 * Sends SASL continuation data.
 */
സ്ഥിതം CURLcode imap_continue_authenticate(ഘടന Curl_easy *data,
                                           സ്ഥിരം അക്ഷരം *mech,
                                           സ്ഥിരം ഘടന bufref *resp)
{
  ഘടന imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  (ശൂന്യം)mech;
  എങ്കിൽ(!imapc)
    തിരികെ CURLE_FAILED_INIT;
  തിരികെ Curl_pp_sendf(data, &imapc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * imap_cancel_authenticate()
 *
 * Sends SASL cancellation.
 */
സ്ഥിതം CURLcode imap_cancel_authenticate(ഘടന Curl_easy *data,
                                         സ്ഥിരം അക്ഷരം *mech)
{
  ഘടന imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  (ശൂന്യം)mech;
  എങ്കിൽ(!imapc)
    തിരികെ CURLE_FAILED_INIT;
  തിരികെ Curl_pp_sendf(data, &imapc->pp, "*");
}

/***********************************************************************
 *
 * imap_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to clear text should a common
 * mechanism not be available between the client and server.
 */
സ്ഥിതം CURLcode imap_perform_authentication(ഘടന Curl_easy *data,
                                            ഘടന imap_conn *imapc)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  /* Check if already authenticated OR if there is enough data to authenticate
     with and end the connect phase if we do not */
  എങ്കിൽ(imapc->preauth ||
     !Curl_sasl_can_authenticate(&imapc->sasl, data)) {
    imap_state(data, imapc, IMAP_STOP);
    തിരികെ result;
  }

  /* Calculate the SASL login details */
  result = Curl_sasl_start(&imapc->sasl, data, (bool)imapc->ir_supported,
                           &progress);
  എങ്കിൽ(!result) {
    എങ്കിൽ(progress == SASL_INPROGRESS)
      imap_state(data, imapc, IMAP_AUTHENTICATE);
    അല്ലെങ്കിൽ എങ്കിൽ(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
      /* Perform clear text authentication */
      result = imap_perform_login(data, imapc, data->conn);
    അല്ലെങ്കിൽ
      result = Curl_sasl_is_blocked(&imapc->sasl, data);
  }

  തിരികെ result;
}

/***********************************************************************
 *
 * imap_perform_list()
 *
 * Sends a LIST command or an alternative custom request.
 */
സ്ഥിതം CURLcode imap_perform_list(ഘടന Curl_easy *data,
                                  ഘടന imap_conn *imapc,
                                  ഘടന IMAP *imap)
{
  CURLcode result = CURLE_OK;

  എങ്കിൽ(imap->custom)
    /* Send the custom request */
    result = imap_sendf(data, imapc, "%s%s", imap->custom,
                        imap->custom_params ? imap->custom_params : "");
  അല്ലെങ്കിൽ {
    /* Make sure the mailbox is in the correct atom format if necessary */
    അക്ഷരം *mailbox = imap->mailbox ? imap_atom(imap->mailbox, TRUE)
                                  : curlx_strdup("");
    എങ്കിൽ(!mailbox)
      തിരികെ CURLE_OUT_OF_MEMORY;

    /* Send the LIST command */
    result = imap_sendf(data, imapc, "LIST \"%s\" *", mailbox);

    curlx_free(mailbox);
  }

  എങ്കിൽ(!result)
    imap_state(data, imapc, IMAP_LIST);

  തിരികെ result;
}

/***********************************************************************
 *
 * imap_perform_select()
 *
 * Sends a SELECT command to ask the server to change the selected mailbox.
 */
സ്ഥിതം CURLcode imap_perform_select(ഘടന Curl_easy *data,
                                    ഘടന imap_conn *imapc,
                                    ഘടന IMAP *imap)
{
  CURLcode result = CURLE_OK;
  അക്ഷരം *mailbox;

  /* Invalidate old information as we are switching mailboxes */
  curlx_safefree(imapc->mailbox);
  imapc->mb_uidvalidity_set = FALSE;

  /* Check we have a mailbox */
  എങ്കിൽ(!imap->mailbox) {
    failf(data, "Cannot SELECT without a mailbox.");
    തിരികെ CURLE_URL_MALFORMAT;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  എങ്കിൽ(!mailbox)
    തിരികെ CURLE_OUT_OF_MEMORY;

  /* Send the SELECT command */
  result = imap_sendf(data, imapc, "SELECT %s", mailbox);

  curlx_free(mailbox);

  എങ്കിൽ(!result)
    imap_state(data, imapc, IMAP_SELECT);

  തിരികെ result;
}

/***********************************************************************
 *
 * imap_perform_fetch()
 *
 * Sends a FETCH command to initiate the download of a message.
 */
സ്ഥിതം CURLcode imap_perform_fetch(ഘടന Curl_easy *data,
                                   ഘടന imap_conn *imapc,
                                   ഘടന IMAP *imap)
{
  CURLcode result = CURLE_OK;
  /* Check we have a UID */
  എങ്കിൽ(imap->uid) {

    /* Send the FETCH command */
    എങ്കിൽ(imap->partial)
      result = imap_sendf(data, imapc, "UID FETCH %s BODY[%s]<%s>",
                          imap->uid, imap->section ? imap->section : "",
                          imap->partial);
    അല്ലെങ്കിൽ
      result = imap_sendf(data, imapc, "UID FETCH %s BODY[%s]",
                          imap->uid, imap->section ? imap->section : "");
  }
  അല്ലെങ്കിൽ എങ്കിൽ(imap->mindex) {
    /* Send the FETCH command */
    എങ്കിൽ(imap->partial)
      result = imap_sendf(data, imapc, "FETCH %s BODY[%s]<%s>",
                          imap->mindex, imap->section ? imap->section : "",
                          imap->partial);
    അല്ലെങ്കിൽ
      result = imap_sendf(data, imapc, "FETCH %s BODY[%s]",
                          imap->mindex, imap->section ? imap->section : "");
  }
  അല്ലെങ്കിൽ {
    failf(data, "Cannot FETCH without a UID.");
    തിരികെ CURLE_URL_MALFORMAT;
  }
  എങ്കിൽ(!result)
    imap_state(data, imapc, IMAP_FETCH);

  തിരികെ result;
}

/***********************************************************************
 *
 * imap_perform_append()
 *
 * Sends an APPEND command to initiate the upload of a message.
 */
സ്ഥിതം CURLcode imap_perform_append(ഘടന Curl_easy *data,
                                    ഘടന imap_conn *imapc,
                                    ഘടന IMAP *imap)
{
  CURLcode result = CURLE_OK;
  അക്ഷരം *mailbox;
  ഘടന dynbuf flags;

  /* Check we have a mailbox */
  എങ്കിൽ(!imap->mailbox) {
    failf(data, "Cannot APPEND without a mailbox.");
    തിരികെ CURLE_URL_MALFORMAT;
  }

#ifndef CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  എങ്കിൽ(IS_MIME_POST(data)) {
    curl_mimepart *postp = data->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(unsigned പൂർണ്ണസംഖ്യ)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, data->set.headers, 0);
    result = Curl_mime_prepare_headers(data, postp, ശൂന്യ,
                                       ശൂന്യ, MIMESTRATEGY_MAIL);

    എങ്കിൽ(!result && !Curl_checkheaders(data, STRCONST("Mime-Version")))
      result = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    എങ്കിൽ(!result)
      result = Curl_creader_set_mime(data, postp);
    എങ്കിൽ(result)
      തിരികെ result;
  }
  അല്ലെങ്കിൽ
#endif
  {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  /* Check we know the size of the upload. This takes all readers
   * into account. Especially crlf conversions which make the size
   * unpredictable, e.g. -1. */
  data->state.infilesize = Curl_creader_total_length(data);
  എങ്കിൽ(data->state.infilesize < 0) {
    എങ്കിൽ(data->set.crlf)
      failf(data, "Cannot APPEND with CRLF conversion making size unknown");
    അല്ലെങ്കിൽ
      failf(data, "Cannot APPEND with unknown input file size");
    തിരികെ CURLE_UPLOAD_FAILED;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  എങ്കിൽ(!mailbox)
    തിരികെ CURLE_OUT_OF_MEMORY;

  /* Generate flags string and send the APPEND command */
  curlx_dyn_init(&flags, 100);
  എങ്കിൽ(data->set.upload_flags) {
    പൂർണ്ണസംഖ്യ i;
    ഘടന ulbits ulflag[] = {
      { CURLULFLAG_ANSWERED, "Answered" },
      { CURLULFLAG_DELETED, "Deleted" },
      { CURLULFLAG_DRAFT, "Draft" },
      { CURLULFLAG_FLAGGED, "Flagged" },
      { CURLULFLAG_SEEN, "Seen" },
      { 0, ശൂന്യ }
    };

    result = CURLE_OUT_OF_MEMORY;
    എങ്കിൽ(curlx_dyn_add(&flags, " (")) {
      പോകുക cleanup;
    }

    വേണ്ടി(i = 0; ulflag[i].bit; i++) {
      എങ്കിൽ(data->set.upload_flags & ulflag[i].bit &&
         ((curlx_dyn_len(&flags) > 2 && curlx_dyn_add(&flags, " ")) ||
          curlx_dyn_add(&flags, "\\") ||
          curlx_dyn_add(&flags, ulflag[i].flag)))
        പോകുക cleanup;
    }

    എങ്കിൽ(curlx_dyn_add(&flags, ")"))
      പോകുക cleanup;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(curlx_dyn_add(&flags, ""))
    പോകുക cleanup;

  result = imap_sendf(data, imapc, "APPEND %s%s {%" FMT_OFF_T "}",
                      mailbox, curlx_dyn_ptr(&flags), data->state.infilesize);

cleanup:
  curlx_dyn_free(&flags);
  curlx_free(mailbox);

  എങ്കിൽ(!result)
    imap_state(data, imapc, IMAP_APPEND);

  തിരികെ result;
}

/***********************************************************************
 *
 * imap_perform_search()
 *
 * Sends a SEARCH command.
 */
സ്ഥിതം CURLcode imap_perform_തിരയൽ(ഘടന Curl_easy *data,
                                    ഘടന imap_conn *imapc,
                                    ഘടന IMAP *imap)
{
  CURLcode result = CURLE_OK;

  /* Check we have a query string */
  എങ്കിൽ(!imap->query) {
    failf(data, "Cannot SEARCH without a query string.");
    തിരികെ CURLE_URL_MALFORMAT;
  }

  /* Send the SEARCH command */
  result = imap_sendf(data, imapc, "SEARCH %s", imap->query);

  എങ്കിൽ(!result)
    imap_state(data, imapc, IMAP_തിരയൽ);

  തിരികെ result;
}

/***********************************************************************
 *
 * imap_perform_logout()
 *
 * Performs the logout action prior to sclose() being called.
 */
സ്ഥിതം CURLcode imap_perform_logout(ഘടന Curl_easy *data,
                                    ഘടന imap_conn *imapc)
{
  /* Send the LOGOUT command */
  CURLcode result = imap_sendf(data, imapc, "LOGOUT");

  എങ്കിൽ(!result)
    imap_state(data, imapc, IMAP_LOGOUT);

  തിരികെ result;
}

/* For the initial server greeting */
സ്ഥിതം CURLcode imap_state_servergreet_resp(ഘടന Curl_easy *data,
                                            ഘടന imap_conn *imapc,
                                            പൂർണ്ണസംഖ്യ imapcode,
                                            imapstate instate)
{
  (ശൂന്യം)instate;

  എങ്കിൽ(imapcode == IMAP_RESP_PREAUTH) {
    /* PREAUTH */
    imapc->preauth = TRUE;
    infof(data, "PREAUTH connection, already authenticated");
  }
  അല്ലെങ്കിൽ എങ്കിൽ(imapcode != IMAP_RESP_OK) {
    failf(data, "Got unexpected imap-server response");
    തിരികെ CURLE_WEIRD_SERVER_REPLY;
  }

  തിരികെ imap_perform_capability(data, imapc);
}

/* For CAPABILITY responses */
സ്ഥിതം CURLcode imap_state_capability_resp(ഘടന Curl_easy *data,
                                           ഘടന imap_conn *imapc,
                                           പൂർണ്ണസംഖ്യ imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  സ്ഥിരം അക്ഷരം *വരി = curlx_dyn_ptr(&imapc->pp.recvbuf);

  (ശൂന്യം)instate;

  /* Do we have an untagged response? */
  എങ്കിൽ(imapcode == '*') {
    വരി += 2;

    /* Loop through the data line */
    വേണ്ടി(;;) {
      size_t wordlen;
      ആകുമ്പോൾ(*വരി && (ISBLANK(*വരി) || ISNEWLINE(*വരി)))
        വരി++;

      എങ്കിൽ(!*വരി)
        നിർത്തുക;

      /* Extract the word */
      വേണ്ടി(wordlen = 0; വരി[wordlen] && !ISBLANK(വരി[wordlen]) &&
                       !ISNEWLINE(വരി[wordlen]);)
        wordlen++;

      /* Does the server support the STARTTLS capability? */
      എങ്കിൽ(wordlen == 8 && curl_strnequal(വരി, "STARTTLS", 8))
        imapc->tls_supported = TRUE;

      /* Has the server explicitly disabled clear text authentication? */
      അല്ലെങ്കിൽ എങ്കിൽ(wordlen == 13 && curl_strnequal(വരി, "LOGINDISABLED", 13))
        imapc->login_disabled = TRUE;

      /* Does the server support the SASL-IR capability? */
      അല്ലെങ്കിൽ എങ്കിൽ(wordlen == 7 && curl_strnequal(വരി, "SASL-IR", 7))
        imapc->ir_supported = TRUE;

      /* Do we have a SASL based authentication mechanism? */
      അല്ലെങ്കിൽ എങ്കിൽ(wordlen > 5 && curl_strnequal(വരി, "AUTH=", 5)) {
        size_t llen;
        unsigned short mechbit;

        വരി += 5;
        wordlen -= 5;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(വരി, wordlen, &llen);
        എങ്കിൽ(mechbit && llen == wordlen)
          imapc->sasl.authmechs |= mechbit;
      }

      വരി += wordlen;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(data->set.use_ssl && !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
    /* PREAUTH is not compatible with STARTTLS. */
    എങ്കിൽ(imapcode == IMAP_RESP_OK && imapc->tls_supported && !imapc->preauth) {
      /* Switch to TLS connection now */
      result = imap_perform_starttls(data, imapc);
    }
    അല്ലെങ്കിൽ എങ്കിൽ(data->set.use_ssl <= CURLUSESSL_TRY)
      result = imap_perform_authentication(data, imapc);
    അല്ലെങ്കിൽ {
      failf(data, "STARTTLS not available.");
      result = CURLE_USE_SSL_FAILED;
    }
  }
  അല്ലെങ്കിൽ
    result = imap_perform_authentication(data, imapc);

  തിരികെ result;
}

/* For STARTTLS responses */
സ്ഥിതം CURLcode imap_state_starttls_resp(ഘടന Curl_easy *data,
                                         ഘടന imap_conn *imapc,
                                         പൂർണ്ണസംഖ്യ imapcode,
                                         imapstate instate)
{
  CURLcode result = CURLE_OK;

  (ശൂന്യം)instate;

  /* Pipelining in response is forbidden. */
  എങ്കിൽ(imapc->pp.overflow)
    തിരികെ CURLE_WEIRD_SERVER_REPLY;

  എങ്കിൽ(imapcode != IMAP_RESP_OK) {
    എങ്കിൽ(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied");
      result = CURLE_USE_SSL_FAILED;
    }
    അല്ലെങ്കിൽ
      result = imap_perform_authentication(data, imapc);
  }
  അല്ലെങ്കിൽ
    imap_state(data, imapc, IMAP_UPGRADETLS);

  തിരികെ result;
}

/* For SASL authentication responses */
സ്ഥിതം CURLcode imap_state_auth_resp(ഘടന Curl_easy *data,
                                     ഘടന imap_conn *imapc,
                                     പൂർണ്ണസംഖ്യ imapcode,
                                     imapstate instate)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  (ശൂന്യം)instate;

  result = Curl_sasl_continue(&imapc->sasl, data, imapcode, &progress);
  എങ്കിൽ(!result)
    തിരഞ്ഞെടുപ്പ്(progress) {
    അവസ്ഥ SASL_DONE:
      imap_state(data, imapc, IMAP_STOP);  /* Authenticated */
      നിർത്തുക;
    അവസ്ഥ SASL_IDLE:            /* No mechanism left after cancellation */
      എങ്കിൽ(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
        /* Perform clear text authentication */
        result = imap_perform_login(data, imapc, data->conn);
      അല്ലെങ്കിൽ {
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
      }
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      നിർത്തുക;
    }

  തിരികെ result;
}

/* For LOGIN responses */
സ്ഥിതം CURLcode imap_state_login_resp(ഘടന Curl_easy *data,
                                      ഘടന imap_conn *imapc,
                                      പൂർണ്ണസംഖ്യ imapcode,
                                      imapstate instate)
{
  CURLcode result = CURLE_OK;
  (ശൂന്യം)instate;

  എങ്കിൽ(imapcode != IMAP_RESP_OK) {
    failf(data, "Access denied. %c", imapcode);
    result = CURLE_LOGIN_DENIED;
  }
  അല്ലെങ്കിൽ
    /* End of connect phase */
    imap_state(data, imapc, IMAP_STOP);

  തിരികെ result;
}

/* Detect IMAP listings vs. downloading a single email */
സ്ഥിതം bool is_custom_fetch_listing_match(സ്ഥിരം അക്ഷരം *params)
{
  /* match " 1:* (FLAGS ..." or " 1,2,3 (FLAGS ..." */
  എങ്കിൽ(*params++ != ' ')
    തിരികെ FALSE;

  ആകുമ്പോൾ(ISDIGIT(*params)) {
    params++;
    എങ്കിൽ(*params == 0)
      തിരികെ FALSE;
  }
  എങ്കിൽ(*params == ':')
    തിരികെ TRUE;
  എങ്കിൽ(*params == ',')
    തിരികെ TRUE;
  തിരികെ FALSE;
}

സ്ഥിതം bool is_custom_fetch_listing(ഘടന IMAP *imap)
{
  /* filter out "UID FETCH 1:* (FLAGS ..." queries to list emails */
  എങ്കിൽ(!imap->custom)
    തിരികെ FALSE;
  അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal(imap->custom, "FETCH") && imap->custom_params) {
    സ്ഥിരം അക്ഷരം *p = imap->custom_params;
    തിരികെ is_custom_fetch_listing_match(p);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal(imap->custom, "UID") && imap->custom_params &&
          curl_strnequal(imap->custom_params, " FETCH ", 7)) {
    സ്ഥിരം അക്ഷരം *p = imap->custom_params + 6;
    തിരികെ is_custom_fetch_listing_match(p);
  }
  തിരികെ FALSE;
}

/* For LIST and SEARCH responses */
സ്ഥിതം CURLcode imap_state_listsearch_resp(ഘടന Curl_easy *data,
                                           ഘടന imap_conn *imapc,
                                           പൂർണ്ണസംഖ്യ imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  സ്ഥിരം അക്ഷരം *വരി = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  ഘടന IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);

  DEBUGASSERT(imap);
  എങ്കിൽ(!imap)
    തിരികെ CURLE_FAILED_INIT;
  (ശൂന്യം)instate;

  എങ്കിൽ(imapcode == '*' && is_custom_fetch_listing(imap)) {
    /* custom FETCH or UID FETCH for listing is not handled here */
  }
  അല്ലെങ്കിൽ എങ്കിൽ(imapcode == '*') {
    /* Check if this response contains a literal (e.g. FETCH responses with
       body data). Literal syntax is {size}\r\n */
    സ്ഥിരം അക്ഷരം *cr = memchr(വരി, '\r', len);
    size_t വരി_len = cr ? (size_t)(cr - വരി) : len;
    സ്ഥിരം അക്ഷരം *ptr = imap_find_literal(വരി, വരി_len);
    എങ്കിൽ(ptr) {
      curl_off_t size = 0;
      bool parsed = FALSE;
      ptr++;
      എങ്കിൽ(!curlx_str_സംഖ്യ(&ptr, &size, CURL_OFF_T_MAX) &&
         !curlx_str_single(&ptr, '}'))
        parsed = TRUE;

      എങ്കിൽ(parsed) {
        ഘടന pingpong *pp = &imapc->pp;
        size_t buffer_len = curlx_dyn_len(&pp->recvbuf);
        size_t after_header = buffer_len - pp->nfinal;

        /* This is a literal response, setup to receive the body data */
        infof(data, "Found %" FMT_OFF_T " bytes to download", size);

        /* First write the header line */
        result = Curl_client_write(data, CLIENTWRITE_BODY, വരി, len);
        എങ്കിൽ(result)
          തിരികെ result;

        /* Handle data already in buffer after the header line */
        എങ്കിൽ(after_header > 0) {
          /* There is already data in the buffer that is part of the literal
             body or subsequent responses */
          size_t chunk = after_header;

          /* Keep only the data after the header line */
          curlx_dyn_tail(&pp->recvbuf, chunk);
          pp->nfinal = 0; /* done */

          /* Limit chunk to the literal size */
          എങ്കിൽ((curl_off_t)chunk > size)
            chunk = (size_t)size;

          എങ്കിൽ(chunk) {
            /* Write the literal body data */
            result = Curl_client_write(data, CLIENTWRITE_BODY,
                                       curlx_dyn_ptr(&pp->recvbuf), chunk);
            എങ്കിൽ(result)
              തിരികെ result;
          }

          /* Handle remaining data in buffer (either more literal data or
             subsequent responses) */
          എങ്കിൽ(after_header > chunk) {
            /* Keep the data after the literal body */
            pp->overflow = after_header - chunk;
            curlx_dyn_tail(&pp->recvbuf, pp->overflow);
          }
          അല്ലെങ്കിൽ {
            pp->overflow = 0;
            curlx_dyn_reset(&pp->recvbuf);
          }
        }
        അല്ലെങ്കിൽ {
          /* No data in buffer yet, reset overflow */
          pp->overflow = 0;
        }

        എങ്കിൽ((CURL_OFF_T_MAX - size) < (curl_off_t)len)
          /* unlikely to actually be a transfer this big, but avoid integer
             overflow */
          size = CURL_OFF_T_MAX;
        അല്ലെങ്കിൽ
          size += len;

        /* Progress size includes both header line and literal body */
        Curl_pgrsSetDownloadSize(data, size);

        എങ്കിൽ(data->req.bytecount == size)
          /* All data already transferred (header + literal body) */
          Curl_xfer_setup_nop(data);
        അല്ലെങ്കിൽ {
          /* Setup to receive the literal body data.
             maxdownload and transfer size include both header line and
             literal body */
          data->req.maxdownload = size;
          Curl_xfer_setup_recv(data, FIRSTSOCKET, size);
        }
        /* End of DO phase */
        imap_state(data, imapc, IMAP_STOP);
      }
      അല്ലെങ്കിൽ {
        /* Failed to parse literal, write the line */
        result = Curl_client_write(data, CLIENTWRITE_BODY, വരി, len);
      }
    }
    അല്ലെങ്കിൽ {
      /* No literal, write the line as-is */
      result = Curl_client_write(data, CLIENTWRITE_BODY, വരി, len);
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(imapcode != IMAP_RESP_OK)
    result = CURLE_QUOTE_ERROR;
  അല്ലെങ്കിൽ
    /* End of DO phase */
    imap_state(data, imapc, IMAP_STOP);

  തിരികെ result;
}

/* For SELECT responses */
സ്ഥിതം CURLcode imap_state_select_resp(ഘടന Curl_easy *data,
                                       ഘടന imap_conn *imapc,
                                       ഘടന IMAP *imap,
                                       പൂർണ്ണസംഖ്യ imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  (ശൂന്യം)instate;

  എങ്കിൽ(imapcode == '*') {
    /* See if this is an UIDVALIDITY response */
    സ്ഥിരം അക്ഷരം *വരി = curlx_dyn_ptr(&imapc->pp.recvbuf);
    size_t len = curlx_dyn_len(&imapc->pp.recvbuf);
    എങ്കിൽ((len >= 18) && checkprefix("OK [UIDVALIDITY ", &വരി[2])) {
      curl_off_t മൂല്യം;
      സ്ഥിരം അക്ഷരം *p = &വരി[2] + CURL_CSTRLEN("OK [UIDVALIDITY ");
      എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &മൂല്യം, UINT_MAX)) {
        imapc->mb_uidvalidity = (unsigned പൂർണ്ണസംഖ്യ)മൂല്യം;
        imapc->mb_uidvalidity_set = TRUE;
      }
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(imapcode == IMAP_RESP_OK) {
    /* Check if the UIDVALIDITY has been specified and matches */
    എങ്കിൽ(imap->uidvalidity_set && imapc->mb_uidvalidity_set &&
       (imap->uidvalidity != imapc->mb_uidvalidity)) {
      failf(data, "Mailbox UIDVALIDITY has changed");
      result = CURLE_REMOTE_FILE_NOT_FOUND;
    }
    അല്ലെങ്കിൽ {
      /* Note the currently opened mailbox on this connection */
      DEBUGASSERT(!imapc->mailbox);
      imapc->mailbox = curlx_strdup(imap->mailbox);
      എങ്കിൽ(!imapc->mailbox)
        തിരികെ CURLE_OUT_OF_MEMORY;

      എങ്കിൽ(imap->custom)
        result = imap_perform_list(data, imapc, imap);
      അല്ലെങ്കിൽ എങ്കിൽ(imap->query)
        result = imap_perform_തിരയൽ(data, imapc, imap);
      അല്ലെങ്കിൽ
        result = imap_perform_fetch(data, imapc, imap);
    }
  }
  അല്ലെങ്കിൽ {
    failf(data, "Select failed");
    result = CURLE_LOGIN_DENIED;
  }

  തിരികെ result;
}

/* For the (first line of the) FETCH responses */
സ്ഥിതം CURLcode imap_state_fetch_resp(ഘടന Curl_easy *data,
                                      ഘടന imap_conn *imapc,
                                      പൂർണ്ണസംഖ്യ imapcode,
                                      imapstate instate)
{
  CURLcode result = CURLE_OK;
  ഘടന pingpong *pp = &imapc->pp;
  സ്ഥിരം അക്ഷരം *ptr = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  bool parsed = FALSE;
  curl_off_t size = 0;

  (ശൂന്യം)instate;

  എങ്കിൽ(imapcode != '*') {
    Curl_pgrsSetDownloadSize(data, -1);
    imap_state(data, imapc, IMAP_STOP);
    തിരികെ CURLE_REMOTE_FILE_NOT_FOUND;
  }

  /* Something like this is received "* 1 FETCH (BODY[TEXT] {2021}\r" so parse
     the continuation data contained within the curly brackets */
  ptr = imap_find_literal(ptr, len);
  എങ്കിൽ(ptr) {
    ptr++;
    എങ്കിൽ(!curlx_str_സംഖ്യ(&ptr, &size, CURL_OFF_T_MAX) &&
       !curlx_str_single(&ptr, '}'))
      parsed = TRUE;
  }

  എങ്കിൽ(parsed) {
    infof(data, "Found %" FMT_OFF_T " bytes to download", size);
    Curl_pgrsSetDownloadSize(data, size);

    എങ്കിൽ(pp->overflow) {
      /* At this point there is a data in the receive buffer that is body
         content, send it as body and then skip it. Do note that there may
         even be additional "headers" after the body. */
      size_t chunk = pp->overflow;

      /* keep only the overflow */
      curlx_dyn_tail(&pp->recvbuf, chunk);
      pp->nfinal = 0; /* done */

      എങ്കിൽ((curl_off_t)chunk > size)
        /* The conversion from curl_off_t to size_t is always fine here */
        chunk = (size_t)size;

      എങ്കിൽ(!chunk) {
        /* no size, we are done with the data */
        imap_state(data, imapc, IMAP_STOP);
        തിരികെ CURLE_OK;
      }
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 curlx_dyn_ptr(&pp->recvbuf), chunk);
      എങ്കിൽ(result)
        തിരികെ result;

      infof(data, "Written %zu bytes, %" FMT_OFF_T
            " bytes are left for transfer", chunk, (curl_off_t)(size - chunk));

      /* Have we used the entire overflow or part of it?*/
      എങ്കിൽ(pp->overflow > chunk) {
        /* remember the remaining trailing overflow data */
        pp->overflow -= chunk;
        curlx_dyn_tail(&pp->recvbuf, pp->overflow);
      }
      അല്ലെങ്കിൽ {
        pp->overflow = 0; /* handled */
        /* Free the cache */
        curlx_dyn_reset(&pp->recvbuf);
      }
    }

    എങ്കിൽ(data->req.bytecount == size)
      /* The entire data is already transferred! */
      Curl_xfer_setup_nop(data);
    അല്ലെങ്കിൽ {
      /* IMAP download */
      data->req.maxdownload = size;
      Curl_xfer_setup_recv(data, FIRSTSOCKET, size);
    }
  }
  അല്ലെങ്കിൽ {
    /* We do not know how to parse this line */
    failf(data, "Failed to parse FETCH response.");
    result = CURLE_WEIRD_SERVER_REPLY;
  }

  /* End of DO phase */
  imap_state(data, imapc, IMAP_STOP);

  തിരികെ result;
}

/* For final FETCH responses performed after the download */
സ്ഥിതം CURLcode imap_state_fetch_final_resp(ഘടന Curl_easy *data,
                                            ഘടന imap_conn *imapc,
                                            പൂർണ്ണസംഖ്യ imapcode,
                                            imapstate instate)
{
  CURLcode result = CURLE_OK;

  (ശൂന്യം)instate;

  എങ്കിൽ(imapcode != IMAP_RESP_OK)
    result = CURLE_WEIRD_SERVER_REPLY;
  അല്ലെങ്കിൽ
    /* End of DONE phase */
    imap_state(data, imapc, IMAP_STOP);

  തിരികെ result;
}

/* For APPEND responses */
സ്ഥിതം CURLcode imap_state_append_resp(ഘടന Curl_easy *data,
                                       ഘടന imap_conn *imapc,
                                       പൂർണ്ണസംഖ്യ imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  (ശൂന്യം)instate;

  എങ്കിൽ(imapcode != '+') {
    result = CURLE_UPLOAD_FAILED;
  }
  അല്ലെങ്കിൽ {
    /* Set the progress upload size */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* IMAP upload */
    Curl_xfer_setup_send(data, FIRSTSOCKET);

    /* End of DO phase */
    imap_state(data, imapc, IMAP_STOP);
  }

  തിരികെ result;
}

/* For final APPEND responses performed after the upload */
സ്ഥിതം CURLcode imap_state_append_final_resp(ഘടന Curl_easy *data,
                                             ഘടന imap_conn *imapc,
                                             പൂർണ്ണസംഖ്യ imapcode,
                                             imapstate instate)
{
  CURLcode result = CURLE_OK;

  (ശൂന്യം)instate;

  എങ്കിൽ(imapcode != IMAP_RESP_OK)
    result = CURLE_UPLOAD_FAILED;
  അല്ലെങ്കിൽ
    /* End of DONE phase */
    imap_state(data, imapc, IMAP_STOP);

  തിരികെ result;
}

സ്ഥിതം CURLcode imap_pp_statemachine(ഘടന Curl_easy *data,
                                     ഘടന connectdata *conn)
{
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ imapcode;
  ഘടന imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ഘടന IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  ഘടന pingpong *pp;
  size_t nread = 0;

  എങ്കിൽ(!imapc || !imap)
    തിരികെ CURLE_FAILED_INIT;
  pp = &imapc->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not IMAP */
upgrade_tls:
  എങ്കിൽ(imapc->state == IMAP_UPGRADETLS) {
    result = imap_perform_upgrade_tls(data, imapc, conn);
    എങ്കിൽ(result || (imapc->state == IMAP_UPGRADETLS))
      തിരികെ result;
  }

  /* Flush any data that needs to be sent */
  എങ്കിൽ(pp->sendleft)
    തിരികെ Curl_pp_flushsend(data, pp);

  ചെയ്യുക {
    /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, pp, &imapcode, &nread);
    എങ്കിൽ(result)
      തിരികെ result;

    /* Was there an error parsing the response line? */
    എങ്കിൽ(imapcode == -1)
      തിരികെ CURLE_WEIRD_SERVER_REPLY;

    എങ്കിൽ(!imapcode)
      നിർത്തുക;

    /* We have now received a full IMAP server response */
    തിരഞ്ഞെടുപ്പ്(imapc->state) {
    അവസ്ഥ IMAP_SERVERGREET:
      result = imap_state_servergreet_resp(data, imapc,
                                           imapcode, imapc->state);
      നിർത്തുക;

    അവസ്ഥ IMAP_CAPABILITY:
      result = imap_state_capability_resp(data, imapc, imapcode, imapc->state);
      നിർത്തുക;

    അവസ്ഥ IMAP_STARTTLS:
      result = imap_state_starttls_resp(data, imapc, imapcode, imapc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      എങ്കിൽ(!result && (imapc->state == IMAP_UPGRADETLS))
        പോകുക upgrade_tls;
      നിർത്തുക;

    അവസ്ഥ IMAP_AUTHENTICATE:
      result = imap_state_auth_resp(data, imapc, imapcode, imapc->state);
      നിർത്തുക;

    അവസ്ഥ IMAP_LOGIN:
      result = imap_state_login_resp(data, imapc, imapcode, imapc->state);
      നിർത്തുക;

    അവസ്ഥ IMAP_LIST:
    അവസ്ഥ IMAP_തിരയൽ:
      result = imap_state_listsearch_resp(data, imapc, imapcode, imapc->state);
      നിർത്തുക;

    അവസ്ഥ IMAP_SELECT:
      result = imap_state_select_resp(data, imapc, imap,
                                      imapcode, imapc->state);
      നിർത്തുക;

    അവസ്ഥ IMAP_FETCH:
      result = imap_state_fetch_resp(data, imapc, imapcode, imapc->state);
      നിർത്തുക;

    അവസ്ഥ IMAP_FETCH_FINAL:
      result = imap_state_fetch_final_resp(data, imapc,
                                           imapcode, imapc->state);
      നിർത്തുക;

    അവസ്ഥ IMAP_APPEND:
      result = imap_state_append_resp(data, imapc, imapcode, imapc->state);
      നിർത്തുക;

    അവസ്ഥ IMAP_APPEND_FINAL:
      result = imap_state_append_final_resp(data, imapc,
                                            imapcode, imapc->state);
      നിർത്തുക;

    അവസ്ഥ IMAP_LOGOUT:
    സ്ഥിരസ്ഥിതി:
      /* internal error */
      imap_state(data, imapc, IMAP_STOP);
      നിർത്തുക;
    }
  } ആകുമ്പോൾ(!result && imapc->state != IMAP_STOP && Curl_pp_moredata(pp));

  തിരികെ result;
}

/* Called repeatedly until done from multi.c */
സ്ഥിതം CURLcode imap_multi_statemach(ഘടന Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ഘടന imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  *done = FALSE;
  എങ്കിൽ(!imapc)
    തിരികെ CURLE_FAILED_INIT;
  result = Curl_pp_statemach(data, &imapc->pp, FALSE, FALSE);
  *done = (imapc->state == IMAP_STOP);

  തിരികെ result;
}

സ്ഥിതം CURLcode imap_block_statemach(ഘടന Curl_easy *data,
                                     ഘടന imap_conn *imapc,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;

  ആകുമ്പോൾ(imapc->state != IMAP_STOP && !result)
    result = Curl_pp_statemach(data, &imapc->pp, TRUE, disconnecting);

  തിരികെ result;
}

/* For the IMAP "protocol connect" and "doing" phases only */
സ്ഥിതം CURLcode imap_pollset(ഘടന Curl_easy *data,
                             ഘടന easy_pollset *ps)
{
  ഘടന imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  തിരികെ imapc ? Curl_pp_pollset(data, &imapc->pp, ps) : CURLE_OK;
}

സ്ഥിതം ശൂന്യം imap_easy_reset(ഘടന IMAP *imap)
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
സ്ഥിതം bool imap_is_bchar(അക്ഷരം ch)
{
  /* Performing the alnum check first with macro is faster because of ASCII
     arithmetic */
  തിരികെ ch && (ISALNUM(ch) || strchr(":@/&=-._~!$\'()*+,%", ch));
}

/***********************************************************************
 *
 * imap_parse_url_options()
 *
 * Parse the URL login options.
 */
സ്ഥിതം CURLcode imap_parse_url_options(ഘടന connectdata *conn,
                                       ഘടന imap_conn *imapc)
{
  CURLcode result = CURLE_OK;
  സ്ഥിരം അക്ഷരം *ptr = conn->options;
  bool prefer_login = FALSE;

  ആകുമ്പോൾ(!result && ptr && *ptr) {
    സ്ഥിരം അക്ഷരം *താക്കോൽ = ptr;
    സ്ഥിരം അക്ഷരം *മൂല്യം;

    ആകുമ്പോൾ(*ptr && *ptr != '=')
      ptr++;

    മൂല്യം = ptr + 1;

    ആകുമ്പോൾ(*ptr && *ptr != ';')
      ptr++;

    എങ്കിൽ(curl_strnequal(താക്കോൽ, "AUTH=+LOGIN", 11)) {
      /* User prefers plaintext LOGIN over any SASL, including SASL LOGIN */
      prefer_login = TRUE;
      imapc->sasl.prefmech = SASL_AUTH_NONE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(curl_strnequal(താക്കോൽ, "AUTH=", 5)) {
      prefer_login = FALSE;
      result = Curl_sasl_parse_url_auth_option(&imapc->sasl,
                                               മൂല്യം, ptr - മൂല്യം);
    }
    അല്ലെങ്കിൽ {
      prefer_login = FALSE;
      result = CURLE_URL_MALFORMAT;
    }

    എങ്കിൽ(*ptr == ';')
      ptr++;
  }

  എങ്കിൽ(prefer_login)
    imapc->preftype = IMAP_TYPE_CLEARTEXT;
  അല്ലെങ്കിൽ {
    തിരഞ്ഞെടുപ്പ്(imapc->sasl.prefmech) {
    അവസ്ഥ SASL_AUTH_NONE:
      imapc->preftype = IMAP_TYPE_NONE;
      നിർത്തുക;
    അവസ്ഥ SASL_AUTH_DEFAULT:
      imapc->preftype = IMAP_TYPE_ANY;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      imapc->preftype = IMAP_TYPE_SASL;
      നിർത്തുക;
    }
  }

  തിരികെ result;
}

/***********************************************************************
 *
 * imap_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
സ്ഥിതം CURLcode imap_parse_url_path(ഘടന Curl_easy *data,
                                    ഘടന IMAP *imap)
{
  /* The imap struct is already initialized in imap_connect() */
  CURLcode result = CURLE_OK;
  സ്ഥിരം അക്ഷരം *begin = &data->state.up.path[1]; /* skip leading slash */
  സ്ഥിരം അക്ഷരം *ptr = begin;

  /* See how much of the URL is a valid path and decode it */
  ആകുമ്പോൾ(imap_is_bchar(*ptr))
    ptr++;

  എങ്കിൽ(ptr != begin) {
    /* Remove the trailing slash if present */
    സ്ഥിരം അക്ഷരം *end = ptr;
    എങ്കിൽ(end > begin && end[-1] == '/')
      end--;

    result = Curl_urldecode(begin, end - begin, &imap->mailbox, ശൂന്യ,
                            REJECT_CTRL);
    എങ്കിൽ(result)
      തിരികെ result;
  }
  അല്ലെങ്കിൽ
    imap->mailbox = ശൂന്യ;

  /* There can be any number of parameters in the form ";NAME=VALUE" */
  ആകുമ്പോൾ(*ptr == ';') {
    അക്ഷരം *name;
    അക്ഷരം *മൂല്യം;
    size_t valuelen;

    /* Find the length of the name parameter */
    begin = ++ptr;
    ആകുമ്പോൾ(*ptr && *ptr != '=')
      ptr++;

    എങ്കിൽ(!*ptr)
      തിരികെ CURLE_URL_MALFORMAT;

    /* Decode the name parameter */
    result = Curl_urldecode(begin, ptr - begin, &name, ശൂന്യ,
                            REJECT_CTRL);
    എങ്കിൽ(result)
      തിരികെ result;

    /* Find the length of the value parameter */
    begin = ++ptr;
    ആകുമ്പോൾ(imap_is_bchar(*ptr))
      ptr++;

    /* Decode the value parameter */
    result = Curl_urldecode(begin, ptr - begin, &മൂല്യം, &valuelen,
                            REJECT_CTRL);
    എങ്കിൽ(result) {
      curlx_free(name);
      തിരികെ result;
    }

    DEBUGF(infof(data, "IMAP URL parameter '%s' = '%s'", name, മൂല്യം));

    /* Process the known hierarchical parameters (UIDVALIDITY, UID, SECTION
       and PARTIAL) stripping of the trailing slash character if it is
       present.

       Note: Unknown parameters trigger a URL_MALFORMAT error. */
    എങ്കിൽ(valuelen > 0 && മൂല്യം[valuelen - 1] == '/')
      മൂല്യം[valuelen - 1] = '\0';
    എങ്കിൽ(valuelen) {
      എങ്കിൽ(curl_strequal(name, "UIDVALIDITY") && !imap->uidvalidity_set) {
        curl_off_t അക്കം;
        സ്ഥിരം അക്ഷരം *p = (സ്ഥിരം അക്ഷരം *)മൂല്യം;
        എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &അക്കം, UINT_MAX)) {
          imap->uidvalidity = (unsigned പൂർണ്ണസംഖ്യ)അക്കം;
          imap->uidvalidity_set = TRUE;
        }
        curlx_free(മൂല്യം);
      }
      അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal(name, "UID") && !imap->uid) {
        imap->uid = മൂല്യം;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal(name, "MAILINDEX") && !imap->mindex) {
        imap->mindex = മൂല്യം;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal(name, "SECTION") && !imap->section) {
        imap->section = മൂല്യം;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal(name, "PARTIAL") && !imap->partial) {
        imap->partial = മൂല്യം;
      }
      അല്ലെങ്കിൽ {
        curlx_free(name);
        curlx_free(മൂല്യം);
        തിരികെ CURLE_URL_MALFORMAT;
      }
    }
    അല്ലെങ്കിൽ
      /* blank? */
      curlx_free(മൂല്യം);
    curlx_free(name);
  }

  /* Does the URL contain a query parameter? Only valid when we have a mailbox
     and no UID as per RFC-5092 */
  എങ്കിൽ(imap->mailbox && !imap->uid && !imap->mindex) {
    /* Get the query parameter, URL decoded */
    CURLUcode uc = curl_url_get(data->state.uh, CURLUPART_QUERY, &imap->query,
                                CURLU_URLDECODE);
    എങ്കിൽ(uc == CURLUE_OUT_OF_MEMORY)
      തിരികെ CURLE_OUT_OF_MEMORY;
  }

  /* Any extra stuff at the end of the URL is an error */
  എങ്കിൽ(*ptr)
    തിരികെ CURLE_URL_MALFORMAT;

  തിരികെ CURLE_OK;
}

/***********************************************************************
 *
 * imap_parse_custom_request()
 *
 * Parse the custom request.
 */
സ്ഥിതം CURLcode imap_parse_custom_request(ഘടന Curl_easy *data,
                                          ഘടന IMAP *imap)
{
  CURLcode result = CURLE_OK;
  സ്ഥിരം അക്ഷരം *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  എങ്കിൽ(custom) {
    /* URL decode the custom request */
    result = Curl_urldecode(custom, 0, &imap->custom, ശൂന്യ, REJECT_CTRL);

    /* Extract the parameters if specified */
    എങ്കിൽ(!result) {
      സ്ഥിരം അക്ഷരം *params = imap->custom;

      ആകുമ്പോൾ(*params && *params != ' ')
        params++;

      എങ്കിൽ(*params) {
        imap->custom_params = curlx_strdup(params);
        imap->custom[params - imap->custom] = '\0';

        എങ്കിൽ(!imap->custom_params)
          result = CURLE_OUT_OF_MEMORY;
      }
    }
  }

  തിരികെ result;
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
സ്ഥിതം CURLcode imap_connect(ഘടന Curl_easy *data, bool *done)
{
  ഘടന imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  CURLcode result = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  എങ്കിൽ(!imapc)
    തിരികെ CURLE_FAILED_INIT;

  /* Parse the URL options */
  result = imap_parse_url_options(data->conn, imapc);
  എങ്കിൽ(result)
    തിരികെ result;

  /* Start off waiting for the server greeting response */
  imap_state(data, imapc, IMAP_SERVERGREET);

  /* Start off with an response id of '*' */
  curlx_strcopy(imapc->resptag, വലുപ്പം(imapc->resptag), STRCONST("*"));

  result = imap_multi_statemach(data, done);

  തിരികെ result;
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
സ്ഥിതം CURLcode imap_done(ഘടന Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;
  ഘടന imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ഘടന IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);

  എങ്കിൽ(!imapc)
    തിരികെ CURLE_FAILED_INIT;
  എങ്കിൽ(!imap)
    തിരികെ CURLE_OK;

  എങ്കിൽ(status || premature) {
    CURL_TRC_M(data, "IMAP done, not finalizing transfer");
    connclose(conn); /* marked for closure */
    result = status;         /* use the already set error code */
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!data->set.connect_only &&
          ((!imap->custom && (imap->uid || imap->mindex)) ||
           (imap->custom && data->req.maxdownload > 0) ||
           data->state.upload || IS_MIME_POST(data))) {
    /* Handle responses after FETCH or APPEND transfer has finished.
       For custom commands, check if we set up a download which indicates
       a FETCH-like command with literal data. */

    എങ്കിൽ(!data->state.upload && !IS_MIME_POST(data))
      imap_state(data, imapc, IMAP_FETCH_FINAL);
    അല്ലെങ്കിൽ {
      /* End the APPEND command first by sending an empty line */
      result = Curl_pp_sendf(data, &imapc->pp, "%s", "");
      എങ്കിൽ(!result)
        imap_state(data, imapc, IMAP_APPEND_FINAL);
    }

    /* Run the state-machine */
    എങ്കിൽ(!result)
      result = imap_block_statemach(data, imapc, FALSE);
  }

  imap_easy_reset(imap);
  തിരികെ result;
}

/***********************************************************************
 *
 * imap_perform()
 *
 * This is the actual DO function for IMAP. Fetch or append a message, or do
 * other things according to the options previously setup.
 */
സ്ഥിതം CURLcode imap_perform(ഘടന Curl_easy *data, bool *connected,
                             bool *dophase_done)
{
  /* This is IMAP and no proxy */
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;
  ഘടന imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  ഘടന IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  bool selected = FALSE;

  DEBUGF(infof(data, "DO phase starts"));
  എങ്കിൽ(!imapc || !imap)
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ(data->req.no_body) {
    /* Requested no body means no transfer */
    imap->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* Determine if the requested mailbox (with the same UIDVALIDITY if set)
     has already been selected on this connection */
  എങ്കിൽ(imap->mailbox && imapc->mailbox &&
     (!strcmp(imap->mailbox, imapc->mailbox) ||
      (curl_strequal(imap->mailbox, "INBOX") &&
       curl_strequal(imapc->mailbox, "INBOX"))) &&
     (!imap->uidvalidity_set || !imapc->mb_uidvalidity_set ||
      (imap->uidvalidity == imapc->mb_uidvalidity)))
    selected = TRUE;

  /* Start the first command in the DO phase */
  എങ്കിൽ(data->state.upload || IS_MIME_POST(data))
    /* APPEND can be executed directly */
    result = imap_perform_append(data, imapc, imap);
  അല്ലെങ്കിൽ എങ്കിൽ(imap->custom && (selected || !imap->mailbox))
    /* Custom command using the same mailbox or no mailbox */
    result = imap_perform_list(data, imapc, imap);
  അല്ലെങ്കിൽ എങ്കിൽ(!imap->custom && selected && (imap->uid || imap->mindex))
    /* FETCH from the same mailbox */
    result = imap_perform_fetch(data, imapc, imap);
  അല്ലെങ്കിൽ എങ്കിൽ(!imap->custom && selected && imap->query)
    /* SEARCH the current mailbox */
    result = imap_perform_തിരയൽ(data, imapc, imap);
  അല്ലെങ്കിൽ എങ്കിൽ(imap->mailbox && !selected &&
          (imap->custom || imap->uid || imap->mindex || imap->query))
    /* SELECT the mailbox */
    result = imap_perform_select(data, imapc, imap);
  അല്ലെങ്കിൽ
    /* LIST */
    result = imap_perform_list(data, imapc, imap);

  എങ്കിൽ(result)
    തിരികെ result;

  /* Run the state-machine */
  result = imap_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(conn, FIRSTSOCKET);

  എങ്കിൽ(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  തിരികെ result;
}

/* Call this when the DO phase has completed */
സ്ഥിതം CURLcode imap_dophase_done(ഘടന Curl_easy *data,
                                  ഘടന IMAP *imap,
                                  bool connected)
{
  (ശൂന്യം)connected;

  എങ്കിൽ(imap->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);

  തിരികെ CURLE_OK;
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
സ്ഥിതം CURLcode imap_regular_transfer(ഘടന Curl_easy *data,
                                      ഘടന IMAP *imap,
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
  എങ്കിൽ(!result && *dophase_done)
    result = imap_dophase_done(data, imap, connected);

  തിരികെ result;
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
സ്ഥിതം CURLcode imap_do(ഘടന Curl_easy *data, bool *done)
{
  ഘടന IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* default to false */

  എങ്കിൽ(!imap)
    തിരികെ CURLE_FAILED_INIT;
  /* Parse the URL path */
  result = imap_parse_url_path(data, imap);
  എങ്കിൽ(result)
    തിരികെ result;

  /* Parse the custom request */
  result = imap_parse_custom_request(data, imap);
  എങ്കിൽ(result)
    തിരികെ result;

  result = imap_regular_transfer(data, imap, done);

  തിരികെ result;
}

/***********************************************************************
 *
 * imap_disconnect()
 *
 * Disconnect from an IMAP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
സ്ഥിതം CURLcode imap_disconnect(ഘടന Curl_easy *data,
                                ഘടന connectdata *conn, bool dead_connection)
{
  ഘടന imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);

  എങ്കിൽ(imapc &&
     /* We cannot send quit unconditionally. If this connection is stale or
        bad in any way (pingpong has pending data to send),
        sending quit and waiting around here will make the
        disconnect wait in vain and cause more problems than we need to. */
     !dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &imapc->pp) &&
     !imap_perform_logout(data, imapc))
    (ശൂന്യം)imap_block_statemach(data, imapc, TRUE); /* ignore errors */

  തിരികെ CURLE_OK;
}

/* Called from multi.c while DOing */
സ്ഥിതം CURLcode imap_doing(ഘടന Curl_easy *data, bool *dophase_done)
{
  ഘടന IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  CURLcode result;

  എങ്കിൽ(!imap)
    തിരികെ CURLE_FAILED_INIT;

  result = imap_multi_statemach(data, dophase_done);
  എങ്കിൽ(result)
    DEBUGF(infof(data, "DO phase failed"));
  അല്ലെങ്കിൽ എങ്കിൽ(*dophase_done) {
    result = imap_dophase_done(data, imap, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  തിരികെ result;
}

സ്ഥിതം ശൂന്യം imap_easy_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന IMAP *imap = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  imap_easy_reset(imap);
  curlx_free(imap);
}

സ്ഥിതം ശൂന്യം imap_conn_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന imap_conn *imapc = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  Curl_pp_disconnect(&imapc->pp);
  curlx_safefree(imapc->mailbox);
  curlx_free(imapc);
}

/* SASL parameters for the imap protocol */
സ്ഥിതം സ്ഥിരം ഘടന SASLproto saslimap = {
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

സ്ഥിതം CURLcode imap_setup_connection(ഘടന Curl_easy *data,
                                      ഘടന connectdata *conn)
{
  ഘടന imap_conn *imapc;
  ഘടന pingpong *pp;
  ഘടന IMAP *imap;

  imapc = curlx_calloc(1, വലുപ്പം(*imapc));
  എങ്കിൽ(!imapc)
    തിരികെ CURLE_OUT_OF_MEMORY;

  pp = &imapc->pp;
  PINGPONG_SETUP(pp, imap_pp_statemachine, imap_endofresp);

  /* Set the default preferred authentication type and mechanism */
  imapc->preftype = IMAP_TYPE_ANY;
  Curl_sasl_init(&imapc->sasl, data, &saslimap);

  Curl_pp_init(pp, Curl_pgrs_now(data));

  എങ്കിൽ(Curl_conn_meta_set(conn, CURL_META_IMAP_CONN, imapc, imap_conn_dtor))
    തിരികെ CURLE_OUT_OF_MEMORY;

  imap = curlx_calloc(1, വലുപ്പം(ഘടന IMAP));
  എങ്കിൽ(!imap ||
     Curl_meta_set(data, CURL_META_IMAP_EASY, imap, imap_easy_dtor))
    തിരികെ CURLE_OUT_OF_MEMORY;

  തിരികെ CURLE_OK;
}

/*
 * IMAP protocol.
 */
സ്ഥിരം ഘടന Curl_protocol Curl_protocol_imap = {
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
