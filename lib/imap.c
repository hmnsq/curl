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

#यदि_अपरिभाषितम् CURL_DISABLE_IMAP

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#यदि_अन्तः

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
#परिभाषय CURL_META_IMAP_EASY   "meta:proto:imap:easy"
/* meta key for storing protocol meta at connection */
#परिभाषय CURL_META_IMAP_CONN   "meta:proto:imap:conn"

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
  IMAP_चरमः          /* never used */
} imapstate;

/* imap_conn is used for struct connection-oriented data */
संरचना imap_conn {
  संरचना pingpong pp;
  संरचना SASL sasl;           /* SASL-related parameters */
  वर्ण *mailbox;              /* The last selected mailbox */
  imapstate state;            /* Always use imap.c:state() to change state! */
  अचिह्नित पूर्णाङ्क mb_uidvalidity; /* UIDVALIDITY parsed from select response */
  वर्ण resptag[5];            /* Response tag to wait for */
  अचिह्नित वर्ण preftype;     /* Preferred authentication type */
  अचिह्नित वर्ण cmdid;        /* Last used command ID */
  BIT(ssldone);               /* Is connect() over SSL done? */
  BIT(preauth);               /* Is this connection PREAUTH? */
  BIT(tls_supported);         /* StartTLS capability supported by server */
  BIT(login_disabled);        /* LOGIN command disabled by server */
  BIT(ir_supported);          /* Initial response supported by server */
  BIT(mb_uidvalidity_समुच्चयः);
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
  अचिह्नित पूर्णाङ्क uidvalidity; /* UIDVALIDITY to check in select */
  BIT(uidvalidity_समुच्चयः);
};

#परिभाषय IMAP_RESP_OK       1
#परिभाषय IMAP_RESP_NOT_OK   2
#परिभाषय IMAP_RESP_PREAUTH  3

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
स्थिर CURLcode imap_sendf(संरचना Curl_easy *दत्त,
                           संरचना imap_conn *imapc,
                           ध्रुव वर्ण *fmt, ...) CURL_PRINTF(3, 4);
स्थिर CURLcode imap_sendf(संरचना Curl_easy *दत्त,
                           संरचना imap_conn *imapc,
                           ध्रुव वर्ण *fmt, ...)
{
  CURLcode परिणाम = CURLE_OK;
  va_list ap;

  DEBUGASSERT(fmt);

  /* Calculate the tag based on the connection ID and command ID */
  curl_msnprintf(imapc->resptag, परिमाण(imapc->resptag), "%c%03d",
                 'A' + curlx_sltosi((दीर्घ)(दत्त->conn->connection_id % 26)),
                 ++imapc->cmdid);

  va_start(ap, fmt);
  परिणाम = Curl_pp_vsendf(दत्त, &imapc->pp, imapc->resptag, fmt, ap);
  va_end(ap);
  प्रत्यागम परिणाम;
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

  यदि(!str)
    प्रत्यागम रिक्त;

  len = वाक्यदैर्घ्यम्(str);
  nclean = strcspn(str, "() {%*]\\\"");
  यदि(len == nclean)
    /* nothing to escape, return a strdup */
    प्रत्यागम curlx_strdup(str);

  curlx_dyn_init(&पङ्क्तिः, 2000);

  यदि(!escape_only && curlx_dyn_addn(&पङ्क्तिः, "\"", 1))
    प्रत्यागम रिक्त;

  यावत्(*str) {
    यदि((*str == '\\' || *str == '"') &&
       curlx_dyn_addn(&पङ्क्तिः, "\\", 1))
      प्रत्यागम रिक्त;
    यदि(curlx_dyn_addn(&पङ्क्तिः, str, 1))
      प्रत्यागम रिक्त;
    str++;
  }

  यदि(!escape_only && curlx_dyn_addn(&पङ्क्तिः, "\"", 1))
    प्रत्यागम रिक्त;

  प्रत्यागम curlx_dyn_ptr(&पङ्क्तिः);
}

/*
 * Finds the start of a literal '{size}' in line, skipping over quoted strings.
 */
स्थिर ध्रुव वर्ण *imap_find_literal(ध्रुव वर्ण *पङ्क्तिः, size_t len)
{
  ध्रुव वर्ण *अन्तः = पङ्क्तिः + len;
  bool अन्तर्_quote = FALSE;

  यावत्(पङ्क्तिः < अन्तः) {
    यदि(अन्तर्_quote) {
      यदि(*पङ्क्तिः == '\\' && (पङ्क्तिः + 1) < अन्तः) {
        पङ्क्तिः += 2;
        अनुवर्त;
      }
      यदि(*पङ्क्तिः == '"')
        अन्तर्_quote = FALSE;
    }
    अन्यथा {
      यदि(*पङ्क्तिः == '"')
        अन्तर्_quote = TRUE;
      अन्यथा यदि(*पङ्क्तिः == '{')
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
  ध्रुव वर्ण *अन्तः = पङ्क्तिः + len;
  size_t cmd_len = वाक्यदैर्घ्यम्(cmd);

  /* Skip the untagged response marker */
  पङ्क्तिः += 2;

  /* Do we have a number after the marker? */
  यदि(पङ्क्तिः < अन्तः && ISDIGIT(*पङ्क्तिः)) {
    /* Skip the number */
    कुरु
      पङ्क्तिः++;
    यावत्(पङ्क्तिः < अन्तः && ISDIGIT(*पङ्क्तिः));

    /* Do we have the space character? */
    यदि(पङ्क्तिः == अन्तः || *पङ्क्तिः != ' ')
      प्रत्यागम FALSE;

    पङ्क्तिः++;
  }

  /* Does the command name match and is it followed by a space character or at
     the end of line? */
  यदि(पङ्क्तिः + cmd_len <= अन्तः && curl_strnequal(पङ्क्तिः, cmd, cmd_len) &&
     (पङ्क्तिः[cmd_len] == ' ' || पङ्क्तिः + cmd_len + 2 == अन्तः))
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
स्थिर bool imap_endofresp(संरचना Curl_easy *दत्त, संरचना connectdata *conn,
                           ध्रुव वर्ण *पङ्क्तिः, size_t len, पूर्णाङ्क *resp)
{
  संरचना imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  संरचना IMAP *imap = Curl_meta_get(दत्त, CURL_META_IMAP_EASY);
  ध्रुव वर्ण *id;
  size_t id_len;

  DEBUGASSERT(imapc);
  DEBUGASSERT(imap);
  यदि(!imapc || !imap)
    प्रत्यागम FALSE;

  /* Do we have a tagged command response? */
  id = imapc->resptag;
  id_len = वाक्यदैर्घ्यम्(id);
  यदि(len >= id_len + 1 && !memcmp(id, पङ्क्तिः, id_len) && पङ्क्तिः[id_len] == ' ') {
    पङ्क्तिः += id_len + 1;
    len -= id_len + 1;

    यदि(len >= 2 && !memcmp(पङ्क्तिः, "OK", 2))
      *resp = IMAP_RESP_OK;
    अन्यथा यदि(len >= 7 && !memcmp(पङ्क्तिः, "PREAUTH", 7))
      *resp = IMAP_RESP_PREAUTH;
    अन्यथा
      *resp = IMAP_RESP_NOT_OK;

    प्रत्यागम TRUE;
  }

  /* Do we have an untagged command response? */
  यदि(len >= 2 && !memcmp("* ", पङ्क्तिः, 2)) {
    विकल्प(imapc->state) {
    /* States which are interested in untagged responses */
    पक्ष IMAP_CAPABILITY:
      यदि(!imap_matchresp(पङ्क्तिः, len, "CAPABILITY"))
        प्रत्यागम FALSE;
      भङ्ग;

    पक्ष IMAP_LIST:
      यदि((!imap->custom && !imap_matchresp(पङ्क्तिः, len, "LIST")) ||
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
      यदि(!imap_matchresp(पङ्क्तिः, len, "FETCH"))
        प्रत्यागम FALSE;
      भङ्ग;

    पक्ष IMAP_अन्वेषणम्:
      यदि(!imap_matchresp(पङ्क्तिः, len, "SEARCH"))
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
  यदि(!imap->custom && ((len == 3 && पङ्क्तिः[0] == '+') ||
                       (len >= 2 && !memcmp("+ ", पङ्क्तिः, 2)))) {
    विकल्प(imapc->state) {
    /* States which are interested in continuation responses */
    पक्ष IMAP_AUTHENTICATE:
    पक्ष IMAP_APPEND:
      *resp = '+';
      भङ्ग;

    स्वभाव:
      failf(दत्त, "Unexpected continuation response");
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
स्थिर CURLcode imap_get_सन्देशः(संरचना Curl_easy *दत्त, संरचना bufref *बहिर्गमनम्)
{
  संरचना imap_conn *imapc =
    Curl_conn_meta_get(दत्त->conn, CURL_META_IMAP_CONN);
  वर्ण *सन्देशः_2;
  size_t len;

  यदि(!imapc)
    प्रत्यागम CURLE_FAILED_INIT;

  सन्देशः_2 = curlx_dyn_ptr(&imapc->pp.recvbuf);
  len = imapc->pp.nfinal;
  यदि(len > 2) {
    /* Find the start of the message */
    len -= 2;
    प्रति(सन्देशः_2 += 2; ISBLANK(*सन्देशः_2); सन्देशः_2++, len--)
      ;

    /* Find the end of the message */
    यावत्(len--)
      यदि(!ISNEWLINE(सन्देशः_2[len]) && !ISBLANK(सन्देशः_2[len]))
        भङ्ग;

    /* Terminate the message */
    सन्देशः_2[++len] = '\0';
    Curl_bufref_समुच्चयः(बहिर्गमनम्, सन्देशः_2, len, रिक्त);
  }
  अन्यथा
    /* junk input => zero length output */
    Curl_bufref_समुच्चयः(बहिर्गमनम्, "", 0, रिक्त);

  प्रत्यागम CURLE_OK;
}

/***********************************************************************
 *
 * imap_state()
 *
 * This is the ONLY way to change IMAP state!
 */
स्थिर शून्य imap_state(संरचना Curl_easy *दत्त,
                       संरचना imap_conn *imapc,
                       imapstate newstate)
{
#यदि defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  स्थिर ध्रुव वर्ण * ध्रुव नामानि[] = {
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
    /*चरमः*/
  };

  यदि(imapc->state != newstate)
    infof(दत्त, "IMAP %p state change from %s to %s",
          (शून्य *)imapc, नामानि[imapc->state], नामानि[newstate]);
#अन्यथा
  (शून्य)दत्त;
#यदि_अन्तः
  imapc->state = newstate;
}

/***********************************************************************
 *
 * imap_perform_capability()
 *
 * Sends the CAPABILITY command in order to obtain a list of server side
 * supported capabilities.
 */
स्थिर CURLcode imap_perform_capability(संरचना Curl_easy *दत्त,
                                        संरचना imap_conn *imapc)
{
  CURLcode परिणाम = CURLE_OK;

  imapc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  imapc->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  imapc->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPABILITY command */
  परिणाम = imap_sendf(दत्त, imapc, "CAPABILITY");

  यदि(!परिणाम)
    imap_state(दत्त, imapc, IMAP_CAPABILITY);

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * imap_perform_starttls()
 *
 * Sends the STARTTLS command to start the upgrade to TLS.
 */
स्थिर CURLcode imap_perform_starttls(संरचना Curl_easy *दत्त,
                                      संरचना imap_conn *imapc)
{
  /* Send the STARTTLS command */
  CURLcode परिणाम = imap_sendf(दत्त, imapc, "STARTTLS");

  यदि(!परिणाम)
    imap_state(दत्त, imapc, IMAP_STARTTLS);

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * imap_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
स्थिर CURLcode imap_perform_upgrade_tls(संरचना Curl_easy *दत्त,
                                         संरचना imap_conn *imapc,
                                         संरचना connectdata *conn)
{
#यदि_परिभाषितम् USE_SSL
  /* Start the SSL connection */
  CURLcode परिणाम;
  bool ssldone = FALSE;

  यदि(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    परिणाम = Curl_ssl_cfilter_सङ्कलय(
      दत्त, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_imaps;
  }

  DEBUGASSERT(!imapc->ssldone);
  परिणाम = Curl_conn_connect(दत्त, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(दत्त, "imap_perform_upgrade_tls, connect -> %d, %d",
               (पूर्णाङ्क)परिणाम, ssldone));
  यदि(!परिणाम && ssldone) {
    imapc->ssldone = ssldone;
    /* perform CAPA now, changes imapc->state out of IMAP_UPGRADETLS */
    परिणाम = imap_perform_capability(दत्त, imapc);
  }
बहिर्गमनम्:
  प्रत्यागम परिणाम;
#अन्यथा
  (शून्य)दत्त;
  (शून्य)imapc;
  (शून्य)conn;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
}

/***********************************************************************
 *
 * imap_perform_login()
 *
 * Sends a clear text LOGIN command to authenticate with.
 */
स्थिर CURLcode imap_perform_login(संरचना Curl_easy *दत्त,
                                   संरचना imap_conn *imapc,
                                   संरचना connectdata *conn)
{
  CURLcode परिणाम = CURLE_OK;
  वर्ण *user;
  वर्ण *passwd;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  यदि(!conn->creds) {
    imap_state(दत्त, imapc, IMAP_STOP);

    प्रत्यागम परिणाम;
  }

  /* Make sure the username and password are in the correct atom format */
  user = imap_atom(Curl_creds_user(conn->creds), FALSE);
  passwd = imap_atom(Curl_creds_passwd(conn->creds), FALSE);

  /* Send the LOGIN command */
  परिणाम = imap_sendf(दत्त, imapc, "LOGIN %s %s", user ? user : "",
                      passwd ? passwd : "");

  curlx_free(user);
  curlx_strzero(passwd);
  curlx_free(passwd);

  यदि(!परिणाम)
    imap_state(दत्त, imapc, IMAP_LOGIN);

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * imap_perform_authenticate()
 *
 * Sends an AUTHENTICATE command allowing the client to login with the given
 * SASL authentication mechanism.
 */
स्थिर CURLcode imap_perform_authenticate(संरचना Curl_easy *दत्त,
                                          ध्रुव वर्ण *mech,
                                          ध्रुव संरचना bufref *initresp)
{
  संरचना imap_conn *imapc =
    Curl_conn_meta_get(दत्त->conn, CURL_META_IMAP_CONN);
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण *ir = Curl_bufref_ptr(initresp);

  यदि(!imapc)
    प्रत्यागम CURLE_FAILED_INIT;
  यदि(ir) {
    /* Send the AUTHENTICATE command with the initial response */
    परिणाम = imap_sendf(दत्त, imapc, "AUTHENTICATE %s %s",
                        mech, *ir ? ir : "=");
  }
  अन्यथा {
    /* Send the AUTHENTICATE command */
    परिणाम = imap_sendf(दत्त, imapc, "AUTHENTICATE %s", mech);
  }

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * imap_continue_authenticate()
 *
 * Sends SASL continuation data.
 */
स्थिर CURLcode imap_continue_authenticate(संरचना Curl_easy *दत्त,
                                           ध्रुव वर्ण *mech,
                                           ध्रुव संरचना bufref *resp)
{
  संरचना imap_conn *imapc =
    Curl_conn_meta_get(दत्त->conn, CURL_META_IMAP_CONN);

  (शून्य)mech;
  यदि(!imapc)
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम Curl_pp_sendf(दत्त, &imapc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * imap_cancel_authenticate()
 *
 * Sends SASL cancellation.
 */
स्थिर CURLcode imap_cancel_authenticate(संरचना Curl_easy *दत्त,
                                         ध्रुव वर्ण *mech)
{
  संरचना imap_conn *imapc =
    Curl_conn_meta_get(दत्त->conn, CURL_META_IMAP_CONN);

  (शून्य)mech;
  यदि(!imapc)
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम Curl_pp_sendf(दत्त, &imapc->pp, "*");
}

/***********************************************************************
 *
 * imap_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to clear text should a common
 * mechanism not be available between the client and server.
 */
स्थिर CURLcode imap_perform_authentication(संरचना Curl_easy *दत्त,
                                            संरचना imap_conn *imapc)
{
  CURLcode परिणाम = CURLE_OK;
  saslprogress प्रगतिः;

  /* Check if already authenticated OR if there is enough data to authenticate
     with and end the connect phase if we do not */
  यदि(imapc->preauth ||
     !Curl_sasl_can_authenticate(&imapc->sasl, दत्त)) {
    imap_state(दत्त, imapc, IMAP_STOP);
    प्रत्यागम परिणाम;
  }

  /* Calculate the SASL login details */
  परिणाम = Curl_sasl_start(&imapc->sasl, दत्त, (bool)imapc->ir_supported,
                           &प्रगतिः);
  यदि(!परिणाम) {
    यदि(प्रगतिः == SASL_INPROGRESS)
      imap_state(दत्त, imapc, IMAP_AUTHENTICATE);
    अन्यथा यदि(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
      /* Perform clear text authentication */
      परिणाम = imap_perform_login(दत्त, imapc, दत्त->conn);
    अन्यथा
      परिणाम = Curl_sasl_is_blocked(&imapc->sasl, दत्त);
  }

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * imap_perform_list()
 *
 * Sends a LIST command or an alternative custom request.
 */
स्थिर CURLcode imap_perform_list(संरचना Curl_easy *दत्त,
                                  संरचना imap_conn *imapc,
                                  संरचना IMAP *imap)
{
  CURLcode परिणाम = CURLE_OK;

  यदि(imap->custom)
    /* Send the custom request */
    परिणाम = imap_sendf(दत्त, imapc, "%s%s", imap->custom,
                        imap->custom_params ? imap->custom_params : "");
  अन्यथा {
    /* Make sure the mailbox is in the correct atom format if necessary */
    वर्ण *mailbox = imap->mailbox ? imap_atom(imap->mailbox, TRUE)
                                  : curlx_strdup("");
    यदि(!mailbox)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    /* Send the LIST command */
    परिणाम = imap_sendf(दत्त, imapc, "LIST \"%s\" *", mailbox);

    curlx_free(mailbox);
  }

  यदि(!परिणाम)
    imap_state(दत्त, imapc, IMAP_LIST);

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * imap_perform_select()
 *
 * Sends a SELECT command to ask the server to change the selected mailbox.
 */
स्थिर CURLcode imap_perform_select(संरचना Curl_easy *दत्त,
                                    संरचना imap_conn *imapc,
                                    संरचना IMAP *imap)
{
  CURLcode परिणाम = CURLE_OK;
  वर्ण *mailbox;

  /* Invalidate old information as we are switching mailboxes */
  curlx_safefree(imapc->mailbox);
  imapc->mb_uidvalidity_समुच्चयः = FALSE;

  /* Check we have a mailbox */
  यदि(!imap->mailbox) {
    failf(दत्त, "Cannot SELECT without a mailbox.");
    प्रत्यागम CURLE_URL_MALFORMAT;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  यदि(!mailbox)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* Send the SELECT command */
  परिणाम = imap_sendf(दत्त, imapc, "SELECT %s", mailbox);

  curlx_free(mailbox);

  यदि(!परिणाम)
    imap_state(दत्त, imapc, IMAP_SELECT);

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * imap_perform_fetch()
 *
 * Sends a FETCH command to initiate the download of a message.
 */
स्थिर CURLcode imap_perform_fetch(संरचना Curl_easy *दत्त,
                                   संरचना imap_conn *imapc,
                                   संरचना IMAP *imap)
{
  CURLcode परिणाम = CURLE_OK;
  /* Check we have a UID */
  यदि(imap->uid) {

    /* Send the FETCH command */
    यदि(imap->partial)
      परिणाम = imap_sendf(दत्त, imapc, "UID FETCH %s BODY[%s]<%s>",
                          imap->uid, imap->section ? imap->section : "",
                          imap->partial);
    अन्यथा
      परिणाम = imap_sendf(दत्त, imapc, "UID FETCH %s BODY[%s]",
                          imap->uid, imap->section ? imap->section : "");
  }
  अन्यथा यदि(imap->mindex) {
    /* Send the FETCH command */
    यदि(imap->partial)
      परिणाम = imap_sendf(दत्त, imapc, "FETCH %s BODY[%s]<%s>",
                          imap->mindex, imap->section ? imap->section : "",
                          imap->partial);
    अन्यथा
      परिणाम = imap_sendf(दत्त, imapc, "FETCH %s BODY[%s]",
                          imap->mindex, imap->section ? imap->section : "");
  }
  अन्यथा {
    failf(दत्त, "Cannot FETCH without a UID.");
    प्रत्यागम CURLE_URL_MALFORMAT;
  }
  यदि(!परिणाम)
    imap_state(दत्त, imapc, IMAP_FETCH);

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * imap_perform_append()
 *
 * Sends an APPEND command to initiate the upload of a message.
 */
स्थिर CURLcode imap_perform_append(संरचना Curl_easy *दत्त,
                                    संरचना imap_conn *imapc,
                                    संरचना IMAP *imap)
{
  CURLcode परिणाम = CURLE_OK;
  वर्ण *mailbox;
  संरचना dynbuf flags;

  /* Check we have a mailbox */
  यदि(!imap->mailbox) {
    failf(दत्त, "Cannot APPEND without a mailbox.");
    प्रत्यागम CURLE_URL_MALFORMAT;
  }

#यदि_अपरिभाषितम् CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  यदि(IS_MIME_POST(दत्त)) {
    curl_mimepart *postp = दत्त->समुच्चयः.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(अचिह्नित पूर्णाङ्क)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_शीर्षकाणि(postp, दत्त->समुच्चयः.शीर्षकाणि, 0);
    परिणाम = Curl_mime_prepare_शीर्षकाणि(दत्त, postp, रिक्त,
                                       रिक्त, MIMESTRATEGY_MAIL);

    यदि(!परिणाम && !Curl_checkheaders(दत्त, STRCONST("Mime-Version")))
      परिणाम = Curl_mime_सङ्कलय_शीर्षकम्(&postp->curlheaders, "Mime-Version: 1.0");
    यदि(!परिणाम)
      परिणाम = Curl_creader_समुच्चयः_mime(दत्त, postp);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  अन्यथा
#यदि_अन्तः
  {
    परिणाम = Curl_creader_समुच्चयः_fread(दत्त, दत्त->state.infilesize);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  /* Check we know the size of the upload. This takes all readers
   * into account. Especially crlf conversions which make the size
   * unpredictable, e.g. -1. */
  दत्त->state.infilesize = Curl_creader_योग_दीर्घता(दत्त);
  यदि(दत्त->state.infilesize < 0) {
    यदि(दत्त->समुच्चयः.crlf)
      failf(दत्त, "Cannot APPEND with CRLF conversion making size unknown");
    अन्यथा
      failf(दत्त, "Cannot APPEND with unknown input file size");
    प्रत्यागम CURLE_UPLOAD_FAILED;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  यदि(!mailbox)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* Generate flags string and send the APPEND command */
  curlx_dyn_init(&flags, 100);
  यदि(दत्त->समुच्चयः.upload_flags) {
    पूर्णाङ्क i;
    संरचना ulbits ulflag[] = {
      { CURLULFLAG_ANSWERED, "Answered" },
      { CURLULFLAG_DELETED, "Deleted" },
      { CURLULFLAG_DRAFT, "Draft" },
      { CURLULFLAG_FLAGGED, "Flagged" },
      { CURLULFLAG_आलोकितम्, "Seen" },
      { 0, रिक्त }
    };

    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    यदि(curlx_dyn_सङ्कलय(&flags, " (")) {
      गच्छ cleanup;
    }

    प्रति(i = 0; ulflag[i].bit; i++) {
      यदि(दत्त->समुच्चयः.upload_flags & ulflag[i].bit &&
         ((curlx_dyn_len(&flags) > 2 && curlx_dyn_सङ्कलय(&flags, " ")) ||
          curlx_dyn_सङ्कलय(&flags, "\\") ||
          curlx_dyn_सङ्कलय(&flags, ulflag[i].flag)))
        गच्छ cleanup;
    }

    यदि(curlx_dyn_सङ्कलय(&flags, ")"))
      गच्छ cleanup;
  }
  अन्यथा यदि(curlx_dyn_सङ्कलय(&flags, ""))
    गच्छ cleanup;

  परिणाम = imap_sendf(दत्त, imapc, "APPEND %s%s {%" FMT_OFF_T "}",
                      mailbox, curlx_dyn_ptr(&flags), दत्त->state.infilesize);

cleanup:
  curlx_dyn_free(&flags);
  curlx_free(mailbox);

  यदि(!परिणाम)
    imap_state(दत्त, imapc, IMAP_APPEND);

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * imap_perform_search()
 *
 * Sends a SEARCH command.
 */
स्थिर CURLcode imap_perform_अन्वेषणम्(संरचना Curl_easy *दत्त,
                                    संरचना imap_conn *imapc,
                                    संरचना IMAP *imap)
{
  CURLcode परिणाम = CURLE_OK;

  /* Check we have a query string */
  यदि(!imap->query) {
    failf(दत्त, "Cannot SEARCH without a query string.");
    प्रत्यागम CURLE_URL_MALFORMAT;
  }

  /* Send the SEARCH command */
  परिणाम = imap_sendf(दत्त, imapc, "SEARCH %s", imap->query);

  यदि(!परिणाम)
    imap_state(दत्त, imapc, IMAP_अन्वेषणम्);

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * imap_perform_logout()
 *
 * Performs the logout action prior to sclose() being called.
 */
स्थिर CURLcode imap_perform_logout(संरचना Curl_easy *दत्त,
                                    संरचना imap_conn *imapc)
{
  /* Send the LOGOUT command */
  CURLcode परिणाम = imap_sendf(दत्त, imapc, "LOGOUT");

  यदि(!परिणाम)
    imap_state(दत्त, imapc, IMAP_LOGOUT);

  प्रत्यागम परिणाम;
}

/* For the initial server greeting */
स्थिर CURLcode imap_state_servergreet_resp(संरचना Curl_easy *दत्त,
                                            संरचना imap_conn *imapc,
                                            पूर्णाङ्क imapcode,
                                            imapstate instate)
{
  (शून्य)instate;

  यदि(imapcode == IMAP_RESP_PREAUTH) {
    /* PREAUTH */
    imapc->preauth = TRUE;
    infof(दत्त, "PREAUTH connection, already authenticated");
  }
  अन्यथा यदि(imapcode != IMAP_RESP_OK) {
    failf(दत्त, "Got unexpected imap-server response");
    प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  }

  प्रत्यागम imap_perform_capability(दत्त, imapc);
}

/* For CAPABILITY responses */
स्थिर CURLcode imap_state_capability_resp(संरचना Curl_easy *दत्त,
                                           संरचना imap_conn *imapc,
                                           पूर्णाङ्क imapcode,
                                           imapstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण *पङ्क्तिः = curlx_dyn_ptr(&imapc->pp.recvbuf);

  (शून्य)instate;

  /* Do we have an untagged response? */
  यदि(imapcode == '*') {
    पङ्क्तिः += 2;

    /* Loop through the data line */
    प्रति(;;) {
      size_t wordlen;
      यावत्(*पङ्क्तिः && (ISBLANK(*पङ्क्तिः) || ISNEWLINE(*पङ्क्तिः)))
        पङ्क्तिः++;

      यदि(!*पङ्क्तिः)
        भङ्ग;

      /* Extract the word */
      प्रति(wordlen = 0; पङ्क्तिः[wordlen] && !ISBLANK(पङ्क्तिः[wordlen]) &&
                       !ISNEWLINE(पङ्क्तिः[wordlen]);)
        wordlen++;

      /* Does the server support the STARTTLS capability? */
      यदि(wordlen == 8 && curl_strnequal(पङ्क्तिः, "STARTTLS", 8))
        imapc->tls_supported = TRUE;

      /* Has the server explicitly disabled clear text authentication? */
      अन्यथा यदि(wordlen == 13 && curl_strnequal(पङ्क्तिः, "LOGINDISABLED", 13))
        imapc->login_disabled = TRUE;

      /* Does the server support the SASL-IR capability? */
      अन्यथा यदि(wordlen == 7 && curl_strnequal(पङ्क्तिः, "SASL-IR", 7))
        imapc->ir_supported = TRUE;

      /* Do we have a SASL based authentication mechanism? */
      अन्यथा यदि(wordlen > 5 && curl_strnequal(पङ्क्तिः, "AUTH=", 5)) {
        size_t llen;
        अचिह्नित ह्रस्व mechbit;

        पङ्क्तिः += 5;
        wordlen -= 5;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(पङ्क्तिः, wordlen, &llen);
        यदि(mechbit && llen == wordlen)
          imapc->sasl.authmechs |= mechbit;
      }

      पङ्क्तिः += wordlen;
    }
  }
  अन्यथा यदि(दत्त->समुच्चयः.use_ssl && !Curl_conn_is_ssl(दत्त->conn, FIRSTSOCKET)) {
    /* PREAUTH is not compatible with STARTTLS. */
    यदि(imapcode == IMAP_RESP_OK && imapc->tls_supported && !imapc->preauth) {
      /* Switch to TLS connection now */
      परिणाम = imap_perform_starttls(दत्त, imapc);
    }
    अन्यथा यदि(दत्त->समुच्चयः.use_ssl <= CURLUSESSL_TRY)
      परिणाम = imap_perform_authentication(दत्त, imapc);
    अन्यथा {
      failf(दत्त, "STARTTLS not available.");
      परिणाम = CURLE_USE_SSL_FAILED;
    }
  }
  अन्यथा
    परिणाम = imap_perform_authentication(दत्त, imapc);

  प्रत्यागम परिणाम;
}

/* For STARTTLS responses */
स्थिर CURLcode imap_state_starttls_resp(संरचना Curl_easy *दत्त,
                                         संरचना imap_conn *imapc,
                                         पूर्णाङ्क imapcode,
                                         imapstate instate)
{
  CURLcode परिणाम = CURLE_OK;

  (शून्य)instate;

  /* Pipelining in response is forbidden. */
  यदि(imapc->pp.अतिप्रवाहः)
    प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;

  यदि(imapcode != IMAP_RESP_OK) {
    यदि(दत्त->समुच्चयः.use_ssl != CURLUSESSL_TRY) {
      failf(दत्त, "STARTTLS denied");
      परिणाम = CURLE_USE_SSL_FAILED;
    }
    अन्यथा
      परिणाम = imap_perform_authentication(दत्त, imapc);
  }
  अन्यथा
    imap_state(दत्त, imapc, IMAP_UPGRADETLS);

  प्रत्यागम परिणाम;
}

/* For SASL authentication responses */
स्थिर CURLcode imap_state_auth_resp(संरचना Curl_easy *दत्त,
                                     संरचना imap_conn *imapc,
                                     पूर्णाङ्क imapcode,
                                     imapstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  saslprogress प्रगतिः;

  (शून्य)instate;

  परिणाम = Curl_sasl_continue(&imapc->sasl, दत्त, imapcode, &प्रगतिः);
  यदि(!परिणाम)
    विकल्प(प्रगतिः) {
    पक्ष SASL_समाप्तम्:
      imap_state(दत्त, imapc, IMAP_STOP);  /* Authenticated */
      भङ्ग;
    पक्ष SASL_IDLE:            /* No mechanism left after cancellation */
      यदि(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
        /* Perform clear text authentication */
        परिणाम = imap_perform_login(दत्त, imapc, दत्त->conn);
      अन्यथा {
        failf(दत्त, "Authentication cancelled");
        परिणाम = CURLE_LOGIN_DENIED;
      }
      भङ्ग;
    स्वभाव:
      भङ्ग;
    }

  प्रत्यागम परिणाम;
}

/* For LOGIN responses */
स्थिर CURLcode imap_state_login_resp(संरचना Curl_easy *दत्त,
                                      संरचना imap_conn *imapc,
                                      पूर्णाङ्क imapcode,
                                      imapstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  (शून्य)instate;

  यदि(imapcode != IMAP_RESP_OK) {
    failf(दत्त, "Access denied. %c", imapcode);
    परिणाम = CURLE_LOGIN_DENIED;
  }
  अन्यथा
    /* End of connect phase */
    imap_state(दत्त, imapc, IMAP_STOP);

  प्रत्यागम परिणाम;
}

/* Detect IMAP listings vs. downloading a single email */
स्थिर bool is_custom_fetch_listing_मेलनम्(ध्रुव वर्ण *params)
{
  /* match " 1:* (FLAGS ..." or " 1,2,3 (FLAGS ..." */
  यदि(*params++ != ' ')
    प्रत्यागम FALSE;

  यावत्(ISDIGIT(*params)) {
    params++;
    यदि(*params == 0)
      प्रत्यागम FALSE;
  }
  यदि(*params == ':')
    प्रत्यागम TRUE;
  यदि(*params == ',')
    प्रत्यागम TRUE;
  प्रत्यागम FALSE;
}

स्थिर bool is_custom_fetch_listing(संरचना IMAP *imap)
{
  /* filter out "UID FETCH 1:* (FLAGS ..." queries to list emails */
  यदि(!imap->custom)
    प्रत्यागम FALSE;
  अन्यथा यदि(curl_strequal(imap->custom, "FETCH") && imap->custom_params) {
    ध्रुव वर्ण *p = imap->custom_params;
    प्रत्यागम is_custom_fetch_listing_मेलनम्(p);
  }
  अन्यथा यदि(curl_strequal(imap->custom, "UID") && imap->custom_params &&
          curl_strnequal(imap->custom_params, " FETCH ", 7)) {
    ध्रुव वर्ण *p = imap->custom_params + 6;
    प्रत्यागम is_custom_fetch_listing_मेलनम्(p);
  }
  प्रत्यागम FALSE;
}

/* For LIST and SEARCH responses */
स्थिर CURLcode imap_state_listsearch_resp(संरचना Curl_easy *दत्त,
                                           संरचना imap_conn *imapc,
                                           पूर्णाङ्क imapcode,
                                           imapstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण *पङ्क्तिः = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  संरचना IMAP *imap = Curl_meta_get(दत्त, CURL_META_IMAP_EASY);

  DEBUGASSERT(imap);
  यदि(!imap)
    प्रत्यागम CURLE_FAILED_INIT;
  (शून्य)instate;

  यदि(imapcode == '*' && is_custom_fetch_listing(imap)) {
    /* custom FETCH or UID FETCH for listing is not handled here */
  }
  अन्यथा यदि(imapcode == '*') {
    /* Check if this response contains a literal (e.g. FETCH responses with
       body data). Literal syntax is {size}\r\n */
    ध्रुव वर्ण *cr = memchr(पङ्क्तिः, '\r', len);
    size_t पङ्क्तिः_len = cr ? (size_t)(cr - पङ्क्तिः) : len;
    ध्रुव वर्ण *ptr = imap_find_literal(पङ्क्तिः, पङ्क्तिः_len);
    यदि(ptr) {
      curl_off_t आकार = 0;
      bool parsed = FALSE;
      ptr++;
      यदि(!curlx_str_सङ्ख्या(&ptr, &आकार, CURL_OFF_T_MAX) &&
         !curlx_str_single(&ptr, '}'))
        parsed = TRUE;

      यदि(parsed) {
        संरचना pingpong *pp = &imapc->pp;
        size_t बफर_len = curlx_dyn_len(&pp->recvbuf);
        size_t after_शीर्षकम् = बफर_len - pp->nfinal;

        /* This is a literal response, setup to receive the body data */
        infof(दत्त, "Found %" FMT_OFF_T " bytes to download", आकार);

        /* First write the header line */
        परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, पङ्क्तिः, len);
        यदि(परिणाम)
          प्रत्यागम परिणाम;

        /* Handle data already in buffer after the header line */
        यदि(after_शीर्षकम् > 0) {
          /* There is already data in the buffer that is part of the literal
             body or subsequent responses */
          size_t chunk = after_शीर्षकम्;

          /* Keep only the data after the header line */
          curlx_dyn_पुच्छ(&pp->recvbuf, chunk);
          pp->nfinal = 0; /* done */

          /* Limit chunk to the literal size */
          यदि((curl_off_t)chunk > आकार)
            chunk = (size_t)आकार;

          यदि(chunk) {
            /* Write the literal body data */
            परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY,
                                       curlx_dyn_ptr(&pp->recvbuf), chunk);
            यदि(परिणाम)
              प्रत्यागम परिणाम;
          }

          /* Handle remaining data in buffer (either more literal data or
             subsequent responses) */
          यदि(after_शीर्षकम् > chunk) {
            /* Keep the data after the literal body */
            pp->अतिप्रवाहः = after_शीर्षकम् - chunk;
            curlx_dyn_पुच्छ(&pp->recvbuf, pp->अतिप्रवाहः);
          }
          अन्यथा {
            pp->अतिप्रवाहः = 0;
            curlx_dyn_reset(&pp->recvbuf);
          }
        }
        अन्यथा {
          /* No data in buffer yet, reset overflow */
          pp->अतिप्रवाहः = 0;
        }

        यदि((CURL_OFF_T_MAX - आकार) < (curl_off_t)len)
          /* unlikely to actually be a transfer this big, but avoid integer
अतिप्रवाहः*/
          आकार = CURL_OFF_T_MAX;
        अन्यथा
          आकार += len;

        /* Progress size includes both header line and literal body */
        Curl_pgrs_समुच्चयः_Download_आकार(दत्त, आकार);

        यदि(दत्त->req.bytecount == आकार)
          /* All data already transferred (header + literal body) */
          Curl_xfer_सज्जता_nop(दत्त);
        अन्यथा {
          /* Setup to receive the literal body data.
             maxdownload and transfer size include both header line and
             literal body */
          दत्त->req.maxdownload = आकार;
          Curl_xfer_सज्जता_recv(दत्त, FIRSTSOCKET, आकार);
        }
        /* End of DO phase */
        imap_state(दत्त, imapc, IMAP_STOP);
      }
      अन्यथा {
        /* Failed to parse literal, write the line */
        परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, पङ्क्तिः, len);
      }
    }
    अन्यथा {
      /* No literal, write the line as-is */
      परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, पङ्क्तिः, len);
    }
  }
  अन्यथा यदि(imapcode != IMAP_RESP_OK)
    परिणाम = CURLE_QUOTE_ERROR;
  अन्यथा
    /* End of DO phase */
    imap_state(दत्त, imapc, IMAP_STOP);

  प्रत्यागम परिणाम;
}

/* For SELECT responses */
स्थिर CURLcode imap_state_select_resp(संरचना Curl_easy *दत्त,
                                       संरचना imap_conn *imapc,
                                       संरचना IMAP *imap,
                                       पूर्णाङ्क imapcode,
                                       imapstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  (शून्य)instate;

  यदि(imapcode == '*') {
    /* See if this is an UIDVALIDITY response */
    ध्रुव वर्ण *पङ्क्तिः = curlx_dyn_ptr(&imapc->pp.recvbuf);
    size_t len = curlx_dyn_len(&imapc->pp.recvbuf);
    यदि((len >= 18) && checkprefix("OK [UIDVALIDITY ", &पङ्क्तिः[2])) {
      curl_off_t मानम्;
      ध्रुव वर्ण *p = &पङ्क्तिः[2] + CURL_CSTRLEN("OK [UIDVALIDITY ");
      यदि(!curlx_str_सङ्ख्या(&p, &मानम्, UINT_MAX)) {
        imapc->mb_uidvalidity = (अचिह्नित पूर्णाङ्क)मानम्;
        imapc->mb_uidvalidity_समुच्चयः = TRUE;
      }
    }
  }
  अन्यथा यदि(imapcode == IMAP_RESP_OK) {
    /* Check if the UIDVALIDITY has been specified and matches */
    यदि(imap->uidvalidity_समुच्चयः && imapc->mb_uidvalidity_समुच्चयः &&
       (imap->uidvalidity != imapc->mb_uidvalidity)) {
      failf(दत्त, "Mailbox UIDVALIDITY has changed");
      परिणाम = CURLE_REMOTE_FILE_NOT_प्राप्तम्;
    }
    अन्यथा {
      /* Note the currently opened mailbox on this connection */
      DEBUGASSERT(!imapc->mailbox);
      imapc->mailbox = curlx_strdup(imap->mailbox);
      यदि(!imapc->mailbox)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

      यदि(imap->custom)
        परिणाम = imap_perform_list(दत्त, imapc, imap);
      अन्यथा यदि(imap->query)
        परिणाम = imap_perform_अन्वेषणम्(दत्त, imapc, imap);
      अन्यथा
        परिणाम = imap_perform_fetch(दत्त, imapc, imap);
    }
  }
  अन्यथा {
    failf(दत्त, "Select failed");
    परिणाम = CURLE_LOGIN_DENIED;
  }

  प्रत्यागम परिणाम;
}

/* For the (first line of the) FETCH responses */
स्थिर CURLcode imap_state_fetch_resp(संरचना Curl_easy *दत्त,
                                      संरचना imap_conn *imapc,
                                      पूर्णाङ्क imapcode,
                                      imapstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना pingpong *pp = &imapc->pp;
  ध्रुव वर्ण *ptr = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  bool parsed = FALSE;
  curl_off_t आकार = 0;

  (शून्य)instate;

  यदि(imapcode != '*') {
    Curl_pgrs_समुच्चयः_Download_आकार(दत्त, -1);
    imap_state(दत्त, imapc, IMAP_STOP);
    प्रत्यागम CURLE_REMOTE_FILE_NOT_प्राप्तम्;
  }

  /* Something like this is received "* 1 FETCH (BODY[TEXT] {2021}\r" so parse
     the continuation data contained within the curly brackets */
  ptr = imap_find_literal(ptr, len);
  यदि(ptr) {
    ptr++;
    यदि(!curlx_str_सङ्ख्या(&ptr, &आकार, CURL_OFF_T_MAX) &&
       !curlx_str_single(&ptr, '}'))
      parsed = TRUE;
  }

  यदि(parsed) {
    infof(दत्त, "Found %" FMT_OFF_T " bytes to download", आकार);
    Curl_pgrs_समुच्चयः_Download_आकार(दत्त, आकार);

    यदि(pp->अतिप्रवाहः) {
      /* At this point there is a data in the receive buffer that is body
         content, send it as body and then skip it. Do note that there may
         even be additional "headers" after the body. */
      size_t chunk = pp->अतिप्रवाहः;

      /* keep only the overflow */
      curlx_dyn_पुच्छ(&pp->recvbuf, chunk);
      pp->nfinal = 0; /* done */

      यदि((curl_off_t)chunk > आकार)
        /* The conversion from curl_off_t to size_t is always fine here */
        chunk = (size_t)आकार;

      यदि(!chunk) {
        /* no size, we are done with the data */
        imap_state(दत्त, imapc, IMAP_STOP);
        प्रत्यागम CURLE_OK;
      }
      परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY,
                                 curlx_dyn_ptr(&pp->recvbuf), chunk);
      यदि(परिणाम)
        प्रत्यागम परिणाम;

      infof(दत्त, "Written %zu bytes, %" FMT_OFF_T
            " bytes are left for transfer", chunk, (curl_off_t)(आकार - chunk));

      /* Have we used the entire overflow or part of it?*/
      यदि(pp->अतिप्रवाहः > chunk) {
        /* remember the remaining trailing overflow data */
        pp->अतिप्रवाहः -= chunk;
        curlx_dyn_पुच्छ(&pp->recvbuf, pp->अतिप्रवाहः);
      }
      अन्यथा {
        pp->अतिप्रवाहः = 0; /* handled */
        /* Free the cache */
        curlx_dyn_reset(&pp->recvbuf);
      }
    }

    यदि(दत्त->req.bytecount == आकार)
      /* The entire data is already transferred! */
      Curl_xfer_सज्जता_nop(दत्त);
    अन्यथा {
      /* IMAP download */
      दत्त->req.maxdownload = आकार;
      Curl_xfer_सज्जता_recv(दत्त, FIRSTSOCKET, आकार);
    }
  }
  अन्यथा {
    /* We do not know how to parse this line */
    failf(दत्त, "Failed to parse FETCH response.");
    परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  }

  /* End of DO phase */
  imap_state(दत्त, imapc, IMAP_STOP);

  प्रत्यागम परिणाम;
}

/* For final FETCH responses performed after the download */
स्थिर CURLcode imap_state_fetch_final_resp(संरचना Curl_easy *दत्त,
                                            संरचना imap_conn *imapc,
                                            पूर्णाङ्क imapcode,
                                            imapstate instate)
{
  CURLcode परिणाम = CURLE_OK;

  (शून्य)instate;

  यदि(imapcode != IMAP_RESP_OK)
    परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  अन्यथा
    /* End of DONE phase */
    imap_state(दत्त, imapc, IMAP_STOP);

  प्रत्यागम परिणाम;
}

/* For APPEND responses */
स्थिर CURLcode imap_state_append_resp(संरचना Curl_easy *दत्त,
                                       संरचना imap_conn *imapc,
                                       पूर्णाङ्क imapcode,
                                       imapstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  (शून्य)instate;

  यदि(imapcode != '+') {
    परिणाम = CURLE_UPLOAD_FAILED;
  }
  अन्यथा {
    /* Set the progress upload size */
    Curl_pgrs_समुच्चयः_Upload_आकार(दत्त, दत्त->state.infilesize);

    /* IMAP upload */
    Curl_xfer_सज्जता_send(दत्त, FIRSTSOCKET);

    /* End of DO phase */
    imap_state(दत्त, imapc, IMAP_STOP);
  }

  प्रत्यागम परिणाम;
}

/* For final APPEND responses performed after the upload */
स्थिर CURLcode imap_state_append_final_resp(संरचना Curl_easy *दत्त,
                                             संरचना imap_conn *imapc,
                                             पूर्णाङ्क imapcode,
                                             imapstate instate)
{
  CURLcode परिणाम = CURLE_OK;

  (शून्य)instate;

  यदि(imapcode != IMAP_RESP_OK)
    परिणाम = CURLE_UPLOAD_FAILED;
  अन्यथा
    /* End of DONE phase */
    imap_state(दत्त, imapc, IMAP_STOP);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode imap_pp_statemachine(संरचना Curl_easy *दत्त,
                                     संरचना connectdata *conn)
{
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क imapcode;
  संरचना imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  संरचना IMAP *imap = Curl_meta_get(दत्त, CURL_META_IMAP_EASY);
  संरचना pingpong *pp;
  size_t nread = 0;

  यदि(!imapc || !imap)
    प्रत्यागम CURLE_FAILED_INIT;
  pp = &imapc->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not IMAP */
upgrade_tls:
  यदि(imapc->state == IMAP_UPGRADETLS) {
    परिणाम = imap_perform_upgrade_tls(दत्त, imapc, conn);
    यदि(परिणाम || (imapc->state == IMAP_UPGRADETLS))
      प्रत्यागम परिणाम;
  }

  /* Flush any data that needs to be sent */
  यदि(pp->sendleft)
    प्रत्यागम Curl_pp_flushsend(दत्त, pp);

  कुरु {
    /* Read the response from the server */
    परिणाम = Curl_pp_readresp(दत्त, FIRSTSOCKET, pp, &imapcode, &nread);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    /* Was there an error parsing the response line? */
    यदि(imapcode == -1)
      प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;

    यदि(!imapcode)
      भङ्ग;

    /* We have now received a full IMAP server response */
    विकल्प(imapc->state) {
    पक्ष IMAP_SERVERGREET:
      परिणाम = imap_state_servergreet_resp(दत्त, imapc,
                                           imapcode, imapc->state);
      भङ्ग;

    पक्ष IMAP_CAPABILITY:
      परिणाम = imap_state_capability_resp(दत्त, imapc, imapcode, imapc->state);
      भङ्ग;

    पक्ष IMAP_STARTTLS:
      परिणाम = imap_state_starttls_resp(दत्त, imapc, imapcode, imapc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      यदि(!परिणाम && (imapc->state == IMAP_UPGRADETLS))
        गच्छ upgrade_tls;
      भङ्ग;

    पक्ष IMAP_AUTHENTICATE:
      परिणाम = imap_state_auth_resp(दत्त, imapc, imapcode, imapc->state);
      भङ्ग;

    पक्ष IMAP_LOGIN:
      परिणाम = imap_state_login_resp(दत्त, imapc, imapcode, imapc->state);
      भङ्ग;

    पक्ष IMAP_LIST:
    पक्ष IMAP_अन्वेषणम्:
      परिणाम = imap_state_listsearch_resp(दत्त, imapc, imapcode, imapc->state);
      भङ्ग;

    पक्ष IMAP_SELECT:
      परिणाम = imap_state_select_resp(दत्त, imapc, imap,
                                      imapcode, imapc->state);
      भङ्ग;

    पक्ष IMAP_FETCH:
      परिणाम = imap_state_fetch_resp(दत्त, imapc, imapcode, imapc->state);
      भङ्ग;

    पक्ष IMAP_FETCH_FINAL:
      परिणाम = imap_state_fetch_final_resp(दत्त, imapc,
                                           imapcode, imapc->state);
      भङ्ग;

    पक्ष IMAP_APPEND:
      परिणाम = imap_state_append_resp(दत्त, imapc, imapcode, imapc->state);
      भङ्ग;

    पक्ष IMAP_APPEND_FINAL:
      परिणाम = imap_state_append_final_resp(दत्त, imapc,
                                            imapcode, imapc->state);
      भङ्ग;

    पक्ष IMAP_LOGOUT:
    स्वभाव:
      /* internal error */
      imap_state(दत्त, imapc, IMAP_STOP);
      भङ्ग;
    }
  } यावत्(!परिणाम && imapc->state != IMAP_STOP && Curl_pp_moredata(pp));

  प्रत्यागम परिणाम;
}

/* Called repeatedly until done from multi.c */
स्थिर CURLcode imap_multi_statemach(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना imap_conn *imapc =
    Curl_conn_meta_get(दत्त->conn, CURL_META_IMAP_CONN);

  *समाप्तम् = FALSE;
  यदि(!imapc)
    प्रत्यागम CURLE_FAILED_INIT;
  परिणाम = Curl_pp_statemach(दत्त, &imapc->pp, FALSE, FALSE);
  *समाप्तम् = (imapc->state == IMAP_STOP);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode imap_खण्डम्_statemach(संरचना Curl_easy *दत्त,
                                     संरचना imap_conn *imapc,
                                     bool disconnecting)
{
  CURLcode परिणाम = CURLE_OK;

  यावत्(imapc->state != IMAP_STOP && !परिणाम)
    परिणाम = Curl_pp_statemach(दत्त, &imapc->pp, TRUE, disconnecting);

  प्रत्यागम परिणाम;
}

/* For the IMAP "protocol connect" and "doing" phases only */
स्थिर CURLcode imap_pollset(संरचना Curl_easy *दत्त,
                             संरचना easy_pollset *ps)
{
  संरचना imap_conn *imapc =
    Curl_conn_meta_get(दत्त->conn, CURL_META_IMAP_CONN);
  प्रत्यागम imapc ? Curl_pp_pollset(दत्त, &imapc->pp, ps) : CURLE_OK;
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
  imap->uidvalidity_समुच्चयः = FALSE;
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
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण *ptr = conn->options;
  bool prefer_login = FALSE;

  यावत्(!परिणाम && ptr && *ptr) {
    ध्रुव वर्ण *कुञ्चिका = ptr;
    ध्रुव वर्ण *मानम्;

    यावत्(*ptr && *ptr != '=')
      ptr++;

    मानम् = ptr + 1;

    यावत्(*ptr && *ptr != ';')
      ptr++;

    यदि(curl_strnequal(कुञ्चिका, "AUTH=+LOGIN", 11)) {
      /* User prefers plaintext LOGIN over any SASL, including SASL LOGIN */
      prefer_login = TRUE;
      imapc->sasl.prefmech = SASL_AUTH_NONE;
    }
    अन्यथा यदि(curl_strnequal(कुञ्चिका, "AUTH=", 5)) {
      prefer_login = FALSE;
      परिणाम = Curl_sasl_parse_url_auth_option(&imapc->sasl,
                                               मानम्, ptr - मानम्);
    }
    अन्यथा {
      prefer_login = FALSE;
      परिणाम = CURLE_URL_MALFORMAT;
    }

    यदि(*ptr == ';')
      ptr++;
  }

  यदि(prefer_login)
    imapc->preftype = IMAP_TYPE_CLEARTEXT;
  अन्यथा {
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

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * imap_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
स्थिर CURLcode imap_parse_url_मार्गः(संरचना Curl_easy *दत्त,
                                    संरचना IMAP *imap)
{
  /* The imap struct is already initialized in imap_connect() */
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण *begin = &दत्त->state.ऊर्ध्वम्.मार्गः[1]; /* skip leading slash */
  ध्रुव वर्ण *ptr = begin;

  /* See how much of the URL is a valid path and decode it */
  यावत्(imap_is_bchar(*ptr))
    ptr++;

  यदि(ptr != begin) {
    /* Remove the trailing slash if present */
    ध्रुव वर्ण *अन्तः = ptr;
    यदि(अन्तः > begin && अन्तः[-1] == '/')
      अन्तः--;

    परिणाम = Curl_urldecode(begin, अन्तः - begin, &imap->mailbox, रिक्त,
                            REJECT_CTRL);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  अन्यथा
    imap->mailbox = रिक्त;

  /* There can be any number of parameters in the form ";NAME=VALUE" */
  यावत्(*ptr == ';') {
    वर्ण *name;
    वर्ण *मानम्;
    size_t valuelen;

    /* Find the length of the name parameter */
    begin = ++ptr;
    यावत्(*ptr && *ptr != '=')
      ptr++;

    यदि(!*ptr)
      प्रत्यागम CURLE_URL_MALFORMAT;

    /* Decode the name parameter */
    परिणाम = Curl_urldecode(begin, ptr - begin, &name, रिक्त,
                            REJECT_CTRL);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    /* Find the length of the value parameter */
    begin = ++ptr;
    यावत्(imap_is_bchar(*ptr))
      ptr++;

    /* Decode the value parameter */
    परिणाम = Curl_urldecode(begin, ptr - begin, &मानम्, &valuelen,
                            REJECT_CTRL);
    यदि(परिणाम) {
      curlx_free(name);
      प्रत्यागम परिणाम;
    }

    DEBUGF(infof(दत्त, "IMAP URL parameter '%s' = '%s'", name, मानम्));

    /* Process the known hierarchical parameters (UIDVALIDITY, UID, SECTION
       and PARTIAL) stripping of the trailing slash character if it is
       present.

       Note: Unknown parameters trigger a URL_MALFORMAT error. */
    यदि(valuelen > 0 && मानम्[valuelen - 1] == '/')
      मानम्[valuelen - 1] = '\0';
    यदि(valuelen) {
      यदि(curl_strequal(name, "UIDVALIDITY") && !imap->uidvalidity_समुच्चयः) {
        curl_off_t अङ्कः;
        ध्रुव वर्ण *p = (ध्रुव वर्ण *)मानम्;
        यदि(!curlx_str_सङ्ख्या(&p, &अङ्कः, UINT_MAX)) {
          imap->uidvalidity = (अचिह्नित पूर्णाङ्क)अङ्कः;
          imap->uidvalidity_समुच्चयः = TRUE;
        }
        curlx_free(मानम्);
      }
      अन्यथा यदि(curl_strequal(name, "UID") && !imap->uid) {
        imap->uid = मानम्;
      }
      अन्यथा यदि(curl_strequal(name, "MAILINDEX") && !imap->mindex) {
        imap->mindex = मानम्;
      }
      अन्यथा यदि(curl_strequal(name, "SECTION") && !imap->section) {
        imap->section = मानम्;
      }
      अन्यथा यदि(curl_strequal(name, "PARTIAL") && !imap->partial) {
        imap->partial = मानम्;
      }
      अन्यथा {
        curlx_free(name);
        curlx_free(मानम्);
        प्रत्यागम CURLE_URL_MALFORMAT;
      }
    }
    अन्यथा
      /* blank? */
      curlx_free(मानम्);
    curlx_free(name);
  }

  /* Does the URL contain a query parameter? Only valid when we have a mailbox
     and no UID as per RFC-5092 */
  यदि(imap->mailbox && !imap->uid && !imap->mindex) {
    /* Get the query parameter, URL decoded */
    CURLUcode uc = curl_url_get(दत्त->state.uh, CURLUPART_QUERY, &imap->query,
                                CURLU_URLDECODE);
    यदि(uc == CURLUE_बहिर्गमनम्_सम्बन्धि_स्मृतिः)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  /* Any extra stuff at the end of the URL is an error */
  यदि(*ptr)
    प्रत्यागम CURLE_URL_MALFORMAT;

  प्रत्यागम CURLE_OK;
}

/***********************************************************************
 *
 * imap_parse_custom_request()
 *
 * Parse the custom request.
 */
स्थिर CURLcode imap_parse_custom_request(संरचना Curl_easy *दत्त,
                                          संरचना IMAP *imap)
{
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण *custom = CURL_EASY_STR(दत्त, STRING_CUSTOMREQUEST);

  यदि(custom) {
    /* URL decode the custom request */
    परिणाम = Curl_urldecode(custom, 0, &imap->custom, रिक्त, REJECT_CTRL);

    /* Extract the parameters if specified */
    यदि(!परिणाम) {
      ध्रुव वर्ण *params = imap->custom;

      यावत्(*params && *params != ' ')
        params++;

      यदि(*params) {
        imap->custom_params = curlx_strdup(params);
        imap->custom[params - imap->custom] = '\0';

        यदि(!imap->custom_params)
          परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
    }
  }

  प्रत्यागम परिणाम;
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
स्थिर CURLcode imap_connect(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  संरचना imap_conn *imapc =
    Curl_conn_meta_get(दत्त->conn, CURL_META_IMAP_CONN);
  CURLcode परिणाम = CURLE_OK;

  *समाप्तम् = FALSE; /* default to not done yet */
  यदि(!imapc)
    प्रत्यागम CURLE_FAILED_INIT;

  /* Parse the URL options */
  परिणाम = imap_parse_url_options(दत्त->conn, imapc);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* Start off waiting for the server greeting response */
  imap_state(दत्त, imapc, IMAP_SERVERGREET);

  /* Start off with an response id of '*' */
  curlx_strcopy(imapc->resptag, परिमाण(imapc->resptag), STRCONST("*"));

  परिणाम = imap_multi_statemach(दत्त, समाप्तम्);

  प्रत्यागम परिणाम;
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
स्थिर CURLcode imap_समाप्तम्(संरचना Curl_easy *दत्त, CURLcode स्थितिः,
                          bool premature)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;
  संरचना imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  संरचना IMAP *imap = Curl_meta_get(दत्त, CURL_META_IMAP_EASY);

  यदि(!imapc)
    प्रत्यागम CURLE_FAILED_INIT;
  यदि(!imap)
    प्रत्यागम CURLE_OK;

  यदि(स्थितिः || premature) {
    CURL_TRC_M(दत्त, "IMAP done, not finalizing transfer");
    connclose(conn); /* marked for closure */
    परिणाम = स्थितिः;         /* use the already set error code */
  }
  अन्यथा यदि(!दत्त->समुच्चयः.connect_only &&
          ((!imap->custom && (imap->uid || imap->mindex)) ||
           (imap->custom && दत्त->req.maxdownload > 0) ||
           दत्त->state.upload || IS_MIME_POST(दत्त))) {
    /* Handle responses after FETCH or APPEND transfer has finished.
       For custom commands, check if we set up a download which indicates
       a FETCH-like command with literal data. */

    यदि(!दत्त->state.upload && !IS_MIME_POST(दत्त))
      imap_state(दत्त, imapc, IMAP_FETCH_FINAL);
    अन्यथा {
      /* End the APPEND command first by sending an empty line */
      परिणाम = Curl_pp_sendf(दत्त, &imapc->pp, "%s", "");
      यदि(!परिणाम)
        imap_state(दत्त, imapc, IMAP_APPEND_FINAL);
    }

    /* Run the state-machine */
    यदि(!परिणाम)
      परिणाम = imap_खण्डम्_statemach(दत्त, imapc, FALSE);
  }

  imap_easy_reset(imap);
  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * imap_perform()
 *
 * This is the actual DO function for IMAP. Fetch or append a message, or do
 * other things according to the options previously setup.
 */
स्थिर CURLcode imap_perform(संरचना Curl_easy *दत्त, bool *connected,
                             bool *dophase_समाप्तम्)
{
  /* This is IMAP and no proxy */
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;
  संरचना imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  संरचना IMAP *imap = Curl_meta_get(दत्त, CURL_META_IMAP_EASY);
  bool selected = FALSE;

  DEBUGF(infof(दत्त, "DO phase starts"));
  यदि(!imapc || !imap)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(दत्त->req.no_body) {
    /* Requested no body means no transfer */
    imap->transfer = PPTRANSFER_INFO;
  }

  *dophase_समाप्तम् = FALSE; /* not done yet */

  /* Determine if the requested mailbox (with the same UIDVALIDITY if set)
     has already been selected on this connection */
  यदि(imap->mailbox && imapc->mailbox &&
     (!वाक्यं_तुलय(imap->mailbox, imapc->mailbox) ||
      (curl_strequal(imap->mailbox, "INBOX") &&
       curl_strequal(imapc->mailbox, "INBOX"))) &&
     (!imap->uidvalidity_समुच्चयः || !imapc->mb_uidvalidity_समुच्चयः ||
      (imap->uidvalidity == imapc->mb_uidvalidity)))
    selected = TRUE;

  /* Start the first command in the DO phase */
  यदि(दत्त->state.upload || IS_MIME_POST(दत्त))
    /* APPEND can be executed directly */
    परिणाम = imap_perform_append(दत्त, imapc, imap);
  अन्यथा यदि(imap->custom && (selected || !imap->mailbox))
    /* Custom command using the same mailbox or no mailbox */
    परिणाम = imap_perform_list(दत्त, imapc, imap);
  अन्यथा यदि(!imap->custom && selected && (imap->uid || imap->mindex))
    /* FETCH from the same mailbox */
    परिणाम = imap_perform_fetch(दत्त, imapc, imap);
  अन्यथा यदि(!imap->custom && selected && imap->query)
    /* SEARCH the current mailbox */
    परिणाम = imap_perform_अन्वेषणम्(दत्त, imapc, imap);
  अन्यथा यदि(imap->mailbox && !selected &&
          (imap->custom || imap->uid || imap->mindex || imap->query))
    /* SELECT the mailbox */
    परिणाम = imap_perform_select(दत्त, imapc, imap);
  अन्यथा
    /* LIST */
    परिणाम = imap_perform_list(दत्त, imapc, imap);

  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* Run the state-machine */
  परिणाम = imap_multi_statemach(दत्त, dophase_समाप्तम्);

  *connected = Curl_conn_is_connected(conn, FIRSTSOCKET);

  यदि(*dophase_समाप्तम्)
    DEBUGF(infof(दत्त, "DO phase is complete"));

  प्रत्यागम परिणाम;
}

/* Call this when the DO phase has completed */
स्थिर CURLcode imap_dophase_समाप्तम्(संरचना Curl_easy *दत्त,
                                  संरचना IMAP *imap,
                                  bool connected)
{
  (शून्य)connected;

  यदि(imap->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_सज्जता_nop(दत्त);

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
स्थिर CURLcode imap_regular_transfer(संरचना Curl_easy *दत्त,
                                      संरचना IMAP *imap,
                                      bool *dophase_समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  दत्त->req.आकार = -1;

  /* Set the progress data */
  Curl_pgrsReset(दत्त);

  /* Carry out the perform */
  परिणाम = imap_perform(दत्त, &connected, dophase_समाप्तम्);

  /* Perform post DO phase operations if necessary */
  यदि(!परिणाम && *dophase_समाप्तम्)
    परिणाम = imap_dophase_समाप्तम्(दत्त, imap, connected);

  प्रत्यागम परिणाम;
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
स्थिर CURLcode imap_do(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  संरचना IMAP *imap = Curl_meta_get(दत्त, CURL_META_IMAP_EASY);
  CURLcode परिणाम = CURLE_OK;
  *समाप्तम् = FALSE; /* default to false */

  यदि(!imap)
    प्रत्यागम CURLE_FAILED_INIT;
  /* Parse the URL path */
  परिणाम = imap_parse_url_मार्गः(दत्त, imap);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* Parse the custom request */
  परिणाम = imap_parse_custom_request(दत्त, imap);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  परिणाम = imap_regular_transfer(दत्त, imap, समाप्तम्);

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * imap_disconnect()
 *
 * Disconnect from an IMAP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
स्थिर CURLcode imap_disconnect(संरचना Curl_easy *दत्त,
                                संरचना connectdata *conn, bool dead_connection)
{
  संरचना imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);

  यदि(imapc &&
     /* We cannot send quit unconditionally. If this connection is stale or
        bad in any way (pingpong has pending data to send),
        sending quit and waiting around here will make the
        disconnect wait in vain and cause more problems than we need to. */
     !dead_connection && conn->द्वयङ्काः.protoconnstart &&
     !Curl_pp_needs_flush(दत्त, &imapc->pp) &&
     !imap_perform_logout(दत्त, imapc))
    (शून्य)imap_खण्डम्_statemach(दत्त, imapc, TRUE); /* ignore errors */

  प्रत्यागम CURLE_OK;
}

/* Called from multi.c while DOing */
स्थिर CURLcode imap_doing(संरचना Curl_easy *दत्त, bool *dophase_समाप्तम्)
{
  संरचना IMAP *imap = Curl_meta_get(दत्त, CURL_META_IMAP_EASY);
  CURLcode परिणाम;

  यदि(!imap)
    प्रत्यागम CURLE_FAILED_INIT;

  परिणाम = imap_multi_statemach(दत्त, dophase_समाप्तम्);
  यदि(परिणाम)
    DEBUGF(infof(दत्त, "DO phase failed"));
  अन्यथा यदि(*dophase_समाप्तम्) {
    परिणाम = imap_dophase_समाप्तम्(दत्त, imap, FALSE /* not connected */);

    DEBUGF(infof(दत्त, "DO phase is complete"));
  }

  प्रत्यागम परिणाम;
}

स्थिर शून्य imap_easy_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना IMAP *imap = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  imap_easy_reset(imap);
  curlx_free(imap);
}

स्थिर शून्य imap_conn_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना imap_conn *imapc = प्रविष्टिः;
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
  imap_get_सन्देशः,           /* Get SASL response message */
  0,                          /* No maximum initial response length */
  '+',                        /* Code received when continuation is expected */
  IMAP_RESP_OK,               /* Code to receive upon authentication success */
  SASL_AUTH_DEFAULT,          /* Default mechanisms */
  SASL_FLAG_BASE64            /* Configuration flags */
};

स्थिर CURLcode imap_सज्जता_connection(संरचना Curl_easy *दत्त,
                                      संरचना connectdata *conn)
{
  संरचना imap_conn *imapc;
  संरचना pingpong *pp;
  संरचना IMAP *imap;

  imapc = curlx_calloc(1, परिमाण(*imapc));
  यदि(!imapc)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  pp = &imapc->pp;
  PINGPONG_सज्जता(pp, imap_pp_statemachine, imap_endofresp);

  /* Set the default preferred authentication type and mechanism */
  imapc->preftype = IMAP_TYPE_ANY;
  Curl_sasl_init(&imapc->sasl, दत्त, &saslimap);

  Curl_pp_init(pp, Curl_pgrs_now(दत्त));

  यदि(Curl_conn_meta_समुच्चयः(conn, CURL_META_IMAP_CONN, imapc, imap_conn_dtor))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  imap = curlx_calloc(1, परिमाण(संरचना IMAP));
  यदि(!imap ||
     Curl_meta_समुच्चयः(दत्त, CURL_META_IMAP_EASY, imap, imap_easy_dtor))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  प्रत्यागम CURLE_OK;
}

/*
 * IMAP protocol.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_imap = {
  imap_सज्जता_connection,            /* setup_connection */
  imap_do,                          /* do_it */
  imap_समाप्तम्,                        /* done */
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

#यदि_अन्तः /* CURL_DISABLE_IMAP */
