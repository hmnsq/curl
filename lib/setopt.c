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
#অন্তর্ভাব "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#অন্তর্ভাব <linux/tcp.h>
#নোচেদ্যদি defined(HAVE_NETINET_TCP_H)
#অন্তর্ভাব <netinet/tcp.h>
#endif

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "content_encoding.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "curl_share.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "setopt.h"
#অন্তর্ভাব "altsvc.h"
#অন্তর্ভাব "hsts.h"
#অন্তর্ভাব "tftp.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "bufref.h"
#অন্তর্ভাব "vauth/vauth.h"

স্থির CURLcode setopt_set_timeout_sec(timediff_t *ptimeout_ms, long secs)
{
  যদি(secs < 0)
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
#যদি LONG_MAX > (TIMEDIFF_T_MAX / 1000)
  যদি(secs > (TIMEDIFF_T_MAX / 1000)) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    প্রত্যাগম CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)secs * 1000;
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode setopt_set_timeout_ms(timediff_t *ptimeout_ms, long ms)
{
  যদি(ms < 0)
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
#যদি LONG_MAX > TIMEDIFF_T_MAX
  যদি(ms > TIMEDIFF_T_MAX) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    প্রত্যাগম CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)ms;
  প্রত্যাগম CURLE_OK;
}

CURLcode Curl_setstropt(সংরচনা Curl_easy *data,
                        গণনা dupstring id, ধ্রুব বর্ণ *s)
{
  size_t slen = s ? strlen(s) : 0;
  DEBUGASSERT((unsigned)id <= UINT8_MAX);
  যদি(s && (slen > CURL_MAX_প্রবেশ_LENGTH))
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;

  প্রত্যাগম CURL_EASY_STR_SET(data, (uint8_t)id, s, slen);
}

CURLcode Curl_setblobopt(সংরচনা curl_blob **blobp,
                         ধ্রুব সংরচনা curl_blob *blob)
{
  /* free the previous storage at `blobp' and replace by a dynamic storage
     copy of blob. If CURL_BLOB_COPY is set, the data is copied. */

  curlx_safefree(*blobp);

  যদি(blob) {
    সংরচনা curl_blob *nblob;
    যদি(!blob->data || !blob->len || (blob->len > CURL_MAX_প্রবেশ_LENGTH))
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    nblob = (সংরচনা curl_blob *)
      curlx_malloc(পরিমাণ(সংরচনা curl_blob) +
                   ((blob->flags & CURL_BLOB_COPY) ? blob->len : 0));
    যদি(!nblob)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    *nblob = *blob;
    যদি(blob->flags & CURL_BLOB_COPY) {
      /* put the data after the blob struct in memory */
      nblob->data = (বর্ণ *)nblob + পরিমাণ(সংরচনা curl_blob);
      memcpy(nblob->data, blob->data, blob->len);
    }

    *blobp = nblob;
    প্রত্যাগম CURLE_OK;
  }

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode setstropt_userpwd(ধ্রুব বর্ণ *option, বর্ণ **userp,
                                  বর্ণ **passwdp)
{
  বর্ণ *user = রিক্ত;
  বর্ণ *passwd = রিক্ত;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Parse the login details if specified. If not, then we treat NULL as a
     hint to clear the existing data */
  যদি(option) {
    size_t len = strlen(option);
    CURLcode result;
    যদি(len > CURL_MAX_প্রবেশ_LENGTH)
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;

    result = Curl_parse_login_details(option, len, &user, &passwd, রিক্ত);
    যদি(result)
      প্রত্যাগম result;
  }

  curlx_free(*userp);
  *userp = user;

  curlx_strzero(*passwdp);
  curlx_free(*passwdp);
  *passwdp = passwd;

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode setstropt_interface(সংরচনা Curl_easy *data, বর্ণ *option)
{
  যদি(option && !option[0])
    option = রিক্ত;
  যদি(option) {
    /* Parse for checking syntax */
    CURLcode result = Curl_parse_interface(option, রিক্ত, রিক্ত, রিক্ত);
    যদি(result)
      প্রত্যাগম result;
    প্রত্যাগম CURL_EASY_STR_SET(data, STRING_INTERFACE_OPT,
                             option, strlen(option));
  }
  CURL_EASY_STR_CLEAR(data, STRING_INTERFACE_OPT);
  প্রত্যাগম CURLE_OK;
}

#ifdef USE_SSL
#define C_SSLVERSION_মান(x)     ((x) & 0xffff)
#define C_SSLVERSION_MAX_মান(x) ((unsigned long)(x) & 0xffff0000)
#endif

স্থির CURLcode protocol2num(ধ্রুব বর্ণ *str, curl_prot_t *val)
{
  /*
   * We are asked to cherry-pick protocols, so play it safe and disallow all
   * protocols to start with, and re-add the wanted ones back in.
   */
  *val = 0;

  যদি(!str)
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(curl_strequal(str, "all")) {
    *val = ~(curl_prot_t)0;
    প্রত্যাগম CURLE_OK;
  }

  কুরু {
    ধ্রুব বর্ণ *token = str;
    size_t tlen;

    str = strchr(str, ',');
    tlen = str ? (size_t)(str - token) : strlen(token);
    যদি(tlen) {
      ধ্রুব সংরচনা Curl_scheme *h = Curl_getn_scheme(token, tlen);

      যদি(!h || !h->run)
        প্রত্যাগম CURLE_UNSUPPORTED_PROTOCOL;

      *val |= h->protocol;
    }
  } যাবত্(str && str++);

  যদি(!*val)
    /* no protocol listed */
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  প্রত্যাগম CURLE_OK;
}

#যদি !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
স্থির CURLcode httpauth(সংরচনা Curl_easy *data, bool proxy,
                         unsigned long auth)
{
  যদি(auth != CURLAUTH_NONE) {
    পূর্ণাঙ্ক bitcheck = 0;
    bool authbits = FALSE;
    যদি(auth & CURLAUTH_DIGEST_IE) {
      auth |= CURLAUTH_DIGEST; /* set standard digest bit */
      auth &= ~CURLAUTH_DIGEST_IE; /* drop the legacy bit */
    }

    /* switch off bits we cannot support */
#ifndef USE_NTLM
    auth &= ~CURLAUTH_NTLM; /* no NTLM support */
#endif
#ifndef USE_SPNEGO
    auth &= ~CURLAUTH_NEGOTIATE; /* no Negotiate (SPNEGO) auth without GSS-API
                                    or SSPI */
#endif

    /* check if any auth bit lower than CURLAUTH_ONLY is still set */
    যাবত্(bitcheck < 31) {
      যদি(auth & (1UL << bitcheck++)) {
        authbits = TRUE;
        ভঙ্গ;
      }
    }
    যদি(!authbits)
      প্রত্যাগম CURLE_NOT_BUILT_IN; /* no supported types left! */
  }
  যদি(proxy)
    data->set.proxyauth = (uint32_t)auth;
  অন্যথা
    data->set.httpauth = (uint32_t)auth;
  প্রত্যাগম CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */

#ifndef CURL_DISABLE_HTTP
স্থির CURLcode setopt_HTTP_VERSION(সংরচনা Curl_easy *data, long arg)
{
  /*
   * This sets a requested HTTP version to be used. The value is one of
   * the listed enums in curl/curl.h.
   */
  বিকল্প(arg) {
  পক্ষ CURL_HTTP_VERSION_NONE:
    /* accepted */
    ভঙ্গ;
  পক্ষ CURL_HTTP_VERSION_1_0:
  পক্ষ CURL_HTTP_VERSION_1_1:
    /* accepted */
    ভঙ্গ;
#ifdef USE_HTTP2
  পক্ষ CURL_HTTP_VERSION_2_0:
  পক্ষ CURL_HTTP_VERSION_2TLS:
  পক্ষ CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    /* accepted */
    ভঙ্গ;
#endif
#ifdef USE_HTTP3
  পক্ষ CURL_HTTP_VERSION_3:
  পক্ষ CURL_HTTP_VERSION_3ONLY:
    /* accepted */
    ভঙ্গ;
#endif
  স্বভাব:
    /* not accepted */
    যদি(arg < CURL_HTTP_VERSION_NONE)
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    প্রত্যাগম CURLE_UNSUPPORTED_PROTOCOL;
  }
  data->set.httpwant = (unsigned বর্ণ)arg;
  প্রত্যাগম CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP */

#ifdef USE_SSL
CURLcode Curl_setopt_SSLVERSION(সংরচনা Curl_easy *data, CURLoption option,
                                long arg)
{
  /*
   * Set explicit SSL version to try to connect with, as some SSL
   * implementations are lame.
   */
  {
    long version, version_max;
    সংরচনা ssl_easy_config *sslc = &data->set.ssl;
#ifndef CURL_DISABLE_PROXY
    যদি(option != CURLOPT_SSLVERSION)
      sslc = &data->set.proxy_ssl;
#অন্যথা
    (শূন্য)option; /* unused */
#endif
    version = C_SSLVERSION_মান(arg);
    version_max = (long)C_SSLVERSION_MAX_মান(arg);

    যদি(version < CURL_SSLVERSION_DEFAULT ||
       version == CURL_SSLVERSION_SSLv2 ||
       version == CURL_SSLVERSION_SSLv3 ||
       version >= CURL_SSLVERSION_LAST ||
       version_max < CURL_SSLVERSION_MAX_NONE ||
       version_max >= CURL_SSLVERSION_MAX_LAST)
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    যদি(version == CURL_SSLVERSION_DEFAULT)
      version = CURL_SSLVERSION_TLSv1_2;

    sslc->version = (unsigned বর্ণ)version;
    sslc->version_max = (unsigned পূর্ণাঙ্ক)version_max;
  }
  প্রত্যাগম CURLE_OK;
}
#endif /* !USE_SSL */

#ifndef CURL_DISABLE_RTSP
স্থির CURLcode setopt_RTSP_REQUEST(সংরচনা Curl_easy *data, long arg)
{
  /*
   * Set the RTSP request method (OPTIONS, SETUP, PLAY, etc...) Would this be
   * better if the RTSPREQ_* were moved into here?
   */
  যদি((arg <= CURL_RTSPREQ_NONE) || (arg >= CURL_RTSPREQ_LAST))
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;

  data->set.rtspreq = (unsigned বর্ণ)arg;
  প্রত্যাগম CURLE_OK;
}
#endif /* !CURL_DISABLE_RTSP */

স্থির CURLcode setopt_long_bool(সংরচনা Curl_easy *data, CURLoption option,
                                 long arg)
{
  bool enabled = !!arg;
  পূর্ণাঙ্ক ok = 1;
  সংরচনা UserDefined *s = &data->set;
  বিকল্প(option) {
  পক্ষ CURLOPT_FORBID_REUSE:
    /*
     * When this transfer is done, it must not be left to be reused by a
     * subsequent transfer but shall be closed immediately.
     */
    s->reuse_forbid = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_FRESH_CONNECT:
    /*
     * This transfer shall not use a previously cached connection but
     * should be made with a fresh new connect!
     */
    s->reuse_fresh = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_VERBOSE:
    /*
     * Verbose means infof() calls that give a lot of information about
     * the connection and transfer procedures as well as internal choices.
     */
    s->verbose = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_HEADER:
    /*
     * Set to include the header in the general data output stream.
     */
    s->include_header = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_NOPROGRESS:
    /*
     * Shut off the internal supported progress meter
     */
    data->progress.hide = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_NOBODY:
    /*
     * Do not include the body part in the output data stream.
     */
    s->opt_no_body = enabled;
#ifndef CURL_DISABLE_HTTP
    যদি(s->opt_no_body)
      /* in HTTP lingo, no body means using the HEAD request... */
      s->method = HTTPREQ_HEAD;
    অন্যথা যদি(s->method == HTTPREQ_HEAD)
      s->method = HTTPREQ_GET;
#endif
    ভঙ্গ;
  পক্ষ CURLOPT_FAILONERROR:
    /*
     * Do not output the >=400 error code HTML-page, but instead only
     * return error.
     */
    s->http_fail_on_error = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_KEEP_SENDING_ON_ERROR:
    s->http_keep_sending_on_error = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_UPLOAD:
  পক্ষ CURLOPT_PUT:
    /*
     * We want to send data to the remote host. If this is HTTP, that equals
     * using the PUT request.
     */
    যদি(enabled) {
      /* If this is HTTP, PUT is what's needed to "upload" */
      s->method = HTTPREQ_PUT;
      s->opt_no_body = FALSE; /* this is implied */
    }
    অন্যথা
      /* In HTTP, the opposite of upload is GET (unless NOBODY is true as
         then this can be changed to HEAD later on) */
      s->method = HTTPREQ_GET;
    ভঙ্গ;
  পক্ষ CURLOPT_FILETIME:
    /*
     * Try to get the file time of the remote document. The time will
     * later (possibly) become available using curl_easy_getinfo().
     */
    s->get_filetime = enabled;
    ভঙ্গ;
#ifndef CURL_DISABLE_HTTP
  পক্ষ CURLOPT_HTTP09_ALLOWED:
    s->http09_allowed = enabled;
    ভঙ্গ;
#ifndef CURL_DISABLE_COOKIES
  পক্ষ CURLOPT_COOKIESESSION:
    /*
     * Set this option to TRUE to start a new "cookie session". It will
     * prevent the forthcoming read-cookies-from-file actions to accept
     * cookies that are marked as being session cookies, as they belong to a
     * previous session.
     */
    s->cookiesession = enabled;
    ভঙ্গ;
#endif
  পক্ষ CURLOPT_AUTOREFERER:
    /*
     * Switch on automatic referer that gets set if curl follows locations.
     */
    s->http_auto_referer = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_TRANSFER_ENCODING:
    s->http_transfer_encoding = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_UNRESTRICTED_AUTH:
    /*
     * Send authentication (user+password) when following locations, even when
     * hostname changed.
     */
    s->allow_auth_to_other_hosts = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_HTTP_TRANSFER_DECODING:
    /*
     * disable libcurl transfer encoding is used
     */
    s->http_te_skip = !enabled; /* reversed */
    ভঙ্গ;
  পক্ষ CURLOPT_HTTP_CONTENT_DECODING:
    /*
     * raw data passed to the application when content encoding is used
     */
    s->http_ce_skip = !enabled; /* reversed */
    ভঙ্গ;
  পক্ষ CURLOPT_HTTPGET:
    /*
     * Set to force us do HTTP GET
     */
    যদি(enabled) {
      s->method = HTTPREQ_GET;
      s->opt_no_body = FALSE; /* this is implied */
    }
    ভঙ্গ;
  পক্ষ CURLOPT_POST:
    /* Does this option serve a purpose anymore? Yes it does, when
       CURLOPT_POSTFIELDS is not used and the POST data is read off the
       callback! */
    যদি(enabled) {
      s->method = HTTPREQ_POST;
      s->opt_no_body = FALSE; /* this is implied */
    }
    অন্যথা
      s->method = HTTPREQ_GET;
    ভঙ্গ;
#endif /* !CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_PROXY
  পক্ষ CURLOPT_HTTPPROXYTUNNEL:
    /*
     * Tunnel operations through the proxy instead of normal proxy use
     */
    s->tunnel_thru_httpproxy = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_HAPROXYPROTOCOL:
    /*
     * Set to send the HAProxy Proxy Protocol header
     */
    s->haproxyprotocol = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_PROXY_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for proxy.
     */
    s->proxy_ssl.verifypeer = enabled;

    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    ভঙ্গ;
  পক্ষ CURLOPT_PROXY_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for proxy
     */
    s->proxy_ssl.verifyhost = enabled;
    ok = 2;
    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    ভঙ্গ;
  পক্ষ CURLOPT_PROXY_TRANSFER_MODE:
    /*
     * set transfer mode (;type=<a|i>) when doing FTP via an HTTP proxy
     */
    s->proxy_transfer_mode = enabled;
    ভঙ্গ;
#endif /* !CURL_DISABLE_PROXY */
#যদি defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  পক্ষ CURLOPT_SOCKS5_GSSAPI_NEC:
    /*
     * Set flag for NEC SOCKS5 support
     */
    s->socks5_gssapi_nec = enabled;
    ভঙ্গ;
#endif
#ifdef CURL_LIST_ONLY_PROTOCOL
  পক্ষ CURLOPT_DIRLISTONLY:
    /*
     * An option that changes the command to one that asks for a list only, no
     * file info details. Used for FTP, POP3 and SFTP.
     */
    s->list_only = enabled;
    ভঙ্গ;
#endif
  পক্ষ CURLOPT_APPEND:
    /*
     * We want to upload and append to an existing file. Used for FTP and
     * SFTP.
     */
    s->remote_append = enabled;
    ভঙ্গ;
#ifndef CURL_DISABLE_FTP
  পক্ষ CURLOPT_FTP_USE_EPRT:
    s->ftp_use_eprt = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_FTP_USE_EPSV:
    s->ftp_use_epsv = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_FTP_USE_PRET:
    s->ftp_use_pret = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_FTP_SKIP_PASV_IP:
    /*
     * Enable or disable FTP_SKIP_PASV_IP, which will disable/enable the
     * bypass of the IP address in PASV responses.
     */
    s->ftp_skip_ip = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_WILDCARDMATCH:
    s->wildcard_enabled = enabled;
    ভঙ্গ;
#endif
  পক্ষ CURLOPT_CRLF:
    /*
     * Kludgy option to enable CRLF conversions. Subject for removal.
     */
    s->crlf = enabled;
    ভঙ্গ;
#ifndef CURL_DISABLE_TFTP
  পক্ষ CURLOPT_TFTP_NO_OPTIONS:
    /*
     * Option that prevents libcurl from sending TFTP option requests to the
     * server.
     */
    s->tftp_no_options = enabled;
    ভঙ্গ;
#endif /* !CURL_DISABLE_TFTP */
  পক্ষ CURLOPT_TRANSFERTEXT:
    /*
     * This option was previously named 'FTPASCII'. Renamed to work with
     * more protocols than merely FTP.
     *
     * Transfer using ASCII (instead of BINARY).
     */
    s->prefer_ascii = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying.
     */
    s->ssl.verifypeer = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    ভঙ্গ;
#ifndef CURL_DISABLE_DOH
  পক্ষ CURLOPT_DOH_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for DoH.
     */
    s->doh_verifypeer = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_DOH_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for DoH
     */
    s->doh_verifyhost = enabled;
    ok = 2;
    ভঙ্গ;
  পক্ষ CURLOPT_DOH_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying for DoH.
     */
    যদি(!Curl_ssl_cert_status_request())
      প্রত্যাগম CURLE_NOT_BUILT_IN;

    s->doh_verifystatus = enabled;
    ok = 2;
    ভঙ্গ;
#endif /* !CURL_DISABLE_DOH */
  পক্ষ CURLOPT_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate
     */

    /* Obviously people are not reading documentation and too many thought
       this argument took a boolean when it was not and misused it.
       Treat 1 and 2 the same */
    s->ssl.verifyhost = enabled;
    ok = 2;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    ভঙ্গ;
  পক্ষ CURLOPT_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying.
     */
    যদি(!Curl_ssl_cert_status_request())
      প্রত্যাগম CURLE_NOT_BUILT_IN;

    s->ssl.verifystatus = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    ভঙ্গ;
  পক্ষ CURLOPT_CERTINFO:
#ifdef USE_SSL
    যদি(Curl_ssl_supports(data, SSLSUPP_CERTINFO))
      s->ssl.certinfo = enabled;
    অন্যথা
#endif
      প্রত্যাগম CURLE_NOT_BUILT_IN;
    ভঙ্গ;
  পক্ষ CURLOPT_NOSIGNAL:
    /*
     * The application asks not to set any signal() or alarm() handlers,
     * even when using a timeout.
     */
    s->no_signal = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_TCP_NODELAY:
    /*
     * Enable or disable TCP_NODELAY, which will disable/enable the Nagle
     * algorithm
     */
    s->tcp_nodelay = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_IGNORE_CONTENT_LENGTH:
    s->ignorecl = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_SSL_SESSIONID_CACHE:
    s->ssl.cache_session = enabled;
#ifndef CURL_DISABLE_PROXY
    s->proxy_ssl.cache_session = s->ssl.cache_session;
#endif
    ভঙ্গ;
#ifdef USE_SSH
  পক্ষ CURLOPT_SSH_COMPRESSION:
    s->ssh_compression = enabled;
    ভঙ্গ;
#endif /* !USE_SSH */
#ifndef CURL_DISABLE_SMTP
  পক্ষ CURLOPT_MAIL_RCPT_ALLOWFAILS:
    /* allow RCPT TO command to fail for some recipients */
    s->mail_rcpt_allowfails = enabled;
    ভঙ্গ;
#endif /* !CURL_DISABLE_SMTP */
  পক্ষ CURLOPT_SASL_IR:
    /* Enable/disable SASL initial response */
    s->sasl_ir = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_TCP_KEEPALIVE:
    s->tcp_keepalive = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_TCP_FASTOPEN:
#যদি defined(CONNECT_DATA_IDEMPOTENT) || defined(MSG_FASTOPEN) || \
  defined(TCP_FASTOPEN_CONNECT)
    s->tcp_fastopen = enabled;
    ভঙ্গ;
#অন্যথা
    প্রত্যাগম CURLE_NOT_BUILT_IN;
#endif
  পক্ষ CURLOPT_SSL_ENABLE_ALPN:
    s->ssl_enable_alpn = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_PATH_AS_IS:
    s->path_as_is = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_PIPEWAIT:
    s->pipewait = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_SUPPRESS_CONNECT_HEADERS:
    s->suppress_connect_headers = enabled;
    ভঙ্গ;
#ifndef CURL_DISABLE_SHUFFLE_DNS
  পক্ষ CURLOPT_DNS_SHUFFLE_ADDRESSES:
    s->dns_shuffle_addresses = enabled;
    ভঙ্গ;
#endif
  পক্ষ CURLOPT_DISALLOW_USERNAME_IN_URL:
    s->disallow_username_in_url = enabled;
    ভঙ্গ;
  পক্ষ CURLOPT_QUICK_EXIT:
    s->quick_exit = enabled;
    ভঙ্গ;
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }
  যদি((arg > ok) || (arg < 0))
    /* reserve other values for future use */
    infof(data, "boolean setopt(%d) got unsupported argument %ld,"
          " treated as %d", (পূর্ণাঙ্ক)option, arg, enabled);

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode মান_range(long *মান, long below_error, long min, long max)
{
  যদি(*মান < below_error)
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  অন্যথা যদি(*মান < min)
    *মান = min;
  অন্যথা যদি(*মান > max)
    *মান = max;
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode setopt_long_net(সংরচনা Curl_easy *data, CURLoption option,
                                long arg)
{
  CURLcode result = CURLE_OK;
  সংরচনা UserDefined *s = &data->set;

  বিকল্প(option) {
  পক্ষ CURLOPT_DNS_CACHE_TIMEOUT:
    যদি(arg != -1)
      প্রত্যাগম setopt_set_timeout_sec(&s->dns_cache_timeout_ms, arg);
    s->dns_cache_timeout_ms = -1;
    ভঙ্গ;
  পক্ষ CURLOPT_MAXCONNECTS:
    result = মান_range(&arg, 0, 0, INT_MAX);
    যদি(!result)
      s->maxconnects = arg ? (uint32_t)arg : DEFAULT_CONNCACHE_SIZE;
    ভঙ্গ;
  পক্ষ CURLOPT_SERVER_RESPONSE_TIMEOUT:
    প্রত্যাগম setopt_set_timeout_sec(&s->server_response_timeout, arg);
  পক্ষ CURLOPT_SERVER_RESPONSE_TIMEOUT_MS:
    প্রত্যাগম setopt_set_timeout_ms(&s->server_response_timeout, arg);
  পক্ষ CURLOPT_নিচের_SPEED_LIMIT:
    যদি(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->নিচের_speed_limit = arg;
    ভঙ্গ;
  পক্ষ CURLOPT_নিচের_SPEED_TIME:
    result = মান_range(&arg, 0, 0, USHRT_MAX);
    যদি(!result)
      s->নিচের_speed_time = (uint16_t)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_PORT:
    যদি((arg < 0) || (arg > 65535))
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    s->use_port = (unsigned short)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_TIMEOUT:
    প্রত্যাগম setopt_set_timeout_sec(&s->timeout, arg);
  পক্ষ CURLOPT_TIMEOUT_MS:
    প্রত্যাগম setopt_set_timeout_ms(&s->timeout, arg);
  পক্ষ CURLOPT_CONNECTTIMEOUT:
    প্রত্যাগম setopt_set_timeout_sec(&s->connecttimeout, arg);
  পক্ষ CURLOPT_CONNECTTIMEOUT_MS:
    প্রত্যাগম setopt_set_timeout_ms(&s->connecttimeout, arg);
#ifndef CURL_DISABLE_BINDLOCAL
  পক্ষ CURLOPT_LOCALPORT:
    যদি((arg < 0) || (arg > 65535))
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    s->localport = curlx_sltous(arg);
    ভঙ্গ;
  পক্ষ CURLOPT_LOCALPORTRANGE:
    যদি((arg < 0) || (arg > 65535))
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    s->localportrange = curlx_sltous(arg);
    ভঙ্গ;
#endif
  পক্ষ CURLOPT_BUFFERSIZE:
    result = মান_range(&arg, 0, READBUFFER_MIN, READBUFFER_MAX);
    যদি(!result)
      s->buffer_size = (unsigned পূর্ণাঙ্ক)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_UPLOAD_BUFFERSIZE:
    result = মান_range(&arg, 0, UPLOADBUFFER_MIN, UPLOADBUFFER_MAX);
    যদি(!result)
      s->upload_buffer_size = (unsigned পূর্ণাঙ্ক)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_MAXFILESIZE:
    যদি(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->max_filesize = arg;
    ভঙ্গ;
  পক্ষ CURLOPT_IPRESOLVE:
    যদি((arg < CURL_IPRESOLVE_WHATEVER) || (arg > CURL_IPRESOLVE_V6))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->ipver = (unsigned বর্ণ)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_CONNECT_ONLY:
    যদি(arg < 0 || arg > 2)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা {
      s->connect_only = !!arg;
      s->connect_only_ws = (arg == 2);
    }
    ভঙ্গ;
#ifdef USE_IPV6
  পক্ষ CURLOPT_ADDRESS_SCOPE:
#যদি SIZEOF_LONG > 4
    যদি((unsigned long)arg > UINT_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
#endif
    s->scope_id = (unsigned পূর্ণাঙ্ক)arg;
    ভঙ্গ;
#endif
  পক্ষ CURLOPT_TCP_KEEPIDLE:
    result = মান_range(&arg, 0, 0, INT_MAX);
    যদি(!result)
      s->tcp_keepidle = (পূর্ণাঙ্ক)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_TCP_KEEPINTVL:
    result = মান_range(&arg, 0, 0, INT_MAX);
    যদি(!result)
      s->tcp_keepintvl = (পূর্ণাঙ্ক)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_TCP_KEEPCNT:
    result = মান_range(&arg, 0, 0, INT_MAX);
    যদি(!result)
      s->tcp_keepcnt = (পূর্ণাঙ্ক)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
    প্রত্যাগম setopt_set_timeout_ms(&s->happy_eyeballs_timeout, arg);
  পক্ষ CURLOPT_UPKEEP_INTERVAL_MS:
    প্রত্যাগম setopt_set_timeout_ms(&s->upkeep_interval_ms, arg);
  পক্ষ CURLOPT_MAXAGE_CONN:
    প্রত্যাগম setopt_set_timeout_sec(&s->conn_max_idle_ms, arg);
  পক্ষ CURLOPT_MAXLIFETIME_CONN:
    প্রত্যাগম setopt_set_timeout_sec(&s->conn_max_age_ms, arg);
  পক্ষ CURLOPT_DNS_USE_GLOBAL_CACHE:
    /* deprecated */
    ভঙ্গ;
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }
  প্রত্যাগম result;
}

স্থির CURLcode setopt_long_ssl(সংরচনা Curl_easy *data, CURLoption option,
                                long arg)
{
#ifdef USE_SSL
  CURLcode result = CURLE_OK;
  সংরচনা UserDefined *s = &data->set;
  বিকল্প(option) {
  পক্ষ CURLOPT_CA_CACHE_TIMEOUT:
    যদি(Curl_ssl_supports(data, SSLSUPP_CA_CACHE)) {
      result = মান_range(&arg, -1, -1, INT_MAX);
      যদি(!result)
        s->ssl_ca_cache_timeout = (পূর্ণাঙ্ক)arg;
    }
    অন্যথা
      result = CURLE_NOT_BUILT_IN;
    ভঙ্গ;
  পক্ষ CURLOPT_SSLVERSION:
#ifndef CURL_DISABLE_PROXY
  পক্ষ CURLOPT_PROXY_SSLVERSION:
#endif
    প্রত্যাগম Curl_setopt_SSLVERSION(data, option, arg);
  পক্ষ CURLOPT_SSL_FALSESTART:
    result = CURLE_NOT_BUILT_IN;
    ভঙ্গ;
  পক্ষ CURLOPT_USE_SSL:
    যদি((arg < CURLUSESSL_NONE) || (arg >= CURLUSESSL_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->use_ssl = (unsigned বর্ণ)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_SSL_OPTIONS:
    s->ssl.ssl_options = (unsigned বর্ণ)(arg & 0xff);
    ভঙ্গ;
#ifndef CURL_DISABLE_PROXY
  পক্ষ CURLOPT_PROXY_SSL_OPTIONS:
    s->proxy_ssl.ssl_options = (unsigned বর্ণ)(arg & 0xff);
    ভঙ্গ;
#endif
  পক্ষ CURLOPT_SSL_ENABLE_NPN:
    ভঙ্গ;
  পক্ষ CURLOPT_SSLENGINE_DEFAULT:
#ifdef USE_OPENSSL
    যদি(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      প্রত্যাগম CURLE_NOT_BUILT_IN;
    data->set.engine_default = (arg == 1);
#অন্যথা
    প্রত্যাগম CURLE_NOT_BUILT_IN;
#endif
    ভঙ্গ;
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }
  প্রত্যাগম result;
#অন্যথা  /* USE_SSL */
  (শূন্য)data;
  (শূন্য)option;
  (শূন্য)arg;
  প্রত্যাগম CURLE_UNKNOWN_OPTION;
#endif /* !USE_SSL */
}

#ifndef CURL_DISABLE_PROXY
স্থির শূন্য changeproxy(সংরচনা Curl_easy *data)
{
  Curl_auth_digest_cleanup(&data->state.proxydigest);
  memset(&data->state.authproxy, 0, পরিমাণ(data->state.authproxy));
}

স্থির CURLcode setopt_long_proxy(সংরচনা Curl_easy *data, CURLoption option,
                                  long arg)
{
  সংরচনা UserDefined *s = &data->set;

  বিকল্প(option) {
  পক্ষ CURLOPT_PROXYPORT:
    যদি((arg < 0) || (arg > UINT16_MAX))
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    যদি(arg != s->proxyport)
      changeproxy(data);
    s->proxyport = (uint16_t)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_PROXYAUTH:
    প্রত্যাগম httpauth(data, TRUE, (unsigned long)arg);
  পক্ষ CURLOPT_PROXYTYPE:
    যদি((arg < CURLPROXY_HTTP) || (arg > CURLPROXY_HTTPS3))
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef USE_PROXY_HTTP3
    যদি(arg == CURLPROXY_HTTPS3)
      প্রত্যাগম CURLE_NOT_BUILT_IN;
#endif
    s->proxytype = (unsigned বর্ণ)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_SOCKS5_AUTH: {
    unsigned long auth = (unsigned long)arg;
    যদি(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI | CURLAUTH_ONLY))
      প্রত্যাগম CURLE_NOT_BUILT_IN;
    যদি((auth & CURLAUTH_ONLY) && !(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI)))
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    s->socks5auth = (unsigned বর্ণ)(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI));
    s->socks5_auth_only = !!(auth & CURLAUTH_ONLY);
    ভঙ্গ;
  }
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }
  প্রত্যাগম CURLE_OK;
}
#অন্যথা
স্থির CURLcode setopt_long_proxy(সংরচনা Curl_easy *data, CURLoption option,
                                  long arg)
{
  (শূন্য)data;
  (শূন্য)option;
  (শূন্য)arg;
  প্রত্যাগম CURLE_UNKNOWN_OPTION;
}
#endif

স্থির CURLcode setopt_long_http(সংরচনা Curl_easy *data, CURLoption option,
                                 long arg)
{
#ifndef CURL_DISABLE_HTTP
  CURLcode result = CURLE_OK;
  সংরচনা UserDefined *s = &data->set;

  বিকল্প(option) {
  পক্ষ CURLOPT_FOLLOWLOCATION:
    যদি((unsigned long)arg > 3)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->http_follow_mode = (unsigned বর্ণ)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_MAXREDIRS:
    result = মান_range(&arg, -1, -1, 0x7fff);
    যদি(!result)
      s->maxredirs = (short)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_POSTREDIR:
    যদি(arg < CURL_REDIR_GET_ALL)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা {
      s->post301 = !!(arg & CURL_REDIR_POST_301);
      s->post302 = !!(arg & CURL_REDIR_POST_302);
      s->post303 = !!(arg & CURL_REDIR_POST_303);
    }
    ভঙ্গ;
  পক্ষ CURLOPT_HEADEROPT:
    s->sep_headers = !!(arg & CURLHEADER_SEPARATE);
    ভঙ্গ;
  পক্ষ CURLOPT_HTTPAUTH:
    প্রত্যাগম httpauth(data, FALSE, (unsigned long)arg);
  পক্ষ CURLOPT_HTTP_VERSION:
    প্রত্যাগম setopt_HTTP_VERSION(data, arg);
  পক্ষ CURLOPT_EXPECT_100_TIMEOUT_MS:
    result = মান_range(&arg, 0, 0, 0xffff);
    যদি(!result)
      s->expect_100_timeout = (unsigned short)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_STREAM_WEIGHT:
#যদি defined(USE_HTTP2) || defined(USE_HTTP3)
    যদি((arg >= 1) && (arg <= 256))
      s->weight = (পূর্ণাঙ্ক)arg;
    ভঙ্গ;
#অন্যথা
    result = CURLE_NOT_BUILT_IN;
    ভঙ্গ;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  পক্ষ CURLOPT_HTTPSIG_ALGORITHM:
    যদি(arg != CURLHTTPSIG_NONE &&
       arg != CURLHTTPSIG_ED25519 &&
       arg != CURLHTTPSIG_HMAC_SHA256)
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    s->httpsig_algorithm = (uint8_t)arg;
    যদি(arg)
      s->httpauth = (uint32_t)CURLAUTH_HTTPSIG;
    অন্যথা
      s->httpauth &= ~(uint32_t)CURLAUTH_HTTPSIG;
    ভঙ্গ;
#endif
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }
  প্রত্যাগম result;
#অন্যথা
  (শূন্য)data;
  (শূন্য)option;
  (শূন্য)arg;
  প্রত্যাগম CURLE_UNKNOWN_OPTION;
#endif
}

স্থির CURLcode setopt_long_proto(সংরচনা Curl_easy *data, CURLoption option,
                                  long arg)
{
  CURLcode result = CURLE_OK;
  সংরচনা UserDefined *s = &data->set;

  বিকল্প(option) {
#ifndef CURL_DISABLE_TFTP
  পক্ষ CURLOPT_TFTP_BLKSIZE:
    result = মান_range(&arg, 0, TFTP_BLKSIZE_MIN, TFTP_BLKSIZE_MAX);
    যদি(!result)
      s->tftp_blksize = (unsigned short)arg;
    ভঙ্গ;
#endif
#ifndef CURL_DISABLE_NETRC
  পক্ষ CURLOPT_NETRC:
    যদি((arg < CURL_NETRC_IGNORED) || (arg >= CURL_NETRC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->use_netrc = (unsigned বর্ণ)arg;
    ভঙ্গ;
#endif
#ifndef CURL_DISABLE_FTP
  পক্ষ CURLOPT_FTP_FILEMETHOD:
    যদি((arg < CURLFTPMETHOD_DEFAULT) || (arg >= CURLFTPMETHOD_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->ftp_filemethod = (unsigned বর্ণ)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_FTP_SSL_CCC:
    যদি((arg < CURLFTPSSL_CCC_NONE) || (arg >= CURLFTPSSL_CCC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->ftp_ccc = (unsigned বর্ণ)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_FTPSSLAUTH:
    যদি((arg < CURLFTPAUTH_DEFAULT) || (arg >= CURLFTPAUTH_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->ftpsslauth = (unsigned বর্ণ)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_ACCEPTTIMEOUT_MS:
    প্রত্যাগম setopt_set_timeout_ms(&s->accepttimeout, arg);
#endif
#যদি !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  পক্ষ CURLOPT_FTP_CREATE_MISSING_DIRS:
    যদি((arg < CURLFTP_CREATE_DIR_NONE) || (arg > CURLFTP_CREATE_DIR_RETRY))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->ftp_create_missing_dirs = (unsigned বর্ণ)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_NEW_FILE_PERMS:
    যদি((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->new_file_perms = (unsigned পূর্ণাঙ্ক)arg;
    ভঙ্গ;
#endif
#ifndef CURL_DISABLE_RTSP
  পক্ষ CURLOPT_RTSP_REQUEST:
    প্রত্যাগম setopt_RTSP_REQUEST(data, arg);
  পক্ষ CURLOPT_RTSP_CLIENT_CSEQ:
    result = মান_range(&arg, 0, 0, INT_MAX);
    যদি(!result)
      data->state.rtsp_next_client_CSeq = (uint32_t)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_RTSP_SERVER_CSEQ:
    result = মান_range(&arg, 0, 0, INT_MAX);
    যদি(!result)
      data->state.rtsp_next_server_CSeq = (uint32_t)arg;
    ভঙ্গ;
#endif
#ifdef USE_SSH
  পক্ষ CURLOPT_SSH_AUTH_TYPES:
    s->ssh_auth_types = (uint32_t)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_NEW_DIRECTORY_PERMS:
    যদি((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->new_directory_perms = (unsigned পূর্ণাঙ্ক)arg;
    ভঙ্গ;
#endif
  পক্ষ CURLOPT_PROTOCOLS:
    s->allowed_protocols = (curl_prot_t)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_REDIR_PROTOCOLS:
    s->redir_protocols = (curl_prot_t)arg;
    ভঙ্গ;
#ifndef CURL_DISABLE_WEBSOCKETS
  পক্ষ CURLOPT_WS_OPTIONS:
    s->ws_raw_mode = !!(arg & CURLWS_RAW_MODE);
    s->ws_no_auto_pong = !!(arg & CURLWS_NOAUTOPONG);
    ভঙ্গ;
#endif
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }
  প্রত্যাগম result;
}

স্থির CURLcode setopt_long_misc(সংরচনা Curl_easy *data, CURLoption option,
                                 long arg)
{
  সংরচনা UserDefined *s = &data->set;

  বিকল্প(option) {
  পক্ষ CURLOPT_TIMECONDITION:
    যদি((arg < CURL_TIMECOND_NONE) || (arg >= CURL_TIMECOND_LAST))
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    s->timecondition = (unsigned বর্ণ)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_TIMEVALUE:
    s->timevalue = (time_t)arg;
    ভঙ্গ;
  পক্ষ CURLOPT_POSTFIELDSIZE:
    যদি(arg < -1)
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    যদি(s->postfieldsize < arg && s->str_copypostfields) {
      curlx_safefree(s->str_copypostfields);
      s->postfields = রিক্ত;
    }
    s->postfieldsize = arg;
    ভঙ্গ;
  পক্ষ CURLOPT_INFILESIZE:
    যদি(arg < -1)
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = arg;
    ভঙ্গ;
  পক্ষ CURLOPT_RESUME_FROM:
    যদি(arg < -1)
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = arg;
    ভঙ্গ;
  পক্ষ CURLOPT_UPLOAD_FLAGS:
    s->upload_flags = (unsigned বর্ণ)arg;
    ভঙ্গ;
#ifndef CURL_DISABLE_MIME
  পক্ষ CURLOPT_MIME_OPTIONS:
    s->mime_formescape = !!(arg & CURLMIMEOPT_FORMESCAPE);
    ভঙ্গ;
#endif
#ifndef CURL_DISABLE_HSTS
  পক্ষ CURLOPT_HSTS_CTRL:
    যদি(arg & CURLHSTS_ENABLE) {
      যদি(!data->hsts) {
        data->hsts = Curl_hsts_init();
        যদি(!data->hsts)
          প্রত্যাগম CURLE_OUT_OF_MEMORY;
      }
    }
    অন্যথা যদি(!data->share || !data->share->hsts) {
      /* throw away the HSTS cache unless shared */
      Curl_hsts_cleanup(&data->hsts);
      /* flush all the entries */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = রিক্ত;
    }
    অন্যথা
      /* detach from shared HSTS cache without freeing it */
      data->hsts = রিক্ত;
    ভঙ্গ;
#endif
#ifndef CURL_DISABLE_ALTSVC
  পক্ষ CURLOPT_ALTSVC_CTRL:
    প্রত্যাগম Curl_altsvc_ctrl(data, arg);
#endif
#যদি defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  পক্ষ CURLOPT_GSSAPI_DELEGATION:
    s->gssapi_delegation = (unsigned বর্ণ)arg &
      (CURLGSSAPI_DELEGATION_POLICY_FLAG | CURLGSSAPI_DELEGATION_FLAG);
    ভঙ্গ;
#endif
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode setopt_long(সংরচনা Curl_easy *data, CURLoption option,
                            long arg)
{
  প্রকারনির্দেশ CURLcode (*setoptfunc)(সংরচনা Curl_easy *data,
                                 CURLoption option, long arg);
  স্থির ধ্রুব setoptfunc setopt_call[] = {
    setopt_long_bool,
    setopt_long_net,
    setopt_long_http,
    setopt_long_proxy,
    setopt_long_ssl,
    setopt_long_proto,
    setopt_long_misc
  };
  size_t i;

  প্রতি(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, arg);
    যদি(result != CURLE_UNKNOWN_OPTION)
      প্রত্যাগম result;
  }
  প্রত্যাগম CURLE_UNKNOWN_OPTION;
}

স্থির CURLcode setopt_slist(সংরচনা Curl_easy *data, CURLoption option,
                             সংরচনা curl_slist *slist)
{
  CURLcode result = CURLE_OK;
  সংরচনা UserDefined *s = &data->set;
  বিকল্প(option) {
#ifndef CURL_DISABLE_PROXY
  পক্ষ CURLOPT_PROXYHEADER:
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
    ভঙ্গ;
#endif
#ifndef CURL_DISABLE_HTTP
  পক্ষ CURLOPT_HTTP200ALIASES:
    /*
     * Set a list of aliases for HTTP 200 in response header
     */
    s->http200aliases = slist;
    ভঙ্গ;
#endif
#যদি !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  পক্ষ CURLOPT_POSTQUOTE:
    /*
     * List of RAW FTP commands to use after a transfer
     */
    s->postquote = slist;
    ভঙ্গ;
  পক্ষ CURLOPT_PREQUOTE:
    /*
     * List of RAW FTP commands to use prior to RETR (Wesley Laxton)
     */
    s->prequote = slist;
    ভঙ্গ;
  পক্ষ CURLOPT_QUOTE:
    /*
     * List of RAW FTP commands to use before a transfer
     */
    s->quote = slist;
    ভঙ্গ;
#endif
  পক্ষ CURLOPT_RESOLVE:
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
    data->state.resolve = s->resolve;
    ভঙ্গ;
#যদি !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MIME)
  পক্ষ CURLOPT_HTTPHEADER:
    /*
     * Set a list with HTTP headers to use (or replace internals with)
     */
    s->headers = slist;
    ভঙ্গ;
#endif
#ifndef CURL_DISABLE_TELNET
  পক্ষ CURLOPT_TELNETOPTIONS:
    /*
     * Set a linked list of telnet options
     */
    s->telnet_options = slist;
    ভঙ্গ;
#endif
#ifndef CURL_DISABLE_SMTP
  পক্ষ CURLOPT_MAIL_RCPT:
    /* Set the list of mail recipients */
    s->mail_rcpt = slist;
    ভঙ্গ;
#endif
  পক্ষ CURLOPT_CONNECT_TO:
    s->connect_to = slist;
    ভঙ্গ;
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }
  প্রত্যাগম result;
}

#যদি !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
স্থির CURLcode setopt_mimepost(সংরচনা Curl_easy *data, curl_mime *mimep)
{
  /*
   * Set to make us do MIME POST
   */
  CURLcode result;
  সংরচনা UserDefined *s = &data->set;
  যদি(!s->mimepostp) {
    s->mimepostp = curlx_malloc(পরিমাণ(*s->mimepostp));
    যদি(!s->mimepostp)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    Curl_mime_initpart(s->mimepostp);
  }

  result = Curl_mime_set_subparts(s->mimepostp, mimep, FALSE);
  যদি(!result) {
    s->method = HTTPREQ_POST_MIME;
    s->opt_no_body = FALSE; /* this is implied */
#ifndef CURL_DISABLE_FORM_API
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = রিক্ত;
#endif
  }
  প্রত্যাগম result;
}
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */

স্থির CURLcode setopt_share(সংরচনা Curl_easy *data, সংরচনা Curl_share *set)
{
  CURLcode result;

  যদি(data->conn) {
    /* As this handle already has a connection attached, changing share now
       would be complicated and error-prone */
    infof(data, "Cannot change share object while in use");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
  }
  অন্যথা {
    /* disconnect from old share, if any and possible */
    result = Curl_share_easy_unlink(data);
    যদি(!result && GOOD_SHARE_HANDLE(set))
      /* use new share if it set */
      result = Curl_share_easy_link(data, set);
  }
  প্রত্যাগম result;
}

/* assorted pointer type arguments */
স্থির CURLcode setopt_pointers(সংরচনা Curl_easy *data, CURLoption option,
                                va_list param)
{
  CURLcode result = CURLE_OK;
  সংরচনা UserDefined *s = &data->set;
  বিকল্প(option) {
  পক্ষ CURLOPT_CURLU:
    /*
     * pass CURLU to set URL
     */
    Curl_bufref_free(&data->state.url);
    CURL_EASY_STR_CLEAR(data, STRING_SET_URL);
    s->uh = va_arg(param, CURLU *);
    ভঙ্গ;
#ifndef CURL_DISABLE_HTTP
#ifndef CURL_DISABLE_FORM_API
  পক্ষ CURLOPT_HTTPPOST:
    /*
     * Set to make us do HTTP POST. Legacy API-style.
     */
    s->httppost = va_arg(param, সংরচনা curl_httppost *);
    s->method = HTTPREQ_POST_FORM;
    s->opt_no_body = FALSE; /* this is implied */
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = রিক্ত;
    ভঙ্গ;
#endif /* !CURL_DISABLE_FORM_API */
#endif /* !CURL_DISABLE_HTTP */
#যদি !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
  পক্ষ CURLOPT_MIMEPOST:
    result = setopt_mimepost(data, va_arg(param, curl_mime *));
    ভঙ্গ;
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */
  পক্ষ CURLOPT_STDERR:
    /*
     * Set to a FILE * that should receive all error writes. This
     * defaults to stderr for normal operations.
     */
    s->err = va_arg(param, FILE *);
    যদি(!s->err)
      s->err = stderr;
    ভঙ্গ;
  পক্ষ CURLOPT_SHARE:
    প্রত্যাগম setopt_share(data, va_arg(param, সংরচনা Curl_share *));

  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }
  প্রত্যাগম result;
}

#ifndef CURL_DISABLE_COOKIES
স্থির CURLcode cookielist(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *ptr)
{
  CURLcode result = CURLE_OK;
  যদি(!ptr)
    প্রত্যাগম CURLE_OK;

  যদি(curl_strequal(ptr, "ALL")) {
    /* clear all cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearall(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  অন্যথা যদি(curl_strequal(ptr, "SESS")) {
    /* clear session cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearsess(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  অন্যথা যদি(curl_strequal(ptr, "FLUSH")) {
    /* flush cookies to file, takes care of the locking */
    Curl_flush_cookies(data, FALSE);
  }
  অন্যথা যদি(curl_strequal(ptr, "RELOAD")) {
    /* reload cookies from file */
    প্রত্যাগম Curl_cookie_loadfiles(data, COOKIE_NOPSL |
                                 (data->set.cookiesession ?
                                  COOKIE_NOSESSION : 0));
  }
  অন্যথা {
    যদি(!data->cookies) {
      /* if cookie engine was not running, activate it */
      data->cookies = Curl_cookie_init();
      যদি(!data->cookies)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
      data->state.cookie_engine = TRUE;
    }

    /* general protection against mistakes and abuse */
    যদি(strlen(ptr) > CURL_MAX_প্রবেশ_LENGTH)
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;

    /* Adding these cookies without the PSL check, because the PSL is not
       initialized until *perform() time, and this might be called before
       that */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    যদি(checkprefix("Set-Cookie:", ptr))
      /* HTTP Header format line */
      result = Curl_cookie_add(data, data->cookies, ptr + 11,
                               রিক্ত, রিক্ত,
                               COOKIE_HTTPHEADER | COOKIE_SECURE |
                               COOKIE_NOPSL);
    অন্যথা
      /* Netscape format line */
      result = Curl_cookie_add(data, data->cookies, ptr, রিক্ত,
                               রিক্ত, COOKIE_SECURE | COOKIE_NOPSL);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  প্রত্যাগম result;
}

স্থির CURLcode cookiefile(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *ptr)
{
  /*
   * Set cookie file to read and parse. Can be used multiple times.
   */
  যদি(ptr) {
    সংরচনা curl_slist *cl;
    /* general protection against mistakes and abuse */
    যদি(strlen(ptr) > CURL_MAX_প্রবেশ_LENGTH)
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    /* append the cookie filename to the list of filenames, and deal with
       them later */
    cl = curl_slist_append(data->state.cookielist, ptr);
    যদি(!cl) {
      curl_slist_free_all(data->state.cookielist);
      data->state.cookielist = রিক্ত;
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }
    data->state.cookielist = cl; /* store the list for later use */
  }
  অন্যথা {
    /* clear the list of cookie files */
    curl_slist_free_all(data->state.cookielist);
    data->state.cookielist = রিক্ত;

    যদি(!data->share || !data->share->cookies) {
      /* throw away all existing cookies if this is not a shared cookie
         container */
      Curl_cookie_clearall(data->cookies);
      Curl_cookie_cleanup(data->cookies);
    }
    /* disable the cookie engine */
    data->cookies = রিক্ত;
  }
  প্রত্যাগম CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_PROXY
স্থির CURLcode setproxy(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *proxy)
{
  ধ্রুব বর্ণ *str = CURL_EASY_STR(data, STRING_PROXY);
  যদি(str && proxy &&
     /* there was one set, is this a new one? */
     !strcmp(str, proxy))
    প্রত্যাগম CURLE_OK; /* same one as before */

  changeproxy(data);
  প্রত্যাগম Curl_setstropt(data, STRING_PROXY, proxy);
}

স্থির CURLcode setopt_cptr_proxy(সংরচনা Curl_easy *data, CURLoption option,
                                  বর্ণ *ptr)
{
  CURLcode result = CURLE_OK;
  সংরচনা UserDefined *s = &data->set;
  বিকল্প(option) {
  পক্ষ CURLOPT_PROXYUSERPWD: {
    /*
     * user:password needed to use the proxy
     */
    বর্ণ *u = রিক্ত;
    বর্ণ *p = রিক্ত;
    result = setstropt_userpwd(ptr, &u, &p);

    /* URL decode the components */
    যদি(!result) {
      বর্ণ *str = রিক্ত;
      CURL_EASY_STR_CLEAR(data, STRING_PROXYUSERNAME);
      CURL_EASY_STR_CLEAR(data, STRING_PROXYPASSWORD);
      যদি(u) {
        result = Curl_urldecode(u, 0, &str, রিক্ত, REJECT_ZERO);
        যদি(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYUSERNAME, str);
      }
      যদি(!result && p) {
        str = রিক্ত;
        result = Curl_urldecode(p, 0, &str, রিক্ত, REJECT_ZERO);
        যদি(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYPASSWORD, str);
      }
    }
    curlx_free(u);
    curlx_strzero(p);
    curlx_free(p);
    ভঙ্গ;
  }
  পক্ষ CURLOPT_PROXYUSERNAME:
    /*
     * authentication username to use in the operation
     */
    প্রত্যাগম Curl_setstropt(data, STRING_PROXYUSERNAME, ptr);

  পক্ষ CURLOPT_PROXYPASSWORD:
    /*
     * authentication password to use in the operation
     */
    প্রত্যাগম Curl_setstropt(data, STRING_PROXYPASSWORD, ptr);

  পক্ষ CURLOPT_NOPROXY:
    /*
     * proxy exception list
     */
    প্রত্যাগম Curl_setstropt(data, STRING_NOPROXY, ptr);
  পক্ষ CURLOPT_PROXY_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use for proxy
     */
    প্রত্যাগম Curl_setstropt(data, STRING_CERT_PROXY, ptr);
  পক্ষ CURLOPT_PROXY_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use for proxy
     */
    প্রত্যাগম Curl_setstropt(data, STRING_CERT_TYPE_PROXY, ptr);
  পক্ষ CURLOPT_PROXY_SSLKEY:
    /*
     * String that holds filename of the SSL key to use for proxy
     */
    প্রত্যাগম Curl_setstropt(data, STRING_চাবি_PROXY, ptr);
  পক্ষ CURLOPT_PROXY_KEYPASSWD:
    /*
     * String that holds the SSL private key password for proxy.
     */
    প্রত্যাগম Curl_setstropt(data, STRING_চাবি_PASSWD_PROXY, ptr);
  পক্ষ CURLOPT_PROXY_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use for proxy
     */
    প্রত্যাগম Curl_setstropt(data, STRING_চাবি_TYPE_PROXY, ptr);
  পক্ষ CURLOPT_PROXY_SSL_CIPHER_LIST:
    যদি(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST)) {
      /* set a list of cipher we want to use in the SSL connection for proxy */
      প্রত্যাগম Curl_setstropt(data, STRING_SSL_CIPHER_LIST_PROXY, ptr);
    }
    অন্যথা
      প্রত্যাগম CURLE_NOT_BUILT_IN;
  পক্ষ CURLOPT_PROXY_TLS13_CIPHERS:
    যদি(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites for proxy */
      প্রত্যাগম Curl_setstropt(data, STRING_SSL_CIPHER13_LIST_PROXY, ptr);
    অন্যথা
      প্রত্যাগম CURLE_NOT_BUILT_IN;
  পক্ষ CURLOPT_PROXY:
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
    প্রত্যাগম setproxy(data, ptr);
  পক্ষ CURLOPT_PRE_PROXY:
    /*
     * Set proxy server:port to use as SOCKS proxy.
     *
     * If the proxy is set to "" or NULL we explicitly say that we do not want
     * to use the socks proxy.
     */
    প্রত্যাগম Curl_setstropt(data, STRING_PRE_PROXY, ptr);
  পক্ষ CURLOPT_SOCKS5_GSSAPI_SERVICE:
  পক্ষ CURLOPT_PROXY_SERVICE_NAME:
    /*
     * Set proxy authentication service name for Kerberos 5 and SPNEGO
     */
    প্রত্যাগম Curl_setstropt(data, STRING_PROXY_SERVICE_NAME, ptr);
  পক্ষ CURLOPT_PROXY_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
#ifdef USE_SSL
    যদি(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      প্রত্যাগম Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY_PROXY, ptr);
#endif
    প্রত্যাগম CURLE_NOT_BUILT_IN;

  পক্ষ CURLOPT_HAPROXY_CLIENT_IP:
    /*
     * Set the client IP to send through HAProxy PROXY protocol
     */
    result = Curl_setstropt(data, STRING_HAPROXY_CLIENT_IP, ptr);

    /* enable the HAProxy protocol if an IP is provided */
    s->haproxyprotocol = !!CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
    ভঙ্গ;
  পক্ষ CURLOPT_PROXY_CAINFO:
    /*
     * Set CA info SSL connection for proxy. Specify filename of the
     * CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE_PROXY, ptr);
    যদি(!result)
      s->proxy_ssl.custom_cafile = TRUE;
    প্রত্যাগম result;
  পক্ষ CURLOPT_PROXY_CRLFILE:
    /*
     * Set CRL file info for SSL connection for proxy. Specify filename of the
     * CRL to check certificates revocation
     */
    যদি(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      প্রত্যাগম Curl_setstropt(data, STRING_SSL_CRLFILE_PROXY, ptr);
    প্রত্যাগম CURLE_NOT_BUILT_IN;
  পক্ষ CURLOPT_PROXY_ISSUERCERT:
    /*
     * Set Issuer certificate file to check certificates issuer
     */
    যদি(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      প্রত্যাগম Curl_setstropt(data, STRING_SSL_ISSUERCERT_PROXY, ptr);
    প্রত্যাগম CURLE_NOT_BUILT_IN;
  পক্ষ CURLOPT_PROXY_CAPATH:
    /*
     * Set CA path info for SSL connection proxy. Specify directory name of the
     * CA certificates which have been prepared using openssl c_rehash utility.
     */
#ifdef USE_SSL
    যদি(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH_PROXY, ptr);
      যদি(!result)
        s->proxy_ssl.custom_capath = TRUE;
      প্রত্যাগম result;
    }
#endif
    প্রত্যাগম CURLE_NOT_BUILT_IN;
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }
  প্রত্যাগম result;
}
#endif

#যদি !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
/*
 * A string with POST data. Makes curl HTTP POST. Even if it is NULL. If
 * needed, CURLOPT_POSTFIELDSIZE must have been set prior to
 * CURLOPT_COPYPOSTFIELDS and not altered later.
 */
স্থির CURLcode setopt_copypostfields(ধ্রুব বর্ণ *ptr, সংরচনা UserDefined *s)
{
  CURLcode result = CURLE_OK;
  যদি(s->postfieldsize < -1)
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  যদি(!ptr || s->postfieldsize == -1) {
    যদি(ptr && (strlen(ptr) > CURL_MAX_প্রবেশ_LENGTH))
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_safefree(s->str_copypostfields);
    যদি(ptr) {
      s->str_copypostfields = curlx_strdup(ptr);
      যদি(!s->str_copypostfields)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }
  }
  অন্যথা {
    size_t pflen = curlx_sotouz_range(s->postfieldsize, 0, SIZE_MAX);
    যদি(pflen == SIZE_MAX)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    অন্যথা {
      /* Allocate even when size == 0. This satisfies the need of possible
         later address compare to detect the COPYPOSTFIELDS mode, and to mark
         that postfields is used rather than read function or form data. */
      বর্ণ *p = curlx_memdup0(ptr, pflen);
      যদি(!p)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
      অন্যথা {
        curlx_free(s->str_copypostfields);
        s->str_copypostfields = p;
      }
    }
  }

  s->postfields = s->str_copypostfields;
  s->method = HTTPREQ_POST;
  প্রত্যাগম result;
}
#endif

#ifdef USE_ECH
স্থির CURLcode setopt_ech(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *ptr)
{
  সংরচনা UserDefined *s = &data->set;
  CURLcode result = CURLE_OK;

  যদি(!ptr || !strcmp(ptr, "false"))
    s->ssl.ech = CURLECH_DISABLE;
  অন্যথা {
    size_t plen = strlen(ptr);
    যদি(plen > CURL_MAX_প্রবেশ_LENGTH)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা {
      যদি(!strcmp(ptr, "grease"))
        s->ssl.ech = CURLECH_GREASE;
      অন্যথা যদি(!strcmp(ptr, "true"))
        s->ssl.ech = CURLECH_ENABLE;
      অন্যথা যদি(!strcmp(ptr, "hard"))
        s->ssl.ech = CURLECH_HARD;
      অন্যথা যদি(plen > 4 && !strncmp(ptr, "ecl:", 4)) {
        যদি(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_CONFIG, ptr + 4);
      }
      অন্যথা যদি(plen > 3 && !strncmp(ptr, "pn:", 3)) {
        যদি(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_PUBLIC, ptr + 3);
      }
      অন্যথা
        result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  প্রত্যাগম result;
}
#অন্যথা
#define setopt_ech(x, y) CURLE_NOT_BUILT_IN
#endif

#যদি defined(USE_SSL) || defined(USE_SSH)
/* One of the options is used for both TLS and SSH */
স্থির CURLcode setopt_cptr_ssl(সংরচনা Curl_easy *data, CURLoption option,
                                বর্ণ *ptr)
{
  CURLcode result = CURLE_OK;

  বিকল্প(option) {
  পক্ষ CURLOPT_KEYPASSWD:
    /*
     * String that holds the SSL or SSH private key password.
     */
    result = Curl_setstropt(data, STRING_চাবি_PASSWD, ptr);
    ভঙ্গ;
#ifdef USE_SSL
  পক্ষ CURLOPT_CAINFO:
    /*
     * Set CA info for SSL connection. Specify filename of the CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE, ptr);
    যদি(!result)
      data->set.ssl.custom_cafile = TRUE;
    প্রত্যাগম result;
  পক্ষ CURLOPT_CAPATH:
    /*
     * Set CA path info for SSL connection. Specify directory name of the CA
     * certificates which have been prepared using openssl c_rehash utility.
     */
    যদি(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH, ptr);
      যদি(!result)
        data->set.ssl.custom_capath = TRUE;
      প্রত্যাগম result;
    }
    প্রত্যাগম CURLE_NOT_BUILT_IN;
  পক্ষ CURLOPT_CRLFILE:
    /*
     * Set CRL file info for SSL connection. Specify filename of the CRL
     * to check certificates revocation
     */
    যদি(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      প্রত্যাগম Curl_setstropt(data, STRING_SSL_CRLFILE, ptr);
    প্রত্যাগম CURLE_NOT_BUILT_IN;
  পক্ষ CURLOPT_SSL_CIPHER_LIST:
    যদি(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST))
      /* set a list of cipher we want to use in the SSL connection */
      প্রত্যাগম Curl_setstropt(data, STRING_SSL_CIPHER_LIST, ptr);
    অন্যথা
      প্রত্যাগম CURLE_NOT_BUILT_IN;
  পক্ষ CURLOPT_TLS13_CIPHERS:
    যদি(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites */
      প্রত্যাগম Curl_setstropt(data, STRING_SSL_CIPHER13_LIST, ptr);
    অন্যথা
      প্রত্যাগম CURLE_NOT_BUILT_IN;
  পক্ষ CURLOPT_RANDOM_FILE:
    ভঙ্গ;
  পক্ষ CURLOPT_EGDSOCKET:
    ভঙ্গ;
  পক্ষ CURLOPT_SSL_CTX_DATA:
    /*
     * Set an SSL_CTX callback parameter pointer
     */
    যদি(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      data->set.ssl_fsslctxp = ptr;
      ভঙ্গ;
    }
    অন্যথা
      প্রত্যাগম CURLE_NOT_BUILT_IN;
  পক্ষ CURLOPT_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use
     */
    প্রত্যাগম Curl_setstropt(data, STRING_CERT, ptr);
  পক্ষ CURLOPT_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use
     */
    প্রত্যাগম Curl_setstropt(data, STRING_CERT_TYPE, ptr);
  পক্ষ CURLOPT_SSLKEY:
    /*
     * String that holds filename of the SSL key to use
     */
    প্রত্যাগম Curl_setstropt(data, STRING_চাবি, ptr);
  পক্ষ CURLOPT_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use
     */
    প্রত্যাগম Curl_setstropt(data, STRING_চাবি_TYPE, ptr);
  পক্ষ CURLOPT_SSLENGINE:
    /*
     * String that holds the SSL crypto engine.
     */
    যদি(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      প্রত্যাগম CURLE_NOT_BUILT_IN;
    result = Curl_setstropt(data, STRING_SSL_ENGINE, ptr);
    ভঙ্গ;
  পক্ষ CURLOPT_ISSUERCERT:
    /*
     * Set Issuer certificate file
     * to check certificates issuer
     */
    যদি(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      প্রত্যাগম Curl_setstropt(data, STRING_SSL_ISSUERCERT, ptr);
    প্রত্যাগম CURLE_NOT_BUILT_IN;
  পক্ষ CURLOPT_SSL_EC_CURVES:
    /*
     * Set accepted curves in SSL connection setup.
     * Specify colon-delimited list of curve algorithm names.
     */
    যদি(Curl_ssl_supports(data, SSLSUPP_SSL_EC_CURVES))
      প্রত্যাগম Curl_setstropt(data, STRING_SSL_EC_CURVES, ptr);
    প্রত্যাগম CURLE_NOT_BUILT_IN;
  পক্ষ CURLOPT_SSL_SIGNATURE_ALGORITHMS:
    /*
     * Set accepted signature algorithms.
     * Specify colon-delimited list of signature scheme names.
     */
    যদি(Curl_ssl_supports(data, SSLSUPP_SIGNATURE_ALGORITHMS))
      প্রত্যাগম Curl_setstropt(data, STRING_SSL_SIGNATURE_ALGORITHMS, ptr);
    প্রত্যাগম CURLE_NOT_BUILT_IN;
  পক্ষ CURLOPT_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
    যদি(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      প্রত্যাগম Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY, ptr);
    প্রত্যাগম CURLE_NOT_BUILT_IN;
  পক্ষ CURLOPT_ECH:
    প্রত্যাগম setopt_ech(data, ptr);
#endif
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }
  প্রত্যাগম result;
}
#endif

#যদি !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
স্থির CURLcode setopt_cptr_http_mqtt(সংরচনা Curl_easy *data,
                                      CURLoption option, বর্ণ *ptr)
{
  CURLcode result = CURLE_OK;
  সংরচনা UserDefined *s = &data->set;

  বিকল্প(option) {
  পক্ষ CURLOPT_COPYPOSTFIELDS:
    প্রত্যাগম setopt_copypostfields(ptr, s);

  পক্ষ CURLOPT_POSTFIELDS:
    /*
     * Like above, but use static data instead of copying it.
     */
    s->postfields = ptr;
    /* Release old copied data. */
    curlx_safefree(s->str_copypostfields);
    s->method = HTTPREQ_POST;
    ভঙ্গ;

#ifndef CURL_DISABLE_HTTP
  পক্ষ CURLOPT_TRAILERDATA:
    s->trailer_data = ptr;
    ভঙ্গ;
  পক্ষ CURLOPT_ACCEPT_ENCODING:
    /*
     * String to use at the value of Accept-Encoding header.
     *
     * If the encoding is set to "" we use an Accept-Encoding header that
     * encompasses all the encodings we support.
     * If the encoding is set to NULL we do not send an Accept-Encoding header
     * and ignore any received Content-Encoding header.
     *
     */
    যদি(ptr && !*ptr) {
      ptr = Curl_get_content_encodings();
      যদি(ptr)
        result = CURL_EASY_STR_SETN(data, STRING_ENCODING, ptr);
      অন্যথা
        result = CURLE_OUT_OF_MEMORY;
      প্রত্যাগম result;
    }
    প্রত্যাগম Curl_setstropt(data, STRING_ENCODING, ptr);

#ifndef CURL_DISABLE_AWS
  পক্ষ CURLOPT_AWS_SIGV4:
    /*
     * String that is merged to some authentication
     * parameters are used by the algorithm.
     */
    result = Curl_setstropt(data, STRING_AWS_SIGV4, ptr);
    /*
     * Basic has been set by default; it needs to be unset here.
     */
    যদি(CURL_EASY_STR(data, STRING_AWS_SIGV4))
      s->httpauth = CURLAUTH_AWS_SIGV4;
    অন্যথা
      s->httpauth &= ~(uint32_t)CURLAUTH_AWS_SIGV4;
    ভঙ্গ;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  পক্ষ CURLOPT_HTTPSIG_চাবি:
    result = Curl_setstropt(data, STRING_HTTPSIG_চাবি, ptr);
    ভঙ্গ;
  পক্ষ CURLOPT_HTTPSIG_KEYID:
    result = Curl_setstropt(data, STRING_HTTPSIG_KEYID, ptr);
    ভঙ্গ;
  পক্ষ CURLOPT_HTTPSIG_HEADERS:
    result = Curl_setstropt(data, STRING_HTTPSIG_HEADERS, ptr);
    ভঙ্গ;
#endif
  পক্ষ CURLOPT_REFERER: {
    /*
     * String to set in the HTTP Referer: field.
     */
    সংরচনা bufref *oldref = &data->state.referer;
    /* free the old after the storing the new in case the input is actually
       pointing back to this */
    result = Curl_setstropt(data, STRING_SET_REFERER, ptr);
    Curl_bufref_free(oldref);
    ভঙ্গ;
  }

  পক্ষ CURLOPT_USERAGENT:
    /*
     * String to use in the HTTP User-Agent field
     */
    প্রত্যাগম Curl_setstropt(data, STRING_USERAGENT, ptr);

#ifndef CURL_DISABLE_COOKIES
  পক্ষ CURLOPT_COOKIE:
    /*
     * Cookie string to send to the remote server in the request.
     */
    প্রত্যাগম Curl_setstropt(data, STRING_COOKIE, ptr);

  পক্ষ CURLOPT_COOKIEFILE:
    প্রত্যাগম cookiefile(data, ptr);

  পক্ষ CURLOPT_COOKIEJAR:
    /*
     * Set cookie filename to dump all cookies to when we are done.
     */
    result = Curl_setstropt(data, STRING_COOKIEJAR, ptr);
    যদি(!result) {
      /*
       * Activate the cookie parser. This may or may not already
       * have been made.
       */
      যদি(!data->cookies)
        data->cookies = Curl_cookie_init();
      যদি(!data->cookies)
        result = CURLE_OUT_OF_MEMORY;
      অন্যথা
        data->state.cookie_engine = TRUE;
    }
    ভঙ্গ;

  পক্ষ CURLOPT_COOKIELIST:
    প্রত্যাগম cookielist(data, ptr);
#endif /* !CURL_DISABLE_COOKIES */

#endif /* !CURL_DISABLE_HTTP */
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }
  প্রত্যাগম result;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_MQTT */

#ifdef USE_SSH
স্থির CURLcode setopt_cptr_ssh(সংরচনা Curl_easy *data, CURLoption option,
                                বর্ণ *ptr)
{
  সংরচনা UserDefined *s = &data->set;
  বিকল্প(option) {
  পক্ষ CURLOPT_SSH_PUBLIC_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa.pub file
     */
    প্রত্যাগম Curl_setstropt(data, STRING_SSH_PUBLIC_চাবি, ptr);
  পক্ষ CURLOPT_SSH_PRIVATE_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa file
     */
    প্রত্যাগম Curl_setstropt(data, STRING_SSH_PRIVATE_চাবি, ptr);
  পক্ষ CURLOPT_SSH_KEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_keyfunc_userp = ptr;
    ভঙ্গ;
  পক্ষ CURLOPT_SSH_HOST_PUBLIC_চাবি_MD5:
    /*
     * Option to allow for the MD5 of the host public key to be checked
     * for validation purposes.
     */
    প্রত্যাগম Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_চাবি_MD5, ptr);
  পক্ষ CURLOPT_SSH_HOST_PUBLIC_চাবি_SHA256:
    /*
     * Option to allow for the SHA256 of the host public key to be checked
     * for validation purposes.
     */
    প্রত্যাগম Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_চাবি_SHA256, ptr);
  পক্ষ CURLOPT_SSH_KNOWNHOSTS:
    /*
     * Store the filename to read known hosts from.
     */
    প্রত্যাগম Curl_setstropt(data, STRING_SSH_KNOWNHOSTS, ptr);
#ifdef USE_LIBSSH2
  পক্ষ CURLOPT_SSH_HOSTKEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_hostkeyfunc_userp = ptr;
    ভঙ্গ;
#endif /* USE_LIBSSH2 */
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }
  প্রত্যাগম CURLE_OK;
}
#endif /* USE_SSH */

#ifndef CURL_DISABLE_FTP
স্থির CURLcode setopt_cptr_ftp(সংরচনা Curl_easy *data, CURLoption option,
                                বর্ণ *ptr)
{
  CURLcode result = CURLE_OK;
  সংরচনা UserDefined *s = &data->set;
  বিকল্প(option) {
  পক্ষ CURLOPT_FTPPORT:
    /*
     * Use FTP PORT, this also specifies which IP address to use
     */
    result = Curl_setstropt(data, STRING_FTPPORT, ptr);
    s->ftp_use_port = !!CURL_EASY_STR(data, STRING_FTPPORT);
    ভঙ্গ;

  পক্ষ CURLOPT_FTP_ACCOUNT:
    প্রত্যাগম Curl_setstropt(data, STRING_FTP_ACCOUNT, ptr);

  পক্ষ CURLOPT_FTP_ALTERNATIVE_TO_USER:
    প্রত্যাগম Curl_setstropt(data, STRING_FTP_ALTERNATIVE_TO_USER, ptr);

  পক্ষ CURLOPT_KRBLEVEL:
    প্রত্যাগম CURLE_NOT_BUILT_IN; /* removed in 8.17.0 */
  পক্ষ CURLOPT_CHUNK_DATA:
    s->wildcardptr = ptr;
    ভঙ্গ;
  পক্ষ CURLOPT_FNMATCH_DATA:
    s->fnmatch_data = ptr;
    ভঙ্গ;
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }
  প্রত্যাগম result;
}
#endif /* !CURL_DISABLE_FTP */

স্থির CURLcode setopt_cptr_net(সংরচনা Curl_easy *data, CURLoption option,
                                বর্ণ *ptr)
{
  বিকল্প(option) {
  পক্ষ CURLOPT_INTERFACE:
    /*
     * Set what interface or address/hostname to bind the socket to when
     * performing an operation and thus what from-IP your connection will use.
     */
    প্রত্যাগম setstropt_interface(data, ptr);

#ifdef USE_RESOLV_ARES
  পক্ষ CURLOPT_DNS_SERVERS:
    প্রত্যাগম Curl_setstropt(data, STRING_DNS_SERVERS, ptr);

  পক্ষ CURLOPT_DNS_INTERFACE:
    প্রত্যাগম Curl_setstropt(data, STRING_DNS_INTERFACE, ptr);

  পক্ষ CURLOPT_DNS_LOCAL_IP4:
    প্রত্যাগম Curl_setstropt(data, STRING_DNS_LOCAL_IP4, ptr);

  পক্ষ CURLOPT_DNS_LOCAL_IP6:
    প্রত্যাগম Curl_setstropt(data, STRING_DNS_LOCAL_IP6, ptr);
#endif
#ifdef USE_UNIX_SOCKETS
  পক্ষ CURLOPT_UNIX_SOCKET_PATH:
    data->set.abstract_unix_socket = FALSE;
    প্রত্যাগম Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);

  পক্ষ CURLOPT_ABSTRACT_UNIX_SOCKET:
    data->set.abstract_unix_socket = TRUE;
    প্রত্যাগম Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);
#endif
#ifndef CURL_DISABLE_DOH
  পক্ষ CURLOPT_DOH_URL:
    {
      CURLcode result = Curl_setstropt(data, STRING_DOH, ptr);
      data->set.doh = !!CURL_EASY_STR(data, STRING_DOH);
      প্রত্যাগম result;
    }
#endif
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }
}

স্থির CURLcode setopt_cptr_misc(সংরচনা Curl_easy *data, CURLoption option,
                                 বর্ণ *ptr)
{
  CURLcode result = CURLE_OK;
  সংরচনা UserDefined *s = &data->set;

  বিকল্প(option) {
  পক্ষ CURLOPT_REQUEST_TARGET:
    প্রত্যাগম Curl_setstropt(data, STRING_TARGET, ptr);
#ifndef CURL_DISABLE_NETRC
  পক্ষ CURLOPT_NETRC_FILE:
    প্রত্যাগম Curl_setstropt(data, STRING_NETRC_FILE, ptr);
#endif
  পক্ষ CURLOPT_CUSTOMREQUEST:
    প্রত্যাগম Curl_setstropt(data, STRING_CUSTOMREQUEST, ptr);

    /* we do not set s->method = HTTPREQ_CUSTOM; here, we continue as if we
       were using the already set type and this changes the actual request
       keyword */
  পক্ষ CURLOPT_SERVICE_NAME:
    প্রত্যাগম Curl_setstropt(data, STRING_SERVICE_NAME, ptr);

  পক্ষ CURLOPT_HEADERDATA:
    s->writeheader = ptr;
    ভঙ্গ;
  পক্ষ CURLOPT_READDATA:
    s->in_set = ptr;
    ভঙ্গ;
  পক্ষ CURLOPT_WRITEDATA:
    s->out = ptr;
    ভঙ্গ;
  পক্ষ CURLOPT_DEBUGDATA:
    s->debugdata = ptr;
    ভঙ্গ;
  পক্ষ CURLOPT_PROGRESSDATA:
    s->progress_client = ptr;
    ভঙ্গ;
  পক্ষ CURLOPT_SEEKDATA:
    s->seek_client = ptr;
    ভঙ্গ;
  পক্ষ CURLOPT_IOCTLDATA:
    s->ioctl_client = ptr;
    ভঙ্গ;
  পক্ষ CURLOPT_SOCKOPTDATA:
    s->sockopt_client = ptr;
    ভঙ্গ;
  পক্ষ CURLOPT_OPENSOCKETDATA:
    s->opensocket_client = ptr;
    ভঙ্গ;
  পক্ষ CURLOPT_RESOLVER_START_DATA:
    s->resolver_start_client = ptr;
    ভঙ্গ;
  পক্ষ CURLOPT_CLOSESOCKETDATA:
    s->closesocket_client = ptr;
    ভঙ্গ;
  পক্ষ CURLOPT_PREREQDATA:
    s->prereq_userp = ptr;
    ভঙ্গ;
  পক্ষ CURLOPT_ERRORBUFFER:
    s->errorbuffer = ptr;
    ভঙ্গ;
  পক্ষ CURLOPT_URL:
    result = Curl_setstropt(data, STRING_SET_URL, ptr);
    Curl_bufref_set(&data->state.url,
                    CURL_EASY_STR(data, STRING_SET_URL), 0, রিক্ত);
    ভঙ্গ;

  পক্ষ CURLOPT_USERPWD: {
    বর্ণ *u = রিক্ত, *p = রিক্ত;
    result = setstropt_userpwd(ptr, &u, &p);
    যদি(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_USERNAME, u);
      u = রিক্ত;
    }
    যদি(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_PASSWORD, p);
      p = রিক্ত;
    }
    curlx_free(u);
    curlx_free(p);
    প্রত্যাগম result;
  }

  পক্ষ CURLOPT_USERNAME:
    প্রত্যাগম Curl_setstropt(data, STRING_USERNAME, ptr);

  পক্ষ CURLOPT_PASSWORD:
    প্রত্যাগম Curl_setstropt(data, STRING_PASSWORD, ptr);

  পক্ষ CURLOPT_LOGIN_OPTIONS:
    প্রত্যাগম Curl_setstropt(data, STRING_OPTIONS, ptr);

  পক্ষ CURLOPT_XOAUTH2_BEARER:
    প্রত্যাগম Curl_setstropt(data, STRING_BEARER, ptr);
  পক্ষ CURLOPT_RANGE:
    প্রত্যাগম Curl_setstropt(data, STRING_SET_RANGE, ptr);
  পক্ষ CURLOPT_PRIVATE:
    s->private_data = ptr;
    ভঙ্গ;
  পক্ষ CURLOPT_PROTOCOLS_STR:
    যদি(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      যদি(!result)
        s->allowed_protocols = protos;
    }
    অন্যথা
      /* make a NULL argument reset to default */
      s->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
    ভঙ্গ;
  পক্ষ CURLOPT_REDIR_PROTOCOLS_STR:
    যদি(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      যদি(!result)
        s->redir_protocols = protos;
    }
    অন্যথা
      /* make a NULL argument reset to default */
      s->redir_protocols = (curl_prot_t)CURLPROTO_REDIR;
    ভঙ্গ;
  পক্ষ CURLOPT_DEFAULT_PROTOCOL:
    /* Set the protocol to use when the URL does not include any protocol */
    প্রত্যাগম Curl_setstropt(data, STRING_DEFAULT_PROTOCOL, ptr);
#ifndef CURL_DISABLE_SMTP
  পক্ষ CURLOPT_MAIL_FROM:
    /* Set the SMTP mail originator */
    প্রত্যাগম Curl_setstropt(data, STRING_MAIL_FROM, ptr);
  পক্ষ CURLOPT_MAIL_AUTH:
    /* Set the SMTP auth originator */
    প্রত্যাগম Curl_setstropt(data, STRING_MAIL_AUTH, ptr);
#endif
  পক্ষ CURLOPT_SASL_AUTHZID:
    /* Authorization identity (identity to act as) */
    প্রত্যাগম Curl_setstropt(data, STRING_SASL_AUTHZID, ptr);
#ifndef CURL_DISABLE_RTSP
  পক্ষ CURLOPT_RTSP_SESSION_ID:
    Curl_peer_unlink(&data->state.rtsp_session_origin);
    প্রত্যাগম Curl_setstropt(data, STRING_RTSP_SESSION_ID, ptr);
  পক্ষ CURLOPT_RTSP_STREAM_URI:
    প্রত্যাগম Curl_setstropt(data, STRING_RTSP_STREAM_URI, ptr);
  পক্ষ CURLOPT_RTSP_TRANSPORT:
    প্রত্যাগম Curl_setstropt(data, STRING_RTSP_TRANSPORT, ptr);
  পক্ষ CURLOPT_INTERLEAVEDATA:
    s->rtp_out = ptr;
    ভঙ্গ;
#endif /* !CURL_DISABLE_RTSP */
  পক্ষ CURLOPT_TLSAUTH_USERNAME:
  পক্ষ CURLOPT_TLSAUTH_PASSWORD:
  পক্ষ CURLOPT_TLSAUTH_TYPE:
  পক্ষ CURLOPT_PROXY_TLSAUTH_USERNAME:
  পক্ষ CURLOPT_PROXY_TLSAUTH_PASSWORD:
  পক্ষ CURLOPT_PROXY_TLSAUTH_TYPE:
    প্রত্যাগম CURLE_NOT_BUILT_IN;
#ifndef CURL_DISABLE_HSTS
  পক্ষ CURLOPT_HSTSREADDATA:
    s->hsts_read_userp = ptr;
    ভঙ্গ;
  পক্ষ CURLOPT_HSTSWRITEDATA:
    s->hsts_write_userp = ptr;
    ভঙ্গ;
  পক্ষ CURLOPT_HSTS: {
    সংরচনা curl_slist *h;
    যদি(!data->hsts) {
      data->hsts = Curl_hsts_init();
      যদি(!data->hsts)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }
    যদি(ptr) {
      result = Curl_setstropt(data, STRING_HSTS, ptr);
      যদি(result)
        প্রত্যাগম result;
      /* this needs to build a list of filenames to read from, so that it can
         read them later, as we might get a shared HSTS handle to load them
         into */
      h = curl_slist_append(data->state.hstslist, ptr);
      যদি(!h) {
        curl_slist_free_all(data->state.hstslist);
        data->state.hstslist = রিক্ত;
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
      }
      data->state.hstslist = h; /* store the list for later use */
    }
    অন্যথা {
      /* clear the list of HSTS files */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = রিক্ত;
      যদি(!data->share || !data->share->hsts)
        /* throw away the HSTS cache unless shared */
        Curl_hsts_cleanup(&data->hsts);
    }
    ভঙ্গ;
  }
#endif /* !CURL_DISABLE_HSTS */
#ifndef CURL_DISABLE_ALTSVC
  পক্ষ CURLOPT_ALTSVC:
    যদি(!data->asi) {
      data->asi = Curl_altsvc_init();
      যদি(!data->asi)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }
    result = Curl_setstropt(data, STRING_ALTSVC, ptr);
    যদি(result)
      ভঙ্গ;
    যদি(ptr)
      প্রত্যাগম Curl_altsvc_load(data->asi, ptr);
    ভঙ্গ;
#endif /* !CURL_DISABLE_ALTSVC */
  পক্ষ CURLOPT_ECH:
    প্রত্যাগম setopt_ech(data, ptr);
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }
  প্রত্যাগম result;
}

স্থির CURLcode setopt_cptr(সংরচনা Curl_easy *data, CURLoption option,
                            বর্ণ *ptr)
{
  প্রকারনির্দেশ CURLcode (*ptrfunc)(সংরচনা Curl_easy *data, CURLoption option,
                              বর্ণ *ptr);
  /* Order by likeliness */
  স্থির ধ্রুব ptrfunc setopt_call[] = {
    setopt_cptr_misc,
#যদি defined(USE_SSL) || defined(USE_SSH)
    setopt_cptr_ssl,
#endif
#ifndef CURL_DISABLE_PROXY
    setopt_cptr_proxy,
#endif
    setopt_cptr_net,
#ifndef CURL_DISABLE_FTP
    setopt_cptr_ftp,
#endif
#ifdef USE_SSH
    setopt_cptr_ssh,
#endif
#যদি !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
    setopt_cptr_http_mqtt,
#endif
  };
  size_t i;

  প্রতি(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, ptr);
    যদি(result != CURLE_UNKNOWN_OPTION)
      প্রত্যাগম result;
  }
  প্রত্যাগম CURLE_UNKNOWN_OPTION;
}

স্থির CURLcode setopt_func(সংরচনা Curl_easy *data, CURLoption option,
                            va_list param)
{
  সংরচনা UserDefined *s = &data->set;
  বিকল্প(option) {
  পক্ষ CURLOPT_PROGRESSFUNCTION:
    /*
     * Progress callback function
     */
    s->fprogress = va_arg(param, curl_progress_callback);
    যদি(s->fprogress)
      data->progress.callback = TRUE; /* no longer internal */
    অন্যথা
      data->progress.callback = FALSE; /* NULL enforces internal */
    ভঙ্গ;

  পক্ষ CURLOPT_XFERINFOFUNCTION:
    /*
     * Transfer info callback function
     */
    s->fxferinfo = va_arg(param, curl_xferinfo_callback);
    যদি(s->fxferinfo)
      data->progress.callback = TRUE; /* no longer internal */
    অন্যথা
      data->progress.callback = FALSE; /* NULL enforces internal */

    ভঙ্গ;
  পক্ষ CURLOPT_DEBUGFUNCTION:
    /*
     * stderr write callback.
     */
    s->fdebug = va_arg(param, curl_debug_callback);
    /*
     * if the callback provided is NULL, it will use the default callback
     */
    ভঙ্গ;
  পক্ষ CURLOPT_HEADERFUNCTION:
    /*
     * Set header write callback
     */
    s->fwrite_header = va_arg(param, curl_write_callback);
    ভঙ্গ;
  পক্ষ CURLOPT_WRITEFUNCTION:
    /*
     * Set data write callback
     */
    s->fwrite_func = va_arg(param, curl_write_callback);
    যদি(!s->fwrite_func)
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ঠেলো
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fwrite_func = (curl_write_callback)fwrite;
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic বের_করো
#endif
    ভঙ্গ;
  পক্ষ CURLOPT_READFUNCTION:
    /*
     * Read data callback
     */
    s->fread_func_set = va_arg(param, curl_read_callback);
    যদি(!s->fread_func_set) {
      s->is_fread_set = 0;
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ঠেলো
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fread_func_set = (curl_read_callback)fread;
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic বের_করো
#endif
    }
    অন্যথা
      s->is_fread_set = 1;
    ভঙ্গ;
  পক্ষ CURLOPT_SEEKFUNCTION:
    /*
     * Seek callback. Might be NULL.
     */
    s->seek_func = va_arg(param, curl_seek_callback);
    ভঙ্গ;
  পক্ষ CURLOPT_IOCTLFUNCTION:
    /*
     * I/O control callback. Might be NULL.
     */
    s->ioctl_func = va_arg(param, curl_ioctl_callback);
    ভঙ্গ;
  পক্ষ CURLOPT_SSL_CTX_FUNCTION:
    /*
     * Set an SSL_CTX callback
     */
#ifdef USE_SSL
    যদি(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      s->ssl_fsslctx = va_arg(param, curl_ssl_ctx_callback);
      ভঙ্গ;
    }
    অন্যথা
#endif
      প্রত্যাগম CURLE_NOT_BUILT_IN;

  পক্ষ CURLOPT_SOCKOPTFUNCTION:
    /*
     * socket callback function: called after socket() but before connect()
     */
    s->fsockopt = va_arg(param, curl_sockopt_callback);
    ভঙ্গ;

  পক্ষ CURLOPT_OPENSOCKETFUNCTION:
    /*
     * open/create socket callback function: called instead of socket(),
     * before connect()
     */
    s->fopensocket = va_arg(param, curl_opensocket_callback);
    ভঙ্গ;

  পক্ষ CURLOPT_CLOSESOCKETFUNCTION:
    /*
     * close socket callback function: called instead of close()
     * when shutting down a connection
     */
    s->fclosesocket = va_arg(param, curl_closesocket_callback);
    ভঙ্গ;

  পক্ষ CURLOPT_RESOLVER_START_FUNCTION:
    /*
     * resolver start callback function: called before a new resolver request
     * is started
     */
    s->resolver_start = va_arg(param, curl_resolver_start_callback);
    ভঙ্গ;

#ifdef USE_SSH
#ifdef USE_LIBSSH2
  পক্ষ CURLOPT_SSH_HOSTKEYFUNCTION:
    /* the callback to check the hostkey without the knownhost file */
    s->ssh_hostkeyfunc = va_arg(param, curl_sshhostkeycallback);
    ভঙ্গ;
#endif

  পক্ষ CURLOPT_SSH_KEYFUNCTION:
    /* setting to NULL is fine since the ssh.c functions themselves will
       then revert to use the internal default */
    s->ssh_keyfunc = va_arg(param, curl_sshkeycallback);
    ভঙ্গ;

#endif /* USE_SSH */

#ifndef CURL_DISABLE_RTSP
  পক্ষ CURLOPT_INTERLEAVEFUNCTION:
    /* Set the user defined RTP write function */
    s->fwrite_rtp = va_arg(param, curl_write_callback);
    ভঙ্গ;
#endif
#ifndef CURL_DISABLE_FTP
  পক্ষ CURLOPT_CHUNK_BGN_FUNCTION:
    s->chunk_bgn = va_arg(param, curl_chunk_bgn_callback);
    ভঙ্গ;
  পক্ষ CURLOPT_CHUNK_END_FUNCTION:
    s->chunk_end = va_arg(param, curl_chunk_end_callback);
    ভঙ্গ;
  পক্ষ CURLOPT_FNMATCH_FUNCTION:
    s->fnmatch = va_arg(param, curl_fnmatch_callback);
    ভঙ্গ;
#endif
#ifndef CURL_DISABLE_HTTP
  পক্ষ CURLOPT_TRAILERFUNCTION:
    s->trailer_callback = va_arg(param, curl_trailer_callback);
    ভঙ্গ;
#endif
#ifndef CURL_DISABLE_HSTS
  পক্ষ CURLOPT_HSTSREADFUNCTION:
    s->hsts_read = va_arg(param, curl_hstsread_callback);
    ভঙ্গ;
  পক্ষ CURLOPT_HSTSWRITEFUNCTION:
    s->hsts_write = va_arg(param, curl_hstswrite_callback);
    ভঙ্গ;
#endif
  পক্ষ CURLOPT_PREREQFUNCTION:
    s->fprereq = va_arg(param, curl_prereq_callback);
    ভঙ্গ;
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode setopt_offt(সংরচনা Curl_easy *data, CURLoption option,
                            curl_off_t offt)
{
  সংরচনা UserDefined *s = &data->set;
  বিকল্প(option) {
  পক্ষ CURLOPT_TIMEVALUE_LARGE:
    /*
     * This is the value to compare with the remote document with the
     * method set with CURLOPT_TIMECONDITION
     */
    s->timevalue = (time_t)offt;
    ভঙ্গ;

    /* MQTT "borrows" some of the HTTP options */
  পক্ষ CURLOPT_POSTFIELDSIZE_LARGE:
    /*
     * The size of the POSTFIELD data to prevent libcurl to do strlen() to
     * figure it out. Enables binary posts.
     */
    যদি(offt < -1)
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;

    যদি(s->postfieldsize < offt && s->str_copypostfields) {
      /* Previous CURLOPT_COPYPOSTFIELDS is no longer valid. */
      curlx_safefree(s->str_copypostfields);
      s->postfields = রিক্ত;
    }
    s->postfieldsize = offt;
    ভঙ্গ;
  পক্ষ CURLOPT_INFILESIZE_LARGE:
    /*
     * If known, this should inform curl about the file size of the
     * to-be-uploaded file.
     */
    যদি(offt < -1)
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = offt;
    ভঙ্গ;
  পক্ষ CURLOPT_MAX_SEND_SPEED_LARGE:
    /*
     * When transfer uploads are faster than CURLOPT_MAX_SEND_SPEED_LARGE
     * bytes per second the transfer is throttled..
     */
    যদি(offt < 0)
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_send_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.ul.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    ভঙ্গ;
  পক্ষ CURLOPT_MAX_RECV_SPEED_LARGE:
    /*
     * When receiving data faster than CURLOPT_MAX_RECV_SPEED_LARGE bytes per
     * second the transfer is throttled..
     */
    যদি(offt < 0)
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_recv_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.dl.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    ভঙ্গ;
  পক্ষ CURLOPT_RESUME_FROM_LARGE:
    /*
     * Resume transfer at the given file position
     */
    যদি(offt < -1)
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = offt;
    ভঙ্গ;
  পক্ষ CURLOPT_MAXFILESIZE_LARGE:
    /*
     * Set the maximum size of a file to download.
     */
    যদি(offt < 0)
      প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_filesize = offt;
    ভঙ্গ;

  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode setopt_blob(সংরচনা Curl_easy *data, CURLoption option,
                            সংরচনা curl_blob *blob)
{
  সংরচনা UserDefined *s = &data->set;
  বিকল্প(option) {
  পক্ষ CURLOPT_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use
     */
    প্রত্যাগম Curl_setblobopt(&s->blobs[BLOB_CERT], blob);
#ifndef CURL_DISABLE_PROXY
  পক্ষ CURLOPT_PROXY_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use for proxy
     */
    প্রত্যাগম Curl_setblobopt(&s->blobs[BLOB_CERT_PROXY], blob);
  পক্ষ CURLOPT_PROXY_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use for proxy
     */
    প্রত্যাগম Curl_setblobopt(&s->blobs[BLOB_চাবি_PROXY], blob);
  পক্ষ CURLOPT_PROXY_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection proxy.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    যদি(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO_PROXY], blob);
      যদি(!result)
        s->proxy_ssl.custom_cablob = TRUE;
      প্রত্যাগম result;
    }
#endif
    প্রত্যাগম CURLE_NOT_BUILT_IN;
  পক্ষ CURLOPT_PROXY_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    যদি(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      প্রত্যাগম Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT_PROXY], blob);
    প্রত্যাগম CURLE_NOT_BUILT_IN;
#endif
  পক্ষ CURLOPT_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use
     */
    প্রত্যাগম Curl_setblobopt(&s->blobs[BLOB_চাবি], blob);
  পক্ষ CURLOPT_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    যদি(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO], blob);
      যদি(!result)
        s->ssl.custom_cablob = TRUE;
      প্রত্যাগম result;
    }
#endif
    প্রত্যাগম CURLE_NOT_BUILT_IN;
  পক্ষ CURLOPT_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    যদি(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      প্রত্যাগম Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT], blob);
    প্রত্যাগম CURLE_NOT_BUILT_IN;

  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }
  /* unreachable */
}

/*
 * Do not make Curl_vsetopt() static: it is called from
 * projects/OS400/ccsidcurl.c.
 */
CURLcode Curl_vsetopt(সংরচনা Curl_easy *data, CURLoption option, va_list param)
{
  যদি(option < CURLOPTTYPE_OBJECTPOINT)
    প্রত্যাগম setopt_long(data, option, va_arg(param, long));
  অন্যথা যদি(option < CURLOPTTYPE_FUNCTIONPOINT) {
    /* unfortunately, different pointer types cannot be identified any other
       way than being listed explicitly */
    বিকল্প(option) {
    পক্ষ CURLOPT_HTTPHEADER:
    পক্ষ CURLOPT_QUOTE:
    পক্ষ CURLOPT_POSTQUOTE:
    পক্ষ CURLOPT_TELNETOPTIONS:
    পক্ষ CURLOPT_PREQUOTE:
    পক্ষ CURLOPT_HTTP200ALIASES:
    পক্ষ CURLOPT_MAIL_RCPT:
    পক্ষ CURLOPT_RESOLVE:
    পক্ষ CURLOPT_PROXYHEADER:
    পক্ষ CURLOPT_CONNECT_TO:
      প্রত্যাগম setopt_slist(data, option, va_arg(param, সংরচনা curl_slist *));
    পক্ষ CURLOPT_HTTPPOST:         /* curl_httppost * */
    পক্ষ CURLOPT_MIMEPOST:         /* curl_mime * */
    পক্ষ CURLOPT_STDERR:           /* FILE * */
    পক্ষ CURLOPT_SHARE:            /* CURLSH * */
    পক্ষ CURLOPT_CURLU:            /* CURLU * */
      প্রত্যাগম setopt_pointers(data, option, param);
    পক্ষ CURLOPT_STREAM_DEPENDS:   /* CURL * */
    পক্ষ CURLOPT_STREAM_DEPENDS_E: /* CURL * */
      প্রত্যাগম CURLE_OK;
    স্বভাব:
      ভঙ্গ;
    }
    /* the char pointer options */
    প্রত্যাগম setopt_cptr(data, option, va_arg(param, বর্ণ *));
  }
  অন্যথা যদি(option < CURLOPTTYPE_OFF_T)
    প্রত্যাগম setopt_func(data, option, param);
  অন্যথা যদি(option < CURLOPTTYPE_BLOB)
    প্রত্যাগম setopt_offt(data, option, va_arg(param, curl_off_t));
  প্রত্যাগম setopt_blob(data, option, va_arg(param, সংরচনা curl_blob *));
}

/*
 * curl_easy_setopt() is the external interface for setting options on an
 * easy handle.
 *
 * NOTE: This is one of few API functions that are allowed to be called from
 * within a callback.
 */
#undef curl_easy_setopt
CURLcode curl_easy_setopt(CURL *curl, CURLoption option, ...)
{
  সংরচনা Curl_eapi_guard guard;
  CURLcode result;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_setopt, &result)) {
    সংরচনা Curl_easy *data = curl;
    va_list arg;

    va_start(arg, option);

    result = Curl_vsetopt(data, option, arg);

    va_end(arg);
    যদি(result == CURLE_BAD_FUNCTION_ARGUMENT)
      failf(data, "setopt 0x%x got bad argument", (unsigned পূর্ণাঙ্ক)option);
  }
  CURL_EAPI_LEAVE(&guard);
  প্রত্যাগম result;
}
