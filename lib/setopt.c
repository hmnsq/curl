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

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#ضمن <linux/tcp.h>
#وإلاإذا defined(HAVE_NETINET_TCP_H)
#ضمن <netinet/tcp.h>
#endif

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

ساكن CURLcode setopt_set_timeout_sec(timediff_t *ptimeout_ms, long secs)
{
  ܐܢ(secs < 0)
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
#ܐܢ LONG_MAX > (TIMEDIFF_T_MAX / 1000)
  ܐܢ(secs > (TIMEDIFF_T_MAX / 1000)) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    ܦܢܐ CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)secs * 1000;
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode setopt_set_timeout_ms(timediff_t *ptimeout_ms, long ms)
{
  ܐܢ(ms < 0)
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
#ܐܢ LONG_MAX > TIMEDIFF_T_MAX
  ܐܢ(ms > TIMEDIFF_T_MAX) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    ܦܢܐ CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)ms;
  ܦܢܐ CURLE_OK;
}

CURLcode Curl_setstropt(بنية Curl_easy *data,
                        تعداد dupstring id, ثابت حرف *s)
{
  size_t slen = s ? strlen(s) : 0;
  DEBUGASSERT((unsigned)id <= UINT8_MAX);
  ܐܢ(s && (slen > CURL_MAX_INPUT_LENGTH))
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;

  ܦܢܐ CURL_EASY_STR_SET(data, (uint8_t)id, s, slen);
}

CURLcode Curl_setblobopt(بنية curl_blob **blobp,
                         ثابت بنية curl_blob *blob)
{
  /* free the previous storage at `blobp' and replace by a dynamic storage
     copy of blob. If CURL_BLOB_COPY is set, the data is copied. */

  curlx_safefree(*blobp);

  ܐܢ(blob) {
    بنية curl_blob *nblob;
    ܐܢ(!blob->data || !blob->len || (blob->len > CURL_MAX_INPUT_LENGTH))
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    nblob = (بنية curl_blob *)
      curlx_malloc(حجم(بنية curl_blob) +
                   ((blob->flags & CURL_BLOB_COPY) ? blob->len : 0));
    ܐܢ(!nblob)
      ܦܢܐ CURLE_OUT_OF_MEMORY;
    *nblob = *blob;
    ܐܢ(blob->flags & CURL_BLOB_COPY) {
      /* put the data after the blob struct in memory */
      nblob->data = (حرف *)nblob + حجم(بنية curl_blob);
      memcpy(nblob->data, blob->data, blob->len);
    }

    *blobp = nblob;
    ܦܢܐ CURLE_OK;
  }

  ܦܢܐ CURLE_OK;
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
  ܐܢ(option) {
    size_t len = strlen(option);
    CURLcode result;
    ܐܢ(len > CURL_MAX_INPUT_LENGTH)
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;

    result = Curl_parse_login_details(option, len, &user, &passwd, عدم);
    ܐܢ(result)
      ܦܢܐ result;
  }

  curlx_free(*userp);
  *userp = user;

  curlx_strzero(*passwdp);
  curlx_free(*passwdp);
  *passwdp = passwd;

  ܦܢܐ CURLE_OK;
}

ساكن CURLcode setstropt_interface(بنية Curl_easy *data, حرف *option)
{
  ܐܢ(option && !option[0])
    option = عدم;
  ܐܢ(option) {
    /* Parse for checking syntax */
    CURLcode result = Curl_parse_interface(option, عدم, عدم, عدم);
    ܐܢ(result)
      ܦܢܐ result;
    ܦܢܐ CURL_EASY_STR_SET(data, STRING_INTERFACE_OPT,
                             option, strlen(option));
  }
  CURL_EASY_STR_CLEAR(data, STRING_INTERFACE_OPT);
  ܦܢܐ CURLE_OK;
}

#ifdef USE_SSL
#define C_SSLVERSION_VALUE(x)     ((x) & 0xffff)
#define C_SSLVERSION_MAX_VALUE(x) ((unsigned long)(x) & 0xffff0000)
#endif

ساكن CURLcode protocol2num(ثابت حرف *str, curl_prot_t *val)
{
  /*
   * We are asked to cherry-pick protocols, so play it safe and disallow all
   * protocols to start with, and re-add the wanted ones back in.
   */
  *val = 0;

  ܐܢ(!str)
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;

  ܐܢ(curl_strequal(str, "all")) {
    *val = ~(curl_prot_t)0;
    ܦܢܐ CURLE_OK;
  }

  ܥܒܕ {
    ثابت حرف *token = str;
    size_t tlen;

    str = strchr(str, ',');
    tlen = str ? (size_t)(str - token) : strlen(token);
    ܐܢ(tlen) {
      ثابت بنية Curl_scheme *h = Curl_getn_scheme(token, tlen);

      ܐܢ(!h || !h->run)
        ܦܢܐ CURLE_UNSUPPORTED_PROTOCOL;

      *val |= h->protocol;
    }
  } ܟܡܐ(str && str++);

  ܐܢ(!*val)
    /* no protocol listed */
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
  ܦܢܐ CURLE_OK;
}

#ܐܢ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
ساكن CURLcode httpauth(بنية Curl_easy *data, bool proxy,
                         unsigned long auth)
{
  ܐܢ(auth != CURLAUTH_NONE) {
    صحيح bitcheck = 0;
    bool authbits = FALSE;
    ܐܢ(auth & CURLAUTH_DIGEST_IE) {
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
    ܟܡܐ(bitcheck < 31) {
      ܐܢ(auth & (1UL << bitcheck++)) {
        authbits = TRUE;
        اكسر;
      }
    }
    ܐܢ(!authbits)
      ܦܢܐ CURLE_NOT_BUILT_IN; /* no supported types left! */
  }
  ܐܢ(proxy)
    data->set.proxyauth = (uint32_t)auth;
  ܐܚܪܢܐ
    data->set.httpauth = (uint32_t)auth;
  ܦܢܐ CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */

#ifndef CURL_DISABLE_HTTP
ساكن CURLcode setopt_HTTP_VERSION(بنية Curl_easy *data, long arg)
{
  /*
   * This sets a requested HTTP version to be used. The value is one of
   * the listed enums in curl/curl.h.
   */
  بدل(arg) {
  حالة CURL_HTTP_VERSION_NONE:
    /* accepted */
    اكسر;
  حالة CURL_HTTP_VERSION_1_0:
  حالة CURL_HTTP_VERSION_1_1:
    /* accepted */
    اكسر;
#ifdef USE_HTTP2
  حالة CURL_HTTP_VERSION_2_0:
  حالة CURL_HTTP_VERSION_2TLS:
  حالة CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    /* accepted */
    اكسر;
#endif
#ifdef USE_HTTP3
  حالة CURL_HTTP_VERSION_3:
  حالة CURL_HTTP_VERSION_3ONLY:
    /* accepted */
    اكسر;
#endif
  افتراضي:
    /* not accepted */
    ܐܢ(arg < CURL_HTTP_VERSION_NONE)
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    ܦܢܐ CURLE_UNSUPPORTED_PROTOCOL;
  }
  data->set.httpwant = (unsigned حرف)arg;
  ܦܢܐ CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP */

#ifdef USE_SSL
CURLcode Curl_setopt_SSLVERSION(بنية Curl_easy *data, CURLoption option,
                                long arg)
{
  /*
   * Set explicit SSL version to try to connect with, as some SSL
   * implementations are lame.
   */
  {
    long version, version_max;
    بنية ssl_easy_config *sslc = &data->set.ssl;
#ifndef CURL_DISABLE_PROXY
    ܐܢ(option != CURLOPT_SSLVERSION)
      sslc = &data->set.proxy_ssl;
#ܐܚܪܢܐ
    (فراغ)option; /* unused */
#endif
    version = C_SSLVERSION_VALUE(arg);
    version_max = (long)C_SSLVERSION_MAX_VALUE(arg);

    ܐܢ(version < CURL_SSLVERSION_DEFAULT ||
       version == CURL_SSLVERSION_SSLv2 ||
       version == CURL_SSLVERSION_SSLv3 ||
       version >= CURL_SSLVERSION_LAST ||
       version_max < CURL_SSLVERSION_MAX_NONE ||
       version_max >= CURL_SSLVERSION_MAX_LAST)
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    ܐܢ(version == CURL_SSLVERSION_DEFAULT)
      version = CURL_SSLVERSION_TLSv1_2;

    sslc->version = (unsigned حرف)version;
    sslc->version_max = (unsigned صحيح)version_max;
  }
  ܦܢܐ CURLE_OK;
}
#endif /* !USE_SSL */

#ifndef CURL_DISABLE_RTSP
ساكن CURLcode setopt_RTSP_REQUEST(بنية Curl_easy *data, long arg)
{
  /*
   * Set the RTSP request method (OPTIONS, SETUP, PLAY, etc...) Would this be
   * better if the RTSPREQ_* were moved into here?
   */
  ܐܢ((arg <= CURL_RTSPREQ_NONE) || (arg >= CURL_RTSPREQ_LAST))
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;

  data->set.rtspreq = (unsigned حرف)arg;
  ܦܢܐ CURLE_OK;
}
#endif /* !CURL_DISABLE_RTSP */

ساكن CURLcode setopt_long_bool(بنية Curl_easy *data, CURLoption option,
                                 long arg)
{
  bool enabled = !!arg;
  صحيح ok = 1;
  بنية UserDefined *s = &data->set;
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
  حالة CURLOPT_HEADER:
    /*
     * Set to include the header in the general data output stream.
     */
    s->include_header = enabled;
    اكسر;
  حالة CURLOPT_NOPROGRESS:
    /*
     * Shut off the internal supported progress meter
     */
    data->progress.hide = enabled;
    اكسر;
  حالة CURLOPT_NOBODY:
    /*
     * Do not include the body part in the output data stream.
     */
    s->opt_no_body = enabled;
#ifndef CURL_DISABLE_HTTP
    ܐܢ(s->opt_no_body)
      /* in HTTP lingo, no body means using the HEAD request... */
      s->method = HTTPREQ_HEAD;
    ܐܚܪܢܐ ܐܢ(s->method == HTTPREQ_HEAD)
      s->method = HTTPREQ_GET;
#endif
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
    ܐܢ(enabled) {
      /* If this is HTTP, PUT is what's needed to "upload" */
      s->method = HTTPREQ_PUT;
      s->opt_no_body = FALSE; /* this is implied */
    }
    ܐܚܪܢܐ
      /* In HTTP, the opposite of upload is GET (unless NOBODY is true as
         then this can be changed to HEAD later on) */
      s->method = HTTPREQ_GET;
    اكسر;
  حالة CURLOPT_FILETIME:
    /*
     * Try to get the file time of the remote document. The time will
     * later (possibly) become available using curl_easy_getinfo().
     */
    s->get_filetime = enabled;
    اكسر;
#ifndef CURL_DISABLE_HTTP
  حالة CURLOPT_HTTP09_ALLOWED:
    s->http09_allowed = enabled;
    اكسر;
#ifndef CURL_DISABLE_COOKIES
  حالة CURLOPT_COOKIESESSION:
    /*
     * Set this option to TRUE to start a new "cookie session". It will
     * prevent the forthcoming read-cookies-from-file actions to accept
     * cookies that are marked as being session cookies, as they belong to a
     * previous session.
     */
    s->cookiesession = enabled;
    اكسر;
#endif
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
    s->allow_auth_to_other_hosts = enabled;
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
    ܐܢ(enabled) {
      s->method = HTTPREQ_GET;
      s->opt_no_body = FALSE; /* this is implied */
    }
    اكسر;
  حالة CURLOPT_POST:
    /* Does this option serve a purpose anymore? Yes it does, when
       CURLOPT_POSTFIELDS is not used and the POST data is read off the
       callback! */
    ܐܢ(enabled) {
      s->method = HTTPREQ_POST;
      s->opt_no_body = FALSE; /* this is implied */
    }
    ܐܚܪܢܐ
      s->method = HTTPREQ_GET;
    اكسر;
#endif /* !CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_PROXY
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
    Curl_ssl_conn_config_update(data, TRUE);
    اكسر;
  حالة CURLOPT_PROXY_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for proxy
     */
    s->proxy_ssl.verifyhost = enabled;
    ok = 2;
    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    اكسر;
  حالة CURLOPT_PROXY_TRANSFER_MODE:
    /*
     * set transfer mode (;type=<a|i>) when doing FTP via an HTTP proxy
     */
    s->proxy_transfer_mode = enabled;
    اكسر;
#endif /* !CURL_DISABLE_PROXY */
#ܐܢ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  حالة CURLOPT_SOCKS5_GSSAPI_NEC:
    /*
     * Set flag for NEC SOCKS5 support
     */
    s->socks5_gssapi_nec = enabled;
    اكسر;
#endif
#ifdef CURL_LIST_ONLY_PROTOCOL
  حالة CURLOPT_DIRLISTONLY:
    /*
     * An option that changes the command to one that asks for a list only, no
     * file info details. Used for FTP, POP3 and SFTP.
     */
    s->list_only = enabled;
    اكسر;
#endif
  حالة CURLOPT_APPEND:
    /*
     * We want to upload and append to an existing file. Used for FTP and
     * SFTP.
     */
    s->remote_append = enabled;
    اكسر;
#ifndef CURL_DISABLE_FTP
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
#endif
  حالة CURLOPT_CRLF:
    /*
     * Kludgy option to enable CRLF conversions. Subject for removal.
     */
    s->crlf = enabled;
    اكسر;
#ifndef CURL_DISABLE_TFTP
  حالة CURLOPT_TFTP_NO_OPTIONS:
    /*
     * Option that prevents libcurl from sending TFTP option requests to the
     * server.
     */
    s->tftp_no_options = enabled;
    اكسر;
#endif /* !CURL_DISABLE_TFTP */
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
    Curl_ssl_conn_config_update(data, FALSE);
    اكسر;
#ifndef CURL_DISABLE_DOH
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
    ܐܢ(!Curl_ssl_cert_status_request())
      ܦܢܐ CURLE_NOT_BUILT_IN;

    s->doh_verifystatus = enabled;
    ok = 2;
    اكسر;
#endif /* !CURL_DISABLE_DOH */
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
    Curl_ssl_conn_config_update(data, FALSE);
    اكسر;
  حالة CURLOPT_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying.
     */
    ܐܢ(!Curl_ssl_cert_status_request())
      ܦܢܐ CURLE_NOT_BUILT_IN;

    s->ssl.verifystatus = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    اكسر;
  حالة CURLOPT_CERTINFO:
#ifdef USE_SSL
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_CERTINFO))
      s->ssl.certinfo = enabled;
    ܐܚܪܢܐ
#endif
      ܦܢܐ CURLE_NOT_BUILT_IN;
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
  حالة CURLOPT_IGNORE_CONTENT_LENGTH:
    s->ignorecl = enabled;
    اكسر;
  حالة CURLOPT_SSL_SESSIONID_CACHE:
    s->ssl.cache_session = enabled;
#ifndef CURL_DISABLE_PROXY
    s->proxy_ssl.cache_session = s->ssl.cache_session;
#endif
    اكسر;
#ifdef USE_SSH
  حالة CURLOPT_SSH_COMPRESSION:
    s->ssh_compression = enabled;
    اكسر;
#endif /* !USE_SSH */
#ifndef CURL_DISABLE_SMTP
  حالة CURLOPT_MAIL_RCPT_ALLOWFAILS:
    /* allow RCPT TO command to fail for some recipients */
    s->mail_rcpt_allowfails = enabled;
    اكسر;
#endif /* !CURL_DISABLE_SMTP */
  حالة CURLOPT_SASL_IR:
    /* Enable/disable SASL initial response */
    s->sasl_ir = enabled;
    اكسر;
  حالة CURLOPT_TCP_KEEPALIVE:
    s->tcp_keepalive = enabled;
    اكسر;
  حالة CURLOPT_TCP_FASTOPEN:
#ܐܢ defined(CONNECT_DATA_IDEMPOTENT) || defined(MSG_FASTOPEN) || \
  defined(TCP_FASTOPEN_CONNECT)
    s->tcp_fastopen = enabled;
    اكسر;
#ܐܚܪܢܐ
    ܦܢܐ CURLE_NOT_BUILT_IN;
#endif
  حالة CURLOPT_SSL_ENABLE_ALPN:
    s->ssl_enable_alpn = enabled;
    اكسر;
  حالة CURLOPT_PATH_AS_IS:
    s->path_as_is = enabled;
    اكسر;
  حالة CURLOPT_PIPEWAIT:
    s->pipewait = enabled;
    اكسر;
  حالة CURLOPT_SUPPRESS_CONNECT_HEADERS:
    s->suppress_connect_headers = enabled;
    اكسر;
#ifndef CURL_DISABLE_SHUFFLE_DNS
  حالة CURLOPT_DNS_SHUFFLE_ADDRESSES:
    s->dns_shuffle_addresses = enabled;
    اكسر;
#endif
  حالة CURLOPT_DISALLOW_USERNAME_IN_URL:
    s->disallow_username_in_url = enabled;
    اكسر;
  حالة CURLOPT_QUICK_EXIT:
    s->quick_exit = enabled;
    اكسر;
  افتراضي:
    ܦܢܐ CURLE_UNKNOWN_OPTION;
  }
  ܐܢ((arg > ok) || (arg < 0))
    /* reserve other values for future use */
    infof(data, "boolean setopt(%d) got unsupported argument %ld,"
          " treated as %d", (صحيح)option, arg, enabled);

  ܦܢܐ CURLE_OK;
}

ساكن CURLcode value_range(long *value, long below_error, long min, long max)
{
  ܐܢ(*value < below_error)
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
  ܐܚܪܢܐ ܐܢ(*value < min)
    *value = min;
  ܐܚܪܢܐ ܐܢ(*value > max)
    *value = max;
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode setopt_long_net(بنية Curl_easy *data, CURLoption option,
                                long arg)
{
  CURLcode result = CURLE_OK;
  بنية UserDefined *s = &data->set;

  بدل(option) {
  حالة CURLOPT_DNS_CACHE_TIMEOUT:
    ܐܢ(arg != -1)
      ܦܢܐ setopt_set_timeout_sec(&s->dns_cache_timeout_ms, arg);
    s->dns_cache_timeout_ms = -1;
    اكسر;
  حالة CURLOPT_MAXCONNECTS:
    result = value_range(&arg, 0, 0, INT_MAX);
    ܐܢ(!result)
      s->maxconnects = arg ? (uint32_t)arg : DEFAULT_CONNCACHE_SIZE;
    اكسر;
  حالة CURLOPT_SERVER_RESPONSE_TIMEOUT:
    ܦܢܐ setopt_set_timeout_sec(&s->server_response_timeout, arg);
  حالة CURLOPT_SERVER_RESPONSE_TIMEOUT_MS:
    ܦܢܐ setopt_set_timeout_ms(&s->server_response_timeout, arg);
  حالة CURLOPT_LOW_SPEED_LIMIT:
    ܐܢ(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ܐܚܪܢܐ
      s->low_speed_limit = arg;
    اكسر;
  حالة CURLOPT_LOW_SPEED_TIME:
    result = value_range(&arg, 0, 0, USHRT_MAX);
    ܐܢ(!result)
      s->low_speed_time = (uint16_t)arg;
    اكسر;
  حالة CURLOPT_PORT:
    ܐܢ((arg < 0) || (arg > 65535))
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    s->use_port = (unsigned short)arg;
    اكسر;
  حالة CURLOPT_TIMEOUT:
    ܦܢܐ setopt_set_timeout_sec(&s->timeout, arg);
  حالة CURLOPT_TIMEOUT_MS:
    ܦܢܐ setopt_set_timeout_ms(&s->timeout, arg);
  حالة CURLOPT_CONNECTTIMEOUT:
    ܦܢܐ setopt_set_timeout_sec(&s->connecttimeout, arg);
  حالة CURLOPT_CONNECTTIMEOUT_MS:
    ܦܢܐ setopt_set_timeout_ms(&s->connecttimeout, arg);
#ifndef CURL_DISABLE_BINDLOCAL
  حالة CURLOPT_LOCALPORT:
    ܐܢ((arg < 0) || (arg > 65535))
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    s->localport = curlx_sltous(arg);
    اكسر;
  حالة CURLOPT_LOCALPORTRANGE:
    ܐܢ((arg < 0) || (arg > 65535))
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    s->localportrange = curlx_sltous(arg);
    اكسر;
#endif
  حالة CURLOPT_BUFFERSIZE:
    result = value_range(&arg, 0, READBUFFER_MIN, READBUFFER_MAX);
    ܐܢ(!result)
      s->buffer_size = (unsigned صحيح)arg;
    اكسر;
  حالة CURLOPT_UPLOAD_BUFFERSIZE:
    result = value_range(&arg, 0, UPLOADBUFFER_MIN, UPLOADBUFFER_MAX);
    ܐܢ(!result)
      s->upload_buffer_size = (unsigned صحيح)arg;
    اكسر;
  حالة CURLOPT_MAXFILESIZE:
    ܐܢ(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ܐܚܪܢܐ
      s->max_filesize = arg;
    اكسر;
  حالة CURLOPT_IPRESOLVE:
    ܐܢ((arg < CURL_IPRESOLVE_WHATEVER) || (arg > CURL_IPRESOLVE_V6))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ܐܚܪܢܐ
      s->ipver = (unsigned حرف)arg;
    اكسر;
  حالة CURLOPT_CONNECT_ONLY:
    ܐܢ(arg < 0 || arg > 2)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ܐܚܪܢܐ {
      s->connect_only = !!arg;
      s->connect_only_ws = (arg == 2);
    }
    اكسر;
#ifdef USE_IPV6
  حالة CURLOPT_ADDRESS_SCOPE:
#ܐܢ SIZEOF_LONG > 4
    ܐܢ((unsigned long)arg > UINT_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ܐܚܪܢܐ
#endif
    s->scope_id = (unsigned صحيح)arg;
    اكسر;
#endif
  حالة CURLOPT_TCP_KEEPIDLE:
    result = value_range(&arg, 0, 0, INT_MAX);
    ܐܢ(!result)
      s->tcp_keepidle = (صحيح)arg;
    اكسر;
  حالة CURLOPT_TCP_KEEPINTVL:
    result = value_range(&arg, 0, 0, INT_MAX);
    ܐܢ(!result)
      s->tcp_keepintvl = (صحيح)arg;
    اكسر;
  حالة CURLOPT_TCP_KEEPCNT:
    result = value_range(&arg, 0, 0, INT_MAX);
    ܐܢ(!result)
      s->tcp_keepcnt = (صحيح)arg;
    اكسر;
  حالة CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
    ܦܢܐ setopt_set_timeout_ms(&s->happy_eyeballs_timeout, arg);
  حالة CURLOPT_UPKEEP_INTERVAL_MS:
    ܦܢܐ setopt_set_timeout_ms(&s->upkeep_interval_ms, arg);
  حالة CURLOPT_MAXAGE_CONN:
    ܦܢܐ setopt_set_timeout_sec(&s->conn_max_idle_ms, arg);
  حالة CURLOPT_MAXLIFETIME_CONN:
    ܦܢܐ setopt_set_timeout_sec(&s->conn_max_age_ms, arg);
  حالة CURLOPT_DNS_USE_GLOBAL_CACHE:
    /* deprecated */
    اكسر;
  افتراضي:
    ܦܢܐ CURLE_UNKNOWN_OPTION;
  }
  ܦܢܐ result;
}

ساكن CURLcode setopt_long_ssl(بنية Curl_easy *data, CURLoption option,
                                long arg)
{
#ifdef USE_SSL
  CURLcode result = CURLE_OK;
  بنية UserDefined *s = &data->set;
  بدل(option) {
  حالة CURLOPT_CA_CACHE_TIMEOUT:
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_CA_CACHE)) {
      result = value_range(&arg, -1, -1, INT_MAX);
      ܐܢ(!result)
        s->ssl_ca_cache_timeout = (صحيح)arg;
    }
    ܐܚܪܢܐ
      result = CURLE_NOT_BUILT_IN;
    اكسر;
  حالة CURLOPT_SSLVERSION:
#ifndef CURL_DISABLE_PROXY
  حالة CURLOPT_PROXY_SSLVERSION:
#endif
    ܦܢܐ Curl_setopt_SSLVERSION(data, option, arg);
  حالة CURLOPT_SSL_FALSESTART:
    result = CURLE_NOT_BUILT_IN;
    اكسر;
  حالة CURLOPT_USE_SSL:
    ܐܢ((arg < CURLUSESSL_NONE) || (arg >= CURLUSESSL_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ܐܚܪܢܐ
      s->use_ssl = (unsigned حرف)arg;
    اكسر;
  حالة CURLOPT_SSL_OPTIONS:
    s->ssl.ssl_options = (unsigned حرف)(arg & 0xff);
    اكسر;
#ifndef CURL_DISABLE_PROXY
  حالة CURLOPT_PROXY_SSL_OPTIONS:
    s->proxy_ssl.ssl_options = (unsigned حرف)(arg & 0xff);
    اكسر;
#endif
  حالة CURLOPT_SSL_ENABLE_NPN:
    اكسر;
  حالة CURLOPT_SSLENGINE_DEFAULT:
#ifdef USE_OPENSSL
    ܐܢ(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      ܦܢܐ CURLE_NOT_BUILT_IN;
    data->set.engine_default = (arg == 1);
#ܐܚܪܢܐ
    ܦܢܐ CURLE_NOT_BUILT_IN;
#endif
    اكسر;
  افتراضي:
    ܦܢܐ CURLE_UNKNOWN_OPTION;
  }
  ܦܢܐ result;
#ܐܚܪܢܐ  /* USE_SSL */
  (فراغ)data;
  (فراغ)option;
  (فراغ)arg;
  ܦܢܐ CURLE_UNKNOWN_OPTION;
#endif /* !USE_SSL */
}

#ifndef CURL_DISABLE_PROXY
ساكن فراغ changeproxy(بنية Curl_easy *data)
{
  Curl_auth_digest_cleanup(&data->state.proxydigest);
  memset(&data->state.authproxy, 0, حجم(data->state.authproxy));
}

ساكن CURLcode setopt_long_proxy(بنية Curl_easy *data, CURLoption option,
                                  long arg)
{
  بنية UserDefined *s = &data->set;

  بدل(option) {
  حالة CURLOPT_PROXYPORT:
    ܐܢ((arg < 0) || (arg > UINT16_MAX))
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    ܐܢ(arg != s->proxyport)
      changeproxy(data);
    s->proxyport = (uint16_t)arg;
    اكسر;
  حالة CURLOPT_PROXYAUTH:
    ܦܢܐ httpauth(data, TRUE, (unsigned long)arg);
  حالة CURLOPT_PROXYTYPE:
    ܐܢ((arg < CURLPROXY_HTTP) || (arg > CURLPROXY_HTTPS3))
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef USE_PROXY_HTTP3
    ܐܢ(arg == CURLPROXY_HTTPS3)
      ܦܢܐ CURLE_NOT_BUILT_IN;
#endif
    s->proxytype = (unsigned حرف)arg;
    اكسر;
  حالة CURLOPT_SOCKS5_AUTH: {
    unsigned long auth = (unsigned long)arg;
    ܐܢ(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI | CURLAUTH_ONLY))
      ܦܢܐ CURLE_NOT_BUILT_IN;
    ܐܢ((auth & CURLAUTH_ONLY) && !(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI)))
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    s->socks5auth = (unsigned حرف)(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI));
    s->socks5_auth_only = !!(auth & CURLAUTH_ONLY);
    اكسر;
  }
  افتراضي:
    ܦܢܐ CURLE_UNKNOWN_OPTION;
  }
  ܦܢܐ CURLE_OK;
}
#ܐܚܪܢܐ
ساكن CURLcode setopt_long_proxy(بنية Curl_easy *data, CURLoption option,
                                  long arg)
{
  (فراغ)data;
  (فراغ)option;
  (فراغ)arg;
  ܦܢܐ CURLE_UNKNOWN_OPTION;
}
#endif

ساكن CURLcode setopt_long_http(بنية Curl_easy *data, CURLoption option,
                                 long arg)
{
#ifndef CURL_DISABLE_HTTP
  CURLcode result = CURLE_OK;
  بنية UserDefined *s = &data->set;

  بدل(option) {
  حالة CURLOPT_FOLLOWLOCATION:
    ܐܢ((unsigned long)arg > 3)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ܐܚܪܢܐ
      s->http_follow_mode = (unsigned حرف)arg;
    اكسر;
  حالة CURLOPT_MAXREDIRS:
    result = value_range(&arg, -1, -1, 0x7fff);
    ܐܢ(!result)
      s->maxredirs = (short)arg;
    اكسر;
  حالة CURLOPT_POSTREDIR:
    ܐܢ(arg < CURL_REDIR_GET_ALL)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ܐܚܪܢܐ {
      s->post301 = !!(arg & CURL_REDIR_POST_301);
      s->post302 = !!(arg & CURL_REDIR_POST_302);
      s->post303 = !!(arg & CURL_REDIR_POST_303);
    }
    اكسر;
  حالة CURLOPT_HEADEROPT:
    s->sep_headers = !!(arg & CURLHEADER_SEPARATE);
    اكسر;
  حالة CURLOPT_HTTPAUTH:
    ܦܢܐ httpauth(data, FALSE, (unsigned long)arg);
  حالة CURLOPT_HTTP_VERSION:
    ܦܢܐ setopt_HTTP_VERSION(data, arg);
  حالة CURLOPT_EXPECT_100_TIMEOUT_MS:
    result = value_range(&arg, 0, 0, 0xffff);
    ܐܢ(!result)
      s->expect_100_timeout = (unsigned short)arg;
    اكسر;
  حالة CURLOPT_STREAM_WEIGHT:
#ܐܢ defined(USE_HTTP2) || defined(USE_HTTP3)
    ܐܢ((arg >= 1) && (arg <= 256))
      s->weight = (صحيح)arg;
    اكسر;
#ܐܚܪܢܐ
    result = CURLE_NOT_BUILT_IN;
    اكسر;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  حالة CURLOPT_HTTPSIG_ALGORITHM:
    ܐܢ(arg != CURLHTTPSIG_NONE &&
       arg != CURLHTTPSIG_ED25519 &&
       arg != CURLHTTPSIG_HMAC_SHA256)
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    s->httpsig_algorithm = (uint8_t)arg;
    ܐܢ(arg)
      s->httpauth = (uint32_t)CURLAUTH_HTTPSIG;
    ܐܚܪܢܐ
      s->httpauth &= ~(uint32_t)CURLAUTH_HTTPSIG;
    اكسر;
#endif
  افتراضي:
    ܦܢܐ CURLE_UNKNOWN_OPTION;
  }
  ܦܢܐ result;
#ܐܚܪܢܐ
  (فراغ)data;
  (فراغ)option;
  (فراغ)arg;
  ܦܢܐ CURLE_UNKNOWN_OPTION;
#endif
}

ساكن CURLcode setopt_long_proto(بنية Curl_easy *data, CURLoption option,
                                  long arg)
{
  CURLcode result = CURLE_OK;
  بنية UserDefined *s = &data->set;

  بدل(option) {
#ifndef CURL_DISABLE_TFTP
  حالة CURLOPT_TFTP_BLKSIZE:
    result = value_range(&arg, 0, TFTP_BLKSIZE_MIN, TFTP_BLKSIZE_MAX);
    ܐܢ(!result)
      s->tftp_blksize = (unsigned short)arg;
    اكسر;
#endif
#ifndef CURL_DISABLE_NETRC
  حالة CURLOPT_NETRC:
    ܐܢ((arg < CURL_NETRC_IGNORED) || (arg >= CURL_NETRC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ܐܚܪܢܐ
      s->use_netrc = (unsigned حرف)arg;
    اكسر;
#endif
#ifndef CURL_DISABLE_FTP
  حالة CURLOPT_FTP_FILEMETHOD:
    ܐܢ((arg < CURLFTPMETHOD_DEFAULT) || (arg >= CURLFTPMETHOD_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ܐܚܪܢܐ
      s->ftp_filemethod = (unsigned حرف)arg;
    اكسر;
  حالة CURLOPT_FTP_SSL_CCC:
    ܐܢ((arg < CURLFTPSSL_CCC_NONE) || (arg >= CURLFTPSSL_CCC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ܐܚܪܢܐ
      s->ftp_ccc = (unsigned حرف)arg;
    اكسر;
  حالة CURLOPT_FTPSSLAUTH:
    ܐܢ((arg < CURLFTPAUTH_DEFAULT) || (arg >= CURLFTPAUTH_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ܐܚܪܢܐ
      s->ftpsslauth = (unsigned حرف)arg;
    اكسر;
  حالة CURLOPT_ACCEPTTIMEOUT_MS:
    ܦܢܐ setopt_set_timeout_ms(&s->accepttimeout, arg);
#endif
#ܐܢ !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  حالة CURLOPT_FTP_CREATE_MISSING_DIRS:
    ܐܢ((arg < CURLFTP_CREATE_DIR_NONE) || (arg > CURLFTP_CREATE_DIR_RETRY))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ܐܚܪܢܐ
      s->ftp_create_missing_dirs = (unsigned حرف)arg;
    اكسر;
  حالة CURLOPT_NEW_FILE_PERMS:
    ܐܢ((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ܐܚܪܢܐ
      s->new_file_perms = (unsigned صحيح)arg;
    اكسر;
#endif
#ifndef CURL_DISABLE_RTSP
  حالة CURLOPT_RTSP_REQUEST:
    ܦܢܐ setopt_RTSP_REQUEST(data, arg);
  حالة CURLOPT_RTSP_CLIENT_CSEQ:
    result = value_range(&arg, 0, 0, INT_MAX);
    ܐܢ(!result)
      data->state.rtsp_next_client_CSeq = (uint32_t)arg;
    اكسر;
  حالة CURLOPT_RTSP_SERVER_CSEQ:
    result = value_range(&arg, 0, 0, INT_MAX);
    ܐܢ(!result)
      data->state.rtsp_next_server_CSeq = (uint32_t)arg;
    اكسر;
#endif
#ifdef USE_SSH
  حالة CURLOPT_SSH_AUTH_TYPES:
    s->ssh_auth_types = (uint32_t)arg;
    اكسر;
  حالة CURLOPT_NEW_DIRECTORY_PERMS:
    ܐܢ((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ܐܚܪܢܐ
      s->new_directory_perms = (unsigned صحيح)arg;
    اكسر;
#endif
  حالة CURLOPT_PROTOCOLS:
    s->allowed_protocols = (curl_prot_t)arg;
    اكسر;
  حالة CURLOPT_REDIR_PROTOCOLS:
    s->redir_protocols = (curl_prot_t)arg;
    اكسر;
#ifndef CURL_DISABLE_WEBSOCKETS
  حالة CURLOPT_WS_OPTIONS:
    s->ws_raw_mode = !!(arg & CURLWS_RAW_MODE);
    s->ws_no_auto_pong = !!(arg & CURLWS_NOAUTOPONG);
    اكسر;
#endif
  افتراضي:
    ܦܢܐ CURLE_UNKNOWN_OPTION;
  }
  ܦܢܐ result;
}

ساكن CURLcode setopt_long_misc(بنية Curl_easy *data, CURLoption option,
                                 long arg)
{
  بنية UserDefined *s = &data->set;

  بدل(option) {
  حالة CURLOPT_TIMECONDITION:
    ܐܢ((arg < CURL_TIMECOND_NONE) || (arg >= CURL_TIMECOND_LAST))
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    s->timecondition = (unsigned حرف)arg;
    اكسر;
  حالة CURLOPT_TIMEVALUE:
    s->timevalue = (time_t)arg;
    اكسر;
  حالة CURLOPT_POSTFIELDSIZE:
    ܐܢ(arg < -1)
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    ܐܢ(s->postfieldsize < arg && s->str_copypostfields) {
      curlx_safefree(s->str_copypostfields);
      s->postfields = عدم;
    }
    s->postfieldsize = arg;
    اكسر;
  حالة CURLOPT_INFILESIZE:
    ܐܢ(arg < -1)
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = arg;
    اكسر;
  حالة CURLOPT_RESUME_FROM:
    ܐܢ(arg < -1)
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = arg;
    اكسر;
  حالة CURLOPT_UPLOAD_FLAGS:
    s->upload_flags = (unsigned حرف)arg;
    اكسر;
#ifndef CURL_DISABLE_MIME
  حالة CURLOPT_MIME_OPTIONS:
    s->mime_formescape = !!(arg & CURLMIMEOPT_FORMESCAPE);
    اكسر;
#endif
#ifndef CURL_DISABLE_HSTS
  حالة CURLOPT_HSTS_CTRL:
    ܐܢ(arg & CURLHSTS_ENABLE) {
      ܐܢ(!data->hsts) {
        data->hsts = Curl_hsts_init();
        ܐܢ(!data->hsts)
          ܦܢܐ CURLE_OUT_OF_MEMORY;
      }
    }
    ܐܚܪܢܐ ܐܢ(!data->share || !data->share->hsts) {
      /* throw away the HSTS cache unless shared */
      Curl_hsts_cleanup(&data->hsts);
      /* flush all the entries */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = عدم;
    }
    ܐܚܪܢܐ
      /* detach from shared HSTS cache without freeing it */
      data->hsts = عدم;
    اكسر;
#endif
#ifndef CURL_DISABLE_ALTSVC
  حالة CURLOPT_ALTSVC_CTRL:
    ܦܢܐ Curl_altsvc_ctrl(data, arg);
#endif
#ܐܢ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  حالة CURLOPT_GSSAPI_DELEGATION:
    s->gssapi_delegation = (unsigned حرف)arg &
      (CURLGSSAPI_DELEGATION_POLICY_FLAG | CURLGSSAPI_DELEGATION_FLAG);
    اكسر;
#endif
  افتراضي:
    ܦܢܐ CURLE_UNKNOWN_OPTION;
  }
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode setopt_long(بنية Curl_easy *data, CURLoption option,
                            long arg)
{
  تعريفنوع CURLcode (*setoptfunc)(بنية Curl_easy *data,
                                 CURLoption option, long arg);
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
    CURLcode result = setopt_call[i](data, option, arg);
    ܐܢ(result != CURLE_UNKNOWN_OPTION)
      ܦܢܐ result;
  }
  ܦܢܐ CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode setopt_slist(بنية Curl_easy *data, CURLoption option,
                             بنية curl_slist *slist)
{
  CURLcode result = CURLE_OK;
  بنية UserDefined *s = &data->set;
  بدل(option) {
#ifndef CURL_DISABLE_PROXY
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
#endif
#ifndef CURL_DISABLE_HTTP
  حالة CURLOPT_HTTP200ALIASES:
    /*
     * Set a list of aliases for HTTP 200 in response header
     */
    s->http200aliases = slist;
    اكسر;
#endif
#ܐܢ !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
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
#endif
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
    data->state.resolve = s->resolve;
    اكسر;
#ܐܢ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MIME)
  حالة CURLOPT_HTTPHEADER:
    /*
     * Set a list with HTTP headers to use (or replace internals with)
     */
    s->headers = slist;
    اكسر;
#endif
#ifndef CURL_DISABLE_TELNET
  حالة CURLOPT_TELNETOPTIONS:
    /*
     * Set a linked list of telnet options
     */
    s->telnet_options = slist;
    اكسر;
#endif
#ifndef CURL_DISABLE_SMTP
  حالة CURLOPT_MAIL_RCPT:
    /* Set the list of mail recipients */
    s->mail_rcpt = slist;
    اكسر;
#endif
  حالة CURLOPT_CONNECT_TO:
    s->connect_to = slist;
    اكسر;
  افتراضي:
    ܦܢܐ CURLE_UNKNOWN_OPTION;
  }
  ܦܢܐ result;
}

#ܐܢ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
ساكن CURLcode setopt_mimepost(بنية Curl_easy *data, curl_mime *mimep)
{
  /*
   * Set to make us do MIME POST
   */
  CURLcode result;
  بنية UserDefined *s = &data->set;
  ܐܢ(!s->mimepostp) {
    s->mimepostp = curlx_malloc(حجم(*s->mimepostp));
    ܐܢ(!s->mimepostp)
      ܦܢܐ CURLE_OUT_OF_MEMORY;
    Curl_mime_initpart(s->mimepostp);
  }

  result = Curl_mime_set_subparts(s->mimepostp, mimep, FALSE);
  ܐܢ(!result) {
    s->method = HTTPREQ_POST_MIME;
    s->opt_no_body = FALSE; /* this is implied */
#ifndef CURL_DISABLE_FORM_API
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = عدم;
#endif
  }
  ܦܢܐ result;
}
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */

ساكن CURLcode setopt_share(بنية Curl_easy *data, بنية Curl_share *set)
{
  CURLcode result;

  ܐܢ(data->conn) {
    /* As this handle already has a connection attached, changing share now
       would be complicated and error-prone */
    infof(data, "Cannot change share object while in use");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
  }
  ܐܚܪܢܐ {
    /* disconnect from old share, if any and possible */
    result = Curl_share_easy_unlink(data);
    ܐܢ(!result && GOOD_SHARE_HANDLE(set))
      /* use new share if it set */
      result = Curl_share_easy_link(data, set);
  }
  ܦܢܐ result;
}

/* assorted pointer type arguments */
ساكن CURLcode setopt_pointers(بنية Curl_easy *data, CURLoption option,
                                va_list param)
{
  CURLcode result = CURLE_OK;
  بنية UserDefined *s = &data->set;
  بدل(option) {
  حالة CURLOPT_CURLU:
    /*
     * pass CURLU to set URL
     */
    Curl_bufref_free(&data->state.url);
    CURL_EASY_STR_CLEAR(data, STRING_SET_URL);
    s->uh = va_arg(param, CURLU *);
    اكسر;
#ifndef CURL_DISABLE_HTTP
#ifndef CURL_DISABLE_FORM_API
  حالة CURLOPT_HTTPPOST:
    /*
     * Set to make us do HTTP POST. Legacy API-style.
     */
    s->httppost = va_arg(param, بنية curl_httppost *);
    s->method = HTTPREQ_POST_FORM;
    s->opt_no_body = FALSE; /* this is implied */
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = عدم;
    اكسر;
#endif /* !CURL_DISABLE_FORM_API */
#endif /* !CURL_DISABLE_HTTP */
#ܐܢ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
  حالة CURLOPT_MIMEPOST:
    result = setopt_mimepost(data, va_arg(param, curl_mime *));
    اكسر;
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */
  حالة CURLOPT_STDERR:
    /*
     * Set to a FILE * that should receive all error writes. This
     * defaults to stderr for normal operations.
     */
    s->err = va_arg(param, FILE *);
    ܐܢ(!s->err)
      s->err = stderr;
    اكسر;
  حالة CURLOPT_SHARE:
    ܦܢܐ setopt_share(data, va_arg(param, بنية Curl_share *));

  افتراضي:
    ܦܢܐ CURLE_UNKNOWN_OPTION;
  }
  ܦܢܐ result;
}

#ifndef CURL_DISABLE_COOKIES
ساكن CURLcode cookielist(بنية Curl_easy *data, ثابت حرف *ptr)
{
  CURLcode result = CURLE_OK;
  ܐܢ(!ptr)
    ܦܢܐ CURLE_OK;

  ܐܢ(curl_strequal(ptr, "ALL")) {
    /* clear all cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearall(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  ܐܚܪܢܐ ܐܢ(curl_strequal(ptr, "SESS")) {
    /* clear session cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearsess(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  ܐܚܪܢܐ ܐܢ(curl_strequal(ptr, "FLUSH")) {
    /* flush cookies to file, takes care of the locking */
    Curl_flush_cookies(data, FALSE);
  }
  ܐܚܪܢܐ ܐܢ(curl_strequal(ptr, "RELOAD")) {
    /* reload cookies from file */
    ܦܢܐ Curl_cookie_loadfiles(data, COOKIE_NOPSL |
                                 (data->set.cookiesession ?
                                  COOKIE_NOSESSION : 0));
  }
  ܐܚܪܢܐ {
    ܐܢ(!data->cookies) {
      /* if cookie engine was not running, activate it */
      data->cookies = Curl_cookie_init();
      ܐܢ(!data->cookies)
        ܦܢܐ CURLE_OUT_OF_MEMORY;
      data->state.cookie_engine = TRUE;
    }

    /* general protection against mistakes and abuse */
    ܐܢ(strlen(ptr) > CURL_MAX_INPUT_LENGTH)
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;

    /* Adding these cookies without the PSL check, because the PSL is not
       initialized until *perform() time, and this might be called before
       that */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    ܐܢ(checkprefix("Set-Cookie:", ptr))
      /* HTTP Header format line */
      result = Curl_cookie_add(data, data->cookies, ptr + 11,
                               عدم, عدم,
                               COOKIE_HTTPHEADER | COOKIE_SECURE |
                               COOKIE_NOPSL);
    ܐܚܪܢܐ
      /* Netscape format line */
      result = Curl_cookie_add(data, data->cookies, ptr, عدم,
                               عدم, COOKIE_SECURE | COOKIE_NOPSL);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  ܦܢܐ result;
}

ساكن CURLcode cookiefile(بنية Curl_easy *data, ثابت حرف *ptr)
{
  /*
   * Set cookie file to read and parse. Can be used multiple times.
   */
  ܐܢ(ptr) {
    بنية curl_slist *cl;
    /* general protection against mistakes and abuse */
    ܐܢ(strlen(ptr) > CURL_MAX_INPUT_LENGTH)
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    /* append the cookie filename to the list of filenames, and deal with
       them later */
    cl = curl_slist_append(data->state.cookielist, ptr);
    ܐܢ(!cl) {
      curl_slist_free_all(data->state.cookielist);
      data->state.cookielist = عدم;
      ܦܢܐ CURLE_OUT_OF_MEMORY;
    }
    data->state.cookielist = cl; /* store the list for later use */
  }
  ܐܚܪܢܐ {
    /* clear the list of cookie files */
    curl_slist_free_all(data->state.cookielist);
    data->state.cookielist = عدم;

    ܐܢ(!data->share || !data->share->cookies) {
      /* throw away all existing cookies if this is not a shared cookie
         container */
      Curl_cookie_clearall(data->cookies);
      Curl_cookie_cleanup(data->cookies);
    }
    /* disable the cookie engine */
    data->cookies = عدم;
  }
  ܦܢܐ CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_PROXY
ساكن CURLcode setproxy(بنية Curl_easy *data, ثابت حرف *proxy)
{
  ثابت حرف *str = CURL_EASY_STR(data, STRING_PROXY);
  ܐܢ(str && proxy &&
     /* there was one set, is this a new one? */
     !strcmp(str, proxy))
    ܦܢܐ CURLE_OK; /* same one as before */

  changeproxy(data);
  ܦܢܐ Curl_setstropt(data, STRING_PROXY, proxy);
}

ساكن CURLcode setopt_cptr_proxy(بنية Curl_easy *data, CURLoption option,
                                  حرف *ptr)
{
  CURLcode result = CURLE_OK;
  بنية UserDefined *s = &data->set;
  بدل(option) {
  حالة CURLOPT_PROXYUSERPWD: {
    /*
     * user:password needed to use the proxy
     */
    حرف *u = عدم;
    حرف *p = عدم;
    result = setstropt_userpwd(ptr, &u, &p);

    /* URL decode the components */
    ܐܢ(!result) {
      حرف *str = عدم;
      CURL_EASY_STR_CLEAR(data, STRING_PROXYUSERNAME);
      CURL_EASY_STR_CLEAR(data, STRING_PROXYPASSWORD);
      ܐܢ(u) {
        result = Curl_urldecode(u, 0, &str, عدم, REJECT_ZERO);
        ܐܢ(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYUSERNAME, str);
      }
      ܐܢ(!result && p) {
        str = عدم;
        result = Curl_urldecode(p, 0, &str, عدم, REJECT_ZERO);
        ܐܢ(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYPASSWORD, str);
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
    ܦܢܐ Curl_setstropt(data, STRING_PROXYUSERNAME, ptr);

  حالة CURLOPT_PROXYPASSWORD:
    /*
     * authentication password to use in the operation
     */
    ܦܢܐ Curl_setstropt(data, STRING_PROXYPASSWORD, ptr);

  حالة CURLOPT_NOPROXY:
    /*
     * proxy exception list
     */
    ܦܢܐ Curl_setstropt(data, STRING_NOPROXY, ptr);
  حالة CURLOPT_PROXY_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use for proxy
     */
    ܦܢܐ Curl_setstropt(data, STRING_CERT_PROXY, ptr);
  حالة CURLOPT_PROXY_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use for proxy
     */
    ܦܢܐ Curl_setstropt(data, STRING_CERT_TYPE_PROXY, ptr);
  حالة CURLOPT_PROXY_SSLKEY:
    /*
     * String that holds filename of the SSL key to use for proxy
     */
    ܦܢܐ Curl_setstropt(data, STRING_KEY_PROXY, ptr);
  حالة CURLOPT_PROXY_KEYPASSWD:
    /*
     * String that holds the SSL private key password for proxy.
     */
    ܦܢܐ Curl_setstropt(data, STRING_KEY_PASSWD_PROXY, ptr);
  حالة CURLOPT_PROXY_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use for proxy
     */
    ܦܢܐ Curl_setstropt(data, STRING_KEY_TYPE_PROXY, ptr);
  حالة CURLOPT_PROXY_SSL_CIPHER_LIST:
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST)) {
      /* set a list of cipher we want to use in the SSL connection for proxy */
      ܦܢܐ Curl_setstropt(data, STRING_SSL_CIPHER_LIST_PROXY, ptr);
    }
    ܐܚܪܢܐ
      ܦܢܐ CURLE_NOT_BUILT_IN;
  حالة CURLOPT_PROXY_TLS13_CIPHERS:
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites for proxy */
      ܦܢܐ Curl_setstropt(data, STRING_SSL_CIPHER13_LIST_PROXY, ptr);
    ܐܚܪܢܐ
      ܦܢܐ CURLE_NOT_BUILT_IN;
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
    ܦܢܐ setproxy(data, ptr);
  حالة CURLOPT_PRE_PROXY:
    /*
     * Set proxy server:port to use as SOCKS proxy.
     *
     * If the proxy is set to "" or NULL we explicitly say that we do not want
     * to use the socks proxy.
     */
    ܦܢܐ Curl_setstropt(data, STRING_PRE_PROXY, ptr);
  حالة CURLOPT_SOCKS5_GSSAPI_SERVICE:
  حالة CURLOPT_PROXY_SERVICE_NAME:
    /*
     * Set proxy authentication service name for Kerberos 5 and SPNEGO
     */
    ܦܢܐ Curl_setstropt(data, STRING_PROXY_SERVICE_NAME, ptr);
  حالة CURLOPT_PROXY_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
#ifdef USE_SSL
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      ܦܢܐ Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY_PROXY, ptr);
#endif
    ܦܢܐ CURLE_NOT_BUILT_IN;

  حالة CURLOPT_HAPROXY_CLIENT_IP:
    /*
     * Set the client IP to send through HAProxy PROXY protocol
     */
    result = Curl_setstropt(data, STRING_HAPROXY_CLIENT_IP, ptr);

    /* enable the HAProxy protocol if an IP is provided */
    s->haproxyprotocol = !!CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
    اكسر;
  حالة CURLOPT_PROXY_CAINFO:
    /*
     * Set CA info SSL connection for proxy. Specify filename of the
     * CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE_PROXY, ptr);
    ܐܢ(!result)
      s->proxy_ssl.custom_cafile = TRUE;
    ܦܢܐ result;
  حالة CURLOPT_PROXY_CRLFILE:
    /*
     * Set CRL file info for SSL connection for proxy. Specify filename of the
     * CRL to check certificates revocation
     */
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      ܦܢܐ Curl_setstropt(data, STRING_SSL_CRLFILE_PROXY, ptr);
    ܦܢܐ CURLE_NOT_BUILT_IN;
  حالة CURLOPT_PROXY_ISSUERCERT:
    /*
     * Set Issuer certificate file to check certificates issuer
     */
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      ܦܢܐ Curl_setstropt(data, STRING_SSL_ISSUERCERT_PROXY, ptr);
    ܦܢܐ CURLE_NOT_BUILT_IN;
  حالة CURLOPT_PROXY_CAPATH:
    /*
     * Set CA path info for SSL connection proxy. Specify directory name of the
     * CA certificates which have been prepared using openssl c_rehash utility.
     */
#ifdef USE_SSL
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH_PROXY, ptr);
      ܐܢ(!result)
        s->proxy_ssl.custom_capath = TRUE;
      ܦܢܐ result;
    }
#endif
    ܦܢܐ CURLE_NOT_BUILT_IN;
  افتراضي:
    ܦܢܐ CURLE_UNKNOWN_OPTION;
  }
  ܦܢܐ result;
}
#endif

#ܐܢ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
/*
 * A string with POST data. Makes curl HTTP POST. Even if it is NULL. If
 * needed, CURLOPT_POSTFIELDSIZE must have been set prior to
 * CURLOPT_COPYPOSTFIELDS and not altered later.
 */
ساكن CURLcode setopt_copypostfields(ثابت حرف *ptr, بنية UserDefined *s)
{
  CURLcode result = CURLE_OK;
  ܐܢ(s->postfieldsize < -1)
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
  ܐܢ(!ptr || s->postfieldsize == -1) {
    ܐܢ(ptr && (strlen(ptr) > CURL_MAX_INPUT_LENGTH))
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_safefree(s->str_copypostfields);
    ܐܢ(ptr) {
      s->str_copypostfields = curlx_strdup(ptr);
      ܐܢ(!s->str_copypostfields)
        ܦܢܐ CURLE_OUT_OF_MEMORY;
    }
  }
  ܐܚܪܢܐ {
    size_t pflen = curlx_sotouz_range(s->postfieldsize, 0, SIZE_MAX);
    ܐܢ(pflen == SIZE_MAX)
      ܦܢܐ CURLE_OUT_OF_MEMORY;
    ܐܚܪܢܐ {
      /* Allocate even when size == 0. This satisfies the need of possible
         later address compare to detect the COPYPOSTFIELDS mode, and to mark
         that postfields is used rather than read function or form data. */
      حرف *p = curlx_memdup0(ptr, pflen);
      ܐܢ(!p)
        ܦܢܐ CURLE_OUT_OF_MEMORY;
      ܐܚܪܢܐ {
        curlx_free(s->str_copypostfields);
        s->str_copypostfields = p;
      }
    }
  }

  s->postfields = s->str_copypostfields;
  s->method = HTTPREQ_POST;
  ܦܢܐ result;
}
#endif

#ifdef USE_ECH
ساكن CURLcode setopt_ech(بنية Curl_easy *data, ثابت حرف *ptr)
{
  بنية UserDefined *s = &data->set;
  CURLcode result = CURLE_OK;

  ܐܢ(!ptr || !strcmp(ptr, "false"))
    s->ssl.ech = CURLECH_DISABLE;
  ܐܚܪܢܐ {
    size_t plen = strlen(ptr);
    ܐܢ(plen > CURL_MAX_INPUT_LENGTH)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ܐܚܪܢܐ {
      ܐܢ(!strcmp(ptr, "grease"))
        s->ssl.ech = CURLECH_GREASE;
      ܐܚܪܢܐ ܐܢ(!strcmp(ptr, "true"))
        s->ssl.ech = CURLECH_ENABLE;
      ܐܚܪܢܐ ܐܢ(!strcmp(ptr, "hard"))
        s->ssl.ech = CURLECH_HARD;
      ܐܚܪܢܐ ܐܢ(plen > 4 && !strncmp(ptr, "ecl:", 4)) {
        ܐܢ(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_CONFIG, ptr + 4);
      }
      ܐܚܪܢܐ ܐܢ(plen > 3 && !strncmp(ptr, "pn:", 3)) {
        ܐܢ(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_PUBLIC, ptr + 3);
      }
      ܐܚܪܢܐ
        result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  ܦܢܐ result;
}
#ܐܚܪܢܐ
#define setopt_ech(x, y) CURLE_NOT_BUILT_IN
#endif

#ܐܢ defined(USE_SSL) || defined(USE_SSH)
/* One of the options is used for both TLS and SSH */
ساكن CURLcode setopt_cptr_ssl(بنية Curl_easy *data, CURLoption option,
                                حرف *ptr)
{
  CURLcode result = CURLE_OK;

  بدل(option) {
  حالة CURLOPT_KEYPASSWD:
    /*
     * String that holds the SSL or SSH private key password.
     */
    result = Curl_setstropt(data, STRING_KEY_PASSWD, ptr);
    اكسر;
#ifdef USE_SSL
  حالة CURLOPT_CAINFO:
    /*
     * Set CA info for SSL connection. Specify filename of the CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE, ptr);
    ܐܢ(!result)
      data->set.ssl.custom_cafile = TRUE;
    ܦܢܐ result;
  حالة CURLOPT_CAPATH:
    /*
     * Set CA path info for SSL connection. Specify directory name of the CA
     * certificates which have been prepared using openssl c_rehash utility.
     */
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH, ptr);
      ܐܢ(!result)
        data->set.ssl.custom_capath = TRUE;
      ܦܢܐ result;
    }
    ܦܢܐ CURLE_NOT_BUILT_IN;
  حالة CURLOPT_CRLFILE:
    /*
     * Set CRL file info for SSL connection. Specify filename of the CRL
     * to check certificates revocation
     */
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      ܦܢܐ Curl_setstropt(data, STRING_SSL_CRLFILE, ptr);
    ܦܢܐ CURLE_NOT_BUILT_IN;
  حالة CURLOPT_SSL_CIPHER_LIST:
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST))
      /* set a list of cipher we want to use in the SSL connection */
      ܦܢܐ Curl_setstropt(data, STRING_SSL_CIPHER_LIST, ptr);
    ܐܚܪܢܐ
      ܦܢܐ CURLE_NOT_BUILT_IN;
  حالة CURLOPT_TLS13_CIPHERS:
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites */
      ܦܢܐ Curl_setstropt(data, STRING_SSL_CIPHER13_LIST, ptr);
    ܐܚܪܢܐ
      ܦܢܐ CURLE_NOT_BUILT_IN;
  حالة CURLOPT_RANDOM_FILE:
    اكسر;
  حالة CURLOPT_EGDSOCKET:
    اكسر;
  حالة CURLOPT_SSL_CTX_DATA:
    /*
     * Set an SSL_CTX callback parameter pointer
     */
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      data->set.ssl_fsslctxp = ptr;
      اكسر;
    }
    ܐܚܪܢܐ
      ܦܢܐ CURLE_NOT_BUILT_IN;
  حالة CURLOPT_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use
     */
    ܦܢܐ Curl_setstropt(data, STRING_CERT, ptr);
  حالة CURLOPT_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use
     */
    ܦܢܐ Curl_setstropt(data, STRING_CERT_TYPE, ptr);
  حالة CURLOPT_SSLKEY:
    /*
     * String that holds filename of the SSL key to use
     */
    ܦܢܐ Curl_setstropt(data, STRING_KEY, ptr);
  حالة CURLOPT_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use
     */
    ܦܢܐ Curl_setstropt(data, STRING_KEY_TYPE, ptr);
  حالة CURLOPT_SSLENGINE:
    /*
     * String that holds the SSL crypto engine.
     */
    ܐܢ(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      ܦܢܐ CURLE_NOT_BUILT_IN;
    result = Curl_setstropt(data, STRING_SSL_ENGINE, ptr);
    اكسر;
  حالة CURLOPT_ISSUERCERT:
    /*
     * Set Issuer certificate file
     * to check certificates issuer
     */
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      ܦܢܐ Curl_setstropt(data, STRING_SSL_ISSUERCERT, ptr);
    ܦܢܐ CURLE_NOT_BUILT_IN;
  حالة CURLOPT_SSL_EC_CURVES:
    /*
     * Set accepted curves in SSL connection setup.
     * Specify colon-delimited list of curve algorithm names.
     */
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_SSL_EC_CURVES))
      ܦܢܐ Curl_setstropt(data, STRING_SSL_EC_CURVES, ptr);
    ܦܢܐ CURLE_NOT_BUILT_IN;
  حالة CURLOPT_SSL_SIGNATURE_ALGORITHMS:
    /*
     * Set accepted signature algorithms.
     * Specify colon-delimited list of signature scheme names.
     */
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_SIGNATURE_ALGORITHMS))
      ܦܢܐ Curl_setstropt(data, STRING_SSL_SIGNATURE_ALGORITHMS, ptr);
    ܦܢܐ CURLE_NOT_BUILT_IN;
  حالة CURLOPT_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      ܦܢܐ Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY, ptr);
    ܦܢܐ CURLE_NOT_BUILT_IN;
  حالة CURLOPT_ECH:
    ܦܢܐ setopt_ech(data, ptr);
#endif
  افتراضي:
    ܦܢܐ CURLE_UNKNOWN_OPTION;
  }
  ܦܢܐ result;
}
#endif

#ܐܢ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
ساكن CURLcode setopt_cptr_http_mqtt(بنية Curl_easy *data,
                                      CURLoption option, حرف *ptr)
{
  CURLcode result = CURLE_OK;
  بنية UserDefined *s = &data->set;

  بدل(option) {
  حالة CURLOPT_COPYPOSTFIELDS:
    ܦܢܐ setopt_copypostfields(ptr, s);

  حالة CURLOPT_POSTFIELDS:
    /*
     * Like above, but use static data instead of copying it.
     */
    s->postfields = ptr;
    /* Release old copied data. */
    curlx_safefree(s->str_copypostfields);
    s->method = HTTPREQ_POST;
    اكسر;

#ifndef CURL_DISABLE_HTTP
  حالة CURLOPT_TRAILERDATA:
    s->trailer_data = ptr;
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
    ܐܢ(ptr && !*ptr) {
      ptr = Curl_get_content_encodings();
      ܐܢ(ptr)
        result = CURL_EASY_STR_SETN(data, STRING_ENCODING, ptr);
      ܐܚܪܢܐ
        result = CURLE_OUT_OF_MEMORY;
      ܦܢܐ result;
    }
    ܦܢܐ Curl_setstropt(data, STRING_ENCODING, ptr);

#ifndef CURL_DISABLE_AWS
  حالة CURLOPT_AWS_SIGV4:
    /*
     * String that is merged to some authentication
     * parameters are used by the algorithm.
     */
    result = Curl_setstropt(data, STRING_AWS_SIGV4, ptr);
    /*
     * Basic has been set by default; it needs to be unset here.
     */
    ܐܢ(CURL_EASY_STR(data, STRING_AWS_SIGV4))
      s->httpauth = CURLAUTH_AWS_SIGV4;
    ܐܚܪܢܐ
      s->httpauth &= ~(uint32_t)CURLAUTH_AWS_SIGV4;
    اكسر;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  حالة CURLOPT_HTTPSIG_KEY:
    result = Curl_setstropt(data, STRING_HTTPSIG_KEY, ptr);
    اكسر;
  حالة CURLOPT_HTTPSIG_KEYID:
    result = Curl_setstropt(data, STRING_HTTPSIG_KEYID, ptr);
    اكسر;
  حالة CURLOPT_HTTPSIG_HEADERS:
    result = Curl_setstropt(data, STRING_HTTPSIG_HEADERS, ptr);
    اكسر;
#endif
  حالة CURLOPT_REFERER: {
    /*
     * String to set in the HTTP Referer: field.
     */
    بنية bufref *oldref = &data->state.referer;
    /* free the old after the storing the new in case the input is actually
       pointing back to this */
    result = Curl_setstropt(data, STRING_SET_REFERER, ptr);
    Curl_bufref_free(oldref);
    اكسر;
  }

  حالة CURLOPT_USERAGENT:
    /*
     * String to use in the HTTP User-Agent field
     */
    ܦܢܐ Curl_setstropt(data, STRING_USERAGENT, ptr);

#ifndef CURL_DISABLE_COOKIES
  حالة CURLOPT_COOKIE:
    /*
     * Cookie string to send to the remote server in the request.
     */
    ܦܢܐ Curl_setstropt(data, STRING_COOKIE, ptr);

  حالة CURLOPT_COOKIEFILE:
    ܦܢܐ cookiefile(data, ptr);

  حالة CURLOPT_COOKIEJAR:
    /*
     * Set cookie filename to dump all cookies to when we are done.
     */
    result = Curl_setstropt(data, STRING_COOKIEJAR, ptr);
    ܐܢ(!result) {
      /*
       * Activate the cookie parser. This may or may not already
       * have been made.
       */
      ܐܢ(!data->cookies)
        data->cookies = Curl_cookie_init();
      ܐܢ(!data->cookies)
        result = CURLE_OUT_OF_MEMORY;
      ܐܚܪܢܐ
        data->state.cookie_engine = TRUE;
    }
    اكسر;

  حالة CURLOPT_COOKIELIST:
    ܦܢܐ cookielist(data, ptr);
#endif /* !CURL_DISABLE_COOKIES */

#endif /* !CURL_DISABLE_HTTP */
  افتراضي:
    ܦܢܐ CURLE_UNKNOWN_OPTION;
  }
  ܦܢܐ result;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_MQTT */

#ifdef USE_SSH
ساكن CURLcode setopt_cptr_ssh(بنية Curl_easy *data, CURLoption option,
                                حرف *ptr)
{
  بنية UserDefined *s = &data->set;
  بدل(option) {
  حالة CURLOPT_SSH_PUBLIC_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa.pub file
     */
    ܦܢܐ Curl_setstropt(data, STRING_SSH_PUBLIC_KEY, ptr);
  حالة CURLOPT_SSH_PRIVATE_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa file
     */
    ܦܢܐ Curl_setstropt(data, STRING_SSH_PRIVATE_KEY, ptr);
  حالة CURLOPT_SSH_KEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_keyfunc_userp = ptr;
    اكسر;
  حالة CURLOPT_SSH_HOST_PUBLIC_KEY_MD5:
    /*
     * Option to allow for the MD5 of the host public key to be checked
     * for validation purposes.
     */
    ܦܢܐ Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_KEY_MD5, ptr);
  حالة CURLOPT_SSH_HOST_PUBLIC_KEY_SHA256:
    /*
     * Option to allow for the SHA256 of the host public key to be checked
     * for validation purposes.
     */
    ܦܢܐ Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_KEY_SHA256, ptr);
  حالة CURLOPT_SSH_KNOWNHOSTS:
    /*
     * Store the filename to read known hosts from.
     */
    ܦܢܐ Curl_setstropt(data, STRING_SSH_KNOWNHOSTS, ptr);
#ifdef USE_LIBSSH2
  حالة CURLOPT_SSH_HOSTKEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_hostkeyfunc_userp = ptr;
    اكسر;
#endif /* USE_LIBSSH2 */
  افتراضي:
    ܦܢܐ CURLE_UNKNOWN_OPTION;
  }
  ܦܢܐ CURLE_OK;
}
#endif /* USE_SSH */

#ifndef CURL_DISABLE_FTP
ساكن CURLcode setopt_cptr_ftp(بنية Curl_easy *data, CURLoption option,
                                حرف *ptr)
{
  CURLcode result = CURLE_OK;
  بنية UserDefined *s = &data->set;
  بدل(option) {
  حالة CURLOPT_FTPPORT:
    /*
     * Use FTP PORT, this also specifies which IP address to use
     */
    result = Curl_setstropt(data, STRING_FTPPORT, ptr);
    s->ftp_use_port = !!CURL_EASY_STR(data, STRING_FTPPORT);
    اكسر;

  حالة CURLOPT_FTP_ACCOUNT:
    ܦܢܐ Curl_setstropt(data, STRING_FTP_ACCOUNT, ptr);

  حالة CURLOPT_FTP_ALTERNATIVE_TO_USER:
    ܦܢܐ Curl_setstropt(data, STRING_FTP_ALTERNATIVE_TO_USER, ptr);

  حالة CURLOPT_KRBLEVEL:
    ܦܢܐ CURLE_NOT_BUILT_IN; /* removed in 8.17.0 */
  حالة CURLOPT_CHUNK_DATA:
    s->wildcardptr = ptr;
    اكسر;
  حالة CURLOPT_FNMATCH_DATA:
    s->fnmatch_data = ptr;
    اكسر;
  افتراضي:
    ܦܢܐ CURLE_UNKNOWN_OPTION;
  }
  ܦܢܐ result;
}
#endif /* !CURL_DISABLE_FTP */

ساكن CURLcode setopt_cptr_net(بنية Curl_easy *data, CURLoption option,
                                حرف *ptr)
{
  بدل(option) {
  حالة CURLOPT_INTERFACE:
    /*
     * Set what interface or address/hostname to bind the socket to when
     * performing an operation and thus what from-IP your connection will use.
     */
    ܦܢܐ setstropt_interface(data, ptr);

#ifdef USE_RESOLV_ARES
  حالة CURLOPT_DNS_SERVERS:
    ܦܢܐ Curl_setstropt(data, STRING_DNS_SERVERS, ptr);

  حالة CURLOPT_DNS_INTERFACE:
    ܦܢܐ Curl_setstropt(data, STRING_DNS_INTERFACE, ptr);

  حالة CURLOPT_DNS_LOCAL_IP4:
    ܦܢܐ Curl_setstropt(data, STRING_DNS_LOCAL_IP4, ptr);

  حالة CURLOPT_DNS_LOCAL_IP6:
    ܦܢܐ Curl_setstropt(data, STRING_DNS_LOCAL_IP6, ptr);
#endif
#ifdef USE_UNIX_SOCKETS
  حالة CURLOPT_UNIX_SOCKET_PATH:
    data->set.abstract_unix_socket = FALSE;
    ܦܢܐ Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);

  حالة CURLOPT_ABSTRACT_UNIX_SOCKET:
    data->set.abstract_unix_socket = TRUE;
    ܦܢܐ Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);
#endif
#ifndef CURL_DISABLE_DOH
  حالة CURLOPT_DOH_URL:
    {
      CURLcode result = Curl_setstropt(data, STRING_DOH, ptr);
      data->set.doh = !!CURL_EASY_STR(data, STRING_DOH);
      ܦܢܐ result;
    }
#endif
  افتراضي:
    ܦܢܐ CURLE_UNKNOWN_OPTION;
  }
}

ساكن CURLcode setopt_cptr_misc(بنية Curl_easy *data, CURLoption option,
                                 حرف *ptr)
{
  CURLcode result = CURLE_OK;
  بنية UserDefined *s = &data->set;

  بدل(option) {
  حالة CURLOPT_REQUEST_TARGET:
    ܦܢܐ Curl_setstropt(data, STRING_TARGET, ptr);
#ifndef CURL_DISABLE_NETRC
  حالة CURLOPT_NETRC_FILE:
    ܦܢܐ Curl_setstropt(data, STRING_NETRC_FILE, ptr);
#endif
  حالة CURLOPT_CUSTOMREQUEST:
    ܦܢܐ Curl_setstropt(data, STRING_CUSTOMREQUEST, ptr);

    /* we do not set s->method = HTTPREQ_CUSTOM; here, we continue as if we
       were using the already set type and this changes the actual request
       keyword */
  حالة CURLOPT_SERVICE_NAME:
    ܦܢܐ Curl_setstropt(data, STRING_SERVICE_NAME, ptr);

  حالة CURLOPT_HEADERDATA:
    s->writeheader = ptr;
    اكسر;
  حالة CURLOPT_READDATA:
    s->in_set = ptr;
    اكسر;
  حالة CURLOPT_WRITEDATA:
    s->out = ptr;
    اكسر;
  حالة CURLOPT_DEBUGDATA:
    s->debugdata = ptr;
    اكسر;
  حالة CURLOPT_PROGRESSDATA:
    s->progress_client = ptr;
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
  حالة CURLOPT_RESOLVER_START_DATA:
    s->resolver_start_client = ptr;
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
    result = Curl_setstropt(data, STRING_SET_URL, ptr);
    Curl_bufref_set(&data->state.url,
                    CURL_EASY_STR(data, STRING_SET_URL), 0, عدم);
    اكسر;

  حالة CURLOPT_USERPWD: {
    حرف *u = عدم, *p = عدم;
    result = setstropt_userpwd(ptr, &u, &p);
    ܐܢ(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_USERNAME, u);
      u = عدم;
    }
    ܐܢ(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_PASSWORD, p);
      p = عدم;
    }
    curlx_free(u);
    curlx_free(p);
    ܦܢܐ result;
  }

  حالة CURLOPT_USERNAME:
    ܦܢܐ Curl_setstropt(data, STRING_USERNAME, ptr);

  حالة CURLOPT_PASSWORD:
    ܦܢܐ Curl_setstropt(data, STRING_PASSWORD, ptr);

  حالة CURLOPT_LOGIN_OPTIONS:
    ܦܢܐ Curl_setstropt(data, STRING_OPTIONS, ptr);

  حالة CURLOPT_XOAUTH2_BEARER:
    ܦܢܐ Curl_setstropt(data, STRING_BEARER, ptr);
  حالة CURLOPT_RANGE:
    ܦܢܐ Curl_setstropt(data, STRING_SET_RANGE, ptr);
  حالة CURLOPT_PRIVATE:
    s->private_data = ptr;
    اكسر;
  حالة CURLOPT_PROTOCOLS_STR:
    ܐܢ(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      ܐܢ(!result)
        s->allowed_protocols = protos;
    }
    ܐܚܪܢܐ
      /* make a NULL argument reset to default */
      s->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
    اكسر;
  حالة CURLOPT_REDIR_PROTOCOLS_STR:
    ܐܢ(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      ܐܢ(!result)
        s->redir_protocols = protos;
    }
    ܐܚܪܢܐ
      /* make a NULL argument reset to default */
      s->redir_protocols = (curl_prot_t)CURLPROTO_REDIR;
    اكسر;
  حالة CURLOPT_DEFAULT_PROTOCOL:
    /* Set the protocol to use when the URL does not include any protocol */
    ܦܢܐ Curl_setstropt(data, STRING_DEFAULT_PROTOCOL, ptr);
#ifndef CURL_DISABLE_SMTP
  حالة CURLOPT_MAIL_FROM:
    /* Set the SMTP mail originator */
    ܦܢܐ Curl_setstropt(data, STRING_MAIL_FROM, ptr);
  حالة CURLOPT_MAIL_AUTH:
    /* Set the SMTP auth originator */
    ܦܢܐ Curl_setstropt(data, STRING_MAIL_AUTH, ptr);
#endif
  حالة CURLOPT_SASL_AUTHZID:
    /* Authorization identity (identity to act as) */
    ܦܢܐ Curl_setstropt(data, STRING_SASL_AUTHZID, ptr);
#ifndef CURL_DISABLE_RTSP
  حالة CURLOPT_RTSP_SESSION_ID:
    Curl_peer_unlink(&data->state.rtsp_session_origin);
    ܦܢܐ Curl_setstropt(data, STRING_RTSP_SESSION_ID, ptr);
  حالة CURLOPT_RTSP_STREAM_URI:
    ܦܢܐ Curl_setstropt(data, STRING_RTSP_STREAM_URI, ptr);
  حالة CURLOPT_RTSP_TRANSPORT:
    ܦܢܐ Curl_setstropt(data, STRING_RTSP_TRANSPORT, ptr);
  حالة CURLOPT_INTERLEAVEDATA:
    s->rtp_out = ptr;
    اكسر;
#endif /* !CURL_DISABLE_RTSP */
  حالة CURLOPT_TLSAUTH_USERNAME:
  حالة CURLOPT_TLSAUTH_PASSWORD:
  حالة CURLOPT_TLSAUTH_TYPE:
  حالة CURLOPT_PROXY_TLSAUTH_USERNAME:
  حالة CURLOPT_PROXY_TLSAUTH_PASSWORD:
  حالة CURLOPT_PROXY_TLSAUTH_TYPE:
    ܦܢܐ CURLE_NOT_BUILT_IN;
#ifndef CURL_DISABLE_HSTS
  حالة CURLOPT_HSTSREADDATA:
    s->hsts_read_userp = ptr;
    اكسر;
  حالة CURLOPT_HSTSWRITEDATA:
    s->hsts_write_userp = ptr;
    اكسر;
  حالة CURLOPT_HSTS: {
    بنية curl_slist *h;
    ܐܢ(!data->hsts) {
      data->hsts = Curl_hsts_init();
      ܐܢ(!data->hsts)
        ܦܢܐ CURLE_OUT_OF_MEMORY;
    }
    ܐܢ(ptr) {
      result = Curl_setstropt(data, STRING_HSTS, ptr);
      ܐܢ(result)
        ܦܢܐ result;
      /* this needs to build a list of filenames to read from, so that it can
         read them later, as we might get a shared HSTS handle to load them
         into */
      h = curl_slist_append(data->state.hstslist, ptr);
      ܐܢ(!h) {
        curl_slist_free_all(data->state.hstslist);
        data->state.hstslist = عدم;
        ܦܢܐ CURLE_OUT_OF_MEMORY;
      }
      data->state.hstslist = h; /* store the list for later use */
    }
    ܐܚܪܢܐ {
      /* clear the list of HSTS files */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = عدم;
      ܐܢ(!data->share || !data->share->hsts)
        /* throw away the HSTS cache unless shared */
        Curl_hsts_cleanup(&data->hsts);
    }
    اكسر;
  }
#endif /* !CURL_DISABLE_HSTS */
#ifndef CURL_DISABLE_ALTSVC
  حالة CURLOPT_ALTSVC:
    ܐܢ(!data->asi) {
      data->asi = Curl_altsvc_init();
      ܐܢ(!data->asi)
        ܦܢܐ CURLE_OUT_OF_MEMORY;
    }
    result = Curl_setstropt(data, STRING_ALTSVC, ptr);
    ܐܢ(result)
      اكسر;
    ܐܢ(ptr)
      ܦܢܐ Curl_altsvc_load(data->asi, ptr);
    اكسر;
#endif /* !CURL_DISABLE_ALTSVC */
  حالة CURLOPT_ECH:
    ܦܢܐ setopt_ech(data, ptr);
  افتراضي:
    ܦܢܐ CURLE_UNKNOWN_OPTION;
  }
  ܦܢܐ result;
}

ساكن CURLcode setopt_cptr(بنية Curl_easy *data, CURLoption option,
                            حرف *ptr)
{
  تعريفنوع CURLcode (*ptrfunc)(بنية Curl_easy *data, CURLoption option,
                              حرف *ptr);
  /* Order by likeliness */
  ساكن ثابت ptrfunc setopt_call[] = {
    setopt_cptr_misc,
#ܐܢ defined(USE_SSL) || defined(USE_SSH)
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
#ܐܢ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
    setopt_cptr_http_mqtt,
#endif
  };
  size_t i;

  لكل(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, ptr);
    ܐܢ(result != CURLE_UNKNOWN_OPTION)
      ܦܢܐ result;
  }
  ܦܢܐ CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode setopt_func(بنية Curl_easy *data, CURLoption option,
                            va_list param)
{
  بنية UserDefined *s = &data->set;
  بدل(option) {
  حالة CURLOPT_PROGRESSFUNCTION:
    /*
     * Progress callback function
     */
    s->fprogress = va_arg(param, curl_progress_callback);
    ܐܢ(s->fprogress)
      data->progress.callback = TRUE; /* no longer internal */
    ܐܚܪܢܐ
      data->progress.callback = FALSE; /* NULL enforces internal */
    اكسر;

  حالة CURLOPT_XFERINFOFUNCTION:
    /*
     * Transfer info callback function
     */
    s->fxferinfo = va_arg(param, curl_xferinfo_callback);
    ܐܢ(s->fxferinfo)
      data->progress.callback = TRUE; /* no longer internal */
    ܐܚܪܢܐ
      data->progress.callback = FALSE; /* NULL enforces internal */

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
    s->fwrite_header = va_arg(param, curl_write_callback);
    اكسر;
  حالة CURLOPT_WRITEFUNCTION:
    /*
     * Set data write callback
     */
    s->fwrite_func = va_arg(param, curl_write_callback);
    ܐܢ(!s->fwrite_func)
#ܐܢ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fwrite_func = (curl_write_callback)fwrite;
#ܐܢ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic pop
#endif
    اكسر;
  حالة CURLOPT_READFUNCTION:
    /*
     * Read data callback
     */
    s->fread_func_set = va_arg(param, curl_read_callback);
    ܐܢ(!s->fread_func_set) {
      s->is_fread_set = 0;
#ܐܢ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fread_func_set = (curl_read_callback)fread;
#ܐܢ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic pop
#endif
    }
    ܐܚܪܢܐ
      s->is_fread_set = 1;
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
#ifdef USE_SSL
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      s->ssl_fsslctx = va_arg(param, curl_ssl_ctx_callback);
      اكسر;
    }
    ܐܚܪܢܐ
#endif
      ܦܢܐ CURLE_NOT_BUILT_IN;

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

  حالة CURLOPT_RESOLVER_START_FUNCTION:
    /*
     * resolver start callback function: called before a new resolver request
     * is started
     */
    s->resolver_start = va_arg(param, curl_resolver_start_callback);
    اكسر;

#ifdef USE_SSH
#ifdef USE_LIBSSH2
  حالة CURLOPT_SSH_HOSTKEYFUNCTION:
    /* the callback to check the hostkey without the knownhost file */
    s->ssh_hostkeyfunc = va_arg(param, curl_sshhostkeycallback);
    اكسر;
#endif

  حالة CURLOPT_SSH_KEYFUNCTION:
    /* setting to NULL is fine since the ssh.c functions themselves will
       then revert to use the internal default */
    s->ssh_keyfunc = va_arg(param, curl_sshkeycallback);
    اكسر;

#endif /* USE_SSH */

#ifndef CURL_DISABLE_RTSP
  حالة CURLOPT_INTERLEAVEFUNCTION:
    /* Set the user defined RTP write function */
    s->fwrite_rtp = va_arg(param, curl_write_callback);
    اكسر;
#endif
#ifndef CURL_DISABLE_FTP
  حالة CURLOPT_CHUNK_BGN_FUNCTION:
    s->chunk_bgn = va_arg(param, curl_chunk_bgn_callback);
    اكسر;
  حالة CURLOPT_CHUNK_END_FUNCTION:
    s->chunk_end = va_arg(param, curl_chunk_end_callback);
    اكسر;
  حالة CURLOPT_FNMATCH_FUNCTION:
    s->fnmatch = va_arg(param, curl_fnmatch_callback);
    اكسر;
#endif
#ifndef CURL_DISABLE_HTTP
  حالة CURLOPT_TRAILERFUNCTION:
    s->trailer_callback = va_arg(param, curl_trailer_callback);
    اكسر;
#endif
#ifndef CURL_DISABLE_HSTS
  حالة CURLOPT_HSTSREADFUNCTION:
    s->hsts_read = va_arg(param, curl_hstsread_callback);
    اكسر;
  حالة CURLOPT_HSTSWRITEFUNCTION:
    s->hsts_write = va_arg(param, curl_hstswrite_callback);
    اكسر;
#endif
  حالة CURLOPT_PREREQFUNCTION:
    s->fprereq = va_arg(param, curl_prereq_callback);
    اكسر;
  افتراضي:
    ܦܢܐ CURLE_UNKNOWN_OPTION;
  }
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode setopt_offt(بنية Curl_easy *data, CURLoption option,
                            curl_off_t offt)
{
  بنية UserDefined *s = &data->set;
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
    ܐܢ(offt < -1)
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;

    ܐܢ(s->postfieldsize < offt && s->str_copypostfields) {
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
    ܐܢ(offt < -1)
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = offt;
    اكسر;
  حالة CURLOPT_MAX_SEND_SPEED_LARGE:
    /*
     * When transfer uploads are faster than CURLOPT_MAX_SEND_SPEED_LARGE
     * bytes per second the transfer is throttled..
     */
    ܐܢ(offt < 0)
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_send_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.ul.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    اكسر;
  حالة CURLOPT_MAX_RECV_SPEED_LARGE:
    /*
     * When receiving data faster than CURLOPT_MAX_RECV_SPEED_LARGE bytes per
     * second the transfer is throttled..
     */
    ܐܢ(offt < 0)
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_recv_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.dl.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    اكسر;
  حالة CURLOPT_RESUME_FROM_LARGE:
    /*
     * Resume transfer at the given file position
     */
    ܐܢ(offt < -1)
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = offt;
    اكسر;
  حالة CURLOPT_MAXFILESIZE_LARGE:
    /*
     * Set the maximum size of a file to download.
     */
    ܐܢ(offt < 0)
      ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_filesize = offt;
    اكسر;

  افتراضي:
    ܦܢܐ CURLE_UNKNOWN_OPTION;
  }
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode setopt_blob(بنية Curl_easy *data, CURLoption option,
                            بنية curl_blob *blob)
{
  بنية UserDefined *s = &data->set;
  بدل(option) {
  حالة CURLOPT_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use
     */
    ܦܢܐ Curl_setblobopt(&s->blobs[BLOB_CERT], blob);
#ifndef CURL_DISABLE_PROXY
  حالة CURLOPT_PROXY_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use for proxy
     */
    ܦܢܐ Curl_setblobopt(&s->blobs[BLOB_CERT_PROXY], blob);
  حالة CURLOPT_PROXY_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use for proxy
     */
    ܦܢܐ Curl_setblobopt(&s->blobs[BLOB_KEY_PROXY], blob);
  حالة CURLOPT_PROXY_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection proxy.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO_PROXY], blob);
      ܐܢ(!result)
        s->proxy_ssl.custom_cablob = TRUE;
      ܦܢܐ result;
    }
#endif
    ܦܢܐ CURLE_NOT_BUILT_IN;
  حالة CURLOPT_PROXY_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      ܦܢܐ Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT_PROXY], blob);
    ܦܢܐ CURLE_NOT_BUILT_IN;
#endif
  حالة CURLOPT_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use
     */
    ܦܢܐ Curl_setblobopt(&s->blobs[BLOB_KEY], blob);
  حالة CURLOPT_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO], blob);
      ܐܢ(!result)
        s->ssl.custom_cablob = TRUE;
      ܦܢܐ result;
    }
#endif
    ܦܢܐ CURLE_NOT_BUILT_IN;
  حالة CURLOPT_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    ܐܢ(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      ܦܢܐ Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT], blob);
    ܦܢܐ CURLE_NOT_BUILT_IN;

  افتراضي:
    ܦܢܐ CURLE_UNKNOWN_OPTION;
  }
  /* unreachable */
}

/*
 * Do not make Curl_vsetopt() static: it is called from
 * projects/OS400/ccsidcurl.c.
 */
CURLcode Curl_vsetopt(بنية Curl_easy *data, CURLoption option, va_list param)
{
  ܐܢ(option < CURLOPTTYPE_OBJECTPOINT)
    ܦܢܐ setopt_long(data, option, va_arg(param, long));
  ܐܚܪܢܐ ܐܢ(option < CURLOPTTYPE_FUNCTIONPOINT) {
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
    حالة CURLOPT_CONNECT_TO:
      ܦܢܐ setopt_slist(data, option, va_arg(param, بنية curl_slist *));
    حالة CURLOPT_HTTPPOST:         /* curl_httppost * */
    حالة CURLOPT_MIMEPOST:         /* curl_mime * */
    حالة CURLOPT_STDERR:           /* FILE * */
    حالة CURLOPT_SHARE:            /* CURLSH * */
    حالة CURLOPT_CURLU:            /* CURLU * */
      ܦܢܐ setopt_pointers(data, option, param);
    حالة CURLOPT_STREAM_DEPENDS:   /* CURL * */
    حالة CURLOPT_STREAM_DEPENDS_E: /* CURL * */
      ܦܢܐ CURLE_OK;
    افتراضي:
      اكسر;
    }
    /* the char pointer options */
    ܦܢܐ setopt_cptr(data, option, va_arg(param, حرف *));
  }
  ܐܚܪܢܐ ܐܢ(option < CURLOPTTYPE_OFF_T)
    ܦܢܐ setopt_func(data, option, param);
  ܐܚܪܢܐ ܐܢ(option < CURLOPTTYPE_BLOB)
    ܦܢܐ setopt_offt(data, option, va_arg(param, curl_off_t));
  ܦܢܐ setopt_blob(data, option, va_arg(param, بنية curl_blob *));
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
  بنية Curl_eapi_guard guard;
  CURLcode result;

  ܐܢ(CURL_EAPI_ENTER(&guard, curl, easy_setopt, &result)) {
    بنية Curl_easy *data = curl;
    va_list arg;

    va_start(arg, option);

    result = Curl_vsetopt(data, option, arg);

    va_end(arg);
    ܐܢ(result == CURLE_BAD_FUNCTION_ARGUMENT)
      failf(data, "setopt 0x%x got bad argument", (unsigned صحيح)option);
  }
  CURL_EAPI_LEAVE(&guard);
  ܦܢܐ result;
}
