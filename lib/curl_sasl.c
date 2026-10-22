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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curlx/base64.h"
#ଅନ୍ତର୍ଭାଵ "vauth/vauth.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "curl_sasl.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"

/* Supported mechanisms */
ସ୍ଥିର ଅଚଳ ସଂରଚନା {
  ଅଚଳ ଅକ୍ଷର    *name;  /* Name */
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
unsigned short Curl_sasl_decode_mech(ଅଚଳ ଅକ୍ଷର *ptr, size_t maxlen,
                                     size_t *len)
{
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ଅକ୍ଷର c;

  ପାଇଁ(i = 0; mechtable[i].name; i++) {
    ଯଦି(maxlen >= mechtable[i].len &&
       curl_strnequal(ptr, mechtable[i].name, mechtable[i].len)) {
      ଯଦି(len)
        *len = mechtable[i].len;

      ଯଦି(maxlen == mechtable[i].len)
        ଫେରନ୍ତୁ mechtable[i].bit;

      c = ptr[mechtable[i].len];
      ଯଦି(!ISUPPER(c) && !ISDIGIT(c) && c != '-' && c != '_')
        ଫେରନ୍ତୁ mechtable[i].bit;
    }
  }

  ଫେରନ୍ତୁ 0;
}

/*
 * Curl_sasl_parse_url_auth_option()
 *
 * Parse the URL login options.
 */
CURLcode Curl_sasl_parse_url_auth_option(ସଂରଚନା SASL *sasl,
                                         ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, size_t len)
{
  CURLcode result = CURLE_OK;
  size_t mechlen;

  ଯଦି(!len)
    ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;

  ଯଦି(sasl->resetprefs) {
    sasl->resetprefs = FALSE;
    sasl->prefmech = SASL_AUTH_NONE;
  }

  ଯଦି(!strncmp(ମୂଲ୍ୟ, "*", len))
    sasl->prefmech = SASL_AUTH_DEFAULT;
  ଅନ୍ୟଥା {
    unsigned short mechbit = Curl_sasl_decode_mech(ମୂଲ୍ୟ, len, &mechlen);
    ଯଦି(mechbit && mechlen == len)
      sasl->prefmech |= mechbit;
    ଅନ୍ୟଥା
      result = CURLE_URL_MALFORMAT;
  }

  ଫେରନ୍ତୁ result;
}

/*
 * Curl_sasl_init()
 *
 * Initializes the SASL structure.
 */
ଶୂନ୍ୟ Curl_sasl_init(ସଂରଚନା SASL *sasl, ସଂରଚନା Curl_easy *data,
                    ଅଚଳ ସଂରଚନା SASLproto *params)
{
  uint32_t auth = data->set.httpauth;

  sasl->params = params;           /* Set protocol dependent parameters */
  sasl->state = SASL_STOP;         /* Not yet running */
  sasl->curmech = ଖାଲି;            /* No mechanism yet. */
  sasl->authmechs = SASL_AUTH_NONE; /* No known authentication mechanism yet */
  sasl->prefmech = params->defmechs; /* Default preferred mechanisms */
  sasl->authused = SASL_AUTH_NONE; /* The authentication mechanism used */
  sasl->resetprefs = TRUE;         /* Reset prefmech upon AUTH parsing. */
  sasl->mutual_auth = FALSE;       /* No mutual authentication (GSSAPI only) */
  sasl->force_ir = FALSE;          /* Respect external option */

  ଯଦି(auth != CURLAUTH_BASIC) {
    unsigned short mechs = SASL_AUTH_NONE;

    /* If some usable http authentication options have been set, determine
       new defaults from them. */
    ଯଦି(auth & CURLAUTH_BASIC)
      mechs |= SASL_MECH_PLAIN | SASL_MECH_LOGIN;
    ଯଦି(auth & CURLAUTH_DIGEST)
      mechs |= SASL_MECH_DIGEST_MD5;
    ଯଦି(auth & CURLAUTH_NTLM)
      mechs |= SASL_MECH_NTLM;
    ଯଦି(auth & CURLAUTH_BEARER)
      mechs |= SASL_MECH_OAUTHBEARER | SASL_MECH_XOAUTH2;
    ଯଦି(auth & CURLAUTH_GSSAPI)
      mechs |= SASL_MECH_GSSAPI;

    ଯଦି(mechs != SASL_AUTH_NONE)
      sasl->prefmech = mechs;
  }
}

/*
 * sasl_state()
 *
 * This is the ONLY way to change SASL state!
 */
ସ୍ଥିର ଶୂନ୍ୟ sasl_state(ସଂରଚନା SASL *sasl, ସଂରଚନା Curl_easy *data,
                       saslstate newstate)
{
#ଯଦି defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ names[] = {
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

  ଯଦି(sasl->state != newstate)
    infof(data, "SASL %p state change from %s to %s",
          (ଶୂନ୍ୟ *)sasl, names[sasl->state], names[newstate]);
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)data;
#endif

  sasl->state = newstate;
}

#ଯଦି defined(USE_NTLM) || defined(USE_GSASL) || defined(USE_KERBEROS5) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)
/* Get the SASL server message and convert it to binary. */
ସ୍ଥିର CURLcode get_server_message(ସଂରଚନା SASL *sasl, ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା bufref *out)
{
  CURLcode result = CURLE_OK;

  result = sasl->params->getmessage(data, out);
  ଯଦି(!result && (sasl->params->flags & SASL_FLAG_BASE64)) {
    ଅଚଳ ଅକ୍ଷର *serverdata = Curl_bufref_ptr(out);

    ଯଦି(!*serverdata)
      Curl_bufref_set(out, ଖାଲି, 0, ଖାଲି);
    ଅନ୍ୟଥା {
      unsigned ଅକ୍ଷର *ସନ୍ଦେଶ;
      size_t msglen;

      result = curlx_base64_decode(serverdata, &ସନ୍ଦେଶ, &msglen);
      ଯଦି(!result)
        Curl_bufref_set(out, ସନ୍ଦେଶ, msglen, curl_free);
    }
  }
  ଫେରନ୍ତୁ result;
}
#endif

/* Encode the outgoing SASL message. */
ସ୍ଥିର CURLcode build_message(ସଂରଚନା SASL *sasl, ସଂରଚନା bufref *ସନ୍ଦେଶ)
{
  CURLcode result = CURLE_OK;

  ଯଦି(sasl->params->flags & SASL_FLAG_BASE64) {
    ଯଦି(!Curl_bufref_ptr(ସନ୍ଦେଶ))                   /* Empty message. */
      Curl_bufref_set(ସନ୍ଦେଶ, "", 0, ଖାଲି);
    ଅନ୍ୟଥା ଯଦି(Curl_bufref_len(ସନ୍ଦେଶ)) {
      ଅକ୍ଷର *base64;
      size_t base64len;

      result = curlx_base64_encode(Curl_bufref_uptr(ସନ୍ଦେଶ),
                                   Curl_bufref_len(ସନ୍ଦେଶ), &base64, &base64len);
      ଯଦି(!result)
        Curl_bufref_set(ସନ୍ଦେଶ, base64, base64len, curl_free);
    }
  }

  ଫେରନ୍ତୁ result;
}

/*
 * Curl_sasl_can_authenticate()
 *
 * Check if we have enough auth data and capabilities to authenticate.
 */
bool Curl_sasl_can_authenticate(ସଂରଚନା SASL *sasl, ସଂରଚନା Curl_easy *data)
{
  /* Have credentials been provided? */
  ଯଦି(data->conn->creds)
    ଫେରନ୍ତୁ TRUE;

  /* EXTERNAL can authenticate without a username and/or password */
  ଯଦି(sasl->authmechs & sasl->prefmech & SASL_MECH_EXTERNAL)
    ଫେରନ୍ତୁ TRUE;

  ଫେରନ୍ତୁ FALSE;
}

ସଂରଚନା sasl_ctx {
  ସଂରଚନା SASL *sasl;
  ସଂରଚନା connectdata *conn;
  unsigned short enabledmechs;
  ଅଚଳ ଅକ୍ଷର *mech;
  saslstate state1;
  saslstate state2;
  ସଂରଚନା bufref resp;
  CURLcode result;
};

ସ୍ଥିର bool sasl_choose_external(ସଂରଚନା Curl_easy *data, ସଂରଚନା sasl_ctx *sctx)
{
  ଯଦି((sctx->enabledmechs & SASL_MECH_EXTERNAL) &&
     !Curl_creds_has_passwd(sctx->conn->creds)) {
    sctx->mech = SASL_MECH_STRING_EXTERNAL;
    sctx->state1 = SASL_EXTERNAL;
    sctx->sasl->authused = SASL_MECH_EXTERNAL;

    ଯଦି(sctx->sasl->force_ir || data->set.sasl_ir)
      Curl_auth_create_external_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}

#ifdef USE_KERBEROS5
ସ୍ଥିର bool sasl_choose_krb5(ସଂରଚନା Curl_easy *data, ସଂରଚନା sasl_ctx *sctx)
{
  ଯଦି((sctx->enabledmechs & SASL_MECH_GSSAPI) &&
     Curl_auth_is_gssapi_supported() &&
     Curl_auth_user_contains_domain(sctx->conn->creds)) {
    ଅଚଳ ଅକ୍ଷର *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;

    sctx->sasl->mutual_auth = FALSE;
    sctx->mech = SASL_MECH_STRING_GSSAPI;
    sctx->state1 = SASL_GSSAPI;
    sctx->state2 = SASL_GSSAPI_TOKEN;
    sctx->sasl->authused = SASL_MECH_GSSAPI;

    ଯଦି(sctx->sasl->force_ir || data->set.sasl_ir) {
      ସଂରଚନା kerberos5data *krb5 = Curl_auth_krb5_get(sctx->conn);
      sctx->result = !krb5 ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_gssapi_user_message(data, sctx->conn->creds,
                                             service,
                                             sctx->conn->origin->hostname,
                                             (bool)sctx->sasl->mutual_auth,
                                             ଖାଲି, krb5, &sctx->resp);
    }
    ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}
#endif /* USE_KERBEROS5 */

#ifdef USE_GSASL
ସ୍ଥିର bool sasl_choose_gsasl(ସଂରଚନା Curl_easy *data, ସଂରଚନା sasl_ctx *sctx)
{
  ସଂରଚନା gsasldata *gsasl;
  ସଂରଚନା bufref nullmsg;

  ଯଦି((sctx->enabledmechs &
      (SASL_MECH_SCRAM_SHA_256 | SASL_MECH_SCRAM_SHA_1))) {
    gsasl = Curl_auth_gsasl_get(sctx->conn);
    ଯଦି(!gsasl) {
      sctx->result = CURLE_OUT_OF_MEMORY;
      ଫେରନ୍ତୁ TRUE; /* attempted, but failed */
    }

    ଯଦି((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_256) &&
       Curl_auth_gsasl_is_supported(data, SASL_MECH_STRING_SCRAM_SHA_256,
                                    gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_256;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_256;
    }
    ଅନ୍ୟଥା ଯଦି((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_1) &&
            Curl_auth_gsasl_is_supported(data, SASL_MECH_STRING_SCRAM_SHA_1,
                                         gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_1;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_1;
    }
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ FALSE;

    Curl_bufref_init(&nullmsg);
    sctx->state1 = SASL_GSASL;
    sctx->state2 = SASL_GSASL;
    sctx->result = Curl_auth_gsasl_start(data, sctx->conn->creds, gsasl);
    ଯଦି(!sctx->result && (sctx->sasl->force_ir || data->set.sasl_ir))
      sctx->result = Curl_auth_gsasl_token(data, &nullmsg, gsasl, &sctx->resp);
    ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}

#endif /* USE_GSASL */

#ifndef CURL_DISABLE_DIGEST_AUTH
ସ୍ଥିର bool sasl_choose_digest(ସଂରଚନା Curl_easy *data, ସଂରଚନା sasl_ctx *sctx)
{
  (ଶୂନ୍ୟ)data;
  ଯଦି((sctx->enabledmechs & SASL_MECH_DIGEST_MD5) &&
     Curl_auth_is_digest_supported()) {
    sctx->mech = SASL_MECH_STRING_DIGEST_MD5;
    sctx->state1 = SASL_DIGESTMD5;
    sctx->sasl->authused = SASL_MECH_DIGEST_MD5;
    ଫେରନ୍ତୁ TRUE;
  }
  ଅନ୍ୟଥା ଯଦି(sctx->enabledmechs & SASL_MECH_CRAM_MD5) {
    sctx->mech = SASL_MECH_STRING_CRAM_MD5;
    sctx->state1 = SASL_CRAMMD5;
    sctx->sasl->authused = SASL_MECH_CRAM_MD5;
    ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}
#endif /* !CURL_DISABLE_DIGEST_AUTH */

#ifdef USE_NTLM
ସ୍ଥିର bool sasl_choose_ntlm(ସଂରଚନା Curl_easy *data, ସଂରଚନା sasl_ctx *sctx)
{
  ଯଦି((sctx->enabledmechs & SASL_MECH_NTLM) &&
     Curl_auth_is_ntlm_supported()) {
     ଅଚଳ ଅକ୍ଷର *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;
    ଅଚଳ ଅକ୍ଷର *hostname;

    Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, ଖାଲି);

    sctx->mech = SASL_MECH_STRING_NTLM;
    sctx->state1 = SASL_NTLM;
    sctx->state2 = SASL_NTLM_TYPE2_ସନ୍ଦେଶ;
    sctx->sasl->authused = SASL_MECH_NTLM;

    ଯଦି(sctx->sasl->force_ir || data->set.sasl_ir) {
      ସଂରଚନା ntlmdata *ntlm = Curl_auth_ntlm_get(sctx->conn, FALSE);
      sctx->result = !ntlm ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_ntlm_type1_message(data, sctx->conn->creds,
                                            service, hostname,
                                            ntlm, &sctx->resp);
    }
    ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}
#endif /* USE_NTLM */

ସ୍ଥିର bool sasl_choose_oauth(ସଂରଚନା Curl_easy *data, ସଂରଚନା sasl_ctx *sctx)
{
  ଯଦି(Curl_creds_has_oauth_bearer(data->state.creds) &&
     (sctx->enabledmechs & SASL_MECH_OAUTHBEARER)) {
    ଅଚଳ ଅକ୍ଷର *hostname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା port;
    Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, &port);

    sctx->mech = SASL_MECH_STRING_OAUTHBEARER;
    sctx->state1 = SASL_OAUTH2;
    sctx->state2 = SASL_OAUTH2_RESP;
    sctx->sasl->authused = SASL_MECH_OAUTHBEARER;

    ଯଦି(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result =
        Curl_auth_create_oauth_bearer_message(sctx->conn->creds,
                                              hostname, port, &sctx->resp);
    ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର bool sasl_choose_oauth2(ସଂରଚନା Curl_easy *data, ସଂରଚନା sasl_ctx *sctx)
{
  ଯଦି(Curl_creds_has_oauth_bearer(sctx->conn->creds) &&
     (sctx->enabledmechs & SASL_MECH_XOAUTH2)) {
    sctx->mech = SASL_MECH_STRING_XOAUTH2;
    sctx->state1 = SASL_OAUTH2;
    sctx->sasl->authused = SASL_MECH_XOAUTH2;

    ଯଦି(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result = Curl_auth_create_xoauth_bearer_message(
        sctx->conn->creds, &sctx->resp);
    ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର bool sasl_choose_plain(ସଂରଚନା Curl_easy *data, ସଂରଚନା sasl_ctx *sctx)
{
  ଯଦି(sctx->enabledmechs & SASL_MECH_PLAIN) {
    sctx->mech = SASL_MECH_STRING_PLAIN;
    sctx->state1 = SASL_PLAIN;
    sctx->sasl->authused = SASL_MECH_PLAIN;

    ଯଦି(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result =
        Curl_auth_create_plain_message(sctx->conn->creds, &sctx->resp);
    ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର bool sasl_choose_login(ସଂରଚନା Curl_easy *data, ସଂରଚନା sasl_ctx *sctx)
{
  ଯଦି(sctx->enabledmechs & SASL_MECH_LOGIN) {
    sctx->mech = SASL_MECH_STRING_LOGIN;
    sctx->state1 = SASL_LOGIN;
    sctx->state2 = SASL_LOGIN_PASSWD;
    sctx->sasl->authused = SASL_MECH_LOGIN;

    ଯଦି(sctx->sasl->force_ir || data->set.sasl_ir)
      Curl_auth_create_login_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}

/*
 * Curl_sasl_start()
 *
 * Calculate the required login details for SASL authentication.
 */
CURLcode Curl_sasl_start(ସଂରଚନା SASL *sasl, ସଂରଚନା Curl_easy *data,
                         bool force_ir, saslprogress *progress)
{
  ସଂରଚନା sasl_ctx sctx;

  sasl->force_ir = force_ir;    /* Latch for future use */
  sasl->authused = 0;           /* No mechanism used yet */
  *progress = SASL_IDLE;

  memset(&sctx, 0, ଆକାର(sctx));
  sctx.sasl = sasl;
  sctx.conn = data->conn;
  Curl_bufref_init(&sctx.resp);
  sctx.enabledmechs = sasl->authmechs & sasl->prefmech;
  sctx.state1 = SASL_STOP;
  sctx.state2 = SASL_FINAL;

  /* Calculate the supported authentication mechanism, by decreasing order of
     security, as well as the initial response where appropriate */
  ଯଦି(sasl_choose_external(data, &sctx) ||
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

  ଯଦି(!sctx.result && sctx.mech) {
    sasl->curmech = sctx.mech;
    ଯଦି(Curl_bufref_ptr(&sctx.resp))
      sctx.result = build_message(sasl, &sctx.resp);

    ଯଦି(sasl->params->maxirlen &&
       strlen(sctx.mech) + Curl_bufref_len(&sctx.resp) >
         sasl->params->maxirlen)
      Curl_bufref_free(&sctx.resp);

    ଯଦି(!sctx.result)
      sctx.result = sasl->params->sendauth(data, sctx.mech, &sctx.resp);

    ଯଦି(!sctx.result) {
      *progress = SASL_INPROGRESS;
      sasl_state(sasl, data, Curl_bufref_ptr(&sctx.resp) ?
                 sctx.state2 : sctx.state1);
    }
  }

  Curl_bufref_free(&sctx.resp);
  ଫେରନ୍ତୁ sctx.result;
}

/*
 * Curl_sasl_continue()
 *
 * Continue the authentication.
 */
CURLcode Curl_sasl_continue(ସଂରଚନା SASL *sasl, ସଂରଚନା Curl_easy *data,
                            ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, saslprogress *progress)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା connectdata *conn = data->conn;
  saslstate newstate = SASL_FINAL;
  ସଂରଚନା bufref resp;
  ଅଚଳ ଅକ୍ଷର *hostname;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା port;
  ସଂରଚନା bufref serverdata;

  Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, &port);
  Curl_bufref_init(&serverdata);
  Curl_bufref_init(&resp);
  *progress = SASL_INPROGRESS;

  ଯଦି(sasl->state == SASL_FINAL) {
    ଯଦି(code != sasl->params->finalcode)
      result = CURLE_LOGIN_DENIED;
    *progress = SASL_DONE;
    sasl_state(sasl, data, SASL_STOP);
    ଫେରନ୍ତୁ result;
  }

  ଯଦି(sasl->state != SASL_CANCEL && sasl->state != SASL_OAUTH2_RESP &&
     code != sasl->params->contcode) {
    *progress = SASL_DONE;
    sasl_state(sasl, data, SASL_STOP);
    ଫେରନ୍ତୁ CURLE_LOGIN_DENIED;
  }

  ବାଛ(sasl->state) {
  ସ୍ଥିତି SASL_STOP:
    *progress = SASL_DONE;
    ଫେରନ୍ତୁ result;
  ସ୍ଥିତି SASL_PLAIN:
    result = Curl_auth_create_plain_message(conn->creds, &resp);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି SASL_LOGIN:
    Curl_auth_create_login_message(Curl_creds_user(conn->creds), &resp);
    newstate = SASL_LOGIN_PASSWD;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି SASL_LOGIN_PASSWD:
    Curl_auth_create_login_message(Curl_creds_passwd(conn->creds), &resp);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି SASL_EXTERNAL:
    Curl_auth_create_external_message(Curl_creds_user(conn->creds), &resp);
    ଭାଙ୍ଗ;
#ifdef USE_GSASL
  ସ୍ଥିତି SASL_GSASL:
    result = get_server_message(sasl, data, &serverdata);
    ଯଦି(!result) {
      ସଂରଚନା gsasldata *gsasl = Curl_auth_gsasl_get(conn);
      result = !gsasl ? CURLE_OUT_OF_MEMORY :
        Curl_auth_gsasl_token(data, &serverdata, gsasl, &resp);
    }
    ଯଦି(!result && Curl_bufref_len(&resp) > 0)
      newstate = SASL_GSASL;
    ଭାଙ୍ଗ;
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  ସ୍ଥିତି SASL_CRAMMD5:
    result = get_server_message(sasl, data, &serverdata);
    ଯଦି(!result)
      result = Curl_auth_create_cram_md5_message(&serverdata, conn->creds,
                                                 &resp);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି SASL_DIGESTMD5:
    result = get_server_message(sasl, data, &serverdata);
    ଯଦି(!result)
      result = Curl_auth_create_digest_md5_message(data, &serverdata,
                                                   conn->creds,
                                                   sasl->params->service,
                                                   &resp);
    ଯଦି(!result && (sasl->params->flags & SASL_FLAG_BASE64))
      newstate = SASL_DIGESTMD5_RESP;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି SASL_DIGESTMD5_RESP:
    /* Keep response NULL to output an empty line. */
    ଭାଙ୍ଗ;
#endif

#ifdef USE_NTLM
  ସ୍ଥିତି SASL_NTLM: {
    /* Create the type-1 message */
    ସଂରଚନା ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    result = !ntlm ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_ntlm_type1_message(data, conn->creds,
                                          sasl->params->service, hostname,
                                          ntlm, &resp);
    newstate = SASL_NTLM_TYPE2_ସନ୍ଦେଶ;
    ଭାଙ୍ଗ;
  }
  ସ୍ଥିତି SASL_NTLM_TYPE2_ସନ୍ଦେଶ: {
    /* Decode the type-2 message */
    ସଂରଚନା ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    result = !ntlm ? CURLE_OUT_OF_MEMORY :
      get_server_message(sasl, data, &serverdata);
    ଯଦି(!result)
      result = Curl_auth_decode_ntlm_type2_message(data, &serverdata, ntlm);
    ଯଦି(!result)
      result = Curl_auth_create_ntlm_type3_message(data, conn->creds,
                                                   ntlm, &resp);
    ଭାଙ୍ଗ;
  }
#endif

#ifdef USE_KERBEROS5
  ସ୍ଥିତି SASL_GSSAPI: {
    ସଂରଚନା kerberos5data *krb5 = Curl_auth_krb5_get(conn);
    result = !krb5 ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_gssapi_user_message(data, conn->creds,
                                           sasl->params->service,
                                           conn->origin->hostname,
                                           (bool)sasl->mutual_auth, ଖାଲି,
                                           krb5, &resp);
    newstate = SASL_GSSAPI_TOKEN;
    ଭାଙ୍ଗ;
  }
  ସ୍ଥିତି SASL_GSSAPI_TOKEN:
    result = get_server_message(sasl, data, &serverdata);
    ଯଦି(!result) {
      ସଂରଚନା kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      ଯଦି(!krb5)
        result = CURLE_OUT_OF_MEMORY;
      ଅନ୍ୟଥା ଯଦି(sasl->mutual_auth) {
        /* Decode the user token challenge and create the optional response
           message */
        result = Curl_auth_create_gssapi_user_message(data, ଖାଲି,
                                                      ଖାଲି, ଖାଲି,
                                                      (bool)sasl->mutual_auth,
                                                      &serverdata,
                                                      krb5, &resp);
        newstate = SASL_GSSAPI_NO_DATA;
      }
      ଅନ୍ୟଥା
        /* Decode the security challenge and create the response message */
        result = Curl_auth_create_gssapi_security_message(
          data, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି SASL_GSSAPI_NO_DATA:
    /* Decode the security challenge and create the response message */
    result = get_server_message(sasl, data, &serverdata);
    ଯଦି(!result) {
      ସଂରଚନା kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      ଯଦି(!krb5)
        result = CURLE_OUT_OF_MEMORY;
      ଅନ୍ୟଥା
        result = Curl_auth_create_gssapi_security_message(
          data, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    ଭାଙ୍ଗ;
#endif

  ସ୍ଥିତି SASL_OAUTH2:
    /* Create the authorization message */
    ଯଦି(sasl->authused == SASL_MECH_OAUTHBEARER) {
      result = Curl_auth_create_oauth_bearer_message(conn->creds,
                                                     hostname,
                                                     port,
                                                     &resp);

      /* Failures maybe sent by the server as continuations for OAUTHBEARER */
      newstate = SASL_OAUTH2_RESP;
    }
    ଅନ୍ୟଥା
      result = Curl_auth_create_xoauth_bearer_message(conn->creds,
                                                      &resp);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି SASL_OAUTH2_RESP:
    /* The continuation is optional so check the response code */
    ଯଦି(code == sasl->params->finalcode) {
      /* Final response was received so we are done */
      *progress = SASL_DONE;
      sasl_state(sasl, data, SASL_STOP);
      ଫେରନ୍ତୁ result;
    }
    ଅନ୍ୟଥା ଯଦି(code == sasl->params->contcode) {
      /* Acknowledge the continuation by sending a 0x01 response. */
      Curl_bufref_set(&resp, "\x01", 1, ଖାଲି);
      ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା {
      *progress = SASL_DONE;
      sasl_state(sasl, data, SASL_STOP);
      ଫେରନ୍ତୁ CURLE_LOGIN_DENIED;
    }

  ସ୍ଥିତି SASL_CANCEL:
    /* Remove the offending mechanism from the supported list */
    sasl->authmechs &= (unsigned short)~sasl->authused;
    sasl->authused = SASL_AUTH_NONE;
    sasl->curmech = ଖାଲି;

    /* Start an alternative SASL authentication */
    ଫେରନ୍ତୁ Curl_sasl_start(sasl, data, (bool)sasl->force_ir, progress);
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    failf(data, "Unsupported SASL authentication mechanism");
    result = CURLE_UNSUPPORTED_PROTOCOL;  /* Should not happen */
    ଭାଙ୍ଗ;
  }

  Curl_bufref_free(&serverdata);

  ବାଛ(result) {
  ସ୍ଥିତି CURLE_BAD_CONTENT_ENCODING:
    /* Cancel dialog */
    result = sasl->params->cancelauth(data, sasl->curmech);
    newstate = SASL_CANCEL;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLE_OK:
    result = build_message(sasl, &resp);
    ଯଦି(!result)
      result = sasl->params->contauth(data, sasl->curmech, &resp);
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    newstate = SASL_STOP;    /* Stop on error */
    *progress = SASL_DONE;
    ଭାଙ୍ଗ;
  }

  Curl_bufref_free(&resp);

  sasl_state(sasl, data, newstate);

  ଫେରନ୍ତୁ result;
}

#ifdef CURLVERBOSE
ସ୍ଥିର ଶୂନ୍ୟ sasl_unchosen(ସଂରଚନା Curl_easy *data, unsigned short mech,
                          unsigned short enabledmechs,
                          bool built_in, bool platform,
                          ଅଚଳ ଅକ୍ଷର *param_missing)
{
  ଅଚଳ ଅକ୍ଷର *mname = ଖାଲି;
  size_t i;

  ଯଦି(!(enabledmechs & mech))
    ଫେରନ୍ତୁ;

  ପାଇଁ(i = 0; mechtable[i].name; ++i) {
    ଯଦି(mechtable[i].bit == mech) {
      mname = mechtable[i].name;
      ଭାଙ୍ଗ;
    }
  }
  ଯଦି(!mname)  /* should not happen */
    ଫେରନ୍ତୁ;
  ଯଦି(!built_in)
    infof(data, "SASL: %s not builtin", mname);
  ଅନ୍ୟଥା ଯଦି(!platform)
    infof(data, "SASL: %s not supported by the platform/libraries", mname);
  ଅନ୍ୟଥା {
    ଯଦି(param_missing)
      infof(data, "SASL: %s is missing %s", mname, param_missing);
    ଯଦି(!Curl_creds_has_user(data->conn->creds))
      infof(data, "SASL: %s is missing username", mname);
  }
}
#endif /* CURLVERBOSE */

CURLcode Curl_sasl_is_blocked(ସଂରଚନା SASL *sasl, ସଂରଚନା Curl_easy *data)
{
#ifdef CURLVERBOSE
#ifdef USE_KERBEROS5
#define CURL_SASL_KERBEROS5   TRUE
#ଅନ୍ୟଥା
#define CURL_SASL_KERBEROS5   FALSE
#endif
#ifdef USE_GSASL
#define CURL_SASL_GASL        TRUE
#ଅନ୍ୟଥା
#define CURL_SASL_GASL        FALSE
#endif
#ifdef CURL_DISABLE_DIGEST_AUTH
#define CURL_SASL_DIGEST      TRUE
#ଅନ୍ୟଥା
#define CURL_SASL_DIGEST      FALSE
#endif
#ifndef USE_NTLM
#define CURL_SASL_NTLM        TRUE
#ଅନ୍ୟଥା
#define CURL_SASL_NTLM        FALSE
#endif
  /* Failing SASL authentication is a pain. Give a helping hand if
   * we were unable to select an AUTH mechanism.
   * `sasl->authmechs` are mechanisms offered by the peer
   * `sasl->prefmech`  are mechanisms preferred by us */
  unsigned short enabledmechs = sasl->authmechs & sasl->prefmech;

  ଯଦି(!sasl->authmechs)
    infof(data, "SASL: no auth mechanism was offered or recognized");
  ଅନ୍ୟଥା ଯଦି(!enabledmechs)
    infof(data, "SASL: no overlap between offered and configured "
          "auth mechanisms");
  ଅନ୍ୟଥା {
    infof(data, "SASL: no auth mechanism offered could be selected");
    ଯଦି((enabledmechs & SASL_MECH_EXTERNAL) &&
       Curl_creds_has_passwd(data->conn->creds))
      infof(data, "SASL: auth EXTERNAL not chosen with password");
    sasl_unchosen(data, SASL_MECH_GSSAPI, enabledmechs,
                  CURL_SASL_KERBEROS5, Curl_auth_is_gssapi_supported(), ଖାଲି);
    sasl_unchosen(data, SASL_MECH_SCRAM_SHA_256, enabledmechs,
                  CURL_SASL_GASL, FALSE, ଖାଲି);
    sasl_unchosen(data, SASL_MECH_SCRAM_SHA_1, enabledmechs,
                  CURL_SASL_GASL, FALSE, ଖାଲି);
    sasl_unchosen(data, SASL_MECH_DIGEST_MD5, enabledmechs,
                  CURL_SASL_DIGEST, Curl_auth_is_digest_supported(), ଖାଲି);
    sasl_unchosen(data, SASL_MECH_CRAM_MD5, enabledmechs,
                  CURL_SASL_DIGEST, TRUE, ଖାଲି);
    sasl_unchosen(data, SASL_MECH_NTLM, enabledmechs,
                  CURL_SASL_NTLM, Curl_auth_is_ntlm_supported(), ଖାଲି);
    sasl_unchosen(data, SASL_MECH_OAUTHBEARER, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(data->conn->creds) ?
                  ଖାଲି : "CURLOPT_XOAUTH2_BEARER");
    sasl_unchosen(data, SASL_MECH_XOAUTH2, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(data->conn->creds) ?
                  ଖାଲି : "CURLOPT_XOAUTH2_BEARER");
  }
#endif /* CURLVERBOSE */
  (ଶୂନ୍ୟ)sasl;
  (ଶୂନ୍ୟ)data;
  ଫେରନ୍ତୁ CURLE_LOGIN_DENIED;
}

#endif /* protocols are enabled that use SASL */
