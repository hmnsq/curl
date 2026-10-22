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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_LINUX_TCP_H
#అన్తర్భావ <linux/tcp.h>
#లేదావేళ defined(HAVE_NETINET_TCP_H)
#అన్తర్భావ <netinet/tcp.h>
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "content_encoding.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "curl_share.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "setopt.h"
#అన్తర్భావ "altsvc.h"
#అన్తర్భావ "hsts.h"
#అన్తర్భావ "tftp.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "bufref.h"
#అన్తర్భావ "vauth/vauth.h"

స్థిర CURLcode setopt_సమితి_timeout_sec(timediff_t *ptimeout_ms, దీర్ఘ secs)
{
  ఒకవేళ(secs < 0)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
#ఒకవేళ LONG_MAX > (TIMEDIFF_T_MAX / 1000)
  ఒకవేళ(secs > (TIMEDIFF_T_MAX / 1000)) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    తిరిగి CURLE_OK;
  }
#అయితే_ముగింపు
  *ptimeout_ms = (timediff_t)secs * 1000;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode setopt_సమితి_timeout_ms(timediff_t *ptimeout_ms, దీర్ఘ ms)
{
  ఒకవేళ(ms < 0)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
#ఒకవేళ LONG_MAX > TIMEDIFF_T_MAX
  ఒకవేళ(ms > TIMEDIFF_T_MAX) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    తిరిగి CURLE_OK;
  }
#అయితే_ముగింపు
  *ptimeout_ms = (timediff_t)ms;
  తిరిగి CURLE_OK;
}

CURLcode Curl_setstropt(నిర్మాణం Curl_easy *దత్తాంశం,
                        గణన dupstring id, స్థిరరాశి అక్షరం *s)
{
  size_t slen = s ? అక్షరశ్రేణి_పొడవు(s) : 0;
  DEBUGASSERT((గుర్తులేని)id <= UINT8_MAX);
  ఒకవేళ(s && (slen > CURL_MAX_ఆదానం_నిడివి))
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  తిరిగి CURL_EASY_STR_సమితి(దత్తాంశం, (uint8_t)id, s, slen);
}

CURLcode Curl_setblobopt(నిర్మాణం curl_blob **blobp,
                         స్థిరరాశి నిర్మాణం curl_blob *blob)
{
  /* free the previous storage at `blobp' and replace by a dynamic storage
     copy of blob. If CURL_BLOB_COPY is set, the data is copied. */

  curlx_safefree(*blobp);

  ఒకవేళ(blob) {
    నిర్మాణం curl_blob *nblob;
    ఒకవేళ(!blob->దత్తాంశం || !blob->len || (blob->len > CURL_MAX_ఆదానం_నిడివి))
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    nblob = (నిర్మాణం curl_blob *)
      curlx_malloc(పరిమాణం(నిర్మాణం curl_blob) +
                   ((blob->flags & CURL_BLOB_COPY) ? blob->len : 0));
    ఒకవేళ(!nblob)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    *nblob = *blob;
    ఒకవేళ(blob->flags & CURL_BLOB_COPY) {
      /* put the data after the blob struct in memory */
      nblob->దత్తాంశం = (అక్షరం *)nblob + పరిమాణం(నిర్మాణం curl_blob);
      జ్ఞాపకం_నకలు(nblob->దత్తాంశం, blob->దత్తాంశం, blob->len);
    }

    *blobp = nblob;
    తిరిగి CURLE_OK;
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode setstropt_userpwd(స్థిరరాశి అక్షరం *option, అక్షరం **userp,
                                  అక్షరం **passwdp)
{
  అక్షరం *user = ఖాళీ;
  అక్షరం *passwd = ఖాళీ;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Parse the login details if specified. If not, then we treat NULL as a
     hint to clear the existing data */
  ఒకవేళ(option) {
    size_t len = అక్షరశ్రేణి_పొడవు(option);
    CURLcode ఫలితం;
    ఒకవేళ(len > CURL_MAX_ఆదానం_నిడివి)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;

    ఫలితం = Curl_parse_login_details(option, len, &user, &passwd, ఖాళీ);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  curlx_free(*userp);
  *userp = user;

  curlx_strzero(*passwdp);
  curlx_free(*passwdp);
  *passwdp = passwd;

  తిరిగి CURLE_OK;
}

స్థిర CURLcode setstropt_interface(నిర్మాణం Curl_easy *దత్తాంశం, అక్షరం *option)
{
  ఒకవేళ(option && !option[0])
    option = ఖాళీ;
  ఒకవేళ(option) {
    /* Parse for checking syntax */
    CURLcode ఫలితం = Curl_parse_interface(option, ఖాళీ, ఖాళీ, ఖాళీ);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    తిరిగి CURL_EASY_STR_సమితి(దత్తాంశం, STRING_INTERFACE_OPT,
                             option, అక్షరశ్రేణి_పొడవు(option));
  }
  CURL_EASY_STR_CLEAR(దత్తాంశం, STRING_INTERFACE_OPT);
  తిరిగి CURLE_OK;
}

#నిర్వచితమైతే USE_SSL
#నిర్వచించు C_SSLVERSION_విలువ(x)     ((x) & 0xffff)
#నిర్వచించు C_SSLVERSION_MAX_విలువ(x) ((గుర్తులేని దీర్ఘ)(x) & 0xffff0000)
#అయితే_ముగింపు

స్థిర CURLcode protocol2num(స్థిరరాశి అక్షరం *str, curl_prot_t *val)
{
  /*
   * We are asked to cherry-pick protocols, so play it safe and disallow all
   * protocols to start with, and re-add the wanted ones back in.
   */
  *val = 0;

  ఒకవేళ(!str)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  ఒకవేళ(curl_strequal(str, "all")) {
    *val = ~(curl_prot_t)0;
    తిరిగి CURLE_OK;
  }

  చేయి {
    స్థిరరాశి అక్షరం *token = str;
    size_t tlen;

    str = strchr(str, ',');
    tlen = str ? (size_t)(str - token) : అక్షరశ్రేణి_పొడవు(token);
    ఒకవేళ(tlen) {
      స్థిరరాశి నిర్మాణం Curl_scheme *h = Curl_getn_scheme(token, tlen);

      ఒకవేళ(!h || !h->నిరంతరం)
        తిరిగి CURLE_UNSUPPORTED_PROTOCOL;

      *val |= h->protocol;
    }
  } అయితే(str && str++);

  ఒకవేళ(!*val)
    /* no protocol listed */
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  తిరిగి CURLE_OK;
}

#ఒకవేళ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
స్థిర CURLcode httpauth(నిర్మాణం Curl_easy *దత్తాంశం, bool proxy,
                         గుర్తులేని దీర్ఘ auth)
{
  ఒకవేళ(auth != CURLAUTH_NONE) {
    పూర్ణాంకం bitcheck = 0;
    bool authbits = FALSE;
    ఒకవేళ(auth & CURLAUTH_DIGEST_IE) {
      auth |= CURLAUTH_DIGEST; /* set standard digest bit */
      auth &= ~CURLAUTH_DIGEST_IE; /* drop the legacy bit */
    }

    /* switch off bits we cannot support */
#నిర్వచితం_కాకపోతే USE_NTLM
    auth &= ~CURLAUTH_NTLM; /* no NTLM support */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే USE_SPNEGO
    auth &= ~CURLAUTH_NEGOTIATE; /* no Negotiate (SPNEGO) auth without GSS-API
                                    or SSPI */
#అయితే_ముగింపు

    /* check if any auth bit lower than CURLAUTH_ONLY is still set */
    అయితే(bitcheck < 31) {
      ఒకవేళ(auth & (1UL << bitcheck++)) {
        authbits = TRUE;
        ఆపు;
      }
    }
    ఒకవేళ(!authbits)
      తిరిగి CURLE_NOT_BUILT_లోపల; /* no supported types left! */
  }
  ఒకవేళ(proxy)
    దత్తాంశం->సమితి.proxyauth = (uint32_t)auth;
  లేకపోతే
    దత్తాంశం->సమితి.httpauth = (uint32_t)auth;
  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */

#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
స్థిర CURLcode setopt_HTTP_సంస్కరణ(నిర్మాణం Curl_easy *దత్తాంశం, దీర్ఘ arg)
{
  /*
   * This sets a requested HTTP version to be used. The value is one of
   * the listed enums in curl/curl.h.
   */
  ఎంపిక(arg) {
  స్థితి CURL_HTTP_సంస్కరణ_NONE:
    /* accepted */
    ఆపు;
  స్థితి CURL_HTTP_సంస్కరణ_1_0:
  స్థితి CURL_HTTP_సంస్కరణ_1_1:
    /* accepted */
    ఆపు;
#నిర్వచితమైతే USE_HTTP2
  స్థితి CURL_HTTP_సంస్కరణ_2_0:
  స్థితి CURL_HTTP_సంస్కరణ_2_TLS:
  స్థితి CURL_HTTP_సంస్కరణ_2_పూర్వ_KNOWLEDGE:
    /* accepted */
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితమైతే USE_HTTP3
  స్థితి CURL_HTTP_సంస్కరణ_3:
  స్థితి CURL_HTTP_సంస్కరణ_3_ONLY:
    /* accepted */
    ఆపు;
#అయితే_ముగింపు
  అప్రమేయ:
    /* not accepted */
    ఒకవేళ(arg < CURL_HTTP_సంస్కరణ_NONE)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    తిరిగి CURLE_UNSUPPORTED_PROTOCOL;
  }
  దత్తాంశం->సమితి.httpwant = (గుర్తులేని అక్షరం)arg;
  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు /* !CURL_DISABLE_HTTP */

#నిర్వచితమైతే USE_SSL
CURLcode Curl_setopt_SSLVERSION(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                                దీర్ఘ arg)
{
  /*
   * Set explicit SSL version to try to connect with, as some SSL
   * implementations are lame.
   */
  {
    దీర్ఘ సంస్కరణ_2, సంస్కరణ_max;
    నిర్మాణం ssl_easy_config *sslc = &దత్తాంశం->సమితి.ssl;
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
    ఒకవేళ(option != CURLOPT_SSLVERSION)
      sslc = &దత్తాంశం->సమితి.proxy_ssl;
#లేకపోతే
    (శూన్య)option; /*వాడని*/
#అయితే_ముగింపు
    సంస్కరణ_2 = C_SSLVERSION_విలువ(arg);
    సంస్కరణ_max = (దీర్ఘ)C_SSLVERSION_MAX_విలువ(arg);

    ఒకవేళ(సంస్కరణ_2 < CURL_SSLVERSION_DEFAULT ||
       సంస్కరణ_2 == CURL_SSLVERSION_SSLv2 ||
       సంస్కరణ_2 == CURL_SSLVERSION_SSLv3 ||
       సంస్కరణ_2 >= CURL_SSLVERSION_చివరి ||
       సంస్కరణ_max < CURL_SSLVERSION_MAX_NONE ||
       సంస్కరణ_max >= CURL_SSLVERSION_MAX_చివరి)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    ఒకవేళ(సంస్కరణ_2 == CURL_SSLVERSION_DEFAULT)
      సంస్కరణ_2 = CURL_SSLVERSION_TLSv1_2;

    sslc->సంస్కరణ_2 = (గుర్తులేని అక్షరం)సంస్కరణ_2;
    sslc->సంస్కరణ_max = (గుర్తులేని పూర్ణాంకం)సంస్కరణ_max;
  }
  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు /* !USE_SSL */

#నిర్వచితం_కాకపోతే CURL_DISABLE_RTSP
స్థిర CURLcode setopt_RTSP_REQUEST(నిర్మాణం Curl_easy *దత్తాంశం, దీర్ఘ arg)
{
  /*
   * Set the RTSP request method (OPTIONS, SETUP, PLAY, etc...) Would this be
   * better if the RTSPREQ_* were moved into here?
   */
  ఒకవేళ((arg <= CURL_RTSPREQ_NONE) || (arg >= CURL_RTSPREQ_చివరి))
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  దత్తాంశం->సమితి.rtspreq = (గుర్తులేని అక్షరం)arg;
  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు /* !CURL_DISABLE_RTSP */

స్థిర CURLcode setopt_long_bool(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                                 దీర్ఘ arg)
{
  bool enabled = !!arg;
  పూర్ణాంకం ok = 1;
  నిర్మాణం UserDefined *s = &దత్తాంశం->సమితి;
  ఎంపిక(option) {
  స్థితి CURLOPT_FORBID_REUSE:
    /*
     * When this transfer is done, it must not be left to be reused by a
     * subsequent transfer but shall be closed immediately.
     */
    s->reuse_forbid = enabled;
    ఆపు;
  స్థితి CURLOPT_FRESH_CONNECT:
    /*
     * This transfer shall not use a previously cached connection but
     * should be made with a fresh new connect!
     */
    s->reuse_fresh = enabled;
    ఆపు;
  స్థితి CURLOPT_VERBOSE:
    /*
     * Verbose means infof() calls that give a lot of information about
     * the connection and transfer procedures as well as internal choices.
     */
    s->verbose = enabled;
    ఆపు;
  స్థితి CURLOPT_శీర్షిక:
    /*
     * Set to include the header in the general data output stream.
     */
    s->include_శీర్షిక = enabled;
    ఆపు;
  స్థితి CURLOPT_NOPROGRESS:
    /*
     * Shut off the internal supported progress meter
     */
    దత్తాంశం->పురోగతి.hide = enabled;
    ఆపు;
  స్థితి CURLOPT_NOBODY:
    /*
     * Do not include the body part in the output data stream.
     */
    s->opt_no_body = enabled;
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
    ఒకవేళ(s->opt_no_body)
      /* in HTTP lingo, no body means using the HEAD request... */
      s->పద్ధతి = HTTPREQ_తల;
    లేకపోతే ఒకవేళ(s->పద్ధతి == HTTPREQ_తల)
      s->పద్ధతి = HTTPREQ_GET;
#అయితే_ముగింపు
    ఆపు;
  స్థితి CURLOPT_FAILONERROR:
    /*
     * Do not output the >=400 error code HTML-page, but instead only
     * return error.
     */
    s->http_fail_on_error = enabled;
    ఆపు;
  స్థితి CURLOPT_KEEP_SENDING_ON_ERROR:
    s->http_keep_sending_on_error = enabled;
    ఆపు;
  స్థితి CURLOPT_UPLOAD:
  స్థితి CURLOPT_PUT:
    /*
     * We want to send data to the remote host. If this is HTTP, that equals
     * using the PUT request.
     */
    ఒకవేళ(enabled) {
      /* If this is HTTP, PUT is what's needed to "upload" */
      s->పద్ధతి = HTTPREQ_PUT;
      s->opt_no_body = FALSE; /* this is implied */
    }
    లేకపోతే
      /* In HTTP, the opposite of upload is GET (unless NOBODY is true as
         then this can be changed to HEAD later on) */
      s->పద్ధతి = HTTPREQ_GET;
    ఆపు;
  స్థితి CURLOPT_FILETIME:
    /*
     * Try to get the file time of the remote document. The time will
     * later (possibly) become available using curl_easy_getinfo().
     */
    s->get_filetime = enabled;
    ఆపు;
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  స్థితి CURLOPT_HTTP09_ALLOWED:
    s->http09_allowed = enabled;
    ఆపు;
#నిర్వచితం_కాకపోతే CURL_DISABLE_COOKIES
  స్థితి CURLOPT_COOKIESESSION:
    /*
     * Set this option to TRUE to start a new "cookie session". It will
     * prevent the forthcoming read-cookies-from-file actions to accept
     * cookies that are marked as being session cookies, as they belong to a
     * previous session.
     */
    s->cookiesession = enabled;
    ఆపు;
#అయితే_ముగింపు
  స్థితి CURLOPT_AUTOREFERER:
    /*
     * Switch on automatic referer that gets set if curl follows locations.
     */
    s->http_auto_referer = enabled;
    ఆపు;
  స్థితి CURLOPT_TRANSFER_ENCODING:
    s->http_transfer_encoding = enabled;
    ఆపు;
  స్థితి CURLOPT_UNRESTRICTED_AUTH:
    /*
     * Send authentication (user+password) when following locations, even when
     * hostname changed.
     */
    s->allow_auth_గమ్యం_other_hosts = enabled;
    ఆపు;
  స్థితి CURLOPT_HTTP_TRANSFER_DECODING:
    /*
     * disable libcurl transfer encoding is used
     */
    s->http_te_skip = !enabled; /* reversed */
    ఆపు;
  స్థితి CURLOPT_HTTP_CONTENT_DECODING:
    /*
     * raw data passed to the application when content encoding is used
     */
    s->http_ce_skip = !enabled; /* reversed */
    ఆపు;
  స్థితి CURLOPT_HTTPGET:
    /*
     * Set to force us do HTTP GET
     */
    ఒకవేళ(enabled) {
      s->పద్ధతి = HTTPREQ_GET;
      s->opt_no_body = FALSE; /* this is implied */
    }
    ఆపు;
  స్థితి CURLOPT_POST:
    /* Does this option serve a purpose anymore? Yes it does, when
       CURLOPT_POSTFIELDS is not used and the POST data is read off the
       callback! */
    ఒకవేళ(enabled) {
      s->పద్ధతి = HTTPREQ_POST;
      s->opt_no_body = FALSE; /* this is implied */
    }
    లేకపోతే
      s->పద్ధతి = HTTPREQ_GET;
    ఆపు;
#అయితే_ముగింపు /* !CURL_DISABLE_HTTP */
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  స్థితి CURLOPT_HTTPPROXYTUNNEL:
    /*
     * Tunnel operations through the proxy instead of normal proxy use
     */
    s->tunnel_thru_httpproxy = enabled;
    ఆపు;
  స్థితి CURLOPT_HAPROXYPROTOCOL:
    /*
     * Set to send the HAProxy Proxy Protocol header
     */
    s->haproxyprotocol = enabled;
    ఆపు;
  స్థితి CURLOPT_PROXY_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for proxy.
     */
    s->proxy_ssl.verifypeer = enabled;

    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(దత్తాంశం, TRUE);
    ఆపు;
  స్థితి CURLOPT_PROXY_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for proxy
     */
    s->proxy_ssl.verifyhost = enabled;
    ok = 2;
    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(దత్తాంశం, TRUE);
    ఆపు;
  స్థితి CURLOPT_PROXY_TRANSFER_బహుళకం:
    /*
     * set transfer mode (;type=<a|i>) when doing FTP via an HTTP proxy
     */
    s->proxy_transfer_బహుళకం = enabled;
    ఆపు;
#అయితే_ముగింపు /* !CURL_DISABLE_PROXY */
#ఒకవేళ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  స్థితి CURLOPT_SOCKS5_GSSAPI_NEC:
    /*
     * Set flag for NEC SOCKS5 support
     */
    s->socks5_gssapi_nec = enabled;
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితమైతే CURL_LIST_ONLY_PROTOCOL
  స్థితి CURLOPT_DIRLISTONLY:
    /*
     * An option that changes the command to one that asks for a list only, no
     * file info details. Used for FTP, POP3 and SFTP.
     */
    s->list_only = enabled;
    ఆపు;
#అయితే_ముగింపు
  స్థితి CURLOPT_APPEND:
    /*
     * We want to upload and append to an existing file. Used for FTP and
     * SFTP.
     */
    s->remote_append = enabled;
    ఆపు;
#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
  స్థితి CURLOPT_FTP_USE_EPRT:
    s->ftp_use_eprt = enabled;
    ఆపు;
  స్థితి CURLOPT_FTP_USE_EPSV:
    s->ftp_use_epsv = enabled;
    ఆపు;
  స్థితి CURLOPT_FTP_USE_PRET:
    s->ftp_use_pret = enabled;
    ఆపు;
  స్థితి CURLOPT_FTP_SKIP_PASV_IP:
    /*
     * Enable or disable FTP_SKIP_PASV_IP, which will disable/enable the
     * bypass of the IP address in PASV responses.
     */
    s->ftp_skip_ip = enabled;
    ఆపు;
  స్థితి CURLOPT_WILDCARDMATCH:
    s->wildcard_enabled = enabled;
    ఆపు;
#అయితే_ముగింపు
  స్థితి CURLOPT_CRLF:
    /*
     * Kludgy option to enable CRLF conversions. Subject for removal.
     */
    s->crlf = enabled;
    ఆపు;
#నిర్వచితం_కాకపోతే CURL_DISABLE_TFTP
  స్థితి CURLOPT_TFTP_NO_OPTIONS:
    /*
     * Option that prevents libcurl from sending TFTP option requests to the
     * server.
     */
    s->tftp_no_options = enabled;
    ఆపు;
#అయితే_ముగింపు /* !CURL_DISABLE_TFTP */
  స్థితి CURLOPT_TRANSFERTEXT:
    /*
     * This option was previously named 'FTPASCII'. Renamed to work with
     * more protocols than merely FTP.
     *
     * Transfer using ASCII (instead of BINARY).
     */
    s->prefer_ascii = enabled;
    ఆపు;
  స్థితి CURLOPT_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying.
     */
    s->ssl.verifypeer = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(దత్తాంశం, FALSE);
    ఆపు;
#నిర్వచితం_కాకపోతే CURL_DISABLE_DOH
  స్థితి CURLOPT_DOH_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for DoH.
     */
    s->doh_verifypeer = enabled;
    ఆపు;
  స్థితి CURLOPT_DOH_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for DoH
     */
    s->doh_verifyhost = enabled;
    ok = 2;
    ఆపు;
  స్థితి CURLOPT_DOH_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying for DoH.
     */
    ఒకవేళ(!Curl_ssl_cert_స్థితి_request())
      తిరిగి CURLE_NOT_BUILT_లోపల;

    s->doh_verifystatus = enabled;
    ok = 2;
    ఆపు;
#అయితే_ముగింపు /* !CURL_DISABLE_DOH */
  స్థితి CURLOPT_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate
     */

    /* Obviously people are not reading documentation and too many thought
       this argument took a boolean when it was not and misused it.
       Treat 1 and 2 the same */
    s->ssl.verifyhost = enabled;
    ok = 2;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(దత్తాంశం, FALSE);
    ఆపు;
  స్థితి CURLOPT_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying.
     */
    ఒకవేళ(!Curl_ssl_cert_స్థితి_request())
      తిరిగి CURLE_NOT_BUILT_లోపల;

    s->ssl.verifystatus = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(దత్తాంశం, FALSE);
    ఆపు;
  స్థితి CURLOPT_CERTINFO:
#నిర్వచితమైతే USE_SSL
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_CERTINFO))
      s->ssl.certinfo = enabled;
    లేకపోతే
#అయితే_ముగింపు
      తిరిగి CURLE_NOT_BUILT_లోపల;
    ఆపు;
  స్థితి CURLOPT_NOSIGNAL:
    /*
     * The application asks not to set any signal() or alarm() handlers,
     * even when using a timeout.
     */
    s->no_signal = enabled;
    ఆపు;
  స్థితి CURLOPT_TCP_NODELAY:
    /*
     * Enable or disable TCP_NODELAY, which will disable/enable the Nagle
     * algorithm
     */
    s->tcp_nodelay = enabled;
    ఆపు;
  స్థితి CURLOPT_IGNORE_CONTENT_నిడివి:
    s->ignorecl = enabled;
    ఆపు;
  స్థితి CURLOPT_SSL_SESSIONID_CACHE:
    s->ssl.cache_session = enabled;
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
    s->proxy_ssl.cache_session = s->ssl.cache_session;
#అయితే_ముగింపు
    ఆపు;
#నిర్వచితమైతే USE_SSH
  స్థితి CURLOPT_SSH_COMPRESSION:
    s->ssh_compression = enabled;
    ఆపు;
#అయితే_ముగింపు /* !USE_SSH */
#నిర్వచితం_కాకపోతే CURL_DISABLE_SMTP
  స్థితి CURLOPT_MAIL_RCPT_ALLOWFAILS:
    /* allow RCPT TO command to fail for some recipients */
    s->mail_rcpt_allowfails = enabled;
    ఆపు;
#అయితే_ముగింపు /* !CURL_DISABLE_SMTP */
  స్థితి CURLOPT_SASL_IR:
    /* Enable/disable SASL initial response */
    s->sasl_ir = enabled;
    ఆపు;
  స్థితి CURLOPT_TCP_KEEPALIVE:
    s->tcp_keepalive = enabled;
    ఆపు;
  స్థితి CURLOPT_TCP_FASTOPEN:
#ఒకవేళ defined(CONNECT_దత్తాంశం_IDEMPOTENT) || defined(MSG_FASTOPEN) || \
  defined(TCP_FASTOPEN_CONNECT)
    s->tcp_fastopen = enabled;
    ఆపు;
#లేకపోతే
    తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
  స్థితి CURLOPT_SSL_ENABLE_ALPN:
    s->ssl_enable_alpn = enabled;
    ఆపు;
  స్థితి CURLOPT_మార్గం_AS_IS:
    s->మార్గం_as_is = enabled;
    ఆపు;
  స్థితి CURLOPT_PIPEWAIT:
    s->pipewait = enabled;
    ఆపు;
  స్థితి CURLOPT_SUPPRESS_CONNECT_శీర్షికలు:
    s->suppress_connect_శీర్షికలు = enabled;
    ఆపు;
#నిర్వచితం_కాకపోతే CURL_DISABLE_SHUFFLE_DNS
  స్థితి CURLOPT_DNS_SHUFFLE_చిరునామాలు:
    s->dns_shuffle_చిరునామాలు = enabled;
    ఆపు;
#అయితే_ముగింపు
  స్థితి CURLOPT_DISALLOW_USERNAME_లోపల_URL:
    s->disallow_username_లోపల_url = enabled;
    ఆపు;
  స్థితి CURLOPT_శీఘ్ర_EXIT:
    s->quick_exit = enabled;
    ఆపు;
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }
  ఒకవేళ((arg > ok) || (arg < 0))
    /* reserve other values for future use */
    infof(దత్తాంశం, "boolean setopt(%d) got unsupported argument %ld,"
          " treated as %d", (పూర్ణాంకం)option, arg, enabled);

  తిరిగి CURLE_OK;
}

స్థిర CURLcode విలువ_range(దీర్ఘ *విలువ, దీర్ఘ క్రింద_error, దీర్ఘ min, దీర్ఘ max)
{
  ఒకవేళ(*విలువ < క్రింద_error)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  లేకపోతే ఒకవేళ(*విలువ < min)
    *విలువ = min;
  లేకపోతే ఒకవేళ(*విలువ > max)
    *విలువ = max;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode setopt_long_net(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                                దీర్ఘ arg)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం UserDefined *s = &దత్తాంశం->సమితి;

  ఎంపిక(option) {
  స్థితి CURLOPT_DNS_CACHE_TIMEOUT:
    ఒకవేళ(arg != -1)
      తిరిగి setopt_సమితి_timeout_sec(&s->dns_cache_timeout_ms, arg);
    s->dns_cache_timeout_ms = -1;
    ఆపు;
  స్థితి CURLOPT_MAXCONNECTS:
    ఫలితం = విలువ_range(&arg, 0, 0, గరిష్ఠ_పూర్ణాంకం);
    ఒకవేళ(!ఫలితం)
      s->maxconnects = arg ? (uint32_t)arg : DEFAULT_CONNCACHE_కొలత;
    ఆపు;
  స్థితి CURLOPT_SERVER_RESPONSE_TIMEOUT:
    తిరిగి setopt_సమితి_timeout_sec(&s->server_response_timeout, arg);
  స్థితి CURLOPT_SERVER_RESPONSE_TIMEOUT_MS:
    తిరిగి setopt_సమితి_timeout_ms(&s->server_response_timeout, arg);
  స్థితి CURLOPT_దిగువ_వేగం_హద్దు:
    ఒకవేళ(arg < 0)
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    లేకపోతే
      s->దిగువ_వేగం_హద్దు = arg;
    ఆపు;
  స్థితి CURLOPT_దిగువ_వేగం_సమయం:
    ఫలితం = విలువ_range(&arg, 0, 0, USHRT_MAX);
    ఒకవేళ(!ఫలితం)
      s->దిగువ_వేగం_సమయం = (uint16_t)arg;
    ఆపు;
  స్థితి CURLOPT_పోర్టు:
    ఒకవేళ((arg < 0) || (arg > 65535))
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    s->use_పోర్టు = (గుర్తులేని పొట్టి)arg;
    ఆపు;
  స్థితి CURLOPT_TIMEOUT:
    తిరిగి setopt_సమితి_timeout_sec(&s->timeout, arg);
  స్థితి CURLOPT_TIMEOUT_MS:
    తిరిగి setopt_సమితి_timeout_ms(&s->timeout, arg);
  స్థితి CURLOPT_CONNECTTIMEOUT:
    తిరిగి setopt_సమితి_timeout_sec(&s->connecttimeout, arg);
  స్థితి CURLOPT_CONNECTTIMEOUT_MS:
    తిరిగి setopt_సమితి_timeout_ms(&s->connecttimeout, arg);
#నిర్వచితం_కాకపోతే CURL_DISABLE_BINDLOCAL
  స్థితి CURLOPT_LOCALPORT:
    ఒకవేళ((arg < 0) || (arg > 65535))
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    s->localport = curlx_sltous(arg);
    ఆపు;
  స్థితి CURLOPT_LOCALPORTRANGE:
    ఒకవేళ((arg < 0) || (arg > 65535))
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    s->localportrange = curlx_sltous(arg);
    ఆపు;
#అయితే_ముగింపు
  స్థితి CURLOPT_BUFFERSIZE:
    ఫలితం = విలువ_range(&arg, 0, READBUFFER_MIN, READBUFFER_MAX);
    ఒకవేళ(!ఫలితం)
      s->బఫరు_కొలత = (గుర్తులేని పూర్ణాంకం)arg;
    ఆపు;
  స్థితి CURLOPT_UPLOAD_BUFFERSIZE:
    ఫలితం = విలువ_range(&arg, 0, UPLOADBUFFER_MIN, UPLOADBUFFER_MAX);
    ఒకవేళ(!ఫలితం)
      s->upload_బఫరు_కొలత = (గుర్తులేని పూర్ణాంకం)arg;
    ఆపు;
  స్థితి CURLOPT_MAXFILESIZE:
    ఒకవేళ(arg < 0)
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    లేకపోతే
      s->max_filesize = arg;
    ఆపు;
  స్థితి CURLOPT_IPRESOLVE:
    ఒకవేళ((arg < CURL_IPRESOLVE_WHATEVER) || (arg > CURL_IPRESOLVE_V6))
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    లేకపోతే
      s->ipver = (గుర్తులేని అక్షరం)arg;
    ఆపు;
  స్థితి CURLOPT_CONNECT_ONLY:
    ఒకవేళ(arg < 0 || arg > 2)
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    లేకపోతే {
      s->connect_only = !!arg;
      s->connect_only_ws = (arg == 2);
    }
    ఆపు;
#నిర్వచితమైతే USE_IPV6
  స్థితి CURLOPT_చిరునామా_SCOPE:
#ఒకవేళ SIZEOF_LONG > 4
    ఒకవేళ((గుర్తులేని దీర్ఘ)arg > UINT_MAX)
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    లేకపోతే
#అయితే_ముగింపు
    s->scope_id = (గుర్తులేని పూర్ణాంకం)arg;
    ఆపు;
#అయితే_ముగింపు
  స్థితి CURLOPT_TCP_KEEPIDLE:
    ఫలితం = విలువ_range(&arg, 0, 0, గరిష్ఠ_పూర్ణాంకం);
    ఒకవేళ(!ఫలితం)
      s->tcp_keepidle = (పూర్ణాంకం)arg;
    ఆపు;
  స్థితి CURLOPT_TCP_KEEPINTVL:
    ఫలితం = విలువ_range(&arg, 0, 0, గరిష్ఠ_పూర్ణాంకం);
    ఒకవేళ(!ఫలితం)
      s->tcp_keepintvl = (పూర్ణాంకం)arg;
    ఆపు;
  స్థితి CURLOPT_TCP_KEEPCNT:
    ఫలితం = విలువ_range(&arg, 0, 0, గరిష్ఠ_పూర్ణాంకం);
    ఒకవేళ(!ఫలితం)
      s->tcp_keepcnt = (పూర్ణాంకం)arg;
    ఆపు;
  స్థితి CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
    తిరిగి setopt_సమితి_timeout_ms(&s->happy_eyeballs_timeout, arg);
  స్థితి CURLOPT_UPKEEP_INTERVAL_MS:
    తిరిగి setopt_సమితి_timeout_ms(&s->upkeep_interval_ms, arg);
  స్థితి CURLOPT_MAXAGE_CONN:
    తిరిగి setopt_సమితి_timeout_sec(&s->conn_max_idle_ms, arg);
  స్థితి CURLOPT_MAXLIFETIME_CONN:
    తిరిగి setopt_సమితి_timeout_sec(&s->conn_max_age_ms, arg);
  స్థితి CURLOPT_DNS_USE_GLOBAL_CACHE:
    /* deprecated */
    ఆపు;
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode setopt_long_ssl(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                                దీర్ఘ arg)
{
#నిర్వచితమైతే USE_SSL
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం UserDefined *s = &దత్తాంశం->సమితి;
  ఎంపిక(option) {
  స్థితి CURLOPT_CA_CACHE_TIMEOUT:
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_CA_CACHE)) {
      ఫలితం = విలువ_range(&arg, -1, -1, గరిష్ఠ_పూర్ణాంకం);
      ఒకవేళ(!ఫలితం)
        s->ssl_ca_cache_timeout = (పూర్ణాంకం)arg;
    }
    లేకపోతే
      ఫలితం = CURLE_NOT_BUILT_లోపల;
    ఆపు;
  స్థితి CURLOPT_SSLVERSION:
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  స్థితి CURLOPT_PROXY_SSLVERSION:
#అయితే_ముగింపు
    తిరిగి Curl_setopt_SSLVERSION(దత్తాంశం, option, arg);
  స్థితి CURLOPT_SSL_FALSESTART:
    ఫలితం = CURLE_NOT_BUILT_లోపల;
    ఆపు;
  స్థితి CURLOPT_USE_SSL:
    ఒకవేళ((arg < CURLUSESSL_NONE) || (arg >= CURLUSESSL_చివరి))
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    లేకపోతే
      s->use_ssl = (గుర్తులేని అక్షరం)arg;
    ఆపు;
  స్థితి CURLOPT_SSL_OPTIONS:
    s->ssl.ssl_options = (గుర్తులేని అక్షరం)(arg & 0xff);
    ఆపు;
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  స్థితి CURLOPT_PROXY_SSL_OPTIONS:
    s->proxy_ssl.ssl_options = (గుర్తులేని అక్షరం)(arg & 0xff);
    ఆపు;
#అయితే_ముగింపు
  స్థితి CURLOPT_SSL_ENABLE_NPN:
    ఆపు;
  స్థితి CURLOPT_SSLENGINE_DEFAULT:
#నిర్వచితమైతే USE_OPENSSL
    ఒకవేళ(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      తిరిగి CURLE_NOT_BUILT_లోపల;
    దత్తాంశం->సమితి.engine_default = (arg == 1);
#లేకపోతే
    తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
    ఆపు;
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }
  తిరిగి ఫలితం;
#లేకపోతే  /* USE_SSL */
  (శూన్య)దత్తాంశం;
  (శూన్య)option;
  (శూన్య)arg;
  తిరిగి CURLE_UNKNOWN_OPTION;
#అయితే_ముగింపు /* !USE_SSL */
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
స్థిర శూన్య changeproxy(నిర్మాణం Curl_easy *దత్తాంశం)
{
  Curl_auth_digest_cleanup(&దత్తాంశం->state.proxydigest);
  memset(&దత్తాంశం->state.authproxy, 0, పరిమాణం(దత్తాంశం->state.authproxy));
}

స్థిర CURLcode setopt_long_proxy(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                                  దీర్ఘ arg)
{
  నిర్మాణం UserDefined *s = &దత్తాంశం->సమితి;

  ఎంపిక(option) {
  స్థితి CURLOPT_PROXYPORT:
    ఒకవేళ((arg < 0) || (arg > UINT16_MAX))
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    ఒకవేళ(arg != s->proxyport)
      changeproxy(దత్తాంశం);
    s->proxyport = (uint16_t)arg;
    ఆపు;
  స్థితి CURLOPT_PROXYAUTH:
    తిరిగి httpauth(దత్తాంశం, TRUE, (గుర్తులేని దీర్ఘ)arg);
  స్థితి CURLOPT_PROXYTYPE:
    ఒకవేళ((arg < CURLPROXY_HTTP) || (arg > CURLPROXY_HTTPS3))
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
#నిర్వచితం_కాకపోతే USE_PROXY_HTTP3
    ఒకవేళ(arg == CURLPROXY_HTTPS3)
      తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
    s->proxytype = (గుర్తులేని అక్షరం)arg;
    ఆపు;
  స్థితి CURLOPT_SOCKS5_AUTH: {
    గుర్తులేని దీర్ఘ auth = (గుర్తులేని దీర్ఘ)arg;
    ఒకవేళ(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI | CURLAUTH_ONLY))
      తిరిగి CURLE_NOT_BUILT_లోపల;
    ఒకవేళ((auth & CURLAUTH_ONLY) && !(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI)))
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    s->socks5auth = (గుర్తులేని అక్షరం)(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI));
    s->socks5_auth_only = !!(auth & CURLAUTH_ONLY);
    ఆపు;
  }
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }
  తిరిగి CURLE_OK;
}
#లేకపోతే
స్థిర CURLcode setopt_long_proxy(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                                  దీర్ఘ arg)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)option;
  (శూన్య)arg;
  తిరిగి CURLE_UNKNOWN_OPTION;
}
#అయితే_ముగింపు

స్థిర CURLcode setopt_long_http(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                                 దీర్ఘ arg)
{
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం UserDefined *s = &దత్తాంశం->సమితి;

  ఎంపిక(option) {
  స్థితి CURLOPT_FOLLOWLOCATION:
    ఒకవేళ((గుర్తులేని దీర్ఘ)arg > 3)
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    లేకపోతే
      s->http_follow_బహుళకం = (గుర్తులేని అక్షరం)arg;
    ఆపు;
  స్థితి CURLOPT_MAXREDIRS:
    ఫలితం = విలువ_range(&arg, -1, -1, 0x7fff);
    ఒకవేళ(!ఫలితం)
      s->maxredirs = (పొట్టి)arg;
    ఆపు;
  స్థితి CURLOPT_POSTREDIR:
    ఒకవేళ(arg < CURL_REDIR_GET_ALL)
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    లేకపోతే {
      s->post301 = !!(arg & CURL_REDIR_POST_301);
      s->post302 = !!(arg & CURL_REDIR_POST_302);
      s->post303 = !!(arg & CURL_REDIR_POST_303);
    }
    ఆపు;
  స్థితి CURLOPT_HEADEROPT:
    s->sep_శీర్షికలు = !!(arg & CURLHEADER_SEPARATE);
    ఆపు;
  స్థితి CURLOPT_HTTPAUTH:
    తిరిగి httpauth(దత్తాంశం, FALSE, (గుర్తులేని దీర్ఘ)arg);
  స్థితి CURLOPT_HTTP_సంస్కరణ:
    తిరిగి setopt_HTTP_సంస్కరణ(దత్తాంశం, arg);
  స్థితి CURLOPT_EXPECT_100_TIMEOUT_MS:
    ఫలితం = విలువ_range(&arg, 0, 0, 0xffff);
    ఒకవేళ(!ఫలితం)
      s->expect_100_timeout = (గుర్తులేని పొట్టి)arg;
    ఆపు;
  స్థితి CURLOPT_ప్రవాహం_బరువు:
#ఒకవేళ defined(USE_HTTP2) || defined(USE_HTTP3)
    ఒకవేళ((arg >= 1) && (arg <= 256))
      s->బరువు = (పూర్ణాంకం)arg;
    ఆపు;
#లేకపోతే
    ఫలితం = CURLE_NOT_BUILT_లోపల;
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTPSIG
  స్థితి CURLOPT_HTTPSIG_ALGORITHM:
    ఒకవేళ(arg != CURLHTTPSIG_NONE &&
       arg != CURLHTTPSIG_ED25519 &&
       arg != CURLHTTPSIG_HMAC_SHA256)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    s->httpsig_algorithm = (uint8_t)arg;
    ఒకవేళ(arg)
      s->httpauth = (uint32_t)CURLAUTH_HTTPSIG;
    లేకపోతే
      s->httpauth &= ~(uint32_t)CURLAUTH_HTTPSIG;
    ఆపు;
#అయితే_ముగింపు
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }
  తిరిగి ఫలితం;
#లేకపోతే
  (శూన్య)దత్తాంశం;
  (శూన్య)option;
  (శూన్య)arg;
  తిరిగి CURLE_UNKNOWN_OPTION;
#అయితే_ముగింపు
}

స్థిర CURLcode setopt_long_proto(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                                  దీర్ఘ arg)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం UserDefined *s = &దత్తాంశం->సమితి;

  ఎంపిక(option) {
#నిర్వచితం_కాకపోతే CURL_DISABLE_TFTP
  స్థితి CURLOPT_TFTP_BLKSIZE:
    ఫలితం = విలువ_range(&arg, 0, TFTP_BLKSIZE_MIN, TFTP_BLKSIZE_MAX);
    ఒకవేళ(!ఫలితం)
      s->tftp_blksize = (గుర్తులేని పొట్టి)arg;
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_NETRC
  స్థితి CURLOPT_NETRC:
    ఒకవేళ((arg < CURL_NETRC_IGNORED) || (arg >= CURL_NETRC_చివరి))
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    లేకపోతే
      s->use_netrc = (గుర్తులేని అక్షరం)arg;
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
  స్థితి CURLOPT_FTP_FILEMETHOD:
    ఒకవేళ((arg < CURLFTPMETHOD_DEFAULT) || (arg >= CURLFTPMETHOD_చివరి))
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    లేకపోతే
      s->ftp_filemethod = (గుర్తులేని అక్షరం)arg;
    ఆపు;
  స్థితి CURLOPT_FTP_SSL_CCC:
    ఒకవేళ((arg < CURLFTPSSL_CCC_NONE) || (arg >= CURLFTPSSL_CCC_చివరి))
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    లేకపోతే
      s->ftp_ccc = (గుర్తులేని అక్షరం)arg;
    ఆపు;
  స్థితి CURLOPT_FTPSSLAUTH:
    ఒకవేళ((arg < CURLFTPAUTH_DEFAULT) || (arg >= CURLFTPAUTH_చివరి))
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    లేకపోతే
      s->ftpsslauth = (గుర్తులేని అక్షరం)arg;
    ఆపు;
  స్థితి CURLOPT_ACCEPTTIMEOUT_MS:
    తిరిగి setopt_సమితి_timeout_ms(&s->accepttimeout, arg);
#అయితే_ముగింపు
#ఒకవేళ !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  స్థితి CURLOPT_FTP_CREATE_MISSING_DIRS:
    ఒకవేళ((arg < CURLFTP_CREATE_DIR_NONE) || (arg > CURLFTP_CREATE_DIR_RETRY))
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    లేకపోతే
      s->ftp_create_missing_dirs = (గుర్తులేని అక్షరం)arg;
    ఆపు;
  స్థితి CURLOPT_NEW_FILE_PERMS:
    ఒకవేళ((arg < 0) || (arg > 0777))
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    లేకపోతే
      s->new_file_perms = (గుర్తులేని పూర్ణాంకం)arg;
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_RTSP
  స్థితి CURLOPT_RTSP_REQUEST:
    తిరిగి setopt_RTSP_REQUEST(దత్తాంశం, arg);
  స్థితి CURLOPT_RTSP_CLIENT_CSEQ:
    ఫలితం = విలువ_range(&arg, 0, 0, గరిష్ఠ_పూర్ణాంకం);
    ఒకవేళ(!ఫలితం)
      దత్తాంశం->state.rtsp_తదుపరి_client_CSeq = (uint32_t)arg;
    ఆపు;
  స్థితి CURLOPT_RTSP_SERVER_CSEQ:
    ఫలితం = విలువ_range(&arg, 0, 0, గరిష్ఠ_పూర్ణాంకం);
    ఒకవేళ(!ఫలితం)
      దత్తాంశం->state.rtsp_తదుపరి_server_CSeq = (uint32_t)arg;
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSH
  స్థితి CURLOPT_SSH_AUTH_TYPES:
    s->ssh_auth_types = (uint32_t)arg;
    ఆపు;
  స్థితి CURLOPT_NEW_DIRECTORY_PERMS:
    ఒకవేళ((arg < 0) || (arg > 0777))
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    లేకపోతే
      s->new_directory_perms = (గుర్తులేని పూర్ణాంకం)arg;
    ఆపు;
#అయితే_ముగింపు
  స్థితి CURLOPT_PROTOCOLS:
    s->allowed_protocols = (curl_prot_t)arg;
    ఆపు;
  స్థితి CURLOPT_REDIR_PROTOCOLS:
    s->redir_protocols = (curl_prot_t)arg;
    ఆపు;
#నిర్వచితం_కాకపోతే CURL_DISABLE_WEBSOCKETS
  స్థితి CURLOPT_WS_OPTIONS:
    s->ws_raw_బహుళకం = !!(arg & CURLWS_RAW_బహుళకం);
    s->ws_no_auto_pong = !!(arg & CURLWS_NOAUTOPONG);
    ఆపు;
#అయితే_ముగింపు
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode setopt_long_misc(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                                 దీర్ఘ arg)
{
  నిర్మాణం UserDefined *s = &దత్తాంశం->సమితి;

  ఎంపిక(option) {
  స్థితి CURLOPT_TIMECONDITION:
    ఒకవేళ((arg < CURL_TIMECOND_NONE) || (arg >= CURL_TIMECOND_చివరి))
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    s->timecondition = (గుర్తులేని అక్షరం)arg;
    ఆపు;
  స్థితి CURLOPT_TIMEVALUE:
    s->timevalue = (time_t)arg;
    ఆపు;
  స్థితి CURLOPT_POSTFIELDSIZE:
    ఒకవేళ(arg < -1)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    ఒకవేళ(s->postfieldsize < arg && s->str_copypostfields) {
      curlx_safefree(s->str_copypostfields);
      s->postfields = ఖాళీ;
    }
    s->postfieldsize = arg;
    ఆపు;
  స్థితి CURLOPT_INFILESIZE:
    ఒకవేళ(arg < -1)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    s->filesize = arg;
    ఆపు;
  స్థితి CURLOPT_RESUME_FROM:
    ఒకవేళ(arg < -1)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    s->సమితి_resume_from = arg;
    ఆపు;
  స్థితి CURLOPT_UPLOAD_FLAGS:
    s->upload_flags = (గుర్తులేని అక్షరం)arg;
    ఆపు;
#నిర్వచితం_కాకపోతే CURL_DISABLE_MIME
  స్థితి CURLOPT_MIME_OPTIONS:
    s->mime_formescape = !!(arg & CURLMIMEOPT_FORMESCAPE);
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HSTS
  స్థితి CURLOPT_HSTS_CTRL:
    ఒకవేళ(arg & CURLHSTS_ENABLE) {
      ఒకవేళ(!దత్తాంశం->hsts) {
        దత్తాంశం->hsts = Curl_hsts_init();
        ఒకవేళ(!దత్తాంశం->hsts)
          తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
    }
    లేకపోతే ఒకవేళ(!దత్తాంశం->share || !దత్తాంశం->share->hsts) {
      /* throw away the HSTS cache unless shared */
      Curl_hsts_cleanup(&దత్తాంశం->hsts);
      /* flush all the entries */
      curl_slist_free_all(దత్తాంశం->state.hstslist);
      దత్తాంశం->state.hstslist = ఖాళీ;
    }
    లేకపోతే
      /* detach from shared HSTS cache without freeing it */
      దత్తాంశం->hsts = ఖాళీ;
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_ALTSVC
  స్థితి CURLOPT_ALTSVC_CTRL:
    తిరిగి Curl_altsvc_ctrl(దత్తాంశం, arg);
#అయితే_ముగింపు
#ఒకవేళ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  స్థితి CURLOPT_GSSAPI_DELEGATION:
    s->gssapi_delegation = (గుర్తులేని అక్షరం)arg &
      (CURLGSSAPI_DELEGATION_POLICY_FLAG | CURLGSSAPI_DELEGATION_FLAG);
    ఆపు;
#అయితే_ముగింపు
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode setopt_long(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                            దీర్ఘ arg)
{
  రకనిర్వచనం CURLcode (*setoptfunc)(నిర్మాణం Curl_easy *దత్తాంశం,
                                 CURLoption option, దీర్ఘ arg);
  స్థిర స్థిరరాశి setoptfunc setopt_call[] = {
    setopt_long_bool,
    setopt_long_net,
    setopt_long_http,
    setopt_long_proxy,
    setopt_long_ssl,
    setopt_long_proto,
    setopt_long_misc
  };
  size_t i;

  కొరకు(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode ఫలితం = setopt_call[i](దత్తాంశం, option, arg);
    ఒకవేళ(ఫలితం != CURLE_UNKNOWN_OPTION)
      తిరిగి ఫలితం;
  }
  తిరిగి CURLE_UNKNOWN_OPTION;
}

స్థిర CURLcode setopt_slist(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                             నిర్మాణం curl_slist *slist)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం UserDefined *s = &దత్తాంశం->సమితి;
  ఎంపిక(option) {
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  స్థితి CURLOPT_PROXYHEADER:
    /*
     * Set a list with proxy headers to use (or replace internals with)
     *
     * Since CURLOPT_HTTPHEADER was the only way to set HTTP headers for a
     * long time we remain doing it this way until CURLOPT_PROXYHEADER is
     * used. As soon as this option has been used, if set to anything but
     * NULL, custom headers for proxies are only picked from this list.
     *
     * Set this option to NULL to restore the previous behavior.
     */
    s->proxyheaders = slist;
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  స్థితి CURLOPT_HTTP200ALIASES:
    /*
     * Set a list of aliases for HTTP 200 in response header
     */
    s->http200aliases = slist;
    ఆపు;
#అయితే_ముగింపు
#ఒకవేళ !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  స్థితి CURLOPT_POSTQUOTE:
    /*
     * List of RAW FTP commands to use after a transfer
     */
    s->postquote = slist;
    ఆపు;
  స్థితి CURLOPT_PREQUOTE:
    /*
     * List of RAW FTP commands to use prior to RETR (Wesley Laxton)
     */
    s->prequote = slist;
    ఆపు;
  స్థితి CURLOPT_QUOTE:
    /*
     * List of RAW FTP commands to use before a transfer
     */
    s->quote = slist;
    ఆపు;
#అయితే_ముగింపు
  స్థితి CURLOPT_RESOLVE:
    /*
     * List of HOST:PORT:[addresses] strings to populate the DNS cache with
     * Entries added this way will remain in the cache until explicitly
     * removed or the handle is cleaned up.
     *
     * Prefix the HOST with plus sign (+) to have the entry expire like
     * automatically added entries.
     *
     * Prefix the HOST with dash (-) to _remove_ the entry from the cache.
     *
     * This API can remove any entry from the DNS cache, but only entries
     * that are not actually in use right now will be pruned immediately.
     */
    s->resolve = slist;
    దత్తాంశం->state.resolve = s->resolve;
    ఆపు;
#ఒకవేళ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MIME)
  స్థితి CURLOPT_HTTPHEADER:
    /*
     * Set a list with HTTP headers to use (or replace internals with)
     */
    s->శీర్షికలు = slist;
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_TELNET
  స్థితి CURLOPT_TELNETOPTIONS:
    /*
     * Set a linked list of telnet options
     */
    s->telnet_options = slist;
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_SMTP
  స్థితి CURLOPT_MAIL_RCPT:
    /* Set the list of mail recipients */
    s->mail_rcpt = slist;
    ఆపు;
#అయితే_ముగింపు
  స్థితి CURLOPT_CONNECT_గమ్యం:
    s->connect_గమ్యం = slist;
    ఆపు;
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }
  తిరిగి ఫలితం;
}

#ఒకవేళ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#నిర్వచితం_కాకపోతే CURL_DISABLE_MIME
స్థిర CURLcode setopt_mimepost(నిర్మాణం Curl_easy *దత్తాంశం, curl_mime *mimep)
{
  /*
   * Set to make us do MIME POST
   */
  CURLcode ఫలితం;
  నిర్మాణం UserDefined *s = &దత్తాంశం->సమితి;
  ఒకవేళ(!s->mimepostp) {
    s->mimepostp = curlx_malloc(పరిమాణం(*s->mimepostp));
    ఒకవేళ(!s->mimepostp)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    Curl_mime_initpart(s->mimepostp);
  }

  ఫలితం = Curl_mime_సమితి_subparts(s->mimepostp, mimep, FALSE);
  ఒకవేళ(!ఫలితం) {
    s->పద్ధతి = HTTPREQ_POST_MIME;
    s->opt_no_body = FALSE; /* this is implied */
#నిర్వచితం_కాకపోతే CURL_DISABLE_FORM_API
    Curl_mime_cleanpart(దత్తాంశం->state.formp);
    curlx_safefree(దత్తాంశం->state.formp);
    దత్తాంశం->state.mimepost = ఖాళీ;
#అయితే_ముగింపు
  }
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు /* !CURL_DISABLE_MIME */
#అయితే_ముగింపు /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */

స్థిర CURLcode setopt_share(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_share *సమితి)
{
  CURLcode ఫలితం;

  ఒకవేళ(దత్తాంశం->conn) {
    /* As this handle already has a connection attached, changing share now
       would be complicated and error-prone */
    infof(దత్తాంశం, "Cannot change share object while in use");
    ఫలితం = CURLE_BAD_FUNCTION_వాదం;
  }
  లేకపోతే {
    /* disconnect from old share, if any and possible */
    ఫలితం = Curl_share_easy_unlink(దత్తాంశం);
    ఒకవేళ(!ఫలితం && GOOD_SHARE_HANDLE(సమితి))
      /* use new share if it set */
      ఫలితం = Curl_share_easy_link(దత్తాంశం, సమితి);
  }
  తిరిగి ఫలితం;
}

/* assorted pointer type arguments */
స్థిర CURLcode setopt_సూచికలు(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                                va_list param)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం UserDefined *s = &దత్తాంశం->సమితి;
  ఎంపిక(option) {
  స్థితి CURLOPT_CURLU:
    /*
     * pass CURLU to set URL
     */
    Curl_bufref_free(&దత్తాంశం->state.url);
    CURL_EASY_STR_CLEAR(దత్తాంశం, STRING_సమితి_URL);
    s->uh = va_arg(param, CURLU *);
    ఆపు;
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
#నిర్వచితం_కాకపోతే CURL_DISABLE_FORM_API
  స్థితి CURLOPT_HTTPPOST:
    /*
     * Set to make us do HTTP POST. Legacy API-style.
     */
    s->httppost = va_arg(param, నిర్మాణం curl_httppost *);
    s->పద్ధతి = HTTPREQ_POST_FORM;
    s->opt_no_body = FALSE; /* this is implied */
    Curl_mime_cleanpart(దత్తాంశం->state.formp);
    curlx_safefree(దత్తాంశం->state.formp);
    దత్తాంశం->state.mimepost = ఖాళీ;
    ఆపు;
#అయితే_ముగింపు /* !CURL_DISABLE_FORM_API */
#అయితే_ముగింపు /* !CURL_DISABLE_HTTP */
#ఒకవేళ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#నిర్వచితం_కాకపోతే CURL_DISABLE_MIME
  స్థితి CURLOPT_MIMEPOST:
    ఫలితం = setopt_mimepost(దత్తాంశం, va_arg(param, curl_mime *));
    ఆపు;
#అయితే_ముగింపు /* !CURL_DISABLE_MIME */
#అయితే_ముగింపు /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */
  స్థితి CURLOPT_STDERR:
    /*
     * Set to a FILE * that should receive all error writes. This
     * defaults to stderr for normal operations.
     */
    s->err = va_arg(param, FILE *);
    ఒకవేళ(!s->err)
      s->err = stderr;
    ఆపు;
  స్థితి CURLOPT_SHARE:
    తిరిగి setopt_share(దత్తాంశం, va_arg(param, నిర్మాణం Curl_share *));

  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }
  తిరిగి ఫలితం;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_COOKIES
స్థిర CURLcode cookielist(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *ptr)
{
  CURLcode ఫలితం = CURLE_OK;
  ఒకవేళ(!ptr)
    తిరిగి CURLE_OK;

  ఒకవేళ(curl_strequal(ptr, "ALL")) {
    /* clear all cookies */
    Curl_share_తాళం(దత్తాంశం, CURL_తాళం_దత్తాంశం_COOKIE, CURL_తాళం_ACCESS_SINGLE);
    Curl_cookie_clearall(దత్తాంశం->cookies);
    Curl_share_unlock(దత్తాంశం, CURL_తాళం_దత్తాంశం_COOKIE);
  }
  లేకపోతే ఒకవేళ(curl_strequal(ptr, "SESS")) {
    /* clear session cookies */
    Curl_share_తాళం(దత్తాంశం, CURL_తాళం_దత్తాంశం_COOKIE, CURL_తాళం_ACCESS_SINGLE);
    Curl_cookie_clearsess(దత్తాంశం->cookies);
    Curl_share_unlock(దత్తాంశం, CURL_తాళం_దత్తాంశం_COOKIE);
  }
  లేకపోతే ఒకవేళ(curl_strequal(ptr, "FLUSH")) {
    /* flush cookies to file, takes care of the locking */
    Curl_flush_cookies(దత్తాంశం, FALSE);
  }
  లేకపోతే ఒకవేళ(curl_strequal(ptr, "RELOAD")) {
    /* reload cookies from file */
    తిరిగి Curl_cookie_loadfiles(దత్తాంశం, COOKIE_NOPSL |
                                 (దత్తాంశం->సమితి.cookiesession ?
                                  COOKIE_NOSESSION : 0));
  }
  లేకపోతే {
    ఒకవేళ(!దత్తాంశం->cookies) {
      /* if cookie engine was not running, activate it */
      దత్తాంశం->cookies = Curl_cookie_init();
      ఒకవేళ(!దత్తాంశం->cookies)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      దత్తాంశం->state.cookie_engine = TRUE;
    }

    /* general protection against mistakes and abuse */
    ఒకవేళ(అక్షరశ్రేణి_పొడవు(ptr) > CURL_MAX_ఆదానం_నిడివి)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;

    /* Adding these cookies without the PSL check, because the PSL is not
       initialized until *perform() time, and this might be called before
       that */
    Curl_share_తాళం(దత్తాంశం, CURL_తాళం_దత్తాంశం_COOKIE, CURL_తాళం_ACCESS_SINGLE);
    ఒకవేళ(checkprefix("Set-Cookie:", ptr))
      /* HTTP Header format line */
      ఫలితం = Curl_cookie_కూడు(దత్తాంశం, దత్తాంశం->cookies, ptr + 11,
                               ఖాళీ, ఖాళీ,
                               COOKIE_HTTPHEADER | COOKIE_SECURE |
                               COOKIE_NOPSL);
    లేకపోతే
      /* Netscape format line */
      ఫలితం = Curl_cookie_కూడు(దత్తాంశం, దత్తాంశం->cookies, ptr, ఖాళీ,
                               ఖాళీ, COOKIE_SECURE | COOKIE_NOPSL);
    Curl_share_unlock(దత్తాంశం, CURL_తాళం_దత్తాంశం_COOKIE);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode cookiefile(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *ptr)
{
  /*
   * Set cookie file to read and parse. Can be used multiple times.
   */
  ఒకవేళ(ptr) {
    నిర్మాణం curl_slist *cl;
    /* general protection against mistakes and abuse */
    ఒకవేళ(అక్షరశ్రేణి_పొడవు(ptr) > CURL_MAX_ఆదానం_నిడివి)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    /* append the cookie filename to the list of filenames, and deal with
       them later */
    cl = curl_slist_append(దత్తాంశం->state.cookielist, ptr);
    ఒకవేళ(!cl) {
      curl_slist_free_all(దత్తాంశం->state.cookielist);
      దత్తాంశం->state.cookielist = ఖాళీ;
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    దత్తాంశం->state.cookielist = cl; /* store the list for later use */
  }
  లేకపోతే {
    /* clear the list of cookie files */
    curl_slist_free_all(దత్తాంశం->state.cookielist);
    దత్తాంశం->state.cookielist = ఖాళీ;

    ఒకవేళ(!దత్తాంశం->share || !దత్తాంశం->share->cookies) {
      /* throw away all existing cookies if this is not a shared cookie
         container */
      Curl_cookie_clearall(దత్తాంశం->cookies);
      Curl_cookie_cleanup(దత్తాంశం->cookies);
    }
    /* disable the cookie engine */
    దత్తాంశం->cookies = ఖాళీ;
  }
  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
స్థిర CURLcode setproxy(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *proxy)
{
  స్థిరరాశి అక్షరం *str = CURL_EASY_STR(దత్తాంశం, STRING_PROXY);
  ఒకవేళ(str && proxy &&
     /* there was one set, is this a new one? */
     !strcmp(str, proxy))
    తిరిగి CURLE_OK; /* same one as before */

  changeproxy(దత్తాంశం);
  తిరిగి Curl_setstropt(దత్తాంశం, STRING_PROXY, proxy);
}

స్థిర CURLcode setopt_cptr_proxy(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                                  అక్షరం *ptr)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం UserDefined *s = &దత్తాంశం->సమితి;
  ఎంపిక(option) {
  స్థితి CURLOPT_PROXYUSERPWD: {
    /*
     * user:password needed to use the proxy
     */
    అక్షరం *u = ఖాళీ;
    అక్షరం *p = ఖాళీ;
    ఫలితం = setstropt_userpwd(ptr, &u, &p);

    /* URL decode the components */
    ఒకవేళ(!ఫలితం) {
      అక్షరం *str = ఖాళీ;
      CURL_EASY_STR_CLEAR(దత్తాంశం, STRING_PROXYUSERNAME);
      CURL_EASY_STR_CLEAR(దత్తాంశం, STRING_PROXYPASSWORD);
      ఒకవేళ(u) {
        ఫలితం = Curl_urldecode(u, 0, &str, ఖాళీ, REJECT_ZERO);
        ఒకవేళ(!ఫలితం)
          ఫలితం = Curl_u8_strset_setn(&s->అక్షరశ్రేణులు, STRING_PROXYUSERNAME, str);
      }
      ఒకవేళ(!ఫలితం && p) {
        str = ఖాళీ;
        ఫలితం = Curl_urldecode(p, 0, &str, ఖాళీ, REJECT_ZERO);
        ఒకవేళ(!ఫలితం)
          ఫలితం = Curl_u8_strset_setn(&s->అక్షరశ్రేణులు, STRING_PROXYPASSWORD, str);
      }
    }
    curlx_free(u);
    curlx_strzero(p);
    curlx_free(p);
    ఆపు;
  }
  స్థితి CURLOPT_PROXYUSERNAME:
    /*
     * authentication username to use in the operation
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_PROXYUSERNAME, ptr);

  స్థితి CURLOPT_PROXYPASSWORD:
    /*
     * authentication password to use in the operation
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_PROXYPASSWORD, ptr);

  స్థితి CURLOPT_NOPROXY:
    /*
     * proxy exception list
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_NOPROXY, ptr);
  స్థితి CURLOPT_PROXY_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use for proxy
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_CERT_PROXY, ptr);
  స్థితి CURLOPT_PROXY_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use for proxy
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_CERT_TYPE_PROXY, ptr);
  స్థితి CURLOPT_PROXY_SSLKEY:
    /*
     * String that holds filename of the SSL key to use for proxy
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_కీలకం_PROXY, ptr);
  స్థితి CURLOPT_PROXY_KEYPASSWD:
    /*
     * String that holds the SSL private key password for proxy.
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_కీలకం_PASSWD_PROXY, ptr);
  స్థితి CURLOPT_PROXY_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use for proxy
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_కీలకం_TYPE_PROXY, ptr);
  స్థితి CURLOPT_PROXY_SSL_CIPHER_LIST:
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_CIPHER_LIST)) {
      /* set a list of cipher we want to use in the SSL connection for proxy */
      తిరిగి Curl_setstropt(దత్తాంశం, STRING_SSL_CIPHER_LIST_PROXY, ptr);
    }
    లేకపోతే
      తిరిగి CURLE_NOT_BUILT_లోపల;
  స్థితి CURLOPT_PROXY_TLS13_CIPHERS:
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites for proxy */
      తిరిగి Curl_setstropt(దత్తాంశం, STRING_SSL_CIPHER13_LIST_PROXY, ptr);
    లేకపోతే
      తిరిగి CURLE_NOT_BUILT_లోపల;
  స్థితి CURLOPT_PROXY:
    /*
     * Set proxy server:port to use as proxy.
     *
     * If the proxy is set to "" (and CURLOPT_PRE_PROXY is set to "" or NULL)
     * we explicitly say that we do not want to use a proxy (even though there
     * might be environment variables saying so).
     *
     * Setting it to NULL, means no proxy but allows the environment variables
     * to decide for us (if CURLOPT_PRE_PROXY setting it to NULL).
     */
    తిరిగి setproxy(దత్తాంశం, ptr);
  స్థితి CURLOPT_PRE_PROXY:
    /*
     * Set proxy server:port to use as SOCKS proxy.
     *
     * If the proxy is set to "" or NULL we explicitly say that we do not want
     * to use the socks proxy.
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_PRE_PROXY, ptr);
  స్థితి CURLOPT_SOCKS5_GSSAPI_SERVICE:
  స్థితి CURLOPT_PROXY_SERVICE_NAME:
    /*
     * Set proxy authentication service name for Kerberos 5 and SPNEGO
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_PROXY_SERVICE_NAME, ptr);
  స్థితి CURLOPT_PROXY_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
#నిర్వచితమైతే USE_SSL
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_PINNEDPUBKEY))
      తిరిగి Curl_setstropt(దత్తాంశం, STRING_SSL_PINNEDPUBLICKEY_PROXY, ptr);
#అయితే_ముగింపు
    తిరిగి CURLE_NOT_BUILT_లోపల;

  స్థితి CURLOPT_HAPROXY_CLIENT_IP:
    /*
     * Set the client IP to send through HAProxy PROXY protocol
     */
    ఫలితం = Curl_setstropt(దత్తాంశం, STRING_HAPROXY_CLIENT_IP, ptr);

    /* enable the HAProxy protocol if an IP is provided */
    s->haproxyprotocol = !!CURL_EASY_STR(దత్తాంశం, STRING_HAPROXY_CLIENT_IP);
    ఆపు;
  స్థితి CURLOPT_PROXY_CAINFO:
    /*
     * Set CA info SSL connection for proxy. Specify filename of the
     * CA certificate
     */
    ఫలితం = Curl_setstropt(దత్తాంశం, STRING_SSL_CAFILE_PROXY, ptr);
    ఒకవేళ(!ఫలితం)
      s->proxy_ssl.custom_cafile = TRUE;
    తిరిగి ఫలితం;
  స్థితి CURLOPT_PROXY_CRLFILE:
    /*
     * Set CRL file info for SSL connection for proxy. Specify filename of the
     * CRL to check certificates revocation
     */
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_CRLFILE))
      తిరిగి Curl_setstropt(దత్తాంశం, STRING_SSL_CRLFILE_PROXY, ptr);
    తిరిగి CURLE_NOT_BUILT_లోపల;
  స్థితి CURLOPT_PROXY_ISSUERCERT:
    /*
     * Set Issuer certificate file to check certificates issuer
     */
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_ISSUERCERT))
      తిరిగి Curl_setstropt(దత్తాంశం, STRING_SSL_ISSUERCERT_PROXY, ptr);
    తిరిగి CURLE_NOT_BUILT_లోపల;
  స్థితి CURLOPT_PROXY_CAPATH:
    /*
     * Set CA path info for SSL connection proxy. Specify directory name of the
     * CA certificates which have been prepared using openssl c_rehash utility.
     */
#నిర్వచితమైతే USE_SSL
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_CA_మార్గం)) {
      /* This does not work on Windows. */
      ఫలితం = Curl_setstropt(దత్తాంశం, STRING_SSL_CAPATH_PROXY, ptr);
      ఒకవేళ(!ఫలితం)
        s->proxy_ssl.custom_capath = TRUE;
      తిరిగి ఫలితం;
    }
#అయితే_ముగింపు
    తిరిగి CURLE_NOT_BUILT_లోపల;
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు

#ఒకవేళ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
/*
 * A string with POST data. Makes curl HTTP POST. Even if it is NULL. If
 * needed, CURLOPT_POSTFIELDSIZE must have been set prior to
 * CURLOPT_COPYPOSTFIELDS and not altered later.
 */
స్థిర CURLcode setopt_copypostfields(స్థిరరాశి అక్షరం *ptr, నిర్మాణం UserDefined *s)
{
  CURLcode ఫలితం = CURLE_OK;
  ఒకవేళ(s->postfieldsize < -1)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  ఒకవేళ(!ptr || s->postfieldsize == -1) {
    ఒకవేళ(ptr && (అక్షరశ్రేణి_పొడవు(ptr) > CURL_MAX_ఆదానం_నిడివి))
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    curlx_safefree(s->str_copypostfields);
    ఒకవేళ(ptr) {
      s->str_copypostfields = curlx_strdup(ptr);
      ఒకవేళ(!s->str_copypostfields)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }
  లేకపోతే {
    size_t pflen = curlx_sotouz_range(s->postfieldsize, 0, SIZE_MAX);
    ఒకవేళ(pflen == SIZE_MAX)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    లేకపోతే {
      /* Allocate even when size == 0. This satisfies the need of possible
         later address compare to detect the COPYPOSTFIELDS mode, and to mark
         that postfields is used rather than read function or form data. */
      అక్షరం *p = curlx_memdup0(ptr, pflen);
      ఒకవేళ(!p)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      లేకపోతే {
        curlx_free(s->str_copypostfields);
        s->str_copypostfields = p;
      }
    }
  }

  s->postfields = s->str_copypostfields;
  s->పద్ధతి = HTTPREQ_POST;
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు

#నిర్వచితమైతే USE_ECH
స్థిర CURLcode setopt_ech(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *ptr)
{
  నిర్మాణం UserDefined *s = &దత్తాంశం->సమితి;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!ptr || !strcmp(ptr, "false"))
    s->ssl.ech = CURLECH_DISABLE;
  లేకపోతే {
    size_t plen = అక్షరశ్రేణి_పొడవు(ptr);
    ఒకవేళ(plen > CURL_MAX_ఆదానం_నిడివి)
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    లేకపోతే {
      ఒకవేళ(!strcmp(ptr, "grease"))
        s->ssl.ech = CURLECH_GREASE;
      లేకపోతే ఒకవేళ(!strcmp(ptr, "true"))
        s->ssl.ech = CURLECH_ENABLE;
      లేకపోతే ఒకవేళ(!strcmp(ptr, "hard"))
        s->ssl.ech = CURLECH_HARD;
      లేకపోతే ఒకవేళ(plen > 4 && !అక్షరశ్రేణి_పోలిక(ptr, "ecl:", 4)) {
        ఒకవేళ(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        ఫలితం = Curl_setstropt(దత్తాంశం, STRING_ECH_CONFIG, ptr + 4);
      }
      లేకపోతే ఒకవేళ(plen > 3 && !అక్షరశ్రేణి_పోలిక(ptr, "pn:", 3)) {
        ఒకవేళ(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        ఫలితం = Curl_setstropt(దత్తాంశం, STRING_ECH_ప్రజా, ptr + 3);
      }
      లేకపోతే
        ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    }
  }
  తిరిగి ఫలితం;
}
#లేకపోతే
#నిర్వచించు setopt_ech(x, y) CURLE_NOT_BUILT_లోపల
#అయితే_ముగింపు

#ఒకవేళ defined(USE_SSL) || defined(USE_SSH)
/* One of the options is used for both TLS and SSH */
స్థిర CURLcode setopt_cptr_ssl(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                                అక్షరం *ptr)
{
  CURLcode ఫలితం = CURLE_OK;

  ఎంపిక(option) {
  స్థితి CURLOPT_KEYPASSWD:
    /*
     * String that holds the SSL or SSH private key password.
     */
    ఫలితం = Curl_setstropt(దత్తాంశం, STRING_కీలకం_PASSWD, ptr);
    ఆపు;
#నిర్వచితమైతే USE_SSL
  స్థితి CURLOPT_CAINFO:
    /*
     * Set CA info for SSL connection. Specify filename of the CA certificate
     */
    ఫలితం = Curl_setstropt(దత్తాంశం, STRING_SSL_CAFILE, ptr);
    ఒకవేళ(!ఫలితం)
      దత్తాంశం->సమితి.ssl.custom_cafile = TRUE;
    తిరిగి ఫలితం;
  స్థితి CURLOPT_CAPATH:
    /*
     * Set CA path info for SSL connection. Specify directory name of the CA
     * certificates which have been prepared using openssl c_rehash utility.
     */
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_CA_మార్గం)) {
      /* This does not work on Windows. */
      ఫలితం = Curl_setstropt(దత్తాంశం, STRING_SSL_CAPATH, ptr);
      ఒకవేళ(!ఫలితం)
        దత్తాంశం->సమితి.ssl.custom_capath = TRUE;
      తిరిగి ఫలితం;
    }
    తిరిగి CURLE_NOT_BUILT_లోపల;
  స్థితి CURLOPT_CRLFILE:
    /*
     * Set CRL file info for SSL connection. Specify filename of the CRL
     * to check certificates revocation
     */
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_CRLFILE))
      తిరిగి Curl_setstropt(దత్తాంశం, STRING_SSL_CRLFILE, ptr);
    తిరిగి CURLE_NOT_BUILT_లోపల;
  స్థితి CURLOPT_SSL_CIPHER_LIST:
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_CIPHER_LIST))
      /* set a list of cipher we want to use in the SSL connection */
      తిరిగి Curl_setstropt(దత్తాంశం, STRING_SSL_CIPHER_LIST, ptr);
    లేకపోతే
      తిరిగి CURLE_NOT_BUILT_లోపల;
  స్థితి CURLOPT_TLS13_CIPHERS:
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites */
      తిరిగి Curl_setstropt(దత్తాంశం, STRING_SSL_CIPHER13_LIST, ptr);
    లేకపోతే
      తిరిగి CURLE_NOT_BUILT_లోపల;
  స్థితి CURLOPT_RANDOM_FILE:
    ఆపు;
  స్థితి CURLOPT_EGDSOCKET:
    ఆపు;
  స్థితి CURLOPT_SSL_CTX_దత్తాంశం:
    /*
     * Set an SSL_CTX callback parameter pointer
     */
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_SSL_CTX)) {
      దత్తాంశం->సమితి.ssl_fsslctxp = ptr;
      ఆపు;
    }
    లేకపోతే
      తిరిగి CURLE_NOT_BUILT_లోపల;
  స్థితి CURLOPT_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_CERT, ptr);
  స్థితి CURLOPT_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_CERT_TYPE, ptr);
  స్థితి CURLOPT_SSLKEY:
    /*
     * String that holds filename of the SSL key to use
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_కీలకం, ptr);
  స్థితి CURLOPT_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_కీలకం_TYPE, ptr);
  స్థితి CURLOPT_SSLENGINE:
    /*
     * String that holds the SSL crypto engine.
     */
    ఒకవేళ(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      తిరిగి CURLE_NOT_BUILT_లోపల;
    ఫలితం = Curl_setstropt(దత్తాంశం, STRING_SSL_ENGINE, ptr);
    ఆపు;
  స్థితి CURLOPT_ISSUERCERT:
    /*
     * Set Issuer certificate file
     * to check certificates issuer
     */
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_ISSUERCERT))
      తిరిగి Curl_setstropt(దత్తాంశం, STRING_SSL_ISSUERCERT, ptr);
    తిరిగి CURLE_NOT_BUILT_లోపల;
  స్థితి CURLOPT_SSL_EC_CURVES:
    /*
     * Set accepted curves in SSL connection setup.
     * Specify colon-delimited list of curve algorithm names.
     */
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_SSL_EC_CURVES))
      తిరిగి Curl_setstropt(దత్తాంశం, STRING_SSL_EC_CURVES, ptr);
    తిరిగి CURLE_NOT_BUILT_లోపల;
  స్థితి CURLOPT_SSL_SIGNATURE_ALGORITHMS:
    /*
     * Set accepted signature algorithms.
     * Specify colon-delimited list of signature scheme names.
     */
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_SIGNATURE_ALGORITHMS))
      తిరిగి Curl_setstropt(దత్తాంశం, STRING_SSL_SIGNATURE_ALGORITHMS, ptr);
    తిరిగి CURLE_NOT_BUILT_లోపల;
  స్థితి CURLOPT_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_PINNEDPUBKEY))
      తిరిగి Curl_setstropt(దత్తాంశం, STRING_SSL_PINNEDPUBLICKEY, ptr);
    తిరిగి CURLE_NOT_BUILT_లోపల;
  స్థితి CURLOPT_ECH:
    తిరిగి setopt_ech(దత్తాంశం, ptr);
#అయితే_ముగింపు
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు

#ఒకవేళ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
స్థిర CURLcode setopt_cptr_http_mqtt(నిర్మాణం Curl_easy *దత్తాంశం,
                                      CURLoption option, అక్షరం *ptr)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం UserDefined *s = &దత్తాంశం->సమితి;

  ఎంపిక(option) {
  స్థితి CURLOPT_COPYPOSTFIELDS:
    తిరిగి setopt_copypostfields(ptr, s);

  స్థితి CURLOPT_POSTFIELDS:
    /*
     * Like above, but use static data instead of copying it.
     */
    s->postfields = ptr;
    /* Release old copied data. */
    curlx_safefree(s->str_copypostfields);
    s->పద్ధతి = HTTPREQ_POST;
    ఆపు;

#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  స్థితి CURLOPT_TRAILERDATA:
    s->trailer_దత్తాంశం = ptr;
    ఆపు;
  స్థితి CURLOPT_ACCEPT_ENCODING:
    /*
     * String to use at the value of Accept-Encoding header.
     *
     * If the encoding is set to "" we use an Accept-Encoding header that
     * encompasses all the encodings we support.
     * If the encoding is set to NULL we do not send an Accept-Encoding header
     * and ignore any received Content-Encoding header.
     *
     */
    ఒకవేళ(ptr && !*ptr) {
      ptr = Curl_get_content_encodings();
      ఒకవేళ(ptr)
        ఫలితం = CURL_EASY_STR_SETN(దత్తాంశం, STRING_ENCODING, ptr);
      లేకపోతే
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      తిరిగి ఫలితం;
    }
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_ENCODING, ptr);

#నిర్వచితం_కాకపోతే CURL_DISABLE_AWS
  స్థితి CURLOPT_AWS_SIGV4:
    /*
     * String that is merged to some authentication
     * parameters are used by the algorithm.
     */
    ఫలితం = Curl_setstropt(దత్తాంశం, STRING_AWS_SIGV4, ptr);
    /*
     * Basic has been set by default; it needs to be unset here.
     */
    ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_AWS_SIGV4))
      s->httpauth = CURLAUTH_AWS_SIGV4;
    లేకపోతే
      s->httpauth &= ~(uint32_t)CURLAUTH_AWS_SIGV4;
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTPSIG
  స్థితి CURLOPT_HTTPSIG_కీలకం:
    ఫలితం = Curl_setstropt(దత్తాంశం, STRING_HTTPSIG_కీలకం, ptr);
    ఆపు;
  స్థితి CURLOPT_HTTPSIG_KEYID:
    ఫలితం = Curl_setstropt(దత్తాంశం, STRING_HTTPSIG_KEYID, ptr);
    ఆపు;
  స్థితి CURLOPT_HTTPSIG_శీర్షికలు:
    ఫలితం = Curl_setstropt(దత్తాంశం, STRING_HTTPSIG_శీర్షికలు, ptr);
    ఆపు;
#అయితే_ముగింపు
  స్థితి CURLOPT_REFERER: {
    /*
     * String to set in the HTTP Referer: field.
     */
    నిర్మాణం bufref *oldref = &దత్తాంశం->state.referer;
    /* free the old after the storing the new in case the input is actually
       pointing back to this */
    ఫలితం = Curl_setstropt(దత్తాంశం, STRING_సమితి_REFERER, ptr);
    Curl_bufref_free(oldref);
    ఆపు;
  }

  స్థితి CURLOPT_USERAGENT:
    /*
     * String to use in the HTTP User-Agent field
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_USERAGENT, ptr);

#నిర్వచితం_కాకపోతే CURL_DISABLE_COOKIES
  స్థితి CURLOPT_COOKIE:
    /*
     * Cookie string to send to the remote server in the request.
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_COOKIE, ptr);

  స్థితి CURLOPT_COOKIEFILE:
    తిరిగి cookiefile(దత్తాంశం, ptr);

  స్థితి CURLOPT_COOKIEJAR:
    /*
     * Set cookie filename to dump all cookies to when we are done.
     */
    ఫలితం = Curl_setstropt(దత్తాంశం, STRING_COOKIEJAR, ptr);
    ఒకవేళ(!ఫలితం) {
      /*
       * Activate the cookie parser. This may or may not already
       * have been made.
       */
      ఒకవేళ(!దత్తాంశం->cookies)
        దత్తాంశం->cookies = Curl_cookie_init();
      ఒకవేళ(!దత్తాంశం->cookies)
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      లేకపోతే
        దత్తాంశం->state.cookie_engine = TRUE;
    }
    ఆపు;

  స్థితి CURLOPT_COOKIELIST:
    తిరిగి cookielist(దత్తాంశం, ptr);
#అయితే_ముగింపు /* !CURL_DISABLE_COOKIES */

#అయితే_ముగింపు /* !CURL_DISABLE_HTTP */
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు /* !CURL_DISABLE_HTTP || !CURL_DISABLE_MQTT */

#నిర్వచితమైతే USE_SSH
స్థిర CURLcode setopt_cptr_ssh(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                                అక్షరం *ptr)
{
  నిర్మాణం UserDefined *s = &దత్తాంశం->సమితి;
  ఎంపిక(option) {
  స్థితి CURLOPT_SSH_ప్రజా_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa.pub file
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_SSH_ప్రజా_కీలకం, ptr);
  స్థితి CURLOPT_SSH_వ్యక్తిగత_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa file
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_SSH_వ్యక్తిగత_కీలకం, ptr);
  స్థితి CURLOPT_SSH_KEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_keyfunc_userp = ptr;
    ఆపు;
  స్థితి CURLOPT_SSH_HOST_ప్రజా_కీలకం_MD5:
    /*
     * Option to allow for the MD5 of the host public key to be checked
     * for validation purposes.
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_SSH_HOST_ప్రజా_కీలకం_MD5, ptr);
  స్థితి CURLOPT_SSH_HOST_ప్రజా_కీలకం_SHA256:
    /*
     * Option to allow for the SHA256 of the host public key to be checked
     * for validation purposes.
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_SSH_HOST_ప్రజా_కీలకం_SHA256, ptr);
  స్థితి CURLOPT_SSH_KNOWNHOSTS:
    /*
     * Store the filename to read known hosts from.
     */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_SSH_KNOWNHOSTS, ptr);
#నిర్వచితమైతే USE_LIBSSH2
  స్థితి CURLOPT_SSH_HOSTKEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_hostkeyfunc_userp = ptr;
    ఆపు;
#అయితే_ముగింపు /* USE_LIBSSH2 */
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }
  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు /* USE_SSH */

#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
స్థిర CURLcode setopt_cptr_ftp(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                                అక్షరం *ptr)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం UserDefined *s = &దత్తాంశం->సమితి;
  ఎంపిక(option) {
  స్థితి CURLOPT_FTPPORT:
    /*
     * Use FTP PORT, this also specifies which IP address to use
     */
    ఫలితం = Curl_setstropt(దత్తాంశం, STRING_FTPPORT, ptr);
    s->ftp_use_పోర్టు = !!CURL_EASY_STR(దత్తాంశం, STRING_FTPPORT);
    ఆపు;

  స్థితి CURLOPT_FTP_ACCOUNT:
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_FTP_ACCOUNT, ptr);

  స్థితి CURLOPT_FTP_ALTERNATIVE_గమ్యం_USER:
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_FTP_ALTERNATIVE_గమ్యం_USER, ptr);

  స్థితి CURLOPT_KRBLEVEL:
    తిరిగి CURLE_NOT_BUILT_లోపల; /* removed in 8.17.0 */
  స్థితి CURLOPT_CHUNK_దత్తాంశం:
    s->wildcardptr = ptr;
    ఆపు;
  స్థితి CURLOPT_FNMATCH_దత్తాంశం:
    s->fnmatch_దత్తాంశం = ptr;
    ఆపు;
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు /* !CURL_DISABLE_FTP */

స్థిర CURLcode setopt_cptr_net(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                                అక్షరం *ptr)
{
  ఎంపిక(option) {
  స్థితి CURLOPT_INTERFACE:
    /*
     * Set what interface or address/hostname to bind the socket to when
     * performing an operation and thus what from-IP your connection will use.
     */
    తిరిగి setstropt_interface(దత్తాంశం, ptr);

#నిర్వచితమైతే USE_RESOLV_ARES
  స్థితి CURLOPT_DNS_SERVERS:
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_DNS_SERVERS, ptr);

  స్థితి CURLOPT_DNS_INTERFACE:
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_DNS_INTERFACE, ptr);

  స్థితి CURLOPT_DNS_LOCAL_IP4:
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_DNS_LOCAL_IP4, ptr);

  స్థితి CURLOPT_DNS_LOCAL_IP6:
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_DNS_LOCAL_IP6, ptr);
#అయితే_ముగింపు
#నిర్వచితమైతే USE_UNIX_SOCKETS
  స్థితి CURLOPT_UNIX_SOCKET_మార్గం:
    దత్తాంశం->సమితి.abstract_unix_socket = FALSE;
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_UNIX_SOCKET_మార్గం, ptr);

  స్థితి CURLOPT_ABSTRACT_UNIX_SOCKET:
    దత్తాంశం->సమితి.abstract_unix_socket = TRUE;
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_UNIX_SOCKET_మార్గం, ptr);
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_DOH
  స్థితి CURLOPT_DOH_URL:
    {
      CURLcode ఫలితం = Curl_setstropt(దత్తాంశం, STRING_DOH, ptr);
      దత్తాంశం->సమితి.doh = !!CURL_EASY_STR(దత్తాంశం, STRING_DOH);
      తిరిగి ఫలితం;
    }
#అయితే_ముగింపు
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }
}

స్థిర CURLcode setopt_cptr_misc(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                                 అక్షరం *ptr)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం UserDefined *s = &దత్తాంశం->సమితి;

  ఎంపిక(option) {
  స్థితి CURLOPT_REQUEST_లక్ష్యం:
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_లక్ష్యం, ptr);
#నిర్వచితం_కాకపోతే CURL_DISABLE_NETRC
  స్థితి CURLOPT_NETRC_FILE:
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_NETRC_FILE, ptr);
#అయితే_ముగింపు
  స్థితి CURLOPT_CUSTOMREQUEST:
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_CUSTOMREQUEST, ptr);

    /* we do not set s->method = HTTPREQ_CUSTOM; here, we continue as if we
       were using the already set type and this changes the actual request
       keyword */
  స్థితి CURLOPT_SERVICE_NAME:
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_SERVICE_NAME, ptr);

  స్థితి CURLOPT_HEADERDATA:
    s->writeheader = ptr;
    ఆపు;
  స్థితి CURLOPT_READDATA:
    s->లోపల_సమితి = ptr;
    ఆపు;
  స్థితి CURLOPT_WRITEDATA:
    s->బహిర్గతం = ptr;
    ఆపు;
  స్థితి CURLOPT_DEBUGDATA:
    s->debugdata = ptr;
    ఆపు;
  స్థితి CURLOPT_PROGRESSDATA:
    s->పురోగతి_client = ptr;
    ఆపు;
  స్థితి CURLOPT_SEEKDATA:
    s->seek_client = ptr;
    ఆపు;
  స్థితి CURLOPT_IOCTLDATA:
    s->ioctl_client = ptr;
    ఆపు;
  స్థితి CURLOPT_SOCKOPTDATA:
    s->sockopt_client = ptr;
    ఆపు;
  స్థితి CURLOPT_OPENSOCKETDATA:
    s->opensocket_client = ptr;
    ఆపు;
  స్థితి CURLOPT_RESOLVER_ప్రారంభం_దత్తాంశం:
    s->resolver_ప్రారంభం_client = ptr;
    ఆపు;
  స్థితి CURLOPT_CLOSESOCKETDATA:
    s->closesocket_client = ptr;
    ఆపు;
  స్థితి CURLOPT_PREREQDATA:
    s->prereq_userp = ptr;
    ఆపు;
  స్థితి CURLOPT_ERRORBUFFER:
    s->errorbuffer = ptr;
    ఆపు;
  స్థితి CURLOPT_URL:
    ఫలితం = Curl_setstropt(దత్తాంశం, STRING_సమితి_URL, ptr);
    Curl_bufref_సమితి(&దత్తాంశం->state.url,
                    CURL_EASY_STR(దత్తాంశం, STRING_సమితి_URL), 0, ఖాళీ);
    ఆపు;

  స్థితి CURLOPT_USERPWD: {
    అక్షరం *u = ఖాళీ, *p = ఖాళీ;
    ఫలితం = setstropt_userpwd(ptr, &u, &p);
    ఒకవేళ(!ఫలితం) {
      ఫలితం = CURL_EASY_STR_SETN(దత్తాంశం, STRING_USERNAME, u);
      u = ఖాళీ;
    }
    ఒకవేళ(!ఫలితం) {
      ఫలితం = CURL_EASY_STR_SETN(దత్తాంశం, STRING_PASSWORD, p);
      p = ఖాళీ;
    }
    curlx_free(u);
    curlx_free(p);
    తిరిగి ఫలితం;
  }

  స్థితి CURLOPT_USERNAME:
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_USERNAME, ptr);

  స్థితి CURLOPT_PASSWORD:
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_PASSWORD, ptr);

  స్థితి CURLOPT_LOGIN_OPTIONS:
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_OPTIONS, ptr);

  స్థితి CURLOPT_XOAUTH2_BEARER:
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_BEARER, ptr);
  స్థితి CURLOPT_RANGE:
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_సమితి_RANGE, ptr);
  స్థితి CURLOPT_వ్యక్తిగత:
    s->వ్యక్తిగత_దత్తాంశం = ptr;
    ఆపు;
  స్థితి CURLOPT_PROTOCOLS_STR:
    ఒకవేళ(ptr) {
      curl_prot_t protos;
      ఫలితం = protocol2num(ptr, &protos);
      ఒకవేళ(!ఫలితం)
        s->allowed_protocols = protos;
    }
    లేకపోతే
      /* make a NULL argument reset to default */
      s->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
    ఆపు;
  స్థితి CURLOPT_REDIR_PROTOCOLS_STR:
    ఒకవేళ(ptr) {
      curl_prot_t protos;
      ఫలితం = protocol2num(ptr, &protos);
      ఒకవేళ(!ఫలితం)
        s->redir_protocols = protos;
    }
    లేకపోతే
      /* make a NULL argument reset to default */
      s->redir_protocols = (curl_prot_t)CURLPROTO_REDIR;
    ఆపు;
  స్థితి CURLOPT_DEFAULT_PROTOCOL:
    /* Set the protocol to use when the URL does not include any protocol */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_DEFAULT_PROTOCOL, ptr);
#నిర్వచితం_కాకపోతే CURL_DISABLE_SMTP
  స్థితి CURLOPT_MAIL_FROM:
    /* Set the SMTP mail originator */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_MAIL_FROM, ptr);
  స్థితి CURLOPT_MAIL_AUTH:
    /* Set the SMTP auth originator */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_MAIL_AUTH, ptr);
#అయితే_ముగింపు
  స్థితి CURLOPT_SASL_AUTHZID:
    /* Authorization identity (identity to act as) */
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_SASL_AUTHZID, ptr);
#నిర్వచితం_కాకపోతే CURL_DISABLE_RTSP
  స్థితి CURLOPT_RTSP_SESSION_ID:
    Curl_peer_unlink(&దత్తాంశం->state.rtsp_session_origin);
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_RTSP_SESSION_ID, ptr);
  స్థితి CURLOPT_RTSP_ప్రవాహం_URI:
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_RTSP_ప్రవాహం_URI, ptr);
  స్థితి CURLOPT_RTSP_TRANSPORT:
    తిరిగి Curl_setstropt(దత్తాంశం, STRING_RTSP_TRANSPORT, ptr);
  స్థితి CURLOPT_INTERLEAVEDATA:
    s->rtp_బహిర్గతం = ptr;
    ఆపు;
#అయితే_ముగింపు /* !CURL_DISABLE_RTSP */
  స్థితి CURLOPT_TLSAUTH_USERNAME:
  స్థితి CURLOPT_TLSAUTH_PASSWORD:
  స్థితి CURLOPT_TLSAUTH_TYPE:
  స్థితి CURLOPT_PROXY_TLSAUTH_USERNAME:
  స్థితి CURLOPT_PROXY_TLSAUTH_PASSWORD:
  స్థితి CURLOPT_PROXY_TLSAUTH_TYPE:
    తిరిగి CURLE_NOT_BUILT_లోపల;
#నిర్వచితం_కాకపోతే CURL_DISABLE_HSTS
  స్థితి CURLOPT_HSTSREADDATA:
    s->hsts_read_userp = ptr;
    ఆపు;
  స్థితి CURLOPT_HSTSWRITEDATA:
    s->hsts_write_userp = ptr;
    ఆపు;
  స్థితి CURLOPT_HSTS: {
    నిర్మాణం curl_slist *h;
    ఒకవేళ(!దత్తాంశం->hsts) {
      దత్తాంశం->hsts = Curl_hsts_init();
      ఒకవేళ(!దత్తాంశం->hsts)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    ఒకవేళ(ptr) {
      ఫలితం = Curl_setstropt(దత్తాంశం, STRING_HSTS, ptr);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      /* this needs to build a list of filenames to read from, so that it can
         read them later, as we might get a shared HSTS handle to load them
         into */
      h = curl_slist_append(దత్తాంశం->state.hstslist, ptr);
      ఒకవేళ(!h) {
        curl_slist_free_all(దత్తాంశం->state.hstslist);
        దత్తాంశం->state.hstslist = ఖాళీ;
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
      దత్తాంశం->state.hstslist = h; /* store the list for later use */
    }
    లేకపోతే {
      /* clear the list of HSTS files */
      curl_slist_free_all(దత్తాంశం->state.hstslist);
      దత్తాంశం->state.hstslist = ఖాళీ;
      ఒకవేళ(!దత్తాంశం->share || !దత్తాంశం->share->hsts)
        /* throw away the HSTS cache unless shared */
        Curl_hsts_cleanup(&దత్తాంశం->hsts);
    }
    ఆపు;
  }
#అయితే_ముగింపు /* !CURL_DISABLE_HSTS */
#నిర్వచితం_కాకపోతే CURL_DISABLE_ALTSVC
  స్థితి CURLOPT_ALTSVC:
    ఒకవేళ(!దత్తాంశం->asi) {
      దత్తాంశం->asi = Curl_altsvc_init();
      ఒకవేళ(!దత్తాంశం->asi)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    ఫలితం = Curl_setstropt(దత్తాంశం, STRING_ALTSVC, ptr);
    ఒకవేళ(ఫలితం)
      ఆపు;
    ఒకవేళ(ptr)
      తిరిగి Curl_altsvc_ఎక్కించు(దత్తాంశం->asi, ptr);
    ఆపు;
#అయితే_ముగింపు /* !CURL_DISABLE_ALTSVC */
  స్థితి CURLOPT_ECH:
    తిరిగి setopt_ech(దత్తాంశం, ptr);
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode setopt_cptr(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                            అక్షరం *ptr)
{
  రకనిర్వచనం CURLcode (*ptrfunc)(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                              అక్షరం *ptr);
  /* Order by likeliness */
  స్థిర స్థిరరాశి ptrfunc setopt_call[] = {
    setopt_cptr_misc,
#ఒకవేళ defined(USE_SSL) || defined(USE_SSH)
    setopt_cptr_ssl,
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
    setopt_cptr_proxy,
#అయితే_ముగింపు
    setopt_cptr_net,
#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
    setopt_cptr_ftp,
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSH
    setopt_cptr_ssh,
#అయితే_ముగింపు
#ఒకవేళ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
    setopt_cptr_http_mqtt,
#అయితే_ముగింపు
  };
  size_t i;

  కొరకు(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode ఫలితం = setopt_call[i](దత్తాంశం, option, ptr);
    ఒకవేళ(ఫలితం != CURLE_UNKNOWN_OPTION)
      తిరిగి ఫలితం;
  }
  తిరిగి CURLE_UNKNOWN_OPTION;
}

స్థిర CURLcode setopt_func(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                            va_list param)
{
  నిర్మాణం UserDefined *s = &దత్తాంశం->సమితి;
  ఎంపిక(option) {
  స్థితి CURLOPT_PROGRESSFUNCTION:
    /*
     * Progress callback function
     */
    s->fprogress = va_arg(param, curl_పురోగతి_callback);
    ఒకవేళ(s->fprogress)
      దత్తాంశం->పురోగతి.callback = TRUE; /* no longer internal */
    లేకపోతే
      దత్తాంశం->పురోగతి.callback = FALSE; /* NULL enforces internal */
    ఆపు;

  స్థితి CURLOPT_XFERINFOFUNCTION:
    /*
     * Transfer info callback function
     */
    s->fxferinfo = va_arg(param, curl_xferinfo_callback);
    ఒకవేళ(s->fxferinfo)
      దత్తాంశం->పురోగతి.callback = TRUE; /* no longer internal */
    లేకపోతే
      దత్తాంశం->పురోగతి.callback = FALSE; /* NULL enforces internal */

    ఆపు;
  స్థితి CURLOPT_DEBUGFUNCTION:
    /*
     * stderr write callback.
     */
    s->fdebug = va_arg(param, curl_debug_callback);
    /*
     * if the callback provided is NULL, it will use the default callback
     */
    ఆపు;
  స్థితి CURLOPT_HEADERFUNCTION:
    /*
     * Set header write callback
     */
    s->fwrite_శీర్షిక = va_arg(param, curl_write_callback);
    ఆపు;
  స్థితి CURLOPT_WRITEFUNCTION:
    /*
     * Set data write callback
     */
    s->fwrite_func = va_arg(param, curl_write_callback);
    ఒకవేళ(!s->fwrite_func)
#ఒకవేళ defined(__clang__) && __clang_major__ >= 16
#సూచన clang diagnostic నెట్టు
#సూచన clang diagnostic ignored "-Wcast-function-type-strict"
#అయితే_ముగింపు
      /* When set to NULL, reset to our internal default function */
      s->fwrite_func = (curl_write_callback)fwrite;
#ఒకవేళ defined(__clang__) && __clang_major__ >= 16
#సూచన clang diagnostic తీయి
#అయితే_ముగింపు
    ఆపు;
  స్థితి CURLOPT_READFUNCTION:
    /*
     * Read data callback
     */
    s->fread_func_సమితి = va_arg(param, curl_read_callback);
    ఒకవేళ(!s->fread_func_సమితి) {
      s->is_fread_సమితి = 0;
#ఒకవేళ defined(__clang__) && __clang_major__ >= 16
#సూచన clang diagnostic నెట్టు
#సూచన clang diagnostic ignored "-Wcast-function-type-strict"
#అయితే_ముగింపు
      /* When set to NULL, reset to our internal default function */
      s->fread_func_సమితి = (curl_read_callback)fread;
#ఒకవేళ defined(__clang__) && __clang_major__ >= 16
#సూచన clang diagnostic తీయి
#అయితే_ముగింపు
    }
    లేకపోతే
      s->is_fread_సమితి = 1;
    ఆపు;
  స్థితి CURLOPT_SEEKFUNCTION:
    /*
     * Seek callback. Might be NULL.
     */
    s->seek_func = va_arg(param, curl_seek_callback);
    ఆపు;
  స్థితి CURLOPT_IOCTLFUNCTION:
    /*
     * I/O control callback. Might be NULL.
     */
    s->ioctl_func = va_arg(param, curl_ioctl_callback);
    ఆపు;
  స్థితి CURLOPT_SSL_CTX_FUNCTION:
    /*
     * Set an SSL_CTX callback
     */
#నిర్వచితమైతే USE_SSL
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_SSL_CTX)) {
      s->ssl_fsslctx = va_arg(param, curl_ssl_ctx_callback);
      ఆపు;
    }
    లేకపోతే
#అయితే_ముగింపు
      తిరిగి CURLE_NOT_BUILT_లోపల;

  స్థితి CURLOPT_SOCKOPTFUNCTION:
    /*
     * socket callback function: called after socket() but before connect()
     */
    s->fsockopt = va_arg(param, curl_sockopt_callback);
    ఆపు;

  స్థితి CURLOPT_OPENSOCKETFUNCTION:
    /*
     * open/create socket callback function: called instead of socket(),
     * before connect()
     */
    s->fopensocket = va_arg(param, curl_opensocket_callback);
    ఆపు;

  స్థితి CURLOPT_CLOSESOCKETFUNCTION:
    /*
     * close socket callback function: called instead of close()
     * when shutting down a connection
     */
    s->fclosesocket = va_arg(param, curl_closesocket_callback);
    ఆపు;

  స్థితి CURLOPT_RESOLVER_ప్రారంభం_FUNCTION:
    /*
     * resolver start callback function: called before a new resolver request
     * is started
     */
    s->resolver_ప్రారంభం = va_arg(param, curl_resolver_ప్రారంభం_callback);
    ఆపు;

#నిర్వచితమైతే USE_SSH
#నిర్వచితమైతే USE_LIBSSH2
  స్థితి CURLOPT_SSH_HOSTKEYFUNCTION:
    /* the callback to check the hostkey without the knownhost file */
    s->ssh_hostkeyfunc = va_arg(param, curl_sshhostkeycallback);
    ఆపు;
#అయితే_ముగింపు

  స్థితి CURLOPT_SSH_KEYFUNCTION:
    /* setting to NULL is fine since the ssh.c functions themselves will
       then revert to use the internal default */
    s->ssh_keyfunc = va_arg(param, curl_sshkeycallback);
    ఆపు;

#అయితే_ముగింపు /* USE_SSH */

#నిర్వచితం_కాకపోతే CURL_DISABLE_RTSP
  స్థితి CURLOPT_INTERLEAVEFUNCTION:
    /* Set the user defined RTP write function */
    s->fwrite_rtp = va_arg(param, curl_write_callback);
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
  స్థితి CURLOPT_CHUNK_BGN_FUNCTION:
    s->chunk_bgn = va_arg(param, curl_chunk_bgn_callback);
    ఆపు;
  స్థితి CURLOPT_CHUNK_చివర_FUNCTION:
    s->chunk_చివర = va_arg(param, curl_chunk_చివర_callback);
    ఆపు;
  స్థితి CURLOPT_FNMATCH_FUNCTION:
    s->fnmatch = va_arg(param, curl_fnmatch_callback);
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  స్థితి CURLOPT_TRAILERFUNCTION:
    s->trailer_callback = va_arg(param, curl_trailer_callback);
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HSTS
  స్థితి CURLOPT_HSTSREADFUNCTION:
    s->hsts_read = va_arg(param, curl_hstsread_callback);
    ఆపు;
  స్థితి CURLOPT_HSTSWRITEFUNCTION:
    s->hsts_write = va_arg(param, curl_hstswrite_callback);
    ఆపు;
#అయితే_ముగింపు
  స్థితి CURLOPT_PREREQFUNCTION:
    s->fprereq = va_arg(param, curl_prereq_callback);
    ఆపు;
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode setopt_offt(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                            curl_off_t offt)
{
  నిర్మాణం UserDefined *s = &దత్తాంశం->సమితి;
  ఎంపిక(option) {
  స్థితి CURLOPT_TIMEVALUE_LARGE:
    /*
     * This is the value to compare with the remote document with the
     * method set with CURLOPT_TIMECONDITION
     */
    s->timevalue = (time_t)offt;
    ఆపు;

    /* MQTT "borrows" some of the HTTP options */
  స్థితి CURLOPT_POSTFIELDSIZE_LARGE:
    /*
     * The size of the POSTFIELD data to prevent libcurl to do strlen() to
     * figure it out. Enables binary posts.
     */
    ఒకవేళ(offt < -1)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;

    ఒకవేళ(s->postfieldsize < offt && s->str_copypostfields) {
      /* Previous CURLOPT_COPYPOSTFIELDS is no longer valid. */
      curlx_safefree(s->str_copypostfields);
      s->postfields = ఖాళీ;
    }
    s->postfieldsize = offt;
    ఆపు;
  స్థితి CURLOPT_INFILESIZE_LARGE:
    /*
     * If known, this should inform curl about the file size of the
     * to-be-uploaded file.
     */
    ఒకవేళ(offt < -1)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    s->filesize = offt;
    ఆపు;
  స్థితి CURLOPT_MAX_SEND_వేగం_LARGE:
    /*
     * When transfer uploads are faster than CURLOPT_MAX_SEND_SPEED_LARGE
     * bytes per second the transfer is throttled..
     */
    ఒకవేళ(offt < 0)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    s->max_send_వేగం = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&దత్తాంశం->పురోగతి.ul.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    ఆపు;
  స్థితి CURLOPT_MAX_RECV_వేగం_LARGE:
    /*
     * When receiving data faster than CURLOPT_MAX_RECV_SPEED_LARGE bytes per
     * second the transfer is throttled..
     */
    ఒకవేళ(offt < 0)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    s->max_recv_వేగం = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&దత్తాంశం->పురోగతి.dl.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    ఆపు;
  స్థితి CURLOPT_RESUME_FROM_LARGE:
    /*
     * Resume transfer at the given file position
     */
    ఒకవేళ(offt < -1)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    s->సమితి_resume_from = offt;
    ఆపు;
  స్థితి CURLOPT_MAXFILESIZE_LARGE:
    /*
     * Set the maximum size of a file to download.
     */
    ఒకవేళ(offt < 0)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    s->max_filesize = offt;
    ఆపు;

  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode setopt_blob(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                            నిర్మాణం curl_blob *blob)
{
  నిర్మాణం UserDefined *s = &దత్తాంశం->సమితి;
  ఎంపిక(option) {
  స్థితి CURLOPT_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use
     */
    తిరిగి Curl_setblobopt(&s->blobs[BLOB_CERT], blob);
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  స్థితి CURLOPT_PROXY_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use for proxy
     */
    తిరిగి Curl_setblobopt(&s->blobs[BLOB_CERT_PROXY], blob);
  స్థితి CURLOPT_PROXY_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use for proxy
     */
    తిరిగి Curl_setblobopt(&s->blobs[BLOB_కీలకం_PROXY], blob);
  స్థితి CURLOPT_PROXY_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection proxy.
     * Specify entire PEM of the CA certificate
     */
#నిర్వచితమైతే USE_SSL
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_CAINFO_BLOB)) {
      CURLcode ఫలితం = Curl_setblobopt(&s->blobs[BLOB_CAINFO_PROXY], blob);
      ఒకవేళ(!ఫలితం)
        s->proxy_ssl.custom_cablob = TRUE;
      తిరిగి ఫలితం;
    }
#అయితే_ముగింపు
    తిరిగి CURLE_NOT_BUILT_లోపల;
  స్థితి CURLOPT_PROXY_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_ISSUERCERT_BLOB))
      తిరిగి Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT_PROXY], blob);
    తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
  స్థితి CURLOPT_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use
     */
    తిరిగి Curl_setblobopt(&s->blobs[BLOB_కీలకం], blob);
  స్థితి CURLOPT_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection.
     * Specify entire PEM of the CA certificate
     */
#నిర్వచితమైతే USE_SSL
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_CAINFO_BLOB)) {
      CURLcode ఫలితం = Curl_setblobopt(&s->blobs[BLOB_CAINFO], blob);
      ఒకవేళ(!ఫలితం)
        s->ssl.custom_cablob = TRUE;
      తిరిగి ఫలితం;
    }
#అయితే_ముగింపు
    తిరిగి CURLE_NOT_BUILT_లోపల;
  స్థితి CURLOPT_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    ఒకవేళ(Curl_ssl_supports(దత్తాంశం, SSLSUPP_ISSUERCERT_BLOB))
      తిరిగి Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT], blob);
    తిరిగి CURLE_NOT_BUILT_లోపల;

  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }
  /* unreachable */
}

/*
 * Do not make Curl_vsetopt() static: it is called from
 * projects/OS400/ccsidcurl.c.
 */
CURLcode Curl_vsetopt(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option, va_list param)
{
  ఒకవేళ(option < CURLOPTTYPE_OBJECTPOINT)
    తిరిగి setopt_long(దత్తాంశం, option, va_arg(param, దీర్ఘ));
  లేకపోతే ఒకవేళ(option < CURLOPTTYPE_FUNCTIONPOINT) {
    /* unfortunately, different pointer types cannot be identified any other
       way than being listed explicitly */
    ఎంపిక(option) {
    స్థితి CURLOPT_HTTPHEADER:
    స్థితి CURLOPT_QUOTE:
    స్థితి CURLOPT_POSTQUOTE:
    స్థితి CURLOPT_TELNETOPTIONS:
    స్థితి CURLOPT_PREQUOTE:
    స్థితి CURLOPT_HTTP200ALIASES:
    స్థితి CURLOPT_MAIL_RCPT:
    స్థితి CURLOPT_RESOLVE:
    స్థితి CURLOPT_PROXYHEADER:
    స్థితి CURLOPT_CONNECT_గమ్యం:
      తిరిగి setopt_slist(దత్తాంశం, option, va_arg(param, నిర్మాణం curl_slist *));
    స్థితి CURLOPT_HTTPPOST:         /* curl_httppost * */
    స్థితి CURLOPT_MIMEPOST:         /* curl_mime * */
    స్థితి CURLOPT_STDERR:           /* FILE * */
    స్థితి CURLOPT_SHARE:            /* CURLSH * */
    స్థితి CURLOPT_CURLU:            /* CURLU * */
      తిరిగి setopt_సూచికలు(దత్తాంశం, option, param);
    స్థితి CURLOPT_ప్రవాహం_DEPENDS:   /* CURL * */
    స్థితి CURLOPT_ప్రవాహం_DEPENDS_E: /* CURL * */
      తిరిగి CURLE_OK;
    అప్రమేయ:
      ఆపు;
    }
    /* the char pointer options */
    తిరిగి setopt_cptr(దత్తాంశం, option, va_arg(param, అక్షరం *));
  }
  లేకపోతే ఒకవేళ(option < CURLOPTTYPE_OFF_T)
    తిరిగి setopt_func(దత్తాంశం, option, param);
  లేకపోతే ఒకవేళ(option < CURLOPTTYPE_BLOB)
    తిరిగి setopt_offt(దత్తాంశం, option, va_arg(param, curl_off_t));
  తిరిగి setopt_blob(దత్తాంశం, option, va_arg(param, నిర్మాణం curl_blob *));
}

/*
 * curl_easy_setopt() is the external interface for setting options on an
 * easy handle.
 *
 * NOTE: This is one of few API functions that are allowed to be called from
 * within a callback.
 */
#నిర్వచనం_రద్దు curl_easy_setopt
CURLcode curl_easy_setopt(CURL *curl, CURLoption option, ...)
{
  నిర్మాణం Curl_eapi_guard guard;
  CURLcode ఫలితం;

  ఒకవేళ(CURL_EAPI_ENTER(&guard, curl, easy_setopt, &ఫలితం)) {
    నిర్మాణం Curl_easy *దత్తాంశం = curl;
    va_list arg;

    va_start(arg, option);

    ఫలితం = Curl_vsetopt(దత్తాంశం, option, arg);

    va_end(arg);
    ఒకవేళ(ఫలితం == CURLE_BAD_FUNCTION_వాదం)
      failf(దత్తాంశం, "setopt 0x%x got bad argument", (గుర్తులేని పూర్ణాంకం)option);
  }
  CURL_EAPI_LEAVE(&guard);
  తిరిగి ఫలితం;
}
