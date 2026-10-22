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

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#ضمن <netinet/in.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_NETDB_H
#ضمن <netdb.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NET_IF_H
#ضمن <net/if.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#جيڪڏهن_پورو

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

فراغ Curl_http_neg_init(بنية Curl_easy *معلومات, بنية http_negotiation *neg)
{
  memset(neg, 0, حجم(*neg));
  neg->accept_09 = معلومات->سيٽ.http09_allowed;
  بدل(معلومات->سيٽ.httpwant) {
  حالة CURL_HTTP_نسخو_1_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    neg->only_10 = TRUE;
    اكسر;
  حالة CURL_HTTP_نسخو_1_1:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    اكسر;
  حالة CURL_HTTP_نسخو_2_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->h2_upgrade = TRUE;
    اكسر;
  حالة CURL_HTTP_نسخو_2_TLS:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    اكسر;
  حالة CURL_HTTP_نسخو_2_اڳواٽ_KNOWLEDGE:
    neg->wanted = neg->allowed = (CURL_HTTP_V2x);
    معلومات->state.http_neg.h2_اڳواٽ_knowledge = TRUE;
    اكسر;
  حالة CURL_HTTP_نسخو_3:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    neg->allowed = neg->wanted;
    اكسر;
  حالة CURL_HTTP_نسخو_3_ONLY:
    neg->wanted = neg->allowed = (CURL_HTTP_V3x);
    اكسر;
  حالة CURL_HTTP_نسخو_NONE:
  افتراضي:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    اكسر;
  }
}

CURLcode Curl_http_تياري_conn(بنية Curl_easy *معلومات,
                              بنية connectdata *conn)
{
  /* allocate the HTTP-specific struct for the Curl_easy, only to survive
     during this request */
  إذا(معلومات->state.http_neg.wanted == CURL_HTTP_V3x) {
    /* only HTTP/3, needs to work */
    CURLcode نتيجو = Curl_conn_may_http3(معلومات, conn);
    إذا(نتيجو)
      أرجع نتيجو;
  }
  أرجع CURLE_OK;
}

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
/*
 * checkProxyHeaders() checks the linked list of custom proxy headers
 * if proxy headers are not available, then it will lookup into http header
 * link list
 *
 * It takes a connectdata struct as input to see if this is a proxy request or
 * not, as it then might check a different header list. Provide the header
 * prefix without colon!
 */
حرف *Curl_checkProxyheaders(بنية Curl_easy *معلومات,
                             ثابت بنية connectdata *conn,
                             ثابت حرف *thisheader,
                             ثابت size_t thislen)
{
  بنية curl_slist *سر;

  لكل(سر = (conn->http_proxy.peer && معلومات->سيٽ.sep_سرا) ?
        معلومات->سيٽ.proxyheaders : معلومات->سيٽ.سرا;
      سر; سر = سر->ايندڙ) {
    إذا(curl_strnequal(سر->معلومات, thisheader, thislen) &&
       Curl_headersep(سر->معلومات[thislen]))
      أرجع سر->معلومات;
  }

  أرجع عدم;
}
#جيڪڏهن_پورو

/* If the header has a value, this function returns TRUE and the value is in
   'outp' with blanks trimmed off. */
ساكن bool سرو_has_قدر(ثابت حرف **headerp, بنية Curl_str *outp)
{
  bool قدر = !curlx_str_cspn(headerp, outp, ";:") &&
    (!curlx_str_single(headerp, ':') || !curlx_str_single(headerp, ';'));

  إذا(قدر) {
    curlx_str_cspn(headerp, outp, "\r\n");
    curlx_str_trimblanks(outp);
  }
  أرجع قدر;
}

ساكن bool http_سرو_is_خالي_جايون(ثابت حرف *سرو)
{
  بنية Curl_str ٻاهريون;

  إذا(سرو_has_قدر(&سرو, &ٻاهريون)) {
    أرجع curlx_strlen(&ٻاهريون) == 0;
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
ساكن CURLcode copy_custom_قدر(ثابت حرف *سرو, حرف **valp)
{
  بنية Curl_str ٻاهريون = { 0 };

  /* find the end of the header name */
  إذا(سرو_has_قدر(&سرو, &ٻاهريون)) {
    *valp = curlx_memdup0(curlx_str(&ٻاهريون), curlx_strlen(&ٻاهريون));
    إذا(*valp)
      أرجع CURLE_OK;
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  }
  /* bad input */
  *valp = عدم;
  أرجع CURLE_BAD_FUNCTION_دليل;
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
حرف *Curl_copy_سرو_قدر(ثابت حرف *سرو)
{
  بنية Curl_str ٻاهريون;

  /* find the end of the header name */
  إذا(!curlx_str_until(&سرو, &ٻاهريون, MAX_HTTP_RESP_سرو_سائيز, ':') &&
     !curlx_str_single(&سرو, ':')) {
    curlx_str_untilnl(&سرو, &ٻاهريون, MAX_HTTP_RESP_سرو_سائيز);
    curlx_str_trimblanks(&ٻاهريون);
    أرجع curlx_memdup0(curlx_str(&ٻاهريون), curlx_strlen(&ٻاهريون));
  }
  /* bad input, should never happen */
  DEBUGASSERT(0);
  أرجع عدم;
}

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP_AUTH

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_BASIC_AUTH
/*
 * http_output_basic() sets up an Authorization: header (or the proxy version)
 * for HTTP Basic authentication.
 *
 * Returns CURLcode.
 */
ساكن CURLcode http_output_basic(بنية Curl_easy *معلومات,
                                  بنية connectdata *conn, bool proxy)
{
  size_t سائيز = 0;
  حرف *authorization = عدم;
  حرف **p_hd;
  CURLcode نتيجو;
  بنية Curl_creds *creds = عدم;
  حرف *ٻاهريون;

  /* credentials are unique per transfer for HTTP, do not use the ones for the
     connection */
  إذا(proxy) {
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
    p_hd = &معلومات->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
#وإلا
    (فراغ)conn;
    أرجع CURLE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
  }
  وإلا {
    p_hd = &معلومات->req.hd_auth;
    creds = معلومات->state.creds;
  }

  إذا(!creds) {
    DEBUGASSERT(0);
    أرجع CURLE_FAILED_INIT;
  }

  ٻاهريون = curl_maprintf("%s:%s", creds->user, creds->passwd);
  إذا(!ٻاهريون)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  نتيجو = curlx_base64_encode((uint8_t *)ٻاهريون, لڙي_ڊيگهه(ٻاهريون),
                               &authorization, &سائيز);
  إذا(نتيجو)
    اذهب fail;

  إذا(!authorization) {
    نتيجو = CURLE_REMOTE_ACCESS_DENIED;
    اذهب fail;
  }

  curlx_free(*p_hd);
  *p_hd = curl_maprintf("%sAuthorization: Basic %s\r\n",
                        proxy ? "Proxy-" : "",
                        authorization);
  curlx_free(authorization);
  إذا(!*p_hd) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب fail;
  }

fail:
  curlx_free(ٻاهريون);
  أرجع نتيجو;
}

#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_BEARER_AUTH
/*
 * http_output_bearer() sets up an Authorization: header
 * for HTTP Bearer authentication.
 *
 * Returns CURLcode.
 */
ساكن CURLcode http_output_bearer(بنية Curl_easy *معلومات)
{
  حرف **userp;
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(Curl_creds_has_oauth_bearer(معلومات->state.creds));
  userp = &معلومات->req.hd_auth;
  curlx_free(*userp);
  *userp = curl_maprintf("Authorization: Bearer %s\r\n",
                         Curl_creds_oauth_bearer(معلومات->state.creds));

  إذا(!*userp) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب fail;
  }

fail:
  أرجع نتيجو;
}
#جيڪڏهن_پورو

#جيڪڏهن_پورو

/* pickoneauth() selects the most favorable authentication method from the
 * ones available and the ones we want.
 *
 * return TRUE if one was picked
 */
ساكن bool pickoneauth(بنية auth *pick, بنانشان ڊگهو نقاب,
                        بنية Curl_creds *creds)
{
  bool have_user_pass = Curl_creds_has_user_or_pass(creds);
  bool picked;
  /* only deal with authentication we want */
  بنانشان ڊگهو avail = pick->avail & pick->want & نقاب;
  picked = TRUE;

  /* The order of these checks is highly relevant, as this will be the order
     of preference in case of the existence of multiple accepted types. */
  إذا(avail & CURLAUTH_NEGOTIATE)  /* available on empty creds */
    pick->picked = CURLAUTH_NEGOTIATE;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_BEARER_AUTH
  وإلا إذا((avail & CURLAUTH_BEARER) && Curl_creds_has_oauth_bearer(creds))
    pick->picked = CURLAUTH_BEARER;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DIGEST_AUTH
  وإلا إذا((avail & CURLAUTH_DIGEST) && have_user_pass)
    pick->picked = CURLAUTH_DIGEST;
#جيڪڏهن_پورو
  وإلا إذا(avail & CURLAUTH_NTLM)
    pick->picked = CURLAUTH_NTLM;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_BASIC_AUTH
  وإلا إذا((avail & CURLAUTH_BASIC) && have_user_pass)
    pick->picked = CURLAUTH_BASIC;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_AWS
  وإلا إذا(avail & CURLAUTH_AWS_SIGV4)
    pick->picked = CURLAUTH_AWS_SIGV4;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTPSIG
  وإلا إذا(avail & CURLAUTH_HTTPSIG)
    pick->picked = CURLAUTH_HTTPSIG;
#جيڪڏهن_پورو
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
ساكن CURLcode http_perhapsrewind(بنية Curl_easy *معلومات,
                                   بنية connectdata *conn)
{
  curl_off_t bytessent = معلومات->req.writebytecount;
  curl_off_t expectsend = Curl_creader_ڪل_ڊيگهائي(معلومات);
  curl_off_t upload_remain = (expectsend >= 0) ? (expectsend - bytessent) : -1;
  bool little_upload_remains = (upload_remain >= 0 && upload_remain < 2000);
  bool needs_rewind = Curl_creader_needs_rewind(معلومات);
  /* By default, we would like to abort the transfer when little or unknown
   * amount remains. This may be overridden by authentications further
   * below! */
  bool abort_upload = (!معلومات->req.upload_پورو && !little_upload_remains);
  VERBOSE(ثابت حرف *ongoing_auth = عدم);

  /* We need a rewind before uploading client read data again. The
   * checks below influence of the upload is to be continued
   * or aborted early.
   * This depends on how much remains to be sent and in what state
   * the authentication is. Some auth schemes such as NTLM do not work
   * for a new connection. */
  إذا(needs_rewind) {
    infof(معلومات, "Need to rewind upload for next request");
    Curl_creader_سيٽ_rewind(معلومات, TRUE);
  }

  إذا(conn->بٽ.close)
    /* If we already decided to close this connection, we cannot veto. */
    أرجع CURLE_OK;

  إذا(abort_upload) {
    /* We would like to abort the upload - but should we? */
#جيڪڏهن_وصف_ٿيل USE_NTLM
    إذا((معلومات->state.authproxy.picked == CURLAUTH_NTLM) ||
       (معلومات->state.authhost.picked == CURLAUTH_NTLM)) {
      VERBOSE(ongoing_auth = "NTLM");
      إذا((conn->http_ntlm_state != NTLMSTATE_NONE) ||
         (conn->proxy_ntlm_state != NTLMSTATE_NONE)) {
        /* The NTLM-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_SPNEGO
    /* There is still data left to send */
    إذا((معلومات->state.authproxy.picked == CURLAUTH_NEGOTIATE) ||
       (معلومات->state.authhost.picked == CURLAUTH_NEGOTIATE)) {
      VERBOSE(ongoing_auth = "NEGOTIATE");
      إذا((conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) ||
         (conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE)) {
        /* The NEGOTIATE-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#جيڪڏهن_پورو
  }

  إذا(abort_upload) {
    إذا(upload_remain >= 0)
      infof(معلومات, "%s%sclose instead of sending %" FMT_OFF_T " more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "",
            upload_remain);
    وإلا
      infof(معلومات, "%s%sclose instead of sending unknown amount "
            "of more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "");
    /* We decided to abort the ongoing transfer */
    streamclose(conn);
    معلومات->req.سائيز = 0; /* do not download any more than 0 bytes */
    معلومات->req.http_bodyless = TRUE;
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
ساكن bool http_should_fail(بنية Curl_easy *معلومات, صحيح httpcode)
{
  DEBUGASSERT(معلومات);
  DEBUGASSERT(معلومات->conn);

  /*
   * If we have not been asked to fail on error,
   * do not fail.
   */
  إذا(!معلومات->سيٽ.http_fail_on_error)
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
  إذا(معلومات->state.resume_from && معلومات->state.httpreq == HTTPREQ_GET &&
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
  إذا((httpcode == 401) && !معلومات->state.creds)
    أرجع TRUE;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  إذا((httpcode == 407) && !معلومات->conn->http_proxy.creds)
    أرجع TRUE;
#جيڪڏهن_پورو

  أرجع (bool)معلومات->state.authproblem;
}

/*
 * Curl_http_auth_act() gets called when all HTTP headers have been received
 * and it checks what authentication methods that are available and decides
 * which one (if any) to use. It will set 'newurl' if an auth method was
 * picked.
 */
CURLcode Curl_http_auth_act(بنية Curl_easy *معلومات)
{
  بنية connectdata *conn = معلومات->conn;
  bool pickhost = FALSE;
  bool pickproxy = FALSE;
  CURLcode نتيجو = CURLE_OK;
  بنانشان ڊگهو authmask = ~0UL;

  إذا(!Curl_creds_has_oauth_bearer(معلومات->state.creds))
    authmask &= (بنانشان ڊگهو)~CURLAUTH_BEARER;

  إذا(100 <= معلومات->req.httpcode && معلومات->req.httpcode <= 199)
    /* this is a transient response code, ignore */
    أرجع CURLE_OK;

  إذا(معلومات->state.authproblem)
    أرجع معلومات->سيٽ.http_fail_on_error ? CURLE_HTTP_RETURNED_ERROR : CURLE_OK;

  إذا(معلومات->state.creds &&
     ((معلومات->req.httpcode == 401) ||
      (معلومات->req.authneg && معلومات->req.httpcode < 300))) {
    pickhost = pickoneauth(&معلومات->state.authhost, authmask, معلومات->state.creds);
    إذا(!pickhost)
      معلومات->state.authproblem = TRUE;
    وإلا
      معلومات->info.httpauthpicked = معلومات->state.authhost.picked;
    إذا(معلومات->state.authhost.picked == CURLAUTH_NTLM &&
       (معلومات->req.httpversion_موڪليل > 11)) {
      infof(معلومات, "Forcing HTTP/1.1 for NTLM");
      connclose(conn);
      معلومات->state.http_neg.wanted = CURL_HTTP_V1x;
      معلومات->state.http_neg.allowed = CURL_HTTP_V1x;
    }
  }
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  إذا(conn->http_proxy.creds &&
     ((معلومات->req.httpcode == 407) ||
      (معلومات->req.authneg && معلومات->req.httpcode < 300))) {
    pickproxy = pickoneauth(&معلومات->state.authproxy,
                            authmask & ~CURLAUTH_BEARER,
                            conn->http_proxy.creds);
    إذا(!pickproxy)
      معلومات->state.authproblem = TRUE;
    وإلا
      معلومات->info.proxyauthpicked = معلومات->state.authproxy.picked;
  }
#جيڪڏهن_پورو

  إذا(pickhost || pickproxy) {
    نتيجو = http_perhapsrewind(معلومات, conn);
    إذا(نتيجو)
      أرجع نتيجو;

    /* In case this is GSS auth, the newurl field is already allocated so
       we must make sure to free it before allocating a new one. As figured
       out in bug #2284386 */
    curlx_free(معلومات->req.newurl);
    /* clone URL */
    معلومات->req.newurl = Curl_bufref_dup(&معلومات->state.url);
    إذا(!معلومات->req.newurl)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
  }
  وإلا إذا((معلومات->req.httpcode < 300) &&
          !معلومات->state.authhost.پورو &&
          معلومات->req.authneg &&
          /* no (known) authentication available,
             authentication is not "done" yet and
             no authentication seems to be required and
             we did not try HEAD or GET */
          (معلومات->state.httpreq != HTTPREQ_GET) &&
          (معلومات->state.httpreq != HTTPREQ_سر)) {
    /* clone URL */
    معلومات->req.newurl = Curl_bufref_dup(&معلومات->state.url);
    إذا(!معلومات->req.newurl)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    معلومات->state.authhost.پورو = TRUE;
  }
  إذا(http_should_fail(معلومات, معلومات->req.httpcode)) {
    failf(معلومات, "The requested URL returned error: %d",
          معلومات->req.httpcode);
    نتيجو = CURLE_HTTP_RETURNED_ERROR;
  }

  أرجع نتيجو;
}

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP_AUTH
/*
 * Output the correct authentication header depending on the auth type
 * and whether or not it is to a proxy.
 */
ساكن CURLcode output_auth_سرا(بنية Curl_easy *معلومات,
                                    بنية connectdata *conn,
                                    بنية auth *authstatus,
                                    ثابت حرف *request,
                                    ثابت حرف *رستو,
                                    bool proxy)
{
  ثابت حرف *auth = عدم;
  CURLcode نتيجو = CURLE_OK;
  (فراغ)conn;

#جيڪڏهن_وصف_ٿيل CURL_DISABLE_DIGEST_AUTH
  (فراغ)request;
  (فراغ)رستو;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_AWS
  إذا((authstatus->picked == CURLAUTH_AWS_SIGV4) && !proxy) {
    /* this method is never for proxy */
    auth = "AWS_SIGV4";
    نتيجو = Curl_output_aws_sigv4(معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
  }
  وإلا
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTPSIG
  إذا((authstatus->picked == CURLAUTH_HTTPSIG) && !proxy) {
    /* HTTPSIG uses its own configured key material rather than
       data->state.creds. Do not let unrelated credentials from a
       redirected URL bypass the cross-host auth boundary. */
    إذا(Curl_auth_allowed_منزل_host(معلومات)) {
      auth = "HTTPSIG";
      نتيجو = Curl_output_httpsig(معلومات);
      إذا(نتيجو)
        أرجع نتيجو;
    }
    وإلا
      authstatus->پورو = TRUE;
  }
  وإلا
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_SPNEGO
  إذا(authstatus->picked == CURLAUTH_NEGOTIATE) {
    إذا(
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
      (proxy && !Curl_checkProxyheaders(معلومات, conn,
                                        STRCONST("Proxy-authorization"))) ||
#جيڪڏهن_پورو
      (!proxy && !Curl_checkheaders(معلومات, STRCONST("Authorization")) &&
       Curl_auth_allowed_منزل_host(معلومات))) {
      auth = "Negotiate";
      نتيجو = Curl_output_negotiate(معلومات, conn, proxy);
      إذا(نتيجو)
        أرجع نتيجو;
    }
    وإلا
      authstatus->پورو = TRUE;
  }
  وإلا
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_NTLM
  إذا(authstatus->picked == CURLAUTH_NTLM) {
    إذا(proxy || Curl_auth_allowed_منزل_host(معلومات)) {
      auth = "NTLM";
      نتيجو = Curl_output_ntlm(معلومات, proxy);
      إذا(نتيجو)
        أرجع نتيجو;
    }
    وإلا
      authstatus->پورو = TRUE;
  }
  وإلا
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DIGEST_AUTH
  إذا(authstatus->picked == CURLAUTH_DIGEST) {
    auth = "Digest";
    نتيجو = Curl_output_digest(معلومات,
                                proxy,
                                (ثابت بنانشان حرف *)request,
                                (ثابت بنانشان حرف *)رستو);
    إذا(نتيجو)
      أرجع نتيجو;
  }
  وإلا
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_BASIC_AUTH
  إذا(authstatus->picked == CURLAUTH_BASIC) {
    /* Basic */
    إذا(
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
       (proxy && conn->http_proxy.creds &&
        Curl_creds_has_user_or_pass(conn->http_proxy.creds) &&
        !Curl_checkProxyheaders(معلومات, conn,
                                STRCONST("Proxy-authorization"))) ||
#جيڪڏهن_پورو
       (!proxy && معلومات->state.creds &&
        Curl_creds_has_user_or_pass(معلومات->state.creds) &&
        !Curl_checkheaders(معلومات, STRCONST("Authorization")))) {
      auth = "Basic";
      نتيجو = http_output_basic(معلومات, conn, proxy);
      إذا(نتيجو)
        أرجع نتيجو;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->پورو = TRUE;
  }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_BEARER_AUTH
  إذا(authstatus->picked == CURLAUTH_BEARER) {
    /* Bearer */
    إذا(!proxy && Curl_creds_has_oauth_bearer(معلومات->state.creds) &&
       !Curl_checkheaders(معلومات, STRCONST("Authorization"))) {
      auth = "Bearer";
      نتيجو = http_output_bearer(معلومات);
      إذا(نتيجو)
        أرجع نتيجو;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->پورو = TRUE;
  }
#جيڪڏهن_پورو

  إذا(auth) {
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
    إذا(proxy)
      معلومات->info.proxyauthpicked = authstatus->picked;
    وإلا
      معلومات->info.httpauthpicked = authstatus->picked;
    infof(معلومات, "%s auth using %s with user '%s'",
          proxy ? "Proxy" : "Server", auth,
          proxy ? (conn->http_proxy.creds ?
                   conn->http_proxy.creds->user : "") :
          (معلومات->state.creds ?
           معلومات->state.creds->user : ""));
#وإلا
    (فراغ)proxy;
    infof(معلومات, "Server auth using %s with user '%s'",
          auth, معلومات->state.creds ?
          معلومات->state.creds->user : "");
#جيڪڏهن_پورو
    authstatus->multipass = !authstatus->پورو;
  }
  وإلا {
    authstatus->multipass = FALSE;
    إذا(proxy)
      معلومات->info.proxyauthpicked = 0;
    وإلا
      معلومات->info.httpauthpicked = 0;
  }

  أرجع نتيجو;
}

CURLcode Curl_http_output_auth(بنية Curl_easy *معلومات,
                               بنية connectdata *conn,
                               ثابت حرف *request,
                               Curl_HttpReq httpreq,
                               ثابت حرف *رستو,
                               ثابت حرف *query,
                               bool is_connect)
{
  CURLcode نتيجو = CURLE_OK;
  بنية auth *authhost;
  بنية auth *authproxy;
  ثابت حرف *رستو_and_query = رستو;
  حرف *tmp_str = عدم;

  DEBUGASSERT(معلومات);
  authhost = &معلومات->state.authhost;
  authproxy = &معلومات->state.authproxy;

  إذا(
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
    (!conn->http_proxy.peer || !conn->http_proxy.creds) &&
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_SPNEGO
    !(authhost->want & CURLAUTH_NEGOTIATE) &&
    !(authproxy->want & CURLAUTH_NEGOTIATE) &&
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTPSIG
    !(authhost->want & CURLAUTH_HTTPSIG) &&
#جيڪڏهن_پورو
    !معلومات->state.creds) {
    /* no authentication with no user or password */
    authhost->پورو = TRUE;
    authproxy->پورو = TRUE;
    نتيجو = CURLE_OK;
    اذهب ٻاهريون;
  }

  إذا(query) {
    tmp_str = curl_maprintf("%s?%s", رستو, query);
    إذا(!tmp_str) {
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      اذهب ٻاهريون;
    }
    رستو_and_query = tmp_str;
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

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  /* Send proxy authentication header if needed */
  إذا(conn->بٽ.origin_is_proxy || is_connect) {
    نتيجو = output_auth_سرا(معلومات, conn, authproxy, request,
                                 رستو_and_query, TRUE);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
  وإلا
#وإلا
  (فراغ)is_connect;
#جيڪڏهن_پورو /* CURL_DISABLE_PROXY */
    /* we have no proxy so let's pretend we are done authenticating
       with it */
    authproxy->پورو = TRUE;

  /* Either we have credentials for the origin we talk to or
     performing authentication is allowed here */
  إذا(معلومات->state.creds || Curl_auth_allowed_منزل_host(معلومات))
    نتيجو = output_auth_سرا(معلومات, conn, authhost, request,
                                 رستو_and_query, FALSE);
  وإلا
    authhost->پورو = TRUE;

  إذا(((authhost->multipass && !authhost->پورو) ||
      (authproxy->multipass && !authproxy->پورو)) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_سر)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    معلومات->req.authneg = TRUE;
  }
  وإلا
    معلومات->req.authneg = FALSE;

ٻاهريون:
  curlx_free(tmp_str);
  أرجع نتيجو;
}

#وإلا /* !CURL_DISABLE_HTTP_AUTH */
/* when disabled */
CURLcode Curl_http_output_auth(بنية Curl_easy *معلومات,
                               بنية connectdata *conn,
                               ثابت حرف *request,
                               Curl_HttpReq httpreq,
                               ثابت حرف *رستو,
                               ثابت حرف *query,
                               bool is_connect)
{
  (فراغ)معلومات;
  (فراغ)conn;
  (فراغ)request;
  (فراغ)httpreq;
  (فراغ)رستو;
  (فراغ)query;
  (فراغ)is_connect;
  أرجع CURLE_OK;
}
#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP_AUTH, else */

#إذا defined(USE_SPNEGO) || defined(USE_NTLM) || \
  !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_BASIC_AUTH) || \
  !defined(CURL_DISABLE_BEARER_AUTH)
ساكن bool authcmp(ثابت حرف *auth, ثابت حرف *سٽ)
{
  /* the auth string must not have an alnum following */
  size_t n = لڙي_ڊيگهه(auth);
  أرجع curl_strnequal(auth, سٽ, n) && !ISALNUM(سٽ[n]);
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_SPNEGO
ساكن CURLcode auth_spnego(بنية Curl_easy *معلومات,
                            bool proxy,
                            ثابت حرف *auth,
                            بنية auth *authp,
                            uint32_t *availp)
{
  إذا((authp->avail & CURLAUTH_NEGOTIATE) || Curl_auth_is_spnego_supported()) {
    *availp |= CURLAUTH_NEGOTIATE;
    authp->avail |= CURLAUTH_NEGOTIATE;

    إذا(authp->picked == CURLAUTH_NEGOTIATE) {
      بنية connectdata *conn = معلومات->conn;
      CURLcode نتيجو = Curl_داخلا_negotiate(معلومات, conn, proxy, auth);
      uint8_t *negstate = proxy ? &conn->proxy_negotiate_state :
        &conn->http_negotiate_state;
      إذا(!نتيجو) {
        curlx_free(معلومات->req.newurl);
        معلومات->req.newurl = Curl_bufref_dup(&معلومات->state.url);
        إذا(!معلومات->req.newurl)
          أرجع CURLE_ٻاهريون_جو_يادگيري;
        معلومات->state.authproblem = FALSE;
        /* we received a GSS auth token and we dealt with it fine */
        *negstate = CURL_NEGO_AUTH_RECV;
      }
      وإلا
        معلومات->state.authproblem = TRUE;
    }
  }
  أرجع CURLE_OK;
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_NTLM
ساكن CURLcode auth_ntlm(بنية Curl_easy *معلومات,
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
      CURLcode نتيجو = Curl_داخلا_ntlm(معلومات, proxy, auth);
      إذا(!نتيجو)
        معلومات->state.authproblem = FALSE;
      وإلا {
        إذا(نتيجو == CURLE_ٻاهريون_جو_يادگيري)
          أرجع نتيجو;
        infof(معلومات, "NTLM authentication problem, ignoring.");
        معلومات->state.authproblem = TRUE;
      }
    }
  }
  أرجع CURLE_OK;
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DIGEST_AUTH
ساكن CURLcode auth_digest(بنية Curl_easy *معلومات,
                            bool proxy,
                            ثابت حرف *auth,
                            بنية auth *authp,
                            uint32_t *availp)
{
  إذا(authp->avail & CURLAUTH_DIGEST) {
    *availp |= CURLAUTH_DIGEST;
    infof(معلومات, "Ignoring duplicate digest auth header.");
  }
  وإلا إذا(Curl_auth_is_digest_supported()) {
    CURLcode نتيجو;

    *availp |= CURLAUTH_DIGEST;
    authp->avail |= CURLAUTH_DIGEST;

    /* We call this function on input Digest headers even if Digest
     * authentication is not activated yet, as we need to store the
     * incoming data from this header in case we are going to use
     * Digest */
    نتيجو = Curl_داخلا_digest(معلومات, proxy, auth);
    إذا(نتيجو) {
      إذا(نتيجو == CURLE_ٻاهريون_جو_يادگيري)
        أرجع نتيجو;
      infof(معلومات, "Digest authentication problem, ignoring.");
      معلومات->state.authproblem = TRUE;
    }
  }
  أرجع CURLE_OK;
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_BASIC_AUTH
ساكن CURLcode auth_basic(بنية Curl_easy *معلومات,
                           بنية auth *authp,
                           uint32_t *availp)
{
  *availp |= CURLAUTH_BASIC;
  authp->avail |= CURLAUTH_BASIC;
  إذا(authp->picked == CURLAUTH_BASIC) {
    /* We asked for Basic authentication but got a 40X back anyway, which
       means our name+password is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(معلومات, "Basic authentication problem, ignoring.");
    معلومات->state.authproblem = TRUE;
  }
  أرجع CURLE_OK;
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_BEARER_AUTH
ساكن CURLcode auth_bearer(بنية Curl_easy *معلومات,
                            بنية auth *authp,
                            uint32_t *availp)
{
  *availp |= CURLAUTH_BEARER;
  authp->avail |= CURLAUTH_BEARER;
  إذا(authp->picked == CURLAUTH_BEARER) {
    /* We asked for Bearer authentication but got a 40X back anyway, which
       means our token is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(معلومات, "Bearer authentication problem, ignoring.");
    معلومات->state.authproblem = TRUE;
  }
  أرجع CURLE_OK;
}
#جيڪڏهن_پورو

/*
 * Curl_http_input_auth() deals with Proxy-Authenticate: and WWW-Authenticate:
 * headers. They are dealt with both in the transfer.c main loop and in the
 * proxy CONNECT loop.
 *
 * The 'auth' line ends with a NUL byte without CR or LF present.
 */
CURLcode Curl_http_داخلا_auth(بنية Curl_easy *معلومات, bool proxy,
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
  CURLcode نتيجو = CURLE_OK;
  DEBUGASSERT(auth);
  DEBUGASSERT(معلومات);

  إذا(proxy) {
    availp = &معلومات->info.proxyauthavail;
    authp = &معلومات->state.authproxy;
  }
  وإلا {
    availp = &معلومات->info.httpauthavail;
    authp = &معلومات->state.authhost;
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
#جيڪڏهن_وصف_ٿيل USE_SPNEGO
    إذا(authcmp("Negotiate", auth))
      نتيجو = auth_spnego(معلومات, proxy, auth, authp, availp);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_NTLM
    إذا(!نتيجو && authcmp("NTLM", auth))
      نتيجو = auth_ntlm(معلومات, proxy, auth, authp, availp);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DIGEST_AUTH
    إذا(!نتيجو && authcmp("Digest", auth))
      نتيجو = auth_digest(معلومات, proxy, auth, authp, availp);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_BASIC_AUTH
    إذا(!نتيجو && authcmp("Basic", auth))
      نتيجو = auth_basic(معلومات, authp, availp);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_BEARER_AUTH
    إذا(authcmp("Bearer", auth))
      نتيجو = auth_bearer(معلومات, authp, availp);
#جيڪڏهن_پورو

    إذا(نتيجو)
      اكسر;

    /* there may be multiple methods on one line, so keep reading */
    auth = strchr(auth, ',');
    إذا(auth) /* if we are on a comma, skip it */
      auth++;
    وإلا
      اكسر;
    curlx_str_passblanks(&auth);
  }
  أرجع نتيجو;
#وإلا
  (فراغ)معلومات;
  (فراغ)proxy;
  (فراغ)auth;
  /* nothing to do when disabled */
  أرجع CURLE_OK;
#جيڪڏهن_پورو
}

ساكن فراغ http_switch_منزل_get(بنية Curl_easy *معلومات, صحيح ڪوڊ)
{
  ثابت حرف *req = CURL_EASY_STR(معلومات, STRING_CUSTOMREQUEST);

  إذا((req || معلومات->state.httpreq != HTTPREQ_GET) &&
     (معلومات->سيٽ.http_follow_گهڻائي == CURLFOLLOW_OBEYCODE)) {
    NOVERBOSE((فراغ)ڪوڊ);
    infof(معلومات, "Switch to GET because of %d response", ڪوڊ);
    معلومات->state.http_ignorecustom = TRUE;
  }
  وإلا إذا(req && (معلومات->سيٽ.http_follow_گهڻائي != CURLFOLLOW_FIRSTONLY))
    infof(معلومات, "Stick to %s instead of GET", req);

  معلومات->state.httpreq = HTTPREQ_GET;
  Curl_creader_سيٽ_rewind(معلومات, FALSE);
}

#وصف_ڪريو HTTPREQ_IS_POST(معلومات)                    \
  ((معلومات)->state.httpreq == HTTPREQ_POST ||      \
   (معلومات)->state.httpreq == HTTPREQ_POST_FORM || \
   (معلومات)->state.httpreq == HTTPREQ_POST_MIME)

ساكن CURLcode سيٽ_auto_referer(بنية Curl_easy *معلومات)
{
  CURLU *u;
  حرف *referer = عدم;
  CURLUcode uc;

  /* We are asked to automatically set the previous URL as the referer
     when we get the next URL. We pick the ->url field, which may or may
     not be 100% correct */
  Curl_bufref_free(&معلومات->state.referer);

  /* Make a copy of the URL without credentials and fragment */
  u = curl_url();
  إذا(!u)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  uc = curl_url_سيٽ(u, CURLUPART_URL,
                    Curl_bufref_ptr(&معلومات->state.url), 0);
  إذا(!uc)
    uc = curl_url_سيٽ(u, CURLUPART_FRAGMENT, عدم, 0);
  إذا(!uc)
    uc = curl_url_سيٽ(u, CURLUPART_USER, عدم, 0);
  إذا(!uc)
    uc = curl_url_سيٽ(u, CURLUPART_PASSWORD, عدم, 0);
  إذا(!uc)
    uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

  curl_url_cleanup(u);

  إذا(uc || !referer)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  Curl_bufref_سيٽ(&معلومات->state.referer, referer, 0, curl_free);
  أرجع CURLE_OK;
}

ساكن CURLcode follow_parse_url(بنية Curl_easy *معلومات, ثابت حرف *newurl,
                                 followtype type, حرف **follow_urlp,
                                 CURLUcode *ucp)
{
  CURLUcode uc;

  DEBUGASSERT(معلومات->state.uh);
  uc = curl_url_سيٽ(معلومات->state.uh, CURLUPART_URL, newurl, (بنانشان صحيح)
                    ((type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                     ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) |
                     CURLU_ALLOW_SPACE |
                     (معلومات->سيٽ.رستو_as_is ? CURLU_رستو_AS_IS : 0)));
  *ucp = uc;
  إذا(uc) {
    إذا((uc == CURLUE_ٻاهريون_جو_يادگيري) || (type != FOLLOW_FAKE)) {
      failf(معلومات, "The redirect target URL could not be parsed: %s",
            curl_url_strerror(uc));
      أرجع Curl_uc_منزل_curlcode(uc);
    }

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, duplicate the field as-is */
    *follow_urlp = curlx_strdup(newurl);
    إذا(!*follow_urlp)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
  }
  وإلا {
    CURLU *u = curl_url();
    إذا(!u)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    uc = curl_url_سيٽ(u, CURLUPART_URL,
                      Curl_bufref_ptr(&معلومات->state.url),
                      CURLU_URLENCODE | CURLU_ALLOW_SPACE);
    إذا(!uc)
      uc = curl_url_get(معلومات->state.uh, CURLUPART_URL, follow_urlp, 0);
    إذا(uc) {
      curl_url_cleanup(u);
      أرجع Curl_uc_منزل_curlcode(uc);
    }

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DIGEST_AUTH
    {
      bool same_origin = Curl_url_same_origin(u, معلومات->state.uh);
      curl_url_cleanup(u);
      إذا(!same_origin)
        Curl_auth_digest_cleanup(&معلومات->state.digest);
    }
#وإلا
    curl_url_cleanup(u);
#جيڪڏهن_پورو
  }
  أرجع CURLE_OK;
}

ساكن CURLcode follow_fake(بنية Curl_easy *معلومات, حرف *follow_url,
                            CURLUcode uc, bool reachedmax)
{
  /* we are only figuring out the new URL if we would have followed locations
     but now we are done so we can get out! */
  إذا(!uc) {
    CURLU *u = curl_url();
    حرف *nocred = عدم;

    إذا(!u) {
      curlx_free(follow_url);
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
    إذا(!curl_url_سيٽ(u, CURLUPART_URL, follow_url,
                     CURLU_NON_SUPPORT_SCHEME |
                     (معلومات->سيٽ.رستو_as_is ? CURLU_رستو_AS_IS : 0)) &&
       !curl_url_سيٽ(u, CURLUPART_USER, عدم, 0) &&
       !curl_url_سيٽ(u, CURLUPART_PASSWORD, عدم, 0))
      (فراغ)curl_url_get(u, CURLUPART_URL, &nocred, CURLU_GET_خالي_جايون);
    curl_url_cleanup(u);
    إذا(nocred) {
      curlx_free(follow_url);
      follow_url = nocred;
    }
  }
  معلومات->info.wouldredirect = follow_url;

  إذا(reachedmax) {
    failf(معلومات, "Maximum (%d) redirects followed", معلومات->سيٽ.maxredirs);
    أرجع CURLE_TOO_MANY_REDIRECTS;
  }
  أرجع CURLE_OK;
}

ساكن bool follow_switch_get(بنية Curl_easy *معلومات)
{
  /*
   * We get here when the HTTP code is 300-399 and 401 or 407. We need to
   * perform differently based on exactly what return code there was.
   */
  بدل(معلومات->info.httpcode) {
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
    إذا(HTTPREQ_IS_POST(معلومات) && !معلومات->سيٽ.post301) {
      http_switch_منزل_get(معلومات, 301);
      أرجع TRUE;
    }
    اكسر;
  حالة 302: /*مليو*/
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
    إذا(HTTPREQ_IS_POST(معلومات) && !معلومات->سيٽ.post302) {
      http_switch_منزل_get(معلومات, 302);
      أرجع TRUE;
    }
    اكسر;
  حالة 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     */
    إذا(!HTTPREQ_IS_POST(معلومات) || !معلومات->سيٽ.post303) {
      http_switch_منزل_get(معلومات, 303);
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

CURLcode Curl_http_follow(بنية Curl_easy *معلومات, ثابت حرف *newurl,
                          followtype type)
{
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  حرف *follow_url = عدم;
  CURLUcode uc;
  CURLcode نتيجو;
  CURLcode rewind_نتيجو;
  bool switch_منزل_get;

  DEBUGASSERT(type != FOLLOW_NONE);

  إذا(type != FOLLOW_FAKE)
    معلومات->state.درخواستون++; /* count all real follows */
  إذا(type == FOLLOW_REDIR) {
    إذا((معلومات->سيٽ.maxredirs != -1) &&
       (معلومات->state.followlocation >= معلومات->سيٽ.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    وإلا {
      معلومات->state.followlocation++; /* count redirect-followings, including
                                       auth reloads */

      إذا(معلومات->سيٽ.http_auto_referer) {
        نتيجو = سيٽ_auto_referer(معلومات);
        إذا(نتيجو)
          أرجع نتيجو;
      }
    }
  }

  إذا((type != FOLLOW_RETRY) &&
     (معلومات->req.httpcode != 401) && (معلومات->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, عدم, 0, FALSE)) {
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: do not allow a custom port number */
    disallowport = TRUE;
  }

  نتيجو = follow_parse_url(معلومات, newurl, type, &follow_url, &uc);
  إذا(نتيجو)
    أرجع نتيجو;

  DEBUGASSERT(follow_url);

  إذا(type == FOLLOW_FAKE)
    أرجع follow_fake(معلومات, follow_url, uc, reachedmax);

  إذا(disallowport)
    معلومات->state.allow_پورٽ = FALSE;

  Curl_bufref_سيٽ(&معلومات->state.url, follow_url, 0, curl_free);
  rewind_نتيجو = Curl_req_soft_reset(&معلومات->req, معلومات);
  infof(معلومات, "Issue another request to this URL: '%s'", follow_url);
  إذا((معلومات->سيٽ.http_follow_گهڻائي == CURLFOLLOW_FIRSTONLY) &&
     !معلومات->state.http_ignorecustom &&
     CURL_EASY_STR(معلومات, STRING_CUSTOMREQUEST)) {
    معلومات->state.http_ignorecustom = TRUE;
    infof(معلومات, "Drop custom request method for next request");
  }

  switch_منزل_get = follow_switch_get(معلومات);

  /* When rewind of upload data failed and we are not switching to GET,
   * we need to fail the follow, as we cannot send the data again. */
  إذا(rewind_نتيجو && !switch_منزل_get)
    أرجع rewind_نتيجو;

  Curl_pgrs_وقت(معلومات, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(معلومات);

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
                        ثابت حرف *سرو, /* header keyword _with_ colon */
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
  DEBUGASSERT(سرو);
  DEBUGASSERT(content);

  إذا(!curl_strnequal(headerline, سرو, hlen))
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
      بنية Curl_str ايندڙ;
      ثابت حرف *o = p;
      /* after a match there must be a comma, space, newline or NUL byte */
      إذا(curl_strnequal(p, content, clen) &&
         ((p[clen] == ',') || ISBLANK(p[clen]) || ISNEWLINE(p[clen]) ||
          !p[clen]))
        أرجع TRUE; /* match! */
      /* advance to the next comma */
      إذا(curlx_str_until(&p, &ايندڙ, len, ',') ||
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
  بنية curltime شروعات; /* time started waiting */
  تعداد expect100 state;
};

/* Expect: 100-continue client reader, blocking uploads */

ساكن فراغ http_exp100_continue(بنية Curl_easy *معلومات,
                                 بنية Curl_creader *reader)
{
  بنية cr_exp100_ctx *ctx = reader->ctx;
  إذا(ctx->state > EXP100_SEND_معلومات) {
    ctx->state = EXP100_SEND_معلومات;
    Curl_expire_clear(معلومات, EXPIRE_100_TIMEOUT);
  }
}

ساكن CURLcode cr_exp100_read(بنية Curl_easy *معلومات,
                               بنية Curl_creader *reader,
                               حرف *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  بنية cr_exp100_ctx *ctx = reader->ctx;
  timediff_t ms;

  بدل(ctx->state) {
  حالة EXP100_SENDING_REQUEST:
    إذا(!Curl_req_sendbuf_خالي_جايون(معلومات)) {
      /* The initial request data has not been fully sent yet. Do
       * not start the timer yet. */
      DEBUGF(infof(معلومات, "cr_exp100_read, request not full sent yet"));
      *nread = 0;
      *eos = FALSE;
      أرجع CURLE_OK;
    }
    /* We are now waiting for a reply from the server or
     * a timeout on our side IFF the request has been fully sent. */
    DEBUGF(infof(معلومات, "cr_exp100_read, start AWAITING_CONTINUE, "
                 "timeout %dms", معلومات->سيٽ.expect_100_timeout));
    ctx->state = EXP100_AWAITING_CONTINUE;
    ctx->شروعات = *Curl_pgrs_now(معلومات);
    Curl_expire_سيٽ(معلومات, EXPIRE_100_TIMEOUT,
                    معلومات->سيٽ.expect_100_timeout, &ctx->شروعات);
    *nread = 0;
    *eos = FALSE;
    أرجع CURLE_OK;
  حالة EXP100_FAILED:
    DEBUGF(infof(معلومات, "cr_exp100_read, expectation failed, error"));
    *nread = 0;
    *eos = FALSE;
    أرجع CURLE_READ_ERROR;
  حالة EXP100_AWAITING_CONTINUE:
    ms = curlx_ptimediff_ms(Curl_pgrs_now(معلومات), &ctx->شروعات);
    إذا(ms < معلومات->سيٽ.expect_100_timeout) {
      DEBUGF(infof(معلومات, "cr_exp100_read, AWAITING_CONTINUE, not expired"));
      *nread = 0;
      *eos = FALSE;
      أرجع CURLE_OK;
    }
    /* we have waited long enough, continue anyway */
    http_exp100_continue(معلومات, reader);
    infof(معلومات, "Done waiting for 100-continue");
    FALLTHROUGH();
  افتراضي:
    DEBUGF(infof(معلومات, "cr_exp100_read, pass through"));
    أرجع Curl_creader_read(معلومات, reader->ايندڙ, buf, blen, nread, eos);
  }
}

ساكن فراغ cr_exp100_پورو(بنية Curl_easy *معلومات,
                           بنية Curl_creader *reader, صحيح premature)
{
  بنية cr_exp100_ctx *ctx = reader->ctx;
  ctx->state = premature ? EXP100_FAILED : EXP100_SEND_معلومات;
  Curl_expire_clear(معلومات, EXPIRE_100_TIMEOUT);
}

ساكن ثابت بنية Curl_crtype cr_exp100 = {
  "cr-exp100",
  Curl_creader_def_init,
  cr_exp100_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  Curl_creader_def_ڪل_ڊيگهائي,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  cr_exp100_پورو,
  حجم(بنية cr_exp100_ctx)
};

ساكن CURLcode http_exp100_جوڙيو_reader(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *reader = عدم;
  CURLcode نتيجو;

  نتيجو = Curl_creader_create(&reader, معلومات, &cr_exp100, CURL_CR_PROTOCOL);
  إذا(!نتيجو)
    نتيجو = Curl_creader_جوڙيو(معلومات, reader);
  إذا(!نتيجو) {
    بنية cr_exp100_ctx *ctx = reader->ctx;
    ctx->state = EXP100_SENDING_REQUEST;
  }

  إذا(نتيجو && reader)
    Curl_creader_free(معلومات, reader);
  أرجع نتيجو;
}

ساكن فراغ http_exp100_got100(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *r = Curl_creader_get_by_type(معلومات, &cr_exp100);
  إذا(r)
    http_exp100_continue(معلومات, r);
}

ساكن bool http_exp100_is_انتظار(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *r = Curl_creader_get_by_type(معلومات, &cr_exp100);
  إذا(r) {
    بنية cr_exp100_ctx *ctx = r->ctx;
    أرجع ctx->state == EXP100_AWAITING_CONTINUE;
  }
  أرجع FALSE;
}

ساكن فراغ http_exp100_send_anyway(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *r = Curl_creader_get_by_type(معلومات, &cr_exp100);
  إذا(r)
    http_exp100_continue(معلومات, r);
}

ساكن bool http_exp100_is_selected(بنية Curl_easy *معلومات)
{
  بنية Curl_creader *r = Curl_creader_get_by_type(معلومات, &cr_exp100);
  أرجع !!r;
}

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
CURLcode Curl_http_doing_pollset(بنية Curl_easy *معلومات,
                                 بنية easy_pollset *ps)
{
  /* write mode */
  أرجع Curl_pollset_جوڙيو_ٻاهريون(معلومات, ps, معلومات->conn->sock[FIRSTSOCKET]);
}

CURLcode Curl_http_perform_pollset(بنية Curl_easy *معلومات,
                                   بنية easy_pollset *ps)
{
  بنية connectdata *conn = معلومات->conn;
  CURLcode نتيجو = CURLE_OK;

  إذا(CURL_REQ_WANT_RECV(معلومات)) {
    نتيجو = Curl_pollset_جوڙيو_اندريون(معلومات, ps, conn->sock[FIRSTSOCKET]);
  }

  /* on a "Expect: 100-continue" timed wait, do not poll for outgoing */
  إذا(!نتيجو && Curl_req_want_send(معلومات) && !http_exp100_is_انتظار(معلومات)) {
    نتيجو = Curl_pollset_جوڙيو_ٻاهريون(معلومات, ps, conn->sock[FIRSTSOCKET]);
  }
  أرجع نتيجو;
}

ساكن CURLcode http_write_سرو(بنية Curl_easy *معلومات,
                                  ثابت حرف *hd, size_t hdlen)
{
  CURLcode نتيجو;
  صحيح writetype;

  /* now, only output this if the header AND body are requested:
   */
  Curl_debug(معلومات, CURLINFO_سرو_اندريون, hd, hdlen);

  writetype = CLIENTWRITE_سرو |
    ((معلومات->req.httpcode / 100 == 1) ? CLIENTWRITE_1XX : 0);

  نتيجو = Curl_client_write(معلومات, writetype, hd, hdlen);
  إذا(نتيجو)
    أرجع نتيجو;

  نتيجو = Curl_bump_headersize(معلومات, hdlen, FALSE);
  إذا(نتيجو)
    أرجع نتيجو;

  معلومات->req.deductheadercount = (100 <= معلومات->req.httpcode &&
                                 199 >= معلومات->req.httpcode) ?
    معلومات->req.headerbytecount : 0;
  أرجع نتيجو;
}

/*
 * Curl_http_done() gets called after a single HTTP request has been
 * performed.
 */

CURLcode Curl_http_پورو(بنية Curl_easy *معلومات,
                        CURLcode حالت, bool premature)
{
  بنية connectdata *conn = معلومات->conn;

  /* Clear multipass flag. If authentication is not done yet, then it will get
   * a chance to be set back to true when we output the next auth header */
  معلومات->state.authhost.multipass = FALSE;
  معلومات->state.authproxy.multipass = FALSE;

  إذا(curlx_dyn_len(&معلومات->state.headerb)) {
    (فراغ)http_write_سرو(معلومات, curlx_dyn_ptr(&معلومات->state.headerb),
                            curlx_dyn_len(&معلومات->state.headerb));
  }
  curlx_dyn_reset(&معلومات->state.headerb);

  إذا(حالت)
    أرجع حالت;

  إذا(!premature && /* this check is pointless when DONE is called before the
                      entire operation is complete */
     !conn->بٽ.retry &&
     !معلومات->سيٽ.connect_only &&
     (معلومات->req.bytecount +
      معلومات->req.headerbytecount -
      معلومات->req.deductheadercount) <= 0) {
    /* If this connection is not closed to be retried, AND nothing was
       read from the HTTP server (that counts), this cannot be right so we
       return an error here */
    failf(معلومات, "Empty reply from server");
    /* Mark it as closed to avoid the "left intact" message */
    streamclose(conn);
    أرجع CURLE_GOT_NOTHING;
  }

  أرجع CURLE_OK;
}

/* Determine if we may use HTTP 1.1 for this request. */
ساكن bool http_may_use_1_1(ثابت بنية Curl_easy *معلومات)
{
  ثابت بنية connectdata *conn = معلومات->conn;
  /* We have seen a previous response for *this* transfer with 1.0,
   * on another connection or the same one. */
  إذا(معلومات->state.http_neg.rcvd_min == 10)
    أرجع FALSE;
  /* We have seen a previous response on *this* connection with 1.0. */
  إذا(conn && conn->httpversion_ڏٺل == 10)
    أرجع FALSE;
  /* We want 1.0 and have seen no previous response on *this* connection
     with a higher version (maybe no response at all yet). */
  إذا(معلومات->state.http_neg.only_10 &&
     (!conn || conn->httpversion_ڏٺل <= 10))
    أرجع FALSE;
  /* We are not restricted to use 1.0 only. */
  أرجع !معلومات->state.http_neg.only_10;
}

ساكن بنانشان حرف http_request_نسخو(بنية Curl_easy *معلومات)
{
  بنانشان حرف v = Curl_conn_http_نسخو(معلومات, معلومات->conn);
  إذا(!v) {
    /* No specific HTTP connection filter installed. */
    v = http_may_use_1_1(معلومات) ? 11 : 10;
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

CURLcode Curl_جوڙيو_custom_سرا(بنية Curl_easy *معلومات,
                                 bool is_connect, صحيح httpversion,
                                 بنية dynbuf *req)
{
  بنية curl_slist *h[2];
  بنية curl_slist *سرا;
  صحيح numlists = 1; /* by default */
  صحيح i;

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  تعداد Curl_proxy_use proxy;

  إذا(is_connect)
    proxy = سرو_CONNECT;
  وإلا
    proxy = معلومات->conn->بٽ.origin_is_proxy ? سرو_PROXY : سرو_SERVER;

  بدل(proxy) {
  حالة سرو_SERVER:
    h[0] = معلومات->سيٽ.سرا;
    اكسر;
  حالة سرو_PROXY:
    h[0] = معلومات->سيٽ.سرا;
    إذا(معلومات->سيٽ.sep_سرا) {
      h[1] = معلومات->سيٽ.proxyheaders;
      numlists++;
    }
    اكسر;
  حالة سرو_CONNECT:
    إذا(معلومات->سيٽ.sep_سرا)
      h[0] = معلومات->سيٽ.proxyheaders;
    وإلا
      h[0] = معلومات->سيٽ.سرا;
    اكسر;
  حالة سرو_CONNECT_UDP:
    إذا(معلومات->سيٽ.sep_سرا)
      h[0] = معلومات->سيٽ.proxyheaders;
    وإلا
      h[0] = معلومات->سيٽ.سرا;
    اكسر;
  }
#وإلا
  (فراغ)is_connect;
  h[0] = معلومات->سيٽ.سرا;
#جيڪڏهن_پورو

  /* loop through one or two lists */
  لكل(i = 0; i < numlists; i++) {
    لكل(سرا = h[i]; سرا; سرا = سرا->ايندڙ) {
      CURLcode نتيجو = CURLE_OK;
      bool blankheader = FALSE;
      بنية Curl_str name;
      ثابت حرف *p = سرا->معلومات;
      ثابت حرف *origp = p;
      size_t hlen = لڙي_ڊيگهه(origp);

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

      إذا(معلومات->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom
            Host: header as that will produce *two* in the same
            request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      وإلا إذا(معلومات->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      وإلا إذا(معلومات->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      وإلا إذا(معلومات->req.authneg &&
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
              !Curl_auth_allowed_منزل_host(معلومات))
        ;
      وإلا إذا(blankheader) {
        نتيجو = curlx_dyn_addn(req, curlx_str(&name), curlx_strlen(&name));
        إذا(!نتيجو)
          نتيجو = curlx_dyn_addn(req, STRCONST(":\r\n"));
      }
      وإلا
        نتيجو = curlx_dyn_addf(req, "%s\r\n", origp);

      إذا(نتيجو)
        أرجع نتيجو;
    }
  }

  أرجع CURLE_OK;
}

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PARSEDATE
CURLcode Curl_جوڙيو_timecondition(بنية Curl_easy *معلومات,
                                بنية dynbuf *req)
{
  ثابت بنية tm *tm;
  بنية tm keeptime;
  CURLcode نتيجو;
  حرف datestr[80];
  ثابت حرف *condp;
  size_t len;

  إذا(معلومات->سيٽ.timecondition == CURL_TIMECOND_NONE)
    /* no condition was asked for */
    أرجع CURLE_OK;

  نتيجو = curlx_gmtime(معلومات->سيٽ.timevalue, &keeptime);
  إذا(نتيجو) {
    failf(معلومات, "Invalid TIMEVALUE");
    أرجع نتيجو;
  }
  tm = &keeptime;

  بدل(معلومات->سيٽ.timecondition) {
  افتراضي:
    DEBUGF(infof(معلومات, "invalid time condition"));
    أرجع CURLE_BAD_FUNCTION_دليل;

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

  إذا(Curl_checkheaders(معلومات, condp, len)) {
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

  نتيجو = curlx_dyn_جوڙيو(req, datestr);
  أرجع نتيجو;
}
#وإلا
/* disabled */
CURLcode Curl_جوڙيو_timecondition(بنية Curl_easy *معلومات,
                                بنية dynbuf *req)
{
  (فراغ)معلومات;
  (فراغ)req;
  أرجع CURLE_OK;
}
#جيڪڏهن_پورو

فراغ Curl_http_طريقو(بنية Curl_easy *معلومات,
                      ثابت حرف **طريقو, Curl_HttpReq *reqp)
{
  Curl_HttpReq httpreq = (Curl_HttpReq)معلومات->state.httpreq;
  ثابت حرف *request;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_WEBSOCKETS
  إذا(معلومات->conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
    httpreq = HTTPREQ_GET;
  وإلا
#جيڪڏهن_پورو
  إذا((معلومات->conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_FTP)) &&
     معلومات->state.upload)
    httpreq = HTTPREQ_PUT;

  /* Now set the 'request' pointer to the proper request string */
  إذا(!معلومات->state.http_ignorecustom &&
     CURL_EASY_STR(معلومات, STRING_CUSTOMREQUEST)) {
    request = CURL_EASY_STR(معلومات, STRING_CUSTOMREQUEST);
  }
  وإلا {
    إذا(معلومات->req.no_body)
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
  *طريقو = request;
  *reqp = httpreq;
}

ساكن CURLcode http_سيٽ_aptr_host(بنية Curl_easy *معلومات)
{
  بنية connectdata *conn = معلومات->conn;
  ثابت حرف *ptr = عدم;

  curlx_safefree(معلومات->state.http_host);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_COOKIES
  curlx_safefree(معلومات->req.cookiehost);
#جيڪڏهن_پورو

  إذا(Curl_peer_equal(معلومات->state.initial_origin, معلومات->state.origin))
    ptr = Curl_checkheaders(معلومات, STRCONST("Host"));

  إذا(ptr) {
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_COOKIES
    /* If we have a given custom Host: header, we extract the hostname in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the hostname
       is the same as the first one! */
    حرف *cookiehost;
    CURLcode نتيجو = copy_custom_قدر(ptr, &cookiehost);
    إذا(نتيجو)
      أرجع نتيجو;
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
        memmove(cookiehost, cookiehost + 1, لڙي_ڊيگهه(cookiehost) - 1);
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
      معلومات->req.cookiehost = cookiehost;
    }
#جيڪڏهن_پورو

    إذا(!curl_strequal("Host:", ptr)) {
      معلومات->state.http_host = curl_maprintf("Host:%s", &ptr[5]);
      إذا(!معلومات->state.http_host)
        أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
  }
  وإلا {
    /* This is the  HTTP Host: header, so we want
     * - for IPv6 origins: "[ipv6-address]" where the IPv6 address is
     *  found in origin->hostname, stripped of zoneid/scopeid.
     * - the (IDN converted) origin->hostname (DNS name or IPv4) otherwise.
     * Note: zoneid/scopeid  only applies to local routing and has no
     * meaning on the remote HTTP server (eg. would confuse it). */
    bool ipv6 = (bool)معلومات->state.origin->ipv6;
    بنية dynbuf tmp;
    size_t hlen;
    CURLcode نتيجو;

    curlx_dyn_init(&tmp, DYN_HTTP_REQUEST);
    نتيجو = curlx_dyn_addn(&tmp, STRCONST("Host: "));
    إذا(!نتيجو && ipv6)
      نتيجو = curlx_dyn_addn(&tmp, STRCONST("["));
    إذا(!نتيجو)
      نتيجو = curlx_dyn_جوڙيو(&tmp, معلومات->state.origin->hostname);
    إذا(!نتيجو && ipv6)
      نتيجو = curlx_dyn_addn(&tmp, STRCONST("]"));
    إذا(!نتيجو &&
       ((معلومات->state.origin->پورٽ != معلومات->state.origin->scheme->defport) ||
       (معلومات->state.origin->scheme->family != conn->scheme->family))) {
      نتيجو = curlx_dyn_addf(&tmp, ":%u", معلومات->state.origin->پورٽ);
    }

    معلومات->state.http_host = نتيجو ? عدم : curlx_dyn_take(&tmp, &hlen);
    curlx_dyn_free(&tmp);
    أرجع نتيجو;
  }
  أرجع CURLE_OK;
}

/*
 * Append the request-target to the HTTP request
 */
ساكن CURLcode http_هدف(بنية Curl_easy *معلومات,
                            بنية dynbuf *r)
{
  CURLcode نتيجو = CURLE_OK;
  ثابت حرف *رستو = معلومات->state.مٿي.رستو;
  ثابت حرف *query = معلومات->state.مٿي.query;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  بنية connectdata *conn = معلومات->conn;
#جيڪڏهن_پورو

  إذا(CURL_EASY_STR(معلومات, STRING_هدف)) {
    رستو = CURL_EASY_STR(معلومات, STRING_هدف);
    query = عدم;
  }

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  إذا(conn->بٽ.origin_is_proxy) {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL, but if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the decoded hostname! */

    /* and no fragment part */
    CURLUcode uc;
    حرف *url;
    CURLU *h = curl_url_dup(معلومات->state.uh);
    إذا(!h)
      أرجع CURLE_ٻاهريون_جو_يادگيري;

    إذا(!معلومات->state.origin->ipv6 &&
       (معلومات->state.origin->user_hostname != معلومات->state.origin->hostname)) {
      uc = curl_url_سيٽ(h, CURLUPART_HOST, معلومات->state.origin->hostname, 0);
      إذا(uc) {
        curl_url_cleanup(h);
        أرجع CURLE_ٻاهريون_جو_يادگيري;
      }
    }
    uc = curl_url_سيٽ(h, CURLUPART_FRAGMENT, عدم, 0);
    إذا(uc) {
      curl_url_cleanup(h);
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    }

    إذا(معلومات->state.origin->scheme == &Curl_scheme_http) {
      /* when getting HTTP, we do not want the userinfo the URL */
      uc = curl_url_سيٽ(h, CURLUPART_USER, عدم, 0);
      إذا(uc) {
        curl_url_cleanup(h);
        أرجع CURLE_ٻاهريون_جو_يادگيري;
      }
      uc = curl_url_سيٽ(h, CURLUPART_PASSWORD, عدم, 0);
      إذا(uc) {
        curl_url_cleanup(h);
        أرجع CURLE_ٻاهريون_جو_يادگيري;
      }
    }
    وإلا إذا(معلومات->state.creds && (معلومات->state.creds->ذريعو != CREDS_URL)) {
        /* credentials not from the URL need to be set */
      uc = curl_url_سيٽ(h, CURLUPART_USER,
                        معلومات->state.creds->user, CURLU_URLENCODE);
      إذا(!uc)
        uc = curl_url_سيٽ(h, CURLUPART_PASSWORD,
                          معلومات->state.creds->passwd, CURLU_URLENCODE);
      إذا(uc) {
        curl_url_cleanup(h);
        أرجع Curl_uc_منزل_curlcode(uc);
      }
    }

    /* Extract the URL to use in the request. */
    uc = curl_url_get(h, CURLUPART_URL, &url, CURLU_NO_DEFAULT_پورٽ);
    إذا(uc) {
      curl_url_cleanup(h);
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    }

    curl_url_cleanup(h);

    /* target or URL */
    نتيجو = curlx_dyn_جوڙيو(r, CURL_EASY_STR(معلومات, STRING_هدف) ?
      CURL_EASY_STR(معلومات, STRING_هدف) : url);
    curlx_free(url);
    إذا(نتيجو)
      أرجع نتيجو;

    إذا((معلومات->state.origin->scheme == &Curl_scheme_ftp) &&
       معلومات->سيٽ.proxy_transfer_گهڻائي) {
      /* when doing ftp, append ;type=<a|i> if not present */
      size_t len = لڙي_ڊيگهه(رستو);
      bool type_present = FALSE;
      إذا((len >= 7) && !memcmp(&رستو[len - 7], ";type=", 6)) {
        بدل(Curl_raw_toupper(رستو[len - 1])) {
        حالة 'A':
        حالة 'D':
        حالة 'I':
          type_present = TRUE;
          اكسر;
        }
      }
      إذا(!type_present) {
        نتيجو = curlx_dyn_addf(r, ";type=%c",
                                معلومات->state.prefer_ascii ? 'a' : 'i');
        إذا(نتيجو)
          أرجع نتيجو;
      }
    }
  }

  وإلا
#جيڪڏهن_پورو
  {
    نتيجو = curlx_dyn_جوڙيو(r, رستو);
    إذا(نتيجو)
      أرجع نتيجو;
    إذا(query)
      نتيجو = curlx_dyn_addf(r, "?%s", query);
  }

  أرجع نتيجو;
}

#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
ساكن CURLcode سيٽ_post_reader(بنية Curl_easy *معلومات, Curl_HttpReq httpreq)
{
  CURLcode نتيجو;

  بدل(httpreq) {
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_MIME
  حالة HTTPREQ_POST_MIME:
    معلومات->state.mimepost = معلومات->سيٽ.mimepostp;
    اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FORM_API
  حالة HTTPREQ_POST_FORM:
    /* Convert the form structure into a mime structure, then keep
       the conversion */
    إذا(!معلومات->state.formp) {
      معلومات->state.formp = curlx_calloc(1, حجم(curl_mimepart));
      إذا(!معلومات->state.formp)
        أرجع CURLE_ٻاهريون_جو_يادگيري;
      Curl_mime_cleanpart(معلومات->state.formp);
      نتيجو = Curl_getformdata(معلومات, معلومات->state.formp, معلومات->سيٽ.httppost,
                                معلومات->state.fread_func);
      إذا(نتيجو) {
        curlx_safefree(معلومات->state.formp);
        أرجع نتيجو;
      }
      معلومات->state.mimepost = معلومات->state.formp;
    }
    اكسر;
#جيڪڏهن_پورو
  افتراضي:
    معلومات->state.mimepost = عدم;
    اكسر;
  }

  بدل(httpreq) {
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
    /* This is form posting using mime data. */
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_MIME
    إذا(معلومات->state.mimepost) {
      ثابت حرف *cthdr = Curl_checkheaders(معلومات, STRCONST("Content-Type"));

      /* Read and seek body only. */
      معلومات->state.mimepost->flags |= MIME_BODY_ONLY;

      /* Prepare the mime structure headers & set content type. */

      إذا(cthdr)
        لكل(cthdr += 13; *cthdr == ' '; cthdr++)
          ;
      وإلا إذا(معلومات->state.mimepost->قسم == MIMEKIND_MULTIPART)
        cthdr = "multipart/form-data";

      curl_mime_سرا(معلومات->state.mimepost, معلومات->سيٽ.سرا, 0);
      نتيجو = Curl_mime_prepare_سرا(معلومات, معلومات->state.mimepost, cthdr,
                                         عدم, MIMESTRATEGY_FORM);
      إذا(نتيجو)
        أرجع نتيجو;
      curl_mime_سرا(معلومات->state.mimepost, عدم, 0);
      نتيجو = Curl_creader_سيٽ_mime(معلومات, معلومات->state.mimepost);
      إذا(نتيجو)
        أرجع نتيجو;
    }
    وإلا
#جيڪڏهن_پورو
    {
      نتيجو = Curl_creader_سيٽ_null(معلومات);
    }
    معلومات->state.infilesize = Curl_creader_ڪل_ڊيگهائي(معلومات);
    أرجع نتيجو;

  افتراضي:
    أرجع Curl_creader_سيٽ_null(معلومات);
  }
  /* never reached */
}
#جيڪڏهن_پورو

ساكن CURLcode سيٽ_reader(بنية Curl_easy *معلومات, Curl_HttpReq httpreq)
{
  CURLcode نتيجو = CURLE_OK;
  curl_off_t postsize = معلومات->state.infilesize;

  DEBUGASSERT(معلومات->conn);

  إذا(معلومات->req.authneg) {
    أرجع Curl_creader_سيٽ_null(معلومات);
  }

  بدل(httpreq) {
  حالة HTTPREQ_PUT: /* Let's PUT the data to the server! */
    أرجع postsize ? Curl_creader_سيٽ_fread(معلومات, postsize) :
      Curl_creader_سيٽ_null(معلومات);

#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
    أرجع سيٽ_post_reader(معلومات, httpreq);
#جيڪڏهن_پورو

  حالة HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */
    /* the size of the post body */
    إذا(!postsize) {
      نتيجو = Curl_creader_سيٽ_null(معلومات);
    }
    وإلا إذا(معلومات->سيٽ.postfields) {
      size_t plen = curlx_sotouz_range(postsize, 0, SIZE_MAX);
      إذا(plen == SIZE_MAX)
        أرجع CURLE_ٻاهريون_جو_يادگيري;
      وإلا إذا(plen)
        نتيجو = Curl_creader_سيٽ_buf(معلومات, معلومات->سيٽ.postfields, plen);
      وإلا
        نتيجو = Curl_creader_سيٽ_null(معلومات);
    }
    وإلا {
      /* we read the bytes from the callback. In case "chunked" encoding
       * is forced by the application, we disregard `postsize`. This is
       * a backward compatibility decision to earlier versions where
       * chunking disregarded this. See issue #13229. */
      bool chunked = FALSE;
      حرف *ptr = Curl_checkheaders(معلومات, STRCONST("Transfer-Encoding"));
      إذا(ptr) {
        /* Some kind of TE is requested, check if 'chunked' is chosen */
        chunked = Curl_compareheader(ptr, STRCONST("Transfer-Encoding:"),
                                     STRCONST("chunked"));
      }
      نتيجو = Curl_creader_سيٽ_fread(معلومات, chunked ? -1 : postsize);
    }
    أرجع نتيجو;

  افتراضي:
    /* HTTP GET/HEAD download, has no body, needs no Content-Length */
    معلومات->state.infilesize = 0;
    أرجع Curl_creader_سيٽ_null(معلومات);
  }
  /* not reached */
}

ساكن CURLcode http_resume(بنية Curl_easy *معلومات, Curl_HttpReq httpreq)
{
  إذا((HTTPREQ_POST == httpreq || HTTPREQ_POST_FORM == httpreq ||
      HTTPREQ_POST_MIME == httpreq || HTTPREQ_PUT == httpreq) &&
     معلومات->state.resume_from) {
    failf(معلومات, "HTTP upload cannot be resumed");
    أرجع CURLE_BAD_FUNCTION_دليل;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode http_req_سيٽ_TE(بنية Curl_easy *معلومات,
                                بنية dynbuf *req,
                                صحيح httpversion)
{
  CURLcode نتيجو = CURLE_OK;
  ثابت حرف *ptr;

  ptr = Curl_checkheaders(معلومات, STRCONST("Transfer-Encoding"));
  إذا(ptr) {
    /* Some kind of TE is requested, check if 'chunked' is chosen */
    معلومات->req.upload_chunky =
      Curl_compareheader(ptr,
                         STRCONST("Transfer-Encoding:"), STRCONST("chunked"));
    إذا(معلومات->req.upload_chunky && (httpversion >= 20)) {
      infof(معلومات, "suppressing chunked transfer encoding on connection "
            "using HTTP version 2 or higher");
      معلومات->req.upload_chunky = FALSE;
    }
  }
  وإلا {
    curl_off_t req_clen = Curl_creader_ڪل_ڊيگهائي(معلومات);

    إذا(req_clen < 0) {
      /* indeterminate request content length */
      إذا(httpversion > 10) {
        /* On HTTP/1.1, enable chunked, on HTTP/2 and later we do not
         * need it */
        معلومات->req.upload_chunky = (httpversion < 20);
      }
      وإلا {
        failf(معلومات, "Chunky upload is not supported by HTTP 1.0");
        أرجع CURLE_UPLOAD_FAILED;
      }
    }
    وإلا {
      /* else, no chunky upload */
      معلومات->req.upload_chunky = FALSE;
    }

    إذا(معلومات->req.upload_chunky)
      نتيجو = curlx_dyn_جوڙيو(req, "Transfer-Encoding: chunked\r\n");
  }
  أرجع نتيجو;
}

ساكن CURLcode addexpect(بنية Curl_easy *معلومات, بنية dynbuf *r,
                          صحيح httpversion, bool *announced_exp100)
{
  CURLcode نتيجو;
  حرف *ptr;

  *announced_exp100 = FALSE;
  /* Avoid Expect: 100-continue if Upgrade: is used */
  إذا(معلومات->req.upgr101 != UPGR101_NONE)
    أرجع CURLE_OK;

  /* For really small puts we do not use Expect: headers at all, and for
     the somewhat bigger ones we allow the app to disable it. Make
     sure that the expect100header is always set to the preferred value
     here. */
  ptr = Curl_checkheaders(معلومات, STRCONST("Expect"));
  إذا(ptr) {
    *announced_exp100 =
      Curl_compareheader(ptr, STRCONST("Expect:"), STRCONST("100-continue"));
  }
  وإلا إذا(!معلومات->state.disableexpect && (httpversion == 11)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    curl_off_t client_len = Curl_creader_client_ڊيگهائي(معلومات);
    إذا(client_len > EXPECT_100_حد || client_len < 0) {
      نتيجو = curlx_dyn_addn(r, STRCONST("Expect: 100-continue\r\n"));
      إذا(نتيجو)
        أرجع نتيجو;
      *announced_exp100 = TRUE;
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode http_جوڙيو_content_hds(بنية Curl_easy *معلومات,
                                     بنية dynbuf *r,
                                     صحيح httpversion,
                                     Curl_HttpReq httpreq)
{
  CURLcode نتيجو = CURLE_OK;
  curl_off_t req_clen;
  bool announced_exp100 = FALSE;

  DEBUGASSERT(معلومات->conn);
  إذا(معلومات->req.upload_chunky) {
    نتيجو = Curl_httpchunk_جوڙيو_reader(معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  /* Get the request body length that has been set up */
  req_clen = Curl_creader_ڪل_ڊيگهائي(معلومات);
  بدل(httpreq) {
  حالة HTTPREQ_PUT:
  حالة HTTPREQ_POST:
#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
#جيڪڏهن_پورو
    /* We only set Content-Length and allow a custom Content-Length if
       we do not upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length).
       We do not override a custom "Content-Length" header, but during
       authentication negotiation that header is suppressed.
     */
    إذا(req_clen >= 0 && !معلومات->req.upload_chunky &&
       (معلومات->req.authneg ||
        !Curl_checkheaders(معلومات, STRCONST("Content-Length")))) {
      /* we allow replacing this header if not during auth negotiation,
         although it is not wise to actually set your own */
      نتيجو = curlx_dyn_addf(r, "Content-Length: %" FMT_OFF_T "\r\n",
                              req_clen);
    }
    إذا(نتيجو)
      اذهب ٻاهريون;

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_MIME
    /* Output mime-generated headers. */
    إذا(معلومات->state.mimepost &&
       ((httpreq == HTTPREQ_POST_FORM) || (httpreq == HTTPREQ_POST_MIME))) {
      بنية curl_slist *hdr;

      لكل(hdr = معلومات->state.mimepost->curlheaders; hdr; hdr = hdr->ايندڙ) {
        نتيجو = curlx_dyn_addf(r, "%s\r\n", hdr->معلومات);
        إذا(نتيجو)
          اذهب ٻاهريون;
      }
    }
#جيڪڏهن_پورو
    إذا(httpreq == HTTPREQ_POST &&
       !Curl_checkheaders(معلومات, STRCONST("Content-Type"))) {
      نتيجو = curlx_dyn_addn(r, STRCONST("Content-Type: application/"
                                          "x-www-form-urlencoded\r\n"));
      إذا(نتيجو)
        اذهب ٻاهريون;
    }
    نتيجو = addexpect(معلومات, r, httpversion, &announced_exp100);
    إذا(نتيجو)
      اذهب ٻاهريون;
    اكسر;
  افتراضي:
    اكسر;
  }

  Curl_pgrs_سيٽ_Upload_سائيز(معلومات, req_clen);
  إذا(announced_exp100)
    نتيجو = http_exp100_جوڙيو_reader(معلومات);

ٻاهريون:
  أرجع نتيجو;
}

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_COOKIES

ساكن CURLcode http_cookies(بنية Curl_easy *معلومات,
                             بنية dynbuf *r)
{
  CURLcode نتيجو = CURLE_OK;
  ثابت حرف *addcookies = عدم;
  bool linecap = FALSE;
  إذا(CURL_EASY_STR(معلومات, STRING_COOKIE) &&
     !Curl_checkheaders(معلومات, STRCONST("Cookie")) &&
     Curl_auth_allowed_منزل_host(معلومات))
    addcookies = CURL_EASY_STR(معلومات, STRING_COOKIE);

  إذا(معلومات->cookies || addcookies) {
    بنية Curl_llist list;
    صحيح ڳڻپ = 0;

    إذا(معلومات->cookies && معلومات->state.cookie_engine) {
      bool okay;
      ثابت حرف *host = معلومات->req.cookiehost ?
        معلومات->req.cookiehost : معلومات->state.origin->hostname;
      Curl_share_تالو(معلومات, CURL_تالو_معلومات_COOKIE, CURL_تالو_ACCESS_SINGLE);
      نتيجو = Curl_cookie_getlist(معلومات, &okay, host, &list);
      إذا(!نتيجو && okay) {
        بنية Curl_llist_ڳنڍ *n;
        size_t clen = 8; /* hold the size of the generated Cookie: header */

        /* loop through all cookies that matched */
        لكل(n = Curl_llist_سر(&list); n; n = Curl_ڳنڍ_ايندڙ(n)) {
          بنية Cookie *co = Curl_ڳنڍ_elem(n);
          إذا(co->قدر) {
            size_t جوڙيو;
            إذا(!ڳڻپ) {
              نتيجو = curlx_dyn_addn(r, STRCONST("Cookie: "));
              إذا(نتيجو)
                اكسر;
            }
            جوڙيو = لڙي_ڊيگهه(co->name) + لڙي_ڊيگهه(co->قدر) + 1;
            إذا(clen + جوڙيو >= MAX_COOKIE_سرو_LEN) {
              infof(معلومات, "Restricted outgoing cookies due to header size, "
                    "'%s' not sent", co->name);
              linecap = TRUE;
              اكسر;
            }
            نتيجو = curlx_dyn_addf(r, "%s%s=%s", ڳڻپ ? "; " : "",
                                    co->name, co->قدر);
            إذا(نتيجو)
              اكسر;
            clen += جوڙيو + (ڳڻپ ? 2 : 0);
            ڳڻپ++;
          }
        }
        Curl_llist_destroy(&list, عدم);
      }
      Curl_share_unlock(معلومات, CURL_تالو_معلومات_COOKIE);
    }
    إذا(addcookies && !نتيجو && !linecap) {
      إذا(!ڳڻپ)
        نتيجو = curlx_dyn_addn(r, STRCONST("Cookie: "));
      إذا(!نتيجو) {
        نتيجو = curlx_dyn_addf(r, "%s%s", ڳڻپ ? "; " : "", addcookies);
        ڳڻپ++;
      }
    }
    إذا(ڳڻپ && !نتيجو)
      نتيجو = curlx_dyn_addn(r, STRCONST("\r\n"));

    إذا(نتيجو)
      أرجع نتيجو;
  }
  أرجع نتيجو;
}
#وإلا
#وصف_ڪريو http_cookies(a, b) CURLE_OK
#جيڪڏهن_پورو

ساكن CURLcode http_range(بنية Curl_easy *معلومات,
                           Curl_HttpReq httpreq)
{
  إذا(معلومات->state.use_range) {
    /*
     * A range is selected. We use different headers whether we are downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    إذا(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_سر)) &&
       !Curl_checkheaders(معلومات, STRCONST("Range"))) {
      /* if a line like this was already allocated, free the previous one */
      curlx_free(معلومات->state.rangeline);
      معلومات->state.rangeline = curl_maprintf("Range: bytes=%s\r\n",
                                                 معلومات->state.range);
      إذا(!معلومات->state.rangeline)
        أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
    وإلا إذا((httpreq == HTTPREQ_POST || httpreq == HTTPREQ_PUT) &&
            !Curl_checkheaders(معلومات, STRCONST("Content-Range"))) {
      curl_off_t req_clen = Curl_creader_ڪل_ڊيگهائي(معلومات);
      /* if a line like this was already allocated, free the previous one */
      curlx_free(معلومات->state.rangeline);

      إذا(معلومات->سيٽ.سيٽ_resume_from < 0) {
        /* Upload resume was asked for, but we do not know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        معلومات->state.rangeline =
          curl_maprintf("Content-Range: bytes 0-%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n", req_clen - 1, req_clen);
      }
      وإلا إذا(معلومات->state.resume_from) {
        /* This is because "resume" was selected */
        /* Not sure if we want to send this header during authentication
         * negotiation, but test1084 checks for it. In which case we have a
         * "null" client reader installed that gives an unexpected length. */
        curl_off_t ڪل_len = معلومات->req.authneg ?
                               معلومات->state.infilesize :
                               (معلومات->state.resume_from + req_clen);
        معلومات->state.rangeline =
          curl_maprintf("Content-Range: bytes %s%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n",
                        معلومات->state.range, ڪل_len - 1, ڪل_len);
      }
      وإلا {
        /* Range was selected and then we pass the incoming range and append
           total size */
        معلومات->state.rangeline =
          curl_maprintf("Content-Range: bytes %s/%" FMT_OFF_T "\r\n",
                        معلومات->state.range, req_clen);
      }
      إذا(!معلومات->state.rangeline)
        أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode http_firstwrite(بنية Curl_easy *معلومات)
{
  بنية connectdata *conn = معلومات->conn;
  بنية SingleRequest *k = &معلومات->req;

  إذا(معلومات->req.newurl) {
    إذا(conn->بٽ.close) {
      /* Abort after the headers if "follow Location" is set
         and we are set to close anyway. */
      CURL_REQ_CLEAR_RECV(معلومات);
      k->پورو = TRUE;
      أرجع CURLE_OK;
    }
    /* We have a new URL to load, but since we want to be able to reuse this
       connection properly, we read the full response in "ignore more" */
    k->ignorebody = TRUE;
    infof(معلومات, "Ignoring the response-body");
  }
  إذا(معلومات->state.resume_from && !k->content_range &&
     (معلومات->state.httpreq == HTTPREQ_GET) &&
     !k->ignorebody) {

    إذا(k->سائيز == معلومات->state.resume_from) {
      /* The resume point is at the end of file, consider this fine even if it
         does not allow resume from here. */
      infof(معلومات, "The entire document is already downloaded");
      streamclose(conn);
      /* Abort download */
      CURL_REQ_CLEAR_RECV(معلومات);
      k->پورو = TRUE;
      أرجع CURLE_OK;
    }

    /* we wanted to resume a download, although the server does not seem to
     * support this and we did this with a GET (if it was not a GET we did a
     * POST or PUT resume) */
    failf(معلومات, "HTTP server does not seem to support "
          "byte ranges. Cannot resume.");
    أرجع CURLE_RANGE_ERROR;
  }

  إذا(معلومات->سيٽ.timecondition && !معلومات->state.range &&
     /* A time condition has been set AND no ranges have been requested. This
        seems to be what chapter 13.3.4 of RFC 2616 defines to be the correct
        action for an HTTP/1.1 client */
     !Curl_meets_timecondition(معلومات, k->timeofdoc)) {
    k->پورو = TRUE;
    /* We are simulating an HTTP 304 from server so we return
       what should have been returned from the server */
    معلومات->info.httpcode = 304;
    infof(معلومات, "Simulate an HTTP 304 response");
    /* we abort the transfer before it is completed == we ruin the
       reuse ability. Close the connection */
    streamclose(conn);
    أرجع CURLE_OK;
  } /* we have a time condition */

  أرجع CURLE_OK;
}

ساكن CURLcode http_check_new_conn(بنية Curl_easy *معلومات)
{
  بنية connectdata *conn = معلومات->conn;
  ثابت حرف *info_نسخو = عدم;
  ثابت حرف *alpn;
  CURLcode نتيجو;

  alpn = Curl_conn_get_alpn_negotiated(معلومات, conn);
  إذا(alpn && !strcmp("h3", alpn)) {
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
    إذا(!conn->بٽ.origin_is_proxy)
#جيڪڏهن_پورو
      DEBUGASSERT(Curl_conn_http_نسخو(معلومات, conn) == 30);
    info_نسخو = "HTTP/3";
  }
  وإلا إذا(alpn && !strcmp("h2", alpn)) {
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
    إذا((Curl_conn_http_نسخو(معلومات, conn) != 20) &&
       conn->بٽ.origin_is_proxy) {
      نتيجو = Curl_http2_switch(معلومات);
      إذا(نتيجو)
        أرجع نتيجو;
    }
    وإلا
#جيڪڏهن_پورو
    DEBUGASSERT(Curl_conn_http_نسخو(معلومات, conn) == 20);
    info_نسخو = "HTTP/2";
  }
  وإلا {
    /* Check if user wants to use HTTP/2 with clear TCP */
    إذا(Curl_http2_may_switch(معلومات)) {
      DEBUGF(infof(معلومات, "HTTP/2 over clean TCP"));
      نتيجو = Curl_http2_switch(معلومات);
      إذا(نتيجو)
        أرجع نتيجو;
      info_نسخو = "HTTP/2";
      /* There is no ALPN here, but the connection is now definitely h2 */
      conn->httpversion_ڏٺل = 20;
      Curl_conn_سيٽ_multiplex(conn);
    }
    وإلا
      info_نسخو = "HTTP/1.x";
  }

  إذا(info_نسخو)
    infof(معلومات, "using %s", info_نسخو);
  أرجع CURLE_OK;
}

ساكن CURLcode http_جوڙيو_connection_hd(بنية Curl_easy *معلومات,
                                       بنية dynbuf *req)
{
  بنية curl_slist *سر;
  ثابت حرف *sep = "Connection: ";
  CURLcode نتيجو = CURLE_OK;
  size_t rlen = curlx_dyn_len(req);
  bool skip;

  /* Add the 1st custom "Connection: " header, if there is one */
  لكل(سر = معلومات->سيٽ.سرا; سر; سر = سر->ايندڙ) {
    إذا(curl_strnequal(سر->معلومات, "Connection", 10) &&
       Curl_headersep(سر->معلومات[10]) &&
       !http_سرو_is_خالي_جايون(سر->معلومات)) {
      حرف *قدر;
      نتيجو = copy_custom_قدر(سر->معلومات, &قدر);
      إذا(نتيجو)
        أرجع نتيجو;
      نتيجو = curlx_dyn_addf(req, "%s%s", sep, قدر);
      sep = ", ";
      curlx_free(قدر);
      اكسر; /* leave, having added 1st one */
    }
  }

  /* add our internal Connection: header values, if we have any */
  إذا(!نتيجو && معلومات->state.http_hd_te) {
    نتيجو = curlx_dyn_addf(req, "%s%s", sep, "TE");
    sep = ", ";
  }
  إذا(!نتيجو && معلومات->state.http_hd_upgrade) {
    نتيجو = curlx_dyn_addf(req, "%s%s", sep, "Upgrade");
    sep = ", ";
  }
  إذا(!نتيجو && معلومات->state.http_hd_h2_settings) {
    نتيجو = curlx_dyn_addf(req, "%s%s", sep, "HTTP2-Settings");
  }
  إذا(!نتيجو && (rlen < curlx_dyn_len(req)))
    نتيجو = curlx_dyn_addn(req, STRCONST("\r\n"));
  إذا(نتيجو)
    أرجع نتيجو;

  /* Add all user-defined Connection: headers after the first */
  skip = TRUE;
  لكل(سر = معلومات->سيٽ.سرا; سر; سر = سر->ايندڙ) {
    إذا(curl_strnequal(سر->معلومات, "Connection", 10) &&
       Curl_headersep(سر->معلومات[10]) &&
       !http_سرو_is_خالي_جايون(سر->معلومات)) {
      إذا(skip) {
        skip = FALSE;
        استمر;
      }
      نتيجو = curlx_dyn_addf(req, "%s\r\n", سر->معلومات);
      إذا(نتيجو)
        أرجع نتيجو;
    }
  }

  أرجع CURLE_OK;
}

/* Header identifier in order we send them by default */
تعريفنوع تعداد {
  H1_HD_REQUEST,
  H1_HD_HOST,
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  H1_HD_PROXY_AUTH,
#جيڪڏهن_پورو
  H1_HD_AUTH,
  H1_HD_RANGE,
  H1_HD_USER_AGENT,
  H1_HD_ACCEPT,
  H1_HD_TE,
  H1_HD_ACCEPT_ENCODING,
  H1_HD_REFERER,
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  H1_HD_PROXY_CONNECTION,
#جيڪڏهن_پورو
  H1_HD_TRANSFER_ENCODING,
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_ALTSVC
  H1_HD_ALT_استعمال_ٿيل,
#جيڪڏهن_پورو
  H1_HD_UPGRADE,
  H1_HD_COOKIES,
  H1_HD_CONDITIONALS,
  H1_HD_CUSTOM,
  H1_HD_CONTENT,
  H1_HD_CONNECTION,
  H1_HD_آخري  /* the last, empty header line */
} http_hd_t;

ساكن CURLcode http_جوڙيو_hd(بنية Curl_easy *معلومات,
                            بنية dynbuf *req,
                            http_hd_t id,
                            بنانشان حرف httpversion,
                            ثابت حرف *طريقو,
                            Curl_HttpReq httpreq)
{
  CURLcode نتيجو = CURLE_OK;
#إذا !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_PROXY) || \
  !defined(CURL_DISABLE_WEBSOCKETS)
  بنية connectdata *conn = معلومات->conn;
#جيڪڏهن_پورو
  بدل(id) {
  حالة H1_HD_REQUEST:
    /* add the main request stuff */
    /* GET/HEAD/POST/PUT */
    نتيجو = curlx_dyn_addf(req, "%s ", طريقو);
    إذا(!نتيجو)
      نتيجو = http_هدف(معلومات, req);
    إذا(!نتيجو)
      نتيجو = curlx_dyn_addf(req, " HTTP/%s\r\n",
                              get_http_string(httpversion));
    اكسر;

  حالة H1_HD_HOST:
    إذا(معلومات->state.http_host) {
      نتيجو = curlx_dyn_جوڙيو(req, معلومات->state.http_host);
      إذا(!نتيجو)
        نتيجو = curlx_dyn_addn(req, STRCONST("\r\n"));
    }
    اكسر;

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  حالة H1_HD_PROXY_AUTH:
    إذا(معلومات->req.hd_proxy_auth)
      نتيجو = curlx_dyn_جوڙيو(req, معلومات->req.hd_proxy_auth);
    اكسر;
#جيڪڏهن_پورو

  حالة H1_HD_AUTH:
    إذا(معلومات->req.hd_auth)
      نتيجو = curlx_dyn_جوڙيو(req, معلومات->req.hd_auth);
    اكسر;

  حالة H1_HD_RANGE:
    إذا(معلومات->state.use_range && معلومات->state.rangeline)
      نتيجو = curlx_dyn_جوڙيو(req, معلومات->state.rangeline);
    اكسر;

  حالة H1_HD_USER_AGENT: {
    ثابت حرف *ua = CURL_EASY_STR(معلومات, STRING_USERAGENT);
    إذا(ua && *ua && !Curl_checkheaders(معلومات, STRCONST("User-Agent")))
      نتيجو = curlx_dyn_addf(req, "User-Agent: %s\r\n", ua);
    اكسر;
  }

  حالة H1_HD_ACCEPT:
    إذا(!Curl_checkheaders(معلومات, STRCONST("Accept")))
      نتيجو = curlx_dyn_جوڙيو(req, "Accept: */*\r\n");
    اكسر;

  حالة H1_HD_TE:
#جيڪڏهن_وصف_ٿيل HAVE_LIBZ
    إذا(!Curl_checkheaders(معلومات, STRCONST("TE")) &&
       معلومات->سيٽ.http_transfer_encoding) {
      معلومات->state.http_hd_te = TRUE;
      نتيجو = curlx_dyn_جوڙيو(req, "TE: gzip\r\n");
    }
#جيڪڏهن_پورو
    اكسر;

  حالة H1_HD_ACCEPT_ENCODING: {
    ثابت حرف *enc = CURL_EASY_STR(معلومات, STRING_ENCODING);
    إذا(enc && !Curl_checkheaders(معلومات, STRCONST("Accept-Encoding")))
      نتيجو = curlx_dyn_addf(req, "Accept-Encoding: %s\r\n", enc);
    اكسر;
  }

  حالة H1_HD_REFERER:
    إذا(Curl_bufref_ptr(&معلومات->state.referer) &&
       !Curl_checkheaders(معلومات, STRCONST("Referer")))
      نتيجو = curlx_dyn_addf(req, "Referer: %s\r\n",
                              Curl_bufref_ptr(&معلومات->state.referer));
    اكسر;

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  حالة H1_HD_PROXY_CONNECTION:
    إذا(conn->بٽ.origin_is_proxy &&
       !Curl_checkheaders(معلومات, STRCONST("Proxy-Connection")) &&
       !Curl_checkProxyheaders(معلومات, معلومات->conn, STRCONST("Proxy-Connection")))
      نتيجو = curlx_dyn_جوڙيو(req, "Proxy-Connection: Keep-Alive\r\n");
    اكسر;
#جيڪڏهن_پورو

  حالة H1_HD_TRANSFER_ENCODING:
    نتيجو = http_req_سيٽ_TE(معلومات, req, httpversion);
    اكسر;

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_ALTSVC
  حالة H1_HD_ALT_استعمال_ٿيل:
    إذا(conn->بٽ.altused && conn->ذريعي_peer &&
       !Curl_checkheaders(معلومات, STRCONST("Alt-Used")))
      نتيجو = curlx_dyn_addf(req, "Alt-Used: %s:%u\r\n",
                              conn->ذريعي_peer->hostname, conn->ذريعي_peer->پورٽ);
    اكسر;
#جيڪڏهن_پورو

  حالة H1_HD_UPGRADE:
    إذا(!Curl_conn_is_ssl(معلومات->conn, FIRSTSOCKET) && (httpversion < 20) &&
       (معلومات->state.http_neg.wanted & CURL_HTTP_V2x) &&
       معلومات->state.http_neg.h2_upgrade) {
      /* append HTTP2 upgrade magic stuff to the HTTP request if it is not done
         over SSL */
      نتيجو = Curl_http2_request_upgrade(req, معلومات);
    }
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_WEBSOCKETS
    إذا(!نتيجو && conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
      نتيجو = Curl_ws_request(معلومات, req);
#جيڪڏهن_پورو
    اكسر;

  حالة H1_HD_COOKIES:
    نتيجو = http_cookies(معلومات, req);
    اكسر;

  حالة H1_HD_CONDITIONALS:
    نتيجو = Curl_جوڙيو_timecondition(معلومات, req);
    اكسر;

  حالة H1_HD_CUSTOM:
    نتيجو = Curl_جوڙيو_custom_سرا(معلومات, FALSE, httpversion, req);
    اكسر;

  حالة H1_HD_CONTENT:
    نتيجو = http_جوڙيو_content_hds(معلومات, req, httpversion, httpreq);
    اكسر;

  حالة H1_HD_CONNECTION: {
    نتيجو = http_جوڙيو_connection_hd(معلومات, req);
    اكسر;
  }

  حالة H1_HD_آخري:
    نتيجو = curlx_dyn_addn(req, STRCONST("\r\n"));
    اكسر;
  }
  أرجع نتيجو;
}

/*
 * Curl_http() gets called from the generic multi_do() function when an HTTP
 * request is to be performed. This creates and sends a properly constructed
 * HTTP request.
 */
CURLcode Curl_http(بنية Curl_easy *معلومات, bool *پورو)
{
  CURLcode نتيجو = CURLE_OK;
  Curl_HttpReq httpreq;
  ثابت حرف *طريقو;
  بنية dynbuf req;
  بنانشان حرف httpversion;
  size_t hd_id;

  /* Always consider the DO phase done after this function call, even if there
     may be parts of the request that are not yet sent, since we can deal with
     the rest of the request in the PERFORM phase. */
  *پورو = TRUE;
  /* initialize a dynamic send-buffer */
  curlx_dyn_init(&req, DYN_HTTP_REQUEST);
  /* make sure the header buffer is reset - if there are leftovers from a
     previous transfer */
  curlx_dyn_reset(&معلومات->state.headerb);
  معلومات->state.maybe_folded = FALSE;

  إذا(!معلومات->conn->بٽ.reuse) {
    نتيجو = http_check_new_conn(معلومات);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }

  /* Add collecting of headers written to client. For a new connection,
   * we might have done that already, but reuse
   * or multiplex needs it here as well. */
  نتيجو = Curl_سرا_init(معلومات);
  إذا(نتيجو)
    اذهب ٻاهريون;

  معلومات->state.http_hd_te = FALSE;
  معلومات->state.http_hd_upgrade = FALSE;
  معلومات->state.http_hd_h2_settings = FALSE;

  /* what kind of request do we need to send? */
  Curl_http_طريقو(معلومات, &طريقو, &httpreq);

  /* select host to send */
  نتيجو = http_سيٽ_aptr_host(معلومات);
  /* setup the authentication headers, how that method and host are known */
  إذا(!نتيجو)
    نتيجو = Curl_http_output_auth(معلومات, معلومات->conn, طريقو, httpreq,
                                   معلومات->state.مٿي.رستو,
                                   معلومات->state.مٿي.query, FALSE);
  /* Setup input reader, resume information and ranges */
  إذا(!نتيجو)
    نتيجو = سيٽ_reader(معلومات, httpreq);
  إذا(!نتيجو)
    نتيجو = http_resume(معلومات, httpreq);
  إذا(!نتيجو)
    نتيجو = http_range(معلومات, httpreq);
  إذا(نتيجو)
    اذهب ٻاهريون;

  httpversion = http_request_نسخو(معلومات);
  /* Add request line and all headers to `req` */
  لكل(hd_id = 0; hd_id <= H1_HD_آخري; ++hd_id) {
    نتيجو = http_جوڙيو_hd(معلومات, &req, (http_hd_t)hd_id,
                         httpversion, طريقو, httpreq);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }

  /* setup variables for the upcoming transfer and send */
  Curl_xfer_تياري_sendrecv(معلومات, FIRSTSOCKET, -1);
  نتيجو = Curl_req_send(معلومات, &req, httpversion);

  إذا((httpversion >= 20) && معلومات->req.upload_chunky)
    /* upload_chunky was set above to set up the request in a chunky fashion,
       but is disabled here again to avoid that the chunked encoded version is
       actually used when sending the request body over h2 */
    معلومات->req.upload_chunky = FALSE;

ٻاهريون:
  إذا(نتيجو == CURLE_TOO_LARGE)
    failf(معلومات, "HTTP request too large");

  curlx_dyn_free(&req);
  أرجع نتيجو;
}

تعريفنوع تعداد {
  حالت_UNKNOWN, /* not enough data to tell yet */
  حالت_پورو, /* a status line was read */
  حالت_BAD /* not a status line */
} statusline;

/* Check a string for a prefix. Check no more than 'len' bytes */
ساكن bool checkprefixmax(ثابت حرف *اڳياڙي, ثابت حرف *بفر, size_t len)
{
  size_t ch = CURLMIN(لڙي_ڊيگهه(اڳياڙي), len);
  أرجع curl_strnequal(اڳياڙي, بفر, ch);
}

/*
 * checkhttpprefix()
 *
 * Returns TRUE if member of the list matches prefix of string
 */
ساكن statusline checkhttpprefix(بنية Curl_easy *معلومات,
                                  ثابت حرف *s, size_t len)
{
  بنية curl_slist *سر = معلومات->سيٽ.http200aliases;
  statusline rc = حالت_BAD;
  statusline onmatch = len >= 5 ? حالت_پورو : حالت_UNKNOWN;

  طالما(سر) {
    إذا(checkprefixmax(سر->معلومات, s, len)) {
      rc = onmatch;
      اكسر;
    }
    سر = سر->ايندڙ;
  }

  إذا((rc != حالت_پورو) && checkprefixmax("HTTP/", s, len))
    rc = onmatch;

  أرجع rc;
}

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_RTSP
ساكن statusline checkrtspprefix(بنية Curl_easy *معلومات,
                                  ثابت حرف *s, size_t len)
{
  statusline حالت = حالت_BAD;
  statusline onmatch = len >= 5 ? حالت_پورو : حالت_UNKNOWN;
  (فراغ)معلومات;
  إذا(checkprefixmax("RTSP/", s, len))
    حالت = onmatch;

  أرجع حالت;
}
#جيڪڏهن_پورو /* CURL_DISABLE_RTSP */

ساكن statusline checkprotoprefix(بنية Curl_easy *معلومات,
                                   بنية connectdata *conn,
                                   ثابت حرف *s, size_t len)
{
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_RTSP
  إذا(conn->scheme->protocol & CURLPROTO_RTSP)
    أرجع checkrtspprefix(معلومات, s, len);
#وإلا
  (فراغ)conn;
#جيڪڏهن_پورو /* CURL_DISABLE_RTSP */

  أرجع checkhttpprefix(معلومات, s, len);
}

/* HTTP header has field name `n` (a string constant) */
#وصف_ڪريو HD_IS(hd, hdlen, n) \
  (((hdlen) >= (حجم(n) - 1)) && curl_strnequal(n, hd, حجم(n) - 1))

#وصف_ڪريو HD_VAL(hd, hdlen, n) \
  ((((hdlen) >= (حجم(n) - 1)) && (hd) && \
    curl_strnequal(n, hd, حجم(n) - 1)) ? ((hd) + (حجم(n) - 1)) : عدم)

/* HTTP header has field name `n` (a string constant) and contains `v`
 * (a string constant) in its value(s) */
#وصف_ڪريو HD_IS_AND_SAYS(hd, hdlen, n, v) \
  (HD_IS(hd, hdlen, n) && \
   ((hdlen) > ((حجم(n) - 1) + (حجم(v) - 1))) && \
   Curl_compareheader(hd, STRCONST(n), STRCONST(v)))

/*
 * http_header_a() parses a single response header starting with A.
 */
ساكن CURLcode http_سرو_a(بنية Curl_easy *معلومات,
                              ثابت حرف *hd, size_t hdlen)
{
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_ALTSVC
  ثابت حرف *v;
  v = (معلومات->asi &&
       (Curl_xfer_is_secure(معلومات) ||
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_ALTSVC_HTTP")
#وإلا
        0
#جيڪڏهن_پورو
         )) ? HD_VAL(hd, hdlen, "Alt-Svc:") : عدم;
  إذا(v) {
    /* the ALPN of the current request */
    بنية SingleRequest *k = &معلومات->req;
    تعداد alpnid id = (k->httpversion == 30) ? ALPN_h3 :
      (k->httpversion == 20) ? ALPN_h2 : ALPN_h1;
    أرجع Curl_altsvc_parse(معلومات, معلومات->asi, v, معلومات->state.origin, id);
  }
#وإلا
  (فراغ)معلومات;
  (فراغ)hd;
  (فراغ)hdlen;
#جيڪڏهن_پورو
  أرجع CURLE_OK;
}

/*
 * http_header_c() parses a single response header starting with C.
 */
ساكن CURLcode http_سرو_c(بنية Curl_easy *معلومات,
                              ثابت حرف *hd, size_t hdlen)
{
  بنية connectdata *conn = معلومات->conn;
  بنية SingleRequest *k = &معلومات->req;
  ثابت حرف *v;

  /* Check for Content-Length: header lines to get size. Browsers insist we
     should accept multiple Content-Length headers and that a comma separated
     list also is fine and then we should accept them all as long as they are
     the same value. Different values trigger error.
   */
  v = (!k->http_bodyless && !معلومات->سيٽ.ignorecl && !k->ignore_cl) ?
    HD_VAL(hd, hdlen, "Content-Length:") : عدم;
  إذا(v) {
    افعل {
      curl_off_t contentlength;
      صحيح offt = curlx_str_numblanks(&v, &contentlength);

      إذا(offt == STRE_حد_کان_مٿي) {
        /* out of range */
        إذا(معلومات->سيٽ.max_filesize) {
          failf(معلومات, "Maximum file size exceeded");
          أرجع CURLE_FILESIZE_EXCEEDED;
        }
        streamclose(conn);
        infof(معلومات, "Overflow Content-Length: value");
        أرجع CURLE_OK;
      }
      وإلا {
        إذا((offt == STRE_OK) &&
           ((k->سائيز == -1) || /* not set to something before */
            (k->سائيز == contentlength))) { /* or the same value */

          k->سائيز = contentlength;
          curlx_str_passblanks(&v);

          /* on a comma, loop and get the next instead */
          إذا(!curlx_str_single(&v, ','))
            استمر;

          إذا(!curlx_str_newline(&v)) {
            k->maxdownload = k->سائيز;
            أرجع CURLE_OK;
          }
        }
        /* negative, different value or rubbish - bad HTTP */
        failf(معلومات, "Invalid Content-Length: value");
        أرجع CURLE_WEIRD_SERVER_جواب;
      }
    } طالما(1);
  }
  v = (!k->http_bodyless && CURL_EASY_STR(معلومات, STRING_ENCODING)) ?
    HD_VAL(hd, hdlen, "Content-Encoding:") : عدم;
  إذا(v) {
    /*
     * Process Content-Encoding. Look for the values: identity, gzip, deflate,
     * compress, x-gzip and x-compress. x-gzip and x-compress are the same as
     * gzip and compress. (Sec 3.5 RFC 2616). zlib cannot handle compress.
     * Errors are handled further down when the response body is processed
     */
    أرجع Curl_build_unencoding_ڍير(معلومات, v, FALSE);
  }
  /* check for Content-Type: header lines to get the MIME-type */
  v = HD_VAL(hd, hdlen, "Content-Type:");
  إذا(v) {
    حرف *contenttype = Curl_copy_سرو_قدر(hd);
    إذا(!contenttype)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    إذا(!*contenttype)
      /* ignore empty data */
      curlx_free(contenttype);
    وإلا {
      curlx_free(معلومات->info.contenttype);
      معلومات->info.contenttype = contenttype;
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
    infof(معلومات, "HTTP/1.0 connection set to keep alive");
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
      إذا(!curlx_str_عدد(&ptr, &k->وٿي, CURL_OFF_T_MAX) &&
         (معلومات->state.resume_from == k->وٿي))
        /* we asked for a resume and we got it */
        k->content_range = TRUE;
    }
    وإلا إذا(k->httpcode < 300)
      معلومات->state.resume_from = 0; /* get everything */
  }
  أرجع CURLE_OK;
}

/*
 * http_header_l() parses a single response header starting with L.
 */
ساكن CURLcode http_سرو_l(بنية Curl_easy *معلومات,
                              ثابت حرف *hd, size_t hdlen)
{
  بنية connectdata *conn = معلومات->conn;
  بنية SingleRequest *k = &معلومات->req;
  ثابت حرف *v = (!k->http_bodyless &&
                   (معلومات->سيٽ.timecondition || معلومات->سيٽ.get_filetime)) ?
    HD_VAL(hd, hdlen, "Last-Modified:") : عدم;
  إذا(v) {
    إذا(Curl_getdate_capped(v, &k->timeofdoc))
      k->timeofdoc = 0;
    إذا(معلومات->سيٽ.get_filetime)
      معلومات->info.filetime = k->timeofdoc;
    أرجع CURLE_OK;
  }
  إذا(HD_IS(hd, hdlen, "Location:")) {
    /* this is the URL that the server advises us to use instead */
    حرف *location = Curl_copy_سرو_قدر(hd);
    إذا(!location)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    إذا(!*location ||
       (معلومات->req.location && !strcmp(معلومات->req.location, location))) {
      /* ignore empty header, or exact repeat of a previous one */
      curlx_free(location);
      أرجع CURLE_OK;
    }
    وإلا {
      /* has value and is not an exact repeat */
      إذا(معلومات->req.location) {
        failf(معلومات, "Multiple Location headers");
        curlx_free(location);
        أرجع CURLE_WEIRD_SERVER_جواب;
      }
      معلومات->req.location = location;

      إذا((k->httpcode >= 300 && k->httpcode < 400) &&
         معلومات->سيٽ.http_follow_گهڻائي) {
        CURLcode نتيجو;
        DEBUGASSERT(!معلومات->req.newurl);
        معلومات->req.newurl = curlx_strdup(معلومات->req.location); /* clone */
        إذا(!معلومات->req.newurl)
          أرجع CURLE_ٻاهريون_جو_يادگيري;

        /* some cases of POST and PUT etc needs to rewind the data
           stream at this point */
        نتيجو = http_perhapsrewind(معلومات, conn);
        إذا(نتيجو)
          أرجع نتيجو;

        /* mark the next request as a followed location: */
        معلومات->state.this_is_a_follow = TRUE;
      }
    }
  }
  أرجع CURLE_OK;
}

/*
 * http_header_p() parses a single response header starting with P.
 */
ساكن CURLcode http_سرو_p(بنية Curl_easy *معلومات,
                              ثابت حرف *hd, size_t hdlen)
{
  بنية SingleRequest *k = &معلومات->req;

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  ثابت حرف *v = HD_VAL(hd, hdlen, "Proxy-Connection:");
  إذا(v) {
    بنية connectdata *conn = معلومات->conn;
    إذا((k->httpversion == 10) && conn->http_proxy.peer &&
       HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "keep-alive")) {
      /*
       * When an HTTP/1.0 reply comes when using a proxy, the
       * 'Proxy-Connection: keep-alive' line tells us the
       * connection will be kept alive for our pleasure.
       * Default action for 1.0 is to close.
       */
      connkeep(conn); /* do not close */
      infof(معلومات, "HTTP/1.0 proxy connection set to keep alive");
    }
    وإلا إذا((k->httpversion == 11) && conn->http_proxy.peer &&
            HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "close")) {
      /*
       * We get an HTTP/1.1 response from a proxy and it says it will
       * close down after this transfer.
       */
      connclose(conn);
      infof(معلومات, "HTTP/1.1 proxy connection set close");
    }
    أرجع CURLE_OK;
  }
#جيڪڏهن_پورو
  إذا((407 == k->httpcode) && HD_IS(hd, hdlen, "Proxy-authenticate:")) {
    حرف *auth;
    CURLcode نتيجو;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
    إذا(!معلومات->conn->بٽ.origin_is_proxy)
      /* this response is not from a forward proxy, not for us */
      أرجع CURLE_OK;
#جيڪڏهن_پورو
    auth = Curl_copy_سرو_قدر(hd);
    نتيجو = auth ? CURLE_OK : CURLE_ٻاهريون_جو_يادگيري;
    إذا(!نتيجو) {
      نتيجو = Curl_http_داخلا_auth(معلومات, TRUE, auth);
      curlx_free(auth);
    }
    أرجع نتيجو;
  }
#جيڪڏهن_وصف_ٿيل USE_SPNEGO
  إذا(HD_IS(hd, hdlen, "Persistent-Auth:")) {
    بنية connectdata *conn = معلومات->conn;
    بنية negotiatedata *negdata = Curl_auth_nego_get(conn, FALSE);
    بنية auth *authp = &معلومات->state.authhost;
    إذا(!negdata)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    إذا(authp->picked == CURLAUTH_NEGOTIATE) {
      حرف *persistentauth = Curl_copy_سرو_قدر(hd);
      إذا(!persistentauth)
        أرجع CURLE_ٻاهريون_جو_يادگيري;
      negdata->noauthpersist = !!checkprefix("false", persistentauth);
      negdata->havenoauthpersist = TRUE;
      infof(معلومات, "Negotiate: noauthpersist -> %d, header part: %s",
            negdata->noauthpersist, persistentauth);
      curlx_free(persistentauth);
    }
  }
#جيڪڏهن_پورو
  أرجع CURLE_OK;
}

/*
 * http_header_r() parses a single response header starting with R.
 */
ساكن CURLcode http_سرو_r(بنية Curl_easy *معلومات,
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
      time_t current = وقت(عدم);
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
    معلومات->info.retry_after = retry_after;
  }
  أرجع CURLE_OK;
}

/*
 * http_header_s() parses a single response header starting with S.
 */
ساكن CURLcode http_سرو_s(بنية Curl_easy *معلومات,
                              ثابت حرف *hd, size_t hdlen)
{
#إذا !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_HSTS)
  ثابت حرف *v;
#وإلا
  (فراغ)معلومات;
  (فراغ)hd;
  (فراغ)hdlen;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_COOKIES
  v = (معلومات->cookies && معلومات->state.cookie_engine) ?
    HD_VAL(hd, hdlen, "Set-Cookie:") : عدم;
  إذا(v) {
    /* If there is a custom-set Host: name, use it here, or else use
     * real peer hostname. */
    ثابت حرف *host = معلومات->req.cookiehost ?
      معلومات->req.cookiehost : معلومات->state.origin->hostname;
    ثابت بنانشان حرف secure_context = Curl_secure_context(معلومات, host) ?
      COOKIE_SECURE : 0;
    CURLcode نتيجو;
    Curl_share_تالو(معلومات, CURL_تالو_معلومات_COOKIE, CURL_تالو_ACCESS_SINGLE);
    نتيجو = Curl_cookie_جوڙيو(معلومات, معلومات->cookies, v, host,
                             معلومات->state.مٿي.رستو,
                             COOKIE_HTTPHEADER | secure_context);
    Curl_share_unlock(معلومات, CURL_تالو_معلومات_COOKIE);
    أرجع نتيجو;
  }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HSTS
  /* If enabled, the header is incoming and this is over HTTPS */
  v = (معلومات->hsts &&
       (Curl_xfer_is_secure(معلومات) ||
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_HSTS_HTTP")
#وإلا
        0
#جيڪڏهن_پورو
         )
    ) ? HD_VAL(hd, hdlen, "Strict-Transport-Security:") : عدم;
  إذا(v) {
    CURLcode نتيجو = Curl_hsts_parse(
      معلومات->hsts, معلومات->state.origin->hostname, v);
    إذا(نتيجو) {
      إذا(نتيجو == CURLE_ٻاهريون_جو_يادگيري)
        أرجع نتيجو;
      infof(معلومات, "Illegal STS header skipped");
    }
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
    وإلا
      infof(معلومات, "Parsed STS header fine (%zu entries)",
            Curl_llist_ڳڻپ(&معلومات->hsts->list));
#جيڪڏهن_پورو
  }
#جيڪڏهن_پورو

  أرجع CURLE_OK;
}

/*
 * http_header_t() parses a single response header starting with T.
 */
ساكن CURLcode http_سرو_t(بنية Curl_easy *معلومات,
                              ثابت حرف *hd, size_t hdlen)
{
  بنية connectdata *conn = معلومات->conn;
  بنية SingleRequest *k = &معلومات->req;

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
                   (معلومات->state.httpreq != HTTPREQ_سر) &&
                   (k->httpcode != 304)) ?
    HD_VAL(hd, hdlen, "Transfer-Encoding:") : عدم;
  إذا(v) {
    /* One or more encodings. We check for chunked and/or a compression
       algorithm. */
    CURLcode نتيجو = Curl_build_unencoding_ڍير(معلومات, v, TRUE);
    إذا(نتيجو)
      أرجع نتيجو;
    إذا(!k->chunk) {
      /* if this is not chunked, only close can signal the end of this
       * transfer as Content-Length is said not to be trusted for
       * transfer-encoding! */
      CURL_TRC_M(معلومات, "HTTP/1.1 transfer-encoding without chunks");
      connclose(conn);
      k->ignore_cl = TRUE;
    }
    أرجع CURLE_OK;
  }
  v = HD_VAL(hd, hdlen, "Trailer:");
  إذا(v) {
    معلومات->req.resp_trailer = TRUE;
    أرجع CURLE_OK;
  }
  أرجع CURLE_OK;
}

/*
 * http_header_w() parses a single response header starting with W.
 */
ساكن CURLcode http_سرو_w(بنية Curl_easy *معلومات,
                              ثابت حرف *hd, size_t hdlen)
{
  بنية SingleRequest *k = &معلومات->req;
  CURLcode نتيجو = CURLE_OK;

  إذا((401 == k->httpcode) && HD_IS(hd, hdlen, "WWW-Authenticate:")) {
    حرف *auth = Curl_copy_سرو_قدر(hd);
    إذا(!auth)
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    وإلا {
      نتيجو = Curl_http_داخلا_auth(معلومات, FALSE, auth);
      curlx_free(auth);
    }
  }
  أرجع نتيجو;
}

/*
 * http_header() parses a single response header.
 */
ساكن CURLcode http_سرو(بنية Curl_easy *معلومات,
                            ثابت حرف *hd, size_t hdlen)
{
  CURLcode نتيجو = CURLE_OK;

  بدل(hd[0]) {
  حالة 'a':
  حالة 'A':
    نتيجو = http_سرو_a(معلومات, hd, hdlen);
    اكسر;
  حالة 'c':
  حالة 'C':
    نتيجو = http_سرو_c(معلومات, hd, hdlen);
    اكسر;
  حالة 'l':
  حالة 'L':
    نتيجو = http_سرو_l(معلومات, hd, hdlen);
    اكسر;
  حالة 'p':
  حالة 'P':
    نتيجو = http_سرو_p(معلومات, hd, hdlen);
    اكسر;
  حالة 'r':
  حالة 'R':
    نتيجو = http_سرو_r(معلومات, hd, hdlen);
    اكسر;
  حالة 's':
  حالة 'S':
    نتيجو = http_سرو_s(معلومات, hd, hdlen);
    اكسر;
  حالة 't':
  حالة 'T':
    نتيجو = http_سرو_t(معلومات, hd, hdlen);
    اكسر;
  حالة 'w':
  حالة 'W':
    نتيجو = http_سرو_w(معلومات, hd, hdlen);
    اكسر;
  }

  إذا(!نتيجو) {
    بنية connectdata *conn = معلومات->conn;
    إذا(conn->scheme->protocol & CURLPROTO_RTSP)
      نتيجو = Curl_rtsp_parseheader(معلومات, hd);
  }
  أرجع نتيجو;
}

/*
 * Called after the first HTTP response line (the status line) has been
 * received and parsed.
 */
ساكن CURLcode http_statusline(بنية Curl_easy *معلومات,
                                بنية connectdata *conn)
{
  بنية SingleRequest *k = &معلومات->req;

  بدل(k->httpversion) {
  حالة 10:
  حالة 11:
#جيڪڏهن_وصف_ٿيل USE_HTTP2
  حالة 20:
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_HTTP3
  حالة 30:
#جيڪڏهن_پورو
    /* no major version switch mid-connection */
    إذا(k->httpversion_موڪليل &&
       (k->httpversion / 10 != k->httpversion_موڪليل / 10)) {
      failf(معلومات, "Version mismatch (from HTTP/%d to HTTP/%d)",
            k->httpversion_موڪليل / 10, k->httpversion / 10);
      أرجع CURLE_WEIRD_SERVER_جواب;
    }
    اكسر;
  افتراضي:
    failf(معلومات, "Unsupported HTTP version (%d.%d) in response",
          k->httpversion / 10, k->httpversion % 10);
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  }

  معلومات->info.httpcode = k->httpcode;
  معلومات->info.httpversion = k->httpversion;
  conn->httpversion_ڏٺل = k->httpversion;

  إذا(!معلومات->state.http_neg.rcvd_min ||
     معلومات->state.http_neg.rcvd_min > k->httpversion)
    /* store the lowest server version we encounter */
    معلومات->state.http_neg.rcvd_min = k->httpversion;

  /*
   * This code executes as part of processing the header. As a
   * result, it is not totally clear how to interpret the
   * response code yet as that depends on what other headers may
   * be present. 401 and 407 may be errors, but may be OK
   * depending on how authentication is working. Other codes
   * are definitely errors, so give up here.
   */
  إذا(معلومات->state.resume_from && معلومات->state.httpreq == HTTPREQ_GET &&
     k->httpcode == 416) {
    /* "Requested Range Not Satisfiable", proceed and pretend this is no
       error */
    k->ignorebody = TRUE; /* Avoid appending error msg to good data. */
  }

  إذا(k->httpversion == 10) {
    /* Default action for HTTP/1.0 must be to close, unless
       we get one of those fancy headers that tell us the
       server keeps it open for us! */
    infof(معلومات, "HTTP 1.0, assume close after body");
    connclose(conn);
  }

  k->http_bodyless = k->httpcode >= 100 && k->httpcode < 200;
  بدل(k->httpcode) {
  حالة 304:
    /* (quote from RFC2616, section 10.3.5): The 304 response
     * MUST NOT contain a message-body, and thus is always
     * terminated by the first empty line after the header
     * fields. */
    إذا(معلومات->سيٽ.timecondition)
      معلومات->info.timecond = TRUE;
    FALLTHROUGH();
  حالة 204:
    /* (quote from RFC2616, section 10.2.5): The server has
     * fulfilled the request but does not need to return an
     * entity-body ... The 204 response MUST NOT include a
     * message-body, and thus is always terminated by the first
     * empty line after the header fields. */
    k->سائيز = 0;
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
ساكن CURLcode http_سائيز(بنية Curl_easy *معلومات)
{
  بنية SingleRequest *k = &معلومات->req;
  إذا(معلومات->req.ignore_cl || k->chunk) {
    k->سائيز = k->maxdownload = -1;
  }
  وإلا إذا(k->سائيز != -1) {
    إذا(معلومات->سيٽ.max_filesize &&
       !k->ignorebody &&
       (k->سائيز > معلومات->سيٽ.max_filesize)) {
      failf(معلومات, "Maximum file size exceeded");
      أرجع CURLE_FILESIZE_EXCEEDED;
    }
    إذا(k->ignorebody)
      infof(معلومات, "setting size while ignoring");
    Curl_pgrs_سيٽ_Download_سائيز(معلومات, k->سائيز);
    k->maxdownload = k->سائيز;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_verify_سرو(بنية Curl_easy *معلومات,
                            ثابت حرف *hd, size_t hdlen)
{
  بنية SingleRequest *k = &معلومات->req;
  size_t i;
  bool has_colon = FALSE;
  لكل(i = 0; i < hdlen; i++) {
    بدل(hd[i]) {
    حالة '\0':
      /* this is bad, bail out */
      failf(معلومات, "NUL byte in header");
      أرجع CURLE_WEIRD_SERVER_جواب;
    حالة '\r':
      إذا(i < hdlen - 2) {
        /* CR may only precede the LF, nothing else */
        failf(معلومات, "Carriage return found in header");
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
    failf(معلومات, "Header without colon");
    أرجع CURLE_WEIRD_SERVER_جواب;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_bump_headersize(بنية Curl_easy *معلومات,
                              size_t delta,
                              bool connect_only)
{
  size_t bad = 0;
  بنانشان صحيح max = MAX_HTTP_RESP_سرو_سائيز;
  إذا(delta < MAX_HTTP_RESP_سرو_سائيز) {
    معلومات->info.سرو_سائيز += (بنانشان صحيح)delta;
    معلومات->req.allheadercount += (بنانشان صحيح)delta;
    إذا(!connect_only)
      معلومات->req.headerbytecount += (بنانشان صحيح)delta;
    إذا(معلومات->req.allheadercount > max)
      bad = معلومات->req.allheadercount;
    وإلا إذا(معلومات->info.سرو_سائيز > (max * 20)) {
      bad = معلومات->info.سرو_سائيز;
      max *= 20;
    }
  }
  وإلا
    bad = معلومات->req.allheadercount + delta;
  إذا(bad) {
    failf(معلومات, "Too large response headers: %zu > %u", bad, max);
    أرجع CURLE_RECV_ERROR;
  }
  أرجع CURLE_OK;
}

/*
 * Handle a 101 Switching Protocols response. Performs the actual protocol
 * upgrade to HTTP/2 or WebSocket based on what was requested.
 */
ساكن CURLcode http_on_101_upgrade(بنية Curl_easy *معلومات,
                                    ثابت حرف *buf, size_t blen,
                                    size_t *pconsumed,
                                    bool *conn_بدليو)
{
  بنية connectdata *conn = معلومات->conn;
  بنية SingleRequest *k = &معلومات->req;

#إذا !defined(USE_NGHTTP2) && defined(CURL_DISABLE_WEBSOCKETS)
  (فراغ)buf;
  (فراغ)blen;
  (فراغ)pconsumed;
#وإلا
  CURLcode نتيجو;
  صحيح upgr101_requested = k->upgr101;
#جيڪڏهن_پورو

  إذا(k->httpversion_موڪليل != 11) {
    /* invalid for other HTTP versions */
    failf(معلومات, "server sent 101 response while not talking HTTP/1.1");
    أرجع CURLE_WEIRD_SERVER_جواب;
  }

  /* Whatever the success, upgrade was selected. */
  k->upgr101 = UPGR101_RECEIVED;
  conn->بٽ.upgrade_اندريون_ترقي = FALSE;
  *conn_بدليو = TRUE;

  /* To be fully compliant, we would check the "Upgrade:" response header to
   * mention the protocol we requested. */
#جيڪڏهن_وصف_ٿيل USE_NGHTTP2
  إذا(upgr101_requested == UPGR101_H2) {
    /* Switch to HTTP/2, where we will get more responses. blen bytes in buf
     * are already h2 protocol bytes */
    infof(معلومات, "Received 101, Switching to HTTP/2");
    نتيجو = Curl_http2_upgrade(معلومات, conn, FIRSTSOCKET, buf, blen);
    إذا(!نتيجو)
      *pconsumed += blen;
    أرجع نتيجو;
  }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_WEBSOCKETS
  إذا(upgr101_requested == UPGR101_WS) {
    /* Switch to WebSocket, where we now stream ws frames. blen bytes in buf
     * are already ws protocol bytes */
    infof(معلومات, "Received 101, Switching to WebSocket");
    نتيجو = Curl_ws_accept(معلومات, buf, blen);
    إذا(!نتيجو)
      *pconsumed += blen; /* ws accept handled the data */
    أرجع نتيجو;
  }
#جيڪڏهن_پورو
  /* We silently accept this as the final response. What are we switching to
   * if we did not ask for an Upgrade? Maybe the application provided an
   * `Upgrade: xxx` header? */
  k->سرو = FALSE;
  أرجع CURLE_OK;
}

/*
 * Handle 1xx intermediate HTTP responses. Sets up state for more
 * headers and processes 100-continue and 101 upgrade responses.
 */
ساكن CURLcode http_on_1xx_response(بنية Curl_easy *معلومات,
                                     ثابت حرف *buf, size_t blen,
                                     size_t *pconsumed,
                                     bool *conn_بدليو)
{
  بنية SingleRequest *k = &معلومات->req;

  /* "A user agent MAY ignore unexpected 1xx status responses."
   * By default, we expect to get more responses after this one. */
  k->سرو = TRUE;
  k->headerline = 0; /* restart the header line counter */

  بدل(k->httpcode) {
  حالة 100:
    /* We have made an HTTP PUT or POST and this is 1.1-lingo that tells us
     * that the server is OK with this and ready to receive the data. */
    http_exp100_got100(معلومات);
    اكسر;
  حالة 101:
    أرجع http_on_101_upgrade(معلومات, buf, blen, pconsumed, conn_بدليو);
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
ساكن فراغ http_check_auth_گهيرو(بنية Curl_easy *معلومات,
                                    بنية connectdata *conn)
{
  /* At this point we have some idea about the fate of the connection. If we
     are closing the connection it may result auth failure. */
#جيڪڏهن_وصف_ٿيل USE_NTLM
  إذا(conn->بٽ.close &&
     (((معلومات->req.httpcode == 401) &&
       (conn->http_ntlm_state == NTLMSTATE_TYPE2)) ||
      ((معلومات->req.httpcode == 407) &&
       (conn->proxy_ntlm_state == NTLMSTATE_TYPE2)))) {
    infof(معلومات, "Connection closure while negotiating auth (HTTP 1.0?)");
    معلومات->state.authproblem = TRUE;
  }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_SPNEGO
  إذا(conn->بٽ.close &&
    (((معلومات->req.httpcode == 401) &&
      (conn->http_negotiate_state == CURL_NEGO_AUTH_RECV)) ||
     ((معلومات->req.httpcode == 407) &&
      (conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)))) {
    infof(معلومات, "Connection closure while negotiating auth (HTTP 1.0?)");
    معلومات->state.authproblem = TRUE;
  }
  إذا((conn->http_negotiate_state == CURL_NEGO_AUTH_پورو) &&
     (معلومات->req.httpcode != 401)) {
    conn->http_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
  إذا((conn->proxy_negotiate_state == CURL_NEGO_AUTH_پورو) &&
     (معلومات->req.httpcode != 407)) {
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
#جيڪڏهن_پورو
}
#وإلا
#وصف_ڪريو http_check_auth_گهيرو(x, y) /*خالي_جايون*/
#جيڪڏهن_پورو

/*
 * Handle an error response (>= 300) received while still sending the
 * request body. Deals with 417 Expectation Failed retries, keep-sending
 * on error, and aborting the send.
 */
ساكن CURLcode http_handle_send_error(بنية Curl_easy *معلومات)
{
  بنية connectdata *conn = معلومات->conn;
  بنية SingleRequest *k = &معلومات->req;
  CURLcode نتيجو = CURLE_OK;

  إذا(!معلومات->req.authneg && !conn->بٽ.close &&
     !Curl_creader_will_rewind(معلومات)) {
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

    بدل(معلومات->state.httpreq) {
    حالة HTTPREQ_PUT:
    حالة HTTPREQ_POST:
    حالة HTTPREQ_POST_FORM:
    حالة HTTPREQ_POST_MIME:
      /* We got an error response. If this happened before the
       * whole request body has been sent we stop sending and
       * mark the connection for closure after we have read the
       * entire response. */
      إذا(!Curl_req_پورو_sending(معلومات)) {
        إذا((k->httpcode == 417) && http_exp100_is_selected(معلومات)) {
          /* 417 Expectation Failed - try again without the
             Expect header */
          إذا(!k->writebytecount && http_exp100_is_انتظار(معلومات)) {
            infof(معلومات, "Got HTTP failure 417 while waiting for a 100");
          }
          وإلا {
            infof(معلومات, "Got HTTP failure 417 while sending data");
            streamclose(conn);
            نتيجو = http_perhapsrewind(معلومات, conn);
            إذا(نتيجو)
              أرجع نتيجو;
          }
          معلومات->state.disableexpect = TRUE;
          Curl_req_abort_sending(معلومات);
          DEBUGASSERT(!معلومات->req.newurl);
          معلومات->req.newurl = Curl_bufref_dup(&معلومات->state.url);
          إذا(!معلومات->req.newurl)
            أرجع CURLE_ٻاهريون_جو_يادگيري;
        }
        وإلا إذا(معلومات->سيٽ.http_keep_sending_on_error) {
          infof(معلومات, "HTTP error before end of send, keep sending");
          http_exp100_send_anyway(معلومات);
        }
        وإلا {
          infof(معلومات, "HTTP error before end of send, stop sending");
          streamclose(conn);
          نتيجو = Curl_req_abort_sending(معلومات);
          إذا(نتيجو)
            أرجع نتيجو;
        }
      }
      اكسر;

    افتراضي: /* default label present to avoid compiler warnings */
      اكسر;
    }
  }

  إذا(Curl_creader_will_rewind(معلومات) && !Curl_req_پورو_sending(معلومات)) {
    /* We rewind before next send, continue sending now */
    infof(معلومات, "Keep sending data to get tossed away");
    CURL_REQ_سيٽ_SEND(معلومات);
  }
  أرجع نتيجو;
}

ساكن CURLcode http_on_response(بنية Curl_easy *معلومات,
                                 ثابت حرف *آخري_hd, size_t آخري_hd_len,
                                 ثابت حرف *buf, size_t blen,
                                 size_t *pconsumed)
{
  بنية connectdata *conn = معلومات->conn;
  CURLcode نتيجو = CURLE_OK;
  بنية SingleRequest *k = &معلومات->req;
  bool conn_بدليو = FALSE;

  (فراغ)buf; /* not used without HTTP2 enabled */
  *pconsumed = 0;

  إذا(k->upgr101 == UPGR101_RECEIVED) {
    /* supposedly upgraded to http2 now */
    إذا(معلومات->req.httpversion != 20)
      infof(معلومات, "Lying server, not serving HTTP/2");
  }

  إذا(k->httpcode < 200 && آخري_hd) {
    /* Intermediate responses might trigger processing of more responses,
     * write the last header to the client before proceeding. */
    نتيجو = http_write_سرو(معلومات, آخري_hd, آخري_hd_len);
    آخري_hd = عدم; /* handled it */
    إذا(نتيجو)
      اذهب ٻاهريون;
  }

  إذا(k->httpcode < 100) {
    failf(معلومات, "Unsupported response code in HTTP response");
    نتيجو = CURLE_UNSUPPORTED_PROTOCOL;
    اذهب ٻاهريون;
  }
  وإلا إذا(k->httpcode < 200) {
    نتيجو = http_on_1xx_response(معلومات, buf, blen, pconsumed, &conn_بدليو);
    اذهب ٻاهريون;
  }

  /* k->httpcode >= 200, final response */
  k->سرو = FALSE;
  إذا(conn->بٽ.upgrade_اندريون_ترقي) {
    /* Asked for protocol upgrade, but it was not selected */
    conn->بٽ.upgrade_اندريون_ترقي = FALSE;
    conn_بدليو = TRUE;
  }

  إذا((k->سائيز == -1) && !k->chunk && !conn->بٽ.close &&
     (k->httpversion == 11) &&
     !(conn->scheme->protocol & CURLPROTO_RTSP) &&
     معلومات->state.httpreq != HTTPREQ_سر) {
    /* On HTTP 1.1, when connection is not to get closed, but no
       Content-Length nor Transfer-Encoding chunked have been received,
       according to RFC2616 section 4.4 point 5, we assume that the server
       will close the connection to signal the end of the document. */
    infof(معلومات, "no chunk, no close, no size. Assume close to signal end");
    streamclose(conn);
  }

  http_check_auth_گهيرو(معلومات, conn);

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_WEBSOCKETS
  /* All >=200 HTTP status codes are errors when wanting ws */
  إذا(معلومات->req.upgr101 == UPGR101_WS) {
    failf(معلومات, "Refused WebSocket upgrade: %d", k->httpcode);
    نتيجو = CURLE_HTTP_RETURNED_ERROR;
    اذهب ٻاهريون;
  }
#جيڪڏهن_پورو

  /* Check if this response means the transfer errored. */
  إذا(http_should_fail(معلومات, معلومات->req.httpcode)) {
    failf(معلومات, "The requested URL returned error: %d",
          k->httpcode);
    نتيجو = CURLE_HTTP_RETURNED_ERROR;
    اذهب ٻاهريون;
  }

  /* Curl_http_auth_act() checks what authentication methods that are
   * available and decides which one (if any) to use. It will set 'newurl' if
   * an auth method was picked. */
  نتيجو = Curl_http_auth_act(معلومات);
  إذا(نتيجو)
    اذهب ٻاهريون;

  إذا(k->httpcode >= 300) {
    نتيجو = http_handle_send_error(معلومات);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }

  /* final response without error, prepare to receive the body */
  نتيجو = http_firstwrite(معلومات);
  إذا(نتيجو)
    اذهب ٻاهريون;

  /* This is the last response that we get for the current request. Check on
   * the body size and determine if the response is complete. */
  نتيجو = http_سائيز(معلومات);
  إذا(نتيجو)
    اذهب ٻاهريون;

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_RTSP
  /* An RTSP response to any method may carry a body, which is announced
     with Content-Length (RFC 2326 section 12.14). Read it instead of
     treating it as excess data, unless the application asked for no body
     with CURLOPT_NOBODY. */
  إذا((conn->scheme->protocol & CURLPROTO_RTSP) && معلومات->req.no_body &&
     !معلومات->سيٽ.opt_no_body && (k->سائيز > 0))
    معلومات->req.no_body = FALSE;
#جيڪڏهن_پورو

  /* If we requested a "no body", this is a good time to get
   * out and return home.
   */
  إذا(معلومات->req.no_body)
    k->download_پورو = TRUE;

  /* If max download size is *zero* (nothing) we already have nothing and can
     safely return ok now! For HTTP/2, we would like to call
     http2_handle_stream_close to properly close a stream. In order to do
     this, we keep reading until we close the stream. */
  إذا((k->maxdownload == 0) && (k->httpversion_موڪليل < 20))
    k->download_پورو = TRUE;

ٻاهريون:
  إذا(آخري_hd)
    /* if not written yet, write it now */
    نتيجو = Curl_1st_fatal(نتيجو,
                            http_write_سرو(معلومات, آخري_hd, آخري_hd_len));
  إذا(conn_بدليو)
    /* poke the multi handle to allow pending pipewait to retry */
    Curl_multi_connchanged(معلومات->multi);
  أرجع نتيجو;
}

ساكن CURLcode http_rw_hd(بنية Curl_easy *معلومات,
                           ثابت حرف *hd, size_t hdlen,
                           ثابت حرف *buf_remain, size_t blen,
                           size_t *pconsumed)
{
  CURLcode نتيجو = CURLE_OK;
  بنية SingleRequest *k = &معلومات->req;
  صحيح writetype;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  *pconsumed = 0;
  إذا((0x0a == *hd) || (0x0d == *hd)) {
    /* Empty header line means end of headers! */
    بنية dynbuf آخري_سرو;
    size_t consumed;

    curlx_dyn_init(&آخري_سرو, hdlen + 1);
    نتيجو = curlx_dyn_addn(&آخري_سرو, hd, hdlen);
    إذا(نتيجو)
      أرجع نتيجو;

    /* analyze the response to find out what to do. */
    /* Caveat: we clear anything in the header brigade, because a
     * response might switch HTTP version which may call use recursively.
     * Not nice, but that is currently the way of things. */
    curlx_dyn_reset(&معلومات->state.headerb);
    نتيجو = http_on_response(معلومات, curlx_dyn_ptr(&آخري_سرو),
                              curlx_dyn_len(&آخري_سرو),
                              buf_remain, blen, &consumed);
    *pconsumed += consumed;
    curlx_dyn_free(&آخري_سرو);
    أرجع نتيجو;
  }

  /*
   * Checks for special headers coming up.
   */

  writetype = CLIENTWRITE_سرو;
  إذا(!k->headerline++) {
    /* This is the first header, it MUST be the error code line
       or else we consider this to be the body right away! */
    bool fine_statusline = FALSE;

    k->httpversion = 0; /* Do not know yet */
    إذا(معلومات->conn->scheme->protocol & PROTO_FAMILY_HTTP) {
      /*
       * https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.2
       *
       * The response code is always a three-digit number in HTTP as the spec
       * says. We allow any three-digit number here, but we cannot make
       * guarantees on future behaviors since it is not within the protocol.
       */
      ثابت حرف *p = hd;

      curlx_str_passblanks(&p);
      إذا(!لڙي_ڀيٽ(p, "HTTP/", 5)) {
        p += 5;
        بدل(*p) {
        حالة '1':
          p++;
          إذا((p[0] == '.') && (p[1] == '0' || p[1] == '1')) {
            إذا(ISBLANK(p[2])) {
              k->httpversion = (بنانشان حرف)(10 + (p[1] - '0'));
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
            failf(معلومات, "Unsupported HTTP/1 subversion in response");
            أرجع CURLE_UNSUPPORTED_PROTOCOL;
          }
          اكسر;
        حالة '2':
        حالة '3':
          إذا(!ISBLANK(p[1]))
            اكسر;
          k->httpversion = (بنانشان حرف)((*p - '0') * 10);
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
          failf(معلومات, "Unsupported HTTP version in response");
          أرجع CURLE_UNSUPPORTED_PROTOCOL;
        }
      }

      إذا(!fine_statusline) {
        /* If user has set option HTTP200ALIASES,
           compare header line against list of aliases */
        statusline check = checkhttpprefix(معلومات, hd, hdlen);
        إذا(check == حالت_پورو) {
          fine_statusline = TRUE;
          k->httpcode = 200;
          k->httpversion = 10;
        }
      }
    }
    وإلا إذا(معلومات->conn->scheme->protocol & CURLPROTO_RTSP) {
      ثابت حرف *p = hd;
      بنية Curl_str ver;
      curl_off_t حالت;
      /* we set the max string a little excessive to forgive some leading
         spaces */
      إذا(!curlx_str_until(&p, &ver, 32, ' ') &&
         !curlx_str_single(&p, ' ') &&
         !curlx_str_عدد(&p, &حالت, 999)) {
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
      نتيجو = http_statusline(معلومات, معلومات->conn);
      إذا(نتيجو)
        أرجع نتيجو;
      writetype |= CLIENTWRITE_حالت;
    }
    وإلا {
      k->سرو = FALSE;   /* this is not a header line */
      أرجع CURLE_WEIRD_SERVER_جواب;
    }
  }

  نتيجو = Curl_verify_سرو(معلومات, hd, hdlen);
  إذا(نتيجو)
    أرجع نتيجو;

  نتيجو = http_سرو(معلومات, hd, hdlen);
  إذا(نتيجو)
    أرجع نتيجو;

  /*
   * Taken in one (more) header. Write it to the client.
   */
  Curl_debug(معلومات, CURLINFO_سرو_اندريون, hd, hdlen);

  إذا(k->httpcode / 100 == 1)
    writetype |= CLIENTWRITE_1XX;
  نتيجو = Curl_client_write(معلومات, writetype, hd, hdlen);
  إذا(نتيجو)
    أرجع نتيجو;

  نتيجو = Curl_bump_headersize(معلومات, hdlen, FALSE);
  إذا(نتيجو)
    أرجع نتيجو;

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

ساكن فراغ unfold_سرو(بنية Curl_easy *معلومات)
{
  Curl_http_منزل_fold(&معلومات->state.headerb);
  معلومات->state.leading_unfold = TRUE;
}

/*
 * Read any HTTP header lines from the server and pass them to the client app.
 */
ساكن CURLcode http_parse_سرا(بنية Curl_easy *معلومات,
                                   ثابت حرف *buf, size_t blen,
                                   size_t *pconsumed)
{
  بنية connectdata *conn = معلومات->conn;
  CURLcode نتيجو = CURLE_OK;
  بنية SingleRequest *k = &معلومات->req;
  ثابت حرف *پڇاڙي_ptr;
  bool leftover_body = FALSE;

  /* we have bytes for the next header, make sure it is not a folded header
     before passing it on */
  إذا(معلومات->state.maybe_folded && blen) {
    إذا(ISBLANK(buf[0])) {
      /* folded, remove the trailing newlines and append the next header */
      unfold_سرو(معلومات);
    }
    وإلا {
      /* the header data we hold is a complete header, pass it on */
      size_t ignore_this;
      نتيجو = http_rw_hd(معلومات, curlx_dyn_ptr(&معلومات->state.headerb),
                          curlx_dyn_len(&معلومات->state.headerb),
                          عدم, 0, &ignore_this);
      curlx_dyn_reset(&معلومات->state.headerb);
      إذا(نتيجو)
        أرجع نتيجو;
    }
    معلومات->state.maybe_folded = FALSE;
  }

  /* header line within buffer loop */
  *pconsumed = 0;
  طالما(blen && k->سرو) {
    size_t consumed;
    size_t hlen;
    ثابت حرف *hd;
    size_t unfold_len = 0;

    إذا(معلومات->state.leading_unfold) {
      /* immediately after an unfold, keep only a single whitespace */
      طالما(blen && ISBLANK(buf[0])) {
        buf++;
        blen--;
        unfold_len++;
      }
      إذا(blen) {
        /* insert a single space */
        نتيجو = curlx_dyn_addn(&معلومات->state.headerb, " ", 1);
        إذا(نتيجو)
          أرجع نتيجو;
        معلومات->state.leading_unfold = FALSE; /* done now */
      }
    }

    پڇاڙي_ptr = memchr(buf, '\n', blen);
    إذا(!پڇاڙي_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      نتيجو = curlx_dyn_addn(&معلومات->state.headerb, buf, blen);
      إذا(نتيجو)
        أرجع نتيجو;
      *pconsumed += blen + unfold_len;

      إذا(!k->headerline) {
        /* check if this looks like a protocol header */
        statusline st =
          checkprotoprefix(معلومات, conn,
                           curlx_dyn_ptr(&معلومات->state.headerb),
                           curlx_dyn_len(&معلومات->state.headerb));

        إذا(st == حالت_BAD) {
          /* this is not the beginning of a protocol first header line.
           * Cannot be 0.9 if version was detected or connection was reused. */
          k->سرو = FALSE;
          streamclose(conn);
          إذا((k->httpversion >= 10) || conn->بٽ.reuse) {
            failf(معلومات, "Invalid status line");
            أرجع CURLE_WEIRD_SERVER_جواب;
          }
          إذا(!معلومات->state.http_neg.accept_09) {
            failf(معلومات, "Received HTTP/0.9 when not allowed");
            أرجع CURLE_UNSUPPORTED_PROTOCOL;
          }
          leftover_body = TRUE;
          اذهب ٻاهريون;
        }
      }
      اذهب ٻاهريون; /* read more and try again */
    }

    /* the size of the remaining header line */
    consumed = (پڇاڙي_ptr - buf) + 1;

    نتيجو = curlx_dyn_addn(&معلومات->state.headerb, buf, consumed);
    إذا(نتيجو)
      أرجع نتيجو;
    blen -= consumed;
    buf += consumed;
    *pconsumed += consumed + unfold_len;

    /****
     * We now have a FULL header line in 'headerb'.
     *****/

    hlen = curlx_dyn_len(&معلومات->state.headerb);
    hd = curlx_dyn_ptr(&معلومات->state.headerb);

    إذا(!k->headerline) {
      /* the first read "header", the status line */
      statusline st = checkprotoprefix(معلومات, conn, hd, hlen);
      إذا(st == حالت_BAD) {
        streamclose(conn);
        /* this is not the beginning of a protocol first header line.
         * Cannot be 0.9 if version was detected or connection was reused. */
        إذا((k->httpversion >= 10) || conn->بٽ.reuse) {
          failf(معلومات, "Invalid status line");
          أرجع CURLE_WEIRD_SERVER_جواب;
        }
        إذا(!معلومات->state.http_neg.accept_09) {
          failf(معلومات, "Received HTTP/0.9 when not allowed");
          أرجع CURLE_UNSUPPORTED_PROTOCOL;
        }
        k->سرو = FALSE;
        leftover_body = TRUE;
        اذهب ٻاهريون;
      }
    }
    وإلا {
      إذا(hlen && !ISNEWLINE(hd[0])) {
        /* this is NOT the header separator */

        /* if we have bytes for the next header, check for folding */
        إذا(blen && ISBLANK(buf[0])) {
          /* remove the trailing CRLF and append the next header */
          unfold_سرو(معلومات);
          استمر;
        }
        وإلا إذا(!blen) {
          /* this might be a folded header so deal with it in next invoke */
          معلومات->state.maybe_folded = TRUE;
          اكسر;
        }
      }
    }

    نتيجو = http_rw_hd(معلومات, hd, hlen, buf, blen, &consumed);
    /* We are done with this line. We reset because response
     * processing might switch to HTTP/2 and that might call us
     * directly again. */
    curlx_dyn_reset(&معلومات->state.headerb);
    إذا(consumed) {
      blen -= consumed;
      buf += consumed;
      *pconsumed += consumed;
    }
    إذا(نتيجو)
      أرجع نتيجو;
  }

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */
ٻاهريون:
  إذا(!k->سرو && !leftover_body) {
    curlx_dyn_free(&معلومات->state.headerb);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_http_write_resp_hd(بنية Curl_easy *معلومات,
                                 ثابت حرف *hd, size_t hdlen,
                                 bool is_eos)
{
  CURLcode نتيجو;
  size_t consumed;
  حرف tmp = 0;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  نتيجو = http_rw_hd(معلومات, hd, hdlen, &tmp, 0, &consumed);
  إذا(!نتيجو && is_eos) {
    نتيجو = Curl_client_write(معلومات, (CLIENTWRITE_BODY | CLIENTWRITE_EOS),
                               &tmp, 0);
  }
  أرجع نتيجو;
}

/*
 * HTTP protocol `write_resp` implementation. Parse headers
 * when not done yet and otherwise return without consuming data.
 */
CURLcode Curl_http_write_resp_hds(بنية Curl_easy *معلومات,
                                  ثابت حرف *buf, size_t blen,
                                  size_t *pconsumed)
{
  إذا(!معلومات->req.سرو) {
    *pconsumed = 0;
    أرجع CURLE_OK;
  }
  وإلا {
    CURLcode نتيجو;

    نتيجو = http_parse_سرا(معلومات, buf, blen, pconsumed);
    إذا(!نتيجو && !معلومات->req.سرو) {
      إذا(!معلومات->req.no_body && curlx_dyn_len(&معلومات->state.headerb)) {
        /* leftover from parsing something that turned out not
         * to be a header, only happens if we allow for
         * HTTP/0.9 like responses */
        نتيجو = Curl_client_write(معلومات, CLIENTWRITE_BODY,
                                   curlx_dyn_ptr(&معلومات->state.headerb),
                                   curlx_dyn_len(&معلومات->state.headerb));
      }
      curlx_dyn_free(&معلومات->state.headerb);
    }
    أرجع نتيجو;
  }
}

CURLcode Curl_http_write_resp(بنية Curl_easy *معلومات,
                              ثابت حرف *buf, size_t blen,
                              bool is_eos)
{
  CURLcode نتيجو;
  size_t consumed;
  صحيح flags;

  نتيجو = Curl_http_write_resp_hds(معلومات, buf, blen, &consumed);
  إذا(نتيجو || معلومات->req.پورو)
    اذهب ٻاهريون;

  DEBUGASSERT(consumed <= blen);
  blen -= consumed;
  buf += consumed;
  /* either all was consumed in header parsing, or we have data left
   * and are done with headers, e.g. it is BODY data */
  DEBUGASSERT(!blen || !معلومات->req.سرو);
  إذا(!معلومات->req.سرو && (blen || is_eos)) {
    /* BODY data after header been parsed, write and consume */
    flags = CLIENTWRITE_BODY;
    إذا(is_eos)
      flags |= CLIENTWRITE_EOS;
    نتيجو = Curl_client_write(معلومات, flags, buf, blen);
  }
ٻاهريون:
  أرجع نتيجو;
}

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_حالت(int32_t *pstatus, ثابت حرف *s, size_t len)
{
  CURLcode نتيجو = CURLE_BAD_FUNCTION_دليل;
  int32_t حالت = 0;
  صحيح i;

  إذا(len != 3)
    اذهب ٻاهريون;

  لكل(i = 0; i < 3; ++i) {
    حرف c = s[i];

    إذا(c < '0' || c > '9')
      اذهب ٻاهريون;

    حالت *= 10;
    حالت += c - '0';
  }
  نتيجو = CURLE_OK;
ٻاهريون:
  *pstatus = نتيجو ? -1 : حالت;
  أرجع نتيجو;
}

CURLcode Curl_http_req_make(بنية httpreq **preq,
                            ثابت حرف *طريقو, size_t m_len,
                            ثابت حرف *scheme, size_t s_len,
                            ثابت حرف *authority, size_t a_len,
                            ثابت حرف *رستو, size_t p_len)
{
  بنية httpreq *req;
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;

  DEBUGASSERT(طريقو && m_len);

  req = curlx_calloc(1, حجم(*req) + m_len);
  إذا(!req)
    اذهب ٻاهريون;
#إذا defined(__GNUC__) && __GNUC__ >= 13
#هدايت GCC diagnostic ڌڪو
/* error: 'memcpy' offset [137, 142] from the object at 'req' is out of
   the bounds of referenced subobject 'method' with type 'char[1]' at
   offset 136 */
#هدايت GCC diagnostic ignored "-Warray-bounds"
#جيڪڏهن_پورو
  يادگيري_نقل(req->طريقو, طريقو, m_len);
#إذا defined(__GNUC__) && __GNUC__ >= 13
#هدايت GCC diagnostic ڪڍو
#جيڪڏهن_پورو
  إذا(scheme) {
    req->scheme = curlx_memdup0(scheme, s_len);
    إذا(!req->scheme)
      اذهب ٻاهريون;
  }
  إذا(authority) {
    req->authority = curlx_memdup0(authority, a_len);
    إذا(!req->authority)
      اذهب ٻاهريون;
  }
  إذا(رستو) {
    req->رستو = curlx_memdup0(رستو, p_len);
    إذا(!req->رستو)
      اذهب ٻاهريون;
  }
  Curl_dynhds_init(&req->سرا, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  نتيجو = CURLE_OK;

ٻاهريون:
  إذا(نتيجو && req)
    Curl_http_req_free(req);
  *preq = نتيجو ? عدم : req;
  أرجع نتيجو;
}

ساكن CURLcode req_سونپيو_url_authority(بنية httpreq *req, CURLU *url)
{
  حرف *host, *پورٽ;
  بنية dynbuf buf;
  CURLUcode uc;
  CURLcode نتيجو = CURLE_URL_MALFORMAT;

  host = پورٽ = عدم;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_HOST, &host, 0);
  إذا(uc && uc != CURLUE_NO_HOST)
    اذهب ٻاهريون;
  إذا(!host) {
    req->authority = عدم;
    نتيجو = CURLE_OK;
    اذهب ٻاهريون;
  }

  uc = curl_url_get(url, CURLUPART_پورٽ, &پورٽ, CURLU_NO_DEFAULT_پورٽ);
  إذا(uc && uc != CURLUE_NO_پورٽ)
    اذهب ٻاهريون;

  نتيجو = curlx_dyn_جوڙيو(&buf, host);
  إذا(نتيجو)
    اذهب ٻاهريون;
  إذا(پورٽ) {
    نتيجو = curlx_dyn_addf(&buf, ":%s", پورٽ);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
  req->authority = curlx_dyn_ptr(&buf);
ٻاهريون:
  curlx_free(host);
  curlx_free(پورٽ);
  إذا(نتيجو)
    curlx_dyn_free(&buf);
  أرجع نتيجو;
}

ساكن CURLcode req_سونپيو_url_رستو(بنية httpreq *req, CURLU *url)
{
  حرف *رستو, *query;
  بنية dynbuf buf;
  CURLUcode uc;
  CURLcode نتيجو = CURLE_URL_MALFORMAT;

  رستو = query = عدم;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_رستو, &رستو, 0);
  إذا(uc)
    اذهب ٻاهريون;
  uc = curl_url_get(url, CURLUPART_QUERY, &query, 0);
  إذا(uc && uc != CURLUE_NO_QUERY)
    اذهب ٻاهريون;

  إذا(!query) {
    req->رستو = رستو;
    رستو = عدم;
  }
  وإلا {
    نتيجو = curlx_dyn_جوڙيو(&buf, رستو);
    إذا(!نتيجو)
      نتيجو = curlx_dyn_addf(&buf, "?%s", query);
    إذا(نتيجو)
      اذهب ٻاهريون;
    req->رستو = curlx_dyn_ptr(&buf);
  }
  نتيجو = CURLE_OK;

ٻاهريون:
  curlx_free(رستو);
  curlx_free(query);
  إذا(نتيجو)
    curlx_dyn_free(&buf);
  أرجع نتيجو;
}

CURLcode Curl_http_req_make2(بنية httpreq **preq,
                             ثابت حرف *طريقو, size_t m_len,
                             CURLU *url, ثابت حرف *scheme_default)
{
  بنية httpreq *req;
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  CURLUcode uc;

  DEBUGASSERT(طريقو && m_len);

  req = curlx_calloc(1, حجم(*req) + m_len);
  إذا(!req)
    اذهب ٻاهريون;
  يادگيري_نقل(req->طريقو, طريقو, m_len);

  uc = curl_url_get(url, CURLUPART_SCHEME, &req->scheme, 0);
  إذا(uc && uc != CURLUE_NO_SCHEME)
    اذهب ٻاهريون;
  إذا(!req->scheme && scheme_default) {
    req->scheme = curlx_strdup(scheme_default);
    إذا(!req->scheme)
      اذهب ٻاهريون;
  }

  نتيجو = req_سونپيو_url_authority(req, url);
  إذا(نتيجو)
    اذهب ٻاهريون;
  نتيجو = req_سونپيو_url_رستو(req, url);
  إذا(نتيجو)
    اذهب ٻاهريون;

  Curl_dynhds_init(&req->سرا, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  نتيجو = CURLE_OK;

ٻاهريون:
  إذا(نتيجو && req)
    Curl_http_req_free(req);
  *preq = نتيجو ? عدم : req;
  أرجع نتيجو;
}

فراغ Curl_http_req_free(بنية httpreq *req)
{
  إذا(req) {
    curlx_free(req->scheme);
    curlx_free(req->authority);
    curlx_free(req->رستو);
    Curl_dynhds_free(&req->سرا);
    Curl_dynhds_free(&req->trailers);
    curlx_free(req);
  }
}

بنية name_const {
  ثابت حرف *name;
  size_t namelen;
};

ساكن ثابت بنية name_const H2_NON_ميدان[] = {
  { STRCONST("Host") },
  { STRCONST("Upgrade") },
  { STRCONST("Connection") },
  { STRCONST("Keep-Alive") },
  { STRCONST("Proxy-Connection") },
  { STRCONST("Transfer-Encoding") },
};

ساكن bool h2_permissible_ميدان(بنية dynhds_داخلا *e)
{
  size_t i;
  لكل(i = 0; i < CURL_ARRAYSIZE(H2_NON_ميدان); ++i) {
    إذا(e->namelen == H2_NON_ميدان[i].namelen &&
       curl_strnequal(H2_NON_ميدان[i].name, e->name, e->namelen))
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
        إذا(curlx_str_quotedword(&fvalue, &qw, CURL_MAX_HTTP_سرو))
          أرجع FALSE;
      }
      وإلا
        fvalue++;
    }
  }
  أرجع FALSE;
}

CURLcode Curl_http_req_منزل_h2(بنية dynhds *h2_سرا,
                             بنية httpreq *req, بنية Curl_easy *معلومات)
{
  ثابت حرف *scheme = عدم, *authority = عدم;
  بنية dynhds_داخلا *e;
  size_t i;
  CURLcode نتيجو;

  DEBUGASSERT(req);
  DEBUGASSERT(h2_سرا);

  إذا(req->scheme) {
    scheme = req->scheme;
  }
  وإلا إذا(strcmp("CONNECT", req->طريقو)) {
    scheme = Curl_checkheaders(معلومات, STRCONST(HTTP_PSEUDO_SCHEME));
    إذا(scheme) {
      scheme += حجم(HTTP_PSEUDO_SCHEME);
      curlx_str_passblanks(&scheme);
      infof(معلومات, "set pseudo header %s to %s", HTTP_PSEUDO_SCHEME, scheme);
    }
    وإلا {
      scheme = معلومات->state.origin->scheme->name;
    }
  }

  إذا(req->authority) {
    authority = req->authority;
  }
  وإلا {
    e = Curl_dynhds_get(&req->سرا, STRCONST("Host"));
    إذا(e)
      authority = e->قدر;
  }

  Curl_dynhds_reset(h2_سرا);
  Curl_dynhds_سيٽ_opts(h2_سرا, DYNHDS_OPT_LOWERCASE);
  نتيجو = Curl_dynhds_جوڙيو(h2_سرا, STRCONST(HTTP_PSEUDO_طريقو),
                           req->طريقو, لڙي_ڊيگهه(req->طريقو));
  إذا(!نتيجو && scheme) {
    نتيجو = Curl_dynhds_جوڙيو(h2_سرا, STRCONST(HTTP_PSEUDO_SCHEME),
                             scheme, لڙي_ڊيگهه(scheme));
  }
  إذا(!نتيجو && authority) {
    نتيجو = Curl_dynhds_جوڙيو(h2_سرا, STRCONST(HTTP_PSEUDO_AUTHORITY),
                             authority, لڙي_ڊيگهه(authority));
  }
  إذا(!نتيجو && req->رستو) {
    نتيجو = Curl_dynhds_جوڙيو(h2_سرا, STRCONST(HTTP_PSEUDO_رستو),
                             req->رستو, لڙي_ڊيگهه(req->رستو));
  }
  لكل(i = 0; !نتيجو && i < Curl_dynhds_ڳڻپ(&req->سرا); ++i) {
    e = Curl_dynhds_getn(&req->سرا, i);
    /* "TE" is special in that it is only permissible when it
     * has only value "trailers". RFC 9113 ch. 8.2.2 */
    إذا(e->namelen == 2 && curl_strequal("TE", e->name)) {
      إذا(http_TE_has_token(e->قدر, "trailers"))
        نتيجو = Curl_dynhds_جوڙيو(h2_سرا, e->name, e->namelen,
                                 "trailers", CURL_CSTRLEN("trailers"));
    }
    وإلا إذا(h2_permissible_ميدان(e)) {
      نتيجو = Curl_dynhds_جوڙيو(h2_سرا, e->name, e->namelen,
                               e->قدر, e->valuelen);
    }
  }

  أرجع نتيجو;
}

CURLcode Curl_http_resp_make(بنية http_resp **presp,
                             صحيح حالت,
                             ثابت حرف *description)
{
  بنية http_resp *resp;
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;

  resp = curlx_calloc(1, حجم(*resp));
  إذا(!resp)
    اذهب ٻاهريون;

  resp->حالت = حالت;
  إذا(description) {
    resp->description = curlx_strdup(description);
    إذا(!resp->description)
      اذهب ٻاهريون;
  }
  Curl_dynhds_init(&resp->سرا, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&resp->trailers, 0, DYN_HTTP_REQUEST);
  نتيجو = CURLE_OK;

ٻاهريون:
  إذا(نتيجو && resp)
    Curl_http_resp_free(resp);
  *presp = نتيجو ? عدم : resp;
  أرجع نتيجو;
}

فراغ Curl_http_resp_free(بنية http_resp *resp)
{
  إذا(resp) {
    curlx_free(resp->description);
    Curl_dynhds_free(&resp->سرا);
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
  Curl_http_تياري_conn,                 /* setup_connection */
  Curl_http,                            /* do_it */
  Curl_http_پورو,                       /* done */
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

#جيڪڏهن_پورو /* CURL_DISABLE_HTTP */
