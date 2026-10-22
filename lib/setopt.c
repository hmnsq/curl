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
#নতুবা_যদি defined(HAVE_NETINET_TCP_H)
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
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
#যদি LONG_MAX > (TIMEDIFF_T_MAX / 1000)
  যদি(secs > (TIMEDIFF_T_MAX / 1000)) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    ফেরত CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)secs * 1000;
  ফেরত CURLE_OK;
}

স্থির CURLcode setopt_set_timeout_ms(timediff_t *ptimeout_ms, long ms)
{
  যদি(ms < 0)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
#যদি LONG_MAX > TIMEDIFF_T_MAX
  যদি(ms > TIMEDIFF_T_MAX) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    ফেরত CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)ms;
  ফেরত CURLE_OK;
}

CURLcode Curl_setstropt(কাঠামো Curl_easy *তথ্য,
                        গণনা dupstring id, ধ্রুবক অক্ষর *s)
{
  size_t slen = s ? strlen(s) : 0;
  DEBUGASSERT((unsigned)id <= UINT8_MAX);
  যদি(s && (slen > CURL_MAX_প্রবেশ_দৈর্ঘ্য))
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  ফেরত CURL_EASY_STR_SET(তথ্য, (uint8_t)id, s, slen);
}

CURLcode Curl_setblobopt(কাঠামো curl_blob **blobp,
                         ধ্রুবক কাঠামো curl_blob *blob)
{
  /* free the previous storage at `blobp' and replace by a dynamic storage
     copy of blob. If CURL_BLOB_COPY is set, the data is copied. */

  curlx_safefree(*blobp);

  যদি(blob) {
    কাঠামো curl_blob *nblob;
    যদি(!blob->তথ্য || !blob->len || (blob->len > CURL_MAX_প্রবেশ_দৈর্ঘ্য))
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    nblob = (কাঠামো curl_blob *)
      curlx_malloc(আকার(কাঠামো curl_blob) +
                   ((blob->flags & CURL_BLOB_COPY) ? blob->len : 0));
    যদি(!nblob)
      ফেরত CURLE_OUT_OF_MEMORY;
    *nblob = *blob;
    যদি(blob->flags & CURL_BLOB_COPY) {
      /* put the data after the blob struct in memory */
      nblob->তথ্য = (অক্ষর *)nblob + আকার(কাঠামো curl_blob);
      memcpy(nblob->তথ্য, blob->তথ্য, blob->len);
    }

    *blobp = nblob;
    ফেরত CURLE_OK;
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode setstropt_userpwd(ধ্রুবক অক্ষর *option, অক্ষর **userp,
                                  অক্ষর **passwdp)
{
  অক্ষর *user = ফাঁকা;
  অক্ষর *passwd = ফাঁকা;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Parse the login details if specified. If not, then we treat NULL as a
     hint to clear the existing data */
  যদি(option) {
    size_t len = strlen(option);
    CURLcode ফলাফল;
    যদি(len > CURL_MAX_প্রবেশ_দৈর্ঘ্য)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

    ফলাফল = Curl_parse_login_details(option, len, &user, &passwd, ফাঁকা);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  curlx_free(*userp);
  *userp = user;

  curlx_strzero(*passwdp);
  curlx_free(*passwdp);
  *passwdp = passwd;

  ফেরত CURLE_OK;
}

স্থির CURLcode setstropt_interface(কাঠামো Curl_easy *তথ্য, অক্ষর *option)
{
  যদি(option && !option[0])
    option = ফাঁকা;
  যদি(option) {
    /* Parse for checking syntax */
    CURLcode ফলাফল = Curl_parse_interface(option, ফাঁকা, ফাঁকা, ফাঁকা);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    ফেরত CURL_EASY_STR_SET(তথ্য, STRING_INTERFACE_OPT,
                             option, strlen(option));
  }
  CURL_EASY_STR_CLEAR(তথ্য, STRING_INTERFACE_OPT);
  ফেরত CURLE_OK;
}

#ifdef USE_SSL
#define C_SSLVERSION_মান(x)     ((x) & 0xffff)
#define C_SSLVERSION_MAX_মান(x) ((unsigned long)(x) & 0xffff0000)
#endif

স্থির CURLcode protocol2num(ধ্রুবক অক্ষর *str, curl_prot_t *val)
{
  /*
   * We are asked to cherry-pick protocols, so play it safe and disallow all
   * protocols to start with, and re-add the wanted ones back in.
   */
  *val = 0;

  যদি(!str)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(curl_strequal(str, "all")) {
    *val = ~(curl_prot_t)0;
    ফেরত CURLE_OK;
  }

  করো {
    ধ্রুবক অক্ষর *token = str;
    size_t tlen;

    str = strchr(str, ',');
    tlen = str ? (size_t)(str - token) : strlen(token);
    যদি(tlen) {
      ধ্রুবক কাঠামো Curl_scheme *h = Curl_getn_scheme(token, tlen);

      যদি(!h || !h->run)
        ফেরত CURLE_UNSUPPORTED_PROTOCOL;

      *val |= h->protocol;
    }
  } যখন(str && str++);

  যদি(!*val)
    /* no protocol listed */
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  ফেরত CURLE_OK;
}

#যদি !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
স্থির CURLcode httpauth(কাঠামো Curl_easy *তথ্য, bool proxy,
                         unsigned long auth)
{
  যদি(auth != CURLAUTH_NONE) {
    পূর্ণসংখ্যা bitcheck = 0;
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
    যখন(bitcheck < 31) {
      যদি(auth & (1UL << bitcheck++)) {
        authbits = TRUE;
        বিরতি;
      }
    }
    যদি(!authbits)
      ফেরত CURLE_NOT_BUILT_IN; /* no supported types left! */
  }
  যদি(proxy)
    তথ্য->set.proxyauth = (uint32_t)auth;
  অন্যথা
    তথ্য->set.httpauth = (uint32_t)auth;
  ফেরত CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */

#ifndef CURL_DISABLE_HTTP
স্থির CURLcode setopt_HTTP_VERSION(কাঠামো Curl_easy *তথ্য, long arg)
{
  /*
   * This sets a requested HTTP version to be used. The value is one of
   * the listed enums in curl/curl.h.
   */
  নির্বাচন(arg) {
  ক্ষেত্র CURL_HTTP_VERSION_NONE:
    /* accepted */
    বিরতি;
  ক্ষেত্র CURL_HTTP_VERSION_1_0:
  ক্ষেত্র CURL_HTTP_VERSION_1_1:
    /* accepted */
    বিরতি;
#ifdef USE_HTTP2
  ক্ষেত্র CURL_HTTP_VERSION_2_0:
  ক্ষেত্র CURL_HTTP_VERSION_2TLS:
  ক্ষেত্র CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    /* accepted */
    বিরতি;
#endif
#ifdef USE_HTTP3
  ক্ষেত্র CURL_HTTP_VERSION_3:
  ক্ষেত্র CURL_HTTP_VERSION_3ONLY:
    /* accepted */
    বিরতি;
#endif
  পূর্বনির্ধারিত:
    /* not accepted */
    যদি(arg < CURL_HTTP_VERSION_NONE)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    ফেরত CURLE_UNSUPPORTED_PROTOCOL;
  }
  তথ্য->set.httpwant = (unsigned অক্ষর)arg;
  ফেরত CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP */

#ifdef USE_SSL
CURLcode Curl_setopt_SSLVERSION(কাঠামো Curl_easy *তথ্য, CURLoption option,
                                long arg)
{
  /*
   * Set explicit SSL version to try to connect with, as some SSL
   * implementations are lame.
   */
  {
    long version, version_max;
    কাঠামো ssl_easy_config *sslc = &তথ্য->set.ssl;
#ifndef CURL_DISABLE_PROXY
    যদি(option != CURLOPT_SSLVERSION)
      sslc = &তথ্য->set.proxy_ssl;
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
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    যদি(version == CURL_SSLVERSION_DEFAULT)
      version = CURL_SSLVERSION_TLSv1_2;

    sslc->version = (unsigned অক্ষর)version;
    sslc->version_max = (unsigned পূর্ণসংখ্যা)version_max;
  }
  ফেরত CURLE_OK;
}
#endif /* !USE_SSL */

#ifndef CURL_DISABLE_RTSP
স্থির CURLcode setopt_RTSP_REQUEST(কাঠামো Curl_easy *তথ্য, long arg)
{
  /*
   * Set the RTSP request method (OPTIONS, SETUP, PLAY, etc...) Would this be
   * better if the RTSPREQ_* were moved into here?
   */
  যদি((arg <= CURL_RTSPREQ_NONE) || (arg >= CURL_RTSPREQ_LAST))
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  তথ্য->set.rtspreq = (unsigned অক্ষর)arg;
  ফেরত CURLE_OK;
}
#endif /* !CURL_DISABLE_RTSP */

স্থির CURLcode setopt_long_bool(কাঠামো Curl_easy *তথ্য, CURLoption option,
                                 long arg)
{
  bool enabled = !!arg;
  পূর্ণসংখ্যা ok = 1;
  কাঠামো UserDefined *s = &তথ্য->set;
  নির্বাচন(option) {
  ক্ষেত্র CURLOPT_FORBID_REUSE:
    /*
     * When this transfer is done, it must not be left to be reused by a
     * subsequent transfer but shall be closed immediately.
     */
    s->reuse_forbid = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_FRESH_CONNECT:
    /*
     * This transfer shall not use a previously cached connection but
     * should be made with a fresh new connect!
     */
    s->reuse_fresh = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_VERBOSE:
    /*
     * Verbose means infof() calls that give a lot of information about
     * the connection and transfer procedures as well as internal choices.
     */
    s->verbose = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_HEADER:
    /*
     * Set to include the header in the general data output stream.
     */
    s->include_header = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_NOPROGRESS:
    /*
     * Shut off the internal supported progress meter
     */
    তথ্য->progress.hide = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_NOBODY:
    /*
     * Do not include the body part in the output data stream.
     */
    s->opt_no_body = enabled;
#ifndef CURL_DISABLE_HTTP
    যদি(s->opt_no_body)
      /* in HTTP lingo, no body means using the HEAD request... */
      s->method = HTTPREQ_শীর্ষ;
    অন্যথা যদি(s->method == HTTPREQ_শীর্ষ)
      s->method = HTTPREQ_GET;
#endif
    বিরতি;
  ক্ষেত্র CURLOPT_FAILONERROR:
    /*
     * Do not output the >=400 error code HTML-page, but instead only
     * return error.
     */
    s->http_fail_on_error = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_KEEP_SENDING_ON_ERROR:
    s->http_keep_sending_on_error = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_UPLOAD:
  ক্ষেত্র CURLOPT_PUT:
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
    বিরতি;
  ক্ষেত্র CURLOPT_FILETIME:
    /*
     * Try to get the file time of the remote document. The time will
     * later (possibly) become available using curl_easy_getinfo().
     */
    s->get_filetime = enabled;
    বিরতি;
#ifndef CURL_DISABLE_HTTP
  ক্ষেত্র CURLOPT_HTTP09_ALLOWED:
    s->http09_allowed = enabled;
    বিরতি;
#ifndef CURL_DISABLE_COOKIES
  ক্ষেত্র CURLOPT_COOKIESESSION:
    /*
     * Set this option to TRUE to start a new "cookie session". It will
     * prevent the forthcoming read-cookies-from-file actions to accept
     * cookies that are marked as being session cookies, as they belong to a
     * previous session.
     */
    s->cookiesession = enabled;
    বিরতি;
#endif
  ক্ষেত্র CURLOPT_AUTOREFERER:
    /*
     * Switch on automatic referer that gets set if curl follows locations.
     */
    s->http_auto_referer = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_TRANSFER_ENCODING:
    s->http_transfer_encoding = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_UNRESTRICTED_AUTH:
    /*
     * Send authentication (user+password) when following locations, even when
     * hostname changed.
     */
    s->allow_auth_to_other_hosts = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_HTTP_TRANSFER_DECODING:
    /*
     * disable libcurl transfer encoding is used
     */
    s->http_te_skip = !enabled; /* reversed */
    বিরতি;
  ক্ষেত্র CURLOPT_HTTP_CONTENT_DECODING:
    /*
     * raw data passed to the application when content encoding is used
     */
    s->http_ce_skip = !enabled; /* reversed */
    বিরতি;
  ক্ষেত্র CURLOPT_HTTPGET:
    /*
     * Set to force us do HTTP GET
     */
    যদি(enabled) {
      s->method = HTTPREQ_GET;
      s->opt_no_body = FALSE; /* this is implied */
    }
    বিরতি;
  ক্ষেত্র CURLOPT_POST:
    /* Does this option serve a purpose anymore? Yes it does, when
       CURLOPT_POSTFIELDS is not used and the POST data is read off the
       callback! */
    যদি(enabled) {
      s->method = HTTPREQ_POST;
      s->opt_no_body = FALSE; /* this is implied */
    }
    অন্যথা
      s->method = HTTPREQ_GET;
    বিরতি;
#endif /* !CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_PROXY
  ক্ষেত্র CURLOPT_HTTPPROXYTUNNEL:
    /*
     * Tunnel operations through the proxy instead of normal proxy use
     */
    s->tunnel_thru_httpproxy = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_HAPROXYPROTOCOL:
    /*
     * Set to send the HAProxy Proxy Protocol header
     */
    s->haproxyprotocol = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_PROXY_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for proxy.
     */
    s->proxy_ssl.verifypeer = enabled;

    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(তথ্য, TRUE);
    বিরতি;
  ক্ষেত্র CURLOPT_PROXY_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for proxy
     */
    s->proxy_ssl.verifyhost = enabled;
    ok = 2;
    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(তথ্য, TRUE);
    বিরতি;
  ক্ষেত্র CURLOPT_PROXY_TRANSFER_MODE:
    /*
     * set transfer mode (;type=<a|i>) when doing FTP via an HTTP proxy
     */
    s->proxy_transfer_mode = enabled;
    বিরতি;
#endif /* !CURL_DISABLE_PROXY */
#যদি defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ক্ষেত্র CURLOPT_SOCKS5_GSSAPI_NEC:
    /*
     * Set flag for NEC SOCKS5 support
     */
    s->socks5_gssapi_nec = enabled;
    বিরতি;
#endif
#ifdef CURL_LIST_ONLY_PROTOCOL
  ক্ষেত্র CURLOPT_DIRLISTONLY:
    /*
     * An option that changes the command to one that asks for a list only, no
     * file info details. Used for FTP, POP3 and SFTP.
     */
    s->list_only = enabled;
    বিরতি;
#endif
  ক্ষেত্র CURLOPT_APPEND:
    /*
     * We want to upload and append to an existing file. Used for FTP and
     * SFTP.
     */
    s->remote_append = enabled;
    বিরতি;
#ifndef CURL_DISABLE_FTP
  ক্ষেত্র CURLOPT_FTP_USE_EPRT:
    s->ftp_use_eprt = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_FTP_USE_EPSV:
    s->ftp_use_epsv = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_FTP_USE_PRET:
    s->ftp_use_pret = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_FTP_SKIP_PASV_IP:
    /*
     * Enable or disable FTP_SKIP_PASV_IP, which will disable/enable the
     * bypass of the IP address in PASV responses.
     */
    s->ftp_skip_ip = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_WILDCARDMATCH:
    s->wildcard_enabled = enabled;
    বিরতি;
#endif
  ক্ষেত্র CURLOPT_CRLF:
    /*
     * Kludgy option to enable CRLF conversions. Subject for removal.
     */
    s->crlf = enabled;
    বিরতি;
#ifndef CURL_DISABLE_TFTP
  ক্ষেত্র CURLOPT_TFTP_NO_OPTIONS:
    /*
     * Option that prevents libcurl from sending TFTP option requests to the
     * server.
     */
    s->tftp_no_options = enabled;
    বিরতি;
#endif /* !CURL_DISABLE_TFTP */
  ক্ষেত্র CURLOPT_TRANSFERTEXT:
    /*
     * This option was previously named 'FTPASCII'. Renamed to work with
     * more protocols than merely FTP.
     *
     * Transfer using ASCII (instead of BINARY).
     */
    s->prefer_ascii = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying.
     */
    s->ssl.verifypeer = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(তথ্য, FALSE);
    বিরতি;
#ifndef CURL_DISABLE_DOH
  ক্ষেত্র CURLOPT_DOH_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for DoH.
     */
    s->doh_verifypeer = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_DOH_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for DoH
     */
    s->doh_verifyhost = enabled;
    ok = 2;
    বিরতি;
  ক্ষেত্র CURLOPT_DOH_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying for DoH.
     */
    যদি(!Curl_ssl_cert_status_request())
      ফেরত CURLE_NOT_BUILT_IN;

    s->doh_verifystatus = enabled;
    ok = 2;
    বিরতি;
#endif /* !CURL_DISABLE_DOH */
  ক্ষেত্র CURLOPT_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate
     */

    /* Obviously people are not reading documentation and too many thought
       this argument took a boolean when it was not and misused it.
       Treat 1 and 2 the same */
    s->ssl.verifyhost = enabled;
    ok = 2;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(তথ্য, FALSE);
    বিরতি;
  ক্ষেত্র CURLOPT_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying.
     */
    যদি(!Curl_ssl_cert_status_request())
      ফেরত CURLE_NOT_BUILT_IN;

    s->ssl.verifystatus = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(তথ্য, FALSE);
    বিরতি;
  ক্ষেত্র CURLOPT_CERTINFO:
#ifdef USE_SSL
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_CERTINFO))
      s->ssl.certinfo = enabled;
    অন্যথা
#endif
      ফেরত CURLE_NOT_BUILT_IN;
    বিরতি;
  ক্ষেত্র CURLOPT_NOSIGNAL:
    /*
     * The application asks not to set any signal() or alarm() handlers,
     * even when using a timeout.
     */
    s->no_signal = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_TCP_NODELAY:
    /*
     * Enable or disable TCP_NODELAY, which will disable/enable the Nagle
     * algorithm
     */
    s->tcp_nodelay = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_IGNORE_CONTENT_দৈর্ঘ্য:
    s->ignorecl = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_SSL_SESSIONID_CACHE:
    s->ssl.cache_session = enabled;
#ifndef CURL_DISABLE_PROXY
    s->proxy_ssl.cache_session = s->ssl.cache_session;
#endif
    বিরতি;
#ifdef USE_SSH
  ক্ষেত্র CURLOPT_SSH_COMPRESSION:
    s->ssh_compression = enabled;
    বিরতি;
#endif /* !USE_SSH */
#ifndef CURL_DISABLE_SMTP
  ক্ষেত্র CURLOPT_MAIL_RCPT_ALLOWFAILS:
    /* allow RCPT TO command to fail for some recipients */
    s->mail_rcpt_allowfails = enabled;
    বিরতি;
#endif /* !CURL_DISABLE_SMTP */
  ক্ষেত্র CURLOPT_SASL_IR:
    /* Enable/disable SASL initial response */
    s->sasl_ir = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_TCP_KEEPALIVE:
    s->tcp_keepalive = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_TCP_FASTOPEN:
#যদি defined(CONNECT_তথ্য_IDEMPOTENT) || defined(MSG_FASTOPEN) || \
  defined(TCP_FASTOPEN_CONNECT)
    s->tcp_fastopen = enabled;
    বিরতি;
#অন্যথা
    ফেরত CURLE_NOT_BUILT_IN;
#endif
  ক্ষেত্র CURLOPT_SSL_ENABLE_ALPN:
    s->ssl_enable_alpn = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_PATH_AS_IS:
    s->path_as_is = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_PIPEWAIT:
    s->pipewait = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_SUPPRESS_CONNECT_HEADERS:
    s->suppress_connect_headers = enabled;
    বিরতি;
#ifndef CURL_DISABLE_SHUFFLE_DNS
  ক্ষেত্র CURLOPT_DNS_SHUFFLE_ADDRESSES:
    s->dns_shuffle_addresses = enabled;
    বিরতি;
#endif
  ক্ষেত্র CURLOPT_DISALLOW_USERNAME_IN_URL:
    s->disallow_username_in_url = enabled;
    বিরতি;
  ক্ষেত্র CURLOPT_QUICK_EXIT:
    s->quick_exit = enabled;
    বিরতি;
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }
  যদি((arg > ok) || (arg < 0))
    /* reserve other values for future use */
    infof(তথ্য, "boolean setopt(%d) got unsupported argument %ld,"
          " treated as %d", (পূর্ণসংখ্যা)option, arg, enabled);

  ফেরত CURLE_OK;
}

স্থির CURLcode মান_range(long *মান, long below_error, long min, long max)
{
  যদি(*মান < below_error)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  অন্যথা যদি(*মান < min)
    *মান = min;
  অন্যথা যদি(*মান > max)
    *মান = max;
  ফেরত CURLE_OK;
}

স্থির CURLcode setopt_long_net(কাঠামো Curl_easy *তথ্য, CURLoption option,
                                long arg)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো UserDefined *s = &তথ্য->set;

  নির্বাচন(option) {
  ক্ষেত্র CURLOPT_DNS_CACHE_TIMEOUT:
    যদি(arg != -1)
      ফেরত setopt_set_timeout_sec(&s->dns_cache_timeout_ms, arg);
    s->dns_cache_timeout_ms = -1;
    বিরতি;
  ক্ষেত্র CURLOPT_MAXCONNECTS:
    ফলাফল = মান_range(&arg, 0, 0, INT_MAX);
    যদি(!ফলাফল)
      s->maxconnects = arg ? (uint32_t)arg : DEFAULT_CONNCACHE_আকার;
    বিরতি;
  ক্ষেত্র CURLOPT_SERVER_RESPONSE_TIMEOUT:
    ফেরত setopt_set_timeout_sec(&s->server_response_timeout, arg);
  ক্ষেত্র CURLOPT_SERVER_RESPONSE_TIMEOUT_MS:
    ফেরত setopt_set_timeout_ms(&s->server_response_timeout, arg);
  ক্ষেত্র CURLOPT_নিচের_SPEED_LIMIT:
    যদি(arg < 0)
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->নিচের_speed_limit = arg;
    বিরতি;
  ক্ষেত্র CURLOPT_নিচের_SPEED_TIME:
    ফলাফল = মান_range(&arg, 0, 0, USHRT_MAX);
    যদি(!ফলাফল)
      s->নিচের_speed_time = (uint16_t)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_PORT:
    যদি((arg < 0) || (arg > 65535))
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    s->use_port = (unsigned short)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_TIMEOUT:
    ফেরত setopt_set_timeout_sec(&s->timeout, arg);
  ক্ষেত্র CURLOPT_TIMEOUT_MS:
    ফেরত setopt_set_timeout_ms(&s->timeout, arg);
  ক্ষেত্র CURLOPT_CONNECTTIMEOUT:
    ফেরত setopt_set_timeout_sec(&s->connecttimeout, arg);
  ক্ষেত্র CURLOPT_CONNECTTIMEOUT_MS:
    ফেরত setopt_set_timeout_ms(&s->connecttimeout, arg);
#ifndef CURL_DISABLE_BINDLOCAL
  ক্ষেত্র CURLOPT_LOCALPORT:
    যদি((arg < 0) || (arg > 65535))
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    s->localport = curlx_sltous(arg);
    বিরতি;
  ক্ষেত্র CURLOPT_LOCALPORTRANGE:
    যদি((arg < 0) || (arg > 65535))
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    s->localportrange = curlx_sltous(arg);
    বিরতি;
#endif
  ক্ষেত্র CURLOPT_BUFFERSIZE:
    ফলাফল = মান_range(&arg, 0, READBUFFER_MIN, READBUFFER_MAX);
    যদি(!ফলাফল)
      s->বাফার_আকার = (unsigned পূর্ণসংখ্যা)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_UPLOAD_BUFFERSIZE:
    ফলাফল = মান_range(&arg, 0, UPLOADBUFFER_MIN, UPLOADBUFFER_MAX);
    যদি(!ফলাফল)
      s->upload_বাফার_আকার = (unsigned পূর্ণসংখ্যা)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_MAXFILESIZE:
    যদি(arg < 0)
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->max_filesize = arg;
    বিরতি;
  ক্ষেত্র CURLOPT_IPRESOLVE:
    যদি((arg < CURL_IPRESOLVE_WHATEVER) || (arg > CURL_IPRESOLVE_V6))
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->ipver = (unsigned অক্ষর)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_CONNECT_ONLY:
    যদি(arg < 0 || arg > 2)
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা {
      s->connect_only = !!arg;
      s->connect_only_ws = (arg == 2);
    }
    বিরতি;
#ifdef USE_IPV6
  ক্ষেত্র CURLOPT_ADDRESS_SCOPE:
#যদি SIZEOF_LONG > 4
    যদি((unsigned long)arg > UINT_MAX)
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
#endif
    s->scope_id = (unsigned পূর্ণসংখ্যা)arg;
    বিরতি;
#endif
  ক্ষেত্র CURLOPT_TCP_KEEPIDLE:
    ফলাফল = মান_range(&arg, 0, 0, INT_MAX);
    যদি(!ফলাফল)
      s->tcp_keepidle = (পূর্ণসংখ্যা)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_TCP_KEEPINTVL:
    ফলাফল = মান_range(&arg, 0, 0, INT_MAX);
    যদি(!ফলাফল)
      s->tcp_keepintvl = (পূর্ণসংখ্যা)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_TCP_KEEPCNT:
    ফলাফল = মান_range(&arg, 0, 0, INT_MAX);
    যদি(!ফলাফল)
      s->tcp_keepcnt = (পূর্ণসংখ্যা)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
    ফেরত setopt_set_timeout_ms(&s->happy_eyeballs_timeout, arg);
  ক্ষেত্র CURLOPT_UPKEEP_INTERVAL_MS:
    ফেরত setopt_set_timeout_ms(&s->upkeep_interval_ms, arg);
  ক্ষেত্র CURLOPT_MAXAGE_CONN:
    ফেরত setopt_set_timeout_sec(&s->conn_max_idle_ms, arg);
  ক্ষেত্র CURLOPT_MAXLIFETIME_CONN:
    ফেরত setopt_set_timeout_sec(&s->conn_max_age_ms, arg);
  ক্ষেত্র CURLOPT_DNS_USE_GLOBAL_CACHE:
    /* deprecated */
    বিরতি;
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode setopt_long_ssl(কাঠামো Curl_easy *তথ্য, CURLoption option,
                                long arg)
{
#ifdef USE_SSL
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো UserDefined *s = &তথ্য->set;
  নির্বাচন(option) {
  ক্ষেত্র CURLOPT_CA_CACHE_TIMEOUT:
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_CA_CACHE)) {
      ফলাফল = মান_range(&arg, -1, -1, INT_MAX);
      যদি(!ফলাফল)
        s->ssl_ca_cache_timeout = (পূর্ণসংখ্যা)arg;
    }
    অন্যথা
      ফলাফল = CURLE_NOT_BUILT_IN;
    বিরতি;
  ক্ষেত্র CURLOPT_SSLVERSION:
#ifndef CURL_DISABLE_PROXY
  ক্ষেত্র CURLOPT_PROXY_SSLVERSION:
#endif
    ফেরত Curl_setopt_SSLVERSION(তথ্য, option, arg);
  ক্ষেত্র CURLOPT_SSL_FALSESTART:
    ফলাফল = CURLE_NOT_BUILT_IN;
    বিরতি;
  ক্ষেত্র CURLOPT_USE_SSL:
    যদি((arg < CURLUSESSL_NONE) || (arg >= CURLUSESSL_LAST))
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->use_ssl = (unsigned অক্ষর)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_SSL_OPTIONS:
    s->ssl.ssl_options = (unsigned অক্ষর)(arg & 0xff);
    বিরতি;
#ifndef CURL_DISABLE_PROXY
  ক্ষেত্র CURLOPT_PROXY_SSL_OPTIONS:
    s->proxy_ssl.ssl_options = (unsigned অক্ষর)(arg & 0xff);
    বিরতি;
#endif
  ক্ষেত্র CURLOPT_SSL_ENABLE_NPN:
    বিরতি;
  ক্ষেত্র CURLOPT_SSLENGINE_DEFAULT:
#ifdef USE_OPENSSL
    যদি(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      ফেরত CURLE_NOT_BUILT_IN;
    তথ্য->set.engine_default = (arg == 1);
#অন্যথা
    ফেরত CURLE_NOT_BUILT_IN;
#endif
    বিরতি;
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }
  ফেরত ফলাফল;
#অন্যথা  /* USE_SSL */
  (শূন্য)তথ্য;
  (শূন্য)option;
  (শূন্য)arg;
  ফেরত CURLE_UNKNOWN_OPTION;
#endif /* !USE_SSL */
}

#ifndef CURL_DISABLE_PROXY
স্থির শূন্য changeproxy(কাঠামো Curl_easy *তথ্য)
{
  Curl_auth_digest_cleanup(&তথ্য->state.proxydigest);
  memset(&তথ্য->state.authproxy, 0, আকার(তথ্য->state.authproxy));
}

স্থির CURLcode setopt_long_proxy(কাঠামো Curl_easy *তথ্য, CURLoption option,
                                  long arg)
{
  কাঠামো UserDefined *s = &তথ্য->set;

  নির্বাচন(option) {
  ক্ষেত্র CURLOPT_PROXYPORT:
    যদি((arg < 0) || (arg > UINT16_MAX))
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    যদি(arg != s->proxyport)
      changeproxy(তথ্য);
    s->proxyport = (uint16_t)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_PROXYAUTH:
    ফেরত httpauth(তথ্য, TRUE, (unsigned long)arg);
  ক্ষেত্র CURLOPT_PROXYTYPE:
    যদি((arg < CURLPROXY_HTTP) || (arg > CURLPROXY_HTTPS3))
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef USE_PROXY_HTTP3
    যদি(arg == CURLPROXY_HTTPS3)
      ফেরত CURLE_NOT_BUILT_IN;
#endif
    s->proxytype = (unsigned অক্ষর)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_SOCKS5_AUTH: {
    unsigned long auth = (unsigned long)arg;
    যদি(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI | CURLAUTH_ONLY))
      ফেরত CURLE_NOT_BUILT_IN;
    যদি((auth & CURLAUTH_ONLY) && !(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI)))
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    s->socks5auth = (unsigned অক্ষর)(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI));
    s->socks5_auth_only = !!(auth & CURLAUTH_ONLY);
    বিরতি;
  }
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }
  ফেরত CURLE_OK;
}
#অন্যথা
স্থির CURLcode setopt_long_proxy(কাঠামো Curl_easy *তথ্য, CURLoption option,
                                  long arg)
{
  (শূন্য)তথ্য;
  (শূন্য)option;
  (শূন্য)arg;
  ফেরত CURLE_UNKNOWN_OPTION;
}
#endif

স্থির CURLcode setopt_long_http(কাঠামো Curl_easy *তথ্য, CURLoption option,
                                 long arg)
{
#ifndef CURL_DISABLE_HTTP
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো UserDefined *s = &তথ্য->set;

  নির্বাচন(option) {
  ক্ষেত্র CURLOPT_FOLLOWLOCATION:
    যদি((unsigned long)arg > 3)
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->http_follow_mode = (unsigned অক্ষর)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_MAXREDIRS:
    ফলাফল = মান_range(&arg, -1, -1, 0x7fff);
    যদি(!ফলাফল)
      s->maxredirs = (short)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_POSTREDIR:
    যদি(arg < CURL_REDIR_GET_ALL)
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা {
      s->post301 = !!(arg & CURL_REDIR_POST_301);
      s->post302 = !!(arg & CURL_REDIR_POST_302);
      s->post303 = !!(arg & CURL_REDIR_POST_303);
    }
    বিরতি;
  ক্ষেত্র CURLOPT_HEADEROPT:
    s->sep_headers = !!(arg & CURLHEADER_SEPARATE);
    বিরতি;
  ক্ষেত্র CURLOPT_HTTPAUTH:
    ফেরত httpauth(তথ্য, FALSE, (unsigned long)arg);
  ক্ষেত্র CURLOPT_HTTP_VERSION:
    ফেরত setopt_HTTP_VERSION(তথ্য, arg);
  ক্ষেত্র CURLOPT_EXPECT_100_TIMEOUT_MS:
    ফলাফল = মান_range(&arg, 0, 0, 0xffff);
    যদি(!ফলাফল)
      s->expect_100_timeout = (unsigned short)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_STREAM_WEIGHT:
#যদি defined(USE_HTTP2) || defined(USE_HTTP3)
    যদি((arg >= 1) && (arg <= 256))
      s->weight = (পূর্ণসংখ্যা)arg;
    বিরতি;
#অন্যথা
    ফলাফল = CURLE_NOT_BUILT_IN;
    বিরতি;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  ক্ষেত্র CURLOPT_HTTPSIG_ALGORITHM:
    যদি(arg != CURLHTTPSIG_NONE &&
       arg != CURLHTTPSIG_ED25519 &&
       arg != CURLHTTPSIG_HMAC_SHA256)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    s->httpsig_algorithm = (uint8_t)arg;
    যদি(arg)
      s->httpauth = (uint32_t)CURLAUTH_HTTPSIG;
    অন্যথা
      s->httpauth &= ~(uint32_t)CURLAUTH_HTTPSIG;
    বিরতি;
#endif
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }
  ফেরত ফলাফল;
#অন্যথা
  (শূন্য)তথ্য;
  (শূন্য)option;
  (শূন্য)arg;
  ফেরত CURLE_UNKNOWN_OPTION;
#endif
}

স্থির CURLcode setopt_long_proto(কাঠামো Curl_easy *তথ্য, CURLoption option,
                                  long arg)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো UserDefined *s = &তথ্য->set;

  নির্বাচন(option) {
#ifndef CURL_DISABLE_TFTP
  ক্ষেত্র CURLOPT_TFTP_BLKSIZE:
    ফলাফল = মান_range(&arg, 0, TFTP_BLKSIZE_MIN, TFTP_BLKSIZE_MAX);
    যদি(!ফলাফল)
      s->tftp_blksize = (unsigned short)arg;
    বিরতি;
#endif
#ifndef CURL_DISABLE_NETRC
  ক্ষেত্র CURLOPT_NETRC:
    যদি((arg < CURL_NETRC_IGNORED) || (arg >= CURL_NETRC_LAST))
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->use_netrc = (unsigned অক্ষর)arg;
    বিরতি;
#endif
#ifndef CURL_DISABLE_FTP
  ক্ষেত্র CURLOPT_FTP_FILEMETHOD:
    যদি((arg < CURLFTPMETHOD_DEFAULT) || (arg >= CURLFTPMETHOD_LAST))
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->ftp_filemethod = (unsigned অক্ষর)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_FTP_SSL_CCC:
    যদি((arg < CURLFTPSSL_CCC_NONE) || (arg >= CURLFTPSSL_CCC_LAST))
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->ftp_ccc = (unsigned অক্ষর)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_FTPSSLAUTH:
    যদি((arg < CURLFTPAUTH_DEFAULT) || (arg >= CURLFTPAUTH_LAST))
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->ftpsslauth = (unsigned অক্ষর)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_ACCEPTTIMEOUT_MS:
    ফেরত setopt_set_timeout_ms(&s->accepttimeout, arg);
#endif
#যদি !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  ক্ষেত্র CURLOPT_FTP_CREATE_MISSING_DIRS:
    যদি((arg < CURLFTP_CREATE_DIR_NONE) || (arg > CURLFTP_CREATE_DIR_RETRY))
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->ftp_create_missing_dirs = (unsigned অক্ষর)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_NEW_FILE_PERMS:
    যদি((arg < 0) || (arg > 0777))
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->new_file_perms = (unsigned পূর্ণসংখ্যা)arg;
    বিরতি;
#endif
#ifndef CURL_DISABLE_RTSP
  ক্ষেত্র CURLOPT_RTSP_REQUEST:
    ফেরত setopt_RTSP_REQUEST(তথ্য, arg);
  ক্ষেত্র CURLOPT_RTSP_CLIENT_CSEQ:
    ফলাফল = মান_range(&arg, 0, 0, INT_MAX);
    যদি(!ফলাফল)
      তথ্য->state.rtsp_পরবর্তী_client_CSeq = (uint32_t)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_RTSP_SERVER_CSEQ:
    ফলাফল = মান_range(&arg, 0, 0, INT_MAX);
    যদি(!ফলাফল)
      তথ্য->state.rtsp_পরবর্তী_server_CSeq = (uint32_t)arg;
    বিরতি;
#endif
#ifdef USE_SSH
  ক্ষেত্র CURLOPT_SSH_AUTH_TYPES:
    s->ssh_auth_types = (uint32_t)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_NEW_DIRECTORY_PERMS:
    যদি((arg < 0) || (arg > 0777))
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      s->new_directory_perms = (unsigned পূর্ণসংখ্যা)arg;
    বিরতি;
#endif
  ক্ষেত্র CURLOPT_PROTOCOLS:
    s->allowed_protocols = (curl_prot_t)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_REDIR_PROTOCOLS:
    s->redir_protocols = (curl_prot_t)arg;
    বিরতি;
#ifndef CURL_DISABLE_WEBSOCKETS
  ক্ষেত্র CURLOPT_WS_OPTIONS:
    s->ws_raw_mode = !!(arg & CURLWS_RAW_MODE);
    s->ws_no_auto_pong = !!(arg & CURLWS_NOAUTOPONG);
    বিরতি;
#endif
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode setopt_long_misc(কাঠামো Curl_easy *তথ্য, CURLoption option,
                                 long arg)
{
  কাঠামো UserDefined *s = &তথ্য->set;

  নির্বাচন(option) {
  ক্ষেত্র CURLOPT_TIMECONDITION:
    যদি((arg < CURL_TIMECOND_NONE) || (arg >= CURL_TIMECOND_LAST))
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    s->timecondition = (unsigned অক্ষর)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_TIMEVALUE:
    s->timevalue = (time_t)arg;
    বিরতি;
  ক্ষেত্র CURLOPT_POSTFIELDSIZE:
    যদি(arg < -1)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    যদি(s->postfieldsize < arg && s->str_copypostfields) {
      curlx_safefree(s->str_copypostfields);
      s->postfields = ফাঁকা;
    }
    s->postfieldsize = arg;
    বিরতি;
  ক্ষেত্র CURLOPT_INFILESIZE:
    যদি(arg < -1)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = arg;
    বিরতি;
  ক্ষেত্র CURLOPT_RESUME_FROM:
    যদি(arg < -1)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = arg;
    বিরতি;
  ক্ষেত্র CURLOPT_UPLOAD_FLAGS:
    s->upload_flags = (unsigned অক্ষর)arg;
    বিরতি;
#ifndef CURL_DISABLE_MIME
  ক্ষেত্র CURLOPT_MIME_OPTIONS:
    s->mime_formescape = !!(arg & CURLMIMEOPT_FORMESCAPE);
    বিরতি;
#endif
#ifndef CURL_DISABLE_HSTS
  ক্ষেত্র CURLOPT_HSTS_CTRL:
    যদি(arg & CURLHSTS_ENABLE) {
      যদি(!তথ্য->hsts) {
        তথ্য->hsts = Curl_hsts_init();
        যদি(!তথ্য->hsts)
          ফেরত CURLE_OUT_OF_MEMORY;
      }
    }
    অন্যথা যদি(!তথ্য->share || !তথ্য->share->hsts) {
      /* throw away the HSTS cache unless shared */
      Curl_hsts_cleanup(&তথ্য->hsts);
      /* flush all the entries */
      curl_slist_free_all(তথ্য->state.hstslist);
      তথ্য->state.hstslist = ফাঁকা;
    }
    অন্যথা
      /* detach from shared HSTS cache without freeing it */
      তথ্য->hsts = ফাঁকা;
    বিরতি;
#endif
#ifndef CURL_DISABLE_ALTSVC
  ক্ষেত্র CURLOPT_ALTSVC_CTRL:
    ফেরত Curl_altsvc_ctrl(তথ্য, arg);
#endif
#যদি defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ক্ষেত্র CURLOPT_GSSAPI_DELEGATION:
    s->gssapi_delegation = (unsigned অক্ষর)arg &
      (CURLGSSAPI_DELEGATION_POLICY_FLAG | CURLGSSAPI_DELEGATION_FLAG);
    বিরতি;
#endif
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode setopt_long(কাঠামো Curl_easy *তথ্য, CURLoption option,
                            long arg)
{
  প্রকারলেখ CURLcode (*setoptfunc)(কাঠামো Curl_easy *তথ্য,
                                 CURLoption option, long arg);
  স্থির ধ্রুবক setoptfunc setopt_call[] = {
    setopt_long_bool,
    setopt_long_net,
    setopt_long_http,
    setopt_long_proxy,
    setopt_long_ssl,
    setopt_long_proto,
    setopt_long_misc
  };
  size_t i;

  জন্য(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode ফলাফল = setopt_call[i](তথ্য, option, arg);
    যদি(ফলাফল != CURLE_UNKNOWN_OPTION)
      ফেরত ফলাফল;
  }
  ফেরত CURLE_UNKNOWN_OPTION;
}

স্থির CURLcode setopt_slist(কাঠামো Curl_easy *তথ্য, CURLoption option,
                             কাঠামো curl_slist *slist)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো UserDefined *s = &তথ্য->set;
  নির্বাচন(option) {
#ifndef CURL_DISABLE_PROXY
  ক্ষেত্র CURLOPT_PROXYHEADER:
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
    বিরতি;
#endif
#ifndef CURL_DISABLE_HTTP
  ক্ষেত্র CURLOPT_HTTP200ALIASES:
    /*
     * Set a list of aliases for HTTP 200 in response header
     */
    s->http200aliases = slist;
    বিরতি;
#endif
#যদি !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  ক্ষেত্র CURLOPT_POSTQUOTE:
    /*
     * List of RAW FTP commands to use after a transfer
     */
    s->postquote = slist;
    বিরতি;
  ক্ষেত্র CURLOPT_PREQUOTE:
    /*
     * List of RAW FTP commands to use prior to RETR (Wesley Laxton)
     */
    s->prequote = slist;
    বিরতি;
  ক্ষেত্র CURLOPT_QUOTE:
    /*
     * List of RAW FTP commands to use before a transfer
     */
    s->quote = slist;
    বিরতি;
#endif
  ক্ষেত্র CURLOPT_RESOLVE:
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
    তথ্য->state.resolve = s->resolve;
    বিরতি;
#যদি !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MIME)
  ক্ষেত্র CURLOPT_HTTPHEADER:
    /*
     * Set a list with HTTP headers to use (or replace internals with)
     */
    s->headers = slist;
    বিরতি;
#endif
#ifndef CURL_DISABLE_TELNET
  ক্ষেত্র CURLOPT_TELNETOPTIONS:
    /*
     * Set a linked list of telnet options
     */
    s->telnet_options = slist;
    বিরতি;
#endif
#ifndef CURL_DISABLE_SMTP
  ক্ষেত্র CURLOPT_MAIL_RCPT:
    /* Set the list of mail recipients */
    s->mail_rcpt = slist;
    বিরতি;
#endif
  ক্ষেত্র CURLOPT_CONNECT_TO:
    s->connect_to = slist;
    বিরতি;
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }
  ফেরত ফলাফল;
}

#যদি !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
স্থির CURLcode setopt_mimepost(কাঠামো Curl_easy *তথ্য, curl_mime *mimep)
{
  /*
   * Set to make us do MIME POST
   */
  CURLcode ফলাফল;
  কাঠামো UserDefined *s = &তথ্য->set;
  যদি(!s->mimepostp) {
    s->mimepostp = curlx_malloc(আকার(*s->mimepostp));
    যদি(!s->mimepostp)
      ফেরত CURLE_OUT_OF_MEMORY;
    Curl_mime_initpart(s->mimepostp);
  }

  ফলাফল = Curl_mime_set_subparts(s->mimepostp, mimep, FALSE);
  যদি(!ফলাফল) {
    s->method = HTTPREQ_POST_MIME;
    s->opt_no_body = FALSE; /* this is implied */
#ifndef CURL_DISABLE_FORM_API
    Curl_mime_cleanpart(তথ্য->state.formp);
    curlx_safefree(তথ্য->state.formp);
    তথ্য->state.mimepost = ফাঁকা;
#endif
  }
  ফেরত ফলাফল;
}
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */

স্থির CURLcode setopt_share(কাঠামো Curl_easy *তথ্য, কাঠামো Curl_share *set)
{
  CURLcode ফলাফল;

  যদি(তথ্য->conn) {
    /* As this handle already has a connection attached, changing share now
       would be complicated and error-prone */
    infof(তথ্য, "Cannot change share object while in use");
    ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
  }
  অন্যথা {
    /* disconnect from old share, if any and possible */
    ফলাফল = Curl_share_easy_unlink(তথ্য);
    যদি(!ফলাফল && GOOD_SHARE_HANDLE(set))
      /* use new share if it set */
      ফলাফল = Curl_share_easy_link(তথ্য, set);
  }
  ফেরত ফলাফল;
}

/* assorted pointer type arguments */
স্থির CURLcode setopt_pointers(কাঠামো Curl_easy *তথ্য, CURLoption option,
                                va_list param)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো UserDefined *s = &তথ্য->set;
  নির্বাচন(option) {
  ক্ষেত্র CURLOPT_CURLU:
    /*
     * pass CURLU to set URL
     */
    Curl_bufref_free(&তথ্য->state.url);
    CURL_EASY_STR_CLEAR(তথ্য, STRING_SET_URL);
    s->uh = va_arg(param, CURLU *);
    বিরতি;
#ifndef CURL_DISABLE_HTTP
#ifndef CURL_DISABLE_FORM_API
  ক্ষেত্র CURLOPT_HTTPPOST:
    /*
     * Set to make us do HTTP POST. Legacy API-style.
     */
    s->httppost = va_arg(param, কাঠামো curl_httppost *);
    s->method = HTTPREQ_POST_FORM;
    s->opt_no_body = FALSE; /* this is implied */
    Curl_mime_cleanpart(তথ্য->state.formp);
    curlx_safefree(তথ্য->state.formp);
    তথ্য->state.mimepost = ফাঁকা;
    বিরতি;
#endif /* !CURL_DISABLE_FORM_API */
#endif /* !CURL_DISABLE_HTTP */
#যদি !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
  ক্ষেত্র CURLOPT_MIMEPOST:
    ফলাফল = setopt_mimepost(তথ্য, va_arg(param, curl_mime *));
    বিরতি;
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */
  ক্ষেত্র CURLOPT_STDERR:
    /*
     * Set to a FILE * that should receive all error writes. This
     * defaults to stderr for normal operations.
     */
    s->err = va_arg(param, FILE *);
    যদি(!s->err)
      s->err = stderr;
    বিরতি;
  ক্ষেত্র CURLOPT_SHARE:
    ফেরত setopt_share(তথ্য, va_arg(param, কাঠামো Curl_share *));

  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }
  ফেরত ফলাফল;
}

#ifndef CURL_DISABLE_COOKIES
স্থির CURLcode cookielist(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *ptr)
{
  CURLcode ফলাফল = CURLE_OK;
  যদি(!ptr)
    ফেরত CURLE_OK;

  যদি(curl_strequal(ptr, "ALL")) {
    /* clear all cookies */
    Curl_share_lock(তথ্য, CURL_LOCK_তথ্য_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearall(তথ্য->cookies);
    Curl_share_unlock(তথ্য, CURL_LOCK_তথ্য_COOKIE);
  }
  অন্যথা যদি(curl_strequal(ptr, "SESS")) {
    /* clear session cookies */
    Curl_share_lock(তথ্য, CURL_LOCK_তথ্য_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearsess(তথ্য->cookies);
    Curl_share_unlock(তথ্য, CURL_LOCK_তথ্য_COOKIE);
  }
  অন্যথা যদি(curl_strequal(ptr, "FLUSH")) {
    /* flush cookies to file, takes care of the locking */
    Curl_flush_cookies(তথ্য, FALSE);
  }
  অন্যথা যদি(curl_strequal(ptr, "RELOAD")) {
    /* reload cookies from file */
    ফেরত Curl_cookie_loadfiles(তথ্য, COOKIE_NOPSL |
                                 (তথ্য->set.cookiesession ?
                                  COOKIE_NOSESSION : 0));
  }
  অন্যথা {
    যদি(!তথ্য->cookies) {
      /* if cookie engine was not running, activate it */
      তথ্য->cookies = Curl_cookie_init();
      যদি(!তথ্য->cookies)
        ফেরত CURLE_OUT_OF_MEMORY;
      তথ্য->state.cookie_engine = TRUE;
    }

    /* general protection against mistakes and abuse */
    যদি(strlen(ptr) > CURL_MAX_প্রবেশ_দৈর্ঘ্য)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

    /* Adding these cookies without the PSL check, because the PSL is not
       initialized until *perform() time, and this might be called before
       that */
    Curl_share_lock(তথ্য, CURL_LOCK_তথ্য_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    যদি(checkprefix("Set-Cookie:", ptr))
      /* HTTP Header format line */
      ফলাফল = Curl_cookie_add(তথ্য, তথ্য->cookies, ptr + 11,
                               ফাঁকা, ফাঁকা,
                               COOKIE_HTTPHEADER | COOKIE_SECURE |
                               COOKIE_NOPSL);
    অন্যথা
      /* Netscape format line */
      ফলাফল = Curl_cookie_add(তথ্য, তথ্য->cookies, ptr, ফাঁকা,
                               ফাঁকা, COOKIE_SECURE | COOKIE_NOPSL);
    Curl_share_unlock(তথ্য, CURL_LOCK_তথ্য_COOKIE);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode cookiefile(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *ptr)
{
  /*
   * Set cookie file to read and parse. Can be used multiple times.
   */
  যদি(ptr) {
    কাঠামো curl_slist *cl;
    /* general protection against mistakes and abuse */
    যদি(strlen(ptr) > CURL_MAX_প্রবেশ_দৈর্ঘ্য)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    /* append the cookie filename to the list of filenames, and deal with
       them later */
    cl = curl_slist_append(তথ্য->state.cookielist, ptr);
    যদি(!cl) {
      curl_slist_free_all(তথ্য->state.cookielist);
      তথ্য->state.cookielist = ফাঁকা;
      ফেরত CURLE_OUT_OF_MEMORY;
    }
    তথ্য->state.cookielist = cl; /* store the list for later use */
  }
  অন্যথা {
    /* clear the list of cookie files */
    curl_slist_free_all(তথ্য->state.cookielist);
    তথ্য->state.cookielist = ফাঁকা;

    যদি(!তথ্য->share || !তথ্য->share->cookies) {
      /* throw away all existing cookies if this is not a shared cookie
         container */
      Curl_cookie_clearall(তথ্য->cookies);
      Curl_cookie_cleanup(তথ্য->cookies);
    }
    /* disable the cookie engine */
    তথ্য->cookies = ফাঁকা;
  }
  ফেরত CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_PROXY
স্থির CURLcode setproxy(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *proxy)
{
  ধ্রুবক অক্ষর *str = CURL_EASY_STR(তথ্য, STRING_PROXY);
  যদি(str && proxy &&
     /* there was one set, is this a new one? */
     !strcmp(str, proxy))
    ফেরত CURLE_OK; /* same one as before */

  changeproxy(তথ্য);
  ফেরত Curl_setstropt(তথ্য, STRING_PROXY, proxy);
}

স্থির CURLcode setopt_cptr_proxy(কাঠামো Curl_easy *তথ্য, CURLoption option,
                                  অক্ষর *ptr)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো UserDefined *s = &তথ্য->set;
  নির্বাচন(option) {
  ক্ষেত্র CURLOPT_PROXYUSERPWD: {
    /*
     * user:password needed to use the proxy
     */
    অক্ষর *u = ফাঁকা;
    অক্ষর *p = ফাঁকা;
    ফলাফল = setstropt_userpwd(ptr, &u, &p);

    /* URL decode the components */
    যদি(!ফলাফল) {
      অক্ষর *str = ফাঁকা;
      CURL_EASY_STR_CLEAR(তথ্য, STRING_PROXYUSERNAME);
      CURL_EASY_STR_CLEAR(তথ্য, STRING_PROXYPASSWORD);
      যদি(u) {
        ফলাফল = Curl_urldecode(u, 0, &str, ফাঁকা, REJECT_ZERO);
        যদি(!ফলাফল)
          ফলাফল = Curl_u8_strset_setn(&s->strings, STRING_PROXYUSERNAME, str);
      }
      যদি(!ফলাফল && p) {
        str = ফাঁকা;
        ফলাফল = Curl_urldecode(p, 0, &str, ফাঁকা, REJECT_ZERO);
        যদি(!ফলাফল)
          ফলাফল = Curl_u8_strset_setn(&s->strings, STRING_PROXYPASSWORD, str);
      }
    }
    curlx_free(u);
    curlx_strzero(p);
    curlx_free(p);
    বিরতি;
  }
  ক্ষেত্র CURLOPT_PROXYUSERNAME:
    /*
     * authentication username to use in the operation
     */
    ফেরত Curl_setstropt(তথ্য, STRING_PROXYUSERNAME, ptr);

  ক্ষেত্র CURLOPT_PROXYPASSWORD:
    /*
     * authentication password to use in the operation
     */
    ফেরত Curl_setstropt(তথ্য, STRING_PROXYPASSWORD, ptr);

  ক্ষেত্র CURLOPT_NOPROXY:
    /*
     * proxy exception list
     */
    ফেরত Curl_setstropt(তথ্য, STRING_NOPROXY, ptr);
  ক্ষেত্র CURLOPT_PROXY_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use for proxy
     */
    ফেরত Curl_setstropt(তথ্য, STRING_CERT_PROXY, ptr);
  ক্ষেত্র CURLOPT_PROXY_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use for proxy
     */
    ফেরত Curl_setstropt(তথ্য, STRING_CERT_TYPE_PROXY, ptr);
  ক্ষেত্র CURLOPT_PROXY_SSLKEY:
    /*
     * String that holds filename of the SSL key to use for proxy
     */
    ফেরত Curl_setstropt(তথ্য, STRING_চাবি_PROXY, ptr);
  ক্ষেত্র CURLOPT_PROXY_KEYPASSWD:
    /*
     * String that holds the SSL private key password for proxy.
     */
    ফেরত Curl_setstropt(তথ্য, STRING_চাবি_PASSWD_PROXY, ptr);
  ক্ষেত্র CURLOPT_PROXY_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use for proxy
     */
    ফেরত Curl_setstropt(তথ্য, STRING_চাবি_TYPE_PROXY, ptr);
  ক্ষেত্র CURLOPT_PROXY_SSL_CIPHER_LIST:
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_CIPHER_LIST)) {
      /* set a list of cipher we want to use in the SSL connection for proxy */
      ফেরত Curl_setstropt(তথ্য, STRING_SSL_CIPHER_LIST_PROXY, ptr);
    }
    অন্যথা
      ফেরত CURLE_NOT_BUILT_IN;
  ক্ষেত্র CURLOPT_PROXY_TLS13_CIPHERS:
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites for proxy */
      ফেরত Curl_setstropt(তথ্য, STRING_SSL_CIPHER13_LIST_PROXY, ptr);
    অন্যথা
      ফেরত CURLE_NOT_BUILT_IN;
  ক্ষেত্র CURLOPT_PROXY:
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
    ফেরত setproxy(তথ্য, ptr);
  ক্ষেত্র CURLOPT_PRE_PROXY:
    /*
     * Set proxy server:port to use as SOCKS proxy.
     *
     * If the proxy is set to "" or NULL we explicitly say that we do not want
     * to use the socks proxy.
     */
    ফেরত Curl_setstropt(তথ্য, STRING_PRE_PROXY, ptr);
  ক্ষেত্র CURLOPT_SOCKS5_GSSAPI_SERVICE:
  ক্ষেত্র CURLOPT_PROXY_SERVICE_NAME:
    /*
     * Set proxy authentication service name for Kerberos 5 and SPNEGO
     */
    ফেরত Curl_setstropt(তথ্য, STRING_PROXY_SERVICE_NAME, ptr);
  ক্ষেত্র CURLOPT_PROXY_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
#ifdef USE_SSL
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_PINNEDPUBKEY))
      ফেরত Curl_setstropt(তথ্য, STRING_SSL_PINNEDPUBLICKEY_PROXY, ptr);
#endif
    ফেরত CURLE_NOT_BUILT_IN;

  ক্ষেত্র CURLOPT_HAPROXY_CLIENT_IP:
    /*
     * Set the client IP to send through HAProxy PROXY protocol
     */
    ফলাফল = Curl_setstropt(তথ্য, STRING_HAPROXY_CLIENT_IP, ptr);

    /* enable the HAProxy protocol if an IP is provided */
    s->haproxyprotocol = !!CURL_EASY_STR(তথ্য, STRING_HAPROXY_CLIENT_IP);
    বিরতি;
  ক্ষেত্র CURLOPT_PROXY_CAINFO:
    /*
     * Set CA info SSL connection for proxy. Specify filename of the
     * CA certificate
     */
    ফলাফল = Curl_setstropt(তথ্য, STRING_SSL_CAFILE_PROXY, ptr);
    যদি(!ফলাফল)
      s->proxy_ssl.custom_cafile = TRUE;
    ফেরত ফলাফল;
  ক্ষেত্র CURLOPT_PROXY_CRLFILE:
    /*
     * Set CRL file info for SSL connection for proxy. Specify filename of the
     * CRL to check certificates revocation
     */
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_CRLFILE))
      ফেরত Curl_setstropt(তথ্য, STRING_SSL_CRLFILE_PROXY, ptr);
    ফেরত CURLE_NOT_BUILT_IN;
  ক্ষেত্র CURLOPT_PROXY_ISSUERCERT:
    /*
     * Set Issuer certificate file to check certificates issuer
     */
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_ISSUERCERT))
      ফেরত Curl_setstropt(তথ্য, STRING_SSL_ISSUERCERT_PROXY, ptr);
    ফেরত CURLE_NOT_BUILT_IN;
  ক্ষেত্র CURLOPT_PROXY_CAPATH:
    /*
     * Set CA path info for SSL connection proxy. Specify directory name of the
     * CA certificates which have been prepared using openssl c_rehash utility.
     */
#ifdef USE_SSL
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      ফলাফল = Curl_setstropt(তথ্য, STRING_SSL_CAPATH_PROXY, ptr);
      যদি(!ফলাফল)
        s->proxy_ssl.custom_capath = TRUE;
      ফেরত ফলাফল;
    }
#endif
    ফেরত CURLE_NOT_BUILT_IN;
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }
  ফেরত ফলাফল;
}
#endif

#যদি !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
/*
 * A string with POST data. Makes curl HTTP POST. Even if it is NULL. If
 * needed, CURLOPT_POSTFIELDSIZE must have been set prior to
 * CURLOPT_COPYPOSTFIELDS and not altered later.
 */
স্থির CURLcode setopt_copypostfields(ধ্রুবক অক্ষর *ptr, কাঠামো UserDefined *s)
{
  CURLcode ফলাফল = CURLE_OK;
  যদি(s->postfieldsize < -1)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  যদি(!ptr || s->postfieldsize == -1) {
    যদি(ptr && (strlen(ptr) > CURL_MAX_প্রবেশ_দৈর্ঘ্য))
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_safefree(s->str_copypostfields);
    যদি(ptr) {
      s->str_copypostfields = curlx_strdup(ptr);
      যদি(!s->str_copypostfields)
        ফেরত CURLE_OUT_OF_MEMORY;
    }
  }
  অন্যথা {
    size_t pflen = curlx_sotouz_range(s->postfieldsize, 0, SIZE_MAX);
    যদি(pflen == SIZE_MAX)
      ফেরত CURLE_OUT_OF_MEMORY;
    অন্যথা {
      /* Allocate even when size == 0. This satisfies the need of possible
         later address compare to detect the COPYPOSTFIELDS mode, and to mark
         that postfields is used rather than read function or form data. */
      অক্ষর *p = curlx_memdup0(ptr, pflen);
      যদি(!p)
        ফেরত CURLE_OUT_OF_MEMORY;
      অন্যথা {
        curlx_free(s->str_copypostfields);
        s->str_copypostfields = p;
      }
    }
  }

  s->postfields = s->str_copypostfields;
  s->method = HTTPREQ_POST;
  ফেরত ফলাফল;
}
#endif

#ifdef USE_ECH
স্থির CURLcode setopt_ech(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *ptr)
{
  কাঠামো UserDefined *s = &তথ্য->set;
  CURLcode ফলাফল = CURLE_OK;

  যদি(!ptr || !strcmp(ptr, "false"))
    s->ssl.ech = CURLECH_DISABLE;
  অন্যথা {
    size_t plen = strlen(ptr);
    যদি(plen > CURL_MAX_প্রবেশ_দৈর্ঘ্য)
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
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
        ফলাফল = Curl_setstropt(তথ্য, STRING_ECH_CONFIG, ptr + 4);
      }
      অন্যথা যদি(plen > 3 && !strncmp(ptr, "pn:", 3)) {
        যদি(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        ফলাফল = Curl_setstropt(তথ্য, STRING_ECH_PUBLIC, ptr + 3);
      }
      অন্যথা
        ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  ফেরত ফলাফল;
}
#অন্যথা
#define setopt_ech(x, y) CURLE_NOT_BUILT_IN
#endif

#যদি defined(USE_SSL) || defined(USE_SSH)
/* One of the options is used for both TLS and SSH */
স্থির CURLcode setopt_cptr_ssl(কাঠামো Curl_easy *তথ্য, CURLoption option,
                                অক্ষর *ptr)
{
  CURLcode ফলাফল = CURLE_OK;

  নির্বাচন(option) {
  ক্ষেত্র CURLOPT_KEYPASSWD:
    /*
     * String that holds the SSL or SSH private key password.
     */
    ফলাফল = Curl_setstropt(তথ্য, STRING_চাবি_PASSWD, ptr);
    বিরতি;
#ifdef USE_SSL
  ক্ষেত্র CURLOPT_CAINFO:
    /*
     * Set CA info for SSL connection. Specify filename of the CA certificate
     */
    ফলাফল = Curl_setstropt(তথ্য, STRING_SSL_CAFILE, ptr);
    যদি(!ফলাফল)
      তথ্য->set.ssl.custom_cafile = TRUE;
    ফেরত ফলাফল;
  ক্ষেত্র CURLOPT_CAPATH:
    /*
     * Set CA path info for SSL connection. Specify directory name of the CA
     * certificates which have been prepared using openssl c_rehash utility.
     */
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      ফলাফল = Curl_setstropt(তথ্য, STRING_SSL_CAPATH, ptr);
      যদি(!ফলাফল)
        তথ্য->set.ssl.custom_capath = TRUE;
      ফেরত ফলাফল;
    }
    ফেরত CURLE_NOT_BUILT_IN;
  ক্ষেত্র CURLOPT_CRLFILE:
    /*
     * Set CRL file info for SSL connection. Specify filename of the CRL
     * to check certificates revocation
     */
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_CRLFILE))
      ফেরত Curl_setstropt(তথ্য, STRING_SSL_CRLFILE, ptr);
    ফেরত CURLE_NOT_BUILT_IN;
  ক্ষেত্র CURLOPT_SSL_CIPHER_LIST:
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_CIPHER_LIST))
      /* set a list of cipher we want to use in the SSL connection */
      ফেরত Curl_setstropt(তথ্য, STRING_SSL_CIPHER_LIST, ptr);
    অন্যথা
      ফেরত CURLE_NOT_BUILT_IN;
  ক্ষেত্র CURLOPT_TLS13_CIPHERS:
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites */
      ফেরত Curl_setstropt(তথ্য, STRING_SSL_CIPHER13_LIST, ptr);
    অন্যথা
      ফেরত CURLE_NOT_BUILT_IN;
  ক্ষেত্র CURLOPT_RANDOM_FILE:
    বিরতি;
  ক্ষেত্র CURLOPT_EGDSOCKET:
    বিরতি;
  ক্ষেত্র CURLOPT_SSL_CTX_তথ্য:
    /*
     * Set an SSL_CTX callback parameter pointer
     */
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_SSL_CTX)) {
      তথ্য->set.ssl_fsslctxp = ptr;
      বিরতি;
    }
    অন্যথা
      ফেরত CURLE_NOT_BUILT_IN;
  ক্ষেত্র CURLOPT_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use
     */
    ফেরত Curl_setstropt(তথ্য, STRING_CERT, ptr);
  ক্ষেত্র CURLOPT_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use
     */
    ফেরত Curl_setstropt(তথ্য, STRING_CERT_TYPE, ptr);
  ক্ষেত্র CURLOPT_SSLKEY:
    /*
     * String that holds filename of the SSL key to use
     */
    ফেরত Curl_setstropt(তথ্য, STRING_চাবি, ptr);
  ক্ষেত্র CURLOPT_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use
     */
    ফেরত Curl_setstropt(তথ্য, STRING_চাবি_TYPE, ptr);
  ক্ষেত্র CURLOPT_SSLENGINE:
    /*
     * String that holds the SSL crypto engine.
     */
    যদি(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      ফেরত CURLE_NOT_BUILT_IN;
    ফলাফল = Curl_setstropt(তথ্য, STRING_SSL_ENGINE, ptr);
    বিরতি;
  ক্ষেত্র CURLOPT_ISSUERCERT:
    /*
     * Set Issuer certificate file
     * to check certificates issuer
     */
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_ISSUERCERT))
      ফেরত Curl_setstropt(তথ্য, STRING_SSL_ISSUERCERT, ptr);
    ফেরত CURLE_NOT_BUILT_IN;
  ক্ষেত্র CURLOPT_SSL_EC_CURVES:
    /*
     * Set accepted curves in SSL connection setup.
     * Specify colon-delimited list of curve algorithm names.
     */
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_SSL_EC_CURVES))
      ফেরত Curl_setstropt(তথ্য, STRING_SSL_EC_CURVES, ptr);
    ফেরত CURLE_NOT_BUILT_IN;
  ক্ষেত্র CURLOPT_SSL_SIGNATURE_ALGORITHMS:
    /*
     * Set accepted signature algorithms.
     * Specify colon-delimited list of signature scheme names.
     */
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_SIGNATURE_ALGORITHMS))
      ফেরত Curl_setstropt(তথ্য, STRING_SSL_SIGNATURE_ALGORITHMS, ptr);
    ফেরত CURLE_NOT_BUILT_IN;
  ক্ষেত্র CURLOPT_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_PINNEDPUBKEY))
      ফেরত Curl_setstropt(তথ্য, STRING_SSL_PINNEDPUBLICKEY, ptr);
    ফেরত CURLE_NOT_BUILT_IN;
  ক্ষেত্র CURLOPT_ECH:
    ফেরত setopt_ech(তথ্য, ptr);
#endif
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }
  ফেরত ফলাফল;
}
#endif

#যদি !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
স্থির CURLcode setopt_cptr_http_mqtt(কাঠামো Curl_easy *তথ্য,
                                      CURLoption option, অক্ষর *ptr)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো UserDefined *s = &তথ্য->set;

  নির্বাচন(option) {
  ক্ষেত্র CURLOPT_COPYPOSTFIELDS:
    ফেরত setopt_copypostfields(ptr, s);

  ক্ষেত্র CURLOPT_POSTFIELDS:
    /*
     * Like above, but use static data instead of copying it.
     */
    s->postfields = ptr;
    /* Release old copied data. */
    curlx_safefree(s->str_copypostfields);
    s->method = HTTPREQ_POST;
    বিরতি;

#ifndef CURL_DISABLE_HTTP
  ক্ষেত্র CURLOPT_TRAILERDATA:
    s->trailer_তথ্য = ptr;
    বিরতি;
  ক্ষেত্র CURLOPT_ACCEPT_ENCODING:
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
        ফলাফল = CURL_EASY_STR_SETN(তথ্য, STRING_ENCODING, ptr);
      অন্যথা
        ফলাফল = CURLE_OUT_OF_MEMORY;
      ফেরত ফলাফল;
    }
    ফেরত Curl_setstropt(তথ্য, STRING_ENCODING, ptr);

#ifndef CURL_DISABLE_AWS
  ক্ষেত্র CURLOPT_AWS_SIGV4:
    /*
     * String that is merged to some authentication
     * parameters are used by the algorithm.
     */
    ফলাফল = Curl_setstropt(তথ্য, STRING_AWS_SIGV4, ptr);
    /*
     * Basic has been set by default; it needs to be unset here.
     */
    যদি(CURL_EASY_STR(তথ্য, STRING_AWS_SIGV4))
      s->httpauth = CURLAUTH_AWS_SIGV4;
    অন্যথা
      s->httpauth &= ~(uint32_t)CURLAUTH_AWS_SIGV4;
    বিরতি;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  ক্ষেত্র CURLOPT_HTTPSIG_চাবি:
    ফলাফল = Curl_setstropt(তথ্য, STRING_HTTPSIG_চাবি, ptr);
    বিরতি;
  ক্ষেত্র CURLOPT_HTTPSIG_KEYID:
    ফলাফল = Curl_setstropt(তথ্য, STRING_HTTPSIG_KEYID, ptr);
    বিরতি;
  ক্ষেত্র CURLOPT_HTTPSIG_HEADERS:
    ফলাফল = Curl_setstropt(তথ্য, STRING_HTTPSIG_HEADERS, ptr);
    বিরতি;
#endif
  ক্ষেত্র CURLOPT_REFERER: {
    /*
     * String to set in the HTTP Referer: field.
     */
    কাঠামো bufref *oldref = &তথ্য->state.referer;
    /* free the old after the storing the new in case the input is actually
       pointing back to this */
    ফলাফল = Curl_setstropt(তথ্য, STRING_SET_REFERER, ptr);
    Curl_bufref_free(oldref);
    বিরতি;
  }

  ক্ষেত্র CURLOPT_USERAGENT:
    /*
     * String to use in the HTTP User-Agent field
     */
    ফেরত Curl_setstropt(তথ্য, STRING_USERAGENT, ptr);

#ifndef CURL_DISABLE_COOKIES
  ক্ষেত্র CURLOPT_COOKIE:
    /*
     * Cookie string to send to the remote server in the request.
     */
    ফেরত Curl_setstropt(তথ্য, STRING_COOKIE, ptr);

  ক্ষেত্র CURLOPT_COOKIEFILE:
    ফেরত cookiefile(তথ্য, ptr);

  ক্ষেত্র CURLOPT_COOKIEJAR:
    /*
     * Set cookie filename to dump all cookies to when we are done.
     */
    ফলাফল = Curl_setstropt(তথ্য, STRING_COOKIEJAR, ptr);
    যদি(!ফলাফল) {
      /*
       * Activate the cookie parser. This may or may not already
       * have been made.
       */
      যদি(!তথ্য->cookies)
        তথ্য->cookies = Curl_cookie_init();
      যদি(!তথ্য->cookies)
        ফলাফল = CURLE_OUT_OF_MEMORY;
      অন্যথা
        তথ্য->state.cookie_engine = TRUE;
    }
    বিরতি;

  ক্ষেত্র CURLOPT_COOKIELIST:
    ফেরত cookielist(তথ্য, ptr);
#endif /* !CURL_DISABLE_COOKIES */

#endif /* !CURL_DISABLE_HTTP */
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }
  ফেরত ফলাফল;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_MQTT */

#ifdef USE_SSH
স্থির CURLcode setopt_cptr_ssh(কাঠামো Curl_easy *তথ্য, CURLoption option,
                                অক্ষর *ptr)
{
  কাঠামো UserDefined *s = &তথ্য->set;
  নির্বাচন(option) {
  ক্ষেত্র CURLOPT_SSH_PUBLIC_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa.pub file
     */
    ফেরত Curl_setstropt(তথ্য, STRING_SSH_PUBLIC_চাবি, ptr);
  ক্ষেত্র CURLOPT_SSH_PRIVATE_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa file
     */
    ফেরত Curl_setstropt(তথ্য, STRING_SSH_PRIVATE_চাবি, ptr);
  ক্ষেত্র CURLOPT_SSH_KEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_keyfunc_userp = ptr;
    বিরতি;
  ক্ষেত্র CURLOPT_SSH_HOST_PUBLIC_চাবি_MD5:
    /*
     * Option to allow for the MD5 of the host public key to be checked
     * for validation purposes.
     */
    ফেরত Curl_setstropt(তথ্য, STRING_SSH_HOST_PUBLIC_চাবি_MD5, ptr);
  ক্ষেত্র CURLOPT_SSH_HOST_PUBLIC_চাবি_SHA256:
    /*
     * Option to allow for the SHA256 of the host public key to be checked
     * for validation purposes.
     */
    ফেরত Curl_setstropt(তথ্য, STRING_SSH_HOST_PUBLIC_চাবি_SHA256, ptr);
  ক্ষেত্র CURLOPT_SSH_KNOWNHOSTS:
    /*
     * Store the filename to read known hosts from.
     */
    ফেরত Curl_setstropt(তথ্য, STRING_SSH_KNOWNHOSTS, ptr);
#ifdef USE_LIBSSH2
  ক্ষেত্র CURLOPT_SSH_HOSTKEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_hostkeyfunc_userp = ptr;
    বিরতি;
#endif /* USE_LIBSSH2 */
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }
  ফেরত CURLE_OK;
}
#endif /* USE_SSH */

#ifndef CURL_DISABLE_FTP
স্থির CURLcode setopt_cptr_ftp(কাঠামো Curl_easy *তথ্য, CURLoption option,
                                অক্ষর *ptr)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো UserDefined *s = &তথ্য->set;
  নির্বাচন(option) {
  ক্ষেত্র CURLOPT_FTPPORT:
    /*
     * Use FTP PORT, this also specifies which IP address to use
     */
    ফলাফল = Curl_setstropt(তথ্য, STRING_FTPPORT, ptr);
    s->ftp_use_port = !!CURL_EASY_STR(তথ্য, STRING_FTPPORT);
    বিরতি;

  ক্ষেত্র CURLOPT_FTP_ACCOUNT:
    ফেরত Curl_setstropt(তথ্য, STRING_FTP_ACCOUNT, ptr);

  ক্ষেত্র CURLOPT_FTP_ALTERNATIVE_TO_USER:
    ফেরত Curl_setstropt(তথ্য, STRING_FTP_ALTERNATIVE_TO_USER, ptr);

  ক্ষেত্র CURLOPT_KRBLEVEL:
    ফেরত CURLE_NOT_BUILT_IN; /* removed in 8.17.0 */
  ক্ষেত্র CURLOPT_CHUNK_তথ্য:
    s->wildcardptr = ptr;
    বিরতি;
  ক্ষেত্র CURLOPT_FNMATCH_তথ্য:
    s->fnmatch_তথ্য = ptr;
    বিরতি;
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }
  ফেরত ফলাফল;
}
#endif /* !CURL_DISABLE_FTP */

স্থির CURLcode setopt_cptr_net(কাঠামো Curl_easy *তথ্য, CURLoption option,
                                অক্ষর *ptr)
{
  নির্বাচন(option) {
  ক্ষেত্র CURLOPT_INTERFACE:
    /*
     * Set what interface or address/hostname to bind the socket to when
     * performing an operation and thus what from-IP your connection will use.
     */
    ফেরত setstropt_interface(তথ্য, ptr);

#ifdef USE_RESOLV_ARES
  ক্ষেত্র CURLOPT_DNS_SERVERS:
    ফেরত Curl_setstropt(তথ্য, STRING_DNS_SERVERS, ptr);

  ক্ষেত্র CURLOPT_DNS_INTERFACE:
    ফেরত Curl_setstropt(তথ্য, STRING_DNS_INTERFACE, ptr);

  ক্ষেত্র CURLOPT_DNS_LOCAL_IP4:
    ফেরত Curl_setstropt(তথ্য, STRING_DNS_LOCAL_IP4, ptr);

  ক্ষেত্র CURLOPT_DNS_LOCAL_IP6:
    ফেরত Curl_setstropt(তথ্য, STRING_DNS_LOCAL_IP6, ptr);
#endif
#ifdef USE_UNIX_SOCKETS
  ক্ষেত্র CURLOPT_UNIX_SOCKET_PATH:
    তথ্য->set.abstract_unix_socket = FALSE;
    ফেরত Curl_setstropt(তথ্য, STRING_UNIX_SOCKET_PATH, ptr);

  ক্ষেত্র CURLOPT_ABSTRACT_UNIX_SOCKET:
    তথ্য->set.abstract_unix_socket = TRUE;
    ফেরত Curl_setstropt(তথ্য, STRING_UNIX_SOCKET_PATH, ptr);
#endif
#ifndef CURL_DISABLE_DOH
  ক্ষেত্র CURLOPT_DOH_URL:
    {
      CURLcode ফলাফল = Curl_setstropt(তথ্য, STRING_DOH, ptr);
      তথ্য->set.doh = !!CURL_EASY_STR(তথ্য, STRING_DOH);
      ফেরত ফলাফল;
    }
#endif
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }
}

স্থির CURLcode setopt_cptr_misc(কাঠামো Curl_easy *তথ্য, CURLoption option,
                                 অক্ষর *ptr)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো UserDefined *s = &তথ্য->set;

  নির্বাচন(option) {
  ক্ষেত্র CURLOPT_REQUEST_TARGET:
    ফেরত Curl_setstropt(তথ্য, STRING_TARGET, ptr);
#ifndef CURL_DISABLE_NETRC
  ক্ষেত্র CURLOPT_NETRC_FILE:
    ফেরত Curl_setstropt(তথ্য, STRING_NETRC_FILE, ptr);
#endif
  ক্ষেত্র CURLOPT_CUSTOMREQUEST:
    ফেরত Curl_setstropt(তথ্য, STRING_CUSTOMREQUEST, ptr);

    /* we do not set s->method = HTTPREQ_CUSTOM; here, we continue as if we
       were using the already set type and this changes the actual request
       keyword */
  ক্ষেত্র CURLOPT_SERVICE_NAME:
    ফেরত Curl_setstropt(তথ্য, STRING_SERVICE_NAME, ptr);

  ক্ষেত্র CURLOPT_HEADERDATA:
    s->writeheader = ptr;
    বিরতি;
  ক্ষেত্র CURLOPT_READDATA:
    s->in_set = ptr;
    বিরতি;
  ক্ষেত্র CURLOPT_WRITEDATA:
    s->out = ptr;
    বিরতি;
  ক্ষেত্র CURLOPT_DEBUGDATA:
    s->debugdata = ptr;
    বিরতি;
  ক্ষেত্র CURLOPT_PROGRESSDATA:
    s->progress_client = ptr;
    বিরতি;
  ক্ষেত্র CURLOPT_SEEKDATA:
    s->seek_client = ptr;
    বিরতি;
  ক্ষেত্র CURLOPT_IOCTLDATA:
    s->ioctl_client = ptr;
    বিরতি;
  ক্ষেত্র CURLOPT_SOCKOPTDATA:
    s->sockopt_client = ptr;
    বিরতি;
  ক্ষেত্র CURLOPT_OPENSOCKETDATA:
    s->opensocket_client = ptr;
    বিরতি;
  ক্ষেত্র CURLOPT_RESOLVER_START_তথ্য:
    s->resolver_start_client = ptr;
    বিরতি;
  ক্ষেত্র CURLOPT_CLOSESOCKETDATA:
    s->closesocket_client = ptr;
    বিরতি;
  ক্ষেত্র CURLOPT_PREREQDATA:
    s->prereq_userp = ptr;
    বিরতি;
  ক্ষেত্র CURLOPT_ERRORBUFFER:
    s->errorbuffer = ptr;
    বিরতি;
  ক্ষেত্র CURLOPT_URL:
    ফলাফল = Curl_setstropt(তথ্য, STRING_SET_URL, ptr);
    Curl_bufref_set(&তথ্য->state.url,
                    CURL_EASY_STR(তথ্য, STRING_SET_URL), 0, ফাঁকা);
    বিরতি;

  ক্ষেত্র CURLOPT_USERPWD: {
    অক্ষর *u = ফাঁকা, *p = ফাঁকা;
    ফলাফল = setstropt_userpwd(ptr, &u, &p);
    যদি(!ফলাফল) {
      ফলাফল = CURL_EASY_STR_SETN(তথ্য, STRING_USERNAME, u);
      u = ফাঁকা;
    }
    যদি(!ফলাফল) {
      ফলাফল = CURL_EASY_STR_SETN(তথ্য, STRING_PASSWORD, p);
      p = ফাঁকা;
    }
    curlx_free(u);
    curlx_free(p);
    ফেরত ফলাফল;
  }

  ক্ষেত্র CURLOPT_USERNAME:
    ফেরত Curl_setstropt(তথ্য, STRING_USERNAME, ptr);

  ক্ষেত্র CURLOPT_PASSWORD:
    ফেরত Curl_setstropt(তথ্য, STRING_PASSWORD, ptr);

  ক্ষেত্র CURLOPT_LOGIN_OPTIONS:
    ফেরত Curl_setstropt(তথ্য, STRING_OPTIONS, ptr);

  ক্ষেত্র CURLOPT_XOAUTH2_BEARER:
    ফেরত Curl_setstropt(তথ্য, STRING_BEARER, ptr);
  ক্ষেত্র CURLOPT_RANGE:
    ফেরত Curl_setstropt(তথ্য, STRING_SET_RANGE, ptr);
  ক্ষেত্র CURLOPT_PRIVATE:
    s->private_তথ্য = ptr;
    বিরতি;
  ক্ষেত্র CURLOPT_PROTOCOLS_STR:
    যদি(ptr) {
      curl_prot_t protos;
      ফলাফল = protocol2num(ptr, &protos);
      যদি(!ফলাফল)
        s->allowed_protocols = protos;
    }
    অন্যথা
      /* make a NULL argument reset to default */
      s->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
    বিরতি;
  ক্ষেত্র CURLOPT_REDIR_PROTOCOLS_STR:
    যদি(ptr) {
      curl_prot_t protos;
      ফলাফল = protocol2num(ptr, &protos);
      যদি(!ফলাফল)
        s->redir_protocols = protos;
    }
    অন্যথা
      /* make a NULL argument reset to default */
      s->redir_protocols = (curl_prot_t)CURLPROTO_REDIR;
    বিরতি;
  ক্ষেত্র CURLOPT_DEFAULT_PROTOCOL:
    /* Set the protocol to use when the URL does not include any protocol */
    ফেরত Curl_setstropt(তথ্য, STRING_DEFAULT_PROTOCOL, ptr);
#ifndef CURL_DISABLE_SMTP
  ক্ষেত্র CURLOPT_MAIL_FROM:
    /* Set the SMTP mail originator */
    ফেরত Curl_setstropt(তথ্য, STRING_MAIL_FROM, ptr);
  ক্ষেত্র CURLOPT_MAIL_AUTH:
    /* Set the SMTP auth originator */
    ফেরত Curl_setstropt(তথ্য, STRING_MAIL_AUTH, ptr);
#endif
  ক্ষেত্র CURLOPT_SASL_AUTHZID:
    /* Authorization identity (identity to act as) */
    ফেরত Curl_setstropt(তথ্য, STRING_SASL_AUTHZID, ptr);
#ifndef CURL_DISABLE_RTSP
  ক্ষেত্র CURLOPT_RTSP_SESSION_ID:
    Curl_peer_unlink(&তথ্য->state.rtsp_session_origin);
    ফেরত Curl_setstropt(তথ্য, STRING_RTSP_SESSION_ID, ptr);
  ক্ষেত্র CURLOPT_RTSP_STREAM_URI:
    ফেরত Curl_setstropt(তথ্য, STRING_RTSP_STREAM_URI, ptr);
  ক্ষেত্র CURLOPT_RTSP_TRANSPORT:
    ফেরত Curl_setstropt(তথ্য, STRING_RTSP_TRANSPORT, ptr);
  ক্ষেত্র CURLOPT_INTERLEAVEDATA:
    s->rtp_out = ptr;
    বিরতি;
#endif /* !CURL_DISABLE_RTSP */
  ক্ষেত্র CURLOPT_TLSAUTH_USERNAME:
  ক্ষেত্র CURLOPT_TLSAUTH_PASSWORD:
  ক্ষেত্র CURLOPT_TLSAUTH_TYPE:
  ক্ষেত্র CURLOPT_PROXY_TLSAUTH_USERNAME:
  ক্ষেত্র CURLOPT_PROXY_TLSAUTH_PASSWORD:
  ক্ষেত্র CURLOPT_PROXY_TLSAUTH_TYPE:
    ফেরত CURLE_NOT_BUILT_IN;
#ifndef CURL_DISABLE_HSTS
  ক্ষেত্র CURLOPT_HSTSREADDATA:
    s->hsts_read_userp = ptr;
    বিরতি;
  ক্ষেত্র CURLOPT_HSTSWRITEDATA:
    s->hsts_write_userp = ptr;
    বিরতি;
  ক্ষেত্র CURLOPT_HSTS: {
    কাঠামো curl_slist *h;
    যদি(!তথ্য->hsts) {
      তথ্য->hsts = Curl_hsts_init();
      যদি(!তথ্য->hsts)
        ফেরত CURLE_OUT_OF_MEMORY;
    }
    যদি(ptr) {
      ফলাফল = Curl_setstropt(তথ্য, STRING_HSTS, ptr);
      যদি(ফলাফল)
        ফেরত ফলাফল;
      /* this needs to build a list of filenames to read from, so that it can
         read them later, as we might get a shared HSTS handle to load them
         into */
      h = curl_slist_append(তথ্য->state.hstslist, ptr);
      যদি(!h) {
        curl_slist_free_all(তথ্য->state.hstslist);
        তথ্য->state.hstslist = ফাঁকা;
        ফেরত CURLE_OUT_OF_MEMORY;
      }
      তথ্য->state.hstslist = h; /* store the list for later use */
    }
    অন্যথা {
      /* clear the list of HSTS files */
      curl_slist_free_all(তথ্য->state.hstslist);
      তথ্য->state.hstslist = ফাঁকা;
      যদি(!তথ্য->share || !তথ্য->share->hsts)
        /* throw away the HSTS cache unless shared */
        Curl_hsts_cleanup(&তথ্য->hsts);
    }
    বিরতি;
  }
#endif /* !CURL_DISABLE_HSTS */
#ifndef CURL_DISABLE_ALTSVC
  ক্ষেত্র CURLOPT_ALTSVC:
    যদি(!তথ্য->asi) {
      তথ্য->asi = Curl_altsvc_init();
      যদি(!তথ্য->asi)
        ফেরত CURLE_OUT_OF_MEMORY;
    }
    ফলাফল = Curl_setstropt(তথ্য, STRING_ALTSVC, ptr);
    যদি(ফলাফল)
      বিরতি;
    যদি(ptr)
      ফেরত Curl_altsvc_load(তথ্য->asi, ptr);
    বিরতি;
#endif /* !CURL_DISABLE_ALTSVC */
  ক্ষেত্র CURLOPT_ECH:
    ফেরত setopt_ech(তথ্য, ptr);
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode setopt_cptr(কাঠামো Curl_easy *তথ্য, CURLoption option,
                            অক্ষর *ptr)
{
  প্রকারলেখ CURLcode (*ptrfunc)(কাঠামো Curl_easy *তথ্য, CURLoption option,
                              অক্ষর *ptr);
  /* Order by likeliness */
  স্থির ধ্রুবক ptrfunc setopt_call[] = {
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

  জন্য(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode ফলাফল = setopt_call[i](তথ্য, option, ptr);
    যদি(ফলাফল != CURLE_UNKNOWN_OPTION)
      ফেরত ফলাফল;
  }
  ফেরত CURLE_UNKNOWN_OPTION;
}

স্থির CURLcode setopt_func(কাঠামো Curl_easy *তথ্য, CURLoption option,
                            va_list param)
{
  কাঠামো UserDefined *s = &তথ্য->set;
  নির্বাচন(option) {
  ক্ষেত্র CURLOPT_PROGRESSFUNCTION:
    /*
     * Progress callback function
     */
    s->fprogress = va_arg(param, curl_progress_callback);
    যদি(s->fprogress)
      তথ্য->progress.callback = TRUE; /* no longer internal */
    অন্যথা
      তথ্য->progress.callback = FALSE; /* NULL enforces internal */
    বিরতি;

  ক্ষেত্র CURLOPT_XFERINFOFUNCTION:
    /*
     * Transfer info callback function
     */
    s->fxferinfo = va_arg(param, curl_xferinfo_callback);
    যদি(s->fxferinfo)
      তথ্য->progress.callback = TRUE; /* no longer internal */
    অন্যথা
      তথ্য->progress.callback = FALSE; /* NULL enforces internal */

    বিরতি;
  ক্ষেত্র CURLOPT_DEBUGFUNCTION:
    /*
     * stderr write callback.
     */
    s->fdebug = va_arg(param, curl_debug_callback);
    /*
     * if the callback provided is NULL, it will use the default callback
     */
    বিরতি;
  ক্ষেত্র CURLOPT_HEADERFUNCTION:
    /*
     * Set header write callback
     */
    s->fwrite_header = va_arg(param, curl_write_callback);
    বিরতি;
  ক্ষেত্র CURLOPT_WRITEFUNCTION:
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
    বিরতি;
  ক্ষেত্র CURLOPT_READFUNCTION:
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
    বিরতি;
  ক্ষেত্র CURLOPT_SEEKFUNCTION:
    /*
     * Seek callback. Might be NULL.
     */
    s->seek_func = va_arg(param, curl_seek_callback);
    বিরতি;
  ক্ষেত্র CURLOPT_IOCTLFUNCTION:
    /*
     * I/O control callback. Might be NULL.
     */
    s->ioctl_func = va_arg(param, curl_ioctl_callback);
    বিরতি;
  ক্ষেত্র CURLOPT_SSL_CTX_FUNCTION:
    /*
     * Set an SSL_CTX callback
     */
#ifdef USE_SSL
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_SSL_CTX)) {
      s->ssl_fsslctx = va_arg(param, curl_ssl_ctx_callback);
      বিরতি;
    }
    অন্যথা
#endif
      ফেরত CURLE_NOT_BUILT_IN;

  ক্ষেত্র CURLOPT_SOCKOPTFUNCTION:
    /*
     * socket callback function: called after socket() but before connect()
     */
    s->fsockopt = va_arg(param, curl_sockopt_callback);
    বিরতি;

  ক্ষেত্র CURLOPT_OPENSOCKETFUNCTION:
    /*
     * open/create socket callback function: called instead of socket(),
     * before connect()
     */
    s->fopensocket = va_arg(param, curl_opensocket_callback);
    বিরতি;

  ক্ষেত্র CURLOPT_CLOSESOCKETFUNCTION:
    /*
     * close socket callback function: called instead of close()
     * when shutting down a connection
     */
    s->fclosesocket = va_arg(param, curl_closesocket_callback);
    বিরতি;

  ক্ষেত্র CURLOPT_RESOLVER_START_FUNCTION:
    /*
     * resolver start callback function: called before a new resolver request
     * is started
     */
    s->resolver_start = va_arg(param, curl_resolver_start_callback);
    বিরতি;

#ifdef USE_SSH
#ifdef USE_LIBSSH2
  ক্ষেত্র CURLOPT_SSH_HOSTKEYFUNCTION:
    /* the callback to check the hostkey without the knownhost file */
    s->ssh_hostkeyfunc = va_arg(param, curl_sshhostkeycallback);
    বিরতি;
#endif

  ক্ষেত্র CURLOPT_SSH_KEYFUNCTION:
    /* setting to NULL is fine since the ssh.c functions themselves will
       then revert to use the internal default */
    s->ssh_keyfunc = va_arg(param, curl_sshkeycallback);
    বিরতি;

#endif /* USE_SSH */

#ifndef CURL_DISABLE_RTSP
  ক্ষেত্র CURLOPT_INTERLEAVEFUNCTION:
    /* Set the user defined RTP write function */
    s->fwrite_rtp = va_arg(param, curl_write_callback);
    বিরতি;
#endif
#ifndef CURL_DISABLE_FTP
  ক্ষেত্র CURLOPT_CHUNK_BGN_FUNCTION:
    s->chunk_bgn = va_arg(param, curl_chunk_bgn_callback);
    বিরতি;
  ক্ষেত্র CURLOPT_CHUNK_END_FUNCTION:
    s->chunk_end = va_arg(param, curl_chunk_end_callback);
    বিরতি;
  ক্ষেত্র CURLOPT_FNMATCH_FUNCTION:
    s->fnmatch = va_arg(param, curl_fnmatch_callback);
    বিরতি;
#endif
#ifndef CURL_DISABLE_HTTP
  ক্ষেত্র CURLOPT_TRAILERFUNCTION:
    s->trailer_callback = va_arg(param, curl_trailer_callback);
    বিরতি;
#endif
#ifndef CURL_DISABLE_HSTS
  ক্ষেত্র CURLOPT_HSTSREADFUNCTION:
    s->hsts_read = va_arg(param, curl_hstsread_callback);
    বিরতি;
  ক্ষেত্র CURLOPT_HSTSWRITEFUNCTION:
    s->hsts_write = va_arg(param, curl_hstswrite_callback);
    বিরতি;
#endif
  ক্ষেত্র CURLOPT_PREREQFUNCTION:
    s->fprereq = va_arg(param, curl_prereq_callback);
    বিরতি;
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode setopt_offt(কাঠামো Curl_easy *তথ্য, CURLoption option,
                            curl_off_t offt)
{
  কাঠামো UserDefined *s = &তথ্য->set;
  নির্বাচন(option) {
  ক্ষেত্র CURLOPT_TIMEVALUE_LARGE:
    /*
     * This is the value to compare with the remote document with the
     * method set with CURLOPT_TIMECONDITION
     */
    s->timevalue = (time_t)offt;
    বিরতি;

    /* MQTT "borrows" some of the HTTP options */
  ক্ষেত্র CURLOPT_POSTFIELDSIZE_LARGE:
    /*
     * The size of the POSTFIELD data to prevent libcurl to do strlen() to
     * figure it out. Enables binary posts.
     */
    যদি(offt < -1)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

    যদি(s->postfieldsize < offt && s->str_copypostfields) {
      /* Previous CURLOPT_COPYPOSTFIELDS is no longer valid. */
      curlx_safefree(s->str_copypostfields);
      s->postfields = ফাঁকা;
    }
    s->postfieldsize = offt;
    বিরতি;
  ক্ষেত্র CURLOPT_INFILESIZE_LARGE:
    /*
     * If known, this should inform curl about the file size of the
     * to-be-uploaded file.
     */
    যদি(offt < -1)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = offt;
    বিরতি;
  ক্ষেত্র CURLOPT_MAX_SEND_SPEED_LARGE:
    /*
     * When transfer uploads are faster than CURLOPT_MAX_SEND_SPEED_LARGE
     * bytes per second the transfer is throttled..
     */
    যদি(offt < 0)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_send_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&তথ্য->progress.ul.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    বিরতি;
  ক্ষেত্র CURLOPT_MAX_RECV_SPEED_LARGE:
    /*
     * When receiving data faster than CURLOPT_MAX_RECV_SPEED_LARGE bytes per
     * second the transfer is throttled..
     */
    যদি(offt < 0)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_recv_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&তথ্য->progress.dl.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    বিরতি;
  ক্ষেত্র CURLOPT_RESUME_FROM_LARGE:
    /*
     * Resume transfer at the given file position
     */
    যদি(offt < -1)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = offt;
    বিরতি;
  ক্ষেত্র CURLOPT_MAXFILESIZE_LARGE:
    /*
     * Set the maximum size of a file to download.
     */
    যদি(offt < 0)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_filesize = offt;
    বিরতি;

  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode setopt_blob(কাঠামো Curl_easy *তথ্য, CURLoption option,
                            কাঠামো curl_blob *blob)
{
  কাঠামো UserDefined *s = &তথ্য->set;
  নির্বাচন(option) {
  ক্ষেত্র CURLOPT_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use
     */
    ফেরত Curl_setblobopt(&s->blobs[BLOB_CERT], blob);
#ifndef CURL_DISABLE_PROXY
  ক্ষেত্র CURLOPT_PROXY_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use for proxy
     */
    ফেরত Curl_setblobopt(&s->blobs[BLOB_CERT_PROXY], blob);
  ক্ষেত্র CURLOPT_PROXY_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use for proxy
     */
    ফেরত Curl_setblobopt(&s->blobs[BLOB_চাবি_PROXY], blob);
  ক্ষেত্র CURLOPT_PROXY_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection proxy.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_CAINFO_BLOB)) {
      CURLcode ফলাফল = Curl_setblobopt(&s->blobs[BLOB_CAINFO_PROXY], blob);
      যদি(!ফলাফল)
        s->proxy_ssl.custom_cablob = TRUE;
      ফেরত ফলাফল;
    }
#endif
    ফেরত CURLE_NOT_BUILT_IN;
  ক্ষেত্র CURLOPT_PROXY_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_ISSUERCERT_BLOB))
      ফেরত Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT_PROXY], blob);
    ফেরত CURLE_NOT_BUILT_IN;
#endif
  ক্ষেত্র CURLOPT_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use
     */
    ফেরত Curl_setblobopt(&s->blobs[BLOB_চাবি], blob);
  ক্ষেত্র CURLOPT_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_CAINFO_BLOB)) {
      CURLcode ফলাফল = Curl_setblobopt(&s->blobs[BLOB_CAINFO], blob);
      যদি(!ফলাফল)
        s->ssl.custom_cablob = TRUE;
      ফেরত ফলাফল;
    }
#endif
    ফেরত CURLE_NOT_BUILT_IN;
  ক্ষেত্র CURLOPT_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    যদি(Curl_ssl_supports(তথ্য, SSLSUPP_ISSUERCERT_BLOB))
      ফেরত Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT], blob);
    ফেরত CURLE_NOT_BUILT_IN;

  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }
  /* unreachable */
}

/*
 * Do not make Curl_vsetopt() static: it is called from
 * projects/OS400/ccsidcurl.c.
 */
CURLcode Curl_vsetopt(কাঠামো Curl_easy *তথ্য, CURLoption option, va_list param)
{
  যদি(option < CURLOPTTYPE_OBJECTPOINT)
    ফেরত setopt_long(তথ্য, option, va_arg(param, long));
  অন্যথা যদি(option < CURLOPTTYPE_FUNCTIONPOINT) {
    /* unfortunately, different pointer types cannot be identified any other
       way than being listed explicitly */
    নির্বাচন(option) {
    ক্ষেত্র CURLOPT_HTTPHEADER:
    ক্ষেত্র CURLOPT_QUOTE:
    ক্ষেত্র CURLOPT_POSTQUOTE:
    ক্ষেত্র CURLOPT_TELNETOPTIONS:
    ক্ষেত্র CURLOPT_PREQUOTE:
    ক্ষেত্র CURLOPT_HTTP200ALIASES:
    ক্ষেত্র CURLOPT_MAIL_RCPT:
    ক্ষেত্র CURLOPT_RESOLVE:
    ক্ষেত্র CURLOPT_PROXYHEADER:
    ক্ষেত্র CURLOPT_CONNECT_TO:
      ফেরত setopt_slist(তথ্য, option, va_arg(param, কাঠামো curl_slist *));
    ক্ষেত্র CURLOPT_HTTPPOST:         /* curl_httppost * */
    ক্ষেত্র CURLOPT_MIMEPOST:         /* curl_mime * */
    ক্ষেত্র CURLOPT_STDERR:           /* FILE * */
    ক্ষেত্র CURLOPT_SHARE:            /* CURLSH * */
    ক্ষেত্র CURLOPT_CURLU:            /* CURLU * */
      ফেরত setopt_pointers(তথ্য, option, param);
    ক্ষেত্র CURLOPT_STREAM_DEPENDS:   /* CURL * */
    ক্ষেত্র CURLOPT_STREAM_DEPENDS_E: /* CURL * */
      ফেরত CURLE_OK;
    পূর্বনির্ধারিত:
      বিরতি;
    }
    /* the char pointer options */
    ফেরত setopt_cptr(তথ্য, option, va_arg(param, অক্ষর *));
  }
  অন্যথা যদি(option < CURLOPTTYPE_OFF_T)
    ফেরত setopt_func(তথ্য, option, param);
  অন্যথা যদি(option < CURLOPTTYPE_BLOB)
    ফেরত setopt_offt(তথ্য, option, va_arg(param, curl_off_t));
  ফেরত setopt_blob(তথ্য, option, va_arg(param, কাঠামো curl_blob *));
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
  কাঠামো Curl_eapi_guard guard;
  CURLcode ফলাফল;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_setopt, &ফলাফল)) {
    কাঠামো Curl_easy *তথ্য = curl;
    va_list arg;

    va_start(arg, option);

    ফলাফল = Curl_vsetopt(তথ্য, option, arg);

    va_end(arg);
    যদি(ফলাফল == CURLE_BAD_FUNCTION_ARGUMENT)
      failf(তথ্য, "setopt 0x%x got bad argument", (unsigned পূর্ণসংখ্যা)option);
  }
  CURL_EAPI_LEAVE(&guard);
  ফেরত ফলাফল;
}
