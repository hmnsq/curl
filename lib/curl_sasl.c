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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#समावेश "urldata.h"
#समावेश "curlx/base64.h"
#समावेश "vauth/vauth.h"
#समावेश "cfilters.h"
#समावेश "curl_sasl.h"
#समावेश "curl_trc.h"

/* Supported mechanisms */
अटल स्थिर काष्ठा {
  स्थिर अक्षर    *name;  /* Name */
  माप_प्रकार         len;   /* Name length */
  अचिन्हित लघु bit;   /* Flag bit */
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
अचिन्हित लघु Curl_sasl_decode_mech(स्थिर अक्षर *ptr, माप_प्रकार maxlen,
                                     माप_प्रकार *len)
{
  अचिन्हित पूर्णांक i;
  अक्षर c;

  क्रम(i = 0; mechtable[i].name; i++) {
    अगर(maxlen >= mechtable[i].len &&
       curl_strnequal(ptr, mechtable[i].name, mechtable[i].len)) {
      अगर(len)
        *len = mechtable[i].len;

      अगर(maxlen == mechtable[i].len)
        वापस mechtable[i].bit;

      c = ptr[mechtable[i].len];
      अगर(!ISUPPER(c) && !ISDIGIT(c) && c != '-' && c != '_')
        वापस mechtable[i].bit;
    }
  }

  वापस 0;
}

/*
 * Curl_sasl_parse_url_auth_option()
 *
 * Parse the URL login options.
 */
CURLcode Curl_sasl_parse_url_auth_option(काष्ठा SASL *sasl,
                                         स्थिर अक्षर *मूल्य, माप_प्रकार len)
{
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार mechlen;

  अगर(!len)
    वापस CURLE_URL_MALFORMAT;

  अगर(sasl->resetprefs) {
    sasl->resetprefs = FALSE;
    sasl->prefmech = SASL_AUTH_NONE;
  }

  अगर(!म_भेदन(मूल्य, "*", len))
    sasl->prefmech = SASL_AUTH_DEFAULT;
  अन्यथा {
    अचिन्हित लघु mechbit = Curl_sasl_decode_mech(मूल्य, len, &mechlen);
    अगर(mechbit && mechlen == len)
      sasl->prefmech |= mechbit;
    अन्यथा
      परिणाम = CURLE_URL_MALFORMAT;
  }

  वापस परिणाम;
}

/*
 * Curl_sasl_init()
 *
 * Initializes the SASL structure.
 */
व्योम Curl_sasl_init(काष्ठा SASL *sasl, काष्ठा Curl_easy *तथ्य,
                    स्थिर काष्ठा SASLproto *params)
{
  uint32_t auth = तथ्य->set.httpauth;

  sasl->params = params;           /* Set protocol dependent parameters */
  sasl->state = SASL_STOP;         /* Not yet running */
  sasl->curmech = शून्य;            /* No mechanism yet. */
  sasl->authmechs = SASL_AUTH_NONE; /* No known authentication mechanism yet */
  sasl->prefmech = params->defmechs; /* Default preferred mechanisms */
  sasl->authused = SASL_AUTH_NONE; /* The authentication mechanism used */
  sasl->resetprefs = TRUE;         /* Reset prefmech upon AUTH parsing. */
  sasl->mutual_auth = FALSE;       /* No mutual authentication (GSSAPI only) */
  sasl->force_ir = FALSE;          /* Respect external option */

  अगर(auth != CURLAUTH_BASIC) {
    अचिन्हित लघु mechs = SASL_AUTH_NONE;

    /* If some usable http authentication options have been set, determine
       new defaults from them. */
    अगर(auth & CURLAUTH_BASIC)
      mechs |= SASL_MECH_PLAIN | SASL_MECH_LOGIN;
    अगर(auth & CURLAUTH_DIGEST)
      mechs |= SASL_MECH_DIGEST_MD5;
    अगर(auth & CURLAUTH_NTLM)
      mechs |= SASL_MECH_NTLM;
    अगर(auth & CURLAUTH_BEARER)
      mechs |= SASL_MECH_OAUTHBEARER | SASL_MECH_XOAUTH2;
    अगर(auth & CURLAUTH_GSSAPI)
      mechs |= SASL_MECH_GSSAPI;

    अगर(mechs != SASL_AUTH_NONE)
      sasl->prefmech = mechs;
  }
}

/*
 * sasl_state()
 *
 * This is the ONLY way to change SASL state!
 */
अटल व्योम sasl_state(काष्ठा SASL *sasl, काष्ठा Curl_easy *तथ्य,
                       saslstate newstate)
{
#अगर defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  अटल स्थिर अक्षर * स्थिर names[] = {
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

  अगर(sasl->state != newstate)
    infof(तथ्य, "SASL %p state change from %s to %s",
          (व्योम *)sasl, names[sasl->state], names[newstate]);
#अन्यथा
  (व्योम)तथ्य;
#पूर्ण_अगर

  sasl->state = newstate;
}

#अगर defined(USE_NTLM) || defined(USE_GSASL) || defined(USE_KERBEROS5) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)
/* Get the SASL server message and convert it to binary. */
अटल CURLcode get_server_message(काष्ठा SASL *sasl, काष्ठा Curl_easy *तथ्य,
                                   काष्ठा bufref *out)
{
  CURLcode परिणाम = CURLE_OK;

  परिणाम = sasl->params->getmessage(तथ्य, out);
  अगर(!परिणाम && (sasl->params->flags & SASL_FLAG_BASE64)) {
    स्थिर अक्षर *serverdata = Curl_bufref_ptr(out);

    अगर(!*serverdata)
      Curl_bufref_set(out, शून्य, 0, शून्य);
    अन्यथा {
      अचिन्हित अक्षर *संदेश;
      माप_प्रकार msglen;

      परिणाम = curlx_base64_decode(serverdata, &संदेश, &msglen);
      अगर(!परिणाम)
        Curl_bufref_set(out, संदेश, msglen, curl_free);
    }
  }
  वापस परिणाम;
}
#पूर्ण_अगर

/* Encode the outgoing SASL message. */
अटल CURLcode build_message(काष्ठा SASL *sasl, काष्ठा bufref *संदेश)
{
  CURLcode परिणाम = CURLE_OK;

  अगर(sasl->params->flags & SASL_FLAG_BASE64) {
    अगर(!Curl_bufref_ptr(संदेश))                   /* Empty message. */
      Curl_bufref_set(संदेश, "", 0, शून्य);
    अन्यथा अगर(Curl_bufref_len(संदेश)) {
      अक्षर *base64;
      माप_प्रकार base64len;

      परिणाम = curlx_base64_encode(Curl_bufref_uptr(संदेश),
                                   Curl_bufref_len(संदेश), &base64, &base64len);
      अगर(!परिणाम)
        Curl_bufref_set(संदेश, base64, base64len, curl_free);
    }
  }

  वापस परिणाम;
}

/*
 * Curl_sasl_can_authenticate()
 *
 * Check if we have enough auth data and capabilities to authenticate.
 */
bool Curl_sasl_can_authenticate(काष्ठा SASL *sasl, काष्ठा Curl_easy *तथ्य)
{
  /* Have credentials been provided? */
  अगर(तथ्य->conn->creds)
    वापस TRUE;

  /* EXTERNAL can authenticate without a username and/or password */
  अगर(sasl->authmechs & sasl->prefmech & SASL_MECH_EXTERNAL)
    वापस TRUE;

  वापस FALSE;
}

काष्ठा sasl_ctx {
  काष्ठा SASL *sasl;
  काष्ठा connectdata *conn;
  अचिन्हित लघु enabledmechs;
  स्थिर अक्षर *mech;
  saslstate state1;
  saslstate state2;
  काष्ठा bufref resp;
  CURLcode परिणाम;
};

अटल bool sasl_choose_external(काष्ठा Curl_easy *तथ्य, काष्ठा sasl_ctx *sctx)
{
  अगर((sctx->enabledmechs & SASL_MECH_EXTERNAL) &&
     !Curl_creds_has_passwd(sctx->conn->creds)) {
    sctx->mech = SASL_MECH_STRING_EXTERNAL;
    sctx->state1 = SASL_EXTERNAL;
    sctx->sasl->authused = SASL_MECH_EXTERNAL;

    अगर(sctx->sasl->force_ir || तथ्य->set.sasl_ir)
      Curl_auth_create_external_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    वापस TRUE;
  }
  वापस FALSE;
}

#अगर_घोषित USE_KERBEROS5
अटल bool sasl_choose_krb5(काष्ठा Curl_easy *तथ्य, काष्ठा sasl_ctx *sctx)
{
  अगर((sctx->enabledmechs & SASL_MECH_GSSAPI) &&
     Curl_auth_is_gssapi_supported() &&
     Curl_auth_user_contains_domain(sctx->conn->creds)) {
    स्थिर अक्षर *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;

    sctx->sasl->mutual_auth = FALSE;
    sctx->mech = SASL_MECH_STRING_GSSAPI;
    sctx->state1 = SASL_GSSAPI;
    sctx->state2 = SASL_GSSAPI_TOKEN;
    sctx->sasl->authused = SASL_MECH_GSSAPI;

    अगर(sctx->sasl->force_ir || तथ्य->set.sasl_ir) {
      काष्ठा kerberos5data *krb5 = Curl_auth_krb5_get(sctx->conn);
      sctx->परिणाम = !krb5 ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_gssapi_user_message(तथ्य, sctx->conn->creds,
                                             service,
                                             sctx->conn->origin->hostname,
                                             (bool)sctx->sasl->mutual_auth,
                                             शून्य, krb5, &sctx->resp);
    }
    वापस TRUE;
  }
  वापस FALSE;
}
#पूर्ण_अगर /* USE_KERBEROS5 */

#अगर_घोषित USE_GSASL
अटल bool sasl_choose_gsasl(काष्ठा Curl_easy *तथ्य, काष्ठा sasl_ctx *sctx)
{
  काष्ठा gsasldata *gsasl;
  काष्ठा bufref nullmsg;

  अगर((sctx->enabledmechs &
      (SASL_MECH_SCRAM_SHA_256 | SASL_MECH_SCRAM_SHA_1))) {
    gsasl = Curl_auth_gsasl_get(sctx->conn);
    अगर(!gsasl) {
      sctx->परिणाम = CURLE_OUT_OF_MEMORY;
      वापस TRUE; /* attempted, but failed */
    }

    अगर((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_256) &&
       Curl_auth_gsasl_is_supported(तथ्य, SASL_MECH_STRING_SCRAM_SHA_256,
                                    gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_256;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_256;
    }
    अन्यथा अगर((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_1) &&
            Curl_auth_gsasl_is_supported(तथ्य, SASL_MECH_STRING_SCRAM_SHA_1,
                                         gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_1;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_1;
    }
    अन्यथा
      वापस FALSE;

    Curl_bufref_init(&nullmsg);
    sctx->state1 = SASL_GSASL;
    sctx->state2 = SASL_GSASL;
    sctx->परिणाम = Curl_auth_gsasl_start(तथ्य, sctx->conn->creds, gsasl);
    अगर(!sctx->परिणाम && (sctx->sasl->force_ir || तथ्य->set.sasl_ir))
      sctx->परिणाम = Curl_auth_gsasl_token(तथ्य, &nullmsg, gsasl, &sctx->resp);
    वापस TRUE;
  }
  वापस FALSE;
}

#पूर्ण_अगर /* USE_GSASL */

#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH
अटल bool sasl_choose_digest(काष्ठा Curl_easy *तथ्य, काष्ठा sasl_ctx *sctx)
{
  (व्योम)तथ्य;
  अगर((sctx->enabledmechs & SASL_MECH_DIGEST_MD5) &&
     Curl_auth_is_digest_supported()) {
    sctx->mech = SASL_MECH_STRING_DIGEST_MD5;
    sctx->state1 = SASL_DIGESTMD5;
    sctx->sasl->authused = SASL_MECH_DIGEST_MD5;
    वापस TRUE;
  }
  अन्यथा अगर(sctx->enabledmechs & SASL_MECH_CRAM_MD5) {
    sctx->mech = SASL_MECH_STRING_CRAM_MD5;
    sctx->state1 = SASL_CRAMMD5;
    sctx->sasl->authused = SASL_MECH_CRAM_MD5;
    वापस TRUE;
  }
  वापस FALSE;
}
#पूर्ण_अगर /* !CURL_DISABLE_DIGEST_AUTH */

#अगर_घोषित USE_NTLM
अटल bool sasl_choose_ntlm(काष्ठा Curl_easy *तथ्य, काष्ठा sasl_ctx *sctx)
{
  अगर((sctx->enabledmechs & SASL_MECH_NTLM) &&
     Curl_auth_is_ntlm_supported()) {
     स्थिर अक्षर *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;
    स्थिर अक्षर *hostname;

    Curl_conn_get_current_host(तथ्य, FIRSTSOCKET, &hostname, शून्य);

    sctx->mech = SASL_MECH_STRING_NTLM;
    sctx->state1 = SASL_NTLM;
    sctx->state2 = SASL_NTLM_TYPE2_संदेश;
    sctx->sasl->authused = SASL_MECH_NTLM;

    अगर(sctx->sasl->force_ir || तथ्य->set.sasl_ir) {
      काष्ठा ntlmdata *ntlm = Curl_auth_ntlm_get(sctx->conn, FALSE);
      sctx->परिणाम = !ntlm ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_ntlm_type1_message(तथ्य, sctx->conn->creds,
                                            service, hostname,
                                            ntlm, &sctx->resp);
    }
    वापस TRUE;
  }
  वापस FALSE;
}
#पूर्ण_अगर /* USE_NTLM */

अटल bool sasl_choose_oauth(काष्ठा Curl_easy *तथ्य, काष्ठा sasl_ctx *sctx)
{
  अगर(Curl_creds_has_oauth_bearer(तथ्य->state.creds) &&
     (sctx->enabledmechs & SASL_MECH_OAUTHBEARER)) {
    स्थिर अक्षर *hostname;
    पूर्णांक port;
    Curl_conn_get_current_host(तथ्य, FIRSTSOCKET, &hostname, &port);

    sctx->mech = SASL_MECH_STRING_OAUTHBEARER;
    sctx->state1 = SASL_OAUTH2;
    sctx->state2 = SASL_OAUTH2_RESP;
    sctx->sasl->authused = SASL_MECH_OAUTHBEARER;

    अगर(sctx->sasl->force_ir || तथ्य->set.sasl_ir)
      sctx->परिणाम =
        Curl_auth_create_oauth_bearer_message(sctx->conn->creds,
                                              hostname, port, &sctx->resp);
    वापस TRUE;
  }
  वापस FALSE;
}

अटल bool sasl_choose_oauth2(काष्ठा Curl_easy *तथ्य, काष्ठा sasl_ctx *sctx)
{
  अगर(Curl_creds_has_oauth_bearer(sctx->conn->creds) &&
     (sctx->enabledmechs & SASL_MECH_XOAUTH2)) {
    sctx->mech = SASL_MECH_STRING_XOAUTH2;
    sctx->state1 = SASL_OAUTH2;
    sctx->sasl->authused = SASL_MECH_XOAUTH2;

    अगर(sctx->sasl->force_ir || तथ्य->set.sasl_ir)
      sctx->परिणाम = Curl_auth_create_xoauth_bearer_message(
        sctx->conn->creds, &sctx->resp);
    वापस TRUE;
  }
  वापस FALSE;
}

अटल bool sasl_choose_plain(काष्ठा Curl_easy *तथ्य, काष्ठा sasl_ctx *sctx)
{
  अगर(sctx->enabledmechs & SASL_MECH_PLAIN) {
    sctx->mech = SASL_MECH_STRING_PLAIN;
    sctx->state1 = SASL_PLAIN;
    sctx->sasl->authused = SASL_MECH_PLAIN;

    अगर(sctx->sasl->force_ir || तथ्य->set.sasl_ir)
      sctx->परिणाम =
        Curl_auth_create_plain_message(sctx->conn->creds, &sctx->resp);
    वापस TRUE;
  }
  वापस FALSE;
}

अटल bool sasl_choose_login(काष्ठा Curl_easy *तथ्य, काष्ठा sasl_ctx *sctx)
{
  अगर(sctx->enabledmechs & SASL_MECH_LOGIN) {
    sctx->mech = SASL_MECH_STRING_LOGIN;
    sctx->state1 = SASL_LOGIN;
    sctx->state2 = SASL_LOGIN_PASSWD;
    sctx->sasl->authused = SASL_MECH_LOGIN;

    अगर(sctx->sasl->force_ir || तथ्य->set.sasl_ir)
      Curl_auth_create_login_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    वापस TRUE;
  }
  वापस FALSE;
}

/*
 * Curl_sasl_start()
 *
 * Calculate the required login details for SASL authentication.
 */
CURLcode Curl_sasl_start(काष्ठा SASL *sasl, काष्ठा Curl_easy *तथ्य,
                         bool force_ir, saslprogress *progress)
{
  काष्ठा sasl_ctx sctx;

  sasl->force_ir = force_ir;    /* Latch for future use */
  sasl->authused = 0;           /* No mechanism used yet */
  *progress = SASL_IDLE;

  स_रखो(&sctx, 0, माप(sctx));
  sctx.sasl = sasl;
  sctx.conn = तथ्य->conn;
  Curl_bufref_init(&sctx.resp);
  sctx.enabledmechs = sasl->authmechs & sasl->prefmech;
  sctx.state1 = SASL_STOP;
  sctx.state2 = SASL_FINAL;

  /* Calculate the supported authentication mechanism, by decreasing order of
     security, as well as the initial response where appropriate */
  अगर(sasl_choose_external(तथ्य, &sctx) ||
#अगर_घोषित USE_KERBEROS5
     sasl_choose_krb5(तथ्य, &sctx) ||
#पूर्ण_अगर
#अगर_घोषित USE_GSASL
     sasl_choose_gsasl(तथ्य, &sctx) ||
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH
     sasl_choose_digest(तथ्य, &sctx) ||
#पूर्ण_अगर
#अगर_घोषित USE_NTLM
     sasl_choose_ntlm(तथ्य, &sctx) ||
#पूर्ण_अगर
     sasl_choose_oauth(तथ्य, &sctx) ||
     sasl_choose_oauth2(तथ्य, &sctx) ||
     sasl_choose_plain(तथ्य, &sctx) ||
     sasl_choose_login(तथ्य, &sctx)) {
    /* selected, either we have a mechanism or a failure */
    DEBUGASSERT(sctx.mech || sctx.परिणाम);
  }

  अगर(!sctx.परिणाम && sctx.mech) {
    sasl->curmech = sctx.mech;
    अगर(Curl_bufref_ptr(&sctx.resp))
      sctx.परिणाम = build_message(sasl, &sctx.resp);

    अगर(sasl->params->maxirlen &&
       म_माप(sctx.mech) + Curl_bufref_len(&sctx.resp) >
         sasl->params->maxirlen)
      Curl_bufref_free(&sctx.resp);

    अगर(!sctx.परिणाम)
      sctx.परिणाम = sasl->params->sendauth(तथ्य, sctx.mech, &sctx.resp);

    अगर(!sctx.परिणाम) {
      *progress = SASL_INPROGRESS;
      sasl_state(sasl, तथ्य, Curl_bufref_ptr(&sctx.resp) ?
                 sctx.state2 : sctx.state1);
    }
  }

  Curl_bufref_free(&sctx.resp);
  वापस sctx.परिणाम;
}

/*
 * Curl_sasl_continue()
 *
 * Continue the authentication.
 */
CURLcode Curl_sasl_continue(काष्ठा SASL *sasl, काष्ठा Curl_easy *तथ्य,
                            पूर्णांक code, saslprogress *progress)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;
  saslstate newstate = SASL_FINAL;
  काष्ठा bufref resp;
  स्थिर अक्षर *hostname;
  पूर्णांक port;
  काष्ठा bufref serverdata;

  Curl_conn_get_current_host(तथ्य, FIRSTSOCKET, &hostname, &port);
  Curl_bufref_init(&serverdata);
  Curl_bufref_init(&resp);
  *progress = SASL_INPROGRESS;

  अगर(sasl->state == SASL_FINAL) {
    अगर(code != sasl->params->finalcode)
      परिणाम = CURLE_LOGIN_DENIED;
    *progress = SASL_DONE;
    sasl_state(sasl, तथ्य, SASL_STOP);
    वापस परिणाम;
  }

  अगर(sasl->state != SASL_CANCEL && sasl->state != SASL_OAUTH2_RESP &&
     code != sasl->params->contcode) {
    *progress = SASL_DONE;
    sasl_state(sasl, तथ्य, SASL_STOP);
    वापस CURLE_LOGIN_DENIED;
  }

  चयन(sasl->state) {
  हाल SASL_STOP:
    *progress = SASL_DONE;
    वापस परिणाम;
  हाल SASL_PLAIN:
    परिणाम = Curl_auth_create_plain_message(conn->creds, &resp);
    अवरोध;
  हाल SASL_LOGIN:
    Curl_auth_create_login_message(Curl_creds_user(conn->creds), &resp);
    newstate = SASL_LOGIN_PASSWD;
    अवरोध;
  हाल SASL_LOGIN_PASSWD:
    Curl_auth_create_login_message(Curl_creds_passwd(conn->creds), &resp);
    अवरोध;
  हाल SASL_EXTERNAL:
    Curl_auth_create_external_message(Curl_creds_user(conn->creds), &resp);
    अवरोध;
#अगर_घोषित USE_GSASL
  हाल SASL_GSASL:
    परिणाम = get_server_message(sasl, तथ्य, &serverdata);
    अगर(!परिणाम) {
      काष्ठा gsasldata *gsasl = Curl_auth_gsasl_get(conn);
      परिणाम = !gsasl ? CURLE_OUT_OF_MEMORY :
        Curl_auth_gsasl_token(तथ्य, &serverdata, gsasl, &resp);
    }
    अगर(!परिणाम && Curl_bufref_len(&resp) > 0)
      newstate = SASL_GSASL;
    अवरोध;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH
  हाल SASL_CRAMMD5:
    परिणाम = get_server_message(sasl, तथ्य, &serverdata);
    अगर(!परिणाम)
      परिणाम = Curl_auth_create_cram_md5_message(&serverdata, conn->creds,
                                                 &resp);
    अवरोध;
  हाल SASL_DIGESTMD5:
    परिणाम = get_server_message(sasl, तथ्य, &serverdata);
    अगर(!परिणाम)
      परिणाम = Curl_auth_create_digest_md5_message(तथ्य, &serverdata,
                                                   conn->creds,
                                                   sasl->params->service,
                                                   &resp);
    अगर(!परिणाम && (sasl->params->flags & SASL_FLAG_BASE64))
      newstate = SASL_DIGESTMD5_RESP;
    अवरोध;
  हाल SASL_DIGESTMD5_RESP:
    /* Keep response NULL to output an empty line. */
    अवरोध;
#पूर्ण_अगर

#अगर_घोषित USE_NTLM
  हाल SASL_NTLM: {
    /* Create the type-1 message */
    काष्ठा ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    परिणाम = !ntlm ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_ntlm_type1_message(तथ्य, conn->creds,
                                          sasl->params->service, hostname,
                                          ntlm, &resp);
    newstate = SASL_NTLM_TYPE2_संदेश;
    अवरोध;
  }
  हाल SASL_NTLM_TYPE2_संदेश: {
    /* Decode the type-2 message */
    काष्ठा ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    परिणाम = !ntlm ? CURLE_OUT_OF_MEMORY :
      get_server_message(sasl, तथ्य, &serverdata);
    अगर(!परिणाम)
      परिणाम = Curl_auth_decode_ntlm_type2_message(तथ्य, &serverdata, ntlm);
    अगर(!परिणाम)
      परिणाम = Curl_auth_create_ntlm_type3_message(तथ्य, conn->creds,
                                                   ntlm, &resp);
    अवरोध;
  }
#पूर्ण_अगर

#अगर_घोषित USE_KERBEROS5
  हाल SASL_GSSAPI: {
    काष्ठा kerberos5data *krb5 = Curl_auth_krb5_get(conn);
    परिणाम = !krb5 ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_gssapi_user_message(तथ्य, conn->creds,
                                           sasl->params->service,
                                           conn->origin->hostname,
                                           (bool)sasl->mutual_auth, शून्य,
                                           krb5, &resp);
    newstate = SASL_GSSAPI_TOKEN;
    अवरोध;
  }
  हाल SASL_GSSAPI_TOKEN:
    परिणाम = get_server_message(sasl, तथ्य, &serverdata);
    अगर(!परिणाम) {
      काष्ठा kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      अगर(!krb5)
        परिणाम = CURLE_OUT_OF_MEMORY;
      अन्यथा अगर(sasl->mutual_auth) {
        /* Decode the user token challenge and create the optional response
           message */
        परिणाम = Curl_auth_create_gssapi_user_message(तथ्य, शून्य,
                                                      शून्य, शून्य,
                                                      (bool)sasl->mutual_auth,
                                                      &serverdata,
                                                      krb5, &resp);
        newstate = SASL_GSSAPI_NO_तथ्य;
      }
      अन्यथा
        /* Decode the security challenge and create the response message */
        परिणाम = Curl_auth_create_gssapi_security_message(
          तथ्य, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    अवरोध;
  हाल SASL_GSSAPI_NO_तथ्य:
    /* Decode the security challenge and create the response message */
    परिणाम = get_server_message(sasl, तथ्य, &serverdata);
    अगर(!परिणाम) {
      काष्ठा kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      अगर(!krb5)
        परिणाम = CURLE_OUT_OF_MEMORY;
      अन्यथा
        परिणाम = Curl_auth_create_gssapi_security_message(
          तथ्य, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    अवरोध;
#पूर्ण_अगर

  हाल SASL_OAUTH2:
    /* Create the authorization message */
    अगर(sasl->authused == SASL_MECH_OAUTHBEARER) {
      परिणाम = Curl_auth_create_oauth_bearer_message(conn->creds,
                                                     hostname,
                                                     port,
                                                     &resp);

      /* Failures maybe sent by the server as continuations for OAUTHBEARER */
      newstate = SASL_OAUTH2_RESP;
    }
    अन्यथा
      परिणाम = Curl_auth_create_xoauth_bearer_message(conn->creds,
                                                      &resp);
    अवरोध;

  हाल SASL_OAUTH2_RESP:
    /* The continuation is optional so check the response code */
    अगर(code == sasl->params->finalcode) {
      /* Final response was received so we are done */
      *progress = SASL_DONE;
      sasl_state(sasl, तथ्य, SASL_STOP);
      वापस परिणाम;
    }
    अन्यथा अगर(code == sasl->params->contcode) {
      /* Acknowledge the continuation by sending a 0x01 response. */
      Curl_bufref_set(&resp, "\x01", 1, शून्य);
      अवरोध;
    }
    अन्यथा {
      *progress = SASL_DONE;
      sasl_state(sasl, तथ्य, SASL_STOP);
      वापस CURLE_LOGIN_DENIED;
    }

  हाल SASL_CANCEL:
    /* Remove the offending mechanism from the supported list */
    sasl->authmechs &= (अचिन्हित लघु)~sasl->authused;
    sasl->authused = SASL_AUTH_NONE;
    sasl->curmech = शून्य;

    /* Start an alternative SASL authentication */
    वापस Curl_sasl_start(sasl, तथ्य, (bool)sasl->force_ir, progress);
  शेष:
    failf(तथ्य, "Unsupported SASL authentication mechanism");
    परिणाम = CURLE_UNSUPPORTED_PROTOCOL;  /* Should not happen */
    अवरोध;
  }

  Curl_bufref_free(&serverdata);

  चयन(परिणाम) {
  हाल CURLE_BAD_CONTENT_ENCODING:
    /* Cancel dialog */
    परिणाम = sasl->params->cancelauth(तथ्य, sasl->curmech);
    newstate = SASL_CANCEL;
    अवरोध;
  हाल CURLE_OK:
    परिणाम = build_message(sasl, &resp);
    अगर(!परिणाम)
      परिणाम = sasl->params->contauth(तथ्य, sasl->curmech, &resp);
    अवरोध;
  शेष:
    newstate = SASL_STOP;    /* Stop on error */
    *progress = SASL_DONE;
    अवरोध;
  }

  Curl_bufref_free(&resp);

  sasl_state(sasl, तथ्य, newstate);

  वापस परिणाम;
}

#अगर_घोषित CURLVERBOSE
अटल व्योम sasl_unchosen(काष्ठा Curl_easy *तथ्य, अचिन्हित लघु mech,
                          अचिन्हित लघु enabledmechs,
                          bool built_in, bool platform,
                          स्थिर अक्षर *param_missing)
{
  स्थिर अक्षर *mname = शून्य;
  माप_प्रकार i;

  अगर(!(enabledmechs & mech))
    वापस;

  क्रम(i = 0; mechtable[i].name; ++i) {
    अगर(mechtable[i].bit == mech) {
      mname = mechtable[i].name;
      अवरोध;
    }
  }
  अगर(!mname)  /* should not happen */
    वापस;
  अगर(!built_in)
    infof(तथ्य, "SASL: %s not builtin", mname);
  अन्यथा अगर(!platform)
    infof(तथ्य, "SASL: %s not supported by the platform/libraries", mname);
  अन्यथा {
    अगर(param_missing)
      infof(तथ्य, "SASL: %s is missing %s", mname, param_missing);
    अगर(!Curl_creds_has_user(तथ्य->conn->creds))
      infof(तथ्य, "SASL: %s is missing username", mname);
  }
}
#पूर्ण_अगर /* CURLVERBOSE */

CURLcode Curl_sasl_is_blocked(काष्ठा SASL *sasl, काष्ठा Curl_easy *तथ्य)
{
#अगर_घोषित CURLVERBOSE
#अगर_घोषित USE_KERBEROS5
#घोषणा CURL_SASL_KERBEROS5   TRUE
#अन्यथा
#घोषणा CURL_SASL_KERBEROS5   FALSE
#पूर्ण_अगर
#अगर_घोषित USE_GSASL
#घोषणा CURL_SASL_GASL        TRUE
#अन्यथा
#घोषणा CURL_SASL_GASL        FALSE
#पूर्ण_अगर
#अगर_घोषित CURL_DISABLE_DIGEST_AUTH
#घोषणा CURL_SASL_DIGEST      TRUE
#अन्यथा
#घोषणा CURL_SASL_DIGEST      FALSE
#पूर्ण_अगर
#अगर_अघोषित USE_NTLM
#घोषणा CURL_SASL_NTLM        TRUE
#अन्यथा
#घोषणा CURL_SASL_NTLM        FALSE
#पूर्ण_अगर
  /* Failing SASL authentication is a pain. Give a helping hand if
   * we were unable to select an AUTH mechanism.
   * `sasl->authmechs` are mechanisms offered by the peer
   * `sasl->prefmech`  are mechanisms preferred by us */
  अचिन्हित लघु enabledmechs = sasl->authmechs & sasl->prefmech;

  अगर(!sasl->authmechs)
    infof(तथ्य, "SASL: no auth mechanism was offered or recognized");
  अन्यथा अगर(!enabledmechs)
    infof(तथ्य, "SASL: no overlap between offered and configured "
          "auth mechanisms");
  अन्यथा {
    infof(तथ्य, "SASL: no auth mechanism offered could be selected");
    अगर((enabledmechs & SASL_MECH_EXTERNAL) &&
       Curl_creds_has_passwd(तथ्य->conn->creds))
      infof(तथ्य, "SASL: auth EXTERNAL not chosen with password");
    sasl_unchosen(तथ्य, SASL_MECH_GSSAPI, enabledmechs,
                  CURL_SASL_KERBEROS5, Curl_auth_is_gssapi_supported(), शून्य);
    sasl_unchosen(तथ्य, SASL_MECH_SCRAM_SHA_256, enabledmechs,
                  CURL_SASL_GASL, FALSE, शून्य);
    sasl_unchosen(तथ्य, SASL_MECH_SCRAM_SHA_1, enabledmechs,
                  CURL_SASL_GASL, FALSE, शून्य);
    sasl_unchosen(तथ्य, SASL_MECH_DIGEST_MD5, enabledmechs,
                  CURL_SASL_DIGEST, Curl_auth_is_digest_supported(), शून्य);
    sasl_unchosen(तथ्य, SASL_MECH_CRAM_MD5, enabledmechs,
                  CURL_SASL_DIGEST, TRUE, शून्य);
    sasl_unchosen(तथ्य, SASL_MECH_NTLM, enabledmechs,
                  CURL_SASL_NTLM, Curl_auth_is_ntlm_supported(), शून्य);
    sasl_unchosen(तथ्य, SASL_MECH_OAUTHBEARER, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(तथ्य->conn->creds) ?
                  शून्य : "CURLOPT_XOAUTH2_BEARER");
    sasl_unchosen(तथ्य, SASL_MECH_XOAUTH2, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(तथ्य->conn->creds) ?
                  शून्य : "CURLOPT_XOAUTH2_BEARER");
  }
#पूर्ण_अगर /* CURLVERBOSE */
  (व्योम)sasl;
  (व्योम)तथ्य;
  वापस CURLE_LOGIN_DENIED;
}

#पूर्ण_अगर /* protocols are enabled that use SASL */
