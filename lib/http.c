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
#شامل "curl_setup.h"
#شامل "urldata.h"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NET_IF_H
#شامل <net/if.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_SYS_PARAM_H
#شامل <sys/param.h>
#اگر_ختم

#شامل "transfer.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "formdata.h"
#شامل "mime.h"
#شامل "progress.h"
#شامل "curlx/base64.h"
#شامل "cookie.h"
#شامل "vauth/vauth.h"
#شامل "vquic/vquic.h"
#شامل "http_digest.h"
#شامل "http_ntlm.h"
#شامل "http_negotiate.h"
#شامل "http_aws_sigv4.h"
#شامل "http_httpsig.h"
#شامل "url.h"
#شامل "urlapi-int.h"
#شامل "curl_share.h"
#شامل "dynhds.h"
#شامل "http.h"
#شامل "headers.h"
#شامل "select.h"
#شامل "parsedate.h" /* for the week day and month names */
#شامل "multiif.h"
#شامل "strcase.h"
#شامل "content_encoding.h"
#شامل "http_proxy.h"
#شامل "http2.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "curlx/strdup.h"
#شامل "altsvc.h"
#شامل "hsts.h"
#شامل "rtsp.h"
#شامل "ws.h"
#شامل "bufref.h"
#شامل "curlx/strparse.h"

خالی Curl_http_neg_init(ساخت Curl_easy *ڈیٹا, ساخت http_negotiation *neg)
{
  memset(neg, 0, حجم(*neg));
  neg->accept_09 = ڈیٹا->سیٹ.http09_allowed;
  انتخاب(ڈیٹا->سیٹ.httpwant) {
  حالت CURL_HTTP_نسخہ_1_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    neg->only_10 = TRUE;
    توڑو;
  حالت CURL_HTTP_نسخہ_1_1:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x);
    توڑو;
  حالت CURL_HTTP_نسخہ_2_0:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->h2_upgrade = TRUE;
    توڑو;
  حالت CURL_HTTP_نسخہ_2_TLS:
    neg->wanted = neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    توڑو;
  حالت CURL_HTTP_نسخہ_2_پیشگی_KNOWLEDGE:
    neg->wanted = neg->allowed = (CURL_HTTP_V2x);
    ڈیٹا->state.http_neg.h2_پیشگی_knowledge = TRUE;
    توڑو;
  حالت CURL_HTTP_نسخہ_3:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    neg->allowed = neg->wanted;
    توڑو;
  حالت CURL_HTTP_نسخہ_3_ONLY:
    neg->wanted = neg->allowed = (CURL_HTTP_V3x);
    توڑو;
  حالت CURL_HTTP_نسخہ_NONE:
  طےشدہ:
    neg->wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
    neg->allowed = (CURL_HTTP_V1x | CURL_HTTP_V2x | CURL_HTTP_V3x);
    توڑو;
  }
}

CURLcode Curl_http_تیاری_conn(ساخت Curl_easy *ڈیٹا,
                              ساخت connectdata *conn)
{
  /* allocate the HTTP-specific struct for the Curl_easy, only to survive
     during this request */
  اگر(ڈیٹا->state.http_neg.wanted == CURL_HTTP_V3x) {
    /* only HTTP/3, needs to work */
    CURLcode نتیجہ = Curl_conn_may_http3(ڈیٹا, conn);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
  واپس CURLE_OK;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
/*
 * checkProxyHeaders() checks the linked list of custom proxy headers
 * if proxy headers are not available, then it will lookup into http header
 * link list
 *
 * It takes a connectdata struct as input to see if this is a proxy request or
 * not, as it then might check a different header list. Provide the header
 * prefix without colon!
 */
حرف *Curl_checkProxyheaders(ساخت Curl_easy *ڈیٹا,
                             ثابت ساخت connectdata *conn,
                             ثابت حرف *thisheader,
                             ثابت size_t thislen)
{
  ساخت curl_slist *سر;

  برائے(سر = (conn->http_proxy.peer && ڈیٹا->سیٹ.sep_سرخیاں) ?
        ڈیٹا->سیٹ.proxyheaders : ڈیٹا->سیٹ.سرخیاں;
      سر; سر = سر->اگلا) {
    اگر(curl_strnequal(سر->ڈیٹا, thisheader, thislen) &&
       Curl_headersep(سر->ڈیٹا[thislen]))
      واپس سر->ڈیٹا;
  }

  واپس تہی;
}
#اگر_ختم

/* If the header has a value, this function returns TRUE and the value is in
   'outp' with blanks trimmed off. */
ساکن bool سرخی_has_قدر(ثابت حرف **headerp, ساخت Curl_str *outp)
{
  bool قدر = !curlx_str_cspn(headerp, outp, ";:") &&
    (!curlx_str_single(headerp, ':') || !curlx_str_single(headerp, ';'));

  اگر(قدر) {
    curlx_str_cspn(headerp, outp, "\r\n");
    curlx_str_trimblanks(outp);
  }
  واپس قدر;
}

ساکن bool http_سرخی_is_خالی_خانے(ثابت حرف *سرخی)
{
  ساخت Curl_str برآمد;

  اگر(سرخی_has_قدر(&سرخی, &برآمد)) {
    واپس curlx_strlen(&برآمد) == 0;
  }
  واپس TRUE; /* invalid header format, treat as empty */
}

/*
 * Strip off leading and trailing whitespace from the value in the given HTTP
 * header line and return a strdup-ed copy in 'valp' - returns an empty
 * string if the header value consists entirely of whitespace.
 *
 * If the header is provided as "name;", ending with a semicolon, it returns a
 * blank string.
 */
ساکن CURLcode copy_custom_قدر(ثابت حرف *سرخی, حرف **valp)
{
  ساخت Curl_str برآمد = { 0 };

  /* find the end of the header name */
  اگر(سرخی_has_قدر(&سرخی, &برآمد)) {
    *valp = curlx_memdup0(curlx_str(&برآمد), curlx_strlen(&برآمد));
    اگر(*valp)
      واپس CURLE_OK;
    واپس CURLE_برآمد_کا_حافظہ;
  }
  /* bad input */
  *valp = تہی;
  واپس CURLE_BAD_FUNCTION_دلیل;
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
  ساخت Curl_str برآمد;

  /* find the end of the header name */
  اگر(!curlx_str_until(&سرخی, &برآمد, MAX_HTTP_RESP_سرخی_سائز, ':') &&
     !curlx_str_single(&سرخی, ':')) {
    curlx_str_untilnl(&سرخی, &برآمد, MAX_HTTP_RESP_سرخی_سائز);
    curlx_str_trimblanks(&برآمد);
    واپس curlx_memdup0(curlx_str(&برآمد), curlx_strlen(&برآمد));
  }
  /* bad input, should never happen */
  DEBUGASSERT(0);
  واپس تہی;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP_AUTH

#اگر_غیر_تعریف_شدہ CURL_DISABLE_BASIC_AUTH
/*
 * http_output_basic() sets up an Authorization: header (or the proxy version)
 * for HTTP Basic authentication.
 *
 * Returns CURLcode.
 */
ساکن CURLcode http_output_basic(ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata *conn, bool proxy)
{
  size_t سائز = 0;
  حرف *authorization = تہی;
  حرف **p_hd;
  CURLcode نتیجہ;
  ساخت Curl_creds *creds = تہی;
  حرف *برآمد;

  /* credentials are unique per transfer for HTTP, do not use the ones for the
     connection */
  اگر(proxy) {
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
    p_hd = &ڈیٹا->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
#ورنہ
    (خالی)conn;
    واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
  }
  ورنہ {
    p_hd = &ڈیٹا->req.hd_auth;
    creds = ڈیٹا->state.creds;
  }

  اگر(!creds) {
    DEBUGASSERT(0);
    واپس CURLE_FAILED_INIT;
  }

  برآمد = curl_maprintf("%s:%s", creds->user, creds->passwd);
  اگر(!برآمد)
    واپس CURLE_برآمد_کا_حافظہ;

  نتیجہ = curlx_base64_encode((uint8_t *)برآمد, لڑی_لمبائی(برآمد),
                               &authorization, &سائز);
  اگر(نتیجہ)
    جاؤ fail;

  اگر(!authorization) {
    نتیجہ = CURLE_REMOTE_ACCESS_DENIED;
    جاؤ fail;
  }

  curlx_free(*p_hd);
  *p_hd = curl_maprintf("%sAuthorization: Basic %s\r\n",
                        proxy ? "Proxy-" : "",
                        authorization);
  curlx_free(authorization);
  اگر(!*p_hd) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ fail;
  }

fail:
  curlx_free(برآمد);
  واپس نتیجہ;
}

#اگر_ختم

#اگر_غیر_تعریف_شدہ CURL_DISABLE_BEARER_AUTH
/*
 * http_output_bearer() sets up an Authorization: header
 * for HTTP Bearer authentication.
 *
 * Returns CURLcode.
 */
ساکن CURLcode http_output_bearer(ساخت Curl_easy *ڈیٹا)
{
  حرف **userp;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(Curl_creds_has_oauth_bearer(ڈیٹا->state.creds));
  userp = &ڈیٹا->req.hd_auth;
  curlx_free(*userp);
  *userp = curl_maprintf("Authorization: Bearer %s\r\n",
                         Curl_creds_oauth_bearer(ڈیٹا->state.creds));

  اگر(!*userp) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ fail;
  }

fail:
  واپس نتیجہ;
}
#اگر_ختم

#اگر_ختم

/* pickoneauth() selects the most favorable authentication method from the
 * ones available and the ones we want.
 *
 * return TRUE if one was picked
 */
ساکن bool pickoneauth(ساخت auth *pick, غیرعلامتی طویل نقاب,
                        ساخت Curl_creds *creds)
{
  bool have_user_pass = Curl_creds_has_user_or_pass(creds);
  bool picked;
  /* only deal with authentication we want */
  غیرعلامتی طویل avail = pick->avail & pick->want & نقاب;
  picked = TRUE;

  /* The order of these checks is highly relevant, as this will be the order
     of preference in case of the existence of multiple accepted types. */
  اگر(avail & CURLAUTH_NEGOTIATE)  /* available on empty creds */
    pick->picked = CURLAUTH_NEGOTIATE;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_BEARER_AUTH
  ورنہ اگر((avail & CURLAUTH_BEARER) && Curl_creds_has_oauth_bearer(creds))
    pick->picked = CURLAUTH_BEARER;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
  ورنہ اگر((avail & CURLAUTH_DIGEST) && have_user_pass)
    pick->picked = CURLAUTH_DIGEST;
#اگر_ختم
  ورنہ اگر(avail & CURLAUTH_NTLM)
    pick->picked = CURLAUTH_NTLM;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_BASIC_AUTH
  ورنہ اگر((avail & CURLAUTH_BASIC) && have_user_pass)
    pick->picked = CURLAUTH_BASIC;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_AWS
  ورنہ اگر(avail & CURLAUTH_AWS_SIGV4)
    pick->picked = CURLAUTH_AWS_SIGV4;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTPSIG
  ورنہ اگر(avail & CURLAUTH_HTTPSIG)
    pick->picked = CURLAUTH_HTTPSIG;
#اگر_ختم
  ورنہ {
    pick->picked = CURLAUTH_PICKNONE; /* we select to use nothing */
    picked = FALSE;
  }
  pick->avail = CURLAUTH_NONE; /* clear it here */

  واپس picked;
}

/*
 * http_perhapsrewind()
 *
 * The current request needs to be done again - maybe due to a follow
 * or authentication negotiation. Check if:
 * 1) a rewind of the data sent to the server is necessary
 * 2) the current transfer should continue or be stopped early
 */
ساکن CURLcode http_perhapsrewind(ساخت Curl_easy *ڈیٹا,
                                   ساخت connectdata *conn)
{
  curl_off_t bytessent = ڈیٹا->req.writebytecount;
  curl_off_t expectsend = Curl_creader_کل_لمبائی(ڈیٹا);
  curl_off_t upload_remain = (expectsend >= 0) ? (expectsend - bytessent) : -1;
  bool little_upload_remains = (upload_remain >= 0 && upload_remain < 2000);
  bool needs_rewind = Curl_creader_needs_rewind(ڈیٹا);
  /* By default, we would like to abort the transfer when little or unknown
   * amount remains. This may be overridden by authentications further
   * below! */
  bool abort_upload = (!ڈیٹا->req.upload_مکمل && !little_upload_remains);
  VERBOSE(ثابت حرف *ongoing_auth = تہی);

  /* We need a rewind before uploading client read data again. The
   * checks below influence of the upload is to be continued
   * or aborted early.
   * This depends on how much remains to be sent and in what state
   * the authentication is. Some auth schemes such as NTLM do not work
   * for a new connection. */
  اگر(needs_rewind) {
    infof(ڈیٹا, "Need to rewind upload for next request");
    Curl_creader_سیٹ_rewind(ڈیٹا, TRUE);
  }

  اگر(conn->بٹ.close)
    /* If we already decided to close this connection, we cannot veto. */
    واپس CURLE_OK;

  اگر(abort_upload) {
    /* We would like to abort the upload - but should we? */
#اگر_تعریف_شدہ USE_NTLM
    اگر((ڈیٹا->state.authproxy.picked == CURLAUTH_NTLM) ||
       (ڈیٹا->state.authhost.picked == CURLAUTH_NTLM)) {
      VERBOSE(ongoing_auth = "NTLM");
      اگر((conn->http_ntlm_state != NTLMSTATE_NONE) ||
         (conn->proxy_ntlm_state != NTLMSTATE_NONE)) {
        /* The NTLM-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#اگر_ختم
#اگر_تعریف_شدہ USE_SPNEGO
    /* There is still data left to send */
    اگر((ڈیٹا->state.authproxy.picked == CURLAUTH_NEGOTIATE) ||
       (ڈیٹا->state.authhost.picked == CURLAUTH_NEGOTIATE)) {
      VERBOSE(ongoing_auth = "NEGOTIATE");
      اگر((conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) ||
         (conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE)) {
        /* The NEGOTIATE-negotiation has started, keep on sending.
         * Need to do further work on same connection */
        abort_upload = FALSE;
      }
    }
#اگر_ختم
  }

  اگر(abort_upload) {
    اگر(upload_remain >= 0)
      infof(ڈیٹا, "%s%sclose instead of sending %" FMT_OFF_T " more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "",
            upload_remain);
    ورنہ
      infof(ڈیٹا, "%s%sclose instead of sending unknown amount "
            "of more bytes",
            ongoing_auth ? ongoing_auth : "",
            ongoing_auth ? " send, " : "");
    /* We decided to abort the ongoing transfer */
    streamclose(conn);
    ڈیٹا->req.سائز = 0; /* do not download any more than 0 bytes */
    ڈیٹا->req.http_bodyless = TRUE;
  }
  واپس CURLE_OK;
}

/**
 * http_should_fail() determines whether an HTTP response code has gotten us
 * into an error state or not.
 *
 * @retval FALSE communications should continue
 *
 * @retval TRUE communications should not continue
 */
ساکن bool http_should_fail(ساخت Curl_easy *ڈیٹا, صحیح httpcode)
{
  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(ڈیٹا->conn);

  /*
   * If we have not been asked to fail on error,
   * do not fail.
   */
  اگر(!ڈیٹا->سیٹ.http_fail_on_error)
    واپس FALSE;

  /*
   * Any code < 400 is never terminal.
   */
  اگر(httpcode < 400)
    واپس FALSE;

  /*
   * A 416 response to a resume request is presumably because the file is
   * already completely downloaded and thus not actually a fail.
   */
  اگر(ڈیٹا->state.resume_from && ڈیٹا->state.httpreq == HTTPREQ_GET &&
     httpcode == 416)
    واپس FALSE;

  /*
   * Any code >= 400 that is not 401 or 407 is always
   * a terminal error
   */
  اگر((httpcode != 401) && (httpcode != 407))
    واپس TRUE;

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
  اگر((httpcode == 401) && !ڈیٹا->state.creds)
    واپس TRUE;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  اگر((httpcode == 407) && !ڈیٹا->conn->http_proxy.creds)
    واپس TRUE;
#اگر_ختم

  واپس (bool)ڈیٹا->state.authproblem;
}

/*
 * Curl_http_auth_act() gets called when all HTTP headers have been received
 * and it checks what authentication methods that are available and decides
 * which one (if any) to use. It will set 'newurl' if an auth method was
 * picked.
 */
CURLcode Curl_http_auth_act(ساخت Curl_easy *ڈیٹا)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  bool pickhost = FALSE;
  bool pickproxy = FALSE;
  CURLcode نتیجہ = CURLE_OK;
  غیرعلامتی طویل authmask = ~0UL;

  اگر(!Curl_creds_has_oauth_bearer(ڈیٹا->state.creds))
    authmask &= (غیرعلامتی طویل)~CURLAUTH_BEARER;

  اگر(100 <= ڈیٹا->req.httpcode && ڈیٹا->req.httpcode <= 199)
    /* this is a transient response code, ignore */
    واپس CURLE_OK;

  اگر(ڈیٹا->state.authproblem)
    واپس ڈیٹا->سیٹ.http_fail_on_error ? CURLE_HTTP_RETURNED_ERROR : CURLE_OK;

  اگر(ڈیٹا->state.creds &&
     ((ڈیٹا->req.httpcode == 401) ||
      (ڈیٹا->req.authneg && ڈیٹا->req.httpcode < 300))) {
    pickhost = pickoneauth(&ڈیٹا->state.authhost, authmask, ڈیٹا->state.creds);
    اگر(!pickhost)
      ڈیٹا->state.authproblem = TRUE;
    ورنہ
      ڈیٹا->info.httpauthpicked = ڈیٹا->state.authhost.picked;
    اگر(ڈیٹا->state.authhost.picked == CURLAUTH_NTLM &&
       (ڈیٹا->req.httpversion_بھیجا > 11)) {
      infof(ڈیٹا, "Forcing HTTP/1.1 for NTLM");
      connclose(conn);
      ڈیٹا->state.http_neg.wanted = CURL_HTTP_V1x;
      ڈیٹا->state.http_neg.allowed = CURL_HTTP_V1x;
    }
  }
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  اگر(conn->http_proxy.creds &&
     ((ڈیٹا->req.httpcode == 407) ||
      (ڈیٹا->req.authneg && ڈیٹا->req.httpcode < 300))) {
    pickproxy = pickoneauth(&ڈیٹا->state.authproxy,
                            authmask & ~CURLAUTH_BEARER,
                            conn->http_proxy.creds);
    اگر(!pickproxy)
      ڈیٹا->state.authproblem = TRUE;
    ورنہ
      ڈیٹا->info.proxyauthpicked = ڈیٹا->state.authproxy.picked;
  }
#اگر_ختم

  اگر(pickhost || pickproxy) {
    نتیجہ = http_perhapsrewind(ڈیٹا, conn);
    اگر(نتیجہ)
      واپس نتیجہ;

    /* In case this is GSS auth, the newurl field is already allocated so
       we must make sure to free it before allocating a new one. As figured
       out in bug #2284386 */
    curlx_free(ڈیٹا->req.newurl);
    /* clone URL */
    ڈیٹا->req.newurl = Curl_bufref_dup(&ڈیٹا->state.url);
    اگر(!ڈیٹا->req.newurl)
      واپس CURLE_برآمد_کا_حافظہ;
  }
  ورنہ اگر((ڈیٹا->req.httpcode < 300) &&
          !ڈیٹا->state.authhost.مکمل &&
          ڈیٹا->req.authneg &&
          /* no (known) authentication available,
             authentication is not "done" yet and
             no authentication seems to be required and
             we did not try HEAD or GET */
          (ڈیٹا->state.httpreq != HTTPREQ_GET) &&
          (ڈیٹا->state.httpreq != HTTPREQ_سر)) {
    /* clone URL */
    ڈیٹا->req.newurl = Curl_bufref_dup(&ڈیٹا->state.url);
    اگر(!ڈیٹا->req.newurl)
      واپس CURLE_برآمد_کا_حافظہ;
    ڈیٹا->state.authhost.مکمل = TRUE;
  }
  اگر(http_should_fail(ڈیٹا, ڈیٹا->req.httpcode)) {
    failf(ڈیٹا, "The requested URL returned error: %d",
          ڈیٹا->req.httpcode);
    نتیجہ = CURLE_HTTP_RETURNED_ERROR;
  }

  واپس نتیجہ;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP_AUTH
/*
 * Output the correct authentication header depending on the auth type
 * and whether or not it is to a proxy.
 */
ساکن CURLcode output_auth_سرخیاں(ساخت Curl_easy *ڈیٹا,
                                    ساخت connectdata *conn,
                                    ساخت auth *authstatus,
                                    ثابت حرف *request,
                                    ثابت حرف *راستہ,
                                    bool proxy)
{
  ثابت حرف *auth = تہی;
  CURLcode نتیجہ = CURLE_OK;
  (خالی)conn;

#اگر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
  (خالی)request;
  (خالی)راستہ;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_AWS
  اگر((authstatus->picked == CURLAUTH_AWS_SIGV4) && !proxy) {
    /* this method is never for proxy */
    auth = "AWS_SIGV4";
    نتیجہ = Curl_output_aws_sigv4(ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
  ورنہ
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTPSIG
  اگر((authstatus->picked == CURLAUTH_HTTPSIG) && !proxy) {
    /* HTTPSIG uses its own configured key material rather than
       data->state.creds. Do not let unrelated credentials from a
       redirected URL bypass the cross-host auth boundary. */
    اگر(Curl_auth_allowed_مقصد_host(ڈیٹا)) {
      auth = "HTTPSIG";
      نتیجہ = Curl_output_httpsig(ڈیٹا);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
    ورنہ
      authstatus->مکمل = TRUE;
  }
  ورنہ
#اگر_ختم
#اگر_تعریف_شدہ USE_SPNEGO
  اگر(authstatus->picked == CURLAUTH_NEGOTIATE) {
    اگر(
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
      (proxy && !Curl_checkProxyheaders(ڈیٹا, conn,
                                        STRCONST("Proxy-authorization"))) ||
#اگر_ختم
      (!proxy && !Curl_checkheaders(ڈیٹا, STRCONST("Authorization")) &&
       Curl_auth_allowed_مقصد_host(ڈیٹا))) {
      auth = "Negotiate";
      نتیجہ = Curl_output_negotiate(ڈیٹا, conn, proxy);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
    ورنہ
      authstatus->مکمل = TRUE;
  }
  ورنہ
#اگر_ختم
#اگر_تعریف_شدہ USE_NTLM
  اگر(authstatus->picked == CURLAUTH_NTLM) {
    اگر(proxy || Curl_auth_allowed_مقصد_host(ڈیٹا)) {
      auth = "NTLM";
      نتیجہ = Curl_output_ntlm(ڈیٹا, proxy);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
    ورنہ
      authstatus->مکمل = TRUE;
  }
  ورنہ
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
  اگر(authstatus->picked == CURLAUTH_DIGEST) {
    auth = "Digest";
    نتیجہ = Curl_output_digest(ڈیٹا,
                                proxy,
                                (ثابت غیرعلامتی حرف *)request,
                                (ثابت غیرعلامتی حرف *)راستہ);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
  ورنہ
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_BASIC_AUTH
  اگر(authstatus->picked == CURLAUTH_BASIC) {
    /* Basic */
    اگر(
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
       (proxy && conn->http_proxy.creds &&
        Curl_creds_has_user_or_pass(conn->http_proxy.creds) &&
        !Curl_checkProxyheaders(ڈیٹا, conn,
                                STRCONST("Proxy-authorization"))) ||
#اگر_ختم
       (!proxy && ڈیٹا->state.creds &&
        Curl_creds_has_user_or_pass(ڈیٹا->state.creds) &&
        !Curl_checkheaders(ڈیٹا, STRCONST("Authorization")))) {
      auth = "Basic";
      نتیجہ = http_output_basic(ڈیٹا, conn, proxy);
      اگر(نتیجہ)
        واپس نتیجہ;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->مکمل = TRUE;
  }
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_BEARER_AUTH
  اگر(authstatus->picked == CURLAUTH_BEARER) {
    /* Bearer */
    اگر(!proxy && Curl_creds_has_oauth_bearer(ڈیٹا->state.creds) &&
       !Curl_checkheaders(ڈیٹا, STRCONST("Authorization"))) {
      auth = "Bearer";
      نتیجہ = http_output_bearer(ڈیٹا);
      اگر(نتیجہ)
        واپس نتیجہ;
    }

    /* NOTE: this function should set 'done' TRUE, as the other auth
       functions work that way */
    authstatus->مکمل = TRUE;
  }
#اگر_ختم

  اگر(auth) {
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
    اگر(proxy)
      ڈیٹا->info.proxyauthpicked = authstatus->picked;
    ورنہ
      ڈیٹا->info.httpauthpicked = authstatus->picked;
    infof(ڈیٹا, "%s auth using %s with user '%s'",
          proxy ? "Proxy" : "Server", auth,
          proxy ? (conn->http_proxy.creds ?
                   conn->http_proxy.creds->user : "") :
          (ڈیٹا->state.creds ?
           ڈیٹا->state.creds->user : ""));
#ورنہ
    (خالی)proxy;
    infof(ڈیٹا, "Server auth using %s with user '%s'",
          auth, ڈیٹا->state.creds ?
          ڈیٹا->state.creds->user : "");
#اگر_ختم
    authstatus->multipass = !authstatus->مکمل;
  }
  ورنہ {
    authstatus->multipass = FALSE;
    اگر(proxy)
      ڈیٹا->info.proxyauthpicked = 0;
    ورنہ
      ڈیٹا->info.httpauthpicked = 0;
  }

  واپس نتیجہ;
}

CURLcode Curl_http_output_auth(ساخت Curl_easy *ڈیٹا,
                               ساخت connectdata *conn,
                               ثابت حرف *request,
                               Curl_HttpReq httpreq,
                               ثابت حرف *راستہ,
                               ثابت حرف *query,
                               bool is_connect)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت auth *authhost;
  ساخت auth *authproxy;
  ثابت حرف *راستہ_and_query = راستہ;
  حرف *tmp_str = تہی;

  DEBUGASSERT(ڈیٹا);
  authhost = &ڈیٹا->state.authhost;
  authproxy = &ڈیٹا->state.authproxy;

  اگر(
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
    (!conn->http_proxy.peer || !conn->http_proxy.creds) &&
#اگر_ختم
#اگر_تعریف_شدہ USE_SPNEGO
    !(authhost->want & CURLAUTH_NEGOTIATE) &&
    !(authproxy->want & CURLAUTH_NEGOTIATE) &&
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTPSIG
    !(authhost->want & CURLAUTH_HTTPSIG) &&
#اگر_ختم
    !ڈیٹا->state.creds) {
    /* no authentication with no user or password */
    authhost->مکمل = TRUE;
    authproxy->مکمل = TRUE;
    نتیجہ = CURLE_OK;
    جاؤ برآمد;
  }

  اگر(query) {
    tmp_str = curl_maprintf("%s?%s", راستہ, query);
    اگر(!tmp_str) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
    راستہ_and_query = tmp_str;
  }

  اگر(authhost->want && !authhost->picked)
    /* The app has selected one or more methods, but none has been picked
       so far by a server round-trip. Then we set the picked one to the
       want one, and if this is one single bit it will be used instantly. */
    authhost->picked = authhost->want;

  اگر(authproxy->want && !authproxy->picked)
    /* The app has selected one or more methods, but none has been picked so
       far by a proxy round-trip. Then we set the picked one to the want one,
       and if this is one single bit it will be used instantly. */
    authproxy->picked = authproxy->want;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  /* Send proxy authentication header if needed */
  اگر(conn->بٹ.origin_is_proxy || is_connect) {
    نتیجہ = output_auth_سرخیاں(ڈیٹا, conn, authproxy, request,
                                 راستہ_and_query, TRUE);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  ورنہ
#ورنہ
  (خالی)is_connect;
#اگر_ختم /* CURL_DISABLE_PROXY */
    /* we have no proxy so let's pretend we are done authenticating
       with it */
    authproxy->مکمل = TRUE;

  /* Either we have credentials for the origin we talk to or
     performing authentication is allowed here */
  اگر(ڈیٹا->state.creds || Curl_auth_allowed_مقصد_host(ڈیٹا))
    نتیجہ = output_auth_سرخیاں(ڈیٹا, conn, authhost, request,
                                 راستہ_and_query, FALSE);
  ورنہ
    authhost->مکمل = TRUE;

  اگر(((authhost->multipass && !authhost->مکمل) ||
      (authproxy->multipass && !authproxy->مکمل)) &&
     (httpreq != HTTPREQ_GET) &&
     (httpreq != HTTPREQ_سر)) {
    /* Auth is required and we are not authenticated yet. Make a PUT or POST
       with content-length zero as a "probe". */
    ڈیٹا->req.authneg = TRUE;
  }
  ورنہ
    ڈیٹا->req.authneg = FALSE;

برآمد:
  curlx_free(tmp_str);
  واپس نتیجہ;
}

#ورنہ /* !CURL_DISABLE_HTTP_AUTH */
/* when disabled */
CURLcode Curl_http_output_auth(ساخت Curl_easy *ڈیٹا,
                               ساخت connectdata *conn,
                               ثابت حرف *request,
                               Curl_HttpReq httpreq,
                               ثابت حرف *راستہ,
                               ثابت حرف *query,
                               bool is_connect)
{
  (خالی)ڈیٹا;
  (خالی)conn;
  (خالی)request;
  (خالی)httpreq;
  (خالی)راستہ;
  (خالی)query;
  (خالی)is_connect;
  واپس CURLE_OK;
}
#اگر_ختم /* !CURL_DISABLE_HTTP_AUTH, else */

#اگر defined(USE_SPNEGO) || defined(USE_NTLM) || \
  !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_BASIC_AUTH) || \
  !defined(CURL_DISABLE_BEARER_AUTH)
ساکن bool authcmp(ثابت حرف *auth, ثابت حرف *سطر)
{
  /* the auth string must not have an alnum following */
  size_t n = لڑی_لمبائی(auth);
  واپس curl_strnequal(auth, سطر, n) && !ISALNUM(سطر[n]);
}
#اگر_ختم

#اگر_تعریف_شدہ USE_SPNEGO
ساکن CURLcode auth_spnego(ساخت Curl_easy *ڈیٹا,
                            bool proxy,
                            ثابت حرف *auth,
                            ساخت auth *authp,
                            uint32_t *availp)
{
  اگر((authp->avail & CURLAUTH_NEGOTIATE) || Curl_auth_is_spnego_supported()) {
    *availp |= CURLAUTH_NEGOTIATE;
    authp->avail |= CURLAUTH_NEGOTIATE;

    اگر(authp->picked == CURLAUTH_NEGOTIATE) {
      ساخت connectdata *conn = ڈیٹا->conn;
      CURLcode نتیجہ = Curl_داخلہ_negotiate(ڈیٹا, conn, proxy, auth);
      uint8_t *negstate = proxy ? &conn->proxy_negotiate_state :
        &conn->http_negotiate_state;
      اگر(!نتیجہ) {
        curlx_free(ڈیٹا->req.newurl);
        ڈیٹا->req.newurl = Curl_bufref_dup(&ڈیٹا->state.url);
        اگر(!ڈیٹا->req.newurl)
          واپس CURLE_برآمد_کا_حافظہ;
        ڈیٹا->state.authproblem = FALSE;
        /* we received a GSS auth token and we dealt with it fine */
        *negstate = CURL_NEGO_AUTH_RECV;
      }
      ورنہ
        ڈیٹا->state.authproblem = TRUE;
    }
  }
  واپس CURLE_OK;
}
#اگر_ختم

#اگر_تعریف_شدہ USE_NTLM
ساکن CURLcode auth_ntlm(ساخت Curl_easy *ڈیٹا,
                          bool proxy,
                          ثابت حرف *auth,
                          ساخت auth *authp,
                          uint32_t *availp)
{
  /* NTLM support requires the SSL crypto libs */
  اگر((authp->avail & CURLAUTH_NTLM) || Curl_auth_is_ntlm_supported()) {
    *availp |= CURLAUTH_NTLM;
    authp->avail |= CURLAUTH_NTLM;

    اگر(authp->picked == CURLAUTH_NTLM) {
      /* NTLM authentication is picked and activated */
      CURLcode نتیجہ = Curl_داخلہ_ntlm(ڈیٹا, proxy, auth);
      اگر(!نتیجہ)
        ڈیٹا->state.authproblem = FALSE;
      ورنہ {
        اگر(نتیجہ == CURLE_برآمد_کا_حافظہ)
          واپس نتیجہ;
        infof(ڈیٹا, "NTLM authentication problem, ignoring.");
        ڈیٹا->state.authproblem = TRUE;
      }
    }
  }
  واپس CURLE_OK;
}
#اگر_ختم

#اگر_غیر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
ساکن CURLcode auth_digest(ساخت Curl_easy *ڈیٹا,
                            bool proxy,
                            ثابت حرف *auth,
                            ساخت auth *authp,
                            uint32_t *availp)
{
  اگر(authp->avail & CURLAUTH_DIGEST) {
    *availp |= CURLAUTH_DIGEST;
    infof(ڈیٹا, "Ignoring duplicate digest auth header.");
  }
  ورنہ اگر(Curl_auth_is_digest_supported()) {
    CURLcode نتیجہ;

    *availp |= CURLAUTH_DIGEST;
    authp->avail |= CURLAUTH_DIGEST;

    /* We call this function on input Digest headers even if Digest
     * authentication is not activated yet, as we need to store the
     * incoming data from this header in case we are going to use
     * Digest */
    نتیجہ = Curl_داخلہ_digest(ڈیٹا, proxy, auth);
    اگر(نتیجہ) {
      اگر(نتیجہ == CURLE_برآمد_کا_حافظہ)
        واپس نتیجہ;
      infof(ڈیٹا, "Digest authentication problem, ignoring.");
      ڈیٹا->state.authproblem = TRUE;
    }
  }
  واپس CURLE_OK;
}
#اگر_ختم

#اگر_غیر_تعریف_شدہ CURL_DISABLE_BASIC_AUTH
ساکن CURLcode auth_basic(ساخت Curl_easy *ڈیٹا,
                           ساخت auth *authp,
                           uint32_t *availp)
{
  *availp |= CURLAUTH_BASIC;
  authp->avail |= CURLAUTH_BASIC;
  اگر(authp->picked == CURLAUTH_BASIC) {
    /* We asked for Basic authentication but got a 40X back anyway, which
       means our name+password is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(ڈیٹا, "Basic authentication problem, ignoring.");
    ڈیٹا->state.authproblem = TRUE;
  }
  واپس CURLE_OK;
}
#اگر_ختم

#اگر_غیر_تعریف_شدہ CURL_DISABLE_BEARER_AUTH
ساکن CURLcode auth_bearer(ساخت Curl_easy *ڈیٹا,
                            ساخت auth *authp,
                            uint32_t *availp)
{
  *availp |= CURLAUTH_BEARER;
  authp->avail |= CURLAUTH_BEARER;
  اگر(authp->picked == CURLAUTH_BEARER) {
    /* We asked for Bearer authentication but got a 40X back anyway, which
       means our token is not valid. */
    authp->avail = CURLAUTH_NONE;
    infof(ڈیٹا, "Bearer authentication problem, ignoring.");
    ڈیٹا->state.authproblem = TRUE;
  }
  واپس CURLE_OK;
}
#اگر_ختم

/*
 * Curl_http_input_auth() deals with Proxy-Authenticate: and WWW-Authenticate:
 * headers. They are dealt with both in the transfer.c main loop and in the
 * proxy CONNECT loop.
 *
 * The 'auth' line ends with a NUL byte without CR or LF present.
 */
CURLcode Curl_http_داخلہ_auth(ساخت Curl_easy *ڈیٹا, bool proxy,
                              ثابت حرف *auth) /* the first non-space */
{
  /*
   * This resource requires authentication
   */
#اگر defined(USE_SPNEGO) ||                      \
  defined(USE_NTLM) ||                          \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||         \
  !defined(CURL_DISABLE_BASIC_AUTH) ||          \
  !defined(CURL_DISABLE_BEARER_AUTH)

  uint32_t *availp;
  ساخت auth *authp;
  CURLcode نتیجہ = CURLE_OK;
  DEBUGASSERT(auth);
  DEBUGASSERT(ڈیٹا);

  اگر(proxy) {
    availp = &ڈیٹا->info.proxyauthavail;
    authp = &ڈیٹا->state.authproxy;
  }
  ورنہ {
    availp = &ڈیٹا->info.httpauthavail;
    authp = &ڈیٹا->state.authhost;
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

  جب(*auth) {
#اگر_تعریف_شدہ USE_SPNEGO
    اگر(authcmp("Negotiate", auth))
      نتیجہ = auth_spnego(ڈیٹا, proxy, auth, authp, availp);
#اگر_ختم
#اگر_تعریف_شدہ USE_NTLM
    اگر(!نتیجہ && authcmp("NTLM", auth))
      نتیجہ = auth_ntlm(ڈیٹا, proxy, auth, authp, availp);
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
    اگر(!نتیجہ && authcmp("Digest", auth))
      نتیجہ = auth_digest(ڈیٹا, proxy, auth, authp, availp);
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_BASIC_AUTH
    اگر(!نتیجہ && authcmp("Basic", auth))
      نتیجہ = auth_basic(ڈیٹا, authp, availp);
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_BEARER_AUTH
    اگر(authcmp("Bearer", auth))
      نتیجہ = auth_bearer(ڈیٹا, authp, availp);
#اگر_ختم

    اگر(نتیجہ)
      توڑو;

    /* there may be multiple methods on one line, so keep reading */
    auth = strchr(auth, ',');
    اگر(auth) /* if we are on a comma, skip it */
      auth++;
    ورنہ
      توڑو;
    curlx_str_passblanks(&auth);
  }
  واپس نتیجہ;
#ورنہ
  (خالی)ڈیٹا;
  (خالی)proxy;
  (خالی)auth;
  /* nothing to do when disabled */
  واپس CURLE_OK;
#اگر_ختم
}

ساکن خالی http_switch_مقصد_get(ساخت Curl_easy *ڈیٹا, صحیح رمز)
{
  ثابت حرف *req = CURL_EASY_STR(ڈیٹا, STRING_CUSTOMREQUEST);

  اگر((req || ڈیٹا->state.httpreq != HTTPREQ_GET) &&
     (ڈیٹا->سیٹ.http_follow_منوال == CURLFOLLOW_OBEYCODE)) {
    NOVERBOSE((خالی)رمز);
    infof(ڈیٹا, "Switch to GET because of %d response", رمز);
    ڈیٹا->state.http_ignorecustom = TRUE;
  }
  ورنہ اگر(req && (ڈیٹا->سیٹ.http_follow_منوال != CURLFOLLOW_FIRSTONLY))
    infof(ڈیٹا, "Stick to %s instead of GET", req);

  ڈیٹا->state.httpreq = HTTPREQ_GET;
  Curl_creader_سیٹ_rewind(ڈیٹا, FALSE);
}

#تعریف_کرو HTTPREQ_IS_POST(ڈیٹا)                    \
  ((ڈیٹا)->state.httpreq == HTTPREQ_POST ||      \
   (ڈیٹا)->state.httpreq == HTTPREQ_POST_FORM || \
   (ڈیٹا)->state.httpreq == HTTPREQ_POST_MIME)

ساکن CURLcode سیٹ_auto_referer(ساخت Curl_easy *ڈیٹا)
{
  CURLU *u;
  حرف *referer = تہی;
  CURLUcode uc;

  /* We are asked to automatically set the previous URL as the referer
     when we get the next URL. We pick the ->url field, which may or may
     not be 100% correct */
  Curl_bufref_free(&ڈیٹا->state.referer);

  /* Make a copy of the URL without credentials and fragment */
  u = curl_url();
  اگر(!u)
    واپس CURLE_برآمد_کا_حافظہ;

  uc = curl_url_سیٹ(u, CURLUPART_URL,
                    Curl_bufref_ptr(&ڈیٹا->state.url), 0);
  اگر(!uc)
    uc = curl_url_سیٹ(u, CURLUPART_FRAGMENT, تہی, 0);
  اگر(!uc)
    uc = curl_url_سیٹ(u, CURLUPART_USER, تہی, 0);
  اگر(!uc)
    uc = curl_url_سیٹ(u, CURLUPART_PASSWORD, تہی, 0);
  اگر(!uc)
    uc = curl_url_get(u, CURLUPART_URL, &referer, 0);

  curl_url_cleanup(u);

  اگر(uc || !referer)
    واپس CURLE_برآمد_کا_حافظہ;

  Curl_bufref_سیٹ(&ڈیٹا->state.referer, referer, 0, curl_free);
  واپس CURLE_OK;
}

ساکن CURLcode follow_parse_url(ساخت Curl_easy *ڈیٹا, ثابت حرف *newurl,
                                 followtype type, حرف **follow_urlp,
                                 CURLUcode *ucp)
{
  CURLUcode uc;

  DEBUGASSERT(ڈیٹا->state.uh);
  uc = curl_url_سیٹ(ڈیٹا->state.uh, CURLUPART_URL, newurl, (غیرعلامتی صحیح)
                    ((type == FOLLOW_FAKE) ? CURLU_NON_SUPPORT_SCHEME :
                     ((type == FOLLOW_REDIR) ? CURLU_URLENCODE : 0) |
                     CURLU_ALLOW_SPACE |
                     (ڈیٹا->سیٹ.راستہ_as_is ? CURLU_راستہ_AS_IS : 0)));
  *ucp = uc;
  اگر(uc) {
    اگر((uc == CURLUE_برآمد_کا_حافظہ) || (type != FOLLOW_FAKE)) {
      failf(ڈیٹا, "The redirect target URL could not be parsed: %s",
            curl_url_strerror(uc));
      واپس Curl_uc_مقصد_curlcode(uc);
    }

    /* the URL could not be parsed for some reason, but since this is FAKE
       mode, duplicate the field as-is */
    *follow_urlp = curlx_strdup(newurl);
    اگر(!*follow_urlp)
      واپس CURLE_برآمد_کا_حافظہ;
  }
  ورنہ {
    CURLU *u = curl_url();
    اگر(!u)
      واپس CURLE_برآمد_کا_حافظہ;
    uc = curl_url_سیٹ(u, CURLUPART_URL,
                      Curl_bufref_ptr(&ڈیٹا->state.url),
                      CURLU_URLENCODE | CURLU_ALLOW_SPACE);
    اگر(!uc)
      uc = curl_url_get(ڈیٹا->state.uh, CURLUPART_URL, follow_urlp, 0);
    اگر(uc) {
      curl_url_cleanup(u);
      واپس Curl_uc_مقصد_curlcode(uc);
    }

#اگر_غیر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
    {
      bool same_origin = Curl_url_same_origin(u, ڈیٹا->state.uh);
      curl_url_cleanup(u);
      اگر(!same_origin)
        Curl_auth_digest_cleanup(&ڈیٹا->state.digest);
    }
#ورنہ
    curl_url_cleanup(u);
#اگر_ختم
  }
  واپس CURLE_OK;
}

ساکن CURLcode follow_fake(ساخت Curl_easy *ڈیٹا, حرف *follow_url,
                            CURLUcode uc, bool reachedmax)
{
  /* we are only figuring out the new URL if we would have followed locations
     but now we are done so we can get out! */
  اگر(!uc) {
    CURLU *u = curl_url();
    حرف *nocred = تہی;

    اگر(!u) {
      curlx_free(follow_url);
      واپس CURLE_برآمد_کا_حافظہ;
    }
    اگر(!curl_url_سیٹ(u, CURLUPART_URL, follow_url,
                     CURLU_NON_SUPPORT_SCHEME |
                     (ڈیٹا->سیٹ.راستہ_as_is ? CURLU_راستہ_AS_IS : 0)) &&
       !curl_url_سیٹ(u, CURLUPART_USER, تہی, 0) &&
       !curl_url_سیٹ(u, CURLUPART_PASSWORD, تہی, 0))
      (خالی)curl_url_get(u, CURLUPART_URL, &nocred, CURLU_GET_خالی_خانے);
    curl_url_cleanup(u);
    اگر(nocred) {
      curlx_free(follow_url);
      follow_url = nocred;
    }
  }
  ڈیٹا->info.wouldredirect = follow_url;

  اگر(reachedmax) {
    failf(ڈیٹا, "Maximum (%d) redirects followed", ڈیٹا->سیٹ.maxredirs);
    واپس CURLE_TOO_MANY_REDIRECTS;
  }
  واپس CURLE_OK;
}

ساکن bool follow_switch_get(ساخت Curl_easy *ڈیٹا)
{
  /*
   * We get here when the HTTP code is 300-399 and 401 or 407. We need to
   * perform differently based on exactly what return code there was.
   */
  انتخاب(ڈیٹا->info.httpcode) {
  حالت 301: /* Moved Permanently */
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
    اگر(HTTPREQ_IS_POST(ڈیٹا) && !ڈیٹا->سیٹ.post301) {
      http_switch_مقصد_get(ڈیٹا, 301);
      واپس TRUE;
    }
    توڑو;
  حالت 302: /*ملا*/
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
    اگر(HTTPREQ_IS_POST(ڈیٹا) && !ڈیٹا->سیٹ.post302) {
      http_switch_مقصد_get(ڈیٹا, 302);
      واپس TRUE;
    }
    توڑو;
  حالت 303: /* See Other */
    /* 'See Other' location is not the resource but a substitute for the
     * resource. In this case we switch the method to GET/HEAD, unless the
     * method is POST and the user specified to keep it as POST.
     */
    اگر(!HTTPREQ_IS_POST(ڈیٹا) || !ڈیٹا->سیٹ.post303) {
      http_switch_مقصد_get(ڈیٹا, 303);
      واپس TRUE;
    }
    توڑو;

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
  طےشدہ:
    توڑو;
  }
  واپس FALSE;
}

CURLcode Curl_http_follow(ساخت Curl_easy *ڈیٹا, ثابت حرف *newurl,
                          followtype type)
{
  bool disallowport = FALSE;
  bool reachedmax = FALSE;
  حرف *follow_url = تہی;
  CURLUcode uc;
  CURLcode نتیجہ;
  CURLcode rewind_نتیجہ;
  bool switch_مقصد_get;

  DEBUGASSERT(type != FOLLOW_NONE);

  اگر(type != FOLLOW_FAKE)
    ڈیٹا->state.درخواستیں++; /* count all real follows */
  اگر(type == FOLLOW_REDIR) {
    اگر((ڈیٹا->سیٹ.maxredirs != -1) &&
       (ڈیٹا->state.followlocation >= ڈیٹا->سیٹ.maxredirs)) {
      reachedmax = TRUE;
      type = FOLLOW_FAKE; /* switch to fake to store the would-be-redirected
                             to URL */
    }
    ورنہ {
      ڈیٹا->state.followlocation++; /* count redirect-followings, including
                                       auth reloads */

      اگر(ڈیٹا->سیٹ.http_auto_referer) {
        نتیجہ = سیٹ_auto_referer(ڈیٹا);
        اگر(نتیجہ)
          واپس نتیجہ;
      }
    }
  }

  اگر((type != FOLLOW_RETRY) &&
     (ڈیٹا->req.httpcode != 401) && (ڈیٹا->req.httpcode != 407) &&
     Curl_is_absolute_url(newurl, تہی, 0, FALSE)) {
    /* If this is not redirect due to a 401 or 407 response and an absolute
       URL: do not allow a custom port number */
    disallowport = TRUE;
  }

  نتیجہ = follow_parse_url(ڈیٹا, newurl, type, &follow_url, &uc);
  اگر(نتیجہ)
    واپس نتیجہ;

  DEBUGASSERT(follow_url);

  اگر(type == FOLLOW_FAKE)
    واپس follow_fake(ڈیٹا, follow_url, uc, reachedmax);

  اگر(disallowport)
    ڈیٹا->state.allow_پورٹ = FALSE;

  Curl_bufref_سیٹ(&ڈیٹا->state.url, follow_url, 0, curl_free);
  rewind_نتیجہ = Curl_req_soft_reset(&ڈیٹا->req, ڈیٹا);
  infof(ڈیٹا, "Issue another request to this URL: '%s'", follow_url);
  اگر((ڈیٹا->سیٹ.http_follow_منوال == CURLFOLLOW_FIRSTONLY) &&
     !ڈیٹا->state.http_ignorecustom &&
     CURL_EASY_STR(ڈیٹا, STRING_CUSTOMREQUEST)) {
    ڈیٹا->state.http_ignorecustom = TRUE;
    infof(ڈیٹا, "Drop custom request method for next request");
  }

  switch_مقصد_get = follow_switch_get(ڈیٹا);

  /* When rewind of upload data failed and we are not switching to GET,
   * we need to fail the follow, as we cannot send the data again. */
  اگر(rewind_نتیجہ && !switch_مقصد_get)
    واپس rewind_نتیجہ;

  Curl_pgrs_وقت(ڈیٹا, TIMER_REDIRECT);
  Curl_pgrsResetTransferSizes(ڈیٹا);

  واپس CURLE_OK;
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
  ساخت Curl_str val;
  DEBUGASSERT(hlen);
  DEBUGASSERT(clen);
  DEBUGASSERT(سرخی);
  DEBUGASSERT(content);

  اگر(!curl_strnequal(headerline, سرخی, hlen))
    واپس FALSE; /* does not start with header */

  /* pass the header */
  p = &headerline[hlen];

  اگر(curlx_str_cspn(&p, &val, "\r\n"))
    واپس FALSE;
  curlx_str_trimblanks(&val);

  /* find the content string in the rest of the line */
  اگر(curlx_strlen(&val) >= clen) {
    size_t len;
    p = curlx_str(&val);
    برائے(len = curlx_strlen(&val); len >= clen;) {
      ساخت Curl_str اگلا;
      ثابت حرف *o = p;
      /* after a match there must be a comma, space, newline or NUL byte */
      اگر(curl_strnequal(p, content, clen) &&
         ((p[clen] == ',') || ISBLANK(p[clen]) || ISNEWLINE(p[clen]) ||
          !p[clen]))
        واپس TRUE; /* match! */
      /* advance to the next comma */
      اگر(curlx_str_until(&p, &اگلا, len, ',') ||
         curlx_str_single(&p, ','))
        توڑو; /* no comma, get out */

      /* if there are more dummy commas, move over them as well */
      کرو
        curlx_str_passblanks(&p);
      جب(!curlx_str_single(&p, ','));
      /* trailing blanks may move the parsing point past the value end,
         then there is nothing left to match */
      اگر((size_t)(p - o) > len)
        توڑو;
      len -= (p - o);
    }
  }
  واپس FALSE; /* no match */
}

ساخت cr_exp100_ctx {
  ساخت Curl_creader super;
  ساخت curltime آغاز; /* time started waiting */
  شمار expect100 state;
};

/* Expect: 100-continue client reader, blocking uploads */

ساکن خالی http_exp100_continue(ساخت Curl_easy *ڈیٹا,
                                 ساخت Curl_creader *reader)
{
  ساخت cr_exp100_ctx *ctx = reader->ctx;
  اگر(ctx->state > EXP100_SEND_ڈیٹا) {
    ctx->state = EXP100_SEND_ڈیٹا;
    Curl_expire_clear(ڈیٹا, EXPIRE_100_TIMEOUT);
  }
}

ساکن CURLcode cr_exp100_read(ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_creader *reader,
                               حرف *buf, size_t blen,
                               size_t *nread, bool *eos)
{
  ساخت cr_exp100_ctx *ctx = reader->ctx;
  timediff_t ms;

  انتخاب(ctx->state) {
  حالت EXP100_SENDING_REQUEST:
    اگر(!Curl_req_sendbuf_خالی_خانے(ڈیٹا)) {
      /* The initial request data has not been fully sent yet. Do
       * not start the timer yet. */
      DEBUGF(infof(ڈیٹا, "cr_exp100_read, request not full sent yet"));
      *nread = 0;
      *eos = FALSE;
      واپس CURLE_OK;
    }
    /* We are now waiting for a reply from the server or
     * a timeout on our side IFF the request has been fully sent. */
    DEBUGF(infof(ڈیٹا, "cr_exp100_read, start AWAITING_CONTINUE, "
                 "timeout %dms", ڈیٹا->سیٹ.expect_100_timeout));
    ctx->state = EXP100_AWAITING_CONTINUE;
    ctx->آغاز = *Curl_pgrs_now(ڈیٹا);
    Curl_expire_سیٹ(ڈیٹا, EXPIRE_100_TIMEOUT,
                    ڈیٹا->سیٹ.expect_100_timeout, &ctx->آغاز);
    *nread = 0;
    *eos = FALSE;
    واپس CURLE_OK;
  حالت EXP100_FAILED:
    DEBUGF(infof(ڈیٹا, "cr_exp100_read, expectation failed, error"));
    *nread = 0;
    *eos = FALSE;
    واپس CURLE_READ_ERROR;
  حالت EXP100_AWAITING_CONTINUE:
    ms = curlx_ptimediff_ms(Curl_pgrs_now(ڈیٹا), &ctx->آغاز);
    اگر(ms < ڈیٹا->سیٹ.expect_100_timeout) {
      DEBUGF(infof(ڈیٹا, "cr_exp100_read, AWAITING_CONTINUE, not expired"));
      *nread = 0;
      *eos = FALSE;
      واپس CURLE_OK;
    }
    /* we have waited long enough, continue anyway */
    http_exp100_continue(ڈیٹا, reader);
    infof(ڈیٹا, "Done waiting for 100-continue");
    FALLTHROUGH();
  طےشدہ:
    DEBUGF(infof(ڈیٹا, "cr_exp100_read, pass through"));
    واپس Curl_creader_read(ڈیٹا, reader->اگلا, buf, blen, nread, eos);
  }
}

ساکن خالی cr_exp100_مکمل(ساخت Curl_easy *ڈیٹا,
                           ساخت Curl_creader *reader, صحیح premature)
{
  ساخت cr_exp100_ctx *ctx = reader->ctx;
  ctx->state = premature ? EXP100_FAILED : EXP100_SEND_ڈیٹا;
  Curl_expire_clear(ڈیٹا, EXPIRE_100_TIMEOUT);
}

ساکن ثابت ساخت Curl_crtype cr_exp100 = {
  "cr-exp100",
  Curl_creader_def_init,
  cr_exp100_read,
  Curl_creader_def_close,
  Curl_creader_def_needs_rewind,
  Curl_creader_def_کل_لمبائی,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  cr_exp100_مکمل,
  حجم(ساخت cr_exp100_ctx)
};

ساکن CURLcode http_exp100_جوڑو_reader(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_creader *reader = تہی;
  CURLcode نتیجہ;

  نتیجہ = Curl_creader_create(&reader, ڈیٹا, &cr_exp100, CURL_CR_PROTOCOL);
  اگر(!نتیجہ)
    نتیجہ = Curl_creader_جوڑو(ڈیٹا, reader);
  اگر(!نتیجہ) {
    ساخت cr_exp100_ctx *ctx = reader->ctx;
    ctx->state = EXP100_SENDING_REQUEST;
  }

  اگر(نتیجہ && reader)
    Curl_creader_free(ڈیٹا, reader);
  واپس نتیجہ;
}

ساکن خالی http_exp100_got100(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_creader *r = Curl_creader_get_by_type(ڈیٹا, &cr_exp100);
  اگر(r)
    http_exp100_continue(ڈیٹا, r);
}

ساکن bool http_exp100_is_انتظار(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_creader *r = Curl_creader_get_by_type(ڈیٹا, &cr_exp100);
  اگر(r) {
    ساخت cr_exp100_ctx *ctx = r->ctx;
    واپس ctx->state == EXP100_AWAITING_CONTINUE;
  }
  واپس FALSE;
}

ساکن خالی http_exp100_send_anyway(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_creader *r = Curl_creader_get_by_type(ڈیٹا, &cr_exp100);
  اگر(r)
    http_exp100_continue(ڈیٹا, r);
}

ساکن bool http_exp100_is_selected(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_creader *r = Curl_creader_get_by_type(ڈیٹا, &cr_exp100);
  واپس !!r;
}

/* this returns the socket to wait for in the DO and DOING state for the multi
   interface and then we are always _sending_ a request and thus we wait for
   the single socket to become writable only */
CURLcode Curl_http_doing_pollset(ساخت Curl_easy *ڈیٹا,
                                 ساخت easy_pollset *ps)
{
  /* write mode */
  واپس Curl_pollset_جوڑو_برآمد(ڈیٹا, ps, ڈیٹا->conn->sock[FIRSTSOCKET]);
}

CURLcode Curl_http_perform_pollset(ساخت Curl_easy *ڈیٹا,
                                   ساخت easy_pollset *ps)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  CURLcode نتیجہ = CURLE_OK;

  اگر(CURL_REQ_WANT_RECV(ڈیٹا)) {
    نتیجہ = Curl_pollset_جوڑو_اندر(ڈیٹا, ps, conn->sock[FIRSTSOCKET]);
  }

  /* on a "Expect: 100-continue" timed wait, do not poll for outgoing */
  اگر(!نتیجہ && Curl_req_want_send(ڈیٹا) && !http_exp100_is_انتظار(ڈیٹا)) {
    نتیجہ = Curl_pollset_جوڑو_برآمد(ڈیٹا, ps, conn->sock[FIRSTSOCKET]);
  }
  واپس نتیجہ;
}

ساکن CURLcode http_write_سرخی(ساخت Curl_easy *ڈیٹا,
                                  ثابت حرف *hd, size_t hdlen)
{
  CURLcode نتیجہ;
  صحیح writetype;

  /* now, only output this if the header AND body are requested:
   */
  Curl_debug(ڈیٹا, CURLINFO_سرخی_اندر, hd, hdlen);

  writetype = CLIENTWRITE_سرخی |
    ((ڈیٹا->req.httpcode / 100 == 1) ? CLIENTWRITE_1XX : 0);

  نتیجہ = Curl_client_write(ڈیٹا, writetype, hd, hdlen);
  اگر(نتیجہ)
    واپس نتیجہ;

  نتیجہ = Curl_bump_headersize(ڈیٹا, hdlen, FALSE);
  اگر(نتیجہ)
    واپس نتیجہ;

  ڈیٹا->req.deductheadercount = (100 <= ڈیٹا->req.httpcode &&
                                 199 >= ڈیٹا->req.httpcode) ?
    ڈیٹا->req.headerbytecount : 0;
  واپس نتیجہ;
}

/*
 * Curl_http_done() gets called after a single HTTP request has been
 * performed.
 */

CURLcode Curl_http_مکمل(ساخت Curl_easy *ڈیٹا,
                        CURLcode حالت_2, bool premature)
{
  ساخت connectdata *conn = ڈیٹا->conn;

  /* Clear multipass flag. If authentication is not done yet, then it will get
   * a chance to be set back to true when we output the next auth header */
  ڈیٹا->state.authhost.multipass = FALSE;
  ڈیٹا->state.authproxy.multipass = FALSE;

  اگر(curlx_dyn_len(&ڈیٹا->state.headerb)) {
    (خالی)http_write_سرخی(ڈیٹا, curlx_dyn_ptr(&ڈیٹا->state.headerb),
                            curlx_dyn_len(&ڈیٹا->state.headerb));
  }
  curlx_dyn_reset(&ڈیٹا->state.headerb);

  اگر(حالت_2)
    واپس حالت_2;

  اگر(!premature && /* this check is pointless when DONE is called before the
                      entire operation is complete */
     !conn->بٹ.retry &&
     !ڈیٹا->سیٹ.connect_only &&
     (ڈیٹا->req.bytecount +
      ڈیٹا->req.headerbytecount -
      ڈیٹا->req.deductheadercount) <= 0) {
    /* If this connection is not closed to be retried, AND nothing was
       read from the HTTP server (that counts), this cannot be right so we
       return an error here */
    failf(ڈیٹا, "Empty reply from server");
    /* Mark it as closed to avoid the "left intact" message */
    streamclose(conn);
    واپس CURLE_GOT_NOTHING;
  }

  واپس CURLE_OK;
}

/* Determine if we may use HTTP 1.1 for this request. */
ساکن bool http_may_use_1_1(ثابت ساخت Curl_easy *ڈیٹا)
{
  ثابت ساخت connectdata *conn = ڈیٹا->conn;
  /* We have seen a previous response for *this* transfer with 1.0,
   * on another connection or the same one. */
  اگر(ڈیٹا->state.http_neg.rcvd_min == 10)
    واپس FALSE;
  /* We have seen a previous response on *this* connection with 1.0. */
  اگر(conn && conn->httpversion_نظر_آیا == 10)
    واپس FALSE;
  /* We want 1.0 and have seen no previous response on *this* connection
     with a higher version (maybe no response at all yet). */
  اگر(ڈیٹا->state.http_neg.only_10 &&
     (!conn || conn->httpversion_نظر_آیا <= 10))
    واپس FALSE;
  /* We are not restricted to use 1.0 only. */
  واپس !ڈیٹا->state.http_neg.only_10;
}

ساکن غیرعلامتی حرف http_request_نسخہ(ساخت Curl_easy *ڈیٹا)
{
  غیرعلامتی حرف v = Curl_conn_http_نسخہ(ڈیٹا, ڈیٹا->conn);
  اگر(!v) {
    /* No specific HTTP connection filter installed. */
    v = http_may_use_1_1(ڈیٹا) ? 11 : 10;
  }
  واپس v;
}

ساکن ثابت حرف *get_http_string(صحیح httpversion)
{
  انتخاب(httpversion) {
  حالت 30:
    واپس "3";
  حالت 20:
    واپس "2";
  حالت 11:
    واپس "1.1";
  طےشدہ:
    واپس "1.0";
  }
}

CURLcode Curl_جوڑو_custom_سرخیاں(ساخت Curl_easy *ڈیٹا,
                                 bool is_connect, صحیح httpversion,
                                 ساخت dynbuf *req)
{
  ساخت curl_slist *h[2];
  ساخت curl_slist *سرخیاں;
  صحیح numlists = 1; /* by default */
  صحیح i;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  شمار Curl_proxy_use proxy;

  اگر(is_connect)
    proxy = سرخی_CONNECT;
  ورنہ
    proxy = ڈیٹا->conn->بٹ.origin_is_proxy ? سرخی_PROXY : سرخی_SERVER;

  انتخاب(proxy) {
  حالت سرخی_SERVER:
    h[0] = ڈیٹا->سیٹ.سرخیاں;
    توڑو;
  حالت سرخی_PROXY:
    h[0] = ڈیٹا->سیٹ.سرخیاں;
    اگر(ڈیٹا->سیٹ.sep_سرخیاں) {
      h[1] = ڈیٹا->سیٹ.proxyheaders;
      numlists++;
    }
    توڑو;
  حالت سرخی_CONNECT:
    اگر(ڈیٹا->سیٹ.sep_سرخیاں)
      h[0] = ڈیٹا->سیٹ.proxyheaders;
    ورنہ
      h[0] = ڈیٹا->سیٹ.سرخیاں;
    توڑو;
  حالت سرخی_CONNECT_UDP:
    اگر(ڈیٹا->سیٹ.sep_سرخیاں)
      h[0] = ڈیٹا->سیٹ.proxyheaders;
    ورنہ
      h[0] = ڈیٹا->سیٹ.سرخیاں;
    توڑو;
  }
#ورنہ
  (خالی)is_connect;
  h[0] = ڈیٹا->سیٹ.سرخیاں;
#اگر_ختم

  /* loop through one or two lists */
  برائے(i = 0; i < numlists; i++) {
    برائے(سرخیاں = h[i]; سرخیاں; سرخیاں = سرخیاں->اگلا) {
      CURLcode نتیجہ = CURLE_OK;
      bool blankheader = FALSE;
      ساخت Curl_str name;
      ثابت حرف *p = سرخیاں->ڈیٹا;
      ثابت حرف *origp = p;
      size_t hlen = لڑی_لمبائی(origp);

      /* explicitly asked to send header without content is done by a header
         that ends with a semicolon, but there must be no colon present in the
         name */
      اگر(!curlx_str_until(&p, &name, hlen, ';') &&
         !curlx_str_single(&p, ';') &&
         !curlx_str_single(&p, '\0') &&
         !memchr(curlx_str(&name), ':', curlx_strlen(&name)))
        blankheader = TRUE;
      ورنہ {
        p = origp;
        اگر(!curlx_str_until(&p, &name, hlen, ':') &&
           !curlx_str_single(&p, ':')) {
          ساخت Curl_str val;
          curlx_str_untilnl(&p, &val, hlen);
          curlx_str_trimblanks(&val);
          اگر(!curlx_strlen(&val))
            /* no content, do not send this */
            جاری;
        }
        ورنہ
          /* no colon */
          جاری;
      }

      /* a field name is a token and carries no surrounding whitespace, so
         trim the parsed name before matching. Otherwise `Authorization :`
         (space before the colon) slips past the Authorization/Cookie check
         below and gets forwarded to another host on a redirect. */
      curlx_str_trimblanks(&name);

      /* only send this if the contents was non-blank or done special */

      اگر(ڈیٹا->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom
            Host: header as that will produce *two* in the same
            request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      ورنہ اگر(ڈیٹا->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ورنہ اگر(ڈیٹا->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ورنہ اگر(ڈیٹا->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      ورنہ اگر(curlx_str_casecompare(&name, "Connection"))
        /* Connection headers are handled specially */
        ;
      ورنہ اگر((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        /* HTTP/2 does not support chunked requests */
        ;
      ورنہ اگر((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_مقصد_host(ڈیٹا))
        ;
      ورنہ اگر(blankheader) {
        نتیجہ = curlx_dyn_addn(req, curlx_str(&name), curlx_strlen(&name));
        اگر(!نتیجہ)
          نتیجہ = curlx_dyn_addn(req, STRCONST(":\r\n"));
      }
      ورنہ
        نتیجہ = curlx_dyn_addf(req, "%s\r\n", origp);

      اگر(نتیجہ)
        واپس نتیجہ;
    }
  }

  واپس CURLE_OK;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PARSEDATE
CURLcode Curl_جوڑو_timecondition(ساخت Curl_easy *ڈیٹا,
                                ساخت dynbuf *req)
{
  ثابت ساخت tm *tm;
  ساخت tm keeptime;
  CURLcode نتیجہ;
  حرف datestr[80];
  ثابت حرف *condp;
  size_t len;

  اگر(ڈیٹا->سیٹ.timecondition == CURL_TIMECOND_NONE)
    /* no condition was asked for */
    واپس CURLE_OK;

  نتیجہ = curlx_gmtime(ڈیٹا->سیٹ.timevalue, &keeptime);
  اگر(نتیجہ) {
    failf(ڈیٹا, "Invalid TIMEVALUE");
    واپس نتیجہ;
  }
  tm = &keeptime;

  انتخاب(ڈیٹا->سیٹ.timecondition) {
  طےشدہ:
    DEBUGF(infof(ڈیٹا, "invalid time condition"));
    واپس CURLE_BAD_FUNCTION_دلیل;

  حالت CURL_TIMECOND_IFMODSINCE:
    condp = "If-Modified-Since";
    len = 17;
    توڑو;
  حالت CURL_TIMECOND_IFUNMODSINCE:
    condp = "If-Unmodified-Since";
    len = 19;
    توڑو;
  حالت CURL_TIMECOND_LASTMOD:
    condp = "Last-Modified";
    len = 13;
    توڑو;
  }

  اگر(Curl_checkheaders(ڈیٹا, condp, len)) {
    /* A custom header was specified; it will be sent instead. */
    واپس CURLE_OK;
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
  واپس نتیجہ;
}
#ورنہ
/* disabled */
CURLcode Curl_جوڑو_timecondition(ساخت Curl_easy *ڈیٹا,
                                ساخت dynbuf *req)
{
  (خالی)ڈیٹا;
  (خالی)req;
  واپس CURLE_OK;
}
#اگر_ختم

خالی Curl_http_طریقہ(ساخت Curl_easy *ڈیٹا,
                      ثابت حرف **طریقہ, Curl_HttpReq *reqp)
{
  Curl_HttpReq httpreq = (Curl_HttpReq)ڈیٹا->state.httpreq;
  ثابت حرف *request;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_WEBSOCKETS
  اگر(ڈیٹا->conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
    httpreq = HTTPREQ_GET;
  ورنہ
#اگر_ختم
  اگر((ڈیٹا->conn->scheme->protocol & (PROTO_FAMILY_HTTP | CURLPROTO_FTP)) &&
     ڈیٹا->state.upload)
    httpreq = HTTPREQ_PUT;

  /* Now set the 'request' pointer to the proper request string */
  اگر(!ڈیٹا->state.http_ignorecustom &&
     CURL_EASY_STR(ڈیٹا, STRING_CUSTOMREQUEST)) {
    request = CURL_EASY_STR(ڈیٹا, STRING_CUSTOMREQUEST);
  }
  ورنہ {
    اگر(ڈیٹا->req.no_body)
      request = "HEAD";
    ورنہ {
      DEBUGASSERT((httpreq >= HTTPREQ_GET) && (httpreq <= HTTPREQ_سر));
      انتخاب(httpreq) {
      حالت HTTPREQ_POST:
      حالت HTTPREQ_POST_FORM:
      حالت HTTPREQ_POST_MIME:
        request = "POST";
        توڑو;
      حالت HTTPREQ_PUT:
        request = "PUT";
        توڑو;
      طےشدہ: /* this should never happen */
      حالت HTTPREQ_GET:
        request = "GET";
        توڑو;
      حالت HTTPREQ_سر:
        request = "HEAD";
        توڑو;
      }
    }
  }
  *طریقہ = request;
  *reqp = httpreq;
}

ساکن CURLcode http_سیٹ_aptr_host(ساخت Curl_easy *ڈیٹا)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ثابت حرف *ptr = تہی;

  curlx_safefree(ڈیٹا->state.http_host);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_COOKIES
  curlx_safefree(ڈیٹا->req.cookiehost);
#اگر_ختم

  اگر(Curl_peer_equal(ڈیٹا->state.initial_origin, ڈیٹا->state.origin))
    ptr = Curl_checkheaders(ڈیٹا, STRCONST("Host"));

  اگر(ptr) {
#اگر_غیر_تعریف_شدہ CURL_DISABLE_COOKIES
    /* If we have a given custom Host: header, we extract the hostname in
       order to possibly use it for cookie reasons later on. We only allow the
       custom Host: header if this is NOT a redirect, as setting Host: in the
       redirected request is being out on thin ice. Except if the hostname
       is the same as the first one! */
    حرف *cookiehost;
    CURLcode نتیجہ = copy_custom_قدر(ptr, &cookiehost);
    اگر(نتیجہ)
      واپس نتیجہ;
    اگر(!*cookiehost)
      /* ignore empty data */
      curlx_free(cookiehost);
    ورنہ {
      /* If the host begins with '[', we start searching for the port after
         the bracket has been closed */
      اگر(*cookiehost == '[') {
        حرف *closingbracket;
        /* since the 'cookiehost' is an allocated memory area that will be
           freed later we cannot increment the pointer */
        memmove(cookiehost, cookiehost + 1, لڑی_لمبائی(cookiehost) - 1);
        closingbracket = strchr(cookiehost, ']');
        اگر(closingbracket)
          *closingbracket = 0;
      }
      ورنہ {
        صحیح startsearch = 0;
        حرف *colon = strchr(cookiehost + startsearch, ':');
        اگر(colon)
          *colon = 0; /* The host must not include an embedded port number */
      }
      ڈیٹا->req.cookiehost = cookiehost;
    }
#اگر_ختم

    اگر(!curl_strequal("Host:", ptr)) {
      ڈیٹا->state.http_host = curl_maprintf("Host:%s", &ptr[5]);
      اگر(!ڈیٹا->state.http_host)
        واپس CURLE_برآمد_کا_حافظہ;
    }
  }
  ورنہ {
    /* This is the  HTTP Host: header, so we want
     * - for IPv6 origins: "[ipv6-address]" where the IPv6 address is
     *  found in origin->hostname, stripped of zoneid/scopeid.
     * - the (IDN converted) origin->hostname (DNS name or IPv4) otherwise.
     * Note: zoneid/scopeid  only applies to local routing and has no
     * meaning on the remote HTTP server (eg. would confuse it). */
    bool ipv6 = (bool)ڈیٹا->state.origin->ipv6;
    ساخت dynbuf tmp;
    size_t hlen;
    CURLcode نتیجہ;

    curlx_dyn_init(&tmp, DYN_HTTP_REQUEST);
    نتیجہ = curlx_dyn_addn(&tmp, STRCONST("Host: "));
    اگر(!نتیجہ && ipv6)
      نتیجہ = curlx_dyn_addn(&tmp, STRCONST("["));
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_جوڑو(&tmp, ڈیٹا->state.origin->hostname);
    اگر(!نتیجہ && ipv6)
      نتیجہ = curlx_dyn_addn(&tmp, STRCONST("]"));
    اگر(!نتیجہ &&
       ((ڈیٹا->state.origin->پورٹ != ڈیٹا->state.origin->scheme->defport) ||
       (ڈیٹا->state.origin->scheme->family != conn->scheme->family))) {
      نتیجہ = curlx_dyn_addf(&tmp, ":%u", ڈیٹا->state.origin->پورٹ);
    }

    ڈیٹا->state.http_host = نتیجہ ? تہی : curlx_dyn_take(&tmp, &hlen);
    curlx_dyn_free(&tmp);
    واپس نتیجہ;
  }
  واپس CURLE_OK;
}

/*
 * Append the request-target to the HTTP request
 */
ساکن CURLcode http_ہدف(ساخت Curl_easy *ڈیٹا,
                            ساخت dynbuf *r)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *راستہ = ڈیٹا->state.اوپر.راستہ;
  ثابت حرف *query = ڈیٹا->state.اوپر.query;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  ساخت connectdata *conn = ڈیٹا->conn;
#اگر_ختم

  اگر(CURL_EASY_STR(ڈیٹا, STRING_ہدف)) {
    راستہ = CURL_EASY_STR(ڈیٹا, STRING_ہدف);
    query = تہی;
  }

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  اگر(conn->بٹ.origin_is_proxy) {
    /* Using a proxy but does not tunnel through it */

    /* The path sent to the proxy is in fact the entire URL, but if the remote
       host is a IDN-name, we must make sure that the request we produce only
       uses the decoded hostname! */

    /* and no fragment part */
    CURLUcode uc;
    حرف *url;
    CURLU *h = curl_url_dup(ڈیٹا->state.uh);
    اگر(!h)
      واپس CURLE_برآمد_کا_حافظہ;

    اگر(!ڈیٹا->state.origin->ipv6 &&
       (ڈیٹا->state.origin->user_hostname != ڈیٹا->state.origin->hostname)) {
      uc = curl_url_سیٹ(h, CURLUPART_HOST, ڈیٹا->state.origin->hostname, 0);
      اگر(uc) {
        curl_url_cleanup(h);
        واپس CURLE_برآمد_کا_حافظہ;
      }
    }
    uc = curl_url_سیٹ(h, CURLUPART_FRAGMENT, تہی, 0);
    اگر(uc) {
      curl_url_cleanup(h);
      واپس CURLE_برآمد_کا_حافظہ;
    }

    اگر(ڈیٹا->state.origin->scheme == &Curl_scheme_http) {
      /* when getting HTTP, we do not want the userinfo the URL */
      uc = curl_url_سیٹ(h, CURLUPART_USER, تہی, 0);
      اگر(uc) {
        curl_url_cleanup(h);
        واپس CURLE_برآمد_کا_حافظہ;
      }
      uc = curl_url_سیٹ(h, CURLUPART_PASSWORD, تہی, 0);
      اگر(uc) {
        curl_url_cleanup(h);
        واپس CURLE_برآمد_کا_حافظہ;
      }
    }
    ورنہ اگر(ڈیٹا->state.creds && (ڈیٹا->state.creds->ماخذ != CREDS_URL)) {
        /* credentials not from the URL need to be set */
      uc = curl_url_سیٹ(h, CURLUPART_USER,
                        ڈیٹا->state.creds->user, CURLU_URLENCODE);
      اگر(!uc)
        uc = curl_url_سیٹ(h, CURLUPART_PASSWORD,
                          ڈیٹا->state.creds->passwd, CURLU_URLENCODE);
      اگر(uc) {
        curl_url_cleanup(h);
        واپس Curl_uc_مقصد_curlcode(uc);
      }
    }

    /* Extract the URL to use in the request. */
    uc = curl_url_get(h, CURLUPART_URL, &url, CURLU_NO_DEFAULT_پورٹ);
    اگر(uc) {
      curl_url_cleanup(h);
      واپس CURLE_برآمد_کا_حافظہ;
    }

    curl_url_cleanup(h);

    /* target or URL */
    نتیجہ = curlx_dyn_جوڑو(r, CURL_EASY_STR(ڈیٹا, STRING_ہدف) ?
      CURL_EASY_STR(ڈیٹا, STRING_ہدف) : url);
    curlx_free(url);
    اگر(نتیجہ)
      واپس نتیجہ;

    اگر((ڈیٹا->state.origin->scheme == &Curl_scheme_ftp) &&
       ڈیٹا->سیٹ.proxy_transfer_منوال) {
      /* when doing ftp, append ;type=<a|i> if not present */
      size_t len = لڑی_لمبائی(راستہ);
      bool type_present = FALSE;
      اگر((len >= 7) && !memcmp(&راستہ[len - 7], ";type=", 6)) {
        انتخاب(Curl_raw_toupper(راستہ[len - 1])) {
        حالت 'A':
        حالت 'D':
        حالت 'I':
          type_present = TRUE;
          توڑو;
        }
      }
      اگر(!type_present) {
        نتیجہ = curlx_dyn_addf(r, ";type=%c",
                                ڈیٹا->state.prefer_ascii ? 'a' : 'i');
        اگر(نتیجہ)
          واپس نتیجہ;
      }
    }
  }

  ورنہ
#اگر_ختم
  {
    نتیجہ = curlx_dyn_جوڑو(r, راستہ);
    اگر(نتیجہ)
      واپس نتیجہ;
    اگر(query)
      نتیجہ = curlx_dyn_addf(r, "?%s", query);
  }

  واپس نتیجہ;
}

#اگر !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
ساکن CURLcode سیٹ_post_reader(ساخت Curl_easy *ڈیٹا, Curl_HttpReq httpreq)
{
  CURLcode نتیجہ;

  انتخاب(httpreq) {
#اگر_غیر_تعریف_شدہ CURL_DISABLE_MIME
  حالت HTTPREQ_POST_MIME:
    ڈیٹا->state.mimepost = ڈیٹا->سیٹ.mimepostp;
    توڑو;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_FORM_API
  حالت HTTPREQ_POST_FORM:
    /* Convert the form structure into a mime structure, then keep
       the conversion */
    اگر(!ڈیٹا->state.formp) {
      ڈیٹا->state.formp = curlx_calloc(1, حجم(curl_mimepart));
      اگر(!ڈیٹا->state.formp)
        واپس CURLE_برآمد_کا_حافظہ;
      Curl_mime_cleanpart(ڈیٹا->state.formp);
      نتیجہ = Curl_getformdata(ڈیٹا, ڈیٹا->state.formp, ڈیٹا->سیٹ.httppost,
                                ڈیٹا->state.fread_func);
      اگر(نتیجہ) {
        curlx_safefree(ڈیٹا->state.formp);
        واپس نتیجہ;
      }
      ڈیٹا->state.mimepost = ڈیٹا->state.formp;
    }
    توڑو;
#اگر_ختم
  طےشدہ:
    ڈیٹا->state.mimepost = تہی;
    توڑو;
  }

  انتخاب(httpreq) {
  حالت HTTPREQ_POST_FORM:
  حالت HTTPREQ_POST_MIME:
    /* This is form posting using mime data. */
#اگر_غیر_تعریف_شدہ CURL_DISABLE_MIME
    اگر(ڈیٹا->state.mimepost) {
      ثابت حرف *cthdr = Curl_checkheaders(ڈیٹا, STRCONST("Content-Type"));

      /* Read and seek body only. */
      ڈیٹا->state.mimepost->flags |= MIME_BODY_ONLY;

      /* Prepare the mime structure headers & set content type. */

      اگر(cthdr)
        برائے(cthdr += 13; *cthdr == ' '; cthdr++)
          ;
      ورنہ اگر(ڈیٹا->state.mimepost->قسم == MIMEKIND_MULTIPART)
        cthdr = "multipart/form-data";

      curl_mime_سرخیاں(ڈیٹا->state.mimepost, ڈیٹا->سیٹ.سرخیاں, 0);
      نتیجہ = Curl_mime_prepare_سرخیاں(ڈیٹا, ڈیٹا->state.mimepost, cthdr,
                                         تہی, MIMESTRATEGY_FORM);
      اگر(نتیجہ)
        واپس نتیجہ;
      curl_mime_سرخیاں(ڈیٹا->state.mimepost, تہی, 0);
      نتیجہ = Curl_creader_سیٹ_mime(ڈیٹا, ڈیٹا->state.mimepost);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
    ورنہ
#اگر_ختم
    {
      نتیجہ = Curl_creader_سیٹ_null(ڈیٹا);
    }
    ڈیٹا->state.infilesize = Curl_creader_کل_لمبائی(ڈیٹا);
    واپس نتیجہ;

  طےشدہ:
    واپس Curl_creader_سیٹ_null(ڈیٹا);
  }
  /* never reached */
}
#اگر_ختم

ساکن CURLcode سیٹ_reader(ساخت Curl_easy *ڈیٹا, Curl_HttpReq httpreq)
{
  CURLcode نتیجہ = CURLE_OK;
  curl_off_t postsize = ڈیٹا->state.infilesize;

  DEBUGASSERT(ڈیٹا->conn);

  اگر(ڈیٹا->req.authneg) {
    واپس Curl_creader_سیٹ_null(ڈیٹا);
  }

  انتخاب(httpreq) {
  حالت HTTPREQ_PUT: /* Let's PUT the data to the server! */
    واپس postsize ? Curl_creader_سیٹ_fread(ڈیٹا, postsize) :
      Curl_creader_سیٹ_null(ڈیٹا);

#اگر !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  حالت HTTPREQ_POST_FORM:
  حالت HTTPREQ_POST_MIME:
    واپس سیٹ_post_reader(ڈیٹا, httpreq);
#اگر_ختم

  حالت HTTPREQ_POST:
    /* this is the simple POST, using x-www-form-urlencoded style */
    /* the size of the post body */
    اگر(!postsize) {
      نتیجہ = Curl_creader_سیٹ_null(ڈیٹا);
    }
    ورنہ اگر(ڈیٹا->سیٹ.postfields) {
      size_t plen = curlx_sotouz_range(postsize, 0, SIZE_MAX);
      اگر(plen == SIZE_MAX)
        واپس CURLE_برآمد_کا_حافظہ;
      ورنہ اگر(plen)
        نتیجہ = Curl_creader_سیٹ_buf(ڈیٹا, ڈیٹا->سیٹ.postfields, plen);
      ورنہ
        نتیجہ = Curl_creader_سیٹ_null(ڈیٹا);
    }
    ورنہ {
      /* we read the bytes from the callback. In case "chunked" encoding
       * is forced by the application, we disregard `postsize`. This is
       * a backward compatibility decision to earlier versions where
       * chunking disregarded this. See issue #13229. */
      bool chunked = FALSE;
      حرف *ptr = Curl_checkheaders(ڈیٹا, STRCONST("Transfer-Encoding"));
      اگر(ptr) {
        /* Some kind of TE is requested, check if 'chunked' is chosen */
        chunked = Curl_compareheader(ptr, STRCONST("Transfer-Encoding:"),
                                     STRCONST("chunked"));
      }
      نتیجہ = Curl_creader_سیٹ_fread(ڈیٹا, chunked ? -1 : postsize);
    }
    واپس نتیجہ;

  طےشدہ:
    /* HTTP GET/HEAD download, has no body, needs no Content-Length */
    ڈیٹا->state.infilesize = 0;
    واپس Curl_creader_سیٹ_null(ڈیٹا);
  }
  /* not reached */
}

ساکن CURLcode http_resume(ساخت Curl_easy *ڈیٹا, Curl_HttpReq httpreq)
{
  اگر((HTTPREQ_POST == httpreq || HTTPREQ_POST_FORM == httpreq ||
      HTTPREQ_POST_MIME == httpreq || HTTPREQ_PUT == httpreq) &&
     ڈیٹا->state.resume_from) {
    failf(ڈیٹا, "HTTP upload cannot be resumed");
    واپس CURLE_BAD_FUNCTION_دلیل;
  }
  واپس CURLE_OK;
}

ساکن CURLcode http_req_سیٹ_TE(ساخت Curl_easy *ڈیٹا,
                                ساخت dynbuf *req,
                                صحیح httpversion)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *ptr;

  ptr = Curl_checkheaders(ڈیٹا, STRCONST("Transfer-Encoding"));
  اگر(ptr) {
    /* Some kind of TE is requested, check if 'chunked' is chosen */
    ڈیٹا->req.upload_chunky =
      Curl_compareheader(ptr,
                         STRCONST("Transfer-Encoding:"), STRCONST("chunked"));
    اگر(ڈیٹا->req.upload_chunky && (httpversion >= 20)) {
      infof(ڈیٹا, "suppressing chunked transfer encoding on connection "
            "using HTTP version 2 or higher");
      ڈیٹا->req.upload_chunky = FALSE;
    }
  }
  ورنہ {
    curl_off_t req_clen = Curl_creader_کل_لمبائی(ڈیٹا);

    اگر(req_clen < 0) {
      /* indeterminate request content length */
      اگر(httpversion > 10) {
        /* On HTTP/1.1, enable chunked, on HTTP/2 and later we do not
         * need it */
        ڈیٹا->req.upload_chunky = (httpversion < 20);
      }
      ورنہ {
        failf(ڈیٹا, "Chunky upload is not supported by HTTP 1.0");
        واپس CURLE_UPLOAD_FAILED;
      }
    }
    ورنہ {
      /* else, no chunky upload */
      ڈیٹا->req.upload_chunky = FALSE;
    }

    اگر(ڈیٹا->req.upload_chunky)
      نتیجہ = curlx_dyn_جوڑو(req, "Transfer-Encoding: chunked\r\n");
  }
  واپس نتیجہ;
}

ساکن CURLcode addexpect(ساخت Curl_easy *ڈیٹا, ساخت dynbuf *r,
                          صحیح httpversion, bool *announced_exp100)
{
  CURLcode نتیجہ;
  حرف *ptr;

  *announced_exp100 = FALSE;
  /* Avoid Expect: 100-continue if Upgrade: is used */
  اگر(ڈیٹا->req.upgr101 != UPGR101_NONE)
    واپس CURLE_OK;

  /* For really small puts we do not use Expect: headers at all, and for
     the somewhat bigger ones we allow the app to disable it. Make
     sure that the expect100header is always set to the preferred value
     here. */
  ptr = Curl_checkheaders(ڈیٹا, STRCONST("Expect"));
  اگر(ptr) {
    *announced_exp100 =
      Curl_compareheader(ptr, STRCONST("Expect:"), STRCONST("100-continue"));
  }
  ورنہ اگر(!ڈیٹا->state.disableexpect && (httpversion == 11)) {
    /* if not doing HTTP 1.0 or version 2, or disabled explicitly, we add an
       Expect: 100-continue to the headers which actually speeds up post
       operations (as there is one packet coming back from the web server) */
    curl_off_t client_len = Curl_creader_client_لمبائی(ڈیٹا);
    اگر(client_len > EXPECT_100_حد || client_len < 0) {
      نتیجہ = curlx_dyn_addn(r, STRCONST("Expect: 100-continue\r\n"));
      اگر(نتیجہ)
        واپس نتیجہ;
      *announced_exp100 = TRUE;
    }
  }
  واپس CURLE_OK;
}

ساکن CURLcode http_جوڑو_content_hds(ساخت Curl_easy *ڈیٹا,
                                     ساخت dynbuf *r,
                                     صحیح httpversion,
                                     Curl_HttpReq httpreq)
{
  CURLcode نتیجہ = CURLE_OK;
  curl_off_t req_clen;
  bool announced_exp100 = FALSE;

  DEBUGASSERT(ڈیٹا->conn);
  اگر(ڈیٹا->req.upload_chunky) {
    نتیجہ = Curl_httpchunk_جوڑو_reader(ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  /* Get the request body length that has been set up */
  req_clen = Curl_creader_کل_لمبائی(ڈیٹا);
  انتخاب(httpreq) {
  حالت HTTPREQ_PUT:
  حالت HTTPREQ_POST:
#اگر !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  حالت HTTPREQ_POST_FORM:
  حالت HTTPREQ_POST_MIME:
#اگر_ختم
    /* We only set Content-Length and allow a custom Content-Length if
       we do not upload data chunked, as RFC2616 forbids us to set both
       kinds of headers (Transfer-Encoding: chunked and Content-Length).
       We do not override a custom "Content-Length" header, but during
       authentication negotiation that header is suppressed.
     */
    اگر(req_clen >= 0 && !ڈیٹا->req.upload_chunky &&
       (ڈیٹا->req.authneg ||
        !Curl_checkheaders(ڈیٹا, STRCONST("Content-Length")))) {
      /* we allow replacing this header if not during auth negotiation,
         although it is not wise to actually set your own */
      نتیجہ = curlx_dyn_addf(r, "Content-Length: %" FMT_OFF_T "\r\n",
                              req_clen);
    }
    اگر(نتیجہ)
      جاؤ برآمد;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_MIME
    /* Output mime-generated headers. */
    اگر(ڈیٹا->state.mimepost &&
       ((httpreq == HTTPREQ_POST_FORM) || (httpreq == HTTPREQ_POST_MIME))) {
      ساخت curl_slist *hdr;

      برائے(hdr = ڈیٹا->state.mimepost->curlheaders; hdr; hdr = hdr->اگلا) {
        نتیجہ = curlx_dyn_addf(r, "%s\r\n", hdr->ڈیٹا);
        اگر(نتیجہ)
          جاؤ برآمد;
      }
    }
#اگر_ختم
    اگر(httpreq == HTTPREQ_POST &&
       !Curl_checkheaders(ڈیٹا, STRCONST("Content-Type"))) {
      نتیجہ = curlx_dyn_addn(r, STRCONST("Content-Type: application/"
                                          "x-www-form-urlencoded\r\n"));
      اگر(نتیجہ)
        جاؤ برآمد;
    }
    نتیجہ = addexpect(ڈیٹا, r, httpversion, &announced_exp100);
    اگر(نتیجہ)
      جاؤ برآمد;
    توڑو;
  طےشدہ:
    توڑو;
  }

  Curl_pgrs_سیٹ_Upload_سائز(ڈیٹا, req_clen);
  اگر(announced_exp100)
    نتیجہ = http_exp100_جوڑو_reader(ڈیٹا);

برآمد:
  واپس نتیجہ;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_COOKIES

ساکن CURLcode http_cookies(ساخت Curl_easy *ڈیٹا,
                             ساخت dynbuf *r)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *addcookies = تہی;
  bool linecap = FALSE;
  اگر(CURL_EASY_STR(ڈیٹا, STRING_COOKIE) &&
     !Curl_checkheaders(ڈیٹا, STRCONST("Cookie")) &&
     Curl_auth_allowed_مقصد_host(ڈیٹا))
    addcookies = CURL_EASY_STR(ڈیٹا, STRING_COOKIE);

  اگر(ڈیٹا->cookies || addcookies) {
    ساخت Curl_llist list;
    صحیح گنتی = 0;

    اگر(ڈیٹا->cookies && ڈیٹا->state.cookie_engine) {
      bool okay;
      ثابت حرف *host = ڈیٹا->req.cookiehost ?
        ڈیٹا->req.cookiehost : ڈیٹا->state.origin->hostname;
      Curl_share_تالا(ڈیٹا, CURL_تالا_ڈیٹا_COOKIE, CURL_تالا_ACCESS_SINGLE);
      نتیجہ = Curl_cookie_getlist(ڈیٹا, &okay, host, &list);
      اگر(!نتیجہ && okay) {
        ساخت Curl_llist_گرہ *n;
        size_t clen = 8; /* hold the size of the generated Cookie: header */

        /* loop through all cookies that matched */
        برائے(n = Curl_llist_سر(&list); n; n = Curl_گرہ_اگلا(n)) {
          ساخت Cookie *co = Curl_گرہ_elem(n);
          اگر(co->قدر) {
            size_t جوڑو;
            اگر(!گنتی) {
              نتیجہ = curlx_dyn_addn(r, STRCONST("Cookie: "));
              اگر(نتیجہ)
                توڑو;
            }
            جوڑو = لڑی_لمبائی(co->name) + لڑی_لمبائی(co->قدر) + 1;
            اگر(clen + جوڑو >= MAX_COOKIE_سرخی_LEN) {
              infof(ڈیٹا, "Restricted outgoing cookies due to header size, "
                    "'%s' not sent", co->name);
              linecap = TRUE;
              توڑو;
            }
            نتیجہ = curlx_dyn_addf(r, "%s%s=%s", گنتی ? "; " : "",
                                    co->name, co->قدر);
            اگر(نتیجہ)
              توڑو;
            clen += جوڑو + (گنتی ? 2 : 0);
            گنتی++;
          }
        }
        Curl_llist_destroy(&list, تہی);
      }
      Curl_share_unlock(ڈیٹا, CURL_تالا_ڈیٹا_COOKIE);
    }
    اگر(addcookies && !نتیجہ && !linecap) {
      اگر(!گنتی)
        نتیجہ = curlx_dyn_addn(r, STRCONST("Cookie: "));
      اگر(!نتیجہ) {
        نتیجہ = curlx_dyn_addf(r, "%s%s", گنتی ? "; " : "", addcookies);
        گنتی++;
      }
    }
    اگر(گنتی && !نتیجہ)
      نتیجہ = curlx_dyn_addn(r, STRCONST("\r\n"));

    اگر(نتیجہ)
      واپس نتیجہ;
  }
  واپس نتیجہ;
}
#ورنہ
#تعریف_کرو http_cookies(a, b) CURLE_OK
#اگر_ختم

ساکن CURLcode http_range(ساخت Curl_easy *ڈیٹا,
                           Curl_HttpReq httpreq)
{
  اگر(ڈیٹا->state.use_range) {
    /*
     * A range is selected. We use different headers whether we are downloading
     * or uploading and we always let customized headers override our internal
     * ones if any such are specified.
     */
    اگر(((httpreq == HTTPREQ_GET) || (httpreq == HTTPREQ_سر)) &&
       !Curl_checkheaders(ڈیٹا, STRCONST("Range"))) {
      /* if a line like this was already allocated, free the previous one */
      curlx_free(ڈیٹا->state.rangeline);
      ڈیٹا->state.rangeline = curl_maprintf("Range: bytes=%s\r\n",
                                                 ڈیٹا->state.range);
      اگر(!ڈیٹا->state.rangeline)
        واپس CURLE_برآمد_کا_حافظہ;
    }
    ورنہ اگر((httpreq == HTTPREQ_POST || httpreq == HTTPREQ_PUT) &&
            !Curl_checkheaders(ڈیٹا, STRCONST("Content-Range"))) {
      curl_off_t req_clen = Curl_creader_کل_لمبائی(ڈیٹا);
      /* if a line like this was already allocated, free the previous one */
      curlx_free(ڈیٹا->state.rangeline);

      اگر(ڈیٹا->سیٹ.سیٹ_resume_from < 0) {
        /* Upload resume was asked for, but we do not know the size of the
           remote part so we tell the server (and act accordingly) that we
           upload the whole file (again) */
        ڈیٹا->state.rangeline =
          curl_maprintf("Content-Range: bytes 0-%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n", req_clen - 1, req_clen);
      }
      ورنہ اگر(ڈیٹا->state.resume_from) {
        /* This is because "resume" was selected */
        /* Not sure if we want to send this header during authentication
         * negotiation, but test1084 checks for it. In which case we have a
         * "null" client reader installed that gives an unexpected length. */
        curl_off_t کل_len = ڈیٹا->req.authneg ?
                               ڈیٹا->state.infilesize :
                               (ڈیٹا->state.resume_from + req_clen);
        ڈیٹا->state.rangeline =
          curl_maprintf("Content-Range: bytes %s%" FMT_OFF_T "/"
                        "%" FMT_OFF_T "\r\n",
                        ڈیٹا->state.range, کل_len - 1, کل_len);
      }
      ورنہ {
        /* Range was selected and then we pass the incoming range and append
           total size */
        ڈیٹا->state.rangeline =
          curl_maprintf("Content-Range: bytes %s/%" FMT_OFF_T "\r\n",
                        ڈیٹا->state.range, req_clen);
      }
      اگر(!ڈیٹا->state.rangeline)
        واپس CURLE_برآمد_کا_حافظہ;
    }
  }
  واپس CURLE_OK;
}

ساکن CURLcode http_firstwrite(ساخت Curl_easy *ڈیٹا)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت SingleRequest *k = &ڈیٹا->req;

  اگر(ڈیٹا->req.newurl) {
    اگر(conn->بٹ.close) {
      /* Abort after the headers if "follow Location" is set
         and we are set to close anyway. */
      CURL_REQ_CLEAR_RECV(ڈیٹا);
      k->مکمل = TRUE;
      واپس CURLE_OK;
    }
    /* We have a new URL to load, but since we want to be able to reuse this
       connection properly, we read the full response in "ignore more" */
    k->ignorebody = TRUE;
    infof(ڈیٹا, "Ignoring the response-body");
  }
  اگر(ڈیٹا->state.resume_from && !k->content_range &&
     (ڈیٹا->state.httpreq == HTTPREQ_GET) &&
     !k->ignorebody) {

    اگر(k->سائز == ڈیٹا->state.resume_from) {
      /* The resume point is at the end of file, consider this fine even if it
         does not allow resume from here. */
      infof(ڈیٹا, "The entire document is already downloaded");
      streamclose(conn);
      /* Abort download */
      CURL_REQ_CLEAR_RECV(ڈیٹا);
      k->مکمل = TRUE;
      واپس CURLE_OK;
    }

    /* we wanted to resume a download, although the server does not seem to
     * support this and we did this with a GET (if it was not a GET we did a
     * POST or PUT resume) */
    failf(ڈیٹا, "HTTP server does not seem to support "
          "byte ranges. Cannot resume.");
    واپس CURLE_RANGE_ERROR;
  }

  اگر(ڈیٹا->سیٹ.timecondition && !ڈیٹا->state.range &&
     /* A time condition has been set AND no ranges have been requested. This
        seems to be what chapter 13.3.4 of RFC 2616 defines to be the correct
        action for an HTTP/1.1 client */
     !Curl_meets_timecondition(ڈیٹا, k->timeofdoc)) {
    k->مکمل = TRUE;
    /* We are simulating an HTTP 304 from server so we return
       what should have been returned from the server */
    ڈیٹا->info.httpcode = 304;
    infof(ڈیٹا, "Simulate an HTTP 304 response");
    /* we abort the transfer before it is completed == we ruin the
       reuse ability. Close the connection */
    streamclose(conn);
    واپس CURLE_OK;
  } /* we have a time condition */

  واپس CURLE_OK;
}

ساکن CURLcode http_check_new_conn(ساخت Curl_easy *ڈیٹا)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ثابت حرف *info_نسخہ = تہی;
  ثابت حرف *alpn;
  CURLcode نتیجہ;

  alpn = Curl_conn_get_alpn_negotiated(ڈیٹا, conn);
  اگر(alpn && !strcmp("h3", alpn)) {
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
    اگر(!conn->بٹ.origin_is_proxy)
#اگر_ختم
      DEBUGASSERT(Curl_conn_http_نسخہ(ڈیٹا, conn) == 30);
    info_نسخہ = "HTTP/3";
  }
  ورنہ اگر(alpn && !strcmp("h2", alpn)) {
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
    اگر((Curl_conn_http_نسخہ(ڈیٹا, conn) != 20) &&
       conn->بٹ.origin_is_proxy) {
      نتیجہ = Curl_http2_switch(ڈیٹا);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
    ورنہ
#اگر_ختم
    DEBUGASSERT(Curl_conn_http_نسخہ(ڈیٹا, conn) == 20);
    info_نسخہ = "HTTP/2";
  }
  ورنہ {
    /* Check if user wants to use HTTP/2 with clear TCP */
    اگر(Curl_http2_may_switch(ڈیٹا)) {
      DEBUGF(infof(ڈیٹا, "HTTP/2 over clean TCP"));
      نتیجہ = Curl_http2_switch(ڈیٹا);
      اگر(نتیجہ)
        واپس نتیجہ;
      info_نسخہ = "HTTP/2";
      /* There is no ALPN here, but the connection is now definitely h2 */
      conn->httpversion_نظر_آیا = 20;
      Curl_conn_سیٹ_multiplex(conn);
    }
    ورنہ
      info_نسخہ = "HTTP/1.x";
  }

  اگر(info_نسخہ)
    infof(ڈیٹا, "using %s", info_نسخہ);
  واپس CURLE_OK;
}

ساکن CURLcode http_جوڑو_connection_hd(ساخت Curl_easy *ڈیٹا,
                                       ساخت dynbuf *req)
{
  ساخت curl_slist *سر;
  ثابت حرف *sep = "Connection: ";
  CURLcode نتیجہ = CURLE_OK;
  size_t rlen = curlx_dyn_len(req);
  bool skip;

  /* Add the 1st custom "Connection: " header, if there is one */
  برائے(سر = ڈیٹا->سیٹ.سرخیاں; سر; سر = سر->اگلا) {
    اگر(curl_strnequal(سر->ڈیٹا, "Connection", 10) &&
       Curl_headersep(سر->ڈیٹا[10]) &&
       !http_سرخی_is_خالی_خانے(سر->ڈیٹا)) {
      حرف *قدر;
      نتیجہ = copy_custom_قدر(سر->ڈیٹا, &قدر);
      اگر(نتیجہ)
        واپس نتیجہ;
      نتیجہ = curlx_dyn_addf(req, "%s%s", sep, قدر);
      sep = ", ";
      curlx_free(قدر);
      توڑو; /* leave, having added 1st one */
    }
  }

  /* add our internal Connection: header values, if we have any */
  اگر(!نتیجہ && ڈیٹا->state.http_hd_te) {
    نتیجہ = curlx_dyn_addf(req, "%s%s", sep, "TE");
    sep = ", ";
  }
  اگر(!نتیجہ && ڈیٹا->state.http_hd_upgrade) {
    نتیجہ = curlx_dyn_addf(req, "%s%s", sep, "Upgrade");
    sep = ", ";
  }
  اگر(!نتیجہ && ڈیٹا->state.http_hd_h2_settings) {
    نتیجہ = curlx_dyn_addf(req, "%s%s", sep, "HTTP2-Settings");
  }
  اگر(!نتیجہ && (rlen < curlx_dyn_len(req)))
    نتیجہ = curlx_dyn_addn(req, STRCONST("\r\n"));
  اگر(نتیجہ)
    واپس نتیجہ;

  /* Add all user-defined Connection: headers after the first */
  skip = TRUE;
  برائے(سر = ڈیٹا->سیٹ.سرخیاں; سر; سر = سر->اگلا) {
    اگر(curl_strnequal(سر->ڈیٹا, "Connection", 10) &&
       Curl_headersep(سر->ڈیٹا[10]) &&
       !http_سرخی_is_خالی_خانے(سر->ڈیٹا)) {
      اگر(skip) {
        skip = FALSE;
        جاری;
      }
      نتیجہ = curlx_dyn_addf(req, "%s\r\n", سر->ڈیٹا);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
  }

  واپس CURLE_OK;
}

/* Header identifier in order we send them by default */
قسمتعریف شمار {
  H1_HD_REQUEST,
  H1_HD_HOST,
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  H1_HD_PROXY_AUTH,
#اگر_ختم
  H1_HD_AUTH,
  H1_HD_RANGE,
  H1_HD_USER_AGENT,
  H1_HD_ACCEPT,
  H1_HD_TE,
  H1_HD_ACCEPT_ENCODING,
  H1_HD_REFERER,
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  H1_HD_PROXY_CONNECTION,
#اگر_ختم
  H1_HD_TRANSFER_ENCODING,
#اگر_غیر_تعریف_شدہ CURL_DISABLE_ALTSVC
  H1_HD_ALT_مستعمل,
#اگر_ختم
  H1_HD_UPGRADE,
  H1_HD_COOKIES,
  H1_HD_CONDITIONALS,
  H1_HD_CUSTOM,
  H1_HD_CONTENT,
  H1_HD_CONNECTION,
  H1_HD_آخری  /* the last, empty header line */
} http_hd_t;

ساکن CURLcode http_جوڑو_hd(ساخت Curl_easy *ڈیٹا,
                            ساخت dynbuf *req,
                            http_hd_t id,
                            غیرعلامتی حرف httpversion,
                            ثابت حرف *طریقہ,
                            Curl_HttpReq httpreq)
{
  CURLcode نتیجہ = CURLE_OK;
#اگر !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_PROXY) || \
  !defined(CURL_DISABLE_WEBSOCKETS)
  ساخت connectdata *conn = ڈیٹا->conn;
#اگر_ختم
  انتخاب(id) {
  حالت H1_HD_REQUEST:
    /* add the main request stuff */
    /* GET/HEAD/POST/PUT */
    نتیجہ = curlx_dyn_addf(req, "%s ", طریقہ);
    اگر(!نتیجہ)
      نتیجہ = http_ہدف(ڈیٹا, req);
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_addf(req, " HTTP/%s\r\n",
                              get_http_string(httpversion));
    توڑو;

  حالت H1_HD_HOST:
    اگر(ڈیٹا->state.http_host) {
      نتیجہ = curlx_dyn_جوڑو(req, ڈیٹا->state.http_host);
      اگر(!نتیجہ)
        نتیجہ = curlx_dyn_addn(req, STRCONST("\r\n"));
    }
    توڑو;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  حالت H1_HD_PROXY_AUTH:
    اگر(ڈیٹا->req.hd_proxy_auth)
      نتیجہ = curlx_dyn_جوڑو(req, ڈیٹا->req.hd_proxy_auth);
    توڑو;
#اگر_ختم

  حالت H1_HD_AUTH:
    اگر(ڈیٹا->req.hd_auth)
      نتیجہ = curlx_dyn_جوڑو(req, ڈیٹا->req.hd_auth);
    توڑو;

  حالت H1_HD_RANGE:
    اگر(ڈیٹا->state.use_range && ڈیٹا->state.rangeline)
      نتیجہ = curlx_dyn_جوڑو(req, ڈیٹا->state.rangeline);
    توڑو;

  حالت H1_HD_USER_AGENT: {
    ثابت حرف *ua = CURL_EASY_STR(ڈیٹا, STRING_USERAGENT);
    اگر(ua && *ua && !Curl_checkheaders(ڈیٹا, STRCONST("User-Agent")))
      نتیجہ = curlx_dyn_addf(req, "User-Agent: %s\r\n", ua);
    توڑو;
  }

  حالت H1_HD_ACCEPT:
    اگر(!Curl_checkheaders(ڈیٹا, STRCONST("Accept")))
      نتیجہ = curlx_dyn_جوڑو(req, "Accept: */*\r\n");
    توڑو;

  حالت H1_HD_TE:
#اگر_تعریف_شدہ HAVE_LIBZ
    اگر(!Curl_checkheaders(ڈیٹا, STRCONST("TE")) &&
       ڈیٹا->سیٹ.http_transfer_encoding) {
      ڈیٹا->state.http_hd_te = TRUE;
      نتیجہ = curlx_dyn_جوڑو(req, "TE: gzip\r\n");
    }
#اگر_ختم
    توڑو;

  حالت H1_HD_ACCEPT_ENCODING: {
    ثابت حرف *enc = CURL_EASY_STR(ڈیٹا, STRING_ENCODING);
    اگر(enc && !Curl_checkheaders(ڈیٹا, STRCONST("Accept-Encoding")))
      نتیجہ = curlx_dyn_addf(req, "Accept-Encoding: %s\r\n", enc);
    توڑو;
  }

  حالت H1_HD_REFERER:
    اگر(Curl_bufref_ptr(&ڈیٹا->state.referer) &&
       !Curl_checkheaders(ڈیٹا, STRCONST("Referer")))
      نتیجہ = curlx_dyn_addf(req, "Referer: %s\r\n",
                              Curl_bufref_ptr(&ڈیٹا->state.referer));
    توڑو;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  حالت H1_HD_PROXY_CONNECTION:
    اگر(conn->بٹ.origin_is_proxy &&
       !Curl_checkheaders(ڈیٹا, STRCONST("Proxy-Connection")) &&
       !Curl_checkProxyheaders(ڈیٹا, ڈیٹا->conn, STRCONST("Proxy-Connection")))
      نتیجہ = curlx_dyn_جوڑو(req, "Proxy-Connection: Keep-Alive\r\n");
    توڑو;
#اگر_ختم

  حالت H1_HD_TRANSFER_ENCODING:
    نتیجہ = http_req_سیٹ_TE(ڈیٹا, req, httpversion);
    توڑو;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_ALTSVC
  حالت H1_HD_ALT_مستعمل:
    اگر(conn->بٹ.altused && conn->بذریعہ_peer &&
       !Curl_checkheaders(ڈیٹا, STRCONST("Alt-Used")))
      نتیجہ = curlx_dyn_addf(req, "Alt-Used: %s:%u\r\n",
                              conn->بذریعہ_peer->hostname, conn->بذریعہ_peer->پورٹ);
    توڑو;
#اگر_ختم

  حالت H1_HD_UPGRADE:
    اگر(!Curl_conn_is_ssl(ڈیٹا->conn, FIRSTSOCKET) && (httpversion < 20) &&
       (ڈیٹا->state.http_neg.wanted & CURL_HTTP_V2x) &&
       ڈیٹا->state.http_neg.h2_upgrade) {
      /* append HTTP2 upgrade magic stuff to the HTTP request if it is not done
         over SSL */
      نتیجہ = Curl_http2_request_upgrade(req, ڈیٹا);
    }
#اگر_غیر_تعریف_شدہ CURL_DISABLE_WEBSOCKETS
    اگر(!نتیجہ && conn->scheme->protocol & (CURLPROTO_WS | CURLPROTO_WSS))
      نتیجہ = Curl_ws_request(ڈیٹا, req);
#اگر_ختم
    توڑو;

  حالت H1_HD_COOKIES:
    نتیجہ = http_cookies(ڈیٹا, req);
    توڑو;

  حالت H1_HD_CONDITIONALS:
    نتیجہ = Curl_جوڑو_timecondition(ڈیٹا, req);
    توڑو;

  حالت H1_HD_CUSTOM:
    نتیجہ = Curl_جوڑو_custom_سرخیاں(ڈیٹا, FALSE, httpversion, req);
    توڑو;

  حالت H1_HD_CONTENT:
    نتیجہ = http_جوڑو_content_hds(ڈیٹا, req, httpversion, httpreq);
    توڑو;

  حالت H1_HD_CONNECTION: {
    نتیجہ = http_جوڑو_connection_hd(ڈیٹا, req);
    توڑو;
  }

  حالت H1_HD_آخری:
    نتیجہ = curlx_dyn_addn(req, STRCONST("\r\n"));
    توڑو;
  }
  واپس نتیجہ;
}

/*
 * Curl_http() gets called from the generic multi_do() function when an HTTP
 * request is to be performed. This creates and sends a properly constructed
 * HTTP request.
 */
CURLcode Curl_http(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  Curl_HttpReq httpreq;
  ثابت حرف *طریقہ;
  ساخت dynbuf req;
  غیرعلامتی حرف httpversion;
  size_t hd_id;

  /* Always consider the DO phase done after this function call, even if there
     may be parts of the request that are not yet sent, since we can deal with
     the rest of the request in the PERFORM phase. */
  *مکمل = TRUE;
  /* initialize a dynamic send-buffer */
  curlx_dyn_init(&req, DYN_HTTP_REQUEST);
  /* make sure the header buffer is reset - if there are leftovers from a
     previous transfer */
  curlx_dyn_reset(&ڈیٹا->state.headerb);
  ڈیٹا->state.maybe_folded = FALSE;

  اگر(!ڈیٹا->conn->بٹ.reuse) {
    نتیجہ = http_check_new_conn(ڈیٹا);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  /* Add collecting of headers written to client. For a new connection,
   * we might have done that already, but reuse
   * or multiplex needs it here as well. */
  نتیجہ = Curl_سرخیاں_init(ڈیٹا);
  اگر(نتیجہ)
    جاؤ برآمد;

  ڈیٹا->state.http_hd_te = FALSE;
  ڈیٹا->state.http_hd_upgrade = FALSE;
  ڈیٹا->state.http_hd_h2_settings = FALSE;

  /* what kind of request do we need to send? */
  Curl_http_طریقہ(ڈیٹا, &طریقہ, &httpreq);

  /* select host to send */
  نتیجہ = http_سیٹ_aptr_host(ڈیٹا);
  /* setup the authentication headers, how that method and host are known */
  اگر(!نتیجہ)
    نتیجہ = Curl_http_output_auth(ڈیٹا, ڈیٹا->conn, طریقہ, httpreq,
                                   ڈیٹا->state.اوپر.راستہ,
                                   ڈیٹا->state.اوپر.query, FALSE);
  /* Setup input reader, resume information and ranges */
  اگر(!نتیجہ)
    نتیجہ = سیٹ_reader(ڈیٹا, httpreq);
  اگر(!نتیجہ)
    نتیجہ = http_resume(ڈیٹا, httpreq);
  اگر(!نتیجہ)
    نتیجہ = http_range(ڈیٹا, httpreq);
  اگر(نتیجہ)
    جاؤ برآمد;

  httpversion = http_request_نسخہ(ڈیٹا);
  /* Add request line and all headers to `req` */
  برائے(hd_id = 0; hd_id <= H1_HD_آخری; ++hd_id) {
    نتیجہ = http_جوڑو_hd(ڈیٹا, &req, (http_hd_t)hd_id,
                         httpversion, طریقہ, httpreq);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  /* setup variables for the upcoming transfer and send */
  Curl_xfer_تیاری_sendrecv(ڈیٹا, FIRSTSOCKET, -1);
  نتیجہ = Curl_req_send(ڈیٹا, &req, httpversion);

  اگر((httpversion >= 20) && ڈیٹا->req.upload_chunky)
    /* upload_chunky was set above to set up the request in a chunky fashion,
       but is disabled here again to avoid that the chunked encoded version is
       actually used when sending the request body over h2 */
    ڈیٹا->req.upload_chunky = FALSE;

برآمد:
  اگر(نتیجہ == CURLE_TOO_LARGE)
    failf(ڈیٹا, "HTTP request too large");

  curlx_dyn_free(&req);
  واپس نتیجہ;
}

قسمتعریف شمار {
  حالت_UNKNOWN, /* not enough data to tell yet */
  حالت_مکمل, /* a status line was read */
  حالت_BAD /* not a status line */
} statusline;

/* Check a string for a prefix. Check no more than 'len' bytes */
ساکن bool checkprefixmax(ثابت حرف *سابقہ, ثابت حرف *بفر, size_t len)
{
  size_t ch = CURLMIN(لڑی_لمبائی(سابقہ), len);
  واپس curl_strnequal(سابقہ, بفر, ch);
}

/*
 * checkhttpprefix()
 *
 * Returns TRUE if member of the list matches prefix of string
 */
ساکن statusline checkhttpprefix(ساخت Curl_easy *ڈیٹا,
                                  ثابت حرف *s, size_t len)
{
  ساخت curl_slist *سر = ڈیٹا->سیٹ.http200aliases;
  statusline rc = حالت_BAD;
  statusline onmatch = len >= 5 ? حالت_مکمل : حالت_UNKNOWN;

  جب(سر) {
    اگر(checkprefixmax(سر->ڈیٹا, s, len)) {
      rc = onmatch;
      توڑو;
    }
    سر = سر->اگلا;
  }

  اگر((rc != حالت_مکمل) && checkprefixmax("HTTP/", s, len))
    rc = onmatch;

  واپس rc;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_RTSP
ساکن statusline checkrtspprefix(ساخت Curl_easy *ڈیٹا,
                                  ثابت حرف *s, size_t len)
{
  statusline حالت_2 = حالت_BAD;
  statusline onmatch = len >= 5 ? حالت_مکمل : حالت_UNKNOWN;
  (خالی)ڈیٹا;
  اگر(checkprefixmax("RTSP/", s, len))
    حالت_2 = onmatch;

  واپس حالت_2;
}
#اگر_ختم /* CURL_DISABLE_RTSP */

ساکن statusline checkprotoprefix(ساخت Curl_easy *ڈیٹا,
                                   ساخت connectdata *conn,
                                   ثابت حرف *s, size_t len)
{
#اگر_غیر_تعریف_شدہ CURL_DISABLE_RTSP
  اگر(conn->scheme->protocol & CURLPROTO_RTSP)
    واپس checkrtspprefix(ڈیٹا, s, len);
#ورنہ
  (خالی)conn;
#اگر_ختم /* CURL_DISABLE_RTSP */

  واپس checkhttpprefix(ڈیٹا, s, len);
}

/* HTTP header has field name `n` (a string constant) */
#تعریف_کرو HD_IS(hd, hdlen, n) \
  (((hdlen) >= (حجم(n) - 1)) && curl_strnequal(n, hd, حجم(n) - 1))

#تعریف_کرو HD_VAL(hd, hdlen, n) \
  ((((hdlen) >= (حجم(n) - 1)) && (hd) && \
    curl_strnequal(n, hd, حجم(n) - 1)) ? ((hd) + (حجم(n) - 1)) : تہی)

/* HTTP header has field name `n` (a string constant) and contains `v`
 * (a string constant) in its value(s) */
#تعریف_کرو HD_IS_AND_SAYS(hd, hdlen, n, v) \
  (HD_IS(hd, hdlen, n) && \
   ((hdlen) > ((حجم(n) - 1) + (حجم(v) - 1))) && \
   Curl_compareheader(hd, STRCONST(n), STRCONST(v)))

/*
 * http_header_a() parses a single response header starting with A.
 */
ساکن CURLcode http_سرخی_a(ساخت Curl_easy *ڈیٹا,
                              ثابت حرف *hd, size_t hdlen)
{
#اگر_غیر_تعریف_شدہ CURL_DISABLE_ALTSVC
  ثابت حرف *v;
  v = (ڈیٹا->asi &&
       (Curl_xfer_is_secure(ڈیٹا) ||
#اگر_تعریف_شدہ DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_ALTSVC_HTTP")
#ورنہ
        0
#اگر_ختم
         )) ? HD_VAL(hd, hdlen, "Alt-Svc:") : تہی;
  اگر(v) {
    /* the ALPN of the current request */
    ساخت SingleRequest *k = &ڈیٹا->req;
    شمار alpnid id = (k->httpversion == 30) ? ALPN_h3 :
      (k->httpversion == 20) ? ALPN_h2 : ALPN_h1;
    واپس Curl_altsvc_parse(ڈیٹا, ڈیٹا->asi, v, ڈیٹا->state.origin, id);
  }
#ورنہ
  (خالی)ڈیٹا;
  (خالی)hd;
  (خالی)hdlen;
#اگر_ختم
  واپس CURLE_OK;
}

/*
 * http_header_c() parses a single response header starting with C.
 */
ساکن CURLcode http_سرخی_c(ساخت Curl_easy *ڈیٹا,
                              ثابت حرف *hd, size_t hdlen)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت SingleRequest *k = &ڈیٹا->req;
  ثابت حرف *v;

  /* Check for Content-Length: header lines to get size. Browsers insist we
     should accept multiple Content-Length headers and that a comma separated
     list also is fine and then we should accept them all as long as they are
     the same value. Different values trigger error.
   */
  v = (!k->http_bodyless && !ڈیٹا->سیٹ.ignorecl && !k->ignore_cl) ?
    HD_VAL(hd, hdlen, "Content-Length:") : تہی;
  اگر(v) {
    کرو {
      curl_off_t contentlength;
      صحیح offt = curlx_str_numblanks(&v, &contentlength);

      اگر(offt == STRE_حد_سے_تجاوز) {
        /* out of range */
        اگر(ڈیٹا->سیٹ.max_filesize) {
          failf(ڈیٹا, "Maximum file size exceeded");
          واپس CURLE_FILESIZE_EXCEEDED;
        }
        streamclose(conn);
        infof(ڈیٹا, "Overflow Content-Length: value");
        واپس CURLE_OK;
      }
      ورنہ {
        اگر((offt == STRE_OK) &&
           ((k->سائز == -1) || /* not set to something before */
            (k->سائز == contentlength))) { /* or the same value */

          k->سائز = contentlength;
          curlx_str_passblanks(&v);

          /* on a comma, loop and get the next instead */
          اگر(!curlx_str_single(&v, ','))
            جاری;

          اگر(!curlx_str_newline(&v)) {
            k->maxdownload = k->سائز;
            واپس CURLE_OK;
          }
        }
        /* negative, different value or rubbish - bad HTTP */
        failf(ڈیٹا, "Invalid Content-Length: value");
        واپس CURLE_WEIRD_SERVER_جواب;
      }
    } جب(1);
  }
  v = (!k->http_bodyless && CURL_EASY_STR(ڈیٹا, STRING_ENCODING)) ?
    HD_VAL(hd, hdlen, "Content-Encoding:") : تہی;
  اگر(v) {
    /*
     * Process Content-Encoding. Look for the values: identity, gzip, deflate,
     * compress, x-gzip and x-compress. x-gzip and x-compress are the same as
     * gzip and compress. (Sec 3.5 RFC 2616). zlib cannot handle compress.
     * Errors are handled further down when the response body is processed
     */
    واپس Curl_build_unencoding_ڈھیر(ڈیٹا, v, FALSE);
  }
  /* check for Content-Type: header lines to get the MIME-type */
  v = HD_VAL(hd, hdlen, "Content-Type:");
  اگر(v) {
    حرف *contenttype = Curl_copy_سرخی_قدر(hd);
    اگر(!contenttype)
      واپس CURLE_برآمد_کا_حافظہ;
    اگر(!*contenttype)
      /* ignore empty data */
      curlx_free(contenttype);
    ورنہ {
      curlx_free(ڈیٹا->info.contenttype);
      ڈیٹا->info.contenttype = contenttype;
    }
    واپس CURLE_OK;
  }
  اگر((k->httpversion < 20) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "close")) {
    /*
     * [RFC 2616, section 8.1.2.1]
     * "Connection: close" is HTTP/1.1 language and means that
     * the connection will close when this request has been
     * served.
     */
    connclose(conn);
    واپس CURLE_OK;
  }
  اگر((k->httpversion == 10) &&
     HD_IS_AND_SAYS(hd, hdlen, "Connection:", "keep-alive")) {
    /*
     * An HTTP/1.0 reply with the 'Connection: keep-alive' line
     * tells us the connection will be kept alive for our
     * pleasure. Default action for 1.0 is to close.
     *
     * [RFC2068, section 19.7.1] */
    connkeep(conn);
    infof(ڈیٹا, "HTTP/1.0 connection set to keep alive");
    واپس CURLE_OK;
  }
  v = !k->http_bodyless ? HD_VAL(hd, hdlen, "Content-Range:") : تہی;
  اگر(v) {
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
    جب(*ptr && !ISDIGIT(*ptr) && *ptr != '*')
      ptr++;

    /* if it truly stopped on a digit */
    اگر(ISDIGIT(*ptr)) {
      اگر(!curlx_str_نمبر(&ptr, &k->انحراف, CURL_OFF_T_MAX) &&
         (ڈیٹا->state.resume_from == k->انحراف))
        /* we asked for a resume and we got it */
        k->content_range = TRUE;
    }
    ورنہ اگر(k->httpcode < 300)
      ڈیٹا->state.resume_from = 0; /* get everything */
  }
  واپس CURLE_OK;
}

/*
 * http_header_l() parses a single response header starting with L.
 */
ساکن CURLcode http_سرخی_l(ساخت Curl_easy *ڈیٹا,
                              ثابت حرف *hd, size_t hdlen)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت SingleRequest *k = &ڈیٹا->req;
  ثابت حرف *v = (!k->http_bodyless &&
                   (ڈیٹا->سیٹ.timecondition || ڈیٹا->سیٹ.get_filetime)) ?
    HD_VAL(hd, hdlen, "Last-Modified:") : تہی;
  اگر(v) {
    اگر(Curl_getdate_capped(v, &k->timeofdoc))
      k->timeofdoc = 0;
    اگر(ڈیٹا->سیٹ.get_filetime)
      ڈیٹا->info.filetime = k->timeofdoc;
    واپس CURLE_OK;
  }
  اگر(HD_IS(hd, hdlen, "Location:")) {
    /* this is the URL that the server advises us to use instead */
    حرف *location = Curl_copy_سرخی_قدر(hd);
    اگر(!location)
      واپس CURLE_برآمد_کا_حافظہ;
    اگر(!*location ||
       (ڈیٹا->req.location && !strcmp(ڈیٹا->req.location, location))) {
      /* ignore empty header, or exact repeat of a previous one */
      curlx_free(location);
      واپس CURLE_OK;
    }
    ورنہ {
      /* has value and is not an exact repeat */
      اگر(ڈیٹا->req.location) {
        failf(ڈیٹا, "Multiple Location headers");
        curlx_free(location);
        واپس CURLE_WEIRD_SERVER_جواب;
      }
      ڈیٹا->req.location = location;

      اگر((k->httpcode >= 300 && k->httpcode < 400) &&
         ڈیٹا->سیٹ.http_follow_منوال) {
        CURLcode نتیجہ;
        DEBUGASSERT(!ڈیٹا->req.newurl);
        ڈیٹا->req.newurl = curlx_strdup(ڈیٹا->req.location); /* clone */
        اگر(!ڈیٹا->req.newurl)
          واپس CURLE_برآمد_کا_حافظہ;

        /* some cases of POST and PUT etc needs to rewind the data
           stream at this point */
        نتیجہ = http_perhapsrewind(ڈیٹا, conn);
        اگر(نتیجہ)
          واپس نتیجہ;

        /* mark the next request as a followed location: */
        ڈیٹا->state.this_is_a_follow = TRUE;
      }
    }
  }
  واپس CURLE_OK;
}

/*
 * http_header_p() parses a single response header starting with P.
 */
ساکن CURLcode http_سرخی_p(ساخت Curl_easy *ڈیٹا,
                              ثابت حرف *hd, size_t hdlen)
{
  ساخت SingleRequest *k = &ڈیٹا->req;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  ثابت حرف *v = HD_VAL(hd, hdlen, "Proxy-Connection:");
  اگر(v) {
    ساخت connectdata *conn = ڈیٹا->conn;
    اگر((k->httpversion == 10) && conn->http_proxy.peer &&
       HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "keep-alive")) {
      /*
       * When an HTTP/1.0 reply comes when using a proxy, the
       * 'Proxy-Connection: keep-alive' line tells us the
       * connection will be kept alive for our pleasure.
       * Default action for 1.0 is to close.
       */
      connkeep(conn); /* do not close */
      infof(ڈیٹا, "HTTP/1.0 proxy connection set to keep alive");
    }
    ورنہ اگر((k->httpversion == 11) && conn->http_proxy.peer &&
            HD_IS_AND_SAYS(hd, hdlen, "Proxy-Connection:", "close")) {
      /*
       * We get an HTTP/1.1 response from a proxy and it says it will
       * close down after this transfer.
       */
      connclose(conn);
      infof(ڈیٹا, "HTTP/1.1 proxy connection set close");
    }
    واپس CURLE_OK;
  }
#اگر_ختم
  اگر((407 == k->httpcode) && HD_IS(hd, hdlen, "Proxy-authenticate:")) {
    حرف *auth;
    CURLcode نتیجہ;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
    اگر(!ڈیٹا->conn->بٹ.origin_is_proxy)
      /* this response is not from a forward proxy, not for us */
      واپس CURLE_OK;
#اگر_ختم
    auth = Curl_copy_سرخی_قدر(hd);
    نتیجہ = auth ? CURLE_OK : CURLE_برآمد_کا_حافظہ;
    اگر(!نتیجہ) {
      نتیجہ = Curl_http_داخلہ_auth(ڈیٹا, TRUE, auth);
      curlx_free(auth);
    }
    واپس نتیجہ;
  }
#اگر_تعریف_شدہ USE_SPNEGO
  اگر(HD_IS(hd, hdlen, "Persistent-Auth:")) {
    ساخت connectdata *conn = ڈیٹا->conn;
    ساخت negotiatedata *negdata = Curl_auth_nego_get(conn, FALSE);
    ساخت auth *authp = &ڈیٹا->state.authhost;
    اگر(!negdata)
      واپس CURLE_برآمد_کا_حافظہ;
    اگر(authp->picked == CURLAUTH_NEGOTIATE) {
      حرف *persistentauth = Curl_copy_سرخی_قدر(hd);
      اگر(!persistentauth)
        واپس CURLE_برآمد_کا_حافظہ;
      negdata->noauthpersist = !!checkprefix("false", persistentauth);
      negdata->havenoauthpersist = TRUE;
      infof(ڈیٹا, "Negotiate: noauthpersist -> %d, header part: %s",
            negdata->noauthpersist, persistentauth);
      curlx_free(persistentauth);
    }
  }
#اگر_ختم
  واپس CURLE_OK;
}

/*
 * http_header_r() parses a single response header starting with R.
 */
ساکن CURLcode http_سرخی_r(ساخت Curl_easy *ڈیٹا,
                              ثابت حرف *hd, size_t hdlen)
{
  ثابت حرف *v = HD_VAL(hd, hdlen, "Retry-After:");
  اگر(v) {
    /* Retry-After = HTTP-date / delay-seconds */
    curl_off_t retry_after = 0; /* zero for unknown or "now" */
    time_t date = 0;
    curlx_str_passblanks(&v);

    /* try it as a date first, because a date can otherwise start with and
       get treated as a number */
    اگر(!Curl_getdate_capped(v, &date)) {
      time_t current = وقت(تہی);
      اگر(date >= current)
        /* convert date to number of seconds into the future */
        retry_after = date - current;
    }
    ورنہ
      /* Try it as a decimal number, ignore errors */
      (خالی)curlx_str_نمبر(&v, &retry_after, CURL_OFF_T_MAX);
    /* limit to 6 hours max. this is not documented so that it can be changed
       in the future if necessary. */
    اگر(retry_after > 21600)
      retry_after = 21600;
    ڈیٹا->info.retry_after = retry_after;
  }
  واپس CURLE_OK;
}

/*
 * http_header_s() parses a single response header starting with S.
 */
ساکن CURLcode http_سرخی_s(ساخت Curl_easy *ڈیٹا,
                              ثابت حرف *hd, size_t hdlen)
{
#اگر !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_HSTS)
  ثابت حرف *v;
#ورنہ
  (خالی)ڈیٹا;
  (خالی)hd;
  (خالی)hdlen;
#اگر_ختم

#اگر_غیر_تعریف_شدہ CURL_DISABLE_COOKIES
  v = (ڈیٹا->cookies && ڈیٹا->state.cookie_engine) ?
    HD_VAL(hd, hdlen, "Set-Cookie:") : تہی;
  اگر(v) {
    /* If there is a custom-set Host: name, use it here, or else use
     * real peer hostname. */
    ثابت حرف *host = ڈیٹا->req.cookiehost ?
      ڈیٹا->req.cookiehost : ڈیٹا->state.origin->hostname;
    ثابت غیرعلامتی حرف secure_context = Curl_secure_context(ڈیٹا, host) ?
      COOKIE_SECURE : 0;
    CURLcode نتیجہ;
    Curl_share_تالا(ڈیٹا, CURL_تالا_ڈیٹا_COOKIE, CURL_تالا_ACCESS_SINGLE);
    نتیجہ = Curl_cookie_جوڑو(ڈیٹا, ڈیٹا->cookies, v, host,
                             ڈیٹا->state.اوپر.راستہ,
                             COOKIE_HTTPHEADER | secure_context);
    Curl_share_unlock(ڈیٹا, CURL_تالا_ڈیٹا_COOKIE);
    واپس نتیجہ;
  }
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HSTS
  /* If enabled, the header is incoming and this is over HTTPS */
  v = (ڈیٹا->hsts &&
       (Curl_xfer_is_secure(ڈیٹا) ||
#اگر_تعریف_شدہ DEBUGBUILD
        /* allow debug builds to circumvent the HTTPS restriction */
        getenv("CURL_HSTS_HTTP")
#ورنہ
        0
#اگر_ختم
         )
    ) ? HD_VAL(hd, hdlen, "Strict-Transport-Security:") : تہی;
  اگر(v) {
    CURLcode نتیجہ = Curl_hsts_parse(
      ڈیٹا->hsts, ڈیٹا->state.origin->hostname, v);
    اگر(نتیجہ) {
      اگر(نتیجہ == CURLE_برآمد_کا_حافظہ)
        واپس نتیجہ;
      infof(ڈیٹا, "Illegal STS header skipped");
    }
#اگر_تعریف_شدہ DEBUGBUILD
    ورنہ
      infof(ڈیٹا, "Parsed STS header fine (%zu entries)",
            Curl_llist_گنتی(&ڈیٹا->hsts->list));
#اگر_ختم
  }
#اگر_ختم

  واپس CURLE_OK;
}

/*
 * http_header_t() parses a single response header starting with T.
 */
ساکن CURLcode http_سرخی_t(ساخت Curl_easy *ڈیٹا,
                              ثابت حرف *hd, size_t hdlen)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت SingleRequest *k = &ڈیٹا->req;

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
                   (ڈیٹا->state.httpreq != HTTPREQ_سر) &&
                   (k->httpcode != 304)) ?
    HD_VAL(hd, hdlen, "Transfer-Encoding:") : تہی;
  اگر(v) {
    /* One or more encodings. We check for chunked and/or a compression
       algorithm. */
    CURLcode نتیجہ = Curl_build_unencoding_ڈھیر(ڈیٹا, v, TRUE);
    اگر(نتیجہ)
      واپس نتیجہ;
    اگر(!k->chunk) {
      /* if this is not chunked, only close can signal the end of this
       * transfer as Content-Length is said not to be trusted for
       * transfer-encoding! */
      CURL_TRC_M(ڈیٹا, "HTTP/1.1 transfer-encoding without chunks");
      connclose(conn);
      k->ignore_cl = TRUE;
    }
    واپس CURLE_OK;
  }
  v = HD_VAL(hd, hdlen, "Trailer:");
  اگر(v) {
    ڈیٹا->req.resp_trailer = TRUE;
    واپس CURLE_OK;
  }
  واپس CURLE_OK;
}

/*
 * http_header_w() parses a single response header starting with W.
 */
ساکن CURLcode http_سرخی_w(ساخت Curl_easy *ڈیٹا,
                              ثابت حرف *hd, size_t hdlen)
{
  ساخت SingleRequest *k = &ڈیٹا->req;
  CURLcode نتیجہ = CURLE_OK;

  اگر((401 == k->httpcode) && HD_IS(hd, hdlen, "WWW-Authenticate:")) {
    حرف *auth = Curl_copy_سرخی_قدر(hd);
    اگر(!auth)
      نتیجہ = CURLE_برآمد_کا_حافظہ;
    ورنہ {
      نتیجہ = Curl_http_داخلہ_auth(ڈیٹا, FALSE, auth);
      curlx_free(auth);
    }
  }
  واپس نتیجہ;
}

/*
 * http_header() parses a single response header.
 */
ساکن CURLcode http_سرخی(ساخت Curl_easy *ڈیٹا,
                            ثابت حرف *hd, size_t hdlen)
{
  CURLcode نتیجہ = CURLE_OK;

  انتخاب(hd[0]) {
  حالت 'a':
  حالت 'A':
    نتیجہ = http_سرخی_a(ڈیٹا, hd, hdlen);
    توڑو;
  حالت 'c':
  حالت 'C':
    نتیجہ = http_سرخی_c(ڈیٹا, hd, hdlen);
    توڑو;
  حالت 'l':
  حالت 'L':
    نتیجہ = http_سرخی_l(ڈیٹا, hd, hdlen);
    توڑو;
  حالت 'p':
  حالت 'P':
    نتیجہ = http_سرخی_p(ڈیٹا, hd, hdlen);
    توڑو;
  حالت 'r':
  حالت 'R':
    نتیجہ = http_سرخی_r(ڈیٹا, hd, hdlen);
    توڑو;
  حالت 's':
  حالت 'S':
    نتیجہ = http_سرخی_s(ڈیٹا, hd, hdlen);
    توڑو;
  حالت 't':
  حالت 'T':
    نتیجہ = http_سرخی_t(ڈیٹا, hd, hdlen);
    توڑو;
  حالت 'w':
  حالت 'W':
    نتیجہ = http_سرخی_w(ڈیٹا, hd, hdlen);
    توڑو;
  }

  اگر(!نتیجہ) {
    ساخت connectdata *conn = ڈیٹا->conn;
    اگر(conn->scheme->protocol & CURLPROTO_RTSP)
      نتیجہ = Curl_rtsp_parseheader(ڈیٹا, hd);
  }
  واپس نتیجہ;
}

/*
 * Called after the first HTTP response line (the status line) has been
 * received and parsed.
 */
ساکن CURLcode http_statusline(ساخت Curl_easy *ڈیٹا,
                                ساخت connectdata *conn)
{
  ساخت SingleRequest *k = &ڈیٹا->req;

  انتخاب(k->httpversion) {
  حالت 10:
  حالت 11:
#اگر_تعریف_شدہ USE_HTTP2
  حالت 20:
#اگر_ختم
#اگر_تعریف_شدہ USE_HTTP3
  حالت 30:
#اگر_ختم
    /* no major version switch mid-connection */
    اگر(k->httpversion_بھیجا &&
       (k->httpversion / 10 != k->httpversion_بھیجا / 10)) {
      failf(ڈیٹا, "Version mismatch (from HTTP/%d to HTTP/%d)",
            k->httpversion_بھیجا / 10, k->httpversion / 10);
      واپس CURLE_WEIRD_SERVER_جواب;
    }
    توڑو;
  طےشدہ:
    failf(ڈیٹا, "Unsupported HTTP version (%d.%d) in response",
          k->httpversion / 10, k->httpversion % 10);
    واپس CURLE_UNSUPPORTED_PROTOCOL;
  }

  ڈیٹا->info.httpcode = k->httpcode;
  ڈیٹا->info.httpversion = k->httpversion;
  conn->httpversion_نظر_آیا = k->httpversion;

  اگر(!ڈیٹا->state.http_neg.rcvd_min ||
     ڈیٹا->state.http_neg.rcvd_min > k->httpversion)
    /* store the lowest server version we encounter */
    ڈیٹا->state.http_neg.rcvd_min = k->httpversion;

  /*
   * This code executes as part of processing the header. As a
   * result, it is not totally clear how to interpret the
   * response code yet as that depends on what other headers may
   * be present. 401 and 407 may be errors, but may be OK
   * depending on how authentication is working. Other codes
   * are definitely errors, so give up here.
   */
  اگر(ڈیٹا->state.resume_from && ڈیٹا->state.httpreq == HTTPREQ_GET &&
     k->httpcode == 416) {
    /* "Requested Range Not Satisfiable", proceed and pretend this is no
       error */
    k->ignorebody = TRUE; /* Avoid appending error msg to good data. */
  }

  اگر(k->httpversion == 10) {
    /* Default action for HTTP/1.0 must be to close, unless
       we get one of those fancy headers that tell us the
       server keeps it open for us! */
    infof(ڈیٹا, "HTTP 1.0, assume close after body");
    connclose(conn);
  }

  k->http_bodyless = k->httpcode >= 100 && k->httpcode < 200;
  انتخاب(k->httpcode) {
  حالت 304:
    /* (quote from RFC2616, section 10.3.5): The 304 response
     * MUST NOT contain a message-body, and thus is always
     * terminated by the first empty line after the header
     * fields. */
    اگر(ڈیٹا->سیٹ.timecondition)
      ڈیٹا->info.timecond = TRUE;
    FALLTHROUGH();
  حالت 204:
    /* (quote from RFC2616, section 10.2.5): The server has
     * fulfilled the request but does not need to return an
     * entity-body ... The 204 response MUST NOT include a
     * message-body, and thus is always terminated by the first
     * empty line after the header fields. */
    k->سائز = 0;
    k->maxdownload = 0;
    k->http_bodyless = TRUE;
    توڑو;
  طےشدہ:
    توڑو;
  }
  واپس CURLE_OK;
}

/* Content-Length must be ignored if any Transfer-Encoding is present in the
   response. Refer to RFC 7230 section 3.3.3 and RFC2616 section 4.4. This is
   figured out here after all headers have been received but before the final
   call to the user's header callback, so that a valid content length can be
   retrieved by the user in the final call. */
ساکن CURLcode http_سائز(ساخت Curl_easy *ڈیٹا)
{
  ساخت SingleRequest *k = &ڈیٹا->req;
  اگر(ڈیٹا->req.ignore_cl || k->chunk) {
    k->سائز = k->maxdownload = -1;
  }
  ورنہ اگر(k->سائز != -1) {
    اگر(ڈیٹا->سیٹ.max_filesize &&
       !k->ignorebody &&
       (k->سائز > ڈیٹا->سیٹ.max_filesize)) {
      failf(ڈیٹا, "Maximum file size exceeded");
      واپس CURLE_FILESIZE_EXCEEDED;
    }
    اگر(k->ignorebody)
      infof(ڈیٹا, "setting size while ignoring");
    Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, k->سائز);
    k->maxdownload = k->سائز;
  }
  واپس CURLE_OK;
}

CURLcode Curl_verify_سرخی(ساخت Curl_easy *ڈیٹا,
                            ثابت حرف *hd, size_t hdlen)
{
  ساخت SingleRequest *k = &ڈیٹا->req;
  size_t i;
  bool has_colon = FALSE;
  برائے(i = 0; i < hdlen; i++) {
    انتخاب(hd[i]) {
    حالت '\0':
      /* this is bad, bail out */
      failf(ڈیٹا, "NUL byte in header");
      واپس CURLE_WEIRD_SERVER_جواب;
    حالت '\r':
      اگر(i < hdlen - 2) {
        /* CR may only precede the LF, nothing else */
        failf(ڈیٹا, "Carriage return found in header");
        واپس CURLE_WEIRD_SERVER_جواب;
      }
      توڑو;
    حالت ':':
      has_colon = TRUE;
      توڑو;
    }
  }
  اگر(k->headerline < 2)
    /* the first "header" is the status-line and it has no colon */
    واپس CURLE_OK;
  اگر(((hd[0] == ' ') || (hd[0] == '\t')) && k->headerline > 2)
    /* line folding, cannot happen on line 2 */
    ;
  ورنہ اگر(!has_colon) {
    /* this is bad, bail out */
    failf(ڈیٹا, "Header without colon");
    واپس CURLE_WEIRD_SERVER_جواب;
  }
  واپس CURLE_OK;
}

CURLcode Curl_bump_headersize(ساخت Curl_easy *ڈیٹا,
                              size_t delta,
                              bool connect_only)
{
  size_t bad = 0;
  غیرعلامتی صحیح max = MAX_HTTP_RESP_سرخی_سائز;
  اگر(delta < MAX_HTTP_RESP_سرخی_سائز) {
    ڈیٹا->info.سرخی_سائز += (غیرعلامتی صحیح)delta;
    ڈیٹا->req.allheadercount += (غیرعلامتی صحیح)delta;
    اگر(!connect_only)
      ڈیٹا->req.headerbytecount += (غیرعلامتی صحیح)delta;
    اگر(ڈیٹا->req.allheadercount > max)
      bad = ڈیٹا->req.allheadercount;
    ورنہ اگر(ڈیٹا->info.سرخی_سائز > (max * 20)) {
      bad = ڈیٹا->info.سرخی_سائز;
      max *= 20;
    }
  }
  ورنہ
    bad = ڈیٹا->req.allheadercount + delta;
  اگر(bad) {
    failf(ڈیٹا, "Too large response headers: %zu > %u", bad, max);
    واپس CURLE_RECV_ERROR;
  }
  واپس CURLE_OK;
}

/*
 * Handle a 101 Switching Protocols response. Performs the actual protocol
 * upgrade to HTTP/2 or WebSocket based on what was requested.
 */
ساکن CURLcode http_on_101_upgrade(ساخت Curl_easy *ڈیٹا,
                                    ثابت حرف *buf, size_t blen,
                                    size_t *pconsumed,
                                    bool *conn_بدلا)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت SingleRequest *k = &ڈیٹا->req;

#اگر !defined(USE_NGHTTP2) && defined(CURL_DISABLE_WEBSOCKETS)
  (خالی)buf;
  (خالی)blen;
  (خالی)pconsumed;
#ورنہ
  CURLcode نتیجہ;
  صحیح upgr101_requested = k->upgr101;
#اگر_ختم

  اگر(k->httpversion_بھیجا != 11) {
    /* invalid for other HTTP versions */
    failf(ڈیٹا, "server sent 101 response while not talking HTTP/1.1");
    واپس CURLE_WEIRD_SERVER_جواب;
  }

  /* Whatever the success, upgrade was selected. */
  k->upgr101 = UPGR101_RECEIVED;
  conn->بٹ.upgrade_اندر_پیش_رفت = FALSE;
  *conn_بدلا = TRUE;

  /* To be fully compliant, we would check the "Upgrade:" response header to
   * mention the protocol we requested. */
#اگر_تعریف_شدہ USE_NGHTTP2
  اگر(upgr101_requested == UPGR101_H2) {
    /* Switch to HTTP/2, where we will get more responses. blen bytes in buf
     * are already h2 protocol bytes */
    infof(ڈیٹا, "Received 101, Switching to HTTP/2");
    نتیجہ = Curl_http2_upgrade(ڈیٹا, conn, FIRSTSOCKET, buf, blen);
    اگر(!نتیجہ)
      *pconsumed += blen;
    واپس نتیجہ;
  }
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_WEBSOCKETS
  اگر(upgr101_requested == UPGR101_WS) {
    /* Switch to WebSocket, where we now stream ws frames. blen bytes in buf
     * are already ws protocol bytes */
    infof(ڈیٹا, "Received 101, Switching to WebSocket");
    نتیجہ = Curl_ws_accept(ڈیٹا, buf, blen);
    اگر(!نتیجہ)
      *pconsumed += blen; /* ws accept handled the data */
    واپس نتیجہ;
  }
#اگر_ختم
  /* We silently accept this as the final response. What are we switching to
   * if we did not ask for an Upgrade? Maybe the application provided an
   * `Upgrade: xxx` header? */
  k->سرخی = FALSE;
  واپس CURLE_OK;
}

/*
 * Handle 1xx intermediate HTTP responses. Sets up state for more
 * headers and processes 100-continue and 101 upgrade responses.
 */
ساکن CURLcode http_on_1xx_response(ساخت Curl_easy *ڈیٹا,
                                     ثابت حرف *buf, size_t blen,
                                     size_t *pconsumed,
                                     bool *conn_بدلا)
{
  ساخت SingleRequest *k = &ڈیٹا->req;

  /* "A user agent MAY ignore unexpected 1xx status responses."
   * By default, we expect to get more responses after this one. */
  k->سرخی = TRUE;
  k->headerline = 0; /* restart the header line counter */

  انتخاب(k->httpcode) {
  حالت 100:
    /* We have made an HTTP PUT or POST and this is 1.1-lingo that tells us
     * that the server is OK with this and ready to receive the data. */
    http_exp100_got100(ڈیٹا);
    توڑو;
  حالت 101:
    واپس http_on_101_upgrade(ڈیٹا, buf, blen, pconsumed, conn_بدلا);
  طےشدہ:
    /* The server may send us other 1xx responses, like informative 103. This
     * has no influence on request processing and we expect to receive a
     * final response eventually. */
    توڑو;
  }
  واپس CURLE_OK;
}

#اگر defined(USE_NTLM) || defined(USE_SPNEGO)
/*
 * Check if NTLM or SPNEGO authentication negotiation failed due to
 * connection closure (typically on HTTP/1.0 servers).
 */
ساکن خالی http_check_auth_احاطہ(ساخت Curl_easy *ڈیٹا,
                                    ساخت connectdata *conn)
{
  /* At this point we have some idea about the fate of the connection. If we
     are closing the connection it may result auth failure. */
#اگر_تعریف_شدہ USE_NTLM
  اگر(conn->بٹ.close &&
     (((ڈیٹا->req.httpcode == 401) &&
       (conn->http_ntlm_state == NTLMSTATE_TYPE2)) ||
      ((ڈیٹا->req.httpcode == 407) &&
       (conn->proxy_ntlm_state == NTLMSTATE_TYPE2)))) {
    infof(ڈیٹا, "Connection closure while negotiating auth (HTTP 1.0?)");
    ڈیٹا->state.authproblem = TRUE;
  }
#اگر_ختم
#اگر_تعریف_شدہ USE_SPNEGO
  اگر(conn->بٹ.close &&
    (((ڈیٹا->req.httpcode == 401) &&
      (conn->http_negotiate_state == CURL_NEGO_AUTH_RECV)) ||
     ((ڈیٹا->req.httpcode == 407) &&
      (conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)))) {
    infof(ڈیٹا, "Connection closure while negotiating auth (HTTP 1.0?)");
    ڈیٹا->state.authproblem = TRUE;
  }
  اگر((conn->http_negotiate_state == CURL_NEGO_AUTH_مکمل) &&
     (ڈیٹا->req.httpcode != 401)) {
    conn->http_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
  اگر((conn->proxy_negotiate_state == CURL_NEGO_AUTH_مکمل) &&
     (ڈیٹا->req.httpcode != 407)) {
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_SUCC;
  }
#اگر_ختم
}
#ورنہ
#تعریف_کرو http_check_auth_احاطہ(x, y) /*خالی_خانے*/
#اگر_ختم

/*
 * Handle an error response (>= 300) received while still sending the
 * request body. Deals with 417 Expectation Failed retries, keep-sending
 * on error, and aborting the send.
 */
ساکن CURLcode http_handle_send_error(ساخت Curl_easy *ڈیٹا)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت SingleRequest *k = &ڈیٹا->req;
  CURLcode نتیجہ = CURLE_OK;

  اگر(!ڈیٹا->req.authneg && !conn->بٹ.close &&
     !Curl_creader_will_rewind(ڈیٹا)) {
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

    انتخاب(ڈیٹا->state.httpreq) {
    حالت HTTPREQ_PUT:
    حالت HTTPREQ_POST:
    حالت HTTPREQ_POST_FORM:
    حالت HTTPREQ_POST_MIME:
      /* We got an error response. If this happened before the
       * whole request body has been sent we stop sending and
       * mark the connection for closure after we have read the
       * entire response. */
      اگر(!Curl_req_مکمل_sending(ڈیٹا)) {
        اگر((k->httpcode == 417) && http_exp100_is_selected(ڈیٹا)) {
          /* 417 Expectation Failed - try again without the
             Expect header */
          اگر(!k->writebytecount && http_exp100_is_انتظار(ڈیٹا)) {
            infof(ڈیٹا, "Got HTTP failure 417 while waiting for a 100");
          }
          ورنہ {
            infof(ڈیٹا, "Got HTTP failure 417 while sending data");
            streamclose(conn);
            نتیجہ = http_perhapsrewind(ڈیٹا, conn);
            اگر(نتیجہ)
              واپس نتیجہ;
          }
          ڈیٹا->state.disableexpect = TRUE;
          Curl_req_abort_sending(ڈیٹا);
          DEBUGASSERT(!ڈیٹا->req.newurl);
          ڈیٹا->req.newurl = Curl_bufref_dup(&ڈیٹا->state.url);
          اگر(!ڈیٹا->req.newurl)
            واپس CURLE_برآمد_کا_حافظہ;
        }
        ورنہ اگر(ڈیٹا->سیٹ.http_keep_sending_on_error) {
          infof(ڈیٹا, "HTTP error before end of send, keep sending");
          http_exp100_send_anyway(ڈیٹا);
        }
        ورنہ {
          infof(ڈیٹا, "HTTP error before end of send, stop sending");
          streamclose(conn);
          نتیجہ = Curl_req_abort_sending(ڈیٹا);
          اگر(نتیجہ)
            واپس نتیجہ;
        }
      }
      توڑو;

    طےشدہ: /* default label present to avoid compiler warnings */
      توڑو;
    }
  }

  اگر(Curl_creader_will_rewind(ڈیٹا) && !Curl_req_مکمل_sending(ڈیٹا)) {
    /* We rewind before next send, continue sending now */
    infof(ڈیٹا, "Keep sending data to get tossed away");
    CURL_REQ_سیٹ_SEND(ڈیٹا);
  }
  واپس نتیجہ;
}

ساکن CURLcode http_on_response(ساخت Curl_easy *ڈیٹا,
                                 ثابت حرف *آخری_hd, size_t آخری_hd_len,
                                 ثابت حرف *buf, size_t blen,
                                 size_t *pconsumed)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  CURLcode نتیجہ = CURLE_OK;
  ساخت SingleRequest *k = &ڈیٹا->req;
  bool conn_بدلا = FALSE;

  (خالی)buf; /* not used without HTTP2 enabled */
  *pconsumed = 0;

  اگر(k->upgr101 == UPGR101_RECEIVED) {
    /* supposedly upgraded to http2 now */
    اگر(ڈیٹا->req.httpversion != 20)
      infof(ڈیٹا, "Lying server, not serving HTTP/2");
  }

  اگر(k->httpcode < 200 && آخری_hd) {
    /* Intermediate responses might trigger processing of more responses,
     * write the last header to the client before proceeding. */
    نتیجہ = http_write_سرخی(ڈیٹا, آخری_hd, آخری_hd_len);
    آخری_hd = تہی; /* handled it */
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  اگر(k->httpcode < 100) {
    failf(ڈیٹا, "Unsupported response code in HTTP response");
    نتیجہ = CURLE_UNSUPPORTED_PROTOCOL;
    جاؤ برآمد;
  }
  ورنہ اگر(k->httpcode < 200) {
    نتیجہ = http_on_1xx_response(ڈیٹا, buf, blen, pconsumed, &conn_بدلا);
    جاؤ برآمد;
  }

  /* k->httpcode >= 200, final response */
  k->سرخی = FALSE;
  اگر(conn->بٹ.upgrade_اندر_پیش_رفت) {
    /* Asked for protocol upgrade, but it was not selected */
    conn->بٹ.upgrade_اندر_پیش_رفت = FALSE;
    conn_بدلا = TRUE;
  }

  اگر((k->سائز == -1) && !k->chunk && !conn->بٹ.close &&
     (k->httpversion == 11) &&
     !(conn->scheme->protocol & CURLPROTO_RTSP) &&
     ڈیٹا->state.httpreq != HTTPREQ_سر) {
    /* On HTTP 1.1, when connection is not to get closed, but no
       Content-Length nor Transfer-Encoding chunked have been received,
       according to RFC2616 section 4.4 point 5, we assume that the server
       will close the connection to signal the end of the document. */
    infof(ڈیٹا, "no chunk, no close, no size. Assume close to signal end");
    streamclose(conn);
  }

  http_check_auth_احاطہ(ڈیٹا, conn);

#اگر_غیر_تعریف_شدہ CURL_DISABLE_WEBSOCKETS
  /* All >=200 HTTP status codes are errors when wanting ws */
  اگر(ڈیٹا->req.upgr101 == UPGR101_WS) {
    failf(ڈیٹا, "Refused WebSocket upgrade: %d", k->httpcode);
    نتیجہ = CURLE_HTTP_RETURNED_ERROR;
    جاؤ برآمد;
  }
#اگر_ختم

  /* Check if this response means the transfer errored. */
  اگر(http_should_fail(ڈیٹا, ڈیٹا->req.httpcode)) {
    failf(ڈیٹا, "The requested URL returned error: %d",
          k->httpcode);
    نتیجہ = CURLE_HTTP_RETURNED_ERROR;
    جاؤ برآمد;
  }

  /* Curl_http_auth_act() checks what authentication methods that are
   * available and decides which one (if any) to use. It will set 'newurl' if
   * an auth method was picked. */
  نتیجہ = Curl_http_auth_act(ڈیٹا);
  اگر(نتیجہ)
    جاؤ برآمد;

  اگر(k->httpcode >= 300) {
    نتیجہ = http_handle_send_error(ڈیٹا);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  /* final response without error, prepare to receive the body */
  نتیجہ = http_firstwrite(ڈیٹا);
  اگر(نتیجہ)
    جاؤ برآمد;

  /* This is the last response that we get for the current request. Check on
   * the body size and determine if the response is complete. */
  نتیجہ = http_سائز(ڈیٹا);
  اگر(نتیجہ)
    جاؤ برآمد;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_RTSP
  /* An RTSP response to any method may carry a body, which is announced
     with Content-Length (RFC 2326 section 12.14). Read it instead of
     treating it as excess data, unless the application asked for no body
     with CURLOPT_NOBODY. */
  اگر((conn->scheme->protocol & CURLPROTO_RTSP) && ڈیٹا->req.no_body &&
     !ڈیٹا->سیٹ.opt_no_body && (k->سائز > 0))
    ڈیٹا->req.no_body = FALSE;
#اگر_ختم

  /* If we requested a "no body", this is a good time to get
   * out and return home.
   */
  اگر(ڈیٹا->req.no_body)
    k->download_مکمل = TRUE;

  /* If max download size is *zero* (nothing) we already have nothing and can
     safely return ok now! For HTTP/2, we would like to call
     http2_handle_stream_close to properly close a stream. In order to do
     this, we keep reading until we close the stream. */
  اگر((k->maxdownload == 0) && (k->httpversion_بھیجا < 20))
    k->download_مکمل = TRUE;

برآمد:
  اگر(آخری_hd)
    /* if not written yet, write it now */
    نتیجہ = Curl_1st_fatal(نتیجہ,
                            http_write_سرخی(ڈیٹا, آخری_hd, آخری_hd_len));
  اگر(conn_بدلا)
    /* poke the multi handle to allow pending pipewait to retry */
    Curl_multi_connchanged(ڈیٹا->multi);
  واپس نتیجہ;
}

ساکن CURLcode http_rw_hd(ساخت Curl_easy *ڈیٹا,
                           ثابت حرف *hd, size_t hdlen,
                           ثابت حرف *buf_remain, size_t blen,
                           size_t *pconsumed)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت SingleRequest *k = &ڈیٹا->req;
  صحیح writetype;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  *pconsumed = 0;
  اگر((0x0a == *hd) || (0x0d == *hd)) {
    /* Empty header line means end of headers! */
    ساخت dynbuf آخری_سرخی;
    size_t consumed;

    curlx_dyn_init(&آخری_سرخی, hdlen + 1);
    نتیجہ = curlx_dyn_addn(&آخری_سرخی, hd, hdlen);
    اگر(نتیجہ)
      واپس نتیجہ;

    /* analyze the response to find out what to do. */
    /* Caveat: we clear anything in the header brigade, because a
     * response might switch HTTP version which may call use recursively.
     * Not nice, but that is currently the way of things. */
    curlx_dyn_reset(&ڈیٹا->state.headerb);
    نتیجہ = http_on_response(ڈیٹا, curlx_dyn_ptr(&آخری_سرخی),
                              curlx_dyn_len(&آخری_سرخی),
                              buf_remain, blen, &consumed);
    *pconsumed += consumed;
    curlx_dyn_free(&آخری_سرخی);
    واپس نتیجہ;
  }

  /*
   * Checks for special headers coming up.
   */

  writetype = CLIENTWRITE_سرخی;
  اگر(!k->headerline++) {
    /* This is the first header, it MUST be the error code line
       or else we consider this to be the body right away! */
    bool fine_statusline = FALSE;

    k->httpversion = 0; /* Do not know yet */
    اگر(ڈیٹا->conn->scheme->protocol & PROTO_FAMILY_HTTP) {
      /*
       * https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.2
       *
       * The response code is always a three-digit number in HTTP as the spec
       * says. We allow any three-digit number here, but we cannot make
       * guarantees on future behaviors since it is not within the protocol.
       */
      ثابت حرف *p = hd;

      curlx_str_passblanks(&p);
      اگر(!لڑی_موازنہ(p, "HTTP/", 5)) {
        p += 5;
        انتخاب(*p) {
        حالت '1':
          p++;
          اگر((p[0] == '.') && (p[1] == '0' || p[1] == '1')) {
            اگر(ISBLANK(p[2])) {
              k->httpversion = (غیرعلامتی حرف)(10 + (p[1] - '0'));
              p += 3;
              اگر(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
                k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
                  (p[2] - '0');
                /* RFC 9112 requires a single space following the status code,
                   but the browsers do not so let's not insist */
                fine_statusline = TRUE;
              }
            }
          }
          اگر(!fine_statusline) {
            failf(ڈیٹا, "Unsupported HTTP/1 subversion in response");
            واپس CURLE_UNSUPPORTED_PROTOCOL;
          }
          توڑو;
        حالت '2':
        حالت '3':
          اگر(!ISBLANK(p[1]))
            توڑو;
          k->httpversion = (غیرعلامتی حرف)((*p - '0') * 10);
          p += 2;
          اگر(ISDIGIT(p[0]) && ISDIGIT(p[1]) && ISDIGIT(p[2])) {
            k->httpcode = ((p[0] - '0') * 100) + ((p[1] - '0') * 10) +
              (p[2] - '0');
            p += 3;
            اگر(!ISBLANK(*p))
              توڑو;
            fine_statusline = TRUE;
          }
          توڑو;
        طےشدہ: /* unsupported */
          failf(ڈیٹا, "Unsupported HTTP version in response");
          واپس CURLE_UNSUPPORTED_PROTOCOL;
        }
      }

      اگر(!fine_statusline) {
        /* If user has set option HTTP200ALIASES,
           compare header line against list of aliases */
        statusline check = checkhttpprefix(ڈیٹا, hd, hdlen);
        اگر(check == حالت_مکمل) {
          fine_statusline = TRUE;
          k->httpcode = 200;
          k->httpversion = 10;
        }
      }
    }
    ورنہ اگر(ڈیٹا->conn->scheme->protocol & CURLPROTO_RTSP) {
      ثابت حرف *p = hd;
      ساخت Curl_str ver;
      curl_off_t حالت_2;
      /* we set the max string a little excessive to forgive some leading
         spaces */
      اگر(!curlx_str_until(&p, &ver, 32, ' ') &&
         !curlx_str_single(&p, ' ') &&
         !curlx_str_نمبر(&p, &حالت_2, 999)) {
        curlx_str_trimblanks(&ver);
        اگر(curlx_str_cmp(&ver, "RTSP/1.0")) {
          k->httpcode = (صحیح)حالت_2;
          fine_statusline = TRUE;
          k->httpversion = 11; /* RTSP acts like HTTP 1.1 */
        }
      }
      اگر(!fine_statusline)
        واپس CURLE_WEIRD_SERVER_جواب;
    }

    اگر(fine_statusline) {
      نتیجہ = http_statusline(ڈیٹا, ڈیٹا->conn);
      اگر(نتیجہ)
        واپس نتیجہ;
      writetype |= CLIENTWRITE_حالت;
    }
    ورنہ {
      k->سرخی = FALSE;   /* this is not a header line */
      واپس CURLE_WEIRD_SERVER_جواب;
    }
  }

  نتیجہ = Curl_verify_سرخی(ڈیٹا, hd, hdlen);
  اگر(نتیجہ)
    واپس نتیجہ;

  نتیجہ = http_سرخی(ڈیٹا, hd, hdlen);
  اگر(نتیجہ)
    واپس نتیجہ;

  /*
   * Taken in one (more) header. Write it to the client.
   */
  Curl_debug(ڈیٹا, CURLINFO_سرخی_اندر, hd, hdlen);

  اگر(k->httpcode / 100 == 1)
    writetype |= CLIENTWRITE_1XX;
  نتیجہ = Curl_client_write(ڈیٹا, writetype, hd, hdlen);
  اگر(نتیجہ)
    واپس نتیجہ;

  نتیجہ = Curl_bump_headersize(ڈیٹا, hdlen, FALSE);
  اگر(نتیجہ)
    واپس نتیجہ;

  واپس CURLE_OK;
}

/* remove trailing CRLF then all trailing whitespace */
خالی Curl_http_مقصد_fold(ساخت dynbuf *bf)
{
  size_t len = curlx_dyn_len(bf);
  ثابت حرف *hd = curlx_dyn_ptr(bf);
  اگر(len && (hd[len - 1] == '\n'))
    len--;
  اگر(len && (hd[len - 1] == '\r'))
    len--;
  جب(len && ISBLANK(hd[len - 1])) /* strip off trailing whitespace */
    len--;
  curlx_dyn_setlen(bf, len);
}

ساکن خالی unfold_سرخی(ساخت Curl_easy *ڈیٹا)
{
  Curl_http_مقصد_fold(&ڈیٹا->state.headerb);
  ڈیٹا->state.leading_unfold = TRUE;
}

/*
 * Read any HTTP header lines from the server and pass them to the client app.
 */
ساکن CURLcode http_parse_سرخیاں(ساخت Curl_easy *ڈیٹا,
                                   ثابت حرف *buf, size_t blen,
                                   size_t *pconsumed)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  CURLcode نتیجہ = CURLE_OK;
  ساخت SingleRequest *k = &ڈیٹا->req;
  ثابت حرف *انتہا_ptr;
  bool leftover_body = FALSE;

  /* we have bytes for the next header, make sure it is not a folded header
     before passing it on */
  اگر(ڈیٹا->state.maybe_folded && blen) {
    اگر(ISBLANK(buf[0])) {
      /* folded, remove the trailing newlines and append the next header */
      unfold_سرخی(ڈیٹا);
    }
    ورنہ {
      /* the header data we hold is a complete header, pass it on */
      size_t ignore_this;
      نتیجہ = http_rw_hd(ڈیٹا, curlx_dyn_ptr(&ڈیٹا->state.headerb),
                          curlx_dyn_len(&ڈیٹا->state.headerb),
                          تہی, 0, &ignore_this);
      curlx_dyn_reset(&ڈیٹا->state.headerb);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
    ڈیٹا->state.maybe_folded = FALSE;
  }

  /* header line within buffer loop */
  *pconsumed = 0;
  جب(blen && k->سرخی) {
    size_t consumed;
    size_t hlen;
    ثابت حرف *hd;
    size_t unfold_len = 0;

    اگر(ڈیٹا->state.leading_unfold) {
      /* immediately after an unfold, keep only a single whitespace */
      جب(blen && ISBLANK(buf[0])) {
        buf++;
        blen--;
        unfold_len++;
      }
      اگر(blen) {
        /* insert a single space */
        نتیجہ = curlx_dyn_addn(&ڈیٹا->state.headerb, " ", 1);
        اگر(نتیجہ)
          واپس نتیجہ;
        ڈیٹا->state.leading_unfold = FALSE; /* done now */
      }
    }

    انتہا_ptr = memchr(buf, '\n', blen);
    اگر(!انتہا_ptr) {
      /* Not a complete header line within buffer, append the data to
         the end of the headerbuff. */
      نتیجہ = curlx_dyn_addn(&ڈیٹا->state.headerb, buf, blen);
      اگر(نتیجہ)
        واپس نتیجہ;
      *pconsumed += blen + unfold_len;

      اگر(!k->headerline) {
        /* check if this looks like a protocol header */
        statusline st =
          checkprotoprefix(ڈیٹا, conn,
                           curlx_dyn_ptr(&ڈیٹا->state.headerb),
                           curlx_dyn_len(&ڈیٹا->state.headerb));

        اگر(st == حالت_BAD) {
          /* this is not the beginning of a protocol first header line.
           * Cannot be 0.9 if version was detected or connection was reused. */
          k->سرخی = FALSE;
          streamclose(conn);
          اگر((k->httpversion >= 10) || conn->بٹ.reuse) {
            failf(ڈیٹا, "Invalid status line");
            واپس CURLE_WEIRD_SERVER_جواب;
          }
          اگر(!ڈیٹا->state.http_neg.accept_09) {
            failf(ڈیٹا, "Received HTTP/0.9 when not allowed");
            واپس CURLE_UNSUPPORTED_PROTOCOL;
          }
          leftover_body = TRUE;
          جاؤ برآمد;
        }
      }
      جاؤ برآمد; /* read more and try again */
    }

    /* the size of the remaining header line */
    consumed = (انتہا_ptr - buf) + 1;

    نتیجہ = curlx_dyn_addn(&ڈیٹا->state.headerb, buf, consumed);
    اگر(نتیجہ)
      واپس نتیجہ;
    blen -= consumed;
    buf += consumed;
    *pconsumed += consumed + unfold_len;

    /****
     * We now have a FULL header line in 'headerb'.
     *****/

    hlen = curlx_dyn_len(&ڈیٹا->state.headerb);
    hd = curlx_dyn_ptr(&ڈیٹا->state.headerb);

    اگر(!k->headerline) {
      /* the first read "header", the status line */
      statusline st = checkprotoprefix(ڈیٹا, conn, hd, hlen);
      اگر(st == حالت_BAD) {
        streamclose(conn);
        /* this is not the beginning of a protocol first header line.
         * Cannot be 0.9 if version was detected or connection was reused. */
        اگر((k->httpversion >= 10) || conn->بٹ.reuse) {
          failf(ڈیٹا, "Invalid status line");
          واپس CURLE_WEIRD_SERVER_جواب;
        }
        اگر(!ڈیٹا->state.http_neg.accept_09) {
          failf(ڈیٹا, "Received HTTP/0.9 when not allowed");
          واپس CURLE_UNSUPPORTED_PROTOCOL;
        }
        k->سرخی = FALSE;
        leftover_body = TRUE;
        جاؤ برآمد;
      }
    }
    ورنہ {
      اگر(hlen && !ISNEWLINE(hd[0])) {
        /* this is NOT the header separator */

        /* if we have bytes for the next header, check for folding */
        اگر(blen && ISBLANK(buf[0])) {
          /* remove the trailing CRLF and append the next header */
          unfold_سرخی(ڈیٹا);
          جاری;
        }
        ورنہ اگر(!blen) {
          /* this might be a folded header so deal with it in next invoke */
          ڈیٹا->state.maybe_folded = TRUE;
          توڑو;
        }
      }
    }

    نتیجہ = http_rw_hd(ڈیٹا, hd, hlen, buf, blen, &consumed);
    /* We are done with this line. We reset because response
     * processing might switch to HTTP/2 and that might call us
     * directly again. */
    curlx_dyn_reset(&ڈیٹا->state.headerb);
    اگر(consumed) {
      blen -= consumed;
      buf += consumed;
      *pconsumed += consumed;
    }
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  /* We might have reached the end of the header part here, but
     there might be a non-header part left in the end of the read
     buffer. */
برآمد:
  اگر(!k->سرخی && !leftover_body) {
    curlx_dyn_free(&ڈیٹا->state.headerb);
  }
  واپس CURLE_OK;
}

CURLcode Curl_http_write_resp_hd(ساخت Curl_easy *ڈیٹا,
                                 ثابت حرف *hd, size_t hdlen,
                                 bool is_eos)
{
  CURLcode نتیجہ;
  size_t consumed;
  حرف tmp = 0;
  DEBUGASSERT(!hd[hdlen]); /* null-terminated */

  نتیجہ = http_rw_hd(ڈیٹا, hd, hdlen, &tmp, 0, &consumed);
  اگر(!نتیجہ && is_eos) {
    نتیجہ = Curl_client_write(ڈیٹا, (CLIENTWRITE_BODY | CLIENTWRITE_EOS),
                               &tmp, 0);
  }
  واپس نتیجہ;
}

/*
 * HTTP protocol `write_resp` implementation. Parse headers
 * when not done yet and otherwise return without consuming data.
 */
CURLcode Curl_http_write_resp_hds(ساخت Curl_easy *ڈیٹا,
                                  ثابت حرف *buf, size_t blen,
                                  size_t *pconsumed)
{
  اگر(!ڈیٹا->req.سرخی) {
    *pconsumed = 0;
    واپس CURLE_OK;
  }
  ورنہ {
    CURLcode نتیجہ;

    نتیجہ = http_parse_سرخیاں(ڈیٹا, buf, blen, pconsumed);
    اگر(!نتیجہ && !ڈیٹا->req.سرخی) {
      اگر(!ڈیٹا->req.no_body && curlx_dyn_len(&ڈیٹا->state.headerb)) {
        /* leftover from parsing something that turned out not
         * to be a header, only happens if we allow for
         * HTTP/0.9 like responses */
        نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY,
                                   curlx_dyn_ptr(&ڈیٹا->state.headerb),
                                   curlx_dyn_len(&ڈیٹا->state.headerb));
      }
      curlx_dyn_free(&ڈیٹا->state.headerb);
    }
    واپس نتیجہ;
  }
}

CURLcode Curl_http_write_resp(ساخت Curl_easy *ڈیٹا,
                              ثابت حرف *buf, size_t blen,
                              bool is_eos)
{
  CURLcode نتیجہ;
  size_t consumed;
  صحیح flags;

  نتیجہ = Curl_http_write_resp_hds(ڈیٹا, buf, blen, &consumed);
  اگر(نتیجہ || ڈیٹا->req.مکمل)
    جاؤ برآمد;

  DEBUGASSERT(consumed <= blen);
  blen -= consumed;
  buf += consumed;
  /* either all was consumed in header parsing, or we have data left
   * and are done with headers, e.g. it is BODY data */
  DEBUGASSERT(!blen || !ڈیٹا->req.سرخی);
  اگر(!ڈیٹا->req.سرخی && (blen || is_eos)) {
    /* BODY data after header been parsed, write and consume */
    flags = CLIENTWRITE_BODY;
    اگر(is_eos)
      flags |= CLIENTWRITE_EOS;
    نتیجہ = Curl_client_write(ڈیٹا, flags, buf, blen);
  }
برآمد:
  واپس نتیجہ;
}

/* Decode HTTP status code string. */
CURLcode Curl_http_decode_حالت(int32_t *pstatus, ثابت حرف *s, size_t len)
{
  CURLcode نتیجہ = CURLE_BAD_FUNCTION_دلیل;
  int32_t حالت_2 = 0;
  صحیح i;

  اگر(len != 3)
    جاؤ برآمد;

  برائے(i = 0; i < 3; ++i) {
    حرف c = s[i];

    اگر(c < '0' || c > '9')
      جاؤ برآمد;

    حالت_2 *= 10;
    حالت_2 += c - '0';
  }
  نتیجہ = CURLE_OK;
برآمد:
  *pstatus = نتیجہ ? -1 : حالت_2;
  واپس نتیجہ;
}

CURLcode Curl_http_req_make(ساخت httpreq **preq,
                            ثابت حرف *طریقہ, size_t m_len,
                            ثابت حرف *scheme, size_t s_len,
                            ثابت حرف *authority, size_t a_len,
                            ثابت حرف *راستہ, size_t p_len)
{
  ساخت httpreq *req;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;

  DEBUGASSERT(طریقہ && m_len);

  req = curlx_calloc(1, حجم(*req) + m_len);
  اگر(!req)
    جاؤ برآمد;
#اگر defined(__GNUC__) && __GNUC__ >= 13
#ہدایت GCC diagnostic ڈالو
/* error: 'memcpy' offset [137, 142] from the object at 'req' is out of
   the bounds of referenced subobject 'method' with type 'char[1]' at
   offset 136 */
#ہدایت GCC diagnostic ignored "-Warray-bounds"
#اگر_ختم
  حافظہ_نقل(req->طریقہ, طریقہ, m_len);
#اگر defined(__GNUC__) && __GNUC__ >= 13
#ہدایت GCC diagnostic نکالو
#اگر_ختم
  اگر(scheme) {
    req->scheme = curlx_memdup0(scheme, s_len);
    اگر(!req->scheme)
      جاؤ برآمد;
  }
  اگر(authority) {
    req->authority = curlx_memdup0(authority, a_len);
    اگر(!req->authority)
      جاؤ برآمد;
  }
  اگر(راستہ) {
    req->راستہ = curlx_memdup0(راستہ, p_len);
    اگر(!req->راستہ)
      جاؤ برآمد;
  }
  Curl_dynhds_init(&req->سرخیاں, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  نتیجہ = CURLE_OK;

برآمد:
  اگر(نتیجہ && req)
    Curl_http_req_free(req);
  *preq = نتیجہ ? تہی : req;
  واپس نتیجہ;
}

ساکن CURLcode req_تفویض_url_authority(ساخت httpreq *req, CURLU *url)
{
  حرف *host, *پورٹ;
  ساخت dynbuf buf;
  CURLUcode uc;
  CURLcode نتیجہ = CURLE_URL_MALFORMAT;

  host = پورٹ = تہی;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_HOST, &host, 0);
  اگر(uc && uc != CURLUE_NO_HOST)
    جاؤ برآمد;
  اگر(!host) {
    req->authority = تہی;
    نتیجہ = CURLE_OK;
    جاؤ برآمد;
  }

  uc = curl_url_get(url, CURLUPART_پورٹ, &پورٹ, CURLU_NO_DEFAULT_پورٹ);
  اگر(uc && uc != CURLUE_NO_پورٹ)
    جاؤ برآمد;

  نتیجہ = curlx_dyn_جوڑو(&buf, host);
  اگر(نتیجہ)
    جاؤ برآمد;
  اگر(پورٹ) {
    نتیجہ = curlx_dyn_addf(&buf, ":%s", پورٹ);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  req->authority = curlx_dyn_ptr(&buf);
برآمد:
  curlx_free(host);
  curlx_free(پورٹ);
  اگر(نتیجہ)
    curlx_dyn_free(&buf);
  واپس نتیجہ;
}

ساکن CURLcode req_تفویض_url_راستہ(ساخت httpreq *req, CURLU *url)
{
  حرف *راستہ, *query;
  ساخت dynbuf buf;
  CURLUcode uc;
  CURLcode نتیجہ = CURLE_URL_MALFORMAT;

  راستہ = query = تہی;
  curlx_dyn_init(&buf, DYN_HTTP_REQUEST);

  uc = curl_url_get(url, CURLUPART_راستہ, &راستہ, 0);
  اگر(uc)
    جاؤ برآمد;
  uc = curl_url_get(url, CURLUPART_QUERY, &query, 0);
  اگر(uc && uc != CURLUE_NO_QUERY)
    جاؤ برآمد;

  اگر(!query) {
    req->راستہ = راستہ;
    راستہ = تہی;
  }
  ورنہ {
    نتیجہ = curlx_dyn_جوڑو(&buf, راستہ);
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_addf(&buf, "?%s", query);
    اگر(نتیجہ)
      جاؤ برآمد;
    req->راستہ = curlx_dyn_ptr(&buf);
  }
  نتیجہ = CURLE_OK;

برآمد:
  curlx_free(راستہ);
  curlx_free(query);
  اگر(نتیجہ)
    curlx_dyn_free(&buf);
  واپس نتیجہ;
}

CURLcode Curl_http_req_make2(ساخت httpreq **preq,
                             ثابت حرف *طریقہ, size_t m_len,
                             CURLU *url, ثابت حرف *scheme_default)
{
  ساخت httpreq *req;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;
  CURLUcode uc;

  DEBUGASSERT(طریقہ && m_len);

  req = curlx_calloc(1, حجم(*req) + m_len);
  اگر(!req)
    جاؤ برآمد;
  حافظہ_نقل(req->طریقہ, طریقہ, m_len);

  uc = curl_url_get(url, CURLUPART_SCHEME, &req->scheme, 0);
  اگر(uc && uc != CURLUE_NO_SCHEME)
    جاؤ برآمد;
  اگر(!req->scheme && scheme_default) {
    req->scheme = curlx_strdup(scheme_default);
    اگر(!req->scheme)
      جاؤ برآمد;
  }

  نتیجہ = req_تفویض_url_authority(req, url);
  اگر(نتیجہ)
    جاؤ برآمد;
  نتیجہ = req_تفویض_url_راستہ(req, url);
  اگر(نتیجہ)
    جاؤ برآمد;

  Curl_dynhds_init(&req->سرخیاں, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&req->trailers, 0, DYN_HTTP_REQUEST);
  نتیجہ = CURLE_OK;

برآمد:
  اگر(نتیجہ && req)
    Curl_http_req_free(req);
  *preq = نتیجہ ? تہی : req;
  واپس نتیجہ;
}

خالی Curl_http_req_free(ساخت httpreq *req)
{
  اگر(req) {
    curlx_free(req->scheme);
    curlx_free(req->authority);
    curlx_free(req->راستہ);
    Curl_dynhds_free(&req->سرخیاں);
    Curl_dynhds_free(&req->trailers);
    curlx_free(req);
  }
}

ساخت name_const {
  ثابت حرف *name;
  size_t namelen;
};

ساکن ثابت ساخت name_const H2_NON_میدان[] = {
  { STRCONST("Host") },
  { STRCONST("Upgrade") },
  { STRCONST("Connection") },
  { STRCONST("Keep-Alive") },
  { STRCONST("Proxy-Connection") },
  { STRCONST("Transfer-Encoding") },
};

ساکن bool h2_permissible_میدان(ساخت dynhds_اندراج *e)
{
  size_t i;
  برائے(i = 0; i < CURL_ARRAYSIZE(H2_NON_میدان); ++i) {
    اگر(e->namelen == H2_NON_میدان[i].namelen &&
       curl_strnequal(H2_NON_میدان[i].name, e->name, e->namelen))
      واپس FALSE;
  }
  واپس TRUE;
}

ساکن bool http_TE_has_token(ثابت حرف *fvalue, ثابت حرف *token)
{
  جب(*fvalue) {
    ساخت Curl_str name;

    /* skip to first token */
    جب(ISBLANK(*fvalue) || *fvalue == ',')
      fvalue++;
    اگر(curlx_str_cspn(&fvalue, &name, " \t\r;,"))
      واپس FALSE;
    اگر(curlx_str_casecompare(&name, token))
      واپس TRUE;

    /* skip any remainder after token, e.g. parameters with quoted strings */
    جب(*fvalue && *fvalue != ',') {
      اگر(*fvalue == '"') {
        ساخت Curl_str qw;
        /* if we do not cleanly find a quoted word here, the header value
         * does not follow HTTP syntax and we reject */
        اگر(curlx_str_quotedword(&fvalue, &qw, CURL_MAX_HTTP_سرخی))
          واپس FALSE;
      }
      ورنہ
        fvalue++;
    }
  }
  واپس FALSE;
}

CURLcode Curl_http_req_مقصد_h2(ساخت dynhds *h2_سرخیاں,
                             ساخت httpreq *req, ساخت Curl_easy *ڈیٹا)
{
  ثابت حرف *scheme = تہی, *authority = تہی;
  ساخت dynhds_اندراج *e;
  size_t i;
  CURLcode نتیجہ;

  DEBUGASSERT(req);
  DEBUGASSERT(h2_سرخیاں);

  اگر(req->scheme) {
    scheme = req->scheme;
  }
  ورنہ اگر(strcmp("CONNECT", req->طریقہ)) {
    scheme = Curl_checkheaders(ڈیٹا, STRCONST(HTTP_PSEUDO_SCHEME));
    اگر(scheme) {
      scheme += حجم(HTTP_PSEUDO_SCHEME);
      curlx_str_passblanks(&scheme);
      infof(ڈیٹا, "set pseudo header %s to %s", HTTP_PSEUDO_SCHEME, scheme);
    }
    ورنہ {
      scheme = ڈیٹا->state.origin->scheme->name;
    }
  }

  اگر(req->authority) {
    authority = req->authority;
  }
  ورنہ {
    e = Curl_dynhds_get(&req->سرخیاں, STRCONST("Host"));
    اگر(e)
      authority = e->قدر;
  }

  Curl_dynhds_reset(h2_سرخیاں);
  Curl_dynhds_سیٹ_opts(h2_سرخیاں, DYNHDS_OPT_LOWERCASE);
  نتیجہ = Curl_dynhds_جوڑو(h2_سرخیاں, STRCONST(HTTP_PSEUDO_طریقہ),
                           req->طریقہ, لڑی_لمبائی(req->طریقہ));
  اگر(!نتیجہ && scheme) {
    نتیجہ = Curl_dynhds_جوڑو(h2_سرخیاں, STRCONST(HTTP_PSEUDO_SCHEME),
                             scheme, لڑی_لمبائی(scheme));
  }
  اگر(!نتیجہ && authority) {
    نتیجہ = Curl_dynhds_جوڑو(h2_سرخیاں, STRCONST(HTTP_PSEUDO_AUTHORITY),
                             authority, لڑی_لمبائی(authority));
  }
  اگر(!نتیجہ && req->راستہ) {
    نتیجہ = Curl_dynhds_جوڑو(h2_سرخیاں, STRCONST(HTTP_PSEUDO_راستہ),
                             req->راستہ, لڑی_لمبائی(req->راستہ));
  }
  برائے(i = 0; !نتیجہ && i < Curl_dynhds_گنتی(&req->سرخیاں); ++i) {
    e = Curl_dynhds_getn(&req->سرخیاں, i);
    /* "TE" is special in that it is only permissible when it
     * has only value "trailers". RFC 9113 ch. 8.2.2 */
    اگر(e->namelen == 2 && curl_strequal("TE", e->name)) {
      اگر(http_TE_has_token(e->قدر, "trailers"))
        نتیجہ = Curl_dynhds_جوڑو(h2_سرخیاں, e->name, e->namelen,
                                 "trailers", CURL_CSTRLEN("trailers"));
    }
    ورنہ اگر(h2_permissible_میدان(e)) {
      نتیجہ = Curl_dynhds_جوڑو(h2_سرخیاں, e->name, e->namelen,
                               e->قدر, e->valuelen);
    }
  }

  واپس نتیجہ;
}

CURLcode Curl_http_resp_make(ساخت http_resp **presp,
                             صحیح حالت_2,
                             ثابت حرف *description)
{
  ساخت http_resp *resp;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;

  resp = curlx_calloc(1, حجم(*resp));
  اگر(!resp)
    جاؤ برآمد;

  resp->حالت_2 = حالت_2;
  اگر(description) {
    resp->description = curlx_strdup(description);
    اگر(!resp->description)
      جاؤ برآمد;
  }
  Curl_dynhds_init(&resp->سرخیاں, 0, DYN_HTTP_REQUEST);
  Curl_dynhds_init(&resp->trailers, 0, DYN_HTTP_REQUEST);
  نتیجہ = CURLE_OK;

برآمد:
  اگر(نتیجہ && resp)
    Curl_http_resp_free(resp);
  *presp = نتیجہ ? تہی : resp;
  واپس نتیجہ;
}

خالی Curl_http_resp_free(ساخت http_resp *resp)
{
  اگر(resp) {
    curlx_free(resp->description);
    Curl_dynhds_free(&resp->سرخیاں);
    Curl_dynhds_free(&resp->trailers);
    اگر(resp->پچھلا)
      Curl_http_resp_free(resp->پچھلا);
    curlx_free(resp);
  }
}

/*
 * HTTP handler interface.
 */
ثابت ساخت Curl_protocol Curl_protocol_http = {
  Curl_http_تیاری_conn,                 /* setup_connection */
  Curl_http,                            /* do_it */
  Curl_http_مکمل,                       /* done */
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

#اگر_ختم /* CURL_DISABLE_HTTP */
