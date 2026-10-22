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

#که_تعریف_شوی HAVE_NETINET_دننه_H
#ضمن <netinet/in.h>
#د_که_پای

#که_تعریف_شوی HAVE_LINUX_TCP_H
#ضمن <linux/tcp.h>
#وإلاإذا defined(HAVE_NETINET_TCP_H)
#ضمن <netinet/tcp.h>
#د_که_پای

#ضمن "urldata.h"
#ضمن "url.h"
#ضمن "progress.h"
#ضمن "content_encoding.h"
#ضمن "strcase.h"
#ضمن "curl_share.h"
#ضمن "vtls/vtls.h"
#ضمن "curl_trc.h"
#ضمن "setopt.h"
#ضمن "altsvc.h"
#ضمن "hsts.h"
#ضمن "tftp.h"
#ضمن "curlx/strdup.h"
#ضمن "escape.h"
#ضمن "bufref.h"
#ضمن "vauth/vauth.h"

ساكن CURLcode setopt_سېټ_timeout_sec(timediff_t *ptimeout_ms, اوږد secs)
{
  إذا(secs < 0)
    أرجع CURLE_BAD_FUNCTION_دلیل;
#إذا LONG_MAX > (TIMEDIFF_T_MAX / 1000)
  إذا(secs > (TIMEDIFF_T_MAX / 1000)) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    أرجع CURLE_OK;
  }
#د_که_پای
  *ptimeout_ms = (timediff_t)secs * 1000;
  أرجع CURLE_OK;
}

ساكن CURLcode setopt_سېټ_timeout_ms(timediff_t *ptimeout_ms, اوږد ms)
{
  إذا(ms < 0)
    أرجع CURLE_BAD_FUNCTION_دلیل;
#إذا LONG_MAX > TIMEDIFF_T_MAX
  إذا(ms > TIMEDIFF_T_MAX) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    أرجع CURLE_OK;
  }
#د_که_پای
  *ptimeout_ms = (timediff_t)ms;
  أرجع CURLE_OK;
}

CURLcode Curl_setstropt(بنية Curl_easy *معلومات,
                        تعداد dupstring id, ثابت حرف *s)
{
  size_t slen = s ? مزي_اوږدوالی(s) : 0;
  DEBUGASSERT((بینښه)id <= UINT8_MAX);
  إذا(s && (slen > CURL_MAX_ننوت_اوږدتیا))
    أرجع CURLE_BAD_FUNCTION_دلیل;

  أرجع CURL_EASY_STR_سېټ(معلومات, (uint8_t)id, s, slen);
}

CURLcode Curl_setblobopt(بنية curl_blob **blobp,
                         ثابت بنية curl_blob *blob)
{
  /* free the previous storage at `blobp' and replace by a dynamic storage
     copy of blob. If CURL_BLOB_COPY is set, the data is copied. */

  curlx_safefree(*blobp);

  إذا(blob) {
    بنية curl_blob *nblob;
    إذا(!blob->معلومات || !blob->len || (blob->len > CURL_MAX_ننوت_اوږدتیا))
      أرجع CURLE_BAD_FUNCTION_دلیل;
    nblob = (بنية curl_blob *)
      curlx_malloc(حجم(بنية curl_blob) +
                   ((blob->flags & CURL_BLOB_COPY) ? blob->len : 0));
    إذا(!nblob)
      أرجع CURLE_بهر_د_حافظه;
    *nblob = *blob;
    إذا(blob->flags & CURL_BLOB_COPY) {
      /* put the data after the blob struct in memory */
      nblob->معلومات = (حرف *)nblob + حجم(بنية curl_blob);
      حافظه_نقل(nblob->معلومات, blob->معلومات, blob->len);
    }

    *blobp = nblob;
    أرجع CURLE_OK;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode setstropt_userpwd(ثابت حرف *option, حرف **userp,
                                  حرف **passwdp)
{
  حرف *user = عدم;
  حرف *passwd = عدم;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Parse the login details if specified. If not, then we treat NULL as a
     hint to clear the existing data */
  إذا(option) {
    size_t len = مزي_اوږدوالی(option);
    CURLcode پایله;
    إذا(len > CURL_MAX_ننوت_اوږدتیا)
      أرجع CURLE_BAD_FUNCTION_دلیل;

    پایله = Curl_parse_login_details(option, len, &user, &passwd, عدم);
    إذا(پایله)
      أرجع پایله;
  }

  curlx_free(*userp);
  *userp = user;

  curlx_strzero(*passwdp);
  curlx_free(*passwdp);
  *passwdp = passwd;

  أرجع CURLE_OK;
}

ساكن CURLcode setstropt_interface(بنية Curl_easy *معلومات, حرف *option)
{
  إذا(option && !option[0])
    option = عدم;
  إذا(option) {
    /* Parse for checking syntax */
    CURLcode پایله = Curl_parse_interface(option, عدم, عدم, عدم);
    إذا(پایله)
      أرجع پایله;
    أرجع CURL_EASY_STR_سېټ(معلومات, STRING_INTERFACE_OPT,
                             option, مزي_اوږدوالی(option));
  }
  CURL_EASY_STR_CLEAR(معلومات, STRING_INTERFACE_OPT);
  أرجع CURLE_OK;
}

#که_تعریف_شوی USE_SSL
#تعریف_کړه C_SSLVERSION_ارزښت(x)     ((x) & 0xffff)
#تعریف_کړه C_SSLVERSION_MAX_ارزښت(x) ((بینښه اوږد)(x) & 0xffff0000)
#د_که_پای

ساكن CURLcode protocol2num(ثابت حرف *str, curl_prot_t *val)
{
  /*
   * We are asked to cherry-pick protocols, so play it safe and disallow all
   * protocols to start with, and re-add the wanted ones back in.
   */
  *val = 0;

  إذا(!str)
    أرجع CURLE_BAD_FUNCTION_دلیل;

  إذا(curl_strequal(str, "all")) {
    *val = ~(curl_prot_t)0;
    أرجع CURLE_OK;
  }

  افعل {
    ثابت حرف *token = str;
    size_t tlen;

    str = strchr(str, ',');
    tlen = str ? (size_t)(str - token) : مزي_اوږدوالی(token);
    إذا(tlen) {
      ثابت بنية Curl_scheme *h = Curl_getn_scheme(token, tlen);

      إذا(!h || !h->پرلپسې)
        أرجع CURLE_UNSUPPORTED_PROTOCOL;

      *val |= h->protocol;
    }
  } طالما(str && str++);

  إذا(!*val)
    /* no protocol listed */
    أرجع CURLE_BAD_FUNCTION_دلیل;
  أرجع CURLE_OK;
}

#إذا !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
ساكن CURLcode httpauth(بنية Curl_easy *معلومات, bool proxy,
                         بینښه اوږد auth)
{
  إذا(auth != CURLAUTH_NONE) {
    صحيح bitcheck = 0;
    bool authbits = FALSE;
    إذا(auth & CURLAUTH_DIGEST_IE) {
      auth |= CURLAUTH_DIGEST; /* set standard digest bit */
      auth &= ~CURLAUTH_DIGEST_IE; /* drop the legacy bit */
    }

    /* switch off bits we cannot support */
#که_نه_تعریف_شوی USE_NTLM
    auth &= ~CURLAUTH_NTLM; /* no NTLM support */
#د_که_پای
#که_نه_تعریف_شوی USE_SPNEGO
    auth &= ~CURLAUTH_NEGOTIATE; /* no Negotiate (SPNEGO) auth without GSS-API
                                    or SSPI */
#د_که_پای

    /* check if any auth bit lower than CURLAUTH_ONLY is still set */
    طالما(bitcheck < 31) {
      إذا(auth & (1UL << bitcheck++)) {
        authbits = TRUE;
        اكسر;
      }
    }
    إذا(!authbits)
      أرجع CURLE_NOT_BUILT_دننه; /* no supported types left! */
  }
  إذا(proxy)
    معلومات->سېټ.proxyauth = (uint32_t)auth;
  وإلا
    معلومات->سېټ.httpauth = (uint32_t)auth;
  أرجع CURLE_OK;
}
#د_که_پای /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */

#که_نه_تعریف_شوی CURL_DISABLE_HTTP
ساكن CURLcode setopt_HTTP_نسخه(بنية Curl_easy *معلومات, اوږد arg)
{
  /*
   * This sets a requested HTTP version to be used. The value is one of
   * the listed enums in curl/curl.h.
   */
  بدل(arg) {
  حالة CURL_HTTP_نسخه_NONE:
    /* accepted */
    اكسر;
  حالة CURL_HTTP_نسخه_1_0:
  حالة CURL_HTTP_نسخه_1_1:
    /* accepted */
    اكسر;
#که_تعریف_شوی USE_HTTP2
  حالة CURL_HTTP_نسخه_2_0:
  حالة CURL_HTTP_نسخه_2_TLS:
  حالة CURL_HTTP_نسخه_2_مخکینی_KNOWLEDGE:
    /* accepted */
    اكسر;
#د_که_پای
#که_تعریف_شوی USE_HTTP3
  حالة CURL_HTTP_نسخه_3:
  حالة CURL_HTTP_نسخه_3_ONLY:
    /* accepted */
    اكسر;
#د_که_پای
  افتراضي:
    /* not accepted */
    إذا(arg < CURL_HTTP_نسخه_NONE)
      أرجع CURLE_BAD_FUNCTION_دلیل;
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  }
  معلومات->سېټ.httpwant = (بینښه حرف)arg;
  أرجع CURLE_OK;
}
#د_که_پای /* !CURL_DISABLE_HTTP */

#که_تعریف_شوی USE_SSL
CURLcode Curl_setopt_SSLVERSION(بنية Curl_easy *معلومات, CURLoption option,
                                اوږد arg)
{
  /*
   * Set explicit SSL version to try to connect with, as some SSL
   * implementations are lame.
   */
  {
    اوږد نسخه_2, نسخه_max;
    بنية ssl_easy_config *sslc = &معلومات->سېټ.ssl;
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
    إذا(option != CURLOPT_SSLVERSION)
      sslc = &معلومات->سېټ.proxy_ssl;
#وإلا
    (فراغ)option; /*ناکارول_شوی*/
#د_که_پای
    نسخه_2 = C_SSLVERSION_ارزښت(arg);
    نسخه_max = (اوږد)C_SSLVERSION_MAX_ارزښت(arg);

    إذا(نسخه_2 < CURL_SSLVERSION_DEFAULT ||
       نسخه_2 == CURL_SSLVERSION_SSLv2 ||
       نسخه_2 == CURL_SSLVERSION_SSLv3 ||
       نسخه_2 >= CURL_SSLVERSION_وروستی ||
       نسخه_max < CURL_SSLVERSION_MAX_NONE ||
       نسخه_max >= CURL_SSLVERSION_MAX_وروستی)
      أرجع CURLE_BAD_FUNCTION_دلیل;
    إذا(نسخه_2 == CURL_SSLVERSION_DEFAULT)
      نسخه_2 = CURL_SSLVERSION_TLSv1_2;

    sslc->نسخه_2 = (بینښه حرف)نسخه_2;
    sslc->نسخه_max = (بینښه صحيح)نسخه_max;
  }
  أرجع CURLE_OK;
}
#د_که_پای /* !USE_SSL */

#که_نه_تعریف_شوی CURL_DISABLE_RTSP
ساكن CURLcode setopt_RTSP_REQUEST(بنية Curl_easy *معلومات, اوږد arg)
{
  /*
   * Set the RTSP request method (OPTIONS, SETUP, PLAY, etc...) Would this be
   * better if the RTSPREQ_* were moved into here?
   */
  إذا((arg <= CURL_RTSPREQ_NONE) || (arg >= CURL_RTSPREQ_وروستی))
    أرجع CURLE_BAD_FUNCTION_دلیل;

  معلومات->سېټ.rtspreq = (بینښه حرف)arg;
  أرجع CURLE_OK;
}
#د_که_پای /* !CURL_DISABLE_RTSP */

ساكن CURLcode setopt_long_bool(بنية Curl_easy *معلومات, CURLoption option,
                                 اوږد arg)
{
  bool enabled = !!arg;
  صحيح ok = 1;
  بنية UserDefined *s = &معلومات->سېټ;
  بدل(option) {
  حالة CURLOPT_FORBID_REUSE:
    /*
     * When this transfer is done, it must not be left to be reused by a
     * subsequent transfer but shall be closed immediately.
     */
    s->reuse_forbid = enabled;
    اكسر;
  حالة CURLOPT_FRESH_CONNECT:
    /*
     * This transfer shall not use a previously cached connection but
     * should be made with a fresh new connect!
     */
    s->reuse_fresh = enabled;
    اكسر;
  حالة CURLOPT_VERBOSE:
    /*
     * Verbose means infof() calls that give a lot of information about
     * the connection and transfer procedures as well as internal choices.
     */
    s->verbose = enabled;
    اكسر;
  حالة CURLOPT_سرلیک:
    /*
     * Set to include the header in the general data output stream.
     */
    s->include_سرلیک = enabled;
    اكسر;
  حالة CURLOPT_NOPROGRESS:
    /*
     * Shut off the internal supported progress meter
     */
    معلومات->پرمختګ.hide = enabled;
    اكسر;
  حالة CURLOPT_NOBODY:
    /*
     * Do not include the body part in the output data stream.
     */
    s->opt_no_body = enabled;
#که_نه_تعریف_شوی CURL_DISABLE_HTTP
    إذا(s->opt_no_body)
      /* in HTTP lingo, no body means using the HEAD request... */
      s->لاره = HTTPREQ_سر;
    وإلا إذا(s->لاره == HTTPREQ_سر)
      s->لاره = HTTPREQ_GET;
#د_که_پای
    اكسر;
  حالة CURLOPT_FAILONERROR:
    /*
     * Do not output the >=400 error code HTML-page, but instead only
     * return error.
     */
    s->http_fail_on_error = enabled;
    اكسر;
  حالة CURLOPT_KEEP_SENDING_ON_ERROR:
    s->http_keep_sending_on_error = enabled;
    اكسر;
  حالة CURLOPT_UPLOAD:
  حالة CURLOPT_PUT:
    /*
     * We want to send data to the remote host. If this is HTTP, that equals
     * using the PUT request.
     */
    إذا(enabled) {
      /* If this is HTTP, PUT is what's needed to "upload" */
      s->لاره = HTTPREQ_PUT;
      s->opt_no_body = FALSE; /* this is implied */
    }
    وإلا
      /* In HTTP, the opposite of upload is GET (unless NOBODY is true as
         then this can be changed to HEAD later on) */
      s->لاره = HTTPREQ_GET;
    اكسر;
  حالة CURLOPT_FILETIME:
    /*
     * Try to get the file time of the remote document. The time will
     * later (possibly) become available using curl_easy_getinfo().
     */
    s->get_filetime = enabled;
    اكسر;
#که_نه_تعریف_شوی CURL_DISABLE_HTTP
  حالة CURLOPT_HTTP09_ALLOWED:
    s->http09_allowed = enabled;
    اكسر;
#که_نه_تعریف_شوی CURL_DISABLE_COOKIES
  حالة CURLOPT_COOKIESESSION:
    /*
     * Set this option to TRUE to start a new "cookie session". It will
     * prevent the forthcoming read-cookies-from-file actions to accept
     * cookies that are marked as being session cookies, as they belong to a
     * previous session.
     */
    s->cookiesession = enabled;
    اكسر;
#د_که_پای
  حالة CURLOPT_AUTOREFERER:
    /*
     * Switch on automatic referer that gets set if curl follows locations.
     */
    s->http_auto_referer = enabled;
    اكسر;
  حالة CURLOPT_TRANSFER_ENCODING:
    s->http_transfer_encoding = enabled;
    اكسر;
  حالة CURLOPT_UNRESTRICTED_AUTH:
    /*
     * Send authentication (user+password) when following locations, even when
     * hostname changed.
     */
    s->allow_auth_منزل_other_hosts = enabled;
    اكسر;
  حالة CURLOPT_HTTP_TRANSFER_DECODING:
    /*
     * disable libcurl transfer encoding is used
     */
    s->http_te_skip = !enabled; /* reversed */
    اكسر;
  حالة CURLOPT_HTTP_CONTENT_DECODING:
    /*
     * raw data passed to the application when content encoding is used
     */
    s->http_ce_skip = !enabled; /* reversed */
    اكسر;
  حالة CURLOPT_HTTPGET:
    /*
     * Set to force us do HTTP GET
     */
    إذا(enabled) {
      s->لاره = HTTPREQ_GET;
      s->opt_no_body = FALSE; /* this is implied */
    }
    اكسر;
  حالة CURLOPT_POST:
    /* Does this option serve a purpose anymore? Yes it does, when
       CURLOPT_POSTFIELDS is not used and the POST data is read off the
       callback! */
    إذا(enabled) {
      s->لاره = HTTPREQ_POST;
      s->opt_no_body = FALSE; /* this is implied */
    }
    وإلا
      s->لاره = HTTPREQ_GET;
    اكسر;
#د_که_پای /* !CURL_DISABLE_HTTP */
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  حالة CURLOPT_HTTPPROXYTUNNEL:
    /*
     * Tunnel operations through the proxy instead of normal proxy use
     */
    s->tunnel_thru_httpproxy = enabled;
    اكسر;
  حالة CURLOPT_HAPROXYPROTOCOL:
    /*
     * Set to send the HAProxy Proxy Protocol header
     */
    s->haproxyprotocol = enabled;
    اكسر;
  حالة CURLOPT_PROXY_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for proxy.
     */
    s->proxy_ssl.verifypeer = enabled;

    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(معلومات, TRUE);
    اكسر;
  حالة CURLOPT_PROXY_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for proxy
     */
    s->proxy_ssl.verifyhost = enabled;
    ok = 2;
    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(معلومات, TRUE);
    اكسر;
  حالة CURLOPT_PROXY_TRANSFER_ډېرپېښ:
    /*
     * set transfer mode (;type=<a|i>) when doing FTP via an HTTP proxy
     */
    s->proxy_transfer_ډېرپېښ = enabled;
    اكسر;
#د_که_پای /* !CURL_DISABLE_PROXY */
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  حالة CURLOPT_SOCKS5_GSSAPI_NEC:
    /*
     * Set flag for NEC SOCKS5 support
     */
    s->socks5_gssapi_nec = enabled;
    اكسر;
#د_که_پای
#که_تعریف_شوی CURL_LIST_ONLY_PROTOCOL
  حالة CURLOPT_DIRLISTONLY:
    /*
     * An option that changes the command to one that asks for a list only, no
     * file info details. Used for FTP, POP3 and SFTP.
     */
    s->list_only = enabled;
    اكسر;
#د_که_پای
  حالة CURLOPT_APPEND:
    /*
     * We want to upload and append to an existing file. Used for FTP and
     * SFTP.
     */
    s->remote_append = enabled;
    اكسر;
#که_نه_تعریف_شوی CURL_DISABLE_FTP
  حالة CURLOPT_FTP_USE_EPRT:
    s->ftp_use_eprt = enabled;
    اكسر;
  حالة CURLOPT_FTP_USE_EPSV:
    s->ftp_use_epsv = enabled;
    اكسر;
  حالة CURLOPT_FTP_USE_PRET:
    s->ftp_use_pret = enabled;
    اكسر;
  حالة CURLOPT_FTP_SKIP_PASV_IP:
    /*
     * Enable or disable FTP_SKIP_PASV_IP, which will disable/enable the
     * bypass of the IP address in PASV responses.
     */
    s->ftp_skip_ip = enabled;
    اكسر;
  حالة CURLOPT_WILDCARDMATCH:
    s->wildcard_enabled = enabled;
    اكسر;
#د_که_پای
  حالة CURLOPT_CRLF:
    /*
     * Kludgy option to enable CRLF conversions. Subject for removal.
     */
    s->crlf = enabled;
    اكسر;
#که_نه_تعریف_شوی CURL_DISABLE_TFTP
  حالة CURLOPT_TFTP_NO_OPTIONS:
    /*
     * Option that prevents libcurl from sending TFTP option requests to the
     * server.
     */
    s->tftp_no_options = enabled;
    اكسر;
#د_که_پای /* !CURL_DISABLE_TFTP */
  حالة CURLOPT_TRANSFERTEXT:
    /*
     * This option was previously named 'FTPASCII'. Renamed to work with
     * more protocols than merely FTP.
     *
     * Transfer using ASCII (instead of BINARY).
     */
    s->prefer_ascii = enabled;
    اكسر;
  حالة CURLOPT_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying.
     */
    s->ssl.verifypeer = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(معلومات, FALSE);
    اكسر;
#که_نه_تعریف_شوی CURL_DISABLE_DOH
  حالة CURLOPT_DOH_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for DoH.
     */
    s->doh_verifypeer = enabled;
    اكسر;
  حالة CURLOPT_DOH_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for DoH
     */
    s->doh_verifyhost = enabled;
    ok = 2;
    اكسر;
  حالة CURLOPT_DOH_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying for DoH.
     */
    إذا(!Curl_ssl_cert_حالت_request())
      أرجع CURLE_NOT_BUILT_دننه;

    s->doh_verifystatus = enabled;
    ok = 2;
    اكسر;
#د_که_پای /* !CURL_DISABLE_DOH */
  حالة CURLOPT_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate
     */

    /* Obviously people are not reading documentation and too many thought
       this argument took a boolean when it was not and misused it.
       Treat 1 and 2 the same */
    s->ssl.verifyhost = enabled;
    ok = 2;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(معلومات, FALSE);
    اكسر;
  حالة CURLOPT_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying.
     */
    إذا(!Curl_ssl_cert_حالت_request())
      أرجع CURLE_NOT_BUILT_دننه;

    s->ssl.verifystatus = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(معلومات, FALSE);
    اكسر;
  حالة CURLOPT_CERTINFO:
#که_تعریف_شوی USE_SSL
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_CERTINFO))
      s->ssl.certinfo = enabled;
    وإلا
#د_که_پای
      أرجع CURLE_NOT_BUILT_دننه;
    اكسر;
  حالة CURLOPT_NOSIGNAL:
    /*
     * The application asks not to set any signal() or alarm() handlers,
     * even when using a timeout.
     */
    s->no_signal = enabled;
    اكسر;
  حالة CURLOPT_TCP_NODELAY:
    /*
     * Enable or disable TCP_NODELAY, which will disable/enable the Nagle
     * algorithm
     */
    s->tcp_nodelay = enabled;
    اكسر;
  حالة CURLOPT_IGNORE_CONTENT_اوږدتیا:
    s->ignorecl = enabled;
    اكسر;
  حالة CURLOPT_SSL_SESSIONID_CACHE:
    s->ssl.cache_session = enabled;
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
    s->proxy_ssl.cache_session = s->ssl.cache_session;
#د_که_پای
    اكسر;
#که_تعریف_شوی USE_SSH
  حالة CURLOPT_SSH_COMPRESSION:
    s->ssh_compression = enabled;
    اكسر;
#د_که_پای /* !USE_SSH */
#که_نه_تعریف_شوی CURL_DISABLE_SMTP
  حالة CURLOPT_MAIL_RCPT_ALLOWFAILS:
    /* allow RCPT TO command to fail for some recipients */
    s->mail_rcpt_allowfails = enabled;
    اكسر;
#د_که_پای /* !CURL_DISABLE_SMTP */
  حالة CURLOPT_SASL_IR:
    /* Enable/disable SASL initial response */
    s->sasl_ir = enabled;
    اكسر;
  حالة CURLOPT_TCP_KEEPALIVE:
    s->tcp_keepalive = enabled;
    اكسر;
  حالة CURLOPT_TCP_FASTOPEN:
#إذا defined(CONNECT_معلومات_IDEMPOTENT) || defined(MSG_FASTOPEN) || \
  defined(TCP_FASTOPEN_CONNECT)
    s->tcp_fastopen = enabled;
    اكسر;
#وإلا
    أرجع CURLE_NOT_BUILT_دننه;
#د_که_پای
  حالة CURLOPT_SSL_ENABLE_ALPN:
    s->ssl_enable_alpn = enabled;
    اكسر;
  حالة CURLOPT_لار_AS_IS:
    s->لار_as_is = enabled;
    اكسر;
  حالة CURLOPT_PIPEWAIT:
    s->pipewait = enabled;
    اكسر;
  حالة CURLOPT_SUPPRESS_CONNECT_سرلیکونه:
    s->suppress_connect_سرلیکونه = enabled;
    اكسر;
#که_نه_تعریف_شوی CURL_DISABLE_SHUFFLE_DNS
  حالة CURLOPT_DNS_SHUFFLE_پتې:
    s->dns_shuffle_پتې = enabled;
    اكسر;
#د_که_پای
  حالة CURLOPT_DISALLOW_USERNAME_دننه_URL:
    s->disallow_username_دننه_url = enabled;
    اكسر;
  حالة CURLOPT_چټک_EXIT:
    s->quick_exit = enabled;
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  إذا((arg > ok) || (arg < 0))
    /* reserve other values for future use */
    infof(معلومات, "boolean setopt(%d) got unsupported argument %ld,"
          " treated as %d", (صحيح)option, arg, enabled);

  أرجع CURLE_OK;
}

ساكن CURLcode ارزښت_range(اوږد *ارزښت, اوږد لاندې_error, اوږد min, اوږد max)
{
  إذا(*ارزښت < لاندې_error)
    أرجع CURLE_BAD_FUNCTION_دلیل;
  وإلا إذا(*ارزښت < min)
    *ارزښت = min;
  وإلا إذا(*ارزښت > max)
    *ارزښت = max;
  أرجع CURLE_OK;
}

ساكن CURLcode setopt_long_net(بنية Curl_easy *معلومات, CURLoption option,
                                اوږد arg)
{
  CURLcode پایله = CURLE_OK;
  بنية UserDefined *s = &معلومات->سېټ;

  بدل(option) {
  حالة CURLOPT_DNS_CACHE_TIMEOUT:
    إذا(arg != -1)
      أرجع setopt_سېټ_timeout_sec(&s->dns_cache_timeout_ms, arg);
    s->dns_cache_timeout_ms = -1;
    اكسر;
  حالة CURLOPT_MAXCONNECTS:
    پایله = ارزښت_range(&arg, 0, 0, لوی_عدد);
    إذا(!پایله)
      s->maxconnects = arg ? (uint32_t)arg : DEFAULT_CONNCACHE_اندازه;
    اكسر;
  حالة CURLOPT_SERVER_RESPONSE_TIMEOUT:
    أرجع setopt_سېټ_timeout_sec(&s->server_response_timeout, arg);
  حالة CURLOPT_SERVER_RESPONSE_TIMEOUT_MS:
    أرجع setopt_سېټ_timeout_ms(&s->server_response_timeout, arg);
  حالة CURLOPT_ټیټ_چټکتیا_حد:
    إذا(arg < 0)
      پایله = CURLE_BAD_FUNCTION_دلیل;
    وإلا
      s->ټیټ_چټکتیا_حد = arg;
    اكسر;
  حالة CURLOPT_ټیټ_چټکتیا_وخت:
    پایله = ارزښت_range(&arg, 0, 0, USHRT_MAX);
    إذا(!پایله)
      s->ټیټ_چټکتیا_وخت = (uint16_t)arg;
    اكسر;
  حالة CURLOPT_پورټ:
    إذا((arg < 0) || (arg > 65535))
      أرجع CURLE_BAD_FUNCTION_دلیل;
    s->use_پورټ = (بینښه لنډ)arg;
    اكسر;
  حالة CURLOPT_TIMEOUT:
    أرجع setopt_سېټ_timeout_sec(&s->timeout, arg);
  حالة CURLOPT_TIMEOUT_MS:
    أرجع setopt_سېټ_timeout_ms(&s->timeout, arg);
  حالة CURLOPT_CONNECTTIMEOUT:
    أرجع setopt_سېټ_timeout_sec(&s->connecttimeout, arg);
  حالة CURLOPT_CONNECTTIMEOUT_MS:
    أرجع setopt_سېټ_timeout_ms(&s->connecttimeout, arg);
#که_نه_تعریف_شوی CURL_DISABLE_BINDLOCAL
  حالة CURLOPT_LOCALPORT:
    إذا((arg < 0) || (arg > 65535))
      أرجع CURLE_BAD_FUNCTION_دلیل;
    s->localport = curlx_sltous(arg);
    اكسر;
  حالة CURLOPT_LOCALPORTRANGE:
    إذا((arg < 0) || (arg > 65535))
      أرجع CURLE_BAD_FUNCTION_دلیل;
    s->localportrange = curlx_sltous(arg);
    اكسر;
#د_که_پای
  حالة CURLOPT_BUFFERSIZE:
    پایله = ارزښت_range(&arg, 0, READBUFFER_MIN, READBUFFER_MAX);
    إذا(!پایله)
      s->بفر_اندازه = (بینښه صحيح)arg;
    اكسر;
  حالة CURLOPT_UPLOAD_BUFFERSIZE:
    پایله = ارزښت_range(&arg, 0, UPLOADBUFFER_MIN, UPLOADBUFFER_MAX);
    إذا(!پایله)
      s->upload_بفر_اندازه = (بینښه صحيح)arg;
    اكسر;
  حالة CURLOPT_MAXFILESIZE:
    إذا(arg < 0)
      پایله = CURLE_BAD_FUNCTION_دلیل;
    وإلا
      s->max_filesize = arg;
    اكسر;
  حالة CURLOPT_IPRESOLVE:
    إذا((arg < CURL_IPRESOLVE_WHATEVER) || (arg > CURL_IPRESOLVE_V6))
      پایله = CURLE_BAD_FUNCTION_دلیل;
    وإلا
      s->ipver = (بینښه حرف)arg;
    اكسر;
  حالة CURLOPT_CONNECT_ONLY:
    إذا(arg < 0 || arg > 2)
      پایله = CURLE_BAD_FUNCTION_دلیل;
    وإلا {
      s->connect_only = !!arg;
      s->connect_only_ws = (arg == 2);
    }
    اكسر;
#که_تعریف_شوی USE_IPV6
  حالة CURLOPT_پته_SCOPE:
#إذا SIZEOF_LONG > 4
    إذا((بینښه اوږد)arg > UINT_MAX)
      پایله = CURLE_BAD_FUNCTION_دلیل;
    وإلا
#د_که_پای
    s->scope_id = (بینښه صحيح)arg;
    اكسر;
#د_که_پای
  حالة CURLOPT_TCP_KEEPIDLE:
    پایله = ارزښت_range(&arg, 0, 0, لوی_عدد);
    إذا(!پایله)
      s->tcp_keepidle = (صحيح)arg;
    اكسر;
  حالة CURLOPT_TCP_KEEPINTVL:
    پایله = ارزښت_range(&arg, 0, 0, لوی_عدد);
    إذا(!پایله)
      s->tcp_keepintvl = (صحيح)arg;
    اكسر;
  حالة CURLOPT_TCP_KEEPCNT:
    پایله = ارزښت_range(&arg, 0, 0, لوی_عدد);
    إذا(!پایله)
      s->tcp_keepcnt = (صحيح)arg;
    اكسر;
  حالة CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
    أرجع setopt_سېټ_timeout_ms(&s->happy_eyeballs_timeout, arg);
  حالة CURLOPT_UPKEEP_INTERVAL_MS:
    أرجع setopt_سېټ_timeout_ms(&s->upkeep_interval_ms, arg);
  حالة CURLOPT_MAXAGE_CONN:
    أرجع setopt_سېټ_timeout_sec(&s->conn_max_idle_ms, arg);
  حالة CURLOPT_MAXLIFETIME_CONN:
    أرجع setopt_سېټ_timeout_sec(&s->conn_max_age_ms, arg);
  حالة CURLOPT_DNS_USE_GLOBAL_CACHE:
    /* deprecated */
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع پایله;
}

ساكن CURLcode setopt_long_ssl(بنية Curl_easy *معلومات, CURLoption option,
                                اوږد arg)
{
#که_تعریف_شوی USE_SSL
  CURLcode پایله = CURLE_OK;
  بنية UserDefined *s = &معلومات->سېټ;
  بدل(option) {
  حالة CURLOPT_CA_CACHE_TIMEOUT:
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_CA_CACHE)) {
      پایله = ارزښت_range(&arg, -1, -1, لوی_عدد);
      إذا(!پایله)
        s->ssl_ca_cache_timeout = (صحيح)arg;
    }
    وإلا
      پایله = CURLE_NOT_BUILT_دننه;
    اكسر;
  حالة CURLOPT_SSLVERSION:
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  حالة CURLOPT_PROXY_SSLVERSION:
#د_که_پای
    أرجع Curl_setopt_SSLVERSION(معلومات, option, arg);
  حالة CURLOPT_SSL_FALSESTART:
    پایله = CURLE_NOT_BUILT_دننه;
    اكسر;
  حالة CURLOPT_USE_SSL:
    إذا((arg < CURLUSESSL_NONE) || (arg >= CURLUSESSL_وروستی))
      پایله = CURLE_BAD_FUNCTION_دلیل;
    وإلا
      s->use_ssl = (بینښه حرف)arg;
    اكسر;
  حالة CURLOPT_SSL_OPTIONS:
    s->ssl.ssl_options = (بینښه حرف)(arg & 0xff);
    اكسر;
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  حالة CURLOPT_PROXY_SSL_OPTIONS:
    s->proxy_ssl.ssl_options = (بینښه حرف)(arg & 0xff);
    اكسر;
#د_که_پای
  حالة CURLOPT_SSL_ENABLE_NPN:
    اكسر;
  حالة CURLOPT_SSLENGINE_DEFAULT:
#که_تعریف_شوی USE_OPENSSL
    إذا(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      أرجع CURLE_NOT_BUILT_دننه;
    معلومات->سېټ.engine_default = (arg == 1);
#وإلا
    أرجع CURLE_NOT_BUILT_دننه;
#د_که_پای
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع پایله;
#وإلا  /* USE_SSL */
  (فراغ)معلومات;
  (فراغ)option;
  (فراغ)arg;
  أرجع CURLE_UNKNOWN_OPTION;
#د_که_پای /* !USE_SSL */
}

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
ساكن فراغ changeproxy(بنية Curl_easy *معلومات)
{
  Curl_auth_digest_cleanup(&معلومات->state.proxydigest);
  memset(&معلومات->state.authproxy, 0, حجم(معلومات->state.authproxy));
}

ساكن CURLcode setopt_long_proxy(بنية Curl_easy *معلومات, CURLoption option,
                                  اوږد arg)
{
  بنية UserDefined *s = &معلومات->سېټ;

  بدل(option) {
  حالة CURLOPT_PROXYPORT:
    إذا((arg < 0) || (arg > UINT16_MAX))
      أرجع CURLE_BAD_FUNCTION_دلیل;
    إذا(arg != s->proxyport)
      changeproxy(معلومات);
    s->proxyport = (uint16_t)arg;
    اكسر;
  حالة CURLOPT_PROXYAUTH:
    أرجع httpauth(معلومات, TRUE, (بینښه اوږد)arg);
  حالة CURLOPT_PROXYTYPE:
    إذا((arg < CURLPROXY_HTTP) || (arg > CURLPROXY_HTTPS3))
      أرجع CURLE_BAD_FUNCTION_دلیل;
#که_نه_تعریف_شوی USE_PROXY_HTTP3
    إذا(arg == CURLPROXY_HTTPS3)
      أرجع CURLE_NOT_BUILT_دننه;
#د_که_پای
    s->proxytype = (بینښه حرف)arg;
    اكسر;
  حالة CURLOPT_SOCKS5_AUTH: {
    بینښه اوږد auth = (بینښه اوږد)arg;
    إذا(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI | CURLAUTH_ONLY))
      أرجع CURLE_NOT_BUILT_دننه;
    إذا((auth & CURLAUTH_ONLY) && !(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI)))
      أرجع CURLE_BAD_FUNCTION_دلیل;
    s->socks5auth = (بینښه حرف)(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI));
    s->socks5_auth_only = !!(auth & CURLAUTH_ONLY);
    اكسر;
  }
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع CURLE_OK;
}
#وإلا
ساكن CURLcode setopt_long_proxy(بنية Curl_easy *معلومات, CURLoption option,
                                  اوږد arg)
{
  (فراغ)معلومات;
  (فراغ)option;
  (فراغ)arg;
  أرجع CURLE_UNKNOWN_OPTION;
}
#د_که_پای

ساكن CURLcode setopt_long_http(بنية Curl_easy *معلومات, CURLoption option,
                                 اوږد arg)
{
#که_نه_تعریف_شوی CURL_DISABLE_HTTP
  CURLcode پایله = CURLE_OK;
  بنية UserDefined *s = &معلومات->سېټ;

  بدل(option) {
  حالة CURLOPT_FOLLOWLOCATION:
    إذا((بینښه اوږد)arg > 3)
      پایله = CURLE_BAD_FUNCTION_دلیل;
    وإلا
      s->http_follow_ډېرپېښ = (بینښه حرف)arg;
    اكسر;
  حالة CURLOPT_MAXREDIRS:
    پایله = ارزښت_range(&arg, -1, -1, 0x7fff);
    إذا(!پایله)
      s->maxredirs = (لنډ)arg;
    اكسر;
  حالة CURLOPT_POSTREDIR:
    إذا(arg < CURL_REDIR_GET_ALL)
      پایله = CURLE_BAD_FUNCTION_دلیل;
    وإلا {
      s->post301 = !!(arg & CURL_REDIR_POST_301);
      s->post302 = !!(arg & CURL_REDIR_POST_302);
      s->post303 = !!(arg & CURL_REDIR_POST_303);
    }
    اكسر;
  حالة CURLOPT_HEADEROPT:
    s->sep_سرلیکونه = !!(arg & CURLHEADER_SEPARATE);
    اكسر;
  حالة CURLOPT_HTTPAUTH:
    أرجع httpauth(معلومات, FALSE, (بینښه اوږد)arg);
  حالة CURLOPT_HTTP_نسخه:
    أرجع setopt_HTTP_نسخه(معلومات, arg);
  حالة CURLOPT_EXPECT_100_TIMEOUT_MS:
    پایله = ارزښت_range(&arg, 0, 0, 0xffff);
    إذا(!پایله)
      s->expect_100_timeout = (بینښه لنډ)arg;
    اكسر;
  حالة CURLOPT_بهیر_وزن:
#إذا defined(USE_HTTP2) || defined(USE_HTTP3)
    إذا((arg >= 1) && (arg <= 256))
      s->وزن = (صحيح)arg;
    اكسر;
#وإلا
    پایله = CURLE_NOT_BUILT_دننه;
    اكسر;
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_HTTPSIG
  حالة CURLOPT_HTTPSIG_ALGORITHM:
    إذا(arg != CURLHTTPSIG_NONE &&
       arg != CURLHTTPSIG_ED25519 &&
       arg != CURLHTTPSIG_HMAC_SHA256)
      أرجع CURLE_BAD_FUNCTION_دلیل;
    s->httpsig_algorithm = (uint8_t)arg;
    إذا(arg)
      s->httpauth = (uint32_t)CURLAUTH_HTTPSIG;
    وإلا
      s->httpauth &= ~(uint32_t)CURLAUTH_HTTPSIG;
    اكسر;
#د_که_پای
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع پایله;
#وإلا
  (فراغ)معلومات;
  (فراغ)option;
  (فراغ)arg;
  أرجع CURLE_UNKNOWN_OPTION;
#د_که_پای
}

ساكن CURLcode setopt_long_proto(بنية Curl_easy *معلومات, CURLoption option,
                                  اوږد arg)
{
  CURLcode پایله = CURLE_OK;
  بنية UserDefined *s = &معلومات->سېټ;

  بدل(option) {
#که_نه_تعریف_شوی CURL_DISABLE_TFTP
  حالة CURLOPT_TFTP_BLKSIZE:
    پایله = ارزښت_range(&arg, 0, TFTP_BLKSIZE_MIN, TFTP_BLKSIZE_MAX);
    إذا(!پایله)
      s->tftp_blksize = (بینښه لنډ)arg;
    اكسر;
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_NETRC
  حالة CURLOPT_NETRC:
    إذا((arg < CURL_NETRC_IGNORED) || (arg >= CURL_NETRC_وروستی))
      پایله = CURLE_BAD_FUNCTION_دلیل;
    وإلا
      s->use_netrc = (بینښه حرف)arg;
    اكسر;
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_FTP
  حالة CURLOPT_FTP_FILEMETHOD:
    إذا((arg < CURLFTPMETHOD_DEFAULT) || (arg >= CURLFTPMETHOD_وروستی))
      پایله = CURLE_BAD_FUNCTION_دلیل;
    وإلا
      s->ftp_filemethod = (بینښه حرف)arg;
    اكسر;
  حالة CURLOPT_FTP_SSL_CCC:
    إذا((arg < CURLFTPSSL_CCC_NONE) || (arg >= CURLFTPSSL_CCC_وروستی))
      پایله = CURLE_BAD_FUNCTION_دلیل;
    وإلا
      s->ftp_ccc = (بینښه حرف)arg;
    اكسر;
  حالة CURLOPT_FTPSSLAUTH:
    إذا((arg < CURLFTPAUTH_DEFAULT) || (arg >= CURLFTPAUTH_وروستی))
      پایله = CURLE_BAD_FUNCTION_دلیل;
    وإلا
      s->ftpsslauth = (بینښه حرف)arg;
    اكسر;
  حالة CURLOPT_ACCEPTTIMEOUT_MS:
    أرجع setopt_سېټ_timeout_ms(&s->accepttimeout, arg);
#د_که_پای
#إذا !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  حالة CURLOPT_FTP_CREATE_MISSING_DIRS:
    إذا((arg < CURLFTP_CREATE_DIR_NONE) || (arg > CURLFTP_CREATE_DIR_RETRY))
      پایله = CURLE_BAD_FUNCTION_دلیل;
    وإلا
      s->ftp_create_missing_dirs = (بینښه حرف)arg;
    اكسر;
  حالة CURLOPT_NEW_FILE_PERMS:
    إذا((arg < 0) || (arg > 0777))
      پایله = CURLE_BAD_FUNCTION_دلیل;
    وإلا
      s->new_file_perms = (بینښه صحيح)arg;
    اكسر;
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_RTSP
  حالة CURLOPT_RTSP_REQUEST:
    أرجع setopt_RTSP_REQUEST(معلومات, arg);
  حالة CURLOPT_RTSP_CLIENT_CSEQ:
    پایله = ارزښت_range(&arg, 0, 0, لوی_عدد);
    إذا(!پایله)
      معلومات->state.rtsp_بل_client_CSeq = (uint32_t)arg;
    اكسر;
  حالة CURLOPT_RTSP_SERVER_CSEQ:
    پایله = ارزښت_range(&arg, 0, 0, لوی_عدد);
    إذا(!پایله)
      معلومات->state.rtsp_بل_server_CSeq = (uint32_t)arg;
    اكسر;
#د_که_پای
#که_تعریف_شوی USE_SSH
  حالة CURLOPT_SSH_AUTH_TYPES:
    s->ssh_auth_types = (uint32_t)arg;
    اكسر;
  حالة CURLOPT_NEW_DIRECTORY_PERMS:
    إذا((arg < 0) || (arg > 0777))
      پایله = CURLE_BAD_FUNCTION_دلیل;
    وإلا
      s->new_directory_perms = (بینښه صحيح)arg;
    اكسر;
#د_که_پای
  حالة CURLOPT_PROTOCOLS:
    s->allowed_protocols = (curl_prot_t)arg;
    اكسر;
  حالة CURLOPT_REDIR_PROTOCOLS:
    s->redir_protocols = (curl_prot_t)arg;
    اكسر;
#که_نه_تعریف_شوی CURL_DISABLE_WEBSOCKETS
  حالة CURLOPT_WS_OPTIONS:
    s->ws_raw_ډېرپېښ = !!(arg & CURLWS_RAW_ډېرپېښ);
    s->ws_no_auto_pong = !!(arg & CURLWS_NOAUTOPONG);
    اكسر;
#د_که_پای
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع پایله;
}

ساكن CURLcode setopt_long_misc(بنية Curl_easy *معلومات, CURLoption option,
                                 اوږد arg)
{
  بنية UserDefined *s = &معلومات->سېټ;

  بدل(option) {
  حالة CURLOPT_TIMECONDITION:
    إذا((arg < CURL_TIMECOND_NONE) || (arg >= CURL_TIMECOND_وروستی))
      أرجع CURLE_BAD_FUNCTION_دلیل;
    s->timecondition = (بینښه حرف)arg;
    اكسر;
  حالة CURLOPT_TIMEVALUE:
    s->timevalue = (time_t)arg;
    اكسر;
  حالة CURLOPT_POSTFIELDSIZE:
    إذا(arg < -1)
      أرجع CURLE_BAD_FUNCTION_دلیل;
    إذا(s->postfieldsize < arg && s->str_copypostfields) {
      curlx_safefree(s->str_copypostfields);
      s->postfields = عدم;
    }
    s->postfieldsize = arg;
    اكسر;
  حالة CURLOPT_INFILESIZE:
    إذا(arg < -1)
      أرجع CURLE_BAD_FUNCTION_دلیل;
    s->filesize = arg;
    اكسر;
  حالة CURLOPT_RESUME_FROM:
    إذا(arg < -1)
      أرجع CURLE_BAD_FUNCTION_دلیل;
    s->سېټ_resume_from = arg;
    اكسر;
  حالة CURLOPT_UPLOAD_FLAGS:
    s->upload_flags = (بینښه حرف)arg;
    اكسر;
#که_نه_تعریف_شوی CURL_DISABLE_MIME
  حالة CURLOPT_MIME_OPTIONS:
    s->mime_formescape = !!(arg & CURLMIMEOPT_FORMESCAPE);
    اكسر;
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_HSTS
  حالة CURLOPT_HSTS_CTRL:
    إذا(arg & CURLHSTS_ENABLE) {
      إذا(!معلومات->hsts) {
        معلومات->hsts = Curl_hsts_init();
        إذا(!معلومات->hsts)
          أرجع CURLE_بهر_د_حافظه;
      }
    }
    وإلا إذا(!معلومات->share || !معلومات->share->hsts) {
      /* throw away the HSTS cache unless shared */
      Curl_hsts_cleanup(&معلومات->hsts);
      /* flush all the entries */
      curl_slist_free_all(معلومات->state.hstslist);
      معلومات->state.hstslist = عدم;
    }
    وإلا
      /* detach from shared HSTS cache without freeing it */
      معلومات->hsts = عدم;
    اكسر;
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_ALTSVC
  حالة CURLOPT_ALTSVC_CTRL:
    أرجع Curl_altsvc_ctrl(معلومات, arg);
#د_که_پای
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  حالة CURLOPT_GSSAPI_DELEGATION:
    s->gssapi_delegation = (بینښه حرف)arg &
      (CURLGSSAPI_DELEGATION_POLICY_FLAG | CURLGSSAPI_DELEGATION_FLAG);
    اكسر;
#د_که_پای
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode setopt_long(بنية Curl_easy *معلومات, CURLoption option,
                            اوږد arg)
{
  تعريفنوع CURLcode (*setoptfunc)(بنية Curl_easy *معلومات,
                                 CURLoption option, اوږد arg);
  ساكن ثابت setoptfunc setopt_call[] = {
    setopt_long_bool,
    setopt_long_net,
    setopt_long_http,
    setopt_long_proxy,
    setopt_long_ssl,
    setopt_long_proto,
    setopt_long_misc
  };
  size_t i;

  لكل(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode پایله = setopt_call[i](معلومات, option, arg);
    إذا(پایله != CURLE_UNKNOWN_OPTION)
      أرجع پایله;
  }
  أرجع CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode setopt_slist(بنية Curl_easy *معلومات, CURLoption option,
                             بنية curl_slist *slist)
{
  CURLcode پایله = CURLE_OK;
  بنية UserDefined *s = &معلومات->سېټ;
  بدل(option) {
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  حالة CURLOPT_PROXYHEADER:
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
    اكسر;
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_HTTP
  حالة CURLOPT_HTTP200ALIASES:
    /*
     * Set a list of aliases for HTTP 200 in response header
     */
    s->http200aliases = slist;
    اكسر;
#د_که_پای
#إذا !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  حالة CURLOPT_POSTQUOTE:
    /*
     * List of RAW FTP commands to use after a transfer
     */
    s->postquote = slist;
    اكسر;
  حالة CURLOPT_PREQUOTE:
    /*
     * List of RAW FTP commands to use prior to RETR (Wesley Laxton)
     */
    s->prequote = slist;
    اكسر;
  حالة CURLOPT_QUOTE:
    /*
     * List of RAW FTP commands to use before a transfer
     */
    s->quote = slist;
    اكسر;
#د_که_پای
  حالة CURLOPT_RESOLVE:
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
    معلومات->state.resolve = s->resolve;
    اكسر;
#إذا !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MIME)
  حالة CURLOPT_HTTPHEADER:
    /*
     * Set a list with HTTP headers to use (or replace internals with)
     */
    s->سرلیکونه = slist;
    اكسر;
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_TELNET
  حالة CURLOPT_TELNETOPTIONS:
    /*
     * Set a linked list of telnet options
     */
    s->telnet_options = slist;
    اكسر;
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_SMTP
  حالة CURLOPT_MAIL_RCPT:
    /* Set the list of mail recipients */
    s->mail_rcpt = slist;
    اكسر;
#د_که_پای
  حالة CURLOPT_CONNECT_منزل:
    s->connect_منزل = slist;
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع پایله;
}

#إذا !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#که_نه_تعریف_شوی CURL_DISABLE_MIME
ساكن CURLcode setopt_mimepost(بنية Curl_easy *معلومات, curl_mime *mimep)
{
  /*
   * Set to make us do MIME POST
   */
  CURLcode پایله;
  بنية UserDefined *s = &معلومات->سېټ;
  إذا(!s->mimepostp) {
    s->mimepostp = curlx_malloc(حجم(*s->mimepostp));
    إذا(!s->mimepostp)
      أرجع CURLE_بهر_د_حافظه;
    Curl_mime_initpart(s->mimepostp);
  }

  پایله = Curl_mime_سېټ_subparts(s->mimepostp, mimep, FALSE);
  إذا(!پایله) {
    s->لاره = HTTPREQ_POST_MIME;
    s->opt_no_body = FALSE; /* this is implied */
#که_نه_تعریف_شوی CURL_DISABLE_FORM_API
    Curl_mime_cleanpart(معلومات->state.formp);
    curlx_safefree(معلومات->state.formp);
    معلومات->state.mimepost = عدم;
#د_که_پای
  }
  أرجع پایله;
}
#د_که_پای /* !CURL_DISABLE_MIME */
#د_که_پای /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */

ساكن CURLcode setopt_share(بنية Curl_easy *معلومات, بنية Curl_share *سېټ)
{
  CURLcode پایله;

  إذا(معلومات->conn) {
    /* As this handle already has a connection attached, changing share now
       would be complicated and error-prone */
    infof(معلومات, "Cannot change share object while in use");
    پایله = CURLE_BAD_FUNCTION_دلیل;
  }
  وإلا {
    /* disconnect from old share, if any and possible */
    پایله = Curl_share_easy_unlink(معلومات);
    إذا(!پایله && GOOD_SHARE_HANDLE(سېټ))
      /* use new share if it set */
      پایله = Curl_share_easy_link(معلومات, سېټ);
  }
  أرجع پایله;
}

/* assorted pointer type arguments */
ساكن CURLcode setopt_اشارې(بنية Curl_easy *معلومات, CURLoption option,
                                va_list param)
{
  CURLcode پایله = CURLE_OK;
  بنية UserDefined *s = &معلومات->سېټ;
  بدل(option) {
  حالة CURLOPT_CURLU:
    /*
     * pass CURLU to set URL
     */
    Curl_bufref_free(&معلومات->state.url);
    CURL_EASY_STR_CLEAR(معلومات, STRING_سېټ_URL);
    s->uh = va_arg(param, CURLU *);
    اكسر;
#که_نه_تعریف_شوی CURL_DISABLE_HTTP
#که_نه_تعریف_شوی CURL_DISABLE_FORM_API
  حالة CURLOPT_HTTPPOST:
    /*
     * Set to make us do HTTP POST. Legacy API-style.
     */
    s->httppost = va_arg(param, بنية curl_httppost *);
    s->لاره = HTTPREQ_POST_FORM;
    s->opt_no_body = FALSE; /* this is implied */
    Curl_mime_cleanpart(معلومات->state.formp);
    curlx_safefree(معلومات->state.formp);
    معلومات->state.mimepost = عدم;
    اكسر;
#د_که_پای /* !CURL_DISABLE_FORM_API */
#د_که_پای /* !CURL_DISABLE_HTTP */
#إذا !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#که_نه_تعریف_شوی CURL_DISABLE_MIME
  حالة CURLOPT_MIMEPOST:
    پایله = setopt_mimepost(معلومات, va_arg(param, curl_mime *));
    اكسر;
#د_که_پای /* !CURL_DISABLE_MIME */
#د_که_پای /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */
  حالة CURLOPT_STDERR:
    /*
     * Set to a FILE * that should receive all error writes. This
     * defaults to stderr for normal operations.
     */
    s->err = va_arg(param, FILE *);
    إذا(!s->err)
      s->err = stderr;
    اكسر;
  حالة CURLOPT_SHARE:
    أرجع setopt_share(معلومات, va_arg(param, بنية Curl_share *));

  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع پایله;
}

#که_نه_تعریف_شوی CURL_DISABLE_COOKIES
ساكن CURLcode cookielist(بنية Curl_easy *معلومات, ثابت حرف *ptr)
{
  CURLcode پایله = CURLE_OK;
  إذا(!ptr)
    أرجع CURLE_OK;

  إذا(curl_strequal(ptr, "ALL")) {
    /* clear all cookies */
    Curl_share_قلف(معلومات, CURL_قلف_معلومات_COOKIE, CURL_قلف_ACCESS_SINGLE);
    Curl_cookie_clearall(معلومات->cookies);
    Curl_share_unlock(معلومات, CURL_قلف_معلومات_COOKIE);
  }
  وإلا إذا(curl_strequal(ptr, "SESS")) {
    /* clear session cookies */
    Curl_share_قلف(معلومات, CURL_قلف_معلومات_COOKIE, CURL_قلف_ACCESS_SINGLE);
    Curl_cookie_clearsess(معلومات->cookies);
    Curl_share_unlock(معلومات, CURL_قلف_معلومات_COOKIE);
  }
  وإلا إذا(curl_strequal(ptr, "FLUSH")) {
    /* flush cookies to file, takes care of the locking */
    Curl_flush_cookies(معلومات, FALSE);
  }
  وإلا إذا(curl_strequal(ptr, "RELOAD")) {
    /* reload cookies from file */
    أرجع Curl_cookie_loadfiles(معلومات, COOKIE_NOPSL |
                                 (معلومات->سېټ.cookiesession ?
                                  COOKIE_NOSESSION : 0));
  }
  وإلا {
    إذا(!معلومات->cookies) {
      /* if cookie engine was not running, activate it */
      معلومات->cookies = Curl_cookie_init();
      إذا(!معلومات->cookies)
        أرجع CURLE_بهر_د_حافظه;
      معلومات->state.cookie_engine = TRUE;
    }

    /* general protection against mistakes and abuse */
    إذا(مزي_اوږدوالی(ptr) > CURL_MAX_ننوت_اوږدتیا)
      أرجع CURLE_BAD_FUNCTION_دلیل;

    /* Adding these cookies without the PSL check, because the PSL is not
       initialized until *perform() time, and this might be called before
       that */
    Curl_share_قلف(معلومات, CURL_قلف_معلومات_COOKIE, CURL_قلف_ACCESS_SINGLE);
    إذا(checkprefix("Set-Cookie:", ptr))
      /* HTTP Header format line */
      پایله = Curl_cookie_جمع_کړه(معلومات, معلومات->cookies, ptr + 11,
                               عدم, عدم,
                               COOKIE_HTTPHEADER | COOKIE_SECURE |
                               COOKIE_NOPSL);
    وإلا
      /* Netscape format line */
      پایله = Curl_cookie_جمع_کړه(معلومات, معلومات->cookies, ptr, عدم,
                               عدم, COOKIE_SECURE | COOKIE_NOPSL);
    Curl_share_unlock(معلومات, CURL_قلف_معلومات_COOKIE);
  }
  أرجع پایله;
}

ساكن CURLcode cookiefile(بنية Curl_easy *معلومات, ثابت حرف *ptr)
{
  /*
   * Set cookie file to read and parse. Can be used multiple times.
   */
  إذا(ptr) {
    بنية curl_slist *cl;
    /* general protection against mistakes and abuse */
    إذا(مزي_اوږدوالی(ptr) > CURL_MAX_ننوت_اوږدتیا)
      أرجع CURLE_BAD_FUNCTION_دلیل;
    /* append the cookie filename to the list of filenames, and deal with
       them later */
    cl = curl_slist_append(معلومات->state.cookielist, ptr);
    إذا(!cl) {
      curl_slist_free_all(معلومات->state.cookielist);
      معلومات->state.cookielist = عدم;
      أرجع CURLE_بهر_د_حافظه;
    }
    معلومات->state.cookielist = cl; /* store the list for later use */
  }
  وإلا {
    /* clear the list of cookie files */
    curl_slist_free_all(معلومات->state.cookielist);
    معلومات->state.cookielist = عدم;

    إذا(!معلومات->share || !معلومات->share->cookies) {
      /* throw away all existing cookies if this is not a shared cookie
         container */
      Curl_cookie_clearall(معلومات->cookies);
      Curl_cookie_cleanup(معلومات->cookies);
    }
    /* disable the cookie engine */
    معلومات->cookies = عدم;
  }
  أرجع CURLE_OK;
}
#د_که_پای

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
ساكن CURLcode setproxy(بنية Curl_easy *معلومات, ثابت حرف *proxy)
{
  ثابت حرف *str = CURL_EASY_STR(معلومات, STRING_PROXY);
  إذا(str && proxy &&
     /* there was one set, is this a new one? */
     !strcmp(str, proxy))
    أرجع CURLE_OK; /* same one as before */

  changeproxy(معلومات);
  أرجع Curl_setstropt(معلومات, STRING_PROXY, proxy);
}

ساكن CURLcode setopt_cptr_proxy(بنية Curl_easy *معلومات, CURLoption option,
                                  حرف *ptr)
{
  CURLcode پایله = CURLE_OK;
  بنية UserDefined *s = &معلومات->سېټ;
  بدل(option) {
  حالة CURLOPT_PROXYUSERPWD: {
    /*
     * user:password needed to use the proxy
     */
    حرف *u = عدم;
    حرف *p = عدم;
    پایله = setstropt_userpwd(ptr, &u, &p);

    /* URL decode the components */
    إذا(!پایله) {
      حرف *str = عدم;
      CURL_EASY_STR_CLEAR(معلومات, STRING_PROXYUSERNAME);
      CURL_EASY_STR_CLEAR(معلومات, STRING_PROXYPASSWORD);
      إذا(u) {
        پایله = Curl_urldecode(u, 0, &str, عدم, REJECT_ZERO);
        إذا(!پایله)
          پایله = Curl_u8_strset_setn(&s->مزي_ګانې, STRING_PROXYUSERNAME, str);
      }
      إذا(!پایله && p) {
        str = عدم;
        پایله = Curl_urldecode(p, 0, &str, عدم, REJECT_ZERO);
        إذا(!پایله)
          پایله = Curl_u8_strset_setn(&s->مزي_ګانې, STRING_PROXYPASSWORD, str);
      }
    }
    curlx_free(u);
    curlx_strzero(p);
    curlx_free(p);
    اكسر;
  }
  حالة CURLOPT_PROXYUSERNAME:
    /*
     * authentication username to use in the operation
     */
    أرجع Curl_setstropt(معلومات, STRING_PROXYUSERNAME, ptr);

  حالة CURLOPT_PROXYPASSWORD:
    /*
     * authentication password to use in the operation
     */
    أرجع Curl_setstropt(معلومات, STRING_PROXYPASSWORD, ptr);

  حالة CURLOPT_NOPROXY:
    /*
     * proxy exception list
     */
    أرجع Curl_setstropt(معلومات, STRING_NOPROXY, ptr);
  حالة CURLOPT_PROXY_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use for proxy
     */
    أرجع Curl_setstropt(معلومات, STRING_CERT_PROXY, ptr);
  حالة CURLOPT_PROXY_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use for proxy
     */
    أرجع Curl_setstropt(معلومات, STRING_CERT_TYPE_PROXY, ptr);
  حالة CURLOPT_PROXY_SSLKEY:
    /*
     * String that holds filename of the SSL key to use for proxy
     */
    أرجع Curl_setstropt(معلومات, STRING_کلی_PROXY, ptr);
  حالة CURLOPT_PROXY_KEYPASSWD:
    /*
     * String that holds the SSL private key password for proxy.
     */
    أرجع Curl_setstropt(معلومات, STRING_کلی_PASSWD_PROXY, ptr);
  حالة CURLOPT_PROXY_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use for proxy
     */
    أرجع Curl_setstropt(معلومات, STRING_کلی_TYPE_PROXY, ptr);
  حالة CURLOPT_PROXY_SSL_CIPHER_LIST:
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_CIPHER_LIST)) {
      /* set a list of cipher we want to use in the SSL connection for proxy */
      أرجع Curl_setstropt(معلومات, STRING_SSL_CIPHER_LIST_PROXY, ptr);
    }
    وإلا
      أرجع CURLE_NOT_BUILT_دننه;
  حالة CURLOPT_PROXY_TLS13_CIPHERS:
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites for proxy */
      أرجع Curl_setstropt(معلومات, STRING_SSL_CIPHER13_LIST_PROXY, ptr);
    وإلا
      أرجع CURLE_NOT_BUILT_دننه;
  حالة CURLOPT_PROXY:
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
    أرجع setproxy(معلومات, ptr);
  حالة CURLOPT_PRE_PROXY:
    /*
     * Set proxy server:port to use as SOCKS proxy.
     *
     * If the proxy is set to "" or NULL we explicitly say that we do not want
     * to use the socks proxy.
     */
    أرجع Curl_setstropt(معلومات, STRING_PRE_PROXY, ptr);
  حالة CURLOPT_SOCKS5_GSSAPI_SERVICE:
  حالة CURLOPT_PROXY_SERVICE_NAME:
    /*
     * Set proxy authentication service name for Kerberos 5 and SPNEGO
     */
    أرجع Curl_setstropt(معلومات, STRING_PROXY_SERVICE_NAME, ptr);
  حالة CURLOPT_PROXY_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
#که_تعریف_شوی USE_SSL
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_PINNEDPUBKEY))
      أرجع Curl_setstropt(معلومات, STRING_SSL_PINNEDPUBLICKEY_PROXY, ptr);
#د_که_پای
    أرجع CURLE_NOT_BUILT_دننه;

  حالة CURLOPT_HAPROXY_CLIENT_IP:
    /*
     * Set the client IP to send through HAProxy PROXY protocol
     */
    پایله = Curl_setstropt(معلومات, STRING_HAPROXY_CLIENT_IP, ptr);

    /* enable the HAProxy protocol if an IP is provided */
    s->haproxyprotocol = !!CURL_EASY_STR(معلومات, STRING_HAPROXY_CLIENT_IP);
    اكسر;
  حالة CURLOPT_PROXY_CAINFO:
    /*
     * Set CA info SSL connection for proxy. Specify filename of the
     * CA certificate
     */
    پایله = Curl_setstropt(معلومات, STRING_SSL_CAFILE_PROXY, ptr);
    إذا(!پایله)
      s->proxy_ssl.custom_cafile = TRUE;
    أرجع پایله;
  حالة CURLOPT_PROXY_CRLFILE:
    /*
     * Set CRL file info for SSL connection for proxy. Specify filename of the
     * CRL to check certificates revocation
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_CRLFILE))
      أرجع Curl_setstropt(معلومات, STRING_SSL_CRLFILE_PROXY, ptr);
    أرجع CURLE_NOT_BUILT_دننه;
  حالة CURLOPT_PROXY_ISSUERCERT:
    /*
     * Set Issuer certificate file to check certificates issuer
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_ISSUERCERT))
      أرجع Curl_setstropt(معلومات, STRING_SSL_ISSUERCERT_PROXY, ptr);
    أرجع CURLE_NOT_BUILT_دننه;
  حالة CURLOPT_PROXY_CAPATH:
    /*
     * Set CA path info for SSL connection proxy. Specify directory name of the
     * CA certificates which have been prepared using openssl c_rehash utility.
     */
#که_تعریف_شوی USE_SSL
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_CA_لار)) {
      /* This does not work on Windows. */
      پایله = Curl_setstropt(معلومات, STRING_SSL_CAPATH_PROXY, ptr);
      إذا(!پایله)
        s->proxy_ssl.custom_capath = TRUE;
      أرجع پایله;
    }
#د_که_پای
    أرجع CURLE_NOT_BUILT_دننه;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع پایله;
}
#د_که_پای

#إذا !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
/*
 * A string with POST data. Makes curl HTTP POST. Even if it is NULL. If
 * needed, CURLOPT_POSTFIELDSIZE must have been set prior to
 * CURLOPT_COPYPOSTFIELDS and not altered later.
 */
ساكن CURLcode setopt_copypostfields(ثابت حرف *ptr, بنية UserDefined *s)
{
  CURLcode پایله = CURLE_OK;
  إذا(s->postfieldsize < -1)
    أرجع CURLE_BAD_FUNCTION_دلیل;
  إذا(!ptr || s->postfieldsize == -1) {
    إذا(ptr && (مزي_اوږدوالی(ptr) > CURL_MAX_ننوت_اوږدتیا))
      أرجع CURLE_BAD_FUNCTION_دلیل;
    curlx_safefree(s->str_copypostfields);
    إذا(ptr) {
      s->str_copypostfields = curlx_strdup(ptr);
      إذا(!s->str_copypostfields)
        أرجع CURLE_بهر_د_حافظه;
    }
  }
  وإلا {
    size_t pflen = curlx_sotouz_range(s->postfieldsize, 0, SIZE_MAX);
    إذا(pflen == SIZE_MAX)
      أرجع CURLE_بهر_د_حافظه;
    وإلا {
      /* Allocate even when size == 0. This satisfies the need of possible
         later address compare to detect the COPYPOSTFIELDS mode, and to mark
         that postfields is used rather than read function or form data. */
      حرف *p = curlx_memdup0(ptr, pflen);
      إذا(!p)
        أرجع CURLE_بهر_د_حافظه;
      وإلا {
        curlx_free(s->str_copypostfields);
        s->str_copypostfields = p;
      }
    }
  }

  s->postfields = s->str_copypostfields;
  s->لاره = HTTPREQ_POST;
  أرجع پایله;
}
#د_که_پای

#که_تعریف_شوی USE_ECH
ساكن CURLcode setopt_ech(بنية Curl_easy *معلومات, ثابت حرف *ptr)
{
  بنية UserDefined *s = &معلومات->سېټ;
  CURLcode پایله = CURLE_OK;

  إذا(!ptr || !strcmp(ptr, "false"))
    s->ssl.ech = CURLECH_DISABLE;
  وإلا {
    size_t plen = مزي_اوږدوالی(ptr);
    إذا(plen > CURL_MAX_ننوت_اوږدتیا)
      پایله = CURLE_BAD_FUNCTION_دلیل;
    وإلا {
      إذا(!strcmp(ptr, "grease"))
        s->ssl.ech = CURLECH_GREASE;
      وإلا إذا(!strcmp(ptr, "true"))
        s->ssl.ech = CURLECH_ENABLE;
      وإلا إذا(!strcmp(ptr, "hard"))
        s->ssl.ech = CURLECH_HARD;
      وإلا إذا(plen > 4 && !مزي_پرتله(ptr, "ecl:", 4)) {
        إذا(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        پایله = Curl_setstropt(معلومات, STRING_ECH_CONFIG, ptr + 4);
      }
      وإلا إذا(plen > 3 && !مزي_پرتله(ptr, "pn:", 3)) {
        إذا(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        پایله = Curl_setstropt(معلومات, STRING_ECH_عامه, ptr + 3);
      }
      وإلا
        پایله = CURLE_BAD_FUNCTION_دلیل;
    }
  }
  أرجع پایله;
}
#وإلا
#تعریف_کړه setopt_ech(x, y) CURLE_NOT_BUILT_دننه
#د_که_پای

#إذا defined(USE_SSL) || defined(USE_SSH)
/* One of the options is used for both TLS and SSH */
ساكن CURLcode setopt_cptr_ssl(بنية Curl_easy *معلومات, CURLoption option,
                                حرف *ptr)
{
  CURLcode پایله = CURLE_OK;

  بدل(option) {
  حالة CURLOPT_KEYPASSWD:
    /*
     * String that holds the SSL or SSH private key password.
     */
    پایله = Curl_setstropt(معلومات, STRING_کلی_PASSWD, ptr);
    اكسر;
#که_تعریف_شوی USE_SSL
  حالة CURLOPT_CAINFO:
    /*
     * Set CA info for SSL connection. Specify filename of the CA certificate
     */
    پایله = Curl_setstropt(معلومات, STRING_SSL_CAFILE, ptr);
    إذا(!پایله)
      معلومات->سېټ.ssl.custom_cafile = TRUE;
    أرجع پایله;
  حالة CURLOPT_CAPATH:
    /*
     * Set CA path info for SSL connection. Specify directory name of the CA
     * certificates which have been prepared using openssl c_rehash utility.
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_CA_لار)) {
      /* This does not work on Windows. */
      پایله = Curl_setstropt(معلومات, STRING_SSL_CAPATH, ptr);
      إذا(!پایله)
        معلومات->سېټ.ssl.custom_capath = TRUE;
      أرجع پایله;
    }
    أرجع CURLE_NOT_BUILT_دننه;
  حالة CURLOPT_CRLFILE:
    /*
     * Set CRL file info for SSL connection. Specify filename of the CRL
     * to check certificates revocation
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_CRLFILE))
      أرجع Curl_setstropt(معلومات, STRING_SSL_CRLFILE, ptr);
    أرجع CURLE_NOT_BUILT_دننه;
  حالة CURLOPT_SSL_CIPHER_LIST:
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_CIPHER_LIST))
      /* set a list of cipher we want to use in the SSL connection */
      أرجع Curl_setstropt(معلومات, STRING_SSL_CIPHER_LIST, ptr);
    وإلا
      أرجع CURLE_NOT_BUILT_دننه;
  حالة CURLOPT_TLS13_CIPHERS:
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites */
      أرجع Curl_setstropt(معلومات, STRING_SSL_CIPHER13_LIST, ptr);
    وإلا
      أرجع CURLE_NOT_BUILT_دننه;
  حالة CURLOPT_RANDOM_FILE:
    اكسر;
  حالة CURLOPT_EGDSOCKET:
    اكسر;
  حالة CURLOPT_SSL_CTX_معلومات:
    /*
     * Set an SSL_CTX callback parameter pointer
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_SSL_CTX)) {
      معلومات->سېټ.ssl_fsslctxp = ptr;
      اكسر;
    }
    وإلا
      أرجع CURLE_NOT_BUILT_دننه;
  حالة CURLOPT_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use
     */
    أرجع Curl_setstropt(معلومات, STRING_CERT, ptr);
  حالة CURLOPT_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use
     */
    أرجع Curl_setstropt(معلومات, STRING_CERT_TYPE, ptr);
  حالة CURLOPT_SSLKEY:
    /*
     * String that holds filename of the SSL key to use
     */
    أرجع Curl_setstropt(معلومات, STRING_کلی, ptr);
  حالة CURLOPT_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use
     */
    أرجع Curl_setstropt(معلومات, STRING_کلی_TYPE, ptr);
  حالة CURLOPT_SSLENGINE:
    /*
     * String that holds the SSL crypto engine.
     */
    إذا(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      أرجع CURLE_NOT_BUILT_دننه;
    پایله = Curl_setstropt(معلومات, STRING_SSL_ENGINE, ptr);
    اكسر;
  حالة CURLOPT_ISSUERCERT:
    /*
     * Set Issuer certificate file
     * to check certificates issuer
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_ISSUERCERT))
      أرجع Curl_setstropt(معلومات, STRING_SSL_ISSUERCERT, ptr);
    أرجع CURLE_NOT_BUILT_دننه;
  حالة CURLOPT_SSL_EC_CURVES:
    /*
     * Set accepted curves in SSL connection setup.
     * Specify colon-delimited list of curve algorithm names.
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_SSL_EC_CURVES))
      أرجع Curl_setstropt(معلومات, STRING_SSL_EC_CURVES, ptr);
    أرجع CURLE_NOT_BUILT_دننه;
  حالة CURLOPT_SSL_SIGNATURE_ALGORITHMS:
    /*
     * Set accepted signature algorithms.
     * Specify colon-delimited list of signature scheme names.
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_SIGNATURE_ALGORITHMS))
      أرجع Curl_setstropt(معلومات, STRING_SSL_SIGNATURE_ALGORITHMS, ptr);
    أرجع CURLE_NOT_BUILT_دننه;
  حالة CURLOPT_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_PINNEDPUBKEY))
      أرجع Curl_setstropt(معلومات, STRING_SSL_PINNEDPUBLICKEY, ptr);
    أرجع CURLE_NOT_BUILT_دننه;
  حالة CURLOPT_ECH:
    أرجع setopt_ech(معلومات, ptr);
#د_که_پای
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع پایله;
}
#د_که_پای

#إذا !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
ساكن CURLcode setopt_cptr_http_mqtt(بنية Curl_easy *معلومات,
                                      CURLoption option, حرف *ptr)
{
  CURLcode پایله = CURLE_OK;
  بنية UserDefined *s = &معلومات->سېټ;

  بدل(option) {
  حالة CURLOPT_COPYPOSTFIELDS:
    أرجع setopt_copypostfields(ptr, s);

  حالة CURLOPT_POSTFIELDS:
    /*
     * Like above, but use static data instead of copying it.
     */
    s->postfields = ptr;
    /* Release old copied data. */
    curlx_safefree(s->str_copypostfields);
    s->لاره = HTTPREQ_POST;
    اكسر;

#که_نه_تعریف_شوی CURL_DISABLE_HTTP
  حالة CURLOPT_TRAILERDATA:
    s->trailer_معلومات = ptr;
    اكسر;
  حالة CURLOPT_ACCEPT_ENCODING:
    /*
     * String to use at the value of Accept-Encoding header.
     *
     * If the encoding is set to "" we use an Accept-Encoding header that
     * encompasses all the encodings we support.
     * If the encoding is set to NULL we do not send an Accept-Encoding header
     * and ignore any received Content-Encoding header.
     *
     */
    إذا(ptr && !*ptr) {
      ptr = Curl_get_content_encodings();
      إذا(ptr)
        پایله = CURL_EASY_STR_SETN(معلومات, STRING_ENCODING, ptr);
      وإلا
        پایله = CURLE_بهر_د_حافظه;
      أرجع پایله;
    }
    أرجع Curl_setstropt(معلومات, STRING_ENCODING, ptr);

#که_نه_تعریف_شوی CURL_DISABLE_AWS
  حالة CURLOPT_AWS_SIGV4:
    /*
     * String that is merged to some authentication
     * parameters are used by the algorithm.
     */
    پایله = Curl_setstropt(معلومات, STRING_AWS_SIGV4, ptr);
    /*
     * Basic has been set by default; it needs to be unset here.
     */
    إذا(CURL_EASY_STR(معلومات, STRING_AWS_SIGV4))
      s->httpauth = CURLAUTH_AWS_SIGV4;
    وإلا
      s->httpauth &= ~(uint32_t)CURLAUTH_AWS_SIGV4;
    اكسر;
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_HTTPSIG
  حالة CURLOPT_HTTPSIG_کلی:
    پایله = Curl_setstropt(معلومات, STRING_HTTPSIG_کلی, ptr);
    اكسر;
  حالة CURLOPT_HTTPSIG_KEYID:
    پایله = Curl_setstropt(معلومات, STRING_HTTPSIG_KEYID, ptr);
    اكسر;
  حالة CURLOPT_HTTPSIG_سرلیکونه:
    پایله = Curl_setstropt(معلومات, STRING_HTTPSIG_سرلیکونه, ptr);
    اكسر;
#د_که_پای
  حالة CURLOPT_REFERER: {
    /*
     * String to set in the HTTP Referer: field.
     */
    بنية bufref *oldref = &معلومات->state.referer;
    /* free the old after the storing the new in case the input is actually
       pointing back to this */
    پایله = Curl_setstropt(معلومات, STRING_سېټ_REFERER, ptr);
    Curl_bufref_free(oldref);
    اكسر;
  }

  حالة CURLOPT_USERAGENT:
    /*
     * String to use in the HTTP User-Agent field
     */
    أرجع Curl_setstropt(معلومات, STRING_USERAGENT, ptr);

#که_نه_تعریف_شوی CURL_DISABLE_COOKIES
  حالة CURLOPT_COOKIE:
    /*
     * Cookie string to send to the remote server in the request.
     */
    أرجع Curl_setstropt(معلومات, STRING_COOKIE, ptr);

  حالة CURLOPT_COOKIEFILE:
    أرجع cookiefile(معلومات, ptr);

  حالة CURLOPT_COOKIEJAR:
    /*
     * Set cookie filename to dump all cookies to when we are done.
     */
    پایله = Curl_setstropt(معلومات, STRING_COOKIEJAR, ptr);
    إذا(!پایله) {
      /*
       * Activate the cookie parser. This may or may not already
       * have been made.
       */
      إذا(!معلومات->cookies)
        معلومات->cookies = Curl_cookie_init();
      إذا(!معلومات->cookies)
        پایله = CURLE_بهر_د_حافظه;
      وإلا
        معلومات->state.cookie_engine = TRUE;
    }
    اكسر;

  حالة CURLOPT_COOKIELIST:
    أرجع cookielist(معلومات, ptr);
#د_که_پای /* !CURL_DISABLE_COOKIES */

#د_که_پای /* !CURL_DISABLE_HTTP */
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع پایله;
}
#د_که_پای /* !CURL_DISABLE_HTTP || !CURL_DISABLE_MQTT */

#که_تعریف_شوی USE_SSH
ساكن CURLcode setopt_cptr_ssh(بنية Curl_easy *معلومات, CURLoption option,
                                حرف *ptr)
{
  بنية UserDefined *s = &معلومات->سېټ;
  بدل(option) {
  حالة CURLOPT_SSH_عامه_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa.pub file
     */
    أرجع Curl_setstropt(معلومات, STRING_SSH_عامه_کلی, ptr);
  حالة CURLOPT_SSH_شخصي_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa file
     */
    أرجع Curl_setstropt(معلومات, STRING_SSH_شخصي_کلی, ptr);
  حالة CURLOPT_SSH_KEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_keyfunc_userp = ptr;
    اكسر;
  حالة CURLOPT_SSH_HOST_عامه_کلی_MD5:
    /*
     * Option to allow for the MD5 of the host public key to be checked
     * for validation purposes.
     */
    أرجع Curl_setstropt(معلومات, STRING_SSH_HOST_عامه_کلی_MD5, ptr);
  حالة CURLOPT_SSH_HOST_عامه_کلی_SHA256:
    /*
     * Option to allow for the SHA256 of the host public key to be checked
     * for validation purposes.
     */
    أرجع Curl_setstropt(معلومات, STRING_SSH_HOST_عامه_کلی_SHA256, ptr);
  حالة CURLOPT_SSH_KNOWNHOSTS:
    /*
     * Store the filename to read known hosts from.
     */
    أرجع Curl_setstropt(معلومات, STRING_SSH_KNOWNHOSTS, ptr);
#که_تعریف_شوی USE_LIBSSH2
  حالة CURLOPT_SSH_HOSTKEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_hostkeyfunc_userp = ptr;
    اكسر;
#د_که_پای /* USE_LIBSSH2 */
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع CURLE_OK;
}
#د_که_پای /* USE_SSH */

#که_نه_تعریف_شوی CURL_DISABLE_FTP
ساكن CURLcode setopt_cptr_ftp(بنية Curl_easy *معلومات, CURLoption option,
                                حرف *ptr)
{
  CURLcode پایله = CURLE_OK;
  بنية UserDefined *s = &معلومات->سېټ;
  بدل(option) {
  حالة CURLOPT_FTPPORT:
    /*
     * Use FTP PORT, this also specifies which IP address to use
     */
    پایله = Curl_setstropt(معلومات, STRING_FTPPORT, ptr);
    s->ftp_use_پورټ = !!CURL_EASY_STR(معلومات, STRING_FTPPORT);
    اكسر;

  حالة CURLOPT_FTP_ACCOUNT:
    أرجع Curl_setstropt(معلومات, STRING_FTP_ACCOUNT, ptr);

  حالة CURLOPT_FTP_ALTERNATIVE_منزل_USER:
    أرجع Curl_setstropt(معلومات, STRING_FTP_ALTERNATIVE_منزل_USER, ptr);

  حالة CURLOPT_KRBLEVEL:
    أرجع CURLE_NOT_BUILT_دننه; /* removed in 8.17.0 */
  حالة CURLOPT_CHUNK_معلومات:
    s->wildcardptr = ptr;
    اكسر;
  حالة CURLOPT_FNMATCH_معلومات:
    s->fnmatch_معلومات = ptr;
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع پایله;
}
#د_که_پای /* !CURL_DISABLE_FTP */

ساكن CURLcode setopt_cptr_net(بنية Curl_easy *معلومات, CURLoption option,
                                حرف *ptr)
{
  بدل(option) {
  حالة CURLOPT_INTERFACE:
    /*
     * Set what interface or address/hostname to bind the socket to when
     * performing an operation and thus what from-IP your connection will use.
     */
    أرجع setstropt_interface(معلومات, ptr);

#که_تعریف_شوی USE_RESOLV_ARES
  حالة CURLOPT_DNS_SERVERS:
    أرجع Curl_setstropt(معلومات, STRING_DNS_SERVERS, ptr);

  حالة CURLOPT_DNS_INTERFACE:
    أرجع Curl_setstropt(معلومات, STRING_DNS_INTERFACE, ptr);

  حالة CURLOPT_DNS_LOCAL_IP4:
    أرجع Curl_setstropt(معلومات, STRING_DNS_LOCAL_IP4, ptr);

  حالة CURLOPT_DNS_LOCAL_IP6:
    أرجع Curl_setstropt(معلومات, STRING_DNS_LOCAL_IP6, ptr);
#د_که_پای
#که_تعریف_شوی USE_UNIX_SOCKETS
  حالة CURLOPT_UNIX_SOCKET_لار:
    معلومات->سېټ.abstract_unix_socket = FALSE;
    أرجع Curl_setstropt(معلومات, STRING_UNIX_SOCKET_لار, ptr);

  حالة CURLOPT_ABSTRACT_UNIX_SOCKET:
    معلومات->سېټ.abstract_unix_socket = TRUE;
    أرجع Curl_setstropt(معلومات, STRING_UNIX_SOCKET_لار, ptr);
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_DOH
  حالة CURLOPT_DOH_URL:
    {
      CURLcode پایله = Curl_setstropt(معلومات, STRING_DOH, ptr);
      معلومات->سېټ.doh = !!CURL_EASY_STR(معلومات, STRING_DOH);
      أرجع پایله;
    }
#د_که_پای
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
}

ساكن CURLcode setopt_cptr_misc(بنية Curl_easy *معلومات, CURLoption option,
                                 حرف *ptr)
{
  CURLcode پایله = CURLE_OK;
  بنية UserDefined *s = &معلومات->سېټ;

  بدل(option) {
  حالة CURLOPT_REQUEST_موخه:
    أرجع Curl_setstropt(معلومات, STRING_موخه, ptr);
#که_نه_تعریف_شوی CURL_DISABLE_NETRC
  حالة CURLOPT_NETRC_FILE:
    أرجع Curl_setstropt(معلومات, STRING_NETRC_FILE, ptr);
#د_که_پای
  حالة CURLOPT_CUSTOMREQUEST:
    أرجع Curl_setstropt(معلومات, STRING_CUSTOMREQUEST, ptr);

    /* we do not set s->method = HTTPREQ_CUSTOM; here, we continue as if we
       were using the already set type and this changes the actual request
       keyword */
  حالة CURLOPT_SERVICE_NAME:
    أرجع Curl_setstropt(معلومات, STRING_SERVICE_NAME, ptr);

  حالة CURLOPT_HEADERDATA:
    s->writeheader = ptr;
    اكسر;
  حالة CURLOPT_READDATA:
    s->دننه_سېټ = ptr;
    اكسر;
  حالة CURLOPT_WRITEDATA:
    s->بهر = ptr;
    اكسر;
  حالة CURLOPT_DEBUGDATA:
    s->debugdata = ptr;
    اكسر;
  حالة CURLOPT_PROGRESSDATA:
    s->پرمختګ_client = ptr;
    اكسر;
  حالة CURLOPT_SEEKDATA:
    s->seek_client = ptr;
    اكسر;
  حالة CURLOPT_IOCTLDATA:
    s->ioctl_client = ptr;
    اكسر;
  حالة CURLOPT_SOCKOPTDATA:
    s->sockopt_client = ptr;
    اكسر;
  حالة CURLOPT_OPENSOCKETDATA:
    s->opensocket_client = ptr;
    اكسر;
  حالة CURLOPT_RESOLVER_پیل_معلومات:
    s->resolver_پیل_client = ptr;
    اكسر;
  حالة CURLOPT_CLOSESOCKETDATA:
    s->closesocket_client = ptr;
    اكسر;
  حالة CURLOPT_PREREQDATA:
    s->prereq_userp = ptr;
    اكسر;
  حالة CURLOPT_ERRORBUFFER:
    s->errorbuffer = ptr;
    اكسر;
  حالة CURLOPT_URL:
    پایله = Curl_setstropt(معلومات, STRING_سېټ_URL, ptr);
    Curl_bufref_سېټ(&معلومات->state.url,
                    CURL_EASY_STR(معلومات, STRING_سېټ_URL), 0, عدم);
    اكسر;

  حالة CURLOPT_USERPWD: {
    حرف *u = عدم, *p = عدم;
    پایله = setstropt_userpwd(ptr, &u, &p);
    إذا(!پایله) {
      پایله = CURL_EASY_STR_SETN(معلومات, STRING_USERNAME, u);
      u = عدم;
    }
    إذا(!پایله) {
      پایله = CURL_EASY_STR_SETN(معلومات, STRING_PASSWORD, p);
      p = عدم;
    }
    curlx_free(u);
    curlx_free(p);
    أرجع پایله;
  }

  حالة CURLOPT_USERNAME:
    أرجع Curl_setstropt(معلومات, STRING_USERNAME, ptr);

  حالة CURLOPT_PASSWORD:
    أرجع Curl_setstropt(معلومات, STRING_PASSWORD, ptr);

  حالة CURLOPT_LOGIN_OPTIONS:
    أرجع Curl_setstropt(معلومات, STRING_OPTIONS, ptr);

  حالة CURLOPT_XOAUTH2_BEARER:
    أرجع Curl_setstropt(معلومات, STRING_BEARER, ptr);
  حالة CURLOPT_RANGE:
    أرجع Curl_setstropt(معلومات, STRING_سېټ_RANGE, ptr);
  حالة CURLOPT_شخصي:
    s->شخصي_معلومات = ptr;
    اكسر;
  حالة CURLOPT_PROTOCOLS_STR:
    إذا(ptr) {
      curl_prot_t protos;
      پایله = protocol2num(ptr, &protos);
      إذا(!پایله)
        s->allowed_protocols = protos;
    }
    وإلا
      /* make a NULL argument reset to default */
      s->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
    اكسر;
  حالة CURLOPT_REDIR_PROTOCOLS_STR:
    إذا(ptr) {
      curl_prot_t protos;
      پایله = protocol2num(ptr, &protos);
      إذا(!پایله)
        s->redir_protocols = protos;
    }
    وإلا
      /* make a NULL argument reset to default */
      s->redir_protocols = (curl_prot_t)CURLPROTO_REDIR;
    اكسر;
  حالة CURLOPT_DEFAULT_PROTOCOL:
    /* Set the protocol to use when the URL does not include any protocol */
    أرجع Curl_setstropt(معلومات, STRING_DEFAULT_PROTOCOL, ptr);
#که_نه_تعریف_شوی CURL_DISABLE_SMTP
  حالة CURLOPT_MAIL_FROM:
    /* Set the SMTP mail originator */
    أرجع Curl_setstropt(معلومات, STRING_MAIL_FROM, ptr);
  حالة CURLOPT_MAIL_AUTH:
    /* Set the SMTP auth originator */
    أرجع Curl_setstropt(معلومات, STRING_MAIL_AUTH, ptr);
#د_که_پای
  حالة CURLOPT_SASL_AUTHZID:
    /* Authorization identity (identity to act as) */
    أرجع Curl_setstropt(معلومات, STRING_SASL_AUTHZID, ptr);
#که_نه_تعریف_شوی CURL_DISABLE_RTSP
  حالة CURLOPT_RTSP_SESSION_ID:
    Curl_peer_unlink(&معلومات->state.rtsp_session_origin);
    أرجع Curl_setstropt(معلومات, STRING_RTSP_SESSION_ID, ptr);
  حالة CURLOPT_RTSP_بهیر_URI:
    أرجع Curl_setstropt(معلومات, STRING_RTSP_بهیر_URI, ptr);
  حالة CURLOPT_RTSP_TRANSPORT:
    أرجع Curl_setstropt(معلومات, STRING_RTSP_TRANSPORT, ptr);
  حالة CURLOPT_INTERLEAVEDATA:
    s->rtp_بهر = ptr;
    اكسر;
#د_که_پای /* !CURL_DISABLE_RTSP */
  حالة CURLOPT_TLSAUTH_USERNAME:
  حالة CURLOPT_TLSAUTH_PASSWORD:
  حالة CURLOPT_TLSAUTH_TYPE:
  حالة CURLOPT_PROXY_TLSAUTH_USERNAME:
  حالة CURLOPT_PROXY_TLSAUTH_PASSWORD:
  حالة CURLOPT_PROXY_TLSAUTH_TYPE:
    أرجع CURLE_NOT_BUILT_دننه;
#که_نه_تعریف_شوی CURL_DISABLE_HSTS
  حالة CURLOPT_HSTSREADDATA:
    s->hsts_read_userp = ptr;
    اكسر;
  حالة CURLOPT_HSTSWRITEDATA:
    s->hsts_write_userp = ptr;
    اكسر;
  حالة CURLOPT_HSTS: {
    بنية curl_slist *h;
    إذا(!معلومات->hsts) {
      معلومات->hsts = Curl_hsts_init();
      إذا(!معلومات->hsts)
        أرجع CURLE_بهر_د_حافظه;
    }
    إذا(ptr) {
      پایله = Curl_setstropt(معلومات, STRING_HSTS, ptr);
      إذا(پایله)
        أرجع پایله;
      /* this needs to build a list of filenames to read from, so that it can
         read them later, as we might get a shared HSTS handle to load them
         into */
      h = curl_slist_append(معلومات->state.hstslist, ptr);
      إذا(!h) {
        curl_slist_free_all(معلومات->state.hstslist);
        معلومات->state.hstslist = عدم;
        أرجع CURLE_بهر_د_حافظه;
      }
      معلومات->state.hstslist = h; /* store the list for later use */
    }
    وإلا {
      /* clear the list of HSTS files */
      curl_slist_free_all(معلومات->state.hstslist);
      معلومات->state.hstslist = عدم;
      إذا(!معلومات->share || !معلومات->share->hsts)
        /* throw away the HSTS cache unless shared */
        Curl_hsts_cleanup(&معلومات->hsts);
    }
    اكسر;
  }
#د_که_پای /* !CURL_DISABLE_HSTS */
#که_نه_تعریف_شوی CURL_DISABLE_ALTSVC
  حالة CURLOPT_ALTSVC:
    إذا(!معلومات->asi) {
      معلومات->asi = Curl_altsvc_init();
      إذا(!معلومات->asi)
        أرجع CURLE_بهر_د_حافظه;
    }
    پایله = Curl_setstropt(معلومات, STRING_ALTSVC, ptr);
    إذا(پایله)
      اكسر;
    إذا(ptr)
      أرجع Curl_altsvc_بارول(معلومات->asi, ptr);
    اكسر;
#د_که_پای /* !CURL_DISABLE_ALTSVC */
  حالة CURLOPT_ECH:
    أرجع setopt_ech(معلومات, ptr);
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع پایله;
}

ساكن CURLcode setopt_cptr(بنية Curl_easy *معلومات, CURLoption option,
                            حرف *ptr)
{
  تعريفنوع CURLcode (*ptrfunc)(بنية Curl_easy *معلومات, CURLoption option,
                              حرف *ptr);
  /* Order by likeliness */
  ساكن ثابت ptrfunc setopt_call[] = {
    setopt_cptr_misc,
#إذا defined(USE_SSL) || defined(USE_SSH)
    setopt_cptr_ssl,
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
    setopt_cptr_proxy,
#د_که_پای
    setopt_cptr_net,
#که_نه_تعریف_شوی CURL_DISABLE_FTP
    setopt_cptr_ftp,
#د_که_پای
#که_تعریف_شوی USE_SSH
    setopt_cptr_ssh,
#د_که_پای
#إذا !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
    setopt_cptr_http_mqtt,
#د_که_پای
  };
  size_t i;

  لكل(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode پایله = setopt_call[i](معلومات, option, ptr);
    إذا(پایله != CURLE_UNKNOWN_OPTION)
      أرجع پایله;
  }
  أرجع CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode setopt_func(بنية Curl_easy *معلومات, CURLoption option,
                            va_list param)
{
  بنية UserDefined *s = &معلومات->سېټ;
  بدل(option) {
  حالة CURLOPT_PROGRESSFUNCTION:
    /*
     * Progress callback function
     */
    s->fprogress = va_arg(param, curl_پرمختګ_callback);
    إذا(s->fprogress)
      معلومات->پرمختګ.callback = TRUE; /* no longer internal */
    وإلا
      معلومات->پرمختګ.callback = FALSE; /* NULL enforces internal */
    اكسر;

  حالة CURLOPT_XFERINFOFUNCTION:
    /*
     * Transfer info callback function
     */
    s->fxferinfo = va_arg(param, curl_xferinfo_callback);
    إذا(s->fxferinfo)
      معلومات->پرمختګ.callback = TRUE; /* no longer internal */
    وإلا
      معلومات->پرمختګ.callback = FALSE; /* NULL enforces internal */

    اكسر;
  حالة CURLOPT_DEBUGFUNCTION:
    /*
     * stderr write callback.
     */
    s->fdebug = va_arg(param, curl_debug_callback);
    /*
     * if the callback provided is NULL, it will use the default callback
     */
    اكسر;
  حالة CURLOPT_HEADERFUNCTION:
    /*
     * Set header write callback
     */
    s->fwrite_سرلیک = va_arg(param, curl_write_callback);
    اكسر;
  حالة CURLOPT_WRITEFUNCTION:
    /*
     * Set data write callback
     */
    s->fwrite_func = va_arg(param, curl_write_callback);
    إذا(!s->fwrite_func)
#إذا defined(__clang__) && __clang_major__ >= 16
#لارښوونه clang diagnostic ټېل
#لارښوونه clang diagnostic ignored "-Wcast-function-type-strict"
#د_که_پای
      /* When set to NULL, reset to our internal default function */
      s->fwrite_func = (curl_write_callback)fwrite;
#إذا defined(__clang__) && __clang_major__ >= 16
#لارښوونه clang diagnostic راوباسه
#د_که_پای
    اكسر;
  حالة CURLOPT_READFUNCTION:
    /*
     * Read data callback
     */
    s->fread_func_سېټ = va_arg(param, curl_read_callback);
    إذا(!s->fread_func_سېټ) {
      s->is_fread_سېټ = 0;
#إذا defined(__clang__) && __clang_major__ >= 16
#لارښوونه clang diagnostic ټېل
#لارښوونه clang diagnostic ignored "-Wcast-function-type-strict"
#د_که_پای
      /* When set to NULL, reset to our internal default function */
      s->fread_func_سېټ = (curl_read_callback)fread;
#إذا defined(__clang__) && __clang_major__ >= 16
#لارښوونه clang diagnostic راوباسه
#د_که_پای
    }
    وإلا
      s->is_fread_سېټ = 1;
    اكسر;
  حالة CURLOPT_SEEKFUNCTION:
    /*
     * Seek callback. Might be NULL.
     */
    s->seek_func = va_arg(param, curl_seek_callback);
    اكسر;
  حالة CURLOPT_IOCTLFUNCTION:
    /*
     * I/O control callback. Might be NULL.
     */
    s->ioctl_func = va_arg(param, curl_ioctl_callback);
    اكسر;
  حالة CURLOPT_SSL_CTX_FUNCTION:
    /*
     * Set an SSL_CTX callback
     */
#که_تعریف_شوی USE_SSL
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_SSL_CTX)) {
      s->ssl_fsslctx = va_arg(param, curl_ssl_ctx_callback);
      اكسر;
    }
    وإلا
#د_که_پای
      أرجع CURLE_NOT_BUILT_دننه;

  حالة CURLOPT_SOCKOPTFUNCTION:
    /*
     * socket callback function: called after socket() but before connect()
     */
    s->fsockopt = va_arg(param, curl_sockopt_callback);
    اكسر;

  حالة CURLOPT_OPENSOCKETFUNCTION:
    /*
     * open/create socket callback function: called instead of socket(),
     * before connect()
     */
    s->fopensocket = va_arg(param, curl_opensocket_callback);
    اكسر;

  حالة CURLOPT_CLOSESOCKETFUNCTION:
    /*
     * close socket callback function: called instead of close()
     * when shutting down a connection
     */
    s->fclosesocket = va_arg(param, curl_closesocket_callback);
    اكسر;

  حالة CURLOPT_RESOLVER_پیل_FUNCTION:
    /*
     * resolver start callback function: called before a new resolver request
     * is started
     */
    s->resolver_پیل = va_arg(param, curl_resolver_پیل_callback);
    اكسر;

#که_تعریف_شوی USE_SSH
#که_تعریف_شوی USE_LIBSSH2
  حالة CURLOPT_SSH_HOSTKEYFUNCTION:
    /* the callback to check the hostkey without the knownhost file */
    s->ssh_hostkeyfunc = va_arg(param, curl_sshhostkeycallback);
    اكسر;
#د_که_پای

  حالة CURLOPT_SSH_KEYFUNCTION:
    /* setting to NULL is fine since the ssh.c functions themselves will
       then revert to use the internal default */
    s->ssh_keyfunc = va_arg(param, curl_sshkeycallback);
    اكسر;

#د_که_پای /* USE_SSH */

#که_نه_تعریف_شوی CURL_DISABLE_RTSP
  حالة CURLOPT_INTERLEAVEFUNCTION:
    /* Set the user defined RTP write function */
    s->fwrite_rtp = va_arg(param, curl_write_callback);
    اكسر;
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_FTP
  حالة CURLOPT_CHUNK_BGN_FUNCTION:
    s->chunk_bgn = va_arg(param, curl_chunk_bgn_callback);
    اكسر;
  حالة CURLOPT_CHUNK_پای_ټکی_FUNCTION:
    s->chunk_پای_ټکی = va_arg(param, curl_chunk_پای_ټکی_callback);
    اكسر;
  حالة CURLOPT_FNMATCH_FUNCTION:
    s->fnmatch = va_arg(param, curl_fnmatch_callback);
    اكسر;
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_HTTP
  حالة CURLOPT_TRAILERFUNCTION:
    s->trailer_callback = va_arg(param, curl_trailer_callback);
    اكسر;
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_HSTS
  حالة CURLOPT_HSTSREADFUNCTION:
    s->hsts_read = va_arg(param, curl_hstsread_callback);
    اكسر;
  حالة CURLOPT_HSTSWRITEFUNCTION:
    s->hsts_write = va_arg(param, curl_hstswrite_callback);
    اكسر;
#د_که_پای
  حالة CURLOPT_PREREQFUNCTION:
    s->fprereq = va_arg(param, curl_prereq_callback);
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode setopt_offt(بنية Curl_easy *معلومات, CURLoption option,
                            curl_off_t offt)
{
  بنية UserDefined *s = &معلومات->سېټ;
  بدل(option) {
  حالة CURLOPT_TIMEVALUE_LARGE:
    /*
     * This is the value to compare with the remote document with the
     * method set with CURLOPT_TIMECONDITION
     */
    s->timevalue = (time_t)offt;
    اكسر;

    /* MQTT "borrows" some of the HTTP options */
  حالة CURLOPT_POSTFIELDSIZE_LARGE:
    /*
     * The size of the POSTFIELD data to prevent libcurl to do strlen() to
     * figure it out. Enables binary posts.
     */
    إذا(offt < -1)
      أرجع CURLE_BAD_FUNCTION_دلیل;

    إذا(s->postfieldsize < offt && s->str_copypostfields) {
      /* Previous CURLOPT_COPYPOSTFIELDS is no longer valid. */
      curlx_safefree(s->str_copypostfields);
      s->postfields = عدم;
    }
    s->postfieldsize = offt;
    اكسر;
  حالة CURLOPT_INFILESIZE_LARGE:
    /*
     * If known, this should inform curl about the file size of the
     * to-be-uploaded file.
     */
    إذا(offt < -1)
      أرجع CURLE_BAD_FUNCTION_دلیل;
    s->filesize = offt;
    اكسر;
  حالة CURLOPT_MAX_SEND_چټکتیا_LARGE:
    /*
     * When transfer uploads are faster than CURLOPT_MAX_SEND_SPEED_LARGE
     * bytes per second the transfer is throttled..
     */
    إذا(offt < 0)
      أرجع CURLE_BAD_FUNCTION_دلیل;
    s->max_send_چټکتیا = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&معلومات->پرمختګ.ul.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    اكسر;
  حالة CURLOPT_MAX_RECV_چټکتیا_LARGE:
    /*
     * When receiving data faster than CURLOPT_MAX_RECV_SPEED_LARGE bytes per
     * second the transfer is throttled..
     */
    إذا(offt < 0)
      أرجع CURLE_BAD_FUNCTION_دلیل;
    s->max_recv_چټکتیا = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&معلومات->پرمختګ.dl.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    اكسر;
  حالة CURLOPT_RESUME_FROM_LARGE:
    /*
     * Resume transfer at the given file position
     */
    إذا(offt < -1)
      أرجع CURLE_BAD_FUNCTION_دلیل;
    s->سېټ_resume_from = offt;
    اكسر;
  حالة CURLOPT_MAXFILESIZE_LARGE:
    /*
     * Set the maximum size of a file to download.
     */
    إذا(offt < 0)
      أرجع CURLE_BAD_FUNCTION_دلیل;
    s->max_filesize = offt;
    اكسر;

  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode setopt_blob(بنية Curl_easy *معلومات, CURLoption option,
                            بنية curl_blob *blob)
{
  بنية UserDefined *s = &معلومات->سېټ;
  بدل(option) {
  حالة CURLOPT_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use
     */
    أرجع Curl_setblobopt(&s->blobs[BLOB_CERT], blob);
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  حالة CURLOPT_PROXY_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use for proxy
     */
    أرجع Curl_setblobopt(&s->blobs[BLOB_CERT_PROXY], blob);
  حالة CURLOPT_PROXY_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use for proxy
     */
    أرجع Curl_setblobopt(&s->blobs[BLOB_کلی_PROXY], blob);
  حالة CURLOPT_PROXY_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection proxy.
     * Specify entire PEM of the CA certificate
     */
#که_تعریف_شوی USE_SSL
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_CAINFO_BLOB)) {
      CURLcode پایله = Curl_setblobopt(&s->blobs[BLOB_CAINFO_PROXY], blob);
      إذا(!پایله)
        s->proxy_ssl.custom_cablob = TRUE;
      أرجع پایله;
    }
#د_که_پای
    أرجع CURLE_NOT_BUILT_دننه;
  حالة CURLOPT_PROXY_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_ISSUERCERT_BLOB))
      أرجع Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT_PROXY], blob);
    أرجع CURLE_NOT_BUILT_دننه;
#د_که_پای
  حالة CURLOPT_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use
     */
    أرجع Curl_setblobopt(&s->blobs[BLOB_کلی], blob);
  حالة CURLOPT_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection.
     * Specify entire PEM of the CA certificate
     */
#که_تعریف_شوی USE_SSL
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_CAINFO_BLOB)) {
      CURLcode پایله = Curl_setblobopt(&s->blobs[BLOB_CAINFO], blob);
      إذا(!پایله)
        s->ssl.custom_cablob = TRUE;
      أرجع پایله;
    }
#د_که_پای
    أرجع CURLE_NOT_BUILT_دننه;
  حالة CURLOPT_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_ISSUERCERT_BLOB))
      أرجع Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT], blob);
    أرجع CURLE_NOT_BUILT_دننه;

  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  /* unreachable */
}

/*
 * Do not make Curl_vsetopt() static: it is called from
 * projects/OS400/ccsidcurl.c.
 */
CURLcode Curl_vsetopt(بنية Curl_easy *معلومات, CURLoption option, va_list param)
{
  إذا(option < CURLOPTTYPE_OBJECTPOINT)
    أرجع setopt_long(معلومات, option, va_arg(param, اوږد));
  وإلا إذا(option < CURLOPTTYPE_FUNCTIONPOINT) {
    /* unfortunately, different pointer types cannot be identified any other
       way than being listed explicitly */
    بدل(option) {
    حالة CURLOPT_HTTPHEADER:
    حالة CURLOPT_QUOTE:
    حالة CURLOPT_POSTQUOTE:
    حالة CURLOPT_TELNETOPTIONS:
    حالة CURLOPT_PREQUOTE:
    حالة CURLOPT_HTTP200ALIASES:
    حالة CURLOPT_MAIL_RCPT:
    حالة CURLOPT_RESOLVE:
    حالة CURLOPT_PROXYHEADER:
    حالة CURLOPT_CONNECT_منزل:
      أرجع setopt_slist(معلومات, option, va_arg(param, بنية curl_slist *));
    حالة CURLOPT_HTTPPOST:         /* curl_httppost * */
    حالة CURLOPT_MIMEPOST:         /* curl_mime * */
    حالة CURLOPT_STDERR:           /* FILE * */
    حالة CURLOPT_SHARE:            /* CURLSH * */
    حالة CURLOPT_CURLU:            /* CURLU * */
      أرجع setopt_اشارې(معلومات, option, param);
    حالة CURLOPT_بهیر_DEPENDS:   /* CURL * */
    حالة CURLOPT_بهیر_DEPENDS_E: /* CURL * */
      أرجع CURLE_OK;
    افتراضي:
      اكسر;
    }
    /* the char pointer options */
    أرجع setopt_cptr(معلومات, option, va_arg(param, حرف *));
  }
  وإلا إذا(option < CURLOPTTYPE_OFF_T)
    أرجع setopt_func(معلومات, option, param);
  وإلا إذا(option < CURLOPTTYPE_BLOB)
    أرجع setopt_offt(معلومات, option, va_arg(param, curl_off_t));
  أرجع setopt_blob(معلومات, option, va_arg(param, بنية curl_blob *));
}

/*
 * curl_easy_setopt() is the external interface for setting options on an
 * easy handle.
 *
 * NOTE: This is one of few API functions that are allowed to be called from
 * within a callback.
 */
#تعریف_لغوه curl_easy_setopt
CURLcode curl_easy_setopt(CURL *curl, CURLoption option, ...)
{
  بنية Curl_eapi_guard guard;
  CURLcode پایله;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_setopt, &پایله)) {
    بنية Curl_easy *معلومات = curl;
    va_list arg;

    va_start(arg, option);

    پایله = Curl_vsetopt(معلومات, option, arg);

    va_end(arg);
    إذا(پایله == CURLE_BAD_FUNCTION_دلیل)
      failf(معلومات, "setopt 0x%x got bad argument", (بینښه صحيح)option);
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع پایله;
}
