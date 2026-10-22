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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#شامل "urldata.h"
#شامل "curlx/base64.h"
#شامل "vauth/vauth.h"
#شامل "cfilters.h"
#شامل "curl_sasl.h"
#شامل "curl_trc.h"

/* Supported mechanisms */
ایستا ثابت ساختار {
  ثابت نویسه    *name;  /* Name */
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
unsigned short Curl_sasl_decode_mech(ثابت نویسه *ptr, size_t maxlen,
                                     size_t *len)
{
  unsigned صحیح i;
  نویسه c;

  برای(i = 0; mechtable[i].name; i++) {
    اگر(maxlen >= mechtable[i].len &&
       curl_strnequal(ptr, mechtable[i].name, mechtable[i].len)) {
      اگر(len)
        *len = mechtable[i].len;

      اگر(maxlen == mechtable[i].len)
        بازگشت mechtable[i].bit;

      c = ptr[mechtable[i].len];
      اگر(!ISUPPER(c) && !ISDIGIT(c) && c != '-' && c != '_')
        بازگشت mechtable[i].bit;
    }
  }

  بازگشت 0;
}

/*
 * Curl_sasl_parse_url_auth_option()
 *
 * Parse the URL login options.
 */
CURLcode Curl_sasl_parse_url_auth_option(ساختار SASL *sasl,
                                         ثابت نویسه *مقدار, size_t len)
{
  CURLcode result = CURLE_OK;
  size_t mechlen;

  اگر(!len)
    بازگشت CURLE_URL_MALFORMAT;

  اگر(sasl->resetprefs) {
    sasl->resetprefs = FALSE;
    sasl->prefmech = SASL_AUTH_NONE;
  }

  اگر(!strncmp(مقدار, "*", len))
    sasl->prefmech = SASL_AUTH_DEFAULT;
  وگرنه {
    unsigned short mechbit = Curl_sasl_decode_mech(مقدار, len, &mechlen);
    اگر(mechbit && mechlen == len)
      sasl->prefmech |= mechbit;
    وگرنه
      result = CURLE_URL_MALFORMAT;
  }

  بازگشت result;
}

/*
 * Curl_sasl_init()
 *
 * Initializes the SASL structure.
 */
تهی Curl_sasl_init(ساختار SASL *sasl, ساختار Curl_easy *data,
                    ثابت ساختار SASLproto *params)
{
  uint32_t auth = data->set.httpauth;

  sasl->params = params;           /* Set protocol dependent parameters */
  sasl->state = SASL_STOP;         /* Not yet running */
  sasl->curmech = پوچ;            /* No mechanism yet. */
  sasl->authmechs = SASL_AUTH_NONE; /* No known authentication mechanism yet */
  sasl->prefmech = params->defmechs; /* Default preferred mechanisms */
  sasl->authused = SASL_AUTH_NONE; /* The authentication mechanism used */
  sasl->resetprefs = TRUE;         /* Reset prefmech upon AUTH parsing. */
  sasl->mutual_auth = FALSE;       /* No mutual authentication (GSSAPI only) */
  sasl->force_ir = FALSE;          /* Respect external option */

  اگر(auth != CURLAUTH_BASIC) {
    unsigned short mechs = SASL_AUTH_NONE;

    /* If some usable http authentication options have been set, determine
       new defaults from them. */
    اگر(auth & CURLAUTH_BASIC)
      mechs |= SASL_MECH_PLAIN | SASL_MECH_LOGIN;
    اگر(auth & CURLAUTH_DIGEST)
      mechs |= SASL_MECH_DIGEST_MD5;
    اگر(auth & CURLAUTH_NTLM)
      mechs |= SASL_MECH_NTLM;
    اگر(auth & CURLAUTH_BEARER)
      mechs |= SASL_MECH_OAUTHBEARER | SASL_MECH_XOAUTH2;
    اگر(auth & CURLAUTH_GSSAPI)
      mechs |= SASL_MECH_GSSAPI;

    اگر(mechs != SASL_AUTH_NONE)
      sasl->prefmech = mechs;
  }
}

/*
 * sasl_state()
 *
 * This is the ONLY way to change SASL state!
 */
ایستا تهی sasl_state(ساختار SASL *sasl, ساختار Curl_easy *data,
                       saslstate newstate)
{
#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ایستا ثابت نویسه * ثابت names[] = {
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

  اگر(sasl->state != newstate)
    infof(data, "SASL %p state change from %s to %s",
          (تهی *)sasl, names[sasl->state], names[newstate]);
#وگرنه
  (تهی)data;
#endif

  sasl->state = newstate;
}

#اگر defined(USE_NTLM) || defined(USE_GSASL) || defined(USE_KERBEROS5) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)
/* Get the SASL server message and convert it to binary. */
ایستا CURLcode get_server_message(ساختار SASL *sasl, ساختار Curl_easy *data,
                                   ساختار bufref *out)
{
  CURLcode result = CURLE_OK;

  result = sasl->params->getmessage(data, out);
  اگر(!result && (sasl->params->flags & SASL_FLAG_BASE64)) {
    ثابت نویسه *serverdata = Curl_bufref_ptr(out);

    اگر(!*serverdata)
      Curl_bufref_set(out, پوچ, 0, پوچ);
    وگرنه {
      unsigned نویسه *پیام;
      size_t msglen;

      result = curlx_base64_decode(serverdata, &پیام, &msglen);
      اگر(!result)
        Curl_bufref_set(out, پیام, msglen, curl_free);
    }
  }
  بازگشت result;
}
#endif

/* Encode the outgoing SASL message. */
ایستا CURLcode build_message(ساختار SASL *sasl, ساختار bufref *پیام)
{
  CURLcode result = CURLE_OK;

  اگر(sasl->params->flags & SASL_FLAG_BASE64) {
    اگر(!Curl_bufref_ptr(پیام))                   /* Empty message. */
      Curl_bufref_set(پیام, "", 0, پوچ);
    وگرنه اگر(Curl_bufref_len(پیام)) {
      نویسه *base64;
      size_t base64len;

      result = curlx_base64_encode(Curl_bufref_uptr(پیام),
                                   Curl_bufref_len(پیام), &base64, &base64len);
      اگر(!result)
        Curl_bufref_set(پیام, base64, base64len, curl_free);
    }
  }

  بازگشت result;
}

/*
 * Curl_sasl_can_authenticate()
 *
 * Check if we have enough auth data and capabilities to authenticate.
 */
bool Curl_sasl_can_authenticate(ساختار SASL *sasl, ساختار Curl_easy *data)
{
  /* Have credentials been provided? */
  اگر(data->conn->creds)
    بازگشت TRUE;

  /* EXTERNAL can authenticate without a username and/or password */
  اگر(sasl->authmechs & sasl->prefmech & SASL_MECH_EXTERNAL)
    بازگشت TRUE;

  بازگشت FALSE;
}

ساختار sasl_ctx {
  ساختار SASL *sasl;
  ساختار connectdata *conn;
  unsigned short enabledmechs;
  ثابت نویسه *mech;
  saslstate state1;
  saslstate state2;
  ساختار bufref resp;
  CURLcode result;
};

ایستا bool sasl_choose_external(ساختار Curl_easy *data, ساختار sasl_ctx *sctx)
{
  اگر((sctx->enabledmechs & SASL_MECH_EXTERNAL) &&
     !Curl_creds_has_passwd(sctx->conn->creds)) {
    sctx->mech = SASL_MECH_STRING_EXTERNAL;
    sctx->state1 = SASL_EXTERNAL;
    sctx->sasl->authused = SASL_MECH_EXTERNAL;

    اگر(sctx->sasl->force_ir || data->set.sasl_ir)
      Curl_auth_create_external_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    بازگشت TRUE;
  }
  بازگشت FALSE;
}

#ifdef USE_KERBEROS5
ایستا bool sasl_choose_krb5(ساختار Curl_easy *data, ساختار sasl_ctx *sctx)
{
  اگر((sctx->enabledmechs & SASL_MECH_GSSAPI) &&
     Curl_auth_is_gssapi_supported() &&
     Curl_auth_user_contains_domain(sctx->conn->creds)) {
    ثابت نویسه *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;

    sctx->sasl->mutual_auth = FALSE;
    sctx->mech = SASL_MECH_STRING_GSSAPI;
    sctx->state1 = SASL_GSSAPI;
    sctx->state2 = SASL_GSSAPI_TOKEN;
    sctx->sasl->authused = SASL_MECH_GSSAPI;

    اگر(sctx->sasl->force_ir || data->set.sasl_ir) {
      ساختار kerberos5data *krb5 = Curl_auth_krb5_get(sctx->conn);
      sctx->result = !krb5 ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_gssapi_user_message(data, sctx->conn->creds,
                                             service,
                                             sctx->conn->origin->hostname,
                                             (bool)sctx->sasl->mutual_auth,
                                             پوچ, krb5, &sctx->resp);
    }
    بازگشت TRUE;
  }
  بازگشت FALSE;
}
#endif /* USE_KERBEROS5 */

#ifdef USE_GSASL
ایستا bool sasl_choose_gsasl(ساختار Curl_easy *data, ساختار sasl_ctx *sctx)
{
  ساختار gsasldata *gsasl;
  ساختار bufref nullmsg;

  اگر((sctx->enabledmechs &
      (SASL_MECH_SCRAM_SHA_256 | SASL_MECH_SCRAM_SHA_1))) {
    gsasl = Curl_auth_gsasl_get(sctx->conn);
    اگر(!gsasl) {
      sctx->result = CURLE_OUT_OF_MEMORY;
      بازگشت TRUE; /* attempted, but failed */
    }

    اگر((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_256) &&
       Curl_auth_gsasl_is_supported(data, SASL_MECH_STRING_SCRAM_SHA_256,
                                    gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_256;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_256;
    }
    وگرنه اگر((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_1) &&
            Curl_auth_gsasl_is_supported(data, SASL_MECH_STRING_SCRAM_SHA_1,
                                         gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_1;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_1;
    }
    وگرنه
      بازگشت FALSE;

    Curl_bufref_init(&nullmsg);
    sctx->state1 = SASL_GSASL;
    sctx->state2 = SASL_GSASL;
    sctx->result = Curl_auth_gsasl_start(data, sctx->conn->creds, gsasl);
    اگر(!sctx->result && (sctx->sasl->force_ir || data->set.sasl_ir))
      sctx->result = Curl_auth_gsasl_token(data, &nullmsg, gsasl, &sctx->resp);
    بازگشت TRUE;
  }
  بازگشت FALSE;
}

#endif /* USE_GSASL */

#ifndef CURL_DISABLE_DIGEST_AUTH
ایستا bool sasl_choose_digest(ساختار Curl_easy *data, ساختار sasl_ctx *sctx)
{
  (تهی)data;
  اگر((sctx->enabledmechs & SASL_MECH_DIGEST_MD5) &&
     Curl_auth_is_digest_supported()) {
    sctx->mech = SASL_MECH_STRING_DIGEST_MD5;
    sctx->state1 = SASL_DIGESTMD5;
    sctx->sasl->authused = SASL_MECH_DIGEST_MD5;
    بازگشت TRUE;
  }
  وگرنه اگر(sctx->enabledmechs & SASL_MECH_CRAM_MD5) {
    sctx->mech = SASL_MECH_STRING_CRAM_MD5;
    sctx->state1 = SASL_CRAMMD5;
    sctx->sasl->authused = SASL_MECH_CRAM_MD5;
    بازگشت TRUE;
  }
  بازگشت FALSE;
}
#endif /* !CURL_DISABLE_DIGEST_AUTH */

#ifdef USE_NTLM
ایستا bool sasl_choose_ntlm(ساختار Curl_easy *data, ساختار sasl_ctx *sctx)
{
  اگر((sctx->enabledmechs & SASL_MECH_NTLM) &&
     Curl_auth_is_ntlm_supported()) {
     ثابت نویسه *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;
    ثابت نویسه *hostname;

    Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, پوچ);

    sctx->mech = SASL_MECH_STRING_NTLM;
    sctx->state1 = SASL_NTLM;
    sctx->state2 = SASL_NTLM_TYPE2_پیام;
    sctx->sasl->authused = SASL_MECH_NTLM;

    اگر(sctx->sasl->force_ir || data->set.sasl_ir) {
      ساختار ntlmdata *ntlm = Curl_auth_ntlm_get(sctx->conn, FALSE);
      sctx->result = !ntlm ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_ntlm_type1_message(data, sctx->conn->creds,
                                            service, hostname,
                                            ntlm, &sctx->resp);
    }
    بازگشت TRUE;
  }
  بازگشت FALSE;
}
#endif /* USE_NTLM */

ایستا bool sasl_choose_oauth(ساختار Curl_easy *data, ساختار sasl_ctx *sctx)
{
  اگر(Curl_creds_has_oauth_bearer(data->state.creds) &&
     (sctx->enabledmechs & SASL_MECH_OAUTHBEARER)) {
    ثابت نویسه *hostname;
    صحیح port;
    Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, &port);

    sctx->mech = SASL_MECH_STRING_OAUTHBEARER;
    sctx->state1 = SASL_OAUTH2;
    sctx->state2 = SASL_OAUTH2_RESP;
    sctx->sasl->authused = SASL_MECH_OAUTHBEARER;

    اگر(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result =
        Curl_auth_create_oauth_bearer_message(sctx->conn->creds,
                                              hostname, port, &sctx->resp);
    بازگشت TRUE;
  }
  بازگشت FALSE;
}

ایستا bool sasl_choose_oauth2(ساختار Curl_easy *data, ساختار sasl_ctx *sctx)
{
  اگر(Curl_creds_has_oauth_bearer(sctx->conn->creds) &&
     (sctx->enabledmechs & SASL_MECH_XOAUTH2)) {
    sctx->mech = SASL_MECH_STRING_XOAUTH2;
    sctx->state1 = SASL_OAUTH2;
    sctx->sasl->authused = SASL_MECH_XOAUTH2;

    اگر(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result = Curl_auth_create_xoauth_bearer_message(
        sctx->conn->creds, &sctx->resp);
    بازگشت TRUE;
  }
  بازگشت FALSE;
}

ایستا bool sasl_choose_plain(ساختار Curl_easy *data, ساختار sasl_ctx *sctx)
{
  اگر(sctx->enabledmechs & SASL_MECH_PLAIN) {
    sctx->mech = SASL_MECH_STRING_PLAIN;
    sctx->state1 = SASL_PLAIN;
    sctx->sasl->authused = SASL_MECH_PLAIN;

    اگر(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result =
        Curl_auth_create_plain_message(sctx->conn->creds, &sctx->resp);
    بازگشت TRUE;
  }
  بازگشت FALSE;
}

ایستا bool sasl_choose_login(ساختار Curl_easy *data, ساختار sasl_ctx *sctx)
{
  اگر(sctx->enabledmechs & SASL_MECH_LOGIN) {
    sctx->mech = SASL_MECH_STRING_LOGIN;
    sctx->state1 = SASL_LOGIN;
    sctx->state2 = SASL_LOGIN_PASSWD;
    sctx->sasl->authused = SASL_MECH_LOGIN;

    اگر(sctx->sasl->force_ir || data->set.sasl_ir)
      Curl_auth_create_login_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    بازگشت TRUE;
  }
  بازگشت FALSE;
}

/*
 * Curl_sasl_start()
 *
 * Calculate the required login details for SASL authentication.
 */
CURLcode Curl_sasl_start(ساختار SASL *sasl, ساختار Curl_easy *data,
                         bool force_ir, saslprogress *progress)
{
  ساختار sasl_ctx sctx;

  sasl->force_ir = force_ir;    /* Latch for future use */
  sasl->authused = 0;           /* No mechanism used yet */
  *progress = SASL_IDLE;

  memset(&sctx, 0, اندازه(sctx));
  sctx.sasl = sasl;
  sctx.conn = data->conn;
  Curl_bufref_init(&sctx.resp);
  sctx.enabledmechs = sasl->authmechs & sasl->prefmech;
  sctx.state1 = SASL_STOP;
  sctx.state2 = SASL_FINAL;

  /* Calculate the supported authentication mechanism, by decreasing order of
     security, as well as the initial response where appropriate */
  اگر(sasl_choose_external(data, &sctx) ||
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

  اگر(!sctx.result && sctx.mech) {
    sasl->curmech = sctx.mech;
    اگر(Curl_bufref_ptr(&sctx.resp))
      sctx.result = build_message(sasl, &sctx.resp);

    اگر(sasl->params->maxirlen &&
       strlen(sctx.mech) + Curl_bufref_len(&sctx.resp) >
         sasl->params->maxirlen)
      Curl_bufref_free(&sctx.resp);

    اگر(!sctx.result)
      sctx.result = sasl->params->sendauth(data, sctx.mech, &sctx.resp);

    اگر(!sctx.result) {
      *progress = SASL_INPROGRESS;
      sasl_state(sasl, data, Curl_bufref_ptr(&sctx.resp) ?
                 sctx.state2 : sctx.state1);
    }
  }

  Curl_bufref_free(&sctx.resp);
  بازگشت sctx.result;
}

/*
 * Curl_sasl_continue()
 *
 * Continue the authentication.
 */
CURLcode Curl_sasl_continue(ساختار SASL *sasl, ساختار Curl_easy *data,
                            صحیح code, saslprogress *progress)
{
  CURLcode result = CURLE_OK;
  ساختار connectdata *conn = data->conn;
  saslstate newstate = SASL_FINAL;
  ساختار bufref resp;
  ثابت نویسه *hostname;
  صحیح port;
  ساختار bufref serverdata;

  Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, &port);
  Curl_bufref_init(&serverdata);
  Curl_bufref_init(&resp);
  *progress = SASL_INPROGRESS;

  اگر(sasl->state == SASL_FINAL) {
    اگر(code != sasl->params->finalcode)
      result = CURLE_LOGIN_DENIED;
    *progress = SASL_DONE;
    sasl_state(sasl, data, SASL_STOP);
    بازگشت result;
  }

  اگر(sasl->state != SASL_CANCEL && sasl->state != SASL_OAUTH2_RESP &&
     code != sasl->params->contcode) {
    *progress = SASL_DONE;
    sasl_state(sasl, data, SASL_STOP);
    بازگشت CURLE_LOGIN_DENIED;
  }

  انتخاب(sasl->state) {
  حالت SASL_STOP:
    *progress = SASL_DONE;
    بازگشت result;
  حالت SASL_PLAIN:
    result = Curl_auth_create_plain_message(conn->creds, &resp);
    بشکن;
  حالت SASL_LOGIN:
    Curl_auth_create_login_message(Curl_creds_user(conn->creds), &resp);
    newstate = SASL_LOGIN_PASSWD;
    بشکن;
  حالت SASL_LOGIN_PASSWD:
    Curl_auth_create_login_message(Curl_creds_passwd(conn->creds), &resp);
    بشکن;
  حالت SASL_EXTERNAL:
    Curl_auth_create_external_message(Curl_creds_user(conn->creds), &resp);
    بشکن;
#ifdef USE_GSASL
  حالت SASL_GSASL:
    result = get_server_message(sasl, data, &serverdata);
    اگر(!result) {
      ساختار gsasldata *gsasl = Curl_auth_gsasl_get(conn);
      result = !gsasl ? CURLE_OUT_OF_MEMORY :
        Curl_auth_gsasl_token(data, &serverdata, gsasl, &resp);
    }
    اگر(!result && Curl_bufref_len(&resp) > 0)
      newstate = SASL_GSASL;
    بشکن;
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  حالت SASL_CRAMMD5:
    result = get_server_message(sasl, data, &serverdata);
    اگر(!result)
      result = Curl_auth_create_cram_md5_message(&serverdata, conn->creds,
                                                 &resp);
    بشکن;
  حالت SASL_DIGESTMD5:
    result = get_server_message(sasl, data, &serverdata);
    اگر(!result)
      result = Curl_auth_create_digest_md5_message(data, &serverdata,
                                                   conn->creds,
                                                   sasl->params->service,
                                                   &resp);
    اگر(!result && (sasl->params->flags & SASL_FLAG_BASE64))
      newstate = SASL_DIGESTMD5_RESP;
    بشکن;
  حالت SASL_DIGESTMD5_RESP:
    /* Keep response NULL to output an empty line. */
    بشکن;
#endif

#ifdef USE_NTLM
  حالت SASL_NTLM: {
    /* Create the type-1 message */
    ساختار ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    result = !ntlm ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_ntlm_type1_message(data, conn->creds,
                                          sasl->params->service, hostname,
                                          ntlm, &resp);
    newstate = SASL_NTLM_TYPE2_پیام;
    بشکن;
  }
  حالت SASL_NTLM_TYPE2_پیام: {
    /* Decode the type-2 message */
    ساختار ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    result = !ntlm ? CURLE_OUT_OF_MEMORY :
      get_server_message(sasl, data, &serverdata);
    اگر(!result)
      result = Curl_auth_decode_ntlm_type2_message(data, &serverdata, ntlm);
    اگر(!result)
      result = Curl_auth_create_ntlm_type3_message(data, conn->creds,
                                                   ntlm, &resp);
    بشکن;
  }
#endif

#ifdef USE_KERBEROS5
  حالت SASL_GSSAPI: {
    ساختار kerberos5data *krb5 = Curl_auth_krb5_get(conn);
    result = !krb5 ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_gssapi_user_message(data, conn->creds,
                                           sasl->params->service,
                                           conn->origin->hostname,
                                           (bool)sasl->mutual_auth, پوچ,
                                           krb5, &resp);
    newstate = SASL_GSSAPI_TOKEN;
    بشکن;
  }
  حالت SASL_GSSAPI_TOKEN:
    result = get_server_message(sasl, data, &serverdata);
    اگر(!result) {
      ساختار kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      اگر(!krb5)
        result = CURLE_OUT_OF_MEMORY;
      وگرنه اگر(sasl->mutual_auth) {
        /* Decode the user token challenge and create the optional response
           message */
        result = Curl_auth_create_gssapi_user_message(data, پوچ,
                                                      پوچ, پوچ,
                                                      (bool)sasl->mutual_auth,
                                                      &serverdata,
                                                      krb5, &resp);
        newstate = SASL_GSSAPI_NO_DATA;
      }
      وگرنه
        /* Decode the security challenge and create the response message */
        result = Curl_auth_create_gssapi_security_message(
          data, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    بشکن;
  حالت SASL_GSSAPI_NO_DATA:
    /* Decode the security challenge and create the response message */
    result = get_server_message(sasl, data, &serverdata);
    اگر(!result) {
      ساختار kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      اگر(!krb5)
        result = CURLE_OUT_OF_MEMORY;
      وگرنه
        result = Curl_auth_create_gssapi_security_message(
          data, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    بشکن;
#endif

  حالت SASL_OAUTH2:
    /* Create the authorization message */
    اگر(sasl->authused == SASL_MECH_OAUTHBEARER) {
      result = Curl_auth_create_oauth_bearer_message(conn->creds,
                                                     hostname,
                                                     port,
                                                     &resp);

      /* Failures maybe sent by the server as continuations for OAUTHBEARER */
      newstate = SASL_OAUTH2_RESP;
    }
    وگرنه
      result = Curl_auth_create_xoauth_bearer_message(conn->creds,
                                                      &resp);
    بشکن;

  حالت SASL_OAUTH2_RESP:
    /* The continuation is optional so check the response code */
    اگر(code == sasl->params->finalcode) {
      /* Final response was received so we are done */
      *progress = SASL_DONE;
      sasl_state(sasl, data, SASL_STOP);
      بازگشت result;
    }
    وگرنه اگر(code == sasl->params->contcode) {
      /* Acknowledge the continuation by sending a 0x01 response. */
      Curl_bufref_set(&resp, "\x01", 1, پوچ);
      بشکن;
    }
    وگرنه {
      *progress = SASL_DONE;
      sasl_state(sasl, data, SASL_STOP);
      بازگشت CURLE_LOGIN_DENIED;
    }

  حالت SASL_CANCEL:
    /* Remove the offending mechanism from the supported list */
    sasl->authmechs &= (unsigned short)~sasl->authused;
    sasl->authused = SASL_AUTH_NONE;
    sasl->curmech = پوچ;

    /* Start an alternative SASL authentication */
    بازگشت Curl_sasl_start(sasl, data, (bool)sasl->force_ir, progress);
  پیشفرض:
    failf(data, "Unsupported SASL authentication mechanism");
    result = CURLE_UNSUPPORTED_PROTOCOL;  /* Should not happen */
    بشکن;
  }

  Curl_bufref_free(&serverdata);

  انتخاب(result) {
  حالت CURLE_BAD_CONTENT_ENCODING:
    /* Cancel dialog */
    result = sasl->params->cancelauth(data, sasl->curmech);
    newstate = SASL_CANCEL;
    بشکن;
  حالت CURLE_OK:
    result = build_message(sasl, &resp);
    اگر(!result)
      result = sasl->params->contauth(data, sasl->curmech, &resp);
    بشکن;
  پیشفرض:
    newstate = SASL_STOP;    /* Stop on error */
    *progress = SASL_DONE;
    بشکن;
  }

  Curl_bufref_free(&resp);

  sasl_state(sasl, data, newstate);

  بازگشت result;
}

#ifdef CURLVERBOSE
ایستا تهی sasl_unchosen(ساختار Curl_easy *data, unsigned short mech,
                          unsigned short enabledmechs,
                          bool built_in, bool platform,
                          ثابت نویسه *param_missing)
{
  ثابت نویسه *mname = پوچ;
  size_t i;

  اگر(!(enabledmechs & mech))
    بازگشت;

  برای(i = 0; mechtable[i].name; ++i) {
    اگر(mechtable[i].bit == mech) {
      mname = mechtable[i].name;
      بشکن;
    }
  }
  اگر(!mname)  /* should not happen */
    بازگشت;
  اگر(!built_in)
    infof(data, "SASL: %s not builtin", mname);
  وگرنه اگر(!platform)
    infof(data, "SASL: %s not supported by the platform/libraries", mname);
  وگرنه {
    اگر(param_missing)
      infof(data, "SASL: %s is missing %s", mname, param_missing);
    اگر(!Curl_creds_has_user(data->conn->creds))
      infof(data, "SASL: %s is missing username", mname);
  }
}
#endif /* CURLVERBOSE */

CURLcode Curl_sasl_is_blocked(ساختار SASL *sasl, ساختار Curl_easy *data)
{
#ifdef CURLVERBOSE
#ifdef USE_KERBEROS5
#define CURL_SASL_KERBEROS5   TRUE
#وگرنه
#define CURL_SASL_KERBEROS5   FALSE
#endif
#ifdef USE_GSASL
#define CURL_SASL_GASL        TRUE
#وگرنه
#define CURL_SASL_GASL        FALSE
#endif
#ifdef CURL_DISABLE_DIGEST_AUTH
#define CURL_SASL_DIGEST      TRUE
#وگرنه
#define CURL_SASL_DIGEST      FALSE
#endif
#ifndef USE_NTLM
#define CURL_SASL_NTLM        TRUE
#وگرنه
#define CURL_SASL_NTLM        FALSE
#endif
  /* Failing SASL authentication is a pain. Give a helping hand if
   * we were unable to select an AUTH mechanism.
   * `sasl->authmechs` are mechanisms offered by the peer
   * `sasl->prefmech`  are mechanisms preferred by us */
  unsigned short enabledmechs = sasl->authmechs & sasl->prefmech;

  اگر(!sasl->authmechs)
    infof(data, "SASL: no auth mechanism was offered or recognized");
  وگرنه اگر(!enabledmechs)
    infof(data, "SASL: no overlap between offered and configured "
          "auth mechanisms");
  وگرنه {
    infof(data, "SASL: no auth mechanism offered could be selected");
    اگر((enabledmechs & SASL_MECH_EXTERNAL) &&
       Curl_creds_has_passwd(data->conn->creds))
      infof(data, "SASL: auth EXTERNAL not chosen with password");
    sasl_unchosen(data, SASL_MECH_GSSAPI, enabledmechs,
                  CURL_SASL_KERBEROS5, Curl_auth_is_gssapi_supported(), پوچ);
    sasl_unchosen(data, SASL_MECH_SCRAM_SHA_256, enabledmechs,
                  CURL_SASL_GASL, FALSE, پوچ);
    sasl_unchosen(data, SASL_MECH_SCRAM_SHA_1, enabledmechs,
                  CURL_SASL_GASL, FALSE, پوچ);
    sasl_unchosen(data, SASL_MECH_DIGEST_MD5, enabledmechs,
                  CURL_SASL_DIGEST, Curl_auth_is_digest_supported(), پوچ);
    sasl_unchosen(data, SASL_MECH_CRAM_MD5, enabledmechs,
                  CURL_SASL_DIGEST, TRUE, پوچ);
    sasl_unchosen(data, SASL_MECH_NTLM, enabledmechs,
                  CURL_SASL_NTLM, Curl_auth_is_ntlm_supported(), پوچ);
    sasl_unchosen(data, SASL_MECH_OAUTHBEARER, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(data->conn->creds) ?
                  پوچ : "CURLOPT_XOAUTH2_BEARER");
    sasl_unchosen(data, SASL_MECH_XOAUTH2, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(data->conn->creds) ?
                  پوچ : "CURLOPT_XOAUTH2_BEARER");
  }
#endif /* CURLVERBOSE */
  (تهی)sasl;
  (تهی)data;
  بازگشت CURLE_LOGIN_DENIED;
}

#endif /* protocols are enabled that use SASL */
