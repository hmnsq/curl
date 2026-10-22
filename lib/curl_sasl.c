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
 * RFC2617 Basic and Digest Access Authentication
 * RFC2831 DIGEST-MD5 authentication
 * RFC4422 Simple Authentication and Security Layer (SASL)
 * RFC4616 PLAIN authentication
 * RFC5802 SCRAM-SHA-1 authentication
 * RFC7677 SCRAM-SHA-256 authentication
 * RFC6749 OAuth 2.0 Authorization Framework
 * RFC7628 A Set of SASL Mechanisms for OAuth
 * Draft   LOGIN SASL Mechanism <draft-murchison-sasl-login-00.txt>
 *
 ***************************************************************************/
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curlx/base64.h"
#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "curl_sasl.h"
#అన్తర్భావ "curl_trc.h"

/* Supported mechanisms */
స్థిర స్థిరరాశి నిర్మాణం {
  స్థిరరాశి అక్షరం    *name;  /* Name */
  size_t         len;   /* Name length */
  గుర్తులేని పొట్టి బిట్టు_2;   /* Flag bit */
} mechtable[] = {
  { "LOGIN",        5,  SASL_MECH_LOGIN },
  { "PLAIN",        5,  SASL_MECH_PLAIN },
  { "CRAM-MD5",     8,  SASL_MECH_CRAM_MD5 },
  { "DIGEST-MD5",   10, SASL_MECH_DIGEST_MD5 },
  { "GSSAPI",       6,  SASL_MECH_GSSAPI },
  { "EXTERNAL",     8,  SASL_MECH_EXTERNAL },
  { "NTLM",         4,  SASL_MECH_NTLM },
  { "XOAUTH2",      7,  SASL_MECH_XOAUTH2 },
  { "OAUTHBEARER",  11, SASL_MECH_OAUTHBEARER },
  { "SCRAM-SHA-1",  11, SASL_MECH_SCRAM_SHA_1 },
  { "SCRAM-SHA-256",13, SASL_MECH_SCRAM_SHA_256 },
  { ZERO_NULL,      0,  0 }
};

/*
 * Curl_sasl_decode_mech()
 *
 * Convert a SASL mechanism name into a token.
 *
 * Parameters:
 *
 * ptr    [in]     - The mechanism string.
 * maxlen [in]     - Maximum mechanism string length.
 * len    [out]    - If not NULL, effective name length.
 *
 * Returns the SASL mechanism token or 0 if no match.
 */
గుర్తులేని పొట్టి Curl_sasl_decode_mech(స్థిరరాశి అక్షరం *ptr, size_t maxlen,
                                     size_t *len)
{
  గుర్తులేని పూర్ణాంకం i;
  అక్షరం c;

  కొరకు(i = 0; mechtable[i].name; i++) {
    ఒకవేళ(maxlen >= mechtable[i].len &&
       curl_strnequal(ptr, mechtable[i].name, mechtable[i].len)) {
      ఒకవేళ(len)
        *len = mechtable[i].len;

      ఒకవేళ(maxlen == mechtable[i].len)
        తిరిగి mechtable[i].బిట్టు_2;

      c = ptr[mechtable[i].len];
      ఒకవేళ(!ISUPPER(c) && !ISDIGIT(c) && c != '-' && c != '_')
        తిరిగి mechtable[i].బిట్టు_2;
    }
  }

  తిరిగి 0;
}

/*
 * Curl_sasl_parse_url_auth_option()
 *
 * Parse the URL login options.
 */
CURLcode Curl_sasl_parse_url_auth_option(నిర్మాణం SASL *sasl,
                                         స్థిరరాశి అక్షరం *విలువ, size_t len)
{
  CURLcode ఫలితం = CURLE_OK;
  size_t mechlen;

  ఒకవేళ(!len)
    తిరిగి CURLE_URL_MALFORMAT;

  ఒకవేళ(sasl->resetprefs) {
    sasl->resetprefs = FALSE;
    sasl->prefmech = SASL_AUTH_NONE;
  }

  ఒకవేళ(!అక్షరశ్రేణి_పోలిక(విలువ, "*", len))
    sasl->prefmech = SASL_AUTH_DEFAULT;
  లేకపోతే {
    గుర్తులేని పొట్టి mechbit = Curl_sasl_decode_mech(విలువ, len, &mechlen);
    ఒకవేళ(mechbit && mechlen == len)
      sasl->prefmech |= mechbit;
    లేకపోతే
      ఫలితం = CURLE_URL_MALFORMAT;
  }

  తిరిగి ఫలితం;
}

/*
 * Curl_sasl_init()
 *
 * Initializes the SASL structure.
 */
శూన్య Curl_sasl_init(నిర్మాణం SASL *sasl, నిర్మాణం Curl_easy *దత్తాంశం,
                    స్థిరరాశి నిర్మాణం SASLproto *params)
{
  uint32_t auth = దత్తాంశం->సమితి.httpauth;

  sasl->params = params;           /* Set protocol dependent parameters */
  sasl->state = SASL_STOP;         /* Not yet running */
  sasl->curmech = ఖాళీ;            /* No mechanism yet. */
  sasl->authmechs = SASL_AUTH_NONE; /* No known authentication mechanism yet */
  sasl->prefmech = params->defmechs; /* Default preferred mechanisms */
  sasl->authused = SASL_AUTH_NONE; /* The authentication mechanism used */
  sasl->resetprefs = TRUE;         /* Reset prefmech upon AUTH parsing. */
  sasl->mutual_auth = FALSE;       /* No mutual authentication (GSSAPI only) */
  sasl->force_ir = FALSE;          /* Respect external option */

  ఒకవేళ(auth != CURLAUTH_BASIC) {
    గుర్తులేని పొట్టి mechs = SASL_AUTH_NONE;

    /* If some usable http authentication options have been set, determine
       new defaults from them. */
    ఒకవేళ(auth & CURLAUTH_BASIC)
      mechs |= SASL_MECH_PLAIN | SASL_MECH_LOGIN;
    ఒకవేళ(auth & CURLAUTH_DIGEST)
      mechs |= SASL_MECH_DIGEST_MD5;
    ఒకవేళ(auth & CURLAUTH_NTLM)
      mechs |= SASL_MECH_NTLM;
    ఒకవేళ(auth & CURLAUTH_BEARER)
      mechs |= SASL_MECH_OAUTHBEARER | SASL_MECH_XOAUTH2;
    ఒకవేళ(auth & CURLAUTH_GSSAPI)
      mechs |= SASL_MECH_GSSAPI;

    ఒకవేళ(mechs != SASL_AUTH_NONE)
      sasl->prefmech = mechs;
  }
}

/*
 * sasl_state()
 *
 * This is the ONLY way to change SASL state!
 */
స్థిర శూన్య sasl_state(నిర్మాణం SASL *sasl, నిర్మాణం Curl_easy *దత్తాంశం,
                       saslstate newstate)
{
#ఒకవేళ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి పేర్లు[] = {
    "STOP",
    "PLAIN",
    "LOGIN",
    "LOGIN_PASSWD",
    "EXTERNAL",
    "CRAMMD5",
    "DIGESTMD5",
    "DIGESTMD5_RESP",
    "NTLM",
    "NTLM_TYPE2MSG",
    "GSSAPI",
    "GSSAPI_TOKEN",
    "GSSAPI_NO_DATA",
    "OAUTH2",
    "OAUTH2_RESP",
    "GSASL",
    "CANCEL",
    "FINAL",
    /*చివరి*/
  };

  ఒకవేళ(sasl->state != newstate)
    infof(దత్తాంశం, "SASL %p state change from %s to %s",
          (శూన్య *)sasl, పేర్లు[sasl->state], పేర్లు[newstate]);
#లేకపోతే
  (శూన్య)దత్తాంశం;
#అయితే_ముగింపు

  sasl->state = newstate;
}

#ఒకవేళ defined(USE_NTLM) || defined(USE_GSASL) || defined(USE_KERBEROS5) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)
/* Get the SASL server message and convert it to binary. */
స్థిర CURLcode get_server_సందేశం(నిర్మాణం SASL *sasl, నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం bufref *బహిర్గతం)
{
  CURLcode ఫలితం = CURLE_OK;

  ఫలితం = sasl->params->getmessage(దత్తాంశం, బహిర్గతం);
  ఒకవేళ(!ఫలితం && (sasl->params->flags & SASL_FLAG_BASE64)) {
    స్థిరరాశి అక్షరం *serverdata = Curl_bufref_ptr(బహిర్గతం);

    ఒకవేళ(!*serverdata)
      Curl_bufref_సమితి(బహిర్గతం, ఖాళీ, 0, ఖాళీ);
    లేకపోతే {
      గుర్తులేని అక్షరం *సందేశం;
      size_t msglen;

      ఫలితం = curlx_base64_decode(serverdata, &సందేశం, &msglen);
      ఒకవేళ(!ఫలితం)
        Curl_bufref_సమితి(బహిర్గతం, సందేశం, msglen, curl_free);
    }
  }
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు

/* Encode the outgoing SASL message. */
స్థిర CURLcode build_సందేశం(నిర్మాణం SASL *sasl, నిర్మాణం bufref *సందేశం)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(sasl->params->flags & SASL_FLAG_BASE64) {
    ఒకవేళ(!Curl_bufref_ptr(సందేశం))                   /* Empty message. */
      Curl_bufref_సమితి(సందేశం, "", 0, ఖాళీ);
    లేకపోతే ఒకవేళ(Curl_bufref_len(సందేశం)) {
      అక్షరం *base64;
      size_t base64len;

      ఫలితం = curlx_base64_encode(Curl_bufref_uptr(సందేశం),
                                   Curl_bufref_len(సందేశం), &base64, &base64len);
      ఒకవేళ(!ఫలితం)
        Curl_bufref_సమితి(సందేశం, base64, base64len, curl_free);
    }
  }

  తిరిగి ఫలితం;
}

/*
 * Curl_sasl_can_authenticate()
 *
 * Check if we have enough auth data and capabilities to authenticate.
 */
bool Curl_sasl_can_authenticate(నిర్మాణం SASL *sasl, నిర్మాణం Curl_easy *దత్తాంశం)
{
  /* Have credentials been provided? */
  ఒకవేళ(దత్తాంశం->conn->creds)
    తిరిగి TRUE;

  /* EXTERNAL can authenticate without a username and/or password */
  ఒకవేళ(sasl->authmechs & sasl->prefmech & SASL_MECH_EXTERNAL)
    తిరిగి TRUE;

  తిరిగి FALSE;
}

నిర్మాణం sasl_ctx {
  నిర్మాణం SASL *sasl;
  నిర్మాణం connectdata *conn;
  గుర్తులేని పొట్టి enabledmechs;
  స్థిరరాశి అక్షరం *mech;
  saslstate state1;
  saslstate state2;
  నిర్మాణం bufref resp;
  CURLcode ఫలితం;
};

స్థిర bool sasl_ఎంచుకో_external(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం sasl_ctx *sctx)
{
  ఒకవేళ((sctx->enabledmechs & SASL_MECH_EXTERNAL) &&
     !Curl_creds_has_passwd(sctx->conn->creds)) {
    sctx->mech = SASL_MECH_STRING_EXTERNAL;
    sctx->state1 = SASL_EXTERNAL;
    sctx->sasl->authused = SASL_MECH_EXTERNAL;

    ఒకవేళ(sctx->sasl->force_ir || దత్తాంశం->సమితి.sasl_ir)
      Curl_auth_create_external_సందేశం(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    తిరిగి TRUE;
  }
  తిరిగి FALSE;
}

#నిర్వచితమైతే USE_KERBEROS5
స్థిర bool sasl_ఎంచుకో_krb5(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం sasl_ctx *sctx)
{
  ఒకవేళ((sctx->enabledmechs & SASL_MECH_GSSAPI) &&
     Curl_auth_is_gssapi_supported() &&
     Curl_auth_user_కలిగియుంది_domain(sctx->conn->creds)) {
    స్థిరరాశి అక్షరం *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;

    sctx->sasl->mutual_auth = FALSE;
    sctx->mech = SASL_MECH_STRING_GSSAPI;
    sctx->state1 = SASL_GSSAPI;
    sctx->state2 = SASL_GSSAPI_TOKEN;
    sctx->sasl->authused = SASL_MECH_GSSAPI;

    ఒకవేళ(sctx->sasl->force_ir || దత్తాంశం->సమితి.sasl_ir) {
      నిర్మాణం kerberos5data *krb5 = Curl_auth_krb5_get(sctx->conn);
      sctx->ఫలితం = !krb5 ? CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం :
        Curl_auth_create_gssapi_user_సందేశం(దత్తాంశం, sctx->conn->creds,
                                             service,
                                             sctx->conn->origin->hostname,
                                             (bool)sctx->sasl->mutual_auth,
                                             ఖాళీ, krb5, &sctx->resp);
    }
    తిరిగి TRUE;
  }
  తిరిగి FALSE;
}
#అయితే_ముగింపు /* USE_KERBEROS5 */

#నిర్వచితమైతే USE_GSASL
స్థిర bool sasl_ఎంచుకో_gsasl(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం sasl_ctx *sctx)
{
  నిర్మాణం gsasldata *gsasl;
  నిర్మాణం bufref nullmsg;

  ఒకవేళ((sctx->enabledmechs &
      (SASL_MECH_SCRAM_SHA_256 | SASL_MECH_SCRAM_SHA_1))) {
    gsasl = Curl_auth_gsasl_get(sctx->conn);
    ఒకవేళ(!gsasl) {
      sctx->ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      తిరిగి TRUE; /* attempted, but failed */
    }

    ఒకవేళ((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_256) &&
       Curl_auth_gsasl_is_supported(దత్తాంశం, SASL_MECH_STRING_SCRAM_SHA_256,
                                    gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_256;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_256;
    }
    లేకపోతే ఒకవేళ((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_1) &&
            Curl_auth_gsasl_is_supported(దత్తాంశం, SASL_MECH_STRING_SCRAM_SHA_1,
                                         gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_1;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_1;
    }
    లేకపోతే
      తిరిగి FALSE;

    Curl_bufref_init(&nullmsg);
    sctx->state1 = SASL_GSASL;
    sctx->state2 = SASL_GSASL;
    sctx->ఫలితం = Curl_auth_gsasl_ప్రారంభం(దత్తాంశం, sctx->conn->creds, gsasl);
    ఒకవేళ(!sctx->ఫలితం && (sctx->sasl->force_ir || దత్తాంశం->సమితి.sasl_ir))
      sctx->ఫలితం = Curl_auth_gsasl_token(దత్తాంశం, &nullmsg, gsasl, &sctx->resp);
    తిరిగి TRUE;
  }
  తిరిగి FALSE;
}

#అయితే_ముగింపు /* USE_GSASL */

#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH
స్థిర bool sasl_ఎంచుకో_digest(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం sasl_ctx *sctx)
{
  (శూన్య)దత్తాంశం;
  ఒకవేళ((sctx->enabledmechs & SASL_MECH_DIGEST_MD5) &&
     Curl_auth_is_digest_supported()) {
    sctx->mech = SASL_MECH_STRING_DIGEST_MD5;
    sctx->state1 = SASL_DIGESTMD5;
    sctx->sasl->authused = SASL_MECH_DIGEST_MD5;
    తిరిగి TRUE;
  }
  లేకపోతే ఒకవేళ(sctx->enabledmechs & SASL_MECH_CRAM_MD5) {
    sctx->mech = SASL_MECH_STRING_CRAM_MD5;
    sctx->state1 = SASL_CRAMMD5;
    sctx->sasl->authused = SASL_MECH_CRAM_MD5;
    తిరిగి TRUE;
  }
  తిరిగి FALSE;
}
#అయితే_ముగింపు /* !CURL_DISABLE_DIGEST_AUTH */

#నిర్వచితమైతే USE_NTLM
స్థిర bool sasl_ఎంచుకో_ntlm(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం sasl_ctx *sctx)
{
  ఒకవేళ((sctx->enabledmechs & SASL_MECH_NTLM) &&
     Curl_auth_is_ntlm_supported()) {
     స్థిరరాశి అక్షరం *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;
    స్థిరరాశి అక్షరం *hostname;

    Curl_conn_get_current_host(దత్తాంశం, FIRSTSOCKET, &hostname, ఖాళీ);

    sctx->mech = SASL_MECH_STRING_NTLM;
    sctx->state1 = SASL_NTLM;
    sctx->state2 = SASL_NTLM_TYPE2_సందేశం;
    sctx->sasl->authused = SASL_MECH_NTLM;

    ఒకవేళ(sctx->sasl->force_ir || దత్తాంశం->సమితి.sasl_ir) {
      నిర్మాణం ntlmdata *ntlm = Curl_auth_ntlm_get(sctx->conn, FALSE);
      sctx->ఫలితం = !ntlm ? CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం :
        Curl_auth_create_ntlm_type1_సందేశం(దత్తాంశం, sctx->conn->creds,
                                            service, hostname,
                                            ntlm, &sctx->resp);
    }
    తిరిగి TRUE;
  }
  తిరిగి FALSE;
}
#అయితే_ముగింపు /* USE_NTLM */

స్థిర bool sasl_ఎంచుకో_oauth(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం sasl_ctx *sctx)
{
  ఒకవేళ(Curl_creds_has_oauth_bearer(దత్తాంశం->state.creds) &&
     (sctx->enabledmechs & SASL_MECH_OAUTHBEARER)) {
    స్థిరరాశి అక్షరం *hostname;
    పూర్ణాంకం పోర్టు;
    Curl_conn_get_current_host(దత్తాంశం, FIRSTSOCKET, &hostname, &పోర్టు);

    sctx->mech = SASL_MECH_STRING_OAUTHBEARER;
    sctx->state1 = SASL_OAUTH2;
    sctx->state2 = SASL_OAUTH2_RESP;
    sctx->sasl->authused = SASL_MECH_OAUTHBEARER;

    ఒకవేళ(sctx->sasl->force_ir || దత్తాంశం->సమితి.sasl_ir)
      sctx->ఫలితం =
        Curl_auth_create_oauth_bearer_సందేశం(sctx->conn->creds,
                                              hostname, పోర్టు, &sctx->resp);
    తిరిగి TRUE;
  }
  తిరిగి FALSE;
}

స్థిర bool sasl_ఎంచుకో_oauth2(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం sasl_ctx *sctx)
{
  ఒకవేళ(Curl_creds_has_oauth_bearer(sctx->conn->creds) &&
     (sctx->enabledmechs & SASL_MECH_XOAUTH2)) {
    sctx->mech = SASL_MECH_STRING_XOAUTH2;
    sctx->state1 = SASL_OAUTH2;
    sctx->sasl->authused = SASL_MECH_XOAUTH2;

    ఒకవేళ(sctx->sasl->force_ir || దత్తాంశం->సమితి.sasl_ir)
      sctx->ఫలితం = Curl_auth_create_xoauth_bearer_సందేశం(
        sctx->conn->creds, &sctx->resp);
    తిరిగి TRUE;
  }
  తిరిగి FALSE;
}

స్థిర bool sasl_ఎంచుకో_plain(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం sasl_ctx *sctx)
{
  ఒకవేళ(sctx->enabledmechs & SASL_MECH_PLAIN) {
    sctx->mech = SASL_MECH_STRING_PLAIN;
    sctx->state1 = SASL_PLAIN;
    sctx->sasl->authused = SASL_MECH_PLAIN;

    ఒకవేళ(sctx->sasl->force_ir || దత్తాంశం->సమితి.sasl_ir)
      sctx->ఫలితం =
        Curl_auth_create_plain_సందేశం(sctx->conn->creds, &sctx->resp);
    తిరిగి TRUE;
  }
  తిరిగి FALSE;
}

స్థిర bool sasl_ఎంచుకో_login(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం sasl_ctx *sctx)
{
  ఒకవేళ(sctx->enabledmechs & SASL_MECH_LOGIN) {
    sctx->mech = SASL_MECH_STRING_LOGIN;
    sctx->state1 = SASL_LOGIN;
    sctx->state2 = SASL_LOGIN_PASSWD;
    sctx->sasl->authused = SASL_MECH_LOGIN;

    ఒకవేళ(sctx->sasl->force_ir || దత్తాంశం->సమితి.sasl_ir)
      Curl_auth_create_login_సందేశం(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    తిరిగి TRUE;
  }
  తిరిగి FALSE;
}

/*
 * Curl_sasl_start()
 *
 * Calculate the required login details for SASL authentication.
 */
CURLcode Curl_sasl_ప్రారంభం(నిర్మాణం SASL *sasl, నిర్మాణం Curl_easy *దత్తాంశం,
                         bool force_ir, saslprogress *పురోగతి)
{
  నిర్మాణం sasl_ctx sctx;

  sasl->force_ir = force_ir;    /* Latch for future use */
  sasl->authused = 0;           /* No mechanism used yet */
  *పురోగతి = SASL_IDLE;

  memset(&sctx, 0, పరిమాణం(sctx));
  sctx.sasl = sasl;
  sctx.conn = దత్తాంశం->conn;
  Curl_bufref_init(&sctx.resp);
  sctx.enabledmechs = sasl->authmechs & sasl->prefmech;
  sctx.state1 = SASL_STOP;
  sctx.state2 = SASL_FINAL;

  /* Calculate the supported authentication mechanism, by decreasing order of
     security, as well as the initial response where appropriate */
  ఒకవేళ(sasl_ఎంచుకో_external(దత్తాంశం, &sctx) ||
#నిర్వచితమైతే USE_KERBEROS5
     sasl_ఎంచుకో_krb5(దత్తాంశం, &sctx) ||
#అయితే_ముగింపు
#నిర్వచితమైతే USE_GSASL
     sasl_ఎంచుకో_gsasl(దత్తాంశం, &sctx) ||
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH
     sasl_ఎంచుకో_digest(దత్తాంశం, &sctx) ||
#అయితే_ముగింపు
#నిర్వచితమైతే USE_NTLM
     sasl_ఎంచుకో_ntlm(దత్తాంశం, &sctx) ||
#అయితే_ముగింపు
     sasl_ఎంచుకో_oauth(దత్తాంశం, &sctx) ||
     sasl_ఎంచుకో_oauth2(దత్తాంశం, &sctx) ||
     sasl_ఎంచుకో_plain(దత్తాంశం, &sctx) ||
     sasl_ఎంచుకో_login(దత్తాంశం, &sctx)) {
    /* selected, either we have a mechanism or a failure */
    DEBUGASSERT(sctx.mech || sctx.ఫలితం);
  }

  ఒకవేళ(!sctx.ఫలితం && sctx.mech) {
    sasl->curmech = sctx.mech;
    ఒకవేళ(Curl_bufref_ptr(&sctx.resp))
      sctx.ఫలితం = build_సందేశం(sasl, &sctx.resp);

    ఒకవేళ(sasl->params->maxirlen &&
       అక్షరశ్రేణి_పొడవు(sctx.mech) + Curl_bufref_len(&sctx.resp) >
         sasl->params->maxirlen)
      Curl_bufref_free(&sctx.resp);

    ఒకవేళ(!sctx.ఫలితం)
      sctx.ఫలితం = sasl->params->sendauth(దత్తాంశం, sctx.mech, &sctx.resp);

    ఒకవేళ(!sctx.ఫలితం) {
      *పురోగతి = SASL_INPROGRESS;
      sasl_state(sasl, దత్తాంశం, Curl_bufref_ptr(&sctx.resp) ?
                 sctx.state2 : sctx.state1);
    }
  }

  Curl_bufref_free(&sctx.resp);
  తిరిగి sctx.ఫలితం;
}

/*
 * Curl_sasl_continue()
 *
 * Continue the authentication.
 */
CURLcode Curl_sasl_continue(నిర్మాణం SASL *sasl, నిర్మాణం Curl_easy *దత్తాంశం,
                            పూర్ణాంకం సంకేతం_2, saslprogress *పురోగతి)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  saslstate newstate = SASL_FINAL;
  నిర్మాణం bufref resp;
  స్థిరరాశి అక్షరం *hostname;
  పూర్ణాంకం పోర్టు;
  నిర్మాణం bufref serverdata;

  Curl_conn_get_current_host(దత్తాంశం, FIRSTSOCKET, &hostname, &పోర్టు);
  Curl_bufref_init(&serverdata);
  Curl_bufref_init(&resp);
  *పురోగతి = SASL_INPROGRESS;

  ఒకవేళ(sasl->state == SASL_FINAL) {
    ఒకవేళ(సంకేతం_2 != sasl->params->finalcode)
      ఫలితం = CURLE_LOGIN_DENIED;
    *పురోగతి = SASL_పూర్తి;
    sasl_state(sasl, దత్తాంశం, SASL_STOP);
    తిరిగి ఫలితం;
  }

  ఒకవేళ(sasl->state != SASL_CANCEL && sasl->state != SASL_OAUTH2_RESP &&
     సంకేతం_2 != sasl->params->contcode) {
    *పురోగతి = SASL_పూర్తి;
    sasl_state(sasl, దత్తాంశం, SASL_STOP);
    తిరిగి CURLE_LOGIN_DENIED;
  }

  ఎంపిక(sasl->state) {
  స్థితి SASL_STOP:
    *పురోగతి = SASL_పూర్తి;
    తిరిగి ఫలితం;
  స్థితి SASL_PLAIN:
    ఫలితం = Curl_auth_create_plain_సందేశం(conn->creds, &resp);
    ఆపు;
  స్థితి SASL_LOGIN:
    Curl_auth_create_login_సందేశం(Curl_creds_user(conn->creds), &resp);
    newstate = SASL_LOGIN_PASSWD;
    ఆపు;
  స్థితి SASL_LOGIN_PASSWD:
    Curl_auth_create_login_సందేశం(Curl_creds_passwd(conn->creds), &resp);
    ఆపు;
  స్థితి SASL_EXTERNAL:
    Curl_auth_create_external_సందేశం(Curl_creds_user(conn->creds), &resp);
    ఆపు;
#నిర్వచితమైతే USE_GSASL
  స్థితి SASL_GSASL:
    ఫలితం = get_server_సందేశం(sasl, దత్తాంశం, &serverdata);
    ఒకవేళ(!ఫలితం) {
      నిర్మాణం gsasldata *gsasl = Curl_auth_gsasl_get(conn);
      ఫలితం = !gsasl ? CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం :
        Curl_auth_gsasl_token(దత్తాంశం, &serverdata, gsasl, &resp);
    }
    ఒకవేళ(!ఫలితం && Curl_bufref_len(&resp) > 0)
      newstate = SASL_GSASL;
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH
  స్థితి SASL_CRAMMD5:
    ఫలితం = get_server_సందేశం(sasl, దత్తాంశం, &serverdata);
    ఒకవేళ(!ఫలితం)
      ఫలితం = Curl_auth_create_cram_md5_సందేశం(&serverdata, conn->creds,
                                                 &resp);
    ఆపు;
  స్థితి SASL_DIGESTMD5:
    ఫలితం = get_server_సందేశం(sasl, దత్తాంశం, &serverdata);
    ఒకవేళ(!ఫలితం)
      ఫలితం = Curl_auth_create_digest_md5_సందేశం(దత్తాంశం, &serverdata,
                                                   conn->creds,
                                                   sasl->params->service,
                                                   &resp);
    ఒకవేళ(!ఫలితం && (sasl->params->flags & SASL_FLAG_BASE64))
      newstate = SASL_DIGESTMD5_RESP;
    ఆపు;
  స్థితి SASL_DIGESTMD5_RESP:
    /* Keep response NULL to output an empty line. */
    ఆపు;
#అయితే_ముగింపు

#నిర్వచితమైతే USE_NTLM
  స్థితి SASL_NTLM: {
    /* Create the type-1 message */
    నిర్మాణం ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    ఫలితం = !ntlm ? CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం :
      Curl_auth_create_ntlm_type1_సందేశం(దత్తాంశం, conn->creds,
                                          sasl->params->service, hostname,
                                          ntlm, &resp);
    newstate = SASL_NTLM_TYPE2_సందేశం;
    ఆపు;
  }
  స్థితి SASL_NTLM_TYPE2_సందేశం: {
    /* Decode the type-2 message */
    నిర్మాణం ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    ఫలితం = !ntlm ? CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం :
      get_server_సందేశం(sasl, దత్తాంశం, &serverdata);
    ఒకవేళ(!ఫలితం)
      ఫలితం = Curl_auth_decode_ntlm_type2_సందేశం(దత్తాంశం, &serverdata, ntlm);
    ఒకవేళ(!ఫలితం)
      ఫలితం = Curl_auth_create_ntlm_type3_సందేశం(దత్తాంశం, conn->creds,
                                                   ntlm, &resp);
    ఆపు;
  }
#అయితే_ముగింపు

#నిర్వచితమైతే USE_KERBEROS5
  స్థితి SASL_GSSAPI: {
    నిర్మాణం kerberos5data *krb5 = Curl_auth_krb5_get(conn);
    ఫలితం = !krb5 ? CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం :
      Curl_auth_create_gssapi_user_సందేశం(దత్తాంశం, conn->creds,
                                           sasl->params->service,
                                           conn->origin->hostname,
                                           (bool)sasl->mutual_auth, ఖాళీ,
                                           krb5, &resp);
    newstate = SASL_GSSAPI_TOKEN;
    ఆపు;
  }
  స్థితి SASL_GSSAPI_TOKEN:
    ఫలితం = get_server_సందేశం(sasl, దత్తాంశం, &serverdata);
    ఒకవేళ(!ఫలితం) {
      నిర్మాణం kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      ఒకవేళ(!krb5)
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      లేకపోతే ఒకవేళ(sasl->mutual_auth) {
        /* Decode the user token challenge and create the optional response
సందేశం*/
        ఫలితం = Curl_auth_create_gssapi_user_సందేశం(దత్తాంశం, ఖాళీ,
                                                      ఖాళీ, ఖాళీ,
                                                      (bool)sasl->mutual_auth,
                                                      &serverdata,
                                                      krb5, &resp);
        newstate = SASL_GSSAPI_NO_దత్తాంశం;
      }
      లేకపోతే
        /* Decode the security challenge and create the response message */
        ఫలితం = Curl_auth_create_gssapi_security_సందేశం(
          దత్తాంశం, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    ఆపు;
  స్థితి SASL_GSSAPI_NO_దత్తాంశం:
    /* Decode the security challenge and create the response message */
    ఫలితం = get_server_సందేశం(sasl, దత్తాంశం, &serverdata);
    ఒకవేళ(!ఫలితం) {
      నిర్మాణం kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      ఒకవేళ(!krb5)
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      లేకపోతే
        ఫలితం = Curl_auth_create_gssapi_security_సందేశం(
          దత్తాంశం, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    ఆపు;
#అయితే_ముగింపు

  స్థితి SASL_OAUTH2:
    /* Create the authorization message */
    ఒకవేళ(sasl->authused == SASL_MECH_OAUTHBEARER) {
      ఫలితం = Curl_auth_create_oauth_bearer_సందేశం(conn->creds,
                                                     hostname,
                                                     పోర్టు,
                                                     &resp);

      /* Failures maybe sent by the server as continuations for OAUTHBEARER */
      newstate = SASL_OAUTH2_RESP;
    }
    లేకపోతే
      ఫలితం = Curl_auth_create_xoauth_bearer_సందేశం(conn->creds,
                                                      &resp);
    ఆపు;

  స్థితి SASL_OAUTH2_RESP:
    /* The continuation is optional so check the response code */
    ఒకవేళ(సంకేతం_2 == sasl->params->finalcode) {
      /* Final response was received so we are done */
      *పురోగతి = SASL_పూర్తి;
      sasl_state(sasl, దత్తాంశం, SASL_STOP);
      తిరిగి ఫలితం;
    }
    లేకపోతే ఒకవేళ(సంకేతం_2 == sasl->params->contcode) {
      /* Acknowledge the continuation by sending a 0x01 response. */
      Curl_bufref_సమితి(&resp, "\x01", 1, ఖాళీ);
      ఆపు;
    }
    లేకపోతే {
      *పురోగతి = SASL_పూర్తి;
      sasl_state(sasl, దత్తాంశం, SASL_STOP);
      తిరిగి CURLE_LOGIN_DENIED;
    }

  స్థితి SASL_CANCEL:
    /* Remove the offending mechanism from the supported list */
    sasl->authmechs &= (గుర్తులేని పొట్టి)~sasl->authused;
    sasl->authused = SASL_AUTH_NONE;
    sasl->curmech = ఖాళీ;

    /* Start an alternative SASL authentication */
    తిరిగి Curl_sasl_ప్రారంభం(sasl, దత్తాంశం, (bool)sasl->force_ir, పురోగతి);
  అప్రమేయ:
    failf(దత్తాంశం, "Unsupported SASL authentication mechanism");
    ఫలితం = CURLE_UNSUPPORTED_PROTOCOL;  /* Should not happen */
    ఆపు;
  }

  Curl_bufref_free(&serverdata);

  ఎంపిక(ఫలితం) {
  స్థితి CURLE_BAD_CONTENT_ENCODING:
    /* Cancel dialog */
    ఫలితం = sasl->params->cancelauth(దత్తాంశం, sasl->curmech);
    newstate = SASL_CANCEL;
    ఆపు;
  స్థితి CURLE_OK:
    ఫలితం = build_సందేశం(sasl, &resp);
    ఒకవేళ(!ఫలితం)
      ఫలితం = sasl->params->contauth(దత్తాంశం, sasl->curmech, &resp);
    ఆపు;
  అప్రమేయ:
    newstate = SASL_STOP;    /* Stop on error */
    *పురోగతి = SASL_పూర్తి;
    ఆపు;
  }

  Curl_bufref_free(&resp);

  sasl_state(sasl, దత్తాంశం, newstate);

  తిరిగి ఫలితం;
}

#నిర్వచితమైతే CURLVERBOSE
స్థిర శూన్య sasl_unchosen(నిర్మాణం Curl_easy *దత్తాంశం, గుర్తులేని పొట్టి mech,
                          గుర్తులేని పొట్టి enabledmechs,
                          bool built_లోపల, bool platform,
                          స్థిరరాశి అక్షరం *param_missing)
{
  స్థిరరాశి అక్షరం *mname = ఖాళీ;
  size_t i;

  ఒకవేళ(!(enabledmechs & mech))
    తిరిగి;

  కొరకు(i = 0; mechtable[i].name; ++i) {
    ఒకవేళ(mechtable[i].బిట్టు_2 == mech) {
      mname = mechtable[i].name;
      ఆపు;
    }
  }
  ఒకవేళ(!mname)  /* should not happen */
    తిరిగి;
  ఒకవేళ(!built_లోపల)
    infof(దత్తాంశం, "SASL: %s not builtin", mname);
  లేకపోతే ఒకవేళ(!platform)
    infof(దత్తాంశం, "SASL: %s not supported by the platform/libraries", mname);
  లేకపోతే {
    ఒకవేళ(param_missing)
      infof(దత్తాంశం, "SASL: %s is missing %s", mname, param_missing);
    ఒకవేళ(!Curl_creds_has_user(దత్తాంశం->conn->creds))
      infof(దత్తాంశం, "SASL: %s is missing username", mname);
  }
}
#అయితే_ముగింపు /* CURLVERBOSE */

CURLcode Curl_sasl_is_blocked(నిర్మాణం SASL *sasl, నిర్మాణం Curl_easy *దత్తాంశం)
{
#నిర్వచితమైతే CURLVERBOSE
#నిర్వచితమైతే USE_KERBEROS5
#నిర్వచించు CURL_SASL_KERBEROS5   TRUE
#లేకపోతే
#నిర్వచించు CURL_SASL_KERBEROS5   FALSE
#అయితే_ముగింపు
#నిర్వచితమైతే USE_GSASL
#నిర్వచించు CURL_SASL_GASL        TRUE
#లేకపోతే
#నిర్వచించు CURL_SASL_GASL        FALSE
#అయితే_ముగింపు
#నిర్వచితమైతే CURL_DISABLE_DIGEST_AUTH
#నిర్వచించు CURL_SASL_DIGEST      TRUE
#లేకపోతే
#నిర్వచించు CURL_SASL_DIGEST      FALSE
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే USE_NTLM
#నిర్వచించు CURL_SASL_NTLM        TRUE
#లేకపోతే
#నిర్వచించు CURL_SASL_NTLM        FALSE
#అయితే_ముగింపు
  /* Failing SASL authentication is a pain. Give a helping hand if
   * we were unable to select an AUTH mechanism.
   * `sasl->authmechs` are mechanisms offered by the peer
   * `sasl->prefmech`  are mechanisms preferred by us */
  గుర్తులేని పొట్టి enabledmechs = sasl->authmechs & sasl->prefmech;

  ఒకవేళ(!sasl->authmechs)
    infof(దత్తాంశం, "SASL: no auth mechanism was offered or recognized");
  లేకపోతే ఒకవేళ(!enabledmechs)
    infof(దత్తాంశం, "SASL: no overlap between offered and configured "
          "auth mechanisms");
  లేకపోతే {
    infof(దత్తాంశం, "SASL: no auth mechanism offered could be selected");
    ఒకవేళ((enabledmechs & SASL_MECH_EXTERNAL) &&
       Curl_creds_has_passwd(దత్తాంశం->conn->creds))
      infof(దత్తాంశం, "SASL: auth EXTERNAL not chosen with password");
    sasl_unchosen(దత్తాంశం, SASL_MECH_GSSAPI, enabledmechs,
                  CURL_SASL_KERBEROS5, Curl_auth_is_gssapi_supported(), ఖాళీ);
    sasl_unchosen(దత్తాంశం, SASL_MECH_SCRAM_SHA_256, enabledmechs,
                  CURL_SASL_GASL, FALSE, ఖాళీ);
    sasl_unchosen(దత్తాంశం, SASL_MECH_SCRAM_SHA_1, enabledmechs,
                  CURL_SASL_GASL, FALSE, ఖాళీ);
    sasl_unchosen(దత్తాంశం, SASL_MECH_DIGEST_MD5, enabledmechs,
                  CURL_SASL_DIGEST, Curl_auth_is_digest_supported(), ఖాళీ);
    sasl_unchosen(దత్తాంశం, SASL_MECH_CRAM_MD5, enabledmechs,
                  CURL_SASL_DIGEST, TRUE, ఖాళీ);
    sasl_unchosen(దత్తాంశం, SASL_MECH_NTLM, enabledmechs,
                  CURL_SASL_NTLM, Curl_auth_is_ntlm_supported(), ఖాళీ);
    sasl_unchosen(దత్తాంశం, SASL_MECH_OAUTHBEARER, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(దత్తాంశం->conn->creds) ?
                  ఖాళీ : "CURLOPT_XOAUTH2_BEARER");
    sasl_unchosen(దత్తాంశం, SASL_MECH_XOAUTH2, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(దత్తాంశం->conn->creds) ?
                  ఖాళీ : "CURLOPT_XOAUTH2_BEARER");
  }
#అయితే_ముగింపు /* CURLVERBOSE */
  (శూన్య)sasl;
  (శూన్య)దత్తాంశం;
  తిరిగి CURLE_LOGIN_DENIED;
}

#అయితే_ముగింపు /* protocols are enabled that use SASL */
