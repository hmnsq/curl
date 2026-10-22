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

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_LINUX_TCP_H
#شامل <linux/tcp.h>
#وگرنہ_اگر defined(HAVE_NETINET_TCP_H)
#شامل <netinet/tcp.h>
#اگر_ختم

#شامل "urldata.h"
#شامل "url.h"
#شامل "progress.h"
#شامل "content_encoding.h"
#شامل "strcase.h"
#شامل "curl_share.h"
#شامل "vtls/vtls.h"
#شامل "curl_trc.h"
#شامل "setopt.h"
#شامل "altsvc.h"
#شامل "hsts.h"
#شامل "tftp.h"
#شامل "curlx/strdup.h"
#شامل "escape.h"
#شامل "bufref.h"
#شامل "vauth/vauth.h"

ساکن CURLcode setopt_سیٹ_timeout_sec(timediff_t *ptimeout_ms, طویل secs)
{
  اگر(secs < 0)
    واپس CURLE_BAD_FUNCTION_دلیل;
#اگر LONG_MAX > (TIMEDIFF_T_MAX / 1000)
  اگر(secs > (TIMEDIFF_T_MAX / 1000)) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    واپس CURLE_OK;
  }
#اگر_ختم
  *ptimeout_ms = (timediff_t)secs * 1000;
  واپس CURLE_OK;
}

ساکن CURLcode setopt_سیٹ_timeout_ms(timediff_t *ptimeout_ms, طویل ms)
{
  اگر(ms < 0)
    واپس CURLE_BAD_FUNCTION_دلیل;
#اگر LONG_MAX > TIMEDIFF_T_MAX
  اگر(ms > TIMEDIFF_T_MAX) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    واپس CURLE_OK;
  }
#اگر_ختم
  *ptimeout_ms = (timediff_t)ms;
  واپس CURLE_OK;
}

CURLcode Curl_setstropt(ساخت Curl_easy *ڈیٹا,
                        شمار dupstring id, ثابت حرف *s)
{
  size_t slen = s ? لڑی_لمبائی(s) : 0;
  DEBUGASSERT((غیرعلامتی)id <= UINT8_MAX);
  اگر(s && (slen > CURL_MAX_داخلہ_لمبائی))
    واپس CURLE_BAD_FUNCTION_دلیل;

  واپس CURL_EASY_STR_سیٹ(ڈیٹا, (uint8_t)id, s, slen);
}

CURLcode Curl_setblobopt(ساخت curl_blob **blobp,
                         ثابت ساخت curl_blob *blob)
{
  /* free the previous storage at `blobp' and replace by a dynamic storage
     copy of blob. If CURL_BLOB_COPY is set, the data is copied. */

  curlx_safefree(*blobp);

  اگر(blob) {
    ساخت curl_blob *nblob;
    اگر(!blob->ڈیٹا || !blob->len || (blob->len > CURL_MAX_داخلہ_لمبائی))
      واپس CURLE_BAD_FUNCTION_دلیل;
    nblob = (ساخت curl_blob *)
      curlx_malloc(حجم(ساخت curl_blob) +
                   ((blob->flags & CURL_BLOB_COPY) ? blob->len : 0));
    اگر(!nblob)
      واپس CURLE_برآمد_کا_حافظہ;
    *nblob = *blob;
    اگر(blob->flags & CURL_BLOB_COPY) {
      /* put the data after the blob struct in memory */
      nblob->ڈیٹا = (حرف *)nblob + حجم(ساخت curl_blob);
      حافظہ_نقل(nblob->ڈیٹا, blob->ڈیٹا, blob->len);
    }

    *blobp = nblob;
    واپس CURLE_OK;
  }

  واپس CURLE_OK;
}

ساکن CURLcode setstropt_userpwd(ثابت حرف *option, حرف **userp,
                                  حرف **passwdp)
{
  حرف *user = تہی;
  حرف *passwd = تہی;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Parse the login details if specified. If not, then we treat NULL as a
     hint to clear the existing data */
  اگر(option) {
    size_t len = لڑی_لمبائی(option);
    CURLcode نتیجہ;
    اگر(len > CURL_MAX_داخلہ_لمبائی)
      واپس CURLE_BAD_FUNCTION_دلیل;

    نتیجہ = Curl_parse_login_details(option, len, &user, &passwd, تہی);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  curlx_free(*userp);
  *userp = user;

  curlx_strzero(*passwdp);
  curlx_free(*passwdp);
  *passwdp = passwd;

  واپس CURLE_OK;
}

ساکن CURLcode setstropt_interface(ساخت Curl_easy *ڈیٹا, حرف *option)
{
  اگر(option && !option[0])
    option = تہی;
  اگر(option) {
    /* Parse for checking syntax */
    CURLcode نتیجہ = Curl_parse_interface(option, تہی, تہی, تہی);
    اگر(نتیجہ)
      واپس نتیجہ;
    واپس CURL_EASY_STR_سیٹ(ڈیٹا, STRING_INTERFACE_OPT,
                             option, لڑی_لمبائی(option));
  }
  CURL_EASY_STR_CLEAR(ڈیٹا, STRING_INTERFACE_OPT);
  واپس CURLE_OK;
}

#اگر_تعریف_شدہ USE_SSL
#تعریف_کرو C_SSLVERSION_قدر(x)     ((x) & 0xffff)
#تعریف_کرو C_SSLVERSION_MAX_قدر(x) ((غیرعلامتی طویل)(x) & 0xffff0000)
#اگر_ختم

ساکن CURLcode protocol2num(ثابت حرف *str, curl_prot_t *val)
{
  /*
   * We are asked to cherry-pick protocols, so play it safe and disallow all
   * protocols to start with, and re-add the wanted ones back in.
   */
  *val = 0;

  اگر(!str)
    واپس CURLE_BAD_FUNCTION_دلیل;

  اگر(curl_strequal(str, "all")) {
    *val = ~(curl_prot_t)0;
    واپس CURLE_OK;
  }

  کرو {
    ثابت حرف *token = str;
    size_t tlen;

    str = strchr(str, ',');
    tlen = str ? (size_t)(str - token) : لڑی_لمبائی(token);
    اگر(tlen) {
      ثابت ساخت Curl_scheme *h = Curl_getn_scheme(token, tlen);

      اگر(!h || !h->تسلسل)
        واپس CURLE_UNSUPPORTED_PROTOCOL;

      *val |= h->protocol;
    }
  } جب(str && str++);

  اگر(!*val)
    /* no protocol listed */
    واپس CURLE_BAD_FUNCTION_دلیل;
  واپس CURLE_OK;
}

#اگر !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
ساکن CURLcode httpauth(ساخت Curl_easy *ڈیٹا, bool proxy,
                         غیرعلامتی طویل auth)
{
  اگر(auth != CURLAUTH_NONE) {
    صحیح bitcheck = 0;
    bool authbits = FALSE;
    اگر(auth & CURLAUTH_DIGEST_IE) {
      auth |= CURLAUTH_DIGEST; /* set standard digest bit */
      auth &= ~CURLAUTH_DIGEST_IE; /* drop the legacy bit */
    }

    /* switch off bits we cannot support */
#اگر_غیر_تعریف_شدہ USE_NTLM
    auth &= ~CURLAUTH_NTLM; /* no NTLM support */
#اگر_ختم
#اگر_غیر_تعریف_شدہ USE_SPNEGO
    auth &= ~CURLAUTH_NEGOTIATE; /* no Negotiate (SPNEGO) auth without GSS-API
                                    or SSPI */
#اگر_ختم

    /* check if any auth bit lower than CURLAUTH_ONLY is still set */
    جب(bitcheck < 31) {
      اگر(auth & (1UL << bitcheck++)) {
        authbits = TRUE;
        توڑو;
      }
    }
    اگر(!authbits)
      واپس CURLE_NOT_BUILT_اندر; /* no supported types left! */
  }
  اگر(proxy)
    ڈیٹا->سیٹ.proxyauth = (uint32_t)auth;
  ورنہ
    ڈیٹا->سیٹ.httpauth = (uint32_t)auth;
  واپس CURLE_OK;
}
#اگر_ختم /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */

#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
ساکن CURLcode setopt_HTTP_نسخہ(ساخت Curl_easy *ڈیٹا, طویل arg)
{
  /*
   * This sets a requested HTTP version to be used. The value is one of
   * the listed enums in curl/curl.h.
   */
  انتخاب(arg) {
  حالت CURL_HTTP_نسخہ_NONE:
    /* accepted */
    توڑو;
  حالت CURL_HTTP_نسخہ_1_0:
  حالت CURL_HTTP_نسخہ_1_1:
    /* accepted */
    توڑو;
#اگر_تعریف_شدہ USE_HTTP2
  حالت CURL_HTTP_نسخہ_2_0:
  حالت CURL_HTTP_نسخہ_2_TLS:
  حالت CURL_HTTP_نسخہ_2_پیشگی_KNOWLEDGE:
    /* accepted */
    توڑو;
#اگر_ختم
#اگر_تعریف_شدہ USE_HTTP3
  حالت CURL_HTTP_نسخہ_3:
  حالت CURL_HTTP_نسخہ_3_ONLY:
    /* accepted */
    توڑو;
#اگر_ختم
  طےشدہ:
    /* not accepted */
    اگر(arg < CURL_HTTP_نسخہ_NONE)
      واپس CURLE_BAD_FUNCTION_دلیل;
    واپس CURLE_UNSUPPORTED_PROTOCOL;
  }
  ڈیٹا->سیٹ.httpwant = (غیرعلامتی حرف)arg;
  واپس CURLE_OK;
}
#اگر_ختم /* !CURL_DISABLE_HTTP */

#اگر_تعریف_شدہ USE_SSL
CURLcode Curl_setopt_SSLVERSION(ساخت Curl_easy *ڈیٹا, CURLoption option,
                                طویل arg)
{
  /*
   * Set explicit SSL version to try to connect with, as some SSL
   * implementations are lame.
   */
  {
    طویل نسخہ_2, نسخہ_max;
    ساخت ssl_easy_config *sslc = &ڈیٹا->سیٹ.ssl;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
    اگر(option != CURLOPT_SSLVERSION)
      sslc = &ڈیٹا->سیٹ.proxy_ssl;
#ورنہ
    (خالی)option; /*غیر_مستعمل*/
#اگر_ختم
    نسخہ_2 = C_SSLVERSION_قدر(arg);
    نسخہ_max = (طویل)C_SSLVERSION_MAX_قدر(arg);

    اگر(نسخہ_2 < CURL_SSLVERSION_DEFAULT ||
       نسخہ_2 == CURL_SSLVERSION_SSLv2 ||
       نسخہ_2 == CURL_SSLVERSION_SSLv3 ||
       نسخہ_2 >= CURL_SSLVERSION_آخری ||
       نسخہ_max < CURL_SSLVERSION_MAX_NONE ||
       نسخہ_max >= CURL_SSLVERSION_MAX_آخری)
      واپس CURLE_BAD_FUNCTION_دلیل;
    اگر(نسخہ_2 == CURL_SSLVERSION_DEFAULT)
      نسخہ_2 = CURL_SSLVERSION_TLSv1_2;

    sslc->نسخہ_2 = (غیرعلامتی حرف)نسخہ_2;
    sslc->نسخہ_max = (غیرعلامتی صحیح)نسخہ_max;
  }
  واپس CURLE_OK;
}
#اگر_ختم /* !USE_SSL */

#اگر_غیر_تعریف_شدہ CURL_DISABLE_RTSP
ساکن CURLcode setopt_RTSP_REQUEST(ساخت Curl_easy *ڈیٹا, طویل arg)
{
  /*
   * Set the RTSP request method (OPTIONS, SETUP, PLAY, etc...) Would this be
   * better if the RTSPREQ_* were moved into here?
   */
  اگر((arg <= CURL_RTSPREQ_NONE) || (arg >= CURL_RTSPREQ_آخری))
    واپس CURLE_BAD_FUNCTION_دلیل;

  ڈیٹا->سیٹ.rtspreq = (غیرعلامتی حرف)arg;
  واپس CURLE_OK;
}
#اگر_ختم /* !CURL_DISABLE_RTSP */

ساکن CURLcode setopt_long_bool(ساخت Curl_easy *ڈیٹا, CURLoption option,
                                 طویل arg)
{
  bool enabled = !!arg;
  صحیح ok = 1;
  ساخت UserDefined *s = &ڈیٹا->سیٹ;
  انتخاب(option) {
  حالت CURLOPT_FORBID_REUSE:
    /*
     * When this transfer is done, it must not be left to be reused by a
     * subsequent transfer but shall be closed immediately.
     */
    s->reuse_forbid = enabled;
    توڑو;
  حالت CURLOPT_FRESH_CONNECT:
    /*
     * This transfer shall not use a previously cached connection but
     * should be made with a fresh new connect!
     */
    s->reuse_fresh = enabled;
    توڑو;
  حالت CURLOPT_VERBOSE:
    /*
     * Verbose means infof() calls that give a lot of information about
     * the connection and transfer procedures as well as internal choices.
     */
    s->verbose = enabled;
    توڑو;
  حالت CURLOPT_سرخی:
    /*
     * Set to include the header in the general data output stream.
     */
    s->include_سرخی = enabled;
    توڑو;
  حالت CURLOPT_NOPROGRESS:
    /*
     * Shut off the internal supported progress meter
     */
    ڈیٹا->پیش_رفت.hide = enabled;
    توڑو;
  حالت CURLOPT_NOBODY:
    /*
     * Do not include the body part in the output data stream.
     */
    s->opt_no_body = enabled;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
    اگر(s->opt_no_body)
      /* in HTTP lingo, no body means using the HEAD request... */
      s->طریقہ = HTTPREQ_سر;
    ورنہ اگر(s->طریقہ == HTTPREQ_سر)
      s->طریقہ = HTTPREQ_GET;
#اگر_ختم
    توڑو;
  حالت CURLOPT_FAILONERROR:
    /*
     * Do not output the >=400 error code HTML-page, but instead only
     * return error.
     */
    s->http_fail_on_error = enabled;
    توڑو;
  حالت CURLOPT_KEEP_SENDING_ON_ERROR:
    s->http_keep_sending_on_error = enabled;
    توڑو;
  حالت CURLOPT_UPLOAD:
  حالت CURLOPT_PUT:
    /*
     * We want to send data to the remote host. If this is HTTP, that equals
     * using the PUT request.
     */
    اگر(enabled) {
      /* If this is HTTP, PUT is what's needed to "upload" */
      s->طریقہ = HTTPREQ_PUT;
      s->opt_no_body = FALSE; /* this is implied */
    }
    ورنہ
      /* In HTTP, the opposite of upload is GET (unless NOBODY is true as
         then this can be changed to HEAD later on) */
      s->طریقہ = HTTPREQ_GET;
    توڑو;
  حالت CURLOPT_FILETIME:
    /*
     * Try to get the file time of the remote document. The time will
     * later (possibly) become available using curl_easy_getinfo().
     */
    s->get_filetime = enabled;
    توڑو;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
  حالت CURLOPT_HTTP09_ALLOWED:
    s->http09_allowed = enabled;
    توڑو;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_COOKIES
  حالت CURLOPT_COOKIESESSION:
    /*
     * Set this option to TRUE to start a new "cookie session". It will
     * prevent the forthcoming read-cookies-from-file actions to accept
     * cookies that are marked as being session cookies, as they belong to a
     * previous session.
     */
    s->cookiesession = enabled;
    توڑو;
#اگر_ختم
  حالت CURLOPT_AUTOREFERER:
    /*
     * Switch on automatic referer that gets set if curl follows locations.
     */
    s->http_auto_referer = enabled;
    توڑو;
  حالت CURLOPT_TRANSFER_ENCODING:
    s->http_transfer_encoding = enabled;
    توڑو;
  حالت CURLOPT_UNRESTRICTED_AUTH:
    /*
     * Send authentication (user+password) when following locations, even when
     * hostname changed.
     */
    s->allow_auth_مقصد_other_hosts = enabled;
    توڑو;
  حالت CURLOPT_HTTP_TRANSFER_DECODING:
    /*
     * disable libcurl transfer encoding is used
     */
    s->http_te_skip = !enabled; /* reversed */
    توڑو;
  حالت CURLOPT_HTTP_CONTENT_DECODING:
    /*
     * raw data passed to the application when content encoding is used
     */
    s->http_ce_skip = !enabled; /* reversed */
    توڑو;
  حالت CURLOPT_HTTPGET:
    /*
     * Set to force us do HTTP GET
     */
    اگر(enabled) {
      s->طریقہ = HTTPREQ_GET;
      s->opt_no_body = FALSE; /* this is implied */
    }
    توڑو;
  حالت CURLOPT_POST:
    /* Does this option serve a purpose anymore? Yes it does, when
       CURLOPT_POSTFIELDS is not used and the POST data is read off the
       callback! */
    اگر(enabled) {
      s->طریقہ = HTTPREQ_POST;
      s->opt_no_body = FALSE; /* this is implied */
    }
    ورنہ
      s->طریقہ = HTTPREQ_GET;
    توڑو;
#اگر_ختم /* !CURL_DISABLE_HTTP */
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  حالت CURLOPT_HTTPPROXYTUNNEL:
    /*
     * Tunnel operations through the proxy instead of normal proxy use
     */
    s->tunnel_thru_httpproxy = enabled;
    توڑو;
  حالت CURLOPT_HAPROXYPROTOCOL:
    /*
     * Set to send the HAProxy Proxy Protocol header
     */
    s->haproxyprotocol = enabled;
    توڑو;
  حالت CURLOPT_PROXY_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for proxy.
     */
    s->proxy_ssl.verifypeer = enabled;

    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(ڈیٹا, TRUE);
    توڑو;
  حالت CURLOPT_PROXY_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for proxy
     */
    s->proxy_ssl.verifyhost = enabled;
    ok = 2;
    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(ڈیٹا, TRUE);
    توڑو;
  حالت CURLOPT_PROXY_TRANSFER_منوال:
    /*
     * set transfer mode (;type=<a|i>) when doing FTP via an HTTP proxy
     */
    s->proxy_transfer_منوال = enabled;
    توڑو;
#اگر_ختم /* !CURL_DISABLE_PROXY */
#اگر defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  حالت CURLOPT_SOCKS5_GSSAPI_NEC:
    /*
     * Set flag for NEC SOCKS5 support
     */
    s->socks5_gssapi_nec = enabled;
    توڑو;
#اگر_ختم
#اگر_تعریف_شدہ CURL_LIST_ONLY_PROTOCOL
  حالت CURLOPT_DIRLISTONLY:
    /*
     * An option that changes the command to one that asks for a list only, no
     * file info details. Used for FTP, POP3 and SFTP.
     */
    s->list_only = enabled;
    توڑو;
#اگر_ختم
  حالت CURLOPT_APPEND:
    /*
     * We want to upload and append to an existing file. Used for FTP and
     * SFTP.
     */
    s->remote_append = enabled;
    توڑو;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
  حالت CURLOPT_FTP_USE_EPRT:
    s->ftp_use_eprt = enabled;
    توڑو;
  حالت CURLOPT_FTP_USE_EPSV:
    s->ftp_use_epsv = enabled;
    توڑو;
  حالت CURLOPT_FTP_USE_PRET:
    s->ftp_use_pret = enabled;
    توڑو;
  حالت CURLOPT_FTP_SKIP_PASV_IP:
    /*
     * Enable or disable FTP_SKIP_PASV_IP, which will disable/enable the
     * bypass of the IP address in PASV responses.
     */
    s->ftp_skip_ip = enabled;
    توڑو;
  حالت CURLOPT_WILDCARDMATCH:
    s->wildcard_enabled = enabled;
    توڑو;
#اگر_ختم
  حالت CURLOPT_CRLF:
    /*
     * Kludgy option to enable CRLF conversions. Subject for removal.
     */
    s->crlf = enabled;
    توڑو;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_TFTP
  حالت CURLOPT_TFTP_NO_OPTIONS:
    /*
     * Option that prevents libcurl from sending TFTP option requests to the
     * server.
     */
    s->tftp_no_options = enabled;
    توڑو;
#اگر_ختم /* !CURL_DISABLE_TFTP */
  حالت CURLOPT_TRANSFERTEXT:
    /*
     * This option was previously named 'FTPASCII'. Renamed to work with
     * more protocols than merely FTP.
     *
     * Transfer using ASCII (instead of BINARY).
     */
    s->prefer_ascii = enabled;
    توڑو;
  حالت CURLOPT_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying.
     */
    s->ssl.verifypeer = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(ڈیٹا, FALSE);
    توڑو;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DOH
  حالت CURLOPT_DOH_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for DoH.
     */
    s->doh_verifypeer = enabled;
    توڑو;
  حالت CURLOPT_DOH_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for DoH
     */
    s->doh_verifyhost = enabled;
    ok = 2;
    توڑو;
  حالت CURLOPT_DOH_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying for DoH.
     */
    اگر(!Curl_ssl_cert_حالت_request())
      واپس CURLE_NOT_BUILT_اندر;

    s->doh_verifystatus = enabled;
    ok = 2;
    توڑو;
#اگر_ختم /* !CURL_DISABLE_DOH */
  حالت CURLOPT_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate
     */

    /* Obviously people are not reading documentation and too many thought
       this argument took a boolean when it was not and misused it.
       Treat 1 and 2 the same */
    s->ssl.verifyhost = enabled;
    ok = 2;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(ڈیٹا, FALSE);
    توڑو;
  حالت CURLOPT_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying.
     */
    اگر(!Curl_ssl_cert_حالت_request())
      واپس CURLE_NOT_BUILT_اندر;

    s->ssl.verifystatus = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(ڈیٹا, FALSE);
    توڑو;
  حالت CURLOPT_CERTINFO:
#اگر_تعریف_شدہ USE_SSL
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_CERTINFO))
      s->ssl.certinfo = enabled;
    ورنہ
#اگر_ختم
      واپس CURLE_NOT_BUILT_اندر;
    توڑو;
  حالت CURLOPT_NOSIGNAL:
    /*
     * The application asks not to set any signal() or alarm() handlers,
     * even when using a timeout.
     */
    s->no_signal = enabled;
    توڑو;
  حالت CURLOPT_TCP_NODELAY:
    /*
     * Enable or disable TCP_NODELAY, which will disable/enable the Nagle
     * algorithm
     */
    s->tcp_nodelay = enabled;
    توڑو;
  حالت CURLOPT_IGNORE_CONTENT_لمبائی:
    s->ignorecl = enabled;
    توڑو;
  حالت CURLOPT_SSL_SESSIONID_CACHE:
    s->ssl.cache_session = enabled;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
    s->proxy_ssl.cache_session = s->ssl.cache_session;
#اگر_ختم
    توڑو;
#اگر_تعریف_شدہ USE_SSH
  حالت CURLOPT_SSH_COMPRESSION:
    s->ssh_compression = enabled;
    توڑو;
#اگر_ختم /* !USE_SSH */
#اگر_غیر_تعریف_شدہ CURL_DISABLE_SMTP
  حالت CURLOPT_MAIL_RCPT_ALLOWFAILS:
    /* allow RCPT TO command to fail for some recipients */
    s->mail_rcpt_allowfails = enabled;
    توڑو;
#اگر_ختم /* !CURL_DISABLE_SMTP */
  حالت CURLOPT_SASL_IR:
    /* Enable/disable SASL initial response */
    s->sasl_ir = enabled;
    توڑو;
  حالت CURLOPT_TCP_KEEPALIVE:
    s->tcp_keepalive = enabled;
    توڑو;
  حالت CURLOPT_TCP_FASTOPEN:
#اگر defined(CONNECT_ڈیٹا_IDEMPOTENT) || defined(MSG_FASTOPEN) || \
  defined(TCP_FASTOPEN_CONNECT)
    s->tcp_fastopen = enabled;
    توڑو;
#ورنہ
    واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
  حالت CURLOPT_SSL_ENABLE_ALPN:
    s->ssl_enable_alpn = enabled;
    توڑو;
  حالت CURLOPT_راستہ_AS_IS:
    s->راستہ_as_is = enabled;
    توڑو;
  حالت CURLOPT_PIPEWAIT:
    s->pipewait = enabled;
    توڑو;
  حالت CURLOPT_SUPPRESS_CONNECT_سرخیاں:
    s->suppress_connect_سرخیاں = enabled;
    توڑو;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_SHUFFLE_DNS
  حالت CURLOPT_DNS_SHUFFLE_پتے:
    s->dns_shuffle_پتے = enabled;
    توڑو;
#اگر_ختم
  حالت CURLOPT_DISALLOW_USERNAME_اندر_URL:
    s->disallow_username_اندر_url = enabled;
    توڑو;
  حالت CURLOPT_تیز_EXIT:
    s->quick_exit = enabled;
    توڑو;
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }
  اگر((arg > ok) || (arg < 0))
    /* reserve other values for future use */
    infof(ڈیٹا, "boolean setopt(%d) got unsupported argument %ld,"
          " treated as %d", (صحیح)option, arg, enabled);

  واپس CURLE_OK;
}

ساکن CURLcode قدر_range(طویل *قدر, طویل نیچے_error, طویل min, طویل max)
{
  اگر(*قدر < نیچے_error)
    واپس CURLE_BAD_FUNCTION_دلیل;
  ورنہ اگر(*قدر < min)
    *قدر = min;
  ورنہ اگر(*قدر > max)
    *قدر = max;
  واپس CURLE_OK;
}

ساکن CURLcode setopt_long_net(ساخت Curl_easy *ڈیٹا, CURLoption option,
                                طویل arg)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت UserDefined *s = &ڈیٹا->سیٹ;

  انتخاب(option) {
  حالت CURLOPT_DNS_CACHE_TIMEOUT:
    اگر(arg != -1)
      واپس setopt_سیٹ_timeout_sec(&s->dns_cache_timeout_ms, arg);
    s->dns_cache_timeout_ms = -1;
    توڑو;
  حالت CURLOPT_MAXCONNECTS:
    نتیجہ = قدر_range(&arg, 0, 0, زیادہ_سے_زیادہ_عدد);
    اگر(!نتیجہ)
      s->maxconnects = arg ? (uint32_t)arg : DEFAULT_CONNCACHE_سائز;
    توڑو;
  حالت CURLOPT_SERVER_RESPONSE_TIMEOUT:
    واپس setopt_سیٹ_timeout_sec(&s->server_response_timeout, arg);
  حالت CURLOPT_SERVER_RESPONSE_TIMEOUT_MS:
    واپس setopt_سیٹ_timeout_ms(&s->server_response_timeout, arg);
  حالت CURLOPT_کم_رفتار_حد_بندی:
    اگر(arg < 0)
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    ورنہ
      s->کم_رفتار_حد_بندی = arg;
    توڑو;
  حالت CURLOPT_کم_رفتار_وقت:
    نتیجہ = قدر_range(&arg, 0, 0, USHRT_MAX);
    اگر(!نتیجہ)
      s->کم_رفتار_وقت = (uint16_t)arg;
    توڑو;
  حالت CURLOPT_پورٹ:
    اگر((arg < 0) || (arg > 65535))
      واپس CURLE_BAD_FUNCTION_دلیل;
    s->use_پورٹ = (غیرعلامتی مختصر)arg;
    توڑو;
  حالت CURLOPT_TIMEOUT:
    واپس setopt_سیٹ_timeout_sec(&s->timeout, arg);
  حالت CURLOPT_TIMEOUT_MS:
    واپس setopt_سیٹ_timeout_ms(&s->timeout, arg);
  حالت CURLOPT_CONNECTTIMEOUT:
    واپس setopt_سیٹ_timeout_sec(&s->connecttimeout, arg);
  حالت CURLOPT_CONNECTTIMEOUT_MS:
    واپس setopt_سیٹ_timeout_ms(&s->connecttimeout, arg);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_BINDLOCAL
  حالت CURLOPT_LOCALPORT:
    اگر((arg < 0) || (arg > 65535))
      واپس CURLE_BAD_FUNCTION_دلیل;
    s->localport = curlx_sltous(arg);
    توڑو;
  حالت CURLOPT_LOCALPORTRANGE:
    اگر((arg < 0) || (arg > 65535))
      واپس CURLE_BAD_FUNCTION_دلیل;
    s->localportrange = curlx_sltous(arg);
    توڑو;
#اگر_ختم
  حالت CURLOPT_BUFFERSIZE:
    نتیجہ = قدر_range(&arg, 0, READBUFFER_MIN, READBUFFER_MAX);
    اگر(!نتیجہ)
      s->بفر_سائز = (غیرعلامتی صحیح)arg;
    توڑو;
  حالت CURLOPT_UPLOAD_BUFFERSIZE:
    نتیجہ = قدر_range(&arg, 0, UPLOADBUFFER_MIN, UPLOADBUFFER_MAX);
    اگر(!نتیجہ)
      s->upload_بفر_سائز = (غیرعلامتی صحیح)arg;
    توڑو;
  حالت CURLOPT_MAXFILESIZE:
    اگر(arg < 0)
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    ورنہ
      s->max_filesize = arg;
    توڑو;
  حالت CURLOPT_IPRESOLVE:
    اگر((arg < CURL_IPRESOLVE_WHATEVER) || (arg > CURL_IPRESOLVE_V6))
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    ورنہ
      s->ipver = (غیرعلامتی حرف)arg;
    توڑو;
  حالت CURLOPT_CONNECT_ONLY:
    اگر(arg < 0 || arg > 2)
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    ورنہ {
      s->connect_only = !!arg;
      s->connect_only_ws = (arg == 2);
    }
    توڑو;
#اگر_تعریف_شدہ USE_IPV6
  حالت CURLOPT_پتہ_SCOPE:
#اگر SIZEOF_LONG > 4
    اگر((غیرعلامتی طویل)arg > UINT_MAX)
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    ورنہ
#اگر_ختم
    s->scope_id = (غیرعلامتی صحیح)arg;
    توڑو;
#اگر_ختم
  حالت CURLOPT_TCP_KEEPIDLE:
    نتیجہ = قدر_range(&arg, 0, 0, زیادہ_سے_زیادہ_عدد);
    اگر(!نتیجہ)
      s->tcp_keepidle = (صحیح)arg;
    توڑو;
  حالت CURLOPT_TCP_KEEPINTVL:
    نتیجہ = قدر_range(&arg, 0, 0, زیادہ_سے_زیادہ_عدد);
    اگر(!نتیجہ)
      s->tcp_keepintvl = (صحیح)arg;
    توڑو;
  حالت CURLOPT_TCP_KEEPCNT:
    نتیجہ = قدر_range(&arg, 0, 0, زیادہ_سے_زیادہ_عدد);
    اگر(!نتیجہ)
      s->tcp_keepcnt = (صحیح)arg;
    توڑو;
  حالت CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
    واپس setopt_سیٹ_timeout_ms(&s->happy_eyeballs_timeout, arg);
  حالت CURLOPT_UPKEEP_INTERVAL_MS:
    واپس setopt_سیٹ_timeout_ms(&s->upkeep_interval_ms, arg);
  حالت CURLOPT_MAXAGE_CONN:
    واپس setopt_سیٹ_timeout_sec(&s->conn_max_idle_ms, arg);
  حالت CURLOPT_MAXLIFETIME_CONN:
    واپس setopt_سیٹ_timeout_sec(&s->conn_max_age_ms, arg);
  حالت CURLOPT_DNS_USE_GLOBAL_CACHE:
    /* deprecated */
    توڑو;
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }
  واپس نتیجہ;
}

ساکن CURLcode setopt_long_ssl(ساخت Curl_easy *ڈیٹا, CURLoption option,
                                طویل arg)
{
#اگر_تعریف_شدہ USE_SSL
  CURLcode نتیجہ = CURLE_OK;
  ساخت UserDefined *s = &ڈیٹا->سیٹ;
  انتخاب(option) {
  حالت CURLOPT_CA_CACHE_TIMEOUT:
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_CA_CACHE)) {
      نتیجہ = قدر_range(&arg, -1, -1, زیادہ_سے_زیادہ_عدد);
      اگر(!نتیجہ)
        s->ssl_ca_cache_timeout = (صحیح)arg;
    }
    ورنہ
      نتیجہ = CURLE_NOT_BUILT_اندر;
    توڑو;
  حالت CURLOPT_SSLVERSION:
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  حالت CURLOPT_PROXY_SSLVERSION:
#اگر_ختم
    واپس Curl_setopt_SSLVERSION(ڈیٹا, option, arg);
  حالت CURLOPT_SSL_FALSESTART:
    نتیجہ = CURLE_NOT_BUILT_اندر;
    توڑو;
  حالت CURLOPT_USE_SSL:
    اگر((arg < CURLUSESSL_NONE) || (arg >= CURLUSESSL_آخری))
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    ورنہ
      s->use_ssl = (غیرعلامتی حرف)arg;
    توڑو;
  حالت CURLOPT_SSL_OPTIONS:
    s->ssl.ssl_options = (غیرعلامتی حرف)(arg & 0xff);
    توڑو;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  حالت CURLOPT_PROXY_SSL_OPTIONS:
    s->proxy_ssl.ssl_options = (غیرعلامتی حرف)(arg & 0xff);
    توڑو;
#اگر_ختم
  حالت CURLOPT_SSL_ENABLE_NPN:
    توڑو;
  حالت CURLOPT_SSLENGINE_DEFAULT:
#اگر_تعریف_شدہ USE_OPENSSL
    اگر(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      واپس CURLE_NOT_BUILT_اندر;
    ڈیٹا->سیٹ.engine_default = (arg == 1);
#ورنہ
    واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
    توڑو;
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }
  واپس نتیجہ;
#ورنہ  /* USE_SSL */
  (خالی)ڈیٹا;
  (خالی)option;
  (خالی)arg;
  واپس CURLE_UNKNOWN_OPTION;
#اگر_ختم /* !USE_SSL */
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
ساکن خالی changeproxy(ساخت Curl_easy *ڈیٹا)
{
  Curl_auth_digest_cleanup(&ڈیٹا->state.proxydigest);
  memset(&ڈیٹا->state.authproxy, 0, حجم(ڈیٹا->state.authproxy));
}

ساکن CURLcode setopt_long_proxy(ساخت Curl_easy *ڈیٹا, CURLoption option,
                                  طویل arg)
{
  ساخت UserDefined *s = &ڈیٹا->سیٹ;

  انتخاب(option) {
  حالت CURLOPT_PROXYPORT:
    اگر((arg < 0) || (arg > UINT16_MAX))
      واپس CURLE_BAD_FUNCTION_دلیل;
    اگر(arg != s->proxyport)
      changeproxy(ڈیٹا);
    s->proxyport = (uint16_t)arg;
    توڑو;
  حالت CURLOPT_PROXYAUTH:
    واپس httpauth(ڈیٹا, TRUE, (غیرعلامتی طویل)arg);
  حالت CURLOPT_PROXYTYPE:
    اگر((arg < CURLPROXY_HTTP) || (arg > CURLPROXY_HTTPS3))
      واپس CURLE_BAD_FUNCTION_دلیل;
#اگر_غیر_تعریف_شدہ USE_PROXY_HTTP3
    اگر(arg == CURLPROXY_HTTPS3)
      واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
    s->proxytype = (غیرعلامتی حرف)arg;
    توڑو;
  حالت CURLOPT_SOCKS5_AUTH: {
    غیرعلامتی طویل auth = (غیرعلامتی طویل)arg;
    اگر(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI | CURLAUTH_ONLY))
      واپس CURLE_NOT_BUILT_اندر;
    اگر((auth & CURLAUTH_ONLY) && !(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI)))
      واپس CURLE_BAD_FUNCTION_دلیل;
    s->socks5auth = (غیرعلامتی حرف)(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI));
    s->socks5_auth_only = !!(auth & CURLAUTH_ONLY);
    توڑو;
  }
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }
  واپس CURLE_OK;
}
#ورنہ
ساکن CURLcode setopt_long_proxy(ساخت Curl_easy *ڈیٹا, CURLoption option,
                                  طویل arg)
{
  (خالی)ڈیٹا;
  (خالی)option;
  (خالی)arg;
  واپس CURLE_UNKNOWN_OPTION;
}
#اگر_ختم

ساکن CURLcode setopt_long_http(ساخت Curl_easy *ڈیٹا, CURLoption option,
                                 طویل arg)
{
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
  CURLcode نتیجہ = CURLE_OK;
  ساخت UserDefined *s = &ڈیٹا->سیٹ;

  انتخاب(option) {
  حالت CURLOPT_FOLLOWLOCATION:
    اگر((غیرعلامتی طویل)arg > 3)
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    ورنہ
      s->http_follow_منوال = (غیرعلامتی حرف)arg;
    توڑو;
  حالت CURLOPT_MAXREDIRS:
    نتیجہ = قدر_range(&arg, -1, -1, 0x7fff);
    اگر(!نتیجہ)
      s->maxredirs = (مختصر)arg;
    توڑو;
  حالت CURLOPT_POSTREDIR:
    اگر(arg < CURL_REDIR_GET_ALL)
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    ورنہ {
      s->post301 = !!(arg & CURL_REDIR_POST_301);
      s->post302 = !!(arg & CURL_REDIR_POST_302);
      s->post303 = !!(arg & CURL_REDIR_POST_303);
    }
    توڑو;
  حالت CURLOPT_HEADEROPT:
    s->sep_سرخیاں = !!(arg & CURLHEADER_SEPARATE);
    توڑو;
  حالت CURLOPT_HTTPAUTH:
    واپس httpauth(ڈیٹا, FALSE, (غیرعلامتی طویل)arg);
  حالت CURLOPT_HTTP_نسخہ:
    واپس setopt_HTTP_نسخہ(ڈیٹا, arg);
  حالت CURLOPT_EXPECT_100_TIMEOUT_MS:
    نتیجہ = قدر_range(&arg, 0, 0, 0xffff);
    اگر(!نتیجہ)
      s->expect_100_timeout = (غیرعلامتی مختصر)arg;
    توڑو;
  حالت CURLOPT_دھارا_وزن:
#اگر defined(USE_HTTP2) || defined(USE_HTTP3)
    اگر((arg >= 1) && (arg <= 256))
      s->وزن = (صحیح)arg;
    توڑو;
#ورنہ
    نتیجہ = CURLE_NOT_BUILT_اندر;
    توڑو;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTPSIG
  حالت CURLOPT_HTTPSIG_ALGORITHM:
    اگر(arg != CURLHTTPSIG_NONE &&
       arg != CURLHTTPSIG_ED25519 &&
       arg != CURLHTTPSIG_HMAC_SHA256)
      واپس CURLE_BAD_FUNCTION_دلیل;
    s->httpsig_algorithm = (uint8_t)arg;
    اگر(arg)
      s->httpauth = (uint32_t)CURLAUTH_HTTPSIG;
    ورنہ
      s->httpauth &= ~(uint32_t)CURLAUTH_HTTPSIG;
    توڑو;
#اگر_ختم
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }
  واپس نتیجہ;
#ورنہ
  (خالی)ڈیٹا;
  (خالی)option;
  (خالی)arg;
  واپس CURLE_UNKNOWN_OPTION;
#اگر_ختم
}

ساکن CURLcode setopt_long_proto(ساخت Curl_easy *ڈیٹا, CURLoption option,
                                  طویل arg)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت UserDefined *s = &ڈیٹا->سیٹ;

  انتخاب(option) {
#اگر_غیر_تعریف_شدہ CURL_DISABLE_TFTP
  حالت CURLOPT_TFTP_BLKSIZE:
    نتیجہ = قدر_range(&arg, 0, TFTP_BLKSIZE_MIN, TFTP_BLKSIZE_MAX);
    اگر(!نتیجہ)
      s->tftp_blksize = (غیرعلامتی مختصر)arg;
    توڑو;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_NETRC
  حالت CURLOPT_NETRC:
    اگر((arg < CURL_NETRC_IGNORED) || (arg >= CURL_NETRC_آخری))
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    ورنہ
      s->use_netrc = (غیرعلامتی حرف)arg;
    توڑو;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
  حالت CURLOPT_FTP_FILEMETHOD:
    اگر((arg < CURLFTPMETHOD_DEFAULT) || (arg >= CURLFTPMETHOD_آخری))
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    ورنہ
      s->ftp_filemethod = (غیرعلامتی حرف)arg;
    توڑو;
  حالت CURLOPT_FTP_SSL_CCC:
    اگر((arg < CURLFTPSSL_CCC_NONE) || (arg >= CURLFTPSSL_CCC_آخری))
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    ورنہ
      s->ftp_ccc = (غیرعلامتی حرف)arg;
    توڑو;
  حالت CURLOPT_FTPSSLAUTH:
    اگر((arg < CURLFTPAUTH_DEFAULT) || (arg >= CURLFTPAUTH_آخری))
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    ورنہ
      s->ftpsslauth = (غیرعلامتی حرف)arg;
    توڑو;
  حالت CURLOPT_ACCEPTTIMEOUT_MS:
    واپس setopt_سیٹ_timeout_ms(&s->accepttimeout, arg);
#اگر_ختم
#اگر !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  حالت CURLOPT_FTP_CREATE_MISSING_DIRS:
    اگر((arg < CURLFTP_CREATE_DIR_NONE) || (arg > CURLFTP_CREATE_DIR_RETRY))
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    ورنہ
      s->ftp_create_missing_dirs = (غیرعلامتی حرف)arg;
    توڑو;
  حالت CURLOPT_NEW_FILE_PERMS:
    اگر((arg < 0) || (arg > 0777))
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    ورنہ
      s->new_file_perms = (غیرعلامتی صحیح)arg;
    توڑو;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_RTSP
  حالت CURLOPT_RTSP_REQUEST:
    واپس setopt_RTSP_REQUEST(ڈیٹا, arg);
  حالت CURLOPT_RTSP_CLIENT_CSEQ:
    نتیجہ = قدر_range(&arg, 0, 0, زیادہ_سے_زیادہ_عدد);
    اگر(!نتیجہ)
      ڈیٹا->state.rtsp_اگلا_client_CSeq = (uint32_t)arg;
    توڑو;
  حالت CURLOPT_RTSP_SERVER_CSEQ:
    نتیجہ = قدر_range(&arg, 0, 0, زیادہ_سے_زیادہ_عدد);
    اگر(!نتیجہ)
      ڈیٹا->state.rtsp_اگلا_server_CSeq = (uint32_t)arg;
    توڑو;
#اگر_ختم
#اگر_تعریف_شدہ USE_SSH
  حالت CURLOPT_SSH_AUTH_TYPES:
    s->ssh_auth_types = (uint32_t)arg;
    توڑو;
  حالت CURLOPT_NEW_DIRECTORY_PERMS:
    اگر((arg < 0) || (arg > 0777))
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    ورنہ
      s->new_directory_perms = (غیرعلامتی صحیح)arg;
    توڑو;
#اگر_ختم
  حالت CURLOPT_PROTOCOLS:
    s->allowed_protocols = (curl_prot_t)arg;
    توڑو;
  حالت CURLOPT_REDIR_PROTOCOLS:
    s->redir_protocols = (curl_prot_t)arg;
    توڑو;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_WEBSOCKETS
  حالت CURLOPT_WS_OPTIONS:
    s->ws_raw_منوال = !!(arg & CURLWS_RAW_منوال);
    s->ws_no_auto_pong = !!(arg & CURLWS_NOAUTOPONG);
    توڑو;
#اگر_ختم
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }
  واپس نتیجہ;
}

ساکن CURLcode setopt_long_misc(ساخت Curl_easy *ڈیٹا, CURLoption option,
                                 طویل arg)
{
  ساخت UserDefined *s = &ڈیٹا->سیٹ;

  انتخاب(option) {
  حالت CURLOPT_TIMECONDITION:
    اگر((arg < CURL_TIMECOND_NONE) || (arg >= CURL_TIMECOND_آخری))
      واپس CURLE_BAD_FUNCTION_دلیل;
    s->timecondition = (غیرعلامتی حرف)arg;
    توڑو;
  حالت CURLOPT_TIMEVALUE:
    s->timevalue = (time_t)arg;
    توڑو;
  حالت CURLOPT_POSTFIELDSIZE:
    اگر(arg < -1)
      واپس CURLE_BAD_FUNCTION_دلیل;
    اگر(s->postfieldsize < arg && s->str_copypostfields) {
      curlx_safefree(s->str_copypostfields);
      s->postfields = تہی;
    }
    s->postfieldsize = arg;
    توڑو;
  حالت CURLOPT_INFILESIZE:
    اگر(arg < -1)
      واپس CURLE_BAD_FUNCTION_دلیل;
    s->filesize = arg;
    توڑو;
  حالت CURLOPT_RESUME_FROM:
    اگر(arg < -1)
      واپس CURLE_BAD_FUNCTION_دلیل;
    s->سیٹ_resume_from = arg;
    توڑو;
  حالت CURLOPT_UPLOAD_FLAGS:
    s->upload_flags = (غیرعلامتی حرف)arg;
    توڑو;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_MIME
  حالت CURLOPT_MIME_OPTIONS:
    s->mime_formescape = !!(arg & CURLMIMEOPT_FORMESCAPE);
    توڑو;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HSTS
  حالت CURLOPT_HSTS_CTRL:
    اگر(arg & CURLHSTS_ENABLE) {
      اگر(!ڈیٹا->hsts) {
        ڈیٹا->hsts = Curl_hsts_init();
        اگر(!ڈیٹا->hsts)
          واپس CURLE_برآمد_کا_حافظہ;
      }
    }
    ورنہ اگر(!ڈیٹا->share || !ڈیٹا->share->hsts) {
      /* throw away the HSTS cache unless shared */
      Curl_hsts_cleanup(&ڈیٹا->hsts);
      /* flush all the entries */
      curl_slist_free_all(ڈیٹا->state.hstslist);
      ڈیٹا->state.hstslist = تہی;
    }
    ورنہ
      /* detach from shared HSTS cache without freeing it */
      ڈیٹا->hsts = تہی;
    توڑو;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_ALTSVC
  حالت CURLOPT_ALTSVC_CTRL:
    واپس Curl_altsvc_ctrl(ڈیٹا, arg);
#اگر_ختم
#اگر defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  حالت CURLOPT_GSSAPI_DELEGATION:
    s->gssapi_delegation = (غیرعلامتی حرف)arg &
      (CURLGSSAPI_DELEGATION_POLICY_FLAG | CURLGSSAPI_DELEGATION_FLAG);
    توڑو;
#اگر_ختم
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }
  واپس CURLE_OK;
}

ساکن CURLcode setopt_long(ساخت Curl_easy *ڈیٹا, CURLoption option,
                            طویل arg)
{
  قسمتعریف CURLcode (*setoptfunc)(ساخت Curl_easy *ڈیٹا,
                                 CURLoption option, طویل arg);
  ساکن ثابت setoptfunc setopt_call[] = {
    setopt_long_bool,
    setopt_long_net,
    setopt_long_http,
    setopt_long_proxy,
    setopt_long_ssl,
    setopt_long_proto,
    setopt_long_misc
  };
  size_t i;

  برائے(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode نتیجہ = setopt_call[i](ڈیٹا, option, arg);
    اگر(نتیجہ != CURLE_UNKNOWN_OPTION)
      واپس نتیجہ;
  }
  واپس CURLE_UNKNOWN_OPTION;
}

ساکن CURLcode setopt_slist(ساخت Curl_easy *ڈیٹا, CURLoption option,
                             ساخت curl_slist *slist)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت UserDefined *s = &ڈیٹا->سیٹ;
  انتخاب(option) {
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  حالت CURLOPT_PROXYHEADER:
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
    توڑو;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
  حالت CURLOPT_HTTP200ALIASES:
    /*
     * Set a list of aliases for HTTP 200 in response header
     */
    s->http200aliases = slist;
    توڑو;
#اگر_ختم
#اگر !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  حالت CURLOPT_POSTQUOTE:
    /*
     * List of RAW FTP commands to use after a transfer
     */
    s->postquote = slist;
    توڑو;
  حالت CURLOPT_PREQUOTE:
    /*
     * List of RAW FTP commands to use prior to RETR (Wesley Laxton)
     */
    s->prequote = slist;
    توڑو;
  حالت CURLOPT_QUOTE:
    /*
     * List of RAW FTP commands to use before a transfer
     */
    s->quote = slist;
    توڑو;
#اگر_ختم
  حالت CURLOPT_RESOLVE:
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
    ڈیٹا->state.resolve = s->resolve;
    توڑو;
#اگر !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MIME)
  حالت CURLOPT_HTTPHEADER:
    /*
     * Set a list with HTTP headers to use (or replace internals with)
     */
    s->سرخیاں = slist;
    توڑو;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_TELNET
  حالت CURLOPT_TELNETOPTIONS:
    /*
     * Set a linked list of telnet options
     */
    s->telnet_options = slist;
    توڑو;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_SMTP
  حالت CURLOPT_MAIL_RCPT:
    /* Set the list of mail recipients */
    s->mail_rcpt = slist;
    توڑو;
#اگر_ختم
  حالت CURLOPT_CONNECT_مقصد:
    s->connect_مقصد = slist;
    توڑو;
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }
  واپس نتیجہ;
}

#اگر !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#اگر_غیر_تعریف_شدہ CURL_DISABLE_MIME
ساکن CURLcode setopt_mimepost(ساخت Curl_easy *ڈیٹا, curl_mime *mimep)
{
  /*
   * Set to make us do MIME POST
   */
  CURLcode نتیجہ;
  ساخت UserDefined *s = &ڈیٹا->سیٹ;
  اگر(!s->mimepostp) {
    s->mimepostp = curlx_malloc(حجم(*s->mimepostp));
    اگر(!s->mimepostp)
      واپس CURLE_برآمد_کا_حافظہ;
    Curl_mime_initpart(s->mimepostp);
  }

  نتیجہ = Curl_mime_سیٹ_subparts(s->mimepostp, mimep, FALSE);
  اگر(!نتیجہ) {
    s->طریقہ = HTTPREQ_POST_MIME;
    s->opt_no_body = FALSE; /* this is implied */
#اگر_غیر_تعریف_شدہ CURL_DISABLE_FORM_API
    Curl_mime_cleanpart(ڈیٹا->state.formp);
    curlx_safefree(ڈیٹا->state.formp);
    ڈیٹا->state.mimepost = تہی;
#اگر_ختم
  }
  واپس نتیجہ;
}
#اگر_ختم /* !CURL_DISABLE_MIME */
#اگر_ختم /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */

ساکن CURLcode setopt_share(ساخت Curl_easy *ڈیٹا, ساخت Curl_share *سیٹ)
{
  CURLcode نتیجہ;

  اگر(ڈیٹا->conn) {
    /* As this handle already has a connection attached, changing share now
       would be complicated and error-prone */
    infof(ڈیٹا, "Cannot change share object while in use");
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
  }
  ورنہ {
    /* disconnect from old share, if any and possible */
    نتیجہ = Curl_share_easy_unlink(ڈیٹا);
    اگر(!نتیجہ && GOOD_SHARE_HANDLE(سیٹ))
      /* use new share if it set */
      نتیجہ = Curl_share_easy_link(ڈیٹا, سیٹ);
  }
  واپس نتیجہ;
}

/* assorted pointer type arguments */
ساکن CURLcode setopt_اشارے(ساخت Curl_easy *ڈیٹا, CURLoption option,
                                va_list param)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت UserDefined *s = &ڈیٹا->سیٹ;
  انتخاب(option) {
  حالت CURLOPT_CURLU:
    /*
     * pass CURLU to set URL
     */
    Curl_bufref_free(&ڈیٹا->state.url);
    CURL_EASY_STR_CLEAR(ڈیٹا, STRING_سیٹ_URL);
    s->uh = va_arg(param, CURLU *);
    توڑو;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
#اگر_غیر_تعریف_شدہ CURL_DISABLE_FORM_API
  حالت CURLOPT_HTTPPOST:
    /*
     * Set to make us do HTTP POST. Legacy API-style.
     */
    s->httppost = va_arg(param, ساخت curl_httppost *);
    s->طریقہ = HTTPREQ_POST_FORM;
    s->opt_no_body = FALSE; /* this is implied */
    Curl_mime_cleanpart(ڈیٹا->state.formp);
    curlx_safefree(ڈیٹا->state.formp);
    ڈیٹا->state.mimepost = تہی;
    توڑو;
#اگر_ختم /* !CURL_DISABLE_FORM_API */
#اگر_ختم /* !CURL_DISABLE_HTTP */
#اگر !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#اگر_غیر_تعریف_شدہ CURL_DISABLE_MIME
  حالت CURLOPT_MIMEPOST:
    نتیجہ = setopt_mimepost(ڈیٹا, va_arg(param, curl_mime *));
    توڑو;
#اگر_ختم /* !CURL_DISABLE_MIME */
#اگر_ختم /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */
  حالت CURLOPT_STDERR:
    /*
     * Set to a FILE * that should receive all error writes. This
     * defaults to stderr for normal operations.
     */
    s->err = va_arg(param, FILE *);
    اگر(!s->err)
      s->err = stderr;
    توڑو;
  حالت CURLOPT_SHARE:
    واپس setopt_share(ڈیٹا, va_arg(param, ساخت Curl_share *));

  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }
  واپس نتیجہ;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_COOKIES
ساکن CURLcode cookielist(ساخت Curl_easy *ڈیٹا, ثابت حرف *ptr)
{
  CURLcode نتیجہ = CURLE_OK;
  اگر(!ptr)
    واپس CURLE_OK;

  اگر(curl_strequal(ptr, "ALL")) {
    /* clear all cookies */
    Curl_share_تالا(ڈیٹا, CURL_تالا_ڈیٹا_COOKIE, CURL_تالا_ACCESS_SINGLE);
    Curl_cookie_clearall(ڈیٹا->cookies);
    Curl_share_unlock(ڈیٹا, CURL_تالا_ڈیٹا_COOKIE);
  }
  ورنہ اگر(curl_strequal(ptr, "SESS")) {
    /* clear session cookies */
    Curl_share_تالا(ڈیٹا, CURL_تالا_ڈیٹا_COOKIE, CURL_تالا_ACCESS_SINGLE);
    Curl_cookie_clearsess(ڈیٹا->cookies);
    Curl_share_unlock(ڈیٹا, CURL_تالا_ڈیٹا_COOKIE);
  }
  ورنہ اگر(curl_strequal(ptr, "FLUSH")) {
    /* flush cookies to file, takes care of the locking */
    Curl_flush_cookies(ڈیٹا, FALSE);
  }
  ورنہ اگر(curl_strequal(ptr, "RELOAD")) {
    /* reload cookies from file */
    واپس Curl_cookie_loadfiles(ڈیٹا, COOKIE_NOPSL |
                                 (ڈیٹا->سیٹ.cookiesession ?
                                  COOKIE_NOSESSION : 0));
  }
  ورنہ {
    اگر(!ڈیٹا->cookies) {
      /* if cookie engine was not running, activate it */
      ڈیٹا->cookies = Curl_cookie_init();
      اگر(!ڈیٹا->cookies)
        واپس CURLE_برآمد_کا_حافظہ;
      ڈیٹا->state.cookie_engine = TRUE;
    }

    /* general protection against mistakes and abuse */
    اگر(لڑی_لمبائی(ptr) > CURL_MAX_داخلہ_لمبائی)
      واپس CURLE_BAD_FUNCTION_دلیل;

    /* Adding these cookies without the PSL check, because the PSL is not
       initialized until *perform() time, and this might be called before
       that */
    Curl_share_تالا(ڈیٹا, CURL_تالا_ڈیٹا_COOKIE, CURL_تالا_ACCESS_SINGLE);
    اگر(checkprefix("Set-Cookie:", ptr))
      /* HTTP Header format line */
      نتیجہ = Curl_cookie_جوڑو(ڈیٹا, ڈیٹا->cookies, ptr + 11,
                               تہی, تہی,
                               COOKIE_HTTPHEADER | COOKIE_SECURE |
                               COOKIE_NOPSL);
    ورنہ
      /* Netscape format line */
      نتیجہ = Curl_cookie_جوڑو(ڈیٹا, ڈیٹا->cookies, ptr, تہی,
                               تہی, COOKIE_SECURE | COOKIE_NOPSL);
    Curl_share_unlock(ڈیٹا, CURL_تالا_ڈیٹا_COOKIE);
  }
  واپس نتیجہ;
}

ساکن CURLcode cookiefile(ساخت Curl_easy *ڈیٹا, ثابت حرف *ptr)
{
  /*
   * Set cookie file to read and parse. Can be used multiple times.
   */
  اگر(ptr) {
    ساخت curl_slist *cl;
    /* general protection against mistakes and abuse */
    اگر(لڑی_لمبائی(ptr) > CURL_MAX_داخلہ_لمبائی)
      واپس CURLE_BAD_FUNCTION_دلیل;
    /* append the cookie filename to the list of filenames, and deal with
       them later */
    cl = curl_slist_append(ڈیٹا->state.cookielist, ptr);
    اگر(!cl) {
      curl_slist_free_all(ڈیٹا->state.cookielist);
      ڈیٹا->state.cookielist = تہی;
      واپس CURLE_برآمد_کا_حافظہ;
    }
    ڈیٹا->state.cookielist = cl; /* store the list for later use */
  }
  ورنہ {
    /* clear the list of cookie files */
    curl_slist_free_all(ڈیٹا->state.cookielist);
    ڈیٹا->state.cookielist = تہی;

    اگر(!ڈیٹا->share || !ڈیٹا->share->cookies) {
      /* throw away all existing cookies if this is not a shared cookie
         container */
      Curl_cookie_clearall(ڈیٹا->cookies);
      Curl_cookie_cleanup(ڈیٹا->cookies);
    }
    /* disable the cookie engine */
    ڈیٹا->cookies = تہی;
  }
  واپس CURLE_OK;
}
#اگر_ختم

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
ساکن CURLcode setproxy(ساخت Curl_easy *ڈیٹا, ثابت حرف *proxy)
{
  ثابت حرف *str = CURL_EASY_STR(ڈیٹا, STRING_PROXY);
  اگر(str && proxy &&
     /* there was one set, is this a new one? */
     !strcmp(str, proxy))
    واپس CURLE_OK; /* same one as before */

  changeproxy(ڈیٹا);
  واپس Curl_setstropt(ڈیٹا, STRING_PROXY, proxy);
}

ساکن CURLcode setopt_cptr_proxy(ساخت Curl_easy *ڈیٹا, CURLoption option,
                                  حرف *ptr)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت UserDefined *s = &ڈیٹا->سیٹ;
  انتخاب(option) {
  حالت CURLOPT_PROXYUSERPWD: {
    /*
     * user:password needed to use the proxy
     */
    حرف *u = تہی;
    حرف *p = تہی;
    نتیجہ = setstropt_userpwd(ptr, &u, &p);

    /* URL decode the components */
    اگر(!نتیجہ) {
      حرف *str = تہی;
      CURL_EASY_STR_CLEAR(ڈیٹا, STRING_PROXYUSERNAME);
      CURL_EASY_STR_CLEAR(ڈیٹا, STRING_PROXYPASSWORD);
      اگر(u) {
        نتیجہ = Curl_urldecode(u, 0, &str, تہی, REJECT_ZERO);
        اگر(!نتیجہ)
          نتیجہ = Curl_u8_strset_setn(&s->لڑیاں, STRING_PROXYUSERNAME, str);
      }
      اگر(!نتیجہ && p) {
        str = تہی;
        نتیجہ = Curl_urldecode(p, 0, &str, تہی, REJECT_ZERO);
        اگر(!نتیجہ)
          نتیجہ = Curl_u8_strset_setn(&s->لڑیاں, STRING_PROXYPASSWORD, str);
      }
    }
    curlx_free(u);
    curlx_strzero(p);
    curlx_free(p);
    توڑو;
  }
  حالت CURLOPT_PROXYUSERNAME:
    /*
     * authentication username to use in the operation
     */
    واپس Curl_setstropt(ڈیٹا, STRING_PROXYUSERNAME, ptr);

  حالت CURLOPT_PROXYPASSWORD:
    /*
     * authentication password to use in the operation
     */
    واپس Curl_setstropt(ڈیٹا, STRING_PROXYPASSWORD, ptr);

  حالت CURLOPT_NOPROXY:
    /*
     * proxy exception list
     */
    واپس Curl_setstropt(ڈیٹا, STRING_NOPROXY, ptr);
  حالت CURLOPT_PROXY_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use for proxy
     */
    واپس Curl_setstropt(ڈیٹا, STRING_CERT_PROXY, ptr);
  حالت CURLOPT_PROXY_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use for proxy
     */
    واپس Curl_setstropt(ڈیٹا, STRING_CERT_TYPE_PROXY, ptr);
  حالت CURLOPT_PROXY_SSLKEY:
    /*
     * String that holds filename of the SSL key to use for proxy
     */
    واپس Curl_setstropt(ڈیٹا, STRING_کلید_PROXY, ptr);
  حالت CURLOPT_PROXY_KEYPASSWD:
    /*
     * String that holds the SSL private key password for proxy.
     */
    واپس Curl_setstropt(ڈیٹا, STRING_کلید_PASSWD_PROXY, ptr);
  حالت CURLOPT_PROXY_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use for proxy
     */
    واپس Curl_setstropt(ڈیٹا, STRING_کلید_TYPE_PROXY, ptr);
  حالت CURLOPT_PROXY_SSL_CIPHER_LIST:
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_CIPHER_LIST)) {
      /* set a list of cipher we want to use in the SSL connection for proxy */
      واپس Curl_setstropt(ڈیٹا, STRING_SSL_CIPHER_LIST_PROXY, ptr);
    }
    ورنہ
      واپس CURLE_NOT_BUILT_اندر;
  حالت CURLOPT_PROXY_TLS13_CIPHERS:
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites for proxy */
      واپس Curl_setstropt(ڈیٹا, STRING_SSL_CIPHER13_LIST_PROXY, ptr);
    ورنہ
      واپس CURLE_NOT_BUILT_اندر;
  حالت CURLOPT_PROXY:
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
    واپس setproxy(ڈیٹا, ptr);
  حالت CURLOPT_PRE_PROXY:
    /*
     * Set proxy server:port to use as SOCKS proxy.
     *
     * If the proxy is set to "" or NULL we explicitly say that we do not want
     * to use the socks proxy.
     */
    واپس Curl_setstropt(ڈیٹا, STRING_PRE_PROXY, ptr);
  حالت CURLOPT_SOCKS5_GSSAPI_SERVICE:
  حالت CURLOPT_PROXY_SERVICE_NAME:
    /*
     * Set proxy authentication service name for Kerberos 5 and SPNEGO
     */
    واپس Curl_setstropt(ڈیٹا, STRING_PROXY_SERVICE_NAME, ptr);
  حالت CURLOPT_PROXY_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
#اگر_تعریف_شدہ USE_SSL
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_PINNEDPUBKEY))
      واپس Curl_setstropt(ڈیٹا, STRING_SSL_PINNEDPUBLICKEY_PROXY, ptr);
#اگر_ختم
    واپس CURLE_NOT_BUILT_اندر;

  حالت CURLOPT_HAPROXY_CLIENT_IP:
    /*
     * Set the client IP to send through HAProxy PROXY protocol
     */
    نتیجہ = Curl_setstropt(ڈیٹا, STRING_HAPROXY_CLIENT_IP, ptr);

    /* enable the HAProxy protocol if an IP is provided */
    s->haproxyprotocol = !!CURL_EASY_STR(ڈیٹا, STRING_HAPROXY_CLIENT_IP);
    توڑو;
  حالت CURLOPT_PROXY_CAINFO:
    /*
     * Set CA info SSL connection for proxy. Specify filename of the
     * CA certificate
     */
    نتیجہ = Curl_setstropt(ڈیٹا, STRING_SSL_CAFILE_PROXY, ptr);
    اگر(!نتیجہ)
      s->proxy_ssl.custom_cafile = TRUE;
    واپس نتیجہ;
  حالت CURLOPT_PROXY_CRLFILE:
    /*
     * Set CRL file info for SSL connection for proxy. Specify filename of the
     * CRL to check certificates revocation
     */
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_CRLFILE))
      واپس Curl_setstropt(ڈیٹا, STRING_SSL_CRLFILE_PROXY, ptr);
    واپس CURLE_NOT_BUILT_اندر;
  حالت CURLOPT_PROXY_ISSUERCERT:
    /*
     * Set Issuer certificate file to check certificates issuer
     */
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_ISSUERCERT))
      واپس Curl_setstropt(ڈیٹا, STRING_SSL_ISSUERCERT_PROXY, ptr);
    واپس CURLE_NOT_BUILT_اندر;
  حالت CURLOPT_PROXY_CAPATH:
    /*
     * Set CA path info for SSL connection proxy. Specify directory name of the
     * CA certificates which have been prepared using openssl c_rehash utility.
     */
#اگر_تعریف_شدہ USE_SSL
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_CA_راستہ)) {
      /* This does not work on Windows. */
      نتیجہ = Curl_setstropt(ڈیٹا, STRING_SSL_CAPATH_PROXY, ptr);
      اگر(!نتیجہ)
        s->proxy_ssl.custom_capath = TRUE;
      واپس نتیجہ;
    }
#اگر_ختم
    واپس CURLE_NOT_BUILT_اندر;
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }
  واپس نتیجہ;
}
#اگر_ختم

#اگر !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
/*
 * A string with POST data. Makes curl HTTP POST. Even if it is NULL. If
 * needed, CURLOPT_POSTFIELDSIZE must have been set prior to
 * CURLOPT_COPYPOSTFIELDS and not altered later.
 */
ساکن CURLcode setopt_copypostfields(ثابت حرف *ptr, ساخت UserDefined *s)
{
  CURLcode نتیجہ = CURLE_OK;
  اگر(s->postfieldsize < -1)
    واپس CURLE_BAD_FUNCTION_دلیل;
  اگر(!ptr || s->postfieldsize == -1) {
    اگر(ptr && (لڑی_لمبائی(ptr) > CURL_MAX_داخلہ_لمبائی))
      واپس CURLE_BAD_FUNCTION_دلیل;
    curlx_safefree(s->str_copypostfields);
    اگر(ptr) {
      s->str_copypostfields = curlx_strdup(ptr);
      اگر(!s->str_copypostfields)
        واپس CURLE_برآمد_کا_حافظہ;
    }
  }
  ورنہ {
    size_t pflen = curlx_sotouz_range(s->postfieldsize, 0, SIZE_MAX);
    اگر(pflen == SIZE_MAX)
      واپس CURLE_برآمد_کا_حافظہ;
    ورنہ {
      /* Allocate even when size == 0. This satisfies the need of possible
         later address compare to detect the COPYPOSTFIELDS mode, and to mark
         that postfields is used rather than read function or form data. */
      حرف *p = curlx_memdup0(ptr, pflen);
      اگر(!p)
        واپس CURLE_برآمد_کا_حافظہ;
      ورنہ {
        curlx_free(s->str_copypostfields);
        s->str_copypostfields = p;
      }
    }
  }

  s->postfields = s->str_copypostfields;
  s->طریقہ = HTTPREQ_POST;
  واپس نتیجہ;
}
#اگر_ختم

#اگر_تعریف_شدہ USE_ECH
ساکن CURLcode setopt_ech(ساخت Curl_easy *ڈیٹا, ثابت حرف *ptr)
{
  ساخت UserDefined *s = &ڈیٹا->سیٹ;
  CURLcode نتیجہ = CURLE_OK;

  اگر(!ptr || !strcmp(ptr, "false"))
    s->ssl.ech = CURLECH_DISABLE;
  ورنہ {
    size_t plen = لڑی_لمبائی(ptr);
    اگر(plen > CURL_MAX_داخلہ_لمبائی)
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    ورنہ {
      اگر(!strcmp(ptr, "grease"))
        s->ssl.ech = CURLECH_GREASE;
      ورنہ اگر(!strcmp(ptr, "true"))
        s->ssl.ech = CURLECH_ENABLE;
      ورنہ اگر(!strcmp(ptr, "hard"))
        s->ssl.ech = CURLECH_HARD;
      ورنہ اگر(plen > 4 && !لڑی_موازنہ(ptr, "ecl:", 4)) {
        اگر(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        نتیجہ = Curl_setstropt(ڈیٹا, STRING_ECH_CONFIG, ptr + 4);
      }
      ورنہ اگر(plen > 3 && !لڑی_موازنہ(ptr, "pn:", 3)) {
        اگر(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        نتیجہ = Curl_setstropt(ڈیٹا, STRING_ECH_عوامی, ptr + 3);
      }
      ورنہ
        نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    }
  }
  واپس نتیجہ;
}
#ورنہ
#تعریف_کرو setopt_ech(x, y) CURLE_NOT_BUILT_اندر
#اگر_ختم

#اگر defined(USE_SSL) || defined(USE_SSH)
/* One of the options is used for both TLS and SSH */
ساکن CURLcode setopt_cptr_ssl(ساخت Curl_easy *ڈیٹا, CURLoption option,
                                حرف *ptr)
{
  CURLcode نتیجہ = CURLE_OK;

  انتخاب(option) {
  حالت CURLOPT_KEYPASSWD:
    /*
     * String that holds the SSL or SSH private key password.
     */
    نتیجہ = Curl_setstropt(ڈیٹا, STRING_کلید_PASSWD, ptr);
    توڑو;
#اگر_تعریف_شدہ USE_SSL
  حالت CURLOPT_CAINFO:
    /*
     * Set CA info for SSL connection. Specify filename of the CA certificate
     */
    نتیجہ = Curl_setstropt(ڈیٹا, STRING_SSL_CAFILE, ptr);
    اگر(!نتیجہ)
      ڈیٹا->سیٹ.ssl.custom_cafile = TRUE;
    واپس نتیجہ;
  حالت CURLOPT_CAPATH:
    /*
     * Set CA path info for SSL connection. Specify directory name of the CA
     * certificates which have been prepared using openssl c_rehash utility.
     */
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_CA_راستہ)) {
      /* This does not work on Windows. */
      نتیجہ = Curl_setstropt(ڈیٹا, STRING_SSL_CAPATH, ptr);
      اگر(!نتیجہ)
        ڈیٹا->سیٹ.ssl.custom_capath = TRUE;
      واپس نتیجہ;
    }
    واپس CURLE_NOT_BUILT_اندر;
  حالت CURLOPT_CRLFILE:
    /*
     * Set CRL file info for SSL connection. Specify filename of the CRL
     * to check certificates revocation
     */
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_CRLFILE))
      واپس Curl_setstropt(ڈیٹا, STRING_SSL_CRLFILE, ptr);
    واپس CURLE_NOT_BUILT_اندر;
  حالت CURLOPT_SSL_CIPHER_LIST:
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_CIPHER_LIST))
      /* set a list of cipher we want to use in the SSL connection */
      واپس Curl_setstropt(ڈیٹا, STRING_SSL_CIPHER_LIST, ptr);
    ورنہ
      واپس CURLE_NOT_BUILT_اندر;
  حالت CURLOPT_TLS13_CIPHERS:
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites */
      واپس Curl_setstropt(ڈیٹا, STRING_SSL_CIPHER13_LIST, ptr);
    ورنہ
      واپس CURLE_NOT_BUILT_اندر;
  حالت CURLOPT_RANDOM_FILE:
    توڑو;
  حالت CURLOPT_EGDSOCKET:
    توڑو;
  حالت CURLOPT_SSL_CTX_ڈیٹا:
    /*
     * Set an SSL_CTX callback parameter pointer
     */
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_SSL_CTX)) {
      ڈیٹا->سیٹ.ssl_fsslctxp = ptr;
      توڑو;
    }
    ورنہ
      واپس CURLE_NOT_BUILT_اندر;
  حالت CURLOPT_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use
     */
    واپس Curl_setstropt(ڈیٹا, STRING_CERT, ptr);
  حالت CURLOPT_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use
     */
    واپس Curl_setstropt(ڈیٹا, STRING_CERT_TYPE, ptr);
  حالت CURLOPT_SSLKEY:
    /*
     * String that holds filename of the SSL key to use
     */
    واپس Curl_setstropt(ڈیٹا, STRING_کلید, ptr);
  حالت CURLOPT_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use
     */
    واپس Curl_setstropt(ڈیٹا, STRING_کلید_TYPE, ptr);
  حالت CURLOPT_SSLENGINE:
    /*
     * String that holds the SSL crypto engine.
     */
    اگر(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      واپس CURLE_NOT_BUILT_اندر;
    نتیجہ = Curl_setstropt(ڈیٹا, STRING_SSL_ENGINE, ptr);
    توڑو;
  حالت CURLOPT_ISSUERCERT:
    /*
     * Set Issuer certificate file
     * to check certificates issuer
     */
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_ISSUERCERT))
      واپس Curl_setstropt(ڈیٹا, STRING_SSL_ISSUERCERT, ptr);
    واپس CURLE_NOT_BUILT_اندر;
  حالت CURLOPT_SSL_EC_CURVES:
    /*
     * Set accepted curves in SSL connection setup.
     * Specify colon-delimited list of curve algorithm names.
     */
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_SSL_EC_CURVES))
      واپس Curl_setstropt(ڈیٹا, STRING_SSL_EC_CURVES, ptr);
    واپس CURLE_NOT_BUILT_اندر;
  حالت CURLOPT_SSL_SIGNATURE_ALGORITHMS:
    /*
     * Set accepted signature algorithms.
     * Specify colon-delimited list of signature scheme names.
     */
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_SIGNATURE_ALGORITHMS))
      واپس Curl_setstropt(ڈیٹا, STRING_SSL_SIGNATURE_ALGORITHMS, ptr);
    واپس CURLE_NOT_BUILT_اندر;
  حالت CURLOPT_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_PINNEDPUBKEY))
      واپس Curl_setstropt(ڈیٹا, STRING_SSL_PINNEDPUBLICKEY, ptr);
    واپس CURLE_NOT_BUILT_اندر;
  حالت CURLOPT_ECH:
    واپس setopt_ech(ڈیٹا, ptr);
#اگر_ختم
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }
  واپس نتیجہ;
}
#اگر_ختم

#اگر !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
ساکن CURLcode setopt_cptr_http_mqtt(ساخت Curl_easy *ڈیٹا,
                                      CURLoption option, حرف *ptr)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت UserDefined *s = &ڈیٹا->سیٹ;

  انتخاب(option) {
  حالت CURLOPT_COPYPOSTFIELDS:
    واپس setopt_copypostfields(ptr, s);

  حالت CURLOPT_POSTFIELDS:
    /*
     * Like above, but use static data instead of copying it.
     */
    s->postfields = ptr;
    /* Release old copied data. */
    curlx_safefree(s->str_copypostfields);
    s->طریقہ = HTTPREQ_POST;
    توڑو;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
  حالت CURLOPT_TRAILERDATA:
    s->trailer_ڈیٹا = ptr;
    توڑو;
  حالت CURLOPT_ACCEPT_ENCODING:
    /*
     * String to use at the value of Accept-Encoding header.
     *
     * If the encoding is set to "" we use an Accept-Encoding header that
     * encompasses all the encodings we support.
     * If the encoding is set to NULL we do not send an Accept-Encoding header
     * and ignore any received Content-Encoding header.
     *
     */
    اگر(ptr && !*ptr) {
      ptr = Curl_get_content_encodings();
      اگر(ptr)
        نتیجہ = CURL_EASY_STR_SETN(ڈیٹا, STRING_ENCODING, ptr);
      ورنہ
        نتیجہ = CURLE_برآمد_کا_حافظہ;
      واپس نتیجہ;
    }
    واپس Curl_setstropt(ڈیٹا, STRING_ENCODING, ptr);

#اگر_غیر_تعریف_شدہ CURL_DISABLE_AWS
  حالت CURLOPT_AWS_SIGV4:
    /*
     * String that is merged to some authentication
     * parameters are used by the algorithm.
     */
    نتیجہ = Curl_setstropt(ڈیٹا, STRING_AWS_SIGV4, ptr);
    /*
     * Basic has been set by default; it needs to be unset here.
     */
    اگر(CURL_EASY_STR(ڈیٹا, STRING_AWS_SIGV4))
      s->httpauth = CURLAUTH_AWS_SIGV4;
    ورنہ
      s->httpauth &= ~(uint32_t)CURLAUTH_AWS_SIGV4;
    توڑو;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTPSIG
  حالت CURLOPT_HTTPSIG_کلید:
    نتیجہ = Curl_setstropt(ڈیٹا, STRING_HTTPSIG_کلید, ptr);
    توڑو;
  حالت CURLOPT_HTTPSIG_KEYID:
    نتیجہ = Curl_setstropt(ڈیٹا, STRING_HTTPSIG_KEYID, ptr);
    توڑو;
  حالت CURLOPT_HTTPSIG_سرخیاں:
    نتیجہ = Curl_setstropt(ڈیٹا, STRING_HTTPSIG_سرخیاں, ptr);
    توڑو;
#اگر_ختم
  حالت CURLOPT_REFERER: {
    /*
     * String to set in the HTTP Referer: field.
     */
    ساخت bufref *oldref = &ڈیٹا->state.referer;
    /* free the old after the storing the new in case the input is actually
       pointing back to this */
    نتیجہ = Curl_setstropt(ڈیٹا, STRING_سیٹ_REFERER, ptr);
    Curl_bufref_free(oldref);
    توڑو;
  }

  حالت CURLOPT_USERAGENT:
    /*
     * String to use in the HTTP User-Agent field
     */
    واپس Curl_setstropt(ڈیٹا, STRING_USERAGENT, ptr);

#اگر_غیر_تعریف_شدہ CURL_DISABLE_COOKIES
  حالت CURLOPT_COOKIE:
    /*
     * Cookie string to send to the remote server in the request.
     */
    واپس Curl_setstropt(ڈیٹا, STRING_COOKIE, ptr);

  حالت CURLOPT_COOKIEFILE:
    واپس cookiefile(ڈیٹا, ptr);

  حالت CURLOPT_COOKIEJAR:
    /*
     * Set cookie filename to dump all cookies to when we are done.
     */
    نتیجہ = Curl_setstropt(ڈیٹا, STRING_COOKIEJAR, ptr);
    اگر(!نتیجہ) {
      /*
       * Activate the cookie parser. This may or may not already
       * have been made.
       */
      اگر(!ڈیٹا->cookies)
        ڈیٹا->cookies = Curl_cookie_init();
      اگر(!ڈیٹا->cookies)
        نتیجہ = CURLE_برآمد_کا_حافظہ;
      ورنہ
        ڈیٹا->state.cookie_engine = TRUE;
    }
    توڑو;

  حالت CURLOPT_COOKIELIST:
    واپس cookielist(ڈیٹا, ptr);
#اگر_ختم /* !CURL_DISABLE_COOKIES */

#اگر_ختم /* !CURL_DISABLE_HTTP */
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }
  واپس نتیجہ;
}
#اگر_ختم /* !CURL_DISABLE_HTTP || !CURL_DISABLE_MQTT */

#اگر_تعریف_شدہ USE_SSH
ساکن CURLcode setopt_cptr_ssh(ساخت Curl_easy *ڈیٹا, CURLoption option,
                                حرف *ptr)
{
  ساخت UserDefined *s = &ڈیٹا->سیٹ;
  انتخاب(option) {
  حالت CURLOPT_SSH_عوامی_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa.pub file
     */
    واپس Curl_setstropt(ڈیٹا, STRING_SSH_عوامی_کلید, ptr);
  حالت CURLOPT_SSH_نجی_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa file
     */
    واپس Curl_setstropt(ڈیٹا, STRING_SSH_نجی_کلید, ptr);
  حالت CURLOPT_SSH_KEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_keyfunc_userp = ptr;
    توڑو;
  حالت CURLOPT_SSH_HOST_عوامی_کلید_MD5:
    /*
     * Option to allow for the MD5 of the host public key to be checked
     * for validation purposes.
     */
    واپس Curl_setstropt(ڈیٹا, STRING_SSH_HOST_عوامی_کلید_MD5, ptr);
  حالت CURLOPT_SSH_HOST_عوامی_کلید_SHA256:
    /*
     * Option to allow for the SHA256 of the host public key to be checked
     * for validation purposes.
     */
    واپس Curl_setstropt(ڈیٹا, STRING_SSH_HOST_عوامی_کلید_SHA256, ptr);
  حالت CURLOPT_SSH_KNOWNHOSTS:
    /*
     * Store the filename to read known hosts from.
     */
    واپس Curl_setstropt(ڈیٹا, STRING_SSH_KNOWNHOSTS, ptr);
#اگر_تعریف_شدہ USE_LIBSSH2
  حالت CURLOPT_SSH_HOSTKEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_hostkeyfunc_userp = ptr;
    توڑو;
#اگر_ختم /* USE_LIBSSH2 */
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }
  واپس CURLE_OK;
}
#اگر_ختم /* USE_SSH */

#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
ساکن CURLcode setopt_cptr_ftp(ساخت Curl_easy *ڈیٹا, CURLoption option,
                                حرف *ptr)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت UserDefined *s = &ڈیٹا->سیٹ;
  انتخاب(option) {
  حالت CURLOPT_FTPPORT:
    /*
     * Use FTP PORT, this also specifies which IP address to use
     */
    نتیجہ = Curl_setstropt(ڈیٹا, STRING_FTPPORT, ptr);
    s->ftp_use_پورٹ = !!CURL_EASY_STR(ڈیٹا, STRING_FTPPORT);
    توڑو;

  حالت CURLOPT_FTP_ACCOUNT:
    واپس Curl_setstropt(ڈیٹا, STRING_FTP_ACCOUNT, ptr);

  حالت CURLOPT_FTP_ALTERNATIVE_مقصد_USER:
    واپس Curl_setstropt(ڈیٹا, STRING_FTP_ALTERNATIVE_مقصد_USER, ptr);

  حالت CURLOPT_KRBLEVEL:
    واپس CURLE_NOT_BUILT_اندر; /* removed in 8.17.0 */
  حالت CURLOPT_CHUNK_ڈیٹا:
    s->wildcardptr = ptr;
    توڑو;
  حالت CURLOPT_FNMATCH_ڈیٹا:
    s->fnmatch_ڈیٹا = ptr;
    توڑو;
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }
  واپس نتیجہ;
}
#اگر_ختم /* !CURL_DISABLE_FTP */

ساکن CURLcode setopt_cptr_net(ساخت Curl_easy *ڈیٹا, CURLoption option,
                                حرف *ptr)
{
  انتخاب(option) {
  حالت CURLOPT_INTERFACE:
    /*
     * Set what interface or address/hostname to bind the socket to when
     * performing an operation and thus what from-IP your connection will use.
     */
    واپس setstropt_interface(ڈیٹا, ptr);

#اگر_تعریف_شدہ USE_RESOLV_ARES
  حالت CURLOPT_DNS_SERVERS:
    واپس Curl_setstropt(ڈیٹا, STRING_DNS_SERVERS, ptr);

  حالت CURLOPT_DNS_INTERFACE:
    واپس Curl_setstropt(ڈیٹا, STRING_DNS_INTERFACE, ptr);

  حالت CURLOPT_DNS_LOCAL_IP4:
    واپس Curl_setstropt(ڈیٹا, STRING_DNS_LOCAL_IP4, ptr);

  حالت CURLOPT_DNS_LOCAL_IP6:
    واپس Curl_setstropt(ڈیٹا, STRING_DNS_LOCAL_IP6, ptr);
#اگر_ختم
#اگر_تعریف_شدہ USE_UNIX_SOCKETS
  حالت CURLOPT_UNIX_SOCKET_راستہ:
    ڈیٹا->سیٹ.abstract_unix_socket = FALSE;
    واپس Curl_setstropt(ڈیٹا, STRING_UNIX_SOCKET_راستہ, ptr);

  حالت CURLOPT_ABSTRACT_UNIX_SOCKET:
    ڈیٹا->سیٹ.abstract_unix_socket = TRUE;
    واپس Curl_setstropt(ڈیٹا, STRING_UNIX_SOCKET_راستہ, ptr);
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DOH
  حالت CURLOPT_DOH_URL:
    {
      CURLcode نتیجہ = Curl_setstropt(ڈیٹا, STRING_DOH, ptr);
      ڈیٹا->سیٹ.doh = !!CURL_EASY_STR(ڈیٹا, STRING_DOH);
      واپس نتیجہ;
    }
#اگر_ختم
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }
}

ساکن CURLcode setopt_cptr_misc(ساخت Curl_easy *ڈیٹا, CURLoption option,
                                 حرف *ptr)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت UserDefined *s = &ڈیٹا->سیٹ;

  انتخاب(option) {
  حالت CURLOPT_REQUEST_ہدف:
    واپس Curl_setstropt(ڈیٹا, STRING_ہدف, ptr);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_NETRC
  حالت CURLOPT_NETRC_FILE:
    واپس Curl_setstropt(ڈیٹا, STRING_NETRC_FILE, ptr);
#اگر_ختم
  حالت CURLOPT_CUSTOMREQUEST:
    واپس Curl_setstropt(ڈیٹا, STRING_CUSTOMREQUEST, ptr);

    /* we do not set s->method = HTTPREQ_CUSTOM; here, we continue as if we
       were using the already set type and this changes the actual request
       keyword */
  حالت CURLOPT_SERVICE_NAME:
    واپس Curl_setstropt(ڈیٹا, STRING_SERVICE_NAME, ptr);

  حالت CURLOPT_HEADERDATA:
    s->writeheader = ptr;
    توڑو;
  حالت CURLOPT_READDATA:
    s->اندر_سیٹ = ptr;
    توڑو;
  حالت CURLOPT_WRITEDATA:
    s->برآمد = ptr;
    توڑو;
  حالت CURLOPT_DEBUGDATA:
    s->debugdata = ptr;
    توڑو;
  حالت CURLOPT_PROGRESSDATA:
    s->پیش_رفت_client = ptr;
    توڑو;
  حالت CURLOPT_SEEKDATA:
    s->seek_client = ptr;
    توڑو;
  حالت CURLOPT_IOCTLDATA:
    s->ioctl_client = ptr;
    توڑو;
  حالت CURLOPT_SOCKOPTDATA:
    s->sockopt_client = ptr;
    توڑو;
  حالت CURLOPT_OPENSOCKETDATA:
    s->opensocket_client = ptr;
    توڑو;
  حالت CURLOPT_RESOLVER_آغاز_ڈیٹا:
    s->resolver_آغاز_client = ptr;
    توڑو;
  حالت CURLOPT_CLOSESOCKETDATA:
    s->closesocket_client = ptr;
    توڑو;
  حالت CURLOPT_PREREQDATA:
    s->prereq_userp = ptr;
    توڑو;
  حالت CURLOPT_ERRORBUFFER:
    s->errorbuffer = ptr;
    توڑو;
  حالت CURLOPT_URL:
    نتیجہ = Curl_setstropt(ڈیٹا, STRING_سیٹ_URL, ptr);
    Curl_bufref_سیٹ(&ڈیٹا->state.url,
                    CURL_EASY_STR(ڈیٹا, STRING_سیٹ_URL), 0, تہی);
    توڑو;

  حالت CURLOPT_USERPWD: {
    حرف *u = تہی, *p = تہی;
    نتیجہ = setstropt_userpwd(ptr, &u, &p);
    اگر(!نتیجہ) {
      نتیجہ = CURL_EASY_STR_SETN(ڈیٹا, STRING_USERNAME, u);
      u = تہی;
    }
    اگر(!نتیجہ) {
      نتیجہ = CURL_EASY_STR_SETN(ڈیٹا, STRING_PASSWORD, p);
      p = تہی;
    }
    curlx_free(u);
    curlx_free(p);
    واپس نتیجہ;
  }

  حالت CURLOPT_USERNAME:
    واپس Curl_setstropt(ڈیٹا, STRING_USERNAME, ptr);

  حالت CURLOPT_PASSWORD:
    واپس Curl_setstropt(ڈیٹا, STRING_PASSWORD, ptr);

  حالت CURLOPT_LOGIN_OPTIONS:
    واپس Curl_setstropt(ڈیٹا, STRING_OPTIONS, ptr);

  حالت CURLOPT_XOAUTH2_BEARER:
    واپس Curl_setstropt(ڈیٹا, STRING_BEARER, ptr);
  حالت CURLOPT_RANGE:
    واپس Curl_setstropt(ڈیٹا, STRING_سیٹ_RANGE, ptr);
  حالت CURLOPT_نجی:
    s->نجی_ڈیٹا = ptr;
    توڑو;
  حالت CURLOPT_PROTOCOLS_STR:
    اگر(ptr) {
      curl_prot_t protos;
      نتیجہ = protocol2num(ptr, &protos);
      اگر(!نتیجہ)
        s->allowed_protocols = protos;
    }
    ورنہ
      /* make a NULL argument reset to default */
      s->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
    توڑو;
  حالت CURLOPT_REDIR_PROTOCOLS_STR:
    اگر(ptr) {
      curl_prot_t protos;
      نتیجہ = protocol2num(ptr, &protos);
      اگر(!نتیجہ)
        s->redir_protocols = protos;
    }
    ورنہ
      /* make a NULL argument reset to default */
      s->redir_protocols = (curl_prot_t)CURLPROTO_REDIR;
    توڑو;
  حالت CURLOPT_DEFAULT_PROTOCOL:
    /* Set the protocol to use when the URL does not include any protocol */
    واپس Curl_setstropt(ڈیٹا, STRING_DEFAULT_PROTOCOL, ptr);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_SMTP
  حالت CURLOPT_MAIL_FROM:
    /* Set the SMTP mail originator */
    واپس Curl_setstropt(ڈیٹا, STRING_MAIL_FROM, ptr);
  حالت CURLOPT_MAIL_AUTH:
    /* Set the SMTP auth originator */
    واپس Curl_setstropt(ڈیٹا, STRING_MAIL_AUTH, ptr);
#اگر_ختم
  حالت CURLOPT_SASL_AUTHZID:
    /* Authorization identity (identity to act as) */
    واپس Curl_setstropt(ڈیٹا, STRING_SASL_AUTHZID, ptr);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_RTSP
  حالت CURLOPT_RTSP_SESSION_ID:
    Curl_peer_unlink(&ڈیٹا->state.rtsp_session_origin);
    واپس Curl_setstropt(ڈیٹا, STRING_RTSP_SESSION_ID, ptr);
  حالت CURLOPT_RTSP_دھارا_URI:
    واپس Curl_setstropt(ڈیٹا, STRING_RTSP_دھارا_URI, ptr);
  حالت CURLOPT_RTSP_TRANSPORT:
    واپس Curl_setstropt(ڈیٹا, STRING_RTSP_TRANSPORT, ptr);
  حالت CURLOPT_INTERLEAVEDATA:
    s->rtp_برآمد = ptr;
    توڑو;
#اگر_ختم /* !CURL_DISABLE_RTSP */
  حالت CURLOPT_TLSAUTH_USERNAME:
  حالت CURLOPT_TLSAUTH_PASSWORD:
  حالت CURLOPT_TLSAUTH_TYPE:
  حالت CURLOPT_PROXY_TLSAUTH_USERNAME:
  حالت CURLOPT_PROXY_TLSAUTH_PASSWORD:
  حالت CURLOPT_PROXY_TLSAUTH_TYPE:
    واپس CURLE_NOT_BUILT_اندر;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HSTS
  حالت CURLOPT_HSTSREADDATA:
    s->hsts_read_userp = ptr;
    توڑو;
  حالت CURLOPT_HSTSWRITEDATA:
    s->hsts_write_userp = ptr;
    توڑو;
  حالت CURLOPT_HSTS: {
    ساخت curl_slist *h;
    اگر(!ڈیٹا->hsts) {
      ڈیٹا->hsts = Curl_hsts_init();
      اگر(!ڈیٹا->hsts)
        واپس CURLE_برآمد_کا_حافظہ;
    }
    اگر(ptr) {
      نتیجہ = Curl_setstropt(ڈیٹا, STRING_HSTS, ptr);
      اگر(نتیجہ)
        واپس نتیجہ;
      /* this needs to build a list of filenames to read from, so that it can
         read them later, as we might get a shared HSTS handle to load them
         into */
      h = curl_slist_append(ڈیٹا->state.hstslist, ptr);
      اگر(!h) {
        curl_slist_free_all(ڈیٹا->state.hstslist);
        ڈیٹا->state.hstslist = تہی;
        واپس CURLE_برآمد_کا_حافظہ;
      }
      ڈیٹا->state.hstslist = h; /* store the list for later use */
    }
    ورنہ {
      /* clear the list of HSTS files */
      curl_slist_free_all(ڈیٹا->state.hstslist);
      ڈیٹا->state.hstslist = تہی;
      اگر(!ڈیٹا->share || !ڈیٹا->share->hsts)
        /* throw away the HSTS cache unless shared */
        Curl_hsts_cleanup(&ڈیٹا->hsts);
    }
    توڑو;
  }
#اگر_ختم /* !CURL_DISABLE_HSTS */
#اگر_غیر_تعریف_شدہ CURL_DISABLE_ALTSVC
  حالت CURLOPT_ALTSVC:
    اگر(!ڈیٹا->asi) {
      ڈیٹا->asi = Curl_altsvc_init();
      اگر(!ڈیٹا->asi)
        واپس CURLE_برآمد_کا_حافظہ;
    }
    نتیجہ = Curl_setstropt(ڈیٹا, STRING_ALTSVC, ptr);
    اگر(نتیجہ)
      توڑو;
    اگر(ptr)
      واپس Curl_altsvc_بھرو(ڈیٹا->asi, ptr);
    توڑو;
#اگر_ختم /* !CURL_DISABLE_ALTSVC */
  حالت CURLOPT_ECH:
    واپس setopt_ech(ڈیٹا, ptr);
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }
  واپس نتیجہ;
}

ساکن CURLcode setopt_cptr(ساخت Curl_easy *ڈیٹا, CURLoption option,
                            حرف *ptr)
{
  قسمتعریف CURLcode (*ptrfunc)(ساخت Curl_easy *ڈیٹا, CURLoption option,
                              حرف *ptr);
  /* Order by likeliness */
  ساکن ثابت ptrfunc setopt_call[] = {
    setopt_cptr_misc,
#اگر defined(USE_SSL) || defined(USE_SSH)
    setopt_cptr_ssl,
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
    setopt_cptr_proxy,
#اگر_ختم
    setopt_cptr_net,
#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
    setopt_cptr_ftp,
#اگر_ختم
#اگر_تعریف_شدہ USE_SSH
    setopt_cptr_ssh,
#اگر_ختم
#اگر !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
    setopt_cptr_http_mqtt,
#اگر_ختم
  };
  size_t i;

  برائے(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode نتیجہ = setopt_call[i](ڈیٹا, option, ptr);
    اگر(نتیجہ != CURLE_UNKNOWN_OPTION)
      واپس نتیجہ;
  }
  واپس CURLE_UNKNOWN_OPTION;
}

ساکن CURLcode setopt_func(ساخت Curl_easy *ڈیٹا, CURLoption option,
                            va_list param)
{
  ساخت UserDefined *s = &ڈیٹا->سیٹ;
  انتخاب(option) {
  حالت CURLOPT_PROGRESSFUNCTION:
    /*
     * Progress callback function
     */
    s->fprogress = va_arg(param, curl_پیش_رفت_callback);
    اگر(s->fprogress)
      ڈیٹا->پیش_رفت.callback = TRUE; /* no longer internal */
    ورنہ
      ڈیٹا->پیش_رفت.callback = FALSE; /* NULL enforces internal */
    توڑو;

  حالت CURLOPT_XFERINFOFUNCTION:
    /*
     * Transfer info callback function
     */
    s->fxferinfo = va_arg(param, curl_xferinfo_callback);
    اگر(s->fxferinfo)
      ڈیٹا->پیش_رفت.callback = TRUE; /* no longer internal */
    ورنہ
      ڈیٹا->پیش_رفت.callback = FALSE; /* NULL enforces internal */

    توڑو;
  حالت CURLOPT_DEBUGFUNCTION:
    /*
     * stderr write callback.
     */
    s->fdebug = va_arg(param, curl_debug_callback);
    /*
     * if the callback provided is NULL, it will use the default callback
     */
    توڑو;
  حالت CURLOPT_HEADERFUNCTION:
    /*
     * Set header write callback
     */
    s->fwrite_سرخی = va_arg(param, curl_write_callback);
    توڑو;
  حالت CURLOPT_WRITEFUNCTION:
    /*
     * Set data write callback
     */
    s->fwrite_func = va_arg(param, curl_write_callback);
    اگر(!s->fwrite_func)
#اگر defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic ڈالو
#ہدایت clang diagnostic ignored "-Wcast-function-type-strict"
#اگر_ختم
      /* When set to NULL, reset to our internal default function */
      s->fwrite_func = (curl_write_callback)fwrite;
#اگر defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic نکالو
#اگر_ختم
    توڑو;
  حالت CURLOPT_READFUNCTION:
    /*
     * Read data callback
     */
    s->fread_func_سیٹ = va_arg(param, curl_read_callback);
    اگر(!s->fread_func_سیٹ) {
      s->is_fread_سیٹ = 0;
#اگر defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic ڈالو
#ہدایت clang diagnostic ignored "-Wcast-function-type-strict"
#اگر_ختم
      /* When set to NULL, reset to our internal default function */
      s->fread_func_سیٹ = (curl_read_callback)fread;
#اگر defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic نکالو
#اگر_ختم
    }
    ورنہ
      s->is_fread_سیٹ = 1;
    توڑو;
  حالت CURLOPT_SEEKFUNCTION:
    /*
     * Seek callback. Might be NULL.
     */
    s->seek_func = va_arg(param, curl_seek_callback);
    توڑو;
  حالت CURLOPT_IOCTLFUNCTION:
    /*
     * I/O control callback. Might be NULL.
     */
    s->ioctl_func = va_arg(param, curl_ioctl_callback);
    توڑو;
  حالت CURLOPT_SSL_CTX_FUNCTION:
    /*
     * Set an SSL_CTX callback
     */
#اگر_تعریف_شدہ USE_SSL
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_SSL_CTX)) {
      s->ssl_fsslctx = va_arg(param, curl_ssl_ctx_callback);
      توڑو;
    }
    ورنہ
#اگر_ختم
      واپس CURLE_NOT_BUILT_اندر;

  حالت CURLOPT_SOCKOPTFUNCTION:
    /*
     * socket callback function: called after socket() but before connect()
     */
    s->fsockopt = va_arg(param, curl_sockopt_callback);
    توڑو;

  حالت CURLOPT_OPENSOCKETFUNCTION:
    /*
     * open/create socket callback function: called instead of socket(),
     * before connect()
     */
    s->fopensocket = va_arg(param, curl_opensocket_callback);
    توڑو;

  حالت CURLOPT_CLOSESOCKETFUNCTION:
    /*
     * close socket callback function: called instead of close()
     * when shutting down a connection
     */
    s->fclosesocket = va_arg(param, curl_closesocket_callback);
    توڑو;

  حالت CURLOPT_RESOLVER_آغاز_FUNCTION:
    /*
     * resolver start callback function: called before a new resolver request
     * is started
     */
    s->resolver_آغاز = va_arg(param, curl_resolver_آغاز_callback);
    توڑو;

#اگر_تعریف_شدہ USE_SSH
#اگر_تعریف_شدہ USE_LIBSSH2
  حالت CURLOPT_SSH_HOSTKEYFUNCTION:
    /* the callback to check the hostkey without the knownhost file */
    s->ssh_hostkeyfunc = va_arg(param, curl_sshhostkeycallback);
    توڑو;
#اگر_ختم

  حالت CURLOPT_SSH_KEYFUNCTION:
    /* setting to NULL is fine since the ssh.c functions themselves will
       then revert to use the internal default */
    s->ssh_keyfunc = va_arg(param, curl_sshkeycallback);
    توڑو;

#اگر_ختم /* USE_SSH */

#اگر_غیر_تعریف_شدہ CURL_DISABLE_RTSP
  حالت CURLOPT_INTERLEAVEFUNCTION:
    /* Set the user defined RTP write function */
    s->fwrite_rtp = va_arg(param, curl_write_callback);
    توڑو;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
  حالت CURLOPT_CHUNK_BGN_FUNCTION:
    s->chunk_bgn = va_arg(param, curl_chunk_bgn_callback);
    توڑو;
  حالت CURLOPT_CHUNK_انتہا_FUNCTION:
    s->chunk_انتہا = va_arg(param, curl_chunk_انتہا_callback);
    توڑو;
  حالت CURLOPT_FNMATCH_FUNCTION:
    s->fnmatch = va_arg(param, curl_fnmatch_callback);
    توڑو;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
  حالت CURLOPT_TRAILERFUNCTION:
    s->trailer_callback = va_arg(param, curl_trailer_callback);
    توڑو;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HSTS
  حالت CURLOPT_HSTSREADFUNCTION:
    s->hsts_read = va_arg(param, curl_hstsread_callback);
    توڑو;
  حالت CURLOPT_HSTSWRITEFUNCTION:
    s->hsts_write = va_arg(param, curl_hstswrite_callback);
    توڑو;
#اگر_ختم
  حالت CURLOPT_PREREQFUNCTION:
    s->fprereq = va_arg(param, curl_prereq_callback);
    توڑو;
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }
  واپس CURLE_OK;
}

ساکن CURLcode setopt_offt(ساخت Curl_easy *ڈیٹا, CURLoption option,
                            curl_off_t offt)
{
  ساخت UserDefined *s = &ڈیٹا->سیٹ;
  انتخاب(option) {
  حالت CURLOPT_TIMEVALUE_LARGE:
    /*
     * This is the value to compare with the remote document with the
     * method set with CURLOPT_TIMECONDITION
     */
    s->timevalue = (time_t)offt;
    توڑو;

    /* MQTT "borrows" some of the HTTP options */
  حالت CURLOPT_POSTFIELDSIZE_LARGE:
    /*
     * The size of the POSTFIELD data to prevent libcurl to do strlen() to
     * figure it out. Enables binary posts.
     */
    اگر(offt < -1)
      واپس CURLE_BAD_FUNCTION_دلیل;

    اگر(s->postfieldsize < offt && s->str_copypostfields) {
      /* Previous CURLOPT_COPYPOSTFIELDS is no longer valid. */
      curlx_safefree(s->str_copypostfields);
      s->postfields = تہی;
    }
    s->postfieldsize = offt;
    توڑو;
  حالت CURLOPT_INFILESIZE_LARGE:
    /*
     * If known, this should inform curl about the file size of the
     * to-be-uploaded file.
     */
    اگر(offt < -1)
      واپس CURLE_BAD_FUNCTION_دلیل;
    s->filesize = offt;
    توڑو;
  حالت CURLOPT_MAX_SEND_رفتار_LARGE:
    /*
     * When transfer uploads are faster than CURLOPT_MAX_SEND_SPEED_LARGE
     * bytes per second the transfer is throttled..
     */
    اگر(offt < 0)
      واپس CURLE_BAD_FUNCTION_دلیل;
    s->max_send_رفتار = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&ڈیٹا->پیش_رفت.ul.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    توڑو;
  حالت CURLOPT_MAX_RECV_رفتار_LARGE:
    /*
     * When receiving data faster than CURLOPT_MAX_RECV_SPEED_LARGE bytes per
     * second the transfer is throttled..
     */
    اگر(offt < 0)
      واپس CURLE_BAD_FUNCTION_دلیل;
    s->max_recv_رفتار = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&ڈیٹا->پیش_رفت.dl.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    توڑو;
  حالت CURLOPT_RESUME_FROM_LARGE:
    /*
     * Resume transfer at the given file position
     */
    اگر(offt < -1)
      واپس CURLE_BAD_FUNCTION_دلیل;
    s->سیٹ_resume_from = offt;
    توڑو;
  حالت CURLOPT_MAXFILESIZE_LARGE:
    /*
     * Set the maximum size of a file to download.
     */
    اگر(offt < 0)
      واپس CURLE_BAD_FUNCTION_دلیل;
    s->max_filesize = offt;
    توڑو;

  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }
  واپس CURLE_OK;
}

ساکن CURLcode setopt_blob(ساخت Curl_easy *ڈیٹا, CURLoption option,
                            ساخت curl_blob *blob)
{
  ساخت UserDefined *s = &ڈیٹا->سیٹ;
  انتخاب(option) {
  حالت CURLOPT_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use
     */
    واپس Curl_setblobopt(&s->blobs[BLOB_CERT], blob);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  حالت CURLOPT_PROXY_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use for proxy
     */
    واپس Curl_setblobopt(&s->blobs[BLOB_CERT_PROXY], blob);
  حالت CURLOPT_PROXY_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use for proxy
     */
    واپس Curl_setblobopt(&s->blobs[BLOB_کلید_PROXY], blob);
  حالت CURLOPT_PROXY_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection proxy.
     * Specify entire PEM of the CA certificate
     */
#اگر_تعریف_شدہ USE_SSL
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_CAINFO_BLOB)) {
      CURLcode نتیجہ = Curl_setblobopt(&s->blobs[BLOB_CAINFO_PROXY], blob);
      اگر(!نتیجہ)
        s->proxy_ssl.custom_cablob = TRUE;
      واپس نتیجہ;
    }
#اگر_ختم
    واپس CURLE_NOT_BUILT_اندر;
  حالت CURLOPT_PROXY_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_ISSUERCERT_BLOB))
      واپس Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT_PROXY], blob);
    واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
  حالت CURLOPT_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use
     */
    واپس Curl_setblobopt(&s->blobs[BLOB_کلید], blob);
  حالت CURLOPT_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection.
     * Specify entire PEM of the CA certificate
     */
#اگر_تعریف_شدہ USE_SSL
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_CAINFO_BLOB)) {
      CURLcode نتیجہ = Curl_setblobopt(&s->blobs[BLOB_CAINFO], blob);
      اگر(!نتیجہ)
        s->ssl.custom_cablob = TRUE;
      واپس نتیجہ;
    }
#اگر_ختم
    واپس CURLE_NOT_BUILT_اندر;
  حالت CURLOPT_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    اگر(Curl_ssl_supports(ڈیٹا, SSLSUPP_ISSUERCERT_BLOB))
      واپس Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT], blob);
    واپس CURLE_NOT_BUILT_اندر;

  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }
  /* unreachable */
}

/*
 * Do not make Curl_vsetopt() static: it is called from
 * projects/OS400/ccsidcurl.c.
 */
CURLcode Curl_vsetopt(ساخت Curl_easy *ڈیٹا, CURLoption option, va_list param)
{
  اگر(option < CURLOPTTYPE_OBJECTPOINT)
    واپس setopt_long(ڈیٹا, option, va_arg(param, طویل));
  ورنہ اگر(option < CURLOPTTYPE_FUNCTIONPOINT) {
    /* unfortunately, different pointer types cannot be identified any other
       way than being listed explicitly */
    انتخاب(option) {
    حالت CURLOPT_HTTPHEADER:
    حالت CURLOPT_QUOTE:
    حالت CURLOPT_POSTQUOTE:
    حالت CURLOPT_TELNETOPTIONS:
    حالت CURLOPT_PREQUOTE:
    حالت CURLOPT_HTTP200ALIASES:
    حالت CURLOPT_MAIL_RCPT:
    حالت CURLOPT_RESOLVE:
    حالت CURLOPT_PROXYHEADER:
    حالت CURLOPT_CONNECT_مقصد:
      واپس setopt_slist(ڈیٹا, option, va_arg(param, ساخت curl_slist *));
    حالت CURLOPT_HTTPPOST:         /* curl_httppost * */
    حالت CURLOPT_MIMEPOST:         /* curl_mime * */
    حالت CURLOPT_STDERR:           /* FILE * */
    حالت CURLOPT_SHARE:            /* CURLSH * */
    حالت CURLOPT_CURLU:            /* CURLU * */
      واپس setopt_اشارے(ڈیٹا, option, param);
    حالت CURLOPT_دھارا_DEPENDS:   /* CURL * */
    حالت CURLOPT_دھارا_DEPENDS_E: /* CURL * */
      واپس CURLE_OK;
    طےشدہ:
      توڑو;
    }
    /* the char pointer options */
    واپس setopt_cptr(ڈیٹا, option, va_arg(param, حرف *));
  }
  ورنہ اگر(option < CURLOPTTYPE_OFF_T)
    واپس setopt_func(ڈیٹا, option, param);
  ورنہ اگر(option < CURLOPTTYPE_BLOB)
    واپس setopt_offt(ڈیٹا, option, va_arg(param, curl_off_t));
  واپس setopt_blob(ڈیٹا, option, va_arg(param, ساخت curl_blob *));
}

/*
 * curl_easy_setopt() is the external interface for setting options on an
 * easy handle.
 *
 * NOTE: This is one of few API functions that are allowed to be called from
 * within a callback.
 */
#تعریف_منسوخ curl_easy_setopt
CURLcode curl_easy_setopt(CURL *curl, CURLoption option, ...)
{
  ساخت Curl_eapi_guard guard;
  CURLcode نتیجہ;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_setopt, &نتیجہ)) {
    ساخت Curl_easy *ڈیٹا = curl;
    va_list arg;

    va_start(arg, option);

    نتیجہ = Curl_vsetopt(ڈیٹا, option, arg);

    va_end(arg);
    اگر(نتیجہ == CURLE_BAD_FUNCTION_دلیل)
      failf(ڈیٹا, "setopt 0x%x got bad argument", (غیرعلامتی صحیح)option);
  }
  CURL_EAPI_LEAVE(&guard);
  واپس نتیجہ;
}
