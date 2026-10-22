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
#अन्तर्भाव "curl_setup.h"

#यदि !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curlx/base64.h"
#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "curl_sasl.h"
#अन्तर्भाव "curl_trc.h"

/* Supported mechanisms */
स्थिर ध्रुव संरचना {
  ध्रुव वर्ण    *name;  /* Name */
  size_t         len;   /* Name length */
  अचिह्नित ह्रस्व bit;   /* Flag bit */
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
अचिह्नित ह्रस्व Curl_sasl_decode_mech(ध्रुव वर्ण *ptr, size_t maxlen,
                                     size_t *len)
{
  अचिह्नित पूर्णाङ्क i;
  वर्ण c;

  प्रति(i = 0; mechtable[i].name; i++) {
    यदि(maxlen >= mechtable[i].len &&
       curl_strnequal(ptr, mechtable[i].name, mechtable[i].len)) {
      यदि(len)
        *len = mechtable[i].len;

      यदि(maxlen == mechtable[i].len)
        प्रत्यागम mechtable[i].bit;

      c = ptr[mechtable[i].len];
      यदि(!ISUPPER(c) && !ISDIGIT(c) && c != '-' && c != '_')
        प्रत्यागम mechtable[i].bit;
    }
  }

  प्रत्यागम 0;
}

/*
 * Curl_sasl_parse_url_auth_option()
 *
 * Parse the URL login options.
 */
CURLcode Curl_sasl_parse_url_auth_option(संरचना SASL *sasl,
                                         ध्रुव वर्ण *मानम्, size_t len)
{
  CURLcode परिणाम = CURLE_OK;
  size_t mechlen;

  यदि(!len)
    प्रत्यागम CURLE_URL_MALFORMAT;

  यदि(sasl->resetprefs) {
    sasl->resetprefs = FALSE;
    sasl->prefmech = SASL_AUTH_NONE;
  }

  यदि(!वाक्ये_तुलय(मानम्, "*", len))
    sasl->prefmech = SASL_AUTH_DEFAULT;
  अन्यथा {
    अचिह्नित ह्रस्व mechbit = Curl_sasl_decode_mech(मानम्, len, &mechlen);
    यदि(mechbit && mechlen == len)
      sasl->prefmech |= mechbit;
    अन्यथा
      परिणाम = CURLE_URL_MALFORMAT;
  }

  प्रत्यागम परिणाम;
}

/*
 * Curl_sasl_init()
 *
 * Initializes the SASL structure.
 */
शून्य Curl_sasl_init(संरचना SASL *sasl, संरचना Curl_easy *दत्त,
                    ध्रुव संरचना SASLproto *params)
{
  uint32_t auth = दत्त->समुच्चयः.httpauth;

  sasl->params = params;           /* Set protocol dependent parameters */
  sasl->state = SASL_STOP;         /* Not yet running */
  sasl->curmech = रिक्त;            /* No mechanism yet. */
  sasl->authmechs = SASL_AUTH_NONE; /* No known authentication mechanism yet */
  sasl->prefmech = params->defmechs; /* Default preferred mechanisms */
  sasl->authused = SASL_AUTH_NONE; /* The authentication mechanism used */
  sasl->resetprefs = TRUE;         /* Reset prefmech upon AUTH parsing. */
  sasl->mutual_auth = FALSE;       /* No mutual authentication (GSSAPI only) */
  sasl->force_ir = FALSE;          /* Respect external option */

  यदि(auth != CURLAUTH_BASIC) {
    अचिह्नित ह्रस्व mechs = SASL_AUTH_NONE;

    /* If some usable http authentication options have been set, determine
       new defaults from them. */
    यदि(auth & CURLAUTH_BASIC)
      mechs |= SASL_MECH_PLAIN | SASL_MECH_LOGIN;
    यदि(auth & CURLAUTH_DIGEST)
      mechs |= SASL_MECH_DIGEST_MD5;
    यदि(auth & CURLAUTH_NTLM)
      mechs |= SASL_MECH_NTLM;
    यदि(auth & CURLAUTH_BEARER)
      mechs |= SASL_MECH_OAUTHBEARER | SASL_MECH_XOAUTH2;
    यदि(auth & CURLAUTH_GSSAPI)
      mechs |= SASL_MECH_GSSAPI;

    यदि(mechs != SASL_AUTH_NONE)
      sasl->prefmech = mechs;
  }
}

/*
 * sasl_state()
 *
 * This is the ONLY way to change SASL state!
 */
स्थिर शून्य sasl_state(संरचना SASL *sasl, संरचना Curl_easy *दत्त,
                       saslstate newstate)
{
#यदि defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  स्थिर ध्रुव वर्ण * ध्रुव नामानि[] = {
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
    /*चरमः*/
  };

  यदि(sasl->state != newstate)
    infof(दत्त, "SASL %p state change from %s to %s",
          (शून्य *)sasl, नामानि[sasl->state], नामानि[newstate]);
#अन्यथा
  (शून्य)दत्त;
#यदि_अन्तः

  sasl->state = newstate;
}

#यदि defined(USE_NTLM) || defined(USE_GSASL) || defined(USE_KERBEROS5) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)
/* Get the SASL server message and convert it to binary. */
स्थिर CURLcode get_server_सन्देशः(संरचना SASL *sasl, संरचना Curl_easy *दत्त,
                                   संरचना bufref *बहिर्गमनम्)
{
  CURLcode परिणाम = CURLE_OK;

  परिणाम = sasl->params->getmessage(दत्त, बहिर्गमनम्);
  यदि(!परिणाम && (sasl->params->flags & SASL_FLAG_BASE64)) {
    ध्रुव वर्ण *serverdata = Curl_bufref_ptr(बहिर्गमनम्);

    यदि(!*serverdata)
      Curl_bufref_समुच्चयः(बहिर्गमनम्, रिक्त, 0, रिक्त);
    अन्यथा {
      अचिह्नित वर्ण *सन्देशः;
      size_t msglen;

      परिणाम = curlx_base64_decode(serverdata, &सन्देशः, &msglen);
      यदि(!परिणाम)
        Curl_bufref_समुच्चयः(बहिर्गमनम्, सन्देशः, msglen, curl_free);
    }
  }
  प्रत्यागम परिणाम;
}
#यदि_अन्तः

/* Encode the outgoing SASL message. */
स्थिर CURLcode build_सन्देशः(संरचना SASL *sasl, संरचना bufref *सन्देशः)
{
  CURLcode परिणाम = CURLE_OK;

  यदि(sasl->params->flags & SASL_FLAG_BASE64) {
    यदि(!Curl_bufref_ptr(सन्देशः))                   /* Empty message. */
      Curl_bufref_समुच्चयः(सन्देशः, "", 0, रिक्त);
    अन्यथा यदि(Curl_bufref_len(सन्देशः)) {
      वर्ण *base64;
      size_t base64len;

      परिणाम = curlx_base64_encode(Curl_bufref_uptr(सन्देशः),
                                   Curl_bufref_len(सन्देशः), &base64, &base64len);
      यदि(!परिणाम)
        Curl_bufref_समुच्चयः(सन्देशः, base64, base64len, curl_free);
    }
  }

  प्रत्यागम परिणाम;
}

/*
 * Curl_sasl_can_authenticate()
 *
 * Check if we have enough auth data and capabilities to authenticate.
 */
bool Curl_sasl_can_authenticate(संरचना SASL *sasl, संरचना Curl_easy *दत्त)
{
  /* Have credentials been provided? */
  यदि(दत्त->conn->creds)
    प्रत्यागम TRUE;

  /* EXTERNAL can authenticate without a username and/or password */
  यदि(sasl->authmechs & sasl->prefmech & SASL_MECH_EXTERNAL)
    प्रत्यागम TRUE;

  प्रत्यागम FALSE;
}

संरचना sasl_ctx {
  संरचना SASL *sasl;
  संरचना connectdata *conn;
  अचिह्नित ह्रस्व enabledmechs;
  ध्रुव वर्ण *mech;
  saslstate state1;
  saslstate state2;
  संरचना bufref resp;
  CURLcode परिणाम;
};

स्थिर bool sasl_वृणु_external(संरचना Curl_easy *दत्त, संरचना sasl_ctx *sctx)
{
  यदि((sctx->enabledmechs & SASL_MECH_EXTERNAL) &&
     !Curl_creds_has_passwd(sctx->conn->creds)) {
    sctx->mech = SASL_MECH_STRING_EXTERNAL;
    sctx->state1 = SASL_EXTERNAL;
    sctx->sasl->authused = SASL_MECH_EXTERNAL;

    यदि(sctx->sasl->force_ir || दत्त->समुच्चयः.sasl_ir)
      Curl_auth_create_external_सन्देशः(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

#यदि_परिभाषितम् USE_KERBEROS5
स्थिर bool sasl_वृणु_krb5(संरचना Curl_easy *दत्त, संरचना sasl_ctx *sctx)
{
  यदि((sctx->enabledmechs & SASL_MECH_GSSAPI) &&
     Curl_auth_is_gssapi_supported() &&
     Curl_auth_user_अन्तर्भवति_domain(sctx->conn->creds)) {
    ध्रुव वर्ण *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;

    sctx->sasl->mutual_auth = FALSE;
    sctx->mech = SASL_MECH_STRING_GSSAPI;
    sctx->state1 = SASL_GSSAPI;
    sctx->state2 = SASL_GSSAPI_TOKEN;
    sctx->sasl->authused = SASL_MECH_GSSAPI;

    यदि(sctx->sasl->force_ir || दत्त->समुच्चयः.sasl_ir) {
      संरचना kerberos5data *krb5 = Curl_auth_krb5_get(sctx->conn);
      sctx->परिणाम = !krb5 ? CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः :
        Curl_auth_create_gssapi_user_सन्देशः(दत्त, sctx->conn->creds,
                                             service,
                                             sctx->conn->origin->hostname,
                                             (bool)sctx->sasl->mutual_auth,
                                             रिक्त, krb5, &sctx->resp);
    }
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}
#यदि_अन्तः /* USE_KERBEROS5 */

#यदि_परिभाषितम् USE_GSASL
स्थिर bool sasl_वृणु_gsasl(संरचना Curl_easy *दत्त, संरचना sasl_ctx *sctx)
{
  संरचना gsasldata *gsasl;
  संरचना bufref nullmsg;

  यदि((sctx->enabledmechs &
      (SASL_MECH_SCRAM_SHA_256 | SASL_MECH_SCRAM_SHA_1))) {
    gsasl = Curl_auth_gsasl_get(sctx->conn);
    यदि(!gsasl) {
      sctx->परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      प्रत्यागम TRUE; /* attempted, but failed */
    }

    यदि((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_256) &&
       Curl_auth_gsasl_is_supported(दत्त, SASL_MECH_STRING_SCRAM_SHA_256,
                                    gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_256;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_256;
    }
    अन्यथा यदि((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_1) &&
            Curl_auth_gsasl_is_supported(दत्त, SASL_MECH_STRING_SCRAM_SHA_1,
                                         gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_1;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_1;
    }
    अन्यथा
      प्रत्यागम FALSE;

    Curl_bufref_init(&nullmsg);
    sctx->state1 = SASL_GSASL;
    sctx->state2 = SASL_GSASL;
    sctx->परिणाम = Curl_auth_gsasl_start(दत्त, sctx->conn->creds, gsasl);
    यदि(!sctx->परिणाम && (sctx->sasl->force_ir || दत्त->समुच्चयः.sasl_ir))
      sctx->परिणाम = Curl_auth_gsasl_token(दत्त, &nullmsg, gsasl, &sctx->resp);
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

#यदि_अन्तः /* USE_GSASL */

#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH
स्थिर bool sasl_वृणु_digest(संरचना Curl_easy *दत्त, संरचना sasl_ctx *sctx)
{
  (शून्य)दत्त;
  यदि((sctx->enabledmechs & SASL_MECH_DIGEST_MD5) &&
     Curl_auth_is_digest_supported()) {
    sctx->mech = SASL_MECH_STRING_DIGEST_MD5;
    sctx->state1 = SASL_DIGESTMD5;
    sctx->sasl->authused = SASL_MECH_DIGEST_MD5;
    प्रत्यागम TRUE;
  }
  अन्यथा यदि(sctx->enabledmechs & SASL_MECH_CRAM_MD5) {
    sctx->mech = SASL_MECH_STRING_CRAM_MD5;
    sctx->state1 = SASL_CRAMMD5;
    sctx->sasl->authused = SASL_MECH_CRAM_MD5;
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}
#यदि_अन्तः /* !CURL_DISABLE_DIGEST_AUTH */

#यदि_परिभाषितम् USE_NTLM
स्थिर bool sasl_वृणु_ntlm(संरचना Curl_easy *दत्त, संरचना sasl_ctx *sctx)
{
  यदि((sctx->enabledmechs & SASL_MECH_NTLM) &&
     Curl_auth_is_ntlm_supported()) {
     ध्रुव वर्ण *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;
    ध्रुव वर्ण *hostname;

    Curl_conn_get_current_host(दत्त, FIRSTSOCKET, &hostname, रिक्त);

    sctx->mech = SASL_MECH_STRING_NTLM;
    sctx->state1 = SASL_NTLM;
    sctx->state2 = SASL_NTLM_TYPE2_सन्देशः;
    sctx->sasl->authused = SASL_MECH_NTLM;

    यदि(sctx->sasl->force_ir || दत्त->समुच्चयः.sasl_ir) {
      संरचना ntlmdata *ntlm = Curl_auth_ntlm_get(sctx->conn, FALSE);
      sctx->परिणाम = !ntlm ? CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः :
        Curl_auth_create_ntlm_type1_सन्देशः(दत्त, sctx->conn->creds,
                                            service, hostname,
                                            ntlm, &sctx->resp);
    }
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}
#यदि_अन्तः /* USE_NTLM */

स्थिर bool sasl_वृणु_oauth(संरचना Curl_easy *दत्त, संरचना sasl_ctx *sctx)
{
  यदि(Curl_creds_has_oauth_bearer(दत्त->state.creds) &&
     (sctx->enabledmechs & SASL_MECH_OAUTHBEARER)) {
    ध्रुव वर्ण *hostname;
    पूर्णाङ्क पत्तनम्;
    Curl_conn_get_current_host(दत्त, FIRSTSOCKET, &hostname, &पत्तनम्);

    sctx->mech = SASL_MECH_STRING_OAUTHBEARER;
    sctx->state1 = SASL_OAUTH2;
    sctx->state2 = SASL_OAUTH2_RESP;
    sctx->sasl->authused = SASL_MECH_OAUTHBEARER;

    यदि(sctx->sasl->force_ir || दत्त->समुच्चयः.sasl_ir)
      sctx->परिणाम =
        Curl_auth_create_oauth_bearer_सन्देशः(sctx->conn->creds,
                                              hostname, पत्तनम्, &sctx->resp);
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

स्थिर bool sasl_वृणु_oauth2(संरचना Curl_easy *दत्त, संरचना sasl_ctx *sctx)
{
  यदि(Curl_creds_has_oauth_bearer(sctx->conn->creds) &&
     (sctx->enabledmechs & SASL_MECH_XOAUTH2)) {
    sctx->mech = SASL_MECH_STRING_XOAUTH2;
    sctx->state1 = SASL_OAUTH2;
    sctx->sasl->authused = SASL_MECH_XOAUTH2;

    यदि(sctx->sasl->force_ir || दत्त->समुच्चयः.sasl_ir)
      sctx->परिणाम = Curl_auth_create_xoauth_bearer_सन्देशः(
        sctx->conn->creds, &sctx->resp);
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

स्थिर bool sasl_वृणु_plain(संरचना Curl_easy *दत्त, संरचना sasl_ctx *sctx)
{
  यदि(sctx->enabledmechs & SASL_MECH_PLAIN) {
    sctx->mech = SASL_MECH_STRING_PLAIN;
    sctx->state1 = SASL_PLAIN;
    sctx->sasl->authused = SASL_MECH_PLAIN;

    यदि(sctx->sasl->force_ir || दत्त->समुच्चयः.sasl_ir)
      sctx->परिणाम =
        Curl_auth_create_plain_सन्देशः(sctx->conn->creds, &sctx->resp);
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

स्थिर bool sasl_वृणु_login(संरचना Curl_easy *दत्त, संरचना sasl_ctx *sctx)
{
  यदि(sctx->enabledmechs & SASL_MECH_LOGIN) {
    sctx->mech = SASL_MECH_STRING_LOGIN;
    sctx->state1 = SASL_LOGIN;
    sctx->state2 = SASL_LOGIN_PASSWD;
    sctx->sasl->authused = SASL_MECH_LOGIN;

    यदि(sctx->sasl->force_ir || दत्त->समुच्चयः.sasl_ir)
      Curl_auth_create_login_सन्देशः(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

/*
 * Curl_sasl_start()
 *
 * Calculate the required login details for SASL authentication.
 */
CURLcode Curl_sasl_start(संरचना SASL *sasl, संरचना Curl_easy *दत्त,
                         bool force_ir, saslprogress *प्रगतिः)
{
  संरचना sasl_ctx sctx;

  sasl->force_ir = force_ir;    /* Latch for future use */
  sasl->authused = 0;           /* No mechanism used yet */
  *प्रगतिः = SASL_IDLE;

  स्मृतिं_पूरय(&sctx, 0, परिमाण(sctx));
  sctx.sasl = sasl;
  sctx.conn = दत्त->conn;
  Curl_bufref_init(&sctx.resp);
  sctx.enabledmechs = sasl->authmechs & sasl->prefmech;
  sctx.state1 = SASL_STOP;
  sctx.state2 = SASL_FINAL;

  /* Calculate the supported authentication mechanism, by decreasing order of
     security, as well as the initial response where appropriate */
  यदि(sasl_वृणु_external(दत्त, &sctx) ||
#यदि_परिभाषितम् USE_KERBEROS5
     sasl_वृणु_krb5(दत्त, &sctx) ||
#यदि_अन्तः
#यदि_परिभाषितम् USE_GSASL
     sasl_वृणु_gsasl(दत्त, &sctx) ||
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH
     sasl_वृणु_digest(दत्त, &sctx) ||
#यदि_अन्तः
#यदि_परिभाषितम् USE_NTLM
     sasl_वृणु_ntlm(दत्त, &sctx) ||
#यदि_अन्तः
     sasl_वृणु_oauth(दत्त, &sctx) ||
     sasl_वृणु_oauth2(दत्त, &sctx) ||
     sasl_वृणु_plain(दत्त, &sctx) ||
     sasl_वृणु_login(दत्त, &sctx)) {
    /* selected, either we have a mechanism or a failure */
    DEBUGASSERT(sctx.mech || sctx.परिणाम);
  }

  यदि(!sctx.परिणाम && sctx.mech) {
    sasl->curmech = sctx.mech;
    यदि(Curl_bufref_ptr(&sctx.resp))
      sctx.परिणाम = build_सन्देशः(sasl, &sctx.resp);

    यदि(sasl->params->maxirlen &&
       वाक्यदैर्घ्यम्(sctx.mech) + Curl_bufref_len(&sctx.resp) >
         sasl->params->maxirlen)
      Curl_bufref_free(&sctx.resp);

    यदि(!sctx.परिणाम)
      sctx.परिणाम = sasl->params->sendauth(दत्त, sctx.mech, &sctx.resp);

    यदि(!sctx.परिणाम) {
      *प्रगतिः = SASL_INPROGRESS;
      sasl_state(sasl, दत्त, Curl_bufref_ptr(&sctx.resp) ?
                 sctx.state2 : sctx.state1);
    }
  }

  Curl_bufref_free(&sctx.resp);
  प्रत्यागम sctx.परिणाम;
}

/*
 * Curl_sasl_continue()
 *
 * Continue the authentication.
 */
CURLcode Curl_sasl_continue(संरचना SASL *sasl, संरचना Curl_easy *दत्त,
                            पूर्णाङ्क संहिता, saslprogress *प्रगतिः)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;
  saslstate newstate = SASL_FINAL;
  संरचना bufref resp;
  ध्रुव वर्ण *hostname;
  पूर्णाङ्क पत्तनम्;
  संरचना bufref serverdata;

  Curl_conn_get_current_host(दत्त, FIRSTSOCKET, &hostname, &पत्तनम्);
  Curl_bufref_init(&serverdata);
  Curl_bufref_init(&resp);
  *प्रगतिः = SASL_INPROGRESS;

  यदि(sasl->state == SASL_FINAL) {
    यदि(संहिता != sasl->params->finalcode)
      परिणाम = CURLE_LOGIN_DENIED;
    *प्रगतिः = SASL_समाप्तम्;
    sasl_state(sasl, दत्त, SASL_STOP);
    प्रत्यागम परिणाम;
  }

  यदि(sasl->state != SASL_CANCEL && sasl->state != SASL_OAUTH2_RESP &&
     संहिता != sasl->params->contcode) {
    *प्रगतिः = SASL_समाप्तम्;
    sasl_state(sasl, दत्त, SASL_STOP);
    प्रत्यागम CURLE_LOGIN_DENIED;
  }

  विकल्प(sasl->state) {
  पक्ष SASL_STOP:
    *प्रगतिः = SASL_समाप्तम्;
    प्रत्यागम परिणाम;
  पक्ष SASL_PLAIN:
    परिणाम = Curl_auth_create_plain_सन्देशः(conn->creds, &resp);
    भङ्ग;
  पक्ष SASL_LOGIN:
    Curl_auth_create_login_सन्देशः(Curl_creds_user(conn->creds), &resp);
    newstate = SASL_LOGIN_PASSWD;
    भङ्ग;
  पक्ष SASL_LOGIN_PASSWD:
    Curl_auth_create_login_सन्देशः(Curl_creds_passwd(conn->creds), &resp);
    भङ्ग;
  पक्ष SASL_EXTERNAL:
    Curl_auth_create_external_सन्देशः(Curl_creds_user(conn->creds), &resp);
    भङ्ग;
#यदि_परिभाषितम् USE_GSASL
  पक्ष SASL_GSASL:
    परिणाम = get_server_सन्देशः(sasl, दत्त, &serverdata);
    यदि(!परिणाम) {
      संरचना gsasldata *gsasl = Curl_auth_gsasl_get(conn);
      परिणाम = !gsasl ? CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः :
        Curl_auth_gsasl_token(दत्त, &serverdata, gsasl, &resp);
    }
    यदि(!परिणाम && Curl_bufref_len(&resp) > 0)
      newstate = SASL_GSASL;
    भङ्ग;
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH
  पक्ष SASL_CRAMMD5:
    परिणाम = get_server_सन्देशः(sasl, दत्त, &serverdata);
    यदि(!परिणाम)
      परिणाम = Curl_auth_create_cram_md5_सन्देशः(&serverdata, conn->creds,
                                                 &resp);
    भङ्ग;
  पक्ष SASL_DIGESTMD5:
    परिणाम = get_server_सन्देशः(sasl, दत्त, &serverdata);
    यदि(!परिणाम)
      परिणाम = Curl_auth_create_digest_md5_सन्देशः(दत्त, &serverdata,
                                                   conn->creds,
                                                   sasl->params->service,
                                                   &resp);
    यदि(!परिणाम && (sasl->params->flags & SASL_FLAG_BASE64))
      newstate = SASL_DIGESTMD5_RESP;
    भङ्ग;
  पक्ष SASL_DIGESTMD5_RESP:
    /* Keep response NULL to output an empty line. */
    भङ्ग;
#यदि_अन्तः

#यदि_परिभाषितम् USE_NTLM
  पक्ष SASL_NTLM: {
    /* Create the type-1 message */
    संरचना ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    परिणाम = !ntlm ? CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः :
      Curl_auth_create_ntlm_type1_सन्देशः(दत्त, conn->creds,
                                          sasl->params->service, hostname,
                                          ntlm, &resp);
    newstate = SASL_NTLM_TYPE2_सन्देशः;
    भङ्ग;
  }
  पक्ष SASL_NTLM_TYPE2_सन्देशः: {
    /* Decode the type-2 message */
    संरचना ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    परिणाम = !ntlm ? CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः :
      get_server_सन्देशः(sasl, दत्त, &serverdata);
    यदि(!परिणाम)
      परिणाम = Curl_auth_decode_ntlm_type2_सन्देशः(दत्त, &serverdata, ntlm);
    यदि(!परिणाम)
      परिणाम = Curl_auth_create_ntlm_type3_सन्देशः(दत्त, conn->creds,
                                                   ntlm, &resp);
    भङ्ग;
  }
#यदि_अन्तः

#यदि_परिभाषितम् USE_KERBEROS5
  पक्ष SASL_GSSAPI: {
    संरचना kerberos5data *krb5 = Curl_auth_krb5_get(conn);
    परिणाम = !krb5 ? CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः :
      Curl_auth_create_gssapi_user_सन्देशः(दत्त, conn->creds,
                                           sasl->params->service,
                                           conn->origin->hostname,
                                           (bool)sasl->mutual_auth, रिक्त,
                                           krb5, &resp);
    newstate = SASL_GSSAPI_TOKEN;
    भङ्ग;
  }
  पक्ष SASL_GSSAPI_TOKEN:
    परिणाम = get_server_सन्देशः(sasl, दत्त, &serverdata);
    यदि(!परिणाम) {
      संरचना kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      यदि(!krb5)
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      अन्यथा यदि(sasl->mutual_auth) {
        /* Decode the user token challenge and create the optional response
सन्देशः*/
        परिणाम = Curl_auth_create_gssapi_user_सन्देशः(दत्त, रिक्त,
                                                      रिक्त, रिक्त,
                                                      (bool)sasl->mutual_auth,
                                                      &serverdata,
                                                      krb5, &resp);
        newstate = SASL_GSSAPI_NO_दत्त;
      }
      अन्यथा
        /* Decode the security challenge and create the response message */
        परिणाम = Curl_auth_create_gssapi_security_सन्देशः(
          दत्त, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    भङ्ग;
  पक्ष SASL_GSSAPI_NO_दत्त:
    /* Decode the security challenge and create the response message */
    परिणाम = get_server_सन्देशः(sasl, दत्त, &serverdata);
    यदि(!परिणाम) {
      संरचना kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      यदि(!krb5)
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      अन्यथा
        परिणाम = Curl_auth_create_gssapi_security_सन्देशः(
          दत्त, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    भङ्ग;
#यदि_अन्तः

  पक्ष SASL_OAUTH2:
    /* Create the authorization message */
    यदि(sasl->authused == SASL_MECH_OAUTHBEARER) {
      परिणाम = Curl_auth_create_oauth_bearer_सन्देशः(conn->creds,
                                                     hostname,
                                                     पत्तनम्,
                                                     &resp);

      /* Failures maybe sent by the server as continuations for OAUTHBEARER */
      newstate = SASL_OAUTH2_RESP;
    }
    अन्यथा
      परिणाम = Curl_auth_create_xoauth_bearer_सन्देशः(conn->creds,
                                                      &resp);
    भङ्ग;

  पक्ष SASL_OAUTH2_RESP:
    /* The continuation is optional so check the response code */
    यदि(संहिता == sasl->params->finalcode) {
      /* Final response was received so we are done */
      *प्रगतिः = SASL_समाप्तम्;
      sasl_state(sasl, दत्त, SASL_STOP);
      प्रत्यागम परिणाम;
    }
    अन्यथा यदि(संहिता == sasl->params->contcode) {
      /* Acknowledge the continuation by sending a 0x01 response. */
      Curl_bufref_समुच्चयः(&resp, "\x01", 1, रिक्त);
      भङ्ग;
    }
    अन्यथा {
      *प्रगतिः = SASL_समाप्तम्;
      sasl_state(sasl, दत्त, SASL_STOP);
      प्रत्यागम CURLE_LOGIN_DENIED;
    }

  पक्ष SASL_CANCEL:
    /* Remove the offending mechanism from the supported list */
    sasl->authmechs &= (अचिह्नित ह्रस्व)~sasl->authused;
    sasl->authused = SASL_AUTH_NONE;
    sasl->curmech = रिक्त;

    /* Start an alternative SASL authentication */
    प्रत्यागम Curl_sasl_start(sasl, दत्त, (bool)sasl->force_ir, प्रगतिः);
  स्वभाव:
    failf(दत्त, "Unsupported SASL authentication mechanism");
    परिणाम = CURLE_UNSUPPORTED_PROTOCOL;  /* Should not happen */
    भङ्ग;
  }

  Curl_bufref_free(&serverdata);

  विकल्प(परिणाम) {
  पक्ष CURLE_BAD_CONTENT_ENCODING:
    /* Cancel dialog */
    परिणाम = sasl->params->cancelauth(दत्त, sasl->curmech);
    newstate = SASL_CANCEL;
    भङ्ग;
  पक्ष CURLE_OK:
    परिणाम = build_सन्देशः(sasl, &resp);
    यदि(!परिणाम)
      परिणाम = sasl->params->contauth(दत्त, sasl->curmech, &resp);
    भङ्ग;
  स्वभाव:
    newstate = SASL_STOP;    /* Stop on error */
    *प्रगतिः = SASL_समाप्तम्;
    भङ्ग;
  }

  Curl_bufref_free(&resp);

  sasl_state(sasl, दत्त, newstate);

  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् CURLVERBOSE
स्थिर शून्य sasl_unchosen(संरचना Curl_easy *दत्त, अचिह्नित ह्रस्व mech,
                          अचिह्नित ह्रस्व enabledmechs,
                          bool built_अन्तर्, bool platform,
                          ध्रुव वर्ण *param_missing)
{
  ध्रुव वर्ण *mname = रिक्त;
  size_t i;

  यदि(!(enabledmechs & mech))
    प्रत्यागम;

  प्रति(i = 0; mechtable[i].name; ++i) {
    यदि(mechtable[i].bit == mech) {
      mname = mechtable[i].name;
      भङ्ग;
    }
  }
  यदि(!mname)  /* should not happen */
    प्रत्यागम;
  यदि(!built_अन्तर्)
    infof(दत्त, "SASL: %s not builtin", mname);
  अन्यथा यदि(!platform)
    infof(दत्त, "SASL: %s not supported by the platform/libraries", mname);
  अन्यथा {
    यदि(param_missing)
      infof(दत्त, "SASL: %s is missing %s", mname, param_missing);
    यदि(!Curl_creds_has_user(दत्त->conn->creds))
      infof(दत्त, "SASL: %s is missing username", mname);
  }
}
#यदि_अन्तः /* CURLVERBOSE */

CURLcode Curl_sasl_is_blocked(संरचना SASL *sasl, संरचना Curl_easy *दत्त)
{
#यदि_परिभाषितम् CURLVERBOSE
#यदि_परिभाषितम् USE_KERBEROS5
#परिभाषय CURL_SASL_KERBEROS5   TRUE
#अन्यथा
#परिभाषय CURL_SASL_KERBEROS5   FALSE
#यदि_अन्तः
#यदि_परिभाषितम् USE_GSASL
#परिभाषय CURL_SASL_GASL        TRUE
#अन्यथा
#परिभाषय CURL_SASL_GASL        FALSE
#यदि_अन्तः
#यदि_परिभाषितम् CURL_DISABLE_DIGEST_AUTH
#परिभाषय CURL_SASL_DIGEST      TRUE
#अन्यथा
#परिभाषय CURL_SASL_DIGEST      FALSE
#यदि_अन्तः
#यदि_अपरिभाषितम् USE_NTLM
#परिभाषय CURL_SASL_NTLM        TRUE
#अन्यथा
#परिभाषय CURL_SASL_NTLM        FALSE
#यदि_अन्तः
  /* Failing SASL authentication is a pain. Give a helping hand if
   * we were unable to select an AUTH mechanism.
   * `sasl->authmechs` are mechanisms offered by the peer
   * `sasl->prefmech`  are mechanisms preferred by us */
  अचिह्नित ह्रस्व enabledmechs = sasl->authmechs & sasl->prefmech;

  यदि(!sasl->authmechs)
    infof(दत्त, "SASL: no auth mechanism was offered or recognized");
  अन्यथा यदि(!enabledmechs)
    infof(दत्त, "SASL: no overlap between offered and configured "
          "auth mechanisms");
  अन्यथा {
    infof(दत्त, "SASL: no auth mechanism offered could be selected");
    यदि((enabledmechs & SASL_MECH_EXTERNAL) &&
       Curl_creds_has_passwd(दत्त->conn->creds))
      infof(दत्त, "SASL: auth EXTERNAL not chosen with password");
    sasl_unchosen(दत्त, SASL_MECH_GSSAPI, enabledmechs,
                  CURL_SASL_KERBEROS5, Curl_auth_is_gssapi_supported(), रिक्त);
    sasl_unchosen(दत्त, SASL_MECH_SCRAM_SHA_256, enabledmechs,
                  CURL_SASL_GASL, FALSE, रिक्त);
    sasl_unchosen(दत्त, SASL_MECH_SCRAM_SHA_1, enabledmechs,
                  CURL_SASL_GASL, FALSE, रिक्त);
    sasl_unchosen(दत्त, SASL_MECH_DIGEST_MD5, enabledmechs,
                  CURL_SASL_DIGEST, Curl_auth_is_digest_supported(), रिक्त);
    sasl_unchosen(दत्त, SASL_MECH_CRAM_MD5, enabledmechs,
                  CURL_SASL_DIGEST, TRUE, रिक्त);
    sasl_unchosen(दत्त, SASL_MECH_NTLM, enabledmechs,
                  CURL_SASL_NTLM, Curl_auth_is_ntlm_supported(), रिक्त);
    sasl_unchosen(दत्त, SASL_MECH_OAUTHBEARER, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(दत्त->conn->creds) ?
                  रिक्त : "CURLOPT_XOAUTH2_BEARER");
    sasl_unchosen(दत्त, SASL_MECH_XOAUTH2, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(दत्त->conn->creds) ?
                  रिक्त : "CURLOPT_XOAUTH2_BEARER");
  }
#यदि_अन्तः /* CURLVERBOSE */
  (शून्य)sasl;
  (शून्य)दत्त;
  प्रत्यागम CURLE_LOGIN_DENIED;
}

#यदि_अन्तः /* protocols are enabled that use SASL */
