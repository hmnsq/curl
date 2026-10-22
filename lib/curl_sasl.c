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
अचिन्हित लघु Curl_sasl_decode_mech(ध्रुव वर्ण *ptr, माप_प्रकार maxlen,
                                     माप_प्रकार *len)
{
  अचिन्हित पूर्णाङ्क i;
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
                                         ध्रुव वर्ण *मूल्य, माप_प्रकार len)
{
  CURLcode result = CURLE_OK;
  माप_प्रकार mechlen;

  यदि(!len)
    प्रत्यागम CURLE_URL_MALFORMAT;

  यदि(sasl->resetprefs) {
    sasl->resetprefs = FALSE;
    sasl->prefmech = SASL_AUTH_NONE;
  }

  यदि(!म_भेदन(मूल्य, "*", len))
    sasl->prefmech = SASL_AUTH_DEFAULT;
  अन्यथा {
    अचिन्हित लघु mechbit = Curl_sasl_decode_mech(मूल्य, len, &mechlen);
    यदि(mechbit && mechlen == len)
      sasl->prefmech |= mechbit;
    अन्यथा
      result = CURLE_URL_MALFORMAT;
  }

  प्रत्यागम result;
}

/*
 * Curl_sasl_init()
 *
 * Initializes the SASL structure.
 */
शून्य Curl_sasl_init(संरचना SASL *sasl, संरचना Curl_easy *data,
                    ध्रुव संरचना SASLproto *params)
{
  uint32_t auth = data->set.httpauth;

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
    अचिन्हित लघु mechs = SASL_AUTH_NONE;

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
स्थिर शून्य sasl_state(संरचना SASL *sasl, संरचना Curl_easy *data,
                       saslstate newstate)
{
#यदि defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  स्थिर ध्रुव वर्ण * ध्रुव names[] = {
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

  यदि(sasl->state != newstate)
    infof(data, "SASL %p state change from %s to %s",
          (शून्य *)sasl, names[sasl->state], names[newstate]);
#अन्यथा
  (शून्य)data;
#endif

  sasl->state = newstate;
}

#यदि defined(USE_NTLM) || defined(USE_GSASL) || defined(USE_KERBEROS5) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)
/* Get the SASL server message and convert it to binary. */
स्थिर CURLcode get_server_message(संरचना SASL *sasl, संरचना Curl_easy *data,
                                   संरचना bufref *out)
{
  CURLcode result = CURLE_OK;

  result = sasl->params->getmessage(data, out);
  यदि(!result && (sasl->params->flags & SASL_FLAG_BASE64)) {
    ध्रुव वर्ण *serverdata = Curl_bufref_ptr(out);

    यदि(!*serverdata)
      Curl_bufref_set(out, रिक्त, 0, रिक्त);
    अन्यथा {
      अचिन्हित वर्ण *संदेश;
      माप_प्रकार msglen;

      result = curlx_base64_decode(serverdata, &संदेश, &msglen);
      यदि(!result)
        Curl_bufref_set(out, संदेश, msglen, curl_free);
    }
  }
  प्रत्यागम result;
}
#endif

/* Encode the outgoing SASL message. */
स्थिर CURLcode build_message(संरचना SASL *sasl, संरचना bufref *संदेश)
{
  CURLcode result = CURLE_OK;

  यदि(sasl->params->flags & SASL_FLAG_BASE64) {
    यदि(!Curl_bufref_ptr(संदेश))                   /* Empty message. */
      Curl_bufref_set(संदेश, "", 0, रिक्त);
    अन्यथा यदि(Curl_bufref_len(संदेश)) {
      वर्ण *base64;
      माप_प्रकार base64len;

      result = curlx_base64_encode(Curl_bufref_uptr(संदेश),
                                   Curl_bufref_len(संदेश), &base64, &base64len);
      यदि(!result)
        Curl_bufref_set(संदेश, base64, base64len, curl_free);
    }
  }

  प्रत्यागम result;
}

/*
 * Curl_sasl_can_authenticate()
 *
 * Check if we have enough auth data and capabilities to authenticate.
 */
bool Curl_sasl_can_authenticate(संरचना SASL *sasl, संरचना Curl_easy *data)
{
  /* Have credentials been provided? */
  यदि(data->conn->creds)
    प्रत्यागम TRUE;

  /* EXTERNAL can authenticate without a username and/or password */
  यदि(sasl->authmechs & sasl->prefmech & SASL_MECH_EXTERNAL)
    प्रत्यागम TRUE;

  प्रत्यागम FALSE;
}

संरचना sasl_ctx {
  संरचना SASL *sasl;
  संरचना connectdata *conn;
  अचिन्हित लघु enabledmechs;
  ध्रुव वर्ण *mech;
  saslstate state1;
  saslstate state2;
  संरचना bufref resp;
  CURLcode result;
};

स्थिर bool sasl_choose_external(संरचना Curl_easy *data, संरचना sasl_ctx *sctx)
{
  यदि((sctx->enabledmechs & SASL_MECH_EXTERNAL) &&
     !Curl_creds_has_passwd(sctx->conn->creds)) {
    sctx->mech = SASL_MECH_STRING_EXTERNAL;
    sctx->state1 = SASL_EXTERNAL;
    sctx->sasl->authused = SASL_MECH_EXTERNAL;

    यदि(sctx->sasl->force_ir || data->set.sasl_ir)
      Curl_auth_create_external_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

#ifdef USE_KERBEROS5
स्थिर bool sasl_choose_krb5(संरचना Curl_easy *data, संरचना sasl_ctx *sctx)
{
  यदि((sctx->enabledmechs & SASL_MECH_GSSAPI) &&
     Curl_auth_is_gssapi_supported() &&
     Curl_auth_user_contains_domain(sctx->conn->creds)) {
    ध्रुव वर्ण *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;

    sctx->sasl->mutual_auth = FALSE;
    sctx->mech = SASL_MECH_STRING_GSSAPI;
    sctx->state1 = SASL_GSSAPI;
    sctx->state2 = SASL_GSSAPI_TOKEN;
    sctx->sasl->authused = SASL_MECH_GSSAPI;

    यदि(sctx->sasl->force_ir || data->set.sasl_ir) {
      संरचना kerberos5data *krb5 = Curl_auth_krb5_get(sctx->conn);
      sctx->result = !krb5 ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_gssapi_user_message(data, sctx->conn->creds,
                                             service,
                                             sctx->conn->origin->hostname,
                                             (bool)sctx->sasl->mutual_auth,
                                             रिक्त, krb5, &sctx->resp);
    }
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}
#endif /* USE_KERBEROS5 */

#ifdef USE_GSASL
स्थिर bool sasl_choose_gsasl(संरचना Curl_easy *data, संरचना sasl_ctx *sctx)
{
  संरचना gsasldata *gsasl;
  संरचना bufref nullmsg;

  यदि((sctx->enabledmechs &
      (SASL_MECH_SCRAM_SHA_256 | SASL_MECH_SCRAM_SHA_1))) {
    gsasl = Curl_auth_gsasl_get(sctx->conn);
    यदि(!gsasl) {
      sctx->result = CURLE_OUT_OF_MEMORY;
      प्रत्यागम TRUE; /* attempted, but failed */
    }

    यदि((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_256) &&
       Curl_auth_gsasl_is_supported(data, SASL_MECH_STRING_SCRAM_SHA_256,
                                    gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_256;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_256;
    }
    अन्यथा यदि((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_1) &&
            Curl_auth_gsasl_is_supported(data, SASL_MECH_STRING_SCRAM_SHA_1,
                                         gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_1;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_1;
    }
    अन्यथा
      प्रत्यागम FALSE;

    Curl_bufref_init(&nullmsg);
    sctx->state1 = SASL_GSASL;
    sctx->state2 = SASL_GSASL;
    sctx->result = Curl_auth_gsasl_start(data, sctx->conn->creds, gsasl);
    यदि(!sctx->result && (sctx->sasl->force_ir || data->set.sasl_ir))
      sctx->result = Curl_auth_gsasl_token(data, &nullmsg, gsasl, &sctx->resp);
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

#endif /* USE_GSASL */

#ifndef CURL_DISABLE_DIGEST_AUTH
स्थिर bool sasl_choose_digest(संरचना Curl_easy *data, संरचना sasl_ctx *sctx)
{
  (शून्य)data;
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
#endif /* !CURL_DISABLE_DIGEST_AUTH */

#ifdef USE_NTLM
स्थिर bool sasl_choose_ntlm(संरचना Curl_easy *data, संरचना sasl_ctx *sctx)
{
  यदि((sctx->enabledmechs & SASL_MECH_NTLM) &&
     Curl_auth_is_ntlm_supported()) {
     ध्रुव वर्ण *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;
    ध्रुव वर्ण *hostname;

    Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, रिक्त);

    sctx->mech = SASL_MECH_STRING_NTLM;
    sctx->state1 = SASL_NTLM;
    sctx->state2 = SASL_NTLM_TYPE2_संदेश;
    sctx->sasl->authused = SASL_MECH_NTLM;

    यदि(sctx->sasl->force_ir || data->set.sasl_ir) {
      संरचना ntlmdata *ntlm = Curl_auth_ntlm_get(sctx->conn, FALSE);
      sctx->result = !ntlm ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_ntlm_type1_message(data, sctx->conn->creds,
                                            service, hostname,
                                            ntlm, &sctx->resp);
    }
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}
#endif /* USE_NTLM */

स्थिर bool sasl_choose_oauth(संरचना Curl_easy *data, संरचना sasl_ctx *sctx)
{
  यदि(Curl_creds_has_oauth_bearer(data->state.creds) &&
     (sctx->enabledmechs & SASL_MECH_OAUTHBEARER)) {
    ध्रुव वर्ण *hostname;
    पूर्णाङ्क port;
    Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, &port);

    sctx->mech = SASL_MECH_STRING_OAUTHBEARER;
    sctx->state1 = SASL_OAUTH2;
    sctx->state2 = SASL_OAUTH2_RESP;
    sctx->sasl->authused = SASL_MECH_OAUTHBEARER;

    यदि(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result =
        Curl_auth_create_oauth_bearer_message(sctx->conn->creds,
                                              hostname, port, &sctx->resp);
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

स्थिर bool sasl_choose_oauth2(संरचना Curl_easy *data, संरचना sasl_ctx *sctx)
{
  यदि(Curl_creds_has_oauth_bearer(sctx->conn->creds) &&
     (sctx->enabledmechs & SASL_MECH_XOAUTH2)) {
    sctx->mech = SASL_MECH_STRING_XOAUTH2;
    sctx->state1 = SASL_OAUTH2;
    sctx->sasl->authused = SASL_MECH_XOAUTH2;

    यदि(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result = Curl_auth_create_xoauth_bearer_message(
        sctx->conn->creds, &sctx->resp);
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

स्थिर bool sasl_choose_plain(संरचना Curl_easy *data, संरचना sasl_ctx *sctx)
{
  यदि(sctx->enabledmechs & SASL_MECH_PLAIN) {
    sctx->mech = SASL_MECH_STRING_PLAIN;
    sctx->state1 = SASL_PLAIN;
    sctx->sasl->authused = SASL_MECH_PLAIN;

    यदि(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result =
        Curl_auth_create_plain_message(sctx->conn->creds, &sctx->resp);
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

स्थिर bool sasl_choose_login(संरचना Curl_easy *data, संरचना sasl_ctx *sctx)
{
  यदि(sctx->enabledmechs & SASL_MECH_LOGIN) {
    sctx->mech = SASL_MECH_STRING_LOGIN;
    sctx->state1 = SASL_LOGIN;
    sctx->state2 = SASL_LOGIN_PASSWD;
    sctx->sasl->authused = SASL_MECH_LOGIN;

    यदि(sctx->sasl->force_ir || data->set.sasl_ir)
      Curl_auth_create_login_message(
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
CURLcode Curl_sasl_start(संरचना SASL *sasl, संरचना Curl_easy *data,
                         bool force_ir, saslprogress *progress)
{
  संरचना sasl_ctx sctx;

  sasl->force_ir = force_ir;    /* Latch for future use */
  sasl->authused = 0;           /* No mechanism used yet */
  *progress = SASL_IDLE;

  स_रखो(&sctx, 0, परिमाण(sctx));
  sctx.sasl = sasl;
  sctx.conn = data->conn;
  Curl_bufref_init(&sctx.resp);
  sctx.enabledmechs = sasl->authmechs & sasl->prefmech;
  sctx.state1 = SASL_STOP;
  sctx.state2 = SASL_FINAL;

  /* Calculate the supported authentication mechanism, by decreasing order of
     security, as well as the initial response where appropriate */
  यदि(sasl_choose_external(data, &sctx) ||
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

  यदि(!sctx.result && sctx.mech) {
    sasl->curmech = sctx.mech;
    यदि(Curl_bufref_ptr(&sctx.resp))
      sctx.result = build_message(sasl, &sctx.resp);

    यदि(sasl->params->maxirlen &&
       म_माप(sctx.mech) + Curl_bufref_len(&sctx.resp) >
         sasl->params->maxirlen)
      Curl_bufref_free(&sctx.resp);

    यदि(!sctx.result)
      sctx.result = sasl->params->sendauth(data, sctx.mech, &sctx.resp);

    यदि(!sctx.result) {
      *progress = SASL_INPROGRESS;
      sasl_state(sasl, data, Curl_bufref_ptr(&sctx.resp) ?
                 sctx.state2 : sctx.state1);
    }
  }

  Curl_bufref_free(&sctx.resp);
  प्रत्यागम sctx.result;
}

/*
 * Curl_sasl_continue()
 *
 * Continue the authentication.
 */
CURLcode Curl_sasl_continue(संरचना SASL *sasl, संरचना Curl_easy *data,
                            पूर्णाङ्क code, saslprogress *progress)
{
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;
  saslstate newstate = SASL_FINAL;
  संरचना bufref resp;
  ध्रुव वर्ण *hostname;
  पूर्णाङ्क port;
  संरचना bufref serverdata;

  Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, &port);
  Curl_bufref_init(&serverdata);
  Curl_bufref_init(&resp);
  *progress = SASL_INPROGRESS;

  यदि(sasl->state == SASL_FINAL) {
    यदि(code != sasl->params->finalcode)
      result = CURLE_LOGIN_DENIED;
    *progress = SASL_DONE;
    sasl_state(sasl, data, SASL_STOP);
    प्रत्यागम result;
  }

  यदि(sasl->state != SASL_CANCEL && sasl->state != SASL_OAUTH2_RESP &&
     code != sasl->params->contcode) {
    *progress = SASL_DONE;
    sasl_state(sasl, data, SASL_STOP);
    प्रत्यागम CURLE_LOGIN_DENIED;
  }

  विकल्प(sasl->state) {
  पक्ष SASL_STOP:
    *progress = SASL_DONE;
    प्रत्यागम result;
  पक्ष SASL_PLAIN:
    result = Curl_auth_create_plain_message(conn->creds, &resp);
    भङ्ग;
  पक्ष SASL_LOGIN:
    Curl_auth_create_login_message(Curl_creds_user(conn->creds), &resp);
    newstate = SASL_LOGIN_PASSWD;
    भङ्ग;
  पक्ष SASL_LOGIN_PASSWD:
    Curl_auth_create_login_message(Curl_creds_passwd(conn->creds), &resp);
    भङ्ग;
  पक्ष SASL_EXTERNAL:
    Curl_auth_create_external_message(Curl_creds_user(conn->creds), &resp);
    भङ्ग;
#ifdef USE_GSASL
  पक्ष SASL_GSASL:
    result = get_server_message(sasl, data, &serverdata);
    यदि(!result) {
      संरचना gsasldata *gsasl = Curl_auth_gsasl_get(conn);
      result = !gsasl ? CURLE_OUT_OF_MEMORY :
        Curl_auth_gsasl_token(data, &serverdata, gsasl, &resp);
    }
    यदि(!result && Curl_bufref_len(&resp) > 0)
      newstate = SASL_GSASL;
    भङ्ग;
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  पक्ष SASL_CRAMMD5:
    result = get_server_message(sasl, data, &serverdata);
    यदि(!result)
      result = Curl_auth_create_cram_md5_message(&serverdata, conn->creds,
                                                 &resp);
    भङ्ग;
  पक्ष SASL_DIGESTMD5:
    result = get_server_message(sasl, data, &serverdata);
    यदि(!result)
      result = Curl_auth_create_digest_md5_message(data, &serverdata,
                                                   conn->creds,
                                                   sasl->params->service,
                                                   &resp);
    यदि(!result && (sasl->params->flags & SASL_FLAG_BASE64))
      newstate = SASL_DIGESTMD5_RESP;
    भङ्ग;
  पक्ष SASL_DIGESTMD5_RESP:
    /* Keep response NULL to output an empty line. */
    भङ्ग;
#endif

#ifdef USE_NTLM
  पक्ष SASL_NTLM: {
    /* Create the type-1 message */
    संरचना ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    result = !ntlm ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_ntlm_type1_message(data, conn->creds,
                                          sasl->params->service, hostname,
                                          ntlm, &resp);
    newstate = SASL_NTLM_TYPE2_संदेश;
    भङ्ग;
  }
  पक्ष SASL_NTLM_TYPE2_संदेश: {
    /* Decode the type-2 message */
    संरचना ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    result = !ntlm ? CURLE_OUT_OF_MEMORY :
      get_server_message(sasl, data, &serverdata);
    यदि(!result)
      result = Curl_auth_decode_ntlm_type2_message(data, &serverdata, ntlm);
    यदि(!result)
      result = Curl_auth_create_ntlm_type3_message(data, conn->creds,
                                                   ntlm, &resp);
    भङ्ग;
  }
#endif

#ifdef USE_KERBEROS5
  पक्ष SASL_GSSAPI: {
    संरचना kerberos5data *krb5 = Curl_auth_krb5_get(conn);
    result = !krb5 ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_gssapi_user_message(data, conn->creds,
                                           sasl->params->service,
                                           conn->origin->hostname,
                                           (bool)sasl->mutual_auth, रिक्त,
                                           krb5, &resp);
    newstate = SASL_GSSAPI_TOKEN;
    भङ्ग;
  }
  पक्ष SASL_GSSAPI_TOKEN:
    result = get_server_message(sasl, data, &serverdata);
    यदि(!result) {
      संरचना kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      यदि(!krb5)
        result = CURLE_OUT_OF_MEMORY;
      अन्यथा यदि(sasl->mutual_auth) {
        /* Decode the user token challenge and create the optional response
           message */
        result = Curl_auth_create_gssapi_user_message(data, रिक्त,
                                                      रिक्त, रिक्त,
                                                      (bool)sasl->mutual_auth,
                                                      &serverdata,
                                                      krb5, &resp);
        newstate = SASL_GSSAPI_NO_DATA;
      }
      अन्यथा
        /* Decode the security challenge and create the response message */
        result = Curl_auth_create_gssapi_security_message(
          data, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    भङ्ग;
  पक्ष SASL_GSSAPI_NO_DATA:
    /* Decode the security challenge and create the response message */
    result = get_server_message(sasl, data, &serverdata);
    यदि(!result) {
      संरचना kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      यदि(!krb5)
        result = CURLE_OUT_OF_MEMORY;
      अन्यथा
        result = Curl_auth_create_gssapi_security_message(
          data, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    भङ्ग;
#endif

  पक्ष SASL_OAUTH2:
    /* Create the authorization message */
    यदि(sasl->authused == SASL_MECH_OAUTHBEARER) {
      result = Curl_auth_create_oauth_bearer_message(conn->creds,
                                                     hostname,
                                                     port,
                                                     &resp);

      /* Failures maybe sent by the server as continuations for OAUTHBEARER */
      newstate = SASL_OAUTH2_RESP;
    }
    अन्यथा
      result = Curl_auth_create_xoauth_bearer_message(conn->creds,
                                                      &resp);
    भङ्ग;

  पक्ष SASL_OAUTH2_RESP:
    /* The continuation is optional so check the response code */
    यदि(code == sasl->params->finalcode) {
      /* Final response was received so we are done */
      *progress = SASL_DONE;
      sasl_state(sasl, data, SASL_STOP);
      प्रत्यागम result;
    }
    अन्यथा यदि(code == sasl->params->contcode) {
      /* Acknowledge the continuation by sending a 0x01 response. */
      Curl_bufref_set(&resp, "\x01", 1, रिक्त);
      भङ्ग;
    }
    अन्यथा {
      *progress = SASL_DONE;
      sasl_state(sasl, data, SASL_STOP);
      प्रत्यागम CURLE_LOGIN_DENIED;
    }

  पक्ष SASL_CANCEL:
    /* Remove the offending mechanism from the supported list */
    sasl->authmechs &= (अचिन्हित लघु)~sasl->authused;
    sasl->authused = SASL_AUTH_NONE;
    sasl->curmech = रिक्त;

    /* Start an alternative SASL authentication */
    प्रत्यागम Curl_sasl_start(sasl, data, (bool)sasl->force_ir, progress);
  स्वभाव:
    failf(data, "Unsupported SASL authentication mechanism");
    result = CURLE_UNSUPPORTED_PROTOCOL;  /* Should not happen */
    भङ्ग;
  }

  Curl_bufref_free(&serverdata);

  विकल्प(result) {
  पक्ष CURLE_BAD_CONTENT_ENCODING:
    /* Cancel dialog */
    result = sasl->params->cancelauth(data, sasl->curmech);
    newstate = SASL_CANCEL;
    भङ्ग;
  पक्ष CURLE_OK:
    result = build_message(sasl, &resp);
    यदि(!result)
      result = sasl->params->contauth(data, sasl->curmech, &resp);
    भङ्ग;
  स्वभाव:
    newstate = SASL_STOP;    /* Stop on error */
    *progress = SASL_DONE;
    भङ्ग;
  }

  Curl_bufref_free(&resp);

  sasl_state(sasl, data, newstate);

  प्रत्यागम result;
}

#ifdef CURLVERBOSE
स्थिर शून्य sasl_unchosen(संरचना Curl_easy *data, अचिन्हित लघु mech,
                          अचिन्हित लघु enabledmechs,
                          bool built_in, bool platform,
                          ध्रुव वर्ण *param_missing)
{
  ध्रुव वर्ण *mname = रिक्त;
  माप_प्रकार i;

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
  यदि(!built_in)
    infof(data, "SASL: %s not builtin", mname);
  अन्यथा यदि(!platform)
    infof(data, "SASL: %s not supported by the platform/libraries", mname);
  अन्यथा {
    यदि(param_missing)
      infof(data, "SASL: %s is missing %s", mname, param_missing);
    यदि(!Curl_creds_has_user(data->conn->creds))
      infof(data, "SASL: %s is missing username", mname);
  }
}
#endif /* CURLVERBOSE */

CURLcode Curl_sasl_is_blocked(संरचना SASL *sasl, संरचना Curl_easy *data)
{
#ifdef CURLVERBOSE
#ifdef USE_KERBEROS5
#define CURL_SASL_KERBEROS5   TRUE
#अन्यथा
#define CURL_SASL_KERBEROS5   FALSE
#endif
#ifdef USE_GSASL
#define CURL_SASL_GASL        TRUE
#अन्यथा
#define CURL_SASL_GASL        FALSE
#endif
#ifdef CURL_DISABLE_DIGEST_AUTH
#define CURL_SASL_DIGEST      TRUE
#अन्यथा
#define CURL_SASL_DIGEST      FALSE
#endif
#ifndef USE_NTLM
#define CURL_SASL_NTLM        TRUE
#अन्यथा
#define CURL_SASL_NTLM        FALSE
#endif
  /* Failing SASL authentication is a pain. Give a helping hand if
   * we were unable to select an AUTH mechanism.
   * `sasl->authmechs` are mechanisms offered by the peer
   * `sasl->prefmech`  are mechanisms preferred by us */
  अचिन्हित लघु enabledmechs = sasl->authmechs & sasl->prefmech;

  यदि(!sasl->authmechs)
    infof(data, "SASL: no auth mechanism was offered or recognized");
  अन्यथा यदि(!enabledmechs)
    infof(data, "SASL: no overlap between offered and configured "
          "auth mechanisms");
  अन्यथा {
    infof(data, "SASL: no auth mechanism offered could be selected");
    यदि((enabledmechs & SASL_MECH_EXTERNAL) &&
       Curl_creds_has_passwd(data->conn->creds))
      infof(data, "SASL: auth EXTERNAL not chosen with password");
    sasl_unchosen(data, SASL_MECH_GSSAPI, enabledmechs,
                  CURL_SASL_KERBEROS5, Curl_auth_is_gssapi_supported(), रिक्त);
    sasl_unchosen(data, SASL_MECH_SCRAM_SHA_256, enabledmechs,
                  CURL_SASL_GASL, FALSE, रिक्त);
    sasl_unchosen(data, SASL_MECH_SCRAM_SHA_1, enabledmechs,
                  CURL_SASL_GASL, FALSE, रिक्त);
    sasl_unchosen(data, SASL_MECH_DIGEST_MD5, enabledmechs,
                  CURL_SASL_DIGEST, Curl_auth_is_digest_supported(), रिक्त);
    sasl_unchosen(data, SASL_MECH_CRAM_MD5, enabledmechs,
                  CURL_SASL_DIGEST, TRUE, रिक्त);
    sasl_unchosen(data, SASL_MECH_NTLM, enabledmechs,
                  CURL_SASL_NTLM, Curl_auth_is_ntlm_supported(), रिक्त);
    sasl_unchosen(data, SASL_MECH_OAUTHBEARER, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(data->conn->creds) ?
                  रिक्त : "CURLOPT_XOAUTH2_BEARER");
    sasl_unchosen(data, SASL_MECH_XOAUTH2, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(data->conn->creds) ?
                  रिक्त : "CURLOPT_XOAUTH2_BEARER");
  }
#endif /* CURLVERBOSE */
  (शून्य)sasl;
  (शून्य)data;
  प्रत्यागम CURLE_LOGIN_DENIED;
}

#endif /* protocols are enabled that use SASL */
