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
#include "curl_setup.h"

#якщо !defined(CURL_DISABLE_IMAP) || !defined(CURL_DISABLE_SMTP) || \
  !defined(CURL_DISABLE_POP3) ||                                  \
  (!defined(CURL_DISABLE_LDAP) && defined(USE_OPENLDAP))

#include "urldata.h"
#include "curlx/base64.h"
#include "vauth/vauth.h"
#include "cfilters.h"
#include "curl_sasl.h"
#include "curl_trc.h"

/* Supported mechanisms */
static const struct {
  const char    *name;  /* Name */
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
unsigned short Curl_sasl_decode_mech(const char *ptr, size_t maxlen,
                                     size_t *len)
{
  unsigned ціле i;
  char c;

  для(i = 0; mechtable[i].name; i++) {
    якщо(maxlen >= mechtable[i].len &&
       curl_strnequal(ptr, mechtable[i].name, mechtable[i].len)) {
      якщо(len)
        *len = mechtable[i].len;

      якщо(maxlen == mechtable[i].len)
        повернути mechtable[i].bit;

      c = ptr[mechtable[i].len];
      якщо(!ISUPPER(c) && !ISDIGIT(c) && c != '-' && c != '_')
        повернути mechtable[i].bit;
    }
  }

  повернути 0;
}

/*
 * Curl_sasl_parse_url_auth_option()
 *
 * Parse the URL login options.
 */
CURLcode Curl_sasl_parse_url_auth_option(struct SASL *sasl,
                                         const char *value, size_t len)
{
  CURLcode result = CURLE_OK;
  size_t mechlen;

  якщо(!len)
    повернути CURLE_URL_MALFORMAT;

  якщо(sasl->resetprefs) {
    sasl->resetprefs = FALSE;
    sasl->prefmech = SASL_AUTH_NONE;
  }

  якщо(!strncmp(value, "*", len))
    sasl->prefmech = SASL_AUTH_DEFAULT;
  інакше {
    unsigned short mechbit = Curl_sasl_decode_mech(value, len, &mechlen);
    якщо(mechbit && mechlen == len)
      sasl->prefmech |= mechbit;
    інакше
      result = CURLE_URL_MALFORMAT;
  }

  повернути result;
}

/*
 * Curl_sasl_init()
 *
 * Initializes the SASL structure.
 */
void Curl_sasl_init(struct SASL *sasl, struct Curl_easy *data,
                    const struct SASLproto *params)
{
  uint32_t auth = data->set.httpauth;

  sasl->params = params;           /* Set protocol dependent parameters */
  sasl->state = SASL_STOP;         /* Not yet running */
  sasl->curmech = NULL;            /* No mechanism yet. */
  sasl->authmechs = SASL_AUTH_NONE; /* No known authentication mechanism yet */
  sasl->prefmech = params->defmechs; /* Default preferred mechanisms */
  sasl->authused = SASL_AUTH_NONE; /* The authentication mechanism used */
  sasl->resetprefs = TRUE;         /* Reset prefmech upon AUTH parsing. */
  sasl->mutual_auth = FALSE;       /* No mutual authentication (GSSAPI only) */
  sasl->force_ir = FALSE;          /* Respect external option */

  якщо(auth != CURLAUTH_BASIC) {
    unsigned short mechs = SASL_AUTH_NONE;

    /* If some usable http authentication options have been set, determine
       new defaults from them. */
    якщо(auth & CURLAUTH_BASIC)
      mechs |= SASL_MECH_PLAIN | SASL_MECH_LOGIN;
    якщо(auth & CURLAUTH_DIGEST)
      mechs |= SASL_MECH_DIGEST_MD5;
    якщо(auth & CURLAUTH_NTLM)
      mechs |= SASL_MECH_NTLM;
    якщо(auth & CURLAUTH_BEARER)
      mechs |= SASL_MECH_OAUTHBEARER | SASL_MECH_XOAUTH2;
    якщо(auth & CURLAUTH_GSSAPI)
      mechs |= SASL_MECH_GSSAPI;

    якщо(mechs != SASL_AUTH_NONE)
      sasl->prefmech = mechs;
  }
}

/*
 * sasl_state()
 *
 * This is the ONLY way to change SASL state!
 */
static void sasl_state(struct SASL *sasl, struct Curl_easy *data,
                       saslstate newstate)
{
#якщо defined(DEBUGBUILD) && defined(CURLVERBOSE)
  /* for debug purposes */
  static const char * const names[] = {
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

  якщо(sasl->state != newstate)
    infof(data, "SASL %p state change from %s to %s",
          (void *)sasl, names[sasl->state], names[newstate]);
#інакше
  (void)data;
#endif

  sasl->state = newstate;
}

#якщо defined(USE_NTLM) || defined(USE_GSASL) || defined(USE_KERBEROS5) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)
/* Get the SASL server message and convert it to binary. */
static CURLcode get_server_message(struct SASL *sasl, struct Curl_easy *data,
                                   struct bufref *out)
{
  CURLcode result = CURLE_OK;

  result = sasl->params->getmessage(data, out);
  якщо(!result && (sasl->params->flags & SASL_FLAG_BASE64)) {
    const char *serverdata = Curl_bufref_ptr(out);

    якщо(!*serverdata)
      Curl_bufref_set(out, NULL, 0, NULL);
    інакше {
      unsigned char *msg;
      size_t msglen;

      result = curlx_base64_decode(serverdata, &msg, &msglen);
      якщо(!result)
        Curl_bufref_set(out, msg, msglen, curl_free);
    }
  }
  повернути result;
}
#endif

/* Encode the outgoing SASL message. */
static CURLcode build_message(struct SASL *sasl, struct bufref *msg)
{
  CURLcode result = CURLE_OK;

  якщо(sasl->params->flags & SASL_FLAG_BASE64) {
    якщо(!Curl_bufref_ptr(msg))                   /* Empty message. */
      Curl_bufref_set(msg, "", 0, NULL);
    інакше якщо(Curl_bufref_len(msg)) {
      char *base64;
      size_t base64len;

      result = curlx_base64_encode(Curl_bufref_uptr(msg),
                                   Curl_bufref_len(msg), &base64, &base64len);
      якщо(!result)
        Curl_bufref_set(msg, base64, base64len, curl_free);
    }
  }

  повернути result;
}

/*
 * Curl_sasl_can_authenticate()
 *
 * Check if we have enough auth data and capabilities to authenticate.
 */
bool Curl_sasl_can_authenticate(struct SASL *sasl, struct Curl_easy *data)
{
  /* Have credentials been provided? */
  якщо(data->conn->creds)
    повернути TRUE;

  /* EXTERNAL can authenticate without a username and/or password */
  якщо(sasl->authmechs & sasl->prefmech & SASL_MECH_EXTERNAL)
    повернути TRUE;

  повернути FALSE;
}

struct sasl_ctx {
  struct SASL *sasl;
  struct connectdata *conn;
  unsigned short enabledmechs;
  const char *mech;
  saslstate state1;
  saslstate state2;
  struct bufref resp;
  CURLcode result;
};

static bool sasl_choose_external(struct Curl_easy *data, struct sasl_ctx *sctx)
{
  якщо((sctx->enabledmechs & SASL_MECH_EXTERNAL) &&
     !Curl_creds_has_passwd(sctx->conn->creds)) {
    sctx->mech = SASL_MECH_STRING_EXTERNAL;
    sctx->state1 = SASL_EXTERNAL;
    sctx->sasl->authused = SASL_MECH_EXTERNAL;

    якщо(sctx->sasl->force_ir || data->set.sasl_ir)
      Curl_auth_create_external_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    повернути TRUE;
  }
  повернути FALSE;
}

#ifdef USE_KERBEROS5
static bool sasl_choose_krb5(struct Curl_easy *data, struct sasl_ctx *sctx)
{
  якщо((sctx->enabledmechs & SASL_MECH_GSSAPI) &&
     Curl_auth_is_gssapi_supported() &&
     Curl_auth_user_contains_domain(sctx->conn->creds)) {
    const char *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;

    sctx->sasl->mutual_auth = FALSE;
    sctx->mech = SASL_MECH_STRING_GSSAPI;
    sctx->state1 = SASL_GSSAPI;
    sctx->state2 = SASL_GSSAPI_TOKEN;
    sctx->sasl->authused = SASL_MECH_GSSAPI;

    якщо(sctx->sasl->force_ir || data->set.sasl_ir) {
      struct kerberos5data *krb5 = Curl_auth_krb5_get(sctx->conn);
      sctx->result = !krb5 ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_gssapi_user_message(data, sctx->conn->creds,
                                             service,
                                             sctx->conn->origin->hostname,
                                             (bool)sctx->sasl->mutual_auth,
                                             NULL, krb5, &sctx->resp);
    }
    повернути TRUE;
  }
  повернути FALSE;
}
#endif /* USE_KERBEROS5 */

#ifdef USE_GSASL
static bool sasl_choose_gsasl(struct Curl_easy *data, struct sasl_ctx *sctx)
{
  struct gsasldata *gsasl;
  struct bufref nullmsg;

  якщо((sctx->enabledmechs &
      (SASL_MECH_SCRAM_SHA_256 | SASL_MECH_SCRAM_SHA_1))) {
    gsasl = Curl_auth_gsasl_get(sctx->conn);
    якщо(!gsasl) {
      sctx->result = CURLE_OUT_OF_MEMORY;
      повернути TRUE; /* attempted, but failed */
    }

    якщо((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_256) &&
       Curl_auth_gsasl_is_supported(data, SASL_MECH_STRING_SCRAM_SHA_256,
                                    gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_256;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_256;
    }
    інакше якщо((sctx->enabledmechs & SASL_MECH_SCRAM_SHA_1) &&
            Curl_auth_gsasl_is_supported(data, SASL_MECH_STRING_SCRAM_SHA_1,
                                         gsasl)) {
      sctx->mech = SASL_MECH_STRING_SCRAM_SHA_1;
      sctx->sasl->authused = SASL_MECH_SCRAM_SHA_1;
    }
    інакше
      повернути FALSE;

    Curl_bufref_init(&nullmsg);
    sctx->state1 = SASL_GSASL;
    sctx->state2 = SASL_GSASL;
    sctx->result = Curl_auth_gsasl_start(data, sctx->conn->creds, gsasl);
    якщо(!sctx->result && (sctx->sasl->force_ir || data->set.sasl_ir))
      sctx->result = Curl_auth_gsasl_token(data, &nullmsg, gsasl, &sctx->resp);
    повернути TRUE;
  }
  повернути FALSE;
}

#endif /* USE_GSASL */

#ifndef CURL_DISABLE_DIGEST_AUTH
static bool sasl_choose_digest(struct Curl_easy *data, struct sasl_ctx *sctx)
{
  (void)data;
  якщо((sctx->enabledmechs & SASL_MECH_DIGEST_MD5) &&
     Curl_auth_is_digest_supported()) {
    sctx->mech = SASL_MECH_STRING_DIGEST_MD5;
    sctx->state1 = SASL_DIGESTMD5;
    sctx->sasl->authused = SASL_MECH_DIGEST_MD5;
    повернути TRUE;
  }
  інакше якщо(sctx->enabledmechs & SASL_MECH_CRAM_MD5) {
    sctx->mech = SASL_MECH_STRING_CRAM_MD5;
    sctx->state1 = SASL_CRAMMD5;
    sctx->sasl->authused = SASL_MECH_CRAM_MD5;
    повернути TRUE;
  }
  повернути FALSE;
}
#endif /* !CURL_DISABLE_DIGEST_AUTH */

#ifdef USE_NTLM
static bool sasl_choose_ntlm(struct Curl_easy *data, struct sasl_ctx *sctx)
{
  якщо((sctx->enabledmechs & SASL_MECH_NTLM) &&
     Curl_auth_is_ntlm_supported()) {
     const char *service = Curl_creds_has_sasl_service(sctx->conn->creds) ?
      Curl_creds_sasl_service(sctx->conn->creds) : sctx->sasl->params->service;
    const char *hostname;

    Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, NULL);

    sctx->mech = SASL_MECH_STRING_NTLM;
    sctx->state1 = SASL_NTLM;
    sctx->state2 = SASL_NTLM_TYPE2MSG;
    sctx->sasl->authused = SASL_MECH_NTLM;

    якщо(sctx->sasl->force_ir || data->set.sasl_ir) {
      struct ntlmdata *ntlm = Curl_auth_ntlm_get(sctx->conn, FALSE);
      sctx->result = !ntlm ? CURLE_OUT_OF_MEMORY :
        Curl_auth_create_ntlm_type1_message(data, sctx->conn->creds,
                                            service, hostname,
                                            ntlm, &sctx->resp);
    }
    повернути TRUE;
  }
  повернути FALSE;
}
#endif /* USE_NTLM */

static bool sasl_choose_oauth(struct Curl_easy *data, struct sasl_ctx *sctx)
{
  якщо(Curl_creds_has_oauth_bearer(data->state.creds) &&
     (sctx->enabledmechs & SASL_MECH_OAUTHBEARER)) {
    const char *hostname;
    ціле port;
    Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, &port);

    sctx->mech = SASL_MECH_STRING_OAUTHBEARER;
    sctx->state1 = SASL_OAUTH2;
    sctx->state2 = SASL_OAUTH2_RESP;
    sctx->sasl->authused = SASL_MECH_OAUTHBEARER;

    якщо(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result =
        Curl_auth_create_oauth_bearer_message(sctx->conn->creds,
                                              hostname, port, &sctx->resp);
    повернути TRUE;
  }
  повернути FALSE;
}

static bool sasl_choose_oauth2(struct Curl_easy *data, struct sasl_ctx *sctx)
{
  якщо(Curl_creds_has_oauth_bearer(sctx->conn->creds) &&
     (sctx->enabledmechs & SASL_MECH_XOAUTH2)) {
    sctx->mech = SASL_MECH_STRING_XOAUTH2;
    sctx->state1 = SASL_OAUTH2;
    sctx->sasl->authused = SASL_MECH_XOAUTH2;

    якщо(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result = Curl_auth_create_xoauth_bearer_message(
        sctx->conn->creds, &sctx->resp);
    повернути TRUE;
  }
  повернути FALSE;
}

static bool sasl_choose_plain(struct Curl_easy *data, struct sasl_ctx *sctx)
{
  якщо(sctx->enabledmechs & SASL_MECH_PLAIN) {
    sctx->mech = SASL_MECH_STRING_PLAIN;
    sctx->state1 = SASL_PLAIN;
    sctx->sasl->authused = SASL_MECH_PLAIN;

    якщо(sctx->sasl->force_ir || data->set.sasl_ir)
      sctx->result =
        Curl_auth_create_plain_message(sctx->conn->creds, &sctx->resp);
    повернути TRUE;
  }
  повернути FALSE;
}

static bool sasl_choose_login(struct Curl_easy *data, struct sasl_ctx *sctx)
{
  якщо(sctx->enabledmechs & SASL_MECH_LOGIN) {
    sctx->mech = SASL_MECH_STRING_LOGIN;
    sctx->state1 = SASL_LOGIN;
    sctx->state2 = SASL_LOGIN_PASSWD;
    sctx->sasl->authused = SASL_MECH_LOGIN;

    якщо(sctx->sasl->force_ir || data->set.sasl_ir)
      Curl_auth_create_login_message(
        Curl_creds_user(sctx->conn->creds), &sctx->resp);
    повернути TRUE;
  }
  повернути FALSE;
}

/*
 * Curl_sasl_start()
 *
 * Calculate the required login details for SASL authentication.
 */
CURLcode Curl_sasl_start(struct SASL *sasl, struct Curl_easy *data,
                         bool force_ir, saslprogress *progress)
{
  struct sasl_ctx sctx;

  sasl->force_ir = force_ir;    /* Latch for future use */
  sasl->authused = 0;           /* No mechanism used yet */
  *progress = SASL_IDLE;

  memset(&sctx, 0, sizeof(sctx));
  sctx.sasl = sasl;
  sctx.conn = data->conn;
  Curl_bufref_init(&sctx.resp);
  sctx.enabledmechs = sasl->authmechs & sasl->prefmech;
  sctx.state1 = SASL_STOP;
  sctx.state2 = SASL_FINAL;

  /* Calculate the supported authentication mechanism, by decreasing order of
     security, as well as the initial response where appropriate */
  якщо(sasl_choose_external(data, &sctx) ||
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

  якщо(!sctx.result && sctx.mech) {
    sasl->curmech = sctx.mech;
    якщо(Curl_bufref_ptr(&sctx.resp))
      sctx.result = build_message(sasl, &sctx.resp);

    якщо(sasl->params->maxirlen &&
       strlen(sctx.mech) + Curl_bufref_len(&sctx.resp) >
         sasl->params->maxirlen)
      Curl_bufref_free(&sctx.resp);

    якщо(!sctx.result)
      sctx.result = sasl->params->sendauth(data, sctx.mech, &sctx.resp);

    якщо(!sctx.result) {
      *progress = SASL_INPROGRESS;
      sasl_state(sasl, data, Curl_bufref_ptr(&sctx.resp) ?
                 sctx.state2 : sctx.state1);
    }
  }

  Curl_bufref_free(&sctx.resp);
  повернути sctx.result;
}

/*
 * Curl_sasl_continue()
 *
 * Continue the authentication.
 */
CURLcode Curl_sasl_continue(struct SASL *sasl, struct Curl_easy *data,
                            ціле code, saslprogress *progress)
{
  CURLcode result = CURLE_OK;
  struct connectdata *conn = data->conn;
  saslstate newstate = SASL_FINAL;
  struct bufref resp;
  const char *hostname;
  ціле port;
  struct bufref serverdata;

  Curl_conn_get_current_host(data, FIRSTSOCKET, &hostname, &port);
  Curl_bufref_init(&serverdata);
  Curl_bufref_init(&resp);
  *progress = SASL_INPROGRESS;

  якщо(sasl->state == SASL_FINAL) {
    якщо(code != sasl->params->finalcode)
      result = CURLE_LOGIN_DENIED;
    *progress = SASL_DONE;
    sasl_state(sasl, data, SASL_STOP);
    повернути result;
  }

  якщо(sasl->state != SASL_CANCEL && sasl->state != SASL_OAUTH2_RESP &&
     code != sasl->params->contcode) {
    *progress = SASL_DONE;
    sasl_state(sasl, data, SASL_STOP);
    повернути CURLE_LOGIN_DENIED;
  }

  switch(sasl->state) {
  case SASL_STOP:
    *progress = SASL_DONE;
    повернути result;
  case SASL_PLAIN:
    result = Curl_auth_create_plain_message(conn->creds, &resp);
    break;
  case SASL_LOGIN:
    Curl_auth_create_login_message(Curl_creds_user(conn->creds), &resp);
    newstate = SASL_LOGIN_PASSWD;
    break;
  case SASL_LOGIN_PASSWD:
    Curl_auth_create_login_message(Curl_creds_passwd(conn->creds), &resp);
    break;
  case SASL_EXTERNAL:
    Curl_auth_create_external_message(Curl_creds_user(conn->creds), &resp);
    break;
#ifdef USE_GSASL
  case SASL_GSASL:
    result = get_server_message(sasl, data, &serverdata);
    якщо(!result) {
      struct gsasldata *gsasl = Curl_auth_gsasl_get(conn);
      result = !gsasl ? CURLE_OUT_OF_MEMORY :
        Curl_auth_gsasl_token(data, &serverdata, gsasl, &resp);
    }
    якщо(!result && Curl_bufref_len(&resp) > 0)
      newstate = SASL_GSASL;
    break;
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  case SASL_CRAMMD5:
    result = get_server_message(sasl, data, &serverdata);
    якщо(!result)
      result = Curl_auth_create_cram_md5_message(&serverdata, conn->creds,
                                                 &resp);
    break;
  case SASL_DIGESTMD5:
    result = get_server_message(sasl, data, &serverdata);
    якщо(!result)
      result = Curl_auth_create_digest_md5_message(data, &serverdata,
                                                   conn->creds,
                                                   sasl->params->service,
                                                   &resp);
    якщо(!result && (sasl->params->flags & SASL_FLAG_BASE64))
      newstate = SASL_DIGESTMD5_RESP;
    break;
  case SASL_DIGESTMD5_RESP:
    /* Keep response NULL to output an empty line. */
    break;
#endif

#ifdef USE_NTLM
  case SASL_NTLM: {
    /* Create the type-1 message */
    struct ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    result = !ntlm ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_ntlm_type1_message(data, conn->creds,
                                          sasl->params->service, hostname,
                                          ntlm, &resp);
    newstate = SASL_NTLM_TYPE2MSG;
    break;
  }
  case SASL_NTLM_TYPE2MSG: {
    /* Decode the type-2 message */
    struct ntlmdata *ntlm = Curl_auth_ntlm_get(conn, FALSE);
    result = !ntlm ? CURLE_OUT_OF_MEMORY :
      get_server_message(sasl, data, &serverdata);
    якщо(!result)
      result = Curl_auth_decode_ntlm_type2_message(data, &serverdata, ntlm);
    якщо(!result)
      result = Curl_auth_create_ntlm_type3_message(data, conn->creds,
                                                   ntlm, &resp);
    break;
  }
#endif

#ifdef USE_KERBEROS5
  case SASL_GSSAPI: {
    struct kerberos5data *krb5 = Curl_auth_krb5_get(conn);
    result = !krb5 ? CURLE_OUT_OF_MEMORY :
      Curl_auth_create_gssapi_user_message(data, conn->creds,
                                           sasl->params->service,
                                           conn->origin->hostname,
                                           (bool)sasl->mutual_auth, NULL,
                                           krb5, &resp);
    newstate = SASL_GSSAPI_TOKEN;
    break;
  }
  case SASL_GSSAPI_TOKEN:
    result = get_server_message(sasl, data, &serverdata);
    якщо(!result) {
      struct kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      якщо(!krb5)
        result = CURLE_OUT_OF_MEMORY;
      інакше якщо(sasl->mutual_auth) {
        /* Decode the user token challenge and create the optional response
           message */
        result = Curl_auth_create_gssapi_user_message(data, NULL,
                                                      NULL, NULL,
                                                      (bool)sasl->mutual_auth,
                                                      &serverdata,
                                                      krb5, &resp);
        newstate = SASL_GSSAPI_NO_DATA;
      }
      інакше
        /* Decode the security challenge and create the response message */
        result = Curl_auth_create_gssapi_security_message(
          data, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    break;
  case SASL_GSSAPI_NO_DATA:
    /* Decode the security challenge and create the response message */
    result = get_server_message(sasl, data, &serverdata);
    якщо(!result) {
      struct kerberos5data *krb5 = Curl_auth_krb5_get(conn);
      якщо(!krb5)
        result = CURLE_OUT_OF_MEMORY;
      інакше
        result = Curl_auth_create_gssapi_security_message(
          data, Curl_creds_sasl_authzid(conn->creds), &serverdata,
          krb5, &resp);
    }
    break;
#endif

  case SASL_OAUTH2:
    /* Create the authorization message */
    якщо(sasl->authused == SASL_MECH_OAUTHBEARER) {
      result = Curl_auth_create_oauth_bearer_message(conn->creds,
                                                     hostname,
                                                     port,
                                                     &resp);

      /* Failures maybe sent by the server as continuations for OAUTHBEARER */
      newstate = SASL_OAUTH2_RESP;
    }
    інакше
      result = Curl_auth_create_xoauth_bearer_message(conn->creds,
                                                      &resp);
    break;

  case SASL_OAUTH2_RESP:
    /* The continuation is optional so check the response code */
    якщо(code == sasl->params->finalcode) {
      /* Final response was received so we are done */
      *progress = SASL_DONE;
      sasl_state(sasl, data, SASL_STOP);
      повернути result;
    }
    інакше якщо(code == sasl->params->contcode) {
      /* Acknowledge the continuation by sending a 0x01 response. */
      Curl_bufref_set(&resp, "\x01", 1, NULL);
      break;
    }
    інакше {
      *progress = SASL_DONE;
      sasl_state(sasl, data, SASL_STOP);
      повернути CURLE_LOGIN_DENIED;
    }

  case SASL_CANCEL:
    /* Remove the offending mechanism from the supported list */
    sasl->authmechs &= (unsigned short)~sasl->authused;
    sasl->authused = SASL_AUTH_NONE;
    sasl->curmech = NULL;

    /* Start an alternative SASL authentication */
    повернути Curl_sasl_start(sasl, data, (bool)sasl->force_ir, progress);
  default:
    failf(data, "Unsupported SASL authentication mechanism");
    result = CURLE_UNSUPPORTED_PROTOCOL;  /* Should not happen */
    break;
  }

  Curl_bufref_free(&serverdata);

  switch(result) {
  case CURLE_BAD_CONTENT_ENCODING:
    /* Cancel dialog */
    result = sasl->params->cancelauth(data, sasl->curmech);
    newstate = SASL_CANCEL;
    break;
  case CURLE_OK:
    result = build_message(sasl, &resp);
    якщо(!result)
      result = sasl->params->contauth(data, sasl->curmech, &resp);
    break;
  default:
    newstate = SASL_STOP;    /* Stop on error */
    *progress = SASL_DONE;
    break;
  }

  Curl_bufref_free(&resp);

  sasl_state(sasl, data, newstate);

  повернути result;
}

#ifdef CURLVERBOSE
static void sasl_unchosen(struct Curl_easy *data, unsigned short mech,
                          unsigned short enabledmechs,
                          bool built_in, bool platform,
                          const char *param_missing)
{
  const char *mname = NULL;
  size_t i;

  якщо(!(enabledmechs & mech))
    повернути;

  для(i = 0; mechtable[i].name; ++i) {
    якщо(mechtable[i].bit == mech) {
      mname = mechtable[i].name;
      break;
    }
  }
  якщо(!mname)  /* should not happen */
    повернути;
  якщо(!built_in)
    infof(data, "SASL: %s not builtin", mname);
  інакше якщо(!platform)
    infof(data, "SASL: %s not supported by the platform/libraries", mname);
  інакше {
    якщо(param_missing)
      infof(data, "SASL: %s is missing %s", mname, param_missing);
    якщо(!Curl_creds_has_user(data->conn->creds))
      infof(data, "SASL: %s is missing username", mname);
  }
}
#endif /* CURLVERBOSE */

CURLcode Curl_sasl_is_blocked(struct SASL *sasl, struct Curl_easy *data)
{
#ifdef CURLVERBOSE
#ifdef USE_KERBEROS5
#define CURL_SASL_KERBEROS5   TRUE
#інакше
#define CURL_SASL_KERBEROS5   FALSE
#endif
#ifdef USE_GSASL
#define CURL_SASL_GASL        TRUE
#інакше
#define CURL_SASL_GASL        FALSE
#endif
#ifdef CURL_DISABLE_DIGEST_AUTH
#define CURL_SASL_DIGEST      TRUE
#інакше
#define CURL_SASL_DIGEST      FALSE
#endif
#ifndef USE_NTLM
#define CURL_SASL_NTLM        TRUE
#інакше
#define CURL_SASL_NTLM        FALSE
#endif
  /* Failing SASL authentication is a pain. Give a helping hand if
   * we were unable to select an AUTH mechanism.
   * `sasl->authmechs` are mechanisms offered by the peer
   * `sasl->prefmech`  are mechanisms preferred by us */
  unsigned short enabledmechs = sasl->authmechs & sasl->prefmech;

  якщо(!sasl->authmechs)
    infof(data, "SASL: no auth mechanism was offered or recognized");
  інакше якщо(!enabledmechs)
    infof(data, "SASL: no overlap between offered and configured "
          "auth mechanisms");
  інакше {
    infof(data, "SASL: no auth mechanism offered could be selected");
    якщо((enabledmechs & SASL_MECH_EXTERNAL) &&
       Curl_creds_has_passwd(data->conn->creds))
      infof(data, "SASL: auth EXTERNAL not chosen with password");
    sasl_unchosen(data, SASL_MECH_GSSAPI, enabledmechs,
                  CURL_SASL_KERBEROS5, Curl_auth_is_gssapi_supported(), NULL);
    sasl_unchosen(data, SASL_MECH_SCRAM_SHA_256, enabledmechs,
                  CURL_SASL_GASL, FALSE, NULL);
    sasl_unchosen(data, SASL_MECH_SCRAM_SHA_1, enabledmechs,
                  CURL_SASL_GASL, FALSE, NULL);
    sasl_unchosen(data, SASL_MECH_DIGEST_MD5, enabledmechs,
                  CURL_SASL_DIGEST, Curl_auth_is_digest_supported(), NULL);
    sasl_unchosen(data, SASL_MECH_CRAM_MD5, enabledmechs,
                  CURL_SASL_DIGEST, TRUE, NULL);
    sasl_unchosen(data, SASL_MECH_NTLM, enabledmechs,
                  CURL_SASL_NTLM, Curl_auth_is_ntlm_supported(), NULL);
    sasl_unchosen(data, SASL_MECH_OAUTHBEARER, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(data->conn->creds) ?
                  NULL : "CURLOPT_XOAUTH2_BEARER");
    sasl_unchosen(data, SASL_MECH_XOAUTH2, enabledmechs, TRUE, TRUE,
                  Curl_creds_has_oauth_bearer(data->conn->creds) ?
                  NULL : "CURLOPT_XOAUTH2_BEARER");
  }
#endif /* CURLVERBOSE */
  (void)sasl;
  (void)data;
  повернути CURLE_LOGIN_DENIED;
}

#endif /* protocols are enabled that use SASL */
