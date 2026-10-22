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
 ***************************************************************************/
#include "curl_setup.h"
#include "urldata.h"

#ifndef CURL_DISABLE_HTTP

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#include "transfer.h"
#include "sendf.h"
#include "curl_trc.h"
#include "formdata.h"
#include "mime.h"
#include "progress.h"
#include "curlx/base64.h"
#include "cookie.h"
#include "vauth/vauth.h"
#include "vquic/vquic.h"
#include "http_digest.h"
#include "http_ntlm.h"
#include "http_negotiate.h"
#include "http_aws_sigv4.h"
#include "http_httpsig.h"
#include "url.h"
#include "urlapi-int.h"
#include "curl_share.h"
#include "dynhds.h"
#include "http.h"
#include "headers.h"
#include "select.h"
#include "parsedate.h" /* for the week day and month names */
#include "multiif.h"
#include "strcase.h"
#include "content_encoding.h"
#include "http_proxy.h"
#include "http2.h"
#include "cfilters.h"
#include "connect.h"
#include "curlx/strdup.h"
#include "altsvc.h"
#include "hsts.h"
#include "rtsp.h"
#include "ws.h"
#include "bufref.h"
#include "curlx/strparse.h"

void Curl_http_neg_init(struct Curl_easy *data, struct http_negotiation *neg)
{
  memset(neg, 0, sizeof(*neg));
  neg->accept_09 = data->set.http09_allowed;
  switch(data->set.httpwant) {
  case CURL_HTTP_VERSION_1_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    neg->only_10 = TRUE;
    break;
  case CURL_HTTP_VERSION_1_1:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    break;
  case CURL_HTTP_VERSION_2_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->h2_upgrade = TRUE;
    break;
  case CURL_HTTP_VERSION_2TLS:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    break;
  case CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    neg->wanted = neg->allowed = (CURL_HTTP_V2x);
    data->state.http_neg.h2_prior_knowledge = TRUE;
    break;
  case CURL_HTTP_VERSION_3:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    neg->allowed = neg->wanted;
    break;
  case CURL_HTTP_VERSION_3ONLY:
    neg->wanted = neg->allowed = (CURL_HTTP_V3x);
    break;
  case CURL_HTTP_VERSION_NONE:
  default:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    break;
  }
}

CURLcode Curl_http_setup_conn(struct Curl_easy *data,
                              struct connectdata *conn)
{
  /* allocate the HTTP-specific struct for the Curl_easy, only to survive
     during this request */
  만약(data->state.http_neg.wanted == CURL_HTTP_V3x) {
    /* only HTTP/3, needs to work */
    CURLcode result = Curl_conn_may_http3(data, conn);
    만약(result)
      반환 result;
  }
  반환 CURLE_OK;
}

#ifndef CURL_DISABLE_PROXY
/*
 * checkProxyHeaders() checks the linked list of custom proxy headers
 * if proxy headers are not available, then it will lookup into http header
 * link list
 *
 * It takes a connectdata struct as input to see if this is a proxy request or
 * not, as it then might check a different header list. Provide the header
 * prefix without colon!
 */
char *Curl_checkProxyheaders(struct Curl_easy *data,
                             const struct connectdata *conn,
                             const char *thisheader,
                             const size_t thislen)
{
  struct curl_slist *head;

  위해(head = (conn->http_proxy.peer && data->set.sep_headers) ?
        data->set.proxyheaders : data->set.headers;
      head; head = head->next) {
    만약(curl_strnequal(head->data, thisheader, thislen) &&
       Curl_headersep(head->data[thislen]))
      반환 head->data;
  }

  반환 NULL;
}
#endif

/* If the header has a value, this function returns TRUE and the value is in
   'outp' with blanks trimmed off. */
static bool header_has_value(const char **headerp, struct Curl_str *outp)
{
  bool value = !curlx_str_cspn(headerp, outp, ";:") &&
    (!curlx_str_single(headerp, ':') || !curlx_str_single(headerp, ';'));

  만약(value) {
    curlx_str_cspn(headerp, outp, "\r\n");
    curlx_str_trimblanks(outp);
  }
  반환 value;
}

static bool http_header_is_empty(const char *header)
{
  struct Curl_str out;

  만약(header_has_value(&header, &out)) {
    반환 curlx_strlen(&out) == 0;
  }
  반환 TRUE; /* invalid header format, treat as empty */
}

/*
 * Strip off leading and trailing whitespace from the value in the given HTTP
 * header line and return a strdup-ed copy in 'valp' - returns an empty
 * string if the header value consists entirely of whitespace.
 *
 * If the header is provided as "name;", ending with a semicolon, it returns a
 * blank string.
 */
static CURLcode copy_custom_value(const char *header, char **valp)
{
  struct Curl_str out = { 0 };

  /* find the end of the header name */
  만약(header_has_value(&header, &out)) {
    *valp = curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
    만약(*valp)
      반환 CURLE_OK;
    반환 CURLE_OUT_OF_MEMORY;
  }
  /* bad input */
  *valp = NULL;
  반환 CURLE_BAD_FUNCTION_ARGUMENT;
}

/*
 * Strip off leading and trailing whitespace from the value in the given HTTP
 * header line and return a strdup-ed copy in 'valp' - returns an empty
 * string if the header value consists entirely of whitespace.
 *
 * This function MUST be used after the header has already been confirmed to
 * lead with "word:".
 *
 * @unittest: 1626
 */
char *Curl_copy_header_value(const char *header)
{
  struct Curl_str out;

  /* find the end of the header name */
  만약(!curlx_str_until(&header, &out, MAX_HTTP_RESP_HEADER_SIZE, ':') &&
     !curlx_str_single(&header, ':')) {
    curlx_str_untilnl(&header, &out, MAX_HTTP_RESP_HEADER_SIZE);
    curlx_str_trimblanks(&out);
    반환 curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
  }
  /* bad input, should never happen */
  DEBUGASSERT(0);
  반환 NULL;
}

#ifndef CURL_DISABLE_HTTP_AUTH

#ifndef CURL_DISABLE_BASIC_AUTH
/*
 * http_output_basic() sets up an Authorization: header (or the proxy version)
 * for HTTP Basic authentication.
 *
 * Returns CURLcode.
 */
static CURLcode http_output_basic(struct Curl_easy *data,
                                  struct connectdata *conn, bool proxy)
{
  size_t size = 0;
  char *authorization = NULL;
  char **p_hd;
  CURLcode result;
  struct Curl_creds *creds = NULL;
  char *out;

  /* credentials are unique per transfer for HTTP, do not use the ones for the
     connection */
  만약(proxy) {
#ifndef CURL_DISABLE_PROXY
    p_hd = &data->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
#아니면
    (void)conn;
    반환 CURLE_NOT_BUILT_IN;
#endif
  }
  아니면 {
    p_hd = &data->req.hd_auth;
    creds = data->state.creds;
  }

  만약(!creds) {
    DEBUGASSERT(0);
    반환 CURLE_FAILED_INIT;
  }

  out = curl_maprintf("%s:%s", creds->user, creds->passwd);
  만약(!out)
    반환 CURLE_OUT_OF_MEMORY;

  result = curlx_base64_encode((uint8_t *)out, strlen(out),
                               &authorization, &size);
  만약(result)
    goto fail;

  만약(!authorization) {
    result = CURLE_REMOTE_ACCESS_DENIED;
    goto fail;
  }

  curlx_free(*p_hd);
  *p_hd = curl_maprintf("%sAuthorization: Basic %s\r\n",
                        proxy ? "Proxy-" : "",
                        authorization);
  curlx_free(authorization);
  만약(!*p_hd) {
    result = CURLE_OUT_OF_MEMORY;
    goto fail;
  }

fail:
  curlx_free(out);
  반환 result;
}

#endif

#ifndef CURL_DISABLE_BEARER_AUTH
/*
 * http_output_bearer() sets up an Authorization: header
 * for HTTP Bearer authentication.
 *
 * Returns CURLcode.
 */
static CURLcode http_output_bearer(struct Curl_easy *data)
{
  char **userp;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(Curl_creds_has_oauth_bearer(data->state.creds));
  userp = &data->req.hd_auth;
  curlx_free(*userp);
  *userp = curl_maprintf("Authorization: Bearer %s\r\n",
                         Curl_creds_oauth_bearer(data->state.creds));

  만약(!*userp) {
    result = CURLE_OUT_OF_MEMORY;
    goto fail;
  }

fail:
  반환 result;
}
#endif

#endif

/* pickoneauth() selects the most favorable authentication method from the
 * ones available and the ones we want.
 *
 * return TRUE if one was picked
 */
static bool pickoneauth(struct auth *pick, unsigned long mask,
                        struct Curl_creds *creds)
{
  bool have_user_pass = Curl_creds_has_user_or_pass(creds);
  bool picked;
  /* only deal with authentication we want */
  unsigned long avail = pick->avail & pick->want & mask;
  picked = TRUE;

  /* The order of these checks is highly relevant, as this will be the order
     of preference in case of the existence of multiple accepted types. */
  만약(avail & CURLAUTH_NEGOTIATE)  /* available on empty creds */
    pick->picked = CURLAUTH_NEGOTIATE;
#ifndef CURL_DISABLE_BEARER_AUTH
  아니면 만약((avail & CURLAUTH_BEARER) && Curl_creds_has_oauth_bearer(creds))
    pick->picked = CURLAUTH_BEARER;
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  아니면 만약((avail & CURLAUTH_DIGEST) && have_user_pass)
    pick->picked = CURLAUTH_DIGEST;
#endif
  아니면 만약(avail & CURLAUTH_NTLM)
    pick->picked = CURLAUTH_NTLM;
#ifndef CURL_DISABLE_BASIC_AUTH
  아니면 만약((avail & CURLAUTH_BASIC) && have_user_pass)
    pick->picked = CURLAUTH_BASIC;
#endif
#ifndef CURL_DISABLE_AWS
  아니면 만약(avail & CURLAUTH_AWS_SIGV4)
    pick->picked = CURLAUTH_AWS_SIGV4;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  아니면 만약(avail & CURLAUTH_HTTPSIG)
    pick->picked = CURLAUTH_HTTPSIG;
#endif
  아니면 {
    pick->picked = CURLAUTH_PICKNONE; /* we select to use nothing */
    picked = FALSE;
  }
  pick->avail = CURLAUTH_NONE; /* clear it here */

  반환 picked;
}

/*
 * http_perhapsrewind()
 *
 * The current request needs to be done again - maybe due to a follow
 * or authentication negotiation. Check if:
 * 1) a rewind of the data sent to the server is necessary
 * 2) the current transfer should continue or be stopped early
 */
static CURLcode http_perhapsrewind(struct Curl_easy *data,
                                   struct connectdata *conn)
{
  curl_off_t bytessent = data->req.writebytecount;
  curl_off_t expectsend = Curl_creader_total_length(data);
  curl_off_t upload_remain = (expectsend >= 0) ? (expectsend - bytessent) : -1;
  bool little_upload_remains = (upload_remain >= 0 && upload_remain < 2000);
  bool needs_rewind = Curl_creader_needs_rewind(data);
  /* By default, we would like to abort the transfer when little or unknown
   * amount remains. This may be overridden by authentications further
   * below! */
  bool abort_upload = (!data->req.upload_done && !little_upload_remains);
  VERBOSE(const char *ongoing_auth = NULL);

  /* We need a rewind before uploading client read data again. The
   * checks below influence of the upload is to be continued
   * or aborted early.
   * This depends on how much remains to be sent and in what state
   * the authentication is. Some auth schemes such as NTLM do not work
   * for a new connection. */
  만약(needs_rewind) {
    infof(data, "Need to rewind upload for next request");
    Curl_creader_set_rewind(data, TRUE);
  }

  만약(conn->bits.close)
    /* If we already decided to close this connection, we cannot veto. */
    반환 CURLE_OK;

  만약(abort_upload) {
    /* We would like to abort the upload - but should we? */
#ifdef USE_NTLM
    만약((data->state.authproxy.picked == CURLAUTH_NTLM) ||
       (data->state.authhost.picked == CURLAUTH_NTLM)) {
      VERBOSE(ongoing_auth = "NTLM");
      만약((conn->http_ntlm_state != NTLMSTATE_NONE) ||
         (conn->proxy_ntlm_state != NTLMSTATE_NONE)) {
        /* The NTLM-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#endif
#ifdef USE_SPNEGO
    /* There is still data left to send */
    만약((data->state.authproxy.picked == CURLAUTH_NEGOTIATE) ||
       (data->state.authhost.picked == CURLAUTH_NEGOTIATE)) {
      VERBOSE(ongoing_auth = "NEGOTIATE");
      만약((conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) ||
         (conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE)) {
        /* The NEGOTIATE-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#endif
  }

  만약(abort_upload) {
    만약(upload_remain >= 0)
      infof(data, "%s%sclose instead of sending %" FMT_OFF_T " more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "",
            upload_remain);
    아니면
      infof(data, "%s%sclose instead of sending unknown amount "
            "of more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "");
    /* We decided to abort the ongoing transfer */
    streamclose(conn);
    data->req.size = 0; /* do not download any more than 0 bytes */
    data->req.http_bodyless = TRUE;
  }
  반환 CURLE_OK;
}

/**
 * http_should_fail() determines whether an HTTP response code has gotten us
 * into an error state or not.
 *
 * @retval FALSE communications should continue
 *
 * @retval TRUE communications should not continue
 */
static bool http_should_fail(struct Curl_easy *data, 정수 httpcode)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  /*
   * If we have not been asked to fail on error,
   * do not fail.
   */
  만약(!data->set.http_fail_on_error)
    반환 FALSE;

  /*
   * Any code < 400 is never terminal.
   */
  만약(httpcode < 400)
    반환 FALSE;

  /*
   * A 416 response to a resume request is presumably because the file is
   * already completely downloaded and thus not actually a fail.
   */
  만약(data->state.resume_from && data->state.httpreq == HTTPREQ_GET &&
     httpcode == 416)
    반환 FALSE;

  /*
   * Any code >= 400 that is not 401 or 407 is always
   * a terminal error
   */
  만약((httpcode != 401) && (httpcode != 407))
    반환 TRUE;

  /*
   * All we have left to deal with is 401 and 407
   */
  DEBUGASSERT((httpcode == 401) || (httpcode == 407));

  /*
   * Examine the current authentication state to see if this is an error. The
   * idea is for this function to get called after processing all the headers
   * in a response message. If we have been asked to authenticate at
   * a particular stage, and we have done it, we are OK. If we are already
   * completely authenticated, it is not OK to get another 401 or 407.
   *
   * It is possible for authentication to go stale such that the client needs
   * to reauthenticate. Once that info is available, use it here.
   */

  /*
   * Either we are not authenticating, or we are supposed to be authenticating
   * something else. This is an error.
   */
  만약((httpcode == 401) && !data->state.creds)
    반환 TRUE;
#ifndef CURL_DISABLE_PROXY
  만약((httpcode == 407) && !data->conn->http_proxy.creds)
    반환 TRUE;
#endif

  반환 (bool)data->state.authproblem;
}

/*
 * Curl_http_auth_act() gets called when all HTTP headers have been received
 * and it checks what authentication methods that are available and decides
 * which one (if any) to use. It will set 'newurl' if an auth method was
 * picked.
 */
CURLcode Curl_http_auth_act(struct Curl_easy *data)
{
  struct connectdata *conn = data->conn;
  bool pickhost = FALSE;
  bool pickproxy = FALSE;
  CURLcode result = CURLE_OK;
  unsigned long authmask = ~0UL;

  만약(!Curl_creds_has_oauth_bearer(data->state.creds))
    authmask &= (unsigned long)~CURLAUTH_BEARER;

  만약(100 <= data->req.httpcode && data->req.httpcode <= 199)
    /* this is a transient response code, ignore */
    반환 CURLE_OK;

  만약(data->state.authproblem)
    반환 data->set.http_fail_on_error ? CURLE_HTTP_RETURNED_ERROR : CURLE_OK;

  만약(data->state.creds &&
     ((data->req.httpcode == 401) ||
      (data->req.authneg && data->req.httpcode < 300))) {
    pickhost = pickoneauth(&data->state.authhost, authmask, data->state.creds);
    만약(!pickhost)
      data->state.authproblem = TRUE;
    아니면
      data->info.httpauthpicked = data->state.authhost.picked;
    만약(data->state.authhost.picked == CURLAUTH_NTLM &&
       (data->req.httpversion_sent > 11)) {
      infof(data, "Forcing HTTP/1.1 for NTLM");
      connclose(conn);
      data->state.http_neg.wanted = CURL_HTTP_V1x;
      data->state.http_neg.allowed = CURL_HTTP_V1x;
    }
  }
#ifndef CURL_DISABLE_PROXY
  만약(conn->http_proxy.creds &&
     ((data->req.httpcode == 407) ||
      (data->req.authneg && data->req.httpcode < 300))) {
    pickproxy = pickoneauth(&data->state.authproxy,
                            authmask & ~CURLAUTH_BEARER,
                            conn->http_proxy.creds);
    만약(!pickproxy)
      data->state.authproblem = TRUE;
    아니면
      data->info.proxyauthpicked = data->state.authproxy.picked;
  }
#endif

  만약(pickhost || pickproxy) {
    result = http_perhapsrewind(data, conn);
    만약(result)
      반환 result;

    /* In case this is GSS auth, the newurl field is already allocated so
       we must make sure to free it before allocating a new one. As figured
       out in bug #2284386 */
    curlx_free(data->req.newurl);
    /* clone URL */
    data->req.newurl = Curl_bufref_dup(&data->state.url);
    만약(!data->req.newurl)
      반환 CURLE_OUT_OF_MEMORY;
  }
  아니면 만약((data->req.httpcode < 300) &&
          !data->state.authhost.done &&
          data->req.authneg &&
          /* no (known) authentication available,
             authentication is not "done" yet and
             no authentication seems to be required and
             we did not try HEAD or GET */
          (data->state.httpreq != HTTPREQ_GET) &&
          (data->state.httpreq != HTTPREQ_HEAD)) {
    /* clone URL */
    data->req.newurl = Curl_bufref_dup(&data->state.url);
    만약(!data->req.newurl)
      반환 CURLE_OUT_OF_MEMORY;
    data->state.authhost.done = TRUE;
  }
  만약(http_should_fail(data, data->req.httpcode)) {
    failf(data, "The requested URL returned error: %d",
          data->req.httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
  }

  반환 result;
}

#ifndef CURL_DISABLE_HTTP_AUTH
/*
 * Output the correct authentication header depending on the auth type
 * and whether or not it is to a proxy.
 */
static CURLcode output_auth_headers(struct Curl_easy *data,
                                    struct connectdata *conn,
                                    struct auth *authstatus,
                                    const char *request,
                                    const char *path,
                                    bool proxy)
{
  const char *auth = NULL;
  CURLcode result = CURLE_OK;
  (void)conn;

#ifdef CURL_DISABLE_DIGEST_AUTH
  (void)request;
  (void)path;
#endif
#ifndef CURL_DISABLE_AWS
  만약((authstatus->picked == CURLAUTH_AWS_SIGV4) && !proxy) {
    /* this method is never for proxy */
    auth = "AWS_SIGV4";
    result = Curl_output_aws_sigv4(data);
    만약(result)
      반환 result;
  }
  아니면
#endif
#ifndef CURL_DISABLE_HTTPSIG
  만약((authstatus->picked == CURLAUTH_HTTPSIG) && !proxy) {
    /* HTTPSIG uses its own configured key material rather than
       data->state.creds. Do not let unrelated credentials from a
       redirected URL bypass the cross-host auth boundary. */
    만약(Curl_auth_allowed_to_host(data)) {
      auth = "HTTPSIG";
      result = Curl_output_httpsig(data);
      만약(result)
        반환 result;
    }
    아니면
      authstatus->done = TRUE;
  }
  아니면
#endif
#ifdef USE_SPNEGO
  만약(authstatus->picked == CURLAUTH_NEGOTIATE) {
    만약(
#ifndef CURL_DISABLE_PROXY
      (proxy && !Curl_checkProxyheaders(data, conn,
                                        STRCONST("Proxy-authorization"))) ||
#endif
      (!proxy && !Curl_checkheaders(data, STRCONST("Authorization")) &&
       Curl_auth_allowed_to_host(data))) {
      auth = "Negotiate";
      result = Curl_output_negotiate(data, conn, proxy);
      만약(result)
        반환 result;
    }
    아니면
      authstatus->done = TRUE;
  }
  아니면
#endif
#ifdef USE_NTLM
  만약(authstatus->picked == CURLAUTH_NTLM) {
    만약(proxy || Curl_auth_allowed_to_host(data)) {
      auth = "NTLM";
      result = Curl_output_ntlm(data, proxy);
      만약(result)
        반환 result;
    }
    아니면
      authstatus->done = TRUE;
  }
  아니면
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  만약(authstatus->picked == CURLAUTH_DIGEST) {
    auth = "Digest";
    result = Curl_output_digest(data,
                                proxy,
                                (const unsigned char *)request,
                                (const unsigned char *)path);
    만약(result)
      반환 result;
  }
  아니면
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
  만약(authstatus->picked == CURLAUTH_BASIC) {
    /* Basic */
    만약(
#ifndef CURL_DISABLE_PROXY
       (proxy && conn->http_proxy.creds &&
        Curl_creds_has_user_or_pass(conn->http_proxy.creds) &&
        !Curl_checkProxyheaders(data, conn,
                                STRCONST("Proxy-authorization"))) ||
#endif
       (!proxy && data->state.creds &&
        Curl_creds_has_user_or_pass(data->state.creds) &&
        !Curl_checkheaders(data, STRCONST("Authorization")))) {
      auth = "Basic";
      result = http_output_basic(data, conn, proxy);
      만약(result)
        반환 result;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
  만약(authstatus->picked == CURLAUTH_BEARER) {
    /* Bearer */
    만약(!proxy && Curl_creds_has_oauth_bearer(data->state.creds) &&
       !Curl_checkheaders(data, STRCONST("Authorization"))) {
      auth = "Bearer";
      result = http_output_bearer(data);
      만약(result)
        반환 result;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif

  만약(auth) {
#ifndef CURL_DISABLE_PROXY
    만약(proxy)
      data->info.proxyauthpicked = authstatus->picked;
    아니면
      data->info.httpauthpicked = authstatus->picked;
    infof(data, "%s auth using %s with user '%s'",
          proxy ? "Proxy" : "Server", auth,
          proxy ? (conn->http_proxy.creds ?
                   conn->http_proxy.creds->user : "") :
          (data->state.creds ?
           data->state.creds->user : ""));
#아니면
    (void)proxy;
    infof(data, "Server auth using %s with user '%s'",
          auth, data->state.creds ?
          data->state.creds->user : "");
#endif
    authstatus->multipass = !authstatus->done;
  }
  아니면 {
    authstatus->multipass = FALSE;
    만약(proxy)
      data->info.proxyauthpicked = 0;
    아니면
      data->info.httpauthpicked = 0;
  }

  반환 result;
}

CURLcode Curl_http_output_auth(struct Curl_easy *data,
                               struct connectdata *conn,
                               const char *request,
                               Curl_HttpReq httpreq,
                               const char *path,
                               const char *query,
                               bool is_connect)
{
  CURLcode result = CURLE_OK;
  struct auth *authhost;
  struct auth *authproxy;
  const char *path_and_query = path;
  char *tmp_str = NULL;

  DEBUGASSERT(data);
  authhost = &data->state.authhost;
  authproxy = &data->state.authproxy;

  만약(
#ifndef CURL_DISABLE_PROXY
    (!conn->http_proxy.peer || !conn->http_proxy.creds) &&
#endif
#ifdef USE_SPNEGO
    !(authhost->want & CURLAUTH_NEGOTIATE) &&
    !(authproxy->want & CURLAUTH_NEGOTIATE) &&
#endif
#ifndef CURL_DISABLE_HTTPSIG
    !(authhost->want & CURLAUTH_HTTPSIG) &&
#endif
    !data->state.creds) {
    /* no authentication with no user or password */
    authhost->done = TRUE;
    authproxy->done = TRUE;
    result = CURLE_OK;
    goto out;
  }

  만약(query) {
    tmp_str = curl_maprintf("%s?%s", path, query);
    만약(!tmp_str) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
    path_and_query = tmp_str;
  }

  만약(authhost->want && !authhost->picked)
    /* The app has selected one or more methods, but none has been picked
       so far by a server round-trip. Then we set the picked one to the
       want one, and if this is one single bit it will be used instantly. */
    authhost->picked = authhost->want;

  만약(authproxy->want && !authproxy->picked)
    /* The app has selected one or more methods, but none has been picked so
       far by a proxy round-trip. Then we set the picked one to the want one,
       and if this is one single bit it will be used instantly. */
    authproxy->picked = authproxy->want;

#ifndef CURL_DISABLE_PROXY
  /* Send proxy authentication header if needed */
  만약(conn->bits.origin_is_proxy || is_connect) {
    result = output_auth_headers(data, conn, authproxy, request,
                                 path_and_query, TRUE);
    만약(result)
      goto out;
  }
  아니면
#아니면
  (void)is_connect;
#endif /* CURL_DISABLE_PROXY */
    /* we have no proxy so let's pretend we are done authenticating
       with it */
    authproxy->done = TRUE;

  /* Either we have credentials for the origin we talk to or
     performing authentication is allowed here */
  만약(data->state.creds || Curl_auth_allowed_to_host(data))
    result = output_auth_headers(data, conn, authhost, request,
                                 path_and_query, FALSE);
  아니면
    authhost->done = TRUE;

  만약(((authhost->multipass && !authhost->done) ||
      (authproxy->multipass && !authproxy->done)) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_HEAD)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    data->req.authneg = TRUE;
  }
  아니면
    data->req.authneg = FALSE;

out:
  curlx_free(tmp_str);
  반환 result;
}

#아니면 /* !CURL_DISABLE_HTTP_AUTH */
/* when disabled */
CURLcode Curl_http_output_auth(struct Curl_easy *data,
                               struct connectdata *conn,
                               const char *request,
                               Curl_HttpReq httpreq,
                               const char *path,
                               const char *query,
                               bool is_connect)
{
  (void)data;
  (void)conn;
  (void)request;
  (void)httpreq;
  (void)path;
  (void)query;
  (void)is_connect;
  반환 CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP_AUTH, else */

#만약 defined(USE_SPNEGO) || defined(USE_NTLM) || \
  !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_BASIC_AUTH) || \
  !defined(CURL_DISABLE_BEARER_AUTH)
static bool authcmp(const char *auth, const char *line)
{
  /* the auth string must not have an alnum following */
  size_t n = strlen(auth);
  반환 curl_strnequal(auth, line, n) && !ISALNUM(line[n]);
}
#endif

#ifdef USE_SPNEGO
static CURLcode auth_spnego(struct Curl_easy *data,
                            bool proxy,
                            const char *auth,
                            struct auth *authp,
                            uint32_t *availp)
{
  만약((authp->avail & CURLAUTH_NEGOTIATE) || Curl_auth_is_spnego_supported()) {
    *availp |= CURLAUTH_NEGOTIATE;
    authp->avail |= CURLAUTH_NEGOTIATE;

    만약(authp->picked == CURLAUTH_NEGOTIATE) {
      struct connectdata *conn = data->conn;
      CURLcode result = Curl_input_negotiate(data, conn, proxy, auth);
      uint8_t *negstate = proxy ? &conn->proxy_negotiate_state :
        &conn->http_negotiate_state;
      만약(!result) {
        curlx_free(data->req.newurl);
        data->req.newurl = Curl_bufref_dup(&data->state.url);
        만약(!data->req.newurl)
          반환 CURLE_OUT_OF_MEMORY;
        data->state.authproblem = FALSE;
        /* we received a GSS auth token and we dealt with it fine */
        *negstate = CURL_NEGO_AUTH_RECV;
      }
      아니면
        data->state.authproblem = TRUE;
    }
  }
  반환 CURLE_OK;
}
#endif

#ifdef USE_NTLM
static CURLcode auth_ntlm(struct Curl_easy *data,
                          bool proxy,
                          const char *auth,
                          struct auth *authp,
                          uint32_t *availp)
{
  /* NTLM support requires the SSL crypto libs */
  만약((authp->avail & CURLAUTH_NTLM) || Curl_auth_is_ntlm_supported()) {
    *availp |= CURLAUTH_NTLM;
    authp->avail |= CURLAUTH_NTLM;

    만약(authp->picked == CURLAUTH_NTLM) {
      /* NTLM authentication is picked and activated */
      CURLcode result = Curl_input_ntlm(data, proxy, auth);
      만약(!result)
        data->state.authproblem = FALSE;
      아니면 {
        만약(result == CURLE_OUT_OF_MEMORY)
          반환 result;
        infof(data, "NTLM authentication problem, ignoring.");
        data->state.authproblem = TRUE;
      }
    }
  }
  반환 CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_DIGEST_AUTH
static CURLcode auth_digest(struct Curl_easy *data,
                            bool proxy,
                            const char *auth,
                            struct auth *authp,
                            uint32_t *availp)
{
  만약(authp->avail & CURLAUTH_DIGEST) {
    *availp |= CURLAUTH_DIGEST;
    infof(data, "Ignoring duplicate digest auth header.");
  }
  아니면 만약(Curl_auth_is_digest_supported()) {
    CURLcode result;

    *availp |= CURLAUTH_DIGEST;
    authp->avail |= CURLAUTH_DIGEST;

    /* We call this function on input Digest headers even if Digest
     * authentication is not activated yet, as we need to store the
     * incoming data from this header in case we are going to use
     * Digest */
    result = Curl_input_digest(data, proxy, auth);
    만약(result) {
      만약(result == CURLE_OUT_OF_MEMORY)
        반환 result;
      infof(data, "Digest authentication problem, ignoring.");
      data->state.authproblem = TRUE;
    }
  }
  반환 CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BASIC_AUTH
static CURLcode auth_basic(struct Curl_easy *data,
                           struct auth *authp,
                           uint32_t *availp)
{
  *availp |= CURLAUTH_BASIC;
  authp->avail |= CURLAUTH_BASIC;
  만약(authp->picked == CURLAUTH_BASIC) {
    /* We asked for Basic authentication but got a 40X back anyway, which
       means our name+password is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(data, "Basic authentication problem, ignoring.");
    data->state.authproblem = TRUE;
  }
  반환 CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BEARER_AUTH
static CURLcode auth_bearer(struct Curl_easy *data,
                            struct auth *authp,
                            uint32_t *availp)
{
  *availp |= CURLAUTH_BEARER;
  authp->avail |= CURLAUTH_BEARER;
  만약(authp->picked == CURLAUTH_BEARER) {
    /* We asked for Bearer authentication but got a 40X back anyway, which
       means our token is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(data, "Bearer authentication problem, ignoring.");
    data->state.authproblem = TRUE;
  }
  반환 CURLE_OK;
}
#endif

/*
 * Curl_http_input_auth() deals with Proxy-Authenticate: and WWW-Authenticate:
 * headers. They are dealt with both in the transfer.c main loop and in the
 * proxy CONNECT loop.
 *
 * The 'auth' line ends with a NUL byte without CR or LF present.
 */
CURLcode Curl_http_input_auth(struct Curl_easy *data, bool proxy,
                              const char *auth) /* the first non-space */
{
  /*
   * This resource requires authentication
   */
#만약 defined(USE_SPNEGO) ||                      \
  defined(USE_NTLM) ||                          \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||         \
  !defined(CURL_DISABLE_BASIC_AUTH) ||          \
  !defined(CURL_DISABLE_BEARER_AUTH)

  uint32_t *availp;
  struct auth *authp;
  CURLcode result = CURLE_OK;
  DEBUGASSERT(auth);
  DEBUGASSERT(data);

  만약(proxy) {
    availp = &data->info.proxyauthavail;
    authp = &data->state.authproxy;
  }
  아니면 {
    availp = &data->info.httpauthavail;
    authp = &data->state.authhost;
  }

  /*
   * Here we check if we want the specific single authentication (using ==) and
   * if we do, we initiate usage of it.
   *
   * If the provided authentication is wanted as one out of several accepted
   * types (using &), we OR this authentication type to the authavail
   * variable.
   *
   * Note:
   *
   * ->picked is first set to the 'want' value (one or more bits) before the
   * request is sent, and then it is again set _after_ all response 401/407
   * headers have been received but then only to a single preferred method
   * (bit).
   */

  동안(*auth) {
#ifdef USE_SPNEGO
    만약(authcmp("Negotiate", auth))
      result = auth_spnego(data, proxy, auth, authp, availp);
#endif
#ifdef USE_NTLM
    만약(!result && authcmp("NTLM", auth))
      result = auth_ntlm(data, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
    만약(!result && authcmp("Digest", auth))
      result = auth_digest(data, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
    만약(!result && authcmp("Basic", auth))
      result = auth_basic(data, authp, availp);
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
    만약(authcmp("Bearer", auth))
      result = auth_bearer(data, authp, availp);
#endif

    만약(result)
      break;

    /* there may be multiple methods on one line, so keep reading */
    auth = strchr(auth, ',');
    만약(auth) /* if we are on a comma, skip it */
      auth++;
    아니면
      break;
    curlx_str_passblanks(&auth);
  }
  반환 result;
#아니면
  (void)data;
  (void)proxy;
  (void)auth;
  /* nothing to do when disabled */
  반환 CURLE_OK;
#endif
}

static void http_switch_to_get(struct Curl_easy *data, 정수 code)
{
  const char *req = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  만약((req || data->state.httpreq != HTTPREQ_GET) &&
     (data->set.http_follow_mode == CURLFOLLOW_OBEYCODE)) {
    NOVERBOSE((void)code);
    infof(data, "Switch to GET because of %d response", code);
    data->state.http_ignorecustom = TRUE;
  }
  아니면 만약(req && (data->set.http_follow_mode != CURLFOLLOW_FIRSTONLY))
    infof(data, "Stick to %s instead of GET", req);

  data->state.httpreq = HTTPREQ_GET;
  Curl_creader_set_rewind(data, FALSE);
}

#define HTTPREQ_IS_POST(data)                    \
  ((data)->state.httpreq == HTTPREQ_POST ||      \
   (data)->state.httpreq == HTTPREQ_POST_FORM || \
   (data)->state.httpreq == HTTPREQ_POST_MIME)

static CURLcode set_auto_referer(struct Curl_easy *data)
{
  CURLU *u;
  char *referer = NULL;
  CURLUcode uc;

  /* We are asked to automatically set the previous URL as the referer
     when we get the next URL. We pick the ->url field, which may or may
     not be 100% correct */
  Curl_bufref_free(&data->state.referer);

  /* Make a copy of the URL without credentials and fragment */
  u = curl_url();
  만약(!u)
    반환 CURLE_OUT_OF_MEMORY;

  uc = curl_url_set(u, CURLUPART_URL,
                    Curl_bufref_ptr(&data->state.url), 0);
  만약(!uc)
    uc = curl_url_set(u, CURLUPART_FRAGMENT, NULL, 0);
  만약(!uc)
    uc = curl_url_set(u, CURLUPART_USER, NULL, 0);
  만약(!uc)
    uc = curl_url_set(u, CURLUPART_PASSWORD, NULL, 0);
  만약(!uc)
    uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

  curl_url_cleanup(u);

  만약(uc || !referer)
    반환 CURLE_OUT_OF_MEMORY;

  Curl_bufref_set(&data->state.referer, referer, 0, curl_free);
  반환 CURLE_OK;
}

static CURLcode follow_parse_url(struct Curl_easy *data, const char *newurl,
                                 followtype type, char **follow_urlp,
                                 CURLUcode *ucp)
{
  CURLUcode uc;

  DEBUGASSERT(data->state.uh);
  uc = curl_url_set(data->state.uh, CURLUPART_URL, newurl, (unsigned 정수)
                    ((type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                     ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) |
                     CURLU_ALLOW_SPACE |
                     (data->set.path_as_is ? CURLU_PATH_AS_IS : 0)));
  *ucp = uc;
  만약(uc) {
    만약((uc == CURLUE_OUT_OF_MEMORY) || (type != FOLLOW_FAKE)) {
      failf(data, "The redirect target URL could not be parsed: %s",
            curl_url_strerror(uc));
      반환 Curl_uc_to_curlcode(uc);
    }

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, duplicate the field as-is */
    *follow_urlp = curlx_strdup(newurl);
    만약(!*follow_urlp)
      반환 CURLE_OUT_OF_MEMORY;
  }
  아니면 {
    CURLU *u = curl_url();
    만약(!u)
      반환 CURLE_OUT_OF_MEMORY;
    uc = curl_url_set(u, CURLUPART_URL,
                      Curl_bufref_ptr(&data->state.url),
                      CURLU_URLENCODE | CURLU_ALLOW_SPACE);
    만약(!uc)
      uc = curl_url_get(data->state.uh, CURLUPART_URL, follow_urlp, 0);
    만약(uc) {
      curl_url_cleanup(u);
      반환 Curl_uc_to_curlcode(uc);
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    {
      bool same_origin = Curl_url_same_origin(u, data->state.uh);
      curl_url_cleanup(u);
      만약(!same_origin)
        Curl_auth_digest_cleanup(&data->state.digest);
    }
#아니면
    curl_url_cleanup(u);
#endif
  }
  반환 CURLE_OK;
}

static CURLcode follow_fake(struct Curl_easy *data, char *follow_url,
                            CURLUcode uc, bool reachedmax)
{
  /* we are only figuring out the new URL if we would have followed locations
     but now we are done so we can get out! */
  만약(!uc) {
    CURLU *u = curl_url();
    char *nocred = NULL;

    만약(!u) {
      curlx_free(follow_url);
      반환 CURLE_OUT_OF_MEMORY;
    }
    만약(!curl_url_set(u, CURLUPART_URL, follow_url,
                     CURLU_NON_SUPPORT_SCHEME |
                     (data->set.path_as_is ? CURLU_PATH_AS_IS : 0)) &&
       !curl_url_set(u, CURLUPART_USER, NULL, 0) &&
       !curl_url_set(u, CURLUPART_PASSWORD, NULL, 0))
      (void)curl_url_get(u, CURLUPART_URL, &nocred, CURLU_GET_EMPTY);
    curl_url_cleanup(u);
    만약(nocred) {
      curlx_free(follow_url);
      follow_url = nocred;
    }
  }
  data->info.wouldredirect = follow_url;

  만약(reachedmax) {
    failf(data, "Maximum (%d) redirects followed", data->set.maxredirs);
    반환 CURLE_TOO_MANY_REDIRECTS;
  }
  반환 CURLE_OK;
}

static bool follow_switch_get(struct Curl_easy *data)
{
  /*
   * We get here when the HTTP code is 300-399 and 401 or 407. We need to
   * perform differently based on exactly what return code there was.
   */
  switch(data->info.httpcode) {
  case 301: /* Moved Permanently */
    /* (quote from RFC7231, section 6.4.2)
     *
     * Note: For historical reasons, a user agent MAY change the request
     * method from POST to GET for the subsequent request. If this
     * behavior is undesired, the 307 (Temporary Redirect) status code
     * can be used instead.
     *
     * ----
     *
     * Many webservers expect this, so these servers often answers to a POST
     * request with an error page. To be sure that libcurl gets the page that
     * most user agents would get, libcurl has to force GET.
     *
     * This behavior is forbidden by RFC1945 and the obsolete RFC2616, and
     * can be overridden with CURLOPT_POSTREDIR.
     */
    만약(HTTPREQ_IS_POST(data) && !data->set.post301) {
      http_switch_to_get(data, 301);
      반환 TRUE;
    }
    break;
  case 302: /* Found */
    /* (quote from RFC7231, section 6.4.3)
     *
     * Note: For historical reasons, a user agent MAY change the request
     * method from POST to GET for the subsequent request. If this
     * behavior is undesired, the 307 (Temporary Redirect) status code
     * can be used instead.
     *
     * ----
     *
     * Many webservers expect this, so these servers often answers to a POST
     * request with an error page. To be sure that libcurl gets the page that
     * most user agents would get, libcurl has to force GET.
     *
     * This behavior is forbidden by RFC1945 and the obsolete RFC2616, and
     * can be overridden with CURLOPT_POSTREDIR.
     */
    만약(HTTPREQ_IS_POST(data) && !data->set.post302) {
      http_switch_to_get(data, 302);
      반환 TRUE;
    }
    break;
  case 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     */
    만약(!HTTPREQ_IS_POST(data) || !data->set.post303) {
      http_switch_to_get(data, 303);
      반환 TRUE;
    }
    break;

    /* 304 means we did a conditional request and it was "Not modified".
     * We should not get any Location: header in this response!
     */

    /* 305 (quote from RFC2616, section 10.3.6):
     * "The requested resource MUST be accessed through the proxy given
     * by the Location field. The Location field gives the URI of the
     * proxy. The recipient is expected to repeat this single request
     * via the proxy. 305 responses MUST only be generated by origin
     * servers."
     */
  default:
    break;
  }
  반환 FALSE;
}

CURLcode Curl_http_follow(struct Curl_easy *data, const char *newurl,
                          followtype type)
{
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  char *follow_url = NULL;
  CURLUcode uc;
  CURLcode result;
  CURLcode rewind_result;
  bool switch_to_get;

  DEBUGASSERT(type != FOLLOW_NONE);

  만약(type != FOLLOW_FAKE)
    data->state.requests++; /* count all real follows */
  만약(type == FOLLOW_REDIR) {
    만약((data->set.maxredirs != -1) &&
       (data->state.followlocation >= data->set.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    아니면 {
      data->state.followlocation++; /* count redirect-followings, including
                                       auth reloads */

      만약(data->set.http_auto_referer) {
        result = set_auto_referer(data);
        만약(result)
          반환 result;
      }
    }
  }

  만약((type != FOLLOW_RETRY) &&
     (data->req.httpcode != 401) && (data->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, NULL, 0, FALSE)) {
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: do not allow a custom port number */
    disallowport = TRUE;
  }

  result = follow_parse_url(data, newurl, type, &follow_url, &uc);
  만약(result)
    반환 result;

  DEBUGASSERT(follow_url);

  만약(type == FOLLOW_FAKE)
    반환 follow_fake(data, follow_url, uc, reachedmax);

  만약(disallowport)
    data->state.allow_port = FALSE;

  Curl_bufref_set(&data->state.url, follow_url, 0, curl_free);
  rewind_result = Curl_req_soft_reset(&data->req, data);
  infof(data, "Issue another request to this URL: '%s'", follow_url);
  만약((data->set.http_follow_mode == CURLFOLLOW_FIRSTONLY) &&
     !data->state.http_ignorecustom &&
     CURL_EASY_STR(data, STRING_CUSTOMREQUEST)) {
    data->state.http_ignorecustom = TRUE;
    infof(data, "Drop custom request method for next request");
  }

  switch_to_get = follow_switch_get(data);

  /* When rewind of upload data failed and we are not switching to GET,
   * we need to fail the follow, as we cannot send the data again. */
  만약(rewind_result && !switch_to_get)
    반환 rewind_result;

  Curl_pgrsTime(data, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(data);

  반환 CURLE_OK;
}

/*
 * Curl_compareheader()
 *
 * Returns TRUE if 'headerline' contains the 'header' with given 'content'
 * (within a comma-separated list of tokens). Pass 'header' WITH the colon.
 *
 * @unittest: 1625
 */
bool Curl_compareheader(const char *headerline, /* line to check */
                        const char *header, /* header keyword _with_ colon */
                        const size_t hlen, /* len of the keyword in bytes */
                        const char *content, /* content string to find */
                        const size_t clen) /* len of the content in bytes */
{
  /* RFC2616, section 4.2 says: "Each header field consists of a name followed
   * by a colon (":") and the field value. Field names are case-insensitive.
   * The field value MAY be preceded by any amount of LWS, though a single SP
   * is preferred." */

  const char *p;
  struct Curl_str val;
  DEBUGASSERT(hlen);
  DEBUGASSERT(clen);
  DEBUGASSERT(header);
  DEBUGASSERT(content);

  만약(!curl_strnequal(headerline, header, hlen))
    반환 FALSE; /* does not start with header */

  /* pass the header */
  p = &headerline[hlen];

  만약(curlx_str_cspn(&p, &val, "\r\n"))
    반환 FALSE;
  curlx_str_trimblanks(&val);

  /* find the content string in the rest of the line */
  만약(curlx_strlen(&val) >= clen) {
    size_t len;
    p = curlx_str(&val);
    위해(len = curlx_strlen(&val); len >= clen;) {
      struct Curl_str next;
      const char *o = p;
      /* after a match there must be a comma, space, newline or NUL byte */
      만약(curl_strnequal(p, content, clen) &&
         ((p[clen] == ',') || ISBLANK(p[clen]) || ISNEWLINE(p[clen]) ||
          !p[clen]))
        반환 TRUE; /* match! */
      /* advance to the next comma */
      만약(curlx_str_until(&p, &next, len, ',') ||
         curlx_str_single(&p, ','))
        break; /* no comma, get out */

      /* if there are more dummy commas, move over them as well */
      do
        curlx_str_passblanks(&p);
      동안(!curlx_str_single(&p, ','));
      /* trailing blanks may move the parsing point past the value end,
         then there is nothing left to match */
      만약((size_t)(p - o) > len)
        break;
      len -= (p - o);
    }
  }
  반환 FALSE; /* no match */
}

struct cr_exp100_ctx {
  struct Curl_creader super;
  struct curltime start; /* time started waiting */
  enum expect100 state;
};

/* Expect: 100-continue client reader, blocking uploads */

static void http_exp100_continue(struct Curl_easy *data,
                                 struct Curl_creader *reader)
{
  struct cr_exp100_ctx *ctx = reader->ctx;
  만약(ctx->state > EXP100_SEND_DATA) {
    ctx->state = EXP100_SEND_DATA;
    Curl_expire_clear(data, EXPIRE_100_TIMEOUT);
  }
}

static CURLcode cr_exp100_read(struct Curl_easy *data,
                               struct Curl_creader *reader,
                               char *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  struct cr_exp100_ctx *ctx = reader->ctx;
  timediff_t ms;

  switch(ctx->state) {
  case EXP100_SENDING_REQUEST:
    만약(!Curl_req_sendbuf_empty(data)) {
      /* The initial request data has not been fully sent yet. Do
       * not start the timer yet. */
      DEBUGF(infof(data, "cr_exp100_read, request not full sent yet"));
      *nread = 0;
      *eos = FALSE;
      반환 CURLE_OK;
    }
    /* We are now waiting for a reply from the server or
     * a timeout on our side IFF the request has been fully sent. */
    DEBUGF(infof(data, "cr_exp100_read, start AWAITING_CONTINUE, "
                 "timeout %dms", data->set.expect_100_timeout));
    ctx->state = EXP100_AWAITING_CONTINUE;
    ctx->start = *Curl_pgrs_now(data);
    Curl_expire_set(data, EXPIRE_100_TIMEOUT,
                    data->set.expect_100_timeout, &ctx->start);
    *nread = 0;
    *eos = FALSE;
    반환 CURLE_OK;
  case EXP100_FAILED:
    DEBUGF(infof(data, "cr_exp100_read, expectation failed, error"));
    *nread = 0;
    *eos = FALSE;
    반환 CURLE_READ_ERROR;
  case EXP100_AWAITING_CONTINUE:
    ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->start);
    만약(ms < data->set.expect_100_timeout) {
      DEBUGF(infof(data, "cr_exp100_read, AWAITING_CONTINUE, not expired"));
      *nread = 0;
      *eos = FALSE;
      반환 CURLE_OK;
    }
    /* we have waited long enough, continue anyway */
    http_exp100_continue(data, reader);
    infof(data, "Done waiting for 100-continue");
    FALLTHROUGH();
  default:
    DEBUGF(infof(data, "cr_exp100_read, pass through"));
    반환 Curl_creader_read(data, reader->next, buf, blen, nread, eos);
  }
}

static void cr_exp100_done(struct Curl_easy *data,
                           struct Curl_creader *reader, 정수 premature)
{
  struct cr_exp100_ctx *ctx = reader->ctx;
  ctx->state = premature ? EXP100_FAILED : EXP100_SEND_DATA;
  Curl_expire_clear(data, EXPIRE_100_TIMEOUT);
}

static const struct Curl_crtype cr_exp100 = {
  "cr-exp100",
  Curl_creader_def_init,
  cr_exp100_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  Curl_creader_def_total_length,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  cr_exp100_done,
  sizeof(struct cr_exp100_ctx)
};

static CURLcode http_exp100_add_reader(struct Curl_easy *data)
{
  struct Curl_creader *reader = NULL;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_exp100, CURL_CR_PROTOCOL);
  만약(!result)
    result = Curl_creader_add(data, reader);
  만약(!result) {
    struct cr_exp100_ctx *ctx = reader->ctx;
    ctx->state = EXP100_SENDING_REQUEST;
  }

  만약(result && reader)
    Curl_creader_free(data, reader);
  반환 result;
}

static void http_exp100_got100(struct Curl_easy *data)
{
  struct Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  만약(r)
    http_exp100_continue(data, r);
}

static bool http_exp100_is_waiting(struct Curl_easy *data)
{
  struct Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  만약(r) {
    struct cr_exp100_ctx *ctx = r->ctx;
    반환 ctx->state == EXP100_AWAITING_CONTINUE;
  }
  반환 FALSE;
}

static void http_exp100_send_anyway(struct Curl_easy *data)
{
  struct Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  만약(r)
    http_exp100_continue(data, r);
}

static bool http_exp100_is_selected(struct Curl_easy *data)
{
  struct Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  반환 !!r;
}

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
CURLcode Curl_http_doing_pollset(struct Curl_easy *data,
                                 struct easy_pollset *ps)
{
  /* write mode */
  반환 Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

CURLcode Curl_http_perform_pollset(struct Curl_easy *data,
                                   struct easy_pollset *ps)
{
  struct connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  만약(CURL_REQ_WANT_RECV(data)) {
    result = Curl_pollset_add_in(data, ps, conn->sock[FIRSTSOCKET]);
  }

  /* on a "Expect: 100-continue" timed wait, do not poll for outgoing */
  만약(!result && Curl_req_want_send(data) && !http_exp100_is_waiting(data)) {
    result = Curl_pollset_add_out(data, ps, conn->sock[FIRSTSOCKET]);
  }
  반환 result;
}

static CURLcode http_write_header(struct Curl_easy *data,
                                  const char *hd, size_t hdlen)
{
  CURLcode result;
  정수 writetype;

  /* now, only output this if the header AND body are requested:
   */
  Curl_debug(data, CURLINFO_HEADER_IN, hd, hdlen);

  writetype = CLIENTWRITE_HEADER |
    ((data->req.httpcode / 100 == 1) ? CLIENTWRITE_1XX : 0);

  result = Curl_client_write(data, writetype, hd, hdlen);
  만약(result)
    반환 result;

  result = Curl_bump_headersize(data, hdlen, FALSE);
  만약(result)
    반환 result;

  data->req.deductheadercount = (100 <= data->req.httpcode &&
                                 199 >= data->req.httpcode) ?
    data->req.headerbytecount : 0;
  반환 result;
}

/*
 * Curl_http_done() gets called after a single HTTP request has been
 * performed.
 */

CURLcode Curl_http_done(struct Curl_easy *data,
                        CURLcode status, bool premature)
{
  struct connectdata *conn = data->conn;

  /* Clear multipass flag. If authentication is not done yet, then it will get
   * a chance to be set back to true when we output the next auth header */
  data->state.authhost.multipass = FALSE;
  data->state.authproxy.multipass = FALSE;

  만약(curlx_dyn_len(&data->state.headerb)) {
    (void)http_write_header(data, curlx_dyn_ptr(&data->state.headerb),
                            curlx_dyn_len(&data->state.headerb));
  }
  curlx_dyn_reset(&data->state.headerb);

  만약(status)
    반환 status;

  만약(!premature && /* this check is pointless when DONE is called before the
                      entire operation is complete */
     !conn->bits.retry &&
     !data->set.connect_only &&
     (data->req.bytecount +
      data->req.headerbytecount -
      data->req.deductheadercount) <= 0) {
    /* If this connection is not closed to be retried, AND nothing was
       read from the HTTP server (that counts), this cannot be right so we
       return an error here */
    failf(data, "Empty reply from server");
    /* Mark it as closed to avoid the "left intact" message */
    streamclose(conn);
    반환 CURLE_GOT_NOTHING;
  }

  반환 CURLE_OK;
}

/* Determine if we may use HTTP 1.1 for this request. */
static bool http_may_use_1_1(const struct Curl_easy *data)
{
  const struct connectdata *conn = data->conn;
  /* We have seen a previous response for *this* transfer with 1.0,
   * on another connection or the same one. */
  만약(data->state.http_neg.rcvd_min == 10)
    반환 FALSE;
  /* We have seen a previous response on *this* connection with 1.0. */
  만약(conn && conn->httpversion_seen == 10)
    반환 FALSE;
  /* We want 1.0 and have seen no previous response on *this* connection
     with a higher version (maybe no response at all yet). */
  만약(data->state.http_neg.only_10 &&
     (!conn || conn->httpversion_seen <= 10))
    반환 FALSE;
  /* We are not restricted to use 1.0 only. */
  반환 !data->state.http_neg.only_10;
}

static unsigned char http_request_version(struct Curl_easy *data)
{
  unsigned char v = Curl_conn_http_version(data, data->conn);
  만약(!v) {
    /* No specific HTTP connection filter installed. */
    v = http_may_use_1_1(data) ? 11 : 10;
  }
  반환 v;
}

static const char *get_http_string(정수 httpversion)
{
  switch(httpversion) {
  case 30:
    반환 "3";
  case 20:
    반환 "2";
  case 11:
    반환 "1.1";
  default:
    반환 "1.0";
  }
}

CURLcode Curl_add_custom_headers(struct Curl_easy *data,
                                 bool is_connect, 정수 httpversion,
                                 struct dynbuf *req)
{
  struct curl_slist *h[2];
  struct curl_slist *headers;
  정수 numlists = 1; /* by default */
  정수 i;

#ifndef CURL_DISABLE_PROXY
  enum Curl_proxy_use proxy;

  만약(is_connect)
    proxy = HEADER_CONNECT;
  아니면
    proxy = data->conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  switch(proxy) {
  case HEADER_SERVER:
    h[0] = data->set.headers;
    break;
  case HEADER_PROXY:
    h[0] = data->set.headers;
    만약(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    break;
  case HEADER_CONNECT:
    만약(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    아니면
      h[0] = data->set.headers;
    break;
  case HEADER_CONNECT_UDP:
    만약(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    아니면
      h[0] = data->set.headers;
    break;
  }
#아니면
  (void)is_connect;
  h[0] = data->set.headers;
#endif

  /* loop through one or two lists */
  위해(i = 0; i < numlists; i++) {
    위해(headers = h[i]; headers; headers = headers->next) {
      CURLcode result = CURLE_OK;
      bool blankheader = FALSE;
      struct Curl_str name;
      const char *p = headers->data;
      const char *origp = p;
      size_t hlen = strlen(origp);

      /* explicitly asked to send header without content is done by a header
         that ends with a semicolon, but there must be no colon present in the
         name */
      만약(!curlx_str_until(&p, &name, hlen, ';') &&
         !curlx_str_single(&p, ';') &&
         !curlx_str_single(&p, '\0') &&
         !memchr(curlx_str(&name), ':', curlx_strlen(&name)))
        blankheader = TRUE;
      아니면 {
        p = origp;
        만약(!curlx_str_until(&p, &name, hlen, ':') &&
           !curlx_str_single(&p, ':')) {
          struct Curl_str val;
          curlx_str_untilnl(&p, &val, hlen);
          curlx_str_trimblanks(&val);
          만약(!curlx_strlen(&val))
            /* no content, do not send this */
            continue;
        }
        아니면
          /* no colon */
          continue;
      }

      /* a field name is a token and carries no surrounding whitespace, so
         trim the parsed name before matching. Otherwise `Authorization :`
         (space before the colon) slips past the Authorization/Cookie check
         below and gets forwarded to another host on a redirect. */
      curlx_str_trimblanks(&name);

      /* only send this if the contents was non-blank or done special */

      만약(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom
            Host: header as that will produce *two* in the same
            request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      아니면 만약(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      아니면 만약(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      아니면 만약(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      아니면 만약(curlx_str_casecompare(&name, "Connection"))
        /* Connection headers are handled specially */
        ;
      아니면 만약((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        /* HTTP/2 does not support chunked requests */
        ;
      아니면 만약((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      아니면 만약(blankheader) {
        result = curlx_dyn_addn(req, curlx_str(&name), curlx_strlen(&name));
        만약(!result)
          result = curlx_dyn_addn(req, STRCONST(":\r\n"));
      }
      아니면
        result = curlx_dyn_addf(req, "%s\r\n", origp);

      만약(result)
        반환 result;
    }
  }

  반환 CURLE_OK;
}

#ifndef CURL_DISABLE_PARSEDATE
CURLcode Curl_add_timecondition(struct Curl_easy *data,
                                struct dynbuf *req)
{
  const struct tm *tm;
  struct tm keeptime;
  CURLcode result;
  char datestr[80];
  const char *condp;
  size_t len;

  만약(data->set.timecondition == CURL_TIMECOND_NONE)
    /* no condition was asked for */
    반환 CURLE_OK;

  result = curlx_gmtime(data->set.timevalue, &keeptime);
  만약(result) {
    failf(data, "Invalid TIMEVALUE");
    반환 result;
  }
  tm = &keeptime;

  switch(data->set.timecondition) {
  default:
    DEBUGF(infof(data, "invalid time condition"));
    반환 CURLE_BAD_FUNCTION_ARGUMENT;

  case CURL_TIMECOND_IFMODSINCE:
    condp = "If-Modified-Since";
    len = 17;
    break;
  case CURL_TIMECOND_IFUNMODSINCE:
    condp = "If-Unmodified-Since";
    len = 19;
    break;
  case CURL_TIMECOND_LASTMOD:
    condp = "Last-Modified";
    len = 13;
    break;
  }

  만약(Curl_checkheaders(data, condp, len)) {
    /* A custom header was specified; it will be sent instead. */
    반환 CURLE_OK;
  }

  /* The If-Modified-Since header family should have their times set in
   * GMT as RFC2616 defines: "All HTTP date/time stamps MUST be
   * represented in Greenwich Mean Time (GMT), without exception. For the
   * purposes of HTTP, GMT is exactly equal to UTC (Coordinated Universal
   * Time)." (see page 20 of RFC2616).
   */

  /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
  curl_msnprintf(datestr, sizeof(datestr),
                 "%s: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                 condp,
                 Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                 tm->tm_mday,
                 Curl_month[tm->tm_mon],
                 tm->tm_year + 1900,
                 tm->tm_hour,
                 tm->tm_min,
                 tm->tm_sec);

  result = curlx_dyn_add(req, datestr);
  반환 result;
}
#아니면
/* disabled */
CURLcode Curl_add_timecondition(struct Curl_easy *data,
                                struct dynbuf *req)
{
  (void)data;
  (void)req;
  반환 CURLE_OK;
}
#endif

void Curl_http_method(struct Curl_easy *data,
                      const char **method, Curl_HttpReq *reqp)
{
  Curl_HttpReq httpreq = (Curl_HttpReq)data->state.httpreq;
  const char *request;
#ifndef CURL_DISABLE_WEBSOCKETS
  만약(data->conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
    httpreq = HTTPREQ_GET;
  아니면
#endif
  만약((data->conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_FTP)) &&
     data->state.upload)
    httpreq = HTTPREQ_PUT;

  /* Now set the 'request' pointer to the proper request string */
  만약(!data->state.http_ignorecustom &&
     CURL_EASY_STR(data, STRING_CUSTOMREQUEST)) {
    request = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);
  }
  아니면 {
    만약(data->req.no_body)
      request = "HEAD";
    아니면 {
      DEBUGASSERT((httpreq >= HTTPREQ_GET) && (httpreq <= HTTPREQ_HEAD));
      switch(httpreq) {
      case HTTPREQ_POST:
      case HTTPREQ_POST_FORM:
      case HTTPREQ_POST_MIME:
        request = "POST";
        break;
      case HTTPREQ_PUT:
        request = "PUT";
        break;
      default: /* this should never happen */
      case HTTPREQ_GET:
        request = "GET";
        break;
      case HTTPREQ_HEAD:
        request = "HEAD";
        break;
      }
    }
  }
  *method = request;
  *reqp = httpreq;
}

static CURLcode http_set_aptr_host(struct Curl_easy *data)
{
  struct connectdata *conn = data->conn;
  const char *ptr = NULL;

  curlx_safefree(data->state.http_host);
#ifndef CURL_DISABLE_COOKIES
  curlx_safefree(data->req.cookiehost);
#endif

  만약(Curl_peer_equal(data->state.initial_origin, data->state.origin))
    ptr = Curl_checkheaders(data, STRCONST("Host"));

  만약(ptr) {
#ifndef CURL_DISABLE_COOKIES
    /* If we have a given custom Host: header, we extract the hostname in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the hostname
       is the same as the first one! */
    char *cookiehost;
    CURLcode result = copy_custom_value(ptr, &cookiehost);
    만약(result)
      반환 result;
    만약(!*cookiehost)
      /* ignore empty data */
      curlx_free(cookiehost);
    아니면 {
      /* If the host begins with '[', we start searching for the port after
         the bracket has been closed */
      만약(*cookiehost == '[') {
        char *closingbracket;
        /* since the 'cookiehost' is an allocated memory area that will be
           freed later we cannot increment the pointer */
        memmove(cookiehost, cookiehost + 1, strlen(cookiehost) - 1);
        closingbracket = strchr(cookiehost, ']');
        만약(closingbracket)
          *closingbracket = 0;
      }
      아니면 {
        정수 startsearch = 0;
        char *colon = strchr(cookiehost + startsearch, ':');
        만약(colon)
          *colon = 0; /* The host must not include an embedded port number */
      }
      data->req.cookiehost = cookiehost;
    }
#endif

    만약(!curl_strequal("Host:", ptr)) {
      data->state.http_host = curl_maprintf("Host:%s", &ptr[5]);
      만약(!data->state.http_host)
        반환 CURLE_OUT_OF_MEMORY;
    }
  }
  아니면 {
    /* This is the  HTTP Host: header, so we want
     * - for IPv6 origins: "[ipv6-address]" where the IPv6 address is
     *  found in origin->hostname, stripped of zoneid/scopeid.
     * - the (IDN converted) origin->hostname (DNS name or IPv4) otherwise.
     * Note: zoneid/scopeid  only applies to local routing and has no
     * meaning on the remote HTTP server (eg. would confuse it). */
    bool ipv6 = (bool)data->state.origin->ipv6;
    struct dynbuf tmp;
    size_t hlen;
    CURLcode result;

    curlx_dyn_init(&tmp, DYN_HTTP_REQUEST);
    result = curlx_dyn_addn(&tmp, STRCONST("Host: "));
    만약(!result && ipv6)
      result = curlx_dyn_addn(&tmp, STRCONST("["));
    만약(!result)
      result = curlx_dyn_add(&tmp, data->state.origin->hostname);
    만약(!result && ipv6)
      result = curlx_dyn_addn(&tmp, STRCONST("]"));
    만약(!result &&
       ((data->state.origin->port != data->state.origin->scheme->defport) ||
       (data->state.origin->scheme->family != conn->scheme->family))) {
      result = curlx_dyn_addf(&tmp, ":%u", data->state.origin->port);
    }

    data->state.http_host = result ? NULL : curlx_dyn_take(&tmp, &hlen);
    curlx_dyn_free(&tmp);
    반환 result;
  }
  반환 CURLE_OK;
}

/*
 * Append the request-target to the HTTP request
 */
static CURLcode http_target(struct Curl_easy *data,
                            struct dynbuf *r)
{
  CURLcode result = CURLE_OK;
  const char *path = data->state.up.path;
  const char *query = data->state.up.query;
#ifndef CURL_DISABLE_PROXY
  struct connectdata *conn = data->conn;
#endif

  만약(CURL_EASY_STR(data, STRING_TARGET)) {
    path = CURL_EASY_STR(data, STRING_TARGET);
    query = NULL;
  }

#ifndef CURL_DISABLE_PROXY
  만약(conn->bits.origin_is_proxy) {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL, but if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the decoded hostname! */

    /* and no fragment part */
    CURLUcode uc;
    char *url;
    CURLU *h = curl_url_dup(data->state.uh);
    만약(!h)
      반환 CURLE_OUT_OF_MEMORY;

    만약(!data->state.origin->ipv6 &&
       (data->state.origin->user_hostname != data->state.origin->hostname)) {
      uc = curl_url_set(h, CURLUPART_HOST, data->state.origin->hostname, 0);
      만약(uc) {
        curl_url_cleanup(h);
        반환 CURLE_OUT_OF_MEMORY;
      }
    }
    uc = curl_url_set(h, CURLUPART_FRAGMENT, NULL, 0);
    만약(uc) {
      curl_url_cleanup(h);
      반환 CURLE_OUT_OF_MEMORY;
    }

    만약(data->state.origin->scheme == &Curl_scheme_http) {
      /* when getting HTTP, we do not want the userinfo the URL */
      uc = curl_url_set(h, CURLUPART_USER, NULL, 0);
      만약(uc) {
        curl_url_cleanup(h);
        반환 CURLE_OUT_OF_MEMORY;
      }
      uc = curl_url_set(h, CURLUPART_PASSWORD, NULL, 0);
      만약(uc) {
        curl_url_cleanup(h);
        반환 CURLE_OUT_OF_MEMORY;
      }
    }
    아니면 만약(data->state.creds && (data->state.creds->source != CREDS_URL)) {
        /* credentials not from the URL need to be set */
      uc = curl_url_set(h, CURLUPART_USER,
                        data->state.creds->user, CURLU_URLENCODE);
      만약(!uc)
        uc = curl_url_set(h, CURLUPART_PASSWORD,
                          data->state.creds->passwd, CURLU_URLENCODE);
      만약(uc) {
        curl_url_cleanup(h);
        반환 Curl_uc_to_curlcode(uc);
      }
    }

    /* Extract the URL to use in the request. */
    uc = curl_url_get(h, CURLUPART_URL, &url, CURLU_NO_DEFAULT_PORT);
    만약(uc) {
      curl_url_cleanup(h);
      반환 CURLE_OUT_OF_MEMORY;
    }

    curl_url_cleanup(h);

    /* target or URL */
    result = curlx_dyn_add(r, CURL_EASY_STR(data, STRING_TARGET) ?
      CURL_EASY_STR(data, STRING_TARGET) : url);
    curlx_free(url);
    만약(result)
      반환 result;

    만약((data->state.origin->scheme == &Curl_scheme_ftp) &&
       data->set.proxy_transfer_mode) {
      /* when doing ftp, append ;type=<a|i> if not present */
      size_t len = strlen(path);
      bool type_present = FALSE;
      만약((len >= 7) && !memcmp(&path[len - 7], ";type=", 6)) {
        switch(Curl_raw_toupper(path[len - 1])) {
        case 'A':
        case 'D':
        case 'I':
          type_present = TRUE;
          break;
        }
      }
      만약(!type_present) {
        result = curlx_dyn_addf(r, ";type=%c",
                                data->state.prefer_ascii ? 'a' : 'i');
        만약(result)
          반환 result;
      }
    }
  }

  아니면
#endif
  {
    result = curlx_dyn_add(r, path);
    만약(result)
      반환 result;
    만약(query)
      result = curlx_dyn_addf(r, "?%s", query);
  }

  반환 result;
}

#만약 !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
static CURLcode set_post_reader(struct Curl_easy *data, Curl_HttpReq httpreq)
{
  CURLcode result;

  switch(httpreq) {
#ifndef CURL_DISABLE_MIME
  case HTTPREQ_POST_MIME:
    data->state.mimepost = data->set.mimepostp;
    break;
#endif
#ifndef CURL_DISABLE_FORM_API
  case HTTPREQ_POST_FORM:
    /* Convert the form structure into a mime structure, then keep
       the conversion */
    만약(!data->state.formp) {
      data->state.formp = curlx_calloc(1, sizeof(curl_mimepart));
      만약(!data->state.formp)
        반환 CURLE_OUT_OF_MEMORY;
      Curl_mime_cleanpart(data->state.formp);
      result = Curl_getformdata(data, data->state.formp, data->set.httppost,
                                data->state.fread_func);
      만약(result) {
        curlx_safefree(data->state.formp);
        반환 result;
      }
      data->state.mimepost = data->state.formp;
    }
    break;
#endif
  default:
    data->state.mimepost = NULL;
    break;
  }

  switch(httpreq) {
  case HTTPREQ_POST_FORM:
  case HTTPREQ_POST_MIME:
    /* This is form posting using mime data. */
#ifndef CURL_DISABLE_MIME
    만약(data->state.mimepost) {
      const char *cthdr = Curl_checkheaders(data, STRCONST("Content-Type"));

      /* Read and seek body only. */
      data->state.mimepost->flags |= MIME_BODY_ONLY;

      /* Prepare the mime structure headers & set content type. */

      만약(cthdr)
        위해(cthdr += 13; *cthdr == ' '; cthdr++)
          ;
      아니면 만약(data->state.mimepost->kind == MIMEKIND_MULTIPART)
        cthdr = "multipart/form-data";

      curl_mime_headers(data->state.mimepost, data->set.headers, 0);
      result = Curl_mime_prepare_headers(data, data->state.mimepost, cthdr,
                                         NULL, MIMESTRATEGY_FORM);
      만약(result)
        반환 result;
      curl_mime_headers(data->state.mimepost, NULL, 0);
      result = Curl_creader_set_mime(data, data->state.mimepost);
      만약(result)
        반환 result;
    }
    아니면
#endif
    {
      result = Curl_creader_set_null(data);
    }
    data->state.infilesize = Curl_creader_total_length(data);
    반환 result;

  default:
    반환 Curl_creader_set_null(data);
  }
  /* never reached */
}
#endif

static CURLcode set_reader(struct Curl_easy *data, Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
  curl_off_t postsize = data->state.infilesize;

  DEBUGASSERT(data->conn);

  만약(data->req.authneg) {
    반환 Curl_creader_set_null(data);
  }

  switch(httpreq) {
  case HTTPREQ_PUT: /* Let's PUT the data to the server! */
    반환 postsize ? Curl_creader_set_fread(data, postsize) :
      Curl_creader_set_null(data);

#만약 !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  case HTTPREQ_POST_FORM:
  case HTTPREQ_POST_MIME:
    반환 set_post_reader(data, httpreq);
#endif

  case HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */
    /* the size of the post body */
    만약(!postsize) {
      result = Curl_creader_set_null(data);
    }
    아니면 만약(data->set.postfields) {
      size_t plen = curlx_sotouz_range(postsize, 0, SIZE_MAX);
      만약(plen == SIZE_MAX)
        반환 CURLE_OUT_OF_MEMORY;
      아니면 만약(plen)
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      아니면
        result = Curl_creader_set_null(data);
    }
    아니면 {
      /* we read the bytes from the callback. In case "chunked" encoding
       * is forced by the application, we disregard `postsize`. This is
       * a backward compatibility decision to earlier versions where
       * chunking disregarded this. See issue #13229. */
      bool chunked = FALSE;
      char *ptr = Curl_checkheaders(data, STRCONST("Transfer-Encoding"));
      만약(ptr) {
        /* Some kind of TE is requested, check if 'chunked' is chosen */
        chunked = Curl_compareheader(ptr, STRCONST("Transfer-Encoding:"),
                                     STRCONST("chunked"));
      }
      result = Curl_creader_set_fread(data, chunked ? -1 : postsize);
    }
    반환 result;

  default:
    /* HTTP GET/HEAD download, has no body, needs no Content-Length */
    data->state.infilesize = 0;
    반환 Curl_creader_set_null(data);
  }
  /* not reached */
}

static CURLcode http_resume(struct Curl_easy *data, Curl_HttpReq httpreq)
{
  만약((HTTPREQ_POST == httpreq || HTTPREQ_POST_FORM == httpreq ||
      HTTPREQ_POST_MIME == httpreq || HTTPREQ_PUT == httpreq) &&
     data->state.resume_from) {
    failf(data, "HTTP upload cannot be resumed");
    반환 CURLE_BAD_FUNCTION_ARGUMENT;
  }
  반환 CURLE_OK;
}

static CURLcode http_req_set_TE(struct Curl_easy *data,
                                struct dynbuf *req,
                                정수 httpversion)
{
  CURLcode result = CURLE_OK;
  const char *ptr;

  ptr = Curl_checkheaders(data, STRCONST("Transfer-Encoding"));
  만약(ptr) {
    /* Some kind of TE is requested, check if 'chunked' is chosen */
    data->req.upload_chunky =
      Curl_compareheader(ptr,
                         STRCONST("Transfer-Encoding:"), STRCONST("chunked"));
    만약(data->req.upload_chunky && (httpversion >= 20)) {
      infof(data, "suppressing chunked transfer encoding on connection "
            "using HTTP version 2 or higher");
      data->req.upload_chunky = FALSE;
    }
  }
  아니면 {
    curl_off_t req_clen = Curl_creader_total_length(data);

    만약(req_clen < 0) {
      /* indeterminate request content length */
      만약(httpversion > 10) {
        /* On HTTP/1.1, enable chunked, on HTTP/2 and later we do not
         * need it */
        data->req.upload_chunky = (httpversion < 20);
      }
      아니면 {
        failf(data, "Chunky upload is not supported by HTTP 1.0");
        반환 CURLE_UPLOAD_FAILED;
      }
    }
    아니면 {
      /* else, no chunky upload */
      data->req.upload_chunky = FALSE;
    }

    만약(data->req.upload_chunky)
      result = curlx_dyn_add(req, "Transfer-Encoding: chunked\r\n");
  }
  반환 result;
}

static CURLcode addexpect(struct Curl_easy *data, struct dynbuf *r,
                          정수 httpversion, bool *announced_exp100)
{
  CURLcode result;
  char *ptr;

  *announced_exp100 = FALSE;
  /* Avoid Expect: 100-continue if Upgrade: is used */
  만약(data->req.upgr101 != UPGR101_NONE)
    반환 CURLE_OK;

  /* For really small puts we do not use Expect: headers at all, and for
     the somewhat bigger ones we allow the app to disable it. Make
     sure that the expect100header is always set to the preferred value
     here. */
  ptr = Curl_checkheaders(data, STRCONST("Expect"));
  만약(ptr) {
    *announced_exp100 =
      Curl_compareheader(ptr, STRCONST("Expect:"), STRCONST("100-continue"));
  }
  아니면 만약(!data->state.disableexpect && (httpversion == 11)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    curl_off_t client_len = Curl_creader_client_length(data);
    만약(client_len > EXPECT_100_THRESHOLD || client_len < 0) {
      result = curlx_dyn_addn(r, STRCONST("Expect: 100-continue\r\n"));
      만약(result)
        반환 result;
      *announced_exp100 = TRUE;
    }
  }
  반환 CURLE_OK;
}

static CURLcode http_add_content_hds(struct Curl_easy *data,
                                     struct dynbuf *r,
                                     정수 httpversion,
                                     Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
  curl_off_t req_clen;
  bool announced_exp100 = FALSE;

  DEBUGASSERT(data->conn);
  만약(data->req.upload_chunky) {
    result = Curl_httpchunk_add_reader(data);
    만약(result)
      반환 result;
  }

  /* Get the request body length that has been set up */
  req_clen = Curl_creader_total_length(data);
  switch(httpreq) {
  case HTTPREQ_PUT:
  case HTTPREQ_POST:
#만약 !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  case HTTPREQ_POST_FORM:
  case HTTPREQ_POST_MIME:
#endif
    /* We only set Content-Length and allow a custom Content-Length if
       we do not upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length).
       We do not override a custom "Content-Length" header, but during
       authentication negotiation that header is suppressed.
     */
    만약(req_clen >= 0 && !data->req.upload_chunky &&
       (data->req.authneg ||
        !Curl_checkheaders(data, STRCONST("Content-Length")))) {
      /* we allow replacing this header if not during auth negotiation,
         although it is not wise to actually set your own */
      result = curlx_dyn_addf(r, "Content-Length: %" FMT_OFF_T "\r\n",
                              req_clen);
    }
    만약(result)
      goto out;

#ifndef CURL_DISABLE_MIME
    /* Output mime-generated headers. */
    만약(data->state.mimepost &&
       ((httpreq == HTTPREQ_POST_FORM) || (httpreq == HTTPREQ_POST_MIME))) {
      struct curl_slist *hdr;

      위해(hdr = data->state.mimepost->curlheaders; hdr; hdr = hdr->next) {
        result = curlx_dyn_addf(r, "%s\r\n", hdr->data);
        만약(result)
          goto out;
      }
    }
#endif
    만약(httpreq == HTTPREQ_POST &&
       !Curl_checkheaders(data, STRCONST("Content-Type"))) {
      result = curlx_dyn_addn(r, STRCONST("Content-Type: application/"
                                          "x-www-form-urlencoded\r\n"));
      만약(result)
        goto out;
    }
    result = addexpect(data, r, httpversion, &announced_exp100);
    만약(result)
      goto out;
    break;
  default:
    break;
  }

  Curl_pgrsSetUploadSize(data, req_clen);
  만약(announced_exp100)
    result = http_exp100_add_reader(data);

out:
  반환 result;
}

#ifndef CURL_DISABLE_COOKIES

static CURLcode http_cookies(struct Curl_easy *data,
                             struct dynbuf *r)
{
  CURLcode result = CURLE_OK;
  const char *addcookies = NULL;
  bool linecap = FALSE;
  만약(CURL_EASY_STR(data, STRING_COOKIE) &&
     !Curl_checkheaders(data, STRCONST("Cookie")) &&
     Curl_auth_allowed_to_host(data))
    addcookies = CURL_EASY_STR(data, STRING_COOKIE);

  만약(data->cookies || addcookies) {
    struct Curl_llist list;
    정수 count = 0;

    만약(data->cookies && data->state.cookie_engine) {
      bool okay;
      const char *host = data->req.cookiehost ?
        data->req.cookiehost : data->state.origin->hostname;
      Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
      result = Curl_cookie_getlist(data, &okay, host, &list);
      만약(!result && okay) {
        struct Curl_llist_node *n;
        size_t clen = 8; /* hold the size of the generated Cookie: header */

        /* loop through all cookies that matched */
        위해(n = Curl_llist_head(&list); n; n = Curl_node_next(n)) {
          struct Cookie *co = Curl_node_elem(n);
          만약(co->value) {
            size_t add;
            만약(!count) {
              result = curlx_dyn_addn(r, STRCONST("Cookie: "));
              만약(result)
                break;
            }
            add = strlen(co->name) + strlen(co->value) + 1;
            만약(clen + add >= MAX_COOKIE_HEADER_LEN) {
              infof(data, "Restricted outgoing cookies due to header size, "
                    "'%s' not sent", co->name);
              linecap = TRUE;
              break;
            }
            result = curlx_dyn_addf(r, "%s%s=%s", count ? "; " : "",
                                    co->name, co->value);
            만약(result)
              break;
            clen += add + (count ? 2 : 0);
            count++;
          }
        }
        Curl_llist_destroy(&list, NULL);
      }
      Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    }
    만약(addcookies && !result && !linecap) {
      만약(!count)
        result = curlx_dyn_addn(r, STRCONST("Cookie: "));
      만약(!result) {
        result = curlx_dyn_addf(r, "%s%s", count ? "; " : "", addcookies);
        count++;
      }
    }
    만약(count && !result)
      result = curlx_dyn_addn(r, STRCONST("\r\n"));

    만약(result)
      반환 result;
  }
  반환 result;
}
#아니면
#define http_cookies(a, b) CURLE_OK
#endif

static CURLcode http_range(struct Curl_easy *data,
                           Curl_HttpReq httpreq)
{
  만약(data->state.use_range) {
    /*
     * A range is selected. We use different headers whether we are downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    만약(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_HEAD)) &&
       !Curl_checkheaders(data, STRCONST("Range"))) {
      /* if a line like this was already allocated, free the previous one */
      curlx_free(data->state.rangeline);
      data->state.rangeline = curl_maprintf("Range: bytes=%s\r\n",
                                                 data->state.range);
      만약(!data->state.rangeline)
        반환 CURLE_OUT_OF_MEMORY;
    }
    아니면 만약((httpreq == HTTPREQ_POST || httpreq == HTTPREQ_PUT) &&
            !Curl_checkheaders(data, STRCONST("Content-Range"))) {
      curl_off_t req_clen = Curl_creader_total_length(data);
      /* if a line like this was already allocated, free the previous one */
      curlx_free(data->state.rangeline);

      만약(data->set.set_resume_from < 0) {
        /* Upload resume was asked for, but we do not know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes 0-%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n", req_clen - 1, req_clen);
      }
      아니면 만약(data->state.resume_from) {
        /* This is because "resume" was selected */
        /* Not sure if we want to send this header during authentication
         * negotiation, but test1084 checks for it. In which case we have a
         * "null" client reader installed that gives an unexpected length. */
        curl_off_t total_len = data->req.authneg ?
                               data->state.infilesize :
                               (data->state.resume_from + req_clen);
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes %s%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n",
                        data->state.range, total_len - 1, total_len);
      }
      아니면 {
        /* Range was selected and then we pass the incoming range and append
           total size */
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes %s/%" FMT_OFF_T "\r\n",
                        data->state.range, req_clen);
      }
      만약(!data->state.rangeline)
        반환 CURLE_OUT_OF_MEMORY;
    }
  }
  반환 CURLE_OK;
}

static CURLcode http_firstwrite(struct Curl_easy *data)
{
  struct connectdata *conn = data->conn;
  struct SingleRequest *k = &data->req;

  만약(data->req.newurl) {
    만약(conn->bits.close) {
      /* Abort after the headers if "follow Location" is set
         and we are set to close anyway. */
      CURL_REQ_CLEAR_RECV(data);
      k->done = TRUE;
      반환 CURLE_OK;
    }
    /* We have a new URL to load, but since we want to be able to reuse this
       connection properly, we read the full response in "ignore more" */
    k->ignorebody = TRUE;
    infof(data, "Ignoring the response-body");
  }
  만약(data->state.resume_from && !k->content_range &&
     (data->state.httpreq == HTTPREQ_GET) &&
     !k->ignorebody) {

    만약(k->size == data->state.resume_from) {
      /* The resume point is at the end of file, consider this fine even if it
         does not allow resume from here. */
      infof(data, "The entire document is already downloaded");
      streamclose(conn);
      /* Abort download */
      CURL_REQ_CLEAR_RECV(data);
      k->done = TRUE;
      반환 CURLE_OK;
    }

    /* we wanted to resume a download, although the server does not seem to
     * support this and we did this with a GET (if it was not a GET we did a
     * POST or PUT resume) */
    failf(data, "HTTP server does not seem to support "
          "byte ranges. Cannot resume.");
    반환 CURLE_RANGE_ERROR;
  }

  만약(data->set.timecondition && !data->state.range &&
     /* A time condition has been set AND no ranges have been requested. This
        seems to be what chapter 13.3.4 of RFC 2616 defines to be the correct
        action for an HTTP/1.1 client */
     !Curl_meets_timecondition(data, k->timeofdoc)) {
    k->done = TRUE;
    /* We are simulating an HTTP 304 from server so we return
       what should have been returned from the server */
    data->info.httpcode = 304;
    infof(data, "Simulate an HTTP 304 response");
    /* we abort the transfer before it is completed == we ruin the
       reuse ability. Close the connection */
    streamclose(conn);
    반환 CURLE_OK;
  } /* we have a time condition */

  반환 CURLE_OK;
}

static CURLcode http_check_new_conn(struct Curl_easy *data)
{
  struct connectdata *conn = data->conn;
  const char *info_version = NULL;
  const char *alpn;
  CURLcode result;

  alpn = Curl_conn_get_alpn_negotiated(data, conn);
  만약(alpn && !strcmp("h3", alpn)) {
#ifndef CURL_DISABLE_PROXY
    만약(!conn->bits.origin_is_proxy)
#endif
      DEBUGASSERT(Curl_conn_http_version(data, conn) == 30);
    info_version = "HTTP/3";
  }
  아니면 만약(alpn && !strcmp("h2", alpn)) {
#ifndef CURL_DISABLE_PROXY
    만약((Curl_conn_http_version(data, conn) != 20) &&
       conn->bits.origin_is_proxy) {
      result = Curl_http2_switch(data);
      만약(result)
        반환 result;
    }
    아니면
#endif
    DEBUGASSERT(Curl_conn_http_version(data, conn) == 20);
    info_version = "HTTP/2";
  }
  아니면 {
    /* Check if user wants to use HTTP/2 with clear TCP */
    만약(Curl_http2_may_switch(data)) {
      DEBUGF(infof(data, "HTTP/2 over clean TCP"));
      result = Curl_http2_switch(data);
      만약(result)
        반환 result;
      info_version = "HTTP/2";
      /* There is no ALPN here, but the connection is now definitely h2 */
      conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(conn);
    }
    아니면
      info_version = "HTTP/1.x";
  }

  만약(info_version)
    infof(data, "using %s", info_version);
  반환 CURLE_OK;
}

static CURLcode http_add_connection_hd(struct Curl_easy *data,
                                       struct dynbuf *req)
{
  struct curl_slist *head;
  const char *sep = "Connection: ";
  CURLcode result = CURLE_OK;
  size_t rlen = curlx_dyn_len(req);
  bool skip;

  /* Add the 1st custom "Connection: " header, if there is one */
  위해(head = data->set.headers; head; head = head->next) {
    만약(curl_strnequal(head->data, "Connection", 10) &&
       Curl_headersep(head->data[10]) &&
       !http_header_is_empty(head->data)) {
      char *value;
      result = copy_custom_value(head->data, &value);
      만약(result)
        반환 result;
      result = curlx_dyn_addf(req, "%s%s", sep, value);
      sep = ", ";
      curlx_free(value);
      break; /* leave, having added 1st one */
    }
  }

  /* add our internal Connection: header values, if we have any */
  만약(!result && data->state.http_hd_te) {
    result = curlx_dyn_addf(req, "%s%s", sep, "TE");
    sep = ", ";
  }
  만약(!result && data->state.http_hd_upgrade) {
    result = curlx_dyn_addf(req, "%s%s", sep, "Upgrade");
    sep = ", ";
  }
  만약(!result && data->state.http_hd_h2_settings) {
    result = curlx_dyn_addf(req, "%s%s", sep, "HTTP2-Settings");
  }
  만약(!result && (rlen < curlx_dyn_len(req)))
    result = curlx_dyn_addn(req, STRCONST("\r\n"));
  만약(result)
    반환 result;

  /* Add all user-defined Connection: headers after the first */
  skip = TRUE;
  위해(head = data->set.headers; head; head = head->next) {
    만약(curl_strnequal(head->data, "Connection", 10) &&
       Curl_headersep(head->data[10]) &&
       !http_header_is_empty(head->data)) {
      만약(skip) {
        skip = FALSE;
        continue;
      }
      result = curlx_dyn_addf(req, "%s\r\n", head->data);
      만약(result)
        반환 result;
    }
  }

  반환 CURLE_OK;
}

/* Header identifier in order we send them by default */
typedef enum {
  H1_HD_REQUEST,
  H1_HD_HOST,
#ifndef CURL_DISABLE_PROXY
  H1_HD_PROXY_AUTH,
#endif
  H1_HD_AUTH,
  H1_HD_RANGE,
  H1_HD_USER_AGENT,
  H1_HD_ACCEPT,
  H1_HD_TE,
  H1_HD_ACCEPT_ENCODING,
  H1_HD_REFERER,
#ifndef CURL_DISABLE_PROXY
  H1_HD_PROXY_CONNECTION,
#endif
  H1_HD_TRANSFER_ENCODING,
#ifndef CURL_DISABLE_ALTSVC
  H1_HD_ALT_USED,
#endif
  H1_HD_UPGRADE,
  H1_HD_COOKIES,
  H1_HD_CONDITIONALS,
  H1_HD_CUSTOM,
  H1_HD_CONTENT,
  H1_HD_CONNECTION,
  H1_HD_LAST  /* the last, empty header line */
} http_hd_t;

static CURLcode http_add_hd(struct Curl_easy *data,
                            struct dynbuf *req,
                            http_hd_t id,
                            unsigned char httpversion,
                            const char *method,
                            Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
#만약 !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_PROXY) || \
  !defined(CURL_DISABLE_WEBSOCKETS)
  struct connectdata *conn = data->conn;
#endif
  switch(id) {
  case H1_HD_REQUEST:
    /* add the main request stuff */
    /* GET/HEAD/POST/PUT */
    result = curlx_dyn_addf(req, "%s ", method);
    만약(!result)
      result = http_target(data, req);
    만약(!result)
      result = curlx_dyn_addf(req, " HTTP/%s\r\n",
                              get_http_string(httpversion));
    break;

  case H1_HD_HOST:
    만약(data->state.http_host) {
      result = curlx_dyn_add(req, data->state.http_host);
      만약(!result)
        result = curlx_dyn_addn(req, STRCONST("\r\n"));
    }
    break;

#ifndef CURL_DISABLE_PROXY
  case H1_HD_PROXY_AUTH:
    만약(data->req.hd_proxy_auth)
      result = curlx_dyn_add(req, data->req.hd_proxy_auth);
    break;
#endif

  case H1_HD_AUTH:
    만약(data->req.hd_auth)
      result = curlx_dyn_add(req, data->req.hd_auth);
    break;

  case H1_HD_RANGE:
    만약(data->state.use_range && data->state.rangeline)
      result = curlx_dyn_add(req, data->state.rangeline);
    break;

  case H1_HD_USER_AGENT: {
    const char *ua = CURL_EASY_STR(data, STRING_USERAGENT);
    만약(ua && *ua && !Curl_checkheaders(data, STRCONST("User-Agent")))
      result = curlx_dyn_addf(req, "User-Agent: %s\r\n", ua);
    break;
  }

  case H1_HD_ACCEPT:
    만약(!Curl_checkheaders(data, STRCONST("Accept")))
      result = curlx_dyn_add(req, "Accept: */*\r\n");
    break;

  case H1_HD_TE:
#ifdef HAVE_LIBZ
    만약(!Curl_checkheaders(data, STRCONST("TE")) &&
       data->set.http_transfer_encoding) {
      data->state.http_hd_te = TRUE;
      result = curlx_dyn_add(req, "TE: gzip\r\n");
    }
#endif
    break;

  case H1_HD_ACCEPT_ENCODING: {
    const char *enc = CURL_EASY_STR(data, STRING_ENCODING);
    만약(enc && !Curl_checkheaders(data, STRCONST("Accept-Encoding")))
      result = curlx_dyn_addf(req, "Accept-Encoding: %s\r\n", enc);
    break;
  }

  case H1_HD_REFERER:
    만약(Curl_bufref_ptr(&data->state.referer) &&
       !Curl_checkheaders(data, STRCONST("Referer")))
      result = curlx_dyn_addf(req, "Referer: %s\r\n",
                              Curl_bufref_ptr(&data->state.referer));
    break;

#ifndef CURL_DISABLE_PROXY
  case H1_HD_PROXY_CONNECTION:
    만약(conn->bits.origin_is_proxy &&
       !Curl_checkheaders(data, STRCONST("Proxy-Connection")) &&
       !Curl_checkProxyheaders(data, data->conn, STRCONST("Proxy-Connection")))
      result = curlx_dyn_add(req, "Proxy-Connection: Keep-Alive\r\n");
    break;
#endif

  case H1_HD_TRANSFER_ENCODING:
    result = http_req_set_TE(data, req, httpversion);
    break;

#ifndef CURL_DISABLE_ALTSVC
  case H1_HD_ALT_USED:
    만약(conn->bits.altused && conn->via_peer &&
       !Curl_checkheaders(data, STRCONST("Alt-Used")))
      result = curlx_dyn_addf(req, "Alt-Used: %s:%u\r\n",
                              conn->via_peer->hostname, conn->via_peer->port);
    break;
#endif

  case H1_HD_UPGRADE:
    만약(!Curl_conn_is_ssl(data->conn, FIRSTSOCKET) && (httpversion < 20) &&
       (data->state.http_neg.wanted & CURL_HTTP_V2x) &&
       data->state.http_neg.h2_upgrade) {
      /* append HTTP2 upgrade magic stuff to the HTTP request if it is not done
         over SSL */
      result = Curl_http2_request_upgrade(req, data);
    }
#ifndef CURL_DISABLE_WEBSOCKETS
    만약(!result && conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
      result = Curl_ws_request(data, req);
#endif
    break;

  case H1_HD_COOKIES:
    result = http_cookies(data, req);
    break;

  case H1_HD_CONDITIONALS:
    result = Curl_add_timecondition(data, req);
    break;

  case H1_HD_CUSTOM:
    result = Curl_add_custom_headers(data, FALSE, httpversion, req);
    break;

  case H1_HD_CONTENT:
    result = http_add_content_hds(data, req, httpversion, httpreq);
    break;

  case H1_HD_CONNECTION: {
    result = http_add_connection_hd(data, req);
    break;
  }

  case H1_HD_LAST:
    result = curlx_dyn_addn(req, STRCONST("\r\n"));
    break;
  }
  반환 result;
}

/*
 * Curl_http() gets called from the generic multi_do() function when an HTTP
 * request is to be performed. This creates and sends a properly constructed
 * HTTP request.
 */
CURLcode Curl_http(struct Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  Curl_HttpReq httpreq;
  const char *method;
  struct dynbuf req;
  unsigned char httpversion;
  size_t hd_id;

  /* Always consider the DO phase done after this function call, even if there
     may be parts of the request that are not yet sent, since we can deal with
     the rest of the request in the PERFORM phase. */
  *done = TRUE;
  /* initialize a dynamic send-buffer */
  curlx_dyn_init(&req, DYN_HTTP_REQUEST);
  /* make sure the header buffer is reset - if there are leftovers from a
     previous transfer */
  curlx_dyn_reset(&data->state.headerb);
  data->state.maybe_folded = FALSE;

  만약(!data->conn->bits.reuse) {
    result = http_check_new_conn(data);
    만약(result)
      goto out;
  }

  /* Add collecting of headers written to client. For a new connection,
   * we might have done that already, but reuse
   * or multiplex needs it here as well. */
  result = Curl_headers_init(data);
  만약(result)
    goto out;

  data->state.http_hd_te = FALSE;
  data->state.http_hd_upgrade = FALSE;
  data->state.http_hd_h2_settings = FALSE;

  /* what kind of request do we need to send? */
  Curl_http_method(data, &method, &httpreq);

  /* select host to send */
  result = http_set_aptr_host(data);
  /* setup the authentication headers, how that method and host are known */
  만약(!result)
    result = Curl_http_output_auth(data, data->conn, method, httpreq,
                                   data->state.up.path,
                                   data->state.up.query, FALSE);
  /* Setup input reader, resume information and ranges */
  만약(!result)
    result = set_reader(data, httpreq);
  만약(!result)
    result = http_resume(data, httpreq);
  만약(!result)
    result = http_range(data, httpreq);
  만약(result)
    goto out;

  httpversion = http_request_version(data);
  /* Add request line and all headers to `req` */
  위해(hd_id = 0; hd_id <= H1_HD_LAST; ++hd_id) {
    result = http_add_hd(data, &req, (http_hd_t)hd_id,
                         httpversion, method, httpreq);
    만약(result)
      goto out;
  }

  /* setup variables for the upcoming transfer and send */
  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);
  result = Curl_req_send(data, &req, httpversion);

  만약((httpversion >= 20) && data->req.upload_chunky)
    /* upload_chunky was set above to set up the request in a chunky fashion,
       but is disabled here again to avoid that the chunked encoded version is
       actually used when sending the request body over h2 */
    data->req.upload_chunky = FALSE;

out:
  만약(result == CURLE_TOO_LARGE)
    failf(data, "HTTP request too large");

  curlx_dyn_free(&req);
  반환 result;
}

typedef enum {
  STATUS_UNKNOWN, /* not enough data to tell yet */
  STATUS_DONE, /* a status line was read */
  STATUS_BAD /* not a status line */
} statusline;

/* Check a string for a prefix. Check no more than 'len' bytes */
static bool checkprefixmax(const char *prefix, const char *buffer, size_t len)
{
  size_t ch = CURLMIN(strlen(prefix), len);
  반환 curl_strnequal(prefix, buffer, ch);
}

/*
 * checkhttpprefix()
 *
 * Returns TRUE if member of the list matches prefix of string
 */
static statusline checkhttpprefix(struct Curl_easy *data,
                                  const char *s, size_t len)
{
  struct curl_slist *head = data->set.http200aliases;
  statusline rc = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;

  동안(head) {
    만약(checkprefixmax(head->data, s, len)) {
      rc = onmatch;
      break;
    }
    head = head->next;
  }

  만약((rc != STATUS_DONE) && checkprefixmax("HTTP/", s, len))
    rc = onmatch;

  반환 rc;
}

#ifndef CURL_DISABLE_RTSP
static statusline checkrtspprefix(struct Curl_easy *data,
                                  const char *s, size_t len)
{
  statusline status = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;
  (void)data;
  만약(checkprefixmax("RTSP/", s, len))
    status = onmatch;

  반환 status;
}
#endif /* CURL_DISABLE_RTSP */

static statusline checkprotoprefix(struct Curl_easy *data,
                                   struct connectdata *conn,
                                   const char *s, size_t len)
{
#ifndef CURL_DISABLE_RTSP
  만약(conn->scheme->protocol & CURLPROTO_RTSP)
    반환 checkrtspprefix(data, s, len);
#아니면
  (void)conn;
#endif /* CURL_DISABLE_RTSP */

  반환 checkhttpprefix(data, s, len);
}

/* HTTP header has field name `n` (a string constant) */
#define HD_IS(hd, hdlen, n) \
  (((hdlen) >= (sizeof(n) - 1)) && curl_strnequal(n, hd, sizeof(n) - 1))

#define HD_VAL(hd, hdlen, n) \
  ((((hdlen) >= (sizeof(n) - 1)) && (hd) && \
    curl_strnequal(n, hd, sizeof(n) - 1)) ? ((hd) + (sizeof(n) - 1)) : NULL)

/* HTTP header has field name `n` (a string constant) and contains `v`
 * (a string constant) in its value(s) */
#define HD_IS_AND_SAYS(hd, hdlen, n, v) \
  (HD_IS(hd, hdlen, n) && \
   ((hdlen) > ((sizeof(n) - 1) + (sizeof(v) - 1))) && \
   Curl_compareheader(hd, STRCONST(n), STRCONST(v)))

/*
 * http_header_a() parses a single response header starting with A.
 */
static CURLcode http_header_a(struct Curl_easy *data,
                              const char *hd, size_t hdlen)
{
#ifndef CURL_DISABLE_ALTSVC
  const char *v;
  v = (data->asi &&
       (Curl_xfer_is_secure(data) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_ALTSVC_HTTP")
#아니면
        0
#endif
         )) ? HD_VAL(hd, hdlen, "Alt-Svc:") : NULL;
  만약(v) {
    /* the ALPN of the current request */
    struct SingleRequest *k = &data->req;
    enum alpnid id = (k->httpversion == 30) ? ALPN_h3 :
      (k->httpversion == 20) ? ALPN_h2 : ALPN_h1;
    반환 Curl_altsvc_parse(data, data->asi, v, data->state.origin, id);
  }
#아니면
  (void)data;
  (void)hd;
  (void)hdlen;
#endif
  반환 CURLE_OK;
}

/*
 * http_header_c() parses a single response header starting with C.
 */
static CURLcode http_header_c(struct Curl_easy *data,
                              const char *hd, size_t hdlen)
{
  struct connectdata *conn = data->conn;
  struct SingleRequest *k = &data->req;
  const char *v;

  /* Check for Content-Length: header lines to get size. Browsers insist we
     should accept multiple Content-Length headers and that a comma separated
     list also is fine and then we should accept them all as long as they are
     the same value. Different values trigger error.
   */
  v = (!k->http_bodyless && !data->set.ignorecl && !k->ignore_cl) ?
    HD_VAL(hd, hdlen, "Content-Length:") : NULL;
  만약(v) {
    do {
      curl_off_t contentlength;
      정수 offt = curlx_str_numblanks(&v, &contentlength);

      만약(offt == STRE_OVERFLOW) {
        /* out of range */
        만약(data->set.max_filesize) {
          failf(data, "Maximum file size exceeded");
          반환 CURLE_FILESIZE_EXCEEDED;
        }
        streamclose(conn);
        infof(data, "Overflow Content-Length: value");
        반환 CURLE_OK;
      }
      아니면 {
        만약((offt == STRE_OK) &&
           ((k->size == -1) || /* not set to something before */
            (k->size == contentlength))) { /* or the same value */

          k->size = contentlength;
          curlx_str_passblanks(&v);

          /* on a comma, loop and get the next instead */
          만약(!curlx_str_single(&v, ','))
            continue;

          만약(!curlx_str_newline(&v)) {
            k->maxdownload = k->size;
            반환 CURLE_OK;
          }
        }
        /* negative, different value or rubbish - bad HTTP */
        failf(data, "Invalid Content-Length: value");
        반환 CURLE_WEIRD_SERVER_REPLY;
      }
    } 동안(1);
  }
  v = (!k->http_bodyless && CURL_EASY_STR(data, STRING_ENCODING)) ?
    HD_VAL(hd, hdlen, "Content-Encoding:") : NULL;
  만약(v) {
    /*
     * Process Content-Encoding. Look for the values: identity, gzip, deflate,
     * compress, x-gzip and x-compress. x-gzip and x-compress are the same as
     * gzip and compress. (Sec 3.5 RFC 2616). zlib cannot handle compress.
     * Errors are handled further down when the response body is processed
     */
    반환 Curl_build_unencoding_stack(data, v, FALSE);
  }
  /* check for Content-Type: header lines to get the MIME-type */
  v = HD_VAL(hd, hdlen, "Content-Type:");
  만약(v) {
    char *contenttype = Curl_copy_header_value(hd);
    만약(!contenttype)
      반환 CURLE_OUT_OF_MEMORY;
    만약(!*contenttype)
      /* ignore empty data */
      curlx_free(contenttype);
    아니면 {
      curlx_free(data->info.contenttype);
      data->info.contenttype = contenttype;
    }
    반환 CURLE_OK;
  }
  만약((k->httpversion < 20) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "close")) {
    /*
     * [RFC 2616, section 8.1.2.1]
     * "Connection: close" is HTTP/1.1 language and means that
     * the connection will close when this request has been
     * served.
     */
    connclose(conn);
    반환 CURLE_OK;
  }
  만약((k->httpversion == 10) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "keep-alive")) {
    /*
     * An HTTP/1.0 reply with the 'Connection: keep-alive' line
     * tells us the connection will be kept alive for our
     * pleasure. Default action for 1.0 is to close.
     *
     * [RFC2068, section 19.7.1] */
    connkeep(conn);
    infof(data, "HTTP/1.0 connection set to keep alive");
    반환 CURLE_OK;
  }
  v = !k->http_bodyless ? HD_VAL(hd, hdlen, "Content-Range:") : NULL;
  만약(v) {
    /* Content-Range: bytes [num]-
       Content-Range: bytes: [num]-
       Content-Range: [num]-
       Content-Range: [asterisk]/[total]

       The second format was added since Sun's webserver
       JavaWebServer/1.1.1 obviously sends the header this way!
       The third added since some servers use that!
       The fourth means the requested range was unsatisfied.
     */

    const char *ptr = v;

    /* Move forward until first digit or asterisk */
    동안(*ptr && !ISDIGIT(*ptr) && *ptr != '*')
      ptr++;

    /* if it truly stopped on a digit */
    만약(ISDIGIT(*ptr)) {
      만약(!curlx_str_number(&ptr, &k->offset, CURL_OFF_T_MAX) &&
         (data->state.resume_from == k->offset))
        /* we asked for a resume and we got it */
        k->content_range = TRUE;
    }
    아니면 만약(k->httpcode < 300)
      data->state.resume_from = 0; /* get everything */
  }
  반환 CURLE_OK;
}

/*
 * http_header_l() parses a single response header starting with L.
 */
static CURLcode http_header_l(struct Curl_easy *data,
                              const char *hd, size_t hdlen)
{
  struct connectdata *conn = data->conn;
  struct SingleRequest *k = &data->req;
  const char *v = (!k->http_bodyless &&
                   (data->set.timecondition || data->set.get_filetime)) ?
    HD_VAL(hd, hdlen, "Last-Modified:") : NULL;
  만약(v) {
    만약(Curl_getdate_capped(v, &k->timeofdoc))
      k->timeofdoc = 0;
    만약(data->set.get_filetime)
      data->info.filetime = k->timeofdoc;
    반환 CURLE_OK;
  }
  만약(HD_IS(hd, hdlen, "Location:")) {
    /* this is the URL that the server advises us to use instead */
    char *location = Curl_copy_header_value(hd);
    만약(!location)
      반환 CURLE_OUT_OF_MEMORY;
    만약(!*location ||
       (data->req.location && !strcmp(data->req.location, location))) {
      /* ignore empty header, or exact repeat of a previous one */
      curlx_free(location);
      반환 CURLE_OK;
    }
    아니면 {
      /* has value and is not an exact repeat */
      만약(data->req.location) {
        failf(data, "Multiple Location headers");
        curlx_free(location);
        반환 CURLE_WEIRD_SERVER_REPLY;
      }
      data->req.location = location;

      만약((k->httpcode >= 300 && k->httpcode < 400) &&
         data->set.http_follow_mode) {
        CURLcode result;
        DEBUGASSERT(!data->req.newurl);
        data->req.newurl = curlx_strdup(data->req.location); /* clone */
        만약(!data->req.newurl)
          반환 CURLE_OUT_OF_MEMORY;

        /* some cases of POST and PUT etc needs to rewind the data
           stream at this point */
        result = http_perhapsrewind(data, conn);
        만약(result)
          반환 result;

        /* mark the next request as a followed location: */
        data->state.this_is_a_follow = TRUE;
      }
    }
  }
  반환 CURLE_OK;
}

/*
 * http_header_p() parses a single response header starting with P.
 */
static CURLcode http_header_p(struct Curl_easy *data,
                              const char *hd, size_t hdlen)
{
  struct SingleRequest *k = &data->req;

#ifndef CURL_DISABLE_PROXY
  const char *v = HD_VAL(hd, hdlen, "Proxy-Connection:");
  만약(v) {
    struct connectdata *conn = data->conn;
    만약((k->httpversion == 10) && conn->http_proxy.peer &&
       HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "keep-alive")) {
      /*
       * When an HTTP/1.0 reply comes when using a proxy, the
       * 'Proxy-Connection: keep-alive' line tells us the
       * connection will be kept alive for our pleasure.
       * Default action for 1.0 is to close.
       */
      connkeep(conn); /* do not close */
      infof(data, "HTTP/1.0 proxy connection set to keep alive");
    }
    아니면 만약((k->httpversion == 11) && conn->http_proxy.peer &&
            HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "close")) {
      /*
       * We get an HTTP/1.1 response from a proxy and it says it will
       * close down after this transfer.
       */
      connclose(conn);
      infof(data, "HTTP/1.1 proxy connection set close");
    }
    반환 CURLE_OK;
  }
#endif
  만약((407 == k->httpcode) && HD_IS(hd, hdlen, "Proxy-authenticate:")) {
    char *auth;
    CURLcode result;
#ifndef CURL_DISABLE_PROXY
    만약(!data->conn->bits.origin_is_proxy)
      /* this response is not from a forward proxy, not for us */
      반환 CURLE_OK;
#endif
    auth = Curl_copy_header_value(hd);
    result = auth ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    만약(!result) {
      result = Curl_http_input_auth(data, TRUE, auth);
      curlx_free(auth);
    }
    반환 result;
  }
#ifdef USE_SPNEGO
  만약(HD_IS(hd, hdlen, "Persistent-Auth:")) {
    struct connectdata *conn = data->conn;
    struct negotiatedata *negdata = Curl_auth_nego_get(conn, FALSE);
    struct auth *authp = &data->state.authhost;
    만약(!negdata)
      반환 CURLE_OUT_OF_MEMORY;
    만약(authp->picked == CURLAUTH_NEGOTIATE) {
      char *persistentauth = Curl_copy_header_value(hd);
      만약(!persistentauth)
        반환 CURLE_OUT_OF_MEMORY;
      negdata->noauthpersist = !!checkprefix("false", persistentauth);
      negdata->havenoauthpersist = TRUE;
      infof(data, "Negotiate: noauthpersist -> %d, header part: %s",
            negdata->noauthpersist, persistentauth);
      curlx_free(persistentauth);
    }
  }
#endif
  반환 CURLE_OK;
}

/*
 * http_header_r() parses a single response header starting with R.
 */
static CURLcode http_header_r(struct Curl_easy *data,
                              const char *hd, size_t hdlen)
{
  const char *v = HD_VAL(hd, hdlen, "Retry-After:");
  만약(v) {
    /* Retry-After = HTTP-date / delay-seconds */
    curl_off_t retry_after = 0; /* zero for unknown or "now" */
    time_t date = 0;
    curlx_str_passblanks(&v);

    /* try it as a date first, because a date can otherwise start with and
       get treated as a number */
    만약(!Curl_getdate_capped(v, &date)) {
      time_t current = time(NULL);
      만약(date >= current)
        /* convert date to number of seconds into the future */
        retry_after = date - current;
    }
    아니면
      /* Try it as a decimal number, ignore errors */
      (void)curlx_str_number(&v, &retry_after, CURL_OFF_T_MAX);
    /* limit to 6 hours max. this is not documented so that it can be changed
       in the future if necessary. */
    만약(retry_after > 21600)
      retry_after = 21600;
    data->info.retry_after = retry_after;
  }
  반환 CURLE_OK;
}

/*
 * http_header_s() parses a single response header starting with S.
 */
static CURLcode http_header_s(struct Curl_easy *data,
                              const char *hd, size_t hdlen)
{
#만약 !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_HSTS)
  const char *v;
#아니면
  (void)data;
  (void)hd;
  (void)hdlen;
#endif

#ifndef CURL_DISABLE_COOKIES
  v = (data->cookies && data->state.cookie_engine) ?
    HD_VAL(hd, hdlen, "Set-Cookie:") : NULL;
  만약(v) {
    /* If there is a custom-set Host: name, use it here, or else use
     * real peer hostname. */
    const char *host = data->req.cookiehost ?
      data->req.cookiehost : data->state.origin->hostname;
    const unsigned char secure_context = Curl_secure_context(data, host) ?
      COOKIE_SECURE : 0;
    CURLcode result;
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    result = Curl_cookie_add(data, data->cookies, v, host,
                             data->state.up.path,
                             COOKIE_HTTPHEADER | secure_context);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    반환 result;
  }
#endif
#ifndef CURL_DISABLE_HSTS
  /* If enabled, the header is incoming and this is over HTTPS */
  v = (data->hsts &&
       (Curl_xfer_is_secure(data) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_HSTS_HTTP")
#아니면
        0
#endif
         )
    ) ? HD_VAL(hd, hdlen, "Strict-Transport-Security:") : NULL;
  만약(v) {
    CURLcode result = Curl_hsts_parse(
      data->hsts, data->state.origin->hostname, v);
    만약(result) {
      만약(result == CURLE_OUT_OF_MEMORY)
        반환 result;
      infof(data, "Illegal STS header skipped");
    }
#ifdef DEBUGBUILD
    아니면
      infof(data, "Parsed STS header fine (%zu entries)",
            Curl_llist_count(&data->hsts->list));
#endif
  }
#endif

  반환 CURLE_OK;
}

/*
 * http_header_t() parses a single response header starting with T.
 */
static CURLcode http_header_t(struct Curl_easy *data,
                              const char *hd, size_t hdlen)
{
  struct connectdata *conn = data->conn;
  struct SingleRequest *k = &data->req;

  /* RFC 9112, ch. 6.1
   * "Transfer-Encoding MAY be sent in a response to a HEAD request or
   *  in a 304 (Not Modified) response (Section 15.4.5 of [HTTP]) to a
   *  GET request, neither of which includes a message body, to indicate
   *  that the origin server would have applied a transfer coding to the
   *  message body if the request had been an unconditional GET."
   *
   * Read: in these cases the 'Transfer-Encoding' does not apply
   * to any data following the response headers. Do not add any decoders.
   */
  const char *v = (!k->http_bodyless &&
                   (data->state.httpreq != HTTPREQ_HEAD) &&
                   (k->httpcode != 304)) ?
    HD_VAL(hd, hdlen, "Transfer-Encoding:") : NULL;
  만약(v) {
    /* One or more encodings. We check for chunked and/or a compression
       algorithm. */
    CURLcode result = Curl_build_unencoding_stack(data, v, TRUE);
    만약(result)
      반환 result;
    만약(!k->chunk) {
      /* if this is not chunked, only close can signal the end of this
       * transfer as Content-Length is said not to be trusted for
       * transfer-encoding! */
      CURL_TRC_M(data, "HTTP/1.1 transfer-encoding without chunks");
      connclose(conn);
      k->ignore_cl = TRUE;
    }
    반환 CURLE_OK;
  }
  v = HD_VAL(hd, hdlen, "Trailer:");
  만약(v) {
    data->req.resp_trailer = TRUE;
    반환 CURLE_OK;
  }
  반환 CURLE_OK;
}

/*
 * http_header_w() parses a single response header starting with W.
 */
static CURLcode http_header_w(struct Curl_easy *data,
                              const char *hd, size_t hdlen)
{
  struct SingleRequest *k = &data->req;
  CURLcode result = CURLE_OK;

  만약((401 == k->httpcode) && HD_IS(hd, hdlen, "WWW-Authenticate:")) {
    char *auth = Curl_copy_header_value(hd);
    만약(!auth)
      result = CURLE_OUT_OF_MEMORY;
    아니면 {
      result = Curl_http_input_auth(data, FALSE, auth);
      curlx_free(auth);
    }
  }
  반환 result;
}

/*
 * http_header() parses a single response header.
 */
static CURLcode http_header(struct Curl_easy *data,
                            const char *hd, size_t hdlen)
{
  CURLcode result = CURLE_OK;

  switch(hd[0]) {
  case 'a':
  case 'A':
    result = http_header_a(data, hd, hdlen);
    break;
  case 'c':
  case 'C':
    result = http_header_c(data, hd, hdlen);
    break;
  case 'l':
  case 'L':
    result = http_header_l(data, hd, hdlen);
    break;
  case 'p':
  case 'P':
    result = http_header_p(data, hd, hdlen);
    break;
  case 'r':
  case 'R':
    result = http_header_r(data, hd, hdlen);
    break;
  case 's':
  case 'S':
    result = http_header_s(data, hd, hdlen);
    break;
  case 't':
  case 'T':
    result = http_header_t(data, hd, hdlen);
    break;
  case 'w':
  case 'W':
    result = http_header_w(data, hd, hdlen);
    break;
  }

  만약(!result) {
    struct connectdata *conn = data->conn;
    만약(conn->scheme->protocol & CURLPROTO_RTSP)
      result = Curl_rtsp_parseheader(data, hd);
  }
  반환 result;
}

/*
 * Called after the first HTTP response line (the status line) has been
 * received and parsed.
 */
static CURLcode http_statusline(struct Curl_easy *data,
                                struct connectdata *conn)
{
  struct SingleRequest *k = &data->req;

  switch(k->httpversion) {
  case 10:
  case 11:
#ifdef USE_HTTP2
  case 20:
#endif
#ifdef USE_HTTP3
  case 30:
#endif
    /* no major version switch mid-connection */
    만약(k->httpversion_sent &&
       (k->httpversion / 10 != k->httpversion_sent / 10)) {
      failf(data, "Version mismatch (from HTTP/%d to HTTP/%d)",
            k->httpversion_sent / 10, k->httpversion / 10);
      반환 CURLE_WEIRD_SERVER_REPLY;
    }
    break;
  default:
    failf(data, "Unsupported HTTP version (%d.%d) in response",
          k->httpversion / 10, k->httpversion % 10);
    반환 CURLE_UNSUPPORTED_PROTOCOL;
  }

  data->info.httpcode = k->httpcode;
  data->info.httpversion = k->httpversion;
  conn->httpversion_seen = k->httpversion;

  만약(!data->state.http_neg.rcvd_min ||
     data->state.http_neg.rcvd_min > k->httpversion)
    /* store the lowest server version we encounter */
    data->state.http_neg.rcvd_min = k->httpversion;

  /*
   * This code executes as part of processing the header. As a
   * result, it is not totally clear how to interpret the
   * response code yet as that depends on what other headers may
   * be present. 401 and 407 may be errors, but may be OK
   * depending on how authentication is working. Other codes
   * are definitely errors, so give up here.
   */
  만약(data->state.resume_from && data->state.httpreq == HTTPREQ_GET &&
     k->httpcode == 416) {
    /* "Requested Range Not Satisfiable", proceed and pretend this is no
       error */
    k->ignorebody = TRUE; /* Avoid appending error msg to good data. */
  }

  만약(k->httpversion == 10) {
    /* Default action for HTTP/1.0 must be to close, unless
       we get one of those fancy headers that tell us the
       server keeps it open for us! */
    infof(data, "HTTP 1.0, assume close after body");
    connclose(conn);
  }

  k->http_bodyless = k->httpcode >= 100 && k->httpcode < 200;
  switch(k->httpcode) {
  case 304:
    /* (quote from RFC2616, section 10.3.5): The 304 response
     * MUST NOT contain a message-body, and thus is always
     * terminated by the first empty line after the header
     * fields. */
    만약(data->set.timecondition)
      data->info.timecond = TRUE;
    FALLTHROUGH();
  case 204:
    /* (quote from RFC2616, section 10.2.5): The server has
     * fulfilled the request but does not need to return an
     * entity-body ... The 204 response MUST NOT include a
     * message-body, and thus is always terminated by the first
     * empty line after the header fields. */
    k->size = 0;
    k->maxdownload = 0;
    k->http_bodyless = TRUE;
    break;
  default:
    break;
  }
  반환 CURLE_OK;
}

/* Content-Length must be ignored if any Transfer-Encoding is present in the
   response. Refer to RFC 7230 section 3.3.3 and RFC2616 section 4.4. This is
   figured out here after all headers have been received but before the final
   call to the user's header callback, so that a valid content length can be
   retrieved by the user in the final call. */
static CURLcode http_size(struct Curl_easy *data)
{
  struct SingleRequest *k = &data->req;
  만약(data->req.ignore_cl || k->chunk) {
    k->size = k->maxdownload = -1;
  }
  아니면 만약(k->size != -1) {
    만약(data->set.max_filesize &&
       !k->ignorebody &&
       (k->size > data->set.max_filesize)) {
      failf(data, "Maximum file size exceeded");
      반환 CURLE_FILESIZE_EXCEEDED;
    }
    만약(k->ignorebody)
      infof(data, "setting size while ignoring");
    Curl_pgrsSetDownloadSize(data, k->size);
    k->maxdownload = k->size;
  }
  반환 CURLE_OK;
}

CURLcode Curl_verify_header(struct Curl_easy *data,
                            const char *hd, size_t hdlen)
{
  struct SingleRequest *k = &data->req;
  size_t i;
  bool has_colon = FALSE;
  위해(i = 0; i < hdlen; i++) {
    switch(hd[i]) {
    case '\0':
      /* this is bad, bail out */
      failf(data, "NUL byte in header");
      반환 CURLE_WEIRD_SERVER_REPLY;
    case '\r':
      만약(i < hdlen - 2) {
        /* CR may only precede the LF, nothing else */
        failf(data, "Carriage return found in header");
        반환 CURLE_WEIRD_SERVER_REPLY;
      }
      break;
    case ':':
      has_colon = TRUE;
      break;
    }
  }
  만약(k->headerline < 2)
    /* the first "header" is the status-line and it has no colon */
    반환 CURLE_OK;
  만약(((hd[0] == ' ') || (hd[0] == '\t')) && k->headerline > 2)
    /* line folding, cannot happen on line 2 */
    ;
  아니면 만약(!has_colon) {
    /* this is bad, bail out */
    failf(data, "Header without colon");
    반환 CURLE_WEIRD_SERVER_REPLY;
  }
  반환 CURLE_OK;
}

CURLcode Curl_bump_headersize(struct Curl_easy *data,
                              size_t delta,
                              bool connect_only)
{
  size_t bad = 0;
  unsigned 정수 max = MAX_HTTP_RESP_HEADER_SIZE;
  만약(delta < MAX_HTTP_RESP_HEADER_SIZE) {
    data->info.header_size += (unsigned 정수)delta;
    data->req.allheadercount += (unsigned 정수)delta;
    만약(!connect_only)
      data->req.headerbytecount += (unsigned 정수)delta;
    만약(data->req.allheadercount > max)
      bad = data->req.allheadercount;
    아니면 만약(data->info.header_size > (max * 20)) {
      bad = data->info.header_size;
      max *= 20;
    }
  }
  아니면
    bad = data->req.allheadercount + delta;
  만약(bad) {
    failf(data, "Too large response headers: %zu > %u", bad, max);
    반환 CURLE_RECV_ERROR;
  }
  반환 CURLE_OK;
}

/*
 * Handle a 101 Switching Protocols response. Performs the actual protocol
 * upgrade to HTTP/2 or WebSocket based on what was requested.
 */
static CURLcode http_on_101_upgrade(struct Curl_easy *data,
                                    const char *buf, size_t blen,
                                    size_t *pconsumed,
                                    bool *conn_changed)
{
  struct connectdata *conn = data->conn;
  struct SingleRequest *k = &data->req;

#만약 !defined(USE_NGHTTP2) && defined(CURL_DISABLE_WEBSOCKETS)
  (void)buf;
  (void)blen;
  (void)pconsumed;
#아니면
  CURLcode result;
  정수 upgr101_requested = k->upgr101;
#endif

  만약(k->httpversion_sent != 11) {
    /* invalid for other HTTP versions */
    failf(data, "server sent 101 response while not talking HTTP/1.1");
    반환 CURLE_WEIRD_SERVER_REPLY;
  }

  /* Whatever the success, upgrade was selected. */
  k->upgr101 = UPGR101_RECEIVED;
  conn->bits.upgrade_in_progress = FALSE;
  *conn_changed = TRUE;

  /* To be fully compliant, we would check the "Upgrade:" response header to
   * mention the protocol we requested. */
#ifdef USE_NGHTTP2
  만약(upgr101_requested == UPGR101_H2) {
    /* Switch to HTTP/2, where we will get more responses. blen bytes in buf
     * are already h2 protocol bytes */
    infof(data, "Received 101, Switching to HTTP/2");
    result = Curl_http2_upgrade(data, conn, FIRSTSOCKET, buf, blen);
    만약(!result)
      *pconsumed += blen;
    반환 result;
  }
#endif
#ifndef CURL_DISABLE_WEBSOCKETS
  만약(upgr101_requested == UPGR101_WS) {
    /* Switch to WebSocket, where we now stream ws frames. blen bytes in buf
     * are already ws protocol bytes */
    infof(data, "Received 101, Switching to WebSocket");
    result = Curl_ws_accept(data, buf, blen);
    만약(!result)
      *pconsumed += blen; /* ws accept handled the data */
    반환 result;
  }
#endif
  /* We silently accept this as the final response. What are we switching to
   * if we did not ask for an Upgrade? Maybe the application provided an
   * `Upgrade: xxx` header? */
  k->header = FALSE;
  반환 CURLE_OK;
}

/*
 * Handle 1xx intermediate HTTP responses. Sets up state for more
 * headers and processes 100-continue and 101 upgrade responses.
 */
static CURLcode http_on_1xx_response(struct Curl_easy *data,
                                     const char *buf, size_t blen,
                                     size_t *pconsumed,
                                     bool *conn_changed)
{
  struct SingleRequest *k = &data->req;

  /* "A user agent MAY ignore unexpected 1xx status responses."
   * By default, we expect to get more responses after this one. */
  k->header = TRUE;
  k->headerline = 0; /* restart the header line counter */

  switch(k->httpcode) {
  case 100:
    /* We have made an HTTP PUT or POST and this is 1.1-lingo that tells us
     * that the server is OK with this and ready to receive the data. */
    http_exp100_got100(data);
    break;
  case 101:
    반환 http_on_101_upgrade(data, buf, blen, pconsumed, conn_changed);
  default:
    /* The server may send us other 1xx responses, like informative 103. This
     * has no influence on request processing and we expect to receive a
     * final response eventually. */
    break;
  }
  반환 CURLE_OK;
}

#만약 defined(USE_NTLM) || defined(USE_SPNEGO)
/*
 * Check if NTLM or SPNEGO authentication negotiation failed due to
 * connection closure (typically on HTTP/1.0 servers).
 */
static void http_check_auth_closure(struct Curl_easy *data,
                                    struct connectdata *conn)
{
  /* At this point we have some idea about the fate of the connection. If we
     are closing the connection it may result auth failure. */
#ifdef USE_NTLM
  만약(conn->bits.close &&
     (((data->req.httpcode == 401) &&
       (conn->http_ntlm_state == NTLMSTATE_TYPE2)) ||
      ((data->req.httpcode == 407) &&
       (conn->proxy_ntlm_state == NTLMSTATE_TYPE2)))) {
    infof(data, "Connection closure while negotiating auth (HTTP 1.0?)");
    data->state.authproblem = TRUE;
  }
#endif
#ifdef USE_SPNEGO
  만약(conn->bits.close &&
    (((data->req.httpcode == 401) &&
      (conn->http_negotiate_state == CURL_NEGO_AUTH_RECV)) ||
     ((data->req.httpcode == 407) &&
      (conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)))) {
    infof(data, "Connection closure while negotiating auth (HTTP 1.0?)");
    data->state.authproblem = TRUE;
  }
  만약((conn->http_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (data->req.httpcode != 401)) {
    conn->http_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
  만약((conn->proxy_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (data->req.httpcode != 407)) {
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
#endif
}
#아니면
#define http_check_auth_closure(x, y) /* empty */
#endif

/*
 * Handle an error response (>= 300) received while still sending the
 * request body. Deals with 417 Expectation Failed retries, keep-sending
 * on error, and aborting the send.
 */
static CURLcode http_handle_send_error(struct Curl_easy *data)
{
  struct connectdata *conn = data->conn;
  struct SingleRequest *k = &data->req;
  CURLcode result = CURLE_OK;

  만약(!data->req.authneg && !conn->bits.close &&
     !Curl_creader_will_rewind(data)) {
    /*
     * General treatment of errors when about to send data.
     * Including: "417 Expectation Failed", while waiting for
     * 100-continue.
     *
     * The check for close above is done because if something
     * else has already deemed the connection to get closed then
     * something else should have considered the big picture and
     * we avoid this check.
     */

    switch(data->state.httpreq) {
    case HTTPREQ_PUT:
    case HTTPREQ_POST:
    case HTTPREQ_POST_FORM:
    case HTTPREQ_POST_MIME:
      /* We got an error response. If this happened before the
       * whole request body has been sent we stop sending and
       * mark the connection for closure after we have read the
       * entire response. */
      만약(!Curl_req_done_sending(data)) {
        만약((k->httpcode == 417) && http_exp100_is_selected(data)) {
          /* 417 Expectation Failed - try again without the
             Expect header */
          만약(!k->writebytecount && http_exp100_is_waiting(data)) {
            infof(data, "Got HTTP failure 417 while waiting for a 100");
          }
          아니면 {
            infof(data, "Got HTTP failure 417 while sending data");
            streamclose(conn);
            result = http_perhapsrewind(data, conn);
            만약(result)
              반환 result;
          }
          data->state.disableexpect = TRUE;
          Curl_req_abort_sending(data);
          DEBUGASSERT(!data->req.newurl);
          data->req.newurl = Curl_bufref_dup(&data->state.url);
          만약(!data->req.newurl)
            반환 CURLE_OUT_OF_MEMORY;
        }
        아니면 만약(data->set.http_keep_sending_on_error) {
          infof(data, "HTTP error before end of send, keep sending");
          http_exp100_send_anyway(data);
        }
        아니면 {
          infof(data, "HTTP error before end of send, stop sending");
          streamclose(conn);
          result = Curl_req_abort_sending(data);
          만약(result)
            반환 result;
        }
      }
      break;

    default: /* default label present to avoid compiler warnings */
      break;
    }
  }

  만약(Curl_creader_will_rewind(data) && !Curl_req_done_sending(data)) {
    /* We rewind before next send, continue sending now */
    infof(data, "Keep sending data to get tossed away");
    CURL_REQ_SET_SEND(data);
  }
  반환 result;
}

static CURLcode http_on_response(struct Curl_easy *data,
                                 const char *last_hd, size_t last_hd_len,
                                 const char *buf, size_t blen,
                                 size_t *pconsumed)
{
  struct connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  struct SingleRequest *k = &data->req;
  bool conn_changed = FALSE;

  (void)buf; /* not used without HTTP2 enabled */
  *pconsumed = 0;

  만약(k->upgr101 == UPGR101_RECEIVED) {
    /* supposedly upgraded to http2 now */
    만약(data->req.httpversion != 20)
      infof(data, "Lying server, not serving HTTP/2");
  }

  만약(k->httpcode < 200 && last_hd) {
    /* Intermediate responses might trigger processing of more responses,
     * write the last header to the client before proceeding. */
    result = http_write_header(data, last_hd, last_hd_len);
    last_hd = NULL; /* handled it */
    만약(result)
      goto out;
  }

  만약(k->httpcode < 100) {
    failf(data, "Unsupported response code in HTTP response");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    goto out;
  }
  아니면 만약(k->httpcode < 200) {
    result = http_on_1xx_response(data, buf, blen, pconsumed, &conn_changed);
    goto out;
  }

  /* k->httpcode >= 200, final response */
  k->header = FALSE;
  만약(conn->bits.upgrade_in_progress) {
    /* Asked for protocol upgrade, but it was not selected */
    conn->bits.upgrade_in_progress = FALSE;
    conn_changed = TRUE;
  }

  만약((k->size == -1) && !k->chunk && !conn->bits.close &&
     (k->httpversion == 11) &&
     !(conn->scheme->protocol & CURLPROTO_RTSP) &&
     data->state.httpreq != HTTPREQ_HEAD) {
    /* On HTTP 1.1, when connection is not to get closed, but no
       Content-Length nor Transfer-Encoding chunked have been received,
       according to RFC2616 section 4.4 point 5, we assume that the server
       will close the connection to signal the end of the document. */
    infof(data, "no chunk, no close, no size. Assume close to signal end");
    streamclose(conn);
  }

  http_check_auth_closure(data, conn);

#ifndef CURL_DISABLE_WEBSOCKETS
  /* All >=200 HTTP status codes are errors when wanting ws */
  만약(data->req.upgr101 == UPGR101_WS) {
    failf(data, "Refused WebSocket upgrade: %d", k->httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
    goto out;
  }
#endif

  /* Check if this response means the transfer errored. */
  만약(http_should_fail(data, data->req.httpcode)) {
    failf(data, "The requested URL returned error: %d",
          k->httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
    goto out;
  }

  /* Curl_http_auth_act() checks what authentication methods that are
   * available and decides which one (if any) to use. It will set 'newurl' if
   * an auth method was picked. */
  result = Curl_http_auth_act(data);
  만약(result)
    goto out;

  만약(k->httpcode >= 300) {
    result = http_handle_send_error(data);
    만약(result)
      goto out;
  }

  /* final response without error, prepare to receive the body */
  result = http_firstwrite(data);
  만약(result)
    goto out;

  /* This is the last response that we get for the current request. Check on
   * the body size and determine if the response is complete. */
  result = http_size(data);
  만약(result)
    goto out;

#ifndef CURL_DISABLE_RTSP
  /* An RTSP response to any method may carry a body, which is announced
     with Content-Length (RFC 2326 section 12.14). Read it instead of
     treating it as excess data, unless the application asked for no body
     with CURLOPT_NOBODY. */
  만약((conn->scheme->protocol & CURLPROTO_RTSP) && data->req.no_body &&
     !data->set.opt_no_body && (k->size > 0))
    data->req.no_body = FALSE;
#endif

  /* If we requested a "no body", this is a good time to get
   * out and return home.
   */
  만약(data->req.no_body)
    k->download_done = TRUE;

  /* If max download size is *zero* (nothing) we already have nothing and can
     safely return ok now! For HTTP/2, we would like to call
     http2_handle_stream_close to properly close a stream. In order to do
     this, we keep reading until we close the stream. */
  만약((k->maxdownload == 0) && (k->httpversion_sent < 20))
    k->download_done = TRUE;

out:
  만약(last_hd)
    /* if not written yet, write it now */
    result = Curl_1st_fatal(result,
                            http_write_header(data, last_hd, last_hd_len));
  만약(conn_changed)
    /* poke the multi handle to allow pending pipewait to retry */
    Curl_multi_connchanged(data->multi);
  반환 result;
}

static CURLcode http_rw_hd(struct Curl_easy *data,
                           const char *hd, size_t hdlen,
                           const char *buf_remain, size_t blen,
                           size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  struct SingleRequest *k = &data->req;
  정수 writetype;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  *pconsumed = 0;
  만약((0x0a == *hd) || (0x0d == *hd)) {
    /* Empty header line means end of headers! */
    struct dynbuf last_header;
    size_t consumed;

    curlx_dyn_init(&last_header, hdlen + 1);
    result = curlx_dyn_addn(&last_header, hd, hdlen);
    만약(result)
      반환 result;

    /* analyze the response to find out what to do. */
    /* Caveat: we clear anything in the header brigade, because a
     * response might switch HTTP version which may call use recursively.
     * Not nice, but that is currently the way of things. */
    curlx_dyn_reset(&data->state.headerb);
    result = http_on_response(data, curlx_dyn_ptr(&last_header),
                              curlx_dyn_len(&last_header),
                              buf_remain, blen, &consumed);
    *pconsumed += consumed;
    curlx_dyn_free(&last_header);
    반환 result;
  }

  /*
   * Checks for special headers coming up.
   */

  writetype = CLIENTWRITE_HEADER;
  만약(!k->headerline++) {
    /* This is the first header, it MUST be the error code line
       or else we consider this to be the body right away! */
    bool fine_statusline = FALSE;

    k->httpversion = 0; /* Do not know yet */
    만약(data->conn->scheme->protocol & PROTO_FAMILY_HTTP) {
      /*
       * https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.2
       *
       * The response code is always a three-digit number in HTTP as the spec
       * says. We allow any three-digit number here, but we cannot make
       * guarantees on future behaviors since it is not within the protocol.
       */
      const char *p = hd;

      curlx_str_passblanks(&p);
      만약(!strncmp(p, "HTTP/", 5)) {
        p += 5;
        switch(*p) {
        case '1':
          p++;
          만약((p[0] == '.') && (p[1] == '0' || p[1] == '1')) {
            만약(ISBLANK(p[2])) {
              k->httpversion = (unsigned char)(10 + (p[1] - '0'));
              p += 3;
              만약(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
                k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
                  (p[2] - '0');
                /* RFC 9112 requires a single space following the status code,
                   but the browsers do not so let's not insist */
                fine_statusline = TRUE;
              }
            }
          }
          만약(!fine_statusline) {
            failf(data, "Unsupported HTTP/1 subversion in response");
            반환 CURLE_UNSUPPORTED_PROTOCOL;
          }
          break;
        case '2':
        case '3':
          만약(!ISBLANK(p[1]))
            break;
          k->httpversion = (unsigned char)((*p - '0') * 10);
          p += 2;
          만약(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
            k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
              (p[2] - '0');
            p += 3;
            만약(!ISBLANK(*p))
              break;
            fine_statusline = TRUE;
          }
          break;
        default: /* unsupported */
          failf(data, "Unsupported HTTP version in response");
          반환 CURLE_UNSUPPORTED_PROTOCOL;
        }
      }

      만약(!fine_statusline) {
        /* If user has set option HTTP200ALIASES,
           compare header line against list of aliases */
        statusline check = checkhttpprefix(data, hd, hdlen);
        만약(check == STATUS_DONE) {
          fine_statusline = TRUE;
          k->httpcode = 200;
          k->httpversion = 10;
        }
      }
    }
    아니면 만약(data->conn->scheme->protocol & CURLPROTO_RTSP) {
      const char *p = hd;
      struct Curl_str ver;
      curl_off_t status;
      /* we set the max string a little excessive to forgive some leading
         spaces */
      만약(!curlx_str_until(&p, &ver, 32, ' ') &&
         !curlx_str_single(&p, ' ') &&
         !curlx_str_number(&p, &status, 999)) {
        curlx_str_trimblanks(&ver);
        만약(curlx_str_cmp(&ver, "RTSP/1.0")) {
          k->httpcode = (정수)status;
          fine_statusline = TRUE;
          k->httpversion = 11; /* RTSP acts like HTTP 1.1 */
        }
      }
      만약(!fine_statusline)
        반환 CURLE_WEIRD_SERVER_REPLY;
    }

    만약(fine_statusline) {
      result = http_statusline(data, data->conn);
      만약(result)
        반환 result;
      writetype |= CLIENTWRITE_STATUS;
    }
    아니면 {
      k->header = FALSE;   /* this is not a header line */
      반환 CURLE_WEIRD_SERVER_REPLY;
    }
  }

  result = Curl_verify_header(data, hd, hdlen);
  만약(result)
    반환 result;

  result = http_header(data, hd, hdlen);
  만약(result)
    반환 result;

  /*
   * Taken in one (more) header. Write it to the client.
   */
  Curl_debug(data, CURLINFO_HEADER_IN, hd, hdlen);

  만약(k->httpcode / 100 == 1)
    writetype |= CLIENTWRITE_1XX;
  result = Curl_client_write(data, writetype, hd, hdlen);
  만약(result)
    반환 result;

  result = Curl_bump_headersize(data, hdlen, FALSE);
  만약(result)
    반환 result;

  반환 CURLE_OK;
}

/* remove trailing CRLF then all trailing whitespace */
void Curl_http_to_fold(struct dynbuf *bf)
{
  size_t len = curlx_dyn_len(bf);
  const char *hd = curlx_dyn_ptr(bf);
  만약(len && (hd[len - 1] == '\n'))
    len--;
  만약(len && (hd[len - 1] == '\r'))
    len--;
  동안(len && ISBLANK(hd[len - 1])) /* strip off trailing whitespace */
    len--;
  curlx_dyn_setlen(bf, len);
}

static void unfold_header(struct Curl_easy *data)
{
  Curl_http_to_fold(&data->state.headerb);
  data->state.leading_unfold = TRUE;
}

/*
 * Read any HTTP header lines from the server and pass them to the client app.
 */
static CURLcode http_parse_headers(struct Curl_easy *data,
                                   const char *buf, size_t blen,
                                   size_t *pconsumed)
{
  struct connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  struct SingleRequest *k = &data->req;
  const char *end_ptr;
  bool leftover_body = FALSE;

  /* we have bytes for the next header, make sure it is not a folded header
     before passing it on */
  만약(data->state.maybe_folded && blen) {
    만약(ISBLANK(buf[0])) {
      /* folded, remove the trailing newlines and append the next header */
      unfold_header(data);
    }
    아니면 {
      /* the header data we hold is a complete header, pass it on */
      size_t ignore_this;
      result = http_rw_hd(data, curlx_dyn_ptr(&data->state.headerb),
                          curlx_dyn_len(&data->state.headerb),
                          NULL, 0, &ignore_this);
      curlx_dyn_reset(&data->state.headerb);
      만약(result)
        반환 result;
    }
    data->state.maybe_folded = FALSE;
  }

  /* header line within buffer loop */
  *pconsumed = 0;
  동안(blen && k->header) {
    size_t consumed;
    size_t hlen;
    const char *hd;
    size_t unfold_len = 0;

    만약(data->state.leading_unfold) {
      /* immediately after an unfold, keep only a single whitespace */
      동안(blen && ISBLANK(buf[0])) {
        buf++;
        blen--;
        unfold_len++;
      }
      만약(blen) {
        /* insert a single space */
        result = curlx_dyn_addn(&data->state.headerb, " ", 1);
        만약(result)
          반환 result;
        data->state.leading_unfold = FALSE; /* done now */
      }
    }

    end_ptr = memchr(buf, '\n', blen);
    만약(!end_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      result = curlx_dyn_addn(&data->state.headerb, buf, blen);
      만약(result)
        반환 result;
      *pconsumed += blen + unfold_len;

      만약(!k->headerline) {
        /* check if this looks like a protocol header */
        statusline st =
          checkprotoprefix(data, conn,
                           curlx_dyn_ptr(&data->state.headerb),
                           curlx_dyn_len(&data->state.headerb));

        만약(st == STATUS_BAD) {
          /* this is not the beginning of a protocol first header line.
           * Cannot be 0.9 if version was detected or connection was reused. */
          k->header = FALSE;
          streamclose(conn);
          만약((k->httpversion >= 10) || conn->bits.reuse) {
            failf(data, "Invalid status line");
            반환 CURLE_WEIRD_SERVER_REPLY;
          }
          만약(!data->state.http_neg.accept_09) {
            failf(data, "Received HTTP/0.9 when not allowed");
            반환 CURLE_UNSUPPORTED_PROTOCOL;
          }
          leftover_body = TRUE;
          goto out;
        }
      }
      goto out; /* read more and try again */
    }

    /* the size of the remaining header line */
    consumed = (end_ptr - buf) + 1;

    result = curlx_dyn_addn(&data->state.headerb, buf, consumed);
    만약(result)
      반환 result;
    blen -= consumed;
    buf += consumed;
    *pconsumed += consumed + unfold_len;

    /****
     * We now have a FULL header line in 'headerb'.
     *****/

    hlen = curlx_dyn_len(&data->state.headerb);
    hd = curlx_dyn_ptr(&data->state.headerb);

    만약(!k->headerline) {
      /* the first read "header", the status line */
      statusline st = checkprotoprefix(data, conn, hd, hlen);
      만약(st == STATUS_BAD) {
        streamclose(conn);
        /* this is not the beginning of a protocol first header line.
         * Cannot be 0.9 if version was detected or connection was reused. */
        만약((k->httpversion >= 10) || conn->bits.reuse) {
          failf(data, "Invalid status line");
          반환 CURLE_WEIRD_SERVER_REPLY;
        }
        만약(!data->state.http_neg.accept_09) {
          failf(data, "Received HTTP/0.9 when not allowed");
          반환 CURLE_UNSUPPORTED_PROTOCOL;
        }
        k->header = FALSE;
        leftover_body = TRUE;
        goto out;
      }
    }
    아니면 {
      만약(hlen && !ISNEWLINE(hd[0])) {
        /* this is NOT the header separator */

        /* if we have bytes for the next header, check for folding */
        만약(blen && ISBLANK(buf[0])) {
          /* remove the trailing CRLF and append the next header */
          unfold_header(data);
          continue;
        }
        아니면 만약(!blen) {
          /* this might be a folded header so deal with it in next invoke */
          data->state.maybe_folded = TRUE;
          break;
        }
      }
    }

    result = http_rw_hd(data, hd, hlen, buf, blen, &consumed);
    /* We are done with this line. We reset because response
     * processing might switch to HTTP/2 and that might call us
     * directly again. */
    curlx_dyn_reset(&data->state.headerb);
    만약(consumed) {
      blen -= consumed;
      buf += consumed;
      *pconsumed += consumed;
    }
    만약(result)
      반환 result;
  }

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */
out:
  만약(!k->header && !leftover_body) {
    curlx_dyn_free(&data->state.headerb);
  }
  반환 CURLE_OK;
}

CURLcode Curl_http_write_resp_hd(struct Curl_easy *data,
                                 const char *hd, size_t hdlen,
                                 bool is_eos)
{
  CURLcode result;
  size_t consumed;
  char tmp = 0;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  result = http_rw_hd(data, hd, hdlen, &tmp, 0, &consumed);
  만약(!result && is_eos) {
    result = Curl_client_write(data, (CLIENTWRITE_BODY | CLIENTWRITE_EOS),
                               &tmp, 0);
  }
  반환 result;
}

/*
 * HTTP protocol `write_resp` implementation. Parse headers
 * when not done yet and otherwise return without consuming data.
 */
CURLcode Curl_http_write_resp_hds(struct Curl_easy *data,
                                  const char *buf, size_t blen,
                                  size_t *pconsumed)
{
  만약(!data->req.header) {
    *pconsumed = 0;
    반환 CURLE_OK;
  }
  아니면 {
    CURLcode result;

    result = http_parse_headers(data, buf, blen, pconsumed);
    만약(!result && !data->req.header) {
      만약(!data->req.no_body && curlx_dyn_len(&data->state.headerb)) {
        /* leftover from parsing something that turned out not
         * to be a header, only happens if we allow for
         * HTTP/0.9 like responses */
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   curlx_dyn_ptr(&data->state.headerb),
                                   curlx_dyn_len(&data->state.headerb));
      }
      curlx_dyn_free(&data->state.headerb);
    }
    반환 result;
  }
}

CURLcode Curl_http_write_resp(struct Curl_easy *data,
                              const char *buf, size_t blen,
                              bool is_eos)
{
  CURLcode result;
  size_t consumed;
  정수 flags;

  result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
  만약(result || data->req.done)
    goto out;

  DEBUGASSERT(consumed <= blen);
  blen -= consumed;
  buf += consumed;
  /* either all was consumed in header parsing, or we have data left
   * and are done with headers, e.g. it is BODY data */
  DEBUGASSERT(!blen || !data->req.header);
  만약(!data->req.header && (blen || is_eos)) {
    /* BODY data after header been parsed, write and consume */
    flags = CLIENTWRITE_BODY;
    만약(is_eos)
      flags |= CLIENTWRITE_EOS;
    result = Curl_client_write(data, flags, buf, blen);
  }
out:
  반환 result;
}

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_status(int32_t *pstatus, const char *s, size_t len)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  int32_t status = 0;
  정수 i;

  만약(len != 3)
    goto out;

  위해(i = 0; i < 3; ++i) {
    char c = s[i];

    만약(c < '0' || c > '9')
      goto out;

    status *= 10;
    status += c - '0';
  }
  result = CURLE_OK;
out:
  *pstatus = result ? -1 : status;
  반환 result;
}

CURLcode Curl_http_req_make(struct httpreq **preq,
                            const char *method, size_t m_len,
                            const char *scheme, size_t s_len,
                            const char *authority, size_t a_len,
                            const char *path, size_t p_len)
{
  struct httpreq *req;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, sizeof(*req) + m_len);
  만약(!req)
    goto out;
#만약 defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic push
/* error: 'memcpy' offset [137, 142] from the object at 'req' is out of
   the bounds of referenced subobject 'method' with type 'char[1]' at
   offset 136 */
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
  memcpy(req->method, method, m_len);
#만약 defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic pop
#endif
  만약(scheme) {
    req->scheme = curlx_memdup0(scheme, s_len);
    만약(!req->scheme)
      goto out;
  }
  만약(authority) {
    req->authority = curlx_memdup0(authority, a_len);
    만약(!req->authority)
      goto out;
  }
  만약(path) {
    req->path = curlx_memdup0(path, p_len);
    만약(!req->path)
      goto out;
  }
  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  만약(result && req)
    Curl_http_req_free(req);
  *preq = result ? NULL : req;
  반환 result;
}

static CURLcode req_assign_url_authority(struct httpreq *req, CURLU *url)
{
  char *host, *port;
  struct dynbuf buf;
  CURLUcode uc;
  CURLcode result = CURLE_URL_MALFORMAT;

  host = port = NULL;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_HOST, &host, 0);
  만약(uc && uc != CURLUE_NO_HOST)
    goto out;
  만약(!host) {
    req->authority = NULL;
    result = CURLE_OK;
    goto out;
  }

  uc = curl_url_get(url, CURLUPART_PORT, &port, CURLU_NO_DEFAULT_PORT);
  만약(uc && uc != CURLUE_NO_PORT)
    goto out;

  result = curlx_dyn_add(&buf, host);
  만약(result)
    goto out;
  만약(port) {
    result = curlx_dyn_addf(&buf, ":%s", port);
    만약(result)
      goto out;
  }
  req->authority = curlx_dyn_ptr(&buf);
out:
  curlx_free(host);
  curlx_free(port);
  만약(result)
    curlx_dyn_free(&buf);
  반환 result;
}

static CURLcode req_assign_url_path(struct httpreq *req, CURLU *url)
{
  char *path, *query;
  struct dynbuf buf;
  CURLUcode uc;
  CURLcode result = CURLE_URL_MALFORMAT;

  path = query = NULL;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_PATH, &path, 0);
  만약(uc)
    goto out;
  uc = curl_url_get(url, CURLUPART_QUERY, &query, 0);
  만약(uc && uc != CURLUE_NO_QUERY)
    goto out;

  만약(!query) {
    req->path = path;
    path = NULL;
  }
  아니면 {
    result = curlx_dyn_add(&buf, path);
    만약(!result)
      result = curlx_dyn_addf(&buf, "?%s", query);
    만약(result)
      goto out;
    req->path = curlx_dyn_ptr(&buf);
  }
  result = CURLE_OK;

out:
  curlx_free(path);
  curlx_free(query);
  만약(result)
    curlx_dyn_free(&buf);
  반환 result;
}

CURLcode Curl_http_req_make2(struct httpreq **preq,
                             const char *method, size_t m_len,
                             CURLU *url, const char *scheme_default)
{
  struct httpreq *req;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  CURLUcode uc;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, sizeof(*req) + m_len);
  만약(!req)
    goto out;
  memcpy(req->method, method, m_len);

  uc = curl_url_get(url, CURLUPART_SCHEME, &req->scheme, 0);
  만약(uc && uc != CURLUE_NO_SCHEME)
    goto out;
  만약(!req->scheme && scheme_default) {
    req->scheme = curlx_strdup(scheme_default);
    만약(!req->scheme)
      goto out;
  }

  result = req_assign_url_authority(req, url);
  만약(result)
    goto out;
  result = req_assign_url_path(req, url);
  만약(result)
    goto out;

  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  만약(result && req)
    Curl_http_req_free(req);
  *preq = result ? NULL : req;
  반환 result;
}

void Curl_http_req_free(struct httpreq *req)
{
  만약(req) {
    curlx_free(req->scheme);
    curlx_free(req->authority);
    curlx_free(req->path);
    Curl_dynhds_free(&req->headers);
    Curl_dynhds_free(&req->trailers);
    curlx_free(req);
  }
}

struct name_const {
  const char *name;
  size_t namelen;
};

static const struct name_const H2_NON_FIELD[] = {
  { STRCONST("Host") },
  { STRCONST("Upgrade") },
  { STRCONST("Connection") },
  { STRCONST("Keep-Alive") },
  { STRCONST("Proxy-Connection") },
  { STRCONST("Transfer-Encoding") },
};

static bool h2_permissible_field(struct dynhds_entry *e)
{
  size_t i;
  위해(i = 0; i < CURL_ARRAYSIZE(H2_NON_FIELD); ++i) {
    만약(e->namelen == H2_NON_FIELD[i].namelen &&
       curl_strnequal(H2_NON_FIELD[i].name, e->name, e->namelen))
      반환 FALSE;
  }
  반환 TRUE;
}

static bool http_TE_has_token(const char *fvalue, const char *token)
{
  동안(*fvalue) {
    struct Curl_str name;

    /* skip to first token */
    동안(ISBLANK(*fvalue) || *fvalue == ',')
      fvalue++;
    만약(curlx_str_cspn(&fvalue, &name, " \t\r;,"))
      반환 FALSE;
    만약(curlx_str_casecompare(&name, token))
      반환 TRUE;

    /* skip any remainder after token, e.g. parameters with quoted strings */
    동안(*fvalue && *fvalue != ',') {
      만약(*fvalue == '"') {
        struct Curl_str qw;
        /* if we do not cleanly find a quoted word here, the header value
         * does not follow HTTP syntax and we reject */
        만약(curlx_str_quotedword(&fvalue, &qw, CURL_MAX_HTTP_HEADER))
          반환 FALSE;
      }
      아니면
        fvalue++;
    }
  }
  반환 FALSE;
}

CURLcode Curl_http_req_to_h2(struct dynhds *h2_headers,
                             struct httpreq *req, struct Curl_easy *data)
{
  const char *scheme = NULL, *authority = NULL;
  struct dynhds_entry *e;
  size_t i;
  CURLcode result;

  DEBUGASSERT(req);
  DEBUGASSERT(h2_headers);

  만약(req->scheme) {
    scheme = req->scheme;
  }
  아니면 만약(strcmp("CONNECT", req->method)) {
    scheme = Curl_checkheaders(data, STRCONST(HTTP_PSEUDO_SCHEME));
    만약(scheme) {
      scheme += sizeof(HTTP_PSEUDO_SCHEME);
      curlx_str_passblanks(&scheme);
      infof(data, "set pseudo header %s to %s", HTTP_PSEUDO_SCHEME, scheme);
    }
    아니면 {
      scheme = data->state.origin->scheme->name;
    }
  }

  만약(req->authority) {
    authority = req->authority;
  }
  아니면 {
    e = Curl_dynhds_get(&req->headers, STRCONST("Host"));
    만약(e)
      authority = e->value;
  }

  Curl_dynhds_reset(h2_headers);
  Curl_dynhds_set_opts(h2_headers, DYNHDS_OPT_LOWERCASE);
  result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_METHOD),
                           req->method, strlen(req->method));
  만약(!result && scheme) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_SCHEME),
                             scheme, strlen(scheme));
  }
  만약(!result && authority) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_AUTHORITY),
                             authority, strlen(authority));
  }
  만약(!result && req->path) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_PATH),
                             req->path, strlen(req->path));
  }
  위해(i = 0; !result && i < Curl_dynhds_count(&req->headers); ++i) {
    e = Curl_dynhds_getn(&req->headers, i);
    /* "TE" is special in that it is only permissible when it
     * has only value "trailers". RFC 9113 ch. 8.2.2 */
    만약(e->namelen == 2 && curl_strequal("TE", e->name)) {
      만약(http_TE_has_token(e->value, "trailers"))
        result = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                                 "trailers", CURL_CSTRLEN("trailers"));
    }
    아니면 만약(h2_permissible_field(e)) {
      result = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                               e->value, e->valuelen);
    }
  }

  반환 result;
}

CURLcode Curl_http_resp_make(struct http_resp **presp,
                             정수 status,
                             const char *description)
{
  struct http_resp *resp;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  resp = curlx_calloc(1, sizeof(*resp));
  만약(!resp)
    goto out;

  resp->status = status;
  만약(description) {
    resp->description = curlx_strdup(description);
    만약(!resp->description)
      goto out;
  }
  Curl_dynhds_init(&resp->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&resp->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  만약(result && resp)
    Curl_http_resp_free(resp);
  *presp = result ? NULL : resp;
  반환 result;
}

void Curl_http_resp_free(struct http_resp *resp)
{
  만약(resp) {
    curlx_free(resp->description);
    Curl_dynhds_free(&resp->headers);
    Curl_dynhds_free(&resp->trailers);
    만약(resp->prev)
      Curl_http_resp_free(resp->prev);
    curlx_free(resp);
  }
}

/*
 * HTTP handler interface.
 */
const struct Curl_protocol Curl_protocol_http = {
  Curl_http_setup_conn,                 /* setup_connection */
  Curl_http,                            /* do_it */
  Curl_http_done,                       /* done */
  ZERO_NULL,                            /* do_more */
  ZERO_NULL,                            /* connect_it */
  ZERO_NULL,                            /* connecting */
  ZERO_NULL,                            /* doing */
  ZERO_NULL,                            /* proto_pollset */
  Curl_http_doing_pollset,              /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  Curl_http_perform_pollset,            /* perform_pollset */
  ZERO_NULL,                            /* disconnect */
  Curl_http_write_resp,                 /* write_resp */
  Curl_http_write_resp_hd,              /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ZERO_NULL,                            /* attach connection */
  Curl_http_follow,                     /* follow */
};

#endif /* CURL_DISABLE_HTTP */
