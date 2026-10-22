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
#అన్తర్భావ "curl_setup.h"
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "imap.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_IMAP

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే __VMS
#అన్తర్భావ <in.h>
#అన్తర్భావ <inet.h>
#అయితే_ముగింపు

#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "pingpong.h"
#అన్తర్భావ "mime.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "bufref.h"
#అన్తర్భావ "curl_sasl.h"
#అన్తర్భావ "curlx/strcopy.h"

/* meta key for storing protocol meta at easy handle */
#నిర్వచించు CURL_META_IMAP_EASY   "meta:proto:imap:easy"
/* meta key for storing protocol meta at connection */
#నిర్వచించు CURL_META_IMAP_CONN   "meta:proto:imap:conn"

రకనిర్వచనం గణన {
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
  IMAP_శోధన,
  IMAP_LOGOUT,
  IMAP_చివరి          /* never used */
} imapstate;

/* imap_conn is used for struct connection-oriented data */
నిర్మాణం imap_conn {
  నిర్మాణం pingpong pp;
  నిర్మాణం SASL sasl;           /* SASL-related parameters */
  అక్షరం *mailbox;              /* The last selected mailbox */
  imapstate state;            /* Always use imap.c:state() to change state! */
  గుర్తులేని పూర్ణాంకం mb_uidvalidity; /* UIDVALIDITY parsed from select response */
  అక్షరం resptag[5];            /* Response tag to wait for */
  గుర్తులేని అక్షరం preftype;     /* Preferred authentication type */
  గుర్తులేని అక్షరం cmdid;        /* Last used command ID */
  బిట్టు(ssldone);               /* Is connect() over SSL done? */
  బిట్టు(preauth);               /* Is this connection PREAUTH? */
  బిట్టు(tls_supported);         /* StartTLS capability supported by server */
  బిట్టు(login_disabled);        /* LOGIN command disabled by server */
  బిట్టు(ir_supported);          /* Initial response supported by server */
  బిట్టు(mb_uidvalidity_సమితి);
};

/* This IMAP struct is used in the Curl_easy. All IMAP data that is
   connection-oriented must be in imap_conn to properly deal with the fact that
   perhaps the Curl_easy is changed between the times the connection is
   used. */
నిర్మాణం IMAP {
  curl_pp_transfer transfer;
  అక్షరం *mailbox;          /* Mailbox to select */
  అక్షరం *uid;              /* Message UID to fetch */
  అక్షరం *mindex;           /* Index in mail box of mail to fetch */
  అక్షరం *section;          /* Message SECTION to fetch */
  అక్షరం *partial;          /* Message PARTIAL to fetch */
  అక్షరం *query;            /* Query to search for */
  అక్షరం *custom;           /* Custom request */
  అక్షరం *custom_params;    /* Parameters for the custom request */
  గుర్తులేని పూర్ణాంకం uidvalidity; /* UIDVALIDITY to check in select */
  బిట్టు(uidvalidity_సమితి);
};

#నిర్వచించు IMAP_RESP_OK       1
#నిర్వచించు IMAP_RESP_NOT_OK   2
#నిర్వచించు IMAP_RESP_PREAUTH  3

నిర్మాణం ulbits {
  పూర్ణాంకం బిట్టు_2;
  స్థిరరాశి అక్షరం *flag;
};

/***********************************************************************
 *
 * imap_sendf()
 *
 * Sends the formatted string as an IMAP command to the server.
 *
 * Designed to never block.
 */
స్థిర CURLcode imap_sendf(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం imap_conn *imapc,
                           స్థిరరాశి అక్షరం *fmt, ...) CURL_PRINTF(3, 4);
స్థిర CURLcode imap_sendf(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం imap_conn *imapc,
                           స్థిరరాశి అక్షరం *fmt, ...)
{
  CURLcode ఫలితం = CURLE_OK;
  va_list ap;

  DEBUGASSERT(fmt);

  /* Calculate the tag based on the connection ID and command ID */
  curl_msnprintf(imapc->resptag, పరిమాణం(imapc->resptag), "%c%03d",
                 'A' + curlx_sltosi((దీర్ఘ)(దత్తాంశం->conn->connection_id % 26)),
                 ++imapc->cmdid);

  va_start(ap, fmt);
  ఫలితం = Curl_pp_vsendf(దత్తాంశం, &imapc->pp, imapc->resptag, fmt, ap);
  va_end(ap);
  తిరిగి ఫలితం;
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
స్థిర అక్షరం *imap_atom(స్థిరరాశి అక్షరం *str, bool escape_only)
{
  నిర్మాణం dynbuf పంక్తి;
  size_t nclean;
  size_t len;

  ఒకవేళ(!str)
    తిరిగి ఖాళీ;

  len = అక్షరశ్రేణి_పొడవు(str);
  nclean = strcspn(str, "() {%*]\\\"");
  ఒకవేళ(len == nclean)
    /* nothing to escape, return a strdup */
    తిరిగి curlx_strdup(str);

  curlx_dyn_init(&పంక్తి, 2000);

  ఒకవేళ(!escape_only && curlx_dyn_addn(&పంక్తి, "\"", 1))
    తిరిగి ఖాళీ;

  అయితే(*str) {
    ఒకవేళ((*str == '\\' || *str == '"') &&
       curlx_dyn_addn(&పంక్తి, "\\", 1))
      తిరిగి ఖాళీ;
    ఒకవేళ(curlx_dyn_addn(&పంక్తి, str, 1))
      తిరిగి ఖాళీ;
    str++;
  }

  ఒకవేళ(!escape_only && curlx_dyn_addn(&పంక్తి, "\"", 1))
    తిరిగి ఖాళీ;

  తిరిగి curlx_dyn_ptr(&పంక్తి);
}

/*
 * Finds the start of a literal '{size}' in line, skipping over quoted strings.
 */
స్థిర స్థిరరాశి అక్షరం *imap_కనుగొను_literal(స్థిరరాశి అక్షరం *పంక్తి, size_t len)
{
  స్థిరరాశి అక్షరం *చివర = పంక్తి + len;
  bool లోపల_quote = FALSE;

  అయితే(పంక్తి < చివర) {
    ఒకవేళ(లోపల_quote) {
      ఒకవేళ(*పంక్తి == '\\' && (పంక్తి + 1) < చివర) {
        పంక్తి += 2;
        కొనసాగు;
      }
      ఒకవేళ(*పంక్తి == '"')
        లోపల_quote = FALSE;
    }
    లేకపోతే {
      ఒకవేళ(*పంక్తి == '"')
        లోపల_quote = TRUE;
      లేకపోతే ఒకవేళ(*పంక్తి == '{')
        తిరిగి పంక్తి;
    }
    పంక్తి++;
  }
  తిరిగి ఖాళీ;
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
స్థిర bool imap_matchresp(స్థిరరాశి అక్షరం *పంక్తి, size_t len, స్థిరరాశి అక్షరం *cmd)
{
  స్థిరరాశి అక్షరం *చివర = పంక్తి + len;
  size_t cmd_len = అక్షరశ్రేణి_పొడవు(cmd);

  /* Skip the untagged response marker */
  పంక్తి += 2;

  /* Do we have a number after the marker? */
  ఒకవేళ(పంక్తి < చివర && ISDIGIT(*పంక్తి)) {
    /* Skip the number */
    చేయి
      పంక్తి++;
    అయితే(పంక్తి < చివర && ISDIGIT(*పంక్తి));

    /* Do we have the space character? */
    ఒకవేళ(పంక్తి == చివర || *పంక్తి != ' ')
      తిరిగి FALSE;

    పంక్తి++;
  }

  /* Does the command name match and is it followed by a space character or at
     the end of line? */
  ఒకవేళ(పంక్తి + cmd_len <= చివర && curl_strnequal(పంక్తి, cmd, cmd_len) &&
     (పంక్తి[cmd_len] == ' ' || పంక్తి + cmd_len + 2 == చివర))
    తిరిగి TRUE;

  తిరిగి FALSE;
}

/***********************************************************************
 *
 * imap_endofresp()
 *
 * Checks whether the given string is a valid tagged, untagged or continuation
 * response which can be processed by the response handler.
 */
స్థిర bool imap_endofresp(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం connectdata *conn,
                           స్థిరరాశి అక్షరం *పంక్తి, size_t len, పూర్ణాంకం *resp)
{
  నిర్మాణం imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  నిర్మాణం IMAP *imap = Curl_meta_get(దత్తాంశం, CURL_META_IMAP_EASY);
  స్థిరరాశి అక్షరం *id;
  size_t id_len;

  DEBUGASSERT(imapc);
  DEBUGASSERT(imap);
  ఒకవేళ(!imapc || !imap)
    తిరిగి FALSE;

  /* Do we have a tagged command response? */
  id = imapc->resptag;
  id_len = అక్షరశ్రేణి_పొడవు(id);
  ఒకవేళ(len >= id_len + 1 && !memcmp(id, పంక్తి, id_len) && పంక్తి[id_len] == ' ') {
    పంక్తి += id_len + 1;
    len -= id_len + 1;

    ఒకవేళ(len >= 2 && !memcmp(పంక్తి, "OK", 2))
      *resp = IMAP_RESP_OK;
    లేకపోతే ఒకవేళ(len >= 7 && !memcmp(పంక్తి, "PREAUTH", 7))
      *resp = IMAP_RESP_PREAUTH;
    లేకపోతే
      *resp = IMAP_RESP_NOT_OK;

    తిరిగి TRUE;
  }

  /* Do we have an untagged command response? */
  ఒకవేళ(len >= 2 && !memcmp("* ", పంక్తి, 2)) {
    ఎంపిక(imapc->state) {
    /* States which are interested in untagged responses */
    స్థితి IMAP_CAPABILITY:
      ఒకవేళ(!imap_matchresp(పంక్తి, len, "CAPABILITY"))
        తిరిగి FALSE;
      ఆపు;

    స్థితి IMAP_LIST:
      ఒకవేళ((!imap->custom && !imap_matchresp(పంక్తి, len, "LIST")) ||
         (imap->custom && !imap_matchresp(పంక్తి, len, imap->custom) &&
          (!curl_strequal(imap->custom, "STORE") ||
           !imap_matchresp(పంక్తి, len, "FETCH")) &&
          !curl_strequal(imap->custom, "SELECT") &&
          !curl_strequal(imap->custom, "EXAMINE") &&
          !curl_strequal(imap->custom, "SEARCH") &&
          !curl_strequal(imap->custom, "EXPUNGE") &&
          !curl_strequal(imap->custom, "LSUB") &&
          !curl_strequal(imap->custom, "UID") &&
          !curl_strequal(imap->custom, "GETQUOTAROOT") &&
          !curl_strequal(imap->custom, "NOOP")))
        తిరిగి FALSE;
      ఆపు;

    స్థితి IMAP_SELECT:
      /* SELECT is special in that its untagged responses do not have a
         common prefix so accept anything! */
      ఆపు;

    స్థితి IMAP_FETCH:
      ఒకవేళ(!imap_matchresp(పంక్తి, len, "FETCH"))
        తిరిగి FALSE;
      ఆపు;

    స్థితి IMAP_శోధన:
      ఒకవేళ(!imap_matchresp(పంక్తి, len, "SEARCH"))
        తిరిగి FALSE;
      ఆపు;

    /* Ignore other untagged responses */
    అప్రమేయ:
      తిరిగి FALSE;
    }

    *resp = '*';
    తిరిగి TRUE;
  }

  /* Do we have a continuation response? This should be a + symbol followed by
     a space and optionally some text as per RFC-3501 for the AUTHENTICATE and
     APPEND commands and as outlined in Section 4. Examples of RFC-4959 but
     some email servers ignore this and only send a single + instead. */
  ఒకవేళ(!imap->custom && ((len == 3 && పంక్తి[0] == '+') ||
                       (len >= 2 && !memcmp("+ ", పంక్తి, 2)))) {
    ఎంపిక(imapc->state) {
    /* States which are interested in continuation responses */
    స్థితి IMAP_AUTHENTICATE:
    స్థితి IMAP_APPEND:
      *resp = '+';
      ఆపు;

    అప్రమేయ:
      failf(దత్తాంశం, "Unexpected continuation response");
      *resp = -1;
      ఆపు;
    }

    తిరిగి TRUE;
  }

  తిరిగి FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * imap_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
స్థిర CURLcode imap_get_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం bufref *బహిర్గతం)
{
  నిర్మాణం imap_conn *imapc =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_IMAP_CONN);
  అక్షరం *సందేశం_2;
  size_t len;

  ఒకవేళ(!imapc)
    తిరిగి CURLE_FAILED_INIT;

  సందేశం_2 = curlx_dyn_ptr(&imapc->pp.recvbuf);
  len = imapc->pp.nfinal;
  ఒకవేళ(len > 2) {
    /* Find the start of the message */
    len -= 2;
    కొరకు(సందేశం_2 += 2; ISBLANK(*సందేశం_2); సందేశం_2++, len--)
      ;

    /* Find the end of the message */
    అయితే(len--)
      ఒకవేళ(!ISNEWLINE(సందేశం_2[len]) && !ISBLANK(సందేశం_2[len]))
        ఆపు;

    /* Terminate the message */
    సందేశం_2[++len] = '\0';
    Curl_bufref_సమితి(బహిర్గతం, సందేశం_2, len, ఖాళీ);
  }
  లేకపోతే
    /* junk input => zero length output */
    Curl_bufref_సమితి(బహిర్గతం, "", 0, ఖాళీ);

  తిరిగి CURLE_OK;
}

/***********************************************************************
 *
 * imap_state()
 *
 * This is the ONLY way to change IMAP state!
 */
స్థిర శూన్య imap_state(నిర్మాణం Curl_easy *దత్తాంశం,
                       నిర్మాణం imap_conn *imapc,
                       imapstate newstate)
{
#ఒకవేళ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి పేర్లు[] = {
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
    /*చివరి*/
  };

  ఒకవేళ(imapc->state != newstate)
    infof(దత్తాంశం, "IMAP %p state change from %s to %s",
          (శూన్య *)imapc, పేర్లు[imapc->state], పేర్లు[newstate]);
#లేకపోతే
  (శూన్య)దత్తాంశం;
#అయితే_ముగింపు
  imapc->state = newstate;
}

/***********************************************************************
 *
 * imap_perform_capability()
 *
 * Sends the CAPABILITY command in order to obtain a list of server side
 * supported capabilities.
 */
స్థిర CURLcode imap_perform_capability(నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం imap_conn *imapc)
{
  CURLcode ఫలితం = CURLE_OK;

  imapc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  imapc->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  imapc->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPABILITY command */
  ఫలితం = imap_sendf(దత్తాంశం, imapc, "CAPABILITY");

  ఒకవేళ(!ఫలితం)
    imap_state(దత్తాంశం, imapc, IMAP_CAPABILITY);

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * imap_perform_starttls()
 *
 * Sends the STARTTLS command to start the upgrade to TLS.
 */
స్థిర CURLcode imap_perform_starttls(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం imap_conn *imapc)
{
  /* Send the STARTTLS command */
  CURLcode ఫలితం = imap_sendf(దత్తాంశం, imapc, "STARTTLS");

  ఒకవేళ(!ఫలితం)
    imap_state(దత్తాంశం, imapc, IMAP_STARTTLS);

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * imap_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
స్థిర CURLcode imap_perform_upgrade_tls(నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం imap_conn *imapc,
                                         నిర్మాణం connectdata *conn)
{
#నిర్వచితమైతే USE_SSL
  /* Start the SSL connection */
  CURLcode ఫలితం;
  bool ssldone = FALSE;

  ఒకవేళ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    ఫలితం = Curl_ssl_cfilter_కూడు(
      దత్తాంశం, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_imaps;
  }

  DEBUGASSERT(!imapc->ssldone);
  ఫలితం = Curl_conn_connect(దత్తాంశం, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(దత్తాంశం, "imap_perform_upgrade_tls, connect -> %d, %d",
               (పూర్ణాంకం)ఫలితం, ssldone));
  ఒకవేళ(!ఫలితం && ssldone) {
    imapc->ssldone = ssldone;
    /* perform CAPA now, changes imapc->state out of IMAP_UPGRADETLS */
    ఫలితం = imap_perform_capability(దత్తాంశం, imapc);
  }
బహిర్గతం:
  తిరిగి ఫలితం;
#లేకపోతే
  (శూన్య)దత్తాంశం;
  (శూన్య)imapc;
  (శూన్య)conn;
  తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
}

/***********************************************************************
 *
 * imap_perform_login()
 *
 * Sends a clear text LOGIN command to authenticate with.
 */
స్థిర CURLcode imap_perform_login(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం imap_conn *imapc,
                                   నిర్మాణం connectdata *conn)
{
  CURLcode ఫలితం = CURLE_OK;
  అక్షరం *user;
  అక్షరం *passwd;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  ఒకవేళ(!conn->creds) {
    imap_state(దత్తాంశం, imapc, IMAP_STOP);

    తిరిగి ఫలితం;
  }

  /* Make sure the username and password are in the correct atom format */
  user = imap_atom(Curl_creds_user(conn->creds), FALSE);
  passwd = imap_atom(Curl_creds_passwd(conn->creds), FALSE);

  /* Send the LOGIN command */
  ఫలితం = imap_sendf(దత్తాంశం, imapc, "LOGIN %s %s", user ? user : "",
                      passwd ? passwd : "");

  curlx_free(user);
  curlx_strzero(passwd);
  curlx_free(passwd);

  ఒకవేళ(!ఫలితం)
    imap_state(దత్తాంశం, imapc, IMAP_LOGIN);

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * imap_perform_authenticate()
 *
 * Sends an AUTHENTICATE command allowing the client to login with the given
 * SASL authentication mechanism.
 */
స్థిర CURLcode imap_perform_authenticate(నిర్మాణం Curl_easy *దత్తాంశం,
                                          స్థిరరాశి అక్షరం *mech,
                                          స్థిరరాశి నిర్మాణం bufref *initresp)
{
  నిర్మాణం imap_conn *imapc =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_IMAP_CONN);
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం *ir = Curl_bufref_ptr(initresp);

  ఒకవేళ(!imapc)
    తిరిగి CURLE_FAILED_INIT;
  ఒకవేళ(ir) {
    /* Send the AUTHENTICATE command with the initial response */
    ఫలితం = imap_sendf(దత్తాంశం, imapc, "AUTHENTICATE %s %s",
                        mech, *ir ? ir : "=");
  }
  లేకపోతే {
    /* Send the AUTHENTICATE command */
    ఫలితం = imap_sendf(దత్తాంశం, imapc, "AUTHENTICATE %s", mech);
  }

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * imap_continue_authenticate()
 *
 * Sends SASL continuation data.
 */
స్థిర CURLcode imap_continue_authenticate(నిర్మాణం Curl_easy *దత్తాంశం,
                                           స్థిరరాశి అక్షరం *mech,
                                           స్థిరరాశి నిర్మాణం bufref *resp)
{
  నిర్మాణం imap_conn *imapc =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_IMAP_CONN);

  (శూన్య)mech;
  ఒకవేళ(!imapc)
    తిరిగి CURLE_FAILED_INIT;
  తిరిగి Curl_pp_sendf(దత్తాంశం, &imapc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * imap_cancel_authenticate()
 *
 * Sends SASL cancellation.
 */
స్థిర CURLcode imap_cancel_authenticate(నిర్మాణం Curl_easy *దత్తాంశం,
                                         స్థిరరాశి అక్షరం *mech)
{
  నిర్మాణం imap_conn *imapc =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_IMAP_CONN);

  (శూన్య)mech;
  ఒకవేళ(!imapc)
    తిరిగి CURLE_FAILED_INIT;
  తిరిగి Curl_pp_sendf(దత్తాంశం, &imapc->pp, "*");
}

/***********************************************************************
 *
 * imap_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to clear text should a common
 * mechanism not be available between the client and server.
 */
స్థిర CURLcode imap_perform_authentication(నిర్మాణం Curl_easy *దత్తాంశం,
                                            నిర్మాణం imap_conn *imapc)
{
  CURLcode ఫలితం = CURLE_OK;
  saslprogress పురోగతి;

  /* Check if already authenticated OR if there is enough data to authenticate
     with and end the connect phase if we do not */
  ఒకవేళ(imapc->preauth ||
     !Curl_sasl_can_authenticate(&imapc->sasl, దత్తాంశం)) {
    imap_state(దత్తాంశం, imapc, IMAP_STOP);
    తిరిగి ఫలితం;
  }

  /* Calculate the SASL login details */
  ఫలితం = Curl_sasl_ప్రారంభం(&imapc->sasl, దత్తాంశం, (bool)imapc->ir_supported,
                           &పురోగతి);
  ఒకవేళ(!ఫలితం) {
    ఒకవేళ(పురోగతి == SASL_INPROGRESS)
      imap_state(దత్తాంశం, imapc, IMAP_AUTHENTICATE);
    లేకపోతే ఒకవేళ(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
      /* Perform clear text authentication */
      ఫలితం = imap_perform_login(దత్తాంశం, imapc, దత్తాంశం->conn);
    లేకపోతే
      ఫలితం = Curl_sasl_is_blocked(&imapc->sasl, దత్తాంశం);
  }

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * imap_perform_list()
 *
 * Sends a LIST command or an alternative custom request.
 */
స్థిర CURLcode imap_perform_list(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం imap_conn *imapc,
                                  నిర్మాణం IMAP *imap)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(imap->custom)
    /* Send the custom request */
    ఫలితం = imap_sendf(దత్తాంశం, imapc, "%s%s", imap->custom,
                        imap->custom_params ? imap->custom_params : "");
  లేకపోతే {
    /* Make sure the mailbox is in the correct atom format if necessary */
    అక్షరం *mailbox = imap->mailbox ? imap_atom(imap->mailbox, TRUE)
                                  : curlx_strdup("");
    ఒకవేళ(!mailbox)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    /* Send the LIST command */
    ఫలితం = imap_sendf(దత్తాంశం, imapc, "LIST \"%s\" *", mailbox);

    curlx_free(mailbox);
  }

  ఒకవేళ(!ఫలితం)
    imap_state(దత్తాంశం, imapc, IMAP_LIST);

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * imap_perform_select()
 *
 * Sends a SELECT command to ask the server to change the selected mailbox.
 */
స్థిర CURLcode imap_perform_select(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం imap_conn *imapc,
                                    నిర్మాణం IMAP *imap)
{
  CURLcode ఫలితం = CURLE_OK;
  అక్షరం *mailbox;

  /* Invalidate old information as we are switching mailboxes */
  curlx_safefree(imapc->mailbox);
  imapc->mb_uidvalidity_సమితి = FALSE;

  /* Check we have a mailbox */
  ఒకవేళ(!imap->mailbox) {
    failf(దత్తాంశం, "Cannot SELECT without a mailbox.");
    తిరిగి CURLE_URL_MALFORMAT;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  ఒకవేళ(!mailbox)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* Send the SELECT command */
  ఫలితం = imap_sendf(దత్తాంశం, imapc, "SELECT %s", mailbox);

  curlx_free(mailbox);

  ఒకవేళ(!ఫలితం)
    imap_state(దత్తాంశం, imapc, IMAP_SELECT);

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * imap_perform_fetch()
 *
 * Sends a FETCH command to initiate the download of a message.
 */
స్థిర CURLcode imap_perform_fetch(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం imap_conn *imapc,
                                   నిర్మాణం IMAP *imap)
{
  CURLcode ఫలితం = CURLE_OK;
  /* Check we have a UID */
  ఒకవేళ(imap->uid) {

    /* Send the FETCH command */
    ఒకవేళ(imap->partial)
      ఫలితం = imap_sendf(దత్తాంశం, imapc, "UID FETCH %s BODY[%s]<%s>",
                          imap->uid, imap->section ? imap->section : "",
                          imap->partial);
    లేకపోతే
      ఫలితం = imap_sendf(దత్తాంశం, imapc, "UID FETCH %s BODY[%s]",
                          imap->uid, imap->section ? imap->section : "");
  }
  లేకపోతే ఒకవేళ(imap->mindex) {
    /* Send the FETCH command */
    ఒకవేళ(imap->partial)
      ఫలితం = imap_sendf(దత్తాంశం, imapc, "FETCH %s BODY[%s]<%s>",
                          imap->mindex, imap->section ? imap->section : "",
                          imap->partial);
    లేకపోతే
      ఫలితం = imap_sendf(దత్తాంశం, imapc, "FETCH %s BODY[%s]",
                          imap->mindex, imap->section ? imap->section : "");
  }
  లేకపోతే {
    failf(దత్తాంశం, "Cannot FETCH without a UID.");
    తిరిగి CURLE_URL_MALFORMAT;
  }
  ఒకవేళ(!ఫలితం)
    imap_state(దత్తాంశం, imapc, IMAP_FETCH);

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * imap_perform_append()
 *
 * Sends an APPEND command to initiate the upload of a message.
 */
స్థిర CURLcode imap_perform_append(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం imap_conn *imapc,
                                    నిర్మాణం IMAP *imap)
{
  CURLcode ఫలితం = CURLE_OK;
  అక్షరం *mailbox;
  నిర్మాణం dynbuf flags;

  /* Check we have a mailbox */
  ఒకవేళ(!imap->mailbox) {
    failf(దత్తాంశం, "Cannot APPEND without a mailbox.");
    తిరిగి CURLE_URL_MALFORMAT;
  }

#నిర్వచితం_కాకపోతే CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  ఒకవేళ(IS_MIME_POST(దత్తాంశం)) {
    curl_mimepart *postp = దత్తాంశం->సమితి.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(గుర్తులేని పూర్ణాంకం)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_శీర్షికలు(postp, దత్తాంశం->సమితి.శీర్షికలు, 0);
    ఫలితం = Curl_mime_prepare_శీర్షికలు(దత్తాంశం, postp, ఖాళీ,
                                       ఖాళీ, MIMESTRATEGY_MAIL);

    ఒకవేళ(!ఫలితం && !Curl_checkheaders(దత్తాంశం, STRCONST("Mime-Version")))
      ఫలితం = Curl_mime_కూడు_శీర్షిక(&postp->curlheaders, "Mime-Version: 1.0");
    ఒకవేళ(!ఫలితం)
      ఫలితం = Curl_creader_సమితి_mime(దత్తాంశం, postp);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  లేకపోతే
#అయితే_ముగింపు
  {
    ఫలితం = Curl_creader_సమితి_fread(దత్తాంశం, దత్తాంశం->state.infilesize);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  /* Check we know the size of the upload. This takes all readers
   * into account. Especially crlf conversions which make the size
   * unpredictable, e.g. -1. */
  దత్తాంశం->state.infilesize = Curl_creader_మొత్తం_నిడివి(దత్తాంశం);
  ఒకవేళ(దత్తాంశం->state.infilesize < 0) {
    ఒకవేళ(దత్తాంశం->సమితి.crlf)
      failf(దత్తాంశం, "Cannot APPEND with CRLF conversion making size unknown");
    లేకపోతే
      failf(దత్తాంశం, "Cannot APPEND with unknown input file size");
    తిరిగి CURLE_UPLOAD_FAILED;
  }

  /* Make sure the mailbox is in the correct atom format */
  mailbox = imap_atom(imap->mailbox, FALSE);
  ఒకవేళ(!mailbox)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* Generate flags string and send the APPEND command */
  curlx_dyn_init(&flags, 100);
  ఒకవేళ(దత్తాంశం->సమితి.upload_flags) {
    పూర్ణాంకం i;
    నిర్మాణం ulbits ulflag[] = {
      { CURLULFLAG_ANSWERED, "Answered" },
      { CURLULFLAG_DELETED, "Deleted" },
      { CURLULFLAG_DRAFT, "Draft" },
      { CURLULFLAG_FLAGGED, "Flagged" },
      { CURLULFLAG_చూసిన, "Seen" },
      { 0, ఖాళీ }
    };

    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    ఒకవేళ(curlx_dyn_కూడు(&flags, " (")) {
      వెళ్ళు cleanup;
    }

    కొరకు(i = 0; ulflag[i].బిట్టు_2; i++) {
      ఒకవేళ(దత్తాంశం->సమితి.upload_flags & ulflag[i].బిట్టు_2 &&
         ((curlx_dyn_len(&flags) > 2 && curlx_dyn_కూడు(&flags, " ")) ||
          curlx_dyn_కూడు(&flags, "\\") ||
          curlx_dyn_కూడు(&flags, ulflag[i].flag)))
        వెళ్ళు cleanup;
    }

    ఒకవేళ(curlx_dyn_కూడు(&flags, ")"))
      వెళ్ళు cleanup;
  }
  లేకపోతే ఒకవేళ(curlx_dyn_కూడు(&flags, ""))
    వెళ్ళు cleanup;

  ఫలితం = imap_sendf(దత్తాంశం, imapc, "APPEND %s%s {%" FMT_OFF_T "}",
                      mailbox, curlx_dyn_ptr(&flags), దత్తాంశం->state.infilesize);

cleanup:
  curlx_dyn_free(&flags);
  curlx_free(mailbox);

  ఒకవేళ(!ఫలితం)
    imap_state(దత్తాంశం, imapc, IMAP_APPEND);

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * imap_perform_search()
 *
 * Sends a SEARCH command.
 */
స్థిర CURLcode imap_perform_శోధన(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం imap_conn *imapc,
                                    నిర్మాణం IMAP *imap)
{
  CURLcode ఫలితం = CURLE_OK;

  /* Check we have a query string */
  ఒకవేళ(!imap->query) {
    failf(దత్తాంశం, "Cannot SEARCH without a query string.");
    తిరిగి CURLE_URL_MALFORMAT;
  }

  /* Send the SEARCH command */
  ఫలితం = imap_sendf(దత్తాంశం, imapc, "SEARCH %s", imap->query);

  ఒకవేళ(!ఫలితం)
    imap_state(దత్తాంశం, imapc, IMAP_శోధన);

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * imap_perform_logout()
 *
 * Performs the logout action prior to sclose() being called.
 */
స్థిర CURLcode imap_perform_logout(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం imap_conn *imapc)
{
  /* Send the LOGOUT command */
  CURLcode ఫలితం = imap_sendf(దత్తాంశం, imapc, "LOGOUT");

  ఒకవేళ(!ఫలితం)
    imap_state(దత్తాంశం, imapc, IMAP_LOGOUT);

  తిరిగి ఫలితం;
}

/* For the initial server greeting */
స్థిర CURLcode imap_state_servergreet_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                            నిర్మాణం imap_conn *imapc,
                                            పూర్ణాంకం imapcode,
                                            imapstate instate)
{
  (శూన్య)instate;

  ఒకవేళ(imapcode == IMAP_RESP_PREAUTH) {
    /* PREAUTH */
    imapc->preauth = TRUE;
    infof(దత్తాంశం, "PREAUTH connection, already authenticated");
  }
  లేకపోతే ఒకవేళ(imapcode != IMAP_RESP_OK) {
    failf(దత్తాంశం, "Got unexpected imap-server response");
    తిరిగి CURLE_WEIRD_SERVER_జవాబు;
  }

  తిరిగి imap_perform_capability(దత్తాంశం, imapc);
}

/* For CAPABILITY responses */
స్థిర CURLcode imap_state_capability_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                           నిర్మాణం imap_conn *imapc,
                                           పూర్ణాంకం imapcode,
                                           imapstate instate)
{
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం *పంక్తి = curlx_dyn_ptr(&imapc->pp.recvbuf);

  (శూన్య)instate;

  /* Do we have an untagged response? */
  ఒకవేళ(imapcode == '*') {
    పంక్తి += 2;

    /* Loop through the data line */
    కొరకు(;;) {
      size_t wordlen;
      అయితే(*పంక్తి && (ISBLANK(*పంక్తి) || ISNEWLINE(*పంక్తి)))
        పంక్తి++;

      ఒకవేళ(!*పంక్తి)
        ఆపు;

      /* Extract the word */
      కొరకు(wordlen = 0; పంక్తి[wordlen] && !ISBLANK(పంక్తి[wordlen]) &&
                       !ISNEWLINE(పంక్తి[wordlen]);)
        wordlen++;

      /* Does the server support the STARTTLS capability? */
      ఒకవేళ(wordlen == 8 && curl_strnequal(పంక్తి, "STARTTLS", 8))
        imapc->tls_supported = TRUE;

      /* Has the server explicitly disabled clear text authentication? */
      లేకపోతే ఒకవేళ(wordlen == 13 && curl_strnequal(పంక్తి, "LOGINDISABLED", 13))
        imapc->login_disabled = TRUE;

      /* Does the server support the SASL-IR capability? */
      లేకపోతే ఒకవేళ(wordlen == 7 && curl_strnequal(పంక్తి, "SASL-IR", 7))
        imapc->ir_supported = TRUE;

      /* Do we have a SASL based authentication mechanism? */
      లేకపోతే ఒకవేళ(wordlen > 5 && curl_strnequal(పంక్తి, "AUTH=", 5)) {
        size_t llen;
        గుర్తులేని పొట్టి mechbit;

        పంక్తి += 5;
        wordlen -= 5;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(పంక్తి, wordlen, &llen);
        ఒకవేళ(mechbit && llen == wordlen)
          imapc->sasl.authmechs |= mechbit;
      }

      పంక్తి += wordlen;
    }
  }
  లేకపోతే ఒకవేళ(దత్తాంశం->సమితి.use_ssl && !Curl_conn_is_ssl(దత్తాంశం->conn, FIRSTSOCKET)) {
    /* PREAUTH is not compatible with STARTTLS. */
    ఒకవేళ(imapcode == IMAP_RESP_OK && imapc->tls_supported && !imapc->preauth) {
      /* Switch to TLS connection now */
      ఫలితం = imap_perform_starttls(దత్తాంశం, imapc);
    }
    లేకపోతే ఒకవేళ(దత్తాంశం->సమితి.use_ssl <= CURLUSESSL_TRY)
      ఫలితం = imap_perform_authentication(దత్తాంశం, imapc);
    లేకపోతే {
      failf(దత్తాంశం, "STARTTLS not available.");
      ఫలితం = CURLE_USE_SSL_FAILED;
    }
  }
  లేకపోతే
    ఫలితం = imap_perform_authentication(దత్తాంశం, imapc);

  తిరిగి ఫలితం;
}

/* For STARTTLS responses */
స్థిర CURLcode imap_state_starttls_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం imap_conn *imapc,
                                         పూర్ణాంకం imapcode,
                                         imapstate instate)
{
  CURLcode ఫలితం = CURLE_OK;

  (శూన్య)instate;

  /* Pipelining in response is forbidden. */
  ఒకవేళ(imapc->pp.అధికప్రవాహం)
    తిరిగి CURLE_WEIRD_SERVER_జవాబు;

  ఒకవేళ(imapcode != IMAP_RESP_OK) {
    ఒకవేళ(దత్తాంశం->సమితి.use_ssl != CURLUSESSL_TRY) {
      failf(దత్తాంశం, "STARTTLS denied");
      ఫలితం = CURLE_USE_SSL_FAILED;
    }
    లేకపోతే
      ఫలితం = imap_perform_authentication(దత్తాంశం, imapc);
  }
  లేకపోతే
    imap_state(దత్తాంశం, imapc, IMAP_UPGRADETLS);

  తిరిగి ఫలితం;
}

/* For SASL authentication responses */
స్థిర CURLcode imap_state_auth_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం imap_conn *imapc,
                                     పూర్ణాంకం imapcode,
                                     imapstate instate)
{
  CURLcode ఫలితం = CURLE_OK;
  saslprogress పురోగతి;

  (శూన్య)instate;

  ఫలితం = Curl_sasl_continue(&imapc->sasl, దత్తాంశం, imapcode, &పురోగతి);
  ఒకవేళ(!ఫలితం)
    ఎంపిక(పురోగతి) {
    స్థితి SASL_పూర్తి:
      imap_state(దత్తాంశం, imapc, IMAP_STOP);  /* Authenticated */
      ఆపు;
    స్థితి SASL_IDLE:            /* No mechanism left after cancellation */
      ఒకవేళ(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
        /* Perform clear text authentication */
        ఫలితం = imap_perform_login(దత్తాంశం, imapc, దత్తాంశం->conn);
      లేకపోతే {
        failf(దత్తాంశం, "Authentication cancelled");
        ఫలితం = CURLE_LOGIN_DENIED;
      }
      ఆపు;
    అప్రమేయ:
      ఆపు;
    }

  తిరిగి ఫలితం;
}

/* For LOGIN responses */
స్థిర CURLcode imap_state_login_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం imap_conn *imapc,
                                      పూర్ణాంకం imapcode,
                                      imapstate instate)
{
  CURLcode ఫలితం = CURLE_OK;
  (శూన్య)instate;

  ఒకవేళ(imapcode != IMAP_RESP_OK) {
    failf(దత్తాంశం, "Access denied. %c", imapcode);
    ఫలితం = CURLE_LOGIN_DENIED;
  }
  లేకపోతే
    /* End of connect phase */
    imap_state(దత్తాంశం, imapc, IMAP_STOP);

  తిరిగి ఫలితం;
}

/* Detect IMAP listings vs. downloading a single email */
స్థిర bool is_custom_fetch_listing_సరిపోలిక(స్థిరరాశి అక్షరం *params)
{
  /* match " 1:* (FLAGS ..." or " 1,2,3 (FLAGS ..." */
  ఒకవేళ(*params++ != ' ')
    తిరిగి FALSE;

  అయితే(ISDIGIT(*params)) {
    params++;
    ఒకవేళ(*params == 0)
      తిరిగి FALSE;
  }
  ఒకవేళ(*params == ':')
    తిరిగి TRUE;
  ఒకవేళ(*params == ',')
    తిరిగి TRUE;
  తిరిగి FALSE;
}

స్థిర bool is_custom_fetch_listing(నిర్మాణం IMAP *imap)
{
  /* filter out "UID FETCH 1:* (FLAGS ..." queries to list emails */
  ఒకవేళ(!imap->custom)
    తిరిగి FALSE;
  లేకపోతే ఒకవేళ(curl_strequal(imap->custom, "FETCH") && imap->custom_params) {
    స్థిరరాశి అక్షరం *p = imap->custom_params;
    తిరిగి is_custom_fetch_listing_సరిపోలిక(p);
  }
  లేకపోతే ఒకవేళ(curl_strequal(imap->custom, "UID") && imap->custom_params &&
          curl_strnequal(imap->custom_params, " FETCH ", 7)) {
    స్థిరరాశి అక్షరం *p = imap->custom_params + 6;
    తిరిగి is_custom_fetch_listing_సరిపోలిక(p);
  }
  తిరిగి FALSE;
}

/* For LIST and SEARCH responses */
స్థిర CURLcode imap_state_listsearch_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                           నిర్మాణం imap_conn *imapc,
                                           పూర్ణాంకం imapcode,
                                           imapstate instate)
{
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం *పంక్తి = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  నిర్మాణం IMAP *imap = Curl_meta_get(దత్తాంశం, CURL_META_IMAP_EASY);

  DEBUGASSERT(imap);
  ఒకవేళ(!imap)
    తిరిగి CURLE_FAILED_INIT;
  (శూన్య)instate;

  ఒకవేళ(imapcode == '*' && is_custom_fetch_listing(imap)) {
    /* custom FETCH or UID FETCH for listing is not handled here */
  }
  లేకపోతే ఒకవేళ(imapcode == '*') {
    /* Check if this response contains a literal (e.g. FETCH responses with
       body data). Literal syntax is {size}\r\n */
    స్థిరరాశి అక్షరం *cr = memchr(పంక్తి, '\r', len);
    size_t పంక్తి_len = cr ? (size_t)(cr - పంక్తి) : len;
    స్థిరరాశి అక్షరం *ptr = imap_కనుగొను_literal(పంక్తి, పంక్తి_len);
    ఒకవేళ(ptr) {
      curl_off_t కొలత = 0;
      bool parsed = FALSE;
      ptr++;
      ఒకవేళ(!curlx_str_సంఖ్య(&ptr, &కొలత, CURL_OFF_T_MAX) &&
         !curlx_str_single(&ptr, '}'))
        parsed = TRUE;

      ఒకవేళ(parsed) {
        నిర్మాణం pingpong *pp = &imapc->pp;
        size_t బఫరు_len = curlx_dyn_len(&pp->recvbuf);
        size_t after_శీర్షిక = బఫరు_len - pp->nfinal;

        /* This is a literal response, setup to receive the body data */
        infof(దత్తాంశం, "Found %" FMT_OFF_T " bytes to download", కొలత);

        /* First write the header line */
        ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, పంక్తి, len);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;

        /* Handle data already in buffer after the header line */
        ఒకవేళ(after_శీర్షిక > 0) {
          /* There is already data in the buffer that is part of the literal
             body or subsequent responses */
          size_t chunk = after_శీర్షిక;

          /* Keep only the data after the header line */
          curlx_dyn_తోక(&pp->recvbuf, chunk);
          pp->nfinal = 0; /* done */

          /* Limit chunk to the literal size */
          ఒకవేళ((curl_off_t)chunk > కొలత)
            chunk = (size_t)కొలత;

          ఒకవేళ(chunk) {
            /* Write the literal body data */
            ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY,
                                       curlx_dyn_ptr(&pp->recvbuf), chunk);
            ఒకవేళ(ఫలితం)
              తిరిగి ఫలితం;
          }

          /* Handle remaining data in buffer (either more literal data or
             subsequent responses) */
          ఒకవేళ(after_శీర్షిక > chunk) {
            /* Keep the data after the literal body */
            pp->అధికప్రవాహం = after_శీర్షిక - chunk;
            curlx_dyn_తోక(&pp->recvbuf, pp->అధికప్రవాహం);
          }
          లేకపోతే {
            pp->అధికప్రవాహం = 0;
            curlx_dyn_reset(&pp->recvbuf);
          }
        }
        లేకపోతే {
          /* No data in buffer yet, reset overflow */
          pp->అధికప్రవాహం = 0;
        }

        ఒకవేళ((CURL_OFF_T_MAX - కొలత) < (curl_off_t)len)
          /* unlikely to actually be a transfer this big, but avoid integer
అధికప్రవాహం*/
          కొలత = CURL_OFF_T_MAX;
        లేకపోతే
          కొలత += len;

        /* Progress size includes both header line and literal body */
        Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, కొలత);

        ఒకవేళ(దత్తాంశం->req.bytecount == కొలత)
          /* All data already transferred (header + literal body) */
          Curl_xfer_అమరిక_nop(దత్తాంశం);
        లేకపోతే {
          /* Setup to receive the literal body data.
             maxdownload and transfer size include both header line and
             literal body */
          దత్తాంశం->req.maxdownload = కొలత;
          Curl_xfer_అమరిక_recv(దత్తాంశం, FIRSTSOCKET, కొలత);
        }
        /* End of DO phase */
        imap_state(దత్తాంశం, imapc, IMAP_STOP);
      }
      లేకపోతే {
        /* Failed to parse literal, write the line */
        ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, పంక్తి, len);
      }
    }
    లేకపోతే {
      /* No literal, write the line as-is */
      ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, పంక్తి, len);
    }
  }
  లేకపోతే ఒకవేళ(imapcode != IMAP_RESP_OK)
    ఫలితం = CURLE_QUOTE_ERROR;
  లేకపోతే
    /* End of DO phase */
    imap_state(దత్తాంశం, imapc, IMAP_STOP);

  తిరిగి ఫలితం;
}

/* For SELECT responses */
స్థిర CURLcode imap_state_select_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం imap_conn *imapc,
                                       నిర్మాణం IMAP *imap,
                                       పూర్ణాంకం imapcode,
                                       imapstate instate)
{
  CURLcode ఫలితం = CURLE_OK;
  (శూన్య)instate;

  ఒకవేళ(imapcode == '*') {
    /* See if this is an UIDVALIDITY response */
    స్థిరరాశి అక్షరం *పంక్తి = curlx_dyn_ptr(&imapc->pp.recvbuf);
    size_t len = curlx_dyn_len(&imapc->pp.recvbuf);
    ఒకవేళ((len >= 18) && checkprefix("OK [UIDVALIDITY ", &పంక్తి[2])) {
      curl_off_t విలువ;
      స్థిరరాశి అక్షరం *p = &పంక్తి[2] + CURL_CSTRLEN("OK [UIDVALIDITY ");
      ఒకవేళ(!curlx_str_సంఖ్య(&p, &విలువ, UINT_MAX)) {
        imapc->mb_uidvalidity = (గుర్తులేని పూర్ణాంకం)విలువ;
        imapc->mb_uidvalidity_సమితి = TRUE;
      }
    }
  }
  లేకపోతే ఒకవేళ(imapcode == IMAP_RESP_OK) {
    /* Check if the UIDVALIDITY has been specified and matches */
    ఒకవేళ(imap->uidvalidity_సమితి && imapc->mb_uidvalidity_సమితి &&
       (imap->uidvalidity != imapc->mb_uidvalidity)) {
      failf(దత్తాంశం, "Mailbox UIDVALIDITY has changed");
      ఫలితం = CURLE_REMOTE_FILE_NOT_దొరికింది;
    }
    లేకపోతే {
      /* Note the currently opened mailbox on this connection */
      DEBUGASSERT(!imapc->mailbox);
      imapc->mailbox = curlx_strdup(imap->mailbox);
      ఒకవేళ(!imapc->mailbox)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

      ఒకవేళ(imap->custom)
        ఫలితం = imap_perform_list(దత్తాంశం, imapc, imap);
      లేకపోతే ఒకవేళ(imap->query)
        ఫలితం = imap_perform_శోధన(దత్తాంశం, imapc, imap);
      లేకపోతే
        ఫలితం = imap_perform_fetch(దత్తాంశం, imapc, imap);
    }
  }
  లేకపోతే {
    failf(దత్తాంశం, "Select failed");
    ఫలితం = CURLE_LOGIN_DENIED;
  }

  తిరిగి ఫలితం;
}

/* For the (first line of the) FETCH responses */
స్థిర CURLcode imap_state_fetch_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం imap_conn *imapc,
                                      పూర్ణాంకం imapcode,
                                      imapstate instate)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం pingpong *pp = &imapc->pp;
  స్థిరరాశి అక్షరం *ptr = curlx_dyn_ptr(&imapc->pp.recvbuf);
  size_t len = imapc->pp.nfinal;
  bool parsed = FALSE;
  curl_off_t కొలత = 0;

  (శూన్య)instate;

  ఒకవేళ(imapcode != '*') {
    Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, -1);
    imap_state(దత్తాంశం, imapc, IMAP_STOP);
    తిరిగి CURLE_REMOTE_FILE_NOT_దొరికింది;
  }

  /* Something like this is received "* 1 FETCH (BODY[TEXT] {2021}\r" so parse
     the continuation data contained within the curly brackets */
  ptr = imap_కనుగొను_literal(ptr, len);
  ఒకవేళ(ptr) {
    ptr++;
    ఒకవేళ(!curlx_str_సంఖ్య(&ptr, &కొలత, CURL_OFF_T_MAX) &&
       !curlx_str_single(&ptr, '}'))
      parsed = TRUE;
  }

  ఒకవేళ(parsed) {
    infof(దత్తాంశం, "Found %" FMT_OFF_T " bytes to download", కొలత);
    Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, కొలత);

    ఒకవేళ(pp->అధికప్రవాహం) {
      /* At this point there is a data in the receive buffer that is body
         content, send it as body and then skip it. Do note that there may
         even be additional "headers" after the body. */
      size_t chunk = pp->అధికప్రవాహం;

      /* keep only the overflow */
      curlx_dyn_తోక(&pp->recvbuf, chunk);
      pp->nfinal = 0; /* done */

      ఒకవేళ((curl_off_t)chunk > కొలత)
        /* The conversion from curl_off_t to size_t is always fine here */
        chunk = (size_t)కొలత;

      ఒకవేళ(!chunk) {
        /* no size, we are done with the data */
        imap_state(దత్తాంశం, imapc, IMAP_STOP);
        తిరిగి CURLE_OK;
      }
      ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY,
                                 curlx_dyn_ptr(&pp->recvbuf), chunk);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;

      infof(దత్తాంశం, "Written %zu bytes, %" FMT_OFF_T
            " bytes are left for transfer", chunk, (curl_off_t)(కొలత - chunk));

      /* Have we used the entire overflow or part of it?*/
      ఒకవేళ(pp->అధికప్రవాహం > chunk) {
        /* remember the remaining trailing overflow data */
        pp->అధికప్రవాహం -= chunk;
        curlx_dyn_తోక(&pp->recvbuf, pp->అధికప్రవాహం);
      }
      లేకపోతే {
        pp->అధికప్రవాహం = 0; /* handled */
        /* Free the cache */
        curlx_dyn_reset(&pp->recvbuf);
      }
    }

    ఒకవేళ(దత్తాంశం->req.bytecount == కొలత)
      /* The entire data is already transferred! */
      Curl_xfer_అమరిక_nop(దత్తాంశం);
    లేకపోతే {
      /* IMAP download */
      దత్తాంశం->req.maxdownload = కొలత;
      Curl_xfer_అమరిక_recv(దత్తాంశం, FIRSTSOCKET, కొలత);
    }
  }
  లేకపోతే {
    /* We do not know how to parse this line */
    failf(దత్తాంశం, "Failed to parse FETCH response.");
    ఫలితం = CURLE_WEIRD_SERVER_జవాబు;
  }

  /* End of DO phase */
  imap_state(దత్తాంశం, imapc, IMAP_STOP);

  తిరిగి ఫలితం;
}

/* For final FETCH responses performed after the download */
స్థిర CURLcode imap_state_fetch_final_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                            నిర్మాణం imap_conn *imapc,
                                            పూర్ణాంకం imapcode,
                                            imapstate instate)
{
  CURLcode ఫలితం = CURLE_OK;

  (శూన్య)instate;

  ఒకవేళ(imapcode != IMAP_RESP_OK)
    ఫలితం = CURLE_WEIRD_SERVER_జవాబు;
  లేకపోతే
    /* End of DONE phase */
    imap_state(దత్తాంశం, imapc, IMAP_STOP);

  తిరిగి ఫలితం;
}

/* For APPEND responses */
స్థిర CURLcode imap_state_append_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం imap_conn *imapc,
                                       పూర్ణాంకం imapcode,
                                       imapstate instate)
{
  CURLcode ఫలితం = CURLE_OK;
  (శూన్య)instate;

  ఒకవేళ(imapcode != '+') {
    ఫలితం = CURLE_UPLOAD_FAILED;
  }
  లేకపోతే {
    /* Set the progress upload size */
    Curl_pgrs_సమితి_Upload_కొలత(దత్తాంశం, దత్తాంశం->state.infilesize);

    /* IMAP upload */
    Curl_xfer_అమరిక_send(దత్తాంశం, FIRSTSOCKET);

    /* End of DO phase */
    imap_state(దత్తాంశం, imapc, IMAP_STOP);
  }

  తిరిగి ఫలితం;
}

/* For final APPEND responses performed after the upload */
స్థిర CURLcode imap_state_append_final_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం imap_conn *imapc,
                                             పూర్ణాంకం imapcode,
                                             imapstate instate)
{
  CURLcode ఫలితం = CURLE_OK;

  (శూన్య)instate;

  ఒకవేళ(imapcode != IMAP_RESP_OK)
    ఫలితం = CURLE_UPLOAD_FAILED;
  లేకపోతే
    /* End of DONE phase */
    imap_state(దత్తాంశం, imapc, IMAP_STOP);

  తిరిగి ఫలితం;
}

స్థిర CURLcode imap_pp_statemachine(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం connectdata *conn)
{
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం imapcode;
  నిర్మాణం imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  నిర్మాణం IMAP *imap = Curl_meta_get(దత్తాంశం, CURL_META_IMAP_EASY);
  నిర్మాణం pingpong *pp;
  size_t nread = 0;

  ఒకవేళ(!imapc || !imap)
    తిరిగి CURLE_FAILED_INIT;
  pp = &imapc->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not IMAP */
upgrade_tls:
  ఒకవేళ(imapc->state == IMAP_UPGRADETLS) {
    ఫలితం = imap_perform_upgrade_tls(దత్తాంశం, imapc, conn);
    ఒకవేళ(ఫలితం || (imapc->state == IMAP_UPGRADETLS))
      తిరిగి ఫలితం;
  }

  /* Flush any data that needs to be sent */
  ఒకవేళ(pp->sendleft)
    తిరిగి Curl_pp_flushsend(దత్తాంశం, pp);

  చేయి {
    /* Read the response from the server */
    ఫలితం = Curl_pp_readresp(దత్తాంశం, FIRSTSOCKET, pp, &imapcode, &nread);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    /* Was there an error parsing the response line? */
    ఒకవేళ(imapcode == -1)
      తిరిగి CURLE_WEIRD_SERVER_జవాబు;

    ఒకవేళ(!imapcode)
      ఆపు;

    /* We have now received a full IMAP server response */
    ఎంపిక(imapc->state) {
    స్థితి IMAP_SERVERGREET:
      ఫలితం = imap_state_servergreet_resp(దత్తాంశం, imapc,
                                           imapcode, imapc->state);
      ఆపు;

    స్థితి IMAP_CAPABILITY:
      ఫలితం = imap_state_capability_resp(దత్తాంశం, imapc, imapcode, imapc->state);
      ఆపు;

    స్థితి IMAP_STARTTLS:
      ఫలితం = imap_state_starttls_resp(దత్తాంశం, imapc, imapcode, imapc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      ఒకవేళ(!ఫలితం && (imapc->state == IMAP_UPGRADETLS))
        వెళ్ళు upgrade_tls;
      ఆపు;

    స్థితి IMAP_AUTHENTICATE:
      ఫలితం = imap_state_auth_resp(దత్తాంశం, imapc, imapcode, imapc->state);
      ఆపు;

    స్థితి IMAP_LOGIN:
      ఫలితం = imap_state_login_resp(దత్తాంశం, imapc, imapcode, imapc->state);
      ఆపు;

    స్థితి IMAP_LIST:
    స్థితి IMAP_శోధన:
      ఫలితం = imap_state_listsearch_resp(దత్తాంశం, imapc, imapcode, imapc->state);
      ఆపు;

    స్థితి IMAP_SELECT:
      ఫలితం = imap_state_select_resp(దత్తాంశం, imapc, imap,
                                      imapcode, imapc->state);
      ఆపు;

    స్థితి IMAP_FETCH:
      ఫలితం = imap_state_fetch_resp(దత్తాంశం, imapc, imapcode, imapc->state);
      ఆపు;

    స్థితి IMAP_FETCH_FINAL:
      ఫలితం = imap_state_fetch_final_resp(దత్తాంశం, imapc,
                                           imapcode, imapc->state);
      ఆపు;

    స్థితి IMAP_APPEND:
      ఫలితం = imap_state_append_resp(దత్తాంశం, imapc, imapcode, imapc->state);
      ఆపు;

    స్థితి IMAP_APPEND_FINAL:
      ఫలితం = imap_state_append_final_resp(దత్తాంశం, imapc,
                                            imapcode, imapc->state);
      ఆపు;

    స్థితి IMAP_LOGOUT:
    అప్రమేయ:
      /* internal error */
      imap_state(దత్తాంశం, imapc, IMAP_STOP);
      ఆపు;
    }
  } అయితే(!ఫలితం && imapc->state != IMAP_STOP && Curl_pp_moredata(pp));

  తిరిగి ఫలితం;
}

/* Called repeatedly until done from multi.c */
స్థిర CURLcode imap_multi_statemach(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం imap_conn *imapc =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_IMAP_CONN);

  *పూర్తి = FALSE;
  ఒకవేళ(!imapc)
    తిరిగి CURLE_FAILED_INIT;
  ఫలితం = Curl_pp_statemach(దత్తాంశం, &imapc->pp, FALSE, FALSE);
  *పూర్తి = (imapc->state == IMAP_STOP);

  తిరిగి ఫలితం;
}

స్థిర CURLcode imap_ఖండం_statemach(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం imap_conn *imapc,
                                     bool disconnecting)
{
  CURLcode ఫలితం = CURLE_OK;

  అయితే(imapc->state != IMAP_STOP && !ఫలితం)
    ఫలితం = Curl_pp_statemach(దత్తాంశం, &imapc->pp, TRUE, disconnecting);

  తిరిగి ఫలితం;
}

/* For the IMAP "protocol connect" and "doing" phases only */
స్థిర CURLcode imap_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం imap_conn *imapc =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_IMAP_CONN);
  తిరిగి imapc ? Curl_pp_pollset(దత్తాంశం, &imapc->pp, ps) : CURLE_OK;
}

స్థిర శూన్య imap_easy_reset(నిర్మాణం IMAP *imap)
{
  curlx_safefree(imap->mailbox);
  curlx_safefree(imap->uid);
  curlx_safefree(imap->mindex);
  curlx_safefree(imap->section);
  curlx_safefree(imap->partial);
  curlx_safefree(imap->query);
  curlx_safefree(imap->custom);
  curlx_safefree(imap->custom_params);
  imap->uidvalidity_సమితి = FALSE;
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
స్థిర bool imap_is_bchar(అక్షరం ch)
{
  /* Performing the alnum check first with macro is faster because of ASCII
     arithmetic */
  తిరిగి ch && (ISALNUM(ch) || strchr(":@/&=-._~!$\'()*+,%", ch));
}

/***********************************************************************
 *
 * imap_parse_url_options()
 *
 * Parse the URL login options.
 */
స్థిర CURLcode imap_parse_url_options(నిర్మాణం connectdata *conn,
                                       నిర్మాణం imap_conn *imapc)
{
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం *ptr = conn->options;
  bool prefer_login = FALSE;

  అయితే(!ఫలితం && ptr && *ptr) {
    స్థిరరాశి అక్షరం *కీలకం = ptr;
    స్థిరరాశి అక్షరం *విలువ;

    అయితే(*ptr && *ptr != '=')
      ptr++;

    విలువ = ptr + 1;

    అయితే(*ptr && *ptr != ';')
      ptr++;

    ఒకవేళ(curl_strnequal(కీలకం, "AUTH=+LOGIN", 11)) {
      /* User prefers plaintext LOGIN over any SASL, including SASL LOGIN */
      prefer_login = TRUE;
      imapc->sasl.prefmech = SASL_AUTH_NONE;
    }
    లేకపోతే ఒకవేళ(curl_strnequal(కీలకం, "AUTH=", 5)) {
      prefer_login = FALSE;
      ఫలితం = Curl_sasl_parse_url_auth_option(&imapc->sasl,
                                               విలువ, ptr - విలువ);
    }
    లేకపోతే {
      prefer_login = FALSE;
      ఫలితం = CURLE_URL_MALFORMAT;
    }

    ఒకవేళ(*ptr == ';')
      ptr++;
  }

  ఒకవేళ(prefer_login)
    imapc->preftype = IMAP_TYPE_CLEARTEXT;
  లేకపోతే {
    ఎంపిక(imapc->sasl.prefmech) {
    స్థితి SASL_AUTH_NONE:
      imapc->preftype = IMAP_TYPE_NONE;
      ఆపు;
    స్థితి SASL_AUTH_DEFAULT:
      imapc->preftype = IMAP_TYPE_ANY;
      ఆపు;
    అప్రమేయ:
      imapc->preftype = IMAP_TYPE_SASL;
      ఆపు;
    }
  }

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * imap_parse_url_path()
 *
 * Parse the URL path into separate path components.
 *
 */
స్థిర CURLcode imap_parse_url_మార్గం(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం IMAP *imap)
{
  /* The imap struct is already initialized in imap_connect() */
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం *begin = &దత్తాంశం->state.పైకి.మార్గం[1]; /* skip leading slash */
  స్థిరరాశి అక్షరం *ptr = begin;

  /* See how much of the URL is a valid path and decode it */
  అయితే(imap_is_bchar(*ptr))
    ptr++;

  ఒకవేళ(ptr != begin) {
    /* Remove the trailing slash if present */
    స్థిరరాశి అక్షరం *చివర = ptr;
    ఒకవేళ(చివర > begin && చివర[-1] == '/')
      చివర--;

    ఫలితం = Curl_urldecode(begin, చివర - begin, &imap->mailbox, ఖాళీ,
                            REJECT_CTRL);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  లేకపోతే
    imap->mailbox = ఖాళీ;

  /* There can be any number of parameters in the form ";NAME=VALUE" */
  అయితే(*ptr == ';') {
    అక్షరం *name;
    అక్షరం *విలువ;
    size_t valuelen;

    /* Find the length of the name parameter */
    begin = ++ptr;
    అయితే(*ptr && *ptr != '=')
      ptr++;

    ఒకవేళ(!*ptr)
      తిరిగి CURLE_URL_MALFORMAT;

    /* Decode the name parameter */
    ఫలితం = Curl_urldecode(begin, ptr - begin, &name, ఖాళీ,
                            REJECT_CTRL);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    /* Find the length of the value parameter */
    begin = ++ptr;
    అయితే(imap_is_bchar(*ptr))
      ptr++;

    /* Decode the value parameter */
    ఫలితం = Curl_urldecode(begin, ptr - begin, &విలువ, &valuelen,
                            REJECT_CTRL);
    ఒకవేళ(ఫలితం) {
      curlx_free(name);
      తిరిగి ఫలితం;
    }

    DEBUGF(infof(దత్తాంశం, "IMAP URL parameter '%s' = '%s'", name, విలువ));

    /* Process the known hierarchical parameters (UIDVALIDITY, UID, SECTION
       and PARTIAL) stripping of the trailing slash character if it is
       present.

       Note: Unknown parameters trigger a URL_MALFORMAT error. */
    ఒకవేళ(valuelen > 0 && విలువ[valuelen - 1] == '/')
      విలువ[valuelen - 1] = '\0';
    ఒకవేళ(valuelen) {
      ఒకవేళ(curl_strequal(name, "UIDVALIDITY") && !imap->uidvalidity_సమితి) {
        curl_off_t అంకె;
        స్థిరరాశి అక్షరం *p = (స్థిరరాశి అక్షరం *)విలువ;
        ఒకవేళ(!curlx_str_సంఖ్య(&p, &అంకె, UINT_MAX)) {
          imap->uidvalidity = (గుర్తులేని పూర్ణాంకం)అంకె;
          imap->uidvalidity_సమితి = TRUE;
        }
        curlx_free(విలువ);
      }
      లేకపోతే ఒకవేళ(curl_strequal(name, "UID") && !imap->uid) {
        imap->uid = విలువ;
      }
      లేకపోతే ఒకవేళ(curl_strequal(name, "MAILINDEX") && !imap->mindex) {
        imap->mindex = విలువ;
      }
      లేకపోతే ఒకవేళ(curl_strequal(name, "SECTION") && !imap->section) {
        imap->section = విలువ;
      }
      లేకపోతే ఒకవేళ(curl_strequal(name, "PARTIAL") && !imap->partial) {
        imap->partial = విలువ;
      }
      లేకపోతే {
        curlx_free(name);
        curlx_free(విలువ);
        తిరిగి CURLE_URL_MALFORMAT;
      }
    }
    లేకపోతే
      /* blank? */
      curlx_free(విలువ);
    curlx_free(name);
  }

  /* Does the URL contain a query parameter? Only valid when we have a mailbox
     and no UID as per RFC-5092 */
  ఒకవేళ(imap->mailbox && !imap->uid && !imap->mindex) {
    /* Get the query parameter, URL decoded */
    CURLUcode uc = curl_url_get(దత్తాంశం->state.uh, CURLUPART_QUERY, &imap->query,
                                CURLU_URLDECODE);
    ఒకవేళ(uc == CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  /* Any extra stuff at the end of the URL is an error */
  ఒకవేళ(*ptr)
    తిరిగి CURLE_URL_MALFORMAT;

  తిరిగి CURLE_OK;
}

/***********************************************************************
 *
 * imap_parse_custom_request()
 *
 * Parse the custom request.
 */
స్థిర CURLcode imap_parse_custom_request(నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం IMAP *imap)
{
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం *custom = CURL_EASY_STR(దత్తాంశం, STRING_CUSTOMREQUEST);

  ఒకవేళ(custom) {
    /* URL decode the custom request */
    ఫలితం = Curl_urldecode(custom, 0, &imap->custom, ఖాళీ, REJECT_CTRL);

    /* Extract the parameters if specified */
    ఒకవేళ(!ఫలితం) {
      స్థిరరాశి అక్షరం *params = imap->custom;

      అయితే(*params && *params != ' ')
        params++;

      ఒకవేళ(*params) {
        imap->custom_params = curlx_strdup(params);
        imap->custom[params - imap->custom] = '\0';

        ఒకవేళ(!imap->custom_params)
          ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
    }
  }

  తిరిగి ఫలితం;
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
స్థిర CURLcode imap_connect(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  నిర్మాణం imap_conn *imapc =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_IMAP_CONN);
  CURLcode ఫలితం = CURLE_OK;

  *పూర్తి = FALSE; /* default to not done yet */
  ఒకవేళ(!imapc)
    తిరిగి CURLE_FAILED_INIT;

  /* Parse the URL options */
  ఫలితం = imap_parse_url_options(దత్తాంశం->conn, imapc);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* Start off waiting for the server greeting response */
  imap_state(దత్తాంశం, imapc, IMAP_SERVERGREET);

  /* Start off with an response id of '*' */
  curlx_strcopy(imapc->resptag, పరిమాణం(imapc->resptag), STRCONST("*"));

  ఫలితం = imap_multi_statemach(దత్తాంశం, పూర్తి);

  తిరిగి ఫలితం;
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
స్థిర CURLcode imap_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, CURLcode స్థితి_2,
                          bool premature)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  నిర్మాణం IMAP *imap = Curl_meta_get(దత్తాంశం, CURL_META_IMAP_EASY);

  ఒకవేళ(!imapc)
    తిరిగి CURLE_FAILED_INIT;
  ఒకవేళ(!imap)
    తిరిగి CURLE_OK;

  ఒకవేళ(స్థితి_2 || premature) {
    CURL_TRC_M(దత్తాంశం, "IMAP done, not finalizing transfer");
    connclose(conn); /* marked for closure */
    ఫలితం = స్థితి_2;         /* use the already set error code */
  }
  లేకపోతే ఒకవేళ(!దత్తాంశం->సమితి.connect_only &&
          ((!imap->custom && (imap->uid || imap->mindex)) ||
           (imap->custom && దత్తాంశం->req.maxdownload > 0) ||
           దత్తాంశం->state.upload || IS_MIME_POST(దత్తాంశం))) {
    /* Handle responses after FETCH or APPEND transfer has finished.
       For custom commands, check if we set up a download which indicates
       a FETCH-like command with literal data. */

    ఒకవేళ(!దత్తాంశం->state.upload && !IS_MIME_POST(దత్తాంశం))
      imap_state(దత్తాంశం, imapc, IMAP_FETCH_FINAL);
    లేకపోతే {
      /* End the APPEND command first by sending an empty line */
      ఫలితం = Curl_pp_sendf(దత్తాంశం, &imapc->pp, "%s", "");
      ఒకవేళ(!ఫలితం)
        imap_state(దత్తాంశం, imapc, IMAP_APPEND_FINAL);
    }

    /* Run the state-machine */
    ఒకవేళ(!ఫలితం)
      ఫలితం = imap_ఖండం_statemach(దత్తాంశం, imapc, FALSE);
  }

  imap_easy_reset(imap);
  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * imap_perform()
 *
 * This is the actual DO function for IMAP. Fetch or append a message, or do
 * other things according to the options previously setup.
 */
స్థిర CURLcode imap_perform(నిర్మాణం Curl_easy *దత్తాంశం, bool *connected,
                             bool *dophase_పూర్తి)
{
  /* This is IMAP and no proxy */
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);
  నిర్మాణం IMAP *imap = Curl_meta_get(దత్తాంశం, CURL_META_IMAP_EASY);
  bool selected = FALSE;

  DEBUGF(infof(దత్తాంశం, "DO phase starts"));
  ఒకవేళ(!imapc || !imap)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(దత్తాంశం->req.no_body) {
    /* Requested no body means no transfer */
    imap->transfer = PPTRANSFER_INFO;
  }

  *dophase_పూర్తి = FALSE; /* not done yet */

  /* Determine if the requested mailbox (with the same UIDVALIDITY if set)
     has already been selected on this connection */
  ఒకవేళ(imap->mailbox && imapc->mailbox &&
     (!strcmp(imap->mailbox, imapc->mailbox) ||
      (curl_strequal(imap->mailbox, "INBOX") &&
       curl_strequal(imapc->mailbox, "INBOX"))) &&
     (!imap->uidvalidity_సమితి || !imapc->mb_uidvalidity_సమితి ||
      (imap->uidvalidity == imapc->mb_uidvalidity)))
    selected = TRUE;

  /* Start the first command in the DO phase */
  ఒకవేళ(దత్తాంశం->state.upload || IS_MIME_POST(దత్తాంశం))
    /* APPEND can be executed directly */
    ఫలితం = imap_perform_append(దత్తాంశం, imapc, imap);
  లేకపోతే ఒకవేళ(imap->custom && (selected || !imap->mailbox))
    /* Custom command using the same mailbox or no mailbox */
    ఫలితం = imap_perform_list(దత్తాంశం, imapc, imap);
  లేకపోతే ఒకవేళ(!imap->custom && selected && (imap->uid || imap->mindex))
    /* FETCH from the same mailbox */
    ఫలితం = imap_perform_fetch(దత్తాంశం, imapc, imap);
  లేకపోతే ఒకవేళ(!imap->custom && selected && imap->query)
    /* SEARCH the current mailbox */
    ఫలితం = imap_perform_శోధన(దత్తాంశం, imapc, imap);
  లేకపోతే ఒకవేళ(imap->mailbox && !selected &&
          (imap->custom || imap->uid || imap->mindex || imap->query))
    /* SELECT the mailbox */
    ఫలితం = imap_perform_select(దత్తాంశం, imapc, imap);
  లేకపోతే
    /* LIST */
    ఫలితం = imap_perform_list(దత్తాంశం, imapc, imap);

  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* Run the state-machine */
  ఫలితం = imap_multi_statemach(దత్తాంశం, dophase_పూర్తి);

  *connected = Curl_conn_is_connected(conn, FIRSTSOCKET);

  ఒకవేళ(*dophase_పూర్తి)
    DEBUGF(infof(దత్తాంశం, "DO phase is complete"));

  తిరిగి ఫలితం;
}

/* Call this when the DO phase has completed */
స్థిర CURLcode imap_dophase_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం IMAP *imap,
                                  bool connected)
{
  (శూన్య)connected;

  ఒకవేళ(imap->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_అమరిక_nop(దత్తాంశం);

  తిరిగి CURLE_OK;
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
స్థిర CURLcode imap_regular_transfer(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం IMAP *imap,
                                      bool *dophase_పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  దత్తాంశం->req.కొలత = -1;

  /* Set the progress data */
  Curl_pgrsReset(దత్తాంశం);

  /* Carry out the perform */
  ఫలితం = imap_perform(దత్తాంశం, &connected, dophase_పూర్తి);

  /* Perform post DO phase operations if necessary */
  ఒకవేళ(!ఫలితం && *dophase_పూర్తి)
    ఫలితం = imap_dophase_పూర్తి(దత్తాంశం, imap, connected);

  తిరిగి ఫలితం;
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
స్థిర CURLcode imap_do(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  నిర్మాణం IMAP *imap = Curl_meta_get(దత్తాంశం, CURL_META_IMAP_EASY);
  CURLcode ఫలితం = CURLE_OK;
  *పూర్తి = FALSE; /* default to false */

  ఒకవేళ(!imap)
    తిరిగి CURLE_FAILED_INIT;
  /* Parse the URL path */
  ఫలితం = imap_parse_url_మార్గం(దత్తాంశం, imap);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* Parse the custom request */
  ఫలితం = imap_parse_custom_request(దత్తాంశం, imap);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఫలితం = imap_regular_transfer(దత్తాంశం, imap, పూర్తి);

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * imap_disconnect()
 *
 * Disconnect from an IMAP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
స్థిర CURLcode imap_disconnect(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం connectdata *conn, bool dead_connection)
{
  నిర్మాణం imap_conn *imapc = Curl_conn_meta_get(conn, CURL_META_IMAP_CONN);

  ఒకవేళ(imapc &&
     /* We cannot send quit unconditionally. If this connection is stale or
        bad in any way (pingpong has pending data to send),
        sending quit and waiting around here will make the
        disconnect wait in vain and cause more problems than we need to. */
     !dead_connection && conn->బిట్లు.protoconnstart &&
     !Curl_pp_needs_flush(దత్తాంశం, &imapc->pp) &&
     !imap_perform_logout(దత్తాంశం, imapc))
    (శూన్య)imap_ఖండం_statemach(దత్తాంశం, imapc, TRUE); /* ignore errors */

  తిరిగి CURLE_OK;
}

/* Called from multi.c while DOing */
స్థిర CURLcode imap_doing(నిర్మాణం Curl_easy *దత్తాంశం, bool *dophase_పూర్తి)
{
  నిర్మాణం IMAP *imap = Curl_meta_get(దత్తాంశం, CURL_META_IMAP_EASY);
  CURLcode ఫలితం;

  ఒకవేళ(!imap)
    తిరిగి CURLE_FAILED_INIT;

  ఫలితం = imap_multi_statemach(దత్తాంశం, dophase_పూర్తి);
  ఒకవేళ(ఫలితం)
    DEBUGF(infof(దత్తాంశం, "DO phase failed"));
  లేకపోతే ఒకవేళ(*dophase_పూర్తి) {
    ఫలితం = imap_dophase_పూర్తి(దత్తాంశం, imap, FALSE /* not connected */);

    DEBUGF(infof(దత్తాంశం, "DO phase is complete"));
  }

  తిరిగి ఫలితం;
}

స్థిర శూన్య imap_easy_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం IMAP *imap = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  imap_easy_reset(imap);
  curlx_free(imap);
}

స్థిర శూన్య imap_conn_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం imap_conn *imapc = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  Curl_pp_disconnect(&imapc->pp);
  curlx_safefree(imapc->mailbox);
  curlx_free(imapc);
}

/* SASL parameters for the imap protocol */
స్థిర స్థిరరాశి నిర్మాణం SASLproto saslimap = {
  "imap",                     /* The service name */
  imap_perform_authenticate,  /* Send authentication command */
  imap_continue_authenticate, /* Send authentication continuation */
  imap_cancel_authenticate,   /* Send authentication cancellation */
  imap_get_సందేశం,           /* Get SASL response message */
  0,                          /* No maximum initial response length */
  '+',                        /* Code received when continuation is expected */
  IMAP_RESP_OK,               /* Code to receive upon authentication success */
  SASL_AUTH_DEFAULT,          /* Default mechanisms */
  SASL_FLAG_BASE64            /* Configuration flags */
};

స్థిర CURLcode imap_అమరిక_connection(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం connectdata *conn)
{
  నిర్మాణం imap_conn *imapc;
  నిర్మాణం pingpong *pp;
  నిర్మాణం IMAP *imap;

  imapc = curlx_calloc(1, పరిమాణం(*imapc));
  ఒకవేళ(!imapc)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  pp = &imapc->pp;
  PINGPONG_అమరిక(pp, imap_pp_statemachine, imap_endofresp);

  /* Set the default preferred authentication type and mechanism */
  imapc->preftype = IMAP_TYPE_ANY;
  Curl_sasl_init(&imapc->sasl, దత్తాంశం, &saslimap);

  Curl_pp_init(pp, Curl_pgrs_now(దత్తాంశం));

  ఒకవేళ(Curl_conn_meta_సమితి(conn, CURL_META_IMAP_CONN, imapc, imap_conn_dtor))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  imap = curlx_calloc(1, పరిమాణం(నిర్మాణం IMAP));
  ఒకవేళ(!imap ||
     Curl_meta_సమితి(దత్తాంశం, CURL_META_IMAP_EASY, imap, imap_easy_dtor))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  తిరిగి CURLE_OK;
}

/*
 * IMAP protocol.
 */
స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_imap = {
  imap_అమరిక_connection,            /* setup_connection */
  imap_do,                          /* do_it */
  imap_పూర్తి,                        /* done */
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

#అయితే_ముగింపు /* CURL_DISABLE_IMAP */
