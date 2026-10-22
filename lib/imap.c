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
#ضمن "curl_setup.h"
#ضمن "urldata.h"
#ضمن "imap.h"

#ifndef CURL_DISABLE_IMAP

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#ضمن <netdb.h>
#endif
#ifdef __VMS
#ضمن <in.h>
#ضمن <inet.h>
#endif

#ضمن "curlx/dynbuf.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "progress.h"
#ضمن "transfer.h"
#ضمن "escape.h"
#ضمن "pingpong.h"
#ضمن "mime.h"
#ضمن "curlx/strparse.h"
#ضمن "strcase.h"
#ضمن "vtls/vtls.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "url.h"
#ضمن "bufref.h"
#ضمن "curl_sasl.h"
#ضمن "curlx/strcopy.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_IMAP_EASY   "meta:proto:imap:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_IMAP_CONN   "meta:proto:imap:conn"

تعريفنوع تعداد {
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
بنية imap_conn {
  بنية pingpong pp;
  بنية SASL sasl;           /* SASL-related parameters */
  حرف *mailbox;              /* The last selected mailbox */
  imapstate state;            /* Always use imap.c:state() to change state! */
  unsigned صحيح mb_uidvalidity; /* UIDVALIDITY parsed from select response */
  حرف resptag[5];            /* Response tag to wait for */
  unsigned حرف preftype;     /* Preferred authentication type */
  unsigned حرف cmdid;        /* Last used command ID */
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
بنية IMAP {
  curl_pp_transfer transfer;
  حرف *mailbox;          /* Mailbox to select */
  حرف *uid;              /* Message UID to fetch */
  حرف *mindex;           /* Index in mail box of mail to fetch */
  حرف *section;          /* Message SECTION to fetch */
  حرف *partial;          /* Message PARTIAL to fetch */
  حرف *query;            /* Query to search for */
  حرف *custom;           /* Custom request */
  حرف *custom_params;    /* Parameters for the custom request */
  unsigned صحيح uidvalidity; /* UIDVALIDITY to check in select */
  BIT(uidvalidity_set);
};

#define IMAP_RESP_OK       1
#define IMAP_RESP_NOT_OK   2
#define IMAP_RESP_PREAUTH  3

بنية ulbits {
  صحيح bit;
  ثابت حرف *flag;
};

/***********************************************************************
 *
 * imap_sendf()
 *
 * Sends the formatted string as an IMAP command to the server.
 *
 * Designed to never block.
 */
ساكن CURLcode imap_sendf(بنية Curl_easy *data,
                           بنية imap_conn *imapc,
                           ثابت حرف *fmt, ...) CURL_PRINTF(3, 4);
ساكن CURLcode imap_sendf(بنية Curl_easy *data,
                           بنية imap_conn *imapc,
                           ثابت حرف *fmt, ...)
{
  CURLcode result = CURLE_OK;
  va_list ap;

  DEBUGASSERT(fmt);

  /* Calculate the tag based on the connection ID and command ID */
  curl_msnprintf(imapc->resptag, حجم(imapc->resptag), "%c%03d",
                 'A' + curlx_sltosi((long)(data->conn->connection_id % 26)),
                 ++imapc->cmdid);

  va_start(ap, fmt);
  result = Curl_pp_vsendf(data, &imapc->pp, imapc->resptag, fmt, ap);
  va_end(ap);
  החזר result;
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
ساكن حرف *imap_atom(ثابت حرف *str, bool escape_only)
{
  بنية dynbuf line;
  size_t nclean;
  size_t len;

  אם(!str)
    החזר عدم;

  len = strlen(str);
  nclean = strcspn(str, "() {%*]\\\"");
  אם(len == nclean)
    /* nothing to escape, return a strdup */
    החזר curlx_strdup(str);

  curlx_dyn_init(&line, 2000);

  אם(!escape_only && curlx_dyn_addn(&line, "\"", 1))
    החזר عدم;

  כלעוד(*str) {
    אם((*str == '\\' || *str == '"') &&
       curlx_dyn_addn(&line, "\\", 1))
      החזר عدم;
    אם(curlx_dyn_addn(&line, str, 1))
      החזר عدم;
    str++;
  }

  אם(!escape_only && curlx_dyn_addn(&line, "\"", 1))
    החזר عدم;

  החזר curlx_dyn_ptr(&line);
}

/*
 * Finds the start of a literal '{size}' in line, skipping over quoted strings.
 */
ساكن ثابت حرف *imap_find_literal(ثابت حرف *line, size_t len)
{
  ثابت حرف *end = line + len;
  bool in_quote = FALSE;

  כלעוד(line < end) {
    אם(in_quote) {
      אם(*line == '\\' && (line + 1) < end) {
        line += 2;
        استمر;
      }
      אם(*line == '"')
        in_quote = FALSE;
    }
    אחרת {
      אם(*line == '"')
        in_quote = TRUE;
      אחרת אם(*line == '{')
        החזר line;
    }
    line++;
  }
  החזר عدم;
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
ساكن bool imap_matchresp(ثابت حرف *line, size_t len, ثابت حرف *cmd)
{
  ثابت حرف *end = line + len;
  size_t cmd_len = strlen(cmd);

  /* Skip the untagged response marker */
  line += 2;

  /* Do we have a number after the marker? */
  אם(line < end && ISDIGIT(*line)) {
    /* Skip the number */
    עשה
      line++;
    כלעוד(line < end && ISDIGIT(*line));

    /* Do we have the space character? */
    אם(line == end || *line != ' ')
      החזר FALSE;

    line++;
  }

  /* Does the command name match and is it followed by a space character or at
     the end of line? */
  אם(line + cmd_len <= end && curl_strnequal(line, cmd, cmd_len) &&
     (line[cmd_len] == ' ' || line + cmd_len + 2 == end))
    החזר TRUE;

  החזר FALSE;
}

/***********************************************************************
 *
 * imap_endofresp()
 *
 * Checks whether the given string is a valid tagged, untagged or continuation
 * response which can be processed by the response handler.
 */
ساكن bool imap_endofresp(بنية Curl_easy *data, بنية connectdata *conn,
                           ثابت حرف *line, size_t len, صحيح *resp)
{
  بنية imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  بنية IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  ثابت حرف *id;
  size_t id_len;

  DEBUGASSERT(imapc);
  DEBUGASSERT(imap);
  אם(!imapc || !imap)
    החזר FALSE;

  /* Do we have a tagged command response? */
  id = imapc->resptag;
  id_len = strlen(id);
  אם(len >= id_len + 1 && !memcmp(id, line, id_len) && line[id_len] == ' ') {
    line += id_len + 1;
    len -= id_len + 1;

    אם(len >= 2 && !memcmp(line, "OK", 2))
      *resp = IMAP_RESP_OK;
    אחרת אם(len >= 7 && !memcmp(line, "PREAUTH", 7))
      *resp = IMAP_RESP_PREAUTH;
    אחרת
      *resp = IMAP_RESP_NOT_OK;

    החזר TRUE;
  }

  /* Do we have an untagged command response? */
  אם(len >= 2 && !memcmp("* ", line, 2)) {
    بدل(imapc->state) {
    /* States which are interested in untagged responses */
    حالة IMAP_CAPABILITY:
      אם(!imap_matchresp(line, len, "CAPABILITY"))
        החזר FALSE;
      اكسر;

    حالة IMAP_LIST:
      אם((!imap->custom && !imap_matchresp(line, len, "LIST")) ||
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
        החזר FALSE;
      اكسر;

    حالة IMAP_SELECT:
      /* SELECT is special in that its untagged responses do not have a
         common prefix so accept anything! */
      اكسر;

    حالة IMAP_FETCH:
      אם(!imap_matchresp(line, len, "FETCH"))
        החזר FALSE;
      اكسر;

    حالة IMAP_SEARCH:
      אם(!imap_matchresp(line, len, "SEARCH"))
        החזר FALSE;
      اكسر;

    /* Ignore other untagged responses */
    افتراضي:
      החזר FALSE;
    }

    *resp = '*';
    החזר TRUE;
  }

  /* Do we have a continuation response? This should be a + symbol followed by
     a space and optionally some text as per RFC-3501 for the AUTHENTICATE and
     APPEND commands and as outlined in Section 4. Examples of RFC-4959 but
     some email servers ignore this and only send a single + instead. */
  אם(!imap->custom && ((len == 3 && line[0] == '+') ||
                       (len >= 2 && !memcmp("+ ", line, 2)))) {
    بدل(imapc->state) {
    /* States which are interested in continuation responses */
    حالة IMAP_AUTHENTICATE:
    حالة IMAP_APPEND:
      *resp = '+';
      اكسر;

    افتراضي:
      failf(data, "Unexpected continuation response");
      *resp = -1;
      اكسر;
    }

    החזר TRUE;
  }

  החזר FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * imap_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ساكن CURLcode imap_get_message(بنية Curl_easy *data, بنية bufref *out)
{
  بنية imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  حرف *message;
  size_t len;

  אם(!imapc)
    החזר CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&imapc->pp.recvbuf);
  len = imapc->pp.nfinal;
  אם(len > 2) {
    /* Find the start of the message */
    len -= 2;
    עבור(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    כלעוד(len--)
      אם(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        اكسر;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, عدم);
  }
  אחרת
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, عدم);

  החזר CURLE_OK;
}

/***********************************************************************
 *
 * imap_state()
 *
 * This is the ONLY way to change IMAP state!
 */
ساكن فراغ imap_state(بنية Curl_easy *data,
                       بنية imap_conn *imapc,
                       imapstate newstate)
{
#אם defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ساكن ثابت حرف * ثابت names[] = {
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

  אם(imapc->state != newstate)
    infof(data, "IMAP %p state change from %s to %s",
          (فراغ *)imapc, names[imapc->state], names[newstate]);
#אחרת
  (فراغ)data;
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
ساكن CURLcode imap_perform_capability(بنية Curl_easy *data,
                                        بنية imap_conn *imapc)
{
  CURLcode result = CURLE_OK;

  imapc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  imapc->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  imapc->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPABILITY command */
  result = imap_sendf(data, imapc, "CAPABILITY");

  אם(!result)
    imap_state(data, imapc, IMAP_CAPABILITY);

  החזר result;
}

/***********************************************************************
 *
 * imap_perform_starttls()
 *
 * Sends the STARTTLS command to start the upgrade to TLS.
 */
ساكن CURLcode imap_perform_starttls(بنية Curl_easy *data,
                                      بنية imap_conn *imapc)
{
  /* Send the STARTTLS command */
  CURLcode result = imap_sendf(data, imapc, "STARTTLS");

  אם(!result)
    imap_state(data, imapc, IMAP_STARTTLS);

  החזר result;
}

/***********************************************************************
 *
 * imap_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ساكن CURLcode imap_perform_upgrade_tls(بنية Curl_easy *data,
                                         بنية imap_conn *imapc,
                                         بنية connectdata *conn)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  CURLcode result;
  bool ssldone = FALSE;

  אם(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    אם(result)
      اذهب out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_imaps;
  }

  DEBUGASSERT(!imapc->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "imap_perform_upgrade_tls, connect -> %d, %d",
               (صحيح)result, ssldone));
  אם(!result && ssldone) {
    imapc->ssldone = ssldone;
    /* perform CAPA now, changes imapc->state out of IMAP_UPGRADETLS */
    result = imap_perform_capability(data, imapc);
  }
out:
  החזר result;
#אחרת
  (فراغ)data;
  (فراغ)imapc;
  (فراغ)conn;
  החזר CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * imap_perform_login()
 *
 * Sends a clear text LOGIN command to authenticate with.
 */
ساكن CURLcode imap_perform_login(بنية Curl_easy *data,
                                   بنية imap_conn *imapc,
                                   بنية connectdata *conn)
{
  CURLcode result = CURLE_OK;
  حرف *user;
  حرف *passwd;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  אם(!conn->creds) {
    imap_state(data, imapc, IMAP_STOP);

    החזר result;
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

  אם(!result)
    imap_state(data, imapc, IMAP_LOGIN);

  החזר result;
}

/***********************************************************************
 *
 * imap_perform_authenticate()
 *
 * Sends an AUTHENTICATE command allowing the client to login with the given
 * SASL authentication mechanism.
 */
ساكن CURLcode imap_perform_authenticate(بنية Curl_easy *data,
                                          ثابت حرف *mech,
                                          ثابت بنية bufref *initresp)
{
  بنية imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  CURLcode result = CURLE_OK;
  ثابت حرف *ir = Curl_bufref_ptr(initresp);

  אם(!imapc)
    החזר CURLE_FAILED_INIT;
  אם(ir) {
    /* Send the AUTHENTICATE command with the initial response */
    result = imap_sendf(data, imapc, "AUTHENTICATE %s %s",
                        mech, *ir ? ir : "=");
  }
  אחרת {
    /* Send the AUTHENTICATE command */
    result = imap_sendf(data, imapc, "AUTHENTICATE %s", mech);
  }

  החזר result;
}

/***********************************************************************
 *
 * imap_continue_authenticate()
 *
 * Sends SASL continuation data.
 */
ساكن CURLcode imap_continue_authenticate(بنية Curl_easy *data,
                                           ثابت حرف *mech,
                                           ثابت بنية bufref *resp)
{
  بنية imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  (فراغ)mech;
  אם(!imapc)
    החזר CURLE_FAILED_INIT;
  החזר Curl_pp_sendf(data, &imapc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * imap_cancel_authenticate()
 *
 * Sends SASL cancellation.
 */
ساكن CURLcode imap_cancel_authenticate(بنية Curl_easy *data,
                                         ثابت حرف *mech)
{
  بنية imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  (فراغ)mech;
  אם(!imapc)
    החזר CURLE_FAILED_INIT;
  החזר Curl_pp_sendf(data, &imapc->pp, "*");
}

/***********************************************************************
 *
 * imap_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to clear text should a common
 * mechanism not be available between the client and server.
 */
ساكن CURLcode imap_perform_authentication(بنية Curl_easy *data,
                                            بنية imap_conn *imapc)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  /* Check if already authenticated OR if there is enough data to authenticate
     with and end the connect phase if we do not */
  אם(imapc->preauth ||
     !Curl_sasl_can_authenticate(&imapc->sasl, data)) {
    imap_state(data, imapc, IMAP_STOP);
    החזר result;
  }

  /* Calculate the SASL login details */
  result = Curl_sasl_start(&imapc->sasl, data, (bool)imapc->ir_supported,
                           &progress);
  אם(!result) {
    אם(progress == SASL_INPROGRESS)
      imap_state(data, imapc, IMAP_AUTHENTICATE);
    אחרת אם(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
      /* Perform clear text authentication */
      result = imap_perform_login(data, imapc, data->conn);
    אחרת
      result = Curl_sasl_is_blocked(&imapc->sasl, data);
  }

  החזר result;
}

/***********************************************************************
 *
 * imap_perform_list()
 *
 * Sends a LIST command or an alternative custom request.
 */
ساكن CURLcode imap_perform_list(بنية Curl_easy *data,
                                  بنية imap_conn *imapc,
                                  بنية IMAP *imap)
{
  CURLcode result = CURLE_OK;

  אם(imap->custom)
    /* Send the custom request */
    result = imap_sendf(data, imapc, "%s%s", imap->custom,
                        imap->custom_params ? imap->custom_params : "");
  אחרת {
    /* Make sure the mailbox is in the correct atom format if necessary */
    حرف *mailbox = imap->mailbox ? imap_atom(imap->mailbox, TRUE)
                                  : curlx_strdup("");
    אם(!mailbox)
      החזר CURLE_OUT_OF_MEMORY;

    /* Send the LIST command */
    result = imap_sendf(data, imapc, "LIST \"%s\" *", mailbox);

    curlx_free(mailbox);
  }

  אם(!result)
    imap_state(data, imapc, IMAP_LIST);

  החזר result;
}

/***********************************************************************
 *
 * imap_perform_select()
 *
 * Sends a SELECT command to ask the server to change the selected mailbox.
 */
ساكن CURLcode imap_perform_select(بنية Curl_easy *data,
                                    بنية imap_conn *imapc,
                                    بنية IMAP *imap)
{
  CURLcode result = CURLE_OK;
  حرف *mailbox;

  /* Invalidate old information as we are switching mailboxes */
  curlx_safefree(imapc->mailbox);
  imapc->mb_uidvalidity_set = FALSE;

  /* Check we have a mailbox */
  אם(!imap->mailbox) {
    failf(data, "Cannot SELECT without a mailbox.");
    החזר CURLE_URL_MALFORMAT;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  אם(!mailbox)
    החזר CURLE_OUT_OF_MEMORY;

  /* Send the SELECT command */
  result = imap_sendf(data, imapc, "SELECT %s", mailbox);

  curlx_free(mailbox);

  אם(!result)
    imap_state(data, imapc, IMAP_SELECT);

  החזר result;
}

/***********************************************************************
 *
 * imap_perform_fetch()
 *
 * Sends a FETCH command to initiate the download of a message.
 */
ساكن CURLcode imap_perform_fetch(بنية Curl_easy *data,
                                   بنية imap_conn *imapc,
                                   بنية IMAP *imap)
{
  CURLcode result = CURLE_OK;
  /* Check we have a UID */
  אם(imap->uid) {

    /* Send the FETCH command */
    אם(imap->partial)
      result = imap_sendf(data, imapc, "UID FETCH %s BODY[%s]<%s>",
                          imap->uid, imap->section ? imap->section : "",
                          imap->partial);
    אחרת
      result = imap_sendf(data, imapc, "UID FETCH %s BODY[%s]",
                          imap->uid, imap->section ? imap->section : "");
  }
  אחרת אם(imap->mindex) {
    /* Send the FETCH command */
    אם(imap->partial)
      result = imap_sendf(data, imapc, "FETCH %s BODY[%s]<%s>",
                          imap->mindex, imap->section ? imap->section : "",
                          imap->partial);
    אחרת
      result = imap_sendf(data, imapc, "FETCH %s BODY[%s]",
                          imap->mindex, imap->section ? imap->section : "");
  }
  אחרת {
    failf(data, "Cannot FETCH without a UID.");
    החזר CURLE_URL_MALFORMAT;
  }
  אם(!result)
    imap_state(data, imapc, IMAP_FETCH);

  החזר result;
}

/***********************************************************************
 *
 * imap_perform_append()
 *
 * Sends an APPEND command to initiate the upload of a message.
 */
ساكن CURLcode imap_perform_append(بنية Curl_easy *data,
                                    بنية imap_conn *imapc,
                                    بنية IMAP *imap)
{
  CURLcode result = CURLE_OK;
  حرف *mailbox;
  بنية dynbuf flags;

  /* Check we have a mailbox */
  אם(!imap->mailbox) {
    failf(data, "Cannot APPEND without a mailbox.");
    החזר CURLE_URL_MALFORMAT;
  }

#ifndef CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  אם(IS_MIME_POST(data)) {
    curl_mimepart *postp = data->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(unsigned صحيح)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, data->set.headers, 0);
    result = Curl_mime_prepare_headers(data, postp, عدم,
                                       عدم, MIMESTRATEGY_MAIL);

    אם(!result && !Curl_checkheaders(data, STRCONST("Mime-Version")))
      result = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    אם(!result)
      result = Curl_creader_set_mime(data, postp);
    אם(result)
      החזר result;
  }
  אחרת
#endif
  {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    אם(result)
      החזר result;
  }

  /* Check we know the size of the upload. This takes all readers
   * into account. Especially crlf conversions which make the size
   * unpredictable, e.g. -1. */
  data->state.infilesize = Curl_creader_total_length(data);
  אם(data->state.infilesize < 0) {
    אם(data->set.crlf)
      failf(data, "Cannot APPEND with CRLF conversion making size unknown");
    אחרת
      failf(data, "Cannot APPEND with unknown input file size");
    החזר CURLE_UPLOAD_FAILED;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  אם(!mailbox)
    החזר CURLE_OUT_OF_MEMORY;

  /* Generate flags string and send the APPEND command */
  curlx_dyn_init(&flags, 100);
  אם(data->set.upload_flags) {
    صحيح i;
    بنية ulbits ulflag[] = {
      { CURLULFLAG_ANSWERED, "Answered" },
      { CURLULFLAG_DELETED, "Deleted" },
      { CURLULFLAG_DRAFT, "Draft" },
      { CURLULFLAG_FLAGGED, "Flagged" },
      { CURLULFLAG_SEEN, "Seen" },
      { 0, عدم }
    };

    result = CURLE_OUT_OF_MEMORY;
    אם(curlx_dyn_add(&flags, " (")) {
      اذهب cleanup;
    }

    עבור(i = 0; ulflag[i].bit; i++) {
      אם(data->set.upload_flags & ulflag[i].bit &&
         ((curlx_dyn_len(&flags) > 2 && curlx_dyn_add(&flags, " ")) ||
          curlx_dyn_add(&flags, "\\") ||
          curlx_dyn_add(&flags, ulflag[i].flag)))
        اذهب cleanup;
    }

    אם(curlx_dyn_add(&flags, ")"))
      اذهب cleanup;
  }
  אחרת אם(curlx_dyn_add(&flags, ""))
    اذهب cleanup;

  result = imap_sendf(data, imapc, "APPEND %s%s {%" FMT_OFF_T "}",
                      mailbox, curlx_dyn_ptr(&flags), data->state.infilesize);

cleanup:
  curlx_dyn_free(&flags);
  curlx_free(mailbox);

  אם(!result)
    imap_state(data, imapc, IMAP_APPEND);

  החזר result;
}

/***********************************************************************
 *
 * imap_perform_search()
 *
 * Sends a SEARCH command.
 */
ساكن CURLcode imap_perform_search(بنية Curl_easy *data,
                                    بنية imap_conn *imapc,
                                    بنية IMAP *imap)
{
  CURLcode result = CURLE_OK;

  /* Check we have a query string */
  אם(!imap->query) {
    failf(data, "Cannot SEARCH without a query string.");
    החזר CURLE_URL_MALFORMAT;
  }

  /* Send the SEARCH command */
  result = imap_sendf(data, imapc, "SEARCH %s", imap->query);

  אם(!result)
    imap_state(data, imapc, IMAP_SEARCH);

  החזר result;
}

/***********************************************************************
 *
 * imap_perform_logout()
 *
 * Performs the logout action prior to sclose() being called.
 */
ساكن CURLcode imap_perform_logout(بنية Curl_easy *data,
                                    بنية imap_conn *imapc)
{
  /* Send the LOGOUT command */
  CURLcode result = imap_sendf(data, imapc, "LOGOUT");

  אם(!result)
    imap_state(data, imapc, IMAP_LOGOUT);

  החזר result;
}

/* For the initial server greeting */
ساكن CURLcode imap_state_servergreet_resp(بنية Curl_easy *data,
                                            بنية imap_conn *imapc,
                                            صحيح imapcode,
                                            imapstate instate)
{
  (فراغ)instate;

  אם(imapcode == IMAP_RESP_PREAUTH) {
    /* PREAUTH */
    imapc->preauth = TRUE;
    infof(data, "PREAUTH connection, already authenticated");
  }
  אחרת אם(imapcode != IMAP_RESP_OK) {
    failf(data, "Got unexpected imap-server response");
    החזר CURLE_WEIRD_SERVER_REPLY;
  }

  החזר imap_perform_capability(data, imapc);
}

/* For CAPABILITY responses */
ساكن CURLcode imap_state_capability_resp(بنية Curl_easy *data,
                                           بنية imap_conn *imapc,
                                           صحيح imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  ثابت حرف *line = curlx_dyn_ptr(&imapc->pp.recvbuf);

  (فراغ)instate;

  /* Do we have an untagged response? */
  אם(imapcode == '*') {
    line += 2;

    /* Loop through the data line */
    עבור(;;) {
      size_t wordlen;
      כלעוד(*line && (ISBLANK(*line) || ISNEWLINE(*line)))
        line++;

      אם(!*line)
        اكسر;

      /* Extract the word */
      עבור(wordlen = 0; line[wordlen] && !ISBLANK(line[wordlen]) &&
                       !ISNEWLINE(line[wordlen]);)
        wordlen++;

      /* Does the server support the STARTTLS capability? */
      אם(wordlen == 8 && curl_strnequal(line, "STARTTLS", 8))
        imapc->tls_supported = TRUE;

      /* Has the server explicitly disabled clear text authentication? */
      אחרת אם(wordlen == 13 && curl_strnequal(line, "LOGINDISABLED", 13))
        imapc->login_disabled = TRUE;

      /* Does the server support the SASL-IR capability? */
      אחרת אם(wordlen == 7 && curl_strnequal(line, "SASL-IR", 7))
        imapc->ir_supported = TRUE;

      /* Do we have a SASL based authentication mechanism? */
      אחרת אם(wordlen > 5 && curl_strnequal(line, "AUTH=", 5)) {
        size_t llen;
        unsigned short mechbit;

        line += 5;
        wordlen -= 5;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(line, wordlen, &llen);
        אם(mechbit && llen == wordlen)
          imapc->sasl.authmechs |= mechbit;
      }

      line += wordlen;
    }
  }
  אחרת אם(data->set.use_ssl && !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
    /* PREAUTH is not compatible with STARTTLS. */
    אם(imapcode == IMAP_RESP_OK && imapc->tls_supported && !imapc->preauth) {
      /* Switch to TLS connection now */
      result = imap_perform_starttls(data, imapc);
    }
    אחרת אם(data->set.use_ssl <= CURLUSESSL_TRY)
      result = imap_perform_authentication(data, imapc);
    אחרת {
      failf(data, "STARTTLS not available.");
      result = CURLE_USE_SSL_FAILED;
    }
  }
  אחרת
    result = imap_perform_authentication(data, imapc);

  החזר result;
}

/* For STARTTLS responses */
ساكن CURLcode imap_state_starttls_resp(بنية Curl_easy *data,
                                         بنية imap_conn *imapc,
                                         صحيح imapcode,
                                         imapstate instate)
{
  CURLcode result = CURLE_OK;

  (فراغ)instate;

  /* Pipelining in response is forbidden. */
  אם(imapc->pp.overflow)
    החזר CURLE_WEIRD_SERVER_REPLY;

  אם(imapcode != IMAP_RESP_OK) {
    אם(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied");
      result = CURLE_USE_SSL_FAILED;
    }
    אחרת
      result = imap_perform_authentication(data, imapc);
  }
  אחרת
    imap_state(data, imapc, IMAP_UPGRADETLS);

  החזר result;
}

/* For SASL authentication responses */
ساكن CURLcode imap_state_auth_resp(بنية Curl_easy *data,
                                     بنية imap_conn *imapc,
                                     صحيح imapcode,
                                     imapstate instate)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  (فراغ)instate;

  result = Curl_sasl_continue(&imapc->sasl, data, imapcode, &progress);
  אם(!result)
    بدل(progress) {
    حالة SASL_DONE:
      imap_state(data, imapc, IMAP_STOP);  /* Authenticated */
      اكسر;
    حالة SASL_IDLE:            /* No mechanism left after cancellation */
      אם(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
        /* Perform clear text authentication */
        result = imap_perform_login(data, imapc, data->conn);
      אחרת {
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
      }
      اكسر;
    افتراضي:
      اكسر;
    }

  החזר result;
}

/* For LOGIN responses */
ساكن CURLcode imap_state_login_resp(بنية Curl_easy *data,
                                      بنية imap_conn *imapc,
                                      صحيح imapcode,
                                      imapstate instate)
{
  CURLcode result = CURLE_OK;
  (فراغ)instate;

  אם(imapcode != IMAP_RESP_OK) {
    failf(data, "Access denied. %c", imapcode);
    result = CURLE_LOGIN_DENIED;
  }
  אחרת
    /* End of connect phase */
    imap_state(data, imapc, IMAP_STOP);

  החזר result;
}

/* Detect IMAP listings vs. downloading a single email */
ساكن bool is_custom_fetch_listing_match(ثابت حرف *params)
{
  /* match " 1:* (FLAGS ..." or " 1,2,3 (FLAGS ..." */
  אם(*params++ != ' ')
    החזר FALSE;

  כלעוד(ISDIGIT(*params)) {
    params++;
    אם(*params == 0)
      החזר FALSE;
  }
  אם(*params == ':')
    החזר TRUE;
  אם(*params == ',')
    החזר TRUE;
  החזר FALSE;
}

ساكن bool is_custom_fetch_listing(بنية IMAP *imap)
{
  /* filter out "UID FETCH 1:* (FLAGS ..." queries to list emails */
  אם(!imap->custom)
    החזר FALSE;
  אחרת אם(curl_strequal(imap->custom, "FETCH") && imap->custom_params) {
    ثابت حرف *p = imap->custom_params;
    החזר is_custom_fetch_listing_match(p);
  }
  אחרת אם(curl_strequal(imap->custom, "UID") && imap->custom_params &&
          curl_strnequal(imap->custom_params, " FETCH ", 7)) {
    ثابت حرف *p = imap->custom_params + 6;
    החזר is_custom_fetch_listing_match(p);
  }
  החזר FALSE;
}

/* For LIST and SEARCH responses */
ساكن CURLcode imap_state_listsearch_resp(بنية Curl_easy *data,
                                           بنية imap_conn *imapc,
                                           صحيح imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  ثابت حرف *line = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  بنية IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);

  DEBUGASSERT(imap);
  אם(!imap)
    החזר CURLE_FAILED_INIT;
  (فراغ)instate;

  אם(imapcode == '*' && is_custom_fetch_listing(imap)) {
    /* custom FETCH or UID FETCH for listing is not handled here */
  }
  אחרת אם(imapcode == '*') {
    /* Check if this response contains a literal (e.g. FETCH responses with
       body data). Literal syntax is {size}\r\n */
    ثابت حرف *cr = memchr(line, '\r', len);
    size_t line_len = cr ? (size_t)(cr - line) : len;
    ثابت حرف *ptr = imap_find_literal(line, line_len);
    אם(ptr) {
      curl_off_t size = 0;
      bool parsed = FALSE;
      ptr++;
      אם(!curlx_str_number(&ptr, &size, CURL_OFF_T_MAX) &&
         !curlx_str_single(&ptr, '}'))
        parsed = TRUE;

      אם(parsed) {
        بنية pingpong *pp = &imapc->pp;
        size_t buffer_len = curlx_dyn_len(&pp->recvbuf);
        size_t after_header = buffer_len - pp->nfinal;

        /* This is a literal response, setup to receive the body data */
        infof(data, "Found %" FMT_OFF_T " bytes to download", size);

        /* First write the header line */
        result = Curl_client_write(data, CLIENTWRITE_BODY, line, len);
        אם(result)
          החזר result;

        /* Handle data already in buffer after the header line */
        אם(after_header > 0) {
          /* There is already data in the buffer that is part of the literal
             body or subsequent responses */
          size_t chunk = after_header;

          /* Keep only the data after the header line */
          curlx_dyn_tail(&pp->recvbuf, chunk);
          pp->nfinal = 0; /* done */

          /* Limit chunk to the literal size */
          אם((curl_off_t)chunk > size)
            chunk = (size_t)size;

          אם(chunk) {
            /* Write the literal body data */
            result = Curl_client_write(data, CLIENTWRITE_BODY,
                                       curlx_dyn_ptr(&pp->recvbuf), chunk);
            אם(result)
              החזר result;
          }

          /* Handle remaining data in buffer (either more literal data or
             subsequent responses) */
          אם(after_header > chunk) {
            /* Keep the data after the literal body */
            pp->overflow = after_header - chunk;
            curlx_dyn_tail(&pp->recvbuf, pp->overflow);
          }
          אחרת {
            pp->overflow = 0;
            curlx_dyn_reset(&pp->recvbuf);
          }
        }
        אחרת {
          /* No data in buffer yet, reset overflow */
          pp->overflow = 0;
        }

        אם((CURL_OFF_T_MAX - size) < (curl_off_t)len)
          /* unlikely to actually be a transfer this big, but avoid integer
             overflow */
          size = CURL_OFF_T_MAX;
        אחרת
          size += len;

        /* Progress size includes both header line and literal body */
        Curl_pgrsSetDownloadSize(data, size);

        אם(data->req.bytecount == size)
          /* All data already transferred (header + literal body) */
          Curl_xfer_setup_nop(data);
        אחרת {
          /* Setup to receive the literal body data.
             maxdownload and transfer size include both header line and
             literal body */
          data->req.maxdownload = size;
          Curl_xfer_setup_recv(data, FIRSTSOCKET, size);
        }
        /* End of DO phase */
        imap_state(data, imapc, IMAP_STOP);
      }
      אחרת {
        /* Failed to parse literal, write the line */
        result = Curl_client_write(data, CLIENTWRITE_BODY, line, len);
      }
    }
    אחרת {
      /* No literal, write the line as-is */
      result = Curl_client_write(data, CLIENTWRITE_BODY, line, len);
    }
  }
  אחרת אם(imapcode != IMAP_RESP_OK)
    result = CURLE_QUOTE_ERROR;
  אחרת
    /* End of DO phase */
    imap_state(data, imapc, IMAP_STOP);

  החזר result;
}

/* For SELECT responses */
ساكن CURLcode imap_state_select_resp(بنية Curl_easy *data,
                                       بنية imap_conn *imapc,
                                       بنية IMAP *imap,
                                       صحيح imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  (فراغ)instate;

  אם(imapcode == '*') {
    /* See if this is an UIDVALIDITY response */
    ثابت حرف *line = curlx_dyn_ptr(&imapc->pp.recvbuf);
    size_t len = curlx_dyn_len(&imapc->pp.recvbuf);
    אם((len >= 18) && checkprefix("OK [UIDVALIDITY ", &line[2])) {
      curl_off_t value;
      ثابت حرف *p = &line[2] + CURL_CSTRLEN("OK [UIDVALIDITY ");
      אם(!curlx_str_number(&p, &value, UINT_MAX)) {
        imapc->mb_uidvalidity = (unsigned صحيح)value;
        imapc->mb_uidvalidity_set = TRUE;
      }
    }
  }
  אחרת אם(imapcode == IMAP_RESP_OK) {
    /* Check if the UIDVALIDITY has been specified and matches */
    אם(imap->uidvalidity_set && imapc->mb_uidvalidity_set &&
       (imap->uidvalidity != imapc->mb_uidvalidity)) {
      failf(data, "Mailbox UIDVALIDITY has changed");
      result = CURLE_REMOTE_FILE_NOT_FOUND;
    }
    אחרת {
      /* Note the currently opened mailbox on this connection */
      DEBUGASSERT(!imapc->mailbox);
      imapc->mailbox = curlx_strdup(imap->mailbox);
      אם(!imapc->mailbox)
        החזר CURLE_OUT_OF_MEMORY;

      אם(imap->custom)
        result = imap_perform_list(data, imapc, imap);
      אחרת אם(imap->query)
        result = imap_perform_search(data, imapc, imap);
      אחרת
        result = imap_perform_fetch(data, imapc, imap);
    }
  }
  אחרת {
    failf(data, "Select failed");
    result = CURLE_LOGIN_DENIED;
  }

  החזר result;
}

/* For the (first line of the) FETCH responses */
ساكن CURLcode imap_state_fetch_resp(بنية Curl_easy *data,
                                      بنية imap_conn *imapc,
                                      صحيح imapcode,
                                      imapstate instate)
{
  CURLcode result = CURLE_OK;
  بنية pingpong *pp = &imapc->pp;
  ثابت حرف *ptr = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  bool parsed = FALSE;
  curl_off_t size = 0;

  (فراغ)instate;

  אם(imapcode != '*') {
    Curl_pgrsSetDownloadSize(data, -1);
    imap_state(data, imapc, IMAP_STOP);
    החזר CURLE_REMOTE_FILE_NOT_FOUND;
  }

  /* Something like this is received "* 1 FETCH (BODY[TEXT] {2021}\r" so parse
     the continuation data contained within the curly brackets */
  ptr = imap_find_literal(ptr, len);
  אם(ptr) {
    ptr++;
    אם(!curlx_str_number(&ptr, &size, CURL_OFF_T_MAX) &&
       !curlx_str_single(&ptr, '}'))
      parsed = TRUE;
  }

  אם(parsed) {
    infof(data, "Found %" FMT_OFF_T " bytes to download", size);
    Curl_pgrsSetDownloadSize(data, size);

    אם(pp->overflow) {
      /* At this point there is a data in the receive buffer that is body
         content, send it as body and then skip it. Do note that there may
         even be additional "headers" after the body. */
      size_t chunk = pp->overflow;

      /* keep only the overflow */
      curlx_dyn_tail(&pp->recvbuf, chunk);
      pp->nfinal = 0; /* done */

      אם((curl_off_t)chunk > size)
        /* The conversion from curl_off_t to size_t is always fine here */
        chunk = (size_t)size;

      אם(!chunk) {
        /* no size, we are done with the data */
        imap_state(data, imapc, IMAP_STOP);
        החזר CURLE_OK;
      }
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 curlx_dyn_ptr(&pp->recvbuf), chunk);
      אם(result)
        החזר result;

      infof(data, "Written %zu bytes, %" FMT_OFF_T
            " bytes are left for transfer", chunk, (curl_off_t)(size - chunk));

      /* Have we used the entire overflow or part of it?*/
      אם(pp->overflow > chunk) {
        /* remember the remaining trailing overflow data */
        pp->overflow -= chunk;
        curlx_dyn_tail(&pp->recvbuf, pp->overflow);
      }
      אחרת {
        pp->overflow = 0; /* handled */
        /* Free the cache */
        curlx_dyn_reset(&pp->recvbuf);
      }
    }

    אם(data->req.bytecount == size)
      /* The entire data is already transferred! */
      Curl_xfer_setup_nop(data);
    אחרת {
      /* IMAP download */
      data->req.maxdownload = size;
      Curl_xfer_setup_recv(data, FIRSTSOCKET, size);
    }
  }
  אחרת {
    /* We do not know how to parse this line */
    failf(data, "Failed to parse FETCH response.");
    result = CURLE_WEIRD_SERVER_REPLY;
  }

  /* End of DO phase */
  imap_state(data, imapc, IMAP_STOP);

  החזר result;
}

/* For final FETCH responses performed after the download */
ساكن CURLcode imap_state_fetch_final_resp(بنية Curl_easy *data,
                                            بنية imap_conn *imapc,
                                            صحيح imapcode,
                                            imapstate instate)
{
  CURLcode result = CURLE_OK;

  (فراغ)instate;

  אם(imapcode != IMAP_RESP_OK)
    result = CURLE_WEIRD_SERVER_REPLY;
  אחרת
    /* End of DONE phase */
    imap_state(data, imapc, IMAP_STOP);

  החזר result;
}

/* For APPEND responses */
ساكن CURLcode imap_state_append_resp(بنية Curl_easy *data,
                                       بنية imap_conn *imapc,
                                       صحيح imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  (فراغ)instate;

  אם(imapcode != '+') {
    result = CURLE_UPLOAD_FAILED;
  }
  אחרת {
    /* Set the progress upload size */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* IMAP upload */
    Curl_xfer_setup_send(data, FIRSTSOCKET);

    /* End of DO phase */
    imap_state(data, imapc, IMAP_STOP);
  }

  החזר result;
}

/* For final APPEND responses performed after the upload */
ساكن CURLcode imap_state_append_final_resp(بنية Curl_easy *data,
                                             بنية imap_conn *imapc,
                                             صحيح imapcode,
                                             imapstate instate)
{
  CURLcode result = CURLE_OK;

  (فراغ)instate;

  אם(imapcode != IMAP_RESP_OK)
    result = CURLE_UPLOAD_FAILED;
  אחרת
    /* End of DONE phase */
    imap_state(data, imapc, IMAP_STOP);

  החזר result;
}

ساكن CURLcode imap_pp_statemachine(بنية Curl_easy *data,
                                     بنية connectdata *conn)
{
  CURLcode result = CURLE_OK;
  صحيح imapcode;
  بنية imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  بنية IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  بنية pingpong *pp;
  size_t nread = 0;

  אם(!imapc || !imap)
    החזר CURLE_FAILED_INIT;
  pp = &imapc->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not IMAP */
upgrade_tls:
  אם(imapc->state == IMAP_UPGRADETLS) {
    result = imap_perform_upgrade_tls(data, imapc, conn);
    אם(result || (imapc->state == IMAP_UPGRADETLS))
      החזר result;
  }

  /* Flush any data that needs to be sent */
  אם(pp->sendleft)
    החזר Curl_pp_flushsend(data, pp);

  עשה {
    /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, pp, &imapcode, &nread);
    אם(result)
      החזר result;

    /* Was there an error parsing the response line? */
    אם(imapcode == -1)
      החזר CURLE_WEIRD_SERVER_REPLY;

    אם(!imapcode)
      اكسر;

    /* We have now received a full IMAP server response */
    بدل(imapc->state) {
    حالة IMAP_SERVERGREET:
      result = imap_state_servergreet_resp(data, imapc,
                                           imapcode, imapc->state);
      اكسر;

    حالة IMAP_CAPABILITY:
      result = imap_state_capability_resp(data, imapc, imapcode, imapc->state);
      اكسر;

    حالة IMAP_STARTTLS:
      result = imap_state_starttls_resp(data, imapc, imapcode, imapc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      אם(!result && (imapc->state == IMAP_UPGRADETLS))
        اذهب upgrade_tls;
      اكسر;

    حالة IMAP_AUTHENTICATE:
      result = imap_state_auth_resp(data, imapc, imapcode, imapc->state);
      اكسر;

    حالة IMAP_LOGIN:
      result = imap_state_login_resp(data, imapc, imapcode, imapc->state);
      اكسر;

    حالة IMAP_LIST:
    حالة IMAP_SEARCH:
      result = imap_state_listsearch_resp(data, imapc, imapcode, imapc->state);
      اكسر;

    حالة IMAP_SELECT:
      result = imap_state_select_resp(data, imapc, imap,
                                      imapcode, imapc->state);
      اكسر;

    حالة IMAP_FETCH:
      result = imap_state_fetch_resp(data, imapc, imapcode, imapc->state);
      اكسر;

    حالة IMAP_FETCH_FINAL:
      result = imap_state_fetch_final_resp(data, imapc,
                                           imapcode, imapc->state);
      اكسر;

    حالة IMAP_APPEND:
      result = imap_state_append_resp(data, imapc, imapcode, imapc->state);
      اكسر;

    حالة IMAP_APPEND_FINAL:
      result = imap_state_append_final_resp(data, imapc,
                                            imapcode, imapc->state);
      اكسر;

    حالة IMAP_LOGOUT:
    افتراضي:
      /* internal error */
      imap_state(data, imapc, IMAP_STOP);
      اكسر;
    }
  } כלעוד(!result && imapc->state != IMAP_STOP && Curl_pp_moredata(pp));

  החזר result;
}

/* Called repeatedly until done from multi.c */
ساكن CURLcode imap_multi_statemach(بنية Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  بنية imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  *done = FALSE;
  אם(!imapc)
    החזר CURLE_FAILED_INIT;
  result = Curl_pp_statemach(data, &imapc->pp, FALSE, FALSE);
  *done = (imapc->state == IMAP_STOP);

  החזר result;
}

ساكن CURLcode imap_block_statemach(بنية Curl_easy *data,
                                     بنية imap_conn *imapc,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;

  כלעוד(imapc->state != IMAP_STOP && !result)
    result = Curl_pp_statemach(data, &imapc->pp, TRUE, disconnecting);

  החזר result;
}

/* For the IMAP "protocol connect" and "doing" phases only */
ساكن CURLcode imap_pollset(بنية Curl_easy *data,
                             بنية easy_pollset *ps)
{
  بنية imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  החזר imapc ? Curl_pp_pollset(data, &imapc->pp, ps) : CURLE_OK;
}

ساكن فراغ imap_easy_reset(بنية IMAP *imap)
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
ساكن bool imap_is_bchar(حرف ch)
{
  /* Performing the alnum check first with macro is faster because of ASCII
     arithmetic */
  החזר ch && (ISALNUM(ch) || strchr(":@/&=-._~!$\'()*+,%", ch));
}

/***********************************************************************
 *
 * imap_parse_url_options()
 *
 * Parse the URL login options.
 */
ساكن CURLcode imap_parse_url_options(بنية connectdata *conn,
                                       بنية imap_conn *imapc)
{
  CURLcode result = CURLE_OK;
  ثابت حرف *ptr = conn->options;
  bool prefer_login = FALSE;

  כלעוד(!result && ptr && *ptr) {
    ثابت حرف *key = ptr;
    ثابت حرف *value;

    כלעוד(*ptr && *ptr != '=')
      ptr++;

    value = ptr + 1;

    כלעוד(*ptr && *ptr != ';')
      ptr++;

    אם(curl_strnequal(key, "AUTH=+LOGIN", 11)) {
      /* User prefers plaintext LOGIN over any SASL, including SASL LOGIN */
      prefer_login = TRUE;
      imapc->sasl.prefmech = SASL_AUTH_NONE;
    }
    אחרת אם(curl_strnequal(key, "AUTH=", 5)) {
      prefer_login = FALSE;
      result = Curl_sasl_parse_url_auth_option(&imapc->sasl,
                                               value, ptr - value);
    }
    אחרת {
      prefer_login = FALSE;
      result = CURLE_URL_MALFORMAT;
    }

    אם(*ptr == ';')
      ptr++;
  }

  אם(prefer_login)
    imapc->preftype = IMAP_TYPE_CLEARTEXT;
  אחרת {
    بدل(imapc->sasl.prefmech) {
    حالة SASL_AUTH_NONE:
      imapc->preftype = IMAP_TYPE_NONE;
      اكسر;
    حالة SASL_AUTH_DEFAULT:
      imapc->preftype = IMAP_TYPE_ANY;
      اكسر;
    افتراضي:
      imapc->preftype = IMAP_TYPE_SASL;
      اكسر;
    }
  }

  החזר result;
}

/***********************************************************************
 *
 * imap_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
ساكن CURLcode imap_parse_url_path(بنية Curl_easy *data,
                                    بنية IMAP *imap)
{
  /* The imap struct is already initialized in imap_connect() */
  CURLcode result = CURLE_OK;
  ثابت حرف *begin = &data->state.up.path[1]; /* skip leading slash */
  ثابت حرف *ptr = begin;

  /* See how much of the URL is a valid path and decode it */
  כלעוד(imap_is_bchar(*ptr))
    ptr++;

  אם(ptr != begin) {
    /* Remove the trailing slash if present */
    ثابت حرف *end = ptr;
    אם(end > begin && end[-1] == '/')
      end--;

    result = Curl_urldecode(begin, end - begin, &imap->mailbox, عدم,
                            REJECT_CTRL);
    אם(result)
      החזר result;
  }
  אחרת
    imap->mailbox = عدم;

  /* There can be any number of parameters in the form ";NAME=VALUE" */
  כלעוד(*ptr == ';') {
    حرف *name;
    حرف *value;
    size_t valuelen;

    /* Find the length of the name parameter */
    begin = ++ptr;
    כלעוד(*ptr && *ptr != '=')
      ptr++;

    אם(!*ptr)
      החזר CURLE_URL_MALFORMAT;

    /* Decode the name parameter */
    result = Curl_urldecode(begin, ptr - begin, &name, عدم,
                            REJECT_CTRL);
    אם(result)
      החזר result;

    /* Find the length of the value parameter */
    begin = ++ptr;
    כלעוד(imap_is_bchar(*ptr))
      ptr++;

    /* Decode the value parameter */
    result = Curl_urldecode(begin, ptr - begin, &value, &valuelen,
                            REJECT_CTRL);
    אם(result) {
      curlx_free(name);
      החזר result;
    }

    DEBUGF(infof(data, "IMAP URL parameter '%s' = '%s'", name, value));

    /* Process the known hierarchical parameters (UIDVALIDITY, UID, SECTION
       and PARTIAL) stripping of the trailing slash character if it is
       present.

       Note: Unknown parameters trigger a URL_MALFORMAT error. */
    אם(valuelen > 0 && value[valuelen - 1] == '/')
      value[valuelen - 1] = '\0';
    אם(valuelen) {
      אם(curl_strequal(name, "UIDVALIDITY") && !imap->uidvalidity_set) {
        curl_off_t num;
        ثابت حرف *p = (ثابت حرف *)value;
        אם(!curlx_str_number(&p, &num, UINT_MAX)) {
          imap->uidvalidity = (unsigned صحيح)num;
          imap->uidvalidity_set = TRUE;
        }
        curlx_free(value);
      }
      אחרת אם(curl_strequal(name, "UID") && !imap->uid) {
        imap->uid = value;
      }
      אחרת אם(curl_strequal(name, "MAILINDEX") && !imap->mindex) {
        imap->mindex = value;
      }
      אחרת אם(curl_strequal(name, "SECTION") && !imap->section) {
        imap->section = value;
      }
      אחרת אם(curl_strequal(name, "PARTIAL") && !imap->partial) {
        imap->partial = value;
      }
      אחרת {
        curlx_free(name);
        curlx_free(value);
        החזר CURLE_URL_MALFORMAT;
      }
    }
    אחרת
      /* blank? */
      curlx_free(value);
    curlx_free(name);
  }

  /* Does the URL contain a query parameter? Only valid when we have a mailbox
     and no UID as per RFC-5092 */
  אם(imap->mailbox && !imap->uid && !imap->mindex) {
    /* Get the query parameter, URL decoded */
    CURLUcode uc = curl_url_get(data->state.uh, CURLUPART_QUERY, &imap->query,
                                CURLU_URLDECODE);
    אם(uc == CURLUE_OUT_OF_MEMORY)
      החזר CURLE_OUT_OF_MEMORY;
  }

  /* Any extra stuff at the end of the URL is an error */
  אם(*ptr)
    החזר CURLE_URL_MALFORMAT;

  החזר CURLE_OK;
}

/***********************************************************************
 *
 * imap_parse_custom_request()
 *
 * Parse the custom request.
 */
ساكن CURLcode imap_parse_custom_request(بنية Curl_easy *data,
                                          بنية IMAP *imap)
{
  CURLcode result = CURLE_OK;
  ثابت حرف *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  אם(custom) {
    /* URL decode the custom request */
    result = Curl_urldecode(custom, 0, &imap->custom, عدم, REJECT_CTRL);

    /* Extract the parameters if specified */
    אם(!result) {
      ثابت حرف *params = imap->custom;

      כלעוד(*params && *params != ' ')
        params++;

      אם(*params) {
        imap->custom_params = curlx_strdup(params);
        imap->custom[params - imap->custom] = '\0';

        אם(!imap->custom_params)
          result = CURLE_OUT_OF_MEMORY;
      }
    }
  }

  החזר result;
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
ساكن CURLcode imap_connect(بنية Curl_easy *data, bool *done)
{
  بنية imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  CURLcode result = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  אם(!imapc)
    החזר CURLE_FAILED_INIT;

  /* Parse the URL options */
  result = imap_parse_url_options(data->conn, imapc);
  אם(result)
    החזר result;

  /* Start off waiting for the server greeting response */
  imap_state(data, imapc, IMAP_SERVERGREET);

  /* Start off with an response id of '*' */
  curlx_strcopy(imapc->resptag, حجم(imapc->resptag), STRCONST("*"));

  result = imap_multi_statemach(data, done);

  החזר result;
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
ساكن CURLcode imap_done(بنية Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  بنية connectdata *conn = data->conn;
  بنية imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  بنية IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);

  אם(!imapc)
    החזר CURLE_FAILED_INIT;
  אם(!imap)
    החזר CURLE_OK;

  אם(status || premature) {
    CURL_TRC_M(data, "IMAP done, not finalizing transfer");
    connclose(conn); /* marked for closure */
    result = status;         /* use the already set error code */
  }
  אחרת אם(!data->set.connect_only &&
          ((!imap->custom && (imap->uid || imap->mindex)) ||
           (imap->custom && data->req.maxdownload > 0) ||
           data->state.upload || IS_MIME_POST(data))) {
    /* Handle responses after FETCH or APPEND transfer has finished.
       For custom commands, check if we set up a download which indicates
       a FETCH-like command with literal data. */

    אם(!data->state.upload && !IS_MIME_POST(data))
      imap_state(data, imapc, IMAP_FETCH_FINAL);
    אחרת {
      /* End the APPEND command first by sending an empty line */
      result = Curl_pp_sendf(data, &imapc->pp, "%s", "");
      אם(!result)
        imap_state(data, imapc, IMAP_APPEND_FINAL);
    }

    /* Run the state-machine */
    אם(!result)
      result = imap_block_statemach(data, imapc, FALSE);
  }

  imap_easy_reset(imap);
  החזר result;
}

/***********************************************************************
 *
 * imap_perform()
 *
 * This is the actual DO function for IMAP. Fetch or append a message, or do
 * other things according to the options previously setup.
 */
ساكن CURLcode imap_perform(بنية Curl_easy *data, bool *connected,
                             bool *dophase_done)
{
  /* This is IMAP and no proxy */
  CURLcode result = CURLE_OK;
  بنية connectdata *conn = data->conn;
  بنية imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  بنية IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  bool selected = FALSE;

  DEBUGF(infof(data, "DO phase starts"));
  אם(!imapc || !imap)
    החזר CURLE_FAILED_INIT;

  אם(data->req.no_body) {
    /* Requested no body means no transfer */
    imap->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* Determine if the requested mailbox (with the same UIDVALIDITY if set)
     has already been selected on this connection */
  אם(imap->mailbox && imapc->mailbox &&
     (!strcmp(imap->mailbox, imapc->mailbox) ||
      (curl_strequal(imap->mailbox, "INBOX") &&
       curl_strequal(imapc->mailbox, "INBOX"))) &&
     (!imap->uidvalidity_set || !imapc->mb_uidvalidity_set ||
      (imap->uidvalidity == imapc->mb_uidvalidity)))
    selected = TRUE;

  /* Start the first command in the DO phase */
  אם(data->state.upload || IS_MIME_POST(data))
    /* APPEND can be executed directly */
    result = imap_perform_append(data, imapc, imap);
  אחרת אם(imap->custom && (selected || !imap->mailbox))
    /* Custom command using the same mailbox or no mailbox */
    result = imap_perform_list(data, imapc, imap);
  אחרת אם(!imap->custom && selected && (imap->uid || imap->mindex))
    /* FETCH from the same mailbox */
    result = imap_perform_fetch(data, imapc, imap);
  אחרת אם(!imap->custom && selected && imap->query)
    /* SEARCH the current mailbox */
    result = imap_perform_search(data, imapc, imap);
  אחרת אם(imap->mailbox && !selected &&
          (imap->custom || imap->uid || imap->mindex || imap->query))
    /* SELECT the mailbox */
    result = imap_perform_select(data, imapc, imap);
  אחרת
    /* LIST */
    result = imap_perform_list(data, imapc, imap);

  אם(result)
    החזר result;

  /* Run the state-machine */
  result = imap_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(conn, FIRSTSOCKET);

  אם(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  החזר result;
}

/* Call this when the DO phase has completed */
ساكن CURLcode imap_dophase_done(بنية Curl_easy *data,
                                  بنية IMAP *imap,
                                  bool connected)
{
  (فراغ)connected;

  אם(imap->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);

  החזר CURLE_OK;
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
ساكن CURLcode imap_regular_transfer(بنية Curl_easy *data,
                                      بنية IMAP *imap,
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
  אם(!result && *dophase_done)
    result = imap_dophase_done(data, imap, connected);

  החזר result;
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
ساكن CURLcode imap_do(بنية Curl_easy *data, bool *done)
{
  بنية IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* default to false */

  אם(!imap)
    החזר CURLE_FAILED_INIT;
  /* Parse the URL path */
  result = imap_parse_url_path(data, imap);
  אם(result)
    החזר result;

  /* Parse the custom request */
  result = imap_parse_custom_request(data, imap);
  אם(result)
    החזר result;

  result = imap_regular_transfer(data, imap, done);

  החזר result;
}

/***********************************************************************
 *
 * imap_disconnect()
 *
 * Disconnect from an IMAP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ساكن CURLcode imap_disconnect(بنية Curl_easy *data,
                                بنية connectdata *conn, bool dead_connection)
{
  بنية imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);

  אם(imapc &&
     /* We cannot send quit unconditionally. If this connection is stale or
        bad in any way (pingpong has pending data to send),
        sending quit and waiting around here will make the
        disconnect wait in vain and cause more problems than we need to. */
     !dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &imapc->pp) &&
     !imap_perform_logout(data, imapc))
    (فراغ)imap_block_statemach(data, imapc, TRUE); /* ignore errors */

  החזר CURLE_OK;
}

/* Called from multi.c while DOing */
ساكن CURLcode imap_doing(بنية Curl_easy *data, bool *dophase_done)
{
  بنية IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  CURLcode result;

  אם(!imap)
    החזר CURLE_FAILED_INIT;

  result = imap_multi_statemach(data, dophase_done);
  אם(result)
    DEBUGF(infof(data, "DO phase failed"));
  אחרת אם(*dophase_done) {
    result = imap_dophase_done(data, imap, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  החזר result;
}

ساكن فراغ imap_easy_dtor(ثابت فراغ *key, size_t klen, فراغ *entry)
{
  بنية IMAP *imap = entry;
  (فراغ)key;
  (فراغ)klen;
  imap_easy_reset(imap);
  curlx_free(imap);
}

ساكن فراغ imap_conn_dtor(ثابت فراغ *key, size_t klen, فراغ *entry)
{
  بنية imap_conn *imapc = entry;
  (فراغ)key;
  (فراغ)klen;
  Curl_pp_disconnect(&imapc->pp);
  curlx_safefree(imapc->mailbox);
  curlx_free(imapc);
}

/* SASL parameters for the imap protocol */
ساكن ثابت بنية SASLproto saslimap = {
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

ساكن CURLcode imap_setup_connection(بنية Curl_easy *data,
                                      بنية connectdata *conn)
{
  بنية imap_conn *imapc;
  بنية pingpong *pp;
  بنية IMAP *imap;

  imapc = curlx_calloc(1, حجم(*imapc));
  אם(!imapc)
    החזר CURLE_OUT_OF_MEMORY;

  pp = &imapc->pp;
  PINGPONG_SETUP(pp, imap_pp_statemachine, imap_endofresp);

  /* Set the default preferred authentication type and mechanism */
  imapc->preftype = IMAP_TYPE_ANY;
  Curl_sasl_init(&imapc->sasl, data, &saslimap);

  Curl_pp_init(pp, Curl_pgrs_now(data));

  אם(Curl_conn_meta_set(conn, CURL_META_IMAP_CONN, imapc, imap_conn_dtor))
    החזר CURLE_OUT_OF_MEMORY;

  imap = curlx_calloc(1, حجم(بنية IMAP));
  אם(!imap ||
     Curl_meta_set(data, CURL_META_IMAP_EASY, imap, imap_easy_dtor))
    החזר CURLE_OUT_OF_MEMORY;

  החזר CURLE_OK;
}

/*
 * IMAP protocol.
 */
ثابت بنية Curl_protocol Curl_protocol_imap = {
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
