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

#جے_بنا_تعریف CURL_DISABLE_HTTP

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت

#جے_تعریف_شدہ HAVE_NETDB_H
#ضمن <netdb.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NET_IF_H
#ضمن <net/if.h>
#جے_انت
#جے_تعریف_شدہ HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#جے_انت

#جے_تعریف_شدہ HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#جے_انت

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

فراغ Curl_http_neg_init(بنية Curl_easy *انکڑے, بنية http_negotiation *neg)
{
  memset(neg, 0, حجم(*neg));
  neg->accept_09 = انکڑے->سیٹ.http09_allowed;
  بدل(انکڑے->سیٹ.httpwant) {
  حالة CURL_HTTP_نسخہ_1_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    neg->only_10 = TRUE;
    اكسر;
  حالة CURL_HTTP_نسخہ_1_1:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    اكسر;
  حالة CURL_HTTP_نسخہ_2_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->h2_upgrade = TRUE;
    اكسر;
  حالة CURL_HTTP_نسخہ_2_TLS:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    اكسر;
  حالة CURL_HTTP_نسخہ_2_پہلوں_KNOWLEDGE:
    neg->wanted = neg->allowed = (CURL_HTTP_V2x);
    انکڑے->state.http_neg.h2_پہلوں_knowledge = TRUE;
    اكسر;
  حالة CURL_HTTP_نسخہ_3:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    neg->allowed = neg->wanted;
    اكسر;
  حالة CURL_HTTP_نسخہ_3_ONLY:
    neg->wanted = neg->allowed = (CURL_HTTP_V3x);
    اكسر;
  حالة CURL_HTTP_نسخہ_NONE:
  افتراضي:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    اكسر;
  }
}

CURLcode Curl_http_تیاری_conn(بنية Curl_easy *انکڑے,
                              بنية connectdata *conn)
{
  /* allocate the HTTP-specific struct for the Curl_easy, only to survive
     during this request */
  إذا(انکڑے->state.http_neg.wanted == CURL_HTTP_V3x) {
    /* only HTTP/3, needs to work */
    CURLcode نتیجہ = Curl_conn_may_http3(انکڑے, conn);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }
  أرجع CURLE_OK;
}

#جے_بنا_تعریف CURL_DISABLE_PROXY
/*
 * checkProxyHeaders() checks the linked list of custom proxy headers
 * if proxy headers are not available, then it will lookup into http header
 * link list
 *
 * It takes a connectdata struct as input to see if this is a proxy request or
 * not, as it then might check a different header list. Provide the header
 * prefix without colon!
 */
حرف *Curl_checkProxyheaders(بنية Curl_easy *انکڑے,
                             ثابت بنية connectdata *conn,
                             ثابت حرف *thisheader,
                             ثابت size_t thislen)
{
  بنية curl_slist *سر;

  لكل(سر = (conn->http_proxy.peer && انکڑے->سیٹ.sep_سرخیاں) ?
        انکڑے->سیٹ.proxyheaders : انکڑے->سیٹ.سرخیاں;
      سر; سر = سر->اگلا) {
    إذا(curl_strnequal(سر->انکڑے, thisheader, thislen) &&
       Curl_headersep(سر->انکڑے[thislen]))
      أرجع سر->انکڑے;
  }

  أرجع عدم;
}
#جے_انت

/* If the header has a value, this function returns TRUE and the value is in
   'outp' with blanks trimmed off. */
ساكن bool سرخی_has_قدر(ثابت حرف **headerp, بنية Curl_str *outp)
{
  bool قدر = !curlx_str_cspn(headerp, outp, ";:") &&
    (!curlx_str_single(headerp, ':') || !curlx_str_single(headerp, ';'));

  إذا(قدر) {
    curlx_str_cspn(headerp, outp, "\r\n");
    curlx_str_trimblanks(outp);
  }
  أرجع قدر;
}

ساكن bool http_سرخی_is_خالی_تھاواں(ثابت حرف *سرخی)
{
  بنية Curl_str باہرلا;

  إذا(سرخی_has_قدر(&سرخی, &باہرلا)) {
    أرجع curlx_strlen(&باہرلا) == 0;
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
ساكن CURLcode copy_custom_قدر(ثابت حرف *سرخی, حرف **valp)
{
  بنية Curl_str باہرلا = { 0 };

  /* find the end of the header name */
  إذا(سرخی_has_قدر(&سرخی, &باہرلا)) {
    *valp = curlx_memdup0(curlx_str(&باہرلا), curlx_strlen(&باہرلا));
    إذا(*valp)
      أرجع CURLE_OK;
    أرجع CURLE_باہرلا_دا_حافظہ;
  }
  /* bad input */
  *valp = عدم;
  أرجع CURLE_BAD_FUNCTION_دلیل;
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
حرف *Curl_copy_سرخی_قدر(ثابت حرف *سرخی)
{
  بنية Curl_str باہرلا;

  /* find the end of the header name */
  إذا(!curlx_str_until(&سرخی, &باہرلا, MAX_HTTP_RESP_سرخی_ناپ, ':') &&
     !curlx_str_single(&سرخی, ':')) {
    curlx_str_untilnl(&سرخی, &باہرلا, MAX_HTTP_RESP_سرخی_ناپ);
    curlx_str_trimblanks(&باہرلا);
    أرجع curlx_memdup0(curlx_str(&باہرلا), curlx_strlen(&باہرلا));
  }
  /* bad input, should never happen */
  DEBUGASSERT(0);
  أرجع عدم;
}

#جے_بنا_تعریف CURL_DISABLE_HTTP_AUTH

#جے_بنا_تعریف CURL_DISABLE_BASIC_AUTH
/*
 * http_output_basic() sets up an Authorization: header (or the proxy version)
 * for HTTP Basic authentication.
 *
 * Returns CURLcode.
 */
ساكن CURLcode http_output_basic(بنية Curl_easy *انکڑے,
                                  بنية connectdata *conn, bool proxy)
{
  size_t ناپ = 0;
  حرف *authorization = عدم;
  حرف **p_hd;
  CURLcode نتیجہ;
  بنية Curl_creds *creds = عدم;
  حرف *باہرلا;

  /* credentials are unique per transfer for HTTP, do not use the ones for the
     connection */
  إذا(proxy) {
#جے_بنا_تعریف CURL_DISABLE_PROXY
    p_hd = &انکڑے->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
#وإلا
    (فراغ)conn;
    أرجع CURLE_NOT_BUILT_وچ;
#جے_انت
  }
  وإلا {
    p_hd = &انکڑے->req.hd_auth;
    creds = انکڑے->state.creds;
  }

  إذا(!creds) {
    DEBUGASSERT(0);
    أرجع CURLE_FAILED_INIT;
  }

  باہرلا = curl_maprintf("%s:%s", creds->user, creds->passwd);
  إذا(!باہرلا)
    أرجع CURLE_باہرلا_دا_حافظہ;

  نتیجہ = curlx_base64_encode((uint8_t *)باہرلا, لڑی_لمبائی(باہرلا),
                               &authorization, &ناپ);
  إذا(نتیجہ)
    اذهب fail;

  إذا(!authorization) {
    نتیجہ = CURLE_REMOTE_ACCESS_DENIED;
    اذهب fail;
  }

  curlx_free(*p_hd);
  *p_hd = curl_maprintf("%sAuthorization: Basic %s\r\n",
                        proxy ? "Proxy-" : "",
                        authorization);
  curlx_free(authorization);
  إذا(!*p_hd) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب fail;
  }

fail:
  curlx_free(باہرلا);
  أرجع نتیجہ;
}

#جے_انت

#جے_بنا_تعریف CURL_DISABLE_BEARER_AUTH
/*
 * http_output_bearer() sets up an Authorization: header
 * for HTTP Bearer authentication.
 *
 * Returns CURLcode.
 */
ساكن CURLcode http_output_bearer(بنية Curl_easy *انکڑے)
{
  حرف **userp;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(Curl_creds_has_oauth_bearer(انکڑے->state.creds));
  userp = &انکڑے->req.hd_auth;
  curlx_free(*userp);
  *userp = curl_maprintf("Authorization: Bearer %s\r\n",
                         Curl_creds_oauth_bearer(انکڑے->state.creds));

  إذا(!*userp) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب fail;
  }

fail:
  أرجع نتیجہ;
}
#جے_انت

#جے_انت

/* pickoneauth() selects the most favorable authentication method from the
 * ones available and the ones we want.
 *
 * return TRUE if one was picked
 */
ساكن bool pickoneauth(بنية auth *pick, بنانشانی لما نقاب,
                        بنية Curl_creds *creds)
{
  bool have_user_pass = Curl_creds_has_user_or_pass(creds);
  bool picked;
  /* only deal with authentication we want */
  بنانشانی لما avail = pick->avail & pick->want & نقاب;
  picked = TRUE;

  /* The order of these checks is highly relevant, as this will be the order
     of preference in case of the existence of multiple accepted types. */
  إذا(avail & CURLAUTH_NEGOTIATE)  /* available on empty creds */
    pick->picked = CURLAUTH_NEGOTIATE;
#جے_بنا_تعریف CURL_DISABLE_BEARER_AUTH
  وإلا إذا((avail & CURLAUTH_BEARER) && Curl_creds_has_oauth_bearer(creds))
    pick->picked = CURLAUTH_BEARER;
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_DIGEST_AUTH
  وإلا إذا((avail & CURLAUTH_DIGEST) && have_user_pass)
    pick->picked = CURLAUTH_DIGEST;
#جے_انت
  وإلا إذا(avail & CURLAUTH_NTLM)
    pick->picked = CURLAUTH_NTLM;
#جے_بنا_تعریف CURL_DISABLE_BASIC_AUTH
  وإلا إذا((avail & CURLAUTH_BASIC) && have_user_pass)
    pick->picked = CURLAUTH_BASIC;
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_AWS
  وإلا إذا(avail & CURLAUTH_AWS_SIGV4)
    pick->picked = CURLAUTH_AWS_SIGV4;
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_HTTPSIG
  وإلا إذا(avail & CURLAUTH_HTTPSIG)
    pick->picked = CURLAUTH_HTTPSIG;
#جے_انت
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
ساكن CURLcode http_perhapsrewind(بنية Curl_easy *انکڑے,
                                   بنية connectdata *conn)
{
  curl_off_t bytessent = انکڑے->req.writebytecount;
  curl_off_t expectsend = Curl_creader_کل_لمائی(انکڑے);
  curl_off_t upload_remain = (expectsend >= 0) ? (expectsend - bytessent) : -1;
  bool little_upload_remains = (upload_remain >= 0 && upload_remain < 2000);
  bool needs_rewind = Curl_creader_needs_rewind(انکڑے);
  /* By default, we would like to abort the transfer when little or unknown
   * amount remains. This may be overridden by authentications further
   * below! */
  bool abort_upload = (!انکڑے->req.upload_پورا && !little_upload_remains);
  VERBOSE(ثابت حرف *ongoing_auth = عدم);

  /* We need a rewind before uploading client read data again. The
   * checks below influence of the upload is to be continued
   * or aborted early.
   * This depends on how much remains to be sent and in what state
   * the authentication is. Some auth schemes such as NTLM do not work
   * for a new connection. */
  إذا(needs_rewind) {
    infof(انکڑے, "Need to rewind upload for next request");
    Curl_creader_سیٹ_rewind(انکڑے, TRUE);
  }

  إذا(conn->بٹ.close)
    /* If we already decided to close this connection, we cannot veto. */
    أرجع CURLE_OK;

  إذا(abort_upload) {
    /* We would like to abort the upload - but should we? */
#جے_تعریف_شدہ USE_NTLM
    إذا((انکڑے->state.authproxy.picked == CURLAUTH_NTLM) ||
       (انکڑے->state.authhost.picked == CURLAUTH_NTLM)) {
      VERBOSE(ongoing_auth = "NTLM");
      إذا((conn->http_ntlm_state != NTLMSTATE_NONE) ||
         (conn->proxy_ntlm_state != NTLMSTATE_NONE)) {
        /* The NTLM-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#جے_انت
#جے_تعریف_شدہ USE_SPNEGO
    /* There is still data left to send */
    إذا((انکڑے->state.authproxy.picked == CURLAUTH_NEGOTIATE) ||
       (انکڑے->state.authhost.picked == CURLAUTH_NEGOTIATE)) {
      VERBOSE(ongoing_auth = "NEGOTIATE");
      إذا((conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) ||
         (conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE)) {
        /* The NEGOTIATE-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#جے_انت
  }

  إذا(abort_upload) {
    إذا(upload_remain >= 0)
      infof(انکڑے, "%s%sclose instead of sending %" FMT_OFF_T " more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "",
            upload_remain);
    وإلا
      infof(انکڑے, "%s%sclose instead of sending unknown amount "
            "of more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "");
    /* We decided to abort the ongoing transfer */
    streamclose(conn);
    انکڑے->req.ناپ = 0; /* do not download any more than 0 bytes */
    انکڑے->req.http_bodyless = TRUE;
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
ساكن bool http_should_fail(بنية Curl_easy *انکڑے, صحيح httpcode)
{
  DEBUGASSERT(انکڑے);
  DEBUGASSERT(انکڑے->conn);

  /*
   * If we have not been asked to fail on error,
   * do not fail.
   */
  إذا(!انکڑے->سیٹ.http_fail_on_error)
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
  إذا(انکڑے->state.resume_from && انکڑے->state.httpreq == HTTPREQ_GET &&
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
  إذا((httpcode == 401) && !انکڑے->state.creds)
    أرجع TRUE;
#جے_بنا_تعریف CURL_DISABLE_PROXY
  إذا((httpcode == 407) && !انکڑے->conn->http_proxy.creds)
    أرجع TRUE;
#جے_انت

  أرجع (bool)انکڑے->state.authproblem;
}

/*
 * Curl_http_auth_act() gets called when all HTTP headers have been received
 * and it checks what authentication methods that are available and decides
 * which one (if any) to use. It will set 'newurl' if an auth method was
 * picked.
 */
CURLcode Curl_http_auth_act(بنية Curl_easy *انکڑے)
{
  بنية connectdata *conn = انکڑے->conn;
  bool pickhost = FALSE;
  bool pickproxy = FALSE;
  CURLcode نتیجہ = CURLE_OK;
  بنانشانی لما authmask = ~0UL;

  إذا(!Curl_creds_has_oauth_bearer(انکڑے->state.creds))
    authmask &= (بنانشانی لما)~CURLAUTH_BEARER;

  إذا(100 <= انکڑے->req.httpcode && انکڑے->req.httpcode <= 199)
    /* this is a transient response code, ignore */
    أرجع CURLE_OK;

  إذا(انکڑے->state.authproblem)
    أرجع انکڑے->سیٹ.http_fail_on_error ? CURLE_HTTP_RETURNED_ERROR : CURLE_OK;

  إذا(انکڑے->state.creds &&
     ((انکڑے->req.httpcode == 401) ||
      (انکڑے->req.authneg && انکڑے->req.httpcode < 300))) {
    pickhost = pickoneauth(&انکڑے->state.authhost, authmask, انکڑے->state.creds);
    إذا(!pickhost)
      انکڑے->state.authproblem = TRUE;
    وإلا
      انکڑے->info.httpauthpicked = انکڑے->state.authhost.picked;
    إذا(انکڑے->state.authhost.picked == CURLAUTH_NTLM &&
       (انکڑے->req.httpversion_گھلیا > 11)) {
      infof(انکڑے, "Forcing HTTP/1.1 for NTLM");
      connclose(conn);
      انکڑے->state.http_neg.wanted = CURL_HTTP_V1x;
      انکڑے->state.http_neg.allowed = CURL_HTTP_V1x;
    }
  }
#جے_بنا_تعریف CURL_DISABLE_PROXY
  إذا(conn->http_proxy.creds &&
     ((انکڑے->req.httpcode == 407) ||
      (انکڑے->req.authneg && انکڑے->req.httpcode < 300))) {
    pickproxy = pickoneauth(&انکڑے->state.authproxy,
                            authmask & ~CURLAUTH_BEARER,
                            conn->http_proxy.creds);
    إذا(!pickproxy)
      انکڑے->state.authproblem = TRUE;
    وإلا
      انکڑے->info.proxyauthpicked = انکڑے->state.authproxy.picked;
  }
#جے_انت

  إذا(pickhost || pickproxy) {
    نتیجہ = http_perhapsrewind(انکڑے, conn);
    إذا(نتیجہ)
      أرجع نتیجہ;

    /* In case this is GSS auth, the newurl field is already allocated so
       we must make sure to free it before allocating a new one. As figured
       out in bug #2284386 */
    curlx_free(انکڑے->req.newurl);
    /* clone URL */
    انکڑے->req.newurl = Curl_bufref_dup(&انکڑے->state.url);
    إذا(!انکڑے->req.newurl)
      أرجع CURLE_باہرلا_دا_حافظہ;
  }
  وإلا إذا((انکڑے->req.httpcode < 300) &&
          !انکڑے->state.authhost.پورا &&
          انکڑے->req.authneg &&
          /* no (known) authentication available,
             authentication is not "done" yet and
             no authentication seems to be required and
             we did not try HEAD or GET */
          (انکڑے->state.httpreq != HTTPREQ_GET) &&
          (انکڑے->state.httpreq != HTTPREQ_سر)) {
    /* clone URL */
    انکڑے->req.newurl = Curl_bufref_dup(&انکڑے->state.url);
    إذا(!انکڑے->req.newurl)
      أرجع CURLE_باہرلا_دا_حافظہ;
    انکڑے->state.authhost.پورا = TRUE;
  }
  إذا(http_should_fail(انکڑے, انکڑے->req.httpcode)) {
    failf(انکڑے, "The requested URL returned error: %d",
          انکڑے->req.httpcode);
    نتیجہ = CURLE_HTTP_RETURNED_ERROR;
  }

  أرجع نتیجہ;
}

#جے_بنا_تعریف CURL_DISABLE_HTTP_AUTH
/*
 * Output the correct authentication header depending on the auth type
 * and whether or not it is to a proxy.
 */
ساكن CURLcode output_auth_سرخیاں(بنية Curl_easy *انکڑے,
                                    بنية connectdata *conn,
                                    بنية auth *authstatus,
                                    ثابت حرف *request,
                                    ثابت حرف *رستہ,
                                    bool proxy)
{
  ثابت حرف *auth = عدم;
  CURLcode نتیجہ = CURLE_OK;
  (فراغ)conn;

#جے_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
  (فراغ)request;
  (فراغ)رستہ;
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_AWS
  إذا((authstatus->picked == CURLAUTH_AWS_SIGV4) && !proxy) {
    /* this method is never for proxy */
    auth = "AWS_SIGV4";
    نتیجہ = Curl_output_aws_sigv4(انکڑے);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }
  وإلا
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_HTTPSIG
  إذا((authstatus->picked == CURLAUTH_HTTPSIG) && !proxy) {
    /* HTTPSIG uses its own configured key material rather than
       data->state.creds. Do not let unrelated credentials from a
       redirected URL bypass the cross-host auth boundary. */
    إذا(Curl_auth_allowed_منزل_host(انکڑے)) {
      auth = "HTTPSIG";
      نتیجہ = Curl_output_httpsig(انکڑے);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
    وإلا
      authstatus->پورا = TRUE;
  }
  وإلا
#جے_انت
#جے_تعریف_شدہ USE_SPNEGO
  إذا(authstatus->picked == CURLAUTH_NEGOTIATE) {
    إذا(
#جے_بنا_تعریف CURL_DISABLE_PROXY
      (proxy && !Curl_checkProxyheaders(انکڑے, conn,
                                        STRCONST("Proxy-authorization"))) ||
#جے_انت
      (!proxy && !Curl_checkheaders(انکڑے, STRCONST("Authorization")) &&
       Curl_auth_allowed_منزل_host(انکڑے))) {
      auth = "Negotiate";
      نتیجہ = Curl_output_negotiate(انکڑے, conn, proxy);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
    وإلا
      authstatus->پورا = TRUE;
  }
  وإلا
#جے_انت
#جے_تعریف_شدہ USE_NTLM
  إذا(authstatus->picked == CURLAUTH_NTLM) {
    إذا(proxy || Curl_auth_allowed_منزل_host(انکڑے)) {
      auth = "NTLM";
      نتیجہ = Curl_output_ntlm(انکڑے, proxy);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
    وإلا
      authstatus->پورا = TRUE;
  }
  وإلا
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_DIGEST_AUTH
  إذا(authstatus->picked == CURLAUTH_DIGEST) {
    auth = "Digest";
    نتیجہ = Curl_output_digest(انکڑے,
                                proxy,
                                (ثابت بنانشانی حرف *)request,
                                (ثابت بنانشانی حرف *)رستہ);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }
  وإلا
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_BASIC_AUTH
  إذا(authstatus->picked == CURLAUTH_BASIC) {
    /* Basic */
    إذا(
#جے_بنا_تعریف CURL_DISABLE_PROXY
       (proxy && conn->http_proxy.creds &&
        Curl_creds_has_user_or_pass(conn->http_proxy.creds) &&
        !Curl_checkProxyheaders(انکڑے, conn,
                                STRCONST("Proxy-authorization"))) ||
#جے_انت
       (!proxy && انکڑے->state.creds &&
        Curl_creds_has_user_or_pass(انکڑے->state.creds) &&
        !Curl_checkheaders(انکڑے, STRCONST("Authorization")))) {
      auth = "Basic";
      نتیجہ = http_output_basic(انکڑے, conn, proxy);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->پورا = TRUE;
  }
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_BEARER_AUTH
  إذا(authstatus->picked == CURLAUTH_BEARER) {
    /* Bearer */
    إذا(!proxy && Curl_creds_has_oauth_bearer(انکڑے->state.creds) &&
       !Curl_checkheaders(انکڑے, STRCONST("Authorization"))) {
      auth = "Bearer";
      نتیجہ = http_output_bearer(انکڑے);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->پورا = TRUE;
  }
#جے_انت

  إذا(auth) {
#جے_بنا_تعریف CURL_DISABLE_PROXY
    إذا(proxy)
      انکڑے->info.proxyauthpicked = authstatus->picked;
    وإلا
      انکڑے->info.httpauthpicked = authstatus->picked;
    infof(انکڑے, "%s auth using %s with user '%s'",
          proxy ? "Proxy" : "Server", auth,
          proxy ? (conn->http_proxy.creds ?
                   conn->http_proxy.creds->user : "") :
          (انکڑے->state.creds ?
           انکڑے->state.creds->user : ""));
#وإلا
    (فراغ)proxy;
    infof(انکڑے, "Server auth using %s with user '%s'",
          auth, انکڑے->state.creds ?
          انکڑے->state.creds->user : "");
#جے_انت
    authstatus->multipass = !authstatus->پورا;
  }
  وإلا {
    authstatus->multipass = FALSE;
    إذا(proxy)
      انکڑے->info.proxyauthpicked = 0;
    وإلا
      انکڑے->info.httpauthpicked = 0;
  }

  أرجع نتیجہ;
}

CURLcode Curl_http_output_auth(بنية Curl_easy *انکڑے,
                               بنية connectdata *conn,
                               ثابت حرف *request,
                               Curl_HttpReq httpreq,
                               ثابت حرف *رستہ,
                               ثابت حرف *query,
                               bool is_connect)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية auth *authhost;
  بنية auth *authproxy;
  ثابت حرف *رستہ_and_query = رستہ;
  حرف *tmp_str = عدم;

  DEBUGASSERT(انکڑے);
  authhost = &انکڑے->state.authhost;
  authproxy = &انکڑے->state.authproxy;

  إذا(
#جے_بنا_تعریف CURL_DISABLE_PROXY
    (!conn->http_proxy.peer || !conn->http_proxy.creds) &&
#جے_انت
#جے_تعریف_شدہ USE_SPNEGO
    !(authhost->want & CURLAUTH_NEGOTIATE) &&
    !(authproxy->want & CURLAUTH_NEGOTIATE) &&
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_HTTPSIG
    !(authhost->want & CURLAUTH_HTTPSIG) &&
#جے_انت
    !انکڑے->state.creds) {
    /* no authentication with no user or password */
    authhost->پورا = TRUE;
    authproxy->پورا = TRUE;
    نتیجہ = CURLE_OK;
    اذهب باہرلا;
  }

  إذا(query) {
    tmp_str = curl_maprintf("%s?%s", رستہ, query);
    إذا(!tmp_str) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب باہرلا;
    }
    رستہ_and_query = tmp_str;
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

#جے_بنا_تعریف CURL_DISABLE_PROXY
  /* Send proxy authentication header if needed */
  إذا(conn->بٹ.origin_is_proxy || is_connect) {
    نتیجہ = output_auth_سرخیاں(انکڑے, conn, authproxy, request,
                                 رستہ_and_query, TRUE);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  وإلا
#وإلا
  (فراغ)is_connect;
#جے_انت /* CURL_DISABLE_PROXY */
    /* we have no proxy so let's pretend we are done authenticating
       with it */
    authproxy->پورا = TRUE;

  /* Either we have credentials for the origin we talk to or
     performing authentication is allowed here */
  إذا(انکڑے->state.creds || Curl_auth_allowed_منزل_host(انکڑے))
    نتیجہ = output_auth_سرخیاں(انکڑے, conn, authhost, request,
                                 رستہ_and_query, FALSE);
  وإلا
    authhost->پورا = TRUE;

  إذا(((authhost->multipass && !authhost->پورا) ||
      (authproxy->multipass && !authproxy->پورا)) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_سر)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    انکڑے->req.authneg = TRUE;
  }
  وإلا
    انکڑے->req.authneg = FALSE;

باہرلا:
  curlx_free(tmp_str);
  أرجع نتیجہ;
}

#وإلا /* !CURL_DISABLE_HTTP_AUTH */
/* when disabled */
CURLcode Curl_http_output_auth(بنية Curl_easy *انکڑے,
                               بنية connectdata *conn,
                               ثابت حرف *request,
                               Curl_HttpReq httpreq,
                               ثابت حرف *رستہ,
                               ثابت حرف *query,
                               bool is_connect)
{
  (فراغ)انکڑے;
  (فراغ)conn;
  (فراغ)request;
  (فراغ)httpreq;
  (فراغ)رستہ;
  (فراغ)query;
  (فراغ)is_connect;
  أرجع CURLE_OK;
}
#جے_انت /* !CURL_DISABLE_HTTP_AUTH, else */

#إذا defined(USE_SPNEGO) || defined(USE_NTLM) || \
  !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_BASIC_AUTH) || \
  !defined(CURL_DISABLE_BEARER_AUTH)
ساكن bool authcmp(ثابت حرف *auth, ثابت حرف *سطر)
{
  /* the auth string must not have an alnum following */
  size_t n = لڑی_لمبائی(auth);
  أرجع curl_strnequal(auth, سطر, n) && !ISALNUM(سطر[n]);
}
#جے_انت

#جے_تعریف_شدہ USE_SPNEGO
ساكن CURLcode auth_spnego(بنية Curl_easy *انکڑے,
                            bool proxy,
                            ثابت حرف *auth,
                            بنية auth *authp,
                            uint32_t *availp)
{
  إذا((authp->avail & CURLAUTH_NEGOTIATE) || Curl_auth_is_spnego_supported()) {
    *availp |= CURLAUTH_NEGOTIATE;
    authp->avail |= CURLAUTH_NEGOTIATE;

    إذا(authp->picked == CURLAUTH_NEGOTIATE) {
      بنية connectdata *conn = انکڑے->conn;
      CURLcode نتیجہ = Curl_داخلہ_negotiate(انکڑے, conn, proxy, auth);
      uint8_t *negstate = proxy ? &conn->proxy_negotiate_state :
        &conn->http_negotiate_state;
      إذا(!نتیجہ) {
        curlx_free(انکڑے->req.newurl);
        انکڑے->req.newurl = Curl_bufref_dup(&انکڑے->state.url);
        إذا(!انکڑے->req.newurl)
          أرجع CURLE_باہرلا_دا_حافظہ;
        انکڑے->state.authproblem = FALSE;
        /* we received a GSS auth token and we dealt with it fine */
        *negstate = CURL_NEGO_AUTH_RECV;
      }
      وإلا
        انکڑے->state.authproblem = TRUE;
    }
  }
  أرجع CURLE_OK;
}
#جے_انت

#جے_تعریف_شدہ USE_NTLM
ساكن CURLcode auth_ntlm(بنية Curl_easy *انکڑے,
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
      CURLcode نتیجہ = Curl_داخلہ_ntlm(انکڑے, proxy, auth);
      إذا(!نتیجہ)
        انکڑے->state.authproblem = FALSE;
      وإلا {
        إذا(نتیجہ == CURLE_باہرلا_دا_حافظہ)
          أرجع نتیجہ;
        infof(انکڑے, "NTLM authentication problem, ignoring.");
        انکڑے->state.authproblem = TRUE;
      }
    }
  }
  أرجع CURLE_OK;
}
#جے_انت

#جے_بنا_تعریف CURL_DISABLE_DIGEST_AUTH
ساكن CURLcode auth_digest(بنية Curl_easy *انکڑے,
                            bool proxy,
                            ثابت حرف *auth,
                            بنية auth *authp,
                            uint32_t *availp)
{
  إذا(authp->avail & CURLAUTH_DIGEST) {
    *availp |= CURLAUTH_DIGEST;
    infof(انکڑے, "Ignoring duplicate digest auth header.");
  }
  وإلا إذا(Curl_auth_is_digest_supported()) {
    CURLcode نتیجہ;

    *availp |= CURLAUTH_DIGEST;
    authp->avail |= CURLAUTH_DIGEST;

    /* We call this function on input Digest headers even if Digest
     * authentication is not activated yet, as we need to store the
     * incoming data from this header in case we are going to use
     * Digest */
    نتیجہ = Curl_داخلہ_digest(انکڑے, proxy, auth);
    إذا(نتیجہ) {
      إذا(نتیجہ == CURLE_باہرلا_دا_حافظہ)
        أرجع نتیجہ;
      infof(انکڑے, "Digest authentication problem, ignoring.");
      انکڑے->state.authproblem = TRUE;
    }
  }
  أرجع CURLE_OK;
}
#جے_انت

#جے_بنا_تعریف CURL_DISABLE_BASIC_AUTH
ساكن CURLcode auth_basic(بنية Curl_easy *انکڑے,
                           بنية auth *authp,
                           uint32_t *availp)
{
  *availp |= CURLAUTH_BASIC;
  authp->avail |= CURLAUTH_BASIC;
  إذا(authp->picked == CURLAUTH_BASIC) {
    /* We asked for Basic authentication but got a 40X back anyway, which
       means our name+password is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(انکڑے, "Basic authentication problem, ignoring.");
    انکڑے->state.authproblem = TRUE;
  }
  أرجع CURLE_OK;
}
#جے_انت

#جے_بنا_تعریف CURL_DISABLE_BEARER_AUTH
ساكن CURLcode auth_bearer(بنية Curl_easy *انکڑے,
                            بنية auth *authp,
                            uint32_t *availp)
{
  *availp |= CURLAUTH_BEARER;
  authp->avail |= CURLAUTH_BEARER;
  إذا(authp->picked == CURLAUTH_BEARER) {
    /* We asked for Bearer authentication but got a 40X back anyway, which
       means our token is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(انکڑے, "Bearer authentication problem, ignoring.");
    انکڑے->state.authproblem = TRUE;
  }
  أرجع CURLE_OK;
}
#جے_انت

/*
 * Curl_http_input_auth() deals with Proxy-Authenticate: and WWW-Authenticate:
 * headers. They are dealt with both in the transfer.c main loop and in the
 * proxy CONNECT loop.
 *
 * The 'auth' line ends with a NUL byte without CR or LF present.
 */
CURLcode Curl_http_داخلہ_auth(بنية Curl_easy *انکڑے, bool proxy,
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
  CURLcode نتیجہ = CURLE_OK;
  DEBUGASSERT(auth);
  DEBUGASSERT(انکڑے);

  إذا(proxy) {
    availp = &انکڑے->info.proxyauthavail;
    authp = &انکڑے->state.authproxy;
  }
  وإلا {
    availp = &انکڑے->info.httpauthavail;
    authp = &انکڑے->state.authhost;
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
#جے_تعریف_شدہ USE_SPNEGO
    إذا(authcmp("Negotiate", auth))
      نتیجہ = auth_spnego(انکڑے, proxy, auth, authp, availp);
#جے_انت
#جے_تعریف_شدہ USE_NTLM
    إذا(!نتیجہ && authcmp("NTLM", auth))
      نتیجہ = auth_ntlm(انکڑے, proxy, auth, authp, availp);
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_DIGEST_AUTH
    إذا(!نتیجہ && authcmp("Digest", auth))
      نتیجہ = auth_digest(انکڑے, proxy, auth, authp, availp);
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_BASIC_AUTH
    إذا(!نتیجہ && authcmp("Basic", auth))
      نتیجہ = auth_basic(انکڑے, authp, availp);
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_BEARER_AUTH
    إذا(authcmp("Bearer", auth))
      نتیجہ = auth_bearer(انکڑے, authp, availp);
#جے_انت

    إذا(نتیجہ)
      اكسر;

    /* there may be multiple methods on one line, so keep reading */
    auth = strchr(auth, ',');
    إذا(auth) /* if we are on a comma, skip it */
      auth++;
    وإلا
      اكسر;
    curlx_str_passblanks(&auth);
  }
  أرجع نتیجہ;
#وإلا
  (فراغ)انکڑے;
  (فراغ)proxy;
  (فراغ)auth;
  /* nothing to do when disabled */
  أرجع CURLE_OK;
#جے_انت
}

ساكن فراغ http_switch_منزل_get(بنية Curl_easy *انکڑے, صحيح کوڈ)
{
  ثابت حرف *req = CURL_EASY_STR(انکڑے, STRING_CUSTOMREQUEST);

  إذا((req || انکڑے->state.httpreq != HTTPREQ_GET) &&
     (انکڑے->سیٹ.http_follow_منوال == CURLFOLLOW_OBEYCODE)) {
    NOVERBOSE((فراغ)کوڈ);
    infof(انکڑے, "Switch to GET because of %d response", کوڈ);
    انکڑے->state.http_ignorecustom = TRUE;
  }
  وإلا إذا(req && (انکڑے->سیٹ.http_follow_منوال != CURLFOLLOW_FIRSTONLY))
    infof(انکڑے, "Stick to %s instead of GET", req);

  انکڑے->state.httpreq = HTTPREQ_GET;
  Curl_creader_سیٹ_rewind(انکڑے, FALSE);
}

#تعریف_کرو HTTPREQ_IS_POST(انکڑے)                    \
  ((انکڑے)->state.httpreq == HTTPREQ_POST ||      \
   (انکڑے)->state.httpreq == HTTPREQ_POST_FORM || \
   (انکڑے)->state.httpreq == HTTPREQ_POST_MIME)

ساكن CURLcode سیٹ_auto_referer(بنية Curl_easy *انکڑے)
{
  CURLU *u;
  حرف *referer = عدم;
  CURLUcode uc;

  /* We are asked to automatically set the previous URL as the referer
     when we get the next URL. We pick the ->url field, which may or may
     not be 100% correct */
  Curl_bufref_free(&انکڑے->state.referer);

  /* Make a copy of the URL without credentials and fragment */
  u = curl_url();
  إذا(!u)
    أرجع CURLE_باہرلا_دا_حافظہ;

  uc = curl_url_سیٹ(u, CURLUPART_URL,
                    Curl_bufref_ptr(&انکڑے->state.url), 0);
  إذا(!uc)
    uc = curl_url_سیٹ(u, CURLUPART_FRAGMENT, عدم, 0);
  إذا(!uc)
    uc = curl_url_سیٹ(u, CURLUPART_USER, عدم, 0);
  إذا(!uc)
    uc = curl_url_سیٹ(u, CURLUPART_PASSWORD, عدم, 0);
  إذا(!uc)
    uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

  curl_url_cleanup(u);

  إذا(uc || !referer)
    أرجع CURLE_باہرلا_دا_حافظہ;

  Curl_bufref_سیٹ(&انکڑے->state.referer, referer, 0, curl_free);
  أرجع CURLE_OK;
}

ساكن CURLcode follow_parse_url(بنية Curl_easy *انکڑے, ثابت حرف *newurl,
                                 followtype type, حرف **follow_urlp,
                                 CURLUcode *ucp)
{
  CURLUcode uc;

  DEBUGASSERT(انکڑے->state.uh);
  uc = curl_url_سیٹ(انکڑے->state.uh, CURLUPART_URL, newurl, (بنانشانی صحيح)
                    ((type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                     ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) |
                     CURLU_ALLOW_SPACE |
                     (انکڑے->سیٹ.رستہ_as_is ? CURLU_رستہ_AS_IS : 0)));
  *ucp = uc;
  إذا(uc) {
    إذا((uc == CURLUE_باہرلا_دا_حافظہ) || (type != FOLLOW_FAKE)) {
      failf(انکڑے, "The redirect target URL could not be parsed: %s",
            curl_url_strerror(uc));
      أرجع Curl_uc_منزل_curlcode(uc);
    }

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, duplicate the field as-is */
    *follow_urlp = curlx_strdup(newurl);
    إذا(!*follow_urlp)
      أرجع CURLE_باہرلا_دا_حافظہ;
  }
  وإلا {
    CURLU *u = curl_url();
    إذا(!u)
      أرجع CURLE_باہرلا_دا_حافظہ;
    uc = curl_url_سیٹ(u, CURLUPART_URL,
                      Curl_bufref_ptr(&انکڑے->state.url),
                      CURLU_URLENCODE | CURLU_ALLOW_SPACE);
    إذا(!uc)
      uc = curl_url_get(انکڑے->state.uh, CURLUPART_URL, follow_urlp, 0);
    إذا(uc) {
      curl_url_cleanup(u);
      أرجع Curl_uc_منزل_curlcode(uc);
    }

#جے_بنا_تعریف CURL_DISABLE_DIGEST_AUTH
    {
      bool same_origin = Curl_url_same_origin(u, انکڑے->state.uh);
      curl_url_cleanup(u);
      إذا(!same_origin)
        Curl_auth_digest_cleanup(&انکڑے->state.digest);
    }
#وإلا
    curl_url_cleanup(u);
#جے_انت
  }
  أرجع CURLE_OK;
}

ساكن CURLcode follow_fake(بنية Curl_easy *انکڑے, حرف *follow_url,
                            CURLUcode uc, bool reachedmax)
{
  /* we are only figuring out the new URL if we would have followed locations
     but now we are done so we can get out! */
  إذا(!uc) {
    CURLU *u = curl_url();
    حرف *nocred = عدم;

    إذا(!u) {
      curlx_free(follow_url);
      أرجع CURLE_باہرلا_دا_حافظہ;
    }
    إذا(!curl_url_سیٹ(u, CURLUPART_URL, follow_url,
                     CURLU_NON_SUPPORT_SCHEME |
                     (انکڑے->سیٹ.رستہ_as_is ? CURLU_رستہ_AS_IS : 0)) &&
       !curl_url_سیٹ(u, CURLUPART_USER, عدم, 0) &&
       !curl_url_سیٹ(u, CURLUPART_PASSWORD, عدم, 0))
      (فراغ)curl_url_get(u, CURLUPART_URL, &nocred, CURLU_GET_خالی_تھاواں);
    curl_url_cleanup(u);
    إذا(nocred) {
      curlx_free(follow_url);
      follow_url = nocred;
    }
  }
  انکڑے->info.wouldredirect = follow_url;

  إذا(reachedmax) {
    failf(انکڑے, "Maximum (%d) redirects followed", انکڑے->سیٹ.maxredirs);
    أرجع CURLE_TOO_MANY_REDIRECTS;
  }
  أرجع CURLE_OK;
}

ساكن bool follow_switch_get(بنية Curl_easy *انکڑے)
{
  /*
   * We get here when the HTTP code is 300-399 and 401 or 407. We need to
   * perform differently based on exactly what return code there was.
   */
  بدل(انکڑے->info.httpcode) {
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
    إذا(HTTPREQ_IS_POST(انکڑے) && !انکڑے->سیٹ.post301) {
      http_switch_منزل_get(انکڑے, 301);
      أرجع TRUE;
    }
    اكسر;
  حالة 302: /*لبھیا*/
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
    إذا(HTTPREQ_IS_POST(انکڑے) && !انکڑے->سیٹ.post302) {
      http_switch_منزل_get(انکڑے, 302);
      أرجع TRUE;
    }
    اكسر;
  حالة 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     */
    إذا(!HTTPREQ_IS_POST(انکڑے) || !انکڑے->سیٹ.post303) {
      http_switch_منزل_get(انکڑے, 303);
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

CURLcode Curl_http_follow(بنية Curl_easy *انکڑے, ثابت حرف *newurl,
                          followtype type)
{
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  حرف *follow_url = عدم;
  CURLUcode uc;
  CURLcode نتیجہ;
  CURLcode rewind_نتیجہ;
  bool switch_منزل_get;

  DEBUGASSERT(type != FOLLOW_NONE);

  إذا(type != FOLLOW_FAKE)
    انکڑے->state.بینتیاں++; /* count all real follows */
  إذا(type == FOLLOW_REDIR) {
    إذا((انکڑے->سیٹ.maxredirs != -1) &&
       (انکڑے->state.followlocation >= انکڑے->سیٹ.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    وإلا {
      انکڑے->state.followlocation++; /* count redirect-followings, including
                                       auth reloads */

      إذا(انکڑے->سیٹ.http_auto_referer) {
        نتیجہ = سیٹ_auto_referer(انکڑے);
        إذا(نتیجہ)
          أرجع نتیجہ;
      }
    }
  }

  إذا((type != FOLLOW_RETRY) &&
     (انکڑے->req.httpcode != 401) && (انکڑے->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, عدم, 0, FALSE)) {
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: do not allow a custom port number */
    disallowport = TRUE;
  }

  نتیجہ = follow_parse_url(انکڑے, newurl, type, &follow_url, &uc);
  إذا(نتیجہ)
    أرجع نتیجہ;

  DEBUGASSERT(follow_url);

  إذا(type == FOLLOW_FAKE)
    أرجع follow_fake(انکڑے, follow_url, uc, reachedmax);

  إذا(disallowport)
    انکڑے->state.allow_پورٹ = FALSE;

  Curl_bufref_سیٹ(&انکڑے->state.url, follow_url, 0, curl_free);
  rewind_نتیجہ = Curl_req_soft_reset(&انکڑے->req, انکڑے);
  infof(انکڑے, "Issue another request to this URL: '%s'", follow_url);
  إذا((انکڑے->سیٹ.http_follow_منوال == CURLFOLLOW_FIRSTONLY) &&
     !انکڑے->state.http_ignorecustom &&
     CURL_EASY_STR(انکڑے, STRING_CUSTOMREQUEST)) {
    انکڑے->state.http_ignorecustom = TRUE;
    infof(انکڑے, "Drop custom request method for next request");
  }

  switch_منزل_get = follow_switch_get(انکڑے);

  /* When rewind of upload data failed and we are not switching to GET,
   * we need to fail the follow, as we cannot send the data again. */
  إذا(rewind_نتیجہ && !switch_منزل_get)
    أرجع rewind_نتیجہ;

  Curl_pgrs_ویلا(انکڑے, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(انکڑے);

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
                        ثابت حرف *سرخی, /* header keyword _with_ colon */
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
  DEBUGASSERT(سرخی);
  DEBUGASSERT(content);

  إذا(!curl_strnequal(headerline, سرخی, hlen))
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
      بنية Curl_str اگلا;
      ثابت حرف *o = p;
      /* after a match there must be a comma, space, newline or NUL byte */
      إذا(curl_strnequal(p, content, clen) &&
         ((p[clen] == ',') || ISBLANK(p[clen]) || ISNEWLINE(p[clen]) ||
          !p[clen]))
        أرجع TRUE; /* match! */
      /* advance to the next comma */
      إذا(curlx_str_until(&p, &اگلا, len, ',') ||
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

ساكن فراغ http_exp100_continue(بنية Curl_easy *انکڑے,
                                 بنية Curl_creader *reader)
{
  بنية cr_exp100_ctx *ctx = reader->ctx;
  إذا(ctx->state > EXP100_SEND_انکڑے) {
    ctx->state = EXP100_SEND_انکڑے;
    Curl_expire_clear(انکڑے, EXPIRE_100_TIMEOUT);
  }
}

ساكن CURLcode cr_exp100_read(بنية Curl_easy *انکڑے,
                               بنية Curl_creader *reader,
                               حرف *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  بنية cr_exp100_ctx *ctx = reader->ctx;
  timediff_t ms;

  بدل(ctx->state) {
  حالة EXP100_SENDING_REQUEST:
    إذا(!Curl_req_sendbuf_خالی_تھاواں(انکڑے)) {
      /* The initial request data has not been fully sent yet. Do
       * not start the timer yet. */
      DEBUGF(infof(انکڑے, "cr_exp100_read, request not full sent yet"));
      *nread = 0;
      *eos = FALSE;
      أرجع CURLE_OK;
    }
    /* We are now waiting for a reply from the server or
     * a timeout on our side IFF the request has been fully sent. */
    DEBUGF(infof(انکڑے, "cr_exp100_read, start AWAITING_CONTINUE, "
                 "timeout %dms", انکڑے->سیٹ.expect_100_timeout));
    ctx->state = EXP100_AWAITING_CONTINUE;
    ctx->شروع = *Curl_pgrs_now(انکڑے);
    Curl_expire_سیٹ(انکڑے, EXPIRE_100_TIMEOUT,
                    انکڑے->سیٹ.expect_100_timeout, &ctx->شروع);
    *nread = 0;
    *eos = FALSE;
    أرجع CURLE_OK;
  حالة EXP100_FAILED:
    DEBUGF(infof(انکڑے, "cr_exp100_read, expectation failed, error"));
    *nread = 0;
    *eos = FALSE;
    أرجع CURLE_READ_ERROR;
  حالة EXP100_AWAITING_CONTINUE:
    ms = curlx_ptimediff_ms(Curl_pgrs_now(انکڑے), &ctx->شروع);
    إذا(ms < انکڑے->سیٹ.expect_100_timeout) {
      DEBUGF(infof(انکڑے, "cr_exp100_read, AWAITING_CONTINUE, not expired"));
      *nread = 0;
      *eos = FALSE;
      أرجع CURLE_OK;
    }
    /* we have waited long enough, continue anyway */
    http_exp100_continue(انکڑے, reader);
    infof(انکڑے, "Done waiting for 100-continue");
    FALLTHROUGH();
  افتراضي:
    DEBUGF(infof(انکڑے, "cr_exp100_read, pass through"));
    أرجع Curl_creader_read(انکڑے, reader->اگلا, buf, blen, nread, eos);
  }
}

ساكن فراغ cr_exp100_پورا(بنية Curl_easy *انکڑے,
                           بنية Curl_creader *reader, صحيح premature)
{
  بنية cr_exp100_ctx *ctx = reader->ctx;
  ctx->state = premature ? EXP100_FAILED : EXP100_SEND_انکڑے;
  Curl_expire_clear(انکڑے, EXPIRE_100_TIMEOUT);
}

ساكن ثابت بنية Curl_crtype cr_exp100 = {
  "cr-exp100",
  Curl_creader_def_init,
  cr_exp100_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  Curl_creader_def_کل_لمائی,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  cr_exp100_پورا,
  حجم(بنية cr_exp100_ctx)
};

ساكن CURLcode http_exp100_جوڑو_reader(بنية Curl_easy *انکڑے)
{
  بنية Curl_creader *reader = عدم;
  CURLcode نتیجہ;

  نتیجہ = Curl_creader_create(&reader, انکڑے, &cr_exp100, CURL_CR_PROTOCOL);
  إذا(!نتیجہ)
    نتیجہ = Curl_creader_جوڑو(انکڑے, reader);
  إذا(!نتیجہ) {
    بنية cr_exp100_ctx *ctx = reader->ctx;
    ctx->state = EXP100_SENDING_REQUEST;
  }

  إذا(نتیجہ && reader)
    Curl_creader_free(انکڑے, reader);
  أرجع نتیجہ;
}

ساكن فراغ http_exp100_got100(بنية Curl_easy *انکڑے)
{
  بنية Curl_creader *r = Curl_creader_get_by_type(انکڑے, &cr_exp100);
  إذا(r)
    http_exp100_continue(انکڑے, r);
}

ساكن bool http_exp100_is_اڈیک(بنية Curl_easy *انکڑے)
{
  بنية Curl_creader *r = Curl_creader_get_by_type(انکڑے, &cr_exp100);
  إذا(r) {
    بنية cr_exp100_ctx *ctx = r->ctx;
    أرجع ctx->state == EXP100_AWAITING_CONTINUE;
  }
  أرجع FALSE;
}

ساكن فراغ http_exp100_send_anyway(بنية Curl_easy *انکڑے)
{
  بنية Curl_creader *r = Curl_creader_get_by_type(انکڑے, &cr_exp100);
  إذا(r)
    http_exp100_continue(انکڑے, r);
}

ساكن bool http_exp100_is_selected(بنية Curl_easy *انکڑے)
{
  بنية Curl_creader *r = Curl_creader_get_by_type(انکڑے, &cr_exp100);
  أرجع !!r;
}

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
CURLcode Curl_http_doing_pollset(بنية Curl_easy *انکڑے,
                                 بنية easy_pollset *ps)
{
  /* write mode */
  أرجع Curl_pollset_جوڑو_باہرلا(انکڑے, ps, انکڑے->conn->sock[FIRSTSOCKET]);
}

CURLcode Curl_http_perform_pollset(بنية Curl_easy *انکڑے,
                                   بنية easy_pollset *ps)
{
  بنية connectdata *conn = انکڑے->conn;
  CURLcode نتیجہ = CURLE_OK;

  إذا(CURL_REQ_WANT_RECV(انکڑے)) {
    نتیجہ = Curl_pollset_جوڑو_وچ(انکڑے, ps, conn->sock[FIRSTSOCKET]);
  }

  /* on a "Expect: 100-continue" timed wait, do not poll for outgoing */
  إذا(!نتیجہ && Curl_req_want_send(انکڑے) && !http_exp100_is_اڈیک(انکڑے)) {
    نتیجہ = Curl_pollset_جوڑو_باہرلا(انکڑے, ps, conn->sock[FIRSTSOCKET]);
  }
  أرجع نتیجہ;
}

ساكن CURLcode http_write_سرخی(بنية Curl_easy *انکڑے,
                                  ثابت حرف *hd, size_t hdlen)
{
  CURLcode نتیجہ;
  صحيح writetype;

  /* now, only output this if the header AND body are requested:
   */
  Curl_debug(انکڑے, CURLINFO_سرخی_وچ, hd, hdlen);

  writetype = CLIENTWRITE_سرخی |
    ((انکڑے->req.httpcode / 100 == 1) ? CLIENTWRITE_1XX : 0);

  نتیجہ = Curl_client_write(انکڑے, writetype, hd, hdlen);
  إذا(نتیجہ)
    أرجع نتیجہ;

  نتیجہ = Curl_bump_headersize(انکڑے, hdlen, FALSE);
  إذا(نتیجہ)
    أرجع نتیجہ;

  انکڑے->req.deductheadercount = (100 <= انکڑے->req.httpcode &&
                                 199 >= انکڑے->req.httpcode) ?
    انکڑے->req.headerbytecount : 0;
  أرجع نتیجہ;
}

/*
 * Curl_http_done() gets called after a single HTTP request has been
 * performed.
 */

CURLcode Curl_http_پورا(بنية Curl_easy *انکڑے,
                        CURLcode حالت, bool premature)
{
  بنية connectdata *conn = انکڑے->conn;

  /* Clear multipass flag. If authentication is not done yet, then it will get
   * a chance to be set back to true when we output the next auth header */
  انکڑے->state.authhost.multipass = FALSE;
  انکڑے->state.authproxy.multipass = FALSE;

  إذا(curlx_dyn_len(&انکڑے->state.headerb)) {
    (فراغ)http_write_سرخی(انکڑے, curlx_dyn_ptr(&انکڑے->state.headerb),
                            curlx_dyn_len(&انکڑے->state.headerb));
  }
  curlx_dyn_reset(&انکڑے->state.headerb);

  إذا(حالت)
    أرجع حالت;

  إذا(!premature && /* this check is pointless when DONE is called before the
                      entire operation is complete */
     !conn->بٹ.retry &&
     !انکڑے->سیٹ.connect_only &&
     (انکڑے->req.bytecount +
      انکڑے->req.headerbytecount -
      انکڑے->req.deductheadercount) <= 0) {
    /* If this connection is not closed to be retried, AND nothing was
       read from the HTTP server (that counts), this cannot be right so we
       return an error here */
    failf(انکڑے, "Empty reply from server");
    /* Mark it as closed to avoid the "left intact" message */
    streamclose(conn);
    أرجع CURLE_GOT_NOTHING;
  }

  أرجع CURLE_OK;
}

/* Determine if we may use HTTP 1.1 for this request. */
ساكن bool http_may_use_1_1(ثابت بنية Curl_easy *انکڑے)
{
  ثابت بنية connectdata *conn = انکڑے->conn;
  /* We have seen a previous response for *this* transfer with 1.0,
   * on another connection or the same one. */
  إذا(انکڑے->state.http_neg.rcvd_min == 10)
    أرجع FALSE;
  /* We have seen a previous response on *this* connection with 1.0. */
  إذا(conn && conn->httpversion_تکیا == 10)
    أرجع FALSE;
  /* We want 1.0 and have seen no previous response on *this* connection
     with a higher version (maybe no response at all yet). */
  إذا(انکڑے->state.http_neg.only_10 &&
     (!conn || conn->httpversion_تکیا <= 10))
    أرجع FALSE;
  /* We are not restricted to use 1.0 only. */
  أرجع !انکڑے->state.http_neg.only_10;
}

ساكن بنانشانی حرف http_request_نسخہ(بنية Curl_easy *انکڑے)
{
  بنانشانی حرف v = Curl_conn_http_نسخہ(انکڑے, انکڑے->conn);
  إذا(!v) {
    /* No specific HTTP connection filter installed. */
    v = http_may_use_1_1(انکڑے) ? 11 : 10;
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

CURLcode Curl_جوڑو_custom_سرخیاں(بنية Curl_easy *انکڑے,
                                 bool is_connect, صحيح httpversion,
                                 بنية dynbuf *req)
{
  بنية curl_slist *h[2];
  بنية curl_slist *سرخیاں;
  صحيح numlists = 1; /* by default */
  صحيح i;

#جے_بنا_تعریف CURL_DISABLE_PROXY
  تعداد Curl_proxy_use proxy;

  إذا(is_connect)
    proxy = سرخی_CONNECT;
  وإلا
    proxy = انکڑے->conn->بٹ.origin_is_proxy ? سرخی_PROXY : سرخی_SERVER;

  بدل(proxy) {
  حالة سرخی_SERVER:
    h[0] = انکڑے->سیٹ.سرخیاں;
    اكسر;
  حالة سرخی_PROXY:
    h[0] = انکڑے->سیٹ.سرخیاں;
    إذا(انکڑے->سیٹ.sep_سرخیاں) {
      h[1] = انکڑے->سیٹ.proxyheaders;
      numlists++;
    }
    اكسر;
  حالة سرخی_CONNECT:
    إذا(انکڑے->سیٹ.sep_سرخیاں)
      h[0] = انکڑے->سیٹ.proxyheaders;
    وإلا
      h[0] = انکڑے->سیٹ.سرخیاں;
    اكسر;
  حالة سرخی_CONNECT_UDP:
    إذا(انکڑے->سیٹ.sep_سرخیاں)
      h[0] = انکڑے->سیٹ.proxyheaders;
    وإلا
      h[0] = انکڑے->سیٹ.سرخیاں;
    اكسر;
  }
#وإلا
  (فراغ)is_connect;
  h[0] = انکڑے->سیٹ.سرخیاں;
#جے_انت

  /* loop through one or two lists */
  لكل(i = 0; i < numlists; i++) {
    لكل(سرخیاں = h[i]; سرخیاں; سرخیاں = سرخیاں->اگلا) {
      CURLcode نتیجہ = CURLE_OK;
      bool blankheader = FALSE;
      بنية Curl_str name;
      ثابت حرف *p = سرخیاں->انکڑے;
      ثابت حرف *origp = p;
      size_t hlen = لڑی_لمبائی(origp);

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

      إذا(انکڑے->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom
            Host: header as that will produce *two* in the same
            request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      وإلا إذا(انکڑے->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      وإلا إذا(انکڑے->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      وإلا إذا(انکڑے->req.authneg &&
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
              !Curl_auth_allowed_منزل_host(انکڑے))
        ;
      وإلا إذا(blankheader) {
        نتیجہ = curlx_dyn_addn(req, curlx_str(&name), curlx_strlen(&name));
        إذا(!نتیجہ)
          نتیجہ = curlx_dyn_addn(req, STRCONST(":\r\n"));
      }
      وإلا
        نتیجہ = curlx_dyn_addf(req, "%s\r\n", origp);

      إذا(نتیجہ)
        أرجع نتیجہ;
    }
  }

  أرجع CURLE_OK;
}

#جے_بنا_تعریف CURL_DISABLE_PARSEDATE
CURLcode Curl_جوڑو_timecondition(بنية Curl_easy *انکڑے,
                                بنية dynbuf *req)
{
  ثابت بنية tm *tm;
  بنية tm keeptime;
  CURLcode نتیجہ;
  حرف datestr[80];
  ثابت حرف *condp;
  size_t len;

  إذا(انکڑے->سیٹ.timecondition == CURL_TIMECOND_NONE)
    /* no condition was asked for */
    أرجع CURLE_OK;

  نتیجہ = curlx_gmtime(انکڑے->سیٹ.timevalue, &keeptime);
  إذا(نتیجہ) {
    failf(انکڑے, "Invalid TIMEVALUE");
    أرجع نتیجہ;
  }
  tm = &keeptime;

  بدل(انکڑے->سیٹ.timecondition) {
  افتراضي:
    DEBUGF(infof(انکڑے, "invalid time condition"));
    أرجع CURLE_BAD_FUNCTION_دلیل;

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

  إذا(Curl_checkheaders(انکڑے, condp, len)) {
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

  نتیجہ = curlx_dyn_جوڑو(req, datestr);
  أرجع نتیجہ;
}
#وإلا
/* disabled */
CURLcode Curl_جوڑو_timecondition(بنية Curl_easy *انکڑے,
                                بنية dynbuf *req)
{
  (فراغ)انکڑے;
  (فراغ)req;
  أرجع CURLE_OK;
}
#جے_انت

فراغ Curl_http_طریقہ(بنية Curl_easy *انکڑے,
                      ثابت حرف **طریقہ, Curl_HttpReq *reqp)
{
  Curl_HttpReq httpreq = (Curl_HttpReq)انکڑے->state.httpreq;
  ثابت حرف *request;
#جے_بنا_تعریف CURL_DISABLE_WEBSOCKETS
  إذا(انکڑے->conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
    httpreq = HTTPREQ_GET;
  وإلا
#جے_انت
  إذا((انکڑے->conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_FTP)) &&
     انکڑے->state.upload)
    httpreq = HTTPREQ_PUT;

  /* Now set the 'request' pointer to the proper request string */
  إذا(!انکڑے->state.http_ignorecustom &&
     CURL_EASY_STR(انکڑے, STRING_CUSTOMREQUEST)) {
    request = CURL_EASY_STR(انکڑے, STRING_CUSTOMREQUEST);
  }
  وإلا {
    إذا(انکڑے->req.no_body)
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
  *طریقہ = request;
  *reqp = httpreq;
}

ساكن CURLcode http_سیٹ_aptr_host(بنية Curl_easy *انکڑے)
{
  بنية connectdata *conn = انکڑے->conn;
  ثابت حرف *ptr = عدم;

  curlx_safefree(انکڑے->state.http_host);
#جے_بنا_تعریف CURL_DISABLE_COOKIES
  curlx_safefree(انکڑے->req.cookiehost);
#جے_انت

  إذا(Curl_peer_equal(انکڑے->state.initial_origin, انکڑے->state.origin))
    ptr = Curl_checkheaders(انکڑے, STRCONST("Host"));

  إذا(ptr) {
#جے_بنا_تعریف CURL_DISABLE_COOKIES
    /* If we have a given custom Host: header, we extract the hostname in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the hostname
       is the same as the first one! */
    حرف *cookiehost;
    CURLcode نتیجہ = copy_custom_قدر(ptr, &cookiehost);
    إذا(نتیجہ)
      أرجع نتیجہ;
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
        memmove(cookiehost, cookiehost + 1, لڑی_لمبائی(cookiehost) - 1);
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
      انکڑے->req.cookiehost = cookiehost;
    }
#جے_انت

    إذا(!curl_strequal("Host:", ptr)) {
      انکڑے->state.http_host = curl_maprintf("Host:%s", &ptr[5]);
      إذا(!انکڑے->state.http_host)
        أرجع CURLE_باہرلا_دا_حافظہ;
    }
  }
  وإلا {
    /* This is the  HTTP Host: header, so we want
     * - for IPv6 origins: "[ipv6-address]" where the IPv6 address is
     *  found in origin->hostname, stripped of zoneid/scopeid.
     * - the (IDN converted) origin->hostname (DNS name or IPv4) otherwise.
     * Note: zoneid/scopeid  only applies to local routing and has no
     * meaning on the remote HTTP server (eg. would confuse it). */
    bool ipv6 = (bool)انکڑے->state.origin->ipv6;
    بنية dynbuf tmp;
    size_t hlen;
    CURLcode نتیجہ;

    curlx_dyn_init(&tmp, DYN_HTTP_REQUEST);
    نتیجہ = curlx_dyn_addn(&tmp, STRCONST("Host: "));
    إذا(!نتیجہ && ipv6)
      نتیجہ = curlx_dyn_addn(&tmp, STRCONST("["));
    إذا(!نتیجہ)
      نتیجہ = curlx_dyn_جوڑو(&tmp, انکڑے->state.origin->hostname);
    إذا(!نتیجہ && ipv6)
      نتیجہ = curlx_dyn_addn(&tmp, STRCONST("]"));
    إذا(!نتیجہ &&
       ((انکڑے->state.origin->پورٹ != انکڑے->state.origin->scheme->defport) ||
       (انکڑے->state.origin->scheme->family != conn->scheme->family))) {
      نتیجہ = curlx_dyn_addf(&tmp, ":%u", انکڑے->state.origin->پورٹ);
    }

    انکڑے->state.http_host = نتیجہ ? عدم : curlx_dyn_take(&tmp, &hlen);
    curlx_dyn_free(&tmp);
    أرجع نتیجہ;
  }
  أرجع CURLE_OK;
}

/*
 * Append the request-target to the HTTP request
 */
ساكن CURLcode http_نشانہ(بنية Curl_easy *انکڑے,
                            بنية dynbuf *r)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *رستہ = انکڑے->state.اتے.رستہ;
  ثابت حرف *query = انکڑے->state.اتے.query;
#جے_بنا_تعریف CURL_DISABLE_PROXY
  بنية connectdata *conn = انکڑے->conn;
#جے_انت

  إذا(CURL_EASY_STR(انکڑے, STRING_نشانہ)) {
    رستہ = CURL_EASY_STR(انکڑے, STRING_نشانہ);
    query = عدم;
  }

#جے_بنا_تعریف CURL_DISABLE_PROXY
  إذا(conn->بٹ.origin_is_proxy) {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL, but if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the decoded hostname! */

    /* and no fragment part */
    CURLUcode uc;
    حرف *url;
    CURLU *h = curl_url_dup(انکڑے->state.uh);
    إذا(!h)
      أرجع CURLE_باہرلا_دا_حافظہ;

    إذا(!انکڑے->state.origin->ipv6 &&
       (انکڑے->state.origin->user_hostname != انکڑے->state.origin->hostname)) {
      uc = curl_url_سیٹ(h, CURLUPART_HOST, انکڑے->state.origin->hostname, 0);
      إذا(uc) {
        curl_url_cleanup(h);
        أرجع CURLE_باہرلا_دا_حافظہ;
      }
    }
    uc = curl_url_سیٹ(h, CURLUPART_FRAGMENT, عدم, 0);
    إذا(uc) {
      curl_url_cleanup(h);
      أرجع CURLE_باہرلا_دا_حافظہ;
    }

    إذا(انکڑے->state.origin->scheme == &Curl_scheme_http) {
      /* when getting HTTP, we do not want the userinfo the URL */
      uc = curl_url_سیٹ(h, CURLUPART_USER, عدم, 0);
      إذا(uc) {
        curl_url_cleanup(h);
        أرجع CURLE_باہرلا_دا_حافظہ;
      }
      uc = curl_url_سیٹ(h, CURLUPART_PASSWORD, عدم, 0);
      إذا(uc) {
        curl_url_cleanup(h);
        أرجع CURLE_باہرلا_دا_حافظہ;
      }
    }
    وإلا إذا(انکڑے->state.creds && (انکڑے->state.creds->سروت != CREDS_URL)) {
        /* credentials not from the URL need to be set */
      uc = curl_url_سیٹ(h, CURLUPART_USER,
                        انکڑے->state.creds->user, CURLU_URLENCODE);
      إذا(!uc)
        uc = curl_url_سیٹ(h, CURLUPART_PASSWORD,
                          انکڑے->state.creds->passwd, CURLU_URLENCODE);
      إذا(uc) {
        curl_url_cleanup(h);
        أرجع Curl_uc_منزل_curlcode(uc);
      }
    }

    /* Extract the URL to use in the request. */
    uc = curl_url_get(h, CURLUPART_URL, &url, CURLU_NO_DEFAULT_پورٹ);
    إذا(uc) {
      curl_url_cleanup(h);
      أرجع CURLE_باہرلا_دا_حافظہ;
    }

    curl_url_cleanup(h);

    /* target or URL */
    نتیجہ = curlx_dyn_جوڑو(r, CURL_EASY_STR(انکڑے, STRING_نشانہ) ?
      CURL_EASY_STR(انکڑے, STRING_نشانہ) : url);
    curlx_free(url);
    إذا(نتیجہ)
      أرجع نتیجہ;

    إذا((انکڑے->state.origin->scheme == &Curl_scheme_ftp) &&
       انکڑے->سیٹ.proxy_transfer_منوال) {
      /* when doing ftp, append ;type=<a|i> if not present */
      size_t len = لڑی_لمبائی(رستہ);
      bool type_present = FALSE;
      إذا((len >= 7) && !memcmp(&رستہ[len - 7], ";type=", 6)) {
        بدل(Curl_raw_toupper(رستہ[len - 1])) {
        حالة 'A':
        حالة 'D':
        حالة 'I':
          type_present = TRUE;
          اكسر;
        }
      }
      إذا(!type_present) {
        نتیجہ = curlx_dyn_addf(r, ";type=%c",
                                انکڑے->state.prefer_ascii ? 'a' : 'i');
        إذا(نتیجہ)
          أرجع نتیجہ;
      }
    }
  }

  وإلا
#جے_انت
  {
    نتیجہ = curlx_dyn_جوڑو(r, رستہ);
    إذا(نتیجہ)
      أرجع نتیجہ;
    إذا(query)
      نتیجہ = curlx_dyn_addf(r, "?%s", query);
  }

  أرجع نتیجہ;
}

#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
ساكن CURLcode سیٹ_post_reader(بنية Curl_easy *انکڑے, Curl_HttpReq httpreq)
{
  CURLcode نتیجہ;

  بدل(httpreq) {
#جے_بنا_تعریف CURL_DISABLE_MIME
  حالة HTTPREQ_POST_MIME:
    انکڑے->state.mimepost = انکڑے->سیٹ.mimepostp;
    اكسر;
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_FORM_API
  حالة HTTPREQ_POST_FORM:
    /* Convert the form structure into a mime structure, then keep
       the conversion */
    إذا(!انکڑے->state.formp) {
      انکڑے->state.formp = curlx_calloc(1, حجم(curl_mimepart));
      إذا(!انکڑے->state.formp)
        أرجع CURLE_باہرلا_دا_حافظہ;
      Curl_mime_cleanpart(انکڑے->state.formp);
      نتیجہ = Curl_getformdata(انکڑے, انکڑے->state.formp, انکڑے->سیٹ.httppost,
                                انکڑے->state.fread_func);
      إذا(نتیجہ) {
        curlx_safefree(انکڑے->state.formp);
        أرجع نتیجہ;
      }
      انکڑے->state.mimepost = انکڑے->state.formp;
    }
    اكسر;
#جے_انت
  افتراضي:
    انکڑے->state.mimepost = عدم;
    اكسر;
  }

  بدل(httpreq) {
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
    /* This is form posting using mime data. */
#جے_بنا_تعریف CURL_DISABLE_MIME
    إذا(انکڑے->state.mimepost) {
      ثابت حرف *cthdr = Curl_checkheaders(انکڑے, STRCONST("Content-Type"));

      /* Read and seek body only. */
      انکڑے->state.mimepost->flags |= MIME_BODY_ONLY;

      /* Prepare the mime structure headers & set content type. */

      إذا(cthdr)
        لكل(cthdr += 13; *cthdr == ' '; cthdr++)
          ;
      وإلا إذا(انکڑے->state.mimepost->قسم == MIMEKIND_MULTIPART)
        cthdr = "multipart/form-data";

      curl_mime_سرخیاں(انکڑے->state.mimepost, انکڑے->سیٹ.سرخیاں, 0);
      نتیجہ = Curl_mime_prepare_سرخیاں(انکڑے, انکڑے->state.mimepost, cthdr,
                                         عدم, MIMESTRATEGY_FORM);
      إذا(نتیجہ)
        أرجع نتیجہ;
      curl_mime_سرخیاں(انکڑے->state.mimepost, عدم, 0);
      نتیجہ = Curl_creader_سیٹ_mime(انکڑے, انکڑے->state.mimepost);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
    وإلا
#جے_انت
    {
      نتیجہ = Curl_creader_سیٹ_null(انکڑے);
    }
    انکڑے->state.infilesize = Curl_creader_کل_لمائی(انکڑے);
    أرجع نتیجہ;

  افتراضي:
    أرجع Curl_creader_سیٹ_null(انکڑے);
  }
  /* never reached */
}
#جے_انت

ساكن CURLcode سیٹ_reader(بنية Curl_easy *انکڑے, Curl_HttpReq httpreq)
{
  CURLcode نتیجہ = CURLE_OK;
  curl_off_t postsize = انکڑے->state.infilesize;

  DEBUGASSERT(انکڑے->conn);

  إذا(انکڑے->req.authneg) {
    أرجع Curl_creader_سیٹ_null(انکڑے);
  }

  بدل(httpreq) {
  حالة HTTPREQ_PUT: /* Let's PUT the data to the server! */
    أرجع postsize ? Curl_creader_سیٹ_fread(انکڑے, postsize) :
      Curl_creader_سیٹ_null(انکڑے);

#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
    أرجع سیٹ_post_reader(انکڑے, httpreq);
#جے_انت

  حالة HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */
    /* the size of the post body */
    إذا(!postsize) {
      نتیجہ = Curl_creader_سیٹ_null(انکڑے);
    }
    وإلا إذا(انکڑے->سیٹ.postfields) {
      size_t plen = curlx_sotouz_range(postsize, 0, SIZE_MAX);
      إذا(plen == SIZE_MAX)
        أرجع CURLE_باہرلا_دا_حافظہ;
      وإلا إذا(plen)
        نتیجہ = Curl_creader_سیٹ_buf(انکڑے, انکڑے->سیٹ.postfields, plen);
      وإلا
        نتیجہ = Curl_creader_سیٹ_null(انکڑے);
    }
    وإلا {
      /* we read the bytes from the callback. In case "chunked" encoding
       * is forced by the application, we disregard `postsize`. This is
       * a backward compatibility decision to earlier versions where
       * chunking disregarded this. See issue #13229. */
      bool chunked = FALSE;
      حرف *ptr = Curl_checkheaders(انکڑے, STRCONST("Transfer-Encoding"));
      إذا(ptr) {
        /* Some kind of TE is requested, check if 'chunked' is chosen */
        chunked = Curl_compareheader(ptr, STRCONST("Transfer-Encoding:"),
                                     STRCONST("chunked"));
      }
      نتیجہ = Curl_creader_سیٹ_fread(انکڑے, chunked ? -1 : postsize);
    }
    أرجع نتیجہ;

  افتراضي:
    /* HTTP GET/HEAD download, has no body, needs no Content-Length */
    انکڑے->state.infilesize = 0;
    أرجع Curl_creader_سیٹ_null(انکڑے);
  }
  /* not reached */
}

ساكن CURLcode http_resume(بنية Curl_easy *انکڑے, Curl_HttpReq httpreq)
{
  إذا((HTTPREQ_POST == httpreq || HTTPREQ_POST_FORM == httpreq ||
      HTTPREQ_POST_MIME == httpreq || HTTPREQ_PUT == httpreq) &&
     انکڑے->state.resume_from) {
    failf(انکڑے, "HTTP upload cannot be resumed");
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode http_req_سیٹ_TE(بنية Curl_easy *انکڑے,
                                بنية dynbuf *req,
                                صحيح httpversion)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *ptr;

  ptr = Curl_checkheaders(انکڑے, STRCONST("Transfer-Encoding"));
  إذا(ptr) {
    /* Some kind of TE is requested, check if 'chunked' is chosen */
    انکڑے->req.upload_chunky =
      Curl_compareheader(ptr,
                         STRCONST("Transfer-Encoding:"), STRCONST("chunked"));
    إذا(انکڑے->req.upload_chunky && (httpversion >= 20)) {
      infof(انکڑے, "suppressing chunked transfer encoding on connection "
            "using HTTP version 2 or higher");
      انکڑے->req.upload_chunky = FALSE;
    }
  }
  وإلا {
    curl_off_t req_clen = Curl_creader_کل_لمائی(انکڑے);

    إذا(req_clen < 0) {
      /* indeterminate request content length */
      إذا(httpversion > 10) {
        /* On HTTP/1.1, enable chunked, on HTTP/2 and later we do not
         * need it */
        انکڑے->req.upload_chunky = (httpversion < 20);
      }
      وإلا {
        failf(انکڑے, "Chunky upload is not supported by HTTP 1.0");
        أرجع CURLE_UPLOAD_FAILED;
      }
    }
    وإلا {
      /* else, no chunky upload */
      انکڑے->req.upload_chunky = FALSE;
    }

    إذا(انکڑے->req.upload_chunky)
      نتیجہ = curlx_dyn_جوڑو(req, "Transfer-Encoding: chunked\r\n");
  }
  أرجع نتیجہ;
}

ساكن CURLcode addexpect(بنية Curl_easy *انکڑے, بنية dynbuf *r,
                          صحيح httpversion, bool *announced_exp100)
{
  CURLcode نتیجہ;
  حرف *ptr;

  *announced_exp100 = FALSE;
  /* Avoid Expect: 100-continue if Upgrade: is used */
  إذا(انکڑے->req.upgr101 != UPGR101_NONE)
    أرجع CURLE_OK;

  /* For really small puts we do not use Expect: headers at all, and for
     the somewhat bigger ones we allow the app to disable it. Make
     sure that the expect100header is always set to the preferred value
     here. */
  ptr = Curl_checkheaders(انکڑے, STRCONST("Expect"));
  إذا(ptr) {
    *announced_exp100 =
      Curl_compareheader(ptr, STRCONST("Expect:"), STRCONST("100-continue"));
  }
  وإلا إذا(!انکڑے->state.disableexpect && (httpversion == 11)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    curl_off_t client_len = Curl_creader_client_لمائی(انکڑے);
    إذا(client_len > EXPECT_100_حد || client_len < 0) {
      نتیجہ = curlx_dyn_addn(r, STRCONST("Expect: 100-continue\r\n"));
      إذا(نتیجہ)
        أرجع نتیجہ;
      *announced_exp100 = TRUE;
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode http_جوڑو_content_hds(بنية Curl_easy *انکڑے,
                                     بنية dynbuf *r,
                                     صحيح httpversion,
                                     Curl_HttpReq httpreq)
{
  CURLcode نتیجہ = CURLE_OK;
  curl_off_t req_clen;
  bool announced_exp100 = FALSE;

  DEBUGASSERT(انکڑے->conn);
  إذا(انکڑے->req.upload_chunky) {
    نتیجہ = Curl_httpchunk_جوڑو_reader(انکڑے);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  /* Get the request body length that has been set up */
  req_clen = Curl_creader_کل_لمائی(انکڑے);
  بدل(httpreq) {
  حالة HTTPREQ_PUT:
  حالة HTTPREQ_POST:
#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
#جے_انت
    /* We only set Content-Length and allow a custom Content-Length if
       we do not upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length).
       We do not override a custom "Content-Length" header, but during
       authentication negotiation that header is suppressed.
     */
    إذا(req_clen >= 0 && !انکڑے->req.upload_chunky &&
       (انکڑے->req.authneg ||
        !Curl_checkheaders(انکڑے, STRCONST("Content-Length")))) {
      /* we allow replacing this header if not during auth negotiation,
         although it is not wise to actually set your own */
      نتیجہ = curlx_dyn_addf(r, "Content-Length: %" FMT_OFF_T "\r\n",
                              req_clen);
    }
    إذا(نتیجہ)
      اذهب باہرلا;

#جے_بنا_تعریف CURL_DISABLE_MIME
    /* Output mime-generated headers. */
    إذا(انکڑے->state.mimepost &&
       ((httpreq == HTTPREQ_POST_FORM) || (httpreq == HTTPREQ_POST_MIME))) {
      بنية curl_slist *hdr;

      لكل(hdr = انکڑے->state.mimepost->curlheaders; hdr; hdr = hdr->اگلا) {
        نتیجہ = curlx_dyn_addf(r, "%s\r\n", hdr->انکڑے);
        إذا(نتیجہ)
          اذهب باہرلا;
      }
    }
#جے_انت
    إذا(httpreq == HTTPREQ_POST &&
       !Curl_checkheaders(انکڑے, STRCONST("Content-Type"))) {
      نتیجہ = curlx_dyn_addn(r, STRCONST("Content-Type: application/"
                                          "x-www-form-urlencoded\r\n"));
      إذا(نتیجہ)
        اذهب باہرلا;
    }
    نتیجہ = addexpect(انکڑے, r, httpversion, &announced_exp100);
    إذا(نتیجہ)
      اذهب باہرلا;
    اكسر;
  افتراضي:
    اكسر;
  }

  Curl_pgrs_سیٹ_Upload_ناپ(انکڑے, req_clen);
  إذا(announced_exp100)
    نتیجہ = http_exp100_جوڑو_reader(انکڑے);

باہرلا:
  أرجع نتیجہ;
}

#جے_بنا_تعریف CURL_DISABLE_COOKIES

ساكن CURLcode http_cookies(بنية Curl_easy *انکڑے,
                             بنية dynbuf *r)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *addcookies = عدم;
  bool linecap = FALSE;
  إذا(CURL_EASY_STR(انکڑے, STRING_COOKIE) &&
     !Curl_checkheaders(انکڑے, STRCONST("Cookie")) &&
     Curl_auth_allowed_منزل_host(انکڑے))
    addcookies = CURL_EASY_STR(انکڑے, STRING_COOKIE);

  إذا(انکڑے->cookies || addcookies) {
    بنية Curl_llist list;
    صحيح گنتی = 0;

    إذا(انکڑے->cookies && انکڑے->state.cookie_engine) {
      bool okay;
      ثابت حرف *host = انکڑے->req.cookiehost ?
        انکڑے->req.cookiehost : انکڑے->state.origin->hostname;
      Curl_share_جندرا(انکڑے, CURL_جندرا_انکڑے_COOKIE, CURL_جندرا_ACCESS_SINGLE);
      نتیجہ = Curl_cookie_getlist(انکڑے, &okay, host, &list);
      إذا(!نتیجہ && okay) {
        بنية Curl_llist_گرہ *n;
        size_t clen = 8; /* hold the size of the generated Cookie: header */

        /* loop through all cookies that matched */
        لكل(n = Curl_llist_سر(&list); n; n = Curl_گرہ_اگلا(n)) {
          بنية Cookie *co = Curl_گرہ_elem(n);
          إذا(co->قدر) {
            size_t جوڑو;
            إذا(!گنتی) {
              نتیجہ = curlx_dyn_addn(r, STRCONST("Cookie: "));
              إذا(نتیجہ)
                اكسر;
            }
            جوڑو = لڑی_لمبائی(co->name) + لڑی_لمبائی(co->قدر) + 1;
            إذا(clen + جوڑو >= MAX_COOKIE_سرخی_LEN) {
              infof(انکڑے, "Restricted outgoing cookies due to header size, "
                    "'%s' not sent", co->name);
              linecap = TRUE;
              اكسر;
            }
            نتیجہ = curlx_dyn_addf(r, "%s%s=%s", گنتی ? "; " : "",
                                    co->name, co->قدر);
            إذا(نتیجہ)
              اكسر;
            clen += جوڑو + (گنتی ? 2 : 0);
            گنتی++;
          }
        }
        Curl_llist_destroy(&list, عدم);
      }
      Curl_share_unlock(انکڑے, CURL_جندرا_انکڑے_COOKIE);
    }
    إذا(addcookies && !نتیجہ && !linecap) {
      إذا(!گنتی)
        نتیجہ = curlx_dyn_addn(r, STRCONST("Cookie: "));
      إذا(!نتیجہ) {
        نتیجہ = curlx_dyn_addf(r, "%s%s", گنتی ? "; " : "", addcookies);
        گنتی++;
      }
    }
    إذا(گنتی && !نتیجہ)
      نتیجہ = curlx_dyn_addn(r, STRCONST("\r\n"));

    إذا(نتیجہ)
      أرجع نتیجہ;
  }
  أرجع نتیجہ;
}
#وإلا
#تعریف_کرو http_cookies(a, b) CURLE_OK
#جے_انت

ساكن CURLcode http_range(بنية Curl_easy *انکڑے,
                           Curl_HttpReq httpreq)
{
  إذا(انکڑے->state.use_range) {
    /*
     * A range is selected. We use different headers whether we are downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    إذا(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_سر)) &&
       !Curl_checkheaders(انکڑے, STRCONST("Range"))) {
      /* if a line like this was already allocated, free the previous one */
      curlx_free(انکڑے->state.rangeline);
      انکڑے->state.rangeline = curl_maprintf("Range: bytes=%s\r\n",
                                                 انکڑے->state.range);
      إذا(!انکڑے->state.rangeline)
        أرجع CURLE_باہرلا_دا_حافظہ;
    }
    وإلا إذا((httpreq == HTTPREQ_POST || httpreq == HTTPREQ_PUT) &&
            !Curl_checkheaders(انکڑے, STRCONST("Content-Range"))) {
      curl_off_t req_clen = Curl_creader_کل_لمائی(انکڑے);
      /* if a line like this was already allocated, free the previous one */
      curlx_free(انکڑے->state.rangeline);

      إذا(انکڑے->سیٹ.سیٹ_resume_from < 0) {
        /* Upload resume was asked for, but we do not know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        انکڑے->state.rangeline =
          curl_maprintf("Content-Range: bytes 0-%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n", req_clen - 1, req_clen);
      }
      وإلا إذا(انکڑے->state.resume_from) {
        /* This is because "resume" was selected */
        /* Not sure if we want to send this header during authentication
         * negotiation, but test1084 checks for it. In which case we have a
         * "null" client reader installed that gives an unexpected length. */
        curl_off_t کل_len = انکڑے->req.authneg ?
                               انکڑے->state.infilesize :
                               (انکڑے->state.resume_from + req_clen);
        انکڑے->state.rangeline =
          curl_maprintf("Content-Range: bytes %s%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n",
                        انکڑے->state.range, کل_len - 1, کل_len);
      }
      وإلا {
        /* Range was selected and then we pass the incoming range and append
           total size */
        انکڑے->state.rangeline =
          curl_maprintf("Content-Range: bytes %s/%" FMT_OFF_T "\r\n",
                        انکڑے->state.range, req_clen);
      }
      إذا(!انکڑے->state.rangeline)
        أرجع CURLE_باہرلا_دا_حافظہ;
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode http_firstwrite(بنية Curl_easy *انکڑے)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية SingleRequest *k = &انکڑے->req;

  إذا(انکڑے->req.newurl) {
    إذا(conn->بٹ.close) {
      /* Abort after the headers if "follow Location" is set
         and we are set to close anyway. */
      CURL_REQ_CLEAR_RECV(انکڑے);
      k->پورا = TRUE;
      أرجع CURLE_OK;
    }
    /* We have a new URL to load, but since we want to be able to reuse this
       connection properly, we read the full response in "ignore more" */
    k->ignorebody = TRUE;
    infof(انکڑے, "Ignoring the response-body");
  }
  إذا(انکڑے->state.resume_from && !k->content_range &&
     (انکڑے->state.httpreq == HTTPREQ_GET) &&
     !k->ignorebody) {

    إذا(k->ناپ == انکڑے->state.resume_from) {
      /* The resume point is at the end of file, consider this fine even if it
         does not allow resume from here. */
      infof(انکڑے, "The entire document is already downloaded");
      streamclose(conn);
      /* Abort download */
      CURL_REQ_CLEAR_RECV(انکڑے);
      k->پورا = TRUE;
      أرجع CURLE_OK;
    }

    /* we wanted to resume a download, although the server does not seem to
     * support this and we did this with a GET (if it was not a GET we did a
     * POST or PUT resume) */
    failf(انکڑے, "HTTP server does not seem to support "
          "byte ranges. Cannot resume.");
    أرجع CURLE_RANGE_ERROR;
  }

  إذا(انکڑے->سیٹ.timecondition && !انکڑے->state.range &&
     /* A time condition has been set AND no ranges have been requested. This
        seems to be what chapter 13.3.4 of RFC 2616 defines to be the correct
        action for an HTTP/1.1 client */
     !Curl_meets_timecondition(انکڑے, k->timeofdoc)) {
    k->پورا = TRUE;
    /* We are simulating an HTTP 304 from server so we return
       what should have been returned from the server */
    انکڑے->info.httpcode = 304;
    infof(انکڑے, "Simulate an HTTP 304 response");
    /* we abort the transfer before it is completed == we ruin the
       reuse ability. Close the connection */
    streamclose(conn);
    أرجع CURLE_OK;
  } /* we have a time condition */

  أرجع CURLE_OK;
}

ساكن CURLcode http_check_new_conn(بنية Curl_easy *انکڑے)
{
  بنية connectdata *conn = انکڑے->conn;
  ثابت حرف *info_نسخہ = عدم;
  ثابت حرف *alpn;
  CURLcode نتیجہ;

  alpn = Curl_conn_get_alpn_negotiated(انکڑے, conn);
  إذا(alpn && !strcmp("h3", alpn)) {
#جے_بنا_تعریف CURL_DISABLE_PROXY
    إذا(!conn->بٹ.origin_is_proxy)
#جے_انت
      DEBUGASSERT(Curl_conn_http_نسخہ(انکڑے, conn) == 30);
    info_نسخہ = "HTTP/3";
  }
  وإلا إذا(alpn && !strcmp("h2", alpn)) {
#جے_بنا_تعریف CURL_DISABLE_PROXY
    إذا((Curl_conn_http_نسخہ(انکڑے, conn) != 20) &&
       conn->بٹ.origin_is_proxy) {
      نتیجہ = Curl_http2_switch(انکڑے);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
    وإلا
#جے_انت
    DEBUGASSERT(Curl_conn_http_نسخہ(انکڑے, conn) == 20);
    info_نسخہ = "HTTP/2";
  }
  وإلا {
    /* Check if user wants to use HTTP/2 with clear TCP */
    إذا(Curl_http2_may_switch(انکڑے)) {
      DEBUGF(infof(انکڑے, "HTTP/2 over clean TCP"));
      نتیجہ = Curl_http2_switch(انکڑے);
      إذا(نتیجہ)
        أرجع نتیجہ;
      info_نسخہ = "HTTP/2";
      /* There is no ALPN here, but the connection is now definitely h2 */
      conn->httpversion_تکیا = 20;
      Curl_conn_سیٹ_multiplex(conn);
    }
    وإلا
      info_نسخہ = "HTTP/1.x";
  }

  إذا(info_نسخہ)
    infof(انکڑے, "using %s", info_نسخہ);
  أرجع CURLE_OK;
}

ساكن CURLcode http_جوڑو_connection_hd(بنية Curl_easy *انکڑے,
                                       بنية dynbuf *req)
{
  بنية curl_slist *سر;
  ثابت حرف *sep = "Connection: ";
  CURLcode نتیجہ = CURLE_OK;
  size_t rlen = curlx_dyn_len(req);
  bool skip;

  /* Add the 1st custom "Connection: " header, if there is one */
  لكل(سر = انکڑے->سیٹ.سرخیاں; سر; سر = سر->اگلا) {
    إذا(curl_strnequal(سر->انکڑے, "Connection", 10) &&
       Curl_headersep(سر->انکڑے[10]) &&
       !http_سرخی_is_خالی_تھاواں(سر->انکڑے)) {
      حرف *قدر;
      نتیجہ = copy_custom_قدر(سر->انکڑے, &قدر);
      إذا(نتیجہ)
        أرجع نتیجہ;
      نتیجہ = curlx_dyn_addf(req, "%s%s", sep, قدر);
      sep = ", ";
      curlx_free(قدر);
      اكسر; /* leave, having added 1st one */
    }
  }

  /* add our internal Connection: header values, if we have any */
  إذا(!نتیجہ && انکڑے->state.http_hd_te) {
    نتیجہ = curlx_dyn_addf(req, "%s%s", sep, "TE");
    sep = ", ";
  }
  إذا(!نتیجہ && انکڑے->state.http_hd_upgrade) {
    نتیجہ = curlx_dyn_addf(req, "%s%s", sep, "Upgrade");
    sep = ", ";
  }
  إذا(!نتیجہ && انکڑے->state.http_hd_h2_settings) {
    نتیجہ = curlx_dyn_addf(req, "%s%s", sep, "HTTP2-Settings");
  }
  إذا(!نتیجہ && (rlen < curlx_dyn_len(req)))
    نتیجہ = curlx_dyn_addn(req, STRCONST("\r\n"));
  إذا(نتیجہ)
    أرجع نتیجہ;

  /* Add all user-defined Connection: headers after the first */
  skip = TRUE;
  لكل(سر = انکڑے->سیٹ.سرخیاں; سر; سر = سر->اگلا) {
    إذا(curl_strnequal(سر->انکڑے, "Connection", 10) &&
       Curl_headersep(سر->انکڑے[10]) &&
       !http_سرخی_is_خالی_تھاواں(سر->انکڑے)) {
      إذا(skip) {
        skip = FALSE;
        استمر;
      }
      نتیجہ = curlx_dyn_addf(req, "%s\r\n", سر->انکڑے);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
  }

  أرجع CURLE_OK;
}

/* Header identifier in order we send them by default */
تعريفنوع تعداد {
  H1_HD_REQUEST,
  H1_HD_HOST,
#جے_بنا_تعریف CURL_DISABLE_PROXY
  H1_HD_PROXY_AUTH,
#جے_انت
  H1_HD_AUTH,
  H1_HD_RANGE,
  H1_HD_USER_AGENT,
  H1_HD_ACCEPT,
  H1_HD_TE,
  H1_HD_ACCEPT_ENCODING,
  H1_HD_REFERER,
#جے_بنا_تعریف CURL_DISABLE_PROXY
  H1_HD_PROXY_CONNECTION,
#جے_انت
  H1_HD_TRANSFER_ENCODING,
#جے_بنا_تعریف CURL_DISABLE_ALTSVC
  H1_HD_ALT_ورتیا,
#جے_انت
  H1_HD_UPGRADE,
  H1_HD_COOKIES,
  H1_HD_CONDITIONALS,
  H1_HD_CUSTOM,
  H1_HD_CONTENT,
  H1_HD_CONNECTION,
  H1_HD_آخری  /* the last, empty header line */
} http_hd_t;

ساكن CURLcode http_جوڑو_hd(بنية Curl_easy *انکڑے,
                            بنية dynbuf *req,
                            http_hd_t id,
                            بنانشانی حرف httpversion,
                            ثابت حرف *طریقہ,
                            Curl_HttpReq httpreq)
{
  CURLcode نتیجہ = CURLE_OK;
#إذا !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_PROXY) || \
  !defined(CURL_DISABLE_WEBSOCKETS)
  بنية connectdata *conn = انکڑے->conn;
#جے_انت
  بدل(id) {
  حالة H1_HD_REQUEST:
    /* add the main request stuff */
    /* GET/HEAD/POST/PUT */
    نتیجہ = curlx_dyn_addf(req, "%s ", طریقہ);
    إذا(!نتیجہ)
      نتیجہ = http_نشانہ(انکڑے, req);
    إذا(!نتیجہ)
      نتیجہ = curlx_dyn_addf(req, " HTTP/%s\r\n",
                              get_http_string(httpversion));
    اكسر;

  حالة H1_HD_HOST:
    إذا(انکڑے->state.http_host) {
      نتیجہ = curlx_dyn_جوڑو(req, انکڑے->state.http_host);
      إذا(!نتیجہ)
        نتیجہ = curlx_dyn_addn(req, STRCONST("\r\n"));
    }
    اكسر;

#جے_بنا_تعریف CURL_DISABLE_PROXY
  حالة H1_HD_PROXY_AUTH:
    إذا(انکڑے->req.hd_proxy_auth)
      نتیجہ = curlx_dyn_جوڑو(req, انکڑے->req.hd_proxy_auth);
    اكسر;
#جے_انت

  حالة H1_HD_AUTH:
    إذا(انکڑے->req.hd_auth)
      نتیجہ = curlx_dyn_جوڑو(req, انکڑے->req.hd_auth);
    اكسر;

  حالة H1_HD_RANGE:
    إذا(انکڑے->state.use_range && انکڑے->state.rangeline)
      نتیجہ = curlx_dyn_جوڑو(req, انکڑے->state.rangeline);
    اكسر;

  حالة H1_HD_USER_AGENT: {
    ثابت حرف *ua = CURL_EASY_STR(انکڑے, STRING_USERAGENT);
    إذا(ua && *ua && !Curl_checkheaders(انکڑے, STRCONST("User-Agent")))
      نتیجہ = curlx_dyn_addf(req, "User-Agent: %s\r\n", ua);
    اكسر;
  }

  حالة H1_HD_ACCEPT:
    إذا(!Curl_checkheaders(انکڑے, STRCONST("Accept")))
      نتیجہ = curlx_dyn_جوڑو(req, "Accept: */*\r\n");
    اكسر;

  حالة H1_HD_TE:
#جے_تعریف_شدہ HAVE_LIBZ
    إذا(!Curl_checkheaders(انکڑے, STRCONST("TE")) &&
       انکڑے->سیٹ.http_transfer_encoding) {
      انکڑے->state.http_hd_te = TRUE;
      نتیجہ = curlx_dyn_جوڑو(req, "TE: gzip\r\n");
    }
#جے_انت
    اكسر;

  حالة H1_HD_ACCEPT_ENCODING: {
    ثابت حرف *enc = CURL_EASY_STR(انکڑے, STRING_ENCODING);
    إذا(enc && !Curl_checkheaders(انکڑے, STRCONST("Accept-Encoding")))
      نتیجہ = curlx_dyn_addf(req, "Accept-Encoding: %s\r\n", enc);
    اكسر;
  }

  حالة H1_HD_REFERER:
    إذا(Curl_bufref_ptr(&انکڑے->state.referer) &&
       !Curl_checkheaders(انکڑے, STRCONST("Referer")))
      نتیجہ = curlx_dyn_addf(req, "Referer: %s\r\n",
                              Curl_bufref_ptr(&انکڑے->state.referer));
    اكسر;

#جے_بنا_تعریف CURL_DISABLE_PROXY
  حالة H1_HD_PROXY_CONNECTION:
    إذا(conn->بٹ.origin_is_proxy &&
       !Curl_checkheaders(انکڑے, STRCONST("Proxy-Connection")) &&
       !Curl_checkProxyheaders(انکڑے, انکڑے->conn, STRCONST("Proxy-Connection")))
      نتیجہ = curlx_dyn_جوڑو(req, "Proxy-Connection: Keep-Alive\r\n");
    اكسر;
#جے_انت

  حالة H1_HD_TRANSFER_ENCODING:
    نتیجہ = http_req_سیٹ_TE(انکڑے, req, httpversion);
    اكسر;

#جے_بنا_تعریف CURL_DISABLE_ALTSVC
  حالة H1_HD_ALT_ورتیا:
    إذا(conn->بٹ.altused && conn->راہیں_peer &&
       !Curl_checkheaders(انکڑے, STRCONST("Alt-Used")))
      نتیجہ = curlx_dyn_addf(req, "Alt-Used: %s:%u\r\n",
                              conn->راہیں_peer->hostname, conn->راہیں_peer->پورٹ);
    اكسر;
#جے_انت

  حالة H1_HD_UPGRADE:
    إذا(!Curl_conn_is_ssl(انکڑے->conn, FIRSTSOCKET) && (httpversion < 20) &&
       (انکڑے->state.http_neg.wanted & CURL_HTTP_V2x) &&
       انکڑے->state.http_neg.h2_upgrade) {
      /* append HTTP2 upgrade magic stuff to the HTTP request if it is not done
         over SSL */
      نتیجہ = Curl_http2_request_upgrade(req, انکڑے);
    }
#جے_بنا_تعریف CURL_DISABLE_WEBSOCKETS
    إذا(!نتیجہ && conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
      نتیجہ = Curl_ws_request(انکڑے, req);
#جے_انت
    اكسر;

  حالة H1_HD_COOKIES:
    نتیجہ = http_cookies(انکڑے, req);
    اكسر;

  حالة H1_HD_CONDITIONALS:
    نتیجہ = Curl_جوڑو_timecondition(انکڑے, req);
    اكسر;

  حالة H1_HD_CUSTOM:
    نتیجہ = Curl_جوڑو_custom_سرخیاں(انکڑے, FALSE, httpversion, req);
    اكسر;

  حالة H1_HD_CONTENT:
    نتیجہ = http_جوڑو_content_hds(انکڑے, req, httpversion, httpreq);
    اكسر;

  حالة H1_HD_CONNECTION: {
    نتیجہ = http_جوڑو_connection_hd(انکڑے, req);
    اكسر;
  }

  حالة H1_HD_آخری:
    نتیجہ = curlx_dyn_addn(req, STRCONST("\r\n"));
    اكسر;
  }
  أرجع نتیجہ;
}

/*
 * Curl_http() gets called from the generic multi_do() function when an HTTP
 * request is to be performed. This creates and sends a properly constructed
 * HTTP request.
 */
CURLcode Curl_http(بنية Curl_easy *انکڑے, bool *پورا)
{
  CURLcode نتیجہ = CURLE_OK;
  Curl_HttpReq httpreq;
  ثابت حرف *طریقہ;
  بنية dynbuf req;
  بنانشانی حرف httpversion;
  size_t hd_id;

  /* Always consider the DO phase done after this function call, even if there
     may be parts of the request that are not yet sent, since we can deal with
     the rest of the request in the PERFORM phase. */
  *پورا = TRUE;
  /* initialize a dynamic send-buffer */
  curlx_dyn_init(&req, DYN_HTTP_REQUEST);
  /* make sure the header buffer is reset - if there are leftovers from a
     previous transfer */
  curlx_dyn_reset(&انکڑے->state.headerb);
  انکڑے->state.maybe_folded = FALSE;

  إذا(!انکڑے->conn->بٹ.reuse) {
    نتیجہ = http_check_new_conn(انکڑے);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  /* Add collecting of headers written to client. For a new connection,
   * we might have done that already, but reuse
   * or multiplex needs it here as well. */
  نتیجہ = Curl_سرخیاں_init(انکڑے);
  إذا(نتیجہ)
    اذهب باہرلا;

  انکڑے->state.http_hd_te = FALSE;
  انکڑے->state.http_hd_upgrade = FALSE;
  انکڑے->state.http_hd_h2_settings = FALSE;

  /* what kind of request do we need to send? */
  Curl_http_طریقہ(انکڑے, &طریقہ, &httpreq);

  /* select host to send */
  نتیجہ = http_سیٹ_aptr_host(انکڑے);
  /* setup the authentication headers, how that method and host are known */
  إذا(!نتیجہ)
    نتیجہ = Curl_http_output_auth(انکڑے, انکڑے->conn, طریقہ, httpreq,
                                   انکڑے->state.اتے.رستہ,
                                   انکڑے->state.اتے.query, FALSE);
  /* Setup input reader, resume information and ranges */
  إذا(!نتیجہ)
    نتیجہ = سیٹ_reader(انکڑے, httpreq);
  إذا(!نتیجہ)
    نتیجہ = http_resume(انکڑے, httpreq);
  إذا(!نتیجہ)
    نتیجہ = http_range(انکڑے, httpreq);
  إذا(نتیجہ)
    اذهب باہرلا;

  httpversion = http_request_نسخہ(انکڑے);
  /* Add request line and all headers to `req` */
  لكل(hd_id = 0; hd_id <= H1_HD_آخری; ++hd_id) {
    نتیجہ = http_جوڑو_hd(انکڑے, &req, (http_hd_t)hd_id,
                         httpversion, طریقہ, httpreq);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  /* setup variables for the upcoming transfer and send */
  Curl_xfer_تیاری_sendrecv(انکڑے, FIRSTSOCKET, -1);
  نتیجہ = Curl_req_send(انکڑے, &req, httpversion);

  إذا((httpversion >= 20) && انکڑے->req.upload_chunky)
    /* upload_chunky was set above to set up the request in a chunky fashion,
       but is disabled here again to avoid that the chunked encoded version is
       actually used when sending the request body over h2 */
    انکڑے->req.upload_chunky = FALSE;

باہرلا:
  إذا(نتیجہ == CURLE_TOO_LARGE)
    failf(انکڑے, "HTTP request too large");

  curlx_dyn_free(&req);
  أرجع نتیجہ;
}

تعريفنوع تعداد {
  حالت_UNKNOWN, /* not enough data to tell yet */
  حالت_پورا, /* a status line was read */
  حالت_BAD /* not a status line */
} statusline;

/* Check a string for a prefix. Check no more than 'len' bytes */
ساكن bool checkprefixmax(ثابت حرف *اگیتر, ثابت حرف *بفر, size_t len)
{
  size_t ch = CURLMIN(لڑی_لمبائی(اگیتر), len);
  أرجع curl_strnequal(اگیتر, بفر, ch);
}

/*
 * checkhttpprefix()
 *
 * Returns TRUE if member of the list matches prefix of string
 */
ساكن statusline checkhttpprefix(بنية Curl_easy *انکڑے,
                                  ثابت حرف *s, size_t len)
{
  بنية curl_slist *سر = انکڑے->سیٹ.http200aliases;
  statusline rc = حالت_BAD;
  statusline onmatch = len >= 5 ? حالت_پورا : حالت_UNKNOWN;

  طالما(سر) {
    إذا(checkprefixmax(سر->انکڑے, s, len)) {
      rc = onmatch;
      اكسر;
    }
    سر = سر->اگلا;
  }

  إذا((rc != حالت_پورا) && checkprefixmax("HTTP/", s, len))
    rc = onmatch;

  أرجع rc;
}

#جے_بنا_تعریف CURL_DISABLE_RTSP
ساكن statusline checkrtspprefix(بنية Curl_easy *انکڑے,
                                  ثابت حرف *s, size_t len)
{
  statusline حالت = حالت_BAD;
  statusline onmatch = len >= 5 ? حالت_پورا : حالت_UNKNOWN;
  (فراغ)انکڑے;
  إذا(checkprefixmax("RTSP/", s, len))
    حالت = onmatch;

  أرجع حالت;
}
#جے_انت /* CURL_DISABLE_RTSP */

ساكن statusline checkprotoprefix(بنية Curl_easy *انکڑے,
                                   بنية connectdata *conn,
                                   ثابت حرف *s, size_t len)
{
#جے_بنا_تعریف CURL_DISABLE_RTSP
  إذا(conn->scheme->protocol & CURLPROTO_RTSP)
    أرجع checkrtspprefix(انکڑے, s, len);
#وإلا
  (فراغ)conn;
#جے_انت /* CURL_DISABLE_RTSP */

  أرجع checkhttpprefix(انکڑے, s, len);
}

/* HTTP header has field name `n` (a string constant) */
#تعریف_کرو HD_IS(hd, hdlen, n) \
  (((hdlen) >= (حجم(n) - 1)) && curl_strnequal(n, hd, حجم(n) - 1))

#تعریف_کرو HD_VAL(hd, hdlen, n) \
  ((((hdlen) >= (حجم(n) - 1)) && (hd) && \
    curl_strnequal(n, hd, حجم(n) - 1)) ? ((hd) + (حجم(n) - 1)) : عدم)

/* HTTP header has field name `n` (a string constant) and contains `v`
 * (a string constant) in its value(s) */
#تعریف_کرو HD_IS_AND_SAYS(hd, hdlen, n, v) \
  (HD_IS(hd, hdlen, n) && \
   ((hdlen) > ((حجم(n) - 1) + (حجم(v) - 1))) && \
   Curl_compareheader(hd, STRCONST(n), STRCONST(v)))

/*
 * http_header_a() parses a single response header starting with A.
 */
ساكن CURLcode http_سرخی_a(بنية Curl_easy *انکڑے,
                              ثابت حرف *hd, size_t hdlen)
{
#جے_بنا_تعریف CURL_DISABLE_ALTSVC
  ثابت حرف *v;
  v = (انکڑے->asi &&
       (Curl_xfer_is_secure(انکڑے) ||
#جے_تعریف_شدہ DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_ALTSVC_HTTP")
#وإلا
        0
#جے_انت
         )) ? HD_VAL(hd, hdlen, "Alt-Svc:") : عدم;
  إذا(v) {
    /* the ALPN of the current request */
    بنية SingleRequest *k = &انکڑے->req;
    تعداد alpnid id = (k->httpversion == 30) ? ALPN_h3 :
      (k->httpversion == 20) ? ALPN_h2 : ALPN_h1;
    أرجع Curl_altsvc_parse(انکڑے, انکڑے->asi, v, انکڑے->state.origin, id);
  }
#وإلا
  (فراغ)انکڑے;
  (فراغ)hd;
  (فراغ)hdlen;
#جے_انت
  أرجع CURLE_OK;
}

/*
 * http_header_c() parses a single response header starting with C.
 */
ساكن CURLcode http_سرخی_c(بنية Curl_easy *انکڑے,
                              ثابت حرف *hd, size_t hdlen)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية SingleRequest *k = &انکڑے->req;
  ثابت حرف *v;

  /* Check for Content-Length: header lines to get size. Browsers insist we
     should accept multiple Content-Length headers and that a comma separated
     list also is fine and then we should accept them all as long as they are
     the same value. Different values trigger error.
   */
  v = (!k->http_bodyless && !انکڑے->سیٹ.ignorecl && !k->ignore_cl) ?
    HD_VAL(hd, hdlen, "Content-Length:") : عدم;
  إذا(v) {
    افعل {
      curl_off_t contentlength;
      صحيح offt = curlx_str_numblanks(&v, &contentlength);

      إذا(offt == STRE_حد_توں_ودھ) {
        /* out of range */
        إذا(انکڑے->سیٹ.max_filesize) {
          failf(انکڑے, "Maximum file size exceeded");
          أرجع CURLE_FILESIZE_EXCEEDED;
        }
        streamclose(conn);
        infof(انکڑے, "Overflow Content-Length: value");
        أرجع CURLE_OK;
      }
      وإلا {
        إذا((offt == STRE_OK) &&
           ((k->ناپ == -1) || /* not set to something before */
            (k->ناپ == contentlength))) { /* or the same value */

          k->ناپ = contentlength;
          curlx_str_passblanks(&v);

          /* on a comma, loop and get the next instead */
          إذا(!curlx_str_single(&v, ','))
            استمر;

          إذا(!curlx_str_newline(&v)) {
            k->maxdownload = k->ناپ;
            أرجع CURLE_OK;
          }
        }
        /* negative, different value or rubbish - bad HTTP */
        failf(انکڑے, "Invalid Content-Length: value");
        أرجع CURLE_WEIRD_SERVER_جواب;
      }
    } طالما(1);
  }
  v = (!k->http_bodyless && CURL_EASY_STR(انکڑے, STRING_ENCODING)) ?
    HD_VAL(hd, hdlen, "Content-Encoding:") : عدم;
  إذا(v) {
    /*
     * Process Content-Encoding. Look for the values: identity, gzip, deflate,
     * compress, x-gzip and x-compress. x-gzip and x-compress are the same as
     * gzip and compress. (Sec 3.5 RFC 2616). zlib cannot handle compress.
     * Errors are handled further down when the response body is processed
     */
    أرجع Curl_build_unencoding_ڈھیر(انکڑے, v, FALSE);
  }
  /* check for Content-Type: header lines to get the MIME-type */
  v = HD_VAL(hd, hdlen, "Content-Type:");
  إذا(v) {
    حرف *contenttype = Curl_copy_سرخی_قدر(hd);
    إذا(!contenttype)
      أرجع CURLE_باہرلا_دا_حافظہ;
    إذا(!*contenttype)
      /* ignore empty data */
      curlx_free(contenttype);
    وإلا {
      curlx_free(انکڑے->info.contenttype);
      انکڑے->info.contenttype = contenttype;
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
    infof(انکڑے, "HTTP/1.0 connection set to keep alive");
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
      إذا(!curlx_str_نمبر(&ptr, &k->وتھ, CURL_OFF_T_MAX) &&
         (انکڑے->state.resume_from == k->وتھ))
        /* we asked for a resume and we got it */
        k->content_range = TRUE;
    }
    وإلا إذا(k->httpcode < 300)
      انکڑے->state.resume_from = 0; /* get everything */
  }
  أرجع CURLE_OK;
}

/*
 * http_header_l() parses a single response header starting with L.
 */
ساكن CURLcode http_سرخی_l(بنية Curl_easy *انکڑے,
                              ثابت حرف *hd, size_t hdlen)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية SingleRequest *k = &انکڑے->req;
  ثابت حرف *v = (!k->http_bodyless &&
                   (انکڑے->سیٹ.timecondition || انکڑے->سیٹ.get_filetime)) ?
    HD_VAL(hd, hdlen, "Last-Modified:") : عدم;
  إذا(v) {
    إذا(Curl_getdate_capped(v, &k->timeofdoc))
      k->timeofdoc = 0;
    إذا(انکڑے->سیٹ.get_filetime)
      انکڑے->info.filetime = k->timeofdoc;
    أرجع CURLE_OK;
  }
  إذا(HD_IS(hd, hdlen, "Location:")) {
    /* this is the URL that the server advises us to use instead */
    حرف *location = Curl_copy_سرخی_قدر(hd);
    إذا(!location)
      أرجع CURLE_باہرلا_دا_حافظہ;
    إذا(!*location ||
       (انکڑے->req.location && !strcmp(انکڑے->req.location, location))) {
      /* ignore empty header, or exact repeat of a previous one */
      curlx_free(location);
      أرجع CURLE_OK;
    }
    وإلا {
      /* has value and is not an exact repeat */
      إذا(انکڑے->req.location) {
        failf(انکڑے, "Multiple Location headers");
        curlx_free(location);
        أرجع CURLE_WEIRD_SERVER_جواب;
      }
      انکڑے->req.location = location;

      إذا((k->httpcode >= 300 && k->httpcode < 400) &&
         انکڑے->سیٹ.http_follow_منوال) {
        CURLcode نتیجہ;
        DEBUGASSERT(!انکڑے->req.newurl);
        انکڑے->req.newurl = curlx_strdup(انکڑے->req.location); /* clone */
        إذا(!انکڑے->req.newurl)
          أرجع CURLE_باہرلا_دا_حافظہ;

        /* some cases of POST and PUT etc needs to rewind the data
           stream at this point */
        نتیجہ = http_perhapsrewind(انکڑے, conn);
        إذا(نتیجہ)
          أرجع نتیجہ;

        /* mark the next request as a followed location: */
        انکڑے->state.this_is_a_follow = TRUE;
      }
    }
  }
  أرجع CURLE_OK;
}

/*
 * http_header_p() parses a single response header starting with P.
 */
ساكن CURLcode http_سرخی_p(بنية Curl_easy *انکڑے,
                              ثابت حرف *hd, size_t hdlen)
{
  بنية SingleRequest *k = &انکڑے->req;

#جے_بنا_تعریف CURL_DISABLE_PROXY
  ثابت حرف *v = HD_VAL(hd, hdlen, "Proxy-Connection:");
  إذا(v) {
    بنية connectdata *conn = انکڑے->conn;
    إذا((k->httpversion == 10) && conn->http_proxy.peer &&
       HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "keep-alive")) {
      /*
       * When an HTTP/1.0 reply comes when using a proxy, the
       * 'Proxy-Connection: keep-alive' line tells us the
       * connection will be kept alive for our pleasure.
       * Default action for 1.0 is to close.
       */
      connkeep(conn); /* do not close */
      infof(انکڑے, "HTTP/1.0 proxy connection set to keep alive");
    }
    وإلا إذا((k->httpversion == 11) && conn->http_proxy.peer &&
            HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "close")) {
      /*
       * We get an HTTP/1.1 response from a proxy and it says it will
       * close down after this transfer.
       */
      connclose(conn);
      infof(انکڑے, "HTTP/1.1 proxy connection set close");
    }
    أرجع CURLE_OK;
  }
#جے_انت
  إذا((407 == k->httpcode) && HD_IS(hd, hdlen, "Proxy-authenticate:")) {
    حرف *auth;
    CURLcode نتیجہ;
#جے_بنا_تعریف CURL_DISABLE_PROXY
    إذا(!انکڑے->conn->بٹ.origin_is_proxy)
      /* this response is not from a forward proxy, not for us */
      أرجع CURLE_OK;
#جے_انت
    auth = Curl_copy_سرخی_قدر(hd);
    نتیجہ = auth ? CURLE_OK : CURLE_باہرلا_دا_حافظہ;
    إذا(!نتیجہ) {
      نتیجہ = Curl_http_داخلہ_auth(انکڑے, TRUE, auth);
      curlx_free(auth);
    }
    أرجع نتیجہ;
  }
#جے_تعریف_شدہ USE_SPNEGO
  إذا(HD_IS(hd, hdlen, "Persistent-Auth:")) {
    بنية connectdata *conn = انکڑے->conn;
    بنية negotiatedata *negdata = Curl_auth_nego_get(conn, FALSE);
    بنية auth *authp = &انکڑے->state.authhost;
    إذا(!negdata)
      أرجع CURLE_باہرلا_دا_حافظہ;
    إذا(authp->picked == CURLAUTH_NEGOTIATE) {
      حرف *persistentauth = Curl_copy_سرخی_قدر(hd);
      إذا(!persistentauth)
        أرجع CURLE_باہرلا_دا_حافظہ;
      negdata->noauthpersist = !!checkprefix("false", persistentauth);
      negdata->havenoauthpersist = TRUE;
      infof(انکڑے, "Negotiate: noauthpersist -> %d, header part: %s",
            negdata->noauthpersist, persistentauth);
      curlx_free(persistentauth);
    }
  }
#جے_انت
  أرجع CURLE_OK;
}

/*
 * http_header_r() parses a single response header starting with R.
 */
ساكن CURLcode http_سرخی_r(بنية Curl_easy *انکڑے,
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
      time_t current = ویلا(عدم);
      إذا(date >= current)
        /* convert date to number of seconds into the future */
        retry_after = date - current;
    }
    وإلا
      /* Try it as a decimal number, ignore errors */
      (فراغ)curlx_str_نمبر(&v, &retry_after, CURL_OFF_T_MAX);
    /* limit to 6 hours max. this is not documented so that it can be changed
       in the future if necessary. */
    إذا(retry_after > 21600)
      retry_after = 21600;
    انکڑے->info.retry_after = retry_after;
  }
  أرجع CURLE_OK;
}

/*
 * http_header_s() parses a single response header starting with S.
 */
ساكن CURLcode http_سرخی_s(بنية Curl_easy *انکڑے,
                              ثابت حرف *hd, size_t hdlen)
{
#إذا !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_HSTS)
  ثابت حرف *v;
#وإلا
  (فراغ)انکڑے;
  (فراغ)hd;
  (فراغ)hdlen;
#جے_انت

#جے_بنا_تعریف CURL_DISABLE_COOKIES
  v = (انکڑے->cookies && انکڑے->state.cookie_engine) ?
    HD_VAL(hd, hdlen, "Set-Cookie:") : عدم;
  إذا(v) {
    /* If there is a custom-set Host: name, use it here, or else use
     * real peer hostname. */
    ثابت حرف *host = انکڑے->req.cookiehost ?
      انکڑے->req.cookiehost : انکڑے->state.origin->hostname;
    ثابت بنانشانی حرف secure_context = Curl_secure_context(انکڑے, host) ?
      COOKIE_SECURE : 0;
    CURLcode نتیجہ;
    Curl_share_جندرا(انکڑے, CURL_جندرا_انکڑے_COOKIE, CURL_جندرا_ACCESS_SINGLE);
    نتیجہ = Curl_cookie_جوڑو(انکڑے, انکڑے->cookies, v, host,
                             انکڑے->state.اتے.رستہ,
                             COOKIE_HTTPHEADER | secure_context);
    Curl_share_unlock(انکڑے, CURL_جندرا_انکڑے_COOKIE);
    أرجع نتیجہ;
  }
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_HSTS
  /* If enabled, the header is incoming and this is over HTTPS */
  v = (انکڑے->hsts &&
       (Curl_xfer_is_secure(انکڑے) ||
#جے_تعریف_شدہ DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_HSTS_HTTP")
#وإلا
        0
#جے_انت
         )
    ) ? HD_VAL(hd, hdlen, "Strict-Transport-Security:") : عدم;
  إذا(v) {
    CURLcode نتیجہ = Curl_hsts_parse(
      انکڑے->hsts, انکڑے->state.origin->hostname, v);
    إذا(نتیجہ) {
      إذا(نتیجہ == CURLE_باہرلا_دا_حافظہ)
        أرجع نتیجہ;
      infof(انکڑے, "Illegal STS header skipped");
    }
#جے_تعریف_شدہ DEBUGBUILD
    وإلا
      infof(انکڑے, "Parsed STS header fine (%zu entries)",
            Curl_llist_گنتی(&انکڑے->hsts->list));
#جے_انت
  }
#جے_انت

  أرجع CURLE_OK;
}

/*
 * http_header_t() parses a single response header starting with T.
 */
ساكن CURLcode http_سرخی_t(بنية Curl_easy *انکڑے,
                              ثابت حرف *hd, size_t hdlen)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية SingleRequest *k = &انکڑے->req;

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
                   (انکڑے->state.httpreq != HTTPREQ_سر) &&
                   (k->httpcode != 304)) ?
    HD_VAL(hd, hdlen, "Transfer-Encoding:") : عدم;
  إذا(v) {
    /* One or more encodings. We check for chunked and/or a compression
       algorithm. */
    CURLcode نتیجہ = Curl_build_unencoding_ڈھیر(انکڑے, v, TRUE);
    إذا(نتیجہ)
      أرجع نتیجہ;
    إذا(!k->chunk) {
      /* if this is not chunked, only close can signal the end of this
       * transfer as Content-Length is said not to be trusted for
       * transfer-encoding! */
      CURL_TRC_M(انکڑے, "HTTP/1.1 transfer-encoding without chunks");
      connclose(conn);
      k->ignore_cl = TRUE;
    }
    أرجع CURLE_OK;
  }
  v = HD_VAL(hd, hdlen, "Trailer:");
  إذا(v) {
    انکڑے->req.resp_trailer = TRUE;
    أرجع CURLE_OK;
  }
  أرجع CURLE_OK;
}

/*
 * http_header_w() parses a single response header starting with W.
 */
ساكن CURLcode http_سرخی_w(بنية Curl_easy *انکڑے,
                              ثابت حرف *hd, size_t hdlen)
{
  بنية SingleRequest *k = &انکڑے->req;
  CURLcode نتیجہ = CURLE_OK;

  إذا((401 == k->httpcode) && HD_IS(hd, hdlen, "WWW-Authenticate:")) {
    حرف *auth = Curl_copy_سرخی_قدر(hd);
    إذا(!auth)
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
    وإلا {
      نتیجہ = Curl_http_داخلہ_auth(انکڑے, FALSE, auth);
      curlx_free(auth);
    }
  }
  أرجع نتیجہ;
}

/*
 * http_header() parses a single response header.
 */
ساكن CURLcode http_سرخی(بنية Curl_easy *انکڑے,
                            ثابت حرف *hd, size_t hdlen)
{
  CURLcode نتیجہ = CURLE_OK;

  بدل(hd[0]) {
  حالة 'a':
  حالة 'A':
    نتیجہ = http_سرخی_a(انکڑے, hd, hdlen);
    اكسر;
  حالة 'c':
  حالة 'C':
    نتیجہ = http_سرخی_c(انکڑے, hd, hdlen);
    اكسر;
  حالة 'l':
  حالة 'L':
    نتیجہ = http_سرخی_l(انکڑے, hd, hdlen);
    اكسر;
  حالة 'p':
  حالة 'P':
    نتیجہ = http_سرخی_p(انکڑے, hd, hdlen);
    اكسر;
  حالة 'r':
  حالة 'R':
    نتیجہ = http_سرخی_r(انکڑے, hd, hdlen);
    اكسر;
  حالة 's':
  حالة 'S':
    نتیجہ = http_سرخی_s(انکڑے, hd, hdlen);
    اكسر;
  حالة 't':
  حالة 'T':
    نتیجہ = http_سرخی_t(انکڑے, hd, hdlen);
    اكسر;
  حالة 'w':
  حالة 'W':
    نتیجہ = http_سرخی_w(انکڑے, hd, hdlen);
    اكسر;
  }

  إذا(!نتیجہ) {
    بنية connectdata *conn = انکڑے->conn;
    إذا(conn->scheme->protocol & CURLPROTO_RTSP)
      نتیجہ = Curl_rtsp_parseheader(انکڑے, hd);
  }
  أرجع نتیجہ;
}

/*
 * Called after the first HTTP response line (the status line) has been
 * received and parsed.
 */
ساكن CURLcode http_statusline(بنية Curl_easy *انکڑے,
                                بنية connectdata *conn)
{
  بنية SingleRequest *k = &انکڑے->req;

  بدل(k->httpversion) {
  حالة 10:
  حالة 11:
#جے_تعریف_شدہ USE_HTTP2
  حالة 20:
#جے_انت
#جے_تعریف_شدہ USE_HTTP3
  حالة 30:
#جے_انت
    /* no major version switch mid-connection */
    إذا(k->httpversion_گھلیا &&
       (k->httpversion / 10 != k->httpversion_گھلیا / 10)) {
      failf(انکڑے, "Version mismatch (from HTTP/%d to HTTP/%d)",
            k->httpversion_گھلیا / 10, k->httpversion / 10);
      أرجع CURLE_WEIRD_SERVER_جواب;
    }
    اكسر;
  افتراضي:
    failf(انکڑے, "Unsupported HTTP version (%d.%d) in response",
          k->httpversion / 10, k->httpversion % 10);
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  }

  انکڑے->info.httpcode = k->httpcode;
  انکڑے->info.httpversion = k->httpversion;
  conn->httpversion_تکیا = k->httpversion;

  إذا(!انکڑے->state.http_neg.rcvd_min ||
     انکڑے->state.http_neg.rcvd_min > k->httpversion)
    /* store the lowest server version we encounter */
    انکڑے->state.http_neg.rcvd_min = k->httpversion;

  /*
   * This code executes as part of processing the header. As a
   * result, it is not totally clear how to interpret the
   * response code yet as that depends on what other headers may
   * be present. 401 and 407 may be errors, but may be OK
   * depending on how authentication is working. Other codes
   * are definitely errors, so give up here.
   */
  إذا(انکڑے->state.resume_from && انکڑے->state.httpreq == HTTPREQ_GET &&
     k->httpcode == 416) {
    /* "Requested Range Not Satisfiable", proceed and pretend this is no
       error */
    k->ignorebody = TRUE; /* Avoid appending error msg to good data. */
  }

  إذا(k->httpversion == 10) {
    /* Default action for HTTP/1.0 must be to close, unless
       we get one of those fancy headers that tell us the
       server keeps it open for us! */
    infof(انکڑے, "HTTP 1.0, assume close after body");
    connclose(conn);
  }

  k->http_bodyless = k->httpcode >= 100 && k->httpcode < 200;
  بدل(k->httpcode) {
  حالة 304:
    /* (quote from RFC2616, section 10.3.5): The 304 response
     * MUST NOT contain a message-body, and thus is always
     * terminated by the first empty line after the header
     * fields. */
    إذا(انکڑے->سیٹ.timecondition)
      انکڑے->info.timecond = TRUE;
    FALLTHROUGH();
  حالة 204:
    /* (quote from RFC2616, section 10.2.5): The server has
     * fulfilled the request but does not need to return an
     * entity-body ... The 204 response MUST NOT include a
     * message-body, and thus is always terminated by the first
     * empty line after the header fields. */
    k->ناپ = 0;
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
ساكن CURLcode http_ناپ(بنية Curl_easy *انکڑے)
{
  بنية SingleRequest *k = &انکڑے->req;
  إذا(انکڑے->req.ignore_cl || k->chunk) {
    k->ناپ = k->maxdownload = -1;
  }
  وإلا إذا(k->ناپ != -1) {
    إذا(انکڑے->سیٹ.max_filesize &&
       !k->ignorebody &&
       (k->ناپ > انکڑے->سیٹ.max_filesize)) {
      failf(انکڑے, "Maximum file size exceeded");
      أرجع CURLE_FILESIZE_EXCEEDED;
    }
    إذا(k->ignorebody)
      infof(انکڑے, "setting size while ignoring");
    Curl_pgrs_سیٹ_Download_ناپ(انکڑے, k->ناپ);
    k->maxdownload = k->ناپ;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_verify_سرخی(بنية Curl_easy *انکڑے,
                            ثابت حرف *hd, size_t hdlen)
{
  بنية SingleRequest *k = &انکڑے->req;
  size_t i;
  bool has_colon = FALSE;
  لكل(i = 0; i < hdlen; i++) {
    بدل(hd[i]) {
    حالة '\0':
      /* this is bad, bail out */
      failf(انکڑے, "NUL byte in header");
      أرجع CURLE_WEIRD_SERVER_جواب;
    حالة '\r':
      إذا(i < hdlen - 2) {
        /* CR may only precede the LF, nothing else */
        failf(انکڑے, "Carriage return found in header");
        أرجع CURLE_WEIRD_SERVER_جواب;
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
    failf(انکڑے, "Header without colon");
    أرجع CURLE_WEIRD_SERVER_جواب;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_bump_headersize(بنية Curl_easy *انکڑے,
                              size_t delta,
                              bool connect_only)
{
  size_t bad = 0;
  بنانشانی صحيح max = MAX_HTTP_RESP_سرخی_ناپ;
  إذا(delta < MAX_HTTP_RESP_سرخی_ناپ) {
    انکڑے->info.سرخی_ناپ += (بنانشانی صحيح)delta;
    انکڑے->req.allheadercount += (بنانشانی صحيح)delta;
    إذا(!connect_only)
      انکڑے->req.headerbytecount += (بنانشانی صحيح)delta;
    إذا(انکڑے->req.allheadercount > max)
      bad = انکڑے->req.allheadercount;
    وإلا إذا(انکڑے->info.سرخی_ناپ > (max * 20)) {
      bad = انکڑے->info.سرخی_ناپ;
      max *= 20;
    }
  }
  وإلا
    bad = انکڑے->req.allheadercount + delta;
  إذا(bad) {
    failf(انکڑے, "Too large response headers: %zu > %u", bad, max);
    أرجع CURLE_RECV_ERROR;
  }
  أرجع CURLE_OK;
}

/*
 * Handle a 101 Switching Protocols response. Performs the actual protocol
 * upgrade to HTTP/2 or WebSocket based on what was requested.
 */
ساكن CURLcode http_on_101_upgrade(بنية Curl_easy *انکڑے,
                                    ثابت حرف *buf, size_t blen,
                                    size_t *pconsumed,
                                    bool *conn_بدلیا)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية SingleRequest *k = &انکڑے->req;

#إذا !defined(USE_NGHTTP2) && defined(CURL_DISABLE_WEBSOCKETS)
  (فراغ)buf;
  (فراغ)blen;
  (فراغ)pconsumed;
#وإلا
  CURLcode نتیجہ;
  صحيح upgr101_requested = k->upgr101;
#جے_انت

  إذا(k->httpversion_گھلیا != 11) {
    /* invalid for other HTTP versions */
    failf(انکڑے, "server sent 101 response while not talking HTTP/1.1");
    أرجع CURLE_WEIRD_SERVER_جواب;
  }

  /* Whatever the success, upgrade was selected. */
  k->upgr101 = UPGR101_RECEIVED;
  conn->بٹ.upgrade_وچ_ترقی = FALSE;
  *conn_بدلیا = TRUE;

  /* To be fully compliant, we would check the "Upgrade:" response header to
   * mention the protocol we requested. */
#جے_تعریف_شدہ USE_NGHTTP2
  إذا(upgr101_requested == UPGR101_H2) {
    /* Switch to HTTP/2, where we will get more responses. blen bytes in buf
     * are already h2 protocol bytes */
    infof(انکڑے, "Received 101, Switching to HTTP/2");
    نتیجہ = Curl_http2_upgrade(انکڑے, conn, FIRSTSOCKET, buf, blen);
    إذا(!نتیجہ)
      *pconsumed += blen;
    أرجع نتیجہ;
  }
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_WEBSOCKETS
  إذا(upgr101_requested == UPGR101_WS) {
    /* Switch to WebSocket, where we now stream ws frames. blen bytes in buf
     * are already ws protocol bytes */
    infof(انکڑے, "Received 101, Switching to WebSocket");
    نتیجہ = Curl_ws_accept(انکڑے, buf, blen);
    إذا(!نتیجہ)
      *pconsumed += blen; /* ws accept handled the data */
    أرجع نتیجہ;
  }
#جے_انت
  /* We silently accept this as the final response. What are we switching to
   * if we did not ask for an Upgrade? Maybe the application provided an
   * `Upgrade: xxx` header? */
  k->سرخی = FALSE;
  أرجع CURLE_OK;
}

/*
 * Handle 1xx intermediate HTTP responses. Sets up state for more
 * headers and processes 100-continue and 101 upgrade responses.
 */
ساكن CURLcode http_on_1xx_response(بنية Curl_easy *انکڑے,
                                     ثابت حرف *buf, size_t blen,
                                     size_t *pconsumed,
                                     bool *conn_بدلیا)
{
  بنية SingleRequest *k = &انکڑے->req;

  /* "A user agent MAY ignore unexpected 1xx status responses."
   * By default, we expect to get more responses after this one. */
  k->سرخی = TRUE;
  k->headerline = 0; /* restart the header line counter */

  بدل(k->httpcode) {
  حالة 100:
    /* We have made an HTTP PUT or POST and this is 1.1-lingo that tells us
     * that the server is OK with this and ready to receive the data. */
    http_exp100_got100(انکڑے);
    اكسر;
  حالة 101:
    أرجع http_on_101_upgrade(انکڑے, buf, blen, pconsumed, conn_بدلیا);
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
ساكن فراغ http_check_auth_گھیرا(بنية Curl_easy *انکڑے,
                                    بنية connectdata *conn)
{
  /* At this point we have some idea about the fate of the connection. If we
     are closing the connection it may result auth failure. */
#جے_تعریف_شدہ USE_NTLM
  إذا(conn->بٹ.close &&
     (((انکڑے->req.httpcode == 401) &&
       (conn->http_ntlm_state == NTLMSTATE_TYPE2)) ||
      ((انکڑے->req.httpcode == 407) &&
       (conn->proxy_ntlm_state == NTLMSTATE_TYPE2)))) {
    infof(انکڑے, "Connection closure while negotiating auth (HTTP 1.0?)");
    انکڑے->state.authproblem = TRUE;
  }
#جے_انت
#جے_تعریف_شدہ USE_SPNEGO
  إذا(conn->بٹ.close &&
    (((انکڑے->req.httpcode == 401) &&
      (conn->http_negotiate_state == CURL_NEGO_AUTH_RECV)) ||
     ((انکڑے->req.httpcode == 407) &&
      (conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)))) {
    infof(انکڑے, "Connection closure while negotiating auth (HTTP 1.0?)");
    انکڑے->state.authproblem = TRUE;
  }
  إذا((conn->http_negotiate_state == CURL_NEGO_AUTH_پورا) &&
     (انکڑے->req.httpcode != 401)) {
    conn->http_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
  إذا((conn->proxy_negotiate_state == CURL_NEGO_AUTH_پورا) &&
     (انکڑے->req.httpcode != 407)) {
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
#جے_انت
}
#وإلا
#تعریف_کرو http_check_auth_گھیرا(x, y) /*خالی_تھاواں*/
#جے_انت

/*
 * Handle an error response (>= 300) received while still sending the
 * request body. Deals with 417 Expectation Failed retries, keep-sending
 * on error, and aborting the send.
 */
ساكن CURLcode http_handle_send_error(بنية Curl_easy *انکڑے)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية SingleRequest *k = &انکڑے->req;
  CURLcode نتیجہ = CURLE_OK;

  إذا(!انکڑے->req.authneg && !conn->بٹ.close &&
     !Curl_creader_will_rewind(انکڑے)) {
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

    بدل(انکڑے->state.httpreq) {
    حالة HTTPREQ_PUT:
    حالة HTTPREQ_POST:
    حالة HTTPREQ_POST_FORM:
    حالة HTTPREQ_POST_MIME:
      /* We got an error response. If this happened before the
       * whole request body has been sent we stop sending and
       * mark the connection for closure after we have read the
       * entire response. */
      إذا(!Curl_req_پورا_sending(انکڑے)) {
        إذا((k->httpcode == 417) && http_exp100_is_selected(انکڑے)) {
          /* 417 Expectation Failed - try again without the
             Expect header */
          إذا(!k->writebytecount && http_exp100_is_اڈیک(انکڑے)) {
            infof(انکڑے, "Got HTTP failure 417 while waiting for a 100");
          }
          وإلا {
            infof(انکڑے, "Got HTTP failure 417 while sending data");
            streamclose(conn);
            نتیجہ = http_perhapsrewind(انکڑے, conn);
            إذا(نتیجہ)
              أرجع نتیجہ;
          }
          انکڑے->state.disableexpect = TRUE;
          Curl_req_abort_sending(انکڑے);
          DEBUGASSERT(!انکڑے->req.newurl);
          انکڑے->req.newurl = Curl_bufref_dup(&انکڑے->state.url);
          إذا(!انکڑے->req.newurl)
            أرجع CURLE_باہرلا_دا_حافظہ;
        }
        وإلا إذا(انکڑے->سیٹ.http_keep_sending_on_error) {
          infof(انکڑے, "HTTP error before end of send, keep sending");
          http_exp100_send_anyway(انکڑے);
        }
        وإلا {
          infof(انکڑے, "HTTP error before end of send, stop sending");
          streamclose(conn);
          نتیجہ = Curl_req_abort_sending(انکڑے);
          إذا(نتیجہ)
            أرجع نتیجہ;
        }
      }
      اكسر;

    افتراضي: /* default label present to avoid compiler warnings */
      اكسر;
    }
  }

  إذا(Curl_creader_will_rewind(انکڑے) && !Curl_req_پورا_sending(انکڑے)) {
    /* We rewind before next send, continue sending now */
    infof(انکڑے, "Keep sending data to get tossed away");
    CURL_REQ_سیٹ_SEND(انکڑے);
  }
  أرجع نتیجہ;
}

ساكن CURLcode http_on_response(بنية Curl_easy *انکڑے,
                                 ثابت حرف *آخری_hd, size_t آخری_hd_len,
                                 ثابت حرف *buf, size_t blen,
                                 size_t *pconsumed)
{
  بنية connectdata *conn = انکڑے->conn;
  CURLcode نتیجہ = CURLE_OK;
  بنية SingleRequest *k = &انکڑے->req;
  bool conn_بدلیا = FALSE;

  (فراغ)buf; /* not used without HTTP2 enabled */
  *pconsumed = 0;

  إذا(k->upgr101 == UPGR101_RECEIVED) {
    /* supposedly upgraded to http2 now */
    إذا(انکڑے->req.httpversion != 20)
      infof(انکڑے, "Lying server, not serving HTTP/2");
  }

  إذا(k->httpcode < 200 && آخری_hd) {
    /* Intermediate responses might trigger processing of more responses,
     * write the last header to the client before proceeding. */
    نتیجہ = http_write_سرخی(انکڑے, آخری_hd, آخری_hd_len);
    آخری_hd = عدم; /* handled it */
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  إذا(k->httpcode < 100) {
    failf(انکڑے, "Unsupported response code in HTTP response");
    نتیجہ = CURLE_UNSUPPORTED_PROTOCOL;
    اذهب باہرلا;
  }
  وإلا إذا(k->httpcode < 200) {
    نتیجہ = http_on_1xx_response(انکڑے, buf, blen, pconsumed, &conn_بدلیا);
    اذهب باہرلا;
  }

  /* k->httpcode >= 200, final response */
  k->سرخی = FALSE;
  إذا(conn->بٹ.upgrade_وچ_ترقی) {
    /* Asked for protocol upgrade, but it was not selected */
    conn->بٹ.upgrade_وچ_ترقی = FALSE;
    conn_بدلیا = TRUE;
  }

  إذا((k->ناپ == -1) && !k->chunk && !conn->بٹ.close &&
     (k->httpversion == 11) &&
     !(conn->scheme->protocol & CURLPROTO_RTSP) &&
     انکڑے->state.httpreq != HTTPREQ_سر) {
    /* On HTTP 1.1, when connection is not to get closed, but no
       Content-Length nor Transfer-Encoding chunked have been received,
       according to RFC2616 section 4.4 point 5, we assume that the server
       will close the connection to signal the end of the document. */
    infof(انکڑے, "no chunk, no close, no size. Assume close to signal end");
    streamclose(conn);
  }

  http_check_auth_گھیرا(انکڑے, conn);

#جے_بنا_تعریف CURL_DISABLE_WEBSOCKETS
  /* All >=200 HTTP status codes are errors when wanting ws */
  إذا(انکڑے->req.upgr101 == UPGR101_WS) {
    failf(انکڑے, "Refused WebSocket upgrade: %d", k->httpcode);
    نتیجہ = CURLE_HTTP_RETURNED_ERROR;
    اذهب باہرلا;
  }
#جے_انت

  /* Check if this response means the transfer errored. */
  إذا(http_should_fail(انکڑے, انکڑے->req.httpcode)) {
    failf(انکڑے, "The requested URL returned error: %d",
          k->httpcode);
    نتیجہ = CURLE_HTTP_RETURNED_ERROR;
    اذهب باہرلا;
  }

  /* Curl_http_auth_act() checks what authentication methods that are
   * available and decides which one (if any) to use. It will set 'newurl' if
   * an auth method was picked. */
  نتیجہ = Curl_http_auth_act(انکڑے);
  إذا(نتیجہ)
    اذهب باہرلا;

  إذا(k->httpcode >= 300) {
    نتیجہ = http_handle_send_error(انکڑے);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  /* final response without error, prepare to receive the body */
  نتیجہ = http_firstwrite(انکڑے);
  إذا(نتیجہ)
    اذهب باہرلا;

  /* This is the last response that we get for the current request. Check on
   * the body size and determine if the response is complete. */
  نتیجہ = http_ناپ(انکڑے);
  إذا(نتیجہ)
    اذهب باہرلا;

#جے_بنا_تعریف CURL_DISABLE_RTSP
  /* An RTSP response to any method may carry a body, which is announced
     with Content-Length (RFC 2326 section 12.14). Read it instead of
     treating it as excess data, unless the application asked for no body
     with CURLOPT_NOBODY. */
  إذا((conn->scheme->protocol & CURLPROTO_RTSP) && انکڑے->req.no_body &&
     !انکڑے->سیٹ.opt_no_body && (k->ناپ > 0))
    انکڑے->req.no_body = FALSE;
#جے_انت

  /* If we requested a "no body", this is a good time to get
   * out and return home.
   */
  إذا(انکڑے->req.no_body)
    k->download_پورا = TRUE;

  /* If max download size is *zero* (nothing) we already have nothing and can
     safely return ok now! For HTTP/2, we would like to call
     http2_handle_stream_close to properly close a stream. In order to do
     this, we keep reading until we close the stream. */
  إذا((k->maxdownload == 0) && (k->httpversion_گھلیا < 20))
    k->download_پورا = TRUE;

باہرلا:
  إذا(آخری_hd)
    /* if not written yet, write it now */
    نتیجہ = Curl_1st_fatal(نتیجہ,
                            http_write_سرخی(انکڑے, آخری_hd, آخری_hd_len));
  إذا(conn_بدلیا)
    /* poke the multi handle to allow pending pipewait to retry */
    Curl_multi_connchanged(انکڑے->multi);
  أرجع نتیجہ;
}

ساكن CURLcode http_rw_hd(بنية Curl_easy *انکڑے,
                           ثابت حرف *hd, size_t hdlen,
                           ثابت حرف *buf_remain, size_t blen,
                           size_t *pconsumed)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية SingleRequest *k = &انکڑے->req;
  صحيح writetype;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  *pconsumed = 0;
  إذا((0x0a == *hd) || (0x0d == *hd)) {
    /* Empty header line means end of headers! */
    بنية dynbuf آخری_سرخی;
    size_t consumed;

    curlx_dyn_init(&آخری_سرخی, hdlen + 1);
    نتیجہ = curlx_dyn_addn(&آخری_سرخی, hd, hdlen);
    إذا(نتیجہ)
      أرجع نتیجہ;

    /* analyze the response to find out what to do. */
    /* Caveat: we clear anything in the header brigade, because a
     * response might switch HTTP version which may call use recursively.
     * Not nice, but that is currently the way of things. */
    curlx_dyn_reset(&انکڑے->state.headerb);
    نتیجہ = http_on_response(انکڑے, curlx_dyn_ptr(&آخری_سرخی),
                              curlx_dyn_len(&آخری_سرخی),
                              buf_remain, blen, &consumed);
    *pconsumed += consumed;
    curlx_dyn_free(&آخری_سرخی);
    أرجع نتیجہ;
  }

  /*
   * Checks for special headers coming up.
   */

  writetype = CLIENTWRITE_سرخی;
  إذا(!k->headerline++) {
    /* This is the first header, it MUST be the error code line
       or else we consider this to be the body right away! */
    bool fine_statusline = FALSE;

    k->httpversion = 0; /* Do not know yet */
    إذا(انکڑے->conn->scheme->protocol & PROTO_FAMILY_HTTP) {
      /*
       * https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.2
       *
       * The response code is always a three-digit number in HTTP as the spec
       * says. We allow any three-digit number here, but we cannot make
       * guarantees on future behaviors since it is not within the protocol.
       */
      ثابت حرف *p = hd;

      curlx_str_passblanks(&p);
      إذا(!لڑی_مقابلہ(p, "HTTP/", 5)) {
        p += 5;
        بدل(*p) {
        حالة '1':
          p++;
          إذا((p[0] == '.') && (p[1] == '0' || p[1] == '1')) {
            إذا(ISBLANK(p[2])) {
              k->httpversion = (بنانشانی حرف)(10 + (p[1] - '0'));
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
            failf(انکڑے, "Unsupported HTTP/1 subversion in response");
            أرجع CURLE_UNSUPPORTED_PROTOCOL;
          }
          اكسر;
        حالة '2':
        حالة '3':
          إذا(!ISBLANK(p[1]))
            اكسر;
          k->httpversion = (بنانشانی حرف)((*p - '0') * 10);
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
          failf(انکڑے, "Unsupported HTTP version in response");
          أرجع CURLE_UNSUPPORTED_PROTOCOL;
        }
      }

      إذا(!fine_statusline) {
        /* If user has set option HTTP200ALIASES,
           compare header line against list of aliases */
        statusline check = checkhttpprefix(انکڑے, hd, hdlen);
        إذا(check == حالت_پورا) {
          fine_statusline = TRUE;
          k->httpcode = 200;
          k->httpversion = 10;
        }
      }
    }
    وإلا إذا(انکڑے->conn->scheme->protocol & CURLPROTO_RTSP) {
      ثابت حرف *p = hd;
      بنية Curl_str ver;
      curl_off_t حالت;
      /* we set the max string a little excessive to forgive some leading
         spaces */
      إذا(!curlx_str_until(&p, &ver, 32, ' ') &&
         !curlx_str_single(&p, ' ') &&
         !curlx_str_نمبر(&p, &حالت, 999)) {
        curlx_str_trimblanks(&ver);
        إذا(curlx_str_cmp(&ver, "RTSP/1.0")) {
          k->httpcode = (صحيح)حالت;
          fine_statusline = TRUE;
          k->httpversion = 11; /* RTSP acts like HTTP 1.1 */
        }
      }
      إذا(!fine_statusline)
        أرجع CURLE_WEIRD_SERVER_جواب;
    }

    إذا(fine_statusline) {
      نتیجہ = http_statusline(انکڑے, انکڑے->conn);
      إذا(نتیجہ)
        أرجع نتیجہ;
      writetype |= CLIENTWRITE_حالت;
    }
    وإلا {
      k->سرخی = FALSE;   /* this is not a header line */
      أرجع CURLE_WEIRD_SERVER_جواب;
    }
  }

  نتیجہ = Curl_verify_سرخی(انکڑے, hd, hdlen);
  إذا(نتیجہ)
    أرجع نتیجہ;

  نتیجہ = http_سرخی(انکڑے, hd, hdlen);
  إذا(نتیجہ)
    أرجع نتیجہ;

  /*
   * Taken in one (more) header. Write it to the client.
   */
  Curl_debug(انکڑے, CURLINFO_سرخی_وچ, hd, hdlen);

  إذا(k->httpcode / 100 == 1)
    writetype |= CLIENTWRITE_1XX;
  نتیجہ = Curl_client_write(انکڑے, writetype, hd, hdlen);
  إذا(نتیجہ)
    أرجع نتیجہ;

  نتیجہ = Curl_bump_headersize(انکڑے, hdlen, FALSE);
  إذا(نتیجہ)
    أرجع نتیجہ;

  أرجع CURLE_OK;
}

/* remove trailing CRLF then all trailing whitespace */
فراغ Curl_http_منزل_fold(بنية dynbuf *bf)
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

ساكن فراغ unfold_سرخی(بنية Curl_easy *انکڑے)
{
  Curl_http_منزل_fold(&انکڑے->state.headerb);
  انکڑے->state.leading_unfold = TRUE;
}

/*
 * Read any HTTP header lines from the server and pass them to the client app.
 */
ساكن CURLcode http_parse_سرخیاں(بنية Curl_easy *انکڑے,
                                   ثابت حرف *buf, size_t blen,
                                   size_t *pconsumed)
{
  بنية connectdata *conn = انکڑے->conn;
  CURLcode نتیجہ = CURLE_OK;
  بنية SingleRequest *k = &انکڑے->req;
  ثابت حرف *سرا_ptr;
  bool leftover_body = FALSE;

  /* we have bytes for the next header, make sure it is not a folded header
     before passing it on */
  إذا(انکڑے->state.maybe_folded && blen) {
    إذا(ISBLANK(buf[0])) {
      /* folded, remove the trailing newlines and append the next header */
      unfold_سرخی(انکڑے);
    }
    وإلا {
      /* the header data we hold is a complete header, pass it on */
      size_t ignore_this;
      نتیجہ = http_rw_hd(انکڑے, curlx_dyn_ptr(&انکڑے->state.headerb),
                          curlx_dyn_len(&انکڑے->state.headerb),
                          عدم, 0, &ignore_this);
      curlx_dyn_reset(&انکڑے->state.headerb);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
    انکڑے->state.maybe_folded = FALSE;
  }

  /* header line within buffer loop */
  *pconsumed = 0;
  طالما(blen && k->سرخی) {
    size_t consumed;
    size_t hlen;
    ثابت حرف *hd;
    size_t unfold_len = 0;

    إذا(انکڑے->state.leading_unfold) {
      /* immediately after an unfold, keep only a single whitespace */
      طالما(blen && ISBLANK(buf[0])) {
        buf++;
        blen--;
        unfold_len++;
      }
      إذا(blen) {
        /* insert a single space */
        نتیجہ = curlx_dyn_addn(&انکڑے->state.headerb, " ", 1);
        إذا(نتیجہ)
          أرجع نتیجہ;
        انکڑے->state.leading_unfold = FALSE; /* done now */
      }
    }

    سرا_ptr = memchr(buf, '\n', blen);
    إذا(!سرا_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      نتیجہ = curlx_dyn_addn(&انکڑے->state.headerb, buf, blen);
      إذا(نتیجہ)
        أرجع نتیجہ;
      *pconsumed += blen + unfold_len;

      إذا(!k->headerline) {
        /* check if this looks like a protocol header */
        statusline st =
          checkprotoprefix(انکڑے, conn,
                           curlx_dyn_ptr(&انکڑے->state.headerb),
                           curlx_dyn_len(&انکڑے->state.headerb));

        إذا(st == حالت_BAD) {
          /* this is not the beginning of a protocol first header line.
           * Cannot be 0.9 if version was detected or connection was reused. */
          k->سرخی = FALSE;
          streamclose(conn);
          إذا((k->httpversion >= 10) || conn->بٹ.reuse) {
            failf(انکڑے, "Invalid status line");
            أرجع CURLE_WEIRD_SERVER_جواب;
          }
          إذا(!انکڑے->state.http_neg.accept_09) {
            failf(انکڑے, "Received HTTP/0.9 when not allowed");
            أرجع CURLE_UNSUPPORTED_PROTOCOL;
          }
          leftover_body = TRUE;
          اذهب باہرلا;
        }
      }
      اذهب باہرلا; /* read more and try again */
    }

    /* the size of the remaining header line */
    consumed = (سرا_ptr - buf) + 1;

    نتیجہ = curlx_dyn_addn(&انکڑے->state.headerb, buf, consumed);
    إذا(نتیجہ)
      أرجع نتیجہ;
    blen -= consumed;
    buf += consumed;
    *pconsumed += consumed + unfold_len;

    /****
     * We now have a FULL header line in 'headerb'.
     *****/

    hlen = curlx_dyn_len(&انکڑے->state.headerb);
    hd = curlx_dyn_ptr(&انکڑے->state.headerb);

    إذا(!k->headerline) {
      /* the first read "header", the status line */
      statusline st = checkprotoprefix(انکڑے, conn, hd, hlen);
      إذا(st == حالت_BAD) {
        streamclose(conn);
        /* this is not the beginning of a protocol first header line.
         * Cannot be 0.9 if version was detected or connection was reused. */
        إذا((k->httpversion >= 10) || conn->بٹ.reuse) {
          failf(انکڑے, "Invalid status line");
          أرجع CURLE_WEIRD_SERVER_جواب;
        }
        إذا(!انکڑے->state.http_neg.accept_09) {
          failf(انکڑے, "Received HTTP/0.9 when not allowed");
          أرجع CURLE_UNSUPPORTED_PROTOCOL;
        }
        k->سرخی = FALSE;
        leftover_body = TRUE;
        اذهب باہرلا;
      }
    }
    وإلا {
      إذا(hlen && !ISNEWLINE(hd[0])) {
        /* this is NOT the header separator */

        /* if we have bytes for the next header, check for folding */
        إذا(blen && ISBLANK(buf[0])) {
          /* remove the trailing CRLF and append the next header */
          unfold_سرخی(انکڑے);
          استمر;
        }
        وإلا إذا(!blen) {
          /* this might be a folded header so deal with it in next invoke */
          انکڑے->state.maybe_folded = TRUE;
          اكسر;
        }
      }
    }

    نتیجہ = http_rw_hd(انکڑے, hd, hlen, buf, blen, &consumed);
    /* We are done with this line. We reset because response
     * processing might switch to HTTP/2 and that might call us
     * directly again. */
    curlx_dyn_reset(&انکڑے->state.headerb);
    إذا(consumed) {
      blen -= consumed;
      buf += consumed;
      *pconsumed += consumed;
    }
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */
باہرلا:
  إذا(!k->سرخی && !leftover_body) {
    curlx_dyn_free(&انکڑے->state.headerb);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_http_write_resp_hd(بنية Curl_easy *انکڑے,
                                 ثابت حرف *hd, size_t hdlen,
                                 bool is_eos)
{
  CURLcode نتیجہ;
  size_t consumed;
  حرف tmp = 0;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  نتیجہ = http_rw_hd(انکڑے, hd, hdlen, &tmp, 0, &consumed);
  إذا(!نتیجہ && is_eos) {
    نتیجہ = Curl_client_write(انکڑے, (CLIENTWRITE_BODY | CLIENTWRITE_EOS),
                               &tmp, 0);
  }
  أرجع نتیجہ;
}

/*
 * HTTP protocol `write_resp` implementation. Parse headers
 * when not done yet and otherwise return without consuming data.
 */
CURLcode Curl_http_write_resp_hds(بنية Curl_easy *انکڑے,
                                  ثابت حرف *buf, size_t blen,
                                  size_t *pconsumed)
{
  إذا(!انکڑے->req.سرخی) {
    *pconsumed = 0;
    أرجع CURLE_OK;
  }
  وإلا {
    CURLcode نتیجہ;

    نتیجہ = http_parse_سرخیاں(انکڑے, buf, blen, pconsumed);
    إذا(!نتیجہ && !انکڑے->req.سرخی) {
      إذا(!انکڑے->req.no_body && curlx_dyn_len(&انکڑے->state.headerb)) {
        /* leftover from parsing something that turned out not
         * to be a header, only happens if we allow for
         * HTTP/0.9 like responses */
        نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY,
                                   curlx_dyn_ptr(&انکڑے->state.headerb),
                                   curlx_dyn_len(&انکڑے->state.headerb));
      }
      curlx_dyn_free(&انکڑے->state.headerb);
    }
    أرجع نتیجہ;
  }
}

CURLcode Curl_http_write_resp(بنية Curl_easy *انکڑے,
                              ثابت حرف *buf, size_t blen,
                              bool is_eos)
{
  CURLcode نتیجہ;
  size_t consumed;
  صحيح flags;

  نتیجہ = Curl_http_write_resp_hds(انکڑے, buf, blen, &consumed);
  إذا(نتیجہ || انکڑے->req.پورا)
    اذهب باہرلا;

  DEBUGASSERT(consumed <= blen);
  blen -= consumed;
  buf += consumed;
  /* either all was consumed in header parsing, or we have data left
   * and are done with headers, e.g. it is BODY data */
  DEBUGASSERT(!blen || !انکڑے->req.سرخی);
  إذا(!انکڑے->req.سرخی && (blen || is_eos)) {
    /* BODY data after header been parsed, write and consume */
    flags = CLIENTWRITE_BODY;
    إذا(is_eos)
      flags |= CLIENTWRITE_EOS;
    نتیجہ = Curl_client_write(انکڑے, flags, buf, blen);
  }
باہرلا:
  أرجع نتیجہ;
}

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_حالت(int32_t *pstatus, ثابت حرف *s, size_t len)
{
  CURLcode نتیجہ = CURLE_BAD_FUNCTION_دلیل;
  int32_t حالت = 0;
  صحيح i;

  إذا(len != 3)
    اذهب باہرلا;

  لكل(i = 0; i < 3; ++i) {
    حرف c = s[i];

    إذا(c < '0' || c > '9')
      اذهب باہرلا;

    حالت *= 10;
    حالت += c - '0';
  }
  نتیجہ = CURLE_OK;
باہرلا:
  *pstatus = نتیجہ ? -1 : حالت;
  أرجع نتیجہ;
}

CURLcode Curl_http_req_make(بنية httpreq **preq,
                            ثابت حرف *طریقہ, size_t m_len,
                            ثابت حرف *scheme, size_t s_len,
                            ثابت حرف *authority, size_t a_len,
                            ثابت حرف *رستہ, size_t p_len)
{
  بنية httpreq *req;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;

  DEBUGASSERT(طریقہ && m_len);

  req = curlx_calloc(1, حجم(*req) + m_len);
  إذا(!req)
    اذهب باہرلا;
#إذا defined(__GNUC__) && __GNUC__ >= 13
#ہدایت GCC diagnostic ڈالو
/* error: 'memcpy' offset [137, 142] from the object at 'req' is out of
   the bounds of referenced subobject 'method' with type 'char[1]' at
   offset 136 */
#ہدایت GCC diagnostic ignored "-Warray-bounds"
#جے_انت
  حافظہ_نقل(req->طریقہ, طریقہ, m_len);
#إذا defined(__GNUC__) && __GNUC__ >= 13
#ہدایت GCC diagnostic نکالو
#جے_انت
  إذا(scheme) {
    req->scheme = curlx_memdup0(scheme, s_len);
    إذا(!req->scheme)
      اذهب باہرلا;
  }
  إذا(authority) {
    req->authority = curlx_memdup0(authority, a_len);
    إذا(!req->authority)
      اذهب باہرلا;
  }
  إذا(رستہ) {
    req->رستہ = curlx_memdup0(رستہ, p_len);
    إذا(!req->رستہ)
      اذهب باہرلا;
  }
  Curl_dynhds_init(&req->سرخیاں, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  نتیجہ = CURLE_OK;

باہرلا:
  إذا(نتیجہ && req)
    Curl_http_req_free(req);
  *preq = نتیجہ ? عدم : req;
  أرجع نتیجہ;
}

ساكن CURLcode req_سونپو_url_authority(بنية httpreq *req, CURLU *url)
{
  حرف *host, *پورٹ;
  بنية dynbuf buf;
  CURLUcode uc;
  CURLcode نتیجہ = CURLE_URL_MALFORMAT;

  host = پورٹ = عدم;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_HOST, &host, 0);
  إذا(uc && uc != CURLUE_NO_HOST)
    اذهب باہرلا;
  إذا(!host) {
    req->authority = عدم;
    نتیجہ = CURLE_OK;
    اذهب باہرلا;
  }

  uc = curl_url_get(url, CURLUPART_پورٹ, &پورٹ, CURLU_NO_DEFAULT_پورٹ);
  إذا(uc && uc != CURLUE_NO_پورٹ)
    اذهب باہرلا;

  نتیجہ = curlx_dyn_جوڑو(&buf, host);
  إذا(نتیجہ)
    اذهب باہرلا;
  إذا(پورٹ) {
    نتیجہ = curlx_dyn_addf(&buf, ":%s", پورٹ);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  req->authority = curlx_dyn_ptr(&buf);
باہرلا:
  curlx_free(host);
  curlx_free(پورٹ);
  إذا(نتیجہ)
    curlx_dyn_free(&buf);
  أرجع نتیجہ;
}

ساكن CURLcode req_سونپو_url_رستہ(بنية httpreq *req, CURLU *url)
{
  حرف *رستہ, *query;
  بنية dynbuf buf;
  CURLUcode uc;
  CURLcode نتیجہ = CURLE_URL_MALFORMAT;

  رستہ = query = عدم;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_رستہ, &رستہ, 0);
  إذا(uc)
    اذهب باہرلا;
  uc = curl_url_get(url, CURLUPART_QUERY, &query, 0);
  إذا(uc && uc != CURLUE_NO_QUERY)
    اذهب باہرلا;

  إذا(!query) {
    req->رستہ = رستہ;
    رستہ = عدم;
  }
  وإلا {
    نتیجہ = curlx_dyn_جوڑو(&buf, رستہ);
    إذا(!نتیجہ)
      نتیجہ = curlx_dyn_addf(&buf, "?%s", query);
    إذا(نتیجہ)
      اذهب باہرلا;
    req->رستہ = curlx_dyn_ptr(&buf);
  }
  نتیجہ = CURLE_OK;

باہرلا:
  curlx_free(رستہ);
  curlx_free(query);
  إذا(نتیجہ)
    curlx_dyn_free(&buf);
  أرجع نتیجہ;
}

CURLcode Curl_http_req_make2(بنية httpreq **preq,
                             ثابت حرف *طریقہ, size_t m_len,
                             CURLU *url, ثابت حرف *scheme_default)
{
  بنية httpreq *req;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;
  CURLUcode uc;

  DEBUGASSERT(طریقہ && m_len);

  req = curlx_calloc(1, حجم(*req) + m_len);
  إذا(!req)
    اذهب باہرلا;
  حافظہ_نقل(req->طریقہ, طریقہ, m_len);

  uc = curl_url_get(url, CURLUPART_SCHEME, &req->scheme, 0);
  إذا(uc && uc != CURLUE_NO_SCHEME)
    اذهب باہرلا;
  إذا(!req->scheme && scheme_default) {
    req->scheme = curlx_strdup(scheme_default);
    إذا(!req->scheme)
      اذهب باہرلا;
  }

  نتیجہ = req_سونپو_url_authority(req, url);
  إذا(نتیجہ)
    اذهب باہرلا;
  نتیجہ = req_سونپو_url_رستہ(req, url);
  إذا(نتیجہ)
    اذهب باہرلا;

  Curl_dynhds_init(&req->سرخیاں, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  نتیجہ = CURLE_OK;

باہرلا:
  إذا(نتیجہ && req)
    Curl_http_req_free(req);
  *preq = نتیجہ ? عدم : req;
  أرجع نتیجہ;
}

فراغ Curl_http_req_free(بنية httpreq *req)
{
  إذا(req) {
    curlx_free(req->scheme);
    curlx_free(req->authority);
    curlx_free(req->رستہ);
    Curl_dynhds_free(&req->سرخیاں);
    Curl_dynhds_free(&req->trailers);
    curlx_free(req);
  }
}

بنية name_const {
  ثابت حرف *name;
  size_t namelen;
};

ساكن ثابت بنية name_const H2_NON_میدان[] = {
  { STRCONST("Host") },
  { STRCONST("Upgrade") },
  { STRCONST("Connection") },
  { STRCONST("Keep-Alive") },
  { STRCONST("Proxy-Connection") },
  { STRCONST("Transfer-Encoding") },
};

ساكن bool h2_permissible_میدان(بنية dynhds_اندراج *e)
{
  size_t i;
  لكل(i = 0; i < CURL_ARRAYSIZE(H2_NON_میدان); ++i) {
    إذا(e->namelen == H2_NON_میدان[i].namelen &&
       curl_strnequal(H2_NON_میدان[i].name, e->name, e->namelen))
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
        إذا(curlx_str_quotedword(&fvalue, &qw, CURL_MAX_HTTP_سرخی))
          أرجع FALSE;
      }
      وإلا
        fvalue++;
    }
  }
  أرجع FALSE;
}

CURLcode Curl_http_req_منزل_h2(بنية dynhds *h2_سرخیاں,
                             بنية httpreq *req, بنية Curl_easy *انکڑے)
{
  ثابت حرف *scheme = عدم, *authority = عدم;
  بنية dynhds_اندراج *e;
  size_t i;
  CURLcode نتیجہ;

  DEBUGASSERT(req);
  DEBUGASSERT(h2_سرخیاں);

  إذا(req->scheme) {
    scheme = req->scheme;
  }
  وإلا إذا(strcmp("CONNECT", req->طریقہ)) {
    scheme = Curl_checkheaders(انکڑے, STRCONST(HTTP_PSEUDO_SCHEME));
    إذا(scheme) {
      scheme += حجم(HTTP_PSEUDO_SCHEME);
      curlx_str_passblanks(&scheme);
      infof(انکڑے, "set pseudo header %s to %s", HTTP_PSEUDO_SCHEME, scheme);
    }
    وإلا {
      scheme = انکڑے->state.origin->scheme->name;
    }
  }

  إذا(req->authority) {
    authority = req->authority;
  }
  وإلا {
    e = Curl_dynhds_get(&req->سرخیاں, STRCONST("Host"));
    إذا(e)
      authority = e->قدر;
  }

  Curl_dynhds_reset(h2_سرخیاں);
  Curl_dynhds_سیٹ_opts(h2_سرخیاں, DYNHDS_OPT_LOWERCASE);
  نتیجہ = Curl_dynhds_جوڑو(h2_سرخیاں, STRCONST(HTTP_PSEUDO_طریقہ),
                           req->طریقہ, لڑی_لمبائی(req->طریقہ));
  إذا(!نتیجہ && scheme) {
    نتیجہ = Curl_dynhds_جوڑو(h2_سرخیاں, STRCONST(HTTP_PSEUDO_SCHEME),
                             scheme, لڑی_لمبائی(scheme));
  }
  إذا(!نتیجہ && authority) {
    نتیجہ = Curl_dynhds_جوڑو(h2_سرخیاں, STRCONST(HTTP_PSEUDO_AUTHORITY),
                             authority, لڑی_لمبائی(authority));
  }
  إذا(!نتیجہ && req->رستہ) {
    نتیجہ = Curl_dynhds_جوڑو(h2_سرخیاں, STRCONST(HTTP_PSEUDO_رستہ),
                             req->رستہ, لڑی_لمبائی(req->رستہ));
  }
  لكل(i = 0; !نتیجہ && i < Curl_dynhds_گنتی(&req->سرخیاں); ++i) {
    e = Curl_dynhds_getn(&req->سرخیاں, i);
    /* "TE" is special in that it is only permissible when it
     * has only value "trailers". RFC 9113 ch. 8.2.2 */
    إذا(e->namelen == 2 && curl_strequal("TE", e->name)) {
      إذا(http_TE_has_token(e->قدر, "trailers"))
        نتیجہ = Curl_dynhds_جوڑو(h2_سرخیاں, e->name, e->namelen,
                                 "trailers", CURL_CSTRLEN("trailers"));
    }
    وإلا إذا(h2_permissible_میدان(e)) {
      نتیجہ = Curl_dynhds_جوڑو(h2_سرخیاں, e->name, e->namelen,
                               e->قدر, e->valuelen);
    }
  }

  أرجع نتیجہ;
}

CURLcode Curl_http_resp_make(بنية http_resp **presp,
                             صحيح حالت,
                             ثابت حرف *description)
{
  بنية http_resp *resp;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;

  resp = curlx_calloc(1, حجم(*resp));
  إذا(!resp)
    اذهب باہرلا;

  resp->حالت = حالت;
  إذا(description) {
    resp->description = curlx_strdup(description);
    إذا(!resp->description)
      اذهب باہرلا;
  }
  Curl_dynhds_init(&resp->سرخیاں, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&resp->trailers, 0, DYN_HTTP_REQUEST);
  نتیجہ = CURLE_OK;

باہرلا:
  إذا(نتیجہ && resp)
    Curl_http_resp_free(resp);
  *presp = نتیجہ ? عدم : resp;
  أرجع نتیجہ;
}

فراغ Curl_http_resp_free(بنية http_resp *resp)
{
  إذا(resp) {
    curlx_free(resp->description);
    Curl_dynhds_free(&resp->سرخیاں);
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
  Curl_http_تیاری_conn,                 /* setup_connection */
  Curl_http,                            /* do_it */
  Curl_http_پورا,                       /* done */
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

#جے_انت /* CURL_DISABLE_HTTP */
