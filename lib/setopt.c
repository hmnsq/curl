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

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#ضمن <netinet/in.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_LINUX_TCP_H
#ضمن <linux/tcp.h>
#وإلاإذا defined(HAVE_NETINET_TCP_H)
#ضمن <netinet/tcp.h>
#جيڪڏهن_پورو

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

ساكن CURLcode setopt_سيٽ_timeout_sec(timediff_t *ptimeout_ms, ڊگهو secs)
{
  إذا(secs < 0)
    أرجع CURLE_BAD_FUNCTION_دليل;
#إذا LONG_MAX > (TIMEDIFF_T_MAX / 1000)
  إذا(secs > (TIMEDIFF_T_MAX / 1000)) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    أرجع CURLE_OK;
  }
#جيڪڏهن_پورو
  *ptimeout_ms = (timediff_t)secs * 1000;
  أرجع CURLE_OK;
}

ساكن CURLcode setopt_سيٽ_timeout_ms(timediff_t *ptimeout_ms, ڊگهو ms)
{
  إذا(ms < 0)
    أرجع CURLE_BAD_FUNCTION_دليل;
#إذا LONG_MAX > TIMEDIFF_T_MAX
  إذا(ms > TIMEDIFF_T_MAX) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    أرجع CURLE_OK;
  }
#جيڪڏهن_پورو
  *ptimeout_ms = (timediff_t)ms;
  أرجع CURLE_OK;
}

CURLcode Curl_setstropt(بنية Curl_easy *معلومات,
                        تعداد dupstring id, ثابت حرف *s)
{
  size_t slen = s ? لڙي_ڊيگهه(s) : 0;
  DEBUGASSERT((بنانشان)id <= UINT8_MAX);
  إذا(s && (slen > CURL_MAX_داخلا_ڊيگهائي))
    أرجع CURLE_BAD_FUNCTION_دليل;

  أرجع CURL_EASY_STR_سيٽ(معلومات, (uint8_t)id, s, slen);
}

CURLcode Curl_setblobopt(بنية curl_blob **blobp,
                         ثابت بنية curl_blob *blob)
{
  /* free the previous storage at `blobp' and replace by a dynamic storage
     copy of blob. If CURL_BLOB_COPY is set, the data is copied. */

  curlx_safefree(*blobp);

  إذا(blob) {
    بنية curl_blob *nblob;
    إذا(!blob->معلومات || !blob->len || (blob->len > CURL_MAX_داخلا_ڊيگهائي))
      أرجع CURLE_BAD_FUNCTION_دليل;
    nblob = (بنية curl_blob *)
      curlx_malloc(حجم(بنية curl_blob) +
                   ((blob->flags & CURL_BLOB_COPY) ? blob->len : 0));
    إذا(!nblob)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    *nblob = *blob;
    إذا(blob->flags & CURL_BLOB_COPY) {
      /* put the data after the blob struct in memory */
      nblob->معلومات = (حرف *)nblob + حجم(بنية curl_blob);
      يادگيري_نقل(nblob->معلومات, blob->معلومات, blob->len);
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
    size_t len = لڙي_ڊيگهه(option);
    CURLcode نتيجو;
    إذا(len > CURL_MAX_داخلا_ڊيگهائي)
      أرجع CURLE_BAD_FUNCTION_دليل;

    نتيجو = Curl_parse_login_details(option, len, &user, &passwd, عدم);
    إذا(نتيجو)
      أرجع نتيجو;
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
    CURLcode نتيجو = Curl_parse_interface(option, عدم, عدم, عدم);
    إذا(نتيجو)
      أرجع نتيجو;
    أرجع CURL_EASY_STR_سيٽ(معلومات, STRING_INTERFACE_OPT,
                             option, لڙي_ڊيگهه(option));
  }
  CURL_EASY_STR_CLEAR(معلومات, STRING_INTERFACE_OPT);
  أرجع CURLE_OK;
}

#جيڪڏهن_وصف_ٿيل USE_SSL
#وصف_ڪريو C_SSLVERSION_قدر(x)     ((x) & 0xffff)
#وصف_ڪريو C_SSLVERSION_MAX_قدر(x) ((بنانشان ڊگهو)(x) & 0xffff0000)
#جيڪڏهن_پورو

ساكن CURLcode protocol2num(ثابت حرف *str, curl_prot_t *val)
{
  /*
   * We are asked to cherry-pick protocols, so play it safe and disallow all
   * protocols to start with, and re-add the wanted ones back in.
   */
  *val = 0;

  إذا(!str)
    أرجع CURLE_BAD_FUNCTION_دليل;

  إذا(curl_strequal(str, "all")) {
    *val = ~(curl_prot_t)0;
    أرجع CURLE_OK;
  }

  افعل {
    ثابت حرف *token = str;
    size_t tlen;

    str = strchr(str, ',');
    tlen = str ? (size_t)(str - token) : لڙي_ڊيگهه(token);
    إذا(tlen) {
      ثابت بنية Curl_scheme *h = Curl_getn_scheme(token, tlen);

      إذا(!h || !h->لاڳيتو)
        أرجع CURLE_UNSUPPORTED_PROTOCOL;

      *val |= h->protocol;
    }
  } طالما(str && str++);

  إذا(!*val)
    /* no protocol listed */
    أرجع CURLE_BAD_FUNCTION_دليل;
  أرجع CURLE_OK;
}

#إذا !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
ساكن CURLcode httpauth(بنية Curl_easy *معلومات, bool proxy,
                         بنانشان ڊگهو auth)
{
  إذا(auth != CURLAUTH_NONE) {
    صحيح bitcheck = 0;
    bool authbits = FALSE;
    إذا(auth & CURLAUTH_DIGEST_IE) {
      auth |= CURLAUTH_DIGEST; /* set standard digest bit */
      auth &= ~CURLAUTH_DIGEST_IE; /* drop the legacy bit */
    }

    /* switch off bits we cannot support */
#جيڪڏهن_وصف_نه_ٿيل USE_NTLM
    auth &= ~CURLAUTH_NTLM; /* no NTLM support */
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل USE_SPNEGO
    auth &= ~CURLAUTH_NEGOTIATE; /* no Negotiate (SPNEGO) auth without GSS-API
                                    or SSPI */
#جيڪڏهن_پورو

    /* check if any auth bit lower than CURLAUTH_ONLY is still set */
    طالما(bitcheck < 31) {
      إذا(auth & (1UL << bitcheck++)) {
        authbits = TRUE;
        اكسر;
      }
    }
    إذا(!authbits)
      أرجع CURLE_NOT_BUILT_اندريون; /* no supported types left! */
  }
  إذا(proxy)
    معلومات->سيٽ.proxyauth = (uint32_t)auth;
  وإلا
    معلومات->سيٽ.httpauth = (uint32_t)auth;
  أرجع CURLE_OK;
}
#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP
ساكن CURLcode setopt_HTTP_نسخو(بنية Curl_easy *معلومات, ڊگهو arg)
{
  /*
   * This sets a requested HTTP version to be used. The value is one of
   * the listed enums in curl/curl.h.
   */
  بدل(arg) {
  حالة CURL_HTTP_نسخو_NONE:
    /* accepted */
    اكسر;
  حالة CURL_HTTP_نسخو_1_0:
  حالة CURL_HTTP_نسخو_1_1:
    /* accepted */
    اكسر;
#جيڪڏهن_وصف_ٿيل USE_HTTP2
  حالة CURL_HTTP_نسخو_2_0:
  حالة CURL_HTTP_نسخو_2_TLS:
  حالة CURL_HTTP_نسخو_2_اڳواٽ_KNOWLEDGE:
    /* accepted */
    اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_HTTP3
  حالة CURL_HTTP_نسخو_3:
  حالة CURL_HTTP_نسخو_3_ONLY:
    /* accepted */
    اكسر;
#جيڪڏهن_پورو
  افتراضي:
    /* not accepted */
    إذا(arg < CURL_HTTP_نسخو_NONE)
      أرجع CURLE_BAD_FUNCTION_دليل;
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  }
  معلومات->سيٽ.httpwant = (بنانشان حرف)arg;
  أرجع CURLE_OK;
}
#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP */

#جيڪڏهن_وصف_ٿيل USE_SSL
CURLcode Curl_setopt_SSLVERSION(بنية Curl_easy *معلومات, CURLoption option,
                                ڊگهو arg)
{
  /*
   * Set explicit SSL version to try to connect with, as some SSL
   * implementations are lame.
   */
  {
    ڊگهو نسخو_2, نسخو_max;
    بنية ssl_easy_config *sslc = &معلومات->سيٽ.ssl;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
    إذا(option != CURLOPT_SSLVERSION)
      sslc = &معلومات->سيٽ.proxy_ssl;
#وإلا
    (فراغ)option; /*اڻ_استعمال_ٿيل*/
#جيڪڏهن_پورو
    نسخو_2 = C_SSLVERSION_قدر(arg);
    نسخو_max = (ڊگهو)C_SSLVERSION_MAX_قدر(arg);

    إذا(نسخو_2 < CURL_SSLVERSION_DEFAULT ||
       نسخو_2 == CURL_SSLVERSION_SSLv2 ||
       نسخو_2 == CURL_SSLVERSION_SSLv3 ||
       نسخو_2 >= CURL_SSLVERSION_آخري ||
       نسخو_max < CURL_SSLVERSION_MAX_NONE ||
       نسخو_max >= CURL_SSLVERSION_MAX_آخري)
      أرجع CURLE_BAD_FUNCTION_دليل;
    إذا(نسخو_2 == CURL_SSLVERSION_DEFAULT)
      نسخو_2 = CURL_SSLVERSION_TLSv1_2;

    sslc->نسخو_2 = (بنانشان حرف)نسخو_2;
    sslc->نسخو_max = (بنانشان صحيح)نسخو_max;
  }
  أرجع CURLE_OK;
}
#جيڪڏهن_پورو /* !USE_SSL */

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_RTSP
ساكن CURLcode setopt_RTSP_REQUEST(بنية Curl_easy *معلومات, ڊگهو arg)
{
  /*
   * Set the RTSP request method (OPTIONS, SETUP, PLAY, etc...) Would this be
   * better if the RTSPREQ_* were moved into here?
   */
  إذا((arg <= CURL_RTSPREQ_NONE) || (arg >= CURL_RTSPREQ_آخري))
    أرجع CURLE_BAD_FUNCTION_دليل;

  معلومات->سيٽ.rtspreq = (بنانشان حرف)arg;
  أرجع CURLE_OK;
}
#جيڪڏهن_پورو /* !CURL_DISABLE_RTSP */

ساكن CURLcode setopt_long_bool(بنية Curl_easy *معلومات, CURLoption option,
                                 ڊگهو arg)
{
  bool enabled = !!arg;
  صحيح ok = 1;
  بنية UserDefined *s = &معلومات->سيٽ;
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
  حالة CURLOPT_سرو:
    /*
     * Set to include the header in the general data output stream.
     */
    s->include_سرو = enabled;
    اكسر;
  حالة CURLOPT_NOPROGRESS:
    /*
     * Shut off the internal supported progress meter
     */
    معلومات->ترقي.hide = enabled;
    اكسر;
  حالة CURLOPT_NOBODY:
    /*
     * Do not include the body part in the output data stream.
     */
    s->opt_no_body = enabled;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP
    إذا(s->opt_no_body)
      /* in HTTP lingo, no body means using the HEAD request... */
      s->طريقو = HTTPREQ_سر;
    وإلا إذا(s->طريقو == HTTPREQ_سر)
      s->طريقو = HTTPREQ_GET;
#جيڪڏهن_پورو
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
      s->طريقو = HTTPREQ_PUT;
      s->opt_no_body = FALSE; /* this is implied */
    }
    وإلا
      /* In HTTP, the opposite of upload is GET (unless NOBODY is true as
         then this can be changed to HEAD later on) */
      s->طريقو = HTTPREQ_GET;
    اكسر;
  حالة CURLOPT_FILETIME:
    /*
     * Try to get the file time of the remote document. The time will
     * later (possibly) become available using curl_easy_getinfo().
     */
    s->get_filetime = enabled;
    اكسر;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP
  حالة CURLOPT_HTTP09_ALLOWED:
    s->http09_allowed = enabled;
    اكسر;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_COOKIES
  حالة CURLOPT_COOKIESESSION:
    /*
     * Set this option to TRUE to start a new "cookie session". It will
     * prevent the forthcoming read-cookies-from-file actions to accept
     * cookies that are marked as being session cookies, as they belong to a
     * previous session.
     */
    s->cookiesession = enabled;
    اكسر;
#جيڪڏهن_پورو
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
      s->طريقو = HTTPREQ_GET;
      s->opt_no_body = FALSE; /* this is implied */
    }
    اكسر;
  حالة CURLOPT_POST:
    /* Does this option serve a purpose anymore? Yes it does, when
       CURLOPT_POSTFIELDS is not used and the POST data is read off the
       callback! */
    إذا(enabled) {
      s->طريقو = HTTPREQ_POST;
      s->opt_no_body = FALSE; /* this is implied */
    }
    وإلا
      s->طريقو = HTTPREQ_GET;
    اكسر;
#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP */
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
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
  حالة CURLOPT_PROXY_TRANSFER_گهڻائي:
    /*
     * set transfer mode (;type=<a|i>) when doing FTP via an HTTP proxy
     */
    s->proxy_transfer_گهڻائي = enabled;
    اكسر;
#جيڪڏهن_پورو /* !CURL_DISABLE_PROXY */
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  حالة CURLOPT_SOCKS5_GSSAPI_NEC:
    /*
     * Set flag for NEC SOCKS5 support
     */
    s->socks5_gssapi_nec = enabled;
    اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CURL_LIST_ONLY_PROTOCOL
  حالة CURLOPT_DIRLISTONLY:
    /*
     * An option that changes the command to one that asks for a list only, no
     * file info details. Used for FTP, POP3 and SFTP.
     */
    s->list_only = enabled;
    اكسر;
#جيڪڏهن_پورو
  حالة CURLOPT_APPEND:
    /*
     * We want to upload and append to an existing file. Used for FTP and
     * SFTP.
     */
    s->remote_append = enabled;
    اكسر;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FTP
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
#جيڪڏهن_پورو
  حالة CURLOPT_CRLF:
    /*
     * Kludgy option to enable CRLF conversions. Subject for removal.
     */
    s->crlf = enabled;
    اكسر;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_TFTP
  حالة CURLOPT_TFTP_NO_OPTIONS:
    /*
     * Option that prevents libcurl from sending TFTP option requests to the
     * server.
     */
    s->tftp_no_options = enabled;
    اكسر;
#جيڪڏهن_پورو /* !CURL_DISABLE_TFTP */
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
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DOH
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
      أرجع CURLE_NOT_BUILT_اندريون;

    s->doh_verifystatus = enabled;
    ok = 2;
    اكسر;
#جيڪڏهن_پورو /* !CURL_DISABLE_DOH */
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
      أرجع CURLE_NOT_BUILT_اندريون;

    s->ssl.verifystatus = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(معلومات, FALSE);
    اكسر;
  حالة CURLOPT_CERTINFO:
#جيڪڏهن_وصف_ٿيل USE_SSL
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_CERTINFO))
      s->ssl.certinfo = enabled;
    وإلا
#جيڪڏهن_پورو
      أرجع CURLE_NOT_BUILT_اندريون;
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
  حالة CURLOPT_IGNORE_CONTENT_ڊيگهائي:
    s->ignorecl = enabled;
    اكسر;
  حالة CURLOPT_SSL_SESSIONID_CACHE:
    s->ssl.cache_session = enabled;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
    s->proxy_ssl.cache_session = s->ssl.cache_session;
#جيڪڏهن_پورو
    اكسر;
#جيڪڏهن_وصف_ٿيل USE_SSH
  حالة CURLOPT_SSH_COMPRESSION:
    s->ssh_compression = enabled;
    اكسر;
#جيڪڏهن_پورو /* !USE_SSH */
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_SMTP
  حالة CURLOPT_MAIL_RCPT_ALLOWFAILS:
    /* allow RCPT TO command to fail for some recipients */
    s->mail_rcpt_allowfails = enabled;
    اكسر;
#جيڪڏهن_پورو /* !CURL_DISABLE_SMTP */
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
    أرجع CURLE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
  حالة CURLOPT_SSL_ENABLE_ALPN:
    s->ssl_enable_alpn = enabled;
    اكسر;
  حالة CURLOPT_رستو_AS_IS:
    s->رستو_as_is = enabled;
    اكسر;
  حالة CURLOPT_PIPEWAIT:
    s->pipewait = enabled;
    اكسر;
  حالة CURLOPT_SUPPRESS_CONNECT_سرا:
    s->suppress_connect_سرا = enabled;
    اكسر;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_SHUFFLE_DNS
  حالة CURLOPT_DNS_SHUFFLE_پتا:
    s->dns_shuffle_پتا = enabled;
    اكسر;
#جيڪڏهن_پورو
  حالة CURLOPT_DISALLOW_USERNAME_اندريون_URL:
    s->disallow_username_اندريون_url = enabled;
    اكسر;
  حالة CURLOPT_تيز_EXIT:
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

ساكن CURLcode قدر_range(ڊگهو *قدر, ڊگهو هيٺ_error, ڊگهو min, ڊگهو max)
{
  إذا(*قدر < هيٺ_error)
    أرجع CURLE_BAD_FUNCTION_دليل;
  وإلا إذا(*قدر < min)
    *قدر = min;
  وإلا إذا(*قدر > max)
    *قدر = max;
  أرجع CURLE_OK;
}

ساكن CURLcode setopt_long_net(بنية Curl_easy *معلومات, CURLoption option,
                                ڊگهو arg)
{
  CURLcode نتيجو = CURLE_OK;
  بنية UserDefined *s = &معلومات->سيٽ;

  بدل(option) {
  حالة CURLOPT_DNS_CACHE_TIMEOUT:
    إذا(arg != -1)
      أرجع setopt_سيٽ_timeout_sec(&s->dns_cache_timeout_ms, arg);
    s->dns_cache_timeout_ms = -1;
    اكسر;
  حالة CURLOPT_MAXCONNECTS:
    نتيجو = قدر_range(&arg, 0, 0, وڌ_کان_وڌ_عدد);
    إذا(!نتيجو)
      s->maxconnects = arg ? (uint32_t)arg : DEFAULT_CONNCACHE_سائيز;
    اكسر;
  حالة CURLOPT_SERVER_RESPONSE_TIMEOUT:
    أرجع setopt_سيٽ_timeout_sec(&s->server_response_timeout, arg);
  حالة CURLOPT_SERVER_RESPONSE_TIMEOUT_MS:
    أرجع setopt_سيٽ_timeout_ms(&s->server_response_timeout, arg);
  حالة CURLOPT_هيٺيون_رفتار_حد_بندي:
    إذا(arg < 0)
      نتيجو = CURLE_BAD_FUNCTION_دليل;
    وإلا
      s->هيٺيون_رفتار_حد_بندي = arg;
    اكسر;
  حالة CURLOPT_هيٺيون_رفتار_وقت:
    نتيجو = قدر_range(&arg, 0, 0, USHRT_MAX);
    إذا(!نتيجو)
      s->هيٺيون_رفتار_وقت = (uint16_t)arg;
    اكسر;
  حالة CURLOPT_پورٽ:
    إذا((arg < 0) || (arg > 65535))
      أرجع CURLE_BAD_FUNCTION_دليل;
    s->use_پورٽ = (بنانشان ننڍو)arg;
    اكسر;
  حالة CURLOPT_TIMEOUT:
    أرجع setopt_سيٽ_timeout_sec(&s->timeout, arg);
  حالة CURLOPT_TIMEOUT_MS:
    أرجع setopt_سيٽ_timeout_ms(&s->timeout, arg);
  حالة CURLOPT_CONNECTTIMEOUT:
    أرجع setopt_سيٽ_timeout_sec(&s->connecttimeout, arg);
  حالة CURLOPT_CONNECTTIMEOUT_MS:
    أرجع setopt_سيٽ_timeout_ms(&s->connecttimeout, arg);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_BINDLOCAL
  حالة CURLOPT_LOCALPORT:
    إذا((arg < 0) || (arg > 65535))
      أرجع CURLE_BAD_FUNCTION_دليل;
    s->localport = curlx_sltous(arg);
    اكسر;
  حالة CURLOPT_LOCALPORTRANGE:
    إذا((arg < 0) || (arg > 65535))
      أرجع CURLE_BAD_FUNCTION_دليل;
    s->localportrange = curlx_sltous(arg);
    اكسر;
#جيڪڏهن_پورو
  حالة CURLOPT_BUFFERSIZE:
    نتيجو = قدر_range(&arg, 0, READBUFFER_MIN, READBUFFER_MAX);
    إذا(!نتيجو)
      s->بفر_سائيز = (بنانشان صحيح)arg;
    اكسر;
  حالة CURLOPT_UPLOAD_BUFFERSIZE:
    نتيجو = قدر_range(&arg, 0, UPLOADBUFFER_MIN, UPLOADBUFFER_MAX);
    إذا(!نتيجو)
      s->upload_بفر_سائيز = (بنانشان صحيح)arg;
    اكسر;
  حالة CURLOPT_MAXFILESIZE:
    إذا(arg < 0)
      نتيجو = CURLE_BAD_FUNCTION_دليل;
    وإلا
      s->max_filesize = arg;
    اكسر;
  حالة CURLOPT_IPRESOLVE:
    إذا((arg < CURL_IPRESOLVE_WHATEVER) || (arg > CURL_IPRESOLVE_V6))
      نتيجو = CURLE_BAD_FUNCTION_دليل;
    وإلا
      s->ipver = (بنانشان حرف)arg;
    اكسر;
  حالة CURLOPT_CONNECT_ONLY:
    إذا(arg < 0 || arg > 2)
      نتيجو = CURLE_BAD_FUNCTION_دليل;
    وإلا {
      s->connect_only = !!arg;
      s->connect_only_ws = (arg == 2);
    }
    اكسر;
#جيڪڏهن_وصف_ٿيل USE_IPV6
  حالة CURLOPT_پتو_SCOPE:
#إذا SIZEOF_LONG > 4
    إذا((بنانشان ڊگهو)arg > UINT_MAX)
      نتيجو = CURLE_BAD_FUNCTION_دليل;
    وإلا
#جيڪڏهن_پورو
    s->scope_id = (بنانشان صحيح)arg;
    اكسر;
#جيڪڏهن_پورو
  حالة CURLOPT_TCP_KEEPIDLE:
    نتيجو = قدر_range(&arg, 0, 0, وڌ_کان_وڌ_عدد);
    إذا(!نتيجو)
      s->tcp_keepidle = (صحيح)arg;
    اكسر;
  حالة CURLOPT_TCP_KEEPINTVL:
    نتيجو = قدر_range(&arg, 0, 0, وڌ_کان_وڌ_عدد);
    إذا(!نتيجو)
      s->tcp_keepintvl = (صحيح)arg;
    اكسر;
  حالة CURLOPT_TCP_KEEPCNT:
    نتيجو = قدر_range(&arg, 0, 0, وڌ_کان_وڌ_عدد);
    إذا(!نتيجو)
      s->tcp_keepcnt = (صحيح)arg;
    اكسر;
  حالة CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
    أرجع setopt_سيٽ_timeout_ms(&s->happy_eyeballs_timeout, arg);
  حالة CURLOPT_UPKEEP_INTERVAL_MS:
    أرجع setopt_سيٽ_timeout_ms(&s->upkeep_interval_ms, arg);
  حالة CURLOPT_MAXAGE_CONN:
    أرجع setopt_سيٽ_timeout_sec(&s->conn_max_idle_ms, arg);
  حالة CURLOPT_MAXLIFETIME_CONN:
    أرجع setopt_سيٽ_timeout_sec(&s->conn_max_age_ms, arg);
  حالة CURLOPT_DNS_USE_GLOBAL_CACHE:
    /* deprecated */
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع نتيجو;
}

ساكن CURLcode setopt_long_ssl(بنية Curl_easy *معلومات, CURLoption option,
                                ڊگهو arg)
{
#جيڪڏهن_وصف_ٿيل USE_SSL
  CURLcode نتيجو = CURLE_OK;
  بنية UserDefined *s = &معلومات->سيٽ;
  بدل(option) {
  حالة CURLOPT_CA_CACHE_TIMEOUT:
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_CA_CACHE)) {
      نتيجو = قدر_range(&arg, -1, -1, وڌ_کان_وڌ_عدد);
      إذا(!نتيجو)
        s->ssl_ca_cache_timeout = (صحيح)arg;
    }
    وإلا
      نتيجو = CURLE_NOT_BUILT_اندريون;
    اكسر;
  حالة CURLOPT_SSLVERSION:
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  حالة CURLOPT_PROXY_SSLVERSION:
#جيڪڏهن_پورو
    أرجع Curl_setopt_SSLVERSION(معلومات, option, arg);
  حالة CURLOPT_SSL_FALSESTART:
    نتيجو = CURLE_NOT_BUILT_اندريون;
    اكسر;
  حالة CURLOPT_USE_SSL:
    إذا((arg < CURLUSESSL_NONE) || (arg >= CURLUSESSL_آخري))
      نتيجو = CURLE_BAD_FUNCTION_دليل;
    وإلا
      s->use_ssl = (بنانشان حرف)arg;
    اكسر;
  حالة CURLOPT_SSL_OPTIONS:
    s->ssl.ssl_options = (بنانشان حرف)(arg & 0xff);
    اكسر;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  حالة CURLOPT_PROXY_SSL_OPTIONS:
    s->proxy_ssl.ssl_options = (بنانشان حرف)(arg & 0xff);
    اكسر;
#جيڪڏهن_پورو
  حالة CURLOPT_SSL_ENABLE_NPN:
    اكسر;
  حالة CURLOPT_SSLENGINE_DEFAULT:
#جيڪڏهن_وصف_ٿيل USE_OPENSSL
    إذا(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      أرجع CURLE_NOT_BUILT_اندريون;
    معلومات->سيٽ.engine_default = (arg == 1);
#وإلا
    أرجع CURLE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع نتيجو;
#وإلا  /* USE_SSL */
  (فراغ)معلومات;
  (فراغ)option;
  (فراغ)arg;
  أرجع CURLE_UNKNOWN_OPTION;
#جيڪڏهن_پورو /* !USE_SSL */
}

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
ساكن فراغ changeproxy(بنية Curl_easy *معلومات)
{
  Curl_auth_digest_cleanup(&معلومات->state.proxydigest);
  memset(&معلومات->state.authproxy, 0, حجم(معلومات->state.authproxy));
}

ساكن CURLcode setopt_long_proxy(بنية Curl_easy *معلومات, CURLoption option,
                                  ڊگهو arg)
{
  بنية UserDefined *s = &معلومات->سيٽ;

  بدل(option) {
  حالة CURLOPT_PROXYPORT:
    إذا((arg < 0) || (arg > UINT16_MAX))
      أرجع CURLE_BAD_FUNCTION_دليل;
    إذا(arg != s->proxyport)
      changeproxy(معلومات);
    s->proxyport = (uint16_t)arg;
    اكسر;
  حالة CURLOPT_PROXYAUTH:
    أرجع httpauth(معلومات, TRUE, (بنانشان ڊگهو)arg);
  حالة CURLOPT_PROXYTYPE:
    إذا((arg < CURLPROXY_HTTP) || (arg > CURLPROXY_HTTPS3))
      أرجع CURLE_BAD_FUNCTION_دليل;
#جيڪڏهن_وصف_نه_ٿيل USE_PROXY_HTTP3
    إذا(arg == CURLPROXY_HTTPS3)
      أرجع CURLE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
    s->proxytype = (بنانشان حرف)arg;
    اكسر;
  حالة CURLOPT_SOCKS5_AUTH: {
    بنانشان ڊگهو auth = (بنانشان ڊگهو)arg;
    إذا(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI | CURLAUTH_ONLY))
      أرجع CURLE_NOT_BUILT_اندريون;
    إذا((auth & CURLAUTH_ONLY) && !(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI)))
      أرجع CURLE_BAD_FUNCTION_دليل;
    s->socks5auth = (بنانشان حرف)(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI));
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
                                  ڊگهو arg)
{
  (فراغ)معلومات;
  (فراغ)option;
  (فراغ)arg;
  أرجع CURLE_UNKNOWN_OPTION;
}
#جيڪڏهن_پورو

ساكن CURLcode setopt_long_http(بنية Curl_easy *معلومات, CURLoption option,
                                 ڊگهو arg)
{
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP
  CURLcode نتيجو = CURLE_OK;
  بنية UserDefined *s = &معلومات->سيٽ;

  بدل(option) {
  حالة CURLOPT_FOLLOWLOCATION:
    إذا((بنانشان ڊگهو)arg > 3)
      نتيجو = CURLE_BAD_FUNCTION_دليل;
    وإلا
      s->http_follow_گهڻائي = (بنانشان حرف)arg;
    اكسر;
  حالة CURLOPT_MAXREDIRS:
    نتيجو = قدر_range(&arg, -1, -1, 0x7fff);
    إذا(!نتيجو)
      s->maxredirs = (ننڍو)arg;
    اكسر;
  حالة CURLOPT_POSTREDIR:
    إذا(arg < CURL_REDIR_GET_ALL)
      نتيجو = CURLE_BAD_FUNCTION_دليل;
    وإلا {
      s->post301 = !!(arg & CURL_REDIR_POST_301);
      s->post302 = !!(arg & CURL_REDIR_POST_302);
      s->post303 = !!(arg & CURL_REDIR_POST_303);
    }
    اكسر;
  حالة CURLOPT_HEADEROPT:
    s->sep_سرا = !!(arg & CURLHEADER_SEPARATE);
    اكسر;
  حالة CURLOPT_HTTPAUTH:
    أرجع httpauth(معلومات, FALSE, (بنانشان ڊگهو)arg);
  حالة CURLOPT_HTTP_نسخو:
    أرجع setopt_HTTP_نسخو(معلومات, arg);
  حالة CURLOPT_EXPECT_100_TIMEOUT_MS:
    نتيجو = قدر_range(&arg, 0, 0, 0xffff);
    إذا(!نتيجو)
      s->expect_100_timeout = (بنانشان ننڍو)arg;
    اكسر;
  حالة CURLOPT_وهڪرو_وزن:
#إذا defined(USE_HTTP2) || defined(USE_HTTP3)
    إذا((arg >= 1) && (arg <= 256))
      s->وزن = (صحيح)arg;
    اكسر;
#وإلا
    نتيجو = CURLE_NOT_BUILT_اندريون;
    اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTPSIG
  حالة CURLOPT_HTTPSIG_ALGORITHM:
    إذا(arg != CURLHTTPSIG_NONE &&
       arg != CURLHTTPSIG_ED25519 &&
       arg != CURLHTTPSIG_HMAC_SHA256)
      أرجع CURLE_BAD_FUNCTION_دليل;
    s->httpsig_algorithm = (uint8_t)arg;
    إذا(arg)
      s->httpauth = (uint32_t)CURLAUTH_HTTPSIG;
    وإلا
      s->httpauth &= ~(uint32_t)CURLAUTH_HTTPSIG;
    اكسر;
#جيڪڏهن_پورو
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع نتيجو;
#وإلا
  (فراغ)معلومات;
  (فراغ)option;
  (فراغ)arg;
  أرجع CURLE_UNKNOWN_OPTION;
#جيڪڏهن_پورو
}

ساكن CURLcode setopt_long_proto(بنية Curl_easy *معلومات, CURLoption option,
                                  ڊگهو arg)
{
  CURLcode نتيجو = CURLE_OK;
  بنية UserDefined *s = &معلومات->سيٽ;

  بدل(option) {
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_TFTP
  حالة CURLOPT_TFTP_BLKSIZE:
    نتيجو = قدر_range(&arg, 0, TFTP_BLKSIZE_MIN, TFTP_BLKSIZE_MAX);
    إذا(!نتيجو)
      s->tftp_blksize = (بنانشان ننڍو)arg;
    اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_NETRC
  حالة CURLOPT_NETRC:
    إذا((arg < CURL_NETRC_IGNORED) || (arg >= CURL_NETRC_آخري))
      نتيجو = CURLE_BAD_FUNCTION_دليل;
    وإلا
      s->use_netrc = (بنانشان حرف)arg;
    اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FTP
  حالة CURLOPT_FTP_FILEMETHOD:
    إذا((arg < CURLFTPMETHOD_DEFAULT) || (arg >= CURLFTPMETHOD_آخري))
      نتيجو = CURLE_BAD_FUNCTION_دليل;
    وإلا
      s->ftp_filemethod = (بنانشان حرف)arg;
    اكسر;
  حالة CURLOPT_FTP_SSL_CCC:
    إذا((arg < CURLFTPSSL_CCC_NONE) || (arg >= CURLFTPSSL_CCC_آخري))
      نتيجو = CURLE_BAD_FUNCTION_دليل;
    وإلا
      s->ftp_ccc = (بنانشان حرف)arg;
    اكسر;
  حالة CURLOPT_FTPSSLAUTH:
    إذا((arg < CURLFTPAUTH_DEFAULT) || (arg >= CURLFTPAUTH_آخري))
      نتيجو = CURLE_BAD_FUNCTION_دليل;
    وإلا
      s->ftpsslauth = (بنانشان حرف)arg;
    اكسر;
  حالة CURLOPT_ACCEPTTIMEOUT_MS:
    أرجع setopt_سيٽ_timeout_ms(&s->accepttimeout, arg);
#جيڪڏهن_پورو
#إذا !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  حالة CURLOPT_FTP_CREATE_MISSING_DIRS:
    إذا((arg < CURLFTP_CREATE_DIR_NONE) || (arg > CURLFTP_CREATE_DIR_RETRY))
      نتيجو = CURLE_BAD_FUNCTION_دليل;
    وإلا
      s->ftp_create_missing_dirs = (بنانشان حرف)arg;
    اكسر;
  حالة CURLOPT_NEW_FILE_PERMS:
    إذا((arg < 0) || (arg > 0777))
      نتيجو = CURLE_BAD_FUNCTION_دليل;
    وإلا
      s->new_file_perms = (بنانشان صحيح)arg;
    اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_RTSP
  حالة CURLOPT_RTSP_REQUEST:
    أرجع setopt_RTSP_REQUEST(معلومات, arg);
  حالة CURLOPT_RTSP_CLIENT_CSEQ:
    نتيجو = قدر_range(&arg, 0, 0, وڌ_کان_وڌ_عدد);
    إذا(!نتيجو)
      معلومات->state.rtsp_ايندڙ_client_CSeq = (uint32_t)arg;
    اكسر;
  حالة CURLOPT_RTSP_SERVER_CSEQ:
    نتيجو = قدر_range(&arg, 0, 0, وڌ_کان_وڌ_عدد);
    إذا(!نتيجو)
      معلومات->state.rtsp_ايندڙ_server_CSeq = (uint32_t)arg;
    اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_SSH
  حالة CURLOPT_SSH_AUTH_TYPES:
    s->ssh_auth_types = (uint32_t)arg;
    اكسر;
  حالة CURLOPT_NEW_DIRECTORY_PERMS:
    إذا((arg < 0) || (arg > 0777))
      نتيجو = CURLE_BAD_FUNCTION_دليل;
    وإلا
      s->new_directory_perms = (بنانشان صحيح)arg;
    اكسر;
#جيڪڏهن_پورو
  حالة CURLOPT_PROTOCOLS:
    s->allowed_protocols = (curl_prot_t)arg;
    اكسر;
  حالة CURLOPT_REDIR_PROTOCOLS:
    s->redir_protocols = (curl_prot_t)arg;
    اكسر;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_WEBSOCKETS
  حالة CURLOPT_WS_OPTIONS:
    s->ws_raw_گهڻائي = !!(arg & CURLWS_RAW_گهڻائي);
    s->ws_no_auto_pong = !!(arg & CURLWS_NOAUTOPONG);
    اكسر;
#جيڪڏهن_پورو
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع نتيجو;
}

ساكن CURLcode setopt_long_misc(بنية Curl_easy *معلومات, CURLoption option,
                                 ڊگهو arg)
{
  بنية UserDefined *s = &معلومات->سيٽ;

  بدل(option) {
  حالة CURLOPT_TIMECONDITION:
    إذا((arg < CURL_TIMECOND_NONE) || (arg >= CURL_TIMECOND_آخري))
      أرجع CURLE_BAD_FUNCTION_دليل;
    s->timecondition = (بنانشان حرف)arg;
    اكسر;
  حالة CURLOPT_TIMEVALUE:
    s->timevalue = (time_t)arg;
    اكسر;
  حالة CURLOPT_POSTFIELDSIZE:
    إذا(arg < -1)
      أرجع CURLE_BAD_FUNCTION_دليل;
    إذا(s->postfieldsize < arg && s->str_copypostfields) {
      curlx_safefree(s->str_copypostfields);
      s->postfields = عدم;
    }
    s->postfieldsize = arg;
    اكسر;
  حالة CURLOPT_INFILESIZE:
    إذا(arg < -1)
      أرجع CURLE_BAD_FUNCTION_دليل;
    s->filesize = arg;
    اكسر;
  حالة CURLOPT_RESUME_FROM:
    إذا(arg < -1)
      أرجع CURLE_BAD_FUNCTION_دليل;
    s->سيٽ_resume_from = arg;
    اكسر;
  حالة CURLOPT_UPLOAD_FLAGS:
    s->upload_flags = (بنانشان حرف)arg;
    اكسر;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_MIME
  حالة CURLOPT_MIME_OPTIONS:
    s->mime_formescape = !!(arg & CURLMIMEOPT_FORMESCAPE);
    اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HSTS
  حالة CURLOPT_HSTS_CTRL:
    إذا(arg & CURLHSTS_ENABLE) {
      إذا(!معلومات->hsts) {
        معلومات->hsts = Curl_hsts_init();
        إذا(!معلومات->hsts)
          أرجع CURLE_ٻاهريون_جو_يادگيري;
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
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_ALTSVC
  حالة CURLOPT_ALTSVC_CTRL:
    أرجع Curl_altsvc_ctrl(معلومات, arg);
#جيڪڏهن_پورو
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  حالة CURLOPT_GSSAPI_DELEGATION:
    s->gssapi_delegation = (بنانشان حرف)arg &
      (CURLGSSAPI_DELEGATION_POLICY_FLAG | CURLGSSAPI_DELEGATION_FLAG);
    اكسر;
#جيڪڏهن_پورو
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode setopt_long(بنية Curl_easy *معلومات, CURLoption option,
                            ڊگهو arg)
{
  تعريفنوع CURLcode (*setoptfunc)(بنية Curl_easy *معلومات,
                                 CURLoption option, ڊگهو arg);
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
    CURLcode نتيجو = setopt_call[i](معلومات, option, arg);
    إذا(نتيجو != CURLE_UNKNOWN_OPTION)
      أرجع نتيجو;
  }
  أرجع CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode setopt_slist(بنية Curl_easy *معلومات, CURLoption option,
                             بنية curl_slist *slist)
{
  CURLcode نتيجو = CURLE_OK;
  بنية UserDefined *s = &معلومات->سيٽ;
  بدل(option) {
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
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
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP
  حالة CURLOPT_HTTP200ALIASES:
    /*
     * Set a list of aliases for HTTP 200 in response header
     */
    s->http200aliases = slist;
    اكسر;
#جيڪڏهن_پورو
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
#جيڪڏهن_پورو
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
    s->سرا = slist;
    اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_TELNET
  حالة CURLOPT_TELNETOPTIONS:
    /*
     * Set a linked list of telnet options
     */
    s->telnet_options = slist;
    اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_SMTP
  حالة CURLOPT_MAIL_RCPT:
    /* Set the list of mail recipients */
    s->mail_rcpt = slist;
    اكسر;
#جيڪڏهن_پورو
  حالة CURLOPT_CONNECT_منزل:
    s->connect_منزل = slist;
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع نتيجو;
}

#إذا !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_MIME
ساكن CURLcode setopt_mimepost(بنية Curl_easy *معلومات, curl_mime *mimep)
{
  /*
   * Set to make us do MIME POST
   */
  CURLcode نتيجو;
  بنية UserDefined *s = &معلومات->سيٽ;
  إذا(!s->mimepostp) {
    s->mimepostp = curlx_malloc(حجم(*s->mimepostp));
    إذا(!s->mimepostp)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    Curl_mime_initpart(s->mimepostp);
  }

  نتيجو = Curl_mime_سيٽ_subparts(s->mimepostp, mimep, FALSE);
  إذا(!نتيجو) {
    s->طريقو = HTTPREQ_POST_MIME;
    s->opt_no_body = FALSE; /* this is implied */
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FORM_API
    Curl_mime_cleanpart(معلومات->state.formp);
    curlx_safefree(معلومات->state.formp);
    معلومات->state.mimepost = عدم;
#جيڪڏهن_پورو
  }
  أرجع نتيجو;
}
#جيڪڏهن_پورو /* !CURL_DISABLE_MIME */
#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */

ساكن CURLcode setopt_share(بنية Curl_easy *معلومات, بنية Curl_share *سيٽ)
{
  CURLcode نتيجو;

  إذا(معلومات->conn) {
    /* As this handle already has a connection attached, changing share now
       would be complicated and error-prone */
    infof(معلومات, "Cannot change share object while in use");
    نتيجو = CURLE_BAD_FUNCTION_دليل;
  }
  وإلا {
    /* disconnect from old share, if any and possible */
    نتيجو = Curl_share_easy_unlink(معلومات);
    إذا(!نتيجو && GOOD_SHARE_HANDLE(سيٽ))
      /* use new share if it set */
      نتيجو = Curl_share_easy_link(معلومات, سيٽ);
  }
  أرجع نتيجو;
}

/* assorted pointer type arguments */
ساكن CURLcode setopt_اشارا(بنية Curl_easy *معلومات, CURLoption option,
                                va_list param)
{
  CURLcode نتيجو = CURLE_OK;
  بنية UserDefined *s = &معلومات->سيٽ;
  بدل(option) {
  حالة CURLOPT_CURLU:
    /*
     * pass CURLU to set URL
     */
    Curl_bufref_free(&معلومات->state.url);
    CURL_EASY_STR_CLEAR(معلومات, STRING_سيٽ_URL);
    s->uh = va_arg(param, CURLU *);
    اكسر;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FORM_API
  حالة CURLOPT_HTTPPOST:
    /*
     * Set to make us do HTTP POST. Legacy API-style.
     */
    s->httppost = va_arg(param, بنية curl_httppost *);
    s->طريقو = HTTPREQ_POST_FORM;
    s->opt_no_body = FALSE; /* this is implied */
    Curl_mime_cleanpart(معلومات->state.formp);
    curlx_safefree(معلومات->state.formp);
    معلومات->state.mimepost = عدم;
    اكسر;
#جيڪڏهن_پورو /* !CURL_DISABLE_FORM_API */
#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP */
#إذا !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_MIME
  حالة CURLOPT_MIMEPOST:
    نتيجو = setopt_mimepost(معلومات, va_arg(param, curl_mime *));
    اكسر;
#جيڪڏهن_پورو /* !CURL_DISABLE_MIME */
#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */
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
  أرجع نتيجو;
}

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_COOKIES
ساكن CURLcode cookielist(بنية Curl_easy *معلومات, ثابت حرف *ptr)
{
  CURLcode نتيجو = CURLE_OK;
  إذا(!ptr)
    أرجع CURLE_OK;

  إذا(curl_strequal(ptr, "ALL")) {
    /* clear all cookies */
    Curl_share_تالو(معلومات, CURL_تالو_معلومات_COOKIE, CURL_تالو_ACCESS_SINGLE);
    Curl_cookie_clearall(معلومات->cookies);
    Curl_share_unlock(معلومات, CURL_تالو_معلومات_COOKIE);
  }
  وإلا إذا(curl_strequal(ptr, "SESS")) {
    /* clear session cookies */
    Curl_share_تالو(معلومات, CURL_تالو_معلومات_COOKIE, CURL_تالو_ACCESS_SINGLE);
    Curl_cookie_clearsess(معلومات->cookies);
    Curl_share_unlock(معلومات, CURL_تالو_معلومات_COOKIE);
  }
  وإلا إذا(curl_strequal(ptr, "FLUSH")) {
    /* flush cookies to file, takes care of the locking */
    Curl_flush_cookies(معلومات, FALSE);
  }
  وإلا إذا(curl_strequal(ptr, "RELOAD")) {
    /* reload cookies from file */
    أرجع Curl_cookie_loadfiles(معلومات, COOKIE_NOPSL |
                                 (معلومات->سيٽ.cookiesession ?
                                  COOKIE_NOSESSION : 0));
  }
  وإلا {
    إذا(!معلومات->cookies) {
      /* if cookie engine was not running, activate it */
      معلومات->cookies = Curl_cookie_init();
      إذا(!معلومات->cookies)
        أرجع CURLE_ٻاهريون_جو_يادگيري;
      معلومات->state.cookie_engine = TRUE;
    }

    /* general protection against mistakes and abuse */
    إذا(لڙي_ڊيگهه(ptr) > CURL_MAX_داخلا_ڊيگهائي)
      أرجع CURLE_BAD_FUNCTION_دليل;

    /* Adding these cookies without the PSL check, because the PSL is not
       initialized until *perform() time, and this might be called before
       that */
    Curl_share_تالو(معلومات, CURL_تالو_معلومات_COOKIE, CURL_تالو_ACCESS_SINGLE);
    إذا(checkprefix("Set-Cookie:", ptr))
      /* HTTP Header format line */
      نتيجو = Curl_cookie_جوڙيو(معلومات, معلومات->cookies, ptr + 11,
                               عدم, عدم,
                               COOKIE_HTTPHEADER | COOKIE_SECURE |
                               COOKIE_NOPSL);
    وإلا
      /* Netscape format line */
      نتيجو = Curl_cookie_جوڙيو(معلومات, معلومات->cookies, ptr, عدم,
                               عدم, COOKIE_SECURE | COOKIE_NOPSL);
    Curl_share_unlock(معلومات, CURL_تالو_معلومات_COOKIE);
  }
  أرجع نتيجو;
}

ساكن CURLcode cookiefile(بنية Curl_easy *معلومات, ثابت حرف *ptr)
{
  /*
   * Set cookie file to read and parse. Can be used multiple times.
   */
  إذا(ptr) {
    بنية curl_slist *cl;
    /* general protection against mistakes and abuse */
    إذا(لڙي_ڊيگهه(ptr) > CURL_MAX_داخلا_ڊيگهائي)
      أرجع CURLE_BAD_FUNCTION_دليل;
    /* append the cookie filename to the list of filenames, and deal with
       them later */
    cl = curl_slist_append(معلومات->state.cookielist, ptr);
    إذا(!cl) {
      curl_slist_free_all(معلومات->state.cookielist);
      معلومات->state.cookielist = عدم;
      أرجع CURLE_ٻاهريون_جو_يادگيري;
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
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
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
  CURLcode نتيجو = CURLE_OK;
  بنية UserDefined *s = &معلومات->سيٽ;
  بدل(option) {
  حالة CURLOPT_PROXYUSERPWD: {
    /*
     * user:password needed to use the proxy
     */
    حرف *u = عدم;
    حرف *p = عدم;
    نتيجو = setstropt_userpwd(ptr, &u, &p);

    /* URL decode the components */
    إذا(!نتيجو) {
      حرف *str = عدم;
      CURL_EASY_STR_CLEAR(معلومات, STRING_PROXYUSERNAME);
      CURL_EASY_STR_CLEAR(معلومات, STRING_PROXYPASSWORD);
      إذا(u) {
        نتيجو = Curl_urldecode(u, 0, &str, عدم, REJECT_ZERO);
        إذا(!نتيجو)
          نتيجو = Curl_u8_strset_setn(&s->لڙيون, STRING_PROXYUSERNAME, str);
      }
      إذا(!نتيجو && p) {
        str = عدم;
        نتيجو = Curl_urldecode(p, 0, &str, عدم, REJECT_ZERO);
        إذا(!نتيجو)
          نتيجو = Curl_u8_strset_setn(&s->لڙيون, STRING_PROXYPASSWORD, str);
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
    أرجع Curl_setstropt(معلومات, STRING_ڪنجي_PROXY, ptr);
  حالة CURLOPT_PROXY_KEYPASSWD:
    /*
     * String that holds the SSL private key password for proxy.
     */
    أرجع Curl_setstropt(معلومات, STRING_ڪنجي_PASSWD_PROXY, ptr);
  حالة CURLOPT_PROXY_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use for proxy
     */
    أرجع Curl_setstropt(معلومات, STRING_ڪنجي_TYPE_PROXY, ptr);
  حالة CURLOPT_PROXY_SSL_CIPHER_LIST:
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_CIPHER_LIST)) {
      /* set a list of cipher we want to use in the SSL connection for proxy */
      أرجع Curl_setstropt(معلومات, STRING_SSL_CIPHER_LIST_PROXY, ptr);
    }
    وإلا
      أرجع CURLE_NOT_BUILT_اندريون;
  حالة CURLOPT_PROXY_TLS13_CIPHERS:
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites for proxy */
      أرجع Curl_setstropt(معلومات, STRING_SSL_CIPHER13_LIST_PROXY, ptr);
    وإلا
      أرجع CURLE_NOT_BUILT_اندريون;
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
#جيڪڏهن_وصف_ٿيل USE_SSL
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_PINNEDPUBKEY))
      أرجع Curl_setstropt(معلومات, STRING_SSL_PINNEDPUBLICKEY_PROXY, ptr);
#جيڪڏهن_پورو
    أرجع CURLE_NOT_BUILT_اندريون;

  حالة CURLOPT_HAPROXY_CLIENT_IP:
    /*
     * Set the client IP to send through HAProxy PROXY protocol
     */
    نتيجو = Curl_setstropt(معلومات, STRING_HAPROXY_CLIENT_IP, ptr);

    /* enable the HAProxy protocol if an IP is provided */
    s->haproxyprotocol = !!CURL_EASY_STR(معلومات, STRING_HAPROXY_CLIENT_IP);
    اكسر;
  حالة CURLOPT_PROXY_CAINFO:
    /*
     * Set CA info SSL connection for proxy. Specify filename of the
     * CA certificate
     */
    نتيجو = Curl_setstropt(معلومات, STRING_SSL_CAFILE_PROXY, ptr);
    إذا(!نتيجو)
      s->proxy_ssl.custom_cafile = TRUE;
    أرجع نتيجو;
  حالة CURLOPT_PROXY_CRLFILE:
    /*
     * Set CRL file info for SSL connection for proxy. Specify filename of the
     * CRL to check certificates revocation
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_CRLFILE))
      أرجع Curl_setstropt(معلومات, STRING_SSL_CRLFILE_PROXY, ptr);
    أرجع CURLE_NOT_BUILT_اندريون;
  حالة CURLOPT_PROXY_ISSUERCERT:
    /*
     * Set Issuer certificate file to check certificates issuer
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_ISSUERCERT))
      أرجع Curl_setstropt(معلومات, STRING_SSL_ISSUERCERT_PROXY, ptr);
    أرجع CURLE_NOT_BUILT_اندريون;
  حالة CURLOPT_PROXY_CAPATH:
    /*
     * Set CA path info for SSL connection proxy. Specify directory name of the
     * CA certificates which have been prepared using openssl c_rehash utility.
     */
#جيڪڏهن_وصف_ٿيل USE_SSL
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_CA_رستو)) {
      /* This does not work on Windows. */
      نتيجو = Curl_setstropt(معلومات, STRING_SSL_CAPATH_PROXY, ptr);
      إذا(!نتيجو)
        s->proxy_ssl.custom_capath = TRUE;
      أرجع نتيجو;
    }
#جيڪڏهن_پورو
    أرجع CURLE_NOT_BUILT_اندريون;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع نتيجو;
}
#جيڪڏهن_پورو

#إذا !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
/*
 * A string with POST data. Makes curl HTTP POST. Even if it is NULL. If
 * needed, CURLOPT_POSTFIELDSIZE must have been set prior to
 * CURLOPT_COPYPOSTFIELDS and not altered later.
 */
ساكن CURLcode setopt_copypostfields(ثابت حرف *ptr, بنية UserDefined *s)
{
  CURLcode نتيجو = CURLE_OK;
  إذا(s->postfieldsize < -1)
    أرجع CURLE_BAD_FUNCTION_دليل;
  إذا(!ptr || s->postfieldsize == -1) {
    إذا(ptr && (لڙي_ڊيگهه(ptr) > CURL_MAX_داخلا_ڊيگهائي))
      أرجع CURLE_BAD_FUNCTION_دليل;
    curlx_safefree(s->str_copypostfields);
    إذا(ptr) {
      s->str_copypostfields = curlx_strdup(ptr);
      إذا(!s->str_copypostfields)
        أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
  }
  وإلا {
    size_t pflen = curlx_sotouz_range(s->postfieldsize, 0, SIZE_MAX);
    إذا(pflen == SIZE_MAX)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    وإلا {
      /* Allocate even when size == 0. This satisfies the need of possible
         later address compare to detect the COPYPOSTFIELDS mode, and to mark
         that postfields is used rather than read function or form data. */
      حرف *p = curlx_memdup0(ptr, pflen);
      إذا(!p)
        أرجع CURLE_ٻاهريون_جو_يادگيري;
      وإلا {
        curlx_free(s->str_copypostfields);
        s->str_copypostfields = p;
      }
    }
  }

  s->postfields = s->str_copypostfields;
  s->طريقو = HTTPREQ_POST;
  أرجع نتيجو;
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_ECH
ساكن CURLcode setopt_ech(بنية Curl_easy *معلومات, ثابت حرف *ptr)
{
  بنية UserDefined *s = &معلومات->سيٽ;
  CURLcode نتيجو = CURLE_OK;

  إذا(!ptr || !strcmp(ptr, "false"))
    s->ssl.ech = CURLECH_DISABLE;
  وإلا {
    size_t plen = لڙي_ڊيگهه(ptr);
    إذا(plen > CURL_MAX_داخلا_ڊيگهائي)
      نتيجو = CURLE_BAD_FUNCTION_دليل;
    وإلا {
      إذا(!strcmp(ptr, "grease"))
        s->ssl.ech = CURLECH_GREASE;
      وإلا إذا(!strcmp(ptr, "true"))
        s->ssl.ech = CURLECH_ENABLE;
      وإلا إذا(!strcmp(ptr, "hard"))
        s->ssl.ech = CURLECH_HARD;
      وإلا إذا(plen > 4 && !لڙي_ڀيٽ(ptr, "ecl:", 4)) {
        إذا(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        نتيجو = Curl_setstropt(معلومات, STRING_ECH_CONFIG, ptr + 4);
      }
      وإلا إذا(plen > 3 && !لڙي_ڀيٽ(ptr, "pn:", 3)) {
        إذا(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        نتيجو = Curl_setstropt(معلومات, STRING_ECH_عوامي, ptr + 3);
      }
      وإلا
        نتيجو = CURLE_BAD_FUNCTION_دليل;
    }
  }
  أرجع نتيجو;
}
#وإلا
#وصف_ڪريو setopt_ech(x, y) CURLE_NOT_BUILT_اندريون
#جيڪڏهن_پورو

#إذا defined(USE_SSL) || defined(USE_SSH)
/* One of the options is used for both TLS and SSH */
ساكن CURLcode setopt_cptr_ssl(بنية Curl_easy *معلومات, CURLoption option,
                                حرف *ptr)
{
  CURLcode نتيجو = CURLE_OK;

  بدل(option) {
  حالة CURLOPT_KEYPASSWD:
    /*
     * String that holds the SSL or SSH private key password.
     */
    نتيجو = Curl_setstropt(معلومات, STRING_ڪنجي_PASSWD, ptr);
    اكسر;
#جيڪڏهن_وصف_ٿيل USE_SSL
  حالة CURLOPT_CAINFO:
    /*
     * Set CA info for SSL connection. Specify filename of the CA certificate
     */
    نتيجو = Curl_setstropt(معلومات, STRING_SSL_CAFILE, ptr);
    إذا(!نتيجو)
      معلومات->سيٽ.ssl.custom_cafile = TRUE;
    أرجع نتيجو;
  حالة CURLOPT_CAPATH:
    /*
     * Set CA path info for SSL connection. Specify directory name of the CA
     * certificates which have been prepared using openssl c_rehash utility.
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_CA_رستو)) {
      /* This does not work on Windows. */
      نتيجو = Curl_setstropt(معلومات, STRING_SSL_CAPATH, ptr);
      إذا(!نتيجو)
        معلومات->سيٽ.ssl.custom_capath = TRUE;
      أرجع نتيجو;
    }
    أرجع CURLE_NOT_BUILT_اندريون;
  حالة CURLOPT_CRLFILE:
    /*
     * Set CRL file info for SSL connection. Specify filename of the CRL
     * to check certificates revocation
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_CRLFILE))
      أرجع Curl_setstropt(معلومات, STRING_SSL_CRLFILE, ptr);
    أرجع CURLE_NOT_BUILT_اندريون;
  حالة CURLOPT_SSL_CIPHER_LIST:
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_CIPHER_LIST))
      /* set a list of cipher we want to use in the SSL connection */
      أرجع Curl_setstropt(معلومات, STRING_SSL_CIPHER_LIST, ptr);
    وإلا
      أرجع CURLE_NOT_BUILT_اندريون;
  حالة CURLOPT_TLS13_CIPHERS:
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites */
      أرجع Curl_setstropt(معلومات, STRING_SSL_CIPHER13_LIST, ptr);
    وإلا
      أرجع CURLE_NOT_BUILT_اندريون;
  حالة CURLOPT_RANDOM_FILE:
    اكسر;
  حالة CURLOPT_EGDSOCKET:
    اكسر;
  حالة CURLOPT_SSL_CTX_معلومات:
    /*
     * Set an SSL_CTX callback parameter pointer
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_SSL_CTX)) {
      معلومات->سيٽ.ssl_fsslctxp = ptr;
      اكسر;
    }
    وإلا
      أرجع CURLE_NOT_BUILT_اندريون;
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
    أرجع Curl_setstropt(معلومات, STRING_ڪنجي, ptr);
  حالة CURLOPT_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use
     */
    أرجع Curl_setstropt(معلومات, STRING_ڪنجي_TYPE, ptr);
  حالة CURLOPT_SSLENGINE:
    /*
     * String that holds the SSL crypto engine.
     */
    إذا(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      أرجع CURLE_NOT_BUILT_اندريون;
    نتيجو = Curl_setstropt(معلومات, STRING_SSL_ENGINE, ptr);
    اكسر;
  حالة CURLOPT_ISSUERCERT:
    /*
     * Set Issuer certificate file
     * to check certificates issuer
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_ISSUERCERT))
      أرجع Curl_setstropt(معلومات, STRING_SSL_ISSUERCERT, ptr);
    أرجع CURLE_NOT_BUILT_اندريون;
  حالة CURLOPT_SSL_EC_CURVES:
    /*
     * Set accepted curves in SSL connection setup.
     * Specify colon-delimited list of curve algorithm names.
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_SSL_EC_CURVES))
      أرجع Curl_setstropt(معلومات, STRING_SSL_EC_CURVES, ptr);
    أرجع CURLE_NOT_BUILT_اندريون;
  حالة CURLOPT_SSL_SIGNATURE_ALGORITHMS:
    /*
     * Set accepted signature algorithms.
     * Specify colon-delimited list of signature scheme names.
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_SIGNATURE_ALGORITHMS))
      أرجع Curl_setstropt(معلومات, STRING_SSL_SIGNATURE_ALGORITHMS, ptr);
    أرجع CURLE_NOT_BUILT_اندريون;
  حالة CURLOPT_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_PINNEDPUBKEY))
      أرجع Curl_setstropt(معلومات, STRING_SSL_PINNEDPUBLICKEY, ptr);
    أرجع CURLE_NOT_BUILT_اندريون;
  حالة CURLOPT_ECH:
    أرجع setopt_ech(معلومات, ptr);
#جيڪڏهن_پورو
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع نتيجو;
}
#جيڪڏهن_پورو

#إذا !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
ساكن CURLcode setopt_cptr_http_mqtt(بنية Curl_easy *معلومات,
                                      CURLoption option, حرف *ptr)
{
  CURLcode نتيجو = CURLE_OK;
  بنية UserDefined *s = &معلومات->سيٽ;

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
    s->طريقو = HTTPREQ_POST;
    اكسر;

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP
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
        نتيجو = CURL_EASY_STR_SETN(معلومات, STRING_ENCODING, ptr);
      وإلا
        نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      أرجع نتيجو;
    }
    أرجع Curl_setstropt(معلومات, STRING_ENCODING, ptr);

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_AWS
  حالة CURLOPT_AWS_SIGV4:
    /*
     * String that is merged to some authentication
     * parameters are used by the algorithm.
     */
    نتيجو = Curl_setstropt(معلومات, STRING_AWS_SIGV4, ptr);
    /*
     * Basic has been set by default; it needs to be unset here.
     */
    إذا(CURL_EASY_STR(معلومات, STRING_AWS_SIGV4))
      s->httpauth = CURLAUTH_AWS_SIGV4;
    وإلا
      s->httpauth &= ~(uint32_t)CURLAUTH_AWS_SIGV4;
    اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTPSIG
  حالة CURLOPT_HTTPSIG_ڪنجي:
    نتيجو = Curl_setstropt(معلومات, STRING_HTTPSIG_ڪنجي, ptr);
    اكسر;
  حالة CURLOPT_HTTPSIG_KEYID:
    نتيجو = Curl_setstropt(معلومات, STRING_HTTPSIG_KEYID, ptr);
    اكسر;
  حالة CURLOPT_HTTPSIG_سرا:
    نتيجو = Curl_setstropt(معلومات, STRING_HTTPSIG_سرا, ptr);
    اكسر;
#جيڪڏهن_پورو
  حالة CURLOPT_REFERER: {
    /*
     * String to set in the HTTP Referer: field.
     */
    بنية bufref *oldref = &معلومات->state.referer;
    /* free the old after the storing the new in case the input is actually
       pointing back to this */
    نتيجو = Curl_setstropt(معلومات, STRING_سيٽ_REFERER, ptr);
    Curl_bufref_free(oldref);
    اكسر;
  }

  حالة CURLOPT_USERAGENT:
    /*
     * String to use in the HTTP User-Agent field
     */
    أرجع Curl_setstropt(معلومات, STRING_USERAGENT, ptr);

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_COOKIES
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
    نتيجو = Curl_setstropt(معلومات, STRING_COOKIEJAR, ptr);
    إذا(!نتيجو) {
      /*
       * Activate the cookie parser. This may or may not already
       * have been made.
       */
      إذا(!معلومات->cookies)
        معلومات->cookies = Curl_cookie_init();
      إذا(!معلومات->cookies)
        نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      وإلا
        معلومات->state.cookie_engine = TRUE;
    }
    اكسر;

  حالة CURLOPT_COOKIELIST:
    أرجع cookielist(معلومات, ptr);
#جيڪڏهن_پورو /* !CURL_DISABLE_COOKIES */

#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP */
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع نتيجو;
}
#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP || !CURL_DISABLE_MQTT */

#جيڪڏهن_وصف_ٿيل USE_SSH
ساكن CURLcode setopt_cptr_ssh(بنية Curl_easy *معلومات, CURLoption option,
                                حرف *ptr)
{
  بنية UserDefined *s = &معلومات->سيٽ;
  بدل(option) {
  حالة CURLOPT_SSH_عوامي_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa.pub file
     */
    أرجع Curl_setstropt(معلومات, STRING_SSH_عوامي_ڪنجي, ptr);
  حالة CURLOPT_SSH_ذاتي_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa file
     */
    أرجع Curl_setstropt(معلومات, STRING_SSH_ذاتي_ڪنجي, ptr);
  حالة CURLOPT_SSH_KEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_keyfunc_userp = ptr;
    اكسر;
  حالة CURLOPT_SSH_HOST_عوامي_ڪنجي_MD5:
    /*
     * Option to allow for the MD5 of the host public key to be checked
     * for validation purposes.
     */
    أرجع Curl_setstropt(معلومات, STRING_SSH_HOST_عوامي_ڪنجي_MD5, ptr);
  حالة CURLOPT_SSH_HOST_عوامي_ڪنجي_SHA256:
    /*
     * Option to allow for the SHA256 of the host public key to be checked
     * for validation purposes.
     */
    أرجع Curl_setstropt(معلومات, STRING_SSH_HOST_عوامي_ڪنجي_SHA256, ptr);
  حالة CURLOPT_SSH_KNOWNHOSTS:
    /*
     * Store the filename to read known hosts from.
     */
    أرجع Curl_setstropt(معلومات, STRING_SSH_KNOWNHOSTS, ptr);
#جيڪڏهن_وصف_ٿيل USE_LIBSSH2
  حالة CURLOPT_SSH_HOSTKEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_hostkeyfunc_userp = ptr;
    اكسر;
#جيڪڏهن_پورو /* USE_LIBSSH2 */
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع CURLE_OK;
}
#جيڪڏهن_پورو /* USE_SSH */

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FTP
ساكن CURLcode setopt_cptr_ftp(بنية Curl_easy *معلومات, CURLoption option,
                                حرف *ptr)
{
  CURLcode نتيجو = CURLE_OK;
  بنية UserDefined *s = &معلومات->سيٽ;
  بدل(option) {
  حالة CURLOPT_FTPPORT:
    /*
     * Use FTP PORT, this also specifies which IP address to use
     */
    نتيجو = Curl_setstropt(معلومات, STRING_FTPPORT, ptr);
    s->ftp_use_پورٽ = !!CURL_EASY_STR(معلومات, STRING_FTPPORT);
    اكسر;

  حالة CURLOPT_FTP_ACCOUNT:
    أرجع Curl_setstropt(معلومات, STRING_FTP_ACCOUNT, ptr);

  حالة CURLOPT_FTP_ALTERNATIVE_منزل_USER:
    أرجع Curl_setstropt(معلومات, STRING_FTP_ALTERNATIVE_منزل_USER, ptr);

  حالة CURLOPT_KRBLEVEL:
    أرجع CURLE_NOT_BUILT_اندريون; /* removed in 8.17.0 */
  حالة CURLOPT_CHUNK_معلومات:
    s->wildcardptr = ptr;
    اكسر;
  حالة CURLOPT_FNMATCH_معلومات:
    s->fnmatch_معلومات = ptr;
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع نتيجو;
}
#جيڪڏهن_پورو /* !CURL_DISABLE_FTP */

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

#جيڪڏهن_وصف_ٿيل USE_RESOLV_ARES
  حالة CURLOPT_DNS_SERVERS:
    أرجع Curl_setstropt(معلومات, STRING_DNS_SERVERS, ptr);

  حالة CURLOPT_DNS_INTERFACE:
    أرجع Curl_setstropt(معلومات, STRING_DNS_INTERFACE, ptr);

  حالة CURLOPT_DNS_LOCAL_IP4:
    أرجع Curl_setstropt(معلومات, STRING_DNS_LOCAL_IP4, ptr);

  حالة CURLOPT_DNS_LOCAL_IP6:
    أرجع Curl_setstropt(معلومات, STRING_DNS_LOCAL_IP6, ptr);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_UNIX_SOCKETS
  حالة CURLOPT_UNIX_SOCKET_رستو:
    معلومات->سيٽ.abstract_unix_socket = FALSE;
    أرجع Curl_setstropt(معلومات, STRING_UNIX_SOCKET_رستو, ptr);

  حالة CURLOPT_ABSTRACT_UNIX_SOCKET:
    معلومات->سيٽ.abstract_unix_socket = TRUE;
    أرجع Curl_setstropt(معلومات, STRING_UNIX_SOCKET_رستو, ptr);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DOH
  حالة CURLOPT_DOH_URL:
    {
      CURLcode نتيجو = Curl_setstropt(معلومات, STRING_DOH, ptr);
      معلومات->سيٽ.doh = !!CURL_EASY_STR(معلومات, STRING_DOH);
      أرجع نتيجو;
    }
#جيڪڏهن_پورو
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
}

ساكن CURLcode setopt_cptr_misc(بنية Curl_easy *معلومات, CURLoption option,
                                 حرف *ptr)
{
  CURLcode نتيجو = CURLE_OK;
  بنية UserDefined *s = &معلومات->سيٽ;

  بدل(option) {
  حالة CURLOPT_REQUEST_هدف:
    أرجع Curl_setstropt(معلومات, STRING_هدف, ptr);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_NETRC
  حالة CURLOPT_NETRC_FILE:
    أرجع Curl_setstropt(معلومات, STRING_NETRC_FILE, ptr);
#جيڪڏهن_پورو
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
    s->اندريون_سيٽ = ptr;
    اكسر;
  حالة CURLOPT_WRITEDATA:
    s->ٻاهريون = ptr;
    اكسر;
  حالة CURLOPT_DEBUGDATA:
    s->debugdata = ptr;
    اكسر;
  حالة CURLOPT_PROGRESSDATA:
    s->ترقي_client = ptr;
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
  حالة CURLOPT_RESOLVER_شروعات_معلومات:
    s->resolver_شروعات_client = ptr;
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
    نتيجو = Curl_setstropt(معلومات, STRING_سيٽ_URL, ptr);
    Curl_bufref_سيٽ(&معلومات->state.url,
                    CURL_EASY_STR(معلومات, STRING_سيٽ_URL), 0, عدم);
    اكسر;

  حالة CURLOPT_USERPWD: {
    حرف *u = عدم, *p = عدم;
    نتيجو = setstropt_userpwd(ptr, &u, &p);
    إذا(!نتيجو) {
      نتيجو = CURL_EASY_STR_SETN(معلومات, STRING_USERNAME, u);
      u = عدم;
    }
    إذا(!نتيجو) {
      نتيجو = CURL_EASY_STR_SETN(معلومات, STRING_PASSWORD, p);
      p = عدم;
    }
    curlx_free(u);
    curlx_free(p);
    أرجع نتيجو;
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
    أرجع Curl_setstropt(معلومات, STRING_سيٽ_RANGE, ptr);
  حالة CURLOPT_ذاتي:
    s->ذاتي_معلومات = ptr;
    اكسر;
  حالة CURLOPT_PROTOCOLS_STR:
    إذا(ptr) {
      curl_prot_t protos;
      نتيجو = protocol2num(ptr, &protos);
      إذا(!نتيجو)
        s->allowed_protocols = protos;
    }
    وإلا
      /* make a NULL argument reset to default */
      s->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
    اكسر;
  حالة CURLOPT_REDIR_PROTOCOLS_STR:
    إذا(ptr) {
      curl_prot_t protos;
      نتيجو = protocol2num(ptr, &protos);
      إذا(!نتيجو)
        s->redir_protocols = protos;
    }
    وإلا
      /* make a NULL argument reset to default */
      s->redir_protocols = (curl_prot_t)CURLPROTO_REDIR;
    اكسر;
  حالة CURLOPT_DEFAULT_PROTOCOL:
    /* Set the protocol to use when the URL does not include any protocol */
    أرجع Curl_setstropt(معلومات, STRING_DEFAULT_PROTOCOL, ptr);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_SMTP
  حالة CURLOPT_MAIL_FROM:
    /* Set the SMTP mail originator */
    أرجع Curl_setstropt(معلومات, STRING_MAIL_FROM, ptr);
  حالة CURLOPT_MAIL_AUTH:
    /* Set the SMTP auth originator */
    أرجع Curl_setstropt(معلومات, STRING_MAIL_AUTH, ptr);
#جيڪڏهن_پورو
  حالة CURLOPT_SASL_AUTHZID:
    /* Authorization identity (identity to act as) */
    أرجع Curl_setstropt(معلومات, STRING_SASL_AUTHZID, ptr);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_RTSP
  حالة CURLOPT_RTSP_SESSION_ID:
    Curl_peer_unlink(&معلومات->state.rtsp_session_origin);
    أرجع Curl_setstropt(معلومات, STRING_RTSP_SESSION_ID, ptr);
  حالة CURLOPT_RTSP_وهڪرو_URI:
    أرجع Curl_setstropt(معلومات, STRING_RTSP_وهڪرو_URI, ptr);
  حالة CURLOPT_RTSP_TRANSPORT:
    أرجع Curl_setstropt(معلومات, STRING_RTSP_TRANSPORT, ptr);
  حالة CURLOPT_INTERLEAVEDATA:
    s->rtp_ٻاهريون = ptr;
    اكسر;
#جيڪڏهن_پورو /* !CURL_DISABLE_RTSP */
  حالة CURLOPT_TLSAUTH_USERNAME:
  حالة CURLOPT_TLSAUTH_PASSWORD:
  حالة CURLOPT_TLSAUTH_TYPE:
  حالة CURLOPT_PROXY_TLSAUTH_USERNAME:
  حالة CURLOPT_PROXY_TLSAUTH_PASSWORD:
  حالة CURLOPT_PROXY_TLSAUTH_TYPE:
    أرجع CURLE_NOT_BUILT_اندريون;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HSTS
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
        أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
    إذا(ptr) {
      نتيجو = Curl_setstropt(معلومات, STRING_HSTS, ptr);
      إذا(نتيجو)
        أرجع نتيجو;
      /* this needs to build a list of filenames to read from, so that it can
         read them later, as we might get a shared HSTS handle to load them
         into */
      h = curl_slist_append(معلومات->state.hstslist, ptr);
      إذا(!h) {
        curl_slist_free_all(معلومات->state.hstslist);
        معلومات->state.hstslist = عدم;
        أرجع CURLE_ٻاهريون_جو_يادگيري;
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
#جيڪڏهن_پورو /* !CURL_DISABLE_HSTS */
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_ALTSVC
  حالة CURLOPT_ALTSVC:
    إذا(!معلومات->asi) {
      معلومات->asi = Curl_altsvc_init();
      إذا(!معلومات->asi)
        أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
    نتيجو = Curl_setstropt(معلومات, STRING_ALTSVC, ptr);
    إذا(نتيجو)
      اكسر;
    إذا(ptr)
      أرجع Curl_altsvc_ڀريو(معلومات->asi, ptr);
    اكسر;
#جيڪڏهن_پورو /* !CURL_DISABLE_ALTSVC */
  حالة CURLOPT_ECH:
    أرجع setopt_ech(معلومات, ptr);
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }
  أرجع نتيجو;
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
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
    setopt_cptr_proxy,
#جيڪڏهن_پورو
    setopt_cptr_net,
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FTP
    setopt_cptr_ftp,
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_SSH
    setopt_cptr_ssh,
#جيڪڏهن_پورو
#إذا !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
    setopt_cptr_http_mqtt,
#جيڪڏهن_پورو
  };
  size_t i;

  لكل(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode نتيجو = setopt_call[i](معلومات, option, ptr);
    إذا(نتيجو != CURLE_UNKNOWN_OPTION)
      أرجع نتيجو;
  }
  أرجع CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode setopt_func(بنية Curl_easy *معلومات, CURLoption option,
                            va_list param)
{
  بنية UserDefined *s = &معلومات->سيٽ;
  بدل(option) {
  حالة CURLOPT_PROGRESSFUNCTION:
    /*
     * Progress callback function
     */
    s->fprogress = va_arg(param, curl_ترقي_callback);
    إذا(s->fprogress)
      معلومات->ترقي.callback = TRUE; /* no longer internal */
    وإلا
      معلومات->ترقي.callback = FALSE; /* NULL enforces internal */
    اكسر;

  حالة CURLOPT_XFERINFOFUNCTION:
    /*
     * Transfer info callback function
     */
    s->fxferinfo = va_arg(param, curl_xferinfo_callback);
    إذا(s->fxferinfo)
      معلومات->ترقي.callback = TRUE; /* no longer internal */
    وإلا
      معلومات->ترقي.callback = FALSE; /* NULL enforces internal */

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
    s->fwrite_سرو = va_arg(param, curl_write_callback);
    اكسر;
  حالة CURLOPT_WRITEFUNCTION:
    /*
     * Set data write callback
     */
    s->fwrite_func = va_arg(param, curl_write_callback);
    إذا(!s->fwrite_func)
#إذا defined(__clang__) && __clang_major__ >= 16
#هدايت clang diagnostic ڌڪو
#هدايت clang diagnostic ignored "-Wcast-function-type-strict"
#جيڪڏهن_پورو
      /* When set to NULL, reset to our internal default function */
      s->fwrite_func = (curl_write_callback)fwrite;
#إذا defined(__clang__) && __clang_major__ >= 16
#هدايت clang diagnostic ڪڍو
#جيڪڏهن_پورو
    اكسر;
  حالة CURLOPT_READFUNCTION:
    /*
     * Read data callback
     */
    s->fread_func_سيٽ = va_arg(param, curl_read_callback);
    إذا(!s->fread_func_سيٽ) {
      s->is_fread_سيٽ = 0;
#إذا defined(__clang__) && __clang_major__ >= 16
#هدايت clang diagnostic ڌڪو
#هدايت clang diagnostic ignored "-Wcast-function-type-strict"
#جيڪڏهن_پورو
      /* When set to NULL, reset to our internal default function */
      s->fread_func_سيٽ = (curl_read_callback)fread;
#إذا defined(__clang__) && __clang_major__ >= 16
#هدايت clang diagnostic ڪڍو
#جيڪڏهن_پورو
    }
    وإلا
      s->is_fread_سيٽ = 1;
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
#جيڪڏهن_وصف_ٿيل USE_SSL
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_SSL_CTX)) {
      s->ssl_fsslctx = va_arg(param, curl_ssl_ctx_callback);
      اكسر;
    }
    وإلا
#جيڪڏهن_پورو
      أرجع CURLE_NOT_BUILT_اندريون;

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

  حالة CURLOPT_RESOLVER_شروعات_FUNCTION:
    /*
     * resolver start callback function: called before a new resolver request
     * is started
     */
    s->resolver_شروعات = va_arg(param, curl_resolver_شروعات_callback);
    اكسر;

#جيڪڏهن_وصف_ٿيل USE_SSH
#جيڪڏهن_وصف_ٿيل USE_LIBSSH2
  حالة CURLOPT_SSH_HOSTKEYFUNCTION:
    /* the callback to check the hostkey without the knownhost file */
    s->ssh_hostkeyfunc = va_arg(param, curl_sshhostkeycallback);
    اكسر;
#جيڪڏهن_پورو

  حالة CURLOPT_SSH_KEYFUNCTION:
    /* setting to NULL is fine since the ssh.c functions themselves will
       then revert to use the internal default */
    s->ssh_keyfunc = va_arg(param, curl_sshkeycallback);
    اكسر;

#جيڪڏهن_پورو /* USE_SSH */

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_RTSP
  حالة CURLOPT_INTERLEAVEFUNCTION:
    /* Set the user defined RTP write function */
    s->fwrite_rtp = va_arg(param, curl_write_callback);
    اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FTP
  حالة CURLOPT_CHUNK_BGN_FUNCTION:
    s->chunk_bgn = va_arg(param, curl_chunk_bgn_callback);
    اكسر;
  حالة CURLOPT_CHUNK_پڇاڙي_FUNCTION:
    s->chunk_پڇاڙي = va_arg(param, curl_chunk_پڇاڙي_callback);
    اكسر;
  حالة CURLOPT_FNMATCH_FUNCTION:
    s->fnmatch = va_arg(param, curl_fnmatch_callback);
    اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP
  حالة CURLOPT_TRAILERFUNCTION:
    s->trailer_callback = va_arg(param, curl_trailer_callback);
    اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HSTS
  حالة CURLOPT_HSTSREADFUNCTION:
    s->hsts_read = va_arg(param, curl_hstsread_callback);
    اكسر;
  حالة CURLOPT_HSTSWRITEFUNCTION:
    s->hsts_write = va_arg(param, curl_hstswrite_callback);
    اكسر;
#جيڪڏهن_پورو
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
  بنية UserDefined *s = &معلومات->سيٽ;
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
      أرجع CURLE_BAD_FUNCTION_دليل;

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
      أرجع CURLE_BAD_FUNCTION_دليل;
    s->filesize = offt;
    اكسر;
  حالة CURLOPT_MAX_SEND_رفتار_LARGE:
    /*
     * When transfer uploads are faster than CURLOPT_MAX_SEND_SPEED_LARGE
     * bytes per second the transfer is throttled..
     */
    إذا(offt < 0)
      أرجع CURLE_BAD_FUNCTION_دليل;
    s->max_send_رفتار = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&معلومات->ترقي.ul.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    اكسر;
  حالة CURLOPT_MAX_RECV_رفتار_LARGE:
    /*
     * When receiving data faster than CURLOPT_MAX_RECV_SPEED_LARGE bytes per
     * second the transfer is throttled..
     */
    إذا(offt < 0)
      أرجع CURLE_BAD_FUNCTION_دليل;
    s->max_recv_رفتار = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&معلومات->ترقي.dl.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    اكسر;
  حالة CURLOPT_RESUME_FROM_LARGE:
    /*
     * Resume transfer at the given file position
     */
    إذا(offt < -1)
      أرجع CURLE_BAD_FUNCTION_دليل;
    s->سيٽ_resume_from = offt;
    اكسر;
  حالة CURLOPT_MAXFILESIZE_LARGE:
    /*
     * Set the maximum size of a file to download.
     */
    إذا(offt < 0)
      أرجع CURLE_BAD_FUNCTION_دليل;
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
  بنية UserDefined *s = &معلومات->سيٽ;
  بدل(option) {
  حالة CURLOPT_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use
     */
    أرجع Curl_setblobopt(&s->blobs[BLOB_CERT], blob);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  حالة CURLOPT_PROXY_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use for proxy
     */
    أرجع Curl_setblobopt(&s->blobs[BLOB_CERT_PROXY], blob);
  حالة CURLOPT_PROXY_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use for proxy
     */
    أرجع Curl_setblobopt(&s->blobs[BLOB_ڪنجي_PROXY], blob);
  حالة CURLOPT_PROXY_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection proxy.
     * Specify entire PEM of the CA certificate
     */
#جيڪڏهن_وصف_ٿيل USE_SSL
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_CAINFO_BLOB)) {
      CURLcode نتيجو = Curl_setblobopt(&s->blobs[BLOB_CAINFO_PROXY], blob);
      إذا(!نتيجو)
        s->proxy_ssl.custom_cablob = TRUE;
      أرجع نتيجو;
    }
#جيڪڏهن_پورو
    أرجع CURLE_NOT_BUILT_اندريون;
  حالة CURLOPT_PROXY_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_ISSUERCERT_BLOB))
      أرجع Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT_PROXY], blob);
    أرجع CURLE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
  حالة CURLOPT_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use
     */
    أرجع Curl_setblobopt(&s->blobs[BLOB_ڪنجي], blob);
  حالة CURLOPT_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection.
     * Specify entire PEM of the CA certificate
     */
#جيڪڏهن_وصف_ٿيل USE_SSL
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_CAINFO_BLOB)) {
      CURLcode نتيجو = Curl_setblobopt(&s->blobs[BLOB_CAINFO], blob);
      إذا(!نتيجو)
        s->ssl.custom_cablob = TRUE;
      أرجع نتيجو;
    }
#جيڪڏهن_پورو
    أرجع CURLE_NOT_BUILT_اندريون;
  حالة CURLOPT_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    إذا(Curl_ssl_supports(معلومات, SSLSUPP_ISSUERCERT_BLOB))
      أرجع Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT], blob);
    أرجع CURLE_NOT_BUILT_اندريون;

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
    أرجع setopt_long(معلومات, option, va_arg(param, ڊگهو));
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
      أرجع setopt_اشارا(معلومات, option, param);
    حالة CURLOPT_وهڪرو_DEPENDS:   /* CURL * */
    حالة CURLOPT_وهڪرو_DEPENDS_E: /* CURL * */
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
#وصف_رد curl_easy_setopt
CURLcode curl_easy_setopt(CURL *curl, CURLoption option, ...)
{
  بنية Curl_eapi_guard guard;
  CURLcode نتيجو;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_setopt, &نتيجو)) {
    بنية Curl_easy *معلومات = curl;
    va_list arg;

    va_start(arg, option);

    نتيجو = Curl_vsetopt(معلومات, option, arg);

    va_end(arg);
    إذا(نتيجو == CURLE_BAD_FUNCTION_دليل)
      failf(معلومات, "setopt 0x%x got bad argument", (بنانشان صحيح)option);
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع نتيجو;
}
