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
#समावेश "curl_setup.h"
#समावेश "urldata.h"

#अगर_अघोषित CURL_DISABLE_HTTP

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NET_IF_H
#समावेश <net/if.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_SYS_IOCTL_H
#समावेश <sys/ioctl.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_SYS_PARAM_H
#समावेश <sys/param.h>
#पूर्ण_अगर

#समावेश "transfer.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "formdata.h"
#समावेश "mime.h"
#समावेश "progress.h"
#समावेश "curlx/base64.h"
#समावेश "cookie.h"
#समावेश "vauth/vauth.h"
#समावेश "vquic/vquic.h"
#समावेश "http_digest.h"
#समावेश "http_ntlm.h"
#समावेश "http_negotiate.h"
#समावेश "http_aws_sigv4.h"
#समावेश "http_httpsig.h"
#समावेश "url.h"
#समावेश "urlapi-int.h"
#समावेश "curl_share.h"
#समावेश "dynhds.h"
#समावेश "http.h"
#समावेश "headers.h"
#समावेश "select.h"
#समावेश "parsedate.h" /* for the week day and month names */
#समावेश "multiif.h"
#समावेश "strcase.h"
#समावेश "content_encoding.h"
#समावेश "http_proxy.h"
#समावेश "http2.h"
#समावेश "cfilters.h"
#समावेश "connect.h"
#समावेश "curlx/strdup.h"
#समावेश "altsvc.h"
#समावेश "hsts.h"
#समावेश "rtsp.h"
#समावेश "ws.h"
#समावेश "bufref.h"
#समावेश "curlx/strparse.h"

व्योम Curl_http_neg_init(काष्ठा Curl_easy *तथ्य, काष्ठा http_negotiation *neg)
{
  स_रखो(neg, 0, माप(*neg));
  neg->accept_09 = तथ्य->set.http09_allowed;
  चयन(तथ्य->set.httpwant) {
  हाल CURL_HTTP_VERSION_1_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    neg->only_10 = TRUE;
    अवरोध;
  हाल CURL_HTTP_VERSION_1_1:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    अवरोध;
  हाल CURL_HTTP_VERSION_2_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->h2_upgrade = TRUE;
    अवरोध;
  हाल CURL_HTTP_VERSION_2TLS:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    अवरोध;
  हाल CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    neg->wanted = neg->allowed = (CURL_HTTP_V2x);
    तथ्य->state.http_neg.h2_prior_knowledge = TRUE;
    अवरोध;
  हाल CURL_HTTP_VERSION_3:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    neg->allowed = neg->wanted;
    अवरोध;
  हाल CURL_HTTP_VERSION_3ONLY:
    neg->wanted = neg->allowed = (CURL_HTTP_V3x);
    अवरोध;
  हाल CURL_HTTP_VERSION_NONE:
  शेष:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    अवरोध;
  }
}

CURLcode Curl_http_setup_conn(काष्ठा Curl_easy *तथ्य,
                              काष्ठा connectdata *conn)
{
  /* allocate the HTTP-specific struct for the Curl_easy, only to survive
     during this request */
  अगर(तथ्य->state.http_neg.wanted == CURL_HTTP_V3x) {
    /* only HTTP/3, needs to work */
    CURLcode परिणाम = Curl_conn_may_http3(तथ्य, conn);
    अगर(परिणाम)
      वापस परिणाम;
  }
  वापस CURLE_OK;
}

#अगर_अघोषित CURL_DISABLE_PROXY
/*
 * checkProxyHeaders() checks the linked list of custom proxy headers
 * if proxy headers are not available, then it will lookup into http header
 * link list
 *
 * It takes a connectdata struct as input to see if this is a proxy request or
 * not, as it then might check a different header list. Provide the header
 * prefix without colon!
 */
अक्षर *Curl_checkProxyheaders(काष्ठा Curl_easy *तथ्य,
                             स्थिर काष्ठा connectdata *conn,
                             स्थिर अक्षर *thisheader,
                             स्थिर माप_प्रकार thislen)
{
  काष्ठा curl_slist *शीर्ष;

  क्रम(शीर्ष = (conn->http_proxy.peer && तथ्य->set.sep_headers) ?
        तथ्य->set.proxyheaders : तथ्य->set.headers;
      शीर्ष; शीर्ष = शीर्ष->अग्र) {
    अगर(curl_strnequal(शीर्ष->तथ्य, thisheader, thislen) &&
       Curl_headersep(शीर्ष->तथ्य[thislen]))
      वापस शीर्ष->तथ्य;
  }

  वापस शून्य;
}
#पूर्ण_अगर

/* If the header has a value, this function returns TRUE and the value is in
   'outp' with blanks trimmed off. */
अटल bool header_has_मूल्य(स्थिर अक्षर **headerp, काष्ठा Curl_str *outp)
{
  bool मूल्य = !curlx_str_cspn(headerp, outp, ";:") &&
    (!curlx_str_single(headerp, ':') || !curlx_str_single(headerp, ';'));

  अगर(मूल्य) {
    curlx_str_cspn(headerp, outp, "\r\n");
    curlx_str_trimblanks(outp);
  }
  वापस मूल्य;
}

अटल bool http_header_is_empty(स्थिर अक्षर *header)
{
  काष्ठा Curl_str out;

  अगर(header_has_मूल्य(&header, &out)) {
    वापस curlx_strlen(&out) == 0;
  }
  वापस TRUE; /* invalid header format, treat as empty */
}

/*
 * Strip off leading and trailing whitespace from the value in the given HTTP
 * header line and return a strdup-ed copy in 'valp' - returns an empty
 * string if the header value consists entirely of whitespace.
 *
 * If the header is provided as "name;", ending with a semicolon, it returns a
 * blank string.
 */
अटल CURLcode copy_custom_मूल्य(स्थिर अक्षर *header, अक्षर **valp)
{
  काष्ठा Curl_str out = { 0 };

  /* find the end of the header name */
  अगर(header_has_मूल्य(&header, &out)) {
    *valp = curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
    अगर(*valp)
      वापस CURLE_OK;
    वापस CURLE_OUT_OF_MEMORY;
  }
  /* bad input */
  *valp = शून्य;
  वापस CURLE_BAD_FUNCTION_ARGUMENT;
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
अक्षर *Curl_copy_header_मूल्य(स्थिर अक्षर *header)
{
  काष्ठा Curl_str out;

  /* find the end of the header name */
  अगर(!curlx_str_until(&header, &out, MAX_HTTP_RESP_HEADER_आकार, ':') &&
     !curlx_str_single(&header, ':')) {
    curlx_str_untilnl(&header, &out, MAX_HTTP_RESP_HEADER_आकार);
    curlx_str_trimblanks(&out);
    वापस curlx_memdup0(curlx_str(&out), curlx_strlen(&out));
  }
  /* bad input, should never happen */
  DEBUGASSERT(0);
  वापस शून्य;
}

#अगर_अघोषित CURL_DISABLE_HTTP_AUTH

#अगर_अघोषित CURL_DISABLE_BASIC_AUTH
/*
 * http_output_basic() sets up an Authorization: header (or the proxy version)
 * for HTTP Basic authentication.
 *
 * Returns CURLcode.
 */
अटल CURLcode http_output_basic(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata *conn, bool proxy)
{
  माप_प्रकार आकार = 0;
  अक्षर *authorization = शून्य;
  अक्षर **p_hd;
  CURLcode परिणाम;
  काष्ठा Curl_creds *creds = शून्य;
  अक्षर *out;

  /* credentials are unique per transfer for HTTP, do not use the ones for the
     connection */
  अगर(proxy) {
#अगर_अघोषित CURL_DISABLE_PROXY
    p_hd = &तथ्य->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
#अन्यथा
    (व्योम)conn;
    वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
  }
  अन्यथा {
    p_hd = &तथ्य->req.hd_auth;
    creds = तथ्य->state.creds;
  }

  अगर(!creds) {
    DEBUGASSERT(0);
    वापस CURLE_FAILED_INIT;
  }

  out = curl_maprintf("%s:%s", creds->user, creds->passwd);
  अगर(!out)
    वापस CURLE_OUT_OF_MEMORY;

  परिणाम = curlx_base64_encode((uint8_t *)out, म_माप(out),
                               &authorization, &आकार);
  अगर(परिणाम)
    जाओ fail;

  अगर(!authorization) {
    परिणाम = CURLE_REMOTE_ACCESS_DENIED;
    जाओ fail;
  }

  curlx_free(*p_hd);
  *p_hd = curl_maprintf("%sAuthorization: Basic %s\r\n",
                        proxy ? "Proxy-" : "",
                        authorization);
  curlx_free(authorization);
  अगर(!*p_hd) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ fail;
  }

fail:
  curlx_free(out);
  वापस परिणाम;
}

#पूर्ण_अगर

#अगर_अघोषित CURL_DISABLE_BEARER_AUTH
/*
 * http_output_bearer() sets up an Authorization: header
 * for HTTP Bearer authentication.
 *
 * Returns CURLcode.
 */
अटल CURLcode http_output_bearer(काष्ठा Curl_easy *तथ्य)
{
  अक्षर **userp;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(Curl_creds_has_oauth_bearer(तथ्य->state.creds));
  userp = &तथ्य->req.hd_auth;
  curlx_free(*userp);
  *userp = curl_maprintf("Authorization: Bearer %s\r\n",
                         Curl_creds_oauth_bearer(तथ्य->state.creds));

  अगर(!*userp) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ fail;
  }

fail:
  वापस परिणाम;
}
#पूर्ण_अगर

#पूर्ण_अगर

/* pickoneauth() selects the most favorable authentication method from the
 * ones available and the ones we want.
 *
 * return TRUE if one was picked
 */
अटल bool pickoneauth(काष्ठा auth *pick, अचिन्हित दीर्घ mask,
                        काष्ठा Curl_creds *creds)
{
  bool have_user_pass = Curl_creds_has_user_or_pass(creds);
  bool picked;
  /* only deal with authentication we want */
  अचिन्हित दीर्घ avail = pick->avail & pick->want & mask;
  picked = TRUE;

  /* The order of these checks is highly relevant, as this will be the order
     of preference in case of the existence of multiple accepted types. */
  अगर(avail & CURLAUTH_NEGOTIATE)  /* available on empty creds */
    pick->picked = CURLAUTH_NEGOTIATE;
#अगर_अघोषित CURL_DISABLE_BEARER_AUTH
  अन्यथा अगर((avail & CURLAUTH_BEARER) && Curl_creds_has_oauth_bearer(creds))
    pick->picked = CURLAUTH_BEARER;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH
  अन्यथा अगर((avail & CURLAUTH_DIGEST) && have_user_pass)
    pick->picked = CURLAUTH_DIGEST;
#पूर्ण_अगर
  अन्यथा अगर(avail & CURLAUTH_NTLM)
    pick->picked = CURLAUTH_NTLM;
#अगर_अघोषित CURL_DISABLE_BASIC_AUTH
  अन्यथा अगर((avail & CURLAUTH_BASIC) && have_user_pass)
    pick->picked = CURLAUTH_BASIC;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_AWS
  अन्यथा अगर(avail & CURLAUTH_AWS_SIGV4)
    pick->picked = CURLAUTH_AWS_SIGV4;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HTTPSIG
  अन्यथा अगर(avail & CURLAUTH_HTTPSIG)
    pick->picked = CURLAUTH_HTTPSIG;
#पूर्ण_अगर
  अन्यथा {
    pick->picked = CURLAUTH_PICKNONE; /* we select to use nothing */
    picked = FALSE;
  }
  pick->avail = CURLAUTH_NONE; /* clear it here */

  वापस picked;
}

/*
 * http_perhapsrewind()
 *
 * The current request needs to be done again - maybe due to a follow
 * or authentication negotiation. Check if:
 * 1) a rewind of the data sent to the server is necessary
 * 2) the current transfer should continue or be stopped early
 */
अटल CURLcode http_perhapsrewind(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा connectdata *conn)
{
  curl_off_t bytessent = तथ्य->req.writebytecount;
  curl_off_t expectsend = Curl_creader_योग_लंबाई(तथ्य);
  curl_off_t upload_remain = (expectsend >= 0) ? (expectsend - bytessent) : -1;
  bool little_upload_remains = (upload_remain >= 0 && upload_remain < 2000);
  bool needs_rewind = Curl_creader_needs_rewind(तथ्य);
  /* By default, we would like to abort the transfer when little or unknown
   * amount remains. This may be overridden by authentications further
   * below! */
  bool abort_upload = (!तथ्य->req.upload_done && !little_upload_remains);
  VERBOSE(स्थिर अक्षर *ongoing_auth = शून्य);

  /* We need a rewind before uploading client read data again. The
   * checks below influence of the upload is to be continued
   * or aborted early.
   * This depends on how much remains to be sent and in what state
   * the authentication is. Some auth schemes such as NTLM do not work
   * for a new connection. */
  अगर(needs_rewind) {
    infof(तथ्य, "Need to rewind upload for next request");
    Curl_creader_set_rewind(तथ्य, TRUE);
  }

  अगर(conn->bits.बंद)
    /* If we already decided to close this connection, we cannot veto. */
    वापस CURLE_OK;

  अगर(abort_upload) {
    /* We would like to abort the upload - but should we? */
#अगर_घोषित USE_NTLM
    अगर((तथ्य->state.authproxy.picked == CURLAUTH_NTLM) ||
       (तथ्य->state.authhost.picked == CURLAUTH_NTLM)) {
      VERBOSE(ongoing_auth = "NTLM");
      अगर((conn->http_ntlm_state != NTLMSTATE_NONE) ||
         (conn->proxy_ntlm_state != NTLMSTATE_NONE)) {
        /* The NTLM-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#पूर्ण_अगर
#अगर_घोषित USE_SPNEGO
    /* There is still data left to send */
    अगर((तथ्य->state.authproxy.picked == CURLAUTH_NEGOTIATE) ||
       (तथ्य->state.authhost.picked == CURLAUTH_NEGOTIATE)) {
      VERBOSE(ongoing_auth = "NEGOTIATE");
      अगर((conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) ||
         (conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE)) {
        /* The NEGOTIATE-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#पूर्ण_अगर
  }

  अगर(abort_upload) {
    अगर(upload_remain >= 0)
      infof(तथ्य, "%s%sclose instead of sending %" FMT_OFF_T " more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "",
            upload_remain);
    अन्यथा
      infof(तथ्य, "%s%sclose instead of sending unknown amount "
            "of more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "");
    /* We decided to abort the ongoing transfer */
    streamclose(conn);
    तथ्य->req.आकार = 0; /* do not download any more than 0 bytes */
    तथ्य->req.http_bodyless = TRUE;
  }
  वापस CURLE_OK;
}

/**
 * http_should_fail() determines whether an HTTP response code has gotten us
 * into an error state or not.
 *
 * @retval FALSE communications should continue
 *
 * @retval TRUE communications should not continue
 */
अटल bool http_should_fail(काष्ठा Curl_easy *तथ्य, पूर्णांक httpcode)
{
  DEBUGASSERT(तथ्य);
  DEBUGASSERT(तथ्य->conn);

  /*
   * If we have not been asked to fail on error,
   * do not fail.
   */
  अगर(!तथ्य->set.http_fail_on_error)
    वापस FALSE;

  /*
   * Any code < 400 is never terminal.
   */
  अगर(httpcode < 400)
    वापस FALSE;

  /*
   * A 416 response to a resume request is presumably because the file is
   * already completely downloaded and thus not actually a fail.
   */
  अगर(तथ्य->state.resume_from && तथ्य->state.httpreq == HTTPREQ_GET &&
     httpcode == 416)
    वापस FALSE;

  /*
   * Any code >= 400 that is not 401 or 407 is always
   * a terminal error
   */
  अगर((httpcode != 401) && (httpcode != 407))
    वापस TRUE;

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
  अगर((httpcode == 401) && !तथ्य->state.creds)
    वापस TRUE;
#अगर_अघोषित CURL_DISABLE_PROXY
  अगर((httpcode == 407) && !तथ्य->conn->http_proxy.creds)
    वापस TRUE;
#पूर्ण_अगर

  वापस (bool)तथ्य->state.authproblem;
}

/*
 * Curl_http_auth_act() gets called when all HTTP headers have been received
 * and it checks what authentication methods that are available and decides
 * which one (if any) to use. It will set 'newurl' if an auth method was
 * picked.
 */
CURLcode Curl_http_auth_act(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  bool pickhost = FALSE;
  bool pickproxy = FALSE;
  CURLcode परिणाम = CURLE_OK;
  अचिन्हित दीर्घ authmask = ~0UL;

  अगर(!Curl_creds_has_oauth_bearer(तथ्य->state.creds))
    authmask &= (अचिन्हित दीर्घ)~CURLAUTH_BEARER;

  अगर(100 <= तथ्य->req.httpcode && तथ्य->req.httpcode <= 199)
    /* this is a transient response code, ignore */
    वापस CURLE_OK;

  अगर(तथ्य->state.authproblem)
    वापस तथ्य->set.http_fail_on_error ? CURLE_HTTP_RETURNED_ERROR : CURLE_OK;

  अगर(तथ्य->state.creds &&
     ((तथ्य->req.httpcode == 401) ||
      (तथ्य->req.authneg && तथ्य->req.httpcode < 300))) {
    pickhost = pickoneauth(&तथ्य->state.authhost, authmask, तथ्य->state.creds);
    अगर(!pickhost)
      तथ्य->state.authproblem = TRUE;
    अन्यथा
      तथ्य->info.httpauthpicked = तथ्य->state.authhost.picked;
    अगर(तथ्य->state.authhost.picked == CURLAUTH_NTLM &&
       (तथ्य->req.httpversion_sent > 11)) {
      infof(तथ्य, "Forcing HTTP/1.1 for NTLM");
      connclose(conn);
      तथ्य->state.http_neg.wanted = CURL_HTTP_V1x;
      तथ्य->state.http_neg.allowed = CURL_HTTP_V1x;
    }
  }
#अगर_अघोषित CURL_DISABLE_PROXY
  अगर(conn->http_proxy.creds &&
     ((तथ्य->req.httpcode == 407) ||
      (तथ्य->req.authneg && तथ्य->req.httpcode < 300))) {
    pickproxy = pickoneauth(&तथ्य->state.authproxy,
                            authmask & ~CURLAUTH_BEARER,
                            conn->http_proxy.creds);
    अगर(!pickproxy)
      तथ्य->state.authproblem = TRUE;
    अन्यथा
      तथ्य->info.proxyauthpicked = तथ्य->state.authproxy.picked;
  }
#पूर्ण_अगर

  अगर(pickhost || pickproxy) {
    परिणाम = http_perhapsrewind(तथ्य, conn);
    अगर(परिणाम)
      वापस परिणाम;

    /* In case this is GSS auth, the newurl field is already allocated so
       we must make sure to free it before allocating a new one. As figured
       out in bug #2284386 */
    curlx_free(तथ्य->req.newurl);
    /* clone URL */
    तथ्य->req.newurl = Curl_bufref_dup(&तथ्य->state.url);
    अगर(!तथ्य->req.newurl)
      वापस CURLE_OUT_OF_MEMORY;
  }
  अन्यथा अगर((तथ्य->req.httpcode < 300) &&
          !तथ्य->state.authhost.done &&
          तथ्य->req.authneg &&
          /* no (known) authentication available,
             authentication is not "done" yet and
             no authentication seems to be required and
             we did not try HEAD or GET */
          (तथ्य->state.httpreq != HTTPREQ_GET) &&
          (तथ्य->state.httpreq != HTTPREQ_शीर्ष)) {
    /* clone URL */
    तथ्य->req.newurl = Curl_bufref_dup(&तथ्य->state.url);
    अगर(!तथ्य->req.newurl)
      वापस CURLE_OUT_OF_MEMORY;
    तथ्य->state.authhost.done = TRUE;
  }
  अगर(http_should_fail(तथ्य, तथ्य->req.httpcode)) {
    failf(तथ्य, "The requested URL returned error: %d",
          तथ्य->req.httpcode);
    परिणाम = CURLE_HTTP_RETURNED_ERROR;
  }

  वापस परिणाम;
}

#अगर_अघोषित CURL_DISABLE_HTTP_AUTH
/*
 * Output the correct authentication header depending on the auth type
 * and whether or not it is to a proxy.
 */
अटल CURLcode output_auth_headers(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा connectdata *conn,
                                    काष्ठा auth *authstatus,
                                    स्थिर अक्षर *request,
                                    स्थिर अक्षर *path,
                                    bool proxy)
{
  स्थिर अक्षर *auth = शून्य;
  CURLcode परिणाम = CURLE_OK;
  (व्योम)conn;

#अगर_घोषित CURL_DISABLE_DIGEST_AUTH
  (व्योम)request;
  (व्योम)path;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_AWS
  अगर((authstatus->picked == CURLAUTH_AWS_SIGV4) && !proxy) {
    /* this method is never for proxy */
    auth = "AWS_SIGV4";
    परिणाम = Curl_output_aws_sigv4(तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
  }
  अन्यथा
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HTTPSIG
  अगर((authstatus->picked == CURLAUTH_HTTPSIG) && !proxy) {
    /* HTTPSIG uses its own configured key material rather than
       data->state.creds. Do not let unrelated credentials from a
       redirected URL bypass the cross-host auth boundary. */
    अगर(Curl_auth_allowed_to_host(तथ्य)) {
      auth = "HTTPSIG";
      परिणाम = Curl_output_httpsig(तथ्य);
      अगर(परिणाम)
        वापस परिणाम;
    }
    अन्यथा
      authstatus->done = TRUE;
  }
  अन्यथा
#पूर्ण_अगर
#अगर_घोषित USE_SPNEGO
  अगर(authstatus->picked == CURLAUTH_NEGOTIATE) {
    अगर(
#अगर_अघोषित CURL_DISABLE_PROXY
      (proxy && !Curl_checkProxyheaders(तथ्य, conn,
                                        STRCONST("Proxy-authorization"))) ||
#पूर्ण_अगर
      (!proxy && !Curl_checkheaders(तथ्य, STRCONST("Authorization")) &&
       Curl_auth_allowed_to_host(तथ्य))) {
      auth = "Negotiate";
      परिणाम = Curl_output_negotiate(तथ्य, conn, proxy);
      अगर(परिणाम)
        वापस परिणाम;
    }
    अन्यथा
      authstatus->done = TRUE;
  }
  अन्यथा
#पूर्ण_अगर
#अगर_घोषित USE_NTLM
  अगर(authstatus->picked == CURLAUTH_NTLM) {
    अगर(proxy || Curl_auth_allowed_to_host(तथ्य)) {
      auth = "NTLM";
      परिणाम = Curl_output_ntlm(तथ्य, proxy);
      अगर(परिणाम)
        वापस परिणाम;
    }
    अन्यथा
      authstatus->done = TRUE;
  }
  अन्यथा
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH
  अगर(authstatus->picked == CURLAUTH_DIGEST) {
    auth = "Digest";
    परिणाम = Curl_output_digest(तथ्य,
                                proxy,
                                (स्थिर अचिन्हित अक्षर *)request,
                                (स्थिर अचिन्हित अक्षर *)path);
    अगर(परिणाम)
      वापस परिणाम;
  }
  अन्यथा
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_BASIC_AUTH
  अगर(authstatus->picked == CURLAUTH_BASIC) {
    /* Basic */
    अगर(
#अगर_अघोषित CURL_DISABLE_PROXY
       (proxy && conn->http_proxy.creds &&
        Curl_creds_has_user_or_pass(conn->http_proxy.creds) &&
        !Curl_checkProxyheaders(तथ्य, conn,
                                STRCONST("Proxy-authorization"))) ||
#पूर्ण_अगर
       (!proxy && तथ्य->state.creds &&
        Curl_creds_has_user_or_pass(तथ्य->state.creds) &&
        !Curl_checkheaders(तथ्य, STRCONST("Authorization")))) {
      auth = "Basic";
      परिणाम = http_output_basic(तथ्य, conn, proxy);
      अगर(परिणाम)
        वापस परिणाम;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_BEARER_AUTH
  अगर(authstatus->picked == CURLAUTH_BEARER) {
    /* Bearer */
    अगर(!proxy && Curl_creds_has_oauth_bearer(तथ्य->state.creds) &&
       !Curl_checkheaders(तथ्य, STRCONST("Authorization"))) {
      auth = "Bearer";
      परिणाम = http_output_bearer(तथ्य);
      अगर(परिणाम)
        वापस परिणाम;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->done = TRUE;
  }
#पूर्ण_अगर

  अगर(auth) {
#अगर_अघोषित CURL_DISABLE_PROXY
    अगर(proxy)
      तथ्य->info.proxyauthpicked = authstatus->picked;
    अन्यथा
      तथ्य->info.httpauthpicked = authstatus->picked;
    infof(तथ्य, "%s auth using %s with user '%s'",
          proxy ? "Proxy" : "Server", auth,
          proxy ? (conn->http_proxy.creds ?
                   conn->http_proxy.creds->user : "") :
          (तथ्य->state.creds ?
           तथ्य->state.creds->user : ""));
#अन्यथा
    (व्योम)proxy;
    infof(तथ्य, "Server auth using %s with user '%s'",
          auth, तथ्य->state.creds ?
          तथ्य->state.creds->user : "");
#पूर्ण_अगर
    authstatus->multipass = !authstatus->done;
  }
  अन्यथा {
    authstatus->multipass = FALSE;
    अगर(proxy)
      तथ्य->info.proxyauthpicked = 0;
    अन्यथा
      तथ्य->info.httpauthpicked = 0;
  }

  वापस परिणाम;
}

CURLcode Curl_http_output_auth(काष्ठा Curl_easy *तथ्य,
                               काष्ठा connectdata *conn,
                               स्थिर अक्षर *request,
                               Curl_HttpReq httpreq,
                               स्थिर अक्षर *path,
                               स्थिर अक्षर *query,
                               bool is_connect)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा auth *authhost;
  काष्ठा auth *authproxy;
  स्थिर अक्षर *path_and_query = path;
  अक्षर *tmp_str = शून्य;

  DEBUGASSERT(तथ्य);
  authhost = &तथ्य->state.authhost;
  authproxy = &तथ्य->state.authproxy;

  अगर(
#अगर_अघोषित CURL_DISABLE_PROXY
    (!conn->http_proxy.peer || !conn->http_proxy.creds) &&
#पूर्ण_अगर
#अगर_घोषित USE_SPNEGO
    !(authhost->want & CURLAUTH_NEGOTIATE) &&
    !(authproxy->want & CURLAUTH_NEGOTIATE) &&
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HTTPSIG
    !(authhost->want & CURLAUTH_HTTPSIG) &&
#पूर्ण_अगर
    !तथ्य->state.creds) {
    /* no authentication with no user or password */
    authhost->done = TRUE;
    authproxy->done = TRUE;
    परिणाम = CURLE_OK;
    जाओ out;
  }

  अगर(query) {
    tmp_str = curl_maprintf("%s?%s", path, query);
    अगर(!tmp_str) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
    path_and_query = tmp_str;
  }

  अगर(authhost->want && !authhost->picked)
    /* The app has selected one or more methods, but none has been picked
       so far by a server round-trip. Then we set the picked one to the
       want one, and if this is one single bit it will be used instantly. */
    authhost->picked = authhost->want;

  अगर(authproxy->want && !authproxy->picked)
    /* The app has selected one or more methods, but none has been picked so
       far by a proxy round-trip. Then we set the picked one to the want one,
       and if this is one single bit it will be used instantly. */
    authproxy->picked = authproxy->want;

#अगर_अघोषित CURL_DISABLE_PROXY
  /* Send proxy authentication header if needed */
  अगर(conn->bits.origin_is_proxy || is_connect) {
    परिणाम = output_auth_headers(तथ्य, conn, authproxy, request,
                                 path_and_query, TRUE);
    अगर(परिणाम)
      जाओ out;
  }
  अन्यथा
#अन्यथा
  (व्योम)is_connect;
#पूर्ण_अगर /* CURL_DISABLE_PROXY */
    /* we have no proxy so let's pretend we are done authenticating
       with it */
    authproxy->done = TRUE;

  /* Either we have credentials for the origin we talk to or
     performing authentication is allowed here */
  अगर(तथ्य->state.creds || Curl_auth_allowed_to_host(तथ्य))
    परिणाम = output_auth_headers(तथ्य, conn, authhost, request,
                                 path_and_query, FALSE);
  अन्यथा
    authhost->done = TRUE;

  अगर(((authhost->multipass && !authhost->done) ||
      (authproxy->multipass && !authproxy->done)) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_शीर्ष)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    तथ्य->req.authneg = TRUE;
  }
  अन्यथा
    तथ्य->req.authneg = FALSE;

out:
  curlx_free(tmp_str);
  वापस परिणाम;
}

#अन्यथा /* !CURL_DISABLE_HTTP_AUTH */
/* when disabled */
CURLcode Curl_http_output_auth(काष्ठा Curl_easy *तथ्य,
                               काष्ठा connectdata *conn,
                               स्थिर अक्षर *request,
                               Curl_HttpReq httpreq,
                               स्थिर अक्षर *path,
                               स्थिर अक्षर *query,
                               bool is_connect)
{
  (व्योम)तथ्य;
  (व्योम)conn;
  (व्योम)request;
  (व्योम)httpreq;
  (व्योम)path;
  (व्योम)query;
  (व्योम)is_connect;
  वापस CURLE_OK;
}
#पूर्ण_अगर /* !CURL_DISABLE_HTTP_AUTH, else */

#अगर defined(USE_SPNEGO) || defined(USE_NTLM) || \
  !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_BASIC_AUTH) || \
  !defined(CURL_DISABLE_BEARER_AUTH)
अटल bool authcmp(स्थिर अक्षर *auth, स्थिर अक्षर *पंक्ति)
{
  /* the auth string must not have an alnum following */
  माप_प्रकार n = म_माप(auth);
  वापस curl_strnequal(auth, पंक्ति, n) && !ISALNUM(पंक्ति[n]);
}
#पूर्ण_अगर

#अगर_घोषित USE_SPNEGO
अटल CURLcode auth_spnego(काष्ठा Curl_easy *तथ्य,
                            bool proxy,
                            स्थिर अक्षर *auth,
                            काष्ठा auth *authp,
                            uint32_t *availp)
{
  अगर((authp->avail & CURLAUTH_NEGOTIATE) || Curl_auth_is_spnego_supported()) {
    *availp |= CURLAUTH_NEGOTIATE;
    authp->avail |= CURLAUTH_NEGOTIATE;

    अगर(authp->picked == CURLAUTH_NEGOTIATE) {
      काष्ठा connectdata *conn = तथ्य->conn;
      CURLcode परिणाम = Curl_निवेश_negotiate(तथ्य, conn, proxy, auth);
      uint8_t *negstate = proxy ? &conn->proxy_negotiate_state :
        &conn->http_negotiate_state;
      अगर(!परिणाम) {
        curlx_free(तथ्य->req.newurl);
        तथ्य->req.newurl = Curl_bufref_dup(&तथ्य->state.url);
        अगर(!तथ्य->req.newurl)
          वापस CURLE_OUT_OF_MEMORY;
        तथ्य->state.authproblem = FALSE;
        /* we received a GSS auth token and we dealt with it fine */
        *negstate = CURL_NEGO_AUTH_RECV;
      }
      अन्यथा
        तथ्य->state.authproblem = TRUE;
    }
  }
  वापस CURLE_OK;
}
#पूर्ण_अगर

#अगर_घोषित USE_NTLM
अटल CURLcode auth_ntlm(काष्ठा Curl_easy *तथ्य,
                          bool proxy,
                          स्थिर अक्षर *auth,
                          काष्ठा auth *authp,
                          uint32_t *availp)
{
  /* NTLM support requires the SSL crypto libs */
  अगर((authp->avail & CURLAUTH_NTLM) || Curl_auth_is_ntlm_supported()) {
    *availp |= CURLAUTH_NTLM;
    authp->avail |= CURLAUTH_NTLM;

    अगर(authp->picked == CURLAUTH_NTLM) {
      /* NTLM authentication is picked and activated */
      CURLcode परिणाम = Curl_निवेश_ntlm(तथ्य, proxy, auth);
      अगर(!परिणाम)
        तथ्य->state.authproblem = FALSE;
      अन्यथा {
        अगर(परिणाम == CURLE_OUT_OF_MEMORY)
          वापस परिणाम;
        infof(तथ्य, "NTLM authentication problem, ignoring.");
        तथ्य->state.authproblem = TRUE;
      }
    }
  }
  वापस CURLE_OK;
}
#पूर्ण_अगर

#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH
अटल CURLcode auth_digest(काष्ठा Curl_easy *तथ्य,
                            bool proxy,
                            स्थिर अक्षर *auth,
                            काष्ठा auth *authp,
                            uint32_t *availp)
{
  अगर(authp->avail & CURLAUTH_DIGEST) {
    *availp |= CURLAUTH_DIGEST;
    infof(तथ्य, "Ignoring duplicate digest auth header.");
  }
  अन्यथा अगर(Curl_auth_is_digest_supported()) {
    CURLcode परिणाम;

    *availp |= CURLAUTH_DIGEST;
    authp->avail |= CURLAUTH_DIGEST;

    /* We call this function on input Digest headers even if Digest
     * authentication is not activated yet, as we need to store the
     * incoming data from this header in case we are going to use
     * Digest */
    परिणाम = Curl_निवेश_digest(तथ्य, proxy, auth);
    अगर(परिणाम) {
      अगर(परिणाम == CURLE_OUT_OF_MEMORY)
        वापस परिणाम;
      infof(तथ्य, "Digest authentication problem, ignoring.");
      तथ्य->state.authproblem = TRUE;
    }
  }
  वापस CURLE_OK;
}
#पूर्ण_अगर

#अगर_अघोषित CURL_DISABLE_BASIC_AUTH
अटल CURLcode auth_basic(काष्ठा Curl_easy *तथ्य,
                           काष्ठा auth *authp,
                           uint32_t *availp)
{
  *availp |= CURLAUTH_BASIC;
  authp->avail |= CURLAUTH_BASIC;
  अगर(authp->picked == CURLAUTH_BASIC) {
    /* We asked for Basic authentication but got a 40X back anyway, which
       means our name+password is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(तथ्य, "Basic authentication problem, ignoring.");
    तथ्य->state.authproblem = TRUE;
  }
  वापस CURLE_OK;
}
#पूर्ण_अगर

#अगर_अघोषित CURL_DISABLE_BEARER_AUTH
अटल CURLcode auth_bearer(काष्ठा Curl_easy *तथ्य,
                            काष्ठा auth *authp,
                            uint32_t *availp)
{
  *availp |= CURLAUTH_BEARER;
  authp->avail |= CURLAUTH_BEARER;
  अगर(authp->picked == CURLAUTH_BEARER) {
    /* We asked for Bearer authentication but got a 40X back anyway, which
       means our token is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(तथ्य, "Bearer authentication problem, ignoring.");
    तथ्य->state.authproblem = TRUE;
  }
  वापस CURLE_OK;
}
#पूर्ण_अगर

/*
 * Curl_http_input_auth() deals with Proxy-Authenticate: and WWW-Authenticate:
 * headers. They are dealt with both in the transfer.c main loop and in the
 * proxy CONNECT loop.
 *
 * The 'auth' line ends with a NUL byte without CR or LF present.
 */
CURLcode Curl_http_निवेश_auth(काष्ठा Curl_easy *तथ्य, bool proxy,
                              स्थिर अक्षर *auth) /* the first non-space */
{
  /*
   * This resource requires authentication
   */
#अगर defined(USE_SPNEGO) ||                      \
  defined(USE_NTLM) ||                          \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||         \
  !defined(CURL_DISABLE_BASIC_AUTH) ||          \
  !defined(CURL_DISABLE_BEARER_AUTH)

  uint32_t *availp;
  काष्ठा auth *authp;
  CURLcode परिणाम = CURLE_OK;
  DEBUGASSERT(auth);
  DEBUGASSERT(तथ्य);

  अगर(proxy) {
    availp = &तथ्य->info.proxyauthavail;
    authp = &तथ्य->state.authproxy;
  }
  अन्यथा {
    availp = &तथ्य->info.httpauthavail;
    authp = &तथ्य->state.authhost;
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

  जबतक(*auth) {
#अगर_घोषित USE_SPNEGO
    अगर(authcmp("Negotiate", auth))
      परिणाम = auth_spnego(तथ्य, proxy, auth, authp, availp);
#पूर्ण_अगर
#अगर_घोषित USE_NTLM
    अगर(!परिणाम && authcmp("NTLM", auth))
      परिणाम = auth_ntlm(तथ्य, proxy, auth, authp, availp);
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH
    अगर(!परिणाम && authcmp("Digest", auth))
      परिणाम = auth_digest(तथ्य, proxy, auth, authp, availp);
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_BASIC_AUTH
    अगर(!परिणाम && authcmp("Basic", auth))
      परिणाम = auth_basic(तथ्य, authp, availp);
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_BEARER_AUTH
    अगर(authcmp("Bearer", auth))
      परिणाम = auth_bearer(तथ्य, authp, availp);
#पूर्ण_अगर

    अगर(परिणाम)
      अवरोध;

    /* there may be multiple methods on one line, so keep reading */
    auth = म_अक्षर(auth, ',');
    अगर(auth) /* if we are on a comma, skip it */
      auth++;
    अन्यथा
      अवरोध;
    curlx_str_passblanks(&auth);
  }
  वापस परिणाम;
#अन्यथा
  (व्योम)तथ्य;
  (व्योम)proxy;
  (व्योम)auth;
  /* nothing to do when disabled */
  वापस CURLE_OK;
#पूर्ण_अगर
}

अटल व्योम http_switch_to_get(काष्ठा Curl_easy *तथ्य, पूर्णांक code)
{
  स्थिर अक्षर *req = CURL_EASY_STR(तथ्य, STRING_CUSTOMREQUEST);

  अगर((req || तथ्य->state.httpreq != HTTPREQ_GET) &&
     (तथ्य->set.http_follow_mode == CURLFOLLOW_OBEYCODE)) {
    NOVERBOSE((व्योम)code);
    infof(तथ्य, "Switch to GET because of %d response", code);
    तथ्य->state.http_ignorecustom = TRUE;
  }
  अन्यथा अगर(req && (तथ्य->set.http_follow_mode != CURLFOLLOW_FIRSTONLY))
    infof(तथ्य, "Stick to %s instead of GET", req);

  तथ्य->state.httpreq = HTTPREQ_GET;
  Curl_creader_set_rewind(तथ्य, FALSE);
}

#घोषणा HTTPREQ_IS_POST(तथ्य)                    \
  ((तथ्य)->state.httpreq == HTTPREQ_POST ||      \
   (तथ्य)->state.httpreq == HTTPREQ_POST_FORM || \
   (तथ्य)->state.httpreq == HTTPREQ_POST_MIME)

अटल CURLcode set_auto_referer(काष्ठा Curl_easy *तथ्य)
{
  CURLU *u;
  अक्षर *referer = शून्य;
  CURLUcode uc;

  /* We are asked to automatically set the previous URL as the referer
     when we get the next URL. We pick the ->url field, which may or may
     not be 100% correct */
  Curl_bufref_free(&तथ्य->state.referer);

  /* Make a copy of the URL without credentials and fragment */
  u = curl_url();
  अगर(!u)
    वापस CURLE_OUT_OF_MEMORY;

  uc = curl_url_set(u, CURLUPART_URL,
                    Curl_bufref_ptr(&तथ्य->state.url), 0);
  अगर(!uc)
    uc = curl_url_set(u, CURLUPART_FRAGMENT, शून्य, 0);
  अगर(!uc)
    uc = curl_url_set(u, CURLUPART_USER, शून्य, 0);
  अगर(!uc)
    uc = curl_url_set(u, CURLUPART_PASSWORD, शून्य, 0);
  अगर(!uc)
    uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

  curl_url_cleanup(u);

  अगर(uc || !referer)
    वापस CURLE_OUT_OF_MEMORY;

  Curl_bufref_set(&तथ्य->state.referer, referer, 0, curl_free);
  वापस CURLE_OK;
}

अटल CURLcode follow_parse_url(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *newurl,
                                 followtype type, अक्षर **follow_urlp,
                                 CURLUcode *ucp)
{
  CURLUcode uc;

  DEBUGASSERT(तथ्य->state.uh);
  uc = curl_url_set(तथ्य->state.uh, CURLUPART_URL, newurl, (अचिन्हित पूर्णांक)
                    ((type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                     ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) |
                     CURLU_ALLOW_SPACE |
                     (तथ्य->set.path_as_is ? CURLU_PATH_AS_IS : 0)));
  *ucp = uc;
  अगर(uc) {
    अगर((uc == CURLUE_OUT_OF_MEMORY) || (type != FOLLOW_FAKE)) {
      failf(तथ्य, "The redirect target URL could not be parsed: %s",
            curl_url_strerror(uc));
      वापस Curl_uc_to_curlcode(uc);
    }

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, duplicate the field as-is */
    *follow_urlp = curlx_strdup(newurl);
    अगर(!*follow_urlp)
      वापस CURLE_OUT_OF_MEMORY;
  }
  अन्यथा {
    CURLU *u = curl_url();
    अगर(!u)
      वापस CURLE_OUT_OF_MEMORY;
    uc = curl_url_set(u, CURLUPART_URL,
                      Curl_bufref_ptr(&तथ्य->state.url),
                      CURLU_URLENCODE | CURLU_ALLOW_SPACE);
    अगर(!uc)
      uc = curl_url_get(तथ्य->state.uh, CURLUPART_URL, follow_urlp, 0);
    अगर(uc) {
      curl_url_cleanup(u);
      वापस Curl_uc_to_curlcode(uc);
    }

#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH
    {
      bool same_origin = Curl_url_same_origin(u, तथ्य->state.uh);
      curl_url_cleanup(u);
      अगर(!same_origin)
        Curl_auth_digest_cleanup(&तथ्य->state.digest);
    }
#अन्यथा
    curl_url_cleanup(u);
#पूर्ण_अगर
  }
  वापस CURLE_OK;
}

अटल CURLcode follow_fake(काष्ठा Curl_easy *तथ्य, अक्षर *follow_url,
                            CURLUcode uc, bool reachedmax)
{
  /* we are only figuring out the new URL if we would have followed locations
     but now we are done so we can get out! */
  अगर(!uc) {
    CURLU *u = curl_url();
    अक्षर *nocred = शून्य;

    अगर(!u) {
      curlx_free(follow_url);
      वापस CURLE_OUT_OF_MEMORY;
    }
    अगर(!curl_url_set(u, CURLUPART_URL, follow_url,
                     CURLU_NON_SUPPORT_SCHEME |
                     (तथ्य->set.path_as_is ? CURLU_PATH_AS_IS : 0)) &&
       !curl_url_set(u, CURLUPART_USER, शून्य, 0) &&
       !curl_url_set(u, CURLUPART_PASSWORD, शून्य, 0))
      (व्योम)curl_url_get(u, CURLUPART_URL, &nocred, CURLU_GET_EMPTY);
    curl_url_cleanup(u);
    अगर(nocred) {
      curlx_free(follow_url);
      follow_url = nocred;
    }
  }
  तथ्य->info.wouldredirect = follow_url;

  अगर(reachedmax) {
    failf(तथ्य, "Maximum (%d) redirects followed", तथ्य->set.maxredirs);
    वापस CURLE_TOO_MANY_REDIRECTS;
  }
  वापस CURLE_OK;
}

अटल bool follow_switch_get(काष्ठा Curl_easy *तथ्य)
{
  /*
   * We get here when the HTTP code is 300-399 and 401 or 407. We need to
   * perform differently based on exactly what return code there was.
   */
  चयन(तथ्य->info.httpcode) {
  हाल 301: /* Moved Permanently */
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
    अगर(HTTPREQ_IS_POST(तथ्य) && !तथ्य->set.post301) {
      http_switch_to_get(तथ्य, 301);
      वापस TRUE;
    }
    अवरोध;
  हाल 302: /* Found */
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
    अगर(HTTPREQ_IS_POST(तथ्य) && !तथ्य->set.post302) {
      http_switch_to_get(तथ्य, 302);
      वापस TRUE;
    }
    अवरोध;
  हाल 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     */
    अगर(!HTTPREQ_IS_POST(तथ्य) || !तथ्य->set.post303) {
      http_switch_to_get(तथ्य, 303);
      वापस TRUE;
    }
    अवरोध;

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
  शेष:
    अवरोध;
  }
  वापस FALSE;
}

CURLcode Curl_http_follow(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *newurl,
                          followtype type)
{
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  अक्षर *follow_url = शून्य;
  CURLUcode uc;
  CURLcode परिणाम;
  CURLcode rewind_परिणाम;
  bool switch_to_get;

  DEBUGASSERT(type != FOLLOW_NONE);

  अगर(type != FOLLOW_FAKE)
    तथ्य->state.requests++; /* count all real follows */
  अगर(type == FOLLOW_REDIR) {
    अगर((तथ्य->set.maxredirs != -1) &&
       (तथ्य->state.followlocation >= तथ्य->set.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    अन्यथा {
      तथ्य->state.followlocation++; /* count redirect-followings, including
                                       auth reloads */

      अगर(तथ्य->set.http_auto_referer) {
        परिणाम = set_auto_referer(तथ्य);
        अगर(परिणाम)
          वापस परिणाम;
      }
    }
  }

  अगर((type != FOLLOW_RETRY) &&
     (तथ्य->req.httpcode != 401) && (तथ्य->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, शून्य, 0, FALSE)) {
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: do not allow a custom port number */
    disallowport = TRUE;
  }

  परिणाम = follow_parse_url(तथ्य, newurl, type, &follow_url, &uc);
  अगर(परिणाम)
    वापस परिणाम;

  DEBUGASSERT(follow_url);

  अगर(type == FOLLOW_FAKE)
    वापस follow_fake(तथ्य, follow_url, uc, reachedmax);

  अगर(disallowport)
    तथ्य->state.allow_port = FALSE;

  Curl_bufref_set(&तथ्य->state.url, follow_url, 0, curl_free);
  rewind_परिणाम = Curl_req_soft_reset(&तथ्य->req, तथ्य);
  infof(तथ्य, "Issue another request to this URL: '%s'", follow_url);
  अगर((तथ्य->set.http_follow_mode == CURLFOLLOW_FIRSTONLY) &&
     !तथ्य->state.http_ignorecustom &&
     CURL_EASY_STR(तथ्य, STRING_CUSTOMREQUEST)) {
    तथ्य->state.http_ignorecustom = TRUE;
    infof(तथ्य, "Drop custom request method for next request");
  }

  switch_to_get = follow_switch_get(तथ्य);

  /* When rewind of upload data failed and we are not switching to GET,
   * we need to fail the follow, as we cannot send the data again. */
  अगर(rewind_परिणाम && !switch_to_get)
    वापस rewind_परिणाम;

  Curl_pgrsTime(तथ्य, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(तथ्य);

  वापस CURLE_OK;
}

/*
 * Curl_compareheader()
 *
 * Returns TRUE if 'headerline' contains the 'header' with given 'content'
 * (within a comma-separated list of tokens). Pass 'header' WITH the colon.
 *
 * @unittest: 1625
 */
bool Curl_compareheader(स्थिर अक्षर *headerline, /* line to check */
                        स्थिर अक्षर *header, /* header keyword _with_ colon */
                        स्थिर माप_प्रकार hlen, /* len of the keyword in bytes */
                        स्थिर अक्षर *content, /* content string to find */
                        स्थिर माप_प्रकार clen) /* len of the content in bytes */
{
  /* RFC2616, section 4.2 says: "Each header field consists of a name followed
   * by a colon (":") and the field value. Field names are case-insensitive.
   * The field value MAY be preceded by any amount of LWS, though a single SP
   * is preferred." */

  स्थिर अक्षर *p;
  काष्ठा Curl_str val;
  DEBUGASSERT(hlen);
  DEBUGASSERT(clen);
  DEBUGASSERT(header);
  DEBUGASSERT(content);

  अगर(!curl_strnequal(headerline, header, hlen))
    वापस FALSE; /* does not start with header */

  /* pass the header */
  p = &headerline[hlen];

  अगर(curlx_str_cspn(&p, &val, "\r\n"))
    वापस FALSE;
  curlx_str_trimblanks(&val);

  /* find the content string in the rest of the line */
  अगर(curlx_strlen(&val) >= clen) {
    माप_प्रकार len;
    p = curlx_str(&val);
    क्रम(len = curlx_strlen(&val); len >= clen;) {
      काष्ठा Curl_str अग्र;
      स्थिर अक्षर *o = p;
      /* after a match there must be a comma, space, newline or NUL byte */
      अगर(curl_strnequal(p, content, clen) &&
         ((p[clen] == ',') || ISBLANK(p[clen]) || ISNEWLINE(p[clen]) ||
          !p[clen]))
        वापस TRUE; /* match! */
      /* advance to the next comma */
      अगर(curlx_str_until(&p, &अग्र, len, ',') ||
         curlx_str_single(&p, ','))
        अवरोध; /* no comma, get out */

      /* if there are more dummy commas, move over them as well */
      करो
        curlx_str_passblanks(&p);
      जबतक(!curlx_str_single(&p, ','));
      /* trailing blanks may move the parsing point past the value end,
         then there is nothing left to match */
      अगर((माप_प्रकार)(p - o) > len)
        अवरोध;
      len -= (p - o);
    }
  }
  वापस FALSE; /* no match */
}

काष्ठा cr_exp100_ctx {
  काष्ठा Curl_creader super;
  काष्ठा curltime start; /* time started waiting */
  क्रमागत expect100 state;
};

/* Expect: 100-continue client reader, blocking uploads */

अटल व्योम http_exp100_continue(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा Curl_creader *reader)
{
  काष्ठा cr_exp100_ctx *ctx = reader->ctx;
  अगर(ctx->state > EXP100_SEND_तथ्य) {
    ctx->state = EXP100_SEND_तथ्य;
    Curl_expire_clear(तथ्य, EXPIRE_100_TIMEOUT);
  }
}

अटल CURLcode cr_exp100_read(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_creader *reader,
                               अक्षर *buf, माप_प्रकार blen,
                               माप_प्रकार *nread, bool *eos)
{
  काष्ठा cr_exp100_ctx *ctx = reader->ctx;
  timediff_t ms;

  चयन(ctx->state) {
  हाल EXP100_SENDING_REQUEST:
    अगर(!Curl_req_sendbuf_empty(तथ्य)) {
      /* The initial request data has not been fully sent yet. Do
       * not start the timer yet. */
      DEBUGF(infof(तथ्य, "cr_exp100_read, request not full sent yet"));
      *nread = 0;
      *eos = FALSE;
      वापस CURLE_OK;
    }
    /* We are now waiting for a reply from the server or
     * a timeout on our side IFF the request has been fully sent. */
    DEBUGF(infof(तथ्य, "cr_exp100_read, start AWAITING_CONTINUE, "
                 "timeout %dms", तथ्य->set.expect_100_timeout));
    ctx->state = EXP100_AWAITING_CONTINUE;
    ctx->start = *Curl_pgrs_now(तथ्य);
    Curl_expire_set(तथ्य, EXPIRE_100_TIMEOUT,
                    तथ्य->set.expect_100_timeout, &ctx->start);
    *nread = 0;
    *eos = FALSE;
    वापस CURLE_OK;
  हाल EXP100_FAILED:
    DEBUGF(infof(तथ्य, "cr_exp100_read, expectation failed, error"));
    *nread = 0;
    *eos = FALSE;
    वापस CURLE_READ_ERROR;
  हाल EXP100_AWAITING_CONTINUE:
    ms = curlx_ptimediff_ms(Curl_pgrs_now(तथ्य), &ctx->start);
    अगर(ms < तथ्य->set.expect_100_timeout) {
      DEBUGF(infof(तथ्य, "cr_exp100_read, AWAITING_CONTINUE, not expired"));
      *nread = 0;
      *eos = FALSE;
      वापस CURLE_OK;
    }
    /* we have waited long enough, continue anyway */
    http_exp100_continue(तथ्य, reader);
    infof(तथ्य, "Done waiting for 100-continue");
    FALLTHROUGH();
  शेष:
    DEBUGF(infof(तथ्य, "cr_exp100_read, pass through"));
    वापस Curl_creader_read(तथ्य, reader->अग्र, buf, blen, nread, eos);
  }
}

अटल व्योम cr_exp100_done(काष्ठा Curl_easy *तथ्य,
                           काष्ठा Curl_creader *reader, पूर्णांक premature)
{
  काष्ठा cr_exp100_ctx *ctx = reader->ctx;
  ctx->state = premature ? EXP100_FAILED : EXP100_SEND_तथ्य;
  Curl_expire_clear(तथ्य, EXPIRE_100_TIMEOUT);
}

अटल स्थिर काष्ठा Curl_crtype cr_exp100 = {
  "cr-exp100",
  Curl_creader_def_init,
  cr_exp100_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  Curl_creader_def_योग_लंबाई,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  cr_exp100_done,
  माप(काष्ठा cr_exp100_ctx)
};

अटल CURLcode http_exp100_add_reader(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_creader *reader = शून्य;
  CURLcode परिणाम;

  परिणाम = Curl_creader_create(&reader, तथ्य, &cr_exp100, CURL_CR_PROTOCOL);
  अगर(!परिणाम)
    परिणाम = Curl_creader_add(तथ्य, reader);
  अगर(!परिणाम) {
    काष्ठा cr_exp100_ctx *ctx = reader->ctx;
    ctx->state = EXP100_SENDING_REQUEST;
  }

  अगर(परिणाम && reader)
    Curl_creader_free(तथ्य, reader);
  वापस परिणाम;
}

अटल व्योम http_exp100_got100(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_creader *r = Curl_creader_get_by_type(तथ्य, &cr_exp100);
  अगर(r)
    http_exp100_continue(तथ्य, r);
}

अटल bool http_exp100_is_waiting(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_creader *r = Curl_creader_get_by_type(तथ्य, &cr_exp100);
  अगर(r) {
    काष्ठा cr_exp100_ctx *ctx = r->ctx;
    वापस ctx->state == EXP100_AWAITING_CONTINUE;
  }
  वापस FALSE;
}

अटल व्योम http_exp100_send_anyway(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_creader *r = Curl_creader_get_by_type(तथ्य, &cr_exp100);
  अगर(r)
    http_exp100_continue(तथ्य, r);
}

अटल bool http_exp100_is_selected(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_creader *r = Curl_creader_get_by_type(तथ्य, &cr_exp100);
  वापस !!r;
}

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
CURLcode Curl_http_doing_pollset(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा easy_pollset *ps)
{
  /* write mode */
  वापस Curl_pollset_add_out(तथ्य, ps, तथ्य->conn->sock[FIRSTSOCKET]);
}

CURLcode Curl_http_perform_pollset(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा easy_pollset *ps)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  CURLcode परिणाम = CURLE_OK;

  अगर(CURL_REQ_WANT_RECV(तथ्य)) {
    परिणाम = Curl_pollset_add_in(तथ्य, ps, conn->sock[FIRSTSOCKET]);
  }

  /* on a "Expect: 100-continue" timed wait, do not poll for outgoing */
  अगर(!परिणाम && Curl_req_want_send(तथ्य) && !http_exp100_is_waiting(तथ्य)) {
    परिणाम = Curl_pollset_add_out(तथ्य, ps, conn->sock[FIRSTSOCKET]);
  }
  वापस परिणाम;
}

अटल CURLcode http_write_header(काष्ठा Curl_easy *तथ्य,
                                  स्थिर अक्षर *hd, माप_प्रकार hdlen)
{
  CURLcode परिणाम;
  पूर्णांक writetype;

  /* now, only output this if the header AND body are requested:
   */
  Curl_debug(तथ्य, CURLINFO_HEADER_IN, hd, hdlen);

  writetype = CLIENTWRITE_HEADER |
    ((तथ्य->req.httpcode / 100 == 1) ? CLIENTWRITE_1XX : 0);

  परिणाम = Curl_client_write(तथ्य, writetype, hd, hdlen);
  अगर(परिणाम)
    वापस परिणाम;

  परिणाम = Curl_bump_headersize(तथ्य, hdlen, FALSE);
  अगर(परिणाम)
    वापस परिणाम;

  तथ्य->req.deductheadercount = (100 <= तथ्य->req.httpcode &&
                                 199 >= तथ्य->req.httpcode) ?
    तथ्य->req.headerbytecount : 0;
  वापस परिणाम;
}

/*
 * Curl_http_done() gets called after a single HTTP request has been
 * performed.
 */

CURLcode Curl_http_done(काष्ठा Curl_easy *तथ्य,
                        CURLcode status, bool premature)
{
  काष्ठा connectdata *conn = तथ्य->conn;

  /* Clear multipass flag. If authentication is not done yet, then it will get
   * a chance to be set back to true when we output the next auth header */
  तथ्य->state.authhost.multipass = FALSE;
  तथ्य->state.authproxy.multipass = FALSE;

  अगर(curlx_dyn_len(&तथ्य->state.headerb)) {
    (व्योम)http_write_header(तथ्य, curlx_dyn_ptr(&तथ्य->state.headerb),
                            curlx_dyn_len(&तथ्य->state.headerb));
  }
  curlx_dyn_reset(&तथ्य->state.headerb);

  अगर(status)
    वापस status;

  अगर(!premature && /* this check is pointless when DONE is called before the
                      entire operation is complete */
     !conn->bits.retry &&
     !तथ्य->set.connect_only &&
     (तथ्य->req.bytecount +
      तथ्य->req.headerbytecount -
      तथ्य->req.deductheadercount) <= 0) {
    /* If this connection is not closed to be retried, AND nothing was
       read from the HTTP server (that counts), this cannot be right so we
       return an error here */
    failf(तथ्य, "Empty reply from server");
    /* Mark it as closed to avoid the "left intact" message */
    streamclose(conn);
    वापस CURLE_GOT_NOTHING;
  }

  वापस CURLE_OK;
}

/* Determine if we may use HTTP 1.1 for this request. */
अटल bool http_may_use_1_1(स्थिर काष्ठा Curl_easy *तथ्य)
{
  स्थिर काष्ठा connectdata *conn = तथ्य->conn;
  /* We have seen a previous response for *this* transfer with 1.0,
   * on another connection or the same one. */
  अगर(तथ्य->state.http_neg.rcvd_min == 10)
    वापस FALSE;
  /* We have seen a previous response on *this* connection with 1.0. */
  अगर(conn && conn->httpversion_seen == 10)
    वापस FALSE;
  /* We want 1.0 and have seen no previous response on *this* connection
     with a higher version (maybe no response at all yet). */
  अगर(तथ्य->state.http_neg.only_10 &&
     (!conn || conn->httpversion_seen <= 10))
    वापस FALSE;
  /* We are not restricted to use 1.0 only. */
  वापस !तथ्य->state.http_neg.only_10;
}

अटल अचिन्हित अक्षर http_request_version(काष्ठा Curl_easy *तथ्य)
{
  अचिन्हित अक्षर v = Curl_conn_http_version(तथ्य, तथ्य->conn);
  अगर(!v) {
    /* No specific HTTP connection filter installed. */
    v = http_may_use_1_1(तथ्य) ? 11 : 10;
  }
  वापस v;
}

अटल स्थिर अक्षर *get_http_string(पूर्णांक httpversion)
{
  चयन(httpversion) {
  हाल 30:
    वापस "3";
  हाल 20:
    वापस "2";
  हाल 11:
    वापस "1.1";
  शेष:
    वापस "1.0";
  }
}

CURLcode Curl_add_custom_headers(काष्ठा Curl_easy *तथ्य,
                                 bool is_connect, पूर्णांक httpversion,
                                 काष्ठा dynbuf *req)
{
  काष्ठा curl_slist *h[2];
  काष्ठा curl_slist *headers;
  पूर्णांक numlists = 1; /* by default */
  पूर्णांक i;

#अगर_अघोषित CURL_DISABLE_PROXY
  क्रमागत Curl_proxy_use proxy;

  अगर(is_connect)
    proxy = HEADER_CONNECT;
  अन्यथा
    proxy = तथ्य->conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  चयन(proxy) {
  हाल HEADER_SERVER:
    h[0] = तथ्य->set.headers;
    अवरोध;
  हाल HEADER_PROXY:
    h[0] = तथ्य->set.headers;
    अगर(तथ्य->set.sep_headers) {
      h[1] = तथ्य->set.proxyheaders;
      numlists++;
    }
    अवरोध;
  हाल HEADER_CONNECT:
    अगर(तथ्य->set.sep_headers)
      h[0] = तथ्य->set.proxyheaders;
    अन्यथा
      h[0] = तथ्य->set.headers;
    अवरोध;
  हाल HEADER_CONNECT_UDP:
    अगर(तथ्य->set.sep_headers)
      h[0] = तथ्य->set.proxyheaders;
    अन्यथा
      h[0] = तथ्य->set.headers;
    अवरोध;
  }
#अन्यथा
  (व्योम)is_connect;
  h[0] = तथ्य->set.headers;
#पूर्ण_अगर

  /* loop through one or two lists */
  क्रम(i = 0; i < numlists; i++) {
    क्रम(headers = h[i]; headers; headers = headers->अग्र) {
      CURLcode परिणाम = CURLE_OK;
      bool blankheader = FALSE;
      काष्ठा Curl_str name;
      स्थिर अक्षर *p = headers->तथ्य;
      स्थिर अक्षर *origp = p;
      माप_प्रकार hlen = म_माप(origp);

      /* explicitly asked to send header without content is done by a header
         that ends with a semicolon, but there must be no colon present in the
         name */
      अगर(!curlx_str_until(&p, &name, hlen, ';') &&
         !curlx_str_single(&p, ';') &&
         !curlx_str_single(&p, '\0') &&
         !स_प्रथम(curlx_str(&name), ':', curlx_strlen(&name)))
        blankheader = TRUE;
      अन्यथा {
        p = origp;
        अगर(!curlx_str_until(&p, &name, hlen, ':') &&
           !curlx_str_single(&p, ':')) {
          काष्ठा Curl_str val;
          curlx_str_untilnl(&p, &val, hlen);
          curlx_str_trimblanks(&val);
          अगर(!curlx_strlen(&val))
            /* no content, do not send this */
            जारी;
        }
        अन्यथा
          /* no colon */
          जारी;
      }

      /* a field name is a token and carries no surrounding whitespace, so
         trim the parsed name before matching. Otherwise `Authorization :`
         (space before the colon) slips past the Authorization/Cookie check
         below and gets forwarded to another host on a redirect. */
      curlx_str_trimblanks(&name);

      /* only send this if the contents was non-blank or done special */

      अगर(तथ्य->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom
            Host: header as that will produce *two* in the same
            request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      अन्यथा अगर(तथ्य->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      अन्यथा अगर(तथ्य->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      अन्यथा अगर(तथ्य->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      अन्यथा अगर(curlx_str_casecompare(&name, "Connection"))
        /* Connection headers are handled specially */
        ;
      अन्यथा अगर((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        /* HTTP/2 does not support chunked requests */
        ;
      अन्यथा अगर((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(तथ्य))
        ;
      अन्यथा अगर(blankheader) {
        परिणाम = curlx_dyn_addn(req, curlx_str(&name), curlx_strlen(&name));
        अगर(!परिणाम)
          परिणाम = curlx_dyn_addn(req, STRCONST(":\r\n"));
      }
      अन्यथा
        परिणाम = curlx_dyn_addf(req, "%s\r\n", origp);

      अगर(परिणाम)
        वापस परिणाम;
    }
  }

  वापस CURLE_OK;
}

#अगर_अघोषित CURL_DISABLE_PARSEDATE
CURLcode Curl_add_timecondition(काष्ठा Curl_easy *तथ्य,
                                काष्ठा dynbuf *req)
{
  स्थिर काष्ठा पंचांग *पंचांग;
  काष्ठा पंचांग keeptime;
  CURLcode परिणाम;
  अक्षर datestr[80];
  स्थिर अक्षर *condp;
  माप_प्रकार len;

  अगर(तथ्य->set.timecondition == CURL_TIMECOND_NONE)
    /* no condition was asked for */
    वापस CURLE_OK;

  परिणाम = curlx_gmtime(तथ्य->set.timevalue, &keeptime);
  अगर(परिणाम) {
    failf(तथ्य, "Invalid TIMEVALUE");
    वापस परिणाम;
  }
  पंचांग = &keeptime;

  चयन(तथ्य->set.timecondition) {
  शेष:
    DEBUGF(infof(तथ्य, "invalid time condition"));
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  हाल CURL_TIMECOND_IFMODSINCE:
    condp = "If-Modified-Since";
    len = 17;
    अवरोध;
  हाल CURL_TIMECOND_IFUNMODSINCE:
    condp = "If-Unmodified-Since";
    len = 19;
    अवरोध;
  हाल CURL_TIMECOND_LASTMOD:
    condp = "Last-Modified";
    len = 13;
    अवरोध;
  }

  अगर(Curl_checkheaders(तथ्य, condp, len)) {
    /* A custom header was specified; it will be sent instead. */
    वापस CURLE_OK;
  }

  /* The If-Modified-Since header family should have their times set in
   * GMT as RFC2616 defines: "All HTTP date/time stamps MUST be
   * represented in Greenwich Mean Time (GMT), without exception. For the
   * purposes of HTTP, GMT is exactly equal to UTC (Coordinated Universal
   * Time)." (see page 20 of RFC2616).
   */

  /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
  curl_msnprintf(datestr, माप(datestr),
                 "%s: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                 condp,
                 Curl_wkday[पंचांग->tm_wday ? पंचांग->tm_wday - 1 : 6],
                 पंचांग->tm_mday,
                 Curl_month[पंचांग->tm_mon],
                 पंचांग->tm_year + 1900,
                 पंचांग->tm_hour,
                 पंचांग->tm_min,
                 पंचांग->tm_sec);

  परिणाम = curlx_dyn_add(req, datestr);
  वापस परिणाम;
}
#अन्यथा
/* disabled */
CURLcode Curl_add_timecondition(काष्ठा Curl_easy *तथ्य,
                                काष्ठा dynbuf *req)
{
  (व्योम)तथ्य;
  (व्योम)req;
  वापस CURLE_OK;
}
#पूर्ण_अगर

व्योम Curl_http_method(काष्ठा Curl_easy *तथ्य,
                      स्थिर अक्षर **method, Curl_HttpReq *reqp)
{
  Curl_HttpReq httpreq = (Curl_HttpReq)तथ्य->state.httpreq;
  स्थिर अक्षर *request;
#अगर_अघोषित CURL_DISABLE_WEBSOCKETS
  अगर(तथ्य->conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
    httpreq = HTTPREQ_GET;
  अन्यथा
#पूर्ण_अगर
  अगर((तथ्य->conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_FTP)) &&
     तथ्य->state.upload)
    httpreq = HTTPREQ_PUT;

  /* Now set the 'request' pointer to the proper request string */
  अगर(!तथ्य->state.http_ignorecustom &&
     CURL_EASY_STR(तथ्य, STRING_CUSTOMREQUEST)) {
    request = CURL_EASY_STR(तथ्य, STRING_CUSTOMREQUEST);
  }
  अन्यथा {
    अगर(तथ्य->req.no_body)
      request = "HEAD";
    अन्यथा {
      DEBUGASSERT((httpreq >= HTTPREQ_GET) && (httpreq <= HTTPREQ_शीर्ष));
      चयन(httpreq) {
      हाल HTTPREQ_POST:
      हाल HTTPREQ_POST_FORM:
      हाल HTTPREQ_POST_MIME:
        request = "POST";
        अवरोध;
      हाल HTTPREQ_PUT:
        request = "PUT";
        अवरोध;
      शेष: /* this should never happen */
      हाल HTTPREQ_GET:
        request = "GET";
        अवरोध;
      हाल HTTPREQ_शीर्ष:
        request = "HEAD";
        अवरोध;
      }
    }
  }
  *method = request;
  *reqp = httpreq;
}

अटल CURLcode http_set_aptr_host(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  स्थिर अक्षर *ptr = शून्य;

  curlx_safefree(तथ्य->state.http_host);
#अगर_अघोषित CURL_DISABLE_COOKIES
  curlx_safefree(तथ्य->req.cookiehost);
#पूर्ण_अगर

  अगर(Curl_peer_equal(तथ्य->state.initial_origin, तथ्य->state.origin))
    ptr = Curl_checkheaders(तथ्य, STRCONST("Host"));

  अगर(ptr) {
#अगर_अघोषित CURL_DISABLE_COOKIES
    /* If we have a given custom Host: header, we extract the hostname in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the hostname
       is the same as the first one! */
    अक्षर *cookiehost;
    CURLcode परिणाम = copy_custom_मूल्य(ptr, &cookiehost);
    अगर(परिणाम)
      वापस परिणाम;
    अगर(!*cookiehost)
      /* ignore empty data */
      curlx_free(cookiehost);
    अन्यथा {
      /* If the host begins with '[', we start searching for the port after
         the bracket has been closed */
      अगर(*cookiehost == '[') {
        अक्षर *closingbracket;
        /* since the 'cookiehost' is an allocated memory area that will be
           freed later we cannot increment the pointer */
        स_हटाओ(cookiehost, cookiehost + 1, म_माप(cookiehost) - 1);
        closingbracket = म_अक्षर(cookiehost, ']');
        अगर(closingbracket)
          *closingbracket = 0;
      }
      अन्यथा {
        पूर्णांक startsearch = 0;
        अक्षर *colon = म_अक्षर(cookiehost + startsearch, ':');
        अगर(colon)
          *colon = 0; /* The host must not include an embedded port number */
      }
      तथ्य->req.cookiehost = cookiehost;
    }
#पूर्ण_अगर

    अगर(!curl_strequal("Host:", ptr)) {
      तथ्य->state.http_host = curl_maprintf("Host:%s", &ptr[5]);
      अगर(!तथ्य->state.http_host)
        वापस CURLE_OUT_OF_MEMORY;
    }
  }
  अन्यथा {
    /* This is the  HTTP Host: header, so we want
     * - for IPv6 origins: "[ipv6-address]" where the IPv6 address is
     *  found in origin->hostname, stripped of zoneid/scopeid.
     * - the (IDN converted) origin->hostname (DNS name or IPv4) otherwise.
     * Note: zoneid/scopeid  only applies to local routing and has no
     * meaning on the remote HTTP server (eg. would confuse it). */
    bool ipv6 = (bool)तथ्य->state.origin->ipv6;
    काष्ठा dynbuf tmp;
    माप_प्रकार hlen;
    CURLcode परिणाम;

    curlx_dyn_init(&tmp, DYN_HTTP_REQUEST);
    परिणाम = curlx_dyn_addn(&tmp, STRCONST("Host: "));
    अगर(!परिणाम && ipv6)
      परिणाम = curlx_dyn_addn(&tmp, STRCONST("["));
    अगर(!परिणाम)
      परिणाम = curlx_dyn_add(&tmp, तथ्य->state.origin->hostname);
    अगर(!परिणाम && ipv6)
      परिणाम = curlx_dyn_addn(&tmp, STRCONST("]"));
    अगर(!परिणाम &&
       ((तथ्य->state.origin->port != तथ्य->state.origin->scheme->defport) ||
       (तथ्य->state.origin->scheme->family != conn->scheme->family))) {
      परिणाम = curlx_dyn_addf(&tmp, ":%u", तथ्य->state.origin->port);
    }

    तथ्य->state.http_host = परिणाम ? शून्य : curlx_dyn_take(&tmp, &hlen);
    curlx_dyn_free(&tmp);
    वापस परिणाम;
  }
  वापस CURLE_OK;
}

/*
 * Append the request-target to the HTTP request
 */
अटल CURLcode http_target(काष्ठा Curl_easy *तथ्य,
                            काष्ठा dynbuf *r)
{
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर *path = तथ्य->state.up.path;
  स्थिर अक्षर *query = तथ्य->state.up.query;
#अगर_अघोषित CURL_DISABLE_PROXY
  काष्ठा connectdata *conn = तथ्य->conn;
#पूर्ण_अगर

  अगर(CURL_EASY_STR(तथ्य, STRING_TARGET)) {
    path = CURL_EASY_STR(तथ्य, STRING_TARGET);
    query = शून्य;
  }

#अगर_अघोषित CURL_DISABLE_PROXY
  अगर(conn->bits.origin_is_proxy) {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL, but if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the decoded hostname! */

    /* and no fragment part */
    CURLUcode uc;
    अक्षर *url;
    CURLU *h = curl_url_dup(तथ्य->state.uh);
    अगर(!h)
      वापस CURLE_OUT_OF_MEMORY;

    अगर(!तथ्य->state.origin->ipv6 &&
       (तथ्य->state.origin->user_hostname != तथ्य->state.origin->hostname)) {
      uc = curl_url_set(h, CURLUPART_HOST, तथ्य->state.origin->hostname, 0);
      अगर(uc) {
        curl_url_cleanup(h);
        वापस CURLE_OUT_OF_MEMORY;
      }
    }
    uc = curl_url_set(h, CURLUPART_FRAGMENT, शून्य, 0);
    अगर(uc) {
      curl_url_cleanup(h);
      वापस CURLE_OUT_OF_MEMORY;
    }

    अगर(तथ्य->state.origin->scheme == &Curl_scheme_http) {
      /* when getting HTTP, we do not want the userinfo the URL */
      uc = curl_url_set(h, CURLUPART_USER, शून्य, 0);
      अगर(uc) {
        curl_url_cleanup(h);
        वापस CURLE_OUT_OF_MEMORY;
      }
      uc = curl_url_set(h, CURLUPART_PASSWORD, शून्य, 0);
      अगर(uc) {
        curl_url_cleanup(h);
        वापस CURLE_OUT_OF_MEMORY;
      }
    }
    अन्यथा अगर(तथ्य->state.creds && (तथ्य->state.creds->source != CREDS_URL)) {
        /* credentials not from the URL need to be set */
      uc = curl_url_set(h, CURLUPART_USER,
                        तथ्य->state.creds->user, CURLU_URLENCODE);
      अगर(!uc)
        uc = curl_url_set(h, CURLUPART_PASSWORD,
                          तथ्य->state.creds->passwd, CURLU_URLENCODE);
      अगर(uc) {
        curl_url_cleanup(h);
        वापस Curl_uc_to_curlcode(uc);
      }
    }

    /* Extract the URL to use in the request. */
    uc = curl_url_get(h, CURLUPART_URL, &url, CURLU_NO_DEFAULT_PORT);
    अगर(uc) {
      curl_url_cleanup(h);
      वापस CURLE_OUT_OF_MEMORY;
    }

    curl_url_cleanup(h);

    /* target or URL */
    परिणाम = curlx_dyn_add(r, CURL_EASY_STR(तथ्य, STRING_TARGET) ?
      CURL_EASY_STR(तथ्य, STRING_TARGET) : url);
    curlx_free(url);
    अगर(परिणाम)
      वापस परिणाम;

    अगर((तथ्य->state.origin->scheme == &Curl_scheme_ftp) &&
       तथ्य->set.proxy_transfer_mode) {
      /* when doing ftp, append ;type=<a|i> if not present */
      माप_प्रकार len = म_माप(path);
      bool type_present = FALSE;
      अगर((len >= 7) && !स_भेद(&path[len - 7], ";type=", 6)) {
        चयन(Curl_raw_toupper(path[len - 1])) {
        हाल 'A':
        हाल 'D':
        हाल 'I':
          type_present = TRUE;
          अवरोध;
        }
      }
      अगर(!type_present) {
        परिणाम = curlx_dyn_addf(r, ";type=%c",
                                तथ्य->state.prefer_ascii ? 'a' : 'i');
        अगर(परिणाम)
          वापस परिणाम;
      }
    }
  }

  अन्यथा
#पूर्ण_अगर
  {
    परिणाम = curlx_dyn_add(r, path);
    अगर(परिणाम)
      वापस परिणाम;
    अगर(query)
      परिणाम = curlx_dyn_addf(r, "?%s", query);
  }

  वापस परिणाम;
}

#अगर !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
अटल CURLcode set_post_reader(काष्ठा Curl_easy *तथ्य, Curl_HttpReq httpreq)
{
  CURLcode परिणाम;

  चयन(httpreq) {
#अगर_अघोषित CURL_DISABLE_MIME
  हाल HTTPREQ_POST_MIME:
    तथ्य->state.mimepost = तथ्य->set.mimepostp;
    अवरोध;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_FORM_API
  हाल HTTPREQ_POST_FORM:
    /* Convert the form structure into a mime structure, then keep
       the conversion */
    अगर(!तथ्य->state.formp) {
      तथ्य->state.formp = curlx_calloc(1, माप(curl_mimepart));
      अगर(!तथ्य->state.formp)
        वापस CURLE_OUT_OF_MEMORY;
      Curl_mime_cleanpart(तथ्य->state.formp);
      परिणाम = Curl_getformdata(तथ्य, तथ्य->state.formp, तथ्य->set.httppost,
                                तथ्य->state.fread_func);
      अगर(परिणाम) {
        curlx_safefree(तथ्य->state.formp);
        वापस परिणाम;
      }
      तथ्य->state.mimepost = तथ्य->state.formp;
    }
    अवरोध;
#पूर्ण_अगर
  शेष:
    तथ्य->state.mimepost = शून्य;
    अवरोध;
  }

  चयन(httpreq) {
  हाल HTTPREQ_POST_FORM:
  हाल HTTPREQ_POST_MIME:
    /* This is form posting using mime data. */
#अगर_अघोषित CURL_DISABLE_MIME
    अगर(तथ्य->state.mimepost) {
      स्थिर अक्षर *cthdr = Curl_checkheaders(तथ्य, STRCONST("Content-Type"));

      /* Read and seek body only. */
      तथ्य->state.mimepost->flags |= MIME_BODY_ONLY;

      /* Prepare the mime structure headers & set content type. */

      अगर(cthdr)
        क्रम(cthdr += 13; *cthdr == ' '; cthdr++)
          ;
      अन्यथा अगर(तथ्य->state.mimepost->kind == MIMEKIND_MULTIPART)
        cthdr = "multipart/form-data";

      curl_mime_headers(तथ्य->state.mimepost, तथ्य->set.headers, 0);
      परिणाम = Curl_mime_prepare_headers(तथ्य, तथ्य->state.mimepost, cthdr,
                                         शून्य, MIMESTRATEGY_FORM);
      अगर(परिणाम)
        वापस परिणाम;
      curl_mime_headers(तथ्य->state.mimepost, शून्य, 0);
      परिणाम = Curl_creader_set_mime(तथ्य, तथ्य->state.mimepost);
      अगर(परिणाम)
        वापस परिणाम;
    }
    अन्यथा
#पूर्ण_अगर
    {
      परिणाम = Curl_creader_set_null(तथ्य);
    }
    तथ्य->state.infilesize = Curl_creader_योग_लंबाई(तथ्य);
    वापस परिणाम;

  शेष:
    वापस Curl_creader_set_null(तथ्य);
  }
  /* never reached */
}
#पूर्ण_अगर

अटल CURLcode set_reader(काष्ठा Curl_easy *तथ्य, Curl_HttpReq httpreq)
{
  CURLcode परिणाम = CURLE_OK;
  curl_off_t postsize = तथ्य->state.infilesize;

  DEBUGASSERT(तथ्य->conn);

  अगर(तथ्य->req.authneg) {
    वापस Curl_creader_set_null(तथ्य);
  }

  चयन(httpreq) {
  हाल HTTPREQ_PUT: /* Let's PUT the data to the server! */
    वापस postsize ? Curl_creader_set_fread(तथ्य, postsize) :
      Curl_creader_set_null(तथ्य);

#अगर !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  हाल HTTPREQ_POST_FORM:
  हाल HTTPREQ_POST_MIME:
    वापस set_post_reader(तथ्य, httpreq);
#पूर्ण_अगर

  हाल HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */
    /* the size of the post body */
    अगर(!postsize) {
      परिणाम = Curl_creader_set_null(तथ्य);
    }
    अन्यथा अगर(तथ्य->set.postfields) {
      माप_प्रकार plen = curlx_sotouz_range(postsize, 0, SIZE_MAX);
      अगर(plen == SIZE_MAX)
        वापस CURLE_OUT_OF_MEMORY;
      अन्यथा अगर(plen)
        परिणाम = Curl_creader_set_buf(तथ्य, तथ्य->set.postfields, plen);
      अन्यथा
        परिणाम = Curl_creader_set_null(तथ्य);
    }
    अन्यथा {
      /* we read the bytes from the callback. In case "chunked" encoding
       * is forced by the application, we disregard `postsize`. This is
       * a backward compatibility decision to earlier versions where
       * chunking disregarded this. See issue #13229. */
      bool chunked = FALSE;
      अक्षर *ptr = Curl_checkheaders(तथ्य, STRCONST("Transfer-Encoding"));
      अगर(ptr) {
        /* Some kind of TE is requested, check if 'chunked' is chosen */
        chunked = Curl_compareheader(ptr, STRCONST("Transfer-Encoding:"),
                                     STRCONST("chunked"));
      }
      परिणाम = Curl_creader_set_fread(तथ्य, chunked ? -1 : postsize);
    }
    वापस परिणाम;

  शेष:
    /* HTTP GET/HEAD download, has no body, needs no Content-Length */
    तथ्य->state.infilesize = 0;
    वापस Curl_creader_set_null(तथ्य);
  }
  /* not reached */
}

अटल CURLcode http_resume(काष्ठा Curl_easy *तथ्य, Curl_HttpReq httpreq)
{
  अगर((HTTPREQ_POST == httpreq || HTTPREQ_POST_FORM == httpreq ||
      HTTPREQ_POST_MIME == httpreq || HTTPREQ_PUT == httpreq) &&
     तथ्य->state.resume_from) {
    failf(तथ्य, "HTTP upload cannot be resumed");
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }
  वापस CURLE_OK;
}

अटल CURLcode http_req_set_TE(काष्ठा Curl_easy *तथ्य,
                                काष्ठा dynbuf *req,
                                पूर्णांक httpversion)
{
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर *ptr;

  ptr = Curl_checkheaders(तथ्य, STRCONST("Transfer-Encoding"));
  अगर(ptr) {
    /* Some kind of TE is requested, check if 'chunked' is chosen */
    तथ्य->req.upload_chunky =
      Curl_compareheader(ptr,
                         STRCONST("Transfer-Encoding:"), STRCONST("chunked"));
    अगर(तथ्य->req.upload_chunky && (httpversion >= 20)) {
      infof(तथ्य, "suppressing chunked transfer encoding on connection "
            "using HTTP version 2 or higher");
      तथ्य->req.upload_chunky = FALSE;
    }
  }
  अन्यथा {
    curl_off_t req_clen = Curl_creader_योग_लंबाई(तथ्य);

    अगर(req_clen < 0) {
      /* indeterminate request content length */
      अगर(httpversion > 10) {
        /* On HTTP/1.1, enable chunked, on HTTP/2 and later we do not
         * need it */
        तथ्य->req.upload_chunky = (httpversion < 20);
      }
      अन्यथा {
        failf(तथ्य, "Chunky upload is not supported by HTTP 1.0");
        वापस CURLE_UPLOAD_FAILED;
      }
    }
    अन्यथा {
      /* else, no chunky upload */
      तथ्य->req.upload_chunky = FALSE;
    }

    अगर(तथ्य->req.upload_chunky)
      परिणाम = curlx_dyn_add(req, "Transfer-Encoding: chunked\r\n");
  }
  वापस परिणाम;
}

अटल CURLcode addexpect(काष्ठा Curl_easy *तथ्य, काष्ठा dynbuf *r,
                          पूर्णांक httpversion, bool *announced_exp100)
{
  CURLcode परिणाम;
  अक्षर *ptr;

  *announced_exp100 = FALSE;
  /* Avoid Expect: 100-continue if Upgrade: is used */
  अगर(तथ्य->req.upgr101 != UPGR101_NONE)
    वापस CURLE_OK;

  /* For really small puts we do not use Expect: headers at all, and for
     the somewhat bigger ones we allow the app to disable it. Make
     sure that the expect100header is always set to the preferred value
     here. */
  ptr = Curl_checkheaders(तथ्य, STRCONST("Expect"));
  अगर(ptr) {
    *announced_exp100 =
      Curl_compareheader(ptr, STRCONST("Expect:"), STRCONST("100-continue"));
  }
  अन्यथा अगर(!तथ्य->state.disableexpect && (httpversion == 11)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    curl_off_t client_len = Curl_creader_client_लंबाई(तथ्य);
    अगर(client_len > EXPECT_100_THRESHOLD || client_len < 0) {
      परिणाम = curlx_dyn_addn(r, STRCONST("Expect: 100-continue\r\n"));
      अगर(परिणाम)
        वापस परिणाम;
      *announced_exp100 = TRUE;
    }
  }
  वापस CURLE_OK;
}

अटल CURLcode http_add_content_hds(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा dynbuf *r,
                                     पूर्णांक httpversion,
                                     Curl_HttpReq httpreq)
{
  CURLcode परिणाम = CURLE_OK;
  curl_off_t req_clen;
  bool announced_exp100 = FALSE;

  DEBUGASSERT(तथ्य->conn);
  अगर(तथ्य->req.upload_chunky) {
    परिणाम = Curl_httpchunk_add_reader(तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
  }

  /* Get the request body length that has been set up */
  req_clen = Curl_creader_योग_लंबाई(तथ्य);
  चयन(httpreq) {
  हाल HTTPREQ_PUT:
  हाल HTTPREQ_POST:
#अगर !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  हाल HTTPREQ_POST_FORM:
  हाल HTTPREQ_POST_MIME:
#पूर्ण_अगर
    /* We only set Content-Length and allow a custom Content-Length if
       we do not upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length).
       We do not override a custom "Content-Length" header, but during
       authentication negotiation that header is suppressed.
     */
    अगर(req_clen >= 0 && !तथ्य->req.upload_chunky &&
       (तथ्य->req.authneg ||
        !Curl_checkheaders(तथ्य, STRCONST("Content-Length")))) {
      /* we allow replacing this header if not during auth negotiation,
         although it is not wise to actually set your own */
      परिणाम = curlx_dyn_addf(r, "Content-Length: %" FMT_OFF_T "\r\n",
                              req_clen);
    }
    अगर(परिणाम)
      जाओ out;

#अगर_अघोषित CURL_DISABLE_MIME
    /* Output mime-generated headers. */
    अगर(तथ्य->state.mimepost &&
       ((httpreq == HTTPREQ_POST_FORM) || (httpreq == HTTPREQ_POST_MIME))) {
      काष्ठा curl_slist *hdr;

      क्रम(hdr = तथ्य->state.mimepost->curlheaders; hdr; hdr = hdr->अग्र) {
        परिणाम = curlx_dyn_addf(r, "%s\r\n", hdr->तथ्य);
        अगर(परिणाम)
          जाओ out;
      }
    }
#पूर्ण_अगर
    अगर(httpreq == HTTPREQ_POST &&
       !Curl_checkheaders(तथ्य, STRCONST("Content-Type"))) {
      परिणाम = curlx_dyn_addn(r, STRCONST("Content-Type: application/"
                                          "x-www-form-urlencoded\r\n"));
      अगर(परिणाम)
        जाओ out;
    }
    परिणाम = addexpect(तथ्य, r, httpversion, &announced_exp100);
    अगर(परिणाम)
      जाओ out;
    अवरोध;
  शेष:
    अवरोध;
  }

  Curl_pgrs_Set_Upload_आकार(तथ्य, req_clen);
  अगर(announced_exp100)
    परिणाम = http_exp100_add_reader(तथ्य);

out:
  वापस परिणाम;
}

#अगर_अघोषित CURL_DISABLE_COOKIES

अटल CURLcode http_cookies(काष्ठा Curl_easy *तथ्य,
                             काष्ठा dynbuf *r)
{
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर *addcookies = शून्य;
  bool linecap = FALSE;
  अगर(CURL_EASY_STR(तथ्य, STRING_COOKIE) &&
     !Curl_checkheaders(तथ्य, STRCONST("Cookie")) &&
     Curl_auth_allowed_to_host(तथ्य))
    addcookies = CURL_EASY_STR(तथ्य, STRING_COOKIE);

  अगर(तथ्य->cookies || addcookies) {
    काष्ठा Curl_llist list;
    पूर्णांक गिनती = 0;

    अगर(तथ्य->cookies && तथ्य->state.cookie_engine) {
      bool okay;
      स्थिर अक्षर *host = तथ्य->req.cookiehost ?
        तथ्य->req.cookiehost : तथ्य->state.origin->hostname;
      Curl_share_lock(तथ्य, CURL_LOCK_तथ्य_COOKIE, CURL_LOCK_ACCESS_SINGLE);
      परिणाम = Curl_cookie_getlist(तथ्य, &okay, host, &list);
      अगर(!परिणाम && okay) {
        काष्ठा Curl_llist_गाँठ *n;
        माप_प्रकार clen = 8; /* hold the size of the generated Cookie: header */

        /* loop through all cookies that matched */
        क्रम(n = Curl_llist_शीर्ष(&list); n; n = Curl_गाँठ_अग्र(n)) {
          काष्ठा Cookie *co = Curl_गाँठ_elem(n);
          अगर(co->मूल्य) {
            माप_प्रकार add;
            अगर(!गिनती) {
              परिणाम = curlx_dyn_addn(r, STRCONST("Cookie: "));
              अगर(परिणाम)
                अवरोध;
            }
            add = म_माप(co->name) + म_माप(co->मूल्य) + 1;
            अगर(clen + add >= MAX_COOKIE_HEADER_LEN) {
              infof(तथ्य, "Restricted outgoing cookies due to header size, "
                    "'%s' not sent", co->name);
              linecap = TRUE;
              अवरोध;
            }
            परिणाम = curlx_dyn_addf(r, "%s%s=%s", गिनती ? "; " : "",
                                    co->name, co->मूल्य);
            अगर(परिणाम)
              अवरोध;
            clen += add + (गिनती ? 2 : 0);
            गिनती++;
          }
        }
        Curl_llist_destroy(&list, शून्य);
      }
      Curl_share_unlock(तथ्य, CURL_LOCK_तथ्य_COOKIE);
    }
    अगर(addcookies && !परिणाम && !linecap) {
      अगर(!गिनती)
        परिणाम = curlx_dyn_addn(r, STRCONST("Cookie: "));
      अगर(!परिणाम) {
        परिणाम = curlx_dyn_addf(r, "%s%s", गिनती ? "; " : "", addcookies);
        गिनती++;
      }
    }
    अगर(गिनती && !परिणाम)
      परिणाम = curlx_dyn_addn(r, STRCONST("\r\n"));

    अगर(परिणाम)
      वापस परिणाम;
  }
  वापस परिणाम;
}
#अन्यथा
#घोषणा http_cookies(a, b) CURLE_OK
#पूर्ण_अगर

अटल CURLcode http_range(काष्ठा Curl_easy *तथ्य,
                           Curl_HttpReq httpreq)
{
  अगर(तथ्य->state.use_range) {
    /*
     * A range is selected. We use different headers whether we are downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    अगर(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_शीर्ष)) &&
       !Curl_checkheaders(तथ्य, STRCONST("Range"))) {
      /* if a line like this was already allocated, free the previous one */
      curlx_free(तथ्य->state.rangeline);
      तथ्य->state.rangeline = curl_maprintf("Range: bytes=%s\r\n",
                                                 तथ्य->state.range);
      अगर(!तथ्य->state.rangeline)
        वापस CURLE_OUT_OF_MEMORY;
    }
    अन्यथा अगर((httpreq == HTTPREQ_POST || httpreq == HTTPREQ_PUT) &&
            !Curl_checkheaders(तथ्य, STRCONST("Content-Range"))) {
      curl_off_t req_clen = Curl_creader_योग_लंबाई(तथ्य);
      /* if a line like this was already allocated, free the previous one */
      curlx_free(तथ्य->state.rangeline);

      अगर(तथ्य->set.set_resume_from < 0) {
        /* Upload resume was asked for, but we do not know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        तथ्य->state.rangeline =
          curl_maprintf("Content-Range: bytes 0-%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n", req_clen - 1, req_clen);
      }
      अन्यथा अगर(तथ्य->state.resume_from) {
        /* This is because "resume" was selected */
        /* Not sure if we want to send this header during authentication
         * negotiation, but test1084 checks for it. In which case we have a
         * "null" client reader installed that gives an unexpected length. */
        curl_off_t योग_len = तथ्य->req.authneg ?
                               तथ्य->state.infilesize :
                               (तथ्य->state.resume_from + req_clen);
        तथ्य->state.rangeline =
          curl_maprintf("Content-Range: bytes %s%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n",
                        तथ्य->state.range, योग_len - 1, योग_len);
      }
      अन्यथा {
        /* Range was selected and then we pass the incoming range and append
           total size */
        तथ्य->state.rangeline =
          curl_maprintf("Content-Range: bytes %s/%" FMT_OFF_T "\r\n",
                        तथ्य->state.range, req_clen);
      }
      अगर(!तथ्य->state.rangeline)
        वापस CURLE_OUT_OF_MEMORY;
    }
  }
  वापस CURLE_OK;
}

अटल CURLcode http_firstwrite(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा SingleRequest *k = &तथ्य->req;

  अगर(तथ्य->req.newurl) {
    अगर(conn->bits.बंद) {
      /* Abort after the headers if "follow Location" is set
         and we are set to close anyway. */
      CURL_REQ_CLEAR_RECV(तथ्य);
      k->done = TRUE;
      वापस CURLE_OK;
    }
    /* We have a new URL to load, but since we want to be able to reuse this
       connection properly, we read the full response in "ignore more" */
    k->ignorebody = TRUE;
    infof(तथ्य, "Ignoring the response-body");
  }
  अगर(तथ्य->state.resume_from && !k->content_range &&
     (तथ्य->state.httpreq == HTTPREQ_GET) &&
     !k->ignorebody) {

    अगर(k->आकार == तथ्य->state.resume_from) {
      /* The resume point is at the end of file, consider this fine even if it
         does not allow resume from here. */
      infof(तथ्य, "The entire document is already downloaded");
      streamclose(conn);
      /* Abort download */
      CURL_REQ_CLEAR_RECV(तथ्य);
      k->done = TRUE;
      वापस CURLE_OK;
    }

    /* we wanted to resume a download, although the server does not seem to
     * support this and we did this with a GET (if it was not a GET we did a
     * POST or PUT resume) */
    failf(तथ्य, "HTTP server does not seem to support "
          "byte ranges. Cannot resume.");
    वापस CURLE_RANGE_ERROR;
  }

  अगर(तथ्य->set.timecondition && !तथ्य->state.range &&
     /* A time condition has been set AND no ranges have been requested. This
        seems to be what chapter 13.3.4 of RFC 2616 defines to be the correct
        action for an HTTP/1.1 client */
     !Curl_meets_timecondition(तथ्य, k->timeofdoc)) {
    k->done = TRUE;
    /* We are simulating an HTTP 304 from server so we return
       what should have been returned from the server */
    तथ्य->info.httpcode = 304;
    infof(तथ्य, "Simulate an HTTP 304 response");
    /* we abort the transfer before it is completed == we ruin the
       reuse ability. Close the connection */
    streamclose(conn);
    वापस CURLE_OK;
  } /* we have a time condition */

  वापस CURLE_OK;
}

अटल CURLcode http_check_new_conn(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  स्थिर अक्षर *info_version = शून्य;
  स्थिर अक्षर *alpn;
  CURLcode परिणाम;

  alpn = Curl_conn_get_alpn_negotiated(तथ्य, conn);
  अगर(alpn && !म_भेद("h3", alpn)) {
#अगर_अघोषित CURL_DISABLE_PROXY
    अगर(!conn->bits.origin_is_proxy)
#पूर्ण_अगर
      DEBUGASSERT(Curl_conn_http_version(तथ्य, conn) == 30);
    info_version = "HTTP/3";
  }
  अन्यथा अगर(alpn && !म_भेद("h2", alpn)) {
#अगर_अघोषित CURL_DISABLE_PROXY
    अगर((Curl_conn_http_version(तथ्य, conn) != 20) &&
       conn->bits.origin_is_proxy) {
      परिणाम = Curl_http2_switch(तथ्य);
      अगर(परिणाम)
        वापस परिणाम;
    }
    अन्यथा
#पूर्ण_अगर
    DEBUGASSERT(Curl_conn_http_version(तथ्य, conn) == 20);
    info_version = "HTTP/2";
  }
  अन्यथा {
    /* Check if user wants to use HTTP/2 with clear TCP */
    अगर(Curl_http2_may_switch(तथ्य)) {
      DEBUGF(infof(तथ्य, "HTTP/2 over clean TCP"));
      परिणाम = Curl_http2_switch(तथ्य);
      अगर(परिणाम)
        वापस परिणाम;
      info_version = "HTTP/2";
      /* There is no ALPN here, but the connection is now definitely h2 */
      conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(conn);
    }
    अन्यथा
      info_version = "HTTP/1.x";
  }

  अगर(info_version)
    infof(तथ्य, "using %s", info_version);
  वापस CURLE_OK;
}

अटल CURLcode http_add_connection_hd(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा dynbuf *req)
{
  काष्ठा curl_slist *शीर्ष;
  स्थिर अक्षर *sep = "Connection: ";
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार rlen = curlx_dyn_len(req);
  bool skip;

  /* Add the 1st custom "Connection: " header, if there is one */
  क्रम(शीर्ष = तथ्य->set.headers; शीर्ष; शीर्ष = शीर्ष->अग्र) {
    अगर(curl_strnequal(शीर्ष->तथ्य, "Connection", 10) &&
       Curl_headersep(शीर्ष->तथ्य[10]) &&
       !http_header_is_empty(शीर्ष->तथ्य)) {
      अक्षर *मूल्य;
      परिणाम = copy_custom_मूल्य(शीर्ष->तथ्य, &मूल्य);
      अगर(परिणाम)
        वापस परिणाम;
      परिणाम = curlx_dyn_addf(req, "%s%s", sep, मूल्य);
      sep = ", ";
      curlx_free(मूल्य);
      अवरोध; /* leave, having added 1st one */
    }
  }

  /* add our internal Connection: header values, if we have any */
  अगर(!परिणाम && तथ्य->state.http_hd_te) {
    परिणाम = curlx_dyn_addf(req, "%s%s", sep, "TE");
    sep = ", ";
  }
  अगर(!परिणाम && तथ्य->state.http_hd_upgrade) {
    परिणाम = curlx_dyn_addf(req, "%s%s", sep, "Upgrade");
    sep = ", ";
  }
  अगर(!परिणाम && तथ्य->state.http_hd_h2_settings) {
    परिणाम = curlx_dyn_addf(req, "%s%s", sep, "HTTP2-Settings");
  }
  अगर(!परिणाम && (rlen < curlx_dyn_len(req)))
    परिणाम = curlx_dyn_addn(req, STRCONST("\r\n"));
  अगर(परिणाम)
    वापस परिणाम;

  /* Add all user-defined Connection: headers after the first */
  skip = TRUE;
  क्रम(शीर्ष = तथ्य->set.headers; शीर्ष; शीर्ष = शीर्ष->अग्र) {
    अगर(curl_strnequal(शीर्ष->तथ्य, "Connection", 10) &&
       Curl_headersep(शीर्ष->तथ्य[10]) &&
       !http_header_is_empty(शीर्ष->तथ्य)) {
      अगर(skip) {
        skip = FALSE;
        जारी;
      }
      परिणाम = curlx_dyn_addf(req, "%s\r\n", शीर्ष->तथ्य);
      अगर(परिणाम)
        वापस परिणाम;
    }
  }

  वापस CURLE_OK;
}

/* Header identifier in order we send them by default */
प्रकार क्रमागत {
  H1_HD_REQUEST,
  H1_HD_HOST,
#अगर_अघोषित CURL_DISABLE_PROXY
  H1_HD_PROXY_AUTH,
#पूर्ण_अगर
  H1_HD_AUTH,
  H1_HD_RANGE,
  H1_HD_USER_AGENT,
  H1_HD_ACCEPT,
  H1_HD_TE,
  H1_HD_ACCEPT_ENCODING,
  H1_HD_REFERER,
#अगर_अघोषित CURL_DISABLE_PROXY
  H1_HD_PROXY_CONNECTION,
#पूर्ण_अगर
  H1_HD_TRANSFER_ENCODING,
#अगर_अघोषित CURL_DISABLE_ALTSVC
  H1_HD_ALT_USED,
#पूर्ण_अगर
  H1_HD_UPGRADE,
  H1_HD_COOKIES,
  H1_HD_CONDITIONALS,
  H1_HD_CUSTOM,
  H1_HD_CONTENT,
  H1_HD_CONNECTION,
  H1_HD_LAST  /* the last, empty header line */
} http_hd_t;

अटल CURLcode http_add_hd(काष्ठा Curl_easy *तथ्य,
                            काष्ठा dynbuf *req,
                            http_hd_t id,
                            अचिन्हित अक्षर httpversion,
                            स्थिर अक्षर *method,
                            Curl_HttpReq httpreq)
{
  CURLcode परिणाम = CURLE_OK;
#अगर !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_PROXY) || \
  !defined(CURL_DISABLE_WEBSOCKETS)
  काष्ठा connectdata *conn = तथ्य->conn;
#पूर्ण_अगर
  चयन(id) {
  हाल H1_HD_REQUEST:
    /* add the main request stuff */
    /* GET/HEAD/POST/PUT */
    परिणाम = curlx_dyn_addf(req, "%s ", method);
    अगर(!परिणाम)
      परिणाम = http_target(तथ्य, req);
    अगर(!परिणाम)
      परिणाम = curlx_dyn_addf(req, " HTTP/%s\r\n",
                              get_http_string(httpversion));
    अवरोध;

  हाल H1_HD_HOST:
    अगर(तथ्य->state.http_host) {
      परिणाम = curlx_dyn_add(req, तथ्य->state.http_host);
      अगर(!परिणाम)
        परिणाम = curlx_dyn_addn(req, STRCONST("\r\n"));
    }
    अवरोध;

#अगर_अघोषित CURL_DISABLE_PROXY
  हाल H1_HD_PROXY_AUTH:
    अगर(तथ्य->req.hd_proxy_auth)
      परिणाम = curlx_dyn_add(req, तथ्य->req.hd_proxy_auth);
    अवरोध;
#पूर्ण_अगर

  हाल H1_HD_AUTH:
    अगर(तथ्य->req.hd_auth)
      परिणाम = curlx_dyn_add(req, तथ्य->req.hd_auth);
    अवरोध;

  हाल H1_HD_RANGE:
    अगर(तथ्य->state.use_range && तथ्य->state.rangeline)
      परिणाम = curlx_dyn_add(req, तथ्य->state.rangeline);
    अवरोध;

  हाल H1_HD_USER_AGENT: {
    स्थिर अक्षर *ua = CURL_EASY_STR(तथ्य, STRING_USERAGENT);
    अगर(ua && *ua && !Curl_checkheaders(तथ्य, STRCONST("User-Agent")))
      परिणाम = curlx_dyn_addf(req, "User-Agent: %s\r\n", ua);
    अवरोध;
  }

  हाल H1_HD_ACCEPT:
    अगर(!Curl_checkheaders(तथ्य, STRCONST("Accept")))
      परिणाम = curlx_dyn_add(req, "Accept: */*\r\n");
    अवरोध;

  हाल H1_HD_TE:
#अगर_घोषित HAVE_LIBZ
    अगर(!Curl_checkheaders(तथ्य, STRCONST("TE")) &&
       तथ्य->set.http_transfer_encoding) {
      तथ्य->state.http_hd_te = TRUE;
      परिणाम = curlx_dyn_add(req, "TE: gzip\r\n");
    }
#पूर्ण_अगर
    अवरोध;

  हाल H1_HD_ACCEPT_ENCODING: {
    स्थिर अक्षर *enc = CURL_EASY_STR(तथ्य, STRING_ENCODING);
    अगर(enc && !Curl_checkheaders(तथ्य, STRCONST("Accept-Encoding")))
      परिणाम = curlx_dyn_addf(req, "Accept-Encoding: %s\r\n", enc);
    अवरोध;
  }

  हाल H1_HD_REFERER:
    अगर(Curl_bufref_ptr(&तथ्य->state.referer) &&
       !Curl_checkheaders(तथ्य, STRCONST("Referer")))
      परिणाम = curlx_dyn_addf(req, "Referer: %s\r\n",
                              Curl_bufref_ptr(&तथ्य->state.referer));
    अवरोध;

#अगर_अघोषित CURL_DISABLE_PROXY
  हाल H1_HD_PROXY_CONNECTION:
    अगर(conn->bits.origin_is_proxy &&
       !Curl_checkheaders(तथ्य, STRCONST("Proxy-Connection")) &&
       !Curl_checkProxyheaders(तथ्य, तथ्य->conn, STRCONST("Proxy-Connection")))
      परिणाम = curlx_dyn_add(req, "Proxy-Connection: Keep-Alive\r\n");
    अवरोध;
#पूर्ण_अगर

  हाल H1_HD_TRANSFER_ENCODING:
    परिणाम = http_req_set_TE(तथ्य, req, httpversion);
    अवरोध;

#अगर_अघोषित CURL_DISABLE_ALTSVC
  हाल H1_HD_ALT_USED:
    अगर(conn->bits.altused && conn->via_peer &&
       !Curl_checkheaders(तथ्य, STRCONST("Alt-Used")))
      परिणाम = curlx_dyn_addf(req, "Alt-Used: %s:%u\r\n",
                              conn->via_peer->hostname, conn->via_peer->port);
    अवरोध;
#पूर्ण_अगर

  हाल H1_HD_UPGRADE:
    अगर(!Curl_conn_is_ssl(तथ्य->conn, FIRSTSOCKET) && (httpversion < 20) &&
       (तथ्य->state.http_neg.wanted & CURL_HTTP_V2x) &&
       तथ्य->state.http_neg.h2_upgrade) {
      /* append HTTP2 upgrade magic stuff to the HTTP request if it is not done
         over SSL */
      परिणाम = Curl_http2_request_upgrade(req, तथ्य);
    }
#अगर_अघोषित CURL_DISABLE_WEBSOCKETS
    अगर(!परिणाम && conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
      परिणाम = Curl_ws_request(तथ्य, req);
#पूर्ण_अगर
    अवरोध;

  हाल H1_HD_COOKIES:
    परिणाम = http_cookies(तथ्य, req);
    अवरोध;

  हाल H1_HD_CONDITIONALS:
    परिणाम = Curl_add_timecondition(तथ्य, req);
    अवरोध;

  हाल H1_HD_CUSTOM:
    परिणाम = Curl_add_custom_headers(तथ्य, FALSE, httpversion, req);
    अवरोध;

  हाल H1_HD_CONTENT:
    परिणाम = http_add_content_hds(तथ्य, req, httpversion, httpreq);
    अवरोध;

  हाल H1_HD_CONNECTION: {
    परिणाम = http_add_connection_hd(तथ्य, req);
    अवरोध;
  }

  हाल H1_HD_LAST:
    परिणाम = curlx_dyn_addn(req, STRCONST("\r\n"));
    अवरोध;
  }
  वापस परिणाम;
}

/*
 * Curl_http() gets called from the generic multi_do() function when an HTTP
 * request is to be performed. This creates and sends a properly constructed
 * HTTP request.
 */
CURLcode Curl_http(काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम = CURLE_OK;
  Curl_HttpReq httpreq;
  स्थिर अक्षर *method;
  काष्ठा dynbuf req;
  अचिन्हित अक्षर httpversion;
  माप_प्रकार hd_id;

  /* Always consider the DO phase done after this function call, even if there
     may be parts of the request that are not yet sent, since we can deal with
     the rest of the request in the PERFORM phase. */
  *done = TRUE;
  /* initialize a dynamic send-buffer */
  curlx_dyn_init(&req, DYN_HTTP_REQUEST);
  /* make sure the header buffer is reset - if there are leftovers from a
     previous transfer */
  curlx_dyn_reset(&तथ्य->state.headerb);
  तथ्य->state.maybe_folded = FALSE;

  अगर(!तथ्य->conn->bits.reuse) {
    परिणाम = http_check_new_conn(तथ्य);
    अगर(परिणाम)
      जाओ out;
  }

  /* Add collecting of headers written to client. For a new connection,
   * we might have done that already, but reuse
   * or multiplex needs it here as well. */
  परिणाम = Curl_headers_init(तथ्य);
  अगर(परिणाम)
    जाओ out;

  तथ्य->state.http_hd_te = FALSE;
  तथ्य->state.http_hd_upgrade = FALSE;
  तथ्य->state.http_hd_h2_settings = FALSE;

  /* what kind of request do we need to send? */
  Curl_http_method(तथ्य, &method, &httpreq);

  /* select host to send */
  परिणाम = http_set_aptr_host(तथ्य);
  /* setup the authentication headers, how that method and host are known */
  अगर(!परिणाम)
    परिणाम = Curl_http_output_auth(तथ्य, तथ्य->conn, method, httpreq,
                                   तथ्य->state.up.path,
                                   तथ्य->state.up.query, FALSE);
  /* Setup input reader, resume information and ranges */
  अगर(!परिणाम)
    परिणाम = set_reader(तथ्य, httpreq);
  अगर(!परिणाम)
    परिणाम = http_resume(तथ्य, httpreq);
  अगर(!परिणाम)
    परिणाम = http_range(तथ्य, httpreq);
  अगर(परिणाम)
    जाओ out;

  httpversion = http_request_version(तथ्य);
  /* Add request line and all headers to `req` */
  क्रम(hd_id = 0; hd_id <= H1_HD_LAST; ++hd_id) {
    परिणाम = http_add_hd(तथ्य, &req, (http_hd_t)hd_id,
                         httpversion, method, httpreq);
    अगर(परिणाम)
      जाओ out;
  }

  /* setup variables for the upcoming transfer and send */
  Curl_xfer_setup_sendrecv(तथ्य, FIRSTSOCKET, -1);
  परिणाम = Curl_req_send(तथ्य, &req, httpversion);

  अगर((httpversion >= 20) && तथ्य->req.upload_chunky)
    /* upload_chunky was set above to set up the request in a chunky fashion,
       but is disabled here again to avoid that the chunked encoded version is
       actually used when sending the request body over h2 */
    तथ्य->req.upload_chunky = FALSE;

out:
  अगर(परिणाम == CURLE_TOO_LARGE)
    failf(तथ्य, "HTTP request too large");

  curlx_dyn_free(&req);
  वापस परिणाम;
}

प्रकार क्रमागत {
  STATUS_UNKNOWN, /* not enough data to tell yet */
  STATUS_DONE, /* a status line was read */
  STATUS_BAD /* not a status line */
} statusline;

/* Check a string for a prefix. Check no more than 'len' bytes */
अटल bool checkprefixmax(स्थिर अक्षर *prefix, स्थिर अक्षर *बफर, माप_प्रकार len)
{
  माप_प्रकार ch = CURLMIN(म_माप(prefix), len);
  वापस curl_strnequal(prefix, बफर, ch);
}

/*
 * checkhttpprefix()
 *
 * Returns TRUE if member of the list matches prefix of string
 */
अटल statusline checkhttpprefix(काष्ठा Curl_easy *तथ्य,
                                  स्थिर अक्षर *s, माप_प्रकार len)
{
  काष्ठा curl_slist *शीर्ष = तथ्य->set.http200aliases;
  statusline rc = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;

  जबतक(शीर्ष) {
    अगर(checkprefixmax(शीर्ष->तथ्य, s, len)) {
      rc = onmatch;
      अवरोध;
    }
    शीर्ष = शीर्ष->अग्र;
  }

  अगर((rc != STATUS_DONE) && checkprefixmax("HTTP/", s, len))
    rc = onmatch;

  वापस rc;
}

#अगर_अघोषित CURL_DISABLE_RTSP
अटल statusline checkrtspprefix(काष्ठा Curl_easy *तथ्य,
                                  स्थिर अक्षर *s, माप_प्रकार len)
{
  statusline status = STATUS_BAD;
  statusline onmatch = len >= 5 ? STATUS_DONE : STATUS_UNKNOWN;
  (व्योम)तथ्य;
  अगर(checkprefixmax("RTSP/", s, len))
    status = onmatch;

  वापस status;
}
#पूर्ण_अगर /* CURL_DISABLE_RTSP */

अटल statusline checkprotoprefix(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा connectdata *conn,
                                   स्थिर अक्षर *s, माप_प्रकार len)
{
#अगर_अघोषित CURL_DISABLE_RTSP
  अगर(conn->scheme->protocol & CURLPROTO_RTSP)
    वापस checkrtspprefix(तथ्य, s, len);
#अन्यथा
  (व्योम)conn;
#पूर्ण_अगर /* CURL_DISABLE_RTSP */

  वापस checkhttpprefix(तथ्य, s, len);
}

/* HTTP header has field name `n` (a string constant) */
#घोषणा HD_IS(hd, hdlen, n) \
  (((hdlen) >= (माप(n) - 1)) && curl_strnequal(n, hd, माप(n) - 1))

#घोषणा HD_VAL(hd, hdlen, n) \
  ((((hdlen) >= (माप(n) - 1)) && (hd) && \
    curl_strnequal(n, hd, माप(n) - 1)) ? ((hd) + (माप(n) - 1)) : शून्य)

/* HTTP header has field name `n` (a string constant) and contains `v`
 * (a string constant) in its value(s) */
#घोषणा HD_IS_AND_SAYS(hd, hdlen, n, v) \
  (HD_IS(hd, hdlen, n) && \
   ((hdlen) > ((माप(n) - 1) + (माप(v) - 1))) && \
   Curl_compareheader(hd, STRCONST(n), STRCONST(v)))

/*
 * http_header_a() parses a single response header starting with A.
 */
अटल CURLcode http_header_a(काष्ठा Curl_easy *तथ्य,
                              स्थिर अक्षर *hd, माप_प्रकार hdlen)
{
#अगर_अघोषित CURL_DISABLE_ALTSVC
  स्थिर अक्षर *v;
  v = (तथ्य->asi &&
       (Curl_xfer_is_secure(तथ्य) ||
#अगर_घोषित DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        दो_पर्या("CURL_ALTSVC_HTTP")
#अन्यथा
        0
#पूर्ण_अगर
         )) ? HD_VAL(hd, hdlen, "Alt-Svc:") : शून्य;
  अगर(v) {
    /* the ALPN of the current request */
    काष्ठा SingleRequest *k = &तथ्य->req;
    क्रमागत alpnid id = (k->httpversion == 30) ? ALPN_h3 :
      (k->httpversion == 20) ? ALPN_h2 : ALPN_h1;
    वापस Curl_altsvc_parse(तथ्य, तथ्य->asi, v, तथ्य->state.origin, id);
  }
#अन्यथा
  (व्योम)तथ्य;
  (व्योम)hd;
  (व्योम)hdlen;
#पूर्ण_अगर
  वापस CURLE_OK;
}

/*
 * http_header_c() parses a single response header starting with C.
 */
अटल CURLcode http_header_c(काष्ठा Curl_easy *तथ्य,
                              स्थिर अक्षर *hd, माप_प्रकार hdlen)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा SingleRequest *k = &तथ्य->req;
  स्थिर अक्षर *v;

  /* Check for Content-Length: header lines to get size. Browsers insist we
     should accept multiple Content-Length headers and that a comma separated
     list also is fine and then we should accept them all as long as they are
     the same value. Different values trigger error.
   */
  v = (!k->http_bodyless && !तथ्य->set.ignorecl && !k->ignore_cl) ?
    HD_VAL(hd, hdlen, "Content-Length:") : शून्य;
  अगर(v) {
    करो {
      curl_off_t contentlength;
      पूर्णांक offt = curlx_str_numblanks(&v, &contentlength);

      अगर(offt == STRE_OVERFLOW) {
        /* out of range */
        अगर(तथ्य->set.max_filesize) {
          failf(तथ्य, "Maximum file size exceeded");
          वापस CURLE_FILESIZE_EXCEEDED;
        }
        streamclose(conn);
        infof(तथ्य, "Overflow Content-Length: value");
        वापस CURLE_OK;
      }
      अन्यथा {
        अगर((offt == STRE_OK) &&
           ((k->आकार == -1) || /* not set to something before */
            (k->आकार == contentlength))) { /* or the same value */

          k->आकार = contentlength;
          curlx_str_passblanks(&v);

          /* on a comma, loop and get the next instead */
          अगर(!curlx_str_single(&v, ','))
            जारी;

          अगर(!curlx_str_newline(&v)) {
            k->maxdownload = k->आकार;
            वापस CURLE_OK;
          }
        }
        /* negative, different value or rubbish - bad HTTP */
        failf(तथ्य, "Invalid Content-Length: value");
        वापस CURLE_WEIRD_SERVER_REPLY;
      }
    } जबतक(1);
  }
  v = (!k->http_bodyless && CURL_EASY_STR(तथ्य, STRING_ENCODING)) ?
    HD_VAL(hd, hdlen, "Content-Encoding:") : शून्य;
  अगर(v) {
    /*
     * Process Content-Encoding. Look for the values: identity, gzip, deflate,
     * compress, x-gzip and x-compress. x-gzip and x-compress are the same as
     * gzip and compress. (Sec 3.5 RFC 2616). zlib cannot handle compress.
     * Errors are handled further down when the response body is processed
     */
    वापस Curl_build_unencoding_ढेर(तथ्य, v, FALSE);
  }
  /* check for Content-Type: header lines to get the MIME-type */
  v = HD_VAL(hd, hdlen, "Content-Type:");
  अगर(v) {
    अक्षर *contenttype = Curl_copy_header_मूल्य(hd);
    अगर(!contenttype)
      वापस CURLE_OUT_OF_MEMORY;
    अगर(!*contenttype)
      /* ignore empty data */
      curlx_free(contenttype);
    अन्यथा {
      curlx_free(तथ्य->info.contenttype);
      तथ्य->info.contenttype = contenttype;
    }
    वापस CURLE_OK;
  }
  अगर((k->httpversion < 20) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "close")) {
    /*
     * [RFC 2616, section 8.1.2.1]
     * "Connection: close" is HTTP/1.1 language and means that
     * the connection will close when this request has been
     * served.
     */
    connclose(conn);
    वापस CURLE_OK;
  }
  अगर((k->httpversion == 10) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "keep-alive")) {
    /*
     * An HTTP/1.0 reply with the 'Connection: keep-alive' line
     * tells us the connection will be kept alive for our
     * pleasure. Default action for 1.0 is to close.
     *
     * [RFC2068, section 19.7.1] */
    connkeep(conn);
    infof(तथ्य, "HTTP/1.0 connection set to keep alive");
    वापस CURLE_OK;
  }
  v = !k->http_bodyless ? HD_VAL(hd, hdlen, "Content-Range:") : शून्य;
  अगर(v) {
    /* Content-Range: bytes [num]-
       Content-Range: bytes: [num]-
       Content-Range: [num]-
       Content-Range: [asterisk]/[total]

       The second format was added since Sun's webserver
       JavaWebServer/1.1.1 obviously sends the header this way!
       The third added since some servers use that!
       The fourth means the requested range was unsatisfied.
     */

    स्थिर अक्षर *ptr = v;

    /* Move forward until first digit or asterisk */
    जबतक(*ptr && !ISDIGIT(*ptr) && *ptr != '*')
      ptr++;

    /* if it truly stopped on a digit */
    अगर(ISDIGIT(*ptr)) {
      अगर(!curlx_str_संख्या(&ptr, &k->offset, CURL_OFF_T_MAX) &&
         (तथ्य->state.resume_from == k->offset))
        /* we asked for a resume and we got it */
        k->content_range = TRUE;
    }
    अन्यथा अगर(k->httpcode < 300)
      तथ्य->state.resume_from = 0; /* get everything */
  }
  वापस CURLE_OK;
}

/*
 * http_header_l() parses a single response header starting with L.
 */
अटल CURLcode http_header_l(काष्ठा Curl_easy *तथ्य,
                              स्थिर अक्षर *hd, माप_प्रकार hdlen)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा SingleRequest *k = &तथ्य->req;
  स्थिर अक्षर *v = (!k->http_bodyless &&
                   (तथ्य->set.timecondition || तथ्य->set.get_filetime)) ?
    HD_VAL(hd, hdlen, "Last-Modified:") : शून्य;
  अगर(v) {
    अगर(Curl_getdate_capped(v, &k->timeofdoc))
      k->timeofdoc = 0;
    अगर(तथ्य->set.get_filetime)
      तथ्य->info.filetime = k->timeofdoc;
    वापस CURLE_OK;
  }
  अगर(HD_IS(hd, hdlen, "Location:")) {
    /* this is the URL that the server advises us to use instead */
    अक्षर *location = Curl_copy_header_मूल्य(hd);
    अगर(!location)
      वापस CURLE_OUT_OF_MEMORY;
    अगर(!*location ||
       (तथ्य->req.location && !म_भेद(तथ्य->req.location, location))) {
      /* ignore empty header, or exact repeat of a previous one */
      curlx_free(location);
      वापस CURLE_OK;
    }
    अन्यथा {
      /* has value and is not an exact repeat */
      अगर(तथ्य->req.location) {
        failf(तथ्य, "Multiple Location headers");
        curlx_free(location);
        वापस CURLE_WEIRD_SERVER_REPLY;
      }
      तथ्य->req.location = location;

      अगर((k->httpcode >= 300 && k->httpcode < 400) &&
         तथ्य->set.http_follow_mode) {
        CURLcode परिणाम;
        DEBUGASSERT(!तथ्य->req.newurl);
        तथ्य->req.newurl = curlx_strdup(तथ्य->req.location); /* clone */
        अगर(!तथ्य->req.newurl)
          वापस CURLE_OUT_OF_MEMORY;

        /* some cases of POST and PUT etc needs to rewind the data
           stream at this point */
        परिणाम = http_perhapsrewind(तथ्य, conn);
        अगर(परिणाम)
          वापस परिणाम;

        /* mark the next request as a followed location: */
        तथ्य->state.this_is_a_follow = TRUE;
      }
    }
  }
  वापस CURLE_OK;
}

/*
 * http_header_p() parses a single response header starting with P.
 */
अटल CURLcode http_header_p(काष्ठा Curl_easy *तथ्य,
                              स्थिर अक्षर *hd, माप_प्रकार hdlen)
{
  काष्ठा SingleRequest *k = &तथ्य->req;

#अगर_अघोषित CURL_DISABLE_PROXY
  स्थिर अक्षर *v = HD_VAL(hd, hdlen, "Proxy-Connection:");
  अगर(v) {
    काष्ठा connectdata *conn = तथ्य->conn;
    अगर((k->httpversion == 10) && conn->http_proxy.peer &&
       HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "keep-alive")) {
      /*
       * When an HTTP/1.0 reply comes when using a proxy, the
       * 'Proxy-Connection: keep-alive' line tells us the
       * connection will be kept alive for our pleasure.
       * Default action for 1.0 is to close.
       */
      connkeep(conn); /* do not close */
      infof(तथ्य, "HTTP/1.0 proxy connection set to keep alive");
    }
    अन्यथा अगर((k->httpversion == 11) && conn->http_proxy.peer &&
            HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "close")) {
      /*
       * We get an HTTP/1.1 response from a proxy and it says it will
       * close down after this transfer.
       */
      connclose(conn);
      infof(तथ्य, "HTTP/1.1 proxy connection set close");
    }
    वापस CURLE_OK;
  }
#पूर्ण_अगर
  अगर((407 == k->httpcode) && HD_IS(hd, hdlen, "Proxy-authenticate:")) {
    अक्षर *auth;
    CURLcode परिणाम;
#अगर_अघोषित CURL_DISABLE_PROXY
    अगर(!तथ्य->conn->bits.origin_is_proxy)
      /* this response is not from a forward proxy, not for us */
      वापस CURLE_OK;
#पूर्ण_अगर
    auth = Curl_copy_header_मूल्य(hd);
    परिणाम = auth ? CURLE_OK : CURLE_OUT_OF_MEMORY;
    अगर(!परिणाम) {
      परिणाम = Curl_http_निवेश_auth(तथ्य, TRUE, auth);
      curlx_free(auth);
    }
    वापस परिणाम;
  }
#अगर_घोषित USE_SPNEGO
  अगर(HD_IS(hd, hdlen, "Persistent-Auth:")) {
    काष्ठा connectdata *conn = तथ्य->conn;
    काष्ठा negotiatedata *negdata = Curl_auth_nego_get(conn, FALSE);
    काष्ठा auth *authp = &तथ्य->state.authhost;
    अगर(!negdata)
      वापस CURLE_OUT_OF_MEMORY;
    अगर(authp->picked == CURLAUTH_NEGOTIATE) {
      अक्षर *persistentauth = Curl_copy_header_मूल्य(hd);
      अगर(!persistentauth)
        वापस CURLE_OUT_OF_MEMORY;
      negdata->noauthpersist = !!checkprefix("false", persistentauth);
      negdata->havenoauthpersist = TRUE;
      infof(तथ्य, "Negotiate: noauthpersist -> %d, header part: %s",
            negdata->noauthpersist, persistentauth);
      curlx_free(persistentauth);
    }
  }
#पूर्ण_अगर
  वापस CURLE_OK;
}

/*
 * http_header_r() parses a single response header starting with R.
 */
अटल CURLcode http_header_r(काष्ठा Curl_easy *तथ्य,
                              स्थिर अक्षर *hd, माप_प्रकार hdlen)
{
  स्थिर अक्षर *v = HD_VAL(hd, hdlen, "Retry-After:");
  अगर(v) {
    /* Retry-After = HTTP-date / delay-seconds */
    curl_off_t retry_after = 0; /* zero for unknown or "now" */
    समय_प्रकार date = 0;
    curlx_str_passblanks(&v);

    /* try it as a date first, because a date can otherwise start with and
       get treated as a number */
    अगर(!Curl_getdate_capped(v, &date)) {
      समय_प्रकार current = समय(शून्य);
      अगर(date >= current)
        /* convert date to number of seconds into the future */
        retry_after = date - current;
    }
    अन्यथा
      /* Try it as a decimal number, ignore errors */
      (व्योम)curlx_str_संख्या(&v, &retry_after, CURL_OFF_T_MAX);
    /* limit to 6 hours max. this is not documented so that it can be changed
       in the future if necessary. */
    अगर(retry_after > 21600)
      retry_after = 21600;
    तथ्य->info.retry_after = retry_after;
  }
  वापस CURLE_OK;
}

/*
 * http_header_s() parses a single response header starting with S.
 */
अटल CURLcode http_header_s(काष्ठा Curl_easy *तथ्य,
                              स्थिर अक्षर *hd, माप_प्रकार hdlen)
{
#अगर !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_HSTS)
  स्थिर अक्षर *v;
#अन्यथा
  (व्योम)तथ्य;
  (व्योम)hd;
  (व्योम)hdlen;
#पूर्ण_अगर

#अगर_अघोषित CURL_DISABLE_COOKIES
  v = (तथ्य->cookies && तथ्य->state.cookie_engine) ?
    HD_VAL(hd, hdlen, "Set-Cookie:") : शून्य;
  अगर(v) {
    /* If there is a custom-set Host: name, use it here, or else use
     * real peer hostname. */
    स्थिर अक्षर *host = तथ्य->req.cookiehost ?
      तथ्य->req.cookiehost : तथ्य->state.origin->hostname;
    स्थिर अचिन्हित अक्षर secure_context = Curl_secure_context(तथ्य, host) ?
      COOKIE_SECURE : 0;
    CURLcode परिणाम;
    Curl_share_lock(तथ्य, CURL_LOCK_तथ्य_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    परिणाम = Curl_cookie_add(तथ्य, तथ्य->cookies, v, host,
                             तथ्य->state.up.path,
                             COOKIE_HTTPHEADER | secure_context);
    Curl_share_unlock(तथ्य, CURL_LOCK_तथ्य_COOKIE);
    वापस परिणाम;
  }
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HSTS
  /* If enabled, the header is incoming and this is over HTTPS */
  v = (तथ्य->hsts &&
       (Curl_xfer_is_secure(तथ्य) ||
#अगर_घोषित DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        दो_पर्या("CURL_HSTS_HTTP")
#अन्यथा
        0
#पूर्ण_अगर
         )
    ) ? HD_VAL(hd, hdlen, "Strict-Transport-Security:") : शून्य;
  अगर(v) {
    CURLcode परिणाम = Curl_hsts_parse(
      तथ्य->hsts, तथ्य->state.origin->hostname, v);
    अगर(परिणाम) {
      अगर(परिणाम == CURLE_OUT_OF_MEMORY)
        वापस परिणाम;
      infof(तथ्य, "Illegal STS header skipped");
    }
#अगर_घोषित DEBUGBUILD
    अन्यथा
      infof(तथ्य, "Parsed STS header fine (%zu entries)",
            Curl_llist_गिनती(&तथ्य->hsts->list));
#पूर्ण_अगर
  }
#पूर्ण_अगर

  वापस CURLE_OK;
}

/*
 * http_header_t() parses a single response header starting with T.
 */
अटल CURLcode http_header_t(काष्ठा Curl_easy *तथ्य,
                              स्थिर अक्षर *hd, माप_प्रकार hdlen)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा SingleRequest *k = &तथ्य->req;

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
  स्थिर अक्षर *v = (!k->http_bodyless &&
                   (तथ्य->state.httpreq != HTTPREQ_शीर्ष) &&
                   (k->httpcode != 304)) ?
    HD_VAL(hd, hdlen, "Transfer-Encoding:") : शून्य;
  अगर(v) {
    /* One or more encodings. We check for chunked and/or a compression
       algorithm. */
    CURLcode परिणाम = Curl_build_unencoding_ढेर(तथ्य, v, TRUE);
    अगर(परिणाम)
      वापस परिणाम;
    अगर(!k->chunk) {
      /* if this is not chunked, only close can signal the end of this
       * transfer as Content-Length is said not to be trusted for
       * transfer-encoding! */
      CURL_TRC_M(तथ्य, "HTTP/1.1 transfer-encoding without chunks");
      connclose(conn);
      k->ignore_cl = TRUE;
    }
    वापस CURLE_OK;
  }
  v = HD_VAL(hd, hdlen, "Trailer:");
  अगर(v) {
    तथ्य->req.resp_trailer = TRUE;
    वापस CURLE_OK;
  }
  वापस CURLE_OK;
}

/*
 * http_header_w() parses a single response header starting with W.
 */
अटल CURLcode http_header_w(काष्ठा Curl_easy *तथ्य,
                              स्थिर अक्षर *hd, माप_प्रकार hdlen)
{
  काष्ठा SingleRequest *k = &तथ्य->req;
  CURLcode परिणाम = CURLE_OK;

  अगर((401 == k->httpcode) && HD_IS(hd, hdlen, "WWW-Authenticate:")) {
    अक्षर *auth = Curl_copy_header_मूल्य(hd);
    अगर(!auth)
      परिणाम = CURLE_OUT_OF_MEMORY;
    अन्यथा {
      परिणाम = Curl_http_निवेश_auth(तथ्य, FALSE, auth);
      curlx_free(auth);
    }
  }
  वापस परिणाम;
}

/*
 * http_header() parses a single response header.
 */
अटल CURLcode http_header(काष्ठा Curl_easy *तथ्य,
                            स्थिर अक्षर *hd, माप_प्रकार hdlen)
{
  CURLcode परिणाम = CURLE_OK;

  चयन(hd[0]) {
  हाल 'a':
  हाल 'A':
    परिणाम = http_header_a(तथ्य, hd, hdlen);
    अवरोध;
  हाल 'c':
  हाल 'C':
    परिणाम = http_header_c(तथ्य, hd, hdlen);
    अवरोध;
  हाल 'l':
  हाल 'L':
    परिणाम = http_header_l(तथ्य, hd, hdlen);
    अवरोध;
  हाल 'p':
  हाल 'P':
    परिणाम = http_header_p(तथ्य, hd, hdlen);
    अवरोध;
  हाल 'r':
  हाल 'R':
    परिणाम = http_header_r(तथ्य, hd, hdlen);
    अवरोध;
  हाल 's':
  हाल 'S':
    परिणाम = http_header_s(तथ्य, hd, hdlen);
    अवरोध;
  हाल 't':
  हाल 'T':
    परिणाम = http_header_t(तथ्य, hd, hdlen);
    अवरोध;
  हाल 'w':
  हाल 'W':
    परिणाम = http_header_w(तथ्य, hd, hdlen);
    अवरोध;
  }

  अगर(!परिणाम) {
    काष्ठा connectdata *conn = तथ्य->conn;
    अगर(conn->scheme->protocol & CURLPROTO_RTSP)
      परिणाम = Curl_rtsp_parseheader(तथ्य, hd);
  }
  वापस परिणाम;
}

/*
 * Called after the first HTTP response line (the status line) has been
 * received and parsed.
 */
अटल CURLcode http_statusline(काष्ठा Curl_easy *तथ्य,
                                काष्ठा connectdata *conn)
{
  काष्ठा SingleRequest *k = &तथ्य->req;

  चयन(k->httpversion) {
  हाल 10:
  हाल 11:
#अगर_घोषित USE_HTTP2
  हाल 20:
#पूर्ण_अगर
#अगर_घोषित USE_HTTP3
  हाल 30:
#पूर्ण_अगर
    /* no major version switch mid-connection */
    अगर(k->httpversion_sent &&
       (k->httpversion / 10 != k->httpversion_sent / 10)) {
      failf(तथ्य, "Version mismatch (from HTTP/%d to HTTP/%d)",
            k->httpversion_sent / 10, k->httpversion / 10);
      वापस CURLE_WEIRD_SERVER_REPLY;
    }
    अवरोध;
  शेष:
    failf(तथ्य, "Unsupported HTTP version (%d.%d) in response",
          k->httpversion / 10, k->httpversion % 10);
    वापस CURLE_UNSUPPORTED_PROTOCOL;
  }

  तथ्य->info.httpcode = k->httpcode;
  तथ्य->info.httpversion = k->httpversion;
  conn->httpversion_seen = k->httpversion;

  अगर(!तथ्य->state.http_neg.rcvd_min ||
     तथ्य->state.http_neg.rcvd_min > k->httpversion)
    /* store the lowest server version we encounter */
    तथ्य->state.http_neg.rcvd_min = k->httpversion;

  /*
   * This code executes as part of processing the header. As a
   * result, it is not totally clear how to interpret the
   * response code yet as that depends on what other headers may
   * be present. 401 and 407 may be errors, but may be OK
   * depending on how authentication is working. Other codes
   * are definitely errors, so give up here.
   */
  अगर(तथ्य->state.resume_from && तथ्य->state.httpreq == HTTPREQ_GET &&
     k->httpcode == 416) {
    /* "Requested Range Not Satisfiable", proceed and pretend this is no
       error */
    k->ignorebody = TRUE; /* Avoid appending error msg to good data. */
  }

  अगर(k->httpversion == 10) {
    /* Default action for HTTP/1.0 must be to close, unless
       we get one of those fancy headers that tell us the
       server keeps it open for us! */
    infof(तथ्य, "HTTP 1.0, assume close after body");
    connclose(conn);
  }

  k->http_bodyless = k->httpcode >= 100 && k->httpcode < 200;
  चयन(k->httpcode) {
  हाल 304:
    /* (quote from RFC2616, section 10.3.5): The 304 response
     * MUST NOT contain a message-body, and thus is always
     * terminated by the first empty line after the header
     * fields. */
    अगर(तथ्य->set.timecondition)
      तथ्य->info.timecond = TRUE;
    FALLTHROUGH();
  हाल 204:
    /* (quote from RFC2616, section 10.2.5): The server has
     * fulfilled the request but does not need to return an
     * entity-body ... The 204 response MUST NOT include a
     * message-body, and thus is always terminated by the first
     * empty line after the header fields. */
    k->आकार = 0;
    k->maxdownload = 0;
    k->http_bodyless = TRUE;
    अवरोध;
  शेष:
    अवरोध;
  }
  वापस CURLE_OK;
}

/* Content-Length must be ignored if any Transfer-Encoding is present in the
   response. Refer to RFC 7230 section 3.3.3 and RFC2616 section 4.4. This is
   figured out here after all headers have been received but before the final
   call to the user's header callback, so that a valid content length can be
   retrieved by the user in the final call. */
अटल CURLcode http_आकार(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा SingleRequest *k = &तथ्य->req;
  अगर(तथ्य->req.ignore_cl || k->chunk) {
    k->आकार = k->maxdownload = -1;
  }
  अन्यथा अगर(k->आकार != -1) {
    अगर(तथ्य->set.max_filesize &&
       !k->ignorebody &&
       (k->आकार > तथ्य->set.max_filesize)) {
      failf(तथ्य, "Maximum file size exceeded");
      वापस CURLE_FILESIZE_EXCEEDED;
    }
    अगर(k->ignorebody)
      infof(तथ्य, "setting size while ignoring");
    Curl_pgrs_Set_Download_आकार(तथ्य, k->आकार);
    k->maxdownload = k->आकार;
  }
  वापस CURLE_OK;
}

CURLcode Curl_verify_header(काष्ठा Curl_easy *तथ्य,
                            स्थिर अक्षर *hd, माप_प्रकार hdlen)
{
  काष्ठा SingleRequest *k = &तथ्य->req;
  माप_प्रकार i;
  bool has_colon = FALSE;
  क्रम(i = 0; i < hdlen; i++) {
    चयन(hd[i]) {
    हाल '\0':
      /* this is bad, bail out */
      failf(तथ्य, "NUL byte in header");
      वापस CURLE_WEIRD_SERVER_REPLY;
    हाल '\r':
      अगर(i < hdlen - 2) {
        /* CR may only precede the LF, nothing else */
        failf(तथ्य, "Carriage return found in header");
        वापस CURLE_WEIRD_SERVER_REPLY;
      }
      अवरोध;
    हाल ':':
      has_colon = TRUE;
      अवरोध;
    }
  }
  अगर(k->headerline < 2)
    /* the first "header" is the status-line and it has no colon */
    वापस CURLE_OK;
  अगर(((hd[0] == ' ') || (hd[0] == '\t')) && k->headerline > 2)
    /* line folding, cannot happen on line 2 */
    ;
  अन्यथा अगर(!has_colon) {
    /* this is bad, bail out */
    failf(तथ्य, "Header without colon");
    वापस CURLE_WEIRD_SERVER_REPLY;
  }
  वापस CURLE_OK;
}

CURLcode Curl_bump_headersize(काष्ठा Curl_easy *तथ्य,
                              माप_प्रकार delta,
                              bool connect_only)
{
  माप_प्रकार bad = 0;
  अचिन्हित पूर्णांक max = MAX_HTTP_RESP_HEADER_आकार;
  अगर(delta < MAX_HTTP_RESP_HEADER_आकार) {
    तथ्य->info.header_आकार += (अचिन्हित पूर्णांक)delta;
    तथ्य->req.allheadercount += (अचिन्हित पूर्णांक)delta;
    अगर(!connect_only)
      तथ्य->req.headerbytecount += (अचिन्हित पूर्णांक)delta;
    अगर(तथ्य->req.allheadercount > max)
      bad = तथ्य->req.allheadercount;
    अन्यथा अगर(तथ्य->info.header_आकार > (max * 20)) {
      bad = तथ्य->info.header_आकार;
      max *= 20;
    }
  }
  अन्यथा
    bad = तथ्य->req.allheadercount + delta;
  अगर(bad) {
    failf(तथ्य, "Too large response headers: %zu > %u", bad, max);
    वापस CURLE_RECV_ERROR;
  }
  वापस CURLE_OK;
}

/*
 * Handle a 101 Switching Protocols response. Performs the actual protocol
 * upgrade to HTTP/2 or WebSocket based on what was requested.
 */
अटल CURLcode http_on_101_upgrade(काष्ठा Curl_easy *तथ्य,
                                    स्थिर अक्षर *buf, माप_प्रकार blen,
                                    माप_प्रकार *pconsumed,
                                    bool *conn_changed)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा SingleRequest *k = &तथ्य->req;

#अगर !defined(USE_NGHTTP2) && defined(CURL_DISABLE_WEBSOCKETS)
  (व्योम)buf;
  (व्योम)blen;
  (व्योम)pconsumed;
#अन्यथा
  CURLcode परिणाम;
  पूर्णांक upgr101_requested = k->upgr101;
#पूर्ण_अगर

  अगर(k->httpversion_sent != 11) {
    /* invalid for other HTTP versions */
    failf(तथ्य, "server sent 101 response while not talking HTTP/1.1");
    वापस CURLE_WEIRD_SERVER_REPLY;
  }

  /* Whatever the success, upgrade was selected. */
  k->upgr101 = UPGR101_RECEIVED;
  conn->bits.upgrade_in_progress = FALSE;
  *conn_changed = TRUE;

  /* To be fully compliant, we would check the "Upgrade:" response header to
   * mention the protocol we requested. */
#अगर_घोषित USE_NGHTTP2
  अगर(upgr101_requested == UPGR101_H2) {
    /* Switch to HTTP/2, where we will get more responses. blen bytes in buf
     * are already h2 protocol bytes */
    infof(तथ्य, "Received 101, Switching to HTTP/2");
    परिणाम = Curl_http2_upgrade(तथ्य, conn, FIRSTSOCKET, buf, blen);
    अगर(!परिणाम)
      *pconsumed += blen;
    वापस परिणाम;
  }
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_WEBSOCKETS
  अगर(upgr101_requested == UPGR101_WS) {
    /* Switch to WebSocket, where we now stream ws frames. blen bytes in buf
     * are already ws protocol bytes */
    infof(तथ्य, "Received 101, Switching to WebSocket");
    परिणाम = Curl_ws_accept(तथ्य, buf, blen);
    अगर(!परिणाम)
      *pconsumed += blen; /* ws accept handled the data */
    वापस परिणाम;
  }
#पूर्ण_अगर
  /* We silently accept this as the final response. What are we switching to
   * if we did not ask for an Upgrade? Maybe the application provided an
   * `Upgrade: xxx` header? */
  k->header = FALSE;
  वापस CURLE_OK;
}

/*
 * Handle 1xx intermediate HTTP responses. Sets up state for more
 * headers and processes 100-continue and 101 upgrade responses.
 */
अटल CURLcode http_on_1xx_response(काष्ठा Curl_easy *तथ्य,
                                     स्थिर अक्षर *buf, माप_प्रकार blen,
                                     माप_प्रकार *pconsumed,
                                     bool *conn_changed)
{
  काष्ठा SingleRequest *k = &तथ्य->req;

  /* "A user agent MAY ignore unexpected 1xx status responses."
   * By default, we expect to get more responses after this one. */
  k->header = TRUE;
  k->headerline = 0; /* restart the header line counter */

  चयन(k->httpcode) {
  हाल 100:
    /* We have made an HTTP PUT or POST and this is 1.1-lingo that tells us
     * that the server is OK with this and ready to receive the data. */
    http_exp100_got100(तथ्य);
    अवरोध;
  हाल 101:
    वापस http_on_101_upgrade(तथ्य, buf, blen, pconsumed, conn_changed);
  शेष:
    /* The server may send us other 1xx responses, like informative 103. This
     * has no influence on request processing and we expect to receive a
     * final response eventually. */
    अवरोध;
  }
  वापस CURLE_OK;
}

#अगर defined(USE_NTLM) || defined(USE_SPNEGO)
/*
 * Check if NTLM or SPNEGO authentication negotiation failed due to
 * connection closure (typically on HTTP/1.0 servers).
 */
अटल व्योम http_check_auth_closure(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा connectdata *conn)
{
  /* At this point we have some idea about the fate of the connection. If we
     are closing the connection it may result auth failure. */
#अगर_घोषित USE_NTLM
  अगर(conn->bits.बंद &&
     (((तथ्य->req.httpcode == 401) &&
       (conn->http_ntlm_state == NTLMSTATE_TYPE2)) ||
      ((तथ्य->req.httpcode == 407) &&
       (conn->proxy_ntlm_state == NTLMSTATE_TYPE2)))) {
    infof(तथ्य, "Connection closure while negotiating auth (HTTP 1.0?)");
    तथ्य->state.authproblem = TRUE;
  }
#पूर्ण_अगर
#अगर_घोषित USE_SPNEGO
  अगर(conn->bits.बंद &&
    (((तथ्य->req.httpcode == 401) &&
      (conn->http_negotiate_state == CURL_NEGO_AUTH_RECV)) ||
     ((तथ्य->req.httpcode == 407) &&
      (conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)))) {
    infof(तथ्य, "Connection closure while negotiating auth (HTTP 1.0?)");
    तथ्य->state.authproblem = TRUE;
  }
  अगर((conn->http_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (तथ्य->req.httpcode != 401)) {
    conn->http_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
  अगर((conn->proxy_negotiate_state == CURL_NEGO_AUTH_DONE) &&
     (तथ्य->req.httpcode != 407)) {
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
#पूर्ण_अगर
}
#अन्यथा
#घोषणा http_check_auth_closure(x, y) /* empty */
#पूर्ण_अगर

/*
 * Handle an error response (>= 300) received while still sending the
 * request body. Deals with 417 Expectation Failed retries, keep-sending
 * on error, and aborting the send.
 */
अटल CURLcode http_handle_send_error(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा SingleRequest *k = &तथ्य->req;
  CURLcode परिणाम = CURLE_OK;

  अगर(!तथ्य->req.authneg && !conn->bits.बंद &&
     !Curl_creader_will_rewind(तथ्य)) {
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

    चयन(तथ्य->state.httpreq) {
    हाल HTTPREQ_PUT:
    हाल HTTPREQ_POST:
    हाल HTTPREQ_POST_FORM:
    हाल HTTPREQ_POST_MIME:
      /* We got an error response. If this happened before the
       * whole request body has been sent we stop sending and
       * mark the connection for closure after we have read the
       * entire response. */
      अगर(!Curl_req_done_sending(तथ्य)) {
        अगर((k->httpcode == 417) && http_exp100_is_selected(तथ्य)) {
          /* 417 Expectation Failed - try again without the
             Expect header */
          अगर(!k->writebytecount && http_exp100_is_waiting(तथ्य)) {
            infof(तथ्य, "Got HTTP failure 417 while waiting for a 100");
          }
          अन्यथा {
            infof(तथ्य, "Got HTTP failure 417 while sending data");
            streamclose(conn);
            परिणाम = http_perhapsrewind(तथ्य, conn);
            अगर(परिणाम)
              वापस परिणाम;
          }
          तथ्य->state.disableexpect = TRUE;
          Curl_req_abort_sending(तथ्य);
          DEBUGASSERT(!तथ्य->req.newurl);
          तथ्य->req.newurl = Curl_bufref_dup(&तथ्य->state.url);
          अगर(!तथ्य->req.newurl)
            वापस CURLE_OUT_OF_MEMORY;
        }
        अन्यथा अगर(तथ्य->set.http_keep_sending_on_error) {
          infof(तथ्य, "HTTP error before end of send, keep sending");
          http_exp100_send_anyway(तथ्य);
        }
        अन्यथा {
          infof(तथ्य, "HTTP error before end of send, stop sending");
          streamclose(conn);
          परिणाम = Curl_req_abort_sending(तथ्य);
          अगर(परिणाम)
            वापस परिणाम;
        }
      }
      अवरोध;

    शेष: /* default label present to avoid compiler warnings */
      अवरोध;
    }
  }

  अगर(Curl_creader_will_rewind(तथ्य) && !Curl_req_done_sending(तथ्य)) {
    /* We rewind before next send, continue sending now */
    infof(तथ्य, "Keep sending data to get tossed away");
    CURL_REQ_SET_SEND(तथ्य);
  }
  वापस परिणाम;
}

अटल CURLcode http_on_response(काष्ठा Curl_easy *तथ्य,
                                 स्थिर अक्षर *last_hd, माप_प्रकार last_hd_len,
                                 स्थिर अक्षर *buf, माप_प्रकार blen,
                                 माप_प्रकार *pconsumed)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  CURLcode परिणाम = CURLE_OK;
  काष्ठा SingleRequest *k = &तथ्य->req;
  bool conn_changed = FALSE;

  (व्योम)buf; /* not used without HTTP2 enabled */
  *pconsumed = 0;

  अगर(k->upgr101 == UPGR101_RECEIVED) {
    /* supposedly upgraded to http2 now */
    अगर(तथ्य->req.httpversion != 20)
      infof(तथ्य, "Lying server, not serving HTTP/2");
  }

  अगर(k->httpcode < 200 && last_hd) {
    /* Intermediate responses might trigger processing of more responses,
     * write the last header to the client before proceeding. */
    परिणाम = http_write_header(तथ्य, last_hd, last_hd_len);
    last_hd = शून्य; /* handled it */
    अगर(परिणाम)
      जाओ out;
  }

  अगर(k->httpcode < 100) {
    failf(तथ्य, "Unsupported response code in HTTP response");
    परिणाम = CURLE_UNSUPPORTED_PROTOCOL;
    जाओ out;
  }
  अन्यथा अगर(k->httpcode < 200) {
    परिणाम = http_on_1xx_response(तथ्य, buf, blen, pconsumed, &conn_changed);
    जाओ out;
  }

  /* k->httpcode >= 200, final response */
  k->header = FALSE;
  अगर(conn->bits.upgrade_in_progress) {
    /* Asked for protocol upgrade, but it was not selected */
    conn->bits.upgrade_in_progress = FALSE;
    conn_changed = TRUE;
  }

  अगर((k->आकार == -1) && !k->chunk && !conn->bits.बंद &&
     (k->httpversion == 11) &&
     !(conn->scheme->protocol & CURLPROTO_RTSP) &&
     तथ्य->state.httpreq != HTTPREQ_शीर्ष) {
    /* On HTTP 1.1, when connection is not to get closed, but no
       Content-Length nor Transfer-Encoding chunked have been received,
       according to RFC2616 section 4.4 point 5, we assume that the server
       will close the connection to signal the end of the document. */
    infof(तथ्य, "no chunk, no close, no size. Assume close to signal end");
    streamclose(conn);
  }

  http_check_auth_closure(तथ्य, conn);

#अगर_अघोषित CURL_DISABLE_WEBSOCKETS
  /* All >=200 HTTP status codes are errors when wanting ws */
  अगर(तथ्य->req.upgr101 == UPGR101_WS) {
    failf(तथ्य, "Refused WebSocket upgrade: %d", k->httpcode);
    परिणाम = CURLE_HTTP_RETURNED_ERROR;
    जाओ out;
  }
#पूर्ण_अगर

  /* Check if this response means the transfer errored. */
  अगर(http_should_fail(तथ्य, तथ्य->req.httpcode)) {
    failf(तथ्य, "The requested URL returned error: %d",
          k->httpcode);
    परिणाम = CURLE_HTTP_RETURNED_ERROR;
    जाओ out;
  }

  /* Curl_http_auth_act() checks what authentication methods that are
   * available and decides which one (if any) to use. It will set 'newurl' if
   * an auth method was picked. */
  परिणाम = Curl_http_auth_act(तथ्य);
  अगर(परिणाम)
    जाओ out;

  अगर(k->httpcode >= 300) {
    परिणाम = http_handle_send_error(तथ्य);
    अगर(परिणाम)
      जाओ out;
  }

  /* final response without error, prepare to receive the body */
  परिणाम = http_firstwrite(तथ्य);
  अगर(परिणाम)
    जाओ out;

  /* This is the last response that we get for the current request. Check on
   * the body size and determine if the response is complete. */
  परिणाम = http_आकार(तथ्य);
  अगर(परिणाम)
    जाओ out;

#अगर_अघोषित CURL_DISABLE_RTSP
  /* An RTSP response to any method may carry a body, which is announced
     with Content-Length (RFC 2326 section 12.14). Read it instead of
     treating it as excess data, unless the application asked for no body
     with CURLOPT_NOBODY. */
  अगर((conn->scheme->protocol & CURLPROTO_RTSP) && तथ्य->req.no_body &&
     !तथ्य->set.opt_no_body && (k->आकार > 0))
    तथ्य->req.no_body = FALSE;
#पूर्ण_अगर

  /* If we requested a "no body", this is a good time to get
   * out and return home.
   */
  अगर(तथ्य->req.no_body)
    k->download_done = TRUE;

  /* If max download size is *zero* (nothing) we already have nothing and can
     safely return ok now! For HTTP/2, we would like to call
     http2_handle_stream_close to properly close a stream. In order to do
     this, we keep reading until we close the stream. */
  अगर((k->maxdownload == 0) && (k->httpversion_sent < 20))
    k->download_done = TRUE;

out:
  अगर(last_hd)
    /* if not written yet, write it now */
    परिणाम = Curl_1st_fatal(परिणाम,
                            http_write_header(तथ्य, last_hd, last_hd_len));
  अगर(conn_changed)
    /* poke the multi handle to allow pending pipewait to retry */
    Curl_multi_connchanged(तथ्य->multi);
  वापस परिणाम;
}

अटल CURLcode http_rw_hd(काष्ठा Curl_easy *तथ्य,
                           स्थिर अक्षर *hd, माप_प्रकार hdlen,
                           स्थिर अक्षर *buf_remain, माप_प्रकार blen,
                           माप_प्रकार *pconsumed)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा SingleRequest *k = &तथ्य->req;
  पूर्णांक writetype;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  *pconsumed = 0;
  अगर((0x0a == *hd) || (0x0d == *hd)) {
    /* Empty header line means end of headers! */
    काष्ठा dynbuf last_header;
    माप_प्रकार consumed;

    curlx_dyn_init(&last_header, hdlen + 1);
    परिणाम = curlx_dyn_addn(&last_header, hd, hdlen);
    अगर(परिणाम)
      वापस परिणाम;

    /* analyze the response to find out what to do. */
    /* Caveat: we clear anything in the header brigade, because a
     * response might switch HTTP version which may call use recursively.
     * Not nice, but that is currently the way of things. */
    curlx_dyn_reset(&तथ्य->state.headerb);
    परिणाम = http_on_response(तथ्य, curlx_dyn_ptr(&last_header),
                              curlx_dyn_len(&last_header),
                              buf_remain, blen, &consumed);
    *pconsumed += consumed;
    curlx_dyn_free(&last_header);
    वापस परिणाम;
  }

  /*
   * Checks for special headers coming up.
   */

  writetype = CLIENTWRITE_HEADER;
  अगर(!k->headerline++) {
    /* This is the first header, it MUST be the error code line
       or else we consider this to be the body right away! */
    bool fine_statusline = FALSE;

    k->httpversion = 0; /* Do not know yet */
    अगर(तथ्य->conn->scheme->protocol & PROTO_FAMILY_HTTP) {
      /*
       * https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.2
       *
       * The response code is always a three-digit number in HTTP as the spec
       * says. We allow any three-digit number here, but we cannot make
       * guarantees on future behaviors since it is not within the protocol.
       */
      स्थिर अक्षर *p = hd;

      curlx_str_passblanks(&p);
      अगर(!म_भेदन(p, "HTTP/", 5)) {
        p += 5;
        चयन(*p) {
        हाल '1':
          p++;
          अगर((p[0] == '.') && (p[1] == '0' || p[1] == '1')) {
            अगर(ISBLANK(p[2])) {
              k->httpversion = (अचिन्हित अक्षर)(10 + (p[1] - '0'));
              p += 3;
              अगर(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
                k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
                  (p[2] - '0');
                /* RFC 9112 requires a single space following the status code,
                   but the browsers do not so let's not insist */
                fine_statusline = TRUE;
              }
            }
          }
          अगर(!fine_statusline) {
            failf(तथ्य, "Unsupported HTTP/1 subversion in response");
            वापस CURLE_UNSUPPORTED_PROTOCOL;
          }
          अवरोध;
        हाल '2':
        हाल '3':
          अगर(!ISBLANK(p[1]))
            अवरोध;
          k->httpversion = (अचिन्हित अक्षर)((*p - '0') * 10);
          p += 2;
          अगर(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
            k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
              (p[2] - '0');
            p += 3;
            अगर(!ISBLANK(*p))
              अवरोध;
            fine_statusline = TRUE;
          }
          अवरोध;
        शेष: /* unsupported */
          failf(तथ्य, "Unsupported HTTP version in response");
          वापस CURLE_UNSUPPORTED_PROTOCOL;
        }
      }

      अगर(!fine_statusline) {
        /* If user has set option HTTP200ALIASES,
           compare header line against list of aliases */
        statusline check = checkhttpprefix(तथ्य, hd, hdlen);
        अगर(check == STATUS_DONE) {
          fine_statusline = TRUE;
          k->httpcode = 200;
          k->httpversion = 10;
        }
      }
    }
    अन्यथा अगर(तथ्य->conn->scheme->protocol & CURLPROTO_RTSP) {
      स्थिर अक्षर *p = hd;
      काष्ठा Curl_str ver;
      curl_off_t status;
      /* we set the max string a little excessive to forgive some leading
         spaces */
      अगर(!curlx_str_until(&p, &ver, 32, ' ') &&
         !curlx_str_single(&p, ' ') &&
         !curlx_str_संख्या(&p, &status, 999)) {
        curlx_str_trimblanks(&ver);
        अगर(curlx_str_cmp(&ver, "RTSP/1.0")) {
          k->httpcode = (पूर्णांक)status;
          fine_statusline = TRUE;
          k->httpversion = 11; /* RTSP acts like HTTP 1.1 */
        }
      }
      अगर(!fine_statusline)
        वापस CURLE_WEIRD_SERVER_REPLY;
    }

    अगर(fine_statusline) {
      परिणाम = http_statusline(तथ्य, तथ्य->conn);
      अगर(परिणाम)
        वापस परिणाम;
      writetype |= CLIENTWRITE_STATUS;
    }
    अन्यथा {
      k->header = FALSE;   /* this is not a header line */
      वापस CURLE_WEIRD_SERVER_REPLY;
    }
  }

  परिणाम = Curl_verify_header(तथ्य, hd, hdlen);
  अगर(परिणाम)
    वापस परिणाम;

  परिणाम = http_header(तथ्य, hd, hdlen);
  अगर(परिणाम)
    वापस परिणाम;

  /*
   * Taken in one (more) header. Write it to the client.
   */
  Curl_debug(तथ्य, CURLINFO_HEADER_IN, hd, hdlen);

  अगर(k->httpcode / 100 == 1)
    writetype |= CLIENTWRITE_1XX;
  परिणाम = Curl_client_write(तथ्य, writetype, hd, hdlen);
  अगर(परिणाम)
    वापस परिणाम;

  परिणाम = Curl_bump_headersize(तथ्य, hdlen, FALSE);
  अगर(परिणाम)
    वापस परिणाम;

  वापस CURLE_OK;
}

/* remove trailing CRLF then all trailing whitespace */
व्योम Curl_http_to_fold(काष्ठा dynbuf *bf)
{
  माप_प्रकार len = curlx_dyn_len(bf);
  स्थिर अक्षर *hd = curlx_dyn_ptr(bf);
  अगर(len && (hd[len - 1] == '\n'))
    len--;
  अगर(len && (hd[len - 1] == '\r'))
    len--;
  जबतक(len && ISBLANK(hd[len - 1])) /* strip off trailing whitespace */
    len--;
  curlx_dyn_setlen(bf, len);
}

अटल व्योम unfold_header(काष्ठा Curl_easy *तथ्य)
{
  Curl_http_to_fold(&तथ्य->state.headerb);
  तथ्य->state.leading_unfold = TRUE;
}

/*
 * Read any HTTP header lines from the server and pass them to the client app.
 */
अटल CURLcode http_parse_headers(काष्ठा Curl_easy *तथ्य,
                                   स्थिर अक्षर *buf, माप_प्रकार blen,
                                   माप_प्रकार *pconsumed)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  CURLcode परिणाम = CURLE_OK;
  काष्ठा SingleRequest *k = &तथ्य->req;
  स्थिर अक्षर *end_ptr;
  bool leftover_body = FALSE;

  /* we have bytes for the next header, make sure it is not a folded header
     before passing it on */
  अगर(तथ्य->state.maybe_folded && blen) {
    अगर(ISBLANK(buf[0])) {
      /* folded, remove the trailing newlines and append the next header */
      unfold_header(तथ्य);
    }
    अन्यथा {
      /* the header data we hold is a complete header, pass it on */
      माप_प्रकार ignore_this;
      परिणाम = http_rw_hd(तथ्य, curlx_dyn_ptr(&तथ्य->state.headerb),
                          curlx_dyn_len(&तथ्य->state.headerb),
                          शून्य, 0, &ignore_this);
      curlx_dyn_reset(&तथ्य->state.headerb);
      अगर(परिणाम)
        वापस परिणाम;
    }
    तथ्य->state.maybe_folded = FALSE;
  }

  /* header line within buffer loop */
  *pconsumed = 0;
  जबतक(blen && k->header) {
    माप_प्रकार consumed;
    माप_प्रकार hlen;
    स्थिर अक्षर *hd;
    माप_प्रकार unfold_len = 0;

    अगर(तथ्य->state.leading_unfold) {
      /* immediately after an unfold, keep only a single whitespace */
      जबतक(blen && ISBLANK(buf[0])) {
        buf++;
        blen--;
        unfold_len++;
      }
      अगर(blen) {
        /* insert a single space */
        परिणाम = curlx_dyn_addn(&तथ्य->state.headerb, " ", 1);
        अगर(परिणाम)
          वापस परिणाम;
        तथ्य->state.leading_unfold = FALSE; /* done now */
      }
    }

    end_ptr = स_प्रथम(buf, '\n', blen);
    अगर(!end_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      परिणाम = curlx_dyn_addn(&तथ्य->state.headerb, buf, blen);
      अगर(परिणाम)
        वापस परिणाम;
      *pconsumed += blen + unfold_len;

      अगर(!k->headerline) {
        /* check if this looks like a protocol header */
        statusline st =
          checkprotoprefix(तथ्य, conn,
                           curlx_dyn_ptr(&तथ्य->state.headerb),
                           curlx_dyn_len(&तथ्य->state.headerb));

        अगर(st == STATUS_BAD) {
          /* this is not the beginning of a protocol first header line.
           * Cannot be 0.9 if version was detected or connection was reused. */
          k->header = FALSE;
          streamclose(conn);
          अगर((k->httpversion >= 10) || conn->bits.reuse) {
            failf(तथ्य, "Invalid status line");
            वापस CURLE_WEIRD_SERVER_REPLY;
          }
          अगर(!तथ्य->state.http_neg.accept_09) {
            failf(तथ्य, "Received HTTP/0.9 when not allowed");
            वापस CURLE_UNSUPPORTED_PROTOCOL;
          }
          leftover_body = TRUE;
          जाओ out;
        }
      }
      जाओ out; /* read more and try again */
    }

    /* the size of the remaining header line */
    consumed = (end_ptr - buf) + 1;

    परिणाम = curlx_dyn_addn(&तथ्य->state.headerb, buf, consumed);
    अगर(परिणाम)
      वापस परिणाम;
    blen -= consumed;
    buf += consumed;
    *pconsumed += consumed + unfold_len;

    /****
     * We now have a FULL header line in 'headerb'.
     *****/

    hlen = curlx_dyn_len(&तथ्य->state.headerb);
    hd = curlx_dyn_ptr(&तथ्य->state.headerb);

    अगर(!k->headerline) {
      /* the first read "header", the status line */
      statusline st = checkprotoprefix(तथ्य, conn, hd, hlen);
      अगर(st == STATUS_BAD) {
        streamclose(conn);
        /* this is not the beginning of a protocol first header line.
         * Cannot be 0.9 if version was detected or connection was reused. */
        अगर((k->httpversion >= 10) || conn->bits.reuse) {
          failf(तथ्य, "Invalid status line");
          वापस CURLE_WEIRD_SERVER_REPLY;
        }
        अगर(!तथ्य->state.http_neg.accept_09) {
          failf(तथ्य, "Received HTTP/0.9 when not allowed");
          वापस CURLE_UNSUPPORTED_PROTOCOL;
        }
        k->header = FALSE;
        leftover_body = TRUE;
        जाओ out;
      }
    }
    अन्यथा {
      अगर(hlen && !ISNEWLINE(hd[0])) {
        /* this is NOT the header separator */

        /* if we have bytes for the next header, check for folding */
        अगर(blen && ISBLANK(buf[0])) {
          /* remove the trailing CRLF and append the next header */
          unfold_header(तथ्य);
          जारी;
        }
        अन्यथा अगर(!blen) {
          /* this might be a folded header so deal with it in next invoke */
          तथ्य->state.maybe_folded = TRUE;
          अवरोध;
        }
      }
    }

    परिणाम = http_rw_hd(तथ्य, hd, hlen, buf, blen, &consumed);
    /* We are done with this line. We reset because response
     * processing might switch to HTTP/2 and that might call us
     * directly again. */
    curlx_dyn_reset(&तथ्य->state.headerb);
    अगर(consumed) {
      blen -= consumed;
      buf += consumed;
      *pconsumed += consumed;
    }
    अगर(परिणाम)
      वापस परिणाम;
  }

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */
out:
  अगर(!k->header && !leftover_body) {
    curlx_dyn_free(&तथ्य->state.headerb);
  }
  वापस CURLE_OK;
}

CURLcode Curl_http_write_resp_hd(काष्ठा Curl_easy *तथ्य,
                                 स्थिर अक्षर *hd, माप_प्रकार hdlen,
                                 bool is_eos)
{
  CURLcode परिणाम;
  माप_प्रकार consumed;
  अक्षर tmp = 0;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  परिणाम = http_rw_hd(तथ्य, hd, hdlen, &tmp, 0, &consumed);
  अगर(!परिणाम && is_eos) {
    परिणाम = Curl_client_write(तथ्य, (CLIENTWRITE_BODY | CLIENTWRITE_EOS),
                               &tmp, 0);
  }
  वापस परिणाम;
}

/*
 * HTTP protocol `write_resp` implementation. Parse headers
 * when not done yet and otherwise return without consuming data.
 */
CURLcode Curl_http_write_resp_hds(काष्ठा Curl_easy *तथ्य,
                                  स्थिर अक्षर *buf, माप_प्रकार blen,
                                  माप_प्रकार *pconsumed)
{
  अगर(!तथ्य->req.header) {
    *pconsumed = 0;
    वापस CURLE_OK;
  }
  अन्यथा {
    CURLcode परिणाम;

    परिणाम = http_parse_headers(तथ्य, buf, blen, pconsumed);
    अगर(!परिणाम && !तथ्य->req.header) {
      अगर(!तथ्य->req.no_body && curlx_dyn_len(&तथ्य->state.headerb)) {
        /* leftover from parsing something that turned out not
         * to be a header, only happens if we allow for
         * HTTP/0.9 like responses */
        परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY,
                                   curlx_dyn_ptr(&तथ्य->state.headerb),
                                   curlx_dyn_len(&तथ्य->state.headerb));
      }
      curlx_dyn_free(&तथ्य->state.headerb);
    }
    वापस परिणाम;
  }
}

CURLcode Curl_http_write_resp(काष्ठा Curl_easy *तथ्य,
                              स्थिर अक्षर *buf, माप_प्रकार blen,
                              bool is_eos)
{
  CURLcode परिणाम;
  माप_प्रकार consumed;
  पूर्णांक flags;

  परिणाम = Curl_http_write_resp_hds(तथ्य, buf, blen, &consumed);
  अगर(परिणाम || तथ्य->req.done)
    जाओ out;

  DEBUGASSERT(consumed <= blen);
  blen -= consumed;
  buf += consumed;
  /* either all was consumed in header parsing, or we have data left
   * and are done with headers, e.g. it is BODY data */
  DEBUGASSERT(!blen || !तथ्य->req.header);
  अगर(!तथ्य->req.header && (blen || is_eos)) {
    /* BODY data after header been parsed, write and consume */
    flags = CLIENTWRITE_BODY;
    अगर(is_eos)
      flags |= CLIENTWRITE_EOS;
    परिणाम = Curl_client_write(तथ्य, flags, buf, blen);
  }
out:
  वापस परिणाम;
}

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_status(int32_t *pstatus, स्थिर अक्षर *s, माप_प्रकार len)
{
  CURLcode परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
  int32_t status = 0;
  पूर्णांक i;

  अगर(len != 3)
    जाओ out;

  क्रम(i = 0; i < 3; ++i) {
    अक्षर c = s[i];

    अगर(c < '0' || c > '9')
      जाओ out;

    status *= 10;
    status += c - '0';
  }
  परिणाम = CURLE_OK;
out:
  *pstatus = परिणाम ? -1 : status;
  वापस परिणाम;
}

CURLcode Curl_http_req_make(काष्ठा httpreq **preq,
                            स्थिर अक्षर *method, माप_प्रकार m_len,
                            स्थिर अक्षर *scheme, माप_प्रकार s_len,
                            स्थिर अक्षर *authority, माप_प्रकार a_len,
                            स्थिर अक्षर *path, माप_प्रकार p_len)
{
  काष्ठा httpreq *req;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, माप(*req) + m_len);
  अगर(!req)
    जाओ out;
#अगर defined(__GNUC__) && __GNUC__ >= 13
#आशय GCC diagnostic धकेलो
/* error: 'memcpy' offset [137, 142] from the object at 'req' is out of
   the bounds of referenced subobject 'method' with type 'char[1]' at
   offset 136 */
#आशय GCC diagnostic ignored "-Warray-bounds"
#पूर्ण_अगर
  स_नकल(req->method, method, m_len);
#अगर defined(__GNUC__) && __GNUC__ >= 13
#आशय GCC diagnostic निकालो
#पूर्ण_अगर
  अगर(scheme) {
    req->scheme = curlx_memdup0(scheme, s_len);
    अगर(!req->scheme)
      जाओ out;
  }
  अगर(authority) {
    req->authority = curlx_memdup0(authority, a_len);
    अगर(!req->authority)
      जाओ out;
  }
  अगर(path) {
    req->path = curlx_memdup0(path, p_len);
    अगर(!req->path)
      जाओ out;
  }
  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  परिणाम = CURLE_OK;

out:
  अगर(परिणाम && req)
    Curl_http_req_free(req);
  *preq = परिणाम ? शून्य : req;
  वापस परिणाम;
}

अटल CURLcode req_assign_url_authority(काष्ठा httpreq *req, CURLU *url)
{
  अक्षर *host, *port;
  काष्ठा dynbuf buf;
  CURLUcode uc;
  CURLcode परिणाम = CURLE_URL_MALFORMAT;

  host = port = शून्य;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_HOST, &host, 0);
  अगर(uc && uc != CURLUE_NO_HOST)
    जाओ out;
  अगर(!host) {
    req->authority = शून्य;
    परिणाम = CURLE_OK;
    जाओ out;
  }

  uc = curl_url_get(url, CURLUPART_PORT, &port, CURLU_NO_DEFAULT_PORT);
  अगर(uc && uc != CURLUE_NO_PORT)
    जाओ out;

  परिणाम = curlx_dyn_add(&buf, host);
  अगर(परिणाम)
    जाओ out;
  अगर(port) {
    परिणाम = curlx_dyn_addf(&buf, ":%s", port);
    अगर(परिणाम)
      जाओ out;
  }
  req->authority = curlx_dyn_ptr(&buf);
out:
  curlx_free(host);
  curlx_free(port);
  अगर(परिणाम)
    curlx_dyn_free(&buf);
  वापस परिणाम;
}

अटल CURLcode req_assign_url_path(काष्ठा httpreq *req, CURLU *url)
{
  अक्षर *path, *query;
  काष्ठा dynbuf buf;
  CURLUcode uc;
  CURLcode परिणाम = CURLE_URL_MALFORMAT;

  path = query = शून्य;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_PATH, &path, 0);
  अगर(uc)
    जाओ out;
  uc = curl_url_get(url, CURLUPART_QUERY, &query, 0);
  अगर(uc && uc != CURLUE_NO_QUERY)
    जाओ out;

  अगर(!query) {
    req->path = path;
    path = शून्य;
  }
  अन्यथा {
    परिणाम = curlx_dyn_add(&buf, path);
    अगर(!परिणाम)
      परिणाम = curlx_dyn_addf(&buf, "?%s", query);
    अगर(परिणाम)
      जाओ out;
    req->path = curlx_dyn_ptr(&buf);
  }
  परिणाम = CURLE_OK;

out:
  curlx_free(path);
  curlx_free(query);
  अगर(परिणाम)
    curlx_dyn_free(&buf);
  वापस परिणाम;
}

CURLcode Curl_http_req_make2(काष्ठा httpreq **preq,
                             स्थिर अक्षर *method, माप_प्रकार m_len,
                             CURLU *url, स्थिर अक्षर *scheme_default)
{
  काष्ठा httpreq *req;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;
  CURLUcode uc;

  DEBUGASSERT(method && m_len);

  req = curlx_calloc(1, माप(*req) + m_len);
  अगर(!req)
    जाओ out;
  स_नकल(req->method, method, m_len);

  uc = curl_url_get(url, CURLUPART_SCHEME, &req->scheme, 0);
  अगर(uc && uc != CURLUE_NO_SCHEME)
    जाओ out;
  अगर(!req->scheme && scheme_default) {
    req->scheme = curlx_strdup(scheme_default);
    अगर(!req->scheme)
      जाओ out;
  }

  परिणाम = req_assign_url_authority(req, url);
  अगर(परिणाम)
    जाओ out;
  परिणाम = req_assign_url_path(req, url);
  अगर(परिणाम)
    जाओ out;

  Curl_dynhds_init(&req->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  परिणाम = CURLE_OK;

out:
  अगर(परिणाम && req)
    Curl_http_req_free(req);
  *preq = परिणाम ? शून्य : req;
  वापस परिणाम;
}

व्योम Curl_http_req_free(काष्ठा httpreq *req)
{
  अगर(req) {
    curlx_free(req->scheme);
    curlx_free(req->authority);
    curlx_free(req->path);
    Curl_dynhds_free(&req->headers);
    Curl_dynhds_free(&req->trailers);
    curlx_free(req);
  }
}

काष्ठा name_const {
  स्थिर अक्षर *name;
  माप_प्रकार namelen;
};

अटल स्थिर काष्ठा name_const H2_NON_FIELD[] = {
  { STRCONST("Host") },
  { STRCONST("Upgrade") },
  { STRCONST("Connection") },
  { STRCONST("Keep-Alive") },
  { STRCONST("Proxy-Connection") },
  { STRCONST("Transfer-Encoding") },
};

अटल bool h2_permissible_field(काष्ठा dynhds_entry *e)
{
  माप_प्रकार i;
  क्रम(i = 0; i < CURL_ARRAYSIZE(H2_NON_FIELD); ++i) {
    अगर(e->namelen == H2_NON_FIELD[i].namelen &&
       curl_strnequal(H2_NON_FIELD[i].name, e->name, e->namelen))
      वापस FALSE;
  }
  वापस TRUE;
}

अटल bool http_TE_has_token(स्थिर अक्षर *fvalue, स्थिर अक्षर *token)
{
  जबतक(*fvalue) {
    काष्ठा Curl_str name;

    /* skip to first token */
    जबतक(ISBLANK(*fvalue) || *fvalue == ',')
      fvalue++;
    अगर(curlx_str_cspn(&fvalue, &name, " \t\r;,"))
      वापस FALSE;
    अगर(curlx_str_casecompare(&name, token))
      वापस TRUE;

    /* skip any remainder after token, e.g. parameters with quoted strings */
    जबतक(*fvalue && *fvalue != ',') {
      अगर(*fvalue == '"') {
        काष्ठा Curl_str qw;
        /* if we do not cleanly find a quoted word here, the header value
         * does not follow HTTP syntax and we reject */
        अगर(curlx_str_quotedword(&fvalue, &qw, CURL_MAX_HTTP_HEADER))
          वापस FALSE;
      }
      अन्यथा
        fvalue++;
    }
  }
  वापस FALSE;
}

CURLcode Curl_http_req_to_h2(काष्ठा dynhds *h2_headers,
                             काष्ठा httpreq *req, काष्ठा Curl_easy *तथ्य)
{
  स्थिर अक्षर *scheme = शून्य, *authority = शून्य;
  काष्ठा dynhds_entry *e;
  माप_प्रकार i;
  CURLcode परिणाम;

  DEBUGASSERT(req);
  DEBUGASSERT(h2_headers);

  अगर(req->scheme) {
    scheme = req->scheme;
  }
  अन्यथा अगर(म_भेद("CONNECT", req->method)) {
    scheme = Curl_checkheaders(तथ्य, STRCONST(HTTP_PSEUDO_SCHEME));
    अगर(scheme) {
      scheme += माप(HTTP_PSEUDO_SCHEME);
      curlx_str_passblanks(&scheme);
      infof(तथ्य, "set pseudo header %s to %s", HTTP_PSEUDO_SCHEME, scheme);
    }
    अन्यथा {
      scheme = तथ्य->state.origin->scheme->name;
    }
  }

  अगर(req->authority) {
    authority = req->authority;
  }
  अन्यथा {
    e = Curl_dynhds_get(&req->headers, STRCONST("Host"));
    अगर(e)
      authority = e->मूल्य;
  }

  Curl_dynhds_reset(h2_headers);
  Curl_dynhds_set_opts(h2_headers, DYNHDS_OPT_LOWERCASE);
  परिणाम = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_METHOD),
                           req->method, म_माप(req->method));
  अगर(!परिणाम && scheme) {
    परिणाम = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_SCHEME),
                             scheme, म_माप(scheme));
  }
  अगर(!परिणाम && authority) {
    परिणाम = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_AUTHORITY),
                             authority, म_माप(authority));
  }
  अगर(!परिणाम && req->path) {
    परिणाम = Curl_dynhds_add(h2_headers, STRCONST(HTTP_PSEUDO_PATH),
                             req->path, म_माप(req->path));
  }
  क्रम(i = 0; !परिणाम && i < Curl_dynhds_गिनती(&req->headers); ++i) {
    e = Curl_dynhds_getn(&req->headers, i);
    /* "TE" is special in that it is only permissible when it
     * has only value "trailers". RFC 9113 ch. 8.2.2 */
    अगर(e->namelen == 2 && curl_strequal("TE", e->name)) {
      अगर(http_TE_has_token(e->मूल्य, "trailers"))
        परिणाम = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                                 "trailers", CURL_CSTRLEN("trailers"));
    }
    अन्यथा अगर(h2_permissible_field(e)) {
      परिणाम = Curl_dynhds_add(h2_headers, e->name, e->namelen,
                               e->मूल्य, e->valuelen);
    }
  }

  वापस परिणाम;
}

CURLcode Curl_http_resp_make(काष्ठा http_resp **presp,
                             पूर्णांक status,
                             स्थिर अक्षर *description)
{
  काष्ठा http_resp *resp;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;

  resp = curlx_calloc(1, माप(*resp));
  अगर(!resp)
    जाओ out;

  resp->status = status;
  अगर(description) {
    resp->description = curlx_strdup(description);
    अगर(!resp->description)
      जाओ out;
  }
  Curl_dynhds_init(&resp->headers, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&resp->trailers, 0, DYN_HTTP_REQUEST);
  परिणाम = CURLE_OK;

out:
  अगर(परिणाम && resp)
    Curl_http_resp_free(resp);
  *presp = परिणाम ? शून्य : resp;
  वापस परिणाम;
}

व्योम Curl_http_resp_free(काष्ठा http_resp *resp)
{
  अगर(resp) {
    curlx_free(resp->description);
    Curl_dynhds_free(&resp->headers);
    Curl_dynhds_free(&resp->trailers);
    अगर(resp->पूर्व)
      Curl_http_resp_free(resp->पूर्व);
    curlx_free(resp);
  }
}

/*
 * HTTP handler interface.
 */
स्थिर काष्ठा Curl_protocol Curl_protocol_http = {
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

#पूर्ण_अगर /* CURL_DISABLE_HTTP */
