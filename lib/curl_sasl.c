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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/base64.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sasl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"

/* Supported mechanisms */
ਸਥਿਰ ਅਚਲ ਬਣਤਰ {
  ਅਚਲ ਅੱਖਰ    *name;  /* Name */
  size_t         len;   /* Name length */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਦੋਆਧਾਰੀ_ਅੰਕ_2;   /* Flag bit */
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
ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ Curl_sasl_decode_mech(ਅਚਲ ਅੱਖਰ *ptr, size_t maxlen,
                                     size_t *len)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
  ਅੱਖਰ c;

  ਲਈ(i = 0; mechtable[i].name; i++) {
    ਜੇ(maxlen >= mechtable[i].len &&
       curl_strnequal(ptr, mechtable[i].name, mechtable[i].len)) {
      ਜੇ(len)
        *len = mechtable[i].len;

      ਜੇ(maxlen == mechtable[i].len)
        ਵਾਪਸ mechtable[i].ਦੋਆਧਾਰੀ_ਅੰਕ_2;

      c = ptr[mechtable[i].len];
      ਜੇ(!ISUPPER(c) && !ISDIGIT(c) && c != '-' && c != '_')
        ਵਾਪਸ mechtable[i].ਦੋਆਧਾਰੀ_ਅੰਕ_2;
    }
  }

  ਵਾਪਸ 0;
}

/*
 * Curl_sasl_parse_url_auth_option()
 *
 * Parse the URL login options.
 */
CURLcode Curl_sasl_parse_url_auth_option(ਬਣਤਰ SASL *sasl,
                                         ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, size_t len)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t mechlen;

  ਜੇ(!len)
    ਵਾਪਸ CURLE_URL_MALFORMAT;

  ਜੇ(sasl->resetprefs) {
    sasl->resetprefs = FALSE;
    sasl->prefmech = SASL_AUTH_NONE;
  }

  ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(ਮੁੱਲ, "*", len))
    sasl->prefmech = SASL_AUTH_DEFAULT;
  ਨਹੀਂਤਾਂ {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ mechbit = Curl_sasl_decode_mech(ਮੁੱਲ, len, &mechlen);
    ਜੇ(mechbit && mechlen == len)
      sasl->prefmech |= mechbit;
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = CURLE_URL_MALFORMAT;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Curl_sasl_init()
 *
 * Initializes the SASL structure.
 */
ਖਾਲੀ Curl_sasl_init(ਬਣਤਰ SASL *sasl, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                    ਅਚਲ ਬਣਤਰ SASLproto *params)
{
  uint32_t auth = ਅੰਕੜੇ->ਸੈੱਟ.httpauth;

  sasl->params = params;           /* Set protocol dependent parameters */
  sasl->state = SASL_STOP;         /* Not yet running */
  sasl->curmech = ਨਿਰਾ;            /* No mechanism yet. */
  sasl->authmechs = SASL_AUTH_NONE; /* No known authentication mechanism yet */
  sasl->prefmech = params->defmechs; /* Default preferred mechanisms */
  sasl->authused = SASL_AUTH_NONE; /* The authentication mechanism used */
  sasl->resetprefs = TRUE;         /* Reset prefmech upon AUTH parsing. */
  sasl->mutual_auth = FALSE;       /* No mutual authentication (GSSAPI only) */
  sasl->force_ir = FALSE;          /* Respect external option */

  ਜੇ(auth != CURLAUTH_BASIC) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ mechs = SASL_AUTH_NONE;

    /* If some usable http authentication options have been set, determine
       new defaults from them. */
    ਜੇ(auth & CURLAUTH_BASIC)
      mechs |= SASL_MECH_PLAIN | SASL_MECH_LOGIN;
    ਜੇ(auth & CURLAUTH_DIGEST)
      mechs |= SASL_MECH_DIGEST_MD5;
    ਜੇ(auth & CURLAUTH_NTLM)
      mechs |= SASL_MECH_NTLM;
    ਜੇ(auth & CURLAUTH_BEARER)
      mechs |= SASL_MECH_OAUTHBEARER | SASL_MECH_XOAUTH2;
    ਜੇ(auth & CURLAUTH_GSSAPI)
      mechs |= SASL_MECH_GSSAPI;

    ਜੇ(mechs != SASL_AUTH_NONE)
      sasl->prefmech = mechs;
  }
}

/*
 * sasl_state()
 *
 * This is the ONLY way to change SASL state!
 */
ਸਥਿਰ ਖਾਲੀ sasl_state(ਬਣਤਰ SASL *sasl, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                       saslstate newstate)
{
#ਜੇ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ ਨਾਂ[] = {
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
    /*ਆਖਰੀ*/
  };

  ਜੇ(sasl->state != newstate)
    infof(ਅੰਕੜੇ, "SASL %p state change from %s to %s",
          (ਖਾਲੀ *)sasl, ਨਾਂ[sasl->state], ਨਾਂ[newstate]);
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
#ਜੇ_ਅੰਤ

  sasl->state = newstate;
}

#ਜੇ defined(USE_NTLM) || defined(USE_GSASL) || defined(USE_KERBEROS5) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)
/* Get the SASL server message and convert it to binary. */
ਸਥਿਰ CURLcode get_server_ਸੁਨੇਹਾ(ਬਣਤਰ SASL *sasl, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ bufref *ਬਾਹਰਲਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਨਤੀਜਾ = sasl->params->getmessage(ਅੰਕੜੇ, ਬਾਹਰਲਾ);
  ਜੇ(!ਨਤੀਜਾ && (sasl->params->flags & SASL_FLAG_BASE64)) {
    ਅਚਲ ਅੱਖਰ *serverdata = Curl_bufref_ptr(ਬਾਹਰਲਾ);

    ਜੇ(!*serverdata)
      Curl_bufref_ਸੈੱਟ(ਬਾਹਰਲਾ, ਨਿਰਾ, 0, ਨਿਰਾ);
    ਨਹੀਂਤਾਂ {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਸੁਨੇਹਾ;
      size_t msglen;

      ਨਤੀਜਾ = curlx_base64_decode(serverdata, &ਸੁਨੇਹਾ, &msglen);
      ਜੇ(!ਨਤੀਜਾ)
        Curl_bufref_ਸੈੱਟ(ਬਾਹਰਲਾ, ਸੁਨੇਹਾ, msglen, curl_free);
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

/* Encode the outgoing SASL message. */
ਸਥਿਰ CURLcode build_ਸੁਨੇਹਾ(ਬਣਤਰ SASL *sasl, ਬਣਤਰ bufref *ਸੁਨੇਹਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(sasl->params->flags & SASL_FLAG_BASE64) {
    ਜੇ(!Curl_bufref_ptr(ਸੁਨੇਹਾ))                   /* Empty message. */
      Curl_bufref_ਸੈੱਟ(ਸੁਨੇਹਾ, "", 0, ਨਿਰਾ);
    ਨਹੀਂਤਾਂ ਜੇ(Curl_bufref_len(ਸੁਨੇਹਾ)) {
      ਅੱਖਰ *base64;
      size_t base64len;

      ਨਤੀਜਾ = curlx_base64_encode(Curl_bufref_uptr(ਸੁਨੇਹਾ),
                                   Curl_bufref_len(ਸੁਨੇਹਾ), &base64, &base64len);
      ਜੇ(!ਨਤੀਜਾ)
        Curl_bufref_ਸੈੱਟ(ਸੁਨੇਹਾ, base64, base64len, curl_free);
    }
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Curl_sasl_can_authenticate()
 *
 * Check if we have enough auth data and capabilities to authenticate.
 */
bool Curl_sasl_can_authenticate(ਬਣਤਰ SASL *sasl, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  /* Have credentials been provided? */
  ਜੇ(ਅੰਕੜੇ->conn->creds)
    ਵਾਪਸ TRUE;

  /* EXTERNAL can authenticate without a username and/or password */
  ਜੇ(sasl->authmechs & sasl->prefmech & SASL_MECH_EXTERNAL)
    ਵਾਪਸ TRUE;

  ਵਾਪਸ FALSE;
}

ਬਣਤਰ sasl_ctx {
  ਬਣਤਰ SASL *sasl;
  ਬਣਤਰ connectdata *conn;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ enabledmechs;
  ਅਚਲ ਅੱਖਰ *mech;
  saslstate state1;
  saslstate state2;
  ਬਣਤਰ bufref resp;
  CURLcode ਨਤੀਜਾ;
};

ਸਥਿਰ bool sasl_ਚੁਣੋ_external(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ sasl_ctx *sctx)
{
  ਜੇ((sctx->enabledmechs & SASL_MECH_EXTERNAL) &&
     !Curl_creds_has_passwd(sctx->conn->creds)) {
    sctx->mech = SASL_MECH_STRING_EXTERNAL;
    sctx->state1 = SASL_EXTERNAL;
    sctx->sasl->authused = SASL_MECH_EXTERNAL;

    ਜੇ(sctx->sasl->force_ir || ਅੰਕੜੇ->ਸੈੱਟ.sasl_ir)
      Curl_auth_create_external_ਸੁਨੇਹਾ(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_KERBEROS5
ਸਥਿਰ bool sasl_ਚੁਣੋ_krb5(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ sasl_ctx *sctx)
{
  ਜੇ((sctx->enabledmechs & SASL_MECH_GSSAPI) &&
     Curl_auth_is_gssapi_supported() &&
     Curl_auth_user_ਸ਼ਾਮਲ_ਹੈ_domain(sctx->conn->creds)) {
    ਅਚਲ ਅੱਖਰ *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;

    sctx->sasl->mutual_auth = FALSE;
    sctx->mech = SASL_MECH_STRING_GSSAPI;
    sctx->state1 = SASL_GSSAPI;
    sctx->state2 = SASL_GSSAPI_TOKEN;
    sctx->sasl->authused = SASL_MECH_GSSAPI;

    ਜੇ(sctx->sasl->force_ir || ਅੰਕੜੇ->ਸੈੱਟ.sasl_ir) {
      ਬਣਤਰ kerberos5data *krb5 = Curl_auth_krb5_get(sctx->conn);
      sctx->ਨਤੀਜਾ = !krb5 ? CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ :
        Curl_auth_create_gssapi_user_ਸੁਨੇਹਾ(ਅੰਕੜੇ, sctx->conn->creds,
                                             service,
                                             sctx->conn->origin->hostname,
                                             (bool)sctx->sasl->mutual_auth,
                                             ਨਿਰਾ, krb5, &sctx->resp);
    }
    ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ /* USE_KERBEROS5 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GSASL
ਸਥਿਰ bool sasl_ਚੁਣੋ_gsasl(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ sasl_ctx *sctx)
{
  ਬਣਤਰ gsasldata *gsasl;
  ਬਣਤਰ bufref nullmsg;

  ਜੇ((sctx->enabledmechs &
      (SASL_MECH_SCRAM_SHA_256 | SASL_MECH_SCRAM_SHA_1))) {
    gsasl = Curl_auth_gsasl_get(sctx->conn);
    ਜੇ(!gsasl) {
      sctx->ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਵਾਪਸ TRUE; /* attempted, but failed */
    }

    ਜੇ((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_256) &&
       Curl_auth_gsasl_is_supported(ਅੰਕੜੇ, SASL_MECH_STRING_SCRAM_SHA_256,
                                    gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_256;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_256;
    }
    ਨਹੀਂਤਾਂ ਜੇ((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_1) &&
            Curl_auth_gsasl_is_supported(ਅੰਕੜੇ, SASL_MECH_STRING_SCRAM_SHA_1,
                                         gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_1;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_1;
    }
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ FALSE;

    Curl_bufref_init(&nullmsg);
    sctx->state1 = SASL_GSASL;
    sctx->state2 = SASL_GSASL;
    sctx->ਨਤੀਜਾ = Curl_auth_gsasl_ਸ਼ੁਰੂ(ਅੰਕੜੇ, sctx->conn->creds, gsasl);
    ਜੇ(!sctx->ਨਤੀਜਾ && (sctx->sasl->force_ir || ਅੰਕੜੇ->ਸੈੱਟ.sasl_ir))
      sctx->ਨਤੀਜਾ = Curl_auth_gsasl_token(ਅੰਕੜੇ, &nullmsg, gsasl, &sctx->resp);
    ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}

#ਜੇ_ਅੰਤ /* USE_GSASL */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
ਸਥਿਰ bool sasl_ਚੁਣੋ_digest(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ sasl_ctx *sctx)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ((sctx->enabledmechs & SASL_MECH_DIGEST_MD5) &&
     Curl_auth_is_digest_supported()) {
    sctx->mech = SASL_MECH_STRING_DIGEST_MD5;
    sctx->state1 = SASL_DIGESTMD5;
    sctx->sasl->authused = SASL_MECH_DIGEST_MD5;
    ਵਾਪਸ TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(sctx->enabledmechs & SASL_MECH_CRAM_MD5) {
    sctx->mech = SASL_MECH_STRING_CRAM_MD5;
    sctx->state1 = SASL_CRAMMD5;
    sctx->sasl->authused = SASL_MECH_CRAM_MD5;
    ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_DIGEST_AUTH */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NTLM
ਸਥਿਰ bool sasl_ਚੁਣੋ_ntlm(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ sasl_ctx *sctx)
{
  ਜੇ((sctx->enabledmechs & SASL_MECH_NTLM) &&
     Curl_auth_is_ntlm_supported()) {
     ਅਚਲ ਅੱਖਰ *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;
    ਅਚਲ ਅੱਖਰ *hostname;

    Curl_conn_get_current_host(ਅੰਕੜੇ, FIRSTSOCKET, &hostname, ਨਿਰਾ);

    sctx->mech = SASL_MECH_STRING_NTLM;
    sctx->state1 = SASL_NTLM;
    sctx->state2 = SASL_NTLM_TYPE2_ਸੁਨੇਹਾ;
    sctx->sasl->authused = SASL_MECH_NTLM;

    ਜੇ(sctx->sasl->force_ir || ਅੰਕੜੇ->ਸੈੱਟ.sasl_ir) {
      ਬਣਤਰ ntlmdata *ntlm = Curl_auth_ntlm_get(sctx->conn, FALSE);
      sctx->ਨਤੀਜਾ = !ntlm ? CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ :
        Curl_auth_create_ntlm_type1_ਸੁਨੇਹਾ(ਅੰਕੜੇ, sctx->conn->creds,
                                            service, hostname,
                                            ntlm, &sctx->resp);
    }
    ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ /* USE_NTLM */

ਸਥਿਰ bool sasl_ਚੁਣੋ_oauth(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ sasl_ctx *sctx)
{
  ਜੇ(Curl_creds_has_oauth_bearer(ਅੰਕੜੇ->state.creds) &&
     (sctx->enabledmechs & SASL_MECH_OAUTHBEARER)) {
    ਅਚਲ ਅੱਖਰ *hostname;
    ਪੂਰਨ ਪੋਰਟ;
    Curl_conn_get_current_host(ਅੰਕੜੇ, FIRSTSOCKET, &hostname, &ਪੋਰਟ);

    sctx->mech = SASL_MECH_STRING_OAUTHBEARER;
    sctx->state1 = SASL_OAUTH2;
    sctx->state2 = SASL_OAUTH2_RESP;
    sctx->sasl->authused = SASL_MECH_OAUTHBEARER;

    ਜੇ(sctx->sasl->force_ir || ਅੰਕੜੇ->ਸੈੱਟ.sasl_ir)
      sctx->ਨਤੀਜਾ =
        Curl_auth_create_oauth_bearer_ਸੁਨੇਹਾ(sctx->conn->creds,
                                              hostname, ਪੋਰਟ, &sctx->resp);
    ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ bool sasl_ਚੁਣੋ_oauth2(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ sasl_ctx *sctx)
{
  ਜੇ(Curl_creds_has_oauth_bearer(sctx->conn->creds) &&
     (sctx->enabledmechs & SASL_MECH_XOAUTH2)) {
    sctx->mech = SASL_MECH_STRING_XOAUTH2;
    sctx->state1 = SASL_OAUTH2;
    sctx->sasl->authused = SASL_MECH_XOAUTH2;

    ਜੇ(sctx->sasl->force_ir || ਅੰਕੜੇ->ਸੈੱਟ.sasl_ir)
      sctx->ਨਤੀਜਾ = Curl_auth_create_xoauth_bearer_ਸੁਨੇਹਾ(
        sctx->conn->creds, &sctx->resp);
    ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ bool sasl_ਚੁਣੋ_plain(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ sasl_ctx *sctx)
{
  ਜੇ(sctx->enabledmechs & SASL_MECH_PLAIN) {
    sctx->mech = SASL_MECH_STRING_PLAIN;
    sctx->state1 = SASL_PLAIN;
    sctx->sasl->authused = SASL_MECH_PLAIN;

    ਜੇ(sctx->sasl->force_ir || ਅੰਕੜੇ->ਸੈੱਟ.sasl_ir)
      sctx->ਨਤੀਜਾ =
        Curl_auth_create_plain_ਸੁਨੇਹਾ(sctx->conn->creds, &sctx->resp);
    ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ bool sasl_ਚੁਣੋ_login(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ sasl_ctx *sctx)
{
  ਜੇ(sctx->enabledmechs & SASL_MECH_LOGIN) {
    sctx->mech = SASL_MECH_STRING_LOGIN;
    sctx->state1 = SASL_LOGIN;
    sctx->state2 = SASL_LOGIN_PASSWD;
    sctx->sasl->authused = SASL_MECH_LOGIN;

    ਜੇ(sctx->sasl->force_ir || ਅੰਕੜੇ->ਸੈੱਟ.sasl_ir)
      Curl_auth_create_login_ਸੁਨੇਹਾ(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}

/*
 * Curl_sasl_start()
 *
 * Calculate the required login details for SASL authentication.
 */
CURLcode Curl_sasl_ਸ਼ੁਰੂ(ਬਣਤਰ SASL *sasl, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         bool force_ir, saslprogress *ਤਰੱਕੀ)
{
  ਬਣਤਰ sasl_ctx sctx;

  sasl->force_ir = force_ir;    /* Latch for future use */
  sasl->authused = 0;           /* No mechanism used yet */
  *ਤਰੱਕੀ = SASL_IDLE;

  memset(&sctx, 0, ਆਕਾਰ(sctx));
  sctx.sasl = sasl;
  sctx.conn = ਅੰਕੜੇ->conn;
  Curl_bufref_init(&sctx.resp);
  sctx.enabledmechs = sasl->authmechs & sasl->prefmech;
  sctx.state1 = SASL_STOP;
  sctx.state2 = SASL_FINAL;

  /* Calculate the supported authentication mechanism, by decreasing order of
     security, as well as the initial response where appropriate */
  ਜੇ(sasl_ਚੁਣੋ_external(ਅੰਕੜੇ, &sctx) ||
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_KERBEROS5
     sasl_ਚੁਣੋ_krb5(ਅੰਕੜੇ, &sctx) ||
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GSASL
     sasl_ਚੁਣੋ_gsasl(ਅੰਕੜੇ, &sctx) ||
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
     sasl_ਚੁਣੋ_digest(ਅੰਕੜੇ, &sctx) ||
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NTLM
     sasl_ਚੁਣੋ_ntlm(ਅੰਕੜੇ, &sctx) ||
#ਜੇ_ਅੰਤ
     sasl_ਚੁਣੋ_oauth(ਅੰਕੜੇ, &sctx) ||
     sasl_ਚੁਣੋ_oauth2(ਅੰਕੜੇ, &sctx) ||
     sasl_ਚੁਣੋ_plain(ਅੰਕੜੇ, &sctx) ||
     sasl_ਚੁਣੋ_login(ਅੰਕੜੇ, &sctx)) {
    /* selected, either we have a mechanism or a failure */
    DEBUGASSERT(sctx.mech || sctx.ਨਤੀਜਾ);
  }

  ਜੇ(!sctx.ਨਤੀਜਾ && sctx.mech) {
    sasl->curmech = sctx.mech;
    ਜੇ(Curl_bufref_ptr(&sctx.resp))
      sctx.ਨਤੀਜਾ = build_ਸੁਨੇਹਾ(sasl, &sctx.resp);

    ਜੇ(sasl->params->maxirlen &&
       ਲੜੀ_ਲੰਬਾਈ(sctx.mech) + Curl_bufref_len(&sctx.resp) >
         sasl->params->maxirlen)
      Curl_bufref_free(&sctx.resp);

    ਜੇ(!sctx.ਨਤੀਜਾ)
      sctx.ਨਤੀਜਾ = sasl->params->sendauth(ਅੰਕੜੇ, sctx.mech, &sctx.resp);

    ਜੇ(!sctx.ਨਤੀਜਾ) {
      *ਤਰੱਕੀ = SASL_INPROGRESS;
      sasl_state(sasl, ਅੰਕੜੇ, Curl_bufref_ptr(&sctx.resp) ?
                 sctx.state2 : sctx.state1);
    }
  }

  Curl_bufref_free(&sctx.resp);
  ਵਾਪਸ sctx.ਨਤੀਜਾ;
}

/*
 * Curl_sasl_continue()
 *
 * Continue the authentication.
 */
CURLcode Curl_sasl_continue(ਬਣਤਰ SASL *sasl, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਪੂਰਨ ਕੋਡ, saslprogress *ਤਰੱਕੀ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  saslstate newstate = SASL_FINAL;
  ਬਣਤਰ bufref resp;
  ਅਚਲ ਅੱਖਰ *hostname;
  ਪੂਰਨ ਪੋਰਟ;
  ਬਣਤਰ bufref serverdata;

  Curl_conn_get_current_host(ਅੰਕੜੇ, FIRSTSOCKET, &hostname, &ਪੋਰਟ);
  Curl_bufref_init(&serverdata);
  Curl_bufref_init(&resp);
  *ਤਰੱਕੀ = SASL_INPROGRESS;

  ਜੇ(sasl->state == SASL_FINAL) {
    ਜੇ(ਕੋਡ != sasl->params->finalcode)
      ਨਤੀਜਾ = CURLE_LOGIN_DENIED;
    *ਤਰੱਕੀ = SASL_ਪੂਰਾ;
    sasl_state(sasl, ਅੰਕੜੇ, SASL_STOP);
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(sasl->state != SASL_CANCEL && sasl->state != SASL_OAUTH2_RESP &&
     ਕੋਡ != sasl->params->contcode) {
    *ਤਰੱਕੀ = SASL_ਪੂਰਾ;
    sasl_state(sasl, ਅੰਕੜੇ, SASL_STOP);
    ਵਾਪਸ CURLE_LOGIN_DENIED;
  }

  ਚੋਣ(sasl->state) {
  ਹਾਲਤ SASL_STOP:
    *ਤਰੱਕੀ = SASL_ਪੂਰਾ;
    ਵਾਪਸ ਨਤੀਜਾ;
  ਹਾਲਤ SASL_PLAIN:
    ਨਤੀਜਾ = Curl_auth_create_plain_ਸੁਨੇਹਾ(conn->creds, &resp);
    ਰੋਕੋ;
  ਹਾਲਤ SASL_LOGIN:
    Curl_auth_create_login_ਸੁਨੇਹਾ(Curl_creds_user(conn->creds), &resp);
    newstate = SASL_LOGIN_PASSWD;
    ਰੋਕੋ;
  ਹਾਲਤ SASL_LOGIN_PASSWD:
    Curl_auth_create_login_ਸੁਨੇਹਾ(Curl_creds_passwd(conn->creds), &resp);
    ਰੋਕੋ;
  ਹਾਲਤ SASL_EXTERNAL:
    Curl_auth_create_external_ਸੁਨੇਹਾ(Curl_creds_user(conn->creds), &resp);
    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GSASL
  ਹਾਲਤ SASL_GSASL:
    ਨਤੀਜਾ = get_server_ਸੁਨੇਹਾ(sasl, ਅੰਕੜੇ, &serverdata);
    ਜੇ(!ਨਤੀਜਾ) {
      ਬਣਤਰ gsasldata *gsasl = Curl_auth_gsasl_get(conn);
      ਨਤੀਜਾ = !gsasl ? CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ :
        Curl_auth_gsasl_token(ਅੰਕੜੇ, &serverdata, gsasl, &resp);
    }
    ਜੇ(!ਨਤੀਜਾ && Curl_bufref_len(&resp) > 0)
      newstate = SASL_GSASL;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
  ਹਾਲਤ SASL_CRAMMD5:
    ਨਤੀਜਾ = get_server_ਸੁਨੇਹਾ(sasl, ਅੰਕੜੇ, &serverdata);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = Curl_auth_create_cram_md5_ਸੁਨੇਹਾ(&serverdata, conn->creds,
                                                 &resp);
    ਰੋਕੋ;
  ਹਾਲਤ SASL_DIGESTMD5:
    ਨਤੀਜਾ = get_server_ਸੁਨੇਹਾ(sasl, ਅੰਕੜੇ, &serverdata);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = Curl_auth_create_digest_md5_ਸੁਨੇਹਾ(ਅੰਕੜੇ, &serverdata,
                                                   conn->creds,
                                                   sasl->params->service,
                                                   &resp);
    ਜੇ(!ਨਤੀਜਾ && (sasl->params->flags & SASL_FLAG_BASE64))
      newstate = SASL_DIGESTMD5_RESP;
    ਰੋਕੋ;
  ਹਾਲਤ SASL_DIGESTMD5_RESP:
    /* Keep response NULL to output an empty line. */
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NTLM
  ਹਾਲਤ SASL_NTLM: {
    /* Create the type-1 message */
    ਬਣਤਰ ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    ਨਤੀਜਾ = !ntlm ? CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ :
      Curl_auth_create_ntlm_type1_ਸੁਨੇਹਾ(ਅੰਕੜੇ, conn->creds,
                                          sasl->params->service, hostname,
                                          ntlm, &resp);
    newstate = SASL_NTLM_TYPE2_ਸੁਨੇਹਾ;
    ਰੋਕੋ;
  }
  ਹਾਲਤ SASL_NTLM_TYPE2_ਸੁਨੇਹਾ: {
    /* Decode the type-2 message */
    ਬਣਤਰ ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    ਨਤੀਜਾ = !ntlm ? CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ :
      get_server_ਸੁਨੇਹਾ(sasl, ਅੰਕੜੇ, &serverdata);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = Curl_auth_decode_ntlm_type2_ਸੁਨੇਹਾ(ਅੰਕੜੇ, &serverdata, ntlm);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = Curl_auth_create_ntlm_type3_ਸੁਨੇਹਾ(ਅੰਕੜੇ, conn->creds,
                                                   ntlm, &resp);
    ਰੋਕੋ;
  }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_KERBEROS5
  ਹਾਲਤ SASL_GSSAPI: {
    ਬਣਤਰ kerberos5data *krb5 = Curl_auth_krb5_get(conn);
    ਨਤੀਜਾ = !krb5 ? CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ :
      Curl_auth_create_gssapi_user_ਸੁਨੇਹਾ(ਅੰਕੜੇ, conn->creds,
                                           sasl->params->service,
                                           conn->origin->hostname,
                                           (bool)sasl->mutual_auth, ਨਿਰਾ,
                                           krb5, &resp);
    newstate = SASL_GSSAPI_TOKEN;
    ਰੋਕੋ;
  }
  ਹਾਲਤ SASL_GSSAPI_TOKEN:
    ਨਤੀਜਾ = get_server_ਸੁਨੇਹਾ(sasl, ਅੰਕੜੇ, &serverdata);
    ਜੇ(!ਨਤੀਜਾ) {
      ਬਣਤਰ kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      ਜੇ(!krb5)
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਨਹੀਂਤਾਂ ਜੇ(sasl->mutual_auth) {
        /* Decode the user token challenge and create the optional response
ਸੁਨੇਹਾ*/
        ਨਤੀਜਾ = Curl_auth_create_gssapi_user_ਸੁਨੇਹਾ(ਅੰਕੜੇ, ਨਿਰਾ,
                                                      ਨਿਰਾ, ਨਿਰਾ,
                                                      (bool)sasl->mutual_auth,
                                                      &serverdata,
                                                      krb5, &resp);
        newstate = SASL_GSSAPI_NO_ਅੰਕੜੇ;
      }
      ਨਹੀਂਤਾਂ
        /* Decode the security challenge and create the response message */
        ਨਤੀਜਾ = Curl_auth_create_gssapi_security_ਸੁਨੇਹਾ(
          ਅੰਕੜੇ, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    ਰੋਕੋ;
  ਹਾਲਤ SASL_GSSAPI_NO_ਅੰਕੜੇ:
    /* Decode the security challenge and create the response message */
    ਨਤੀਜਾ = get_server_ਸੁਨੇਹਾ(sasl, ਅੰਕੜੇ, &serverdata);
    ਜੇ(!ਨਤੀਜਾ) {
      ਬਣਤਰ kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      ਜੇ(!krb5)
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਨਹੀਂਤਾਂ
        ਨਤੀਜਾ = Curl_auth_create_gssapi_security_ਸੁਨੇਹਾ(
          ਅੰਕੜੇ, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

  ਹਾਲਤ SASL_OAUTH2:
    /* Create the authorization message */
    ਜੇ(sasl->authused == SASL_MECH_OAUTHBEARER) {
      ਨਤੀਜਾ = Curl_auth_create_oauth_bearer_ਸੁਨੇਹਾ(conn->creds,
                                                     hostname,
                                                     ਪੋਰਟ,
                                                     &resp);

      /* Failures maybe sent by the server as continuations for OAUTHBEARER */
      newstate = SASL_OAUTH2_RESP;
    }
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = Curl_auth_create_xoauth_bearer_ਸੁਨੇਹਾ(conn->creds,
                                                      &resp);
    ਰੋਕੋ;

  ਹਾਲਤ SASL_OAUTH2_RESP:
    /* The continuation is optional so check the response code */
    ਜੇ(ਕੋਡ == sasl->params->finalcode) {
      /* Final response was received so we are done */
      *ਤਰੱਕੀ = SASL_ਪੂਰਾ;
      sasl_state(sasl, ਅੰਕੜੇ, SASL_STOP);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਕੋਡ == sasl->params->contcode) {
      /* Acknowledge the continuation by sending a 0x01 response. */
      Curl_bufref_ਸੈੱਟ(&resp, "\x01", 1, ਨਿਰਾ);
      ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ {
      *ਤਰੱਕੀ = SASL_ਪੂਰਾ;
      sasl_state(sasl, ਅੰਕੜੇ, SASL_STOP);
      ਵਾਪਸ CURLE_LOGIN_DENIED;
    }

  ਹਾਲਤ SASL_CANCEL:
    /* Remove the offending mechanism from the supported list */
    sasl->authmechs &= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)~sasl->authused;
    sasl->authused = SASL_AUTH_NONE;
    sasl->curmech = ਨਿਰਾ;

    /* Start an alternative SASL authentication */
    ਵਾਪਸ Curl_sasl_ਸ਼ੁਰੂ(sasl, ਅੰਕੜੇ, (bool)sasl->force_ir, ਤਰੱਕੀ);
  ਮੂਲ:
    failf(ਅੰਕੜੇ, "Unsupported SASL authentication mechanism");
    ਨਤੀਜਾ = CURLE_UNSUPPORTED_PROTOCOL;  /* Should not happen */
    ਰੋਕੋ;
  }

  Curl_bufref_free(&serverdata);

  ਚੋਣ(ਨਤੀਜਾ) {
  ਹਾਲਤ CURLE_BAD_CONTENT_ENCODING:
    /* Cancel dialog */
    ਨਤੀਜਾ = sasl->params->cancelauth(ਅੰਕੜੇ, sasl->curmech);
    newstate = SASL_CANCEL;
    ਰੋਕੋ;
  ਹਾਲਤ CURLE_OK:
    ਨਤੀਜਾ = build_ਸੁਨੇਹਾ(sasl, &resp);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = sasl->params->contauth(ਅੰਕੜੇ, sasl->curmech, &resp);
    ਰੋਕੋ;
  ਮੂਲ:
    newstate = SASL_STOP;    /* Stop on error */
    *ਤਰੱਕੀ = SASL_ਪੂਰਾ;
    ਰੋਕੋ;
  }

  Curl_bufref_free(&resp);

  sasl_state(sasl, ਅੰਕੜੇ, newstate);

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਸਥਿਰ ਖਾਲੀ sasl_unchosen(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ mech,
                          ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ enabledmechs,
                          bool built_ਵਿੱਚ, bool platform,
                          ਅਚਲ ਅੱਖਰ *param_missing)
{
  ਅਚਲ ਅੱਖਰ *mname = ਨਿਰਾ;
  size_t i;

  ਜੇ(!(enabledmechs & mech))
    ਵਾਪਸ;

  ਲਈ(i = 0; mechtable[i].name; ++i) {
    ਜੇ(mechtable[i].ਦੋਆਧਾਰੀ_ਅੰਕ_2 == mech) {
      mname = mechtable[i].name;
      ਰੋਕੋ;
    }
  }
  ਜੇ(!mname)  /* should not happen */
    ਵਾਪਸ;
  ਜੇ(!built_ਵਿੱਚ)
    infof(ਅੰਕੜੇ, "SASL: %s not builtin", mname);
  ਨਹੀਂਤਾਂ ਜੇ(!platform)
    infof(ਅੰਕੜੇ, "SASL: %s not supported by the platform/libraries", mname);
  ਨਹੀਂਤਾਂ {
    ਜੇ(param_missing)
      infof(ਅੰਕੜੇ, "SASL: %s is missing %s", mname, param_missing);
    ਜੇ(!Curl_creds_has_user(ਅੰਕੜੇ->conn->creds))
      infof(ਅੰਕੜੇ, "SASL: %s is missing username", mname);
  }
}
#ਜੇ_ਅੰਤ /* CURLVERBOSE */

CURLcode Curl_sasl_is_blocked(ਬਣਤਰ SASL *sasl, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_KERBEROS5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SASL_KERBEROS5   TRUE
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SASL_KERBEROS5   FALSE
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GSASL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SASL_GASL        TRUE
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SASL_GASL        FALSE
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SASL_DIGEST      TRUE
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SASL_DIGEST      FALSE
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_NTLM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SASL_NTLM        TRUE
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SASL_NTLM        FALSE
#ਜੇ_ਅੰਤ
  /* Failing SASL authentication is a pain. Give a helping hand if
   * we were unable to select an AUTH mechanism.
   * `sasl->authmechs` are mechanisms offered by the peer
   * `sasl->prefmech`  are mechanisms preferred by us */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ enabledmechs = sasl->authmechs & sasl->prefmech;

  ਜੇ(!sasl->authmechs)
    infof(ਅੰਕੜੇ, "SASL: no auth mechanism was offered or recognized");
  ਨਹੀਂਤਾਂ ਜੇ(!enabledmechs)
    infof(ਅੰਕੜੇ, "SASL: no overlap between offered and configured "
          "auth mechanisms");
  ਨਹੀਂਤਾਂ {
    infof(ਅੰਕੜੇ, "SASL: no auth mechanism offered could be selected");
    ਜੇ((enabledmechs & SASL_MECH_EXTERNAL) &&
       Curl_creds_has_passwd(ਅੰਕੜੇ->conn->creds))
      infof(ਅੰਕੜੇ, "SASL: auth EXTERNAL not chosen with password");
    sasl_unchosen(ਅੰਕੜੇ, SASL_MECH_GSSAPI, enabledmechs,
                  CURL_SASL_KERBEROS5, Curl_auth_is_gssapi_supported(), ਨਿਰਾ);
    sasl_unchosen(ਅੰਕੜੇ, SASL_MECH_SCRAM_SHA_256, enabledmechs,
                  CURL_SASL_GASL, FALSE, ਨਿਰਾ);
    sasl_unchosen(ਅੰਕੜੇ, SASL_MECH_SCRAM_SHA_1, enabledmechs,
                  CURL_SASL_GASL, FALSE, ਨਿਰਾ);
    sasl_unchosen(ਅੰਕੜੇ, SASL_MECH_DIGEST_MD5, enabledmechs,
                  CURL_SASL_DIGEST, Curl_auth_is_digest_supported(), ਨਿਰਾ);
    sasl_unchosen(ਅੰਕੜੇ, SASL_MECH_CRAM_MD5, enabledmechs,
                  CURL_SASL_DIGEST, TRUE, ਨਿਰਾ);
    sasl_unchosen(ਅੰਕੜੇ, SASL_MECH_NTLM, enabledmechs,
                  CURL_SASL_NTLM, Curl_auth_is_ntlm_supported(), ਨਿਰਾ);
    sasl_unchosen(ਅੰਕੜੇ, SASL_MECH_OAUTHBEARER, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(ਅੰਕੜੇ->conn->creds) ?
                  ਨਿਰਾ : "CURLOPT_XOAUTH2_BEARER");
    sasl_unchosen(ਅੰਕੜੇ, SASL_MECH_XOAUTH2, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(ਅੰਕੜੇ->conn->creds) ?
                  ਨਿਰਾ : "CURLOPT_XOAUTH2_BEARER");
  }
#ਜੇ_ਅੰਤ /* CURLVERBOSE */
  (ਖਾਲੀ)sasl;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਵਾਪਸ CURLE_LOGIN_DENIED;
}

#ਜੇ_ਅੰਤ /* protocols are enabled that use SASL */
