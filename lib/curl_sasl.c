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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curlx/base64.h"
#અન્તર્ભાવ "vauth/vauth.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "curl_sasl.h"
#અન્તર્ભાવ "curl_trc.h"

/* Supported mechanisms */
સ્થિર અચલ સંરચના {
  અચલ અક્ષર    *name;  /* Name */
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
unsigned short Curl_sasl_decode_mech(અચલ અક્ષર *ptr, size_t maxlen,
                                     size_t *len)
{
  unsigned પૂર્ણાંક i;
  અક્ષર c;

  માટે(i = 0; mechtable[i].name; i++) {
    જો(maxlen >= mechtable[i].len &&
       curl_strnequal(ptr, mechtable[i].name, mechtable[i].len)) {
      જો(len)
        *len = mechtable[i].len;

      જો(maxlen == mechtable[i].len)
        પાછા mechtable[i].bit;

      c = ptr[mechtable[i].len];
      જો(!ISUPPER(c) && !ISDIGIT(c) && c != '-' && c != '_')
        પાછા mechtable[i].bit;
    }
  }

  પાછા 0;
}

/*
 * Curl_sasl_parse_url_auth_option()
 *
 * Parse the URL login options.
 */
CURLcode Curl_sasl_parse_url_auth_option(સંરચના SASL *sasl,
                                         અચલ અક્ષર *કિંમત, size_t len)
{
  CURLcode result = CURLE_OK;
  size_t mechlen;

  જો(!len)
    પાછા CURLE_URL_MALFORMAT;

  જો(sasl->resetprefs) {
    sasl->resetprefs = FALSE;
    sasl->prefmech = SASL_AUTH_NONE;
  }

  જો(!strncmp(કિંમત, "*", len))
    sasl->prefmech = SASL_AUTH_DEFAULT;
  નહીંતર {
    unsigned short mechbit = Curl_sasl_decode_mech(કિંમત, len, &mechlen);
    જો(mechbit && mechlen == len)
      sasl->prefmech |= mechbit;
    નહીંતર
      result = CURLE_URL_MALFORMAT;
  }

  પાછા result;
}

/*
 * Curl_sasl_init()
 *
 * Initializes the SASL structure.
 */
શૂન્ય Curl_sasl_init(સંરચના SASL *sasl, સંરચના Curl_easy *data,
                    અચલ સંરચના SASLproto *params)
{
  uint32_t auth = data->set.httpauth;

  sasl->params = params;           /* Set protocol dependent parameters */
  sasl->state = SASL_STOP;         /* Not yet running */
  sasl->curmech = ખાલી;            /* No mechanism yet. */
  sasl->authmechs = SASL_AUTH_NONE; /* No known authentication mechanism yet */
  sasl->prefmech = params->defmechs; /* Default preferred mechanisms */
  sasl->authused = SASL_AUTH_NONE; /* The authentication mechanism used */
  sasl->resetprefs = TRUE;         /* Reset prefmech upon AUTH parsing. */
  sasl->mutual_auth = FALSE;       /* No mutual authentication (GSSAPI only) */
  sasl->force_ir = FALSE;          /* Respect external option */

  જો(auth != CURLAUTH_BASIC) {
    unsigned short mechs = SASL_AUTH_NONE;

    /* If some usable http authentication options have been set, determine
       new defaults from them. */
    જો(auth & CURLAUTH_BASIC)
      mechs |= SASL_MECH_PLAIN | SASL_MECH_LOGIN;
    જો(auth & CURLAUTH_DIGEST)
      mechs |= SASL_MECH_DIGEST_MD5;
    જો(auth & CURLAUTH_NTLM)
      mechs |= SASL_MECH_NTLM;
    જો(auth & CURLAUTH_BEARER)
      mechs |= SASL_MECH_OAUTHBEARER | SASL_MECH_XOAUTH2;
    જો(auth & CURLAUTH_GSSAPI)
      mechs |= SASL_MECH_GSSAPI;

    જો(mechs != SASL_AUTH_NONE)
      sasl->prefmech = mechs;
  }
}

/*
 * sasl_state()
 *
 * This is the ONLY way to change SASL state!
 */
સ્થિર શૂન્ય sasl_state(સંરચના SASL *sasl, સંરચના Curl_easy *data,
                       saslstate newstate)
{
#જો defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  સ્થિર અચલ અક્ષર * અચલ names[] = {
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

  જો(sasl->state != newstate)
    infof(data, "SASL %p state change from %s to %s",
          (શૂન્ય *)sasl, names[sasl->state], names[newstate]);
#નહીંતર
  (શૂન્ય)data;
#endif

  sasl->state = newstate;
}

#જો defined(USE_NTLM) || defined(USE_GSASL) || defined(USE_KERBEROS5) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)
/* Get the SASL server message and convert it to binary. */
સ્થિર CURLcode get_server_message(સંરચના SASL *sasl, સંરચના Curl_easy *data,
                                   સંરચના bufref *out)
{
  CURLcode result = CURLE_OK;

  result = sasl->params->getmessage(data, out);
  જો(!result && (sasl->params->flags & SASL_FLAG_BASE64)) {
    અચલ અક્ષર *serverdata = Curl_bufref_ptr(out);

    જો(!*serverdata)
      Curl_bufref_set(out, ખાલી, 0, ખાલી);
    નહીંતર {
      unsigned અક્ષર *સંદેશ;
      size_t msglen;

      result = curlx_base64_decode(serverdata, &સંદેશ, &msglen);
      જો(!result)
        Curl_bufref_set(out, સંદેશ, msglen, curl_free);
    }
  }
  પાછા result;
}
#endif

/* Encode the outgoing SASL message. */
સ્થિર CURLcode build_message(સંરચના SASL *sasl, સંરચના bufref *સંદેશ)
{
  CURLcode result = CURLE_OK;

  જો(sasl->params->flags & SASL_FLAG_BASE64) {
    જો(!Curl_bufref_ptr(સંદેશ))                   /* Empty message. */
      Curl_bufref_set(સંદેશ, "", 0, ખાલી);
    નહીંતર જો(Curl_bufref_len(સંદેશ)) {
      અક્ષર *base64;
      size_t base64len;

      result = curlx_base64_encode(Curl_bufref_uptr(સંદેશ),
                                   Curl_bufref_len(સંદેશ), &base64, &base64len);
      જો(!result)
        Curl_bufref_set(સંદેશ, base64, base64len, curl_free);
    }
  }

  પાછા result;
}

/*
 * Curl_sasl_can_authenticate()
 *
 * Check if we have enough auth data and capabilities to authenticate.
 */
bool Curl_sasl_can_authenticate(સંરચના SASL *sasl, સંરચના Curl_easy *data)
{
  /* Have credentials been provided? */
  જો(data->conn->creds)
    પાછા TRUE;

  /* EXTERNAL can authenticate without a username and/or password */
  જો(sasl->authmechs & sasl->prefmech & SASL_MECH_EXTERNAL)
    પાછા TRUE;

  પાછા FALSE;
}

સંરચના sasl_ctx {
  સંરચના SASL *sasl;
  સંરચના connectdata *conn;
  unsigned short enabledmechs;
  અચલ અક્ષર *mech;
  saslstate state1;
  saslstate state2;
  સંરચના bufref resp;
  CURLcode result;
};

સ્થિર bool sasl_choose_external(સંરચના Curl_easy *data, સંરચના sasl_ctx *sctx)
{
  જો((sctx->enabledmechs & SASL_MECH_EXTERNAL) &&
     !Curl_creds_has_passwd(sctx->conn->creds)) {
    sctx->mech = SASL_MECH_STRING_EXTERNAL;
    sctx->state1 = SASL_EXTERNAL;
    sctx->sasl->authused = SASL_MECH_EXTERNAL;

    જો(sctx->sasl->force_ir || data->set.sasl_ir)
      Curl_auth_create_external_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    પાછા TRUE;
  }
  પાછા FALSE;
}

#ifdef USE_KERBEROS5
સ્થિર bool sasl_choose_krb5(સંરચના Curl_easy *data, સંરચના sasl_ctx *sctx)
{
  જો((sctx->enabledmechs & SASL_MECH_GSSAPI) &&
     Curl_auth_is_gssapi_supported() &&
     Curl_auth_user_contains_domain(sctx->conn->creds)) {
    અચલ અક્ષર *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;

    sctx->sasl->mutual_auth = FALSE;
    sctx->mech = SASL_MECH_STRING_GSSAPI;
    sctx->state1 = SASL_GSSAPI;
    sctx->state2 = SASL_GSSAPI_TOKEN;
    sctx->sasl->authused = SASL_MECH_GSSAPI;

    જો(sctx->sasl->force_ir || data->set.sasl_ir) {
      સંરચના kerberos5data *krb5 = Curl_auth_krb5_get(sctx->conn);
      sctx->result = !krb5 ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_gssapi_user_message(data, sctx->conn->creds,
                                             service,
                                             sctx->conn->origin->hostname,
                                             (bool)sctx->sasl->mutual_auth,
                                             ખાલી, krb5, &sctx->resp);
    }
    પાછા TRUE;
  }
  પાછા FALSE;
}
#endif /* USE_KERBEROS5 */

#ifdef USE_GSASL
સ્થિર bool sasl_choose_gsasl(સંરચના Curl_easy *data, સંરચના sasl_ctx *sctx)
{
  સંરચના gsasldata *gsasl;
  સંરચના bufref nullmsg;

  જો((sctx->enabledmechs &
      (SASL_MECH_SCRAM_SHA_256 | SASL_MECH_SCRAM_SHA_1))) {
    gsasl = Curl_auth_gsasl_get(sctx->conn);
    જો(!gsasl) {
      sctx->result = CURLE_OUT_OF_MEMORY;
      પાછા TRUE; /* attempted, but failed */
    }

    જો((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_256) &&
       Curl_auth_gsasl_is_supported(data, SASL_MECH_STRING_SCRAM_SHA_256,
                                    gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_256;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_256;
    }
    નહીંતર જો((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_1) &&
            Curl_auth_gsasl_is_supported(data, SASL_MECH_STRING_SCRAM_SHA_1,
                                         gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_1;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_1;
    }
    નહીંતર
      પાછા FALSE;

    Curl_bufref_init(&nullmsg);
    sctx->state1 = SASL_GSASL;
    sctx->state2 = SASL_GSASL;
    sctx->result = Curl_auth_gsasl_start(data, sctx->conn->creds, gsasl);
    જો(!sctx->result && (sctx->sasl->force_ir || data->set.sasl_ir))
      sctx->result = Curl_auth_gsasl_token(data, &nullmsg, gsasl, &sctx->resp);
    પાછા TRUE;
  }
  પાછા FALSE;
}

#endif /* USE_GSASL */

#ifndef CURL_DISABLE_DIGEST_AUTH
સ્થિર bool sasl_choose_digest(સંરચના Curl_easy *data, સંરચના sasl_ctx *sctx)
{
  (શૂન્ય)data;
  જો((sctx->enabledmechs & SASL_MECH_DIGEST_MD5) &&
     Curl_auth_is_digest_supported()) {
    sctx->mech = SASL_MECH_STRING_DIGEST_MD5;
    sctx->state1 = SASL_DIGESTMD5;
    sctx->sasl->authused = SASL_MECH_DIGEST_MD5;
    પાછા TRUE;
  }
  નહીંતર જો(sctx->enabledmechs & SASL_MECH_CRAM_MD5) {
    sctx->mech = SASL_MECH_STRING_CRAM_MD5;
    sctx->state1 = SASL_CRAMMD5;
    sctx->sasl->authused = SASL_MECH_CRAM_MD5;
    પાછા TRUE;
  }
  પાછા FALSE;
}
#endif /* !CURL_DISABLE_DIGEST_AUTH */

#ifdef USE_NTLM
સ્થિર bool sasl_choose_ntlm(સંરચના Curl_easy *data, સંરચના sasl_ctx *sctx)
{
  જો((sctx->enabledmechs & SASL_MECH_NTLM) &&
     Curl_auth_is_ntlm_supported()) {
     અચલ અક્ષર *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;
    અચલ અક્ષર *hostname;

    Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, ખાલી);

    sctx->mech = SASL_MECH_STRING_NTLM;
    sctx->state1 = SASL_NTLM;
    sctx->state2 = SASL_NTLM_TYPE2_સંદેશ;
    sctx->sasl->authused = SASL_MECH_NTLM;

    જો(sctx->sasl->force_ir || data->set.sasl_ir) {
      સંરચના ntlmdata *ntlm = Curl_auth_ntlm_get(sctx->conn, FALSE);
      sctx->result = !ntlm ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_ntlm_type1_message(data, sctx->conn->creds,
                                            service, hostname,
                                            ntlm, &sctx->resp);
    }
    પાછા TRUE;
  }
  પાછા FALSE;
}
#endif /* USE_NTLM */

સ્થિર bool sasl_choose_oauth(સંરચના Curl_easy *data, સંરચના sasl_ctx *sctx)
{
  જો(Curl_creds_has_oauth_bearer(data->state.creds) &&
     (sctx->enabledmechs & SASL_MECH_OAUTHBEARER)) {
    અચલ અક્ષર *hostname;
    પૂર્ણાંક port;
    Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, &port);

    sctx->mech = SASL_MECH_STRING_OAUTHBEARER;
    sctx->state1 = SASL_OAUTH2;
    sctx->state2 = SASL_OAUTH2_RESP;
    sctx->sasl->authused = SASL_MECH_OAUTHBEARER;

    જો(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result =
        Curl_auth_create_oauth_bearer_message(sctx->conn->creds,
                                              hostname, port, &sctx->resp);
    પાછા TRUE;
  }
  પાછા FALSE;
}

સ્થિર bool sasl_choose_oauth2(સંરચના Curl_easy *data, સંરચના sasl_ctx *sctx)
{
  જો(Curl_creds_has_oauth_bearer(sctx->conn->creds) &&
     (sctx->enabledmechs & SASL_MECH_XOAUTH2)) {
    sctx->mech = SASL_MECH_STRING_XOAUTH2;
    sctx->state1 = SASL_OAUTH2;
    sctx->sasl->authused = SASL_MECH_XOAUTH2;

    જો(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result = Curl_auth_create_xoauth_bearer_message(
        sctx->conn->creds, &sctx->resp);
    પાછા TRUE;
  }
  પાછા FALSE;
}

સ્થિર bool sasl_choose_plain(સંરચના Curl_easy *data, સંરચના sasl_ctx *sctx)
{
  જો(sctx->enabledmechs & SASL_MECH_PLAIN) {
    sctx->mech = SASL_MECH_STRING_PLAIN;
    sctx->state1 = SASL_PLAIN;
    sctx->sasl->authused = SASL_MECH_PLAIN;

    જો(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result =
        Curl_auth_create_plain_message(sctx->conn->creds, &sctx->resp);
    પાછા TRUE;
  }
  પાછા FALSE;
}

સ્થિર bool sasl_choose_login(સંરચના Curl_easy *data, સંરચના sasl_ctx *sctx)
{
  જો(sctx->enabledmechs & SASL_MECH_LOGIN) {
    sctx->mech = SASL_MECH_STRING_LOGIN;
    sctx->state1 = SASL_LOGIN;
    sctx->state2 = SASL_LOGIN_PASSWD;
    sctx->sasl->authused = SASL_MECH_LOGIN;

    જો(sctx->sasl->force_ir || data->set.sasl_ir)
      Curl_auth_create_login_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    પાછા TRUE;
  }
  પાછા FALSE;
}

/*
 * Curl_sasl_start()
 *
 * Calculate the required login details for SASL authentication.
 */
CURLcode Curl_sasl_start(સંરચના SASL *sasl, સંરચના Curl_easy *data,
                         bool force_ir, saslprogress *progress)
{
  સંરચના sasl_ctx sctx;

  sasl->force_ir = force_ir;    /* Latch for future use */
  sasl->authused = 0;           /* No mechanism used yet */
  *progress = SASL_IDLE;

  memset(&sctx, 0, કદ(sctx));
  sctx.sasl = sasl;
  sctx.conn = data->conn;
  Curl_bufref_init(&sctx.resp);
  sctx.enabledmechs = sasl->authmechs & sasl->prefmech;
  sctx.state1 = SASL_STOP;
  sctx.state2 = SASL_FINAL;

  /* Calculate the supported authentication mechanism, by decreasing order of
     security, as well as the initial response where appropriate */
  જો(sasl_choose_external(data, &sctx) ||
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

  જો(!sctx.result && sctx.mech) {
    sasl->curmech = sctx.mech;
    જો(Curl_bufref_ptr(&sctx.resp))
      sctx.result = build_message(sasl, &sctx.resp);

    જો(sasl->params->maxirlen &&
       strlen(sctx.mech) + Curl_bufref_len(&sctx.resp) >
         sasl->params->maxirlen)
      Curl_bufref_free(&sctx.resp);

    જો(!sctx.result)
      sctx.result = sasl->params->sendauth(data, sctx.mech, &sctx.resp);

    જો(!sctx.result) {
      *progress = SASL_INPROGRESS;
      sasl_state(sasl, data, Curl_bufref_ptr(&sctx.resp) ?
                 sctx.state2 : sctx.state1);
    }
  }

  Curl_bufref_free(&sctx.resp);
  પાછા sctx.result;
}

/*
 * Curl_sasl_continue()
 *
 * Continue the authentication.
 */
CURLcode Curl_sasl_continue(સંરચના SASL *sasl, સંરચના Curl_easy *data,
                            પૂર્ણાંક code, saslprogress *progress)
{
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;
  saslstate newstate = SASL_FINAL;
  સંરચના bufref resp;
  અચલ અક્ષર *hostname;
  પૂર્ણાંક port;
  સંરચના bufref serverdata;

  Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, &port);
  Curl_bufref_init(&serverdata);
  Curl_bufref_init(&resp);
  *progress = SASL_INPROGRESS;

  જો(sasl->state == SASL_FINAL) {
    જો(code != sasl->params->finalcode)
      result = CURLE_LOGIN_DENIED;
    *progress = SASL_DONE;
    sasl_state(sasl, data, SASL_STOP);
    પાછા result;
  }

  જો(sasl->state != SASL_CANCEL && sasl->state != SASL_OAUTH2_RESP &&
     code != sasl->params->contcode) {
    *progress = SASL_DONE;
    sasl_state(sasl, data, SASL_STOP);
    પાછા CURLE_LOGIN_DENIED;
  }

  પસંદગી(sasl->state) {
  સ્થિતિ SASL_STOP:
    *progress = SASL_DONE;
    પાછા result;
  સ્થિતિ SASL_PLAIN:
    result = Curl_auth_create_plain_message(conn->creds, &resp);
    તોડો;
  સ્થિતિ SASL_LOGIN:
    Curl_auth_create_login_message(Curl_creds_user(conn->creds), &resp);
    newstate = SASL_LOGIN_PASSWD;
    તોડો;
  સ્થિતિ SASL_LOGIN_PASSWD:
    Curl_auth_create_login_message(Curl_creds_passwd(conn->creds), &resp);
    તોડો;
  સ્થિતિ SASL_EXTERNAL:
    Curl_auth_create_external_message(Curl_creds_user(conn->creds), &resp);
    તોડો;
#ifdef USE_GSASL
  સ્થિતિ SASL_GSASL:
    result = get_server_message(sasl, data, &serverdata);
    જો(!result) {
      સંરચના gsasldata *gsasl = Curl_auth_gsasl_get(conn);
      result = !gsasl ? CURLE_OUT_OF_MEMORY :
        Curl_auth_gsasl_token(data, &serverdata, gsasl, &resp);
    }
    જો(!result && Curl_bufref_len(&resp) > 0)
      newstate = SASL_GSASL;
    તોડો;
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  સ્થિતિ SASL_CRAMMD5:
    result = get_server_message(sasl, data, &serverdata);
    જો(!result)
      result = Curl_auth_create_cram_md5_message(&serverdata, conn->creds,
                                                 &resp);
    તોડો;
  સ્થિતિ SASL_DIGESTMD5:
    result = get_server_message(sasl, data, &serverdata);
    જો(!result)
      result = Curl_auth_create_digest_md5_message(data, &serverdata,
                                                   conn->creds,
                                                   sasl->params->service,
                                                   &resp);
    જો(!result && (sasl->params->flags & SASL_FLAG_BASE64))
      newstate = SASL_DIGESTMD5_RESP;
    તોડો;
  સ્થિતિ SASL_DIGESTMD5_RESP:
    /* Keep response NULL to output an empty line. */
    તોડો;
#endif

#ifdef USE_NTLM
  સ્થિતિ SASL_NTLM: {
    /* Create the type-1 message */
    સંરચના ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    result = !ntlm ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_ntlm_type1_message(data, conn->creds,
                                          sasl->params->service, hostname,
                                          ntlm, &resp);
    newstate = SASL_NTLM_TYPE2_સંદેશ;
    તોડો;
  }
  સ્થિતિ SASL_NTLM_TYPE2_સંદેશ: {
    /* Decode the type-2 message */
    સંરચના ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    result = !ntlm ? CURLE_OUT_OF_MEMORY :
      get_server_message(sasl, data, &serverdata);
    જો(!result)
      result = Curl_auth_decode_ntlm_type2_message(data, &serverdata, ntlm);
    જો(!result)
      result = Curl_auth_create_ntlm_type3_message(data, conn->creds,
                                                   ntlm, &resp);
    તોડો;
  }
#endif

#ifdef USE_KERBEROS5
  સ્થિતિ SASL_GSSAPI: {
    સંરચના kerberos5data *krb5 = Curl_auth_krb5_get(conn);
    result = !krb5 ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_gssapi_user_message(data, conn->creds,
                                           sasl->params->service,
                                           conn->origin->hostname,
                                           (bool)sasl->mutual_auth, ખાલી,
                                           krb5, &resp);
    newstate = SASL_GSSAPI_TOKEN;
    તોડો;
  }
  સ્થિતિ SASL_GSSAPI_TOKEN:
    result = get_server_message(sasl, data, &serverdata);
    જો(!result) {
      સંરચના kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      જો(!krb5)
        result = CURLE_OUT_OF_MEMORY;
      નહીંતર જો(sasl->mutual_auth) {
        /* Decode the user token challenge and create the optional response
           message */
        result = Curl_auth_create_gssapi_user_message(data, ખાલી,
                                                      ખાલી, ખાલી,
                                                      (bool)sasl->mutual_auth,
                                                      &serverdata,
                                                      krb5, &resp);
        newstate = SASL_GSSAPI_NO_DATA;
      }
      નહીંતર
        /* Decode the security challenge and create the response message */
        result = Curl_auth_create_gssapi_security_message(
          data, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    તોડો;
  સ્થિતિ SASL_GSSAPI_NO_DATA:
    /* Decode the security challenge and create the response message */
    result = get_server_message(sasl, data, &serverdata);
    જો(!result) {
      સંરચના kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      જો(!krb5)
        result = CURLE_OUT_OF_MEMORY;
      નહીંતર
        result = Curl_auth_create_gssapi_security_message(
          data, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    તોડો;
#endif

  સ્થિતિ SASL_OAUTH2:
    /* Create the authorization message */
    જો(sasl->authused == SASL_MECH_OAUTHBEARER) {
      result = Curl_auth_create_oauth_bearer_message(conn->creds,
                                                     hostname,
                                                     port,
                                                     &resp);

      /* Failures maybe sent by the server as continuations for OAUTHBEARER */
      newstate = SASL_OAUTH2_RESP;
    }
    નહીંતર
      result = Curl_auth_create_xoauth_bearer_message(conn->creds,
                                                      &resp);
    તોડો;

  સ્થિતિ SASL_OAUTH2_RESP:
    /* The continuation is optional so check the response code */
    જો(code == sasl->params->finalcode) {
      /* Final response was received so we are done */
      *progress = SASL_DONE;
      sasl_state(sasl, data, SASL_STOP);
      પાછા result;
    }
    નહીંતર જો(code == sasl->params->contcode) {
      /* Acknowledge the continuation by sending a 0x01 response. */
      Curl_bufref_set(&resp, "\x01", 1, ખાલી);
      તોડો;
    }
    નહીંતર {
      *progress = SASL_DONE;
      sasl_state(sasl, data, SASL_STOP);
      પાછા CURLE_LOGIN_DENIED;
    }

  સ્થિતિ SASL_CANCEL:
    /* Remove the offending mechanism from the supported list */
    sasl->authmechs &= (unsigned short)~sasl->authused;
    sasl->authused = SASL_AUTH_NONE;
    sasl->curmech = ખાલી;

    /* Start an alternative SASL authentication */
    પાછા Curl_sasl_start(sasl, data, (bool)sasl->force_ir, progress);
  મૂળભૂત:
    failf(data, "Unsupported SASL authentication mechanism");
    result = CURLE_UNSUPPORTED_PROTOCOL;  /* Should not happen */
    તોડો;
  }

  Curl_bufref_free(&serverdata);

  પસંદગી(result) {
  સ્થિતિ CURLE_BAD_CONTENT_ENCODING:
    /* Cancel dialog */
    result = sasl->params->cancelauth(data, sasl->curmech);
    newstate = SASL_CANCEL;
    તોડો;
  સ્થિતિ CURLE_OK:
    result = build_message(sasl, &resp);
    જો(!result)
      result = sasl->params->contauth(data, sasl->curmech, &resp);
    તોડો;
  મૂળભૂત:
    newstate = SASL_STOP;    /* Stop on error */
    *progress = SASL_DONE;
    તોડો;
  }

  Curl_bufref_free(&resp);

  sasl_state(sasl, data, newstate);

  પાછા result;
}

#ifdef CURLVERBOSE
સ્થિર શૂન્ય sasl_unchosen(સંરચના Curl_easy *data, unsigned short mech,
                          unsigned short enabledmechs,
                          bool built_in, bool platform,
                          અચલ અક્ષર *param_missing)
{
  અચલ અક્ષર *mname = ખાલી;
  size_t i;

  જો(!(enabledmechs & mech))
    પાછા;

  માટે(i = 0; mechtable[i].name; ++i) {
    જો(mechtable[i].bit == mech) {
      mname = mechtable[i].name;
      તોડો;
    }
  }
  જો(!mname)  /* should not happen */
    પાછા;
  જો(!built_in)
    infof(data, "SASL: %s not builtin", mname);
  નહીંતર જો(!platform)
    infof(data, "SASL: %s not supported by the platform/libraries", mname);
  નહીંતર {
    જો(param_missing)
      infof(data, "SASL: %s is missing %s", mname, param_missing);
    જો(!Curl_creds_has_user(data->conn->creds))
      infof(data, "SASL: %s is missing username", mname);
  }
}
#endif /* CURLVERBOSE */

CURLcode Curl_sasl_is_blocked(સંરચના SASL *sasl, સંરચના Curl_easy *data)
{
#ifdef CURLVERBOSE
#ifdef USE_KERBEROS5
#define CURL_SASL_KERBEROS5   TRUE
#નહીંતર
#define CURL_SASL_KERBEROS5   FALSE
#endif
#ifdef USE_GSASL
#define CURL_SASL_GASL        TRUE
#નહીંતર
#define CURL_SASL_GASL        FALSE
#endif
#ifdef CURL_DISABLE_DIGEST_AUTH
#define CURL_SASL_DIGEST      TRUE
#નહીંતર
#define CURL_SASL_DIGEST      FALSE
#endif
#ifndef USE_NTLM
#define CURL_SASL_NTLM        TRUE
#નહીંતર
#define CURL_SASL_NTLM        FALSE
#endif
  /* Failing SASL authentication is a pain. Give a helping hand if
   * we were unable to select an AUTH mechanism.
   * `sasl->authmechs` are mechanisms offered by the peer
   * `sasl->prefmech`  are mechanisms preferred by us */
  unsigned short enabledmechs = sasl->authmechs & sasl->prefmech;

  જો(!sasl->authmechs)
    infof(data, "SASL: no auth mechanism was offered or recognized");
  નહીંતર જો(!enabledmechs)
    infof(data, "SASL: no overlap between offered and configured "
          "auth mechanisms");
  નહીંતર {
    infof(data, "SASL: no auth mechanism offered could be selected");
    જો((enabledmechs & SASL_MECH_EXTERNAL) &&
       Curl_creds_has_passwd(data->conn->creds))
      infof(data, "SASL: auth EXTERNAL not chosen with password");
    sasl_unchosen(data, SASL_MECH_GSSAPI, enabledmechs,
                  CURL_SASL_KERBEROS5, Curl_auth_is_gssapi_supported(), ખાલી);
    sasl_unchosen(data, SASL_MECH_SCRAM_SHA_256, enabledmechs,
                  CURL_SASL_GASL, FALSE, ખાલી);
    sasl_unchosen(data, SASL_MECH_SCRAM_SHA_1, enabledmechs,
                  CURL_SASL_GASL, FALSE, ખાલી);
    sasl_unchosen(data, SASL_MECH_DIGEST_MD5, enabledmechs,
                  CURL_SASL_DIGEST, Curl_auth_is_digest_supported(), ખાલી);
    sasl_unchosen(data, SASL_MECH_CRAM_MD5, enabledmechs,
                  CURL_SASL_DIGEST, TRUE, ખાલી);
    sasl_unchosen(data, SASL_MECH_NTLM, enabledmechs,
                  CURL_SASL_NTLM, Curl_auth_is_ntlm_supported(), ખાલી);
    sasl_unchosen(data, SASL_MECH_OAUTHBEARER, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(data->conn->creds) ?
                  ખાલી : "CURLOPT_XOAUTH2_BEARER");
    sasl_unchosen(data, SASL_MECH_XOAUTH2, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(data->conn->creds) ?
                  ખાલી : "CURLOPT_XOAUTH2_BEARER");
  }
#endif /* CURLVERBOSE */
  (શૂન્ય)sasl;
  (શૂન્ય)data;
  પાછા CURLE_LOGIN_DENIED;
}

#endif /* protocols are enabled that use SASL */
