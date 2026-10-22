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
#அந்தர்பாவ "curl_setup.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "pop3.h"

#ifndef CURL_DISABLE_POP3

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

#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "escape.h"
#அந்தர்பாவ "pingpong.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "bufref.h"
#அந்தர்பாவ "curl_sasl.h"
#அந்தர்பாவ "curl_md5.h"
#அந்தர்பாவ "curlx/strdup.h"

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
கட்டமைப்பு POP3 {
  curl_pp_transfer transfer;
  எழுத்து *id;               /* Message ID */
  எழுத்து *custom;           /* Custom Request */
};

/*
 * POP3 connection state
 */
வகைவரை எண்ணிக்கை {
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

கட்டமைப்பு pop3_conn {
  கட்டமைப்பு pingpong pp;
  pop3state state;        /* Always use pop3.c:state() to change state! */
  size_t eob;             /* Number of bytes of the EOB (End Of Body) that
                             have been received so far */
  size_t strip;           /* Number of bytes from the start to ignore as
                             non-body */
  கட்டமைப்பு SASL sasl;       /* SASL-related storage */
  எழுத்து *apoptimestamp;    /* APOP timestamp from the server greeting */
  unsigned எழுத்து authtypes; /* Accepted authentication types */
  unsigned எழுத்து preftype;  /* Preferred authentication type */
  BIT(ssldone);           /* Is connect() over SSL done? */
  BIT(tls_supported);     /* StartTLS capability supported by server */
};

கட்டமைப்பு pop3_cmd {
  மாறிலி எழுத்து *name;
  unsigned short nlen;
  BIT(multiline); /* response is multi-line with last '.' line */
  BIT(multiline_with_args); /* is multi-line when command has args */
};

நிலையான மாறிலி கட்டமைப்பு pop3_cmd pop3cmds[] = {
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
நிலையான CURLcode pop3_parse_url_options(கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  மாறிலி எழுத்து *ptr = conn->options;

  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  போது(!result && ptr && *ptr) {
    மாறிலி எழுத்து *சாவி = ptr;
    மாறிலி எழுத்து *மதிப்பு;

    போது(*ptr && *ptr != '=')
      ptr++;

    மதிப்பு = ptr + 1;

    போது(*ptr && *ptr != ';')
      ptr++;

    எனில்(curl_strnequal(சாவி, "AUTH=", 5)) {
      result = Curl_sasl_parse_url_auth_option(&pop3c->sasl,
                                               மதிப்பு, ptr - மதிப்பு);

      எனில்(result && curl_strnequal(மதிப்பு, "+APOP", ptr - மதிப்பு)) {
        pop3c->preftype = POP3_TYPE_APOP;
        pop3c->sasl.prefmech = SASL_AUTH_NONE;
        result = CURLE_OK;
      }
    }
    இல்லையெனில்
      result = CURLE_URL_MALFORMAT;

    எனில்(*ptr == ';')
      ptr++;
  }

  எனில்(pop3c->preftype != POP3_TYPE_APOP)
    தேர்வு(pop3c->sasl.prefmech) {
    நிலை SASL_AUTH_NONE:
      pop3c->preftype = POP3_TYPE_NONE;
      நிறுத்து;
    நிலை SASL_AUTH_DEFAULT:
      pop3c->preftype = POP3_TYPE_ANY;
      நிறுத்து;
    இயல்பு:
      pop3c->preftype = POP3_TYPE_SASL;
      நிறுத்து;
    }

  திரும்பு result;
}

/***********************************************************************
 *
 * pop3_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
நிலையான CURLcode pop3_parse_url_path(கட்டமைப்பு Curl_easy *data)
{
  /* The POP3 struct is already initialized in pop3_connect() */
  கட்டமைப்பு POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  மாறிலி எழுத்து *path = &data->state.up.path[1]; /* skip leading path */

  எனில்(!pop3)
    திரும்பு CURLE_FAILED_INIT;
  /* URL decode the path for the message ID */
  திரும்பு Curl_urldecode(path, 0, &pop3->id, வெற்று, REJECT_CTRL);
}

/***********************************************************************
 *
 * pop3_parse_custom_request()
 *
 * Parse the custom request.
 */
நிலையான CURLcode pop3_parse_custom_request(கட்டமைப்பு Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  மாறிலி எழுத்து *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  எனில்(!pop3)
    திரும்பு CURLE_FAILED_INIT;
  /* URL decode the custom request */
  எனில்(custom)
    result = Curl_urldecode(custom, 0, &pop3->custom, வெற்று, REJECT_CTRL);

  திரும்பு result;
}

/* Return iff a command is defined as "multi-line" (RFC 1939),
 * has a response terminated by a last line with a '.'.
 */
நிலையான bool pop3_is_multiline(மாறிலி எழுத்து *cmdline)
{
  size_t i;
  க்கு(i = 0; i < CURL_ARRAYSIZE(pop3cmds); ++i) {
    எனில்(curl_strnequal(pop3cmds[i].name, cmdline, pop3cmds[i].nlen)) {
      எனில்(!cmdline[pop3cmds[i].nlen])
        திரும்பு (bool)pop3cmds[i].multiline;
      இல்லையெனில் எனில்(cmdline[pop3cmds[i].nlen] == ' ')
        திரும்பு (bool)pop3cmds[i].multiline_with_args;
    }
  }
  /* Unknown command, assume multi-line for backward compatibility with
   * earlier curl versions that only could do multi-line responses. */
  திரும்பு TRUE;
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
நிலையான bool pop3_endofresp(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு connectdata *conn,
                           மாறிலி எழுத்து *வரி, size_t len, முழு *resp)
{
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (வெறுமை)data;
  DEBUGASSERT(pop3c);
  எனில்(!pop3c) /* internal error */
    திரும்பு TRUE;

  /* Do we have an error response? */
  எனில்(len >= 4 && !memcmp("-ERR", வரி, 4)) {
    *resp = '-';

    திரும்பு TRUE;
  }

  /* Are we processing CAPA command responses? */
  எனில்(pop3c->state == POP3_CAPA) {
    /* Do we have the terminating line? Per RFC 2449 this is a line
       containing only a single dot */
    எனில்((len == 3 && வரி[0] == '.' && வரி[1] == '\r') ||
       (len == 2 && வரி[0] == '.' && வரி[1] == '\n'))
      /* Treat the response as a success */
      *resp = '+';
    இல்லையெனில்
      /* Treat the response as an untagged continuation */
      *resp = '*';

    திரும்பு TRUE;
  }

  /* Do we have a success response? */
  எனில்(len >= 3 && !memcmp("+OK", வரி, 3)) {
    *resp = '+';

    திரும்பு TRUE;
  }

  /* Do we have a continuation response? */
  எனில்(len >= 1 && வரி[0] == '+') {
    *resp = '*';

    திரும்பு TRUE;
  }

  திரும்பு FALSE; /* Nothing for us */
}

/***********************************************************************
 *
 * pop3_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
நிலையான CURLcode pop3_get_message(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு bufref *out)
{
  கட்டமைப்பு pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  எழுத்து *message;
  size_t len;

  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;
  message = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;
  எனில்(len > 2) {
    /* Find the start of the message */
    len -= 2;
    க்கு(message += 2; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    போது(len--)
      எனில்(!ISBLANK(message[len]) && !ISNEWLINE(message[len]))
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
 * pop3_state()
 *
 * This is the ONLY way to change POP3 state!
 */
நிலையான வெறுமை pop3_state(கட்டமைப்பு Curl_easy *data, pop3state newstate)
{
  கட்டமைப்பு pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  எனில்(pop3c) {
#எனில் defined(DEBUGBUILD) && defined(CURLVERBOSE)
    /* for debug purposes */
    நிலையான மாறிலி எழுத்து * மாறிலி names[] = {
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

    எனில்(pop3c->state != newstate)
      infof(data, "POP3 %p state change from %s to %s",
            (வெறுமை *)pop3c, names[pop3c->state], names[newstate]);
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
நிலையான CURLcode pop3_perform_capa(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;

  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  pop3c->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanisms yet */
  pop3c->sasl.authused = SASL_AUTH_NONE;  /* Clear the auth. mechanism used */
  pop3c->tls_supported = FALSE;           /* Clear the TLS capability */

  /* Send the CAPA command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "CAPA");

  எனில்(!result)
    pop3_state(data, POP3_CAPA);

  திரும்பு result;
}

/***********************************************************************
 *
 * pop3_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
நிலையான CURLcode pop3_perform_starttls(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;

  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  /* Send the STLS command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "STLS");
  எனில்(!result)
    pop3_state(data, POP3_STARTTLS);

  திரும்பு result;
}

/***********************************************************************
 *
 * pop3_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
நிலையான CURLcode pop3_perform_upgrade_tls(கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு connectdata *conn)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;
  bool ssldone = FALSE;

  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  எனில்(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    எனில்(result)
      செல் out;
    /* Change the connection handler */
    conn->scheme = &Curl_scheme_pop3s;
  }

  DEBUGASSERT(!pop3c->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "pop3_perform_upgrade_tls, connect -> %d, %d",
               (முழு)result, ssldone));
  எனில்(!result && ssldone) {
    pop3c->ssldone = ssldone;
    /* perform CAPA now, changes pop3c->state out of POP3_UPGRADETLS */
    result = pop3_perform_capa(data, conn);
  }
out:
  திரும்பு result;
#இல்லையெனில்
  (வெறுமை)data;
  (வெறுமை)conn;
  திரும்பு CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * pop3_perform_user()
 *
 * Sends a clear text USER command to authenticate with.
 */
நிலையான CURLcode pop3_perform_user(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;

  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  எனில்(!conn->creds) {
    pop3_state(data, POP3_STOP);

    திரும்பு result;
  }

  /* Send the USER command */
  result = Curl_pp_sendf(data, &pop3c->pp, "USER %s",
                         Curl_creds_user(conn->creds));
  எனில்(!result)
    pop3_state(data, POP3_USER);

  திரும்பு result;
}

#ifndef CURL_DISABLE_DIGEST_AUTH
/***********************************************************************
 *
 * pop3_perform_apop()
 *
 * Sends an APOP command to authenticate with.
 */
நிலையான CURLcode pop3_perform_apop(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  size_t i;
  கட்டமைப்பு MD5_context *ctxt;
  unsigned எழுத்து digest[MD5_DIGEST_LEN];
  எழுத்து secret[(2 * MD5_DIGEST_LEN) + 1];

  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we do not */
  எனில்(!data->state.creds) {
    pop3_state(data, POP3_STOP);

    திரும்பு result;
  }

  /* Create the digest */
  ctxt = Curl_MD5_init(&Curl_DIGEST_MD5);
  எனில்(!ctxt)
    திரும்பு CURLE_OUT_OF_MEMORY;

  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)pop3c->apoptimestamp,
                  curlx_uztoui(strlen(pop3c->apoptimestamp)));

  Curl_MD5_update(ctxt, (மாறிலி unsigned எழுத்து *)Curl_creds_passwd(conn->creds),
                  curlx_uztoui(strlen(Curl_creds_passwd(conn->creds))));

  /* Finalise the digest */
  Curl_MD5_final(ctxt, digest);

  /* Convert the calculated 16 octet digest into a 32-byte hex string */
  க்கு(i = 0; i < MD5_DIGEST_LEN; i++)
    curl_msnprintf(&secret[2 * i], 3, "%02x", digest[i]);

  result = Curl_pp_sendf(data, &pop3c->pp, "APOP %s %s",
                         Curl_creds_user(conn->creds), secret);

  எனில்(!result)
    pop3_state(data, POP3_APOP);

  திரும்பு result;
}
#endif

/***********************************************************************
 *
 * pop3_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
நிலையான CURLcode pop3_perform_auth(கட்டமைப்பு Curl_easy *data,
                                  மாறிலி எழுத்து *mech,
                                  மாறிலி கட்டமைப்பு bufref *initresp)
{
  கட்டமைப்பு pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  மாறிலி எழுத்து *ir = Curl_bufref_ptr(initresp);

  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  எனில்(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    result = Curl_pp_sendf(data, &pop3c->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  இல்லையெனில் {
    /* Send the AUTH command */
    result = Curl_pp_sendf(data, &pop3c->pp, "AUTH %s", mech);
  }

  திரும்பு result;
}

/***********************************************************************
 *
 * pop3_continue_auth()
 *
 * Sends SASL continuation data.
 */
நிலையான CURLcode pop3_continue_auth(கட்டமைப்பு Curl_easy *data,
                                   மாறிலி எழுத்து *mech,
                                   மாறிலி கட்டமைப்பு bufref *resp)
{
  கட்டமைப்பு pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);

  (வெறுமை)mech;
  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  திரும்பு Curl_pp_sendf(data, &pop3c->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * pop3_cancel_auth()
 *
 * Sends SASL cancellation.
 */
நிலையான CURLcode pop3_cancel_auth(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *mech)
{
  கட்டமைப்பு pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);

  (வெறுமை)mech;
  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  திரும்பு Curl_pp_sendf(data, &pop3c->pp, "*");
}

/***********************************************************************
 *
 * pop3_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism, falling back to APOP and clear text should a
 * common mechanism not be available between the client and server.
 */
நிலையான CURLcode pop3_perform_authentication(கட்டமைப்பு Curl_easy *data,
                                            கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  saslprogress progress = SASL_IDLE;

  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  /* Check we have enough data to authenticate with and end the
     connect phase if we do not */
  எனில்(!Curl_sasl_can_authenticate(&pop3c->sasl, data)) {
    pop3_state(data, POP3_STOP);
    திரும்பு result;
  }

  எனில்(pop3c->authtypes & pop3c->preftype & POP3_TYPE_SASL) {
    /* Calculate the SASL login details */
    result = Curl_sasl_start(&pop3c->sasl, data, FALSE, &progress);

    எனில்(!result && progress == SASL_INPROGRESS)
      pop3_state(data, POP3_AUTH);
  }

  எனில்(!result && progress == SASL_IDLE) {
#ifndef CURL_DISABLE_DIGEST_AUTH
    எனில்(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
      /* Perform APOP authentication */
      result = pop3_perform_apop(data, conn);
    இல்லையெனில்
#endif
    எனில்(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
      /* Perform clear text authentication */
      result = pop3_perform_user(data, conn);
    இல்லையெனில்
      result = Curl_sasl_is_blocked(&pop3c->sasl, data);
  }

  திரும்பு result;
}

/***********************************************************************
 *
 * pop3_perform_command()
 *
 * Sends a POP3 based command.
 */
நிலையான CURLcode pop3_perform_command(கட்டமைப்பு Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  மாறிலி எழுத்து *command = வெற்று;

  எனில்(!pop3 || !pop3c)
    திரும்பு CURLE_FAILED_INIT;

  /* Calculate the default command */
  எனில்(pop3->id[0] == '\0' || data->set.list_only) {
    command = "LIST";

    எனில்(pop3->id[0] != '\0')
      /* Message specific LIST so skip the BODY transfer */
      pop3->transfer = PPTRANSFER_INFO;
  }
  இல்லையெனில்
    command = "RETR";

  எனில்(pop3->custom && pop3->custom[0] != '\0')
    command = pop3->custom;

  /* Send the command */
  எனில்(pop3->id[0] != '\0')
    result = Curl_pp_sendf(data, &pop3c->pp, "%s %s", command, pop3->id);
  இல்லையெனில்
    result = Curl_pp_sendf(data, &pop3c->pp, "%s", command);

  எனில்(!result) {
    pop3_state(data, POP3_COMMAND);
    data->req.no_body = !pop3_is_multiline(command);
  }

  திரும்பு result;
}

/***********************************************************************
 *
 * pop3_perform_quit()
 *
 * Performs the quit action prior to sclose() be called.
 */
நிலையான CURLcode pop3_perform_quit(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result;

  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  /* Send the QUIT command */
  result = Curl_pp_sendf(data, &pop3c->pp, "%s", "QUIT");
  எனில்(!result)
    pop3_state(data, POP3_QUIT);

  திரும்பு result;
}

/* For the initial server greeting */
நிலையான CURLcode pop3_state_servergreet_resp(கட்டமைப்பு Curl_easy *data,
                                            முழு pop3code,
                                            pop3state instate)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  மாறிலி எழுத்து *வரி;
  size_t len;

  (வெறுமை)instate;
  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  வரி = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  எனில்(pop3code != '+') {
    failf(data, "Got unexpected pop3-server response");
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  இல்லையெனில் எனில்(len > 3) {
    /* Does the server support APOP authentication? */
    மாறிலி எழுத்து *lt;
    மாறிலி எழுத்து *gt = வெற்று;

    /* Look for the APOP timestamp */
    lt = memchr(வரி, '<', len);
    எனில்(lt)
      /* search the remainder for '>' */
      gt = memchr(lt, '>', len - (lt - வரி));
    எனில்(gt) {
      /* the length of the timestamp, including the brackets */
      size_t timestamplen = gt - lt + 1;
      மாறிலி எழுத்து *at = memchr(lt, '@', timestamplen);
      /* If the timestamp does not contain '@' it is not (as required by
         RFC-1939) conformant to the RFC-822 message id syntax, and we
         therefore do not use APOP authentication. */
      எனில்(at) {
        /* dupe the timestamp */
        pop3c->apoptimestamp = curlx_memdup0(lt, timestamplen);
        எனில்(!pop3c->apoptimestamp)
          திரும்பு CURLE_OUT_OF_MEMORY;
        /* Store the APOP capability */
        pop3c->authtypes |= POP3_TYPE_APOP;
      }
    }

    எனில்(!result)
      result = pop3_perform_capa(data, conn);
  }

  திரும்பு result;
}

/* For CAPA responses */
நிலையான CURLcode pop3_state_capa_resp(கட்டமைப்பு Curl_easy *data, முழு pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  மாறிலி எழுத்து *வரி;
  size_t len;

  (வெறுமை)instate;
  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  வரி = curlx_dyn_ptr(&pop3c->pp.recvbuf);
  len = pop3c->pp.nfinal;

  /* Do we have an untagged continuation response? */
  எனில்(pop3code == '*') {
    /* Does the server support the STLS capability? */
    எனில்(len >= 4 && curl_strnequal(வரி, "STLS", 4))
      pop3c->tls_supported = TRUE;

    /* Does the server support clear text authentication? */
    இல்லையெனில் எனில்(len >= 4 && curl_strnequal(வரி, "USER", 4))
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    /* Does the server support SASL based authentication? */
    இல்லையெனில் எனில்(len >= 5 && curl_strnequal(வரி, "SASL ", 5)) {
      pop3c->authtypes |= POP3_TYPE_SASL;

      /* Advance past the SASL keyword */
      வரி += 5;
      len -= 5;

      /* Loop through the data line */
      க்கு(;;) {
        size_t llen;
        size_t wordlen = 0;
        unsigned short mechbit;

        போது(len && (ISBLANK(*வரி) || ISNEWLINE(*வரி))) {
          வரி++;
          len--;
        }

        எனில்(!len)
          நிறுத்து;

        /* Extract the word */
        போது(wordlen < len && !ISBLANK(வரி[wordlen]) &&
              !ISNEWLINE(வரி[wordlen]))
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(வரி, wordlen, &llen);
        எனில்(mechbit && llen == wordlen)
          pop3c->sasl.authmechs |= mechbit;

        வரி += wordlen;
        len -= wordlen;
      }
    }
  }
  இல்லையெனில் {
    /* Clear text is supported when CAPA is not recognised */
    எனில்(pop3code != '+')
      pop3c->authtypes |= POP3_TYPE_CLEARTEXT;

    எனில்(!data->set.use_ssl || Curl_conn_is_ssl(conn, FIRSTSOCKET))
      result = pop3_perform_authentication(data, conn);
    இல்லையெனில் எனில்(pop3code == '+' && pop3c->tls_supported)
      /* Switch to TLS connection now */
      result = pop3_perform_starttls(data, conn);
    இல்லையெனில் எனில்(data->set.use_ssl <= CURLUSESSL_TRY)
      /* Fallback and carry on with authentication */
      result = pop3_perform_authentication(data, conn);
    இல்லையெனில் {
      failf(data, "STLS not supported.");
      result = CURLE_USE_SSL_FAILED;
    }
  }

  திரும்பு result;
}

/* For STARTTLS responses */
நிலையான CURLcode pop3_state_starttls_resp(கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு connectdata *conn,
                                         முழு pop3code,
                                         pop3state instate)
{
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  (வெறுமை)instate;

  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  /* Pipelining in response is forbidden. */
  எனில்(pop3c->pp.overflow)
    திரும்பு CURLE_WEIRD_SERVER_REPLY;

  எனில்(pop3code != '+') {
    எனில்(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied");
      result = CURLE_USE_SSL_FAILED;
    }
    இல்லையெனில்
      result = pop3_perform_authentication(data, conn);
  }
  இல்லையெனில்
    pop3_state(data, POP3_UPGRADETLS);

  திரும்பு result;
}

/* For SASL authentication responses */
நிலையான CURLcode pop3_state_auth_resp(கட்டமைப்பு Curl_easy *data,
                                     முழு pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  saslprogress progress;

  (வெறுமை)instate;
  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  result = Curl_sasl_continue(&pop3c->sasl, data, pop3code, &progress);
  எனில்(!result)
    தேர்வு(progress) {
    நிலை SASL_DONE:
      pop3_state(data, POP3_STOP);  /* Authenticated */
      நிறுத்து;
    நிலை SASL_IDLE:            /* No mechanism left after cancellation */
#ifndef CURL_DISABLE_DIGEST_AUTH
      எனில்(pop3c->authtypes & pop3c->preftype & POP3_TYPE_APOP)
        /* Perform APOP authentication */
        result = pop3_perform_apop(data, conn);
      இல்லையெனில்
#endif
      எனில்(pop3c->authtypes & pop3c->preftype & POP3_TYPE_CLEARTEXT)
        /* Perform clear text authentication */
        result = pop3_perform_user(data, conn);
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

#ifndef CURL_DISABLE_DIGEST_AUTH
/* For APOP responses */
நிலையான CURLcode pop3_state_apop_resp(கட்டமைப்பு Curl_easy *data, முழு pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  (வெறுமை)instate;

  எனில்(pop3code != '+') {
    failf(data, "Authentication failed: %d", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  இல்லையெனில்
    /* End of connect phase */
    pop3_state(data, POP3_STOP);

  திரும்பு result;
}
#endif

/* For USER responses */
நிலையான CURLcode pop3_state_user_resp(கட்டமைப்பு Curl_easy *data, முழு pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (வெறுமை)instate;

  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  எனில்(pop3code != '+') {
    failf(data, "Access denied. %c", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  இல்லையெனில்
    /* Send the PASS command */
    result = Curl_pp_sendf(data, &pop3c->pp, "PASS %s",
                           Curl_creds_passwd(conn->creds));
  எனில்(!result)
    pop3_state(data, POP3_PASS);

  திரும்பு result;
}

/* For PASS responses */
நிலையான CURLcode pop3_state_pass_resp(கட்டமைப்பு Curl_easy *data, முழு pop3code,
                                     pop3state instate)
{
  CURLcode result = CURLE_OK;
  (வெறுமை)instate;

  எனில்(pop3code != '+') {
    failf(data, "Access denied. %c", pop3code);
    result = CURLE_LOGIN_DENIED;
  }
  இல்லையெனில்
    /* End of connect phase */
    pop3_state(data, POP3_STOP);

  திரும்பு result;
}

/***********************************************************************
 *
 * pop3_write()
 *
 * This function scans the body after the end-of-body and writes everything
 * until the end is found.
 */
நிலையான CURLcode pop3_write(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *str,
                           size_t nread, bool is_eos)
{
  /* This code could be made into a special function in the handler struct */
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  bool strip_dot = FALSE;
  size_t last = 0;
  size_t i;
  (வெறுமை)is_eos;

  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  /* Search through the buffer looking for the end-of-body marker which is
     5 bytes (0d 0a 2e 0d 0a). Note that a line starting with a dot matches
     the eob so the server will have prefixed it with an extra dot which we
     need to strip out. Additionally the marker could of course be spread out
     over 5 different data chunks. */
  க்கு(i = 0; i < nread; i++) {
    size_t prev = pop3c->eob;

    தேர்வு(str[i]) {
    நிலை 0x0d:
      எனில்(pop3c->eob == 0) {
        pop3c->eob++;

        எனில்(i) {
          /* Write out the body part that did not match */
          result = Curl_client_write(data, CLIENTWRITE_BODY, &str[last],
                                     i - last);

          எனில்(result)
            திரும்பு result;

          last = i;
        }
      }
      இல்லையெனில் எனில்(pop3c->eob == 3)
        pop3c->eob++;
      இல்லையெனில்
        /* If the character match was not at position 0 or 3 then restart the
           pattern matching */
        pop3c->eob = 1;
      நிறுத்து;

    நிலை 0x0a:
      எனில்(pop3c->eob == 1 || pop3c->eob == 4)
        pop3c->eob++;
      இல்லையெனில்
        /* If the character match was not at position 1 or 4 then start the
           search again */
        pop3c->eob = 0;
      நிறுத்து;

    நிலை 0x2e:
      எனில்(pop3c->eob == 2)
        pop3c->eob++;
      இல்லையெனில் எனில்(pop3c->eob == 3) {
        /* We have an extra dot after the CRLF which we need to strip off */
        strip_dot = TRUE;
        pop3c->eob = 0;
      }
      இல்லையெனில்
        /* If the character match was not at position 2 then start the search
           again */
        pop3c->eob = 0;
      நிறுத்து;

    இயல்பு:
      pop3c->eob = 0;
      நிறுத்து;
    }

    /* Did we have a partial match which has subsequently failed? */
    எனில்(prev && prev >= pop3c->eob) {
      /* Strip can only be non-zero for the first mismatch after CRLF and
         then both prev and strip are equal and nothing will be output below */
      போது(prev && pop3c->strip) {
        prev--;
        pop3c->strip--;
      }

      எனில்(prev) {
        /* If the partial match was the CRLF and dot then only write the CRLF
           as the server would have inserted the dot */
        எனில்(strip_dot && prev - 1 > 0) {
          result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB,
                                     prev - 1);
        }
        இல்லையெனில் எனில்(!strip_dot) {
          result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB,
                                     prev);
        }
        இல்லையெனில் {
          result = CURLE_OK;
        }

        எனில்(result)
          திரும்பு result;

        last = i;
        strip_dot = FALSE;
      }
    }
  }

  எனில்(pop3c->eob == POP3_EOB_LEN) {
    /* We have a full match so the transfer is done, however we must transfer
    the CRLF at the start of the EOB as this is considered to be part of the
    message as per RFC-1939, sect. 3 */
    result = Curl_client_write(data, CLIENTWRITE_BODY, POP3_EOB, 2);

    CURL_REQ_CLEAR_RECV(data);
    pop3c->eob = 0;

    திரும்பு result;
  }

  எனில்(pop3c->eob)
    /* While EOB is matching nothing should be output */
    திரும்பு CURLE_OK;

  எனில்(nread - last) {
    result = Curl_client_write(data, CLIENTWRITE_BODY, &str[last],
                               nread - last);
  }

  திரும்பு result;
}

/* For command responses */
நிலையான CURLcode pop3_state_command_resp(கட்டமைப்பு Curl_easy *data,
                                        முழு pop3code,
                                        pop3state instate)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  கட்டமைப்பு pingpong *pp;

  (வெறுமை)instate;
  எனில்(!pop3 || !pop3c)
    திரும்பு CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  எனில்(pop3code != '+') {
    pop3_state(data, POP3_STOP);
    திரும்பு CURLE_WEIRD_SERVER_REPLY;
  }

  /* This 'OK' line ends with a CR LF pair which is the two first bytes of the
     EOB string so count this is two matching bytes. This is necessary to make
     the code detect the EOB if the only data than comes now is %2e CR LF like
     when there is no body to return. */
  pop3c->eob = 2;

  /* Since this initial CR LF pair is not part of the actual body, we set
     the strip counter here so that these bytes will not be delivered. */
  pop3c->strip = 2;

  எனில்(pop3->transfer == PPTRANSFER_BODY) {
    /* POP3 download */
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);

    எனில்(pp->overflow) {
      /* The recv buffer contains data that is actually body content so send
         it as such. Note that there may even be additional "headers" after
         the body */

      /* keep only the overflow */
      curlx_dyn_tail(&pp->recvbuf, pp->overflow);
      pp->nfinal = 0; /* done */

      எனில்(!data->req.no_body) {
        result = pop3_write(data, curlx_dyn_ptr(&pp->recvbuf),
                            curlx_dyn_len(&pp->recvbuf), FALSE);
        எனில்(result)
          திரும்பு result;
      }

      /* reset the buffer */
      curlx_dyn_reset(&pp->recvbuf);
      pp->overflow = 0;
    }
  }
  இல்லையெனில்
    pp->overflow = 0;

  /* End of DO phase */
  pop3_state(data, POP3_STOP);

  திரும்பு result;
}

நிலையான CURLcode pop3_statemachine(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  CURLcode result = CURLE_OK;
  முழு pop3code;
  கட்டமைப்பு pingpong *pp;
  size_t nread = 0;
  (வெறுமை)data;

  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  pp = &pop3c->pp;
  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not POP3 */
upgrade_tls:
  எனில்(pop3c->state == POP3_UPGRADETLS) {
    result = pop3_perform_upgrade_tls(data, conn);
    எனில்(result || (pop3c->state == POP3_UPGRADETLS))
      திரும்பு result;
  }

  /* Flush any data that needs to be sent */
  எனில்(pp->sendleft)
    திரும்பு Curl_pp_flushsend(data, pp);

  செய் {
     /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, pp, &pop3code, &nread);
    எனில்(result)
      திரும்பு result;

    எனில்(!pop3code)
      நிறுத்து;

    /* We have now received a full POP3 server response */
    தேர்வு(pop3c->state) {
    நிலை POP3_SERVERGREET:
      result = pop3_state_servergreet_resp(data, pop3code, pop3c->state);
      நிறுத்து;

    நிலை POP3_CAPA:
      result = pop3_state_capa_resp(data, pop3code, pop3c->state);
      நிறுத்து;

    நிலை POP3_STARTTLS:
      result = pop3_state_starttls_resp(data, conn, pop3code, pop3c->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      எனில்(!result && (pop3c->state == POP3_UPGRADETLS))
        செல் upgrade_tls;
      நிறுத்து;

    நிலை POP3_AUTH:
      result = pop3_state_auth_resp(data, pop3code, pop3c->state);
      நிறுத்து;

#ifndef CURL_DISABLE_DIGEST_AUTH
    நிலை POP3_APOP:
      result = pop3_state_apop_resp(data, pop3code, pop3c->state);
      நிறுத்து;
#endif

    நிலை POP3_USER:
      result = pop3_state_user_resp(data, pop3code, pop3c->state);
      நிறுத்து;

    நிலை POP3_PASS:
      result = pop3_state_pass_resp(data, pop3code, pop3c->state);
      நிறுத்து;

    நிலை POP3_COMMAND:
      result = pop3_state_command_resp(data, pop3code, pop3c->state);
      நிறுத்து;

    நிலை POP3_QUIT:
      pop3_state(data, POP3_STOP);
      நிறுத்து;

    இயல்பு:
      /* internal error */
      pop3_state(data, POP3_STOP);
      நிறுத்து;
    }
  } போது(!result && pop3c->state != POP3_STOP && Curl_pp_moredata(pp));

  திரும்பு result;
}

/* Called repeatedly until done from multi.c */
நிலையான CURLcode pop3_multi_statemach(கட்டமைப்பு Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;
  result = Curl_pp_statemach(data, &pop3c->pp, FALSE, FALSE);
  *done = (pop3c->state == POP3_STOP);

  திரும்பு result;
}

நிலையான CURLcode pop3_block_statemach(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு connectdata *conn,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);

  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  போது(pop3c->state != POP3_STOP && !result)
    result = Curl_pp_statemach(data, &pop3c->pp, TRUE, disconnecting);

  திரும்பு result;
}

/* For the POP3 "protocol connect" and "doing" phases only */
நிலையான CURLcode pop3_pollset(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு easy_pollset *ps)
{
  கட்டமைப்பு pop3_conn *pop3c =
    Curl_conn_meta_get(data->conn, CURL_META_POP3_CONN);
  திரும்பு pop3c ? Curl_pp_pollset(data, &pop3c->pp, ps) : CURLE_OK;
}

/* SASL parameters for the pop3 protocol */
நிலையான மாறிலி கட்டமைப்பு SASLproto saslpop3 = {
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
நிலையான CURLcode pop3_connect(கட்டமைப்பு Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  கட்டமைப்பு pingpong *pp = pop3c ? &pop3c->pp : வெற்று;

  *done = FALSE; /* default to not done yet */
  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  PINGPONG_SETUP(pp, pop3_statemachine, pop3_endofresp);

  /* Set the default preferred authentication type and mechanism */
  pop3c->preftype = POP3_TYPE_ANY;
  Curl_sasl_init(&pop3c->sasl, data, &saslpop3);

  /* Initialize the pingpong layer */
  Curl_pp_init(pp, Curl_pgrs_now(data));

  /* Parse the URL options */
  result = pop3_parse_url_options(conn);
  எனில்(result)
    திரும்பு result;

  /* Start off waiting for the server greeting response */
  pop3_state(data, POP3_SERVERGREET);

  result = pop3_multi_statemach(data, done);

  திரும்பு result;
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
நிலையான CURLcode pop3_done(கட்டமைப்பு Curl_easy *data, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);

  (வெறுமை)premature;

  எனில்(!pop3)
    திரும்பு CURLE_OK;

  எனில்(status) {
    CURL_TRC_M(data, "POP3 done with bad status");
    connclose(data->conn);
    result = status;         /* use the already set error code */
  }

  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);

  /* Clear the transfer mode for the next request */
  pop3->transfer = PPTRANSFER_BODY;

  திரும்பு result;
}

/***********************************************************************
 *
 * pop3_perform()
 *
 * This is the actual DO function for POP3. Get a message/listing according to
 * the options previously setup.
 */
நிலையான CURLcode pop3_perform(கட்டமைப்பு Curl_easy *data, bool *connected,
                             bool *dophase_done)
{
  /* This is POP3 and no proxy */
  CURLcode result = CURLE_OK;
  கட்டமைப்பு POP3 *pop3 = Curl_meta_get(data, CURL_META_POP3_EASY);

  எனில்(!pop3)
    திரும்பு CURLE_FAILED_INIT;

  DEBUGF(infof(data, "DO phase starts"));

  /* Start the first command in the DO phase, may alter data->req.no_body */
  result = pop3_perform_command(data);
  எனில்(result)
    திரும்பு result;

  எனில்(data->req.no_body)
    /* Requested no body means no transfer */
    pop3->transfer = PPTRANSFER_INFO;

  *dophase_done = FALSE; /* not done yet */

  /* Run the state-machine */
  result = pop3_multi_statemach(data, dophase_done);
  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  எனில்(*dophase_done)
    DEBUGF(infof(data, "DO phase is complete"));

  திரும்பு result;
}

/* Call this when the DO phase has completed */
நிலையான CURLcode pop3_dophase_done(கட்டமைப்பு Curl_easy *data, bool connected)
{
  (வெறுமை)data;
  (வெறுமை)connected;

  திரும்பு CURLE_OK;
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
நிலையான CURLcode pop3_regular_transfer(கட்டமைப்பு Curl_easy *data,
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
  எனில்(!result && *dophase_done)
    result = pop3_dophase_done(data, connected);

  திரும்பு result;
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
நிலையான CURLcode pop3_do(கட்டமைப்பு Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  *done = FALSE; /* default to false */

  /* Parse the URL path */
  result = pop3_parse_url_path(data);
  எனில்(result)
    திரும்பு result;

  /* Parse the custom request */
  result = pop3_parse_custom_request(data);
  எனில்(result)
    திரும்பு result;

  result = pop3_regular_transfer(data, done);

  திரும்பு result;
}

/***********************************************************************
 *
 * pop3_disconnect()
 *
 * Disconnect from an POP3 server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
நிலையான CURLcode pop3_disconnect(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு connectdata *conn, bool dead_connection)
{
  கட்டமைப்பு pop3_conn *pop3c = Curl_conn_meta_get(conn, CURL_META_POP3_CONN);
  (வெறுமை)data;

  எனில்(!pop3c)
    திரும்பு CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  எனில்(!dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &pop3c->pp) &&
     !pop3_perform_quit(data, conn))
    (வெறுமை)pop3_block_statemach(data, conn, TRUE); /* ignore errors on QUIT */

  /* Disconnect from the server */
  Curl_pp_disconnect(&pop3c->pp);

  /* Cleanup our connection based variables */
  curlx_safefree(pop3c->apoptimestamp);

  திரும்பு CURLE_OK;
}

/* Called from multi.c while DOing */
நிலையான CURLcode pop3_doing(கட்டமைப்பு Curl_easy *data, bool *dophase_done)
{
  CURLcode result = pop3_multi_statemach(data, dophase_done);

  எனில்(result)
    DEBUGF(infof(data, "DO phase failed"));
  இல்லையெனில் எனில்(*dophase_done) {
    result = pop3_dophase_done(data, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  திரும்பு result;
}

நிலையான வெறுமை pop3_easy_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு POP3 *pop3 = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  DEBUGASSERT(pop3);
  /* Cleanup our per-request based variables */
  curlx_safefree(pop3->id);
  curlx_safefree(pop3->custom);
  curlx_free(pop3);
}

நிலையான வெறுமை pop3_conn_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு pop3_conn *pop3c = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  DEBUGASSERT(pop3c);
  Curl_pp_disconnect(&pop3c->pp);
  curlx_safefree(pop3c->apoptimestamp);
  curlx_free(pop3c);
}

நிலையான CURLcode pop3_setup_connection(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு pop3_conn *pop3c;
  கட்டமைப்பு POP3 *pop3 = curlx_calloc(1, அளவு(*pop3));
  எனில்(!pop3 ||
     Curl_meta_set(data, CURL_META_POP3_EASY, pop3, pop3_easy_dtor))
    திரும்பு CURLE_OUT_OF_MEMORY;

  pop3c = curlx_calloc(1, அளவு(*pop3c));
  எனில்(!pop3c ||
     Curl_conn_meta_set(conn, CURL_META_POP3_CONN, pop3c, pop3_conn_dtor))
    திரும்பு CURLE_OUT_OF_MEMORY;

  திரும்பு CURLE_OK;
}

/*
 * POP3 protocol.
 */
மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_pop3 = {
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
