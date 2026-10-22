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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"

#ifndef CURL_DISABLE_HTTP

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif

#ifdef HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ಅನ್ತರ್ಭಾವ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#ಅನ್ತರ್ಭಾವ <sys/param.h>
#endif

#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "formdata.h"
#ಅನ್ತರ್ಭಾವ "mime.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#ಅನ್ತರ್ಭಾವ "cookie.h"
#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic.h"
#ಅನ್ತರ್ಭಾವ "http_digest.h"
#ಅನ್ತರ್ಭಾವ "http_ntlm.h"
#ಅನ್ತರ್ಭಾವ "http_negotiate.h"
#ಅನ್ತರ್ಭಾವ "http_aws_sigv4.h"
#ಅನ್ತರ್ಭಾವ "http_httpsig.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "urlapi-int.h"
#ಅನ್ತರ್ಭಾವ "curl_share.h"
#ಅನ್ತರ್ಭಾವ "dynhds.h"
#ಅನ್ತರ್ಭಾವ "http.h"
#ಅನ್ತರ್ಭಾವ "headers.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "parsedate.h" /* for the week day and month names */
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "content_encoding.h"
#ಅನ್ತರ್ಭಾವ "http_proxy.h"
#ಅನ್ತರ್ಭಾವ "http2.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "altsvc.h"
#ಅನ್ತರ್ಭಾವ "hsts.h"
#ಅನ್ತರ್ಭಾವ "rtsp.h"
#ಅನ್ತರ್ಭಾವ "ws.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

ಶೂನ್ಯ Curl_http_neg_init(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ http_negotiation *neg)
{
  memset(neg, 0, ಪರಿಮಾಣ(*neg));
  neg->accept_09 = data->set.http09_allowed;
  ವಿಕಲ್ಪ(data->set.httpwant) {
  ಪಕ್ಷ CURL_HTTP_VERSION_1_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    neg->only_10 = TRUE;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_HTTP_VERSION_1_1:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_HTTP_VERSION_2_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->h2_upgrade = TRUE;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_HTTP_VERSION_2TLS:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    neg->wanted = neg->allowed = (CURL_HTTP_V2x);
    data->state.http_neg.h2_prior_knowledge = TRUE;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_HTTP_VERSION_3:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    neg->allowed = neg->wanted;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_HTTP_VERSION_3ONLY:
    neg->wanted = neg->allowed = (CURL_HTTP_V3x);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_HTTP_VERSION_NONE:
  ಸ್ವಭಾವ:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    ಭಙ್ಗ;
  }
}

CURLcode Curl_http_setup_conn(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ connectdata *conn)
{
  /* allocate the HTTP-specific struct for the Curl_easy, only to survive
     during this request */
  ಯದಿ(data->state.http_neg.wanted == CURL_HTTP_V3x) {
    /* only HTTP/3, needs to work */
    CURLcode result = Curl_conn_may_http3(data, conn);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
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
ವರ್ಣ *Curl_checkProxyheaders(ಸಂರಚನಾ Curl_easy *data,
                             ಧ್ರುವ ಸಂರಚನಾ connectdata *conn,
                             ಧ್ರುವ ವರ್ಣ *thisheader,
                             ಧ್ರುವ size_t thislen)
{
  ಸಂರಚನಾ curl_slist *head;

  ಪ್ರತಿ(head = (conn->http_proxy.peer && data->set.sep_headers) ?
        data->set.proxyheaders : data->set.headers;
      head; head = head->next) {
    ಯದಿ(curl_strnequal(head->data, thisheader, thislen) &&
       Curl_headersep(head->data[thislen]))
      ಪ್ರತ್ಯಾಗಮ head->data;
  }

  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/* If the header has a value, this function returns TRUE and the value is in
   'outp' with blanks trimmed off. */
ಸ್ಥಿರ bool header_has_ಮೌಲ್ಯ(ಧ್ರುವ ವರ್ಣ **headerp, ಸಂರಚನಾ Curl_str *outp)
{
  bool ಮೌಲ್ಯ = !curlx_str_cspn(headerp, outp, ";:") &&
    (!curlx_str_single(headerp, ':') || !curlx_str_single(headerp, ';'));

  ಯದಿ(ಮೌಲ್ಯ) {
    curlx_str_cspn(headerp, outp, "\r\n");
    curlx_str_trimblanks(outp);
  }
  ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ;
}

ಸ್ಥಿರ bool http_header_is_empty(ಧ್ರುವ ವರ್ಣ *header)
{
  ಸಂರಚನಾ Curl_str out;

  ಯದಿ(header_has_ಮೌಲ್ಯ(&header, &out)) {
    ಪ್ರತ್ಯಾಗಮ curlx_strlen(&out) == 0;
  }
  ಪ್ರತ್ಯಾಗಮ TRUE; /* invalid header format, treat as empty */
}

/*
 * Strip off leading and trailing whitespace from the value in the given HTTP
 * header line and return a strdup-ed copy in 'valp' - returns an empty
 * string if the header value consists entirely of whitespace.
 *
 * If the header is provided as "name;", ending with a semicolon, it returns a
 * blank string.
 */
ಸ್ಥಿರ CURLcode copy_custom_ಮೌಲ್ಯ(ಧ್ರುವ ವರ್ಣ *header, ವರ್ಣ **valp)
{
  ಸಂರಚನಾ Curl_str out = { 0 };

  /* find the end of the header name */
  ಯದಿ(header_has_ಮೌಲ್ಯ(&header, &out)) {
    *valp = curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
    ಯದಿ(*valp)
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }
  /* bad input */
  *valp = ರಿಕ್ತ;
  ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
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
ವರ್ಣ *Curl_copy_header_ಮೌಲ್ಯ(ಧ್ರುವ ವರ್ಣ *header)
{
  ಸಂರಚನಾ Curl_str out;

  /* find the end of the header name */
  ಯದಿ(!curlx_str_until(&header, &out, MAX_HTTP_RESP_HEADER_SIZE, ':') &&
     !curlx_str_single(&header, ':')) {
    curlx_str_untilnl(&header, &out, MAX_HTTP_RESP_HEADER_SIZE);
    curlx_str_trimblanks(&out);
    ಪ್ರತ್ಯಾಗಮ curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
  }
  /* bad input, should never happen */
  DEBUGASSERT(0);
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ifndef CURL_DISABLE_HTTP_AUTH

#ifndef CURL_DISABLE_BASIC_AUTH
/*
 * http_output_basic() sets up an Authorization: header (or the proxy version)
 * for HTTP Basic authentication.
 *
 * Returns CURLcode.
 */
ಸ್ಥಿರ CURLcode http_output_basic(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ connectdata *conn, bool proxy)
{
  size_t size = 0;
  ವರ್ಣ *authorization = ರಿಕ್ತ;
  ವರ್ಣ **p_hd;
  CURLcode result;
  ಸಂರಚನಾ Curl_creds *creds = ರಿಕ್ತ;
  ವರ್ಣ *out;

  /* credentials are unique per transfer for HTTP, do not use the ones for the
     connection */
  ಯದಿ(proxy) {
#ifndef CURL_DISABLE_PROXY
    p_hd = &data->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
#ಅನ್ಯಥಾ
    (ಶೂನ್ಯ)conn;
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif
  }
  ಅನ್ಯಥಾ {
    p_hd = &data->req.hd_auth;
    creds = data->state.creds;
  }

  ಯದಿ(!creds) {
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }

  out = curl_maprintf("%s:%s", creds->user, creds->passwd);
  ಯದಿ(!out)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  result = curlx_base64_encode((uint8_t *)out, strlen(out),
                               &authorization, &size);
  ಯದಿ(result)
    ಗಚ್ಛ fail;

  ಯದಿ(!authorization) {
    result = CURLE_REMOTE_ACCESS_DENIED;
    ಗಚ್ಛ fail;
  }

  curlx_free(*p_hd);
  *p_hd = curl_maprintf("%sAuthorization: Basic %s\r\n",
                        proxy ? "Proxy-" : "",
                        authorization);
  curlx_free(authorization);
  ಯದಿ(!*p_hd) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ fail;
  }

fail:
  curlx_free(out);
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif

#ifndef CURL_DISABLE_BEARER_AUTH
/*
 * http_output_bearer() sets up an Authorization: header
 * for HTTP Bearer authentication.
 *
 * Returns CURLcode.
 */
ಸ್ಥಿರ CURLcode http_output_bearer(ಸಂರಚನಾ Curl_easy *data)
{
  ವರ್ಣ **userp;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(Curl_creds_has_oauth_bearer(data->state.creds));
  userp = &data->req.hd_auth;
  curlx_free(*userp);
  *userp = curl_maprintf("Authorization: Bearer %s\r\n",
                         Curl_creds_oauth_bearer(data->state.creds));

  ಯದಿ(!*userp) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ fail;
  }

fail:
  ಪ್ರತ್ಯಾಗಮ result;
}
#endif

#endif

/* pickoneauth() selects the most favorable authentication method from the
 * ones available and the ones we want.
 *
 * return TRUE if one was picked
 */
ಸ್ಥಿರ bool pickoneauth(ಸಂರಚನಾ auth *pick, unsigned long mask,
                        ಸಂರಚನಾ Curl_creds *creds)
{
  bool have_user_pass = Curl_creds_has_user_or_pass(creds);
  bool picked;
  /* only deal with authentication we want */
  unsigned long avail = pick->avail & pick->want & mask;
  picked = TRUE;

  /* The order of these checks is highly relevant, as this will be the order
     of preference in case of the existence of multiple accepted types. */
  ಯದಿ(avail & CURLAUTH_NEGOTIATE)  /* available on empty creds */
    pick->picked = CURLAUTH_NEGOTIATE;
#ifndef CURL_DISABLE_BEARER_AUTH
  ಅನ್ಯಥಾ ಯದಿ((avail & CURLAUTH_BEARER) && Curl_creds_has_oauth_bearer(creds))
    pick->picked = CURLAUTH_BEARER;
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  ಅನ್ಯಥಾ ಯದಿ((avail & CURLAUTH_DIGEST) && have_user_pass)
    pick->picked = CURLAUTH_DIGEST;
#endif
  ಅನ್ಯಥಾ ಯದಿ(avail & CURLAUTH_NTLM)
    pick->picked = CURLAUTH_NTLM;
#ifndef CURL_DISABLE_BASIC_AUTH
  ಅನ್ಯಥಾ ಯದಿ((avail & CURLAUTH_BASIC) && have_user_pass)
    pick->picked = CURLAUTH_BASIC;
#endif
#ifndef CURL_DISABLE_AWS
  ಅನ್ಯಥಾ ಯದಿ(avail & CURLAUTH_AWS_SIGV4)
    pick->picked = CURLAUTH_AWS_SIGV4;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  ಅನ್ಯಥಾ ಯದಿ(avail & CURLAUTH_HTTPSIG)
    pick->picked = CURLAUTH_HTTPSIG;
#endif
  ಅನ್ಯಥಾ {
    pick->picked = CURLAUTH_PICKNONE; /* we select to use nothing */
    picked = FALSE;
  }
  pick->avail = CURLAUTH_NONE; /* clear it here */

  ಪ್ರತ್ಯಾಗಮ picked;
}

/*
 * http_perhapsrewind()
 *
 * The current request needs to be done again - maybe due to a follow
 * or authentication negotiation. Check if:
 * 1) a rewind of the data sent to the server is necessary
 * 2) the current transfer should continue or be stopped early
 */
ಸ್ಥಿರ CURLcode http_perhapsrewind(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ connectdata *conn)
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
  VERBOSE(ಧ್ರುವ ವರ್ಣ *ongoing_auth = ರಿಕ್ತ);

  /* We need a rewind before uploading client read data again. The
   * checks below influence of the upload is to be continued
   * or aborted early.
   * This depends on how much remains to be sent and in what state
   * the authentication is. Some auth schemes such as NTLM do not work
   * for a new connection. */
  ಯದಿ(needs_rewind) {
    infof(data, "Need to rewind upload for next request");
    Curl_creader_set_rewind(data, TRUE);
  }

  ಯದಿ(conn->bits.close)
    /* If we already decided to close this connection, we cannot veto. */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಯದಿ(abort_upload) {
    /* We would like to abort the upload - but should we? */
#ifdef USE_NTLM
    ಯದಿ((data->state.authproxy.picked == CURLAUTH_NTLM) ||
       (data->state.authhost.picked == CURLAUTH_NTLM)) {
      VERBOSE(ongoing_auth = "NTLM");
      ಯದಿ((conn->http_ntlm_state != NTLMSTATE_NONE) ||
         (conn->proxy_ntlm_state != NTLMSTATE_NONE)) {
        /* The NTLM-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#endif
#ifdef USE_SPNEGO
    /* There is still data left to send */
    ಯದಿ((data->state.authproxy.picked == CURLAUTH_NEGOTIATE) ||
       (data->state.authhost.picked == CURLAUTH_NEGOTIATE)) {
      VERBOSE(ongoing_auth = "NEGOTIATE");
      ಯದಿ((conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) ||
         (conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE)) {
        /* The NEGOTIATE-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#endif
  }

  ಯದಿ(abort_upload) {
    ಯದಿ(upload_remain >= 0)
      infof(data, "%s%sclose instead of sending %" FMT_OFF_T " more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "",
            upload_remain);
    ಅನ್ಯಥಾ
      infof(data, "%s%sclose instead of sending unknown amount "
            "of more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "");
    /* We decided to abort the ongoing transfer */
    streamclose(conn);
    data->req.size = 0; /* do not download any more than 0 bytes */
    data->req.http_bodyless = TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/**
 * http_should_fail() determines whether an HTTP response code has gotten us
 * into an error state or not.
 *
 * @retval FALSE communications should continue
 *
 * @retval TRUE communications should not continue
 */
ಸ್ಥಿರ bool http_should_fail(ಸಂರಚನಾ Curl_easy *data, ಪೂರ್ಣಾಙ್ಕ httpcode)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  /*
   * If we have not been asked to fail on error,
   * do not fail.
   */
  ಯದಿ(!data->set.http_fail_on_error)
    ಪ್ರತ್ಯಾಗಮ FALSE;

  /*
   * Any code < 400 is never terminal.
   */
  ಯದಿ(httpcode < 400)
    ಪ್ರತ್ಯಾಗಮ FALSE;

  /*
   * A 416 response to a resume request is presumably because the file is
   * already completely downloaded and thus not actually a fail.
   */
  ಯದಿ(data->state.resume_from && data->state.httpreq == HTTPREQ_GET &&
     httpcode == 416)
    ಪ್ರತ್ಯಾಗಮ FALSE;

  /*
   * Any code >= 400 that is not 401 or 407 is always
   * a terminal error
   */
  ಯದಿ((httpcode != 401) && (httpcode != 407))
    ಪ್ರತ್ಯಾಗಮ TRUE;

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
  ಯದಿ((httpcode == 401) && !data->state.creds)
    ಪ್ರತ್ಯಾಗಮ TRUE;
#ifndef CURL_DISABLE_PROXY
  ಯದಿ((httpcode == 407) && !data->conn->http_proxy.creds)
    ಪ್ರತ್ಯಾಗಮ TRUE;
#endif

  ಪ್ರತ್ಯಾಗಮ (bool)data->state.authproblem;
}

/*
 * Curl_http_auth_act() gets called when all HTTP headers have been received
 * and it checks what authentication methods that are available and decides
 * which one (if any) to use. It will set 'newurl' if an auth method was
 * picked.
 */
CURLcode Curl_http_auth_act(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  bool pickhost = FALSE;
  bool pickproxy = FALSE;
  CURLcode result = CURLE_OK;
  unsigned long authmask = ~0UL;

  ಯದಿ(!Curl_creds_has_oauth_bearer(data->state.creds))
    authmask &= (unsigned long)~CURLAUTH_BEARER;

  ಯದಿ(100 <= data->req.httpcode && data->req.httpcode <= 199)
    /* this is a transient response code, ignore */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಯದಿ(data->state.authproblem)
    ಪ್ರತ್ಯಾಗಮ data->set.http_fail_on_error ? CURLE_HTTP_RETURNED_ERROR : CURLE_OK;

  ಯದಿ(data->state.creds &&
     ((data->req.httpcode == 401) ||
      (data->req.authneg && data->req.httpcode < 300))) {
    pickhost = pickoneauth(&data->state.authhost, authmask, data->state.creds);
    ಯದಿ(!pickhost)
      data->state.authproblem = TRUE;
    ಅನ್ಯಥಾ
      data->info.httpauthpicked = data->state.authhost.picked;
    ಯದಿ(data->state.authhost.picked == CURLAUTH_NTLM &&
       (data->req.httpversion_sent > 11)) {
      infof(data, "Forcing HTTP/1.1 for NTLM");
      connclose(conn);
      data->state.http_neg.wanted = CURL_HTTP_V1x;
      data->state.http_neg.allowed = CURL_HTTP_V1x;
    }
  }
#ifndef CURL_DISABLE_PROXY
  ಯದಿ(conn->http_proxy.creds &&
     ((data->req.httpcode == 407) ||
      (data->req.authneg && data->req.httpcode < 300))) {
    pickproxy = pickoneauth(&data->state.authproxy,
                            authmask & ~CURLAUTH_BEARER,
                            conn->http_proxy.creds);
    ಯದಿ(!pickproxy)
      data->state.authproblem = TRUE;
    ಅನ್ಯಥಾ
      data->info.proxyauthpicked = data->state.authproxy.picked;
  }
#endif

  ಯದಿ(pickhost || pickproxy) {
    result = http_perhapsrewind(data, conn);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    /* In case this is GSS auth, the newurl field is already allocated so
       we must make sure to free it before allocating a new one. As figured
       out in bug #2284386 */
    curlx_free(data->req.newurl);
    /* clone URL */
    data->req.newurl = Curl_bufref_dup(&data->state.url);
    ಯದಿ(!data->req.newurl)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }
  ಅನ್ಯಥಾ ಯದಿ((data->req.httpcode < 300) &&
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
    ಯದಿ(!data->req.newurl)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    data->state.authhost.done = TRUE;
  }
  ಯದಿ(http_should_fail(data, data->req.httpcode)) {
    failf(data, "The requested URL returned error: %d",
          data->req.httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

#ifndef CURL_DISABLE_HTTP_AUTH
/*
 * Output the correct authentication header depending on the auth type
 * and whether or not it is to a proxy.
 */
ಸ್ಥಿರ CURLcode output_auth_headers(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ connectdata *conn,
                                    ಸಂರಚನಾ auth *authstatus,
                                    ಧ್ರುವ ವರ್ಣ *request,
                                    ಧ್ರುವ ವರ್ಣ *path,
                                    bool proxy)
{
  ಧ್ರುವ ವರ್ಣ *auth = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;
  (ಶೂನ್ಯ)conn;

#ifdef CURL_DISABLE_DIGEST_AUTH
  (ಶೂನ್ಯ)request;
  (ಶೂನ್ಯ)path;
#endif
#ifndef CURL_DISABLE_AWS
  ಯದಿ((authstatus->picked == CURLAUTH_AWS_SIGV4) && !proxy) {
    /* this method is never for proxy */
    auth = "AWS_SIGV4";
    result = Curl_output_aws_sigv4(data);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಅನ್ಯಥಾ
#endif
#ifndef CURL_DISABLE_HTTPSIG
  ಯದಿ((authstatus->picked == CURLAUTH_HTTPSIG) && !proxy) {
    /* HTTPSIG uses its own configured key material rather than
       data->state.creds. Do not let unrelated credentials from a
       redirected URL bypass the cross-host auth boundary. */
    ಯದಿ(Curl_auth_allowed_to_host(data)) {
      auth = "HTTPSIG";
      result = Curl_output_httpsig(data);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    ಅನ್ಯಥಾ
      authstatus->done = TRUE;
  }
  ಅನ್ಯಥಾ
#endif
#ifdef USE_SPNEGO
  ಯದಿ(authstatus->picked == CURLAUTH_NEGOTIATE) {
    ಯದಿ(
#ifndef CURL_DISABLE_PROXY
      (proxy && !Curl_checkProxyheaders(data, conn,
                                        STRCONST("Proxy-authorization"))) ||
#endif
      (!proxy && !Curl_checkheaders(data, STRCONST("Authorization")) &&
       Curl_auth_allowed_to_host(data))) {
      auth = "Negotiate";
      result = Curl_output_negotiate(data, conn, proxy);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    ಅನ್ಯಥಾ
      authstatus->done = TRUE;
  }
  ಅನ್ಯಥಾ
#endif
#ifdef USE_NTLM
  ಯದಿ(authstatus->picked == CURLAUTH_NTLM) {
    ಯದಿ(proxy || Curl_auth_allowed_to_host(data)) {
      auth = "NTLM";
      result = Curl_output_ntlm(data, proxy);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    ಅನ್ಯಥಾ
      authstatus->done = TRUE;
  }
  ಅನ್ಯಥಾ
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  ಯದಿ(authstatus->picked == CURLAUTH_DIGEST) {
    auth = "Digest";
    result = Curl_output_digest(data,
                                proxy,
                                (ಧ್ರುವ unsigned ವರ್ಣ *)request,
                                (ಧ್ರುವ unsigned ವರ್ಣ *)path);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಅನ್ಯಥಾ
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
  ಯದಿ(authstatus->picked == CURLAUTH_BASIC) {
    /* Basic */
    ಯದಿ(
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
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
  ಯದಿ(authstatus->picked == CURLAUTH_BEARER) {
    /* Bearer */
    ಯದಿ(!proxy && Curl_creds_has_oauth_bearer(data->state.creds) &&
       !Curl_checkheaders(data, STRCONST("Authorization"))) {
      auth = "Bearer";
      result = http_output_bearer(data);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif

  ಯದಿ(auth) {
#ifndef CURL_DISABLE_PROXY
    ಯದಿ(proxy)
      data->info.proxyauthpicked = authstatus->picked;
    ಅನ್ಯಥಾ
      data->info.httpauthpicked = authstatus->picked;
    infof(data, "%s auth using %s with user '%s'",
          proxy ? "Proxy" : "Server", auth,
          proxy ? (conn->http_proxy.creds ?
                   conn->http_proxy.creds->user : "") :
          (data->state.creds ?
           data->state.creds->user : ""));
#ಅನ್ಯಥಾ
    (ಶೂನ್ಯ)proxy;
    infof(data, "Server auth using %s with user '%s'",
          auth, data->state.creds ?
          data->state.creds->user : "");
#endif
    authstatus->multipass = !authstatus->done;
  }
  ಅನ್ಯಥಾ {
    authstatus->multipass = FALSE;
    ಯದಿ(proxy)
      data->info.proxyauthpicked = 0;
    ಅನ್ಯಥಾ
      data->info.httpauthpicked = 0;
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_http_output_auth(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ connectdata *conn,
                               ಧ್ರುವ ವರ್ಣ *request,
                               Curl_HttpReq httpreq,
                               ಧ್ರುವ ವರ್ಣ *path,
                               ಧ್ರುವ ವರ್ಣ *query,
                               bool is_connect)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ auth *authhost;
  ಸಂರಚನಾ auth *authproxy;
  ಧ್ರುವ ವರ್ಣ *path_and_query = path;
  ವರ್ಣ *tmp_str = ರಿಕ್ತ;

  DEBUGASSERT(data);
  authhost = &data->state.authhost;
  authproxy = &data->state.authproxy;

  ಯದಿ(
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
    ಗಚ್ಛ out;
  }

  ಯದಿ(query) {
    tmp_str = curl_maprintf("%s?%s", path, query);
    ಯದಿ(!tmp_str) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
    path_and_query = tmp_str;
  }

  ಯದಿ(authhost->want && !authhost->picked)
    /* The app has selected one or more methods, but none has been picked
       so far by a server round-trip. Then we set the picked one to the
       want one, and if this is one single bit it will be used instantly. */
    authhost->picked = authhost->want;

  ಯದಿ(authproxy->want && !authproxy->picked)
    /* The app has selected one or more methods, but none has been picked so
       far by a proxy round-trip. Then we set the picked one to the want one,
       and if this is one single bit it will be used instantly. */
    authproxy->picked = authproxy->want;

#ifndef CURL_DISABLE_PROXY
  /* Send proxy authentication header if needed */
  ಯದಿ(conn->bits.origin_is_proxy || is_connect) {
    result = output_auth_headers(data, conn, authproxy, request,
                                 path_and_query, TRUE);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)is_connect;
#endif /* CURL_DISABLE_PROXY */
    /* we have no proxy so let's pretend we are done authenticating
       with it */
    authproxy->done = TRUE;

  /* Either we have credentials for the origin we talk to or
     performing authentication is allowed here */
  ಯದಿ(data->state.creds || Curl_auth_allowed_to_host(data))
    result = output_auth_headers(data, conn, authhost, request,
                                 path_and_query, FALSE);
  ಅನ್ಯಥಾ
    authhost->done = TRUE;

  ಯದಿ(((authhost->multipass && !authhost->done) ||
      (authproxy->multipass && !authproxy->done)) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_HEAD)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    data->req.authneg = TRUE;
  }
  ಅನ್ಯಥಾ
    data->req.authneg = FALSE;

out:
  curlx_free(tmp_str);
  ಪ್ರತ್ಯಾಗಮ result;
}

#ಅನ್ಯಥಾ /* !CURL_DISABLE_HTTP_AUTH */
/* when disabled */
CURLcode Curl_http_output_auth(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ connectdata *conn,
                               ಧ್ರುವ ವರ್ಣ *request,
                               Curl_HttpReq httpreq,
                               ಧ್ರುವ ವರ್ಣ *path,
                               ಧ್ರುವ ವರ್ಣ *query,
                               bool is_connect)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)request;
  (ಶೂನ್ಯ)httpreq;
  (ಶೂನ್ಯ)path;
  (ಶೂನ್ಯ)query;
  (ಶೂನ್ಯ)is_connect;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP_AUTH, else */

#ಯದಿ defined(USE_SPNEGO) || defined(USE_NTLM) || \
  !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_BASIC_AUTH) || \
  !defined(CURL_DISABLE_BEARER_AUTH)
ಸ್ಥಿರ bool authcmp(ಧ್ರುವ ವರ್ಣ *auth, ಧ್ರುವ ವರ್ಣ *ಸಾಲು)
{
  /* the auth string must not have an alnum following */
  size_t n = strlen(auth);
  ಪ್ರತ್ಯಾಗಮ curl_strnequal(auth, ಸಾಲು, n) && !ISALNUM(ಸಾಲು[n]);
}
#endif

#ifdef USE_SPNEGO
ಸ್ಥಿರ CURLcode auth_spnego(ಸಂರಚನಾ Curl_easy *data,
                            bool proxy,
                            ಧ್ರುವ ವರ್ಣ *auth,
                            ಸಂರಚನಾ auth *authp,
                            uint32_t *availp)
{
  ಯದಿ((authp->avail & CURLAUTH_NEGOTIATE) || Curl_auth_is_spnego_supported()) {
    *availp |= CURLAUTH_NEGOTIATE;
    authp->avail |= CURLAUTH_NEGOTIATE;

    ಯದಿ(authp->picked == CURLAUTH_NEGOTIATE) {
      ಸಂರಚನಾ connectdata *conn = data->conn;
      CURLcode result = Curl_ಒಳಹರಿವು_negotiate(data, conn, proxy, auth);
      uint8_t *negstate = proxy ? &conn->proxy_negotiate_state :
        &conn->http_negotiate_state;
      ಯದಿ(!result) {
        curlx_free(data->req.newurl);
        data->req.newurl = Curl_bufref_dup(&data->state.url);
        ಯದಿ(!data->req.newurl)
          ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
        data->state.authproblem = FALSE;
        /* we received a GSS auth token and we dealt with it fine */
        *negstate = CURL_NEGO_AUTH_RECV;
      }
      ಅನ್ಯಥಾ
        data->state.authproblem = TRUE;
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif

#ifdef USE_NTLM
ಸ್ಥಿರ CURLcode auth_ntlm(ಸಂರಚನಾ Curl_easy *data,
                          bool proxy,
                          ಧ್ರುವ ವರ್ಣ *auth,
                          ಸಂರಚನಾ auth *authp,
                          uint32_t *availp)
{
  /* NTLM support requires the SSL crypto libs */
  ಯದಿ((authp->avail & CURLAUTH_NTLM) || Curl_auth_is_ntlm_supported()) {
    *availp |= CURLAUTH_NTLM;
    authp->avail |= CURLAUTH_NTLM;

    ಯದಿ(authp->picked == CURLAUTH_NTLM) {
      /* NTLM authentication is picked and activated */
      CURLcode result = Curl_ಒಳಹರಿವು_ntlm(data, proxy, auth);
      ಯದಿ(!result)
        data->state.authproblem = FALSE;
      ಅನ್ಯಥಾ {
        ಯದಿ(result == CURLE_OUT_OF_MEMORY)
          ಪ್ರತ್ಯಾಗಮ result;
        infof(data, "NTLM authentication problem, ignoring.");
        data->state.authproblem = TRUE;
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_DIGEST_AUTH
ಸ್ಥಿರ CURLcode auth_digest(ಸಂರಚನಾ Curl_easy *data,
                            bool proxy,
                            ಧ್ರುವ ವರ್ಣ *auth,
                            ಸಂರಚನಾ auth *authp,
                            uint32_t *availp)
{
  ಯದಿ(authp->avail & CURLAUTH_DIGEST) {
    *availp |= CURLAUTH_DIGEST;
    infof(data, "Ignoring duplicate digest auth header.");
  }
  ಅನ್ಯಥಾ ಯದಿ(Curl_auth_is_digest_supported()) {
    CURLcode result;

    *availp |= CURLAUTH_DIGEST;
    authp->avail |= CURLAUTH_DIGEST;

    /* We call this function on input Digest headers even if Digest
     * authentication is not activated yet, as we need to store the
     * incoming data from this header in case we are going to use
     * Digest */
    result = Curl_ಒಳಹರಿವು_digest(data, proxy, auth);
    ಯದಿ(result) {
      ಯದಿ(result == CURLE_OUT_OF_MEMORY)
        ಪ್ರತ್ಯಾಗಮ result;
      infof(data, "Digest authentication problem, ignoring.");
      data->state.authproblem = TRUE;
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BASIC_AUTH
ಸ್ಥಿರ CURLcode auth_basic(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ auth *authp,
                           uint32_t *availp)
{
  *availp |= CURLAUTH_BASIC;
  authp->avail |= CURLAUTH_BASIC;
  ಯದಿ(authp->picked == CURLAUTH_BASIC) {
    /* We asked for Basic authentication but got a 40X back anyway, which
       means our name+password is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(data, "Basic authentication problem, ignoring.");
    data->state.authproblem = TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BEARER_AUTH
ಸ್ಥಿರ CURLcode auth_bearer(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ auth *authp,
                            uint32_t *availp)
{
  *availp |= CURLAUTH_BEARER;
  authp->avail |= CURLAUTH_BEARER;
  ಯದಿ(authp->picked == CURLAUTH_BEARER) {
    /* We asked for Bearer authentication but got a 40X back anyway, which
       means our token is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(data, "Bearer authentication problem, ignoring.");
    data->state.authproblem = TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif

/*
 * Curl_http_input_auth() deals with Proxy-Authenticate: and WWW-Authenticate:
 * headers. They are dealt with both in the transfer.c main loop and in the
 * proxy CONNECT loop.
 *
 * The 'auth' line ends with a NUL byte without CR or LF present.
 */
CURLcode Curl_http_ಒಳಹರಿವು_auth(ಸಂರಚನಾ Curl_easy *data, bool proxy,
                              ಧ್ರುವ ವರ್ಣ *auth) /* the first non-space */
{
  /*
   * This resource requires authentication
   */
#ಯದಿ defined(USE_SPNEGO) ||                      \
  defined(USE_NTLM) ||                          \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||         \
  !defined(CURL_DISABLE_BASIC_AUTH) ||          \
  !defined(CURL_DISABLE_BEARER_AUTH)

  uint32_t *availp;
  ಸಂರಚನಾ auth *authp;
  CURLcode result = CURLE_OK;
  DEBUGASSERT(auth);
  DEBUGASSERT(data);

  ಯದಿ(proxy) {
    availp = &data->info.proxyauthavail;
    authp = &data->state.authproxy;
  }
  ಅನ್ಯಥಾ {
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

  ಯಾವತ್(*auth) {
#ifdef USE_SPNEGO
    ಯದಿ(authcmp("Negotiate", auth))
      result = auth_spnego(data, proxy, auth, authp, availp);
#endif
#ifdef USE_NTLM
    ಯದಿ(!result && authcmp("NTLM", auth))
      result = auth_ntlm(data, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
    ಯದಿ(!result && authcmp("Digest", auth))
      result = auth_digest(data, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
    ಯದಿ(!result && authcmp("Basic", auth))
      result = auth_basic(data, authp, availp);
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
    ಯದಿ(authcmp("Bearer", auth))
      result = auth_bearer(data, authp, availp);
#endif

    ಯದಿ(result)
      ಭಙ್ಗ;

    /* there may be multiple methods on one line, so keep reading */
    auth = strchr(auth, ',');
    ಯದಿ(auth) /* if we are on a comma, skip it */
      auth++;
    ಅನ್ಯಥಾ
      ಭಙ್ಗ;
    curlx_str_passblanks(&auth);
  }
  ಪ್ರತ್ಯಾಗಮ result;
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)proxy;
  (ಶೂನ್ಯ)auth;
  /* nothing to do when disabled */
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
#endif
}

ಸ್ಥಿರ ಶೂನ್ಯ http_switch_to_get(ಸಂರಚನಾ Curl_easy *data, ಪೂರ್ಣಾಙ್ಕ code)
{
  ಧ್ರುವ ವರ್ಣ *req = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  ಯದಿ((req || data->state.httpreq != HTTPREQ_GET) &&
     (data->set.http_follow_mode == CURLFOLLOW_OBEYCODE)) {
    NOVERBOSE((ಶೂನ್ಯ)code);
    infof(data, "Switch to GET because of %d response", code);
    data->state.http_ignorecustom = TRUE;
  }
  ಅನ್ಯಥಾ ಯದಿ(req && (data->set.http_follow_mode != CURLFOLLOW_FIRSTONLY))
    infof(data, "Stick to %s instead of GET", req);

  data->state.httpreq = HTTPREQ_GET;
  Curl_creader_set_rewind(data, FALSE);
}

#define HTTPREQ_IS_POST(data)                    \
  ((data)->state.httpreq == HTTPREQ_POST ||      \
   (data)->state.httpreq == HTTPREQ_POST_FORM || \
   (data)->state.httpreq == HTTPREQ_POST_MIME)

ಸ್ಥಿರ CURLcode set_auto_referer(ಸಂರಚನಾ Curl_easy *data)
{
  CURLU *u;
  ವರ್ಣ *referer = ರಿಕ್ತ;
  CURLUcode uc;

  /* We are asked to automatically set the previous URL as the referer
     when we get the next URL. We pick the ->url field, which may or may
     not be 100% correct */
  Curl_bufref_free(&data->state.referer);

  /* Make a copy of the URL without credentials and fragment */
  u = curl_url();
  ಯದಿ(!u)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  uc = curl_url_set(u, CURLUPART_URL,
                    Curl_bufref_ptr(&data->state.url), 0);
  ಯದಿ(!uc)
    uc = curl_url_set(u, CURLUPART_FRAGMENT, ರಿಕ್ತ, 0);
  ಯದಿ(!uc)
    uc = curl_url_set(u, CURLUPART_USER, ರಿಕ್ತ, 0);
  ಯದಿ(!uc)
    uc = curl_url_set(u, CURLUPART_PASSWORD, ರಿಕ್ತ, 0);
  ಯದಿ(!uc)
    uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

  curl_url_cleanup(u);

  ಯದಿ(uc || !referer)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  Curl_bufref_set(&data->state.referer, referer, 0, curl_free);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode follow_parse_url(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *newurl,
                                 followtype type, ವರ್ಣ **follow_urlp,
                                 CURLUcode *ucp)
{
  CURLUcode uc;

  DEBUGASSERT(data->state.uh);
  uc = curl_url_set(data->state.uh, CURLUPART_URL, newurl, (unsigned ಪೂರ್ಣಾಙ್ಕ)
                    ((type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                     ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) |
                     CURLU_ALLOW_SPACE |
                     (data->set.path_as_is ? CURLU_PATH_AS_IS : 0)));
  *ucp = uc;
  ಯದಿ(uc) {
    ಯದಿ((uc == CURLUE_OUT_OF_MEMORY) || (type != FOLLOW_FAKE)) {
      failf(data, "The redirect target URL could not be parsed: %s",
            curl_url_strerror(uc));
      ಪ್ರತ್ಯಾಗಮ Curl_uc_to_curlcode(uc);
    }

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, duplicate the field as-is */
    *follow_urlp = curlx_strdup(newurl);
    ಯದಿ(!*follow_urlp)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }
  ಅನ್ಯಥಾ {
    CURLU *u = curl_url();
    ಯದಿ(!u)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    uc = curl_url_set(u, CURLUPART_URL,
                      Curl_bufref_ptr(&data->state.url),
                      CURLU_URLENCODE | CURLU_ALLOW_SPACE);
    ಯದಿ(!uc)
      uc = curl_url_get(data->state.uh, CURLUPART_URL, follow_urlp, 0);
    ಯದಿ(uc) {
      curl_url_cleanup(u);
      ಪ್ರತ್ಯಾಗಮ Curl_uc_to_curlcode(uc);
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    {
      bool same_origin = Curl_url_same_origin(u, data->state.uh);
      curl_url_cleanup(u);
      ಯದಿ(!same_origin)
        Curl_auth_digest_cleanup(&data->state.digest);
    }
#ಅನ್ಯಥಾ
    curl_url_cleanup(u);
#endif
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode follow_fake(ಸಂರಚನಾ Curl_easy *data, ವರ್ಣ *follow_url,
                            CURLUcode uc, bool reachedmax)
{
  /* we are only figuring out the new URL if we would have followed locations
     but now we are done so we can get out! */
  ಯದಿ(!uc) {
    CURLU *u = curl_url();
    ವರ್ಣ *nocred = ರಿಕ್ತ;

    ಯದಿ(!u) {
      curlx_free(follow_url);
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
    ಯದಿ(!curl_url_set(u, CURLUPART_URL, follow_url,
                     CURLU_NON_SUPPORT_SCHEME |
                     (data->set.path_as_is ? CURLU_PATH_AS_IS : 0)) &&
       !curl_url_set(u, CURLUPART_USER, ರಿಕ್ತ, 0) &&
       !curl_url_set(u, CURLUPART_PASSWORD, ರಿಕ್ತ, 0))
      (ಶೂನ್ಯ)curl_url_get(u, CURLUPART_URL, &nocred, CURLU_GET_EMPTY);
    curl_url_cleanup(u);
    ಯದಿ(nocred) {
      curlx_free(follow_url);
      follow_url = nocred;
    }
  }
  data->info.wouldredirect = follow_url;

  ಯದಿ(reachedmax) {
    failf(data, "Maximum (%d) redirects followed", data->set.maxredirs);
    ಪ್ರತ್ಯಾಗಮ CURLE_TOO_MANY_REDIRECTS;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ bool follow_switch_get(ಸಂರಚನಾ Curl_easy *data)
{
  /*
   * We get here when the HTTP code is 300-399 and 401 or 407. We need to
   * perform differently based on exactly what return code there was.
   */
  ವಿಕಲ್ಪ(data->info.httpcode) {
  ಪಕ್ಷ 301: /* Moved Permanently */
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
    ಯದಿ(HTTPREQ_IS_POST(data) && !data->set.post301) {
      http_switch_to_get(data, 301);
      ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ 302: /* Found */
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
    ಯದಿ(HTTPREQ_IS_POST(data) && !data->set.post302) {
      http_switch_to_get(data, 302);
      ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     */
    ಯದಿ(!HTTPREQ_IS_POST(data) || !data->set.post303) {
      http_switch_to_get(data, 303);
      ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಭಙ್ಗ;

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
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

CURLcode Curl_http_follow(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *newurl,
                          followtype type)
{
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  ವರ್ಣ *follow_url = ರಿಕ್ತ;
  CURLUcode uc;
  CURLcode result;
  CURLcode rewind_result;
  bool switch_to_get;

  DEBUGASSERT(type != FOLLOW_NONE);

  ಯದಿ(type != FOLLOW_FAKE)
    data->state.requests++; /* count all real follows */
  ಯದಿ(type == FOLLOW_REDIR) {
    ಯದಿ((data->set.maxredirs != -1) &&
       (data->state.followlocation >= data->set.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    ಅನ್ಯಥಾ {
      data->state.followlocation++; /* count redirect-followings, including
                                       auth reloads */

      ಯದಿ(data->set.http_auto_referer) {
        result = set_auto_referer(data);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;
      }
    }
  }

  ಯದಿ((type != FOLLOW_RETRY) &&
     (data->req.httpcode != 401) && (data->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, ರಿಕ್ತ, 0, FALSE)) {
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: do not allow a custom port number */
    disallowport = TRUE;
  }

  result = follow_parse_url(data, newurl, type, &follow_url, &uc);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  DEBUGASSERT(follow_url);

  ಯದಿ(type == FOLLOW_FAKE)
    ಪ್ರತ್ಯಾಗಮ follow_fake(data, follow_url, uc, reachedmax);

  ಯದಿ(disallowport)
    data->state.allow_port = FALSE;

  Curl_bufref_set(&data->state.url, follow_url, 0, curl_free);
  rewind_result = Curl_req_soft_reset(&data->req, data);
  infof(data, "Issue another request to this URL: '%s'", follow_url);
  ಯದಿ((data->set.http_follow_mode == CURLFOLLOW_FIRSTONLY) &&
     !data->state.http_ignorecustom &&
     CURL_EASY_STR(data, STRING_CUSTOMREQUEST)) {
    data->state.http_ignorecustom = TRUE;
    infof(data, "Drop custom request method for next request");
  }

  switch_to_get = follow_switch_get(data);

  /* When rewind of upload data failed and we are not switching to GET,
   * we need to fail the follow, as we cannot send the data again. */
  ಯದಿ(rewind_result && !switch_to_get)
    ಪ್ರತ್ಯಾಗಮ rewind_result;

  Curl_pgrsTime(data, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(data);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Curl_compareheader()
 *
 * Returns TRUE if 'headerline' contains the 'header' with given 'content'
 * (within a comma-separated list of tokens). Pass 'header' WITH the colon.
 *
 * @unittest: 1625
 */
bool Curl_compareheader(ಧ್ರುವ ವರ್ಣ *headerline, /* line to check */
                        ಧ್ರುವ ವರ್ಣ *header, /* header keyword _with_ colon */
                        ಧ್ರುವ size_t hlen, /* len of the keyword in bytes */
                        ಧ್ರುವ ವರ್ಣ *content, /* content string to find */
                        ಧ್ರುವ size_t clen) /* len of the content in bytes */
{
  /* RFC2616, section 4.2 says: "Each header field consists of a name followed
   * by a colon (":") and the field value. Field names are case-insensitive.
   * The field value MAY be preceded by any amount of LWS, though a single SP
   * is preferred." */

  ಧ್ರುವ ವರ್ಣ *p;
  ಸಂರಚನಾ Curl_str val;
  DEBUGASSERT(hlen);
  DEBUGASSERT(clen);
  DEBUGASSERT(header);
  DEBUGASSERT(content);

  ಯದಿ(!curl_strnequal(headerline, header, hlen))
    ಪ್ರತ್ಯಾಗಮ FALSE; /* does not start with header */

  /* pass the header */
  p = &headerline[hlen];

  ಯದಿ(curlx_str_cspn(&p, &val, "\r\n"))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  curlx_str_trimblanks(&val);

  /* find the content string in the rest of the line */
  ಯದಿ(curlx_strlen(&val) >= clen) {
    size_t len;
    p = curlx_str(&val);
    ಪ್ರತಿ(len = curlx_strlen(&val); len >= clen;) {
      ಸಂರಚನಾ Curl_str next;
      ಧ್ರುವ ವರ್ಣ *o = p;
      /* after a match there must be a comma, space, newline or NUL byte */
      ಯದಿ(curl_strnequal(p, content, clen) &&
         ((p[clen] == ',') || ISBLANK(p[clen]) || ISNEWLINE(p[clen]) ||
          !p[clen]))
        ಪ್ರತ್ಯಾಗಮ TRUE; /* match! */
      /* advance to the next comma */
      ಯದಿ(curlx_str_until(&p, &next, len, ',') ||
         curlx_str_single(&p, ','))
        ಭಙ್ಗ; /* no comma, get out */

      /* if there are more dummy commas, move over them as well */
      ಕುರು
        curlx_str_passblanks(&p);
      ಯಾವತ್(!curlx_str_single(&p, ','));
      /* trailing blanks may move the parsing point past the value end,
         then there is nothing left to match */
      ಯದಿ((size_t)(p - o) > len)
        ಭಙ್ಗ;
      len -= (p - o);
    }
  }
  ಪ್ರತ್ಯಾಗಮ FALSE; /* no match */
}

ಸಂರಚನಾ cr_exp100_ctx {
  ಸಂರಚನಾ Curl_creader super;
  ಸಂರಚನಾ curltime start; /* time started waiting */
  ಗಣನಾ expect100 state;
};

/* Expect: 100-continue client reader, blocking uploads */

ಸ್ಥಿರ ಶೂನ್ಯ http_exp100_continue(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ Curl_creader *reader)
{
  ಸಂರಚನಾ cr_exp100_ctx *ctx = reader->ctx;
  ಯದಿ(ctx->state > EXP100_SEND_DATA) {
    ctx->state = EXP100_SEND_DATA;
    Curl_expire_clear(data, EXPIRE_100_TIMEOUT);
  }
}

ಸ್ಥಿರ CURLcode cr_exp100_read(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ Curl_creader *reader,
                               ವರ್ಣ *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  ಸಂರಚನಾ cr_exp100_ctx *ctx = reader->ctx;
  timediff_t ms;

  ವಿಕಲ್ಪ(ctx->state) {
  ಪಕ್ಷ EXP100_SENDING_REQUEST:
    ಯದಿ(!Curl_req_sendbuf_empty(data)) {
      /* The initial request data has not been fully sent yet. Do
       * not start the timer yet. */
      DEBUGF(infof(data, "cr_exp100_read, request not full sent yet"));
      *nread = 0;
      *eos = FALSE;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
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
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ EXP100_FAILED:
    DEBUGF(infof(data, "cr_exp100_read, expectation failed, error"));
    *nread = 0;
    *eos = FALSE;
    ಪ್ರತ್ಯಾಗಮ CURLE_READ_ERROR;
  ಪಕ್ಷ EXP100_AWAITING_CONTINUE:
    ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->start);
    ಯದಿ(ms < data->set.expect_100_timeout) {
      DEBUGF(infof(data, "cr_exp100_read, AWAITING_CONTINUE, not expired"));
      *nread = 0;
      *eos = FALSE;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    /* we have waited long enough, continue anyway */
    http_exp100_continue(data, reader);
    infof(data, "Done waiting for 100-continue");
    FALLTHROUGH();
  ಸ್ವಭಾವ:
    DEBUGF(infof(data, "cr_exp100_read, pass through"));
    ಪ್ರತ್ಯಾಗಮ Curl_creader_read(data, reader->next, buf, blen, nread, eos);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cr_exp100_done(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ Curl_creader *reader, ಪೂರ್ಣಾಙ್ಕ premature)
{
  ಸಂರಚನಾ cr_exp100_ctx *ctx = reader->ctx;
  ctx->state = premature ? EXP100_FAILED : EXP100_SEND_DATA;
  Curl_expire_clear(data, EXPIRE_100_TIMEOUT);
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_crtype cr_exp100 = {
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
  ಪರಿಮಾಣ(ಸಂರಚನಾ cr_exp100_ctx)
};

ಸ್ಥಿರ CURLcode http_exp100_add_reader(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_creader *reader = ರಿಕ್ತ;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_exp100, CURL_CR_PROTOCOL);
  ಯದಿ(!result)
    result = Curl_creader_add(data, reader);
  ಯದಿ(!result) {
    ಸಂರಚನಾ cr_exp100_ctx *ctx = reader->ctx;
    ctx->state = EXP100_SENDING_REQUEST;
  }

  ಯದಿ(result && reader)
    Curl_creader_free(data, reader);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ http_exp100_got100(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  ಯದಿ(r)
    http_exp100_continue(data, r);
}

ಸ್ಥಿರ bool http_exp100_is_waiting(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  ಯದಿ(r) {
    ಸಂರಚನಾ cr_exp100_ctx *ctx = r->ctx;
    ಪ್ರತ್ಯಾಗಮ ctx->state == EXP100_AWAITING_CONTINUE;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ http_exp100_send_anyway(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  ಯದಿ(r)
    http_exp100_continue(data, r);
}

ಸ್ಥಿರ bool http_exp100_is_selected(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  ಪ್ರತ್ಯಾಗಮ !!r;
}

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
CURLcode Curl_http_doing_pollset(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ easy_pollset *ps)
{
  /* write mode */
  ಪ್ರತ್ಯಾಗಮ Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

CURLcode Curl_http_perform_pollset(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  ಯದಿ(CURL_REQ_WANT_RECV(data)) {
    result = Curl_pollset_add_in(data, ps, conn->sock[FIRSTSOCKET]);
  }

  /* on a "Expect: 100-continue" timed wait, do not poll for outgoing */
  ಯದಿ(!result && Curl_req_want_send(data) && !http_exp100_is_waiting(data)) {
    result = Curl_pollset_add_out(data, ps, conn->sock[FIRSTSOCKET]);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode http_write_header(ಸಂರಚನಾ Curl_easy *data,
                                  ಧ್ರುವ ವರ್ಣ *hd, size_t hdlen)
{
  CURLcode result;
  ಪೂರ್ಣಾಙ್ಕ writetype;

  /* now, only output this if the header AND body are requested:
   */
  Curl_debug(data, CURLINFO_HEADER_IN, hd, hdlen);

  writetype = CLIENTWRITE_HEADER |
    ((data->req.httpcode / 100 == 1) ? CLIENTWRITE_1XX : 0);

  result = Curl_client_write(data, writetype, hd, hdlen);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  result = Curl_bump_headersize(data, hdlen, FALSE);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  data->req.deductheadercount = (100 <= data->req.httpcode &&
                                 199 >= data->req.httpcode) ?
    data->req.headerbytecount : 0;
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Curl_http_done() gets called after a single HTTP request has been
 * performed.
 */

CURLcode Curl_http_done(ಸಂರಚನಾ Curl_easy *data,
                        CURLcode status, bool premature)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;

  /* Clear multipass flag. If authentication is not done yet, then it will get
   * a chance to be set back to true when we output the next auth header */
  data->state.authhost.multipass = FALSE;
  data->state.authproxy.multipass = FALSE;

  ಯದಿ(curlx_dyn_len(&data->state.headerb)) {
    (ಶೂನ್ಯ)http_write_header(data, curlx_dyn_ptr(&data->state.headerb),
                            curlx_dyn_len(&data->state.headerb));
  }
  curlx_dyn_reset(&data->state.headerb);

  ಯದಿ(status)
    ಪ್ರತ್ಯಾಗಮ status;

  ಯದಿ(!premature && /* this check is pointless when DONE is called before the
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
    ಪ್ರತ್ಯಾಗಮ CURLE_GOT_NOTHING;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Determine if we may use HTTP 1.1 for this request. */
ಸ್ಥಿರ bool http_may_use_1_1(ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data)
{
  ಧ್ರುವ ಸಂರಚನಾ connectdata *conn = data->conn;
  /* We have seen a previous response for *this* transfer with 1.0,
   * on another connection or the same one. */
  ಯದಿ(data->state.http_neg.rcvd_min == 10)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  /* We have seen a previous response on *this* connection with 1.0. */
  ಯದಿ(conn && conn->httpversion_seen == 10)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  /* We want 1.0 and have seen no previous response on *this* connection
     with a higher version (maybe no response at all yet). */
  ಯದಿ(data->state.http_neg.only_10 &&
     (!conn || conn->httpversion_seen <= 10))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  /* We are not restricted to use 1.0 only. */
  ಪ್ರತ್ಯಾಗಮ !data->state.http_neg.only_10;
}

ಸ್ಥಿರ unsigned ವರ್ಣ http_request_version(ಸಂರಚನಾ Curl_easy *data)
{
  unsigned ವರ್ಣ v = Curl_conn_http_version(data, data->conn);
  ಯದಿ(!v) {
    /* No specific HTTP connection filter installed. */
    v = http_may_use_1_1(data) ? 11 : 10;
  }
  ಪ್ರತ್ಯಾಗಮ v;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *get_http_string(ಪೂರ್ಣಾಙ್ಕ httpversion)
{
  ವಿಕಲ್ಪ(httpversion) {
  ಪಕ್ಷ 30:
    ಪ್ರತ್ಯಾಗಮ "3";
  ಪಕ್ಷ 20:
    ಪ್ರತ್ಯಾಗಮ "2";
  ಪಕ್ಷ 11:
    ಪ್ರತ್ಯಾಗಮ "1.1";
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ "1.0";
  }
}

CURLcode Curl_add_custom_headers(ಸಂರಚನಾ Curl_easy *data,
                                 bool is_connect, ಪೂರ್ಣಾಙ್ಕ httpversion,
                                 ಸಂರಚನಾ dynbuf *req)
{
  ಸಂರಚನಾ curl_slist *h[2];
  ಸಂರಚನಾ curl_slist *headers;
  ಪೂರ್ಣಾಙ್ಕ numlists = 1; /* by default */
  ಪೂರ್ಣಾಙ್ಕ i;

#ifndef CURL_DISABLE_PROXY
  ಗಣನಾ Curl_proxy_use proxy;

  ಯದಿ(is_connect)
    proxy = HEADER_CONNECT;
  ಅನ್ಯಥಾ
    proxy = data->conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  ವಿಕಲ್ಪ(proxy) {
  ಪಕ್ಷ HEADER_SERVER:
    h[0] = data->set.headers;
    ಭಙ್ಗ;
  ಪಕ್ಷ HEADER_PROXY:
    h[0] = data->set.headers;
    ಯದಿ(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ HEADER_CONNECT:
    ಯದಿ(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    ಅನ್ಯಥಾ
      h[0] = data->set.headers;
    ಭಙ್ಗ;
  ಪಕ್ಷ HEADER_CONNECT_UDP:
    ಯದಿ(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    ಅನ್ಯಥಾ
      h[0] = data->set.headers;
    ಭಙ್ಗ;
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)is_connect;
  h[0] = data->set.headers;
#endif

  /* loop through one or two lists */
  ಪ್ರತಿ(i = 0; i < numlists; i++) {
    ಪ್ರತಿ(headers = h[i]; headers; headers = headers->next) {
      CURLcode result = CURLE_OK;
      bool blankheader = FALSE;
      ಸಂರಚನಾ Curl_str name;
      ಧ್ರುವ ವರ್ಣ *p = headers->data;
      ಧ್ರುವ ವರ್ಣ *origp = p;
      size_t hlen = strlen(origp);

      /* explicitly asked to send header without content is done by a header
         that ends with a semicolon, but there must be no colon present in the
         name */
      ಯದಿ(!curlx_str_until(&p, &name, hlen, ';') &&
         !curlx_str_single(&p, ';') &&
         !curlx_str_single(&p, '\0') &&
         !memchr(curlx_str(&name), ':', curlx_strlen(&name)))
        blankheader = TRUE;
      ಅನ್ಯಥಾ {
        p = origp;
        ಯದಿ(!curlx_str_until(&p, &name, hlen, ':') &&
           !curlx_str_single(&p, ':')) {
          ಸಂರಚನಾ Curl_str val;
          curlx_str_untilnl(&p, &val, hlen);
          curlx_str_trimblanks(&val);
          ಯದಿ(!curlx_strlen(&val))
            /* no content, do not send this */
            ಅನುವರ್ತ;
        }
        ಅನ್ಯಥಾ
          /* no colon */
          ಅನುವರ್ತ;
      }

      /* a field name is a token and carries no surrounding whitespace, so
         trim the parsed name before matching. Otherwise `Authorization :`
         (space before the colon) slips past the Authorization/Cookie check
         below and gets forwarded to another host on a redirect. */
      curlx_str_trimblanks(&name);

      /* only send this if the contents was non-blank or done special */

      ಯದಿ(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom
            Host: header as that will produce *two* in the same
            request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      ಅನ್ಯಥಾ ಯದಿ(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ಅನ್ಯಥಾ ಯದಿ(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ಅನ್ಯಥಾ ಯದಿ(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      ಅನ್ಯಥಾ ಯದಿ(curlx_str_casecompare(&name, "Connection"))
        /* Connection headers are handled specially */
        ;
      ಅನ್ಯಥಾ ಯದಿ((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        /* HTTP/2 does not support chunked requests */
        ;
      ಅನ್ಯಥಾ ಯದಿ((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      ಅನ್ಯಥಾ ಯದಿ(blankheader) {
        result = curlx_dyn_addn(req, curlx_str(&name), curlx_strlen(&name));
        ಯದಿ(!result)
          result = curlx_dyn_addn(req, STRCONST(":\r\n"));
      }
      ಅನ್ಯಥಾ
        result = curlx_dyn_addf(req, "%s\r\n", origp);

      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ifndef CURL_DISABLE_PARSEDATE
CURLcode Curl_add_timecondition(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ dynbuf *req)
{
  ಧ್ರುವ ಸಂರಚನಾ tm *tm;
  ಸಂರಚನಾ tm keeptime;
  CURLcode result;
  ವರ್ಣ datestr[80];
  ಧ್ರುವ ವರ್ಣ *condp;
  size_t len;

  ಯದಿ(data->set.timecondition == CURL_TIMECOND_NONE)
    /* no condition was asked for */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  result = curlx_gmtime(data->set.timevalue, &keeptime);
  ಯದಿ(result) {
    failf(data, "Invalid TIMEVALUE");
    ಪ್ರತ್ಯಾಗಮ result;
  }
  tm = &keeptime;

  ವಿಕಲ್ಪ(data->set.timecondition) {
  ಸ್ವಭಾವ:
    DEBUGF(infof(data, "invalid time condition"));
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  ಪಕ್ಷ CURL_TIMECOND_IFMODSINCE:
    condp = "If-Modified-Since";
    len = 17;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_TIMECOND_IFUNMODSINCE:
    condp = "If-Unmodified-Since";
    len = 19;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_TIMECOND_LASTMOD:
    condp = "Last-Modified";
    len = 13;
    ಭಙ್ಗ;
  }

  ಯದಿ(Curl_checkheaders(data, condp, len)) {
    /* A custom header was specified; it will be sent instead. */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  /* The If-Modified-Since header family should have their times set in
   * GMT as RFC2616 defines: "All HTTP date/time stamps MUST be
   * represented in Greenwich Mean Time (GMT), without exception. For the
   * purposes of HTTP, GMT is exactly equal to UTC (Coordinated Universal
   * Time)." (see page 20 of RFC2616).
   */

  /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
  curl_msnprintf(datestr, ಪರಿಮಾಣ(datestr),
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
  ಪ್ರತ್ಯಾಗಮ result;
}
#ಅನ್ಯಥಾ
/* disabled */
CURLcode Curl_add_timecondition(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ dynbuf *req)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)req;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif

ಶೂನ್ಯ Curl_http_method(ಸಂರಚನಾ Curl_easy *data,
                      ಧ್ರುವ ವರ್ಣ **method, Curl_HttpReq *reqp)
{
  Curl_HttpReq httpreq = (Curl_HttpReq)data->state.httpreq;
  ಧ್ರುವ ವರ್ಣ *request;
#ifndef CURL_DISABLE_WEBSOCKETS
  ಯದಿ(data->conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
    httpreq = HTTPREQ_GET;
  ಅನ್ಯಥಾ
#endif
  ಯದಿ((data->conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_FTP)) &&
     data->state.upload)
    httpreq = HTTPREQ_PUT;

  /* Now set the 'request' pointer to the proper request string */
  ಯದಿ(!data->state.http_ignorecustom &&
     CURL_EASY_STR(data, STRING_CUSTOMREQUEST)) {
    request = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(data->req.no_body)
      request = "HEAD";
    ಅನ್ಯಥಾ {
      DEBUGASSERT((httpreq >= HTTPREQ_GET) && (httpreq <= HTTPREQ_HEAD));
      ವಿಕಲ್ಪ(httpreq) {
      ಪಕ್ಷ HTTPREQ_POST:
      ಪಕ್ಷ HTTPREQ_POST_FORM:
      ಪಕ್ಷ HTTPREQ_POST_MIME:
        request = "POST";
        ಭಙ್ಗ;
      ಪಕ್ಷ HTTPREQ_PUT:
        request = "PUT";
        ಭಙ್ಗ;
      ಸ್ವಭಾವ: /* this should never happen */
      ಪಕ್ಷ HTTPREQ_GET:
        request = "GET";
        ಭಙ್ಗ;
      ಪಕ್ಷ HTTPREQ_HEAD:
        request = "HEAD";
        ಭಙ್ಗ;
      }
    }
  }
  *method = request;
  *reqp = httpreq;
}

ಸ್ಥಿರ CURLcode http_set_aptr_host(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಧ್ರುವ ವರ್ಣ *ptr = ರಿಕ್ತ;

  curlx_safefree(data->state.http_host);
#ifndef CURL_DISABLE_COOKIES
  curlx_safefree(data->req.cookiehost);
#endif

  ಯದಿ(Curl_peer_equal(data->state.initial_origin, data->state.origin))
    ptr = Curl_checkheaders(data, STRCONST("Host"));

  ಯದಿ(ptr) {
#ifndef CURL_DISABLE_COOKIES
    /* If we have a given custom Host: header, we extract the hostname in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the hostname
       is the same as the first one! */
    ವರ್ಣ *cookiehost;
    CURLcode result = copy_custom_ಮೌಲ್ಯ(ptr, &cookiehost);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    ಯದಿ(!*cookiehost)
      /* ignore empty data */
      curlx_free(cookiehost);
    ಅನ್ಯಥಾ {
      /* If the host begins with '[', we start searching for the port after
         the bracket has been closed */
      ಯದಿ(*cookiehost == '[') {
        ವರ್ಣ *closingbracket;
        /* since the 'cookiehost' is an allocated memory area that will be
           freed later we cannot increment the pointer */
        memmove(cookiehost, cookiehost + 1, strlen(cookiehost) - 1);
        closingbracket = strchr(cookiehost, ']');
        ಯದಿ(closingbracket)
          *closingbracket = 0;
      }
      ಅನ್ಯಥಾ {
        ಪೂರ್ಣಾಙ್ಕ startsearch = 0;
        ವರ್ಣ *colon = strchr(cookiehost + startsearch, ':');
        ಯದಿ(colon)
          *colon = 0; /* The host must not include an embedded port number */
      }
      data->req.cookiehost = cookiehost;
    }
#endif

    ಯದಿ(!curl_strequal("Host:", ptr)) {
      data->state.http_host = curl_maprintf("Host:%s", &ptr[5]);
      ಯದಿ(!data->state.http_host)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
  }
  ಅನ್ಯಥಾ {
    /* This is the  HTTP Host: header, so we want
     * - for IPv6 origins: "[ipv6-address]" where the IPv6 address is
     *  found in origin->hostname, stripped of zoneid/scopeid.
     * - the (IDN converted) origin->hostname (DNS name or IPv4) otherwise.
     * Note: zoneid/scopeid  only applies to local routing and has no
     * meaning on the remote HTTP server (eg. would confuse it). */
    bool ipv6 = (bool)data->state.origin->ipv6;
    ಸಂರಚನಾ dynbuf tmp;
    size_t hlen;
    CURLcode result;

    curlx_dyn_init(&tmp, DYN_HTTP_REQUEST);
    result = curlx_dyn_addn(&tmp, STRCONST("Host: "));
    ಯದಿ(!result && ipv6)
      result = curlx_dyn_addn(&tmp, STRCONST("["));
    ಯದಿ(!result)
      result = curlx_dyn_add(&tmp, data->state.origin->hostname);
    ಯದಿ(!result && ipv6)
      result = curlx_dyn_addn(&tmp, STRCONST("]"));
    ಯದಿ(!result &&
       ((data->state.origin->port != data->state.origin->scheme->defport) ||
       (data->state.origin->scheme->family != conn->scheme->family))) {
      result = curlx_dyn_addf(&tmp, ":%u", data->state.origin->port);
    }

    data->state.http_host = result ? ರಿಕ್ತ : curlx_dyn_take(&tmp, &hlen);
    curlx_dyn_free(&tmp);
    ಪ್ರತ್ಯಾಗಮ result;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Append the request-target to the HTTP request
 */
ಸ್ಥಿರ CURLcode http_target(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ dynbuf *r)
{
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ವರ್ಣ *path = data->state.up.path;
  ಧ್ರುವ ವರ್ಣ *query = data->state.up.query;
#ifndef CURL_DISABLE_PROXY
  ಸಂರಚನಾ connectdata *conn = data->conn;
#endif

  ಯದಿ(CURL_EASY_STR(data, STRING_TARGET)) {
    path = CURL_EASY_STR(data, STRING_TARGET);
    query = ರಿಕ್ತ;
  }

#ifndef CURL_DISABLE_PROXY
  ಯದಿ(conn->bits.origin_is_proxy) {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL, but if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the decoded hostname! */

    /* and no fragment part */
    CURLUcode uc;
    ವರ್ಣ *url;
    CURLU *h = curl_url_dup(data->state.uh);
    ಯದಿ(!h)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

    ಯದಿ(!data->state.origin->ipv6 &&
       (data->state.origin->user_hostname != data->state.origin->hostname)) {
      uc = curl_url_set(h, CURLUPART_HOST, data->state.origin->hostname, 0);
      ಯದಿ(uc) {
        curl_url_cleanup(h);
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      }
    }
    uc = curl_url_set(h, CURLUPART_FRAGMENT, ರಿಕ್ತ, 0);
    ಯದಿ(uc) {
      curl_url_cleanup(h);
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }

    ಯದಿ(data->state.origin->scheme == &Curl_scheme_http) {
      /* when getting HTTP, we do not want the userinfo the URL */
      uc = curl_url_set(h, CURLUPART_USER, ರಿಕ್ತ, 0);
      ಯದಿ(uc) {
        curl_url_cleanup(h);
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      }
      uc = curl_url_set(h, CURLUPART_PASSWORD, ರಿಕ್ತ, 0);
      ಯದಿ(uc) {
        curl_url_cleanup(h);
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(data->state.creds && (data->state.creds->source != CREDS_URL)) {
        /* credentials not from the URL need to be set */
      uc = curl_url_set(h, CURLUPART_USER,
                        data->state.creds->user, CURLU_URLENCODE);
      ಯದಿ(!uc)
        uc = curl_url_set(h, CURLUPART_PASSWORD,
                          data->state.creds->passwd, CURLU_URLENCODE);
      ಯದಿ(uc) {
        curl_url_cleanup(h);
        ಪ್ರತ್ಯಾಗಮ Curl_uc_to_curlcode(uc);
      }
    }

    /* Extract the URL to use in the request. */
    uc = curl_url_get(h, CURLUPART_URL, &url, CURLU_NO_DEFAULT_PORT);
    ಯದಿ(uc) {
      curl_url_cleanup(h);
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }

    curl_url_cleanup(h);

    /* target or URL */
    result = curlx_dyn_add(r, CURL_EASY_STR(data, STRING_TARGET) ?
      CURL_EASY_STR(data, STRING_TARGET) : url);
    curlx_free(url);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    ಯದಿ((data->state.origin->scheme == &Curl_scheme_ftp) &&
       data->set.proxy_transfer_mode) {
      /* when doing ftp, append ;type=<a|i> if not present */
      size_t len = strlen(path);
      bool type_present = FALSE;
      ಯದಿ((len >= 7) && !memcmp(&path[len - 7], ";type=", 6)) {
        ವಿಕಲ್ಪ(Curl_raw_toupper(path[len - 1])) {
        ಪಕ್ಷ 'A':
        ಪಕ್ಷ 'D':
        ಪಕ್ಷ 'I':
          type_present = TRUE;
          ಭಙ್ಗ;
        }
      }
      ಯದಿ(!type_present) {
        result = curlx_dyn_addf(r, ";type=%c",
                                data->state.prefer_ascii ? 'a' : 'i');
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;
      }
    }
  }

  ಅನ್ಯಥಾ
#endif
  {
    result = curlx_dyn_add(r, path);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    ಯದಿ(query)
      result = curlx_dyn_addf(r, "?%s", query);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

#ಯದಿ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
ಸ್ಥಿರ CURLcode set_post_reader(ಸಂರಚನಾ Curl_easy *data, Curl_HttpReq httpreq)
{
  CURLcode result;

  ವಿಕಲ್ಪ(httpreq) {
#ifndef CURL_DISABLE_MIME
  ಪಕ್ಷ HTTPREQ_POST_MIME:
    data->state.mimepost = data->set.mimepostp;
    ಭಙ್ಗ;
#endif
#ifndef CURL_DISABLE_FORM_API
  ಪಕ್ಷ HTTPREQ_POST_FORM:
    /* Convert the form structure into a mime structure, then keep
       the conversion */
    ಯದಿ(!data->state.formp) {
      data->state.formp = curlx_calloc(1, ಪರಿಮಾಣ(curl_mimepart));
      ಯದಿ(!data->state.formp)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      Curl_mime_cleanpart(data->state.formp);
      result = Curl_getformdata(data, data->state.formp, data->set.httppost,
                                data->state.fread_func);
      ಯದಿ(result) {
        curlx_safefree(data->state.formp);
        ಪ್ರತ್ಯಾಗಮ result;
      }
      data->state.mimepost = data->state.formp;
    }
    ಭಙ್ಗ;
#endif
  ಸ್ವಭಾವ:
    data->state.mimepost = ರಿಕ್ತ;
    ಭಙ್ಗ;
  }

  ವಿಕಲ್ಪ(httpreq) {
  ಪಕ್ಷ HTTPREQ_POST_FORM:
  ಪಕ್ಷ HTTPREQ_POST_MIME:
    /* This is form posting using mime data. */
#ifndef CURL_DISABLE_MIME
    ಯದಿ(data->state.mimepost) {
      ಧ್ರುವ ವರ್ಣ *cthdr = Curl_checkheaders(data, STRCONST("Content-Type"));

      /* Read and seek body only. */
      data->state.mimepost->flags |= MIME_BODY_ONLY;

      /* Prepare the mime structure headers & set content type. */

      ಯದಿ(cthdr)
        ಪ್ರತಿ(cthdr += 13; *cthdr == ' '; cthdr++)
          ;
      ಅನ್ಯಥಾ ಯದಿ(data->state.mimepost->kind == MIMEKIND_MULTIPART)
        cthdr = "multipart/form-data";

      curl_mime_headers(data->state.mimepost, data->set.headers, 0);
      result = Curl_mime_prepare_headers(data, data->state.mimepost, cthdr,
                                         ರಿಕ್ತ, MIMESTRATEGY_FORM);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      curl_mime_headers(data->state.mimepost, ರಿಕ್ತ, 0);
      result = Curl_creader_set_mime(data, data->state.mimepost);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    ಅನ್ಯಥಾ
#endif
    {
      result = Curl_creader_set_null(data);
    }
    data->state.infilesize = Curl_creader_total_length(data);
    ಪ್ರತ್ಯಾಗಮ result;

  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ Curl_creader_set_null(data);
  }
  /* never reached */
}
#endif

ಸ್ಥಿರ CURLcode set_reader(ಸಂರಚನಾ Curl_easy *data, Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
  curl_off_t postsize = data->state.infilesize;

  DEBUGASSERT(data->conn);

  ಯದಿ(data->req.authneg) {
    ಪ್ರತ್ಯಾಗಮ Curl_creader_set_null(data);
  }

  ವಿಕಲ್ಪ(httpreq) {
  ಪಕ್ಷ HTTPREQ_PUT: /* Let's PUT the data to the server! */
    ಪ್ರತ್ಯಾಗಮ postsize ? Curl_creader_set_fread(data, postsize) :
      Curl_creader_set_null(data);

#ಯದಿ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  ಪಕ್ಷ HTTPREQ_POST_FORM:
  ಪಕ್ಷ HTTPREQ_POST_MIME:
    ಪ್ರತ್ಯಾಗಮ set_post_reader(data, httpreq);
#endif

  ಪಕ್ಷ HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */
    /* the size of the post body */
    ಯದಿ(!postsize) {
      result = Curl_creader_set_null(data);
    }
    ಅನ್ಯಥಾ ಯದಿ(data->set.postfields) {
      size_t plen = curlx_sotouz_range(postsize, 0, SIZE_MAX);
      ಯದಿ(plen == SIZE_MAX)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      ಅನ್ಯಥಾ ಯದಿ(plen)
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      ಅನ್ಯಥಾ
        result = Curl_creader_set_null(data);
    }
    ಅನ್ಯಥಾ {
      /* we read the bytes from the callback. In case "chunked" encoding
       * is forced by the application, we disregard `postsize`. This is
       * a backward compatibility decision to earlier versions where
       * chunking disregarded this. See issue #13229. */
      bool chunked = FALSE;
      ವರ್ಣ *ptr = Curl_checkheaders(data, STRCONST("Transfer-Encoding"));
      ಯದಿ(ptr) {
        /* Some kind of TE is requested, check if 'chunked' is chosen */
        chunked = Curl_compareheader(ptr, STRCONST("Transfer-Encoding:"),
                                     STRCONST("chunked"));
      }
      result = Curl_creader_set_fread(data, chunked ? -1 : postsize);
    }
    ಪ್ರತ್ಯಾಗಮ result;

  ಸ್ವಭಾವ:
    /* HTTP GET/HEAD download, has no body, needs no Content-Length */
    data->state.infilesize = 0;
    ಪ್ರತ್ಯಾಗಮ Curl_creader_set_null(data);
  }
  /* not reached */
}

ಸ್ಥಿರ CURLcode http_resume(ಸಂರಚನಾ Curl_easy *data, Curl_HttpReq httpreq)
{
  ಯದಿ((HTTPREQ_POST == httpreq || HTTPREQ_POST_FORM == httpreq ||
      HTTPREQ_POST_MIME == httpreq || HTTPREQ_PUT == httpreq) &&
     data->state.resume_from) {
    failf(data, "HTTP upload cannot be resumed");
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode http_req_set_TE(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ dynbuf *req,
                                ಪೂರ್ಣಾಙ್ಕ httpversion)
{
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ವರ್ಣ *ptr;

  ptr = Curl_checkheaders(data, STRCONST("Transfer-Encoding"));
  ಯದಿ(ptr) {
    /* Some kind of TE is requested, check if 'chunked' is chosen */
    data->req.upload_chunky =
      Curl_compareheader(ptr,
                         STRCONST("Transfer-Encoding:"), STRCONST("chunked"));
    ಯದಿ(data->req.upload_chunky && (httpversion >= 20)) {
      infof(data, "suppressing chunked transfer encoding on connection "
            "using HTTP version 2 or higher");
      data->req.upload_chunky = FALSE;
    }
  }
  ಅನ್ಯಥಾ {
    curl_off_t req_clen = Curl_creader_total_length(data);

    ಯದಿ(req_clen < 0) {
      /* indeterminate request content length */
      ಯದಿ(httpversion > 10) {
        /* On HTTP/1.1, enable chunked, on HTTP/2 and later we do not
         * need it */
        data->req.upload_chunky = (httpversion < 20);
      }
      ಅನ್ಯಥಾ {
        failf(data, "Chunky upload is not supported by HTTP 1.0");
        ಪ್ರತ್ಯಾಗಮ CURLE_UPLOAD_FAILED;
      }
    }
    ಅನ್ಯಥಾ {
      /* else, no chunky upload */
      data->req.upload_chunky = FALSE;
    }

    ಯದಿ(data->req.upload_chunky)
      result = curlx_dyn_add(req, "Transfer-Encoding: chunked\r\n");
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode addexpect(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ dynbuf *r,
                          ಪೂರ್ಣಾಙ್ಕ httpversion, bool *announced_exp100)
{
  CURLcode result;
  ವರ್ಣ *ptr;

  *announced_exp100 = FALSE;
  /* Avoid Expect: 100-continue if Upgrade: is used */
  ಯದಿ(data->req.upgr101 != UPGR101_NONE)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  /* For really small puts we do not use Expect: headers at all, and for
     the somewhat bigger ones we allow the app to disable it. Make
     sure that the expect100header is always set to the preferred value
     here. */
  ptr = Curl_checkheaders(data, STRCONST("Expect"));
  ಯದಿ(ptr) {
    *announced_exp100 =
      Curl_compareheader(ptr, STRCONST("Expect:"), STRCONST("100-continue"));
  }
  ಅನ್ಯಥಾ ಯದಿ(!data->state.disableexpect && (httpversion == 11)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    curl_off_t client_len = Curl_creader_client_length(data);
    ಯದಿ(client_len > EXPECT_100_THRESHOLD || client_len < 0) {
      result = curlx_dyn_addn(r, STRCONST("Expect: 100-continue\r\n"));
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      *announced_exp100 = TRUE;
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode http_add_content_hds(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ dynbuf *r,
                                     ಪೂರ್ಣಾಙ್ಕ httpversion,
                                     Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
  curl_off_t req_clen;
  bool announced_exp100 = FALSE;

  DEBUGASSERT(data->conn);
  ಯದಿ(data->req.upload_chunky) {
    result = Curl_httpchunk_add_reader(data);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  /* Get the request body length that has been set up */
  req_clen = Curl_creader_total_length(data);
  ವಿಕಲ್ಪ(httpreq) {
  ಪಕ್ಷ HTTPREQ_PUT:
  ಪಕ್ಷ HTTPREQ_POST:
#ಯದಿ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  ಪಕ್ಷ HTTPREQ_POST_FORM:
  ಪಕ್ಷ HTTPREQ_POST_MIME:
#endif
    /* We only set Content-Length and allow a custom Content-Length if
       we do not upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length).
       We do not override a custom "Content-Length" header, but during
       authentication negotiation that header is suppressed.
     */
    ಯದಿ(req_clen >= 0 && !data->req.upload_chunky &&
       (data->req.authneg ||
        !Curl_checkheaders(data, STRCONST("Content-Length")))) {
      /* we allow replacing this header if not during auth negotiation,
         although it is not wise to actually set your own */
      result = curlx_dyn_addf(r, "Content-Length: %" FMT_OFF_T "\r\n",
                              req_clen);
    }
    ಯದಿ(result)
      ಗಚ್ಛ out;

#ifndef CURL_DISABLE_MIME
    /* Output mime-generated headers. */
    ಯದಿ(data->state.mimepost &&
       ((httpreq == HTTPREQ_POST_FORM) || (httpreq == HTTPREQ_POST_MIME))) {
      ಸಂರಚನಾ curl_slist *hdr;

      ಪ್ರತಿ(hdr = data->state.mimepost->curlheaders; hdr; hdr = hdr->next) {
        result = curlx_dyn_addf(r, "%s\r\n", hdr->data);
        ಯದಿ(result)
          ಗಚ್ಛ out;
      }
    }
#endif
    ಯದಿ(httpreq == HTTPREQ_POST &&
       !Curl_checkheaders(data, STRCONST("Content-Type"))) {
      result = curlx_dyn_addn(r, STRCONST("Content-Type: application/"
                                          "x-www-form-urlencoded\r\n"));
      ಯದಿ(result)
        ಗಚ್ಛ out;
    }
    result = addexpect(data, r, httpversion, &announced_exp100);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }

  Curl_pgrsSetUploadSize(data, req_clen);
  ಯದಿ(announced_exp100)
    result = http_exp100_add_reader(data);

out:
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifndef CURL_DISABLE_COOKIES

ಸ್ಥಿರ CURLcode http_cookies(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ dynbuf *r)
{
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ವರ್ಣ *addcookies = ರಿಕ್ತ;
  bool linecap = FALSE;
  ಯದಿ(CURL_EASY_STR(data, STRING_COOKIE) &&
     !Curl_checkheaders(data, STRCONST("Cookie")) &&
     Curl_auth_allowed_to_host(data))
    addcookies = CURL_EASY_STR(data, STRING_COOKIE);

  ಯದಿ(data->cookies || addcookies) {
    ಸಂರಚನಾ Curl_llist list;
    ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ = 0;

    ಯದಿ(data->cookies && data->state.cookie_engine) {
      bool okay;
      ಧ್ರುವ ವರ್ಣ *host = data->req.cookiehost ?
        data->req.cookiehost : data->state.origin->hostname;
      Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
      result = Curl_cookie_getlist(data, &okay, host, &list);
      ಯದಿ(!result && okay) {
        ಸಂರಚನಾ Curl_llist_ಗಂಟು *n;
        size_t clen = 8; /* hold the size of the generated Cookie: header */

        /* loop through all cookies that matched */
        ಪ್ರತಿ(n = Curl_llist_head(&list); n; n = Curl_ಗಂಟು_next(n)) {
          ಸಂರಚನಾ Cookie *co = Curl_ಗಂಟು_elem(n);
          ಯದಿ(co->ಮೌಲ್ಯ) {
            size_t add;
            ಯದಿ(!ಎಣಿಕೆ) {
              result = curlx_dyn_addn(r, STRCONST("Cookie: "));
              ಯದಿ(result)
                ಭಙ್ಗ;
            }
            add = strlen(co->name) + strlen(co->ಮೌಲ್ಯ) + 1;
            ಯದಿ(clen + add >= MAX_COOKIE_HEADER_LEN) {
              infof(data, "Restricted outgoing cookies due to header size, "
                    "'%s' not sent", co->name);
              linecap = TRUE;
              ಭಙ್ಗ;
            }
            result = curlx_dyn_addf(r, "%s%s=%s", ಎಣಿಕೆ ? "; " : "",
                                    co->name, co->ಮೌಲ್ಯ);
            ಯದಿ(result)
              ಭಙ್ಗ;
            clen += add + (ಎಣಿಕೆ ? 2 : 0);
            ಎಣಿಕೆ++;
          }
        }
        Curl_llist_destroy(&list, ರಿಕ್ತ);
      }
      Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    }
    ಯದಿ(addcookies && !result && !linecap) {
      ಯದಿ(!ಎಣಿಕೆ)
        result = curlx_dyn_addn(r, STRCONST("Cookie: "));
      ಯದಿ(!result) {
        result = curlx_dyn_addf(r, "%s%s", ಎಣಿಕೆ ? "; " : "", addcookies);
        ಎಣಿಕೆ++;
      }
    }
    ಯದಿ(ಎಣಿಕೆ && !result)
      result = curlx_dyn_addn(r, STRCONST("\r\n"));

    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}
#ಅನ್ಯಥಾ
#define http_cookies(a, b) CURLE_OK
#endif

ಸ್ಥಿರ CURLcode http_range(ಸಂರಚನಾ Curl_easy *data,
                           Curl_HttpReq httpreq)
{
  ಯದಿ(data->state.use_range) {
    /*
     * A range is selected. We use different headers whether we are downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    ಯದಿ(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_HEAD)) &&
       !Curl_checkheaders(data, STRCONST("Range"))) {
      /* if a line like this was already allocated, free the previous one */
      curlx_free(data->state.rangeline);
      data->state.rangeline = curl_maprintf("Range: bytes=%s\r\n",
                                                 data->state.range);
      ಯದಿ(!data->state.rangeline)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
    ಅನ್ಯಥಾ ಯದಿ((httpreq == HTTPREQ_POST || httpreq == HTTPREQ_PUT) &&
            !Curl_checkheaders(data, STRCONST("Content-Range"))) {
      curl_off_t req_clen = Curl_creader_total_length(data);
      /* if a line like this was already allocated, free the previous one */
      curlx_free(data->state.rangeline);

      ಯದಿ(data->set.set_resume_from < 0) {
        /* Upload resume was asked for, but we do not know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes 0-%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n", req_clen - 1, req_clen);
      }
      ಅನ್ಯಥಾ ಯದಿ(data->state.resume_from) {
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
      ಅನ್ಯಥಾ {
        /* Range was selected and then we pass the incoming range and append
           total size */
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes %s/%" FMT_OFF_T "\r\n",
                        data->state.range, req_clen);
      }
      ಯದಿ(!data->state.rangeline)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode http_firstwrite(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ SingleRequest *k = &data->req;

  ಯದಿ(data->req.newurl) {
    ಯದಿ(conn->bits.close) {
      /* Abort after the headers if "follow Location" is set
         and we are set to close anyway. */
      CURL_REQ_CLEAR_RECV(data);
      k->done = TRUE;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    /* We have a new URL to load, but since we want to be able to reuse this
       connection properly, we read the full response in "ignore more" */
    k->ignorebody = TRUE;
    infof(data, "Ignoring the response-body");
  }
  ಯದಿ(data->state.resume_from && !k->content_range &&
     (data->state.httpreq == HTTPREQ_GET) &&
     !k->ignorebody) {

    ಯದಿ(k->size == data->state.resume_from) {
      /* The resume point is at the end of file, consider this fine even if it
         does not allow resume from here. */
      infof(data, "The entire document is already downloaded");
      streamclose(conn);
      /* Abort download */
      CURL_REQ_CLEAR_RECV(data);
      k->done = TRUE;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }

    /* we wanted to resume a download, although the server does not seem to
     * support this and we did this with a GET (if it was not a GET we did a
     * POST or PUT resume) */
    failf(data, "HTTP server does not seem to support "
          "byte ranges. Cannot resume.");
    ಪ್ರತ್ಯಾಗಮ CURLE_RANGE_ERROR;
  }

  ಯದಿ(data->set.timecondition && !data->state.range &&
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
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  } /* we have a time condition */

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode http_check_new_conn(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಧ್ರುವ ವರ್ಣ *info_version = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *alpn;
  CURLcode result;

  alpn = Curl_conn_get_alpn_negotiated(data, conn);
  ಯದಿ(alpn && !strcmp("h3", alpn)) {
#ifndef CURL_DISABLE_PROXY
    ಯದಿ(!conn->bits.origin_is_proxy)
#endif
      DEBUGASSERT(Curl_conn_http_version(data, conn) == 30);
    info_version = "HTTP/3";
  }
  ಅನ್ಯಥಾ ಯದಿ(alpn && !strcmp("h2", alpn)) {
#ifndef CURL_DISABLE_PROXY
    ಯದಿ((Curl_conn_http_version(data, conn) != 20) &&
       conn->bits.origin_is_proxy) {
      result = Curl_http2_switch(data);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    ಅನ್ಯಥಾ
#endif
    DEBUGASSERT(Curl_conn_http_version(data, conn) == 20);
    info_version = "HTTP/2";
  }
  ಅನ್ಯಥಾ {
    /* Check if user wants to use HTTP/2 with clear TCP */
    ಯದಿ(Curl_http2_may_switch(data)) {
      DEBUGF(infof(data, "HTTP/2 over clean TCP"));
      result = Curl_http2_switch(data);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      info_version = "HTTP/2";
      /* There is no ALPN here, but the connection is now definitely h2 */
      conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(conn);
    }
    ಅನ್ಯಥಾ
      info_version = "HTTP/1.x";
  }

  ಯದಿ(info_version)
    infof(data, "using %s", info_version);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode http_add_connection_hd(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ dynbuf *req)
{
  ಸಂರಚನಾ curl_slist *head;
  ಧ್ರುವ ವರ್ಣ *sep = "Connection: ";
  CURLcode result = CURLE_OK;
  size_t rlen = curlx_dyn_len(req);
  bool skip;

  /* Add the 1st custom "Connection: " header, if there is one */
  ಪ್ರತಿ(head = data->set.headers; head; head = head->next) {
    ಯದಿ(curl_strnequal(head->data, "Connection", 10) &&
       Curl_headersep(head->data[10]) &&
       !http_header_is_empty(head->data)) {
      ವರ್ಣ *ಮೌಲ್ಯ;
      result = copy_custom_ಮೌಲ್ಯ(head->data, &ಮೌಲ್ಯ);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      result = curlx_dyn_addf(req, "%s%s", sep, ಮೌಲ್ಯ);
      sep = ", ";
      curlx_free(ಮೌಲ್ಯ);
      ಭಙ್ಗ; /* leave, having added 1st one */
    }
  }

  /* add our internal Connection: header values, if we have any */
  ಯದಿ(!result && data->state.http_hd_te) {
    result = curlx_dyn_addf(req, "%s%s", sep, "TE");
    sep = ", ";
  }
  ಯದಿ(!result && data->state.http_hd_upgrade) {
    result = curlx_dyn_addf(req, "%s%s", sep, "Upgrade");
    sep = ", ";
  }
  ಯದಿ(!result && data->state.http_hd_h2_settings) {
    result = curlx_dyn_addf(req, "%s%s", sep, "HTTP2-Settings");
  }
  ಯದಿ(!result && (rlen < curlx_dyn_len(req)))
    result = curlx_dyn_addn(req, STRCONST("\r\n"));
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* Add all user-defined Connection: headers after the first */
  skip = TRUE;
  ಪ್ರತಿ(head = data->set.headers; head; head = head->next) {
    ಯದಿ(curl_strnequal(head->data, "Connection", 10) &&
       Curl_headersep(head->data[10]) &&
       !http_header_is_empty(head->data)) {
      ಯದಿ(skip) {
        skip = FALSE;
        ಅನುವರ್ತ;
      }
      result = curlx_dyn_addf(req, "%s\r\n", head->data);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Header identifier in order we send them by default */
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
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

ಸ್ಥಿರ CURLcode http_add_hd(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ dynbuf *req,
                            http_hd_t id,
                            unsigned ವರ್ಣ httpversion,
                            ಧ್ರುವ ವರ್ಣ *method,
                            Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
#ಯದಿ !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_PROXY) || \
  !defined(CURL_DISABLE_WEBSOCKETS)
  ಸಂರಚನಾ connectdata *conn = data->conn;
#endif
  ವಿಕಲ್ಪ(id) {
  ಪಕ್ಷ H1_HD_REQUEST:
    /* add the main request stuff */
    /* GET/HEAD/POST/PUT */
    result = curlx_dyn_addf(req, "%s ", method);
    ಯದಿ(!result)
      result = http_target(data, req);
    ಯದಿ(!result)
      result = curlx_dyn_addf(req, " HTTP/%s\r\n",
                              get_http_string(httpversion));
    ಭಙ್ಗ;

  ಪಕ್ಷ H1_HD_HOST:
    ಯದಿ(data->state.http_host) {
      result = curlx_dyn_add(req, data->state.http_host);
      ಯದಿ(!result)
        result = curlx_dyn_addn(req, STRCONST("\r\n"));
    }
    ಭಙ್ಗ;

#ifndef CURL_DISABLE_PROXY
  ಪಕ್ಷ H1_HD_PROXY_AUTH:
    ಯದಿ(data->req.hd_proxy_auth)
      result = curlx_dyn_add(req, data->req.hd_proxy_auth);
    ಭಙ್ಗ;
#endif

  ಪಕ್ಷ H1_HD_AUTH:
    ಯದಿ(data->req.hd_auth)
      result = curlx_dyn_add(req, data->req.hd_auth);
    ಭಙ್ಗ;

  ಪಕ್ಷ H1_HD_RANGE:
    ಯದಿ(data->state.use_range && data->state.rangeline)
      result = curlx_dyn_add(req, data->state.rangeline);
    ಭಙ್ಗ;

  ಪಕ್ಷ H1_HD_USER_AGENT: {
    ಧ್ರುವ ವರ್ಣ *ua = CURL_EASY_STR(data, STRING_USERAGENT);
    ಯದಿ(ua && *ua && !Curl_checkheaders(data, STRCONST("User-Agent")))
      result = curlx_dyn_addf(req, "User-Agent: %s\r\n", ua);
    ಭಙ್ಗ;
  }

  ಪಕ್ಷ H1_HD_ACCEPT:
    ಯದಿ(!Curl_checkheaders(data, STRCONST("Accept")))
      result = curlx_dyn_add(req, "Accept: */*\r\n");
    ಭಙ್ಗ;

  ಪಕ್ಷ H1_HD_TE:
#ifdef HAVE_LIBZ
    ಯದಿ(!Curl_checkheaders(data, STRCONST("TE")) &&
       data->set.http_transfer_encoding) {
      data->state.http_hd_te = TRUE;
      result = curlx_dyn_add(req, "TE: gzip\r\n");
    }
#endif
    ಭಙ್ಗ;

  ಪಕ್ಷ H1_HD_ACCEPT_ENCODING: {
    ಧ್ರುವ ವರ್ಣ *enc = CURL_EASY_STR(data, STRING_ENCODING);
    ಯದಿ(enc && !Curl_checkheaders(data, STRCONST("Accept-Encoding")))
      result = curlx_dyn_addf(req, "Accept-Encoding: %s\r\n", enc);
    ಭಙ್ಗ;
  }

  ಪಕ್ಷ H1_HD_REFERER:
    ಯದಿ(Curl_bufref_ptr(&data->state.referer) &&
       !Curl_checkheaders(data, STRCONST("Referer")))
      result = curlx_dyn_addf(req, "Referer: %s\r\n",
                              Curl_bufref_ptr(&data->state.referer));
    ಭಙ್ಗ;

#ifndef CURL_DISABLE_PROXY
  ಪಕ್ಷ H1_HD_PROXY_CONNECTION:
    ಯದಿ(conn->bits.origin_is_proxy &&
       !Curl_checkheaders(data, STRCONST("Proxy-Connection")) &&
       !Curl_checkProxyheaders(data, data->conn, STRCONST("Proxy-Connection")))
      result = curlx_dyn_add(req, "Proxy-Connection: Keep-Alive\r\n");
    ಭಙ್ಗ;
#endif

  ಪಕ್ಷ H1_HD_TRANSFER_ENCODING:
    result = http_req_set_TE(data, req, httpversion);
    ಭಙ್ಗ;

#ifndef CURL_DISABLE_ALTSVC
  ಪಕ್ಷ H1_HD_ALT_USED:
    ಯದಿ(conn->bits.altused && conn->via_peer &&
       !Curl_checkheaders(data, STRCONST("Alt-Used")))
      result = curlx_dyn_addf(req, "Alt-Used: %s:%u\r\n",
                              conn->via_peer->hostname, conn->via_peer->port);
    ಭಙ್ಗ;
#endif

  ಪಕ್ಷ H1_HD_UPGRADE:
    ಯದಿ(!Curl_conn_is_ssl(data->conn, FIRSTSOCKET) && (httpversion < 20) &&
       (data->state.http_neg.wanted & CURL_HTTP_V2x) &&
       data->state.http_neg.h2_upgrade) {
      /* append HTTP2 upgrade magic stuff to the HTTP request if it is not done
         over SSL */
      result = Curl_http2_request_upgrade(req, data);
    }
#ifndef CURL_DISABLE_WEBSOCKETS
    ಯದಿ(!result && conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
      result = Curl_ws_request(data, req);
#endif
    ಭಙ್ಗ;

  ಪಕ್ಷ H1_HD_COOKIES:
    result = http_cookies(data, req);
    ಭಙ್ಗ;

  ಪಕ್ಷ H1_HD_CONDITIONALS:
    result = Curl_add_timecondition(data, req);
    ಭಙ್ಗ;

  ಪಕ್ಷ H1_HD_CUSTOM:
    result = Curl_add_custom_headers(data, FALSE, httpversion, req);
    ಭಙ್ಗ;

  ಪಕ್ಷ H1_HD_CONTENT:
    result = http_add_content_hds(data, req, httpversion, httpreq);
    ಭಙ್ಗ;

  ಪಕ್ಷ H1_HD_CONNECTION: {
    result = http_add_connection_hd(data, req);
    ಭಙ್ಗ;
  }

  ಪಕ್ಷ H1_HD_LAST:
    result = curlx_dyn_addn(req, STRCONST("\r\n"));
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Curl_http() gets called from the generic multi_do() function when an HTTP
 * request is to be performed. This creates and sends a properly constructed
 * HTTP request.
 */
CURLcode Curl_http(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  Curl_HttpReq httpreq;
  ಧ್ರುವ ವರ್ಣ *method;
  ಸಂರಚನಾ dynbuf req;
  unsigned ವರ್ಣ httpversion;
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

  ಯದಿ(!data->conn->bits.reuse) {
    result = http_check_new_conn(data);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  /* Add collecting of headers written to client. For a new connection,
   * we might have done that already, but reuse
   * or multiplex needs it here as well. */
  result = Curl_headers_init(data);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  data->state.http_hd_te = FALSE;
  data->state.http_hd_upgrade = FALSE;
  data->state.http_hd_h2_settings = FALSE;

  /* what kind of request do we need to send? */
  Curl_http_method(data, &method, &httpreq);

  /* select host to send */
  result = http_set_aptr_host(data);
  /* setup the authentication headers, how that method and host are known */
  ಯದಿ(!result)
    result = Curl_http_output_auth(data, data->conn, method, httpreq,
                                   data->state.up.path,
                                   data->state.up.query, FALSE);
  /* Setup input reader, resume information and ranges */
  ಯದಿ(!result)
    result = set_reader(data, httpreq);
  ಯದಿ(!result)
    result = http_resume(data, httpreq);
  ಯದಿ(!result)
    result = http_range(data, httpreq);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  httpversion = http_request_version(data);
  /* Add request line and all headers to `req` */
  ಪ್ರತಿ(hd_id = 0; hd_id <= H1_HD_LAST; ++hd_id) {
    result = http_add_hd(data, &req, (http_hd_t)hd_id,
                         httpversion, method, httpreq);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  /* setup variables for the upcoming transfer and send */
  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);
  result = Curl_req_send(data, &req, httpversion);

  ಯದಿ((httpversion >= 20) && data->req.upload_chunky)
    /* upload_chunky was set above to set up the request in a chunky fashion,
       but is disabled here again to avoid that the chunked encoded version is
       actually used when sending the request body over h2 */
    data->req.upload_chunky = FALSE;

out:
  ಯದಿ(result == CURLE_TOO_LARGE)
    failf(data, "HTTP request too large");

  curlx_dyn_free(&req);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  STATUS_UNKNOWN, /* not enough data to tell yet */
  STATUS_DONE, /* a status line was read */
  STATUS_BAD /* not a status line */
} statusline;

/* Check a string for a prefix. Check no more than 'len' bytes */
ಸ್ಥಿರ bool checkprefixmax(ಧ್ರುವ ವರ್ಣ *prefix, ಧ್ರುವ ವರ್ಣ *buffer, size_t len)
{
  size_t ch = CURLMIN(strlen(prefix), len);
  ಪ್ರತ್ಯಾಗಮ curl_strnequal(prefix, buffer, ch);
}

/*
 * checkhttpprefix()
 *
 * Returns TRUE if member of the list matches prefix of string
 */
ಸ್ಥಿರ statusline checkhttpprefix(ಸಂರಚನಾ Curl_easy *data,
                                  ಧ್ರುವ ವರ್ಣ *s, size_t len)
{
  ಸಂರಚನಾ curl_slist *head = data->set.http200aliases;
  statusline rc = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;

  ಯಾವತ್(head) {
    ಯದಿ(checkprefixmax(head->data, s, len)) {
      rc = onmatch;
      ಭಙ್ಗ;
    }
    head = head->next;
  }

  ಯದಿ((rc != STATUS_DONE) && checkprefixmax("HTTP/", s, len))
    rc = onmatch;

  ಪ್ರತ್ಯಾಗಮ rc;
}

#ifndef CURL_DISABLE_RTSP
ಸ್ಥಿರ statusline checkrtspprefix(ಸಂರಚನಾ Curl_easy *data,
                                  ಧ್ರುವ ವರ್ಣ *s, size_t len)
{
  statusline status = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;
  (ಶೂನ್ಯ)data;
  ಯದಿ(checkprefixmax("RTSP/", s, len))
    status = onmatch;

  ಪ್ರತ್ಯಾಗಮ status;
}
#endif /* CURL_DISABLE_RTSP */

ಸ್ಥಿರ statusline checkprotoprefix(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ connectdata *conn,
                                   ಧ್ರುವ ವರ್ಣ *s, size_t len)
{
#ifndef CURL_DISABLE_RTSP
  ಯದಿ(conn->scheme->protocol & CURLPROTO_RTSP)
    ಪ್ರತ್ಯಾಗಮ checkrtspprefix(data, s, len);
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)conn;
#endif /* CURL_DISABLE_RTSP */

  ಪ್ರತ್ಯಾಗಮ checkhttpprefix(data, s, len);
}

/* HTTP header has field name `n` (a string constant) */
#define HD_IS(hd, hdlen, n) \
  (((hdlen) >= (ಪರಿಮಾಣ(n) - 1)) && curl_strnequal(n, hd, ಪರಿಮಾಣ(n) - 1))

#define HD_VAL(hd, hdlen, n) \
  ((((hdlen) >= (ಪರಿಮಾಣ(n) - 1)) && (hd) && \
    curl_strnequal(n, hd, ಪರಿಮಾಣ(n) - 1)) ? ((hd) + (ಪರಿಮಾಣ(n) - 1)) : ರಿಕ್ತ)

/* HTTP header has field name `n` (a string constant) and contains `v`
 * (a string constant) in its value(s) */
#define HD_IS_AND_SAYS(hd, hdlen, n, v) \
  (HD_IS(hd, hdlen, n) && \
   ((hdlen) > ((ಪರಿಮಾಣ(n) - 1) + (ಪರಿಮಾಣ(v) - 1))) && \
   Curl_compareheader(hd, STRCONST(n), STRCONST(v)))

/*
 * http_header_a() parses a single response header starting with A.
 */
ಸ್ಥಿರ CURLcode http_header_a(ಸಂರಚನಾ Curl_easy *data,
                              ಧ್ರುವ ವರ್ಣ *hd, size_t hdlen)
{
#ifndef CURL_DISABLE_ALTSVC
  ಧ್ರುವ ವರ್ಣ *v;
  v = (data->asi &&
       (Curl_xfer_is_secure(data) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_ALTSVC_HTTP")
#ಅನ್ಯಥಾ
        0
#endif
         )) ? HD_VAL(hd, hdlen, "Alt-Svc:") : ರಿಕ್ತ;
  ಯದಿ(v) {
    /* the ALPN of the current request */
    ಸಂರಚನಾ SingleRequest *k = &data->req;
    ಗಣನಾ alpnid id = (k->httpversion == 30) ? ALPN_h3 :
      (k->httpversion == 20) ? ALPN_h2 : ALPN_h1;
    ಪ್ರತ್ಯಾಗಮ Curl_altsvc_parse(data, data->asi, v, data->state.origin, id);
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)hd;
  (ಶೂನ್ಯ)hdlen;
#endif
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * http_header_c() parses a single response header starting with C.
 */
ಸ್ಥಿರ CURLcode http_header_c(ಸಂರಚನಾ Curl_easy *data,
                              ಧ್ರುವ ವರ್ಣ *hd, size_t hdlen)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ SingleRequest *k = &data->req;
  ಧ್ರುವ ವರ್ಣ *v;

  /* Check for Content-Length: header lines to get size. Browsers insist we
     should accept multiple Content-Length headers and that a comma separated
     list also is fine and then we should accept them all as long as they are
     the same value. Different values trigger error.
   */
  v = (!k->http_bodyless && !data->set.ignorecl && !k->ignore_cl) ?
    HD_VAL(hd, hdlen, "Content-Length:") : ರಿಕ್ತ;
  ಯದಿ(v) {
    ಕುರು {
      curl_off_t contentlength;
      ಪೂರ್ಣಾಙ್ಕ offt = curlx_str_numblanks(&v, &contentlength);

      ಯದಿ(offt == STRE_OVERFLOW) {
        /* out of range */
        ಯದಿ(data->set.max_filesize) {
          failf(data, "Maximum file size exceeded");
          ಪ್ರತ್ಯಾಗಮ CURLE_FILESIZE_EXCEEDED;
        }
        streamclose(conn);
        infof(data, "Overflow Content-Length: value");
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
      ಅನ್ಯಥಾ {
        ಯದಿ((offt == STRE_OK) &&
           ((k->size == -1) || /* not set to something before */
            (k->size == contentlength))) { /* or the same value */

          k->size = contentlength;
          curlx_str_passblanks(&v);

          /* on a comma, loop and get the next instead */
          ಯದಿ(!curlx_str_single(&v, ','))
            ಅನುವರ್ತ;

          ಯದಿ(!curlx_str_newline(&v)) {
            k->maxdownload = k->size;
            ಪ್ರತ್ಯಾಗಮ CURLE_OK;
          }
        }
        /* negative, different value or rubbish - bad HTTP */
        failf(data, "Invalid Content-Length: value");
        ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
      }
    } ಯಾವತ್(1);
  }
  v = (!k->http_bodyless && CURL_EASY_STR(data, STRING_ENCODING)) ?
    HD_VAL(hd, hdlen, "Content-Encoding:") : ರಿಕ್ತ;
  ಯದಿ(v) {
    /*
     * Process Content-Encoding. Look for the values: identity, gzip, deflate,
     * compress, x-gzip and x-compress. x-gzip and x-compress are the same as
     * gzip and compress. (Sec 3.5 RFC 2616). zlib cannot handle compress.
     * Errors are handled further down when the response body is processed
     */
    ಪ್ರತ್ಯಾಗಮ Curl_build_unencoding_ರಾಶಿ(data, v, FALSE);
  }
  /* check for Content-Type: header lines to get the MIME-type */
  v = HD_VAL(hd, hdlen, "Content-Type:");
  ಯದಿ(v) {
    ವರ್ಣ *contenttype = Curl_copy_header_ಮೌಲ್ಯ(hd);
    ಯದಿ(!contenttype)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    ಯದಿ(!*contenttype)
      /* ignore empty data */
      curlx_free(contenttype);
    ಅನ್ಯಥಾ {
      curlx_free(data->info.contenttype);
      data->info.contenttype = contenttype;
    }
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಯದಿ((k->httpversion < 20) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "close")) {
    /*
     * [RFC 2616, section 8.1.2.1]
     * "Connection: close" is HTTP/1.1 language and means that
     * the connection will close when this request has been
     * served.
     */
    connclose(conn);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಯದಿ((k->httpversion == 10) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "keep-alive")) {
    /*
     * An HTTP/1.0 reply with the 'Connection: keep-alive' line
     * tells us the connection will be kept alive for our
     * pleasure. Default action for 1.0 is to close.
     *
     * [RFC2068, section 19.7.1] */
    connkeep(conn);
    infof(data, "HTTP/1.0 connection set to keep alive");
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  v = !k->http_bodyless ? HD_VAL(hd, hdlen, "Content-Range:") : ರಿಕ್ತ;
  ಯದಿ(v) {
    /* Content-Range: bytes [num]-
       Content-Range: bytes: [num]-
       Content-Range: [num]-
       Content-Range: [asterisk]/[total]

       The second format was added since Sun's webserver
       JavaWebServer/1.1.1 obviously sends the header this way!
       The third added since some servers use that!
       The fourth means the requested range was unsatisfied.
     */

    ಧ್ರುವ ವರ್ಣ *ptr = v;

    /* Move forward until first digit or asterisk */
    ಯಾವತ್(*ptr && !ISDIGIT(*ptr) && *ptr != '*')
      ptr++;

    /* if it truly stopped on a digit */
    ಯದಿ(ISDIGIT(*ptr)) {
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&ptr, &k->offset, CURL_OFF_T_MAX) &&
         (data->state.resume_from == k->offset))
        /* we asked for a resume and we got it */
        k->content_range = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ(k->httpcode < 300)
      data->state.resume_from = 0; /* get everything */
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * http_header_l() parses a single response header starting with L.
 */
ಸ್ಥಿರ CURLcode http_header_l(ಸಂರಚನಾ Curl_easy *data,
                              ಧ್ರುವ ವರ್ಣ *hd, size_t hdlen)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ SingleRequest *k = &data->req;
  ಧ್ರುವ ವರ್ಣ *v = (!k->http_bodyless &&
                   (data->set.timecondition || data->set.get_filetime)) ?
    HD_VAL(hd, hdlen, "Last-Modified:") : ರಿಕ್ತ;
  ಯದಿ(v) {
    ಯದಿ(Curl_getdate_capped(v, &k->timeofdoc))
      k->timeofdoc = 0;
    ಯದಿ(data->set.get_filetime)
      data->info.filetime = k->timeofdoc;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಯದಿ(HD_IS(hd, hdlen, "Location:")) {
    /* this is the URL that the server advises us to use instead */
    ವರ್ಣ *location = Curl_copy_header_ಮೌಲ್ಯ(hd);
    ಯದಿ(!location)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    ಯದಿ(!*location ||
       (data->req.location && !strcmp(data->req.location, location))) {
      /* ignore empty header, or exact repeat of a previous one */
      curlx_free(location);
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    ಅನ್ಯಥಾ {
      /* has value and is not an exact repeat */
      ಯದಿ(data->req.location) {
        failf(data, "Multiple Location headers");
        curlx_free(location);
        ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
      }
      data->req.location = location;

      ಯದಿ((k->httpcode >= 300 && k->httpcode < 400) &&
         data->set.http_follow_mode) {
        CURLcode result;
        DEBUGASSERT(!data->req.newurl);
        data->req.newurl = curlx_strdup(data->req.location); /* clone */
        ಯದಿ(!data->req.newurl)
          ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

        /* some cases of POST and PUT etc needs to rewind the data
           stream at this point */
        result = http_perhapsrewind(data, conn);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;

        /* mark the next request as a followed location: */
        data->state.this_is_a_follow = TRUE;
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * http_header_p() parses a single response header starting with P.
 */
ಸ್ಥಿರ CURLcode http_header_p(ಸಂರಚನಾ Curl_easy *data,
                              ಧ್ರುವ ವರ್ಣ *hd, size_t hdlen)
{
  ಸಂರಚನಾ SingleRequest *k = &data->req;

#ifndef CURL_DISABLE_PROXY
  ಧ್ರುವ ವರ್ಣ *v = HD_VAL(hd, hdlen, "Proxy-Connection:");
  ಯದಿ(v) {
    ಸಂರಚನಾ connectdata *conn = data->conn;
    ಯದಿ((k->httpversion == 10) && conn->http_proxy.peer &&
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
    ಅನ್ಯಥಾ ಯದಿ((k->httpversion == 11) && conn->http_proxy.peer &&
            HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "close")) {
      /*
       * We get an HTTP/1.1 response from a proxy and it says it will
       * close down after this transfer.
       */
      connclose(conn);
      infof(data, "HTTP/1.1 proxy connection set close");
    }
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
#endif
  ಯದಿ((407 == k->httpcode) && HD_IS(hd, hdlen, "Proxy-authenticate:")) {
    ವರ್ಣ *auth;
    CURLcode result;
#ifndef CURL_DISABLE_PROXY
    ಯದಿ(!data->conn->bits.origin_is_proxy)
      /* this response is not from a forward proxy, not for us */
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
#endif
    auth = Curl_copy_header_ಮೌಲ್ಯ(hd);
    result = auth ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    ಯದಿ(!result) {
      result = Curl_http_ಒಳಹರಿವು_auth(data, TRUE, auth);
      curlx_free(auth);
    }
    ಪ್ರತ್ಯಾಗಮ result;
  }
#ifdef USE_SPNEGO
  ಯದಿ(HD_IS(hd, hdlen, "Persistent-Auth:")) {
    ಸಂರಚನಾ connectdata *conn = data->conn;
    ಸಂರಚನಾ negotiatedata *negdata = Curl_auth_nego_get(conn, FALSE);
    ಸಂರಚನಾ auth *authp = &data->state.authhost;
    ಯದಿ(!negdata)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    ಯದಿ(authp->picked == CURLAUTH_NEGOTIATE) {
      ವರ್ಣ *persistentauth = Curl_copy_header_ಮೌಲ್ಯ(hd);
      ಯದಿ(!persistentauth)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      negdata->noauthpersist = !!checkprefix("false", persistentauth);
      negdata->havenoauthpersist = TRUE;
      infof(data, "Negotiate: noauthpersist -> %d, header part: %s",
            negdata->noauthpersist, persistentauth);
      curlx_free(persistentauth);
    }
  }
#endif
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * http_header_r() parses a single response header starting with R.
 */
ಸ್ಥಿರ CURLcode http_header_r(ಸಂರಚನಾ Curl_easy *data,
                              ಧ್ರುವ ವರ್ಣ *hd, size_t hdlen)
{
  ಧ್ರುವ ವರ್ಣ *v = HD_VAL(hd, hdlen, "Retry-After:");
  ಯದಿ(v) {
    /* Retry-After = HTTP-date / delay-seconds */
    curl_off_t retry_after = 0; /* zero for unknown or "now" */
    time_t date = 0;
    curlx_str_passblanks(&v);

    /* try it as a date first, because a date can otherwise start with and
       get treated as a number */
    ಯದಿ(!Curl_getdate_capped(v, &date)) {
      time_t current = time(ರಿಕ್ತ);
      ಯದಿ(date >= current)
        /* convert date to number of seconds into the future */
        retry_after = date - current;
    }
    ಅನ್ಯಥಾ
      /* Try it as a decimal number, ignore errors */
      (ಶೂನ್ಯ)curlx_str_ಸಂಖ್ಯೆ(&v, &retry_after, CURL_OFF_T_MAX);
    /* limit to 6 hours max. this is not documented so that it can be changed
       in the future if necessary. */
    ಯದಿ(retry_after > 21600)
      retry_after = 21600;
    data->info.retry_after = retry_after;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * http_header_s() parses a single response header starting with S.
 */
ಸ್ಥಿರ CURLcode http_header_s(ಸಂರಚನಾ Curl_easy *data,
                              ಧ್ರುವ ವರ್ಣ *hd, size_t hdlen)
{
#ಯದಿ !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_HSTS)
  ಧ್ರುವ ವರ್ಣ *v;
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)hd;
  (ಶೂನ್ಯ)hdlen;
#endif

#ifndef CURL_DISABLE_COOKIES
  v = (data->cookies && data->state.cookie_engine) ?
    HD_VAL(hd, hdlen, "Set-Cookie:") : ರಿಕ್ತ;
  ಯದಿ(v) {
    /* If there is a custom-set Host: name, use it here, or else use
     * real peer hostname. */
    ಧ್ರುವ ವರ್ಣ *host = data->req.cookiehost ?
      data->req.cookiehost : data->state.origin->hostname;
    ಧ್ರುವ unsigned ವರ್ಣ secure_context = Curl_secure_context(data, host) ?
      COOKIE_SECURE : 0;
    CURLcode result;
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    result = Curl_cookie_add(data, data->cookies, v, host,
                             data->state.up.path,
                             COOKIE_HTTPHEADER | secure_context);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    ಪ್ರತ್ಯಾಗಮ result;
  }
#endif
#ifndef CURL_DISABLE_HSTS
  /* If enabled, the header is incoming and this is over HTTPS */
  v = (data->hsts &&
       (Curl_xfer_is_secure(data) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_HSTS_HTTP")
#ಅನ್ಯಥಾ
        0
#endif
         )
    ) ? HD_VAL(hd, hdlen, "Strict-Transport-Security:") : ರಿಕ್ತ;
  ಯದಿ(v) {
    CURLcode result = Curl_hsts_parse(
      data->hsts, data->state.origin->hostname, v);
    ಯದಿ(result) {
      ಯದಿ(result == CURLE_OUT_OF_MEMORY)
        ಪ್ರತ್ಯಾಗಮ result;
      infof(data, "Illegal STS header skipped");
    }
#ifdef DEBUGBUILD
    ಅನ್ಯಥಾ
      infof(data, "Parsed STS header fine (%zu entries)",
            Curl_llist_ಎಣಿಕೆ(&data->hsts->list));
#endif
  }
#endif

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * http_header_t() parses a single response header starting with T.
 */
ಸ್ಥಿರ CURLcode http_header_t(ಸಂರಚನಾ Curl_easy *data,
                              ಧ್ರುವ ವರ್ಣ *hd, size_t hdlen)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ SingleRequest *k = &data->req;

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
  ಧ್ರುವ ವರ್ಣ *v = (!k->http_bodyless &&
                   (data->state.httpreq != HTTPREQ_HEAD) &&
                   (k->httpcode != 304)) ?
    HD_VAL(hd, hdlen, "Transfer-Encoding:") : ರಿಕ್ತ;
  ಯದಿ(v) {
    /* One or more encodings. We check for chunked and/or a compression
       algorithm. */
    CURLcode result = Curl_build_unencoding_ರಾಶಿ(data, v, TRUE);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    ಯದಿ(!k->chunk) {
      /* if this is not chunked, only close can signal the end of this
       * transfer as Content-Length is said not to be trusted for
       * transfer-encoding! */
      CURL_TRC_M(data, "HTTP/1.1 transfer-encoding without chunks");
      connclose(conn);
      k->ignore_cl = TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  v = HD_VAL(hd, hdlen, "Trailer:");
  ಯದಿ(v) {
    data->req.resp_trailer = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * http_header_w() parses a single response header starting with W.
 */
ಸ್ಥಿರ CURLcode http_header_w(ಸಂರಚನಾ Curl_easy *data,
                              ಧ್ರುವ ವರ್ಣ *hd, size_t hdlen)
{
  ಸಂರಚನಾ SingleRequest *k = &data->req;
  CURLcode result = CURLE_OK;

  ಯದಿ((401 == k->httpcode) && HD_IS(hd, hdlen, "WWW-Authenticate:")) {
    ವರ್ಣ *auth = Curl_copy_header_ಮೌಲ್ಯ(hd);
    ಯದಿ(!auth)
      result = CURLE_OUT_OF_MEMORY;
    ಅನ್ಯಥಾ {
      result = Curl_http_ಒಳಹರಿವು_auth(data, FALSE, auth);
      curlx_free(auth);
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * http_header() parses a single response header.
 */
ಸ್ಥಿರ CURLcode http_header(ಸಂರಚನಾ Curl_easy *data,
                            ಧ್ರುವ ವರ್ಣ *hd, size_t hdlen)
{
  CURLcode result = CURLE_OK;

  ವಿಕಲ್ಪ(hd[0]) {
  ಪಕ್ಷ 'a':
  ಪಕ್ಷ 'A':
    result = http_header_a(data, hd, hdlen);
    ಭಙ್ಗ;
  ಪಕ್ಷ 'c':
  ಪಕ್ಷ 'C':
    result = http_header_c(data, hd, hdlen);
    ಭಙ್ಗ;
  ಪಕ್ಷ 'l':
  ಪಕ್ಷ 'L':
    result = http_header_l(data, hd, hdlen);
    ಭಙ್ಗ;
  ಪಕ್ಷ 'p':
  ಪಕ್ಷ 'P':
    result = http_header_p(data, hd, hdlen);
    ಭಙ್ಗ;
  ಪಕ್ಷ 'r':
  ಪಕ್ಷ 'R':
    result = http_header_r(data, hd, hdlen);
    ಭಙ್ಗ;
  ಪಕ್ಷ 's':
  ಪಕ್ಷ 'S':
    result = http_header_s(data, hd, hdlen);
    ಭಙ್ಗ;
  ಪಕ್ಷ 't':
  ಪಕ್ಷ 'T':
    result = http_header_t(data, hd, hdlen);
    ಭಙ್ಗ;
  ಪಕ್ಷ 'w':
  ಪಕ್ಷ 'W':
    result = http_header_w(data, hd, hdlen);
    ಭಙ್ಗ;
  }

  ಯದಿ(!result) {
    ಸಂರಚನಾ connectdata *conn = data->conn;
    ಯದಿ(conn->scheme->protocol & CURLPROTO_RTSP)
      result = Curl_rtsp_parseheader(data, hd);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Called after the first HTTP response line (the status line) has been
 * received and parsed.
 */
ಸ್ಥಿರ CURLcode http_statusline(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ SingleRequest *k = &data->req;

  ವಿಕಲ್ಪ(k->httpversion) {
  ಪಕ್ಷ 10:
  ಪಕ್ಷ 11:
#ifdef USE_HTTP2
  ಪಕ್ಷ 20:
#endif
#ifdef USE_HTTP3
  ಪಕ್ಷ 30:
#endif
    /* no major version switch mid-connection */
    ಯದಿ(k->httpversion_sent &&
       (k->httpversion / 10 != k->httpversion_sent / 10)) {
      failf(data, "Version mismatch (from HTTP/%d to HTTP/%d)",
            k->httpversion_sent / 10, k->httpversion / 10);
      ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
    }
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    failf(data, "Unsupported HTTP version (%d.%d) in response",
          k->httpversion / 10, k->httpversion % 10);
    ಪ್ರತ್ಯಾಗಮ CURLE_UNSUPPORTED_PROTOCOL;
  }

  data->info.httpcode = k->httpcode;
  data->info.httpversion = k->httpversion;
  conn->httpversion_seen = k->httpversion;

  ಯದಿ(!data->state.http_neg.rcvd_min ||
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
  ಯದಿ(data->state.resume_from && data->state.httpreq == HTTPREQ_GET &&
     k->httpcode == 416) {
    /* "Requested Range Not Satisfiable", proceed and pretend this is no
       error */
    k->ignorebody = TRUE; /* Avoid appending error msg to good data. */
  }

  ಯದಿ(k->httpversion == 10) {
    /* Default action for HTTP/1.0 must be to close, unless
       we get one of those fancy headers that tell us the
       server keeps it open for us! */
    infof(data, "HTTP 1.0, assume close after body");
    connclose(conn);
  }

  k->http_bodyless = k->httpcode >= 100 && k->httpcode < 200;
  ವಿಕಲ್ಪ(k->httpcode) {
  ಪಕ್ಷ 304:
    /* (quote from RFC2616, section 10.3.5): The 304 response
     * MUST NOT contain a message-body, and thus is always
     * terminated by the first empty line after the header
     * fields. */
    ಯದಿ(data->set.timecondition)
      data->info.timecond = TRUE;
    FALLTHROUGH();
  ಪಕ್ಷ 204:
    /* (quote from RFC2616, section 10.2.5): The server has
     * fulfilled the request but does not need to return an
     * entity-body ... The 204 response MUST NOT include a
     * message-body, and thus is always terminated by the first
     * empty line after the header fields. */
    k->size = 0;
    k->maxdownload = 0;
    k->http_bodyless = TRUE;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Content-Length must be ignored if any Transfer-Encoding is present in the
   response. Refer to RFC 7230 section 3.3.3 and RFC2616 section 4.4. This is
   figured out here after all headers have been received but before the final
   call to the user's header callback, so that a valid content length can be
   retrieved by the user in the final call. */
ಸ್ಥಿರ CURLcode http_size(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ SingleRequest *k = &data->req;
  ಯದಿ(data->req.ignore_cl || k->chunk) {
    k->size = k->maxdownload = -1;
  }
  ಅನ್ಯಥಾ ಯದಿ(k->size != -1) {
    ಯದಿ(data->set.max_filesize &&
       !k->ignorebody &&
       (k->size > data->set.max_filesize)) {
      failf(data, "Maximum file size exceeded");
      ಪ್ರತ್ಯಾಗಮ CURLE_FILESIZE_EXCEEDED;
    }
    ಯದಿ(k->ignorebody)
      infof(data, "setting size while ignoring");
    Curl_pgrsSetDownloadSize(data, k->size);
    k->maxdownload = k->size;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_verify_header(ಸಂರಚನಾ Curl_easy *data,
                            ಧ್ರುವ ವರ್ಣ *hd, size_t hdlen)
{
  ಸಂರಚನಾ SingleRequest *k = &data->req;
  size_t i;
  bool has_colon = FALSE;
  ಪ್ರತಿ(i = 0; i < hdlen; i++) {
    ವಿಕಲ್ಪ(hd[i]) {
    ಪಕ್ಷ '\0':
      /* this is bad, bail out */
      failf(data, "NUL byte in header");
      ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
    ಪಕ್ಷ '\r':
      ಯದಿ(i < hdlen - 2) {
        /* CR may only precede the LF, nothing else */
        failf(data, "Carriage return found in header");
        ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ ':':
      has_colon = TRUE;
      ಭಙ್ಗ;
    }
  }
  ಯದಿ(k->headerline < 2)
    /* the first "header" is the status-line and it has no colon */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಯದಿ(((hd[0] == ' ') || (hd[0] == '\t')) && k->headerline > 2)
    /* line folding, cannot happen on line 2 */
    ;
  ಅನ್ಯಥಾ ಯದಿ(!has_colon) {
    /* this is bad, bail out */
    failf(data, "Header without colon");
    ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_bump_headersize(ಸಂರಚನಾ Curl_easy *data,
                              size_t delta,
                              bool connect_only)
{
  size_t bad = 0;
  unsigned ಪೂರ್ಣಾಙ್ಕ max = MAX_HTTP_RESP_HEADER_SIZE;
  ಯದಿ(delta < MAX_HTTP_RESP_HEADER_SIZE) {
    data->info.header_size += (unsigned ಪೂರ್ಣಾಙ್ಕ)delta;
    data->req.allheadercount += (unsigned ಪೂರ್ಣಾಙ್ಕ)delta;
    ಯದಿ(!connect_only)
      data->req.headerbytecount += (unsigned ಪೂರ್ಣಾಙ್ಕ)delta;
    ಯದಿ(data->req.allheadercount > max)
      bad = data->req.allheadercount;
    ಅನ್ಯಥಾ ಯದಿ(data->info.header_size > (max * 20)) {
      bad = data->info.header_size;
      max *= 20;
    }
  }
  ಅನ್ಯಥಾ
    bad = data->req.allheadercount + delta;
  ಯದಿ(bad) {
    failf(data, "Too large response headers: %zu > %u", bad, max);
    ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Handle a 101 Switching Protocols response. Performs the actual protocol
 * upgrade to HTTP/2 or WebSocket based on what was requested.
 */
ಸ್ಥಿರ CURLcode http_on_101_upgrade(ಸಂರಚನಾ Curl_easy *data,
                                    ಧ್ರುವ ವರ್ಣ *buf, size_t blen,
                                    size_t *pconsumed,
                                    bool *conn_changed)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ SingleRequest *k = &data->req;

#ಯದಿ !defined(USE_NGHTTP2) && defined(CURL_DISABLE_WEBSOCKETS)
  (ಶೂನ್ಯ)buf;
  (ಶೂನ್ಯ)blen;
  (ಶೂನ್ಯ)pconsumed;
#ಅನ್ಯಥಾ
  CURLcode result;
  ಪೂರ್ಣಾಙ್ಕ upgr101_requested = k->upgr101;
#endif

  ಯದಿ(k->httpversion_sent != 11) {
    /* invalid for other HTTP versions */
    failf(data, "server sent 101 response while not talking HTTP/1.1");
    ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
  }

  /* Whatever the success, upgrade was selected. */
  k->upgr101 = UPGR101_RECEIVED;
  conn->bits.upgrade_in_progress = FALSE;
  *conn_changed = TRUE;

  /* To be fully compliant, we would check the "Upgrade:" response header to
   * mention the protocol we requested. */
#ifdef USE_NGHTTP2
  ಯದಿ(upgr101_requested == UPGR101_H2) {
    /* Switch to HTTP/2, where we will get more responses. blen bytes in buf
     * are already h2 protocol bytes */
    infof(data, "Received 101, Switching to HTTP/2");
    result = Curl_http2_upgrade(data, conn, FIRSTSOCKET, buf, blen);
    ಯದಿ(!result)
      *pconsumed += blen;
    ಪ್ರತ್ಯಾಗಮ result;
  }
#endif
#ifndef CURL_DISABLE_WEBSOCKETS
  ಯದಿ(upgr101_requested == UPGR101_WS) {
    /* Switch to WebSocket, where we now stream ws frames. blen bytes in buf
     * are already ws protocol bytes */
    infof(data, "Received 101, Switching to WebSocket");
    result = Curl_ws_accept(data, buf, blen);
    ಯದಿ(!result)
      *pconsumed += blen; /* ws accept handled the data */
    ಪ್ರತ್ಯಾಗಮ result;
  }
#endif
  /* We silently accept this as the final response. What are we switching to
   * if we did not ask for an Upgrade? Maybe the application provided an
   * `Upgrade: xxx` header? */
  k->header = FALSE;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Handle 1xx intermediate HTTP responses. Sets up state for more
 * headers and processes 100-continue and 101 upgrade responses.
 */
ಸ್ಥಿರ CURLcode http_on_1xx_response(ಸಂರಚನಾ Curl_easy *data,
                                     ಧ್ರುವ ವರ್ಣ *buf, size_t blen,
                                     size_t *pconsumed,
                                     bool *conn_changed)
{
  ಸಂರಚನಾ SingleRequest *k = &data->req;

  /* "A user agent MAY ignore unexpected 1xx status responses."
   * By default, we expect to get more responses after this one. */
  k->header = TRUE;
  k->headerline = 0; /* restart the header line counter */

  ವಿಕಲ್ಪ(k->httpcode) {
  ಪಕ್ಷ 100:
    /* We have made an HTTP PUT or POST and this is 1.1-lingo that tells us
     * that the server is OK with this and ready to receive the data. */
    http_exp100_got100(data);
    ಭಙ್ಗ;
  ಪಕ್ಷ 101:
    ಪ್ರತ್ಯಾಗಮ http_on_101_upgrade(data, buf, blen, pconsumed, conn_changed);
  ಸ್ವಭಾವ:
    /* The server may send us other 1xx responses, like informative 103. This
     * has no influence on request processing and we expect to receive a
     * final response eventually. */
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ಯದಿ defined(USE_NTLM) || defined(USE_SPNEGO)
/*
 * Check if NTLM or SPNEGO authentication negotiation failed due to
 * connection closure (typically on HTTP/1.0 servers).
 */
ಸ್ಥಿರ ಶೂನ್ಯ http_check_auth_closure(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ connectdata *conn)
{
  /* At this point we have some idea about the fate of the connection. If we
     are closing the connection it may result auth failure. */
#ifdef USE_NTLM
  ಯದಿ(conn->bits.close &&
     (((data->req.httpcode == 401) &&
       (conn->http_ntlm_state == NTLMSTATE_TYPE2)) ||
      ((data->req.httpcode == 407) &&
       (conn->proxy_ntlm_state == NTLMSTATE_TYPE2)))) {
    infof(data, "Connection closure while negotiating auth (HTTP 1.0?)");
    data->state.authproblem = TRUE;
  }
#endif
#ifdef USE_SPNEGO
  ಯದಿ(conn->bits.close &&
    (((data->req.httpcode == 401) &&
      (conn->http_negotiate_state == CURL_NEGO_AUTH_RECV)) ||
     ((data->req.httpcode == 407) &&
      (conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)))) {
    infof(data, "Connection closure while negotiating auth (HTTP 1.0?)");
    data->state.authproblem = TRUE;
  }
  ಯದಿ((conn->http_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (data->req.httpcode != 401)) {
    conn->http_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
  ಯದಿ((conn->proxy_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (data->req.httpcode != 407)) {
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
#endif
}
#ಅನ್ಯಥಾ
#define http_check_auth_closure(x, y) /* empty */
#endif

/*
 * Handle an error response (>= 300) received while still sending the
 * request body. Deals with 417 Expectation Failed retries, keep-sending
 * on error, and aborting the send.
 */
ಸ್ಥಿರ CURLcode http_handle_send_error(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ SingleRequest *k = &data->req;
  CURLcode result = CURLE_OK;

  ಯದಿ(!data->req.authneg && !conn->bits.close &&
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

    ವಿಕಲ್ಪ(data->state.httpreq) {
    ಪಕ್ಷ HTTPREQ_PUT:
    ಪಕ್ಷ HTTPREQ_POST:
    ಪಕ್ಷ HTTPREQ_POST_FORM:
    ಪಕ್ಷ HTTPREQ_POST_MIME:
      /* We got an error response. If this happened before the
       * whole request body has been sent we stop sending and
       * mark the connection for closure after we have read the
       * entire response. */
      ಯದಿ(!Curl_req_done_sending(data)) {
        ಯದಿ((k->httpcode == 417) && http_exp100_is_selected(data)) {
          /* 417 Expectation Failed - try again without the
             Expect header */
          ಯದಿ(!k->writebytecount && http_exp100_is_waiting(data)) {
            infof(data, "Got HTTP failure 417 while waiting for a 100");
          }
          ಅನ್ಯಥಾ {
            infof(data, "Got HTTP failure 417 while sending data");
            streamclose(conn);
            result = http_perhapsrewind(data, conn);
            ಯದಿ(result)
              ಪ್ರತ್ಯಾಗಮ result;
          }
          data->state.disableexpect = TRUE;
          Curl_req_abort_sending(data);
          DEBUGASSERT(!data->req.newurl);
          data->req.newurl = Curl_bufref_dup(&data->state.url);
          ಯದಿ(!data->req.newurl)
            ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
        }
        ಅನ್ಯಥಾ ಯದಿ(data->set.http_keep_sending_on_error) {
          infof(data, "HTTP error before end of send, keep sending");
          http_exp100_send_anyway(data);
        }
        ಅನ್ಯಥಾ {
          infof(data, "HTTP error before end of send, stop sending");
          streamclose(conn);
          result = Curl_req_abort_sending(data);
          ಯದಿ(result)
            ಪ್ರತ್ಯಾಗಮ result;
        }
      }
      ಭಙ್ಗ;

    ಸ್ವಭಾವ: /* default label present to avoid compiler warnings */
      ಭಙ್ಗ;
    }
  }

  ಯದಿ(Curl_creader_will_rewind(data) && !Curl_req_done_sending(data)) {
    /* We rewind before next send, continue sending now */
    infof(data, "Keep sending data to get tossed away");
    CURL_REQ_SET_SEND(data);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode http_on_response(ಸಂರಚನಾ Curl_easy *data,
                                 ಧ್ರುವ ವರ್ಣ *last_hd, size_t last_hd_len,
                                 ಧ್ರುವ ವರ್ಣ *buf, size_t blen,
                                 size_t *pconsumed)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ SingleRequest *k = &data->req;
  bool conn_changed = FALSE;

  (ಶೂನ್ಯ)buf; /* not used without HTTP2 enabled */
  *pconsumed = 0;

  ಯದಿ(k->upgr101 == UPGR101_RECEIVED) {
    /* supposedly upgraded to http2 now */
    ಯದಿ(data->req.httpversion != 20)
      infof(data, "Lying server, not serving HTTP/2");
  }

  ಯದಿ(k->httpcode < 200 && last_hd) {
    /* Intermediate responses might trigger processing of more responses,
     * write the last header to the client before proceeding. */
    result = http_write_header(data, last_hd, last_hd_len);
    last_hd = ರಿಕ್ತ; /* handled it */
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  ಯದಿ(k->httpcode < 100) {
    failf(data, "Unsupported response code in HTTP response");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ ಯದಿ(k->httpcode < 200) {
    result = http_on_1xx_response(data, buf, blen, pconsumed, &conn_changed);
    ಗಚ್ಛ out;
  }

  /* k->httpcode >= 200, final response */
  k->header = FALSE;
  ಯದಿ(conn->bits.upgrade_in_progress) {
    /* Asked for protocol upgrade, but it was not selected */
    conn->bits.upgrade_in_progress = FALSE;
    conn_changed = TRUE;
  }

  ಯದಿ((k->size == -1) && !k->chunk && !conn->bits.close &&
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
  ಯದಿ(data->req.upgr101 == UPGR101_WS) {
    failf(data, "Refused WebSocket upgrade: %d", k->httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
    ಗಚ್ಛ out;
  }
#endif

  /* Check if this response means the transfer errored. */
  ಯದಿ(http_should_fail(data, data->req.httpcode)) {
    failf(data, "The requested URL returned error: %d",
          k->httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
    ಗಚ್ಛ out;
  }

  /* Curl_http_auth_act() checks what authentication methods that are
   * available and decides which one (if any) to use. It will set 'newurl' if
   * an auth method was picked. */
  result = Curl_http_auth_act(data);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  ಯದಿ(k->httpcode >= 300) {
    result = http_handle_send_error(data);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  /* final response without error, prepare to receive the body */
  result = http_firstwrite(data);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  /* This is the last response that we get for the current request. Check on
   * the body size and determine if the response is complete. */
  result = http_size(data);
  ಯದಿ(result)
    ಗಚ್ಛ out;

#ifndef CURL_DISABLE_RTSP
  /* An RTSP response to any method may carry a body, which is announced
     with Content-Length (RFC 2326 section 12.14). Read it instead of
     treating it as excess data, unless the application asked for no body
     with CURLOPT_NOBODY. */
  ಯದಿ((conn->scheme->protocol & CURLPROTO_RTSP) && data->req.no_body &&
     !data->set.opt_no_body && (k->size > 0))
    data->req.no_body = FALSE;
#endif

  /* If we requested a "no body", this is a good time to get
   * out and return home.
   */
  ಯದಿ(data->req.no_body)
    k->download_done = TRUE;

  /* If max download size is *zero* (nothing) we already have nothing and can
     safely return ok now! For HTTP/2, we would like to call
     http2_handle_stream_close to properly close a stream. In order to do
     this, we keep reading until we close the stream. */
  ಯದಿ((k->maxdownload == 0) && (k->httpversion_sent < 20))
    k->download_done = TRUE;

out:
  ಯದಿ(last_hd)
    /* if not written yet, write it now */
    result = Curl_1st_fatal(result,
                            http_write_header(data, last_hd, last_hd_len));
  ಯದಿ(conn_changed)
    /* poke the multi handle to allow pending pipewait to retry */
    Curl_multi_connchanged(data->multi);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode http_rw_hd(ಸಂರಚನಾ Curl_easy *data,
                           ಧ್ರುವ ವರ್ಣ *hd, size_t hdlen,
                           ಧ್ರುವ ವರ್ಣ *buf_remain, size_t blen,
                           size_t *pconsumed)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ SingleRequest *k = &data->req;
  ಪೂರ್ಣಾಙ್ಕ writetype;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  *pconsumed = 0;
  ಯದಿ((0x0a == *hd) || (0x0d == *hd)) {
    /* Empty header line means end of headers! */
    ಸಂರಚನಾ dynbuf last_header;
    size_t consumed;

    curlx_dyn_init(&last_header, hdlen + 1);
    result = curlx_dyn_addn(&last_header, hd, hdlen);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

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
    ಪ್ರತ್ಯಾಗಮ result;
  }

  /*
   * Checks for special headers coming up.
   */

  writetype = CLIENTWRITE_HEADER;
  ಯದಿ(!k->headerline++) {
    /* This is the first header, it MUST be the error code line
       or else we consider this to be the body right away! */
    bool fine_statusline = FALSE;

    k->httpversion = 0; /* Do not know yet */
    ಯದಿ(data->conn->scheme->protocol & PROTO_FAMILY_HTTP) {
      /*
       * https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.2
       *
       * The response code is always a three-digit number in HTTP as the spec
       * says. We allow any three-digit number here, but we cannot make
       * guarantees on future behaviors since it is not within the protocol.
       */
      ಧ್ರುವ ವರ್ಣ *p = hd;

      curlx_str_passblanks(&p);
      ಯದಿ(!strncmp(p, "HTTP/", 5)) {
        p += 5;
        ವಿಕಲ್ಪ(*p) {
        ಪಕ್ಷ '1':
          p++;
          ಯದಿ((p[0] == '.') && (p[1] == '0' || p[1] == '1')) {
            ಯದಿ(ISBLANK(p[2])) {
              k->httpversion = (unsigned ವರ್ಣ)(10 + (p[1] - '0'));
              p += 3;
              ಯದಿ(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
                k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
                  (p[2] - '0');
                /* RFC 9112 requires a single space following the status code,
                   but the browsers do not so let's not insist */
                fine_statusline = TRUE;
              }
            }
          }
          ಯದಿ(!fine_statusline) {
            failf(data, "Unsupported HTTP/1 subversion in response");
            ಪ್ರತ್ಯಾಗಮ CURLE_UNSUPPORTED_PROTOCOL;
          }
          ಭಙ್ಗ;
        ಪಕ್ಷ '2':
        ಪಕ್ಷ '3':
          ಯದಿ(!ISBLANK(p[1]))
            ಭಙ್ಗ;
          k->httpversion = (unsigned ವರ್ಣ)((*p - '0') * 10);
          p += 2;
          ಯದಿ(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
            k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
              (p[2] - '0');
            p += 3;
            ಯದಿ(!ISBLANK(*p))
              ಭಙ್ಗ;
            fine_statusline = TRUE;
          }
          ಭಙ್ಗ;
        ಸ್ವಭಾವ: /* unsupported */
          failf(data, "Unsupported HTTP version in response");
          ಪ್ರತ್ಯಾಗಮ CURLE_UNSUPPORTED_PROTOCOL;
        }
      }

      ಯದಿ(!fine_statusline) {
        /* If user has set option HTTP200ALIASES,
           compare header line against list of aliases */
        statusline check = checkhttpprefix(data, hd, hdlen);
        ಯದಿ(check == STATUS_DONE) {
          fine_statusline = TRUE;
          k->httpcode = 200;
          k->httpversion = 10;
        }
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(data->conn->scheme->protocol & CURLPROTO_RTSP) {
      ಧ್ರುವ ವರ್ಣ *p = hd;
      ಸಂರಚನಾ Curl_str ver;
      curl_off_t status;
      /* we set the max string a little excessive to forgive some leading
         spaces */
      ಯದಿ(!curlx_str_until(&p, &ver, 32, ' ') &&
         !curlx_str_single(&p, ' ') &&
         !curlx_str_ಸಂಖ್ಯೆ(&p, &status, 999)) {
        curlx_str_trimblanks(&ver);
        ಯದಿ(curlx_str_cmp(&ver, "RTSP/1.0")) {
          k->httpcode = (ಪೂರ್ಣಾಙ್ಕ)status;
          fine_statusline = TRUE;
          k->httpversion = 11; /* RTSP acts like HTTP 1.1 */
        }
      }
      ಯದಿ(!fine_statusline)
        ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
    }

    ಯದಿ(fine_statusline) {
      result = http_statusline(data, data->conn);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      writetype |= CLIENTWRITE_STATUS;
    }
    ಅನ್ಯಥಾ {
      k->header = FALSE;   /* this is not a header line */
      ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
    }
  }

  result = Curl_verify_header(data, hd, hdlen);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  result = http_header(data, hd, hdlen);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /*
   * Taken in one (more) header. Write it to the client.
   */
  Curl_debug(data, CURLINFO_HEADER_IN, hd, hdlen);

  ಯದಿ(k->httpcode / 100 == 1)
    writetype |= CLIENTWRITE_1XX;
  result = Curl_client_write(data, writetype, hd, hdlen);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  result = Curl_bump_headersize(data, hdlen, FALSE);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* remove trailing CRLF then all trailing whitespace */
ಶೂನ್ಯ Curl_http_to_fold(ಸಂರಚನಾ dynbuf *bf)
{
  size_t len = curlx_dyn_len(bf);
  ಧ್ರುವ ವರ್ಣ *hd = curlx_dyn_ptr(bf);
  ಯದಿ(len && (hd[len - 1] == '\n'))
    len--;
  ಯದಿ(len && (hd[len - 1] == '\r'))
    len--;
  ಯಾವತ್(len && ISBLANK(hd[len - 1])) /* strip off trailing whitespace */
    len--;
  curlx_dyn_setlen(bf, len);
}

ಸ್ಥಿರ ಶೂನ್ಯ unfold_header(ಸಂರಚನಾ Curl_easy *data)
{
  Curl_http_to_fold(&data->state.headerb);
  data->state.leading_unfold = TRUE;
}

/*
 * Read any HTTP header lines from the server and pass them to the client app.
 */
ಸ್ಥಿರ CURLcode http_parse_headers(ಸಂರಚನಾ Curl_easy *data,
                                   ಧ್ರುವ ವರ್ಣ *buf, size_t blen,
                                   size_t *pconsumed)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ SingleRequest *k = &data->req;
  ಧ್ರುವ ವರ್ಣ *end_ptr;
  bool leftover_body = FALSE;

  /* we have bytes for the next header, make sure it is not a folded header
     before passing it on */
  ಯದಿ(data->state.maybe_folded && blen) {
    ಯದಿ(ISBLANK(buf[0])) {
      /* folded, remove the trailing newlines and append the next header */
      unfold_header(data);
    }
    ಅನ್ಯಥಾ {
      /* the header data we hold is a complete header, pass it on */
      size_t ignore_this;
      result = http_rw_hd(data, curlx_dyn_ptr(&data->state.headerb),
                          curlx_dyn_len(&data->state.headerb),
                          ರಿಕ್ತ, 0, &ignore_this);
      curlx_dyn_reset(&data->state.headerb);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    data->state.maybe_folded = FALSE;
  }

  /* header line within buffer loop */
  *pconsumed = 0;
  ಯಾವತ್(blen && k->header) {
    size_t consumed;
    size_t hlen;
    ಧ್ರುವ ವರ್ಣ *hd;
    size_t unfold_len = 0;

    ಯದಿ(data->state.leading_unfold) {
      /* immediately after an unfold, keep only a single whitespace */
      ಯಾವತ್(blen && ISBLANK(buf[0])) {
        buf++;
        blen--;
        unfold_len++;
      }
      ಯದಿ(blen) {
        /* insert a single space */
        result = curlx_dyn_addn(&data->state.headerb, " ", 1);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;
        data->state.leading_unfold = FALSE; /* done now */
      }
    }

    end_ptr = memchr(buf, '\n', blen);
    ಯದಿ(!end_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      result = curlx_dyn_addn(&data->state.headerb, buf, blen);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      *pconsumed += blen + unfold_len;

      ಯದಿ(!k->headerline) {
        /* check if this looks like a protocol header */
        statusline st =
          checkprotoprefix(data, conn,
                           curlx_dyn_ptr(&data->state.headerb),
                           curlx_dyn_len(&data->state.headerb));

        ಯದಿ(st == STATUS_BAD) {
          /* this is not the beginning of a protocol first header line.
           * Cannot be 0.9 if version was detected or connection was reused. */
          k->header = FALSE;
          streamclose(conn);
          ಯದಿ((k->httpversion >= 10) || conn->bits.reuse) {
            failf(data, "Invalid status line");
            ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
          }
          ಯದಿ(!data->state.http_neg.accept_09) {
            failf(data, "Received HTTP/0.9 when not allowed");
            ಪ್ರತ್ಯಾಗಮ CURLE_UNSUPPORTED_PROTOCOL;
          }
          leftover_body = TRUE;
          ಗಚ್ಛ out;
        }
      }
      ಗಚ್ಛ out; /* read more and try again */
    }

    /* the size of the remaining header line */
    consumed = (end_ptr - buf) + 1;

    result = curlx_dyn_addn(&data->state.headerb, buf, consumed);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    blen -= consumed;
    buf += consumed;
    *pconsumed += consumed + unfold_len;

    /****
     * We now have a FULL header line in 'headerb'.
     *****/

    hlen = curlx_dyn_len(&data->state.headerb);
    hd = curlx_dyn_ptr(&data->state.headerb);

    ಯದಿ(!k->headerline) {
      /* the first read "header", the status line */
      statusline st = checkprotoprefix(data, conn, hd, hlen);
      ಯದಿ(st == STATUS_BAD) {
        streamclose(conn);
        /* this is not the beginning of a protocol first header line.
         * Cannot be 0.9 if version was detected or connection was reused. */
        ಯದಿ((k->httpversion >= 10) || conn->bits.reuse) {
          failf(data, "Invalid status line");
          ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
        }
        ಯದಿ(!data->state.http_neg.accept_09) {
          failf(data, "Received HTTP/0.9 when not allowed");
          ಪ್ರತ್ಯಾಗಮ CURLE_UNSUPPORTED_PROTOCOL;
        }
        k->header = FALSE;
        leftover_body = TRUE;
        ಗಚ್ಛ out;
      }
    }
    ಅನ್ಯಥಾ {
      ಯದಿ(hlen && !ISNEWLINE(hd[0])) {
        /* this is NOT the header separator */

        /* if we have bytes for the next header, check for folding */
        ಯದಿ(blen && ISBLANK(buf[0])) {
          /* remove the trailing CRLF and append the next header */
          unfold_header(data);
          ಅನುವರ್ತ;
        }
        ಅನ್ಯಥಾ ಯದಿ(!blen) {
          /* this might be a folded header so deal with it in next invoke */
          data->state.maybe_folded = TRUE;
          ಭಙ್ಗ;
        }
      }
    }

    result = http_rw_hd(data, hd, hlen, buf, blen, &consumed);
    /* We are done with this line. We reset because response
     * processing might switch to HTTP/2 and that might call us
     * directly again. */
    curlx_dyn_reset(&data->state.headerb);
    ಯದಿ(consumed) {
      blen -= consumed;
      buf += consumed;
      *pconsumed += consumed;
    }
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */
out:
  ಯದಿ(!k->header && !leftover_body) {
    curlx_dyn_free(&data->state.headerb);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_http_write_resp_hd(ಸಂರಚನಾ Curl_easy *data,
                                 ಧ್ರುವ ವರ್ಣ *hd, size_t hdlen,
                                 bool is_eos)
{
  CURLcode result;
  size_t consumed;
  ವರ್ಣ tmp = 0;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  result = http_rw_hd(data, hd, hdlen, &tmp, 0, &consumed);
  ಯದಿ(!result && is_eos) {
    result = Curl_client_write(data, (CLIENTWRITE_BODY | CLIENTWRITE_EOS),
                               &tmp, 0);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * HTTP protocol `write_resp` implementation. Parse headers
 * when not done yet and otherwise return without consuming data.
 */
CURLcode Curl_http_write_resp_hds(ಸಂರಚನಾ Curl_easy *data,
                                  ಧ್ರುವ ವರ್ಣ *buf, size_t blen,
                                  size_t *pconsumed)
{
  ಯದಿ(!data->req.header) {
    *pconsumed = 0;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಅನ್ಯಥಾ {
    CURLcode result;

    result = http_parse_headers(data, buf, blen, pconsumed);
    ಯದಿ(!result && !data->req.header) {
      ಯದಿ(!data->req.no_body && curlx_dyn_len(&data->state.headerb)) {
        /* leftover from parsing something that turned out not
         * to be a header, only happens if we allow for
         * HTTP/0.9 like responses */
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   curlx_dyn_ptr(&data->state.headerb),
                                   curlx_dyn_len(&data->state.headerb));
      }
      curlx_dyn_free(&data->state.headerb);
    }
    ಪ್ರತ್ಯಾಗಮ result;
  }
}

CURLcode Curl_http_write_resp(ಸಂರಚನಾ Curl_easy *data,
                              ಧ್ರುವ ವರ್ಣ *buf, size_t blen,
                              bool is_eos)
{
  CURLcode result;
  size_t consumed;
  ಪೂರ್ಣಾಙ್ಕ flags;

  result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
  ಯದಿ(result || data->req.done)
    ಗಚ್ಛ out;

  DEBUGASSERT(consumed <= blen);
  blen -= consumed;
  buf += consumed;
  /* either all was consumed in header parsing, or we have data left
   * and are done with headers, e.g. it is BODY data */
  DEBUGASSERT(!blen || !data->req.header);
  ಯದಿ(!data->req.header && (blen || is_eos)) {
    /* BODY data after header been parsed, write and consume */
    flags = CLIENTWRITE_BODY;
    ಯದಿ(is_eos)
      flags |= CLIENTWRITE_EOS;
    result = Curl_client_write(data, flags, buf, blen);
  }
out:
  ಪ್ರತ್ಯಾಗಮ result;
}

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_status(int32_t *pstatus, ಧ್ರುವ ವರ್ಣ *s, size_t len)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  int32_t status = 0;
  ಪೂರ್ಣಾಙ್ಕ i;

  ಯದಿ(len != 3)
    ಗಚ್ಛ out;

  ಪ್ರತಿ(i = 0; i < 3; ++i) {
    ವರ್ಣ c = s[i];

    ಯದಿ(c < '0' || c > '9')
      ಗಚ್ಛ out;

    status *= 10;
    status += c - '0';
  }
  result = CURLE_OK;
out:
  *pstatus = result ? -1 : status;
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_http_req_make(ಸಂರಚನಾ httpreq **preq,
                            ಧ್ರುವ ವರ್ಣ *method, size_t m_len,
                            ಧ್ರುವ ವರ್ಣ *scheme, size_t s_len,
                            ಧ್ರುವ ವರ್ಣ *authority, size_t a_len,
                            ಧ್ರುವ ವರ್ಣ *path, size_t p_len)
{
  ಸಂರಚನಾ httpreq *req;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, ಪರಿಮಾಣ(*req) + m_len);
  ಯದಿ(!req)
    ಗಚ್ಛ out;
#ಯದಿ defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic ತಳ್ಳು
/* error: 'memcpy' offset [137, 142] from the object at 'req' is out of
   the bounds of referenced subobject 'method' with type 'char[1]' at
   offset 136 */
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
  memcpy(req->method, method, m_len);
#ಯದಿ defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic ತೆಗೆ
#endif
  ಯದಿ(scheme) {
    req->scheme = curlx_memdup0(scheme, s_len);
    ಯದಿ(!req->scheme)
      ಗಚ್ಛ out;
  }
  ಯದಿ(authority) {
    req->authority = curlx_memdup0(authority, a_len);
    ಯದಿ(!req->authority)
      ಗಚ್ಛ out;
  }
  ಯದಿ(path) {
    req->path = curlx_memdup0(path, p_len);
    ಯದಿ(!req->path)
      ಗಚ್ಛ out;
  }
  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  ಯದಿ(result && req)
    Curl_http_req_free(req);
  *preq = result ? ರಿಕ್ತ : req;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode req_assign_url_authority(ಸಂರಚನಾ httpreq *req, CURLU *url)
{
  ವರ್ಣ *host, *port;
  ಸಂರಚನಾ dynbuf buf;
  CURLUcode uc;
  CURLcode result = CURLE_URL_MALFORMAT;

  host = port = ರಿಕ್ತ;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_HOST, &host, 0);
  ಯದಿ(uc && uc != CURLUE_NO_HOST)
    ಗಚ್ಛ out;
  ಯದಿ(!host) {
    req->authority = ರಿಕ್ತ;
    result = CURLE_OK;
    ಗಚ್ಛ out;
  }

  uc = curl_url_get(url, CURLUPART_PORT, &port, CURLU_NO_DEFAULT_PORT);
  ಯದಿ(uc && uc != CURLUE_NO_PORT)
    ಗಚ್ಛ out;

  result = curlx_dyn_add(&buf, host);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  ಯದಿ(port) {
    result = curlx_dyn_addf(&buf, ":%s", port);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  req->authority = curlx_dyn_ptr(&buf);
out:
  curlx_free(host);
  curlx_free(port);
  ಯದಿ(result)
    curlx_dyn_free(&buf);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode req_assign_url_path(ಸಂರಚನಾ httpreq *req, CURLU *url)
{
  ವರ್ಣ *path, *query;
  ಸಂರಚನಾ dynbuf buf;
  CURLUcode uc;
  CURLcode result = CURLE_URL_MALFORMAT;

  path = query = ರಿಕ್ತ;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_PATH, &path, 0);
  ಯದಿ(uc)
    ಗಚ್ಛ out;
  uc = curl_url_get(url, CURLUPART_QUERY, &query, 0);
  ಯದಿ(uc && uc != CURLUE_NO_QUERY)
    ಗಚ್ಛ out;

  ಯದಿ(!query) {
    req->path = path;
    path = ರಿಕ್ತ;
  }
  ಅನ್ಯಥಾ {
    result = curlx_dyn_add(&buf, path);
    ಯದಿ(!result)
      result = curlx_dyn_addf(&buf, "?%s", query);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    req->path = curlx_dyn_ptr(&buf);
  }
  result = CURLE_OK;

out:
  curlx_free(path);
  curlx_free(query);
  ಯದಿ(result)
    curlx_dyn_free(&buf);
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_http_req_make2(ಸಂರಚನಾ httpreq **preq,
                             ಧ್ರುವ ವರ್ಣ *method, size_t m_len,
                             CURLU *url, ಧ್ರುವ ವರ್ಣ *scheme_default)
{
  ಸಂರಚನಾ httpreq *req;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  CURLUcode uc;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, ಪರಿಮಾಣ(*req) + m_len);
  ಯದಿ(!req)
    ಗಚ್ಛ out;
  memcpy(req->method, method, m_len);

  uc = curl_url_get(url, CURLUPART_SCHEME, &req->scheme, 0);
  ಯದಿ(uc && uc != CURLUE_NO_SCHEME)
    ಗಚ್ಛ out;
  ಯದಿ(!req->scheme && scheme_default) {
    req->scheme = curlx_strdup(scheme_default);
    ಯದಿ(!req->scheme)
      ಗಚ್ಛ out;
  }

  result = req_assign_url_authority(req, url);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  result = req_assign_url_path(req, url);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  ಯದಿ(result && req)
    Curl_http_req_free(req);
  *preq = result ? ರಿಕ್ತ : req;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ Curl_http_req_free(ಸಂರಚನಾ httpreq *req)
{
  ಯದಿ(req) {
    curlx_free(req->scheme);
    curlx_free(req->authority);
    curlx_free(req->path);
    Curl_dynhds_free(&req->headers);
    Curl_dynhds_free(&req->trailers);
    curlx_free(req);
  }
}

ಸಂರಚನಾ name_const {
  ಧ್ರುವ ವರ್ಣ *name;
  size_t namelen;
};

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ name_const H2_NON_FIELD[] = {
  { STRCONST("Host") },
  { STRCONST("Upgrade") },
  { STRCONST("Connection") },
  { STRCONST("Keep-Alive") },
  { STRCONST("Proxy-Connection") },
  { STRCONST("Transfer-Encoding") },
};

ಸ್ಥಿರ bool h2_permissible_field(ಸಂರಚನಾ dynhds_entry *e)
{
  size_t i;
  ಪ್ರತಿ(i = 0; i < CURL_ARRAYSIZE(H2_NON_FIELD); ++i) {
    ಯದಿ(e->namelen == H2_NON_FIELD[i].namelen &&
       curl_strnequal(H2_NON_FIELD[i].name, e->name, e->namelen))
      ಪ್ರತ್ಯಾಗಮ FALSE;
  }
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಸ್ಥಿರ bool http_TE_has_token(ಧ್ರುವ ವರ್ಣ *fvalue, ಧ್ರುವ ವರ್ಣ *token)
{
  ಯಾವತ್(*fvalue) {
    ಸಂರಚನಾ Curl_str name;

    /* skip to first token */
    ಯಾವತ್(ISBLANK(*fvalue) || *fvalue == ',')
      fvalue++;
    ಯದಿ(curlx_str_cspn(&fvalue, &name, " \t\r;,"))
      ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ(curlx_str_casecompare(&name, token))
      ಪ್ರತ್ಯಾಗಮ TRUE;

    /* skip any remainder after token, e.g. parameters with quoted strings */
    ಯಾವತ್(*fvalue && *fvalue != ',') {
      ಯದಿ(*fvalue == '"') {
        ಸಂರಚನಾ Curl_str qw;
        /* if we do not cleanly find a quoted word here, the header value
         * does not follow HTTP syntax and we reject */
        ಯದಿ(curlx_str_quotedword(&fvalue, &qw, CURL_MAX_HTTP_HEADER))
          ಪ್ರತ್ಯಾಗಮ FALSE;
      }
      ಅನ್ಯಥಾ
        fvalue++;
    }
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

CURLcode Curl_http_req_to_h2(ಸಂರಚನಾ dynhds *h2_headers,
                             ಸಂರಚನಾ httpreq *req, ಸಂರಚನಾ Curl_easy *data)
{
  ಧ್ರುವ ವರ್ಣ *scheme = ರಿಕ್ತ, *authority = ರಿಕ್ತ;
  ಸಂರಚನಾ dynhds_entry *e;
  size_t i;
  CURLcode result;

  DEBUGASSERT(req);
  DEBUGASSERT(h2_headers);

  ಯದಿ(req->scheme) {
    scheme = req->scheme;
  }
  ಅನ್ಯಥಾ ಯದಿ(strcmp("CONNECT", req->method)) {
    scheme = Curl_checkheaders(data, STRCONST(HTTP_PSEUDO_SCHEME));
    ಯದಿ(scheme) {
      scheme += ಪರಿಮಾಣ(HTTP_PSEUDO_SCHEME);
      curlx_str_passblanks(&scheme);
      infof(data, "set pseudo header %s to %s", HTTP_PSEUDO_SCHEME, scheme);
    }
    ಅನ್ಯಥಾ {
      scheme = data->state.origin->scheme->name;
    }
  }

  ಯದಿ(req->authority) {
    authority = req->authority;
  }
  ಅನ್ಯಥಾ {
    e = Curl_dynhds_get(&req->headers, STRCONST("Host"));
    ಯದಿ(e)
      authority = e->ಮೌಲ್ಯ;
  }

  Curl_dynhds_reset(h2_headers);
  Curl_dynhds_set_opts(h2_headers, DYNHDS_OPT_LOWERCASE);
  result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_METHOD),
                           req->method, strlen(req->method));
  ಯದಿ(!result && scheme) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_SCHEME),
                             scheme, strlen(scheme));
  }
  ಯದಿ(!result && authority) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_AUTHORITY),
                             authority, strlen(authority));
  }
  ಯದಿ(!result && req->path) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_PATH),
                             req->path, strlen(req->path));
  }
  ಪ್ರತಿ(i = 0; !result && i < Curl_dynhds_ಎಣಿಕೆ(&req->headers); ++i) {
    e = Curl_dynhds_getn(&req->headers, i);
    /* "TE" is special in that it is only permissible when it
     * has only value "trailers". RFC 9113 ch. 8.2.2 */
    ಯದಿ(e->namelen == 2 && curl_strequal("TE", e->name)) {
      ಯದಿ(http_TE_has_token(e->ಮೌಲ್ಯ, "trailers"))
        result = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                                 "trailers", CURL_CSTRLEN("trailers"));
    }
    ಅನ್ಯಥಾ ಯದಿ(h2_permissible_field(e)) {
      result = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                               e->ಮೌಲ್ಯ, e->valuelen);
    }
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_http_resp_make(ಸಂರಚನಾ http_resp **presp,
                             ಪೂರ್ಣಾಙ್ಕ status,
                             ಧ್ರುವ ವರ್ಣ *description)
{
  ಸಂರಚನಾ http_resp *resp;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  resp = curlx_calloc(1, ಪರಿಮಾಣ(*resp));
  ಯದಿ(!resp)
    ಗಚ್ಛ out;

  resp->status = status;
  ಯದಿ(description) {
    resp->description = curlx_strdup(description);
    ಯದಿ(!resp->description)
      ಗಚ್ಛ out;
  }
  Curl_dynhds_init(&resp->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&resp->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  ಯದಿ(result && resp)
    Curl_http_resp_free(resp);
  *presp = result ? ರಿಕ್ತ : resp;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ Curl_http_resp_free(ಸಂರಚನಾ http_resp *resp)
{
  ಯದಿ(resp) {
    curlx_free(resp->description);
    Curl_dynhds_free(&resp->headers);
    Curl_dynhds_free(&resp->trailers);
    ಯದಿ(resp->prev)
      Curl_http_resp_free(resp->prev);
    curlx_free(resp);
  }
}

/*
 * HTTP handler interface.
 */
ಧ್ರುವ ಸಂರಚನಾ Curl_protocol Curl_protocol_http = {
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
