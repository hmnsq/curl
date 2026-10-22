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
#அந்தர்பாவ "curl_setup.h"
#அந்தர்பாவ "urldata.h"

#ifndef CURL_DISABLE_HTTP

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif

#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#அந்தர்பாவ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#அந்தர்பாவ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#அந்தர்பாவ <sys/param.h>
#endif

#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "formdata.h"
#அந்தர்பாவ "mime.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "curlx/base64.h"
#அந்தர்பாவ "cookie.h"
#அந்தர்பாவ "vauth/vauth.h"
#அந்தர்பாவ "vquic/vquic.h"
#அந்தர்பாவ "http_digest.h"
#அந்தர்பாவ "http_ntlm.h"
#அந்தர்பாவ "http_negotiate.h"
#அந்தர்பாவ "http_aws_sigv4.h"
#அந்தர்பாவ "http_httpsig.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "urlapi-int.h"
#அந்தர்பாவ "curl_share.h"
#அந்தர்பாவ "dynhds.h"
#அந்தர்பாவ "http.h"
#அந்தர்பாவ "headers.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "parsedate.h" /* for the week day and month names */
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "strcase.h"
#அந்தர்பாவ "content_encoding.h"
#அந்தர்பாவ "http_proxy.h"
#அந்தர்பாவ "http2.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "altsvc.h"
#அந்தர்பாவ "hsts.h"
#அந்தர்பாவ "rtsp.h"
#அந்தர்பாவ "ws.h"
#அந்தர்பாவ "bufref.h"
#அந்தர்பாவ "curlx/strparse.h"

வெறுமை Curl_http_neg_init(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு http_negotiation *neg)
{
  memset(neg, 0, அளவு(*neg));
  neg->accept_09 = data->set.http09_allowed;
  தேர்வு(data->set.httpwant) {
  நிலை CURL_HTTP_VERSION_1_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    neg->only_10 = TRUE;
    நிறுத்து;
  நிலை CURL_HTTP_VERSION_1_1:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    நிறுத்து;
  நிலை CURL_HTTP_VERSION_2_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->h2_upgrade = TRUE;
    நிறுத்து;
  நிலை CURL_HTTP_VERSION_2TLS:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    நிறுத்து;
  நிலை CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    neg->wanted = neg->allowed = (CURL_HTTP_V2x);
    data->state.http_neg.h2_prior_knowledge = TRUE;
    நிறுத்து;
  நிலை CURL_HTTP_VERSION_3:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    neg->allowed = neg->wanted;
    நிறுத்து;
  நிலை CURL_HTTP_VERSION_3ONLY:
    neg->wanted = neg->allowed = (CURL_HTTP_V3x);
    நிறுத்து;
  நிலை CURL_HTTP_VERSION_NONE:
  இயல்பு:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    நிறுத்து;
  }
}

CURLcode Curl_http_setup_conn(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு connectdata *conn)
{
  /* allocate the HTTP-specific struct for the Curl_easy, only to survive
     during this request */
  எனில்(data->state.http_neg.wanted == CURL_HTTP_V3x) {
    /* only HTTP/3, needs to work */
    CURLcode result = Curl_conn_may_http3(data, conn);
    எனில்(result)
      திரும்பு result;
  }
  திரும்பு CURLE_OK;
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
எழுத்து *Curl_checkProxyheaders(கட்டமைப்பு Curl_easy *data,
                             மாறிலி கட்டமைப்பு connectdata *conn,
                             மாறிலி எழுத்து *thisheader,
                             மாறிலி size_t thislen)
{
  கட்டமைப்பு curl_slist *head;

  க்கு(head = (conn->http_proxy.peer && data->set.sep_headers) ?
        data->set.proxyheaders : data->set.headers;
      head; head = head->next) {
    எனில்(curl_strnequal(head->data, thisheader, thislen) &&
       Curl_headersep(head->data[thislen]))
      திரும்பு head->data;
  }

  திரும்பு வெற்று;
}
#endif

/* If the header has a value, this function returns TRUE and the value is in
   'outp' with blanks trimmed off. */
நிலையான bool header_has_மதிப்பு(மாறிலி எழுத்து **headerp, கட்டமைப்பு Curl_str *outp)
{
  bool மதிப்பு = !curlx_str_cspn(headerp, outp, ";:") &&
    (!curlx_str_single(headerp, ':') || !curlx_str_single(headerp, ';'));

  எனில்(மதிப்பு) {
    curlx_str_cspn(headerp, outp, "\r\n");
    curlx_str_trimblanks(outp);
  }
  திரும்பு மதிப்பு;
}

நிலையான bool http_header_is_empty(மாறிலி எழுத்து *header)
{
  கட்டமைப்பு Curl_str out;

  எனில்(header_has_மதிப்பு(&header, &out)) {
    திரும்பு curlx_strlen(&out) == 0;
  }
  திரும்பு TRUE; /* invalid header format, treat as empty */
}

/*
 * Strip off leading and trailing whitespace from the value in the given HTTP
 * header line and return a strdup-ed copy in 'valp' - returns an empty
 * string if the header value consists entirely of whitespace.
 *
 * If the header is provided as "name;", ending with a semicolon, it returns a
 * blank string.
 */
நிலையான CURLcode copy_custom_மதிப்பு(மாறிலி எழுத்து *header, எழுத்து **valp)
{
  கட்டமைப்பு Curl_str out = { 0 };

  /* find the end of the header name */
  எனில்(header_has_மதிப்பு(&header, &out)) {
    *valp = curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
    எனில்(*valp)
      திரும்பு CURLE_OK;
    திரும்பு CURLE_OUT_OF_MEMORY;
  }
  /* bad input */
  *valp = வெற்று;
  திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
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
எழுத்து *Curl_copy_header_மதிப்பு(மாறிலி எழுத்து *header)
{
  கட்டமைப்பு Curl_str out;

  /* find the end of the header name */
  எனில்(!curlx_str_until(&header, &out, MAX_HTTP_RESP_HEADER_SIZE, ':') &&
     !curlx_str_single(&header, ':')) {
    curlx_str_untilnl(&header, &out, MAX_HTTP_RESP_HEADER_SIZE);
    curlx_str_trimblanks(&out);
    திரும்பு curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
  }
  /* bad input, should never happen */
  DEBUGASSERT(0);
  திரும்பு வெற்று;
}

#ifndef CURL_DISABLE_HTTP_AUTH

#ifndef CURL_DISABLE_BASIC_AUTH
/*
 * http_output_basic() sets up an Authorization: header (or the proxy version)
 * for HTTP Basic authentication.
 *
 * Returns CURLcode.
 */
நிலையான CURLcode http_output_basic(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு connectdata *conn, bool proxy)
{
  size_t size = 0;
  எழுத்து *authorization = வெற்று;
  எழுத்து **p_hd;
  CURLcode result;
  கட்டமைப்பு Curl_creds *creds = வெற்று;
  எழுத்து *out;

  /* credentials are unique per transfer for HTTP, do not use the ones for the
     connection */
  எனில்(proxy) {
#ifndef CURL_DISABLE_PROXY
    p_hd = &data->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
#இல்லையெனில்
    (வெறுமை)conn;
    திரும்பு CURLE_NOT_BUILT_IN;
#endif
  }
  இல்லையெனில் {
    p_hd = &data->req.hd_auth;
    creds = data->state.creds;
  }

  எனில்(!creds) {
    DEBUGASSERT(0);
    திரும்பு CURLE_FAILED_INIT;
  }

  out = curl_maprintf("%s:%s", creds->user, creds->passwd);
  எனில்(!out)
    திரும்பு CURLE_OUT_OF_MEMORY;

  result = curlx_base64_encode((uint8_t *)out, strlen(out),
                               &authorization, &size);
  எனில்(result)
    செல் fail;

  எனில்(!authorization) {
    result = CURLE_REMOTE_ACCESS_DENIED;
    செல் fail;
  }

  curlx_free(*p_hd);
  *p_hd = curl_maprintf("%sAuthorization: Basic %s\r\n",
                        proxy ? "Proxy-" : "",
                        authorization);
  curlx_free(authorization);
  எனில்(!*p_hd) {
    result = CURLE_OUT_OF_MEMORY;
    செல் fail;
  }

fail:
  curlx_free(out);
  திரும்பு result;
}

#endif

#ifndef CURL_DISABLE_BEARER_AUTH
/*
 * http_output_bearer() sets up an Authorization: header
 * for HTTP Bearer authentication.
 *
 * Returns CURLcode.
 */
நிலையான CURLcode http_output_bearer(கட்டமைப்பு Curl_easy *data)
{
  எழுத்து **userp;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(Curl_creds_has_oauth_bearer(data->state.creds));
  userp = &data->req.hd_auth;
  curlx_free(*userp);
  *userp = curl_maprintf("Authorization: Bearer %s\r\n",
                         Curl_creds_oauth_bearer(data->state.creds));

  எனில்(!*userp) {
    result = CURLE_OUT_OF_MEMORY;
    செல் fail;
  }

fail:
  திரும்பு result;
}
#endif

#endif

/* pickoneauth() selects the most favorable authentication method from the
 * ones available and the ones we want.
 *
 * return TRUE if one was picked
 */
நிலையான bool pickoneauth(கட்டமைப்பு auth *pick, unsigned long mask,
                        கட்டமைப்பு Curl_creds *creds)
{
  bool have_user_pass = Curl_creds_has_user_or_pass(creds);
  bool picked;
  /* only deal with authentication we want */
  unsigned long avail = pick->avail & pick->want & mask;
  picked = TRUE;

  /* The order of these checks is highly relevant, as this will be the order
     of preference in case of the existence of multiple accepted types. */
  எனில்(avail & CURLAUTH_NEGOTIATE)  /* available on empty creds */
    pick->picked = CURLAUTH_NEGOTIATE;
#ifndef CURL_DISABLE_BEARER_AUTH
  இல்லையெனில் எனில்((avail & CURLAUTH_BEARER) && Curl_creds_has_oauth_bearer(creds))
    pick->picked = CURLAUTH_BEARER;
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  இல்லையெனில் எனில்((avail & CURLAUTH_DIGEST) && have_user_pass)
    pick->picked = CURLAUTH_DIGEST;
#endif
  இல்லையெனில் எனில்(avail & CURLAUTH_NTLM)
    pick->picked = CURLAUTH_NTLM;
#ifndef CURL_DISABLE_BASIC_AUTH
  இல்லையெனில் எனில்((avail & CURLAUTH_BASIC) && have_user_pass)
    pick->picked = CURLAUTH_BASIC;
#endif
#ifndef CURL_DISABLE_AWS
  இல்லையெனில் எனில்(avail & CURLAUTH_AWS_SIGV4)
    pick->picked = CURLAUTH_AWS_SIGV4;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  இல்லையெனில் எனில்(avail & CURLAUTH_HTTPSIG)
    pick->picked = CURLAUTH_HTTPSIG;
#endif
  இல்லையெனில் {
    pick->picked = CURLAUTH_PICKNONE; /* we select to use nothing */
    picked = FALSE;
  }
  pick->avail = CURLAUTH_NONE; /* clear it here */

  திரும்பு picked;
}

/*
 * http_perhapsrewind()
 *
 * The current request needs to be done again - maybe due to a follow
 * or authentication negotiation. Check if:
 * 1) a rewind of the data sent to the server is necessary
 * 2) the current transfer should continue or be stopped early
 */
நிலையான CURLcode http_perhapsrewind(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு connectdata *conn)
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
  VERBOSE(மாறிலி எழுத்து *ongoing_auth = வெற்று);

  /* We need a rewind before uploading client read data again. The
   * checks below influence of the upload is to be continued
   * or aborted early.
   * This depends on how much remains to be sent and in what state
   * the authentication is. Some auth schemes such as NTLM do not work
   * for a new connection. */
  எனில்(needs_rewind) {
    infof(data, "Need to rewind upload for next request");
    Curl_creader_set_rewind(data, TRUE);
  }

  எனில்(conn->bits.close)
    /* If we already decided to close this connection, we cannot veto. */
    திரும்பு CURLE_OK;

  எனில்(abort_upload) {
    /* We would like to abort the upload - but should we? */
#ifdef USE_NTLM
    எனில்((data->state.authproxy.picked == CURLAUTH_NTLM) ||
       (data->state.authhost.picked == CURLAUTH_NTLM)) {
      VERBOSE(ongoing_auth = "NTLM");
      எனில்((conn->http_ntlm_state != NTLMSTATE_NONE) ||
         (conn->proxy_ntlm_state != NTLMSTATE_NONE)) {
        /* The NTLM-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#endif
#ifdef USE_SPNEGO
    /* There is still data left to send */
    எனில்((data->state.authproxy.picked == CURLAUTH_NEGOTIATE) ||
       (data->state.authhost.picked == CURLAUTH_NEGOTIATE)) {
      VERBOSE(ongoing_auth = "NEGOTIATE");
      எனில்((conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) ||
         (conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE)) {
        /* The NEGOTIATE-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#endif
  }

  எனில்(abort_upload) {
    எனில்(upload_remain >= 0)
      infof(data, "%s%sclose instead of sending %" FMT_OFF_T " more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "",
            upload_remain);
    இல்லையெனில்
      infof(data, "%s%sclose instead of sending unknown amount "
            "of more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "");
    /* We decided to abort the ongoing transfer */
    streamclose(conn);
    data->req.size = 0; /* do not download any more than 0 bytes */
    data->req.http_bodyless = TRUE;
  }
  திரும்பு CURLE_OK;
}

/**
 * http_should_fail() determines whether an HTTP response code has gotten us
 * into an error state or not.
 *
 * @retval FALSE communications should continue
 *
 * @retval TRUE communications should not continue
 */
நிலையான bool http_should_fail(கட்டமைப்பு Curl_easy *data, முழு httpcode)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  /*
   * If we have not been asked to fail on error,
   * do not fail.
   */
  எனில்(!data->set.http_fail_on_error)
    திரும்பு FALSE;

  /*
   * Any code < 400 is never terminal.
   */
  எனில்(httpcode < 400)
    திரும்பு FALSE;

  /*
   * A 416 response to a resume request is presumably because the file is
   * already completely downloaded and thus not actually a fail.
   */
  எனில்(data->state.resume_from && data->state.httpreq == HTTPREQ_GET &&
     httpcode == 416)
    திரும்பு FALSE;

  /*
   * Any code >= 400 that is not 401 or 407 is always
   * a terminal error
   */
  எனில்((httpcode != 401) && (httpcode != 407))
    திரும்பு TRUE;

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
  எனில்((httpcode == 401) && !data->state.creds)
    திரும்பு TRUE;
#ifndef CURL_DISABLE_PROXY
  எனில்((httpcode == 407) && !data->conn->http_proxy.creds)
    திரும்பு TRUE;
#endif

  திரும்பு (bool)data->state.authproblem;
}

/*
 * Curl_http_auth_act() gets called when all HTTP headers have been received
 * and it checks what authentication methods that are available and decides
 * which one (if any) to use. It will set 'newurl' if an auth method was
 * picked.
 */
CURLcode Curl_http_auth_act(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  bool pickhost = FALSE;
  bool pickproxy = FALSE;
  CURLcode result = CURLE_OK;
  unsigned long authmask = ~0UL;

  எனில்(!Curl_creds_has_oauth_bearer(data->state.creds))
    authmask &= (unsigned long)~CURLAUTH_BEARER;

  எனில்(100 <= data->req.httpcode && data->req.httpcode <= 199)
    /* this is a transient response code, ignore */
    திரும்பு CURLE_OK;

  எனில்(data->state.authproblem)
    திரும்பு data->set.http_fail_on_error ? CURLE_HTTP_RETURNED_ERROR : CURLE_OK;

  எனில்(data->state.creds &&
     ((data->req.httpcode == 401) ||
      (data->req.authneg && data->req.httpcode < 300))) {
    pickhost = pickoneauth(&data->state.authhost, authmask, data->state.creds);
    எனில்(!pickhost)
      data->state.authproblem = TRUE;
    இல்லையெனில்
      data->info.httpauthpicked = data->state.authhost.picked;
    எனில்(data->state.authhost.picked == CURLAUTH_NTLM &&
       (data->req.httpversion_sent > 11)) {
      infof(data, "Forcing HTTP/1.1 for NTLM");
      connclose(conn);
      data->state.http_neg.wanted = CURL_HTTP_V1x;
      data->state.http_neg.allowed = CURL_HTTP_V1x;
    }
  }
#ifndef CURL_DISABLE_PROXY
  எனில்(conn->http_proxy.creds &&
     ((data->req.httpcode == 407) ||
      (data->req.authneg && data->req.httpcode < 300))) {
    pickproxy = pickoneauth(&data->state.authproxy,
                            authmask & ~CURLAUTH_BEARER,
                            conn->http_proxy.creds);
    எனில்(!pickproxy)
      data->state.authproblem = TRUE;
    இல்லையெனில்
      data->info.proxyauthpicked = data->state.authproxy.picked;
  }
#endif

  எனில்(pickhost || pickproxy) {
    result = http_perhapsrewind(data, conn);
    எனில்(result)
      திரும்பு result;

    /* In case this is GSS auth, the newurl field is already allocated so
       we must make sure to free it before allocating a new one. As figured
       out in bug #2284386 */
    curlx_free(data->req.newurl);
    /* clone URL */
    data->req.newurl = Curl_bufref_dup(&data->state.url);
    எனில்(!data->req.newurl)
      திரும்பு CURLE_OUT_OF_MEMORY;
  }
  இல்லையெனில் எனில்((data->req.httpcode < 300) &&
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
    எனில்(!data->req.newurl)
      திரும்பு CURLE_OUT_OF_MEMORY;
    data->state.authhost.done = TRUE;
  }
  எனில்(http_should_fail(data, data->req.httpcode)) {
    failf(data, "The requested URL returned error: %d",
          data->req.httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
  }

  திரும்பு result;
}

#ifndef CURL_DISABLE_HTTP_AUTH
/*
 * Output the correct authentication header depending on the auth type
 * and whether or not it is to a proxy.
 */
நிலையான CURLcode output_auth_headers(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு connectdata *conn,
                                    கட்டமைப்பு auth *authstatus,
                                    மாறிலி எழுத்து *request,
                                    மாறிலி எழுத்து *path,
                                    bool proxy)
{
  மாறிலி எழுத்து *auth = வெற்று;
  CURLcode result = CURLE_OK;
  (வெறுமை)conn;

#ifdef CURL_DISABLE_DIGEST_AUTH
  (வெறுமை)request;
  (வெறுமை)path;
#endif
#ifndef CURL_DISABLE_AWS
  எனில்((authstatus->picked == CURLAUTH_AWS_SIGV4) && !proxy) {
    /* this method is never for proxy */
    auth = "AWS_SIGV4";
    result = Curl_output_aws_sigv4(data);
    எனில்(result)
      திரும்பு result;
  }
  இல்லையெனில்
#endif
#ifndef CURL_DISABLE_HTTPSIG
  எனில்((authstatus->picked == CURLAUTH_HTTPSIG) && !proxy) {
    /* HTTPSIG uses its own configured key material rather than
       data->state.creds. Do not let unrelated credentials from a
       redirected URL bypass the cross-host auth boundary. */
    எனில்(Curl_auth_allowed_to_host(data)) {
      auth = "HTTPSIG";
      result = Curl_output_httpsig(data);
      எனில்(result)
        திரும்பு result;
    }
    இல்லையெனில்
      authstatus->done = TRUE;
  }
  இல்லையெனில்
#endif
#ifdef USE_SPNEGO
  எனில்(authstatus->picked == CURLAUTH_NEGOTIATE) {
    எனில்(
#ifndef CURL_DISABLE_PROXY
      (proxy && !Curl_checkProxyheaders(data, conn,
                                        STRCONST("Proxy-authorization"))) ||
#endif
      (!proxy && !Curl_checkheaders(data, STRCONST("Authorization")) &&
       Curl_auth_allowed_to_host(data))) {
      auth = "Negotiate";
      result = Curl_output_negotiate(data, conn, proxy);
      எனில்(result)
        திரும்பு result;
    }
    இல்லையெனில்
      authstatus->done = TRUE;
  }
  இல்லையெனில்
#endif
#ifdef USE_NTLM
  எனில்(authstatus->picked == CURLAUTH_NTLM) {
    எனில்(proxy || Curl_auth_allowed_to_host(data)) {
      auth = "NTLM";
      result = Curl_output_ntlm(data, proxy);
      எனில்(result)
        திரும்பு result;
    }
    இல்லையெனில்
      authstatus->done = TRUE;
  }
  இல்லையெனில்
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  எனில்(authstatus->picked == CURLAUTH_DIGEST) {
    auth = "Digest";
    result = Curl_output_digest(data,
                                proxy,
                                (மாறிலி unsigned எழுத்து *)request,
                                (மாறிலி unsigned எழுத்து *)path);
    எனில்(result)
      திரும்பு result;
  }
  இல்லையெனில்
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
  எனில்(authstatus->picked == CURLAUTH_BASIC) {
    /* Basic */
    எனில்(
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
      எனில்(result)
        திரும்பு result;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
  எனில்(authstatus->picked == CURLAUTH_BEARER) {
    /* Bearer */
    எனில்(!proxy && Curl_creds_has_oauth_bearer(data->state.creds) &&
       !Curl_checkheaders(data, STRCONST("Authorization"))) {
      auth = "Bearer";
      result = http_output_bearer(data);
      எனில்(result)
        திரும்பு result;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif

  எனில்(auth) {
#ifndef CURL_DISABLE_PROXY
    எனில்(proxy)
      data->info.proxyauthpicked = authstatus->picked;
    இல்லையெனில்
      data->info.httpauthpicked = authstatus->picked;
    infof(data, "%s auth using %s with user '%s'",
          proxy ? "Proxy" : "Server", auth,
          proxy ? (conn->http_proxy.creds ?
                   conn->http_proxy.creds->user : "") :
          (data->state.creds ?
           data->state.creds->user : ""));
#இல்லையெனில்
    (வெறுமை)proxy;
    infof(data, "Server auth using %s with user '%s'",
          auth, data->state.creds ?
          data->state.creds->user : "");
#endif
    authstatus->multipass = !authstatus->done;
  }
  இல்லையெனில் {
    authstatus->multipass = FALSE;
    எனில்(proxy)
      data->info.proxyauthpicked = 0;
    இல்லையெனில்
      data->info.httpauthpicked = 0;
  }

  திரும்பு result;
}

CURLcode Curl_http_output_auth(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு connectdata *conn,
                               மாறிலி எழுத்து *request,
                               Curl_HttpReq httpreq,
                               மாறிலி எழுத்து *path,
                               மாறிலி எழுத்து *query,
                               bool is_connect)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு auth *authhost;
  கட்டமைப்பு auth *authproxy;
  மாறிலி எழுத்து *path_and_query = path;
  எழுத்து *tmp_str = வெற்று;

  DEBUGASSERT(data);
  authhost = &data->state.authhost;
  authproxy = &data->state.authproxy;

  எனில்(
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
    செல் out;
  }

  எனில்(query) {
    tmp_str = curl_maprintf("%s?%s", path, query);
    எனில்(!tmp_str) {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }
    path_and_query = tmp_str;
  }

  எனில்(authhost->want && !authhost->picked)
    /* The app has selected one or more methods, but none has been picked
       so far by a server round-trip. Then we set the picked one to the
       want one, and if this is one single bit it will be used instantly. */
    authhost->picked = authhost->want;

  எனில்(authproxy->want && !authproxy->picked)
    /* The app has selected one or more methods, but none has been picked so
       far by a proxy round-trip. Then we set the picked one to the want one,
       and if this is one single bit it will be used instantly. */
    authproxy->picked = authproxy->want;

#ifndef CURL_DISABLE_PROXY
  /* Send proxy authentication header if needed */
  எனில்(conn->bits.origin_is_proxy || is_connect) {
    result = output_auth_headers(data, conn, authproxy, request,
                                 path_and_query, TRUE);
    எனில்(result)
      செல் out;
  }
  இல்லையெனில்
#இல்லையெனில்
  (வெறுமை)is_connect;
#endif /* CURL_DISABLE_PROXY */
    /* we have no proxy so let's pretend we are done authenticating
       with it */
    authproxy->done = TRUE;

  /* Either we have credentials for the origin we talk to or
     performing authentication is allowed here */
  எனில்(data->state.creds || Curl_auth_allowed_to_host(data))
    result = output_auth_headers(data, conn, authhost, request,
                                 path_and_query, FALSE);
  இல்லையெனில்
    authhost->done = TRUE;

  எனில்(((authhost->multipass && !authhost->done) ||
      (authproxy->multipass && !authproxy->done)) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_HEAD)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    data->req.authneg = TRUE;
  }
  இல்லையெனில்
    data->req.authneg = FALSE;

out:
  curlx_free(tmp_str);
  திரும்பு result;
}

#இல்லையெனில் /* !CURL_DISABLE_HTTP_AUTH */
/* when disabled */
CURLcode Curl_http_output_auth(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு connectdata *conn,
                               மாறிலி எழுத்து *request,
                               Curl_HttpReq httpreq,
                               மாறிலி எழுத்து *path,
                               மாறிலி எழுத்து *query,
                               bool is_connect)
{
  (வெறுமை)data;
  (வெறுமை)conn;
  (வெறுமை)request;
  (வெறுமை)httpreq;
  (வெறுமை)path;
  (வெறுமை)query;
  (வெறுமை)is_connect;
  திரும்பு CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP_AUTH, else */

#எனில் defined(USE_SPNEGO) || defined(USE_NTLM) || \
  !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_BASIC_AUTH) || \
  !defined(CURL_DISABLE_BEARER_AUTH)
நிலையான bool authcmp(மாறிலி எழுத்து *auth, மாறிலி எழுத்து *வரி)
{
  /* the auth string must not have an alnum following */
  size_t n = strlen(auth);
  திரும்பு curl_strnequal(auth, வரி, n) && !ISALNUM(வரி[n]);
}
#endif

#ifdef USE_SPNEGO
நிலையான CURLcode auth_spnego(கட்டமைப்பு Curl_easy *data,
                            bool proxy,
                            மாறிலி எழுத்து *auth,
                            கட்டமைப்பு auth *authp,
                            uint32_t *availp)
{
  எனில்((authp->avail & CURLAUTH_NEGOTIATE) || Curl_auth_is_spnego_supported()) {
    *availp |= CURLAUTH_NEGOTIATE;
    authp->avail |= CURLAUTH_NEGOTIATE;

    எனில்(authp->picked == CURLAUTH_NEGOTIATE) {
      கட்டமைப்பு connectdata *conn = data->conn;
      CURLcode result = Curl_உள்ளீடு_negotiate(data, conn, proxy, auth);
      uint8_t *negstate = proxy ? &conn->proxy_negotiate_state :
        &conn->http_negotiate_state;
      எனில்(!result) {
        curlx_free(data->req.newurl);
        data->req.newurl = Curl_bufref_dup(&data->state.url);
        எனில்(!data->req.newurl)
          திரும்பு CURLE_OUT_OF_MEMORY;
        data->state.authproblem = FALSE;
        /* we received a GSS auth token and we dealt with it fine */
        *negstate = CURL_NEGO_AUTH_RECV;
      }
      இல்லையெனில்
        data->state.authproblem = TRUE;
    }
  }
  திரும்பு CURLE_OK;
}
#endif

#ifdef USE_NTLM
நிலையான CURLcode auth_ntlm(கட்டமைப்பு Curl_easy *data,
                          bool proxy,
                          மாறிலி எழுத்து *auth,
                          கட்டமைப்பு auth *authp,
                          uint32_t *availp)
{
  /* NTLM support requires the SSL crypto libs */
  எனில்((authp->avail & CURLAUTH_NTLM) || Curl_auth_is_ntlm_supported()) {
    *availp |= CURLAUTH_NTLM;
    authp->avail |= CURLAUTH_NTLM;

    எனில்(authp->picked == CURLAUTH_NTLM) {
      /* NTLM authentication is picked and activated */
      CURLcode result = Curl_உள்ளீடு_ntlm(data, proxy, auth);
      எனில்(!result)
        data->state.authproblem = FALSE;
      இல்லையெனில் {
        எனில்(result == CURLE_OUT_OF_MEMORY)
          திரும்பு result;
        infof(data, "NTLM authentication problem, ignoring.");
        data->state.authproblem = TRUE;
      }
    }
  }
  திரும்பு CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_DIGEST_AUTH
நிலையான CURLcode auth_digest(கட்டமைப்பு Curl_easy *data,
                            bool proxy,
                            மாறிலி எழுத்து *auth,
                            கட்டமைப்பு auth *authp,
                            uint32_t *availp)
{
  எனில்(authp->avail & CURLAUTH_DIGEST) {
    *availp |= CURLAUTH_DIGEST;
    infof(data, "Ignoring duplicate digest auth header.");
  }
  இல்லையெனில் எனில்(Curl_auth_is_digest_supported()) {
    CURLcode result;

    *availp |= CURLAUTH_DIGEST;
    authp->avail |= CURLAUTH_DIGEST;

    /* We call this function on input Digest headers even if Digest
     * authentication is not activated yet, as we need to store the
     * incoming data from this header in case we are going to use
     * Digest */
    result = Curl_உள்ளீடு_digest(data, proxy, auth);
    எனில்(result) {
      எனில்(result == CURLE_OUT_OF_MEMORY)
        திரும்பு result;
      infof(data, "Digest authentication problem, ignoring.");
      data->state.authproblem = TRUE;
    }
  }
  திரும்பு CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BASIC_AUTH
நிலையான CURLcode auth_basic(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு auth *authp,
                           uint32_t *availp)
{
  *availp |= CURLAUTH_BASIC;
  authp->avail |= CURLAUTH_BASIC;
  எனில்(authp->picked == CURLAUTH_BASIC) {
    /* We asked for Basic authentication but got a 40X back anyway, which
       means our name+password is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(data, "Basic authentication problem, ignoring.");
    data->state.authproblem = TRUE;
  }
  திரும்பு CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BEARER_AUTH
நிலையான CURLcode auth_bearer(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு auth *authp,
                            uint32_t *availp)
{
  *availp |= CURLAUTH_BEARER;
  authp->avail |= CURLAUTH_BEARER;
  எனில்(authp->picked == CURLAUTH_BEARER) {
    /* We asked for Bearer authentication but got a 40X back anyway, which
       means our token is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(data, "Bearer authentication problem, ignoring.");
    data->state.authproblem = TRUE;
  }
  திரும்பு CURLE_OK;
}
#endif

/*
 * Curl_http_input_auth() deals with Proxy-Authenticate: and WWW-Authenticate:
 * headers. They are dealt with both in the transfer.c main loop and in the
 * proxy CONNECT loop.
 *
 * The 'auth' line ends with a NUL byte without CR or LF present.
 */
CURLcode Curl_http_உள்ளீடு_auth(கட்டமைப்பு Curl_easy *data, bool proxy,
                              மாறிலி எழுத்து *auth) /* the first non-space */
{
  /*
   * This resource requires authentication
   */
#எனில் defined(USE_SPNEGO) ||                      \
  defined(USE_NTLM) ||                          \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||         \
  !defined(CURL_DISABLE_BASIC_AUTH) ||          \
  !defined(CURL_DISABLE_BEARER_AUTH)

  uint32_t *availp;
  கட்டமைப்பு auth *authp;
  CURLcode result = CURLE_OK;
  DEBUGASSERT(auth);
  DEBUGASSERT(data);

  எனில்(proxy) {
    availp = &data->info.proxyauthavail;
    authp = &data->state.authproxy;
  }
  இல்லையெனில் {
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

  போது(*auth) {
#ifdef USE_SPNEGO
    எனில்(authcmp("Negotiate", auth))
      result = auth_spnego(data, proxy, auth, authp, availp);
#endif
#ifdef USE_NTLM
    எனில்(!result && authcmp("NTLM", auth))
      result = auth_ntlm(data, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
    எனில்(!result && authcmp("Digest", auth))
      result = auth_digest(data, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
    எனில்(!result && authcmp("Basic", auth))
      result = auth_basic(data, authp, availp);
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
    எனில்(authcmp("Bearer", auth))
      result = auth_bearer(data, authp, availp);
#endif

    எனில்(result)
      நிறுத்து;

    /* there may be multiple methods on one line, so keep reading */
    auth = strchr(auth, ',');
    எனில்(auth) /* if we are on a comma, skip it */
      auth++;
    இல்லையெனில்
      நிறுத்து;
    curlx_str_passblanks(&auth);
  }
  திரும்பு result;
#இல்லையெனில்
  (வெறுமை)data;
  (வெறுமை)proxy;
  (வெறுமை)auth;
  /* nothing to do when disabled */
  திரும்பு CURLE_OK;
#endif
}

நிலையான வெறுமை http_switch_to_get(கட்டமைப்பு Curl_easy *data, முழு code)
{
  மாறிலி எழுத்து *req = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  எனில்((req || data->state.httpreq != HTTPREQ_GET) &&
     (data->set.http_follow_mode == CURLFOLLOW_OBEYCODE)) {
    NOVERBOSE((வெறுமை)code);
    infof(data, "Switch to GET because of %d response", code);
    data->state.http_ignorecustom = TRUE;
  }
  இல்லையெனில் எனில்(req && (data->set.http_follow_mode != CURLFOLLOW_FIRSTONLY))
    infof(data, "Stick to %s instead of GET", req);

  data->state.httpreq = HTTPREQ_GET;
  Curl_creader_set_rewind(data, FALSE);
}

#define HTTPREQ_IS_POST(data)                    \
  ((data)->state.httpreq == HTTPREQ_POST ||      \
   (data)->state.httpreq == HTTPREQ_POST_FORM || \
   (data)->state.httpreq == HTTPREQ_POST_MIME)

நிலையான CURLcode set_auto_referer(கட்டமைப்பு Curl_easy *data)
{
  CURLU *u;
  எழுத்து *referer = வெற்று;
  CURLUcode uc;

  /* We are asked to automatically set the previous URL as the referer
     when we get the next URL. We pick the ->url field, which may or may
     not be 100% correct */
  Curl_bufref_free(&data->state.referer);

  /* Make a copy of the URL without credentials and fragment */
  u = curl_url();
  எனில்(!u)
    திரும்பு CURLE_OUT_OF_MEMORY;

  uc = curl_url_set(u, CURLUPART_URL,
                    Curl_bufref_ptr(&data->state.url), 0);
  எனில்(!uc)
    uc = curl_url_set(u, CURLUPART_FRAGMENT, வெற்று, 0);
  எனில்(!uc)
    uc = curl_url_set(u, CURLUPART_USER, வெற்று, 0);
  எனில்(!uc)
    uc = curl_url_set(u, CURLUPART_PASSWORD, வெற்று, 0);
  எனில்(!uc)
    uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

  curl_url_cleanup(u);

  எனில்(uc || !referer)
    திரும்பு CURLE_OUT_OF_MEMORY;

  Curl_bufref_set(&data->state.referer, referer, 0, curl_free);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode follow_parse_url(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *newurl,
                                 followtype type, எழுத்து **follow_urlp,
                                 CURLUcode *ucp)
{
  CURLUcode uc;

  DEBUGASSERT(data->state.uh);
  uc = curl_url_set(data->state.uh, CURLUPART_URL, newurl, (unsigned முழு)
                    ((type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                     ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) |
                     CURLU_ALLOW_SPACE |
                     (data->set.path_as_is ? CURLU_PATH_AS_IS : 0)));
  *ucp = uc;
  எனில்(uc) {
    எனில்((uc == CURLUE_OUT_OF_MEMORY) || (type != FOLLOW_FAKE)) {
      failf(data, "The redirect target URL could not be parsed: %s",
            curl_url_strerror(uc));
      திரும்பு Curl_uc_to_curlcode(uc);
    }

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, duplicate the field as-is */
    *follow_urlp = curlx_strdup(newurl);
    எனில்(!*follow_urlp)
      திரும்பு CURLE_OUT_OF_MEMORY;
  }
  இல்லையெனில் {
    CURLU *u = curl_url();
    எனில்(!u)
      திரும்பு CURLE_OUT_OF_MEMORY;
    uc = curl_url_set(u, CURLUPART_URL,
                      Curl_bufref_ptr(&data->state.url),
                      CURLU_URLENCODE | CURLU_ALLOW_SPACE);
    எனில்(!uc)
      uc = curl_url_get(data->state.uh, CURLUPART_URL, follow_urlp, 0);
    எனில்(uc) {
      curl_url_cleanup(u);
      திரும்பு Curl_uc_to_curlcode(uc);
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    {
      bool same_origin = Curl_url_same_origin(u, data->state.uh);
      curl_url_cleanup(u);
      எனில்(!same_origin)
        Curl_auth_digest_cleanup(&data->state.digest);
    }
#இல்லையெனில்
    curl_url_cleanup(u);
#endif
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode follow_fake(கட்டமைப்பு Curl_easy *data, எழுத்து *follow_url,
                            CURLUcode uc, bool reachedmax)
{
  /* we are only figuring out the new URL if we would have followed locations
     but now we are done so we can get out! */
  எனில்(!uc) {
    CURLU *u = curl_url();
    எழுத்து *nocred = வெற்று;

    எனில்(!u) {
      curlx_free(follow_url);
      திரும்பு CURLE_OUT_OF_MEMORY;
    }
    எனில்(!curl_url_set(u, CURLUPART_URL, follow_url,
                     CURLU_NON_SUPPORT_SCHEME |
                     (data->set.path_as_is ? CURLU_PATH_AS_IS : 0)) &&
       !curl_url_set(u, CURLUPART_USER, வெற்று, 0) &&
       !curl_url_set(u, CURLUPART_PASSWORD, வெற்று, 0))
      (வெறுமை)curl_url_get(u, CURLUPART_URL, &nocred, CURLU_GET_EMPTY);
    curl_url_cleanup(u);
    எனில்(nocred) {
      curlx_free(follow_url);
      follow_url = nocred;
    }
  }
  data->info.wouldredirect = follow_url;

  எனில்(reachedmax) {
    failf(data, "Maximum (%d) redirects followed", data->set.maxredirs);
    திரும்பு CURLE_TOO_MANY_REDIRECTS;
  }
  திரும்பு CURLE_OK;
}

நிலையான bool follow_switch_get(கட்டமைப்பு Curl_easy *data)
{
  /*
   * We get here when the HTTP code is 300-399 and 401 or 407. We need to
   * perform differently based on exactly what return code there was.
   */
  தேர்வு(data->info.httpcode) {
  நிலை 301: /* Moved Permanently */
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
    எனில்(HTTPREQ_IS_POST(data) && !data->set.post301) {
      http_switch_to_get(data, 301);
      திரும்பு TRUE;
    }
    நிறுத்து;
  நிலை 302: /* Found */
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
    எனில்(HTTPREQ_IS_POST(data) && !data->set.post302) {
      http_switch_to_get(data, 302);
      திரும்பு TRUE;
    }
    நிறுத்து;
  நிலை 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     */
    எனில்(!HTTPREQ_IS_POST(data) || !data->set.post303) {
      http_switch_to_get(data, 303);
      திரும்பு TRUE;
    }
    நிறுத்து;

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
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு FALSE;
}

CURLcode Curl_http_follow(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *newurl,
                          followtype type)
{
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  எழுத்து *follow_url = வெற்று;
  CURLUcode uc;
  CURLcode result;
  CURLcode rewind_result;
  bool switch_to_get;

  DEBUGASSERT(type != FOLLOW_NONE);

  எனில்(type != FOLLOW_FAKE)
    data->state.requests++; /* count all real follows */
  எனில்(type == FOLLOW_REDIR) {
    எனில்((data->set.maxredirs != -1) &&
       (data->state.followlocation >= data->set.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    இல்லையெனில் {
      data->state.followlocation++; /* count redirect-followings, including
                                       auth reloads */

      எனில்(data->set.http_auto_referer) {
        result = set_auto_referer(data);
        எனில்(result)
          திரும்பு result;
      }
    }
  }

  எனில்((type != FOLLOW_RETRY) &&
     (data->req.httpcode != 401) && (data->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, வெற்று, 0, FALSE)) {
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: do not allow a custom port number */
    disallowport = TRUE;
  }

  result = follow_parse_url(data, newurl, type, &follow_url, &uc);
  எனில்(result)
    திரும்பு result;

  DEBUGASSERT(follow_url);

  எனில்(type == FOLLOW_FAKE)
    திரும்பு follow_fake(data, follow_url, uc, reachedmax);

  எனில்(disallowport)
    data->state.allow_port = FALSE;

  Curl_bufref_set(&data->state.url, follow_url, 0, curl_free);
  rewind_result = Curl_req_soft_reset(&data->req, data);
  infof(data, "Issue another request to this URL: '%s'", follow_url);
  எனில்((data->set.http_follow_mode == CURLFOLLOW_FIRSTONLY) &&
     !data->state.http_ignorecustom &&
     CURL_EASY_STR(data, STRING_CUSTOMREQUEST)) {
    data->state.http_ignorecustom = TRUE;
    infof(data, "Drop custom request method for next request");
  }

  switch_to_get = follow_switch_get(data);

  /* When rewind of upload data failed and we are not switching to GET,
   * we need to fail the follow, as we cannot send the data again. */
  எனில்(rewind_result && !switch_to_get)
    திரும்பு rewind_result;

  Curl_pgrsTime(data, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(data);

  திரும்பு CURLE_OK;
}

/*
 * Curl_compareheader()
 *
 * Returns TRUE if 'headerline' contains the 'header' with given 'content'
 * (within a comma-separated list of tokens). Pass 'header' WITH the colon.
 *
 * @unittest: 1625
 */
bool Curl_compareheader(மாறிலி எழுத்து *headerline, /* line to check */
                        மாறிலி எழுத்து *header, /* header keyword _with_ colon */
                        மாறிலி size_t hlen, /* len of the keyword in bytes */
                        மாறிலி எழுத்து *content, /* content string to find */
                        மாறிலி size_t clen) /* len of the content in bytes */
{
  /* RFC2616, section 4.2 says: "Each header field consists of a name followed
   * by a colon (":") and the field value. Field names are case-insensitive.
   * The field value MAY be preceded by any amount of LWS, though a single SP
   * is preferred." */

  மாறிலி எழுத்து *p;
  கட்டமைப்பு Curl_str val;
  DEBUGASSERT(hlen);
  DEBUGASSERT(clen);
  DEBUGASSERT(header);
  DEBUGASSERT(content);

  எனில்(!curl_strnequal(headerline, header, hlen))
    திரும்பு FALSE; /* does not start with header */

  /* pass the header */
  p = &headerline[hlen];

  எனில்(curlx_str_cspn(&p, &val, "\r\n"))
    திரும்பு FALSE;
  curlx_str_trimblanks(&val);

  /* find the content string in the rest of the line */
  எனில்(curlx_strlen(&val) >= clen) {
    size_t len;
    p = curlx_str(&val);
    க்கு(len = curlx_strlen(&val); len >= clen;) {
      கட்டமைப்பு Curl_str next;
      மாறிலி எழுத்து *o = p;
      /* after a match there must be a comma, space, newline or NUL byte */
      எனில்(curl_strnequal(p, content, clen) &&
         ((p[clen] == ',') || ISBLANK(p[clen]) || ISNEWLINE(p[clen]) ||
          !p[clen]))
        திரும்பு TRUE; /* match! */
      /* advance to the next comma */
      எனில்(curlx_str_until(&p, &next, len, ',') ||
         curlx_str_single(&p, ','))
        நிறுத்து; /* no comma, get out */

      /* if there are more dummy commas, move over them as well */
      செய்
        curlx_str_passblanks(&p);
      போது(!curlx_str_single(&p, ','));
      /* trailing blanks may move the parsing point past the value end,
         then there is nothing left to match */
      எனில்((size_t)(p - o) > len)
        நிறுத்து;
      len -= (p - o);
    }
  }
  திரும்பு FALSE; /* no match */
}

கட்டமைப்பு cr_exp100_ctx {
  கட்டமைப்பு Curl_creader super;
  கட்டமைப்பு curltime start; /* time started waiting */
  எண்ணிக்கை expect100 state;
};

/* Expect: 100-continue client reader, blocking uploads */

நிலையான வெறுமை http_exp100_continue(கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு Curl_creader *reader)
{
  கட்டமைப்பு cr_exp100_ctx *ctx = reader->ctx;
  எனில்(ctx->state > EXP100_SEND_DATA) {
    ctx->state = EXP100_SEND_DATA;
    Curl_expire_clear(data, EXPIRE_100_TIMEOUT);
  }
}

நிலையான CURLcode cr_exp100_read(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_creader *reader,
                               எழுத்து *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  கட்டமைப்பு cr_exp100_ctx *ctx = reader->ctx;
  timediff_t ms;

  தேர்வு(ctx->state) {
  நிலை EXP100_SENDING_REQUEST:
    எனில்(!Curl_req_sendbuf_empty(data)) {
      /* The initial request data has not been fully sent yet. Do
       * not start the timer yet. */
      DEBUGF(infof(data, "cr_exp100_read, request not full sent yet"));
      *nread = 0;
      *eos = FALSE;
      திரும்பு CURLE_OK;
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
    திரும்பு CURLE_OK;
  நிலை EXP100_FAILED:
    DEBUGF(infof(data, "cr_exp100_read, expectation failed, error"));
    *nread = 0;
    *eos = FALSE;
    திரும்பு CURLE_READ_ERROR;
  நிலை EXP100_AWAITING_CONTINUE:
    ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->start);
    எனில்(ms < data->set.expect_100_timeout) {
      DEBUGF(infof(data, "cr_exp100_read, AWAITING_CONTINUE, not expired"));
      *nread = 0;
      *eos = FALSE;
      திரும்பு CURLE_OK;
    }
    /* we have waited long enough, continue anyway */
    http_exp100_continue(data, reader);
    infof(data, "Done waiting for 100-continue");
    FALLTHROUGH();
  இயல்பு:
    DEBUGF(infof(data, "cr_exp100_read, pass through"));
    திரும்பு Curl_creader_read(data, reader->next, buf, blen, nread, eos);
  }
}

நிலையான வெறுமை cr_exp100_done(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு Curl_creader *reader, முழு premature)
{
  கட்டமைப்பு cr_exp100_ctx *ctx = reader->ctx;
  ctx->state = premature ? EXP100_FAILED : EXP100_SEND_DATA;
  Curl_expire_clear(data, EXPIRE_100_TIMEOUT);
}

நிலையான மாறிலி கட்டமைப்பு Curl_crtype cr_exp100 = {
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
  அளவு(கட்டமைப்பு cr_exp100_ctx)
};

நிலையான CURLcode http_exp100_add_reader(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_creader *reader = வெற்று;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_exp100, CURL_CR_PROTOCOL);
  எனில்(!result)
    result = Curl_creader_add(data, reader);
  எனில்(!result) {
    கட்டமைப்பு cr_exp100_ctx *ctx = reader->ctx;
    ctx->state = EXP100_SENDING_REQUEST;
  }

  எனில்(result && reader)
    Curl_creader_free(data, reader);
  திரும்பு result;
}

நிலையான வெறுமை http_exp100_got100(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  எனில்(r)
    http_exp100_continue(data, r);
}

நிலையான bool http_exp100_is_waiting(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  எனில்(r) {
    கட்டமைப்பு cr_exp100_ctx *ctx = r->ctx;
    திரும்பு ctx->state == EXP100_AWAITING_CONTINUE;
  }
  திரும்பு FALSE;
}

நிலையான வெறுமை http_exp100_send_anyway(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  எனில்(r)
    http_exp100_continue(data, r);
}

நிலையான bool http_exp100_is_selected(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  திரும்பு !!r;
}

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
CURLcode Curl_http_doing_pollset(கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு easy_pollset *ps)
{
  /* write mode */
  திரும்பு Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

CURLcode Curl_http_perform_pollset(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு easy_pollset *ps)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  எனில்(CURL_REQ_WANT_RECV(data)) {
    result = Curl_pollset_add_in(data, ps, conn->sock[FIRSTSOCKET]);
  }

  /* on a "Expect: 100-continue" timed wait, do not poll for outgoing */
  எனில்(!result && Curl_req_want_send(data) && !http_exp100_is_waiting(data)) {
    result = Curl_pollset_add_out(data, ps, conn->sock[FIRSTSOCKET]);
  }
  திரும்பு result;
}

நிலையான CURLcode http_write_header(கட்டமைப்பு Curl_easy *data,
                                  மாறிலி எழுத்து *hd, size_t hdlen)
{
  CURLcode result;
  முழு writetype;

  /* now, only output this if the header AND body are requested:
   */
  Curl_debug(data, CURLINFO_HEADER_IN, hd, hdlen);

  writetype = CLIENTWRITE_HEADER |
    ((data->req.httpcode / 100 == 1) ? CLIENTWRITE_1XX : 0);

  result = Curl_client_write(data, writetype, hd, hdlen);
  எனில்(result)
    திரும்பு result;

  result = Curl_bump_headersize(data, hdlen, FALSE);
  எனில்(result)
    திரும்பு result;

  data->req.deductheadercount = (100 <= data->req.httpcode &&
                                 199 >= data->req.httpcode) ?
    data->req.headerbytecount : 0;
  திரும்பு result;
}

/*
 * Curl_http_done() gets called after a single HTTP request has been
 * performed.
 */

CURLcode Curl_http_done(கட்டமைப்பு Curl_easy *data,
                        CURLcode status, bool premature)
{
  கட்டமைப்பு connectdata *conn = data->conn;

  /* Clear multipass flag. If authentication is not done yet, then it will get
   * a chance to be set back to true when we output the next auth header */
  data->state.authhost.multipass = FALSE;
  data->state.authproxy.multipass = FALSE;

  எனில்(curlx_dyn_len(&data->state.headerb)) {
    (வெறுமை)http_write_header(data, curlx_dyn_ptr(&data->state.headerb),
                            curlx_dyn_len(&data->state.headerb));
  }
  curlx_dyn_reset(&data->state.headerb);

  எனில்(status)
    திரும்பு status;

  எனில்(!premature && /* this check is pointless when DONE is called before the
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
    திரும்பு CURLE_GOT_NOTHING;
  }

  திரும்பு CURLE_OK;
}

/* Determine if we may use HTTP 1.1 for this request. */
நிலையான bool http_may_use_1_1(மாறிலி கட்டமைப்பு Curl_easy *data)
{
  மாறிலி கட்டமைப்பு connectdata *conn = data->conn;
  /* We have seen a previous response for *this* transfer with 1.0,
   * on another connection or the same one. */
  எனில்(data->state.http_neg.rcvd_min == 10)
    திரும்பு FALSE;
  /* We have seen a previous response on *this* connection with 1.0. */
  எனில்(conn && conn->httpversion_seen == 10)
    திரும்பு FALSE;
  /* We want 1.0 and have seen no previous response on *this* connection
     with a higher version (maybe no response at all yet). */
  எனில்(data->state.http_neg.only_10 &&
     (!conn || conn->httpversion_seen <= 10))
    திரும்பு FALSE;
  /* We are not restricted to use 1.0 only. */
  திரும்பு !data->state.http_neg.only_10;
}

நிலையான unsigned எழுத்து http_request_version(கட்டமைப்பு Curl_easy *data)
{
  unsigned எழுத்து v = Curl_conn_http_version(data, data->conn);
  எனில்(!v) {
    /* No specific HTTP connection filter installed. */
    v = http_may_use_1_1(data) ? 11 : 10;
  }
  திரும்பு v;
}

நிலையான மாறிலி எழுத்து *get_http_string(முழு httpversion)
{
  தேர்வு(httpversion) {
  நிலை 30:
    திரும்பு "3";
  நிலை 20:
    திரும்பு "2";
  நிலை 11:
    திரும்பு "1.1";
  இயல்பு:
    திரும்பு "1.0";
  }
}

CURLcode Curl_add_custom_headers(கட்டமைப்பு Curl_easy *data,
                                 bool is_connect, முழு httpversion,
                                 கட்டமைப்பு dynbuf *req)
{
  கட்டமைப்பு curl_slist *h[2];
  கட்டமைப்பு curl_slist *headers;
  முழு numlists = 1; /* by default */
  முழு i;

#ifndef CURL_DISABLE_PROXY
  எண்ணிக்கை Curl_proxy_use proxy;

  எனில்(is_connect)
    proxy = HEADER_CONNECT;
  இல்லையெனில்
    proxy = data->conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  தேர்வு(proxy) {
  நிலை HEADER_SERVER:
    h[0] = data->set.headers;
    நிறுத்து;
  நிலை HEADER_PROXY:
    h[0] = data->set.headers;
    எனில்(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    நிறுத்து;
  நிலை HEADER_CONNECT:
    எனில்(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    இல்லையெனில்
      h[0] = data->set.headers;
    நிறுத்து;
  நிலை HEADER_CONNECT_UDP:
    எனில்(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    இல்லையெனில்
      h[0] = data->set.headers;
    நிறுத்து;
  }
#இல்லையெனில்
  (வெறுமை)is_connect;
  h[0] = data->set.headers;
#endif

  /* loop through one or two lists */
  க்கு(i = 0; i < numlists; i++) {
    க்கு(headers = h[i]; headers; headers = headers->next) {
      CURLcode result = CURLE_OK;
      bool blankheader = FALSE;
      கட்டமைப்பு Curl_str name;
      மாறிலி எழுத்து *p = headers->data;
      மாறிலி எழுத்து *origp = p;
      size_t hlen = strlen(origp);

      /* explicitly asked to send header without content is done by a header
         that ends with a semicolon, but there must be no colon present in the
         name */
      எனில்(!curlx_str_until(&p, &name, hlen, ';') &&
         !curlx_str_single(&p, ';') &&
         !curlx_str_single(&p, '\0') &&
         !memchr(curlx_str(&name), ':', curlx_strlen(&name)))
        blankheader = TRUE;
      இல்லையெனில் {
        p = origp;
        எனில்(!curlx_str_until(&p, &name, hlen, ':') &&
           !curlx_str_single(&p, ':')) {
          கட்டமைப்பு Curl_str val;
          curlx_str_untilnl(&p, &val, hlen);
          curlx_str_trimblanks(&val);
          எனில்(!curlx_strlen(&val))
            /* no content, do not send this */
            தொடர்;
        }
        இல்லையெனில்
          /* no colon */
          தொடர்;
      }

      /* a field name is a token and carries no surrounding whitespace, so
         trim the parsed name before matching. Otherwise `Authorization :`
         (space before the colon) slips past the Authorization/Cookie check
         below and gets forwarded to another host on a redirect. */
      curlx_str_trimblanks(&name);

      /* only send this if the contents was non-blank or done special */

      எனில்(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom
            Host: header as that will produce *two* in the same
            request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      இல்லையெனில் எனில்(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      இல்லையெனில் எனில்(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      இல்லையெனில் எனில்(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      இல்லையெனில் எனில்(curlx_str_casecompare(&name, "Connection"))
        /* Connection headers are handled specially */
        ;
      இல்லையெனில் எனில்((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        /* HTTP/2 does not support chunked requests */
        ;
      இல்லையெனில் எனில்((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      இல்லையெனில் எனில்(blankheader) {
        result = curlx_dyn_addn(req, curlx_str(&name), curlx_strlen(&name));
        எனில்(!result)
          result = curlx_dyn_addn(req, STRCONST(":\r\n"));
      }
      இல்லையெனில்
        result = curlx_dyn_addf(req, "%s\r\n", origp);

      எனில்(result)
        திரும்பு result;
    }
  }

  திரும்பு CURLE_OK;
}

#ifndef CURL_DISABLE_PARSEDATE
CURLcode Curl_add_timecondition(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு dynbuf *req)
{
  மாறிலி கட்டமைப்பு tm *tm;
  கட்டமைப்பு tm keeptime;
  CURLcode result;
  எழுத்து datestr[80];
  மாறிலி எழுத்து *condp;
  size_t len;

  எனில்(data->set.timecondition == CURL_TIMECOND_NONE)
    /* no condition was asked for */
    திரும்பு CURLE_OK;

  result = curlx_gmtime(data->set.timevalue, &keeptime);
  எனில்(result) {
    failf(data, "Invalid TIMEVALUE");
    திரும்பு result;
  }
  tm = &keeptime;

  தேர்வு(data->set.timecondition) {
  இயல்பு:
    DEBUGF(infof(data, "invalid time condition"));
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  நிலை CURL_TIMECOND_IFMODSINCE:
    condp = "If-Modified-Since";
    len = 17;
    நிறுத்து;
  நிலை CURL_TIMECOND_IFUNMODSINCE:
    condp = "If-Unmodified-Since";
    len = 19;
    நிறுத்து;
  நிலை CURL_TIMECOND_LASTMOD:
    condp = "Last-Modified";
    len = 13;
    நிறுத்து;
  }

  எனில்(Curl_checkheaders(data, condp, len)) {
    /* A custom header was specified; it will be sent instead. */
    திரும்பு CURLE_OK;
  }

  /* The If-Modified-Since header family should have their times set in
   * GMT as RFC2616 defines: "All HTTP date/time stamps MUST be
   * represented in Greenwich Mean Time (GMT), without exception. For the
   * purposes of HTTP, GMT is exactly equal to UTC (Coordinated Universal
   * Time)." (see page 20 of RFC2616).
   */

  /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
  curl_msnprintf(datestr, அளவு(datestr),
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
  திரும்பு result;
}
#இல்லையெனில்
/* disabled */
CURLcode Curl_add_timecondition(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு dynbuf *req)
{
  (வெறுமை)data;
  (வெறுமை)req;
  திரும்பு CURLE_OK;
}
#endif

வெறுமை Curl_http_method(கட்டமைப்பு Curl_easy *data,
                      மாறிலி எழுத்து **method, Curl_HttpReq *reqp)
{
  Curl_HttpReq httpreq = (Curl_HttpReq)data->state.httpreq;
  மாறிலி எழுத்து *request;
#ifndef CURL_DISABLE_WEBSOCKETS
  எனில்(data->conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
    httpreq = HTTPREQ_GET;
  இல்லையெனில்
#endif
  எனில்((data->conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_FTP)) &&
     data->state.upload)
    httpreq = HTTPREQ_PUT;

  /* Now set the 'request' pointer to the proper request string */
  எனில்(!data->state.http_ignorecustom &&
     CURL_EASY_STR(data, STRING_CUSTOMREQUEST)) {
    request = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);
  }
  இல்லையெனில் {
    எனில்(data->req.no_body)
      request = "HEAD";
    இல்லையெனில் {
      DEBUGASSERT((httpreq >= HTTPREQ_GET) && (httpreq <= HTTPREQ_HEAD));
      தேர்வு(httpreq) {
      நிலை HTTPREQ_POST:
      நிலை HTTPREQ_POST_FORM:
      நிலை HTTPREQ_POST_MIME:
        request = "POST";
        நிறுத்து;
      நிலை HTTPREQ_PUT:
        request = "PUT";
        நிறுத்து;
      இயல்பு: /* this should never happen */
      நிலை HTTPREQ_GET:
        request = "GET";
        நிறுத்து;
      நிலை HTTPREQ_HEAD:
        request = "HEAD";
        நிறுத்து;
      }
    }
  }
  *method = request;
  *reqp = httpreq;
}

நிலையான CURLcode http_set_aptr_host(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  மாறிலி எழுத்து *ptr = வெற்று;

  curlx_safefree(data->state.http_host);
#ifndef CURL_DISABLE_COOKIES
  curlx_safefree(data->req.cookiehost);
#endif

  எனில்(Curl_peer_equal(data->state.initial_origin, data->state.origin))
    ptr = Curl_checkheaders(data, STRCONST("Host"));

  எனில்(ptr) {
#ifndef CURL_DISABLE_COOKIES
    /* If we have a given custom Host: header, we extract the hostname in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the hostname
       is the same as the first one! */
    எழுத்து *cookiehost;
    CURLcode result = copy_custom_மதிப்பு(ptr, &cookiehost);
    எனில்(result)
      திரும்பு result;
    எனில்(!*cookiehost)
      /* ignore empty data */
      curlx_free(cookiehost);
    இல்லையெனில் {
      /* If the host begins with '[', we start searching for the port after
         the bracket has been closed */
      எனில்(*cookiehost == '[') {
        எழுத்து *closingbracket;
        /* since the 'cookiehost' is an allocated memory area that will be
           freed later we cannot increment the pointer */
        memmove(cookiehost, cookiehost + 1, strlen(cookiehost) - 1);
        closingbracket = strchr(cookiehost, ']');
        எனில்(closingbracket)
          *closingbracket = 0;
      }
      இல்லையெனில் {
        முழு startsearch = 0;
        எழுத்து *colon = strchr(cookiehost + startsearch, ':');
        எனில்(colon)
          *colon = 0; /* The host must not include an embedded port number */
      }
      data->req.cookiehost = cookiehost;
    }
#endif

    எனில்(!curl_strequal("Host:", ptr)) {
      data->state.http_host = curl_maprintf("Host:%s", &ptr[5]);
      எனில்(!data->state.http_host)
        திரும்பு CURLE_OUT_OF_MEMORY;
    }
  }
  இல்லையெனில் {
    /* This is the  HTTP Host: header, so we want
     * - for IPv6 origins: "[ipv6-address]" where the IPv6 address is
     *  found in origin->hostname, stripped of zoneid/scopeid.
     * - the (IDN converted) origin->hostname (DNS name or IPv4) otherwise.
     * Note: zoneid/scopeid  only applies to local routing and has no
     * meaning on the remote HTTP server (eg. would confuse it). */
    bool ipv6 = (bool)data->state.origin->ipv6;
    கட்டமைப்பு dynbuf tmp;
    size_t hlen;
    CURLcode result;

    curlx_dyn_init(&tmp, DYN_HTTP_REQUEST);
    result = curlx_dyn_addn(&tmp, STRCONST("Host: "));
    எனில்(!result && ipv6)
      result = curlx_dyn_addn(&tmp, STRCONST("["));
    எனில்(!result)
      result = curlx_dyn_add(&tmp, data->state.origin->hostname);
    எனில்(!result && ipv6)
      result = curlx_dyn_addn(&tmp, STRCONST("]"));
    எனில்(!result &&
       ((data->state.origin->port != data->state.origin->scheme->defport) ||
       (data->state.origin->scheme->family != conn->scheme->family))) {
      result = curlx_dyn_addf(&tmp, ":%u", data->state.origin->port);
    }

    data->state.http_host = result ? வெற்று : curlx_dyn_take(&tmp, &hlen);
    curlx_dyn_free(&tmp);
    திரும்பு result;
  }
  திரும்பு CURLE_OK;
}

/*
 * Append the request-target to the HTTP request
 */
நிலையான CURLcode http_target(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு dynbuf *r)
{
  CURLcode result = CURLE_OK;
  மாறிலி எழுத்து *path = data->state.up.path;
  மாறிலி எழுத்து *query = data->state.up.query;
#ifndef CURL_DISABLE_PROXY
  கட்டமைப்பு connectdata *conn = data->conn;
#endif

  எனில்(CURL_EASY_STR(data, STRING_TARGET)) {
    path = CURL_EASY_STR(data, STRING_TARGET);
    query = வெற்று;
  }

#ifndef CURL_DISABLE_PROXY
  எனில்(conn->bits.origin_is_proxy) {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL, but if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the decoded hostname! */

    /* and no fragment part */
    CURLUcode uc;
    எழுத்து *url;
    CURLU *h = curl_url_dup(data->state.uh);
    எனில்(!h)
      திரும்பு CURLE_OUT_OF_MEMORY;

    எனில்(!data->state.origin->ipv6 &&
       (data->state.origin->user_hostname != data->state.origin->hostname)) {
      uc = curl_url_set(h, CURLUPART_HOST, data->state.origin->hostname, 0);
      எனில்(uc) {
        curl_url_cleanup(h);
        திரும்பு CURLE_OUT_OF_MEMORY;
      }
    }
    uc = curl_url_set(h, CURLUPART_FRAGMENT, வெற்று, 0);
    எனில்(uc) {
      curl_url_cleanup(h);
      திரும்பு CURLE_OUT_OF_MEMORY;
    }

    எனில்(data->state.origin->scheme == &Curl_scheme_http) {
      /* when getting HTTP, we do not want the userinfo the URL */
      uc = curl_url_set(h, CURLUPART_USER, வெற்று, 0);
      எனில்(uc) {
        curl_url_cleanup(h);
        திரும்பு CURLE_OUT_OF_MEMORY;
      }
      uc = curl_url_set(h, CURLUPART_PASSWORD, வெற்று, 0);
      எனில்(uc) {
        curl_url_cleanup(h);
        திரும்பு CURLE_OUT_OF_MEMORY;
      }
    }
    இல்லையெனில் எனில்(data->state.creds && (data->state.creds->source != CREDS_URL)) {
        /* credentials not from the URL need to be set */
      uc = curl_url_set(h, CURLUPART_USER,
                        data->state.creds->user, CURLU_URLENCODE);
      எனில்(!uc)
        uc = curl_url_set(h, CURLUPART_PASSWORD,
                          data->state.creds->passwd, CURLU_URLENCODE);
      எனில்(uc) {
        curl_url_cleanup(h);
        திரும்பு Curl_uc_to_curlcode(uc);
      }
    }

    /* Extract the URL to use in the request. */
    uc = curl_url_get(h, CURLUPART_URL, &url, CURLU_NO_DEFAULT_PORT);
    எனில்(uc) {
      curl_url_cleanup(h);
      திரும்பு CURLE_OUT_OF_MEMORY;
    }

    curl_url_cleanup(h);

    /* target or URL */
    result = curlx_dyn_add(r, CURL_EASY_STR(data, STRING_TARGET) ?
      CURL_EASY_STR(data, STRING_TARGET) : url);
    curlx_free(url);
    எனில்(result)
      திரும்பு result;

    எனில்((data->state.origin->scheme == &Curl_scheme_ftp) &&
       data->set.proxy_transfer_mode) {
      /* when doing ftp, append ;type=<a|i> if not present */
      size_t len = strlen(path);
      bool type_present = FALSE;
      எனில்((len >= 7) && !memcmp(&path[len - 7], ";type=", 6)) {
        தேர்வு(Curl_raw_toupper(path[len - 1])) {
        நிலை 'A':
        நிலை 'D':
        நிலை 'I':
          type_present = TRUE;
          நிறுத்து;
        }
      }
      எனில்(!type_present) {
        result = curlx_dyn_addf(r, ";type=%c",
                                data->state.prefer_ascii ? 'a' : 'i');
        எனில்(result)
          திரும்பு result;
      }
    }
  }

  இல்லையெனில்
#endif
  {
    result = curlx_dyn_add(r, path);
    எனில்(result)
      திரும்பு result;
    எனில்(query)
      result = curlx_dyn_addf(r, "?%s", query);
  }

  திரும்பு result;
}

#எனில் !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
நிலையான CURLcode set_post_reader(கட்டமைப்பு Curl_easy *data, Curl_HttpReq httpreq)
{
  CURLcode result;

  தேர்வு(httpreq) {
#ifndef CURL_DISABLE_MIME
  நிலை HTTPREQ_POST_MIME:
    data->state.mimepost = data->set.mimepostp;
    நிறுத்து;
#endif
#ifndef CURL_DISABLE_FORM_API
  நிலை HTTPREQ_POST_FORM:
    /* Convert the form structure into a mime structure, then keep
       the conversion */
    எனில்(!data->state.formp) {
      data->state.formp = curlx_calloc(1, அளவு(curl_mimepart));
      எனில்(!data->state.formp)
        திரும்பு CURLE_OUT_OF_MEMORY;
      Curl_mime_cleanpart(data->state.formp);
      result = Curl_getformdata(data, data->state.formp, data->set.httppost,
                                data->state.fread_func);
      எனில்(result) {
        curlx_safefree(data->state.formp);
        திரும்பு result;
      }
      data->state.mimepost = data->state.formp;
    }
    நிறுத்து;
#endif
  இயல்பு:
    data->state.mimepost = வெற்று;
    நிறுத்து;
  }

  தேர்வு(httpreq) {
  நிலை HTTPREQ_POST_FORM:
  நிலை HTTPREQ_POST_MIME:
    /* This is form posting using mime data. */
#ifndef CURL_DISABLE_MIME
    எனில்(data->state.mimepost) {
      மாறிலி எழுத்து *cthdr = Curl_checkheaders(data, STRCONST("Content-Type"));

      /* Read and seek body only. */
      data->state.mimepost->flags |= MIME_BODY_ONLY;

      /* Prepare the mime structure headers & set content type. */

      எனில்(cthdr)
        க்கு(cthdr += 13; *cthdr == ' '; cthdr++)
          ;
      இல்லையெனில் எனில்(data->state.mimepost->kind == MIMEKIND_MULTIPART)
        cthdr = "multipart/form-data";

      curl_mime_headers(data->state.mimepost, data->set.headers, 0);
      result = Curl_mime_prepare_headers(data, data->state.mimepost, cthdr,
                                         வெற்று, MIMESTRATEGY_FORM);
      எனில்(result)
        திரும்பு result;
      curl_mime_headers(data->state.mimepost, வெற்று, 0);
      result = Curl_creader_set_mime(data, data->state.mimepost);
      எனில்(result)
        திரும்பு result;
    }
    இல்லையெனில்
#endif
    {
      result = Curl_creader_set_null(data);
    }
    data->state.infilesize = Curl_creader_total_length(data);
    திரும்பு result;

  இயல்பு:
    திரும்பு Curl_creader_set_null(data);
  }
  /* never reached */
}
#endif

நிலையான CURLcode set_reader(கட்டமைப்பு Curl_easy *data, Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
  curl_off_t postsize = data->state.infilesize;

  DEBUGASSERT(data->conn);

  எனில்(data->req.authneg) {
    திரும்பு Curl_creader_set_null(data);
  }

  தேர்வு(httpreq) {
  நிலை HTTPREQ_PUT: /* Let's PUT the data to the server! */
    திரும்பு postsize ? Curl_creader_set_fread(data, postsize) :
      Curl_creader_set_null(data);

#எனில் !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  நிலை HTTPREQ_POST_FORM:
  நிலை HTTPREQ_POST_MIME:
    திரும்பு set_post_reader(data, httpreq);
#endif

  நிலை HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */
    /* the size of the post body */
    எனில்(!postsize) {
      result = Curl_creader_set_null(data);
    }
    இல்லையெனில் எனில்(data->set.postfields) {
      size_t plen = curlx_sotouz_range(postsize, 0, SIZE_MAX);
      எனில்(plen == SIZE_MAX)
        திரும்பு CURLE_OUT_OF_MEMORY;
      இல்லையெனில் எனில்(plen)
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      இல்லையெனில்
        result = Curl_creader_set_null(data);
    }
    இல்லையெனில் {
      /* we read the bytes from the callback. In case "chunked" encoding
       * is forced by the application, we disregard `postsize`. This is
       * a backward compatibility decision to earlier versions where
       * chunking disregarded this. See issue #13229. */
      bool chunked = FALSE;
      எழுத்து *ptr = Curl_checkheaders(data, STRCONST("Transfer-Encoding"));
      எனில்(ptr) {
        /* Some kind of TE is requested, check if 'chunked' is chosen */
        chunked = Curl_compareheader(ptr, STRCONST("Transfer-Encoding:"),
                                     STRCONST("chunked"));
      }
      result = Curl_creader_set_fread(data, chunked ? -1 : postsize);
    }
    திரும்பு result;

  இயல்பு:
    /* HTTP GET/HEAD download, has no body, needs no Content-Length */
    data->state.infilesize = 0;
    திரும்பு Curl_creader_set_null(data);
  }
  /* not reached */
}

நிலையான CURLcode http_resume(கட்டமைப்பு Curl_easy *data, Curl_HttpReq httpreq)
{
  எனில்((HTTPREQ_POST == httpreq || HTTPREQ_POST_FORM == httpreq ||
      HTTPREQ_POST_MIME == httpreq || HTTPREQ_PUT == httpreq) &&
     data->state.resume_from) {
    failf(data, "HTTP upload cannot be resumed");
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode http_req_set_TE(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு dynbuf *req,
                                முழு httpversion)
{
  CURLcode result = CURLE_OK;
  மாறிலி எழுத்து *ptr;

  ptr = Curl_checkheaders(data, STRCONST("Transfer-Encoding"));
  எனில்(ptr) {
    /* Some kind of TE is requested, check if 'chunked' is chosen */
    data->req.upload_chunky =
      Curl_compareheader(ptr,
                         STRCONST("Transfer-Encoding:"), STRCONST("chunked"));
    எனில்(data->req.upload_chunky && (httpversion >= 20)) {
      infof(data, "suppressing chunked transfer encoding on connection "
            "using HTTP version 2 or higher");
      data->req.upload_chunky = FALSE;
    }
  }
  இல்லையெனில் {
    curl_off_t req_clen = Curl_creader_total_length(data);

    எனில்(req_clen < 0) {
      /* indeterminate request content length */
      எனில்(httpversion > 10) {
        /* On HTTP/1.1, enable chunked, on HTTP/2 and later we do not
         * need it */
        data->req.upload_chunky = (httpversion < 20);
      }
      இல்லையெனில் {
        failf(data, "Chunky upload is not supported by HTTP 1.0");
        திரும்பு CURLE_UPLOAD_FAILED;
      }
    }
    இல்லையெனில் {
      /* else, no chunky upload */
      data->req.upload_chunky = FALSE;
    }

    எனில்(data->req.upload_chunky)
      result = curlx_dyn_add(req, "Transfer-Encoding: chunked\r\n");
  }
  திரும்பு result;
}

நிலையான CURLcode addexpect(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு dynbuf *r,
                          முழு httpversion, bool *announced_exp100)
{
  CURLcode result;
  எழுத்து *ptr;

  *announced_exp100 = FALSE;
  /* Avoid Expect: 100-continue if Upgrade: is used */
  எனில்(data->req.upgr101 != UPGR101_NONE)
    திரும்பு CURLE_OK;

  /* For really small puts we do not use Expect: headers at all, and for
     the somewhat bigger ones we allow the app to disable it. Make
     sure that the expect100header is always set to the preferred value
     here. */
  ptr = Curl_checkheaders(data, STRCONST("Expect"));
  எனில்(ptr) {
    *announced_exp100 =
      Curl_compareheader(ptr, STRCONST("Expect:"), STRCONST("100-continue"));
  }
  இல்லையெனில் எனில்(!data->state.disableexpect && (httpversion == 11)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    curl_off_t client_len = Curl_creader_client_length(data);
    எனில்(client_len > EXPECT_100_THRESHOLD || client_len < 0) {
      result = curlx_dyn_addn(r, STRCONST("Expect: 100-continue\r\n"));
      எனில்(result)
        திரும்பு result;
      *announced_exp100 = TRUE;
    }
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode http_add_content_hds(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு dynbuf *r,
                                     முழு httpversion,
                                     Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
  curl_off_t req_clen;
  bool announced_exp100 = FALSE;

  DEBUGASSERT(data->conn);
  எனில்(data->req.upload_chunky) {
    result = Curl_httpchunk_add_reader(data);
    எனில்(result)
      திரும்பு result;
  }

  /* Get the request body length that has been set up */
  req_clen = Curl_creader_total_length(data);
  தேர்வு(httpreq) {
  நிலை HTTPREQ_PUT:
  நிலை HTTPREQ_POST:
#எனில் !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  நிலை HTTPREQ_POST_FORM:
  நிலை HTTPREQ_POST_MIME:
#endif
    /* We only set Content-Length and allow a custom Content-Length if
       we do not upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length).
       We do not override a custom "Content-Length" header, but during
       authentication negotiation that header is suppressed.
     */
    எனில்(req_clen >= 0 && !data->req.upload_chunky &&
       (data->req.authneg ||
        !Curl_checkheaders(data, STRCONST("Content-Length")))) {
      /* we allow replacing this header if not during auth negotiation,
         although it is not wise to actually set your own */
      result = curlx_dyn_addf(r, "Content-Length: %" FMT_OFF_T "\r\n",
                              req_clen);
    }
    எனில்(result)
      செல் out;

#ifndef CURL_DISABLE_MIME
    /* Output mime-generated headers. */
    எனில்(data->state.mimepost &&
       ((httpreq == HTTPREQ_POST_FORM) || (httpreq == HTTPREQ_POST_MIME))) {
      கட்டமைப்பு curl_slist *hdr;

      க்கு(hdr = data->state.mimepost->curlheaders; hdr; hdr = hdr->next) {
        result = curlx_dyn_addf(r, "%s\r\n", hdr->data);
        எனில்(result)
          செல் out;
      }
    }
#endif
    எனில்(httpreq == HTTPREQ_POST &&
       !Curl_checkheaders(data, STRCONST("Content-Type"))) {
      result = curlx_dyn_addn(r, STRCONST("Content-Type: application/"
                                          "x-www-form-urlencoded\r\n"));
      எனில்(result)
        செல் out;
    }
    result = addexpect(data, r, httpversion, &announced_exp100);
    எனில்(result)
      செல் out;
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }

  Curl_pgrsSetUploadSize(data, req_clen);
  எனில்(announced_exp100)
    result = http_exp100_add_reader(data);

out:
  திரும்பு result;
}

#ifndef CURL_DISABLE_COOKIES

நிலையான CURLcode http_cookies(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு dynbuf *r)
{
  CURLcode result = CURLE_OK;
  மாறிலி எழுத்து *addcookies = வெற்று;
  bool linecap = FALSE;
  எனில்(CURL_EASY_STR(data, STRING_COOKIE) &&
     !Curl_checkheaders(data, STRCONST("Cookie")) &&
     Curl_auth_allowed_to_host(data))
    addcookies = CURL_EASY_STR(data, STRING_COOKIE);

  எனில்(data->cookies || addcookies) {
    கட்டமைப்பு Curl_llist list;
    முழு மொத்தம் = 0;

    எனில்(data->cookies && data->state.cookie_engine) {
      bool okay;
      மாறிலி எழுத்து *host = data->req.cookiehost ?
        data->req.cookiehost : data->state.origin->hostname;
      Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
      result = Curl_cookie_getlist(data, &okay, host, &list);
      எனில்(!result && okay) {
        கட்டமைப்பு Curl_llist_முனை *n;
        size_t clen = 8; /* hold the size of the generated Cookie: header */

        /* loop through all cookies that matched */
        க்கு(n = Curl_llist_head(&list); n; n = Curl_முனை_next(n)) {
          கட்டமைப்பு Cookie *co = Curl_முனை_elem(n);
          எனில்(co->மதிப்பு) {
            size_t add;
            எனில்(!மொத்தம்) {
              result = curlx_dyn_addn(r, STRCONST("Cookie: "));
              எனில்(result)
                நிறுத்து;
            }
            add = strlen(co->name) + strlen(co->மதிப்பு) + 1;
            எனில்(clen + add >= MAX_COOKIE_HEADER_LEN) {
              infof(data, "Restricted outgoing cookies due to header size, "
                    "'%s' not sent", co->name);
              linecap = TRUE;
              நிறுத்து;
            }
            result = curlx_dyn_addf(r, "%s%s=%s", மொத்தம் ? "; " : "",
                                    co->name, co->மதிப்பு);
            எனில்(result)
              நிறுத்து;
            clen += add + (மொத்தம் ? 2 : 0);
            மொத்தம்++;
          }
        }
        Curl_llist_destroy(&list, வெற்று);
      }
      Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    }
    எனில்(addcookies && !result && !linecap) {
      எனில்(!மொத்தம்)
        result = curlx_dyn_addn(r, STRCONST("Cookie: "));
      எனில்(!result) {
        result = curlx_dyn_addf(r, "%s%s", மொத்தம் ? "; " : "", addcookies);
        மொத்தம்++;
      }
    }
    எனில்(மொத்தம் && !result)
      result = curlx_dyn_addn(r, STRCONST("\r\n"));

    எனில்(result)
      திரும்பு result;
  }
  திரும்பு result;
}
#இல்லையெனில்
#define http_cookies(a, b) CURLE_OK
#endif

நிலையான CURLcode http_range(கட்டமைப்பு Curl_easy *data,
                           Curl_HttpReq httpreq)
{
  எனில்(data->state.use_range) {
    /*
     * A range is selected. We use different headers whether we are downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    எனில்(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_HEAD)) &&
       !Curl_checkheaders(data, STRCONST("Range"))) {
      /* if a line like this was already allocated, free the previous one */
      curlx_free(data->state.rangeline);
      data->state.rangeline = curl_maprintf("Range: bytes=%s\r\n",
                                                 data->state.range);
      எனில்(!data->state.rangeline)
        திரும்பு CURLE_OUT_OF_MEMORY;
    }
    இல்லையெனில் எனில்((httpreq == HTTPREQ_POST || httpreq == HTTPREQ_PUT) &&
            !Curl_checkheaders(data, STRCONST("Content-Range"))) {
      curl_off_t req_clen = Curl_creader_total_length(data);
      /* if a line like this was already allocated, free the previous one */
      curlx_free(data->state.rangeline);

      எனில்(data->set.set_resume_from < 0) {
        /* Upload resume was asked for, but we do not know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes 0-%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n", req_clen - 1, req_clen);
      }
      இல்லையெனில் எனில்(data->state.resume_from) {
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
      இல்லையெனில் {
        /* Range was selected and then we pass the incoming range and append
           total size */
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes %s/%" FMT_OFF_T "\r\n",
                        data->state.range, req_clen);
      }
      எனில்(!data->state.rangeline)
        திரும்பு CURLE_OUT_OF_MEMORY;
    }
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode http_firstwrite(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு SingleRequest *k = &data->req;

  எனில்(data->req.newurl) {
    எனில்(conn->bits.close) {
      /* Abort after the headers if "follow Location" is set
         and we are set to close anyway. */
      CURL_REQ_CLEAR_RECV(data);
      k->done = TRUE;
      திரும்பு CURLE_OK;
    }
    /* We have a new URL to load, but since we want to be able to reuse this
       connection properly, we read the full response in "ignore more" */
    k->ignorebody = TRUE;
    infof(data, "Ignoring the response-body");
  }
  எனில்(data->state.resume_from && !k->content_range &&
     (data->state.httpreq == HTTPREQ_GET) &&
     !k->ignorebody) {

    எனில்(k->size == data->state.resume_from) {
      /* The resume point is at the end of file, consider this fine even if it
         does not allow resume from here. */
      infof(data, "The entire document is already downloaded");
      streamclose(conn);
      /* Abort download */
      CURL_REQ_CLEAR_RECV(data);
      k->done = TRUE;
      திரும்பு CURLE_OK;
    }

    /* we wanted to resume a download, although the server does not seem to
     * support this and we did this with a GET (if it was not a GET we did a
     * POST or PUT resume) */
    failf(data, "HTTP server does not seem to support "
          "byte ranges. Cannot resume.");
    திரும்பு CURLE_RANGE_ERROR;
  }

  எனில்(data->set.timecondition && !data->state.range &&
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
    திரும்பு CURLE_OK;
  } /* we have a time condition */

  திரும்பு CURLE_OK;
}

நிலையான CURLcode http_check_new_conn(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  மாறிலி எழுத்து *info_version = வெற்று;
  மாறிலி எழுத்து *alpn;
  CURLcode result;

  alpn = Curl_conn_get_alpn_negotiated(data, conn);
  எனில்(alpn && !strcmp("h3", alpn)) {
#ifndef CURL_DISABLE_PROXY
    எனில்(!conn->bits.origin_is_proxy)
#endif
      DEBUGASSERT(Curl_conn_http_version(data, conn) == 30);
    info_version = "HTTP/3";
  }
  இல்லையெனில் எனில்(alpn && !strcmp("h2", alpn)) {
#ifndef CURL_DISABLE_PROXY
    எனில்((Curl_conn_http_version(data, conn) != 20) &&
       conn->bits.origin_is_proxy) {
      result = Curl_http2_switch(data);
      எனில்(result)
        திரும்பு result;
    }
    இல்லையெனில்
#endif
    DEBUGASSERT(Curl_conn_http_version(data, conn) == 20);
    info_version = "HTTP/2";
  }
  இல்லையெனில் {
    /* Check if user wants to use HTTP/2 with clear TCP */
    எனில்(Curl_http2_may_switch(data)) {
      DEBUGF(infof(data, "HTTP/2 over clean TCP"));
      result = Curl_http2_switch(data);
      எனில்(result)
        திரும்பு result;
      info_version = "HTTP/2";
      /* There is no ALPN here, but the connection is now definitely h2 */
      conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(conn);
    }
    இல்லையெனில்
      info_version = "HTTP/1.x";
  }

  எனில்(info_version)
    infof(data, "using %s", info_version);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode http_add_connection_hd(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு dynbuf *req)
{
  கட்டமைப்பு curl_slist *head;
  மாறிலி எழுத்து *sep = "Connection: ";
  CURLcode result = CURLE_OK;
  size_t rlen = curlx_dyn_len(req);
  bool skip;

  /* Add the 1st custom "Connection: " header, if there is one */
  க்கு(head = data->set.headers; head; head = head->next) {
    எனில்(curl_strnequal(head->data, "Connection", 10) &&
       Curl_headersep(head->data[10]) &&
       !http_header_is_empty(head->data)) {
      எழுத்து *மதிப்பு;
      result = copy_custom_மதிப்பு(head->data, &மதிப்பு);
      எனில்(result)
        திரும்பு result;
      result = curlx_dyn_addf(req, "%s%s", sep, மதிப்பு);
      sep = ", ";
      curlx_free(மதிப்பு);
      நிறுத்து; /* leave, having added 1st one */
    }
  }

  /* add our internal Connection: header values, if we have any */
  எனில்(!result && data->state.http_hd_te) {
    result = curlx_dyn_addf(req, "%s%s", sep, "TE");
    sep = ", ";
  }
  எனில்(!result && data->state.http_hd_upgrade) {
    result = curlx_dyn_addf(req, "%s%s", sep, "Upgrade");
    sep = ", ";
  }
  எனில்(!result && data->state.http_hd_h2_settings) {
    result = curlx_dyn_addf(req, "%s%s", sep, "HTTP2-Settings");
  }
  எனில்(!result && (rlen < curlx_dyn_len(req)))
    result = curlx_dyn_addn(req, STRCONST("\r\n"));
  எனில்(result)
    திரும்பு result;

  /* Add all user-defined Connection: headers after the first */
  skip = TRUE;
  க்கு(head = data->set.headers; head; head = head->next) {
    எனில்(curl_strnequal(head->data, "Connection", 10) &&
       Curl_headersep(head->data[10]) &&
       !http_header_is_empty(head->data)) {
      எனில்(skip) {
        skip = FALSE;
        தொடர்;
      }
      result = curlx_dyn_addf(req, "%s\r\n", head->data);
      எனில்(result)
        திரும்பு result;
    }
  }

  திரும்பு CURLE_OK;
}

/* Header identifier in order we send them by default */
வகைவரை எண்ணிக்கை {
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

நிலையான CURLcode http_add_hd(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு dynbuf *req,
                            http_hd_t id,
                            unsigned எழுத்து httpversion,
                            மாறிலி எழுத்து *method,
                            Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
#எனில் !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_PROXY) || \
  !defined(CURL_DISABLE_WEBSOCKETS)
  கட்டமைப்பு connectdata *conn = data->conn;
#endif
  தேர்வு(id) {
  நிலை H1_HD_REQUEST:
    /* add the main request stuff */
    /* GET/HEAD/POST/PUT */
    result = curlx_dyn_addf(req, "%s ", method);
    எனில்(!result)
      result = http_target(data, req);
    எனில்(!result)
      result = curlx_dyn_addf(req, " HTTP/%s\r\n",
                              get_http_string(httpversion));
    நிறுத்து;

  நிலை H1_HD_HOST:
    எனில்(data->state.http_host) {
      result = curlx_dyn_add(req, data->state.http_host);
      எனில்(!result)
        result = curlx_dyn_addn(req, STRCONST("\r\n"));
    }
    நிறுத்து;

#ifndef CURL_DISABLE_PROXY
  நிலை H1_HD_PROXY_AUTH:
    எனில்(data->req.hd_proxy_auth)
      result = curlx_dyn_add(req, data->req.hd_proxy_auth);
    நிறுத்து;
#endif

  நிலை H1_HD_AUTH:
    எனில்(data->req.hd_auth)
      result = curlx_dyn_add(req, data->req.hd_auth);
    நிறுத்து;

  நிலை H1_HD_RANGE:
    எனில்(data->state.use_range && data->state.rangeline)
      result = curlx_dyn_add(req, data->state.rangeline);
    நிறுத்து;

  நிலை H1_HD_USER_AGENT: {
    மாறிலி எழுத்து *ua = CURL_EASY_STR(data, STRING_USERAGENT);
    எனில்(ua && *ua && !Curl_checkheaders(data, STRCONST("User-Agent")))
      result = curlx_dyn_addf(req, "User-Agent: %s\r\n", ua);
    நிறுத்து;
  }

  நிலை H1_HD_ACCEPT:
    எனில்(!Curl_checkheaders(data, STRCONST("Accept")))
      result = curlx_dyn_add(req, "Accept: */*\r\n");
    நிறுத்து;

  நிலை H1_HD_TE:
#ifdef HAVE_LIBZ
    எனில்(!Curl_checkheaders(data, STRCONST("TE")) &&
       data->set.http_transfer_encoding) {
      data->state.http_hd_te = TRUE;
      result = curlx_dyn_add(req, "TE: gzip\r\n");
    }
#endif
    நிறுத்து;

  நிலை H1_HD_ACCEPT_ENCODING: {
    மாறிலி எழுத்து *enc = CURL_EASY_STR(data, STRING_ENCODING);
    எனில்(enc && !Curl_checkheaders(data, STRCONST("Accept-Encoding")))
      result = curlx_dyn_addf(req, "Accept-Encoding: %s\r\n", enc);
    நிறுத்து;
  }

  நிலை H1_HD_REFERER:
    எனில்(Curl_bufref_ptr(&data->state.referer) &&
       !Curl_checkheaders(data, STRCONST("Referer")))
      result = curlx_dyn_addf(req, "Referer: %s\r\n",
                              Curl_bufref_ptr(&data->state.referer));
    நிறுத்து;

#ifndef CURL_DISABLE_PROXY
  நிலை H1_HD_PROXY_CONNECTION:
    எனில்(conn->bits.origin_is_proxy &&
       !Curl_checkheaders(data, STRCONST("Proxy-Connection")) &&
       !Curl_checkProxyheaders(data, data->conn, STRCONST("Proxy-Connection")))
      result = curlx_dyn_add(req, "Proxy-Connection: Keep-Alive\r\n");
    நிறுத்து;
#endif

  நிலை H1_HD_TRANSFER_ENCODING:
    result = http_req_set_TE(data, req, httpversion);
    நிறுத்து;

#ifndef CURL_DISABLE_ALTSVC
  நிலை H1_HD_ALT_USED:
    எனில்(conn->bits.altused && conn->via_peer &&
       !Curl_checkheaders(data, STRCONST("Alt-Used")))
      result = curlx_dyn_addf(req, "Alt-Used: %s:%u\r\n",
                              conn->via_peer->hostname, conn->via_peer->port);
    நிறுத்து;
#endif

  நிலை H1_HD_UPGRADE:
    எனில்(!Curl_conn_is_ssl(data->conn, FIRSTSOCKET) && (httpversion < 20) &&
       (data->state.http_neg.wanted & CURL_HTTP_V2x) &&
       data->state.http_neg.h2_upgrade) {
      /* append HTTP2 upgrade magic stuff to the HTTP request if it is not done
         over SSL */
      result = Curl_http2_request_upgrade(req, data);
    }
#ifndef CURL_DISABLE_WEBSOCKETS
    எனில்(!result && conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
      result = Curl_ws_request(data, req);
#endif
    நிறுத்து;

  நிலை H1_HD_COOKIES:
    result = http_cookies(data, req);
    நிறுத்து;

  நிலை H1_HD_CONDITIONALS:
    result = Curl_add_timecondition(data, req);
    நிறுத்து;

  நிலை H1_HD_CUSTOM:
    result = Curl_add_custom_headers(data, FALSE, httpversion, req);
    நிறுத்து;

  நிலை H1_HD_CONTENT:
    result = http_add_content_hds(data, req, httpversion, httpreq);
    நிறுத்து;

  நிலை H1_HD_CONNECTION: {
    result = http_add_connection_hd(data, req);
    நிறுத்து;
  }

  நிலை H1_HD_LAST:
    result = curlx_dyn_addn(req, STRCONST("\r\n"));
    நிறுத்து;
  }
  திரும்பு result;
}

/*
 * Curl_http() gets called from the generic multi_do() function when an HTTP
 * request is to be performed. This creates and sends a properly constructed
 * HTTP request.
 */
CURLcode Curl_http(கட்டமைப்பு Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  Curl_HttpReq httpreq;
  மாறிலி எழுத்து *method;
  கட்டமைப்பு dynbuf req;
  unsigned எழுத்து httpversion;
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

  எனில்(!data->conn->bits.reuse) {
    result = http_check_new_conn(data);
    எனில்(result)
      செல் out;
  }

  /* Add collecting of headers written to client. For a new connection,
   * we might have done that already, but reuse
   * or multiplex needs it here as well. */
  result = Curl_headers_init(data);
  எனில்(result)
    செல் out;

  data->state.http_hd_te = FALSE;
  data->state.http_hd_upgrade = FALSE;
  data->state.http_hd_h2_settings = FALSE;

  /* what kind of request do we need to send? */
  Curl_http_method(data, &method, &httpreq);

  /* select host to send */
  result = http_set_aptr_host(data);
  /* setup the authentication headers, how that method and host are known */
  எனில்(!result)
    result = Curl_http_output_auth(data, data->conn, method, httpreq,
                                   data->state.up.path,
                                   data->state.up.query, FALSE);
  /* Setup input reader, resume information and ranges */
  எனில்(!result)
    result = set_reader(data, httpreq);
  எனில்(!result)
    result = http_resume(data, httpreq);
  எனில்(!result)
    result = http_range(data, httpreq);
  எனில்(result)
    செல் out;

  httpversion = http_request_version(data);
  /* Add request line and all headers to `req` */
  க்கு(hd_id = 0; hd_id <= H1_HD_LAST; ++hd_id) {
    result = http_add_hd(data, &req, (http_hd_t)hd_id,
                         httpversion, method, httpreq);
    எனில்(result)
      செல் out;
  }

  /* setup variables for the upcoming transfer and send */
  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);
  result = Curl_req_send(data, &req, httpversion);

  எனில்((httpversion >= 20) && data->req.upload_chunky)
    /* upload_chunky was set above to set up the request in a chunky fashion,
       but is disabled here again to avoid that the chunked encoded version is
       actually used when sending the request body over h2 */
    data->req.upload_chunky = FALSE;

out:
  எனில்(result == CURLE_TOO_LARGE)
    failf(data, "HTTP request too large");

  curlx_dyn_free(&req);
  திரும்பு result;
}

வகைவரை எண்ணிக்கை {
  STATUS_UNKNOWN, /* not enough data to tell yet */
  STATUS_DONE, /* a status line was read */
  STATUS_BAD /* not a status line */
} statusline;

/* Check a string for a prefix. Check no more than 'len' bytes */
நிலையான bool checkprefixmax(மாறிலி எழுத்து *prefix, மாறிலி எழுத்து *buffer, size_t len)
{
  size_t ch = CURLMIN(strlen(prefix), len);
  திரும்பு curl_strnequal(prefix, buffer, ch);
}

/*
 * checkhttpprefix()
 *
 * Returns TRUE if member of the list matches prefix of string
 */
நிலையான statusline checkhttpprefix(கட்டமைப்பு Curl_easy *data,
                                  மாறிலி எழுத்து *s, size_t len)
{
  கட்டமைப்பு curl_slist *head = data->set.http200aliases;
  statusline rc = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;

  போது(head) {
    எனில்(checkprefixmax(head->data, s, len)) {
      rc = onmatch;
      நிறுத்து;
    }
    head = head->next;
  }

  எனில்((rc != STATUS_DONE) && checkprefixmax("HTTP/", s, len))
    rc = onmatch;

  திரும்பு rc;
}

#ifndef CURL_DISABLE_RTSP
நிலையான statusline checkrtspprefix(கட்டமைப்பு Curl_easy *data,
                                  மாறிலி எழுத்து *s, size_t len)
{
  statusline status = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;
  (வெறுமை)data;
  எனில்(checkprefixmax("RTSP/", s, len))
    status = onmatch;

  திரும்பு status;
}
#endif /* CURL_DISABLE_RTSP */

நிலையான statusline checkprotoprefix(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு connectdata *conn,
                                   மாறிலி எழுத்து *s, size_t len)
{
#ifndef CURL_DISABLE_RTSP
  எனில்(conn->scheme->protocol & CURLPROTO_RTSP)
    திரும்பு checkrtspprefix(data, s, len);
#இல்லையெனில்
  (வெறுமை)conn;
#endif /* CURL_DISABLE_RTSP */

  திரும்பு checkhttpprefix(data, s, len);
}

/* HTTP header has field name `n` (a string constant) */
#define HD_IS(hd, hdlen, n) \
  (((hdlen) >= (அளவு(n) - 1)) && curl_strnequal(n, hd, அளவு(n) - 1))

#define HD_VAL(hd, hdlen, n) \
  ((((hdlen) >= (அளவு(n) - 1)) && (hd) && \
    curl_strnequal(n, hd, அளவு(n) - 1)) ? ((hd) + (அளவு(n) - 1)) : வெற்று)

/* HTTP header has field name `n` (a string constant) and contains `v`
 * (a string constant) in its value(s) */
#define HD_IS_AND_SAYS(hd, hdlen, n, v) \
  (HD_IS(hd, hdlen, n) && \
   ((hdlen) > ((அளவு(n) - 1) + (அளவு(v) - 1))) && \
   Curl_compareheader(hd, STRCONST(n), STRCONST(v)))

/*
 * http_header_a() parses a single response header starting with A.
 */
நிலையான CURLcode http_header_a(கட்டமைப்பு Curl_easy *data,
                              மாறிலி எழுத்து *hd, size_t hdlen)
{
#ifndef CURL_DISABLE_ALTSVC
  மாறிலி எழுத்து *v;
  v = (data->asi &&
       (Curl_xfer_is_secure(data) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_ALTSVC_HTTP")
#இல்லையெனில்
        0
#endif
         )) ? HD_VAL(hd, hdlen, "Alt-Svc:") : வெற்று;
  எனில்(v) {
    /* the ALPN of the current request */
    கட்டமைப்பு SingleRequest *k = &data->req;
    எண்ணிக்கை alpnid id = (k->httpversion == 30) ? ALPN_h3 :
      (k->httpversion == 20) ? ALPN_h2 : ALPN_h1;
    திரும்பு Curl_altsvc_parse(data, data->asi, v, data->state.origin, id);
  }
#இல்லையெனில்
  (வெறுமை)data;
  (வெறுமை)hd;
  (வெறுமை)hdlen;
#endif
  திரும்பு CURLE_OK;
}

/*
 * http_header_c() parses a single response header starting with C.
 */
நிலையான CURLcode http_header_c(கட்டமைப்பு Curl_easy *data,
                              மாறிலி எழுத்து *hd, size_t hdlen)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு SingleRequest *k = &data->req;
  மாறிலி எழுத்து *v;

  /* Check for Content-Length: header lines to get size. Browsers insist we
     should accept multiple Content-Length headers and that a comma separated
     list also is fine and then we should accept them all as long as they are
     the same value. Different values trigger error.
   */
  v = (!k->http_bodyless && !data->set.ignorecl && !k->ignore_cl) ?
    HD_VAL(hd, hdlen, "Content-Length:") : வெற்று;
  எனில்(v) {
    செய் {
      curl_off_t contentlength;
      முழு offt = curlx_str_numblanks(&v, &contentlength);

      எனில்(offt == STRE_OVERFLOW) {
        /* out of range */
        எனில்(data->set.max_filesize) {
          failf(data, "Maximum file size exceeded");
          திரும்பு CURLE_FILESIZE_EXCEEDED;
        }
        streamclose(conn);
        infof(data, "Overflow Content-Length: value");
        திரும்பு CURLE_OK;
      }
      இல்லையெனில் {
        எனில்((offt == STRE_OK) &&
           ((k->size == -1) || /* not set to something before */
            (k->size == contentlength))) { /* or the same value */

          k->size = contentlength;
          curlx_str_passblanks(&v);

          /* on a comma, loop and get the next instead */
          எனில்(!curlx_str_single(&v, ','))
            தொடர்;

          எனில்(!curlx_str_newline(&v)) {
            k->maxdownload = k->size;
            திரும்பு CURLE_OK;
          }
        }
        /* negative, different value or rubbish - bad HTTP */
        failf(data, "Invalid Content-Length: value");
        திரும்பு CURLE_WEIRD_SERVER_REPLY;
      }
    } போது(1);
  }
  v = (!k->http_bodyless && CURL_EASY_STR(data, STRING_ENCODING)) ?
    HD_VAL(hd, hdlen, "Content-Encoding:") : வெற்று;
  எனில்(v) {
    /*
     * Process Content-Encoding. Look for the values: identity, gzip, deflate,
     * compress, x-gzip and x-compress. x-gzip and x-compress are the same as
     * gzip and compress. (Sec 3.5 RFC 2616). zlib cannot handle compress.
     * Errors are handled further down when the response body is processed
     */
    திரும்பு Curl_build_unencoding_அடுக்கு(data, v, FALSE);
  }
  /* check for Content-Type: header lines to get the MIME-type */
  v = HD_VAL(hd, hdlen, "Content-Type:");
  எனில்(v) {
    எழுத்து *contenttype = Curl_copy_header_மதிப்பு(hd);
    எனில்(!contenttype)
      திரும்பு CURLE_OUT_OF_MEMORY;
    எனில்(!*contenttype)
      /* ignore empty data */
      curlx_free(contenttype);
    இல்லையெனில் {
      curlx_free(data->info.contenttype);
      data->info.contenttype = contenttype;
    }
    திரும்பு CURLE_OK;
  }
  எனில்((k->httpversion < 20) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "close")) {
    /*
     * [RFC 2616, section 8.1.2.1]
     * "Connection: close" is HTTP/1.1 language and means that
     * the connection will close when this request has been
     * served.
     */
    connclose(conn);
    திரும்பு CURLE_OK;
  }
  எனில்((k->httpversion == 10) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "keep-alive")) {
    /*
     * An HTTP/1.0 reply with the 'Connection: keep-alive' line
     * tells us the connection will be kept alive for our
     * pleasure. Default action for 1.0 is to close.
     *
     * [RFC2068, section 19.7.1] */
    connkeep(conn);
    infof(data, "HTTP/1.0 connection set to keep alive");
    திரும்பு CURLE_OK;
  }
  v = !k->http_bodyless ? HD_VAL(hd, hdlen, "Content-Range:") : வெற்று;
  எனில்(v) {
    /* Content-Range: bytes [num]-
       Content-Range: bytes: [num]-
       Content-Range: [num]-
       Content-Range: [asterisk]/[total]

       The second format was added since Sun's webserver
       JavaWebServer/1.1.1 obviously sends the header this way!
       The third added since some servers use that!
       The fourth means the requested range was unsatisfied.
     */

    மாறிலி எழுத்து *ptr = v;

    /* Move forward until first digit or asterisk */
    போது(*ptr && !ISDIGIT(*ptr) && *ptr != '*')
      ptr++;

    /* if it truly stopped on a digit */
    எனில்(ISDIGIT(*ptr)) {
      எனில்(!curlx_str_எண்(&ptr, &k->offset, CURL_OFF_T_MAX) &&
         (data->state.resume_from == k->offset))
        /* we asked for a resume and we got it */
        k->content_range = TRUE;
    }
    இல்லையெனில் எனில்(k->httpcode < 300)
      data->state.resume_from = 0; /* get everything */
  }
  திரும்பு CURLE_OK;
}

/*
 * http_header_l() parses a single response header starting with L.
 */
நிலையான CURLcode http_header_l(கட்டமைப்பு Curl_easy *data,
                              மாறிலி எழுத்து *hd, size_t hdlen)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு SingleRequest *k = &data->req;
  மாறிலி எழுத்து *v = (!k->http_bodyless &&
                   (data->set.timecondition || data->set.get_filetime)) ?
    HD_VAL(hd, hdlen, "Last-Modified:") : வெற்று;
  எனில்(v) {
    எனில்(Curl_getdate_capped(v, &k->timeofdoc))
      k->timeofdoc = 0;
    எனில்(data->set.get_filetime)
      data->info.filetime = k->timeofdoc;
    திரும்பு CURLE_OK;
  }
  எனில்(HD_IS(hd, hdlen, "Location:")) {
    /* this is the URL that the server advises us to use instead */
    எழுத்து *location = Curl_copy_header_மதிப்பு(hd);
    எனில்(!location)
      திரும்பு CURLE_OUT_OF_MEMORY;
    எனில்(!*location ||
       (data->req.location && !strcmp(data->req.location, location))) {
      /* ignore empty header, or exact repeat of a previous one */
      curlx_free(location);
      திரும்பு CURLE_OK;
    }
    இல்லையெனில் {
      /* has value and is not an exact repeat */
      எனில்(data->req.location) {
        failf(data, "Multiple Location headers");
        curlx_free(location);
        திரும்பு CURLE_WEIRD_SERVER_REPLY;
      }
      data->req.location = location;

      எனில்((k->httpcode >= 300 && k->httpcode < 400) &&
         data->set.http_follow_mode) {
        CURLcode result;
        DEBUGASSERT(!data->req.newurl);
        data->req.newurl = curlx_strdup(data->req.location); /* clone */
        எனில்(!data->req.newurl)
          திரும்பு CURLE_OUT_OF_MEMORY;

        /* some cases of POST and PUT etc needs to rewind the data
           stream at this point */
        result = http_perhapsrewind(data, conn);
        எனில்(result)
          திரும்பு result;

        /* mark the next request as a followed location: */
        data->state.this_is_a_follow = TRUE;
      }
    }
  }
  திரும்பு CURLE_OK;
}

/*
 * http_header_p() parses a single response header starting with P.
 */
நிலையான CURLcode http_header_p(கட்டமைப்பு Curl_easy *data,
                              மாறிலி எழுத்து *hd, size_t hdlen)
{
  கட்டமைப்பு SingleRequest *k = &data->req;

#ifndef CURL_DISABLE_PROXY
  மாறிலி எழுத்து *v = HD_VAL(hd, hdlen, "Proxy-Connection:");
  எனில்(v) {
    கட்டமைப்பு connectdata *conn = data->conn;
    எனில்((k->httpversion == 10) && conn->http_proxy.peer &&
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
    இல்லையெனில் எனில்((k->httpversion == 11) && conn->http_proxy.peer &&
            HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "close")) {
      /*
       * We get an HTTP/1.1 response from a proxy and it says it will
       * close down after this transfer.
       */
      connclose(conn);
      infof(data, "HTTP/1.1 proxy connection set close");
    }
    திரும்பு CURLE_OK;
  }
#endif
  எனில்((407 == k->httpcode) && HD_IS(hd, hdlen, "Proxy-authenticate:")) {
    எழுத்து *auth;
    CURLcode result;
#ifndef CURL_DISABLE_PROXY
    எனில்(!data->conn->bits.origin_is_proxy)
      /* this response is not from a forward proxy, not for us */
      திரும்பு CURLE_OK;
#endif
    auth = Curl_copy_header_மதிப்பு(hd);
    result = auth ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    எனில்(!result) {
      result = Curl_http_உள்ளீடு_auth(data, TRUE, auth);
      curlx_free(auth);
    }
    திரும்பு result;
  }
#ifdef USE_SPNEGO
  எனில்(HD_IS(hd, hdlen, "Persistent-Auth:")) {
    கட்டமைப்பு connectdata *conn = data->conn;
    கட்டமைப்பு negotiatedata *negdata = Curl_auth_nego_get(conn, FALSE);
    கட்டமைப்பு auth *authp = &data->state.authhost;
    எனில்(!negdata)
      திரும்பு CURLE_OUT_OF_MEMORY;
    எனில்(authp->picked == CURLAUTH_NEGOTIATE) {
      எழுத்து *persistentauth = Curl_copy_header_மதிப்பு(hd);
      எனில்(!persistentauth)
        திரும்பு CURLE_OUT_OF_MEMORY;
      negdata->noauthpersist = !!checkprefix("false", persistentauth);
      negdata->havenoauthpersist = TRUE;
      infof(data, "Negotiate: noauthpersist -> %d, header part: %s",
            negdata->noauthpersist, persistentauth);
      curlx_free(persistentauth);
    }
  }
#endif
  திரும்பு CURLE_OK;
}

/*
 * http_header_r() parses a single response header starting with R.
 */
நிலையான CURLcode http_header_r(கட்டமைப்பு Curl_easy *data,
                              மாறிலி எழுத்து *hd, size_t hdlen)
{
  மாறிலி எழுத்து *v = HD_VAL(hd, hdlen, "Retry-After:");
  எனில்(v) {
    /* Retry-After = HTTP-date / delay-seconds */
    curl_off_t retry_after = 0; /* zero for unknown or "now" */
    time_t date = 0;
    curlx_str_passblanks(&v);

    /* try it as a date first, because a date can otherwise start with and
       get treated as a number */
    எனில்(!Curl_getdate_capped(v, &date)) {
      time_t current = time(வெற்று);
      எனில்(date >= current)
        /* convert date to number of seconds into the future */
        retry_after = date - current;
    }
    இல்லையெனில்
      /* Try it as a decimal number, ignore errors */
      (வெறுமை)curlx_str_எண்(&v, &retry_after, CURL_OFF_T_MAX);
    /* limit to 6 hours max. this is not documented so that it can be changed
       in the future if necessary. */
    எனில்(retry_after > 21600)
      retry_after = 21600;
    data->info.retry_after = retry_after;
  }
  திரும்பு CURLE_OK;
}

/*
 * http_header_s() parses a single response header starting with S.
 */
நிலையான CURLcode http_header_s(கட்டமைப்பு Curl_easy *data,
                              மாறிலி எழுத்து *hd, size_t hdlen)
{
#எனில் !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_HSTS)
  மாறிலி எழுத்து *v;
#இல்லையெனில்
  (வெறுமை)data;
  (வெறுமை)hd;
  (வெறுமை)hdlen;
#endif

#ifndef CURL_DISABLE_COOKIES
  v = (data->cookies && data->state.cookie_engine) ?
    HD_VAL(hd, hdlen, "Set-Cookie:") : வெற்று;
  எனில்(v) {
    /* If there is a custom-set Host: name, use it here, or else use
     * real peer hostname. */
    மாறிலி எழுத்து *host = data->req.cookiehost ?
      data->req.cookiehost : data->state.origin->hostname;
    மாறிலி unsigned எழுத்து secure_context = Curl_secure_context(data, host) ?
      COOKIE_SECURE : 0;
    CURLcode result;
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    result = Curl_cookie_add(data, data->cookies, v, host,
                             data->state.up.path,
                             COOKIE_HTTPHEADER | secure_context);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    திரும்பு result;
  }
#endif
#ifndef CURL_DISABLE_HSTS
  /* If enabled, the header is incoming and this is over HTTPS */
  v = (data->hsts &&
       (Curl_xfer_is_secure(data) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_HSTS_HTTP")
#இல்லையெனில்
        0
#endif
         )
    ) ? HD_VAL(hd, hdlen, "Strict-Transport-Security:") : வெற்று;
  எனில்(v) {
    CURLcode result = Curl_hsts_parse(
      data->hsts, data->state.origin->hostname, v);
    எனில்(result) {
      எனில்(result == CURLE_OUT_OF_MEMORY)
        திரும்பு result;
      infof(data, "Illegal STS header skipped");
    }
#ifdef DEBUGBUILD
    இல்லையெனில்
      infof(data, "Parsed STS header fine (%zu entries)",
            Curl_llist_மொத்தம்(&data->hsts->list));
#endif
  }
#endif

  திரும்பு CURLE_OK;
}

/*
 * http_header_t() parses a single response header starting with T.
 */
நிலையான CURLcode http_header_t(கட்டமைப்பு Curl_easy *data,
                              மாறிலி எழுத்து *hd, size_t hdlen)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு SingleRequest *k = &data->req;

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
  மாறிலி எழுத்து *v = (!k->http_bodyless &&
                   (data->state.httpreq != HTTPREQ_HEAD) &&
                   (k->httpcode != 304)) ?
    HD_VAL(hd, hdlen, "Transfer-Encoding:") : வெற்று;
  எனில்(v) {
    /* One or more encodings. We check for chunked and/or a compression
       algorithm. */
    CURLcode result = Curl_build_unencoding_அடுக்கு(data, v, TRUE);
    எனில்(result)
      திரும்பு result;
    எனில்(!k->chunk) {
      /* if this is not chunked, only close can signal the end of this
       * transfer as Content-Length is said not to be trusted for
       * transfer-encoding! */
      CURL_TRC_M(data, "HTTP/1.1 transfer-encoding without chunks");
      connclose(conn);
      k->ignore_cl = TRUE;
    }
    திரும்பு CURLE_OK;
  }
  v = HD_VAL(hd, hdlen, "Trailer:");
  எனில்(v) {
    data->req.resp_trailer = TRUE;
    திரும்பு CURLE_OK;
  }
  திரும்பு CURLE_OK;
}

/*
 * http_header_w() parses a single response header starting with W.
 */
நிலையான CURLcode http_header_w(கட்டமைப்பு Curl_easy *data,
                              மாறிலி எழுத்து *hd, size_t hdlen)
{
  கட்டமைப்பு SingleRequest *k = &data->req;
  CURLcode result = CURLE_OK;

  எனில்((401 == k->httpcode) && HD_IS(hd, hdlen, "WWW-Authenticate:")) {
    எழுத்து *auth = Curl_copy_header_மதிப்பு(hd);
    எனில்(!auth)
      result = CURLE_OUT_OF_MEMORY;
    இல்லையெனில் {
      result = Curl_http_உள்ளீடு_auth(data, FALSE, auth);
      curlx_free(auth);
    }
  }
  திரும்பு result;
}

/*
 * http_header() parses a single response header.
 */
நிலையான CURLcode http_header(கட்டமைப்பு Curl_easy *data,
                            மாறிலி எழுத்து *hd, size_t hdlen)
{
  CURLcode result = CURLE_OK;

  தேர்வு(hd[0]) {
  நிலை 'a':
  நிலை 'A':
    result = http_header_a(data, hd, hdlen);
    நிறுத்து;
  நிலை 'c':
  நிலை 'C':
    result = http_header_c(data, hd, hdlen);
    நிறுத்து;
  நிலை 'l':
  நிலை 'L':
    result = http_header_l(data, hd, hdlen);
    நிறுத்து;
  நிலை 'p':
  நிலை 'P':
    result = http_header_p(data, hd, hdlen);
    நிறுத்து;
  நிலை 'r':
  நிலை 'R':
    result = http_header_r(data, hd, hdlen);
    நிறுத்து;
  நிலை 's':
  நிலை 'S':
    result = http_header_s(data, hd, hdlen);
    நிறுத்து;
  நிலை 't':
  நிலை 'T':
    result = http_header_t(data, hd, hdlen);
    நிறுத்து;
  நிலை 'w':
  நிலை 'W':
    result = http_header_w(data, hd, hdlen);
    நிறுத்து;
  }

  எனில்(!result) {
    கட்டமைப்பு connectdata *conn = data->conn;
    எனில்(conn->scheme->protocol & CURLPROTO_RTSP)
      result = Curl_rtsp_parseheader(data, hd);
  }
  திரும்பு result;
}

/*
 * Called after the first HTTP response line (the status line) has been
 * received and parsed.
 */
நிலையான CURLcode http_statusline(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு SingleRequest *k = &data->req;

  தேர்வு(k->httpversion) {
  நிலை 10:
  நிலை 11:
#ifdef USE_HTTP2
  நிலை 20:
#endif
#ifdef USE_HTTP3
  நிலை 30:
#endif
    /* no major version switch mid-connection */
    எனில்(k->httpversion_sent &&
       (k->httpversion / 10 != k->httpversion_sent / 10)) {
      failf(data, "Version mismatch (from HTTP/%d to HTTP/%d)",
            k->httpversion_sent / 10, k->httpversion / 10);
      திரும்பு CURLE_WEIRD_SERVER_REPLY;
    }
    நிறுத்து;
  இயல்பு:
    failf(data, "Unsupported HTTP version (%d.%d) in response",
          k->httpversion / 10, k->httpversion % 10);
    திரும்பு CURLE_UNSUPPORTED_PROTOCOL;
  }

  data->info.httpcode = k->httpcode;
  data->info.httpversion = k->httpversion;
  conn->httpversion_seen = k->httpversion;

  எனில்(!data->state.http_neg.rcvd_min ||
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
  எனில்(data->state.resume_from && data->state.httpreq == HTTPREQ_GET &&
     k->httpcode == 416) {
    /* "Requested Range Not Satisfiable", proceed and pretend this is no
       error */
    k->ignorebody = TRUE; /* Avoid appending error msg to good data. */
  }

  எனில்(k->httpversion == 10) {
    /* Default action for HTTP/1.0 must be to close, unless
       we get one of those fancy headers that tell us the
       server keeps it open for us! */
    infof(data, "HTTP 1.0, assume close after body");
    connclose(conn);
  }

  k->http_bodyless = k->httpcode >= 100 && k->httpcode < 200;
  தேர்வு(k->httpcode) {
  நிலை 304:
    /* (quote from RFC2616, section 10.3.5): The 304 response
     * MUST NOT contain a message-body, and thus is always
     * terminated by the first empty line after the header
     * fields. */
    எனில்(data->set.timecondition)
      data->info.timecond = TRUE;
    FALLTHROUGH();
  நிலை 204:
    /* (quote from RFC2616, section 10.2.5): The server has
     * fulfilled the request but does not need to return an
     * entity-body ... The 204 response MUST NOT include a
     * message-body, and thus is always terminated by the first
     * empty line after the header fields. */
    k->size = 0;
    k->maxdownload = 0;
    k->http_bodyless = TRUE;
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு CURLE_OK;
}

/* Content-Length must be ignored if any Transfer-Encoding is present in the
   response. Refer to RFC 7230 section 3.3.3 and RFC2616 section 4.4. This is
   figured out here after all headers have been received but before the final
   call to the user's header callback, so that a valid content length can be
   retrieved by the user in the final call. */
நிலையான CURLcode http_size(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு SingleRequest *k = &data->req;
  எனில்(data->req.ignore_cl || k->chunk) {
    k->size = k->maxdownload = -1;
  }
  இல்லையெனில் எனில்(k->size != -1) {
    எனில்(data->set.max_filesize &&
       !k->ignorebody &&
       (k->size > data->set.max_filesize)) {
      failf(data, "Maximum file size exceeded");
      திரும்பு CURLE_FILESIZE_EXCEEDED;
    }
    எனில்(k->ignorebody)
      infof(data, "setting size while ignoring");
    Curl_pgrsSetDownloadSize(data, k->size);
    k->maxdownload = k->size;
  }
  திரும்பு CURLE_OK;
}

CURLcode Curl_verify_header(கட்டமைப்பு Curl_easy *data,
                            மாறிலி எழுத்து *hd, size_t hdlen)
{
  கட்டமைப்பு SingleRequest *k = &data->req;
  size_t i;
  bool has_colon = FALSE;
  க்கு(i = 0; i < hdlen; i++) {
    தேர்வு(hd[i]) {
    நிலை '\0':
      /* this is bad, bail out */
      failf(data, "NUL byte in header");
      திரும்பு CURLE_WEIRD_SERVER_REPLY;
    நிலை '\r':
      எனில்(i < hdlen - 2) {
        /* CR may only precede the LF, nothing else */
        failf(data, "Carriage return found in header");
        திரும்பு CURLE_WEIRD_SERVER_REPLY;
      }
      நிறுத்து;
    நிலை ':':
      has_colon = TRUE;
      நிறுத்து;
    }
  }
  எனில்(k->headerline < 2)
    /* the first "header" is the status-line and it has no colon */
    திரும்பு CURLE_OK;
  எனில்(((hd[0] == ' ') || (hd[0] == '\t')) && k->headerline > 2)
    /* line folding, cannot happen on line 2 */
    ;
  இல்லையெனில் எனில்(!has_colon) {
    /* this is bad, bail out */
    failf(data, "Header without colon");
    திரும்பு CURLE_WEIRD_SERVER_REPLY;
  }
  திரும்பு CURLE_OK;
}

CURLcode Curl_bump_headersize(கட்டமைப்பு Curl_easy *data,
                              size_t delta,
                              bool connect_only)
{
  size_t bad = 0;
  unsigned முழு max = MAX_HTTP_RESP_HEADER_SIZE;
  எனில்(delta < MAX_HTTP_RESP_HEADER_SIZE) {
    data->info.header_size += (unsigned முழு)delta;
    data->req.allheadercount += (unsigned முழு)delta;
    எனில்(!connect_only)
      data->req.headerbytecount += (unsigned முழு)delta;
    எனில்(data->req.allheadercount > max)
      bad = data->req.allheadercount;
    இல்லையெனில் எனில்(data->info.header_size > (max * 20)) {
      bad = data->info.header_size;
      max *= 20;
    }
  }
  இல்லையெனில்
    bad = data->req.allheadercount + delta;
  எனில்(bad) {
    failf(data, "Too large response headers: %zu > %u", bad, max);
    திரும்பு CURLE_RECV_ERROR;
  }
  திரும்பு CURLE_OK;
}

/*
 * Handle a 101 Switching Protocols response. Performs the actual protocol
 * upgrade to HTTP/2 or WebSocket based on what was requested.
 */
நிலையான CURLcode http_on_101_upgrade(கட்டமைப்பு Curl_easy *data,
                                    மாறிலி எழுத்து *buf, size_t blen,
                                    size_t *pconsumed,
                                    bool *conn_changed)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு SingleRequest *k = &data->req;

#எனில் !defined(USE_NGHTTP2) && defined(CURL_DISABLE_WEBSOCKETS)
  (வெறுமை)buf;
  (வெறுமை)blen;
  (வெறுமை)pconsumed;
#இல்லையெனில்
  CURLcode result;
  முழு upgr101_requested = k->upgr101;
#endif

  எனில்(k->httpversion_sent != 11) {
    /* invalid for other HTTP versions */
    failf(data, "server sent 101 response while not talking HTTP/1.1");
    திரும்பு CURLE_WEIRD_SERVER_REPLY;
  }

  /* Whatever the success, upgrade was selected. */
  k->upgr101 = UPGR101_RECEIVED;
  conn->bits.upgrade_in_progress = FALSE;
  *conn_changed = TRUE;

  /* To be fully compliant, we would check the "Upgrade:" response header to
   * mention the protocol we requested. */
#ifdef USE_NGHTTP2
  எனில்(upgr101_requested == UPGR101_H2) {
    /* Switch to HTTP/2, where we will get more responses. blen bytes in buf
     * are already h2 protocol bytes */
    infof(data, "Received 101, Switching to HTTP/2");
    result = Curl_http2_upgrade(data, conn, FIRSTSOCKET, buf, blen);
    எனில்(!result)
      *pconsumed += blen;
    திரும்பு result;
  }
#endif
#ifndef CURL_DISABLE_WEBSOCKETS
  எனில்(upgr101_requested == UPGR101_WS) {
    /* Switch to WebSocket, where we now stream ws frames. blen bytes in buf
     * are already ws protocol bytes */
    infof(data, "Received 101, Switching to WebSocket");
    result = Curl_ws_accept(data, buf, blen);
    எனில்(!result)
      *pconsumed += blen; /* ws accept handled the data */
    திரும்பு result;
  }
#endif
  /* We silently accept this as the final response. What are we switching to
   * if we did not ask for an Upgrade? Maybe the application provided an
   * `Upgrade: xxx` header? */
  k->header = FALSE;
  திரும்பு CURLE_OK;
}

/*
 * Handle 1xx intermediate HTTP responses. Sets up state for more
 * headers and processes 100-continue and 101 upgrade responses.
 */
நிலையான CURLcode http_on_1xx_response(கட்டமைப்பு Curl_easy *data,
                                     மாறிலி எழுத்து *buf, size_t blen,
                                     size_t *pconsumed,
                                     bool *conn_changed)
{
  கட்டமைப்பு SingleRequest *k = &data->req;

  /* "A user agent MAY ignore unexpected 1xx status responses."
   * By default, we expect to get more responses after this one. */
  k->header = TRUE;
  k->headerline = 0; /* restart the header line counter */

  தேர்வு(k->httpcode) {
  நிலை 100:
    /* We have made an HTTP PUT or POST and this is 1.1-lingo that tells us
     * that the server is OK with this and ready to receive the data. */
    http_exp100_got100(data);
    நிறுத்து;
  நிலை 101:
    திரும்பு http_on_101_upgrade(data, buf, blen, pconsumed, conn_changed);
  இயல்பு:
    /* The server may send us other 1xx responses, like informative 103. This
     * has no influence on request processing and we expect to receive a
     * final response eventually. */
    நிறுத்து;
  }
  திரும்பு CURLE_OK;
}

#எனில் defined(USE_NTLM) || defined(USE_SPNEGO)
/*
 * Check if NTLM or SPNEGO authentication negotiation failed due to
 * connection closure (typically on HTTP/1.0 servers).
 */
நிலையான வெறுமை http_check_auth_closure(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு connectdata *conn)
{
  /* At this point we have some idea about the fate of the connection. If we
     are closing the connection it may result auth failure. */
#ifdef USE_NTLM
  எனில்(conn->bits.close &&
     (((data->req.httpcode == 401) &&
       (conn->http_ntlm_state == NTLMSTATE_TYPE2)) ||
      ((data->req.httpcode == 407) &&
       (conn->proxy_ntlm_state == NTLMSTATE_TYPE2)))) {
    infof(data, "Connection closure while negotiating auth (HTTP 1.0?)");
    data->state.authproblem = TRUE;
  }
#endif
#ifdef USE_SPNEGO
  எனில்(conn->bits.close &&
    (((data->req.httpcode == 401) &&
      (conn->http_negotiate_state == CURL_NEGO_AUTH_RECV)) ||
     ((data->req.httpcode == 407) &&
      (conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)))) {
    infof(data, "Connection closure while negotiating auth (HTTP 1.0?)");
    data->state.authproblem = TRUE;
  }
  எனில்((conn->http_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (data->req.httpcode != 401)) {
    conn->http_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
  எனில்((conn->proxy_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (data->req.httpcode != 407)) {
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
#endif
}
#இல்லையெனில்
#define http_check_auth_closure(x, y) /* empty */
#endif

/*
 * Handle an error response (>= 300) received while still sending the
 * request body. Deals with 417 Expectation Failed retries, keep-sending
 * on error, and aborting the send.
 */
நிலையான CURLcode http_handle_send_error(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு SingleRequest *k = &data->req;
  CURLcode result = CURLE_OK;

  எனில்(!data->req.authneg && !conn->bits.close &&
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

    தேர்வு(data->state.httpreq) {
    நிலை HTTPREQ_PUT:
    நிலை HTTPREQ_POST:
    நிலை HTTPREQ_POST_FORM:
    நிலை HTTPREQ_POST_MIME:
      /* We got an error response. If this happened before the
       * whole request body has been sent we stop sending and
       * mark the connection for closure after we have read the
       * entire response. */
      எனில்(!Curl_req_done_sending(data)) {
        எனில்((k->httpcode == 417) && http_exp100_is_selected(data)) {
          /* 417 Expectation Failed - try again without the
             Expect header */
          எனில்(!k->writebytecount && http_exp100_is_waiting(data)) {
            infof(data, "Got HTTP failure 417 while waiting for a 100");
          }
          இல்லையெனில் {
            infof(data, "Got HTTP failure 417 while sending data");
            streamclose(conn);
            result = http_perhapsrewind(data, conn);
            எனில்(result)
              திரும்பு result;
          }
          data->state.disableexpect = TRUE;
          Curl_req_abort_sending(data);
          DEBUGASSERT(!data->req.newurl);
          data->req.newurl = Curl_bufref_dup(&data->state.url);
          எனில்(!data->req.newurl)
            திரும்பு CURLE_OUT_OF_MEMORY;
        }
        இல்லையெனில் எனில்(data->set.http_keep_sending_on_error) {
          infof(data, "HTTP error before end of send, keep sending");
          http_exp100_send_anyway(data);
        }
        இல்லையெனில் {
          infof(data, "HTTP error before end of send, stop sending");
          streamclose(conn);
          result = Curl_req_abort_sending(data);
          எனில்(result)
            திரும்பு result;
        }
      }
      நிறுத்து;

    இயல்பு: /* default label present to avoid compiler warnings */
      நிறுத்து;
    }
  }

  எனில்(Curl_creader_will_rewind(data) && !Curl_req_done_sending(data)) {
    /* We rewind before next send, continue sending now */
    infof(data, "Keep sending data to get tossed away");
    CURL_REQ_SET_SEND(data);
  }
  திரும்பு result;
}

நிலையான CURLcode http_on_response(கட்டமைப்பு Curl_easy *data,
                                 மாறிலி எழுத்து *last_hd, size_t last_hd_len,
                                 மாறிலி எழுத்து *buf, size_t blen,
                                 size_t *pconsumed)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  கட்டமைப்பு SingleRequest *k = &data->req;
  bool conn_changed = FALSE;

  (வெறுமை)buf; /* not used without HTTP2 enabled */
  *pconsumed = 0;

  எனில்(k->upgr101 == UPGR101_RECEIVED) {
    /* supposedly upgraded to http2 now */
    எனில்(data->req.httpversion != 20)
      infof(data, "Lying server, not serving HTTP/2");
  }

  எனில்(k->httpcode < 200 && last_hd) {
    /* Intermediate responses might trigger processing of more responses,
     * write the last header to the client before proceeding. */
    result = http_write_header(data, last_hd, last_hd_len);
    last_hd = வெற்று; /* handled it */
    எனில்(result)
      செல் out;
  }

  எனில்(k->httpcode < 100) {
    failf(data, "Unsupported response code in HTTP response");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    செல் out;
  }
  இல்லையெனில் எனில்(k->httpcode < 200) {
    result = http_on_1xx_response(data, buf, blen, pconsumed, &conn_changed);
    செல் out;
  }

  /* k->httpcode >= 200, final response */
  k->header = FALSE;
  எனில்(conn->bits.upgrade_in_progress) {
    /* Asked for protocol upgrade, but it was not selected */
    conn->bits.upgrade_in_progress = FALSE;
    conn_changed = TRUE;
  }

  எனில்((k->size == -1) && !k->chunk && !conn->bits.close &&
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
  எனில்(data->req.upgr101 == UPGR101_WS) {
    failf(data, "Refused WebSocket upgrade: %d", k->httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
    செல் out;
  }
#endif

  /* Check if this response means the transfer errored. */
  எனில்(http_should_fail(data, data->req.httpcode)) {
    failf(data, "The requested URL returned error: %d",
          k->httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
    செல் out;
  }

  /* Curl_http_auth_act() checks what authentication methods that are
   * available and decides which one (if any) to use. It will set 'newurl' if
   * an auth method was picked. */
  result = Curl_http_auth_act(data);
  எனில்(result)
    செல் out;

  எனில்(k->httpcode >= 300) {
    result = http_handle_send_error(data);
    எனில்(result)
      செல் out;
  }

  /* final response without error, prepare to receive the body */
  result = http_firstwrite(data);
  எனில்(result)
    செல் out;

  /* This is the last response that we get for the current request. Check on
   * the body size and determine if the response is complete. */
  result = http_size(data);
  எனில்(result)
    செல் out;

#ifndef CURL_DISABLE_RTSP
  /* An RTSP response to any method may carry a body, which is announced
     with Content-Length (RFC 2326 section 12.14). Read it instead of
     treating it as excess data, unless the application asked for no body
     with CURLOPT_NOBODY. */
  எனில்((conn->scheme->protocol & CURLPROTO_RTSP) && data->req.no_body &&
     !data->set.opt_no_body && (k->size > 0))
    data->req.no_body = FALSE;
#endif

  /* If we requested a "no body", this is a good time to get
   * out and return home.
   */
  எனில்(data->req.no_body)
    k->download_done = TRUE;

  /* If max download size is *zero* (nothing) we already have nothing and can
     safely return ok now! For HTTP/2, we would like to call
     http2_handle_stream_close to properly close a stream. In order to do
     this, we keep reading until we close the stream. */
  எனில்((k->maxdownload == 0) && (k->httpversion_sent < 20))
    k->download_done = TRUE;

out:
  எனில்(last_hd)
    /* if not written yet, write it now */
    result = Curl_1st_fatal(result,
                            http_write_header(data, last_hd, last_hd_len));
  எனில்(conn_changed)
    /* poke the multi handle to allow pending pipewait to retry */
    Curl_multi_connchanged(data->multi);
  திரும்பு result;
}

நிலையான CURLcode http_rw_hd(கட்டமைப்பு Curl_easy *data,
                           மாறிலி எழுத்து *hd, size_t hdlen,
                           மாறிலி எழுத்து *buf_remain, size_t blen,
                           size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு SingleRequest *k = &data->req;
  முழு writetype;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  *pconsumed = 0;
  எனில்((0x0a == *hd) || (0x0d == *hd)) {
    /* Empty header line means end of headers! */
    கட்டமைப்பு dynbuf last_header;
    size_t consumed;

    curlx_dyn_init(&last_header, hdlen + 1);
    result = curlx_dyn_addn(&last_header, hd, hdlen);
    எனில்(result)
      திரும்பு result;

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
    திரும்பு result;
  }

  /*
   * Checks for special headers coming up.
   */

  writetype = CLIENTWRITE_HEADER;
  எனில்(!k->headerline++) {
    /* This is the first header, it MUST be the error code line
       or else we consider this to be the body right away! */
    bool fine_statusline = FALSE;

    k->httpversion = 0; /* Do not know yet */
    எனில்(data->conn->scheme->protocol & PROTO_FAMILY_HTTP) {
      /*
       * https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.2
       *
       * The response code is always a three-digit number in HTTP as the spec
       * says. We allow any three-digit number here, but we cannot make
       * guarantees on future behaviors since it is not within the protocol.
       */
      மாறிலி எழுத்து *p = hd;

      curlx_str_passblanks(&p);
      எனில்(!strncmp(p, "HTTP/", 5)) {
        p += 5;
        தேர்வு(*p) {
        நிலை '1':
          p++;
          எனில்((p[0] == '.') && (p[1] == '0' || p[1] == '1')) {
            எனில்(ISBLANK(p[2])) {
              k->httpversion = (unsigned எழுத்து)(10 + (p[1] - '0'));
              p += 3;
              எனில்(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
                k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
                  (p[2] - '0');
                /* RFC 9112 requires a single space following the status code,
                   but the browsers do not so let's not insist */
                fine_statusline = TRUE;
              }
            }
          }
          எனில்(!fine_statusline) {
            failf(data, "Unsupported HTTP/1 subversion in response");
            திரும்பு CURLE_UNSUPPORTED_PROTOCOL;
          }
          நிறுத்து;
        நிலை '2':
        நிலை '3':
          எனில்(!ISBLANK(p[1]))
            நிறுத்து;
          k->httpversion = (unsigned எழுத்து)((*p - '0') * 10);
          p += 2;
          எனில்(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
            k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
              (p[2] - '0');
            p += 3;
            எனில்(!ISBLANK(*p))
              நிறுத்து;
            fine_statusline = TRUE;
          }
          நிறுத்து;
        இயல்பு: /* unsupported */
          failf(data, "Unsupported HTTP version in response");
          திரும்பு CURLE_UNSUPPORTED_PROTOCOL;
        }
      }

      எனில்(!fine_statusline) {
        /* If user has set option HTTP200ALIASES,
           compare header line against list of aliases */
        statusline check = checkhttpprefix(data, hd, hdlen);
        எனில்(check == STATUS_DONE) {
          fine_statusline = TRUE;
          k->httpcode = 200;
          k->httpversion = 10;
        }
      }
    }
    இல்லையெனில் எனில்(data->conn->scheme->protocol & CURLPROTO_RTSP) {
      மாறிலி எழுத்து *p = hd;
      கட்டமைப்பு Curl_str ver;
      curl_off_t status;
      /* we set the max string a little excessive to forgive some leading
         spaces */
      எனில்(!curlx_str_until(&p, &ver, 32, ' ') &&
         !curlx_str_single(&p, ' ') &&
         !curlx_str_எண்(&p, &status, 999)) {
        curlx_str_trimblanks(&ver);
        எனில்(curlx_str_cmp(&ver, "RTSP/1.0")) {
          k->httpcode = (முழு)status;
          fine_statusline = TRUE;
          k->httpversion = 11; /* RTSP acts like HTTP 1.1 */
        }
      }
      எனில்(!fine_statusline)
        திரும்பு CURLE_WEIRD_SERVER_REPLY;
    }

    எனில்(fine_statusline) {
      result = http_statusline(data, data->conn);
      எனில்(result)
        திரும்பு result;
      writetype |= CLIENTWRITE_STATUS;
    }
    இல்லையெனில் {
      k->header = FALSE;   /* this is not a header line */
      திரும்பு CURLE_WEIRD_SERVER_REPLY;
    }
  }

  result = Curl_verify_header(data, hd, hdlen);
  எனில்(result)
    திரும்பு result;

  result = http_header(data, hd, hdlen);
  எனில்(result)
    திரும்பு result;

  /*
   * Taken in one (more) header. Write it to the client.
   */
  Curl_debug(data, CURLINFO_HEADER_IN, hd, hdlen);

  எனில்(k->httpcode / 100 == 1)
    writetype |= CLIENTWRITE_1XX;
  result = Curl_client_write(data, writetype, hd, hdlen);
  எனில்(result)
    திரும்பு result;

  result = Curl_bump_headersize(data, hdlen, FALSE);
  எனில்(result)
    திரும்பு result;

  திரும்பு CURLE_OK;
}

/* remove trailing CRLF then all trailing whitespace */
வெறுமை Curl_http_to_fold(கட்டமைப்பு dynbuf *bf)
{
  size_t len = curlx_dyn_len(bf);
  மாறிலி எழுத்து *hd = curlx_dyn_ptr(bf);
  எனில்(len && (hd[len - 1] == '\n'))
    len--;
  எனில்(len && (hd[len - 1] == '\r'))
    len--;
  போது(len && ISBLANK(hd[len - 1])) /* strip off trailing whitespace */
    len--;
  curlx_dyn_setlen(bf, len);
}

நிலையான வெறுமை unfold_header(கட்டமைப்பு Curl_easy *data)
{
  Curl_http_to_fold(&data->state.headerb);
  data->state.leading_unfold = TRUE;
}

/*
 * Read any HTTP header lines from the server and pass them to the client app.
 */
நிலையான CURLcode http_parse_headers(கட்டமைப்பு Curl_easy *data,
                                   மாறிலி எழுத்து *buf, size_t blen,
                                   size_t *pconsumed)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  கட்டமைப்பு SingleRequest *k = &data->req;
  மாறிலி எழுத்து *end_ptr;
  bool leftover_body = FALSE;

  /* we have bytes for the next header, make sure it is not a folded header
     before passing it on */
  எனில்(data->state.maybe_folded && blen) {
    எனில்(ISBLANK(buf[0])) {
      /* folded, remove the trailing newlines and append the next header */
      unfold_header(data);
    }
    இல்லையெனில் {
      /* the header data we hold is a complete header, pass it on */
      size_t ignore_this;
      result = http_rw_hd(data, curlx_dyn_ptr(&data->state.headerb),
                          curlx_dyn_len(&data->state.headerb),
                          வெற்று, 0, &ignore_this);
      curlx_dyn_reset(&data->state.headerb);
      எனில்(result)
        திரும்பு result;
    }
    data->state.maybe_folded = FALSE;
  }

  /* header line within buffer loop */
  *pconsumed = 0;
  போது(blen && k->header) {
    size_t consumed;
    size_t hlen;
    மாறிலி எழுத்து *hd;
    size_t unfold_len = 0;

    எனில்(data->state.leading_unfold) {
      /* immediately after an unfold, keep only a single whitespace */
      போது(blen && ISBLANK(buf[0])) {
        buf++;
        blen--;
        unfold_len++;
      }
      எனில்(blen) {
        /* insert a single space */
        result = curlx_dyn_addn(&data->state.headerb, " ", 1);
        எனில்(result)
          திரும்பு result;
        data->state.leading_unfold = FALSE; /* done now */
      }
    }

    end_ptr = memchr(buf, '\n', blen);
    எனில்(!end_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      result = curlx_dyn_addn(&data->state.headerb, buf, blen);
      எனில்(result)
        திரும்பு result;
      *pconsumed += blen + unfold_len;

      எனில்(!k->headerline) {
        /* check if this looks like a protocol header */
        statusline st =
          checkprotoprefix(data, conn,
                           curlx_dyn_ptr(&data->state.headerb),
                           curlx_dyn_len(&data->state.headerb));

        எனில்(st == STATUS_BAD) {
          /* this is not the beginning of a protocol first header line.
           * Cannot be 0.9 if version was detected or connection was reused. */
          k->header = FALSE;
          streamclose(conn);
          எனில்((k->httpversion >= 10) || conn->bits.reuse) {
            failf(data, "Invalid status line");
            திரும்பு CURLE_WEIRD_SERVER_REPLY;
          }
          எனில்(!data->state.http_neg.accept_09) {
            failf(data, "Received HTTP/0.9 when not allowed");
            திரும்பு CURLE_UNSUPPORTED_PROTOCOL;
          }
          leftover_body = TRUE;
          செல் out;
        }
      }
      செல் out; /* read more and try again */
    }

    /* the size of the remaining header line */
    consumed = (end_ptr - buf) + 1;

    result = curlx_dyn_addn(&data->state.headerb, buf, consumed);
    எனில்(result)
      திரும்பு result;
    blen -= consumed;
    buf += consumed;
    *pconsumed += consumed + unfold_len;

    /****
     * We now have a FULL header line in 'headerb'.
     *****/

    hlen = curlx_dyn_len(&data->state.headerb);
    hd = curlx_dyn_ptr(&data->state.headerb);

    எனில்(!k->headerline) {
      /* the first read "header", the status line */
      statusline st = checkprotoprefix(data, conn, hd, hlen);
      எனில்(st == STATUS_BAD) {
        streamclose(conn);
        /* this is not the beginning of a protocol first header line.
         * Cannot be 0.9 if version was detected or connection was reused. */
        எனில்((k->httpversion >= 10) || conn->bits.reuse) {
          failf(data, "Invalid status line");
          திரும்பு CURLE_WEIRD_SERVER_REPLY;
        }
        எனில்(!data->state.http_neg.accept_09) {
          failf(data, "Received HTTP/0.9 when not allowed");
          திரும்பு CURLE_UNSUPPORTED_PROTOCOL;
        }
        k->header = FALSE;
        leftover_body = TRUE;
        செல் out;
      }
    }
    இல்லையெனில் {
      எனில்(hlen && !ISNEWLINE(hd[0])) {
        /* this is NOT the header separator */

        /* if we have bytes for the next header, check for folding */
        எனில்(blen && ISBLANK(buf[0])) {
          /* remove the trailing CRLF and append the next header */
          unfold_header(data);
          தொடர்;
        }
        இல்லையெனில் எனில்(!blen) {
          /* this might be a folded header so deal with it in next invoke */
          data->state.maybe_folded = TRUE;
          நிறுத்து;
        }
      }
    }

    result = http_rw_hd(data, hd, hlen, buf, blen, &consumed);
    /* We are done with this line. We reset because response
     * processing might switch to HTTP/2 and that might call us
     * directly again. */
    curlx_dyn_reset(&data->state.headerb);
    எனில்(consumed) {
      blen -= consumed;
      buf += consumed;
      *pconsumed += consumed;
    }
    எனில்(result)
      திரும்பு result;
  }

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */
out:
  எனில்(!k->header && !leftover_body) {
    curlx_dyn_free(&data->state.headerb);
  }
  திரும்பு CURLE_OK;
}

CURLcode Curl_http_write_resp_hd(கட்டமைப்பு Curl_easy *data,
                                 மாறிலி எழுத்து *hd, size_t hdlen,
                                 bool is_eos)
{
  CURLcode result;
  size_t consumed;
  எழுத்து tmp = 0;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  result = http_rw_hd(data, hd, hdlen, &tmp, 0, &consumed);
  எனில்(!result && is_eos) {
    result = Curl_client_write(data, (CLIENTWRITE_BODY | CLIENTWRITE_EOS),
                               &tmp, 0);
  }
  திரும்பு result;
}

/*
 * HTTP protocol `write_resp` implementation. Parse headers
 * when not done yet and otherwise return without consuming data.
 */
CURLcode Curl_http_write_resp_hds(கட்டமைப்பு Curl_easy *data,
                                  மாறிலி எழுத்து *buf, size_t blen,
                                  size_t *pconsumed)
{
  எனில்(!data->req.header) {
    *pconsumed = 0;
    திரும்பு CURLE_OK;
  }
  இல்லையெனில் {
    CURLcode result;

    result = http_parse_headers(data, buf, blen, pconsumed);
    எனில்(!result && !data->req.header) {
      எனில்(!data->req.no_body && curlx_dyn_len(&data->state.headerb)) {
        /* leftover from parsing something that turned out not
         * to be a header, only happens if we allow for
         * HTTP/0.9 like responses */
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   curlx_dyn_ptr(&data->state.headerb),
                                   curlx_dyn_len(&data->state.headerb));
      }
      curlx_dyn_free(&data->state.headerb);
    }
    திரும்பு result;
  }
}

CURLcode Curl_http_write_resp(கட்டமைப்பு Curl_easy *data,
                              மாறிலி எழுத்து *buf, size_t blen,
                              bool is_eos)
{
  CURLcode result;
  size_t consumed;
  முழு flags;

  result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
  எனில்(result || data->req.done)
    செல் out;

  DEBUGASSERT(consumed <= blen);
  blen -= consumed;
  buf += consumed;
  /* either all was consumed in header parsing, or we have data left
   * and are done with headers, e.g. it is BODY data */
  DEBUGASSERT(!blen || !data->req.header);
  எனில்(!data->req.header && (blen || is_eos)) {
    /* BODY data after header been parsed, write and consume */
    flags = CLIENTWRITE_BODY;
    எனில்(is_eos)
      flags |= CLIENTWRITE_EOS;
    result = Curl_client_write(data, flags, buf, blen);
  }
out:
  திரும்பு result;
}

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_status(int32_t *pstatus, மாறிலி எழுத்து *s, size_t len)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  int32_t status = 0;
  முழு i;

  எனில்(len != 3)
    செல் out;

  க்கு(i = 0; i < 3; ++i) {
    எழுத்து c = s[i];

    எனில்(c < '0' || c > '9')
      செல் out;

    status *= 10;
    status += c - '0';
  }
  result = CURLE_OK;
out:
  *pstatus = result ? -1 : status;
  திரும்பு result;
}

CURLcode Curl_http_req_make(கட்டமைப்பு httpreq **preq,
                            மாறிலி எழுத்து *method, size_t m_len,
                            மாறிலி எழுத்து *scheme, size_t s_len,
                            மாறிலி எழுத்து *authority, size_t a_len,
                            மாறிலி எழுத்து *path, size_t p_len)
{
  கட்டமைப்பு httpreq *req;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, அளவு(*req) + m_len);
  எனில்(!req)
    செல் out;
#எனில் defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic தள்ளு
/* error: 'memcpy' offset [137, 142] from the object at 'req' is out of
   the bounds of referenced subobject 'method' with type 'char[1]' at
   offset 136 */
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
  memcpy(req->method, method, m_len);
#எனில் defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic எடு
#endif
  எனில்(scheme) {
    req->scheme = curlx_memdup0(scheme, s_len);
    எனில்(!req->scheme)
      செல் out;
  }
  எனில்(authority) {
    req->authority = curlx_memdup0(authority, a_len);
    எனில்(!req->authority)
      செல் out;
  }
  எனில்(path) {
    req->path = curlx_memdup0(path, p_len);
    எனில்(!req->path)
      செல் out;
  }
  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  எனில்(result && req)
    Curl_http_req_free(req);
  *preq = result ? வெற்று : req;
  திரும்பு result;
}

நிலையான CURLcode req_assign_url_authority(கட்டமைப்பு httpreq *req, CURLU *url)
{
  எழுத்து *host, *port;
  கட்டமைப்பு dynbuf buf;
  CURLUcode uc;
  CURLcode result = CURLE_URL_MALFORMAT;

  host = port = வெற்று;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_HOST, &host, 0);
  எனில்(uc && uc != CURLUE_NO_HOST)
    செல் out;
  எனில்(!host) {
    req->authority = வெற்று;
    result = CURLE_OK;
    செல் out;
  }

  uc = curl_url_get(url, CURLUPART_PORT, &port, CURLU_NO_DEFAULT_PORT);
  எனில்(uc && uc != CURLUE_NO_PORT)
    செல் out;

  result = curlx_dyn_add(&buf, host);
  எனில்(result)
    செல் out;
  எனில்(port) {
    result = curlx_dyn_addf(&buf, ":%s", port);
    எனில்(result)
      செல் out;
  }
  req->authority = curlx_dyn_ptr(&buf);
out:
  curlx_free(host);
  curlx_free(port);
  எனில்(result)
    curlx_dyn_free(&buf);
  திரும்பு result;
}

நிலையான CURLcode req_assign_url_path(கட்டமைப்பு httpreq *req, CURLU *url)
{
  எழுத்து *path, *query;
  கட்டமைப்பு dynbuf buf;
  CURLUcode uc;
  CURLcode result = CURLE_URL_MALFORMAT;

  path = query = வெற்று;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_PATH, &path, 0);
  எனில்(uc)
    செல் out;
  uc = curl_url_get(url, CURLUPART_QUERY, &query, 0);
  எனில்(uc && uc != CURLUE_NO_QUERY)
    செல் out;

  எனில்(!query) {
    req->path = path;
    path = வெற்று;
  }
  இல்லையெனில் {
    result = curlx_dyn_add(&buf, path);
    எனில்(!result)
      result = curlx_dyn_addf(&buf, "?%s", query);
    எனில்(result)
      செல் out;
    req->path = curlx_dyn_ptr(&buf);
  }
  result = CURLE_OK;

out:
  curlx_free(path);
  curlx_free(query);
  எனில்(result)
    curlx_dyn_free(&buf);
  திரும்பு result;
}

CURLcode Curl_http_req_make2(கட்டமைப்பு httpreq **preq,
                             மாறிலி எழுத்து *method, size_t m_len,
                             CURLU *url, மாறிலி எழுத்து *scheme_default)
{
  கட்டமைப்பு httpreq *req;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  CURLUcode uc;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, அளவு(*req) + m_len);
  எனில்(!req)
    செல் out;
  memcpy(req->method, method, m_len);

  uc = curl_url_get(url, CURLUPART_SCHEME, &req->scheme, 0);
  எனில்(uc && uc != CURLUE_NO_SCHEME)
    செல் out;
  எனில்(!req->scheme && scheme_default) {
    req->scheme = curlx_strdup(scheme_default);
    எனில்(!req->scheme)
      செல் out;
  }

  result = req_assign_url_authority(req, url);
  எனில்(result)
    செல் out;
  result = req_assign_url_path(req, url);
  எனில்(result)
    செல் out;

  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  எனில்(result && req)
    Curl_http_req_free(req);
  *preq = result ? வெற்று : req;
  திரும்பு result;
}

வெறுமை Curl_http_req_free(கட்டமைப்பு httpreq *req)
{
  எனில்(req) {
    curlx_free(req->scheme);
    curlx_free(req->authority);
    curlx_free(req->path);
    Curl_dynhds_free(&req->headers);
    Curl_dynhds_free(&req->trailers);
    curlx_free(req);
  }
}

கட்டமைப்பு name_const {
  மாறிலி எழுத்து *name;
  size_t namelen;
};

நிலையான மாறிலி கட்டமைப்பு name_const H2_NON_FIELD[] = {
  { STRCONST("Host") },
  { STRCONST("Upgrade") },
  { STRCONST("Connection") },
  { STRCONST("Keep-Alive") },
  { STRCONST("Proxy-Connection") },
  { STRCONST("Transfer-Encoding") },
};

நிலையான bool h2_permissible_field(கட்டமைப்பு dynhds_entry *e)
{
  size_t i;
  க்கு(i = 0; i < CURL_ARRAYSIZE(H2_NON_FIELD); ++i) {
    எனில்(e->namelen == H2_NON_FIELD[i].namelen &&
       curl_strnequal(H2_NON_FIELD[i].name, e->name, e->namelen))
      திரும்பு FALSE;
  }
  திரும்பு TRUE;
}

நிலையான bool http_TE_has_token(மாறிலி எழுத்து *fvalue, மாறிலி எழுத்து *token)
{
  போது(*fvalue) {
    கட்டமைப்பு Curl_str name;

    /* skip to first token */
    போது(ISBLANK(*fvalue) || *fvalue == ',')
      fvalue++;
    எனில்(curlx_str_cspn(&fvalue, &name, " \t\r;,"))
      திரும்பு FALSE;
    எனில்(curlx_str_casecompare(&name, token))
      திரும்பு TRUE;

    /* skip any remainder after token, e.g. parameters with quoted strings */
    போது(*fvalue && *fvalue != ',') {
      எனில்(*fvalue == '"') {
        கட்டமைப்பு Curl_str qw;
        /* if we do not cleanly find a quoted word here, the header value
         * does not follow HTTP syntax and we reject */
        எனில்(curlx_str_quotedword(&fvalue, &qw, CURL_MAX_HTTP_HEADER))
          திரும்பு FALSE;
      }
      இல்லையெனில்
        fvalue++;
    }
  }
  திரும்பு FALSE;
}

CURLcode Curl_http_req_to_h2(கட்டமைப்பு dynhds *h2_headers,
                             கட்டமைப்பு httpreq *req, கட்டமைப்பு Curl_easy *data)
{
  மாறிலி எழுத்து *scheme = வெற்று, *authority = வெற்று;
  கட்டமைப்பு dynhds_entry *e;
  size_t i;
  CURLcode result;

  DEBUGASSERT(req);
  DEBUGASSERT(h2_headers);

  எனில்(req->scheme) {
    scheme = req->scheme;
  }
  இல்லையெனில் எனில்(strcmp("CONNECT", req->method)) {
    scheme = Curl_checkheaders(data, STRCONST(HTTP_PSEUDO_SCHEME));
    எனில்(scheme) {
      scheme += அளவு(HTTP_PSEUDO_SCHEME);
      curlx_str_passblanks(&scheme);
      infof(data, "set pseudo header %s to %s", HTTP_PSEUDO_SCHEME, scheme);
    }
    இல்லையெனில் {
      scheme = data->state.origin->scheme->name;
    }
  }

  எனில்(req->authority) {
    authority = req->authority;
  }
  இல்லையெனில் {
    e = Curl_dynhds_get(&req->headers, STRCONST("Host"));
    எனில்(e)
      authority = e->மதிப்பு;
  }

  Curl_dynhds_reset(h2_headers);
  Curl_dynhds_set_opts(h2_headers, DYNHDS_OPT_LOWERCASE);
  result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_METHOD),
                           req->method, strlen(req->method));
  எனில்(!result && scheme) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_SCHEME),
                             scheme, strlen(scheme));
  }
  எனில்(!result && authority) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_AUTHORITY),
                             authority, strlen(authority));
  }
  எனில்(!result && req->path) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_PATH),
                             req->path, strlen(req->path));
  }
  க்கு(i = 0; !result && i < Curl_dynhds_மொத்தம்(&req->headers); ++i) {
    e = Curl_dynhds_getn(&req->headers, i);
    /* "TE" is special in that it is only permissible when it
     * has only value "trailers". RFC 9113 ch. 8.2.2 */
    எனில்(e->namelen == 2 && curl_strequal("TE", e->name)) {
      எனில்(http_TE_has_token(e->மதிப்பு, "trailers"))
        result = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                                 "trailers", CURL_CSTRLEN("trailers"));
    }
    இல்லையெனில் எனில்(h2_permissible_field(e)) {
      result = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                               e->மதிப்பு, e->valuelen);
    }
  }

  திரும்பு result;
}

CURLcode Curl_http_resp_make(கட்டமைப்பு http_resp **presp,
                             முழு status,
                             மாறிலி எழுத்து *description)
{
  கட்டமைப்பு http_resp *resp;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  resp = curlx_calloc(1, அளவு(*resp));
  எனில்(!resp)
    செல் out;

  resp->status = status;
  எனில்(description) {
    resp->description = curlx_strdup(description);
    எனில்(!resp->description)
      செல் out;
  }
  Curl_dynhds_init(&resp->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&resp->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  எனில்(result && resp)
    Curl_http_resp_free(resp);
  *presp = result ? வெற்று : resp;
  திரும்பு result;
}

வெறுமை Curl_http_resp_free(கட்டமைப்பு http_resp *resp)
{
  எனில்(resp) {
    curlx_free(resp->description);
    Curl_dynhds_free(&resp->headers);
    Curl_dynhds_free(&resp->trailers);
    எனில்(resp->prev)
      Curl_http_resp_free(resp->prev);
    curlx_free(resp);
  }
}

/*
 * HTTP handler interface.
 */
மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_http = {
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
