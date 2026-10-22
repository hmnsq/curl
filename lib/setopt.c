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

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#شامل <linux/tcp.h>
#وگرنهاگر defined(HAVE_NETINET_TCP_H)
#شامل <netinet/tcp.h>
#endif

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

ایستا CURLcode setopt_set_timeout_sec(timediff_t *ptimeout_ms, long secs)
{
  اگر(secs < 0)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
#اگر LONG_MAX > (TIMEDIFF_T_MAX / 1000)
  اگر(secs > (TIMEDIFF_T_MAX / 1000)) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    بازگشت CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)secs * 1000;
  بازگشت CURLE_OK;
}

ایستا CURLcode setopt_set_timeout_ms(timediff_t *ptimeout_ms, long ms)
{
  اگر(ms < 0)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
#اگر LONG_MAX > TIMEDIFF_T_MAX
  اگر(ms > TIMEDIFF_T_MAX) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    بازگشت CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)ms;
  بازگشت CURLE_OK;
}

CURLcode Curl_setstropt(ساختار Curl_easy *data,
                        شمارش dupstring id, ثابت نویسه *s)
{
  size_t slen = s ? strlen(s) : 0;
  DEBUGASSERT((unsigned)id <= UINT8_MAX);
  اگر(s && (slen > CURL_MAX_درون_داد_LENGTH))
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  بازگشت CURL_EASY_STR_SET(data, (uint8_t)id, s, slen);
}

CURLcode Curl_setblobopt(ساختار curl_blob **blobp,
                         ثابت ساختار curl_blob *blob)
{
  /* free the previous storage at `blobp' and replace by a dynamic storage
     copy of blob. If CURL_BLOB_COPY is set, the data is copied. */

  curlx_safefree(*blobp);

  اگر(blob) {
    ساختار curl_blob *nblob;
    اگر(!blob->data || !blob->len || (blob->len > CURL_MAX_درون_داد_LENGTH))
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    nblob = (ساختار curl_blob *)
      curlx_malloc(اندازه(ساختار curl_blob) +
                   ((blob->flags & CURL_BLOB_COPY) ? blob->len : 0));
    اگر(!nblob)
      بازگشت CURLE_OUT_OF_MEMORY;
    *nblob = *blob;
    اگر(blob->flags & CURL_BLOB_COPY) {
      /* put the data after the blob struct in memory */
      nblob->data = (نویسه *)nblob + اندازه(ساختار curl_blob);
      memcpy(nblob->data, blob->data, blob->len);
    }

    *blobp = nblob;
    بازگشت CURLE_OK;
  }

  بازگشت CURLE_OK;
}

ایستا CURLcode setstropt_userpwd(ثابت نویسه *option, نویسه **userp,
                                  نویسه **passwdp)
{
  نویسه *user = پوچ;
  نویسه *passwd = پوچ;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Parse the login details if specified. If not, then we treat NULL as a
     hint to clear the existing data */
  اگر(option) {
    size_t len = strlen(option);
    CURLcode result;
    اگر(len > CURL_MAX_درون_داد_LENGTH)
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

    result = Curl_parse_login_details(option, len, &user, &passwd, پوچ);
    اگر(result)
      بازگشت result;
  }

  curlx_free(*userp);
  *userp = user;

  curlx_strzero(*passwdp);
  curlx_free(*passwdp);
  *passwdp = passwd;

  بازگشت CURLE_OK;
}

ایستا CURLcode setstropt_interface(ساختار Curl_easy *data, نویسه *option)
{
  اگر(option && !option[0])
    option = پوچ;
  اگر(option) {
    /* Parse for checking syntax */
    CURLcode result = Curl_parse_interface(option, پوچ, پوچ, پوچ);
    اگر(result)
      بازگشت result;
    بازگشت CURL_EASY_STR_SET(data, STRING_INTERFACE_OPT,
                             option, strlen(option));
  }
  CURL_EASY_STR_CLEAR(data, STRING_INTERFACE_OPT);
  بازگشت CURLE_OK;
}

#ifdef USE_SSL
#define C_SSLVERSION_مقدار(x)     ((x) & 0xffff)
#define C_SSLVERSION_MAX_مقدار(x) ((unsigned long)(x) & 0xffff0000)
#endif

ایستا CURLcode protocol2num(ثابت نویسه *str, curl_prot_t *val)
{
  /*
   * We are asked to cherry-pick protocols, so play it safe and disallow all
   * protocols to start with, and re-add the wanted ones back in.
   */
  *val = 0;

  اگر(!str)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  اگر(curl_strequal(str, "all")) {
    *val = ~(curl_prot_t)0;
    بازگشت CURLE_OK;
  }

  انجامبده {
    ثابت نویسه *token = str;
    size_t tlen;

    str = strchr(str, ',');
    tlen = str ? (size_t)(str - token) : strlen(token);
    اگر(tlen) {
      ثابت ساختار Curl_scheme *h = Curl_getn_scheme(token, tlen);

      اگر(!h || !h->run)
        بازگشت CURLE_UNSUPPORTED_PROTOCOL;

      *val |= h->protocol;
    }
  } وقتی(str && str++);

  اگر(!*val)
    /* no protocol listed */
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  بازگشت CURLE_OK;
}

#اگر !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
ایستا CURLcode httpauth(ساختار Curl_easy *data, bool proxy,
                         unsigned long auth)
{
  اگر(auth != CURLAUTH_NONE) {
    صحیح bitcheck = 0;
    bool authbits = FALSE;
    اگر(auth & CURLAUTH_DIGEST_IE) {
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
    وقتی(bitcheck < 31) {
      اگر(auth & (1UL << bitcheck++)) {
        authbits = TRUE;
        بشکن;
      }
    }
    اگر(!authbits)
      بازگشت CURLE_NOT_BUILT_IN; /* no supported types left! */
  }
  اگر(proxy)
    data->set.proxyauth = (uint32_t)auth;
  وگرنه
    data->set.httpauth = (uint32_t)auth;
  بازگشت CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */

#ifndef CURL_DISABLE_HTTP
ایستا CURLcode setopt_HTTP_VERSION(ساختار Curl_easy *data, long arg)
{
  /*
   * This sets a requested HTTP version to be used. The value is one of
   * the listed enums in curl/curl.h.
   */
  انتخاب(arg) {
  حالت CURL_HTTP_VERSION_NONE:
    /* accepted */
    بشکن;
  حالت CURL_HTTP_VERSION_1_0:
  حالت CURL_HTTP_VERSION_1_1:
    /* accepted */
    بشکن;
#ifdef USE_HTTP2
  حالت CURL_HTTP_VERSION_2_0:
  حالت CURL_HTTP_VERSION_2TLS:
  حالت CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    /* accepted */
    بشکن;
#endif
#ifdef USE_HTTP3
  حالت CURL_HTTP_VERSION_3:
  حالت CURL_HTTP_VERSION_3ONLY:
    /* accepted */
    بشکن;
#endif
  پیشفرض:
    /* not accepted */
    اگر(arg < CURL_HTTP_VERSION_NONE)
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    بازگشت CURLE_UNSUPPORTED_PROTOCOL;
  }
  data->set.httpwant = (unsigned نویسه)arg;
  بازگشت CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP */

#ifdef USE_SSL
CURLcode Curl_setopt_SSLVERSION(ساختار Curl_easy *data, CURLoption option,
                                long arg)
{
  /*
   * Set explicit SSL version to try to connect with, as some SSL
   * implementations are lame.
   */
  {
    long version, version_max;
    ساختار ssl_easy_config *sslc = &data->set.ssl;
#ifndef CURL_DISABLE_PROXY
    اگر(option != CURLOPT_SSLVERSION)
      sslc = &data->set.proxy_ssl;
#وگرنه
    (تهی)option; /* unused */
#endif
    version = C_SSLVERSION_مقدار(arg);
    version_max = (long)C_SSLVERSION_MAX_مقدار(arg);

    اگر(version < CURL_SSLVERSION_DEFAULT ||
       version == CURL_SSLVERSION_SSLv2 ||
       version == CURL_SSLVERSION_SSLv3 ||
       version >= CURL_SSLVERSION_LAST ||
       version_max < CURL_SSLVERSION_MAX_NONE ||
       version_max >= CURL_SSLVERSION_MAX_LAST)
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    اگر(version == CURL_SSLVERSION_DEFAULT)
      version = CURL_SSLVERSION_TLSv1_2;

    sslc->version = (unsigned نویسه)version;
    sslc->version_max = (unsigned صحیح)version_max;
  }
  بازگشت CURLE_OK;
}
#endif /* !USE_SSL */

#ifndef CURL_DISABLE_RTSP
ایستا CURLcode setopt_RTSP_REQUEST(ساختار Curl_easy *data, long arg)
{
  /*
   * Set the RTSP request method (OPTIONS, SETUP, PLAY, etc...) Would this be
   * better if the RTSPREQ_* were moved into here?
   */
  اگر((arg <= CURL_RTSPREQ_NONE) || (arg >= CURL_RTSPREQ_LAST))
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  data->set.rtspreq = (unsigned نویسه)arg;
  بازگشت CURLE_OK;
}
#endif /* !CURL_DISABLE_RTSP */

ایستا CURLcode setopt_long_bool(ساختار Curl_easy *data, CURLoption option,
                                 long arg)
{
  bool enabled = !!arg;
  صحیح ok = 1;
  ساختار UserDefined *s = &data->set;
  انتخاب(option) {
  حالت CURLOPT_FORBID_REUSE:
    /*
     * When this transfer is done, it must not be left to be reused by a
     * subsequent transfer but shall be closed immediately.
     */
    s->reuse_forbid = enabled;
    بشکن;
  حالت CURLOPT_FRESH_CONNECT:
    /*
     * This transfer shall not use a previously cached connection but
     * should be made with a fresh new connect!
     */
    s->reuse_fresh = enabled;
    بشکن;
  حالت CURLOPT_VERBOSE:
    /*
     * Verbose means infof() calls that give a lot of information about
     * the connection and transfer procedures as well as internal choices.
     */
    s->verbose = enabled;
    بشکن;
  حالت CURLOPT_HEADER:
    /*
     * Set to include the header in the general data output stream.
     */
    s->include_header = enabled;
    بشکن;
  حالت CURLOPT_NOPROGRESS:
    /*
     * Shut off the internal supported progress meter
     */
    data->progress.hide = enabled;
    بشکن;
  حالت CURLOPT_NOBODY:
    /*
     * Do not include the body part in the output data stream.
     */
    s->opt_no_body = enabled;
#ifndef CURL_DISABLE_HTTP
    اگر(s->opt_no_body)
      /* in HTTP lingo, no body means using the HEAD request... */
      s->method = HTTPREQ_HEAD;
    وگرنه اگر(s->method == HTTPREQ_HEAD)
      s->method = HTTPREQ_GET;
#endif
    بشکن;
  حالت CURLOPT_FAILONERROR:
    /*
     * Do not output the >=400 error code HTML-page, but instead only
     * return error.
     */
    s->http_fail_on_error = enabled;
    بشکن;
  حالت CURLOPT_KEEP_SENDING_ON_ERROR:
    s->http_keep_sending_on_error = enabled;
    بشکن;
  حالت CURLOPT_UPLOAD:
  حالت CURLOPT_PUT:
    /*
     * We want to send data to the remote host. If this is HTTP, that equals
     * using the PUT request.
     */
    اگر(enabled) {
      /* If this is HTTP, PUT is what's needed to "upload" */
      s->method = HTTPREQ_PUT;
      s->opt_no_body = FALSE; /* this is implied */
    }
    وگرنه
      /* In HTTP, the opposite of upload is GET (unless NOBODY is true as
         then this can be changed to HEAD later on) */
      s->method = HTTPREQ_GET;
    بشکن;
  حالت CURLOPT_FILETIME:
    /*
     * Try to get the file time of the remote document. The time will
     * later (possibly) become available using curl_easy_getinfo().
     */
    s->get_filetime = enabled;
    بشکن;
#ifndef CURL_DISABLE_HTTP
  حالت CURLOPT_HTTP09_ALLOWED:
    s->http09_allowed = enabled;
    بشکن;
#ifndef CURL_DISABLE_COOKIES
  حالت CURLOPT_COOKIESESSION:
    /*
     * Set this option to TRUE to start a new "cookie session". It will
     * prevent the forthcoming read-cookies-from-file actions to accept
     * cookies that are marked as being session cookies, as they belong to a
     * previous session.
     */
    s->cookiesession = enabled;
    بشکن;
#endif
  حالت CURLOPT_AUTOREFERER:
    /*
     * Switch on automatic referer that gets set if curl follows locations.
     */
    s->http_auto_referer = enabled;
    بشکن;
  حالت CURLOPT_TRANSFER_ENCODING:
    s->http_transfer_encoding = enabled;
    بشکن;
  حالت CURLOPT_UNRESTRICTED_AUTH:
    /*
     * Send authentication (user+password) when following locations, even when
     * hostname changed.
     */
    s->allow_auth_to_other_hosts = enabled;
    بشکن;
  حالت CURLOPT_HTTP_TRANSFER_DECODING:
    /*
     * disable libcurl transfer encoding is used
     */
    s->http_te_skip = !enabled; /* reversed */
    بشکن;
  حالت CURLOPT_HTTP_CONTENT_DECODING:
    /*
     * raw data passed to the application when content encoding is used
     */
    s->http_ce_skip = !enabled; /* reversed */
    بشکن;
  حالت CURLOPT_HTTPGET:
    /*
     * Set to force us do HTTP GET
     */
    اگر(enabled) {
      s->method = HTTPREQ_GET;
      s->opt_no_body = FALSE; /* this is implied */
    }
    بشکن;
  حالت CURLOPT_POST:
    /* Does this option serve a purpose anymore? Yes it does, when
       CURLOPT_POSTFIELDS is not used and the POST data is read off the
       callback! */
    اگر(enabled) {
      s->method = HTTPREQ_POST;
      s->opt_no_body = FALSE; /* this is implied */
    }
    وگرنه
      s->method = HTTPREQ_GET;
    بشکن;
#endif /* !CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_PROXY
  حالت CURLOPT_HTTPPROXYTUNNEL:
    /*
     * Tunnel operations through the proxy instead of normal proxy use
     */
    s->tunnel_thru_httpproxy = enabled;
    بشکن;
  حالت CURLOPT_HAPROXYPROTOCOL:
    /*
     * Set to send the HAProxy Proxy Protocol header
     */
    s->haproxyprotocol = enabled;
    بشکن;
  حالت CURLOPT_PROXY_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for proxy.
     */
    s->proxy_ssl.verifypeer = enabled;

    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    بشکن;
  حالت CURLOPT_PROXY_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for proxy
     */
    s->proxy_ssl.verifyhost = enabled;
    ok = 2;
    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    بشکن;
  حالت CURLOPT_PROXY_TRANSFER_MODE:
    /*
     * set transfer mode (;type=<a|i>) when doing FTP via an HTTP proxy
     */
    s->proxy_transfer_mode = enabled;
    بشکن;
#endif /* !CURL_DISABLE_PROXY */
#اگر defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  حالت CURLOPT_SOCKS5_GSSAPI_NEC:
    /*
     * Set flag for NEC SOCKS5 support
     */
    s->socks5_gssapi_nec = enabled;
    بشکن;
#endif
#ifdef CURL_LIST_ONLY_PROTOCOL
  حالت CURLOPT_DIRLISTONLY:
    /*
     * An option that changes the command to one that asks for a list only, no
     * file info details. Used for FTP, POP3 and SFTP.
     */
    s->list_only = enabled;
    بشکن;
#endif
  حالت CURLOPT_APPEND:
    /*
     * We want to upload and append to an existing file. Used for FTP and
     * SFTP.
     */
    s->remote_append = enabled;
    بشکن;
#ifndef CURL_DISABLE_FTP
  حالت CURLOPT_FTP_USE_EPRT:
    s->ftp_use_eprt = enabled;
    بشکن;
  حالت CURLOPT_FTP_USE_EPSV:
    s->ftp_use_epsv = enabled;
    بشکن;
  حالت CURLOPT_FTP_USE_PRET:
    s->ftp_use_pret = enabled;
    بشکن;
  حالت CURLOPT_FTP_SKIP_PASV_IP:
    /*
     * Enable or disable FTP_SKIP_PASV_IP, which will disable/enable the
     * bypass of the IP address in PASV responses.
     */
    s->ftp_skip_ip = enabled;
    بشکن;
  حالت CURLOPT_WILDCARDMATCH:
    s->wildcard_enabled = enabled;
    بشکن;
#endif
  حالت CURLOPT_CRLF:
    /*
     * Kludgy option to enable CRLF conversions. Subject for removal.
     */
    s->crlf = enabled;
    بشکن;
#ifndef CURL_DISABLE_TFTP
  حالت CURLOPT_TFTP_NO_OPTIONS:
    /*
     * Option that prevents libcurl from sending TFTP option requests to the
     * server.
     */
    s->tftp_no_options = enabled;
    بشکن;
#endif /* !CURL_DISABLE_TFTP */
  حالت CURLOPT_TRANSFERTEXT:
    /*
     * This option was previously named 'FTPASCII'. Renamed to work with
     * more protocols than merely FTP.
     *
     * Transfer using ASCII (instead of BINARY).
     */
    s->prefer_ascii = enabled;
    بشکن;
  حالت CURLOPT_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying.
     */
    s->ssl.verifypeer = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    بشکن;
#ifndef CURL_DISABLE_DOH
  حالت CURLOPT_DOH_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for DoH.
     */
    s->doh_verifypeer = enabled;
    بشکن;
  حالت CURLOPT_DOH_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for DoH
     */
    s->doh_verifyhost = enabled;
    ok = 2;
    بشکن;
  حالت CURLOPT_DOH_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying for DoH.
     */
    اگر(!Curl_ssl_cert_status_request())
      بازگشت CURLE_NOT_BUILT_IN;

    s->doh_verifystatus = enabled;
    ok = 2;
    بشکن;
#endif /* !CURL_DISABLE_DOH */
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
    Curl_ssl_conn_config_update(data, FALSE);
    بشکن;
  حالت CURLOPT_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying.
     */
    اگر(!Curl_ssl_cert_status_request())
      بازگشت CURLE_NOT_BUILT_IN;

    s->ssl.verifystatus = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    بشکن;
  حالت CURLOPT_CERTINFO:
#ifdef USE_SSL
    اگر(Curl_ssl_supports(data, SSLSUPP_CERTINFO))
      s->ssl.certinfo = enabled;
    وگرنه
#endif
      بازگشت CURLE_NOT_BUILT_IN;
    بشکن;
  حالت CURLOPT_NOSIGNAL:
    /*
     * The application asks not to set any signal() or alarm() handlers,
     * even when using a timeout.
     */
    s->no_signal = enabled;
    بشکن;
  حالت CURLOPT_TCP_NODELAY:
    /*
     * Enable or disable TCP_NODELAY, which will disable/enable the Nagle
     * algorithm
     */
    s->tcp_nodelay = enabled;
    بشکن;
  حالت CURLOPT_IGNORE_CONTENT_LENGTH:
    s->ignorecl = enabled;
    بشکن;
  حالت CURLOPT_SSL_SESSIONID_CACHE:
    s->ssl.cache_session = enabled;
#ifndef CURL_DISABLE_PROXY
    s->proxy_ssl.cache_session = s->ssl.cache_session;
#endif
    بشکن;
#ifdef USE_SSH
  حالت CURLOPT_SSH_COMPRESSION:
    s->ssh_compression = enabled;
    بشکن;
#endif /* !USE_SSH */
#ifndef CURL_DISABLE_SMTP
  حالت CURLOPT_MAIL_RCPT_ALLOWFAILS:
    /* allow RCPT TO command to fail for some recipients */
    s->mail_rcpt_allowfails = enabled;
    بشکن;
#endif /* !CURL_DISABLE_SMTP */
  حالت CURLOPT_SASL_IR:
    /* Enable/disable SASL initial response */
    s->sasl_ir = enabled;
    بشکن;
  حالت CURLOPT_TCP_KEEPALIVE:
    s->tcp_keepalive = enabled;
    بشکن;
  حالت CURLOPT_TCP_FASTOPEN:
#اگر defined(CONNECT_DATA_IDEMPOTENT) || defined(MSG_FASTOPEN) || \
  defined(TCP_FASTOPEN_CONNECT)
    s->tcp_fastopen = enabled;
    بشکن;
#وگرنه
    بازگشت CURLE_NOT_BUILT_IN;
#endif
  حالت CURLOPT_SSL_ENABLE_ALPN:
    s->ssl_enable_alpn = enabled;
    بشکن;
  حالت CURLOPT_PATH_AS_IS:
    s->path_as_is = enabled;
    بشکن;
  حالت CURLOPT_PIPEWAIT:
    s->pipewait = enabled;
    بشکن;
  حالت CURLOPT_SUPPRESS_CONNECT_HEADERS:
    s->suppress_connect_headers = enabled;
    بشکن;
#ifndef CURL_DISABLE_SHUFFLE_DNS
  حالت CURLOPT_DNS_SHUFFLE_ADDRESSES:
    s->dns_shuffle_addresses = enabled;
    بشکن;
#endif
  حالت CURLOPT_DISALLOW_USERNAME_IN_URL:
    s->disallow_username_in_url = enabled;
    بشکن;
  حالت CURLOPT_QUICK_EXIT:
    s->quick_exit = enabled;
    بشکن;
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }
  اگر((arg > ok) || (arg < 0))
    /* reserve other values for future use */
    infof(data, "boolean setopt(%d) got unsupported argument %ld,"
          " treated as %d", (صحیح)option, arg, enabled);

  بازگشت CURLE_OK;
}

ایستا CURLcode مقدار_range(long *مقدار, long below_error, long min, long max)
{
  اگر(*مقدار < below_error)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  وگرنه اگر(*مقدار < min)
    *مقدار = min;
  وگرنه اگر(*مقدار > max)
    *مقدار = max;
  بازگشت CURLE_OK;
}

ایستا CURLcode setopt_long_net(ساختار Curl_easy *data, CURLoption option,
                                long arg)
{
  CURLcode result = CURLE_OK;
  ساختار UserDefined *s = &data->set;

  انتخاب(option) {
  حالت CURLOPT_DNS_CACHE_TIMEOUT:
    اگر(arg != -1)
      بازگشت setopt_set_timeout_sec(&s->dns_cache_timeout_ms, arg);
    s->dns_cache_timeout_ms = -1;
    بشکن;
  حالت CURLOPT_MAXCONNECTS:
    result = مقدار_range(&arg, 0, 0, INT_MAX);
    اگر(!result)
      s->maxconnects = arg ? (uint32_t)arg : DEFAULT_CONNCACHE_SIZE;
    بشکن;
  حالت CURLOPT_SERVER_RESPONSE_TIMEOUT:
    بازگشت setopt_set_timeout_sec(&s->server_response_timeout, arg);
  حالت CURLOPT_SERVER_RESPONSE_TIMEOUT_MS:
    بازگشت setopt_set_timeout_ms(&s->server_response_timeout, arg);
  حالت CURLOPT_کم_SPEED_LIMIT:
    اگر(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    وگرنه
      s->کم_speed_limit = arg;
    بشکن;
  حالت CURLOPT_کم_SPEED_TIME:
    result = مقدار_range(&arg, 0, 0, USHRT_MAX);
    اگر(!result)
      s->کم_speed_time = (uint16_t)arg;
    بشکن;
  حالت CURLOPT_PORT:
    اگر((arg < 0) || (arg > 65535))
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    s->use_port = (unsigned short)arg;
    بشکن;
  حالت CURLOPT_TIMEOUT:
    بازگشت setopt_set_timeout_sec(&s->timeout, arg);
  حالت CURLOPT_TIMEOUT_MS:
    بازگشت setopt_set_timeout_ms(&s->timeout, arg);
  حالت CURLOPT_CONNECTTIMEOUT:
    بازگشت setopt_set_timeout_sec(&s->connecttimeout, arg);
  حالت CURLOPT_CONNECTTIMEOUT_MS:
    بازگشت setopt_set_timeout_ms(&s->connecttimeout, arg);
#ifndef CURL_DISABLE_BINDLOCAL
  حالت CURLOPT_LOCALPORT:
    اگر((arg < 0) || (arg > 65535))
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    s->localport = curlx_sltous(arg);
    بشکن;
  حالت CURLOPT_LOCALPORTRANGE:
    اگر((arg < 0) || (arg > 65535))
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    s->localportrange = curlx_sltous(arg);
    بشکن;
#endif
  حالت CURLOPT_BUFFERSIZE:
    result = مقدار_range(&arg, 0, READBUFFER_MIN, READBUFFER_MAX);
    اگر(!result)
      s->buffer_size = (unsigned صحیح)arg;
    بشکن;
  حالت CURLOPT_UPLOAD_BUFFERSIZE:
    result = مقدار_range(&arg, 0, UPLOADBUFFER_MIN, UPLOADBUFFER_MAX);
    اگر(!result)
      s->upload_buffer_size = (unsigned صحیح)arg;
    بشکن;
  حالت CURLOPT_MAXFILESIZE:
    اگر(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    وگرنه
      s->max_filesize = arg;
    بشکن;
  حالت CURLOPT_IPRESOLVE:
    اگر((arg < CURL_IPRESOLVE_WHATEVER) || (arg > CURL_IPRESOLVE_V6))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    وگرنه
      s->ipver = (unsigned نویسه)arg;
    بشکن;
  حالت CURLOPT_CONNECT_ONLY:
    اگر(arg < 0 || arg > 2)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    وگرنه {
      s->connect_only = !!arg;
      s->connect_only_ws = (arg == 2);
    }
    بشکن;
#ifdef USE_IPV6
  حالت CURLOPT_ADDRESS_SCOPE:
#اگر SIZEOF_LONG > 4
    اگر((unsigned long)arg > UINT_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    وگرنه
#endif
    s->scope_id = (unsigned صحیح)arg;
    بشکن;
#endif
  حالت CURLOPT_TCP_KEEPIDLE:
    result = مقدار_range(&arg, 0, 0, INT_MAX);
    اگر(!result)
      s->tcp_keepidle = (صحیح)arg;
    بشکن;
  حالت CURLOPT_TCP_KEEPINTVL:
    result = مقدار_range(&arg, 0, 0, INT_MAX);
    اگر(!result)
      s->tcp_keepintvl = (صحیح)arg;
    بشکن;
  حالت CURLOPT_TCP_KEEPCNT:
    result = مقدار_range(&arg, 0, 0, INT_MAX);
    اگر(!result)
      s->tcp_keepcnt = (صحیح)arg;
    بشکن;
  حالت CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
    بازگشت setopt_set_timeout_ms(&s->happy_eyeballs_timeout, arg);
  حالت CURLOPT_UPKEEP_INTERVAL_MS:
    بازگشت setopt_set_timeout_ms(&s->upkeep_interval_ms, arg);
  حالت CURLOPT_MAXAGE_CONN:
    بازگشت setopt_set_timeout_sec(&s->conn_max_idle_ms, arg);
  حالت CURLOPT_MAXLIFETIME_CONN:
    بازگشت setopt_set_timeout_sec(&s->conn_max_age_ms, arg);
  حالت CURLOPT_DNS_USE_GLOBAL_CACHE:
    /* deprecated */
    بشکن;
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }
  بازگشت result;
}

ایستا CURLcode setopt_long_ssl(ساختار Curl_easy *data, CURLoption option,
                                long arg)
{
#ifdef USE_SSL
  CURLcode result = CURLE_OK;
  ساختار UserDefined *s = &data->set;
  انتخاب(option) {
  حالت CURLOPT_CA_CACHE_TIMEOUT:
    اگر(Curl_ssl_supports(data, SSLSUPP_CA_CACHE)) {
      result = مقدار_range(&arg, -1, -1, INT_MAX);
      اگر(!result)
        s->ssl_ca_cache_timeout = (صحیح)arg;
    }
    وگرنه
      result = CURLE_NOT_BUILT_IN;
    بشکن;
  حالت CURLOPT_SSLVERSION:
#ifndef CURL_DISABLE_PROXY
  حالت CURLOPT_PROXY_SSLVERSION:
#endif
    بازگشت Curl_setopt_SSLVERSION(data, option, arg);
  حالت CURLOPT_SSL_FALSESTART:
    result = CURLE_NOT_BUILT_IN;
    بشکن;
  حالت CURLOPT_USE_SSL:
    اگر((arg < CURLUSESSL_NONE) || (arg >= CURLUSESSL_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    وگرنه
      s->use_ssl = (unsigned نویسه)arg;
    بشکن;
  حالت CURLOPT_SSL_OPTIONS:
    s->ssl.ssl_options = (unsigned نویسه)(arg & 0xff);
    بشکن;
#ifndef CURL_DISABLE_PROXY
  حالت CURLOPT_PROXY_SSL_OPTIONS:
    s->proxy_ssl.ssl_options = (unsigned نویسه)(arg & 0xff);
    بشکن;
#endif
  حالت CURLOPT_SSL_ENABLE_NPN:
    بشکن;
  حالت CURLOPT_SSLENGINE_DEFAULT:
#ifdef USE_OPENSSL
    اگر(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      بازگشت CURLE_NOT_BUILT_IN;
    data->set.engine_default = (arg == 1);
#وگرنه
    بازگشت CURLE_NOT_BUILT_IN;
#endif
    بشکن;
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }
  بازگشت result;
#وگرنه  /* USE_SSL */
  (تهی)data;
  (تهی)option;
  (تهی)arg;
  بازگشت CURLE_UNKNOWN_OPTION;
#endif /* !USE_SSL */
}

#ifndef CURL_DISABLE_PROXY
ایستا تهی changeproxy(ساختار Curl_easy *data)
{
  Curl_auth_digest_cleanup(&data->state.proxydigest);
  memset(&data->state.authproxy, 0, اندازه(data->state.authproxy));
}

ایستا CURLcode setopt_long_proxy(ساختار Curl_easy *data, CURLoption option,
                                  long arg)
{
  ساختار UserDefined *s = &data->set;

  انتخاب(option) {
  حالت CURLOPT_PROXYPORT:
    اگر((arg < 0) || (arg > UINT16_MAX))
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    اگر(arg != s->proxyport)
      changeproxy(data);
    s->proxyport = (uint16_t)arg;
    بشکن;
  حالت CURLOPT_PROXYAUTH:
    بازگشت httpauth(data, TRUE, (unsigned long)arg);
  حالت CURLOPT_PROXYTYPE:
    اگر((arg < CURLPROXY_HTTP) || (arg > CURLPROXY_HTTPS3))
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef USE_PROXY_HTTP3
    اگر(arg == CURLPROXY_HTTPS3)
      بازگشت CURLE_NOT_BUILT_IN;
#endif
    s->proxytype = (unsigned نویسه)arg;
    بشکن;
  حالت CURLOPT_SOCKS5_AUTH: {
    unsigned long auth = (unsigned long)arg;
    اگر(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI | CURLAUTH_ONLY))
      بازگشت CURLE_NOT_BUILT_IN;
    اگر((auth & CURLAUTH_ONLY) && !(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI)))
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    s->socks5auth = (unsigned نویسه)(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI));
    s->socks5_auth_only = !!(auth & CURLAUTH_ONLY);
    بشکن;
  }
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }
  بازگشت CURLE_OK;
}
#وگرنه
ایستا CURLcode setopt_long_proxy(ساختار Curl_easy *data, CURLoption option,
                                  long arg)
{
  (تهی)data;
  (تهی)option;
  (تهی)arg;
  بازگشت CURLE_UNKNOWN_OPTION;
}
#endif

ایستا CURLcode setopt_long_http(ساختار Curl_easy *data, CURLoption option,
                                 long arg)
{
#ifndef CURL_DISABLE_HTTP
  CURLcode result = CURLE_OK;
  ساختار UserDefined *s = &data->set;

  انتخاب(option) {
  حالت CURLOPT_FOLLOWLOCATION:
    اگر((unsigned long)arg > 3)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    وگرنه
      s->http_follow_mode = (unsigned نویسه)arg;
    بشکن;
  حالت CURLOPT_MAXREDIRS:
    result = مقدار_range(&arg, -1, -1, 0x7fff);
    اگر(!result)
      s->maxredirs = (short)arg;
    بشکن;
  حالت CURLOPT_POSTREDIR:
    اگر(arg < CURL_REDIR_GET_ALL)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    وگرنه {
      s->post301 = !!(arg & CURL_REDIR_POST_301);
      s->post302 = !!(arg & CURL_REDIR_POST_302);
      s->post303 = !!(arg & CURL_REDIR_POST_303);
    }
    بشکن;
  حالت CURLOPT_HEADEROPT:
    s->sep_headers = !!(arg & CURLHEADER_SEPARATE);
    بشکن;
  حالت CURLOPT_HTTPAUTH:
    بازگشت httpauth(data, FALSE, (unsigned long)arg);
  حالت CURLOPT_HTTP_VERSION:
    بازگشت setopt_HTTP_VERSION(data, arg);
  حالت CURLOPT_EXPECT_100_TIMEOUT_MS:
    result = مقدار_range(&arg, 0, 0, 0xffff);
    اگر(!result)
      s->expect_100_timeout = (unsigned short)arg;
    بشکن;
  حالت CURLOPT_STREAM_WEIGHT:
#اگر defined(USE_HTTP2) || defined(USE_HTTP3)
    اگر((arg >= 1) && (arg <= 256))
      s->weight = (صحیح)arg;
    بشکن;
#وگرنه
    result = CURLE_NOT_BUILT_IN;
    بشکن;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  حالت CURLOPT_HTTPSIG_ALGORITHM:
    اگر(arg != CURLHTTPSIG_NONE &&
       arg != CURLHTTPSIG_ED25519 &&
       arg != CURLHTTPSIG_HMAC_SHA256)
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    s->httpsig_algorithm = (uint8_t)arg;
    اگر(arg)
      s->httpauth = (uint32_t)CURLAUTH_HTTPSIG;
    وگرنه
      s->httpauth &= ~(uint32_t)CURLAUTH_HTTPSIG;
    بشکن;
#endif
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }
  بازگشت result;
#وگرنه
  (تهی)data;
  (تهی)option;
  (تهی)arg;
  بازگشت CURLE_UNKNOWN_OPTION;
#endif
}

ایستا CURLcode setopt_long_proto(ساختار Curl_easy *data, CURLoption option,
                                  long arg)
{
  CURLcode result = CURLE_OK;
  ساختار UserDefined *s = &data->set;

  انتخاب(option) {
#ifndef CURL_DISABLE_TFTP
  حالت CURLOPT_TFTP_BLKSIZE:
    result = مقدار_range(&arg, 0, TFTP_BLKSIZE_MIN, TFTP_BLKSIZE_MAX);
    اگر(!result)
      s->tftp_blksize = (unsigned short)arg;
    بشکن;
#endif
#ifndef CURL_DISABLE_NETRC
  حالت CURLOPT_NETRC:
    اگر((arg < CURL_NETRC_IGNORED) || (arg >= CURL_NETRC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    وگرنه
      s->use_netrc = (unsigned نویسه)arg;
    بشکن;
#endif
#ifndef CURL_DISABLE_FTP
  حالت CURLOPT_FTP_FILEMETHOD:
    اگر((arg < CURLFTPMETHOD_DEFAULT) || (arg >= CURLFTPMETHOD_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    وگرنه
      s->ftp_filemethod = (unsigned نویسه)arg;
    بشکن;
  حالت CURLOPT_FTP_SSL_CCC:
    اگر((arg < CURLFTPSSL_CCC_NONE) || (arg >= CURLFTPSSL_CCC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    وگرنه
      s->ftp_ccc = (unsigned نویسه)arg;
    بشکن;
  حالت CURLOPT_FTPSSLAUTH:
    اگر((arg < CURLFTPAUTH_DEFAULT) || (arg >= CURLFTPAUTH_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    وگرنه
      s->ftpsslauth = (unsigned نویسه)arg;
    بشکن;
  حالت CURLOPT_ACCEPTTIMEOUT_MS:
    بازگشت setopt_set_timeout_ms(&s->accepttimeout, arg);
#endif
#اگر !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  حالت CURLOPT_FTP_CREATE_MISSING_DIRS:
    اگر((arg < CURLFTP_CREATE_DIR_NONE) || (arg > CURLFTP_CREATE_DIR_RETRY))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    وگرنه
      s->ftp_create_missing_dirs = (unsigned نویسه)arg;
    بشکن;
  حالت CURLOPT_NEW_FILE_PERMS:
    اگر((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    وگرنه
      s->new_file_perms = (unsigned صحیح)arg;
    بشکن;
#endif
#ifndef CURL_DISABLE_RTSP
  حالت CURLOPT_RTSP_REQUEST:
    بازگشت setopt_RTSP_REQUEST(data, arg);
  حالت CURLOPT_RTSP_CLIENT_CSEQ:
    result = مقدار_range(&arg, 0, 0, INT_MAX);
    اگر(!result)
      data->state.rtsp_next_client_CSeq = (uint32_t)arg;
    بشکن;
  حالت CURLOPT_RTSP_SERVER_CSEQ:
    result = مقدار_range(&arg, 0, 0, INT_MAX);
    اگر(!result)
      data->state.rtsp_next_server_CSeq = (uint32_t)arg;
    بشکن;
#endif
#ifdef USE_SSH
  حالت CURLOPT_SSH_AUTH_TYPES:
    s->ssh_auth_types = (uint32_t)arg;
    بشکن;
  حالت CURLOPT_NEW_DIRECTORY_PERMS:
    اگر((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    وگرنه
      s->new_directory_perms = (unsigned صحیح)arg;
    بشکن;
#endif
  حالت CURLOPT_PROTOCOLS:
    s->allowed_protocols = (curl_prot_t)arg;
    بشکن;
  حالت CURLOPT_REDIR_PROTOCOLS:
    s->redir_protocols = (curl_prot_t)arg;
    بشکن;
#ifndef CURL_DISABLE_WEBSOCKETS
  حالت CURLOPT_WS_OPTIONS:
    s->ws_raw_mode = !!(arg & CURLWS_RAW_MODE);
    s->ws_no_auto_pong = !!(arg & CURLWS_NOAUTOPONG);
    بشکن;
#endif
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }
  بازگشت result;
}

ایستا CURLcode setopt_long_misc(ساختار Curl_easy *data, CURLoption option,
                                 long arg)
{
  ساختار UserDefined *s = &data->set;

  انتخاب(option) {
  حالت CURLOPT_TIMECONDITION:
    اگر((arg < CURL_TIMECOND_NONE) || (arg >= CURL_TIMECOND_LAST))
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    s->timecondition = (unsigned نویسه)arg;
    بشکن;
  حالت CURLOPT_TIMEVALUE:
    s->timevalue = (time_t)arg;
    بشکن;
  حالت CURLOPT_POSTFIELDSIZE:
    اگر(arg < -1)
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    اگر(s->postfieldsize < arg && s->str_copypostfields) {
      curlx_safefree(s->str_copypostfields);
      s->postfields = پوچ;
    }
    s->postfieldsize = arg;
    بشکن;
  حالت CURLOPT_INFILESIZE:
    اگر(arg < -1)
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = arg;
    بشکن;
  حالت CURLOPT_RESUME_FROM:
    اگر(arg < -1)
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = arg;
    بشکن;
  حالت CURLOPT_UPLOAD_FLAGS:
    s->upload_flags = (unsigned نویسه)arg;
    بشکن;
#ifndef CURL_DISABLE_MIME
  حالت CURLOPT_MIME_OPTIONS:
    s->mime_formescape = !!(arg & CURLMIMEOPT_FORMESCAPE);
    بشکن;
#endif
#ifndef CURL_DISABLE_HSTS
  حالت CURLOPT_HSTS_CTRL:
    اگر(arg & CURLHSTS_ENABLE) {
      اگر(!data->hsts) {
        data->hsts = Curl_hsts_init();
        اگر(!data->hsts)
          بازگشت CURLE_OUT_OF_MEMORY;
      }
    }
    وگرنه اگر(!data->share || !data->share->hsts) {
      /* throw away the HSTS cache unless shared */
      Curl_hsts_cleanup(&data->hsts);
      /* flush all the entries */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = پوچ;
    }
    وگرنه
      /* detach from shared HSTS cache without freeing it */
      data->hsts = پوچ;
    بشکن;
#endif
#ifndef CURL_DISABLE_ALTSVC
  حالت CURLOPT_ALTSVC_CTRL:
    بازگشت Curl_altsvc_ctrl(data, arg);
#endif
#اگر defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  حالت CURLOPT_GSSAPI_DELEGATION:
    s->gssapi_delegation = (unsigned نویسه)arg &
      (CURLGSSAPI_DELEGATION_POLICY_FLAG | CURLGSSAPI_DELEGATION_FLAG);
    بشکن;
#endif
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode setopt_long(ساختار Curl_easy *data, CURLoption option,
                            long arg)
{
  تعریفنوع CURLcode (*setoptfunc)(ساختار Curl_easy *data,
                                 CURLoption option, long arg);
  ایستا ثابت setoptfunc setopt_call[] = {
    setopt_long_bool,
    setopt_long_net,
    setopt_long_http,
    setopt_long_proxy,
    setopt_long_ssl,
    setopt_long_proto,
    setopt_long_misc
  };
  size_t i;

  برای(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, arg);
    اگر(result != CURLE_UNKNOWN_OPTION)
      بازگشت result;
  }
  بازگشت CURLE_UNKNOWN_OPTION;
}

ایستا CURLcode setopt_slist(ساختار Curl_easy *data, CURLoption option,
                             ساختار curl_slist *slist)
{
  CURLcode result = CURLE_OK;
  ساختار UserDefined *s = &data->set;
  انتخاب(option) {
#ifndef CURL_DISABLE_PROXY
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
    بشکن;
#endif
#ifndef CURL_DISABLE_HTTP
  حالت CURLOPT_HTTP200ALIASES:
    /*
     * Set a list of aliases for HTTP 200 in response header
     */
    s->http200aliases = slist;
    بشکن;
#endif
#اگر !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  حالت CURLOPT_POSTQUOTE:
    /*
     * List of RAW FTP commands to use after a transfer
     */
    s->postquote = slist;
    بشکن;
  حالت CURLOPT_PREQUOTE:
    /*
     * List of RAW FTP commands to use prior to RETR (Wesley Laxton)
     */
    s->prequote = slist;
    بشکن;
  حالت CURLOPT_QUOTE:
    /*
     * List of RAW FTP commands to use before a transfer
     */
    s->quote = slist;
    بشکن;
#endif
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
    data->state.resolve = s->resolve;
    بشکن;
#اگر !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MIME)
  حالت CURLOPT_HTTPHEADER:
    /*
     * Set a list with HTTP headers to use (or replace internals with)
     */
    s->headers = slist;
    بشکن;
#endif
#ifndef CURL_DISABLE_TELNET
  حالت CURLOPT_TELNETOPTIONS:
    /*
     * Set a linked list of telnet options
     */
    s->telnet_options = slist;
    بشکن;
#endif
#ifndef CURL_DISABLE_SMTP
  حالت CURLOPT_MAIL_RCPT:
    /* Set the list of mail recipients */
    s->mail_rcpt = slist;
    بشکن;
#endif
  حالت CURLOPT_CONNECT_TO:
    s->connect_to = slist;
    بشکن;
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }
  بازگشت result;
}

#اگر !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
ایستا CURLcode setopt_mimepost(ساختار Curl_easy *data, curl_mime *mimep)
{
  /*
   * Set to make us do MIME POST
   */
  CURLcode result;
  ساختار UserDefined *s = &data->set;
  اگر(!s->mimepostp) {
    s->mimepostp = curlx_malloc(اندازه(*s->mimepostp));
    اگر(!s->mimepostp)
      بازگشت CURLE_OUT_OF_MEMORY;
    Curl_mime_initpart(s->mimepostp);
  }

  result = Curl_mime_set_subparts(s->mimepostp, mimep, FALSE);
  اگر(!result) {
    s->method = HTTPREQ_POST_MIME;
    s->opt_no_body = FALSE; /* this is implied */
#ifndef CURL_DISABLE_FORM_API
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = پوچ;
#endif
  }
  بازگشت result;
}
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */

ایستا CURLcode setopt_share(ساختار Curl_easy *data, ساختار Curl_share *set)
{
  CURLcode result;

  اگر(data->conn) {
    /* As this handle already has a connection attached, changing share now
       would be complicated and error-prone */
    infof(data, "Cannot change share object while in use");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
  }
  وگرنه {
    /* disconnect from old share, if any and possible */
    result = Curl_share_easy_unlink(data);
    اگر(!result && GOOD_SHARE_HANDLE(set))
      /* use new share if it set */
      result = Curl_share_easy_link(data, set);
  }
  بازگشت result;
}

/* assorted pointer type arguments */
ایستا CURLcode setopt_pointers(ساختار Curl_easy *data, CURLoption option,
                                va_list param)
{
  CURLcode result = CURLE_OK;
  ساختار UserDefined *s = &data->set;
  انتخاب(option) {
  حالت CURLOPT_CURLU:
    /*
     * pass CURLU to set URL
     */
    Curl_bufref_free(&data->state.url);
    CURL_EASY_STR_CLEAR(data, STRING_SET_URL);
    s->uh = va_arg(param, CURLU *);
    بشکن;
#ifndef CURL_DISABLE_HTTP
#ifndef CURL_DISABLE_FORM_API
  حالت CURLOPT_HTTPPOST:
    /*
     * Set to make us do HTTP POST. Legacy API-style.
     */
    s->httppost = va_arg(param, ساختار curl_httppost *);
    s->method = HTTPREQ_POST_FORM;
    s->opt_no_body = FALSE; /* this is implied */
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = پوچ;
    بشکن;
#endif /* !CURL_DISABLE_FORM_API */
#endif /* !CURL_DISABLE_HTTP */
#اگر !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
  حالت CURLOPT_MIMEPOST:
    result = setopt_mimepost(data, va_arg(param, curl_mime *));
    بشکن;
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */
  حالت CURLOPT_STDERR:
    /*
     * Set to a FILE * that should receive all error writes. This
     * defaults to stderr for normal operations.
     */
    s->err = va_arg(param, FILE *);
    اگر(!s->err)
      s->err = stderr;
    بشکن;
  حالت CURLOPT_SHARE:
    بازگشت setopt_share(data, va_arg(param, ساختار Curl_share *));

  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }
  بازگشت result;
}

#ifndef CURL_DISABLE_COOKIES
ایستا CURLcode cookielist(ساختار Curl_easy *data, ثابت نویسه *ptr)
{
  CURLcode result = CURLE_OK;
  اگر(!ptr)
    بازگشت CURLE_OK;

  اگر(curl_strequal(ptr, "ALL")) {
    /* clear all cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearall(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  وگرنه اگر(curl_strequal(ptr, "SESS")) {
    /* clear session cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearsess(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  وگرنه اگر(curl_strequal(ptr, "FLUSH")) {
    /* flush cookies to file, takes care of the locking */
    Curl_flush_cookies(data, FALSE);
  }
  وگرنه اگر(curl_strequal(ptr, "RELOAD")) {
    /* reload cookies from file */
    بازگشت Curl_cookie_loadfiles(data, COOKIE_NOPSL |
                                 (data->set.cookiesession ?
                                  COOKIE_NOSESSION : 0));
  }
  وگرنه {
    اگر(!data->cookies) {
      /* if cookie engine was not running, activate it */
      data->cookies = Curl_cookie_init();
      اگر(!data->cookies)
        بازگشت CURLE_OUT_OF_MEMORY;
      data->state.cookie_engine = TRUE;
    }

    /* general protection against mistakes and abuse */
    اگر(strlen(ptr) > CURL_MAX_درون_داد_LENGTH)
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

    /* Adding these cookies without the PSL check, because the PSL is not
       initialized until *perform() time, and this might be called before
       that */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    اگر(checkprefix("Set-Cookie:", ptr))
      /* HTTP Header format line */
      result = Curl_cookie_add(data, data->cookies, ptr + 11,
                               پوچ, پوچ,
                               COOKIE_HTTPHEADER | COOKIE_SECURE |
                               COOKIE_NOPSL);
    وگرنه
      /* Netscape format line */
      result = Curl_cookie_add(data, data->cookies, ptr, پوچ,
                               پوچ, COOKIE_SECURE | COOKIE_NOPSL);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  بازگشت result;
}

ایستا CURLcode cookiefile(ساختار Curl_easy *data, ثابت نویسه *ptr)
{
  /*
   * Set cookie file to read and parse. Can be used multiple times.
   */
  اگر(ptr) {
    ساختار curl_slist *cl;
    /* general protection against mistakes and abuse */
    اگر(strlen(ptr) > CURL_MAX_درون_داد_LENGTH)
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    /* append the cookie filename to the list of filenames, and deal with
       them later */
    cl = curl_slist_append(data->state.cookielist, ptr);
    اگر(!cl) {
      curl_slist_free_all(data->state.cookielist);
      data->state.cookielist = پوچ;
      بازگشت CURLE_OUT_OF_MEMORY;
    }
    data->state.cookielist = cl; /* store the list for later use */
  }
  وگرنه {
    /* clear the list of cookie files */
    curl_slist_free_all(data->state.cookielist);
    data->state.cookielist = پوچ;

    اگر(!data->share || !data->share->cookies) {
      /* throw away all existing cookies if this is not a shared cookie
         container */
      Curl_cookie_clearall(data->cookies);
      Curl_cookie_cleanup(data->cookies);
    }
    /* disable the cookie engine */
    data->cookies = پوچ;
  }
  بازگشت CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_PROXY
ایستا CURLcode setproxy(ساختار Curl_easy *data, ثابت نویسه *proxy)
{
  ثابت نویسه *str = CURL_EASY_STR(data, STRING_PROXY);
  اگر(str && proxy &&
     /* there was one set, is this a new one? */
     !strcmp(str, proxy))
    بازگشت CURLE_OK; /* same one as before */

  changeproxy(data);
  بازگشت Curl_setstropt(data, STRING_PROXY, proxy);
}

ایستا CURLcode setopt_cptr_proxy(ساختار Curl_easy *data, CURLoption option,
                                  نویسه *ptr)
{
  CURLcode result = CURLE_OK;
  ساختار UserDefined *s = &data->set;
  انتخاب(option) {
  حالت CURLOPT_PROXYUSERPWD: {
    /*
     * user:password needed to use the proxy
     */
    نویسه *u = پوچ;
    نویسه *p = پوچ;
    result = setstropt_userpwd(ptr, &u, &p);

    /* URL decode the components */
    اگر(!result) {
      نویسه *str = پوچ;
      CURL_EASY_STR_CLEAR(data, STRING_PROXYUSERNAME);
      CURL_EASY_STR_CLEAR(data, STRING_PROXYPASSWORD);
      اگر(u) {
        result = Curl_urldecode(u, 0, &str, پوچ, REJECT_ZERO);
        اگر(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYUSERNAME, str);
      }
      اگر(!result && p) {
        str = پوچ;
        result = Curl_urldecode(p, 0, &str, پوچ, REJECT_ZERO);
        اگر(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYPASSWORD, str);
      }
    }
    curlx_free(u);
    curlx_strzero(p);
    curlx_free(p);
    بشکن;
  }
  حالت CURLOPT_PROXYUSERNAME:
    /*
     * authentication username to use in the operation
     */
    بازگشت Curl_setstropt(data, STRING_PROXYUSERNAME, ptr);

  حالت CURLOPT_PROXYPASSWORD:
    /*
     * authentication password to use in the operation
     */
    بازگشت Curl_setstropt(data, STRING_PROXYPASSWORD, ptr);

  حالت CURLOPT_NOPROXY:
    /*
     * proxy exception list
     */
    بازگشت Curl_setstropt(data, STRING_NOPROXY, ptr);
  حالت CURLOPT_PROXY_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use for proxy
     */
    بازگشت Curl_setstropt(data, STRING_CERT_PROXY, ptr);
  حالت CURLOPT_PROXY_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use for proxy
     */
    بازگشت Curl_setstropt(data, STRING_CERT_TYPE_PROXY, ptr);
  حالت CURLOPT_PROXY_SSLKEY:
    /*
     * String that holds filename of the SSL key to use for proxy
     */
    بازگشت Curl_setstropt(data, STRING_کلید_PROXY, ptr);
  حالت CURLOPT_PROXY_KEYPASSWD:
    /*
     * String that holds the SSL private key password for proxy.
     */
    بازگشت Curl_setstropt(data, STRING_کلید_PASSWD_PROXY, ptr);
  حالت CURLOPT_PROXY_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use for proxy
     */
    بازگشت Curl_setstropt(data, STRING_کلید_TYPE_PROXY, ptr);
  حالت CURLOPT_PROXY_SSL_CIPHER_LIST:
    اگر(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST)) {
      /* set a list of cipher we want to use in the SSL connection for proxy */
      بازگشت Curl_setstropt(data, STRING_SSL_CIPHER_LIST_PROXY, ptr);
    }
    وگرنه
      بازگشت CURLE_NOT_BUILT_IN;
  حالت CURLOPT_PROXY_TLS13_CIPHERS:
    اگر(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites for proxy */
      بازگشت Curl_setstropt(data, STRING_SSL_CIPHER13_LIST_PROXY, ptr);
    وگرنه
      بازگشت CURLE_NOT_BUILT_IN;
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
    بازگشت setproxy(data, ptr);
  حالت CURLOPT_PRE_PROXY:
    /*
     * Set proxy server:port to use as SOCKS proxy.
     *
     * If the proxy is set to "" or NULL we explicitly say that we do not want
     * to use the socks proxy.
     */
    بازگشت Curl_setstropt(data, STRING_PRE_PROXY, ptr);
  حالت CURLOPT_SOCKS5_GSSAPI_SERVICE:
  حالت CURLOPT_PROXY_SERVICE_NAME:
    /*
     * Set proxy authentication service name for Kerberos 5 and SPNEGO
     */
    بازگشت Curl_setstropt(data, STRING_PROXY_SERVICE_NAME, ptr);
  حالت CURLOPT_PROXY_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
#ifdef USE_SSL
    اگر(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      بازگشت Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY_PROXY, ptr);
#endif
    بازگشت CURLE_NOT_BUILT_IN;

  حالت CURLOPT_HAPROXY_CLIENT_IP:
    /*
     * Set the client IP to send through HAProxy PROXY protocol
     */
    result = Curl_setstropt(data, STRING_HAPROXY_CLIENT_IP, ptr);

    /* enable the HAProxy protocol if an IP is provided */
    s->haproxyprotocol = !!CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
    بشکن;
  حالت CURLOPT_PROXY_CAINFO:
    /*
     * Set CA info SSL connection for proxy. Specify filename of the
     * CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE_PROXY, ptr);
    اگر(!result)
      s->proxy_ssl.custom_cafile = TRUE;
    بازگشت result;
  حالت CURLOPT_PROXY_CRLFILE:
    /*
     * Set CRL file info for SSL connection for proxy. Specify filename of the
     * CRL to check certificates revocation
     */
    اگر(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      بازگشت Curl_setstropt(data, STRING_SSL_CRLFILE_PROXY, ptr);
    بازگشت CURLE_NOT_BUILT_IN;
  حالت CURLOPT_PROXY_ISSUERCERT:
    /*
     * Set Issuer certificate file to check certificates issuer
     */
    اگر(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      بازگشت Curl_setstropt(data, STRING_SSL_ISSUERCERT_PROXY, ptr);
    بازگشت CURLE_NOT_BUILT_IN;
  حالت CURLOPT_PROXY_CAPATH:
    /*
     * Set CA path info for SSL connection proxy. Specify directory name of the
     * CA certificates which have been prepared using openssl c_rehash utility.
     */
#ifdef USE_SSL
    اگر(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH_PROXY, ptr);
      اگر(!result)
        s->proxy_ssl.custom_capath = TRUE;
      بازگشت result;
    }
#endif
    بازگشت CURLE_NOT_BUILT_IN;
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }
  بازگشت result;
}
#endif

#اگر !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
/*
 * A string with POST data. Makes curl HTTP POST. Even if it is NULL. If
 * needed, CURLOPT_POSTFIELDSIZE must have been set prior to
 * CURLOPT_COPYPOSTFIELDS and not altered later.
 */
ایستا CURLcode setopt_copypostfields(ثابت نویسه *ptr, ساختار UserDefined *s)
{
  CURLcode result = CURLE_OK;
  اگر(s->postfieldsize < -1)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  اگر(!ptr || s->postfieldsize == -1) {
    اگر(ptr && (strlen(ptr) > CURL_MAX_درون_داد_LENGTH))
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_safefree(s->str_copypostfields);
    اگر(ptr) {
      s->str_copypostfields = curlx_strdup(ptr);
      اگر(!s->str_copypostfields)
        بازگشت CURLE_OUT_OF_MEMORY;
    }
  }
  وگرنه {
    size_t pflen = curlx_sotouz_range(s->postfieldsize, 0, SIZE_MAX);
    اگر(pflen == SIZE_MAX)
      بازگشت CURLE_OUT_OF_MEMORY;
    وگرنه {
      /* Allocate even when size == 0. This satisfies the need of possible
         later address compare to detect the COPYPOSTFIELDS mode, and to mark
         that postfields is used rather than read function or form data. */
      نویسه *p = curlx_memdup0(ptr, pflen);
      اگر(!p)
        بازگشت CURLE_OUT_OF_MEMORY;
      وگرنه {
        curlx_free(s->str_copypostfields);
        s->str_copypostfields = p;
      }
    }
  }

  s->postfields = s->str_copypostfields;
  s->method = HTTPREQ_POST;
  بازگشت result;
}
#endif

#ifdef USE_ECH
ایستا CURLcode setopt_ech(ساختار Curl_easy *data, ثابت نویسه *ptr)
{
  ساختار UserDefined *s = &data->set;
  CURLcode result = CURLE_OK;

  اگر(!ptr || !strcmp(ptr, "false"))
    s->ssl.ech = CURLECH_DISABLE;
  وگرنه {
    size_t plen = strlen(ptr);
    اگر(plen > CURL_MAX_درون_داد_LENGTH)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    وگرنه {
      اگر(!strcmp(ptr, "grease"))
        s->ssl.ech = CURLECH_GREASE;
      وگرنه اگر(!strcmp(ptr, "true"))
        s->ssl.ech = CURLECH_ENABLE;
      وگرنه اگر(!strcmp(ptr, "hard"))
        s->ssl.ech = CURLECH_HARD;
      وگرنه اگر(plen > 4 && !strncmp(ptr, "ecl:", 4)) {
        اگر(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_CONFIG, ptr + 4);
      }
      وگرنه اگر(plen > 3 && !strncmp(ptr, "pn:", 3)) {
        اگر(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_PUBLIC, ptr + 3);
      }
      وگرنه
        result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  بازگشت result;
}
#وگرنه
#define setopt_ech(x, y) CURLE_NOT_BUILT_IN
#endif

#اگر defined(USE_SSL) || defined(USE_SSH)
/* One of the options is used for both TLS and SSH */
ایستا CURLcode setopt_cptr_ssl(ساختار Curl_easy *data, CURLoption option,
                                نویسه *ptr)
{
  CURLcode result = CURLE_OK;

  انتخاب(option) {
  حالت CURLOPT_KEYPASSWD:
    /*
     * String that holds the SSL or SSH private key password.
     */
    result = Curl_setstropt(data, STRING_کلید_PASSWD, ptr);
    بشکن;
#ifdef USE_SSL
  حالت CURLOPT_CAINFO:
    /*
     * Set CA info for SSL connection. Specify filename of the CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE, ptr);
    اگر(!result)
      data->set.ssl.custom_cafile = TRUE;
    بازگشت result;
  حالت CURLOPT_CAPATH:
    /*
     * Set CA path info for SSL connection. Specify directory name of the CA
     * certificates which have been prepared using openssl c_rehash utility.
     */
    اگر(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH, ptr);
      اگر(!result)
        data->set.ssl.custom_capath = TRUE;
      بازگشت result;
    }
    بازگشت CURLE_NOT_BUILT_IN;
  حالت CURLOPT_CRLFILE:
    /*
     * Set CRL file info for SSL connection. Specify filename of the CRL
     * to check certificates revocation
     */
    اگر(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      بازگشت Curl_setstropt(data, STRING_SSL_CRLFILE, ptr);
    بازگشت CURLE_NOT_BUILT_IN;
  حالت CURLOPT_SSL_CIPHER_LIST:
    اگر(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST))
      /* set a list of cipher we want to use in the SSL connection */
      بازگشت Curl_setstropt(data, STRING_SSL_CIPHER_LIST, ptr);
    وگرنه
      بازگشت CURLE_NOT_BUILT_IN;
  حالت CURLOPT_TLS13_CIPHERS:
    اگر(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites */
      بازگشت Curl_setstropt(data, STRING_SSL_CIPHER13_LIST, ptr);
    وگرنه
      بازگشت CURLE_NOT_BUILT_IN;
  حالت CURLOPT_RANDOM_FILE:
    بشکن;
  حالت CURLOPT_EGDSOCKET:
    بشکن;
  حالت CURLOPT_SSL_CTX_DATA:
    /*
     * Set an SSL_CTX callback parameter pointer
     */
    اگر(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      data->set.ssl_fsslctxp = ptr;
      بشکن;
    }
    وگرنه
      بازگشت CURLE_NOT_BUILT_IN;
  حالت CURLOPT_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use
     */
    بازگشت Curl_setstropt(data, STRING_CERT, ptr);
  حالت CURLOPT_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use
     */
    بازگشت Curl_setstropt(data, STRING_CERT_TYPE, ptr);
  حالت CURLOPT_SSLKEY:
    /*
     * String that holds filename of the SSL key to use
     */
    بازگشت Curl_setstropt(data, STRING_کلید, ptr);
  حالت CURLOPT_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use
     */
    بازگشت Curl_setstropt(data, STRING_کلید_TYPE, ptr);
  حالت CURLOPT_SSLENGINE:
    /*
     * String that holds the SSL crypto engine.
     */
    اگر(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      بازگشت CURLE_NOT_BUILT_IN;
    result = Curl_setstropt(data, STRING_SSL_ENGINE, ptr);
    بشکن;
  حالت CURLOPT_ISSUERCERT:
    /*
     * Set Issuer certificate file
     * to check certificates issuer
     */
    اگر(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      بازگشت Curl_setstropt(data, STRING_SSL_ISSUERCERT, ptr);
    بازگشت CURLE_NOT_BUILT_IN;
  حالت CURLOPT_SSL_EC_CURVES:
    /*
     * Set accepted curves in SSL connection setup.
     * Specify colon-delimited list of curve algorithm names.
     */
    اگر(Curl_ssl_supports(data, SSLSUPP_SSL_EC_CURVES))
      بازگشت Curl_setstropt(data, STRING_SSL_EC_CURVES, ptr);
    بازگشت CURLE_NOT_BUILT_IN;
  حالت CURLOPT_SSL_SIGNATURE_ALGORITHMS:
    /*
     * Set accepted signature algorithms.
     * Specify colon-delimited list of signature scheme names.
     */
    اگر(Curl_ssl_supports(data, SSLSUPP_SIGNATURE_ALGORITHMS))
      بازگشت Curl_setstropt(data, STRING_SSL_SIGNATURE_ALGORITHMS, ptr);
    بازگشت CURLE_NOT_BUILT_IN;
  حالت CURLOPT_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
    اگر(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      بازگشت Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY, ptr);
    بازگشت CURLE_NOT_BUILT_IN;
  حالت CURLOPT_ECH:
    بازگشت setopt_ech(data, ptr);
#endif
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }
  بازگشت result;
}
#endif

#اگر !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
ایستا CURLcode setopt_cptr_http_mqtt(ساختار Curl_easy *data,
                                      CURLoption option, نویسه *ptr)
{
  CURLcode result = CURLE_OK;
  ساختار UserDefined *s = &data->set;

  انتخاب(option) {
  حالت CURLOPT_COPYPOSTFIELDS:
    بازگشت setopt_copypostfields(ptr, s);

  حالت CURLOPT_POSTFIELDS:
    /*
     * Like above, but use static data instead of copying it.
     */
    s->postfields = ptr;
    /* Release old copied data. */
    curlx_safefree(s->str_copypostfields);
    s->method = HTTPREQ_POST;
    بشکن;

#ifndef CURL_DISABLE_HTTP
  حالت CURLOPT_TRAILERDATA:
    s->trailer_data = ptr;
    بشکن;
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
        result = CURL_EASY_STR_SETN(data, STRING_ENCODING, ptr);
      وگرنه
        result = CURLE_OUT_OF_MEMORY;
      بازگشت result;
    }
    بازگشت Curl_setstropt(data, STRING_ENCODING, ptr);

#ifndef CURL_DISABLE_AWS
  حالت CURLOPT_AWS_SIGV4:
    /*
     * String that is merged to some authentication
     * parameters are used by the algorithm.
     */
    result = Curl_setstropt(data, STRING_AWS_SIGV4, ptr);
    /*
     * Basic has been set by default; it needs to be unset here.
     */
    اگر(CURL_EASY_STR(data, STRING_AWS_SIGV4))
      s->httpauth = CURLAUTH_AWS_SIGV4;
    وگرنه
      s->httpauth &= ~(uint32_t)CURLAUTH_AWS_SIGV4;
    بشکن;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  حالت CURLOPT_HTTPSIG_کلید:
    result = Curl_setstropt(data, STRING_HTTPSIG_کلید, ptr);
    بشکن;
  حالت CURLOPT_HTTPSIG_KEYID:
    result = Curl_setstropt(data, STRING_HTTPSIG_KEYID, ptr);
    بشکن;
  حالت CURLOPT_HTTPSIG_HEADERS:
    result = Curl_setstropt(data, STRING_HTTPSIG_HEADERS, ptr);
    بشکن;
#endif
  حالت CURLOPT_REFERER: {
    /*
     * String to set in the HTTP Referer: field.
     */
    ساختار bufref *oldref = &data->state.referer;
    /* free the old after the storing the new in case the input is actually
       pointing back to this */
    result = Curl_setstropt(data, STRING_SET_REFERER, ptr);
    Curl_bufref_free(oldref);
    بشکن;
  }

  حالت CURLOPT_USERAGENT:
    /*
     * String to use in the HTTP User-Agent field
     */
    بازگشت Curl_setstropt(data, STRING_USERAGENT, ptr);

#ifndef CURL_DISABLE_COOKIES
  حالت CURLOPT_COOKIE:
    /*
     * Cookie string to send to the remote server in the request.
     */
    بازگشت Curl_setstropt(data, STRING_COOKIE, ptr);

  حالت CURLOPT_COOKIEFILE:
    بازگشت cookiefile(data, ptr);

  حالت CURLOPT_COOKIEJAR:
    /*
     * Set cookie filename to dump all cookies to when we are done.
     */
    result = Curl_setstropt(data, STRING_COOKIEJAR, ptr);
    اگر(!result) {
      /*
       * Activate the cookie parser. This may or may not already
       * have been made.
       */
      اگر(!data->cookies)
        data->cookies = Curl_cookie_init();
      اگر(!data->cookies)
        result = CURLE_OUT_OF_MEMORY;
      وگرنه
        data->state.cookie_engine = TRUE;
    }
    بشکن;

  حالت CURLOPT_COOKIELIST:
    بازگشت cookielist(data, ptr);
#endif /* !CURL_DISABLE_COOKIES */

#endif /* !CURL_DISABLE_HTTP */
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }
  بازگشت result;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_MQTT */

#ifdef USE_SSH
ایستا CURLcode setopt_cptr_ssh(ساختار Curl_easy *data, CURLoption option,
                                نویسه *ptr)
{
  ساختار UserDefined *s = &data->set;
  انتخاب(option) {
  حالت CURLOPT_SSH_PUBLIC_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa.pub file
     */
    بازگشت Curl_setstropt(data, STRING_SSH_PUBLIC_کلید, ptr);
  حالت CURLOPT_SSH_PRIVATE_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa file
     */
    بازگشت Curl_setstropt(data, STRING_SSH_PRIVATE_کلید, ptr);
  حالت CURLOPT_SSH_KEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_keyfunc_userp = ptr;
    بشکن;
  حالت CURLOPT_SSH_HOST_PUBLIC_کلید_MD5:
    /*
     * Option to allow for the MD5 of the host public key to be checked
     * for validation purposes.
     */
    بازگشت Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_کلید_MD5, ptr);
  حالت CURLOPT_SSH_HOST_PUBLIC_کلید_SHA256:
    /*
     * Option to allow for the SHA256 of the host public key to be checked
     * for validation purposes.
     */
    بازگشت Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_کلید_SHA256, ptr);
  حالت CURLOPT_SSH_KNOWNHOSTS:
    /*
     * Store the filename to read known hosts from.
     */
    بازگشت Curl_setstropt(data, STRING_SSH_KNOWNHOSTS, ptr);
#ifdef USE_LIBSSH2
  حالت CURLOPT_SSH_HOSTKEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_hostkeyfunc_userp = ptr;
    بشکن;
#endif /* USE_LIBSSH2 */
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }
  بازگشت CURLE_OK;
}
#endif /* USE_SSH */

#ifndef CURL_DISABLE_FTP
ایستا CURLcode setopt_cptr_ftp(ساختار Curl_easy *data, CURLoption option,
                                نویسه *ptr)
{
  CURLcode result = CURLE_OK;
  ساختار UserDefined *s = &data->set;
  انتخاب(option) {
  حالت CURLOPT_FTPPORT:
    /*
     * Use FTP PORT, this also specifies which IP address to use
     */
    result = Curl_setstropt(data, STRING_FTPPORT, ptr);
    s->ftp_use_port = !!CURL_EASY_STR(data, STRING_FTPPORT);
    بشکن;

  حالت CURLOPT_FTP_ACCOUNT:
    بازگشت Curl_setstropt(data, STRING_FTP_ACCOUNT, ptr);

  حالت CURLOPT_FTP_ALTERNATIVE_TO_USER:
    بازگشت Curl_setstropt(data, STRING_FTP_ALTERNATIVE_TO_USER, ptr);

  حالت CURLOPT_KRBLEVEL:
    بازگشت CURLE_NOT_BUILT_IN; /* removed in 8.17.0 */
  حالت CURLOPT_CHUNK_DATA:
    s->wildcardptr = ptr;
    بشکن;
  حالت CURLOPT_FNMATCH_DATA:
    s->fnmatch_data = ptr;
    بشکن;
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }
  بازگشت result;
}
#endif /* !CURL_DISABLE_FTP */

ایستا CURLcode setopt_cptr_net(ساختار Curl_easy *data, CURLoption option,
                                نویسه *ptr)
{
  انتخاب(option) {
  حالت CURLOPT_INTERFACE:
    /*
     * Set what interface or address/hostname to bind the socket to when
     * performing an operation and thus what from-IP your connection will use.
     */
    بازگشت setstropt_interface(data, ptr);

#ifdef USE_RESOLV_ARES
  حالت CURLOPT_DNS_SERVERS:
    بازگشت Curl_setstropt(data, STRING_DNS_SERVERS, ptr);

  حالت CURLOPT_DNS_INTERFACE:
    بازگشت Curl_setstropt(data, STRING_DNS_INTERFACE, ptr);

  حالت CURLOPT_DNS_LOCAL_IP4:
    بازگشت Curl_setstropt(data, STRING_DNS_LOCAL_IP4, ptr);

  حالت CURLOPT_DNS_LOCAL_IP6:
    بازگشت Curl_setstropt(data, STRING_DNS_LOCAL_IP6, ptr);
#endif
#ifdef USE_UNIX_SOCKETS
  حالت CURLOPT_UNIX_SOCKET_PATH:
    data->set.abstract_unix_socket = FALSE;
    بازگشت Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);

  حالت CURLOPT_ABSTRACT_UNIX_SOCKET:
    data->set.abstract_unix_socket = TRUE;
    بازگشت Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);
#endif
#ifndef CURL_DISABLE_DOH
  حالت CURLOPT_DOH_URL:
    {
      CURLcode result = Curl_setstropt(data, STRING_DOH, ptr);
      data->set.doh = !!CURL_EASY_STR(data, STRING_DOH);
      بازگشت result;
    }
#endif
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }
}

ایستا CURLcode setopt_cptr_misc(ساختار Curl_easy *data, CURLoption option,
                                 نویسه *ptr)
{
  CURLcode result = CURLE_OK;
  ساختار UserDefined *s = &data->set;

  انتخاب(option) {
  حالت CURLOPT_REQUEST_TARGET:
    بازگشت Curl_setstropt(data, STRING_TARGET, ptr);
#ifndef CURL_DISABLE_NETRC
  حالت CURLOPT_NETRC_FILE:
    بازگشت Curl_setstropt(data, STRING_NETRC_FILE, ptr);
#endif
  حالت CURLOPT_CUSTOMREQUEST:
    بازگشت Curl_setstropt(data, STRING_CUSTOMREQUEST, ptr);

    /* we do not set s->method = HTTPREQ_CUSTOM; here, we continue as if we
       were using the already set type and this changes the actual request
       keyword */
  حالت CURLOPT_SERVICE_NAME:
    بازگشت Curl_setstropt(data, STRING_SERVICE_NAME, ptr);

  حالت CURLOPT_HEADERDATA:
    s->writeheader = ptr;
    بشکن;
  حالت CURLOPT_READDATA:
    s->in_set = ptr;
    بشکن;
  حالت CURLOPT_WRITEDATA:
    s->out = ptr;
    بشکن;
  حالت CURLOPT_DEBUGDATA:
    s->debugdata = ptr;
    بشکن;
  حالت CURLOPT_PROGRESSDATA:
    s->progress_client = ptr;
    بشکن;
  حالت CURLOPT_SEEKDATA:
    s->seek_client = ptr;
    بشکن;
  حالت CURLOPT_IOCTLDATA:
    s->ioctl_client = ptr;
    بشکن;
  حالت CURLOPT_SOCKOPTDATA:
    s->sockopt_client = ptr;
    بشکن;
  حالت CURLOPT_OPENSOCKETDATA:
    s->opensocket_client = ptr;
    بشکن;
  حالت CURLOPT_RESOLVER_START_DATA:
    s->resolver_start_client = ptr;
    بشکن;
  حالت CURLOPT_CLOSESOCKETDATA:
    s->closesocket_client = ptr;
    بشکن;
  حالت CURLOPT_PREREQDATA:
    s->prereq_userp = ptr;
    بشکن;
  حالت CURLOPT_ERRORBUFFER:
    s->errorbuffer = ptr;
    بشکن;
  حالت CURLOPT_URL:
    result = Curl_setstropt(data, STRING_SET_URL, ptr);
    Curl_bufref_set(&data->state.url,
                    CURL_EASY_STR(data, STRING_SET_URL), 0, پوچ);
    بشکن;

  حالت CURLOPT_USERPWD: {
    نویسه *u = پوچ, *p = پوچ;
    result = setstropt_userpwd(ptr, &u, &p);
    اگر(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_USERNAME, u);
      u = پوچ;
    }
    اگر(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_PASSWORD, p);
      p = پوچ;
    }
    curlx_free(u);
    curlx_free(p);
    بازگشت result;
  }

  حالت CURLOPT_USERNAME:
    بازگشت Curl_setstropt(data, STRING_USERNAME, ptr);

  حالت CURLOPT_PASSWORD:
    بازگشت Curl_setstropt(data, STRING_PASSWORD, ptr);

  حالت CURLOPT_LOGIN_OPTIONS:
    بازگشت Curl_setstropt(data, STRING_OPTIONS, ptr);

  حالت CURLOPT_XOAUTH2_BEARER:
    بازگشت Curl_setstropt(data, STRING_BEARER, ptr);
  حالت CURLOPT_RANGE:
    بازگشت Curl_setstropt(data, STRING_SET_RANGE, ptr);
  حالت CURLOPT_PRIVATE:
    s->private_data = ptr;
    بشکن;
  حالت CURLOPT_PROTOCOLS_STR:
    اگر(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      اگر(!result)
        s->allowed_protocols = protos;
    }
    وگرنه
      /* make a NULL argument reset to default */
      s->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
    بشکن;
  حالت CURLOPT_REDIR_PROTOCOLS_STR:
    اگر(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      اگر(!result)
        s->redir_protocols = protos;
    }
    وگرنه
      /* make a NULL argument reset to default */
      s->redir_protocols = (curl_prot_t)CURLPROTO_REDIR;
    بشکن;
  حالت CURLOPT_DEFAULT_PROTOCOL:
    /* Set the protocol to use when the URL does not include any protocol */
    بازگشت Curl_setstropt(data, STRING_DEFAULT_PROTOCOL, ptr);
#ifndef CURL_DISABLE_SMTP
  حالت CURLOPT_MAIL_FROM:
    /* Set the SMTP mail originator */
    بازگشت Curl_setstropt(data, STRING_MAIL_FROM, ptr);
  حالت CURLOPT_MAIL_AUTH:
    /* Set the SMTP auth originator */
    بازگشت Curl_setstropt(data, STRING_MAIL_AUTH, ptr);
#endif
  حالت CURLOPT_SASL_AUTHZID:
    /* Authorization identity (identity to act as) */
    بازگشت Curl_setstropt(data, STRING_SASL_AUTHZID, ptr);
#ifndef CURL_DISABLE_RTSP
  حالت CURLOPT_RTSP_SESSION_ID:
    Curl_peer_unlink(&data->state.rtsp_session_origin);
    بازگشت Curl_setstropt(data, STRING_RTSP_SESSION_ID, ptr);
  حالت CURLOPT_RTSP_STREAM_URI:
    بازگشت Curl_setstropt(data, STRING_RTSP_STREAM_URI, ptr);
  حالت CURLOPT_RTSP_TRANSPORT:
    بازگشت Curl_setstropt(data, STRING_RTSP_TRANSPORT, ptr);
  حالت CURLOPT_INTERLEAVEDATA:
    s->rtp_out = ptr;
    بشکن;
#endif /* !CURL_DISABLE_RTSP */
  حالت CURLOPT_TLSAUTH_USERNAME:
  حالت CURLOPT_TLSAUTH_PASSWORD:
  حالت CURLOPT_TLSAUTH_TYPE:
  حالت CURLOPT_PROXY_TLSAUTH_USERNAME:
  حالت CURLOPT_PROXY_TLSAUTH_PASSWORD:
  حالت CURLOPT_PROXY_TLSAUTH_TYPE:
    بازگشت CURLE_NOT_BUILT_IN;
#ifndef CURL_DISABLE_HSTS
  حالت CURLOPT_HSTSREADDATA:
    s->hsts_read_userp = ptr;
    بشکن;
  حالت CURLOPT_HSTSWRITEDATA:
    s->hsts_write_userp = ptr;
    بشکن;
  حالت CURLOPT_HSTS: {
    ساختار curl_slist *h;
    اگر(!data->hsts) {
      data->hsts = Curl_hsts_init();
      اگر(!data->hsts)
        بازگشت CURLE_OUT_OF_MEMORY;
    }
    اگر(ptr) {
      result = Curl_setstropt(data, STRING_HSTS, ptr);
      اگر(result)
        بازگشت result;
      /* this needs to build a list of filenames to read from, so that it can
         read them later, as we might get a shared HSTS handle to load them
         into */
      h = curl_slist_append(data->state.hstslist, ptr);
      اگر(!h) {
        curl_slist_free_all(data->state.hstslist);
        data->state.hstslist = پوچ;
        بازگشت CURLE_OUT_OF_MEMORY;
      }
      data->state.hstslist = h; /* store the list for later use */
    }
    وگرنه {
      /* clear the list of HSTS files */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = پوچ;
      اگر(!data->share || !data->share->hsts)
        /* throw away the HSTS cache unless shared */
        Curl_hsts_cleanup(&data->hsts);
    }
    بشکن;
  }
#endif /* !CURL_DISABLE_HSTS */
#ifndef CURL_DISABLE_ALTSVC
  حالت CURLOPT_ALTSVC:
    اگر(!data->asi) {
      data->asi = Curl_altsvc_init();
      اگر(!data->asi)
        بازگشت CURLE_OUT_OF_MEMORY;
    }
    result = Curl_setstropt(data, STRING_ALTSVC, ptr);
    اگر(result)
      بشکن;
    اگر(ptr)
      بازگشت Curl_altsvc_load(data->asi, ptr);
    بشکن;
#endif /* !CURL_DISABLE_ALTSVC */
  حالت CURLOPT_ECH:
    بازگشت setopt_ech(data, ptr);
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }
  بازگشت result;
}

ایستا CURLcode setopt_cptr(ساختار Curl_easy *data, CURLoption option,
                            نویسه *ptr)
{
  تعریفنوع CURLcode (*ptrfunc)(ساختار Curl_easy *data, CURLoption option,
                              نویسه *ptr);
  /* Order by likeliness */
  ایستا ثابت ptrfunc setopt_call[] = {
    setopt_cptr_misc,
#اگر defined(USE_SSL) || defined(USE_SSH)
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
#اگر !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
    setopt_cptr_http_mqtt,
#endif
  };
  size_t i;

  برای(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, ptr);
    اگر(result != CURLE_UNKNOWN_OPTION)
      بازگشت result;
  }
  بازگشت CURLE_UNKNOWN_OPTION;
}

ایستا CURLcode setopt_func(ساختار Curl_easy *data, CURLoption option,
                            va_list param)
{
  ساختار UserDefined *s = &data->set;
  انتخاب(option) {
  حالت CURLOPT_PROGRESSFUNCTION:
    /*
     * Progress callback function
     */
    s->fprogress = va_arg(param, curl_progress_callback);
    اگر(s->fprogress)
      data->progress.callback = TRUE; /* no longer internal */
    وگرنه
      data->progress.callback = FALSE; /* NULL enforces internal */
    بشکن;

  حالت CURLOPT_XFERINFOFUNCTION:
    /*
     * Transfer info callback function
     */
    s->fxferinfo = va_arg(param, curl_xferinfo_callback);
    اگر(s->fxferinfo)
      data->progress.callback = TRUE; /* no longer internal */
    وگرنه
      data->progress.callback = FALSE; /* NULL enforces internal */

    بشکن;
  حالت CURLOPT_DEBUGFUNCTION:
    /*
     * stderr write callback.
     */
    s->fdebug = va_arg(param, curl_debug_callback);
    /*
     * if the callback provided is NULL, it will use the default callback
     */
    بشکن;
  حالت CURLOPT_HEADERFUNCTION:
    /*
     * Set header write callback
     */
    s->fwrite_header = va_arg(param, curl_write_callback);
    بشکن;
  حالت CURLOPT_WRITEFUNCTION:
    /*
     * Set data write callback
     */
    s->fwrite_func = va_arg(param, curl_write_callback);
    اگر(!s->fwrite_func)
#اگر defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic افزودن
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fwrite_func = (curl_write_callback)fwrite;
#اگر defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic برداشت
#endif
    بشکن;
  حالت CURLOPT_READFUNCTION:
    /*
     * Read data callback
     */
    s->fread_func_set = va_arg(param, curl_read_callback);
    اگر(!s->fread_func_set) {
      s->is_fread_set = 0;
#اگر defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic افزودن
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fread_func_set = (curl_read_callback)fread;
#اگر defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic برداشت
#endif
    }
    وگرنه
      s->is_fread_set = 1;
    بشکن;
  حالت CURLOPT_SEEKFUNCTION:
    /*
     * Seek callback. Might be NULL.
     */
    s->seek_func = va_arg(param, curl_seek_callback);
    بشکن;
  حالت CURLOPT_IOCTLFUNCTION:
    /*
     * I/O control callback. Might be NULL.
     */
    s->ioctl_func = va_arg(param, curl_ioctl_callback);
    بشکن;
  حالت CURLOPT_SSL_CTX_FUNCTION:
    /*
     * Set an SSL_CTX callback
     */
#ifdef USE_SSL
    اگر(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      s->ssl_fsslctx = va_arg(param, curl_ssl_ctx_callback);
      بشکن;
    }
    وگرنه
#endif
      بازگشت CURLE_NOT_BUILT_IN;

  حالت CURLOPT_SOCKOPTFUNCTION:
    /*
     * socket callback function: called after socket() but before connect()
     */
    s->fsockopt = va_arg(param, curl_sockopt_callback);
    بشکن;

  حالت CURLOPT_OPENSOCKETFUNCTION:
    /*
     * open/create socket callback function: called instead of socket(),
     * before connect()
     */
    s->fopensocket = va_arg(param, curl_opensocket_callback);
    بشکن;

  حالت CURLOPT_CLOSESOCKETFUNCTION:
    /*
     * close socket callback function: called instead of close()
     * when shutting down a connection
     */
    s->fclosesocket = va_arg(param, curl_closesocket_callback);
    بشکن;

  حالت CURLOPT_RESOLVER_START_FUNCTION:
    /*
     * resolver start callback function: called before a new resolver request
     * is started
     */
    s->resolver_start = va_arg(param, curl_resolver_start_callback);
    بشکن;

#ifdef USE_SSH
#ifdef USE_LIBSSH2
  حالت CURLOPT_SSH_HOSTKEYFUNCTION:
    /* the callback to check the hostkey without the knownhost file */
    s->ssh_hostkeyfunc = va_arg(param, curl_sshhostkeycallback);
    بشکن;
#endif

  حالت CURLOPT_SSH_KEYFUNCTION:
    /* setting to NULL is fine since the ssh.c functions themselves will
       then revert to use the internal default */
    s->ssh_keyfunc = va_arg(param, curl_sshkeycallback);
    بشکن;

#endif /* USE_SSH */

#ifndef CURL_DISABLE_RTSP
  حالت CURLOPT_INTERLEAVEFUNCTION:
    /* Set the user defined RTP write function */
    s->fwrite_rtp = va_arg(param, curl_write_callback);
    بشکن;
#endif
#ifndef CURL_DISABLE_FTP
  حالت CURLOPT_CHUNK_BGN_FUNCTION:
    s->chunk_bgn = va_arg(param, curl_chunk_bgn_callback);
    بشکن;
  حالت CURLOPT_CHUNK_END_FUNCTION:
    s->chunk_end = va_arg(param, curl_chunk_end_callback);
    بشکن;
  حالت CURLOPT_FNMATCH_FUNCTION:
    s->fnmatch = va_arg(param, curl_fnmatch_callback);
    بشکن;
#endif
#ifndef CURL_DISABLE_HTTP
  حالت CURLOPT_TRAILERFUNCTION:
    s->trailer_callback = va_arg(param, curl_trailer_callback);
    بشکن;
#endif
#ifndef CURL_DISABLE_HSTS
  حالت CURLOPT_HSTSREADFUNCTION:
    s->hsts_read = va_arg(param, curl_hstsread_callback);
    بشکن;
  حالت CURLOPT_HSTSWRITEFUNCTION:
    s->hsts_write = va_arg(param, curl_hstswrite_callback);
    بشکن;
#endif
  حالت CURLOPT_PREREQFUNCTION:
    s->fprereq = va_arg(param, curl_prereq_callback);
    بشکن;
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode setopt_offt(ساختار Curl_easy *data, CURLoption option,
                            curl_off_t offt)
{
  ساختار UserDefined *s = &data->set;
  انتخاب(option) {
  حالت CURLOPT_TIMEVALUE_LARGE:
    /*
     * This is the value to compare with the remote document with the
     * method set with CURLOPT_TIMECONDITION
     */
    s->timevalue = (time_t)offt;
    بشکن;

    /* MQTT "borrows" some of the HTTP options */
  حالت CURLOPT_POSTFIELDSIZE_LARGE:
    /*
     * The size of the POSTFIELD data to prevent libcurl to do strlen() to
     * figure it out. Enables binary posts.
     */
    اگر(offt < -1)
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

    اگر(s->postfieldsize < offt && s->str_copypostfields) {
      /* Previous CURLOPT_COPYPOSTFIELDS is no longer valid. */
      curlx_safefree(s->str_copypostfields);
      s->postfields = پوچ;
    }
    s->postfieldsize = offt;
    بشکن;
  حالت CURLOPT_INFILESIZE_LARGE:
    /*
     * If known, this should inform curl about the file size of the
     * to-be-uploaded file.
     */
    اگر(offt < -1)
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = offt;
    بشکن;
  حالت CURLOPT_MAX_SEND_SPEED_LARGE:
    /*
     * When transfer uploads are faster than CURLOPT_MAX_SEND_SPEED_LARGE
     * bytes per second the transfer is throttled..
     */
    اگر(offt < 0)
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_send_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.ul.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    بشکن;
  حالت CURLOPT_MAX_RECV_SPEED_LARGE:
    /*
     * When receiving data faster than CURLOPT_MAX_RECV_SPEED_LARGE bytes per
     * second the transfer is throttled..
     */
    اگر(offt < 0)
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_recv_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.dl.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    بشکن;
  حالت CURLOPT_RESUME_FROM_LARGE:
    /*
     * Resume transfer at the given file position
     */
    اگر(offt < -1)
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = offt;
    بشکن;
  حالت CURLOPT_MAXFILESIZE_LARGE:
    /*
     * Set the maximum size of a file to download.
     */
    اگر(offt < 0)
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_filesize = offt;
    بشکن;

  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode setopt_blob(ساختار Curl_easy *data, CURLoption option,
                            ساختار curl_blob *blob)
{
  ساختار UserDefined *s = &data->set;
  انتخاب(option) {
  حالت CURLOPT_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use
     */
    بازگشت Curl_setblobopt(&s->blobs[BLOB_CERT], blob);
#ifndef CURL_DISABLE_PROXY
  حالت CURLOPT_PROXY_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use for proxy
     */
    بازگشت Curl_setblobopt(&s->blobs[BLOB_CERT_PROXY], blob);
  حالت CURLOPT_PROXY_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use for proxy
     */
    بازگشت Curl_setblobopt(&s->blobs[BLOB_کلید_PROXY], blob);
  حالت CURLOPT_PROXY_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection proxy.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    اگر(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO_PROXY], blob);
      اگر(!result)
        s->proxy_ssl.custom_cablob = TRUE;
      بازگشت result;
    }
#endif
    بازگشت CURLE_NOT_BUILT_IN;
  حالت CURLOPT_PROXY_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    اگر(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      بازگشت Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT_PROXY], blob);
    بازگشت CURLE_NOT_BUILT_IN;
#endif
  حالت CURLOPT_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use
     */
    بازگشت Curl_setblobopt(&s->blobs[BLOB_کلید], blob);
  حالت CURLOPT_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    اگر(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO], blob);
      اگر(!result)
        s->ssl.custom_cablob = TRUE;
      بازگشت result;
    }
#endif
    بازگشت CURLE_NOT_BUILT_IN;
  حالت CURLOPT_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    اگر(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      بازگشت Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT], blob);
    بازگشت CURLE_NOT_BUILT_IN;

  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }
  /* unreachable */
}

/*
 * Do not make Curl_vsetopt() static: it is called from
 * projects/OS400/ccsidcurl.c.
 */
CURLcode Curl_vsetopt(ساختار Curl_easy *data, CURLoption option, va_list param)
{
  اگر(option < CURLOPTTYPE_OBJECTPOINT)
    بازگشت setopt_long(data, option, va_arg(param, long));
  وگرنه اگر(option < CURLOPTTYPE_FUNCTIONPOINT) {
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
    حالت CURLOPT_CONNECT_TO:
      بازگشت setopt_slist(data, option, va_arg(param, ساختار curl_slist *));
    حالت CURLOPT_HTTPPOST:         /* curl_httppost * */
    حالت CURLOPT_MIMEPOST:         /* curl_mime * */
    حالت CURLOPT_STDERR:           /* FILE * */
    حالت CURLOPT_SHARE:            /* CURLSH * */
    حالت CURLOPT_CURLU:            /* CURLU * */
      بازگشت setopt_pointers(data, option, param);
    حالت CURLOPT_STREAM_DEPENDS:   /* CURL * */
    حالت CURLOPT_STREAM_DEPENDS_E: /* CURL * */
      بازگشت CURLE_OK;
    پیشفرض:
      بشکن;
    }
    /* the char pointer options */
    بازگشت setopt_cptr(data, option, va_arg(param, نویسه *));
  }
  وگرنه اگر(option < CURLOPTTYPE_OFF_T)
    بازگشت setopt_func(data, option, param);
  وگرنه اگر(option < CURLOPTTYPE_BLOB)
    بازگشت setopt_offt(data, option, va_arg(param, curl_off_t));
  بازگشت setopt_blob(data, option, va_arg(param, ساختار curl_blob *));
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
  ساختار Curl_eapi_guard guard;
  CURLcode result;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_setopt, &result)) {
    ساختار Curl_easy *data = curl;
    va_list arg;

    va_start(arg, option);

    result = Curl_vsetopt(data, option, arg);

    va_end(arg);
    اگر(result == CURLE_BAD_FUNCTION_ARGUMENT)
      failf(data, "setopt 0x%x got bad argument", (unsigned صحیح)option);
  }
  CURL_EAPI_LEAVE(&guard);
  بازگشت result;
}
