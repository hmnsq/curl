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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"

#ifndef CURL_DISABLE_HTTP

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif

#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ଅନ୍ତର୍ଭାଵ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#ଅନ୍ତର୍ଭାଵ <sys/param.h>
#endif

#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "formdata.h"
#ଅନ୍ତର୍ଭାଵ "mime.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "curlx/base64.h"
#ଅନ୍ତର୍ଭାଵ "cookie.h"
#ଅନ୍ତର୍ଭାଵ "vauth/vauth.h"
#ଅନ୍ତର୍ଭାଵ "vquic/vquic.h"
#ଅନ୍ତର୍ଭାଵ "http_digest.h"
#ଅନ୍ତର୍ଭାଵ "http_ntlm.h"
#ଅନ୍ତର୍ଭାଵ "http_negotiate.h"
#ଅନ୍ତର୍ଭାଵ "http_aws_sigv4.h"
#ଅନ୍ତର୍ଭାଵ "http_httpsig.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "urlapi-int.h"
#ଅନ୍ତର୍ଭାଵ "curl_share.h"
#ଅନ୍ତର୍ଭାଵ "dynhds.h"
#ଅନ୍ତର୍ଭାଵ "http.h"
#ଅନ୍ତର୍ଭାଵ "headers.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "parsedate.h" /* for the week day and month names */
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "content_encoding.h"
#ଅନ୍ତର୍ଭାଵ "http_proxy.h"
#ଅନ୍ତର୍ଭାଵ "http2.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"
#ଅନ୍ତର୍ଭାଵ "altsvc.h"
#ଅନ୍ତର୍ଭାଵ "hsts.h"
#ଅନ୍ତର୍ଭାଵ "rtsp.h"
#ଅନ୍ତର୍ଭାଵ "ws.h"
#ଅନ୍ତର୍ଭାଵ "bufref.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

ଶୂନ୍ୟ Curl_http_neg_init(ସଂରଚନା Curl_easy *data, ସଂରଚନା http_negotiation *neg)
{
  memset(neg, 0, ଆକାର(*neg));
  neg->accept_09 = data->set.http09_allowed;
  ବାଛ(data->set.httpwant) {
  ସ୍ଥିତି CURL_HTTP_VERSION_1_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    neg->only_10 = TRUE;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_HTTP_VERSION_1_1:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_HTTP_VERSION_2_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->h2_upgrade = TRUE;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_HTTP_VERSION_2TLS:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    neg->wanted = neg->allowed = (CURL_HTTP_V2x);
    data->state.http_neg.h2_prior_knowledge = TRUE;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_HTTP_VERSION_3:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    neg->allowed = neg->wanted;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_HTTP_VERSION_3ONLY:
    neg->wanted = neg->allowed = (CURL_HTTP_V3x);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_HTTP_VERSION_NONE:
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    ଭାଙ୍ଗ;
  }
}

CURLcode Curl_http_setup_conn(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା connectdata *conn)
{
  /* allocate the HTTP-specific struct for the Curl_easy, only to survive
     during this request */
  ଯଦି(data->state.http_neg.wanted == CURL_HTTP_V3x) {
    /* only HTTP/3, needs to work */
    CURLcode result = Curl_conn_may_http3(data, conn);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଫେରନ୍ତୁ CURLE_OK;
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
ଅକ୍ଷର *Curl_checkProxyheaders(ସଂରଚନା Curl_easy *data,
                             ଅଚଳ ସଂରଚନା connectdata *conn,
                             ଅଚଳ ଅକ୍ଷର *thisheader,
                             ଅଚଳ size_t thislen)
{
  ସଂରଚନା curl_slist *head;

  ପାଇଁ(head = (conn->http_proxy.peer && data->set.sep_headers) ?
        data->set.proxyheaders : data->set.headers;
      head; head = head->next) {
    ଯଦି(curl_strnequal(head->data, thisheader, thislen) &&
       Curl_headersep(head->data[thislen]))
      ଫେରନ୍ତୁ head->data;
  }

  ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/* If the header has a value, this function returns TRUE and the value is in
   'outp' with blanks trimmed off. */
ସ୍ଥିର bool header_has_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର **headerp, ସଂରଚନା Curl_str *outp)
{
  bool ମୂଲ୍ୟ = !curlx_str_cspn(headerp, outp, ";:") &&
    (!curlx_str_single(headerp, ':') || !curlx_str_single(headerp, ';'));

  ଯଦି(ମୂଲ୍ୟ) {
    curlx_str_cspn(headerp, outp, "\r\n");
    curlx_str_trimblanks(outp);
  }
  ଫେରନ୍ତୁ ମୂଲ୍ୟ;
}

ସ୍ଥିର bool http_header_is_empty(ଅଚଳ ଅକ୍ଷର *header)
{
  ସଂରଚନା Curl_str out;

  ଯଦି(header_has_ମୂଲ୍ୟ(&header, &out)) {
    ଫେରନ୍ତୁ curlx_strlen(&out) == 0;
  }
  ଫେରନ୍ତୁ TRUE; /* invalid header format, treat as empty */
}

/*
 * Strip off leading and trailing whitespace from the value in the given HTTP
 * header line and return a strdup-ed copy in 'valp' - returns an empty
 * string if the header value consists entirely of whitespace.
 *
 * If the header is provided as "name;", ending with a semicolon, it returns a
 * blank string.
 */
ସ୍ଥିର CURLcode copy_custom_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର *header, ଅକ୍ଷର **valp)
{
  ସଂରଚନା Curl_str out = { 0 };

  /* find the end of the header name */
  ଯଦି(header_has_ମୂଲ୍ୟ(&header, &out)) {
    *valp = curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
    ଯଦି(*valp)
      ଫେରନ୍ତୁ CURLE_OK;
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }
  /* bad input */
  *valp = ଖାଲି;
  ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
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
ଅକ୍ଷର *Curl_copy_header_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର *header)
{
  ସଂରଚନା Curl_str out;

  /* find the end of the header name */
  ଯଦି(!curlx_str_until(&header, &out, MAX_HTTP_RESP_HEADER_SIZE, ':') &&
     !curlx_str_single(&header, ':')) {
    curlx_str_untilnl(&header, &out, MAX_HTTP_RESP_HEADER_SIZE);
    curlx_str_trimblanks(&out);
    ଫେରନ୍ତୁ curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
  }
  /* bad input, should never happen */
  DEBUGASSERT(0);
  ଫେରନ୍ତୁ ଖାଲି;
}

#ifndef CURL_DISABLE_HTTP_AUTH

#ifndef CURL_DISABLE_BASIC_AUTH
/*
 * http_output_basic() sets up an Authorization: header (or the proxy version)
 * for HTTP Basic authentication.
 *
 * Returns CURLcode.
 */
ସ୍ଥିର CURLcode http_output_basic(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା connectdata *conn, bool proxy)
{
  size_t size = 0;
  ଅକ୍ଷର *authorization = ଖାଲି;
  ଅକ୍ଷର **p_hd;
  CURLcode result;
  ସଂରଚନା Curl_creds *creds = ଖାଲି;
  ଅକ୍ଷର *out;

  /* credentials are unique per transfer for HTTP, do not use the ones for the
     connection */
  ଯଦି(proxy) {
#ifndef CURL_DISABLE_PROXY
    p_hd = &data->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
#ଅନ୍ୟଥା
    (ଶୂନ୍ୟ)conn;
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif
  }
  ଅନ୍ୟଥା {
    p_hd = &data->req.hd_auth;
    creds = data->state.creds;
  }

  ଯଦି(!creds) {
    DEBUGASSERT(0);
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  out = curl_maprintf("%s:%s", creds->user, creds->passwd);
  ଯଦି(!out)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  result = curlx_base64_encode((uint8_t *)out, strlen(out),
                               &authorization, &size);
  ଯଦି(result)
    ଯାଅ fail;

  ଯଦି(!authorization) {
    result = CURLE_REMOTE_ACCESS_DENIED;
    ଯାଅ fail;
  }

  curlx_free(*p_hd);
  *p_hd = curl_maprintf("%sAuthorization: Basic %s\r\n",
                        proxy ? "Proxy-" : "",
                        authorization);
  curlx_free(authorization);
  ଯଦି(!*p_hd) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ fail;
  }

fail:
  curlx_free(out);
  ଫେରନ୍ତୁ result;
}

#endif

#ifndef CURL_DISABLE_BEARER_AUTH
/*
 * http_output_bearer() sets up an Authorization: header
 * for HTTP Bearer authentication.
 *
 * Returns CURLcode.
 */
ସ୍ଥିର CURLcode http_output_bearer(ସଂରଚନା Curl_easy *data)
{
  ଅକ୍ଷର **userp;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(Curl_creds_has_oauth_bearer(data->state.creds));
  userp = &data->req.hd_auth;
  curlx_free(*userp);
  *userp = curl_maprintf("Authorization: Bearer %s\r\n",
                         Curl_creds_oauth_bearer(data->state.creds));

  ଯଦି(!*userp) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ fail;
  }

fail:
  ଫେରନ୍ତୁ result;
}
#endif

#endif

/* pickoneauth() selects the most favorable authentication method from the
 * ones available and the ones we want.
 *
 * return TRUE if one was picked
 */
ସ୍ଥିର bool pickoneauth(ସଂରଚନା auth *pick, unsigned long mask,
                        ସଂରଚନା Curl_creds *creds)
{
  bool have_user_pass = Curl_creds_has_user_or_pass(creds);
  bool picked;
  /* only deal with authentication we want */
  unsigned long avail = pick->avail & pick->want & mask;
  picked = TRUE;

  /* The order of these checks is highly relevant, as this will be the order
     of preference in case of the existence of multiple accepted types. */
  ଯଦି(avail & CURLAUTH_NEGOTIATE)  /* available on empty creds */
    pick->picked = CURLAUTH_NEGOTIATE;
#ifndef CURL_DISABLE_BEARER_AUTH
  ଅନ୍ୟଥା ଯଦି((avail & CURLAUTH_BEARER) && Curl_creds_has_oauth_bearer(creds))
    pick->picked = CURLAUTH_BEARER;
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  ଅନ୍ୟଥା ଯଦି((avail & CURLAUTH_DIGEST) && have_user_pass)
    pick->picked = CURLAUTH_DIGEST;
#endif
  ଅନ୍ୟଥା ଯଦି(avail & CURLAUTH_NTLM)
    pick->picked = CURLAUTH_NTLM;
#ifndef CURL_DISABLE_BASIC_AUTH
  ଅନ୍ୟଥା ଯଦି((avail & CURLAUTH_BASIC) && have_user_pass)
    pick->picked = CURLAUTH_BASIC;
#endif
#ifndef CURL_DISABLE_AWS
  ଅନ୍ୟଥା ଯଦି(avail & CURLAUTH_AWS_SIGV4)
    pick->picked = CURLAUTH_AWS_SIGV4;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  ଅନ୍ୟଥା ଯଦି(avail & CURLAUTH_HTTPSIG)
    pick->picked = CURLAUTH_HTTPSIG;
#endif
  ଅନ୍ୟଥା {
    pick->picked = CURLAUTH_PICKNONE; /* we select to use nothing */
    picked = FALSE;
  }
  pick->avail = CURLAUTH_NONE; /* clear it here */

  ଫେରନ୍ତୁ picked;
}

/*
 * http_perhapsrewind()
 *
 * The current request needs to be done again - maybe due to a follow
 * or authentication negotiation. Check if:
 * 1) a rewind of the data sent to the server is necessary
 * 2) the current transfer should continue or be stopped early
 */
ସ୍ଥିର CURLcode http_perhapsrewind(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା connectdata *conn)
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
  VERBOSE(ଅଚଳ ଅକ୍ଷର *ongoing_auth = ଖାଲି);

  /* We need a rewind before uploading client read data again. The
   * checks below influence of the upload is to be continued
   * or aborted early.
   * This depends on how much remains to be sent and in what state
   * the authentication is. Some auth schemes such as NTLM do not work
   * for a new connection. */
  ଯଦି(needs_rewind) {
    infof(data, "Need to rewind upload for next request");
    Curl_creader_set_rewind(data, TRUE);
  }

  ଯଦି(conn->bits.close)
    /* If we already decided to close this connection, we cannot veto. */
    ଫେରନ୍ତୁ CURLE_OK;

  ଯଦି(abort_upload) {
    /* We would like to abort the upload - but should we? */
#ifdef USE_NTLM
    ଯଦି((data->state.authproxy.picked == CURLAUTH_NTLM) ||
       (data->state.authhost.picked == CURLAUTH_NTLM)) {
      VERBOSE(ongoing_auth = "NTLM");
      ଯଦି((conn->http_ntlm_state != NTLMSTATE_NONE) ||
         (conn->proxy_ntlm_state != NTLMSTATE_NONE)) {
        /* The NTLM-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#endif
#ifdef USE_SPNEGO
    /* There is still data left to send */
    ଯଦି((data->state.authproxy.picked == CURLAUTH_NEGOTIATE) ||
       (data->state.authhost.picked == CURLAUTH_NEGOTIATE)) {
      VERBOSE(ongoing_auth = "NEGOTIATE");
      ଯଦି((conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) ||
         (conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE)) {
        /* The NEGOTIATE-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#endif
  }

  ଯଦି(abort_upload) {
    ଯଦି(upload_remain >= 0)
      infof(data, "%s%sclose instead of sending %" FMT_OFF_T " more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "",
            upload_remain);
    ଅନ୍ୟଥା
      infof(data, "%s%sclose instead of sending unknown amount "
            "of more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "");
    /* We decided to abort the ongoing transfer */
    streamclose(conn);
    data->req.size = 0; /* do not download any more than 0 bytes */
    data->req.http_bodyless = TRUE;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/**
 * http_should_fail() determines whether an HTTP response code has gotten us
 * into an error state or not.
 *
 * @retval FALSE communications should continue
 *
 * @retval TRUE communications should not continue
 */
ସ୍ଥିର bool http_should_fail(ସଂରଚନା Curl_easy *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା httpcode)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  /*
   * If we have not been asked to fail on error,
   * do not fail.
   */
  ଯଦି(!data->set.http_fail_on_error)
    ଫେରନ୍ତୁ FALSE;

  /*
   * Any code < 400 is never terminal.
   */
  ଯଦି(httpcode < 400)
    ଫେରନ୍ତୁ FALSE;

  /*
   * A 416 response to a resume request is presumably because the file is
   * already completely downloaded and thus not actually a fail.
   */
  ଯଦି(data->state.resume_from && data->state.httpreq == HTTPREQ_GET &&
     httpcode == 416)
    ଫେରନ୍ତୁ FALSE;

  /*
   * Any code >= 400 that is not 401 or 407 is always
   * a terminal error
   */
  ଯଦି((httpcode != 401) && (httpcode != 407))
    ଫେରନ୍ତୁ TRUE;

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
  ଯଦି((httpcode == 401) && !data->state.creds)
    ଫେରନ୍ତୁ TRUE;
#ifndef CURL_DISABLE_PROXY
  ଯଦି((httpcode == 407) && !data->conn->http_proxy.creds)
    ଫେରନ୍ତୁ TRUE;
#endif

  ଫେରନ୍ତୁ (bool)data->state.authproblem;
}

/*
 * Curl_http_auth_act() gets called when all HTTP headers have been received
 * and it checks what authentication methods that are available and decides
 * which one (if any) to use. It will set 'newurl' if an auth method was
 * picked.
 */
CURLcode Curl_http_auth_act(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା connectdata *conn = data->conn;
  bool pickhost = FALSE;
  bool pickproxy = FALSE;
  CURLcode result = CURLE_OK;
  unsigned long authmask = ~0UL;

  ଯଦି(!Curl_creds_has_oauth_bearer(data->state.creds))
    authmask &= (unsigned long)~CURLAUTH_BEARER;

  ଯଦି(100 <= data->req.httpcode && data->req.httpcode <= 199)
    /* this is a transient response code, ignore */
    ଫେରନ୍ତୁ CURLE_OK;

  ଯଦି(data->state.authproblem)
    ଫେରନ୍ତୁ data->set.http_fail_on_error ? CURLE_HTTP_RETURNED_ERROR : CURLE_OK;

  ଯଦି(data->state.creds &&
     ((data->req.httpcode == 401) ||
      (data->req.authneg && data->req.httpcode < 300))) {
    pickhost = pickoneauth(&data->state.authhost, authmask, data->state.creds);
    ଯଦି(!pickhost)
      data->state.authproblem = TRUE;
    ଅନ୍ୟଥା
      data->info.httpauthpicked = data->state.authhost.picked;
    ଯଦି(data->state.authhost.picked == CURLAUTH_NTLM &&
       (data->req.httpversion_sent > 11)) {
      infof(data, "Forcing HTTP/1.1 for NTLM");
      connclose(conn);
      data->state.http_neg.wanted = CURL_HTTP_V1x;
      data->state.http_neg.allowed = CURL_HTTP_V1x;
    }
  }
#ifndef CURL_DISABLE_PROXY
  ଯଦି(conn->http_proxy.creds &&
     ((data->req.httpcode == 407) ||
      (data->req.authneg && data->req.httpcode < 300))) {
    pickproxy = pickoneauth(&data->state.authproxy,
                            authmask & ~CURLAUTH_BEARER,
                            conn->http_proxy.creds);
    ଯଦି(!pickproxy)
      data->state.authproblem = TRUE;
    ଅନ୍ୟଥା
      data->info.proxyauthpicked = data->state.authproxy.picked;
  }
#endif

  ଯଦି(pickhost || pickproxy) {
    result = http_perhapsrewind(data, conn);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;

    /* In case this is GSS auth, the newurl field is already allocated so
       we must make sure to free it before allocating a new one. As figured
       out in bug #2284386 */
    curlx_free(data->req.newurl);
    /* clone URL */
    data->req.newurl = Curl_bufref_dup(&data->state.url);
    ଯଦି(!data->req.newurl)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }
  ଅନ୍ୟଥା ଯଦି((data->req.httpcode < 300) &&
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
    ଯଦି(!data->req.newurl)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    data->state.authhost.done = TRUE;
  }
  ଯଦି(http_should_fail(data, data->req.httpcode)) {
    failf(data, "The requested URL returned error: %d",
          data->req.httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
  }

  ଫେରନ୍ତୁ result;
}

#ifndef CURL_DISABLE_HTTP_AUTH
/*
 * Output the correct authentication header depending on the auth type
 * and whether or not it is to a proxy.
 */
ସ୍ଥିର CURLcode output_auth_headers(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା connectdata *conn,
                                    ସଂରଚନା auth *authstatus,
                                    ଅଚଳ ଅକ୍ଷର *request,
                                    ଅଚଳ ଅକ୍ଷର *path,
                                    bool proxy)
{
  ଅଚଳ ଅକ୍ଷର *auth = ଖାଲି;
  CURLcode result = CURLE_OK;
  (ଶୂନ୍ୟ)conn;

#ifdef CURL_DISABLE_DIGEST_AUTH
  (ଶୂନ୍ୟ)request;
  (ଶୂନ୍ୟ)path;
#endif
#ifndef CURL_DISABLE_AWS
  ଯଦି((authstatus->picked == CURLAUTH_AWS_SIGV4) && !proxy) {
    /* this method is never for proxy */
    auth = "AWS_SIGV4";
    result = Curl_output_aws_sigv4(data);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଅନ୍ୟଥା
#endif
#ifndef CURL_DISABLE_HTTPSIG
  ଯଦି((authstatus->picked == CURLAUTH_HTTPSIG) && !proxy) {
    /* HTTPSIG uses its own configured key material rather than
       data->state.creds. Do not let unrelated credentials from a
       redirected URL bypass the cross-host auth boundary. */
    ଯଦି(Curl_auth_allowed_to_host(data)) {
      auth = "HTTPSIG";
      result = Curl_output_httpsig(data);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
    ଅନ୍ୟଥା
      authstatus->done = TRUE;
  }
  ଅନ୍ୟଥା
#endif
#ifdef USE_SPNEGO
  ଯଦି(authstatus->picked == CURLAUTH_NEGOTIATE) {
    ଯଦି(
#ifndef CURL_DISABLE_PROXY
      (proxy && !Curl_checkProxyheaders(data, conn,
                                        STRCONST("Proxy-authorization"))) ||
#endif
      (!proxy && !Curl_checkheaders(data, STRCONST("Authorization")) &&
       Curl_auth_allowed_to_host(data))) {
      auth = "Negotiate";
      result = Curl_output_negotiate(data, conn, proxy);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
    ଅନ୍ୟଥା
      authstatus->done = TRUE;
  }
  ଅନ୍ୟଥା
#endif
#ifdef USE_NTLM
  ଯଦି(authstatus->picked == CURLAUTH_NTLM) {
    ଯଦି(proxy || Curl_auth_allowed_to_host(data)) {
      auth = "NTLM";
      result = Curl_output_ntlm(data, proxy);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
    ଅନ୍ୟଥା
      authstatus->done = TRUE;
  }
  ଅନ୍ୟଥା
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  ଯଦି(authstatus->picked == CURLAUTH_DIGEST) {
    auth = "Digest";
    result = Curl_output_digest(data,
                                proxy,
                                (ଅଚଳ unsigned ଅକ୍ଷର *)request,
                                (ଅଚଳ unsigned ଅକ୍ଷର *)path);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଅନ୍ୟଥା
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
  ଯଦି(authstatus->picked == CURLAUTH_BASIC) {
    /* Basic */
    ଯଦି(
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
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
  ଯଦି(authstatus->picked == CURLAUTH_BEARER) {
    /* Bearer */
    ଯଦି(!proxy && Curl_creds_has_oauth_bearer(data->state.creds) &&
       !Curl_checkheaders(data, STRCONST("Authorization"))) {
      auth = "Bearer";
      result = http_output_bearer(data);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif

  ଯଦି(auth) {
#ifndef CURL_DISABLE_PROXY
    ଯଦି(proxy)
      data->info.proxyauthpicked = authstatus->picked;
    ଅନ୍ୟଥା
      data->info.httpauthpicked = authstatus->picked;
    infof(data, "%s auth using %s with user '%s'",
          proxy ? "Proxy" : "Server", auth,
          proxy ? (conn->http_proxy.creds ?
                   conn->http_proxy.creds->user : "") :
          (data->state.creds ?
           data->state.creds->user : ""));
#ଅନ୍ୟଥା
    (ଶୂନ୍ୟ)proxy;
    infof(data, "Server auth using %s with user '%s'",
          auth, data->state.creds ?
          data->state.creds->user : "");
#endif
    authstatus->multipass = !authstatus->done;
  }
  ଅନ୍ୟଥା {
    authstatus->multipass = FALSE;
    ଯଦି(proxy)
      data->info.proxyauthpicked = 0;
    ଅନ୍ୟଥା
      data->info.httpauthpicked = 0;
  }

  ଫେରନ୍ତୁ result;
}

CURLcode Curl_http_output_auth(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା connectdata *conn,
                               ଅଚଳ ଅକ୍ଷର *request,
                               Curl_HttpReq httpreq,
                               ଅଚଳ ଅକ୍ଷର *path,
                               ଅଚଳ ଅକ୍ଷର *query,
                               bool is_connect)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା auth *authhost;
  ସଂରଚନା auth *authproxy;
  ଅଚଳ ଅକ୍ଷର *path_and_query = path;
  ଅକ୍ଷର *tmp_str = ଖାଲି;

  DEBUGASSERT(data);
  authhost = &data->state.authhost;
  authproxy = &data->state.authproxy;

  ଯଦି(
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
    ଯାଅ out;
  }

  ଯଦି(query) {
    tmp_str = curl_maprintf("%s?%s", path, query);
    ଯଦି(!tmp_str) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }
    path_and_query = tmp_str;
  }

  ଯଦି(authhost->want && !authhost->picked)
    /* The app has selected one or more methods, but none has been picked
       so far by a server round-trip. Then we set the picked one to the
       want one, and if this is one single bit it will be used instantly. */
    authhost->picked = authhost->want;

  ଯଦି(authproxy->want && !authproxy->picked)
    /* The app has selected one or more methods, but none has been picked so
       far by a proxy round-trip. Then we set the picked one to the want one,
       and if this is one single bit it will be used instantly. */
    authproxy->picked = authproxy->want;

#ifndef CURL_DISABLE_PROXY
  /* Send proxy authentication header if needed */
  ଯଦି(conn->bits.origin_is_proxy || is_connect) {
    result = output_auth_headers(data, conn, authproxy, request,
                                 path_and_query, TRUE);
    ଯଦି(result)
      ଯାଅ out;
  }
  ଅନ୍ୟଥା
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)is_connect;
#endif /* CURL_DISABLE_PROXY */
    /* we have no proxy so let's pretend we are done authenticating
       with it */
    authproxy->done = TRUE;

  /* Either we have credentials for the origin we talk to or
     performing authentication is allowed here */
  ଯଦି(data->state.creds || Curl_auth_allowed_to_host(data))
    result = output_auth_headers(data, conn, authhost, request,
                                 path_and_query, FALSE);
  ଅନ୍ୟଥା
    authhost->done = TRUE;

  ଯଦି(((authhost->multipass && !authhost->done) ||
      (authproxy->multipass && !authproxy->done)) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_HEAD)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    data->req.authneg = TRUE;
  }
  ଅନ୍ୟଥା
    data->req.authneg = FALSE;

out:
  curlx_free(tmp_str);
  ଫେରନ୍ତୁ result;
}

#ଅନ୍ୟଥା /* !CURL_DISABLE_HTTP_AUTH */
/* when disabled */
CURLcode Curl_http_output_auth(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା connectdata *conn,
                               ଅଚଳ ଅକ୍ଷର *request,
                               Curl_HttpReq httpreq,
                               ଅଚଳ ଅକ୍ଷର *path,
                               ଅଚଳ ଅକ୍ଷର *query,
                               bool is_connect)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)conn;
  (ଶୂନ୍ୟ)request;
  (ଶୂନ୍ୟ)httpreq;
  (ଶୂନ୍ୟ)path;
  (ଶୂନ୍ୟ)query;
  (ଶୂନ୍ୟ)is_connect;
  ଫେରନ୍ତୁ CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP_AUTH, else */

#ଯଦି defined(USE_SPNEGO) || defined(USE_NTLM) || \
  !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_BASIC_AUTH) || \
  !defined(CURL_DISABLE_BEARER_AUTH)
ସ୍ଥିର bool authcmp(ଅଚଳ ଅକ୍ଷର *auth, ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି)
{
  /* the auth string must not have an alnum following */
  size_t n = strlen(auth);
  ଫେରନ୍ତୁ curl_strnequal(auth, ଧାଡ଼ି, n) && !ISALNUM(ଧାଡ଼ି[n]);
}
#endif

#ifdef USE_SPNEGO
ସ୍ଥିର CURLcode auth_spnego(ସଂରଚନା Curl_easy *data,
                            bool proxy,
                            ଅଚଳ ଅକ୍ଷର *auth,
                            ସଂରଚନା auth *authp,
                            uint32_t *availp)
{
  ଯଦି((authp->avail & CURLAUTH_NEGOTIATE) || Curl_auth_is_spnego_supported()) {
    *availp |= CURLAUTH_NEGOTIATE;
    authp->avail |= CURLAUTH_NEGOTIATE;

    ଯଦି(authp->picked == CURLAUTH_NEGOTIATE) {
      ସଂରଚନା connectdata *conn = data->conn;
      CURLcode result = Curl_ନିବେଶ_negotiate(data, conn, proxy, auth);
      uint8_t *negstate = proxy ? &conn->proxy_negotiate_state :
        &conn->http_negotiate_state;
      ଯଦି(!result) {
        curlx_free(data->req.newurl);
        data->req.newurl = Curl_bufref_dup(&data->state.url);
        ଯଦି(!data->req.newurl)
          ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
        data->state.authproblem = FALSE;
        /* we received a GSS auth token and we dealt with it fine */
        *negstate = CURL_NEGO_AUTH_RECV;
      }
      ଅନ୍ୟଥା
        data->state.authproblem = TRUE;
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}
#endif

#ifdef USE_NTLM
ସ୍ଥିର CURLcode auth_ntlm(ସଂରଚନା Curl_easy *data,
                          bool proxy,
                          ଅଚଳ ଅକ୍ଷର *auth,
                          ସଂରଚନା auth *authp,
                          uint32_t *availp)
{
  /* NTLM support requires the SSL crypto libs */
  ଯଦି((authp->avail & CURLAUTH_NTLM) || Curl_auth_is_ntlm_supported()) {
    *availp |= CURLAUTH_NTLM;
    authp->avail |= CURLAUTH_NTLM;

    ଯଦି(authp->picked == CURLAUTH_NTLM) {
      /* NTLM authentication is picked and activated */
      CURLcode result = Curl_ନିବେଶ_ntlm(data, proxy, auth);
      ଯଦି(!result)
        data->state.authproblem = FALSE;
      ଅନ୍ୟଥା {
        ଯଦି(result == CURLE_OUT_OF_MEMORY)
          ଫେରନ୍ତୁ result;
        infof(data, "NTLM authentication problem, ignoring.");
        data->state.authproblem = TRUE;
      }
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_DIGEST_AUTH
ସ୍ଥିର CURLcode auth_digest(ସଂରଚନା Curl_easy *data,
                            bool proxy,
                            ଅଚଳ ଅକ୍ଷର *auth,
                            ସଂରଚନା auth *authp,
                            uint32_t *availp)
{
  ଯଦି(authp->avail & CURLAUTH_DIGEST) {
    *availp |= CURLAUTH_DIGEST;
    infof(data, "Ignoring duplicate digest auth header.");
  }
  ଅନ୍ୟଥା ଯଦି(Curl_auth_is_digest_supported()) {
    CURLcode result;

    *availp |= CURLAUTH_DIGEST;
    authp->avail |= CURLAUTH_DIGEST;

    /* We call this function on input Digest headers even if Digest
     * authentication is not activated yet, as we need to store the
     * incoming data from this header in case we are going to use
     * Digest */
    result = Curl_ନିବେଶ_digest(data, proxy, auth);
    ଯଦି(result) {
      ଯଦି(result == CURLE_OUT_OF_MEMORY)
        ଫେରନ୍ତୁ result;
      infof(data, "Digest authentication problem, ignoring.");
      data->state.authproblem = TRUE;
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BASIC_AUTH
ସ୍ଥିର CURLcode auth_basic(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା auth *authp,
                           uint32_t *availp)
{
  *availp |= CURLAUTH_BASIC;
  authp->avail |= CURLAUTH_BASIC;
  ଯଦି(authp->picked == CURLAUTH_BASIC) {
    /* We asked for Basic authentication but got a 40X back anyway, which
       means our name+password is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(data, "Basic authentication problem, ignoring.");
    data->state.authproblem = TRUE;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BEARER_AUTH
ସ୍ଥିର CURLcode auth_bearer(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା auth *authp,
                            uint32_t *availp)
{
  *availp |= CURLAUTH_BEARER;
  authp->avail |= CURLAUTH_BEARER;
  ଯଦି(authp->picked == CURLAUTH_BEARER) {
    /* We asked for Bearer authentication but got a 40X back anyway, which
       means our token is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(data, "Bearer authentication problem, ignoring.");
    data->state.authproblem = TRUE;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}
#endif

/*
 * Curl_http_input_auth() deals with Proxy-Authenticate: and WWW-Authenticate:
 * headers. They are dealt with both in the transfer.c main loop and in the
 * proxy CONNECT loop.
 *
 * The 'auth' line ends with a NUL byte without CR or LF present.
 */
CURLcode Curl_http_ନିବେଶ_auth(ସଂରଚନା Curl_easy *data, bool proxy,
                              ଅଚଳ ଅକ୍ଷର *auth) /* the first non-space */
{
  /*
   * This resource requires authentication
   */
#ଯଦି defined(USE_SPNEGO) ||                      \
  defined(USE_NTLM) ||                          \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||         \
  !defined(CURL_DISABLE_BASIC_AUTH) ||          \
  !defined(CURL_DISABLE_BEARER_AUTH)

  uint32_t *availp;
  ସଂରଚନା auth *authp;
  CURLcode result = CURLE_OK;
  DEBUGASSERT(auth);
  DEBUGASSERT(data);

  ଯଦି(proxy) {
    availp = &data->info.proxyauthavail;
    authp = &data->state.authproxy;
  }
  ଅନ୍ୟଥା {
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

  ଯେତେବେଳେ(*auth) {
#ifdef USE_SPNEGO
    ଯଦି(authcmp("Negotiate", auth))
      result = auth_spnego(data, proxy, auth, authp, availp);
#endif
#ifdef USE_NTLM
    ଯଦି(!result && authcmp("NTLM", auth))
      result = auth_ntlm(data, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
    ଯଦି(!result && authcmp("Digest", auth))
      result = auth_digest(data, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
    ଯଦି(!result && authcmp("Basic", auth))
      result = auth_basic(data, authp, availp);
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
    ଯଦି(authcmp("Bearer", auth))
      result = auth_bearer(data, authp, availp);
#endif

    ଯଦି(result)
      ଭାଙ୍ଗ;

    /* there may be multiple methods on one line, so keep reading */
    auth = strchr(auth, ',');
    ଯଦି(auth) /* if we are on a comma, skip it */
      auth++;
    ଅନ୍ୟଥା
      ଭାଙ୍ଗ;
    curlx_str_passblanks(&auth);
  }
  ଫେରନ୍ତୁ result;
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)proxy;
  (ଶୂନ୍ୟ)auth;
  /* nothing to do when disabled */
  ଫେରନ୍ତୁ CURLE_OK;
#endif
}

ସ୍ଥିର ଶୂନ୍ୟ http_switch_to_get(ସଂରଚନା Curl_easy *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
  ଅଚଳ ଅକ୍ଷର *req = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  ଯଦି((req || data->state.httpreq != HTTPREQ_GET) &&
     (data->set.http_follow_mode == CURLFOLLOW_OBEYCODE)) {
    NOVERBOSE((ଶୂନ୍ୟ)code);
    infof(data, "Switch to GET because of %d response", code);
    data->state.http_ignorecustom = TRUE;
  }
  ଅନ୍ୟଥା ଯଦି(req && (data->set.http_follow_mode != CURLFOLLOW_FIRSTONLY))
    infof(data, "Stick to %s instead of GET", req);

  data->state.httpreq = HTTPREQ_GET;
  Curl_creader_set_rewind(data, FALSE);
}

#define HTTPREQ_IS_POST(data)                    \
  ((data)->state.httpreq == HTTPREQ_POST ||      \
   (data)->state.httpreq == HTTPREQ_POST_FORM || \
   (data)->state.httpreq == HTTPREQ_POST_MIME)

ସ୍ଥିର CURLcode set_auto_referer(ସଂରଚନା Curl_easy *data)
{
  CURLU *u;
  ଅକ୍ଷର *referer = ଖାଲି;
  CURLUcode uc;

  /* We are asked to automatically set the previous URL as the referer
     when we get the next URL. We pick the ->url field, which may or may
     not be 100% correct */
  Curl_bufref_free(&data->state.referer);

  /* Make a copy of the URL without credentials and fragment */
  u = curl_url();
  ଯଦି(!u)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  uc = curl_url_set(u, CURLUPART_URL,
                    Curl_bufref_ptr(&data->state.url), 0);
  ଯଦି(!uc)
    uc = curl_url_set(u, CURLUPART_FRAGMENT, ଖାଲି, 0);
  ଯଦି(!uc)
    uc = curl_url_set(u, CURLUPART_USER, ଖାଲି, 0);
  ଯଦି(!uc)
    uc = curl_url_set(u, CURLUPART_PASSWORD, ଖାଲି, 0);
  ଯଦି(!uc)
    uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

  curl_url_cleanup(u);

  ଯଦି(uc || !referer)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  Curl_bufref_set(&data->state.referer, referer, 0, curl_free);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode follow_parse_url(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *newurl,
                                 followtype type, ଅକ୍ଷର **follow_urlp,
                                 CURLUcode *ucp)
{
  CURLUcode uc;

  DEBUGASSERT(data->state.uh);
  uc = curl_url_set(data->state.uh, CURLUPART_URL, newurl, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
                    ((type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                     ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) |
                     CURLU_ALLOW_SPACE |
                     (data->set.path_as_is ? CURLU_PATH_AS_IS : 0)));
  *ucp = uc;
  ଯଦି(uc) {
    ଯଦି((uc == CURLUE_OUT_OF_MEMORY) || (type != FOLLOW_FAKE)) {
      failf(data, "The redirect target URL could not be parsed: %s",
            curl_url_strerror(uc));
      ଫେରନ୍ତୁ Curl_uc_to_curlcode(uc);
    }

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, duplicate the field as-is */
    *follow_urlp = curlx_strdup(newurl);
    ଯଦି(!*follow_urlp)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }
  ଅନ୍ୟଥା {
    CURLU *u = curl_url();
    ଯଦି(!u)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    uc = curl_url_set(u, CURLUPART_URL,
                      Curl_bufref_ptr(&data->state.url),
                      CURLU_URLENCODE | CURLU_ALLOW_SPACE);
    ଯଦି(!uc)
      uc = curl_url_get(data->state.uh, CURLUPART_URL, follow_urlp, 0);
    ଯଦି(uc) {
      curl_url_cleanup(u);
      ଫେରନ୍ତୁ Curl_uc_to_curlcode(uc);
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    {
      bool same_origin = Curl_url_same_origin(u, data->state.uh);
      curl_url_cleanup(u);
      ଯଦି(!same_origin)
        Curl_auth_digest_cleanup(&data->state.digest);
    }
#ଅନ୍ୟଥା
    curl_url_cleanup(u);
#endif
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode follow_fake(ସଂରଚନା Curl_easy *data, ଅକ୍ଷର *follow_url,
                            CURLUcode uc, bool reachedmax)
{
  /* we are only figuring out the new URL if we would have followed locations
     but now we are done so we can get out! */
  ଯଦି(!uc) {
    CURLU *u = curl_url();
    ଅକ୍ଷର *nocred = ଖାଲି;

    ଯଦି(!u) {
      curlx_free(follow_url);
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
    ଯଦି(!curl_url_set(u, CURLUPART_URL, follow_url,
                     CURLU_NON_SUPPORT_SCHEME |
                     (data->set.path_as_is ? CURLU_PATH_AS_IS : 0)) &&
       !curl_url_set(u, CURLUPART_USER, ଖାଲି, 0) &&
       !curl_url_set(u, CURLUPART_PASSWORD, ଖାଲି, 0))
      (ଶୂନ୍ୟ)curl_url_get(u, CURLUPART_URL, &nocred, CURLU_GET_EMPTY);
    curl_url_cleanup(u);
    ଯଦି(nocred) {
      curlx_free(follow_url);
      follow_url = nocred;
    }
  }
  data->info.wouldredirect = follow_url;

  ଯଦି(reachedmax) {
    failf(data, "Maximum (%d) redirects followed", data->set.maxredirs);
    ଫେରନ୍ତୁ CURLE_TOO_MANY_REDIRECTS;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର bool follow_switch_get(ସଂରଚନା Curl_easy *data)
{
  /*
   * We get here when the HTTP code is 300-399 and 401 or 407. We need to
   * perform differently based on exactly what return code there was.
   */
  ବାଛ(data->info.httpcode) {
  ସ୍ଥିତି 301: /* Moved Permanently */
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
    ଯଦି(HTTPREQ_IS_POST(data) && !data->set.post301) {
      http_switch_to_get(data, 301);
      ଫେରନ୍ତୁ TRUE;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 302: /* Found */
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
    ଯଦି(HTTPREQ_IS_POST(data) && !data->set.post302) {
      http_switch_to_get(data, 302);
      ଫେରନ୍ତୁ TRUE;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     */
    ଯଦି(!HTTPREQ_IS_POST(data) || !data->set.post303) {
      http_switch_to_get(data, 303);
      ଫେରନ୍ତୁ TRUE;
    }
    ଭାଙ୍ଗ;

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
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ FALSE;
}

CURLcode Curl_http_follow(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *newurl,
                          followtype type)
{
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  ଅକ୍ଷର *follow_url = ଖାଲି;
  CURLUcode uc;
  CURLcode result;
  CURLcode rewind_result;
  bool switch_to_get;

  DEBUGASSERT(type != FOLLOW_NONE);

  ଯଦି(type != FOLLOW_FAKE)
    data->state.requests++; /* count all real follows */
  ଯଦି(type == FOLLOW_REDIR) {
    ଯଦି((data->set.maxredirs != -1) &&
       (data->state.followlocation >= data->set.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    ଅନ୍ୟଥା {
      data->state.followlocation++; /* count redirect-followings, including
                                       auth reloads */

      ଯଦି(data->set.http_auto_referer) {
        result = set_auto_referer(data);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;
      }
    }
  }

  ଯଦି((type != FOLLOW_RETRY) &&
     (data->req.httpcode != 401) && (data->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, ଖାଲି, 0, FALSE)) {
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: do not allow a custom port number */
    disallowport = TRUE;
  }

  result = follow_parse_url(data, newurl, type, &follow_url, &uc);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  DEBUGASSERT(follow_url);

  ଯଦି(type == FOLLOW_FAKE)
    ଫେରନ୍ତୁ follow_fake(data, follow_url, uc, reachedmax);

  ଯଦି(disallowport)
    data->state.allow_port = FALSE;

  Curl_bufref_set(&data->state.url, follow_url, 0, curl_free);
  rewind_result = Curl_req_soft_reset(&data->req, data);
  infof(data, "Issue another request to this URL: '%s'", follow_url);
  ଯଦି((data->set.http_follow_mode == CURLFOLLOW_FIRSTONLY) &&
     !data->state.http_ignorecustom &&
     CURL_EASY_STR(data, STRING_CUSTOMREQUEST)) {
    data->state.http_ignorecustom = TRUE;
    infof(data, "Drop custom request method for next request");
  }

  switch_to_get = follow_switch_get(data);

  /* When rewind of upload data failed and we are not switching to GET,
   * we need to fail the follow, as we cannot send the data again. */
  ଯଦି(rewind_result && !switch_to_get)
    ଫେରନ୍ତୁ rewind_result;

  Curl_pgrsTime(data, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(data);

  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Curl_compareheader()
 *
 * Returns TRUE if 'headerline' contains the 'header' with given 'content'
 * (within a comma-separated list of tokens). Pass 'header' WITH the colon.
 *
 * @unittest: 1625
 */
bool Curl_compareheader(ଅଚଳ ଅକ୍ଷର *headerline, /* line to check */
                        ଅଚଳ ଅକ୍ଷର *header, /* header keyword _with_ colon */
                        ଅଚଳ size_t hlen, /* len of the keyword in bytes */
                        ଅଚଳ ଅକ୍ଷର *content, /* content string to find */
                        ଅଚଳ size_t clen) /* len of the content in bytes */
{
  /* RFC2616, section 4.2 says: "Each header field consists of a name followed
   * by a colon (":") and the field value. Field names are case-insensitive.
   * The field value MAY be preceded by any amount of LWS, though a single SP
   * is preferred." */

  ଅଚଳ ଅକ୍ଷର *p;
  ସଂରଚନା Curl_str val;
  DEBUGASSERT(hlen);
  DEBUGASSERT(clen);
  DEBUGASSERT(header);
  DEBUGASSERT(content);

  ଯଦି(!curl_strnequal(headerline, header, hlen))
    ଫେରନ୍ତୁ FALSE; /* does not start with header */

  /* pass the header */
  p = &headerline[hlen];

  ଯଦି(curlx_str_cspn(&p, &val, "\r\n"))
    ଫେରନ୍ତୁ FALSE;
  curlx_str_trimblanks(&val);

  /* find the content string in the rest of the line */
  ଯଦି(curlx_strlen(&val) >= clen) {
    size_t len;
    p = curlx_str(&val);
    ପାଇଁ(len = curlx_strlen(&val); len >= clen;) {
      ସଂରଚନା Curl_str next;
      ଅଚଳ ଅକ୍ଷର *o = p;
      /* after a match there must be a comma, space, newline or NUL byte */
      ଯଦି(curl_strnequal(p, content, clen) &&
         ((p[clen] == ',') || ISBLANK(p[clen]) || ISNEWLINE(p[clen]) ||
          !p[clen]))
        ଫେରନ୍ତୁ TRUE; /* match! */
      /* advance to the next comma */
      ଯଦି(curlx_str_until(&p, &next, len, ',') ||
         curlx_str_single(&p, ','))
        ଭାଙ୍ଗ; /* no comma, get out */

      /* if there are more dummy commas, move over them as well */
      କର
        curlx_str_passblanks(&p);
      ଯେତେବେଳେ(!curlx_str_single(&p, ','));
      /* trailing blanks may move the parsing point past the value end,
         then there is nothing left to match */
      ଯଦି((size_t)(p - o) > len)
        ଭାଙ୍ଗ;
      len -= (p - o);
    }
  }
  ଫେରନ୍ତୁ FALSE; /* no match */
}

ସଂରଚନା cr_exp100_ctx {
  ସଂରଚନା Curl_creader super;
  ସଂରଚନା curltime start; /* time started waiting */
  ଗଣନା expect100 state;
};

/* Expect: 100-continue client reader, blocking uploads */

ସ୍ଥିର ଶୂନ୍ୟ http_exp100_continue(ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା Curl_creader *reader)
{
  ସଂରଚନା cr_exp100_ctx *ctx = reader->ctx;
  ଯଦି(ctx->state > EXP100_SEND_DATA) {
    ctx->state = EXP100_SEND_DATA;
    Curl_expire_clear(data, EXPIRE_100_TIMEOUT);
  }
}

ସ୍ଥିର CURLcode cr_exp100_read(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_creader *reader,
                               ଅକ୍ଷର *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  ସଂରଚନା cr_exp100_ctx *ctx = reader->ctx;
  timediff_t ms;

  ବାଛ(ctx->state) {
  ସ୍ଥିତି EXP100_SENDING_REQUEST:
    ଯଦି(!Curl_req_sendbuf_empty(data)) {
      /* The initial request data has not been fully sent yet. Do
       * not start the timer yet. */
      DEBUGF(infof(data, "cr_exp100_read, request not full sent yet"));
      *nread = 0;
      *eos = FALSE;
      ଫେରନ୍ତୁ CURLE_OK;
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
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି EXP100_FAILED:
    DEBUGF(infof(data, "cr_exp100_read, expectation failed, error"));
    *nread = 0;
    *eos = FALSE;
    ଫେରନ୍ତୁ CURLE_READ_ERROR;
  ସ୍ଥିତି EXP100_AWAITING_CONTINUE:
    ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->start);
    ଯଦି(ms < data->set.expect_100_timeout) {
      DEBUGF(infof(data, "cr_exp100_read, AWAITING_CONTINUE, not expired"));
      *nread = 0;
      *eos = FALSE;
      ଫେରନ୍ତୁ CURLE_OK;
    }
    /* we have waited long enough, continue anyway */
    http_exp100_continue(data, reader);
    infof(data, "Done waiting for 100-continue");
    FALLTHROUGH();
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    DEBUGF(infof(data, "cr_exp100_read, pass through"));
    ଫେରନ୍ତୁ Curl_creader_read(data, reader->next, buf, blen, nread, eos);
  }
}

ସ୍ଥିର ଶୂନ୍ୟ cr_exp100_done(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା Curl_creader *reader, ପୂର୍ଣ୍ଣସଂଖ୍ୟା premature)
{
  ସଂରଚନା cr_exp100_ctx *ctx = reader->ctx;
  ctx->state = premature ? EXP100_FAILED : EXP100_SEND_DATA;
  Curl_expire_clear(data, EXPIRE_100_TIMEOUT);
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_crtype cr_exp100 = {
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
  ଆକାର(ସଂରଚନା cr_exp100_ctx)
};

ସ୍ଥିର CURLcode http_exp100_add_reader(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_creader *reader = ଖାଲି;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_exp100, CURL_CR_PROTOCOL);
  ଯଦି(!result)
    result = Curl_creader_add(data, reader);
  ଯଦି(!result) {
    ସଂରଚନା cr_exp100_ctx *ctx = reader->ctx;
    ctx->state = EXP100_SENDING_REQUEST;
  }

  ଯଦି(result && reader)
    Curl_creader_free(data, reader);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ http_exp100_got100(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  ଯଦି(r)
    http_exp100_continue(data, r);
}

ସ୍ଥିର bool http_exp100_is_waiting(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  ଯଦି(r) {
    ସଂରଚନା cr_exp100_ctx *ctx = r->ctx;
    ଫେରନ୍ତୁ ctx->state == EXP100_AWAITING_CONTINUE;
  }
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର ଶୂନ୍ୟ http_exp100_send_anyway(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  ଯଦି(r)
    http_exp100_continue(data, r);
}

ସ୍ଥିର bool http_exp100_is_selected(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  ଫେରନ୍ତୁ !!r;
}

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
CURLcode Curl_http_doing_pollset(ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା easy_pollset *ps)
{
  /* write mode */
  ଫେରନ୍ତୁ Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

CURLcode Curl_http_perform_pollset(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା easy_pollset *ps)
{
  ସଂରଚନା connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  ଯଦି(CURL_REQ_WANT_RECV(data)) {
    result = Curl_pollset_add_in(data, ps, conn->sock[FIRSTSOCKET]);
  }

  /* on a "Expect: 100-continue" timed wait, do not poll for outgoing */
  ଯଦି(!result && Curl_req_want_send(data) && !http_exp100_is_waiting(data)) {
    result = Curl_pollset_add_out(data, ps, conn->sock[FIRSTSOCKET]);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode http_write_header(ସଂରଚନା Curl_easy *data,
                                  ଅଚଳ ଅକ୍ଷର *hd, size_t hdlen)
{
  CURLcode result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା writetype;

  /* now, only output this if the header AND body are requested:
   */
  Curl_debug(data, CURLINFO_HEADER_IN, hd, hdlen);

  writetype = CLIENTWRITE_HEADER |
    ((data->req.httpcode / 100 == 1) ? CLIENTWRITE_1XX : 0);

  result = Curl_client_write(data, writetype, hd, hdlen);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  result = Curl_bump_headersize(data, hdlen, FALSE);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  data->req.deductheadercount = (100 <= data->req.httpcode &&
                                 199 >= data->req.httpcode) ?
    data->req.headerbytecount : 0;
  ଫେରନ୍ତୁ result;
}

/*
 * Curl_http_done() gets called after a single HTTP request has been
 * performed.
 */

CURLcode Curl_http_done(ସଂରଚନା Curl_easy *data,
                        CURLcode status, bool premature)
{
  ସଂରଚନା connectdata *conn = data->conn;

  /* Clear multipass flag. If authentication is not done yet, then it will get
   * a chance to be set back to true when we output the next auth header */
  data->state.authhost.multipass = FALSE;
  data->state.authproxy.multipass = FALSE;

  ଯଦି(curlx_dyn_len(&data->state.headerb)) {
    (ଶୂନ୍ୟ)http_write_header(data, curlx_dyn_ptr(&data->state.headerb),
                            curlx_dyn_len(&data->state.headerb));
  }
  curlx_dyn_reset(&data->state.headerb);

  ଯଦି(status)
    ଫେରନ୍ତୁ status;

  ଯଦି(!premature && /* this check is pointless when DONE is called before the
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
    ଫେରନ୍ତୁ CURLE_GOT_NOTHING;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

/* Determine if we may use HTTP 1.1 for this request. */
ସ୍ଥିର bool http_may_use_1_1(ଅଚଳ ସଂରଚନା Curl_easy *data)
{
  ଅଚଳ ସଂରଚନା connectdata *conn = data->conn;
  /* We have seen a previous response for *this* transfer with 1.0,
   * on another connection or the same one. */
  ଯଦି(data->state.http_neg.rcvd_min == 10)
    ଫେରନ୍ତୁ FALSE;
  /* We have seen a previous response on *this* connection with 1.0. */
  ଯଦି(conn && conn->httpversion_seen == 10)
    ଫେରନ୍ତୁ FALSE;
  /* We want 1.0 and have seen no previous response on *this* connection
     with a higher version (maybe no response at all yet). */
  ଯଦି(data->state.http_neg.only_10 &&
     (!conn || conn->httpversion_seen <= 10))
    ଫେରନ୍ତୁ FALSE;
  /* We are not restricted to use 1.0 only. */
  ଫେରନ୍ତୁ !data->state.http_neg.only_10;
}

ସ୍ଥିର unsigned ଅକ୍ଷର http_request_version(ସଂରଚନା Curl_easy *data)
{
  unsigned ଅକ୍ଷର v = Curl_conn_http_version(data, data->conn);
  ଯଦି(!v) {
    /* No specific HTTP connection filter installed. */
    v = http_may_use_1_1(data) ? 11 : 10;
  }
  ଫେରନ୍ତୁ v;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *get_http_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା httpversion)
{
  ବାଛ(httpversion) {
  ସ୍ଥିତି 30:
    ଫେରନ୍ତୁ "3";
  ସ୍ଥିତି 20:
    ଫେରନ୍ତୁ "2";
  ସ୍ଥିତି 11:
    ଫେରନ୍ତୁ "1.1";
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ "1.0";
  }
}

CURLcode Curl_add_custom_headers(ସଂରଚନା Curl_easy *data,
                                 bool is_connect, ପୂର୍ଣ୍ଣସଂଖ୍ୟା httpversion,
                                 ସଂରଚନା dynbuf *req)
{
  ସଂରଚନା curl_slist *h[2];
  ସଂରଚନା curl_slist *headers;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା numlists = 1; /* by default */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

#ifndef CURL_DISABLE_PROXY
  ଗଣନା Curl_proxy_use proxy;

  ଯଦି(is_connect)
    proxy = HEADER_CONNECT;
  ଅନ୍ୟଥା
    proxy = data->conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  ବାଛ(proxy) {
  ସ୍ଥିତି HEADER_SERVER:
    h[0] = data->set.headers;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି HEADER_PROXY:
    h[0] = data->set.headers;
    ଯଦି(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି HEADER_CONNECT:
    ଯଦି(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    ଅନ୍ୟଥା
      h[0] = data->set.headers;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି HEADER_CONNECT_UDP:
    ଯଦି(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    ଅନ୍ୟଥା
      h[0] = data->set.headers;
    ଭାଙ୍ଗ;
  }
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)is_connect;
  h[0] = data->set.headers;
#endif

  /* loop through one or two lists */
  ପାଇଁ(i = 0; i < numlists; i++) {
    ପାଇଁ(headers = h[i]; headers; headers = headers->next) {
      CURLcode result = CURLE_OK;
      bool blankheader = FALSE;
      ସଂରଚନା Curl_str name;
      ଅଚଳ ଅକ୍ଷର *p = headers->data;
      ଅଚଳ ଅକ୍ଷର *origp = p;
      size_t hlen = strlen(origp);

      /* explicitly asked to send header without content is done by a header
         that ends with a semicolon, but there must be no colon present in the
         name */
      ଯଦି(!curlx_str_until(&p, &name, hlen, ';') &&
         !curlx_str_single(&p, ';') &&
         !curlx_str_single(&p, '\0') &&
         !memchr(curlx_str(&name), ':', curlx_strlen(&name)))
        blankheader = TRUE;
      ଅନ୍ୟଥା {
        p = origp;
        ଯଦି(!curlx_str_until(&p, &name, hlen, ':') &&
           !curlx_str_single(&p, ':')) {
          ସଂରଚନା Curl_str val;
          curlx_str_untilnl(&p, &val, hlen);
          curlx_str_trimblanks(&val);
          ଯଦି(!curlx_strlen(&val))
            /* no content, do not send this */
            ଜାରି;
        }
        ଅନ୍ୟଥା
          /* no colon */
          ଜାରି;
      }

      /* a field name is a token and carries no surrounding whitespace, so
         trim the parsed name before matching. Otherwise `Authorization :`
         (space before the colon) slips past the Authorization/Cookie check
         below and gets forwarded to another host on a redirect. */
      curlx_str_trimblanks(&name);

      /* only send this if the contents was non-blank or done special */

      ଯଦି(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom
            Host: header as that will produce *two* in the same
            request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      ଅନ୍ୟଥା ଯଦି(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ଅନ୍ୟଥା ଯଦି(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ଅନ୍ୟଥା ଯଦି(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      ଅନ୍ୟଥା ଯଦି(curlx_str_casecompare(&name, "Connection"))
        /* Connection headers are handled specially */
        ;
      ଅନ୍ୟଥା ଯଦି((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        /* HTTP/2 does not support chunked requests */
        ;
      ଅନ୍ୟଥା ଯଦି((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      ଅନ୍ୟଥା ଯଦି(blankheader) {
        result = curlx_dyn_addn(req, curlx_str(&name), curlx_strlen(&name));
        ଯଦି(!result)
          result = curlx_dyn_addn(req, STRCONST(":\r\n"));
      }
      ଅନ୍ୟଥା
        result = curlx_dyn_addf(req, "%s\r\n", origp);

      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

#ifndef CURL_DISABLE_PARSEDATE
CURLcode Curl_add_timecondition(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା dynbuf *req)
{
  ଅଚଳ ସଂରଚନା tm *tm;
  ସଂରଚନା tm keeptime;
  CURLcode result;
  ଅକ୍ଷର datestr[80];
  ଅଚଳ ଅକ୍ଷର *condp;
  size_t len;

  ଯଦି(data->set.timecondition == CURL_TIMECOND_NONE)
    /* no condition was asked for */
    ଫେରନ୍ତୁ CURLE_OK;

  result = curlx_gmtime(data->set.timevalue, &keeptime);
  ଯଦି(result) {
    failf(data, "Invalid TIMEVALUE");
    ଫେରନ୍ତୁ result;
  }
  tm = &keeptime;

  ବାଛ(data->set.timecondition) {
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    DEBUGF(infof(data, "invalid time condition"));
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  ସ୍ଥିତି CURL_TIMECOND_IFMODSINCE:
    condp = "If-Modified-Since";
    len = 17;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_TIMECOND_IFUNMODSINCE:
    condp = "If-Unmodified-Since";
    len = 19;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_TIMECOND_LASTMOD:
    condp = "Last-Modified";
    len = 13;
    ଭାଙ୍ଗ;
  }

  ଯଦି(Curl_checkheaders(data, condp, len)) {
    /* A custom header was specified; it will be sent instead. */
    ଫେରନ୍ତୁ CURLE_OK;
  }

  /* The If-Modified-Since header family should have their times set in
   * GMT as RFC2616 defines: "All HTTP date/time stamps MUST be
   * represented in Greenwich Mean Time (GMT), without exception. For the
   * purposes of HTTP, GMT is exactly equal to UTC (Coordinated Universal
   * Time)." (see page 20 of RFC2616).
   */

  /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
  curl_msnprintf(datestr, ଆକାର(datestr),
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
  ଫେରନ୍ତୁ result;
}
#ଅନ୍ୟଥା
/* disabled */
CURLcode Curl_add_timecondition(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା dynbuf *req)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)req;
  ଫେରନ୍ତୁ CURLE_OK;
}
#endif

ଶୂନ୍ୟ Curl_http_method(ସଂରଚନା Curl_easy *data,
                      ଅଚଳ ଅକ୍ଷର **method, Curl_HttpReq *reqp)
{
  Curl_HttpReq httpreq = (Curl_HttpReq)data->state.httpreq;
  ଅଚଳ ଅକ୍ଷର *request;
#ifndef CURL_DISABLE_WEBSOCKETS
  ଯଦି(data->conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
    httpreq = HTTPREQ_GET;
  ଅନ୍ୟଥା
#endif
  ଯଦି((data->conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_FTP)) &&
     data->state.upload)
    httpreq = HTTPREQ_PUT;

  /* Now set the 'request' pointer to the proper request string */
  ଯଦି(!data->state.http_ignorecustom &&
     CURL_EASY_STR(data, STRING_CUSTOMREQUEST)) {
    request = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);
  }
  ଅନ୍ୟଥା {
    ଯଦି(data->req.no_body)
      request = "HEAD";
    ଅନ୍ୟଥା {
      DEBUGASSERT((httpreq >= HTTPREQ_GET) && (httpreq <= HTTPREQ_HEAD));
      ବାଛ(httpreq) {
      ସ୍ଥିତି HTTPREQ_POST:
      ସ୍ଥିତି HTTPREQ_POST_FORM:
      ସ୍ଥିତି HTTPREQ_POST_MIME:
        request = "POST";
        ଭାଙ୍ଗ;
      ସ୍ଥିତି HTTPREQ_PUT:
        request = "PUT";
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* this should never happen */
      ସ୍ଥିତି HTTPREQ_GET:
        request = "GET";
        ଭାଙ୍ଗ;
      ସ୍ଥିତି HTTPREQ_HEAD:
        request = "HEAD";
        ଭାଙ୍ଗ;
      }
    }
  }
  *method = request;
  *reqp = httpreq;
}

ସ୍ଥିର CURLcode http_set_aptr_host(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ଅଚଳ ଅକ୍ଷର *ptr = ଖାଲି;

  curlx_safefree(data->state.http_host);
#ifndef CURL_DISABLE_COOKIES
  curlx_safefree(data->req.cookiehost);
#endif

  ଯଦି(Curl_peer_equal(data->state.initial_origin, data->state.origin))
    ptr = Curl_checkheaders(data, STRCONST("Host"));

  ଯଦି(ptr) {
#ifndef CURL_DISABLE_COOKIES
    /* If we have a given custom Host: header, we extract the hostname in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the hostname
       is the same as the first one! */
    ଅକ୍ଷର *cookiehost;
    CURLcode result = copy_custom_ମୂଲ୍ୟ(ptr, &cookiehost);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    ଯଦି(!*cookiehost)
      /* ignore empty data */
      curlx_free(cookiehost);
    ଅନ୍ୟଥା {
      /* If the host begins with '[', we start searching for the port after
         the bracket has been closed */
      ଯଦି(*cookiehost == '[') {
        ଅକ୍ଷର *closingbracket;
        /* since the 'cookiehost' is an allocated memory area that will be
           freed later we cannot increment the pointer */
        memmove(cookiehost, cookiehost + 1, strlen(cookiehost) - 1);
        closingbracket = strchr(cookiehost, ']');
        ଯଦି(closingbracket)
          *closingbracket = 0;
      }
      ଅନ୍ୟଥା {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା startsearch = 0;
        ଅକ୍ଷର *colon = strchr(cookiehost + startsearch, ':');
        ଯଦି(colon)
          *colon = 0; /* The host must not include an embedded port number */
      }
      data->req.cookiehost = cookiehost;
    }
#endif

    ଯଦି(!curl_strequal("Host:", ptr)) {
      data->state.http_host = curl_maprintf("Host:%s", &ptr[5]);
      ଯଦି(!data->state.http_host)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
  }
  ଅନ୍ୟଥା {
    /* This is the  HTTP Host: header, so we want
     * - for IPv6 origins: "[ipv6-address]" where the IPv6 address is
     *  found in origin->hostname, stripped of zoneid/scopeid.
     * - the (IDN converted) origin->hostname (DNS name or IPv4) otherwise.
     * Note: zoneid/scopeid  only applies to local routing and has no
     * meaning on the remote HTTP server (eg. would confuse it). */
    bool ipv6 = (bool)data->state.origin->ipv6;
    ସଂରଚନା dynbuf tmp;
    size_t hlen;
    CURLcode result;

    curlx_dyn_init(&tmp, DYN_HTTP_REQUEST);
    result = curlx_dyn_addn(&tmp, STRCONST("Host: "));
    ଯଦି(!result && ipv6)
      result = curlx_dyn_addn(&tmp, STRCONST("["));
    ଯଦି(!result)
      result = curlx_dyn_add(&tmp, data->state.origin->hostname);
    ଯଦି(!result && ipv6)
      result = curlx_dyn_addn(&tmp, STRCONST("]"));
    ଯଦି(!result &&
       ((data->state.origin->port != data->state.origin->scheme->defport) ||
       (data->state.origin->scheme->family != conn->scheme->family))) {
      result = curlx_dyn_addf(&tmp, ":%u", data->state.origin->port);
    }

    data->state.http_host = result ? ଖାଲି : curlx_dyn_take(&tmp, &hlen);
    curlx_dyn_free(&tmp);
    ଫେରନ୍ତୁ result;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Append the request-target to the HTTP request
 */
ସ୍ଥିର CURLcode http_target(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା dynbuf *r)
{
  CURLcode result = CURLE_OK;
  ଅଚଳ ଅକ୍ଷର *path = data->state.up.path;
  ଅଚଳ ଅକ୍ଷର *query = data->state.up.query;
#ifndef CURL_DISABLE_PROXY
  ସଂରଚନା connectdata *conn = data->conn;
#endif

  ଯଦି(CURL_EASY_STR(data, STRING_TARGET)) {
    path = CURL_EASY_STR(data, STRING_TARGET);
    query = ଖାଲି;
  }

#ifndef CURL_DISABLE_PROXY
  ଯଦି(conn->bits.origin_is_proxy) {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL, but if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the decoded hostname! */

    /* and no fragment part */
    CURLUcode uc;
    ଅକ୍ଷର *url;
    CURLU *h = curl_url_dup(data->state.uh);
    ଯଦି(!h)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

    ଯଦି(!data->state.origin->ipv6 &&
       (data->state.origin->user_hostname != data->state.origin->hostname)) {
      uc = curl_url_set(h, CURLUPART_HOST, data->state.origin->hostname, 0);
      ଯଦି(uc) {
        curl_url_cleanup(h);
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      }
    }
    uc = curl_url_set(h, CURLUPART_FRAGMENT, ଖାଲି, 0);
    ଯଦି(uc) {
      curl_url_cleanup(h);
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }

    ଯଦି(data->state.origin->scheme == &Curl_scheme_http) {
      /* when getting HTTP, we do not want the userinfo the URL */
      uc = curl_url_set(h, CURLUPART_USER, ଖାଲି, 0);
      ଯଦି(uc) {
        curl_url_cleanup(h);
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      }
      uc = curl_url_set(h, CURLUPART_PASSWORD, ଖାଲି, 0);
      ଯଦି(uc) {
        curl_url_cleanup(h);
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      }
    }
    ଅନ୍ୟଥା ଯଦି(data->state.creds && (data->state.creds->source != CREDS_URL)) {
        /* credentials not from the URL need to be set */
      uc = curl_url_set(h, CURLUPART_USER,
                        data->state.creds->user, CURLU_URLENCODE);
      ଯଦି(!uc)
        uc = curl_url_set(h, CURLUPART_PASSWORD,
                          data->state.creds->passwd, CURLU_URLENCODE);
      ଯଦି(uc) {
        curl_url_cleanup(h);
        ଫେରନ୍ତୁ Curl_uc_to_curlcode(uc);
      }
    }

    /* Extract the URL to use in the request. */
    uc = curl_url_get(h, CURLUPART_URL, &url, CURLU_NO_DEFAULT_PORT);
    ଯଦି(uc) {
      curl_url_cleanup(h);
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }

    curl_url_cleanup(h);

    /* target or URL */
    result = curlx_dyn_add(r, CURL_EASY_STR(data, STRING_TARGET) ?
      CURL_EASY_STR(data, STRING_TARGET) : url);
    curlx_free(url);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;

    ଯଦି((data->state.origin->scheme == &Curl_scheme_ftp) &&
       data->set.proxy_transfer_mode) {
      /* when doing ftp, append ;type=<a|i> if not present */
      size_t len = strlen(path);
      bool type_present = FALSE;
      ଯଦି((len >= 7) && !memcmp(&path[len - 7], ";type=", 6)) {
        ବାଛ(Curl_raw_toupper(path[len - 1])) {
        ସ୍ଥିତି 'A':
        ସ୍ଥିତି 'D':
        ସ୍ଥିତି 'I':
          type_present = TRUE;
          ଭାଙ୍ଗ;
        }
      }
      ଯଦି(!type_present) {
        result = curlx_dyn_addf(r, ";type=%c",
                                data->state.prefer_ascii ? 'a' : 'i');
        ଯଦି(result)
          ଫେରନ୍ତୁ result;
      }
    }
  }

  ଅନ୍ୟଥା
#endif
  {
    result = curlx_dyn_add(r, path);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    ଯଦି(query)
      result = curlx_dyn_addf(r, "?%s", query);
  }

  ଫେରନ୍ତୁ result;
}

#ଯଦି !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
ସ୍ଥିର CURLcode set_post_reader(ସଂରଚନା Curl_easy *data, Curl_HttpReq httpreq)
{
  CURLcode result;

  ବାଛ(httpreq) {
#ifndef CURL_DISABLE_MIME
  ସ୍ଥିତି HTTPREQ_POST_MIME:
    data->state.mimepost = data->set.mimepostp;
    ଭାଙ୍ଗ;
#endif
#ifndef CURL_DISABLE_FORM_API
  ସ୍ଥିତି HTTPREQ_POST_FORM:
    /* Convert the form structure into a mime structure, then keep
       the conversion */
    ଯଦି(!data->state.formp) {
      data->state.formp = curlx_calloc(1, ଆକାର(curl_mimepart));
      ଯଦି(!data->state.formp)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      Curl_mime_cleanpart(data->state.formp);
      result = Curl_getformdata(data, data->state.formp, data->set.httppost,
                                data->state.fread_func);
      ଯଦି(result) {
        curlx_safefree(data->state.formp);
        ଫେରନ୍ତୁ result;
      }
      data->state.mimepost = data->state.formp;
    }
    ଭାଙ୍ଗ;
#endif
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    data->state.mimepost = ଖାଲି;
    ଭାଙ୍ଗ;
  }

  ବାଛ(httpreq) {
  ସ୍ଥିତି HTTPREQ_POST_FORM:
  ସ୍ଥିତି HTTPREQ_POST_MIME:
    /* This is form posting using mime data. */
#ifndef CURL_DISABLE_MIME
    ଯଦି(data->state.mimepost) {
      ଅଚଳ ଅକ୍ଷର *cthdr = Curl_checkheaders(data, STRCONST("Content-Type"));

      /* Read and seek body only. */
      data->state.mimepost->flags |= MIME_BODY_ONLY;

      /* Prepare the mime structure headers & set content type. */

      ଯଦି(cthdr)
        ପାଇଁ(cthdr += 13; *cthdr == ' '; cthdr++)
          ;
      ଅନ୍ୟଥା ଯଦି(data->state.mimepost->kind == MIMEKIND_MULTIPART)
        cthdr = "multipart/form-data";

      curl_mime_headers(data->state.mimepost, data->set.headers, 0);
      result = Curl_mime_prepare_headers(data, data->state.mimepost, cthdr,
                                         ଖାଲି, MIMESTRATEGY_FORM);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      curl_mime_headers(data->state.mimepost, ଖାଲି, 0);
      result = Curl_creader_set_mime(data, data->state.mimepost);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
    ଅନ୍ୟଥା
#endif
    {
      result = Curl_creader_set_null(data);
    }
    data->state.infilesize = Curl_creader_total_length(data);
    ଫେରନ୍ତୁ result;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ Curl_creader_set_null(data);
  }
  /* never reached */
}
#endif

ସ୍ଥିର CURLcode set_reader(ସଂରଚନା Curl_easy *data, Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
  curl_off_t postsize = data->state.infilesize;

  DEBUGASSERT(data->conn);

  ଯଦି(data->req.authneg) {
    ଫେରନ୍ତୁ Curl_creader_set_null(data);
  }

  ବାଛ(httpreq) {
  ସ୍ଥିତି HTTPREQ_PUT: /* Let's PUT the data to the server! */
    ଫେରନ୍ତୁ postsize ? Curl_creader_set_fread(data, postsize) :
      Curl_creader_set_null(data);

#ଯଦି !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  ସ୍ଥିତି HTTPREQ_POST_FORM:
  ସ୍ଥିତି HTTPREQ_POST_MIME:
    ଫେରନ୍ତୁ set_post_reader(data, httpreq);
#endif

  ସ୍ଥିତି HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */
    /* the size of the post body */
    ଯଦି(!postsize) {
      result = Curl_creader_set_null(data);
    }
    ଅନ୍ୟଥା ଯଦି(data->set.postfields) {
      size_t plen = curlx_sotouz_range(postsize, 0, SIZE_MAX);
      ଯଦି(plen == SIZE_MAX)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      ଅନ୍ୟଥା ଯଦି(plen)
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      ଅନ୍ୟଥା
        result = Curl_creader_set_null(data);
    }
    ଅନ୍ୟଥା {
      /* we read the bytes from the callback. In case "chunked" encoding
       * is forced by the application, we disregard `postsize`. This is
       * a backward compatibility decision to earlier versions where
       * chunking disregarded this. See issue #13229. */
      bool chunked = FALSE;
      ଅକ୍ଷର *ptr = Curl_checkheaders(data, STRCONST("Transfer-Encoding"));
      ଯଦି(ptr) {
        /* Some kind of TE is requested, check if 'chunked' is chosen */
        chunked = Curl_compareheader(ptr, STRCONST("Transfer-Encoding:"),
                                     STRCONST("chunked"));
      }
      result = Curl_creader_set_fread(data, chunked ? -1 : postsize);
    }
    ଫେରନ୍ତୁ result;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    /* HTTP GET/HEAD download, has no body, needs no Content-Length */
    data->state.infilesize = 0;
    ଫେରନ୍ତୁ Curl_creader_set_null(data);
  }
  /* not reached */
}

ସ୍ଥିର CURLcode http_resume(ସଂରଚନା Curl_easy *data, Curl_HttpReq httpreq)
{
  ଯଦି((HTTPREQ_POST == httpreq || HTTPREQ_POST_FORM == httpreq ||
      HTTPREQ_POST_MIME == httpreq || HTTPREQ_PUT == httpreq) &&
     data->state.resume_from) {
    failf(data, "HTTP upload cannot be resumed");
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode http_req_set_TE(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା dynbuf *req,
                                ପୂର୍ଣ୍ଣସଂଖ୍ୟା httpversion)
{
  CURLcode result = CURLE_OK;
  ଅଚଳ ଅକ୍ଷର *ptr;

  ptr = Curl_checkheaders(data, STRCONST("Transfer-Encoding"));
  ଯଦି(ptr) {
    /* Some kind of TE is requested, check if 'chunked' is chosen */
    data->req.upload_chunky =
      Curl_compareheader(ptr,
                         STRCONST("Transfer-Encoding:"), STRCONST("chunked"));
    ଯଦି(data->req.upload_chunky && (httpversion >= 20)) {
      infof(data, "suppressing chunked transfer encoding on connection "
            "using HTTP version 2 or higher");
      data->req.upload_chunky = FALSE;
    }
  }
  ଅନ୍ୟଥା {
    curl_off_t req_clen = Curl_creader_total_length(data);

    ଯଦି(req_clen < 0) {
      /* indeterminate request content length */
      ଯଦି(httpversion > 10) {
        /* On HTTP/1.1, enable chunked, on HTTP/2 and later we do not
         * need it */
        data->req.upload_chunky = (httpversion < 20);
      }
      ଅନ୍ୟଥା {
        failf(data, "Chunky upload is not supported by HTTP 1.0");
        ଫେରନ୍ତୁ CURLE_UPLOAD_FAILED;
      }
    }
    ଅନ୍ୟଥା {
      /* else, no chunky upload */
      data->req.upload_chunky = FALSE;
    }

    ଯଦି(data->req.upload_chunky)
      result = curlx_dyn_add(req, "Transfer-Encoding: chunked\r\n");
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode addexpect(ସଂରଚନା Curl_easy *data, ସଂରଚନା dynbuf *r,
                          ପୂର୍ଣ୍ଣସଂଖ୍ୟା httpversion, bool *announced_exp100)
{
  CURLcode result;
  ଅକ୍ଷର *ptr;

  *announced_exp100 = FALSE;
  /* Avoid Expect: 100-continue if Upgrade: is used */
  ଯଦି(data->req.upgr101 != UPGR101_NONE)
    ଫେରନ୍ତୁ CURLE_OK;

  /* For really small puts we do not use Expect: headers at all, and for
     the somewhat bigger ones we allow the app to disable it. Make
     sure that the expect100header is always set to the preferred value
     here. */
  ptr = Curl_checkheaders(data, STRCONST("Expect"));
  ଯଦି(ptr) {
    *announced_exp100 =
      Curl_compareheader(ptr, STRCONST("Expect:"), STRCONST("100-continue"));
  }
  ଅନ୍ୟଥା ଯଦି(!data->state.disableexpect && (httpversion == 11)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    curl_off_t client_len = Curl_creader_client_length(data);
    ଯଦି(client_len > EXPECT_100_THRESHOLD || client_len < 0) {
      result = curlx_dyn_addn(r, STRCONST("Expect: 100-continue\r\n"));
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      *announced_exp100 = TRUE;
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode http_add_content_hds(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା dynbuf *r,
                                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା httpversion,
                                     Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
  curl_off_t req_clen;
  bool announced_exp100 = FALSE;

  DEBUGASSERT(data->conn);
  ଯଦି(data->req.upload_chunky) {
    result = Curl_httpchunk_add_reader(data);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  /* Get the request body length that has been set up */
  req_clen = Curl_creader_total_length(data);
  ବାଛ(httpreq) {
  ସ୍ଥିତି HTTPREQ_PUT:
  ସ୍ଥିତି HTTPREQ_POST:
#ଯଦି !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  ସ୍ଥିତି HTTPREQ_POST_FORM:
  ସ୍ଥିତି HTTPREQ_POST_MIME:
#endif
    /* We only set Content-Length and allow a custom Content-Length if
       we do not upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length).
       We do not override a custom "Content-Length" header, but during
       authentication negotiation that header is suppressed.
     */
    ଯଦି(req_clen >= 0 && !data->req.upload_chunky &&
       (data->req.authneg ||
        !Curl_checkheaders(data, STRCONST("Content-Length")))) {
      /* we allow replacing this header if not during auth negotiation,
         although it is not wise to actually set your own */
      result = curlx_dyn_addf(r, "Content-Length: %" FMT_OFF_T "\r\n",
                              req_clen);
    }
    ଯଦି(result)
      ଯାଅ out;

#ifndef CURL_DISABLE_MIME
    /* Output mime-generated headers. */
    ଯଦି(data->state.mimepost &&
       ((httpreq == HTTPREQ_POST_FORM) || (httpreq == HTTPREQ_POST_MIME))) {
      ସଂରଚନା curl_slist *hdr;

      ପାଇଁ(hdr = data->state.mimepost->curlheaders; hdr; hdr = hdr->next) {
        result = curlx_dyn_addf(r, "%s\r\n", hdr->data);
        ଯଦି(result)
          ଯାଅ out;
      }
    }
#endif
    ଯଦି(httpreq == HTTPREQ_POST &&
       !Curl_checkheaders(data, STRCONST("Content-Type"))) {
      result = curlx_dyn_addn(r, STRCONST("Content-Type: application/"
                                          "x-www-form-urlencoded\r\n"));
      ଯଦି(result)
        ଯାଅ out;
    }
    result = addexpect(data, r, httpversion, &announced_exp100);
    ଯଦି(result)
      ଯାଅ out;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }

  Curl_pgrsSetUploadSize(data, req_clen);
  ଯଦି(announced_exp100)
    result = http_exp100_add_reader(data);

out:
  ଫେରନ୍ତୁ result;
}

#ifndef CURL_DISABLE_COOKIES

ସ୍ଥିର CURLcode http_cookies(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା dynbuf *r)
{
  CURLcode result = CURLE_OK;
  ଅଚଳ ଅକ୍ଷର *addcookies = ଖାଲି;
  bool linecap = FALSE;
  ଯଦି(CURL_EASY_STR(data, STRING_COOKIE) &&
     !Curl_checkheaders(data, STRCONST("Cookie")) &&
     Curl_auth_allowed_to_host(data))
    addcookies = CURL_EASY_STR(data, STRING_COOKIE);

  ଯଦି(data->cookies || addcookies) {
    ସଂରଚନା Curl_llist list;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = 0;

    ଯଦି(data->cookies && data->state.cookie_engine) {
      bool okay;
      ଅଚଳ ଅକ୍ଷର *host = data->req.cookiehost ?
        data->req.cookiehost : data->state.origin->hostname;
      Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
      result = Curl_cookie_getlist(data, &okay, host, &list);
      ଯଦି(!result && okay) {
        ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n;
        size_t clen = 8; /* hold the size of the generated Cookie: header */

        /* loop through all cookies that matched */
        ପାଇଁ(n = Curl_llist_head(&list); n; n = Curl_ଗଣ୍ଠି_next(n)) {
          ସଂରଚନା Cookie *co = Curl_ଗଣ୍ଠି_elem(n);
          ଯଦି(co->ମୂଲ୍ୟ) {
            size_t add;
            ଯଦି(!ଗଣନା_2) {
              result = curlx_dyn_addn(r, STRCONST("Cookie: "));
              ଯଦି(result)
                ଭାଙ୍ଗ;
            }
            add = strlen(co->name) + strlen(co->ମୂଲ୍ୟ) + 1;
            ଯଦି(clen + add >= MAX_COOKIE_HEADER_LEN) {
              infof(data, "Restricted outgoing cookies due to header size, "
                    "'%s' not sent", co->name);
              linecap = TRUE;
              ଭାଙ୍ଗ;
            }
            result = curlx_dyn_addf(r, "%s%s=%s", ଗଣନା_2 ? "; " : "",
                                    co->name, co->ମୂଲ୍ୟ);
            ଯଦି(result)
              ଭାଙ୍ଗ;
            clen += add + (ଗଣନା_2 ? 2 : 0);
            ଗଣନା_2++;
          }
        }
        Curl_llist_destroy(&list, ଖାଲି);
      }
      Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    }
    ଯଦି(addcookies && !result && !linecap) {
      ଯଦି(!ଗଣନା_2)
        result = curlx_dyn_addn(r, STRCONST("Cookie: "));
      ଯଦି(!result) {
        result = curlx_dyn_addf(r, "%s%s", ଗଣନା_2 ? "; " : "", addcookies);
        ଗଣନା_2++;
      }
    }
    ଯଦି(ଗଣନା_2 && !result)
      result = curlx_dyn_addn(r, STRCONST("\r\n"));

    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଫେରନ୍ତୁ result;
}
#ଅନ୍ୟଥା
#define http_cookies(a, b) CURLE_OK
#endif

ସ୍ଥିର CURLcode http_range(ସଂରଚନା Curl_easy *data,
                           Curl_HttpReq httpreq)
{
  ଯଦି(data->state.use_range) {
    /*
     * A range is selected. We use different headers whether we are downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    ଯଦି(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_HEAD)) &&
       !Curl_checkheaders(data, STRCONST("Range"))) {
      /* if a line like this was already allocated, free the previous one */
      curlx_free(data->state.rangeline);
      data->state.rangeline = curl_maprintf("Range: bytes=%s\r\n",
                                                 data->state.range);
      ଯଦି(!data->state.rangeline)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
    ଅନ୍ୟଥା ଯଦି((httpreq == HTTPREQ_POST || httpreq == HTTPREQ_PUT) &&
            !Curl_checkheaders(data, STRCONST("Content-Range"))) {
      curl_off_t req_clen = Curl_creader_total_length(data);
      /* if a line like this was already allocated, free the previous one */
      curlx_free(data->state.rangeline);

      ଯଦି(data->set.set_resume_from < 0) {
        /* Upload resume was asked for, but we do not know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes 0-%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n", req_clen - 1, req_clen);
      }
      ଅନ୍ୟଥା ଯଦି(data->state.resume_from) {
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
      ଅନ୍ୟଥା {
        /* Range was selected and then we pass the incoming range and append
           total size */
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes %s/%" FMT_OFF_T "\r\n",
                        data->state.range, req_clen);
      }
      ଯଦି(!data->state.rangeline)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode http_firstwrite(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା SingleRequest *k = &data->req;

  ଯଦି(data->req.newurl) {
    ଯଦି(conn->bits.close) {
      /* Abort after the headers if "follow Location" is set
         and we are set to close anyway. */
      CURL_REQ_CLEAR_RECV(data);
      k->done = TRUE;
      ଫେରନ୍ତୁ CURLE_OK;
    }
    /* We have a new URL to load, but since we want to be able to reuse this
       connection properly, we read the full response in "ignore more" */
    k->ignorebody = TRUE;
    infof(data, "Ignoring the response-body");
  }
  ଯଦି(data->state.resume_from && !k->content_range &&
     (data->state.httpreq == HTTPREQ_GET) &&
     !k->ignorebody) {

    ଯଦି(k->size == data->state.resume_from) {
      /* The resume point is at the end of file, consider this fine even if it
         does not allow resume from here. */
      infof(data, "The entire document is already downloaded");
      streamclose(conn);
      /* Abort download */
      CURL_REQ_CLEAR_RECV(data);
      k->done = TRUE;
      ଫେରନ୍ତୁ CURLE_OK;
    }

    /* we wanted to resume a download, although the server does not seem to
     * support this and we did this with a GET (if it was not a GET we did a
     * POST or PUT resume) */
    failf(data, "HTTP server does not seem to support "
          "byte ranges. Cannot resume.");
    ଫେରନ୍ତୁ CURLE_RANGE_ERROR;
  }

  ଯଦି(data->set.timecondition && !data->state.range &&
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
    ଫେରନ୍ତୁ CURLE_OK;
  } /* we have a time condition */

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode http_check_new_conn(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ଅଚଳ ଅକ୍ଷର *info_version = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *alpn;
  CURLcode result;

  alpn = Curl_conn_get_alpn_negotiated(data, conn);
  ଯଦି(alpn && !strcmp("h3", alpn)) {
#ifndef CURL_DISABLE_PROXY
    ଯଦି(!conn->bits.origin_is_proxy)
#endif
      DEBUGASSERT(Curl_conn_http_version(data, conn) == 30);
    info_version = "HTTP/3";
  }
  ଅନ୍ୟଥା ଯଦି(alpn && !strcmp("h2", alpn)) {
#ifndef CURL_DISABLE_PROXY
    ଯଦି((Curl_conn_http_version(data, conn) != 20) &&
       conn->bits.origin_is_proxy) {
      result = Curl_http2_switch(data);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
    ଅନ୍ୟଥା
#endif
    DEBUGASSERT(Curl_conn_http_version(data, conn) == 20);
    info_version = "HTTP/2";
  }
  ଅନ୍ୟଥା {
    /* Check if user wants to use HTTP/2 with clear TCP */
    ଯଦି(Curl_http2_may_switch(data)) {
      DEBUGF(infof(data, "HTTP/2 over clean TCP"));
      result = Curl_http2_switch(data);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      info_version = "HTTP/2";
      /* There is no ALPN here, but the connection is now definitely h2 */
      conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(conn);
    }
    ଅନ୍ୟଥା
      info_version = "HTTP/1.x";
  }

  ଯଦି(info_version)
    infof(data, "using %s", info_version);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode http_add_connection_hd(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା dynbuf *req)
{
  ସଂରଚନା curl_slist *head;
  ଅଚଳ ଅକ୍ଷର *sep = "Connection: ";
  CURLcode result = CURLE_OK;
  size_t rlen = curlx_dyn_len(req);
  bool skip;

  /* Add the 1st custom "Connection: " header, if there is one */
  ପାଇଁ(head = data->set.headers; head; head = head->next) {
    ଯଦି(curl_strnequal(head->data, "Connection", 10) &&
       Curl_headersep(head->data[10]) &&
       !http_header_is_empty(head->data)) {
      ଅକ୍ଷର *ମୂଲ୍ୟ;
      result = copy_custom_ମୂଲ୍ୟ(head->data, &ମୂଲ୍ୟ);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      result = curlx_dyn_addf(req, "%s%s", sep, ମୂଲ୍ୟ);
      sep = ", ";
      curlx_free(ମୂଲ୍ୟ);
      ଭାଙ୍ଗ; /* leave, having added 1st one */
    }
  }

  /* add our internal Connection: header values, if we have any */
  ଯଦି(!result && data->state.http_hd_te) {
    result = curlx_dyn_addf(req, "%s%s", sep, "TE");
    sep = ", ";
  }
  ଯଦି(!result && data->state.http_hd_upgrade) {
    result = curlx_dyn_addf(req, "%s%s", sep, "Upgrade");
    sep = ", ";
  }
  ଯଦି(!result && data->state.http_hd_h2_settings) {
    result = curlx_dyn_addf(req, "%s%s", sep, "HTTP2-Settings");
  }
  ଯଦି(!result && (rlen < curlx_dyn_len(req)))
    result = curlx_dyn_addn(req, STRCONST("\r\n"));
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* Add all user-defined Connection: headers after the first */
  skip = TRUE;
  ପାଇଁ(head = data->set.headers; head; head = head->next) {
    ଯଦି(curl_strnequal(head->data, "Connection", 10) &&
       Curl_headersep(head->data[10]) &&
       !http_header_is_empty(head->data)) {
      ଯଦି(skip) {
        skip = FALSE;
        ଜାରି;
      }
      result = curlx_dyn_addf(req, "%s\r\n", head->data);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

/* Header identifier in order we send them by default */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
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

ସ୍ଥିର CURLcode http_add_hd(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା dynbuf *req,
                            http_hd_t id,
                            unsigned ଅକ୍ଷର httpversion,
                            ଅଚଳ ଅକ୍ଷର *method,
                            Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
#ଯଦି !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_PROXY) || \
  !defined(CURL_DISABLE_WEBSOCKETS)
  ସଂରଚନା connectdata *conn = data->conn;
#endif
  ବାଛ(id) {
  ସ୍ଥିତି H1_HD_REQUEST:
    /* add the main request stuff */
    /* GET/HEAD/POST/PUT */
    result = curlx_dyn_addf(req, "%s ", method);
    ଯଦି(!result)
      result = http_target(data, req);
    ଯଦି(!result)
      result = curlx_dyn_addf(req, " HTTP/%s\r\n",
                              get_http_string(httpversion));
    ଭାଙ୍ଗ;

  ସ୍ଥିତି H1_HD_HOST:
    ଯଦି(data->state.http_host) {
      result = curlx_dyn_add(req, data->state.http_host);
      ଯଦି(!result)
        result = curlx_dyn_addn(req, STRCONST("\r\n"));
    }
    ଭାଙ୍ଗ;

#ifndef CURL_DISABLE_PROXY
  ସ୍ଥିତି H1_HD_PROXY_AUTH:
    ଯଦି(data->req.hd_proxy_auth)
      result = curlx_dyn_add(req, data->req.hd_proxy_auth);
    ଭାଙ୍ଗ;
#endif

  ସ୍ଥିତି H1_HD_AUTH:
    ଯଦି(data->req.hd_auth)
      result = curlx_dyn_add(req, data->req.hd_auth);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି H1_HD_RANGE:
    ଯଦି(data->state.use_range && data->state.rangeline)
      result = curlx_dyn_add(req, data->state.rangeline);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି H1_HD_USER_AGENT: {
    ଅଚଳ ଅକ୍ଷର *ua = CURL_EASY_STR(data, STRING_USERAGENT);
    ଯଦି(ua && *ua && !Curl_checkheaders(data, STRCONST("User-Agent")))
      result = curlx_dyn_addf(req, "User-Agent: %s\r\n", ua);
    ଭାଙ୍ଗ;
  }

  ସ୍ଥିତି H1_HD_ACCEPT:
    ଯଦି(!Curl_checkheaders(data, STRCONST("Accept")))
      result = curlx_dyn_add(req, "Accept: */*\r\n");
    ଭାଙ୍ଗ;

  ସ୍ଥିତି H1_HD_TE:
#ifdef HAVE_LIBZ
    ଯଦି(!Curl_checkheaders(data, STRCONST("TE")) &&
       data->set.http_transfer_encoding) {
      data->state.http_hd_te = TRUE;
      result = curlx_dyn_add(req, "TE: gzip\r\n");
    }
#endif
    ଭାଙ୍ଗ;

  ସ୍ଥିତି H1_HD_ACCEPT_ENCODING: {
    ଅଚଳ ଅକ୍ଷର *enc = CURL_EASY_STR(data, STRING_ENCODING);
    ଯଦି(enc && !Curl_checkheaders(data, STRCONST("Accept-Encoding")))
      result = curlx_dyn_addf(req, "Accept-Encoding: %s\r\n", enc);
    ଭାଙ୍ଗ;
  }

  ସ୍ଥିତି H1_HD_REFERER:
    ଯଦି(Curl_bufref_ptr(&data->state.referer) &&
       !Curl_checkheaders(data, STRCONST("Referer")))
      result = curlx_dyn_addf(req, "Referer: %s\r\n",
                              Curl_bufref_ptr(&data->state.referer));
    ଭାଙ୍ଗ;

#ifndef CURL_DISABLE_PROXY
  ସ୍ଥିତି H1_HD_PROXY_CONNECTION:
    ଯଦି(conn->bits.origin_is_proxy &&
       !Curl_checkheaders(data, STRCONST("Proxy-Connection")) &&
       !Curl_checkProxyheaders(data, data->conn, STRCONST("Proxy-Connection")))
      result = curlx_dyn_add(req, "Proxy-Connection: Keep-Alive\r\n");
    ଭାଙ୍ଗ;
#endif

  ସ୍ଥିତି H1_HD_TRANSFER_ENCODING:
    result = http_req_set_TE(data, req, httpversion);
    ଭାଙ୍ଗ;

#ifndef CURL_DISABLE_ALTSVC
  ସ୍ଥିତି H1_HD_ALT_USED:
    ଯଦି(conn->bits.altused && conn->via_peer &&
       !Curl_checkheaders(data, STRCONST("Alt-Used")))
      result = curlx_dyn_addf(req, "Alt-Used: %s:%u\r\n",
                              conn->via_peer->hostname, conn->via_peer->port);
    ଭାଙ୍ଗ;
#endif

  ସ୍ଥିତି H1_HD_UPGRADE:
    ଯଦି(!Curl_conn_is_ssl(data->conn, FIRSTSOCKET) && (httpversion < 20) &&
       (data->state.http_neg.wanted & CURL_HTTP_V2x) &&
       data->state.http_neg.h2_upgrade) {
      /* append HTTP2 upgrade magic stuff to the HTTP request if it is not done
         over SSL */
      result = Curl_http2_request_upgrade(req, data);
    }
#ifndef CURL_DISABLE_WEBSOCKETS
    ଯଦି(!result && conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
      result = Curl_ws_request(data, req);
#endif
    ଭାଙ୍ଗ;

  ସ୍ଥିତି H1_HD_COOKIES:
    result = http_cookies(data, req);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି H1_HD_CONDITIONALS:
    result = Curl_add_timecondition(data, req);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି H1_HD_CUSTOM:
    result = Curl_add_custom_headers(data, FALSE, httpversion, req);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି H1_HD_CONTENT:
    result = http_add_content_hds(data, req, httpversion, httpreq);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି H1_HD_CONNECTION: {
    result = http_add_connection_hd(data, req);
    ଭାଙ୍ଗ;
  }

  ସ୍ଥିତି H1_HD_LAST:
    result = curlx_dyn_addn(req, STRCONST("\r\n"));
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ result;
}

/*
 * Curl_http() gets called from the generic multi_do() function when an HTTP
 * request is to be performed. This creates and sends a properly constructed
 * HTTP request.
 */
CURLcode Curl_http(ସଂରଚନା Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  Curl_HttpReq httpreq;
  ଅଚଳ ଅକ୍ଷର *method;
  ସଂରଚନା dynbuf req;
  unsigned ଅକ୍ଷର httpversion;
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

  ଯଦି(!data->conn->bits.reuse) {
    result = http_check_new_conn(data);
    ଯଦି(result)
      ଯାଅ out;
  }

  /* Add collecting of headers written to client. For a new connection,
   * we might have done that already, but reuse
   * or multiplex needs it here as well. */
  result = Curl_headers_init(data);
  ଯଦି(result)
    ଯାଅ out;

  data->state.http_hd_te = FALSE;
  data->state.http_hd_upgrade = FALSE;
  data->state.http_hd_h2_settings = FALSE;

  /* what kind of request do we need to send? */
  Curl_http_method(data, &method, &httpreq);

  /* select host to send */
  result = http_set_aptr_host(data);
  /* setup the authentication headers, how that method and host are known */
  ଯଦି(!result)
    result = Curl_http_output_auth(data, data->conn, method, httpreq,
                                   data->state.up.path,
                                   data->state.up.query, FALSE);
  /* Setup input reader, resume information and ranges */
  ଯଦି(!result)
    result = set_reader(data, httpreq);
  ଯଦି(!result)
    result = http_resume(data, httpreq);
  ଯଦି(!result)
    result = http_range(data, httpreq);
  ଯଦି(result)
    ଯାଅ out;

  httpversion = http_request_version(data);
  /* Add request line and all headers to `req` */
  ପାଇଁ(hd_id = 0; hd_id <= H1_HD_LAST; ++hd_id) {
    result = http_add_hd(data, &req, (http_hd_t)hd_id,
                         httpversion, method, httpreq);
    ଯଦି(result)
      ଯାଅ out;
  }

  /* setup variables for the upcoming transfer and send */
  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);
  result = Curl_req_send(data, &req, httpversion);

  ଯଦି((httpversion >= 20) && data->req.upload_chunky)
    /* upload_chunky was set above to set up the request in a chunky fashion,
       but is disabled here again to avoid that the chunked encoded version is
       actually used when sending the request body over h2 */
    data->req.upload_chunky = FALSE;

out:
  ଯଦି(result == CURLE_TOO_LARGE)
    failf(data, "HTTP request too large");

  curlx_dyn_free(&req);
  ଫେରନ୍ତୁ result;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  STATUS_UNKNOWN, /* not enough data to tell yet */
  STATUS_DONE, /* a status line was read */
  STATUS_BAD /* not a status line */
} statusline;

/* Check a string for a prefix. Check no more than 'len' bytes */
ସ୍ଥିର bool checkprefixmax(ଅଚଳ ଅକ୍ଷର *prefix, ଅଚଳ ଅକ୍ଷର *buffer, size_t len)
{
  size_t ch = CURLMIN(strlen(prefix), len);
  ଫେରନ୍ତୁ curl_strnequal(prefix, buffer, ch);
}

/*
 * checkhttpprefix()
 *
 * Returns TRUE if member of the list matches prefix of string
 */
ସ୍ଥିର statusline checkhttpprefix(ସଂରଚନା Curl_easy *data,
                                  ଅଚଳ ଅକ୍ଷର *s, size_t len)
{
  ସଂରଚନା curl_slist *head = data->set.http200aliases;
  statusline rc = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;

  ଯେତେବେଳେ(head) {
    ଯଦି(checkprefixmax(head->data, s, len)) {
      rc = onmatch;
      ଭାଙ୍ଗ;
    }
    head = head->next;
  }

  ଯଦି((rc != STATUS_DONE) && checkprefixmax("HTTP/", s, len))
    rc = onmatch;

  ଫେରନ୍ତୁ rc;
}

#ifndef CURL_DISABLE_RTSP
ସ୍ଥିର statusline checkrtspprefix(ସଂରଚନା Curl_easy *data,
                                  ଅଚଳ ଅକ୍ଷର *s, size_t len)
{
  statusline status = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;
  (ଶୂନ୍ୟ)data;
  ଯଦି(checkprefixmax("RTSP/", s, len))
    status = onmatch;

  ଫେରନ୍ତୁ status;
}
#endif /* CURL_DISABLE_RTSP */

ସ୍ଥିର statusline checkprotoprefix(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା connectdata *conn,
                                   ଅଚଳ ଅକ୍ଷର *s, size_t len)
{
#ifndef CURL_DISABLE_RTSP
  ଯଦି(conn->scheme->protocol & CURLPROTO_RTSP)
    ଫେରନ୍ତୁ checkrtspprefix(data, s, len);
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)conn;
#endif /* CURL_DISABLE_RTSP */

  ଫେରନ୍ତୁ checkhttpprefix(data, s, len);
}

/* HTTP header has field name `n` (a string constant) */
#define HD_IS(hd, hdlen, n) \
  (((hdlen) >= (ଆକାର(n) - 1)) && curl_strnequal(n, hd, ଆକାର(n) - 1))

#define HD_VAL(hd, hdlen, n) \
  ((((hdlen) >= (ଆକାର(n) - 1)) && (hd) && \
    curl_strnequal(n, hd, ଆକାର(n) - 1)) ? ((hd) + (ଆକାର(n) - 1)) : ଖାଲି)

/* HTTP header has field name `n` (a string constant) and contains `v`
 * (a string constant) in its value(s) */
#define HD_IS_AND_SAYS(hd, hdlen, n, v) \
  (HD_IS(hd, hdlen, n) && \
   ((hdlen) > ((ଆକାର(n) - 1) + (ଆକାର(v) - 1))) && \
   Curl_compareheader(hd, STRCONST(n), STRCONST(v)))

/*
 * http_header_a() parses a single response header starting with A.
 */
ସ୍ଥିର CURLcode http_header_a(ସଂରଚନା Curl_easy *data,
                              ଅଚଳ ଅକ୍ଷର *hd, size_t hdlen)
{
#ifndef CURL_DISABLE_ALTSVC
  ଅଚଳ ଅକ୍ଷର *v;
  v = (data->asi &&
       (Curl_xfer_is_secure(data) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_ALTSVC_HTTP")
#ଅନ୍ୟଥା
        0
#endif
         )) ? HD_VAL(hd, hdlen, "Alt-Svc:") : ଖାଲି;
  ଯଦି(v) {
    /* the ALPN of the current request */
    ସଂରଚନା SingleRequest *k = &data->req;
    ଗଣନା alpnid id = (k->httpversion == 30) ? ALPN_h3 :
      (k->httpversion == 20) ? ALPN_h2 : ALPN_h1;
    ଫେରନ୍ତୁ Curl_altsvc_parse(data, data->asi, v, data->state.origin, id);
  }
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)hd;
  (ଶୂନ୍ୟ)hdlen;
#endif
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * http_header_c() parses a single response header starting with C.
 */
ସ୍ଥିର CURLcode http_header_c(ସଂରଚନା Curl_easy *data,
                              ଅଚଳ ଅକ୍ଷର *hd, size_t hdlen)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା SingleRequest *k = &data->req;
  ଅଚଳ ଅକ୍ଷର *v;

  /* Check for Content-Length: header lines to get size. Browsers insist we
     should accept multiple Content-Length headers and that a comma separated
     list also is fine and then we should accept them all as long as they are
     the same value. Different values trigger error.
   */
  v = (!k->http_bodyless && !data->set.ignorecl && !k->ignore_cl) ?
    HD_VAL(hd, hdlen, "Content-Length:") : ଖାଲି;
  ଯଦି(v) {
    କର {
      curl_off_t contentlength;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା offt = curlx_str_numblanks(&v, &contentlength);

      ଯଦି(offt == STRE_OVERFLOW) {
        /* out of range */
        ଯଦି(data->set.max_filesize) {
          failf(data, "Maximum file size exceeded");
          ଫେରନ୍ତୁ CURLE_FILESIZE_EXCEEDED;
        }
        streamclose(conn);
        infof(data, "Overflow Content-Length: value");
        ଫେରନ୍ତୁ CURLE_OK;
      }
      ଅନ୍ୟଥା {
        ଯଦି((offt == STRE_OK) &&
           ((k->size == -1) || /* not set to something before */
            (k->size == contentlength))) { /* or the same value */

          k->size = contentlength;
          curlx_str_passblanks(&v);

          /* on a comma, loop and get the next instead */
          ଯଦି(!curlx_str_single(&v, ','))
            ଜାରି;

          ଯଦି(!curlx_str_newline(&v)) {
            k->maxdownload = k->size;
            ଫେରନ୍ତୁ CURLE_OK;
          }
        }
        /* negative, different value or rubbish - bad HTTP */
        failf(data, "Invalid Content-Length: value");
        ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
      }
    } ଯେତେବେଳେ(1);
  }
  v = (!k->http_bodyless && CURL_EASY_STR(data, STRING_ENCODING)) ?
    HD_VAL(hd, hdlen, "Content-Encoding:") : ଖାଲି;
  ଯଦି(v) {
    /*
     * Process Content-Encoding. Look for the values: identity, gzip, deflate,
     * compress, x-gzip and x-compress. x-gzip and x-compress are the same as
     * gzip and compress. (Sec 3.5 RFC 2616). zlib cannot handle compress.
     * Errors are handled further down when the response body is processed
     */
    ଫେରନ୍ତୁ Curl_build_unencoding_ଗଦା(data, v, FALSE);
  }
  /* check for Content-Type: header lines to get the MIME-type */
  v = HD_VAL(hd, hdlen, "Content-Type:");
  ଯଦି(v) {
    ଅକ୍ଷର *contenttype = Curl_copy_header_ମୂଲ୍ୟ(hd);
    ଯଦି(!contenttype)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    ଯଦି(!*contenttype)
      /* ignore empty data */
      curlx_free(contenttype);
    ଅନ୍ୟଥା {
      curlx_free(data->info.contenttype);
      data->info.contenttype = contenttype;
    }
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଯଦି((k->httpversion < 20) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "close")) {
    /*
     * [RFC 2616, section 8.1.2.1]
     * "Connection: close" is HTTP/1.1 language and means that
     * the connection will close when this request has been
     * served.
     */
    connclose(conn);
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଯଦି((k->httpversion == 10) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "keep-alive")) {
    /*
     * An HTTP/1.0 reply with the 'Connection: keep-alive' line
     * tells us the connection will be kept alive for our
     * pleasure. Default action for 1.0 is to close.
     *
     * [RFC2068, section 19.7.1] */
    connkeep(conn);
    infof(data, "HTTP/1.0 connection set to keep alive");
    ଫେରନ୍ତୁ CURLE_OK;
  }
  v = !k->http_bodyless ? HD_VAL(hd, hdlen, "Content-Range:") : ଖାଲି;
  ଯଦି(v) {
    /* Content-Range: bytes [num]-
       Content-Range: bytes: [num]-
       Content-Range: [num]-
       Content-Range: [asterisk]/[total]

       The second format was added since Sun's webserver
       JavaWebServer/1.1.1 obviously sends the header this way!
       The third added since some servers use that!
       The fourth means the requested range was unsatisfied.
     */

    ଅଚଳ ଅକ୍ଷର *ptr = v;

    /* Move forward until first digit or asterisk */
    ଯେତେବେଳେ(*ptr && !ISDIGIT(*ptr) && *ptr != '*')
      ptr++;

    /* if it truly stopped on a digit */
    ଯଦି(ISDIGIT(*ptr)) {
      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&ptr, &k->offset, CURL_OFF_T_MAX) &&
         (data->state.resume_from == k->offset))
        /* we asked for a resume and we got it */
        k->content_range = TRUE;
    }
    ଅନ୍ୟଥା ଯଦି(k->httpcode < 300)
      data->state.resume_from = 0; /* get everything */
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * http_header_l() parses a single response header starting with L.
 */
ସ୍ଥିର CURLcode http_header_l(ସଂରଚନା Curl_easy *data,
                              ଅଚଳ ଅକ୍ଷର *hd, size_t hdlen)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା SingleRequest *k = &data->req;
  ଅଚଳ ଅକ୍ଷର *v = (!k->http_bodyless &&
                   (data->set.timecondition || data->set.get_filetime)) ?
    HD_VAL(hd, hdlen, "Last-Modified:") : ଖାଲି;
  ଯଦି(v) {
    ଯଦି(Curl_getdate_capped(v, &k->timeofdoc))
      k->timeofdoc = 0;
    ଯଦି(data->set.get_filetime)
      data->info.filetime = k->timeofdoc;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଯଦି(HD_IS(hd, hdlen, "Location:")) {
    /* this is the URL that the server advises us to use instead */
    ଅକ୍ଷର *location = Curl_copy_header_ମୂଲ୍ୟ(hd);
    ଯଦି(!location)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    ଯଦି(!*location ||
       (data->req.location && !strcmp(data->req.location, location))) {
      /* ignore empty header, or exact repeat of a previous one */
      curlx_free(location);
      ଫେରନ୍ତୁ CURLE_OK;
    }
    ଅନ୍ୟଥା {
      /* has value and is not an exact repeat */
      ଯଦି(data->req.location) {
        failf(data, "Multiple Location headers");
        curlx_free(location);
        ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
      }
      data->req.location = location;

      ଯଦି((k->httpcode >= 300 && k->httpcode < 400) &&
         data->set.http_follow_mode) {
        CURLcode result;
        DEBUGASSERT(!data->req.newurl);
        data->req.newurl = curlx_strdup(data->req.location); /* clone */
        ଯଦି(!data->req.newurl)
          ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

        /* some cases of POST and PUT etc needs to rewind the data
           stream at this point */
        result = http_perhapsrewind(data, conn);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;

        /* mark the next request as a followed location: */
        data->state.this_is_a_follow = TRUE;
      }
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * http_header_p() parses a single response header starting with P.
 */
ସ୍ଥିର CURLcode http_header_p(ସଂରଚନା Curl_easy *data,
                              ଅଚଳ ଅକ୍ଷର *hd, size_t hdlen)
{
  ସଂରଚନା SingleRequest *k = &data->req;

#ifndef CURL_DISABLE_PROXY
  ଅଚଳ ଅକ୍ଷର *v = HD_VAL(hd, hdlen, "Proxy-Connection:");
  ଯଦି(v) {
    ସଂରଚନା connectdata *conn = data->conn;
    ଯଦି((k->httpversion == 10) && conn->http_proxy.peer &&
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
    ଅନ୍ୟଥା ଯଦି((k->httpversion == 11) && conn->http_proxy.peer &&
            HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "close")) {
      /*
       * We get an HTTP/1.1 response from a proxy and it says it will
       * close down after this transfer.
       */
      connclose(conn);
      infof(data, "HTTP/1.1 proxy connection set close");
    }
    ଫେରନ୍ତୁ CURLE_OK;
  }
#endif
  ଯଦି((407 == k->httpcode) && HD_IS(hd, hdlen, "Proxy-authenticate:")) {
    ଅକ୍ଷର *auth;
    CURLcode result;
#ifndef CURL_DISABLE_PROXY
    ଯଦି(!data->conn->bits.origin_is_proxy)
      /* this response is not from a forward proxy, not for us */
      ଫେରନ୍ତୁ CURLE_OK;
#endif
    auth = Curl_copy_header_ମୂଲ୍ୟ(hd);
    result = auth ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    ଯଦି(!result) {
      result = Curl_http_ନିବେଶ_auth(data, TRUE, auth);
      curlx_free(auth);
    }
    ଫେରନ୍ତୁ result;
  }
#ifdef USE_SPNEGO
  ଯଦି(HD_IS(hd, hdlen, "Persistent-Auth:")) {
    ସଂରଚନା connectdata *conn = data->conn;
    ସଂରଚନା negotiatedata *negdata = Curl_auth_nego_get(conn, FALSE);
    ସଂରଚନା auth *authp = &data->state.authhost;
    ଯଦି(!negdata)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    ଯଦି(authp->picked == CURLAUTH_NEGOTIATE) {
      ଅକ୍ଷର *persistentauth = Curl_copy_header_ମୂଲ୍ୟ(hd);
      ଯଦି(!persistentauth)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      negdata->noauthpersist = !!checkprefix("false", persistentauth);
      negdata->havenoauthpersist = TRUE;
      infof(data, "Negotiate: noauthpersist -> %d, header part: %s",
            negdata->noauthpersist, persistentauth);
      curlx_free(persistentauth);
    }
  }
#endif
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * http_header_r() parses a single response header starting with R.
 */
ସ୍ଥିର CURLcode http_header_r(ସଂରଚନା Curl_easy *data,
                              ଅଚଳ ଅକ୍ଷର *hd, size_t hdlen)
{
  ଅଚଳ ଅକ୍ଷର *v = HD_VAL(hd, hdlen, "Retry-After:");
  ଯଦି(v) {
    /* Retry-After = HTTP-date / delay-seconds */
    curl_off_t retry_after = 0; /* zero for unknown or "now" */
    time_t date = 0;
    curlx_str_passblanks(&v);

    /* try it as a date first, because a date can otherwise start with and
       get treated as a number */
    ଯଦି(!Curl_getdate_capped(v, &date)) {
      time_t current = time(ଖାଲି);
      ଯଦି(date >= current)
        /* convert date to number of seconds into the future */
        retry_after = date - current;
    }
    ଅନ୍ୟଥା
      /* Try it as a decimal number, ignore errors */
      (ଶୂନ୍ୟ)curlx_str_ସଂଖ୍ୟା(&v, &retry_after, CURL_OFF_T_MAX);
    /* limit to 6 hours max. this is not documented so that it can be changed
       in the future if necessary. */
    ଯଦି(retry_after > 21600)
      retry_after = 21600;
    data->info.retry_after = retry_after;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * http_header_s() parses a single response header starting with S.
 */
ସ୍ଥିର CURLcode http_header_s(ସଂରଚନା Curl_easy *data,
                              ଅଚଳ ଅକ୍ଷର *hd, size_t hdlen)
{
#ଯଦି !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_HSTS)
  ଅଚଳ ଅକ୍ଷର *v;
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)hd;
  (ଶୂନ୍ୟ)hdlen;
#endif

#ifndef CURL_DISABLE_COOKIES
  v = (data->cookies && data->state.cookie_engine) ?
    HD_VAL(hd, hdlen, "Set-Cookie:") : ଖାଲି;
  ଯଦି(v) {
    /* If there is a custom-set Host: name, use it here, or else use
     * real peer hostname. */
    ଅଚଳ ଅକ୍ଷର *host = data->req.cookiehost ?
      data->req.cookiehost : data->state.origin->hostname;
    ଅଚଳ unsigned ଅକ୍ଷର secure_context = Curl_secure_context(data, host) ?
      COOKIE_SECURE : 0;
    CURLcode result;
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    result = Curl_cookie_add(data, data->cookies, v, host,
                             data->state.up.path,
                             COOKIE_HTTPHEADER | secure_context);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    ଫେରନ୍ତୁ result;
  }
#endif
#ifndef CURL_DISABLE_HSTS
  /* If enabled, the header is incoming and this is over HTTPS */
  v = (data->hsts &&
       (Curl_xfer_is_secure(data) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_HSTS_HTTP")
#ଅନ୍ୟଥା
        0
#endif
         )
    ) ? HD_VAL(hd, hdlen, "Strict-Transport-Security:") : ଖାଲି;
  ଯଦି(v) {
    CURLcode result = Curl_hsts_parse(
      data->hsts, data->state.origin->hostname, v);
    ଯଦି(result) {
      ଯଦି(result == CURLE_OUT_OF_MEMORY)
        ଫେରନ୍ତୁ result;
      infof(data, "Illegal STS header skipped");
    }
#ifdef DEBUGBUILD
    ଅନ୍ୟଥା
      infof(data, "Parsed STS header fine (%zu entries)",
            Curl_llist_ଗଣନା(&data->hsts->list));
#endif
  }
#endif

  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * http_header_t() parses a single response header starting with T.
 */
ସ୍ଥିର CURLcode http_header_t(ସଂରଚନା Curl_easy *data,
                              ଅଚଳ ଅକ୍ଷର *hd, size_t hdlen)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା SingleRequest *k = &data->req;

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
  ଅଚଳ ଅକ୍ଷର *v = (!k->http_bodyless &&
                   (data->state.httpreq != HTTPREQ_HEAD) &&
                   (k->httpcode != 304)) ?
    HD_VAL(hd, hdlen, "Transfer-Encoding:") : ଖାଲି;
  ଯଦି(v) {
    /* One or more encodings. We check for chunked and/or a compression
       algorithm. */
    CURLcode result = Curl_build_unencoding_ଗଦା(data, v, TRUE);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    ଯଦି(!k->chunk) {
      /* if this is not chunked, only close can signal the end of this
       * transfer as Content-Length is said not to be trusted for
       * transfer-encoding! */
      CURL_TRC_M(data, "HTTP/1.1 transfer-encoding without chunks");
      connclose(conn);
      k->ignore_cl = TRUE;
    }
    ଫେରନ୍ତୁ CURLE_OK;
  }
  v = HD_VAL(hd, hdlen, "Trailer:");
  ଯଦି(v) {
    data->req.resp_trailer = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * http_header_w() parses a single response header starting with W.
 */
ସ୍ଥିର CURLcode http_header_w(ସଂରଚନା Curl_easy *data,
                              ଅଚଳ ଅକ୍ଷର *hd, size_t hdlen)
{
  ସଂରଚନା SingleRequest *k = &data->req;
  CURLcode result = CURLE_OK;

  ଯଦି((401 == k->httpcode) && HD_IS(hd, hdlen, "WWW-Authenticate:")) {
    ଅକ୍ଷର *auth = Curl_copy_header_ମୂଲ୍ୟ(hd);
    ଯଦି(!auth)
      result = CURLE_OUT_OF_MEMORY;
    ଅନ୍ୟଥା {
      result = Curl_http_ନିବେଶ_auth(data, FALSE, auth);
      curlx_free(auth);
    }
  }
  ଫେରନ୍ତୁ result;
}

/*
 * http_header() parses a single response header.
 */
ସ୍ଥିର CURLcode http_header(ସଂରଚନା Curl_easy *data,
                            ଅଚଳ ଅକ୍ଷର *hd, size_t hdlen)
{
  CURLcode result = CURLE_OK;

  ବାଛ(hd[0]) {
  ସ୍ଥିତି 'a':
  ସ୍ଥିତି 'A':
    result = http_header_a(data, hd, hdlen);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'c':
  ସ୍ଥିତି 'C':
    result = http_header_c(data, hd, hdlen);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'l':
  ସ୍ଥିତି 'L':
    result = http_header_l(data, hd, hdlen);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'p':
  ସ୍ଥିତି 'P':
    result = http_header_p(data, hd, hdlen);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'r':
  ସ୍ଥିତି 'R':
    result = http_header_r(data, hd, hdlen);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 's':
  ସ୍ଥିତି 'S':
    result = http_header_s(data, hd, hdlen);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 't':
  ସ୍ଥିତି 'T':
    result = http_header_t(data, hd, hdlen);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'w':
  ସ୍ଥିତି 'W':
    result = http_header_w(data, hd, hdlen);
    ଭାଙ୍ଗ;
  }

  ଯଦି(!result) {
    ସଂରଚନା connectdata *conn = data->conn;
    ଯଦି(conn->scheme->protocol & CURLPROTO_RTSP)
      result = Curl_rtsp_parseheader(data, hd);
  }
  ଫେରନ୍ତୁ result;
}

/*
 * Called after the first HTTP response line (the status line) has been
 * received and parsed.
 */
ସ୍ଥିର CURLcode http_statusline(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା connectdata *conn)
{
  ସଂରଚନା SingleRequest *k = &data->req;

  ବାଛ(k->httpversion) {
  ସ୍ଥିତି 10:
  ସ୍ଥିତି 11:
#ifdef USE_HTTP2
  ସ୍ଥିତି 20:
#endif
#ifdef USE_HTTP3
  ସ୍ଥିତି 30:
#endif
    /* no major version switch mid-connection */
    ଯଦି(k->httpversion_sent &&
       (k->httpversion / 10 != k->httpversion_sent / 10)) {
      failf(data, "Version mismatch (from HTTP/%d to HTTP/%d)",
            k->httpversion_sent / 10, k->httpversion / 10);
      ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
    }
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    failf(data, "Unsupported HTTP version (%d.%d) in response",
          k->httpversion / 10, k->httpversion % 10);
    ଫେରନ୍ତୁ CURLE_UNSUPPORTED_PROTOCOL;
  }

  data->info.httpcode = k->httpcode;
  data->info.httpversion = k->httpversion;
  conn->httpversion_seen = k->httpversion;

  ଯଦି(!data->state.http_neg.rcvd_min ||
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
  ଯଦି(data->state.resume_from && data->state.httpreq == HTTPREQ_GET &&
     k->httpcode == 416) {
    /* "Requested Range Not Satisfiable", proceed and pretend this is no
       error */
    k->ignorebody = TRUE; /* Avoid appending error msg to good data. */
  }

  ଯଦି(k->httpversion == 10) {
    /* Default action for HTTP/1.0 must be to close, unless
       we get one of those fancy headers that tell us the
       server keeps it open for us! */
    infof(data, "HTTP 1.0, assume close after body");
    connclose(conn);
  }

  k->http_bodyless = k->httpcode >= 100 && k->httpcode < 200;
  ବାଛ(k->httpcode) {
  ସ୍ଥିତି 304:
    /* (quote from RFC2616, section 10.3.5): The 304 response
     * MUST NOT contain a message-body, and thus is always
     * terminated by the first empty line after the header
     * fields. */
    ଯଦି(data->set.timecondition)
      data->info.timecond = TRUE;
    FALLTHROUGH();
  ସ୍ଥିତି 204:
    /* (quote from RFC2616, section 10.2.5): The server has
     * fulfilled the request but does not need to return an
     * entity-body ... The 204 response MUST NOT include a
     * message-body, and thus is always terminated by the first
     * empty line after the header fields. */
    k->size = 0;
    k->maxdownload = 0;
    k->http_bodyless = TRUE;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/* Content-Length must be ignored if any Transfer-Encoding is present in the
   response. Refer to RFC 7230 section 3.3.3 and RFC2616 section 4.4. This is
   figured out here after all headers have been received but before the final
   call to the user's header callback, so that a valid content length can be
   retrieved by the user in the final call. */
ସ୍ଥିର CURLcode http_size(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା SingleRequest *k = &data->req;
  ଯଦି(data->req.ignore_cl || k->chunk) {
    k->size = k->maxdownload = -1;
  }
  ଅନ୍ୟଥା ଯଦି(k->size != -1) {
    ଯଦି(data->set.max_filesize &&
       !k->ignorebody &&
       (k->size > data->set.max_filesize)) {
      failf(data, "Maximum file size exceeded");
      ଫେରନ୍ତୁ CURLE_FILESIZE_EXCEEDED;
    }
    ଯଦି(k->ignorebody)
      infof(data, "setting size while ignoring");
    Curl_pgrsSetDownloadSize(data, k->size);
    k->maxdownload = k->size;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_verify_header(ସଂରଚନା Curl_easy *data,
                            ଅଚଳ ଅକ୍ଷର *hd, size_t hdlen)
{
  ସଂରଚନା SingleRequest *k = &data->req;
  size_t i;
  bool has_colon = FALSE;
  ପାଇଁ(i = 0; i < hdlen; i++) {
    ବାଛ(hd[i]) {
    ସ୍ଥିତି '\0':
      /* this is bad, bail out */
      failf(data, "NUL byte in header");
      ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
    ସ୍ଥିତି '\r':
      ଯଦି(i < hdlen - 2) {
        /* CR may only precede the LF, nothing else */
        failf(data, "Carriage return found in header");
        ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି ':':
      has_colon = TRUE;
      ଭାଙ୍ଗ;
    }
  }
  ଯଦି(k->headerline < 2)
    /* the first "header" is the status-line and it has no colon */
    ଫେରନ୍ତୁ CURLE_OK;
  ଯଦି(((hd[0] == ' ') || (hd[0] == '\t')) && k->headerline > 2)
    /* line folding, cannot happen on line 2 */
    ;
  ଅନ୍ୟଥା ଯଦି(!has_colon) {
    /* this is bad, bail out */
    failf(data, "Header without colon");
    ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_bump_headersize(ସଂରଚନା Curl_easy *data,
                              size_t delta,
                              bool connect_only)
{
  size_t bad = 0;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା max = MAX_HTTP_RESP_HEADER_SIZE;
  ଯଦି(delta < MAX_HTTP_RESP_HEADER_SIZE) {
    data->info.header_size += (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)delta;
    data->req.allheadercount += (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)delta;
    ଯଦି(!connect_only)
      data->req.headerbytecount += (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)delta;
    ଯଦି(data->req.allheadercount > max)
      bad = data->req.allheadercount;
    ଅନ୍ୟଥା ଯଦି(data->info.header_size > (max * 20)) {
      bad = data->info.header_size;
      max *= 20;
    }
  }
  ଅନ୍ୟଥା
    bad = data->req.allheadercount + delta;
  ଯଦି(bad) {
    failf(data, "Too large response headers: %zu > %u", bad, max);
    ଫେରନ୍ତୁ CURLE_RECV_ERROR;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Handle a 101 Switching Protocols response. Performs the actual protocol
 * upgrade to HTTP/2 or WebSocket based on what was requested.
 */
ସ୍ଥିର CURLcode http_on_101_upgrade(ସଂରଚନା Curl_easy *data,
                                    ଅଚଳ ଅକ୍ଷର *buf, size_t blen,
                                    size_t *pconsumed,
                                    bool *conn_changed)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା SingleRequest *k = &data->req;

#ଯଦି !defined(USE_NGHTTP2) && defined(CURL_DISABLE_WEBSOCKETS)
  (ଶୂନ୍ୟ)buf;
  (ଶୂନ୍ୟ)blen;
  (ଶୂନ୍ୟ)pconsumed;
#ଅନ୍ୟଥା
  CURLcode result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା upgr101_requested = k->upgr101;
#endif

  ଯଦି(k->httpversion_sent != 11) {
    /* invalid for other HTTP versions */
    failf(data, "server sent 101 response while not talking HTTP/1.1");
    ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
  }

  /* Whatever the success, upgrade was selected. */
  k->upgr101 = UPGR101_RECEIVED;
  conn->bits.upgrade_in_progress = FALSE;
  *conn_changed = TRUE;

  /* To be fully compliant, we would check the "Upgrade:" response header to
   * mention the protocol we requested. */
#ifdef USE_NGHTTP2
  ଯଦି(upgr101_requested == UPGR101_H2) {
    /* Switch to HTTP/2, where we will get more responses. blen bytes in buf
     * are already h2 protocol bytes */
    infof(data, "Received 101, Switching to HTTP/2");
    result = Curl_http2_upgrade(data, conn, FIRSTSOCKET, buf, blen);
    ଯଦି(!result)
      *pconsumed += blen;
    ଫେରନ୍ତୁ result;
  }
#endif
#ifndef CURL_DISABLE_WEBSOCKETS
  ଯଦି(upgr101_requested == UPGR101_WS) {
    /* Switch to WebSocket, where we now stream ws frames. blen bytes in buf
     * are already ws protocol bytes */
    infof(data, "Received 101, Switching to WebSocket");
    result = Curl_ws_accept(data, buf, blen);
    ଯଦି(!result)
      *pconsumed += blen; /* ws accept handled the data */
    ଫେରନ୍ତୁ result;
  }
#endif
  /* We silently accept this as the final response. What are we switching to
   * if we did not ask for an Upgrade? Maybe the application provided an
   * `Upgrade: xxx` header? */
  k->header = FALSE;
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Handle 1xx intermediate HTTP responses. Sets up state for more
 * headers and processes 100-continue and 101 upgrade responses.
 */
ସ୍ଥିର CURLcode http_on_1xx_response(ସଂରଚନା Curl_easy *data,
                                     ଅଚଳ ଅକ୍ଷର *buf, size_t blen,
                                     size_t *pconsumed,
                                     bool *conn_changed)
{
  ସଂରଚନା SingleRequest *k = &data->req;

  /* "A user agent MAY ignore unexpected 1xx status responses."
   * By default, we expect to get more responses after this one. */
  k->header = TRUE;
  k->headerline = 0; /* restart the header line counter */

  ବାଛ(k->httpcode) {
  ସ୍ଥିତି 100:
    /* We have made an HTTP PUT or POST and this is 1.1-lingo that tells us
     * that the server is OK with this and ready to receive the data. */
    http_exp100_got100(data);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 101:
    ଫେରନ୍ତୁ http_on_101_upgrade(data, buf, blen, pconsumed, conn_changed);
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    /* The server may send us other 1xx responses, like informative 103. This
     * has no influence on request processing and we expect to receive a
     * final response eventually. */
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

#ଯଦି defined(USE_NTLM) || defined(USE_SPNEGO)
/*
 * Check if NTLM or SPNEGO authentication negotiation failed due to
 * connection closure (typically on HTTP/1.0 servers).
 */
ସ୍ଥିର ଶୂନ୍ୟ http_check_auth_closure(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା connectdata *conn)
{
  /* At this point we have some idea about the fate of the connection. If we
     are closing the connection it may result auth failure. */
#ifdef USE_NTLM
  ଯଦି(conn->bits.close &&
     (((data->req.httpcode == 401) &&
       (conn->http_ntlm_state == NTLMSTATE_TYPE2)) ||
      ((data->req.httpcode == 407) &&
       (conn->proxy_ntlm_state == NTLMSTATE_TYPE2)))) {
    infof(data, "Connection closure while negotiating auth (HTTP 1.0?)");
    data->state.authproblem = TRUE;
  }
#endif
#ifdef USE_SPNEGO
  ଯଦି(conn->bits.close &&
    (((data->req.httpcode == 401) &&
      (conn->http_negotiate_state == CURL_NEGO_AUTH_RECV)) ||
     ((data->req.httpcode == 407) &&
      (conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)))) {
    infof(data, "Connection closure while negotiating auth (HTTP 1.0?)");
    data->state.authproblem = TRUE;
  }
  ଯଦି((conn->http_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (data->req.httpcode != 401)) {
    conn->http_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
  ଯଦି((conn->proxy_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (data->req.httpcode != 407)) {
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
#endif
}
#ଅନ୍ୟଥା
#define http_check_auth_closure(x, y) /* empty */
#endif

/*
 * Handle an error response (>= 300) received while still sending the
 * request body. Deals with 417 Expectation Failed retries, keep-sending
 * on error, and aborting the send.
 */
ସ୍ଥିର CURLcode http_handle_send_error(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା SingleRequest *k = &data->req;
  CURLcode result = CURLE_OK;

  ଯଦି(!data->req.authneg && !conn->bits.close &&
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

    ବାଛ(data->state.httpreq) {
    ସ୍ଥିତି HTTPREQ_PUT:
    ସ୍ଥିତି HTTPREQ_POST:
    ସ୍ଥିତି HTTPREQ_POST_FORM:
    ସ୍ଥିତି HTTPREQ_POST_MIME:
      /* We got an error response. If this happened before the
       * whole request body has been sent we stop sending and
       * mark the connection for closure after we have read the
       * entire response. */
      ଯଦି(!Curl_req_done_sending(data)) {
        ଯଦି((k->httpcode == 417) && http_exp100_is_selected(data)) {
          /* 417 Expectation Failed - try again without the
             Expect header */
          ଯଦି(!k->writebytecount && http_exp100_is_waiting(data)) {
            infof(data, "Got HTTP failure 417 while waiting for a 100");
          }
          ଅନ୍ୟଥା {
            infof(data, "Got HTTP failure 417 while sending data");
            streamclose(conn);
            result = http_perhapsrewind(data, conn);
            ଯଦି(result)
              ଫେରନ୍ତୁ result;
          }
          data->state.disableexpect = TRUE;
          Curl_req_abort_sending(data);
          DEBUGASSERT(!data->req.newurl);
          data->req.newurl = Curl_bufref_dup(&data->state.url);
          ଯଦି(!data->req.newurl)
            ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
        }
        ଅନ୍ୟଥା ଯଦି(data->set.http_keep_sending_on_error) {
          infof(data, "HTTP error before end of send, keep sending");
          http_exp100_send_anyway(data);
        }
        ଅନ୍ୟଥା {
          infof(data, "HTTP error before end of send, stop sending");
          streamclose(conn);
          result = Curl_req_abort_sending(data);
          ଯଦି(result)
            ଫେରନ୍ତୁ result;
        }
      }
      ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* default label present to avoid compiler warnings */
      ଭାଙ୍ଗ;
    }
  }

  ଯଦି(Curl_creader_will_rewind(data) && !Curl_req_done_sending(data)) {
    /* We rewind before next send, continue sending now */
    infof(data, "Keep sending data to get tossed away");
    CURL_REQ_SET_SEND(data);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode http_on_response(ସଂରଚନା Curl_easy *data,
                                 ଅଚଳ ଅକ୍ଷର *last_hd, size_t last_hd_len,
                                 ଅଚଳ ଅକ୍ଷର *buf, size_t blen,
                                 size_t *pconsumed)
{
  ସଂରଚନା connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  ସଂରଚନା SingleRequest *k = &data->req;
  bool conn_changed = FALSE;

  (ଶୂନ୍ୟ)buf; /* not used without HTTP2 enabled */
  *pconsumed = 0;

  ଯଦି(k->upgr101 == UPGR101_RECEIVED) {
    /* supposedly upgraded to http2 now */
    ଯଦି(data->req.httpversion != 20)
      infof(data, "Lying server, not serving HTTP/2");
  }

  ଯଦି(k->httpcode < 200 && last_hd) {
    /* Intermediate responses might trigger processing of more responses,
     * write the last header to the client before proceeding. */
    result = http_write_header(data, last_hd, last_hd_len);
    last_hd = ଖାଲି; /* handled it */
    ଯଦି(result)
      ଯାଅ out;
  }

  ଯଦି(k->httpcode < 100) {
    failf(data, "Unsupported response code in HTTP response");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    ଯାଅ out;
  }
  ଅନ୍ୟଥା ଯଦି(k->httpcode < 200) {
    result = http_on_1xx_response(data, buf, blen, pconsumed, &conn_changed);
    ଯାଅ out;
  }

  /* k->httpcode >= 200, final response */
  k->header = FALSE;
  ଯଦି(conn->bits.upgrade_in_progress) {
    /* Asked for protocol upgrade, but it was not selected */
    conn->bits.upgrade_in_progress = FALSE;
    conn_changed = TRUE;
  }

  ଯଦି((k->size == -1) && !k->chunk && !conn->bits.close &&
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
  ଯଦି(data->req.upgr101 == UPGR101_WS) {
    failf(data, "Refused WebSocket upgrade: %d", k->httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
    ଯାଅ out;
  }
#endif

  /* Check if this response means the transfer errored. */
  ଯଦି(http_should_fail(data, data->req.httpcode)) {
    failf(data, "The requested URL returned error: %d",
          k->httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
    ଯାଅ out;
  }

  /* Curl_http_auth_act() checks what authentication methods that are
   * available and decides which one (if any) to use. It will set 'newurl' if
   * an auth method was picked. */
  result = Curl_http_auth_act(data);
  ଯଦି(result)
    ଯାଅ out;

  ଯଦି(k->httpcode >= 300) {
    result = http_handle_send_error(data);
    ଯଦି(result)
      ଯାଅ out;
  }

  /* final response without error, prepare to receive the body */
  result = http_firstwrite(data);
  ଯଦି(result)
    ଯାଅ out;

  /* This is the last response that we get for the current request. Check on
   * the body size and determine if the response is complete. */
  result = http_size(data);
  ଯଦି(result)
    ଯାଅ out;

#ifndef CURL_DISABLE_RTSP
  /* An RTSP response to any method may carry a body, which is announced
     with Content-Length (RFC 2326 section 12.14). Read it instead of
     treating it as excess data, unless the application asked for no body
     with CURLOPT_NOBODY. */
  ଯଦି((conn->scheme->protocol & CURLPROTO_RTSP) && data->req.no_body &&
     !data->set.opt_no_body && (k->size > 0))
    data->req.no_body = FALSE;
#endif

  /* If we requested a "no body", this is a good time to get
   * out and return home.
   */
  ଯଦି(data->req.no_body)
    k->download_done = TRUE;

  /* If max download size is *zero* (nothing) we already have nothing and can
     safely return ok now! For HTTP/2, we would like to call
     http2_handle_stream_close to properly close a stream. In order to do
     this, we keep reading until we close the stream. */
  ଯଦି((k->maxdownload == 0) && (k->httpversion_sent < 20))
    k->download_done = TRUE;

out:
  ଯଦି(last_hd)
    /* if not written yet, write it now */
    result = Curl_1st_fatal(result,
                            http_write_header(data, last_hd, last_hd_len));
  ଯଦି(conn_changed)
    /* poke the multi handle to allow pending pipewait to retry */
    Curl_multi_connchanged(data->multi);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode http_rw_hd(ସଂରଚନା Curl_easy *data,
                           ଅଚଳ ଅକ୍ଷର *hd, size_t hdlen,
                           ଅଚଳ ଅକ୍ଷର *buf_remain, size_t blen,
                           size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା SingleRequest *k = &data->req;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା writetype;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  *pconsumed = 0;
  ଯଦି((0x0a == *hd) || (0x0d == *hd)) {
    /* Empty header line means end of headers! */
    ସଂରଚନା dynbuf last_header;
    size_t consumed;

    curlx_dyn_init(&last_header, hdlen + 1);
    result = curlx_dyn_addn(&last_header, hd, hdlen);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;

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
    ଫେରନ୍ତୁ result;
  }

  /*
   * Checks for special headers coming up.
   */

  writetype = CLIENTWRITE_HEADER;
  ଯଦି(!k->headerline++) {
    /* This is the first header, it MUST be the error code line
       or else we consider this to be the body right away! */
    bool fine_statusline = FALSE;

    k->httpversion = 0; /* Do not know yet */
    ଯଦି(data->conn->scheme->protocol & PROTO_FAMILY_HTTP) {
      /*
       * https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.2
       *
       * The response code is always a three-digit number in HTTP as the spec
       * says. We allow any three-digit number here, but we cannot make
       * guarantees on future behaviors since it is not within the protocol.
       */
      ଅଚଳ ଅକ୍ଷର *p = hd;

      curlx_str_passblanks(&p);
      ଯଦି(!strncmp(p, "HTTP/", 5)) {
        p += 5;
        ବାଛ(*p) {
        ସ୍ଥିତି '1':
          p++;
          ଯଦି((p[0] == '.') && (p[1] == '0' || p[1] == '1')) {
            ଯଦି(ISBLANK(p[2])) {
              k->httpversion = (unsigned ଅକ୍ଷର)(10 + (p[1] - '0'));
              p += 3;
              ଯଦି(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
                k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
                  (p[2] - '0');
                /* RFC 9112 requires a single space following the status code,
                   but the browsers do not so let's not insist */
                fine_statusline = TRUE;
              }
            }
          }
          ଯଦି(!fine_statusline) {
            failf(data, "Unsupported HTTP/1 subversion in response");
            ଫେରନ୍ତୁ CURLE_UNSUPPORTED_PROTOCOL;
          }
          ଭାଙ୍ଗ;
        ସ୍ଥିତି '2':
        ସ୍ଥିତି '3':
          ଯଦି(!ISBLANK(p[1]))
            ଭାଙ୍ଗ;
          k->httpversion = (unsigned ଅକ୍ଷର)((*p - '0') * 10);
          p += 2;
          ଯଦି(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
            k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
              (p[2] - '0');
            p += 3;
            ଯଦି(!ISBLANK(*p))
              ଭାଙ୍ଗ;
            fine_statusline = TRUE;
          }
          ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* unsupported */
          failf(data, "Unsupported HTTP version in response");
          ଫେରନ୍ତୁ CURLE_UNSUPPORTED_PROTOCOL;
        }
      }

      ଯଦି(!fine_statusline) {
        /* If user has set option HTTP200ALIASES,
           compare header line against list of aliases */
        statusline check = checkhttpprefix(data, hd, hdlen);
        ଯଦି(check == STATUS_DONE) {
          fine_statusline = TRUE;
          k->httpcode = 200;
          k->httpversion = 10;
        }
      }
    }
    ଅନ୍ୟଥା ଯଦି(data->conn->scheme->protocol & CURLPROTO_RTSP) {
      ଅଚଳ ଅକ୍ଷର *p = hd;
      ସଂରଚନା Curl_str ver;
      curl_off_t status;
      /* we set the max string a little excessive to forgive some leading
         spaces */
      ଯଦି(!curlx_str_until(&p, &ver, 32, ' ') &&
         !curlx_str_single(&p, ' ') &&
         !curlx_str_ସଂଖ୍ୟା(&p, &status, 999)) {
        curlx_str_trimblanks(&ver);
        ଯଦି(curlx_str_cmp(&ver, "RTSP/1.0")) {
          k->httpcode = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)status;
          fine_statusline = TRUE;
          k->httpversion = 11; /* RTSP acts like HTTP 1.1 */
        }
      }
      ଯଦି(!fine_statusline)
        ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
    }

    ଯଦି(fine_statusline) {
      result = http_statusline(data, data->conn);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      writetype |= CLIENTWRITE_STATUS;
    }
    ଅନ୍ୟଥା {
      k->header = FALSE;   /* this is not a header line */
      ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
    }
  }

  result = Curl_verify_header(data, hd, hdlen);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  result = http_header(data, hd, hdlen);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /*
   * Taken in one (more) header. Write it to the client.
   */
  Curl_debug(data, CURLINFO_HEADER_IN, hd, hdlen);

  ଯଦି(k->httpcode / 100 == 1)
    writetype |= CLIENTWRITE_1XX;
  result = Curl_client_write(data, writetype, hd, hdlen);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  result = Curl_bump_headersize(data, hdlen, FALSE);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ଫେରନ୍ତୁ CURLE_OK;
}

/* remove trailing CRLF then all trailing whitespace */
ଶୂନ୍ୟ Curl_http_to_fold(ସଂରଚନା dynbuf *bf)
{
  size_t len = curlx_dyn_len(bf);
  ଅଚଳ ଅକ୍ଷର *hd = curlx_dyn_ptr(bf);
  ଯଦି(len && (hd[len - 1] == '\n'))
    len--;
  ଯଦି(len && (hd[len - 1] == '\r'))
    len--;
  ଯେତେବେଳେ(len && ISBLANK(hd[len - 1])) /* strip off trailing whitespace */
    len--;
  curlx_dyn_setlen(bf, len);
}

ସ୍ଥିର ଶୂନ୍ୟ unfold_header(ସଂରଚନା Curl_easy *data)
{
  Curl_http_to_fold(&data->state.headerb);
  data->state.leading_unfold = TRUE;
}

/*
 * Read any HTTP header lines from the server and pass them to the client app.
 */
ସ୍ଥିର CURLcode http_parse_headers(ସଂରଚନା Curl_easy *data,
                                   ଅଚଳ ଅକ୍ଷର *buf, size_t blen,
                                   size_t *pconsumed)
{
  ସଂରଚନା connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  ସଂରଚନା SingleRequest *k = &data->req;
  ଅଚଳ ଅକ୍ଷର *end_ptr;
  bool leftover_body = FALSE;

  /* we have bytes for the next header, make sure it is not a folded header
     before passing it on */
  ଯଦି(data->state.maybe_folded && blen) {
    ଯଦି(ISBLANK(buf[0])) {
      /* folded, remove the trailing newlines and append the next header */
      unfold_header(data);
    }
    ଅନ୍ୟଥା {
      /* the header data we hold is a complete header, pass it on */
      size_t ignore_this;
      result = http_rw_hd(data, curlx_dyn_ptr(&data->state.headerb),
                          curlx_dyn_len(&data->state.headerb),
                          ଖାଲି, 0, &ignore_this);
      curlx_dyn_reset(&data->state.headerb);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
    data->state.maybe_folded = FALSE;
  }

  /* header line within buffer loop */
  *pconsumed = 0;
  ଯେତେବେଳେ(blen && k->header) {
    size_t consumed;
    size_t hlen;
    ଅଚଳ ଅକ୍ଷର *hd;
    size_t unfold_len = 0;

    ଯଦି(data->state.leading_unfold) {
      /* immediately after an unfold, keep only a single whitespace */
      ଯେତେବେଳେ(blen && ISBLANK(buf[0])) {
        buf++;
        blen--;
        unfold_len++;
      }
      ଯଦି(blen) {
        /* insert a single space */
        result = curlx_dyn_addn(&data->state.headerb, " ", 1);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;
        data->state.leading_unfold = FALSE; /* done now */
      }
    }

    end_ptr = memchr(buf, '\n', blen);
    ଯଦି(!end_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      result = curlx_dyn_addn(&data->state.headerb, buf, blen);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      *pconsumed += blen + unfold_len;

      ଯଦି(!k->headerline) {
        /* check if this looks like a protocol header */
        statusline st =
          checkprotoprefix(data, conn,
                           curlx_dyn_ptr(&data->state.headerb),
                           curlx_dyn_len(&data->state.headerb));

        ଯଦି(st == STATUS_BAD) {
          /* this is not the beginning of a protocol first header line.
           * Cannot be 0.9 if version was detected or connection was reused. */
          k->header = FALSE;
          streamclose(conn);
          ଯଦି((k->httpversion >= 10) || conn->bits.reuse) {
            failf(data, "Invalid status line");
            ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
          }
          ଯଦି(!data->state.http_neg.accept_09) {
            failf(data, "Received HTTP/0.9 when not allowed");
            ଫେରନ୍ତୁ CURLE_UNSUPPORTED_PROTOCOL;
          }
          leftover_body = TRUE;
          ଯାଅ out;
        }
      }
      ଯାଅ out; /* read more and try again */
    }

    /* the size of the remaining header line */
    consumed = (end_ptr - buf) + 1;

    result = curlx_dyn_addn(&data->state.headerb, buf, consumed);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    blen -= consumed;
    buf += consumed;
    *pconsumed += consumed + unfold_len;

    /****
     * We now have a FULL header line in 'headerb'.
     *****/

    hlen = curlx_dyn_len(&data->state.headerb);
    hd = curlx_dyn_ptr(&data->state.headerb);

    ଯଦି(!k->headerline) {
      /* the first read "header", the status line */
      statusline st = checkprotoprefix(data, conn, hd, hlen);
      ଯଦି(st == STATUS_BAD) {
        streamclose(conn);
        /* this is not the beginning of a protocol first header line.
         * Cannot be 0.9 if version was detected or connection was reused. */
        ଯଦି((k->httpversion >= 10) || conn->bits.reuse) {
          failf(data, "Invalid status line");
          ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
        }
        ଯଦି(!data->state.http_neg.accept_09) {
          failf(data, "Received HTTP/0.9 when not allowed");
          ଫେରନ୍ତୁ CURLE_UNSUPPORTED_PROTOCOL;
        }
        k->header = FALSE;
        leftover_body = TRUE;
        ଯାଅ out;
      }
    }
    ଅନ୍ୟଥା {
      ଯଦି(hlen && !ISNEWLINE(hd[0])) {
        /* this is NOT the header separator */

        /* if we have bytes for the next header, check for folding */
        ଯଦି(blen && ISBLANK(buf[0])) {
          /* remove the trailing CRLF and append the next header */
          unfold_header(data);
          ଜାରି;
        }
        ଅନ୍ୟଥା ଯଦି(!blen) {
          /* this might be a folded header so deal with it in next invoke */
          data->state.maybe_folded = TRUE;
          ଭାଙ୍ଗ;
        }
      }
    }

    result = http_rw_hd(data, hd, hlen, buf, blen, &consumed);
    /* We are done with this line. We reset because response
     * processing might switch to HTTP/2 and that might call us
     * directly again. */
    curlx_dyn_reset(&data->state.headerb);
    ଯଦି(consumed) {
      blen -= consumed;
      buf += consumed;
      *pconsumed += consumed;
    }
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */
out:
  ଯଦି(!k->header && !leftover_body) {
    curlx_dyn_free(&data->state.headerb);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_http_write_resp_hd(ସଂରଚନା Curl_easy *data,
                                 ଅଚଳ ଅକ୍ଷର *hd, size_t hdlen,
                                 bool is_eos)
{
  CURLcode result;
  size_t consumed;
  ଅକ୍ଷର tmp = 0;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  result = http_rw_hd(data, hd, hdlen, &tmp, 0, &consumed);
  ଯଦି(!result && is_eos) {
    result = Curl_client_write(data, (CLIENTWRITE_BODY | CLIENTWRITE_EOS),
                               &tmp, 0);
  }
  ଫେରନ୍ତୁ result;
}

/*
 * HTTP protocol `write_resp` implementation. Parse headers
 * when not done yet and otherwise return without consuming data.
 */
CURLcode Curl_http_write_resp_hds(ସଂରଚନା Curl_easy *data,
                                  ଅଚଳ ଅକ୍ଷର *buf, size_t blen,
                                  size_t *pconsumed)
{
  ଯଦି(!data->req.header) {
    *pconsumed = 0;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଅନ୍ୟଥା {
    CURLcode result;

    result = http_parse_headers(data, buf, blen, pconsumed);
    ଯଦି(!result && !data->req.header) {
      ଯଦି(!data->req.no_body && curlx_dyn_len(&data->state.headerb)) {
        /* leftover from parsing something that turned out not
         * to be a header, only happens if we allow for
         * HTTP/0.9 like responses */
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   curlx_dyn_ptr(&data->state.headerb),
                                   curlx_dyn_len(&data->state.headerb));
      }
      curlx_dyn_free(&data->state.headerb);
    }
    ଫେରନ୍ତୁ result;
  }
}

CURLcode Curl_http_write_resp(ସଂରଚନା Curl_easy *data,
                              ଅଚଳ ଅକ୍ଷର *buf, size_t blen,
                              bool is_eos)
{
  CURLcode result;
  size_t consumed;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags;

  result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
  ଯଦି(result || data->req.done)
    ଯାଅ out;

  DEBUGASSERT(consumed <= blen);
  blen -= consumed;
  buf += consumed;
  /* either all was consumed in header parsing, or we have data left
   * and are done with headers, e.g. it is BODY data */
  DEBUGASSERT(!blen || !data->req.header);
  ଯଦି(!data->req.header && (blen || is_eos)) {
    /* BODY data after header been parsed, write and consume */
    flags = CLIENTWRITE_BODY;
    ଯଦି(is_eos)
      flags |= CLIENTWRITE_EOS;
    result = Curl_client_write(data, flags, buf, blen);
  }
out:
  ଫେରନ୍ତୁ result;
}

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_status(int32_t *pstatus, ଅଚଳ ଅକ୍ଷର *s, size_t len)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  int32_t status = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  ଯଦି(len != 3)
    ଯାଅ out;

  ପାଇଁ(i = 0; i < 3; ++i) {
    ଅକ୍ଷର c = s[i];

    ଯଦି(c < '0' || c > '9')
      ଯାଅ out;

    status *= 10;
    status += c - '0';
  }
  result = CURLE_OK;
out:
  *pstatus = result ? -1 : status;
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_http_req_make(ସଂରଚନା httpreq **preq,
                            ଅଚଳ ଅକ୍ଷର *method, size_t m_len,
                            ଅଚଳ ଅକ୍ଷର *scheme, size_t s_len,
                            ଅଚଳ ଅକ୍ଷର *authority, size_t a_len,
                            ଅଚଳ ଅକ୍ଷର *path, size_t p_len)
{
  ସଂରଚନା httpreq *req;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, ଆକାର(*req) + m_len);
  ଯଦି(!req)
    ଯାଅ out;
#ଯଦି defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic ଠେଲ
/* error: 'memcpy' offset [137, 142] from the object at 'req' is out of
   the bounds of referenced subobject 'method' with type 'char[1]' at
   offset 136 */
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
  memcpy(req->method, method, m_len);
#ଯଦି defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic କାଢ଼
#endif
  ଯଦି(scheme) {
    req->scheme = curlx_memdup0(scheme, s_len);
    ଯଦି(!req->scheme)
      ଯାଅ out;
  }
  ଯଦି(authority) {
    req->authority = curlx_memdup0(authority, a_len);
    ଯଦି(!req->authority)
      ଯାଅ out;
  }
  ଯଦି(path) {
    req->path = curlx_memdup0(path, p_len);
    ଯଦି(!req->path)
      ଯାଅ out;
  }
  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  ଯଦି(result && req)
    Curl_http_req_free(req);
  *preq = result ? ଖାଲି : req;
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode req_assign_url_authority(ସଂରଚନା httpreq *req, CURLU *url)
{
  ଅକ୍ଷର *host, *port;
  ସଂରଚନା dynbuf buf;
  CURLUcode uc;
  CURLcode result = CURLE_URL_MALFORMAT;

  host = port = ଖାଲି;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_HOST, &host, 0);
  ଯଦି(uc && uc != CURLUE_NO_HOST)
    ଯାଅ out;
  ଯଦି(!host) {
    req->authority = ଖାଲି;
    result = CURLE_OK;
    ଯାଅ out;
  }

  uc = curl_url_get(url, CURLUPART_PORT, &port, CURLU_NO_DEFAULT_PORT);
  ଯଦି(uc && uc != CURLUE_NO_PORT)
    ଯାଅ out;

  result = curlx_dyn_add(&buf, host);
  ଯଦି(result)
    ଯାଅ out;
  ଯଦି(port) {
    result = curlx_dyn_addf(&buf, ":%s", port);
    ଯଦି(result)
      ଯାଅ out;
  }
  req->authority = curlx_dyn_ptr(&buf);
out:
  curlx_free(host);
  curlx_free(port);
  ଯଦି(result)
    curlx_dyn_free(&buf);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode req_assign_url_path(ସଂରଚନା httpreq *req, CURLU *url)
{
  ଅକ୍ଷର *path, *query;
  ସଂରଚନା dynbuf buf;
  CURLUcode uc;
  CURLcode result = CURLE_URL_MALFORMAT;

  path = query = ଖାଲି;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_PATH, &path, 0);
  ଯଦି(uc)
    ଯାଅ out;
  uc = curl_url_get(url, CURLUPART_QUERY, &query, 0);
  ଯଦି(uc && uc != CURLUE_NO_QUERY)
    ଯାଅ out;

  ଯଦି(!query) {
    req->path = path;
    path = ଖାଲି;
  }
  ଅନ୍ୟଥା {
    result = curlx_dyn_add(&buf, path);
    ଯଦି(!result)
      result = curlx_dyn_addf(&buf, "?%s", query);
    ଯଦି(result)
      ଯାଅ out;
    req->path = curlx_dyn_ptr(&buf);
  }
  result = CURLE_OK;

out:
  curlx_free(path);
  curlx_free(query);
  ଯଦି(result)
    curlx_dyn_free(&buf);
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_http_req_make2(ସଂରଚନା httpreq **preq,
                             ଅଚଳ ଅକ୍ଷର *method, size_t m_len,
                             CURLU *url, ଅଚଳ ଅକ୍ଷର *scheme_default)
{
  ସଂରଚନା httpreq *req;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  CURLUcode uc;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, ଆକାର(*req) + m_len);
  ଯଦି(!req)
    ଯାଅ out;
  memcpy(req->method, method, m_len);

  uc = curl_url_get(url, CURLUPART_SCHEME, &req->scheme, 0);
  ଯଦି(uc && uc != CURLUE_NO_SCHEME)
    ଯାଅ out;
  ଯଦି(!req->scheme && scheme_default) {
    req->scheme = curlx_strdup(scheme_default);
    ଯଦି(!req->scheme)
      ଯାଅ out;
  }

  result = req_assign_url_authority(req, url);
  ଯଦି(result)
    ଯାଅ out;
  result = req_assign_url_path(req, url);
  ଯଦି(result)
    ଯାଅ out;

  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  ଯଦି(result && req)
    Curl_http_req_free(req);
  *preq = result ? ଖାଲି : req;
  ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ Curl_http_req_free(ସଂରଚନା httpreq *req)
{
  ଯଦି(req) {
    curlx_free(req->scheme);
    curlx_free(req->authority);
    curlx_free(req->path);
    Curl_dynhds_free(&req->headers);
    Curl_dynhds_free(&req->trailers);
    curlx_free(req);
  }
}

ସଂରଚନା name_const {
  ଅଚଳ ଅକ୍ଷର *name;
  size_t namelen;
};

ସ୍ଥିର ଅଚଳ ସଂରଚନା name_const H2_NON_FIELD[] = {
  { STRCONST("Host") },
  { STRCONST("Upgrade") },
  { STRCONST("Connection") },
  { STRCONST("Keep-Alive") },
  { STRCONST("Proxy-Connection") },
  { STRCONST("Transfer-Encoding") },
};

ସ୍ଥିର bool h2_permissible_field(ସଂରଚନା dynhds_entry *e)
{
  size_t i;
  ପାଇଁ(i = 0; i < CURL_ARRAYSIZE(H2_NON_FIELD); ++i) {
    ଯଦି(e->namelen == H2_NON_FIELD[i].namelen &&
       curl_strnequal(H2_NON_FIELD[i].name, e->name, e->namelen))
      ଫେରନ୍ତୁ FALSE;
  }
  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର bool http_TE_has_token(ଅଚଳ ଅକ୍ଷର *fvalue, ଅଚଳ ଅକ୍ଷର *token)
{
  ଯେତେବେଳେ(*fvalue) {
    ସଂରଚନା Curl_str name;

    /* skip to first token */
    ଯେତେବେଳେ(ISBLANK(*fvalue) || *fvalue == ',')
      fvalue++;
    ଯଦି(curlx_str_cspn(&fvalue, &name, " \t\r;,"))
      ଫେରନ୍ତୁ FALSE;
    ଯଦି(curlx_str_casecompare(&name, token))
      ଫେରନ୍ତୁ TRUE;

    /* skip any remainder after token, e.g. parameters with quoted strings */
    ଯେତେବେଳେ(*fvalue && *fvalue != ',') {
      ଯଦି(*fvalue == '"') {
        ସଂରଚନା Curl_str qw;
        /* if we do not cleanly find a quoted word here, the header value
         * does not follow HTTP syntax and we reject */
        ଯଦି(curlx_str_quotedword(&fvalue, &qw, CURL_MAX_HTTP_HEADER))
          ଫେରନ୍ତୁ FALSE;
      }
      ଅନ୍ୟଥା
        fvalue++;
    }
  }
  ଫେରନ୍ତୁ FALSE;
}

CURLcode Curl_http_req_to_h2(ସଂରଚନା dynhds *h2_headers,
                             ସଂରଚନା httpreq *req, ସଂରଚନା Curl_easy *data)
{
  ଅଚଳ ଅକ୍ଷର *scheme = ଖାଲି, *authority = ଖାଲି;
  ସଂରଚନା dynhds_entry *e;
  size_t i;
  CURLcode result;

  DEBUGASSERT(req);
  DEBUGASSERT(h2_headers);

  ଯଦି(req->scheme) {
    scheme = req->scheme;
  }
  ଅନ୍ୟଥା ଯଦି(strcmp("CONNECT", req->method)) {
    scheme = Curl_checkheaders(data, STRCONST(HTTP_PSEUDO_SCHEME));
    ଯଦି(scheme) {
      scheme += ଆକାର(HTTP_PSEUDO_SCHEME);
      curlx_str_passblanks(&scheme);
      infof(data, "set pseudo header %s to %s", HTTP_PSEUDO_SCHEME, scheme);
    }
    ଅନ୍ୟଥା {
      scheme = data->state.origin->scheme->name;
    }
  }

  ଯଦି(req->authority) {
    authority = req->authority;
  }
  ଅନ୍ୟଥା {
    e = Curl_dynhds_get(&req->headers, STRCONST("Host"));
    ଯଦି(e)
      authority = e->ମୂଲ୍ୟ;
  }

  Curl_dynhds_reset(h2_headers);
  Curl_dynhds_set_opts(h2_headers, DYNHDS_OPT_LOWERCASE);
  result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_METHOD),
                           req->method, strlen(req->method));
  ଯଦି(!result && scheme) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_SCHEME),
                             scheme, strlen(scheme));
  }
  ଯଦି(!result && authority) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_AUTHORITY),
                             authority, strlen(authority));
  }
  ଯଦି(!result && req->path) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_PATH),
                             req->path, strlen(req->path));
  }
  ପାଇଁ(i = 0; !result && i < Curl_dynhds_ଗଣନା(&req->headers); ++i) {
    e = Curl_dynhds_getn(&req->headers, i);
    /* "TE" is special in that it is only permissible when it
     * has only value "trailers". RFC 9113 ch. 8.2.2 */
    ଯଦି(e->namelen == 2 && curl_strequal("TE", e->name)) {
      ଯଦି(http_TE_has_token(e->ମୂଲ୍ୟ, "trailers"))
        result = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                                 "trailers", CURL_CSTRLEN("trailers"));
    }
    ଅନ୍ୟଥା ଯଦି(h2_permissible_field(e)) {
      result = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                               e->ମୂଲ୍ୟ, e->valuelen);
    }
  }

  ଫେରନ୍ତୁ result;
}

CURLcode Curl_http_resp_make(ସଂରଚନା http_resp **presp,
                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା status,
                             ଅଚଳ ଅକ୍ଷର *description)
{
  ସଂରଚନା http_resp *resp;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  resp = curlx_calloc(1, ଆକାର(*resp));
  ଯଦି(!resp)
    ଯାଅ out;

  resp->status = status;
  ଯଦି(description) {
    resp->description = curlx_strdup(description);
    ଯଦି(!resp->description)
      ଯାଅ out;
  }
  Curl_dynhds_init(&resp->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&resp->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  ଯଦି(result && resp)
    Curl_http_resp_free(resp);
  *presp = result ? ଖାଲି : resp;
  ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ Curl_http_resp_free(ସଂରଚନା http_resp *resp)
{
  ଯଦି(resp) {
    curlx_free(resp->description);
    Curl_dynhds_free(&resp->headers);
    Curl_dynhds_free(&resp->trailers);
    ଯଦି(resp->prev)
      Curl_http_resp_free(resp->prev);
    curlx_free(resp);
  }
}

/*
 * HTTP handler interface.
 */
ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_http = {
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
