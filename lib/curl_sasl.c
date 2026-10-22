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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curlx/base64.h"
#অন্তর্ভাব "vauth/vauth.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "curl_sasl.h"
#অন্তর্ভাব "curl_trc.h"

/* Supported mechanisms */
স্থির ধ্রুব সংরচনা {
  ধ্রুব বর্ণ    *name;  /* Name */
  size_t         len;   /* Name length */
  unsigned short bit;   /* Flag bit */
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
unsigned short Curl_sasl_decode_mech(ধ্রুব বর্ণ *ptr, size_t maxlen,
                                     size_t *len)
{
  unsigned পূর্ণাঙ্ক i;
  বর্ণ c;

  প্রতি(i = 0; mechtable[i].name; i++) {
    যদি(maxlen >= mechtable[i].len &&
       curl_strnequal(ptr, mechtable[i].name, mechtable[i].len)) {
      যদি(len)
        *len = mechtable[i].len;

      যদি(maxlen == mechtable[i].len)
        প্রত্যাগম mechtable[i].bit;

      c = ptr[mechtable[i].len];
      যদি(!ISUPPER(c) && !ISDIGIT(c) && c != '-' && c != '_')
        প্রত্যাগম mechtable[i].bit;
    }
  }

  প্রত্যাগম 0;
}

/*
 * Curl_sasl_parse_url_auth_option()
 *
 * Parse the URL login options.
 */
CURLcode Curl_sasl_parse_url_auth_option(সংরচনা SASL *sasl,
                                         ধ্রুব বর্ণ *মান, size_t len)
{
  CURLcode result = CURLE_OK;
  size_t mechlen;

  যদি(!len)
    প্রত্যাগম CURLE_URL_MALFORMAT;

  যদি(sasl->resetprefs) {
    sasl->resetprefs = FALSE;
    sasl->prefmech = SASL_AUTH_NONE;
  }

  যদি(!strncmp(মান, "*", len))
    sasl->prefmech = SASL_AUTH_DEFAULT;
  অন্যথা {
    unsigned short mechbit = Curl_sasl_decode_mech(মান, len, &mechlen);
    যদি(mechbit && mechlen == len)
      sasl->prefmech |= mechbit;
    অন্যথা
      result = CURLE_URL_MALFORMAT;
  }

  প্রত্যাগম result;
}

/*
 * Curl_sasl_init()
 *
 * Initializes the SASL structure.
 */
শূন্য Curl_sasl_init(সংরচনা SASL *sasl, সংরচনা Curl_easy *data,
                    ধ্রুব সংরচনা SASLproto *params)
{
  uint32_t auth = data->set.httpauth;

  sasl->params = params;           /* Set protocol dependent parameters */
  sasl->state = SASL_STOP;         /* Not yet running */
  sasl->curmech = রিক্ত;            /* No mechanism yet. */
  sasl->authmechs = SASL_AUTH_NONE; /* No known authentication mechanism yet */
  sasl->prefmech = params->defmechs; /* Default preferred mechanisms */
  sasl->authused = SASL_AUTH_NONE; /* The authentication mechanism used */
  sasl->resetprefs = TRUE;         /* Reset prefmech upon AUTH parsing. */
  sasl->mutual_auth = FALSE;       /* No mutual authentication (GSSAPI only) */
  sasl->force_ir = FALSE;          /* Respect external option */

  যদি(auth != CURLAUTH_BASIC) {
    unsigned short mechs = SASL_AUTH_NONE;

    /* If some usable http authentication options have been set, determine
       new defaults from them. */
    যদি(auth & CURLAUTH_BASIC)
      mechs |= SASL_MECH_PLAIN | SASL_MECH_LOGIN;
    যদি(auth & CURLAUTH_DIGEST)
      mechs |= SASL_MECH_DIGEST_MD5;
    যদি(auth & CURLAUTH_NTLM)
      mechs |= SASL_MECH_NTLM;
    যদি(auth & CURLAUTH_BEARER)
      mechs |= SASL_MECH_OAUTHBEARER | SASL_MECH_XOAUTH2;
    যদি(auth & CURLAUTH_GSSAPI)
      mechs |= SASL_MECH_GSSAPI;

    যদি(mechs != SASL_AUTH_NONE)
      sasl->prefmech = mechs;
  }
}

/*
 * sasl_state()
 *
 * This is the ONLY way to change SASL state!
 */
স্থির শূন্য sasl_state(সংরচনা SASL *sasl, সংরচনা Curl_easy *data,
                       saslstate newstate)
{
#যদি defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  স্থির ধ্রুব বর্ণ * ধ্রুব names[] = {
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
    /* LAST */
  };

  যদি(sasl->state != newstate)
    infof(data, "SASL %p state change from %s to %s",
          (শূন্য *)sasl, names[sasl->state], names[newstate]);
#অন্যথা
  (শূন্য)data;
#endif

  sasl->state = newstate;
}

#যদি defined(USE_NTLM) || defined(USE_GSASL) || defined(USE_KERBEROS5) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)
/* Get the SASL server message and convert it to binary. */
স্থির CURLcode get_server_message(সংরচনা SASL *sasl, সংরচনা Curl_easy *data,
                                   সংরচনা bufref *out)
{
  CURLcode result = CURLE_OK;

  result = sasl->params->getmessage(data, out);
  যদি(!result && (sasl->params->flags & SASL_FLAG_BASE64)) {
    ধ্রুব বর্ণ *serverdata = Curl_bufref_ptr(out);

    যদি(!*serverdata)
      Curl_bufref_set(out, রিক্ত, 0, রিক্ত);
    অন্যথা {
      unsigned বর্ণ *বার্তা;
      size_t msglen;

      result = curlx_base64_decode(serverdata, &বার্তা, &msglen);
      যদি(!result)
        Curl_bufref_set(out, বার্তা, msglen, curl_free);
    }
  }
  প্রত্যাগম result;
}
#endif

/* Encode the outgoing SASL message. */
স্থির CURLcode build_message(সংরচনা SASL *sasl, সংরচনা bufref *বার্তা)
{
  CURLcode result = CURLE_OK;

  যদি(sasl->params->flags & SASL_FLAG_BASE64) {
    যদি(!Curl_bufref_ptr(বার্তা))                   /* Empty message. */
      Curl_bufref_set(বার্তা, "", 0, রিক্ত);
    অন্যথা যদি(Curl_bufref_len(বার্তা)) {
      বর্ণ *base64;
      size_t base64len;

      result = curlx_base64_encode(Curl_bufref_uptr(বার্তা),
                                   Curl_bufref_len(বার্তা), &base64, &base64len);
      যদি(!result)
        Curl_bufref_set(বার্তা, base64, base64len, curl_free);
    }
  }

  প্রত্যাগম result;
}

/*
 * Curl_sasl_can_authenticate()
 *
 * Check if we have enough auth data and capabilities to authenticate.
 */
bool Curl_sasl_can_authenticate(সংরচনা SASL *sasl, সংরচনা Curl_easy *data)
{
  /* Have credentials been provided? */
  যদি(data->conn->creds)
    প্রত্যাগম TRUE;

  /* EXTERNAL can authenticate without a username and/or password */
  যদি(sasl->authmechs & sasl->prefmech & SASL_MECH_EXTERNAL)
    প্রত্যাগম TRUE;

  প্রত্যাগম FALSE;
}

সংরচনা sasl_ctx {
  সংরচনা SASL *sasl;
  সংরচনা connectdata *conn;
  unsigned short enabledmechs;
  ধ্রুব বর্ণ *mech;
  saslstate state1;
  saslstate state2;
  সংরচনা bufref resp;
  CURLcode result;
};

স্থির bool sasl_choose_external(সংরচনা Curl_easy *data, সংরচনা sasl_ctx *sctx)
{
  যদি((sctx->enabledmechs & SASL_MECH_EXTERNAL) &&
     !Curl_creds_has_passwd(sctx->conn->creds)) {
    sctx->mech = SASL_MECH_STRING_EXTERNAL;
    sctx->state1 = SASL_EXTERNAL;
    sctx->sasl->authused = SASL_MECH_EXTERNAL;

    যদি(sctx->sasl->force_ir || data->set.sasl_ir)
      Curl_auth_create_external_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    প্রত্যাগম TRUE;
  }
  প্রত্যাগম FALSE;
}

#ifdef USE_KERBEROS5
স্থির bool sasl_choose_krb5(সংরচনা Curl_easy *data, সংরচনা sasl_ctx *sctx)
{
  যদি((sctx->enabledmechs & SASL_MECH_GSSAPI) &&
     Curl_auth_is_gssapi_supported() &&
     Curl_auth_user_contains_domain(sctx->conn->creds)) {
    ধ্রুব বর্ণ *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;

    sctx->sasl->mutual_auth = FALSE;
    sctx->mech = SASL_MECH_STRING_GSSAPI;
    sctx->state1 = SASL_GSSAPI;
    sctx->state2 = SASL_GSSAPI_TOKEN;
    sctx->sasl->authused = SASL_MECH_GSSAPI;

    যদি(sctx->sasl->force_ir || data->set.sasl_ir) {
      সংরচনা kerberos5data *krb5 = Curl_auth_krb5_get(sctx->conn);
      sctx->result = !krb5 ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_gssapi_user_message(data, sctx->conn->creds,
                                             service,
                                             sctx->conn->origin->hostname,
                                             (bool)sctx->sasl->mutual_auth,
                                             রিক্ত, krb5, &sctx->resp);
    }
    প্রত্যাগম TRUE;
  }
  প্রত্যাগম FALSE;
}
#endif /* USE_KERBEROS5 */

#ifdef USE_GSASL
স্থির bool sasl_choose_gsasl(সংরচনা Curl_easy *data, সংরচনা sasl_ctx *sctx)
{
  সংরচনা gsasldata *gsasl;
  সংরচনা bufref nullmsg;

  যদি((sctx->enabledmechs &
      (SASL_MECH_SCRAM_SHA_256 | SASL_MECH_SCRAM_SHA_1))) {
    gsasl = Curl_auth_gsasl_get(sctx->conn);
    যদি(!gsasl) {
      sctx->result = CURLE_OUT_OF_MEMORY;
      প্রত্যাগম TRUE; /* attempted, but failed */
    }

    যদি((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_256) &&
       Curl_auth_gsasl_is_supported(data, SASL_MECH_STRING_SCRAM_SHA_256,
                                    gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_256;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_256;
    }
    অন্যথা যদি((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_1) &&
            Curl_auth_gsasl_is_supported(data, SASL_MECH_STRING_SCRAM_SHA_1,
                                         gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_1;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_1;
    }
    অন্যথা
      প্রত্যাগম FALSE;

    Curl_bufref_init(&nullmsg);
    sctx->state1 = SASL_GSASL;
    sctx->state2 = SASL_GSASL;
    sctx->result = Curl_auth_gsasl_start(data, sctx->conn->creds, gsasl);
    যদি(!sctx->result && (sctx->sasl->force_ir || data->set.sasl_ir))
      sctx->result = Curl_auth_gsasl_token(data, &nullmsg, gsasl, &sctx->resp);
    প্রত্যাগম TRUE;
  }
  প্রত্যাগম FALSE;
}

#endif /* USE_GSASL */

#ifndef CURL_DISABLE_DIGEST_AUTH
স্থির bool sasl_choose_digest(সংরচনা Curl_easy *data, সংরচনা sasl_ctx *sctx)
{
  (শূন্য)data;
  যদি((sctx->enabledmechs & SASL_MECH_DIGEST_MD5) &&
     Curl_auth_is_digest_supported()) {
    sctx->mech = SASL_MECH_STRING_DIGEST_MD5;
    sctx->state1 = SASL_DIGESTMD5;
    sctx->sasl->authused = SASL_MECH_DIGEST_MD5;
    প্রত্যাগম TRUE;
  }
  অন্যথা যদি(sctx->enabledmechs & SASL_MECH_CRAM_MD5) {
    sctx->mech = SASL_MECH_STRING_CRAM_MD5;
    sctx->state1 = SASL_CRAMMD5;
    sctx->sasl->authused = SASL_MECH_CRAM_MD5;
    প্রত্যাগম TRUE;
  }
  প্রত্যাগম FALSE;
}
#endif /* !CURL_DISABLE_DIGEST_AUTH */

#ifdef USE_NTLM
স্থির bool sasl_choose_ntlm(সংরচনা Curl_easy *data, সংরচনা sasl_ctx *sctx)
{
  যদি((sctx->enabledmechs & SASL_MECH_NTLM) &&
     Curl_auth_is_ntlm_supported()) {
     ধ্রুব বর্ণ *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;
    ধ্রুব বর্ণ *hostname;

    Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, রিক্ত);

    sctx->mech = SASL_MECH_STRING_NTLM;
    sctx->state1 = SASL_NTLM;
    sctx->state2 = SASL_NTLM_TYPE2_বার্তা;
    sctx->sasl->authused = SASL_MECH_NTLM;

    যদি(sctx->sasl->force_ir || data->set.sasl_ir) {
      সংরচনা ntlmdata *ntlm = Curl_auth_ntlm_get(sctx->conn, FALSE);
      sctx->result = !ntlm ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_ntlm_type1_message(data, sctx->conn->creds,
                                            service, hostname,
                                            ntlm, &sctx->resp);
    }
    প্রত্যাগম TRUE;
  }
  প্রত্যাগম FALSE;
}
#endif /* USE_NTLM */

স্থির bool sasl_choose_oauth(সংরচনা Curl_easy *data, সংরচনা sasl_ctx *sctx)
{
  যদি(Curl_creds_has_oauth_bearer(data->state.creds) &&
     (sctx->enabledmechs & SASL_MECH_OAUTHBEARER)) {
    ধ্রুব বর্ণ *hostname;
    পূর্ণাঙ্ক port;
    Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, &port);

    sctx->mech = SASL_MECH_STRING_OAUTHBEARER;
    sctx->state1 = SASL_OAUTH2;
    sctx->state2 = SASL_OAUTH2_RESP;
    sctx->sasl->authused = SASL_MECH_OAUTHBEARER;

    যদি(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result =
        Curl_auth_create_oauth_bearer_message(sctx->conn->creds,
                                              hostname, port, &sctx->resp);
    প্রত্যাগম TRUE;
  }
  প্রত্যাগম FALSE;
}

স্থির bool sasl_choose_oauth2(সংরচনা Curl_easy *data, সংরচনা sasl_ctx *sctx)
{
  যদি(Curl_creds_has_oauth_bearer(sctx->conn->creds) &&
     (sctx->enabledmechs & SASL_MECH_XOAUTH2)) {
    sctx->mech = SASL_MECH_STRING_XOAUTH2;
    sctx->state1 = SASL_OAUTH2;
    sctx->sasl->authused = SASL_MECH_XOAUTH2;

    যদি(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result = Curl_auth_create_xoauth_bearer_message(
        sctx->conn->creds, &sctx->resp);
    প্রত্যাগম TRUE;
  }
  প্রত্যাগম FALSE;
}

স্থির bool sasl_choose_plain(সংরচনা Curl_easy *data, সংরচনা sasl_ctx *sctx)
{
  যদি(sctx->enabledmechs & SASL_MECH_PLAIN) {
    sctx->mech = SASL_MECH_STRING_PLAIN;
    sctx->state1 = SASL_PLAIN;
    sctx->sasl->authused = SASL_MECH_PLAIN;

    যদি(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result =
        Curl_auth_create_plain_message(sctx->conn->creds, &sctx->resp);
    প্রত্যাগম TRUE;
  }
  প্রত্যাগম FALSE;
}

স্থির bool sasl_choose_login(সংরচনা Curl_easy *data, সংরচনা sasl_ctx *sctx)
{
  যদি(sctx->enabledmechs & SASL_MECH_LOGIN) {
    sctx->mech = SASL_MECH_STRING_LOGIN;
    sctx->state1 = SASL_LOGIN;
    sctx->state2 = SASL_LOGIN_PASSWD;
    sctx->sasl->authused = SASL_MECH_LOGIN;

    যদি(sctx->sasl->force_ir || data->set.sasl_ir)
      Curl_auth_create_login_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    প্রত্যাগম TRUE;
  }
  প্রত্যাগম FALSE;
}

/*
 * Curl_sasl_start()
 *
 * Calculate the required login details for SASL authentication.
 */
CURLcode Curl_sasl_start(সংরচনা SASL *sasl, সংরচনা Curl_easy *data,
                         bool force_ir, saslprogress *progress)
{
  সংরচনা sasl_ctx sctx;

  sasl->force_ir = force_ir;    /* Latch for future use */
  sasl->authused = 0;           /* No mechanism used yet */
  *progress = SASL_IDLE;

  memset(&sctx, 0, পরিমাণ(sctx));
  sctx.sasl = sasl;
  sctx.conn = data->conn;
  Curl_bufref_init(&sctx.resp);
  sctx.enabledmechs = sasl->authmechs & sasl->prefmech;
  sctx.state1 = SASL_STOP;
  sctx.state2 = SASL_FINAL;

  /* Calculate the supported authentication mechanism, by decreasing order of
     security, as well as the initial response where appropriate */
  যদি(sasl_choose_external(data, &sctx) ||
#ifdef USE_KERBEROS5
     sasl_choose_krb5(data, &sctx) ||
#endif
#ifdef USE_GSASL
     sasl_choose_gsasl(data, &sctx) ||
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
     sasl_choose_digest(data, &sctx) ||
#endif
#ifdef USE_NTLM
     sasl_choose_ntlm(data, &sctx) ||
#endif
     sasl_choose_oauth(data, &sctx) ||
     sasl_choose_oauth2(data, &sctx) ||
     sasl_choose_plain(data, &sctx) ||
     sasl_choose_login(data, &sctx)) {
    /* selected, either we have a mechanism or a failure */
    DEBUGASSERT(sctx.mech || sctx.result);
  }

  যদি(!sctx.result && sctx.mech) {
    sasl->curmech = sctx.mech;
    যদি(Curl_bufref_ptr(&sctx.resp))
      sctx.result = build_message(sasl, &sctx.resp);

    যদি(sasl->params->maxirlen &&
       strlen(sctx.mech) + Curl_bufref_len(&sctx.resp) >
         sasl->params->maxirlen)
      Curl_bufref_free(&sctx.resp);

    যদি(!sctx.result)
      sctx.result = sasl->params->sendauth(data, sctx.mech, &sctx.resp);

    যদি(!sctx.result) {
      *progress = SASL_INPROGRESS;
      sasl_state(sasl, data, Curl_bufref_ptr(&sctx.resp) ?
                 sctx.state2 : sctx.state1);
    }
  }

  Curl_bufref_free(&sctx.resp);
  প্রত্যাগম sctx.result;
}

/*
 * Curl_sasl_continue()
 *
 * Continue the authentication.
 */
CURLcode Curl_sasl_continue(সংরচনা SASL *sasl, সংরচনা Curl_easy *data,
                            পূর্ণাঙ্ক code, saslprogress *progress)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;
  saslstate newstate = SASL_FINAL;
  সংরচনা bufref resp;
  ধ্রুব বর্ণ *hostname;
  পূর্ণাঙ্ক port;
  সংরচনা bufref serverdata;

  Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, &port);
  Curl_bufref_init(&serverdata);
  Curl_bufref_init(&resp);
  *progress = SASL_INPROGRESS;

  যদি(sasl->state == SASL_FINAL) {
    যদি(code != sasl->params->finalcode)
      result = CURLE_LOGIN_DENIED;
    *progress = SASL_DONE;
    sasl_state(sasl, data, SASL_STOP);
    প্রত্যাগম result;
  }

  যদি(sasl->state != SASL_CANCEL && sasl->state != SASL_OAUTH2_RESP &&
     code != sasl->params->contcode) {
    *progress = SASL_DONE;
    sasl_state(sasl, data, SASL_STOP);
    প্রত্যাগম CURLE_LOGIN_DENIED;
  }

  বিকল্প(sasl->state) {
  পক্ষ SASL_STOP:
    *progress = SASL_DONE;
    প্রত্যাগম result;
  পক্ষ SASL_PLAIN:
    result = Curl_auth_create_plain_message(conn->creds, &resp);
    ভঙ্গ;
  পক্ষ SASL_LOGIN:
    Curl_auth_create_login_message(Curl_creds_user(conn->creds), &resp);
    newstate = SASL_LOGIN_PASSWD;
    ভঙ্গ;
  পক্ষ SASL_LOGIN_PASSWD:
    Curl_auth_create_login_message(Curl_creds_passwd(conn->creds), &resp);
    ভঙ্গ;
  পক্ষ SASL_EXTERNAL:
    Curl_auth_create_external_message(Curl_creds_user(conn->creds), &resp);
    ভঙ্গ;
#ifdef USE_GSASL
  পক্ষ SASL_GSASL:
    result = get_server_message(sasl, data, &serverdata);
    যদি(!result) {
      সংরচনা gsasldata *gsasl = Curl_auth_gsasl_get(conn);
      result = !gsasl ? CURLE_OUT_OF_MEMORY :
        Curl_auth_gsasl_token(data, &serverdata, gsasl, &resp);
    }
    যদি(!result && Curl_bufref_len(&resp) > 0)
      newstate = SASL_GSASL;
    ভঙ্গ;
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  পক্ষ SASL_CRAMMD5:
    result = get_server_message(sasl, data, &serverdata);
    যদি(!result)
      result = Curl_auth_create_cram_md5_message(&serverdata, conn->creds,
                                                 &resp);
    ভঙ্গ;
  পক্ষ SASL_DIGESTMD5:
    result = get_server_message(sasl, data, &serverdata);
    যদি(!result)
      result = Curl_auth_create_digest_md5_message(data, &serverdata,
                                                   conn->creds,
                                                   sasl->params->service,
                                                   &resp);
    যদি(!result && (sasl->params->flags & SASL_FLAG_BASE64))
      newstate = SASL_DIGESTMD5_RESP;
    ভঙ্গ;
  পক্ষ SASL_DIGESTMD5_RESP:
    /* Keep response NULL to output an empty line. */
    ভঙ্গ;
#endif

#ifdef USE_NTLM
  পক্ষ SASL_NTLM: {
    /* Create the type-1 message */
    সংরচনা ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    result = !ntlm ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_ntlm_type1_message(data, conn->creds,
                                          sasl->params->service, hostname,
                                          ntlm, &resp);
    newstate = SASL_NTLM_TYPE2_বার্তা;
    ভঙ্গ;
  }
  পক্ষ SASL_NTLM_TYPE2_বার্তা: {
    /* Decode the type-2 message */
    সংরচনা ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    result = !ntlm ? CURLE_OUT_OF_MEMORY :
      get_server_message(sasl, data, &serverdata);
    যদি(!result)
      result = Curl_auth_decode_ntlm_type2_message(data, &serverdata, ntlm);
    যদি(!result)
      result = Curl_auth_create_ntlm_type3_message(data, conn->creds,
                                                   ntlm, &resp);
    ভঙ্গ;
  }
#endif

#ifdef USE_KERBEROS5
  পক্ষ SASL_GSSAPI: {
    সংরচনা kerberos5data *krb5 = Curl_auth_krb5_get(conn);
    result = !krb5 ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_gssapi_user_message(data, conn->creds,
                                           sasl->params->service,
                                           conn->origin->hostname,
                                           (bool)sasl->mutual_auth, রিক্ত,
                                           krb5, &resp);
    newstate = SASL_GSSAPI_TOKEN;
    ভঙ্গ;
  }
  পক্ষ SASL_GSSAPI_TOKEN:
    result = get_server_message(sasl, data, &serverdata);
    যদি(!result) {
      সংরচনা kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      যদি(!krb5)
        result = CURLE_OUT_OF_MEMORY;
      অন্যথা যদি(sasl->mutual_auth) {
        /* Decode the user token challenge and create the optional response
           message */
        result = Curl_auth_create_gssapi_user_message(data, রিক্ত,
                                                      রিক্ত, রিক্ত,
                                                      (bool)sasl->mutual_auth,
                                                      &serverdata,
                                                      krb5, &resp);
        newstate = SASL_GSSAPI_NO_DATA;
      }
      অন্যথা
        /* Decode the security challenge and create the response message */
        result = Curl_auth_create_gssapi_security_message(
          data, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    ভঙ্গ;
  পক্ষ SASL_GSSAPI_NO_DATA:
    /* Decode the security challenge and create the response message */
    result = get_server_message(sasl, data, &serverdata);
    যদি(!result) {
      সংরচনা kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      যদি(!krb5)
        result = CURLE_OUT_OF_MEMORY;
      অন্যথা
        result = Curl_auth_create_gssapi_security_message(
          data, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    ভঙ্গ;
#endif

  পক্ষ SASL_OAUTH2:
    /* Create the authorization message */
    যদি(sasl->authused == SASL_MECH_OAUTHBEARER) {
      result = Curl_auth_create_oauth_bearer_message(conn->creds,
                                                     hostname,
                                                     port,
                                                     &resp);

      /* Failures maybe sent by the server as continuations for OAUTHBEARER */
      newstate = SASL_OAUTH2_RESP;
    }
    অন্যথা
      result = Curl_auth_create_xoauth_bearer_message(conn->creds,
                                                      &resp);
    ভঙ্গ;

  পক্ষ SASL_OAUTH2_RESP:
    /* The continuation is optional so check the response code */
    যদি(code == sasl->params->finalcode) {
      /* Final response was received so we are done */
      *progress = SASL_DONE;
      sasl_state(sasl, data, SASL_STOP);
      প্রত্যাগম result;
    }
    অন্যথা যদি(code == sasl->params->contcode) {
      /* Acknowledge the continuation by sending a 0x01 response. */
      Curl_bufref_set(&resp, "\x01", 1, রিক্ত);
      ভঙ্গ;
    }
    অন্যথা {
      *progress = SASL_DONE;
      sasl_state(sasl, data, SASL_STOP);
      প্রত্যাগম CURLE_LOGIN_DENIED;
    }

  পক্ষ SASL_CANCEL:
    /* Remove the offending mechanism from the supported list */
    sasl->authmechs &= (unsigned short)~sasl->authused;
    sasl->authused = SASL_AUTH_NONE;
    sasl->curmech = রিক্ত;

    /* Start an alternative SASL authentication */
    প্রত্যাগম Curl_sasl_start(sasl, data, (bool)sasl->force_ir, progress);
  স্বভাব:
    failf(data, "Unsupported SASL authentication mechanism");
    result = CURLE_UNSUPPORTED_PROTOCOL;  /* Should not happen */
    ভঙ্গ;
  }

  Curl_bufref_free(&serverdata);

  বিকল্প(result) {
  পক্ষ CURLE_BAD_CONTENT_ENCODING:
    /* Cancel dialog */
    result = sasl->params->cancelauth(data, sasl->curmech);
    newstate = SASL_CANCEL;
    ভঙ্গ;
  পক্ষ CURLE_OK:
    result = build_message(sasl, &resp);
    যদি(!result)
      result = sasl->params->contauth(data, sasl->curmech, &resp);
    ভঙ্গ;
  স্বভাব:
    newstate = SASL_STOP;    /* Stop on error */
    *progress = SASL_DONE;
    ভঙ্গ;
  }

  Curl_bufref_free(&resp);

  sasl_state(sasl, data, newstate);

  প্রত্যাগম result;
}

#ifdef CURLVERBOSE
স্থির শূন্য sasl_unchosen(সংরচনা Curl_easy *data, unsigned short mech,
                          unsigned short enabledmechs,
                          bool built_in, bool platform,
                          ধ্রুব বর্ণ *param_missing)
{
  ধ্রুব বর্ণ *mname = রিক্ত;
  size_t i;

  যদি(!(enabledmechs & mech))
    প্রত্যাগম;

  প্রতি(i = 0; mechtable[i].name; ++i) {
    যদি(mechtable[i].bit == mech) {
      mname = mechtable[i].name;
      ভঙ্গ;
    }
  }
  যদি(!mname)  /* should not happen */
    প্রত্যাগম;
  যদি(!built_in)
    infof(data, "SASL: %s not builtin", mname);
  অন্যথা যদি(!platform)
    infof(data, "SASL: %s not supported by the platform/libraries", mname);
  অন্যথা {
    যদি(param_missing)
      infof(data, "SASL: %s is missing %s", mname, param_missing);
    যদি(!Curl_creds_has_user(data->conn->creds))
      infof(data, "SASL: %s is missing username", mname);
  }
}
#endif /* CURLVERBOSE */

CURLcode Curl_sasl_is_blocked(সংরচনা SASL *sasl, সংরচনা Curl_easy *data)
{
#ifdef CURLVERBOSE
#ifdef USE_KERBEROS5
#define CURL_SASL_KERBEROS5   TRUE
#অন্যথা
#define CURL_SASL_KERBEROS5   FALSE
#endif
#ifdef USE_GSASL
#define CURL_SASL_GASL        TRUE
#অন্যথা
#define CURL_SASL_GASL        FALSE
#endif
#ifdef CURL_DISABLE_DIGEST_AUTH
#define CURL_SASL_DIGEST      TRUE
#অন্যথা
#define CURL_SASL_DIGEST      FALSE
#endif
#ifndef USE_NTLM
#define CURL_SASL_NTLM        TRUE
#অন্যথা
#define CURL_SASL_NTLM        FALSE
#endif
  /* Failing SASL authentication is a pain. Give a helping hand if
   * we were unable to select an AUTH mechanism.
   * `sasl->authmechs` are mechanisms offered by the peer
   * `sasl->prefmech`  are mechanisms preferred by us */
  unsigned short enabledmechs = sasl->authmechs & sasl->prefmech;

  যদি(!sasl->authmechs)
    infof(data, "SASL: no auth mechanism was offered or recognized");
  অন্যথা যদি(!enabledmechs)
    infof(data, "SASL: no overlap between offered and configured "
          "auth mechanisms");
  অন্যথা {
    infof(data, "SASL: no auth mechanism offered could be selected");
    যদি((enabledmechs & SASL_MECH_EXTERNAL) &&
       Curl_creds_has_passwd(data->conn->creds))
      infof(data, "SASL: auth EXTERNAL not chosen with password");
    sasl_unchosen(data, SASL_MECH_GSSAPI, enabledmechs,
                  CURL_SASL_KERBEROS5, Curl_auth_is_gssapi_supported(), রিক্ত);
    sasl_unchosen(data, SASL_MECH_SCRAM_SHA_256, enabledmechs,
                  CURL_SASL_GASL, FALSE, রিক্ত);
    sasl_unchosen(data, SASL_MECH_SCRAM_SHA_1, enabledmechs,
                  CURL_SASL_GASL, FALSE, রিক্ত);
    sasl_unchosen(data, SASL_MECH_DIGEST_MD5, enabledmechs,
                  CURL_SASL_DIGEST, Curl_auth_is_digest_supported(), রিক্ত);
    sasl_unchosen(data, SASL_MECH_CRAM_MD5, enabledmechs,
                  CURL_SASL_DIGEST, TRUE, রিক্ত);
    sasl_unchosen(data, SASL_MECH_NTLM, enabledmechs,
                  CURL_SASL_NTLM, Curl_auth_is_ntlm_supported(), রিক্ত);
    sasl_unchosen(data, SASL_MECH_OAUTHBEARER, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(data->conn->creds) ?
                  রিক্ত : "CURLOPT_XOAUTH2_BEARER");
    sasl_unchosen(data, SASL_MECH_XOAUTH2, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(data->conn->creds) ?
                  রিক্ত : "CURLOPT_XOAUTH2_BEARER");
  }
#endif /* CURLVERBOSE */
  (শূন্য)sasl;
  (শূন্য)data;
  প্রত্যাগম CURLE_LOGIN_DENIED;
}

#endif /* protocols are enabled that use SASL */
