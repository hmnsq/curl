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
ساکن ثابت ساخت {
  ثابت حرف    *name;  /* Name */
  size_t         len;   /* Name length */
  غیرعلامتی مختصر ثنائی_ہندسہ_2;   /* Flag bit */
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
غیرعلامتی مختصر Curl_sasl_decode_mech(ثابت حرف *ptr, size_t maxlen,
                                     size_t *len)
{
  غیرعلامتی صحیح i;
  حرف c;

  برائے(i = 0; mechtable[i].name; i++) {
    اگر(maxlen >= mechtable[i].len &&
       curl_strnequal(ptr, mechtable[i].name, mechtable[i].len)) {
      اگر(len)
        *len = mechtable[i].len;

      اگر(maxlen == mechtable[i].len)
        واپس mechtable[i].ثنائی_ہندسہ_2;

      c = ptr[mechtable[i].len];
      اگر(!ISUPPER(c) && !ISDIGIT(c) && c != '-' && c != '_')
        واپس mechtable[i].ثنائی_ہندسہ_2;
    }
  }

  واپس 0;
}

/*
 * Curl_sasl_parse_url_auth_option()
 *
 * Parse the URL login options.
 */
CURLcode Curl_sasl_parse_url_auth_option(ساخت SASL *sasl,
                                         ثابت حرف *قدر, size_t len)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t mechlen;

  اگر(!len)
    واپس CURLE_URL_MALFORMAT;

  اگر(sasl->resetprefs) {
    sasl->resetprefs = FALSE;
    sasl->prefmech = SASL_AUTH_NONE;
  }

  اگر(!لڑی_موازنہ(قدر, "*", len))
    sasl->prefmech = SASL_AUTH_DEFAULT;
  ورنہ {
    غیرعلامتی مختصر mechbit = Curl_sasl_decode_mech(قدر, len, &mechlen);
    اگر(mechbit && mechlen == len)
      sasl->prefmech |= mechbit;
    ورنہ
      نتیجہ = CURLE_URL_MALFORMAT;
  }

  واپس نتیجہ;
}

/*
 * Curl_sasl_init()
 *
 * Initializes the SASL structure.
 */
خالی Curl_sasl_init(ساخت SASL *sasl, ساخت Curl_easy *ڈیٹا,
                    ثابت ساخت SASLproto *params)
{
  uint32_t auth = ڈیٹا->سیٹ.httpauth;

  sasl->params = params;           /* Set protocol dependent parameters */
  sasl->state = SASL_STOP;         /* Not yet running */
  sasl->curmech = تہی;            /* No mechanism yet. */
  sasl->authmechs = SASL_AUTH_NONE; /* No known authentication mechanism yet */
  sasl->prefmech = params->defmechs; /* Default preferred mechanisms */
  sasl->authused = SASL_AUTH_NONE; /* The authentication mechanism used */
  sasl->resetprefs = TRUE;         /* Reset prefmech upon AUTH parsing. */
  sasl->mutual_auth = FALSE;       /* No mutual authentication (GSSAPI only) */
  sasl->force_ir = FALSE;          /* Respect external option */

  اگر(auth != CURLAUTH_BASIC) {
    غیرعلامتی مختصر mechs = SASL_AUTH_NONE;

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
ساکن خالی sasl_state(ساخت SASL *sasl, ساخت Curl_easy *ڈیٹا,
                       saslstate newstate)
{
#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  ساکن ثابت حرف * ثابت نام[] = {
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

  اگر(sasl->state != newstate)
    infof(ڈیٹا, "SASL %p state change from %s to %s",
          (خالی *)sasl, نام[sasl->state], نام[newstate]);
#ورنہ
  (خالی)ڈیٹا;
#اگر_ختم

  sasl->state = newstate;
}

#اگر defined(USE_NTLM) || defined(USE_GSASL) || defined(USE_KERBEROS5) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)
/* Get the SASL server message and convert it to binary. */
ساکن CURLcode get_server_پیغام(ساخت SASL *sasl, ساخت Curl_easy *ڈیٹا,
                                   ساخت bufref *برآمد)
{
  CURLcode نتیجہ = CURLE_OK;

  نتیجہ = sasl->params->getmessage(ڈیٹا, برآمد);
  اگر(!نتیجہ && (sasl->params->flags & SASL_FLAG_BASE64)) {
    ثابت حرف *serverdata = Curl_bufref_ptr(برآمد);

    اگر(!*serverdata)
      Curl_bufref_سیٹ(برآمد, تہی, 0, تہی);
    ورنہ {
      غیرعلامتی حرف *پیغام;
      size_t msglen;

      نتیجہ = curlx_base64_decode(serverdata, &پیغام, &msglen);
      اگر(!نتیجہ)
        Curl_bufref_سیٹ(برآمد, پیغام, msglen, curl_free);
    }
  }
  واپس نتیجہ;
}
#اگر_ختم

/* Encode the outgoing SASL message. */
ساکن CURLcode build_پیغام(ساخت SASL *sasl, ساخت bufref *پیغام)
{
  CURLcode نتیجہ = CURLE_OK;

  اگر(sasl->params->flags & SASL_FLAG_BASE64) {
    اگر(!Curl_bufref_ptr(پیغام))                   /* Empty message. */
      Curl_bufref_سیٹ(پیغام, "", 0, تہی);
    ورنہ اگر(Curl_bufref_len(پیغام)) {
      حرف *base64;
      size_t base64len;

      نتیجہ = curlx_base64_encode(Curl_bufref_uptr(پیغام),
                                   Curl_bufref_len(پیغام), &base64, &base64len);
      اگر(!نتیجہ)
        Curl_bufref_سیٹ(پیغام, base64, base64len, curl_free);
    }
  }

  واپس نتیجہ;
}

/*
 * Curl_sasl_can_authenticate()
 *
 * Check if we have enough auth data and capabilities to authenticate.
 */
bool Curl_sasl_can_authenticate(ساخت SASL *sasl, ساخت Curl_easy *ڈیٹا)
{
  /* Have credentials been provided? */
  اگر(ڈیٹا->conn->creds)
    واپس TRUE;

  /* EXTERNAL can authenticate without a username and/or password */
  اگر(sasl->authmechs & sasl->prefmech & SASL_MECH_EXTERNAL)
    واپس TRUE;

  واپس FALSE;
}

ساخت sasl_ctx {
  ساخت SASL *sasl;
  ساخت connectdata *conn;
  غیرعلامتی مختصر enabledmechs;
  ثابت حرف *mech;
  saslstate state1;
  saslstate state2;
  ساخت bufref resp;
  CURLcode نتیجہ;
};

ساکن bool sasl_انتخاب_external(ساخت Curl_easy *ڈیٹا, ساخت sasl_ctx *sctx)
{
  اگر((sctx->enabledmechs & SASL_MECH_EXTERNAL) &&
     !Curl_creds_has_passwd(sctx->conn->creds)) {
    sctx->mech = SASL_MECH_STRING_EXTERNAL;
    sctx->state1 = SASL_EXTERNAL;
    sctx->sasl->authused = SASL_MECH_EXTERNAL;

    اگر(sctx->sasl->force_ir || ڈیٹا->سیٹ.sasl_ir)
      Curl_auth_create_external_پیغام(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    واپس TRUE;
  }
  واپس FALSE;
}

#اگر_تعریف_شدہ USE_KERBEROS5
ساکن bool sasl_انتخاب_krb5(ساخت Curl_easy *ڈیٹا, ساخت sasl_ctx *sctx)
{
  اگر((sctx->enabledmechs & SASL_MECH_GSSAPI) &&
     Curl_auth_is_gssapi_supported() &&
     Curl_auth_user_شامل_ہے_domain(sctx->conn->creds)) {
    ثابت حرف *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;

    sctx->sasl->mutual_auth = FALSE;
    sctx->mech = SASL_MECH_STRING_GSSAPI;
    sctx->state1 = SASL_GSSAPI;
    sctx->state2 = SASL_GSSAPI_TOKEN;
    sctx->sasl->authused = SASL_MECH_GSSAPI;

    اگر(sctx->sasl->force_ir || ڈیٹا->سیٹ.sasl_ir) {
      ساخت kerberos5data *krb5 = Curl_auth_krb5_get(sctx->conn);
      sctx->نتیجہ = !krb5 ? CURLE_برآمد_کا_حافظہ :
        Curl_auth_create_gssapi_user_پیغام(ڈیٹا, sctx->conn->creds,
                                             service,
                                             sctx->conn->origin->hostname,
                                             (bool)sctx->sasl->mutual_auth,
                                             تہی, krb5, &sctx->resp);
    }
    واپس TRUE;
  }
  واپس FALSE;
}
#اگر_ختم /* USE_KERBEROS5 */

#اگر_تعریف_شدہ USE_GSASL
ساکن bool sasl_انتخاب_gsasl(ساخت Curl_easy *ڈیٹا, ساخت sasl_ctx *sctx)
{
  ساخت gsasldata *gsasl;
  ساخت bufref nullmsg;

  اگر((sctx->enabledmechs &
      (SASL_MECH_SCRAM_SHA_256 | SASL_MECH_SCRAM_SHA_1))) {
    gsasl = Curl_auth_gsasl_get(sctx->conn);
    اگر(!gsasl) {
      sctx->نتیجہ = CURLE_برآمد_کا_حافظہ;
      واپس TRUE; /* attempted, but failed */
    }

    اگر((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_256) &&
       Curl_auth_gsasl_is_supported(ڈیٹا, SASL_MECH_STRING_SCRAM_SHA_256,
                                    gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_256;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_256;
    }
    ورنہ اگر((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_1) &&
            Curl_auth_gsasl_is_supported(ڈیٹا, SASL_MECH_STRING_SCRAM_SHA_1,
                                         gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_1;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_1;
    }
    ورنہ
      واپس FALSE;

    Curl_bufref_init(&nullmsg);
    sctx->state1 = SASL_GSASL;
    sctx->state2 = SASL_GSASL;
    sctx->نتیجہ = Curl_auth_gsasl_آغاز(ڈیٹا, sctx->conn->creds, gsasl);
    اگر(!sctx->نتیجہ && (sctx->sasl->force_ir || ڈیٹا->سیٹ.sasl_ir))
      sctx->نتیجہ = Curl_auth_gsasl_token(ڈیٹا, &nullmsg, gsasl, &sctx->resp);
    واپس TRUE;
  }
  واپس FALSE;
}

#اگر_ختم /* USE_GSASL */

#اگر_غیر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
ساکن bool sasl_انتخاب_digest(ساخت Curl_easy *ڈیٹا, ساخت sasl_ctx *sctx)
{
  (خالی)ڈیٹا;
  اگر((sctx->enabledmechs & SASL_MECH_DIGEST_MD5) &&
     Curl_auth_is_digest_supported()) {
    sctx->mech = SASL_MECH_STRING_DIGEST_MD5;
    sctx->state1 = SASL_DIGESTMD5;
    sctx->sasl->authused = SASL_MECH_DIGEST_MD5;
    واپس TRUE;
  }
  ورنہ اگر(sctx->enabledmechs & SASL_MECH_CRAM_MD5) {
    sctx->mech = SASL_MECH_STRING_CRAM_MD5;
    sctx->state1 = SASL_CRAMMD5;
    sctx->sasl->authused = SASL_MECH_CRAM_MD5;
    واپس TRUE;
  }
  واپس FALSE;
}
#اگر_ختم /* !CURL_DISABLE_DIGEST_AUTH */

#اگر_تعریف_شدہ USE_NTLM
ساکن bool sasl_انتخاب_ntlm(ساخت Curl_easy *ڈیٹا, ساخت sasl_ctx *sctx)
{
  اگر((sctx->enabledmechs & SASL_MECH_NTLM) &&
     Curl_auth_is_ntlm_supported()) {
     ثابت حرف *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;
    ثابت حرف *hostname;

    Curl_conn_get_current_host(ڈیٹا, FIRSTSOCKET, &hostname, تہی);

    sctx->mech = SASL_MECH_STRING_NTLM;
    sctx->state1 = SASL_NTLM;
    sctx->state2 = SASL_NTLM_TYPE2_پیغام;
    sctx->sasl->authused = SASL_MECH_NTLM;

    اگر(sctx->sasl->force_ir || ڈیٹا->سیٹ.sasl_ir) {
      ساخت ntlmdata *ntlm = Curl_auth_ntlm_get(sctx->conn, FALSE);
      sctx->نتیجہ = !ntlm ? CURLE_برآمد_کا_حافظہ :
        Curl_auth_create_ntlm_type1_پیغام(ڈیٹا, sctx->conn->creds,
                                            service, hostname,
                                            ntlm, &sctx->resp);
    }
    واپس TRUE;
  }
  واپس FALSE;
}
#اگر_ختم /* USE_NTLM */

ساکن bool sasl_انتخاب_oauth(ساخت Curl_easy *ڈیٹا, ساخت sasl_ctx *sctx)
{
  اگر(Curl_creds_has_oauth_bearer(ڈیٹا->state.creds) &&
     (sctx->enabledmechs & SASL_MECH_OAUTHBEARER)) {
    ثابت حرف *hostname;
    صحیح پورٹ;
    Curl_conn_get_current_host(ڈیٹا, FIRSTSOCKET, &hostname, &پورٹ);

    sctx->mech = SASL_MECH_STRING_OAUTHBEARER;
    sctx->state1 = SASL_OAUTH2;
    sctx->state2 = SASL_OAUTH2_RESP;
    sctx->sasl->authused = SASL_MECH_OAUTHBEARER;

    اگر(sctx->sasl->force_ir || ڈیٹا->سیٹ.sasl_ir)
      sctx->نتیجہ =
        Curl_auth_create_oauth_bearer_پیغام(sctx->conn->creds,
                                              hostname, پورٹ, &sctx->resp);
    واپس TRUE;
  }
  واپس FALSE;
}

ساکن bool sasl_انتخاب_oauth2(ساخت Curl_easy *ڈیٹا, ساخت sasl_ctx *sctx)
{
  اگر(Curl_creds_has_oauth_bearer(sctx->conn->creds) &&
     (sctx->enabledmechs & SASL_MECH_XOAUTH2)) {
    sctx->mech = SASL_MECH_STRING_XOAUTH2;
    sctx->state1 = SASL_OAUTH2;
    sctx->sasl->authused = SASL_MECH_XOAUTH2;

    اگر(sctx->sasl->force_ir || ڈیٹا->سیٹ.sasl_ir)
      sctx->نتیجہ = Curl_auth_create_xoauth_bearer_پیغام(
        sctx->conn->creds, &sctx->resp);
    واپس TRUE;
  }
  واپس FALSE;
}

ساکن bool sasl_انتخاب_plain(ساخت Curl_easy *ڈیٹا, ساخت sasl_ctx *sctx)
{
  اگر(sctx->enabledmechs & SASL_MECH_PLAIN) {
    sctx->mech = SASL_MECH_STRING_PLAIN;
    sctx->state1 = SASL_PLAIN;
    sctx->sasl->authused = SASL_MECH_PLAIN;

    اگر(sctx->sasl->force_ir || ڈیٹا->سیٹ.sasl_ir)
      sctx->نتیجہ =
        Curl_auth_create_plain_پیغام(sctx->conn->creds, &sctx->resp);
    واپس TRUE;
  }
  واپس FALSE;
}

ساکن bool sasl_انتخاب_login(ساخت Curl_easy *ڈیٹا, ساخت sasl_ctx *sctx)
{
  اگر(sctx->enabledmechs & SASL_MECH_LOGIN) {
    sctx->mech = SASL_MECH_STRING_LOGIN;
    sctx->state1 = SASL_LOGIN;
    sctx->state2 = SASL_LOGIN_PASSWD;
    sctx->sasl->authused = SASL_MECH_LOGIN;

    اگر(sctx->sasl->force_ir || ڈیٹا->سیٹ.sasl_ir)
      Curl_auth_create_login_پیغام(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    واپس TRUE;
  }
  واپس FALSE;
}

/*
 * Curl_sasl_start()
 *
 * Calculate the required login details for SASL authentication.
 */
CURLcode Curl_sasl_آغاز(ساخت SASL *sasl, ساخت Curl_easy *ڈیٹا,
                         bool force_ir, saslprogress *پیش_رفت)
{
  ساخت sasl_ctx sctx;

  sasl->force_ir = force_ir;    /* Latch for future use */
  sasl->authused = 0;           /* No mechanism used yet */
  *پیش_رفت = SASL_IDLE;

  memset(&sctx, 0, حجم(sctx));
  sctx.sasl = sasl;
  sctx.conn = ڈیٹا->conn;
  Curl_bufref_init(&sctx.resp);
  sctx.enabledmechs = sasl->authmechs & sasl->prefmech;
  sctx.state1 = SASL_STOP;
  sctx.state2 = SASL_FINAL;

  /* Calculate the supported authentication mechanism, by decreasing order of
     security, as well as the initial response where appropriate */
  اگر(sasl_انتخاب_external(ڈیٹا, &sctx) ||
#اگر_تعریف_شدہ USE_KERBEROS5
     sasl_انتخاب_krb5(ڈیٹا, &sctx) ||
#اگر_ختم
#اگر_تعریف_شدہ USE_GSASL
     sasl_انتخاب_gsasl(ڈیٹا, &sctx) ||
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
     sasl_انتخاب_digest(ڈیٹا, &sctx) ||
#اگر_ختم
#اگر_تعریف_شدہ USE_NTLM
     sasl_انتخاب_ntlm(ڈیٹا, &sctx) ||
#اگر_ختم
     sasl_انتخاب_oauth(ڈیٹا, &sctx) ||
     sasl_انتخاب_oauth2(ڈیٹا, &sctx) ||
     sasl_انتخاب_plain(ڈیٹا, &sctx) ||
     sasl_انتخاب_login(ڈیٹا, &sctx)) {
    /* selected, either we have a mechanism or a failure */
    DEBUGASSERT(sctx.mech || sctx.نتیجہ);
  }

  اگر(!sctx.نتیجہ && sctx.mech) {
    sasl->curmech = sctx.mech;
    اگر(Curl_bufref_ptr(&sctx.resp))
      sctx.نتیجہ = build_پیغام(sasl, &sctx.resp);

    اگر(sasl->params->maxirlen &&
       لڑی_لمبائی(sctx.mech) + Curl_bufref_len(&sctx.resp) >
         sasl->params->maxirlen)
      Curl_bufref_free(&sctx.resp);

    اگر(!sctx.نتیجہ)
      sctx.نتیجہ = sasl->params->sendauth(ڈیٹا, sctx.mech, &sctx.resp);

    اگر(!sctx.نتیجہ) {
      *پیش_رفت = SASL_INPROGRESS;
      sasl_state(sasl, ڈیٹا, Curl_bufref_ptr(&sctx.resp) ?
                 sctx.state2 : sctx.state1);
    }
  }

  Curl_bufref_free(&sctx.resp);
  واپس sctx.نتیجہ;
}

/*
 * Curl_sasl_continue()
 *
 * Continue the authentication.
 */
CURLcode Curl_sasl_continue(ساخت SASL *sasl, ساخت Curl_easy *ڈیٹا,
                            صحیح رمز, saslprogress *پیش_رفت)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;
  saslstate newstate = SASL_FINAL;
  ساخت bufref resp;
  ثابت حرف *hostname;
  صحیح پورٹ;
  ساخت bufref serverdata;

  Curl_conn_get_current_host(ڈیٹا, FIRSTSOCKET, &hostname, &پورٹ);
  Curl_bufref_init(&serverdata);
  Curl_bufref_init(&resp);
  *پیش_رفت = SASL_INPROGRESS;

  اگر(sasl->state == SASL_FINAL) {
    اگر(رمز != sasl->params->finalcode)
      نتیجہ = CURLE_LOGIN_DENIED;
    *پیش_رفت = SASL_مکمل;
    sasl_state(sasl, ڈیٹا, SASL_STOP);
    واپس نتیجہ;
  }

  اگر(sasl->state != SASL_CANCEL && sasl->state != SASL_OAUTH2_RESP &&
     رمز != sasl->params->contcode) {
    *پیش_رفت = SASL_مکمل;
    sasl_state(sasl, ڈیٹا, SASL_STOP);
    واپس CURLE_LOGIN_DENIED;
  }

  انتخاب(sasl->state) {
  حالت SASL_STOP:
    *پیش_رفت = SASL_مکمل;
    واپس نتیجہ;
  حالت SASL_PLAIN:
    نتیجہ = Curl_auth_create_plain_پیغام(conn->creds, &resp);
    توڑو;
  حالت SASL_LOGIN:
    Curl_auth_create_login_پیغام(Curl_creds_user(conn->creds), &resp);
    newstate = SASL_LOGIN_PASSWD;
    توڑو;
  حالت SASL_LOGIN_PASSWD:
    Curl_auth_create_login_پیغام(Curl_creds_passwd(conn->creds), &resp);
    توڑو;
  حالت SASL_EXTERNAL:
    Curl_auth_create_external_پیغام(Curl_creds_user(conn->creds), &resp);
    توڑو;
#اگر_تعریف_شدہ USE_GSASL
  حالت SASL_GSASL:
    نتیجہ = get_server_پیغام(sasl, ڈیٹا, &serverdata);
    اگر(!نتیجہ) {
      ساخت gsasldata *gsasl = Curl_auth_gsasl_get(conn);
      نتیجہ = !gsasl ? CURLE_برآمد_کا_حافظہ :
        Curl_auth_gsasl_token(ڈیٹا, &serverdata, gsasl, &resp);
    }
    اگر(!نتیجہ && Curl_bufref_len(&resp) > 0)
      newstate = SASL_GSASL;
    توڑو;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
  حالت SASL_CRAMMD5:
    نتیجہ = get_server_پیغام(sasl, ڈیٹا, &serverdata);
    اگر(!نتیجہ)
      نتیجہ = Curl_auth_create_cram_md5_پیغام(&serverdata, conn->creds,
                                                 &resp);
    توڑو;
  حالت SASL_DIGESTMD5:
    نتیجہ = get_server_پیغام(sasl, ڈیٹا, &serverdata);
    اگر(!نتیجہ)
      نتیجہ = Curl_auth_create_digest_md5_پیغام(ڈیٹا, &serverdata,
                                                   conn->creds,
                                                   sasl->params->service,
                                                   &resp);
    اگر(!نتیجہ && (sasl->params->flags & SASL_FLAG_BASE64))
      newstate = SASL_DIGESTMD5_RESP;
    توڑو;
  حالت SASL_DIGESTMD5_RESP:
    /* Keep response NULL to output an empty line. */
    توڑو;
#اگر_ختم

#اگر_تعریف_شدہ USE_NTLM
  حالت SASL_NTLM: {
    /* Create the type-1 message */
    ساخت ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    نتیجہ = !ntlm ? CURLE_برآمد_کا_حافظہ :
      Curl_auth_create_ntlm_type1_پیغام(ڈیٹا, conn->creds,
                                          sasl->params->service, hostname,
                                          ntlm, &resp);
    newstate = SASL_NTLM_TYPE2_پیغام;
    توڑو;
  }
  حالت SASL_NTLM_TYPE2_پیغام: {
    /* Decode the type-2 message */
    ساخت ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    نتیجہ = !ntlm ? CURLE_برآمد_کا_حافظہ :
      get_server_پیغام(sasl, ڈیٹا, &serverdata);
    اگر(!نتیجہ)
      نتیجہ = Curl_auth_decode_ntlm_type2_پیغام(ڈیٹا, &serverdata, ntlm);
    اگر(!نتیجہ)
      نتیجہ = Curl_auth_create_ntlm_type3_پیغام(ڈیٹا, conn->creds,
                                                   ntlm, &resp);
    توڑو;
  }
#اگر_ختم

#اگر_تعریف_شدہ USE_KERBEROS5
  حالت SASL_GSSAPI: {
    ساخت kerberos5data *krb5 = Curl_auth_krb5_get(conn);
    نتیجہ = !krb5 ? CURLE_برآمد_کا_حافظہ :
      Curl_auth_create_gssapi_user_پیغام(ڈیٹا, conn->creds,
                                           sasl->params->service,
                                           conn->origin->hostname,
                                           (bool)sasl->mutual_auth, تہی,
                                           krb5, &resp);
    newstate = SASL_GSSAPI_TOKEN;
    توڑو;
  }
  حالت SASL_GSSAPI_TOKEN:
    نتیجہ = get_server_پیغام(sasl, ڈیٹا, &serverdata);
    اگر(!نتیجہ) {
      ساخت kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      اگر(!krb5)
        نتیجہ = CURLE_برآمد_کا_حافظہ;
      ورنہ اگر(sasl->mutual_auth) {
        /* Decode the user token challenge and create the optional response
پیغام*/
        نتیجہ = Curl_auth_create_gssapi_user_پیغام(ڈیٹا, تہی,
                                                      تہی, تہی,
                                                      (bool)sasl->mutual_auth,
                                                      &serverdata,
                                                      krb5, &resp);
        newstate = SASL_GSSAPI_NO_ڈیٹا;
      }
      ورنہ
        /* Decode the security challenge and create the response message */
        نتیجہ = Curl_auth_create_gssapi_security_پیغام(
          ڈیٹا, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    توڑو;
  حالت SASL_GSSAPI_NO_ڈیٹا:
    /* Decode the security challenge and create the response message */
    نتیجہ = get_server_پیغام(sasl, ڈیٹا, &serverdata);
    اگر(!نتیجہ) {
      ساخت kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      اگر(!krb5)
        نتیجہ = CURLE_برآمد_کا_حافظہ;
      ورنہ
        نتیجہ = Curl_auth_create_gssapi_security_پیغام(
          ڈیٹا, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    توڑو;
#اگر_ختم

  حالت SASL_OAUTH2:
    /* Create the authorization message */
    اگر(sasl->authused == SASL_MECH_OAUTHBEARER) {
      نتیجہ = Curl_auth_create_oauth_bearer_پیغام(conn->creds,
                                                     hostname,
                                                     پورٹ,
                                                     &resp);

      /* Failures maybe sent by the server as continuations for OAUTHBEARER */
      newstate = SASL_OAUTH2_RESP;
    }
    ورنہ
      نتیجہ = Curl_auth_create_xoauth_bearer_پیغام(conn->creds,
                                                      &resp);
    توڑو;

  حالت SASL_OAUTH2_RESP:
    /* The continuation is optional so check the response code */
    اگر(رمز == sasl->params->finalcode) {
      /* Final response was received so we are done */
      *پیش_رفت = SASL_مکمل;
      sasl_state(sasl, ڈیٹا, SASL_STOP);
      واپس نتیجہ;
    }
    ورنہ اگر(رمز == sasl->params->contcode) {
      /* Acknowledge the continuation by sending a 0x01 response. */
      Curl_bufref_سیٹ(&resp, "\x01", 1, تہی);
      توڑو;
    }
    ورنہ {
      *پیش_رفت = SASL_مکمل;
      sasl_state(sasl, ڈیٹا, SASL_STOP);
      واپس CURLE_LOGIN_DENIED;
    }

  حالت SASL_CANCEL:
    /* Remove the offending mechanism from the supported list */
    sasl->authmechs &= (غیرعلامتی مختصر)~sasl->authused;
    sasl->authused = SASL_AUTH_NONE;
    sasl->curmech = تہی;

    /* Start an alternative SASL authentication */
    واپس Curl_sasl_آغاز(sasl, ڈیٹا, (bool)sasl->force_ir, پیش_رفت);
  طےشدہ:
    failf(ڈیٹا, "Unsupported SASL authentication mechanism");
    نتیجہ = CURLE_UNSUPPORTED_PROTOCOL;  /* Should not happen */
    توڑو;
  }

  Curl_bufref_free(&serverdata);

  انتخاب(نتیجہ) {
  حالت CURLE_BAD_CONTENT_ENCODING:
    /* Cancel dialog */
    نتیجہ = sasl->params->cancelauth(ڈیٹا, sasl->curmech);
    newstate = SASL_CANCEL;
    توڑو;
  حالت CURLE_OK:
    نتیجہ = build_پیغام(sasl, &resp);
    اگر(!نتیجہ)
      نتیجہ = sasl->params->contauth(ڈیٹا, sasl->curmech, &resp);
    توڑو;
  طےشدہ:
    newstate = SASL_STOP;    /* Stop on error */
    *پیش_رفت = SASL_مکمل;
    توڑو;
  }

  Curl_bufref_free(&resp);

  sasl_state(sasl, ڈیٹا, newstate);

  واپس نتیجہ;
}

#اگر_تعریف_شدہ CURLVERBOSE
ساکن خالی sasl_unchosen(ساخت Curl_easy *ڈیٹا, غیرعلامتی مختصر mech,
                          غیرعلامتی مختصر enabledmechs,
                          bool built_اندر, bool platform,
                          ثابت حرف *param_missing)
{
  ثابت حرف *mname = تہی;
  size_t i;

  اگر(!(enabledmechs & mech))
    واپس;

  برائے(i = 0; mechtable[i].name; ++i) {
    اگر(mechtable[i].ثنائی_ہندسہ_2 == mech) {
      mname = mechtable[i].name;
      توڑو;
    }
  }
  اگر(!mname)  /* should not happen */
    واپس;
  اگر(!built_اندر)
    infof(ڈیٹا, "SASL: %s not builtin", mname);
  ورنہ اگر(!platform)
    infof(ڈیٹا, "SASL: %s not supported by the platform/libraries", mname);
  ورنہ {
    اگر(param_missing)
      infof(ڈیٹا, "SASL: %s is missing %s", mname, param_missing);
    اگر(!Curl_creds_has_user(ڈیٹا->conn->creds))
      infof(ڈیٹا, "SASL: %s is missing username", mname);
  }
}
#اگر_ختم /* CURLVERBOSE */

CURLcode Curl_sasl_is_blocked(ساخت SASL *sasl, ساخت Curl_easy *ڈیٹا)
{
#اگر_تعریف_شدہ CURLVERBOSE
#اگر_تعریف_شدہ USE_KERBEROS5
#تعریف_کرو CURL_SASL_KERBEROS5   TRUE
#ورنہ
#تعریف_کرو CURL_SASL_KERBEROS5   FALSE
#اگر_ختم
#اگر_تعریف_شدہ USE_GSASL
#تعریف_کرو CURL_SASL_GASL        TRUE
#ورنہ
#تعریف_کرو CURL_SASL_GASL        FALSE
#اگر_ختم
#اگر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
#تعریف_کرو CURL_SASL_DIGEST      TRUE
#ورنہ
#تعریف_کرو CURL_SASL_DIGEST      FALSE
#اگر_ختم
#اگر_غیر_تعریف_شدہ USE_NTLM
#تعریف_کرو CURL_SASL_NTLM        TRUE
#ورنہ
#تعریف_کرو CURL_SASL_NTLM        FALSE
#اگر_ختم
  /* Failing SASL authentication is a pain. Give a helping hand if
   * we were unable to select an AUTH mechanism.
   * `sasl->authmechs` are mechanisms offered by the peer
   * `sasl->prefmech`  are mechanisms preferred by us */
  غیرعلامتی مختصر enabledmechs = sasl->authmechs & sasl->prefmech;

  اگر(!sasl->authmechs)
    infof(ڈیٹا, "SASL: no auth mechanism was offered or recognized");
  ورنہ اگر(!enabledmechs)
    infof(ڈیٹا, "SASL: no overlap between offered and configured "
          "auth mechanisms");
  ورنہ {
    infof(ڈیٹا, "SASL: no auth mechanism offered could be selected");
    اگر((enabledmechs & SASL_MECH_EXTERNAL) &&
       Curl_creds_has_passwd(ڈیٹا->conn->creds))
      infof(ڈیٹا, "SASL: auth EXTERNAL not chosen with password");
    sasl_unchosen(ڈیٹا, SASL_MECH_GSSAPI, enabledmechs,
                  CURL_SASL_KERBEROS5, Curl_auth_is_gssapi_supported(), تہی);
    sasl_unchosen(ڈیٹا, SASL_MECH_SCRAM_SHA_256, enabledmechs,
                  CURL_SASL_GASL, FALSE, تہی);
    sasl_unchosen(ڈیٹا, SASL_MECH_SCRAM_SHA_1, enabledmechs,
                  CURL_SASL_GASL, FALSE, تہی);
    sasl_unchosen(ڈیٹا, SASL_MECH_DIGEST_MD5, enabledmechs,
                  CURL_SASL_DIGEST, Curl_auth_is_digest_supported(), تہی);
    sasl_unchosen(ڈیٹا, SASL_MECH_CRAM_MD5, enabledmechs,
                  CURL_SASL_DIGEST, TRUE, تہی);
    sasl_unchosen(ڈیٹا, SASL_MECH_NTLM, enabledmechs,
                  CURL_SASL_NTLM, Curl_auth_is_ntlm_supported(), تہی);
    sasl_unchosen(ڈیٹا, SASL_MECH_OAUTHBEARER, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(ڈیٹا->conn->creds) ?
                  تہی : "CURLOPT_XOAUTH2_BEARER");
    sasl_unchosen(ڈیٹا, SASL_MECH_XOAUTH2, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(ڈیٹا->conn->creds) ?
                  تہی : "CURLOPT_XOAUTH2_BEARER");
  }
#اگر_ختم /* CURLVERBOSE */
  (خالی)sasl;
  (خالی)ڈیٹا;
  واپس CURLE_LOGIN_DENIED;
}

#اگر_ختم /* protocols are enabled that use SASL */
