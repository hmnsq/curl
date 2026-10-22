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
#ضمن "curl_setup.h"

#אם !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#ضمن "urldata.h"
#ضمن "curlx/base64.h"
#ضمن "vauth/vauth.h"
#ضمن "cfilters.h"
#ضمن "curl_sasl.h"
#ضمن "curl_trc.h"

/* Supported mechanisms */
ساكن ثابت بنية {
  ثابت حرف    *name;  /* Name */
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
unsigned short Curl_sasl_decode_mech(ثابت حرف *ptr, size_t maxlen,
                                     size_t *len)
{
  unsigned صحيح i;
  حرف c;

  עבור(i = 0; mechtable[i].name; i++) {
    אם(maxlen >= mechtable[i].len &&
       curl_strnequal(ptr, mechtable[i].name, mechtable[i].len)) {
      אם(len)
        *len = mechtable[i].len;

      אם(maxlen == mechtable[i].len)
        החזר mechtable[i].bit;

      c = ptr[mechtable[i].len];
      אם(!ISUPPER(c) && !ISDIGIT(c) && c != '-' && c != '_')
        החזר mechtable[i].bit;
    }
  }

  החזר 0;
}

/*
 * Curl_sasl_parse_url_auth_option()
 *
 * Parse the URL login options.
 */
CURLcode Curl_sasl_parse_url_auth_option(بنية SASL *sasl,
                                         ثابت حرف *value, size_t len)
{
  CURLcode result = CURLE_OK;
  size_t mechlen;

  אם(!len)
    החזר CURLE_URL_MALFORMAT;

  אם(sasl->resetprefs) {
    sasl->resetprefs = FALSE;
    sasl->prefmech = SASL_AUTH_NONE;
  }

  אם(!strncmp(value, "*", len))
    sasl->prefmech = SASL_AUTH_DEFAULT;
  אחרת {
    unsigned short mechbit = Curl_sasl_decode_mech(value, len, &mechlen);
    אם(mechbit && mechlen == len)
      sasl->prefmech |= mechbit;
    אחרת
      result = CURLE_URL_MALFORMAT;
  }

  החזר result;
}

/*
 * Curl_sasl_init()
 *
 * Initializes the SASL structure.
 */
فراغ Curl_sasl_init(بنية SASL *sasl, بنية Curl_easy *data,
                    ثابت بنية SASLproto *params)
{
  uint32_t auth = data->set.httpauth;

  sasl->params = params;           /* Set protocol dependent parameters */
  sasl->state = SASL_STOP;         /* Not yet running */
  sasl->curmech = عدم;            /* No mechanism yet. */
  sasl->authmechs = SASL_AUTH_NONE; /* No known authentication mechanism yet */
  sasl->prefmech = params->defmechs; /* Default preferred mechanisms */
  sasl->authused = SASL_AUTH_NONE; /* The authentication mechanism used */
  sasl->resetprefs = TRUE;         /* Reset prefmech upon AUTH parsing. */
  sasl->mutual_auth = FALSE;       /* No mutual authentication (GSSAPI only) */
  sasl->force_ir = FALSE;          /* Respect external option */

  אם(auth != CURLAUTH_BASIC) {
    unsigned short mechs = SASL_AUTH_NONE;

    /* If some usable http authentication options have been set, determine
       new defaults from them. */
    אם(auth & CURLAUTH_BASIC)
      mechs |= SASL_MECH_PLAIN | SASL_MECH_LOGIN;
    אם(auth & CURLAUTH_DIGEST)
      mechs |= SASL_MECH_DIGEST_MD5;
    אם(auth & CURLAUTH_NTLM)
      mechs |= SASL_MECH_NTLM;
    אם(auth & CURLAUTH_BEARER)
      mechs |= SASL_MECH_OAUTHBEARER | SASL_MECH_XOAUTH2;
    אם(auth & CURLAUTH_GSSAPI)
      mechs |= SASL_MECH_GSSAPI;

    אם(mechs != SASL_AUTH_NONE)
      sasl->prefmech = mechs;
  }
}

/*
 * sasl_state()
 *
 * This is the ONLY way to change SASL state!
 */
ساكن فراغ sasl_state(بنية SASL *sasl, بنية Curl_easy *data,
                       saslstate newstate)
{
#אם defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ساكن ثابت حرف * ثابت names[] = {
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

  אם(sasl->state != newstate)
    infof(data, "SASL %p state change from %s to %s",
          (فراغ *)sasl, names[sasl->state], names[newstate]);
#אחרת
  (فراغ)data;
#endif

  sasl->state = newstate;
}

#אם defined(USE_NTLM) || defined(USE_GSASL) || defined(USE_KERBEROS5) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)
/* Get the SASL server message and convert it to binary. */
ساكن CURLcode get_server_message(بنية SASL *sasl, بنية Curl_easy *data,
                                   بنية bufref *out)
{
  CURLcode result = CURLE_OK;

  result = sasl->params->getmessage(data, out);
  אם(!result && (sasl->params->flags & SASL_FLAG_BASE64)) {
    ثابت حرف *serverdata = Curl_bufref_ptr(out);

    אם(!*serverdata)
      Curl_bufref_set(out, عدم, 0, عدم);
    אחרת {
      unsigned حرف *msg;
      size_t msglen;

      result = curlx_base64_decode(serverdata, &msg, &msglen);
      אם(!result)
        Curl_bufref_set(out, msg, msglen, curl_free);
    }
  }
  החזר result;
}
#endif

/* Encode the outgoing SASL message. */
ساكن CURLcode build_message(بنية SASL *sasl, بنية bufref *msg)
{
  CURLcode result = CURLE_OK;

  אם(sasl->params->flags & SASL_FLAG_BASE64) {
    אם(!Curl_bufref_ptr(msg))                   /* Empty message. */
      Curl_bufref_set(msg, "", 0, عدم);
    אחרת אם(Curl_bufref_len(msg)) {
      حرف *base64;
      size_t base64len;

      result = curlx_base64_encode(Curl_bufref_uptr(msg),
                                   Curl_bufref_len(msg), &base64, &base64len);
      אם(!result)
        Curl_bufref_set(msg, base64, base64len, curl_free);
    }
  }

  החזר result;
}

/*
 * Curl_sasl_can_authenticate()
 *
 * Check if we have enough auth data and capabilities to authenticate.
 */
bool Curl_sasl_can_authenticate(بنية SASL *sasl, بنية Curl_easy *data)
{
  /* Have credentials been provided? */
  אם(data->conn->creds)
    החזר TRUE;

  /* EXTERNAL can authenticate without a username and/or password */
  אם(sasl->authmechs & sasl->prefmech & SASL_MECH_EXTERNAL)
    החזר TRUE;

  החזר FALSE;
}

بنية sasl_ctx {
  بنية SASL *sasl;
  بنية connectdata *conn;
  unsigned short enabledmechs;
  ثابت حرف *mech;
  saslstate state1;
  saslstate state2;
  بنية bufref resp;
  CURLcode result;
};

ساكن bool sasl_choose_external(بنية Curl_easy *data, بنية sasl_ctx *sctx)
{
  אם((sctx->enabledmechs & SASL_MECH_EXTERNAL) &&
     !Curl_creds_has_passwd(sctx->conn->creds)) {
    sctx->mech = SASL_MECH_STRING_EXTERNAL;
    sctx->state1 = SASL_EXTERNAL;
    sctx->sasl->authused = SASL_MECH_EXTERNAL;

    אם(sctx->sasl->force_ir || data->set.sasl_ir)
      Curl_auth_create_external_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    החזר TRUE;
  }
  החזר FALSE;
}

#ifdef USE_KERBEROS5
ساكن bool sasl_choose_krb5(بنية Curl_easy *data, بنية sasl_ctx *sctx)
{
  אם((sctx->enabledmechs & SASL_MECH_GSSAPI) &&
     Curl_auth_is_gssapi_supported() &&
     Curl_auth_user_contains_domain(sctx->conn->creds)) {
    ثابت حرف *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;

    sctx->sasl->mutual_auth = FALSE;
    sctx->mech = SASL_MECH_STRING_GSSAPI;
    sctx->state1 = SASL_GSSAPI;
    sctx->state2 = SASL_GSSAPI_TOKEN;
    sctx->sasl->authused = SASL_MECH_GSSAPI;

    אם(sctx->sasl->force_ir || data->set.sasl_ir) {
      بنية kerberos5data *krb5 = Curl_auth_krb5_get(sctx->conn);
      sctx->result = !krb5 ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_gssapi_user_message(data, sctx->conn->creds,
                                             service,
                                             sctx->conn->origin->hostname,
                                             (bool)sctx->sasl->mutual_auth,
                                             عدم, krb5, &sctx->resp);
    }
    החזר TRUE;
  }
  החזר FALSE;
}
#endif /* USE_KERBEROS5 */

#ifdef USE_GSASL
ساكن bool sasl_choose_gsasl(بنية Curl_easy *data, بنية sasl_ctx *sctx)
{
  بنية gsasldata *gsasl;
  بنية bufref nullmsg;

  אם((sctx->enabledmechs &
      (SASL_MECH_SCRAM_SHA_256 | SASL_MECH_SCRAM_SHA_1))) {
    gsasl = Curl_auth_gsasl_get(sctx->conn);
    אם(!gsasl) {
      sctx->result = CURLE_OUT_OF_MEMORY;
      החזר TRUE; /* attempted, but failed */
    }

    אם((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_256) &&
       Curl_auth_gsasl_is_supported(data, SASL_MECH_STRING_SCRAM_SHA_256,
                                    gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_256;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_256;
    }
    אחרת אם((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_1) &&
            Curl_auth_gsasl_is_supported(data, SASL_MECH_STRING_SCRAM_SHA_1,
                                         gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_1;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_1;
    }
    אחרת
      החזר FALSE;

    Curl_bufref_init(&nullmsg);
    sctx->state1 = SASL_GSASL;
    sctx->state2 = SASL_GSASL;
    sctx->result = Curl_auth_gsasl_start(data, sctx->conn->creds, gsasl);
    אם(!sctx->result && (sctx->sasl->force_ir || data->set.sasl_ir))
      sctx->result = Curl_auth_gsasl_token(data, &nullmsg, gsasl, &sctx->resp);
    החזר TRUE;
  }
  החזר FALSE;
}

#endif /* USE_GSASL */

#ifndef CURL_DISABLE_DIGEST_AUTH
ساكن bool sasl_choose_digest(بنية Curl_easy *data, بنية sasl_ctx *sctx)
{
  (فراغ)data;
  אם((sctx->enabledmechs & SASL_MECH_DIGEST_MD5) &&
     Curl_auth_is_digest_supported()) {
    sctx->mech = SASL_MECH_STRING_DIGEST_MD5;
    sctx->state1 = SASL_DIGESTMD5;
    sctx->sasl->authused = SASL_MECH_DIGEST_MD5;
    החזר TRUE;
  }
  אחרת אם(sctx->enabledmechs & SASL_MECH_CRAM_MD5) {
    sctx->mech = SASL_MECH_STRING_CRAM_MD5;
    sctx->state1 = SASL_CRAMMD5;
    sctx->sasl->authused = SASL_MECH_CRAM_MD5;
    החזר TRUE;
  }
  החזר FALSE;
}
#endif /* !CURL_DISABLE_DIGEST_AUTH */

#ifdef USE_NTLM
ساكن bool sasl_choose_ntlm(بنية Curl_easy *data, بنية sasl_ctx *sctx)
{
  אם((sctx->enabledmechs & SASL_MECH_NTLM) &&
     Curl_auth_is_ntlm_supported()) {
     ثابت حرف *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;
    ثابت حرف *hostname;

    Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, عدم);

    sctx->mech = SASL_MECH_STRING_NTLM;
    sctx->state1 = SASL_NTLM;
    sctx->state2 = SASL_NTLM_TYPE2MSG;
    sctx->sasl->authused = SASL_MECH_NTLM;

    אם(sctx->sasl->force_ir || data->set.sasl_ir) {
      بنية ntlmdata *ntlm = Curl_auth_ntlm_get(sctx->conn, FALSE);
      sctx->result = !ntlm ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_ntlm_type1_message(data, sctx->conn->creds,
                                            service, hostname,
                                            ntlm, &sctx->resp);
    }
    החזר TRUE;
  }
  החזר FALSE;
}
#endif /* USE_NTLM */

ساكن bool sasl_choose_oauth(بنية Curl_easy *data, بنية sasl_ctx *sctx)
{
  אם(Curl_creds_has_oauth_bearer(data->state.creds) &&
     (sctx->enabledmechs & SASL_MECH_OAUTHBEARER)) {
    ثابت حرف *hostname;
    صحيح port;
    Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, &port);

    sctx->mech = SASL_MECH_STRING_OAUTHBEARER;
    sctx->state1 = SASL_OAUTH2;
    sctx->state2 = SASL_OAUTH2_RESP;
    sctx->sasl->authused = SASL_MECH_OAUTHBEARER;

    אם(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result =
        Curl_auth_create_oauth_bearer_message(sctx->conn->creds,
                                              hostname, port, &sctx->resp);
    החזר TRUE;
  }
  החזר FALSE;
}

ساكن bool sasl_choose_oauth2(بنية Curl_easy *data, بنية sasl_ctx *sctx)
{
  אם(Curl_creds_has_oauth_bearer(sctx->conn->creds) &&
     (sctx->enabledmechs & SASL_MECH_XOAUTH2)) {
    sctx->mech = SASL_MECH_STRING_XOAUTH2;
    sctx->state1 = SASL_OAUTH2;
    sctx->sasl->authused = SASL_MECH_XOAUTH2;

    אם(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result = Curl_auth_create_xoauth_bearer_message(
        sctx->conn->creds, &sctx->resp);
    החזר TRUE;
  }
  החזר FALSE;
}

ساكن bool sasl_choose_plain(بنية Curl_easy *data, بنية sasl_ctx *sctx)
{
  אם(sctx->enabledmechs & SASL_MECH_PLAIN) {
    sctx->mech = SASL_MECH_STRING_PLAIN;
    sctx->state1 = SASL_PLAIN;
    sctx->sasl->authused = SASL_MECH_PLAIN;

    אם(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result =
        Curl_auth_create_plain_message(sctx->conn->creds, &sctx->resp);
    החזר TRUE;
  }
  החזר FALSE;
}

ساكن bool sasl_choose_login(بنية Curl_easy *data, بنية sasl_ctx *sctx)
{
  אם(sctx->enabledmechs & SASL_MECH_LOGIN) {
    sctx->mech = SASL_MECH_STRING_LOGIN;
    sctx->state1 = SASL_LOGIN;
    sctx->state2 = SASL_LOGIN_PASSWD;
    sctx->sasl->authused = SASL_MECH_LOGIN;

    אם(sctx->sasl->force_ir || data->set.sasl_ir)
      Curl_auth_create_login_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    החזר TRUE;
  }
  החזר FALSE;
}

/*
 * Curl_sasl_start()
 *
 * Calculate the required login details for SASL authentication.
 */
CURLcode Curl_sasl_start(بنية SASL *sasl, بنية Curl_easy *data,
                         bool force_ir, saslprogress *progress)
{
  بنية sasl_ctx sctx;

  sasl->force_ir = force_ir;    /* Latch for future use */
  sasl->authused = 0;           /* No mechanism used yet */
  *progress = SASL_IDLE;

  memset(&sctx, 0, حجم(sctx));
  sctx.sasl = sasl;
  sctx.conn = data->conn;
  Curl_bufref_init(&sctx.resp);
  sctx.enabledmechs = sasl->authmechs & sasl->prefmech;
  sctx.state1 = SASL_STOP;
  sctx.state2 = SASL_FINAL;

  /* Calculate the supported authentication mechanism, by decreasing order of
     security, as well as the initial response where appropriate */
  אם(sasl_choose_external(data, &sctx) ||
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

  אם(!sctx.result && sctx.mech) {
    sasl->curmech = sctx.mech;
    אם(Curl_bufref_ptr(&sctx.resp))
      sctx.result = build_message(sasl, &sctx.resp);

    אם(sasl->params->maxirlen &&
       strlen(sctx.mech) + Curl_bufref_len(&sctx.resp) >
         sasl->params->maxirlen)
      Curl_bufref_free(&sctx.resp);

    אם(!sctx.result)
      sctx.result = sasl->params->sendauth(data, sctx.mech, &sctx.resp);

    אם(!sctx.result) {
      *progress = SASL_INPROGRESS;
      sasl_state(sasl, data, Curl_bufref_ptr(&sctx.resp) ?
                 sctx.state2 : sctx.state1);
    }
  }

  Curl_bufref_free(&sctx.resp);
  החזר sctx.result;
}

/*
 * Curl_sasl_continue()
 *
 * Continue the authentication.
 */
CURLcode Curl_sasl_continue(بنية SASL *sasl, بنية Curl_easy *data,
                            صحيح code, saslprogress *progress)
{
  CURLcode result = CURLE_OK;
  بنية connectdata *conn = data->conn;
  saslstate newstate = SASL_FINAL;
  بنية bufref resp;
  ثابت حرف *hostname;
  صحيح port;
  بنية bufref serverdata;

  Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, &port);
  Curl_bufref_init(&serverdata);
  Curl_bufref_init(&resp);
  *progress = SASL_INPROGRESS;

  אם(sasl->state == SASL_FINAL) {
    אם(code != sasl->params->finalcode)
      result = CURLE_LOGIN_DENIED;
    *progress = SASL_DONE;
    sasl_state(sasl, data, SASL_STOP);
    החזר result;
  }

  אם(sasl->state != SASL_CANCEL && sasl->state != SASL_OAUTH2_RESP &&
     code != sasl->params->contcode) {
    *progress = SASL_DONE;
    sasl_state(sasl, data, SASL_STOP);
    החזר CURLE_LOGIN_DENIED;
  }

  بدل(sasl->state) {
  حالة SASL_STOP:
    *progress = SASL_DONE;
    החזר result;
  حالة SASL_PLAIN:
    result = Curl_auth_create_plain_message(conn->creds, &resp);
    اكسر;
  حالة SASL_LOGIN:
    Curl_auth_create_login_message(Curl_creds_user(conn->creds), &resp);
    newstate = SASL_LOGIN_PASSWD;
    اكسر;
  حالة SASL_LOGIN_PASSWD:
    Curl_auth_create_login_message(Curl_creds_passwd(conn->creds), &resp);
    اكسر;
  حالة SASL_EXTERNAL:
    Curl_auth_create_external_message(Curl_creds_user(conn->creds), &resp);
    اكسر;
#ifdef USE_GSASL
  حالة SASL_GSASL:
    result = get_server_message(sasl, data, &serverdata);
    אם(!result) {
      بنية gsasldata *gsasl = Curl_auth_gsasl_get(conn);
      result = !gsasl ? CURLE_OUT_OF_MEMORY :
        Curl_auth_gsasl_token(data, &serverdata, gsasl, &resp);
    }
    אם(!result && Curl_bufref_len(&resp) > 0)
      newstate = SASL_GSASL;
    اكسر;
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  حالة SASL_CRAMMD5:
    result = get_server_message(sasl, data, &serverdata);
    אם(!result)
      result = Curl_auth_create_cram_md5_message(&serverdata, conn->creds,
                                                 &resp);
    اكسر;
  حالة SASL_DIGESTMD5:
    result = get_server_message(sasl, data, &serverdata);
    אם(!result)
      result = Curl_auth_create_digest_md5_message(data, &serverdata,
                                                   conn->creds,
                                                   sasl->params->service,
                                                   &resp);
    אם(!result && (sasl->params->flags & SASL_FLAG_BASE64))
      newstate = SASL_DIGESTMD5_RESP;
    اكسر;
  حالة SASL_DIGESTMD5_RESP:
    /* Keep response NULL to output an empty line. */
    اكسر;
#endif

#ifdef USE_NTLM
  حالة SASL_NTLM: {
    /* Create the type-1 message */
    بنية ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    result = !ntlm ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_ntlm_type1_message(data, conn->creds,
                                          sasl->params->service, hostname,
                                          ntlm, &resp);
    newstate = SASL_NTLM_TYPE2MSG;
    اكسر;
  }
  حالة SASL_NTLM_TYPE2MSG: {
    /* Decode the type-2 message */
    بنية ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    result = !ntlm ? CURLE_OUT_OF_MEMORY :
      get_server_message(sasl, data, &serverdata);
    אם(!result)
      result = Curl_auth_decode_ntlm_type2_message(data, &serverdata, ntlm);
    אם(!result)
      result = Curl_auth_create_ntlm_type3_message(data, conn->creds,
                                                   ntlm, &resp);
    اكسر;
  }
#endif

#ifdef USE_KERBEROS5
  حالة SASL_GSSAPI: {
    بنية kerberos5data *krb5 = Curl_auth_krb5_get(conn);
    result = !krb5 ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_gssapi_user_message(data, conn->creds,
                                           sasl->params->service,
                                           conn->origin->hostname,
                                           (bool)sasl->mutual_auth, عدم,
                                           krb5, &resp);
    newstate = SASL_GSSAPI_TOKEN;
    اكسر;
  }
  حالة SASL_GSSAPI_TOKEN:
    result = get_server_message(sasl, data, &serverdata);
    אם(!result) {
      بنية kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      אם(!krb5)
        result = CURLE_OUT_OF_MEMORY;
      אחרת אם(sasl->mutual_auth) {
        /* Decode the user token challenge and create the optional response
           message */
        result = Curl_auth_create_gssapi_user_message(data, عدم,
                                                      عدم, عدم,
                                                      (bool)sasl->mutual_auth,
                                                      &serverdata,
                                                      krb5, &resp);
        newstate = SASL_GSSAPI_NO_DATA;
      }
      אחרת
        /* Decode the security challenge and create the response message */
        result = Curl_auth_create_gssapi_security_message(
          data, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    اكسر;
  حالة SASL_GSSAPI_NO_DATA:
    /* Decode the security challenge and create the response message */
    result = get_server_message(sasl, data, &serverdata);
    אם(!result) {
      بنية kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      אם(!krb5)
        result = CURLE_OUT_OF_MEMORY;
      אחרת
        result = Curl_auth_create_gssapi_security_message(
          data, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    اكسر;
#endif

  حالة SASL_OAUTH2:
    /* Create the authorization message */
    אם(sasl->authused == SASL_MECH_OAUTHBEARER) {
      result = Curl_auth_create_oauth_bearer_message(conn->creds,
                                                     hostname,
                                                     port,
                                                     &resp);

      /* Failures maybe sent by the server as continuations for OAUTHBEARER */
      newstate = SASL_OAUTH2_RESP;
    }
    אחרת
      result = Curl_auth_create_xoauth_bearer_message(conn->creds,
                                                      &resp);
    اكسر;

  حالة SASL_OAUTH2_RESP:
    /* The continuation is optional so check the response code */
    אם(code == sasl->params->finalcode) {
      /* Final response was received so we are done */
      *progress = SASL_DONE;
      sasl_state(sasl, data, SASL_STOP);
      החזר result;
    }
    אחרת אם(code == sasl->params->contcode) {
      /* Acknowledge the continuation by sending a 0x01 response. */
      Curl_bufref_set(&resp, "\x01", 1, عدم);
      اكسر;
    }
    אחרת {
      *progress = SASL_DONE;
      sasl_state(sasl, data, SASL_STOP);
      החזר CURLE_LOGIN_DENIED;
    }

  حالة SASL_CANCEL:
    /* Remove the offending mechanism from the supported list */
    sasl->authmechs &= (unsigned short)~sasl->authused;
    sasl->authused = SASL_AUTH_NONE;
    sasl->curmech = عدم;

    /* Start an alternative SASL authentication */
    החזר Curl_sasl_start(sasl, data, (bool)sasl->force_ir, progress);
  افتراضي:
    failf(data, "Unsupported SASL authentication mechanism");
    result = CURLE_UNSUPPORTED_PROTOCOL;  /* Should not happen */
    اكسر;
  }

  Curl_bufref_free(&serverdata);

  بدل(result) {
  حالة CURLE_BAD_CONTENT_ENCODING:
    /* Cancel dialog */
    result = sasl->params->cancelauth(data, sasl->curmech);
    newstate = SASL_CANCEL;
    اكسر;
  حالة CURLE_OK:
    result = build_message(sasl, &resp);
    אם(!result)
      result = sasl->params->contauth(data, sasl->curmech, &resp);
    اكسر;
  افتراضي:
    newstate = SASL_STOP;    /* Stop on error */
    *progress = SASL_DONE;
    اكسر;
  }

  Curl_bufref_free(&resp);

  sasl_state(sasl, data, newstate);

  החזר result;
}

#ifdef CURLVERBOSE
ساكن فراغ sasl_unchosen(بنية Curl_easy *data, unsigned short mech,
                          unsigned short enabledmechs,
                          bool built_in, bool platform,
                          ثابت حرف *param_missing)
{
  ثابت حرف *mname = عدم;
  size_t i;

  אם(!(enabledmechs & mech))
    החזר;

  עבור(i = 0; mechtable[i].name; ++i) {
    אם(mechtable[i].bit == mech) {
      mname = mechtable[i].name;
      اكسر;
    }
  }
  אם(!mname)  /* should not happen */
    החזר;
  אם(!built_in)
    infof(data, "SASL: %s not builtin", mname);
  אחרת אם(!platform)
    infof(data, "SASL: %s not supported by the platform/libraries", mname);
  אחרת {
    אם(param_missing)
      infof(data, "SASL: %s is missing %s", mname, param_missing);
    אם(!Curl_creds_has_user(data->conn->creds))
      infof(data, "SASL: %s is missing username", mname);
  }
}
#endif /* CURLVERBOSE */

CURLcode Curl_sasl_is_blocked(بنية SASL *sasl, بنية Curl_easy *data)
{
#ifdef CURLVERBOSE
#ifdef USE_KERBEROS5
#define CURL_SASL_KERBEROS5   TRUE
#אחרת
#define CURL_SASL_KERBEROS5   FALSE
#endif
#ifdef USE_GSASL
#define CURL_SASL_GASL        TRUE
#אחרת
#define CURL_SASL_GASL        FALSE
#endif
#ifdef CURL_DISABLE_DIGEST_AUTH
#define CURL_SASL_DIGEST      TRUE
#אחרת
#define CURL_SASL_DIGEST      FALSE
#endif
#ifndef USE_NTLM
#define CURL_SASL_NTLM        TRUE
#אחרת
#define CURL_SASL_NTLM        FALSE
#endif
  /* Failing SASL authentication is a pain. Give a helping hand if
   * we were unable to select an AUTH mechanism.
   * `sasl->authmechs` are mechanisms offered by the peer
   * `sasl->prefmech`  are mechanisms preferred by us */
  unsigned short enabledmechs = sasl->authmechs & sasl->prefmech;

  אם(!sasl->authmechs)
    infof(data, "SASL: no auth mechanism was offered or recognized");
  אחרת אם(!enabledmechs)
    infof(data, "SASL: no overlap between offered and configured "
          "auth mechanisms");
  אחרת {
    infof(data, "SASL: no auth mechanism offered could be selected");
    אם((enabledmechs & SASL_MECH_EXTERNAL) &&
       Curl_creds_has_passwd(data->conn->creds))
      infof(data, "SASL: auth EXTERNAL not chosen with password");
    sasl_unchosen(data, SASL_MECH_GSSAPI, enabledmechs,
                  CURL_SASL_KERBEROS5, Curl_auth_is_gssapi_supported(), عدم);
    sasl_unchosen(data, SASL_MECH_SCRAM_SHA_256, enabledmechs,
                  CURL_SASL_GASL, FALSE, عدم);
    sasl_unchosen(data, SASL_MECH_SCRAM_SHA_1, enabledmechs,
                  CURL_SASL_GASL, FALSE, عدم);
    sasl_unchosen(data, SASL_MECH_DIGEST_MD5, enabledmechs,
                  CURL_SASL_DIGEST, Curl_auth_is_digest_supported(), عدم);
    sasl_unchosen(data, SASL_MECH_CRAM_MD5, enabledmechs,
                  CURL_SASL_DIGEST, TRUE, عدم);
    sasl_unchosen(data, SASL_MECH_NTLM, enabledmechs,
                  CURL_SASL_NTLM, Curl_auth_is_ntlm_supported(), عدم);
    sasl_unchosen(data, SASL_MECH_OAUTHBEARER, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(data->conn->creds) ?
                  عدم : "CURLOPT_XOAUTH2_BEARER");
    sasl_unchosen(data, SASL_MECH_XOAUTH2, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(data->conn->creds) ?
                  عدم : "CURLOPT_XOAUTH2_BEARER");
  }
#endif /* CURLVERBOSE */
  (فراغ)sasl;
  (فراغ)data;
  החזר CURLE_LOGIN_DENIED;
}

#endif /* protocols are enabled that use SASL */
