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
  بنانشانی چھوٹا ثنائی_ہندسہ_2;   /* Flag bit */
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
بنانشانی چھوٹا Curl_sasl_decode_mech(ثابت حرف *ptr, size_t maxlen,
                                     size_t *len)
{
  بنانشانی صحيح i;
  حرف c;

  لكل(i = 0; mechtable[i].name; i++) {
    إذا(maxlen >= mechtable[i].len &&
       curl_strnequal(ptr, mechtable[i].name, mechtable[i].len)) {
      إذا(len)
        *len = mechtable[i].len;

      إذا(maxlen == mechtable[i].len)
        أرجع mechtable[i].ثنائی_ہندسہ_2;

      c = ptr[mechtable[i].len];
      إذا(!ISUPPER(c) && !ISDIGIT(c) && c != '-' && c != '_')
        أرجع mechtable[i].ثنائی_ہندسہ_2;
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
                                         ثابت حرف *قدر, size_t len)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t mechlen;

  إذا(!len)
    أرجع CURLE_URL_MALFORMAT;

  إذا(sasl->resetprefs) {
    sasl->resetprefs = FALSE;
    sasl->prefmech = SASL_AUTH_NONE;
  }

  إذا(!لڑی_مقابلہ(قدر, "*", len))
    sasl->prefmech = SASL_AUTH_DEFAULT;
  وإلا {
    بنانشانی چھوٹا mechbit = Curl_sasl_decode_mech(قدر, len, &mechlen);
    إذا(mechbit && mechlen == len)
      sasl->prefmech |= mechbit;
    وإلا
      نتیجہ = CURLE_URL_MALFORMAT;
  }

  أرجع نتیجہ;
}

/*
 * Curl_sasl_init()
 *
 * Initializes the SASL structure.
 */
فراغ Curl_sasl_init(بنية SASL *sasl, بنية Curl_easy *انکڑے,
                    ثابت بنية SASLproto *params)
{
  uint32_t auth = انکڑے->سیٹ.httpauth;

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
    بنانشانی چھوٹا mechs = SASL_AUTH_NONE;

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
ساكن فراغ sasl_state(بنية SASL *sasl, بنية Curl_easy *انکڑے,
                       saslstate newstate)
{
#إذا defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ساكن ثابت حرف * ثابت ناں[] = {
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
    /*آخری*/
  };

  إذا(sasl->state != newstate)
    infof(انکڑے, "SASL %p state change from %s to %s",
          (فراغ *)sasl, ناں[sasl->state], ناں[newstate]);
#وإلا
  (فراغ)انکڑے;
#جے_انت

  sasl->state = newstate;
}

#إذا defined(USE_NTLM) || defined(USE_GSASL) || defined(USE_KERBEROS5) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)
/* Get the SASL server message and convert it to binary. */
ساكن CURLcode get_server_سنیہا(بنية SASL *sasl, بنية Curl_easy *انکڑے,
                                   بنية bufref *باہرلا)
{
  CURLcode نتیجہ = CURLE_OK;

  نتیجہ = sasl->params->getmessage(انکڑے, باہرلا);
  إذا(!نتیجہ && (sasl->params->flags & SASL_FLAG_BASE64)) {
    ثابت حرف *serverdata = Curl_bufref_ptr(باہرلا);

    إذا(!*serverdata)
      Curl_bufref_سیٹ(باہرلا, عدم, 0, عدم);
    وإلا {
      بنانشانی حرف *پیغام;
      size_t msglen;

      نتیجہ = curlx_base64_decode(serverdata, &پیغام, &msglen);
      إذا(!نتیجہ)
        Curl_bufref_سیٹ(باہرلا, پیغام, msglen, curl_free);
    }
  }
  أرجع نتیجہ;
}
#جے_انت

/* Encode the outgoing SASL message. */
ساكن CURLcode build_سنیہا(بنية SASL *sasl, بنية bufref *پیغام)
{
  CURLcode نتیجہ = CURLE_OK;

  إذا(sasl->params->flags & SASL_FLAG_BASE64) {
    إذا(!Curl_bufref_ptr(پیغام))                   /* Empty message. */
      Curl_bufref_سیٹ(پیغام, "", 0, عدم);
    وإلا إذا(Curl_bufref_len(پیغام)) {
      حرف *base64;
      size_t base64len;

      نتیجہ = curlx_base64_encode(Curl_bufref_uptr(پیغام),
                                   Curl_bufref_len(پیغام), &base64, &base64len);
      إذا(!نتیجہ)
        Curl_bufref_سیٹ(پیغام, base64, base64len, curl_free);
    }
  }

  أرجع نتیجہ;
}

/*
 * Curl_sasl_can_authenticate()
 *
 * Check if we have enough auth data and capabilities to authenticate.
 */
bool Curl_sasl_can_authenticate(بنية SASL *sasl, بنية Curl_easy *انکڑے)
{
  /* Have credentials been provided? */
  إذا(انکڑے->conn->creds)
    أرجع TRUE;

  /* EXTERNAL can authenticate without a username and/or password */
  إذا(sasl->authmechs & sasl->prefmech & SASL_MECH_EXTERNAL)
    أرجع TRUE;

  أرجع FALSE;
}

بنية sasl_ctx {
  بنية SASL *sasl;
  بنية connectdata *conn;
  بنانشانی چھوٹا enabledmechs;
  ثابت حرف *mech;
  saslstate state1;
  saslstate state2;
  بنية bufref resp;
  CURLcode نتیجہ;
};

ساكن bool sasl_چنو_external(بنية Curl_easy *انکڑے, بنية sasl_ctx *sctx)
{
  إذا((sctx->enabledmechs & SASL_MECH_EXTERNAL) &&
     !Curl_creds_has_passwd(sctx->conn->creds)) {
    sctx->mech = SASL_MECH_STRING_EXTERNAL;
    sctx->state1 = SASL_EXTERNAL;
    sctx->sasl->authused = SASL_MECH_EXTERNAL;

    إذا(sctx->sasl->force_ir || انکڑے->سیٹ.sasl_ir)
      Curl_auth_create_external_سنیہا(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    أرجع TRUE;
  }
  أرجع FALSE;
}

#جے_تعریف_شدہ USE_KERBEROS5
ساكن bool sasl_چنو_krb5(بنية Curl_easy *انکڑے, بنية sasl_ctx *sctx)
{
  إذا((sctx->enabledmechs & SASL_MECH_GSSAPI) &&
     Curl_auth_is_gssapi_supported() &&
     Curl_auth_user_شامل_اے_domain(sctx->conn->creds)) {
    ثابت حرف *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;

    sctx->sasl->mutual_auth = FALSE;
    sctx->mech = SASL_MECH_STRING_GSSAPI;
    sctx->state1 = SASL_GSSAPI;
    sctx->state2 = SASL_GSSAPI_TOKEN;
    sctx->sasl->authused = SASL_MECH_GSSAPI;

    إذا(sctx->sasl->force_ir || انکڑے->سیٹ.sasl_ir) {
      بنية kerberos5data *krb5 = Curl_auth_krb5_get(sctx->conn);
      sctx->نتیجہ = !krb5 ? CURLE_باہرلا_دا_حافظہ :
        Curl_auth_create_gssapi_user_سنیہا(انکڑے, sctx->conn->creds,
                                             service,
                                             sctx->conn->origin->hostname,
                                             (bool)sctx->sasl->mutual_auth,
                                             عدم, krb5, &sctx->resp);
    }
    أرجع TRUE;
  }
  أرجع FALSE;
}
#جے_انت /* USE_KERBEROS5 */

#جے_تعریف_شدہ USE_GSASL
ساكن bool sasl_چنو_gsasl(بنية Curl_easy *انکڑے, بنية sasl_ctx *sctx)
{
  بنية gsasldata *gsasl;
  بنية bufref nullmsg;

  إذا((sctx->enabledmechs &
      (SASL_MECH_SCRAM_SHA_256 | SASL_MECH_SCRAM_SHA_1))) {
    gsasl = Curl_auth_gsasl_get(sctx->conn);
    إذا(!gsasl) {
      sctx->نتیجہ = CURLE_باہرلا_دا_حافظہ;
      أرجع TRUE; /* attempted, but failed */
    }

    إذا((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_256) &&
       Curl_auth_gsasl_is_supported(انکڑے, SASL_MECH_STRING_SCRAM_SHA_256,
                                    gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_256;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_256;
    }
    وإلا إذا((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_1) &&
            Curl_auth_gsasl_is_supported(انکڑے, SASL_MECH_STRING_SCRAM_SHA_1,
                                         gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_1;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_1;
    }
    وإلا
      أرجع FALSE;

    Curl_bufref_init(&nullmsg);
    sctx->state1 = SASL_GSASL;
    sctx->state2 = SASL_GSASL;
    sctx->نتیجہ = Curl_auth_gsasl_شروع(انکڑے, sctx->conn->creds, gsasl);
    إذا(!sctx->نتیجہ && (sctx->sasl->force_ir || انکڑے->سیٹ.sasl_ir))
      sctx->نتیجہ = Curl_auth_gsasl_token(انکڑے, &nullmsg, gsasl, &sctx->resp);
    أرجع TRUE;
  }
  أرجع FALSE;
}

#جے_انت /* USE_GSASL */

#جے_بنا_تعریف CURL_DISABLE_DIGEST_AUTH
ساكن bool sasl_چنو_digest(بنية Curl_easy *انکڑے, بنية sasl_ctx *sctx)
{
  (فراغ)انکڑے;
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
#جے_انت /* !CURL_DISABLE_DIGEST_AUTH */

#جے_تعریف_شدہ USE_NTLM
ساكن bool sasl_چنو_ntlm(بنية Curl_easy *انکڑے, بنية sasl_ctx *sctx)
{
  إذا((sctx->enabledmechs & SASL_MECH_NTLM) &&
     Curl_auth_is_ntlm_supported()) {
     ثابت حرف *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;
    ثابت حرف *hostname;

    Curl_conn_get_current_host(انکڑے, FIRSTSOCKET, &hostname, عدم);

    sctx->mech = SASL_MECH_STRING_NTLM;
    sctx->state1 = SASL_NTLM;
    sctx->state2 = SASL_NTLM_TYPE2_پیغام;
    sctx->sasl->authused = SASL_MECH_NTLM;

    إذا(sctx->sasl->force_ir || انکڑے->سیٹ.sasl_ir) {
      بنية ntlmdata *ntlm = Curl_auth_ntlm_get(sctx->conn, FALSE);
      sctx->نتیجہ = !ntlm ? CURLE_باہرلا_دا_حافظہ :
        Curl_auth_create_ntlm_type1_سنیہا(انکڑے, sctx->conn->creds,
                                            service, hostname,
                                            ntlm, &sctx->resp);
    }
    أرجع TRUE;
  }
  أرجع FALSE;
}
#جے_انت /* USE_NTLM */

ساكن bool sasl_چنو_oauth(بنية Curl_easy *انکڑے, بنية sasl_ctx *sctx)
{
  إذا(Curl_creds_has_oauth_bearer(انکڑے->state.creds) &&
     (sctx->enabledmechs & SASL_MECH_OAUTHBEARER)) {
    ثابت حرف *hostname;
    صحيح پورٹ;
    Curl_conn_get_current_host(انکڑے, FIRSTSOCKET, &hostname, &پورٹ);

    sctx->mech = SASL_MECH_STRING_OAUTHBEARER;
    sctx->state1 = SASL_OAUTH2;
    sctx->state2 = SASL_OAUTH2_RESP;
    sctx->sasl->authused = SASL_MECH_OAUTHBEARER;

    إذا(sctx->sasl->force_ir || انکڑے->سیٹ.sasl_ir)
      sctx->نتیجہ =
        Curl_auth_create_oauth_bearer_سنیہا(sctx->conn->creds,
                                              hostname, پورٹ, &sctx->resp);
    أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن bool sasl_چنو_oauth2(بنية Curl_easy *انکڑے, بنية sasl_ctx *sctx)
{
  إذا(Curl_creds_has_oauth_bearer(sctx->conn->creds) &&
     (sctx->enabledmechs & SASL_MECH_XOAUTH2)) {
    sctx->mech = SASL_MECH_STRING_XOAUTH2;
    sctx->state1 = SASL_OAUTH2;
    sctx->sasl->authused = SASL_MECH_XOAUTH2;

    إذا(sctx->sasl->force_ir || انکڑے->سیٹ.sasl_ir)
      sctx->نتیجہ = Curl_auth_create_xoauth_bearer_سنیہا(
        sctx->conn->creds, &sctx->resp);
    أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن bool sasl_چنو_plain(بنية Curl_easy *انکڑے, بنية sasl_ctx *sctx)
{
  إذا(sctx->enabledmechs & SASL_MECH_PLAIN) {
    sctx->mech = SASL_MECH_STRING_PLAIN;
    sctx->state1 = SASL_PLAIN;
    sctx->sasl->authused = SASL_MECH_PLAIN;

    إذا(sctx->sasl->force_ir || انکڑے->سیٹ.sasl_ir)
      sctx->نتیجہ =
        Curl_auth_create_plain_سنیہا(sctx->conn->creds, &sctx->resp);
    أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن bool sasl_چنو_login(بنية Curl_easy *انکڑے, بنية sasl_ctx *sctx)
{
  إذا(sctx->enabledmechs & SASL_MECH_LOGIN) {
    sctx->mech = SASL_MECH_STRING_LOGIN;
    sctx->state1 = SASL_LOGIN;
    sctx->state2 = SASL_LOGIN_PASSWD;
    sctx->sasl->authused = SASL_MECH_LOGIN;

    إذا(sctx->sasl->force_ir || انکڑے->سیٹ.sasl_ir)
      Curl_auth_create_login_سنیہا(
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
CURLcode Curl_sasl_شروع(بنية SASL *sasl, بنية Curl_easy *انکڑے,
                         bool force_ir, saslprogress *ترقی)
{
  بنية sasl_ctx sctx;

  sasl->force_ir = force_ir;    /* Latch for future use */
  sasl->authused = 0;           /* No mechanism used yet */
  *ترقی = SASL_IDLE;

  memset(&sctx, 0, حجم(sctx));
  sctx.sasl = sasl;
  sctx.conn = انکڑے->conn;
  Curl_bufref_init(&sctx.resp);
  sctx.enabledmechs = sasl->authmechs & sasl->prefmech;
  sctx.state1 = SASL_STOP;
  sctx.state2 = SASL_FINAL;

  /* Calculate the supported authentication mechanism, by decreasing order of
     security, as well as the initial response where appropriate */
  إذا(sasl_چنو_external(انکڑے, &sctx) ||
#جے_تعریف_شدہ USE_KERBEROS5
     sasl_چنو_krb5(انکڑے, &sctx) ||
#جے_انت
#جے_تعریف_شدہ USE_GSASL
     sasl_چنو_gsasl(انکڑے, &sctx) ||
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_DIGEST_AUTH
     sasl_چنو_digest(انکڑے, &sctx) ||
#جے_انت
#جے_تعریف_شدہ USE_NTLM
     sasl_چنو_ntlm(انکڑے, &sctx) ||
#جے_انت
     sasl_چنو_oauth(انکڑے, &sctx) ||
     sasl_چنو_oauth2(انکڑے, &sctx) ||
     sasl_چنو_plain(انکڑے, &sctx) ||
     sasl_چنو_login(انکڑے, &sctx)) {
    /* selected, either we have a mechanism or a failure */
    DEBUGASSERT(sctx.mech || sctx.نتیجہ);
  }

  إذا(!sctx.نتیجہ && sctx.mech) {
    sasl->curmech = sctx.mech;
    إذا(Curl_bufref_ptr(&sctx.resp))
      sctx.نتیجہ = build_سنیہا(sasl, &sctx.resp);

    إذا(sasl->params->maxirlen &&
       لڑی_لمبائی(sctx.mech) + Curl_bufref_len(&sctx.resp) >
         sasl->params->maxirlen)
      Curl_bufref_free(&sctx.resp);

    إذا(!sctx.نتیجہ)
      sctx.نتیجہ = sasl->params->sendauth(انکڑے, sctx.mech, &sctx.resp);

    إذا(!sctx.نتیجہ) {
      *ترقی = SASL_INPROGRESS;
      sasl_state(sasl, انکڑے, Curl_bufref_ptr(&sctx.resp) ?
                 sctx.state2 : sctx.state1);
    }
  }

  Curl_bufref_free(&sctx.resp);
  أرجع sctx.نتیجہ;
}

/*
 * Curl_sasl_continue()
 *
 * Continue the authentication.
 */
CURLcode Curl_sasl_continue(بنية SASL *sasl, بنية Curl_easy *انکڑے,
                            صحيح کوڈ, saslprogress *ترقی)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية connectdata *conn = انکڑے->conn;
  saslstate newstate = SASL_FINAL;
  بنية bufref resp;
  ثابت حرف *hostname;
  صحيح پورٹ;
  بنية bufref serverdata;

  Curl_conn_get_current_host(انکڑے, FIRSTSOCKET, &hostname, &پورٹ);
  Curl_bufref_init(&serverdata);
  Curl_bufref_init(&resp);
  *ترقی = SASL_INPROGRESS;

  إذا(sasl->state == SASL_FINAL) {
    إذا(کوڈ != sasl->params->finalcode)
      نتیجہ = CURLE_LOGIN_DENIED;
    *ترقی = SASL_پورا;
    sasl_state(sasl, انکڑے, SASL_STOP);
    أرجع نتیجہ;
  }

  إذا(sasl->state != SASL_CANCEL && sasl->state != SASL_OAUTH2_RESP &&
     کوڈ != sasl->params->contcode) {
    *ترقی = SASL_پورا;
    sasl_state(sasl, انکڑے, SASL_STOP);
    أرجع CURLE_LOGIN_DENIED;
  }

  بدل(sasl->state) {
  حالة SASL_STOP:
    *ترقی = SASL_پورا;
    أرجع نتیجہ;
  حالة SASL_PLAIN:
    نتیجہ = Curl_auth_create_plain_سنیہا(conn->creds, &resp);
    اكسر;
  حالة SASL_LOGIN:
    Curl_auth_create_login_سنیہا(Curl_creds_user(conn->creds), &resp);
    newstate = SASL_LOGIN_PASSWD;
    اكسر;
  حالة SASL_LOGIN_PASSWD:
    Curl_auth_create_login_سنیہا(Curl_creds_passwd(conn->creds), &resp);
    اكسر;
  حالة SASL_EXTERNAL:
    Curl_auth_create_external_سنیہا(Curl_creds_user(conn->creds), &resp);
    اكسر;
#جے_تعریف_شدہ USE_GSASL
  حالة SASL_GSASL:
    نتیجہ = get_server_سنیہا(sasl, انکڑے, &serverdata);
    إذا(!نتیجہ) {
      بنية gsasldata *gsasl = Curl_auth_gsasl_get(conn);
      نتیجہ = !gsasl ? CURLE_باہرلا_دا_حافظہ :
        Curl_auth_gsasl_token(انکڑے, &serverdata, gsasl, &resp);
    }
    إذا(!نتیجہ && Curl_bufref_len(&resp) > 0)
      newstate = SASL_GSASL;
    اكسر;
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_DIGEST_AUTH
  حالة SASL_CRAMMD5:
    نتیجہ = get_server_سنیہا(sasl, انکڑے, &serverdata);
    إذا(!نتیجہ)
      نتیجہ = Curl_auth_create_cram_md5_سنیہا(&serverdata, conn->creds,
                                                 &resp);
    اكسر;
  حالة SASL_DIGESTMD5:
    نتیجہ = get_server_سنیہا(sasl, انکڑے, &serverdata);
    إذا(!نتیجہ)
      نتیجہ = Curl_auth_create_digest_md5_سنیہا(انکڑے, &serverdata,
                                                   conn->creds,
                                                   sasl->params->service,
                                                   &resp);
    إذا(!نتیجہ && (sasl->params->flags & SASL_FLAG_BASE64))
      newstate = SASL_DIGESTMD5_RESP;
    اكسر;
  حالة SASL_DIGESTMD5_RESP:
    /* Keep response NULL to output an empty line. */
    اكسر;
#جے_انت

#جے_تعریف_شدہ USE_NTLM
  حالة SASL_NTLM: {
    /* Create the type-1 message */
    بنية ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    نتیجہ = !ntlm ? CURLE_باہرلا_دا_حافظہ :
      Curl_auth_create_ntlm_type1_سنیہا(انکڑے, conn->creds,
                                          sasl->params->service, hostname,
                                          ntlm, &resp);
    newstate = SASL_NTLM_TYPE2_پیغام;
    اكسر;
  }
  حالة SASL_NTLM_TYPE2_پیغام: {
    /* Decode the type-2 message */
    بنية ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    نتیجہ = !ntlm ? CURLE_باہرلا_دا_حافظہ :
      get_server_سنیہا(sasl, انکڑے, &serverdata);
    إذا(!نتیجہ)
      نتیجہ = Curl_auth_decode_ntlm_type2_سنیہا(انکڑے, &serverdata, ntlm);
    إذا(!نتیجہ)
      نتیجہ = Curl_auth_create_ntlm_type3_سنیہا(انکڑے, conn->creds,
                                                   ntlm, &resp);
    اكسر;
  }
#جے_انت

#جے_تعریف_شدہ USE_KERBEROS5
  حالة SASL_GSSAPI: {
    بنية kerberos5data *krb5 = Curl_auth_krb5_get(conn);
    نتیجہ = !krb5 ? CURLE_باہرلا_دا_حافظہ :
      Curl_auth_create_gssapi_user_سنیہا(انکڑے, conn->creds,
                                           sasl->params->service,
                                           conn->origin->hostname,
                                           (bool)sasl->mutual_auth, عدم,
                                           krb5, &resp);
    newstate = SASL_GSSAPI_TOKEN;
    اكسر;
  }
  حالة SASL_GSSAPI_TOKEN:
    نتیجہ = get_server_سنیہا(sasl, انکڑے, &serverdata);
    إذا(!نتیجہ) {
      بنية kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      إذا(!krb5)
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
      وإلا إذا(sasl->mutual_auth) {
        /* Decode the user token challenge and create the optional response
سنیہا*/
        نتیجہ = Curl_auth_create_gssapi_user_سنیہا(انکڑے, عدم,
                                                      عدم, عدم,
                                                      (bool)sasl->mutual_auth,
                                                      &serverdata,
                                                      krb5, &resp);
        newstate = SASL_GSSAPI_NO_انکڑے;
      }
      وإلا
        /* Decode the security challenge and create the response message */
        نتیجہ = Curl_auth_create_gssapi_security_سنیہا(
          انکڑے, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    اكسر;
  حالة SASL_GSSAPI_NO_انکڑے:
    /* Decode the security challenge and create the response message */
    نتیجہ = get_server_سنیہا(sasl, انکڑے, &serverdata);
    إذا(!نتیجہ) {
      بنية kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      إذا(!krb5)
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
      وإلا
        نتیجہ = Curl_auth_create_gssapi_security_سنیہا(
          انکڑے, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    اكسر;
#جے_انت

  حالة SASL_OAUTH2:
    /* Create the authorization message */
    إذا(sasl->authused == SASL_MECH_OAUTHBEARER) {
      نتیجہ = Curl_auth_create_oauth_bearer_سنیہا(conn->creds,
                                                     hostname,
                                                     پورٹ,
                                                     &resp);

      /* Failures maybe sent by the server as continuations for OAUTHBEARER */
      newstate = SASL_OAUTH2_RESP;
    }
    وإلا
      نتیجہ = Curl_auth_create_xoauth_bearer_سنیہا(conn->creds,
                                                      &resp);
    اكسر;

  حالة SASL_OAUTH2_RESP:
    /* The continuation is optional so check the response code */
    إذا(کوڈ == sasl->params->finalcode) {
      /* Final response was received so we are done */
      *ترقی = SASL_پورا;
      sasl_state(sasl, انکڑے, SASL_STOP);
      أرجع نتیجہ;
    }
    وإلا إذا(کوڈ == sasl->params->contcode) {
      /* Acknowledge the continuation by sending a 0x01 response. */
      Curl_bufref_سیٹ(&resp, "\x01", 1, عدم);
      اكسر;
    }
    وإلا {
      *ترقی = SASL_پورا;
      sasl_state(sasl, انکڑے, SASL_STOP);
      أرجع CURLE_LOGIN_DENIED;
    }

  حالة SASL_CANCEL:
    /* Remove the offending mechanism from the supported list */
    sasl->authmechs &= (بنانشانی چھوٹا)~sasl->authused;
    sasl->authused = SASL_AUTH_NONE;
    sasl->curmech = عدم;

    /* Start an alternative SASL authentication */
    أرجع Curl_sasl_شروع(sasl, انکڑے, (bool)sasl->force_ir, ترقی);
  افتراضي:
    failf(انکڑے, "Unsupported SASL authentication mechanism");
    نتیجہ = CURLE_UNSUPPORTED_PROTOCOL;  /* Should not happen */
    اكسر;
  }

  Curl_bufref_free(&serverdata);

  بدل(نتیجہ) {
  حالة CURLE_BAD_CONTENT_ENCODING:
    /* Cancel dialog */
    نتیجہ = sasl->params->cancelauth(انکڑے, sasl->curmech);
    newstate = SASL_CANCEL;
    اكسر;
  حالة CURLE_OK:
    نتیجہ = build_سنیہا(sasl, &resp);
    إذا(!نتیجہ)
      نتیجہ = sasl->params->contauth(انکڑے, sasl->curmech, &resp);
    اكسر;
  افتراضي:
    newstate = SASL_STOP;    /* Stop on error */
    *ترقی = SASL_پورا;
    اكسر;
  }

  Curl_bufref_free(&resp);

  sasl_state(sasl, انکڑے, newstate);

  أرجع نتیجہ;
}

#جے_تعریف_شدہ CURLVERBOSE
ساكن فراغ sasl_unchosen(بنية Curl_easy *انکڑے, بنانشانی چھوٹا mech,
                          بنانشانی چھوٹا enabledmechs,
                          bool built_وچ, bool platform,
                          ثابت حرف *param_missing)
{
  ثابت حرف *mname = عدم;
  size_t i;

  إذا(!(enabledmechs & mech))
    أرجع;

  لكل(i = 0; mechtable[i].name; ++i) {
    إذا(mechtable[i].ثنائی_ہندسہ_2 == mech) {
      mname = mechtable[i].name;
      اكسر;
    }
  }
  إذا(!mname)  /* should not happen */
    أرجع;
  إذا(!built_وچ)
    infof(انکڑے, "SASL: %s not builtin", mname);
  وإلا إذا(!platform)
    infof(انکڑے, "SASL: %s not supported by the platform/libraries", mname);
  وإلا {
    إذا(param_missing)
      infof(انکڑے, "SASL: %s is missing %s", mname, param_missing);
    إذا(!Curl_creds_has_user(انکڑے->conn->creds))
      infof(انکڑے, "SASL: %s is missing username", mname);
  }
}
#جے_انت /* CURLVERBOSE */

CURLcode Curl_sasl_is_blocked(بنية SASL *sasl, بنية Curl_easy *انکڑے)
{
#جے_تعریف_شدہ CURLVERBOSE
#جے_تعریف_شدہ USE_KERBEROS5
#تعریف_کرو CURL_SASL_KERBEROS5   TRUE
#وإلا
#تعریف_کرو CURL_SASL_KERBEROS5   FALSE
#جے_انت
#جے_تعریف_شدہ USE_GSASL
#تعریف_کرو CURL_SASL_GASL        TRUE
#وإلا
#تعریف_کرو CURL_SASL_GASL        FALSE
#جے_انت
#جے_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
#تعریف_کرو CURL_SASL_DIGEST      TRUE
#وإلا
#تعریف_کرو CURL_SASL_DIGEST      FALSE
#جے_انت
#جے_بنا_تعریف USE_NTLM
#تعریف_کرو CURL_SASL_NTLM        TRUE
#وإلا
#تعریف_کرو CURL_SASL_NTLM        FALSE
#جے_انت
  /* Failing SASL authentication is a pain. Give a helping hand if
   * we were unable to select an AUTH mechanism.
   * `sasl->authmechs` are mechanisms offered by the peer
   * `sasl->prefmech`  are mechanisms preferred by us */
  بنانشانی چھوٹا enabledmechs = sasl->authmechs & sasl->prefmech;

  إذا(!sasl->authmechs)
    infof(انکڑے, "SASL: no auth mechanism was offered or recognized");
  وإلا إذا(!enabledmechs)
    infof(انکڑے, "SASL: no overlap between offered and configured "
          "auth mechanisms");
  وإلا {
    infof(انکڑے, "SASL: no auth mechanism offered could be selected");
    إذا((enabledmechs & SASL_MECH_EXTERNAL) &&
       Curl_creds_has_passwd(انکڑے->conn->creds))
      infof(انکڑے, "SASL: auth EXTERNAL not chosen with password");
    sasl_unchosen(انکڑے, SASL_MECH_GSSAPI, enabledmechs,
                  CURL_SASL_KERBEROS5, Curl_auth_is_gssapi_supported(), عدم);
    sasl_unchosen(انکڑے, SASL_MECH_SCRAM_SHA_256, enabledmechs,
                  CURL_SASL_GASL, FALSE, عدم);
    sasl_unchosen(انکڑے, SASL_MECH_SCRAM_SHA_1, enabledmechs,
                  CURL_SASL_GASL, FALSE, عدم);
    sasl_unchosen(انکڑے, SASL_MECH_DIGEST_MD5, enabledmechs,
                  CURL_SASL_DIGEST, Curl_auth_is_digest_supported(), عدم);
    sasl_unchosen(انکڑے, SASL_MECH_CRAM_MD5, enabledmechs,
                  CURL_SASL_DIGEST, TRUE, عدم);
    sasl_unchosen(انکڑے, SASL_MECH_NTLM, enabledmechs,
                  CURL_SASL_NTLM, Curl_auth_is_ntlm_supported(), عدم);
    sasl_unchosen(انکڑے, SASL_MECH_OAUTHBEARER, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(انکڑے->conn->creds) ?
                  عدم : "CURLOPT_XOAUTH2_BEARER");
    sasl_unchosen(انکڑے, SASL_MECH_XOAUTH2, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(انکڑے->conn->creds) ?
                  عدم : "CURLOPT_XOAUTH2_BEARER");
  }
#جے_انت /* CURLVERBOSE */
  (فراغ)sasl;
  (فراغ)انکڑے;
  أرجع CURLE_LOGIN_DENIED;
}

#جے_انت /* protocols are enabled that use SASL */
