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
 * RFC1734 POP3 Authentication
 * RFC1939 POP3 protocol
 * RFC2195 CRAM-MD5 authentication
 * RFC2384 POP URL Scheme
 * RFC2449 POP3 Extension Mechanism
 * RFC2595 Using TLS with IMAP, POP3 and ACAP
 * RFC2831 DIGEST-MD5 authentication
 * RFC4422 Simple Authentication and Security Layer (SASL)
 * RFC4616 PLAIN authentication
 * RFC4752 The Kerberos V5 ("GSSAPI") SASL Mechanism
 * RFC5034 POP3 SASL Authentication Mechanism
 * RFC6749 OAuth 2.0 Authorization Framework
 * RFC8314 Use of TLS for Email Submission and Access
 * Draft   LOGIN SASL Mechanism <draft-murchison-sasl-login-00.txt>
 *
 ***************************************************************************/
#समावेश "curl_setup.h"
#समावेश "urldata.h"
#समावेश "pop3.h"

#अगर_अघोषित CURL_DISABLE_POP3

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

#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "progress.h"
#समावेश "transfer.h"
#समावेश "escape.h"
#समावेश "pingpong.h"
#समावेश "vtls/vtls.h"
#समावेश "cfilters.h"
#समावेश "connect.h"
#समावेश "select.h"
#समावेश "url.h"
#समावेश "bufref.h"
#समावेश "curl_sasl.h"
#समावेश "curl_md5.h"
#समावेश "curlx/strdup.h"

/* Authentication type flags */
#घोषणा POP3_TYPE_CLEARTEXT (1 << 0)
#घोषणा POP3_TYPE_APOP      (1 << 1)
#घोषणा POP3_TYPE_SASL      (1 << 2)

/* Authentication type values */
#घोषणा POP3_TYPE_NONE 0
#घोषणा POP3_TYPE_ANY  (POP3_TYPE_CLEARTEXT | POP3_TYPE_APOP | POP3_TYPE_SASL)

/* This is the 5-bytes End-Of-Body marker for POP3 */
#घोषणा POP3_EOB     "\x0d\x0a\x2e\x0d\x0a"
#घोषणा POP3_EOB_LEN 5

/* meta key for storing protocol meta at easy handle */
#घोषणा CURL_META_POP3_EASY   "meta:proto:pop3:easy"
/* meta key for storing protocol meta at connection */
#घोषणा CURL_META_POP3_CONN   "meta:proto:pop3:conn"

/*
 * POP3 easy handle state
 */
काष्ठा POP3 {
  curl_pp_transfer transfer;
  अक्षर *id;               /* Message ID */
  अक्षर *custom;           /* Custom Request */
};

/*
 * POP3 connection state
 */
प्रकार क्रमागत {
  POP3_STOP,         /* do nothing state, stops the state machine */
  POP3_SERVERGREET,  /* waiting for the initial greeting immediately after
                        a connect */
  POP3_CAPA,
  POP3_STARTTLS,
  POP3_UPGRADETLS,   /* asynchronously upgrade the connection to SSL/TLS
                       (multi mode only) */
  POP3_AUTH,
  POP3_APOP,
  POP3_USER,
  POP3_PASS,
  POP3_COMMAND,
  POP3_QUIT,
  POP3_LAST          /* never used */
} pop3state;

काष्ठा pop3_conn {
  काष्ठा pingpong pp;
  pop3state state;        /* Always use pop3.c:state() to change state! */
  माप_प्रकार eob;             /* Number of bytes of the EOB (End Of Body) that
                             have been received so far */
  माप_प्रकार strip;           /* Number of bytes from the start to ignore as
                             non-body */
  काष्ठा SASL sasl;       /* SASL-related storage */
  अक्षर *apoptimestamp;    /* APOP timestamp from the server greeting */
  अचिन्हित अक्षर authtypes; /* Accepted authentication types */
  अचिन्हित अक्षर preftype;  /* Preferred authentication type */
  BIT(ssldone);           /* Is connect() over SSL done? */
  BIT(tls_supported);     /* StartTLS capability supported by server */
};

काष्ठा pop3_cmd {
  स्थिर अक्षर *name;
  अचिन्हित लघु nlen;
  BIT(multiline); /* response is multi-line with last '.' line */
  BIT(multiline_with_args); /* is multi-line when command has args */
};

अटल स्थिर काष्ठा pop3_cmd pop3cmds[] = {
  { "APOP", 4, FALSE, FALSE },
  { "AUTH", 4, FALSE, FALSE },
  { "CAPA", 4, TRUE, TRUE },
  { "DELE", 4, FALSE, FALSE },
  { "LIST", 4, TRUE, FALSE },
  { "MSG",  3, TRUE, TRUE },
  { "NOOP", 4, FALSE, FALSE },
  { "PASS", 4, FALSE, FALSE },
  { "QUIT", 4, FALSE, FALSE },
  { "RETR", 4, TRUE, TRUE },
  { "RSET", 4, FALSE, FALSE },
  { "STAT", 4, FALSE, FALSE },
  { "STLS", 4, FALSE, FALSE },
  { "TOP",  3, TRUE, TRUE },
  { "UIDL", 4, TRUE, FALSE },
  { "USER", 4, FALSE, FALSE },
  { "UTF8", 4, FALSE, FALSE },
  { "XTND", 4, TRUE, TRUE },
};

/***********************************************************************
 *
 * pop3_parse_url_options()
 *
 * Parse the URL login options.
 */
अटल CURLcode pop3_parse_url_options(काष्ठा connectdata *conn)
{
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर *ptr = conn->options;

  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  जबतक(!परिणाम && ptr && *ptr) {
    स्थिर अक्षर *कुंजी = ptr;
    स्थिर अक्षर *मूल्य;

    जबतक(*ptr && *ptr != '=')
      ptr++;

    मूल्य = ptr + 1;

    जबतक(*ptr && *ptr != ';')
      ptr++;

    अगर(curl_strnequal(कुंजी, "AUTH=", 5)) {
      परिणाम = Curl_sasl_parse_url_auth_option(&pop3c->sasl,
                                               मूल्य, ptr - मूल्य);

      अगर(परिणाम && curl_strnequal(मूल्य, "+APOP", ptr - मूल्य)) {
        pop3c->preftype = POP3_TYPE_APOP;
        pop3c->sasl.prefmech = SASL_AUTH_NONE;
        परिणाम = CURLE_OK;
      }
    }
    अन्यथा
      परिणाम = CURLE_URL_MALFORMAT;

    अगर(*ptr == ';')
      ptr++;
  }

  अगर(pop3c->preftype != POP3_TYPE_APOP)
    चयन(pop3c->sasl.prefmech) {
    हाल SASL_AUTH_NONE:
      pop3c->preftype = POP3_TYPE_NONE;
      अवरोध;
    हाल SASL_AUTH_DEFAULT:
      pop3c->preftype = POP3_TYPE_ANY;
      अवरोध;
    शेष:
      pop3c->preftype = POP3_TYPE_SASL;
      अवरोध;
    }

  वापस परिणाम;
}

/***********************************************************************
 *
 * pop3_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
अटल CURLcode pop3_parse_url_path(काष्ठा Curl_easy *तथ्य)
{
  /* The POP3 struct is already initialized in pop3_connect() */
  काष्ठा POP3 *pop3 = Curl_meta_get(तथ्य, CURL_META_POP3_EASY);
  स्थिर अक्षर *path = &तथ्य->state.up.path[1]; /* skip leading path */

  अगर(!pop3)
    वापस CURLE_FAILED_INIT;
  /* URL decode the path for the message ID */
  वापस Curl_urldecode(path, 0, &pop3->id, शून्य, REJECT_CTRL);
}

/***********************************************************************
 *
 * pop3_parse_custom_request()
 *
 * Parse the custom request.
 */
अटल CURLcode pop3_parse_custom_request(काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा POP3 *pop3 = Curl_meta_get(तथ्य, CURL_META_POP3_EASY);
  स्थिर अक्षर *custom = CURL_EASY_STR(तथ्य, STRING_CUSTOMREQUEST);

  अगर(!pop3)
    वापस CURLE_FAILED_INIT;
  /* URL decode the custom request */
  अगर(custom)
    परिणाम = Curl_urldecode(custom, 0, &pop3->custom, शून्य, REJECT_CTRL);

  वापस परिणाम;
}

/* Return iff a command is defined as "multi-line" (RFC 1939),
 * has a response terminated by a last line with a '.'.
 */
अटल bool pop3_is_multiline(स्थिर अक्षर *cmdline)
{
  माप_प्रकार i;
  क्रम(i = 0; i < CURL_ARRAYSIZE(pop3cmds); ++i) {
    अगर(curl_strnequal(pop3cmds[i].name, cmdline, pop3cmds[i].nlen)) {
      अगर(!cmdline[pop3cmds[i].nlen])
        वापस (bool)pop3cmds[i].multiline;
      अन्यथा अगर(cmdline[pop3cmds[i].nlen] == ' ')
        वापस (bool)pop3cmds[i].multiline_with_args;
    }
  }
  /* Unknown command, assume multi-line for backward compatibility with
   * earlier curl versions that only could do multi-line responses. */
  वापस TRUE;
}

/***********************************************************************
 *
 * pop3_endofresp()
 *
 * Checks for an ending POP3 status code at the start of the given string, but
 * also detects the APOP timestamp from the server greeting and various
 * capabilities from the CAPA response including the supported authentication
 * types and allowed SASL mechanisms.
 */
अटल bool pop3_endofresp(काष्ठा Curl_easy *तथ्य, काष्ठा connectdata *conn,
                           स्थिर अक्षर *पंक्ति, माप_प्रकार len, पूर्णांक *resp)
{
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (व्योम)तथ्य;
  DEBUGASSERT(pop3c);
  अगर(!pop3c) /* internal error */
    वापस TRUE;

  /* Do we have an error response? */
  अगर(len >= 4 && !स_भेद("-ERR", पंक्ति, 4)) {
    *resp = '-';

    वापस TRUE;
  }

  /* Are we processing CAPA command responses? */
  अगर(pop3c->state == POP3_CAPA) {
    /* Do we have the terminating line? Per RFC 2449 this is a line
       containing only a single dot */
    अगर((len == 3 && पंक्ति[0] == '.' && पंक्ति[1] == '\r') ||
       (len == 2 && पंक्ति[0] == '.' && पंक्ति[1] == '\n'))
      /* Treat the response as a success */
      *resp = '+';
    अन्यथा
      /* Treat the response as an untagged continuation */
      *resp = '*';

    वापस TRUE;
  }

  /* Do we have a success response? */
  अगर(len >= 3 && !स_भेद("+OK", पंक्ति, 3)) {
    *resp = '+';

    वापस TRUE;
  }

  /* Do we have a continuation response? */
  अगर(len >= 1 && पंक्ति[0] == '+') {
    *resp = '*';

    वापस TRUE;
  }

  वापस FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * pop3_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
अटल CURLcode pop3_get_message(काष्ठा Curl_easy *तथ्य, काष्ठा bufref *out)
{
  काष्ठा pop3_conn *pop3c =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_POP3_CONN);
  अक्षर *message;
  माप_प्रकार len;

  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;
  message = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;
  अगर(len > 2) {
    /* Find the start of the message */
    len -= 2;
    क्रम(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    जबतक(len--)
      अगर(!ISBLANK(message[len]) && !ISNEWLINE(message[len]))
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
 * pop3_state()
 *
 * This is the ONLY way to change POP3 state!
 */
अटल व्योम pop3_state(काष्ठा Curl_easy *तथ्य, pop3state newstate)
{
  काष्ठा pop3_conn *pop3c =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_POP3_CONN);
  अगर(pop3c) {
#अगर defined(DEBUGBUILD) && defined(CURLVERBOSE)
    /* for debug purposes */
    अटल स्थिर अक्षर * स्थिर names[] = {
      "STOP",
      "SERVERGREET",
      "CAPA",
      "STARTTLS",
      "UPGRADETLS",
      "AUTH",
      "APOP",
      "USER",
      "PASS",
      "COMMAND",
      "QUIT",
      /* LAST */
    };

    अगर(pop3c->state != newstate)
      infof(तथ्य, "POP3 %p state change from %s to %s",
            (व्योम *)pop3c, names[pop3c->state], names[newstate]);
#पूर्ण_अगर

    pop3c->state = newstate;
  }
}

/***********************************************************************
 *
 * pop3_perform_capa()
 *
 * Sends the CAPA command in order to obtain a list of server side supported
 * capabilities.
 */
अटल CURLcode pop3_perform_capa(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata *conn)
{
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode परिणाम = CURLE_OK;

  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  pop3c->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  pop3c->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  pop3c->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPA command */
  परिणाम = Curl_pp_sendf(तथ्य, &pop3c->pp, "%s", "CAPA");

  अगर(!परिणाम)
    pop3_state(तथ्य, POP3_CAPA);

  वापस परिणाम;
}

/***********************************************************************
 *
 * pop3_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
अटल CURLcode pop3_perform_starttls(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा connectdata *conn)
{
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode परिणाम;

  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  /* Send the STLS command */
  परिणाम = Curl_pp_sendf(तथ्य, &pop3c->pp, "%s", "STLS");
  अगर(!परिणाम)
    pop3_state(तथ्य, POP3_STARTTLS);

  वापस परिणाम;
}

/***********************************************************************
 *
 * pop3_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
अटल CURLcode pop3_perform_upgrade_tls(काष्ठा Curl_easy *तथ्य,
                                         काष्ठा connectdata *conn)
{
#अगर_घोषित USE_SSL
  /* Start the SSL connection */
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode परिणाम;
  bool ssldone = FALSE;

  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  अगर(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    परिणाम = Curl_ssl_cfilter_add(
      तथ्य, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    अगर(परिणाम)
      जाओ out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_pop3s;
  }

  DEBUGASSERT(!pop3c->ssldone);
  परिणाम = Curl_conn_connect(तथ्य, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(तथ्य, "pop3_perform_upgrade_tls, connect -> %d, %d",
               (पूर्णांक)परिणाम, ssldone));
  अगर(!परिणाम && ssldone) {
    pop3c->ssldone = ssldone;
    /* perform CAPA now, changes pop3c->state out of POP3_UPGRADETLS */
    परिणाम = pop3_perform_capa(तथ्य, conn);
  }
out:
  वापस परिणाम;
#अन्यथा
  (व्योम)तथ्य;
  (व्योम)conn;
  वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
}

/***********************************************************************
 *
 * pop3_perform_user()
 *
 * Sends a clear text USER command to authenticate with.
 */
अटल CURLcode pop3_perform_user(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata *conn)
{
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode परिणाम = CURLE_OK;

  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  अगर(!conn->creds) {
    pop3_state(तथ्य, POP3_STOP);

    वापस परिणाम;
  }

  /* Send the USER command */
  परिणाम = Curl_pp_sendf(तथ्य, &pop3c->pp, "USER %s",
                         Curl_creds_user(conn->creds));
  अगर(!परिणाम)
    pop3_state(तथ्य, POP3_USER);

  वापस परिणाम;
}

#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH
/***********************************************************************
 *
 * pop3_perform_apop()
 *
 * Sends an APOP command to authenticate with.
 */
अटल CURLcode pop3_perform_apop(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata *conn)
{
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार i;
  काष्ठा MD5_context *ctxt;
  अचिन्हित अक्षर digest[MD5_DIGEST_LEN];
  अक्षर secret[(2 * MD5_DIGEST_LEN) + 1];

  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  अगर(!तथ्य->state.creds) {
    pop3_state(तथ्य, POP3_STOP);

    वापस परिणाम;
  }

  /* Create the digest */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  अगर(!ctxt)
    वापस CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)pop3c->apoptimestamp,
                  curlx_uztoui(म_माप(pop3c->apoptimestamp)));

  Curl_MD5_update(ctxt, (स्थिर अचिन्हित अक्षर *)Curl_creds_passwd(conn->creds),
                  curlx_uztoui(म_माप(Curl_creds_passwd(conn->creds))));

  /* Finalise the digest */
  Curl_MD5_final(ctxt, digest);

  /* Convert the calculated 16 octet digest into a 32-byte hex string */
  क्रम(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&secret[2 * i], 3, "%02x", digest[i]);

  परिणाम = Curl_pp_sendf(तथ्य, &pop3c->pp, "APOP %s %s",
                         Curl_creds_user(conn->creds), secret);

  अगर(!परिणाम)
    pop3_state(तथ्य, POP3_APOP);

  वापस परिणाम;
}
#पूर्ण_अगर

/***********************************************************************
 *
 * pop3_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
अटल CURLcode pop3_perform_auth(काष्ठा Curl_easy *तथ्य,
                                  स्थिर अक्षर *mech,
                                  स्थिर काष्ठा bufref *initresp)
{
  काष्ठा pop3_conn *pop3c =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_POP3_CONN);
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर *ir = Curl_bufref_ptr(initresp);

  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  अगर(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    परिणाम = Curl_pp_sendf(तथ्य, &pop3c->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  अन्यथा {
    /* Send the AUTH command */
    परिणाम = Curl_pp_sendf(तथ्य, &pop3c->pp, "AUTH %s", mech);
  }

  वापस परिणाम;
}

/***********************************************************************
 *
 * pop3_continue_auth()
 *
 * Sends SASL continuation data.
 */
अटल CURLcode pop3_continue_auth(काष्ठा Curl_easy *तथ्य,
                                   स्थिर अक्षर *mech,
                                   स्थिर काष्ठा bufref *resp)
{
  काष्ठा pop3_conn *pop3c =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_POP3_CONN);

  (व्योम)mech;
  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  वापस Curl_pp_sendf(तथ्य, &pop3c->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * pop3_cancel_auth()
 *
 * Sends SASL cancellation.
 */
अटल CURLcode pop3_cancel_auth(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *mech)
{
  काष्ठा pop3_conn *pop3c =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_POP3_CONN);

  (व्योम)mech;
  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  वापस Curl_pp_sendf(तथ्य, &pop3c->pp, "*");
}

/***********************************************************************
 *
 * pop3_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to APOP and clear text should a
 * common mechanism not be available between the client and server.
 */
अटल CURLcode pop3_perform_authentication(काष्ठा Curl_easy *तथ्य,
                                            काष्ठा connectdata *conn)
{
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode परिणाम = CURLE_OK;
  saslprogress progress = SASL_IDLE;

  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  /* Check we have enough data to authenticate with and end the
     connect phase if we do not */
  अगर(!Curl_sasl_can_authenticate(&pop3c->sasl, तथ्य)) {
    pop3_state(तथ्य, POP3_STOP);
    वापस परिणाम;
  }

  अगर(pop3c->authtypes & pop3c->preftype & POP3_TYPE_SASL) {
    /* Calculate the SASL login details */
    परिणाम = Curl_sasl_start(&pop3c->sasl, तथ्य, FALSE, &progress);

    अगर(!परिणाम && progress == SASL_INPROGRESS)
      pop3_state(तथ्य, POP3_AUTH);
  }

  अगर(!परिणाम && progress == SASL_IDLE) {
#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH
    अगर(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
      /* Perform APOP authentication */
      परिणाम = pop3_perform_apop(तथ्य, conn);
    अन्यथा
#पूर्ण_अगर
    अगर(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
      /* Perform clear text authentication */
      परिणाम = pop3_perform_user(तथ्य, conn);
    अन्यथा
      परिणाम = Curl_sasl_is_blocked(&pop3c->sasl, तथ्य);
  }

  वापस परिणाम;
}

/***********************************************************************
 *
 * pop3_perform_command()
 *
 * Sends a POP3 based command.
 */
अटल CURLcode pop3_perform_command(काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा POP3 *pop3 = Curl_meta_get(तथ्य, CURL_META_POP3_EASY);
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  स्थिर अक्षर *command = शून्य;

  अगर(!pop3 || !pop3c)
    वापस CURLE_FAILED_INIT;

  /* Calculate the default command */
  अगर(pop3->id[0] == '\0' || तथ्य->set.list_only) {
    command = "LIST";

    अगर(pop3->id[0] != '\0')
      /* Message specific LIST so skip the BODY transfer */
      pop3->transfer = PPTRANSFER_INFO;
  }
  अन्यथा
    command = "RETR";

  अगर(pop3->custom && pop3->custom[0] != '\0')
    command = pop3->custom;

  /* Send the command */
  अगर(pop3->id[0] != '\0')
    परिणाम = Curl_pp_sendf(तथ्य, &pop3c->pp, "%s %s", command, pop3->id);
  अन्यथा
    परिणाम = Curl_pp_sendf(तथ्य, &pop3c->pp, "%s", command);

  अगर(!परिणाम) {
    pop3_state(तथ्य, POP3_COMMAND);
    तथ्य->req.no_body = !pop3_is_multiline(command);
  }

  वापस परिणाम;
}

/***********************************************************************
 *
 * pop3_perform_quit()
 *
 * Performs the quit action prior to sclose() be called.
 */
अटल CURLcode pop3_perform_quit(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata *conn)
{
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode परिणाम;

  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  /* Send the QUIT command */
  परिणाम = Curl_pp_sendf(तथ्य, &pop3c->pp, "%s", "QUIT");
  अगर(!परिणाम)
    pop3_state(तथ्य, POP3_QUIT);

  वापस परिणाम;
}

/* For the initial server greeting */
अटल CURLcode pop3_state_servergreet_resp(काष्ठा Curl_easy *तथ्य,
                                            पूर्णांक pop3code,
                                            pop3state instate)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  स्थिर अक्षर *पंक्ति;
  माप_प्रकार len;

  (व्योम)instate;
  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  पंक्ति = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  अगर(pop3code != '+') {
    failf(तथ्य, "Got unexpected pop3-server response");
    परिणाम = CURLE_WEIRD_SERVER_REPLY;
  }
  अन्यथा अगर(len > 3) {
    /* Does the server support APOP authentication? */
    स्थिर अक्षर *lt;
    स्थिर अक्षर *gt = शून्य;

    /* Look for the APOP timestamp */
    lt = स_प्रथम(पंक्ति, '<', len);
    अगर(lt)
      /* search the remainder for '>' */
      gt = स_प्रथम(lt, '>', len - (lt - पंक्ति));
    अगर(gt) {
      /* the length of the timestamp, including the brackets */
      माप_प्रकार timestamplen = gt - lt + 1;
      स्थिर अक्षर *at = स_प्रथम(lt, '@', timestamplen);
      /* If the timestamp does not contain '@' it is not (as required by
         RFC-1939) conformant to the RFC-822 message id syntax, and we
         therefore do not use APOP authentication. */
      अगर(at) {
        /* dupe the timestamp */
        pop3c->apoptimestamp = curlx_memdup0(lt, timestamplen);
        अगर(!pop3c->apoptimestamp)
          वापस CURLE_OUT_OF_MEMORY;
        /* Store the APOP capability */
        pop3c->authtypes |= POP3_TYPE_APOP;
      }
    }

    अगर(!परिणाम)
      परिणाम = pop3_perform_capa(तथ्य, conn);
  }

  वापस परिणाम;
}

/* For CAPA responses */
अटल CURLcode pop3_state_capa_resp(काष्ठा Curl_easy *तथ्य, पूर्णांक pop3code,
                                     pop3state instate)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  स्थिर अक्षर *पंक्ति;
  माप_प्रकार len;

  (व्योम)instate;
  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  पंक्ति = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  /* Do we have an untagged continuation response? */
  अगर(pop3code == '*') {
    /* Does the server support the STLS capability? */
    अगर(len >= 4 && curl_strnequal(पंक्ति, "STLS", 4))
      pop3c->tls_supported = TRUE;

    /* Does the server support clear text authentication? */
    अन्यथा अगर(len >= 4 && curl_strnequal(पंक्ति, "USER", 4))
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    /* Does the server support SASL based authentication? */
    अन्यथा अगर(len >= 5 && curl_strnequal(पंक्ति, "SASL ", 5)) {
      pop3c->authtypes |= POP3_TYPE_SASL;

      /* Advance past the SASL keyword */
      पंक्ति += 5;
      len -= 5;

      /* Loop through the data line */
      क्रम(;;) {
        माप_प्रकार llen;
        माप_प्रकार wordlen = 0;
        अचिन्हित लघु mechbit;

        जबतक(len && (ISBLANK(*पंक्ति) || ISNEWLINE(*पंक्ति))) {
          पंक्ति++;
          len--;
        }

        अगर(!len)
          अवरोध;

        /* Extract the word */
        जबतक(wordlen < len && !ISBLANK(पंक्ति[wordlen]) &&
              !ISNEWLINE(पंक्ति[wordlen]))
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(पंक्ति, wordlen, &llen);
        अगर(mechbit && llen == wordlen)
          pop3c->sasl.authmechs |= mechbit;

        पंक्ति += wordlen;
        len -= wordlen;
      }
    }
  }
  अन्यथा {
    /* Clear text is supported when CAPA is not recognised */
    अगर(pop3code != '+')
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    अगर(!तथ्य->set.use_ssl || Curl_conn_is_ssl(conn, FIRSTSOCKET))
      परिणाम = pop3_perform_authentication(तथ्य, conn);
    अन्यथा अगर(pop3code == '+' && pop3c->tls_supported)
      /* Switch to TLS connection now */
      परिणाम = pop3_perform_starttls(तथ्य, conn);
    अन्यथा अगर(तथ्य->set.use_ssl <= CURLUSESSL_TRY)
      /* Fallback and carry on with authentication */
      परिणाम = pop3_perform_authentication(तथ्य, conn);
    अन्यथा {
      failf(तथ्य, "STLS not supported.");
      परिणाम = CURLE_USE_SSL_FAILED;
    }
  }

  वापस परिणाम;
}

/* For STARTTLS responses */
अटल CURLcode pop3_state_starttls_resp(काष्ठा Curl_easy *तथ्य,
                                         काष्ठा connectdata *conn,
                                         पूर्णांक pop3code,
                                         pop3state instate)
{
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode परिणाम = CURLE_OK;
  (व्योम)instate;

  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  /* Pipelining in response is forbidden. */
  अगर(pop3c->pp.overflow)
    वापस CURLE_WEIRD_SERVER_REPLY;

  अगर(pop3code != '+') {
    अगर(तथ्य->set.use_ssl != CURLUSESSL_TRY) {
      failf(तथ्य, "STARTTLS denied");
      परिणाम = CURLE_USE_SSL_FAILED;
    }
    अन्यथा
      परिणाम = pop3_perform_authentication(तथ्य, conn);
  }
  अन्यथा
    pop3_state(तथ्य, POP3_UPGRADETLS);

  वापस परिणाम;
}

/* For SASL authentication responses */
अटल CURLcode pop3_state_auth_resp(काष्ठा Curl_easy *तथ्य,
                                     पूर्णांक pop3code,
                                     pop3state instate)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  saslprogress progress;

  (व्योम)instate;
  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  परिणाम = Curl_sasl_continue(&pop3c->sasl, तथ्य, pop3code, &progress);
  अगर(!परिणाम)
    चयन(progress) {
    हाल SASL_DONE:
      pop3_state(तथ्य, POP3_STOP);  /* Authenticated */
      अवरोध;
    हाल SASL_IDLE:            /* No mechanism left after cancellation */
#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH
      अगर(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
        /* Perform APOP authentication */
        परिणाम = pop3_perform_apop(तथ्य, conn);
      अन्यथा
#पूर्ण_अगर
      अगर(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
        /* Perform clear text authentication */
        परिणाम = pop3_perform_user(तथ्य, conn);
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

#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH
/* For APOP responses */
अटल CURLcode pop3_state_apop_resp(काष्ठा Curl_easy *तथ्य, पूर्णांक pop3code,
                                     pop3state instate)
{
  CURLcode परिणाम = CURLE_OK;
  (व्योम)instate;

  अगर(pop3code != '+') {
    failf(तथ्य, "Authentication failed: %d", pop3code);
    परिणाम = CURLE_LOGIN_DENIED;
  }
  अन्यथा
    /* End of connect phase */
    pop3_state(तथ्य, POP3_STOP);

  वापस परिणाम;
}
#पूर्ण_अगर

/* For USER responses */
अटल CURLcode pop3_state_user_resp(काष्ठा Curl_easy *तथ्य, पूर्णांक pop3code,
                                     pop3state instate)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (व्योम)instate;

  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  अगर(pop3code != '+') {
    failf(तथ्य, "Access denied. %c", pop3code);
    परिणाम = CURLE_LOGIN_DENIED;
  }
  अन्यथा
    /* Send the PASS command */
    परिणाम = Curl_pp_sendf(तथ्य, &pop3c->pp, "PASS %s",
                           Curl_creds_passwd(conn->creds));
  अगर(!परिणाम)
    pop3_state(तथ्य, POP3_PASS);

  वापस परिणाम;
}

/* For PASS responses */
अटल CURLcode pop3_state_pass_resp(काष्ठा Curl_easy *तथ्य, पूर्णांक pop3code,
                                     pop3state instate)
{
  CURLcode परिणाम = CURLE_OK;
  (व्योम)instate;

  अगर(pop3code != '+') {
    failf(तथ्य, "Access denied. %c", pop3code);
    परिणाम = CURLE_LOGIN_DENIED;
  }
  अन्यथा
    /* End of connect phase */
    pop3_state(तथ्य, POP3_STOP);

  वापस परिणाम;
}

/***********************************************************************
 *
 * pop3_write()
 *
 * This function scans the body after the end-of-body and writes everything
 * until the end is found.
 */
अटल CURLcode pop3_write(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *str,
                           माप_प्रकार nread, bool is_eos)
{
  /* This code could be made into a special function in the handler struct */
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  bool strip_dot = FALSE;
  माप_प्रकार last = 0;
  माप_प्रकार i;
  (व्योम)is_eos;

  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  /* Search through the buffer looking for the end-of-body marker which is
     5 bytes (0d 0a 2e 0d 0a). Note that a line starting with a dot matches
     the eob so the server will have prefixed it with an extra dot which we
     need to strip out. Additionally the marker could of course be spread out
     over 5 different data chunks. */
  क्रम(i = 0; i < nread; i++) {
    माप_प्रकार पूर्व = pop3c->eob;

    चयन(str[i]) {
    हाल 0x0d:
      अगर(pop3c->eob == 0) {
        pop3c->eob++;

        अगर(i) {
          /* Write out the body part that did not match */
          परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, &str[last],
                                     i - last);

          अगर(परिणाम)
            वापस परिणाम;

          last = i;
        }
      }
      अन्यथा अगर(pop3c->eob == 3)
        pop3c->eob++;
      अन्यथा
        /* If the character match was not at position 0 or 3 then restart the
           pattern matching */
        pop3c->eob = 1;
      अवरोध;

    हाल 0x0a:
      अगर(pop3c->eob == 1 || pop3c->eob == 4)
        pop3c->eob++;
      अन्यथा
        /* If the character match was not at position 1 or 4 then start the
           search again */
        pop3c->eob = 0;
      अवरोध;

    हाल 0x2e:
      अगर(pop3c->eob == 2)
        pop3c->eob++;
      अन्यथा अगर(pop3c->eob == 3) {
        /* We have an extra dot after the CRLF which we need to strip off */
        strip_dot = TRUE;
        pop3c->eob = 0;
      }
      अन्यथा
        /* If the character match was not at position 2 then start the search
           again */
        pop3c->eob = 0;
      अवरोध;

    शेष:
      pop3c->eob = 0;
      अवरोध;
    }

    /* Did we have a partial match which has subsequently failed? */
    अगर(पूर्व && पूर्व >= pop3c->eob) {
      /* Strip can only be non-zero for the first mismatch after CRLF and
         then both prev and strip are equal and nothing will be output below */
      जबतक(पूर्व && pop3c->strip) {
        पूर्व--;
        pop3c->strip--;
      }

      अगर(पूर्व) {
        /* If the partial match was the CRLF and dot then only write the CRLF
           as the server would have inserted the dot */
        अगर(strip_dot && पूर्व - 1 > 0) {
          परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, POP3_EOB,
                                     पूर्व - 1);
        }
        अन्यथा अगर(!strip_dot) {
          परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, POP3_EOB,
                                     पूर्व);
        }
        अन्यथा {
          परिणाम = CURLE_OK;
        }

        अगर(परिणाम)
          वापस परिणाम;

        last = i;
        strip_dot = FALSE;
      }
    }
  }

  अगर(pop3c->eob == POP3_EOB_LEN) {
    /* We have a full match so the transfer is done, however we must transfer
    the CRLF at the start of the EOB as this is considered to be part of the
    message as per RFC-1939, sect. 3 */
    परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, POP3_EOB, 2);

    CURL_REQ_CLEAR_RECV(तथ्य);
    pop3c->eob = 0;

    वापस परिणाम;
  }

  अगर(pop3c->eob)
    /* While EOB is matching nothing should be output */
    वापस CURLE_OK;

  अगर(nread - last) {
    परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, &str[last],
                               nread - last);
  }

  वापस परिणाम;
}

/* For command responses */
अटल CURLcode pop3_state_command_resp(काष्ठा Curl_easy *तथ्य,
                                        पूर्णांक pop3code,
                                        pop3state instate)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा POP3 *pop3 = Curl_meta_get(तथ्य, CURL_META_POP3_EASY);
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  काष्ठा pingpong *pp;

  (व्योम)instate;
  अगर(!pop3 || !pop3c)
    वापस CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  अगर(pop3code != '+') {
    pop3_state(तथ्य, POP3_STOP);
    वापस CURLE_WEIRD_SERVER_REPLY;
  }

  /* This 'OK' line ends with a CR LF pair which is the two first bytes of the
     EOB string so count this is two matching bytes. This is necessary to make
     the code detect the EOB if the only data than comes now is %2e CR LF like
     when there is no body to return. */
  pop3c->eob = 2;

  /* Since this initial CR LF pair is not part of the actual body, we set
     the strip counter here so that these bytes will not be delivered. */
  pop3c->strip = 2;

  अगर(pop3->transfer == PPTRANSFER_BODY) {
    /* POP3 download */
    Curl_xfer_setup_recv(तथ्य, FIRSTSOCKET, -1);

    अगर(pp->overflow) {
      /* The recv buffer contains data that is actually body content so send
         it as such. Note that there may even be additional "headers" after
         the body */

      /* keep only the overflow */
      curlx_dyn_पुच्छ(&pp->recvbuf, pp->overflow);
      pp->nfinal = 0; /* done */

      अगर(!तथ्य->req.no_body) {
        परिणाम = pop3_write(तथ्य, curlx_dyn_ptr(&pp->recvbuf),
                            curlx_dyn_len(&pp->recvbuf), FALSE);
        अगर(परिणाम)
          वापस परिणाम;
      }

      /* reset the buffer */
      curlx_dyn_reset(&pp->recvbuf);
      pp->overflow = 0;
    }
  }
  अन्यथा
    pp->overflow = 0;

  /* End of DO phase */
  pop3_state(तथ्य, POP3_STOP);

  वापस परिणाम;
}

अटल CURLcode pop3_statemachine(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata *conn)
{
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक pop3code;
  काष्ठा pingpong *pp;
  माप_प्रकार nread = 0;
  (व्योम)तथ्य;

  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not POP3 */
upgrade_tls:
  अगर(pop3c->state == POP3_UPGRADETLS) {
    परिणाम = pop3_perform_upgrade_tls(तथ्य, conn);
    अगर(परिणाम || (pop3c->state == POP3_UPGRADETLS))
      वापस परिणाम;
  }

  /* Flush any data that needs to be sent */
  अगर(pp->sendleft)
    वापस Curl_pp_flushsend(तथ्य, pp);

  करो {
     /* Read the response from the server */
    परिणाम = Curl_pp_readresp(तथ्य, FIRSTSOCKET, pp, &pop3code, &nread);
    अगर(परिणाम)
      वापस परिणाम;

    अगर(!pop3code)
      अवरोध;

    /* We have now received a full POP3 server response */
    चयन(pop3c->state) {
    हाल POP3_SERVERGREET:
      परिणाम = pop3_state_servergreet_resp(तथ्य, pop3code, pop3c->state);
      अवरोध;

    हाल POP3_CAPA:
      परिणाम = pop3_state_capa_resp(तथ्य, pop3code, pop3c->state);
      अवरोध;

    हाल POP3_STARTTLS:
      परिणाम = pop3_state_starttls_resp(तथ्य, conn, pop3code, pop3c->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      अगर(!परिणाम && (pop3c->state == POP3_UPGRADETLS))
        जाओ upgrade_tls;
      अवरोध;

    हाल POP3_AUTH:
      परिणाम = pop3_state_auth_resp(तथ्य, pop3code, pop3c->state);
      अवरोध;

#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH
    हाल POP3_APOP:
      परिणाम = pop3_state_apop_resp(तथ्य, pop3code, pop3c->state);
      अवरोध;
#पूर्ण_अगर

    हाल POP3_USER:
      परिणाम = pop3_state_user_resp(तथ्य, pop3code, pop3c->state);
      अवरोध;

    हाल POP3_PASS:
      परिणाम = pop3_state_pass_resp(तथ्य, pop3code, pop3c->state);
      अवरोध;

    हाल POP3_COMMAND:
      परिणाम = pop3_state_command_resp(तथ्य, pop3code, pop3c->state);
      अवरोध;

    हाल POP3_QUIT:
      pop3_state(तथ्य, POP3_STOP);
      अवरोध;

    शेष:
      /* internal error */
      pop3_state(तथ्य, POP3_STOP);
      अवरोध;
    }
  } जबतक(!परिणाम && pop3c->state != POP3_STOP && Curl_pp_moredata(pp));

  वापस परिणाम;
}

/* Called repeatedly until done from multi.c */
अटल CURLcode pop3_multi_statemach(काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;
  परिणाम = Curl_pp_statemach(तथ्य, &pop3c->pp, FALSE, FALSE);
  *done = (pop3c->state == POP3_STOP);

  वापस परिणाम;
}

अटल CURLcode pop3_block_statemach(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा connectdata *conn,
                                     bool disconnecting)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  जबतक(pop3c->state != POP3_STOP && !परिणाम)
    परिणाम = Curl_pp_statemach(तथ्य, &pop3c->pp, TRUE, disconnecting);

  वापस परिणाम;
}

/* For the POP3 "protocol connect" and "doing" phases only */
अटल CURLcode pop3_pollset(काष्ठा Curl_easy *तथ्य,
                             काष्ठा easy_pollset *ps)
{
  काष्ठा pop3_conn *pop3c =
    Curl_conn_meta_get(तथ्य->conn, CURL_META_POP3_CONN);
  वापस pop3c ? Curl_pp_pollset(तथ्य, &pop3c->pp, ps) : CURLE_OK;
}

/* SASL parameters for the pop3 protocol */
अटल स्थिर काष्ठा SASLproto saslpop3 = {
  "pop",                /* The service name */
  pop3_perform_auth,    /* Send authentication command */
  pop3_continue_auth,   /* Send authentication continuation */
  pop3_cancel_auth,     /* Send authentication cancellation */
  pop3_get_message,     /* Get SASL response message */
  255 - 8,              /* Max line len - strlen("AUTH ") - 1 space - CRLF */
  '*',                  /* Code received when continuation is expected */
  '+',                  /* Code to receive upon authentication success */
  SASL_AUTH_DEFAULT,    /* Default mechanisms */
  SASL_FLAG_BASE64      /* Configuration flags */
};

/***********************************************************************
 *
 * pop3_connect()
 *
 * This function should do everything that is to be considered a part of the
 * connection phase.
 *
 * The variable 'done' points to will be TRUE if the protocol-layer connect
 * phase is done when this function returns, or FALSE if not.
 */
अटल CURLcode pop3_connect(काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  काष्ठा pingpong *pp = pop3c ? &pop3c->pp : शून्य;

  *done = FALSE; /* default to not done yet */
  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  PINGPONG_SETUP(pp, pop3_statemachine, pop3_endofresp);

  /* Set the default preferred authentication type and mechanism */
  pop3c->preftype = POP3_TYPE_ANY;
  Curl_sasl_init(&pop3c->sasl, तथ्य, &saslpop3);

  /* Initialize the pingpong layer */
  Curl_pp_init(pp, Curl_pgrs_now(तथ्य));

  /* Parse the URL options */
  परिणाम = pop3_parse_url_options(conn);
  अगर(परिणाम)
    वापस परिणाम;

  /* Start off waiting for the server greeting response */
  pop3_state(तथ्य, POP3_SERVERGREET);

  परिणाम = pop3_multi_statemach(तथ्य, done);

  वापस परिणाम;
}

/***********************************************************************
 *
 * pop3_done()
 *
 * The DONE function. This does what needs to be done after a single DO has
 * performed.
 *
 * Input argument is already checked for validity.
 */
अटल CURLcode pop3_done(काष्ठा Curl_easy *तथ्य, CURLcode status,
                          bool premature)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा POP3 *pop3 = Curl_meta_get(तथ्य, CURL_META_POP3_EASY);

  (व्योम)premature;

  अगर(!pop3)
    वापस CURLE_OK;

  अगर(status) {
    CURL_TRC_M(तथ्य, "POP3 done with bad status");
    connclose(तथ्य->conn);
    परिणाम = status;         /* use the already set error code */
  }

  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);

  /* Clear the transfer mode for the next request */
  pop3->transfer = PPTRANSFER_BODY;

  वापस परिणाम;
}

/***********************************************************************
 *
 * pop3_perform()
 *
 * This is the actual DO function for POP3. Get a message/listing according to
 * the options previously setup.
 */
अटल CURLcode pop3_perform(काष्ठा Curl_easy *तथ्य, bool *connected,
                             bool *dophase_done)
{
  /* This is POP3 and no proxy */
  CURLcode परिणाम = CURLE_OK;
  काष्ठा POP3 *pop3 = Curl_meta_get(तथ्य, CURL_META_POP3_EASY);

  अगर(!pop3)
    वापस CURLE_FAILED_INIT;

  DEBUGF(infof(तथ्य, "DO phase starts"));

  /* Start the first command in the DO phase, may alter data->req.no_body */
  परिणाम = pop3_perform_command(तथ्य);
  अगर(परिणाम)
    वापस परिणाम;

  अगर(तथ्य->req.no_body)
    /* Requested no body means no transfer */
    pop3->transfer = PPTRANSFER_INFO;

  *dophase_done = FALSE; /* not done yet */

  /* Run the state-machine */
  परिणाम = pop3_multi_statemach(तथ्य, dophase_done);
  *connected = Curl_conn_is_connected(तथ्य->conn, FIRSTSOCKET);

  अगर(*dophase_done)
    DEBUGF(infof(तथ्य, "DO phase is complete"));

  वापस परिणाम;
}

/* Call this when the DO phase has completed */
अटल CURLcode pop3_dophase_done(काष्ठा Curl_easy *तथ्य, bool connected)
{
  (व्योम)तथ्य;
  (व्योम)connected;

  वापस CURLE_OK;
}

/***********************************************************************
 *
 * pop3_regular_transfer()
 *
 * The input argument is already checked for validity.
 *
 * Performs all commands done before a regular transfer between a local and a
 * remote host.
 */
अटल CURLcode pop3_regular_transfer(काष्ठा Curl_easy *तथ्य,
                                      bool *dophase_done)
{
  CURLcode परिणाम = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  तथ्य->req.आकार = -1;

  /* Set the progress data */
  Curl_pgrsReset(तथ्य);

  /* Carry out the perform */
  परिणाम = pop3_perform(तथ्य, &connected, dophase_done);

  /* Perform post DO phase operations if necessary */
  अगर(!परिणाम && *dophase_done)
    परिणाम = pop3_dophase_done(तथ्य, connected);

  वापस परिणाम;
}

/***********************************************************************
 *
 * pop3_do()
 *
 * This function is registered as 'curl_do' function. It decodes the path
 * parts etc as a wrapper to the actual DO function (pop3_perform).
 *
 * The input argument is already checked for validity.
 */
अटल CURLcode pop3_do(काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम = CURLE_OK;
  *done = FALSE; /* default to false */

  /* Parse the URL path */
  परिणाम = pop3_parse_url_path(तथ्य);
  अगर(परिणाम)
    वापस परिणाम;

  /* Parse the custom request */
  परिणाम = pop3_parse_custom_request(तथ्य);
  अगर(परिणाम)
    वापस परिणाम;

  परिणाम = pop3_regular_transfer(तथ्य, done);

  वापस परिणाम;
}

/***********************************************************************
 *
 * pop3_disconnect()
 *
 * Disconnect from an POP3 server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
अटल CURLcode pop3_disconnect(काष्ठा Curl_easy *तथ्य,
                                काष्ठा connectdata *conn, bool dead_connection)
{
  काष्ठा pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (व्योम)तथ्य;

  अगर(!pop3c)
    वापस CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  अगर(!dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(तथ्य, &pop3c->pp) &&
     !pop3_perform_quit(तथ्य, conn))
    (व्योम)pop3_block_statemach(तथ्य, conn, TRUE); /* ignore errors on QUIT */

  /* Disconnect from the server */
  Curl_pp_disconnect(&pop3c->pp);

  /* Cleanup our connection based variables */
  curlx_safefree(pop3c->apoptimestamp);

  वापस CURLE_OK;
}

/* Called from multi.c while DOing */
अटल CURLcode pop3_doing(काष्ठा Curl_easy *तथ्य, bool *dophase_done)
{
  CURLcode परिणाम = pop3_multi_statemach(तथ्य, dophase_done);

  अगर(परिणाम)
    DEBUGF(infof(तथ्य, "DO phase failed"));
  अन्यथा अगर(*dophase_done) {
    परिणाम = pop3_dophase_done(तथ्य, FALSE /* not connected */);

    DEBUGF(infof(तथ्य, "DO phase is complete"));
  }

  वापस परिणाम;
}

अटल व्योम pop3_easy_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा POP3 *pop3 = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  DEBUGASSERT(pop3);
  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);
  curlx_free(pop3);
}

अटल व्योम pop3_conn_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा pop3_conn *pop3c = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  DEBUGASSERT(pop3c);
  Curl_pp_disconnect(&pop3c->pp);
  curlx_safefree(pop3c->apoptimestamp);
  curlx_free(pop3c);
}

अटल CURLcode pop3_setup_connection(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा connectdata *conn)
{
  काष्ठा pop3_conn *pop3c;
  काष्ठा POP3 *pop3 = curlx_calloc(1, माप(*pop3));
  अगर(!pop3 ||
     Curl_meta_set(तथ्य, CURL_META_POP3_EASY, pop3, pop3_easy_dtor))
    वापस CURLE_OUT_OF_MEMORY;

  pop3c = curlx_calloc(1, माप(*pop3c));
  अगर(!pop3c ||
     Curl_conn_meta_set(conn, CURL_META_POP3_CONN, pop3c, pop3_conn_dtor))
    वापस CURLE_OUT_OF_MEMORY;

  वापस CURLE_OK;
}

/*
 * POP3 protocol.
 */
स्थिर काष्ठा Curl_protocol Curl_protocol_pop3 = {
  pop3_setup_connection,            /* setup_connection */
  pop3_do,                          /* do_it */
  pop3_done,                        /* done */
  ZERO_NULL,                        /* do_more */
  pop3_connect,                     /* connect_it */
  pop3_multi_statemach,             /* connecting */
  pop3_doing,                       /* doing */
  pop3_pollset,                     /* proto_pollset */
  pop3_pollset,                     /* doing_pollset */
  ZERO_NULL,                        /* domore_pollset */
  ZERO_NULL,                        /* perform_pollset */
  pop3_disconnect,                  /* disconnect */
  pop3_write,                       /* write_resp */
  ZERO_NULL,                        /* write_resp_hd */
  ZERO_NULL,                        /* connection_is_dead */
  ZERO_NULL,                        /* attach connection */
  ZERO_NULL,                        /* follow */
};

#पूर्ण_अगर /* CURL_DISABLE_POP3 */
