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
#समावेश "curl_setup.h"
#समावेश "urldata.h"
#समावेश "imap.h"

#अगर_अघोषित CURL_DISABLE_IMAP

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित __VMS
#समावेश <in.h>
#समावेश <inet.h>
#पूर्ण_अगर

#समावेश "curlx/dynbuf.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "progress.h"
#समावेश "transfer.h"
#समावेश "escape.h"
#समावेश "pingpong.h"
#समावेश "mime.h"
#समावेश "curlx/strparse.h"
#समावेश "strcase.h"
#समावेश "vtls/vtls.h"
#समावेश "cfilters.h"
#समावेश "connect.h"
#समावेश "select.h"
#समावेश "url.h"
#समावेश "bufref.h"
#समावेश "curl_sasl.h"
#समावेश "curlx/strcopy.h"

/* meta key for storing protocol meta at easy handle */
#घोषणा CURL_META_IMAP_EASY   "meta:proto:imap:easy"
/* meta key for storing protocol meta at connection */
#घोषणा CURL_META_IMAP_CONN   "meta:proto:imap:conn"

प्रकार क्रमागत {
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
  IMAP_खोज,
  IMAP_LOGOUT,
  IMAP_LAST          /* never used */
} imapstate;

/* imap_conn is used for struct connection-oriented data */
काष्ठा imap_conn {
  काष्ठा pingpong pp;
  काष्ठा SASL sasl;           /* SASL-related parameters */
  अक्षर *mailbox;              /* The last selected mailbox */
  imapstate state;            /* Always use imap.c:state() to change state! */
  अचिन्हित पूर्णांक mb_uidvalidity; /* UIDVALIDITY parsed from select response */
  अक्षर resptag[5];            /* Response tag to wait for */
  अचिन्हित अक्षर preftype;     /* Preferred authentication type */
  अचिन्हित अक्षर cmdid;        /* Last used command ID */
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
काष्ठा IMAP {
  curl_pp_transfer transfer;
  अक्षर *mailbox;          /* Mailbox to select */
  अक्षर *uid;              /* Message UID to fetch */
  अक्षर *mindex;           /* Index in mail box of mail to fetch */
  अक्षर *section;          /* Message SECTION to fetch */
  अक्षर *partial;          /* Message PARTIAL to fetch */
  अक्षर *query;            /* Query to search for */
  अक्षर *custom;           /* Custom request */
  अक्षर *custom_params;    /* Parameters for the custom request */
  अचिन्हित पूर्णांक uidvalidity; /* UIDVALIDITY to check in select */
  BIT(uidvalidity_set);
};

#घोषणा IMAP_RESP_OK       1
#घोषणा IMAP_RESP_NOT_OK   2
#घोषणा IMAP_RESP_PREAUTH  3

काष्ठा ulbits {
  पूर्णांक bit;
  स्थिर अक्षर *flag;
};

/***********************************************************************
 *
 * imap_sendf()
 *
 * Sends the formatted string as an IMAP command to the server.
 *
 * Designed to never block.
 */
अटल CURLcode imap_sendf(काष्ठा Curl_easy *तथ्य,
                           काष्ठा imap_conn *imapc,
                           स्थिर अक्षर *fmt, ...) CURL_PRINTF(3, 4);
अटल CURLcode imap_sendf(काष्ठा Curl_easy *तथ्य,
                           काष्ठा imap_conn *imapc,
                           स्थिर अक्षर *fmt, ...)
{
  CURLcode परिणाम = CURLE_OK;
  बहु_सूची ap;

  DEBUGASSERT(fmt);

  /* Calculate the tag based on the connection ID and command ID */
  curl_msnprintf(imapc->resptag, माप(imapc->resptag), "%c%03d",
                 'A' + curlx_sltosi((दीर्घ)(तथ्य->conn->connection_id % 26)),
                 ++imapc->cmdid);

  बहु_शुरू(ap, fmt);
  परिणाम = Curl_pp_vsendf(तथ्य, &imapc->pp, imapc->resptag, fmt, ap);
  बहु_पूर्ण(ap);
  वापस परिणाम;
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
अटल अक्षर *imap_atom(स्थिर अक्षर *str, bool escape_only)
{
  काष्ठा dynbuf पंक्ति;
  माप_प्रकार nclean;
  माप_प्रकार len;

  अगर(!str)
    वापस शून्य;

  len = म_माप(str);
  nclean = म_खोज(str, "() {%*]\\\"");
  अगर(len == nclean)
    /* nothing to escape, return a strdup */
    वापस curlx_strdup(str);

  curlx_dyn_init(&पंक्ति, 2000);

  अगर(!escape_only && curlx_dyn_addn(&पंक्ति, "\"", 1))
    वापस शून्य;

  जबतक(*str) {
    अगर((*str == '\\' || *str == '"') &&
       curlx_dyn_addn(&पंक्ति, "\\", 1))
      वापस शून्य;
    अगर(curlx_dyn_addn(&पंक्ति, str, 1))
      वापस शून्य;
    str++;
  }

  अगर(!escape_only && curlx_dyn_addn(&पंक्ति, "\"", 1))
    वापस शून्य;

  वापस curlx_dyn_ptr(&पंक्ति);
}

/*
 * Finds the start of a literal '{size}' in line, skipping over quoted strings.
 */
अटल स्थिर अक्षर *imap_find_literal(स्थिर अक्षर *पंक्ति, माप_प्रकार len)
{
  स्थिर अक्षर *end = पंक्ति + len;
  bool in_quote = FALSE;

  जबतक(पंक्ति < end) {
    अगर(in_quote) {
      अगर(*पंक्ति == '\\' && (पंक्ति + 1) < end) {
        पंक्ति += 2;
        जारी;
      }
      अगर(*पंक्ति == '"')
        in_quote = FALSE;
    }
    अन्यथा {
      अगर(*पंक्ति == '"')
        in_quote = TRUE;
      अन्यथा अगर(*पंक्ति == '{')
        वापस पंक्ति;
    }
    पंक्ति++;
  }
  वापस शून्य;
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
अटल bool imap_matchresp(स्थिर अक्षर *पंक्ति, माप_प्रकार len, स्थिर अक्षर *cmd)
{
  स्थिर अक्षर *end = पंक्ति + len;
  माप_प्रकार cmd_len = म_माप(cmd);

  /* Skip the untagged response marker */
  पंक्ति += 2;

  /* Do we have a number after the marker? */
  अगर(पंक्ति < end && ISDIGIT(*पंक्ति)) {
    /* Skip the number */
    करो
      पंक्ति++;
    जबतक(पंक्ति < end && ISDIGIT(*पंक्ति));

    /* Do we have the space character? */
    अगर(पंक्ति == end || *पंक्ति != ' ')
      वापस FALSE;

    पंक्ति++;
  }

  /* Does the command name match and is it followed by a space character or at
     the end of line? */
  अगर(पंक्ति + cmd_len <= end && curl_strnequal(पंक्ति, cmd, cmd_len) &&
     (पंक्ति[cmd_len] == ' ' || पंक्ति + cmd_len + 2 == end))
    वापस TRUE;

  वापस FALSE;
}

/***********************************************************************
 *
 * imap_endofresp()
 *
 * Checks whether the given string is a valid tagged, untagged or continuation
 * response which can be processed by the response handler.
 */
अटल bool imap_endofresp(काष्ठा Curl_easy *तथ्य, काष्ठा connectdata *conn,
                           स्थिर अक्षर *पंक्ति, माप_प्रकार len, पूर्णांक *resp)
{
  काष्ठा imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  काष्ठा IMAP *imap = Curl_meta_get(तथ्य, CURL_META_IMAP_EASY);
  स्थिर अक्षर *id;
  माप_प्रकार id_len;

  DEBUGASSERT(imapc);
  DEBUGASSERT(imap);
  अगर(!imapc || !imap)
    वापस FALSE;

  /* Do we have a tagged command response? */
  id = imapc->resptag;
  id_len = म_माप(id);
  अगर(len >= id_len + 1 && !स_भेद(id, पंक्ति, id_len) && पंक्ति[id_len] == ' ') {
    पंक्ति += id_len + 1;
    len -= id_len + 1;

    अगर(len >= 2 && !स_भेद(पंक्ति, "OK", 2))
      *resp = IMAP_RESP_OK;
    अन्यथा अगर(len >= 7 && !स_भेद(पंक्ति, "PREAUTH", 7))
      *resp = IMAP_RESP_PREAUTH;
    अन्यथा
      *resp = IMAP_RESP_NOT_OK;

    वापस TRUE;
  }

  /* Do we have an untagged command response? */
  अगर(len >= 2 && !स_भेद("* ", पंक्ति, 2)) {
    चयन(imapc->state) {
    /* States which are interested in untagged responses */
    हाल IMAP_CAPABILITY:
      अगर(!imap_matchresp(पंक्ति, len, "CAPABILITY"))
        वापस FALSE;
      अवरोध;

    हाल IMAP_LIST:
      अगर((!imap->custom && !imap_matchresp(पंक्ति, len, "LIST")) ||
         (imap->custom && !imap_matchresp(पंक्ति, len, imap->custom) &&
          (!curl_strequal(imap->custom, "STORE") ||
           !imap_matchresp(पंक्ति, len, "FETCH")) &&
          !curl_strequal(imap->custom, "SELECT") &&
          !curl_strequal(imap->custom, "EXAMINE") &&
          !curl_strequal(imap->custom, "SEARCH") &&
          !curl_strequal(imap->custom, "EXPUNGE") &&
          !curl_strequal(imap->custom, "LSUB") &&
          !curl_strequal(imap->custom, "UID") &&
          !curl_strequal(imap->custom, "GETQUOTAROOT") &&
          !curl_strequal(imap->custom, "NOOP")))
        वापस FALSE;
      अवरोध;

    हाल IMAP_SELECT:
      /* SELECT is special in that its untagged responses do not have a
         common prefix so accept anything! */
      अवरोध;

    हाल IMAP_FETCH:
      अगर(!imap_matchresp(पंक्ति, len, "FETCH"))
        वापस FALSE;
      अवरोध;

    हाल IMAP_खोज:
      अगर(!imap_matchresp(पंक्ति, len, "SEARCH"))
        वापस FALSE;
      अवरोध;

    /* Ignore other untagged responses */
    शेष:
      वापस FALSE;
    }

    *resp = '*';
    वापस TRUE;
  }

  /* Do we have a continuation response? This should be a + symbol followed by
     a space and optionally some text as per RFC-3501 for the AUTHENTICATE and
     APPEND commands and as outlined in Section 4. Examples of RFC-4959 but
     some email servers ignore this and only send a single + instead. */
  अगर(!imap->custom && ((len == 3 && पंक्ति[0] == '+') ||
                       (len >= 2 && !स_भेद("+ ", पंक्ति, 2)))) {
    चयन(imapc->state) {
    /* States which are interested in continuation responses */
    हाल IMAP_AUTHENTICATE:
    हाल IMAP_APPEND:
      *resp = '+';
      अवरोध;

    शेष:
      failf(तथ्य, "Unexpected continuation response");
      *resp = -1;
      अवरोध;
    }

    वापस TRUE;
  }

  वापस FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * imap_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
अटल CURLcode imap_get_message(काष्ठा Curl_easy *तथ्य, काष्ठा bufref *out)
{
  काष्ठा imap_conn *imapc =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_IMAP_CONN);
  अक्षर *message;
  माप_प्रकार len;

  अगर(!imapc)
    वापस CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&imapc->pp.recvbuf);
  len = imapc->pp.nfinal;
  अगर(len > 2) {
    /* Find the start of the message */
    len -= 2;
    क्रम(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    जबतक(len--)
      अगर(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        अवरोध;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, शून्य);
  }
  अन्यथा
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, शून्य);

  वापस CURLE_OK;
}

/***********************************************************************
 *
 * imap_state()
 *
 * This is the ONLY way to change IMAP state!
 */
अटल व्योम imap_state(काष्ठा Curl_easy *तथ्य,
                       काष्ठा imap_conn *imapc,
                       imapstate newstate)
{
#अगर defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  अटल स्थिर अक्षर * स्थिर names[] = {
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

  अगर(imapc->state != newstate)
    infof(तथ्य, "IMAP %p state change from %s to %s",
          (व्योम *)imapc, names[imapc->state], names[newstate]);
#अन्यथा
  (व्योम)तथ्य;
#पूर्ण_अगर
  imapc->state = newstate;
}

/***********************************************************************
 *
 * imap_perform_capability()
 *
 * Sends the CAPABILITY command in order to obtain a list of server side
 * supported capabilities.
 */
अटल CURLcode imap_perform_capability(काष्ठा Curl_easy *तथ्य,
                                        काष्ठा imap_conn *imapc)
{
  CURLcode परिणाम = CURLE_OK;

  imapc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  imapc->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  imapc->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPABILITY command */
  परिणाम = imap_sendf(तथ्य, imapc, "CAPABILITY");

  अगर(!परिणाम)
    imap_state(तथ्य, imapc, IMAP_CAPABILITY);

  वापस परिणाम;
}

/***********************************************************************
 *
 * imap_perform_starttls()
 *
 * Sends the STARTTLS command to start the upgrade to TLS.
 */
अटल CURLcode imap_perform_starttls(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा imap_conn *imapc)
{
  /* Send the STARTTLS command */
  CURLcode परिणाम = imap_sendf(तथ्य, imapc, "STARTTLS");

  अगर(!परिणाम)
    imap_state(तथ्य, imapc, IMAP_STARTTLS);

  वापस परिणाम;
}

/***********************************************************************
 *
 * imap_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
अटल CURLcode imap_perform_upgrade_tls(काष्ठा Curl_easy *तथ्य,
                                         काष्ठा imap_conn *imapc,
                                         काष्ठा connectdata *conn)
{
#अगर_घोषित USE_SSL
  /* Start the SSL connection */
  CURLcode परिणाम;
  bool ssldone = FALSE;

  अगर(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    परिणाम = Curl_ssl_cfilter_add(
      तथ्य, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    अगर(परिणाम)
      जाओ out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_imaps;
  }

  DEBUGASSERT(!imapc->ssldone);
  परिणाम = Curl_conn_connect(तथ्य, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(तथ्य, "imap_perform_upgrade_tls, connect -> %d, %d",
               (पूर्णांक)परिणाम, ssldone));
  अगर(!परिणाम && ssldone) {
    imapc->ssldone = ssldone;
    /* perform CAPA now, changes imapc->state out of IMAP_UPGRADETLS */
    परिणाम = imap_perform_capability(तथ्य, imapc);
  }
out:
  वापस परिणाम;
#अन्यथा
  (व्योम)तथ्य;
  (व्योम)imapc;
  (व्योम)conn;
  वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
}

/***********************************************************************
 *
 * imap_perform_login()
 *
 * Sends a clear text LOGIN command to authenticate with.
 */
अटल CURLcode imap_perform_login(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा imap_conn *imapc,
                                   काष्ठा connectdata *conn)
{
  CURLcode परिणाम = CURLE_OK;
  अक्षर *user;
  अक्षर *passwd;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  अगर(!conn->creds) {
    imap_state(तथ्य, imapc, IMAP_STOP);

    वापस परिणाम;
  }

  /* Make sure the username and password are in the correct atom format */
  user = imap_atom(Curl_creds_user(conn->creds), FALSE);
  passwd = imap_atom(Curl_creds_passwd(conn->creds), FALSE);

  /* Send the LOGIN command */
  परिणाम = imap_sendf(तथ्य, imapc, "LOGIN %s %s", user ? user : "",
                      passwd ? passwd : "");

  curlx_free(user);
  curlx_strzero(passwd);
  curlx_free(passwd);

  अगर(!परिणाम)
    imap_state(तथ्य, imapc, IMAP_LOGIN);

  वापस परिणाम;
}

/***********************************************************************
 *
 * imap_perform_authenticate()
 *
 * Sends an AUTHENTICATE command allowing the client to login with the given
 * SASL authentication mechanism.
 */
अटल CURLcode imap_perform_authenticate(काष्ठा Curl_easy *तथ्य,
                                          स्थिर अक्षर *mech,
                                          स्थिर काष्ठा bufref *initresp)
{
  काष्ठा imap_conn *imapc =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_IMAP_CONN);
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर *ir = Curl_bufref_ptr(initresp);

  अगर(!imapc)
    वापस CURLE_FAILED_INIT;
  अगर(ir) {
    /* Send the AUTHENTICATE command with the initial response */
    परिणाम = imap_sendf(तथ्य, imapc, "AUTHENTICATE %s %s",
                        mech, *ir ? ir : "=");
  }
  अन्यथा {
    /* Send the AUTHENTICATE command */
    परिणाम = imap_sendf(तथ्य, imapc, "AUTHENTICATE %s", mech);
  }

  वापस परिणाम;
}

/***********************************************************************
 *
 * imap_continue_authenticate()
 *
 * Sends SASL continuation data.
 */
अटल CURLcode imap_continue_authenticate(काष्ठा Curl_easy *तथ्य,
                                           स्थिर अक्षर *mech,
                                           स्थिर काष्ठा bufref *resp)
{
  काष्ठा imap_conn *imapc =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_IMAP_CONN);

  (व्योम)mech;
  अगर(!imapc)
    वापस CURLE_FAILED_INIT;
  वापस Curl_pp_sendf(तथ्य, &imapc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * imap_cancel_authenticate()
 *
 * Sends SASL cancellation.
 */
अटल CURLcode imap_cancel_authenticate(काष्ठा Curl_easy *तथ्य,
                                         स्थिर अक्षर *mech)
{
  काष्ठा imap_conn *imapc =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_IMAP_CONN);

  (व्योम)mech;
  अगर(!imapc)
    वापस CURLE_FAILED_INIT;
  वापस Curl_pp_sendf(तथ्य, &imapc->pp, "*");
}

/***********************************************************************
 *
 * imap_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to clear text should a common
 * mechanism not be available between the client and server.
 */
अटल CURLcode imap_perform_authentication(काष्ठा Curl_easy *तथ्य,
                                            काष्ठा imap_conn *imapc)
{
  CURLcode परिणाम = CURLE_OK;
  saslprogress progress;

  /* Check if already authenticated OR if there is enough data to authenticate
     with and end the connect phase if we do not */
  अगर(imapc->preauth ||
     !Curl_sasl_can_authenticate(&imapc->sasl, तथ्य)) {
    imap_state(तथ्य, imapc, IMAP_STOP);
    वापस परिणाम;
  }

  /* Calculate the SASL login details */
  परिणाम = Curl_sasl_start(&imapc->sasl, तथ्य, (bool)imapc->ir_supported,
                           &progress);
  अगर(!परिणाम) {
    अगर(progress == SASL_INPROGRESS)
      imap_state(तथ्य, imapc, IMAP_AUTHENTICATE);
    अन्यथा अगर(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
      /* Perform clear text authentication */
      परिणाम = imap_perform_login(तथ्य, imapc, तथ्य->conn);
    अन्यथा
      परिणाम = Curl_sasl_is_blocked(&imapc->sasl, तथ्य);
  }

  वापस परिणाम;
}

/***********************************************************************
 *
 * imap_perform_list()
 *
 * Sends a LIST command or an alternative custom request.
 */
अटल CURLcode imap_perform_list(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा imap_conn *imapc,
                                  काष्ठा IMAP *imap)
{
  CURLcode परिणाम = CURLE_OK;

  अगर(imap->custom)
    /* Send the custom request */
    परिणाम = imap_sendf(तथ्य, imapc, "%s%s", imap->custom,
                        imap->custom_params ? imap->custom_params : "");
  अन्यथा {
    /* Make sure the mailbox is in the correct atom format if necessary */
    अक्षर *mailbox = imap->mailbox ? imap_atom(imap->mailbox, TRUE)
                                  : curlx_strdup("");
    अगर(!mailbox)
      वापस CURLE_OUT_OF_MEMORY;

    /* Send the LIST command */
    परिणाम = imap_sendf(तथ्य, imapc, "LIST \"%s\" *", mailbox);

    curlx_free(mailbox);
  }

  अगर(!परिणाम)
    imap_state(तथ्य, imapc, IMAP_LIST);

  वापस परिणाम;
}

/***********************************************************************
 *
 * imap_perform_select()
 *
 * Sends a SELECT command to ask the server to change the selected mailbox.
 */
अटल CURLcode imap_perform_select(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा imap_conn *imapc,
                                    काष्ठा IMAP *imap)
{
  CURLcode परिणाम = CURLE_OK;
  अक्षर *mailbox;

  /* Invalidate old information as we are switching mailboxes */
  curlx_safefree(imapc->mailbox);
  imapc->mb_uidvalidity_set = FALSE;

  /* Check we have a mailbox */
  अगर(!imap->mailbox) {
    failf(तथ्य, "Cannot SELECT without a mailbox.");
    वापस CURLE_URL_MALFORMAT;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  अगर(!mailbox)
    वापस CURLE_OUT_OF_MEMORY;

  /* Send the SELECT command */
  परिणाम = imap_sendf(तथ्य, imapc, "SELECT %s", mailbox);

  curlx_free(mailbox);

  अगर(!परिणाम)
    imap_state(तथ्य, imapc, IMAP_SELECT);

  वापस परिणाम;
}

/***********************************************************************
 *
 * imap_perform_fetch()
 *
 * Sends a FETCH command to initiate the download of a message.
 */
अटल CURLcode imap_perform_fetch(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा imap_conn *imapc,
                                   काष्ठा IMAP *imap)
{
  CURLcode परिणाम = CURLE_OK;
  /* Check we have a UID */
  अगर(imap->uid) {

    /* Send the FETCH command */
    अगर(imap->partial)
      परिणाम = imap_sendf(तथ्य, imapc, "UID FETCH %s BODY[%s]<%s>",
                          imap->uid, imap->section ? imap->section : "",
                          imap->partial);
    अन्यथा
      परिणाम = imap_sendf(तथ्य, imapc, "UID FETCH %s BODY[%s]",
                          imap->uid, imap->section ? imap->section : "");
  }
  अन्यथा अगर(imap->mindex) {
    /* Send the FETCH command */
    अगर(imap->partial)
      परिणाम = imap_sendf(तथ्य, imapc, "FETCH %s BODY[%s]<%s>",
                          imap->mindex, imap->section ? imap->section : "",
                          imap->partial);
    अन्यथा
      परिणाम = imap_sendf(तथ्य, imapc, "FETCH %s BODY[%s]",
                          imap->mindex, imap->section ? imap->section : "");
  }
  अन्यथा {
    failf(तथ्य, "Cannot FETCH without a UID.");
    वापस CURLE_URL_MALFORMAT;
  }
  अगर(!परिणाम)
    imap_state(तथ्य, imapc, IMAP_FETCH);

  वापस परिणाम;
}

/***********************************************************************
 *
 * imap_perform_append()
 *
 * Sends an APPEND command to initiate the upload of a message.
 */
अटल CURLcode imap_perform_append(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा imap_conn *imapc,
                                    काष्ठा IMAP *imap)
{
  CURLcode परिणाम = CURLE_OK;
  अक्षर *mailbox;
  काष्ठा dynbuf flags;

  /* Check we have a mailbox */
  अगर(!imap->mailbox) {
    failf(तथ्य, "Cannot APPEND without a mailbox.");
    वापस CURLE_URL_MALFORMAT;
  }

#अगर_अघोषित CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  अगर(IS_MIME_POST(तथ्य)) {
    curl_mimepart *postp = तथ्य->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(अचिन्हित पूर्णांक)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, तथ्य->set.headers, 0);
    परिणाम = Curl_mime_prepare_headers(तथ्य, postp, शून्य,
                                       शून्य, MIMESTRATEGY_MAIL);

    अगर(!परिणाम && !Curl_checkheaders(तथ्य, STRCONST("Mime-Version")))
      परिणाम = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    अगर(!परिणाम)
      परिणाम = Curl_creader_set_mime(तथ्य, postp);
    अगर(परिणाम)
      वापस परिणाम;
  }
  अन्यथा
#पूर्ण_अगर
  {
    परिणाम = Curl_creader_set_fread(तथ्य, तथ्य->state.infilesize);
    अगर(परिणाम)
      वापस परिणाम;
  }

  /* Check we know the size of the upload. This takes all readers
   * into account. Especially crlf conversions which make the size
   * unpredictable, e.g. -1. */
  तथ्य->state.infilesize = Curl_creader_योग_लंबाई(तथ्य);
  अगर(तथ्य->state.infilesize < 0) {
    अगर(तथ्य->set.crlf)
      failf(तथ्य, "Cannot APPEND with CRLF conversion making size unknown");
    अन्यथा
      failf(तथ्य, "Cannot APPEND with unknown input file size");
    वापस CURLE_UPLOAD_FAILED;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  अगर(!mailbox)
    वापस CURLE_OUT_OF_MEMORY;

  /* Generate flags string and send the APPEND command */
  curlx_dyn_init(&flags, 100);
  अगर(तथ्य->set.upload_flags) {
    पूर्णांक i;
    काष्ठा ulbits ulflag[] = {
      { CURLULFLAG_ANSWERED, "Answered" },
      { CURLULFLAG_DELETED, "Deleted" },
      { CURLULFLAG_DRAFT, "Draft" },
      { CURLULFLAG_FLAGGED, "Flagged" },
      { CURLULFLAG_SEEN, "Seen" },
      { 0, शून्य }
    };

    परिणाम = CURLE_OUT_OF_MEMORY;
    अगर(curlx_dyn_add(&flags, " (")) {
      जाओ cleanup;
    }

    क्रम(i = 0; ulflag[i].bit; i++) {
      अगर(तथ्य->set.upload_flags & ulflag[i].bit &&
         ((curlx_dyn_len(&flags) > 2 && curlx_dyn_add(&flags, " ")) ||
          curlx_dyn_add(&flags, "\\") ||
          curlx_dyn_add(&flags, ulflag[i].flag)))
        जाओ cleanup;
    }

    अगर(curlx_dyn_add(&flags, ")"))
      जाओ cleanup;
  }
  अन्यथा अगर(curlx_dyn_add(&flags, ""))
    जाओ cleanup;

  परिणाम = imap_sendf(तथ्य, imapc, "APPEND %s%s {%" FMT_OFF_T "}",
                      mailbox, curlx_dyn_ptr(&flags), तथ्य->state.infilesize);

cleanup:
  curlx_dyn_free(&flags);
  curlx_free(mailbox);

  अगर(!परिणाम)
    imap_state(तथ्य, imapc, IMAP_APPEND);

  वापस परिणाम;
}

/***********************************************************************
 *
 * imap_perform_search()
 *
 * Sends a SEARCH command.
 */
अटल CURLcode imap_perform_खोज(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा imap_conn *imapc,
                                    काष्ठा IMAP *imap)
{
  CURLcode परिणाम = CURLE_OK;

  /* Check we have a query string */
  अगर(!imap->query) {
    failf(तथ्य, "Cannot SEARCH without a query string.");
    वापस CURLE_URL_MALFORMAT;
  }

  /* Send the SEARCH command */
  परिणाम = imap_sendf(तथ्य, imapc, "SEARCH %s", imap->query);

  अगर(!परिणाम)
    imap_state(तथ्य, imapc, IMAP_खोज);

  वापस परिणाम;
}

/***********************************************************************
 *
 * imap_perform_logout()
 *
 * Performs the logout action prior to sclose() being called.
 */
अटल CURLcode imap_perform_logout(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा imap_conn *imapc)
{
  /* Send the LOGOUT command */
  CURLcode परिणाम = imap_sendf(तथ्य, imapc, "LOGOUT");

  अगर(!परिणाम)
    imap_state(तथ्य, imapc, IMAP_LOGOUT);

  वापस परिणाम;
}

/* For the initial server greeting */
अटल CURLcode imap_state_servergreet_resp(काष्ठा Curl_easy *तथ्य,
                                            काष्ठा imap_conn *imapc,
                                            पूर्णांक imapcode,
                                            imapstate instate)
{
  (व्योम)instate;

  अगर(imapcode == IMAP_RESP_PREAUTH) {
    /* PREAUTH */
    imapc->preauth = TRUE;
    infof(तथ्य, "PREAUTH connection, already authenticated");
  }
  अन्यथा अगर(imapcode != IMAP_RESP_OK) {
    failf(तथ्य, "Got unexpected imap-server response");
    वापस CURLE_WEIRD_SERVER_REPLY;
  }

  वापस imap_perform_capability(तथ्य, imapc);
}

/* For CAPABILITY responses */
अटल CURLcode imap_state_capability_resp(काष्ठा Curl_easy *तथ्य,
                                           काष्ठा imap_conn *imapc,
                                           पूर्णांक imapcode,
                                           imapstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर *पंक्ति = curlx_dyn_ptr(&imapc->pp.recvbuf);

  (व्योम)instate;

  /* Do we have an untagged response? */
  अगर(imapcode == '*') {
    पंक्ति += 2;

    /* Loop through the data line */
    क्रम(;;) {
      माप_प्रकार wordlen;
      जबतक(*पंक्ति && (ISBLANK(*पंक्ति) || ISNEWLINE(*पंक्ति)))
        पंक्ति++;

      अगर(!*पंक्ति)
        अवरोध;

      /* Extract the word */
      क्रम(wordlen = 0; पंक्ति[wordlen] && !ISBLANK(पंक्ति[wordlen]) &&
                       !ISNEWLINE(पंक्ति[wordlen]);)
        wordlen++;

      /* Does the server support the STARTTLS capability? */
      अगर(wordlen == 8 && curl_strnequal(पंक्ति, "STARTTLS", 8))
        imapc->tls_supported = TRUE;

      /* Has the server explicitly disabled clear text authentication? */
      अन्यथा अगर(wordlen == 13 && curl_strnequal(पंक्ति, "LOGINDISABLED", 13))
        imapc->login_disabled = TRUE;

      /* Does the server support the SASL-IR capability? */
      अन्यथा अगर(wordlen == 7 && curl_strnequal(पंक्ति, "SASL-IR", 7))
        imapc->ir_supported = TRUE;

      /* Do we have a SASL based authentication mechanism? */
      अन्यथा अगर(wordlen > 5 && curl_strnequal(पंक्ति, "AUTH=", 5)) {
        माप_प्रकार llen;
        अचिन्हित लघु mechbit;

        पंक्ति += 5;
        wordlen -= 5;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(पंक्ति, wordlen, &llen);
        अगर(mechbit && llen == wordlen)
          imapc->sasl.authmechs |= mechbit;
      }

      पंक्ति += wordlen;
    }
  }
  अन्यथा अगर(तथ्य->set.use_ssl && !Curl_conn_is_ssl(तथ्य->conn, FIRSTSOCKET)) {
    /* PREAUTH is not compatible with STARTTLS. */
    अगर(imapcode == IMAP_RESP_OK && imapc->tls_supported && !imapc->preauth) {
      /* Switch to TLS connection now */
      परिणाम = imap_perform_starttls(तथ्य, imapc);
    }
    अन्यथा अगर(तथ्य->set.use_ssl <= CURLUSESSL_TRY)
      परिणाम = imap_perform_authentication(तथ्य, imapc);
    अन्यथा {
      failf(तथ्य, "STARTTLS not available.");
      परिणाम = CURLE_USE_SSL_FAILED;
    }
  }
  अन्यथा
    परिणाम = imap_perform_authentication(तथ्य, imapc);

  वापस परिणाम;
}

/* For STARTTLS responses */
अटल CURLcode imap_state_starttls_resp(काष्ठा Curl_easy *तथ्य,
                                         काष्ठा imap_conn *imapc,
                                         पूर्णांक imapcode,
                                         imapstate instate)
{
  CURLcode परिणाम = CURLE_OK;

  (व्योम)instate;

  /* Pipelining in response is forbidden. */
  अगर(imapc->pp.overflow)
    वापस CURLE_WEIRD_SERVER_REPLY;

  अगर(imapcode != IMAP_RESP_OK) {
    अगर(तथ्य->set.use_ssl != CURLUSESSL_TRY) {
      failf(तथ्य, "STARTTLS denied");
      परिणाम = CURLE_USE_SSL_FAILED;
    }
    अन्यथा
      परिणाम = imap_perform_authentication(तथ्य, imapc);
  }
  अन्यथा
    imap_state(तथ्य, imapc, IMAP_UPGRADETLS);

  वापस परिणाम;
}

/* For SASL authentication responses */
अटल CURLcode imap_state_auth_resp(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा imap_conn *imapc,
                                     पूर्णांक imapcode,
                                     imapstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  saslprogress progress;

  (व्योम)instate;

  परिणाम = Curl_sasl_continue(&imapc->sasl, तथ्य, imapcode, &progress);
  अगर(!परिणाम)
    चयन(progress) {
    हाल SASL_DONE:
      imap_state(तथ्य, imapc, IMAP_STOP);  /* Authenticated */
      अवरोध;
    हाल SASL_IDLE:            /* No mechanism left after cancellation */
      अगर(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
        /* Perform clear text authentication */
        परिणाम = imap_perform_login(तथ्य, imapc, तथ्य->conn);
      अन्यथा {
        failf(तथ्य, "Authentication cancelled");
        परिणाम = CURLE_LOGIN_DENIED;
      }
      अवरोध;
    शेष:
      अवरोध;
    }

  वापस परिणाम;
}

/* For LOGIN responses */
अटल CURLcode imap_state_login_resp(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा imap_conn *imapc,
                                      पूर्णांक imapcode,
                                      imapstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  (व्योम)instate;

  अगर(imapcode != IMAP_RESP_OK) {
    failf(तथ्य, "Access denied. %c", imapcode);
    परिणाम = CURLE_LOGIN_DENIED;
  }
  अन्यथा
    /* End of connect phase */
    imap_state(तथ्य, imapc, IMAP_STOP);

  वापस परिणाम;
}

/* Detect IMAP listings vs. downloading a single email */
अटल bool is_custom_fetch_listing_match(स्थिर अक्षर *params)
{
  /* match " 1:* (FLAGS ..." or " 1,2,3 (FLAGS ..." */
  अगर(*params++ != ' ')
    वापस FALSE;

  जबतक(ISDIGIT(*params)) {
    params++;
    अगर(*params == 0)
      वापस FALSE;
  }
  अगर(*params == ':')
    वापस TRUE;
  अगर(*params == ',')
    वापस TRUE;
  वापस FALSE;
}

अटल bool is_custom_fetch_listing(काष्ठा IMAP *imap)
{
  /* filter out "UID FETCH 1:* (FLAGS ..." queries to list emails */
  अगर(!imap->custom)
    वापस FALSE;
  अन्यथा अगर(curl_strequal(imap->custom, "FETCH") && imap->custom_params) {
    स्थिर अक्षर *p = imap->custom_params;
    वापस is_custom_fetch_listing_match(p);
  }
  अन्यथा अगर(curl_strequal(imap->custom, "UID") && imap->custom_params &&
          curl_strnequal(imap->custom_params, " FETCH ", 7)) {
    स्थिर अक्षर *p = imap->custom_params + 6;
    वापस is_custom_fetch_listing_match(p);
  }
  वापस FALSE;
}

/* For LIST and SEARCH responses */
अटल CURLcode imap_state_listsearch_resp(काष्ठा Curl_easy *तथ्य,
                                           काष्ठा imap_conn *imapc,
                                           पूर्णांक imapcode,
                                           imapstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर *पंक्ति = curlx_dyn_ptr(&imapc->pp.recvbuf);
  माप_प्रकार len = imapc->pp.nfinal;
  काष्ठा IMAP *imap = Curl_meta_get(तथ्य, CURL_META_IMAP_EASY);

  DEBUGASSERT(imap);
  अगर(!imap)
    वापस CURLE_FAILED_INIT;
  (व्योम)instate;

  अगर(imapcode == '*' && is_custom_fetch_listing(imap)) {
    /* custom FETCH or UID FETCH for listing is not handled here */
  }
  अन्यथा अगर(imapcode == '*') {
    /* Check if this response contains a literal (e.g. FETCH responses with
       body data). Literal syntax is {size}\r\n */
    स्थिर अक्षर *cr = स_प्रथम(पंक्ति, '\r', len);
    माप_प्रकार पंक्ति_len = cr ? (माप_प्रकार)(cr - पंक्ति) : len;
    स्थिर अक्षर *ptr = imap_find_literal(पंक्ति, पंक्ति_len);
    अगर(ptr) {
      curl_off_t आकार = 0;
      bool parsed = FALSE;
      ptr++;
      अगर(!curlx_str_संख्या(&ptr, &आकार, CURL_OFF_T_MAX) &&
         !curlx_str_single(&ptr, '}'))
        parsed = TRUE;

      अगर(parsed) {
        काष्ठा pingpong *pp = &imapc->pp;
        माप_प्रकार बफर_len = curlx_dyn_len(&pp->recvbuf);
        माप_प्रकार after_header = बफर_len - pp->nfinal;

        /* This is a literal response, setup to receive the body data */
        infof(तथ्य, "Found %" FMT_OFF_T " bytes to download", आकार);

        /* First write the header line */
        परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, पंक्ति, len);
        अगर(परिणाम)
          वापस परिणाम;

        /* Handle data already in buffer after the header line */
        अगर(after_header > 0) {
          /* There is already data in the buffer that is part of the literal
             body or subsequent responses */
          माप_प्रकार chunk = after_header;

          /* Keep only the data after the header line */
          curlx_dyn_पुच्छ(&pp->recvbuf, chunk);
          pp->nfinal = 0; /* done */

          /* Limit chunk to the literal size */
          अगर((curl_off_t)chunk > आकार)
            chunk = (माप_प्रकार)आकार;

          अगर(chunk) {
            /* Write the literal body data */
            परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY,
                                       curlx_dyn_ptr(&pp->recvbuf), chunk);
            अगर(परिणाम)
              वापस परिणाम;
          }

          /* Handle remaining data in buffer (either more literal data or
             subsequent responses) */
          अगर(after_header > chunk) {
            /* Keep the data after the literal body */
            pp->overflow = after_header - chunk;
            curlx_dyn_पुच्छ(&pp->recvbuf, pp->overflow);
          }
          अन्यथा {
            pp->overflow = 0;
            curlx_dyn_reset(&pp->recvbuf);
          }
        }
        अन्यथा {
          /* No data in buffer yet, reset overflow */
          pp->overflow = 0;
        }

        अगर((CURL_OFF_T_MAX - आकार) < (curl_off_t)len)
          /* unlikely to actually be a transfer this big, but avoid integer
             overflow */
          आकार = CURL_OFF_T_MAX;
        अन्यथा
          आकार += len;

        /* Progress size includes both header line and literal body */
        Curl_pgrs_Set_Download_आकार(तथ्य, आकार);

        अगर(तथ्य->req.bytecount == आकार)
          /* All data already transferred (header + literal body) */
          Curl_xfer_setup_nop(तथ्य);
        अन्यथा {
          /* Setup to receive the literal body data.
             maxdownload and transfer size include both header line and
             literal body */
          तथ्य->req.maxdownload = आकार;
          Curl_xfer_setup_recv(तथ्य, FIRSTSOCKET, आकार);
        }
        /* End of DO phase */
        imap_state(तथ्य, imapc, IMAP_STOP);
      }
      अन्यथा {
        /* Failed to parse literal, write the line */
        परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, पंक्ति, len);
      }
    }
    अन्यथा {
      /* No literal, write the line as-is */
      परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, पंक्ति, len);
    }
  }
  अन्यथा अगर(imapcode != IMAP_RESP_OK)
    परिणाम = CURLE_QUOTE_ERROR;
  अन्यथा
    /* End of DO phase */
    imap_state(तथ्य, imapc, IMAP_STOP);

  वापस परिणाम;
}

/* For SELECT responses */
अटल CURLcode imap_state_select_resp(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा imap_conn *imapc,
                                       काष्ठा IMAP *imap,
                                       पूर्णांक imapcode,
                                       imapstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  (व्योम)instate;

  अगर(imapcode == '*') {
    /* See if this is an UIDVALIDITY response */
    स्थिर अक्षर *पंक्ति = curlx_dyn_ptr(&imapc->pp.recvbuf);
    माप_प्रकार len = curlx_dyn_len(&imapc->pp.recvbuf);
    अगर((len >= 18) && checkprefix("OK [UIDVALIDITY ", &पंक्ति[2])) {
      curl_off_t मूल्य;
      स्थिर अक्षर *p = &पंक्ति[2] + CURL_CSTRLEN("OK [UIDVALIDITY ");
      अगर(!curlx_str_संख्या(&p, &मूल्य, अच_पूर्णांक_उच्च)) {
        imapc->mb_uidvalidity = (अचिन्हित पूर्णांक)मूल्य;
        imapc->mb_uidvalidity_set = TRUE;
      }
    }
  }
  अन्यथा अगर(imapcode == IMAP_RESP_OK) {
    /* Check if the UIDVALIDITY has been specified and matches */
    अगर(imap->uidvalidity_set && imapc->mb_uidvalidity_set &&
       (imap->uidvalidity != imapc->mb_uidvalidity)) {
      failf(तथ्य, "Mailbox UIDVALIDITY has changed");
      परिणाम = CURLE_REMOTE_FILE_NOT_FOUND;
    }
    अन्यथा {
      /* Note the currently opened mailbox on this connection */
      DEBUGASSERT(!imapc->mailbox);
      imapc->mailbox = curlx_strdup(imap->mailbox);
      अगर(!imapc->mailbox)
        वापस CURLE_OUT_OF_MEMORY;

      अगर(imap->custom)
        परिणाम = imap_perform_list(तथ्य, imapc, imap);
      अन्यथा अगर(imap->query)
        परिणाम = imap_perform_खोज(तथ्य, imapc, imap);
      अन्यथा
        परिणाम = imap_perform_fetch(तथ्य, imapc, imap);
    }
  }
  अन्यथा {
    failf(तथ्य, "Select failed");
    परिणाम = CURLE_LOGIN_DENIED;
  }

  वापस परिणाम;
}

/* For the (first line of the) FETCH responses */
अटल CURLcode imap_state_fetch_resp(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा imap_conn *imapc,
                                      पूर्णांक imapcode,
                                      imapstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा pingpong *pp = &imapc->pp;
  स्थिर अक्षर *ptr = curlx_dyn_ptr(&imapc->pp.recvbuf);
  माप_प्रकार len = imapc->pp.nfinal;
  bool parsed = FALSE;
  curl_off_t आकार = 0;

  (व्योम)instate;

  अगर(imapcode != '*') {
    Curl_pgrs_Set_Download_आकार(तथ्य, -1);
    imap_state(तथ्य, imapc, IMAP_STOP);
    वापस CURLE_REMOTE_FILE_NOT_FOUND;
  }

  /* Something like this is received "* 1 FETCH (BODY[TEXT] {2021}\r" so parse
     the continuation data contained within the curly brackets */
  ptr = imap_find_literal(ptr, len);
  अगर(ptr) {
    ptr++;
    अगर(!curlx_str_संख्या(&ptr, &आकार, CURL_OFF_T_MAX) &&
       !curlx_str_single(&ptr, '}'))
      parsed = TRUE;
  }

  अगर(parsed) {
    infof(तथ्य, "Found %" FMT_OFF_T " bytes to download", आकार);
    Curl_pgrs_Set_Download_आकार(तथ्य, आकार);

    अगर(pp->overflow) {
      /* At this point there is a data in the receive buffer that is body
         content, send it as body and then skip it. Do note that there may
         even be additional "headers" after the body. */
      माप_प्रकार chunk = pp->overflow;

      /* keep only the overflow */
      curlx_dyn_पुच्छ(&pp->recvbuf, chunk);
      pp->nfinal = 0; /* done */

      अगर((curl_off_t)chunk > आकार)
        /* The conversion from curl_off_t to size_t is always fine here */
        chunk = (माप_प्रकार)आकार;

      अगर(!chunk) {
        /* no size, we are done with the data */
        imap_state(तथ्य, imapc, IMAP_STOP);
        वापस CURLE_OK;
      }
      परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY,
                                 curlx_dyn_ptr(&pp->recvbuf), chunk);
      अगर(परिणाम)
        वापस परिणाम;

      infof(तथ्य, "Written %zu bytes, %" FMT_OFF_T
            " bytes are left for transfer", chunk, (curl_off_t)(आकार - chunk));

      /* Have we used the entire overflow or part of it?*/
      अगर(pp->overflow > chunk) {
        /* remember the remaining trailing overflow data */
        pp->overflow -= chunk;
        curlx_dyn_पुच्छ(&pp->recvbuf, pp->overflow);
      }
      अन्यथा {
        pp->overflow = 0; /* handled */
        /* Free the cache */
        curlx_dyn_reset(&pp->recvbuf);
      }
    }

    अगर(तथ्य->req.bytecount == आकार)
      /* The entire data is already transferred! */
      Curl_xfer_setup_nop(तथ्य);
    अन्यथा {
      /* IMAP download */
      तथ्य->req.maxdownload = आकार;
      Curl_xfer_setup_recv(तथ्य, FIRSTSOCKET, आकार);
    }
  }
  अन्यथा {
    /* We do not know how to parse this line */
    failf(तथ्य, "Failed to parse FETCH response.");
    परिणाम = CURLE_WEIRD_SERVER_REPLY;
  }

  /* End of DO phase */
  imap_state(तथ्य, imapc, IMAP_STOP);

  वापस परिणाम;
}

/* For final FETCH responses performed after the download */
अटल CURLcode imap_state_fetch_final_resp(काष्ठा Curl_easy *तथ्य,
                                            काष्ठा imap_conn *imapc,
                                            पूर्णांक imapcode,
                                            imapstate instate)
{
  CURLcode परिणाम = CURLE_OK;

  (व्योम)instate;

  अगर(imapcode != IMAP_RESP_OK)
    परिणाम = CURLE_WEIRD_SERVER_REPLY;
  अन्यथा
    /* End of DONE phase */
    imap_state(तथ्य, imapc, IMAP_STOP);

  वापस परिणाम;
}

/* For APPEND responses */
अटल CURLcode imap_state_append_resp(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा imap_conn *imapc,
                                       पूर्णांक imapcode,
                                       imapstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  (व्योम)instate;

  अगर(imapcode != '+') {
    परिणाम = CURLE_UPLOAD_FAILED;
  }
  अन्यथा {
    /* Set the progress upload size */
    Curl_pgrs_Set_Upload_आकार(तथ्य, तथ्य->state.infilesize);

    /* IMAP upload */
    Curl_xfer_setup_send(तथ्य, FIRSTSOCKET);

    /* End of DO phase */
    imap_state(तथ्य, imapc, IMAP_STOP);
  }

  वापस परिणाम;
}

/* For final APPEND responses performed after the upload */
अटल CURLcode imap_state_append_final_resp(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा imap_conn *imapc,
                                             पूर्णांक imapcode,
                                             imapstate instate)
{
  CURLcode परिणाम = CURLE_OK;

  (व्योम)instate;

  अगर(imapcode != IMAP_RESP_OK)
    परिणाम = CURLE_UPLOAD_FAILED;
  अन्यथा
    /* End of DONE phase */
    imap_state(तथ्य, imapc, IMAP_STOP);

  वापस परिणाम;
}

अटल CURLcode imap_pp_statemachine(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा connectdata *conn)
{
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक imapcode;
  काष्ठा imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  काष्ठा IMAP *imap = Curl_meta_get(तथ्य, CURL_META_IMAP_EASY);
  काष्ठा pingpong *pp;
  माप_प्रकार nread = 0;

  अगर(!imapc || !imap)
    वापस CURLE_FAILED_INIT;
  pp = &imapc->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not IMAP */
upgrade_tls:
  अगर(imapc->state == IMAP_UPGRADETLS) {
    परिणाम = imap_perform_upgrade_tls(तथ्य, imapc, conn);
    अगर(परिणाम || (imapc->state == IMAP_UPGRADETLS))
      वापस परिणाम;
  }

  /* Flush any data that needs to be sent */
  अगर(pp->sendleft)
    वापस Curl_pp_flushsend(तथ्य, pp);

  करो {
    /* Read the response from the server */
    परिणाम = Curl_pp_readresp(तथ्य, FIRSTSOCKET, pp, &imapcode, &nread);
    अगर(परिणाम)
      वापस परिणाम;

    /* Was there an error parsing the response line? */
    अगर(imapcode == -1)
      वापस CURLE_WEIRD_SERVER_REPLY;

    अगर(!imapcode)
      अवरोध;

    /* We have now received a full IMAP server response */
    चयन(imapc->state) {
    हाल IMAP_SERVERGREET:
      परिणाम = imap_state_servergreet_resp(तथ्य, imapc,
                                           imapcode, imapc->state);
      अवरोध;

    हाल IMAP_CAPABILITY:
      परिणाम = imap_state_capability_resp(तथ्य, imapc, imapcode, imapc->state);
      अवरोध;

    हाल IMAP_STARTTLS:
      परिणाम = imap_state_starttls_resp(तथ्य, imapc, imapcode, imapc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      अगर(!परिणाम && (imapc->state == IMAP_UPGRADETLS))
        जाओ upgrade_tls;
      अवरोध;

    हाल IMAP_AUTHENTICATE:
      परिणाम = imap_state_auth_resp(तथ्य, imapc, imapcode, imapc->state);
      अवरोध;

    हाल IMAP_LOGIN:
      परिणाम = imap_state_login_resp(तथ्य, imapc, imapcode, imapc->state);
      अवरोध;

    हाल IMAP_LIST:
    हाल IMAP_खोज:
      परिणाम = imap_state_listsearch_resp(तथ्य, imapc, imapcode, imapc->state);
      अवरोध;

    हाल IMAP_SELECT:
      परिणाम = imap_state_select_resp(तथ्य, imapc, imap,
                                      imapcode, imapc->state);
      अवरोध;

    हाल IMAP_FETCH:
      परिणाम = imap_state_fetch_resp(तथ्य, imapc, imapcode, imapc->state);
      अवरोध;

    हाल IMAP_FETCH_FINAL:
      परिणाम = imap_state_fetch_final_resp(तथ्य, imapc,
                                           imapcode, imapc->state);
      अवरोध;

    हाल IMAP_APPEND:
      परिणाम = imap_state_append_resp(तथ्य, imapc, imapcode, imapc->state);
      अवरोध;

    हाल IMAP_APPEND_FINAL:
      परिणाम = imap_state_append_final_resp(तथ्य, imapc,
                                            imapcode, imapc->state);
      अवरोध;

    हाल IMAP_LOGOUT:
    शेष:
      /* internal error */
      imap_state(तथ्य, imapc, IMAP_STOP);
      अवरोध;
    }
  } जबतक(!परिणाम && imapc->state != IMAP_STOP && Curl_pp_moredata(pp));

  वापस परिणाम;
}

/* Called repeatedly until done from multi.c */
अटल CURLcode imap_multi_statemach(काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा imap_conn *imapc =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_IMAP_CONN);

  *done = FALSE;
  अगर(!imapc)
    वापस CURLE_FAILED_INIT;
  परिणाम = Curl_pp_statemach(तथ्य, &imapc->pp, FALSE, FALSE);
  *done = (imapc->state == IMAP_STOP);

  वापस परिणाम;
}

अटल CURLcode imap_block_statemach(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा imap_conn *imapc,
                                     bool disconnecting)
{
  CURLcode परिणाम = CURLE_OK;

  जबतक(imapc->state != IMAP_STOP && !परिणाम)
    परिणाम = Curl_pp_statemach(तथ्य, &imapc->pp, TRUE, disconnecting);

  वापस परिणाम;
}

/* For the IMAP "protocol connect" and "doing" phases only */
अटल CURLcode imap_pollset(काष्ठा Curl_easy *तथ्य,
                             काष्ठा easy_pollset *ps)
{
  काष्ठा imap_conn *imapc =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_IMAP_CONN);
  वापस imapc ? Curl_pp_pollset(तथ्य, &imapc->pp, ps) : CURLE_OK;
}

अटल व्योम imap_easy_reset(काष्ठा IMAP *imap)
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
अटल bool imap_is_bchar(अक्षर ch)
{
  /* Performing the alnum check first with macro is faster because of ASCII
     arithmetic */
  वापस ch && (ISALNUM(ch) || म_अक्षर(":@/&=-._~!$\'()*+,%", ch));
}

/***********************************************************************
 *
 * imap_parse_url_options()
 *
 * Parse the URL login options.
 */
अटल CURLcode imap_parse_url_options(काष्ठा connectdata *conn,
                                       काष्ठा imap_conn *imapc)
{
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर *ptr = conn->options;
  bool prefer_login = FALSE;

  जबतक(!परिणाम && ptr && *ptr) {
    स्थिर अक्षर *कुंजी = ptr;
    स्थिर अक्षर *मूल्य;

    जबतक(*ptr && *ptr != '=')
      ptr++;

    मूल्य = ptr + 1;

    जबतक(*ptr && *ptr != ';')
      ptr++;

    अगर(curl_strnequal(कुंजी, "AUTH=+LOGIN", 11)) {
      /* User prefers plaintext LOGIN over any SASL, including SASL LOGIN */
      prefer_login = TRUE;
      imapc->sasl.prefmech = SASL_AUTH_NONE;
    }
    अन्यथा अगर(curl_strnequal(कुंजी, "AUTH=", 5)) {
      prefer_login = FALSE;
      परिणाम = Curl_sasl_parse_url_auth_option(&imapc->sasl,
                                               मूल्य, ptr - मूल्य);
    }
    अन्यथा {
      prefer_login = FALSE;
      परिणाम = CURLE_URL_MALFORMAT;
    }

    अगर(*ptr == ';')
      ptr++;
  }

  अगर(prefer_login)
    imapc->preftype = IMAP_TYPE_CLEARTEXT;
  अन्यथा {
    चयन(imapc->sasl.prefmech) {
    हाल SASL_AUTH_NONE:
      imapc->preftype = IMAP_TYPE_NONE;
      अवरोध;
    हाल SASL_AUTH_DEFAULT:
      imapc->preftype = IMAP_TYPE_ANY;
      अवरोध;
    शेष:
      imapc->preftype = IMAP_TYPE_SASL;
      अवरोध;
    }
  }

  वापस परिणाम;
}

/***********************************************************************
 *
 * imap_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
अटल CURLcode imap_parse_url_path(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा IMAP *imap)
{
  /* The imap struct is already initialized in imap_connect() */
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर *begin = &तथ्य->state.up.path[1]; /* skip leading slash */
  स्थिर अक्षर *ptr = begin;

  /* See how much of the URL is a valid path and decode it */
  जबतक(imap_is_bchar(*ptr))
    ptr++;

  अगर(ptr != begin) {
    /* Remove the trailing slash if present */
    स्थिर अक्षर *end = ptr;
    अगर(end > begin && end[-1] == '/')
      end--;

    परिणाम = Curl_urldecode(begin, end - begin, &imap->mailbox, शून्य,
                            REJECT_CTRL);
    अगर(परिणाम)
      वापस परिणाम;
  }
  अन्यथा
    imap->mailbox = शून्य;

  /* There can be any number of parameters in the form ";NAME=VALUE" */
  जबतक(*ptr == ';') {
    अक्षर *name;
    अक्षर *मूल्य;
    माप_प्रकार valuelen;

    /* Find the length of the name parameter */
    begin = ++ptr;
    जबतक(*ptr && *ptr != '=')
      ptr++;

    अगर(!*ptr)
      वापस CURLE_URL_MALFORMAT;

    /* Decode the name parameter */
    परिणाम = Curl_urldecode(begin, ptr - begin, &name, शून्य,
                            REJECT_CTRL);
    अगर(परिणाम)
      वापस परिणाम;

    /* Find the length of the value parameter */
    begin = ++ptr;
    जबतक(imap_is_bchar(*ptr))
      ptr++;

    /* Decode the value parameter */
    परिणाम = Curl_urldecode(begin, ptr - begin, &मूल्य, &valuelen,
                            REJECT_CTRL);
    अगर(परिणाम) {
      curlx_free(name);
      वापस परिणाम;
    }

    DEBUGF(infof(तथ्य, "IMAP URL parameter '%s' = '%s'", name, मूल्य));

    /* Process the known hierarchical parameters (UIDVALIDITY, UID, SECTION
       and PARTIAL) stripping of the trailing slash character if it is
       present.

       Note: Unknown parameters trigger a URL_MALFORMAT error. */
    अगर(valuelen > 0 && मूल्य[valuelen - 1] == '/')
      मूल्य[valuelen - 1] = '\0';
    अगर(valuelen) {
      अगर(curl_strequal(name, "UIDVALIDITY") && !imap->uidvalidity_set) {
        curl_off_t अंक;
        स्थिर अक्षर *p = (स्थिर अक्षर *)मूल्य;
        अगर(!curlx_str_संख्या(&p, &अंक, अच_पूर्णांक_उच्च)) {
          imap->uidvalidity = (अचिन्हित पूर्णांक)अंक;
          imap->uidvalidity_set = TRUE;
        }
        curlx_free(मूल्य);
      }
      अन्यथा अगर(curl_strequal(name, "UID") && !imap->uid) {
        imap->uid = मूल्य;
      }
      अन्यथा अगर(curl_strequal(name, "MAILINDEX") && !imap->mindex) {
        imap->mindex = मूल्य;
      }
      अन्यथा अगर(curl_strequal(name, "SECTION") && !imap->section) {
        imap->section = मूल्य;
      }
      अन्यथा अगर(curl_strequal(name, "PARTIAL") && !imap->partial) {
        imap->partial = मूल्य;
      }
      अन्यथा {
        curlx_free(name);
        curlx_free(मूल्य);
        वापस CURLE_URL_MALFORMAT;
      }
    }
    अन्यथा
      /* blank? */
      curlx_free(मूल्य);
    curlx_free(name);
  }

  /* Does the URL contain a query parameter? Only valid when we have a mailbox
     and no UID as per RFC-5092 */
  अगर(imap->mailbox && !imap->uid && !imap->mindex) {
    /* Get the query parameter, URL decoded */
    CURLUcode uc = curl_url_get(तथ्य->state.uh, CURLUPART_QUERY, &imap->query,
                                CURLU_URLDECODE);
    अगर(uc == CURLUE_OUT_OF_MEMORY)
      वापस CURLE_OUT_OF_MEMORY;
  }

  /* Any extra stuff at the end of the URL is an error */
  अगर(*ptr)
    वापस CURLE_URL_MALFORMAT;

  वापस CURLE_OK;
}

/***********************************************************************
 *
 * imap_parse_custom_request()
 *
 * Parse the custom request.
 */
अटल CURLcode imap_parse_custom_request(काष्ठा Curl_easy *तथ्य,
                                          काष्ठा IMAP *imap)
{
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर *custom = CURL_EASY_STR(तथ्य, STRING_CUSTOMREQUEST);

  अगर(custom) {
    /* URL decode the custom request */
    परिणाम = Curl_urldecode(custom, 0, &imap->custom, शून्य, REJECT_CTRL);

    /* Extract the parameters if specified */
    अगर(!परिणाम) {
      स्थिर अक्षर *params = imap->custom;

      जबतक(*params && *params != ' ')
        params++;

      अगर(*params) {
        imap->custom_params = curlx_strdup(params);
        imap->custom[params - imap->custom] = '\0';

        अगर(!imap->custom_params)
          परिणाम = CURLE_OUT_OF_MEMORY;
      }
    }
  }

  वापस परिणाम;
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
अटल CURLcode imap_connect(काष्ठा Curl_easy *तथ्य, bool *done)
{
  काष्ठा imap_conn *imapc =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_IMAP_CONN);
  CURLcode परिणाम = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  अगर(!imapc)
    वापस CURLE_FAILED_INIT;

  /* Parse the URL options */
  परिणाम = imap_parse_url_options(तथ्य->conn, imapc);
  अगर(परिणाम)
    वापस परिणाम;

  /* Start off waiting for the server greeting response */
  imap_state(तथ्य, imapc, IMAP_SERVERGREET);

  /* Start off with an response id of '*' */
  curlx_strcopy(imapc->resptag, माप(imapc->resptag), STRCONST("*"));

  परिणाम = imap_multi_statemach(तथ्य, done);

  वापस परिणाम;
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
अटल CURLcode imap_done(काष्ठा Curl_easy *तथ्य, CURLcode status,
                          bool premature)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  काष्ठा IMAP *imap = Curl_meta_get(तथ्य, CURL_META_IMAP_EASY);

  अगर(!imapc)
    वापस CURLE_FAILED_INIT;
  अगर(!imap)
    वापस CURLE_OK;

  अगर(status || premature) {
    CURL_TRC_M(तथ्य, "IMAP done, not finalizing transfer");
    connclose(conn); /* marked for closure */
    परिणाम = status;         /* use the already set error code */
  }
  अन्यथा अगर(!तथ्य->set.connect_only &&
          ((!imap->custom && (imap->uid || imap->mindex)) ||
           (imap->custom && तथ्य->req.maxdownload > 0) ||
           तथ्य->state.upload || IS_MIME_POST(तथ्य))) {
    /* Handle responses after FETCH or APPEND transfer has finished.
       For custom commands, check if we set up a download which indicates
       a FETCH-like command with literal data. */

    अगर(!तथ्य->state.upload && !IS_MIME_POST(तथ्य))
      imap_state(तथ्य, imapc, IMAP_FETCH_FINAL);
    अन्यथा {
      /* End the APPEND command first by sending an empty line */
      परिणाम = Curl_pp_sendf(तथ्य, &imapc->pp, "%s", "");
      अगर(!परिणाम)
        imap_state(तथ्य, imapc, IMAP_APPEND_FINAL);
    }

    /* Run the state-machine */
    अगर(!परिणाम)
      परिणाम = imap_block_statemach(तथ्य, imapc, FALSE);
  }

  imap_easy_reset(imap);
  वापस परिणाम;
}

/***********************************************************************
 *
 * imap_perform()
 *
 * This is the actual DO function for IMAP. Fetch or append a message, or do
 * other things according to the options previously setup.
 */
अटल CURLcode imap_perform(काष्ठा Curl_easy *तथ्य, bool *connected,
                             bool *dophase_done)
{
  /* This is IMAP and no proxy */
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  काष्ठा IMAP *imap = Curl_meta_get(तथ्य, CURL_META_IMAP_EASY);
  bool selected = FALSE;

  DEBUGF(infof(तथ्य, "DO phase starts"));
  अगर(!imapc || !imap)
    वापस CURLE_FAILED_INIT;

  अगर(तथ्य->req.no_body) {
    /* Requested no body means no transfer */
    imap->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* Determine if the requested mailbox (with the same UIDVALIDITY if set)
     has already been selected on this connection */
  अगर(imap->mailbox && imapc->mailbox &&
     (!म_भेद(imap->mailbox, imapc->mailbox) ||
      (curl_strequal(imap->mailbox, "INBOX") &&
       curl_strequal(imapc->mailbox, "INBOX"))) &&
     (!imap->uidvalidity_set || !imapc->mb_uidvalidity_set ||
      (imap->uidvalidity == imapc->mb_uidvalidity)))
    selected = TRUE;

  /* Start the first command in the DO phase */
  अगर(तथ्य->state.upload || IS_MIME_POST(तथ्य))
    /* APPEND can be executed directly */
    परिणाम = imap_perform_append(तथ्य, imapc, imap);
  अन्यथा अगर(imap->custom && (selected || !imap->mailbox))
    /* Custom command using the same mailbox or no mailbox */
    परिणाम = imap_perform_list(तथ्य, imapc, imap);
  अन्यथा अगर(!imap->custom && selected && (imap->uid || imap->mindex))
    /* FETCH from the same mailbox */
    परिणाम = imap_perform_fetch(तथ्य, imapc, imap);
  अन्यथा अगर(!imap->custom && selected && imap->query)
    /* SEARCH the current mailbox */
    परिणाम = imap_perform_खोज(तथ्य, imapc, imap);
  अन्यथा अगर(imap->mailbox && !selected &&
          (imap->custom || imap->uid || imap->mindex || imap->query))
    /* SELECT the mailbox */
    परिणाम = imap_perform_select(तथ्य, imapc, imap);
  अन्यथा
    /* LIST */
    परिणाम = imap_perform_list(तथ्य, imapc, imap);

  अगर(परिणाम)
    वापस परिणाम;

  /* Run the state-machine */
  परिणाम = imap_multi_statemach(तथ्य, dophase_done);

  *connected = Curl_conn_is_connected(conn, FIRSTSOCKET);

  अगर(*dophase_done)
    DEBUGF(infof(तथ्य, "DO phase is complete"));

  वापस परिणाम;
}

/* Call this when the DO phase has completed */
अटल CURLcode imap_dophase_done(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा IMAP *imap,
                                  bool connected)
{
  (व्योम)connected;

  अगर(imap->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(तथ्य);

  वापस CURLE_OK;
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
अटल CURLcode imap_regular_transfer(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा IMAP *imap,
                                      bool *dophase_done)
{
  CURLcode परिणाम = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  तथ्य->req.आकार = -1;

  /* Set the progress data */
  Curl_pgrsReset(तथ्य);

  /* Carry out the perform */
  परिणाम = imap_perform(तथ्य, &connected, dophase_done);

  /* Perform post DO phase operations if necessary */
  अगर(!परिणाम && *dophase_done)
    परिणाम = imap_dophase_done(तथ्य, imap, connected);

  वापस परिणाम;
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
अटल CURLcode imap_do(काष्ठा Curl_easy *तथ्य, bool *done)
{
  काष्ठा IMAP *imap = Curl_meta_get(तथ्य, CURL_META_IMAP_EASY);
  CURLcode परिणाम = CURLE_OK;
  *done = FALSE; /* default to false */

  अगर(!imap)
    वापस CURLE_FAILED_INIT;
  /* Parse the URL path */
  परिणाम = imap_parse_url_path(तथ्य, imap);
  अगर(परिणाम)
    वापस परिणाम;

  /* Parse the custom request */
  परिणाम = imap_parse_custom_request(तथ्य, imap);
  अगर(परिणाम)
    वापस परिणाम;

  परिणाम = imap_regular_transfer(तथ्य, imap, done);

  वापस परिणाम;
}

/***********************************************************************
 *
 * imap_disconnect()
 *
 * Disconnect from an IMAP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
अटल CURLcode imap_disconnect(काष्ठा Curl_easy *तथ्य,
                                काष्ठा connectdata *conn, bool dead_connection)
{
  काष्ठा imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);

  अगर(imapc &&
     /* We cannot send quit unconditionally. If this connection is stale or
        bad in any way (pingpong has pending data to send),
        sending quit and waiting around here will make the
        disconnect wait in vain and cause more problems than we need to. */
     !dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(तथ्य, &imapc->pp) &&
     !imap_perform_logout(तथ्य, imapc))
    (व्योम)imap_block_statemach(तथ्य, imapc, TRUE); /* ignore errors */

  वापस CURLE_OK;
}

/* Called from multi.c while DOing */
अटल CURLcode imap_doing(काष्ठा Curl_easy *तथ्य, bool *dophase_done)
{
  काष्ठा IMAP *imap = Curl_meta_get(तथ्य, CURL_META_IMAP_EASY);
  CURLcode परिणाम;

  अगर(!imap)
    वापस CURLE_FAILED_INIT;

  परिणाम = imap_multi_statemach(तथ्य, dophase_done);
  अगर(परिणाम)
    DEBUGF(infof(तथ्य, "DO phase failed"));
  अन्यथा अगर(*dophase_done) {
    परिणाम = imap_dophase_done(तथ्य, imap, FALSE /* not connected */);

    DEBUGF(infof(तथ्य, "DO phase is complete"));
  }

  वापस परिणाम;
}

अटल व्योम imap_easy_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा IMAP *imap = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  imap_easy_reset(imap);
  curlx_free(imap);
}

अटल व्योम imap_conn_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा imap_conn *imapc = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  Curl_pp_disconnect(&imapc->pp);
  curlx_safefree(imapc->mailbox);
  curlx_free(imapc);
}

/* SASL parameters for the imap protocol */
अटल स्थिर काष्ठा SASLproto saslimap = {
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

अटल CURLcode imap_setup_connection(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा connectdata *conn)
{
  काष्ठा imap_conn *imapc;
  काष्ठा pingpong *pp;
  काष्ठा IMAP *imap;

  imapc = curlx_calloc(1, माप(*imapc));
  अगर(!imapc)
    वापस CURLE_OUT_OF_MEMORY;

  pp = &imapc->pp;
  PINGPONG_SETUP(pp, imap_pp_statemachine, imap_endofresp);

  /* Set the default preferred authentication type and mechanism */
  imapc->preftype = IMAP_TYPE_ANY;
  Curl_sasl_init(&imapc->sasl, तथ्य, &saslimap);

  Curl_pp_init(pp, Curl_pgrs_now(तथ्य));

  अगर(Curl_conn_meta_set(conn, CURL_META_IMAP_CONN, imapc, imap_conn_dtor))
    वापस CURLE_OUT_OF_MEMORY;

  imap = curlx_calloc(1, माप(काष्ठा IMAP));
  अगर(!imap ||
     Curl_meta_set(तथ्य, CURL_META_IMAP_EASY, imap, imap_easy_dtor))
    वापस CURLE_OUT_OF_MEMORY;

  वापस CURLE_OK;
}

/*
 * IMAP protocol.
 */
स्थिर काष्ठा Curl_protocol Curl_protocol_imap = {
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

#पूर्ण_अगर /* CURL_DISABLE_IMAP */
