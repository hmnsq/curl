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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "curl_sasl.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"

/* Supported mechanisms */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ    *name;  /* Name */
  size_t         len;   /* Name length */
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಬಿಟ್_2;   /* Flag bit */
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
ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ Curl_sasl_decode_mech(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr, size_t maxlen,
                                     size_t *len)
{
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
  ಅಕ್ಷರ c;

  ಗಾಗಿ(i = 0; mechtable[i].name; i++) {
    ಆದರೆ(maxlen >= mechtable[i].len &&
       curl_strnequal(ptr, mechtable[i].name, mechtable[i].len)) {
      ಆದರೆ(len)
        *len = mechtable[i].len;

      ಆದರೆ(maxlen == mechtable[i].len)
        ಹಿಂತಿರುಗಿ mechtable[i].ಬಿಟ್_2;

      c = ptr[mechtable[i].len];
      ಆದರೆ(!ISUPPER(c) && !ISDIGIT(c) && c != '-' && c != '_')
        ಹಿಂತಿರುಗಿ mechtable[i].ಬಿಟ್_2;
    }
  }

  ಹಿಂತಿರುಗಿ 0;
}

/*
 * Curl_sasl_parse_url_auth_option()
 *
 * Parse the URL login options.
 */
CURLcode Curl_sasl_parse_url_auth_option(ರಚನೆ SASL *sasl,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, size_t len)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t mechlen;

  ಆದರೆ(!len)
    ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;

  ಆದರೆ(sasl->resetprefs) {
    sasl->resetprefs = FALSE;
    sasl->prefmech = SASL_AUTH_NONE;
  }

  ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(ಮೌಲ್ಯ, "*", len))
    sasl->prefmech = SASL_AUTH_DEFAULT;
  ಇಲ್ಲದಿದ್ದರೆ {
    ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ mechbit = Curl_sasl_decode_mech(ಮೌಲ್ಯ, len, &mechlen);
    ಆದರೆ(mechbit && mechlen == len)
      sasl->prefmech |= mechbit;
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = CURLE_URL_MALFORMAT;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Curl_sasl_init()
 *
 * Initializes the SASL structure.
 */
ಶೂನ್ಯ Curl_sasl_init(ರಚನೆ SASL *sasl, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                    ಸ್ಥಿರಾಂಕ ರಚನೆ SASLproto *params)
{
  uint32_t auth = ದತ್ತಾಂಶ->ಗಣ.httpauth;

  sasl->params = params;           /* Set protocol dependent parameters */
  sasl->state = SASL_STOP;         /* Not yet running */
  sasl->curmech = ಖಾಲಿ;            /* No mechanism yet. */
  sasl->authmechs = SASL_AUTH_NONE; /* No known authentication mechanism yet */
  sasl->prefmech = params->defmechs; /* Default preferred mechanisms */
  sasl->authused = SASL_AUTH_NONE; /* The authentication mechanism used */
  sasl->resetprefs = TRUE;         /* Reset prefmech upon AUTH parsing. */
  sasl->mutual_auth = FALSE;       /* No mutual authentication (GSSAPI only) */
  sasl->force_ir = FALSE;          /* Respect external option */

  ಆದರೆ(auth != CURLAUTH_BASIC) {
    ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ mechs = SASL_AUTH_NONE;

    /* If some usable http authentication options have been set, determine
       new defaults from them. */
    ಆದರೆ(auth & CURLAUTH_BASIC)
      mechs |= SASL_MECH_PLAIN | SASL_MECH_LOGIN;
    ಆದರೆ(auth & CURLAUTH_DIGEST)
      mechs |= SASL_MECH_DIGEST_MD5;
    ಆದರೆ(auth & CURLAUTH_NTLM)
      mechs |= SASL_MECH_NTLM;
    ಆದರೆ(auth & CURLAUTH_BEARER)
      mechs |= SASL_MECH_OAUTHBEARER | SASL_MECH_XOAUTH2;
    ಆದರೆ(auth & CURLAUTH_GSSAPI)
      mechs |= SASL_MECH_GSSAPI;

    ಆದರೆ(mechs != SASL_AUTH_NONE)
      sasl->prefmech = mechs;
  }
}

/*
 * sasl_state()
 *
 * This is the ONLY way to change SASL state!
 */
ಸ್ಥಿರ ಶೂನ್ಯ sasl_state(ರಚನೆ SASL *sasl, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                       saslstate newstate)
{
#ಆದರೆ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ಹೆಸರುಗಳು[] = {
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
    /*ಕೊನೆಯ*/
  };

  ಆದರೆ(sasl->state != newstate)
    infof(ದತ್ತಾಂಶ, "SASL %p state change from %s to %s",
          (ಶೂನ್ಯ *)sasl, ಹೆಸರುಗಳು[sasl->state], ಹೆಸರುಗಳು[newstate]);
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ

  sasl->state = newstate;
}

#ಆದರೆ defined(USE_NTLM) || defined(USE_GSASL) || defined(USE_KERBEROS5) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)
/* Get the SASL server message and convert it to binary. */
ಸ್ಥಿರ CURLcode get_server_ಸಂದೇಶ(ರಚನೆ SASL *sasl, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ bufref *ಹೊರಹೋಗು)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಫಲಿತಾಂಶ = sasl->params->getmessage(ದತ್ತಾಂಶ, ಹೊರಹೋಗು);
  ಆದರೆ(!ಫಲಿತಾಂಶ && (sasl->params->flags & SASL_FLAG_BASE64)) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *serverdata = Curl_bufref_ptr(ಹೊರಹೋಗು);

    ಆದರೆ(!*serverdata)
      Curl_bufref_ಗಣ(ಹೊರಹೋಗು, ಖಾಲಿ, 0, ಖಾಲಿ);
    ಇಲ್ಲದಿದ್ದರೆ {
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಸಂದೇಶ;
      size_t msglen;

      ಫಲಿತಾಂಶ = curlx_base64_decode(serverdata, &ಸಂದೇಶ, &msglen);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        Curl_bufref_ಗಣ(ಹೊರಹೋಗು, ಸಂದೇಶ, msglen, curl_free);
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

/* Encode the outgoing SASL message. */
ಸ್ಥಿರ CURLcode build_ಸಂದೇಶ(ರಚನೆ SASL *sasl, ರಚನೆ bufref *ಸಂದೇಶ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(sasl->params->flags & SASL_FLAG_BASE64) {
    ಆದರೆ(!Curl_bufref_ptr(ಸಂದೇಶ))                   /* Empty message. */
      Curl_bufref_ಗಣ(ಸಂದೇಶ, "", 0, ಖಾಲಿ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(Curl_bufref_len(ಸಂದೇಶ)) {
      ಅಕ್ಷರ *base64;
      size_t base64len;

      ಫಲಿತಾಂಶ = curlx_base64_encode(Curl_bufref_uptr(ಸಂದೇಶ),
                                   Curl_bufref_len(ಸಂದೇಶ), &base64, &base64len);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        Curl_bufref_ಗಣ(ಸಂದೇಶ, base64, base64len, curl_free);
    }
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Curl_sasl_can_authenticate()
 *
 * Check if we have enough auth data and capabilities to authenticate.
 */
bool Curl_sasl_can_authenticate(ರಚನೆ SASL *sasl, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  /* Have credentials been provided? */
  ಆದರೆ(ದತ್ತಾಂಶ->conn->creds)
    ಹಿಂತಿರುಗಿ TRUE;

  /* EXTERNAL can authenticate without a username and/or password */
  ಆದರೆ(sasl->authmechs & sasl->prefmech & SASL_MECH_EXTERNAL)
    ಹಿಂತಿರುಗಿ TRUE;

  ಹಿಂತಿರುಗಿ FALSE;
}

ರಚನೆ sasl_ctx {
  ರಚನೆ SASL *sasl;
  ರಚನೆ connectdata *conn;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ enabledmechs;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mech;
  saslstate state1;
  saslstate state2;
  ರಚನೆ bufref resp;
  CURLcode ಫಲಿತಾಂಶ;
};

ಸ್ಥಿರ bool sasl_ಆರಿಸು_external(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ sasl_ctx *sctx)
{
  ಆದರೆ((sctx->enabledmechs & SASL_MECH_EXTERNAL) &&
     !Curl_creds_has_passwd(sctx->conn->creds)) {
    sctx->mech = SASL_MECH_STRING_EXTERNAL;
    sctx->state1 = SASL_EXTERNAL;
    sctx->sasl->authused = SASL_MECH_EXTERNAL;

    ಆದರೆ(sctx->sasl->force_ir || ದತ್ತಾಂಶ->ಗಣ.sasl_ir)
      Curl_auth_create_external_ಸಂದೇಶ(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_KERBEROS5
ಸ್ಥಿರ bool sasl_ಆರಿಸು_krb5(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ sasl_ctx *sctx)
{
  ಆದರೆ((sctx->enabledmechs & SASL_MECH_GSSAPI) &&
     Curl_auth_is_gssapi_supported() &&
     Curl_auth_user_ಒಳಗೊಂಡಿದೆ_domain(sctx->conn->creds)) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;

    sctx->sasl->mutual_auth = FALSE;
    sctx->mech = SASL_MECH_STRING_GSSAPI;
    sctx->state1 = SASL_GSSAPI;
    sctx->state2 = SASL_GSSAPI_TOKEN;
    sctx->sasl->authused = SASL_MECH_GSSAPI;

    ಆದರೆ(sctx->sasl->force_ir || ದತ್ತಾಂಶ->ಗಣ.sasl_ir) {
      ರಚನೆ kerberos5data *krb5 = Curl_auth_krb5_get(sctx->conn);
      sctx->ಫಲಿತಾಂಶ = !krb5 ? CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ :
        Curl_auth_create_gssapi_user_ಸಂದೇಶ(ದತ್ತಾಂಶ, sctx->conn->creds,
                                             service,
                                             sctx->conn->origin->hostname,
                                             (bool)sctx->sasl->mutual_auth,
                                             ಖಾಲಿ, krb5, &sctx->resp);
    }
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_KERBEROS5 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GSASL
ಸ್ಥಿರ bool sasl_ಆರಿಸು_gsasl(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ sasl_ctx *sctx)
{
  ರಚನೆ gsasldata *gsasl;
  ರಚನೆ bufref nullmsg;

  ಆದರೆ((sctx->enabledmechs &
      (SASL_MECH_SCRAM_SHA_256 | SASL_MECH_SCRAM_SHA_1))) {
    gsasl = Curl_auth_gsasl_get(sctx->conn);
    ಆದರೆ(!gsasl) {
      sctx->ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹಿಂತಿರುಗಿ TRUE; /* attempted, but failed */
    }

    ಆದರೆ((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_256) &&
       Curl_auth_gsasl_is_supported(ದತ್ತಾಂಶ, SASL_MECH_STRING_SCRAM_SHA_256,
                                    gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_256;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_256;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_1) &&
            Curl_auth_gsasl_is_supported(ದತ್ತಾಂಶ, SASL_MECH_STRING_SCRAM_SHA_1,
                                         gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_1;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ FALSE;

    Curl_bufref_init(&nullmsg);
    sctx->state1 = SASL_GSASL;
    sctx->state2 = SASL_GSASL;
    sctx->ಫಲಿತಾಂಶ = Curl_auth_gsasl_ಆರಂಭ(ದತ್ತಾಂಶ, sctx->conn->creds, gsasl);
    ಆದರೆ(!sctx->ಫಲಿತಾಂಶ && (sctx->sasl->force_ir || ದತ್ತಾಂಶ->ಗಣ.sasl_ir))
      sctx->ಫಲಿತಾಂಶ = Curl_auth_gsasl_token(ದತ್ತಾಂಶ, &nullmsg, gsasl, &sctx->resp);
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_GSASL */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH
ಸ್ಥಿರ bool sasl_ಆರಿಸು_digest(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ sasl_ctx *sctx)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ((sctx->enabledmechs & SASL_MECH_DIGEST_MD5) &&
     Curl_auth_is_digest_supported()) {
    sctx->mech = SASL_MECH_STRING_DIGEST_MD5;
    sctx->state1 = SASL_DIGESTMD5;
    sctx->sasl->authused = SASL_MECH_DIGEST_MD5;
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(sctx->enabledmechs & SASL_MECH_CRAM_MD5) {
    sctx->mech = SASL_MECH_STRING_CRAM_MD5;
    sctx->state1 = SASL_CRAMMD5;
    sctx->sasl->authused = SASL_MECH_CRAM_MD5;
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_DIGEST_AUTH */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NTLM
ಸ್ಥಿರ bool sasl_ಆರಿಸು_ntlm(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ sasl_ctx *sctx)
{
  ಆದರೆ((sctx->enabledmechs & SASL_MECH_NTLM) &&
     Curl_auth_is_ntlm_supported()) {
     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname;

    Curl_conn_get_current_host(ದತ್ತಾಂಶ, FIRSTSOCKET, &hostname, ಖಾಲಿ);

    sctx->mech = SASL_MECH_STRING_NTLM;
    sctx->state1 = SASL_NTLM;
    sctx->state2 = SASL_NTLM_TYPE2_ಸಂದೇಶ;
    sctx->sasl->authused = SASL_MECH_NTLM;

    ಆದರೆ(sctx->sasl->force_ir || ದತ್ತಾಂಶ->ಗಣ.sasl_ir) {
      ರಚನೆ ntlmdata *ntlm = Curl_auth_ntlm_get(sctx->conn, FALSE);
      sctx->ಫಲಿತಾಂಶ = !ntlm ? CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ :
        Curl_auth_create_ntlm_type1_ಸಂದೇಶ(ದತ್ತಾಂಶ, sctx->conn->creds,
                                            service, hostname,
                                            ntlm, &sctx->resp);
    }
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_NTLM */

ಸ್ಥಿರ bool sasl_ಆರಿಸು_oauth(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ sasl_ctx *sctx)
{
  ಆದರೆ(Curl_creds_has_oauth_bearer(ದತ್ತಾಂಶ->state.creds) &&
     (sctx->enabledmechs & SASL_MECH_OAUTHBEARER)) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname;
    ಪೂರ್ಣಾಂಕ ಪೋರ್ಟ್;
    Curl_conn_get_current_host(ದತ್ತಾಂಶ, FIRSTSOCKET, &hostname, &ಪೋರ್ಟ್);

    sctx->mech = SASL_MECH_STRING_OAUTHBEARER;
    sctx->state1 = SASL_OAUTH2;
    sctx->state2 = SASL_OAUTH2_RESP;
    sctx->sasl->authused = SASL_MECH_OAUTHBEARER;

    ಆದರೆ(sctx->sasl->force_ir || ದತ್ತಾಂಶ->ಗಣ.sasl_ir)
      sctx->ಫಲಿತಾಂಶ =
        Curl_auth_create_oauth_bearer_ಸಂದೇಶ(sctx->conn->creds,
                                              hostname, ಪೋರ್ಟ್, &sctx->resp);
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ bool sasl_ಆರಿಸು_oauth2(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ sasl_ctx *sctx)
{
  ಆದರೆ(Curl_creds_has_oauth_bearer(sctx->conn->creds) &&
     (sctx->enabledmechs & SASL_MECH_XOAUTH2)) {
    sctx->mech = SASL_MECH_STRING_XOAUTH2;
    sctx->state1 = SASL_OAUTH2;
    sctx->sasl->authused = SASL_MECH_XOAUTH2;

    ಆದರೆ(sctx->sasl->force_ir || ದತ್ತಾಂಶ->ಗಣ.sasl_ir)
      sctx->ಫಲಿತಾಂಶ = Curl_auth_create_xoauth_bearer_ಸಂದೇಶ(
        sctx->conn->creds, &sctx->resp);
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ bool sasl_ಆರಿಸು_plain(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ sasl_ctx *sctx)
{
  ಆದರೆ(sctx->enabledmechs & SASL_MECH_PLAIN) {
    sctx->mech = SASL_MECH_STRING_PLAIN;
    sctx->state1 = SASL_PLAIN;
    sctx->sasl->authused = SASL_MECH_PLAIN;

    ಆದರೆ(sctx->sasl->force_ir || ದತ್ತಾಂಶ->ಗಣ.sasl_ir)
      sctx->ಫಲಿತಾಂಶ =
        Curl_auth_create_plain_ಸಂದೇಶ(sctx->conn->creds, &sctx->resp);
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ bool sasl_ಆರಿಸು_login(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ sasl_ctx *sctx)
{
  ಆದರೆ(sctx->enabledmechs & SASL_MECH_LOGIN) {
    sctx->mech = SASL_MECH_STRING_LOGIN;
    sctx->state1 = SASL_LOGIN;
    sctx->state2 = SASL_LOGIN_PASSWD;
    sctx->sasl->authused = SASL_MECH_LOGIN;

    ಆದರೆ(sctx->sasl->force_ir || ದತ್ತಾಂಶ->ಗಣ.sasl_ir)
      Curl_auth_create_login_ಸಂದೇಶ(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Curl_sasl_start()
 *
 * Calculate the required login details for SASL authentication.
 */
CURLcode Curl_sasl_ಆರಂಭ(ರಚನೆ SASL *sasl, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         bool force_ir, saslprogress *ಪ್ರಗತಿ)
{
  ರಚನೆ sasl_ctx sctx;

  sasl->force_ir = force_ir;    /* Latch for future use */
  sasl->authused = 0;           /* No mechanism used yet */
  *ಪ್ರಗತಿ = SASL_IDLE;

  memset(&sctx, 0, ಗಾತ್ರ(sctx));
  sctx.sasl = sasl;
  sctx.conn = ದತ್ತಾಂಶ->conn;
  Curl_bufref_init(&sctx.resp);
  sctx.enabledmechs = sasl->authmechs & sasl->prefmech;
  sctx.state1 = SASL_STOP;
  sctx.state2 = SASL_FINAL;

  /* Calculate the supported authentication mechanism, by decreasing order of
     security, as well as the initial response where appropriate */
  ಆದರೆ(sasl_ಆರಿಸು_external(ದತ್ತಾಂಶ, &sctx) ||
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_KERBEROS5
     sasl_ಆರಿಸು_krb5(ದತ್ತಾಂಶ, &sctx) ||
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GSASL
     sasl_ಆರಿಸು_gsasl(ದತ್ತಾಂಶ, &sctx) ||
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH
     sasl_ಆರಿಸು_digest(ದತ್ತಾಂಶ, &sctx) ||
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NTLM
     sasl_ಆರಿಸು_ntlm(ದತ್ತಾಂಶ, &sctx) ||
#ಆದರೆ_ಅಂತ್ಯ
     sasl_ಆರಿಸು_oauth(ದತ್ತಾಂಶ, &sctx) ||
     sasl_ಆರಿಸು_oauth2(ದತ್ತಾಂಶ, &sctx) ||
     sasl_ಆರಿಸು_plain(ದತ್ತಾಂಶ, &sctx) ||
     sasl_ಆರಿಸು_login(ದತ್ತಾಂಶ, &sctx)) {
    /* selected, either we have a mechanism or a failure */
    DEBUGASSERT(sctx.mech || sctx.ಫಲಿತಾಂಶ);
  }

  ಆದರೆ(!sctx.ಫಲಿತಾಂಶ && sctx.mech) {
    sasl->curmech = sctx.mech;
    ಆದರೆ(Curl_bufref_ptr(&sctx.resp))
      sctx.ಫಲಿತಾಂಶ = build_ಸಂದೇಶ(sasl, &sctx.resp);

    ಆದರೆ(sasl->params->maxirlen &&
       ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sctx.mech) + Curl_bufref_len(&sctx.resp) >
         sasl->params->maxirlen)
      Curl_bufref_free(&sctx.resp);

    ಆದರೆ(!sctx.ಫಲಿತಾಂಶ)
      sctx.ಫಲಿತಾಂಶ = sasl->params->sendauth(ದತ್ತಾಂಶ, sctx.mech, &sctx.resp);

    ಆದರೆ(!sctx.ಫಲಿತಾಂಶ) {
      *ಪ್ರಗತಿ = SASL_INPROGRESS;
      sasl_state(sasl, ದತ್ತಾಂಶ, Curl_bufref_ptr(&sctx.resp) ?
                 sctx.state2 : sctx.state1);
    }
  }

  Curl_bufref_free(&sctx.resp);
  ಹಿಂತಿರುಗಿ sctx.ಫಲಿತಾಂಶ;
}

/*
 * Curl_sasl_continue()
 *
 * Continue the authentication.
 */
CURLcode Curl_sasl_continue(ರಚನೆ SASL *sasl, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2, saslprogress *ಪ್ರಗತಿ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  saslstate newstate = SASL_FINAL;
  ರಚನೆ bufref resp;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname;
  ಪೂರ್ಣಾಂಕ ಪೋರ್ಟ್;
  ರಚನೆ bufref serverdata;

  Curl_conn_get_current_host(ದತ್ತಾಂಶ, FIRSTSOCKET, &hostname, &ಪೋರ್ಟ್);
  Curl_bufref_init(&serverdata);
  Curl_bufref_init(&resp);
  *ಪ್ರಗತಿ = SASL_INPROGRESS;

  ಆದರೆ(sasl->state == SASL_FINAL) {
    ಆದರೆ(ಸಂಕೇತ_2 != sasl->params->finalcode)
      ಫಲಿತಾಂಶ = CURLE_LOGIN_DENIED;
    *ಪ್ರಗತಿ = SASL_ಮುಗಿದ;
    sasl_state(sasl, ದತ್ತಾಂಶ, SASL_STOP);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(sasl->state != SASL_CANCEL && sasl->state != SASL_OAUTH2_RESP &&
     ಸಂಕೇತ_2 != sasl->params->contcode) {
    *ಪ್ರಗತಿ = SASL_ಮುಗಿದ;
    sasl_state(sasl, ದತ್ತಾಂಶ, SASL_STOP);
    ಹಿಂತಿರುಗಿ CURLE_LOGIN_DENIED;
  }

  ಆಯ್ಕೆ(sasl->state) {
  ಸ್ಥಿತಿ SASL_STOP:
    *ಪ್ರಗತಿ = SASL_ಮುಗಿದ;
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  ಸ್ಥಿತಿ SASL_PLAIN:
    ಫಲಿತಾಂಶ = Curl_auth_create_plain_ಸಂದೇಶ(conn->creds, &resp);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ SASL_LOGIN:
    Curl_auth_create_login_ಸಂದೇಶ(Curl_creds_user(conn->creds), &resp);
    newstate = SASL_LOGIN_PASSWD;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ SASL_LOGIN_PASSWD:
    Curl_auth_create_login_ಸಂದೇಶ(Curl_creds_passwd(conn->creds), &resp);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ SASL_EXTERNAL:
    Curl_auth_create_external_ಸಂದೇಶ(Curl_creds_user(conn->creds), &resp);
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GSASL
  ಸ್ಥಿತಿ SASL_GSASL:
    ಫಲಿತಾಂಶ = get_server_ಸಂದೇಶ(sasl, ದತ್ತಾಂಶ, &serverdata);
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      ರಚನೆ gsasldata *gsasl = Curl_auth_gsasl_get(conn);
      ಫಲಿತಾಂಶ = !gsasl ? CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ :
        Curl_auth_gsasl_token(ದತ್ತಾಂಶ, &serverdata, gsasl, &resp);
    }
    ಆದರೆ(!ಫಲಿತಾಂಶ && Curl_bufref_len(&resp) > 0)
      newstate = SASL_GSASL;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH
  ಸ್ಥಿತಿ SASL_CRAMMD5:
    ಫಲಿತಾಂಶ = get_server_ಸಂದೇಶ(sasl, ದತ್ತಾಂಶ, &serverdata);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = Curl_auth_create_cram_md5_ಸಂದೇಶ(&serverdata, conn->creds,
                                                 &resp);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ SASL_DIGESTMD5:
    ಫಲಿತಾಂಶ = get_server_ಸಂದೇಶ(sasl, ದತ್ತಾಂಶ, &serverdata);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = Curl_auth_create_digest_md5_ಸಂದೇಶ(ದತ್ತಾಂಶ, &serverdata,
                                                   conn->creds,
                                                   sasl->params->service,
                                                   &resp);
    ಆದರೆ(!ಫಲಿತಾಂಶ && (sasl->params->flags & SASL_FLAG_BASE64))
      newstate = SASL_DIGESTMD5_RESP;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ SASL_DIGESTMD5_RESP:
    /* Keep response NULL to output an empty line. */
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NTLM
  ಸ್ಥಿತಿ SASL_NTLM: {
    /* Create the type-1 message */
    ರಚನೆ ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    ಫಲಿತಾಂಶ = !ntlm ? CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ :
      Curl_auth_create_ntlm_type1_ಸಂದೇಶ(ದತ್ತಾಂಶ, conn->creds,
                                          sasl->params->service, hostname,
                                          ntlm, &resp);
    newstate = SASL_NTLM_TYPE2_ಸಂದೇಶ;
    ನಿಲ್ಲಿಸು;
  }
  ಸ್ಥಿತಿ SASL_NTLM_TYPE2_ಸಂದೇಶ: {
    /* Decode the type-2 message */
    ರಚನೆ ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    ಫಲಿತಾಂಶ = !ntlm ? CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ :
      get_server_ಸಂದೇಶ(sasl, ದತ್ತಾಂಶ, &serverdata);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = Curl_auth_decode_ntlm_type2_ಸಂದೇಶ(ದತ್ತಾಂಶ, &serverdata, ntlm);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = Curl_auth_create_ntlm_type3_ಸಂದೇಶ(ದತ್ತಾಂಶ, conn->creds,
                                                   ntlm, &resp);
    ನಿಲ್ಲಿಸು;
  }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_KERBEROS5
  ಸ್ಥಿತಿ SASL_GSSAPI: {
    ರಚನೆ kerberos5data *krb5 = Curl_auth_krb5_get(conn);
    ಫಲಿತಾಂಶ = !krb5 ? CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ :
      Curl_auth_create_gssapi_user_ಸಂದೇಶ(ದತ್ತಾಂಶ, conn->creds,
                                           sasl->params->service,
                                           conn->origin->hostname,
                                           (bool)sasl->mutual_auth, ಖಾಲಿ,
                                           krb5, &resp);
    newstate = SASL_GSSAPI_TOKEN;
    ನಿಲ್ಲಿಸು;
  }
  ಸ್ಥಿತಿ SASL_GSSAPI_TOKEN:
    ಫಲಿತಾಂಶ = get_server_ಸಂದೇಶ(sasl, ದತ್ತಾಂಶ, &serverdata);
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      ರಚನೆ kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      ಆದರೆ(!krb5)
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(sasl->mutual_auth) {
        /* Decode the user token challenge and create the optional response
ಸಂದೇಶ*/
        ಫಲಿತಾಂಶ = Curl_auth_create_gssapi_user_ಸಂದೇಶ(ದತ್ತಾಂಶ, ಖಾಲಿ,
                                                      ಖಾಲಿ, ಖಾಲಿ,
                                                      (bool)sasl->mutual_auth,
                                                      &serverdata,
                                                      krb5, &resp);
        newstate = SASL_GSSAPI_NO_ದತ್ತಾಂಶ;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        /* Decode the security challenge and create the response message */
        ಫಲಿತಾಂಶ = Curl_auth_create_gssapi_security_ಸಂದೇಶ(
          ದತ್ತಾಂಶ, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ SASL_GSSAPI_NO_ದತ್ತಾಂಶ:
    /* Decode the security challenge and create the response message */
    ಫಲಿತಾಂಶ = get_server_ಸಂದೇಶ(sasl, ದತ್ತಾಂಶ, &serverdata);
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      ರಚನೆ kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      ಆದರೆ(!krb5)
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಇಲ್ಲದಿದ್ದರೆ
        ಫಲಿತಾಂಶ = Curl_auth_create_gssapi_security_ಸಂದೇಶ(
          ದತ್ತಾಂಶ, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

  ಸ್ಥಿತಿ SASL_OAUTH2:
    /* Create the authorization message */
    ಆದರೆ(sasl->authused == SASL_MECH_OAUTHBEARER) {
      ಫಲಿತಾಂಶ = Curl_auth_create_oauth_bearer_ಸಂದೇಶ(conn->creds,
                                                     hostname,
                                                     ಪೋರ್ಟ್,
                                                     &resp);

      /* Failures maybe sent by the server as continuations for OAUTHBEARER */
      newstate = SASL_OAUTH2_RESP;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = Curl_auth_create_xoauth_bearer_ಸಂದೇಶ(conn->creds,
                                                      &resp);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ SASL_OAUTH2_RESP:
    /* The continuation is optional so check the response code */
    ಆದರೆ(ಸಂಕೇತ_2 == sasl->params->finalcode) {
      /* Final response was received so we are done */
      *ಪ್ರಗತಿ = SASL_ಮುಗಿದ;
      sasl_state(sasl, ದತ್ತಾಂಶ, SASL_STOP);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಸಂಕೇತ_2 == sasl->params->contcode) {
      /* Acknowledge the continuation by sending a 0x01 response. */
      Curl_bufref_ಗಣ(&resp, "\x01", 1, ಖಾಲಿ);
      ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      *ಪ್ರಗತಿ = SASL_ಮುಗಿದ;
      sasl_state(sasl, ದತ್ತಾಂಶ, SASL_STOP);
      ಹಿಂತಿರುಗಿ CURLE_LOGIN_DENIED;
    }

  ಸ್ಥಿತಿ SASL_CANCEL:
    /* Remove the offending mechanism from the supported list */
    sasl->authmechs &= (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)~sasl->authused;
    sasl->authused = SASL_AUTH_NONE;
    sasl->curmech = ಖಾಲಿ;

    /* Start an alternative SASL authentication */
    ಹಿಂತಿರುಗಿ Curl_sasl_ಆರಂಭ(sasl, ದತ್ತಾಂಶ, (bool)sasl->force_ir, ಪ್ರಗತಿ);
  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(ದತ್ತಾಂಶ, "Unsupported SASL authentication mechanism");
    ಫಲಿತಾಂಶ = CURLE_UNSUPPORTED_PROTOCOL;  /* Should not happen */
    ನಿಲ್ಲಿಸು;
  }

  Curl_bufref_free(&serverdata);

  ಆಯ್ಕೆ(ಫಲಿತಾಂಶ) {
  ಸ್ಥಿತಿ CURLE_BAD_CONTENT_ENCODING:
    /* Cancel dialog */
    ಫಲಿತಾಂಶ = sasl->params->cancelauth(ದತ್ತಾಂಶ, sasl->curmech);
    newstate = SASL_CANCEL;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLE_OK:
    ಫಲಿತಾಂಶ = build_ಸಂದೇಶ(sasl, &resp);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = sasl->params->contauth(ದತ್ತಾಂಶ, sasl->curmech, &resp);
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    newstate = SASL_STOP;    /* Stop on error */
    *ಪ್ರಗತಿ = SASL_ಮುಗಿದ;
    ನಿಲ್ಲಿಸು;
  }

  Curl_bufref_free(&resp);

  sasl_state(sasl, ದತ್ತಾಂಶ, newstate);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಸ್ಥಿರ ಶೂನ್ಯ sasl_unchosen(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ mech,
                          ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ enabledmechs,
                          bool built_ಒಳ, bool platform,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *param_missing)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mname = ಖಾಲಿ;
  size_t i;

  ಆದರೆ(!(enabledmechs & mech))
    ಹಿಂತಿರುಗಿ;

  ಗಾಗಿ(i = 0; mechtable[i].name; ++i) {
    ಆದರೆ(mechtable[i].ಬಿಟ್_2 == mech) {
      mname = mechtable[i].name;
      ನಿಲ್ಲಿಸು;
    }
  }
  ಆದರೆ(!mname)  /* should not happen */
    ಹಿಂತಿರುಗಿ;
  ಆದರೆ(!built_ಒಳ)
    infof(ದತ್ತಾಂಶ, "SASL: %s not builtin", mname);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!platform)
    infof(ದತ್ತಾಂಶ, "SASL: %s not supported by the platform/libraries", mname);
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(param_missing)
      infof(ದತ್ತಾಂಶ, "SASL: %s is missing %s", mname, param_missing);
    ಆದರೆ(!Curl_creds_has_user(ದತ್ತಾಂಶ->conn->creds))
      infof(ದತ್ತಾಂಶ, "SASL: %s is missing username", mname);
  }
}
#ಆದರೆ_ಅಂತ್ಯ /* CURLVERBOSE */

CURLcode Curl_sasl_is_blocked(ರಚನೆ SASL *sasl, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_KERBEROS5
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SASL_KERBEROS5   TRUE
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SASL_KERBEROS5   FALSE
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GSASL
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SASL_GASL        TRUE
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SASL_GASL        FALSE
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_DIGEST_AUTH
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SASL_DIGEST      TRUE
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SASL_DIGEST      FALSE
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_NTLM
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SASL_NTLM        TRUE
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SASL_NTLM        FALSE
#ಆದರೆ_ಅಂತ್ಯ
  /* Failing SASL authentication is a pain. Give a helping hand if
   * we were unable to select an AUTH mechanism.
   * `sasl->authmechs` are mechanisms offered by the peer
   * `sasl->prefmech`  are mechanisms preferred by us */
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ enabledmechs = sasl->authmechs & sasl->prefmech;

  ಆದರೆ(!sasl->authmechs)
    infof(ದತ್ತಾಂಶ, "SASL: no auth mechanism was offered or recognized");
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!enabledmechs)
    infof(ದತ್ತಾಂಶ, "SASL: no overlap between offered and configured "
          "auth mechanisms");
  ಇಲ್ಲದಿದ್ದರೆ {
    infof(ದತ್ತಾಂಶ, "SASL: no auth mechanism offered could be selected");
    ಆದರೆ((enabledmechs & SASL_MECH_EXTERNAL) &&
       Curl_creds_has_passwd(ದತ್ತಾಂಶ->conn->creds))
      infof(ದತ್ತಾಂಶ, "SASL: auth EXTERNAL not chosen with password");
    sasl_unchosen(ದತ್ತಾಂಶ, SASL_MECH_GSSAPI, enabledmechs,
                  CURL_SASL_KERBEROS5, Curl_auth_is_gssapi_supported(), ಖಾಲಿ);
    sasl_unchosen(ದತ್ತಾಂಶ, SASL_MECH_SCRAM_SHA_256, enabledmechs,
                  CURL_SASL_GASL, FALSE, ಖಾಲಿ);
    sasl_unchosen(ದತ್ತಾಂಶ, SASL_MECH_SCRAM_SHA_1, enabledmechs,
                  CURL_SASL_GASL, FALSE, ಖಾಲಿ);
    sasl_unchosen(ದತ್ತಾಂಶ, SASL_MECH_DIGEST_MD5, enabledmechs,
                  CURL_SASL_DIGEST, Curl_auth_is_digest_supported(), ಖಾಲಿ);
    sasl_unchosen(ದತ್ತಾಂಶ, SASL_MECH_CRAM_MD5, enabledmechs,
                  CURL_SASL_DIGEST, TRUE, ಖಾಲಿ);
    sasl_unchosen(ದತ್ತಾಂಶ, SASL_MECH_NTLM, enabledmechs,
                  CURL_SASL_NTLM, Curl_auth_is_ntlm_supported(), ಖಾಲಿ);
    sasl_unchosen(ದತ್ತಾಂಶ, SASL_MECH_OAUTHBEARER, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(ದತ್ತಾಂಶ->conn->creds) ?
                  ಖಾಲಿ : "CURLOPT_XOAUTH2_BEARER");
    sasl_unchosen(ದತ್ತಾಂಶ, SASL_MECH_XOAUTH2, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(ದತ್ತಾಂಶ->conn->creds) ?
                  ಖಾಲಿ : "CURLOPT_XOAUTH2_BEARER");
  }
#ಆದರೆ_ಅಂತ್ಯ /* CURLVERBOSE */
  (ಶೂನ್ಯ)sasl;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ CURLE_LOGIN_DENIED;
}

#ಆದರೆ_ಅಂತ್ಯ /* protocols are enabled that use SASL */
