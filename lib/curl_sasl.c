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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curlx/base64.h"
#অন্তর্ভাব "vauth/vauth.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "curl_sasl.h"
#অন্তর্ভাব "curl_trc.h"

/* Supported mechanisms */
স্থির ধ্রুবক কাঠামো {
  ধ্রুবক অক্ষর    *name;  /* Name */
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
unsigned short Curl_sasl_decode_mech(ধ্রুবক অক্ষর *ptr, size_t maxlen,
                                     size_t *len)
{
  unsigned পূর্ণসংখ্যা i;
  অক্ষর c;

  জন্য(i = 0; mechtable[i].name; i++) {
    যদি(maxlen >= mechtable[i].len &&
       curl_strnequal(ptr, mechtable[i].name, mechtable[i].len)) {
      যদি(len)
        *len = mechtable[i].len;

      যদি(maxlen == mechtable[i].len)
        ফেরত mechtable[i].bit;

      c = ptr[mechtable[i].len];
      যদি(!ISUPPER(c) && !ISDIGIT(c) && c != '-' && c != '_')
        ফেরত mechtable[i].bit;
    }
  }

  ফেরত 0;
}

/*
 * Curl_sasl_parse_url_auth_option()
 *
 * Parse the URL login options.
 */
CURLcode Curl_sasl_parse_url_auth_option(কাঠামো SASL *sasl,
                                         ধ্রুবক অক্ষর *মান, size_t len)
{
  CURLcode ফলাফল = CURLE_OK;
  size_t mechlen;

  যদি(!len)
    ফেরত CURLE_URL_MALFORMAT;

  যদি(sasl->resetprefs) {
    sasl->resetprefs = FALSE;
    sasl->prefmech = SASL_AUTH_NONE;
  }

  যদি(!strncmp(মান, "*", len))
    sasl->prefmech = SASL_AUTH_DEFAULT;
  অন্যথা {
    unsigned short mechbit = Curl_sasl_decode_mech(মান, len, &mechlen);
    যদি(mechbit && mechlen == len)
      sasl->prefmech |= mechbit;
    অন্যথা
      ফলাফল = CURLE_URL_MALFORMAT;
  }

  ফেরত ফলাফল;
}

/*
 * Curl_sasl_init()
 *
 * Initializes the SASL structure.
 */
শূন্য Curl_sasl_init(কাঠামো SASL *sasl, কাঠামো Curl_easy *তথ্য,
                    ধ্রুবক কাঠামো SASLproto *params)
{
  uint32_t auth = তথ্য->set.httpauth;

  sasl->params = params;           /* Set protocol dependent parameters */
  sasl->state = SASL_STOP;         /* Not yet running */
  sasl->curmech = ফাঁকা;            /* No mechanism yet. */
  sasl->authmechs = SASL_AUTH_NONE; /* No known authentication mechanism yet */
  sasl->prefmech = params->defmechs; /* Default preferred mechanisms */
  sasl->authused = SASL_AUTH_NONE; /* The authentication mechanism used */
  sasl->resetprefs = TRUE;         /* Reset prefmech upon AUTH parsing. */
  sasl->mutual_auth = FALSE;       /* No mutual authentication (GSSAPI only) */
  sasl->force_ir = FALSE;          /* Respect external option */

  যদি(auth != CURLAUTH_BASIC) {
    unsigned short mechs = SASL_AUTH_NONE;

    /* If some usable http authentication options have been set, determine
       new defaults from them. */
    যদি(auth & CURLAUTH_BASIC)
      mechs |= SASL_MECH_PLAIN | SASL_MECH_LOGIN;
    যদি(auth & CURLAUTH_DIGEST)
      mechs |= SASL_MECH_DIGEST_MD5;
    যদি(auth & CURLAUTH_NTLM)
      mechs |= SASL_MECH_NTLM;
    যদি(auth & CURLAUTH_BEARER)
      mechs |= SASL_MECH_OAUTHBEARER | SASL_MECH_XOAUTH2;
    যদি(auth & CURLAUTH_GSSAPI)
      mechs |= SASL_MECH_GSSAPI;

    যদি(mechs != SASL_AUTH_NONE)
      sasl->prefmech = mechs;
  }
}

/*
 * sasl_state()
 *
 * This is the ONLY way to change SASL state!
 */
স্থির শূন্য sasl_state(কাঠামো SASL *sasl, কাঠামো Curl_easy *তথ্য,
                       saslstate newstate)
{
#যদি defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  স্থির ধ্রুবক অক্ষর * ধ্রুবক names[] = {
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

  যদি(sasl->state != newstate)
    infof(তথ্য, "SASL %p state change from %s to %s",
          (শূন্য *)sasl, names[sasl->state], names[newstate]);
#অন্যথা
  (শূন্য)তথ্য;
#endif

  sasl->state = newstate;
}

#যদি defined(USE_NTLM) || defined(USE_GSASL) || defined(USE_KERBEROS5) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)
/* Get the SASL server message and convert it to binary. */
স্থির CURLcode get_server_message(কাঠামো SASL *sasl, কাঠামো Curl_easy *তথ্য,
                                   কাঠামো bufref *out)
{
  CURLcode ফলাফল = CURLE_OK;

  ফলাফল = sasl->params->getmessage(তথ্য, out);
  যদি(!ফলাফল && (sasl->params->flags & SASL_FLAG_BASE64)) {
    ধ্রুবক অক্ষর *serverdata = Curl_bufref_ptr(out);

    যদি(!*serverdata)
      Curl_bufref_set(out, ফাঁকা, 0, ফাঁকা);
    অন্যথা {
      unsigned অক্ষর *বার্তা;
      size_t msglen;

      ফলাফল = curlx_base64_decode(serverdata, &বার্তা, &msglen);
      যদি(!ফলাফল)
        Curl_bufref_set(out, বার্তা, msglen, curl_free);
    }
  }
  ফেরত ফলাফল;
}
#endif

/* Encode the outgoing SASL message. */
স্থির CURLcode build_message(কাঠামো SASL *sasl, কাঠামো bufref *বার্তা)
{
  CURLcode ফলাফল = CURLE_OK;

  যদি(sasl->params->flags & SASL_FLAG_BASE64) {
    যদি(!Curl_bufref_ptr(বার্তা))                   /* Empty message. */
      Curl_bufref_set(বার্তা, "", 0, ফাঁকা);
    অন্যথা যদি(Curl_bufref_len(বার্তা)) {
      অক্ষর *base64;
      size_t base64len;

      ফলাফল = curlx_base64_encode(Curl_bufref_uptr(বার্তা),
                                   Curl_bufref_len(বার্তা), &base64, &base64len);
      যদি(!ফলাফল)
        Curl_bufref_set(বার্তা, base64, base64len, curl_free);
    }
  }

  ফেরত ফলাফল;
}

/*
 * Curl_sasl_can_authenticate()
 *
 * Check if we have enough auth data and capabilities to authenticate.
 */
bool Curl_sasl_can_authenticate(কাঠামো SASL *sasl, কাঠামো Curl_easy *তথ্য)
{
  /* Have credentials been provided? */
  যদি(তথ্য->conn->creds)
    ফেরত TRUE;

  /* EXTERNAL can authenticate without a username and/or password */
  যদি(sasl->authmechs & sasl->prefmech & SASL_MECH_EXTERNAL)
    ফেরত TRUE;

  ফেরত FALSE;
}

কাঠামো sasl_ctx {
  কাঠামো SASL *sasl;
  কাঠামো connectdata *conn;
  unsigned short enabledmechs;
  ধ্রুবক অক্ষর *mech;
  saslstate state1;
  saslstate state2;
  কাঠামো bufref resp;
  CURLcode ফলাফল;
};

স্থির bool sasl_choose_external(কাঠামো Curl_easy *তথ্য, কাঠামো sasl_ctx *sctx)
{
  যদি((sctx->enabledmechs & SASL_MECH_EXTERNAL) &&
     !Curl_creds_has_passwd(sctx->conn->creds)) {
    sctx->mech = SASL_MECH_STRING_EXTERNAL;
    sctx->state1 = SASL_EXTERNAL;
    sctx->sasl->authused = SASL_MECH_EXTERNAL;

    যদি(sctx->sasl->force_ir || তথ্য->set.sasl_ir)
      Curl_auth_create_external_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    ফেরত TRUE;
  }
  ফেরত FALSE;
}

#ifdef USE_KERBEROS5
স্থির bool sasl_choose_krb5(কাঠামো Curl_easy *তথ্য, কাঠামো sasl_ctx *sctx)
{
  যদি((sctx->enabledmechs & SASL_MECH_GSSAPI) &&
     Curl_auth_is_gssapi_supported() &&
     Curl_auth_user_contains_domain(sctx->conn->creds)) {
    ধ্রুবক অক্ষর *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;

    sctx->sasl->mutual_auth = FALSE;
    sctx->mech = SASL_MECH_STRING_GSSAPI;
    sctx->state1 = SASL_GSSAPI;
    sctx->state2 = SASL_GSSAPI_TOKEN;
    sctx->sasl->authused = SASL_MECH_GSSAPI;

    যদি(sctx->sasl->force_ir || তথ্য->set.sasl_ir) {
      কাঠামো kerberos5data *krb5 = Curl_auth_krb5_get(sctx->conn);
      sctx->ফলাফল = !krb5 ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_gssapi_user_message(তথ্য, sctx->conn->creds,
                                             service,
                                             sctx->conn->origin->hostname,
                                             (bool)sctx->sasl->mutual_auth,
                                             ফাঁকা, krb5, &sctx->resp);
    }
    ফেরত TRUE;
  }
  ফেরত FALSE;
}
#endif /* USE_KERBEROS5 */

#ifdef USE_GSASL
স্থির bool sasl_choose_gsasl(কাঠামো Curl_easy *তথ্য, কাঠামো sasl_ctx *sctx)
{
  কাঠামো gsasldata *gsasl;
  কাঠামো bufref nullmsg;

  যদি((sctx->enabledmechs &
      (SASL_MECH_SCRAM_SHA_256 | SASL_MECH_SCRAM_SHA_1))) {
    gsasl = Curl_auth_gsasl_get(sctx->conn);
    যদি(!gsasl) {
      sctx->ফলাফল = CURLE_OUT_OF_MEMORY;
      ফেরত TRUE; /* attempted, but failed */
    }

    যদি((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_256) &&
       Curl_auth_gsasl_is_supported(তথ্য, SASL_MECH_STRING_SCRAM_SHA_256,
                                    gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_256;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_256;
    }
    অন্যথা যদি((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_1) &&
            Curl_auth_gsasl_is_supported(তথ্য, SASL_MECH_STRING_SCRAM_SHA_1,
                                         gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_1;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_1;
    }
    অন্যথা
      ফেরত FALSE;

    Curl_bufref_init(&nullmsg);
    sctx->state1 = SASL_GSASL;
    sctx->state2 = SASL_GSASL;
    sctx->ফলাফল = Curl_auth_gsasl_start(তথ্য, sctx->conn->creds, gsasl);
    যদি(!sctx->ফলাফল && (sctx->sasl->force_ir || তথ্য->set.sasl_ir))
      sctx->ফলাফল = Curl_auth_gsasl_token(তথ্য, &nullmsg, gsasl, &sctx->resp);
    ফেরত TRUE;
  }
  ফেরত FALSE;
}

#endif /* USE_GSASL */

#ifndef CURL_DISABLE_DIGEST_AUTH
স্থির bool sasl_choose_digest(কাঠামো Curl_easy *তথ্য, কাঠামো sasl_ctx *sctx)
{
  (শূন্য)তথ্য;
  যদি((sctx->enabledmechs & SASL_MECH_DIGEST_MD5) &&
     Curl_auth_is_digest_supported()) {
    sctx->mech = SASL_MECH_STRING_DIGEST_MD5;
    sctx->state1 = SASL_DIGESTMD5;
    sctx->sasl->authused = SASL_MECH_DIGEST_MD5;
    ফেরত TRUE;
  }
  অন্যথা যদি(sctx->enabledmechs & SASL_MECH_CRAM_MD5) {
    sctx->mech = SASL_MECH_STRING_CRAM_MD5;
    sctx->state1 = SASL_CRAMMD5;
    sctx->sasl->authused = SASL_MECH_CRAM_MD5;
    ফেরত TRUE;
  }
  ফেরত FALSE;
}
#endif /* !CURL_DISABLE_DIGEST_AUTH */

#ifdef USE_NTLM
স্থির bool sasl_choose_ntlm(কাঠামো Curl_easy *তথ্য, কাঠামো sasl_ctx *sctx)
{
  যদি((sctx->enabledmechs & SASL_MECH_NTLM) &&
     Curl_auth_is_ntlm_supported()) {
     ধ্রুবক অক্ষর *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;
    ধ্রুবক অক্ষর *hostname;

    Curl_conn_get_current_host(তথ্য, FIRSTSOCKET, &hostname, ফাঁকা);

    sctx->mech = SASL_MECH_STRING_NTLM;
    sctx->state1 = SASL_NTLM;
    sctx->state2 = SASL_NTLM_TYPE2_বার্তা;
    sctx->sasl->authused = SASL_MECH_NTLM;

    যদি(sctx->sasl->force_ir || তথ্য->set.sasl_ir) {
      কাঠামো ntlmdata *ntlm = Curl_auth_ntlm_get(sctx->conn, FALSE);
      sctx->ফলাফল = !ntlm ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_ntlm_type1_message(তথ্য, sctx->conn->creds,
                                            service, hostname,
                                            ntlm, &sctx->resp);
    }
    ফেরত TRUE;
  }
  ফেরত FALSE;
}
#endif /* USE_NTLM */

স্থির bool sasl_choose_oauth(কাঠামো Curl_easy *তথ্য, কাঠামো sasl_ctx *sctx)
{
  যদি(Curl_creds_has_oauth_bearer(তথ্য->state.creds) &&
     (sctx->enabledmechs & SASL_MECH_OAUTHBEARER)) {
    ধ্রুবক অক্ষর *hostname;
    পূর্ণসংখ্যা port;
    Curl_conn_get_current_host(তথ্য, FIRSTSOCKET, &hostname, &port);

    sctx->mech = SASL_MECH_STRING_OAUTHBEARER;
    sctx->state1 = SASL_OAUTH2;
    sctx->state2 = SASL_OAUTH2_RESP;
    sctx->sasl->authused = SASL_MECH_OAUTHBEARER;

    যদি(sctx->sasl->force_ir || তথ্য->set.sasl_ir)
      sctx->ফলাফল =
        Curl_auth_create_oauth_bearer_message(sctx->conn->creds,
                                              hostname, port, &sctx->resp);
    ফেরত TRUE;
  }
  ফেরত FALSE;
}

স্থির bool sasl_choose_oauth2(কাঠামো Curl_easy *তথ্য, কাঠামো sasl_ctx *sctx)
{
  যদি(Curl_creds_has_oauth_bearer(sctx->conn->creds) &&
     (sctx->enabledmechs & SASL_MECH_XOAUTH2)) {
    sctx->mech = SASL_MECH_STRING_XOAUTH2;
    sctx->state1 = SASL_OAUTH2;
    sctx->sasl->authused = SASL_MECH_XOAUTH2;

    যদি(sctx->sasl->force_ir || তথ্য->set.sasl_ir)
      sctx->ফলাফল = Curl_auth_create_xoauth_bearer_message(
        sctx->conn->creds, &sctx->resp);
    ফেরত TRUE;
  }
  ফেরত FALSE;
}

স্থির bool sasl_choose_plain(কাঠামো Curl_easy *তথ্য, কাঠামো sasl_ctx *sctx)
{
  যদি(sctx->enabledmechs & SASL_MECH_PLAIN) {
    sctx->mech = SASL_MECH_STRING_PLAIN;
    sctx->state1 = SASL_PLAIN;
    sctx->sasl->authused = SASL_MECH_PLAIN;

    যদি(sctx->sasl->force_ir || তথ্য->set.sasl_ir)
      sctx->ফলাফল =
        Curl_auth_create_plain_message(sctx->conn->creds, &sctx->resp);
    ফেরত TRUE;
  }
  ফেরত FALSE;
}

স্থির bool sasl_choose_login(কাঠামো Curl_easy *তথ্য, কাঠামো sasl_ctx *sctx)
{
  যদি(sctx->enabledmechs & SASL_MECH_LOGIN) {
    sctx->mech = SASL_MECH_STRING_LOGIN;
    sctx->state1 = SASL_LOGIN;
    sctx->state2 = SASL_LOGIN_PASSWD;
    sctx->sasl->authused = SASL_MECH_LOGIN;

    যদি(sctx->sasl->force_ir || তথ্য->set.sasl_ir)
      Curl_auth_create_login_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    ফেরত TRUE;
  }
  ফেরত FALSE;
}

/*
 * Curl_sasl_start()
 *
 * Calculate the required login details for SASL authentication.
 */
CURLcode Curl_sasl_start(কাঠামো SASL *sasl, কাঠামো Curl_easy *তথ্য,
                         bool force_ir, saslprogress *progress)
{
  কাঠামো sasl_ctx sctx;

  sasl->force_ir = force_ir;    /* Latch for future use */
  sasl->authused = 0;           /* No mechanism used yet */
  *progress = SASL_IDLE;

  memset(&sctx, 0, আকার(sctx));
  sctx.sasl = sasl;
  sctx.conn = তথ্য->conn;
  Curl_bufref_init(&sctx.resp);
  sctx.enabledmechs = sasl->authmechs & sasl->prefmech;
  sctx.state1 = SASL_STOP;
  sctx.state2 = SASL_FINAL;

  /* Calculate the supported authentication mechanism, by decreasing order of
     security, as well as the initial response where appropriate */
  যদি(sasl_choose_external(তথ্য, &sctx) ||
#ifdef USE_KERBEROS5
     sasl_choose_krb5(তথ্য, &sctx) ||
#endif
#ifdef USE_GSASL
     sasl_choose_gsasl(তথ্য, &sctx) ||
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
     sasl_choose_digest(তথ্য, &sctx) ||
#endif
#ifdef USE_NTLM
     sasl_choose_ntlm(তথ্য, &sctx) ||
#endif
     sasl_choose_oauth(তথ্য, &sctx) ||
     sasl_choose_oauth2(তথ্য, &sctx) ||
     sasl_choose_plain(তথ্য, &sctx) ||
     sasl_choose_login(তথ্য, &sctx)) {
    /* selected, either we have a mechanism or a failure */
    DEBUGASSERT(sctx.mech || sctx.ফলাফল);
  }

  যদি(!sctx.ফলাফল && sctx.mech) {
    sasl->curmech = sctx.mech;
    যদি(Curl_bufref_ptr(&sctx.resp))
      sctx.ফলাফল = build_message(sasl, &sctx.resp);

    যদি(sasl->params->maxirlen &&
       strlen(sctx.mech) + Curl_bufref_len(&sctx.resp) >
         sasl->params->maxirlen)
      Curl_bufref_free(&sctx.resp);

    যদি(!sctx.ফলাফল)
      sctx.ফলাফল = sasl->params->sendauth(তথ্য, sctx.mech, &sctx.resp);

    যদি(!sctx.ফলাফল) {
      *progress = SASL_INPROGRESS;
      sasl_state(sasl, তথ্য, Curl_bufref_ptr(&sctx.resp) ?
                 sctx.state2 : sctx.state1);
    }
  }

  Curl_bufref_free(&sctx.resp);
  ফেরত sctx.ফলাফল;
}

/*
 * Curl_sasl_continue()
 *
 * Continue the authentication.
 */
CURLcode Curl_sasl_continue(কাঠামো SASL *sasl, কাঠামো Curl_easy *তথ্য,
                            পূর্ণসংখ্যা code, saslprogress *progress)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো connectdata *conn = তথ্য->conn;
  saslstate newstate = SASL_FINAL;
  কাঠামো bufref resp;
  ধ্রুবক অক্ষর *hostname;
  পূর্ণসংখ্যা port;
  কাঠামো bufref serverdata;

  Curl_conn_get_current_host(তথ্য, FIRSTSOCKET, &hostname, &port);
  Curl_bufref_init(&serverdata);
  Curl_bufref_init(&resp);
  *progress = SASL_INPROGRESS;

  যদি(sasl->state == SASL_FINAL) {
    যদি(code != sasl->params->finalcode)
      ফলাফল = CURLE_LOGIN_DENIED;
    *progress = SASL_DONE;
    sasl_state(sasl, তথ্য, SASL_STOP);
    ফেরত ফলাফল;
  }

  যদি(sasl->state != SASL_CANCEL && sasl->state != SASL_OAUTH2_RESP &&
     code != sasl->params->contcode) {
    *progress = SASL_DONE;
    sasl_state(sasl, তথ্য, SASL_STOP);
    ফেরত CURLE_LOGIN_DENIED;
  }

  নির্বাচন(sasl->state) {
  ক্ষেত্র SASL_STOP:
    *progress = SASL_DONE;
    ফেরত ফলাফল;
  ক্ষেত্র SASL_PLAIN:
    ফলাফল = Curl_auth_create_plain_message(conn->creds, &resp);
    বিরতি;
  ক্ষেত্র SASL_LOGIN:
    Curl_auth_create_login_message(Curl_creds_user(conn->creds), &resp);
    newstate = SASL_LOGIN_PASSWD;
    বিরতি;
  ক্ষেত্র SASL_LOGIN_PASSWD:
    Curl_auth_create_login_message(Curl_creds_passwd(conn->creds), &resp);
    বিরতি;
  ক্ষেত্র SASL_EXTERNAL:
    Curl_auth_create_external_message(Curl_creds_user(conn->creds), &resp);
    বিরতি;
#ifdef USE_GSASL
  ক্ষেত্র SASL_GSASL:
    ফলাফল = get_server_message(sasl, তথ্য, &serverdata);
    যদি(!ফলাফল) {
      কাঠামো gsasldata *gsasl = Curl_auth_gsasl_get(conn);
      ফলাফল = !gsasl ? CURLE_OUT_OF_MEMORY :
        Curl_auth_gsasl_token(তথ্য, &serverdata, gsasl, &resp);
    }
    যদি(!ফলাফল && Curl_bufref_len(&resp) > 0)
      newstate = SASL_GSASL;
    বিরতি;
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  ক্ষেত্র SASL_CRAMMD5:
    ফলাফল = get_server_message(sasl, তথ্য, &serverdata);
    যদি(!ফলাফল)
      ফলাফল = Curl_auth_create_cram_md5_message(&serverdata, conn->creds,
                                                 &resp);
    বিরতি;
  ক্ষেত্র SASL_DIGESTMD5:
    ফলাফল = get_server_message(sasl, তথ্য, &serverdata);
    যদি(!ফলাফল)
      ফলাফল = Curl_auth_create_digest_md5_message(তথ্য, &serverdata,
                                                   conn->creds,
                                                   sasl->params->service,
                                                   &resp);
    যদি(!ফলাফল && (sasl->params->flags & SASL_FLAG_BASE64))
      newstate = SASL_DIGESTMD5_RESP;
    বিরতি;
  ক্ষেত্র SASL_DIGESTMD5_RESP:
    /* Keep response NULL to output an empty line. */
    বিরতি;
#endif

#ifdef USE_NTLM
  ক্ষেত্র SASL_NTLM: {
    /* Create the type-1 message */
    কাঠামো ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    ফলাফল = !ntlm ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_ntlm_type1_message(তথ্য, conn->creds,
                                          sasl->params->service, hostname,
                                          ntlm, &resp);
    newstate = SASL_NTLM_TYPE2_বার্তা;
    বিরতি;
  }
  ক্ষেত্র SASL_NTLM_TYPE2_বার্তা: {
    /* Decode the type-2 message */
    কাঠামো ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    ফলাফল = !ntlm ? CURLE_OUT_OF_MEMORY :
      get_server_message(sasl, তথ্য, &serverdata);
    যদি(!ফলাফল)
      ফলাফল = Curl_auth_decode_ntlm_type2_message(তথ্য, &serverdata, ntlm);
    যদি(!ফলাফল)
      ফলাফল = Curl_auth_create_ntlm_type3_message(তথ্য, conn->creds,
                                                   ntlm, &resp);
    বিরতি;
  }
#endif

#ifdef USE_KERBEROS5
  ক্ষেত্র SASL_GSSAPI: {
    কাঠামো kerberos5data *krb5 = Curl_auth_krb5_get(conn);
    ফলাফল = !krb5 ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_gssapi_user_message(তথ্য, conn->creds,
                                           sasl->params->service,
                                           conn->origin->hostname,
                                           (bool)sasl->mutual_auth, ফাঁকা,
                                           krb5, &resp);
    newstate = SASL_GSSAPI_TOKEN;
    বিরতি;
  }
  ক্ষেত্র SASL_GSSAPI_TOKEN:
    ফলাফল = get_server_message(sasl, তথ্য, &serverdata);
    যদি(!ফলাফল) {
      কাঠামো kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      যদি(!krb5)
        ফলাফল = CURLE_OUT_OF_MEMORY;
      অন্যথা যদি(sasl->mutual_auth) {
        /* Decode the user token challenge and create the optional response
           message */
        ফলাফল = Curl_auth_create_gssapi_user_message(তথ্য, ফাঁকা,
                                                      ফাঁকা, ফাঁকা,
                                                      (bool)sasl->mutual_auth,
                                                      &serverdata,
                                                      krb5, &resp);
        newstate = SASL_GSSAPI_NO_তথ্য;
      }
      অন্যথা
        /* Decode the security challenge and create the response message */
        ফলাফল = Curl_auth_create_gssapi_security_message(
          তথ্য, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    বিরতি;
  ক্ষেত্র SASL_GSSAPI_NO_তথ্য:
    /* Decode the security challenge and create the response message */
    ফলাফল = get_server_message(sasl, তথ্য, &serverdata);
    যদি(!ফলাফল) {
      কাঠামো kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      যদি(!krb5)
        ফলাফল = CURLE_OUT_OF_MEMORY;
      অন্যথা
        ফলাফল = Curl_auth_create_gssapi_security_message(
          তথ্য, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    বিরতি;
#endif

  ক্ষেত্র SASL_OAUTH2:
    /* Create the authorization message */
    যদি(sasl->authused == SASL_MECH_OAUTHBEARER) {
      ফলাফল = Curl_auth_create_oauth_bearer_message(conn->creds,
                                                     hostname,
                                                     port,
                                                     &resp);

      /* Failures maybe sent by the server as continuations for OAUTHBEARER */
      newstate = SASL_OAUTH2_RESP;
    }
    অন্যথা
      ফলাফল = Curl_auth_create_xoauth_bearer_message(conn->creds,
                                                      &resp);
    বিরতি;

  ক্ষেত্র SASL_OAUTH2_RESP:
    /* The continuation is optional so check the response code */
    যদি(code == sasl->params->finalcode) {
      /* Final response was received so we are done */
      *progress = SASL_DONE;
      sasl_state(sasl, তথ্য, SASL_STOP);
      ফেরত ফলাফল;
    }
    অন্যথা যদি(code == sasl->params->contcode) {
      /* Acknowledge the continuation by sending a 0x01 response. */
      Curl_bufref_set(&resp, "\x01", 1, ফাঁকা);
      বিরতি;
    }
    অন্যথা {
      *progress = SASL_DONE;
      sasl_state(sasl, তথ্য, SASL_STOP);
      ফেরত CURLE_LOGIN_DENIED;
    }

  ক্ষেত্র SASL_CANCEL:
    /* Remove the offending mechanism from the supported list */
    sasl->authmechs &= (unsigned short)~sasl->authused;
    sasl->authused = SASL_AUTH_NONE;
    sasl->curmech = ফাঁকা;

    /* Start an alternative SASL authentication */
    ফেরত Curl_sasl_start(sasl, তথ্য, (bool)sasl->force_ir, progress);
  পূর্বনির্ধারিত:
    failf(তথ্য, "Unsupported SASL authentication mechanism");
    ফলাফল = CURLE_UNSUPPORTED_PROTOCOL;  /* Should not happen */
    বিরতি;
  }

  Curl_bufref_free(&serverdata);

  নির্বাচন(ফলাফল) {
  ক্ষেত্র CURLE_BAD_CONTENT_ENCODING:
    /* Cancel dialog */
    ফলাফল = sasl->params->cancelauth(তথ্য, sasl->curmech);
    newstate = SASL_CANCEL;
    বিরতি;
  ক্ষেত্র CURLE_OK:
    ফলাফল = build_message(sasl, &resp);
    যদি(!ফলাফল)
      ফলাফল = sasl->params->contauth(তথ্য, sasl->curmech, &resp);
    বিরতি;
  পূর্বনির্ধারিত:
    newstate = SASL_STOP;    /* Stop on error */
    *progress = SASL_DONE;
    বিরতি;
  }

  Curl_bufref_free(&resp);

  sasl_state(sasl, তথ্য, newstate);

  ফেরত ফলাফল;
}

#ifdef CURLVERBOSE
স্থির শূন্য sasl_unchosen(কাঠামো Curl_easy *তথ্য, unsigned short mech,
                          unsigned short enabledmechs,
                          bool built_in, bool platform,
                          ধ্রুবক অক্ষর *param_missing)
{
  ধ্রুবক অক্ষর *mname = ফাঁকা;
  size_t i;

  যদি(!(enabledmechs & mech))
    ফেরত;

  জন্য(i = 0; mechtable[i].name; ++i) {
    যদি(mechtable[i].bit == mech) {
      mname = mechtable[i].name;
      বিরতি;
    }
  }
  যদি(!mname)  /* should not happen */
    ফেরত;
  যদি(!built_in)
    infof(তথ্য, "SASL: %s not builtin", mname);
  অন্যথা যদি(!platform)
    infof(তথ্য, "SASL: %s not supported by the platform/libraries", mname);
  অন্যথা {
    যদি(param_missing)
      infof(তথ্য, "SASL: %s is missing %s", mname, param_missing);
    যদি(!Curl_creds_has_user(তথ্য->conn->creds))
      infof(তথ্য, "SASL: %s is missing username", mname);
  }
}
#endif /* CURLVERBOSE */

CURLcode Curl_sasl_is_blocked(কাঠামো SASL *sasl, কাঠামো Curl_easy *তথ্য)
{
#ifdef CURLVERBOSE
#ifdef USE_KERBEROS5
#define CURL_SASL_KERBEROS5   TRUE
#অন্যথা
#define CURL_SASL_KERBEROS5   FALSE
#endif
#ifdef USE_GSASL
#define CURL_SASL_GASL        TRUE
#অন্যথা
#define CURL_SASL_GASL        FALSE
#endif
#ifdef CURL_DISABLE_DIGEST_AUTH
#define CURL_SASL_DIGEST      TRUE
#অন্যথা
#define CURL_SASL_DIGEST      FALSE
#endif
#ifndef USE_NTLM
#define CURL_SASL_NTLM        TRUE
#অন্যথা
#define CURL_SASL_NTLM        FALSE
#endif
  /* Failing SASL authentication is a pain. Give a helping hand if
   * we were unable to select an AUTH mechanism.
   * `sasl->authmechs` are mechanisms offered by the peer
   * `sasl->prefmech`  are mechanisms preferred by us */
  unsigned short enabledmechs = sasl->authmechs & sasl->prefmech;

  যদি(!sasl->authmechs)
    infof(তথ্য, "SASL: no auth mechanism was offered or recognized");
  অন্যথা যদি(!enabledmechs)
    infof(তথ্য, "SASL: no overlap between offered and configured "
          "auth mechanisms");
  অন্যথা {
    infof(তথ্য, "SASL: no auth mechanism offered could be selected");
    যদি((enabledmechs & SASL_MECH_EXTERNAL) &&
       Curl_creds_has_passwd(তথ্য->conn->creds))
      infof(তথ্য, "SASL: auth EXTERNAL not chosen with password");
    sasl_unchosen(তথ্য, SASL_MECH_GSSAPI, enabledmechs,
                  CURL_SASL_KERBEROS5, Curl_auth_is_gssapi_supported(), ফাঁকা);
    sasl_unchosen(তথ্য, SASL_MECH_SCRAM_SHA_256, enabledmechs,
                  CURL_SASL_GASL, FALSE, ফাঁকা);
    sasl_unchosen(তথ্য, SASL_MECH_SCRAM_SHA_1, enabledmechs,
                  CURL_SASL_GASL, FALSE, ফাঁকা);
    sasl_unchosen(তথ্য, SASL_MECH_DIGEST_MD5, enabledmechs,
                  CURL_SASL_DIGEST, Curl_auth_is_digest_supported(), ফাঁকা);
    sasl_unchosen(তথ্য, SASL_MECH_CRAM_MD5, enabledmechs,
                  CURL_SASL_DIGEST, TRUE, ফাঁকা);
    sasl_unchosen(তথ্য, SASL_MECH_NTLM, enabledmechs,
                  CURL_SASL_NTLM, Curl_auth_is_ntlm_supported(), ফাঁকা);
    sasl_unchosen(তথ্য, SASL_MECH_OAUTHBEARER, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(তথ্য->conn->creds) ?
                  ফাঁকা : "CURLOPT_XOAUTH2_BEARER");
    sasl_unchosen(তথ্য, SASL_MECH_XOAUTH2, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(তথ্য->conn->creds) ?
                  ফাঁকা : "CURLOPT_XOAUTH2_BEARER");
  }
#endif /* CURLVERBOSE */
  (শূন্য)sasl;
  (শূন্য)তথ্য;
  ফেরত CURLE_LOGIN_DENIED;
}

#endif /* protocols are enabled that use SASL */
