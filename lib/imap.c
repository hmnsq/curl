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
#include "curl_setup.h"
#include "urldata.h"
#include "imap.h"

#ifndef CURL_DISABLE_IMAP

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef __VMS
#include <in.h>
#include <inet.h>
#endif

#include "curlx/dynbuf.h"
#include "sendf.h"
#include "curl_trc.h"
#include "progress.h"
#include "transfer.h"
#include "escape.h"
#include "pingpong.h"
#include "mime.h"
#include "curlx/strparse.h"
#include "strcase.h"
#include "vtls/vtls.h"
#include "cfilters.h"
#include "connect.h"
#include "select.h"
#include "url.h"
#include "bufref.h"
#include "curl_sasl.h"
#include "curlx/strcopy.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_IMAP_EASY   "meta:proto:imap:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_IMAP_CONN   "meta:proto:imap:conn"

typedef enum {
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
  IMAP_SEARCH,
  IMAP_LOGOUT,
  IMAP_LAST          /* never used */
} imapstate;

/* imap_conn is used for struct connection-oriented data */
struct imap_conn {
  struct pingpong pp;
  struct SASL sasl;           /* SASL-related parameters */
  char *mailbox;              /* The last selected mailbox */
  imapstate state;            /* Always use imap.c:state() to change state! */
  unsigned ціле mb_uidvalidity; /* UIDVALIDITY parsed from select response */
  char resptag[5];            /* Response tag to wait for */
  unsigned char preftype;     /* Preferred authentication type */
  unsigned char cmdid;        /* Last used command ID */
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
struct IMAP {
  curl_pp_transfer transfer;
  char *mailbox;          /* Mailbox to select */
  char *uid;              /* Message UID to fetch */
  char *mindex;           /* Index in mail box of mail to fetch */
  char *section;          /* Message SECTION to fetch */
  char *partial;          /* Message PARTIAL to fetch */
  char *query;            /* Query to search for */
  char *custom;           /* Custom request */
  char *custom_params;    /* Parameters for the custom request */
  unsigned ціле uidvalidity; /* UIDVALIDITY to check in select */
  BIT(uidvalidity_set);
};

#define IMAP_RESP_OK       1
#define IMAP_RESP_NOT_OK   2
#define IMAP_RESP_PREAUTH  3

struct ulbits {
  ціле bit;
  const char *flag;
};

/***********************************************************************
 *
 * imap_sendf()
 *
 * Sends the formatted string as an IMAP command to the server.
 *
 * Designed to never block.
 */
static CURLcode imap_sendf(struct Curl_easy *data,
                           struct imap_conn *imapc,
                           const char *fmt, ...) CURL_PRINTF(3, 4);
static CURLcode imap_sendf(struct Curl_easy *data,
                           struct imap_conn *imapc,
                           const char *fmt, ...)
{
  CURLcode result = CURLE_OK;
  va_list ap;

  DEBUGASSERT(fmt);

  /* Calculate the tag based on the connection ID and command ID */
  curl_msnprintf(imapc->resptag, sizeof(imapc->resptag), "%c%03d",
                 'A' + curlx_sltosi((long)(data->conn->connection_id % 26)),
                 ++imapc->cmdid);

  va_start(ap, fmt);
  result = Curl_pp_vsendf(data, &imapc->pp, imapc->resptag, fmt, ap);
  va_end(ap);
  повернути result;
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
static char *imap_atom(const char *str, bool escape_only)
{
  struct dynbuf line;
  size_t nclean;
  size_t len;

  якщо(!str)
    повернути NULL;

  len = strlen(str);
  nclean = strcspn(str, "() {%*]\\\"");
  якщо(len == nclean)
    /* nothing to escape, return a strdup */
    повернути curlx_strdup(str);

  curlx_dyn_init(&line, 2000);

  якщо(!escape_only && curlx_dyn_addn(&line, "\"", 1))
    повернути NULL;

  поки(*str) {
    якщо((*str == '\\' || *str == '"') &&
       curlx_dyn_addn(&line, "\\", 1))
      повернути NULL;
    якщо(curlx_dyn_addn(&line, str, 1))
      повернути NULL;
    str++;
  }

  якщо(!escape_only && curlx_dyn_addn(&line, "\"", 1))
    повернути NULL;

  повернути curlx_dyn_ptr(&line);
}

/*
 * Finds the start of a literal '{size}' in line, skipping over quoted strings.
 */
static const char *imap_find_literal(const char *line, size_t len)
{
  const char *end = line + len;
  bool in_quote = FALSE;

  поки(line < end) {
    якщо(in_quote) {
      якщо(*line == '\\' && (line + 1) < end) {
        line += 2;
        continue;
      }
      якщо(*line == '"')
        in_quote = FALSE;
    }
    інакше {
      якщо(*line == '"')
        in_quote = TRUE;
      інакше якщо(*line == '{')
        повернути line;
    }
    line++;
  }
  повернути NULL;
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
static bool imap_matchresp(const char *line, size_t len, const char *cmd)
{
  const char *end = line + len;
  size_t cmd_len = strlen(cmd);

  /* Skip the untagged response marker */
  line += 2;

  /* Do we have a number after the marker? */
  якщо(line < end && ISDIGIT(*line)) {
    /* Skip the number */
    do
      line++;
    поки(line < end && ISDIGIT(*line));

    /* Do we have the space character? */
    якщо(line == end || *line != ' ')
      повернути FALSE;

    line++;
  }

  /* Does the command name match and is it followed by a space character or at
     the end of line? */
  якщо(line + cmd_len <= end && curl_strnequal(line, cmd, cmd_len) &&
     (line[cmd_len] == ' ' || line + cmd_len + 2 == end))
    повернути TRUE;

  повернути FALSE;
}

/***********************************************************************
 *
 * imap_endofresp()
 *
 * Checks whether the given string is a valid tagged, untagged or continuation
 * response which can be processed by the response handler.
 */
static bool imap_endofresp(struct Curl_easy *data, struct connectdata *conn,
                           const char *line, size_t len, ціле *resp)
{
  struct imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  struct IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  const char *id;
  size_t id_len;

  DEBUGASSERT(imapc);
  DEBUGASSERT(imap);
  якщо(!imapc || !imap)
    повернути FALSE;

  /* Do we have a tagged command response? */
  id = imapc->resptag;
  id_len = strlen(id);
  якщо(len >= id_len + 1 && !memcmp(id, line, id_len) && line[id_len] == ' ') {
    line += id_len + 1;
    len -= id_len + 1;

    якщо(len >= 2 && !memcmp(line, "OK", 2))
      *resp = IMAP_RESP_OK;
    інакше якщо(len >= 7 && !memcmp(line, "PREAUTH", 7))
      *resp = IMAP_RESP_PREAUTH;
    інакше
      *resp = IMAP_RESP_NOT_OK;

    повернути TRUE;
  }

  /* Do we have an untagged command response? */
  якщо(len >= 2 && !memcmp("* ", line, 2)) {
    switch(imapc->state) {
    /* States which are interested in untagged responses */
    case IMAP_CAPABILITY:
      якщо(!imap_matchresp(line, len, "CAPABILITY"))
        повернути FALSE;
      break;

    case IMAP_LIST:
      якщо((!imap->custom && !imap_matchresp(line, len, "LIST")) ||
         (imap->custom && !imap_matchresp(line, len, imap->custom) &&
          (!curl_strequal(imap->custom, "STORE") ||
           !imap_matchresp(line, len, "FETCH")) &&
          !curl_strequal(imap->custom, "SELECT") &&
          !curl_strequal(imap->custom, "EXAMINE") &&
          !curl_strequal(imap->custom, "SEARCH") &&
          !curl_strequal(imap->custom, "EXPUNGE") &&
          !curl_strequal(imap->custom, "LSUB") &&
          !curl_strequal(imap->custom, "UID") &&
          !curl_strequal(imap->custom, "GETQUOTAROOT") &&
          !curl_strequal(imap->custom, "NOOP")))
        повернути FALSE;
      break;

    case IMAP_SELECT:
      /* SELECT is special in that its untagged responses do not have a
         common prefix so accept anything! */
      break;

    case IMAP_FETCH:
      якщо(!imap_matchresp(line, len, "FETCH"))
        повернути FALSE;
      break;

    case IMAP_SEARCH:
      якщо(!imap_matchresp(line, len, "SEARCH"))
        повернути FALSE;
      break;

    /* Ignore other untagged responses */
    default:
      повернути FALSE;
    }

    *resp = '*';
    повернути TRUE;
  }

  /* Do we have a continuation response? This should be a + symbol followed by
     a space and optionally some text as per RFC-3501 for the AUTHENTICATE and
     APPEND commands and as outlined in Section 4. Examples of RFC-4959 but
     some email servers ignore this and only send a single + instead. */
  якщо(!imap->custom && ((len == 3 && line[0] == '+') ||
                       (len >= 2 && !memcmp("+ ", line, 2)))) {
    switch(imapc->state) {
    /* States which are interested in continuation responses */
    case IMAP_AUTHENTICATE:
    case IMAP_APPEND:
      *resp = '+';
      break;

    default:
      failf(data, "Unexpected continuation response");
      *resp = -1;
      break;
    }

    повернути TRUE;
  }

  повернути FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * imap_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
static CURLcode imap_get_message(struct Curl_easy *data, struct bufref *out)
{
  struct imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  char *message;
  size_t len;

  якщо(!imapc)
    повернути CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&imapc->pp.recvbuf);
  len = imapc->pp.nfinal;
  якщо(len > 2) {
    /* Find the start of the message */
    len -= 2;
    для(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    поки(len--)
      якщо(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        break;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, NULL);
  }
  інакше
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, NULL);

  повернути CURLE_OK;
}

/***********************************************************************
 *
 * imap_state()
 *
 * This is the ONLY way to change IMAP state!
 */
static void imap_state(struct Curl_easy *data,
                       struct imap_conn *imapc,
                       imapstate newstate)
{
#якщо defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  static const char * const names[] = {
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

  якщо(imapc->state != newstate)
    infof(data, "IMAP %p state change from %s to %s",
          (void *)imapc, names[imapc->state], names[newstate]);
#інакше
  (void)data;
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
static CURLcode imap_perform_capability(struct Curl_easy *data,
                                        struct imap_conn *imapc)
{
  CURLcode result = CURLE_OK;

  imapc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  imapc->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  imapc->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPABILITY command */
  result = imap_sendf(data, imapc, "CAPABILITY");

  якщо(!result)
    imap_state(data, imapc, IMAP_CAPABILITY);

  повернути result;
}

/***********************************************************************
 *
 * imap_perform_starttls()
 *
 * Sends the STARTTLS command to start the upgrade to TLS.
 */
static CURLcode imap_perform_starttls(struct Curl_easy *data,
                                      struct imap_conn *imapc)
{
  /* Send the STARTTLS command */
  CURLcode result = imap_sendf(data, imapc, "STARTTLS");

  якщо(!result)
    imap_state(data, imapc, IMAP_STARTTLS);

  повернути result;
}

/***********************************************************************
 *
 * imap_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
static CURLcode imap_perform_upgrade_tls(struct Curl_easy *data,
                                         struct imap_conn *imapc,
                                         struct connectdata *conn)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  CURLcode result;
  bool ssldone = FALSE;

  якщо(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    якщо(result)
      goto out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_imaps;
  }

  DEBUGASSERT(!imapc->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "imap_perform_upgrade_tls, connect -> %d, %d",
               (ціле)result, ssldone));
  якщо(!result && ssldone) {
    imapc->ssldone = ssldone;
    /* perform CAPA now, changes imapc->state out of IMAP_UPGRADETLS */
    result = imap_perform_capability(data, imapc);
  }
out:
  повернути result;
#інакше
  (void)data;
  (void)imapc;
  (void)conn;
  повернути CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * imap_perform_login()
 *
 * Sends a clear text LOGIN command to authenticate with.
 */
static CURLcode imap_perform_login(struct Curl_easy *data,
                                   struct imap_conn *imapc,
                                   struct connectdata *conn)
{
  CURLcode result = CURLE_OK;
  char *user;
  char *passwd;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  якщо(!conn->creds) {
    imap_state(data, imapc, IMAP_STOP);

    повернути result;
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

  якщо(!result)
    imap_state(data, imapc, IMAP_LOGIN);

  повернути result;
}

/***********************************************************************
 *
 * imap_perform_authenticate()
 *
 * Sends an AUTHENTICATE command allowing the client to login with the given
 * SASL authentication mechanism.
 */
static CURLcode imap_perform_authenticate(struct Curl_easy *data,
                                          const char *mech,
                                          const struct bufref *initresp)
{
  struct imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  CURLcode result = CURLE_OK;
  const char *ir = Curl_bufref_ptr(initresp);

  якщо(!imapc)
    повернути CURLE_FAILED_INIT;
  якщо(ir) {
    /* Send the AUTHENTICATE command with the initial response */
    result = imap_sendf(data, imapc, "AUTHENTICATE %s %s",
                        mech, *ir ? ir : "=");
  }
  інакше {
    /* Send the AUTHENTICATE command */
    result = imap_sendf(data, imapc, "AUTHENTICATE %s", mech);
  }

  повернути result;
}

/***********************************************************************
 *
 * imap_continue_authenticate()
 *
 * Sends SASL continuation data.
 */
static CURLcode imap_continue_authenticate(struct Curl_easy *data,
                                           const char *mech,
                                           const struct bufref *resp)
{
  struct imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  (void)mech;
  якщо(!imapc)
    повернути CURLE_FAILED_INIT;
  повернути Curl_pp_sendf(data, &imapc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * imap_cancel_authenticate()
 *
 * Sends SASL cancellation.
 */
static CURLcode imap_cancel_authenticate(struct Curl_easy *data,
                                         const char *mech)
{
  struct imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  (void)mech;
  якщо(!imapc)
    повернути CURLE_FAILED_INIT;
  повернути Curl_pp_sendf(data, &imapc->pp, "*");
}

/***********************************************************************
 *
 * imap_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to clear text should a common
 * mechanism not be available between the client and server.
 */
static CURLcode imap_perform_authentication(struct Curl_easy *data,
                                            struct imap_conn *imapc)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  /* Check if already authenticated OR if there is enough data to authenticate
     with and end the connect phase if we do not */
  якщо(imapc->preauth ||
     !Curl_sasl_can_authenticate(&imapc->sasl, data)) {
    imap_state(data, imapc, IMAP_STOP);
    повернути result;
  }

  /* Calculate the SASL login details */
  result = Curl_sasl_start(&imapc->sasl, data, (bool)imapc->ir_supported,
                           &progress);
  якщо(!result) {
    якщо(progress == SASL_INPROGRESS)
      imap_state(data, imapc, IMAP_AUTHENTICATE);
    інакше якщо(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
      /* Perform clear text authentication */
      result = imap_perform_login(data, imapc, data->conn);
    інакше
      result = Curl_sasl_is_blocked(&imapc->sasl, data);
  }

  повернути result;
}

/***********************************************************************
 *
 * imap_perform_list()
 *
 * Sends a LIST command or an alternative custom request.
 */
static CURLcode imap_perform_list(struct Curl_easy *data,
                                  struct imap_conn *imapc,
                                  struct IMAP *imap)
{
  CURLcode result = CURLE_OK;

  якщо(imap->custom)
    /* Send the custom request */
    result = imap_sendf(data, imapc, "%s%s", imap->custom,
                        imap->custom_params ? imap->custom_params : "");
  інакше {
    /* Make sure the mailbox is in the correct atom format if necessary */
    char *mailbox = imap->mailbox ? imap_atom(imap->mailbox, TRUE)
                                  : curlx_strdup("");
    якщо(!mailbox)
      повернути CURLE_OUT_OF_MEMORY;

    /* Send the LIST command */
    result = imap_sendf(data, imapc, "LIST \"%s\" *", mailbox);

    curlx_free(mailbox);
  }

  якщо(!result)
    imap_state(data, imapc, IMAP_LIST);

  повернути result;
}

/***********************************************************************
 *
 * imap_perform_select()
 *
 * Sends a SELECT command to ask the server to change the selected mailbox.
 */
static CURLcode imap_perform_select(struct Curl_easy *data,
                                    struct imap_conn *imapc,
                                    struct IMAP *imap)
{
  CURLcode result = CURLE_OK;
  char *mailbox;

  /* Invalidate old information as we are switching mailboxes */
  curlx_safefree(imapc->mailbox);
  imapc->mb_uidvalidity_set = FALSE;

  /* Check we have a mailbox */
  якщо(!imap->mailbox) {
    failf(data, "Cannot SELECT without a mailbox.");
    повернути CURLE_URL_MALFORMAT;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  якщо(!mailbox)
    повернути CURLE_OUT_OF_MEMORY;

  /* Send the SELECT command */
  result = imap_sendf(data, imapc, "SELECT %s", mailbox);

  curlx_free(mailbox);

  якщо(!result)
    imap_state(data, imapc, IMAP_SELECT);

  повернути result;
}

/***********************************************************************
 *
 * imap_perform_fetch()
 *
 * Sends a FETCH command to initiate the download of a message.
 */
static CURLcode imap_perform_fetch(struct Curl_easy *data,
                                   struct imap_conn *imapc,
                                   struct IMAP *imap)
{
  CURLcode result = CURLE_OK;
  /* Check we have a UID */
  якщо(imap->uid) {

    /* Send the FETCH command */
    якщо(imap->partial)
      result = imap_sendf(data, imapc, "UID FETCH %s BODY[%s]<%s>",
                          imap->uid, imap->section ? imap->section : "",
                          imap->partial);
    інакше
      result = imap_sendf(data, imapc, "UID FETCH %s BODY[%s]",
                          imap->uid, imap->section ? imap->section : "");
  }
  інакше якщо(imap->mindex) {
    /* Send the FETCH command */
    якщо(imap->partial)
      result = imap_sendf(data, imapc, "FETCH %s BODY[%s]<%s>",
                          imap->mindex, imap->section ? imap->section : "",
                          imap->partial);
    інакше
      result = imap_sendf(data, imapc, "FETCH %s BODY[%s]",
                          imap->mindex, imap->section ? imap->section : "");
  }
  інакше {
    failf(data, "Cannot FETCH without a UID.");
    повернути CURLE_URL_MALFORMAT;
  }
  якщо(!result)
    imap_state(data, imapc, IMAP_FETCH);

  повернути result;
}

/***********************************************************************
 *
 * imap_perform_append()
 *
 * Sends an APPEND command to initiate the upload of a message.
 */
static CURLcode imap_perform_append(struct Curl_easy *data,
                                    struct imap_conn *imapc,
                                    struct IMAP *imap)
{
  CURLcode result = CURLE_OK;
  char *mailbox;
  struct dynbuf flags;

  /* Check we have a mailbox */
  якщо(!imap->mailbox) {
    failf(data, "Cannot APPEND without a mailbox.");
    повернути CURLE_URL_MALFORMAT;
  }

#ifndef CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  якщо(IS_MIME_POST(data)) {
    curl_mimepart *postp = data->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(unsigned ціле)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, data->set.headers, 0);
    result = Curl_mime_prepare_headers(data, postp, NULL,
                                       NULL, MIMESTRATEGY_MAIL);

    якщо(!result && !Curl_checkheaders(data, STRCONST("Mime-Version")))
      result = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    якщо(!result)
      result = Curl_creader_set_mime(data, postp);
    якщо(result)
      повернути result;
  }
  інакше
#endif
  {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    якщо(result)
      повернути result;
  }

  /* Check we know the size of the upload. This takes all readers
   * into account. Especially crlf conversions which make the size
   * unpredictable, e.g. -1. */
  data->state.infilesize = Curl_creader_total_length(data);
  якщо(data->state.infilesize < 0) {
    якщо(data->set.crlf)
      failf(data, "Cannot APPEND with CRLF conversion making size unknown");
    інакше
      failf(data, "Cannot APPEND with unknown input file size");
    повернути CURLE_UPLOAD_FAILED;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  якщо(!mailbox)
    повернути CURLE_OUT_OF_MEMORY;

  /* Generate flags string and send the APPEND command */
  curlx_dyn_init(&flags, 100);
  якщо(data->set.upload_flags) {
    ціле i;
    struct ulbits ulflag[] = {
      { CURLULFLAG_ANSWERED, "Answered" },
      { CURLULFLAG_DELETED, "Deleted" },
      { CURLULFLAG_DRAFT, "Draft" },
      { CURLULFLAG_FLAGGED, "Flagged" },
      { CURLULFLAG_SEEN, "Seen" },
      { 0, NULL }
    };

    result = CURLE_OUT_OF_MEMORY;
    якщо(curlx_dyn_add(&flags, " (")) {
      goto cleanup;
    }

    для(i = 0; ulflag[i].bit; i++) {
      якщо(data->set.upload_flags & ulflag[i].bit &&
         ((curlx_dyn_len(&flags) > 2 && curlx_dyn_add(&flags, " ")) ||
          curlx_dyn_add(&flags, "\\") ||
          curlx_dyn_add(&flags, ulflag[i].flag)))
        goto cleanup;
    }

    якщо(curlx_dyn_add(&flags, ")"))
      goto cleanup;
  }
  інакше якщо(curlx_dyn_add(&flags, ""))
    goto cleanup;

  result = imap_sendf(data, imapc, "APPEND %s%s {%" FMT_OFF_T "}",
                      mailbox, curlx_dyn_ptr(&flags), data->state.infilesize);

cleanup:
  curlx_dyn_free(&flags);
  curlx_free(mailbox);

  якщо(!result)
    imap_state(data, imapc, IMAP_APPEND);

  повернути result;
}

/***********************************************************************
 *
 * imap_perform_search()
 *
 * Sends a SEARCH command.
 */
static CURLcode imap_perform_search(struct Curl_easy *data,
                                    struct imap_conn *imapc,
                                    struct IMAP *imap)
{
  CURLcode result = CURLE_OK;

  /* Check we have a query string */
  якщо(!imap->query) {
    failf(data, "Cannot SEARCH without a query string.");
    повернути CURLE_URL_MALFORMAT;
  }

  /* Send the SEARCH command */
  result = imap_sendf(data, imapc, "SEARCH %s", imap->query);

  якщо(!result)
    imap_state(data, imapc, IMAP_SEARCH);

  повернути result;
}

/***********************************************************************
 *
 * imap_perform_logout()
 *
 * Performs the logout action prior to sclose() being called.
 */
static CURLcode imap_perform_logout(struct Curl_easy *data,
                                    struct imap_conn *imapc)
{
  /* Send the LOGOUT command */
  CURLcode result = imap_sendf(data, imapc, "LOGOUT");

  якщо(!result)
    imap_state(data, imapc, IMAP_LOGOUT);

  повернути result;
}

/* For the initial server greeting */
static CURLcode imap_state_servergreet_resp(struct Curl_easy *data,
                                            struct imap_conn *imapc,
                                            ціле imapcode,
                                            imapstate instate)
{
  (void)instate;

  якщо(imapcode == IMAP_RESP_PREAUTH) {
    /* PREAUTH */
    imapc->preauth = TRUE;
    infof(data, "PREAUTH connection, already authenticated");
  }
  інакше якщо(imapcode != IMAP_RESP_OK) {
    failf(data, "Got unexpected imap-server response");
    повернути CURLE_WEIRD_SERVER_REPLY;
  }

  повернути imap_perform_capability(data, imapc);
}

/* For CAPABILITY responses */
static CURLcode imap_state_capability_resp(struct Curl_easy *data,
                                           struct imap_conn *imapc,
                                           ціле imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  const char *line = curlx_dyn_ptr(&imapc->pp.recvbuf);

  (void)instate;

  /* Do we have an untagged response? */
  якщо(imapcode == '*') {
    line += 2;

    /* Loop through the data line */
    для(;;) {
      size_t wordlen;
      поки(*line && (ISBLANK(*line) || ISNEWLINE(*line)))
        line++;

      якщо(!*line)
        break;

      /* Extract the word */
      для(wordlen = 0; line[wordlen] && !ISBLANK(line[wordlen]) &&
                       !ISNEWLINE(line[wordlen]);)
        wordlen++;

      /* Does the server support the STARTTLS capability? */
      якщо(wordlen == 8 && curl_strnequal(line, "STARTTLS", 8))
        imapc->tls_supported = TRUE;

      /* Has the server explicitly disabled clear text authentication? */
      інакше якщо(wordlen == 13 && curl_strnequal(line, "LOGINDISABLED", 13))
        imapc->login_disabled = TRUE;

      /* Does the server support the SASL-IR capability? */
      інакше якщо(wordlen == 7 && curl_strnequal(line, "SASL-IR", 7))
        imapc->ir_supported = TRUE;

      /* Do we have a SASL based authentication mechanism? */
      інакше якщо(wordlen > 5 && curl_strnequal(line, "AUTH=", 5)) {
        size_t llen;
        unsigned short mechbit;

        line += 5;
        wordlen -= 5;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(line, wordlen, &llen);
        якщо(mechbit && llen == wordlen)
          imapc->sasl.authmechs |= mechbit;
      }

      line += wordlen;
    }
  }
  інакше якщо(data->set.use_ssl && !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
    /* PREAUTH is not compatible with STARTTLS. */
    якщо(imapcode == IMAP_RESP_OK && imapc->tls_supported && !imapc->preauth) {
      /* Switch to TLS connection now */
      result = imap_perform_starttls(data, imapc);
    }
    інакше якщо(data->set.use_ssl <= CURLUSESSL_TRY)
      result = imap_perform_authentication(data, imapc);
    інакше {
      failf(data, "STARTTLS not available.");
      result = CURLE_USE_SSL_FAILED;
    }
  }
  інакше
    result = imap_perform_authentication(data, imapc);

  повернути result;
}

/* For STARTTLS responses */
static CURLcode imap_state_starttls_resp(struct Curl_easy *data,
                                         struct imap_conn *imapc,
                                         ціле imapcode,
                                         imapstate instate)
{
  CURLcode result = CURLE_OK;

  (void)instate;

  /* Pipelining in response is forbidden. */
  якщо(imapc->pp.overflow)
    повернути CURLE_WEIRD_SERVER_REPLY;

  якщо(imapcode != IMAP_RESP_OK) {
    якщо(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied");
      result = CURLE_USE_SSL_FAILED;
    }
    інакше
      result = imap_perform_authentication(data, imapc);
  }
  інакше
    imap_state(data, imapc, IMAP_UPGRADETLS);

  повернути result;
}

/* For SASL authentication responses */
static CURLcode imap_state_auth_resp(struct Curl_easy *data,
                                     struct imap_conn *imapc,
                                     ціле imapcode,
                                     imapstate instate)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  (void)instate;

  result = Curl_sasl_continue(&imapc->sasl, data, imapcode, &progress);
  якщо(!result)
    switch(progress) {
    case SASL_DONE:
      imap_state(data, imapc, IMAP_STOP);  /* Authenticated */
      break;
    case SASL_IDLE:            /* No mechanism left after cancellation */
      якщо(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
        /* Perform clear text authentication */
        result = imap_perform_login(data, imapc, data->conn);
      інакше {
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
      }
      break;
    default:
      break;
    }

  повернути result;
}

/* For LOGIN responses */
static CURLcode imap_state_login_resp(struct Curl_easy *data,
                                      struct imap_conn *imapc,
                                      ціле imapcode,
                                      imapstate instate)
{
  CURLcode result = CURLE_OK;
  (void)instate;

  якщо(imapcode != IMAP_RESP_OK) {
    failf(data, "Access denied. %c", imapcode);
    result = CURLE_LOGIN_DENIED;
  }
  інакше
    /* End of connect phase */
    imap_state(data, imapc, IMAP_STOP);

  повернути result;
}

/* Detect IMAP listings vs. downloading a single email */
static bool is_custom_fetch_listing_match(const char *params)
{
  /* match " 1:* (FLAGS ..." or " 1,2,3 (FLAGS ..." */
  якщо(*params++ != ' ')
    повернути FALSE;

  поки(ISDIGIT(*params)) {
    params++;
    якщо(*params == 0)
      повернути FALSE;
  }
  якщо(*params == ':')
    повернути TRUE;
  якщо(*params == ',')
    повернути TRUE;
  повернути FALSE;
}

static bool is_custom_fetch_listing(struct IMAP *imap)
{
  /* filter out "UID FETCH 1:* (FLAGS ..." queries to list emails */
  якщо(!imap->custom)
    повернути FALSE;
  інакше якщо(curl_strequal(imap->custom, "FETCH") && imap->custom_params) {
    const char *p = imap->custom_params;
    повернути is_custom_fetch_listing_match(p);
  }
  інакше якщо(curl_strequal(imap->custom, "UID") && imap->custom_params &&
          curl_strnequal(imap->custom_params, " FETCH ", 7)) {
    const char *p = imap->custom_params + 6;
    повернути is_custom_fetch_listing_match(p);
  }
  повернути FALSE;
}

/* For LIST and SEARCH responses */
static CURLcode imap_state_listsearch_resp(struct Curl_easy *data,
                                           struct imap_conn *imapc,
                                           ціле imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  const char *line = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  struct IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);

  DEBUGASSERT(imap);
  якщо(!imap)
    повернути CURLE_FAILED_INIT;
  (void)instate;

  якщо(imapcode == '*' && is_custom_fetch_listing(imap)) {
    /* custom FETCH or UID FETCH for listing is not handled here */
  }
  інакше якщо(imapcode == '*') {
    /* Check if this response contains a literal (e.g. FETCH responses with
       body data). Literal syntax is {size}\r\n */
    const char *cr = memchr(line, '\r', len);
    size_t line_len = cr ? (size_t)(cr - line) : len;
    const char *ptr = imap_find_literal(line, line_len);
    якщо(ptr) {
      curl_off_t size = 0;
      bool parsed = FALSE;
      ptr++;
      якщо(!curlx_str_number(&ptr, &size, CURL_OFF_T_MAX) &&
         !curlx_str_single(&ptr, '}'))
        parsed = TRUE;

      якщо(parsed) {
        struct pingpong *pp = &imapc->pp;
        size_t buffer_len = curlx_dyn_len(&pp->recvbuf);
        size_t after_header = buffer_len - pp->nfinal;

        /* This is a literal response, setup to receive the body data */
        infof(data, "Found %" FMT_OFF_T " bytes to download", size);

        /* First write the header line */
        result = Curl_client_write(data, CLIENTWRITE_BODY, line, len);
        якщо(result)
          повернути result;

        /* Handle data already in buffer after the header line */
        якщо(after_header > 0) {
          /* There is already data in the buffer that is part of the literal
             body or subsequent responses */
          size_t chunk = after_header;

          /* Keep only the data after the header line */
          curlx_dyn_tail(&pp->recvbuf, chunk);
          pp->nfinal = 0; /* done */

          /* Limit chunk to the literal size */
          якщо((curl_off_t)chunk > size)
            chunk = (size_t)size;

          якщо(chunk) {
            /* Write the literal body data */
            result = Curl_client_write(data, CLIENTWRITE_BODY,
                                       curlx_dyn_ptr(&pp->recvbuf), chunk);
            якщо(result)
              повернути result;
          }

          /* Handle remaining data in buffer (either more literal data or
             subsequent responses) */
          якщо(after_header > chunk) {
            /* Keep the data after the literal body */
            pp->overflow = after_header - chunk;
            curlx_dyn_tail(&pp->recvbuf, pp->overflow);
          }
          інакше {
            pp->overflow = 0;
            curlx_dyn_reset(&pp->recvbuf);
          }
        }
        інакше {
          /* No data in buffer yet, reset overflow */
          pp->overflow = 0;
        }

        якщо((CURL_OFF_T_MAX - size) < (curl_off_t)len)
          /* unlikely to actually be a transfer this big, but avoid integer
             overflow */
          size = CURL_OFF_T_MAX;
        інакше
          size += len;

        /* Progress size includes both header line and literal body */
        Curl_pgrsSetDownloadSize(data, size);

        якщо(data->req.bytecount == size)
          /* All data already transferred (header + literal body) */
          Curl_xfer_setup_nop(data);
        інакше {
          /* Setup to receive the literal body data.
             maxdownload and transfer size include both header line and
             literal body */
          data->req.maxdownload = size;
          Curl_xfer_setup_recv(data, FIRSTSOCKET, size);
        }
        /* End of DO phase */
        imap_state(data, imapc, IMAP_STOP);
      }
      інакше {
        /* Failed to parse literal, write the line */
        result = Curl_client_write(data, CLIENTWRITE_BODY, line, len);
      }
    }
    інакше {
      /* No literal, write the line as-is */
      result = Curl_client_write(data, CLIENTWRITE_BODY, line, len);
    }
  }
  інакше якщо(imapcode != IMAP_RESP_OK)
    result = CURLE_QUOTE_ERROR;
  інакше
    /* End of DO phase */
    imap_state(data, imapc, IMAP_STOP);

  повернути result;
}

/* For SELECT responses */
static CURLcode imap_state_select_resp(struct Curl_easy *data,
                                       struct imap_conn *imapc,
                                       struct IMAP *imap,
                                       ціле imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  (void)instate;

  якщо(imapcode == '*') {
    /* See if this is an UIDVALIDITY response */
    const char *line = curlx_dyn_ptr(&imapc->pp.recvbuf);
    size_t len = curlx_dyn_len(&imapc->pp.recvbuf);
    якщо((len >= 18) && checkprefix("OK [UIDVALIDITY ", &line[2])) {
      curl_off_t value;
      const char *p = &line[2] + CURL_CSTRLEN("OK [UIDVALIDITY ");
      якщо(!curlx_str_number(&p, &value, UINT_MAX)) {
        imapc->mb_uidvalidity = (unsigned ціле)value;
        imapc->mb_uidvalidity_set = TRUE;
      }
    }
  }
  інакше якщо(imapcode == IMAP_RESP_OK) {
    /* Check if the UIDVALIDITY has been specified and matches */
    якщо(imap->uidvalidity_set && imapc->mb_uidvalidity_set &&
       (imap->uidvalidity != imapc->mb_uidvalidity)) {
      failf(data, "Mailbox UIDVALIDITY has changed");
      result = CURLE_REMOTE_FILE_NOT_FOUND;
    }
    інакше {
      /* Note the currently opened mailbox on this connection */
      DEBUGASSERT(!imapc->mailbox);
      imapc->mailbox = curlx_strdup(imap->mailbox);
      якщо(!imapc->mailbox)
        повернути CURLE_OUT_OF_MEMORY;

      якщо(imap->custom)
        result = imap_perform_list(data, imapc, imap);
      інакше якщо(imap->query)
        result = imap_perform_search(data, imapc, imap);
      інакше
        result = imap_perform_fetch(data, imapc, imap);
    }
  }
  інакше {
    failf(data, "Select failed");
    result = CURLE_LOGIN_DENIED;
  }

  повернути result;
}

/* For the (first line of the) FETCH responses */
static CURLcode imap_state_fetch_resp(struct Curl_easy *data,
                                      struct imap_conn *imapc,
                                      ціле imapcode,
                                      imapstate instate)
{
  CURLcode result = CURLE_OK;
  struct pingpong *pp = &imapc->pp;
  const char *ptr = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  bool parsed = FALSE;
  curl_off_t size = 0;

  (void)instate;

  якщо(imapcode != '*') {
    Curl_pgrsSetDownloadSize(data, -1);
    imap_state(data, imapc, IMAP_STOP);
    повернути CURLE_REMOTE_FILE_NOT_FOUND;
  }

  /* Something like this is received "* 1 FETCH (BODY[TEXT] {2021}\r" so parse
     the continuation data contained within the curly brackets */
  ptr = imap_find_literal(ptr, len);
  якщо(ptr) {
    ptr++;
    якщо(!curlx_str_number(&ptr, &size, CURL_OFF_T_MAX) &&
       !curlx_str_single(&ptr, '}'))
      parsed = TRUE;
  }

  якщо(parsed) {
    infof(data, "Found %" FMT_OFF_T " bytes to download", size);
    Curl_pgrsSetDownloadSize(data, size);

    якщо(pp->overflow) {
      /* At this point there is a data in the receive buffer that is body
         content, send it as body and then skip it. Do note that there may
         even be additional "headers" after the body. */
      size_t chunk = pp->overflow;

      /* keep only the overflow */
      curlx_dyn_tail(&pp->recvbuf, chunk);
      pp->nfinal = 0; /* done */

      якщо((curl_off_t)chunk > size)
        /* The conversion from curl_off_t to size_t is always fine here */
        chunk = (size_t)size;

      якщо(!chunk) {
        /* no size, we are done with the data */
        imap_state(data, imapc, IMAP_STOP);
        повернути CURLE_OK;
      }
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 curlx_dyn_ptr(&pp->recvbuf), chunk);
      якщо(result)
        повернути result;

      infof(data, "Written %zu bytes, %" FMT_OFF_T
            " bytes are left for transfer", chunk, (curl_off_t)(size - chunk));

      /* Have we used the entire overflow or part of it?*/
      якщо(pp->overflow > chunk) {
        /* remember the remaining trailing overflow data */
        pp->overflow -= chunk;
        curlx_dyn_tail(&pp->recvbuf, pp->overflow);
      }
      інакше {
        pp->overflow = 0; /* handled */
        /* Free the cache */
        curlx_dyn_reset(&pp->recvbuf);
      }
    }

    якщо(data->req.bytecount == size)
      /* The entire data is already transferred! */
      Curl_xfer_setup_nop(data);
    інакше {
      /* IMAP download */
      data->req.maxdownload = size;
      Curl_xfer_setup_recv(data, FIRSTSOCKET, size);
    }
  }
  інакше {
    /* We do not know how to parse this line */
    failf(data, "Failed to parse FETCH response.");
    result = CURLE_WEIRD_SERVER_REPLY;
  }

  /* End of DO phase */
  imap_state(data, imapc, IMAP_STOP);

  повернути result;
}

/* For final FETCH responses performed after the download */
static CURLcode imap_state_fetch_final_resp(struct Curl_easy *data,
                                            struct imap_conn *imapc,
                                            ціле imapcode,
                                            imapstate instate)
{
  CURLcode result = CURLE_OK;

  (void)instate;

  якщо(imapcode != IMAP_RESP_OK)
    result = CURLE_WEIRD_SERVER_REPLY;
  інакше
    /* End of DONE phase */
    imap_state(data, imapc, IMAP_STOP);

  повернути result;
}

/* For APPEND responses */
static CURLcode imap_state_append_resp(struct Curl_easy *data,
                                       struct imap_conn *imapc,
                                       ціле imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  (void)instate;

  якщо(imapcode != '+') {
    result = CURLE_UPLOAD_FAILED;
  }
  інакше {
    /* Set the progress upload size */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* IMAP upload */
    Curl_xfer_setup_send(data, FIRSTSOCKET);

    /* End of DO phase */
    imap_state(data, imapc, IMAP_STOP);
  }

  повернути result;
}

/* For final APPEND responses performed after the upload */
static CURLcode imap_state_append_final_resp(struct Curl_easy *data,
                                             struct imap_conn *imapc,
                                             ціле imapcode,
                                             imapstate instate)
{
  CURLcode result = CURLE_OK;

  (void)instate;

  якщо(imapcode != IMAP_RESP_OK)
    result = CURLE_UPLOAD_FAILED;
  інакше
    /* End of DONE phase */
    imap_state(data, imapc, IMAP_STOP);

  повернути result;
}

static CURLcode imap_pp_statemachine(struct Curl_easy *data,
                                     struct connectdata *conn)
{
  CURLcode result = CURLE_OK;
  ціле imapcode;
  struct imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  struct IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  struct pingpong *pp;
  size_t nread = 0;

  якщо(!imapc || !imap)
    повернути CURLE_FAILED_INIT;
  pp = &imapc->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not IMAP */
upgrade_tls:
  якщо(imapc->state == IMAP_UPGRADETLS) {
    result = imap_perform_upgrade_tls(data, imapc, conn);
    якщо(result || (imapc->state == IMAP_UPGRADETLS))
      повернути result;
  }

  /* Flush any data that needs to be sent */
  якщо(pp->sendleft)
    повернути Curl_pp_flushsend(data, pp);

  do {
    /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, pp, &imapcode, &nread);
    якщо(result)
      повернути result;

    /* Was there an error parsing the response line? */
    якщо(imapcode == -1)
      повернути CURLE_WEIRD_SERVER_REPLY;

    якщо(!imapcode)
      break;

    /* We have now received a full IMAP server response */
    switch(imapc->state) {
    case IMAP_SERVERGREET:
      result = imap_state_servergreet_resp(data, imapc,
                                           imapcode, imapc->state);
      break;

    case IMAP_CAPABILITY:
      result = imap_state_capability_resp(data, imapc, imapcode, imapc->state);
      break;

    case IMAP_STARTTLS:
      result = imap_state_starttls_resp(data, imapc, imapcode, imapc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      якщо(!result && (imapc->state == IMAP_UPGRADETLS))
        goto upgrade_tls;
      break;

    case IMAP_AUTHENTICATE:
      result = imap_state_auth_resp(data, imapc, imapcode, imapc->state);
      break;

    case IMAP_LOGIN:
      result = imap_state_login_resp(data, imapc, imapcode, imapc->state);
      break;

    case IMAP_LIST:
    case IMAP_SEARCH:
      result = imap_state_listsearch_resp(data, imapc, imapcode, imapc->state);
      break;

    case IMAP_SELECT:
      result = imap_state_select_resp(data, imapc, imap,
                                      imapcode, imapc->state);
      break;

    case IMAP_FETCH:
      result = imap_state_fetch_resp(data, imapc, imapcode, imapc->state);
      break;

    case IMAP_FETCH_FINAL:
      result = imap_state_fetch_final_resp(data, imapc,
                                           imapcode, imapc->state);
      break;

    case IMAP_APPEND:
      result = imap_state_append_resp(data, imapc, imapcode, imapc->state);
      break;

    case IMAP_APPEND_FINAL:
      result = imap_state_append_final_resp(data, imapc,
                                            imapcode, imapc->state);
      break;

    case IMAP_LOGOUT:
    default:
      /* internal error */
      imap_state(data, imapc, IMAP_STOP);
      break;
    }
  } поки(!result && imapc->state != IMAP_STOP && Curl_pp_moredata(pp));

  повернути result;
}

/* Called repeatedly until done from multi.c */
static CURLcode imap_multi_statemach(struct Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  struct imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  *done = FALSE;
  якщо(!imapc)
    повернути CURLE_FAILED_INIT;
  result = Curl_pp_statemach(data, &imapc->pp, FALSE, FALSE);
  *done = (imapc->state == IMAP_STOP);

  повернути result;
}

static CURLcode imap_block_statemach(struct Curl_easy *data,
                                     struct imap_conn *imapc,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;

  поки(imapc->state != IMAP_STOP && !result)
    result = Curl_pp_statemach(data, &imapc->pp, TRUE, disconnecting);

  повернути result;
}

/* For the IMAP "protocol connect" and "doing" phases only */
static CURLcode imap_pollset(struct Curl_easy *data,
                             struct easy_pollset *ps)
{
  struct imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  повернути imapc ? Curl_pp_pollset(data, &imapc->pp, ps) : CURLE_OK;
}

static void imap_easy_reset(struct IMAP *imap)
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
static bool imap_is_bchar(char ch)
{
  /* Performing the alnum check first with macro is faster because of ASCII
     arithmetic */
  повернути ch && (ISALNUM(ch) || strchr(":@/&=-._~!$\'()*+,%", ch));
}

/***********************************************************************
 *
 * imap_parse_url_options()
 *
 * Parse the URL login options.
 */
static CURLcode imap_parse_url_options(struct connectdata *conn,
                                       struct imap_conn *imapc)
{
  CURLcode result = CURLE_OK;
  const char *ptr = conn->options;
  bool prefer_login = FALSE;

  поки(!result && ptr && *ptr) {
    const char *key = ptr;
    const char *value;

    поки(*ptr && *ptr != '=')
      ptr++;

    value = ptr + 1;

    поки(*ptr && *ptr != ';')
      ptr++;

    якщо(curl_strnequal(key, "AUTH=+LOGIN", 11)) {
      /* User prefers plaintext LOGIN over any SASL, including SASL LOGIN */
      prefer_login = TRUE;
      imapc->sasl.prefmech = SASL_AUTH_NONE;
    }
    інакше якщо(curl_strnequal(key, "AUTH=", 5)) {
      prefer_login = FALSE;
      result = Curl_sasl_parse_url_auth_option(&imapc->sasl,
                                               value, ptr - value);
    }
    інакше {
      prefer_login = FALSE;
      result = CURLE_URL_MALFORMAT;
    }

    якщо(*ptr == ';')
      ptr++;
  }

  якщо(prefer_login)
    imapc->preftype = IMAP_TYPE_CLEARTEXT;
  інакше {
    switch(imapc->sasl.prefmech) {
    case SASL_AUTH_NONE:
      imapc->preftype = IMAP_TYPE_NONE;
      break;
    case SASL_AUTH_DEFAULT:
      imapc->preftype = IMAP_TYPE_ANY;
      break;
    default:
      imapc->preftype = IMAP_TYPE_SASL;
      break;
    }
  }

  повернути result;
}

/***********************************************************************
 *
 * imap_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
static CURLcode imap_parse_url_path(struct Curl_easy *data,
                                    struct IMAP *imap)
{
  /* The imap struct is already initialized in imap_connect() */
  CURLcode result = CURLE_OK;
  const char *begin = &data->state.up.path[1]; /* skip leading slash */
  const char *ptr = begin;

  /* See how much of the URL is a valid path and decode it */
  поки(imap_is_bchar(*ptr))
    ptr++;

  якщо(ptr != begin) {
    /* Remove the trailing slash if present */
    const char *end = ptr;
    якщо(end > begin && end[-1] == '/')
      end--;

    result = Curl_urldecode(begin, end - begin, &imap->mailbox, NULL,
                            REJECT_CTRL);
    якщо(result)
      повернути result;
  }
  інакше
    imap->mailbox = NULL;

  /* There can be any number of parameters in the form ";NAME=VALUE" */
  поки(*ptr == ';') {
    char *name;
    char *value;
    size_t valuelen;

    /* Find the length of the name parameter */
    begin = ++ptr;
    поки(*ptr && *ptr != '=')
      ptr++;

    якщо(!*ptr)
      повернути CURLE_URL_MALFORMAT;

    /* Decode the name parameter */
    result = Curl_urldecode(begin, ptr - begin, &name, NULL,
                            REJECT_CTRL);
    якщо(result)
      повернути result;

    /* Find the length of the value parameter */
    begin = ++ptr;
    поки(imap_is_bchar(*ptr))
      ptr++;

    /* Decode the value parameter */
    result = Curl_urldecode(begin, ptr - begin, &value, &valuelen,
                            REJECT_CTRL);
    якщо(result) {
      curlx_free(name);
      повернути result;
    }

    DEBUGF(infof(data, "IMAP URL parameter '%s' = '%s'", name, value));

    /* Process the known hierarchical parameters (UIDVALIDITY, UID, SECTION
       and PARTIAL) stripping of the trailing slash character if it is
       present.

       Note: Unknown parameters trigger a URL_MALFORMAT error. */
    якщо(valuelen > 0 && value[valuelen - 1] == '/')
      value[valuelen - 1] = '\0';
    якщо(valuelen) {
      якщо(curl_strequal(name, "UIDVALIDITY") && !imap->uidvalidity_set) {
        curl_off_t num;
        const char *p = (const char *)value;
        якщо(!curlx_str_number(&p, &num, UINT_MAX)) {
          imap->uidvalidity = (unsigned ціле)num;
          imap->uidvalidity_set = TRUE;
        }
        curlx_free(value);
      }
      інакше якщо(curl_strequal(name, "UID") && !imap->uid) {
        imap->uid = value;
      }
      інакше якщо(curl_strequal(name, "MAILINDEX") && !imap->mindex) {
        imap->mindex = value;
      }
      інакше якщо(curl_strequal(name, "SECTION") && !imap->section) {
        imap->section = value;
      }
      інакше якщо(curl_strequal(name, "PARTIAL") && !imap->partial) {
        imap->partial = value;
      }
      інакше {
        curlx_free(name);
        curlx_free(value);
        повернути CURLE_URL_MALFORMAT;
      }
    }
    інакше
      /* blank? */
      curlx_free(value);
    curlx_free(name);
  }

  /* Does the URL contain a query parameter? Only valid when we have a mailbox
     and no UID as per RFC-5092 */
  якщо(imap->mailbox && !imap->uid && !imap->mindex) {
    /* Get the query parameter, URL decoded */
    CURLUcode uc = curl_url_get(data->state.uh, CURLUPART_QUERY, &imap->query,
                                CURLU_URLDECODE);
    якщо(uc == CURLUE_OUT_OF_MEMORY)
      повернути CURLE_OUT_OF_MEMORY;
  }

  /* Any extra stuff at the end of the URL is an error */
  якщо(*ptr)
    повернути CURLE_URL_MALFORMAT;

  повернути CURLE_OK;
}

/***********************************************************************
 *
 * imap_parse_custom_request()
 *
 * Parse the custom request.
 */
static CURLcode imap_parse_custom_request(struct Curl_easy *data,
                                          struct IMAP *imap)
{
  CURLcode result = CURLE_OK;
  const char *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  якщо(custom) {
    /* URL decode the custom request */
    result = Curl_urldecode(custom, 0, &imap->custom, NULL, REJECT_CTRL);

    /* Extract the parameters if specified */
    якщо(!result) {
      const char *params = imap->custom;

      поки(*params && *params != ' ')
        params++;

      якщо(*params) {
        imap->custom_params = curlx_strdup(params);
        imap->custom[params - imap->custom] = '\0';

        якщо(!imap->custom_params)
          result = CURLE_OUT_OF_MEMORY;
      }
    }
  }

  повернути result;
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
static CURLcode imap_connect(struct Curl_easy *data, bool *done)
{
  struct imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  CURLcode result = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  якщо(!imapc)
    повернути CURLE_FAILED_INIT;

  /* Parse the URL options */
  result = imap_parse_url_options(data->conn, imapc);
  якщо(result)
    повернути result;

  /* Start off waiting for the server greeting response */
  imap_state(data, imapc, IMAP_SERVERGREET);

  /* Start off with an response id of '*' */
  curlx_strcopy(imapc->resptag, sizeof(imapc->resptag), STRCONST("*"));

  result = imap_multi_statemach(data, done);

  повернути result;
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
static CURLcode imap_done(struct Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  struct connectdata *conn = data->conn;
  struct imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  struct IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);

  якщо(!imapc)
    повернути CURLE_FAILED_INIT;
  якщо(!imap)
    повернути CURLE_OK;

  якщо(status || premature) {
    CURL_TRC_M(data, "IMAP done, not finalizing transfer");
    connclose(conn); /* marked for closure */
    result = status;         /* use the already set error code */
  }
  інакше якщо(!data->set.connect_only &&
          ((!imap->custom && (imap->uid || imap->mindex)) ||
           (imap->custom && data->req.maxdownload > 0) ||
           data->state.upload || IS_MIME_POST(data))) {
    /* Handle responses after FETCH or APPEND transfer has finished.
       For custom commands, check if we set up a download which indicates
       a FETCH-like command with literal data. */

    якщо(!data->state.upload && !IS_MIME_POST(data))
      imap_state(data, imapc, IMAP_FETCH_FINAL);
    інакше {
      /* End the APPEND command first by sending an empty line */
      result = Curl_pp_sendf(data, &imapc->pp, "%s", "");
      якщо(!result)
        imap_state(data, imapc, IMAP_APPEND_FINAL);
    }

    /* Run the state-machine */
    якщо(!result)
      result = imap_block_statemach(data, imapc, FALSE);
  }

  imap_easy_reset(imap);
  повернути result;
}

/***********************************************************************
 *
 * imap_perform()
 *
 * This is the actual DO function for IMAP. Fetch or append a message, or do
 * other things according to the options previously setup.
 */
static CURLcode imap_perform(struct Curl_easy *data, bool *connected,
                             bool *dophase_done)
{
  /* This is IMAP and no proxy */
  CURLcode result = CURLE_OK;
  struct connectdata *conn = data->conn;
  struct imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  struct IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  bool selected = FALSE;

  DEBUGF(infof(data, "DO phase starts"));
  якщо(!imapc || !imap)
    повернути CURLE_FAILED_INIT;

  якщо(data->req.no_body) {
    /* Requested no body means no transfer */
    imap->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* Determine if the requested mailbox (with the same UIDVALIDITY if set)
     has already been selected on this connection */
  якщо(imap->mailbox && imapc->mailbox &&
     (!strcmp(imap->mailbox, imapc->mailbox) ||
      (curl_strequal(imap->mailbox, "INBOX") &&
       curl_strequal(imapc->mailbox, "INBOX"))) &&
     (!imap->uidvalidity_set || !imapc->mb_uidvalidity_set ||
      (imap->uidvalidity == imapc->mb_uidvalidity)))
    selected = TRUE;

  /* Start the first command in the DO phase */
  якщо(data->state.upload || IS_MIME_POST(data))
    /* APPEND can be executed directly */
    result = imap_perform_append(data, imapc, imap);
  інакше якщо(imap->custom && (selected || !imap->mailbox))
    /* Custom command using the same mailbox or no mailbox */
    result = imap_perform_list(data, imapc, imap);
  інакше якщо(!imap->custom && selected && (imap->uid || imap->mindex))
    /* FETCH from the same mailbox */
    result = imap_perform_fetch(data, imapc, imap);
  інакше якщо(!imap->custom && selected && imap->query)
    /* SEARCH the current mailbox */
    result = imap_perform_search(data, imapc, imap);
  інакше якщо(imap->mailbox && !selected &&
          (imap->custom || imap->uid || imap->mindex || imap->query))
    /* SELECT the mailbox */
    result = imap_perform_select(data, imapc, imap);
  інакше
    /* LIST */
    result = imap_perform_list(data, imapc, imap);

  якщо(result)
    повернути result;

  /* Run the state-machine */
  result = imap_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(conn, FIRSTSOCKET);

  якщо(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  повернути result;
}

/* Call this when the DO phase has completed */
static CURLcode imap_dophase_done(struct Curl_easy *data,
                                  struct IMAP *imap,
                                  bool connected)
{
  (void)connected;

  якщо(imap->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);

  повернути CURLE_OK;
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
static CURLcode imap_regular_transfer(struct Curl_easy *data,
                                      struct IMAP *imap,
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
  якщо(!result && *dophase_done)
    result = imap_dophase_done(data, imap, connected);

  повернути result;
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
static CURLcode imap_do(struct Curl_easy *data, bool *done)
{
  struct IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* default to false */

  якщо(!imap)
    повернути CURLE_FAILED_INIT;
  /* Parse the URL path */
  result = imap_parse_url_path(data, imap);
  якщо(result)
    повернути result;

  /* Parse the custom request */
  result = imap_parse_custom_request(data, imap);
  якщо(result)
    повернути result;

  result = imap_regular_transfer(data, imap, done);

  повернути result;
}

/***********************************************************************
 *
 * imap_disconnect()
 *
 * Disconnect from an IMAP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
static CURLcode imap_disconnect(struct Curl_easy *data,
                                struct connectdata *conn, bool dead_connection)
{
  struct imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);

  якщо(imapc &&
     /* We cannot send quit unconditionally. If this connection is stale or
        bad in any way (pingpong has pending data to send),
        sending quit and waiting around here will make the
        disconnect wait in vain and cause more problems than we need to. */
     !dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &imapc->pp) &&
     !imap_perform_logout(data, imapc))
    (void)imap_block_statemach(data, imapc, TRUE); /* ignore errors */

  повернути CURLE_OK;
}

/* Called from multi.c while DOing */
static CURLcode imap_doing(struct Curl_easy *data, bool *dophase_done)
{
  struct IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  CURLcode result;

  якщо(!imap)
    повернути CURLE_FAILED_INIT;

  result = imap_multi_statemach(data, dophase_done);
  якщо(result)
    DEBUGF(infof(data, "DO phase failed"));
  інакше якщо(*dophase_done) {
    result = imap_dophase_done(data, imap, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  повернути result;
}

static void imap_easy_dtor(const void *key, size_t klen, void *entry)
{
  struct IMAP *imap = entry;
  (void)key;
  (void)klen;
  imap_easy_reset(imap);
  curlx_free(imap);
}

static void imap_conn_dtor(const void *key, size_t klen, void *entry)
{
  struct imap_conn *imapc = entry;
  (void)key;
  (void)klen;
  Curl_pp_disconnect(&imapc->pp);
  curlx_safefree(imapc->mailbox);
  curlx_free(imapc);
}

/* SASL parameters for the imap protocol */
static const struct SASLproto saslimap = {
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

static CURLcode imap_setup_connection(struct Curl_easy *data,
                                      struct connectdata *conn)
{
  struct imap_conn *imapc;
  struct pingpong *pp;
  struct IMAP *imap;

  imapc = curlx_calloc(1, sizeof(*imapc));
  якщо(!imapc)
    повернути CURLE_OUT_OF_MEMORY;

  pp = &imapc->pp;
  PINGPONG_SETUP(pp, imap_pp_statemachine, imap_endofresp);

  /* Set the default preferred authentication type and mechanism */
  imapc->preftype = IMAP_TYPE_ANY;
  Curl_sasl_init(&imapc->sasl, data, &saslimap);

  Curl_pp_init(pp, Curl_pgrs_now(data));

  якщо(Curl_conn_meta_set(conn, CURL_META_IMAP_CONN, imapc, imap_conn_dtor))
    повернути CURLE_OUT_OF_MEMORY;

  imap = curlx_calloc(1, sizeof(struct IMAP));
  якщо(!imap ||
     Curl_meta_set(data, CURL_META_IMAP_EASY, imap, imap_easy_dtor))
    повернути CURLE_OUT_OF_MEMORY;

  повернути CURLE_OK;
}

/*
 * IMAP protocol.
 */
const struct Curl_protocol Curl_protocol_imap = {
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
