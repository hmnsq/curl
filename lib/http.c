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
#ضمن "curl_setup.h"
#ضمن "urldata.h"

#اگر_تعریف_نشده CURL_DISABLE_HTTP

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NET_IF_H
#ضمن <net/if.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#پایان_اگر

#ضمن "transfer.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "formdata.h"
#ضمن "mime.h"
#ضمن "progress.h"
#ضمن "curlx/base64.h"
#ضمن "cookie.h"
#ضمن "vauth/vauth.h"
#ضمن "vquic/vquic.h"
#ضمن "http_digest.h"
#ضمن "http_ntlm.h"
#ضمن "http_negotiate.h"
#ضمن "http_aws_sigv4.h"
#ضمن "http_httpsig.h"
#ضمن "url.h"
#ضمن "urlapi-int.h"
#ضمن "curl_share.h"
#ضمن "dynhds.h"
#ضمن "http.h"
#ضمن "headers.h"
#ضمن "select.h"
#ضمن "parsedate.h" /* for the week day and month names */
#ضمن "multiif.h"
#ضمن "strcase.h"
#ضمن "content_encoding.h"
#ضمن "http_proxy.h"
#ضمن "http2.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "curlx/strdup.h"
#ضمن "altsvc.h"
#ضمن "hsts.h"
#ضمن "rtsp.h"
#ضمن "ws.h"
#ضمن "bufref.h"
#ضمن "curlx/strparse.h"

فراغ Curl_http_neg_init(بنية Curl_easy *داده, بنية http_negotiation *neg)
{
  memset(neg, 0, حجم(*neg));
  neg->accept_09 = داده->مجموعه.http09_allowed;
  بدل(داده->مجموعه.httpwant) {
  حالة CURL_HTTP_نسخه_1_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    neg->only_10 = TRUE;
    اكسر;
  حالة CURL_HTTP_نسخه_1_1:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    اكسر;
  حالة CURL_HTTP_نسخه_2_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->h2_upgrade = TRUE;
    اكسر;
  حالة CURL_HTTP_نسخه_2_TLS:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    اكسر;
  حالة CURL_HTTP_نسخه_2_پیشین_KNOWLEDGE:
    neg->wanted = neg->allowed = (CURL_HTTP_V2x);
    داده->state.http_neg.h2_پیشین_knowledge = TRUE;
    اكسر;
  حالة CURL_HTTP_نسخه_3:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    neg->allowed = neg->wanted;
    اكسر;
  حالة CURL_HTTP_نسخه_3_ONLY:
    neg->wanted = neg->allowed = (CURL_HTTP_V3x);
    اكسر;
  حالة CURL_HTTP_نسخه_NONE:
  افتراضي:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    اكسر;
  }
}

CURLcode Curl_http_راهاندازی_conn(بنية Curl_easy *داده,
                              بنية connectdata *conn)
{
  /* allocate the HTTP-specific struct for the Curl_easy, only to survive
     during this request */
  إذا(داده->state.http_neg.wanted == CURL_HTTP_V3x) {
    /* only HTTP/3, needs to work */
    CURLcode نتیجه = Curl_conn_may_http3(داده, conn);
    إذا(نتیجه)
      أرجع نتیجه;
  }
  أرجع CURLE_OK;
}

#اگر_تعریف_نشده CURL_DISABLE_PROXY
/*
 * checkProxyHeaders() checks the linked list of custom proxy headers
 * if proxy headers are not available, then it will lookup into http header
 * link list
 *
 * It takes a connectdata struct as input to see if this is a proxy request or
 * not, as it then might check a different header list. Provide the header
 * prefix without colon!
 */
حرف *Curl_checkProxyheaders(بنية Curl_easy *داده,
                             ثابت بنية connectdata *conn,
                             ثابت حرف *thisheader,
                             ثابت size_t thislen)
{
  بنية curl_slist *سر;

  لكل(سر = (conn->http_proxy.peer && داده->مجموعه.sep_سرآیندها) ?
        داده->مجموعه.proxyheaders : داده->مجموعه.سرآیندها;
      سر; سر = سر->بعدی) {
    إذا(curl_strnequal(سر->داده, thisheader, thislen) &&
       Curl_headersep(سر->داده[thislen]))
      أرجع سر->داده;
  }

  أرجع عدم;
}
#پایان_اگر

/* If the header has a value, this function returns TRUE and the value is in
   'outp' with blanks trimmed off. */
ساكن bool سرآیند_has_مقدار(ثابت حرف **headerp, بنية Curl_str *outp)
{
  bool مقدار = !curlx_str_cspn(headerp, outp, ";:") &&
    (!curlx_str_single(headerp, ':') || !curlx_str_single(headerp, ';'));

  إذا(مقدار) {
    curlx_str_cspn(headerp, outp, "\r\n");
    curlx_str_trimblanks(outp);
  }
  أرجع مقدار;
}

ساكن bool http_سرآیند_is_خالی(ثابت حرف *سرآیند)
{
  بنية Curl_str بیرون;

  إذا(سرآیند_has_مقدار(&سرآیند, &بیرون)) {
    أرجع curlx_strlen(&بیرون) == 0;
  }
  أرجع TRUE; /* invalid header format, treat as empty */
}

/*
 * Strip off leading and trailing whitespace from the value in the given HTTP
 * header line and return a strdup-ed copy in 'valp' - returns an empty
 * string if the header value consists entirely of whitespace.
 *
 * If the header is provided as "name;", ending with a semicolon, it returns a
 * blank string.
 */
ساكن CURLcode copy_custom_مقدار(ثابت حرف *سرآیند, حرف **valp)
{
  بنية Curl_str بیرون = { 0 };

  /* find the end of the header name */
  إذا(سرآیند_has_مقدار(&سرآیند, &بیرون)) {
    *valp = curlx_memdup0(curlx_str(&بیرون), curlx_strlen(&بیرون));
    إذا(*valp)
      أرجع CURLE_OK;
    أرجع CURLE_بیرون_از_حافظه;
  }
  /* bad input */
  *valp = عدم;
  أرجع CURLE_BAD_FUNCTION_آرگومان;
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
حرف *Curl_copy_سرآیند_مقدار(ثابت حرف *سرآیند)
{
  بنية Curl_str بیرون;

  /* find the end of the header name */
  إذا(!curlx_str_until(&سرآیند, &بیرون, MAX_HTTP_RESP_سرآیند_اندازه, ':') &&
     !curlx_str_single(&سرآیند, ':')) {
    curlx_str_untilnl(&سرآیند, &بیرون, MAX_HTTP_RESP_سرآیند_اندازه);
    curlx_str_trimblanks(&بیرون);
    أرجع curlx_memdup0(curlx_str(&بیرون), curlx_strlen(&بیرون));
  }
  /* bad input, should never happen */
  DEBUGASSERT(0);
  أرجع عدم;
}

#اگر_تعریف_نشده CURL_DISABLE_HTTP_AUTH

#اگر_تعریف_نشده CURL_DISABLE_BASIC_AUTH
/*
 * http_output_basic() sets up an Authorization: header (or the proxy version)
 * for HTTP Basic authentication.
 *
 * Returns CURLcode.
 */
ساكن CURLcode http_output_basic(بنية Curl_easy *داده,
                                  بنية connectdata *conn, bool proxy)
{
  size_t اندازه = 0;
  حرف *authorization = عدم;
  حرف **p_hd;
  CURLcode نتیجه;
  بنية Curl_creds *creds = عدم;
  حرف *بیرون;

  /* credentials are unique per transfer for HTTP, do not use the ones for the
     connection */
  إذا(proxy) {
#اگر_تعریف_نشده CURL_DISABLE_PROXY
    p_hd = &داده->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
#وإلا
    (فراغ)conn;
    أرجع CURLE_NOT_BUILT_درون;
#پایان_اگر
  }
  وإلا {
    p_hd = &داده->req.hd_auth;
    creds = داده->state.creds;
  }

  إذا(!creds) {
    DEBUGASSERT(0);
    أرجع CURLE_FAILED_INIT;
  }

  بیرون = curl_maprintf("%s:%s", creds->user, creds->passwd);
  إذا(!بیرون)
    أرجع CURLE_بیرون_از_حافظه;

  نتیجه = curlx_base64_encode((uint8_t *)بیرون, طول_رشته(بیرون),
                               &authorization, &اندازه);
  إذا(نتیجه)
    اذهب fail;

  إذا(!authorization) {
    نتیجه = CURLE_REMOTE_ACCESS_DENIED;
    اذهب fail;
  }

  curlx_free(*p_hd);
  *p_hd = curl_maprintf("%sAuthorization: Basic %s\r\n",
                        proxy ? "Proxy-" : "",
                        authorization);
  curlx_free(authorization);
  إذا(!*p_hd) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب fail;
  }

fail:
  curlx_free(بیرون);
  أرجع نتیجه;
}

#پایان_اگر

#اگر_تعریف_نشده CURL_DISABLE_BEARER_AUTH
/*
 * http_output_bearer() sets up an Authorization: header
 * for HTTP Bearer authentication.
 *
 * Returns CURLcode.
 */
ساكن CURLcode http_output_bearer(بنية Curl_easy *داده)
{
  حرف **userp;
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(Curl_creds_has_oauth_bearer(داده->state.creds));
  userp = &داده->req.hd_auth;
  curlx_free(*userp);
  *userp = curl_maprintf("Authorization: Bearer %s\r\n",
                         Curl_creds_oauth_bearer(داده->state.creds));

  إذا(!*userp) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب fail;
  }

fail:
  أرجع نتیجه;
}
#پایان_اگر

#پایان_اگر

/* pickoneauth() selects the most favorable authentication method from the
 * ones available and the ones we want.
 *
 * return TRUE if one was picked
 */
ساكن bool pickoneauth(بنية auth *pick, بیعلامت دراز ماسک,
                        بنية Curl_creds *creds)
{
  bool have_user_pass = Curl_creds_has_user_or_pass(creds);
  bool picked;
  /* only deal with authentication we want */
  بیعلامت دراز avail = pick->avail & pick->want & ماسک;
  picked = TRUE;

  /* The order of these checks is highly relevant, as this will be the order
     of preference in case of the existence of multiple accepted types. */
  إذا(avail & CURLAUTH_NEGOTIATE)  /* available on empty creds */
    pick->picked = CURLAUTH_NEGOTIATE;
#اگر_تعریف_نشده CURL_DISABLE_BEARER_AUTH
  وإلا إذا((avail & CURLAUTH_BEARER) && Curl_creds_has_oauth_bearer(creds))
    pick->picked = CURLAUTH_BEARER;
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_DIGEST_AUTH
  وإلا إذا((avail & CURLAUTH_DIGEST) && have_user_pass)
    pick->picked = CURLAUTH_DIGEST;
#پایان_اگر
  وإلا إذا(avail & CURLAUTH_NTLM)
    pick->picked = CURLAUTH_NTLM;
#اگر_تعریف_نشده CURL_DISABLE_BASIC_AUTH
  وإلا إذا((avail & CURLAUTH_BASIC) && have_user_pass)
    pick->picked = CURLAUTH_BASIC;
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_AWS
  وإلا إذا(avail & CURLAUTH_AWS_SIGV4)
    pick->picked = CURLAUTH_AWS_SIGV4;
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_HTTPSIG
  وإلا إذا(avail & CURLAUTH_HTTPSIG)
    pick->picked = CURLAUTH_HTTPSIG;
#پایان_اگر
  وإلا {
    pick->picked = CURLAUTH_PICKNONE; /* we select to use nothing */
    picked = FALSE;
  }
  pick->avail = CURLAUTH_NONE; /* clear it here */

  أرجع picked;
}

/*
 * http_perhapsrewind()
 *
 * The current request needs to be done again - maybe due to a follow
 * or authentication negotiation. Check if:
 * 1) a rewind of the data sent to the server is necessary
 * 2) the current transfer should continue or be stopped early
 */
ساكن CURLcode http_perhapsrewind(بنية Curl_easy *داده,
                                   بنية connectdata *conn)
{
  curl_off_t bytessent = داده->req.writebytecount;
  curl_off_t expectsend = Curl_creader_مجموع_درازی(داده);
  curl_off_t upload_remain = (expectsend >= 0) ? (expectsend - bytessent) : -1;
  bool little_upload_remains = (upload_remain >= 0 && upload_remain < 2000);
  bool needs_rewind = Curl_creader_needs_rewind(داده);
  /* By default, we would like to abort the transfer when little or unknown
   * amount remains. This may be overridden by authentications further
   * below! */
  bool abort_upload = (!داده->req.upload_انجامشده && !little_upload_remains);
  VERBOSE(ثابت حرف *ongoing_auth = عدم);

  /* We need a rewind before uploading client read data again. The
   * checks below influence of the upload is to be continued
   * or aborted early.
   * This depends on how much remains to be sent and in what state
   * the authentication is. Some auth schemes such as NTLM do not work
   * for a new connection. */
  إذا(needs_rewind) {
    infof(داده, "Need to rewind upload for next request");
    Curl_creader_مجموعه_rewind(داده, TRUE);
  }

  إذا(conn->بیتها.close)
    /* If we already decided to close this connection, we cannot veto. */
    أرجع CURLE_OK;

  إذا(abort_upload) {
    /* We would like to abort the upload - but should we? */
#اگر_تعریف_شده USE_NTLM
    إذا((داده->state.authproxy.picked == CURLAUTH_NTLM) ||
       (داده->state.authhost.picked == CURLAUTH_NTLM)) {
      VERBOSE(ongoing_auth = "NTLM");
      إذا((conn->http_ntlm_state != NTLMSTATE_NONE) ||
         (conn->proxy_ntlm_state != NTLMSTATE_NONE)) {
        /* The NTLM-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#پایان_اگر
#اگر_تعریف_شده USE_SPNEGO
    /* There is still data left to send */
    إذا((داده->state.authproxy.picked == CURLAUTH_NEGOTIATE) ||
       (داده->state.authhost.picked == CURLAUTH_NEGOTIATE)) {
      VERBOSE(ongoing_auth = "NEGOTIATE");
      إذا((conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) ||
         (conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE)) {
        /* The NEGOTIATE-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#پایان_اگر
  }

  إذا(abort_upload) {
    إذا(upload_remain >= 0)
      infof(داده, "%s%sclose instead of sending %" FMT_OFF_T " more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "",
            upload_remain);
    وإلا
      infof(داده, "%s%sclose instead of sending unknown amount "
            "of more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "");
    /* We decided to abort the ongoing transfer */
    streamclose(conn);
    داده->req.اندازه = 0; /* do not download any more than 0 bytes */
    داده->req.http_bodyless = TRUE;
  }
  أرجع CURLE_OK;
}

/**
 * http_should_fail() determines whether an HTTP response code has gotten us
 * into an error state or not.
 *
 * @retval FALSE communications should continue
 *
 * @retval TRUE communications should not continue
 */
ساكن bool http_should_fail(بنية Curl_easy *داده, صحيح httpcode)
{
  DEBUGASSERT(داده);
  DEBUGASSERT(داده->conn);

  /*
   * If we have not been asked to fail on error,
   * do not fail.
   */
  إذا(!داده->مجموعه.http_fail_on_error)
    أرجع FALSE;

  /*
   * Any code < 400 is never terminal.
   */
  إذا(httpcode < 400)
    أرجع FALSE;

  /*
   * A 416 response to a resume request is presumably because the file is
   * already completely downloaded and thus not actually a fail.
   */
  إذا(داده->state.resume_from && داده->state.httpreq == HTTPREQ_GET &&
     httpcode == 416)
    أرجع FALSE;

  /*
   * Any code >= 400 that is not 401 or 407 is always
   * a terminal error
   */
  إذا((httpcode != 401) && (httpcode != 407))
    أرجع TRUE;

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
  إذا((httpcode == 401) && !داده->state.creds)
    أرجع TRUE;
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  إذا((httpcode == 407) && !داده->conn->http_proxy.creds)
    أرجع TRUE;
#پایان_اگر

  أرجع (bool)داده->state.authproblem;
}

/*
 * Curl_http_auth_act() gets called when all HTTP headers have been received
 * and it checks what authentication methods that are available and decides
 * which one (if any) to use. It will set 'newurl' if an auth method was
 * picked.
 */
CURLcode Curl_http_auth_act(بنية Curl_easy *داده)
{
  بنية connectdata *conn = داده->conn;
  bool pickhost = FALSE;
  bool pickproxy = FALSE;
  CURLcode نتیجه = CURLE_OK;
  بیعلامت دراز authmask = ~0UL;

  إذا(!Curl_creds_has_oauth_bearer(داده->state.creds))
    authmask &= (بیعلامت دراز)~CURLAUTH_BEARER;

  إذا(100 <= داده->req.httpcode && داده->req.httpcode <= 199)
    /* this is a transient response code, ignore */
    أرجع CURLE_OK;

  إذا(داده->state.authproblem)
    أرجع داده->مجموعه.http_fail_on_error ? CURLE_HTTP_RETURNED_ERROR : CURLE_OK;

  إذا(داده->state.creds &&
     ((داده->req.httpcode == 401) ||
      (داده->req.authneg && داده->req.httpcode < 300))) {
    pickhost = pickoneauth(&داده->state.authhost, authmask, داده->state.creds);
    إذا(!pickhost)
      داده->state.authproblem = TRUE;
    وإلا
      داده->info.httpauthpicked = داده->state.authhost.picked;
    إذا(داده->state.authhost.picked == CURLAUTH_NTLM &&
       (داده->req.httpversion_فرستاده > 11)) {
      infof(داده, "Forcing HTTP/1.1 for NTLM");
      connclose(conn);
      داده->state.http_neg.wanted = CURL_HTTP_V1x;
      داده->state.http_neg.allowed = CURL_HTTP_V1x;
    }
  }
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  إذا(conn->http_proxy.creds &&
     ((داده->req.httpcode == 407) ||
      (داده->req.authneg && داده->req.httpcode < 300))) {
    pickproxy = pickoneauth(&داده->state.authproxy,
                            authmask & ~CURLAUTH_BEARER,
                            conn->http_proxy.creds);
    إذا(!pickproxy)
      داده->state.authproblem = TRUE;
    وإلا
      داده->info.proxyauthpicked = داده->state.authproxy.picked;
  }
#پایان_اگر

  إذا(pickhost || pickproxy) {
    نتیجه = http_perhapsrewind(داده, conn);
    إذا(نتیجه)
      أرجع نتیجه;

    /* In case this is GSS auth, the newurl field is already allocated so
       we must make sure to free it before allocating a new one. As figured
       out in bug #2284386 */
    curlx_free(داده->req.newurl);
    /* clone URL */
    داده->req.newurl = Curl_bufref_dup(&داده->state.url);
    إذا(!داده->req.newurl)
      أرجع CURLE_بیرون_از_حافظه;
  }
  وإلا إذا((داده->req.httpcode < 300) &&
          !داده->state.authhost.انجامشده &&
          داده->req.authneg &&
          /* no (known) authentication available,
             authentication is not "done" yet and
             no authentication seems to be required and
             we did not try HEAD or GET */
          (داده->state.httpreq != HTTPREQ_GET) &&
          (داده->state.httpreq != HTTPREQ_سر)) {
    /* clone URL */
    داده->req.newurl = Curl_bufref_dup(&داده->state.url);
    إذا(!داده->req.newurl)
      أرجع CURLE_بیرون_از_حافظه;
    داده->state.authhost.انجامشده = TRUE;
  }
  إذا(http_should_fail(داده, داده->req.httpcode)) {
    failf(داده, "The requested URL returned error: %d",
          داده->req.httpcode);
    نتیجه = CURLE_HTTP_RETURNED_ERROR;
  }

  أرجع نتیجه;
}

#اگر_تعریف_نشده CURL_DISABLE_HTTP_AUTH
/*
 * Output the correct authentication header depending on the auth type
 * and whether or not it is to a proxy.
 */
ساكن CURLcode output_auth_سرآیندها(بنية Curl_easy *داده,
                                    بنية connectdata *conn,
                                    بنية auth *authstatus,
                                    ثابت حرف *request,
                                    ثابت حرف *مسیر,
                                    bool proxy)
{
  ثابت حرف *auth = عدم;
  CURLcode نتیجه = CURLE_OK;
  (فراغ)conn;

#اگر_تعریف_شده CURL_DISABLE_DIGEST_AUTH
  (فراغ)request;
  (فراغ)مسیر;
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_AWS
  إذا((authstatus->picked == CURLAUTH_AWS_SIGV4) && !proxy) {
    /* this method is never for proxy */
    auth = "AWS_SIGV4";
    نتیجه = Curl_output_aws_sigv4(داده);
    إذا(نتیجه)
      أرجع نتیجه;
  }
  وإلا
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_HTTPSIG
  إذا((authstatus->picked == CURLAUTH_HTTPSIG) && !proxy) {
    /* HTTPSIG uses its own configured key material rather than
       data->state.creds. Do not let unrelated credentials from a
       redirected URL bypass the cross-host auth boundary. */
    إذا(Curl_auth_allowed_مقصد_host(داده)) {
      auth = "HTTPSIG";
      نتیجه = Curl_output_httpsig(داده);
      إذا(نتیجه)
        أرجع نتیجه;
    }
    وإلا
      authstatus->انجامشده = TRUE;
  }
  وإلا
#پایان_اگر
#اگر_تعریف_شده USE_SPNEGO
  إذا(authstatus->picked == CURLAUTH_NEGOTIATE) {
    إذا(
#اگر_تعریف_نشده CURL_DISABLE_PROXY
      (proxy && !Curl_checkProxyheaders(داده, conn,
                                        STRCONST("Proxy-authorization"))) ||
#پایان_اگر
      (!proxy && !Curl_checkheaders(داده, STRCONST("Authorization")) &&
       Curl_auth_allowed_مقصد_host(داده))) {
      auth = "Negotiate";
      نتیجه = Curl_output_negotiate(داده, conn, proxy);
      إذا(نتیجه)
        أرجع نتیجه;
    }
    وإلا
      authstatus->انجامشده = TRUE;
  }
  وإلا
#پایان_اگر
#اگر_تعریف_شده USE_NTLM
  إذا(authstatus->picked == CURLAUTH_NTLM) {
    إذا(proxy || Curl_auth_allowed_مقصد_host(داده)) {
      auth = "NTLM";
      نتیجه = Curl_output_ntlm(داده, proxy);
      إذا(نتیجه)
        أرجع نتیجه;
    }
    وإلا
      authstatus->انجامشده = TRUE;
  }
  وإلا
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_DIGEST_AUTH
  إذا(authstatus->picked == CURLAUTH_DIGEST) {
    auth = "Digest";
    نتیجه = Curl_output_digest(داده,
                                proxy,
                                (ثابت بیعلامت حرف *)request,
                                (ثابت بیعلامت حرف *)مسیر);
    إذا(نتیجه)
      أرجع نتیجه;
  }
  وإلا
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_BASIC_AUTH
  إذا(authstatus->picked == CURLAUTH_BASIC) {
    /* Basic */
    إذا(
#اگر_تعریف_نشده CURL_DISABLE_PROXY
       (proxy && conn->http_proxy.creds &&
        Curl_creds_has_user_or_pass(conn->http_proxy.creds) &&
        !Curl_checkProxyheaders(داده, conn,
                                STRCONST("Proxy-authorization"))) ||
#پایان_اگر
       (!proxy && داده->state.creds &&
        Curl_creds_has_user_or_pass(داده->state.creds) &&
        !Curl_checkheaders(داده, STRCONST("Authorization")))) {
      auth = "Basic";
      نتیجه = http_output_basic(داده, conn, proxy);
      إذا(نتیجه)
        أرجع نتیجه;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->انجامشده = TRUE;
  }
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_BEARER_AUTH
  إذا(authstatus->picked == CURLAUTH_BEARER) {
    /* Bearer */
    إذا(!proxy && Curl_creds_has_oauth_bearer(داده->state.creds) &&
       !Curl_checkheaders(داده, STRCONST("Authorization"))) {
      auth = "Bearer";
      نتیجه = http_output_bearer(داده);
      إذا(نتیجه)
        أرجع نتیجه;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->انجامشده = TRUE;
  }
#پایان_اگر

  إذا(auth) {
#اگر_تعریف_نشده CURL_DISABLE_PROXY
    إذا(proxy)
      داده->info.proxyauthpicked = authstatus->picked;
    وإلا
      داده->info.httpauthpicked = authstatus->picked;
    infof(داده, "%s auth using %s with user '%s'",
          proxy ? "Proxy" : "Server", auth,
          proxy ? (conn->http_proxy.creds ?
                   conn->http_proxy.creds->user : "") :
          (داده->state.creds ?
           داده->state.creds->user : ""));
#وإلا
    (فراغ)proxy;
    infof(داده, "Server auth using %s with user '%s'",
          auth, داده->state.creds ?
          داده->state.creds->user : "");
#پایان_اگر
    authstatus->multipass = !authstatus->انجامشده;
  }
  وإلا {
    authstatus->multipass = FALSE;
    إذا(proxy)
      داده->info.proxyauthpicked = 0;
    وإلا
      داده->info.httpauthpicked = 0;
  }

  أرجع نتیجه;
}

CURLcode Curl_http_output_auth(بنية Curl_easy *داده,
                               بنية connectdata *conn,
                               ثابت حرف *request,
                               Curl_HttpReq httpreq,
                               ثابت حرف *مسیر,
                               ثابت حرف *query,
                               bool is_connect)
{
  CURLcode نتیجه = CURLE_OK;
  بنية auth *authhost;
  بنية auth *authproxy;
  ثابت حرف *مسیر_and_query = مسیر;
  حرف *tmp_str = عدم;

  DEBUGASSERT(داده);
  authhost = &داده->state.authhost;
  authproxy = &داده->state.authproxy;

  إذا(
#اگر_تعریف_نشده CURL_DISABLE_PROXY
    (!conn->http_proxy.peer || !conn->http_proxy.creds) &&
#پایان_اگر
#اگر_تعریف_شده USE_SPNEGO
    !(authhost->want & CURLAUTH_NEGOTIATE) &&
    !(authproxy->want & CURLAUTH_NEGOTIATE) &&
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_HTTPSIG
    !(authhost->want & CURLAUTH_HTTPSIG) &&
#پایان_اگر
    !داده->state.creds) {
    /* no authentication with no user or password */
    authhost->انجامشده = TRUE;
    authproxy->انجامشده = TRUE;
    نتیجه = CURLE_OK;
    اذهب بیرون;
  }

  إذا(query) {
    tmp_str = curl_maprintf("%s?%s", مسیر, query);
    إذا(!tmp_str) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }
    مسیر_and_query = tmp_str;
  }

  إذا(authhost->want && !authhost->picked)
    /* The app has selected one or more methods, but none has been picked
       so far by a server round-trip. Then we set the picked one to the
       want one, and if this is one single bit it will be used instantly. */
    authhost->picked = authhost->want;

  إذا(authproxy->want && !authproxy->picked)
    /* The app has selected one or more methods, but none has been picked so
       far by a proxy round-trip. Then we set the picked one to the want one,
       and if this is one single bit it will be used instantly. */
    authproxy->picked = authproxy->want;

#اگر_تعریف_نشده CURL_DISABLE_PROXY
  /* Send proxy authentication header if needed */
  إذا(conn->بیتها.origin_is_proxy || is_connect) {
    نتیجه = output_auth_سرآیندها(داده, conn, authproxy, request,
                                 مسیر_and_query, TRUE);
    إذا(نتیجه)
      اذهب بیرون;
  }
  وإلا
#وإلا
  (فراغ)is_connect;
#پایان_اگر /* CURL_DISABLE_PROXY */
    /* we have no proxy so let's pretend we are done authenticating
       with it */
    authproxy->انجامشده = TRUE;

  /* Either we have credentials for the origin we talk to or
     performing authentication is allowed here */
  إذا(داده->state.creds || Curl_auth_allowed_مقصد_host(داده))
    نتیجه = output_auth_سرآیندها(داده, conn, authhost, request,
                                 مسیر_and_query, FALSE);
  وإلا
    authhost->انجامشده = TRUE;

  إذا(((authhost->multipass && !authhost->انجامشده) ||
      (authproxy->multipass && !authproxy->انجامشده)) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_سر)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    داده->req.authneg = TRUE;
  }
  وإلا
    داده->req.authneg = FALSE;

بیرون:
  curlx_free(tmp_str);
  أرجع نتیجه;
}

#وإلا /* !CURL_DISABLE_HTTP_AUTH */
/* when disabled */
CURLcode Curl_http_output_auth(بنية Curl_easy *داده,
                               بنية connectdata *conn,
                               ثابت حرف *request,
                               Curl_HttpReq httpreq,
                               ثابت حرف *مسیر,
                               ثابت حرف *query,
                               bool is_connect)
{
  (فراغ)داده;
  (فراغ)conn;
  (فراغ)request;
  (فراغ)httpreq;
  (فراغ)مسیر;
  (فراغ)query;
  (فراغ)is_connect;
  أرجع CURLE_OK;
}
#پایان_اگر /* !CURL_DISABLE_HTTP_AUTH, else */

#إذا defined(USE_SPNEGO) || defined(USE_NTLM) || \
  !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_BASIC_AUTH) || \
  !defined(CURL_DISABLE_BEARER_AUTH)
ساكن bool authcmp(ثابت حرف *auth, ثابت حرف *سطر)
{
  /* the auth string must not have an alnum following */
  size_t n = طول_رشته(auth);
  أرجع curl_strnequal(auth, سطر, n) && !ISALNUM(سطر[n]);
}
#پایان_اگر

#اگر_تعریف_شده USE_SPNEGO
ساكن CURLcode auth_spnego(بنية Curl_easy *داده,
                            bool proxy,
                            ثابت حرف *auth,
                            بنية auth *authp,
                            uint32_t *availp)
{
  إذا((authp->avail & CURLAUTH_NEGOTIATE) || Curl_auth_is_spnego_supported()) {
    *availp |= CURLAUTH_NEGOTIATE;
    authp->avail |= CURLAUTH_NEGOTIATE;

    إذا(authp->picked == CURLAUTH_NEGOTIATE) {
      بنية connectdata *conn = داده->conn;
      CURLcode نتیجه = Curl_درون_داد_negotiate(داده, conn, proxy, auth);
      uint8_t *negstate = proxy ? &conn->proxy_negotiate_state :
        &conn->http_negotiate_state;
      إذا(!نتیجه) {
        curlx_free(داده->req.newurl);
        داده->req.newurl = Curl_bufref_dup(&داده->state.url);
        إذا(!داده->req.newurl)
          أرجع CURLE_بیرون_از_حافظه;
        داده->state.authproblem = FALSE;
        /* we received a GSS auth token and we dealt with it fine */
        *negstate = CURL_NEGO_AUTH_RECV;
      }
      وإلا
        داده->state.authproblem = TRUE;
    }
  }
  أرجع CURLE_OK;
}
#پایان_اگر

#اگر_تعریف_شده USE_NTLM
ساكن CURLcode auth_ntlm(بنية Curl_easy *داده,
                          bool proxy,
                          ثابت حرف *auth,
                          بنية auth *authp,
                          uint32_t *availp)
{
  /* NTLM support requires the SSL crypto libs */
  إذا((authp->avail & CURLAUTH_NTLM) || Curl_auth_is_ntlm_supported()) {
    *availp |= CURLAUTH_NTLM;
    authp->avail |= CURLAUTH_NTLM;

    إذا(authp->picked == CURLAUTH_NTLM) {
      /* NTLM authentication is picked and activated */
      CURLcode نتیجه = Curl_درون_داد_ntlm(داده, proxy, auth);
      إذا(!نتیجه)
        داده->state.authproblem = FALSE;
      وإلا {
        إذا(نتیجه == CURLE_بیرون_از_حافظه)
          أرجع نتیجه;
        infof(داده, "NTLM authentication problem, ignoring.");
        داده->state.authproblem = TRUE;
      }
    }
  }
  أرجع CURLE_OK;
}
#پایان_اگر

#اگر_تعریف_نشده CURL_DISABLE_DIGEST_AUTH
ساكن CURLcode auth_digest(بنية Curl_easy *داده,
                            bool proxy,
                            ثابت حرف *auth,
                            بنية auth *authp,
                            uint32_t *availp)
{
  إذا(authp->avail & CURLAUTH_DIGEST) {
    *availp |= CURLAUTH_DIGEST;
    infof(داده, "Ignoring duplicate digest auth header.");
  }
  وإلا إذا(Curl_auth_is_digest_supported()) {
    CURLcode نتیجه;

    *availp |= CURLAUTH_DIGEST;
    authp->avail |= CURLAUTH_DIGEST;

    /* We call this function on input Digest headers even if Digest
     * authentication is not activated yet, as we need to store the
     * incoming data from this header in case we are going to use
     * Digest */
    نتیجه = Curl_درون_داد_digest(داده, proxy, auth);
    إذا(نتیجه) {
      إذا(نتیجه == CURLE_بیرون_از_حافظه)
        أرجع نتیجه;
      infof(داده, "Digest authentication problem, ignoring.");
      داده->state.authproblem = TRUE;
    }
  }
  أرجع CURLE_OK;
}
#پایان_اگر

#اگر_تعریف_نشده CURL_DISABLE_BASIC_AUTH
ساكن CURLcode auth_basic(بنية Curl_easy *داده,
                           بنية auth *authp,
                           uint32_t *availp)
{
  *availp |= CURLAUTH_BASIC;
  authp->avail |= CURLAUTH_BASIC;
  إذا(authp->picked == CURLAUTH_BASIC) {
    /* We asked for Basic authentication but got a 40X back anyway, which
       means our name+password is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(داده, "Basic authentication problem, ignoring.");
    داده->state.authproblem = TRUE;
  }
  أرجع CURLE_OK;
}
#پایان_اگر

#اگر_تعریف_نشده CURL_DISABLE_BEARER_AUTH
ساكن CURLcode auth_bearer(بنية Curl_easy *داده,
                            بنية auth *authp,
                            uint32_t *availp)
{
  *availp |= CURLAUTH_BEARER;
  authp->avail |= CURLAUTH_BEARER;
  إذا(authp->picked == CURLAUTH_BEARER) {
    /* We asked for Bearer authentication but got a 40X back anyway, which
       means our token is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(داده, "Bearer authentication problem, ignoring.");
    داده->state.authproblem = TRUE;
  }
  أرجع CURLE_OK;
}
#پایان_اگر

/*
 * Curl_http_input_auth() deals with Proxy-Authenticate: and WWW-Authenticate:
 * headers. They are dealt with both in the transfer.c main loop and in the
 * proxy CONNECT loop.
 *
 * The 'auth' line ends with a NUL byte without CR or LF present.
 */
CURLcode Curl_http_درون_داد_auth(بنية Curl_easy *داده, bool proxy,
                              ثابت حرف *auth) /* the first non-space */
{
  /*
   * This resource requires authentication
   */
#إذا defined(USE_SPNEGO) ||                      \
  defined(USE_NTLM) ||                          \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||         \
  !defined(CURL_DISABLE_BASIC_AUTH) ||          \
  !defined(CURL_DISABLE_BEARER_AUTH)

  uint32_t *availp;
  بنية auth *authp;
  CURLcode نتیجه = CURLE_OK;
  DEBUGASSERT(auth);
  DEBUGASSERT(داده);

  إذا(proxy) {
    availp = &داده->info.proxyauthavail;
    authp = &داده->state.authproxy;
  }
  وإلا {
    availp = &داده->info.httpauthavail;
    authp = &داده->state.authhost;
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

  طالما(*auth) {
#اگر_تعریف_شده USE_SPNEGO
    إذا(authcmp("Negotiate", auth))
      نتیجه = auth_spnego(داده, proxy, auth, authp, availp);
#پایان_اگر
#اگر_تعریف_شده USE_NTLM
    إذا(!نتیجه && authcmp("NTLM", auth))
      نتیجه = auth_ntlm(داده, proxy, auth, authp, availp);
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_DIGEST_AUTH
    إذا(!نتیجه && authcmp("Digest", auth))
      نتیجه = auth_digest(داده, proxy, auth, authp, availp);
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_BASIC_AUTH
    إذا(!نتیجه && authcmp("Basic", auth))
      نتیجه = auth_basic(داده, authp, availp);
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_BEARER_AUTH
    إذا(authcmp("Bearer", auth))
      نتیجه = auth_bearer(داده, authp, availp);
#پایان_اگر

    إذا(نتیجه)
      اكسر;

    /* there may be multiple methods on one line, so keep reading */
    auth = strchr(auth, ',');
    إذا(auth) /* if we are on a comma, skip it */
      auth++;
    وإلا
      اكسر;
    curlx_str_passblanks(&auth);
  }
  أرجع نتیجه;
#وإلا
  (فراغ)داده;
  (فراغ)proxy;
  (فراغ)auth;
  /* nothing to do when disabled */
  أرجع CURLE_OK;
#پایان_اگر
}

ساكن فراغ http_switch_مقصد_get(بنية Curl_easy *داده, صحيح کد)
{
  ثابت حرف *req = CURL_EASY_STR(داده, STRING_CUSTOMREQUEST);

  إذا((req || داده->state.httpreq != HTTPREQ_GET) &&
     (داده->مجموعه.http_follow_نما == CURLFOLLOW_OBEYCODE)) {
    NOVERBOSE((فراغ)کد);
    infof(داده, "Switch to GET because of %d response", کد);
    داده->state.http_ignorecustom = TRUE;
  }
  وإلا إذا(req && (داده->مجموعه.http_follow_نما != CURLFOLLOW_FIRSTONLY))
    infof(داده, "Stick to %s instead of GET", req);

  داده->state.httpreq = HTTPREQ_GET;
  Curl_creader_مجموعه_rewind(داده, FALSE);
}

#تعریف_کن HTTPREQ_IS_POST(داده)                    \
  ((داده)->state.httpreq == HTTPREQ_POST ||      \
   (داده)->state.httpreq == HTTPREQ_POST_FORM || \
   (داده)->state.httpreq == HTTPREQ_POST_MIME)

ساكن CURLcode مجموعه_auto_referer(بنية Curl_easy *داده)
{
  CURLU *u;
  حرف *referer = عدم;
  CURLUcode uc;

  /* We are asked to automatically set the previous URL as the referer
     when we get the next URL. We pick the ->url field, which may or may
     not be 100% correct */
  Curl_bufref_free(&داده->state.referer);

  /* Make a copy of the URL without credentials and fragment */
  u = curl_url();
  إذا(!u)
    أرجع CURLE_بیرون_از_حافظه;

  uc = curl_url_مجموعه(u, CURLUPART_URL,
                    Curl_bufref_ptr(&داده->state.url), 0);
  إذا(!uc)
    uc = curl_url_مجموعه(u, CURLUPART_FRAGMENT, عدم, 0);
  إذا(!uc)
    uc = curl_url_مجموعه(u, CURLUPART_USER, عدم, 0);
  إذا(!uc)
    uc = curl_url_مجموعه(u, CURLUPART_PASSWORD, عدم, 0);
  إذا(!uc)
    uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

  curl_url_cleanup(u);

  إذا(uc || !referer)
    أرجع CURLE_بیرون_از_حافظه;

  Curl_bufref_مجموعه(&داده->state.referer, referer, 0, curl_free);
  أرجع CURLE_OK;
}

ساكن CURLcode follow_parse_url(بنية Curl_easy *داده, ثابت حرف *newurl,
                                 followtype type, حرف **follow_urlp,
                                 CURLUcode *ucp)
{
  CURLUcode uc;

  DEBUGASSERT(داده->state.uh);
  uc = curl_url_مجموعه(داده->state.uh, CURLUPART_URL, newurl, (بیعلامت صحيح)
                    ((type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                     ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) |
                     CURLU_ALLOW_SPACE |
                     (داده->مجموعه.مسیر_as_is ? CURLU_مسیر_AS_IS : 0)));
  *ucp = uc;
  إذا(uc) {
    إذا((uc == CURLUE_بیرون_از_حافظه) || (type != FOLLOW_FAKE)) {
      failf(داده, "The redirect target URL could not be parsed: %s",
            curl_url_strerror(uc));
      أرجع Curl_uc_مقصد_curlcode(uc);
    }

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, duplicate the field as-is */
    *follow_urlp = curlx_strdup(newurl);
    إذا(!*follow_urlp)
      أرجع CURLE_بیرون_از_حافظه;
  }
  وإلا {
    CURLU *u = curl_url();
    إذا(!u)
      أرجع CURLE_بیرون_از_حافظه;
    uc = curl_url_مجموعه(u, CURLUPART_URL,
                      Curl_bufref_ptr(&داده->state.url),
                      CURLU_URLENCODE | CURLU_ALLOW_SPACE);
    إذا(!uc)
      uc = curl_url_get(داده->state.uh, CURLUPART_URL, follow_urlp, 0);
    إذا(uc) {
      curl_url_cleanup(u);
      أرجع Curl_uc_مقصد_curlcode(uc);
    }

#اگر_تعریف_نشده CURL_DISABLE_DIGEST_AUTH
    {
      bool same_origin = Curl_url_same_origin(u, داده->state.uh);
      curl_url_cleanup(u);
      إذا(!same_origin)
        Curl_auth_digest_cleanup(&داده->state.digest);
    }
#وإلا
    curl_url_cleanup(u);
#پایان_اگر
  }
  أرجع CURLE_OK;
}

ساكن CURLcode follow_fake(بنية Curl_easy *داده, حرف *follow_url,
                            CURLUcode uc, bool reachedmax)
{
  /* we are only figuring out the new URL if we would have followed locations
     but now we are done so we can get out! */
  إذا(!uc) {
    CURLU *u = curl_url();
    حرف *nocred = عدم;

    إذا(!u) {
      curlx_free(follow_url);
      أرجع CURLE_بیرون_از_حافظه;
    }
    إذا(!curl_url_مجموعه(u, CURLUPART_URL, follow_url,
                     CURLU_NON_SUPPORT_SCHEME |
                     (داده->مجموعه.مسیر_as_is ? CURLU_مسیر_AS_IS : 0)) &&
       !curl_url_مجموعه(u, CURLUPART_USER, عدم, 0) &&
       !curl_url_مجموعه(u, CURLUPART_PASSWORD, عدم, 0))
      (فراغ)curl_url_get(u, CURLUPART_URL, &nocred, CURLU_GET_خالی);
    curl_url_cleanup(u);
    إذا(nocred) {
      curlx_free(follow_url);
      follow_url = nocred;
    }
  }
  داده->info.wouldredirect = follow_url;

  إذا(reachedmax) {
    failf(داده, "Maximum (%d) redirects followed", داده->مجموعه.maxredirs);
    أرجع CURLE_TOO_MANY_REDIRECTS;
  }
  أرجع CURLE_OK;
}

ساكن bool follow_switch_get(بنية Curl_easy *داده)
{
  /*
   * We get here when the HTTP code is 300-399 and 401 or 407. We need to
   * perform differently based on exactly what return code there was.
   */
  بدل(داده->info.httpcode) {
  حالة 301: /* Moved Permanently */
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
    إذا(HTTPREQ_IS_POST(داده) && !داده->مجموعه.post301) {
      http_switch_مقصد_get(داده, 301);
      أرجع TRUE;
    }
    اكسر;
  حالة 302: /*یافت*/
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
    إذا(HTTPREQ_IS_POST(داده) && !داده->مجموعه.post302) {
      http_switch_مقصد_get(داده, 302);
      أرجع TRUE;
    }
    اكسر;
  حالة 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     */
    إذا(!HTTPREQ_IS_POST(داده) || !داده->مجموعه.post303) {
      http_switch_مقصد_get(داده, 303);
      أرجع TRUE;
    }
    اكسر;

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
  افتراضي:
    اكسر;
  }
  أرجع FALSE;
}

CURLcode Curl_http_follow(بنية Curl_easy *داده, ثابت حرف *newurl,
                          followtype type)
{
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  حرف *follow_url = عدم;
  CURLUcode uc;
  CURLcode نتیجه;
  CURLcode rewind_نتیجه;
  bool switch_مقصد_get;

  DEBUGASSERT(type != FOLLOW_NONE);

  إذا(type != FOLLOW_FAKE)
    داده->state.درخواستها++; /* count all real follows */
  إذا(type == FOLLOW_REDIR) {
    إذا((داده->مجموعه.maxredirs != -1) &&
       (داده->state.followlocation >= داده->مجموعه.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    وإلا {
      داده->state.followlocation++; /* count redirect-followings, including
                                       auth reloads */

      إذا(داده->مجموعه.http_auto_referer) {
        نتیجه = مجموعه_auto_referer(داده);
        إذا(نتیجه)
          أرجع نتیجه;
      }
    }
  }

  إذا((type != FOLLOW_RETRY) &&
     (داده->req.httpcode != 401) && (داده->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, عدم, 0, FALSE)) {
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: do not allow a custom port number */
    disallowport = TRUE;
  }

  نتیجه = follow_parse_url(داده, newurl, type, &follow_url, &uc);
  إذا(نتیجه)
    أرجع نتیجه;

  DEBUGASSERT(follow_url);

  إذا(type == FOLLOW_FAKE)
    أرجع follow_fake(داده, follow_url, uc, reachedmax);

  إذا(disallowport)
    داده->state.allow_پورت = FALSE;

  Curl_bufref_مجموعه(&داده->state.url, follow_url, 0, curl_free);
  rewind_نتیجه = Curl_req_soft_reset(&داده->req, داده);
  infof(داده, "Issue another request to this URL: '%s'", follow_url);
  إذا((داده->مجموعه.http_follow_نما == CURLFOLLOW_FIRSTONLY) &&
     !داده->state.http_ignorecustom &&
     CURL_EASY_STR(داده, STRING_CUSTOMREQUEST)) {
    داده->state.http_ignorecustom = TRUE;
    infof(داده, "Drop custom request method for next request");
  }

  switch_مقصد_get = follow_switch_get(داده);

  /* When rewind of upload data failed and we are not switching to GET,
   * we need to fail the follow, as we cannot send the data again. */
  إذا(rewind_نتیجه && !switch_مقصد_get)
    أرجع rewind_نتیجه;

  Curl_pgrs_زمان(داده, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(داده);

  أرجع CURLE_OK;
}

/*
 * Curl_compareheader()
 *
 * Returns TRUE if 'headerline' contains the 'header' with given 'content'
 * (within a comma-separated list of tokens). Pass 'header' WITH the colon.
 *
 * @unittest: 1625
 */
bool Curl_compareheader(ثابت حرف *headerline, /* line to check */
                        ثابت حرف *سرآیند, /* header keyword _with_ colon */
                        ثابت size_t hlen, /* len of the keyword in bytes */
                        ثابت حرف *content, /* content string to find */
                        ثابت size_t clen) /* len of the content in bytes */
{
  /* RFC2616, section 4.2 says: "Each header field consists of a name followed
   * by a colon (":") and the field value. Field names are case-insensitive.
   * The field value MAY be preceded by any amount of LWS, though a single SP
   * is preferred." */

  ثابت حرف *p;
  بنية Curl_str val;
  DEBUGASSERT(hlen);
  DEBUGASSERT(clen);
  DEBUGASSERT(سرآیند);
  DEBUGASSERT(content);

  إذا(!curl_strnequal(headerline, سرآیند, hlen))
    أرجع FALSE; /* does not start with header */

  /* pass the header */
  p = &headerline[hlen];

  إذا(curlx_str_cspn(&p, &val, "\r\n"))
    أرجع FALSE;
  curlx_str_trimblanks(&val);

  /* find the content string in the rest of the line */
  إذا(curlx_strlen(&val) >= clen) {
    size_t len;
    p = curlx_str(&val);
    لكل(len = curlx_strlen(&val); len >= clen;) {
      بنية Curl_str بعدی;
      ثابت حرف *o = p;
      /* after a match there must be a comma, space, newline or NUL byte */
      إذا(curl_strnequal(p, content, clen) &&
         ((p[clen] == ',') || ISBLANK(p[clen]) || ISNEWLINE(p[clen]) ||
          !p[clen]))
        أرجع TRUE; /* match! */
      /* advance to the next comma */
      إذا(curlx_str_until(&p, &بعدی, len, ',') ||
         curlx_str_single(&p, ','))
        اكسر; /* no comma, get out */

      /* if there are more dummy commas, move over them as well */
      افعل
        curlx_str_passblanks(&p);
      طالما(!curlx_str_single(&p, ','));
      /* trailing blanks may move the parsing point past the value end,
         then there is nothing left to match */
      إذا((size_t)(p - o) > len)
        اكسر;
      len -= (p - o);
    }
  }
  أرجع FALSE; /* no match */
}

بنية cr_exp100_ctx {
  بنية Curl_creader super;
  بنية curltime شروع; /* time started waiting */
  تعداد expect100 state;
};

/* Expect: 100-continue client reader, blocking uploads */

ساكن فراغ http_exp100_continue(بنية Curl_easy *داده,
                                 بنية Curl_creader *reader)
{
  بنية cr_exp100_ctx *ctx = reader->ctx;
  إذا(ctx->state > EXP100_SEND_داده) {
    ctx->state = EXP100_SEND_داده;
    Curl_expire_clear(داده, EXPIRE_100_TIMEOUT);
  }
}

ساكن CURLcode cr_exp100_read(بنية Curl_easy *داده,
                               بنية Curl_creader *reader,
                               حرف *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  بنية cr_exp100_ctx *ctx = reader->ctx;
  timediff_t ms;

  بدل(ctx->state) {
  حالة EXP100_SENDING_REQUEST:
    إذا(!Curl_req_sendbuf_خالی(داده)) {
      /* The initial request data has not been fully sent yet. Do
       * not start the timer yet. */
      DEBUGF(infof(داده, "cr_exp100_read, request not full sent yet"));
      *nread = 0;
      *eos = FALSE;
      أرجع CURLE_OK;
    }
    /* We are now waiting for a reply from the server or
     * a timeout on our side IFF the request has been fully sent. */
    DEBUGF(infof(داده, "cr_exp100_read, start AWAITING_CONTINUE, "
                 "timeout %dms", داده->مجموعه.expect_100_timeout));
    ctx->state = EXP100_AWAITING_CONTINUE;
    ctx->شروع = *Curl_pgrs_now(داده);
    Curl_expire_مجموعه(داده, EXPIRE_100_TIMEOUT,
                    داده->مجموعه.expect_100_timeout, &ctx->شروع);
    *nread = 0;
    *eos = FALSE;
    أرجع CURLE_OK;
  حالة EXP100_FAILED:
    DEBUGF(infof(داده, "cr_exp100_read, expectation failed, error"));
    *nread = 0;
    *eos = FALSE;
    أرجع CURLE_READ_ERROR;
  حالة EXP100_AWAITING_CONTINUE:
    ms = curlx_ptimediff_ms(Curl_pgrs_now(داده), &ctx->شروع);
    إذا(ms < داده->مجموعه.expect_100_timeout) {
      DEBUGF(infof(داده, "cr_exp100_read, AWAITING_CONTINUE, not expired"));
      *nread = 0;
      *eos = FALSE;
      أرجع CURLE_OK;
    }
    /* we have waited long enough, continue anyway */
    http_exp100_continue(داده, reader);
    infof(داده, "Done waiting for 100-continue");
    FALLTHROUGH();
  افتراضي:
    DEBUGF(infof(داده, "cr_exp100_read, pass through"));
    أرجع Curl_creader_read(داده, reader->بعدی, buf, blen, nread, eos);
  }
}

ساكن فراغ cr_exp100_انجامشده(بنية Curl_easy *داده,
                           بنية Curl_creader *reader, صحيح premature)
{
  بنية cr_exp100_ctx *ctx = reader->ctx;
  ctx->state = premature ? EXP100_FAILED : EXP100_SEND_داده;
  Curl_expire_clear(داده, EXPIRE_100_TIMEOUT);
}

ساكن ثابت بنية Curl_crtype cr_exp100 = {
  "cr-exp100",
  Curl_creader_def_init,
  cr_exp100_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  Curl_creader_def_مجموع_درازی,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  cr_exp100_انجامشده,
  حجم(بنية cr_exp100_ctx)
};

ساكن CURLcode http_exp100_جمع_کن_reader(بنية Curl_easy *داده)
{
  بنية Curl_creader *reader = عدم;
  CURLcode نتیجه;

  نتیجه = Curl_creader_create(&reader, داده, &cr_exp100, CURL_CR_PROTOCOL);
  إذا(!نتیجه)
    نتیجه = Curl_creader_جمع_کن(داده, reader);
  إذا(!نتیجه) {
    بنية cr_exp100_ctx *ctx = reader->ctx;
    ctx->state = EXP100_SENDING_REQUEST;
  }

  إذا(نتیجه && reader)
    Curl_creader_free(داده, reader);
  أرجع نتیجه;
}

ساكن فراغ http_exp100_got100(بنية Curl_easy *داده)
{
  بنية Curl_creader *r = Curl_creader_get_by_type(داده, &cr_exp100);
  إذا(r)
    http_exp100_continue(داده, r);
}

ساكن bool http_exp100_is_انتظار(بنية Curl_easy *داده)
{
  بنية Curl_creader *r = Curl_creader_get_by_type(داده, &cr_exp100);
  إذا(r) {
    بنية cr_exp100_ctx *ctx = r->ctx;
    أرجع ctx->state == EXP100_AWAITING_CONTINUE;
  }
  أرجع FALSE;
}

ساكن فراغ http_exp100_send_anyway(بنية Curl_easy *داده)
{
  بنية Curl_creader *r = Curl_creader_get_by_type(داده, &cr_exp100);
  إذا(r)
    http_exp100_continue(داده, r);
}

ساكن bool http_exp100_is_selected(بنية Curl_easy *داده)
{
  بنية Curl_creader *r = Curl_creader_get_by_type(داده, &cr_exp100);
  أرجع !!r;
}

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
CURLcode Curl_http_doing_pollset(بنية Curl_easy *داده,
                                 بنية easy_pollset *ps)
{
  /* write mode */
  أرجع Curl_pollset_جمع_کن_بیرون(داده, ps, داده->conn->sock[FIRSTSOCKET]);
}

CURLcode Curl_http_perform_pollset(بنية Curl_easy *داده,
                                   بنية easy_pollset *ps)
{
  بنية connectdata *conn = داده->conn;
  CURLcode نتیجه = CURLE_OK;

  إذا(CURL_REQ_WANT_RECV(داده)) {
    نتیجه = Curl_pollset_جمع_کن_درون(داده, ps, conn->sock[FIRSTSOCKET]);
  }

  /* on a "Expect: 100-continue" timed wait, do not poll for outgoing */
  إذا(!نتیجه && Curl_req_want_send(داده) && !http_exp100_is_انتظار(داده)) {
    نتیجه = Curl_pollset_جمع_کن_بیرون(داده, ps, conn->sock[FIRSTSOCKET]);
  }
  أرجع نتیجه;
}

ساكن CURLcode http_write_سرآیند(بنية Curl_easy *داده,
                                  ثابت حرف *hd, size_t hdlen)
{
  CURLcode نتیجه;
  صحيح writetype;

  /* now, only output this if the header AND body are requested:
   */
  Curl_debug(داده, CURLINFO_سرآیند_درون, hd, hdlen);

  writetype = CLIENTWRITE_سرآیند |
    ((داده->req.httpcode / 100 == 1) ? CLIENTWRITE_1XX : 0);

  نتیجه = Curl_client_write(داده, writetype, hd, hdlen);
  إذا(نتیجه)
    أرجع نتیجه;

  نتیجه = Curl_bump_headersize(داده, hdlen, FALSE);
  إذا(نتیجه)
    أرجع نتیجه;

  داده->req.deductheadercount = (100 <= داده->req.httpcode &&
                                 199 >= داده->req.httpcode) ?
    داده->req.headerbytecount : 0;
  أرجع نتیجه;
}

/*
 * Curl_http_done() gets called after a single HTTP request has been
 * performed.
 */

CURLcode Curl_http_انجامشده(بنية Curl_easy *داده,
                        CURLcode وضعیت, bool premature)
{
  بنية connectdata *conn = داده->conn;

  /* Clear multipass flag. If authentication is not done yet, then it will get
   * a chance to be set back to true when we output the next auth header */
  داده->state.authhost.multipass = FALSE;
  داده->state.authproxy.multipass = FALSE;

  إذا(curlx_dyn_len(&داده->state.headerb)) {
    (فراغ)http_write_سرآیند(داده, curlx_dyn_ptr(&داده->state.headerb),
                            curlx_dyn_len(&داده->state.headerb));
  }
  curlx_dyn_reset(&داده->state.headerb);

  إذا(وضعیت)
    أرجع وضعیت;

  إذا(!premature && /* this check is pointless when DONE is called before the
                      entire operation is complete */
     !conn->بیتها.retry &&
     !داده->مجموعه.connect_only &&
     (داده->req.bytecount +
      داده->req.headerbytecount -
      داده->req.deductheadercount) <= 0) {
    /* If this connection is not closed to be retried, AND nothing was
       read from the HTTP server (that counts), this cannot be right so we
       return an error here */
    failf(داده, "Empty reply from server");
    /* Mark it as closed to avoid the "left intact" message */
    streamclose(conn);
    أرجع CURLE_GOT_NOTHING;
  }

  أرجع CURLE_OK;
}

/* Determine if we may use HTTP 1.1 for this request. */
ساكن bool http_may_use_1_1(ثابت بنية Curl_easy *داده)
{
  ثابت بنية connectdata *conn = داده->conn;
  /* We have seen a previous response for *this* transfer with 1.0,
   * on another connection or the same one. */
  إذا(داده->state.http_neg.rcvd_min == 10)
    أرجع FALSE;
  /* We have seen a previous response on *this* connection with 1.0. */
  إذا(conn && conn->httpversion_دیده == 10)
    أرجع FALSE;
  /* We want 1.0 and have seen no previous response on *this* connection
     with a higher version (maybe no response at all yet). */
  إذا(داده->state.http_neg.only_10 &&
     (!conn || conn->httpversion_دیده <= 10))
    أرجع FALSE;
  /* We are not restricted to use 1.0 only. */
  أرجع !داده->state.http_neg.only_10;
}

ساكن بیعلامت حرف http_request_نسخه(بنية Curl_easy *داده)
{
  بیعلامت حرف v = Curl_conn_http_نسخه(داده, داده->conn);
  إذا(!v) {
    /* No specific HTTP connection filter installed. */
    v = http_may_use_1_1(داده) ? 11 : 10;
  }
  أرجع v;
}

ساكن ثابت حرف *get_http_string(صحيح httpversion)
{
  بدل(httpversion) {
  حالة 30:
    أرجع "3";
  حالة 20:
    أرجع "2";
  حالة 11:
    أرجع "1.1";
  افتراضي:
    أرجع "1.0";
  }
}

CURLcode Curl_جمع_کن_custom_سرآیندها(بنية Curl_easy *داده,
                                 bool is_connect, صحيح httpversion,
                                 بنية dynbuf *req)
{
  بنية curl_slist *h[2];
  بنية curl_slist *سرآیندها;
  صحيح numlists = 1; /* by default */
  صحيح i;

#اگر_تعریف_نشده CURL_DISABLE_PROXY
  تعداد Curl_proxy_use proxy;

  إذا(is_connect)
    proxy = سرآیند_CONNECT;
  وإلا
    proxy = داده->conn->بیتها.origin_is_proxy ? سرآیند_PROXY : سرآیند_SERVER;

  بدل(proxy) {
  حالة سرآیند_SERVER:
    h[0] = داده->مجموعه.سرآیندها;
    اكسر;
  حالة سرآیند_PROXY:
    h[0] = داده->مجموعه.سرآیندها;
    إذا(داده->مجموعه.sep_سرآیندها) {
      h[1] = داده->مجموعه.proxyheaders;
      numlists++;
    }
    اكسر;
  حالة سرآیند_CONNECT:
    إذا(داده->مجموعه.sep_سرآیندها)
      h[0] = داده->مجموعه.proxyheaders;
    وإلا
      h[0] = داده->مجموعه.سرآیندها;
    اكسر;
  حالة سرآیند_CONNECT_UDP:
    إذا(داده->مجموعه.sep_سرآیندها)
      h[0] = داده->مجموعه.proxyheaders;
    وإلا
      h[0] = داده->مجموعه.سرآیندها;
    اكسر;
  }
#وإلا
  (فراغ)is_connect;
  h[0] = داده->مجموعه.سرآیندها;
#پایان_اگر

  /* loop through one or two lists */
  لكل(i = 0; i < numlists; i++) {
    لكل(سرآیندها = h[i]; سرآیندها; سرآیندها = سرآیندها->بعدی) {
      CURLcode نتیجه = CURLE_OK;
      bool blankheader = FALSE;
      بنية Curl_str name;
      ثابت حرف *p = سرآیندها->داده;
      ثابت حرف *origp = p;
      size_t hlen = طول_رشته(origp);

      /* explicitly asked to send header without content is done by a header
         that ends with a semicolon, but there must be no colon present in the
         name */
      إذا(!curlx_str_until(&p, &name, hlen, ';') &&
         !curlx_str_single(&p, ';') &&
         !curlx_str_single(&p, '\0') &&
         !memchr(curlx_str(&name), ':', curlx_strlen(&name)))
        blankheader = TRUE;
      وإلا {
        p = origp;
        إذا(!curlx_str_until(&p, &name, hlen, ':') &&
           !curlx_str_single(&p, ':')) {
          بنية Curl_str val;
          curlx_str_untilnl(&p, &val, hlen);
          curlx_str_trimblanks(&val);
          إذا(!curlx_strlen(&val))
            /* no content, do not send this */
            استمر;
        }
        وإلا
          /* no colon */
          استمر;
      }

      /* a field name is a token and carries no surrounding whitespace, so
         trim the parsed name before matching. Otherwise `Authorization :`
         (space before the colon) slips past the Authorization/Cookie check
         below and gets forwarded to another host on a redirect. */
      curlx_str_trimblanks(&name);

      /* only send this if the contents was non-blank or done special */

      إذا(داده->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom
            Host: header as that will produce *two* in the same
            request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      وإلا إذا(داده->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      وإلا إذا(داده->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      وإلا إذا(داده->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      وإلا إذا(curlx_str_casecompare(&name, "Connection"))
        /* Connection headers are handled specially */
        ;
      وإلا إذا((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        /* HTTP/2 does not support chunked requests */
        ;
      وإلا إذا((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_مقصد_host(داده))
        ;
      وإلا إذا(blankheader) {
        نتیجه = curlx_dyn_addn(req, curlx_str(&name), curlx_strlen(&name));
        إذا(!نتیجه)
          نتیجه = curlx_dyn_addn(req, STRCONST(":\r\n"));
      }
      وإلا
        نتیجه = curlx_dyn_addf(req, "%s\r\n", origp);

      إذا(نتیجه)
        أرجع نتیجه;
    }
  }

  أرجع CURLE_OK;
}

#اگر_تعریف_نشده CURL_DISABLE_PARSEDATE
CURLcode Curl_جمع_کن_timecondition(بنية Curl_easy *داده,
                                بنية dynbuf *req)
{
  ثابت بنية tm *tm;
  بنية tm keeptime;
  CURLcode نتیجه;
  حرف datestr[80];
  ثابت حرف *condp;
  size_t len;

  إذا(داده->مجموعه.timecondition == CURL_TIMECOND_NONE)
    /* no condition was asked for */
    أرجع CURLE_OK;

  نتیجه = curlx_gmtime(داده->مجموعه.timevalue, &keeptime);
  إذا(نتیجه) {
    failf(داده, "Invalid TIMEVALUE");
    أرجع نتیجه;
  }
  tm = &keeptime;

  بدل(داده->مجموعه.timecondition) {
  افتراضي:
    DEBUGF(infof(داده, "invalid time condition"));
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  حالة CURL_TIMECOND_IFMODSINCE:
    condp = "If-Modified-Since";
    len = 17;
    اكسر;
  حالة CURL_TIMECOND_IFUNMODSINCE:
    condp = "If-Unmodified-Since";
    len = 19;
    اكسر;
  حالة CURL_TIMECOND_LASTMOD:
    condp = "Last-Modified";
    len = 13;
    اكسر;
  }

  إذا(Curl_checkheaders(داده, condp, len)) {
    /* A custom header was specified; it will be sent instead. */
    أرجع CURLE_OK;
  }

  /* The If-Modified-Since header family should have their times set in
   * GMT as RFC2616 defines: "All HTTP date/time stamps MUST be
   * represented in Greenwich Mean Time (GMT), without exception. For the
   * purposes of HTTP, GMT is exactly equal to UTC (Coordinated Universal
   * Time)." (see page 20 of RFC2616).
   */

  /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
  curl_msnprintf(datestr, حجم(datestr),
                 "%s: %s, %02d %s %4d %02d:%02d:%02d GMT\r\n",
                 condp,
                 Curl_wkday[tm->tm_wday ? tm->tm_wday - 1 : 6],
                 tm->tm_mday,
                 Curl_month[tm->tm_mon],
                 tm->tm_year + 1900,
                 tm->tm_hour,
                 tm->tm_min,
                 tm->tm_sec);

  نتیجه = curlx_dyn_جمع_کن(req, datestr);
  أرجع نتیجه;
}
#وإلا
/* disabled */
CURLcode Curl_جمع_کن_timecondition(بنية Curl_easy *داده,
                                بنية dynbuf *req)
{
  (فراغ)داده;
  (فراغ)req;
  أرجع CURLE_OK;
}
#پایان_اگر

فراغ Curl_http_روش(بنية Curl_easy *داده,
                      ثابت حرف **روش, Curl_HttpReq *reqp)
{
  Curl_HttpReq httpreq = (Curl_HttpReq)داده->state.httpreq;
  ثابت حرف *request;
#اگر_تعریف_نشده CURL_DISABLE_WEBSOCKETS
  إذا(داده->conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
    httpreq = HTTPREQ_GET;
  وإلا
#پایان_اگر
  إذا((داده->conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_FTP)) &&
     داده->state.upload)
    httpreq = HTTPREQ_PUT;

  /* Now set the 'request' pointer to the proper request string */
  إذا(!داده->state.http_ignorecustom &&
     CURL_EASY_STR(داده, STRING_CUSTOMREQUEST)) {
    request = CURL_EASY_STR(داده, STRING_CUSTOMREQUEST);
  }
  وإلا {
    إذا(داده->req.no_body)
      request = "HEAD";
    وإلا {
      DEBUGASSERT((httpreq >= HTTPREQ_GET) && (httpreq <= HTTPREQ_سر));
      بدل(httpreq) {
      حالة HTTPREQ_POST:
      حالة HTTPREQ_POST_FORM:
      حالة HTTPREQ_POST_MIME:
        request = "POST";
        اكسر;
      حالة HTTPREQ_PUT:
        request = "PUT";
        اكسر;
      افتراضي: /* this should never happen */
      حالة HTTPREQ_GET:
        request = "GET";
        اكسر;
      حالة HTTPREQ_سر:
        request = "HEAD";
        اكسر;
      }
    }
  }
  *روش = request;
  *reqp = httpreq;
}

ساكن CURLcode http_مجموعه_aptr_host(بنية Curl_easy *داده)
{
  بنية connectdata *conn = داده->conn;
  ثابت حرف *ptr = عدم;

  curlx_safefree(داده->state.http_host);
#اگر_تعریف_نشده CURL_DISABLE_COOKIES
  curlx_safefree(داده->req.cookiehost);
#پایان_اگر

  إذا(Curl_peer_equal(داده->state.initial_origin, داده->state.origin))
    ptr = Curl_checkheaders(داده, STRCONST("Host"));

  إذا(ptr) {
#اگر_تعریف_نشده CURL_DISABLE_COOKIES
    /* If we have a given custom Host: header, we extract the hostname in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the hostname
       is the same as the first one! */
    حرف *cookiehost;
    CURLcode نتیجه = copy_custom_مقدار(ptr, &cookiehost);
    إذا(نتیجه)
      أرجع نتیجه;
    إذا(!*cookiehost)
      /* ignore empty data */
      curlx_free(cookiehost);
    وإلا {
      /* If the host begins with '[', we start searching for the port after
         the bracket has been closed */
      إذا(*cookiehost == '[') {
        حرف *closingbracket;
        /* since the 'cookiehost' is an allocated memory area that will be
           freed later we cannot increment the pointer */
        memmove(cookiehost, cookiehost + 1, طول_رشته(cookiehost) - 1);
        closingbracket = strchr(cookiehost, ']');
        إذا(closingbracket)
          *closingbracket = 0;
      }
      وإلا {
        صحيح startsearch = 0;
        حرف *colon = strchr(cookiehost + startsearch, ':');
        إذا(colon)
          *colon = 0; /* The host must not include an embedded port number */
      }
      داده->req.cookiehost = cookiehost;
    }
#پایان_اگر

    إذا(!curl_strequal("Host:", ptr)) {
      داده->state.http_host = curl_maprintf("Host:%s", &ptr[5]);
      إذا(!داده->state.http_host)
        أرجع CURLE_بیرون_از_حافظه;
    }
  }
  وإلا {
    /* This is the  HTTP Host: header, so we want
     * - for IPv6 origins: "[ipv6-address]" where the IPv6 address is
     *  found in origin->hostname, stripped of zoneid/scopeid.
     * - the (IDN converted) origin->hostname (DNS name or IPv4) otherwise.
     * Note: zoneid/scopeid  only applies to local routing and has no
     * meaning on the remote HTTP server (eg. would confuse it). */
    bool ipv6 = (bool)داده->state.origin->ipv6;
    بنية dynbuf tmp;
    size_t hlen;
    CURLcode نتیجه;

    curlx_dyn_init(&tmp, DYN_HTTP_REQUEST);
    نتیجه = curlx_dyn_addn(&tmp, STRCONST("Host: "));
    إذا(!نتیجه && ipv6)
      نتیجه = curlx_dyn_addn(&tmp, STRCONST("["));
    إذا(!نتیجه)
      نتیجه = curlx_dyn_جمع_کن(&tmp, داده->state.origin->hostname);
    إذا(!نتیجه && ipv6)
      نتیجه = curlx_dyn_addn(&tmp, STRCONST("]"));
    إذا(!نتیجه &&
       ((داده->state.origin->پورت != داده->state.origin->scheme->defport) ||
       (داده->state.origin->scheme->family != conn->scheme->family))) {
      نتیجه = curlx_dyn_addf(&tmp, ":%u", داده->state.origin->پورت);
    }

    داده->state.http_host = نتیجه ? عدم : curlx_dyn_take(&tmp, &hlen);
    curlx_dyn_free(&tmp);
    أرجع نتیجه;
  }
  أرجع CURLE_OK;
}

/*
 * Append the request-target to the HTTP request
 */
ساكن CURLcode http_هدف(بنية Curl_easy *داده,
                            بنية dynbuf *r)
{
  CURLcode نتیجه = CURLE_OK;
  ثابت حرف *مسیر = داده->state.به_بالا.مسیر;
  ثابت حرف *query = داده->state.به_بالا.query;
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  بنية connectdata *conn = داده->conn;
#پایان_اگر

  إذا(CURL_EASY_STR(داده, STRING_هدف)) {
    مسیر = CURL_EASY_STR(داده, STRING_هدف);
    query = عدم;
  }

#اگر_تعریف_نشده CURL_DISABLE_PROXY
  إذا(conn->بیتها.origin_is_proxy) {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL, but if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the decoded hostname! */

    /* and no fragment part */
    CURLUcode uc;
    حرف *url;
    CURLU *h = curl_url_dup(داده->state.uh);
    إذا(!h)
      أرجع CURLE_بیرون_از_حافظه;

    إذا(!داده->state.origin->ipv6 &&
       (داده->state.origin->user_hostname != داده->state.origin->hostname)) {
      uc = curl_url_مجموعه(h, CURLUPART_HOST, داده->state.origin->hostname, 0);
      إذا(uc) {
        curl_url_cleanup(h);
        أرجع CURLE_بیرون_از_حافظه;
      }
    }
    uc = curl_url_مجموعه(h, CURLUPART_FRAGMENT, عدم, 0);
    إذا(uc) {
      curl_url_cleanup(h);
      أرجع CURLE_بیرون_از_حافظه;
    }

    إذا(داده->state.origin->scheme == &Curl_scheme_http) {
      /* when getting HTTP, we do not want the userinfo the URL */
      uc = curl_url_مجموعه(h, CURLUPART_USER, عدم, 0);
      إذا(uc) {
        curl_url_cleanup(h);
        أرجع CURLE_بیرون_از_حافظه;
      }
      uc = curl_url_مجموعه(h, CURLUPART_PASSWORD, عدم, 0);
      إذا(uc) {
        curl_url_cleanup(h);
        أرجع CURLE_بیرون_از_حافظه;
      }
    }
    وإلا إذا(داده->state.creds && (داده->state.creds->منبع != CREDS_URL)) {
        /* credentials not from the URL need to be set */
      uc = curl_url_مجموعه(h, CURLUPART_USER,
                        داده->state.creds->user, CURLU_URLENCODE);
      إذا(!uc)
        uc = curl_url_مجموعه(h, CURLUPART_PASSWORD,
                          داده->state.creds->passwd, CURLU_URLENCODE);
      إذا(uc) {
        curl_url_cleanup(h);
        أرجع Curl_uc_مقصد_curlcode(uc);
      }
    }

    /* Extract the URL to use in the request. */
    uc = curl_url_get(h, CURLUPART_URL, &url, CURLU_NO_DEFAULT_پورت);
    إذا(uc) {
      curl_url_cleanup(h);
      أرجع CURLE_بیرون_از_حافظه;
    }

    curl_url_cleanup(h);

    /* target or URL */
    نتیجه = curlx_dyn_جمع_کن(r, CURL_EASY_STR(داده, STRING_هدف) ?
      CURL_EASY_STR(داده, STRING_هدف) : url);
    curlx_free(url);
    إذا(نتیجه)
      أرجع نتیجه;

    إذا((داده->state.origin->scheme == &Curl_scheme_ftp) &&
       داده->مجموعه.proxy_transfer_نما) {
      /* when doing ftp, append ;type=<a|i> if not present */
      size_t len = طول_رشته(مسیر);
      bool type_present = FALSE;
      إذا((len >= 7) && !memcmp(&مسیر[len - 7], ";type=", 6)) {
        بدل(Curl_raw_toupper(مسیر[len - 1])) {
        حالة 'A':
        حالة 'D':
        حالة 'I':
          type_present = TRUE;
          اكسر;
        }
      }
      إذا(!type_present) {
        نتیجه = curlx_dyn_addf(r, ";type=%c",
                                داده->state.prefer_ascii ? 'a' : 'i');
        إذا(نتیجه)
          أرجع نتیجه;
      }
    }
  }

  وإلا
#پایان_اگر
  {
    نتیجه = curlx_dyn_جمع_کن(r, مسیر);
    إذا(نتیجه)
      أرجع نتیجه;
    إذا(query)
      نتیجه = curlx_dyn_addf(r, "?%s", query);
  }

  أرجع نتیجه;
}

#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
ساكن CURLcode مجموعه_post_reader(بنية Curl_easy *داده, Curl_HttpReq httpreq)
{
  CURLcode نتیجه;

  بدل(httpreq) {
#اگر_تعریف_نشده CURL_DISABLE_MIME
  حالة HTTPREQ_POST_MIME:
    داده->state.mimepost = داده->مجموعه.mimepostp;
    اكسر;
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_FORM_API
  حالة HTTPREQ_POST_FORM:
    /* Convert the form structure into a mime structure, then keep
       the conversion */
    إذا(!داده->state.formp) {
      داده->state.formp = curlx_calloc(1, حجم(curl_mimepart));
      إذا(!داده->state.formp)
        أرجع CURLE_بیرون_از_حافظه;
      Curl_mime_cleanpart(داده->state.formp);
      نتیجه = Curl_getformdata(داده, داده->state.formp, داده->مجموعه.httppost,
                                داده->state.fread_func);
      إذا(نتیجه) {
        curlx_safefree(داده->state.formp);
        أرجع نتیجه;
      }
      داده->state.mimepost = داده->state.formp;
    }
    اكسر;
#پایان_اگر
  افتراضي:
    داده->state.mimepost = عدم;
    اكسر;
  }

  بدل(httpreq) {
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
    /* This is form posting using mime data. */
#اگر_تعریف_نشده CURL_DISABLE_MIME
    إذا(داده->state.mimepost) {
      ثابت حرف *cthdr = Curl_checkheaders(داده, STRCONST("Content-Type"));

      /* Read and seek body only. */
      داده->state.mimepost->flags |= MIME_BODY_ONLY;

      /* Prepare the mime structure headers & set content type. */

      إذا(cthdr)
        لكل(cthdr += 13; *cthdr == ' '; cthdr++)
          ;
      وإلا إذا(داده->state.mimepost->نوع == MIMEKIND_MULTIPART)
        cthdr = "multipart/form-data";

      curl_mime_سرآیندها(داده->state.mimepost, داده->مجموعه.سرآیندها, 0);
      نتیجه = Curl_mime_prepare_سرآیندها(داده, داده->state.mimepost, cthdr,
                                         عدم, MIMESTRATEGY_FORM);
      إذا(نتیجه)
        أرجع نتیجه;
      curl_mime_سرآیندها(داده->state.mimepost, عدم, 0);
      نتیجه = Curl_creader_مجموعه_mime(داده, داده->state.mimepost);
      إذا(نتیجه)
        أرجع نتیجه;
    }
    وإلا
#پایان_اگر
    {
      نتیجه = Curl_creader_مجموعه_null(داده);
    }
    داده->state.infilesize = Curl_creader_مجموع_درازی(داده);
    أرجع نتیجه;

  افتراضي:
    أرجع Curl_creader_مجموعه_null(داده);
  }
  /* never reached */
}
#پایان_اگر

ساكن CURLcode مجموعه_reader(بنية Curl_easy *داده, Curl_HttpReq httpreq)
{
  CURLcode نتیجه = CURLE_OK;
  curl_off_t postsize = داده->state.infilesize;

  DEBUGASSERT(داده->conn);

  إذا(داده->req.authneg) {
    أرجع Curl_creader_مجموعه_null(داده);
  }

  بدل(httpreq) {
  حالة HTTPREQ_PUT: /* Let's PUT the data to the server! */
    أرجع postsize ? Curl_creader_مجموعه_fread(داده, postsize) :
      Curl_creader_مجموعه_null(داده);

#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
    أرجع مجموعه_post_reader(داده, httpreq);
#پایان_اگر

  حالة HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */
    /* the size of the post body */
    إذا(!postsize) {
      نتیجه = Curl_creader_مجموعه_null(داده);
    }
    وإلا إذا(داده->مجموعه.postfields) {
      size_t plen = curlx_sotouz_range(postsize, 0, SIZE_MAX);
      إذا(plen == SIZE_MAX)
        أرجع CURLE_بیرون_از_حافظه;
      وإلا إذا(plen)
        نتیجه = Curl_creader_مجموعه_buf(داده, داده->مجموعه.postfields, plen);
      وإلا
        نتیجه = Curl_creader_مجموعه_null(داده);
    }
    وإلا {
      /* we read the bytes from the callback. In case "chunked" encoding
       * is forced by the application, we disregard `postsize`. This is
       * a backward compatibility decision to earlier versions where
       * chunking disregarded this. See issue #13229. */
      bool chunked = FALSE;
      حرف *ptr = Curl_checkheaders(داده, STRCONST("Transfer-Encoding"));
      إذا(ptr) {
        /* Some kind of TE is requested, check if 'chunked' is chosen */
        chunked = Curl_compareheader(ptr, STRCONST("Transfer-Encoding:"),
                                     STRCONST("chunked"));
      }
      نتیجه = Curl_creader_مجموعه_fread(داده, chunked ? -1 : postsize);
    }
    أرجع نتیجه;

  افتراضي:
    /* HTTP GET/HEAD download, has no body, needs no Content-Length */
    داده->state.infilesize = 0;
    أرجع Curl_creader_مجموعه_null(داده);
  }
  /* not reached */
}

ساكن CURLcode http_resume(بنية Curl_easy *داده, Curl_HttpReq httpreq)
{
  إذا((HTTPREQ_POST == httpreq || HTTPREQ_POST_FORM == httpreq ||
      HTTPREQ_POST_MIME == httpreq || HTTPREQ_PUT == httpreq) &&
     داده->state.resume_from) {
    failf(داده, "HTTP upload cannot be resumed");
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode http_req_مجموعه_TE(بنية Curl_easy *داده,
                                بنية dynbuf *req,
                                صحيح httpversion)
{
  CURLcode نتیجه = CURLE_OK;
  ثابت حرف *ptr;

  ptr = Curl_checkheaders(داده, STRCONST("Transfer-Encoding"));
  إذا(ptr) {
    /* Some kind of TE is requested, check if 'chunked' is chosen */
    داده->req.upload_chunky =
      Curl_compareheader(ptr,
                         STRCONST("Transfer-Encoding:"), STRCONST("chunked"));
    إذا(داده->req.upload_chunky && (httpversion >= 20)) {
      infof(داده, "suppressing chunked transfer encoding on connection "
            "using HTTP version 2 or higher");
      داده->req.upload_chunky = FALSE;
    }
  }
  وإلا {
    curl_off_t req_clen = Curl_creader_مجموع_درازی(داده);

    إذا(req_clen < 0) {
      /* indeterminate request content length */
      إذا(httpversion > 10) {
        /* On HTTP/1.1, enable chunked, on HTTP/2 and later we do not
         * need it */
        داده->req.upload_chunky = (httpversion < 20);
      }
      وإلا {
        failf(داده, "Chunky upload is not supported by HTTP 1.0");
        أرجع CURLE_UPLOAD_FAILED;
      }
    }
    وإلا {
      /* else, no chunky upload */
      داده->req.upload_chunky = FALSE;
    }

    إذا(داده->req.upload_chunky)
      نتیجه = curlx_dyn_جمع_کن(req, "Transfer-Encoding: chunked\r\n");
  }
  أرجع نتیجه;
}

ساكن CURLcode addexpect(بنية Curl_easy *داده, بنية dynbuf *r,
                          صحيح httpversion, bool *announced_exp100)
{
  CURLcode نتیجه;
  حرف *ptr;

  *announced_exp100 = FALSE;
  /* Avoid Expect: 100-continue if Upgrade: is used */
  إذا(داده->req.upgr101 != UPGR101_NONE)
    أرجع CURLE_OK;

  /* For really small puts we do not use Expect: headers at all, and for
     the somewhat bigger ones we allow the app to disable it. Make
     sure that the expect100header is always set to the preferred value
     here. */
  ptr = Curl_checkheaders(داده, STRCONST("Expect"));
  إذا(ptr) {
    *announced_exp100 =
      Curl_compareheader(ptr, STRCONST("Expect:"), STRCONST("100-continue"));
  }
  وإلا إذا(!داده->state.disableexpect && (httpversion == 11)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    curl_off_t client_len = Curl_creader_client_درازی(داده);
    إذا(client_len > EXPECT_100_آستانه || client_len < 0) {
      نتیجه = curlx_dyn_addn(r, STRCONST("Expect: 100-continue\r\n"));
      إذا(نتیجه)
        أرجع نتیجه;
      *announced_exp100 = TRUE;
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode http_جمع_کن_content_hds(بنية Curl_easy *داده,
                                     بنية dynbuf *r,
                                     صحيح httpversion,
                                     Curl_HttpReq httpreq)
{
  CURLcode نتیجه = CURLE_OK;
  curl_off_t req_clen;
  bool announced_exp100 = FALSE;

  DEBUGASSERT(داده->conn);
  إذا(داده->req.upload_chunky) {
    نتیجه = Curl_httpchunk_جمع_کن_reader(داده);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  /* Get the request body length that has been set up */
  req_clen = Curl_creader_مجموع_درازی(داده);
  بدل(httpreq) {
  حالة HTTPREQ_PUT:
  حالة HTTPREQ_POST:
#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
#پایان_اگر
    /* We only set Content-Length and allow a custom Content-Length if
       we do not upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length).
       We do not override a custom "Content-Length" header, but during
       authentication negotiation that header is suppressed.
     */
    إذا(req_clen >= 0 && !داده->req.upload_chunky &&
       (داده->req.authneg ||
        !Curl_checkheaders(داده, STRCONST("Content-Length")))) {
      /* we allow replacing this header if not during auth negotiation,
         although it is not wise to actually set your own */
      نتیجه = curlx_dyn_addf(r, "Content-Length: %" FMT_OFF_T "\r\n",
                              req_clen);
    }
    إذا(نتیجه)
      اذهب بیرون;

#اگر_تعریف_نشده CURL_DISABLE_MIME
    /* Output mime-generated headers. */
    إذا(داده->state.mimepost &&
       ((httpreq == HTTPREQ_POST_FORM) || (httpreq == HTTPREQ_POST_MIME))) {
      بنية curl_slist *hdr;

      لكل(hdr = داده->state.mimepost->curlheaders; hdr; hdr = hdr->بعدی) {
        نتیجه = curlx_dyn_addf(r, "%s\r\n", hdr->داده);
        إذا(نتیجه)
          اذهب بیرون;
      }
    }
#پایان_اگر
    إذا(httpreq == HTTPREQ_POST &&
       !Curl_checkheaders(داده, STRCONST("Content-Type"))) {
      نتیجه = curlx_dyn_addn(r, STRCONST("Content-Type: application/"
                                          "x-www-form-urlencoded\r\n"));
      إذا(نتیجه)
        اذهب بیرون;
    }
    نتیجه = addexpect(داده, r, httpversion, &announced_exp100);
    إذا(نتیجه)
      اذهب بیرون;
    اكسر;
  افتراضي:
    اكسر;
  }

  Curl_pgrs_مجموعه_Upload_اندازه(داده, req_clen);
  إذا(announced_exp100)
    نتیجه = http_exp100_جمع_کن_reader(داده);

بیرون:
  أرجع نتیجه;
}

#اگر_تعریف_نشده CURL_DISABLE_COOKIES

ساكن CURLcode http_cookies(بنية Curl_easy *داده,
                             بنية dynbuf *r)
{
  CURLcode نتیجه = CURLE_OK;
  ثابت حرف *addcookies = عدم;
  bool linecap = FALSE;
  إذا(CURL_EASY_STR(داده, STRING_COOKIE) &&
     !Curl_checkheaders(داده, STRCONST("Cookie")) &&
     Curl_auth_allowed_مقصد_host(داده))
    addcookies = CURL_EASY_STR(داده, STRING_COOKIE);

  إذا(داده->cookies || addcookies) {
    بنية Curl_llist list;
    صحيح شمار = 0;

    إذا(داده->cookies && داده->state.cookie_engine) {
      bool okay;
      ثابت حرف *host = داده->req.cookiehost ?
        داده->req.cookiehost : داده->state.origin->hostname;
      Curl_share_قفل(داده, CURL_قفل_داده_COOKIE, CURL_قفل_ACCESS_SINGLE);
      نتیجه = Curl_cookie_getlist(داده, &okay, host, &list);
      إذا(!نتیجه && okay) {
        بنية Curl_llist_گره *n;
        size_t clen = 8; /* hold the size of the generated Cookie: header */

        /* loop through all cookies that matched */
        لكل(n = Curl_llist_سر(&list); n; n = Curl_گره_بعدی(n)) {
          بنية Cookie *co = Curl_گره_elem(n);
          إذا(co->مقدار) {
            size_t جمع_کن;
            إذا(!شمار) {
              نتیجه = curlx_dyn_addn(r, STRCONST("Cookie: "));
              إذا(نتیجه)
                اكسر;
            }
            جمع_کن = طول_رشته(co->name) + طول_رشته(co->مقدار) + 1;
            إذا(clen + جمع_کن >= MAX_COOKIE_سرآیند_LEN) {
              infof(داده, "Restricted outgoing cookies due to header size, "
                    "'%s' not sent", co->name);
              linecap = TRUE;
              اكسر;
            }
            نتیجه = curlx_dyn_addf(r, "%s%s=%s", شمار ? "; " : "",
                                    co->name, co->مقدار);
            إذا(نتیجه)
              اكسر;
            clen += جمع_کن + (شمار ? 2 : 0);
            شمار++;
          }
        }
        Curl_llist_destroy(&list, عدم);
      }
      Curl_share_unlock(داده, CURL_قفل_داده_COOKIE);
    }
    إذا(addcookies && !نتیجه && !linecap) {
      إذا(!شمار)
        نتیجه = curlx_dyn_addn(r, STRCONST("Cookie: "));
      إذا(!نتیجه) {
        نتیجه = curlx_dyn_addf(r, "%s%s", شمار ? "; " : "", addcookies);
        شمار++;
      }
    }
    إذا(شمار && !نتیجه)
      نتیجه = curlx_dyn_addn(r, STRCONST("\r\n"));

    إذا(نتیجه)
      أرجع نتیجه;
  }
  أرجع نتیجه;
}
#وإلا
#تعریف_کن http_cookies(a, b) CURLE_OK
#پایان_اگر

ساكن CURLcode http_range(بنية Curl_easy *داده,
                           Curl_HttpReq httpreq)
{
  إذا(داده->state.use_range) {
    /*
     * A range is selected. We use different headers whether we are downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    إذا(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_سر)) &&
       !Curl_checkheaders(داده, STRCONST("Range"))) {
      /* if a line like this was already allocated, free the previous one */
      curlx_free(داده->state.rangeline);
      داده->state.rangeline = curl_maprintf("Range: bytes=%s\r\n",
                                                 داده->state.range);
      إذا(!داده->state.rangeline)
        أرجع CURLE_بیرون_از_حافظه;
    }
    وإلا إذا((httpreq == HTTPREQ_POST || httpreq == HTTPREQ_PUT) &&
            !Curl_checkheaders(داده, STRCONST("Content-Range"))) {
      curl_off_t req_clen = Curl_creader_مجموع_درازی(داده);
      /* if a line like this was already allocated, free the previous one */
      curlx_free(داده->state.rangeline);

      إذا(داده->مجموعه.مجموعه_resume_from < 0) {
        /* Upload resume was asked for, but we do not know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        داده->state.rangeline =
          curl_maprintf("Content-Range: bytes 0-%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n", req_clen - 1, req_clen);
      }
      وإلا إذا(داده->state.resume_from) {
        /* This is because "resume" was selected */
        /* Not sure if we want to send this header during authentication
         * negotiation, but test1084 checks for it. In which case we have a
         * "null" client reader installed that gives an unexpected length. */
        curl_off_t مجموع_len = داده->req.authneg ?
                               داده->state.infilesize :
                               (داده->state.resume_from + req_clen);
        داده->state.rangeline =
          curl_maprintf("Content-Range: bytes %s%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n",
                        داده->state.range, مجموع_len - 1, مجموع_len);
      }
      وإلا {
        /* Range was selected and then we pass the incoming range and append
           total size */
        داده->state.rangeline =
          curl_maprintf("Content-Range: bytes %s/%" FMT_OFF_T "\r\n",
                        داده->state.range, req_clen);
      }
      إذا(!داده->state.rangeline)
        أرجع CURLE_بیرون_از_حافظه;
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode http_firstwrite(بنية Curl_easy *داده)
{
  بنية connectdata *conn = داده->conn;
  بنية SingleRequest *k = &داده->req;

  إذا(داده->req.newurl) {
    إذا(conn->بیتها.close) {
      /* Abort after the headers if "follow Location" is set
         and we are set to close anyway. */
      CURL_REQ_CLEAR_RECV(داده);
      k->انجامشده = TRUE;
      أرجع CURLE_OK;
    }
    /* We have a new URL to load, but since we want to be able to reuse this
       connection properly, we read the full response in "ignore more" */
    k->ignorebody = TRUE;
    infof(داده, "Ignoring the response-body");
  }
  إذا(داده->state.resume_from && !k->content_range &&
     (داده->state.httpreq == HTTPREQ_GET) &&
     !k->ignorebody) {

    إذا(k->اندازه == داده->state.resume_from) {
      /* The resume point is at the end of file, consider this fine even if it
         does not allow resume from here. */
      infof(داده, "The entire document is already downloaded");
      streamclose(conn);
      /* Abort download */
      CURL_REQ_CLEAR_RECV(داده);
      k->انجامشده = TRUE;
      أرجع CURLE_OK;
    }

    /* we wanted to resume a download, although the server does not seem to
     * support this and we did this with a GET (if it was not a GET we did a
     * POST or PUT resume) */
    failf(داده, "HTTP server does not seem to support "
          "byte ranges. Cannot resume.");
    أرجع CURLE_RANGE_ERROR;
  }

  إذا(داده->مجموعه.timecondition && !داده->state.range &&
     /* A time condition has been set AND no ranges have been requested. This
        seems to be what chapter 13.3.4 of RFC 2616 defines to be the correct
        action for an HTTP/1.1 client */
     !Curl_meets_timecondition(داده, k->timeofdoc)) {
    k->انجامشده = TRUE;
    /* We are simulating an HTTP 304 from server so we return
       what should have been returned from the server */
    داده->info.httpcode = 304;
    infof(داده, "Simulate an HTTP 304 response");
    /* we abort the transfer before it is completed == we ruin the
       reuse ability. Close the connection */
    streamclose(conn);
    أرجع CURLE_OK;
  } /* we have a time condition */

  أرجع CURLE_OK;
}

ساكن CURLcode http_check_new_conn(بنية Curl_easy *داده)
{
  بنية connectdata *conn = داده->conn;
  ثابت حرف *info_نسخه = عدم;
  ثابت حرف *alpn;
  CURLcode نتیجه;

  alpn = Curl_conn_get_alpn_negotiated(داده, conn);
  إذا(alpn && !strcmp("h3", alpn)) {
#اگر_تعریف_نشده CURL_DISABLE_PROXY
    إذا(!conn->بیتها.origin_is_proxy)
#پایان_اگر
      DEBUGASSERT(Curl_conn_http_نسخه(داده, conn) == 30);
    info_نسخه = "HTTP/3";
  }
  وإلا إذا(alpn && !strcmp("h2", alpn)) {
#اگر_تعریف_نشده CURL_DISABLE_PROXY
    إذا((Curl_conn_http_نسخه(داده, conn) != 20) &&
       conn->بیتها.origin_is_proxy) {
      نتیجه = Curl_http2_switch(داده);
      إذا(نتیجه)
        أرجع نتیجه;
    }
    وإلا
#پایان_اگر
    DEBUGASSERT(Curl_conn_http_نسخه(داده, conn) == 20);
    info_نسخه = "HTTP/2";
  }
  وإلا {
    /* Check if user wants to use HTTP/2 with clear TCP */
    إذا(Curl_http2_may_switch(داده)) {
      DEBUGF(infof(داده, "HTTP/2 over clean TCP"));
      نتیجه = Curl_http2_switch(داده);
      إذا(نتیجه)
        أرجع نتیجه;
      info_نسخه = "HTTP/2";
      /* There is no ALPN here, but the connection is now definitely h2 */
      conn->httpversion_دیده = 20;
      Curl_conn_مجموعه_multiplex(conn);
    }
    وإلا
      info_نسخه = "HTTP/1.x";
  }

  إذا(info_نسخه)
    infof(داده, "using %s", info_نسخه);
  أرجع CURLE_OK;
}

ساكن CURLcode http_جمع_کن_connection_hd(بنية Curl_easy *داده,
                                       بنية dynbuf *req)
{
  بنية curl_slist *سر;
  ثابت حرف *sep = "Connection: ";
  CURLcode نتیجه = CURLE_OK;
  size_t rlen = curlx_dyn_len(req);
  bool skip;

  /* Add the 1st custom "Connection: " header, if there is one */
  لكل(سر = داده->مجموعه.سرآیندها; سر; سر = سر->بعدی) {
    إذا(curl_strnequal(سر->داده, "Connection", 10) &&
       Curl_headersep(سر->داده[10]) &&
       !http_سرآیند_is_خالی(سر->داده)) {
      حرف *مقدار;
      نتیجه = copy_custom_مقدار(سر->داده, &مقدار);
      إذا(نتیجه)
        أرجع نتیجه;
      نتیجه = curlx_dyn_addf(req, "%s%s", sep, مقدار);
      sep = ", ";
      curlx_free(مقدار);
      اكسر; /* leave, having added 1st one */
    }
  }

  /* add our internal Connection: header values, if we have any */
  إذا(!نتیجه && داده->state.http_hd_te) {
    نتیجه = curlx_dyn_addf(req, "%s%s", sep, "TE");
    sep = ", ";
  }
  إذا(!نتیجه && داده->state.http_hd_upgrade) {
    نتیجه = curlx_dyn_addf(req, "%s%s", sep, "Upgrade");
    sep = ", ";
  }
  إذا(!نتیجه && داده->state.http_hd_h2_settings) {
    نتیجه = curlx_dyn_addf(req, "%s%s", sep, "HTTP2-Settings");
  }
  إذا(!نتیجه && (rlen < curlx_dyn_len(req)))
    نتیجه = curlx_dyn_addn(req, STRCONST("\r\n"));
  إذا(نتیجه)
    أرجع نتیجه;

  /* Add all user-defined Connection: headers after the first */
  skip = TRUE;
  لكل(سر = داده->مجموعه.سرآیندها; سر; سر = سر->بعدی) {
    إذا(curl_strnequal(سر->داده, "Connection", 10) &&
       Curl_headersep(سر->داده[10]) &&
       !http_سرآیند_is_خالی(سر->داده)) {
      إذا(skip) {
        skip = FALSE;
        استمر;
      }
      نتیجه = curlx_dyn_addf(req, "%s\r\n", سر->داده);
      إذا(نتیجه)
        أرجع نتیجه;
    }
  }

  أرجع CURLE_OK;
}

/* Header identifier in order we send them by default */
تعريفنوع تعداد {
  H1_HD_REQUEST,
  H1_HD_HOST,
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  H1_HD_PROXY_AUTH,
#پایان_اگر
  H1_HD_AUTH,
  H1_HD_RANGE,
  H1_HD_USER_AGENT,
  H1_HD_ACCEPT,
  H1_HD_TE,
  H1_HD_ACCEPT_ENCODING,
  H1_HD_REFERER,
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  H1_HD_PROXY_CONNECTION,
#پایان_اگر
  H1_HD_TRANSFER_ENCODING,
#اگر_تعریف_نشده CURL_DISABLE_ALTSVC
  H1_HD_ALT_استفاده_شده,
#پایان_اگر
  H1_HD_UPGRADE,
  H1_HD_COOKIES,
  H1_HD_CONDITIONALS,
  H1_HD_CUSTOM,
  H1_HD_CONTENT,
  H1_HD_CONNECTION,
  H1_HD_آخرین  /* the last, empty header line */
} http_hd_t;

ساكن CURLcode http_جمع_کن_hd(بنية Curl_easy *داده,
                            بنية dynbuf *req,
                            http_hd_t id,
                            بیعلامت حرف httpversion,
                            ثابت حرف *روش,
                            Curl_HttpReq httpreq)
{
  CURLcode نتیجه = CURLE_OK;
#إذا !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_PROXY) || \
  !defined(CURL_DISABLE_WEBSOCKETS)
  بنية connectdata *conn = داده->conn;
#پایان_اگر
  بدل(id) {
  حالة H1_HD_REQUEST:
    /* add the main request stuff */
    /* GET/HEAD/POST/PUT */
    نتیجه = curlx_dyn_addf(req, "%s ", روش);
    إذا(!نتیجه)
      نتیجه = http_هدف(داده, req);
    إذا(!نتیجه)
      نتیجه = curlx_dyn_addf(req, " HTTP/%s\r\n",
                              get_http_string(httpversion));
    اكسر;

  حالة H1_HD_HOST:
    إذا(داده->state.http_host) {
      نتیجه = curlx_dyn_جمع_کن(req, داده->state.http_host);
      إذا(!نتیجه)
        نتیجه = curlx_dyn_addn(req, STRCONST("\r\n"));
    }
    اكسر;

#اگر_تعریف_نشده CURL_DISABLE_PROXY
  حالة H1_HD_PROXY_AUTH:
    إذا(داده->req.hd_proxy_auth)
      نتیجه = curlx_dyn_جمع_کن(req, داده->req.hd_proxy_auth);
    اكسر;
#پایان_اگر

  حالة H1_HD_AUTH:
    إذا(داده->req.hd_auth)
      نتیجه = curlx_dyn_جمع_کن(req, داده->req.hd_auth);
    اكسر;

  حالة H1_HD_RANGE:
    إذا(داده->state.use_range && داده->state.rangeline)
      نتیجه = curlx_dyn_جمع_کن(req, داده->state.rangeline);
    اكسر;

  حالة H1_HD_USER_AGENT: {
    ثابت حرف *ua = CURL_EASY_STR(داده, STRING_USERAGENT);
    إذا(ua && *ua && !Curl_checkheaders(داده, STRCONST("User-Agent")))
      نتیجه = curlx_dyn_addf(req, "User-Agent: %s\r\n", ua);
    اكسر;
  }

  حالة H1_HD_ACCEPT:
    إذا(!Curl_checkheaders(داده, STRCONST("Accept")))
      نتیجه = curlx_dyn_جمع_کن(req, "Accept: */*\r\n");
    اكسر;

  حالة H1_HD_TE:
#اگر_تعریف_شده HAVE_LIBZ
    إذا(!Curl_checkheaders(داده, STRCONST("TE")) &&
       داده->مجموعه.http_transfer_encoding) {
      داده->state.http_hd_te = TRUE;
      نتیجه = curlx_dyn_جمع_کن(req, "TE: gzip\r\n");
    }
#پایان_اگر
    اكسر;

  حالة H1_HD_ACCEPT_ENCODING: {
    ثابت حرف *enc = CURL_EASY_STR(داده, STRING_ENCODING);
    إذا(enc && !Curl_checkheaders(داده, STRCONST("Accept-Encoding")))
      نتیجه = curlx_dyn_addf(req, "Accept-Encoding: %s\r\n", enc);
    اكسر;
  }

  حالة H1_HD_REFERER:
    إذا(Curl_bufref_ptr(&داده->state.referer) &&
       !Curl_checkheaders(داده, STRCONST("Referer")))
      نتیجه = curlx_dyn_addf(req, "Referer: %s\r\n",
                              Curl_bufref_ptr(&داده->state.referer));
    اكسر;

#اگر_تعریف_نشده CURL_DISABLE_PROXY
  حالة H1_HD_PROXY_CONNECTION:
    إذا(conn->بیتها.origin_is_proxy &&
       !Curl_checkheaders(داده, STRCONST("Proxy-Connection")) &&
       !Curl_checkProxyheaders(داده, داده->conn, STRCONST("Proxy-Connection")))
      نتیجه = curlx_dyn_جمع_کن(req, "Proxy-Connection: Keep-Alive\r\n");
    اكسر;
#پایان_اگر

  حالة H1_HD_TRANSFER_ENCODING:
    نتیجه = http_req_مجموعه_TE(داده, req, httpversion);
    اكسر;

#اگر_تعریف_نشده CURL_DISABLE_ALTSVC
  حالة H1_HD_ALT_استفاده_شده:
    إذا(conn->بیتها.altused && conn->از_طریق_peer &&
       !Curl_checkheaders(داده, STRCONST("Alt-Used")))
      نتیجه = curlx_dyn_addf(req, "Alt-Used: %s:%u\r\n",
                              conn->از_طریق_peer->hostname, conn->از_طریق_peer->پورت);
    اكسر;
#پایان_اگر

  حالة H1_HD_UPGRADE:
    إذا(!Curl_conn_is_ssl(داده->conn, FIRSTSOCKET) && (httpversion < 20) &&
       (داده->state.http_neg.wanted & CURL_HTTP_V2x) &&
       داده->state.http_neg.h2_upgrade) {
      /* append HTTP2 upgrade magic stuff to the HTTP request if it is not done
         over SSL */
      نتیجه = Curl_http2_request_upgrade(req, داده);
    }
#اگر_تعریف_نشده CURL_DISABLE_WEBSOCKETS
    إذا(!نتیجه && conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
      نتیجه = Curl_ws_request(داده, req);
#پایان_اگر
    اكسر;

  حالة H1_HD_COOKIES:
    نتیجه = http_cookies(داده, req);
    اكسر;

  حالة H1_HD_CONDITIONALS:
    نتیجه = Curl_جمع_کن_timecondition(داده, req);
    اكسر;

  حالة H1_HD_CUSTOM:
    نتیجه = Curl_جمع_کن_custom_سرآیندها(داده, FALSE, httpversion, req);
    اكسر;

  حالة H1_HD_CONTENT:
    نتیجه = http_جمع_کن_content_hds(داده, req, httpversion, httpreq);
    اكسر;

  حالة H1_HD_CONNECTION: {
    نتیجه = http_جمع_کن_connection_hd(داده, req);
    اكسر;
  }

  حالة H1_HD_آخرین:
    نتیجه = curlx_dyn_addn(req, STRCONST("\r\n"));
    اكسر;
  }
  أرجع نتیجه;
}

/*
 * Curl_http() gets called from the generic multi_do() function when an HTTP
 * request is to be performed. This creates and sends a properly constructed
 * HTTP request.
 */
CURLcode Curl_http(بنية Curl_easy *داده, bool *انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  Curl_HttpReq httpreq;
  ثابت حرف *روش;
  بنية dynbuf req;
  بیعلامت حرف httpversion;
  size_t hd_id;

  /* Always consider the DO phase done after this function call, even if there
     may be parts of the request that are not yet sent, since we can deal with
     the rest of the request in the PERFORM phase. */
  *انجامشده = TRUE;
  /* initialize a dynamic send-buffer */
  curlx_dyn_init(&req, DYN_HTTP_REQUEST);
  /* make sure the header buffer is reset - if there are leftovers from a
     previous transfer */
  curlx_dyn_reset(&داده->state.headerb);
  داده->state.maybe_folded = FALSE;

  إذا(!داده->conn->بیتها.reuse) {
    نتیجه = http_check_new_conn(داده);
    إذا(نتیجه)
      اذهب بیرون;
  }

  /* Add collecting of headers written to client. For a new connection,
   * we might have done that already, but reuse
   * or multiplex needs it here as well. */
  نتیجه = Curl_سرآیندها_init(داده);
  إذا(نتیجه)
    اذهب بیرون;

  داده->state.http_hd_te = FALSE;
  داده->state.http_hd_upgrade = FALSE;
  داده->state.http_hd_h2_settings = FALSE;

  /* what kind of request do we need to send? */
  Curl_http_روش(داده, &روش, &httpreq);

  /* select host to send */
  نتیجه = http_مجموعه_aptr_host(داده);
  /* setup the authentication headers, how that method and host are known */
  إذا(!نتیجه)
    نتیجه = Curl_http_output_auth(داده, داده->conn, روش, httpreq,
                                   داده->state.به_بالا.مسیر,
                                   داده->state.به_بالا.query, FALSE);
  /* Setup input reader, resume information and ranges */
  إذا(!نتیجه)
    نتیجه = مجموعه_reader(داده, httpreq);
  إذا(!نتیجه)
    نتیجه = http_resume(داده, httpreq);
  إذا(!نتیجه)
    نتیجه = http_range(داده, httpreq);
  إذا(نتیجه)
    اذهب بیرون;

  httpversion = http_request_نسخه(داده);
  /* Add request line and all headers to `req` */
  لكل(hd_id = 0; hd_id <= H1_HD_آخرین; ++hd_id) {
    نتیجه = http_جمع_کن_hd(داده, &req, (http_hd_t)hd_id,
                         httpversion, روش, httpreq);
    إذا(نتیجه)
      اذهب بیرون;
  }

  /* setup variables for the upcoming transfer and send */
  Curl_xfer_راهاندازی_sendrecv(داده, FIRSTSOCKET, -1);
  نتیجه = Curl_req_send(داده, &req, httpversion);

  إذا((httpversion >= 20) && داده->req.upload_chunky)
    /* upload_chunky was set above to set up the request in a chunky fashion,
       but is disabled here again to avoid that the chunked encoded version is
       actually used when sending the request body over h2 */
    داده->req.upload_chunky = FALSE;

بیرون:
  إذا(نتیجه == CURLE_TOO_LARGE)
    failf(داده, "HTTP request too large");

  curlx_dyn_free(&req);
  أرجع نتیجه;
}

تعريفنوع تعداد {
  وضعیت_UNKNOWN, /* not enough data to tell yet */
  وضعیت_انجامشده, /* a status line was read */
  وضعیت_BAD /* not a status line */
} statusline;

/* Check a string for a prefix. Check no more than 'len' bytes */
ساكن bool checkprefixmax(ثابت حرف *پیشوند, ثابت حرف *میانگیر, size_t len)
{
  size_t ch = CURLMIN(طول_رشته(پیشوند), len);
  أرجع curl_strnequal(پیشوند, میانگیر, ch);
}

/*
 * checkhttpprefix()
 *
 * Returns TRUE if member of the list matches prefix of string
 */
ساكن statusline checkhttpprefix(بنية Curl_easy *داده,
                                  ثابت حرف *s, size_t len)
{
  بنية curl_slist *سر = داده->مجموعه.http200aliases;
  statusline rc = وضعیت_BAD;
  statusline onmatch = len >= 5 ? وضعیت_انجامشده : وضعیت_UNKNOWN;

  طالما(سر) {
    إذا(checkprefixmax(سر->داده, s, len)) {
      rc = onmatch;
      اكسر;
    }
    سر = سر->بعدی;
  }

  إذا((rc != وضعیت_انجامشده) && checkprefixmax("HTTP/", s, len))
    rc = onmatch;

  أرجع rc;
}

#اگر_تعریف_نشده CURL_DISABLE_RTSP
ساكن statusline checkrtspprefix(بنية Curl_easy *داده,
                                  ثابت حرف *s, size_t len)
{
  statusline وضعیت = وضعیت_BAD;
  statusline onmatch = len >= 5 ? وضعیت_انجامشده : وضعیت_UNKNOWN;
  (فراغ)داده;
  إذا(checkprefixmax("RTSP/", s, len))
    وضعیت = onmatch;

  أرجع وضعیت;
}
#پایان_اگر /* CURL_DISABLE_RTSP */

ساكن statusline checkprotoprefix(بنية Curl_easy *داده,
                                   بنية connectdata *conn,
                                   ثابت حرف *s, size_t len)
{
#اگر_تعریف_نشده CURL_DISABLE_RTSP
  إذا(conn->scheme->protocol & CURLPROTO_RTSP)
    أرجع checkrtspprefix(داده, s, len);
#وإلا
  (فراغ)conn;
#پایان_اگر /* CURL_DISABLE_RTSP */

  أرجع checkhttpprefix(داده, s, len);
}

/* HTTP header has field name `n` (a string constant) */
#تعریف_کن HD_IS(hd, hdlen, n) \
  (((hdlen) >= (حجم(n) - 1)) && curl_strnequal(n, hd, حجم(n) - 1))

#تعریف_کن HD_VAL(hd, hdlen, n) \
  ((((hdlen) >= (حجم(n) - 1)) && (hd) && \
    curl_strnequal(n, hd, حجم(n) - 1)) ? ((hd) + (حجم(n) - 1)) : عدم)

/* HTTP header has field name `n` (a string constant) and contains `v`
 * (a string constant) in its value(s) */
#تعریف_کن HD_IS_AND_SAYS(hd, hdlen, n, v) \
  (HD_IS(hd, hdlen, n) && \
   ((hdlen) > ((حجم(n) - 1) + (حجم(v) - 1))) && \
   Curl_compareheader(hd, STRCONST(n), STRCONST(v)))

/*
 * http_header_a() parses a single response header starting with A.
 */
ساكن CURLcode http_سرآیند_a(بنية Curl_easy *داده,
                              ثابت حرف *hd, size_t hdlen)
{
#اگر_تعریف_نشده CURL_DISABLE_ALTSVC
  ثابت حرف *v;
  v = (داده->asi &&
       (Curl_xfer_is_secure(داده) ||
#اگر_تعریف_شده DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_ALTSVC_HTTP")
#وإلا
        0
#پایان_اگر
         )) ? HD_VAL(hd, hdlen, "Alt-Svc:") : عدم;
  إذا(v) {
    /* the ALPN of the current request */
    بنية SingleRequest *k = &داده->req;
    تعداد alpnid id = (k->httpversion == 30) ? ALPN_h3 :
      (k->httpversion == 20) ? ALPN_h2 : ALPN_h1;
    أرجع Curl_altsvc_parse(داده, داده->asi, v, داده->state.origin, id);
  }
#وإلا
  (فراغ)داده;
  (فراغ)hd;
  (فراغ)hdlen;
#پایان_اگر
  أرجع CURLE_OK;
}

/*
 * http_header_c() parses a single response header starting with C.
 */
ساكن CURLcode http_سرآیند_c(بنية Curl_easy *داده,
                              ثابت حرف *hd, size_t hdlen)
{
  بنية connectdata *conn = داده->conn;
  بنية SingleRequest *k = &داده->req;
  ثابت حرف *v;

  /* Check for Content-Length: header lines to get size. Browsers insist we
     should accept multiple Content-Length headers and that a comma separated
     list also is fine and then we should accept them all as long as they are
     the same value. Different values trigger error.
   */
  v = (!k->http_bodyless && !داده->مجموعه.ignorecl && !k->ignore_cl) ?
    HD_VAL(hd, hdlen, "Content-Length:") : عدم;
  إذا(v) {
    افعل {
      curl_off_t contentlength;
      صحيح offt = curlx_str_numblanks(&v, &contentlength);

      إذا(offt == STRE_سرریز) {
        /* out of range */
        إذا(داده->مجموعه.max_filesize) {
          failf(داده, "Maximum file size exceeded");
          أرجع CURLE_FILESIZE_EXCEEDED;
        }
        streamclose(conn);
        infof(داده, "Overflow Content-Length: value");
        أرجع CURLE_OK;
      }
      وإلا {
        إذا((offt == STRE_OK) &&
           ((k->اندازه == -1) || /* not set to something before */
            (k->اندازه == contentlength))) { /* or the same value */

          k->اندازه = contentlength;
          curlx_str_passblanks(&v);

          /* on a comma, loop and get the next instead */
          إذا(!curlx_str_single(&v, ','))
            استمر;

          إذا(!curlx_str_newline(&v)) {
            k->maxdownload = k->اندازه;
            أرجع CURLE_OK;
          }
        }
        /* negative, different value or rubbish - bad HTTP */
        failf(داده, "Invalid Content-Length: value");
        أرجع CURLE_WEIRD_SERVER_پاسخ;
      }
    } طالما(1);
  }
  v = (!k->http_bodyless && CURL_EASY_STR(داده, STRING_ENCODING)) ?
    HD_VAL(hd, hdlen, "Content-Encoding:") : عدم;
  إذا(v) {
    /*
     * Process Content-Encoding. Look for the values: identity, gzip, deflate,
     * compress, x-gzip and x-compress. x-gzip and x-compress are the same as
     * gzip and compress. (Sec 3.5 RFC 2616). zlib cannot handle compress.
     * Errors are handled further down when the response body is processed
     */
    أرجع Curl_build_unencoding_پشته(داده, v, FALSE);
  }
  /* check for Content-Type: header lines to get the MIME-type */
  v = HD_VAL(hd, hdlen, "Content-Type:");
  إذا(v) {
    حرف *contenttype = Curl_copy_سرآیند_مقدار(hd);
    إذا(!contenttype)
      أرجع CURLE_بیرون_از_حافظه;
    إذا(!*contenttype)
      /* ignore empty data */
      curlx_free(contenttype);
    وإلا {
      curlx_free(داده->info.contenttype);
      داده->info.contenttype = contenttype;
    }
    أرجع CURLE_OK;
  }
  إذا((k->httpversion < 20) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "close")) {
    /*
     * [RFC 2616, section 8.1.2.1]
     * "Connection: close" is HTTP/1.1 language and means that
     * the connection will close when this request has been
     * served.
     */
    connclose(conn);
    أرجع CURLE_OK;
  }
  إذا((k->httpversion == 10) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "keep-alive")) {
    /*
     * An HTTP/1.0 reply with the 'Connection: keep-alive' line
     * tells us the connection will be kept alive for our
     * pleasure. Default action for 1.0 is to close.
     *
     * [RFC2068, section 19.7.1] */
    connkeep(conn);
    infof(داده, "HTTP/1.0 connection set to keep alive");
    أرجع CURLE_OK;
  }
  v = !k->http_bodyless ? HD_VAL(hd, hdlen, "Content-Range:") : عدم;
  إذا(v) {
    /* Content-Range: bytes [num]-
       Content-Range: bytes: [num]-
       Content-Range: [num]-
       Content-Range: [asterisk]/[total]

       The second format was added since Sun's webserver
       JavaWebServer/1.1.1 obviously sends the header this way!
       The third added since some servers use that!
       The fourth means the requested range was unsatisfied.
     */

    ثابت حرف *ptr = v;

    /* Move forward until first digit or asterisk */
    طالما(*ptr && !ISDIGIT(*ptr) && *ptr != '*')
      ptr++;

    /* if it truly stopped on a digit */
    إذا(ISDIGIT(*ptr)) {
      إذا(!curlx_str_عدد(&ptr, &k->جابه_جایی, CURL_OFF_T_MAX) &&
         (داده->state.resume_from == k->جابه_جایی))
        /* we asked for a resume and we got it */
        k->content_range = TRUE;
    }
    وإلا إذا(k->httpcode < 300)
      داده->state.resume_from = 0; /* get everything */
  }
  أرجع CURLE_OK;
}

/*
 * http_header_l() parses a single response header starting with L.
 */
ساكن CURLcode http_سرآیند_l(بنية Curl_easy *داده,
                              ثابت حرف *hd, size_t hdlen)
{
  بنية connectdata *conn = داده->conn;
  بنية SingleRequest *k = &داده->req;
  ثابت حرف *v = (!k->http_bodyless &&
                   (داده->مجموعه.timecondition || داده->مجموعه.get_filetime)) ?
    HD_VAL(hd, hdlen, "Last-Modified:") : عدم;
  إذا(v) {
    إذا(Curl_getdate_capped(v, &k->timeofdoc))
      k->timeofdoc = 0;
    إذا(داده->مجموعه.get_filetime)
      داده->info.filetime = k->timeofdoc;
    أرجع CURLE_OK;
  }
  إذا(HD_IS(hd, hdlen, "Location:")) {
    /* this is the URL that the server advises us to use instead */
    حرف *location = Curl_copy_سرآیند_مقدار(hd);
    إذا(!location)
      أرجع CURLE_بیرون_از_حافظه;
    إذا(!*location ||
       (داده->req.location && !strcmp(داده->req.location, location))) {
      /* ignore empty header, or exact repeat of a previous one */
      curlx_free(location);
      أرجع CURLE_OK;
    }
    وإلا {
      /* has value and is not an exact repeat */
      إذا(داده->req.location) {
        failf(داده, "Multiple Location headers");
        curlx_free(location);
        أرجع CURLE_WEIRD_SERVER_پاسخ;
      }
      داده->req.location = location;

      إذا((k->httpcode >= 300 && k->httpcode < 400) &&
         داده->مجموعه.http_follow_نما) {
        CURLcode نتیجه;
        DEBUGASSERT(!داده->req.newurl);
        داده->req.newurl = curlx_strdup(داده->req.location); /* clone */
        إذا(!داده->req.newurl)
          أرجع CURLE_بیرون_از_حافظه;

        /* some cases of POST and PUT etc needs to rewind the data
           stream at this point */
        نتیجه = http_perhapsrewind(داده, conn);
        إذا(نتیجه)
          أرجع نتیجه;

        /* mark the next request as a followed location: */
        داده->state.this_is_a_follow = TRUE;
      }
    }
  }
  أرجع CURLE_OK;
}

/*
 * http_header_p() parses a single response header starting with P.
 */
ساكن CURLcode http_سرآیند_p(بنية Curl_easy *داده,
                              ثابت حرف *hd, size_t hdlen)
{
  بنية SingleRequest *k = &داده->req;

#اگر_تعریف_نشده CURL_DISABLE_PROXY
  ثابت حرف *v = HD_VAL(hd, hdlen, "Proxy-Connection:");
  إذا(v) {
    بنية connectdata *conn = داده->conn;
    إذا((k->httpversion == 10) && conn->http_proxy.peer &&
       HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "keep-alive")) {
      /*
       * When an HTTP/1.0 reply comes when using a proxy, the
       * 'Proxy-Connection: keep-alive' line tells us the
       * connection will be kept alive for our pleasure.
       * Default action for 1.0 is to close.
       */
      connkeep(conn); /* do not close */
      infof(داده, "HTTP/1.0 proxy connection set to keep alive");
    }
    وإلا إذا((k->httpversion == 11) && conn->http_proxy.peer &&
            HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "close")) {
      /*
       * We get an HTTP/1.1 response from a proxy and it says it will
       * close down after this transfer.
       */
      connclose(conn);
      infof(داده, "HTTP/1.1 proxy connection set close");
    }
    أرجع CURLE_OK;
  }
#پایان_اگر
  إذا((407 == k->httpcode) && HD_IS(hd, hdlen, "Proxy-authenticate:")) {
    حرف *auth;
    CURLcode نتیجه;
#اگر_تعریف_نشده CURL_DISABLE_PROXY
    إذا(!داده->conn->بیتها.origin_is_proxy)
      /* this response is not from a forward proxy, not for us */
      أرجع CURLE_OK;
#پایان_اگر
    auth = Curl_copy_سرآیند_مقدار(hd);
    نتیجه = auth ? CURLE_OK : CURLE_بیرون_از_حافظه;
    إذا(!نتیجه) {
      نتیجه = Curl_http_درون_داد_auth(داده, TRUE, auth);
      curlx_free(auth);
    }
    أرجع نتیجه;
  }
#اگر_تعریف_شده USE_SPNEGO
  إذا(HD_IS(hd, hdlen, "Persistent-Auth:")) {
    بنية connectdata *conn = داده->conn;
    بنية negotiatedata *negdata = Curl_auth_nego_get(conn, FALSE);
    بنية auth *authp = &داده->state.authhost;
    إذا(!negdata)
      أرجع CURLE_بیرون_از_حافظه;
    إذا(authp->picked == CURLAUTH_NEGOTIATE) {
      حرف *persistentauth = Curl_copy_سرآیند_مقدار(hd);
      إذا(!persistentauth)
        أرجع CURLE_بیرون_از_حافظه;
      negdata->noauthpersist = !!checkprefix("false", persistentauth);
      negdata->havenoauthpersist = TRUE;
      infof(داده, "Negotiate: noauthpersist -> %d, header part: %s",
            negdata->noauthpersist, persistentauth);
      curlx_free(persistentauth);
    }
  }
#پایان_اگر
  أرجع CURLE_OK;
}

/*
 * http_header_r() parses a single response header starting with R.
 */
ساكن CURLcode http_سرآیند_r(بنية Curl_easy *داده,
                              ثابت حرف *hd, size_t hdlen)
{
  ثابت حرف *v = HD_VAL(hd, hdlen, "Retry-After:");
  إذا(v) {
    /* Retry-After = HTTP-date / delay-seconds */
    curl_off_t retry_after = 0; /* zero for unknown or "now" */
    time_t date = 0;
    curlx_str_passblanks(&v);

    /* try it as a date first, because a date can otherwise start with and
       get treated as a number */
    إذا(!Curl_getdate_capped(v, &date)) {
      time_t current = زمان(عدم);
      إذا(date >= current)
        /* convert date to number of seconds into the future */
        retry_after = date - current;
    }
    وإلا
      /* Try it as a decimal number, ignore errors */
      (فراغ)curlx_str_عدد(&v, &retry_after, CURL_OFF_T_MAX);
    /* limit to 6 hours max. this is not documented so that it can be changed
       in the future if necessary. */
    إذا(retry_after > 21600)
      retry_after = 21600;
    داده->info.retry_after = retry_after;
  }
  أرجع CURLE_OK;
}

/*
 * http_header_s() parses a single response header starting with S.
 */
ساكن CURLcode http_سرآیند_s(بنية Curl_easy *داده,
                              ثابت حرف *hd, size_t hdlen)
{
#إذا !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_HSTS)
  ثابت حرف *v;
#وإلا
  (فراغ)داده;
  (فراغ)hd;
  (فراغ)hdlen;
#پایان_اگر

#اگر_تعریف_نشده CURL_DISABLE_COOKIES
  v = (داده->cookies && داده->state.cookie_engine) ?
    HD_VAL(hd, hdlen, "Set-Cookie:") : عدم;
  إذا(v) {
    /* If there is a custom-set Host: name, use it here, or else use
     * real peer hostname. */
    ثابت حرف *host = داده->req.cookiehost ?
      داده->req.cookiehost : داده->state.origin->hostname;
    ثابت بیعلامت حرف secure_context = Curl_secure_context(داده, host) ?
      COOKIE_SECURE : 0;
    CURLcode نتیجه;
    Curl_share_قفل(داده, CURL_قفل_داده_COOKIE, CURL_قفل_ACCESS_SINGLE);
    نتیجه = Curl_cookie_جمع_کن(داده, داده->cookies, v, host,
                             داده->state.به_بالا.مسیر,
                             COOKIE_HTTPHEADER | secure_context);
    Curl_share_unlock(داده, CURL_قفل_داده_COOKIE);
    أرجع نتیجه;
  }
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_HSTS
  /* If enabled, the header is incoming and this is over HTTPS */
  v = (داده->hsts &&
       (Curl_xfer_is_secure(داده) ||
#اگر_تعریف_شده DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_HSTS_HTTP")
#وإلا
        0
#پایان_اگر
         )
    ) ? HD_VAL(hd, hdlen, "Strict-Transport-Security:") : عدم;
  إذا(v) {
    CURLcode نتیجه = Curl_hsts_parse(
      داده->hsts, داده->state.origin->hostname, v);
    إذا(نتیجه) {
      إذا(نتیجه == CURLE_بیرون_از_حافظه)
        أرجع نتیجه;
      infof(داده, "Illegal STS header skipped");
    }
#اگر_تعریف_شده DEBUGBUILD
    وإلا
      infof(داده, "Parsed STS header fine (%zu entries)",
            Curl_llist_شمار(&داده->hsts->list));
#پایان_اگر
  }
#پایان_اگر

  أرجع CURLE_OK;
}

/*
 * http_header_t() parses a single response header starting with T.
 */
ساكن CURLcode http_سرآیند_t(بنية Curl_easy *داده,
                              ثابت حرف *hd, size_t hdlen)
{
  بنية connectdata *conn = داده->conn;
  بنية SingleRequest *k = &داده->req;

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
  ثابت حرف *v = (!k->http_bodyless &&
                   (داده->state.httpreq != HTTPREQ_سر) &&
                   (k->httpcode != 304)) ?
    HD_VAL(hd, hdlen, "Transfer-Encoding:") : عدم;
  إذا(v) {
    /* One or more encodings. We check for chunked and/or a compression
       algorithm. */
    CURLcode نتیجه = Curl_build_unencoding_پشته(داده, v, TRUE);
    إذا(نتیجه)
      أرجع نتیجه;
    إذا(!k->chunk) {
      /* if this is not chunked, only close can signal the end of this
       * transfer as Content-Length is said not to be trusted for
       * transfer-encoding! */
      CURL_TRC_M(داده, "HTTP/1.1 transfer-encoding without chunks");
      connclose(conn);
      k->ignore_cl = TRUE;
    }
    أرجع CURLE_OK;
  }
  v = HD_VAL(hd, hdlen, "Trailer:");
  إذا(v) {
    داده->req.resp_trailer = TRUE;
    أرجع CURLE_OK;
  }
  أرجع CURLE_OK;
}

/*
 * http_header_w() parses a single response header starting with W.
 */
ساكن CURLcode http_سرآیند_w(بنية Curl_easy *داده,
                              ثابت حرف *hd, size_t hdlen)
{
  بنية SingleRequest *k = &داده->req;
  CURLcode نتیجه = CURLE_OK;

  إذا((401 == k->httpcode) && HD_IS(hd, hdlen, "WWW-Authenticate:")) {
    حرف *auth = Curl_copy_سرآیند_مقدار(hd);
    إذا(!auth)
      نتیجه = CURLE_بیرون_از_حافظه;
    وإلا {
      نتیجه = Curl_http_درون_داد_auth(داده, FALSE, auth);
      curlx_free(auth);
    }
  }
  أرجع نتیجه;
}

/*
 * http_header() parses a single response header.
 */
ساكن CURLcode http_سرآیند(بنية Curl_easy *داده,
                            ثابت حرف *hd, size_t hdlen)
{
  CURLcode نتیجه = CURLE_OK;

  بدل(hd[0]) {
  حالة 'a':
  حالة 'A':
    نتیجه = http_سرآیند_a(داده, hd, hdlen);
    اكسر;
  حالة 'c':
  حالة 'C':
    نتیجه = http_سرآیند_c(داده, hd, hdlen);
    اكسر;
  حالة 'l':
  حالة 'L':
    نتیجه = http_سرآیند_l(داده, hd, hdlen);
    اكسر;
  حالة 'p':
  حالة 'P':
    نتیجه = http_سرآیند_p(داده, hd, hdlen);
    اكسر;
  حالة 'r':
  حالة 'R':
    نتیجه = http_سرآیند_r(داده, hd, hdlen);
    اكسر;
  حالة 's':
  حالة 'S':
    نتیجه = http_سرآیند_s(داده, hd, hdlen);
    اكسر;
  حالة 't':
  حالة 'T':
    نتیجه = http_سرآیند_t(داده, hd, hdlen);
    اكسر;
  حالة 'w':
  حالة 'W':
    نتیجه = http_سرآیند_w(داده, hd, hdlen);
    اكسر;
  }

  إذا(!نتیجه) {
    بنية connectdata *conn = داده->conn;
    إذا(conn->scheme->protocol & CURLPROTO_RTSP)
      نتیجه = Curl_rtsp_parseheader(داده, hd);
  }
  أرجع نتیجه;
}

/*
 * Called after the first HTTP response line (the status line) has been
 * received and parsed.
 */
ساكن CURLcode http_statusline(بنية Curl_easy *داده,
                                بنية connectdata *conn)
{
  بنية SingleRequest *k = &داده->req;

  بدل(k->httpversion) {
  حالة 10:
  حالة 11:
#اگر_تعریف_شده USE_HTTP2
  حالة 20:
#پایان_اگر
#اگر_تعریف_شده USE_HTTP3
  حالة 30:
#پایان_اگر
    /* no major version switch mid-connection */
    إذا(k->httpversion_فرستاده &&
       (k->httpversion / 10 != k->httpversion_فرستاده / 10)) {
      failf(داده, "Version mismatch (from HTTP/%d to HTTP/%d)",
            k->httpversion_فرستاده / 10, k->httpversion / 10);
      أرجع CURLE_WEIRD_SERVER_پاسخ;
    }
    اكسر;
  افتراضي:
    failf(داده, "Unsupported HTTP version (%d.%d) in response",
          k->httpversion / 10, k->httpversion % 10);
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  }

  داده->info.httpcode = k->httpcode;
  داده->info.httpversion = k->httpversion;
  conn->httpversion_دیده = k->httpversion;

  إذا(!داده->state.http_neg.rcvd_min ||
     داده->state.http_neg.rcvd_min > k->httpversion)
    /* store the lowest server version we encounter */
    داده->state.http_neg.rcvd_min = k->httpversion;

  /*
   * This code executes as part of processing the header. As a
   * result, it is not totally clear how to interpret the
   * response code yet as that depends on what other headers may
   * be present. 401 and 407 may be errors, but may be OK
   * depending on how authentication is working. Other codes
   * are definitely errors, so give up here.
   */
  إذا(داده->state.resume_from && داده->state.httpreq == HTTPREQ_GET &&
     k->httpcode == 416) {
    /* "Requested Range Not Satisfiable", proceed and pretend this is no
       error */
    k->ignorebody = TRUE; /* Avoid appending error msg to good data. */
  }

  إذا(k->httpversion == 10) {
    /* Default action for HTTP/1.0 must be to close, unless
       we get one of those fancy headers that tell us the
       server keeps it open for us! */
    infof(داده, "HTTP 1.0, assume close after body");
    connclose(conn);
  }

  k->http_bodyless = k->httpcode >= 100 && k->httpcode < 200;
  بدل(k->httpcode) {
  حالة 304:
    /* (quote from RFC2616, section 10.3.5): The 304 response
     * MUST NOT contain a message-body, and thus is always
     * terminated by the first empty line after the header
     * fields. */
    إذا(داده->مجموعه.timecondition)
      داده->info.timecond = TRUE;
    FALLTHROUGH();
  حالة 204:
    /* (quote from RFC2616, section 10.2.5): The server has
     * fulfilled the request but does not need to return an
     * entity-body ... The 204 response MUST NOT include a
     * message-body, and thus is always terminated by the first
     * empty line after the header fields. */
    k->اندازه = 0;
    k->maxdownload = 0;
    k->http_bodyless = TRUE;
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع CURLE_OK;
}

/* Content-Length must be ignored if any Transfer-Encoding is present in the
   response. Refer to RFC 7230 section 3.3.3 and RFC2616 section 4.4. This is
   figured out here after all headers have been received but before the final
   call to the user's header callback, so that a valid content length can be
   retrieved by the user in the final call. */
ساكن CURLcode http_اندازه(بنية Curl_easy *داده)
{
  بنية SingleRequest *k = &داده->req;
  إذا(داده->req.ignore_cl || k->chunk) {
    k->اندازه = k->maxdownload = -1;
  }
  وإلا إذا(k->اندازه != -1) {
    إذا(داده->مجموعه.max_filesize &&
       !k->ignorebody &&
       (k->اندازه > داده->مجموعه.max_filesize)) {
      failf(داده, "Maximum file size exceeded");
      أرجع CURLE_FILESIZE_EXCEEDED;
    }
    إذا(k->ignorebody)
      infof(داده, "setting size while ignoring");
    Curl_pgrs_مجموعه_Download_اندازه(داده, k->اندازه);
    k->maxdownload = k->اندازه;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_verify_سرآیند(بنية Curl_easy *داده,
                            ثابت حرف *hd, size_t hdlen)
{
  بنية SingleRequest *k = &داده->req;
  size_t i;
  bool has_colon = FALSE;
  لكل(i = 0; i < hdlen; i++) {
    بدل(hd[i]) {
    حالة '\0':
      /* this is bad, bail out */
      failf(داده, "NUL byte in header");
      أرجع CURLE_WEIRD_SERVER_پاسخ;
    حالة '\r':
      إذا(i < hdlen - 2) {
        /* CR may only precede the LF, nothing else */
        failf(داده, "Carriage return found in header");
        أرجع CURLE_WEIRD_SERVER_پاسخ;
      }
      اكسر;
    حالة ':':
      has_colon = TRUE;
      اكسر;
    }
  }
  إذا(k->headerline < 2)
    /* the first "header" is the status-line and it has no colon */
    أرجع CURLE_OK;
  إذا(((hd[0] == ' ') || (hd[0] == '\t')) && k->headerline > 2)
    /* line folding, cannot happen on line 2 */
    ;
  وإلا إذا(!has_colon) {
    /* this is bad, bail out */
    failf(داده, "Header without colon");
    أرجع CURLE_WEIRD_SERVER_پاسخ;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_bump_headersize(بنية Curl_easy *داده,
                              size_t delta,
                              bool connect_only)
{
  size_t bad = 0;
  بیعلامت صحيح max = MAX_HTTP_RESP_سرآیند_اندازه;
  إذا(delta < MAX_HTTP_RESP_سرآیند_اندازه) {
    داده->info.سرآیند_اندازه += (بیعلامت صحيح)delta;
    داده->req.allheadercount += (بیعلامت صحيح)delta;
    إذا(!connect_only)
      داده->req.headerbytecount += (بیعلامت صحيح)delta;
    إذا(داده->req.allheadercount > max)
      bad = داده->req.allheadercount;
    وإلا إذا(داده->info.سرآیند_اندازه > (max * 20)) {
      bad = داده->info.سرآیند_اندازه;
      max *= 20;
    }
  }
  وإلا
    bad = داده->req.allheadercount + delta;
  إذا(bad) {
    failf(داده, "Too large response headers: %zu > %u", bad, max);
    أرجع CURLE_RECV_ERROR;
  }
  أرجع CURLE_OK;
}

/*
 * Handle a 101 Switching Protocols response. Performs the actual protocol
 * upgrade to HTTP/2 or WebSocket based on what was requested.
 */
ساكن CURLcode http_on_101_upgrade(بنية Curl_easy *داده,
                                    ثابت حرف *buf, size_t blen,
                                    size_t *pconsumed,
                                    bool *conn_تغییریافته)
{
  بنية connectdata *conn = داده->conn;
  بنية SingleRequest *k = &داده->req;

#إذا !defined(USE_NGHTTP2) && defined(CURL_DISABLE_WEBSOCKETS)
  (فراغ)buf;
  (فراغ)blen;
  (فراغ)pconsumed;
#وإلا
  CURLcode نتیجه;
  صحيح upgr101_requested = k->upgr101;
#پایان_اگر

  إذا(k->httpversion_فرستاده != 11) {
    /* invalid for other HTTP versions */
    failf(داده, "server sent 101 response while not talking HTTP/1.1");
    أرجع CURLE_WEIRD_SERVER_پاسخ;
  }

  /* Whatever the success, upgrade was selected. */
  k->upgr101 = UPGR101_RECEIVED;
  conn->بیتها.upgrade_درون_پیشرفت = FALSE;
  *conn_تغییریافته = TRUE;

  /* To be fully compliant, we would check the "Upgrade:" response header to
   * mention the protocol we requested. */
#اگر_تعریف_شده USE_NGHTTP2
  إذا(upgr101_requested == UPGR101_H2) {
    /* Switch to HTTP/2, where we will get more responses. blen bytes in buf
     * are already h2 protocol bytes */
    infof(داده, "Received 101, Switching to HTTP/2");
    نتیجه = Curl_http2_upgrade(داده, conn, FIRSTSOCKET, buf, blen);
    إذا(!نتیجه)
      *pconsumed += blen;
    أرجع نتیجه;
  }
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_WEBSOCKETS
  إذا(upgr101_requested == UPGR101_WS) {
    /* Switch to WebSocket, where we now stream ws frames. blen bytes in buf
     * are already ws protocol bytes */
    infof(داده, "Received 101, Switching to WebSocket");
    نتیجه = Curl_ws_accept(داده, buf, blen);
    إذا(!نتیجه)
      *pconsumed += blen; /* ws accept handled the data */
    أرجع نتیجه;
  }
#پایان_اگر
  /* We silently accept this as the final response. What are we switching to
   * if we did not ask for an Upgrade? Maybe the application provided an
   * `Upgrade: xxx` header? */
  k->سرآیند = FALSE;
  أرجع CURLE_OK;
}

/*
 * Handle 1xx intermediate HTTP responses. Sets up state for more
 * headers and processes 100-continue and 101 upgrade responses.
 */
ساكن CURLcode http_on_1xx_response(بنية Curl_easy *داده,
                                     ثابت حرف *buf, size_t blen,
                                     size_t *pconsumed,
                                     bool *conn_تغییریافته)
{
  بنية SingleRequest *k = &داده->req;

  /* "A user agent MAY ignore unexpected 1xx status responses."
   * By default, we expect to get more responses after this one. */
  k->سرآیند = TRUE;
  k->headerline = 0; /* restart the header line counter */

  بدل(k->httpcode) {
  حالة 100:
    /* We have made an HTTP PUT or POST and this is 1.1-lingo that tells us
     * that the server is OK with this and ready to receive the data. */
    http_exp100_got100(داده);
    اكسر;
  حالة 101:
    أرجع http_on_101_upgrade(داده, buf, blen, pconsumed, conn_تغییریافته);
  افتراضي:
    /* The server may send us other 1xx responses, like informative 103. This
     * has no influence on request processing and we expect to receive a
     * final response eventually. */
    اكسر;
  }
  أرجع CURLE_OK;
}

#إذا defined(USE_NTLM) || defined(USE_SPNEGO)
/*
 * Check if NTLM or SPNEGO authentication negotiation failed due to
 * connection closure (typically on HTTP/1.0 servers).
 */
ساكن فراغ http_check_auth_بستگی(بنية Curl_easy *داده,
                                    بنية connectdata *conn)
{
  /* At this point we have some idea about the fate of the connection. If we
     are closing the connection it may result auth failure. */
#اگر_تعریف_شده USE_NTLM
  إذا(conn->بیتها.close &&
     (((داده->req.httpcode == 401) &&
       (conn->http_ntlm_state == NTLMSTATE_TYPE2)) ||
      ((داده->req.httpcode == 407) &&
       (conn->proxy_ntlm_state == NTLMSTATE_TYPE2)))) {
    infof(داده, "Connection closure while negotiating auth (HTTP 1.0?)");
    داده->state.authproblem = TRUE;
  }
#پایان_اگر
#اگر_تعریف_شده USE_SPNEGO
  إذا(conn->بیتها.close &&
    (((داده->req.httpcode == 401) &&
      (conn->http_negotiate_state == CURL_NEGO_AUTH_RECV)) ||
     ((داده->req.httpcode == 407) &&
      (conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)))) {
    infof(داده, "Connection closure while negotiating auth (HTTP 1.0?)");
    داده->state.authproblem = TRUE;
  }
  إذا((conn->http_negotiate_state == CURL_NEGO_AUTH_انجامشده) &&
     (داده->req.httpcode != 401)) {
    conn->http_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
  إذا((conn->proxy_negotiate_state == CURL_NEGO_AUTH_انجامشده) &&
     (داده->req.httpcode != 407)) {
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
#پایان_اگر
}
#وإلا
#تعریف_کن http_check_auth_بستگی(x, y) /*خالی*/
#پایان_اگر

/*
 * Handle an error response (>= 300) received while still sending the
 * request body. Deals with 417 Expectation Failed retries, keep-sending
 * on error, and aborting the send.
 */
ساكن CURLcode http_handle_send_error(بنية Curl_easy *داده)
{
  بنية connectdata *conn = داده->conn;
  بنية SingleRequest *k = &داده->req;
  CURLcode نتیجه = CURLE_OK;

  إذا(!داده->req.authneg && !conn->بیتها.close &&
     !Curl_creader_will_rewind(داده)) {
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

    بدل(داده->state.httpreq) {
    حالة HTTPREQ_PUT:
    حالة HTTPREQ_POST:
    حالة HTTPREQ_POST_FORM:
    حالة HTTPREQ_POST_MIME:
      /* We got an error response. If this happened before the
       * whole request body has been sent we stop sending and
       * mark the connection for closure after we have read the
       * entire response. */
      إذا(!Curl_req_انجامشده_sending(داده)) {
        إذا((k->httpcode == 417) && http_exp100_is_selected(داده)) {
          /* 417 Expectation Failed - try again without the
             Expect header */
          إذا(!k->writebytecount && http_exp100_is_انتظار(داده)) {
            infof(داده, "Got HTTP failure 417 while waiting for a 100");
          }
          وإلا {
            infof(داده, "Got HTTP failure 417 while sending data");
            streamclose(conn);
            نتیجه = http_perhapsrewind(داده, conn);
            إذا(نتیجه)
              أرجع نتیجه;
          }
          داده->state.disableexpect = TRUE;
          Curl_req_abort_sending(داده);
          DEBUGASSERT(!داده->req.newurl);
          داده->req.newurl = Curl_bufref_dup(&داده->state.url);
          إذا(!داده->req.newurl)
            أرجع CURLE_بیرون_از_حافظه;
        }
        وإلا إذا(داده->مجموعه.http_keep_sending_on_error) {
          infof(داده, "HTTP error before end of send, keep sending");
          http_exp100_send_anyway(داده);
        }
        وإلا {
          infof(داده, "HTTP error before end of send, stop sending");
          streamclose(conn);
          نتیجه = Curl_req_abort_sending(داده);
          إذا(نتیجه)
            أرجع نتیجه;
        }
      }
      اكسر;

    افتراضي: /* default label present to avoid compiler warnings */
      اكسر;
    }
  }

  إذا(Curl_creader_will_rewind(داده) && !Curl_req_انجامشده_sending(داده)) {
    /* We rewind before next send, continue sending now */
    infof(داده, "Keep sending data to get tossed away");
    CURL_REQ_مجموعه_SEND(داده);
  }
  أرجع نتیجه;
}

ساكن CURLcode http_on_response(بنية Curl_easy *داده,
                                 ثابت حرف *آخرین_hd, size_t آخرین_hd_len,
                                 ثابت حرف *buf, size_t blen,
                                 size_t *pconsumed)
{
  بنية connectdata *conn = داده->conn;
  CURLcode نتیجه = CURLE_OK;
  بنية SingleRequest *k = &داده->req;
  bool conn_تغییریافته = FALSE;

  (فراغ)buf; /* not used without HTTP2 enabled */
  *pconsumed = 0;

  إذا(k->upgr101 == UPGR101_RECEIVED) {
    /* supposedly upgraded to http2 now */
    إذا(داده->req.httpversion != 20)
      infof(داده, "Lying server, not serving HTTP/2");
  }

  إذا(k->httpcode < 200 && آخرین_hd) {
    /* Intermediate responses might trigger processing of more responses,
     * write the last header to the client before proceeding. */
    نتیجه = http_write_سرآیند(داده, آخرین_hd, آخرین_hd_len);
    آخرین_hd = عدم; /* handled it */
    إذا(نتیجه)
      اذهب بیرون;
  }

  إذا(k->httpcode < 100) {
    failf(داده, "Unsupported response code in HTTP response");
    نتیجه = CURLE_UNSUPPORTED_PROTOCOL;
    اذهب بیرون;
  }
  وإلا إذا(k->httpcode < 200) {
    نتیجه = http_on_1xx_response(داده, buf, blen, pconsumed, &conn_تغییریافته);
    اذهب بیرون;
  }

  /* k->httpcode >= 200, final response */
  k->سرآیند = FALSE;
  إذا(conn->بیتها.upgrade_درون_پیشرفت) {
    /* Asked for protocol upgrade, but it was not selected */
    conn->بیتها.upgrade_درون_پیشرفت = FALSE;
    conn_تغییریافته = TRUE;
  }

  إذا((k->اندازه == -1) && !k->chunk && !conn->بیتها.close &&
     (k->httpversion == 11) &&
     !(conn->scheme->protocol & CURLPROTO_RTSP) &&
     داده->state.httpreq != HTTPREQ_سر) {
    /* On HTTP 1.1, when connection is not to get closed, but no
       Content-Length nor Transfer-Encoding chunked have been received,
       according to RFC2616 section 4.4 point 5, we assume that the server
       will close the connection to signal the end of the document. */
    infof(داده, "no chunk, no close, no size. Assume close to signal end");
    streamclose(conn);
  }

  http_check_auth_بستگی(داده, conn);

#اگر_تعریف_نشده CURL_DISABLE_WEBSOCKETS
  /* All >=200 HTTP status codes are errors when wanting ws */
  إذا(داده->req.upgr101 == UPGR101_WS) {
    failf(داده, "Refused WebSocket upgrade: %d", k->httpcode);
    نتیجه = CURLE_HTTP_RETURNED_ERROR;
    اذهب بیرون;
  }
#پایان_اگر

  /* Check if this response means the transfer errored. */
  إذا(http_should_fail(داده, داده->req.httpcode)) {
    failf(داده, "The requested URL returned error: %d",
          k->httpcode);
    نتیجه = CURLE_HTTP_RETURNED_ERROR;
    اذهب بیرون;
  }

  /* Curl_http_auth_act() checks what authentication methods that are
   * available and decides which one (if any) to use. It will set 'newurl' if
   * an auth method was picked. */
  نتیجه = Curl_http_auth_act(داده);
  إذا(نتیجه)
    اذهب بیرون;

  إذا(k->httpcode >= 300) {
    نتیجه = http_handle_send_error(داده);
    إذا(نتیجه)
      اذهب بیرون;
  }

  /* final response without error, prepare to receive the body */
  نتیجه = http_firstwrite(داده);
  إذا(نتیجه)
    اذهب بیرون;

  /* This is the last response that we get for the current request. Check on
   * the body size and determine if the response is complete. */
  نتیجه = http_اندازه(داده);
  إذا(نتیجه)
    اذهب بیرون;

#اگر_تعریف_نشده CURL_DISABLE_RTSP
  /* An RTSP response to any method may carry a body, which is announced
     with Content-Length (RFC 2326 section 12.14). Read it instead of
     treating it as excess data, unless the application asked for no body
     with CURLOPT_NOBODY. */
  إذا((conn->scheme->protocol & CURLPROTO_RTSP) && داده->req.no_body &&
     !داده->مجموعه.opt_no_body && (k->اندازه > 0))
    داده->req.no_body = FALSE;
#پایان_اگر

  /* If we requested a "no body", this is a good time to get
   * out and return home.
   */
  إذا(داده->req.no_body)
    k->download_انجامشده = TRUE;

  /* If max download size is *zero* (nothing) we already have nothing and can
     safely return ok now! For HTTP/2, we would like to call
     http2_handle_stream_close to properly close a stream. In order to do
     this, we keep reading until we close the stream. */
  إذا((k->maxdownload == 0) && (k->httpversion_فرستاده < 20))
    k->download_انجامشده = TRUE;

بیرون:
  إذا(آخرین_hd)
    /* if not written yet, write it now */
    نتیجه = Curl_1st_fatal(نتیجه,
                            http_write_سرآیند(داده, آخرین_hd, آخرین_hd_len));
  إذا(conn_تغییریافته)
    /* poke the multi handle to allow pending pipewait to retry */
    Curl_multi_connchanged(داده->multi);
  أرجع نتیجه;
}

ساكن CURLcode http_rw_hd(بنية Curl_easy *داده,
                           ثابت حرف *hd, size_t hdlen,
                           ثابت حرف *buf_remain, size_t blen,
                           size_t *pconsumed)
{
  CURLcode نتیجه = CURLE_OK;
  بنية SingleRequest *k = &داده->req;
  صحيح writetype;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  *pconsumed = 0;
  إذا((0x0a == *hd) || (0x0d == *hd)) {
    /* Empty header line means end of headers! */
    بنية dynbuf آخرین_سرآیند;
    size_t consumed;

    curlx_dyn_init(&آخرین_سرآیند, hdlen + 1);
    نتیجه = curlx_dyn_addn(&آخرین_سرآیند, hd, hdlen);
    إذا(نتیجه)
      أرجع نتیجه;

    /* analyze the response to find out what to do. */
    /* Caveat: we clear anything in the header brigade, because a
     * response might switch HTTP version which may call use recursively.
     * Not nice, but that is currently the way of things. */
    curlx_dyn_reset(&داده->state.headerb);
    نتیجه = http_on_response(داده, curlx_dyn_ptr(&آخرین_سرآیند),
                              curlx_dyn_len(&آخرین_سرآیند),
                              buf_remain, blen, &consumed);
    *pconsumed += consumed;
    curlx_dyn_free(&آخرین_سرآیند);
    أرجع نتیجه;
  }

  /*
   * Checks for special headers coming up.
   */

  writetype = CLIENTWRITE_سرآیند;
  إذا(!k->headerline++) {
    /* This is the first header, it MUST be the error code line
       or else we consider this to be the body right away! */
    bool fine_statusline = FALSE;

    k->httpversion = 0; /* Do not know yet */
    إذا(داده->conn->scheme->protocol & PROTO_FAMILY_HTTP) {
      /*
       * https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.2
       *
       * The response code is always a three-digit number in HTTP as the spec
       * says. We allow any three-digit number here, but we cannot make
       * guarantees on future behaviors since it is not within the protocol.
       */
      ثابت حرف *p = hd;

      curlx_str_passblanks(&p);
      إذا(!مقایسه_رشته(p, "HTTP/", 5)) {
        p += 5;
        بدل(*p) {
        حالة '1':
          p++;
          إذا((p[0] == '.') && (p[1] == '0' || p[1] == '1')) {
            إذا(ISBLANK(p[2])) {
              k->httpversion = (بیعلامت حرف)(10 + (p[1] - '0'));
              p += 3;
              إذا(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
                k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
                  (p[2] - '0');
                /* RFC 9112 requires a single space following the status code,
                   but the browsers do not so let's not insist */
                fine_statusline = TRUE;
              }
            }
          }
          إذا(!fine_statusline) {
            failf(داده, "Unsupported HTTP/1 subversion in response");
            أرجع CURLE_UNSUPPORTED_PROTOCOL;
          }
          اكسر;
        حالة '2':
        حالة '3':
          إذا(!ISBLANK(p[1]))
            اكسر;
          k->httpversion = (بیعلامت حرف)((*p - '0') * 10);
          p += 2;
          إذا(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
            k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
              (p[2] - '0');
            p += 3;
            إذا(!ISBLANK(*p))
              اكسر;
            fine_statusline = TRUE;
          }
          اكسر;
        افتراضي: /* unsupported */
          failf(داده, "Unsupported HTTP version in response");
          أرجع CURLE_UNSUPPORTED_PROTOCOL;
        }
      }

      إذا(!fine_statusline) {
        /* If user has set option HTTP200ALIASES,
           compare header line against list of aliases */
        statusline check = checkhttpprefix(داده, hd, hdlen);
        إذا(check == وضعیت_انجامشده) {
          fine_statusline = TRUE;
          k->httpcode = 200;
          k->httpversion = 10;
        }
      }
    }
    وإلا إذا(داده->conn->scheme->protocol & CURLPROTO_RTSP) {
      ثابت حرف *p = hd;
      بنية Curl_str ver;
      curl_off_t وضعیت;
      /* we set the max string a little excessive to forgive some leading
         spaces */
      إذا(!curlx_str_until(&p, &ver, 32, ' ') &&
         !curlx_str_single(&p, ' ') &&
         !curlx_str_عدد(&p, &وضعیت, 999)) {
        curlx_str_trimblanks(&ver);
        إذا(curlx_str_cmp(&ver, "RTSP/1.0")) {
          k->httpcode = (صحيح)وضعیت;
          fine_statusline = TRUE;
          k->httpversion = 11; /* RTSP acts like HTTP 1.1 */
        }
      }
      إذا(!fine_statusline)
        أرجع CURLE_WEIRD_SERVER_پاسخ;
    }

    إذا(fine_statusline) {
      نتیجه = http_statusline(داده, داده->conn);
      إذا(نتیجه)
        أرجع نتیجه;
      writetype |= CLIENTWRITE_وضعیت;
    }
    وإلا {
      k->سرآیند = FALSE;   /* this is not a header line */
      أرجع CURLE_WEIRD_SERVER_پاسخ;
    }
  }

  نتیجه = Curl_verify_سرآیند(داده, hd, hdlen);
  إذا(نتیجه)
    أرجع نتیجه;

  نتیجه = http_سرآیند(داده, hd, hdlen);
  إذا(نتیجه)
    أرجع نتیجه;

  /*
   * Taken in one (more) header. Write it to the client.
   */
  Curl_debug(داده, CURLINFO_سرآیند_درون, hd, hdlen);

  إذا(k->httpcode / 100 == 1)
    writetype |= CLIENTWRITE_1XX;
  نتیجه = Curl_client_write(داده, writetype, hd, hdlen);
  إذا(نتیجه)
    أرجع نتیجه;

  نتیجه = Curl_bump_headersize(داده, hdlen, FALSE);
  إذا(نتیجه)
    أرجع نتیجه;

  أرجع CURLE_OK;
}

/* remove trailing CRLF then all trailing whitespace */
فراغ Curl_http_مقصد_fold(بنية dynbuf *bf)
{
  size_t len = curlx_dyn_len(bf);
  ثابت حرف *hd = curlx_dyn_ptr(bf);
  إذا(len && (hd[len - 1] == '\n'))
    len--;
  إذا(len && (hd[len - 1] == '\r'))
    len--;
  طالما(len && ISBLANK(hd[len - 1])) /* strip off trailing whitespace */
    len--;
  curlx_dyn_setlen(bf, len);
}

ساكن فراغ unfold_سرآیند(بنية Curl_easy *داده)
{
  Curl_http_مقصد_fold(&داده->state.headerb);
  داده->state.leading_unfold = TRUE;
}

/*
 * Read any HTTP header lines from the server and pass them to the client app.
 */
ساكن CURLcode http_parse_سرآیندها(بنية Curl_easy *داده,
                                   ثابت حرف *buf, size_t blen,
                                   size_t *pconsumed)
{
  بنية connectdata *conn = داده->conn;
  CURLcode نتیجه = CURLE_OK;
  بنية SingleRequest *k = &داده->req;
  ثابت حرف *انتها_ptr;
  bool leftover_body = FALSE;

  /* we have bytes for the next header, make sure it is not a folded header
     before passing it on */
  إذا(داده->state.maybe_folded && blen) {
    إذا(ISBLANK(buf[0])) {
      /* folded, remove the trailing newlines and append the next header */
      unfold_سرآیند(داده);
    }
    وإلا {
      /* the header data we hold is a complete header, pass it on */
      size_t ignore_this;
      نتیجه = http_rw_hd(داده, curlx_dyn_ptr(&داده->state.headerb),
                          curlx_dyn_len(&داده->state.headerb),
                          عدم, 0, &ignore_this);
      curlx_dyn_reset(&داده->state.headerb);
      إذا(نتیجه)
        أرجع نتیجه;
    }
    داده->state.maybe_folded = FALSE;
  }

  /* header line within buffer loop */
  *pconsumed = 0;
  طالما(blen && k->سرآیند) {
    size_t consumed;
    size_t hlen;
    ثابت حرف *hd;
    size_t unfold_len = 0;

    إذا(داده->state.leading_unfold) {
      /* immediately after an unfold, keep only a single whitespace */
      طالما(blen && ISBLANK(buf[0])) {
        buf++;
        blen--;
        unfold_len++;
      }
      إذا(blen) {
        /* insert a single space */
        نتیجه = curlx_dyn_addn(&داده->state.headerb, " ", 1);
        إذا(نتیجه)
          أرجع نتیجه;
        داده->state.leading_unfold = FALSE; /* done now */
      }
    }

    انتها_ptr = memchr(buf, '\n', blen);
    إذا(!انتها_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      نتیجه = curlx_dyn_addn(&داده->state.headerb, buf, blen);
      إذا(نتیجه)
        أرجع نتیجه;
      *pconsumed += blen + unfold_len;

      إذا(!k->headerline) {
        /* check if this looks like a protocol header */
        statusline st =
          checkprotoprefix(داده, conn,
                           curlx_dyn_ptr(&داده->state.headerb),
                           curlx_dyn_len(&داده->state.headerb));

        إذا(st == وضعیت_BAD) {
          /* this is not the beginning of a protocol first header line.
           * Cannot be 0.9 if version was detected or connection was reused. */
          k->سرآیند = FALSE;
          streamclose(conn);
          إذا((k->httpversion >= 10) || conn->بیتها.reuse) {
            failf(داده, "Invalid status line");
            أرجع CURLE_WEIRD_SERVER_پاسخ;
          }
          إذا(!داده->state.http_neg.accept_09) {
            failf(داده, "Received HTTP/0.9 when not allowed");
            أرجع CURLE_UNSUPPORTED_PROTOCOL;
          }
          leftover_body = TRUE;
          اذهب بیرون;
        }
      }
      اذهب بیرون; /* read more and try again */
    }

    /* the size of the remaining header line */
    consumed = (انتها_ptr - buf) + 1;

    نتیجه = curlx_dyn_addn(&داده->state.headerb, buf, consumed);
    إذا(نتیجه)
      أرجع نتیجه;
    blen -= consumed;
    buf += consumed;
    *pconsumed += consumed + unfold_len;

    /****
     * We now have a FULL header line in 'headerb'.
     *****/

    hlen = curlx_dyn_len(&داده->state.headerb);
    hd = curlx_dyn_ptr(&داده->state.headerb);

    إذا(!k->headerline) {
      /* the first read "header", the status line */
      statusline st = checkprotoprefix(داده, conn, hd, hlen);
      إذا(st == وضعیت_BAD) {
        streamclose(conn);
        /* this is not the beginning of a protocol first header line.
         * Cannot be 0.9 if version was detected or connection was reused. */
        إذا((k->httpversion >= 10) || conn->بیتها.reuse) {
          failf(داده, "Invalid status line");
          أرجع CURLE_WEIRD_SERVER_پاسخ;
        }
        إذا(!داده->state.http_neg.accept_09) {
          failf(داده, "Received HTTP/0.9 when not allowed");
          أرجع CURLE_UNSUPPORTED_PROTOCOL;
        }
        k->سرآیند = FALSE;
        leftover_body = TRUE;
        اذهب بیرون;
      }
    }
    وإلا {
      إذا(hlen && !ISNEWLINE(hd[0])) {
        /* this is NOT the header separator */

        /* if we have bytes for the next header, check for folding */
        إذا(blen && ISBLANK(buf[0])) {
          /* remove the trailing CRLF and append the next header */
          unfold_سرآیند(داده);
          استمر;
        }
        وإلا إذا(!blen) {
          /* this might be a folded header so deal with it in next invoke */
          داده->state.maybe_folded = TRUE;
          اكسر;
        }
      }
    }

    نتیجه = http_rw_hd(داده, hd, hlen, buf, blen, &consumed);
    /* We are done with this line. We reset because response
     * processing might switch to HTTP/2 and that might call us
     * directly again. */
    curlx_dyn_reset(&داده->state.headerb);
    إذا(consumed) {
      blen -= consumed;
      buf += consumed;
      *pconsumed += consumed;
    }
    إذا(نتیجه)
      أرجع نتیجه;
  }

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */
بیرون:
  إذا(!k->سرآیند && !leftover_body) {
    curlx_dyn_free(&داده->state.headerb);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_http_write_resp_hd(بنية Curl_easy *داده,
                                 ثابت حرف *hd, size_t hdlen,
                                 bool is_eos)
{
  CURLcode نتیجه;
  size_t consumed;
  حرف tmp = 0;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  نتیجه = http_rw_hd(داده, hd, hdlen, &tmp, 0, &consumed);
  إذا(!نتیجه && is_eos) {
    نتیجه = Curl_client_write(داده, (CLIENTWRITE_BODY | CLIENTWRITE_EOS),
                               &tmp, 0);
  }
  أرجع نتیجه;
}

/*
 * HTTP protocol `write_resp` implementation. Parse headers
 * when not done yet and otherwise return without consuming data.
 */
CURLcode Curl_http_write_resp_hds(بنية Curl_easy *داده,
                                  ثابت حرف *buf, size_t blen,
                                  size_t *pconsumed)
{
  إذا(!داده->req.سرآیند) {
    *pconsumed = 0;
    أرجع CURLE_OK;
  }
  وإلا {
    CURLcode نتیجه;

    نتیجه = http_parse_سرآیندها(داده, buf, blen, pconsumed);
    إذا(!نتیجه && !داده->req.سرآیند) {
      إذا(!داده->req.no_body && curlx_dyn_len(&داده->state.headerb)) {
        /* leftover from parsing something that turned out not
         * to be a header, only happens if we allow for
         * HTTP/0.9 like responses */
        نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY,
                                   curlx_dyn_ptr(&داده->state.headerb),
                                   curlx_dyn_len(&داده->state.headerb));
      }
      curlx_dyn_free(&داده->state.headerb);
    }
    أرجع نتیجه;
  }
}

CURLcode Curl_http_write_resp(بنية Curl_easy *داده,
                              ثابت حرف *buf, size_t blen,
                              bool is_eos)
{
  CURLcode نتیجه;
  size_t consumed;
  صحيح flags;

  نتیجه = Curl_http_write_resp_hds(داده, buf, blen, &consumed);
  إذا(نتیجه || داده->req.انجامشده)
    اذهب بیرون;

  DEBUGASSERT(consumed <= blen);
  blen -= consumed;
  buf += consumed;
  /* either all was consumed in header parsing, or we have data left
   * and are done with headers, e.g. it is BODY data */
  DEBUGASSERT(!blen || !داده->req.سرآیند);
  إذا(!داده->req.سرآیند && (blen || is_eos)) {
    /* BODY data after header been parsed, write and consume */
    flags = CLIENTWRITE_BODY;
    إذا(is_eos)
      flags |= CLIENTWRITE_EOS;
    نتیجه = Curl_client_write(داده, flags, buf, blen);
  }
بیرون:
  أرجع نتیجه;
}

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_وضعیت(int32_t *pstatus, ثابت حرف *s, size_t len)
{
  CURLcode نتیجه = CURLE_BAD_FUNCTION_آرگومان;
  int32_t وضعیت = 0;
  صحيح i;

  إذا(len != 3)
    اذهب بیرون;

  لكل(i = 0; i < 3; ++i) {
    حرف c = s[i];

    إذا(c < '0' || c > '9')
      اذهب بیرون;

    وضعیت *= 10;
    وضعیت += c - '0';
  }
  نتیجه = CURLE_OK;
بیرون:
  *pstatus = نتیجه ? -1 : وضعیت;
  أرجع نتیجه;
}

CURLcode Curl_http_req_make(بنية httpreq **preq,
                            ثابت حرف *روش, size_t m_len,
                            ثابت حرف *scheme, size_t s_len,
                            ثابت حرف *authority, size_t a_len,
                            ثابت حرف *مسیر, size_t p_len)
{
  بنية httpreq *req;
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;

  DEBUGASSERT(روش && m_len);

  req = curlx_calloc(1, حجم(*req) + m_len);
  إذا(!req)
    اذهب بیرون;
#إذا defined(__GNUC__) && __GNUC__ >= 13
#رهنمود GCC diagnostic افزودن
/* error: 'memcpy' offset [137, 142] from the object at 'req' is out of
   the bounds of referenced subobject 'method' with type 'char[1]' at
   offset 136 */
#رهنمود GCC diagnostic ignored "-Warray-bounds"
#پایان_اگر
  کپی_حافظه(req->روش, روش, m_len);
#إذا defined(__GNUC__) && __GNUC__ >= 13
#رهنمود GCC diagnostic برداشت
#پایان_اگر
  إذا(scheme) {
    req->scheme = curlx_memdup0(scheme, s_len);
    إذا(!req->scheme)
      اذهب بیرون;
  }
  إذا(authority) {
    req->authority = curlx_memdup0(authority, a_len);
    إذا(!req->authority)
      اذهب بیرون;
  }
  إذا(مسیر) {
    req->مسیر = curlx_memdup0(مسیر, p_len);
    إذا(!req->مسیر)
      اذهب بیرون;
  }
  Curl_dynhds_init(&req->سرآیندها, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  نتیجه = CURLE_OK;

بیرون:
  إذا(نتیجه && req)
    Curl_http_req_free(req);
  *preq = نتیجه ? عدم : req;
  أرجع نتیجه;
}

ساكن CURLcode req_تخصیص_بده_url_authority(بنية httpreq *req, CURLU *url)
{
  حرف *host, *پورت;
  بنية dynbuf buf;
  CURLUcode uc;
  CURLcode نتیجه = CURLE_URL_MALFORMAT;

  host = پورت = عدم;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_HOST, &host, 0);
  إذا(uc && uc != CURLUE_NO_HOST)
    اذهب بیرون;
  إذا(!host) {
    req->authority = عدم;
    نتیجه = CURLE_OK;
    اذهب بیرون;
  }

  uc = curl_url_get(url, CURLUPART_پورت, &پورت, CURLU_NO_DEFAULT_پورت);
  إذا(uc && uc != CURLUE_NO_پورت)
    اذهب بیرون;

  نتیجه = curlx_dyn_جمع_کن(&buf, host);
  إذا(نتیجه)
    اذهب بیرون;
  إذا(پورت) {
    نتیجه = curlx_dyn_addf(&buf, ":%s", پورت);
    إذا(نتیجه)
      اذهب بیرون;
  }
  req->authority = curlx_dyn_ptr(&buf);
بیرون:
  curlx_free(host);
  curlx_free(پورت);
  إذا(نتیجه)
    curlx_dyn_free(&buf);
  أرجع نتیجه;
}

ساكن CURLcode req_تخصیص_بده_url_مسیر(بنية httpreq *req, CURLU *url)
{
  حرف *مسیر, *query;
  بنية dynbuf buf;
  CURLUcode uc;
  CURLcode نتیجه = CURLE_URL_MALFORMAT;

  مسیر = query = عدم;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_مسیر, &مسیر, 0);
  إذا(uc)
    اذهب بیرون;
  uc = curl_url_get(url, CURLUPART_QUERY, &query, 0);
  إذا(uc && uc != CURLUE_NO_QUERY)
    اذهب بیرون;

  إذا(!query) {
    req->مسیر = مسیر;
    مسیر = عدم;
  }
  وإلا {
    نتیجه = curlx_dyn_جمع_کن(&buf, مسیر);
    إذا(!نتیجه)
      نتیجه = curlx_dyn_addf(&buf, "?%s", query);
    إذا(نتیجه)
      اذهب بیرون;
    req->مسیر = curlx_dyn_ptr(&buf);
  }
  نتیجه = CURLE_OK;

بیرون:
  curlx_free(مسیر);
  curlx_free(query);
  إذا(نتیجه)
    curlx_dyn_free(&buf);
  أرجع نتیجه;
}

CURLcode Curl_http_req_make2(بنية httpreq **preq,
                             ثابت حرف *روش, size_t m_len,
                             CURLU *url, ثابت حرف *scheme_default)
{
  بنية httpreq *req;
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;
  CURLUcode uc;

  DEBUGASSERT(روش && m_len);

  req = curlx_calloc(1, حجم(*req) + m_len);
  إذا(!req)
    اذهب بیرون;
  کپی_حافظه(req->روش, روش, m_len);

  uc = curl_url_get(url, CURLUPART_SCHEME, &req->scheme, 0);
  إذا(uc && uc != CURLUE_NO_SCHEME)
    اذهب بیرون;
  إذا(!req->scheme && scheme_default) {
    req->scheme = curlx_strdup(scheme_default);
    إذا(!req->scheme)
      اذهب بیرون;
  }

  نتیجه = req_تخصیص_بده_url_authority(req, url);
  إذا(نتیجه)
    اذهب بیرون;
  نتیجه = req_تخصیص_بده_url_مسیر(req, url);
  إذا(نتیجه)
    اذهب بیرون;

  Curl_dynhds_init(&req->سرآیندها, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  نتیجه = CURLE_OK;

بیرون:
  إذا(نتیجه && req)
    Curl_http_req_free(req);
  *preq = نتیجه ? عدم : req;
  أرجع نتیجه;
}

فراغ Curl_http_req_free(بنية httpreq *req)
{
  إذا(req) {
    curlx_free(req->scheme);
    curlx_free(req->authority);
    curlx_free(req->مسیر);
    Curl_dynhds_free(&req->سرآیندها);
    Curl_dynhds_free(&req->trailers);
    curlx_free(req);
  }
}

بنية name_const {
  ثابت حرف *name;
  size_t namelen;
};

ساكن ثابت بنية name_const H2_NON_فیلد[] = {
  { STRCONST("Host") },
  { STRCONST("Upgrade") },
  { STRCONST("Connection") },
  { STRCONST("Keep-Alive") },
  { STRCONST("Proxy-Connection") },
  { STRCONST("Transfer-Encoding") },
};

ساكن bool h2_permissible_فیلد(بنية dynhds_مدخل *e)
{
  size_t i;
  لكل(i = 0; i < CURL_ARRAYSIZE(H2_NON_فیلد); ++i) {
    إذا(e->namelen == H2_NON_فیلد[i].namelen &&
       curl_strnequal(H2_NON_فیلد[i].name, e->name, e->namelen))
      أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن bool http_TE_has_token(ثابت حرف *fvalue, ثابت حرف *token)
{
  طالما(*fvalue) {
    بنية Curl_str name;

    /* skip to first token */
    طالما(ISBLANK(*fvalue) || *fvalue == ',')
      fvalue++;
    إذا(curlx_str_cspn(&fvalue, &name, " \t\r;,"))
      أرجع FALSE;
    إذا(curlx_str_casecompare(&name, token))
      أرجع TRUE;

    /* skip any remainder after token, e.g. parameters with quoted strings */
    طالما(*fvalue && *fvalue != ',') {
      إذا(*fvalue == '"') {
        بنية Curl_str qw;
        /* if we do not cleanly find a quoted word here, the header value
         * does not follow HTTP syntax and we reject */
        إذا(curlx_str_quotedword(&fvalue, &qw, CURL_MAX_HTTP_سرآیند))
          أرجع FALSE;
      }
      وإلا
        fvalue++;
    }
  }
  أرجع FALSE;
}

CURLcode Curl_http_req_مقصد_h2(بنية dynhds *h2_سرآیندها,
                             بنية httpreq *req, بنية Curl_easy *داده)
{
  ثابت حرف *scheme = عدم, *authority = عدم;
  بنية dynhds_مدخل *e;
  size_t i;
  CURLcode نتیجه;

  DEBUGASSERT(req);
  DEBUGASSERT(h2_سرآیندها);

  إذا(req->scheme) {
    scheme = req->scheme;
  }
  وإلا إذا(strcmp("CONNECT", req->روش)) {
    scheme = Curl_checkheaders(داده, STRCONST(HTTP_PSEUDO_SCHEME));
    إذا(scheme) {
      scheme += حجم(HTTP_PSEUDO_SCHEME);
      curlx_str_passblanks(&scheme);
      infof(داده, "set pseudo header %s to %s", HTTP_PSEUDO_SCHEME, scheme);
    }
    وإلا {
      scheme = داده->state.origin->scheme->name;
    }
  }

  إذا(req->authority) {
    authority = req->authority;
  }
  وإلا {
    e = Curl_dynhds_get(&req->سرآیندها, STRCONST("Host"));
    إذا(e)
      authority = e->مقدار;
  }

  Curl_dynhds_reset(h2_سرآیندها);
  Curl_dynhds_مجموعه_opts(h2_سرآیندها, DYNHDS_OPT_LOWERCASE);
  نتیجه = Curl_dynhds_جمع_کن(h2_سرآیندها, STRCONST(HTTP_PSEUDO_روش),
                           req->روش, طول_رشته(req->روش));
  إذا(!نتیجه && scheme) {
    نتیجه = Curl_dynhds_جمع_کن(h2_سرآیندها, STRCONST(HTTP_PSEUDO_SCHEME),
                             scheme, طول_رشته(scheme));
  }
  إذا(!نتیجه && authority) {
    نتیجه = Curl_dynhds_جمع_کن(h2_سرآیندها, STRCONST(HTTP_PSEUDO_AUTHORITY),
                             authority, طول_رشته(authority));
  }
  إذا(!نتیجه && req->مسیر) {
    نتیجه = Curl_dynhds_جمع_کن(h2_سرآیندها, STRCONST(HTTP_PSEUDO_مسیر),
                             req->مسیر, طول_رشته(req->مسیر));
  }
  لكل(i = 0; !نتیجه && i < Curl_dynhds_شمار(&req->سرآیندها); ++i) {
    e = Curl_dynhds_getn(&req->سرآیندها, i);
    /* "TE" is special in that it is only permissible when it
     * has only value "trailers". RFC 9113 ch. 8.2.2 */
    إذا(e->namelen == 2 && curl_strequal("TE", e->name)) {
      إذا(http_TE_has_token(e->مقدار, "trailers"))
        نتیجه = Curl_dynhds_جمع_کن(h2_سرآیندها, e->name, e->namelen,
                                 "trailers", CURL_CSTRLEN("trailers"));
    }
    وإلا إذا(h2_permissible_فیلد(e)) {
      نتیجه = Curl_dynhds_جمع_کن(h2_سرآیندها, e->name, e->namelen,
                               e->مقدار, e->valuelen);
    }
  }

  أرجع نتیجه;
}

CURLcode Curl_http_resp_make(بنية http_resp **presp,
                             صحيح وضعیت,
                             ثابت حرف *description)
{
  بنية http_resp *resp;
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;

  resp = curlx_calloc(1, حجم(*resp));
  إذا(!resp)
    اذهب بیرون;

  resp->وضعیت = وضعیت;
  إذا(description) {
    resp->description = curlx_strdup(description);
    إذا(!resp->description)
      اذهب بیرون;
  }
  Curl_dynhds_init(&resp->سرآیندها, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&resp->trailers, 0, DYN_HTTP_REQUEST);
  نتیجه = CURLE_OK;

بیرون:
  إذا(نتیجه && resp)
    Curl_http_resp_free(resp);
  *presp = نتیجه ? عدم : resp;
  أرجع نتیجه;
}

فراغ Curl_http_resp_free(بنية http_resp *resp)
{
  إذا(resp) {
    curlx_free(resp->description);
    Curl_dynhds_free(&resp->سرآیندها);
    Curl_dynhds_free(&resp->trailers);
    إذا(resp->prev)
      Curl_http_resp_free(resp->prev);
    curlx_free(resp);
  }
}

/*
 * HTTP handler interface.
 */
ثابت بنية Curl_protocol Curl_protocol_http = {
  Curl_http_راهاندازی_conn,                 /* setup_connection */
  Curl_http,                            /* do_it */
  Curl_http_انجامشده,                       /* done */
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

#پایان_اگر /* CURL_DISABLE_HTTP */
