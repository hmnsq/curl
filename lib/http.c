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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "urldata.h"

#ifndef CURL_DISABLE_HTTP

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif

#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#अन्तर्भाव <sys/param.h>
#endif

#अन्तर्भाव "transfer.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "formdata.h"
#अन्तर्भाव "mime.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "curlx/base64.h"
#अन्तर्भाव "cookie.h"
#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "vquic/vquic.h"
#अन्तर्भाव "http_digest.h"
#अन्तर्भाव "http_ntlm.h"
#अन्तर्भाव "http_negotiate.h"
#अन्तर्भाव "http_aws_sigv4.h"
#अन्तर्भाव "http_httpsig.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "urlapi-int.h"
#अन्तर्भाव "curl_share.h"
#अन्तर्भाव "dynhds.h"
#अन्तर्भाव "http.h"
#अन्तर्भाव "headers.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "parsedate.h" /* for the week day and month names */
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "content_encoding.h"
#अन्तर्भाव "http_proxy.h"
#अन्तर्भाव "http2.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "altsvc.h"
#अन्तर्भाव "hsts.h"
#अन्तर्भाव "rtsp.h"
#अन्तर्भाव "ws.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "curlx/strparse.h"

शून्य Curl_http_neg_init(संरचना Curl_easy *data, संरचना http_negotiation *neg)
{
  स_रखो(neg, 0, परिमाण(*neg));
  neg->accept_09 = data->set.http09_allowed;
  विकल्प(data->set.httpwant) {
  पक्ष CURL_HTTP_VERSION_1_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    neg->only_10 = TRUE;
    भङ्ग;
  पक्ष CURL_HTTP_VERSION_1_1:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    भङ्ग;
  पक्ष CURL_HTTP_VERSION_2_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->h2_upgrade = TRUE;
    भङ्ग;
  पक्ष CURL_HTTP_VERSION_2TLS:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    भङ्ग;
  पक्ष CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    neg->wanted = neg->allowed = (CURL_HTTP_V2x);
    data->state.http_neg.h2_prior_knowledge = TRUE;
    भङ्ग;
  पक्ष CURL_HTTP_VERSION_3:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    neg->allowed = neg->wanted;
    भङ्ग;
  पक्ष CURL_HTTP_VERSION_3ONLY:
    neg->wanted = neg->allowed = (CURL_HTTP_V3x);
    भङ्ग;
  पक्ष CURL_HTTP_VERSION_NONE:
  स्वभाव:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    भङ्ग;
  }
}

CURLcode Curl_http_setup_conn(संरचना Curl_easy *data,
                              संरचना connectdata *conn)
{
  /* allocate the HTTP-specific struct for the Curl_easy, only to survive
     during this request */
  यदि(data->state.http_neg.wanted == CURL_HTTP_V3x) {
    /* only HTTP/3, needs to work */
    CURLcode result = Curl_conn_may_http3(data, conn);
    यदि(result)
      प्रत्यागम result;
  }
  प्रत्यागम CURLE_OK;
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
वर्ण *Curl_checkProxyheaders(संरचना Curl_easy *data,
                             ध्रुव संरचना connectdata *conn,
                             ध्रुव वर्ण *thisheader,
                             ध्रुव माप_प्रकार thislen)
{
  संरचना curl_slist *head;

  प्रति(head = (conn->http_proxy.peer && data->set.sep_headers) ?
        data->set.proxyheaders : data->set.headers;
      head; head = head->next) {
    यदि(curl_strnequal(head->data, thisheader, thislen) &&
       Curl_headersep(head->data[thislen]))
      प्रत्यागम head->data;
  }

  प्रत्यागम रिक्त;
}
#endif

/* If the header has a value, this function returns TRUE and the value is in
   'outp' with blanks trimmed off. */
स्थिर bool header_has_मूल्य(ध्रुव वर्ण **headerp, संरचना Curl_str *outp)
{
  bool मूल्य = !curlx_str_cspn(headerp, outp, ";:") &&
    (!curlx_str_single(headerp, ':') || !curlx_str_single(headerp, ';'));

  यदि(मूल्य) {
    curlx_str_cspn(headerp, outp, "\r\n");
    curlx_str_trimblanks(outp);
  }
  प्रत्यागम मूल्य;
}

स्थिर bool http_header_is_empty(ध्रुव वर्ण *header)
{
  संरचना Curl_str out;

  यदि(header_has_मूल्य(&header, &out)) {
    प्रत्यागम curlx_strlen(&out) == 0;
  }
  प्रत्यागम TRUE; /* invalid header format, treat as empty */
}

/*
 * Strip off leading and trailing whitespace from the value in the given HTTP
 * header line and return a strdup-ed copy in 'valp' - returns an empty
 * string if the header value consists entirely of whitespace.
 *
 * If the header is provided as "name;", ending with a semicolon, it returns a
 * blank string.
 */
स्थिर CURLcode copy_custom_मूल्य(ध्रुव वर्ण *header, वर्ण **valp)
{
  संरचना Curl_str out = { 0 };

  /* find the end of the header name */
  यदि(header_has_मूल्य(&header, &out)) {
    *valp = curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
    यदि(*valp)
      प्रत्यागम CURLE_OK;
    प्रत्यागम CURLE_OUT_OF_MEMORY;
  }
  /* bad input */
  *valp = रिक्त;
  प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
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
वर्ण *Curl_copy_header_मूल्य(ध्रुव वर्ण *header)
{
  संरचना Curl_str out;

  /* find the end of the header name */
  यदि(!curlx_str_until(&header, &out, MAX_HTTP_RESP_HEADER_SIZE, ':') &&
     !curlx_str_single(&header, ':')) {
    curlx_str_untilnl(&header, &out, MAX_HTTP_RESP_HEADER_SIZE);
    curlx_str_trimblanks(&out);
    प्रत्यागम curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
  }
  /* bad input, should never happen */
  DEBUGASSERT(0);
  प्रत्यागम रिक्त;
}

#ifndef CURL_DISABLE_HTTP_AUTH

#ifndef CURL_DISABLE_BASIC_AUTH
/*
 * http_output_basic() sets up an Authorization: header (or the proxy version)
 * for HTTP Basic authentication.
 *
 * Returns CURLcode.
 */
स्थिर CURLcode http_output_basic(संरचना Curl_easy *data,
                                  संरचना connectdata *conn, bool proxy)
{
  माप_प्रकार size = 0;
  वर्ण *authorization = रिक्त;
  वर्ण **p_hd;
  CURLcode result;
  संरचना Curl_creds *creds = रिक्त;
  वर्ण *out;

  /* credentials are unique per transfer for HTTP, do not use the ones for the
     connection */
  यदि(proxy) {
#ifndef CURL_DISABLE_PROXY
    p_hd = &data->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
#अन्यथा
    (शून्य)conn;
    प्रत्यागम CURLE_NOT_BUILT_IN;
#endif
  }
  अन्यथा {
    p_hd = &data->req.hd_auth;
    creds = data->state.creds;
  }

  यदि(!creds) {
    DEBUGASSERT(0);
    प्रत्यागम CURLE_FAILED_INIT;
  }

  out = curl_maprintf("%s:%s", creds->user, creds->passwd);
  यदि(!out)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  result = curlx_base64_encode((uint8_t *)out, म_माप(out),
                               &authorization, &size);
  यदि(result)
    गच्छ fail;

  यदि(!authorization) {
    result = CURLE_REMOTE_ACCESS_DENIED;
    गच्छ fail;
  }

  curlx_free(*p_hd);
  *p_hd = curl_maprintf("%sAuthorization: Basic %s\r\n",
                        proxy ? "Proxy-" : "",
                        authorization);
  curlx_free(authorization);
  यदि(!*p_hd) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ fail;
  }

fail:
  curlx_free(out);
  प्रत्यागम result;
}

#endif

#ifndef CURL_DISABLE_BEARER_AUTH
/*
 * http_output_bearer() sets up an Authorization: header
 * for HTTP Bearer authentication.
 *
 * Returns CURLcode.
 */
स्थिर CURLcode http_output_bearer(संरचना Curl_easy *data)
{
  वर्ण **userp;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(Curl_creds_has_oauth_bearer(data->state.creds));
  userp = &data->req.hd_auth;
  curlx_free(*userp);
  *userp = curl_maprintf("Authorization: Bearer %s\r\n",
                         Curl_creds_oauth_bearer(data->state.creds));

  यदि(!*userp) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ fail;
  }

fail:
  प्रत्यागम result;
}
#endif

#endif

/* pickoneauth() selects the most favorable authentication method from the
 * ones available and the ones we want.
 *
 * return TRUE if one was picked
 */
स्थिर bool pickoneauth(संरचना auth *pick, अचिन्हित दीर्घ mask,
                        संरचना Curl_creds *creds)
{
  bool have_user_pass = Curl_creds_has_user_or_pass(creds);
  bool picked;
  /* only deal with authentication we want */
  अचिन्हित दीर्घ avail = pick->avail & pick->want & mask;
  picked = TRUE;

  /* The order of these checks is highly relevant, as this will be the order
     of preference in case of the existence of multiple accepted types. */
  यदि(avail & CURLAUTH_NEGOTIATE)  /* available on empty creds */
    pick->picked = CURLAUTH_NEGOTIATE;
#ifndef CURL_DISABLE_BEARER_AUTH
  अन्यथा यदि((avail & CURLAUTH_BEARER) && Curl_creds_has_oauth_bearer(creds))
    pick->picked = CURLAUTH_BEARER;
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  अन्यथा यदि((avail & CURLAUTH_DIGEST) && have_user_pass)
    pick->picked = CURLAUTH_DIGEST;
#endif
  अन्यथा यदि(avail & CURLAUTH_NTLM)
    pick->picked = CURLAUTH_NTLM;
#ifndef CURL_DISABLE_BASIC_AUTH
  अन्यथा यदि((avail & CURLAUTH_BASIC) && have_user_pass)
    pick->picked = CURLAUTH_BASIC;
#endif
#ifndef CURL_DISABLE_AWS
  अन्यथा यदि(avail & CURLAUTH_AWS_SIGV4)
    pick->picked = CURLAUTH_AWS_SIGV4;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  अन्यथा यदि(avail & CURLAUTH_HTTPSIG)
    pick->picked = CURLAUTH_HTTPSIG;
#endif
  अन्यथा {
    pick->picked = CURLAUTH_PICKNONE; /* we select to use nothing */
    picked = FALSE;
  }
  pick->avail = CURLAUTH_NONE; /* clear it here */

  प्रत्यागम picked;
}

/*
 * http_perhapsrewind()
 *
 * The current request needs to be done again - maybe due to a follow
 * or authentication negotiation. Check if:
 * 1) a rewind of the data sent to the server is necessary
 * 2) the current transfer should continue or be stopped early
 */
स्थिर CURLcode http_perhapsrewind(संरचना Curl_easy *data,
                                   संरचना connectdata *conn)
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
  VERBOSE(ध्रुव वर्ण *ongoing_auth = रिक्त);

  /* We need a rewind before uploading client read data again. The
   * checks below influence of the upload is to be continued
   * or aborted early.
   * This depends on how much remains to be sent and in what state
   * the authentication is. Some auth schemes such as NTLM do not work
   * for a new connection. */
  यदि(needs_rewind) {
    infof(data, "Need to rewind upload for next request");
    Curl_creader_set_rewind(data, TRUE);
  }

  यदि(conn->bits.बंद)
    /* If we already decided to close this connection, we cannot veto. */
    प्रत्यागम CURLE_OK;

  यदि(abort_upload) {
    /* We would like to abort the upload - but should we? */
#ifdef USE_NTLM
    यदि((data->state.authproxy.picked == CURLAUTH_NTLM) ||
       (data->state.authhost.picked == CURLAUTH_NTLM)) {
      VERBOSE(ongoing_auth = "NTLM");
      यदि((conn->http_ntlm_state != NTLMSTATE_NONE) ||
         (conn->proxy_ntlm_state != NTLMSTATE_NONE)) {
        /* The NTLM-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#endif
#ifdef USE_SPNEGO
    /* There is still data left to send */
    यदि((data->state.authproxy.picked == CURLAUTH_NEGOTIATE) ||
       (data->state.authhost.picked == CURLAUTH_NEGOTIATE)) {
      VERBOSE(ongoing_auth = "NEGOTIATE");
      यदि((conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) ||
         (conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE)) {
        /* The NEGOTIATE-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#endif
  }

  यदि(abort_upload) {
    यदि(upload_remain >= 0)
      infof(data, "%s%sclose instead of sending %" FMT_OFF_T " more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "",
            upload_remain);
    अन्यथा
      infof(data, "%s%sclose instead of sending unknown amount "
            "of more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "");
    /* We decided to abort the ongoing transfer */
    streamclose(conn);
    data->req.size = 0; /* do not download any more than 0 bytes */
    data->req.http_bodyless = TRUE;
  }
  प्रत्यागम CURLE_OK;
}

/**
 * http_should_fail() determines whether an HTTP response code has gotten us
 * into an error state or not.
 *
 * @retval FALSE communications should continue
 *
 * @retval TRUE communications should not continue
 */
स्थिर bool http_should_fail(संरचना Curl_easy *data, पूर्णाङ्क httpcode)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);

  /*
   * If we have not been asked to fail on error,
   * do not fail.
   */
  यदि(!data->set.http_fail_on_error)
    प्रत्यागम FALSE;

  /*
   * Any code < 400 is never terminal.
   */
  यदि(httpcode < 400)
    प्रत्यागम FALSE;

  /*
   * A 416 response to a resume request is presumably because the file is
   * already completely downloaded and thus not actually a fail.
   */
  यदि(data->state.resume_from && data->state.httpreq == HTTPREQ_GET &&
     httpcode == 416)
    प्रत्यागम FALSE;

  /*
   * Any code >= 400 that is not 401 or 407 is always
   * a terminal error
   */
  यदि((httpcode != 401) && (httpcode != 407))
    प्रत्यागम TRUE;

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
  यदि((httpcode == 401) && !data->state.creds)
    प्रत्यागम TRUE;
#ifndef CURL_DISABLE_PROXY
  यदि((httpcode == 407) && !data->conn->http_proxy.creds)
    प्रत्यागम TRUE;
#endif

  प्रत्यागम (bool)data->state.authproblem;
}

/*
 * Curl_http_auth_act() gets called when all HTTP headers have been received
 * and it checks what authentication methods that are available and decides
 * which one (if any) to use. It will set 'newurl' if an auth method was
 * picked.
 */
CURLcode Curl_http_auth_act(संरचना Curl_easy *data)
{
  संरचना connectdata *conn = data->conn;
  bool pickhost = FALSE;
  bool pickproxy = FALSE;
  CURLcode result = CURLE_OK;
  अचिन्हित दीर्घ authmask = ~0UL;

  यदि(!Curl_creds_has_oauth_bearer(data->state.creds))
    authmask &= (अचिन्हित दीर्घ)~CURLAUTH_BEARER;

  यदि(100 <= data->req.httpcode && data->req.httpcode <= 199)
    /* this is a transient response code, ignore */
    प्रत्यागम CURLE_OK;

  यदि(data->state.authproblem)
    प्रत्यागम data->set.http_fail_on_error ? CURLE_HTTP_RETURNED_ERROR : CURLE_OK;

  यदि(data->state.creds &&
     ((data->req.httpcode == 401) ||
      (data->req.authneg && data->req.httpcode < 300))) {
    pickhost = pickoneauth(&data->state.authhost, authmask, data->state.creds);
    यदि(!pickhost)
      data->state.authproblem = TRUE;
    अन्यथा
      data->info.httpauthpicked = data->state.authhost.picked;
    यदि(data->state.authhost.picked == CURLAUTH_NTLM &&
       (data->req.httpversion_sent > 11)) {
      infof(data, "Forcing HTTP/1.1 for NTLM");
      connclose(conn);
      data->state.http_neg.wanted = CURL_HTTP_V1x;
      data->state.http_neg.allowed = CURL_HTTP_V1x;
    }
  }
#ifndef CURL_DISABLE_PROXY
  यदि(conn->http_proxy.creds &&
     ((data->req.httpcode == 407) ||
      (data->req.authneg && data->req.httpcode < 300))) {
    pickproxy = pickoneauth(&data->state.authproxy,
                            authmask & ~CURLAUTH_BEARER,
                            conn->http_proxy.creds);
    यदि(!pickproxy)
      data->state.authproblem = TRUE;
    अन्यथा
      data->info.proxyauthpicked = data->state.authproxy.picked;
  }
#endif

  यदि(pickhost || pickproxy) {
    result = http_perhapsrewind(data, conn);
    यदि(result)
      प्रत्यागम result;

    /* In case this is GSS auth, the newurl field is already allocated so
       we must make sure to free it before allocating a new one. As figured
       out in bug #2284386 */
    curlx_free(data->req.newurl);
    /* clone URL */
    data->req.newurl = Curl_bufref_dup(&data->state.url);
    यदि(!data->req.newurl)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
  }
  अन्यथा यदि((data->req.httpcode < 300) &&
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
    यदि(!data->req.newurl)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    data->state.authhost.done = TRUE;
  }
  यदि(http_should_fail(data, data->req.httpcode)) {
    failf(data, "The requested URL returned error: %d",
          data->req.httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
  }

  प्रत्यागम result;
}

#ifndef CURL_DISABLE_HTTP_AUTH
/*
 * Output the correct authentication header depending on the auth type
 * and whether or not it is to a proxy.
 */
स्थिर CURLcode output_auth_headers(संरचना Curl_easy *data,
                                    संरचना connectdata *conn,
                                    संरचना auth *authstatus,
                                    ध्रुव वर्ण *request,
                                    ध्रुव वर्ण *path,
                                    bool proxy)
{
  ध्रुव वर्ण *auth = रिक्त;
  CURLcode result = CURLE_OK;
  (शून्य)conn;

#ifdef CURL_DISABLE_DIGEST_AUTH
  (शून्य)request;
  (शून्य)path;
#endif
#ifndef CURL_DISABLE_AWS
  यदि((authstatus->picked == CURLAUTH_AWS_SIGV4) && !proxy) {
    /* this method is never for proxy */
    auth = "AWS_SIGV4";
    result = Curl_output_aws_sigv4(data);
    यदि(result)
      प्रत्यागम result;
  }
  अन्यथा
#endif
#ifndef CURL_DISABLE_HTTPSIG
  यदि((authstatus->picked == CURLAUTH_HTTPSIG) && !proxy) {
    /* HTTPSIG uses its own configured key material rather than
       data->state.creds. Do not let unrelated credentials from a
       redirected URL bypass the cross-host auth boundary. */
    यदि(Curl_auth_allowed_to_host(data)) {
      auth = "HTTPSIG";
      result = Curl_output_httpsig(data);
      यदि(result)
        प्रत्यागम result;
    }
    अन्यथा
      authstatus->done = TRUE;
  }
  अन्यथा
#endif
#ifdef USE_SPNEGO
  यदि(authstatus->picked == CURLAUTH_NEGOTIATE) {
    यदि(
#ifndef CURL_DISABLE_PROXY
      (proxy && !Curl_checkProxyheaders(data, conn,
                                        STRCONST("Proxy-authorization"))) ||
#endif
      (!proxy && !Curl_checkheaders(data, STRCONST("Authorization")) &&
       Curl_auth_allowed_to_host(data))) {
      auth = "Negotiate";
      result = Curl_output_negotiate(data, conn, proxy);
      यदि(result)
        प्रत्यागम result;
    }
    अन्यथा
      authstatus->done = TRUE;
  }
  अन्यथा
#endif
#ifdef USE_NTLM
  यदि(authstatus->picked == CURLAUTH_NTLM) {
    यदि(proxy || Curl_auth_allowed_to_host(data)) {
      auth = "NTLM";
      result = Curl_output_ntlm(data, proxy);
      यदि(result)
        प्रत्यागम result;
    }
    अन्यथा
      authstatus->done = TRUE;
  }
  अन्यथा
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
  यदि(authstatus->picked == CURLAUTH_DIGEST) {
    auth = "Digest";
    result = Curl_output_digest(data,
                                proxy,
                                (ध्रुव अचिन्हित वर्ण *)request,
                                (ध्रुव अचिन्हित वर्ण *)path);
    यदि(result)
      प्रत्यागम result;
  }
  अन्यथा
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
  यदि(authstatus->picked == CURLAUTH_BASIC) {
    /* Basic */
    यदि(
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
      यदि(result)
        प्रत्यागम result;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
  यदि(authstatus->picked == CURLAUTH_BEARER) {
    /* Bearer */
    यदि(!proxy && Curl_creds_has_oauth_bearer(data->state.creds) &&
       !Curl_checkheaders(data, STRCONST("Authorization"))) {
      auth = "Bearer";
      result = http_output_bearer(data);
      यदि(result)
        प्रत्यागम result;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#endif

  यदि(auth) {
#ifndef CURL_DISABLE_PROXY
    यदि(proxy)
      data->info.proxyauthpicked = authstatus->picked;
    अन्यथा
      data->info.httpauthpicked = authstatus->picked;
    infof(data, "%s auth using %s with user '%s'",
          proxy ? "Proxy" : "Server", auth,
          proxy ? (conn->http_proxy.creds ?
                   conn->http_proxy.creds->user : "") :
          (data->state.creds ?
           data->state.creds->user : ""));
#अन्यथा
    (शून्य)proxy;
    infof(data, "Server auth using %s with user '%s'",
          auth, data->state.creds ?
          data->state.creds->user : "");
#endif
    authstatus->multipass = !authstatus->done;
  }
  अन्यथा {
    authstatus->multipass = FALSE;
    यदि(proxy)
      data->info.proxyauthpicked = 0;
    अन्यथा
      data->info.httpauthpicked = 0;
  }

  प्रत्यागम result;
}

CURLcode Curl_http_output_auth(संरचना Curl_easy *data,
                               संरचना connectdata *conn,
                               ध्रुव वर्ण *request,
                               Curl_HttpReq httpreq,
                               ध्रुव वर्ण *path,
                               ध्रुव वर्ण *query,
                               bool is_connect)
{
  CURLcode result = CURLE_OK;
  संरचना auth *authhost;
  संरचना auth *authproxy;
  ध्रुव वर्ण *path_and_query = path;
  वर्ण *tmp_str = रिक्त;

  DEBUGASSERT(data);
  authhost = &data->state.authhost;
  authproxy = &data->state.authproxy;

  यदि(
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
    गच्छ out;
  }

  यदि(query) {
    tmp_str = curl_maprintf("%s?%s", path, query);
    यदि(!tmp_str) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
    path_and_query = tmp_str;
  }

  यदि(authhost->want && !authhost->picked)
    /* The app has selected one or more methods, but none has been picked
       so far by a server round-trip. Then we set the picked one to the
       want one, and if this is one single bit it will be used instantly. */
    authhost->picked = authhost->want;

  यदि(authproxy->want && !authproxy->picked)
    /* The app has selected one or more methods, but none has been picked so
       far by a proxy round-trip. Then we set the picked one to the want one,
       and if this is one single bit it will be used instantly. */
    authproxy->picked = authproxy->want;

#ifndef CURL_DISABLE_PROXY
  /* Send proxy authentication header if needed */
  यदि(conn->bits.origin_is_proxy || is_connect) {
    result = output_auth_headers(data, conn, authproxy, request,
                                 path_and_query, TRUE);
    यदि(result)
      गच्छ out;
  }
  अन्यथा
#अन्यथा
  (शून्य)is_connect;
#endif /* CURL_DISABLE_PROXY */
    /* we have no proxy so let's pretend we are done authenticating
       with it */
    authproxy->done = TRUE;

  /* Either we have credentials for the origin we talk to or
     performing authentication is allowed here */
  यदि(data->state.creds || Curl_auth_allowed_to_host(data))
    result = output_auth_headers(data, conn, authhost, request,
                                 path_and_query, FALSE);
  अन्यथा
    authhost->done = TRUE;

  यदि(((authhost->multipass && !authhost->done) ||
      (authproxy->multipass && !authproxy->done)) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_HEAD)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    data->req.authneg = TRUE;
  }
  अन्यथा
    data->req.authneg = FALSE;

out:
  curlx_free(tmp_str);
  प्रत्यागम result;
}

#अन्यथा /* !CURL_DISABLE_HTTP_AUTH */
/* when disabled */
CURLcode Curl_http_output_auth(संरचना Curl_easy *data,
                               संरचना connectdata *conn,
                               ध्रुव वर्ण *request,
                               Curl_HttpReq httpreq,
                               ध्रुव वर्ण *path,
                               ध्रुव वर्ण *query,
                               bool is_connect)
{
  (शून्य)data;
  (शून्य)conn;
  (शून्य)request;
  (शून्य)httpreq;
  (शून्य)path;
  (शून्य)query;
  (शून्य)is_connect;
  प्रत्यागम CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP_AUTH, else */

#यदि defined(USE_SPNEGO) || defined(USE_NTLM) || \
  !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_BASIC_AUTH) || \
  !defined(CURL_DISABLE_BEARER_AUTH)
स्थिर bool authcmp(ध्रुव वर्ण *auth, ध्रुव वर्ण *पंक्ति)
{
  /* the auth string must not have an alnum following */
  माप_प्रकार n = म_माप(auth);
  प्रत्यागम curl_strnequal(auth, पंक्ति, n) && !ISALNUM(पंक्ति[n]);
}
#endif

#ifdef USE_SPNEGO
स्थिर CURLcode auth_spnego(संरचना Curl_easy *data,
                            bool proxy,
                            ध्रुव वर्ण *auth,
                            संरचना auth *authp,
                            uint32_t *availp)
{
  यदि((authp->avail & CURLAUTH_NEGOTIATE) || Curl_auth_is_spnego_supported()) {
    *availp |= CURLAUTH_NEGOTIATE;
    authp->avail |= CURLAUTH_NEGOTIATE;

    यदि(authp->picked == CURLAUTH_NEGOTIATE) {
      संरचना connectdata *conn = data->conn;
      CURLcode result = Curl_निवेश_negotiate(data, conn, proxy, auth);
      uint8_t *negstate = proxy ? &conn->proxy_negotiate_state :
        &conn->http_negotiate_state;
      यदि(!result) {
        curlx_free(data->req.newurl);
        data->req.newurl = Curl_bufref_dup(&data->state.url);
        यदि(!data->req.newurl)
          प्रत्यागम CURLE_OUT_OF_MEMORY;
        data->state.authproblem = FALSE;
        /* we received a GSS auth token and we dealt with it fine */
        *negstate = CURL_NEGO_AUTH_RECV;
      }
      अन्यथा
        data->state.authproblem = TRUE;
    }
  }
  प्रत्यागम CURLE_OK;
}
#endif

#ifdef USE_NTLM
स्थिर CURLcode auth_ntlm(संरचना Curl_easy *data,
                          bool proxy,
                          ध्रुव वर्ण *auth,
                          संरचना auth *authp,
                          uint32_t *availp)
{
  /* NTLM support requires the SSL crypto libs */
  यदि((authp->avail & CURLAUTH_NTLM) || Curl_auth_is_ntlm_supported()) {
    *availp |= CURLAUTH_NTLM;
    authp->avail |= CURLAUTH_NTLM;

    यदि(authp->picked == CURLAUTH_NTLM) {
      /* NTLM authentication is picked and activated */
      CURLcode result = Curl_निवेश_ntlm(data, proxy, auth);
      यदि(!result)
        data->state.authproblem = FALSE;
      अन्यथा {
        यदि(result == CURLE_OUT_OF_MEMORY)
          प्रत्यागम result;
        infof(data, "NTLM authentication problem, ignoring.");
        data->state.authproblem = TRUE;
      }
    }
  }
  प्रत्यागम CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_DIGEST_AUTH
स्थिर CURLcode auth_digest(संरचना Curl_easy *data,
                            bool proxy,
                            ध्रुव वर्ण *auth,
                            संरचना auth *authp,
                            uint32_t *availp)
{
  यदि(authp->avail & CURLAUTH_DIGEST) {
    *availp |= CURLAUTH_DIGEST;
    infof(data, "Ignoring duplicate digest auth header.");
  }
  अन्यथा यदि(Curl_auth_is_digest_supported()) {
    CURLcode result;

    *availp |= CURLAUTH_DIGEST;
    authp->avail |= CURLAUTH_DIGEST;

    /* We call this function on input Digest headers even if Digest
     * authentication is not activated yet, as we need to store the
     * incoming data from this header in case we are going to use
     * Digest */
    result = Curl_निवेश_digest(data, proxy, auth);
    यदि(result) {
      यदि(result == CURLE_OUT_OF_MEMORY)
        प्रत्यागम result;
      infof(data, "Digest authentication problem, ignoring.");
      data->state.authproblem = TRUE;
    }
  }
  प्रत्यागम CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BASIC_AUTH
स्थिर CURLcode auth_basic(संरचना Curl_easy *data,
                           संरचना auth *authp,
                           uint32_t *availp)
{
  *availp |= CURLAUTH_BASIC;
  authp->avail |= CURLAUTH_BASIC;
  यदि(authp->picked == CURLAUTH_BASIC) {
    /* We asked for Basic authentication but got a 40X back anyway, which
       means our name+password is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(data, "Basic authentication problem, ignoring.");
    data->state.authproblem = TRUE;
  }
  प्रत्यागम CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_BEARER_AUTH
स्थिर CURLcode auth_bearer(संरचना Curl_easy *data,
                            संरचना auth *authp,
                            uint32_t *availp)
{
  *availp |= CURLAUTH_BEARER;
  authp->avail |= CURLAUTH_BEARER;
  यदि(authp->picked == CURLAUTH_BEARER) {
    /* We asked for Bearer authentication but got a 40X back anyway, which
       means our token is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(data, "Bearer authentication problem, ignoring.");
    data->state.authproblem = TRUE;
  }
  प्रत्यागम CURLE_OK;
}
#endif

/*
 * Curl_http_input_auth() deals with Proxy-Authenticate: and WWW-Authenticate:
 * headers. They are dealt with both in the transfer.c main loop and in the
 * proxy CONNECT loop.
 *
 * The 'auth' line ends with a NUL byte without CR or LF present.
 */
CURLcode Curl_http_निवेश_auth(संरचना Curl_easy *data, bool proxy,
                              ध्रुव वर्ण *auth) /* the first non-space */
{
  /*
   * This resource requires authentication
   */
#यदि defined(USE_SPNEGO) ||                      \
  defined(USE_NTLM) ||                          \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||         \
  !defined(CURL_DISABLE_BASIC_AUTH) ||          \
  !defined(CURL_DISABLE_BEARER_AUTH)

  uint32_t *availp;
  संरचना auth *authp;
  CURLcode result = CURLE_OK;
  DEBUGASSERT(auth);
  DEBUGASSERT(data);

  यदि(proxy) {
    availp = &data->info.proxyauthavail;
    authp = &data->state.authproxy;
  }
  अन्यथा {
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

  यावत्(*auth) {
#ifdef USE_SPNEGO
    यदि(authcmp("Negotiate", auth))
      result = auth_spnego(data, proxy, auth, authp, availp);
#endif
#ifdef USE_NTLM
    यदि(!result && authcmp("NTLM", auth))
      result = auth_ntlm(data, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_DIGEST_AUTH
    यदि(!result && authcmp("Digest", auth))
      result = auth_digest(data, proxy, auth, authp, availp);
#endif
#ifndef CURL_DISABLE_BASIC_AUTH
    यदि(!result && authcmp("Basic", auth))
      result = auth_basic(data, authp, availp);
#endif
#ifndef CURL_DISABLE_BEARER_AUTH
    यदि(authcmp("Bearer", auth))
      result = auth_bearer(data, authp, availp);
#endif

    यदि(result)
      भङ्ग;

    /* there may be multiple methods on one line, so keep reading */
    auth = म_अक्षर(auth, ',');
    यदि(auth) /* if we are on a comma, skip it */
      auth++;
    अन्यथा
      भङ्ग;
    curlx_str_passblanks(&auth);
  }
  प्रत्यागम result;
#अन्यथा
  (शून्य)data;
  (शून्य)proxy;
  (शून्य)auth;
  /* nothing to do when disabled */
  प्रत्यागम CURLE_OK;
#endif
}

स्थिर शून्य http_switch_to_get(संरचना Curl_easy *data, पूर्णाङ्क code)
{
  ध्रुव वर्ण *req = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  यदि((req || data->state.httpreq != HTTPREQ_GET) &&
     (data->set.http_follow_mode == CURLFOLLOW_OBEYCODE)) {
    NOVERBOSE((शून्य)code);
    infof(data, "Switch to GET because of %d response", code);
    data->state.http_ignorecustom = TRUE;
  }
  अन्यथा यदि(req && (data->set.http_follow_mode != CURLFOLLOW_FIRSTONLY))
    infof(data, "Stick to %s instead of GET", req);

  data->state.httpreq = HTTPREQ_GET;
  Curl_creader_set_rewind(data, FALSE);
}

#define HTTPREQ_IS_POST(data)                    \
  ((data)->state.httpreq == HTTPREQ_POST ||      \
   (data)->state.httpreq == HTTPREQ_POST_FORM || \
   (data)->state.httpreq == HTTPREQ_POST_MIME)

स्थिर CURLcode set_auto_referer(संरचना Curl_easy *data)
{
  CURLU *u;
  वर्ण *referer = रिक्त;
  CURLUcode uc;

  /* We are asked to automatically set the previous URL as the referer
     when we get the next URL. We pick the ->url field, which may or may
     not be 100% correct */
  Curl_bufref_free(&data->state.referer);

  /* Make a copy of the URL without credentials and fragment */
  u = curl_url();
  यदि(!u)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  uc = curl_url_set(u, CURLUPART_URL,
                    Curl_bufref_ptr(&data->state.url), 0);
  यदि(!uc)
    uc = curl_url_set(u, CURLUPART_FRAGMENT, रिक्त, 0);
  यदि(!uc)
    uc = curl_url_set(u, CURLUPART_USER, रिक्त, 0);
  यदि(!uc)
    uc = curl_url_set(u, CURLUPART_PASSWORD, रिक्त, 0);
  यदि(!uc)
    uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

  curl_url_cleanup(u);

  यदि(uc || !referer)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  Curl_bufref_set(&data->state.referer, referer, 0, curl_free);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode follow_parse_url(संरचना Curl_easy *data, ध्रुव वर्ण *newurl,
                                 followtype type, वर्ण **follow_urlp,
                                 CURLUcode *ucp)
{
  CURLUcode uc;

  DEBUGASSERT(data->state.uh);
  uc = curl_url_set(data->state.uh, CURLUPART_URL, newurl, (अचिन्हित पूर्णाङ्क)
                    ((type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                     ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) |
                     CURLU_ALLOW_SPACE |
                     (data->set.path_as_is ? CURLU_PATH_AS_IS : 0)));
  *ucp = uc;
  यदि(uc) {
    यदि((uc == CURLUE_OUT_OF_MEMORY) || (type != FOLLOW_FAKE)) {
      failf(data, "The redirect target URL could not be parsed: %s",
            curl_url_strerror(uc));
      प्रत्यागम Curl_uc_to_curlcode(uc);
    }

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, duplicate the field as-is */
    *follow_urlp = curlx_strdup(newurl);
    यदि(!*follow_urlp)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
  }
  अन्यथा {
    CURLU *u = curl_url();
    यदि(!u)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    uc = curl_url_set(u, CURLUPART_URL,
                      Curl_bufref_ptr(&data->state.url),
                      CURLU_URLENCODE | CURLU_ALLOW_SPACE);
    यदि(!uc)
      uc = curl_url_get(data->state.uh, CURLUPART_URL, follow_urlp, 0);
    यदि(uc) {
      curl_url_cleanup(u);
      प्रत्यागम Curl_uc_to_curlcode(uc);
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    {
      bool same_origin = Curl_url_same_origin(u, data->state.uh);
      curl_url_cleanup(u);
      यदि(!same_origin)
        Curl_auth_digest_cleanup(&data->state.digest);
    }
#अन्यथा
    curl_url_cleanup(u);
#endif
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode follow_fake(संरचना Curl_easy *data, वर्ण *follow_url,
                            CURLUcode uc, bool reachedmax)
{
  /* we are only figuring out the new URL if we would have followed locations
     but now we are done so we can get out! */
  यदि(!uc) {
    CURLU *u = curl_url();
    वर्ण *nocred = रिक्त;

    यदि(!u) {
      curlx_free(follow_url);
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    }
    यदि(!curl_url_set(u, CURLUPART_URL, follow_url,
                     CURLU_NON_SUPPORT_SCHEME |
                     (data->set.path_as_is ? CURLU_PATH_AS_IS : 0)) &&
       !curl_url_set(u, CURLUPART_USER, रिक्त, 0) &&
       !curl_url_set(u, CURLUPART_PASSWORD, रिक्त, 0))
      (शून्य)curl_url_get(u, CURLUPART_URL, &nocred, CURLU_GET_EMPTY);
    curl_url_cleanup(u);
    यदि(nocred) {
      curlx_free(follow_url);
      follow_url = nocred;
    }
  }
  data->info.wouldredirect = follow_url;

  यदि(reachedmax) {
    failf(data, "Maximum (%d) redirects followed", data->set.maxredirs);
    प्रत्यागम CURLE_TOO_MANY_REDIRECTS;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर bool follow_switch_get(संरचना Curl_easy *data)
{
  /*
   * We get here when the HTTP code is 300-399 and 401 or 407. We need to
   * perform differently based on exactly what return code there was.
   */
  विकल्प(data->info.httpcode) {
  पक्ष 301: /* Moved Permanently */
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
    यदि(HTTPREQ_IS_POST(data) && !data->set.post301) {
      http_switch_to_get(data, 301);
      प्रत्यागम TRUE;
    }
    भङ्ग;
  पक्ष 302: /* Found */
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
    यदि(HTTPREQ_IS_POST(data) && !data->set.post302) {
      http_switch_to_get(data, 302);
      प्रत्यागम TRUE;
    }
    भङ्ग;
  पक्ष 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     */
    यदि(!HTTPREQ_IS_POST(data) || !data->set.post303) {
      http_switch_to_get(data, 303);
      प्रत्यागम TRUE;
    }
    भङ्ग;

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
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम FALSE;
}

CURLcode Curl_http_follow(संरचना Curl_easy *data, ध्रुव वर्ण *newurl,
                          followtype type)
{
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  वर्ण *follow_url = रिक्त;
  CURLUcode uc;
  CURLcode result;
  CURLcode rewind_result;
  bool switch_to_get;

  DEBUGASSERT(type != FOLLOW_NONE);

  यदि(type != FOLLOW_FAKE)
    data->state.requests++; /* count all real follows */
  यदि(type == FOLLOW_REDIR) {
    यदि((data->set.maxredirs != -1) &&
       (data->state.followlocation >= data->set.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    अन्यथा {
      data->state.followlocation++; /* count redirect-followings, including
                                       auth reloads */

      यदि(data->set.http_auto_referer) {
        result = set_auto_referer(data);
        यदि(result)
          प्रत्यागम result;
      }
    }
  }

  यदि((type != FOLLOW_RETRY) &&
     (data->req.httpcode != 401) && (data->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, रिक्त, 0, FALSE)) {
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: do not allow a custom port number */
    disallowport = TRUE;
  }

  result = follow_parse_url(data, newurl, type, &follow_url, &uc);
  यदि(result)
    प्रत्यागम result;

  DEBUGASSERT(follow_url);

  यदि(type == FOLLOW_FAKE)
    प्रत्यागम follow_fake(data, follow_url, uc, reachedmax);

  यदि(disallowport)
    data->state.allow_port = FALSE;

  Curl_bufref_set(&data->state.url, follow_url, 0, curl_free);
  rewind_result = Curl_req_soft_reset(&data->req, data);
  infof(data, "Issue another request to this URL: '%s'", follow_url);
  यदि((data->set.http_follow_mode == CURLFOLLOW_FIRSTONLY) &&
     !data->state.http_ignorecustom &&
     CURL_EASY_STR(data, STRING_CUSTOMREQUEST)) {
    data->state.http_ignorecustom = TRUE;
    infof(data, "Drop custom request method for next request");
  }

  switch_to_get = follow_switch_get(data);

  /* When rewind of upload data failed and we are not switching to GET,
   * we need to fail the follow, as we cannot send the data again. */
  यदि(rewind_result && !switch_to_get)
    प्रत्यागम rewind_result;

  Curl_pgrsTime(data, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(data);

  प्रत्यागम CURLE_OK;
}

/*
 * Curl_compareheader()
 *
 * Returns TRUE if 'headerline' contains the 'header' with given 'content'
 * (within a comma-separated list of tokens). Pass 'header' WITH the colon.
 *
 * @unittest: 1625
 */
bool Curl_compareheader(ध्रुव वर्ण *headerline, /* line to check */
                        ध्रुव वर्ण *header, /* header keyword _with_ colon */
                        ध्रुव माप_प्रकार hlen, /* len of the keyword in bytes */
                        ध्रुव वर्ण *content, /* content string to find */
                        ध्रुव माप_प्रकार clen) /* len of the content in bytes */
{
  /* RFC2616, section 4.2 says: "Each header field consists of a name followed
   * by a colon (":") and the field value. Field names are case-insensitive.
   * The field value MAY be preceded by any amount of LWS, though a single SP
   * is preferred." */

  ध्रुव वर्ण *p;
  संरचना Curl_str val;
  DEBUGASSERT(hlen);
  DEBUGASSERT(clen);
  DEBUGASSERT(header);
  DEBUGASSERT(content);

  यदि(!curl_strnequal(headerline, header, hlen))
    प्रत्यागम FALSE; /* does not start with header */

  /* pass the header */
  p = &headerline[hlen];

  यदि(curlx_str_cspn(&p, &val, "\r\n"))
    प्रत्यागम FALSE;
  curlx_str_trimblanks(&val);

  /* find the content string in the rest of the line */
  यदि(curlx_strlen(&val) >= clen) {
    माप_प्रकार len;
    p = curlx_str(&val);
    प्रति(len = curlx_strlen(&val); len >= clen;) {
      संरचना Curl_str next;
      ध्रुव वर्ण *o = p;
      /* after a match there must be a comma, space, newline or NUL byte */
      यदि(curl_strnequal(p, content, clen) &&
         ((p[clen] == ',') || ISBLANK(p[clen]) || ISNEWLINE(p[clen]) ||
          !p[clen]))
        प्रत्यागम TRUE; /* match! */
      /* advance to the next comma */
      यदि(curlx_str_until(&p, &next, len, ',') ||
         curlx_str_single(&p, ','))
        भङ्ग; /* no comma, get out */

      /* if there are more dummy commas, move over them as well */
      कुरु
        curlx_str_passblanks(&p);
      यावत्(!curlx_str_single(&p, ','));
      /* trailing blanks may move the parsing point past the value end,
         then there is nothing left to match */
      यदि((माप_प्रकार)(p - o) > len)
        भङ्ग;
      len -= (p - o);
    }
  }
  प्रत्यागम FALSE; /* no match */
}

संरचना cr_exp100_ctx {
  संरचना Curl_creader super;
  संरचना curltime start; /* time started waiting */
  गणना expect100 state;
};

/* Expect: 100-continue client reader, blocking uploads */

स्थिर शून्य http_exp100_continue(संरचना Curl_easy *data,
                                 संरचना Curl_creader *reader)
{
  संरचना cr_exp100_ctx *ctx = reader->ctx;
  यदि(ctx->state > EXP100_SEND_DATA) {
    ctx->state = EXP100_SEND_DATA;
    Curl_expire_clear(data, EXPIRE_100_TIMEOUT);
  }
}

स्थिर CURLcode cr_exp100_read(संरचना Curl_easy *data,
                               संरचना Curl_creader *reader,
                               वर्ण *buf, माप_प्रकार blen,
                               माप_प्रकार *nread, bool *eos)
{
  संरचना cr_exp100_ctx *ctx = reader->ctx;
  timediff_t ms;

  विकल्प(ctx->state) {
  पक्ष EXP100_SENDING_REQUEST:
    यदि(!Curl_req_sendbuf_empty(data)) {
      /* The initial request data has not been fully sent yet. Do
       * not start the timer yet. */
      DEBUGF(infof(data, "cr_exp100_read, request not full sent yet"));
      *nread = 0;
      *eos = FALSE;
      प्रत्यागम CURLE_OK;
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
    प्रत्यागम CURLE_OK;
  पक्ष EXP100_FAILED:
    DEBUGF(infof(data, "cr_exp100_read, expectation failed, error"));
    *nread = 0;
    *eos = FALSE;
    प्रत्यागम CURLE_READ_ERROR;
  पक्ष EXP100_AWAITING_CONTINUE:
    ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->start);
    यदि(ms < data->set.expect_100_timeout) {
      DEBUGF(infof(data, "cr_exp100_read, AWAITING_CONTINUE, not expired"));
      *nread = 0;
      *eos = FALSE;
      प्रत्यागम CURLE_OK;
    }
    /* we have waited long enough, continue anyway */
    http_exp100_continue(data, reader);
    infof(data, "Done waiting for 100-continue");
    FALLTHROUGH();
  स्वभाव:
    DEBUGF(infof(data, "cr_exp100_read, pass through"));
    प्रत्यागम Curl_creader_read(data, reader->next, buf, blen, nread, eos);
  }
}

स्थिर शून्य cr_exp100_done(संरचना Curl_easy *data,
                           संरचना Curl_creader *reader, पूर्णाङ्क premature)
{
  संरचना cr_exp100_ctx *ctx = reader->ctx;
  ctx->state = premature ? EXP100_FAILED : EXP100_SEND_DATA;
  Curl_expire_clear(data, EXPIRE_100_TIMEOUT);
}

स्थिर ध्रुव संरचना Curl_crtype cr_exp100 = {
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
  परिमाण(संरचना cr_exp100_ctx)
};

स्थिर CURLcode http_exp100_add_reader(संरचना Curl_easy *data)
{
  संरचना Curl_creader *reader = रिक्त;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_exp100, CURL_CR_PROTOCOL);
  यदि(!result)
    result = Curl_creader_add(data, reader);
  यदि(!result) {
    संरचना cr_exp100_ctx *ctx = reader->ctx;
    ctx->state = EXP100_SENDING_REQUEST;
  }

  यदि(result && reader)
    Curl_creader_free(data, reader);
  प्रत्यागम result;
}

स्थिर शून्य http_exp100_got100(संरचना Curl_easy *data)
{
  संरचना Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  यदि(r)
    http_exp100_continue(data, r);
}

स्थिर bool http_exp100_is_waiting(संरचना Curl_easy *data)
{
  संरचना Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  यदि(r) {
    संरचना cr_exp100_ctx *ctx = r->ctx;
    प्रत्यागम ctx->state == EXP100_AWAITING_CONTINUE;
  }
  प्रत्यागम FALSE;
}

स्थिर शून्य http_exp100_send_anyway(संरचना Curl_easy *data)
{
  संरचना Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  यदि(r)
    http_exp100_continue(data, r);
}

स्थिर bool http_exp100_is_selected(संरचना Curl_easy *data)
{
  संरचना Curl_creader *r = Curl_creader_get_by_type(data, &cr_exp100);
  प्रत्यागम !!r;
}

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
CURLcode Curl_http_doing_pollset(संरचना Curl_easy *data,
                                 संरचना easy_pollset *ps)
{
  /* write mode */
  प्रत्यागम Curl_pollset_add_out(data, ps, data->conn->sock[FIRSTSOCKET]);
}

CURLcode Curl_http_perform_pollset(संरचना Curl_easy *data,
                                   संरचना easy_pollset *ps)
{
  संरचना connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  यदि(CURL_REQ_WANT_RECV(data)) {
    result = Curl_pollset_add_in(data, ps, conn->sock[FIRSTSOCKET]);
  }

  /* on a "Expect: 100-continue" timed wait, do not poll for outgoing */
  यदि(!result && Curl_req_want_send(data) && !http_exp100_is_waiting(data)) {
    result = Curl_pollset_add_out(data, ps, conn->sock[FIRSTSOCKET]);
  }
  प्रत्यागम result;
}

स्थिर CURLcode http_write_header(संरचना Curl_easy *data,
                                  ध्रुव वर्ण *hd, माप_प्रकार hdlen)
{
  CURLcode result;
  पूर्णाङ्क writetype;

  /* now, only output this if the header AND body are requested:
   */
  Curl_debug(data, CURLINFO_HEADER_IN, hd, hdlen);

  writetype = CLIENTWRITE_HEADER |
    ((data->req.httpcode / 100 == 1) ? CLIENTWRITE_1XX : 0);

  result = Curl_client_write(data, writetype, hd, hdlen);
  यदि(result)
    प्रत्यागम result;

  result = Curl_bump_headersize(data, hdlen, FALSE);
  यदि(result)
    प्रत्यागम result;

  data->req.deductheadercount = (100 <= data->req.httpcode &&
                                 199 >= data->req.httpcode) ?
    data->req.headerbytecount : 0;
  प्रत्यागम result;
}

/*
 * Curl_http_done() gets called after a single HTTP request has been
 * performed.
 */

CURLcode Curl_http_done(संरचना Curl_easy *data,
                        CURLcode status, bool premature)
{
  संरचना connectdata *conn = data->conn;

  /* Clear multipass flag. If authentication is not done yet, then it will get
   * a chance to be set back to true when we output the next auth header */
  data->state.authhost.multipass = FALSE;
  data->state.authproxy.multipass = FALSE;

  यदि(curlx_dyn_len(&data->state.headerb)) {
    (शून्य)http_write_header(data, curlx_dyn_ptr(&data->state.headerb),
                            curlx_dyn_len(&data->state.headerb));
  }
  curlx_dyn_reset(&data->state.headerb);

  यदि(status)
    प्रत्यागम status;

  यदि(!premature && /* this check is pointless when DONE is called before the
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
    प्रत्यागम CURLE_GOT_NOTHING;
  }

  प्रत्यागम CURLE_OK;
}

/* Determine if we may use HTTP 1.1 for this request. */
स्थिर bool http_may_use_1_1(ध्रुव संरचना Curl_easy *data)
{
  ध्रुव संरचना connectdata *conn = data->conn;
  /* We have seen a previous response for *this* transfer with 1.0,
   * on another connection or the same one. */
  यदि(data->state.http_neg.rcvd_min == 10)
    प्रत्यागम FALSE;
  /* We have seen a previous response on *this* connection with 1.0. */
  यदि(conn && conn->httpversion_seen == 10)
    प्रत्यागम FALSE;
  /* We want 1.0 and have seen no previous response on *this* connection
     with a higher version (maybe no response at all yet). */
  यदि(data->state.http_neg.only_10 &&
     (!conn || conn->httpversion_seen <= 10))
    प्रत्यागम FALSE;
  /* We are not restricted to use 1.0 only. */
  प्रत्यागम !data->state.http_neg.only_10;
}

स्थिर अचिन्हित वर्ण http_request_version(संरचना Curl_easy *data)
{
  अचिन्हित वर्ण v = Curl_conn_http_version(data, data->conn);
  यदि(!v) {
    /* No specific HTTP connection filter installed. */
    v = http_may_use_1_1(data) ? 11 : 10;
  }
  प्रत्यागम v;
}

स्थिर ध्रुव वर्ण *get_http_string(पूर्णाङ्क httpversion)
{
  विकल्प(httpversion) {
  पक्ष 30:
    प्रत्यागम "3";
  पक्ष 20:
    प्रत्यागम "2";
  पक्ष 11:
    प्रत्यागम "1.1";
  स्वभाव:
    प्रत्यागम "1.0";
  }
}

CURLcode Curl_add_custom_headers(संरचना Curl_easy *data,
                                 bool is_connect, पूर्णाङ्क httpversion,
                                 संरचना dynbuf *req)
{
  संरचना curl_slist *h[2];
  संरचना curl_slist *headers;
  पूर्णाङ्क numlists = 1; /* by default */
  पूर्णाङ्क i;

#ifndef CURL_DISABLE_PROXY
  गणना Curl_proxy_use proxy;

  यदि(is_connect)
    proxy = HEADER_CONNECT;
  अन्यथा
    proxy = data->conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  विकल्प(proxy) {
  पक्ष HEADER_SERVER:
    h[0] = data->set.headers;
    भङ्ग;
  पक्ष HEADER_PROXY:
    h[0] = data->set.headers;
    यदि(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    भङ्ग;
  पक्ष HEADER_CONNECT:
    यदि(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    अन्यथा
      h[0] = data->set.headers;
    भङ्ग;
  पक्ष HEADER_CONNECT_UDP:
    यदि(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    अन्यथा
      h[0] = data->set.headers;
    भङ्ग;
  }
#अन्यथा
  (शून्य)is_connect;
  h[0] = data->set.headers;
#endif

  /* loop through one or two lists */
  प्रति(i = 0; i < numlists; i++) {
    प्रति(headers = h[i]; headers; headers = headers->next) {
      CURLcode result = CURLE_OK;
      bool blankheader = FALSE;
      संरचना Curl_str name;
      ध्रुव वर्ण *p = headers->data;
      ध्रुव वर्ण *origp = p;
      माप_प्रकार hlen = म_माप(origp);

      /* explicitly asked to send header without content is done by a header
         that ends with a semicolon, but there must be no colon present in the
         name */
      यदि(!curlx_str_until(&p, &name, hlen, ';') &&
         !curlx_str_single(&p, ';') &&
         !curlx_str_single(&p, '\0') &&
         !स_प्रथम(curlx_str(&name), ':', curlx_strlen(&name)))
        blankheader = TRUE;
      अन्यथा {
        p = origp;
        यदि(!curlx_str_until(&p, &name, hlen, ':') &&
           !curlx_str_single(&p, ':')) {
          संरचना Curl_str val;
          curlx_str_untilnl(&p, &val, hlen);
          curlx_str_trimblanks(&val);
          यदि(!curlx_strlen(&val))
            /* no content, do not send this */
            अनुवर्त;
        }
        अन्यथा
          /* no colon */
          अनुवर्त;
      }

      /* a field name is a token and carries no surrounding whitespace, so
         trim the parsed name before matching. Otherwise `Authorization :`
         (space before the colon) slips past the Authorization/Cookie check
         below and gets forwarded to another host on a redirect. */
      curlx_str_trimblanks(&name);

      /* only send this if the contents was non-blank or done special */

      यदि(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom
            Host: header as that will produce *two* in the same
            request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      अन्यथा यदि(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      अन्यथा यदि(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      अन्यथा यदि(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      अन्यथा यदि(curlx_str_casecompare(&name, "Connection"))
        /* Connection headers are handled specially */
        ;
      अन्यथा यदि((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        /* HTTP/2 does not support chunked requests */
        ;
      अन्यथा यदि((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      अन्यथा यदि(blankheader) {
        result = curlx_dyn_addn(req, curlx_str(&name), curlx_strlen(&name));
        यदि(!result)
          result = curlx_dyn_addn(req, STRCONST(":\r\n"));
      }
      अन्यथा
        result = curlx_dyn_addf(req, "%s\r\n", origp);

      यदि(result)
        प्रत्यागम result;
    }
  }

  प्रत्यागम CURLE_OK;
}

#ifndef CURL_DISABLE_PARSEDATE
CURLcode Curl_add_timecondition(संरचना Curl_easy *data,
                                संरचना dynbuf *req)
{
  ध्रुव संरचना पंचांग *पंचांग;
  संरचना पंचांग keeptime;
  CURLcode result;
  वर्ण datestr[80];
  ध्रुव वर्ण *condp;
  माप_प्रकार len;

  यदि(data->set.timecondition == CURL_TIMECOND_NONE)
    /* no condition was asked for */
    प्रत्यागम CURLE_OK;

  result = curlx_gmtime(data->set.timevalue, &keeptime);
  यदि(result) {
    failf(data, "Invalid TIMEVALUE");
    प्रत्यागम result;
  }
  पंचांग = &keeptime;

  विकल्प(data->set.timecondition) {
  स्वभाव:
    DEBUGF(infof(data, "invalid time condition"));
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

  पक्ष CURL_TIMECOND_IFMODSINCE:
    condp = "If-Modified-Since";
    len = 17;
    भङ्ग;
  पक्ष CURL_TIMECOND_IFUNMODSINCE:
    condp = "If-Unmodified-Since";
    len = 19;
    भङ्ग;
  पक्ष CURL_TIMECOND_LASTMOD:
    condp = "Last-Modified";
    len = 13;
    भङ्ग;
  }

  यदि(Curl_checkheaders(data, condp, len)) {
    /* A custom header was specified; it will be sent instead. */
    प्रत्यागम CURLE_OK;
  }

  /* The If-Modified-Since header family should have their times set in
   * GMT as RFC2616 defines: "All HTTP date/time stamps MUST be
   * represented in Greenwich Mean Time (GMT), without exception. For the
   * purposes of HTTP, GMT is exactly equal to UTC (Coordinated Universal
   * Time)." (see page 20 of RFC2616).
   */

  /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
  curl_msnprintf(datestr, परिमाण(datestr),
                 "%s: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                 condp,
                 Curl_wkday[पंचांग->tm_wday ? पंचांग->tm_wday - 1 : 6],
                 पंचांग->tm_mday,
                 Curl_month[पंचांग->tm_mon],
                 पंचांग->tm_year + 1900,
                 पंचांग->tm_hour,
                 पंचांग->tm_min,
                 पंचांग->tm_sec);

  result = curlx_dyn_add(req, datestr);
  प्रत्यागम result;
}
#अन्यथा
/* disabled */
CURLcode Curl_add_timecondition(संरचना Curl_easy *data,
                                संरचना dynbuf *req)
{
  (शून्य)data;
  (शून्य)req;
  प्रत्यागम CURLE_OK;
}
#endif

शून्य Curl_http_method(संरचना Curl_easy *data,
                      ध्रुव वर्ण **method, Curl_HttpReq *reqp)
{
  Curl_HttpReq httpreq = (Curl_HttpReq)data->state.httpreq;
  ध्रुव वर्ण *request;
#ifndef CURL_DISABLE_WEBSOCKETS
  यदि(data->conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
    httpreq = HTTPREQ_GET;
  अन्यथा
#endif
  यदि((data->conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_FTP)) &&
     data->state.upload)
    httpreq = HTTPREQ_PUT;

  /* Now set the 'request' pointer to the proper request string */
  यदि(!data->state.http_ignorecustom &&
     CURL_EASY_STR(data, STRING_CUSTOMREQUEST)) {
    request = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);
  }
  अन्यथा {
    यदि(data->req.no_body)
      request = "HEAD";
    अन्यथा {
      DEBUGASSERT((httpreq >= HTTPREQ_GET) && (httpreq <= HTTPREQ_HEAD));
      विकल्प(httpreq) {
      पक्ष HTTPREQ_POST:
      पक्ष HTTPREQ_POST_FORM:
      पक्ष HTTPREQ_POST_MIME:
        request = "POST";
        भङ्ग;
      पक्ष HTTPREQ_PUT:
        request = "PUT";
        भङ्ग;
      स्वभाव: /* this should never happen */
      पक्ष HTTPREQ_GET:
        request = "GET";
        भङ्ग;
      पक्ष HTTPREQ_HEAD:
        request = "HEAD";
        भङ्ग;
      }
    }
  }
  *method = request;
  *reqp = httpreq;
}

स्थिर CURLcode http_set_aptr_host(संरचना Curl_easy *data)
{
  संरचना connectdata *conn = data->conn;
  ध्रुव वर्ण *ptr = रिक्त;

  curlx_safefree(data->state.http_host);
#ifndef CURL_DISABLE_COOKIES
  curlx_safefree(data->req.cookiehost);
#endif

  यदि(Curl_peer_equal(data->state.initial_origin, data->state.origin))
    ptr = Curl_checkheaders(data, STRCONST("Host"));

  यदि(ptr) {
#ifndef CURL_DISABLE_COOKIES
    /* If we have a given custom Host: header, we extract the hostname in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the hostname
       is the same as the first one! */
    वर्ण *cookiehost;
    CURLcode result = copy_custom_मूल्य(ptr, &cookiehost);
    यदि(result)
      प्रत्यागम result;
    यदि(!*cookiehost)
      /* ignore empty data */
      curlx_free(cookiehost);
    अन्यथा {
      /* If the host begins with '[', we start searching for the port after
         the bracket has been closed */
      यदि(*cookiehost == '[') {
        वर्ण *closingbracket;
        /* since the 'cookiehost' is an allocated memory area that will be
           freed later we cannot increment the pointer */
        स_हटाओ(cookiehost, cookiehost + 1, म_माप(cookiehost) - 1);
        closingbracket = म_अक्षर(cookiehost, ']');
        यदि(closingbracket)
          *closingbracket = 0;
      }
      अन्यथा {
        पूर्णाङ्क startsearch = 0;
        वर्ण *colon = म_अक्षर(cookiehost + startsearch, ':');
        यदि(colon)
          *colon = 0; /* The host must not include an embedded port number */
      }
      data->req.cookiehost = cookiehost;
    }
#endif

    यदि(!curl_strequal("Host:", ptr)) {
      data->state.http_host = curl_maprintf("Host:%s", &ptr[5]);
      यदि(!data->state.http_host)
        प्रत्यागम CURLE_OUT_OF_MEMORY;
    }
  }
  अन्यथा {
    /* This is the  HTTP Host: header, so we want
     * - for IPv6 origins: "[ipv6-address]" where the IPv6 address is
     *  found in origin->hostname, stripped of zoneid/scopeid.
     * - the (IDN converted) origin->hostname (DNS name or IPv4) otherwise.
     * Note: zoneid/scopeid  only applies to local routing and has no
     * meaning on the remote HTTP server (eg. would confuse it). */
    bool ipv6 = (bool)data->state.origin->ipv6;
    संरचना dynbuf tmp;
    माप_प्रकार hlen;
    CURLcode result;

    curlx_dyn_init(&tmp, DYN_HTTP_REQUEST);
    result = curlx_dyn_addn(&tmp, STRCONST("Host: "));
    यदि(!result && ipv6)
      result = curlx_dyn_addn(&tmp, STRCONST("["));
    यदि(!result)
      result = curlx_dyn_add(&tmp, data->state.origin->hostname);
    यदि(!result && ipv6)
      result = curlx_dyn_addn(&tmp, STRCONST("]"));
    यदि(!result &&
       ((data->state.origin->port != data->state.origin->scheme->defport) ||
       (data->state.origin->scheme->family != conn->scheme->family))) {
      result = curlx_dyn_addf(&tmp, ":%u", data->state.origin->port);
    }

    data->state.http_host = result ? रिक्त : curlx_dyn_take(&tmp, &hlen);
    curlx_dyn_free(&tmp);
    प्रत्यागम result;
  }
  प्रत्यागम CURLE_OK;
}

/*
 * Append the request-target to the HTTP request
 */
स्थिर CURLcode http_target(संरचना Curl_easy *data,
                            संरचना dynbuf *r)
{
  CURLcode result = CURLE_OK;
  ध्रुव वर्ण *path = data->state.up.path;
  ध्रुव वर्ण *query = data->state.up.query;
#ifndef CURL_DISABLE_PROXY
  संरचना connectdata *conn = data->conn;
#endif

  यदि(CURL_EASY_STR(data, STRING_TARGET)) {
    path = CURL_EASY_STR(data, STRING_TARGET);
    query = रिक्त;
  }

#ifndef CURL_DISABLE_PROXY
  यदि(conn->bits.origin_is_proxy) {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL, but if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the decoded hostname! */

    /* and no fragment part */
    CURLUcode uc;
    वर्ण *url;
    CURLU *h = curl_url_dup(data->state.uh);
    यदि(!h)
      प्रत्यागम CURLE_OUT_OF_MEMORY;

    यदि(!data->state.origin->ipv6 &&
       (data->state.origin->user_hostname != data->state.origin->hostname)) {
      uc = curl_url_set(h, CURLUPART_HOST, data->state.origin->hostname, 0);
      यदि(uc) {
        curl_url_cleanup(h);
        प्रत्यागम CURLE_OUT_OF_MEMORY;
      }
    }
    uc = curl_url_set(h, CURLUPART_FRAGMENT, रिक्त, 0);
    यदि(uc) {
      curl_url_cleanup(h);
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    }

    यदि(data->state.origin->scheme == &Curl_scheme_http) {
      /* when getting HTTP, we do not want the userinfo the URL */
      uc = curl_url_set(h, CURLUPART_USER, रिक्त, 0);
      यदि(uc) {
        curl_url_cleanup(h);
        प्रत्यागम CURLE_OUT_OF_MEMORY;
      }
      uc = curl_url_set(h, CURLUPART_PASSWORD, रिक्त, 0);
      यदि(uc) {
        curl_url_cleanup(h);
        प्रत्यागम CURLE_OUT_OF_MEMORY;
      }
    }
    अन्यथा यदि(data->state.creds && (data->state.creds->source != CREDS_URL)) {
        /* credentials not from the URL need to be set */
      uc = curl_url_set(h, CURLUPART_USER,
                        data->state.creds->user, CURLU_URLENCODE);
      यदि(!uc)
        uc = curl_url_set(h, CURLUPART_PASSWORD,
                          data->state.creds->passwd, CURLU_URLENCODE);
      यदि(uc) {
        curl_url_cleanup(h);
        प्रत्यागम Curl_uc_to_curlcode(uc);
      }
    }

    /* Extract the URL to use in the request. */
    uc = curl_url_get(h, CURLUPART_URL, &url, CURLU_NO_DEFAULT_PORT);
    यदि(uc) {
      curl_url_cleanup(h);
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    }

    curl_url_cleanup(h);

    /* target or URL */
    result = curlx_dyn_add(r, CURL_EASY_STR(data, STRING_TARGET) ?
      CURL_EASY_STR(data, STRING_TARGET) : url);
    curlx_free(url);
    यदि(result)
      प्रत्यागम result;

    यदि((data->state.origin->scheme == &Curl_scheme_ftp) &&
       data->set.proxy_transfer_mode) {
      /* when doing ftp, append ;type=<a|i> if not present */
      माप_प्रकार len = म_माप(path);
      bool type_present = FALSE;
      यदि((len >= 7) && !स_भेद(&path[len - 7], ";type=", 6)) {
        विकल्प(Curl_raw_toupper(path[len - 1])) {
        पक्ष 'A':
        पक्ष 'D':
        पक्ष 'I':
          type_present = TRUE;
          भङ्ग;
        }
      }
      यदि(!type_present) {
        result = curlx_dyn_addf(r, ";type=%c",
                                data->state.prefer_ascii ? 'a' : 'i');
        यदि(result)
          प्रत्यागम result;
      }
    }
  }

  अन्यथा
#endif
  {
    result = curlx_dyn_add(r, path);
    यदि(result)
      प्रत्यागम result;
    यदि(query)
      result = curlx_dyn_addf(r, "?%s", query);
  }

  प्रत्यागम result;
}

#यदि !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
स्थिर CURLcode set_post_reader(संरचना Curl_easy *data, Curl_HttpReq httpreq)
{
  CURLcode result;

  विकल्प(httpreq) {
#ifndef CURL_DISABLE_MIME
  पक्ष HTTPREQ_POST_MIME:
    data->state.mimepost = data->set.mimepostp;
    भङ्ग;
#endif
#ifndef CURL_DISABLE_FORM_API
  पक्ष HTTPREQ_POST_FORM:
    /* Convert the form structure into a mime structure, then keep
       the conversion */
    यदि(!data->state.formp) {
      data->state.formp = curlx_calloc(1, परिमाण(curl_mimepart));
      यदि(!data->state.formp)
        प्रत्यागम CURLE_OUT_OF_MEMORY;
      Curl_mime_cleanpart(data->state.formp);
      result = Curl_getformdata(data, data->state.formp, data->set.httppost,
                                data->state.fread_func);
      यदि(result) {
        curlx_safefree(data->state.formp);
        प्रत्यागम result;
      }
      data->state.mimepost = data->state.formp;
    }
    भङ्ग;
#endif
  स्वभाव:
    data->state.mimepost = रिक्त;
    भङ्ग;
  }

  विकल्प(httpreq) {
  पक्ष HTTPREQ_POST_FORM:
  पक्ष HTTPREQ_POST_MIME:
    /* This is form posting using mime data. */
#ifndef CURL_DISABLE_MIME
    यदि(data->state.mimepost) {
      ध्रुव वर्ण *cthdr = Curl_checkheaders(data, STRCONST("Content-Type"));

      /* Read and seek body only. */
      data->state.mimepost->flags |= MIME_BODY_ONLY;

      /* Prepare the mime structure headers & set content type. */

      यदि(cthdr)
        प्रति(cthdr += 13; *cthdr == ' '; cthdr++)
          ;
      अन्यथा यदि(data->state.mimepost->kind == MIMEKIND_MULTIPART)
        cthdr = "multipart/form-data";

      curl_mime_headers(data->state.mimepost, data->set.headers, 0);
      result = Curl_mime_prepare_headers(data, data->state.mimepost, cthdr,
                                         रिक्त, MIMESTRATEGY_FORM);
      यदि(result)
        प्रत्यागम result;
      curl_mime_headers(data->state.mimepost, रिक्त, 0);
      result = Curl_creader_set_mime(data, data->state.mimepost);
      यदि(result)
        प्रत्यागम result;
    }
    अन्यथा
#endif
    {
      result = Curl_creader_set_null(data);
    }
    data->state.infilesize = Curl_creader_total_length(data);
    प्रत्यागम result;

  स्वभाव:
    प्रत्यागम Curl_creader_set_null(data);
  }
  /* never reached */
}
#endif

स्थिर CURLcode set_reader(संरचना Curl_easy *data, Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
  curl_off_t postsize = data->state.infilesize;

  DEBUGASSERT(data->conn);

  यदि(data->req.authneg) {
    प्रत्यागम Curl_creader_set_null(data);
  }

  विकल्प(httpreq) {
  पक्ष HTTPREQ_PUT: /* Let's PUT the data to the server! */
    प्रत्यागम postsize ? Curl_creader_set_fread(data, postsize) :
      Curl_creader_set_null(data);

#यदि !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  पक्ष HTTPREQ_POST_FORM:
  पक्ष HTTPREQ_POST_MIME:
    प्रत्यागम set_post_reader(data, httpreq);
#endif

  पक्ष HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */
    /* the size of the post body */
    यदि(!postsize) {
      result = Curl_creader_set_null(data);
    }
    अन्यथा यदि(data->set.postfields) {
      माप_प्रकार plen = curlx_sotouz_range(postsize, 0, SIZE_MAX);
      यदि(plen == SIZE_MAX)
        प्रत्यागम CURLE_OUT_OF_MEMORY;
      अन्यथा यदि(plen)
        result = Curl_creader_set_buf(data, data->set.postfields, plen);
      अन्यथा
        result = Curl_creader_set_null(data);
    }
    अन्यथा {
      /* we read the bytes from the callback. In case "chunked" encoding
       * is forced by the application, we disregard `postsize`. This is
       * a backward compatibility decision to earlier versions where
       * chunking disregarded this. See issue #13229. */
      bool chunked = FALSE;
      वर्ण *ptr = Curl_checkheaders(data, STRCONST("Transfer-Encoding"));
      यदि(ptr) {
        /* Some kind of TE is requested, check if 'chunked' is chosen */
        chunked = Curl_compareheader(ptr, STRCONST("Transfer-Encoding:"),
                                     STRCONST("chunked"));
      }
      result = Curl_creader_set_fread(data, chunked ? -1 : postsize);
    }
    प्रत्यागम result;

  स्वभाव:
    /* HTTP GET/HEAD download, has no body, needs no Content-Length */
    data->state.infilesize = 0;
    प्रत्यागम Curl_creader_set_null(data);
  }
  /* not reached */
}

स्थिर CURLcode http_resume(संरचना Curl_easy *data, Curl_HttpReq httpreq)
{
  यदि((HTTPREQ_POST == httpreq || HTTPREQ_POST_FORM == httpreq ||
      HTTPREQ_POST_MIME == httpreq || HTTPREQ_PUT == httpreq) &&
     data->state.resume_from) {
    failf(data, "HTTP upload cannot be resumed");
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode http_req_set_TE(संरचना Curl_easy *data,
                                संरचना dynbuf *req,
                                पूर्णाङ्क httpversion)
{
  CURLcode result = CURLE_OK;
  ध्रुव वर्ण *ptr;

  ptr = Curl_checkheaders(data, STRCONST("Transfer-Encoding"));
  यदि(ptr) {
    /* Some kind of TE is requested, check if 'chunked' is chosen */
    data->req.upload_chunky =
      Curl_compareheader(ptr,
                         STRCONST("Transfer-Encoding:"), STRCONST("chunked"));
    यदि(data->req.upload_chunky && (httpversion >= 20)) {
      infof(data, "suppressing chunked transfer encoding on connection "
            "using HTTP version 2 or higher");
      data->req.upload_chunky = FALSE;
    }
  }
  अन्यथा {
    curl_off_t req_clen = Curl_creader_total_length(data);

    यदि(req_clen < 0) {
      /* indeterminate request content length */
      यदि(httpversion > 10) {
        /* On HTTP/1.1, enable chunked, on HTTP/2 and later we do not
         * need it */
        data->req.upload_chunky = (httpversion < 20);
      }
      अन्यथा {
        failf(data, "Chunky upload is not supported by HTTP 1.0");
        प्रत्यागम CURLE_UPLOAD_FAILED;
      }
    }
    अन्यथा {
      /* else, no chunky upload */
      data->req.upload_chunky = FALSE;
    }

    यदि(data->req.upload_chunky)
      result = curlx_dyn_add(req, "Transfer-Encoding: chunked\r\n");
  }
  प्रत्यागम result;
}

स्थिर CURLcode addexpect(संरचना Curl_easy *data, संरचना dynbuf *r,
                          पूर्णाङ्क httpversion, bool *announced_exp100)
{
  CURLcode result;
  वर्ण *ptr;

  *announced_exp100 = FALSE;
  /* Avoid Expect: 100-continue if Upgrade: is used */
  यदि(data->req.upgr101 != UPGR101_NONE)
    प्रत्यागम CURLE_OK;

  /* For really small puts we do not use Expect: headers at all, and for
     the somewhat bigger ones we allow the app to disable it. Make
     sure that the expect100header is always set to the preferred value
     here. */
  ptr = Curl_checkheaders(data, STRCONST("Expect"));
  यदि(ptr) {
    *announced_exp100 =
      Curl_compareheader(ptr, STRCONST("Expect:"), STRCONST("100-continue"));
  }
  अन्यथा यदि(!data->state.disableexpect && (httpversion == 11)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    curl_off_t client_len = Curl_creader_client_length(data);
    यदि(client_len > EXPECT_100_THRESHOLD || client_len < 0) {
      result = curlx_dyn_addn(r, STRCONST("Expect: 100-continue\r\n"));
      यदि(result)
        प्रत्यागम result;
      *announced_exp100 = TRUE;
    }
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode http_add_content_hds(संरचना Curl_easy *data,
                                     संरचना dynbuf *r,
                                     पूर्णाङ्क httpversion,
                                     Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
  curl_off_t req_clen;
  bool announced_exp100 = FALSE;

  DEBUGASSERT(data->conn);
  यदि(data->req.upload_chunky) {
    result = Curl_httpchunk_add_reader(data);
    यदि(result)
      प्रत्यागम result;
  }

  /* Get the request body length that has been set up */
  req_clen = Curl_creader_total_length(data);
  विकल्प(httpreq) {
  पक्ष HTTPREQ_PUT:
  पक्ष HTTPREQ_POST:
#यदि !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  पक्ष HTTPREQ_POST_FORM:
  पक्ष HTTPREQ_POST_MIME:
#endif
    /* We only set Content-Length and allow a custom Content-Length if
       we do not upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length).
       We do not override a custom "Content-Length" header, but during
       authentication negotiation that header is suppressed.
     */
    यदि(req_clen >= 0 && !data->req.upload_chunky &&
       (data->req.authneg ||
        !Curl_checkheaders(data, STRCONST("Content-Length")))) {
      /* we allow replacing this header if not during auth negotiation,
         although it is not wise to actually set your own */
      result = curlx_dyn_addf(r, "Content-Length: %" FMT_OFF_T "\r\n",
                              req_clen);
    }
    यदि(result)
      गच्छ out;

#ifndef CURL_DISABLE_MIME
    /* Output mime-generated headers. */
    यदि(data->state.mimepost &&
       ((httpreq == HTTPREQ_POST_FORM) || (httpreq == HTTPREQ_POST_MIME))) {
      संरचना curl_slist *hdr;

      प्रति(hdr = data->state.mimepost->curlheaders; hdr; hdr = hdr->next) {
        result = curlx_dyn_addf(r, "%s\r\n", hdr->data);
        यदि(result)
          गच्छ out;
      }
    }
#endif
    यदि(httpreq == HTTPREQ_POST &&
       !Curl_checkheaders(data, STRCONST("Content-Type"))) {
      result = curlx_dyn_addn(r, STRCONST("Content-Type: application/"
                                          "x-www-form-urlencoded\r\n"));
      यदि(result)
        गच्छ out;
    }
    result = addexpect(data, r, httpversion, &announced_exp100);
    यदि(result)
      गच्छ out;
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }

  Curl_pgrsSetUploadSize(data, req_clen);
  यदि(announced_exp100)
    result = http_exp100_add_reader(data);

out:
  प्रत्यागम result;
}

#ifndef CURL_DISABLE_COOKIES

स्थिर CURLcode http_cookies(संरचना Curl_easy *data,
                             संरचना dynbuf *r)
{
  CURLcode result = CURLE_OK;
  ध्रुव वर्ण *addcookies = रिक्त;
  bool linecap = FALSE;
  यदि(CURL_EASY_STR(data, STRING_COOKIE) &&
     !Curl_checkheaders(data, STRCONST("Cookie")) &&
     Curl_auth_allowed_to_host(data))
    addcookies = CURL_EASY_STR(data, STRING_COOKIE);

  यदि(data->cookies || addcookies) {
    संरचना Curl_llist list;
    पूर्णाङ्क गिनती = 0;

    यदि(data->cookies && data->state.cookie_engine) {
      bool okay;
      ध्रुव वर्ण *host = data->req.cookiehost ?
        data->req.cookiehost : data->state.origin->hostname;
      Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
      result = Curl_cookie_getlist(data, &okay, host, &list);
      यदि(!result && okay) {
        संरचना Curl_llist_गाँठ *n;
        माप_प्रकार clen = 8; /* hold the size of the generated Cookie: header */

        /* loop through all cookies that matched */
        प्रति(n = Curl_llist_head(&list); n; n = Curl_गाँठ_next(n)) {
          संरचना Cookie *co = Curl_गाँठ_elem(n);
          यदि(co->मूल्य) {
            माप_प्रकार add;
            यदि(!गिनती) {
              result = curlx_dyn_addn(r, STRCONST("Cookie: "));
              यदि(result)
                भङ्ग;
            }
            add = म_माप(co->name) + म_माप(co->मूल्य) + 1;
            यदि(clen + add >= MAX_COOKIE_HEADER_LEN) {
              infof(data, "Restricted outgoing cookies due to header size, "
                    "'%s' not sent", co->name);
              linecap = TRUE;
              भङ्ग;
            }
            result = curlx_dyn_addf(r, "%s%s=%s", गिनती ? "; " : "",
                                    co->name, co->मूल्य);
            यदि(result)
              भङ्ग;
            clen += add + (गिनती ? 2 : 0);
            गिनती++;
          }
        }
        Curl_llist_destroy(&list, रिक्त);
      }
      Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    }
    यदि(addcookies && !result && !linecap) {
      यदि(!गिनती)
        result = curlx_dyn_addn(r, STRCONST("Cookie: "));
      यदि(!result) {
        result = curlx_dyn_addf(r, "%s%s", गिनती ? "; " : "", addcookies);
        गिनती++;
      }
    }
    यदि(गिनती && !result)
      result = curlx_dyn_addn(r, STRCONST("\r\n"));

    यदि(result)
      प्रत्यागम result;
  }
  प्रत्यागम result;
}
#अन्यथा
#define http_cookies(a, b) CURLE_OK
#endif

स्थिर CURLcode http_range(संरचना Curl_easy *data,
                           Curl_HttpReq httpreq)
{
  यदि(data->state.use_range) {
    /*
     * A range is selected. We use different headers whether we are downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    यदि(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_HEAD)) &&
       !Curl_checkheaders(data, STRCONST("Range"))) {
      /* if a line like this was already allocated, free the previous one */
      curlx_free(data->state.rangeline);
      data->state.rangeline = curl_maprintf("Range: bytes=%s\r\n",
                                                 data->state.range);
      यदि(!data->state.rangeline)
        प्रत्यागम CURLE_OUT_OF_MEMORY;
    }
    अन्यथा यदि((httpreq == HTTPREQ_POST || httpreq == HTTPREQ_PUT) &&
            !Curl_checkheaders(data, STRCONST("Content-Range"))) {
      curl_off_t req_clen = Curl_creader_total_length(data);
      /* if a line like this was already allocated, free the previous one */
      curlx_free(data->state.rangeline);

      यदि(data->set.set_resume_from < 0) {
        /* Upload resume was asked for, but we do not know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes 0-%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n", req_clen - 1, req_clen);
      }
      अन्यथा यदि(data->state.resume_from) {
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
      अन्यथा {
        /* Range was selected and then we pass the incoming range and append
           total size */
        data->state.rangeline =
          curl_maprintf("Content-Range: bytes %s/%" FMT_OFF_T "\r\n",
                        data->state.range, req_clen);
      }
      यदि(!data->state.rangeline)
        प्रत्यागम CURLE_OUT_OF_MEMORY;
    }
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode http_firstwrite(संरचना Curl_easy *data)
{
  संरचना connectdata *conn = data->conn;
  संरचना SingleRequest *k = &data->req;

  यदि(data->req.newurl) {
    यदि(conn->bits.बंद) {
      /* Abort after the headers if "follow Location" is set
         and we are set to close anyway. */
      CURL_REQ_CLEAR_RECV(data);
      k->done = TRUE;
      प्रत्यागम CURLE_OK;
    }
    /* We have a new URL to load, but since we want to be able to reuse this
       connection properly, we read the full response in "ignore more" */
    k->ignorebody = TRUE;
    infof(data, "Ignoring the response-body");
  }
  यदि(data->state.resume_from && !k->content_range &&
     (data->state.httpreq == HTTPREQ_GET) &&
     !k->ignorebody) {

    यदि(k->size == data->state.resume_from) {
      /* The resume point is at the end of file, consider this fine even if it
         does not allow resume from here. */
      infof(data, "The entire document is already downloaded");
      streamclose(conn);
      /* Abort download */
      CURL_REQ_CLEAR_RECV(data);
      k->done = TRUE;
      प्रत्यागम CURLE_OK;
    }

    /* we wanted to resume a download, although the server does not seem to
     * support this and we did this with a GET (if it was not a GET we did a
     * POST or PUT resume) */
    failf(data, "HTTP server does not seem to support "
          "byte ranges. Cannot resume.");
    प्रत्यागम CURLE_RANGE_ERROR;
  }

  यदि(data->set.timecondition && !data->state.range &&
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
    प्रत्यागम CURLE_OK;
  } /* we have a time condition */

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode http_check_new_conn(संरचना Curl_easy *data)
{
  संरचना connectdata *conn = data->conn;
  ध्रुव वर्ण *info_version = रिक्त;
  ध्रुव वर्ण *alpn;
  CURLcode result;

  alpn = Curl_conn_get_alpn_negotiated(data, conn);
  यदि(alpn && !म_भेद("h3", alpn)) {
#ifndef CURL_DISABLE_PROXY
    यदि(!conn->bits.origin_is_proxy)
#endif
      DEBUGASSERT(Curl_conn_http_version(data, conn) == 30);
    info_version = "HTTP/3";
  }
  अन्यथा यदि(alpn && !म_भेद("h2", alpn)) {
#ifndef CURL_DISABLE_PROXY
    यदि((Curl_conn_http_version(data, conn) != 20) &&
       conn->bits.origin_is_proxy) {
      result = Curl_http2_switch(data);
      यदि(result)
        प्रत्यागम result;
    }
    अन्यथा
#endif
    DEBUGASSERT(Curl_conn_http_version(data, conn) == 20);
    info_version = "HTTP/2";
  }
  अन्यथा {
    /* Check if user wants to use HTTP/2 with clear TCP */
    यदि(Curl_http2_may_switch(data)) {
      DEBUGF(infof(data, "HTTP/2 over clean TCP"));
      result = Curl_http2_switch(data);
      यदि(result)
        प्रत्यागम result;
      info_version = "HTTP/2";
      /* There is no ALPN here, but the connection is now definitely h2 */
      conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(conn);
    }
    अन्यथा
      info_version = "HTTP/1.x";
  }

  यदि(info_version)
    infof(data, "using %s", info_version);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode http_add_connection_hd(संरचना Curl_easy *data,
                                       संरचना dynbuf *req)
{
  संरचना curl_slist *head;
  ध्रुव वर्ण *sep = "Connection: ";
  CURLcode result = CURLE_OK;
  माप_प्रकार rlen = curlx_dyn_len(req);
  bool skip;

  /* Add the 1st custom "Connection: " header, if there is one */
  प्रति(head = data->set.headers; head; head = head->next) {
    यदि(curl_strnequal(head->data, "Connection", 10) &&
       Curl_headersep(head->data[10]) &&
       !http_header_is_empty(head->data)) {
      वर्ण *मूल्य;
      result = copy_custom_मूल्य(head->data, &मूल्य);
      यदि(result)
        प्रत्यागम result;
      result = curlx_dyn_addf(req, "%s%s", sep, मूल्य);
      sep = ", ";
      curlx_free(मूल्य);
      भङ्ग; /* leave, having added 1st one */
    }
  }

  /* add our internal Connection: header values, if we have any */
  यदि(!result && data->state.http_hd_te) {
    result = curlx_dyn_addf(req, "%s%s", sep, "TE");
    sep = ", ";
  }
  यदि(!result && data->state.http_hd_upgrade) {
    result = curlx_dyn_addf(req, "%s%s", sep, "Upgrade");
    sep = ", ";
  }
  यदि(!result && data->state.http_hd_h2_settings) {
    result = curlx_dyn_addf(req, "%s%s", sep, "HTTP2-Settings");
  }
  यदि(!result && (rlen < curlx_dyn_len(req)))
    result = curlx_dyn_addn(req, STRCONST("\r\n"));
  यदि(result)
    प्रत्यागम result;

  /* Add all user-defined Connection: headers after the first */
  skip = TRUE;
  प्रति(head = data->set.headers; head; head = head->next) {
    यदि(curl_strnequal(head->data, "Connection", 10) &&
       Curl_headersep(head->data[10]) &&
       !http_header_is_empty(head->data)) {
      यदि(skip) {
        skip = FALSE;
        अनुवर्त;
      }
      result = curlx_dyn_addf(req, "%s\r\n", head->data);
      यदि(result)
        प्रत्यागम result;
    }
  }

  प्रत्यागम CURLE_OK;
}

/* Header identifier in order we send them by default */
प्रकारनिर्देश गणना {
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

स्थिर CURLcode http_add_hd(संरचना Curl_easy *data,
                            संरचना dynbuf *req,
                            http_hd_t id,
                            अचिन्हित वर्ण httpversion,
                            ध्रुव वर्ण *method,
                            Curl_HttpReq httpreq)
{
  CURLcode result = CURLE_OK;
#यदि !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_PROXY) || \
  !defined(CURL_DISABLE_WEBSOCKETS)
  संरचना connectdata *conn = data->conn;
#endif
  विकल्प(id) {
  पक्ष H1_HD_REQUEST:
    /* add the main request stuff */
    /* GET/HEAD/POST/PUT */
    result = curlx_dyn_addf(req, "%s ", method);
    यदि(!result)
      result = http_target(data, req);
    यदि(!result)
      result = curlx_dyn_addf(req, " HTTP/%s\r\n",
                              get_http_string(httpversion));
    भङ्ग;

  पक्ष H1_HD_HOST:
    यदि(data->state.http_host) {
      result = curlx_dyn_add(req, data->state.http_host);
      यदि(!result)
        result = curlx_dyn_addn(req, STRCONST("\r\n"));
    }
    भङ्ग;

#ifndef CURL_DISABLE_PROXY
  पक्ष H1_HD_PROXY_AUTH:
    यदि(data->req.hd_proxy_auth)
      result = curlx_dyn_add(req, data->req.hd_proxy_auth);
    भङ्ग;
#endif

  पक्ष H1_HD_AUTH:
    यदि(data->req.hd_auth)
      result = curlx_dyn_add(req, data->req.hd_auth);
    भङ्ग;

  पक्ष H1_HD_RANGE:
    यदि(data->state.use_range && data->state.rangeline)
      result = curlx_dyn_add(req, data->state.rangeline);
    भङ्ग;

  पक्ष H1_HD_USER_AGENT: {
    ध्रुव वर्ण *ua = CURL_EASY_STR(data, STRING_USERAGENT);
    यदि(ua && *ua && !Curl_checkheaders(data, STRCONST("User-Agent")))
      result = curlx_dyn_addf(req, "User-Agent: %s\r\n", ua);
    भङ्ग;
  }

  पक्ष H1_HD_ACCEPT:
    यदि(!Curl_checkheaders(data, STRCONST("Accept")))
      result = curlx_dyn_add(req, "Accept: */*\r\n");
    भङ्ग;

  पक्ष H1_HD_TE:
#ifdef HAVE_LIBZ
    यदि(!Curl_checkheaders(data, STRCONST("TE")) &&
       data->set.http_transfer_encoding) {
      data->state.http_hd_te = TRUE;
      result = curlx_dyn_add(req, "TE: gzip\r\n");
    }
#endif
    भङ्ग;

  पक्ष H1_HD_ACCEPT_ENCODING: {
    ध्रुव वर्ण *enc = CURL_EASY_STR(data, STRING_ENCODING);
    यदि(enc && !Curl_checkheaders(data, STRCONST("Accept-Encoding")))
      result = curlx_dyn_addf(req, "Accept-Encoding: %s\r\n", enc);
    भङ्ग;
  }

  पक्ष H1_HD_REFERER:
    यदि(Curl_bufref_ptr(&data->state.referer) &&
       !Curl_checkheaders(data, STRCONST("Referer")))
      result = curlx_dyn_addf(req, "Referer: %s\r\n",
                              Curl_bufref_ptr(&data->state.referer));
    भङ्ग;

#ifndef CURL_DISABLE_PROXY
  पक्ष H1_HD_PROXY_CONNECTION:
    यदि(conn->bits.origin_is_proxy &&
       !Curl_checkheaders(data, STRCONST("Proxy-Connection")) &&
       !Curl_checkProxyheaders(data, data->conn, STRCONST("Proxy-Connection")))
      result = curlx_dyn_add(req, "Proxy-Connection: Keep-Alive\r\n");
    भङ्ग;
#endif

  पक्ष H1_HD_TRANSFER_ENCODING:
    result = http_req_set_TE(data, req, httpversion);
    भङ्ग;

#ifndef CURL_DISABLE_ALTSVC
  पक्ष H1_HD_ALT_USED:
    यदि(conn->bits.altused && conn->via_peer &&
       !Curl_checkheaders(data, STRCONST("Alt-Used")))
      result = curlx_dyn_addf(req, "Alt-Used: %s:%u\r\n",
                              conn->via_peer->hostname, conn->via_peer->port);
    भङ्ग;
#endif

  पक्ष H1_HD_UPGRADE:
    यदि(!Curl_conn_is_ssl(data->conn, FIRSTSOCKET) && (httpversion < 20) &&
       (data->state.http_neg.wanted & CURL_HTTP_V2x) &&
       data->state.http_neg.h2_upgrade) {
      /* append HTTP2 upgrade magic stuff to the HTTP request if it is not done
         over SSL */
      result = Curl_http2_request_upgrade(req, data);
    }
#ifndef CURL_DISABLE_WEBSOCKETS
    यदि(!result && conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
      result = Curl_ws_request(data, req);
#endif
    भङ्ग;

  पक्ष H1_HD_COOKIES:
    result = http_cookies(data, req);
    भङ्ग;

  पक्ष H1_HD_CONDITIONALS:
    result = Curl_add_timecondition(data, req);
    भङ्ग;

  पक्ष H1_HD_CUSTOM:
    result = Curl_add_custom_headers(data, FALSE, httpversion, req);
    भङ्ग;

  पक्ष H1_HD_CONTENT:
    result = http_add_content_hds(data, req, httpversion, httpreq);
    भङ्ग;

  पक्ष H1_HD_CONNECTION: {
    result = http_add_connection_hd(data, req);
    भङ्ग;
  }

  पक्ष H1_HD_LAST:
    result = curlx_dyn_addn(req, STRCONST("\r\n"));
    भङ्ग;
  }
  प्रत्यागम result;
}

/*
 * Curl_http() gets called from the generic multi_do() function when an HTTP
 * request is to be performed. This creates and sends a properly constructed
 * HTTP request.
 */
CURLcode Curl_http(संरचना Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  Curl_HttpReq httpreq;
  ध्रुव वर्ण *method;
  संरचना dynbuf req;
  अचिन्हित वर्ण httpversion;
  माप_प्रकार hd_id;

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

  यदि(!data->conn->bits.reuse) {
    result = http_check_new_conn(data);
    यदि(result)
      गच्छ out;
  }

  /* Add collecting of headers written to client. For a new connection,
   * we might have done that already, but reuse
   * or multiplex needs it here as well. */
  result = Curl_headers_init(data);
  यदि(result)
    गच्छ out;

  data->state.http_hd_te = FALSE;
  data->state.http_hd_upgrade = FALSE;
  data->state.http_hd_h2_settings = FALSE;

  /* what kind of request do we need to send? */
  Curl_http_method(data, &method, &httpreq);

  /* select host to send */
  result = http_set_aptr_host(data);
  /* setup the authentication headers, how that method and host are known */
  यदि(!result)
    result = Curl_http_output_auth(data, data->conn, method, httpreq,
                                   data->state.up.path,
                                   data->state.up.query, FALSE);
  /* Setup input reader, resume information and ranges */
  यदि(!result)
    result = set_reader(data, httpreq);
  यदि(!result)
    result = http_resume(data, httpreq);
  यदि(!result)
    result = http_range(data, httpreq);
  यदि(result)
    गच्छ out;

  httpversion = http_request_version(data);
  /* Add request line and all headers to `req` */
  प्रति(hd_id = 0; hd_id <= H1_HD_LAST; ++hd_id) {
    result = http_add_hd(data, &req, (http_hd_t)hd_id,
                         httpversion, method, httpreq);
    यदि(result)
      गच्छ out;
  }

  /* setup variables for the upcoming transfer and send */
  Curl_xfer_setup_sendrecv(data, FIRSTSOCKET, -1);
  result = Curl_req_send(data, &req, httpversion);

  यदि((httpversion >= 20) && data->req.upload_chunky)
    /* upload_chunky was set above to set up the request in a chunky fashion,
       but is disabled here again to avoid that the chunked encoded version is
       actually used when sending the request body over h2 */
    data->req.upload_chunky = FALSE;

out:
  यदि(result == CURLE_TOO_LARGE)
    failf(data, "HTTP request too large");

  curlx_dyn_free(&req);
  प्रत्यागम result;
}

प्रकारनिर्देश गणना {
  STATUS_UNKNOWN, /* not enough data to tell yet */
  STATUS_DONE, /* a status line was read */
  STATUS_BAD /* not a status line */
} statusline;

/* Check a string for a prefix. Check no more than 'len' bytes */
स्थिर bool checkprefixmax(ध्रुव वर्ण *prefix, ध्रुव वर्ण *buffer, माप_प्रकार len)
{
  माप_प्रकार ch = CURLMIN(म_माप(prefix), len);
  प्रत्यागम curl_strnequal(prefix, buffer, ch);
}

/*
 * checkhttpprefix()
 *
 * Returns TRUE if member of the list matches prefix of string
 */
स्थिर statusline checkhttpprefix(संरचना Curl_easy *data,
                                  ध्रुव वर्ण *s, माप_प्रकार len)
{
  संरचना curl_slist *head = data->set.http200aliases;
  statusline rc = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;

  यावत्(head) {
    यदि(checkprefixmax(head->data, s, len)) {
      rc = onmatch;
      भङ्ग;
    }
    head = head->next;
  }

  यदि((rc != STATUS_DONE) && checkprefixmax("HTTP/", s, len))
    rc = onmatch;

  प्रत्यागम rc;
}

#ifndef CURL_DISABLE_RTSP
स्थिर statusline checkrtspprefix(संरचना Curl_easy *data,
                                  ध्रुव वर्ण *s, माप_प्रकार len)
{
  statusline status = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;
  (शून्य)data;
  यदि(checkprefixmax("RTSP/", s, len))
    status = onmatch;

  प्रत्यागम status;
}
#endif /* CURL_DISABLE_RTSP */

स्थिर statusline checkprotoprefix(संरचना Curl_easy *data,
                                   संरचना connectdata *conn,
                                   ध्रुव वर्ण *s, माप_प्रकार len)
{
#ifndef CURL_DISABLE_RTSP
  यदि(conn->scheme->protocol & CURLPROTO_RTSP)
    प्रत्यागम checkrtspprefix(data, s, len);
#अन्यथा
  (शून्य)conn;
#endif /* CURL_DISABLE_RTSP */

  प्रत्यागम checkhttpprefix(data, s, len);
}

/* HTTP header has field name `n` (a string constant) */
#define HD_IS(hd, hdlen, n) \
  (((hdlen) >= (परिमाण(n) - 1)) && curl_strnequal(n, hd, परिमाण(n) - 1))

#define HD_VAL(hd, hdlen, n) \
  ((((hdlen) >= (परिमाण(n) - 1)) && (hd) && \
    curl_strnequal(n, hd, परिमाण(n) - 1)) ? ((hd) + (परिमाण(n) - 1)) : रिक्त)

/* HTTP header has field name `n` (a string constant) and contains `v`
 * (a string constant) in its value(s) */
#define HD_IS_AND_SAYS(hd, hdlen, n, v) \
  (HD_IS(hd, hdlen, n) && \
   ((hdlen) > ((परिमाण(n) - 1) + (परिमाण(v) - 1))) && \
   Curl_compareheader(hd, STRCONST(n), STRCONST(v)))

/*
 * http_header_a() parses a single response header starting with A.
 */
स्थिर CURLcode http_header_a(संरचना Curl_easy *data,
                              ध्रुव वर्ण *hd, माप_प्रकार hdlen)
{
#ifndef CURL_DISABLE_ALTSVC
  ध्रुव वर्ण *v;
  v = (data->asi &&
       (Curl_xfer_is_secure(data) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        दो_पर्या("CURL_ALTSVC_HTTP")
#अन्यथा
        0
#endif
         )) ? HD_VAL(hd, hdlen, "Alt-Svc:") : रिक्त;
  यदि(v) {
    /* the ALPN of the current request */
    संरचना SingleRequest *k = &data->req;
    गणना alpnid id = (k->httpversion == 30) ? ALPN_h3 :
      (k->httpversion == 20) ? ALPN_h2 : ALPN_h1;
    प्रत्यागम Curl_altsvc_parse(data, data->asi, v, data->state.origin, id);
  }
#अन्यथा
  (शून्य)data;
  (शून्य)hd;
  (शून्य)hdlen;
#endif
  प्रत्यागम CURLE_OK;
}

/*
 * http_header_c() parses a single response header starting with C.
 */
स्थिर CURLcode http_header_c(संरचना Curl_easy *data,
                              ध्रुव वर्ण *hd, माप_प्रकार hdlen)
{
  संरचना connectdata *conn = data->conn;
  संरचना SingleRequest *k = &data->req;
  ध्रुव वर्ण *v;

  /* Check for Content-Length: header lines to get size. Browsers insist we
     should accept multiple Content-Length headers and that a comma separated
     list also is fine and then we should accept them all as long as they are
     the same value. Different values trigger error.
   */
  v = (!k->http_bodyless && !data->set.ignorecl && !k->ignore_cl) ?
    HD_VAL(hd, hdlen, "Content-Length:") : रिक्त;
  यदि(v) {
    कुरु {
      curl_off_t contentlength;
      पूर्णाङ्क offt = curlx_str_numblanks(&v, &contentlength);

      यदि(offt == STRE_OVERFLOW) {
        /* out of range */
        यदि(data->set.max_filesize) {
          failf(data, "Maximum file size exceeded");
          प्रत्यागम CURLE_FILESIZE_EXCEEDED;
        }
        streamclose(conn);
        infof(data, "Overflow Content-Length: value");
        प्रत्यागम CURLE_OK;
      }
      अन्यथा {
        यदि((offt == STRE_OK) &&
           ((k->size == -1) || /* not set to something before */
            (k->size == contentlength))) { /* or the same value */

          k->size = contentlength;
          curlx_str_passblanks(&v);

          /* on a comma, loop and get the next instead */
          यदि(!curlx_str_single(&v, ','))
            अनुवर्त;

          यदि(!curlx_str_newline(&v)) {
            k->maxdownload = k->size;
            प्रत्यागम CURLE_OK;
          }
        }
        /* negative, different value or rubbish - bad HTTP */
        failf(data, "Invalid Content-Length: value");
        प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
      }
    } यावत्(1);
  }
  v = (!k->http_bodyless && CURL_EASY_STR(data, STRING_ENCODING)) ?
    HD_VAL(hd, hdlen, "Content-Encoding:") : रिक्त;
  यदि(v) {
    /*
     * Process Content-Encoding. Look for the values: identity, gzip, deflate,
     * compress, x-gzip and x-compress. x-gzip and x-compress are the same as
     * gzip and compress. (Sec 3.5 RFC 2616). zlib cannot handle compress.
     * Errors are handled further down when the response body is processed
     */
    प्रत्यागम Curl_build_unencoding_ढेर(data, v, FALSE);
  }
  /* check for Content-Type: header lines to get the MIME-type */
  v = HD_VAL(hd, hdlen, "Content-Type:");
  यदि(v) {
    वर्ण *contenttype = Curl_copy_header_मूल्य(hd);
    यदि(!contenttype)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    यदि(!*contenttype)
      /* ignore empty data */
      curlx_free(contenttype);
    अन्यथा {
      curlx_free(data->info.contenttype);
      data->info.contenttype = contenttype;
    }
    प्रत्यागम CURLE_OK;
  }
  यदि((k->httpversion < 20) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "close")) {
    /*
     * [RFC 2616, section 8.1.2.1]
     * "Connection: close" is HTTP/1.1 language and means that
     * the connection will close when this request has been
     * served.
     */
    connclose(conn);
    प्रत्यागम CURLE_OK;
  }
  यदि((k->httpversion == 10) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "keep-alive")) {
    /*
     * An HTTP/1.0 reply with the 'Connection: keep-alive' line
     * tells us the connection will be kept alive for our
     * pleasure. Default action for 1.0 is to close.
     *
     * [RFC2068, section 19.7.1] */
    connkeep(conn);
    infof(data, "HTTP/1.0 connection set to keep alive");
    प्रत्यागम CURLE_OK;
  }
  v = !k->http_bodyless ? HD_VAL(hd, hdlen, "Content-Range:") : रिक्त;
  यदि(v) {
    /* Content-Range: bytes [num]-
       Content-Range: bytes: [num]-
       Content-Range: [num]-
       Content-Range: [asterisk]/[total]

       The second format was added since Sun's webserver
       JavaWebServer/1.1.1 obviously sends the header this way!
       The third added since some servers use that!
       The fourth means the requested range was unsatisfied.
     */

    ध्रुव वर्ण *ptr = v;

    /* Move forward until first digit or asterisk */
    यावत्(*ptr && !ISDIGIT(*ptr) && *ptr != '*')
      ptr++;

    /* if it truly stopped on a digit */
    यदि(ISDIGIT(*ptr)) {
      यदि(!curlx_str_संख्या(&ptr, &k->offset, CURL_OFF_T_MAX) &&
         (data->state.resume_from == k->offset))
        /* we asked for a resume and we got it */
        k->content_range = TRUE;
    }
    अन्यथा यदि(k->httpcode < 300)
      data->state.resume_from = 0; /* get everything */
  }
  प्रत्यागम CURLE_OK;
}

/*
 * http_header_l() parses a single response header starting with L.
 */
स्थिर CURLcode http_header_l(संरचना Curl_easy *data,
                              ध्रुव वर्ण *hd, माप_प्रकार hdlen)
{
  संरचना connectdata *conn = data->conn;
  संरचना SingleRequest *k = &data->req;
  ध्रुव वर्ण *v = (!k->http_bodyless &&
                   (data->set.timecondition || data->set.get_filetime)) ?
    HD_VAL(hd, hdlen, "Last-Modified:") : रिक्त;
  यदि(v) {
    यदि(Curl_getdate_capped(v, &k->timeofdoc))
      k->timeofdoc = 0;
    यदि(data->set.get_filetime)
      data->info.filetime = k->timeofdoc;
    प्रत्यागम CURLE_OK;
  }
  यदि(HD_IS(hd, hdlen, "Location:")) {
    /* this is the URL that the server advises us to use instead */
    वर्ण *location = Curl_copy_header_मूल्य(hd);
    यदि(!location)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    यदि(!*location ||
       (data->req.location && !म_भेद(data->req.location, location))) {
      /* ignore empty header, or exact repeat of a previous one */
      curlx_free(location);
      प्रत्यागम CURLE_OK;
    }
    अन्यथा {
      /* has value and is not an exact repeat */
      यदि(data->req.location) {
        failf(data, "Multiple Location headers");
        curlx_free(location);
        प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
      }
      data->req.location = location;

      यदि((k->httpcode >= 300 && k->httpcode < 400) &&
         data->set.http_follow_mode) {
        CURLcode result;
        DEBUGASSERT(!data->req.newurl);
        data->req.newurl = curlx_strdup(data->req.location); /* clone */
        यदि(!data->req.newurl)
          प्रत्यागम CURLE_OUT_OF_MEMORY;

        /* some cases of POST and PUT etc needs to rewind the data
           stream at this point */
        result = http_perhapsrewind(data, conn);
        यदि(result)
          प्रत्यागम result;

        /* mark the next request as a followed location: */
        data->state.this_is_a_follow = TRUE;
      }
    }
  }
  प्रत्यागम CURLE_OK;
}

/*
 * http_header_p() parses a single response header starting with P.
 */
स्थिर CURLcode http_header_p(संरचना Curl_easy *data,
                              ध्रुव वर्ण *hd, माप_प्रकार hdlen)
{
  संरचना SingleRequest *k = &data->req;

#ifndef CURL_DISABLE_PROXY
  ध्रुव वर्ण *v = HD_VAL(hd, hdlen, "Proxy-Connection:");
  यदि(v) {
    संरचना connectdata *conn = data->conn;
    यदि((k->httpversion == 10) && conn->http_proxy.peer &&
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
    अन्यथा यदि((k->httpversion == 11) && conn->http_proxy.peer &&
            HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "close")) {
      /*
       * We get an HTTP/1.1 response from a proxy and it says it will
       * close down after this transfer.
       */
      connclose(conn);
      infof(data, "HTTP/1.1 proxy connection set close");
    }
    प्रत्यागम CURLE_OK;
  }
#endif
  यदि((407 == k->httpcode) && HD_IS(hd, hdlen, "Proxy-authenticate:")) {
    वर्ण *auth;
    CURLcode result;
#ifndef CURL_DISABLE_PROXY
    यदि(!data->conn->bits.origin_is_proxy)
      /* this response is not from a forward proxy, not for us */
      प्रत्यागम CURLE_OK;
#endif
    auth = Curl_copy_header_मूल्य(hd);
    result = auth ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    यदि(!result) {
      result = Curl_http_निवेश_auth(data, TRUE, auth);
      curlx_free(auth);
    }
    प्रत्यागम result;
  }
#ifdef USE_SPNEGO
  यदि(HD_IS(hd, hdlen, "Persistent-Auth:")) {
    संरचना connectdata *conn = data->conn;
    संरचना negotiatedata *negdata = Curl_auth_nego_get(conn, FALSE);
    संरचना auth *authp = &data->state.authhost;
    यदि(!negdata)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    यदि(authp->picked == CURLAUTH_NEGOTIATE) {
      वर्ण *persistentauth = Curl_copy_header_मूल्य(hd);
      यदि(!persistentauth)
        प्रत्यागम CURLE_OUT_OF_MEMORY;
      negdata->noauthpersist = !!checkprefix("false", persistentauth);
      negdata->havenoauthpersist = TRUE;
      infof(data, "Negotiate: noauthpersist -> %d, header part: %s",
            negdata->noauthpersist, persistentauth);
      curlx_free(persistentauth);
    }
  }
#endif
  प्रत्यागम CURLE_OK;
}

/*
 * http_header_r() parses a single response header starting with R.
 */
स्थिर CURLcode http_header_r(संरचना Curl_easy *data,
                              ध्रुव वर्ण *hd, माप_प्रकार hdlen)
{
  ध्रुव वर्ण *v = HD_VAL(hd, hdlen, "Retry-After:");
  यदि(v) {
    /* Retry-After = HTTP-date / delay-seconds */
    curl_off_t retry_after = 0; /* zero for unknown or "now" */
    समय_प्रकार date = 0;
    curlx_str_passblanks(&v);

    /* try it as a date first, because a date can otherwise start with and
       get treated as a number */
    यदि(!Curl_getdate_capped(v, &date)) {
      समय_प्रकार current = समय(रिक्त);
      यदि(date >= current)
        /* convert date to number of seconds into the future */
        retry_after = date - current;
    }
    अन्यथा
      /* Try it as a decimal number, ignore errors */
      (शून्य)curlx_str_संख्या(&v, &retry_after, CURL_OFF_T_MAX);
    /* limit to 6 hours max. this is not documented so that it can be changed
       in the future if necessary. */
    यदि(retry_after > 21600)
      retry_after = 21600;
    data->info.retry_after = retry_after;
  }
  प्रत्यागम CURLE_OK;
}

/*
 * http_header_s() parses a single response header starting with S.
 */
स्थिर CURLcode http_header_s(संरचना Curl_easy *data,
                              ध्रुव वर्ण *hd, माप_प्रकार hdlen)
{
#यदि !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_HSTS)
  ध्रुव वर्ण *v;
#अन्यथा
  (शून्य)data;
  (शून्य)hd;
  (शून्य)hdlen;
#endif

#ifndef CURL_DISABLE_COOKIES
  v = (data->cookies && data->state.cookie_engine) ?
    HD_VAL(hd, hdlen, "Set-Cookie:") : रिक्त;
  यदि(v) {
    /* If there is a custom-set Host: name, use it here, or else use
     * real peer hostname. */
    ध्रुव वर्ण *host = data->req.cookiehost ?
      data->req.cookiehost : data->state.origin->hostname;
    ध्रुव अचिन्हित वर्ण secure_context = Curl_secure_context(data, host) ?
      COOKIE_SECURE : 0;
    CURLcode result;
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    result = Curl_cookie_add(data, data->cookies, v, host,
                             data->state.up.path,
                             COOKIE_HTTPHEADER | secure_context);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
    प्रत्यागम result;
  }
#endif
#ifndef CURL_DISABLE_HSTS
  /* If enabled, the header is incoming and this is over HTTPS */
  v = (data->hsts &&
       (Curl_xfer_is_secure(data) ||
#ifdef DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        दो_पर्या("CURL_HSTS_HTTP")
#अन्यथा
        0
#endif
         )
    ) ? HD_VAL(hd, hdlen, "Strict-Transport-Security:") : रिक्त;
  यदि(v) {
    CURLcode result = Curl_hsts_parse(
      data->hsts, data->state.origin->hostname, v);
    यदि(result) {
      यदि(result == CURLE_OUT_OF_MEMORY)
        प्रत्यागम result;
      infof(data, "Illegal STS header skipped");
    }
#ifdef DEBUGBUILD
    अन्यथा
      infof(data, "Parsed STS header fine (%zu entries)",
            Curl_llist_गिनती(&data->hsts->list));
#endif
  }
#endif

  प्रत्यागम CURLE_OK;
}

/*
 * http_header_t() parses a single response header starting with T.
 */
स्थिर CURLcode http_header_t(संरचना Curl_easy *data,
                              ध्रुव वर्ण *hd, माप_प्रकार hdlen)
{
  संरचना connectdata *conn = data->conn;
  संरचना SingleRequest *k = &data->req;

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
  ध्रुव वर्ण *v = (!k->http_bodyless &&
                   (data->state.httpreq != HTTPREQ_HEAD) &&
                   (k->httpcode != 304)) ?
    HD_VAL(hd, hdlen, "Transfer-Encoding:") : रिक्त;
  यदि(v) {
    /* One or more encodings. We check for chunked and/or a compression
       algorithm. */
    CURLcode result = Curl_build_unencoding_ढेर(data, v, TRUE);
    यदि(result)
      प्रत्यागम result;
    यदि(!k->chunk) {
      /* if this is not chunked, only close can signal the end of this
       * transfer as Content-Length is said not to be trusted for
       * transfer-encoding! */
      CURL_TRC_M(data, "HTTP/1.1 transfer-encoding without chunks");
      connclose(conn);
      k->ignore_cl = TRUE;
    }
    प्रत्यागम CURLE_OK;
  }
  v = HD_VAL(hd, hdlen, "Trailer:");
  यदि(v) {
    data->req.resp_trailer = TRUE;
    प्रत्यागम CURLE_OK;
  }
  प्रत्यागम CURLE_OK;
}

/*
 * http_header_w() parses a single response header starting with W.
 */
स्थिर CURLcode http_header_w(संरचना Curl_easy *data,
                              ध्रुव वर्ण *hd, माप_प्रकार hdlen)
{
  संरचना SingleRequest *k = &data->req;
  CURLcode result = CURLE_OK;

  यदि((401 == k->httpcode) && HD_IS(hd, hdlen, "WWW-Authenticate:")) {
    वर्ण *auth = Curl_copy_header_मूल्य(hd);
    यदि(!auth)
      result = CURLE_OUT_OF_MEMORY;
    अन्यथा {
      result = Curl_http_निवेश_auth(data, FALSE, auth);
      curlx_free(auth);
    }
  }
  प्रत्यागम result;
}

/*
 * http_header() parses a single response header.
 */
स्थिर CURLcode http_header(संरचना Curl_easy *data,
                            ध्रुव वर्ण *hd, माप_प्रकार hdlen)
{
  CURLcode result = CURLE_OK;

  विकल्प(hd[0]) {
  पक्ष 'a':
  पक्ष 'A':
    result = http_header_a(data, hd, hdlen);
    भङ्ग;
  पक्ष 'c':
  पक्ष 'C':
    result = http_header_c(data, hd, hdlen);
    भङ्ग;
  पक्ष 'l':
  पक्ष 'L':
    result = http_header_l(data, hd, hdlen);
    भङ्ग;
  पक्ष 'p':
  पक्ष 'P':
    result = http_header_p(data, hd, hdlen);
    भङ्ग;
  पक्ष 'r':
  पक्ष 'R':
    result = http_header_r(data, hd, hdlen);
    भङ्ग;
  पक्ष 's':
  पक्ष 'S':
    result = http_header_s(data, hd, hdlen);
    भङ्ग;
  पक्ष 't':
  पक्ष 'T':
    result = http_header_t(data, hd, hdlen);
    भङ्ग;
  पक्ष 'w':
  पक्ष 'W':
    result = http_header_w(data, hd, hdlen);
    भङ्ग;
  }

  यदि(!result) {
    संरचना connectdata *conn = data->conn;
    यदि(conn->scheme->protocol & CURLPROTO_RTSP)
      result = Curl_rtsp_parseheader(data, hd);
  }
  प्रत्यागम result;
}

/*
 * Called after the first HTTP response line (the status line) has been
 * received and parsed.
 */
स्थिर CURLcode http_statusline(संरचना Curl_easy *data,
                                संरचना connectdata *conn)
{
  संरचना SingleRequest *k = &data->req;

  विकल्प(k->httpversion) {
  पक्ष 10:
  पक्ष 11:
#ifdef USE_HTTP2
  पक्ष 20:
#endif
#ifdef USE_HTTP3
  पक्ष 30:
#endif
    /* no major version switch mid-connection */
    यदि(k->httpversion_sent &&
       (k->httpversion / 10 != k->httpversion_sent / 10)) {
      failf(data, "Version mismatch (from HTTP/%d to HTTP/%d)",
            k->httpversion_sent / 10, k->httpversion / 10);
      प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
    }
    भङ्ग;
  स्वभाव:
    failf(data, "Unsupported HTTP version (%d.%d) in response",
          k->httpversion / 10, k->httpversion % 10);
    प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
  }

  data->info.httpcode = k->httpcode;
  data->info.httpversion = k->httpversion;
  conn->httpversion_seen = k->httpversion;

  यदि(!data->state.http_neg.rcvd_min ||
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
  यदि(data->state.resume_from && data->state.httpreq == HTTPREQ_GET &&
     k->httpcode == 416) {
    /* "Requested Range Not Satisfiable", proceed and pretend this is no
       error */
    k->ignorebody = TRUE; /* Avoid appending error msg to good data. */
  }

  यदि(k->httpversion == 10) {
    /* Default action for HTTP/1.0 must be to close, unless
       we get one of those fancy headers that tell us the
       server keeps it open for us! */
    infof(data, "HTTP 1.0, assume close after body");
    connclose(conn);
  }

  k->http_bodyless = k->httpcode >= 100 && k->httpcode < 200;
  विकल्प(k->httpcode) {
  पक्ष 304:
    /* (quote from RFC2616, section 10.3.5): The 304 response
     * MUST NOT contain a message-body, and thus is always
     * terminated by the first empty line after the header
     * fields. */
    यदि(data->set.timecondition)
      data->info.timecond = TRUE;
    FALLTHROUGH();
  पक्ष 204:
    /* (quote from RFC2616, section 10.2.5): The server has
     * fulfilled the request but does not need to return an
     * entity-body ... The 204 response MUST NOT include a
     * message-body, and thus is always terminated by the first
     * empty line after the header fields. */
    k->size = 0;
    k->maxdownload = 0;
    k->http_bodyless = TRUE;
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

/* Content-Length must be ignored if any Transfer-Encoding is present in the
   response. Refer to RFC 7230 section 3.3.3 and RFC2616 section 4.4. This is
   figured out here after all headers have been received but before the final
   call to the user's header callback, so that a valid content length can be
   retrieved by the user in the final call. */
स्थिर CURLcode http_size(संरचना Curl_easy *data)
{
  संरचना SingleRequest *k = &data->req;
  यदि(data->req.ignore_cl || k->chunk) {
    k->size = k->maxdownload = -1;
  }
  अन्यथा यदि(k->size != -1) {
    यदि(data->set.max_filesize &&
       !k->ignorebody &&
       (k->size > data->set.max_filesize)) {
      failf(data, "Maximum file size exceeded");
      प्रत्यागम CURLE_FILESIZE_EXCEEDED;
    }
    यदि(k->ignorebody)
      infof(data, "setting size while ignoring");
    Curl_pgrsSetDownloadSize(data, k->size);
    k->maxdownload = k->size;
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_verify_header(संरचना Curl_easy *data,
                            ध्रुव वर्ण *hd, माप_प्रकार hdlen)
{
  संरचना SingleRequest *k = &data->req;
  माप_प्रकार i;
  bool has_colon = FALSE;
  प्रति(i = 0; i < hdlen; i++) {
    विकल्प(hd[i]) {
    पक्ष '\0':
      /* this is bad, bail out */
      failf(data, "NUL byte in header");
      प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
    पक्ष '\r':
      यदि(i < hdlen - 2) {
        /* CR may only precede the LF, nothing else */
        failf(data, "Carriage return found in header");
        प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
      }
      भङ्ग;
    पक्ष ':':
      has_colon = TRUE;
      भङ्ग;
    }
  }
  यदि(k->headerline < 2)
    /* the first "header" is the status-line and it has no colon */
    प्रत्यागम CURLE_OK;
  यदि(((hd[0] == ' ') || (hd[0] == '\t')) && k->headerline > 2)
    /* line folding, cannot happen on line 2 */
    ;
  अन्यथा यदि(!has_colon) {
    /* this is bad, bail out */
    failf(data, "Header without colon");
    प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_bump_headersize(संरचना Curl_easy *data,
                              माप_प्रकार delta,
                              bool connect_only)
{
  माप_प्रकार bad = 0;
  अचिन्हित पूर्णाङ्क max = MAX_HTTP_RESP_HEADER_SIZE;
  यदि(delta < MAX_HTTP_RESP_HEADER_SIZE) {
    data->info.header_size += (अचिन्हित पूर्णाङ्क)delta;
    data->req.allheadercount += (अचिन्हित पूर्णाङ्क)delta;
    यदि(!connect_only)
      data->req.headerbytecount += (अचिन्हित पूर्णाङ्क)delta;
    यदि(data->req.allheadercount > max)
      bad = data->req.allheadercount;
    अन्यथा यदि(data->info.header_size > (max * 20)) {
      bad = data->info.header_size;
      max *= 20;
    }
  }
  अन्यथा
    bad = data->req.allheadercount + delta;
  यदि(bad) {
    failf(data, "Too large response headers: %zu > %u", bad, max);
    प्रत्यागम CURLE_RECV_ERROR;
  }
  प्रत्यागम CURLE_OK;
}

/*
 * Handle a 101 Switching Protocols response. Performs the actual protocol
 * upgrade to HTTP/2 or WebSocket based on what was requested.
 */
स्थिर CURLcode http_on_101_upgrade(संरचना Curl_easy *data,
                                    ध्रुव वर्ण *buf, माप_प्रकार blen,
                                    माप_प्रकार *pconsumed,
                                    bool *conn_changed)
{
  संरचना connectdata *conn = data->conn;
  संरचना SingleRequest *k = &data->req;

#यदि !defined(USE_NGHTTP2) && defined(CURL_DISABLE_WEBSOCKETS)
  (शून्य)buf;
  (शून्य)blen;
  (शून्य)pconsumed;
#अन्यथा
  CURLcode result;
  पूर्णाङ्क upgr101_requested = k->upgr101;
#endif

  यदि(k->httpversion_sent != 11) {
    /* invalid for other HTTP versions */
    failf(data, "server sent 101 response while not talking HTTP/1.1");
    प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
  }

  /* Whatever the success, upgrade was selected. */
  k->upgr101 = UPGR101_RECEIVED;
  conn->bits.upgrade_in_progress = FALSE;
  *conn_changed = TRUE;

  /* To be fully compliant, we would check the "Upgrade:" response header to
   * mention the protocol we requested. */
#ifdef USE_NGHTTP2
  यदि(upgr101_requested == UPGR101_H2) {
    /* Switch to HTTP/2, where we will get more responses. blen bytes in buf
     * are already h2 protocol bytes */
    infof(data, "Received 101, Switching to HTTP/2");
    result = Curl_http2_upgrade(data, conn, FIRSTSOCKET, buf, blen);
    यदि(!result)
      *pconsumed += blen;
    प्रत्यागम result;
  }
#endif
#ifndef CURL_DISABLE_WEBSOCKETS
  यदि(upgr101_requested == UPGR101_WS) {
    /* Switch to WebSocket, where we now stream ws frames. blen bytes in buf
     * are already ws protocol bytes */
    infof(data, "Received 101, Switching to WebSocket");
    result = Curl_ws_accept(data, buf, blen);
    यदि(!result)
      *pconsumed += blen; /* ws accept handled the data */
    प्रत्यागम result;
  }
#endif
  /* We silently accept this as the final response. What are we switching to
   * if we did not ask for an Upgrade? Maybe the application provided an
   * `Upgrade: xxx` header? */
  k->header = FALSE;
  प्रत्यागम CURLE_OK;
}

/*
 * Handle 1xx intermediate HTTP responses. Sets up state for more
 * headers and processes 100-continue and 101 upgrade responses.
 */
स्थिर CURLcode http_on_1xx_response(संरचना Curl_easy *data,
                                     ध्रुव वर्ण *buf, माप_प्रकार blen,
                                     माप_प्रकार *pconsumed,
                                     bool *conn_changed)
{
  संरचना SingleRequest *k = &data->req;

  /* "A user agent MAY ignore unexpected 1xx status responses."
   * By default, we expect to get more responses after this one. */
  k->header = TRUE;
  k->headerline = 0; /* restart the header line counter */

  विकल्प(k->httpcode) {
  पक्ष 100:
    /* We have made an HTTP PUT or POST and this is 1.1-lingo that tells us
     * that the server is OK with this and ready to receive the data. */
    http_exp100_got100(data);
    भङ्ग;
  पक्ष 101:
    प्रत्यागम http_on_101_upgrade(data, buf, blen, pconsumed, conn_changed);
  स्वभाव:
    /* The server may send us other 1xx responses, like informative 103. This
     * has no influence on request processing and we expect to receive a
     * final response eventually. */
    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

#यदि defined(USE_NTLM) || defined(USE_SPNEGO)
/*
 * Check if NTLM or SPNEGO authentication negotiation failed due to
 * connection closure (typically on HTTP/1.0 servers).
 */
स्थिर शून्य http_check_auth_closure(संरचना Curl_easy *data,
                                    संरचना connectdata *conn)
{
  /* At this point we have some idea about the fate of the connection. If we
     are closing the connection it may result auth failure. */
#ifdef USE_NTLM
  यदि(conn->bits.बंद &&
     (((data->req.httpcode == 401) &&
       (conn->http_ntlm_state == NTLMSTATE_TYPE2)) ||
      ((data->req.httpcode == 407) &&
       (conn->proxy_ntlm_state == NTLMSTATE_TYPE2)))) {
    infof(data, "Connection closure while negotiating auth (HTTP 1.0?)");
    data->state.authproblem = TRUE;
  }
#endif
#ifdef USE_SPNEGO
  यदि(conn->bits.बंद &&
    (((data->req.httpcode == 401) &&
      (conn->http_negotiate_state == CURL_NEGO_AUTH_RECV)) ||
     ((data->req.httpcode == 407) &&
      (conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)))) {
    infof(data, "Connection closure while negotiating auth (HTTP 1.0?)");
    data->state.authproblem = TRUE;
  }
  यदि((conn->http_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (data->req.httpcode != 401)) {
    conn->http_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
  यदि((conn->proxy_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (data->req.httpcode != 407)) {
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
#endif
}
#अन्यथा
#define http_check_auth_closure(x, y) /* empty */
#endif

/*
 * Handle an error response (>= 300) received while still sending the
 * request body. Deals with 417 Expectation Failed retries, keep-sending
 * on error, and aborting the send.
 */
स्थिर CURLcode http_handle_send_error(संरचना Curl_easy *data)
{
  संरचना connectdata *conn = data->conn;
  संरचना SingleRequest *k = &data->req;
  CURLcode result = CURLE_OK;

  यदि(!data->req.authneg && !conn->bits.बंद &&
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

    विकल्प(data->state.httpreq) {
    पक्ष HTTPREQ_PUT:
    पक्ष HTTPREQ_POST:
    पक्ष HTTPREQ_POST_FORM:
    पक्ष HTTPREQ_POST_MIME:
      /* We got an error response. If this happened before the
       * whole request body has been sent we stop sending and
       * mark the connection for closure after we have read the
       * entire response. */
      यदि(!Curl_req_done_sending(data)) {
        यदि((k->httpcode == 417) && http_exp100_is_selected(data)) {
          /* 417 Expectation Failed - try again without the
             Expect header */
          यदि(!k->writebytecount && http_exp100_is_waiting(data)) {
            infof(data, "Got HTTP failure 417 while waiting for a 100");
          }
          अन्यथा {
            infof(data, "Got HTTP failure 417 while sending data");
            streamclose(conn);
            result = http_perhapsrewind(data, conn);
            यदि(result)
              प्रत्यागम result;
          }
          data->state.disableexpect = TRUE;
          Curl_req_abort_sending(data);
          DEBUGASSERT(!data->req.newurl);
          data->req.newurl = Curl_bufref_dup(&data->state.url);
          यदि(!data->req.newurl)
            प्रत्यागम CURLE_OUT_OF_MEMORY;
        }
        अन्यथा यदि(data->set.http_keep_sending_on_error) {
          infof(data, "HTTP error before end of send, keep sending");
          http_exp100_send_anyway(data);
        }
        अन्यथा {
          infof(data, "HTTP error before end of send, stop sending");
          streamclose(conn);
          result = Curl_req_abort_sending(data);
          यदि(result)
            प्रत्यागम result;
        }
      }
      भङ्ग;

    स्वभाव: /* default label present to avoid compiler warnings */
      भङ्ग;
    }
  }

  यदि(Curl_creader_will_rewind(data) && !Curl_req_done_sending(data)) {
    /* We rewind before next send, continue sending now */
    infof(data, "Keep sending data to get tossed away");
    CURL_REQ_SET_SEND(data);
  }
  प्रत्यागम result;
}

स्थिर CURLcode http_on_response(संरचना Curl_easy *data,
                                 ध्रुव वर्ण *last_hd, माप_प्रकार last_hd_len,
                                 ध्रुव वर्ण *buf, माप_प्रकार blen,
                                 माप_प्रकार *pconsumed)
{
  संरचना connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  संरचना SingleRequest *k = &data->req;
  bool conn_changed = FALSE;

  (शून्य)buf; /* not used without HTTP2 enabled */
  *pconsumed = 0;

  यदि(k->upgr101 == UPGR101_RECEIVED) {
    /* supposedly upgraded to http2 now */
    यदि(data->req.httpversion != 20)
      infof(data, "Lying server, not serving HTTP/2");
  }

  यदि(k->httpcode < 200 && last_hd) {
    /* Intermediate responses might trigger processing of more responses,
     * write the last header to the client before proceeding. */
    result = http_write_header(data, last_hd, last_hd_len);
    last_hd = रिक्त; /* handled it */
    यदि(result)
      गच्छ out;
  }

  यदि(k->httpcode < 100) {
    failf(data, "Unsupported response code in HTTP response");
    result = CURLE_UNSUPPORTED_PROTOCOL;
    गच्छ out;
  }
  अन्यथा यदि(k->httpcode < 200) {
    result = http_on_1xx_response(data, buf, blen, pconsumed, &conn_changed);
    गच्छ out;
  }

  /* k->httpcode >= 200, final response */
  k->header = FALSE;
  यदि(conn->bits.upgrade_in_progress) {
    /* Asked for protocol upgrade, but it was not selected */
    conn->bits.upgrade_in_progress = FALSE;
    conn_changed = TRUE;
  }

  यदि((k->size == -1) && !k->chunk && !conn->bits.बंद &&
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
  यदि(data->req.upgr101 == UPGR101_WS) {
    failf(data, "Refused WebSocket upgrade: %d", k->httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
    गच्छ out;
  }
#endif

  /* Check if this response means the transfer errored. */
  यदि(http_should_fail(data, data->req.httpcode)) {
    failf(data, "The requested URL returned error: %d",
          k->httpcode);
    result = CURLE_HTTP_RETURNED_ERROR;
    गच्छ out;
  }

  /* Curl_http_auth_act() checks what authentication methods that are
   * available and decides which one (if any) to use. It will set 'newurl' if
   * an auth method was picked. */
  result = Curl_http_auth_act(data);
  यदि(result)
    गच्छ out;

  यदि(k->httpcode >= 300) {
    result = http_handle_send_error(data);
    यदि(result)
      गच्छ out;
  }

  /* final response without error, prepare to receive the body */
  result = http_firstwrite(data);
  यदि(result)
    गच्छ out;

  /* This is the last response that we get for the current request. Check on
   * the body size and determine if the response is complete. */
  result = http_size(data);
  यदि(result)
    गच्छ out;

#ifndef CURL_DISABLE_RTSP
  /* An RTSP response to any method may carry a body, which is announced
     with Content-Length (RFC 2326 section 12.14). Read it instead of
     treating it as excess data, unless the application asked for no body
     with CURLOPT_NOBODY. */
  यदि((conn->scheme->protocol & CURLPROTO_RTSP) && data->req.no_body &&
     !data->set.opt_no_body && (k->size > 0))
    data->req.no_body = FALSE;
#endif

  /* If we requested a "no body", this is a good time to get
   * out and return home.
   */
  यदि(data->req.no_body)
    k->download_done = TRUE;

  /* If max download size is *zero* (nothing) we already have nothing and can
     safely return ok now! For HTTP/2, we would like to call
     http2_handle_stream_close to properly close a stream. In order to do
     this, we keep reading until we close the stream. */
  यदि((k->maxdownload == 0) && (k->httpversion_sent < 20))
    k->download_done = TRUE;

out:
  यदि(last_hd)
    /* if not written yet, write it now */
    result = Curl_1st_fatal(result,
                            http_write_header(data, last_hd, last_hd_len));
  यदि(conn_changed)
    /* poke the multi handle to allow pending pipewait to retry */
    Curl_multi_connchanged(data->multi);
  प्रत्यागम result;
}

स्थिर CURLcode http_rw_hd(संरचना Curl_easy *data,
                           ध्रुव वर्ण *hd, माप_प्रकार hdlen,
                           ध्रुव वर्ण *buf_remain, माप_प्रकार blen,
                           माप_प्रकार *pconsumed)
{
  CURLcode result = CURLE_OK;
  संरचना SingleRequest *k = &data->req;
  पूर्णाङ्क writetype;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  *pconsumed = 0;
  यदि((0x0a == *hd) || (0x0d == *hd)) {
    /* Empty header line means end of headers! */
    संरचना dynbuf last_header;
    माप_प्रकार consumed;

    curlx_dyn_init(&last_header, hdlen + 1);
    result = curlx_dyn_addn(&last_header, hd, hdlen);
    यदि(result)
      प्रत्यागम result;

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
    प्रत्यागम result;
  }

  /*
   * Checks for special headers coming up.
   */

  writetype = CLIENTWRITE_HEADER;
  यदि(!k->headerline++) {
    /* This is the first header, it MUST be the error code line
       or else we consider this to be the body right away! */
    bool fine_statusline = FALSE;

    k->httpversion = 0; /* Do not know yet */
    यदि(data->conn->scheme->protocol & PROTO_FAMILY_HTTP) {
      /*
       * https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.2
       *
       * The response code is always a three-digit number in HTTP as the spec
       * says. We allow any three-digit number here, but we cannot make
       * guarantees on future behaviors since it is not within the protocol.
       */
      ध्रुव वर्ण *p = hd;

      curlx_str_passblanks(&p);
      यदि(!म_भेदन(p, "HTTP/", 5)) {
        p += 5;
        विकल्प(*p) {
        पक्ष '1':
          p++;
          यदि((p[0] == '.') && (p[1] == '0' || p[1] == '1')) {
            यदि(ISBLANK(p[2])) {
              k->httpversion = (अचिन्हित वर्ण)(10 + (p[1] - '0'));
              p += 3;
              यदि(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
                k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
                  (p[2] - '0');
                /* RFC 9112 requires a single space following the status code,
                   but the browsers do not so let's not insist */
                fine_statusline = TRUE;
              }
            }
          }
          यदि(!fine_statusline) {
            failf(data, "Unsupported HTTP/1 subversion in response");
            प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
          }
          भङ्ग;
        पक्ष '2':
        पक्ष '3':
          यदि(!ISBLANK(p[1]))
            भङ्ग;
          k->httpversion = (अचिन्हित वर्ण)((*p - '0') * 10);
          p += 2;
          यदि(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
            k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
              (p[2] - '0');
            p += 3;
            यदि(!ISBLANK(*p))
              भङ्ग;
            fine_statusline = TRUE;
          }
          भङ्ग;
        स्वभाव: /* unsupported */
          failf(data, "Unsupported HTTP version in response");
          प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
        }
      }

      यदि(!fine_statusline) {
        /* If user has set option HTTP200ALIASES,
           compare header line against list of aliases */
        statusline check = checkhttpprefix(data, hd, hdlen);
        यदि(check == STATUS_DONE) {
          fine_statusline = TRUE;
          k->httpcode = 200;
          k->httpversion = 10;
        }
      }
    }
    अन्यथा यदि(data->conn->scheme->protocol & CURLPROTO_RTSP) {
      ध्रुव वर्ण *p = hd;
      संरचना Curl_str ver;
      curl_off_t status;
      /* we set the max string a little excessive to forgive some leading
         spaces */
      यदि(!curlx_str_until(&p, &ver, 32, ' ') &&
         !curlx_str_single(&p, ' ') &&
         !curlx_str_संख्या(&p, &status, 999)) {
        curlx_str_trimblanks(&ver);
        यदि(curlx_str_cmp(&ver, "RTSP/1.0")) {
          k->httpcode = (पूर्णाङ्क)status;
          fine_statusline = TRUE;
          k->httpversion = 11; /* RTSP acts like HTTP 1.1 */
        }
      }
      यदि(!fine_statusline)
        प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
    }

    यदि(fine_statusline) {
      result = http_statusline(data, data->conn);
      यदि(result)
        प्रत्यागम result;
      writetype |= CLIENTWRITE_STATUS;
    }
    अन्यथा {
      k->header = FALSE;   /* this is not a header line */
      प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
    }
  }

  result = Curl_verify_header(data, hd, hdlen);
  यदि(result)
    प्रत्यागम result;

  result = http_header(data, hd, hdlen);
  यदि(result)
    प्रत्यागम result;

  /*
   * Taken in one (more) header. Write it to the client.
   */
  Curl_debug(data, CURLINFO_HEADER_IN, hd, hdlen);

  यदि(k->httpcode / 100 == 1)
    writetype |= CLIENTWRITE_1XX;
  result = Curl_client_write(data, writetype, hd, hdlen);
  यदि(result)
    प्रत्यागम result;

  result = Curl_bump_headersize(data, hdlen, FALSE);
  यदि(result)
    प्रत्यागम result;

  प्रत्यागम CURLE_OK;
}

/* remove trailing CRLF then all trailing whitespace */
शून्य Curl_http_to_fold(संरचना dynbuf *bf)
{
  माप_प्रकार len = curlx_dyn_len(bf);
  ध्रुव वर्ण *hd = curlx_dyn_ptr(bf);
  यदि(len && (hd[len - 1] == '\n'))
    len--;
  यदि(len && (hd[len - 1] == '\r'))
    len--;
  यावत्(len && ISBLANK(hd[len - 1])) /* strip off trailing whitespace */
    len--;
  curlx_dyn_setlen(bf, len);
}

स्थिर शून्य unfold_header(संरचना Curl_easy *data)
{
  Curl_http_to_fold(&data->state.headerb);
  data->state.leading_unfold = TRUE;
}

/*
 * Read any HTTP header lines from the server and pass them to the client app.
 */
स्थिर CURLcode http_parse_headers(संरचना Curl_easy *data,
                                   ध्रुव वर्ण *buf, माप_प्रकार blen,
                                   माप_प्रकार *pconsumed)
{
  संरचना connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;
  संरचना SingleRequest *k = &data->req;
  ध्रुव वर्ण *end_ptr;
  bool leftover_body = FALSE;

  /* we have bytes for the next header, make sure it is not a folded header
     before passing it on */
  यदि(data->state.maybe_folded && blen) {
    यदि(ISBLANK(buf[0])) {
      /* folded, remove the trailing newlines and append the next header */
      unfold_header(data);
    }
    अन्यथा {
      /* the header data we hold is a complete header, pass it on */
      माप_प्रकार ignore_this;
      result = http_rw_hd(data, curlx_dyn_ptr(&data->state.headerb),
                          curlx_dyn_len(&data->state.headerb),
                          रिक्त, 0, &ignore_this);
      curlx_dyn_reset(&data->state.headerb);
      यदि(result)
        प्रत्यागम result;
    }
    data->state.maybe_folded = FALSE;
  }

  /* header line within buffer loop */
  *pconsumed = 0;
  यावत्(blen && k->header) {
    माप_प्रकार consumed;
    माप_प्रकार hlen;
    ध्रुव वर्ण *hd;
    माप_प्रकार unfold_len = 0;

    यदि(data->state.leading_unfold) {
      /* immediately after an unfold, keep only a single whitespace */
      यावत्(blen && ISBLANK(buf[0])) {
        buf++;
        blen--;
        unfold_len++;
      }
      यदि(blen) {
        /* insert a single space */
        result = curlx_dyn_addn(&data->state.headerb, " ", 1);
        यदि(result)
          प्रत्यागम result;
        data->state.leading_unfold = FALSE; /* done now */
      }
    }

    end_ptr = स_प्रथम(buf, '\n', blen);
    यदि(!end_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      result = curlx_dyn_addn(&data->state.headerb, buf, blen);
      यदि(result)
        प्रत्यागम result;
      *pconsumed += blen + unfold_len;

      यदि(!k->headerline) {
        /* check if this looks like a protocol header */
        statusline st =
          checkprotoprefix(data, conn,
                           curlx_dyn_ptr(&data->state.headerb),
                           curlx_dyn_len(&data->state.headerb));

        यदि(st == STATUS_BAD) {
          /* this is not the beginning of a protocol first header line.
           * Cannot be 0.9 if version was detected or connection was reused. */
          k->header = FALSE;
          streamclose(conn);
          यदि((k->httpversion >= 10) || conn->bits.reuse) {
            failf(data, "Invalid status line");
            प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
          }
          यदि(!data->state.http_neg.accept_09) {
            failf(data, "Received HTTP/0.9 when not allowed");
            प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
          }
          leftover_body = TRUE;
          गच्छ out;
        }
      }
      गच्छ out; /* read more and try again */
    }

    /* the size of the remaining header line */
    consumed = (end_ptr - buf) + 1;

    result = curlx_dyn_addn(&data->state.headerb, buf, consumed);
    यदि(result)
      प्रत्यागम result;
    blen -= consumed;
    buf += consumed;
    *pconsumed += consumed + unfold_len;

    /****
     * We now have a FULL header line in 'headerb'.
     *****/

    hlen = curlx_dyn_len(&data->state.headerb);
    hd = curlx_dyn_ptr(&data->state.headerb);

    यदि(!k->headerline) {
      /* the first read "header", the status line */
      statusline st = checkprotoprefix(data, conn, hd, hlen);
      यदि(st == STATUS_BAD) {
        streamclose(conn);
        /* this is not the beginning of a protocol first header line.
         * Cannot be 0.9 if version was detected or connection was reused. */
        यदि((k->httpversion >= 10) || conn->bits.reuse) {
          failf(data, "Invalid status line");
          प्रत्यागम CURLE_WEIRD_SERVER_REPLY;
        }
        यदि(!data->state.http_neg.accept_09) {
          failf(data, "Received HTTP/0.9 when not allowed");
          प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
        }
        k->header = FALSE;
        leftover_body = TRUE;
        गच्छ out;
      }
    }
    अन्यथा {
      यदि(hlen && !ISNEWLINE(hd[0])) {
        /* this is NOT the header separator */

        /* if we have bytes for the next header, check for folding */
        यदि(blen && ISBLANK(buf[0])) {
          /* remove the trailing CRLF and append the next header */
          unfold_header(data);
          अनुवर्त;
        }
        अन्यथा यदि(!blen) {
          /* this might be a folded header so deal with it in next invoke */
          data->state.maybe_folded = TRUE;
          भङ्ग;
        }
      }
    }

    result = http_rw_hd(data, hd, hlen, buf, blen, &consumed);
    /* We are done with this line. We reset because response
     * processing might switch to HTTP/2 and that might call us
     * directly again. */
    curlx_dyn_reset(&data->state.headerb);
    यदि(consumed) {
      blen -= consumed;
      buf += consumed;
      *pconsumed += consumed;
    }
    यदि(result)
      प्रत्यागम result;
  }

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */
out:
  यदि(!k->header && !leftover_body) {
    curlx_dyn_free(&data->state.headerb);
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_http_write_resp_hd(संरचना Curl_easy *data,
                                 ध्रुव वर्ण *hd, माप_प्रकार hdlen,
                                 bool is_eos)
{
  CURLcode result;
  माप_प्रकार consumed;
  वर्ण tmp = 0;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  result = http_rw_hd(data, hd, hdlen, &tmp, 0, &consumed);
  यदि(!result && is_eos) {
    result = Curl_client_write(data, (CLIENTWRITE_BODY | CLIENTWRITE_EOS),
                               &tmp, 0);
  }
  प्रत्यागम result;
}

/*
 * HTTP protocol `write_resp` implementation. Parse headers
 * when not done yet and otherwise return without consuming data.
 */
CURLcode Curl_http_write_resp_hds(संरचना Curl_easy *data,
                                  ध्रुव वर्ण *buf, माप_प्रकार blen,
                                  माप_प्रकार *pconsumed)
{
  यदि(!data->req.header) {
    *pconsumed = 0;
    प्रत्यागम CURLE_OK;
  }
  अन्यथा {
    CURLcode result;

    result = http_parse_headers(data, buf, blen, pconsumed);
    यदि(!result && !data->req.header) {
      यदि(!data->req.no_body && curlx_dyn_len(&data->state.headerb)) {
        /* leftover from parsing something that turned out not
         * to be a header, only happens if we allow for
         * HTTP/0.9 like responses */
        result = Curl_client_write(data, CLIENTWRITE_BODY,
                                   curlx_dyn_ptr(&data->state.headerb),
                                   curlx_dyn_len(&data->state.headerb));
      }
      curlx_dyn_free(&data->state.headerb);
    }
    प्रत्यागम result;
  }
}

CURLcode Curl_http_write_resp(संरचना Curl_easy *data,
                              ध्रुव वर्ण *buf, माप_प्रकार blen,
                              bool is_eos)
{
  CURLcode result;
  माप_प्रकार consumed;
  पूर्णाङ्क flags;

  result = Curl_http_write_resp_hds(data, buf, blen, &consumed);
  यदि(result || data->req.done)
    गच्छ out;

  DEBUGASSERT(consumed <= blen);
  blen -= consumed;
  buf += consumed;
  /* either all was consumed in header parsing, or we have data left
   * and are done with headers, e.g. it is BODY data */
  DEBUGASSERT(!blen || !data->req.header);
  यदि(!data->req.header && (blen || is_eos)) {
    /* BODY data after header been parsed, write and consume */
    flags = CLIENTWRITE_BODY;
    यदि(is_eos)
      flags |= CLIENTWRITE_EOS;
    result = Curl_client_write(data, flags, buf, blen);
  }
out:
  प्रत्यागम result;
}

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_status(int32_t *pstatus, ध्रुव वर्ण *s, माप_प्रकार len)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  int32_t status = 0;
  पूर्णाङ्क i;

  यदि(len != 3)
    गच्छ out;

  प्रति(i = 0; i < 3; ++i) {
    वर्ण c = s[i];

    यदि(c < '0' || c > '9')
      गच्छ out;

    status *= 10;
    status += c - '0';
  }
  result = CURLE_OK;
out:
  *pstatus = result ? -1 : status;
  प्रत्यागम result;
}

CURLcode Curl_http_req_make(संरचना httpreq **preq,
                            ध्रुव वर्ण *method, माप_प्रकार m_len,
                            ध्रुव वर्ण *scheme, माप_प्रकार s_len,
                            ध्रुव वर्ण *authority, माप_प्रकार a_len,
                            ध्रुव वर्ण *path, माप_प्रकार p_len)
{
  संरचना httpreq *req;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, परिमाण(*req) + m_len);
  यदि(!req)
    गच्छ out;
#यदि defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic धकेलो
/* error: 'memcpy' offset [137, 142] from the object at 'req' is out of
   the bounds of referenced subobject 'method' with type 'char[1]' at
   offset 136 */
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
  स_नकल(req->method, method, m_len);
#यदि defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic निकालो
#endif
  यदि(scheme) {
    req->scheme = curlx_memdup0(scheme, s_len);
    यदि(!req->scheme)
      गच्छ out;
  }
  यदि(authority) {
    req->authority = curlx_memdup0(authority, a_len);
    यदि(!req->authority)
      गच्छ out;
  }
  यदि(path) {
    req->path = curlx_memdup0(path, p_len);
    यदि(!req->path)
      गच्छ out;
  }
  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  यदि(result && req)
    Curl_http_req_free(req);
  *preq = result ? रिक्त : req;
  प्रत्यागम result;
}

स्थिर CURLcode req_assign_url_authority(संरचना httpreq *req, CURLU *url)
{
  वर्ण *host, *port;
  संरचना dynbuf buf;
  CURLUcode uc;
  CURLcode result = CURLE_URL_MALFORMAT;

  host = port = रिक्त;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_HOST, &host, 0);
  यदि(uc && uc != CURLUE_NO_HOST)
    गच्छ out;
  यदि(!host) {
    req->authority = रिक्त;
    result = CURLE_OK;
    गच्छ out;
  }

  uc = curl_url_get(url, CURLUPART_PORT, &port, CURLU_NO_DEFAULT_PORT);
  यदि(uc && uc != CURLUE_NO_PORT)
    गच्छ out;

  result = curlx_dyn_add(&buf, host);
  यदि(result)
    गच्छ out;
  यदि(port) {
    result = curlx_dyn_addf(&buf, ":%s", port);
    यदि(result)
      गच्छ out;
  }
  req->authority = curlx_dyn_ptr(&buf);
out:
  curlx_free(host);
  curlx_free(port);
  यदि(result)
    curlx_dyn_free(&buf);
  प्रत्यागम result;
}

स्थिर CURLcode req_assign_url_path(संरचना httpreq *req, CURLU *url)
{
  वर्ण *path, *query;
  संरचना dynbuf buf;
  CURLUcode uc;
  CURLcode result = CURLE_URL_MALFORMAT;

  path = query = रिक्त;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_PATH, &path, 0);
  यदि(uc)
    गच्छ out;
  uc = curl_url_get(url, CURLUPART_QUERY, &query, 0);
  यदि(uc && uc != CURLUE_NO_QUERY)
    गच्छ out;

  यदि(!query) {
    req->path = path;
    path = रिक्त;
  }
  अन्यथा {
    result = curlx_dyn_add(&buf, path);
    यदि(!result)
      result = curlx_dyn_addf(&buf, "?%s", query);
    यदि(result)
      गच्छ out;
    req->path = curlx_dyn_ptr(&buf);
  }
  result = CURLE_OK;

out:
  curlx_free(path);
  curlx_free(query);
  यदि(result)
    curlx_dyn_free(&buf);
  प्रत्यागम result;
}

CURLcode Curl_http_req_make2(संरचना httpreq **preq,
                             ध्रुव वर्ण *method, माप_प्रकार m_len,
                             CURLU *url, ध्रुव वर्ण *scheme_default)
{
  संरचना httpreq *req;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  CURLUcode uc;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, परिमाण(*req) + m_len);
  यदि(!req)
    गच्छ out;
  स_नकल(req->method, method, m_len);

  uc = curl_url_get(url, CURLUPART_SCHEME, &req->scheme, 0);
  यदि(uc && uc != CURLUE_NO_SCHEME)
    गच्छ out;
  यदि(!req->scheme && scheme_default) {
    req->scheme = curlx_strdup(scheme_default);
    यदि(!req->scheme)
      गच्छ out;
  }

  result = req_assign_url_authority(req, url);
  यदि(result)
    गच्छ out;
  result = req_assign_url_path(req, url);
  यदि(result)
    गच्छ out;

  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  यदि(result && req)
    Curl_http_req_free(req);
  *preq = result ? रिक्त : req;
  प्रत्यागम result;
}

शून्य Curl_http_req_free(संरचना httpreq *req)
{
  यदि(req) {
    curlx_free(req->scheme);
    curlx_free(req->authority);
    curlx_free(req->path);
    Curl_dynhds_free(&req->headers);
    Curl_dynhds_free(&req->trailers);
    curlx_free(req);
  }
}

संरचना name_const {
  ध्रुव वर्ण *name;
  माप_प्रकार namelen;
};

स्थिर ध्रुव संरचना name_const H2_NON_FIELD[] = {
  { STRCONST("Host") },
  { STRCONST("Upgrade") },
  { STRCONST("Connection") },
  { STRCONST("Keep-Alive") },
  { STRCONST("Proxy-Connection") },
  { STRCONST("Transfer-Encoding") },
};

स्थिर bool h2_permissible_field(संरचना dynhds_entry *e)
{
  माप_प्रकार i;
  प्रति(i = 0; i < CURL_ARRAYSIZE(H2_NON_FIELD); ++i) {
    यदि(e->namelen == H2_NON_FIELD[i].namelen &&
       curl_strnequal(H2_NON_FIELD[i].name, e->name, e->namelen))
      प्रत्यागम FALSE;
  }
  प्रत्यागम TRUE;
}

स्थिर bool http_TE_has_token(ध्रुव वर्ण *fvalue, ध्रुव वर्ण *token)
{
  यावत्(*fvalue) {
    संरचना Curl_str name;

    /* skip to first token */
    यावत्(ISBLANK(*fvalue) || *fvalue == ',')
      fvalue++;
    यदि(curlx_str_cspn(&fvalue, &name, " \t\r;,"))
      प्रत्यागम FALSE;
    यदि(curlx_str_casecompare(&name, token))
      प्रत्यागम TRUE;

    /* skip any remainder after token, e.g. parameters with quoted strings */
    यावत्(*fvalue && *fvalue != ',') {
      यदि(*fvalue == '"') {
        संरचना Curl_str qw;
        /* if we do not cleanly find a quoted word here, the header value
         * does not follow HTTP syntax and we reject */
        यदि(curlx_str_quotedword(&fvalue, &qw, CURL_MAX_HTTP_HEADER))
          प्रत्यागम FALSE;
      }
      अन्यथा
        fvalue++;
    }
  }
  प्रत्यागम FALSE;
}

CURLcode Curl_http_req_to_h2(संरचना dynhds *h2_headers,
                             संरचना httpreq *req, संरचना Curl_easy *data)
{
  ध्रुव वर्ण *scheme = रिक्त, *authority = रिक्त;
  संरचना dynhds_entry *e;
  माप_प्रकार i;
  CURLcode result;

  DEBUGASSERT(req);
  DEBUGASSERT(h2_headers);

  यदि(req->scheme) {
    scheme = req->scheme;
  }
  अन्यथा यदि(म_भेद("CONNECT", req->method)) {
    scheme = Curl_checkheaders(data, STRCONST(HTTP_PSEUDO_SCHEME));
    यदि(scheme) {
      scheme += परिमाण(HTTP_PSEUDO_SCHEME);
      curlx_str_passblanks(&scheme);
      infof(data, "set pseudo header %s to %s", HTTP_PSEUDO_SCHEME, scheme);
    }
    अन्यथा {
      scheme = data->state.origin->scheme->name;
    }
  }

  यदि(req->authority) {
    authority = req->authority;
  }
  अन्यथा {
    e = Curl_dynhds_get(&req->headers, STRCONST("Host"));
    यदि(e)
      authority = e->मूल्य;
  }

  Curl_dynhds_reset(h2_headers);
  Curl_dynhds_set_opts(h2_headers, DYNHDS_OPT_LOWERCASE);
  result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_METHOD),
                           req->method, म_माप(req->method));
  यदि(!result && scheme) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_SCHEME),
                             scheme, म_माप(scheme));
  }
  यदि(!result && authority) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_AUTHORITY),
                             authority, म_माप(authority));
  }
  यदि(!result && req->path) {
    result = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_PATH),
                             req->path, म_माप(req->path));
  }
  प्रति(i = 0; !result && i < Curl_dynhds_गिनती(&req->headers); ++i) {
    e = Curl_dynhds_getn(&req->headers, i);
    /* "TE" is special in that it is only permissible when it
     * has only value "trailers". RFC 9113 ch. 8.2.2 */
    यदि(e->namelen == 2 && curl_strequal("TE", e->name)) {
      यदि(http_TE_has_token(e->मूल्य, "trailers"))
        result = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                                 "trailers", CURL_CSTRLEN("trailers"));
    }
    अन्यथा यदि(h2_permissible_field(e)) {
      result = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                               e->मूल्य, e->valuelen);
    }
  }

  प्रत्यागम result;
}

CURLcode Curl_http_resp_make(संरचना http_resp **presp,
                             पूर्णाङ्क status,
                             ध्रुव वर्ण *description)
{
  संरचना http_resp *resp;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  resp = curlx_calloc(1, परिमाण(*resp));
  यदि(!resp)
    गच्छ out;

  resp->status = status;
  यदि(description) {
    resp->description = curlx_strdup(description);
    यदि(!resp->description)
      गच्छ out;
  }
  Curl_dynhds_init(&resp->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&resp->trailers, 0, DYN_HTTP_REQUEST);
  result = CURLE_OK;

out:
  यदि(result && resp)
    Curl_http_resp_free(resp);
  *presp = result ? रिक्त : resp;
  प्रत्यागम result;
}

शून्य Curl_http_resp_free(संरचना http_resp *resp)
{
  यदि(resp) {
    curlx_free(resp->description);
    Curl_dynhds_free(&resp->headers);
    Curl_dynhds_free(&resp->trailers);
    यदि(resp->prev)
      Curl_http_resp_free(resp->prev);
    curlx_free(resp);
  }
}

/*
 * HTTP handler interface.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_http = {
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
