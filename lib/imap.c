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
  unsigned 정수 mb_uidvalidity; /* UIDVALIDITY parsed from select response */
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
  unsigned 정수 uidvalidity; /* UIDVALIDITY to check in select */
  BIT(uidvalidity_set);
};

#define IMAP_RESP_OK       1
#define IMAP_RESP_NOT_OK   2
#define IMAP_RESP_PREAUTH  3

struct ulbits {
  정수 bit;
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
  반환 result;
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

  만약(!str)
    반환 NULL;

  len = strlen(str);
  nclean = strcspn(str, "() {%*]\\\"");
  만약(len == nclean)
    /* nothing to escape, return a strdup */
    반환 curlx_strdup(str);

  curlx_dyn_init(&line, 2000);

  만약(!escape_only && curlx_dyn_addn(&line, "\"", 1))
    반환 NULL;

  동안(*str) {
    만약((*str == '\\' || *str == '"') &&
       curlx_dyn_addn(&line, "\\", 1))
      반환 NULL;
    만약(curlx_dyn_addn(&line, str, 1))
      반환 NULL;
    str++;
  }

  만약(!escape_only && curlx_dyn_addn(&line, "\"", 1))
    반환 NULL;

  반환 curlx_dyn_ptr(&line);
}

/*
 * Finds the start of a literal '{size}' in line, skipping over quoted strings.
 */
static const char *imap_find_literal(const char *line, size_t len)
{
  const char *end = line + len;
  bool in_quote = FALSE;

  동안(line < end) {
    만약(in_quote) {
      만약(*line == '\\' && (line + 1) < end) {
        line += 2;
        continue;
      }
      만약(*line == '"')
        in_quote = FALSE;
    }
    아니면 {
      만약(*line == '"')
        in_quote = TRUE;
      아니면 만약(*line == '{')
        반환 line;
    }
    line++;
  }
  반환 NULL;
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
  만약(line < end && ISDIGIT(*line)) {
    /* Skip the number */
    do
      line++;
    동안(line < end && ISDIGIT(*line));

    /* Do we have the space character? */
    만약(line == end || *line != ' ')
      반환 FALSE;

    line++;
  }

  /* Does the command name match and is it followed by a space character or at
     the end of line? */
  만약(line + cmd_len <= end && curl_strnequal(line, cmd, cmd_len) &&
     (line[cmd_len] == ' ' || line + cmd_len + 2 == end))
    반환 TRUE;

  반환 FALSE;
}

/***********************************************************************
 *
 * imap_endofresp()
 *
 * Checks whether the given string is a valid tagged, untagged or continuation
 * response which can be processed by the response handler.
 */
static bool imap_endofresp(struct Curl_easy *data, struct connectdata *conn,
                           const char *line, size_t len, 정수 *resp)
{
  struct imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  struct IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  const char *id;
  size_t id_len;

  DEBUGASSERT(imapc);
  DEBUGASSERT(imap);
  만약(!imapc || !imap)
    반환 FALSE;

  /* Do we have a tagged command response? */
  id = imapc->resptag;
  id_len = strlen(id);
  만약(len >= id_len + 1 && !memcmp(id, line, id_len) && line[id_len] == ' ') {
    line += id_len + 1;
    len -= id_len + 1;

    만약(len >= 2 && !memcmp(line, "OK", 2))
      *resp = IMAP_RESP_OK;
    아니면 만약(len >= 7 && !memcmp(line, "PREAUTH", 7))
      *resp = IMAP_RESP_PREAUTH;
    아니면
      *resp = IMAP_RESP_NOT_OK;

    반환 TRUE;
  }

  /* Do we have an untagged command response? */
  만약(len >= 2 && !memcmp("* ", line, 2)) {
    switch(imapc->state) {
    /* States which are interested in untagged responses */
    case IMAP_CAPABILITY:
      만약(!imap_matchresp(line, len, "CAPABILITY"))
        반환 FALSE;
      break;

    case IMAP_LIST:
      만약((!imap->custom && !imap_matchresp(line, len, "LIST")) ||
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
        반환 FALSE;
      break;

    case IMAP_SELECT:
      /* SELECT is special in that its untagged responses do not have a
         common prefix so accept anything! */
      break;

    case IMAP_FETCH:
      만약(!imap_matchresp(line, len, "FETCH"))
        반환 FALSE;
      break;

    case IMAP_SEARCH:
      만약(!imap_matchresp(line, len, "SEARCH"))
        반환 FALSE;
      break;

    /* Ignore other untagged responses */
    default:
      반환 FALSE;
    }

    *resp = '*';
    반환 TRUE;
  }

  /* Do we have a continuation response? This should be a + symbol followed by
     a space and optionally some text as per RFC-3501 for the AUTHENTICATE and
     APPEND commands and as outlined in Section 4. Examples of RFC-4959 but
     some email servers ignore this and only send a single + instead. */
  만약(!imap->custom && ((len == 3 && line[0] == '+') ||
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

    반환 TRUE;
  }

  반환 FALSE; /* Nothing for us */
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

  만약(!imapc)
    반환 CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&imapc->pp.recvbuf);
  len = imapc->pp.nfinal;
  만약(len > 2) {
    /* Find the start of the message */
    len -= 2;
    위해(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    동안(len--)
      만약(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        break;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, NULL);
  }
  아니면
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, NULL);

  반환 CURLE_OK;
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
#만약 defined(DEBUGBUILD) && defined(CURLVERBOSE)
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

  만약(imapc->state != newstate)
    infof(data, "IMAP %p state change from %s to %s",
          (void *)imapc, names[imapc->state], names[newstate]);
#아니면
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

  만약(!result)
    imap_state(data, imapc, IMAP_CAPABILITY);

  반환 result;
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

  만약(!result)
    imap_state(data, imapc, IMAP_STARTTLS);

  반환 result;
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

  만약(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    만약(result)
      goto out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_imaps;
  }

  DEBUGASSERT(!imapc->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "imap_perform_upgrade_tls, connect -> %d, %d",
               (정수)result, ssldone));
  만약(!result && ssldone) {
    imapc->ssldone = ssldone;
    /* perform CAPA now, changes imapc->state out of IMAP_UPGRADETLS */
    result = imap_perform_capability(data, imapc);
  }
out:
  반환 result;
#아니면
  (void)data;
  (void)imapc;
  (void)conn;
  반환 CURLE_NOT_BUILT_IN;
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
  만약(!conn->creds) {
    imap_state(data, imapc, IMAP_STOP);

    반환 result;
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

  만약(!result)
    imap_state(data, imapc, IMAP_LOGIN);

  반환 result;
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

  만약(!imapc)
    반환 CURLE_FAILED_INIT;
  만약(ir) {
    /* Send the AUTHENTICATE command with the initial response */
    result = imap_sendf(data, imapc, "AUTHENTICATE %s %s",
                        mech, *ir ? ir : "=");
  }
  아니면 {
    /* Send the AUTHENTICATE command */
    result = imap_sendf(data, imapc, "AUTHENTICATE %s", mech);
  }

  반환 result;
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
  만약(!imapc)
    반환 CURLE_FAILED_INIT;
  반환 Curl_pp_sendf(data, &imapc->pp, "%s", Curl_bufref_ptr(resp));
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
  만약(!imapc)
    반환 CURLE_FAILED_INIT;
  반환 Curl_pp_sendf(data, &imapc->pp, "*");
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
  만약(imapc->preauth ||
     !Curl_sasl_can_authenticate(&imapc->sasl, data)) {
    imap_state(data, imapc, IMAP_STOP);
    반환 result;
  }

  /* Calculate the SASL login details */
  result = Curl_sasl_start(&imapc->sasl, data, (bool)imapc->ir_supported,
                           &progress);
  만약(!result) {
    만약(progress == SASL_INPROGRESS)
      imap_state(data, imapc, IMAP_AUTHENTICATE);
    아니면 만약(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
      /* Perform clear text authentication */
      result = imap_perform_login(data, imapc, data->conn);
    아니면
      result = Curl_sasl_is_blocked(&imapc->sasl, data);
  }

  반환 result;
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

  만약(imap->custom)
    /* Send the custom request */
    result = imap_sendf(data, imapc, "%s%s", imap->custom,
                        imap->custom_params ? imap->custom_params : "");
  아니면 {
    /* Make sure the mailbox is in the correct atom format if necessary */
    char *mailbox = imap->mailbox ? imap_atom(imap->mailbox, TRUE)
                                  : curlx_strdup("");
    만약(!mailbox)
      반환 CURLE_OUT_OF_MEMORY;

    /* Send the LIST command */
    result = imap_sendf(data, imapc, "LIST \"%s\" *", mailbox);

    curlx_free(mailbox);
  }

  만약(!result)
    imap_state(data, imapc, IMAP_LIST);

  반환 result;
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
  만약(!imap->mailbox) {
    failf(data, "Cannot SELECT without a mailbox.");
    반환 CURLE_URL_MALFORMAT;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  만약(!mailbox)
    반환 CURLE_OUT_OF_MEMORY;

  /* Send the SELECT command */
  result = imap_sendf(data, imapc, "SELECT %s", mailbox);

  curlx_free(mailbox);

  만약(!result)
    imap_state(data, imapc, IMAP_SELECT);

  반환 result;
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
  만약(imap->uid) {

    /* Send the FETCH command */
    만약(imap->partial)
      result = imap_sendf(data, imapc, "UID FETCH %s BODY[%s]<%s>",
                          imap->uid, imap->section ? imap->section : "",
                          imap->partial);
    아니면
      result = imap_sendf(data, imapc, "UID FETCH %s BODY[%s]",
                          imap->uid, imap->section ? imap->section : "");
  }
  아니면 만약(imap->mindex) {
    /* Send the FETCH command */
    만약(imap->partial)
      result = imap_sendf(data, imapc, "FETCH %s BODY[%s]<%s>",
                          imap->mindex, imap->section ? imap->section : "",
                          imap->partial);
    아니면
      result = imap_sendf(data, imapc, "FETCH %s BODY[%s]",
                          imap->mindex, imap->section ? imap->section : "");
  }
  아니면 {
    failf(data, "Cannot FETCH without a UID.");
    반환 CURLE_URL_MALFORMAT;
  }
  만약(!result)
    imap_state(data, imapc, IMAP_FETCH);

  반환 result;
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
  만약(!imap->mailbox) {
    failf(data, "Cannot APPEND without a mailbox.");
    반환 CURLE_URL_MALFORMAT;
  }

#ifndef CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  만약(IS_MIME_POST(data)) {
    curl_mimepart *postp = data->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(unsigned 정수)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, data->set.headers, 0);
    result = Curl_mime_prepare_headers(data, postp, NULL,
                                       NULL, MIMESTRATEGY_MAIL);

    만약(!result && !Curl_checkheaders(data, STRCONST("Mime-Version")))
      result = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    만약(!result)
      result = Curl_creader_set_mime(data, postp);
    만약(result)
      반환 result;
  }
  아니면
#endif
  {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    만약(result)
      반환 result;
  }

  /* Check we know the size of the upload. This takes all readers
   * into account. Especially crlf conversions which make the size
   * unpredictable, e.g. -1. */
  data->state.infilesize = Curl_creader_total_length(data);
  만약(data->state.infilesize < 0) {
    만약(data->set.crlf)
      failf(data, "Cannot APPEND with CRLF conversion making size unknown");
    아니면
      failf(data, "Cannot APPEND with unknown input file size");
    반환 CURLE_UPLOAD_FAILED;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  만약(!mailbox)
    반환 CURLE_OUT_OF_MEMORY;

  /* Generate flags string and send the APPEND command */
  curlx_dyn_init(&flags, 100);
  만약(data->set.upload_flags) {
    정수 i;
    struct ulbits ulflag[] = {
      { CURLULFLAG_ANSWERED, "Answered" },
      { CURLULFLAG_DELETED, "Deleted" },
      { CURLULFLAG_DRAFT, "Draft" },
      { CURLULFLAG_FLAGGED, "Flagged" },
      { CURLULFLAG_SEEN, "Seen" },
      { 0, NULL }
    };

    result = CURLE_OUT_OF_MEMORY;
    만약(curlx_dyn_add(&flags, " (")) {
      goto cleanup;
    }

    위해(i = 0; ulflag[i].bit; i++) {
      만약(data->set.upload_flags & ulflag[i].bit &&
         ((curlx_dyn_len(&flags) > 2 && curlx_dyn_add(&flags, " ")) ||
          curlx_dyn_add(&flags, "\\") ||
          curlx_dyn_add(&flags, ulflag[i].flag)))
        goto cleanup;
    }

    만약(curlx_dyn_add(&flags, ")"))
      goto cleanup;
  }
  아니면 만약(curlx_dyn_add(&flags, ""))
    goto cleanup;

  result = imap_sendf(data, imapc, "APPEND %s%s {%" FMT_OFF_T "}",
                      mailbox, curlx_dyn_ptr(&flags), data->state.infilesize);

cleanup:
  curlx_dyn_free(&flags);
  curlx_free(mailbox);

  만약(!result)
    imap_state(data, imapc, IMAP_APPEND);

  반환 result;
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
  만약(!imap->query) {
    failf(data, "Cannot SEARCH without a query string.");
    반환 CURLE_URL_MALFORMAT;
  }

  /* Send the SEARCH command */
  result = imap_sendf(data, imapc, "SEARCH %s", imap->query);

  만약(!result)
    imap_state(data, imapc, IMAP_SEARCH);

  반환 result;
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

  만약(!result)
    imap_state(data, imapc, IMAP_LOGOUT);

  반환 result;
}

/* For the initial server greeting */
static CURLcode imap_state_servergreet_resp(struct Curl_easy *data,
                                            struct imap_conn *imapc,
                                            정수 imapcode,
                                            imapstate instate)
{
  (void)instate;

  만약(imapcode == IMAP_RESP_PREAUTH) {
    /* PREAUTH */
    imapc->preauth = TRUE;
    infof(data, "PREAUTH connection, already authenticated");
  }
  아니면 만약(imapcode != IMAP_RESP_OK) {
    failf(data, "Got unexpected imap-server response");
    반환 CURLE_WEIRD_SERVER_REPLY;
  }

  반환 imap_perform_capability(data, imapc);
}

/* For CAPABILITY responses */
static CURLcode imap_state_capability_resp(struct Curl_easy *data,
                                           struct imap_conn *imapc,
                                           정수 imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  const char *line = curlx_dyn_ptr(&imapc->pp.recvbuf);

  (void)instate;

  /* Do we have an untagged response? */
  만약(imapcode == '*') {
    line += 2;

    /* Loop through the data line */
    위해(;;) {
      size_t wordlen;
      동안(*line && (ISBLANK(*line) || ISNEWLINE(*line)))
        line++;

      만약(!*line)
        break;

      /* Extract the word */
      위해(wordlen = 0; line[wordlen] && !ISBLANK(line[wordlen]) &&
                       !ISNEWLINE(line[wordlen]);)
        wordlen++;

      /* Does the server support the STARTTLS capability? */
      만약(wordlen == 8 && curl_strnequal(line, "STARTTLS", 8))
        imapc->tls_supported = TRUE;

      /* Has the server explicitly disabled clear text authentication? */
      아니면 만약(wordlen == 13 && curl_strnequal(line, "LOGINDISABLED", 13))
        imapc->login_disabled = TRUE;

      /* Does the server support the SASL-IR capability? */
      아니면 만약(wordlen == 7 && curl_strnequal(line, "SASL-IR", 7))
        imapc->ir_supported = TRUE;

      /* Do we have a SASL based authentication mechanism? */
      아니면 만약(wordlen > 5 && curl_strnequal(line, "AUTH=", 5)) {
        size_t llen;
        unsigned short mechbit;

        line += 5;
        wordlen -= 5;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(line, wordlen, &llen);
        만약(mechbit && llen == wordlen)
          imapc->sasl.authmechs |= mechbit;
      }

      line += wordlen;
    }
  }
  아니면 만약(data->set.use_ssl && !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
    /* PREAUTH is not compatible with STARTTLS. */
    만약(imapcode == IMAP_RESP_OK && imapc->tls_supported && !imapc->preauth) {
      /* Switch to TLS connection now */
      result = imap_perform_starttls(data, imapc);
    }
    아니면 만약(data->set.use_ssl <= CURLUSESSL_TRY)
      result = imap_perform_authentication(data, imapc);
    아니면 {
      failf(data, "STARTTLS not available.");
      result = CURLE_USE_SSL_FAILED;
    }
  }
  아니면
    result = imap_perform_authentication(data, imapc);

  반환 result;
}

/* For STARTTLS responses */
static CURLcode imap_state_starttls_resp(struct Curl_easy *data,
                                         struct imap_conn *imapc,
                                         정수 imapcode,
                                         imapstate instate)
{
  CURLcode result = CURLE_OK;

  (void)instate;

  /* Pipelining in response is forbidden. */
  만약(imapc->pp.overflow)
    반환 CURLE_WEIRD_SERVER_REPLY;

  만약(imapcode != IMAP_RESP_OK) {
    만약(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied");
      result = CURLE_USE_SSL_FAILED;
    }
    아니면
      result = imap_perform_authentication(data, imapc);
  }
  아니면
    imap_state(data, imapc, IMAP_UPGRADETLS);

  반환 result;
}

/* For SASL authentication responses */
static CURLcode imap_state_auth_resp(struct Curl_easy *data,
                                     struct imap_conn *imapc,
                                     정수 imapcode,
                                     imapstate instate)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  (void)instate;

  result = Curl_sasl_continue(&imapc->sasl, data, imapcode, &progress);
  만약(!result)
    switch(progress) {
    case SASL_DONE:
      imap_state(data, imapc, IMAP_STOP);  /* Authenticated */
      break;
    case SASL_IDLE:            /* No mechanism left after cancellation */
      만약(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
        /* Perform clear text authentication */
        result = imap_perform_login(data, imapc, data->conn);
      아니면 {
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
      }
      break;
    default:
      break;
    }

  반환 result;
}

/* For LOGIN responses */
static CURLcode imap_state_login_resp(struct Curl_easy *data,
                                      struct imap_conn *imapc,
                                      정수 imapcode,
                                      imapstate instate)
{
  CURLcode result = CURLE_OK;
  (void)instate;

  만약(imapcode != IMAP_RESP_OK) {
    failf(data, "Access denied. %c", imapcode);
    result = CURLE_LOGIN_DENIED;
  }
  아니면
    /* End of connect phase */
    imap_state(data, imapc, IMAP_STOP);

  반환 result;
}

/* Detect IMAP listings vs. downloading a single email */
static bool is_custom_fetch_listing_match(const char *params)
{
  /* match " 1:* (FLAGS ..." or " 1,2,3 (FLAGS ..." */
  만약(*params++ != ' ')
    반환 FALSE;

  동안(ISDIGIT(*params)) {
    params++;
    만약(*params == 0)
      반환 FALSE;
  }
  만약(*params == ':')
    반환 TRUE;
  만약(*params == ',')
    반환 TRUE;
  반환 FALSE;
}

static bool is_custom_fetch_listing(struct IMAP *imap)
{
  /* filter out "UID FETCH 1:* (FLAGS ..." queries to list emails */
  만약(!imap->custom)
    반환 FALSE;
  아니면 만약(curl_strequal(imap->custom, "FETCH") && imap->custom_params) {
    const char *p = imap->custom_params;
    반환 is_custom_fetch_listing_match(p);
  }
  아니면 만약(curl_strequal(imap->custom, "UID") && imap->custom_params &&
          curl_strnequal(imap->custom_params, " FETCH ", 7)) {
    const char *p = imap->custom_params + 6;
    반환 is_custom_fetch_listing_match(p);
  }
  반환 FALSE;
}

/* For LIST and SEARCH responses */
static CURLcode imap_state_listsearch_resp(struct Curl_easy *data,
                                           struct imap_conn *imapc,
                                           정수 imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  const char *line = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  struct IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);

  DEBUGASSERT(imap);
  만약(!imap)
    반환 CURLE_FAILED_INIT;
  (void)instate;

  만약(imapcode == '*' && is_custom_fetch_listing(imap)) {
    /* custom FETCH or UID FETCH for listing is not handled here */
  }
  아니면 만약(imapcode == '*') {
    /* Check if this response contains a literal (e.g. FETCH responses with
       body data). Literal syntax is {size}\r\n */
    const char *cr = memchr(line, '\r', len);
    size_t line_len = cr ? (size_t)(cr - line) : len;
    const char *ptr = imap_find_literal(line, line_len);
    만약(ptr) {
      curl_off_t size = 0;
      bool parsed = FALSE;
      ptr++;
      만약(!curlx_str_number(&ptr, &size, CURL_OFF_T_MAX) &&
         !curlx_str_single(&ptr, '}'))
        parsed = TRUE;

      만약(parsed) {
        struct pingpong *pp = &imapc->pp;
        size_t buffer_len = curlx_dyn_len(&pp->recvbuf);
        size_t after_header = buffer_len - pp->nfinal;

        /* This is a literal response, setup to receive the body data */
        infof(data, "Found %" FMT_OFF_T " bytes to download", size);

        /* First write the header line */
        result = Curl_client_write(data, CLIENTWRITE_BODY, line, len);
        만약(result)
          반환 result;

        /* Handle data already in buffer after the header line */
        만약(after_header > 0) {
          /* There is already data in the buffer that is part of the literal
             body or subsequent responses */
          size_t chunk = after_header;

          /* Keep only the data after the header line */
          curlx_dyn_tail(&pp->recvbuf, chunk);
          pp->nfinal = 0; /* done */

          /* Limit chunk to the literal size */
          만약((curl_off_t)chunk > size)
            chunk = (size_t)size;

          만약(chunk) {
            /* Write the literal body data */
            result = Curl_client_write(data, CLIENTWRITE_BODY,
                                       curlx_dyn_ptr(&pp->recvbuf), chunk);
            만약(result)
              반환 result;
          }

          /* Handle remaining data in buffer (either more literal data or
             subsequent responses) */
          만약(after_header > chunk) {
            /* Keep the data after the literal body */
            pp->overflow = after_header - chunk;
            curlx_dyn_tail(&pp->recvbuf, pp->overflow);
          }
          아니면 {
            pp->overflow = 0;
            curlx_dyn_reset(&pp->recvbuf);
          }
        }
        아니면 {
          /* No data in buffer yet, reset overflow */
          pp->overflow = 0;
        }

        만약((CURL_OFF_T_MAX - size) < (curl_off_t)len)
          /* unlikely to actually be a transfer this big, but avoid integer
             overflow */
          size = CURL_OFF_T_MAX;
        아니면
          size += len;

        /* Progress size includes both header line and literal body */
        Curl_pgrsSetDownloadSize(data, size);

        만약(data->req.bytecount == size)
          /* All data already transferred (header + literal body) */
          Curl_xfer_setup_nop(data);
        아니면 {
          /* Setup to receive the literal body data.
             maxdownload and transfer size include both header line and
             literal body */
          data->req.maxdownload = size;
          Curl_xfer_setup_recv(data, FIRSTSOCKET, size);
        }
        /* End of DO phase */
        imap_state(data, imapc, IMAP_STOP);
      }
      아니면 {
        /* Failed to parse literal, write the line */
        result = Curl_client_write(data, CLIENTWRITE_BODY, line, len);
      }
    }
    아니면 {
      /* No literal, write the line as-is */
      result = Curl_client_write(data, CLIENTWRITE_BODY, line, len);
    }
  }
  아니면 만약(imapcode != IMAP_RESP_OK)
    result = CURLE_QUOTE_ERROR;
  아니면
    /* End of DO phase */
    imap_state(data, imapc, IMAP_STOP);

  반환 result;
}

/* For SELECT responses */
static CURLcode imap_state_select_resp(struct Curl_easy *data,
                                       struct imap_conn *imapc,
                                       struct IMAP *imap,
                                       정수 imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  (void)instate;

  만약(imapcode == '*') {
    /* See if this is an UIDVALIDITY response */
    const char *line = curlx_dyn_ptr(&imapc->pp.recvbuf);
    size_t len = curlx_dyn_len(&imapc->pp.recvbuf);
    만약((len >= 18) && checkprefix("OK [UIDVALIDITY ", &line[2])) {
      curl_off_t value;
      const char *p = &line[2] + CURL_CSTRLEN("OK [UIDVALIDITY ");
      만약(!curlx_str_number(&p, &value, UINT_MAX)) {
        imapc->mb_uidvalidity = (unsigned 정수)value;
        imapc->mb_uidvalidity_set = TRUE;
      }
    }
  }
  아니면 만약(imapcode == IMAP_RESP_OK) {
    /* Check if the UIDVALIDITY has been specified and matches */
    만약(imap->uidvalidity_set && imapc->mb_uidvalidity_set &&
       (imap->uidvalidity != imapc->mb_uidvalidity)) {
      failf(data, "Mailbox UIDVALIDITY has changed");
      result = CURLE_REMOTE_FILE_NOT_FOUND;
    }
    아니면 {
      /* Note the currently opened mailbox on this connection */
      DEBUGASSERT(!imapc->mailbox);
      imapc->mailbox = curlx_strdup(imap->mailbox);
      만약(!imapc->mailbox)
        반환 CURLE_OUT_OF_MEMORY;

      만약(imap->custom)
        result = imap_perform_list(data, imapc, imap);
      아니면 만약(imap->query)
        result = imap_perform_search(data, imapc, imap);
      아니면
        result = imap_perform_fetch(data, imapc, imap);
    }
  }
  아니면 {
    failf(data, "Select failed");
    result = CURLE_LOGIN_DENIED;
  }

  반환 result;
}

/* For the (first line of the) FETCH responses */
static CURLcode imap_state_fetch_resp(struct Curl_easy *data,
                                      struct imap_conn *imapc,
                                      정수 imapcode,
                                      imapstate instate)
{
  CURLcode result = CURLE_OK;
  struct pingpong *pp = &imapc->pp;
  const char *ptr = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  bool parsed = FALSE;
  curl_off_t size = 0;

  (void)instate;

  만약(imapcode != '*') {
    Curl_pgrsSetDownloadSize(data, -1);
    imap_state(data, imapc, IMAP_STOP);
    반환 CURLE_REMOTE_FILE_NOT_FOUND;
  }

  /* Something like this is received "* 1 FETCH (BODY[TEXT] {2021}\r" so parse
     the continuation data contained within the curly brackets */
  ptr = imap_find_literal(ptr, len);
  만약(ptr) {
    ptr++;
    만약(!curlx_str_number(&ptr, &size, CURL_OFF_T_MAX) &&
       !curlx_str_single(&ptr, '}'))
      parsed = TRUE;
  }

  만약(parsed) {
    infof(data, "Found %" FMT_OFF_T " bytes to download", size);
    Curl_pgrsSetDownloadSize(data, size);

    만약(pp->overflow) {
      /* At this point there is a data in the receive buffer that is body
         content, send it as body and then skip it. Do note that there may
         even be additional "headers" after the body. */
      size_t chunk = pp->overflow;

      /* keep only the overflow */
      curlx_dyn_tail(&pp->recvbuf, chunk);
      pp->nfinal = 0; /* done */

      만약((curl_off_t)chunk > size)
        /* The conversion from curl_off_t to size_t is always fine here */
        chunk = (size_t)size;

      만약(!chunk) {
        /* no size, we are done with the data */
        imap_state(data, imapc, IMAP_STOP);
        반환 CURLE_OK;
      }
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 curlx_dyn_ptr(&pp->recvbuf), chunk);
      만약(result)
        반환 result;

      infof(data, "Written %zu bytes, %" FMT_OFF_T
            " bytes are left for transfer", chunk, (curl_off_t)(size - chunk));

      /* Have we used the entire overflow or part of it?*/
      만약(pp->overflow > chunk) {
        /* remember the remaining trailing overflow data */
        pp->overflow -= chunk;
        curlx_dyn_tail(&pp->recvbuf, pp->overflow);
      }
      아니면 {
        pp->overflow = 0; /* handled */
        /* Free the cache */
        curlx_dyn_reset(&pp->recvbuf);
      }
    }

    만약(data->req.bytecount == size)
      /* The entire data is already transferred! */
      Curl_xfer_setup_nop(data);
    아니면 {
      /* IMAP download */
      data->req.maxdownload = size;
      Curl_xfer_setup_recv(data, FIRSTSOCKET, size);
    }
  }
  아니면 {
    /* We do not know how to parse this line */
    failf(data, "Failed to parse FETCH response.");
    result = CURLE_WEIRD_SERVER_REPLY;
  }

  /* End of DO phase */
  imap_state(data, imapc, IMAP_STOP);

  반환 result;
}

/* For final FETCH responses performed after the download */
static CURLcode imap_state_fetch_final_resp(struct Curl_easy *data,
                                            struct imap_conn *imapc,
                                            정수 imapcode,
                                            imapstate instate)
{
  CURLcode result = CURLE_OK;

  (void)instate;

  만약(imapcode != IMAP_RESP_OK)
    result = CURLE_WEIRD_SERVER_REPLY;
  아니면
    /* End of DONE phase */
    imap_state(data, imapc, IMAP_STOP);

  반환 result;
}

/* For APPEND responses */
static CURLcode imap_state_append_resp(struct Curl_easy *data,
                                       struct imap_conn *imapc,
                                       정수 imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  (void)instate;

  만약(imapcode != '+') {
    result = CURLE_UPLOAD_FAILED;
  }
  아니면 {
    /* Set the progress upload size */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* IMAP upload */
    Curl_xfer_setup_send(data, FIRSTSOCKET);

    /* End of DO phase */
    imap_state(data, imapc, IMAP_STOP);
  }

  반환 result;
}

/* For final APPEND responses performed after the upload */
static CURLcode imap_state_append_final_resp(struct Curl_easy *data,
                                             struct imap_conn *imapc,
                                             정수 imapcode,
                                             imapstate instate)
{
  CURLcode result = CURLE_OK;

  (void)instate;

  만약(imapcode != IMAP_RESP_OK)
    result = CURLE_UPLOAD_FAILED;
  아니면
    /* End of DONE phase */
    imap_state(data, imapc, IMAP_STOP);

  반환 result;
}

static CURLcode imap_pp_statemachine(struct Curl_easy *data,
                                     struct connectdata *conn)
{
  CURLcode result = CURLE_OK;
  정수 imapcode;
  struct imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  struct IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  struct pingpong *pp;
  size_t nread = 0;

  만약(!imapc || !imap)
    반환 CURLE_FAILED_INIT;
  pp = &imapc->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not IMAP */
upgrade_tls:
  만약(imapc->state == IMAP_UPGRADETLS) {
    result = imap_perform_upgrade_tls(data, imapc, conn);
    만약(result || (imapc->state == IMAP_UPGRADETLS))
      반환 result;
  }

  /* Flush any data that needs to be sent */
  만약(pp->sendleft)
    반환 Curl_pp_flushsend(data, pp);

  do {
    /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, pp, &imapcode, &nread);
    만약(result)
      반환 result;

    /* Was there an error parsing the response line? */
    만약(imapcode == -1)
      반환 CURLE_WEIRD_SERVER_REPLY;

    만약(!imapcode)
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
      만약(!result && (imapc->state == IMAP_UPGRADETLS))
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
  } 동안(!result && imapc->state != IMAP_STOP && Curl_pp_moredata(pp));

  반환 result;
}

/* Called repeatedly until done from multi.c */
static CURLcode imap_multi_statemach(struct Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  struct imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  *done = FALSE;
  만약(!imapc)
    반환 CURLE_FAILED_INIT;
  result = Curl_pp_statemach(data, &imapc->pp, FALSE, FALSE);
  *done = (imapc->state == IMAP_STOP);

  반환 result;
}

static CURLcode imap_block_statemach(struct Curl_easy *data,
                                     struct imap_conn *imapc,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;

  동안(imapc->state != IMAP_STOP && !result)
    result = Curl_pp_statemach(data, &imapc->pp, TRUE, disconnecting);

  반환 result;
}

/* For the IMAP "protocol connect" and "doing" phases only */
static CURLcode imap_pollset(struct Curl_easy *data,
                             struct easy_pollset *ps)
{
  struct imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  반환 imapc ? Curl_pp_pollset(data, &imapc->pp, ps) : CURLE_OK;
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
  반환 ch && (ISALNUM(ch) || strchr(":@/&=-._~!$\'()*+,%", ch));
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

  동안(!result && ptr && *ptr) {
    const char *key = ptr;
    const char *value;

    동안(*ptr && *ptr != '=')
      ptr++;

    value = ptr + 1;

    동안(*ptr && *ptr != ';')
      ptr++;

    만약(curl_strnequal(key, "AUTH=+LOGIN", 11)) {
      /* User prefers plaintext LOGIN over any SASL, including SASL LOGIN */
      prefer_login = TRUE;
      imapc->sasl.prefmech = SASL_AUTH_NONE;
    }
    아니면 만약(curl_strnequal(key, "AUTH=", 5)) {
      prefer_login = FALSE;
      result = Curl_sasl_parse_url_auth_option(&imapc->sasl,
                                               value, ptr - value);
    }
    아니면 {
      prefer_login = FALSE;
      result = CURLE_URL_MALFORMAT;
    }

    만약(*ptr == ';')
      ptr++;
  }

  만약(prefer_login)
    imapc->preftype = IMAP_TYPE_CLEARTEXT;
  아니면 {
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

  반환 result;
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
  동안(imap_is_bchar(*ptr))
    ptr++;

  만약(ptr != begin) {
    /* Remove the trailing slash if present */
    const char *end = ptr;
    만약(end > begin && end[-1] == '/')
      end--;

    result = Curl_urldecode(begin, end - begin, &imap->mailbox, NULL,
                            REJECT_CTRL);
    만약(result)
      반환 result;
  }
  아니면
    imap->mailbox = NULL;

  /* There can be any number of parameters in the form ";NAME=VALUE" */
  동안(*ptr == ';') {
    char *name;
    char *value;
    size_t valuelen;

    /* Find the length of the name parameter */
    begin = ++ptr;
    동안(*ptr && *ptr != '=')
      ptr++;

    만약(!*ptr)
      반환 CURLE_URL_MALFORMAT;

    /* Decode the name parameter */
    result = Curl_urldecode(begin, ptr - begin, &name, NULL,
                            REJECT_CTRL);
    만약(result)
      반환 result;

    /* Find the length of the value parameter */
    begin = ++ptr;
    동안(imap_is_bchar(*ptr))
      ptr++;

    /* Decode the value parameter */
    result = Curl_urldecode(begin, ptr - begin, &value, &valuelen,
                            REJECT_CTRL);
    만약(result) {
      curlx_free(name);
      반환 result;
    }

    DEBUGF(infof(data, "IMAP URL parameter '%s' = '%s'", name, value));

    /* Process the known hierarchical parameters (UIDVALIDITY, UID, SECTION
       and PARTIAL) stripping of the trailing slash character if it is
       present.

       Note: Unknown parameters trigger a URL_MALFORMAT error. */
    만약(valuelen > 0 && value[valuelen - 1] == '/')
      value[valuelen - 1] = '\0';
    만약(valuelen) {
      만약(curl_strequal(name, "UIDVALIDITY") && !imap->uidvalidity_set) {
        curl_off_t num;
        const char *p = (const char *)value;
        만약(!curlx_str_number(&p, &num, UINT_MAX)) {
          imap->uidvalidity = (unsigned 정수)num;
          imap->uidvalidity_set = TRUE;
        }
        curlx_free(value);
      }
      아니면 만약(curl_strequal(name, "UID") && !imap->uid) {
        imap->uid = value;
      }
      아니면 만약(curl_strequal(name, "MAILINDEX") && !imap->mindex) {
        imap->mindex = value;
      }
      아니면 만약(curl_strequal(name, "SECTION") && !imap->section) {
        imap->section = value;
      }
      아니면 만약(curl_strequal(name, "PARTIAL") && !imap->partial) {
        imap->partial = value;
      }
      아니면 {
        curlx_free(name);
        curlx_free(value);
        반환 CURLE_URL_MALFORMAT;
      }
    }
    아니면
      /* blank? */
      curlx_free(value);
    curlx_free(name);
  }

  /* Does the URL contain a query parameter? Only valid when we have a mailbox
     and no UID as per RFC-5092 */
  만약(imap->mailbox && !imap->uid && !imap->mindex) {
    /* Get the query parameter, URL decoded */
    CURLUcode uc = curl_url_get(data->state.uh, CURLUPART_QUERY, &imap->query,
                                CURLU_URLDECODE);
    만약(uc == CURLUE_OUT_OF_MEMORY)
      반환 CURLE_OUT_OF_MEMORY;
  }

  /* Any extra stuff at the end of the URL is an error */
  만약(*ptr)
    반환 CURLE_URL_MALFORMAT;

  반환 CURLE_OK;
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

  만약(custom) {
    /* URL decode the custom request */
    result = Curl_urldecode(custom, 0, &imap->custom, NULL, REJECT_CTRL);

    /* Extract the parameters if specified */
    만약(!result) {
      const char *params = imap->custom;

      동안(*params && *params != ' ')
        params++;

      만약(*params) {
        imap->custom_params = curlx_strdup(params);
        imap->custom[params - imap->custom] = '\0';

        만약(!imap->custom_params)
          result = CURLE_OUT_OF_MEMORY;
      }
    }
  }

  반환 result;
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
  만약(!imapc)
    반환 CURLE_FAILED_INIT;

  /* Parse the URL options */
  result = imap_parse_url_options(data->conn, imapc);
  만약(result)
    반환 result;

  /* Start off waiting for the server greeting response */
  imap_state(data, imapc, IMAP_SERVERGREET);

  /* Start off with an response id of '*' */
  curlx_strcopy(imapc->resptag, sizeof(imapc->resptag), STRCONST("*"));

  result = imap_multi_statemach(data, done);

  반환 result;
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

  만약(!imapc)
    반환 CURLE_FAILED_INIT;
  만약(!imap)
    반환 CURLE_OK;

  만약(status || premature) {
    CURL_TRC_M(data, "IMAP done, not finalizing transfer");
    connclose(conn); /* marked for closure */
    result = status;         /* use the already set error code */
  }
  아니면 만약(!data->set.connect_only &&
          ((!imap->custom && (imap->uid || imap->mindex)) ||
           (imap->custom && data->req.maxdownload > 0) ||
           data->state.upload || IS_MIME_POST(data))) {
    /* Handle responses after FETCH or APPEND transfer has finished.
       For custom commands, check if we set up a download which indicates
       a FETCH-like command with literal data. */

    만약(!data->state.upload && !IS_MIME_POST(data))
      imap_state(data, imapc, IMAP_FETCH_FINAL);
    아니면 {
      /* End the APPEND command first by sending an empty line */
      result = Curl_pp_sendf(data, &imapc->pp, "%s", "");
      만약(!result)
        imap_state(data, imapc, IMAP_APPEND_FINAL);
    }

    /* Run the state-machine */
    만약(!result)
      result = imap_block_statemach(data, imapc, FALSE);
  }

  imap_easy_reset(imap);
  반환 result;
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
  만약(!imapc || !imap)
    반환 CURLE_FAILED_INIT;

  만약(data->req.no_body) {
    /* Requested no body means no transfer */
    imap->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* Determine if the requested mailbox (with the same UIDVALIDITY if set)
     has already been selected on this connection */
  만약(imap->mailbox && imapc->mailbox &&
     (!strcmp(imap->mailbox, imapc->mailbox) ||
      (curl_strequal(imap->mailbox, "INBOX") &&
       curl_strequal(imapc->mailbox, "INBOX"))) &&
     (!imap->uidvalidity_set || !imapc->mb_uidvalidity_set ||
      (imap->uidvalidity == imapc->mb_uidvalidity)))
    selected = TRUE;

  /* Start the first command in the DO phase */
  만약(data->state.upload || IS_MIME_POST(data))
    /* APPEND can be executed directly */
    result = imap_perform_append(data, imapc, imap);
  아니면 만약(imap->custom && (selected || !imap->mailbox))
    /* Custom command using the same mailbox or no mailbox */
    result = imap_perform_list(data, imapc, imap);
  아니면 만약(!imap->custom && selected && (imap->uid || imap->mindex))
    /* FETCH from the same mailbox */
    result = imap_perform_fetch(data, imapc, imap);
  아니면 만약(!imap->custom && selected && imap->query)
    /* SEARCH the current mailbox */
    result = imap_perform_search(data, imapc, imap);
  아니면 만약(imap->mailbox && !selected &&
          (imap->custom || imap->uid || imap->mindex || imap->query))
    /* SELECT the mailbox */
    result = imap_perform_select(data, imapc, imap);
  아니면
    /* LIST */
    result = imap_perform_list(data, imapc, imap);

  만약(result)
    반환 result;

  /* Run the state-machine */
  result = imap_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(conn, FIRSTSOCKET);

  만약(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  반환 result;
}

/* Call this when the DO phase has completed */
static CURLcode imap_dophase_done(struct Curl_easy *data,
                                  struct IMAP *imap,
                                  bool connected)
{
  (void)connected;

  만약(imap->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);

  반환 CURLE_OK;
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
  만약(!result && *dophase_done)
    result = imap_dophase_done(data, imap, connected);

  반환 result;
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

  만약(!imap)
    반환 CURLE_FAILED_INIT;
  /* Parse the URL path */
  result = imap_parse_url_path(data, imap);
  만약(result)
    반환 result;

  /* Parse the custom request */
  result = imap_parse_custom_request(data, imap);
  만약(result)
    반환 result;

  result = imap_regular_transfer(data, imap, done);

  반환 result;
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

  만약(imapc &&
     /* We cannot send quit unconditionally. If this connection is stale or
        bad in any way (pingpong has pending data to send),
        sending quit and waiting around here will make the
        disconnect wait in vain and cause more problems than we need to. */
     !dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &imapc->pp) &&
     !imap_perform_logout(data, imapc))
    (void)imap_block_statemach(data, imapc, TRUE); /* ignore errors */

  반환 CURLE_OK;
}

/* Called from multi.c while DOing */
static CURLcode imap_doing(struct Curl_easy *data, bool *dophase_done)
{
  struct IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  CURLcode result;

  만약(!imap)
    반환 CURLE_FAILED_INIT;

  result = imap_multi_statemach(data, dophase_done);
  만약(result)
    DEBUGF(infof(data, "DO phase failed"));
  아니면 만약(*dophase_done) {
    result = imap_dophase_done(data, imap, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  반환 result;
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
  만약(!imapc)
    반환 CURLE_OUT_OF_MEMORY;

  pp = &imapc->pp;
  PINGPONG_SETUP(pp, imap_pp_statemachine, imap_endofresp);

  /* Set the default preferred authentication type and mechanism */
  imapc->preftype = IMAP_TYPE_ANY;
  Curl_sasl_init(&imapc->sasl, data, &saslimap);

  Curl_pp_init(pp, Curl_pgrs_now(data));

  만약(Curl_conn_meta_set(conn, CURL_META_IMAP_CONN, imapc, imap_conn_dtor))
    반환 CURLE_OUT_OF_MEMORY;

  imap = curlx_calloc(1, sizeof(struct IMAP));
  만약(!imap ||
     Curl_meta_set(data, CURL_META_IMAP_EASY, imap, imap_easy_dtor))
    반환 CURLE_OUT_OF_MEMORY;

  반환 CURLE_OK;
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
