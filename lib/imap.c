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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "imap.h"

#ifndef CURL_DISABLE_IMAP

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#endif

#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "pingpong.h"
#अन्तर्भाव "mime.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "curl_sasl.h"
#अन्तर्भाव "curlx/strcopy.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_IMAP_EASY   "meta:proto:imap:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_IMAP_CONN   "meta:proto:imap:conn"

प्रकारनिर्देश गणना {
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
  IMAP_अन्वेषणम्,
  IMAP_LOGOUT,
  IMAP_LAST          /* never used */
} imapstate;

/* imap_conn is used for struct connection-oriented data */
संरचना imap_conn {
  संरचना pingpong pp;
  संरचना SASL sasl;           /* SASL-related parameters */
  वर्ण *mailbox;              /* The last selected mailbox */
  imapstate state;            /* Always use imap.c:state() to change state! */
  unsigned पूर्णाङ्क mb_uidvalidity; /* UIDVALIDITY parsed from select response */
  वर्ण resptag[5];            /* Response tag to wait for */
  unsigned वर्ण preftype;     /* Preferred authentication type */
  unsigned वर्ण cmdid;        /* Last used command ID */
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
संरचना IMAP {
  curl_pp_transfer transfer;
  वर्ण *mailbox;          /* Mailbox to select */
  वर्ण *uid;              /* Message UID to fetch */
  वर्ण *mindex;           /* Index in mail box of mail to fetch */
  वर्ण *section;          /* Message SECTION to fetch */
  वर्ण *partial;          /* Message PARTIAL to fetch */
  वर्ण *query;            /* Query to search for */
  वर्ण *custom;           /* Custom request */
  वर्ण *custom_params;    /* Parameters for the custom request */
  unsigned पूर्णाङ्क uidvalidity; /* UIDVALIDITY to check in select */
  BIT(uidvalidity_set);
};

#define IMAP_RESP_OK       1
#define IMAP_RESP_NOT_OK   2
#define IMAP_RESP_PREAUTH  3

संरचना ulbits {
  पूर्णाङ्क bit;
  ध्रुव वर्ण *flag;
};

/***********************************************************************
 *
 * imap_sendf()
 *
 * Sends the formatted string as an IMAP command to the server.
 *
 * Designed to never block.
 */
स्थिर CURLcode imap_sendf(संरचना Curl_easy *data,
                           संरचना imap_conn *imapc,
                           ध्रुव वर्ण *fmt, ...) CURL_PRINTF(3, 4);
स्थिर CURLcode imap_sendf(संरचना Curl_easy *data,
                           संरचना imap_conn *imapc,
                           ध्रुव वर्ण *fmt, ...)
{
  CURLcode result = CURLE_OK;
  va_list ap;

  DEBUGASSERT(fmt);

  /* Calculate the tag based on the connection ID and command ID */
  curl_msnprintf(imapc->resptag, परिमाण(imapc->resptag), "%c%03d",
                 'A' + curlx_sltosi((long)(data->conn->connection_id % 26)),
                 ++imapc->cmdid);

  va_start(ap, fmt);
  result = Curl_pp_vsendf(data, &imapc->pp, imapc->resptag, fmt, ap);
  va_end(ap);
  प्रत्यागम result;
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
स्थिर वर्ण *imap_atom(ध्रुव वर्ण *str, bool escape_only)
{
  संरचना dynbuf पङ्क्तिः;
  size_t nclean;
  size_t len;

  जइ(!str)
    प्रत्यागम रिक्त;

  len = strlen(str);
  nclean = strcspn(str, "() {%*]\\\"");
  जइ(len == nclean)
    /* nothing to escape, return a strdup */
    प्रत्यागम curlx_strdup(str);

  curlx_dyn_init(&पङ्क्तिः, 2000);

  जइ(!escape_only && curlx_dyn_addn(&पङ्क्तिः, "\"", 1))
    प्रत्यागम रिक्त;

  जाव(*str) {
    जइ((*str == '\\' || *str == '"') &&
       curlx_dyn_addn(&पङ्क्तिः, "\\", 1))
      प्रत्यागम रिक्त;
    जइ(curlx_dyn_addn(&पङ्क्तिः, str, 1))
      प्रत्यागम रिक्त;
    str++;
  }

  जइ(!escape_only && curlx_dyn_addn(&पङ्क्तिः, "\"", 1))
    प्रत्यागम रिक्त;

  प्रत्यागम curlx_dyn_ptr(&पङ्क्तिः);
}

/*
 * Finds the start of a literal '{size}' in line, skipping over quoted strings.
 */
स्थिर ध्रुव वर्ण *imap_find_literal(ध्रुव वर्ण *पङ्क्तिः, size_t len)
{
  ध्रुव वर्ण *end = पङ्क्तिः + len;
  bool in_quote = FALSE;

  जाव(पङ्क्तिः < end) {
    जइ(in_quote) {
      जइ(*पङ्क्तिः == '\\' && (पङ्क्तिः + 1) < end) {
        पङ्क्तिः += 2;
        अनुवर्त;
      }
      जइ(*पङ्क्तिः == '"')
        in_quote = FALSE;
    }
    अण्णहा {
      जइ(*पङ्क्तिः == '"')
        in_quote = TRUE;
      अण्णहा जइ(*पङ्क्तिः == '{')
        प्रत्यागम पङ्क्तिः;
    }
    पङ्क्तिः++;
  }
  प्रत्यागम रिक्त;
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
स्थिर bool imap_matchresp(ध्रुव वर्ण *पङ्क्तिः, size_t len, ध्रुव वर्ण *cmd)
{
  ध्रुव वर्ण *end = पङ्क्तिः + len;
  size_t cmd_len = strlen(cmd);

  /* Skip the untagged response marker */
  पङ्क्तिः += 2;

  /* Do we have a number after the marker? */
  जइ(पङ्क्तिः < end && ISDIGIT(*पङ्क्तिः)) {
    /* Skip the number */
    करेहि
      पङ्क्तिः++;
    जाव(पङ्क्तिः < end && ISDIGIT(*पङ्क्तिः));

    /* Do we have the space character? */
    जइ(पङ्क्तिः == end || *पङ्क्तिः != ' ')
      प्रत्यागम FALSE;

    पङ्क्तिः++;
  }

  /* Does the command name match and is it followed by a space character or at
     the end of line? */
  जइ(पङ्क्तिः + cmd_len <= end && curl_strnequal(पङ्क्तिः, cmd, cmd_len) &&
     (पङ्क्तिः[cmd_len] == ' ' || पङ्क्तिः + cmd_len + 2 == end))
    प्रत्यागम TRUE;

  प्रत्यागम FALSE;
}

/***********************************************************************
 *
 * imap_endofresp()
 *
 * Checks whether the given string is a valid tagged, untagged or continuation
 * response which can be processed by the response handler.
 */
स्थिर bool imap_endofresp(संरचना Curl_easy *data, संरचना connectdata *conn,
                           ध्रुव वर्ण *पङ्क्तिः, size_t len, पूर्णाङ्क *resp)
{
  संरचना imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  संरचना IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  ध्रुव वर्ण *id;
  size_t id_len;

  DEBUGASSERT(imapc);
  DEBUGASSERT(imap);
  जइ(!imapc || !imap)
    प्रत्यागम FALSE;

  /* Do we have a tagged command response? */
  id = imapc->resptag;
  id_len = strlen(id);
  जइ(len >= id_len + 1 && !memcmp(id, पङ्क्तिः, id_len) && पङ्क्तिः[id_len] == ' ') {
    पङ्क्तिः += id_len + 1;
    len -= id_len + 1;

    जइ(len >= 2 && !memcmp(पङ्क्तिः, "OK", 2))
      *resp = IMAP_RESP_OK;
    अण्णहा जइ(len >= 7 && !memcmp(पङ्क्तिः, "PREAUTH", 7))
      *resp = IMAP_RESP_PREAUTH;
    अण्णहा
      *resp = IMAP_RESP_NOT_OK;

    प्रत्यागम TRUE;
  }

  /* Do we have an untagged command response? */
  जइ(len >= 2 && !memcmp("* ", पङ्क्तिः, 2)) {
    विकल्प(imapc->state) {
    /* States which are interested in untagged responses */
    पक्ष IMAP_CAPABILITY:
      जइ(!imap_matchresp(पङ्क्तिः, len, "CAPABILITY"))
        प्रत्यागम FALSE;
      भङ्ग;

    पक्ष IMAP_LIST:
      जइ((!imap->custom && !imap_matchresp(पङ्क्तिः, len, "LIST")) ||
         (imap->custom && !imap_matchresp(पङ्क्तिः, len, imap->custom) &&
          (!curl_strequal(imap->custom, "STORE") ||
           !imap_matchresp(पङ्क्तिः, len, "FETCH")) &&
          !curl_strequal(imap->custom, "SELECT") &&
          !curl_strequal(imap->custom, "EXAMINE") &&
          !curl_strequal(imap->custom, "SEARCH") &&
          !curl_strequal(imap->custom, "EXPUNGE") &&
          !curl_strequal(imap->custom, "LSUB") &&
          !curl_strequal(imap->custom, "UID") &&
          !curl_strequal(imap->custom, "GETQUOTAROOT") &&
          !curl_strequal(imap->custom, "NOOP")))
        प्रत्यागम FALSE;
      भङ्ग;

    पक्ष IMAP_SELECT:
      /* SELECT is special in that its untagged responses do not have a
         common prefix so accept anything! */
      भङ्ग;

    पक्ष IMAP_FETCH:
      जइ(!imap_matchresp(पङ्क्तिः, len, "FETCH"))
        प्रत्यागम FALSE;
      भङ्ग;

    पक्ष IMAP_अन्वेषणम्:
      जइ(!imap_matchresp(पङ्क्तिः, len, "SEARCH"))
        प्रत्यागम FALSE;
      भङ्ग;

    /* Ignore other untagged responses */
    स्वभाव:
      प्रत्यागम FALSE;
    }

    *resp = '*';
    प्रत्यागम TRUE;
  }

  /* Do we have a continuation response? This should be a + symbol followed by
     a space and optionally some text as per RFC-3501 for the AUTHENTICATE and
     APPEND commands and as outlined in Section 4. Examples of RFC-4959 but
     some email servers ignore this and only send a single + instead. */
  जइ(!imap->custom && ((len == 3 && पङ्क्तिः[0] == '+') ||
                       (len >= 2 && !memcmp("+ ", पङ्क्तिः, 2)))) {
    विकल्प(imapc->state) {
    /* States which are interested in continuation responses */
    पक्ष IMAP_AUTHENTICATE:
    पक्ष IMAP_APPEND:
      *resp = '+';
      भङ्ग;

    स्वभाव:
      failf(data, "Unexpected continuation response");
      *resp = -1;
      भङ्ग;
    }

    प्रत्यागम TRUE;
  }

  प्रत्यागम FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * imap_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
स्थिर CURLcode imap_get_message(संरचना Curl_easy *data, संरचना bufref *out)
{
  संरचना imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  वर्ण *message;
  size_t len;

  जइ(!imapc)
    प्रत्यागम CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&imapc->pp.recvbuf);
  len = imapc->pp.nfinal;
  जइ(len > 2) {
    /* Find the start of the message */
    len -= 2;
    प्रति(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    जाव(len--)
      जइ(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        भङ्ग;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, रिक्त);
  }
  अण्णहा
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, रिक्त);

  प्रत्यागम CURLE_OK;
}

/***********************************************************************
 *
 * imap_state()
 *
 * This is the ONLY way to change IMAP state!
 */
स्थिर शून्य imap_state(संरचना Curl_easy *data,
                       संरचना imap_conn *imapc,
                       imapstate newstate)
{
#जइ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  स्थिर ध्रुव वर्ण * ध्रुव names[] = {
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

  जइ(imapc->state != newstate)
    infof(data, "IMAP %p state change from %s to %s",
          (शून्य *)imapc, names[imapc->state], names[newstate]);
#अण्णहा
  (शून्य)data;
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
स्थिर CURLcode imap_perform_capability(संरचना Curl_easy *data,
                                        संरचना imap_conn *imapc)
{
  CURLcode result = CURLE_OK;

  imapc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  imapc->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  imapc->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPABILITY command */
  result = imap_sendf(data, imapc, "CAPABILITY");

  जइ(!result)
    imap_state(data, imapc, IMAP_CAPABILITY);

  प्रत्यागम result;
}

/***********************************************************************
 *
 * imap_perform_starttls()
 *
 * Sends the STARTTLS command to start the upgrade to TLS.
 */
स्थिर CURLcode imap_perform_starttls(संरचना Curl_easy *data,
                                      संरचना imap_conn *imapc)
{
  /* Send the STARTTLS command */
  CURLcode result = imap_sendf(data, imapc, "STARTTLS");

  जइ(!result)
    imap_state(data, imapc, IMAP_STARTTLS);

  प्रत्यागम result;
}

/***********************************************************************
 *
 * imap_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
स्थिर CURLcode imap_perform_upgrade_tls(संरचना Curl_easy *data,
                                         संरचना imap_conn *imapc,
                                         संरचना connectdata *conn)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  CURLcode result;
  bool ssldone = FALSE;

  जइ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    जइ(result)
      गच्छ out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_imaps;
  }

  DEBUGASSERT(!imapc->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "imap_perform_upgrade_tls, connect -> %d, %d",
               (पूर्णाङ्क)result, ssldone));
  जइ(!result && ssldone) {
    imapc->ssldone = ssldone;
    /* perform CAPA now, changes imapc->state out of IMAP_UPGRADETLS */
    result = imap_perform_capability(data, imapc);
  }
out:
  प्रत्यागम result;
#अण्णहा
  (शून्य)data;
  (शून्य)imapc;
  (शून्य)conn;
  प्रत्यागम CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * imap_perform_login()
 *
 * Sends a clear text LOGIN command to authenticate with.
 */
स्थिर CURLcode imap_perform_login(संरचना Curl_easy *data,
                                   संरचना imap_conn *imapc,
                                   संरचना connectdata *conn)
{
  CURLcode result = CURLE_OK;
  वर्ण *user;
  वर्ण *passwd;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  जइ(!conn->creds) {
    imap_state(data, imapc, IMAP_STOP);

    प्रत्यागम result;
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

  जइ(!result)
    imap_state(data, imapc, IMAP_LOGIN);

  प्रत्यागम result;
}

/***********************************************************************
 *
 * imap_perform_authenticate()
 *
 * Sends an AUTHENTICATE command allowing the client to login with the given
 * SASL authentication mechanism.
 */
स्थिर CURLcode imap_perform_authenticate(संरचना Curl_easy *data,
                                          ध्रुव वर्ण *mech,
                                          ध्रुव संरचना bufref *initresp)
{
  संरचना imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  CURLcode result = CURLE_OK;
  ध्रुव वर्ण *ir = Curl_bufref_ptr(initresp);

  जइ(!imapc)
    प्रत्यागम CURLE_FAILED_INIT;
  जइ(ir) {
    /* Send the AUTHENTICATE command with the initial response */
    result = imap_sendf(data, imapc, "AUTHENTICATE %s %s",
                        mech, *ir ? ir : "=");
  }
  अण्णहा {
    /* Send the AUTHENTICATE command */
    result = imap_sendf(data, imapc, "AUTHENTICATE %s", mech);
  }

  प्रत्यागम result;
}

/***********************************************************************
 *
 * imap_continue_authenticate()
 *
 * Sends SASL continuation data.
 */
स्थिर CURLcode imap_continue_authenticate(संरचना Curl_easy *data,
                                           ध्रुव वर्ण *mech,
                                           ध्रुव संरचना bufref *resp)
{
  संरचना imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  (शून्य)mech;
  जइ(!imapc)
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम Curl_pp_sendf(data, &imapc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * imap_cancel_authenticate()
 *
 * Sends SASL cancellation.
 */
स्थिर CURLcode imap_cancel_authenticate(संरचना Curl_easy *data,
                                         ध्रुव वर्ण *mech)
{
  संरचना imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  (शून्य)mech;
  जइ(!imapc)
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम Curl_pp_sendf(data, &imapc->pp, "*");
}

/***********************************************************************
 *
 * imap_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to clear text should a common
 * mechanism not be available between the client and server.
 */
स्थिर CURLcode imap_perform_authentication(संरचना Curl_easy *data,
                                            संरचना imap_conn *imapc)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  /* Check if already authenticated OR if there is enough data to authenticate
     with and end the connect phase if we do not */
  जइ(imapc->preauth ||
     !Curl_sasl_can_authenticate(&imapc->sasl, data)) {
    imap_state(data, imapc, IMAP_STOP);
    प्रत्यागम result;
  }

  /* Calculate the SASL login details */
  result = Curl_sasl_start(&imapc->sasl, data, (bool)imapc->ir_supported,
                           &progress);
  जइ(!result) {
    जइ(progress == SASL_INPROGRESS)
      imap_state(data, imapc, IMAP_AUTHENTICATE);
    अण्णहा जइ(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
      /* Perform clear text authentication */
      result = imap_perform_login(data, imapc, data->conn);
    अण्णहा
      result = Curl_sasl_is_blocked(&imapc->sasl, data);
  }

  प्रत्यागम result;
}

/***********************************************************************
 *
 * imap_perform_list()
 *
 * Sends a LIST command or an alternative custom request.
 */
स्थिर CURLcode imap_perform_list(संरचना Curl_easy *data,
                                  संरचना imap_conn *imapc,
                                  संरचना IMAP *imap)
{
  CURLcode result = CURLE_OK;

  जइ(imap->custom)
    /* Send the custom request */
    result = imap_sendf(data, imapc, "%s%s", imap->custom,
                        imap->custom_params ? imap->custom_params : "");
  अण्णहा {
    /* Make sure the mailbox is in the correct atom format if necessary */
    वर्ण *mailbox = imap->mailbox ? imap_atom(imap->mailbox, TRUE)
                                  : curlx_strdup("");
    जइ(!mailbox)
      प्रत्यागम CURLE_OUT_OF_MEMORY;

    /* Send the LIST command */
    result = imap_sendf(data, imapc, "LIST \"%s\" *", mailbox);

    curlx_free(mailbox);
  }

  जइ(!result)
    imap_state(data, imapc, IMAP_LIST);

  प्रत्यागम result;
}

/***********************************************************************
 *
 * imap_perform_select()
 *
 * Sends a SELECT command to ask the server to change the selected mailbox.
 */
स्थिर CURLcode imap_perform_select(संरचना Curl_easy *data,
                                    संरचना imap_conn *imapc,
                                    संरचना IMAP *imap)
{
  CURLcode result = CURLE_OK;
  वर्ण *mailbox;

  /* Invalidate old information as we are switching mailboxes */
  curlx_safefree(imapc->mailbox);
  imapc->mb_uidvalidity_set = FALSE;

  /* Check we have a mailbox */
  जइ(!imap->mailbox) {
    failf(data, "Cannot SELECT without a mailbox.");
    प्रत्यागम CURLE_URL_MALFORMAT;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  जइ(!mailbox)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  /* Send the SELECT command */
  result = imap_sendf(data, imapc, "SELECT %s", mailbox);

  curlx_free(mailbox);

  जइ(!result)
    imap_state(data, imapc, IMAP_SELECT);

  प्रत्यागम result;
}

/***********************************************************************
 *
 * imap_perform_fetch()
 *
 * Sends a FETCH command to initiate the download of a message.
 */
स्थिर CURLcode imap_perform_fetch(संरचना Curl_easy *data,
                                   संरचना imap_conn *imapc,
                                   संरचना IMAP *imap)
{
  CURLcode result = CURLE_OK;
  /* Check we have a UID */
  जइ(imap->uid) {

    /* Send the FETCH command */
    जइ(imap->partial)
      result = imap_sendf(data, imapc, "UID FETCH %s BODY[%s]<%s>",
                          imap->uid, imap->section ? imap->section : "",
                          imap->partial);
    अण्णहा
      result = imap_sendf(data, imapc, "UID FETCH %s BODY[%s]",
                          imap->uid, imap->section ? imap->section : "");
  }
  अण्णहा जइ(imap->mindex) {
    /* Send the FETCH command */
    जइ(imap->partial)
      result = imap_sendf(data, imapc, "FETCH %s BODY[%s]<%s>",
                          imap->mindex, imap->section ? imap->section : "",
                          imap->partial);
    अण्णहा
      result = imap_sendf(data, imapc, "FETCH %s BODY[%s]",
                          imap->mindex, imap->section ? imap->section : "");
  }
  अण्णहा {
    failf(data, "Cannot FETCH without a UID.");
    प्रत्यागम CURLE_URL_MALFORMAT;
  }
  जइ(!result)
    imap_state(data, imapc, IMAP_FETCH);

  प्रत्यागम result;
}

/***********************************************************************
 *
 * imap_perform_append()
 *
 * Sends an APPEND command to initiate the upload of a message.
 */
स्थिर CURLcode imap_perform_append(संरचना Curl_easy *data,
                                    संरचना imap_conn *imapc,
                                    संरचना IMAP *imap)
{
  CURLcode result = CURLE_OK;
  वर्ण *mailbox;
  संरचना dynbuf flags;

  /* Check we have a mailbox */
  जइ(!imap->mailbox) {
    failf(data, "Cannot APPEND without a mailbox.");
    प्रत्यागम CURLE_URL_MALFORMAT;
  }

#ifndef CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  जइ(IS_MIME_POST(data)) {
    curl_mimepart *postp = data->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(unsigned पूर्णाङ्क)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, data->set.headers, 0);
    result = Curl_mime_prepare_headers(data, postp, रिक्त,
                                       रिक्त, MIMESTRATEGY_MAIL);

    जइ(!result && !Curl_checkheaders(data, STRCONST("Mime-Version")))
      result = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    जइ(!result)
      result = Curl_creader_set_mime(data, postp);
    जइ(result)
      प्रत्यागम result;
  }
  अण्णहा
#endif
  {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    जइ(result)
      प्रत्यागम result;
  }

  /* Check we know the size of the upload. This takes all readers
   * into account. Especially crlf conversions which make the size
   * unpredictable, e.g. -1. */
  data->state.infilesize = Curl_creader_total_length(data);
  जइ(data->state.infilesize < 0) {
    जइ(data->set.crlf)
      failf(data, "Cannot APPEND with CRLF conversion making size unknown");
    अण्णहा
      failf(data, "Cannot APPEND with unknown input file size");
    प्रत्यागम CURLE_UPLOAD_FAILED;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  जइ(!mailbox)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  /* Generate flags string and send the APPEND command */
  curlx_dyn_init(&flags, 100);
  जइ(data->set.upload_flags) {
    पूर्णाङ्क i;
    संरचना ulbits ulflag[] = {
      { CURLULFLAG_ANSWERED, "Answered" },
      { CURLULFLAG_DELETED, "Deleted" },
      { CURLULFLAG_DRAFT, "Draft" },
      { CURLULFLAG_FLAGGED, "Flagged" },
      { CURLULFLAG_SEEN, "Seen" },
      { 0, रिक्त }
    };

    result = CURLE_OUT_OF_MEMORY;
    जइ(curlx_dyn_add(&flags, " (")) {
      गच्छ cleanup;
    }

    प्रति(i = 0; ulflag[i].bit; i++) {
      जइ(data->set.upload_flags & ulflag[i].bit &&
         ((curlx_dyn_len(&flags) > 2 && curlx_dyn_add(&flags, " ")) ||
          curlx_dyn_add(&flags, "\\") ||
          curlx_dyn_add(&flags, ulflag[i].flag)))
        गच्छ cleanup;
    }

    जइ(curlx_dyn_add(&flags, ")"))
      गच्छ cleanup;
  }
  अण्णहा जइ(curlx_dyn_add(&flags, ""))
    गच्छ cleanup;

  result = imap_sendf(data, imapc, "APPEND %s%s {%" FMT_OFF_T "}",
                      mailbox, curlx_dyn_ptr(&flags), data->state.infilesize);

cleanup:
  curlx_dyn_free(&flags);
  curlx_free(mailbox);

  जइ(!result)
    imap_state(data, imapc, IMAP_APPEND);

  प्रत्यागम result;
}

/***********************************************************************
 *
 * imap_perform_search()
 *
 * Sends a SEARCH command.
 */
स्थिर CURLcode imap_perform_अन्वेषणम्(संरचना Curl_easy *data,
                                    संरचना imap_conn *imapc,
                                    संरचना IMAP *imap)
{
  CURLcode result = CURLE_OK;

  /* Check we have a query string */
  जइ(!imap->query) {
    failf(data, "Cannot SEARCH without a query string.");
    प्रत्यागम CURLE_URL_MALFORMAT;
  }

  /* Send the SEARCH command */
  result = imap_sendf(data, imapc, "SEARCH %s", imap->query);

  जइ(!result)
    imap_state(data, imapc, IMAP_अन्वेषणम्);

  प्रत्यागम result;
}

/***********************************************************************
 *
 * imap_perform_logout()
 *
 * Performs the logout action prior to sclose() being called.
 */
स्थिर CURLcode imap_perform_logout(संरचना Curl_easy *data,
                                    संरचना imap_conn *imapc)
{
  /* Send the LOGOUT command */
  CURLcode result = imap_sendf(data, imapc, "LOGOUT");

  जइ(!result)
    imap_state(data, imapc, IMAP_LOGOUT);

  प्रत्यागम result;
}

/* For the initial server greeting */
स्थिर CURLcode imap_state_servergreet_resp(संरचना Curl_easy *data,
                                            संरचना imap_conn *imapc,
                                            पूर्णाङ्क imapcode,
                                            imapstate instate)
{
  (शून्य)instate;

  जइ(imapcode == IMAP_RESP_PREAUTH) {
    /* PREAUTH */
    imapc->preauth = TRUE;
    infof(data, "PREAUTH connection, already authenticated");
  }
  अण्णहा जइ(imapcode != IMAP_RESP_OK) {
    failf(data, "Got unexpected imap-server response");
    प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
  }

  प्रत्यागम imap_perform_capability(data, imapc);
}

/* For CAPABILITY responses */
स्थिर CURLcode imap_state_capability_resp(संरचना Curl_easy *data,
                                           संरचना imap_conn *imapc,
                                           पूर्णाङ्क imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  ध्रुव वर्ण *पङ्क्तिः = curlx_dyn_ptr(&imapc->pp.recvbuf);

  (शून्य)instate;

  /* Do we have an untagged response? */
  जइ(imapcode == '*') {
    पङ्क्तिः += 2;

    /* Loop through the data line */
    प्रति(;;) {
      size_t wordlen;
      जाव(*पङ्क्तिः && (ISBLANK(*पङ्क्तिः) || ISNEWLINE(*पङ्क्तिः)))
        पङ्क्तिः++;

      जइ(!*पङ्क्तिः)
        भङ्ग;

      /* Extract the word */
      प्रति(wordlen = 0; पङ्क्तिः[wordlen] && !ISBLANK(पङ्क्तिः[wordlen]) &&
                       !ISNEWLINE(पङ्क्तिः[wordlen]);)
        wordlen++;

      /* Does the server support the STARTTLS capability? */
      जइ(wordlen == 8 && curl_strnequal(पङ्क्तिः, "STARTTLS", 8))
        imapc->tls_supported = TRUE;

      /* Has the server explicitly disabled clear text authentication? */
      अण्णहा जइ(wordlen == 13 && curl_strnequal(पङ्क्तिः, "LOGINDISABLED", 13))
        imapc->login_disabled = TRUE;

      /* Does the server support the SASL-IR capability? */
      अण्णहा जइ(wordlen == 7 && curl_strnequal(पङ्क्तिः, "SASL-IR", 7))
        imapc->ir_supported = TRUE;

      /* Do we have a SASL based authentication mechanism? */
      अण्णहा जइ(wordlen > 5 && curl_strnequal(पङ्क्तिः, "AUTH=", 5)) {
        size_t llen;
        unsigned short mechbit;

        पङ्क्तिः += 5;
        wordlen -= 5;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(पङ्क्तिः, wordlen, &llen);
        जइ(mechbit && llen == wordlen)
          imapc->sasl.authmechs |= mechbit;
      }

      पङ्क्तिः += wordlen;
    }
  }
  अण्णहा जइ(data->set.use_ssl && !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
    /* PREAUTH is not compatible with STARTTLS. */
    जइ(imapcode == IMAP_RESP_OK && imapc->tls_supported && !imapc->preauth) {
      /* Switch to TLS connection now */
      result = imap_perform_starttls(data, imapc);
    }
    अण्णहा जइ(data->set.use_ssl <= CURLUSESSL_TRY)
      result = imap_perform_authentication(data, imapc);
    अण्णहा {
      failf(data, "STARTTLS not available.");
      result = CURLE_USE_SSL_FAILED;
    }
  }
  अण्णहा
    result = imap_perform_authentication(data, imapc);

  प्रत्यागम result;
}

/* For STARTTLS responses */
स्थिर CURLcode imap_state_starttls_resp(संरचना Curl_easy *data,
                                         संरचना imap_conn *imapc,
                                         पूर्णाङ्क imapcode,
                                         imapstate instate)
{
  CURLcode result = CURLE_OK;

  (शून्य)instate;

  /* Pipelining in response is forbidden. */
  जइ(imapc->pp.overflow)
    प्रत्यागम CURLE_WEIRD_SERVER_REPLY;

  जइ(imapcode != IMAP_RESP_OK) {
    जइ(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied");
      result = CURLE_USE_SSL_FAILED;
    }
    अण्णहा
      result = imap_perform_authentication(data, imapc);
  }
  अण्णहा
    imap_state(data, imapc, IMAP_UPGRADETLS);

  प्रत्यागम result;
}

/* For SASL authentication responses */
स्थिर CURLcode imap_state_auth_resp(संरचना Curl_easy *data,
                                     संरचना imap_conn *imapc,
                                     पूर्णाङ्क imapcode,
                                     imapstate instate)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  (शून्य)instate;

  result = Curl_sasl_continue(&imapc->sasl, data, imapcode, &progress);
  जइ(!result)
    विकल्प(progress) {
    पक्ष SASL_DONE:
      imap_state(data, imapc, IMAP_STOP);  /* Authenticated */
      भङ्ग;
    पक्ष SASL_IDLE:            /* No mechanism left after cancellation */
      जइ(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
        /* Perform clear text authentication */
        result = imap_perform_login(data, imapc, data->conn);
      अण्णहा {
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
      }
      भङ्ग;
    स्वभाव:
      भङ्ग;
    }

  प्रत्यागम result;
}

/* For LOGIN responses */
स्थिर CURLcode imap_state_login_resp(संरचना Curl_easy *data,
                                      संरचना imap_conn *imapc,
                                      पूर्णाङ्क imapcode,
                                      imapstate instate)
{
  CURLcode result = CURLE_OK;
  (शून्य)instate;

  जइ(imapcode != IMAP_RESP_OK) {
    failf(data, "Access denied. %c", imapcode);
    result = CURLE_LOGIN_DENIED;
  }
  अण्णहा
    /* End of connect phase */
    imap_state(data, imapc, IMAP_STOP);

  प्रत्यागम result;
}

/* Detect IMAP listings vs. downloading a single email */
स्थिर bool is_custom_fetch_listing_match(ध्रुव वर्ण *params)
{
  /* match " 1:* (FLAGS ..." or " 1,2,3 (FLAGS ..." */
  जइ(*params++ != ' ')
    प्रत्यागम FALSE;

  जाव(ISDIGIT(*params)) {
    params++;
    जइ(*params == 0)
      प्रत्यागम FALSE;
  }
  जइ(*params == ':')
    प्रत्यागम TRUE;
  जइ(*params == ',')
    प्रत्यागम TRUE;
  प्रत्यागम FALSE;
}

स्थिर bool is_custom_fetch_listing(संरचना IMAP *imap)
{
  /* filter out "UID FETCH 1:* (FLAGS ..." queries to list emails */
  जइ(!imap->custom)
    प्रत्यागम FALSE;
  अण्णहा जइ(curl_strequal(imap->custom, "FETCH") && imap->custom_params) {
    ध्रुव वर्ण *p = imap->custom_params;
    प्रत्यागम is_custom_fetch_listing_match(p);
  }
  अण्णहा जइ(curl_strequal(imap->custom, "UID") && imap->custom_params &&
          curl_strnequal(imap->custom_params, " FETCH ", 7)) {
    ध्रुव वर्ण *p = imap->custom_params + 6;
    प्रत्यागम is_custom_fetch_listing_match(p);
  }
  प्रत्यागम FALSE;
}

/* For LIST and SEARCH responses */
स्थिर CURLcode imap_state_listsearch_resp(संरचना Curl_easy *data,
                                           संरचना imap_conn *imapc,
                                           पूर्णाङ्क imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  ध्रुव वर्ण *पङ्क्तिः = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  संरचना IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);

  DEBUGASSERT(imap);
  जइ(!imap)
    प्रत्यागम CURLE_FAILED_INIT;
  (शून्य)instate;

  जइ(imapcode == '*' && is_custom_fetch_listing(imap)) {
    /* custom FETCH or UID FETCH for listing is not handled here */
  }
  अण्णहा जइ(imapcode == '*') {
    /* Check if this response contains a literal (e.g. FETCH responses with
       body data). Literal syntax is {size}\r\n */
    ध्रुव वर्ण *cr = memchr(पङ्क्तिः, '\r', len);
    size_t पङ्क्तिः_len = cr ? (size_t)(cr - पङ्क्तिः) : len;
    ध्रुव वर्ण *ptr = imap_find_literal(पङ्क्तिः, पङ्क्तिः_len);
    जइ(ptr) {
      curl_off_t size = 0;
      bool parsed = FALSE;
      ptr++;
      जइ(!curlx_str_सङ्ख्या(&ptr, &size, CURL_OFF_T_MAX) &&
         !curlx_str_single(&ptr, '}'))
        parsed = TRUE;

      जइ(parsed) {
        संरचना pingpong *pp = &imapc->pp;
        size_t buffer_len = curlx_dyn_len(&pp->recvbuf);
        size_t after_header = buffer_len - pp->nfinal;

        /* This is a literal response, setup to receive the body data */
        infof(data, "Found %" FMT_OFF_T " bytes to download", size);

        /* First write the header line */
        result = Curl_client_write(data, CLIENTWRITE_BODY, पङ्क्तिः, len);
        जइ(result)
          प्रत्यागम result;

        /* Handle data already in buffer after the header line */
        जइ(after_header > 0) {
          /* There is already data in the buffer that is part of the literal
             body or subsequent responses */
          size_t chunk = after_header;

          /* Keep only the data after the header line */
          curlx_dyn_tail(&pp->recvbuf, chunk);
          pp->nfinal = 0; /* done */

          /* Limit chunk to the literal size */
          जइ((curl_off_t)chunk > size)
            chunk = (size_t)size;

          जइ(chunk) {
            /* Write the literal body data */
            result = Curl_client_write(data, CLIENTWRITE_BODY,
                                       curlx_dyn_ptr(&pp->recvbuf), chunk);
            जइ(result)
              प्रत्यागम result;
          }

          /* Handle remaining data in buffer (either more literal data or
             subsequent responses) */
          जइ(after_header > chunk) {
            /* Keep the data after the literal body */
            pp->overflow = after_header - chunk;
            curlx_dyn_tail(&pp->recvbuf, pp->overflow);
          }
          अण्णहा {
            pp->overflow = 0;
            curlx_dyn_reset(&pp->recvbuf);
          }
        }
        अण्णहा {
          /* No data in buffer yet, reset overflow */
          pp->overflow = 0;
        }

        जइ((CURL_OFF_T_MAX - size) < (curl_off_t)len)
          /* unlikely to actually be a transfer this big, but avoid integer
             overflow */
          size = CURL_OFF_T_MAX;
        अण्णहा
          size += len;

        /* Progress size includes both header line and literal body */
        Curl_pgrsSetDownloadSize(data, size);

        जइ(data->req.bytecount == size)
          /* All data already transferred (header + literal body) */
          Curl_xfer_setup_nop(data);
        अण्णहा {
          /* Setup to receive the literal body data.
             maxdownload and transfer size include both header line and
             literal body */
          data->req.maxdownload = size;
          Curl_xfer_setup_recv(data, FIRSTSOCKET, size);
        }
        /* End of DO phase */
        imap_state(data, imapc, IMAP_STOP);
      }
      अण्णहा {
        /* Failed to parse literal, write the line */
        result = Curl_client_write(data, CLIENTWRITE_BODY, पङ्क्तिः, len);
      }
    }
    अण्णहा {
      /* No literal, write the line as-is */
      result = Curl_client_write(data, CLIENTWRITE_BODY, पङ्क्तिः, len);
    }
  }
  अण्णहा जइ(imapcode != IMAP_RESP_OK)
    result = CURLE_QUOTE_ERROR;
  अण्णहा
    /* End of DO phase */
    imap_state(data, imapc, IMAP_STOP);

  प्रत्यागम result;
}

/* For SELECT responses */
स्थिर CURLcode imap_state_select_resp(संरचना Curl_easy *data,
                                       संरचना imap_conn *imapc,
                                       संरचना IMAP *imap,
                                       पूर्णाङ्क imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  (शून्य)instate;

  जइ(imapcode == '*') {
    /* See if this is an UIDVALIDITY response */
    ध्रुव वर्ण *पङ्क्तिः = curlx_dyn_ptr(&imapc->pp.recvbuf);
    size_t len = curlx_dyn_len(&imapc->pp.recvbuf);
    जइ((len >= 18) && checkprefix("OK [UIDVALIDITY ", &पङ्क्तिः[2])) {
      curl_off_t मानम्;
      ध्रुव वर्ण *p = &पङ्क्तिः[2] + CURL_CSTRLEN("OK [UIDVALIDITY ");
      जइ(!curlx_str_सङ्ख्या(&p, &मानम्, UINT_MAX)) {
        imapc->mb_uidvalidity = (unsigned पूर्णाङ्क)मानम्;
        imapc->mb_uidvalidity_set = TRUE;
      }
    }
  }
  अण्णहा जइ(imapcode == IMAP_RESP_OK) {
    /* Check if the UIDVALIDITY has been specified and matches */
    जइ(imap->uidvalidity_set && imapc->mb_uidvalidity_set &&
       (imap->uidvalidity != imapc->mb_uidvalidity)) {
      failf(data, "Mailbox UIDVALIDITY has changed");
      result = CURLE_REMOTE_FILE_NOT_FOUND;
    }
    अण्णहा {
      /* Note the currently opened mailbox on this connection */
      DEBUGASSERT(!imapc->mailbox);
      imapc->mailbox = curlx_strdup(imap->mailbox);
      जइ(!imapc->mailbox)
        प्रत्यागम CURLE_OUT_OF_MEMORY;

      जइ(imap->custom)
        result = imap_perform_list(data, imapc, imap);
      अण्णहा जइ(imap->query)
        result = imap_perform_अन्वेषणम्(data, imapc, imap);
      अण्णहा
        result = imap_perform_fetch(data, imapc, imap);
    }
  }
  अण्णहा {
    failf(data, "Select failed");
    result = CURLE_LOGIN_DENIED;
  }

  प्रत्यागम result;
}

/* For the (first line of the) FETCH responses */
स्थिर CURLcode imap_state_fetch_resp(संरचना Curl_easy *data,
                                      संरचना imap_conn *imapc,
                                      पूर्णाङ्क imapcode,
                                      imapstate instate)
{
  CURLcode result = CURLE_OK;
  संरचना pingpong *pp = &imapc->pp;
  ध्रुव वर्ण *ptr = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  bool parsed = FALSE;
  curl_off_t size = 0;

  (शून्य)instate;

  जइ(imapcode != '*') {
    Curl_pgrsSetDownloadSize(data, -1);
    imap_state(data, imapc, IMAP_STOP);
    प्रत्यागम CURLE_REMOTE_FILE_NOT_FOUND;
  }

  /* Something like this is received "* 1 FETCH (BODY[TEXT] {2021}\r" so parse
     the continuation data contained within the curly brackets */
  ptr = imap_find_literal(ptr, len);
  जइ(ptr) {
    ptr++;
    जइ(!curlx_str_सङ्ख्या(&ptr, &size, CURL_OFF_T_MAX) &&
       !curlx_str_single(&ptr, '}'))
      parsed = TRUE;
  }

  जइ(parsed) {
    infof(data, "Found %" FMT_OFF_T " bytes to download", size);
    Curl_pgrsSetDownloadSize(data, size);

    जइ(pp->overflow) {
      /* At this point there is a data in the receive buffer that is body
         content, send it as body and then skip it. Do note that there may
         even be additional "headers" after the body. */
      size_t chunk = pp->overflow;

      /* keep only the overflow */
      curlx_dyn_tail(&pp->recvbuf, chunk);
      pp->nfinal = 0; /* done */

      जइ((curl_off_t)chunk > size)
        /* The conversion from curl_off_t to size_t is always fine here */
        chunk = (size_t)size;

      जइ(!chunk) {
        /* no size, we are done with the data */
        imap_state(data, imapc, IMAP_STOP);
        प्रत्यागम CURLE_OK;
      }
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 curlx_dyn_ptr(&pp->recvbuf), chunk);
      जइ(result)
        प्रत्यागम result;

      infof(data, "Written %zu bytes, %" FMT_OFF_T
            " bytes are left for transfer", chunk, (curl_off_t)(size - chunk));

      /* Have we used the entire overflow or part of it?*/
      जइ(pp->overflow > chunk) {
        /* remember the remaining trailing overflow data */
        pp->overflow -= chunk;
        curlx_dyn_tail(&pp->recvbuf, pp->overflow);
      }
      अण्णहा {
        pp->overflow = 0; /* handled */
        /* Free the cache */
        curlx_dyn_reset(&pp->recvbuf);
      }
    }

    जइ(data->req.bytecount == size)
      /* The entire data is already transferred! */
      Curl_xfer_setup_nop(data);
    अण्णहा {
      /* IMAP download */
      data->req.maxdownload = size;
      Curl_xfer_setup_recv(data, FIRSTSOCKET, size);
    }
  }
  अण्णहा {
    /* We do not know how to parse this line */
    failf(data, "Failed to parse FETCH response.");
    result = CURLE_WEIRD_SERVER_REPLY;
  }

  /* End of DO phase */
  imap_state(data, imapc, IMAP_STOP);

  प्रत्यागम result;
}

/* For final FETCH responses performed after the download */
स्थिर CURLcode imap_state_fetch_final_resp(संरचना Curl_easy *data,
                                            संरचना imap_conn *imapc,
                                            पूर्णाङ्क imapcode,
                                            imapstate instate)
{
  CURLcode result = CURLE_OK;

  (शून्य)instate;

  जइ(imapcode != IMAP_RESP_OK)
    result = CURLE_WEIRD_SERVER_REPLY;
  अण्णहा
    /* End of DONE phase */
    imap_state(data, imapc, IMAP_STOP);

  प्रत्यागम result;
}

/* For APPEND responses */
स्थिर CURLcode imap_state_append_resp(संरचना Curl_easy *data,
                                       संरचना imap_conn *imapc,
                                       पूर्णाङ्क imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  (शून्य)instate;

  जइ(imapcode != '+') {
    result = CURLE_UPLOAD_FAILED;
  }
  अण्णहा {
    /* Set the progress upload size */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* IMAP upload */
    Curl_xfer_setup_send(data, FIRSTSOCKET);

    /* End of DO phase */
    imap_state(data, imapc, IMAP_STOP);
  }

  प्रत्यागम result;
}

/* For final APPEND responses performed after the upload */
स्थिर CURLcode imap_state_append_final_resp(संरचना Curl_easy *data,
                                             संरचना imap_conn *imapc,
                                             पूर्णाङ्क imapcode,
                                             imapstate instate)
{
  CURLcode result = CURLE_OK;

  (शून्य)instate;

  जइ(imapcode != IMAP_RESP_OK)
    result = CURLE_UPLOAD_FAILED;
  अण्णहा
    /* End of DONE phase */
    imap_state(data, imapc, IMAP_STOP);

  प्रत्यागम result;
}

स्थिर CURLcode imap_pp_statemachine(संरचना Curl_easy *data,
                                     संरचना connectdata *conn)
{
  CURLcode result = CURLE_OK;
  पूर्णाङ्क imapcode;
  संरचना imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  संरचना IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  संरचना pingpong *pp;
  size_t nread = 0;

  जइ(!imapc || !imap)
    प्रत्यागम CURLE_FAILED_INIT;
  pp = &imapc->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not IMAP */
upgrade_tls:
  जइ(imapc->state == IMAP_UPGRADETLS) {
    result = imap_perform_upgrade_tls(data, imapc, conn);
    जइ(result || (imapc->state == IMAP_UPGRADETLS))
      प्रत्यागम result;
  }

  /* Flush any data that needs to be sent */
  जइ(pp->sendleft)
    प्रत्यागम Curl_pp_flushsend(data, pp);

  करेहि {
    /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, pp, &imapcode, &nread);
    जइ(result)
      प्रत्यागम result;

    /* Was there an error parsing the response line? */
    जइ(imapcode == -1)
      प्रत्यागम CURLE_WEIRD_SERVER_REPLY;

    जइ(!imapcode)
      भङ्ग;

    /* We have now received a full IMAP server response */
    विकल्प(imapc->state) {
    पक्ष IMAP_SERVERGREET:
      result = imap_state_servergreet_resp(data, imapc,
                                           imapcode, imapc->state);
      भङ्ग;

    पक्ष IMAP_CAPABILITY:
      result = imap_state_capability_resp(data, imapc, imapcode, imapc->state);
      भङ्ग;

    पक्ष IMAP_STARTTLS:
      result = imap_state_starttls_resp(data, imapc, imapcode, imapc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      जइ(!result && (imapc->state == IMAP_UPGRADETLS))
        गच्छ upgrade_tls;
      भङ्ग;

    पक्ष IMAP_AUTHENTICATE:
      result = imap_state_auth_resp(data, imapc, imapcode, imapc->state);
      भङ्ग;

    पक्ष IMAP_LOGIN:
      result = imap_state_login_resp(data, imapc, imapcode, imapc->state);
      भङ्ग;

    पक्ष IMAP_LIST:
    पक्ष IMAP_अन्वेषणम्:
      result = imap_state_listsearch_resp(data, imapc, imapcode, imapc->state);
      भङ्ग;

    पक्ष IMAP_SELECT:
      result = imap_state_select_resp(data, imapc, imap,
                                      imapcode, imapc->state);
      भङ्ग;

    पक्ष IMAP_FETCH:
      result = imap_state_fetch_resp(data, imapc, imapcode, imapc->state);
      भङ्ग;

    पक्ष IMAP_FETCH_FINAL:
      result = imap_state_fetch_final_resp(data, imapc,
                                           imapcode, imapc->state);
      भङ्ग;

    पक्ष IMAP_APPEND:
      result = imap_state_append_resp(data, imapc, imapcode, imapc->state);
      भङ्ग;

    पक्ष IMAP_APPEND_FINAL:
      result = imap_state_append_final_resp(data, imapc,
                                            imapcode, imapc->state);
      भङ्ग;

    पक्ष IMAP_LOGOUT:
    स्वभाव:
      /* internal error */
      imap_state(data, imapc, IMAP_STOP);
      भङ्ग;
    }
  } जाव(!result && imapc->state != IMAP_STOP && Curl_pp_moredata(pp));

  प्रत्यागम result;
}

/* Called repeatedly until done from multi.c */
स्थिर CURLcode imap_multi_statemach(संरचना Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  संरचना imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);

  *done = FALSE;
  जइ(!imapc)
    प्रत्यागम CURLE_FAILED_INIT;
  result = Curl_pp_statemach(data, &imapc->pp, FALSE, FALSE);
  *done = (imapc->state == IMAP_STOP);

  प्रत्यागम result;
}

स्थिर CURLcode imap_block_statemach(संरचना Curl_easy *data,
                                     संरचना imap_conn *imapc,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;

  जाव(imapc->state != IMAP_STOP && !result)
    result = Curl_pp_statemach(data, &imapc->pp, TRUE, disconnecting);

  प्रत्यागम result;
}

/* For the IMAP "protocol connect" and "doing" phases only */
स्थिर CURLcode imap_pollset(संरचना Curl_easy *data,
                             संरचना easy_pollset *ps)
{
  संरचना imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  प्रत्यागम imapc ? Curl_pp_pollset(data, &imapc->pp, ps) : CURLE_OK;
}

स्थिर शून्य imap_easy_reset(संरचना IMAP *imap)
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
स्थिर bool imap_is_bchar(वर्ण ch)
{
  /* Performing the alnum check first with macro is faster because of ASCII
     arithmetic */
  प्रत्यागम ch && (ISALNUM(ch) || strchr(":@/&=-._~!$\'()*+,%", ch));
}

/***********************************************************************
 *
 * imap_parse_url_options()
 *
 * Parse the URL login options.
 */
स्थिर CURLcode imap_parse_url_options(संरचना connectdata *conn,
                                       संरचना imap_conn *imapc)
{
  CURLcode result = CURLE_OK;
  ध्रुव वर्ण *ptr = conn->options;
  bool prefer_login = FALSE;

  जाव(!result && ptr && *ptr) {
    ध्रुव वर्ण *कुञ्चिका = ptr;
    ध्रुव वर्ण *मानम्;

    जाव(*ptr && *ptr != '=')
      ptr++;

    मानम् = ptr + 1;

    जाव(*ptr && *ptr != ';')
      ptr++;

    जइ(curl_strnequal(कुञ्चिका, "AUTH=+LOGIN", 11)) {
      /* User prefers plaintext LOGIN over any SASL, including SASL LOGIN */
      prefer_login = TRUE;
      imapc->sasl.prefmech = SASL_AUTH_NONE;
    }
    अण्णहा जइ(curl_strnequal(कुञ्चिका, "AUTH=", 5)) {
      prefer_login = FALSE;
      result = Curl_sasl_parse_url_auth_option(&imapc->sasl,
                                               मानम्, ptr - मानम्);
    }
    अण्णहा {
      prefer_login = FALSE;
      result = CURLE_URL_MALFORMAT;
    }

    जइ(*ptr == ';')
      ptr++;
  }

  जइ(prefer_login)
    imapc->preftype = IMAP_TYPE_CLEARTEXT;
  अण्णहा {
    विकल्प(imapc->sasl.prefmech) {
    पक्ष SASL_AUTH_NONE:
      imapc->preftype = IMAP_TYPE_NONE;
      भङ्ग;
    पक्ष SASL_AUTH_DEFAULT:
      imapc->preftype = IMAP_TYPE_ANY;
      भङ्ग;
    स्वभाव:
      imapc->preftype = IMAP_TYPE_SASL;
      भङ्ग;
    }
  }

  प्रत्यागम result;
}

/***********************************************************************
 *
 * imap_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
स्थिर CURLcode imap_parse_url_path(संरचना Curl_easy *data,
                                    संरचना IMAP *imap)
{
  /* The imap struct is already initialized in imap_connect() */
  CURLcode result = CURLE_OK;
  ध्रुव वर्ण *begin = &data->state.up.path[1]; /* skip leading slash */
  ध्रुव वर्ण *ptr = begin;

  /* See how much of the URL is a valid path and decode it */
  जाव(imap_is_bchar(*ptr))
    ptr++;

  जइ(ptr != begin) {
    /* Remove the trailing slash if present */
    ध्रुव वर्ण *end = ptr;
    जइ(end > begin && end[-1] == '/')
      end--;

    result = Curl_urldecode(begin, end - begin, &imap->mailbox, रिक्त,
                            REJECT_CTRL);
    जइ(result)
      प्रत्यागम result;
  }
  अण्णहा
    imap->mailbox = रिक्त;

  /* There can be any number of parameters in the form ";NAME=VALUE" */
  जाव(*ptr == ';') {
    वर्ण *name;
    वर्ण *मानम्;
    size_t valuelen;

    /* Find the length of the name parameter */
    begin = ++ptr;
    जाव(*ptr && *ptr != '=')
      ptr++;

    जइ(!*ptr)
      प्रत्यागम CURLE_URL_MALFORMAT;

    /* Decode the name parameter */
    result = Curl_urldecode(begin, ptr - begin, &name, रिक्त,
                            REJECT_CTRL);
    जइ(result)
      प्रत्यागम result;

    /* Find the length of the value parameter */
    begin = ++ptr;
    जाव(imap_is_bchar(*ptr))
      ptr++;

    /* Decode the value parameter */
    result = Curl_urldecode(begin, ptr - begin, &मानम्, &valuelen,
                            REJECT_CTRL);
    जइ(result) {
      curlx_free(name);
      प्रत्यागम result;
    }

    DEBUGF(infof(data, "IMAP URL parameter '%s' = '%s'", name, मानम्));

    /* Process the known hierarchical parameters (UIDVALIDITY, UID, SECTION
       and PARTIAL) stripping of the trailing slash character if it is
       present.

       Note: Unknown parameters trigger a URL_MALFORMAT error. */
    जइ(valuelen > 0 && मानम्[valuelen - 1] == '/')
      मानम्[valuelen - 1] = '\0';
    जइ(valuelen) {
      जइ(curl_strequal(name, "UIDVALIDITY") && !imap->uidvalidity_set) {
        curl_off_t अङ्कः;
        ध्रुव वर्ण *p = (ध्रुव वर्ण *)मानम्;
        जइ(!curlx_str_सङ्ख्या(&p, &अङ्कः, UINT_MAX)) {
          imap->uidvalidity = (unsigned पूर्णाङ्क)अङ्कः;
          imap->uidvalidity_set = TRUE;
        }
        curlx_free(मानम्);
      }
      अण्णहा जइ(curl_strequal(name, "UID") && !imap->uid) {
        imap->uid = मानम्;
      }
      अण्णहा जइ(curl_strequal(name, "MAILINDEX") && !imap->mindex) {
        imap->mindex = मानम्;
      }
      अण्णहा जइ(curl_strequal(name, "SECTION") && !imap->section) {
        imap->section = मानम्;
      }
      अण्णहा जइ(curl_strequal(name, "PARTIAL") && !imap->partial) {
        imap->partial = मानम्;
      }
      अण्णहा {
        curlx_free(name);
        curlx_free(मानम्);
        प्रत्यागम CURLE_URL_MALFORMAT;
      }
    }
    अण्णहा
      /* blank? */
      curlx_free(मानम्);
    curlx_free(name);
  }

  /* Does the URL contain a query parameter? Only valid when we have a mailbox
     and no UID as per RFC-5092 */
  जइ(imap->mailbox && !imap->uid && !imap->mindex) {
    /* Get the query parameter, URL decoded */
    CURLUcode uc = curl_url_get(data->state.uh, CURLUPART_QUERY, &imap->query,
                                CURLU_URLDECODE);
    जइ(uc == CURLUE_OUT_OF_MEMORY)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
  }

  /* Any extra stuff at the end of the URL is an error */
  जइ(*ptr)
    प्रत्यागम CURLE_URL_MALFORMAT;

  प्रत्यागम CURLE_OK;
}

/***********************************************************************
 *
 * imap_parse_custom_request()
 *
 * Parse the custom request.
 */
स्थिर CURLcode imap_parse_custom_request(संरचना Curl_easy *data,
                                          संरचना IMAP *imap)
{
  CURLcode result = CURLE_OK;
  ध्रुव वर्ण *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  जइ(custom) {
    /* URL decode the custom request */
    result = Curl_urldecode(custom, 0, &imap->custom, रिक्त, REJECT_CTRL);

    /* Extract the parameters if specified */
    जइ(!result) {
      ध्रुव वर्ण *params = imap->custom;

      जाव(*params && *params != ' ')
        params++;

      जइ(*params) {
        imap->custom_params = curlx_strdup(params);
        imap->custom[params - imap->custom] = '\0';

        जइ(!imap->custom_params)
          result = CURLE_OUT_OF_MEMORY;
      }
    }
  }

  प्रत्यागम result;
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
स्थिर CURLcode imap_connect(संरचना Curl_easy *data, bool *done)
{
  संरचना imap_conn *imapc =
    Curl_conn_meta_get(data->conn, CURL_META_IMAP_CONN);
  CURLcode result = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  जइ(!imapc)
    प्रत्यागम CURLE_FAILED_INIT;

  /* Parse the URL options */
  result = imap_parse_url_options(data->conn, imapc);
  जइ(result)
    प्रत्यागम result;

  /* Start off waiting for the server greeting response */
  imap_state(data, imapc, IMAP_SERVERGREET);

  /* Start off with an response id of '*' */
  curlx_strcopy(imapc->resptag, परिमाण(imapc->resptag), STRCONST("*"));

  result = imap_multi_statemach(data, done);

  प्रत्यागम result;
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
स्थिर CURLcode imap_done(संरचना Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  संरचना imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  संरचना IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);

  जइ(!imapc)
    प्रत्यागम CURLE_FAILED_INIT;
  जइ(!imap)
    प्रत्यागम CURLE_OK;

  जइ(status || premature) {
    CURL_TRC_M(data, "IMAP done, not finalizing transfer");
    connclose(conn); /* marked for closure */
    result = status;         /* use the already set error code */
  }
  अण्णहा जइ(!data->set.connect_only &&
          ((!imap->custom && (imap->uid || imap->mindex)) ||
           (imap->custom && data->req.maxdownload > 0) ||
           data->state.upload || IS_MIME_POST(data))) {
    /* Handle responses after FETCH or APPEND transfer has finished.
       For custom commands, check if we set up a download which indicates
       a FETCH-like command with literal data. */

    जइ(!data->state.upload && !IS_MIME_POST(data))
      imap_state(data, imapc, IMAP_FETCH_FINAL);
    अण्णहा {
      /* End the APPEND command first by sending an empty line */
      result = Curl_pp_sendf(data, &imapc->pp, "%s", "");
      जइ(!result)
        imap_state(data, imapc, IMAP_APPEND_FINAL);
    }

    /* Run the state-machine */
    जइ(!result)
      result = imap_block_statemach(data, imapc, FALSE);
  }

  imap_easy_reset(imap);
  प्रत्यागम result;
}

/***********************************************************************
 *
 * imap_perform()
 *
 * This is the actual DO function for IMAP. Fetch or append a message, or do
 * other things according to the options previously setup.
 */
स्थिर CURLcode imap_perform(संरचना Curl_easy *data, bool *connected,
                             bool *dophase_done)
{
  /* This is IMAP and no proxy */
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  संरचना imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  संरचना IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  bool selected = FALSE;

  DEBUGF(infof(data, "DO phase starts"));
  जइ(!imapc || !imap)
    प्रत्यागम CURLE_FAILED_INIT;

  जइ(data->req.no_body) {
    /* Requested no body means no transfer */
    imap->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* Determine if the requested mailbox (with the same UIDVALIDITY if set)
     has already been selected on this connection */
  जइ(imap->mailbox && imapc->mailbox &&
     (!strcmp(imap->mailbox, imapc->mailbox) ||
      (curl_strequal(imap->mailbox, "INBOX") &&
       curl_strequal(imapc->mailbox, "INBOX"))) &&
     (!imap->uidvalidity_set || !imapc->mb_uidvalidity_set ||
      (imap->uidvalidity == imapc->mb_uidvalidity)))
    selected = TRUE;

  /* Start the first command in the DO phase */
  जइ(data->state.upload || IS_MIME_POST(data))
    /* APPEND can be executed directly */
    result = imap_perform_append(data, imapc, imap);
  अण्णहा जइ(imap->custom && (selected || !imap->mailbox))
    /* Custom command using the same mailbox or no mailbox */
    result = imap_perform_list(data, imapc, imap);
  अण्णहा जइ(!imap->custom && selected && (imap->uid || imap->mindex))
    /* FETCH from the same mailbox */
    result = imap_perform_fetch(data, imapc, imap);
  अण्णहा जइ(!imap->custom && selected && imap->query)
    /* SEARCH the current mailbox */
    result = imap_perform_अन्वेषणम्(data, imapc, imap);
  अण्णहा जइ(imap->mailbox && !selected &&
          (imap->custom || imap->uid || imap->mindex || imap->query))
    /* SELECT the mailbox */
    result = imap_perform_select(data, imapc, imap);
  अण्णहा
    /* LIST */
    result = imap_perform_list(data, imapc, imap);

  जइ(result)
    प्रत्यागम result;

  /* Run the state-machine */
  result = imap_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(conn, FIRSTSOCKET);

  जइ(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  प्रत्यागम result;
}

/* Call this when the DO phase has completed */
स्थिर CURLcode imap_dophase_done(संरचना Curl_easy *data,
                                  संरचना IMAP *imap,
                                  bool connected)
{
  (शून्य)connected;

  जइ(imap->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);

  प्रत्यागम CURLE_OK;
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
स्थिर CURLcode imap_regular_transfer(संरचना Curl_easy *data,
                                      संरचना IMAP *imap,
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
  जइ(!result && *dophase_done)
    result = imap_dophase_done(data, imap, connected);

  प्रत्यागम result;
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
स्थिर CURLcode imap_do(संरचना Curl_easy *data, bool *done)
{
  संरचना IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  CURLcode result = CURLE_OK;
  *done = FALSE; /* default to false */

  जइ(!imap)
    प्रत्यागम CURLE_FAILED_INIT;
  /* Parse the URL path */
  result = imap_parse_url_path(data, imap);
  जइ(result)
    प्रत्यागम result;

  /* Parse the custom request */
  result = imap_parse_custom_request(data, imap);
  जइ(result)
    प्रत्यागम result;

  result = imap_regular_transfer(data, imap, done);

  प्रत्यागम result;
}

/***********************************************************************
 *
 * imap_disconnect()
 *
 * Disconnect from an IMAP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
स्थिर CURLcode imap_disconnect(संरचना Curl_easy *data,
                                संरचना connectdata *conn, bool dead_connection)
{
  संरचना imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);

  जइ(imapc &&
     /* We cannot send quit unconditionally. If this connection is stale or
        bad in any way (pingpong has pending data to send),
        sending quit and waiting around here will make the
        disconnect wait in vain and cause more problems than we need to. */
     !dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &imapc->pp) &&
     !imap_perform_logout(data, imapc))
    (शून्य)imap_block_statemach(data, imapc, TRUE); /* ignore errors */

  प्रत्यागम CURLE_OK;
}

/* Called from multi.c while DOing */
स्थिर CURLcode imap_doing(संरचना Curl_easy *data, bool *dophase_done)
{
  संरचना IMAP *imap = Curl_meta_get(data, CURL_META_IMAP_EASY);
  CURLcode result;

  जइ(!imap)
    प्रत्यागम CURLE_FAILED_INIT;

  result = imap_multi_statemach(data, dophase_done);
  जइ(result)
    DEBUGF(infof(data, "DO phase failed"));
  अण्णहा जइ(*dophase_done) {
    result = imap_dophase_done(data, imap, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  प्रत्यागम result;
}

स्थिर शून्य imap_easy_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *entry)
{
  संरचना IMAP *imap = entry;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  imap_easy_reset(imap);
  curlx_free(imap);
}

स्थिर शून्य imap_conn_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *entry)
{
  संरचना imap_conn *imapc = entry;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  Curl_pp_disconnect(&imapc->pp);
  curlx_safefree(imapc->mailbox);
  curlx_free(imapc);
}

/* SASL parameters for the imap protocol */
स्थिर ध्रुव संरचना SASLproto saslimap = {
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

स्थिर CURLcode imap_setup_connection(संरचना Curl_easy *data,
                                      संरचना connectdata *conn)
{
  संरचना imap_conn *imapc;
  संरचना pingpong *pp;
  संरचना IMAP *imap;

  imapc = curlx_calloc(1, परिमाण(*imapc));
  जइ(!imapc)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  pp = &imapc->pp;
  PINGPONG_SETUP(pp, imap_pp_statemachine, imap_endofresp);

  /* Set the default preferred authentication type and mechanism */
  imapc->preftype = IMAP_TYPE_ANY;
  Curl_sasl_init(&imapc->sasl, data, &saslimap);

  Curl_pp_init(pp, Curl_pgrs_now(data));

  जइ(Curl_conn_meta_set(conn, CURL_META_IMAP_CONN, imapc, imap_conn_dtor))
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  imap = curlx_calloc(1, परिमाण(संरचना IMAP));
  जइ(!imap ||
     Curl_meta_set(data, CURL_META_IMAP_EASY, imap, imap_easy_dtor))
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  प्रत्यागम CURLE_OK;
}

/*
 * IMAP protocol.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_imap = {
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
