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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curlx/base64.h"
#അന്തര്ഭാവ "vauth/vauth.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "curl_sasl.h"
#അന്തര്ഭാവ "curl_trc.h"

/* Supported mechanisms */
സ്ഥിതം സ്ഥിരം ഘടന {
  സ്ഥിരം അക്ഷരം    *name;  /* Name */
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
unsigned short Curl_sasl_decode_mech(സ്ഥിരം അക്ഷരം *ptr, size_t maxlen,
                                     size_t *len)
{
  unsigned പൂർണ്ണസംഖ്യ i;
  അക്ഷരം c;

  വേണ്ടി(i = 0; mechtable[i].name; i++) {
    എങ്കിൽ(maxlen >= mechtable[i].len &&
       curl_strnequal(ptr, mechtable[i].name, mechtable[i].len)) {
      എങ്കിൽ(len)
        *len = mechtable[i].len;

      എങ്കിൽ(maxlen == mechtable[i].len)
        തിരികെ mechtable[i].bit;

      c = ptr[mechtable[i].len];
      എങ്കിൽ(!ISUPPER(c) && !ISDIGIT(c) && c != '-' && c != '_')
        തിരികെ mechtable[i].bit;
    }
  }

  തിരികെ 0;
}

/*
 * Curl_sasl_parse_url_auth_option()
 *
 * Parse the URL login options.
 */
CURLcode Curl_sasl_parse_url_auth_option(ഘടന SASL *sasl,
                                         സ്ഥിരം അക്ഷരം *മൂല്യം, size_t len)
{
  CURLcode result = CURLE_OK;
  size_t mechlen;

  എങ്കിൽ(!len)
    തിരികെ CURLE_URL_MALFORMAT;

  എങ്കിൽ(sasl->resetprefs) {
    sasl->resetprefs = FALSE;
    sasl->prefmech = SASL_AUTH_NONE;
  }

  എങ്കിൽ(!strncmp(മൂല്യം, "*", len))
    sasl->prefmech = SASL_AUTH_DEFAULT;
  അല്ലെങ്കിൽ {
    unsigned short mechbit = Curl_sasl_decode_mech(മൂല്യം, len, &mechlen);
    എങ്കിൽ(mechbit && mechlen == len)
      sasl->prefmech |= mechbit;
    അല്ലെങ്കിൽ
      result = CURLE_URL_MALFORMAT;
  }

  തിരികെ result;
}

/*
 * Curl_sasl_init()
 *
 * Initializes the SASL structure.
 */
ശൂന്യം Curl_sasl_init(ഘടന SASL *sasl, ഘടന Curl_easy *data,
                    സ്ഥിരം ഘടന SASLproto *params)
{
  uint32_t auth = data->set.httpauth;

  sasl->params = params;           /* Set protocol dependent parameters */
  sasl->state = SASL_STOP;         /* Not yet running */
  sasl->curmech = ശൂന്യ;            /* No mechanism yet. */
  sasl->authmechs = SASL_AUTH_NONE; /* No known authentication mechanism yet */
  sasl->prefmech = params->defmechs; /* Default preferred mechanisms */
  sasl->authused = SASL_AUTH_NONE; /* The authentication mechanism used */
  sasl->resetprefs = TRUE;         /* Reset prefmech upon AUTH parsing. */
  sasl->mutual_auth = FALSE;       /* No mutual authentication (GSSAPI only) */
  sasl->force_ir = FALSE;          /* Respect external option */

  എങ്കിൽ(auth != CURLAUTH_BASIC) {
    unsigned short mechs = SASL_AUTH_NONE;

    /* If some usable http authentication options have been set, determine
       new defaults from them. */
    എങ്കിൽ(auth & CURLAUTH_BASIC)
      mechs |= SASL_MECH_PLAIN | SASL_MECH_LOGIN;
    എങ്കിൽ(auth & CURLAUTH_DIGEST)
      mechs |= SASL_MECH_DIGEST_MD5;
    എങ്കിൽ(auth & CURLAUTH_NTLM)
      mechs |= SASL_MECH_NTLM;
    എങ്കിൽ(auth & CURLAUTH_BEARER)
      mechs |= SASL_MECH_OAUTHBEARER | SASL_MECH_XOAUTH2;
    എങ്കിൽ(auth & CURLAUTH_GSSAPI)
      mechs |= SASL_MECH_GSSAPI;

    എങ്കിൽ(mechs != SASL_AUTH_NONE)
      sasl->prefmech = mechs;
  }
}

/*
 * sasl_state()
 *
 * This is the ONLY way to change SASL state!
 */
സ്ഥിതം ശൂന്യം sasl_state(ഘടന SASL *sasl, ഘടന Curl_easy *data,
                       saslstate newstate)
{
#എങ്കിൽ defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം names[] = {
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

  എങ്കിൽ(sasl->state != newstate)
    infof(data, "SASL %p state change from %s to %s",
          (ശൂന്യം *)sasl, names[sasl->state], names[newstate]);
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
#endif

  sasl->state = newstate;
}

#എങ്കിൽ defined(USE_NTLM) || defined(USE_GSASL) || defined(USE_KERBEROS5) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)
/* Get the SASL server message and convert it to binary. */
സ്ഥിതം CURLcode get_server_message(ഘടന SASL *sasl, ഘടന Curl_easy *data,
                                   ഘടന bufref *out)
{
  CURLcode result = CURLE_OK;

  result = sasl->params->getmessage(data, out);
  എങ്കിൽ(!result && (sasl->params->flags & SASL_FLAG_BASE64)) {
    സ്ഥിരം അക്ഷരം *serverdata = Curl_bufref_ptr(out);

    എങ്കിൽ(!*serverdata)
      Curl_bufref_set(out, ശൂന്യ, 0, ശൂന്യ);
    അല്ലെങ്കിൽ {
      unsigned അക്ഷരം *സന്ദേശം;
      size_t msglen;

      result = curlx_base64_decode(serverdata, &സന്ദേശം, &msglen);
      എങ്കിൽ(!result)
        Curl_bufref_set(out, സന്ദേശം, msglen, curl_free);
    }
  }
  തിരികെ result;
}
#endif

/* Encode the outgoing SASL message. */
സ്ഥിതം CURLcode build_message(ഘടന SASL *sasl, ഘടന bufref *സന്ദേശം)
{
  CURLcode result = CURLE_OK;

  എങ്കിൽ(sasl->params->flags & SASL_FLAG_BASE64) {
    എങ്കിൽ(!Curl_bufref_ptr(സന്ദേശം))                   /* Empty message. */
      Curl_bufref_set(സന്ദേശം, "", 0, ശൂന്യ);
    അല്ലെങ്കിൽ എങ്കിൽ(Curl_bufref_len(സന്ദേശം)) {
      അക്ഷരം *base64;
      size_t base64len;

      result = curlx_base64_encode(Curl_bufref_uptr(സന്ദേശം),
                                   Curl_bufref_len(സന്ദേശം), &base64, &base64len);
      എങ്കിൽ(!result)
        Curl_bufref_set(സന്ദേശം, base64, base64len, curl_free);
    }
  }

  തിരികെ result;
}

/*
 * Curl_sasl_can_authenticate()
 *
 * Check if we have enough auth data and capabilities to authenticate.
 */
bool Curl_sasl_can_authenticate(ഘടന SASL *sasl, ഘടന Curl_easy *data)
{
  /* Have credentials been provided? */
  എങ്കിൽ(data->conn->creds)
    തിരികെ TRUE;

  /* EXTERNAL can authenticate without a username and/or password */
  എങ്കിൽ(sasl->authmechs & sasl->prefmech & SASL_MECH_EXTERNAL)
    തിരികെ TRUE;

  തിരികെ FALSE;
}

ഘടന sasl_ctx {
  ഘടന SASL *sasl;
  ഘടന connectdata *conn;
  unsigned short enabledmechs;
  സ്ഥിരം അക്ഷരം *mech;
  saslstate state1;
  saslstate state2;
  ഘടന bufref resp;
  CURLcode result;
};

സ്ഥിതം bool sasl_choose_external(ഘടന Curl_easy *data, ഘടന sasl_ctx *sctx)
{
  എങ്കിൽ((sctx->enabledmechs & SASL_MECH_EXTERNAL) &&
     !Curl_creds_has_passwd(sctx->conn->creds)) {
    sctx->mech = SASL_MECH_STRING_EXTERNAL;
    sctx->state1 = SASL_EXTERNAL;
    sctx->sasl->authused = SASL_MECH_EXTERNAL;

    എങ്കിൽ(sctx->sasl->force_ir || data->set.sasl_ir)
      Curl_auth_create_external_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    തിരികെ TRUE;
  }
  തിരികെ FALSE;
}

#ifdef USE_KERBEROS5
സ്ഥിതം bool sasl_choose_krb5(ഘടന Curl_easy *data, ഘടന sasl_ctx *sctx)
{
  എങ്കിൽ((sctx->enabledmechs & SASL_MECH_GSSAPI) &&
     Curl_auth_is_gssapi_supported() &&
     Curl_auth_user_contains_domain(sctx->conn->creds)) {
    സ്ഥിരം അക്ഷരം *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;

    sctx->sasl->mutual_auth = FALSE;
    sctx->mech = SASL_MECH_STRING_GSSAPI;
    sctx->state1 = SASL_GSSAPI;
    sctx->state2 = SASL_GSSAPI_TOKEN;
    sctx->sasl->authused = SASL_MECH_GSSAPI;

    എങ്കിൽ(sctx->sasl->force_ir || data->set.sasl_ir) {
      ഘടന kerberos5data *krb5 = Curl_auth_krb5_get(sctx->conn);
      sctx->result = !krb5 ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_gssapi_user_message(data, sctx->conn->creds,
                                             service,
                                             sctx->conn->origin->hostname,
                                             (bool)sctx->sasl->mutual_auth,
                                             ശൂന്യ, krb5, &sctx->resp);
    }
    തിരികെ TRUE;
  }
  തിരികെ FALSE;
}
#endif /* USE_KERBEROS5 */

#ifdef USE_GSASL
സ്ഥിതം bool sasl_choose_gsasl(ഘടന Curl_easy *data, ഘടന sasl_ctx *sctx)
{
  ഘടന gsasldata *gsasl;
  ഘടന bufref nullmsg;

  എങ്കിൽ((sctx->enabledmechs &
      (SASL_MECH_SCRAM_SHA_256 | SASL_MECH_SCRAM_SHA_1))) {
    gsasl = Curl_auth_gsasl_get(sctx->conn);
    എങ്കിൽ(!gsasl) {
      sctx->result = CURLE_OUT_OF_MEMORY;
      തിരികെ TRUE; /* attempted, but failed */
    }

    എങ്കിൽ((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_256) &&
       Curl_auth_gsasl_is_supported(data, SASL_MECH_STRING_SCRAM_SHA_256,
                                    gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_256;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_256;
    }
    അല്ലെങ്കിൽ എങ്കിൽ((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_1) &&
            Curl_auth_gsasl_is_supported(data, SASL_MECH_STRING_SCRAM_SHA_1,
                                         gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_1;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_1;
    }
    അല്ലെങ്കിൽ
      തിരികെ FALSE;

    Curl_bufref_init(&nullmsg);
    sctx->state1 = SASL_GSASL;
    sctx->state2 = SASL_GSASL;
    sctx->result = Curl_auth_gsasl_start(data, sctx->conn->creds, gsasl);
    എങ്കിൽ(!sctx->result && (sctx->sasl->force_ir || data->set.sasl_ir))
      sctx->result = Curl_auth_gsasl_token(data, &nullmsg, gsasl, &sctx->resp);
    തിരികെ TRUE;
  }
  തിരികെ FALSE;
}

#endif /* USE_GSASL */

#ifndef CURL_DISABLE_DIGEST_AUTH
സ്ഥിതം bool sasl_choose_digest(ഘടന Curl_easy *data, ഘടന sasl_ctx *sctx)
{
  (ശൂന്യം)data;
  എങ്കിൽ((sctx->enabledmechs & SASL_MECH_DIGEST_MD5) &&
     Curl_auth_is_digest_supported()) {
    sctx->mech = SASL_MECH_STRING_DIGEST_MD5;
    sctx->state1 = SASL_DIGESTMD5;
    sctx->sasl->authused = SASL_MECH_DIGEST_MD5;
    തിരികെ TRUE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(sctx->enabledmechs & SASL_MECH_CRAM_MD5) {
    sctx->mech = SASL_MECH_STRING_CRAM_MD5;
    sctx->state1 = SASL_CRAMMD5;
    sctx->sasl->authused = SASL_MECH_CRAM_MD5;
    തിരികെ TRUE;
  }
  തിരികെ FALSE;
}
#endif /* !CURL_DISABLE_DIGEST_AUTH */

#ifdef USE_NTLM
സ്ഥിതം bool sasl_choose_ntlm(ഘടന Curl_easy *data, ഘടന sasl_ctx *sctx)
{
  എങ്കിൽ((sctx->enabledmechs & SASL_MECH_NTLM) &&
     Curl_auth_is_ntlm_supported()) {
     സ്ഥിരം അക്ഷരം *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;
    സ്ഥിരം അക്ഷരം *hostname;

    Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, ശൂന്യ);

    sctx->mech = SASL_MECH_STRING_NTLM;
    sctx->state1 = SASL_NTLM;
    sctx->state2 = SASL_NTLM_TYPE2_സന്ദേശം;
    sctx->sasl->authused = SASL_MECH_NTLM;

    എങ്കിൽ(sctx->sasl->force_ir || data->set.sasl_ir) {
      ഘടന ntlmdata *ntlm = Curl_auth_ntlm_get(sctx->conn, FALSE);
      sctx->result = !ntlm ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_ntlm_type1_message(data, sctx->conn->creds,
                                            service, hostname,
                                            ntlm, &sctx->resp);
    }
    തിരികെ TRUE;
  }
  തിരികെ FALSE;
}
#endif /* USE_NTLM */

സ്ഥിതം bool sasl_choose_oauth(ഘടന Curl_easy *data, ഘടന sasl_ctx *sctx)
{
  എങ്കിൽ(Curl_creds_has_oauth_bearer(data->state.creds) &&
     (sctx->enabledmechs & SASL_MECH_OAUTHBEARER)) {
    സ്ഥിരം അക്ഷരം *hostname;
    പൂർണ്ണസംഖ്യ port;
    Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, &port);

    sctx->mech = SASL_MECH_STRING_OAUTHBEARER;
    sctx->state1 = SASL_OAUTH2;
    sctx->state2 = SASL_OAUTH2_RESP;
    sctx->sasl->authused = SASL_MECH_OAUTHBEARER;

    എങ്കിൽ(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result =
        Curl_auth_create_oauth_bearer_message(sctx->conn->creds,
                                              hostname, port, &sctx->resp);
    തിരികെ TRUE;
  }
  തിരികെ FALSE;
}

സ്ഥിതം bool sasl_choose_oauth2(ഘടന Curl_easy *data, ഘടന sasl_ctx *sctx)
{
  എങ്കിൽ(Curl_creds_has_oauth_bearer(sctx->conn->creds) &&
     (sctx->enabledmechs & SASL_MECH_XOAUTH2)) {
    sctx->mech = SASL_MECH_STRING_XOAUTH2;
    sctx->state1 = SASL_OAUTH2;
    sctx->sasl->authused = SASL_MECH_XOAUTH2;

    എങ്കിൽ(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result = Curl_auth_create_xoauth_bearer_message(
        sctx->conn->creds, &sctx->resp);
    തിരികെ TRUE;
  }
  തിരികെ FALSE;
}

സ്ഥിതം bool sasl_choose_plain(ഘടന Curl_easy *data, ഘടന sasl_ctx *sctx)
{
  എങ്കിൽ(sctx->enabledmechs & SASL_MECH_PLAIN) {
    sctx->mech = SASL_MECH_STRING_PLAIN;
    sctx->state1 = SASL_PLAIN;
    sctx->sasl->authused = SASL_MECH_PLAIN;

    എങ്കിൽ(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result =
        Curl_auth_create_plain_message(sctx->conn->creds, &sctx->resp);
    തിരികെ TRUE;
  }
  തിരികെ FALSE;
}

സ്ഥിതം bool sasl_choose_login(ഘടന Curl_easy *data, ഘടന sasl_ctx *sctx)
{
  എങ്കിൽ(sctx->enabledmechs & SASL_MECH_LOGIN) {
    sctx->mech = SASL_MECH_STRING_LOGIN;
    sctx->state1 = SASL_LOGIN;
    sctx->state2 = SASL_LOGIN_PASSWD;
    sctx->sasl->authused = SASL_MECH_LOGIN;

    എങ്കിൽ(sctx->sasl->force_ir || data->set.sasl_ir)
      Curl_auth_create_login_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    തിരികെ TRUE;
  }
  തിരികെ FALSE;
}

/*
 * Curl_sasl_start()
 *
 * Calculate the required login details for SASL authentication.
 */
CURLcode Curl_sasl_start(ഘടന SASL *sasl, ഘടന Curl_easy *data,
                         bool force_ir, saslprogress *progress)
{
  ഘടന sasl_ctx sctx;

  sasl->force_ir = force_ir;    /* Latch for future use */
  sasl->authused = 0;           /* No mechanism used yet */
  *progress = SASL_IDLE;

  memset(&sctx, 0, വലുപ്പം(sctx));
  sctx.sasl = sasl;
  sctx.conn = data->conn;
  Curl_bufref_init(&sctx.resp);
  sctx.enabledmechs = sasl->authmechs & sasl->prefmech;
  sctx.state1 = SASL_STOP;
  sctx.state2 = SASL_FINAL;

  /* Calculate the supported authentication mechanism, by decreasing order of
     security, as well as the initial response where appropriate */
  എങ്കിൽ(sasl_choose_external(data, &sctx) ||
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

  എങ്കിൽ(!sctx.result && sctx.mech) {
    sasl->curmech = sctx.mech;
    എങ്കിൽ(Curl_bufref_ptr(&sctx.resp))
      sctx.result = build_message(sasl, &sctx.resp);

    എങ്കിൽ(sasl->params->maxirlen &&
       strlen(sctx.mech) + Curl_bufref_len(&sctx.resp) >
         sasl->params->maxirlen)
      Curl_bufref_free(&sctx.resp);

    എങ്കിൽ(!sctx.result)
      sctx.result = sasl->params->sendauth(data, sctx.mech, &sctx.resp);

    എങ്കിൽ(!sctx.result) {
      *progress = SASL_INPROGRESS;
      sasl_state(sasl, data, Curl_bufref_ptr(&sctx.resp) ?
                 sctx.state2 : sctx.state1);
    }
  }

  Curl_bufref_free(&sctx.resp);
  തിരികെ sctx.result;
}

/*
 * Curl_sasl_continue()
 *
 * Continue the authentication.
 */
CURLcode Curl_sasl_continue(ഘടന SASL *sasl, ഘടന Curl_easy *data,
                            പൂർണ്ണസംഖ്യ code, saslprogress *progress)
{
  CURLcode result = CURLE_OK;
  ഘടന connectdata *conn = data->conn;
  saslstate newstate = SASL_FINAL;
  ഘടന bufref resp;
  സ്ഥിരം അക്ഷരം *hostname;
  പൂർണ്ണസംഖ്യ port;
  ഘടന bufref serverdata;

  Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, &port);
  Curl_bufref_init(&serverdata);
  Curl_bufref_init(&resp);
  *progress = SASL_INPROGRESS;

  എങ്കിൽ(sasl->state == SASL_FINAL) {
    എങ്കിൽ(code != sasl->params->finalcode)
      result = CURLE_LOGIN_DENIED;
    *progress = SASL_DONE;
    sasl_state(sasl, data, SASL_STOP);
    തിരികെ result;
  }

  എങ്കിൽ(sasl->state != SASL_CANCEL && sasl->state != SASL_OAUTH2_RESP &&
     code != sasl->params->contcode) {
    *progress = SASL_DONE;
    sasl_state(sasl, data, SASL_STOP);
    തിരികെ CURLE_LOGIN_DENIED;
  }

  തിരഞ്ഞെടുപ്പ്(sasl->state) {
  അവസ്ഥ SASL_STOP:
    *progress = SASL_DONE;
    തിരികെ result;
  അവസ്ഥ SASL_PLAIN:
    result = Curl_auth_create_plain_message(conn->creds, &resp);
    നിർത്തുക;
  അവസ്ഥ SASL_LOGIN:
    Curl_auth_create_login_message(Curl_creds_user(conn->creds), &resp);
    newstate = SASL_LOGIN_PASSWD;
    നിർത്തുക;
  അവസ്ഥ SASL_LOGIN_PASSWD:
    Curl_auth_create_login_message(Curl_creds_passwd(conn->creds), &resp);
    നിർത്തുക;
  അവസ്ഥ SASL_EXTERNAL:
    Curl_auth_create_external_message(Curl_creds_user(conn->creds), &resp);
    നിർത്തുക;
#ifdef USE_GSASL
  അവസ്ഥ SASL_GSASL:
    result = get_server_message(sasl, data, &serverdata);
    എങ്കിൽ(!result) {
      ഘടന gsasldata *gsasl = Curl_auth_gsasl_get(conn);
      result = !gsasl ? CURLE_OUT_OF_MEMORY :
        Curl_auth_gsasl_token(data, &serverdata, gsasl, &resp);
    }
    എങ്കിൽ(!result && Curl_bufref_len(&resp) > 0)
      newstate = SASL_GSASL;
    നിർത്തുക;
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  അവസ്ഥ SASL_CRAMMD5:
    result = get_server_message(sasl, data, &serverdata);
    എങ്കിൽ(!result)
      result = Curl_auth_create_cram_md5_message(&serverdata, conn->creds,
                                                 &resp);
    നിർത്തുക;
  അവസ്ഥ SASL_DIGESTMD5:
    result = get_server_message(sasl, data, &serverdata);
    എങ്കിൽ(!result)
      result = Curl_auth_create_digest_md5_message(data, &serverdata,
                                                   conn->creds,
                                                   sasl->params->service,
                                                   &resp);
    എങ്കിൽ(!result && (sasl->params->flags & SASL_FLAG_BASE64))
      newstate = SASL_DIGESTMD5_RESP;
    നിർത്തുക;
  അവസ്ഥ SASL_DIGESTMD5_RESP:
    /* Keep response NULL to output an empty line. */
    നിർത്തുക;
#endif

#ifdef USE_NTLM
  അവസ്ഥ SASL_NTLM: {
    /* Create the type-1 message */
    ഘടന ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    result = !ntlm ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_ntlm_type1_message(data, conn->creds,
                                          sasl->params->service, hostname,
                                          ntlm, &resp);
    newstate = SASL_NTLM_TYPE2_സന്ദേശം;
    നിർത്തുക;
  }
  അവസ്ഥ SASL_NTLM_TYPE2_സന്ദേശം: {
    /* Decode the type-2 message */
    ഘടന ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    result = !ntlm ? CURLE_OUT_OF_MEMORY :
      get_server_message(sasl, data, &serverdata);
    എങ്കിൽ(!result)
      result = Curl_auth_decode_ntlm_type2_message(data, &serverdata, ntlm);
    എങ്കിൽ(!result)
      result = Curl_auth_create_ntlm_type3_message(data, conn->creds,
                                                   ntlm, &resp);
    നിർത്തുക;
  }
#endif

#ifdef USE_KERBEROS5
  അവസ്ഥ SASL_GSSAPI: {
    ഘടന kerberos5data *krb5 = Curl_auth_krb5_get(conn);
    result = !krb5 ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_gssapi_user_message(data, conn->creds,
                                           sasl->params->service,
                                           conn->origin->hostname,
                                           (bool)sasl->mutual_auth, ശൂന്യ,
                                           krb5, &resp);
    newstate = SASL_GSSAPI_TOKEN;
    നിർത്തുക;
  }
  അവസ്ഥ SASL_GSSAPI_TOKEN:
    result = get_server_message(sasl, data, &serverdata);
    എങ്കിൽ(!result) {
      ഘടന kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      എങ്കിൽ(!krb5)
        result = CURLE_OUT_OF_MEMORY;
      അല്ലെങ്കിൽ എങ്കിൽ(sasl->mutual_auth) {
        /* Decode the user token challenge and create the optional response
           message */
        result = Curl_auth_create_gssapi_user_message(data, ശൂന്യ,
                                                      ശൂന്യ, ശൂന്യ,
                                                      (bool)sasl->mutual_auth,
                                                      &serverdata,
                                                      krb5, &resp);
        newstate = SASL_GSSAPI_NO_DATA;
      }
      അല്ലെങ്കിൽ
        /* Decode the security challenge and create the response message */
        result = Curl_auth_create_gssapi_security_message(
          data, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    നിർത്തുക;
  അവസ്ഥ SASL_GSSAPI_NO_DATA:
    /* Decode the security challenge and create the response message */
    result = get_server_message(sasl, data, &serverdata);
    എങ്കിൽ(!result) {
      ഘടന kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      എങ്കിൽ(!krb5)
        result = CURLE_OUT_OF_MEMORY;
      അല്ലെങ്കിൽ
        result = Curl_auth_create_gssapi_security_message(
          data, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    നിർത്തുക;
#endif

  അവസ്ഥ SASL_OAUTH2:
    /* Create the authorization message */
    എങ്കിൽ(sasl->authused == SASL_MECH_OAUTHBEARER) {
      result = Curl_auth_create_oauth_bearer_message(conn->creds,
                                                     hostname,
                                                     port,
                                                     &resp);

      /* Failures maybe sent by the server as continuations for OAUTHBEARER */
      newstate = SASL_OAUTH2_RESP;
    }
    അല്ലെങ്കിൽ
      result = Curl_auth_create_xoauth_bearer_message(conn->creds,
                                                      &resp);
    നിർത്തുക;

  അവസ്ഥ SASL_OAUTH2_RESP:
    /* The continuation is optional so check the response code */
    എങ്കിൽ(code == sasl->params->finalcode) {
      /* Final response was received so we are done */
      *progress = SASL_DONE;
      sasl_state(sasl, data, SASL_STOP);
      തിരികെ result;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(code == sasl->params->contcode) {
      /* Acknowledge the continuation by sending a 0x01 response. */
      Curl_bufref_set(&resp, "\x01", 1, ശൂന്യ);
      നിർത്തുക;
    }
    അല്ലെങ്കിൽ {
      *progress = SASL_DONE;
      sasl_state(sasl, data, SASL_STOP);
      തിരികെ CURLE_LOGIN_DENIED;
    }

  അവസ്ഥ SASL_CANCEL:
    /* Remove the offending mechanism from the supported list */
    sasl->authmechs &= (unsigned short)~sasl->authused;
    sasl->authused = SASL_AUTH_NONE;
    sasl->curmech = ശൂന്യ;

    /* Start an alternative SASL authentication */
    തിരികെ Curl_sasl_start(sasl, data, (bool)sasl->force_ir, progress);
  സ്ഥിരസ്ഥിതി:
    failf(data, "Unsupported SASL authentication mechanism");
    result = CURLE_UNSUPPORTED_PROTOCOL;  /* Should not happen */
    നിർത്തുക;
  }

  Curl_bufref_free(&serverdata);

  തിരഞ്ഞെടുപ്പ്(result) {
  അവസ്ഥ CURLE_BAD_CONTENT_ENCODING:
    /* Cancel dialog */
    result = sasl->params->cancelauth(data, sasl->curmech);
    newstate = SASL_CANCEL;
    നിർത്തുക;
  അവസ്ഥ CURLE_OK:
    result = build_message(sasl, &resp);
    എങ്കിൽ(!result)
      result = sasl->params->contauth(data, sasl->curmech, &resp);
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    newstate = SASL_STOP;    /* Stop on error */
    *progress = SASL_DONE;
    നിർത്തുക;
  }

  Curl_bufref_free(&resp);

  sasl_state(sasl, data, newstate);

  തിരികെ result;
}

#ifdef CURLVERBOSE
സ്ഥിതം ശൂന്യം sasl_unchosen(ഘടന Curl_easy *data, unsigned short mech,
                          unsigned short enabledmechs,
                          bool built_in, bool platform,
                          സ്ഥിരം അക്ഷരം *param_missing)
{
  സ്ഥിരം അക്ഷരം *mname = ശൂന്യ;
  size_t i;

  എങ്കിൽ(!(enabledmechs & mech))
    തിരികെ;

  വേണ്ടി(i = 0; mechtable[i].name; ++i) {
    എങ്കിൽ(mechtable[i].bit == mech) {
      mname = mechtable[i].name;
      നിർത്തുക;
    }
  }
  എങ്കിൽ(!mname)  /* should not happen */
    തിരികെ;
  എങ്കിൽ(!built_in)
    infof(data, "SASL: %s not builtin", mname);
  അല്ലെങ്കിൽ എങ്കിൽ(!platform)
    infof(data, "SASL: %s not supported by the platform/libraries", mname);
  അല്ലെങ്കിൽ {
    എങ്കിൽ(param_missing)
      infof(data, "SASL: %s is missing %s", mname, param_missing);
    എങ്കിൽ(!Curl_creds_has_user(data->conn->creds))
      infof(data, "SASL: %s is missing username", mname);
  }
}
#endif /* CURLVERBOSE */

CURLcode Curl_sasl_is_blocked(ഘടന SASL *sasl, ഘടന Curl_easy *data)
{
#ifdef CURLVERBOSE
#ifdef USE_KERBEROS5
#define CURL_SASL_KERBEROS5   TRUE
#അല്ലെങ്കിൽ
#define CURL_SASL_KERBEROS5   FALSE
#endif
#ifdef USE_GSASL
#define CURL_SASL_GASL        TRUE
#അല്ലെങ്കിൽ
#define CURL_SASL_GASL        FALSE
#endif
#ifdef CURL_DISABLE_DIGEST_AUTH
#define CURL_SASL_DIGEST      TRUE
#അല്ലെങ്കിൽ
#define CURL_SASL_DIGEST      FALSE
#endif
#ifndef USE_NTLM
#define CURL_SASL_NTLM        TRUE
#അല്ലെങ്കിൽ
#define CURL_SASL_NTLM        FALSE
#endif
  /* Failing SASL authentication is a pain. Give a helping hand if
   * we were unable to select an AUTH mechanism.
   * `sasl->authmechs` are mechanisms offered by the peer
   * `sasl->prefmech`  are mechanisms preferred by us */
  unsigned short enabledmechs = sasl->authmechs & sasl->prefmech;

  എങ്കിൽ(!sasl->authmechs)
    infof(data, "SASL: no auth mechanism was offered or recognized");
  അല്ലെങ്കിൽ എങ്കിൽ(!enabledmechs)
    infof(data, "SASL: no overlap between offered and configured "
          "auth mechanisms");
  അല്ലെങ്കിൽ {
    infof(data, "SASL: no auth mechanism offered could be selected");
    എങ്കിൽ((enabledmechs & SASL_MECH_EXTERNAL) &&
       Curl_creds_has_passwd(data->conn->creds))
      infof(data, "SASL: auth EXTERNAL not chosen with password");
    sasl_unchosen(data, SASL_MECH_GSSAPI, enabledmechs,
                  CURL_SASL_KERBEROS5, Curl_auth_is_gssapi_supported(), ശൂന്യ);
    sasl_unchosen(data, SASL_MECH_SCRAM_SHA_256, enabledmechs,
                  CURL_SASL_GASL, FALSE, ശൂന്യ);
    sasl_unchosen(data, SASL_MECH_SCRAM_SHA_1, enabledmechs,
                  CURL_SASL_GASL, FALSE, ശൂന്യ);
    sasl_unchosen(data, SASL_MECH_DIGEST_MD5, enabledmechs,
                  CURL_SASL_DIGEST, Curl_auth_is_digest_supported(), ശൂന്യ);
    sasl_unchosen(data, SASL_MECH_CRAM_MD5, enabledmechs,
                  CURL_SASL_DIGEST, TRUE, ശൂന്യ);
    sasl_unchosen(data, SASL_MECH_NTLM, enabledmechs,
                  CURL_SASL_NTLM, Curl_auth_is_ntlm_supported(), ശൂന്യ);
    sasl_unchosen(data, SASL_MECH_OAUTHBEARER, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(data->conn->creds) ?
                  ശൂന്യ : "CURLOPT_XOAUTH2_BEARER");
    sasl_unchosen(data, SASL_MECH_XOAUTH2, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(data->conn->creds) ?
                  ശൂന്യ : "CURLOPT_XOAUTH2_BEARER");
  }
#endif /* CURLVERBOSE */
  (ശൂന്യം)sasl;
  (ശൂന്യം)data;
  തിരികെ CURLE_LOGIN_DENIED;
}

#endif /* protocols are enabled that use SASL */
