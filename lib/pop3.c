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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "pop3.h"

#ifndef CURL_DISABLE_POP3

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

#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "pingpong.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "curl_sasl.h"
#अन्तर्भाव "curl_md5.h"
#अन्तर्भाव "curlx/strdup.h"

/* Authentication type flags */
#define POP3_TYPE_CLEARTEXT (1 << 0)
#define POP3_TYPE_APOP      (1 << 1)
#define POP3_TYPE_SASL      (1 << 2)

/* Authentication type values */
#define POP3_TYPE_NONE 0
#define POP3_TYPE_ANY  (POP3_TYPE_CLEARTEXT | POP3_TYPE_APOP | POP3_TYPE_SASL)

/* This is the 5-bytes End-Of-Body marker for POP3 */
#define POP3_EOB     "\x0d\x0a\x2e\x0d\x0a"
#define POP3_EOB_LEN 5

/* meta key for storing protocol meta at easy handle */
#define CURL_META_POP3_EASY   "meta:proto:pop3:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_POP3_CONN   "meta:proto:pop3:conn"

/*
 * POP3 easy handle state
 */
संरचना POP3 {
  curl_pp_transfer transfer;
  अक्षर *id;               /* Message ID */
  अक्षर *custom;           /* Custom Request */
};

/*
 * POP3 connection state
 */
प्रकारव्याख्या गणना {
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

संरचना pop3_conn {
  संरचना pingpong pp;
  pop3state state;        /* Always use pop3.c:state() to change state! */
  माप_प्रकार eob;             /* Number of bytes of the EOB (End Of Body) that
                             have been received so far */
  माप_प्रकार strip;           /* Number of bytes from the start to ignore as
                             non-body */
  संरचना SASL sasl;       /* SASL-related storage */
  अक्षर *apoptimestamp;    /* APOP timestamp from the server greeting */
  अचिन्हित अक्षर authtypes; /* Accepted authentication types */
  अचिन्हित अक्षर preftype;  /* Preferred authentication type */
  BIT(ssldone);           /* Is connect() over SSL done? */
  BIT(tls_supported);     /* StartTLS capability supported by server */
};

संरचना pop3_cmd {
  स्थिरांक अक्षर *name;
  अचिन्हित लघु nlen;
  BIT(multiline); /* response is multi-line with last '.' line */
  BIT(multiline_with_args); /* is multi-line when command has args */
};

स्थिर स्थिरांक संरचना pop3_cmd pop3cmds[] = {
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
स्थिर CURLcode pop3_parse_url_options(संरचना connectdata *conn)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  स्थिरांक अक्षर *ptr = conn->options;

  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  पर्यंत(!result && ptr && *ptr) {
    स्थिरांक अक्षर *किल्ली = ptr;
    स्थिरांक अक्षर *मूल्य;

    पर्यंत(*ptr && *ptr != '=')
      ptr++;

    मूल्य = ptr + 1;

    पर्यंत(*ptr && *ptr != ';')
      ptr++;

    जर(curl_strnequal(किल्ली, "AUTH=", 5)) {
      result = Curl_sasl_parse_url_auth_option(&pop3c->sasl,
                                               मूल्य, ptr - मूल्य);

      जर(result && curl_strnequal(मूल्य, "+APOP", ptr - मूल्य)) {
        pop3c->preftype = POP3_TYPE_APOP;
        pop3c->sasl.prefmech = SASL_AUTH_NONE;
        result = CURLE_OK;
      }
    }
    अन्यथा
      result = CURLE_URL_MALFORMAT;

    जर(*ptr == ';')
      ptr++;
  }

  जर(pop3c->preftype != POP3_TYPE_APOP)
    निवड(pop3c->sasl.prefmech) {
    स्थिती SASL_AUTH_NONE:
      pop3c->preftype = POP3_TYPE_NONE;
      थांबा;
    स्थिती SASL_AUTH_DEFAULT:
      pop3c->preftype = POP3_TYPE_ANY;
      थांबा;
    पूर्वनिर्धारित:
      pop3c->preftype = POP3_TYPE_SASL;
      थांबा;
    }

  परत result;
}

/***********************************************************************
 *
 * pop3_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
स्थिर CURLcode pop3_parse_url_path(संरचना Curl_easy *data)
{
  /* The POP3 struct is already initialized in pop3_connect() */
  संरचना POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  स्थिरांक अक्षर *path = &data->state.up.path[1]; /* skip leading path */

  जर(!pop3)
    परत CURLE_FAILED_INIT;
  /* URL decode the path for the message ID */
  परत Curl_urldecode(path, 0, &pop3->id, रिक्त, REJECT_CTRL);
}

/***********************************************************************
 *
 * pop3_parse_custom_request()
 *
 * Parse the custom request.
 */
स्थिर CURLcode pop3_parse_custom_request(संरचना Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  संरचना POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  स्थिरांक अक्षर *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  जर(!pop3)
    परत CURLE_FAILED_INIT;
  /* URL decode the custom request */
  जर(custom)
    result = Curl_urldecode(custom, 0, &pop3->custom, रिक्त, REJECT_CTRL);

  परत result;
}

/* Return iff a command is defined as "multi-line" (RFC 1939),
 * has a response terminated by a last line with a '.'.
 */
स्थिर bool pop3_is_multiline(स्थिरांक अक्षर *cmdline)
{
  माप_प्रकार i;
  साठी(i = 0; i < CURL_ARRAYSIZE(pop3cmds); ++i) {
    जर(curl_strnequal(pop3cmds[i].name, cmdline, pop3cmds[i].nlen)) {
      जर(!cmdline[pop3cmds[i].nlen])
        परत (bool)pop3cmds[i].multiline;
      अन्यथा जर(cmdline[pop3cmds[i].nlen] == ' ')
        परत (bool)pop3cmds[i].multiline_with_args;
    }
  }
  /* Unknown command, assume multi-line for backward compatibility with
   * earlier curl versions that only could do multi-line responses. */
  परत TRUE;
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
स्थिर bool pop3_endofresp(संरचना Curl_easy *data, संरचना connectdata *conn,
                           स्थिरांक अक्षर *ओळ, माप_प्रकार len, पूर्णांक *resp)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (शून्य)data;
  DEBUGASSERT(pop3c);
  जर(!pop3c) /* internal error */
    परत TRUE;

  /* Do we have an error response? */
  जर(len >= 4 && !स_भेद("-ERR", ओळ, 4)) {
    *resp = '-';

    परत TRUE;
  }

  /* Are we processing CAPA command responses? */
  जर(pop3c->state == POP3_CAPA) {
    /* Do we have the terminating line? Per RFC 2449 this is a line
       containing only a single dot */
    जर((len == 3 && ओळ[0] == '.' && ओळ[1] == '\r') ||
       (len == 2 && ओळ[0] == '.' && ओळ[1] == '\n'))
      /* Treat the response as a success */
      *resp = '+';
    अन्यथा
      /* Treat the response as an untagged continuation */
      *resp = '*';

    परत TRUE;
  }

  /* Do we have a success response? */
  जर(len >= 3 && !स_भेद("+OK", ओळ, 3)) {
    *resp = '+';

    परत TRUE;
  }

  /* Do we have a continuation response? */
  जर(len >= 1 && ओळ[0] == '+') {
    *resp = '*';

    परत TRUE;
  }

  परत FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * pop3_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
स्थिर CURLcode pop3_get_message(संरचना Curl_easy *data, संरचना bufref *out)
{
  संरचना pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  अक्षर *message;
  माप_प्रकार len;

  जर(!pop3c)
    परत CURLE_FAILED_INIT;
  message = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;
  जर(len > 2) {
    /* Find the start of the message */
    len -= 2;
    साठी(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    पर्यंत(len--)
      जर(!ISBLANK(message[len]) && !ISNEWLINE(message[len]))
        थांबा;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, रिक्त);
  }
  अन्यथा
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, रिक्त);

  परत CURLE_OK;
}

/***********************************************************************
 *
 * pop3_state()
 *
 * This is the ONLY way to change POP3 state!
 */
स्थिर शून्य pop3_state(संरचना Curl_easy *data, pop3state newstate)
{
  संरचना pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  जर(pop3c) {
#जर defined(DEBUGBUILD) && defined(CURLVERBOSE)
    /* for debug purposes */
    स्थिर स्थिरांक अक्षर * स्थिरांक names[] = {
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

    जर(pop3c->state != newstate)
      infof(data, "POP3 %p state change from %s to %s",
            (शून्य *)pop3c, names[pop3c->state], names[newstate]);
#endif

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
स्थिर CURLcode pop3_perform_capa(संरचना Curl_easy *data,
                                  संरचना connectdata *conn)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;

  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  pop3c->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  pop3c->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  pop3c->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPA command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "CAPA");

  जर(!result)
    pop3_state(data, POP3_CAPA);

  परत result;
}

/***********************************************************************
 *
 * pop3_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
स्थिर CURLcode pop3_perform_starttls(संरचना Curl_easy *data,
                                      संरचना connectdata *conn)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;

  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  /* Send the STLS command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "STLS");
  जर(!result)
    pop3_state(data, POP3_STARTTLS);

  परत result;
}

/***********************************************************************
 *
 * pop3_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
स्थिर CURLcode pop3_perform_upgrade_tls(संरचना Curl_easy *data,
                                         संरचना connectdata *conn)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;
  bool ssldone = FALSE;

  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  जर(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    जर(result)
      जा out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_pop3s;
  }

  DEBUGASSERT(!pop3c->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "pop3_perform_upgrade_tls, connect -> %d, %d",
               (पूर्णांक)result, ssldone));
  जर(!result && ssldone) {
    pop3c->ssldone = ssldone;
    /* perform CAPA now, changes pop3c->state out of POP3_UPGRADETLS */
    result = pop3_perform_capa(data, conn);
  }
out:
  परत result;
#अन्यथा
  (शून्य)data;
  (शून्य)conn;
  परत CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * pop3_perform_user()
 *
 * Sends a clear text USER command to authenticate with.
 */
स्थिर CURLcode pop3_perform_user(संरचना Curl_easy *data,
                                  संरचना connectdata *conn)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;

  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  जर(!conn->creds) {
    pop3_state(data, POP3_STOP);

    परत result;
  }

  /* Send the USER command */
  result = Curl_pp_sendf(data, &pop3c->pp, "USER %s",
                         Curl_creds_user(conn->creds));
  जर(!result)
    pop3_state(data, POP3_USER);

  परत result;
}

#ifndef CURL_DISABLE_DIGEST_AUTH
/***********************************************************************
 *
 * pop3_perform_apop()
 *
 * Sends an APOP command to authenticate with.
 */
स्थिर CURLcode pop3_perform_apop(संरचना Curl_easy *data,
                                  संरचना connectdata *conn)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  माप_प्रकार i;
  संरचना MD5_context *ctxt;
  अचिन्हित अक्षर digest[MD5_DIGEST_LEN];
  अक्षर secret[(2 * MD5_DIGEST_LEN) + 1];

  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  जर(!data->state.creds) {
    pop3_state(data, POP3_STOP);

    परत result;
  }

  /* Create the digest */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  जर(!ctxt)
    परत CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)pop3c->apoptimestamp,
                  curlx_uztoui(म_माप(pop3c->apoptimestamp)));

  Curl_MD5_update(ctxt, (स्थिरांक अचिन्हित अक्षर *)Curl_creds_passwd(conn->creds),
                  curlx_uztoui(म_माप(Curl_creds_passwd(conn->creds))));

  /* Finalise the digest */
  Curl_MD5_final(ctxt, digest);

  /* Convert the calculated 16 octet digest into a 32-byte hex string */
  साठी(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&secret[2 * i], 3, "%02x", digest[i]);

  result = Curl_pp_sendf(data, &pop3c->pp, "APOP %s %s",
                         Curl_creds_user(conn->creds), secret);

  जर(!result)
    pop3_state(data, POP3_APOP);

  परत result;
}
#endif

/***********************************************************************
 *
 * pop3_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
स्थिर CURLcode pop3_perform_auth(संरचना Curl_easy *data,
                                  स्थिरांक अक्षर *mech,
                                  स्थिरांक संरचना bufref *initresp)
{
  संरचना pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  स्थिरांक अक्षर *ir = Curl_bufref_ptr(initresp);

  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  जर(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    result = Curl_pp_sendf(data, &pop3c->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  अन्यथा {
    /* Send the AUTH command */
    result = Curl_pp_sendf(data, &pop3c->pp, "AUTH %s", mech);
  }

  परत result;
}

/***********************************************************************
 *
 * pop3_continue_auth()
 *
 * Sends SASL continuation data.
 */
स्थिर CURLcode pop3_continue_auth(संरचना Curl_easy *data,
                                   स्थिरांक अक्षर *mech,
                                   स्थिरांक संरचना bufref *resp)
{
  संरचना pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);

  (शून्य)mech;
  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  परत Curl_pp_sendf(data, &pop3c->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * pop3_cancel_auth()
 *
 * Sends SASL cancellation.
 */
स्थिर CURLcode pop3_cancel_auth(संरचना Curl_easy *data, स्थिरांक अक्षर *mech)
{
  संरचना pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);

  (शून्य)mech;
  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  परत Curl_pp_sendf(data, &pop3c->pp, "*");
}

/***********************************************************************
 *
 * pop3_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to APOP and clear text should a
 * common mechanism not be available between the client and server.
 */
स्थिर CURLcode pop3_perform_authentication(संरचना Curl_easy *data,
                                            संरचना connectdata *conn)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  saslprogress progress = SASL_IDLE;

  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  /* Check we have enough data to authenticate with and end the
     connect phase if we do not */
  जर(!Curl_sasl_can_authenticate(&pop3c->sasl, data)) {
    pop3_state(data, POP3_STOP);
    परत result;
  }

  जर(pop3c->authtypes & pop3c->preftype & POP3_TYPE_SASL) {
    /* Calculate the SASL login details */
    result = Curl_sasl_start(&pop3c->sasl, data, FALSE, &progress);

    जर(!result && progress == SASL_INPROGRESS)
      pop3_state(data, POP3_AUTH);
  }

  जर(!result && progress == SASL_IDLE) {
#ifndef CURL_DISABLE_DIGEST_AUTH
    जर(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
      /* Perform APOP authentication */
      result = pop3_perform_apop(data, conn);
    अन्यथा
#endif
    जर(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
      /* Perform clear text authentication */
      result = pop3_perform_user(data, conn);
    अन्यथा
      result = Curl_sasl_is_blocked(&pop3c->sasl, data);
  }

  परत result;
}

/***********************************************************************
 *
 * pop3_perform_command()
 *
 * Sends a POP3 based command.
 */
स्थिर CURLcode pop3_perform_command(संरचना Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  संरचना POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  स्थिरांक अक्षर *command = रिक्त;

  जर(!pop3 || !pop3c)
    परत CURLE_FAILED_INIT;

  /* Calculate the default command */
  जर(pop3->id[0] == '\0' || data->set.list_only) {
    command = "LIST";

    जर(pop3->id[0] != '\0')
      /* Message specific LIST so skip the BODY transfer */
      pop3->transfer = PPTRANSFER_INFO;
  }
  अन्यथा
    command = "RETR";

  जर(pop3->custom && pop3->custom[0] != '\0')
    command = pop3->custom;

  /* Send the command */
  जर(pop3->id[0] != '\0')
    result = Curl_pp_sendf(data, &pop3c->pp, "%s %s", command, pop3->id);
  अन्यथा
    result = Curl_pp_sendf(data, &pop3c->pp, "%s", command);

  जर(!result) {
    pop3_state(data, POP3_COMMAND);
    data->req.no_body = !pop3_is_multiline(command);
  }

  परत result;
}

/***********************************************************************
 *
 * pop3_perform_quit()
 *
 * Performs the quit action prior to sclose() be called.
 */
स्थिर CURLcode pop3_perform_quit(संरचना Curl_easy *data,
                                  संरचना connectdata *conn)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;

  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  /* Send the QUIT command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "QUIT");
  जर(!result)
    pop3_state(data, POP3_QUIT);

  परत result;
}

/* For the initial server greeting */
स्थिर CURLcode pop3_state_servergreet_resp(संरचना Curl_easy *data,
                                            पूर्णांक pop3code,
                                            pop3state instate)
{
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  स्थिरांक अक्षर *ओळ;
  माप_प्रकार len;

  (शून्य)instate;
  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  ओळ = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  जर(pop3code != '+') {
    failf(data, "Got unexpected pop3-server response");
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  अन्यथा जर(len > 3) {
    /* Does the server support APOP authentication? */
    स्थिरांक अक्षर *lt;
    स्थिरांक अक्षर *gt = रिक्त;

    /* Look for the APOP timestamp */
    lt = स_प्रथम(ओळ, '<', len);
    जर(lt)
      /* search the remainder for '>' */
      gt = स_प्रथम(lt, '>', len - (lt - ओळ));
    जर(gt) {
      /* the length of the timestamp, including the brackets */
      माप_प्रकार timestamplen = gt - lt + 1;
      स्थिरांक अक्षर *at = स_प्रथम(lt, '@', timestamplen);
      /* If the timestamp does not contain '@' it is not (as required by
         RFC-1939) conformant to the RFC-822 message id syntax, and we
         therefore do not use APOP authentication. */
      जर(at) {
        /* dupe the timestamp */
        pop3c->apoptimestamp = curlx_memdup0(lt, timestamplen);
        जर(!pop3c->apoptimestamp)
          परत CURLE_OUT_OF_MEMORY;
        /* Store the APOP capability */
        pop3c->authtypes |= POP3_TYPE_APOP;
      }
    }

    जर(!result)
      result = pop3_perform_capa(data, conn);
  }

  परत result;
}

/* For CAPA responses */
स्थिर CURLcode pop3_state_capa_resp(संरचना Curl_easy *data, पूर्णांक pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  स्थिरांक अक्षर *ओळ;
  माप_प्रकार len;

  (शून्य)instate;
  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  ओळ = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  /* Do we have an untagged continuation response? */
  जर(pop3code == '*') {
    /* Does the server support the STLS capability? */
    जर(len >= 4 && curl_strnequal(ओळ, "STLS", 4))
      pop3c->tls_supported = TRUE;

    /* Does the server support clear text authentication? */
    अन्यथा जर(len >= 4 && curl_strnequal(ओळ, "USER", 4))
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    /* Does the server support SASL based authentication? */
    अन्यथा जर(len >= 5 && curl_strnequal(ओळ, "SASL ", 5)) {
      pop3c->authtypes |= POP3_TYPE_SASL;

      /* Advance past the SASL keyword */
      ओळ += 5;
      len -= 5;

      /* Loop through the data line */
      साठी(;;) {
        माप_प्रकार llen;
        माप_प्रकार wordlen = 0;
        अचिन्हित लघु mechbit;

        पर्यंत(len && (ISBLANK(*ओळ) || ISNEWLINE(*ओळ))) {
          ओळ++;
          len--;
        }

        जर(!len)
          थांबा;

        /* Extract the word */
        पर्यंत(wordlen < len && !ISBLANK(ओळ[wordlen]) &&
              !ISNEWLINE(ओळ[wordlen]))
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(ओळ, wordlen, &llen);
        जर(mechbit && llen == wordlen)
          pop3c->sasl.authmechs |= mechbit;

        ओळ += wordlen;
        len -= wordlen;
      }
    }
  }
  अन्यथा {
    /* Clear text is supported when CAPA is not recognised */
    जर(pop3code != '+')
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    जर(!data->set.use_ssl || Curl_conn_is_ssl(conn, FIRSTSOCKET))
      result = pop3_perform_authentication(data, conn);
    अन्यथा जर(pop3code == '+' && pop3c->tls_supported)
      /* Switch to TLS connection now */
      result = pop3_perform_starttls(data, conn);
    अन्यथा जर(data->set.use_ssl <= CURLUSESSL_TRY)
      /* Fallback and carry on with authentication */
      result = pop3_perform_authentication(data, conn);
    अन्यथा {
      failf(data, "STLS not supported.");
      result = CURLE_USE_SSL_FAILED;
    }
  }

  परत result;
}

/* For STARTTLS responses */
स्थिर CURLcode pop3_state_starttls_resp(संरचना Curl_easy *data,
                                         संरचना connectdata *conn,
                                         पूर्णांक pop3code,
                                         pop3state instate)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  (शून्य)instate;

  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  /* Pipelining in response is forbidden. */
  जर(pop3c->pp.overflow)
    परत CURLE_WEIRD_SERVER_REPLY;

  जर(pop3code != '+') {
    जर(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied");
      result = CURLE_USE_SSL_FAILED;
    }
    अन्यथा
      result = pop3_perform_authentication(data, conn);
  }
  अन्यथा
    pop3_state(data, POP3_UPGRADETLS);

  परत result;
}

/* For SASL authentication responses */
स्थिर CURLcode pop3_state_auth_resp(संरचना Curl_easy *data,
                                     पूर्णांक pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  saslprogress progress;

  (शून्य)instate;
  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  result = Curl_sasl_continue(&pop3c->sasl, data, pop3code, &progress);
  जर(!result)
    निवड(progress) {
    स्थिती SASL_DONE:
      pop3_state(data, POP3_STOP);  /* Authenticated */
      थांबा;
    स्थिती SASL_IDLE:            /* No mechanism left after cancellation */
#ifndef CURL_DISABLE_DIGEST_AUTH
      जर(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
        /* Perform APOP authentication */
        result = pop3_perform_apop(data, conn);
      अन्यथा
#endif
      जर(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
        /* Perform clear text authentication */
        result = pop3_perform_user(data, conn);
      अन्यथा {
        failf(data, "Authentication cancelled");
        result = CURLE_LOGIN_DENIED;
      }
      थांबा;
    पूर्वनिर्धारित:
      थांबा;
    }

  परत result;
}

#ifndef CURL_DISABLE_DIGEST_AUTH
/* For APOP responses */
स्थिर CURLcode pop3_state_apop_resp(संरचना Curl_easy *data, पूर्णांक pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  (शून्य)instate;

  जर(pop3code != '+') {
    failf(data, "Authentication failed: %d", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  अन्यथा
    /* End of connect phase */
    pop3_state(data, POP3_STOP);

  परत result;
}
#endif

/* For USER responses */
स्थिर CURLcode pop3_state_user_resp(संरचना Curl_easy *data, पूर्णांक pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (शून्य)instate;

  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  जर(pop3code != '+') {
    failf(data, "Access denied. %c", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  अन्यथा
    /* Send the PASS command */
    result = Curl_pp_sendf(data, &pop3c->pp, "PASS %s",
                           Curl_creds_passwd(conn->creds));
  जर(!result)
    pop3_state(data, POP3_PASS);

  परत result;
}

/* For PASS responses */
स्थिर CURLcode pop3_state_pass_resp(संरचना Curl_easy *data, पूर्णांक pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  (शून्य)instate;

  जर(pop3code != '+') {
    failf(data, "Access denied. %c", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  अन्यथा
    /* End of connect phase */
    pop3_state(data, POP3_STOP);

  परत result;
}

/***********************************************************************
 *
 * pop3_write()
 *
 * This function scans the body after the end-of-body and writes everything
 * until the end is found.
 */
स्थिर CURLcode pop3_write(संरचना Curl_easy *data, स्थिरांक अक्षर *str,
                           माप_प्रकार nread, bool is_eos)
{
  /* This code could be made into a special function in the handler struct */
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  bool strip_dot = FALSE;
  माप_प्रकार last = 0;
  माप_प्रकार i;
  (शून्य)is_eos;

  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  /* Search through the buffer looking for the end-of-body marker which is
     5 bytes (0d 0a 2e 0d 0a). Note that a line starting with a dot matches
     the eob so the server will have prefixed it with an extra dot which we
     need to strip out. Additionally the marker could of course be spread out
     over 5 different data chunks. */
  साठी(i = 0; i < nread; i++) {
    माप_प्रकार prev = pop3c->eob;

    निवड(str[i]) {
    स्थिती 0x0d:
      जर(pop3c->eob == 0) {
        pop3c->eob++;

        जर(i) {
          /* Write out the body part that did not match */
          result = Curl_client_write(data, CLIENTWRITE_BODY, &str[last],
                                     i - last);

          जर(result)
            परत result;

          last = i;
        }
      }
      अन्यथा जर(pop3c->eob == 3)
        pop3c->eob++;
      अन्यथा
        /* If the character match was not at position 0 or 3 then restart the
           pattern matching */
        pop3c->eob = 1;
      थांबा;

    स्थिती 0x0a:
      जर(pop3c->eob == 1 || pop3c->eob == 4)
        pop3c->eob++;
      अन्यथा
        /* If the character match was not at position 1 or 4 then start the
           search again */
        pop3c->eob = 0;
      थांबा;

    स्थिती 0x2e:
      जर(pop3c->eob == 2)
        pop3c->eob++;
      अन्यथा जर(pop3c->eob == 3) {
        /* We have an extra dot after the CRLF which we need to strip off */
        strip_dot = TRUE;
        pop3c->eob = 0;
      }
      अन्यथा
        /* If the character match was not at position 2 then start the search
           again */
        pop3c->eob = 0;
      थांबा;

    पूर्वनिर्धारित:
      pop3c->eob = 0;
      थांबा;
    }

    /* Did we have a partial match which has subsequently failed? */
    जर(prev && prev >= pop3c->eob) {
      /* Strip can only be non-zero for the first mismatch after CRLF and
         then both prev and strip are equal and nothing will be output below */
      पर्यंत(prev && pop3c->strip) {
        prev--;
        pop3c->strip--;
      }

      जर(prev) {
        /* If the partial match was the CRLF and dot then only write the CRLF
           as the server would have inserted the dot */
        जर(strip_dot && prev - 1 > 0) {
          result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB,
                                     prev - 1);
        }
        अन्यथा जर(!strip_dot) {
          result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB,
                                     prev);
        }
        अन्यथा {
          result = CURLE_OK;
        }

        जर(result)
          परत result;

        last = i;
        strip_dot = FALSE;
      }
    }
  }

  जर(pop3c->eob == POP3_EOB_LEN) {
    /* We have a full match so the transfer is done, however we must transfer
    the CRLF at the start of the EOB as this is considered to be part of the
    message as per RFC-1939, sect. 3 */
    result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB, 2);

    CURL_REQ_CLEAR_RECV(data);
    pop3c->eob = 0;

    परत result;
  }

  जर(pop3c->eob)
    /* While EOB is matching nothing should be output */
    परत CURLE_OK;

  जर(nread - last) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, &str[last],
                               nread - last);
  }

  परत result;
}

/* For command responses */
स्थिर CURLcode pop3_state_command_resp(संरचना Curl_easy *data,
                                        पूर्णांक pop3code,
                                        pop3state instate)
{
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  संरचना POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  संरचना pingpong *pp;

  (शून्य)instate;
  जर(!pop3 || !pop3c)
    परत CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  जर(pop3code != '+') {
    pop3_state(data, POP3_STOP);
    परत CURLE_WEIRD_SERVER_REPLY;
  }

  /* This 'OK' line ends with a CR LF pair which is the two first bytes of the
     EOB string so count this is two matching bytes. This is necessary to make
     the code detect the EOB if the only data than comes now is %2e CR LF like
     when there is no body to return. */
  pop3c->eob = 2;

  /* Since this initial CR LF pair is not part of the actual body, we set
     the strip counter here so that these bytes will not be delivered. */
  pop3c->strip = 2;

  जर(pop3->transfer == PPTRANSFER_BODY) {
    /* POP3 download */
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);

    जर(pp->overflow) {
      /* The recv buffer contains data that is actually body content so send
         it as such. Note that there may even be additional "headers" after
         the body */

      /* keep only the overflow */
      curlx_dyn_tail(&pp->recvbuf, pp->overflow);
      pp->nfinal = 0; /* done */

      जर(!data->req.no_body) {
        result = pop3_write(data, curlx_dyn_ptr(&pp->recvbuf),
                            curlx_dyn_len(&pp->recvbuf), FALSE);
        जर(result)
          परत result;
      }

      /* reset the buffer */
      curlx_dyn_reset(&pp->recvbuf);
      pp->overflow = 0;
    }
  }
  अन्यथा
    pp->overflow = 0;

  /* End of DO phase */
  pop3_state(data, POP3_STOP);

  परत result;
}

स्थिर CURLcode pop3_statemachine(संरचना Curl_easy *data,
                                  संरचना connectdata *conn)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  पूर्णांक pop3code;
  संरचना pingpong *pp;
  माप_प्रकार nread = 0;
  (शून्य)data;

  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not POP3 */
upgrade_tls:
  जर(pop3c->state == POP3_UPGRADETLS) {
    result = pop3_perform_upgrade_tls(data, conn);
    जर(result || (pop3c->state == POP3_UPGRADETLS))
      परत result;
  }

  /* Flush any data that needs to be sent */
  जर(pp->sendleft)
    परत Curl_pp_flushsend(data, pp);

  करा {
     /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, pp, &pop3code, &nread);
    जर(result)
      परत result;

    जर(!pop3code)
      थांबा;

    /* We have now received a full POP3 server response */
    निवड(pop3c->state) {
    स्थिती POP3_SERVERGREET:
      result = pop3_state_servergreet_resp(data, pop3code, pop3c->state);
      थांबा;

    स्थिती POP3_CAPA:
      result = pop3_state_capa_resp(data, pop3code, pop3c->state);
      थांबा;

    स्थिती POP3_STARTTLS:
      result = pop3_state_starttls_resp(data, conn, pop3code, pop3c->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      जर(!result && (pop3c->state == POP3_UPGRADETLS))
        जा upgrade_tls;
      थांबा;

    स्थिती POP3_AUTH:
      result = pop3_state_auth_resp(data, pop3code, pop3c->state);
      थांबा;

#ifndef CURL_DISABLE_DIGEST_AUTH
    स्थिती POP3_APOP:
      result = pop3_state_apop_resp(data, pop3code, pop3c->state);
      थांबा;
#endif

    स्थिती POP3_USER:
      result = pop3_state_user_resp(data, pop3code, pop3c->state);
      थांबा;

    स्थिती POP3_PASS:
      result = pop3_state_pass_resp(data, pop3code, pop3c->state);
      थांबा;

    स्थिती POP3_COMMAND:
      result = pop3_state_command_resp(data, pop3code, pop3c->state);
      थांबा;

    स्थिती POP3_QUIT:
      pop3_state(data, POP3_STOP);
      थांबा;

    पूर्वनिर्धारित:
      /* internal error */
      pop3_state(data, POP3_STOP);
      थांबा;
    }
  } पर्यंत(!result && pop3c->state != POP3_STOP && Curl_pp_moredata(pp));

  परत result;
}

/* Called repeatedly until done from multi.c */
स्थिर CURLcode pop3_multi_statemach(संरचना Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  जर(!pop3c)
    परत CURLE_FAILED_INIT;
  result = Curl_pp_statemach(data, &pop3c->pp, FALSE, FALSE);
  *done = (pop3c->state == POP3_STOP);

  परत result;
}

स्थिर CURLcode pop3_block_statemach(संरचना Curl_easy *data,
                                     संरचना connectdata *conn,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  पर्यंत(pop3c->state != POP3_STOP && !result)
    result = Curl_pp_statemach(data, &pop3c->pp, TRUE, disconnecting);

  परत result;
}

/* For the POP3 "protocol connect" and "doing" phases only */
स्थिर CURLcode pop3_pollset(संरचना Curl_easy *data,
                             संरचना easy_pollset *ps)
{
  संरचना pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  परत pop3c ? Curl_pp_pollset(data, &pop3c->pp, ps) : CURLE_OK;
}

/* SASL parameters for the pop3 protocol */
स्थिर स्थिरांक संरचना SASLproto saslpop3 = {
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
स्थिर CURLcode pop3_connect(संरचना Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  संरचना pingpong *pp = pop3c ? &pop3c->pp : रिक्त;

  *done = FALSE; /* default to not done yet */
  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  PINGPONG_SETUP(pp, pop3_statemachine, pop3_endofresp);

  /* Set the default preferred authentication type and mechanism */
  pop3c->preftype = POP3_TYPE_ANY;
  Curl_sasl_init(&pop3c->sasl, data, &saslpop3);

  /* Initialize the pingpong layer */
  Curl_pp_init(pp, Curl_pgrs_now(data));

  /* Parse the URL options */
  result = pop3_parse_url_options(conn);
  जर(result)
    परत result;

  /* Start off waiting for the server greeting response */
  pop3_state(data, POP3_SERVERGREET);

  result = pop3_multi_statemach(data, done);

  परत result;
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
स्थिर CURLcode pop3_done(संरचना Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  संरचना POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);

  (शून्य)premature;

  जर(!pop3)
    परत CURLE_OK;

  जर(status) {
    CURL_TRC_M(data, "POP3 done with bad status");
    connclose(data->conn);
    result = status;         /* use the already set error code */
  }

  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);

  /* Clear the transfer mode for the next request */
  pop3->transfer = PPTRANSFER_BODY;

  परत result;
}

/***********************************************************************
 *
 * pop3_perform()
 *
 * This is the actual DO function for POP3. Get a message/listing according to
 * the options previously setup.
 */
स्थिर CURLcode pop3_perform(संरचना Curl_easy *data, bool *connected,
                             bool *dophase_done)
{
  /* This is POP3 and no proxy */
  CURLcode result = CURLE_OK;
  संरचना POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);

  जर(!pop3)
    परत CURLE_FAILED_INIT;

  DEBUGF(infof(data, "DO phase starts"));

  /* Start the first command in the DO phase, may alter data->req.no_body */
  result = pop3_perform_command(data);
  जर(result)
    परत result;

  जर(data->req.no_body)
    /* Requested no body means no transfer */
    pop3->transfer = PPTRANSFER_INFO;

  *dophase_done = FALSE; /* not done yet */

  /* Run the state-machine */
  result = pop3_multi_statemach(data, dophase_done);
  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  जर(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  परत result;
}

/* Call this when the DO phase has completed */
स्थिर CURLcode pop3_dophase_done(संरचना Curl_easy *data, bool connected)
{
  (शून्य)data;
  (शून्य)connected;

  परत CURLE_OK;
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
स्थिर CURLcode pop3_regular_transfer(संरचना Curl_easy *data,
                                      bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  data->req.size = -1;

  /* Set the progress data */
  Curl_pgrsReset(data);

  /* Carry out the perform */
  result = pop3_perform(data, &connected, dophase_done);

  /* Perform post DO phase operations if necessary */
  जर(!result && *dophase_done)
    result = pop3_dophase_done(data, connected);

  परत result;
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
स्थिर CURLcode pop3_do(संरचना Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  *done = FALSE; /* default to false */

  /* Parse the URL path */
  result = pop3_parse_url_path(data);
  जर(result)
    परत result;

  /* Parse the custom request */
  result = pop3_parse_custom_request(data);
  जर(result)
    परत result;

  result = pop3_regular_transfer(data, done);

  परत result;
}

/***********************************************************************
 *
 * pop3_disconnect()
 *
 * Disconnect from an POP3 server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
स्थिर CURLcode pop3_disconnect(संरचना Curl_easy *data,
                                संरचना connectdata *conn, bool dead_connection)
{
  संरचना pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (शून्य)data;

  जर(!pop3c)
    परत CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  जर(!dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &pop3c->pp) &&
     !pop3_perform_quit(data, conn))
    (शून्य)pop3_block_statemach(data, conn, TRUE); /* ignore errors on QUIT */

  /* Disconnect from the server */
  Curl_pp_disconnect(&pop3c->pp);

  /* Cleanup our connection based variables */
  curlx_safefree(pop3c->apoptimestamp);

  परत CURLE_OK;
}

/* Called from multi.c while DOing */
स्थिर CURLcode pop3_doing(संरचना Curl_easy *data, bool *dophase_done)
{
  CURLcode result = pop3_multi_statemach(data, dophase_done);

  जर(result)
    DEBUGF(infof(data, "DO phase failed"));
  अन्यथा जर(*dophase_done) {
    result = pop3_dophase_done(data, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  परत result;
}

स्थिर शून्य pop3_easy_dtor(स्थिरांक शून्य *किल्ली, माप_प्रकार klen, शून्य *entry)
{
  संरचना POP3 *pop3 = entry;
  (शून्य)किल्ली;
  (शून्य)klen;
  DEBUGASSERT(pop3);
  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);
  curlx_free(pop3);
}

स्थिर शून्य pop3_conn_dtor(स्थिरांक शून्य *किल्ली, माप_प्रकार klen, शून्य *entry)
{
  संरचना pop3_conn *pop3c = entry;
  (शून्य)किल्ली;
  (शून्य)klen;
  DEBUGASSERT(pop3c);
  Curl_pp_disconnect(&pop3c->pp);
  curlx_safefree(pop3c->apoptimestamp);
  curlx_free(pop3c);
}

स्थिर CURLcode pop3_setup_connection(संरचना Curl_easy *data,
                                      संरचना connectdata *conn)
{
  संरचना pop3_conn *pop3c;
  संरचना POP3 *pop3 = curlx_calloc(1, आकार(*pop3));
  जर(!pop3 ||
     Curl_meta_set(data, CURL_META_POP3_EASY, pop3, pop3_easy_dtor))
    परत CURLE_OUT_OF_MEMORY;

  pop3c = curlx_calloc(1, आकार(*pop3c));
  जर(!pop3c ||
     Curl_conn_meta_set(conn, CURL_META_POP3_CONN, pop3c, pop3_conn_dtor))
    परत CURLE_OUT_OF_MEMORY;

  परत CURLE_OK;
}

/*
 * POP3 protocol.
 */
स्थिरांक संरचना Curl_protocol Curl_protocol_pop3 = {
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

#endif /* CURL_DISABLE_POP3 */
