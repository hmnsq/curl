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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curlx/base64.h"
#அந்தர்பாவ "vauth/vauth.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "curl_sasl.h"
#அந்தர்பாவ "curl_trc.h"

/* Supported mechanisms */
நிலையான மாறிலி கட்டமைப்பு {
  மாறிலி எழுத்து    *name;  /* Name */
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
unsigned short Curl_sasl_decode_mech(மாறிலி எழுத்து *ptr, size_t maxlen,
                                     size_t *len)
{
  unsigned முழு i;
  எழுத்து c;

  க்கு(i = 0; mechtable[i].name; i++) {
    எனில்(maxlen >= mechtable[i].len &&
       curl_strnequal(ptr, mechtable[i].name, mechtable[i].len)) {
      எனில்(len)
        *len = mechtable[i].len;

      எனில்(maxlen == mechtable[i].len)
        திரும்பு mechtable[i].bit;

      c = ptr[mechtable[i].len];
      எனில்(!ISUPPER(c) && !ISDIGIT(c) && c != '-' && c != '_')
        திரும்பு mechtable[i].bit;
    }
  }

  திரும்பு 0;
}

/*
 * Curl_sasl_parse_url_auth_option()
 *
 * Parse the URL login options.
 */
CURLcode Curl_sasl_parse_url_auth_option(கட்டமைப்பு SASL *sasl,
                                         மாறிலி எழுத்து *மதிப்பு, size_t len)
{
  CURLcode result = CURLE_OK;
  size_t mechlen;

  எனில்(!len)
    திரும்பு CURLE_URL_MALFORMAT;

  எனில்(sasl->resetprefs) {
    sasl->resetprefs = FALSE;
    sasl->prefmech = SASL_AUTH_NONE;
  }

  எனில்(!strncmp(மதிப்பு, "*", len))
    sasl->prefmech = SASL_AUTH_DEFAULT;
  இல்லையெனில் {
    unsigned short mechbit = Curl_sasl_decode_mech(மதிப்பு, len, &mechlen);
    எனில்(mechbit && mechlen == len)
      sasl->prefmech |= mechbit;
    இல்லையெனில்
      result = CURLE_URL_MALFORMAT;
  }

  திரும்பு result;
}

/*
 * Curl_sasl_init()
 *
 * Initializes the SASL structure.
 */
வெறுமை Curl_sasl_init(கட்டமைப்பு SASL *sasl, கட்டமைப்பு Curl_easy *data,
                    மாறிலி கட்டமைப்பு SASLproto *params)
{
  uint32_t auth = data->set.httpauth;

  sasl->params = params;           /* Set protocol dependent parameters */
  sasl->state = SASL_STOP;         /* Not yet running */
  sasl->curmech = வெற்று;            /* No mechanism yet. */
  sasl->authmechs = SASL_AUTH_NONE; /* No known authentication mechanism yet */
  sasl->prefmech = params->defmechs; /* Default preferred mechanisms */
  sasl->authused = SASL_AUTH_NONE; /* The authentication mechanism used */
  sasl->resetprefs = TRUE;         /* Reset prefmech upon AUTH parsing. */
  sasl->mutual_auth = FALSE;       /* No mutual authentication (GSSAPI only) */
  sasl->force_ir = FALSE;          /* Respect external option */

  எனில்(auth != CURLAUTH_BASIC) {
    unsigned short mechs = SASL_AUTH_NONE;

    /* If some usable http authentication options have been set, determine
       new defaults from them. */
    எனில்(auth & CURLAUTH_BASIC)
      mechs |= SASL_MECH_PLAIN | SASL_MECH_LOGIN;
    எனில்(auth & CURLAUTH_DIGEST)
      mechs |= SASL_MECH_DIGEST_MD5;
    எனில்(auth & CURLAUTH_NTLM)
      mechs |= SASL_MECH_NTLM;
    எனில்(auth & CURLAUTH_BEARER)
      mechs |= SASL_MECH_OAUTHBEARER | SASL_MECH_XOAUTH2;
    எனில்(auth & CURLAUTH_GSSAPI)
      mechs |= SASL_MECH_GSSAPI;

    எனில்(mechs != SASL_AUTH_NONE)
      sasl->prefmech = mechs;
  }
}

/*
 * sasl_state()
 *
 * This is the ONLY way to change SASL state!
 */
நிலையான வெறுமை sasl_state(கட்டமைப்பு SASL *sasl, கட்டமைப்பு Curl_easy *data,
                       saslstate newstate)
{
#எனில் defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  நிலையான மாறிலி எழுத்து * மாறிலி names[] = {
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

  எனில்(sasl->state != newstate)
    infof(data, "SASL %p state change from %s to %s",
          (வெறுமை *)sasl, names[sasl->state], names[newstate]);
#இல்லையெனில்
  (வெறுமை)data;
#endif

  sasl->state = newstate;
}

#எனில் defined(USE_NTLM) || defined(USE_GSASL) || defined(USE_KERBEROS5) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)
/* Get the SASL server message and convert it to binary. */
நிலையான CURLcode get_server_message(கட்டமைப்பு SASL *sasl, கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு bufref *out)
{
  CURLcode result = CURLE_OK;

  result = sasl->params->getmessage(data, out);
  எனில்(!result && (sasl->params->flags & SASL_FLAG_BASE64)) {
    மாறிலி எழுத்து *serverdata = Curl_bufref_ptr(out);

    எனில்(!*serverdata)
      Curl_bufref_set(out, வெற்று, 0, வெற்று);
    இல்லையெனில் {
      unsigned எழுத்து *செய்தி;
      size_t msglen;

      result = curlx_base64_decode(serverdata, &செய்தி, &msglen);
      எனில்(!result)
        Curl_bufref_set(out, செய்தி, msglen, curl_free);
    }
  }
  திரும்பு result;
}
#endif

/* Encode the outgoing SASL message. */
நிலையான CURLcode build_message(கட்டமைப்பு SASL *sasl, கட்டமைப்பு bufref *செய்தி)
{
  CURLcode result = CURLE_OK;

  எனில்(sasl->params->flags & SASL_FLAG_BASE64) {
    எனில்(!Curl_bufref_ptr(செய்தி))                   /* Empty message. */
      Curl_bufref_set(செய்தி, "", 0, வெற்று);
    இல்லையெனில் எனில்(Curl_bufref_len(செய்தி)) {
      எழுத்து *base64;
      size_t base64len;

      result = curlx_base64_encode(Curl_bufref_uptr(செய்தி),
                                   Curl_bufref_len(செய்தி), &base64, &base64len);
      எனில்(!result)
        Curl_bufref_set(செய்தி, base64, base64len, curl_free);
    }
  }

  திரும்பு result;
}

/*
 * Curl_sasl_can_authenticate()
 *
 * Check if we have enough auth data and capabilities to authenticate.
 */
bool Curl_sasl_can_authenticate(கட்டமைப்பு SASL *sasl, கட்டமைப்பு Curl_easy *data)
{
  /* Have credentials been provided? */
  எனில்(data->conn->creds)
    திரும்பு TRUE;

  /* EXTERNAL can authenticate without a username and/or password */
  எனில்(sasl->authmechs & sasl->prefmech & SASL_MECH_EXTERNAL)
    திரும்பு TRUE;

  திரும்பு FALSE;
}

கட்டமைப்பு sasl_ctx {
  கட்டமைப்பு SASL *sasl;
  கட்டமைப்பு connectdata *conn;
  unsigned short enabledmechs;
  மாறிலி எழுத்து *mech;
  saslstate state1;
  saslstate state2;
  கட்டமைப்பு bufref resp;
  CURLcode result;
};

நிலையான bool sasl_choose_external(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு sasl_ctx *sctx)
{
  எனில்((sctx->enabledmechs & SASL_MECH_EXTERNAL) &&
     !Curl_creds_has_passwd(sctx->conn->creds)) {
    sctx->mech = SASL_MECH_STRING_EXTERNAL;
    sctx->state1 = SASL_EXTERNAL;
    sctx->sasl->authused = SASL_MECH_EXTERNAL;

    எனில்(sctx->sasl->force_ir || data->set.sasl_ir)
      Curl_auth_create_external_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    திரும்பு TRUE;
  }
  திரும்பு FALSE;
}

#ifdef USE_KERBEROS5
நிலையான bool sasl_choose_krb5(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு sasl_ctx *sctx)
{
  எனில்((sctx->enabledmechs & SASL_MECH_GSSAPI) &&
     Curl_auth_is_gssapi_supported() &&
     Curl_auth_user_contains_domain(sctx->conn->creds)) {
    மாறிலி எழுத்து *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;

    sctx->sasl->mutual_auth = FALSE;
    sctx->mech = SASL_MECH_STRING_GSSAPI;
    sctx->state1 = SASL_GSSAPI;
    sctx->state2 = SASL_GSSAPI_TOKEN;
    sctx->sasl->authused = SASL_MECH_GSSAPI;

    எனில்(sctx->sasl->force_ir || data->set.sasl_ir) {
      கட்டமைப்பு kerberos5data *krb5 = Curl_auth_krb5_get(sctx->conn);
      sctx->result = !krb5 ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_gssapi_user_message(data, sctx->conn->creds,
                                             service,
                                             sctx->conn->origin->hostname,
                                             (bool)sctx->sasl->mutual_auth,
                                             வெற்று, krb5, &sctx->resp);
    }
    திரும்பு TRUE;
  }
  திரும்பு FALSE;
}
#endif /* USE_KERBEROS5 */

#ifdef USE_GSASL
நிலையான bool sasl_choose_gsasl(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு sasl_ctx *sctx)
{
  கட்டமைப்பு gsasldata *gsasl;
  கட்டமைப்பு bufref nullmsg;

  எனில்((sctx->enabledmechs &
      (SASL_MECH_SCRAM_SHA_256 | SASL_MECH_SCRAM_SHA_1))) {
    gsasl = Curl_auth_gsasl_get(sctx->conn);
    எனில்(!gsasl) {
      sctx->result = CURLE_OUT_OF_MEMORY;
      திரும்பு TRUE; /* attempted, but failed */
    }

    எனில்((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_256) &&
       Curl_auth_gsasl_is_supported(data, SASL_MECH_STRING_SCRAM_SHA_256,
                                    gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_256;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_256;
    }
    இல்லையெனில் எனில்((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_1) &&
            Curl_auth_gsasl_is_supported(data, SASL_MECH_STRING_SCRAM_SHA_1,
                                         gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_1;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_1;
    }
    இல்லையெனில்
      திரும்பு FALSE;

    Curl_bufref_init(&nullmsg);
    sctx->state1 = SASL_GSASL;
    sctx->state2 = SASL_GSASL;
    sctx->result = Curl_auth_gsasl_start(data, sctx->conn->creds, gsasl);
    எனில்(!sctx->result && (sctx->sasl->force_ir || data->set.sasl_ir))
      sctx->result = Curl_auth_gsasl_token(data, &nullmsg, gsasl, &sctx->resp);
    திரும்பு TRUE;
  }
  திரும்பு FALSE;
}

#endif /* USE_GSASL */

#ifndef CURL_DISABLE_DIGEST_AUTH
நிலையான bool sasl_choose_digest(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு sasl_ctx *sctx)
{
  (வெறுமை)data;
  எனில்((sctx->enabledmechs & SASL_MECH_DIGEST_MD5) &&
     Curl_auth_is_digest_supported()) {
    sctx->mech = SASL_MECH_STRING_DIGEST_MD5;
    sctx->state1 = SASL_DIGESTMD5;
    sctx->sasl->authused = SASL_MECH_DIGEST_MD5;
    திரும்பு TRUE;
  }
  இல்லையெனில் எனில்(sctx->enabledmechs & SASL_MECH_CRAM_MD5) {
    sctx->mech = SASL_MECH_STRING_CRAM_MD5;
    sctx->state1 = SASL_CRAMMD5;
    sctx->sasl->authused = SASL_MECH_CRAM_MD5;
    திரும்பு TRUE;
  }
  திரும்பு FALSE;
}
#endif /* !CURL_DISABLE_DIGEST_AUTH */

#ifdef USE_NTLM
நிலையான bool sasl_choose_ntlm(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு sasl_ctx *sctx)
{
  எனில்((sctx->enabledmechs & SASL_MECH_NTLM) &&
     Curl_auth_is_ntlm_supported()) {
     மாறிலி எழுத்து *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;
    மாறிலி எழுத்து *hostname;

    Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, வெற்று);

    sctx->mech = SASL_MECH_STRING_NTLM;
    sctx->state1 = SASL_NTLM;
    sctx->state2 = SASL_NTLM_TYPE2_செய்தி;
    sctx->sasl->authused = SASL_MECH_NTLM;

    எனில்(sctx->sasl->force_ir || data->set.sasl_ir) {
      கட்டமைப்பு ntlmdata *ntlm = Curl_auth_ntlm_get(sctx->conn, FALSE);
      sctx->result = !ntlm ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_ntlm_type1_message(data, sctx->conn->creds,
                                            service, hostname,
                                            ntlm, &sctx->resp);
    }
    திரும்பு TRUE;
  }
  திரும்பு FALSE;
}
#endif /* USE_NTLM */

நிலையான bool sasl_choose_oauth(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு sasl_ctx *sctx)
{
  எனில்(Curl_creds_has_oauth_bearer(data->state.creds) &&
     (sctx->enabledmechs & SASL_MECH_OAUTHBEARER)) {
    மாறிலி எழுத்து *hostname;
    முழு port;
    Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, &port);

    sctx->mech = SASL_MECH_STRING_OAUTHBEARER;
    sctx->state1 = SASL_OAUTH2;
    sctx->state2 = SASL_OAUTH2_RESP;
    sctx->sasl->authused = SASL_MECH_OAUTHBEARER;

    எனில்(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result =
        Curl_auth_create_oauth_bearer_message(sctx->conn->creds,
                                              hostname, port, &sctx->resp);
    திரும்பு TRUE;
  }
  திரும்பு FALSE;
}

நிலையான bool sasl_choose_oauth2(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு sasl_ctx *sctx)
{
  எனில்(Curl_creds_has_oauth_bearer(sctx->conn->creds) &&
     (sctx->enabledmechs & SASL_MECH_XOAUTH2)) {
    sctx->mech = SASL_MECH_STRING_XOAUTH2;
    sctx->state1 = SASL_OAUTH2;
    sctx->sasl->authused = SASL_MECH_XOAUTH2;

    எனில்(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result = Curl_auth_create_xoauth_bearer_message(
        sctx->conn->creds, &sctx->resp);
    திரும்பு TRUE;
  }
  திரும்பு FALSE;
}

நிலையான bool sasl_choose_plain(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு sasl_ctx *sctx)
{
  எனில்(sctx->enabledmechs & SASL_MECH_PLAIN) {
    sctx->mech = SASL_MECH_STRING_PLAIN;
    sctx->state1 = SASL_PLAIN;
    sctx->sasl->authused = SASL_MECH_PLAIN;

    எனில்(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result =
        Curl_auth_create_plain_message(sctx->conn->creds, &sctx->resp);
    திரும்பு TRUE;
  }
  திரும்பு FALSE;
}

நிலையான bool sasl_choose_login(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு sasl_ctx *sctx)
{
  எனில்(sctx->enabledmechs & SASL_MECH_LOGIN) {
    sctx->mech = SASL_MECH_STRING_LOGIN;
    sctx->state1 = SASL_LOGIN;
    sctx->state2 = SASL_LOGIN_PASSWD;
    sctx->sasl->authused = SASL_MECH_LOGIN;

    எனில்(sctx->sasl->force_ir || data->set.sasl_ir)
      Curl_auth_create_login_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    திரும்பு TRUE;
  }
  திரும்பு FALSE;
}

/*
 * Curl_sasl_start()
 *
 * Calculate the required login details for SASL authentication.
 */
CURLcode Curl_sasl_start(கட்டமைப்பு SASL *sasl, கட்டமைப்பு Curl_easy *data,
                         bool force_ir, saslprogress *progress)
{
  கட்டமைப்பு sasl_ctx sctx;

  sasl->force_ir = force_ir;    /* Latch for future use */
  sasl->authused = 0;           /* No mechanism used yet */
  *progress = SASL_IDLE;

  memset(&sctx, 0, அளவு(sctx));
  sctx.sasl = sasl;
  sctx.conn = data->conn;
  Curl_bufref_init(&sctx.resp);
  sctx.enabledmechs = sasl->authmechs & sasl->prefmech;
  sctx.state1 = SASL_STOP;
  sctx.state2 = SASL_FINAL;

  /* Calculate the supported authentication mechanism, by decreasing order of
     security, as well as the initial response where appropriate */
  எனில்(sasl_choose_external(data, &sctx) ||
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

  எனில்(!sctx.result && sctx.mech) {
    sasl->curmech = sctx.mech;
    எனில்(Curl_bufref_ptr(&sctx.resp))
      sctx.result = build_message(sasl, &sctx.resp);

    எனில்(sasl->params->maxirlen &&
       strlen(sctx.mech) + Curl_bufref_len(&sctx.resp) >
         sasl->params->maxirlen)
      Curl_bufref_free(&sctx.resp);

    எனில்(!sctx.result)
      sctx.result = sasl->params->sendauth(data, sctx.mech, &sctx.resp);

    எனில்(!sctx.result) {
      *progress = SASL_INPROGRESS;
      sasl_state(sasl, data, Curl_bufref_ptr(&sctx.resp) ?
                 sctx.state2 : sctx.state1);
    }
  }

  Curl_bufref_free(&sctx.resp);
  திரும்பு sctx.result;
}

/*
 * Curl_sasl_continue()
 *
 * Continue the authentication.
 */
CURLcode Curl_sasl_continue(கட்டமைப்பு SASL *sasl, கட்டமைப்பு Curl_easy *data,
                            முழு code, saslprogress *progress)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;
  saslstate newstate = SASL_FINAL;
  கட்டமைப்பு bufref resp;
  மாறிலி எழுத்து *hostname;
  முழு port;
  கட்டமைப்பு bufref serverdata;

  Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, &port);
  Curl_bufref_init(&serverdata);
  Curl_bufref_init(&resp);
  *progress = SASL_INPROGRESS;

  எனில்(sasl->state == SASL_FINAL) {
    எனில்(code != sasl->params->finalcode)
      result = CURLE_LOGIN_DENIED;
    *progress = SASL_DONE;
    sasl_state(sasl, data, SASL_STOP);
    திரும்பு result;
  }

  எனில்(sasl->state != SASL_CANCEL && sasl->state != SASL_OAUTH2_RESP &&
     code != sasl->params->contcode) {
    *progress = SASL_DONE;
    sasl_state(sasl, data, SASL_STOP);
    திரும்பு CURLE_LOGIN_DENIED;
  }

  தேர்வு(sasl->state) {
  நிலை SASL_STOP:
    *progress = SASL_DONE;
    திரும்பு result;
  நிலை SASL_PLAIN:
    result = Curl_auth_create_plain_message(conn->creds, &resp);
    நிறுத்து;
  நிலை SASL_LOGIN:
    Curl_auth_create_login_message(Curl_creds_user(conn->creds), &resp);
    newstate = SASL_LOGIN_PASSWD;
    நிறுத்து;
  நிலை SASL_LOGIN_PASSWD:
    Curl_auth_create_login_message(Curl_creds_passwd(conn->creds), &resp);
    நிறுத்து;
  நிலை SASL_EXTERNAL:
    Curl_auth_create_external_message(Curl_creds_user(conn->creds), &resp);
    நிறுத்து;
#ifdef USE_GSASL
  நிலை SASL_GSASL:
    result = get_server_message(sasl, data, &serverdata);
    எனில்(!result) {
      கட்டமைப்பு gsasldata *gsasl = Curl_auth_gsasl_get(conn);
      result = !gsasl ? CURLE_OUT_OF_MEMORY :
        Curl_auth_gsasl_token(data, &serverdata, gsasl, &resp);
    }
    எனில்(!result && Curl_bufref_len(&resp) > 0)
      newstate = SASL_GSASL;
    நிறுத்து;
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  நிலை SASL_CRAMMD5:
    result = get_server_message(sasl, data, &serverdata);
    எனில்(!result)
      result = Curl_auth_create_cram_md5_message(&serverdata, conn->creds,
                                                 &resp);
    நிறுத்து;
  நிலை SASL_DIGESTMD5:
    result = get_server_message(sasl, data, &serverdata);
    எனில்(!result)
      result = Curl_auth_create_digest_md5_message(data, &serverdata,
                                                   conn->creds,
                                                   sasl->params->service,
                                                   &resp);
    எனில்(!result && (sasl->params->flags & SASL_FLAG_BASE64))
      newstate = SASL_DIGESTMD5_RESP;
    நிறுத்து;
  நிலை SASL_DIGESTMD5_RESP:
    /* Keep response NULL to output an empty line. */
    நிறுத்து;
#endif

#ifdef USE_NTLM
  நிலை SASL_NTLM: {
    /* Create the type-1 message */
    கட்டமைப்பு ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    result = !ntlm ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_ntlm_type1_message(data, conn->creds,
                                          sasl->params->service, hostname,
                                          ntlm, &resp);
    newstate = SASL_NTLM_TYPE2_செய்தி;
    நிறுத்து;
  }
  நிலை SASL_NTLM_TYPE2_செய்தி: {
    /* Decode the type-2 message */
    கட்டமைப்பு ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    result = !ntlm ? CURLE_OUT_OF_MEMORY :
      get_server_message(sasl, data, &serverdata);
    எனில்(!result)
      result = Curl_auth_decode_ntlm_type2_message(data, &serverdata, ntlm);
    எனில்(!result)
      result = Curl_auth_create_ntlm_type3_message(data, conn->creds,
                                                   ntlm, &resp);
    நிறுத்து;
  }
#endif

#ifdef USE_KERBEROS5
  நிலை SASL_GSSAPI: {
    கட்டமைப்பு kerberos5data *krb5 = Curl_auth_krb5_get(conn);
    result = !krb5 ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_gssapi_user_message(data, conn->creds,
                                           sasl->params->service,
                                           conn->origin->hostname,
                                           (bool)sasl->mutual_auth, வெற்று,
                                           krb5, &resp);
    newstate = SASL_GSSAPI_TOKEN;
    நிறுத்து;
  }
  நிலை SASL_GSSAPI_TOKEN:
    result = get_server_message(sasl, data, &serverdata);
    எனில்(!result) {
      கட்டமைப்பு kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      எனில்(!krb5)
        result = CURLE_OUT_OF_MEMORY;
      இல்லையெனில் எனில்(sasl->mutual_auth) {
        /* Decode the user token challenge and create the optional response
           message */
        result = Curl_auth_create_gssapi_user_message(data, வெற்று,
                                                      வெற்று, வெற்று,
                                                      (bool)sasl->mutual_auth,
                                                      &serverdata,
                                                      krb5, &resp);
        newstate = SASL_GSSAPI_NO_DATA;
      }
      இல்லையெனில்
        /* Decode the security challenge and create the response message */
        result = Curl_auth_create_gssapi_security_message(
          data, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    நிறுத்து;
  நிலை SASL_GSSAPI_NO_DATA:
    /* Decode the security challenge and create the response message */
    result = get_server_message(sasl, data, &serverdata);
    எனில்(!result) {
      கட்டமைப்பு kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      எனில்(!krb5)
        result = CURLE_OUT_OF_MEMORY;
      இல்லையெனில்
        result = Curl_auth_create_gssapi_security_message(
          data, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    நிறுத்து;
#endif

  நிலை SASL_OAUTH2:
    /* Create the authorization message */
    எனில்(sasl->authused == SASL_MECH_OAUTHBEARER) {
      result = Curl_auth_create_oauth_bearer_message(conn->creds,
                                                     hostname,
                                                     port,
                                                     &resp);

      /* Failures maybe sent by the server as continuations for OAUTHBEARER */
      newstate = SASL_OAUTH2_RESP;
    }
    இல்லையெனில்
      result = Curl_auth_create_xoauth_bearer_message(conn->creds,
                                                      &resp);
    நிறுத்து;

  நிலை SASL_OAUTH2_RESP:
    /* The continuation is optional so check the response code */
    எனில்(code == sasl->params->finalcode) {
      /* Final response was received so we are done */
      *progress = SASL_DONE;
      sasl_state(sasl, data, SASL_STOP);
      திரும்பு result;
    }
    இல்லையெனில் எனில்(code == sasl->params->contcode) {
      /* Acknowledge the continuation by sending a 0x01 response. */
      Curl_bufref_set(&resp, "\x01", 1, வெற்று);
      நிறுத்து;
    }
    இல்லையெனில் {
      *progress = SASL_DONE;
      sasl_state(sasl, data, SASL_STOP);
      திரும்பு CURLE_LOGIN_DENIED;
    }

  நிலை SASL_CANCEL:
    /* Remove the offending mechanism from the supported list */
    sasl->authmechs &= (unsigned short)~sasl->authused;
    sasl->authused = SASL_AUTH_NONE;
    sasl->curmech = வெற்று;

    /* Start an alternative SASL authentication */
    திரும்பு Curl_sasl_start(sasl, data, (bool)sasl->force_ir, progress);
  இயல்பு:
    failf(data, "Unsupported SASL authentication mechanism");
    result = CURLE_UNSUPPORTED_PROTOCOL;  /* Should not happen */
    நிறுத்து;
  }

  Curl_bufref_free(&serverdata);

  தேர்வு(result) {
  நிலை CURLE_BAD_CONTENT_ENCODING:
    /* Cancel dialog */
    result = sasl->params->cancelauth(data, sasl->curmech);
    newstate = SASL_CANCEL;
    நிறுத்து;
  நிலை CURLE_OK:
    result = build_message(sasl, &resp);
    எனில்(!result)
      result = sasl->params->contauth(data, sasl->curmech, &resp);
    நிறுத்து;
  இயல்பு:
    newstate = SASL_STOP;    /* Stop on error */
    *progress = SASL_DONE;
    நிறுத்து;
  }

  Curl_bufref_free(&resp);

  sasl_state(sasl, data, newstate);

  திரும்பு result;
}

#ifdef CURLVERBOSE
நிலையான வெறுமை sasl_unchosen(கட்டமைப்பு Curl_easy *data, unsigned short mech,
                          unsigned short enabledmechs,
                          bool built_in, bool platform,
                          மாறிலி எழுத்து *param_missing)
{
  மாறிலி எழுத்து *mname = வெற்று;
  size_t i;

  எனில்(!(enabledmechs & mech))
    திரும்பு;

  க்கு(i = 0; mechtable[i].name; ++i) {
    எனில்(mechtable[i].bit == mech) {
      mname = mechtable[i].name;
      நிறுத்து;
    }
  }
  எனில்(!mname)  /* should not happen */
    திரும்பு;
  எனில்(!built_in)
    infof(data, "SASL: %s not builtin", mname);
  இல்லையெனில் எனில்(!platform)
    infof(data, "SASL: %s not supported by the platform/libraries", mname);
  இல்லையெனில் {
    எனில்(param_missing)
      infof(data, "SASL: %s is missing %s", mname, param_missing);
    எனில்(!Curl_creds_has_user(data->conn->creds))
      infof(data, "SASL: %s is missing username", mname);
  }
}
#endif /* CURLVERBOSE */

CURLcode Curl_sasl_is_blocked(கட்டமைப்பு SASL *sasl, கட்டமைப்பு Curl_easy *data)
{
#ifdef CURLVERBOSE
#ifdef USE_KERBEROS5
#define CURL_SASL_KERBEROS5   TRUE
#இல்லையெனில்
#define CURL_SASL_KERBEROS5   FALSE
#endif
#ifdef USE_GSASL
#define CURL_SASL_GASL        TRUE
#இல்லையெனில்
#define CURL_SASL_GASL        FALSE
#endif
#ifdef CURL_DISABLE_DIGEST_AUTH
#define CURL_SASL_DIGEST      TRUE
#இல்லையெனில்
#define CURL_SASL_DIGEST      FALSE
#endif
#ifndef USE_NTLM
#define CURL_SASL_NTLM        TRUE
#இல்லையெனில்
#define CURL_SASL_NTLM        FALSE
#endif
  /* Failing SASL authentication is a pain. Give a helping hand if
   * we were unable to select an AUTH mechanism.
   * `sasl->authmechs` are mechanisms offered by the peer
   * `sasl->prefmech`  are mechanisms preferred by us */
  unsigned short enabledmechs = sasl->authmechs & sasl->prefmech;

  எனில்(!sasl->authmechs)
    infof(data, "SASL: no auth mechanism was offered or recognized");
  இல்லையெனில் எனில்(!enabledmechs)
    infof(data, "SASL: no overlap between offered and configured "
          "auth mechanisms");
  இல்லையெனில் {
    infof(data, "SASL: no auth mechanism offered could be selected");
    எனில்((enabledmechs & SASL_MECH_EXTERNAL) &&
       Curl_creds_has_passwd(data->conn->creds))
      infof(data, "SASL: auth EXTERNAL not chosen with password");
    sasl_unchosen(data, SASL_MECH_GSSAPI, enabledmechs,
                  CURL_SASL_KERBEROS5, Curl_auth_is_gssapi_supported(), வெற்று);
    sasl_unchosen(data, SASL_MECH_SCRAM_SHA_256, enabledmechs,
                  CURL_SASL_GASL, FALSE, வெற்று);
    sasl_unchosen(data, SASL_MECH_SCRAM_SHA_1, enabledmechs,
                  CURL_SASL_GASL, FALSE, வெற்று);
    sasl_unchosen(data, SASL_MECH_DIGEST_MD5, enabledmechs,
                  CURL_SASL_DIGEST, Curl_auth_is_digest_supported(), வெற்று);
    sasl_unchosen(data, SASL_MECH_CRAM_MD5, enabledmechs,
                  CURL_SASL_DIGEST, TRUE, வெற்று);
    sasl_unchosen(data, SASL_MECH_NTLM, enabledmechs,
                  CURL_SASL_NTLM, Curl_auth_is_ntlm_supported(), வெற்று);
    sasl_unchosen(data, SASL_MECH_OAUTHBEARER, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(data->conn->creds) ?
                  வெற்று : "CURLOPT_XOAUTH2_BEARER");
    sasl_unchosen(data, SASL_MECH_XOAUTH2, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(data->conn->creds) ?
                  வெற்று : "CURLOPT_XOAUTH2_BEARER");
  }
#endif /* CURLVERBOSE */
  (வெறுமை)sasl;
  (வெறுமை)data;
  திரும்பு CURLE_LOGIN_DENIED;
}

#endif /* protocols are enabled that use SASL */
