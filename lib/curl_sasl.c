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

#إذا !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
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
  بیعلامت کوتاه بیت_2;   /* Flag bit */
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
بیعلامت کوتاه Curl_sasl_decode_mech(ثابت حرف *ptr, size_t maxlen,
                                     size_t *len)
{
  بیعلامت صحيح i;
  حرف c;

  لكل(i = 0; mechtable[i].name; i++) {
    إذا(maxlen >= mechtable[i].len &&
       curl_strnequal(ptr, mechtable[i].name, mechtable[i].len)) {
      إذا(len)
        *len = mechtable[i].len;

      إذا(maxlen == mechtable[i].len)
        أرجع mechtable[i].بیت_2;

      c = ptr[mechtable[i].len];
      إذا(!ISUPPER(c) && !ISDIGIT(c) && c != '-' && c != '_')
        أرجع mechtable[i].بیت_2;
    }
  }

  أرجع 0;
}

/*
 * Curl_sasl_parse_url_auth_option()
 *
 * Parse the URL login options.
 */
CURLcode Curl_sasl_parse_url_auth_option(بنية SASL *sasl,
                                         ثابت حرف *مقدار, size_t len)
{
  CURLcode نتیجه = CURLE_OK;
  size_t mechlen;

  إذا(!len)
    أرجع CURLE_URL_MALFORMAT;

  إذا(sasl->resetprefs) {
    sasl->resetprefs = FALSE;
    sasl->prefmech = SASL_AUTH_NONE;
  }

  إذا(!مقایسه_رشته(مقدار, "*", len))
    sasl->prefmech = SASL_AUTH_DEFAULT;
  وإلا {
    بیعلامت کوتاه mechbit = Curl_sasl_decode_mech(مقدار, len, &mechlen);
    إذا(mechbit && mechlen == len)
      sasl->prefmech |= mechbit;
    وإلا
      نتیجه = CURLE_URL_MALFORMAT;
  }

  أرجع نتیجه;
}

/*
 * Curl_sasl_init()
 *
 * Initializes the SASL structure.
 */
فراغ Curl_sasl_init(بنية SASL *sasl, بنية Curl_easy *داده,
                    ثابت بنية SASLproto *params)
{
  uint32_t auth = داده->مجموعه.httpauth;

  sasl->params = params;           /* Set protocol dependent parameters */
  sasl->state = SASL_STOP;         /* Not yet running */
  sasl->curmech = عدم;            /* No mechanism yet. */
  sasl->authmechs = SASL_AUTH_NONE; /* No known authentication mechanism yet */
  sasl->prefmech = params->defmechs; /* Default preferred mechanisms */
  sasl->authused = SASL_AUTH_NONE; /* The authentication mechanism used */
  sasl->resetprefs = TRUE;         /* Reset prefmech upon AUTH parsing. */
  sasl->mutual_auth = FALSE;       /* No mutual authentication (GSSAPI only) */
  sasl->force_ir = FALSE;          /* Respect external option */

  إذا(auth != CURLAUTH_BASIC) {
    بیعلامت کوتاه mechs = SASL_AUTH_NONE;

    /* If some usable http authentication options have been set, determine
       new defaults from them. */
    إذا(auth & CURLAUTH_BASIC)
      mechs |= SASL_MECH_PLAIN | SASL_MECH_LOGIN;
    إذا(auth & CURLAUTH_DIGEST)
      mechs |= SASL_MECH_DIGEST_MD5;
    إذا(auth & CURLAUTH_NTLM)
      mechs |= SASL_MECH_NTLM;
    إذا(auth & CURLAUTH_BEARER)
      mechs |= SASL_MECH_OAUTHBEARER | SASL_MECH_XOAUTH2;
    إذا(auth & CURLAUTH_GSSAPI)
      mechs |= SASL_MECH_GSSAPI;

    إذا(mechs != SASL_AUTH_NONE)
      sasl->prefmech = mechs;
  }
}

/*
 * sasl_state()
 *
 * This is the ONLY way to change SASL state!
 */
ساكن فراغ sasl_state(بنية SASL *sasl, بنية Curl_easy *داده,
                       saslstate newstate)
{
#إذا defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ساكن ثابت حرف * ثابت نامها[] = {
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
    /*آخرین*/
  };

  إذا(sasl->state != newstate)
    infof(داده, "SASL %p state change from %s to %s",
          (فراغ *)sasl, نامها[sasl->state], نامها[newstate]);
#وإلا
  (فراغ)داده;
#پایان_اگر

  sasl->state = newstate;
}

#إذا defined(USE_NTLM) || defined(USE_GSASL) || defined(USE_KERBEROS5) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)
/* Get the SASL server message and convert it to binary. */
ساكن CURLcode get_server_پیام(بنية SASL *sasl, بنية Curl_easy *داده,
                                   بنية bufref *بیرون)
{
  CURLcode نتیجه = CURLE_OK;

  نتیجه = sasl->params->getmessage(داده, بیرون);
  إذا(!نتیجه && (sasl->params->flags & SASL_FLAG_BASE64)) {
    ثابت حرف *serverdata = Curl_bufref_ptr(بیرون);

    إذا(!*serverdata)
      Curl_bufref_مجموعه(بیرون, عدم, 0, عدم);
    وإلا {
      بیعلامت حرف *پیام;
      size_t msglen;

      نتیجه = curlx_base64_decode(serverdata, &پیام, &msglen);
      إذا(!نتیجه)
        Curl_bufref_مجموعه(بیرون, پیام, msglen, curl_free);
    }
  }
  أرجع نتیجه;
}
#پایان_اگر

/* Encode the outgoing SASL message. */
ساكن CURLcode build_پیام(بنية SASL *sasl, بنية bufref *پیام)
{
  CURLcode نتیجه = CURLE_OK;

  إذا(sasl->params->flags & SASL_FLAG_BASE64) {
    إذا(!Curl_bufref_ptr(پیام))                   /* Empty message. */
      Curl_bufref_مجموعه(پیام, "", 0, عدم);
    وإلا إذا(Curl_bufref_len(پیام)) {
      حرف *base64;
      size_t base64len;

      نتیجه = curlx_base64_encode(Curl_bufref_uptr(پیام),
                                   Curl_bufref_len(پیام), &base64, &base64len);
      إذا(!نتیجه)
        Curl_bufref_مجموعه(پیام, base64, base64len, curl_free);
    }
  }

  أرجع نتیجه;
}

/*
 * Curl_sasl_can_authenticate()
 *
 * Check if we have enough auth data and capabilities to authenticate.
 */
bool Curl_sasl_can_authenticate(بنية SASL *sasl, بنية Curl_easy *داده)
{
  /* Have credentials been provided? */
  إذا(داده->conn->creds)
    أرجع TRUE;

  /* EXTERNAL can authenticate without a username and/or password */
  إذا(sasl->authmechs & sasl->prefmech & SASL_MECH_EXTERNAL)
    أرجع TRUE;

  أرجع FALSE;
}

بنية sasl_ctx {
  بنية SASL *sasl;
  بنية connectdata *conn;
  بیعلامت کوتاه enabledmechs;
  ثابت حرف *mech;
  saslstate state1;
  saslstate state2;
  بنية bufref resp;
  CURLcode نتیجه;
};

ساكن bool sasl_انتخاب_external(بنية Curl_easy *داده, بنية sasl_ctx *sctx)
{
  إذا((sctx->enabledmechs & SASL_MECH_EXTERNAL) &&
     !Curl_creds_has_passwd(sctx->conn->creds)) {
    sctx->mech = SASL_MECH_STRING_EXTERNAL;
    sctx->state1 = SASL_EXTERNAL;
    sctx->sasl->authused = SASL_MECH_EXTERNAL;

    إذا(sctx->sasl->force_ir || داده->مجموعه.sasl_ir)
      Curl_auth_create_external_پیام(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    أرجع TRUE;
  }
  أرجع FALSE;
}

#اگر_تعریف_شده USE_KERBEROS5
ساكن bool sasl_انتخاب_krb5(بنية Curl_easy *داده, بنية sasl_ctx *sctx)
{
  إذا((sctx->enabledmechs & SASL_MECH_GSSAPI) &&
     Curl_auth_is_gssapi_supported() &&
     Curl_auth_user_شامل_است_domain(sctx->conn->creds)) {
    ثابت حرف *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;

    sctx->sasl->mutual_auth = FALSE;
    sctx->mech = SASL_MECH_STRING_GSSAPI;
    sctx->state1 = SASL_GSSAPI;
    sctx->state2 = SASL_GSSAPI_TOKEN;
    sctx->sasl->authused = SASL_MECH_GSSAPI;

    إذا(sctx->sasl->force_ir || داده->مجموعه.sasl_ir) {
      بنية kerberos5data *krb5 = Curl_auth_krb5_get(sctx->conn);
      sctx->نتیجه = !krb5 ? CURLE_بیرون_از_حافظه :
        Curl_auth_create_gssapi_user_پیام(داده, sctx->conn->creds,
                                             service,
                                             sctx->conn->origin->hostname,
                                             (bool)sctx->sasl->mutual_auth,
                                             عدم, krb5, &sctx->resp);
    }
    أرجع TRUE;
  }
  أرجع FALSE;
}
#پایان_اگر /* USE_KERBEROS5 */

#اگر_تعریف_شده USE_GSASL
ساكن bool sasl_انتخاب_gsasl(بنية Curl_easy *داده, بنية sasl_ctx *sctx)
{
  بنية gsasldata *gsasl;
  بنية bufref nullmsg;

  إذا((sctx->enabledmechs &
      (SASL_MECH_SCRAM_SHA_256 | SASL_MECH_SCRAM_SHA_1))) {
    gsasl = Curl_auth_gsasl_get(sctx->conn);
    إذا(!gsasl) {
      sctx->نتیجه = CURLE_بیرون_از_حافظه;
      أرجع TRUE; /* attempted, but failed */
    }

    إذا((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_256) &&
       Curl_auth_gsasl_is_supported(داده, SASL_MECH_STRING_SCRAM_SHA_256,
                                    gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_256;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_256;
    }
    وإلا إذا((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_1) &&
            Curl_auth_gsasl_is_supported(داده, SASL_MECH_STRING_SCRAM_SHA_1,
                                         gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_1;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_1;
    }
    وإلا
      أرجع FALSE;

    Curl_bufref_init(&nullmsg);
    sctx->state1 = SASL_GSASL;
    sctx->state2 = SASL_GSASL;
    sctx->نتیجه = Curl_auth_gsasl_شروع(داده, sctx->conn->creds, gsasl);
    إذا(!sctx->نتیجه && (sctx->sasl->force_ir || داده->مجموعه.sasl_ir))
      sctx->نتیجه = Curl_auth_gsasl_token(داده, &nullmsg, gsasl, &sctx->resp);
    أرجع TRUE;
  }
  أرجع FALSE;
}

#پایان_اگر /* USE_GSASL */

#اگر_تعریف_نشده CURL_DISABLE_DIGEST_AUTH
ساكن bool sasl_انتخاب_digest(بنية Curl_easy *داده, بنية sasl_ctx *sctx)
{
  (فراغ)داده;
  إذا((sctx->enabledmechs & SASL_MECH_DIGEST_MD5) &&
     Curl_auth_is_digest_supported()) {
    sctx->mech = SASL_MECH_STRING_DIGEST_MD5;
    sctx->state1 = SASL_DIGESTMD5;
    sctx->sasl->authused = SASL_MECH_DIGEST_MD5;
    أرجع TRUE;
  }
  وإلا إذا(sctx->enabledmechs & SASL_MECH_CRAM_MD5) {
    sctx->mech = SASL_MECH_STRING_CRAM_MD5;
    sctx->state1 = SASL_CRAMMD5;
    sctx->sasl->authused = SASL_MECH_CRAM_MD5;
    أرجع TRUE;
  }
  أرجع FALSE;
}
#پایان_اگر /* !CURL_DISABLE_DIGEST_AUTH */

#اگر_تعریف_شده USE_NTLM
ساكن bool sasl_انتخاب_ntlm(بنية Curl_easy *داده, بنية sasl_ctx *sctx)
{
  إذا((sctx->enabledmechs & SASL_MECH_NTLM) &&
     Curl_auth_is_ntlm_supported()) {
     ثابت حرف *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;
    ثابت حرف *hostname;

    Curl_conn_get_current_host(داده, FIRSTSOCKET, &hostname, عدم);

    sctx->mech = SASL_MECH_STRING_NTLM;
    sctx->state1 = SASL_NTLM;
    sctx->state2 = SASL_NTLM_TYPE2_پیام;
    sctx->sasl->authused = SASL_MECH_NTLM;

    إذا(sctx->sasl->force_ir || داده->مجموعه.sasl_ir) {
      بنية ntlmdata *ntlm = Curl_auth_ntlm_get(sctx->conn, FALSE);
      sctx->نتیجه = !ntlm ? CURLE_بیرون_از_حافظه :
        Curl_auth_create_ntlm_type1_پیام(داده, sctx->conn->creds,
                                            service, hostname,
                                            ntlm, &sctx->resp);
    }
    أرجع TRUE;
  }
  أرجع FALSE;
}
#پایان_اگر /* USE_NTLM */

ساكن bool sasl_انتخاب_oauth(بنية Curl_easy *داده, بنية sasl_ctx *sctx)
{
  إذا(Curl_creds_has_oauth_bearer(داده->state.creds) &&
     (sctx->enabledmechs & SASL_MECH_OAUTHBEARER)) {
    ثابت حرف *hostname;
    صحيح پورت;
    Curl_conn_get_current_host(داده, FIRSTSOCKET, &hostname, &پورت);

    sctx->mech = SASL_MECH_STRING_OAUTHBEARER;
    sctx->state1 = SASL_OAUTH2;
    sctx->state2 = SASL_OAUTH2_RESP;
    sctx->sasl->authused = SASL_MECH_OAUTHBEARER;

    إذا(sctx->sasl->force_ir || داده->مجموعه.sasl_ir)
      sctx->نتیجه =
        Curl_auth_create_oauth_bearer_پیام(sctx->conn->creds,
                                              hostname, پورت, &sctx->resp);
    أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن bool sasl_انتخاب_oauth2(بنية Curl_easy *داده, بنية sasl_ctx *sctx)
{
  إذا(Curl_creds_has_oauth_bearer(sctx->conn->creds) &&
     (sctx->enabledmechs & SASL_MECH_XOAUTH2)) {
    sctx->mech = SASL_MECH_STRING_XOAUTH2;
    sctx->state1 = SASL_OAUTH2;
    sctx->sasl->authused = SASL_MECH_XOAUTH2;

    إذا(sctx->sasl->force_ir || داده->مجموعه.sasl_ir)
      sctx->نتیجه = Curl_auth_create_xoauth_bearer_پیام(
        sctx->conn->creds, &sctx->resp);
    أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن bool sasl_انتخاب_plain(بنية Curl_easy *داده, بنية sasl_ctx *sctx)
{
  إذا(sctx->enabledmechs & SASL_MECH_PLAIN) {
    sctx->mech = SASL_MECH_STRING_PLAIN;
    sctx->state1 = SASL_PLAIN;
    sctx->sasl->authused = SASL_MECH_PLAIN;

    إذا(sctx->sasl->force_ir || داده->مجموعه.sasl_ir)
      sctx->نتیجه =
        Curl_auth_create_plain_پیام(sctx->conn->creds, &sctx->resp);
    أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن bool sasl_انتخاب_login(بنية Curl_easy *داده, بنية sasl_ctx *sctx)
{
  إذا(sctx->enabledmechs & SASL_MECH_LOGIN) {
    sctx->mech = SASL_MECH_STRING_LOGIN;
    sctx->state1 = SASL_LOGIN;
    sctx->state2 = SASL_LOGIN_PASSWD;
    sctx->sasl->authused = SASL_MECH_LOGIN;

    إذا(sctx->sasl->force_ir || داده->مجموعه.sasl_ir)
      Curl_auth_create_login_پیام(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    أرجع TRUE;
  }
  أرجع FALSE;
}

/*
 * Curl_sasl_start()
 *
 * Calculate the required login details for SASL authentication.
 */
CURLcode Curl_sasl_شروع(بنية SASL *sasl, بنية Curl_easy *داده,
                         bool force_ir, saslprogress *پیشرفت)
{
  بنية sasl_ctx sctx;

  sasl->force_ir = force_ir;    /* Latch for future use */
  sasl->authused = 0;           /* No mechanism used yet */
  *پیشرفت = SASL_IDLE;

  memset(&sctx, 0, حجم(sctx));
  sctx.sasl = sasl;
  sctx.conn = داده->conn;
  Curl_bufref_init(&sctx.resp);
  sctx.enabledmechs = sasl->authmechs & sasl->prefmech;
  sctx.state1 = SASL_STOP;
  sctx.state2 = SASL_FINAL;

  /* Calculate the supported authentication mechanism, by decreasing order of
     security, as well as the initial response where appropriate */
  إذا(sasl_انتخاب_external(داده, &sctx) ||
#اگر_تعریف_شده USE_KERBEROS5
     sasl_انتخاب_krb5(داده, &sctx) ||
#پایان_اگر
#اگر_تعریف_شده USE_GSASL
     sasl_انتخاب_gsasl(داده, &sctx) ||
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_DIGEST_AUTH
     sasl_انتخاب_digest(داده, &sctx) ||
#پایان_اگر
#اگر_تعریف_شده USE_NTLM
     sasl_انتخاب_ntlm(داده, &sctx) ||
#پایان_اگر
     sasl_انتخاب_oauth(داده, &sctx) ||
     sasl_انتخاب_oauth2(داده, &sctx) ||
     sasl_انتخاب_plain(داده, &sctx) ||
     sasl_انتخاب_login(داده, &sctx)) {
    /* selected, either we have a mechanism or a failure */
    DEBUGASSERT(sctx.mech || sctx.نتیجه);
  }

  إذا(!sctx.نتیجه && sctx.mech) {
    sasl->curmech = sctx.mech;
    إذا(Curl_bufref_ptr(&sctx.resp))
      sctx.نتیجه = build_پیام(sasl, &sctx.resp);

    إذا(sasl->params->maxirlen &&
       طول_رشته(sctx.mech) + Curl_bufref_len(&sctx.resp) >
         sasl->params->maxirlen)
      Curl_bufref_free(&sctx.resp);

    إذا(!sctx.نتیجه)
      sctx.نتیجه = sasl->params->sendauth(داده, sctx.mech, &sctx.resp);

    إذا(!sctx.نتیجه) {
      *پیشرفت = SASL_INPROGRESS;
      sasl_state(sasl, داده, Curl_bufref_ptr(&sctx.resp) ?
                 sctx.state2 : sctx.state1);
    }
  }

  Curl_bufref_free(&sctx.resp);
  أرجع sctx.نتیجه;
}

/*
 * Curl_sasl_continue()
 *
 * Continue the authentication.
 */
CURLcode Curl_sasl_continue(بنية SASL *sasl, بنية Curl_easy *داده,
                            صحيح کد, saslprogress *پیشرفت)
{
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;
  saslstate newstate = SASL_FINAL;
  بنية bufref resp;
  ثابت حرف *hostname;
  صحيح پورت;
  بنية bufref serverdata;

  Curl_conn_get_current_host(داده, FIRSTSOCKET, &hostname, &پورت);
  Curl_bufref_init(&serverdata);
  Curl_bufref_init(&resp);
  *پیشرفت = SASL_INPROGRESS;

  إذا(sasl->state == SASL_FINAL) {
    إذا(کد != sasl->params->finalcode)
      نتیجه = CURLE_LOGIN_DENIED;
    *پیشرفت = SASL_انجامشده;
    sasl_state(sasl, داده, SASL_STOP);
    أرجع نتیجه;
  }

  إذا(sasl->state != SASL_CANCEL && sasl->state != SASL_OAUTH2_RESP &&
     کد != sasl->params->contcode) {
    *پیشرفت = SASL_انجامشده;
    sasl_state(sasl, داده, SASL_STOP);
    أرجع CURLE_LOGIN_DENIED;
  }

  بدل(sasl->state) {
  حالة SASL_STOP:
    *پیشرفت = SASL_انجامشده;
    أرجع نتیجه;
  حالة SASL_PLAIN:
    نتیجه = Curl_auth_create_plain_پیام(conn->creds, &resp);
    اكسر;
  حالة SASL_LOGIN:
    Curl_auth_create_login_پیام(Curl_creds_user(conn->creds), &resp);
    newstate = SASL_LOGIN_PASSWD;
    اكسر;
  حالة SASL_LOGIN_PASSWD:
    Curl_auth_create_login_پیام(Curl_creds_passwd(conn->creds), &resp);
    اكسر;
  حالة SASL_EXTERNAL:
    Curl_auth_create_external_پیام(Curl_creds_user(conn->creds), &resp);
    اكسر;
#اگر_تعریف_شده USE_GSASL
  حالة SASL_GSASL:
    نتیجه = get_server_پیام(sasl, داده, &serverdata);
    إذا(!نتیجه) {
      بنية gsasldata *gsasl = Curl_auth_gsasl_get(conn);
      نتیجه = !gsasl ? CURLE_بیرون_از_حافظه :
        Curl_auth_gsasl_token(داده, &serverdata, gsasl, &resp);
    }
    إذا(!نتیجه && Curl_bufref_len(&resp) > 0)
      newstate = SASL_GSASL;
    اكسر;
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_DIGEST_AUTH
  حالة SASL_CRAMMD5:
    نتیجه = get_server_پیام(sasl, داده, &serverdata);
    إذا(!نتیجه)
      نتیجه = Curl_auth_create_cram_md5_پیام(&serverdata, conn->creds,
                                                 &resp);
    اكسر;
  حالة SASL_DIGESTMD5:
    نتیجه = get_server_پیام(sasl, داده, &serverdata);
    إذا(!نتیجه)
      نتیجه = Curl_auth_create_digest_md5_پیام(داده, &serverdata,
                                                   conn->creds,
                                                   sasl->params->service,
                                                   &resp);
    إذا(!نتیجه && (sasl->params->flags & SASL_FLAG_BASE64))
      newstate = SASL_DIGESTMD5_RESP;
    اكسر;
  حالة SASL_DIGESTMD5_RESP:
    /* Keep response NULL to output an empty line. */
    اكسر;
#پایان_اگر

#اگر_تعریف_شده USE_NTLM
  حالة SASL_NTLM: {
    /* Create the type-1 message */
    بنية ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    نتیجه = !ntlm ? CURLE_بیرون_از_حافظه :
      Curl_auth_create_ntlm_type1_پیام(داده, conn->creds,
                                          sasl->params->service, hostname,
                                          ntlm, &resp);
    newstate = SASL_NTLM_TYPE2_پیام;
    اكسر;
  }
  حالة SASL_NTLM_TYPE2_پیام: {
    /* Decode the type-2 message */
    بنية ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    نتیجه = !ntlm ? CURLE_بیرون_از_حافظه :
      get_server_پیام(sasl, داده, &serverdata);
    إذا(!نتیجه)
      نتیجه = Curl_auth_decode_ntlm_type2_پیام(داده, &serverdata, ntlm);
    إذا(!نتیجه)
      نتیجه = Curl_auth_create_ntlm_type3_پیام(داده, conn->creds,
                                                   ntlm, &resp);
    اكسر;
  }
#پایان_اگر

#اگر_تعریف_شده USE_KERBEROS5
  حالة SASL_GSSAPI: {
    بنية kerberos5data *krb5 = Curl_auth_krb5_get(conn);
    نتیجه = !krb5 ? CURLE_بیرون_از_حافظه :
      Curl_auth_create_gssapi_user_پیام(داده, conn->creds,
                                           sasl->params->service,
                                           conn->origin->hostname,
                                           (bool)sasl->mutual_auth, عدم,
                                           krb5, &resp);
    newstate = SASL_GSSAPI_TOKEN;
    اكسر;
  }
  حالة SASL_GSSAPI_TOKEN:
    نتیجه = get_server_پیام(sasl, داده, &serverdata);
    إذا(!نتیجه) {
      بنية kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      إذا(!krb5)
        نتیجه = CURLE_بیرون_از_حافظه;
      وإلا إذا(sasl->mutual_auth) {
        /* Decode the user token challenge and create the optional response
پیام*/
        نتیجه = Curl_auth_create_gssapi_user_پیام(داده, عدم,
                                                      عدم, عدم,
                                                      (bool)sasl->mutual_auth,
                                                      &serverdata,
                                                      krb5, &resp);
        newstate = SASL_GSSAPI_NO_داده;
      }
      وإلا
        /* Decode the security challenge and create the response message */
        نتیجه = Curl_auth_create_gssapi_security_پیام(
          داده, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    اكسر;
  حالة SASL_GSSAPI_NO_داده:
    /* Decode the security challenge and create the response message */
    نتیجه = get_server_پیام(sasl, داده, &serverdata);
    إذا(!نتیجه) {
      بنية kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      إذا(!krb5)
        نتیجه = CURLE_بیرون_از_حافظه;
      وإلا
        نتیجه = Curl_auth_create_gssapi_security_پیام(
          داده, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    اكسر;
#پایان_اگر

  حالة SASL_OAUTH2:
    /* Create the authorization message */
    إذا(sasl->authused == SASL_MECH_OAUTHBEARER) {
      نتیجه = Curl_auth_create_oauth_bearer_پیام(conn->creds,
                                                     hostname,
                                                     پورت,
                                                     &resp);

      /* Failures maybe sent by the server as continuations for OAUTHBEARER */
      newstate = SASL_OAUTH2_RESP;
    }
    وإلا
      نتیجه = Curl_auth_create_xoauth_bearer_پیام(conn->creds,
                                                      &resp);
    اكسر;

  حالة SASL_OAUTH2_RESP:
    /* The continuation is optional so check the response code */
    إذا(کد == sasl->params->finalcode) {
      /* Final response was received so we are done */
      *پیشرفت = SASL_انجامشده;
      sasl_state(sasl, داده, SASL_STOP);
      أرجع نتیجه;
    }
    وإلا إذا(کد == sasl->params->contcode) {
      /* Acknowledge the continuation by sending a 0x01 response. */
      Curl_bufref_مجموعه(&resp, "\x01", 1, عدم);
      اكسر;
    }
    وإلا {
      *پیشرفت = SASL_انجامشده;
      sasl_state(sasl, داده, SASL_STOP);
      أرجع CURLE_LOGIN_DENIED;
    }

  حالة SASL_CANCEL:
    /* Remove the offending mechanism from the supported list */
    sasl->authmechs &= (بیعلامت کوتاه)~sasl->authused;
    sasl->authused = SASL_AUTH_NONE;
    sasl->curmech = عدم;

    /* Start an alternative SASL authentication */
    أرجع Curl_sasl_شروع(sasl, داده, (bool)sasl->force_ir, پیشرفت);
  افتراضي:
    failf(داده, "Unsupported SASL authentication mechanism");
    نتیجه = CURLE_UNSUPPORTED_PROTOCOL;  /* Should not happen */
    اكسر;
  }

  Curl_bufref_free(&serverdata);

  بدل(نتیجه) {
  حالة CURLE_BAD_CONTENT_ENCODING:
    /* Cancel dialog */
    نتیجه = sasl->params->cancelauth(داده, sasl->curmech);
    newstate = SASL_CANCEL;
    اكسر;
  حالة CURLE_OK:
    نتیجه = build_پیام(sasl, &resp);
    إذا(!نتیجه)
      نتیجه = sasl->params->contauth(داده, sasl->curmech, &resp);
    اكسر;
  افتراضي:
    newstate = SASL_STOP;    /* Stop on error */
    *پیشرفت = SASL_انجامشده;
    اكسر;
  }

  Curl_bufref_free(&resp);

  sasl_state(sasl, داده, newstate);

  أرجع نتیجه;
}

#اگر_تعریف_شده CURLVERBOSE
ساكن فراغ sasl_unchosen(بنية Curl_easy *داده, بیعلامت کوتاه mech,
                          بیعلامت کوتاه enabledmechs,
                          bool built_درون, bool platform,
                          ثابت حرف *param_missing)
{
  ثابت حرف *mname = عدم;
  size_t i;

  إذا(!(enabledmechs & mech))
    أرجع;

  لكل(i = 0; mechtable[i].name; ++i) {
    إذا(mechtable[i].بیت_2 == mech) {
      mname = mechtable[i].name;
      اكسر;
    }
  }
  إذا(!mname)  /* should not happen */
    أرجع;
  إذا(!built_درون)
    infof(داده, "SASL: %s not builtin", mname);
  وإلا إذا(!platform)
    infof(داده, "SASL: %s not supported by the platform/libraries", mname);
  وإلا {
    إذا(param_missing)
      infof(داده, "SASL: %s is missing %s", mname, param_missing);
    إذا(!Curl_creds_has_user(داده->conn->creds))
      infof(داده, "SASL: %s is missing username", mname);
  }
}
#پایان_اگر /* CURLVERBOSE */

CURLcode Curl_sasl_is_blocked(بنية SASL *sasl, بنية Curl_easy *داده)
{
#اگر_تعریف_شده CURLVERBOSE
#اگر_تعریف_شده USE_KERBEROS5
#تعریف_کن CURL_SASL_KERBEROS5   TRUE
#وإلا
#تعریف_کن CURL_SASL_KERBEROS5   FALSE
#پایان_اگر
#اگر_تعریف_شده USE_GSASL
#تعریف_کن CURL_SASL_GASL        TRUE
#وإلا
#تعریف_کن CURL_SASL_GASL        FALSE
#پایان_اگر
#اگر_تعریف_شده CURL_DISABLE_DIGEST_AUTH
#تعریف_کن CURL_SASL_DIGEST      TRUE
#وإلا
#تعریف_کن CURL_SASL_DIGEST      FALSE
#پایان_اگر
#اگر_تعریف_نشده USE_NTLM
#تعریف_کن CURL_SASL_NTLM        TRUE
#وإلا
#تعریف_کن CURL_SASL_NTLM        FALSE
#پایان_اگر
  /* Failing SASL authentication is a pain. Give a helping hand if
   * we were unable to select an AUTH mechanism.
   * `sasl->authmechs` are mechanisms offered by the peer
   * `sasl->prefmech`  are mechanisms preferred by us */
  بیعلامت کوتاه enabledmechs = sasl->authmechs & sasl->prefmech;

  إذا(!sasl->authmechs)
    infof(داده, "SASL: no auth mechanism was offered or recognized");
  وإلا إذا(!enabledmechs)
    infof(داده, "SASL: no overlap between offered and configured "
          "auth mechanisms");
  وإلا {
    infof(داده, "SASL: no auth mechanism offered could be selected");
    إذا((enabledmechs & SASL_MECH_EXTERNAL) &&
       Curl_creds_has_passwd(داده->conn->creds))
      infof(داده, "SASL: auth EXTERNAL not chosen with password");
    sasl_unchosen(داده, SASL_MECH_GSSAPI, enabledmechs,
                  CURL_SASL_KERBEROS5, Curl_auth_is_gssapi_supported(), عدم);
    sasl_unchosen(داده, SASL_MECH_SCRAM_SHA_256, enabledmechs,
                  CURL_SASL_GASL, FALSE, عدم);
    sasl_unchosen(داده, SASL_MECH_SCRAM_SHA_1, enabledmechs,
                  CURL_SASL_GASL, FALSE, عدم);
    sasl_unchosen(داده, SASL_MECH_DIGEST_MD5, enabledmechs,
                  CURL_SASL_DIGEST, Curl_auth_is_digest_supported(), عدم);
    sasl_unchosen(داده, SASL_MECH_CRAM_MD5, enabledmechs,
                  CURL_SASL_DIGEST, TRUE, عدم);
    sasl_unchosen(داده, SASL_MECH_NTLM, enabledmechs,
                  CURL_SASL_NTLM, Curl_auth_is_ntlm_supported(), عدم);
    sasl_unchosen(داده, SASL_MECH_OAUTHBEARER, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(داده->conn->creds) ?
                  عدم : "CURLOPT_XOAUTH2_BEARER");
    sasl_unchosen(داده, SASL_MECH_XOAUTH2, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(داده->conn->creds) ?
                  عدم : "CURLOPT_XOAUTH2_BEARER");
  }
#پایان_اگر /* CURLVERBOSE */
  (فراغ)sasl;
  (فراغ)داده;
  أرجع CURLE_LOGIN_DENIED;
}

#پایان_اگر /* protocols are enabled that use SASL */
