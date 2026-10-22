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
#அந்தர்பாவ "curl_setup.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "imap.h"

#ifndef CURL_DISABLE_IMAP

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef __VMS
#அந்தர்பாவ <in.h>
#அந்தர்பாவ <inet.h>
#endif

#அந்தர்பாவ "curlx/dynbuf.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "escape.h"
#அந்தர்பாவ "pingpong.h"
#அந்தர்பாவ "mime.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "strcase.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "bufref.h"
#அந்தர்பாவ "curl_sasl.h"
#அந்தர்பாவ "curlx/strcopy.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_IMAP_EASY   "meta:proto:imap:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_IMAP_CONN   "meta:proto:imap:conn"

வகைவரை எண்ணிக்கை {
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
  IMAP_தேடல்,
  IMAP_LOGOUT,
  IMAP_LAST          /* never used */
} imapstate;

/* imap_conn is used for struct connection-oriented data */
கட்டமைப்பு imap_conn {
  கட்டமைப்பு pingpong pp;
  கட்டமைப்பு SASL sasl;           /* SASL-related parameters */
  எழுத்து *mailbox;              /* The last selected mailbox */
  imapstate state;            /* Always use imap.c:state() to change state! */
  unsigned முழு mb_uidvalidity; /* UIDVALIDITY parsed from select response */
  எழுத்து resptag[5];            /* Response tag to wait for */
  unsigned எழுத்து preftype;     /* Preferred authentication type */
  unsigned எழுத்து cmdid;        /* Last used command ID */
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
கட்டமைப்பு IMAP {
  curl_pp_transfer transfer;
  எழுத்து *mailbox;          /* Mailbox to select */
  எழுத்து *uid;              /* Message UID to fetch */
  எழுத்து *mindex;           /* Index in mail box of mail to fetch */
  எழுத்து *section;          /* Message SECTION to fetch */
  எழுத்து *partial;          /* Message PARTIAL to fetch */
  எழுத்து *query;            /* Query to search for */
  எழுத்து *custom;           /* Custom request */
  எழுத்து *custom_params;    /* Parameters for the custom request */
  unsigned முழு uidvalidity; /* UIDVALIDITY to check in select */
  BIT(uidvalidity_set);
};

#define IMAP_RESP_OK       1
#define IMAP_RESP_NOT_OK   2
#define IMAP_RESP_PREAUTH  3

கட்டமைப்பு ulbits {
  முழு bit;
  மாறிலி எழுத்து *flag;
};

/***********************************************************************
 *
 * imap_sendf()
 *
 * Sends the formatted string as an IMAP command to the server.
 *
 * Designed to never block.
 */
நிலையான CURLcode imap_sendf(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு imap_conn *imapc,
                           மாறிலி எழுத்து *fmt, ...) CURL_PRINTF(3, 4);
நிலையான CURLcode imap_sendf(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு imap_conn *imapc,
                           மாறிலி எழுத்து *fmt, ...)
{
  CURLcode result = CURLE_OK;
  va_list ap;

  DEBUGASSERT(fmt);

  /* Calculate the tag based on the connection ID and command ID */
  curl_msnprintf(imapc->resptag, அளவு(imapc->resptag), "%c%03d",
                 'A' + curlx_sltosi((long)(data->conn->connection_id % 26)),
                 ++imapc->cmdid);

  va_start(ap, fmt);
  result = Curl_pp_vsendf(data, &imapc->pp, imapc->resptag, fmt, ap);
  va_end(ap);
  திரும்பு result;
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
நிலையான எழுத்து *imap_atom(மாறிலி எழுத்து *str, bool escape_only)
{
  கட்டமைப்பு dynbuf வரி;
  size_t nclean;
  size_t len;

  எனில்(!str)
    திரும்பு வெற்று;

  len = strlen(str);
  nclean = strcspn(str, "() {%*]\\\"");
  எனில்(len == nclean)
    /* nothing to escape, return a strdup */
    திரும்பு curlx_strdup(str);

  curlx_dyn_init(&வரி, 2000);

  எனில்(!escape_only && curlx_dyn_addn(&வரி, "\"", 1))
    திரும்பு வெற்று;

  போது(*str) {
    எனில்((*str == '\\' || *str == '"') &&
       curlx_dyn_addn(&வரி, "\\", 1))
      திரும்பு வெற்று;
    எனில்(curlx_dyn_addn(&வரி, str, 1))
      திரும்பு வெற்று;
    str++;
  }

  எனில்(!escape_only && curlx_dyn_addn(&வரி, "\"", 1))
    திரும்பு வெற்று;

  திரும்பு curlx_dyn_ptr(&வரி);
}

/*
 * Finds the start of a literal '{size}' in line, skipping over quoted strings.
 */
நிலையான மாறிலி எழுத்து *imap_find_literal(மாறிலி எழுத்து *வரி, size_t len)
{
  மாறிலி எழுத்து *end = வரி + len;
  bool in_quote = FALSE;

  போது(வரி < end) {
    எனில்(in_quote) {
      எனில்(*வரி == '\\' && (வரி + 1) < end) {
        வரி += 2;
        தொடர்;
      }
      எனில்(*வரி == '"')
        in_quote = FALSE;
    }
    இல்லையெனில் {
      எனில்(*வரி == '"')
        in_quote = TRUE;
      இல்லையெனில் எனில்(*வரி == '{')
        திரும்பு வரி;
    }
    வரி++;
  }
  திரும்பு வெற்று;
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
நிலையான bool imap_matchresp(மாறிலி எழுத்து *வரி, size_t len, மாறிலி எழுத்து *cmd)
{
  மாறிலி எழுத்து *end = வரி + len;
  size_t cmd_len = strlen(cmd);

  /* Skip the untagged response marker */
  வரி += 2;

  /* Do we have a number after the marker? */
  எனில்(வரி < end && ISDIGIT(*வரி)) {
    /* Skip the number */
    செய்
      வரி++;
    போது(வரி < end && ISDIGIT(*வரி));

    /* Do we have the space character? */
    எனில்(வரி == end || *வரி != ' ')
      திரும்பு FALSE;

    வரி++;
  }

  /* Does the command name match and is it followed by a space character or at
     the end of line? */
  எனில்(வரி + cmd_len <= end && curl_strnequal(வரி, cmd, cmd_len) &&
     (வரி[cmd_len] == ' ' || வரி + cmd_len + 2 == end))
    திரும்பு TRUE;

  திரும்பு FALSE;
}

/***********************************************************************
 *
 * imap_endofresp()
 *
 * Checks whether the given string is a valid tagged, untagged or continuation
 * response which can be processed by the response handler.
 */
நிலையான bool imap_endofresp(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு connectdata *conn,
                           மாறிலி எழுத்து *வரி, size_t len, முழு *resp)
{
  கட்டமைப்பு imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  கட்டமைப்பு IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  மாறிலி எழுத்து *id;
  size_t id_len;

  DEBUGASSERT(imapc);
  DEBUGASSERT(imap);
  எனில்(!imapc || !imap)
    திரும்பு FALSE;

  /* Do we have a tagged command response? */
  id = imapc->resptag;
  id_len = strlen(id);
  எனில்(len >= id_len + 1 && !memcmp(id, வரி, id_len) && வரி[id_len] == ' ') {
    வரி += id_len + 1;
    len -= id_len + 1;

    எனில்(len >= 2 && !memcmp(வரி, "OK", 2))
      *resp = IMAP_RESP_OK;
    இல்லையெனில் எனில்(len >= 7 && !memcmp(வரி, "PREAUTH", 7))
      *resp = IMAP_RESP_PREAUTH;
    இல்லையெனில்
      *resp = IMAP_RESP_NOT_OK;

    திரும்பு TRUE;
  }

  /* Do we have an untagged command response? */
  எனில்(len >= 2 && !memcmp("* ", வரி, 2)) {
    தேர்வு(imapc->state) {
    /* States which are interested in untagged responses */
    நிலை IMAP_CAPABILITY:
      எனில்(!imap_matchresp(வரி, len, "CAPABILITY"))
        திரும்பு FALSE;
      நிறுத்து;

    நிலை IMAP_LIST:
      எனில்((!imap->custom && !imap_matchresp(வரி, len, "LIST")) ||
         (imap->custom && !imap_matchresp(வரி, len, imap->custom) &&
          (!curl_strequal(imap->custom, "STORE") ||
           !imap_matchresp(வரி, len, "FETCH")) &&
          !curl_strequal(imap->custom, "SELECT") &&
          !curl_strequal(imap->custom, "EXAMINE") &&
          !curl_strequal(imap->custom, "SEARCH") &&
          !curl_strequal(imap->custom, "EXPUNGE") &&
          !curl_strequal(imap->custom, "LSUB") &&
          !curl_strequal(imap->custom, "UID") &&
          !curl_strequal(imap->custom, "GETQUOTAROOT") &&
          !curl_strequal(imap->custom, "NOOP")))
        திரும்பு FALSE;
      நிறுத்து;

    நிலை IMAP_SELECT:
      /* SELECT is special in that its untagged responses do not have a
         common prefix so accept anything! */
      நிறுத்து;

    நிலை IMAP_FETCH:
      எனில்(!imap_matchresp(வரி, len, "FETCH"))
        திரும்பு FALSE;
      நிறுத்து;

    நிலை IMAP_தேடல்:
      எனில்(!imap_matchresp(வரி, len, "SEARCH"))
        திரும்பு FALSE;
      நிறுத்து;

    /* Ignore other untagged responses */
    இயல்பு:
      திரும்பு FALSE;
    }

    *resp = '*';
    திரும்பு TRUE;
  }

  /* Do we have a continuation response? This should be a + symbol followed by
     a space and optionally some text as per RFC-3501 for the AUTHENTICATE and
     APPEND commands and as outlined in Section 4. Examples of RFC-4959 but
     some email servers ignore this and only send a single + instead. */
  எனில்(!imap->custom && ((len == 3 && வரி[0] == '+') ||
                       (len >= 2 && !memcmp("+ ", வரி, 2)))) {
    தேர்வு(imapc->state) {
    /* States which are interested in continuation responses */
    நிலை IMAP_AUTHENTICATE:
    நிலை IMAP_APPEND:
      *resp = '+';
      நிறுத்து;

    இயல்பு:
      failf(data, "Unexpected continuation response");
      *resp = -1;
      நிறுத்து;
    }

    திரும்பு TRUE;
  }

  திரும்பு FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * imap_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
நிலையான CURLcode imap_get_message(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு bufref *out)
{
  கட்டமைப்பு imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  எழுத்து *message;
  size_t len;

  எனில்(!imapc)
    திரும்பு CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&imapc->pp.recvbuf);
  len = imapc->pp.nfinal;
  எனில்(len > 2) {
    /* Find the start of the message */
    len -= 2;
    க்கு(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    போது(len--)
      எனில்(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        நிறுத்து;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, வெற்று);
  }
  இல்லையெனில்
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, வெற்று);

  திரும்பு CURLE_OK;
}

/***********************************************************************
 *
 * imap_state()
 *
 * This is the ONLY way to change IMAP state!
 */
நிலையான வெறுமை imap_state(கட்டமைப்பு Curl_easy *data,
                       கட்டமைப்பு imap_conn *imapc,
                       imapstate newstate)
{
#எனில் defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  நிலையான மாறிலி எழுத்து * மாறிலி names[] = {
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

  எனில்(imapc->state != newstate)
    infof(data, "IMAP %p state change from %s to %s",
          (வெறுமை *)imapc, names[imapc->state], names[newstate]);
#இல்லையெனில்
  (வெறுமை)data;
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
நிலையான CURLcode imap_perform_capability(கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு imap_conn *imapc)
{
  CURLcode result = CURLE_OK;

  imapc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  imapc->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  imapc->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPABILITY command */
  result = imap_sendf(data, imapc, "CAPABILITY");

  எனில்(!result)
    imap_state(data, imapc, IMAP_CAPABILITY);

  திரும்பு result;
}

/***********************************************************************
 *
 * imap_perform_starttls()
 *
 * Sends the STARTTLS command to start the upgrade to TLS.
 */
நிலையான CURLcode imap_perform_starttls(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு imap_conn *imapc)
{
  /* Send the STARTTLS command */
  CURLcode result = imap_sendf(data, imapc, "STARTTLS");

  எனில்(!result)
    imap_state(data, imapc, IMAP_STARTTLS);

  திரும்பு result;
}

/***********************************************************************
 *
 * imap_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
நிலையான CURLcode imap_perform_upgrade_tls(கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு imap_conn *imapc,
                                         கட்டமைப்பு connectdata *conn)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  CURLcode result;
  bool ssldone = FALSE;

  எனில்(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    எனில்(result)
      செல் out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_imaps;
  }

  DEBUGASSERT(!imapc->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "imap_perform_upgrade_tls, connect -> %d, %d",
               (முழு)result, ssldone));
  எனில்(!result && ssldone) {
    imapc->ssldone = ssldone;
    /* perform CAPA now, changes imapc->state out of IMAP_UPGRADETLS */
    result = imap_perform_capability(data, imapc);
  }
out:
  திரும்பு result;
#இல்லையெனில்
  (வெறுமை)data;
  (வெறுமை)imapc;
  (வெறுமை)conn;
  திரும்பு CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * imap_perform_login()
 *
 * Sends a clear text LOGIN command to authenticate with.
 */
நிலையான CURLcode imap_perform_login(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு imap_conn *imapc,
                                   கட்டமைப்பு connectdata *conn)
{
  CURLcode result = CURLE_OK;
  எழுத்து *user;
  எழுத்து *passwd;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  எனில்(!conn->creds) {
    imap_state(data, imapc, IMAP_STOP);

    திரும்பு result;
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

  எனில்(!result)
    imap_state(data, imapc, IMAP_LOGIN);

  திரும்பு result;
}

/***********************************************************************
 *
 * imap_perform_authenticate()
 *
 * Sends an AUTHENTICATE command allowing the client to login with the given
 * SASL authentication mechanism.
 */
நிலையான CURLcode imap_perform_authenticate(கட்டமைப்பு Curl_easy *data,
                                          மாறிலி எழுத்து *mech,
                                          மாறிலி கட்டமைப்பு bufref *initresp)
{
  கட்டமைப்பு imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  CURLcode result = CURLE_OK;
  மாறிலி எழுத்து *ir = Curl_bufref_ptr(initresp);

  எனில்(!imapc)
    திரும்பு CURLE_FAILED_INIT;
  எனில்(ir) {
    /* Send the AUTHENTICATE command with the initial response */
    result = imap_sendf(data, imapc, "AUTHENTICATE %s %s",
                        mech, *ir ? ir : "=");
  }
  இல்லையெனில் {
    /* Send the AUTHENTICATE command */
    result = imap_sendf(data, imapc, "AUTHENTICATE %s", mech);
  }

  திரும்பு result;
}

/***********************************************************************
 *
 * imap_continue_authenticate()
 *
 * Sends SASL continuation data.
 */
நிலையான CURLcode imap_continue_authenticate(கட்டமைப்பு Curl_easy *data,
                                           மாறிலி எழுத்து *mech,
                                           மாறிலி கட்டமைப்பு bufref *resp)
{
  கட்டமைப்பு imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  (வெறுமை)mech;
  எனில்(!imapc)
    திரும்பு CURLE_FAILED_INIT;
  திரும்பு Curl_pp_sendf(data, &imapc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * imap_cancel_authenticate()
 *
 * Sends SASL cancellation.
 */
நிலையான CURLcode imap_cancel_authenticate(கட்டமைப்பு Curl_easy *data,
                                         மாறிலி எழுத்து *mech)
{
  கட்டமைப்பு imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  (வெறுமை)mech;
  எனில்(!imapc)
    திரும்பு CURLE_FAILED_INIT;
  திரும்பு Curl_pp_sendf(data, &imapc->pp, "*");
}

/***********************************************************************
 *
 * imap_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to clear text should a common
 * mechanism not be available between the client and server.
 */
நிலையான CURLcode imap_perform_authentication(கட்டமைப்பு Curl_easy *data,
                                            கட்டமைப்பு imap_conn *imapc)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  /* Check if already authenticated OR if there is enough data to authenticate
     with and end the connect phase if we do not */
  எனில்(imapc->preauth ||
     !Curl_sasl_can_authenticate(&imapc->sasl, data)) {
    imap_state(data, imapc, IMAP_STOP);
    திரும்பு result;
  }

  /* Calculate the SASL login details */
  result = Curl_sasl_start(&imapc->sasl, data, (bool)imapc->ir_supported,
                           &progress);
  எனில்(!result) {
    எனில்(progress == SASL_INPROGRESS)
      imap_state(data, imapc, IMAP_AUTHENTICATE);
    இல்லையெனில் எனில்(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
      /* Perform clear text authentication */
      result = imap_perform_login(data, imapc, data->conn);
    இல்லையெனில்
      result = Curl_sasl_is_blocked(&imapc->sasl, data);
  }

  திரும்பு result;
}

/***********************************************************************
 *
 * imap_perform_list()
 *
 * Sends a LIST command or an alternative custom request.
 */
நிலையான CURLcode imap_perform_list(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு imap_conn *imapc,
                                  கட்டமைப்பு IMAP *imap)
{
  CURLcode result = CURLE_OK;

  எனில்(imap->custom)
    /* Send the custom request */
    result = imap_sendf(data, imapc, "%s%s", imap->custom,
                        imap->custom_params ? imap->custom_params : "");
  இல்லையெனில் {
    /* Make sure the mailbox is in the correct atom format if necessary */
    எழுத்து *mailbox = imap->mailbox ? imap_atom(imap->mailbox, TRUE)
                                  : curlx_strdup("");
    எனில்(!mailbox)
      திரும்பு CURLE_OUT_OF_MEMORY;

    /* Send the LIST command */
    result = imap_sendf(data, imapc, "LIST \"%s\" *", mailbox);

    curlx_free(mailbox);
  }

  எனில்(!result)
    imap_state(data, imapc, IMAP_LIST);

  திரும்பு result;
}

/***********************************************************************
 *
 * imap_perform_select()
 *
 * Sends a SELECT command to ask the server to change the selected mailbox.
 */
நிலையான CURLcode imap_perform_select(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு imap_conn *imapc,
                                    கட்டமைப்பு IMAP *imap)
{
  CURLcode result = CURLE_OK;
  எழுத்து *mailbox;

  /* Invalidate old information as we are switching mailboxes */
  curlx_safefree(imapc->mailbox);
  imapc->mb_uidvalidity_set = FALSE;

  /* Check we have a mailbox */
  எனில்(!imap->mailbox) {
    failf(data, "Cannot SELECT without a mailbox.");
    திரும்பு CURLE_URL_MALFORMAT;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  எனில்(!mailbox)
    திரும்பு CURLE_OUT_OF_MEMORY;

  /* Send the SELECT command */
  result = imap_sendf(data, imapc, "SELECT %s", mailbox);

  curlx_free(mailbox);

  எனில்(!result)
    imap_state(data, imapc, IMAP_SELECT);

  திரும்பு result;
}

/***********************************************************************
 *
 * imap_perform_fetch()
 *
 * Sends a FETCH command to initiate the download of a message.
 */
நிலையான CURLcode imap_perform_fetch(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு imap_conn *imapc,
                                   கட்டமைப்பு IMAP *imap)
{
  CURLcode result = CURLE_OK;
  /* Check we have a UID */
  எனில்(imap->uid) {

    /* Send the FETCH command */
    எனில்(imap->partial)
      result = imap_sendf(data, imapc, "UID FETCH %s BODY[%s]<%s>",
                          imap->uid, imap->section ? imap->section : "",
                          imap->partial);
    இல்லையெனில்
      result = imap_sendf(data, imapc, "UID FETCH %s BODY[%s]",
                          imap->uid, imap->section ? imap->section : "");
  }
  இல்லையெனில் எனில்(imap->mindex) {
    /* Send the FETCH command */
    எனில்(imap->partial)
      result = imap_sendf(data, imapc, "FETCH %s BODY[%s]<%s>",
                          imap->mindex, imap->section ? imap->section : "",
                          imap->partial);
    இல்லையெனில்
      result = imap_sendf(data, imapc, "FETCH %s BODY[%s]",
                          imap->mindex, imap->section ? imap->section : "");
  }
  இல்லையெனில் {
    failf(data, "Cannot FETCH without a UID.");
    திரும்பு CURLE_URL_MALFORMAT;
  }
  எனில்(!result)
    imap_state(data, imapc, IMAP_FETCH);

  திரும்பு result;
}

/***********************************************************************
 *
 * imap_perform_append()
 *
 * Sends an APPEND command to initiate the upload of a message.
 */
நிலையான CURLcode imap_perform_append(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு imap_conn *imapc,
                                    கட்டமைப்பு IMAP *imap)
{
  CURLcode result = CURLE_OK;
  எழுத்து *mailbox;
  கட்டமைப்பு dynbuf flags;

  /* Check we have a mailbox */
  எனில்(!imap->mailbox) {
    failf(data, "Cannot APPEND without a mailbox.");
    திரும்பு CURLE_URL_MALFORMAT;
  }

#ifndef CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  எனில்(IS_MIME_POST(data)) {
    curl_mimepart *postp = data->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(unsigned முழு)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, data->set.headers, 0);
    result = Curl_mime_prepare_headers(data, postp, வெற்று,
                                       வெற்று, MIMESTRATEGY_MAIL);

    எனில்(!result && !Curl_checkheaders(data, STRCONST("Mime-Version")))
      result = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    எனில்(!result)
      result = Curl_creader_set_mime(data, postp);
    எனில்(result)
      திரும்பு result;
  }
  இல்லையெனில்
#endif
  {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    எனில்(result)
      திரும்பு result;
  }

  /* Check we know the size of the upload. This takes all readers
   * into account. Especially crlf conversions which make the size
   * unpredictable, e.g. -1. */
  data->state.infilesize = Curl_creader_total_length(data);
  எனில்(data->state.infilesize < 0) {
    எனில்(data->set.crlf)
      failf(data, "Cannot APPEND with CRLF conversion making size unknown");
    இல்லையெனில்
      failf(data, "Cannot APPEND with unknown input file size");
    திரும்பு CURLE_UPLOAD_FAILED;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  எனில்(!mailbox)
    திரும்பு CURLE_OUT_OF_MEMORY;

  /* Generate flags string and send the APPEND command */
  curlx_dyn_init(&flags, 100);
  எனில்(data->set.upload_flags) {
    முழு i;
    கட்டமைப்பு ulbits ulflag[] = {
      { CURLULFLAG_ANSWERED, "Answered" },
      { CURLULFLAG_DELETED, "Deleted" },
      { CURLULFLAG_DRAFT, "Draft" },
      { CURLULFLAG_FLAGGED, "Flagged" },
      { CURLULFLAG_SEEN, "Seen" },
      { 0, வெற்று }
    };

    result = CURLE_OUT_OF_MEMORY;
    எனில்(curlx_dyn_add(&flags, " (")) {
      செல் cleanup;
    }

    க்கு(i = 0; ulflag[i].bit; i++) {
      எனில்(data->set.upload_flags & ulflag[i].bit &&
         ((curlx_dyn_len(&flags) > 2 && curlx_dyn_add(&flags, " ")) ||
          curlx_dyn_add(&flags, "\\") ||
          curlx_dyn_add(&flags, ulflag[i].flag)))
        செல் cleanup;
    }

    எனில்(curlx_dyn_add(&flags, ")"))
      செல் cleanup;
  }
  இல்லையெனில் எனில்(curlx_dyn_add(&flags, ""))
    செல் cleanup;

  result = imap_sendf(data, imapc, "APPEND %s%s {%" FMT_OFF_T "}",
                      mailbox, curlx_dyn_ptr(&flags), data->state.infilesize);

cleanup:
  curlx_dyn_free(&flags);
  curlx_free(mailbox);

  எனில்(!result)
    imap_state(data, imapc, IMAP_APPEND);

  திரும்பு result;
}

/***********************************************************************
 *
 * imap_perform_search()
 *
 * Sends a SEARCH command.
 */
நிலையான CURLcode imap_perform_தேடல்(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு imap_conn *imapc,
                                    கட்டமைப்பு IMAP *imap)
{
  CURLcode result = CURLE_OK;

  /* Check we have a query string */
  எனில்(!imap->query) {
    failf(data, "Cannot SEARCH without a query string.");
    திரும்பு CURLE_URL_MALFORMAT;
  }

  /* Send the SEARCH command */
  result = imap_sendf(data, imapc, "SEARCH %s", imap->query);

  எனில்(!result)
    imap_state(data, imapc, IMAP_தேடல்);

  திரும்பு result;
}

/***********************************************************************
 *
 * imap_perform_logout()
 *
 * Performs the logout action prior to sclose() being called.
 */
நிலையான CURLcode imap_perform_logout(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு imap_conn *imapc)
{
  /* Send the LOGOUT command */
  CURLcode result = imap_sendf(data, imapc, "LOGOUT");

  எனில்(!result)
    imap_state(data, imapc, IMAP_LOGOUT);

  திரும்பு result;
}

/* For the initial server greeting */
நிலையான CURLcode imap_state_servergreet_resp(கட்டமைப்பு Curl_easy *data,
                                            கட்டமைப்பு imap_conn *imapc,
                                            முழு imapcode,
                                            imapstate instate)
{
  (வெறுமை)instate;

  எனில்(imapcode == IMAP_RESP_PREAUTH) {
    /* PREAUTH */
    imapc->preauth = TRUE;
    infof(data, "PREAUTH connection, already authenticated");
  }
  இல்லையெனில் எனில்(imapcode != IMAP_RESP_OK) {
    failf(data, "Got unexpected imap-server response");
    திரும்பு CURLE_WEIRD_SERVER_REPLY;
  }

  திரும்பு imap_perform_capability(data, imapc);
}

/* For CAPABILITY responses */
நிலையான CURLcode imap_state_capability_resp(கட்டமைப்பு Curl_easy *data,
                                           கட்டமைப்பு imap_conn *imapc,
                                           முழு imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  மாறிலி எழுத்து *வரி = curlx_dyn_ptr(&imapc->pp.recvbuf);

  (வெறுமை)instate;

  /* Do we have an untagged response? */
  எனில்(imapcode == '*') {
    வரி += 2;

    /* Loop through the data line */
    க்கு(;;) {
      size_t wordlen;
      போது(*வரி && (ISBLANK(*வரி) || ISNEWLINE(*வரி)))
        வரி++;

      எனில்(!*வரி)
        நிறுத்து;

      /* Extract the word */
      க்கு(wordlen = 0; வரி[wordlen] && !ISBLANK(வரி[wordlen]) &&
                       !ISNEWLINE(வரி[wordlen]);)
        wordlen++;

      /* Does the server support the STARTTLS capability? */
      எனில்(wordlen == 8 && curl_strnequal(வரி, "STARTTLS", 8))
        imapc->tls_supported = TRUE;

      /* Has the server explicitly disabled clear text authentication? */
      இல்லையெனில் எனில்(wordlen == 13 && curl_strnequal(வரி, "LOGINDISABLED", 13))
        imapc->login_disabled = TRUE;

      /* Does the server support the SASL-IR capability? */
      இல்லையெனில் எனில்(wordlen == 7 && curl_strnequal(வரி, "SASL-IR", 7))
        imapc->ir_supported = TRUE;

      /* Do we have a SASL based authentication mechanism? */
      இல்லையெனில் எனில்(wordlen > 5 && curl_strnequal(வரி, "AUTH=", 5)) {
        size_t llen;
        unsigned short mechbit;

        வரி += 5;
        wordlen -= 5;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(வரி, wordlen, &llen);
        எனில்(mechbit && llen == wordlen)
          imapc->sasl.authmechs |= mechbit;
      }

      வரி += wordlen;
    }
  }
  இல்லையெனில் எனில்(data->set.use_ssl && !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
    /* PREAUTH is not compatible with STARTTLS. */
    எனில்(imapcode == IMAP_RESP_OK && imapc->tls_supported && !imapc->preauth) {
      /* Switch to TLS connection now */
      result = imap_perform_starttls(data, imapc);
    }
    இல்லையெனில் எனில்(data->set.use_ssl <= CURLUSESSL_TRY)
      result = imap_perform_authentication(data, imapc);
    இல்லையெனில் {
      failf(data, "STARTTLS not available.");
      result = CURLE_USE_SSL_FAILED;
    }
  }
  இல்லையெனில்
    result = imap_perform_authentication(data, imapc);

  திரும்பு result;
}

/* For STARTTLS responses */
நிலையான CURLcode imap_state_starttls_resp(கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு imap_conn *imapc,
                                         முழு imapcode,
                                         imapstate instate)
{
  CURLcode result = CURLE_OK;

  (வெறுமை)instate;

  /* Pipelining in response is forbidden. */
  எனில்(imapc->pp.overflow)
    திரும்பு CURLE_WEIRD_SERVER_REPLY;

  எனில்(imapcode != IMAP_RESP_OK) {
    எனில்(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied");
      result = CURLE_USE_SSL_FAILED;
    }
    இல்லையெனில்
      result = imap_perform_authentication(data, imapc);
  }
  இல்லையெனில்
    imap_state(data, imapc, IMAP_UPGRADETLS);

  திரும்பு result;
}

/* For SASL authentication responses */
நிலையான CURLcode imap_state_auth_resp(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு imap_conn *imapc,
                                     முழு imapcode,
                                     imapstate instate)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  (வெறுமை)instate;

  result = Curl_sasl_continue(&imapc->sasl, data, imapcode, &progress);
  எனில்(!result)
    தேர்வு(progress) {
    நிலை SASL_DONE:
      imap_state(data, imapc, IMAP_STOP);  /* Authenticated */
      நிறுத்து;
    நிலை SASL_IDLE:            /* No mechanism left after cancellation */
      எனில்(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
        /* Perform clear text authentication */
        result = imap_perform_login(data, imapc, data->conn);
      இல்லையெனில் {
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
      }
      நிறுத்து;
    இயல்பு:
      நிறுத்து;
    }

  திரும்பு result;
}

/* For LOGIN responses */
நிலையான CURLcode imap_state_login_resp(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு imap_conn *imapc,
                                      முழு imapcode,
                                      imapstate instate)
{
  CURLcode result = CURLE_OK;
  (வெறுமை)instate;

  எனில்(imapcode != IMAP_RESP_OK) {
    failf(data, "Access denied. %c", imapcode);
    result = CURLE_LOGIN_DENIED;
  }
  இல்லையெனில்
    /* End of connect phase */
    imap_state(data, imapc, IMAP_STOP);

  திரும்பு result;
}

/* Detect IMAP listings vs. downloading a single email */
நிலையான bool is_custom_fetch_listing_match(மாறிலி எழுத்து *params)
{
  /* match " 1:* (FLAGS ..." or " 1,2,3 (FLAGS ..." */
  எனில்(*params++ != ' ')
    திரும்பு FALSE;

  போது(ISDIGIT(*params)) {
    params++;
    எனில்(*params == 0)
      திரும்பு FALSE;
  }
  எனில்(*params == ':')
    திரும்பு TRUE;
  எனில்(*params == ',')
    திரும்பு TRUE;
  திரும்பு FALSE;
}

நிலையான bool is_custom_fetch_listing(கட்டமைப்பு IMAP *imap)
{
  /* filter out "UID FETCH 1:* (FLAGS ..." queries to list emails */
  எனில்(!imap->custom)
    திரும்பு FALSE;
  இல்லையெனில் எனில்(curl_strequal(imap->custom, "FETCH") && imap->custom_params) {
    மாறிலி எழுத்து *p = imap->custom_params;
    திரும்பு is_custom_fetch_listing_match(p);
  }
  இல்லையெனில் எனில்(curl_strequal(imap->custom, "UID") && imap->custom_params &&
          curl_strnequal(imap->custom_params, " FETCH ", 7)) {
    மாறிலி எழுத்து *p = imap->custom_params + 6;
    திரும்பு is_custom_fetch_listing_match(p);
  }
  திரும்பு FALSE;
}

/* For LIST and SEARCH responses */
நிலையான CURLcode imap_state_listsearch_resp(கட்டமைப்பு Curl_easy *data,
                                           கட்டமைப்பு imap_conn *imapc,
                                           முழு imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  மாறிலி எழுத்து *வரி = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  கட்டமைப்பு IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);

  DEBUGASSERT(imap);
  எனில்(!imap)
    திரும்பு CURLE_FAILED_INIT;
  (வெறுமை)instate;

  எனில்(imapcode == '*' && is_custom_fetch_listing(imap)) {
    /* custom FETCH or UID FETCH for listing is not handled here */
  }
  இல்லையெனில் எனில்(imapcode == '*') {
    /* Check if this response contains a literal (e.g. FETCH responses with
       body data). Literal syntax is {size}\r\n */
    மாறிலி எழுத்து *cr = memchr(வரி, '\r', len);
    size_t வரி_len = cr ? (size_t)(cr - வரி) : len;
    மாறிலி எழுத்து *ptr = imap_find_literal(வரி, வரி_len);
    எனில்(ptr) {
      curl_off_t size = 0;
      bool parsed = FALSE;
      ptr++;
      எனில்(!curlx_str_எண்(&ptr, &size, CURL_OFF_T_MAX) &&
         !curlx_str_single(&ptr, '}'))
        parsed = TRUE;

      எனில்(parsed) {
        கட்டமைப்பு pingpong *pp = &imapc->pp;
        size_t buffer_len = curlx_dyn_len(&pp->recvbuf);
        size_t after_header = buffer_len - pp->nfinal;

        /* This is a literal response, setup to receive the body data */
        infof(data, "Found %" FMT_OFF_T " bytes to download", size);

        /* First write the header line */
        result = Curl_client_write(data, CLIENTWRITE_BODY, வரி, len);
        எனில்(result)
          திரும்பு result;

        /* Handle data already in buffer after the header line */
        எனில்(after_header > 0) {
          /* There is already data in the buffer that is part of the literal
             body or subsequent responses */
          size_t chunk = after_header;

          /* Keep only the data after the header line */
          curlx_dyn_tail(&pp->recvbuf, chunk);
          pp->nfinal = 0; /* done */

          /* Limit chunk to the literal size */
          எனில்((curl_off_t)chunk > size)
            chunk = (size_t)size;

          எனில்(chunk) {
            /* Write the literal body data */
            result = Curl_client_write(data, CLIENTWRITE_BODY,
                                       curlx_dyn_ptr(&pp->recvbuf), chunk);
            எனில்(result)
              திரும்பு result;
          }

          /* Handle remaining data in buffer (either more literal data or
             subsequent responses) */
          எனில்(after_header > chunk) {
            /* Keep the data after the literal body */
            pp->overflow = after_header - chunk;
            curlx_dyn_tail(&pp->recvbuf, pp->overflow);
          }
          இல்லையெனில் {
            pp->overflow = 0;
            curlx_dyn_reset(&pp->recvbuf);
          }
        }
        இல்லையெனில் {
          /* No data in buffer yet, reset overflow */
          pp->overflow = 0;
        }

        எனில்((CURL_OFF_T_MAX - size) < (curl_off_t)len)
          /* unlikely to actually be a transfer this big, but avoid integer
             overflow */
          size = CURL_OFF_T_MAX;
        இல்லையெனில்
          size += len;

        /* Progress size includes both header line and literal body */
        Curl_pgrsSetDownloadSize(data, size);

        எனில்(data->req.bytecount == size)
          /* All data already transferred (header + literal body) */
          Curl_xfer_setup_nop(data);
        இல்லையெனில் {
          /* Setup to receive the literal body data.
             maxdownload and transfer size include both header line and
             literal body */
          data->req.maxdownload = size;
          Curl_xfer_setup_recv(data, FIRSTSOCKET, size);
        }
        /* End of DO phase */
        imap_state(data, imapc, IMAP_STOP);
      }
      இல்லையெனில் {
        /* Failed to parse literal, write the line */
        result = Curl_client_write(data, CLIENTWRITE_BODY, வரி, len);
      }
    }
    இல்லையெனில் {
      /* No literal, write the line as-is */
      result = Curl_client_write(data, CLIENTWRITE_BODY, வரி, len);
    }
  }
  இல்லையெனில் எனில்(imapcode != IMAP_RESP_OK)
    result = CURLE_QUOTE_ERROR;
  இல்லையெனில்
    /* End of DO phase */
    imap_state(data, imapc, IMAP_STOP);

  திரும்பு result;
}

/* For SELECT responses */
நிலையான CURLcode imap_state_select_resp(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு imap_conn *imapc,
                                       கட்டமைப்பு IMAP *imap,
                                       முழு imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  (வெறுமை)instate;

  எனில்(imapcode == '*') {
    /* See if this is an UIDVALIDITY response */
    மாறிலி எழுத்து *வரி = curlx_dyn_ptr(&imapc->pp.recvbuf);
    size_t len = curlx_dyn_len(&imapc->pp.recvbuf);
    எனில்((len >= 18) && checkprefix("OK [UIDVALIDITY ", &வரி[2])) {
      curl_off_t மதிப்பு;
      மாறிலி எழுத்து *p = &வரி[2] + CURL_CSTRLEN("OK [UIDVALIDITY ");
      எனில்(!curlx_str_எண்(&p, &மதிப்பு, UINT_MAX)) {
        imapc->mb_uidvalidity = (unsigned முழு)மதிப்பு;
        imapc->mb_uidvalidity_set = TRUE;
      }
    }
  }
  இல்லையெனில் எனில்(imapcode == IMAP_RESP_OK) {
    /* Check if the UIDVALIDITY has been specified and matches */
    எனில்(imap->uidvalidity_set && imapc->mb_uidvalidity_set &&
       (imap->uidvalidity != imapc->mb_uidvalidity)) {
      failf(data, "Mailbox UIDVALIDITY has changed");
      result = CURLE_REMOTE_FILE_NOT_FOUND;
    }
    இல்லையெனில் {
      /* Note the currently opened mailbox on this connection */
      DEBUGASSERT(!imapc->mailbox);
      imapc->mailbox = curlx_strdup(imap->mailbox);
      எனில்(!imapc->mailbox)
        திரும்பு CURLE_OUT_OF_MEMORY;

      எனில்(imap->custom)
        result = imap_perform_list(data, imapc, imap);
      இல்லையெனில் எனில்(imap->query)
        result = imap_perform_தேடல்(data, imapc, imap);
      இல்லையெனில்
        result = imap_perform_fetch(data, imapc, imap);
    }
  }
  இல்லையெனில் {
    failf(data, "Select failed");
    result = CURLE_LOGIN_DENIED;
  }

  திரும்பு result;
}

/* For the (first line of the) FETCH responses */
நிலையான CURLcode imap_state_fetch_resp(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு imap_conn *imapc,
                                      முழு imapcode,
                                      imapstate instate)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு pingpong *pp = &imapc->pp;
  மாறிலி எழுத்து *ptr = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  bool parsed = FALSE;
  curl_off_t size = 0;

  (வெறுமை)instate;

  எனில்(imapcode != '*') {
    Curl_pgrsSetDownloadSize(data, -1);
    imap_state(data, imapc, IMAP_STOP);
    திரும்பு CURLE_REMOTE_FILE_NOT_FOUND;
  }

  /* Something like this is received "* 1 FETCH (BODY[TEXT] {2021}\r" so parse
     the continuation data contained within the curly brackets */
  ptr = imap_find_literal(ptr, len);
  எனில்(ptr) {
    ptr++;
    எனில்(!curlx_str_எண்(&ptr, &size, CURL_OFF_T_MAX) &&
       !curlx_str_single(&ptr, '}'))
      parsed = TRUE;
  }

  எனில்(parsed) {
    infof(data, "Found %" FMT_OFF_T " bytes to download", size);
    Curl_pgrsSetDownloadSize(data, size);

    எனில்(pp->overflow) {
      /* At this point there is a data in the receive buffer that is body
         content, send it as body and then skip it. Do note that there may
         even be additional "headers" after the body. */
      size_t chunk = pp->overflow;

      /* keep only the overflow */
      curlx_dyn_tail(&pp->recvbuf, chunk);
      pp->nfinal = 0; /* done */

      எனில்((curl_off_t)chunk > size)
        /* The conversion from curl_off_t to size_t is always fine here */
        chunk = (size_t)size;

      எனில்(!chunk) {
        /* no size, we are done with the data */
        imap_state(data, imapc, IMAP_STOP);
        திரும்பு CURLE_OK;
      }
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 curlx_dyn_ptr(&pp->recvbuf), chunk);
      எனில்(result)
        திரும்பு result;

      infof(data, "Written %zu bytes, %" FMT_OFF_T
            " bytes are left for transfer", chunk, (curl_off_t)(size - chunk));

      /* Have we used the entire overflow or part of it?*/
      எனில்(pp->overflow > chunk) {
        /* remember the remaining trailing overflow data */
        pp->overflow -= chunk;
        curlx_dyn_tail(&pp->recvbuf, pp->overflow);
      }
      இல்லையெனில் {
        pp->overflow = 0; /* handled */
        /* Free the cache */
        curlx_dyn_reset(&pp->recvbuf);
      }
    }

    எனில்(data->req.bytecount == size)
      /* The entire data is already transferred! */
      Curl_xfer_setup_nop(data);
    இல்லையெனில் {
      /* IMAP download */
      data->req.maxdownload = size;
      Curl_xfer_setup_recv(data, FIRSTSOCKET, size);
    }
  }
  இல்லையெனில் {
    /* We do not know how to parse this line */
    failf(data, "Failed to parse FETCH response.");
    result = CURLE_WEIRD_SERVER_REPLY;
  }

  /* End of DO phase */
  imap_state(data, imapc, IMAP_STOP);

  திரும்பு result;
}

/* For final FETCH responses performed after the download */
நிலையான CURLcode imap_state_fetch_final_resp(கட்டமைப்பு Curl_easy *data,
                                            கட்டமைப்பு imap_conn *imapc,
                                            முழு imapcode,
                                            imapstate instate)
{
  CURLcode result = CURLE_OK;

  (வெறுமை)instate;

  எனில்(imapcode != IMAP_RESP_OK)
    result = CURLE_WEIRD_SERVER_REPLY;
  இல்லையெனில்
    /* End of DONE phase */
    imap_state(data, imapc, IMAP_STOP);

  திரும்பு result;
}

/* For APPEND responses */
நிலையான CURLcode imap_state_append_resp(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு imap_conn *imapc,
                                       முழு imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  (வெறுமை)instate;

  எனில்(imapcode != '+') {
    result = CURLE_UPLOAD_FAILED;
  }
  இல்லையெனில் {
    /* Set the progress upload size */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* IMAP upload */
    Curl_xfer_setup_send(data, FIRSTSOCKET);

    /* End of DO phase */
    imap_state(data, imapc, IMAP_STOP);
  }

  திரும்பு result;
}

/* For final APPEND responses performed after the upload */
நிலையான CURLcode imap_state_append_final_resp(கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு imap_conn *imapc,
                                             முழு imapcode,
                                             imapstate instate)
{
  CURLcode result = CURLE_OK;

  (வெறுமை)instate;

  எனில்(imapcode != IMAP_RESP_OK)
    result = CURLE_UPLOAD_FAILED;
  இல்லையெனில்
    /* End of DONE phase */
    imap_state(data, imapc, IMAP_STOP);

  திரும்பு result;
}

நிலையான CURLcode imap_pp_statemachine(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு connectdata *conn)
{
  CURLcode result = CURLE_OK;
  முழு imapcode;
  கட்டமைப்பு imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  கட்டமைப்பு IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  கட்டமைப்பு pingpong *pp;
  size_t nread = 0;

  எனில்(!imapc || !imap)
    திரும்பு CURLE_FAILED_INIT;
  pp = &imapc->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not IMAP */
upgrade_tls:
  எனில்(imapc->state == IMAP_UPGRADETLS) {
    result = imap_perform_upgrade_tls(data, imapc, conn);
    எனில்(result || (imapc->state == IMAP_UPGRADETLS))
      திரும்பு result;
  }

  /* Flush any data that needs to be sent */
  எனில்(pp->sendleft)
    திரும்பு Curl_pp_flushsend(data, pp);

  செய் {
    /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, pp, &imapcode, &nread);
    எனில்(result)
      திரும்பு result;

    /* Was there an error parsing the response line? */
    எனில்(imapcode == -1)
      திரும்பு CURLE_WEIRD_SERVER_REPLY;

    எனில்(!imapcode)
      நிறுத்து;

    /* We have now received a full IMAP server response */
    தேர்வு(imapc->state) {
    நிலை IMAP_SERVERGREET:
      result = imap_state_servergreet_resp(data, imapc,
                                           imapcode, imapc->state);
      நிறுத்து;

    நிலை IMAP_CAPABILITY:
      result = imap_state_capability_resp(data, imapc, imapcode, imapc->state);
      நிறுத்து;

    நிலை IMAP_STARTTLS:
      result = imap_state_starttls_resp(data, imapc, imapcode, imapc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      எனில்(!result && (imapc->state == IMAP_UPGRADETLS))
        செல் upgrade_tls;
      நிறுத்து;

    நிலை IMAP_AUTHENTICATE:
      result = imap_state_auth_resp(data, imapc, imapcode, imapc->state);
      நிறுத்து;

    நிலை IMAP_LOGIN:
      result = imap_state_login_resp(data, imapc, imapcode, imapc->state);
      நிறுத்து;

    நிலை IMAP_LIST:
    நிலை IMAP_தேடல்:
      result = imap_state_listsearch_resp(data, imapc, imapcode, imapc->state);
      நிறுத்து;

    நிலை IMAP_SELECT:
      result = imap_state_select_resp(data, imapc, imap,
                                      imapcode, imapc->state);
      நிறுத்து;

    நிலை IMAP_FETCH:
      result = imap_state_fetch_resp(data, imapc, imapcode, imapc->state);
      நிறுத்து;

    நிலை IMAP_FETCH_FINAL:
      result = imap_state_fetch_final_resp(data, imapc,
                                           imapcode, imapc->state);
      நிறுத்து;

    நிலை IMAP_APPEND:
      result = imap_state_append_resp(data, imapc, imapcode, imapc->state);
      நிறுத்து;

    நிலை IMAP_APPEND_FINAL:
      result = imap_state_append_final_resp(data, imapc,
                                            imapcode, imapc->state);
      நிறுத்து;

    நிலை IMAP_LOGOUT:
    இயல்பு:
      /* internal error */
      imap_state(data, imapc, IMAP_STOP);
      நிறுத்து;
    }
  } போது(!result && imapc->state != IMAP_STOP && Curl_pp_moredata(pp));

  திரும்பு result;
}

/* Called repeatedly until done from multi.c */
நிலையான CURLcode imap_multi_statemach(கட்டமைப்பு Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  *done = FALSE;
  எனில்(!imapc)
    திரும்பு CURLE_FAILED_INIT;
  result = Curl_pp_statemach(data, &imapc->pp, FALSE, FALSE);
  *done = (imapc->state == IMAP_STOP);

  திரும்பு result;
}

நிலையான CURLcode imap_block_statemach(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு imap_conn *imapc,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;

  போது(imapc->state != IMAP_STOP && !result)
    result = Curl_pp_statemach(data, &imapc->pp, TRUE, disconnecting);

  திரும்பு result;
}

/* For the IMAP "protocol connect" and "doing" phases only */
நிலையான CURLcode imap_pollset(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு easy_pollset *ps)
{
  கட்டமைப்பு imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  திரும்பு imapc ? Curl_pp_pollset(data, &imapc->pp, ps) : CURLE_OK;
}

நிலையான வெறுமை imap_easy_reset(கட்டமைப்பு IMAP *imap)
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
நிலையான bool imap_is_bchar(எழுத்து ch)
{
  /* Performing the alnum check first with macro is faster because of ASCII
     arithmetic */
  திரும்பு ch && (ISALNUM(ch) || strchr(":@/&=-._~!$\'()*+,%", ch));
}

/***********************************************************************
 *
 * imap_parse_url_options()
 *
 * Parse the URL login options.
 */
நிலையான CURLcode imap_parse_url_options(கட்டமைப்பு connectdata *conn,
                                       கட்டமைப்பு imap_conn *imapc)
{
  CURLcode result = CURLE_OK;
  மாறிலி எழுத்து *ptr = conn->options;
  bool prefer_login = FALSE;

  போது(!result && ptr && *ptr) {
    மாறிலி எழுத்து *சாவி = ptr;
    மாறிலி எழுத்து *மதிப்பு;

    போது(*ptr && *ptr != '=')
      ptr++;

    மதிப்பு = ptr + 1;

    போது(*ptr && *ptr != ';')
      ptr++;

    எனில்(curl_strnequal(சாவி, "AUTH=+LOGIN", 11)) {
      /* User prefers plaintext LOGIN over any SASL, including SASL LOGIN */
      prefer_login = TRUE;
      imapc->sasl.prefmech = SASL_AUTH_NONE;
    }
    இல்லையெனில் எனில்(curl_strnequal(சாவி, "AUTH=", 5)) {
      prefer_login = FALSE;
      result = Curl_sasl_parse_url_auth_option(&imapc->sasl,
                                               மதிப்பு, ptr - மதிப்பு);
    }
    இல்லையெனில் {
      prefer_login = FALSE;
      result = CURLE_URL_MALFORMAT;
    }

    எனில்(*ptr == ';')
      ptr++;
  }

  எனில்(prefer_login)
    imapc->preftype = IMAP_TYPE_CLEARTEXT;
  இல்லையெனில் {
    தேர்வு(imapc->sasl.prefmech) {
    நிலை SASL_AUTH_NONE:
      imapc->preftype = IMAP_TYPE_NONE;
      நிறுத்து;
    நிலை SASL_AUTH_DEFAULT:
      imapc->preftype = IMAP_TYPE_ANY;
      நிறுத்து;
    இயல்பு:
      imapc->preftype = IMAP_TYPE_SASL;
      நிறுத்து;
    }
  }

  திரும்பு result;
}

/***********************************************************************
 *
 * imap_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
நிலையான CURLcode imap_parse_url_path(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு IMAP *imap)
{
  /* The imap struct is already initialized in imap_connect() */
  CURLcode result = CURLE_OK;
  மாறிலி எழுத்து *begin = &data->state.up.path[1]; /* skip leading slash */
  மாறிலி எழுத்து *ptr = begin;

  /* See how much of the URL is a valid path and decode it */
  போது(imap_is_bchar(*ptr))
    ptr++;

  எனில்(ptr != begin) {
    /* Remove the trailing slash if present */
    மாறிலி எழுத்து *end = ptr;
    எனில்(end > begin && end[-1] == '/')
      end--;

    result = Curl_urldecode(begin, end - begin, &imap->mailbox, வெற்று,
                            REJECT_CTRL);
    எனில்(result)
      திரும்பு result;
  }
  இல்லையெனில்
    imap->mailbox = வெற்று;

  /* There can be any number of parameters in the form ";NAME=VALUE" */
  போது(*ptr == ';') {
    எழுத்து *name;
    எழுத்து *மதிப்பு;
    size_t valuelen;

    /* Find the length of the name parameter */
    begin = ++ptr;
    போது(*ptr && *ptr != '=')
      ptr++;

    எனில்(!*ptr)
      திரும்பு CURLE_URL_MALFORMAT;

    /* Decode the name parameter */
    result = Curl_urldecode(begin, ptr - begin, &name, வெற்று,
                            REJECT_CTRL);
    எனில்(result)
      திரும்பு result;

    /* Find the length of the value parameter */
    begin = ++ptr;
    போது(imap_is_bchar(*ptr))
      ptr++;

    /* Decode the value parameter */
    result = Curl_urldecode(begin, ptr - begin, &மதிப்பு, &valuelen,
                            REJECT_CTRL);
    எனில்(result) {
      curlx_free(name);
      திரும்பு result;
    }

    DEBUGF(infof(data, "IMAP URL parameter '%s' = '%s'", name, மதிப்பு));

    /* Process the known hierarchical parameters (UIDVALIDITY, UID, SECTION
       and PARTIAL) stripping of the trailing slash character if it is
       present.

       Note: Unknown parameters trigger a URL_MALFORMAT error. */
    எனில்(valuelen > 0 && மதிப்பு[valuelen - 1] == '/')
      மதிப்பு[valuelen - 1] = '\0';
    எனில்(valuelen) {
      எனில்(curl_strequal(name, "UIDVALIDITY") && !imap->uidvalidity_set) {
        curl_off_t இலக்கம்;
        மாறிலி எழுத்து *p = (மாறிலி எழுத்து *)மதிப்பு;
        எனில்(!curlx_str_எண்(&p, &இலக்கம், UINT_MAX)) {
          imap->uidvalidity = (unsigned முழு)இலக்கம்;
          imap->uidvalidity_set = TRUE;
        }
        curlx_free(மதிப்பு);
      }
      இல்லையெனில் எனில்(curl_strequal(name, "UID") && !imap->uid) {
        imap->uid = மதிப்பு;
      }
      இல்லையெனில் எனில்(curl_strequal(name, "MAILINDEX") && !imap->mindex) {
        imap->mindex = மதிப்பு;
      }
      இல்லையெனில் எனில்(curl_strequal(name, "SECTION") && !imap->section) {
        imap->section = மதிப்பு;
      }
      இல்லையெனில் எனில்(curl_strequal(name, "PARTIAL") && !imap->partial) {
        imap->partial = மதிப்பு;
      }
      இல்லையெனில் {
        curlx_free(name);
        curlx_free(மதிப்பு);
        திரும்பு CURLE_URL_MALFORMAT;
      }
    }
    இல்லையெனில்
      /* blank? */
      curlx_free(மதிப்பு);
    curlx_free(name);
  }

  /* Does the URL contain a query parameter? Only valid when we have a mailbox
     and no UID as per RFC-5092 */
  எனில்(imap->mailbox && !imap->uid && !imap->mindex) {
    /* Get the query parameter, URL decoded */
    CURLUcode uc = curl_url_get(data->state.uh, CURLUPART_QUERY, &imap->query,
                                CURLU_URLDECODE);
    எனில்(uc == CURLUE_OUT_OF_MEMORY)
      திரும்பு CURLE_OUT_OF_MEMORY;
  }

  /* Any extra stuff at the end of the URL is an error */
  எனில்(*ptr)
    திரும்பு CURLE_URL_MALFORMAT;

  திரும்பு CURLE_OK;
}

/***********************************************************************
 *
 * imap_parse_custom_request()
 *
 * Parse the custom request.
 */
நிலையான CURLcode imap_parse_custom_request(கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு IMAP *imap)
{
  CURLcode result = CURLE_OK;
  மாறிலி எழுத்து *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  எனில்(custom) {
    /* URL decode the custom request */
    result = Curl_urldecode(custom, 0, &imap->custom, வெற்று, REJECT_CTRL);

    /* Extract the parameters if specified */
    எனில்(!result) {
      மாறிலி எழுத்து *params = imap->custom;

      போது(*params && *params != ' ')
        params++;

      எனில்(*params) {
        imap->custom_params = curlx_strdup(params);
        imap->custom[params - imap->custom] = '\0';

        எனில்(!imap->custom_params)
          result = CURLE_OUT_OF_MEMORY;
      }
    }
  }

  திரும்பு result;
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
நிலையான CURLcode imap_connect(கட்டமைப்பு Curl_easy *data, bool *done)
{
  கட்டமைப்பு imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  CURLcode result = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  எனில்(!imapc)
    திரும்பு CURLE_FAILED_INIT;

  /* Parse the URL options */
  result = imap_parse_url_options(data->conn, imapc);
  எனில்(result)
    திரும்பு result;

  /* Start off waiting for the server greeting response */
  imap_state(data, imapc, IMAP_SERVERGREET);

  /* Start off with an response id of '*' */
  curlx_strcopy(imapc->resptag, அளவு(imapc->resptag), STRCONST("*"));

  result = imap_multi_statemach(data, done);

  திரும்பு result;
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
நிலையான CURLcode imap_done(கட்டமைப்பு Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  கட்டமைப்பு IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);

  எனில்(!imapc)
    திரும்பு CURLE_FAILED_INIT;
  எனில்(!imap)
    திரும்பு CURLE_OK;

  எனில்(status || premature) {
    CURL_TRC_M(data, "IMAP done, not finalizing transfer");
    connclose(conn); /* marked for closure */
    result = status;         /* use the already set error code */
  }
  இல்லையெனில் எனில்(!data->set.connect_only &&
          ((!imap->custom && (imap->uid || imap->mindex)) ||
           (imap->custom && data->req.maxdownload > 0) ||
           data->state.upload || IS_MIME_POST(data))) {
    /* Handle responses after FETCH or APPEND transfer has finished.
       For custom commands, check if we set up a download which indicates
       a FETCH-like command with literal data. */

    எனில்(!data->state.upload && !IS_MIME_POST(data))
      imap_state(data, imapc, IMAP_FETCH_FINAL);
    இல்லையெனில் {
      /* End the APPEND command first by sending an empty line */
      result = Curl_pp_sendf(data, &imapc->pp, "%s", "");
      எனில்(!result)
        imap_state(data, imapc, IMAP_APPEND_FINAL);
    }

    /* Run the state-machine */
    எனில்(!result)
      result = imap_block_statemach(data, imapc, FALSE);
  }

  imap_easy_reset(imap);
  திரும்பு result;
}

/***********************************************************************
 *
 * imap_perform()
 *
 * This is the actual DO function for IMAP. Fetch or append a message, or do
 * other things according to the options previously setup.
 */
நிலையான CURLcode imap_perform(கட்டமைப்பு Curl_easy *data, bool *connected,
                             bool *dophase_done)
{
  /* This is IMAP and no proxy */
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  கட்டமைப்பு IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  bool selected = FALSE;

  DEBUGF(infof(data, "DO phase starts"));
  எனில்(!imapc || !imap)
    திரும்பு CURLE_FAILED_INIT;

  எனில்(data->req.no_body) {
    /* Requested no body means no transfer */
    imap->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* Determine if the requested mailbox (with the same UIDVALIDITY if set)
     has already been selected on this connection */
  எனில்(imap->mailbox && imapc->mailbox &&
     (!strcmp(imap->mailbox, imapc->mailbox) ||
      (curl_strequal(imap->mailbox, "INBOX") &&
       curl_strequal(imapc->mailbox, "INBOX"))) &&
     (!imap->uidvalidity_set || !imapc->mb_uidvalidity_set ||
      (imap->uidvalidity == imapc->mb_uidvalidity)))
    selected = TRUE;

  /* Start the first command in the DO phase */
  எனில்(data->state.upload || IS_MIME_POST(data))
    /* APPEND can be executed directly */
    result = imap_perform_append(data, imapc, imap);
  இல்லையெனில் எனில்(imap->custom && (selected || !imap->mailbox))
    /* Custom command using the same mailbox or no mailbox */
    result = imap_perform_list(data, imapc, imap);
  இல்லையெனில் எனில்(!imap->custom && selected && (imap->uid || imap->mindex))
    /* FETCH from the same mailbox */
    result = imap_perform_fetch(data, imapc, imap);
  இல்லையெனில் எனில்(!imap->custom && selected && imap->query)
    /* SEARCH the current mailbox */
    result = imap_perform_தேடல்(data, imapc, imap);
  இல்லையெனில் எனில்(imap->mailbox && !selected &&
          (imap->custom || imap->uid || imap->mindex || imap->query))
    /* SELECT the mailbox */
    result = imap_perform_select(data, imapc, imap);
  இல்லையெனில்
    /* LIST */
    result = imap_perform_list(data, imapc, imap);

  எனில்(result)
    திரும்பு result;

  /* Run the state-machine */
  result = imap_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(conn, FIRSTSOCKET);

  எனில்(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  திரும்பு result;
}

/* Call this when the DO phase has completed */
நிலையான CURLcode imap_dophase_done(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு IMAP *imap,
                                  bool connected)
{
  (வெறுமை)connected;

  எனில்(imap->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);

  திரும்பு CURLE_OK;
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
நிலையான CURLcode imap_regular_transfer(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு IMAP *imap,
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
  எனில்(!result && *dophase_done)
    result = imap_dophase_done(data, imap, connected);

  திரும்பு result;
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
நிலையான CURLcode imap_do(கட்டமைப்பு Curl_easy *data, bool *done)
{
  கட்டமைப்பு IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* default to false */

  எனில்(!imap)
    திரும்பு CURLE_FAILED_INIT;
  /* Parse the URL path */
  result = imap_parse_url_path(data, imap);
  எனில்(result)
    திரும்பு result;

  /* Parse the custom request */
  result = imap_parse_custom_request(data, imap);
  எனில்(result)
    திரும்பு result;

  result = imap_regular_transfer(data, imap, done);

  திரும்பு result;
}

/***********************************************************************
 *
 * imap_disconnect()
 *
 * Disconnect from an IMAP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
நிலையான CURLcode imap_disconnect(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு connectdata *conn, bool dead_connection)
{
  கட்டமைப்பு imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);

  எனில்(imapc &&
     /* We cannot send quit unconditionally. If this connection is stale or
        bad in any way (pingpong has pending data to send),
        sending quit and waiting around here will make the
        disconnect wait in vain and cause more problems than we need to. */
     !dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &imapc->pp) &&
     !imap_perform_logout(data, imapc))
    (வெறுமை)imap_block_statemach(data, imapc, TRUE); /* ignore errors */

  திரும்பு CURLE_OK;
}

/* Called from multi.c while DOing */
நிலையான CURLcode imap_doing(கட்டமைப்பு Curl_easy *data, bool *dophase_done)
{
  கட்டமைப்பு IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  CURLcode result;

  எனில்(!imap)
    திரும்பு CURLE_FAILED_INIT;

  result = imap_multi_statemach(data, dophase_done);
  எனில்(result)
    DEBUGF(infof(data, "DO phase failed"));
  இல்லையெனில் எனில்(*dophase_done) {
    result = imap_dophase_done(data, imap, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  திரும்பு result;
}

நிலையான வெறுமை imap_easy_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு IMAP *imap = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  imap_easy_reset(imap);
  curlx_free(imap);
}

நிலையான வெறுமை imap_conn_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு imap_conn *imapc = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  Curl_pp_disconnect(&imapc->pp);
  curlx_safefree(imapc->mailbox);
  curlx_free(imapc);
}

/* SASL parameters for the imap protocol */
நிலையான மாறிலி கட்டமைப்பு SASLproto saslimap = {
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

நிலையான CURLcode imap_setup_connection(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு imap_conn *imapc;
  கட்டமைப்பு pingpong *pp;
  கட்டமைப்பு IMAP *imap;

  imapc = curlx_calloc(1, அளவு(*imapc));
  எனில்(!imapc)
    திரும்பு CURLE_OUT_OF_MEMORY;

  pp = &imapc->pp;
  PINGPONG_SETUP(pp, imap_pp_statemachine, imap_endofresp);

  /* Set the default preferred authentication type and mechanism */
  imapc->preftype = IMAP_TYPE_ANY;
  Curl_sasl_init(&imapc->sasl, data, &saslimap);

  Curl_pp_init(pp, Curl_pgrs_now(data));

  எனில்(Curl_conn_meta_set(conn, CURL_META_IMAP_CONN, imapc, imap_conn_dtor))
    திரும்பு CURLE_OUT_OF_MEMORY;

  imap = curlx_calloc(1, அளவு(கட்டமைப்பு IMAP));
  எனில்(!imap ||
     Curl_meta_set(data, CURL_META_IMAP_EASY, imap, imap_easy_dtor))
    திரும்பு CURLE_OUT_OF_MEMORY;

  திரும்பு CURLE_OK;
}

/*
 * IMAP protocol.
 */
மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_imap = {
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
