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

#यदि_अपरिभाषितम् CURL_DISABLE_HTTP

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_SYS_PARAM_H
#अन्तर्भाव <sys/param.h>
#यदि_अन्तः

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

शून्य Curl_http_neg_init(संरचना Curl_easy *दत्त, संरचना http_negotiation *neg)
{
  स्मृतिं_पूरय(neg, 0, परिमाण(*neg));
  neg->accept_09 = दत्त->समुच्चयः.http09_allowed;
  विकल्प(दत्त->समुच्चयः.httpwant) {
  पक्ष CURL_HTTP_संस्करणम्_1_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    neg->only_10 = TRUE;
    भङ्ग;
  पक्ष CURL_HTTP_संस्करणम्_1_1:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    भङ्ग;
  पक्ष CURL_HTTP_संस्करणम्_2_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->h2_upgrade = TRUE;
    भङ्ग;
  पक्ष CURL_HTTP_संस्करणम्_2_TLS:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    भङ्ग;
  पक्ष CURL_HTTP_संस्करणम्_2_प्राक्_KNOWLEDGE:
    neg->wanted = neg->allowed = (CURL_HTTP_V2x);
    दत्त->state.http_neg.h2_प्राक्_knowledge = TRUE;
    भङ्ग;
  पक्ष CURL_HTTP_संस्करणम्_3:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    neg->allowed = neg->wanted;
    भङ्ग;
  पक्ष CURL_HTTP_संस्करणम्_3_ONLY:
    neg->wanted = neg->allowed = (CURL_HTTP_V3x);
    भङ्ग;
  पक्ष CURL_HTTP_संस्करणम्_NONE:
  स्वभाव:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    भङ्ग;
  }
}

CURLcode Curl_http_सज्जता_conn(संरचना Curl_easy *दत्त,
                              संरचना connectdata *conn)
{
  /* allocate the HTTP-specific struct for the Curl_easy, only to survive
     during this request */
  यदि(दत्त->state.http_neg.wanted == CURL_HTTP_V3x) {
    /* only HTTP/3, needs to work */
    CURLcode परिणाम = Curl_conn_may_http3(दत्त, conn);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  प्रत्यागम CURLE_OK;
}

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
/*
 * checkProxyHeaders() checks the linked list of custom proxy headers
 * if proxy headers are not available, then it will lookup into http header
 * link list
 *
 * It takes a connectdata struct as input to see if this is a proxy request or
 * not, as it then might check a different header list. Provide the header
 * prefix without colon!
 */
वर्ण *Curl_checkProxyheaders(संरचना Curl_easy *दत्त,
                             ध्रुव संरचना connectdata *conn,
                             ध्रुव वर्ण *thisheader,
                             ध्रुव size_t thislen)
{
  संरचना curl_slist *शीर्ष;

  प्रति(शीर्ष = (conn->http_proxy.peer && दत्त->समुच्चयः.sep_शीर्षकाणि) ?
        दत्त->समुच्चयः.proxyheaders : दत्त->समुच्चयः.शीर्षकाणि;
      शीर्ष; शीर्ष = शीर्ष->अग्र) {
    यदि(curl_strnequal(शीर्ष->दत्त, thisheader, thislen) &&
       Curl_headersep(शीर्ष->दत्त[thislen]))
      प्रत्यागम शीर्ष->दत्त;
  }

  प्रत्यागम रिक्त;
}
#यदि_अन्तः

/* If the header has a value, this function returns TRUE and the value is in
   'outp' with blanks trimmed off. */
स्थिर bool शीर्षकम्_has_मानम्(ध्रुव वर्ण **headerp, संरचना Curl_str *outp)
{
  bool मानम् = !curlx_str_cspn(headerp, outp, ";:") &&
    (!curlx_str_single(headerp, ':') || !curlx_str_single(headerp, ';'));

  यदि(मानम्) {
    curlx_str_cspn(headerp, outp, "\r\n");
    curlx_str_trimblanks(outp);
  }
  प्रत्यागम मानम्;
}

स्थिर bool http_शीर्षकम्_is_रिक्तानि(ध्रुव वर्ण *शीर्षकम्)
{
  संरचना Curl_str बहिर्गमनम्;

  यदि(शीर्षकम्_has_मानम्(&शीर्षकम्, &बहिर्गमनम्)) {
    प्रत्यागम curlx_strlen(&बहिर्गमनम्) == 0;
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
स्थिर CURLcode copy_custom_मानम्(ध्रुव वर्ण *शीर्षकम्, वर्ण **valp)
{
  संरचना Curl_str बहिर्गमनम् = { 0 };

  /* find the end of the header name */
  यदि(शीर्षकम्_has_मानम्(&शीर्षकम्, &बहिर्गमनम्)) {
    *valp = curlx_memdup0(curlx_str(&बहिर्गमनम्), curlx_strlen(&बहिर्गमनम्));
    यदि(*valp)
      प्रत्यागम CURLE_OK;
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  /* bad input */
  *valp = रिक्त;
  प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
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
वर्ण *Curl_copy_शीर्षकम्_मानम्(ध्रुव वर्ण *शीर्षकम्)
{
  संरचना Curl_str बहिर्गमनम्;

  /* find the end of the header name */
  यदि(!curlx_str_until(&शीर्षकम्, &बहिर्गमनम्, MAX_HTTP_RESP_शीर्षकम्_आकार, ':') &&
     !curlx_str_single(&शीर्षकम्, ':')) {
    curlx_str_untilnl(&शीर्षकम्, &बहिर्गमनम्, MAX_HTTP_RESP_शीर्षकम्_आकार);
    curlx_str_trimblanks(&बहिर्गमनम्);
    प्रत्यागम curlx_memdup0(curlx_str(&बहिर्गमनम्), curlx_strlen(&बहिर्गमनम्));
  }
  /* bad input, should never happen */
  DEBUGASSERT(0);
  प्रत्यागम रिक्त;
}

#यदि_अपरिभाषितम् CURL_DISABLE_HTTP_AUTH

#यदि_अपरिभाषितम् CURL_DISABLE_BASIC_AUTH
/*
 * http_output_basic() sets up an Authorization: header (or the proxy version)
 * for HTTP Basic authentication.
 *
 * Returns CURLcode.
 */
स्थिर CURLcode http_output_basic(संरचना Curl_easy *दत्त,
                                  संरचना connectdata *conn, bool proxy)
{
  size_t आकार = 0;
  वर्ण *authorization = रिक्त;
  वर्ण **p_hd;
  CURLcode परिणाम;
  संरचना Curl_creds *creds = रिक्त;
  वर्ण *बहिर्गमनम्;

  /* credentials are unique per transfer for HTTP, do not use the ones for the
     connection */
  यदि(proxy) {
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
    p_hd = &दत्त->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
#अन्यथा
    (शून्य)conn;
    प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
  }
  अन्यथा {
    p_hd = &दत्त->req.hd_auth;
    creds = दत्त->state.creds;
  }

  यदि(!creds) {
    DEBUGASSERT(0);
    प्रत्यागम CURLE_FAILED_INIT;
  }

  बहिर्गमनम् = curl_maprintf("%s:%s", creds->user, creds->passwd);
  यदि(!बहिर्गमनम्)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  परिणाम = curlx_base64_encode((uint8_t *)बहिर्गमनम्, वाक्यदैर्घ्यम्(बहिर्गमनम्),
                               &authorization, &आकार);
  यदि(परिणाम)
    गच्छ fail;

  यदि(!authorization) {
    परिणाम = CURLE_REMOTE_ACCESS_DENIED;
    गच्छ fail;
  }

  curlx_free(*p_hd);
  *p_hd = curl_maprintf("%sAuthorization: Basic %s\r\n",
                        proxy ? "Proxy-" : "",
                        authorization);
  curlx_free(authorization);
  यदि(!*p_hd) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ fail;
  }

fail:
  curlx_free(बहिर्गमनम्);
  प्रत्यागम परिणाम;
}

#यदि_अन्तः

#यदि_अपरिभाषितम् CURL_DISABLE_BEARER_AUTH
/*
 * http_output_bearer() sets up an Authorization: header
 * for HTTP Bearer authentication.
 *
 * Returns CURLcode.
 */
स्थिर CURLcode http_output_bearer(संरचना Curl_easy *दत्त)
{
  वर्ण **userp;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(Curl_creds_has_oauth_bearer(दत्त->state.creds));
  userp = &दत्त->req.hd_auth;
  curlx_free(*userp);
  *userp = curl_maprintf("Authorization: Bearer %s\r\n",
                         Curl_creds_oauth_bearer(दत्त->state.creds));

  यदि(!*userp) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ fail;
  }

fail:
  प्रत्यागम परिणाम;
}
#यदि_अन्तः

#यदि_अन्तः

/* pickoneauth() selects the most favorable authentication method from the
 * ones available and the ones we want.
 *
 * return TRUE if one was picked
 */
स्थिर bool pickoneauth(संरचना auth *pick, अचिह्नित दीर्घ आवरणम्,
                        संरचना Curl_creds *creds)
{
  bool have_user_pass = Curl_creds_has_user_or_pass(creds);
  bool picked;
  /* only deal with authentication we want */
  अचिह्नित दीर्घ avail = pick->avail & pick->want & आवरणम्;
  picked = TRUE;

  /* The order of these checks is highly relevant, as this will be the order
     of preference in case of the existence of multiple accepted types. */
  यदि(avail & CURLAUTH_NEGOTIATE)  /* available on empty creds */
    pick->picked = CURLAUTH_NEGOTIATE;
#यदि_अपरिभाषितम् CURL_DISABLE_BEARER_AUTH
  अन्यथा यदि((avail & CURLAUTH_BEARER) && Curl_creds_has_oauth_bearer(creds))
    pick->picked = CURLAUTH_BEARER;
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH
  अन्यथा यदि((avail & CURLAUTH_DIGEST) && have_user_pass)
    pick->picked = CURLAUTH_DIGEST;
#यदि_अन्तः
  अन्यथा यदि(avail & CURLAUTH_NTLM)
    pick->picked = CURLAUTH_NTLM;
#यदि_अपरिभाषितम् CURL_DISABLE_BASIC_AUTH
  अन्यथा यदि((avail & CURLAUTH_BASIC) && have_user_pass)
    pick->picked = CURLAUTH_BASIC;
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_AWS
  अन्यथा यदि(avail & CURLAUTH_AWS_SIGV4)
    pick->picked = CURLAUTH_AWS_SIGV4;
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_HTTPSIG
  अन्यथा यदि(avail & CURLAUTH_HTTPSIG)
    pick->picked = CURLAUTH_HTTPSIG;
#यदि_अन्तः
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
स्थिर CURLcode http_perhapsrewind(संरचना Curl_easy *दत्त,
                                   संरचना connectdata *conn)
{
  curl_off_t bytessent = दत्त->req.writebytecount;
  curl_off_t expectsend = Curl_creader_योग_दीर्घता(दत्त);
  curl_off_t upload_remain = (expectsend >= 0) ? (expectsend - bytessent) : -1;
  bool little_upload_remains = (upload_remain >= 0 && upload_remain < 2000);
  bool needs_rewind = Curl_creader_needs_rewind(दत्त);
  /* By default, we would like to abort the transfer when little or unknown
   * amount remains. This may be overridden by authentications further
   * below! */
  bool abort_upload = (!दत्त->req.upload_समाप्तम् && !little_upload_remains);
  VERBOSE(ध्रुव वर्ण *ongoing_auth = रिक्त);

  /* We need a rewind before uploading client read data again. The
   * checks below influence of the upload is to be continued
   * or aborted early.
   * This depends on how much remains to be sent and in what state
   * the authentication is. Some auth schemes such as NTLM do not work
   * for a new connection. */
  यदि(needs_rewind) {
    infof(दत्त, "Need to rewind upload for next request");
    Curl_creader_समुच्चयः_rewind(दत्त, TRUE);
  }

  यदि(conn->द्वयङ्काः.close)
    /* If we already decided to close this connection, we cannot veto. */
    प्रत्यागम CURLE_OK;

  यदि(abort_upload) {
    /* We would like to abort the upload - but should we? */
#यदि_परिभाषितम् USE_NTLM
    यदि((दत्त->state.authproxy.picked == CURLAUTH_NTLM) ||
       (दत्त->state.authhost.picked == CURLAUTH_NTLM)) {
      VERBOSE(ongoing_auth = "NTLM");
      यदि((conn->http_ntlm_state != NTLMSTATE_NONE) ||
         (conn->proxy_ntlm_state != NTLMSTATE_NONE)) {
        /* The NTLM-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#यदि_अन्तः
#यदि_परिभाषितम् USE_SPNEGO
    /* There is still data left to send */
    यदि((दत्त->state.authproxy.picked == CURLAUTH_NEGOTIATE) ||
       (दत्त->state.authhost.picked == CURLAUTH_NEGOTIATE)) {
      VERBOSE(ongoing_auth = "NEGOTIATE");
      यदि((conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) ||
         (conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE)) {
        /* The NEGOTIATE-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#यदि_अन्तः
  }

  यदि(abort_upload) {
    यदि(upload_remain >= 0)
      infof(दत्त, "%s%sclose instead of sending %" FMT_OFF_T " more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "",
            upload_remain);
    अन्यथा
      infof(दत्त, "%s%sclose instead of sending unknown amount "
            "of more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "");
    /* We decided to abort the ongoing transfer */
    streamclose(conn);
    दत्त->req.आकार = 0; /* do not download any more than 0 bytes */
    दत्त->req.http_bodyless = TRUE;
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
स्थिर bool http_should_fail(संरचना Curl_easy *दत्त, पूर्णाङ्क httpcode)
{
  DEBUGASSERT(दत्त);
  DEBUGASSERT(दत्त->conn);

  /*
   * If we have not been asked to fail on error,
   * do not fail.
   */
  यदि(!दत्त->समुच्चयः.http_fail_on_error)
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
  यदि(दत्त->state.resume_from && दत्त->state.httpreq == HTTPREQ_GET &&
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
  यदि((httpcode == 401) && !दत्त->state.creds)
    प्रत्यागम TRUE;
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  यदि((httpcode == 407) && !दत्त->conn->http_proxy.creds)
    प्रत्यागम TRUE;
#यदि_अन्तः

  प्रत्यागम (bool)दत्त->state.authproblem;
}

/*
 * Curl_http_auth_act() gets called when all HTTP headers have been received
 * and it checks what authentication methods that are available and decides
 * which one (if any) to use. It will set 'newurl' if an auth method was
 * picked.
 */
CURLcode Curl_http_auth_act(संरचना Curl_easy *दत्त)
{
  संरचना connectdata *conn = दत्त->conn;
  bool pickhost = FALSE;
  bool pickproxy = FALSE;
  CURLcode परिणाम = CURLE_OK;
  अचिह्नित दीर्घ authmask = ~0UL;

  यदि(!Curl_creds_has_oauth_bearer(दत्त->state.creds))
    authmask &= (अचिह्नित दीर्घ)~CURLAUTH_BEARER;

  यदि(100 <= दत्त->req.httpcode && दत्त->req.httpcode <= 199)
    /* this is a transient response code, ignore */
    प्रत्यागम CURLE_OK;

  यदि(दत्त->state.authproblem)
    प्रत्यागम दत्त->समुच्चयः.http_fail_on_error ? CURLE_HTTP_RETURNED_ERROR : CURLE_OK;

  यदि(दत्त->state.creds &&
     ((दत्त->req.httpcode == 401) ||
      (दत्त->req.authneg && दत्त->req.httpcode < 300))) {
    pickhost = pickoneauth(&दत्त->state.authhost, authmask, दत्त->state.creds);
    यदि(!pickhost)
      दत्त->state.authproblem = TRUE;
    अन्यथा
      दत्त->info.httpauthpicked = दत्त->state.authhost.picked;
    यदि(दत्त->state.authhost.picked == CURLAUTH_NTLM &&
       (दत्त->req.httpversion_प्रेषितम् > 11)) {
      infof(दत्त, "Forcing HTTP/1.1 for NTLM");
      connclose(conn);
      दत्त->state.http_neg.wanted = CURL_HTTP_V1x;
      दत्त->state.http_neg.allowed = CURL_HTTP_V1x;
    }
  }
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  यदि(conn->http_proxy.creds &&
     ((दत्त->req.httpcode == 407) ||
      (दत्त->req.authneg && दत्त->req.httpcode < 300))) {
    pickproxy = pickoneauth(&दत्त->state.authproxy,
                            authmask & ~CURLAUTH_BEARER,
                            conn->http_proxy.creds);
    यदि(!pickproxy)
      दत्त->state.authproblem = TRUE;
    अन्यथा
      दत्त->info.proxyauthpicked = दत्त->state.authproxy.picked;
  }
#यदि_अन्तः

  यदि(pickhost || pickproxy) {
    परिणाम = http_perhapsrewind(दत्त, conn);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    /* In case this is GSS auth, the newurl field is already allocated so
       we must make sure to free it before allocating a new one. As figured
       out in bug #2284386 */
    curlx_free(दत्त->req.newurl);
    /* clone URL */
    दत्त->req.newurl = Curl_bufref_dup(&दत्त->state.url);
    यदि(!दत्त->req.newurl)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  अन्यथा यदि((दत्त->req.httpcode < 300) &&
          !दत्त->state.authhost.समाप्तम् &&
          दत्त->req.authneg &&
          /* no (known) authentication available,
             authentication is not "done" yet and
             no authentication seems to be required and
             we did not try HEAD or GET */
          (दत्त->state.httpreq != HTTPREQ_GET) &&
          (दत्त->state.httpreq != HTTPREQ_शीर्ष)) {
    /* clone URL */
    दत्त->req.newurl = Curl_bufref_dup(&दत्त->state.url);
    यदि(!दत्त->req.newurl)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    दत्त->state.authhost.समाप्तम् = TRUE;
  }
  यदि(http_should_fail(दत्त, दत्त->req.httpcode)) {
    failf(दत्त, "The requested URL returned error: %d",
          दत्त->req.httpcode);
    परिणाम = CURLE_HTTP_RETURNED_ERROR;
  }

  प्रत्यागम परिणाम;
}

#यदि_अपरिभाषितम् CURL_DISABLE_HTTP_AUTH
/*
 * Output the correct authentication header depending on the auth type
 * and whether or not it is to a proxy.
 */
स्थिर CURLcode output_auth_शीर्षकाणि(संरचना Curl_easy *दत्त,
                                    संरचना connectdata *conn,
                                    संरचना auth *authstatus,
                                    ध्रुव वर्ण *request,
                                    ध्रुव वर्ण *मार्गः,
                                    bool proxy)
{
  ध्रुव वर्ण *auth = रिक्त;
  CURLcode परिणाम = CURLE_OK;
  (शून्य)conn;

#यदि_परिभाषितम् CURL_DISABLE_DIGEST_AUTH
  (शून्य)request;
  (शून्य)मार्गः;
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_AWS
  यदि((authstatus->picked == CURLAUTH_AWS_SIGV4) && !proxy) {
    /* this method is never for proxy */
    auth = "AWS_SIGV4";
    परिणाम = Curl_output_aws_sigv4(दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  अन्यथा
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_HTTPSIG
  यदि((authstatus->picked == CURLAUTH_HTTPSIG) && !proxy) {
    /* HTTPSIG uses its own configured key material rather than
       data->state.creds. Do not let unrelated credentials from a
       redirected URL bypass the cross-host auth boundary. */
    यदि(Curl_auth_allowed_गन्तव्यम्_host(दत्त)) {
      auth = "HTTPSIG";
      परिणाम = Curl_output_httpsig(दत्त);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
    अन्यथा
      authstatus->समाप्तम् = TRUE;
  }
  अन्यथा
#यदि_अन्तः
#यदि_परिभाषितम् USE_SPNEGO
  यदि(authstatus->picked == CURLAUTH_NEGOTIATE) {
    यदि(
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
      (proxy && !Curl_checkProxyheaders(दत्त, conn,
                                        STRCONST("Proxy-authorization"))) ||
#यदि_अन्तः
      (!proxy && !Curl_checkheaders(दत्त, STRCONST("Authorization")) &&
       Curl_auth_allowed_गन्तव्यम्_host(दत्त))) {
      auth = "Negotiate";
      परिणाम = Curl_output_negotiate(दत्त, conn, proxy);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
    अन्यथा
      authstatus->समाप्तम् = TRUE;
  }
  अन्यथा
#यदि_अन्तः
#यदि_परिभाषितम् USE_NTLM
  यदि(authstatus->picked == CURLAUTH_NTLM) {
    यदि(proxy || Curl_auth_allowed_गन्तव्यम्_host(दत्त)) {
      auth = "NTLM";
      परिणाम = Curl_output_ntlm(दत्त, proxy);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
    अन्यथा
      authstatus->समाप्तम् = TRUE;
  }
  अन्यथा
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH
  यदि(authstatus->picked == CURLAUTH_DIGEST) {
    auth = "Digest";
    परिणाम = Curl_output_digest(दत्त,
                                proxy,
                                (ध्रुव अचिह्नित वर्ण *)request,
                                (ध्रुव अचिह्नित वर्ण *)मार्गः);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  अन्यथा
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_BASIC_AUTH
  यदि(authstatus->picked == CURLAUTH_BASIC) {
    /* Basic */
    यदि(
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
       (proxy && conn->http_proxy.creds &&
        Curl_creds_has_user_or_pass(conn->http_proxy.creds) &&
        !Curl_checkProxyheaders(दत्त, conn,
                                STRCONST("Proxy-authorization"))) ||
#यदि_अन्तः
       (!proxy && दत्त->state.creds &&
        Curl_creds_has_user_or_pass(दत्त->state.creds) &&
        !Curl_checkheaders(दत्त, STRCONST("Authorization")))) {
      auth = "Basic";
      परिणाम = http_output_basic(दत्त, conn, proxy);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->समाप्तम् = TRUE;
  }
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_BEARER_AUTH
  यदि(authstatus->picked == CURLAUTH_BEARER) {
    /* Bearer */
    यदि(!proxy && Curl_creds_has_oauth_bearer(दत्त->state.creds) &&
       !Curl_checkheaders(दत्त, STRCONST("Authorization"))) {
      auth = "Bearer";
      परिणाम = http_output_bearer(दत्त);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->समाप्तम् = TRUE;
  }
#यदि_अन्तः

  यदि(auth) {
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
    यदि(proxy)
      दत्त->info.proxyauthpicked = authstatus->picked;
    अन्यथा
      दत्त->info.httpauthpicked = authstatus->picked;
    infof(दत्त, "%s auth using %s with user '%s'",
          proxy ? "Proxy" : "Server", auth,
          proxy ? (conn->http_proxy.creds ?
                   conn->http_proxy.creds->user : "") :
          (दत्त->state.creds ?
           दत्त->state.creds->user : ""));
#अन्यथा
    (शून्य)proxy;
    infof(दत्त, "Server auth using %s with user '%s'",
          auth, दत्त->state.creds ?
          दत्त->state.creds->user : "");
#यदि_अन्तः
    authstatus->multipass = !authstatus->समाप्तम्;
  }
  अन्यथा {
    authstatus->multipass = FALSE;
    यदि(proxy)
      दत्त->info.proxyauthpicked = 0;
    अन्यथा
      दत्त->info.httpauthpicked = 0;
  }

  प्रत्यागम परिणाम;
}

CURLcode Curl_http_output_auth(संरचना Curl_easy *दत्त,
                               संरचना connectdata *conn,
                               ध्रुव वर्ण *request,
                               Curl_HttpReq httpreq,
                               ध्रुव वर्ण *मार्गः,
                               ध्रुव वर्ण *query,
                               bool is_connect)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना auth *authhost;
  संरचना auth *authproxy;
  ध्रुव वर्ण *मार्गः_and_query = मार्गः;
  वर्ण *tmp_str = रिक्त;

  DEBUGASSERT(दत्त);
  authhost = &दत्त->state.authhost;
  authproxy = &दत्त->state.authproxy;

  यदि(
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
    (!conn->http_proxy.peer || !conn->http_proxy.creds) &&
#यदि_अन्तः
#यदि_परिभाषितम् USE_SPNEGO
    !(authhost->want & CURLAUTH_NEGOTIATE) &&
    !(authproxy->want & CURLAUTH_NEGOTIATE) &&
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_HTTPSIG
    !(authhost->want & CURLAUTH_HTTPSIG) &&
#यदि_अन्तः
    !दत्त->state.creds) {
    /* no authentication with no user or password */
    authhost->समाप्तम् = TRUE;
    authproxy->समाप्तम् = TRUE;
    परिणाम = CURLE_OK;
    गच्छ बहिर्गमनम्;
  }

  यदि(query) {
    tmp_str = curl_maprintf("%s?%s", मार्गः, query);
    यदि(!tmp_str) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
    मार्गः_and_query = tmp_str;
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

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  /* Send proxy authentication header if needed */
  यदि(conn->द्वयङ्काः.origin_is_proxy || is_connect) {
    परिणाम = output_auth_शीर्षकाणि(दत्त, conn, authproxy, request,
                                 मार्गः_and_query, TRUE);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  अन्यथा
#अन्यथा
  (शून्य)is_connect;
#यदि_अन्तः /* CURL_DISABLE_PROXY */
    /* we have no proxy so let's pretend we are done authenticating
       with it */
    authproxy->समाप्तम् = TRUE;

  /* Either we have credentials for the origin we talk to or
     performing authentication is allowed here */
  यदि(दत्त->state.creds || Curl_auth_allowed_गन्तव्यम्_host(दत्त))
    परिणाम = output_auth_शीर्षकाणि(दत्त, conn, authhost, request,
                                 मार्गः_and_query, FALSE);
  अन्यथा
    authhost->समाप्तम् = TRUE;

  यदि(((authhost->multipass && !authhost->समाप्तम्) ||
      (authproxy->multipass && !authproxy->समाप्तम्)) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_शीर्ष)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    दत्त->req.authneg = TRUE;
  }
  अन्यथा
    दत्त->req.authneg = FALSE;

बहिर्गमनम्:
  curlx_free(tmp_str);
  प्रत्यागम परिणाम;
}

#अन्यथा /* !CURL_DISABLE_HTTP_AUTH */
/* when disabled */
CURLcode Curl_http_output_auth(संरचना Curl_easy *दत्त,
                               संरचना connectdata *conn,
                               ध्रुव वर्ण *request,
                               Curl_HttpReq httpreq,
                               ध्रुव वर्ण *मार्गः,
                               ध्रुव वर्ण *query,
                               bool is_connect)
{
  (शून्य)दत्त;
  (शून्य)conn;
  (शून्य)request;
  (शून्य)httpreq;
  (शून्य)मार्गः;
  (शून्य)query;
  (शून्य)is_connect;
  प्रत्यागम CURLE_OK;
}
#यदि_अन्तः /* !CURL_DISABLE_HTTP_AUTH, else */

#यदि defined(USE_SPNEGO) || defined(USE_NTLM) || \
  !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_BASIC_AUTH) || \
  !defined(CURL_DISABLE_BEARER_AUTH)
स्थिर bool authcmp(ध्रुव वर्ण *auth, ध्रुव वर्ण *पङ्क्तिः)
{
  /* the auth string must not have an alnum following */
  size_t n = वाक्यदैर्घ्यम्(auth);
  प्रत्यागम curl_strnequal(auth, पङ्क्तिः, n) && !ISALNUM(पङ्क्तिः[n]);
}
#यदि_अन्तः

#यदि_परिभाषितम् USE_SPNEGO
स्थिर CURLcode auth_spnego(संरचना Curl_easy *दत्त,
                            bool proxy,
                            ध्रुव वर्ण *auth,
                            संरचना auth *authp,
                            uint32_t *availp)
{
  यदि((authp->avail & CURLAUTH_NEGOTIATE) || Curl_auth_is_spnego_supported()) {
    *availp |= CURLAUTH_NEGOTIATE;
    authp->avail |= CURLAUTH_NEGOTIATE;

    यदि(authp->picked == CURLAUTH_NEGOTIATE) {
      संरचना connectdata *conn = दत्त->conn;
      CURLcode परिणाम = Curl_निवेशः_negotiate(दत्त, conn, proxy, auth);
      uint8_t *negstate = proxy ? &conn->proxy_negotiate_state :
        &conn->http_negotiate_state;
      यदि(!परिणाम) {
        curlx_free(दत्त->req.newurl);
        दत्त->req.newurl = Curl_bufref_dup(&दत्त->state.url);
        यदि(!दत्त->req.newurl)
          प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        दत्त->state.authproblem = FALSE;
        /* we received a GSS auth token and we dealt with it fine */
        *negstate = CURL_NEGO_AUTH_RECV;
      }
      अन्यथा
        दत्त->state.authproblem = TRUE;
    }
  }
  प्रत्यागम CURLE_OK;
}
#यदि_अन्तः

#यदि_परिभाषितम् USE_NTLM
स्थिर CURLcode auth_ntlm(संरचना Curl_easy *दत्त,
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
      CURLcode परिणाम = Curl_निवेशः_ntlm(दत्त, proxy, auth);
      यदि(!परिणाम)
        दत्त->state.authproblem = FALSE;
      अन्यथा {
        यदि(परिणाम == CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः)
          प्रत्यागम परिणाम;
        infof(दत्त, "NTLM authentication problem, ignoring.");
        दत्त->state.authproblem = TRUE;
      }
    }
  }
  प्रत्यागम CURLE_OK;
}
#यदि_अन्तः

#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH
स्थिर CURLcode auth_digest(संरचना Curl_easy *दत्त,
                            bool proxy,
                            ध्रुव वर्ण *auth,
                            संरचना auth *authp,
                            uint32_t *availp)
{
  यदि(authp->avail & CURLAUTH_DIGEST) {
    *availp |= CURLAUTH_DIGEST;
    infof(दत्त, "Ignoring duplicate digest auth header.");
  }
  अन्यथा यदि(Curl_auth_is_digest_supported()) {
    CURLcode परिणाम;

    *availp |= CURLAUTH_DIGEST;
    authp->avail |= CURLAUTH_DIGEST;

    /* We call this function on input Digest headers even if Digest
     * authentication is not activated yet, as we need to store the
     * incoming data from this header in case we are going to use
     * Digest */
    परिणाम = Curl_निवेशः_digest(दत्त, proxy, auth);
    यदि(परिणाम) {
      यदि(परिणाम == CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः)
        प्रत्यागम परिणाम;
      infof(दत्त, "Digest authentication problem, ignoring.");
      दत्त->state.authproblem = TRUE;
    }
  }
  प्रत्यागम CURLE_OK;
}
#यदि_अन्तः

#यदि_अपरिभाषितम् CURL_DISABLE_BASIC_AUTH
स्थिर CURLcode auth_basic(संरचना Curl_easy *दत्त,
                           संरचना auth *authp,
                           uint32_t *availp)
{
  *availp |= CURLAUTH_BASIC;
  authp->avail |= CURLAUTH_BASIC;
  यदि(authp->picked == CURLAUTH_BASIC) {
    /* We asked for Basic authentication but got a 40X back anyway, which
       means our name+password is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(दत्त, "Basic authentication problem, ignoring.");
    दत्त->state.authproblem = TRUE;
  }
  प्रत्यागम CURLE_OK;
}
#यदि_अन्तः

#यदि_अपरिभाषितम् CURL_DISABLE_BEARER_AUTH
स्थिर CURLcode auth_bearer(संरचना Curl_easy *दत्त,
                            संरचना auth *authp,
                            uint32_t *availp)
{
  *availp |= CURLAUTH_BEARER;
  authp->avail |= CURLAUTH_BEARER;
  यदि(authp->picked == CURLAUTH_BEARER) {
    /* We asked for Bearer authentication but got a 40X back anyway, which
       means our token is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(दत्त, "Bearer authentication problem, ignoring.");
    दत्त->state.authproblem = TRUE;
  }
  प्रत्यागम CURLE_OK;
}
#यदि_अन्तः

/*
 * Curl_http_input_auth() deals with Proxy-Authenticate: and WWW-Authenticate:
 * headers. They are dealt with both in the transfer.c main loop and in the
 * proxy CONNECT loop.
 *
 * The 'auth' line ends with a NUL byte without CR or LF present.
 */
CURLcode Curl_http_निवेशः_auth(संरचना Curl_easy *दत्त, bool proxy,
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
  CURLcode परिणाम = CURLE_OK;
  DEBUGASSERT(auth);
  DEBUGASSERT(दत्त);

  यदि(proxy) {
    availp = &दत्त->info.proxyauthavail;
    authp = &दत्त->state.authproxy;
  }
  अन्यथा {
    availp = &दत्त->info.httpauthavail;
    authp = &दत्त->state.authhost;
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
#यदि_परिभाषितम् USE_SPNEGO
    यदि(authcmp("Negotiate", auth))
      परिणाम = auth_spnego(दत्त, proxy, auth, authp, availp);
#यदि_अन्तः
#यदि_परिभाषितम् USE_NTLM
    यदि(!परिणाम && authcmp("NTLM", auth))
      परिणाम = auth_ntlm(दत्त, proxy, auth, authp, availp);
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH
    यदि(!परिणाम && authcmp("Digest", auth))
      परिणाम = auth_digest(दत्त, proxy, auth, authp, availp);
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_BASIC_AUTH
    यदि(!परिणाम && authcmp("Basic", auth))
      परिणाम = auth_basic(दत्त, authp, availp);
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_BEARER_AUTH
    यदि(authcmp("Bearer", auth))
      परिणाम = auth_bearer(दत्त, authp, availp);
#यदि_अन्तः

    यदि(परिणाम)
      भङ्ग;

    /* there may be multiple methods on one line, so keep reading */
    auth = strchr(auth, ',');
    यदि(auth) /* if we are on a comma, skip it */
      auth++;
    अन्यथा
      भङ्ग;
    curlx_str_passblanks(&auth);
  }
  प्रत्यागम परिणाम;
#अन्यथा
  (शून्य)दत्त;
  (शून्य)proxy;
  (शून्य)auth;
  /* nothing to do when disabled */
  प्रत्यागम CURLE_OK;
#यदि_अन्तः
}

स्थिर शून्य http_switch_गन्तव्यम्_get(संरचना Curl_easy *दत्त, पूर्णाङ्क संहिता)
{
  ध्रुव वर्ण *req = CURL_EASY_STR(दत्त, STRING_CUSTOMREQUEST);

  यदि((req || दत्त->state.httpreq != HTTPREQ_GET) &&
     (दत्त->समुच्चयः.http_follow_बहुलकम् == CURLFOLLOW_OBEYCODE)) {
    NOVERBOSE((शून्य)संहिता);
    infof(दत्त, "Switch to GET because of %d response", संहिता);
    दत्त->state.http_ignorecustom = TRUE;
  }
  अन्यथा यदि(req && (दत्त->समुच्चयः.http_follow_बहुलकम् != CURLFOLLOW_FIRSTONLY))
    infof(दत्त, "Stick to %s instead of GET", req);

  दत्त->state.httpreq = HTTPREQ_GET;
  Curl_creader_समुच्चयः_rewind(दत्त, FALSE);
}

#परिभाषय HTTPREQ_IS_POST(दत्त)                    \
  ((दत्त)->state.httpreq == HTTPREQ_POST ||      \
   (दत्त)->state.httpreq == HTTPREQ_POST_FORM || \
   (दत्त)->state.httpreq == HTTPREQ_POST_MIME)

स्थिर CURLcode समुच्चयः_auto_referer(संरचना Curl_easy *दत्त)
{
  CURLU *u;
  वर्ण *referer = रिक्त;
  CURLUcode uc;

  /* We are asked to automatically set the previous URL as the referer
     when we get the next URL. We pick the ->url field, which may or may
     not be 100% correct */
  Curl_bufref_free(&दत्त->state.referer);

  /* Make a copy of the URL without credentials and fragment */
  u = curl_url();
  यदि(!u)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  uc = curl_url_समुच्चयः(u, CURLUPART_URL,
                    Curl_bufref_ptr(&दत्त->state.url), 0);
  यदि(!uc)
    uc = curl_url_समुच्चयः(u, CURLUPART_FRAGMENT, रिक्त, 0);
  यदि(!uc)
    uc = curl_url_समुच्चयः(u, CURLUPART_USER, रिक्त, 0);
  यदि(!uc)
    uc = curl_url_समुच्चयः(u, CURLUPART_PASSWORD, रिक्त, 0);
  यदि(!uc)
    uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

  curl_url_cleanup(u);

  यदि(uc || !referer)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  Curl_bufref_समुच्चयः(&दत्त->state.referer, referer, 0, curl_free);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode follow_parse_url(संरचना Curl_easy *दत्त, ध्रुव वर्ण *newurl,
                                 followtype type, वर्ण **follow_urlp,
                                 CURLUcode *ucp)
{
  CURLUcode uc;

  DEBUGASSERT(दत्त->state.uh);
  uc = curl_url_समुच्चयः(दत्त->state.uh, CURLUPART_URL, newurl, (अचिह्नित पूर्णाङ्क)
                    ((type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                     ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) |
                     CURLU_ALLOW_SPACE |
                     (दत्त->समुच्चयः.मार्गः_as_is ? CURLU_मार्गः_AS_IS : 0)));
  *ucp = uc;
  यदि(uc) {
    यदि((uc == CURLUE_बहिर्गमनम्_सम्बन्धि_स्मृतिः) || (type != FOLLOW_FAKE)) {
      failf(दत्त, "The redirect target URL could not be parsed: %s",
            curl_url_strerror(uc));
      प्रत्यागम Curl_uc_गन्तव्यम्_curlcode(uc);
    }

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, duplicate the field as-is */
    *follow_urlp = curlx_strdup(newurl);
    यदि(!*follow_urlp)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  अन्यथा {
    CURLU *u = curl_url();
    यदि(!u)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    uc = curl_url_समुच्चयः(u, CURLUPART_URL,
                      Curl_bufref_ptr(&दत्त->state.url),
                      CURLU_URLENCODE | CURLU_ALLOW_SPACE);
    यदि(!uc)
      uc = curl_url_get(दत्त->state.uh, CURLUPART_URL, follow_urlp, 0);
    यदि(uc) {
      curl_url_cleanup(u);
      प्रत्यागम Curl_uc_गन्तव्यम्_curlcode(uc);
    }

#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH
    {
      bool same_origin = Curl_url_same_origin(u, दत्त->state.uh);
      curl_url_cleanup(u);
      यदि(!same_origin)
        Curl_auth_digest_cleanup(&दत्त->state.digest);
    }
#अन्यथा
    curl_url_cleanup(u);
#यदि_अन्तः
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode follow_fake(संरचना Curl_easy *दत्त, वर्ण *follow_url,
                            CURLUcode uc, bool reachedmax)
{
  /* we are only figuring out the new URL if we would have followed locations
     but now we are done so we can get out! */
  यदि(!uc) {
    CURLU *u = curl_url();
    वर्ण *nocred = रिक्त;

    यदि(!u) {
      curlx_free(follow_url);
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
    यदि(!curl_url_समुच्चयः(u, CURLUPART_URL, follow_url,
                     CURLU_NON_SUPPORT_SCHEME |
                     (दत्त->समुच्चयः.मार्गः_as_is ? CURLU_मार्गः_AS_IS : 0)) &&
       !curl_url_समुच्चयः(u, CURLUPART_USER, रिक्त, 0) &&
       !curl_url_समुच्चयः(u, CURLUPART_PASSWORD, रिक्त, 0))
      (शून्य)curl_url_get(u, CURLUPART_URL, &nocred, CURLU_GET_रिक्तानि);
    curl_url_cleanup(u);
    यदि(nocred) {
      curlx_free(follow_url);
      follow_url = nocred;
    }
  }
  दत्त->info.wouldredirect = follow_url;

  यदि(reachedmax) {
    failf(दत्त, "Maximum (%d) redirects followed", दत्त->समुच्चयः.maxredirs);
    प्रत्यागम CURLE_TOO_MANY_REDIRECTS;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर bool follow_switch_get(संरचना Curl_easy *दत्त)
{
  /*
   * We get here when the HTTP code is 300-399 and 401 or 407. We need to
   * perform differently based on exactly what return code there was.
   */
  विकल्प(दत्त->info.httpcode) {
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
    यदि(HTTPREQ_IS_POST(दत्त) && !दत्त->समुच्चयः.post301) {
      http_switch_गन्तव्यम्_get(दत्त, 301);
      प्रत्यागम TRUE;
    }
    भङ्ग;
  पक्ष 302: /*प्राप्तम्*/
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
    यदि(HTTPREQ_IS_POST(दत्त) && !दत्त->समुच्चयः.post302) {
      http_switch_गन्तव्यम्_get(दत्त, 302);
      प्रत्यागम TRUE;
    }
    भङ्ग;
  पक्ष 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     */
    यदि(!HTTPREQ_IS_POST(दत्त) || !दत्त->समुच्चयः.post303) {
      http_switch_गन्तव्यम्_get(दत्त, 303);
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

CURLcode Curl_http_follow(संरचना Curl_easy *दत्त, ध्रुव वर्ण *newurl,
                          followtype type)
{
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  वर्ण *follow_url = रिक्त;
  CURLUcode uc;
  CURLcode परिणाम;
  CURLcode rewind_परिणाम;
  bool switch_गन्तव्यम्_get;

  DEBUGASSERT(type != FOLLOW_NONE);

  यदि(type != FOLLOW_FAKE)
    दत्त->state.अनुरोधाः++; /* count all real follows */
  यदि(type == FOLLOW_REDIR) {
    यदि((दत्त->समुच्चयः.maxredirs != -1) &&
       (दत्त->state.followlocation >= दत्त->समुच्चयः.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    अन्यथा {
      दत्त->state.followlocation++; /* count redirect-followings, including
                                       auth reloads */

      यदि(दत्त->समुच्चयः.http_auto_referer) {
        परिणाम = समुच्चयः_auto_referer(दत्त);
        यदि(परिणाम)
          प्रत्यागम परिणाम;
      }
    }
  }

  यदि((type != FOLLOW_RETRY) &&
     (दत्त->req.httpcode != 401) && (दत्त->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, रिक्त, 0, FALSE)) {
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: do not allow a custom port number */
    disallowport = TRUE;
  }

  परिणाम = follow_parse_url(दत्त, newurl, type, &follow_url, &uc);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  DEBUGASSERT(follow_url);

  यदि(type == FOLLOW_FAKE)
    प्रत्यागम follow_fake(दत्त, follow_url, uc, reachedmax);

  यदि(disallowport)
    दत्त->state.allow_पत्तनम् = FALSE;

  Curl_bufref_समुच्चयः(&दत्त->state.url, follow_url, 0, curl_free);
  rewind_परिणाम = Curl_req_soft_reset(&दत्त->req, दत्त);
  infof(दत्त, "Issue another request to this URL: '%s'", follow_url);
  यदि((दत्त->समुच्चयः.http_follow_बहुलकम् == CURLFOLLOW_FIRSTONLY) &&
     !दत्त->state.http_ignorecustom &&
     CURL_EASY_STR(दत्त, STRING_CUSTOMREQUEST)) {
    दत्त->state.http_ignorecustom = TRUE;
    infof(दत्त, "Drop custom request method for next request");
  }

  switch_गन्तव्यम्_get = follow_switch_get(दत्त);

  /* When rewind of upload data failed and we are not switching to GET,
   * we need to fail the follow, as we cannot send the data again. */
  यदि(rewind_परिणाम && !switch_गन्तव्यम्_get)
    प्रत्यागम rewind_परिणाम;

  Curl_pgrsTime(दत्त, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(दत्त);

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
                        ध्रुव वर्ण *शीर्षकम्, /* header keyword _with_ colon */
                        ध्रुव size_t hlen, /* len of the keyword in bytes */
                        ध्रुव वर्ण *content, /* content string to find */
                        ध्रुव size_t clen) /* len of the content in bytes */
{
  /* RFC2616, section 4.2 says: "Each header field consists of a name followed
   * by a colon (":") and the field value. Field names are case-insensitive.
   * The field value MAY be preceded by any amount of LWS, though a single SP
   * is preferred." */

  ध्रुव वर्ण *p;
  संरचना Curl_str val;
  DEBUGASSERT(hlen);
  DEBUGASSERT(clen);
  DEBUGASSERT(शीर्षकम्);
  DEBUGASSERT(content);

  यदि(!curl_strnequal(headerline, शीर्षकम्, hlen))
    प्रत्यागम FALSE; /* does not start with header */

  /* pass the header */
  p = &headerline[hlen];

  यदि(curlx_str_cspn(&p, &val, "\r\n"))
    प्रत्यागम FALSE;
  curlx_str_trimblanks(&val);

  /* find the content string in the rest of the line */
  यदि(curlx_strlen(&val) >= clen) {
    size_t len;
    p = curlx_str(&val);
    प्रति(len = curlx_strlen(&val); len >= clen;) {
      संरचना Curl_str अग्र;
      ध्रुव वर्ण *o = p;
      /* after a match there must be a comma, space, newline or NUL byte */
      यदि(curl_strnequal(p, content, clen) &&
         ((p[clen] == ',') || ISBLANK(p[clen]) || ISNEWLINE(p[clen]) ||
          !p[clen]))
        प्रत्यागम TRUE; /* match! */
      /* advance to the next comma */
      यदि(curlx_str_until(&p, &अग्र, len, ',') ||
         curlx_str_single(&p, ','))
        भङ्ग; /* no comma, get out */

      /* if there are more dummy commas, move over them as well */
      कुरु
        curlx_str_passblanks(&p);
      यावत्(!curlx_str_single(&p, ','));
      /* trailing blanks may move the parsing point past the value end,
         then there is nothing left to match */
      यदि((size_t)(p - o) > len)
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

स्थिर शून्य http_exp100_continue(संरचना Curl_easy *दत्त,
                                 संरचना Curl_creader *reader)
{
  संरचना cr_exp100_ctx *ctx = reader->ctx;
  यदि(ctx->state > EXP100_SEND_दत्त) {
    ctx->state = EXP100_SEND_दत्त;
    Curl_expire_clear(दत्त, EXPIRE_100_TIMEOUT);
  }
}

स्थिर CURLcode cr_exp100_read(संरचना Curl_easy *दत्त,
                               संरचना Curl_creader *reader,
                               वर्ण *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  संरचना cr_exp100_ctx *ctx = reader->ctx;
  timediff_t ms;

  विकल्प(ctx->state) {
  पक्ष EXP100_SENDING_REQUEST:
    यदि(!Curl_req_sendbuf_रिक्तानि(दत्त)) {
      /* The initial request data has not been fully sent yet. Do
       * not start the timer yet. */
      DEBUGF(infof(दत्त, "cr_exp100_read, request not full sent yet"));
      *nread = 0;
      *eos = FALSE;
      प्रत्यागम CURLE_OK;
    }
    /* We are now waiting for a reply from the server or
     * a timeout on our side IFF the request has been fully sent. */
    DEBUGF(infof(दत्त, "cr_exp100_read, start AWAITING_CONTINUE, "
                 "timeout %dms", दत्त->समुच्चयः.expect_100_timeout));
    ctx->state = EXP100_AWAITING_CONTINUE;
    ctx->start = *Curl_pgrs_now(दत्त);
    Curl_expire_समुच्चयः(दत्त, EXPIRE_100_TIMEOUT,
                    दत्त->समुच्चयः.expect_100_timeout, &ctx->start);
    *nread = 0;
    *eos = FALSE;
    प्रत्यागम CURLE_OK;
  पक्ष EXP100_FAILED:
    DEBUGF(infof(दत्त, "cr_exp100_read, expectation failed, error"));
    *nread = 0;
    *eos = FALSE;
    प्रत्यागम CURLE_READ_ERROR;
  पक्ष EXP100_AWAITING_CONTINUE:
    ms = curlx_ptimediff_ms(Curl_pgrs_now(दत्त), &ctx->start);
    यदि(ms < दत्त->समुच्चयः.expect_100_timeout) {
      DEBUGF(infof(दत्त, "cr_exp100_read, AWAITING_CONTINUE, not expired"));
      *nread = 0;
      *eos = FALSE;
      प्रत्यागम CURLE_OK;
    }
    /* we have waited long enough, continue anyway */
    http_exp100_continue(दत्त, reader);
    infof(दत्त, "Done waiting for 100-continue");
    FALLTHROUGH();
  स्वभाव:
    DEBUGF(infof(दत्त, "cr_exp100_read, pass through"));
    प्रत्यागम Curl_creader_read(दत्त, reader->अग्र, buf, blen, nread, eos);
  }
}

स्थिर शून्य cr_exp100_समाप्तम्(संरचना Curl_easy *दत्त,
                           संरचना Curl_creader *reader, पूर्णाङ्क premature)
{
  संरचना cr_exp100_ctx *ctx = reader->ctx;
  ctx->state = premature ? EXP100_FAILED : EXP100_SEND_दत्त;
  Curl_expire_clear(दत्त, EXPIRE_100_TIMEOUT);
}

स्थिर ध्रुव संरचना Curl_crtype cr_exp100 = {
  "cr-exp100",
  Curl_creader_def_init,
  cr_exp100_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  Curl_creader_def_योग_दीर्घता,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  cr_exp100_समाप्तम्,
  परिमाण(संरचना cr_exp100_ctx)
};

स्थिर CURLcode http_exp100_सङ्कलय_reader(संरचना Curl_easy *दत्त)
{
  संरचना Curl_creader *reader = रिक्त;
  CURLcode परिणाम;

  परिणाम = Curl_creader_create(&reader, दत्त, &cr_exp100, CURL_CR_PROTOCOL);
  यदि(!परिणाम)
    परिणाम = Curl_creader_सङ्कलय(दत्त, reader);
  यदि(!परिणाम) {
    संरचना cr_exp100_ctx *ctx = reader->ctx;
    ctx->state = EXP100_SENDING_REQUEST;
  }

  यदि(परिणाम && reader)
    Curl_creader_free(दत्त, reader);
  प्रत्यागम परिणाम;
}

स्थिर शून्य http_exp100_got100(संरचना Curl_easy *दत्त)
{
  संरचना Curl_creader *r = Curl_creader_get_by_type(दत्त, &cr_exp100);
  यदि(r)
    http_exp100_continue(दत्त, r);
}

स्थिर bool http_exp100_is_प्रतीक्षा(संरचना Curl_easy *दत्त)
{
  संरचना Curl_creader *r = Curl_creader_get_by_type(दत्त, &cr_exp100);
  यदि(r) {
    संरचना cr_exp100_ctx *ctx = r->ctx;
    प्रत्यागम ctx->state == EXP100_AWAITING_CONTINUE;
  }
  प्रत्यागम FALSE;
}

स्थिर शून्य http_exp100_send_anyway(संरचना Curl_easy *दत्त)
{
  संरचना Curl_creader *r = Curl_creader_get_by_type(दत्त, &cr_exp100);
  यदि(r)
    http_exp100_continue(दत्त, r);
}

स्थिर bool http_exp100_is_selected(संरचना Curl_easy *दत्त)
{
  संरचना Curl_creader *r = Curl_creader_get_by_type(दत्त, &cr_exp100);
  प्रत्यागम !!r;
}

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
CURLcode Curl_http_doing_pollset(संरचना Curl_easy *दत्त,
                                 संरचना easy_pollset *ps)
{
  /* write mode */
  प्रत्यागम Curl_pollset_सङ्कलय_बहिर्गमनम्(दत्त, ps, दत्त->conn->sock[FIRSTSOCKET]);
}

CURLcode Curl_http_perform_pollset(संरचना Curl_easy *दत्त,
                                   संरचना easy_pollset *ps)
{
  संरचना connectdata *conn = दत्त->conn;
  CURLcode परिणाम = CURLE_OK;

  यदि(CURL_REQ_WANT_RECV(दत्त)) {
    परिणाम = Curl_pollset_सङ्कलय_अन्तर्(दत्त, ps, conn->sock[FIRSTSOCKET]);
  }

  /* on a "Expect: 100-continue" timed wait, do not poll for outgoing */
  यदि(!परिणाम && Curl_req_want_send(दत्त) && !http_exp100_is_प्रतीक्षा(दत्त)) {
    परिणाम = Curl_pollset_सङ्कलय_बहिर्गमनम्(दत्त, ps, conn->sock[FIRSTSOCKET]);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode http_write_शीर्षकम्(संरचना Curl_easy *दत्त,
                                  ध्रुव वर्ण *hd, size_t hdlen)
{
  CURLcode परिणाम;
  पूर्णाङ्क writetype;

  /* now, only output this if the header AND body are requested:
   */
  Curl_debug(दत्त, CURLINFO_शीर्षकम्_अन्तर्, hd, hdlen);

  writetype = CLIENTWRITE_शीर्षकम् |
    ((दत्त->req.httpcode / 100 == 1) ? CLIENTWRITE_1XX : 0);

  परिणाम = Curl_client_write(दत्त, writetype, hd, hdlen);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  परिणाम = Curl_bump_headersize(दत्त, hdlen, FALSE);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  दत्त->req.deductheadercount = (100 <= दत्त->req.httpcode &&
                                 199 >= दत्त->req.httpcode) ?
    दत्त->req.headerbytecount : 0;
  प्रत्यागम परिणाम;
}

/*
 * Curl_http_done() gets called after a single HTTP request has been
 * performed.
 */

CURLcode Curl_http_समाप्तम्(संरचना Curl_easy *दत्त,
                        CURLcode स्थितिः, bool premature)
{
  संरचना connectdata *conn = दत्त->conn;

  /* Clear multipass flag. If authentication is not done yet, then it will get
   * a chance to be set back to true when we output the next auth header */
  दत्त->state.authhost.multipass = FALSE;
  दत्त->state.authproxy.multipass = FALSE;

  यदि(curlx_dyn_len(&दत्त->state.headerb)) {
    (शून्य)http_write_शीर्षकम्(दत्त, curlx_dyn_ptr(&दत्त->state.headerb),
                            curlx_dyn_len(&दत्त->state.headerb));
  }
  curlx_dyn_reset(&दत्त->state.headerb);

  यदि(स्थितिः)
    प्रत्यागम स्थितिः;

  यदि(!premature && /* this check is pointless when DONE is called before the
                      entire operation is complete */
     !conn->द्वयङ्काः.retry &&
     !दत्त->समुच्चयः.connect_only &&
     (दत्त->req.bytecount +
      दत्त->req.headerbytecount -
      दत्त->req.deductheadercount) <= 0) {
    /* If this connection is not closed to be retried, AND nothing was
       read from the HTTP server (that counts), this cannot be right so we
       return an error here */
    failf(दत्त, "Empty reply from server");
    /* Mark it as closed to avoid the "left intact" message */
    streamclose(conn);
    प्रत्यागम CURLE_GOT_NOTHING;
  }

  प्रत्यागम CURLE_OK;
}

/* Determine if we may use HTTP 1.1 for this request. */
स्थिर bool http_may_use_1_1(ध्रुव संरचना Curl_easy *दत्त)
{
  ध्रुव संरचना connectdata *conn = दत्त->conn;
  /* We have seen a previous response for *this* transfer with 1.0,
   * on another connection or the same one. */
  यदि(दत्त->state.http_neg.rcvd_min == 10)
    प्रत्यागम FALSE;
  /* We have seen a previous response on *this* connection with 1.0. */
  यदि(conn && conn->httpversion_आलोकितम् == 10)
    प्रत्यागम FALSE;
  /* We want 1.0 and have seen no previous response on *this* connection
     with a higher version (maybe no response at all yet). */
  यदि(दत्त->state.http_neg.only_10 &&
     (!conn || conn->httpversion_आलोकितम् <= 10))
    प्रत्यागम FALSE;
  /* We are not restricted to use 1.0 only. */
  प्रत्यागम !दत्त->state.http_neg.only_10;
}

स्थिर अचिह्नित वर्ण http_request_संस्करणम्(संरचना Curl_easy *दत्त)
{
  अचिह्नित वर्ण v = Curl_conn_http_संस्करणम्(दत्त, दत्त->conn);
  यदि(!v) {
    /* No specific HTTP connection filter installed. */
    v = http_may_use_1_1(दत्त) ? 11 : 10;
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

CURLcode Curl_सङ्कलय_custom_शीर्षकाणि(संरचना Curl_easy *दत्त,
                                 bool is_connect, पूर्णाङ्क httpversion,
                                 संरचना dynbuf *req)
{
  संरचना curl_slist *h[2];
  संरचना curl_slist *शीर्षकाणि;
  पूर्णाङ्क numlists = 1; /* by default */
  पूर्णाङ्क i;

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  गणना Curl_proxy_use proxy;

  यदि(is_connect)
    proxy = शीर्षकम्_CONNECT;
  अन्यथा
    proxy = दत्त->conn->द्वयङ्काः.origin_is_proxy ? शीर्षकम्_PROXY : शीर्षकम्_SERVER;

  विकल्प(proxy) {
  पक्ष शीर्षकम्_SERVER:
    h[0] = दत्त->समुच्चयः.शीर्षकाणि;
    भङ्ग;
  पक्ष शीर्षकम्_PROXY:
    h[0] = दत्त->समुच्चयः.शीर्षकाणि;
    यदि(दत्त->समुच्चयः.sep_शीर्षकाणि) {
      h[1] = दत्त->समुच्चयः.proxyheaders;
      numlists++;
    }
    भङ्ग;
  पक्ष शीर्षकम्_CONNECT:
    यदि(दत्त->समुच्चयः.sep_शीर्षकाणि)
      h[0] = दत्त->समुच्चयः.proxyheaders;
    अन्यथा
      h[0] = दत्त->समुच्चयः.शीर्षकाणि;
    भङ्ग;
  पक्ष शीर्षकम्_CONNECT_UDP:
    यदि(दत्त->समुच्चयः.sep_शीर्षकाणि)
      h[0] = दत्त->समुच्चयः.proxyheaders;
    अन्यथा
      h[0] = दत्त->समुच्चयः.शीर्षकाणि;
    भङ्ग;
  }
#अन्यथा
  (शून्य)is_connect;
  h[0] = दत्त->समुच्चयः.शीर्षकाणि;
#यदि_अन्तः

  /* loop through one or two lists */
  प्रति(i = 0; i < numlists; i++) {
    प्रति(शीर्षकाणि = h[i]; शीर्षकाणि; शीर्षकाणि = शीर्षकाणि->अग्र) {
      CURLcode परिणाम = CURLE_OK;
      bool blankheader = FALSE;
      संरचना Curl_str name;
      ध्रुव वर्ण *p = शीर्षकाणि->दत्त;
      ध्रुव वर्ण *origp = p;
      size_t hlen = वाक्यदैर्घ्यम्(origp);

      /* explicitly asked to send header without content is done by a header
         that ends with a semicolon, but there must be no colon present in the
         name */
      यदि(!curlx_str_until(&p, &name, hlen, ';') &&
         !curlx_str_single(&p, ';') &&
         !curlx_str_single(&p, '\0') &&
         !memchr(curlx_str(&name), ':', curlx_strlen(&name)))
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

      यदि(दत्त->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom
            Host: header as that will produce *two* in the same
            request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      अन्यथा यदि(दत्त->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      अन्यथा यदि(दत्त->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      अन्यथा यदि(दत्त->req.authneg &&
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
              !Curl_auth_allowed_गन्तव्यम्_host(दत्त))
        ;
      अन्यथा यदि(blankheader) {
        परिणाम = curlx_dyn_addn(req, curlx_str(&name), curlx_strlen(&name));
        यदि(!परिणाम)
          परिणाम = curlx_dyn_addn(req, STRCONST(":\r\n"));
      }
      अन्यथा
        परिणाम = curlx_dyn_addf(req, "%s\r\n", origp);

      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
  }

  प्रत्यागम CURLE_OK;
}

#यदि_अपरिभाषितम् CURL_DISABLE_PARSEDATE
CURLcode Curl_सङ्कलय_timecondition(संरचना Curl_easy *दत्त,
                                संरचना dynbuf *req)
{
  ध्रुव संरचना tm *tm;
  संरचना tm keeptime;
  CURLcode परिणाम;
  वर्ण datestr[80];
  ध्रुव वर्ण *condp;
  size_t len;

  यदि(दत्त->समुच्चयः.timecondition == CURL_TIMECOND_NONE)
    /* no condition was asked for */
    प्रत्यागम CURLE_OK;

  परिणाम = curlx_gmtime(दत्त->समुच्चयः.timevalue, &keeptime);
  यदि(परिणाम) {
    failf(दत्त, "Invalid TIMEVALUE");
    प्रत्यागम परिणाम;
  }
  tm = &keeptime;

  विकल्प(दत्त->समुच्चयः.timecondition) {
  स्वभाव:
    DEBUGF(infof(दत्त, "invalid time condition"));
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

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

  यदि(Curl_checkheaders(दत्त, condp, len)) {
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
                 Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                 tm->tm_mday,
                 Curl_month[tm->tm_mon],
                 tm->tm_year + 1900,
                 tm->tm_hour,
                 tm->tm_min,
                 tm->tm_sec);

  परिणाम = curlx_dyn_सङ्कलय(req, datestr);
  प्रत्यागम परिणाम;
}
#अन्यथा
/* disabled */
CURLcode Curl_सङ्कलय_timecondition(संरचना Curl_easy *दत्त,
                                संरचना dynbuf *req)
{
  (शून्य)दत्त;
  (शून्य)req;
  प्रत्यागम CURLE_OK;
}
#यदि_अन्तः

शून्य Curl_http_विधानम्(संरचना Curl_easy *दत्त,
                      ध्रुव वर्ण **विधानम्, Curl_HttpReq *reqp)
{
  Curl_HttpReq httpreq = (Curl_HttpReq)दत्त->state.httpreq;
  ध्रुव वर्ण *request;
#यदि_अपरिभाषितम् CURL_DISABLE_WEBSOCKETS
  यदि(दत्त->conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
    httpreq = HTTPREQ_GET;
  अन्यथा
#यदि_अन्तः
  यदि((दत्त->conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_FTP)) &&
     दत्त->state.upload)
    httpreq = HTTPREQ_PUT;

  /* Now set the 'request' pointer to the proper request string */
  यदि(!दत्त->state.http_ignorecustom &&
     CURL_EASY_STR(दत्त, STRING_CUSTOMREQUEST)) {
    request = CURL_EASY_STR(दत्त, STRING_CUSTOMREQUEST);
  }
  अन्यथा {
    यदि(दत्त->req.no_body)
      request = "HEAD";
    अन्यथा {
      DEBUGASSERT((httpreq >= HTTPREQ_GET) && (httpreq <= HTTPREQ_शीर्ष));
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
      पक्ष HTTPREQ_शीर्ष:
        request = "HEAD";
        भङ्ग;
      }
    }
  }
  *विधानम् = request;
  *reqp = httpreq;
}

स्थिर CURLcode http_समुच्चयः_aptr_host(संरचना Curl_easy *दत्त)
{
  संरचना connectdata *conn = दत्त->conn;
  ध्रुव वर्ण *ptr = रिक्त;

  curlx_safefree(दत्त->state.http_host);
#यदि_अपरिभाषितम् CURL_DISABLE_COOKIES
  curlx_safefree(दत्त->req.cookiehost);
#यदि_अन्तः

  यदि(Curl_peer_equal(दत्त->state.initial_origin, दत्त->state.origin))
    ptr = Curl_checkheaders(दत्त, STRCONST("Host"));

  यदि(ptr) {
#यदि_अपरिभाषितम् CURL_DISABLE_COOKIES
    /* If we have a given custom Host: header, we extract the hostname in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the hostname
       is the same as the first one! */
    वर्ण *cookiehost;
    CURLcode परिणाम = copy_custom_मानम्(ptr, &cookiehost);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
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
        memmove(cookiehost, cookiehost + 1, वाक्यदैर्घ्यम्(cookiehost) - 1);
        closingbracket = strchr(cookiehost, ']');
        यदि(closingbracket)
          *closingbracket = 0;
      }
      अन्यथा {
        पूर्णाङ्क startsearch = 0;
        वर्ण *colon = strchr(cookiehost + startsearch, ':');
        यदि(colon)
          *colon = 0; /* The host must not include an embedded port number */
      }
      दत्त->req.cookiehost = cookiehost;
    }
#यदि_अन्तः

    यदि(!curl_strequal("Host:", ptr)) {
      दत्त->state.http_host = curl_maprintf("Host:%s", &ptr[5]);
      यदि(!दत्त->state.http_host)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
  }
  अन्यथा {
    /* This is the  HTTP Host: header, so we want
     * - for IPv6 origins: "[ipv6-address]" where the IPv6 address is
     *  found in origin->hostname, stripped of zoneid/scopeid.
     * - the (IDN converted) origin->hostname (DNS name or IPv4) otherwise.
     * Note: zoneid/scopeid  only applies to local routing and has no
     * meaning on the remote HTTP server (eg. would confuse it). */
    bool ipv6 = (bool)दत्त->state.origin->ipv6;
    संरचना dynbuf tmp;
    size_t hlen;
    CURLcode परिणाम;

    curlx_dyn_init(&tmp, DYN_HTTP_REQUEST);
    परिणाम = curlx_dyn_addn(&tmp, STRCONST("Host: "));
    यदि(!परिणाम && ipv6)
      परिणाम = curlx_dyn_addn(&tmp, STRCONST("["));
    यदि(!परिणाम)
      परिणाम = curlx_dyn_सङ्कलय(&tmp, दत्त->state.origin->hostname);
    यदि(!परिणाम && ipv6)
      परिणाम = curlx_dyn_addn(&tmp, STRCONST("]"));
    यदि(!परिणाम &&
       ((दत्त->state.origin->पत्तनम् != दत्त->state.origin->scheme->defport) ||
       (दत्त->state.origin->scheme->family != conn->scheme->family))) {
      परिणाम = curlx_dyn_addf(&tmp, ":%u", दत्त->state.origin->पत्तनम्);
    }

    दत्त->state.http_host = परिणाम ? रिक्त : curlx_dyn_take(&tmp, &hlen);
    curlx_dyn_free(&tmp);
    प्रत्यागम परिणाम;
  }
  प्रत्यागम CURLE_OK;
}

/*
 * Append the request-target to the HTTP request
 */
स्थिर CURLcode http_target(संरचना Curl_easy *दत्त,
                            संरचना dynbuf *r)
{
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण *मार्गः = दत्त->state.ऊर्ध्वम्.मार्गः;
  ध्रुव वर्ण *query = दत्त->state.ऊर्ध्वम्.query;
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  संरचना connectdata *conn = दत्त->conn;
#यदि_अन्तः

  यदि(CURL_EASY_STR(दत्त, STRING_TARGET)) {
    मार्गः = CURL_EASY_STR(दत्त, STRING_TARGET);
    query = रिक्त;
  }

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  यदि(conn->द्वयङ्काः.origin_is_proxy) {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL, but if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the decoded hostname! */

    /* and no fragment part */
    CURLUcode uc;
    वर्ण *url;
    CURLU *h = curl_url_dup(दत्त->state.uh);
    यदि(!h)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    यदि(!दत्त->state.origin->ipv6 &&
       (दत्त->state.origin->user_hostname != दत्त->state.origin->hostname)) {
      uc = curl_url_समुच्चयः(h, CURLUPART_HOST, दत्त->state.origin->hostname, 0);
      यदि(uc) {
        curl_url_cleanup(h);
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
    }
    uc = curl_url_समुच्चयः(h, CURLUPART_FRAGMENT, रिक्त, 0);
    यदि(uc) {
      curl_url_cleanup(h);
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }

    यदि(दत्त->state.origin->scheme == &Curl_scheme_http) {
      /* when getting HTTP, we do not want the userinfo the URL */
      uc = curl_url_समुच्चयः(h, CURLUPART_USER, रिक्त, 0);
      यदि(uc) {
        curl_url_cleanup(h);
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
      uc = curl_url_समुच्चयः(h, CURLUPART_PASSWORD, रिक्त, 0);
      यदि(uc) {
        curl_url_cleanup(h);
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
    }
    अन्यथा यदि(दत्त->state.creds && (दत्त->state.creds->source != CREDS_URL)) {
        /* credentials not from the URL need to be set */
      uc = curl_url_समुच्चयः(h, CURLUPART_USER,
                        दत्त->state.creds->user, CURLU_URLENCODE);
      यदि(!uc)
        uc = curl_url_समुच्चयः(h, CURLUPART_PASSWORD,
                          दत्त->state.creds->passwd, CURLU_URLENCODE);
      यदि(uc) {
        curl_url_cleanup(h);
        प्रत्यागम Curl_uc_गन्तव्यम्_curlcode(uc);
      }
    }

    /* Extract the URL to use in the request. */
    uc = curl_url_get(h, CURLUPART_URL, &url, CURLU_NO_DEFAULT_पत्तनम्);
    यदि(uc) {
      curl_url_cleanup(h);
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }

    curl_url_cleanup(h);

    /* target or URL */
    परिणाम = curlx_dyn_सङ्कलय(r, CURL_EASY_STR(दत्त, STRING_TARGET) ?
      CURL_EASY_STR(दत्त, STRING_TARGET) : url);
    curlx_free(url);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    यदि((दत्त->state.origin->scheme == &Curl_scheme_ftp) &&
       दत्त->समुच्चयः.proxy_transfer_बहुलकम्) {
      /* when doing ftp, append ;type=<a|i> if not present */
      size_t len = वाक्यदैर्घ्यम्(मार्गः);
      bool type_present = FALSE;
      यदि((len >= 7) && !memcmp(&मार्गः[len - 7], ";type=", 6)) {
        विकल्प(Curl_raw_toupper(मार्गः[len - 1])) {
        पक्ष 'A':
        पक्ष 'D':
        पक्ष 'I':
          type_present = TRUE;
          भङ्ग;
        }
      }
      यदि(!type_present) {
        परिणाम = curlx_dyn_addf(r, ";type=%c",
                                दत्त->state.prefer_ascii ? 'a' : 'i');
        यदि(परिणाम)
          प्रत्यागम परिणाम;
      }
    }
  }

  अन्यथा
#यदि_अन्तः
  {
    परिणाम = curlx_dyn_सङ्कलय(r, मार्गः);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    यदि(query)
      परिणाम = curlx_dyn_addf(r, "?%s", query);
  }

  प्रत्यागम परिणाम;
}

#यदि !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
स्थिर CURLcode समुच्चयः_post_reader(संरचना Curl_easy *दत्त, Curl_HttpReq httpreq)
{
  CURLcode परिणाम;

  विकल्प(httpreq) {
#यदि_अपरिभाषितम् CURL_DISABLE_MIME
  पक्ष HTTPREQ_POST_MIME:
    दत्त->state.mimepost = दत्त->समुच्चयः.mimepostp;
    भङ्ग;
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_FORM_API
  पक्ष HTTPREQ_POST_FORM:
    /* Convert the form structure into a mime structure, then keep
       the conversion */
    यदि(!दत्त->state.formp) {
      दत्त->state.formp = curlx_calloc(1, परिमाण(curl_mimepart));
      यदि(!दत्त->state.formp)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      Curl_mime_cleanpart(दत्त->state.formp);
      परिणाम = Curl_getformdata(दत्त, दत्त->state.formp, दत्त->समुच्चयः.httppost,
                                दत्त->state.fread_func);
      यदि(परिणाम) {
        curlx_safefree(दत्त->state.formp);
        प्रत्यागम परिणाम;
      }
      दत्त->state.mimepost = दत्त->state.formp;
    }
    भङ्ग;
#यदि_अन्तः
  स्वभाव:
    दत्त->state.mimepost = रिक्त;
    भङ्ग;
  }

  विकल्प(httpreq) {
  पक्ष HTTPREQ_POST_FORM:
  पक्ष HTTPREQ_POST_MIME:
    /* This is form posting using mime data. */
#यदि_अपरिभाषितम् CURL_DISABLE_MIME
    यदि(दत्त->state.mimepost) {
      ध्रुव वर्ण *cthdr = Curl_checkheaders(दत्त, STRCONST("Content-Type"));

      /* Read and seek body only. */
      दत्त->state.mimepost->flags |= MIME_BODY_ONLY;

      /* Prepare the mime structure headers & set content type. */

      यदि(cthdr)
        प्रति(cthdr += 13; *cthdr == ' '; cthdr++)
          ;
      अन्यथा यदि(दत्त->state.mimepost->विधा == MIMEKIND_MULTIPART)
        cthdr = "multipart/form-data";

      curl_mime_शीर्षकाणि(दत्त->state.mimepost, दत्त->समुच्चयः.शीर्षकाणि, 0);
      परिणाम = Curl_mime_prepare_शीर्षकाणि(दत्त, दत्त->state.mimepost, cthdr,
                                         रिक्त, MIMESTRATEGY_FORM);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      curl_mime_शीर्षकाणि(दत्त->state.mimepost, रिक्त, 0);
      परिणाम = Curl_creader_समुच्चयः_mime(दत्त, दत्त->state.mimepost);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
    अन्यथा
#यदि_अन्तः
    {
      परिणाम = Curl_creader_समुच्चयः_null(दत्त);
    }
    दत्त->state.infilesize = Curl_creader_योग_दीर्घता(दत्त);
    प्रत्यागम परिणाम;

  स्वभाव:
    प्रत्यागम Curl_creader_समुच्चयः_null(दत्त);
  }
  /* never reached */
}
#यदि_अन्तः

स्थिर CURLcode समुच्चयः_reader(संरचना Curl_easy *दत्त, Curl_HttpReq httpreq)
{
  CURLcode परिणाम = CURLE_OK;
  curl_off_t postsize = दत्त->state.infilesize;

  DEBUGASSERT(दत्त->conn);

  यदि(दत्त->req.authneg) {
    प्रत्यागम Curl_creader_समुच्चयः_null(दत्त);
  }

  विकल्प(httpreq) {
  पक्ष HTTPREQ_PUT: /* Let's PUT the data to the server! */
    प्रत्यागम postsize ? Curl_creader_समुच्चयः_fread(दत्त, postsize) :
      Curl_creader_समुच्चयः_null(दत्त);

#यदि !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  पक्ष HTTPREQ_POST_FORM:
  पक्ष HTTPREQ_POST_MIME:
    प्रत्यागम समुच्चयः_post_reader(दत्त, httpreq);
#यदि_अन्तः

  पक्ष HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */
    /* the size of the post body */
    यदि(!postsize) {
      परिणाम = Curl_creader_समुच्चयः_null(दत्त);
    }
    अन्यथा यदि(दत्त->समुच्चयः.postfields) {
      size_t plen = curlx_sotouz_range(postsize, 0, SIZE_MAX);
      यदि(plen == SIZE_MAX)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      अन्यथा यदि(plen)
        परिणाम = Curl_creader_समुच्चयः_buf(दत्त, दत्त->समुच्चयः.postfields, plen);
      अन्यथा
        परिणाम = Curl_creader_समुच्चयः_null(दत्त);
    }
    अन्यथा {
      /* we read the bytes from the callback. In case "chunked" encoding
       * is forced by the application, we disregard `postsize`. This is
       * a backward compatibility decision to earlier versions where
       * chunking disregarded this. See issue #13229. */
      bool chunked = FALSE;
      वर्ण *ptr = Curl_checkheaders(दत्त, STRCONST("Transfer-Encoding"));
      यदि(ptr) {
        /* Some kind of TE is requested, check if 'chunked' is chosen */
        chunked = Curl_compareheader(ptr, STRCONST("Transfer-Encoding:"),
                                     STRCONST("chunked"));
      }
      परिणाम = Curl_creader_समुच्चयः_fread(दत्त, chunked ? -1 : postsize);
    }
    प्रत्यागम परिणाम;

  स्वभाव:
    /* HTTP GET/HEAD download, has no body, needs no Content-Length */
    दत्त->state.infilesize = 0;
    प्रत्यागम Curl_creader_समुच्चयः_null(दत्त);
  }
  /* not reached */
}

स्थिर CURLcode http_resume(संरचना Curl_easy *दत्त, Curl_HttpReq httpreq)
{
  यदि((HTTPREQ_POST == httpreq || HTTPREQ_POST_FORM == httpreq ||
      HTTPREQ_POST_MIME == httpreq || HTTPREQ_PUT == httpreq) &&
     दत्त->state.resume_from) {
    failf(दत्त, "HTTP upload cannot be resumed");
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode http_req_समुच्चयः_TE(संरचना Curl_easy *दत्त,
                                संरचना dynbuf *req,
                                पूर्णाङ्क httpversion)
{
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण *ptr;

  ptr = Curl_checkheaders(दत्त, STRCONST("Transfer-Encoding"));
  यदि(ptr) {
    /* Some kind of TE is requested, check if 'chunked' is chosen */
    दत्त->req.upload_chunky =
      Curl_compareheader(ptr,
                         STRCONST("Transfer-Encoding:"), STRCONST("chunked"));
    यदि(दत्त->req.upload_chunky && (httpversion >= 20)) {
      infof(दत्त, "suppressing chunked transfer encoding on connection "
            "using HTTP version 2 or higher");
      दत्त->req.upload_chunky = FALSE;
    }
  }
  अन्यथा {
    curl_off_t req_clen = Curl_creader_योग_दीर्घता(दत्त);

    यदि(req_clen < 0) {
      /* indeterminate request content length */
      यदि(httpversion > 10) {
        /* On HTTP/1.1, enable chunked, on HTTP/2 and later we do not
         * need it */
        दत्त->req.upload_chunky = (httpversion < 20);
      }
      अन्यथा {
        failf(दत्त, "Chunky upload is not supported by HTTP 1.0");
        प्रत्यागम CURLE_UPLOAD_FAILED;
      }
    }
    अन्यथा {
      /* else, no chunky upload */
      दत्त->req.upload_chunky = FALSE;
    }

    यदि(दत्त->req.upload_chunky)
      परिणाम = curlx_dyn_सङ्कलय(req, "Transfer-Encoding: chunked\r\n");
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode addexpect(संरचना Curl_easy *दत्त, संरचना dynbuf *r,
                          पूर्णाङ्क httpversion, bool *announced_exp100)
{
  CURLcode परिणाम;
  वर्ण *ptr;

  *announced_exp100 = FALSE;
  /* Avoid Expect: 100-continue if Upgrade: is used */
  यदि(दत्त->req.upgr101 != UPGR101_NONE)
    प्रत्यागम CURLE_OK;

  /* For really small puts we do not use Expect: headers at all, and for
     the somewhat bigger ones we allow the app to disable it. Make
     sure that the expect100header is always set to the preferred value
     here. */
  ptr = Curl_checkheaders(दत्त, STRCONST("Expect"));
  यदि(ptr) {
    *announced_exp100 =
      Curl_compareheader(ptr, STRCONST("Expect:"), STRCONST("100-continue"));
  }
  अन्यथा यदि(!दत्त->state.disableexpect && (httpversion == 11)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    curl_off_t client_len = Curl_creader_client_दीर्घता(दत्त);
    यदि(client_len > EXPECT_100_देहली || client_len < 0) {
      परिणाम = curlx_dyn_addn(r, STRCONST("Expect: 100-continue\r\n"));
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      *announced_exp100 = TRUE;
    }
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode http_सङ्कलय_content_hds(संरचना Curl_easy *दत्त,
                                     संरचना dynbuf *r,
                                     पूर्णाङ्क httpversion,
                                     Curl_HttpReq httpreq)
{
  CURLcode परिणाम = CURLE_OK;
  curl_off_t req_clen;
  bool announced_exp100 = FALSE;

  DEBUGASSERT(दत्त->conn);
  यदि(दत्त->req.upload_chunky) {
    परिणाम = Curl_httpchunk_सङ्कलय_reader(दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  /* Get the request body length that has been set up */
  req_clen = Curl_creader_योग_दीर्घता(दत्त);
  विकल्प(httpreq) {
  पक्ष HTTPREQ_PUT:
  पक्ष HTTPREQ_POST:
#यदि !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  पक्ष HTTPREQ_POST_FORM:
  पक्ष HTTPREQ_POST_MIME:
#यदि_अन्तः
    /* We only set Content-Length and allow a custom Content-Length if
       we do not upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length).
       We do not override a custom "Content-Length" header, but during
       authentication negotiation that header is suppressed.
     */
    यदि(req_clen >= 0 && !दत्त->req.upload_chunky &&
       (दत्त->req.authneg ||
        !Curl_checkheaders(दत्त, STRCONST("Content-Length")))) {
      /* we allow replacing this header if not during auth negotiation,
         although it is not wise to actually set your own */
      परिणाम = curlx_dyn_addf(r, "Content-Length: %" FMT_OFF_T "\r\n",
                              req_clen);
    }
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;

#यदि_अपरिभाषितम् CURL_DISABLE_MIME
    /* Output mime-generated headers. */
    यदि(दत्त->state.mimepost &&
       ((httpreq == HTTPREQ_POST_FORM) || (httpreq == HTTPREQ_POST_MIME))) {
      संरचना curl_slist *hdr;

      प्रति(hdr = दत्त->state.mimepost->curlheaders; hdr; hdr = hdr->अग्र) {
        परिणाम = curlx_dyn_addf(r, "%s\r\n", hdr->दत्त);
        यदि(परिणाम)
          गच्छ बहिर्गमनम्;
      }
    }
#यदि_अन्तः
    यदि(httpreq == HTTPREQ_POST &&
       !Curl_checkheaders(दत्त, STRCONST("Content-Type"))) {
      परिणाम = curlx_dyn_addn(r, STRCONST("Content-Type: application/"
                                          "x-www-form-urlencoded\r\n"));
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
    }
    परिणाम = addexpect(दत्त, r, httpversion, &announced_exp100);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }

  Curl_pgrs_समुच्चयः_Upload_आकार(दत्त, req_clen);
  यदि(announced_exp100)
    परिणाम = http_exp100_सङ्कलय_reader(दत्त);

बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

#यदि_अपरिभाषितम् CURL_DISABLE_COOKIES

स्थिर CURLcode http_cookies(संरचना Curl_easy *दत्त,
                             संरचना dynbuf *r)
{
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण *addcookies = रिक्त;
  bool linecap = FALSE;
  यदि(CURL_EASY_STR(दत्त, STRING_COOKIE) &&
     !Curl_checkheaders(दत्त, STRCONST("Cookie")) &&
     Curl_auth_allowed_गन्तव्यम्_host(दत्त))
    addcookies = CURL_EASY_STR(दत्त, STRING_COOKIE);

  यदि(दत्त->cookies || addcookies) {
    संरचना Curl_llist list;
    पूर्णाङ्क गणनम् = 0;

    यदि(दत्त->cookies && दत्त->state.cookie_engine) {
      bool okay;
      ध्रुव वर्ण *host = दत्त->req.cookiehost ?
        दत्त->req.cookiehost : दत्त->state.origin->hostname;
      Curl_share_तालकम्(दत्त, CURL_तालकम्_दत्त_COOKIE, CURL_तालकम्_ACCESS_SINGLE);
      परिणाम = Curl_cookie_getlist(दत्त, &okay, host, &list);
      यदि(!परिणाम && okay) {
        संरचना Curl_llist_ग्रन्थिः *n;
        size_t clen = 8; /* hold the size of the generated Cookie: header */

        /* loop through all cookies that matched */
        प्रति(n = Curl_llist_शीर्ष(&list); n; n = Curl_ग्रन्थिः_अग्र(n)) {
          संरचना Cookie *co = Curl_ग्रन्थिः_elem(n);
          यदि(co->मानम्) {
            size_t सङ्कलय;
            यदि(!गणनम्) {
              परिणाम = curlx_dyn_addn(r, STRCONST("Cookie: "));
              यदि(परिणाम)
                भङ्ग;
            }
            सङ्कलय = वाक्यदैर्घ्यम्(co->name) + वाक्यदैर्घ्यम्(co->मानम्) + 1;
            यदि(clen + सङ्कलय >= MAX_COOKIE_शीर्षकम्_LEN) {
              infof(दत्त, "Restricted outgoing cookies due to header size, "
                    "'%s' not sent", co->name);
              linecap = TRUE;
              भङ्ग;
            }
            परिणाम = curlx_dyn_addf(r, "%s%s=%s", गणनम् ? "; " : "",
                                    co->name, co->मानम्);
            यदि(परिणाम)
              भङ्ग;
            clen += सङ्कलय + (गणनम् ? 2 : 0);
            गणनम्++;
          }
        }
        Curl_llist_destroy(&list, रिक्त);
      }
      Curl_share_unlock(दत्त, CURL_तालकम्_दत्त_COOKIE);
    }
    यदि(addcookies && !परिणाम && !linecap) {
      यदि(!गणनम्)
        परिणाम = curlx_dyn_addn(r, STRCONST("Cookie: "));
      यदि(!परिणाम) {
        परिणाम = curlx_dyn_addf(r, "%s%s", गणनम् ? "; " : "", addcookies);
        गणनम्++;
      }
    }
    यदि(गणनम् && !परिणाम)
      परिणाम = curlx_dyn_addn(r, STRCONST("\r\n"));

    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  प्रत्यागम परिणाम;
}
#अन्यथा
#परिभाषय http_cookies(a, b) CURLE_OK
#यदि_अन्तः

स्थिर CURLcode http_range(संरचना Curl_easy *दत्त,
                           Curl_HttpReq httpreq)
{
  यदि(दत्त->state.use_range) {
    /*
     * A range is selected. We use different headers whether we are downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    यदि(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_शीर्ष)) &&
       !Curl_checkheaders(दत्त, STRCONST("Range"))) {
      /* if a line like this was already allocated, free the previous one */
      curlx_free(दत्त->state.rangeline);
      दत्त->state.rangeline = curl_maprintf("Range: bytes=%s\r\n",
                                                 दत्त->state.range);
      यदि(!दत्त->state.rangeline)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
    अन्यथा यदि((httpreq == HTTPREQ_POST || httpreq == HTTPREQ_PUT) &&
            !Curl_checkheaders(दत्त, STRCONST("Content-Range"))) {
      curl_off_t req_clen = Curl_creader_योग_दीर्घता(दत्त);
      /* if a line like this was already allocated, free the previous one */
      curlx_free(दत्त->state.rangeline);

      यदि(दत्त->समुच्चयः.समुच्चयः_resume_from < 0) {
        /* Upload resume was asked for, but we do not know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        दत्त->state.rangeline =
          curl_maprintf("Content-Range: bytes 0-%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n", req_clen - 1, req_clen);
      }
      अन्यथा यदि(दत्त->state.resume_from) {
        /* This is because "resume" was selected */
        /* Not sure if we want to send this header during authentication
         * negotiation, but test1084 checks for it. In which case we have a
         * "null" client reader installed that gives an unexpected length. */
        curl_off_t योग_len = दत्त->req.authneg ?
                               दत्त->state.infilesize :
                               (दत्त->state.resume_from + req_clen);
        दत्त->state.rangeline =
          curl_maprintf("Content-Range: bytes %s%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n",
                        दत्त->state.range, योग_len - 1, योग_len);
      }
      अन्यथा {
        /* Range was selected and then we pass the incoming range and append
           total size */
        दत्त->state.rangeline =
          curl_maprintf("Content-Range: bytes %s/%" FMT_OFF_T "\r\n",
                        दत्त->state.range, req_clen);
      }
      यदि(!दत्त->state.rangeline)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode http_firstwrite(संरचना Curl_easy *दत्त)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना SingleRequest *k = &दत्त->req;

  यदि(दत्त->req.newurl) {
    यदि(conn->द्वयङ्काः.close) {
      /* Abort after the headers if "follow Location" is set
         and we are set to close anyway. */
      CURL_REQ_CLEAR_RECV(दत्त);
      k->समाप्तम् = TRUE;
      प्रत्यागम CURLE_OK;
    }
    /* We have a new URL to load, but since we want to be able to reuse this
       connection properly, we read the full response in "ignore more" */
    k->ignorebody = TRUE;
    infof(दत्त, "Ignoring the response-body");
  }
  यदि(दत्त->state.resume_from && !k->content_range &&
     (दत्त->state.httpreq == HTTPREQ_GET) &&
     !k->ignorebody) {

    यदि(k->आकार == दत्त->state.resume_from) {
      /* The resume point is at the end of file, consider this fine even if it
         does not allow resume from here. */
      infof(दत्त, "The entire document is already downloaded");
      streamclose(conn);
      /* Abort download */
      CURL_REQ_CLEAR_RECV(दत्त);
      k->समाप्तम् = TRUE;
      प्रत्यागम CURLE_OK;
    }

    /* we wanted to resume a download, although the server does not seem to
     * support this and we did this with a GET (if it was not a GET we did a
     * POST or PUT resume) */
    failf(दत्त, "HTTP server does not seem to support "
          "byte ranges. Cannot resume.");
    प्रत्यागम CURLE_RANGE_ERROR;
  }

  यदि(दत्त->समुच्चयः.timecondition && !दत्त->state.range &&
     /* A time condition has been set AND no ranges have been requested. This
        seems to be what chapter 13.3.4 of RFC 2616 defines to be the correct
        action for an HTTP/1.1 client */
     !Curl_meets_timecondition(दत्त, k->timeofdoc)) {
    k->समाप्तम् = TRUE;
    /* We are simulating an HTTP 304 from server so we return
       what should have been returned from the server */
    दत्त->info.httpcode = 304;
    infof(दत्त, "Simulate an HTTP 304 response");
    /* we abort the transfer before it is completed == we ruin the
       reuse ability. Close the connection */
    streamclose(conn);
    प्रत्यागम CURLE_OK;
  } /* we have a time condition */

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode http_check_new_conn(संरचना Curl_easy *दत्त)
{
  संरचना connectdata *conn = दत्त->conn;
  ध्रुव वर्ण *info_संस्करणम् = रिक्त;
  ध्रुव वर्ण *alpn;
  CURLcode परिणाम;

  alpn = Curl_conn_get_alpn_negotiated(दत्त, conn);
  यदि(alpn && !वाक्यं_तुलय("h3", alpn)) {
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
    यदि(!conn->द्वयङ्काः.origin_is_proxy)
#यदि_अन्तः
      DEBUGASSERT(Curl_conn_http_संस्करणम्(दत्त, conn) == 30);
    info_संस्करणम् = "HTTP/3";
  }
  अन्यथा यदि(alpn && !वाक्यं_तुलय("h2", alpn)) {
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
    यदि((Curl_conn_http_संस्करणम्(दत्त, conn) != 20) &&
       conn->द्वयङ्काः.origin_is_proxy) {
      परिणाम = Curl_http2_switch(दत्त);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
    अन्यथा
#यदि_अन्तः
    DEBUGASSERT(Curl_conn_http_संस्करणम्(दत्त, conn) == 20);
    info_संस्करणम् = "HTTP/2";
  }
  अन्यथा {
    /* Check if user wants to use HTTP/2 with clear TCP */
    यदि(Curl_http2_may_switch(दत्त)) {
      DEBUGF(infof(दत्त, "HTTP/2 over clean TCP"));
      परिणाम = Curl_http2_switch(दत्त);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      info_संस्करणम् = "HTTP/2";
      /* There is no ALPN here, but the connection is now definitely h2 */
      conn->httpversion_आलोकितम् = 20;
      Curl_conn_समुच्चयः_multiplex(conn);
    }
    अन्यथा
      info_संस्करणम् = "HTTP/1.x";
  }

  यदि(info_संस्करणम्)
    infof(दत्त, "using %s", info_संस्करणम्);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode http_सङ्कलय_connection_hd(संरचना Curl_easy *दत्त,
                                       संरचना dynbuf *req)
{
  संरचना curl_slist *शीर्ष;
  ध्रुव वर्ण *sep = "Connection: ";
  CURLcode परिणाम = CURLE_OK;
  size_t rlen = curlx_dyn_len(req);
  bool skip;

  /* Add the 1st custom "Connection: " header, if there is one */
  प्रति(शीर्ष = दत्त->समुच्चयः.शीर्षकाणि; शीर्ष; शीर्ष = शीर्ष->अग्र) {
    यदि(curl_strnequal(शीर्ष->दत्त, "Connection", 10) &&
       Curl_headersep(शीर्ष->दत्त[10]) &&
       !http_शीर्षकम्_is_रिक्तानि(शीर्ष->दत्त)) {
      वर्ण *मानम्;
      परिणाम = copy_custom_मानम्(शीर्ष->दत्त, &मानम्);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      परिणाम = curlx_dyn_addf(req, "%s%s", sep, मानम्);
      sep = ", ";
      curlx_free(मानम्);
      भङ्ग; /* leave, having added 1st one */
    }
  }

  /* add our internal Connection: header values, if we have any */
  यदि(!परिणाम && दत्त->state.http_hd_te) {
    परिणाम = curlx_dyn_addf(req, "%s%s", sep, "TE");
    sep = ", ";
  }
  यदि(!परिणाम && दत्त->state.http_hd_upgrade) {
    परिणाम = curlx_dyn_addf(req, "%s%s", sep, "Upgrade");
    sep = ", ";
  }
  यदि(!परिणाम && दत्त->state.http_hd_h2_settings) {
    परिणाम = curlx_dyn_addf(req, "%s%s", sep, "HTTP2-Settings");
  }
  यदि(!परिणाम && (rlen < curlx_dyn_len(req)))
    परिणाम = curlx_dyn_addn(req, STRCONST("\r\n"));
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* Add all user-defined Connection: headers after the first */
  skip = TRUE;
  प्रति(शीर्ष = दत्त->समुच्चयः.शीर्षकाणि; शीर्ष; शीर्ष = शीर्ष->अग्र) {
    यदि(curl_strnequal(शीर्ष->दत्त, "Connection", 10) &&
       Curl_headersep(शीर्ष->दत्त[10]) &&
       !http_शीर्षकम्_is_रिक्तानि(शीर्ष->दत्त)) {
      यदि(skip) {
        skip = FALSE;
        अनुवर्त;
      }
      परिणाम = curlx_dyn_addf(req, "%s\r\n", शीर्ष->दत्त);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
  }

  प्रत्यागम CURLE_OK;
}

/* Header identifier in order we send them by default */
प्रकारनिर्देश गणना {
  H1_HD_REQUEST,
  H1_HD_HOST,
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  H1_HD_PROXY_AUTH,
#यदि_अन्तः
  H1_HD_AUTH,
  H1_HD_RANGE,
  H1_HD_USER_AGENT,
  H1_HD_ACCEPT,
  H1_HD_TE,
  H1_HD_ACCEPT_ENCODING,
  H1_HD_REFERER,
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  H1_HD_PROXY_CONNECTION,
#यदि_अन्तः
  H1_HD_TRANSFER_ENCODING,
#यदि_अपरिभाषितम् CURL_DISABLE_ALTSVC
  H1_HD_ALT_प्रयुक्तम्,
#यदि_अन्तः
  H1_HD_UPGRADE,
  H1_HD_COOKIES,
  H1_HD_CONDITIONALS,
  H1_HD_CUSTOM,
  H1_HD_CONTENT,
  H1_HD_CONNECTION,
  H1_HD_चरमः  /* the last, empty header line */
} http_hd_t;

स्थिर CURLcode http_सङ्कलय_hd(संरचना Curl_easy *दत्त,
                            संरचना dynbuf *req,
                            http_hd_t id,
                            अचिह्नित वर्ण httpversion,
                            ध्रुव वर्ण *विधानम्,
                            Curl_HttpReq httpreq)
{
  CURLcode परिणाम = CURLE_OK;
#यदि !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_PROXY) || \
  !defined(CURL_DISABLE_WEBSOCKETS)
  संरचना connectdata *conn = दत्त->conn;
#यदि_अन्तः
  विकल्प(id) {
  पक्ष H1_HD_REQUEST:
    /* add the main request stuff */
    /* GET/HEAD/POST/PUT */
    परिणाम = curlx_dyn_addf(req, "%s ", विधानम्);
    यदि(!परिणाम)
      परिणाम = http_target(दत्त, req);
    यदि(!परिणाम)
      परिणाम = curlx_dyn_addf(req, " HTTP/%s\r\n",
                              get_http_string(httpversion));
    भङ्ग;

  पक्ष H1_HD_HOST:
    यदि(दत्त->state.http_host) {
      परिणाम = curlx_dyn_सङ्कलय(req, दत्त->state.http_host);
      यदि(!परिणाम)
        परिणाम = curlx_dyn_addn(req, STRCONST("\r\n"));
    }
    भङ्ग;

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  पक्ष H1_HD_PROXY_AUTH:
    यदि(दत्त->req.hd_proxy_auth)
      परिणाम = curlx_dyn_सङ्कलय(req, दत्त->req.hd_proxy_auth);
    भङ्ग;
#यदि_अन्तः

  पक्ष H1_HD_AUTH:
    यदि(दत्त->req.hd_auth)
      परिणाम = curlx_dyn_सङ्कलय(req, दत्त->req.hd_auth);
    भङ्ग;

  पक्ष H1_HD_RANGE:
    यदि(दत्त->state.use_range && दत्त->state.rangeline)
      परिणाम = curlx_dyn_सङ्कलय(req, दत्त->state.rangeline);
    भङ्ग;

  पक्ष H1_HD_USER_AGENT: {
    ध्रुव वर्ण *ua = CURL_EASY_STR(दत्त, STRING_USERAGENT);
    यदि(ua && *ua && !Curl_checkheaders(दत्त, STRCONST("User-Agent")))
      परिणाम = curlx_dyn_addf(req, "User-Agent: %s\r\n", ua);
    भङ्ग;
  }

  पक्ष H1_HD_ACCEPT:
    यदि(!Curl_checkheaders(दत्त, STRCONST("Accept")))
      परिणाम = curlx_dyn_सङ्कलय(req, "Accept: */*\r\n");
    भङ्ग;

  पक्ष H1_HD_TE:
#यदि_परिभाषितम् HAVE_LIBZ
    यदि(!Curl_checkheaders(दत्त, STRCONST("TE")) &&
       दत्त->समुच्चयः.http_transfer_encoding) {
      दत्त->state.http_hd_te = TRUE;
      परिणाम = curlx_dyn_सङ्कलय(req, "TE: gzip\r\n");
    }
#यदि_अन्तः
    भङ्ग;

  पक्ष H1_HD_ACCEPT_ENCODING: {
    ध्रुव वर्ण *enc = CURL_EASY_STR(दत्त, STRING_ENCODING);
    यदि(enc && !Curl_checkheaders(दत्त, STRCONST("Accept-Encoding")))
      परिणाम = curlx_dyn_addf(req, "Accept-Encoding: %s\r\n", enc);
    भङ्ग;
  }

  पक्ष H1_HD_REFERER:
    यदि(Curl_bufref_ptr(&दत्त->state.referer) &&
       !Curl_checkheaders(दत्त, STRCONST("Referer")))
      परिणाम = curlx_dyn_addf(req, "Referer: %s\r\n",
                              Curl_bufref_ptr(&दत्त->state.referer));
    भङ्ग;

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  पक्ष H1_HD_PROXY_CONNECTION:
    यदि(conn->द्वयङ्काः.origin_is_proxy &&
       !Curl_checkheaders(दत्त, STRCONST("Proxy-Connection")) &&
       !Curl_checkProxyheaders(दत्त, दत्त->conn, STRCONST("Proxy-Connection")))
      परिणाम = curlx_dyn_सङ्कलय(req, "Proxy-Connection: Keep-Alive\r\n");
    भङ्ग;
#यदि_अन्तः

  पक्ष H1_HD_TRANSFER_ENCODING:
    परिणाम = http_req_समुच्चयः_TE(दत्त, req, httpversion);
    भङ्ग;

#यदि_अपरिभाषितम् CURL_DISABLE_ALTSVC
  पक्ष H1_HD_ALT_प्रयुक्तम्:
    यदि(conn->द्वयङ्काः.altused && conn->द्वारा_peer &&
       !Curl_checkheaders(दत्त, STRCONST("Alt-Used")))
      परिणाम = curlx_dyn_addf(req, "Alt-Used: %s:%u\r\n",
                              conn->द्वारा_peer->hostname, conn->द्वारा_peer->पत्तनम्);
    भङ्ग;
#यदि_अन्तः

  पक्ष H1_HD_UPGRADE:
    यदि(!Curl_conn_is_ssl(दत्त->conn, FIRSTSOCKET) && (httpversion < 20) &&
       (दत्त->state.http_neg.wanted & CURL_HTTP_V2x) &&
       दत्त->state.http_neg.h2_upgrade) {
      /* append HTTP2 upgrade magic stuff to the HTTP request if it is not done
         over SSL */
      परिणाम = Curl_http2_request_upgrade(req, दत्त);
    }
#यदि_अपरिभाषितम् CURL_DISABLE_WEBSOCKETS
    यदि(!परिणाम && conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
      परिणाम = Curl_ws_request(दत्त, req);
#यदि_अन्तः
    भङ्ग;

  पक्ष H1_HD_COOKIES:
    परिणाम = http_cookies(दत्त, req);
    भङ्ग;

  पक्ष H1_HD_CONDITIONALS:
    परिणाम = Curl_सङ्कलय_timecondition(दत्त, req);
    भङ्ग;

  पक्ष H1_HD_CUSTOM:
    परिणाम = Curl_सङ्कलय_custom_शीर्षकाणि(दत्त, FALSE, httpversion, req);
    भङ्ग;

  पक्ष H1_HD_CONTENT:
    परिणाम = http_सङ्कलय_content_hds(दत्त, req, httpversion, httpreq);
    भङ्ग;

  पक्ष H1_HD_CONNECTION: {
    परिणाम = http_सङ्कलय_connection_hd(दत्त, req);
    भङ्ग;
  }

  पक्ष H1_HD_चरमः:
    परिणाम = curlx_dyn_addn(req, STRCONST("\r\n"));
    भङ्ग;
  }
  प्रत्यागम परिणाम;
}

/*
 * Curl_http() gets called from the generic multi_do() function when an HTTP
 * request is to be performed. This creates and sends a properly constructed
 * HTTP request.
 */
CURLcode Curl_http(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  Curl_HttpReq httpreq;
  ध्रुव वर्ण *विधानम्;
  संरचना dynbuf req;
  अचिह्नित वर्ण httpversion;
  size_t hd_id;

  /* Always consider the DO phase done after this function call, even if there
     may be parts of the request that are not yet sent, since we can deal with
     the rest of the request in the PERFORM phase. */
  *समाप्तम् = TRUE;
  /* initialize a dynamic send-buffer */
  curlx_dyn_init(&req, DYN_HTTP_REQUEST);
  /* make sure the header buffer is reset - if there are leftovers from a
     previous transfer */
  curlx_dyn_reset(&दत्त->state.headerb);
  दत्त->state.maybe_folded = FALSE;

  यदि(!दत्त->conn->द्वयङ्काः.reuse) {
    परिणाम = http_check_new_conn(दत्त);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  /* Add collecting of headers written to client. For a new connection,
   * we might have done that already, but reuse
   * or multiplex needs it here as well. */
  परिणाम = Curl_शीर्षकाणि_init(दत्त);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  दत्त->state.http_hd_te = FALSE;
  दत्त->state.http_hd_upgrade = FALSE;
  दत्त->state.http_hd_h2_settings = FALSE;

  /* what kind of request do we need to send? */
  Curl_http_विधानम्(दत्त, &विधानम्, &httpreq);

  /* select host to send */
  परिणाम = http_समुच्चयः_aptr_host(दत्त);
  /* setup the authentication headers, how that method and host are known */
  यदि(!परिणाम)
    परिणाम = Curl_http_output_auth(दत्त, दत्त->conn, विधानम्, httpreq,
                                   दत्त->state.ऊर्ध्वम्.मार्गः,
                                   दत्त->state.ऊर्ध्वम्.query, FALSE);
  /* Setup input reader, resume information and ranges */
  यदि(!परिणाम)
    परिणाम = समुच्चयः_reader(दत्त, httpreq);
  यदि(!परिणाम)
    परिणाम = http_resume(दत्त, httpreq);
  यदि(!परिणाम)
    परिणाम = http_range(दत्त, httpreq);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  httpversion = http_request_संस्करणम्(दत्त);
  /* Add request line and all headers to `req` */
  प्रति(hd_id = 0; hd_id <= H1_HD_चरमः; ++hd_id) {
    परिणाम = http_सङ्कलय_hd(दत्त, &req, (http_hd_t)hd_id,
                         httpversion, विधानम्, httpreq);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  /* setup variables for the upcoming transfer and send */
  Curl_xfer_सज्जता_sendrecv(दत्त, FIRSTSOCKET, -1);
  परिणाम = Curl_req_send(दत्त, &req, httpversion);

  यदि((httpversion >= 20) && दत्त->req.upload_chunky)
    /* upload_chunky was set above to set up the request in a chunky fashion,
       but is disabled here again to avoid that the chunked encoded version is
       actually used when sending the request body over h2 */
    दत्त->req.upload_chunky = FALSE;

बहिर्गमनम्:
  यदि(परिणाम == CURLE_TOO_LARGE)
    failf(दत्त, "HTTP request too large");

  curlx_dyn_free(&req);
  प्रत्यागम परिणाम;
}

प्रकारनिर्देश गणना {
  स्थितिः_UNKNOWN, /* not enough data to tell yet */
  स्थितिः_समाप्तम्, /* a status line was read */
  स्थितिः_BAD /* not a status line */
} statusline;

/* Check a string for a prefix. Check no more than 'len' bytes */
स्थिर bool checkprefixmax(ध्रुव वर्ण *उपसर्गः, ध्रुव वर्ण *बफर, size_t len)
{
  size_t ch = CURLMIN(वाक्यदैर्घ्यम्(उपसर्गः), len);
  प्रत्यागम curl_strnequal(उपसर्गः, बफर, ch);
}

/*
 * checkhttpprefix()
 *
 * Returns TRUE if member of the list matches prefix of string
 */
स्थिर statusline checkhttpprefix(संरचना Curl_easy *दत्त,
                                  ध्रुव वर्ण *s, size_t len)
{
  संरचना curl_slist *शीर्ष = दत्त->समुच्चयः.http200aliases;
  statusline rc = स्थितिः_BAD;
  statusline onmatch = len >= 5 ? स्थितिः_समाप्तम् : स्थितिः_UNKNOWN;

  यावत्(शीर्ष) {
    यदि(checkprefixmax(शीर्ष->दत्त, s, len)) {
      rc = onmatch;
      भङ्ग;
    }
    शीर्ष = शीर्ष->अग्र;
  }

  यदि((rc != स्थितिः_समाप्तम्) && checkprefixmax("HTTP/", s, len))
    rc = onmatch;

  प्रत्यागम rc;
}

#यदि_अपरिभाषितम् CURL_DISABLE_RTSP
स्थिर statusline checkrtspprefix(संरचना Curl_easy *दत्त,
                                  ध्रुव वर्ण *s, size_t len)
{
  statusline स्थितिः = स्थितिः_BAD;
  statusline onmatch = len >= 5 ? स्थितिः_समाप्तम् : स्थितिः_UNKNOWN;
  (शून्य)दत्त;
  यदि(checkprefixmax("RTSP/", s, len))
    स्थितिः = onmatch;

  प्रत्यागम स्थितिः;
}
#यदि_अन्तः /* CURL_DISABLE_RTSP */

स्थिर statusline checkprotoprefix(संरचना Curl_easy *दत्त,
                                   संरचना connectdata *conn,
                                   ध्रुव वर्ण *s, size_t len)
{
#यदि_अपरिभाषितम् CURL_DISABLE_RTSP
  यदि(conn->scheme->protocol & CURLPROTO_RTSP)
    प्रत्यागम checkrtspprefix(दत्त, s, len);
#अन्यथा
  (शून्य)conn;
#यदि_अन्तः /* CURL_DISABLE_RTSP */

  प्रत्यागम checkhttpprefix(दत्त, s, len);
}

/* HTTP header has field name `n` (a string constant) */
#परिभाषय HD_IS(hd, hdlen, n) \
  (((hdlen) >= (परिमाण(n) - 1)) && curl_strnequal(n, hd, परिमाण(n) - 1))

#परिभाषय HD_VAL(hd, hdlen, n) \
  ((((hdlen) >= (परिमाण(n) - 1)) && (hd) && \
    curl_strnequal(n, hd, परिमाण(n) - 1)) ? ((hd) + (परिमाण(n) - 1)) : रिक्त)

/* HTTP header has field name `n` (a string constant) and contains `v`
 * (a string constant) in its value(s) */
#परिभाषय HD_IS_AND_SAYS(hd, hdlen, n, v) \
  (HD_IS(hd, hdlen, n) && \
   ((hdlen) > ((परिमाण(n) - 1) + (परिमाण(v) - 1))) && \
   Curl_compareheader(hd, STRCONST(n), STRCONST(v)))

/*
 * http_header_a() parses a single response header starting with A.
 */
स्थिर CURLcode http_शीर्षकम्_a(संरचना Curl_easy *दत्त,
                              ध्रुव वर्ण *hd, size_t hdlen)
{
#यदि_अपरिभाषितम् CURL_DISABLE_ALTSVC
  ध्रुव वर्ण *v;
  v = (दत्त->asi &&
       (Curl_xfer_is_secure(दत्त) ||
#यदि_परिभाषितम् DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_ALTSVC_HTTP")
#अन्यथा
        0
#यदि_अन्तः
         )) ? HD_VAL(hd, hdlen, "Alt-Svc:") : रिक्त;
  यदि(v) {
    /* the ALPN of the current request */
    संरचना SingleRequest *k = &दत्त->req;
    गणना alpnid id = (k->httpversion == 30) ? ALPN_h3 :
      (k->httpversion == 20) ? ALPN_h2 : ALPN_h1;
    प्रत्यागम Curl_altsvc_parse(दत्त, दत्त->asi, v, दत्त->state.origin, id);
  }
#अन्यथा
  (शून्य)दत्त;
  (शून्य)hd;
  (शून्य)hdlen;
#यदि_अन्तः
  प्रत्यागम CURLE_OK;
}

/*
 * http_header_c() parses a single response header starting with C.
 */
स्थिर CURLcode http_शीर्षकम्_c(संरचना Curl_easy *दत्त,
                              ध्रुव वर्ण *hd, size_t hdlen)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना SingleRequest *k = &दत्त->req;
  ध्रुव वर्ण *v;

  /* Check for Content-Length: header lines to get size. Browsers insist we
     should accept multiple Content-Length headers and that a comma separated
     list also is fine and then we should accept them all as long as they are
     the same value. Different values trigger error.
   */
  v = (!k->http_bodyless && !दत्त->समुच्चयः.ignorecl && !k->ignore_cl) ?
    HD_VAL(hd, hdlen, "Content-Length:") : रिक्त;
  यदि(v) {
    कुरु {
      curl_off_t contentlength;
      पूर्णाङ्क offt = curlx_str_numblanks(&v, &contentlength);

      यदि(offt == STRE_अतिप्रवाहः) {
        /* out of range */
        यदि(दत्त->समुच्चयः.max_filesize) {
          failf(दत्त, "Maximum file size exceeded");
          प्रत्यागम CURLE_FILESIZE_EXCEEDED;
        }
        streamclose(conn);
        infof(दत्त, "Overflow Content-Length: value");
        प्रत्यागम CURLE_OK;
      }
      अन्यथा {
        यदि((offt == STRE_OK) &&
           ((k->आकार == -1) || /* not set to something before */
            (k->आकार == contentlength))) { /* or the same value */

          k->आकार = contentlength;
          curlx_str_passblanks(&v);

          /* on a comma, loop and get the next instead */
          यदि(!curlx_str_single(&v, ','))
            अनुवर्त;

          यदि(!curlx_str_newline(&v)) {
            k->maxdownload = k->आकार;
            प्रत्यागम CURLE_OK;
          }
        }
        /* negative, different value or rubbish - bad HTTP */
        failf(दत्त, "Invalid Content-Length: value");
        प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
      }
    } यावत्(1);
  }
  v = (!k->http_bodyless && CURL_EASY_STR(दत्त, STRING_ENCODING)) ?
    HD_VAL(hd, hdlen, "Content-Encoding:") : रिक्त;
  यदि(v) {
    /*
     * Process Content-Encoding. Look for the values: identity, gzip, deflate,
     * compress, x-gzip and x-compress. x-gzip and x-compress are the same as
     * gzip and compress. (Sec 3.5 RFC 2616). zlib cannot handle compress.
     * Errors are handled further down when the response body is processed
     */
    प्रत्यागम Curl_build_unencoding_राशिः(दत्त, v, FALSE);
  }
  /* check for Content-Type: header lines to get the MIME-type */
  v = HD_VAL(hd, hdlen, "Content-Type:");
  यदि(v) {
    वर्ण *contenttype = Curl_copy_शीर्षकम्_मानम्(hd);
    यदि(!contenttype)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    यदि(!*contenttype)
      /* ignore empty data */
      curlx_free(contenttype);
    अन्यथा {
      curlx_free(दत्त->info.contenttype);
      दत्त->info.contenttype = contenttype;
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
    infof(दत्त, "HTTP/1.0 connection set to keep alive");
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
      यदि(!curlx_str_सङ्ख्या(&ptr, &k->विस्थापनम्, CURL_OFF_T_MAX) &&
         (दत्त->state.resume_from == k->विस्थापनम्))
        /* we asked for a resume and we got it */
        k->content_range = TRUE;
    }
    अन्यथा यदि(k->httpcode < 300)
      दत्त->state.resume_from = 0; /* get everything */
  }
  प्रत्यागम CURLE_OK;
}

/*
 * http_header_l() parses a single response header starting with L.
 */
स्थिर CURLcode http_शीर्षकम्_l(संरचना Curl_easy *दत्त,
                              ध्रुव वर्ण *hd, size_t hdlen)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना SingleRequest *k = &दत्त->req;
  ध्रुव वर्ण *v = (!k->http_bodyless &&
                   (दत्त->समुच्चयः.timecondition || दत्त->समुच्चयः.get_filetime)) ?
    HD_VAL(hd, hdlen, "Last-Modified:") : रिक्त;
  यदि(v) {
    यदि(Curl_getdate_capped(v, &k->timeofdoc))
      k->timeofdoc = 0;
    यदि(दत्त->समुच्चयः.get_filetime)
      दत्त->info.filetime = k->timeofdoc;
    प्रत्यागम CURLE_OK;
  }
  यदि(HD_IS(hd, hdlen, "Location:")) {
    /* this is the URL that the server advises us to use instead */
    वर्ण *location = Curl_copy_शीर्षकम्_मानम्(hd);
    यदि(!location)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    यदि(!*location ||
       (दत्त->req.location && !वाक्यं_तुलय(दत्त->req.location, location))) {
      /* ignore empty header, or exact repeat of a previous one */
      curlx_free(location);
      प्रत्यागम CURLE_OK;
    }
    अन्यथा {
      /* has value and is not an exact repeat */
      यदि(दत्त->req.location) {
        failf(दत्त, "Multiple Location headers");
        curlx_free(location);
        प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
      }
      दत्त->req.location = location;

      यदि((k->httpcode >= 300 && k->httpcode < 400) &&
         दत्त->समुच्चयः.http_follow_बहुलकम्) {
        CURLcode परिणाम;
        DEBUGASSERT(!दत्त->req.newurl);
        दत्त->req.newurl = curlx_strdup(दत्त->req.location); /* clone */
        यदि(!दत्त->req.newurl)
          प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

        /* some cases of POST and PUT etc needs to rewind the data
           stream at this point */
        परिणाम = http_perhapsrewind(दत्त, conn);
        यदि(परिणाम)
          प्रत्यागम परिणाम;

        /* mark the next request as a followed location: */
        दत्त->state.this_is_a_follow = TRUE;
      }
    }
  }
  प्रत्यागम CURLE_OK;
}

/*
 * http_header_p() parses a single response header starting with P.
 */
स्थिर CURLcode http_शीर्षकम्_p(संरचना Curl_easy *दत्त,
                              ध्रुव वर्ण *hd, size_t hdlen)
{
  संरचना SingleRequest *k = &दत्त->req;

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  ध्रुव वर्ण *v = HD_VAL(hd, hdlen, "Proxy-Connection:");
  यदि(v) {
    संरचना connectdata *conn = दत्त->conn;
    यदि((k->httpversion == 10) && conn->http_proxy.peer &&
       HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "keep-alive")) {
      /*
       * When an HTTP/1.0 reply comes when using a proxy, the
       * 'Proxy-Connection: keep-alive' line tells us the
       * connection will be kept alive for our pleasure.
       * Default action for 1.0 is to close.
       */
      connkeep(conn); /* do not close */
      infof(दत्त, "HTTP/1.0 proxy connection set to keep alive");
    }
    अन्यथा यदि((k->httpversion == 11) && conn->http_proxy.peer &&
            HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "close")) {
      /*
       * We get an HTTP/1.1 response from a proxy and it says it will
       * close down after this transfer.
       */
      connclose(conn);
      infof(दत्त, "HTTP/1.1 proxy connection set close");
    }
    प्रत्यागम CURLE_OK;
  }
#यदि_अन्तः
  यदि((407 == k->httpcode) && HD_IS(hd, hdlen, "Proxy-authenticate:")) {
    वर्ण *auth;
    CURLcode परिणाम;
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
    यदि(!दत्त->conn->द्वयङ्काः.origin_is_proxy)
      /* this response is not from a forward proxy, not for us */
      प्रत्यागम CURLE_OK;
#यदि_अन्तः
    auth = Curl_copy_शीर्षकम्_मानम्(hd);
    परिणाम = auth ? CURLE_OK : CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    यदि(!परिणाम) {
      परिणाम = Curl_http_निवेशः_auth(दत्त, TRUE, auth);
      curlx_free(auth);
    }
    प्रत्यागम परिणाम;
  }
#यदि_परिभाषितम् USE_SPNEGO
  यदि(HD_IS(hd, hdlen, "Persistent-Auth:")) {
    संरचना connectdata *conn = दत्त->conn;
    संरचना negotiatedata *negdata = Curl_auth_nego_get(conn, FALSE);
    संरचना auth *authp = &दत्त->state.authhost;
    यदि(!negdata)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    यदि(authp->picked == CURLAUTH_NEGOTIATE) {
      वर्ण *persistentauth = Curl_copy_शीर्षकम्_मानम्(hd);
      यदि(!persistentauth)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      negdata->noauthpersist = !!checkprefix("false", persistentauth);
      negdata->havenoauthpersist = TRUE;
      infof(दत्त, "Negotiate: noauthpersist -> %d, header part: %s",
            negdata->noauthpersist, persistentauth);
      curlx_free(persistentauth);
    }
  }
#यदि_अन्तः
  प्रत्यागम CURLE_OK;
}

/*
 * http_header_r() parses a single response header starting with R.
 */
स्थिर CURLcode http_शीर्षकम्_r(संरचना Curl_easy *दत्त,
                              ध्रुव वर्ण *hd, size_t hdlen)
{
  ध्रुव वर्ण *v = HD_VAL(hd, hdlen, "Retry-After:");
  यदि(v) {
    /* Retry-After = HTTP-date / delay-seconds */
    curl_off_t retry_after = 0; /* zero for unknown or "now" */
    time_t date = 0;
    curlx_str_passblanks(&v);

    /* try it as a date first, because a date can otherwise start with and
       get treated as a number */
    यदि(!Curl_getdate_capped(v, &date)) {
      time_t current = time(रिक्त);
      यदि(date >= current)
        /* convert date to number of seconds into the future */
        retry_after = date - current;
    }
    अन्यथा
      /* Try it as a decimal number, ignore errors */
      (शून्य)curlx_str_सङ्ख्या(&v, &retry_after, CURL_OFF_T_MAX);
    /* limit to 6 hours max. this is not documented so that it can be changed
       in the future if necessary. */
    यदि(retry_after > 21600)
      retry_after = 21600;
    दत्त->info.retry_after = retry_after;
  }
  प्रत्यागम CURLE_OK;
}

/*
 * http_header_s() parses a single response header starting with S.
 */
स्थिर CURLcode http_शीर्षकम्_s(संरचना Curl_easy *दत्त,
                              ध्रुव वर्ण *hd, size_t hdlen)
{
#यदि !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_HSTS)
  ध्रुव वर्ण *v;
#अन्यथा
  (शून्य)दत्त;
  (शून्य)hd;
  (शून्य)hdlen;
#यदि_अन्तः

#यदि_अपरिभाषितम् CURL_DISABLE_COOKIES
  v = (दत्त->cookies && दत्त->state.cookie_engine) ?
    HD_VAL(hd, hdlen, "Set-Cookie:") : रिक्त;
  यदि(v) {
    /* If there is a custom-set Host: name, use it here, or else use
     * real peer hostname. */
    ध्रुव वर्ण *host = दत्त->req.cookiehost ?
      दत्त->req.cookiehost : दत्त->state.origin->hostname;
    ध्रुव अचिह्नित वर्ण secure_context = Curl_secure_context(दत्त, host) ?
      COOKIE_SECURE : 0;
    CURLcode परिणाम;
    Curl_share_तालकम्(दत्त, CURL_तालकम्_दत्त_COOKIE, CURL_तालकम्_ACCESS_SINGLE);
    परिणाम = Curl_cookie_सङ्कलय(दत्त, दत्त->cookies, v, host,
                             दत्त->state.ऊर्ध्वम्.मार्गः,
                             COOKIE_HTTPHEADER | secure_context);
    Curl_share_unlock(दत्त, CURL_तालकम्_दत्त_COOKIE);
    प्रत्यागम परिणाम;
  }
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_HSTS
  /* If enabled, the header is incoming and this is over HTTPS */
  v = (दत्त->hsts &&
       (Curl_xfer_is_secure(दत्त) ||
#यदि_परिभाषितम् DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_HSTS_HTTP")
#अन्यथा
        0
#यदि_अन्तः
         )
    ) ? HD_VAL(hd, hdlen, "Strict-Transport-Security:") : रिक्त;
  यदि(v) {
    CURLcode परिणाम = Curl_hsts_parse(
      दत्त->hsts, दत्त->state.origin->hostname, v);
    यदि(परिणाम) {
      यदि(परिणाम == CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः)
        प्रत्यागम परिणाम;
      infof(दत्त, "Illegal STS header skipped");
    }
#यदि_परिभाषितम् DEBUGBUILD
    अन्यथा
      infof(दत्त, "Parsed STS header fine (%zu entries)",
            Curl_llist_गणनम्(&दत्त->hsts->list));
#यदि_अन्तः
  }
#यदि_अन्तः

  प्रत्यागम CURLE_OK;
}

/*
 * http_header_t() parses a single response header starting with T.
 */
स्थिर CURLcode http_शीर्षकम्_t(संरचना Curl_easy *दत्त,
                              ध्रुव वर्ण *hd, size_t hdlen)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना SingleRequest *k = &दत्त->req;

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
                   (दत्त->state.httpreq != HTTPREQ_शीर्ष) &&
                   (k->httpcode != 304)) ?
    HD_VAL(hd, hdlen, "Transfer-Encoding:") : रिक्त;
  यदि(v) {
    /* One or more encodings. We check for chunked and/or a compression
       algorithm. */
    CURLcode परिणाम = Curl_build_unencoding_राशिः(दत्त, v, TRUE);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    यदि(!k->chunk) {
      /* if this is not chunked, only close can signal the end of this
       * transfer as Content-Length is said not to be trusted for
       * transfer-encoding! */
      CURL_TRC_M(दत्त, "HTTP/1.1 transfer-encoding without chunks");
      connclose(conn);
      k->ignore_cl = TRUE;
    }
    प्रत्यागम CURLE_OK;
  }
  v = HD_VAL(hd, hdlen, "Trailer:");
  यदि(v) {
    दत्त->req.resp_trailer = TRUE;
    प्रत्यागम CURLE_OK;
  }
  प्रत्यागम CURLE_OK;
}

/*
 * http_header_w() parses a single response header starting with W.
 */
स्थिर CURLcode http_शीर्षकम्_w(संरचना Curl_easy *दत्त,
                              ध्रुव वर्ण *hd, size_t hdlen)
{
  संरचना SingleRequest *k = &दत्त->req;
  CURLcode परिणाम = CURLE_OK;

  यदि((401 == k->httpcode) && HD_IS(hd, hdlen, "WWW-Authenticate:")) {
    वर्ण *auth = Curl_copy_शीर्षकम्_मानम्(hd);
    यदि(!auth)
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    अन्यथा {
      परिणाम = Curl_http_निवेशः_auth(दत्त, FALSE, auth);
      curlx_free(auth);
    }
  }
  प्रत्यागम परिणाम;
}

/*
 * http_header() parses a single response header.
 */
स्थिर CURLcode http_शीर्षकम्(संरचना Curl_easy *दत्त,
                            ध्रुव वर्ण *hd, size_t hdlen)
{
  CURLcode परिणाम = CURLE_OK;

  विकल्प(hd[0]) {
  पक्ष 'a':
  पक्ष 'A':
    परिणाम = http_शीर्षकम्_a(दत्त, hd, hdlen);
    भङ्ग;
  पक्ष 'c':
  पक्ष 'C':
    परिणाम = http_शीर्षकम्_c(दत्त, hd, hdlen);
    भङ्ग;
  पक्ष 'l':
  पक्ष 'L':
    परिणाम = http_शीर्षकम्_l(दत्त, hd, hdlen);
    भङ्ग;
  पक्ष 'p':
  पक्ष 'P':
    परिणाम = http_शीर्षकम्_p(दत्त, hd, hdlen);
    भङ्ग;
  पक्ष 'r':
  पक्ष 'R':
    परिणाम = http_शीर्षकम्_r(दत्त, hd, hdlen);
    भङ्ग;
  पक्ष 's':
  पक्ष 'S':
    परिणाम = http_शीर्षकम्_s(दत्त, hd, hdlen);
    भङ्ग;
  पक्ष 't':
  पक्ष 'T':
    परिणाम = http_शीर्षकम्_t(दत्त, hd, hdlen);
    भङ्ग;
  पक्ष 'w':
  पक्ष 'W':
    परिणाम = http_शीर्षकम्_w(दत्त, hd, hdlen);
    भङ्ग;
  }

  यदि(!परिणाम) {
    संरचना connectdata *conn = दत्त->conn;
    यदि(conn->scheme->protocol & CURLPROTO_RTSP)
      परिणाम = Curl_rtsp_parseheader(दत्त, hd);
  }
  प्रत्यागम परिणाम;
}

/*
 * Called after the first HTTP response line (the status line) has been
 * received and parsed.
 */
स्थिर CURLcode http_statusline(संरचना Curl_easy *दत्त,
                                संरचना connectdata *conn)
{
  संरचना SingleRequest *k = &दत्त->req;

  विकल्प(k->httpversion) {
  पक्ष 10:
  पक्ष 11:
#यदि_परिभाषितम् USE_HTTP2
  पक्ष 20:
#यदि_अन्तः
#यदि_परिभाषितम् USE_HTTP3
  पक्ष 30:
#यदि_अन्तः
    /* no major version switch mid-connection */
    यदि(k->httpversion_प्रेषितम् &&
       (k->httpversion / 10 != k->httpversion_प्रेषितम् / 10)) {
      failf(दत्त, "Version mismatch (from HTTP/%d to HTTP/%d)",
            k->httpversion_प्रेषितम् / 10, k->httpversion / 10);
      प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
    }
    भङ्ग;
  स्वभाव:
    failf(दत्त, "Unsupported HTTP version (%d.%d) in response",
          k->httpversion / 10, k->httpversion % 10);
    प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
  }

  दत्त->info.httpcode = k->httpcode;
  दत्त->info.httpversion = k->httpversion;
  conn->httpversion_आलोकितम् = k->httpversion;

  यदि(!दत्त->state.http_neg.rcvd_min ||
     दत्त->state.http_neg.rcvd_min > k->httpversion)
    /* store the lowest server version we encounter */
    दत्त->state.http_neg.rcvd_min = k->httpversion;

  /*
   * This code executes as part of processing the header. As a
   * result, it is not totally clear how to interpret the
   * response code yet as that depends on what other headers may
   * be present. 401 and 407 may be errors, but may be OK
   * depending on how authentication is working. Other codes
   * are definitely errors, so give up here.
   */
  यदि(दत्त->state.resume_from && दत्त->state.httpreq == HTTPREQ_GET &&
     k->httpcode == 416) {
    /* "Requested Range Not Satisfiable", proceed and pretend this is no
       error */
    k->ignorebody = TRUE; /* Avoid appending error msg to good data. */
  }

  यदि(k->httpversion == 10) {
    /* Default action for HTTP/1.0 must be to close, unless
       we get one of those fancy headers that tell us the
       server keeps it open for us! */
    infof(दत्त, "HTTP 1.0, assume close after body");
    connclose(conn);
  }

  k->http_bodyless = k->httpcode >= 100 && k->httpcode < 200;
  विकल्प(k->httpcode) {
  पक्ष 304:
    /* (quote from RFC2616, section 10.3.5): The 304 response
     * MUST NOT contain a message-body, and thus is always
     * terminated by the first empty line after the header
     * fields. */
    यदि(दत्त->समुच्चयः.timecondition)
      दत्त->info.timecond = TRUE;
    FALLTHROUGH();
  पक्ष 204:
    /* (quote from RFC2616, section 10.2.5): The server has
     * fulfilled the request but does not need to return an
     * entity-body ... The 204 response MUST NOT include a
     * message-body, and thus is always terminated by the first
     * empty line after the header fields. */
    k->आकार = 0;
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
स्थिर CURLcode http_आकार(संरचना Curl_easy *दत्त)
{
  संरचना SingleRequest *k = &दत्त->req;
  यदि(दत्त->req.ignore_cl || k->chunk) {
    k->आकार = k->maxdownload = -1;
  }
  अन्यथा यदि(k->आकार != -1) {
    यदि(दत्त->समुच्चयः.max_filesize &&
       !k->ignorebody &&
       (k->आकार > दत्त->समुच्चयः.max_filesize)) {
      failf(दत्त, "Maximum file size exceeded");
      प्रत्यागम CURLE_FILESIZE_EXCEEDED;
    }
    यदि(k->ignorebody)
      infof(दत्त, "setting size while ignoring");
    Curl_pgrs_समुच्चयः_Download_आकार(दत्त, k->आकार);
    k->maxdownload = k->आकार;
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_verify_शीर्षकम्(संरचना Curl_easy *दत्त,
                            ध्रुव वर्ण *hd, size_t hdlen)
{
  संरचना SingleRequest *k = &दत्त->req;
  size_t i;
  bool has_colon = FALSE;
  प्रति(i = 0; i < hdlen; i++) {
    विकल्प(hd[i]) {
    पक्ष '\0':
      /* this is bad, bail out */
      failf(दत्त, "NUL byte in header");
      प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
    पक्ष '\r':
      यदि(i < hdlen - 2) {
        /* CR may only precede the LF, nothing else */
        failf(दत्त, "Carriage return found in header");
        प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
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
    failf(दत्त, "Header without colon");
    प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_bump_headersize(संरचना Curl_easy *दत्त,
                              size_t delta,
                              bool connect_only)
{
  size_t bad = 0;
  अचिह्नित पूर्णाङ्क max = MAX_HTTP_RESP_शीर्षकम्_आकार;
  यदि(delta < MAX_HTTP_RESP_शीर्षकम्_आकार) {
    दत्त->info.शीर्षकम्_आकार += (अचिह्नित पूर्णाङ्क)delta;
    दत्त->req.allheadercount += (अचिह्नित पूर्णाङ्क)delta;
    यदि(!connect_only)
      दत्त->req.headerbytecount += (अचिह्नित पूर्णाङ्क)delta;
    यदि(दत्त->req.allheadercount > max)
      bad = दत्त->req.allheadercount;
    अन्यथा यदि(दत्त->info.शीर्षकम्_आकार > (max * 20)) {
      bad = दत्त->info.शीर्षकम्_आकार;
      max *= 20;
    }
  }
  अन्यथा
    bad = दत्त->req.allheadercount + delta;
  यदि(bad) {
    failf(दत्त, "Too large response headers: %zu > %u", bad, max);
    प्रत्यागम CURLE_RECV_ERROR;
  }
  प्रत्यागम CURLE_OK;
}

/*
 * Handle a 101 Switching Protocols response. Performs the actual protocol
 * upgrade to HTTP/2 or WebSocket based on what was requested.
 */
स्थिर CURLcode http_on_101_upgrade(संरचना Curl_easy *दत्त,
                                    ध्रुव वर्ण *buf, size_t blen,
                                    size_t *pconsumed,
                                    bool *conn_परिवर्तितम्)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना SingleRequest *k = &दत्त->req;

#यदि !defined(USE_NGHTTP2) && defined(CURL_DISABLE_WEBSOCKETS)
  (शून्य)buf;
  (शून्य)blen;
  (शून्य)pconsumed;
#अन्यथा
  CURLcode परिणाम;
  पूर्णाङ्क upgr101_requested = k->upgr101;
#यदि_अन्तः

  यदि(k->httpversion_प्रेषितम् != 11) {
    /* invalid for other HTTP versions */
    failf(दत्त, "server sent 101 response while not talking HTTP/1.1");
    प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  }

  /* Whatever the success, upgrade was selected. */
  k->upgr101 = UPGR101_RECEIVED;
  conn->द्वयङ्काः.upgrade_अन्तर्_प्रगतिः = FALSE;
  *conn_परिवर्तितम् = TRUE;

  /* To be fully compliant, we would check the "Upgrade:" response header to
   * mention the protocol we requested. */
#यदि_परिभाषितम् USE_NGHTTP2
  यदि(upgr101_requested == UPGR101_H2) {
    /* Switch to HTTP/2, where we will get more responses. blen bytes in buf
     * are already h2 protocol bytes */
    infof(दत्त, "Received 101, Switching to HTTP/2");
    परिणाम = Curl_http2_upgrade(दत्त, conn, FIRSTSOCKET, buf, blen);
    यदि(!परिणाम)
      *pconsumed += blen;
    प्रत्यागम परिणाम;
  }
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_WEBSOCKETS
  यदि(upgr101_requested == UPGR101_WS) {
    /* Switch to WebSocket, where we now stream ws frames. blen bytes in buf
     * are already ws protocol bytes */
    infof(दत्त, "Received 101, Switching to WebSocket");
    परिणाम = Curl_ws_accept(दत्त, buf, blen);
    यदि(!परिणाम)
      *pconsumed += blen; /* ws accept handled the data */
    प्रत्यागम परिणाम;
  }
#यदि_अन्तः
  /* We silently accept this as the final response. What are we switching to
   * if we did not ask for an Upgrade? Maybe the application provided an
   * `Upgrade: xxx` header? */
  k->शीर्षकम् = FALSE;
  प्रत्यागम CURLE_OK;
}

/*
 * Handle 1xx intermediate HTTP responses. Sets up state for more
 * headers and processes 100-continue and 101 upgrade responses.
 */
स्थिर CURLcode http_on_1xx_response(संरचना Curl_easy *दत्त,
                                     ध्रुव वर्ण *buf, size_t blen,
                                     size_t *pconsumed,
                                     bool *conn_परिवर्तितम्)
{
  संरचना SingleRequest *k = &दत्त->req;

  /* "A user agent MAY ignore unexpected 1xx status responses."
   * By default, we expect to get more responses after this one. */
  k->शीर्षकम् = TRUE;
  k->headerline = 0; /* restart the header line counter */

  विकल्प(k->httpcode) {
  पक्ष 100:
    /* We have made an HTTP PUT or POST and this is 1.1-lingo that tells us
     * that the server is OK with this and ready to receive the data. */
    http_exp100_got100(दत्त);
    भङ्ग;
  पक्ष 101:
    प्रत्यागम http_on_101_upgrade(दत्त, buf, blen, pconsumed, conn_परिवर्तितम्);
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
स्थिर शून्य http_check_auth_संवृतिः(संरचना Curl_easy *दत्त,
                                    संरचना connectdata *conn)
{
  /* At this point we have some idea about the fate of the connection. If we
     are closing the connection it may result auth failure. */
#यदि_परिभाषितम् USE_NTLM
  यदि(conn->द्वयङ्काः.close &&
     (((दत्त->req.httpcode == 401) &&
       (conn->http_ntlm_state == NTLMSTATE_TYPE2)) ||
      ((दत्त->req.httpcode == 407) &&
       (conn->proxy_ntlm_state == NTLMSTATE_TYPE2)))) {
    infof(दत्त, "Connection closure while negotiating auth (HTTP 1.0?)");
    दत्त->state.authproblem = TRUE;
  }
#यदि_अन्तः
#यदि_परिभाषितम् USE_SPNEGO
  यदि(conn->द्वयङ्काः.close &&
    (((दत्त->req.httpcode == 401) &&
      (conn->http_negotiate_state == CURL_NEGO_AUTH_RECV)) ||
     ((दत्त->req.httpcode == 407) &&
      (conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)))) {
    infof(दत्त, "Connection closure while negotiating auth (HTTP 1.0?)");
    दत्त->state.authproblem = TRUE;
  }
  यदि((conn->http_negotiate_state == CURL_NEGO_AUTH_समाप्तम्) &&
     (दत्त->req.httpcode != 401)) {
    conn->http_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
  यदि((conn->proxy_negotiate_state == CURL_NEGO_AUTH_समाप्तम्) &&
     (दत्त->req.httpcode != 407)) {
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
#यदि_अन्तः
}
#अन्यथा
#परिभाषय http_check_auth_संवृतिः(x, y) /*रिक्तानि*/
#यदि_अन्तः

/*
 * Handle an error response (>= 300) received while still sending the
 * request body. Deals with 417 Expectation Failed retries, keep-sending
 * on error, and aborting the send.
 */
स्थिर CURLcode http_handle_send_error(संरचना Curl_easy *दत्त)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना SingleRequest *k = &दत्त->req;
  CURLcode परिणाम = CURLE_OK;

  यदि(!दत्त->req.authneg && !conn->द्वयङ्काः.close &&
     !Curl_creader_will_rewind(दत्त)) {
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

    विकल्प(दत्त->state.httpreq) {
    पक्ष HTTPREQ_PUT:
    पक्ष HTTPREQ_POST:
    पक्ष HTTPREQ_POST_FORM:
    पक्ष HTTPREQ_POST_MIME:
      /* We got an error response. If this happened before the
       * whole request body has been sent we stop sending and
       * mark the connection for closure after we have read the
       * entire response. */
      यदि(!Curl_req_समाप्तम्_sending(दत्त)) {
        यदि((k->httpcode == 417) && http_exp100_is_selected(दत्त)) {
          /* 417 Expectation Failed - try again without the
             Expect header */
          यदि(!k->writebytecount && http_exp100_is_प्रतीक्षा(दत्त)) {
            infof(दत्त, "Got HTTP failure 417 while waiting for a 100");
          }
          अन्यथा {
            infof(दत्त, "Got HTTP failure 417 while sending data");
            streamclose(conn);
            परिणाम = http_perhapsrewind(दत्त, conn);
            यदि(परिणाम)
              प्रत्यागम परिणाम;
          }
          दत्त->state.disableexpect = TRUE;
          Curl_req_abort_sending(दत्त);
          DEBUGASSERT(!दत्त->req.newurl);
          दत्त->req.newurl = Curl_bufref_dup(&दत्त->state.url);
          यदि(!दत्त->req.newurl)
            प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        }
        अन्यथा यदि(दत्त->समुच्चयः.http_keep_sending_on_error) {
          infof(दत्त, "HTTP error before end of send, keep sending");
          http_exp100_send_anyway(दत्त);
        }
        अन्यथा {
          infof(दत्त, "HTTP error before end of send, stop sending");
          streamclose(conn);
          परिणाम = Curl_req_abort_sending(दत्त);
          यदि(परिणाम)
            प्रत्यागम परिणाम;
        }
      }
      भङ्ग;

    स्वभाव: /* default label present to avoid compiler warnings */
      भङ्ग;
    }
  }

  यदि(Curl_creader_will_rewind(दत्त) && !Curl_req_समाप्तम्_sending(दत्त)) {
    /* We rewind before next send, continue sending now */
    infof(दत्त, "Keep sending data to get tossed away");
    CURL_REQ_समुच्चयः_SEND(दत्त);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode http_on_response(संरचना Curl_easy *दत्त,
                                 ध्रुव वर्ण *चरमः_hd, size_t चरमः_hd_len,
                                 ध्रुव वर्ण *buf, size_t blen,
                                 size_t *pconsumed)
{
  संरचना connectdata *conn = दत्त->conn;
  CURLcode परिणाम = CURLE_OK;
  संरचना SingleRequest *k = &दत्त->req;
  bool conn_परिवर्तितम् = FALSE;

  (शून्य)buf; /* not used without HTTP2 enabled */
  *pconsumed = 0;

  यदि(k->upgr101 == UPGR101_RECEIVED) {
    /* supposedly upgraded to http2 now */
    यदि(दत्त->req.httpversion != 20)
      infof(दत्त, "Lying server, not serving HTTP/2");
  }

  यदि(k->httpcode < 200 && चरमः_hd) {
    /* Intermediate responses might trigger processing of more responses,
     * write the last header to the client before proceeding. */
    परिणाम = http_write_शीर्षकम्(दत्त, चरमः_hd, चरमः_hd_len);
    चरमः_hd = रिक्त; /* handled it */
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  यदि(k->httpcode < 100) {
    failf(दत्त, "Unsupported response code in HTTP response");
    परिणाम = CURLE_UNSUPPORTED_PROTOCOL;
    गच्छ बहिर्गमनम्;
  }
  अन्यथा यदि(k->httpcode < 200) {
    परिणाम = http_on_1xx_response(दत्त, buf, blen, pconsumed, &conn_परिवर्तितम्);
    गच्छ बहिर्गमनम्;
  }

  /* k->httpcode >= 200, final response */
  k->शीर्षकम् = FALSE;
  यदि(conn->द्वयङ्काः.upgrade_अन्तर्_प्रगतिः) {
    /* Asked for protocol upgrade, but it was not selected */
    conn->द्वयङ्काः.upgrade_अन्तर्_प्रगतिः = FALSE;
    conn_परिवर्तितम् = TRUE;
  }

  यदि((k->आकार == -1) && !k->chunk && !conn->द्वयङ्काः.close &&
     (k->httpversion == 11) &&
     !(conn->scheme->protocol & CURLPROTO_RTSP) &&
     दत्त->state.httpreq != HTTPREQ_शीर्ष) {
    /* On HTTP 1.1, when connection is not to get closed, but no
       Content-Length nor Transfer-Encoding chunked have been received,
       according to RFC2616 section 4.4 point 5, we assume that the server
       will close the connection to signal the end of the document. */
    infof(दत्त, "no chunk, no close, no size. Assume close to signal end");
    streamclose(conn);
  }

  http_check_auth_संवृतिः(दत्त, conn);

#यदि_अपरिभाषितम् CURL_DISABLE_WEBSOCKETS
  /* All >=200 HTTP status codes are errors when wanting ws */
  यदि(दत्त->req.upgr101 == UPGR101_WS) {
    failf(दत्त, "Refused WebSocket upgrade: %d", k->httpcode);
    परिणाम = CURLE_HTTP_RETURNED_ERROR;
    गच्छ बहिर्गमनम्;
  }
#यदि_अन्तः

  /* Check if this response means the transfer errored. */
  यदि(http_should_fail(दत्त, दत्त->req.httpcode)) {
    failf(दत्त, "The requested URL returned error: %d",
          k->httpcode);
    परिणाम = CURLE_HTTP_RETURNED_ERROR;
    गच्छ बहिर्गमनम्;
  }

  /* Curl_http_auth_act() checks what authentication methods that are
   * available and decides which one (if any) to use. It will set 'newurl' if
   * an auth method was picked. */
  परिणाम = Curl_http_auth_act(दत्त);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  यदि(k->httpcode >= 300) {
    परिणाम = http_handle_send_error(दत्त);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  /* final response without error, prepare to receive the body */
  परिणाम = http_firstwrite(दत्त);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /* This is the last response that we get for the current request. Check on
   * the body size and determine if the response is complete. */
  परिणाम = http_आकार(दत्त);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

#यदि_अपरिभाषितम् CURL_DISABLE_RTSP
  /* An RTSP response to any method may carry a body, which is announced
     with Content-Length (RFC 2326 section 12.14). Read it instead of
     treating it as excess data, unless the application asked for no body
     with CURLOPT_NOBODY. */
  यदि((conn->scheme->protocol & CURLPROTO_RTSP) && दत्त->req.no_body &&
     !दत्त->समुच्चयः.opt_no_body && (k->आकार > 0))
    दत्त->req.no_body = FALSE;
#यदि_अन्तः

  /* If we requested a "no body", this is a good time to get
   * out and return home.
   */
  यदि(दत्त->req.no_body)
    k->download_समाप्तम् = TRUE;

  /* If max download size is *zero* (nothing) we already have nothing and can
     safely return ok now! For HTTP/2, we would like to call
     http2_handle_stream_close to properly close a stream. In order to do
     this, we keep reading until we close the stream. */
  यदि((k->maxdownload == 0) && (k->httpversion_प्रेषितम् < 20))
    k->download_समाप्तम् = TRUE;

बहिर्गमनम्:
  यदि(चरमः_hd)
    /* if not written yet, write it now */
    परिणाम = Curl_1st_fatal(परिणाम,
                            http_write_शीर्षकम्(दत्त, चरमः_hd, चरमः_hd_len));
  यदि(conn_परिवर्तितम्)
    /* poke the multi handle to allow pending pipewait to retry */
    Curl_multi_connchanged(दत्त->multi);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode http_rw_hd(संरचना Curl_easy *दत्त,
                           ध्रुव वर्ण *hd, size_t hdlen,
                           ध्रुव वर्ण *buf_remain, size_t blen,
                           size_t *pconsumed)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना SingleRequest *k = &दत्त->req;
  पूर्णाङ्क writetype;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  *pconsumed = 0;
  यदि((0x0a == *hd) || (0x0d == *hd)) {
    /* Empty header line means end of headers! */
    संरचना dynbuf चरमः_शीर्षकम्;
    size_t consumed;

    curlx_dyn_init(&चरमः_शीर्षकम्, hdlen + 1);
    परिणाम = curlx_dyn_addn(&चरमः_शीर्षकम्, hd, hdlen);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    /* analyze the response to find out what to do. */
    /* Caveat: we clear anything in the header brigade, because a
     * response might switch HTTP version which may call use recursively.
     * Not nice, but that is currently the way of things. */
    curlx_dyn_reset(&दत्त->state.headerb);
    परिणाम = http_on_response(दत्त, curlx_dyn_ptr(&चरमः_शीर्षकम्),
                              curlx_dyn_len(&चरमः_शीर्षकम्),
                              buf_remain, blen, &consumed);
    *pconsumed += consumed;
    curlx_dyn_free(&चरमः_शीर्षकम्);
    प्रत्यागम परिणाम;
  }

  /*
   * Checks for special headers coming up.
   */

  writetype = CLIENTWRITE_शीर्षकम्;
  यदि(!k->headerline++) {
    /* This is the first header, it MUST be the error code line
       or else we consider this to be the body right away! */
    bool fine_statusline = FALSE;

    k->httpversion = 0; /* Do not know yet */
    यदि(दत्त->conn->scheme->protocol & PROTO_FAMILY_HTTP) {
      /*
       * https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.2
       *
       * The response code is always a three-digit number in HTTP as the spec
       * says. We allow any three-digit number here, but we cannot make
       * guarantees on future behaviors since it is not within the protocol.
       */
      ध्रुव वर्ण *p = hd;

      curlx_str_passblanks(&p);
      यदि(!वाक्ये_तुलय(p, "HTTP/", 5)) {
        p += 5;
        विकल्प(*p) {
        पक्ष '1':
          p++;
          यदि((p[0] == '.') && (p[1] == '0' || p[1] == '1')) {
            यदि(ISBLANK(p[2])) {
              k->httpversion = (अचिह्नित वर्ण)(10 + (p[1] - '0'));
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
            failf(दत्त, "Unsupported HTTP/1 subversion in response");
            प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
          }
          भङ्ग;
        पक्ष '2':
        पक्ष '3':
          यदि(!ISBLANK(p[1]))
            भङ्ग;
          k->httpversion = (अचिह्नित वर्ण)((*p - '0') * 10);
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
          failf(दत्त, "Unsupported HTTP version in response");
          प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
        }
      }

      यदि(!fine_statusline) {
        /* If user has set option HTTP200ALIASES,
           compare header line against list of aliases */
        statusline check = checkhttpprefix(दत्त, hd, hdlen);
        यदि(check == स्थितिः_समाप्तम्) {
          fine_statusline = TRUE;
          k->httpcode = 200;
          k->httpversion = 10;
        }
      }
    }
    अन्यथा यदि(दत्त->conn->scheme->protocol & CURLPROTO_RTSP) {
      ध्रुव वर्ण *p = hd;
      संरचना Curl_str ver;
      curl_off_t स्थितिः;
      /* we set the max string a little excessive to forgive some leading
         spaces */
      यदि(!curlx_str_until(&p, &ver, 32, ' ') &&
         !curlx_str_single(&p, ' ') &&
         !curlx_str_सङ्ख्या(&p, &स्थितिः, 999)) {
        curlx_str_trimblanks(&ver);
        यदि(curlx_str_cmp(&ver, "RTSP/1.0")) {
          k->httpcode = (पूर्णाङ्क)स्थितिः;
          fine_statusline = TRUE;
          k->httpversion = 11; /* RTSP acts like HTTP 1.1 */
        }
      }
      यदि(!fine_statusline)
        प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
    }

    यदि(fine_statusline) {
      परिणाम = http_statusline(दत्त, दत्त->conn);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      writetype |= CLIENTWRITE_स्थितिः;
    }
    अन्यथा {
      k->शीर्षकम् = FALSE;   /* this is not a header line */
      प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
    }
  }

  परिणाम = Curl_verify_शीर्षकम्(दत्त, hd, hdlen);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  परिणाम = http_शीर्षकम्(दत्त, hd, hdlen);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /*
   * Taken in one (more) header. Write it to the client.
   */
  Curl_debug(दत्त, CURLINFO_शीर्षकम्_अन्तर्, hd, hdlen);

  यदि(k->httpcode / 100 == 1)
    writetype |= CLIENTWRITE_1XX;
  परिणाम = Curl_client_write(दत्त, writetype, hd, hdlen);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  परिणाम = Curl_bump_headersize(दत्त, hdlen, FALSE);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  प्रत्यागम CURLE_OK;
}

/* remove trailing CRLF then all trailing whitespace */
शून्य Curl_http_गन्तव्यम्_fold(संरचना dynbuf *bf)
{
  size_t len = curlx_dyn_len(bf);
  ध्रुव वर्ण *hd = curlx_dyn_ptr(bf);
  यदि(len && (hd[len - 1] == '\n'))
    len--;
  यदि(len && (hd[len - 1] == '\r'))
    len--;
  यावत्(len && ISBLANK(hd[len - 1])) /* strip off trailing whitespace */
    len--;
  curlx_dyn_setlen(bf, len);
}

स्थिर शून्य unfold_शीर्षकम्(संरचना Curl_easy *दत्त)
{
  Curl_http_गन्तव्यम्_fold(&दत्त->state.headerb);
  दत्त->state.leading_unfold = TRUE;
}

/*
 * Read any HTTP header lines from the server and pass them to the client app.
 */
स्थिर CURLcode http_parse_शीर्षकाणि(संरचना Curl_easy *दत्त,
                                   ध्रुव वर्ण *buf, size_t blen,
                                   size_t *pconsumed)
{
  संरचना connectdata *conn = दत्त->conn;
  CURLcode परिणाम = CURLE_OK;
  संरचना SingleRequest *k = &दत्त->req;
  ध्रुव वर्ण *अन्तः_ptr;
  bool leftover_body = FALSE;

  /* we have bytes for the next header, make sure it is not a folded header
     before passing it on */
  यदि(दत्त->state.maybe_folded && blen) {
    यदि(ISBLANK(buf[0])) {
      /* folded, remove the trailing newlines and append the next header */
      unfold_शीर्षकम्(दत्त);
    }
    अन्यथा {
      /* the header data we hold is a complete header, pass it on */
      size_t ignore_this;
      परिणाम = http_rw_hd(दत्त, curlx_dyn_ptr(&दत्त->state.headerb),
                          curlx_dyn_len(&दत्त->state.headerb),
                          रिक्त, 0, &ignore_this);
      curlx_dyn_reset(&दत्त->state.headerb);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
    दत्त->state.maybe_folded = FALSE;
  }

  /* header line within buffer loop */
  *pconsumed = 0;
  यावत्(blen && k->शीर्षकम्) {
    size_t consumed;
    size_t hlen;
    ध्रुव वर्ण *hd;
    size_t unfold_len = 0;

    यदि(दत्त->state.leading_unfold) {
      /* immediately after an unfold, keep only a single whitespace */
      यावत्(blen && ISBLANK(buf[0])) {
        buf++;
        blen--;
        unfold_len++;
      }
      यदि(blen) {
        /* insert a single space */
        परिणाम = curlx_dyn_addn(&दत्त->state.headerb, " ", 1);
        यदि(परिणाम)
          प्रत्यागम परिणाम;
        दत्त->state.leading_unfold = FALSE; /* done now */
      }
    }

    अन्तः_ptr = memchr(buf, '\n', blen);
    यदि(!अन्तः_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      परिणाम = curlx_dyn_addn(&दत्त->state.headerb, buf, blen);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      *pconsumed += blen + unfold_len;

      यदि(!k->headerline) {
        /* check if this looks like a protocol header */
        statusline st =
          checkprotoprefix(दत्त, conn,
                           curlx_dyn_ptr(&दत्त->state.headerb),
                           curlx_dyn_len(&दत्त->state.headerb));

        यदि(st == स्थितिः_BAD) {
          /* this is not the beginning of a protocol first header line.
           * Cannot be 0.9 if version was detected or connection was reused. */
          k->शीर्षकम् = FALSE;
          streamclose(conn);
          यदि((k->httpversion >= 10) || conn->द्वयङ्काः.reuse) {
            failf(दत्त, "Invalid status line");
            प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
          }
          यदि(!दत्त->state.http_neg.accept_09) {
            failf(दत्त, "Received HTTP/0.9 when not allowed");
            प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
          }
          leftover_body = TRUE;
          गच्छ बहिर्गमनम्;
        }
      }
      गच्छ बहिर्गमनम्; /* read more and try again */
    }

    /* the size of the remaining header line */
    consumed = (अन्तः_ptr - buf) + 1;

    परिणाम = curlx_dyn_addn(&दत्त->state.headerb, buf, consumed);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    blen -= consumed;
    buf += consumed;
    *pconsumed += consumed + unfold_len;

    /****
     * We now have a FULL header line in 'headerb'.
     *****/

    hlen = curlx_dyn_len(&दत्त->state.headerb);
    hd = curlx_dyn_ptr(&दत्त->state.headerb);

    यदि(!k->headerline) {
      /* the first read "header", the status line */
      statusline st = checkprotoprefix(दत्त, conn, hd, hlen);
      यदि(st == स्थितिः_BAD) {
        streamclose(conn);
        /* this is not the beginning of a protocol first header line.
         * Cannot be 0.9 if version was detected or connection was reused. */
        यदि((k->httpversion >= 10) || conn->द्वयङ्काः.reuse) {
          failf(दत्त, "Invalid status line");
          प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
        }
        यदि(!दत्त->state.http_neg.accept_09) {
          failf(दत्त, "Received HTTP/0.9 when not allowed");
          प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
        }
        k->शीर्षकम् = FALSE;
        leftover_body = TRUE;
        गच्छ बहिर्गमनम्;
      }
    }
    अन्यथा {
      यदि(hlen && !ISNEWLINE(hd[0])) {
        /* this is NOT the header separator */

        /* if we have bytes for the next header, check for folding */
        यदि(blen && ISBLANK(buf[0])) {
          /* remove the trailing CRLF and append the next header */
          unfold_शीर्षकम्(दत्त);
          अनुवर्त;
        }
        अन्यथा यदि(!blen) {
          /* this might be a folded header so deal with it in next invoke */
          दत्त->state.maybe_folded = TRUE;
          भङ्ग;
        }
      }
    }

    परिणाम = http_rw_hd(दत्त, hd, hlen, buf, blen, &consumed);
    /* We are done with this line. We reset because response
     * processing might switch to HTTP/2 and that might call us
     * directly again. */
    curlx_dyn_reset(&दत्त->state.headerb);
    यदि(consumed) {
      blen -= consumed;
      buf += consumed;
      *pconsumed += consumed;
    }
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */
बहिर्गमनम्:
  यदि(!k->शीर्षकम् && !leftover_body) {
    curlx_dyn_free(&दत्त->state.headerb);
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_http_write_resp_hd(संरचना Curl_easy *दत्त,
                                 ध्रुव वर्ण *hd, size_t hdlen,
                                 bool is_eos)
{
  CURLcode परिणाम;
  size_t consumed;
  वर्ण tmp = 0;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  परिणाम = http_rw_hd(दत्त, hd, hdlen, &tmp, 0, &consumed);
  यदि(!परिणाम && is_eos) {
    परिणाम = Curl_client_write(दत्त, (CLIENTWRITE_BODY | CLIENTWRITE_EOS),
                               &tmp, 0);
  }
  प्रत्यागम परिणाम;
}

/*
 * HTTP protocol `write_resp` implementation. Parse headers
 * when not done yet and otherwise return without consuming data.
 */
CURLcode Curl_http_write_resp_hds(संरचना Curl_easy *दत्त,
                                  ध्रुव वर्ण *buf, size_t blen,
                                  size_t *pconsumed)
{
  यदि(!दत्त->req.शीर्षकम्) {
    *pconsumed = 0;
    प्रत्यागम CURLE_OK;
  }
  अन्यथा {
    CURLcode परिणाम;

    परिणाम = http_parse_शीर्षकाणि(दत्त, buf, blen, pconsumed);
    यदि(!परिणाम && !दत्त->req.शीर्षकम्) {
      यदि(!दत्त->req.no_body && curlx_dyn_len(&दत्त->state.headerb)) {
        /* leftover from parsing something that turned out not
         * to be a header, only happens if we allow for
         * HTTP/0.9 like responses */
        परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY,
                                   curlx_dyn_ptr(&दत्त->state.headerb),
                                   curlx_dyn_len(&दत्त->state.headerb));
      }
      curlx_dyn_free(&दत्त->state.headerb);
    }
    प्रत्यागम परिणाम;
  }
}

CURLcode Curl_http_write_resp(संरचना Curl_easy *दत्त,
                              ध्रुव वर्ण *buf, size_t blen,
                              bool is_eos)
{
  CURLcode परिणाम;
  size_t consumed;
  पूर्णाङ्क flags;

  परिणाम = Curl_http_write_resp_hds(दत्त, buf, blen, &consumed);
  यदि(परिणाम || दत्त->req.समाप्तम्)
    गच्छ बहिर्गमनम्;

  DEBUGASSERT(consumed <= blen);
  blen -= consumed;
  buf += consumed;
  /* either all was consumed in header parsing, or we have data left
   * and are done with headers, e.g. it is BODY data */
  DEBUGASSERT(!blen || !दत्त->req.शीर्षकम्);
  यदि(!दत्त->req.शीर्षकम् && (blen || is_eos)) {
    /* BODY data after header been parsed, write and consume */
    flags = CLIENTWRITE_BODY;
    यदि(is_eos)
      flags |= CLIENTWRITE_EOS;
    परिणाम = Curl_client_write(दत्त, flags, buf, blen);
  }
बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_स्थितिः(int32_t *pstatus, ध्रुव वर्ण *s, size_t len)
{
  CURLcode परिणाम = CURLE_BAD_FUNCTION_तर्कः;
  int32_t स्थितिः = 0;
  पूर्णाङ्क i;

  यदि(len != 3)
    गच्छ बहिर्गमनम्;

  प्रति(i = 0; i < 3; ++i) {
    वर्ण c = s[i];

    यदि(c < '0' || c > '9')
      गच्छ बहिर्गमनम्;

    स्थितिः *= 10;
    स्थितिः += c - '0';
  }
  परिणाम = CURLE_OK;
बहिर्गमनम्:
  *pstatus = परिणाम ? -1 : स्थितिः;
  प्रत्यागम परिणाम;
}

CURLcode Curl_http_req_make(संरचना httpreq **preq,
                            ध्रुव वर्ण *विधानम्, size_t m_len,
                            ध्रुव वर्ण *scheme, size_t s_len,
                            ध्रुव वर्ण *authority, size_t a_len,
                            ध्रुव वर्ण *मार्गः, size_t p_len)
{
  संरचना httpreq *req;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  DEBUGASSERT(विधानम् && m_len);

  req = curlx_calloc(1, परिमाण(*req) + m_len);
  यदि(!req)
    गच्छ बहिर्गमनम्;
#यदि defined(__GNUC__) && __GNUC__ >= 13
#निर्देशः GCC diagnostic प्रक्षिप
/* error: 'memcpy' offset [137, 142] from the object at 'req' is out of
   the bounds of referenced subobject 'method' with type 'char[1]' at
   offset 136 */
#निर्देशः GCC diagnostic ignored "-Warray-bounds"
#यदि_अन्तः
  स्मृतिं_प्रतिलिख(req->विधानम्, विधानम्, m_len);
#यदि defined(__GNUC__) && __GNUC__ >= 13
#निर्देशः GCC diagnostic निष्कासय
#यदि_अन्तः
  यदि(scheme) {
    req->scheme = curlx_memdup0(scheme, s_len);
    यदि(!req->scheme)
      गच्छ बहिर्गमनम्;
  }
  यदि(authority) {
    req->authority = curlx_memdup0(authority, a_len);
    यदि(!req->authority)
      गच्छ बहिर्गमनम्;
  }
  यदि(मार्गः) {
    req->मार्गः = curlx_memdup0(मार्गः, p_len);
    यदि(!req->मार्गः)
      गच्छ बहिर्गमनम्;
  }
  Curl_dynhds_init(&req->शीर्षकाणि, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  परिणाम = CURLE_OK;

बहिर्गमनम्:
  यदि(परिणाम && req)
    Curl_http_req_free(req);
  *preq = परिणाम ? रिक्त : req;
  प्रत्यागम परिणाम;
}

स्थिर CURLcode req_नियोजय_url_authority(संरचना httpreq *req, CURLU *url)
{
  वर्ण *host, *पत्तनम्;
  संरचना dynbuf buf;
  CURLUcode uc;
  CURLcode परिणाम = CURLE_URL_MALFORMAT;

  host = पत्तनम् = रिक्त;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_HOST, &host, 0);
  यदि(uc && uc != CURLUE_NO_HOST)
    गच्छ बहिर्गमनम्;
  यदि(!host) {
    req->authority = रिक्त;
    परिणाम = CURLE_OK;
    गच्छ बहिर्गमनम्;
  }

  uc = curl_url_get(url, CURLUPART_पत्तनम्, &पत्तनम्, CURLU_NO_DEFAULT_पत्तनम्);
  यदि(uc && uc != CURLUE_NO_पत्तनम्)
    गच्छ बहिर्गमनम्;

  परिणाम = curlx_dyn_सङ्कलय(&buf, host);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  यदि(पत्तनम्) {
    परिणाम = curlx_dyn_addf(&buf, ":%s", पत्तनम्);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  req->authority = curlx_dyn_ptr(&buf);
बहिर्गमनम्:
  curlx_free(host);
  curlx_free(पत्तनम्);
  यदि(परिणाम)
    curlx_dyn_free(&buf);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode req_नियोजय_url_मार्गः(संरचना httpreq *req, CURLU *url)
{
  वर्ण *मार्गः, *query;
  संरचना dynbuf buf;
  CURLUcode uc;
  CURLcode परिणाम = CURLE_URL_MALFORMAT;

  मार्गः = query = रिक्त;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_मार्गः, &मार्गः, 0);
  यदि(uc)
    गच्छ बहिर्गमनम्;
  uc = curl_url_get(url, CURLUPART_QUERY, &query, 0);
  यदि(uc && uc != CURLUE_NO_QUERY)
    गच्छ बहिर्गमनम्;

  यदि(!query) {
    req->मार्गः = मार्गः;
    मार्गः = रिक्त;
  }
  अन्यथा {
    परिणाम = curlx_dyn_सङ्कलय(&buf, मार्गः);
    यदि(!परिणाम)
      परिणाम = curlx_dyn_addf(&buf, "?%s", query);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    req->मार्गः = curlx_dyn_ptr(&buf);
  }
  परिणाम = CURLE_OK;

बहिर्गमनम्:
  curlx_free(मार्गः);
  curlx_free(query);
  यदि(परिणाम)
    curlx_dyn_free(&buf);
  प्रत्यागम परिणाम;
}

CURLcode Curl_http_req_make2(संरचना httpreq **preq,
                             ध्रुव वर्ण *विधानम्, size_t m_len,
                             CURLU *url, ध्रुव वर्ण *scheme_default)
{
  संरचना httpreq *req;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  CURLUcode uc;

  DEBUGASSERT(विधानम् && m_len);

  req = curlx_calloc(1, परिमाण(*req) + m_len);
  यदि(!req)
    गच्छ बहिर्गमनम्;
  स्मृतिं_प्रतिलिख(req->विधानम्, विधानम्, m_len);

  uc = curl_url_get(url, CURLUPART_SCHEME, &req->scheme, 0);
  यदि(uc && uc != CURLUE_NO_SCHEME)
    गच्छ बहिर्गमनम्;
  यदि(!req->scheme && scheme_default) {
    req->scheme = curlx_strdup(scheme_default);
    यदि(!req->scheme)
      गच्छ बहिर्गमनम्;
  }

  परिणाम = req_नियोजय_url_authority(req, url);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  परिणाम = req_नियोजय_url_मार्गः(req, url);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  Curl_dynhds_init(&req->शीर्षकाणि, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  परिणाम = CURLE_OK;

बहिर्गमनम्:
  यदि(परिणाम && req)
    Curl_http_req_free(req);
  *preq = परिणाम ? रिक्त : req;
  प्रत्यागम परिणाम;
}

शून्य Curl_http_req_free(संरचना httpreq *req)
{
  यदि(req) {
    curlx_free(req->scheme);
    curlx_free(req->authority);
    curlx_free(req->मार्गः);
    Curl_dynhds_free(&req->शीर्षकाणि);
    Curl_dynhds_free(&req->trailers);
    curlx_free(req);
  }
}

संरचना name_const {
  ध्रुव वर्ण *name;
  size_t namelen;
};

स्थिर ध्रुव संरचना name_const H2_NON_क्षेत्रम्[] = {
  { STRCONST("Host") },
  { STRCONST("Upgrade") },
  { STRCONST("Connection") },
  { STRCONST("Keep-Alive") },
  { STRCONST("Proxy-Connection") },
  { STRCONST("Transfer-Encoding") },
};

स्थिर bool h2_permissible_क्षेत्रम्(संरचना dynhds_प्रविष्टिः *e)
{
  size_t i;
  प्रति(i = 0; i < CURL_ARRAYSIZE(H2_NON_क्षेत्रम्); ++i) {
    यदि(e->namelen == H2_NON_क्षेत्रम्[i].namelen &&
       curl_strnequal(H2_NON_क्षेत्रम्[i].name, e->name, e->namelen))
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
        यदि(curlx_str_quotedword(&fvalue, &qw, CURL_MAX_HTTP_शीर्षकम्))
          प्रत्यागम FALSE;
      }
      अन्यथा
        fvalue++;
    }
  }
  प्रत्यागम FALSE;
}

CURLcode Curl_http_req_गन्तव्यम्_h2(संरचना dynhds *h2_शीर्षकाणि,
                             संरचना httpreq *req, संरचना Curl_easy *दत्त)
{
  ध्रुव वर्ण *scheme = रिक्त, *authority = रिक्त;
  संरचना dynhds_प्रविष्टिः *e;
  size_t i;
  CURLcode परिणाम;

  DEBUGASSERT(req);
  DEBUGASSERT(h2_शीर्षकाणि);

  यदि(req->scheme) {
    scheme = req->scheme;
  }
  अन्यथा यदि(वाक्यं_तुलय("CONNECT", req->विधानम्)) {
    scheme = Curl_checkheaders(दत्त, STRCONST(HTTP_PSEUDO_SCHEME));
    यदि(scheme) {
      scheme += परिमाण(HTTP_PSEUDO_SCHEME);
      curlx_str_passblanks(&scheme);
      infof(दत्त, "set pseudo header %s to %s", HTTP_PSEUDO_SCHEME, scheme);
    }
    अन्यथा {
      scheme = दत्त->state.origin->scheme->name;
    }
  }

  यदि(req->authority) {
    authority = req->authority;
  }
  अन्यथा {
    e = Curl_dynhds_get(&req->शीर्षकाणि, STRCONST("Host"));
    यदि(e)
      authority = e->मानम्;
  }

  Curl_dynhds_reset(h2_शीर्षकाणि);
  Curl_dynhds_समुच्चयः_opts(h2_शीर्षकाणि, DYNHDS_OPT_LOWERCASE);
  परिणाम = Curl_dynhds_सङ्कलय(h2_शीर्षकाणि, STRCONST(HTTP_PSEUDO_विधानम्),
                           req->विधानम्, वाक्यदैर्घ्यम्(req->विधानम्));
  यदि(!परिणाम && scheme) {
    परिणाम = Curl_dynhds_सङ्कलय(h2_शीर्षकाणि, STRCONST(HTTP_PSEUDO_SCHEME),
                             scheme, वाक्यदैर्घ्यम्(scheme));
  }
  यदि(!परिणाम && authority) {
    परिणाम = Curl_dynhds_सङ्कलय(h2_शीर्षकाणि, STRCONST(HTTP_PSEUDO_AUTHORITY),
                             authority, वाक्यदैर्घ्यम्(authority));
  }
  यदि(!परिणाम && req->मार्गः) {
    परिणाम = Curl_dynhds_सङ्कलय(h2_शीर्षकाणि, STRCONST(HTTP_PSEUDO_मार्गः),
                             req->मार्गः, वाक्यदैर्घ्यम्(req->मार्गः));
  }
  प्रति(i = 0; !परिणाम && i < Curl_dynhds_गणनम्(&req->शीर्षकाणि); ++i) {
    e = Curl_dynhds_getn(&req->शीर्षकाणि, i);
    /* "TE" is special in that it is only permissible when it
     * has only value "trailers". RFC 9113 ch. 8.2.2 */
    यदि(e->namelen == 2 && curl_strequal("TE", e->name)) {
      यदि(http_TE_has_token(e->मानम्, "trailers"))
        परिणाम = Curl_dynhds_सङ्कलय(h2_शीर्षकाणि, e->name, e->namelen,
                                 "trailers", CURL_CSTRLEN("trailers"));
    }
    अन्यथा यदि(h2_permissible_क्षेत्रम्(e)) {
      परिणाम = Curl_dynhds_सङ्कलय(h2_शीर्षकाणि, e->name, e->namelen,
                               e->मानम्, e->valuelen);
    }
  }

  प्रत्यागम परिणाम;
}

CURLcode Curl_http_resp_make(संरचना http_resp **presp,
                             पूर्णाङ्क स्थितिः,
                             ध्रुव वर्ण *description)
{
  संरचना http_resp *resp;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  resp = curlx_calloc(1, परिमाण(*resp));
  यदि(!resp)
    गच्छ बहिर्गमनम्;

  resp->स्थितिः = स्थितिः;
  यदि(description) {
    resp->description = curlx_strdup(description);
    यदि(!resp->description)
      गच्छ बहिर्गमनम्;
  }
  Curl_dynhds_init(&resp->शीर्षकाणि, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&resp->trailers, 0, DYN_HTTP_REQUEST);
  परिणाम = CURLE_OK;

बहिर्गमनम्:
  यदि(परिणाम && resp)
    Curl_http_resp_free(resp);
  *presp = परिणाम ? रिक्त : resp;
  प्रत्यागम परिणाम;
}

शून्य Curl_http_resp_free(संरचना http_resp *resp)
{
  यदि(resp) {
    curlx_free(resp->description);
    Curl_dynhds_free(&resp->शीर्षकाणि);
    Curl_dynhds_free(&resp->trailers);
    यदि(resp->पूर्व)
      Curl_http_resp_free(resp->पूर्व);
    curlx_free(resp);
  }
}

/*
 * HTTP handler interface.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_http = {
  Curl_http_सज्जता_conn,                 /* setup_connection */
  Curl_http,                            /* do_it */
  Curl_http_समाप्तम्,                       /* done */
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

#यदि_अन्तः /* CURL_DISABLE_HTTP */
