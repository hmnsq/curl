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
#அந்தர்பாவ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#அந்தர்பாவ <linux/tcp.h>
#அல்லதெனில் defined(HAVE_NETINET_TCP_H)
#அந்தர்பாவ <netinet/tcp.h>
#endif

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "content_encoding.h"
#அந்தர்பாவ "strcase.h"
#அந்தர்பாவ "curl_share.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "setopt.h"
#அந்தர்பாவ "altsvc.h"
#அந்தர்பாவ "hsts.h"
#அந்தர்பாவ "tftp.h"
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "escape.h"
#அந்தர்பாவ "bufref.h"
#அந்தர்பாவ "vauth/vauth.h"

நிலையான CURLcode setopt_set_timeout_sec(timediff_t *ptimeout_ms, long secs)
{
  எனில்(secs < 0)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
#எனில் LONG_MAX > (TIMEDIFF_T_MAX / 1000)
  எனில்(secs > (TIMEDIFF_T_MAX / 1000)) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    திரும்பு CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)secs * 1000;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode setopt_set_timeout_ms(timediff_t *ptimeout_ms, long ms)
{
  எனில்(ms < 0)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
#எனில் LONG_MAX > TIMEDIFF_T_MAX
  எனில்(ms > TIMEDIFF_T_MAX) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    திரும்பு CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)ms;
  திரும்பு CURLE_OK;
}

CURLcode Curl_setstropt(கட்டமைப்பு Curl_easy *data,
                        எண்ணிக்கை dupstring id, மாறிலி எழுத்து *s)
{
  size_t slen = s ? strlen(s) : 0;
  DEBUGASSERT((unsigned)id <= UINT8_MAX);
  எனில்(s && (slen > CURL_MAX_உள்ளீடு_LENGTH))
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  திரும்பு CURL_EASY_STR_SET(data, (uint8_t)id, s, slen);
}

CURLcode Curl_setblobopt(கட்டமைப்பு curl_blob **blobp,
                         மாறிலி கட்டமைப்பு curl_blob *blob)
{
  /* free the previous storage at `blobp' and replace by a dynamic storage
     copy of blob. If CURL_BLOB_COPY is set, the data is copied. */

  curlx_safefree(*blobp);

  எனில்(blob) {
    கட்டமைப்பு curl_blob *nblob;
    எனில்(!blob->data || !blob->len || (blob->len > CURL_MAX_உள்ளீடு_LENGTH))
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    nblob = (கட்டமைப்பு curl_blob *)
      curlx_malloc(அளவு(கட்டமைப்பு curl_blob) +
                   ((blob->flags & CURL_BLOB_COPY) ? blob->len : 0));
    எனில்(!nblob)
      திரும்பு CURLE_OUT_OF_MEMORY;
    *nblob = *blob;
    எனில்(blob->flags & CURL_BLOB_COPY) {
      /* put the data after the blob struct in memory */
      nblob->data = (எழுத்து *)nblob + அளவு(கட்டமைப்பு curl_blob);
      memcpy(nblob->data, blob->data, blob->len);
    }

    *blobp = nblob;
    திரும்பு CURLE_OK;
  }

  திரும்பு CURLE_OK;
}

நிலையான CURLcode setstropt_userpwd(மாறிலி எழுத்து *option, எழுத்து **userp,
                                  எழுத்து **passwdp)
{
  எழுத்து *user = வெற்று;
  எழுத்து *passwd = வெற்று;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Parse the login details if specified. If not, then we treat NULL as a
     hint to clear the existing data */
  எனில்(option) {
    size_t len = strlen(option);
    CURLcode result;
    எனில்(len > CURL_MAX_உள்ளீடு_LENGTH)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

    result = Curl_parse_login_details(option, len, &user, &passwd, வெற்று);
    எனில்(result)
      திரும்பு result;
  }

  curlx_free(*userp);
  *userp = user;

  curlx_strzero(*passwdp);
  curlx_free(*passwdp);
  *passwdp = passwd;

  திரும்பு CURLE_OK;
}

நிலையான CURLcode setstropt_interface(கட்டமைப்பு Curl_easy *data, எழுத்து *option)
{
  எனில்(option && !option[0])
    option = வெற்று;
  எனில்(option) {
    /* Parse for checking syntax */
    CURLcode result = Curl_parse_interface(option, வெற்று, வெற்று, வெற்று);
    எனில்(result)
      திரும்பு result;
    திரும்பு CURL_EASY_STR_SET(data, STRING_INTERFACE_OPT,
                             option, strlen(option));
  }
  CURL_EASY_STR_CLEAR(data, STRING_INTERFACE_OPT);
  திரும்பு CURLE_OK;
}

#ifdef USE_SSL
#define C_SSLVERSION_மதிப்பு(x)     ((x) & 0xffff)
#define C_SSLVERSION_MAX_மதிப்பு(x) ((unsigned long)(x) & 0xffff0000)
#endif

நிலையான CURLcode protocol2num(மாறிலி எழுத்து *str, curl_prot_t *val)
{
  /*
   * We are asked to cherry-pick protocols, so play it safe and disallow all
   * protocols to start with, and re-add the wanted ones back in.
   */
  *val = 0;

  எனில்(!str)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  எனில்(curl_strequal(str, "all")) {
    *val = ~(curl_prot_t)0;
    திரும்பு CURLE_OK;
  }

  செய் {
    மாறிலி எழுத்து *token = str;
    size_t tlen;

    str = strchr(str, ',');
    tlen = str ? (size_t)(str - token) : strlen(token);
    எனில்(tlen) {
      மாறிலி கட்டமைப்பு Curl_scheme *h = Curl_getn_scheme(token, tlen);

      எனில்(!h || !h->run)
        திரும்பு CURLE_UNSUPPORTED_PROTOCOL;

      *val |= h->protocol;
    }
  } போது(str && str++);

  எனில்(!*val)
    /* no protocol listed */
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  திரும்பு CURLE_OK;
}

#எனில் !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
நிலையான CURLcode httpauth(கட்டமைப்பு Curl_easy *data, bool proxy,
                         unsigned long auth)
{
  எனில்(auth != CURLAUTH_NONE) {
    முழு bitcheck = 0;
    bool authbits = FALSE;
    எனில்(auth & CURLAUTH_DIGEST_IE) {
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
    போது(bitcheck < 31) {
      எனில்(auth & (1UL << bitcheck++)) {
        authbits = TRUE;
        நிறுத்து;
      }
    }
    எனில்(!authbits)
      திரும்பு CURLE_NOT_BUILT_IN; /* no supported types left! */
  }
  எனில்(proxy)
    data->set.proxyauth = (uint32_t)auth;
  இல்லையெனில்
    data->set.httpauth = (uint32_t)auth;
  திரும்பு CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */

#ifndef CURL_DISABLE_HTTP
நிலையான CURLcode setopt_HTTP_VERSION(கட்டமைப்பு Curl_easy *data, long arg)
{
  /*
   * This sets a requested HTTP version to be used. The value is one of
   * the listed enums in curl/curl.h.
   */
  தேர்வு(arg) {
  நிலை CURL_HTTP_VERSION_NONE:
    /* accepted */
    நிறுத்து;
  நிலை CURL_HTTP_VERSION_1_0:
  நிலை CURL_HTTP_VERSION_1_1:
    /* accepted */
    நிறுத்து;
#ifdef USE_HTTP2
  நிலை CURL_HTTP_VERSION_2_0:
  நிலை CURL_HTTP_VERSION_2TLS:
  நிலை CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    /* accepted */
    நிறுத்து;
#endif
#ifdef USE_HTTP3
  நிலை CURL_HTTP_VERSION_3:
  நிலை CURL_HTTP_VERSION_3ONLY:
    /* accepted */
    நிறுத்து;
#endif
  இயல்பு:
    /* not accepted */
    எனில்(arg < CURL_HTTP_VERSION_NONE)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    திரும்பு CURLE_UNSUPPORTED_PROTOCOL;
  }
  data->set.httpwant = (unsigned எழுத்து)arg;
  திரும்பு CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP */

#ifdef USE_SSL
CURLcode Curl_setopt_SSLVERSION(கட்டமைப்பு Curl_easy *data, CURLoption option,
                                long arg)
{
  /*
   * Set explicit SSL version to try to connect with, as some SSL
   * implementations are lame.
   */
  {
    long version, version_max;
    கட்டமைப்பு ssl_easy_config *sslc = &data->set.ssl;
#ifndef CURL_DISABLE_PROXY
    எனில்(option != CURLOPT_SSLVERSION)
      sslc = &data->set.proxy_ssl;
#இல்லையெனில்
    (வெறுமை)option; /* unused */
#endif
    version = C_SSLVERSION_மதிப்பு(arg);
    version_max = (long)C_SSLVERSION_MAX_மதிப்பு(arg);

    எனில்(version < CURL_SSLVERSION_DEFAULT ||
       version == CURL_SSLVERSION_SSLv2 ||
       version == CURL_SSLVERSION_SSLv3 ||
       version >= CURL_SSLVERSION_LAST ||
       version_max < CURL_SSLVERSION_MAX_NONE ||
       version_max >= CURL_SSLVERSION_MAX_LAST)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    எனில்(version == CURL_SSLVERSION_DEFAULT)
      version = CURL_SSLVERSION_TLSv1_2;

    sslc->version = (unsigned எழுத்து)version;
    sslc->version_max = (unsigned முழு)version_max;
  }
  திரும்பு CURLE_OK;
}
#endif /* !USE_SSL */

#ifndef CURL_DISABLE_RTSP
நிலையான CURLcode setopt_RTSP_REQUEST(கட்டமைப்பு Curl_easy *data, long arg)
{
  /*
   * Set the RTSP request method (OPTIONS, SETUP, PLAY, etc...) Would this be
   * better if the RTSPREQ_* were moved into here?
   */
  எனில்((arg <= CURL_RTSPREQ_NONE) || (arg >= CURL_RTSPREQ_LAST))
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  data->set.rtspreq = (unsigned எழுத்து)arg;
  திரும்பு CURLE_OK;
}
#endif /* !CURL_DISABLE_RTSP */

நிலையான CURLcode setopt_long_bool(கட்டமைப்பு Curl_easy *data, CURLoption option,
                                 long arg)
{
  bool enabled = !!arg;
  முழு ok = 1;
  கட்டமைப்பு UserDefined *s = &data->set;
  தேர்வு(option) {
  நிலை CURLOPT_FORBID_REUSE:
    /*
     * When this transfer is done, it must not be left to be reused by a
     * subsequent transfer but shall be closed immediately.
     */
    s->reuse_forbid = enabled;
    நிறுத்து;
  நிலை CURLOPT_FRESH_CONNECT:
    /*
     * This transfer shall not use a previously cached connection but
     * should be made with a fresh new connect!
     */
    s->reuse_fresh = enabled;
    நிறுத்து;
  நிலை CURLOPT_VERBOSE:
    /*
     * Verbose means infof() calls that give a lot of information about
     * the connection and transfer procedures as well as internal choices.
     */
    s->verbose = enabled;
    நிறுத்து;
  நிலை CURLOPT_HEADER:
    /*
     * Set to include the header in the general data output stream.
     */
    s->include_header = enabled;
    நிறுத்து;
  நிலை CURLOPT_NOPROGRESS:
    /*
     * Shut off the internal supported progress meter
     */
    data->progress.hide = enabled;
    நிறுத்து;
  நிலை CURLOPT_NOBODY:
    /*
     * Do not include the body part in the output data stream.
     */
    s->opt_no_body = enabled;
#ifndef CURL_DISABLE_HTTP
    எனில்(s->opt_no_body)
      /* in HTTP lingo, no body means using the HEAD request... */
      s->method = HTTPREQ_HEAD;
    இல்லையெனில் எனில்(s->method == HTTPREQ_HEAD)
      s->method = HTTPREQ_GET;
#endif
    நிறுத்து;
  நிலை CURLOPT_FAILONERROR:
    /*
     * Do not output the >=400 error code HTML-page, but instead only
     * return error.
     */
    s->http_fail_on_error = enabled;
    நிறுத்து;
  நிலை CURLOPT_KEEP_SENDING_ON_ERROR:
    s->http_keep_sending_on_error = enabled;
    நிறுத்து;
  நிலை CURLOPT_UPLOAD:
  நிலை CURLOPT_PUT:
    /*
     * We want to send data to the remote host. If this is HTTP, that equals
     * using the PUT request.
     */
    எனில்(enabled) {
      /* If this is HTTP, PUT is what's needed to "upload" */
      s->method = HTTPREQ_PUT;
      s->opt_no_body = FALSE; /* this is implied */
    }
    இல்லையெனில்
      /* In HTTP, the opposite of upload is GET (unless NOBODY is true as
         then this can be changed to HEAD later on) */
      s->method = HTTPREQ_GET;
    நிறுத்து;
  நிலை CURLOPT_FILETIME:
    /*
     * Try to get the file time of the remote document. The time will
     * later (possibly) become available using curl_easy_getinfo().
     */
    s->get_filetime = enabled;
    நிறுத்து;
#ifndef CURL_DISABLE_HTTP
  நிலை CURLOPT_HTTP09_ALLOWED:
    s->http09_allowed = enabled;
    நிறுத்து;
#ifndef CURL_DISABLE_COOKIES
  நிலை CURLOPT_COOKIESESSION:
    /*
     * Set this option to TRUE to start a new "cookie session". It will
     * prevent the forthcoming read-cookies-from-file actions to accept
     * cookies that are marked as being session cookies, as they belong to a
     * previous session.
     */
    s->cookiesession = enabled;
    நிறுத்து;
#endif
  நிலை CURLOPT_AUTOREFERER:
    /*
     * Switch on automatic referer that gets set if curl follows locations.
     */
    s->http_auto_referer = enabled;
    நிறுத்து;
  நிலை CURLOPT_TRANSFER_ENCODING:
    s->http_transfer_encoding = enabled;
    நிறுத்து;
  நிலை CURLOPT_UNRESTRICTED_AUTH:
    /*
     * Send authentication (user+password) when following locations, even when
     * hostname changed.
     */
    s->allow_auth_to_other_hosts = enabled;
    நிறுத்து;
  நிலை CURLOPT_HTTP_TRANSFER_DECODING:
    /*
     * disable libcurl transfer encoding is used
     */
    s->http_te_skip = !enabled; /* reversed */
    நிறுத்து;
  நிலை CURLOPT_HTTP_CONTENT_DECODING:
    /*
     * raw data passed to the application when content encoding is used
     */
    s->http_ce_skip = !enabled; /* reversed */
    நிறுத்து;
  நிலை CURLOPT_HTTPGET:
    /*
     * Set to force us do HTTP GET
     */
    எனில்(enabled) {
      s->method = HTTPREQ_GET;
      s->opt_no_body = FALSE; /* this is implied */
    }
    நிறுத்து;
  நிலை CURLOPT_POST:
    /* Does this option serve a purpose anymore? Yes it does, when
       CURLOPT_POSTFIELDS is not used and the POST data is read off the
       callback! */
    எனில்(enabled) {
      s->method = HTTPREQ_POST;
      s->opt_no_body = FALSE; /* this is implied */
    }
    இல்லையெனில்
      s->method = HTTPREQ_GET;
    நிறுத்து;
#endif /* !CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_PROXY
  நிலை CURLOPT_HTTPPROXYTUNNEL:
    /*
     * Tunnel operations through the proxy instead of normal proxy use
     */
    s->tunnel_thru_httpproxy = enabled;
    நிறுத்து;
  நிலை CURLOPT_HAPROXYPROTOCOL:
    /*
     * Set to send the HAProxy Proxy Protocol header
     */
    s->haproxyprotocol = enabled;
    நிறுத்து;
  நிலை CURLOPT_PROXY_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for proxy.
     */
    s->proxy_ssl.verifypeer = enabled;

    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    நிறுத்து;
  நிலை CURLOPT_PROXY_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for proxy
     */
    s->proxy_ssl.verifyhost = enabled;
    ok = 2;
    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    நிறுத்து;
  நிலை CURLOPT_PROXY_TRANSFER_MODE:
    /*
     * set transfer mode (;type=<a|i>) when doing FTP via an HTTP proxy
     */
    s->proxy_transfer_mode = enabled;
    நிறுத்து;
#endif /* !CURL_DISABLE_PROXY */
#எனில் defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  நிலை CURLOPT_SOCKS5_GSSAPI_NEC:
    /*
     * Set flag for NEC SOCKS5 support
     */
    s->socks5_gssapi_nec = enabled;
    நிறுத்து;
#endif
#ifdef CURL_LIST_ONLY_PROTOCOL
  நிலை CURLOPT_DIRLISTONLY:
    /*
     * An option that changes the command to one that asks for a list only, no
     * file info details. Used for FTP, POP3 and SFTP.
     */
    s->list_only = enabled;
    நிறுத்து;
#endif
  நிலை CURLOPT_APPEND:
    /*
     * We want to upload and append to an existing file. Used for FTP and
     * SFTP.
     */
    s->remote_append = enabled;
    நிறுத்து;
#ifndef CURL_DISABLE_FTP
  நிலை CURLOPT_FTP_USE_EPRT:
    s->ftp_use_eprt = enabled;
    நிறுத்து;
  நிலை CURLOPT_FTP_USE_EPSV:
    s->ftp_use_epsv = enabled;
    நிறுத்து;
  நிலை CURLOPT_FTP_USE_PRET:
    s->ftp_use_pret = enabled;
    நிறுத்து;
  நிலை CURLOPT_FTP_SKIP_PASV_IP:
    /*
     * Enable or disable FTP_SKIP_PASV_IP, which will disable/enable the
     * bypass of the IP address in PASV responses.
     */
    s->ftp_skip_ip = enabled;
    நிறுத்து;
  நிலை CURLOPT_WILDCARDMATCH:
    s->wildcard_enabled = enabled;
    நிறுத்து;
#endif
  நிலை CURLOPT_CRLF:
    /*
     * Kludgy option to enable CRLF conversions. Subject for removal.
     */
    s->crlf = enabled;
    நிறுத்து;
#ifndef CURL_DISABLE_TFTP
  நிலை CURLOPT_TFTP_NO_OPTIONS:
    /*
     * Option that prevents libcurl from sending TFTP option requests to the
     * server.
     */
    s->tftp_no_options = enabled;
    நிறுத்து;
#endif /* !CURL_DISABLE_TFTP */
  நிலை CURLOPT_TRANSFERTEXT:
    /*
     * This option was previously named 'FTPASCII'. Renamed to work with
     * more protocols than merely FTP.
     *
     * Transfer using ASCII (instead of BINARY).
     */
    s->prefer_ascii = enabled;
    நிறுத்து;
  நிலை CURLOPT_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying.
     */
    s->ssl.verifypeer = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    நிறுத்து;
#ifndef CURL_DISABLE_DOH
  நிலை CURLOPT_DOH_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for DoH.
     */
    s->doh_verifypeer = enabled;
    நிறுத்து;
  நிலை CURLOPT_DOH_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for DoH
     */
    s->doh_verifyhost = enabled;
    ok = 2;
    நிறுத்து;
  நிலை CURLOPT_DOH_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying for DoH.
     */
    எனில்(!Curl_ssl_cert_status_request())
      திரும்பு CURLE_NOT_BUILT_IN;

    s->doh_verifystatus = enabled;
    ok = 2;
    நிறுத்து;
#endif /* !CURL_DISABLE_DOH */
  நிலை CURLOPT_SSL_VERIFYHOST:
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
    நிறுத்து;
  நிலை CURLOPT_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying.
     */
    எனில்(!Curl_ssl_cert_status_request())
      திரும்பு CURLE_NOT_BUILT_IN;

    s->ssl.verifystatus = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    நிறுத்து;
  நிலை CURLOPT_CERTINFO:
#ifdef USE_SSL
    எனில்(Curl_ssl_supports(data, SSLSUPP_CERTINFO))
      s->ssl.certinfo = enabled;
    இல்லையெனில்
#endif
      திரும்பு CURLE_NOT_BUILT_IN;
    நிறுத்து;
  நிலை CURLOPT_NOSIGNAL:
    /*
     * The application asks not to set any signal() or alarm() handlers,
     * even when using a timeout.
     */
    s->no_signal = enabled;
    நிறுத்து;
  நிலை CURLOPT_TCP_NODELAY:
    /*
     * Enable or disable TCP_NODELAY, which will disable/enable the Nagle
     * algorithm
     */
    s->tcp_nodelay = enabled;
    நிறுத்து;
  நிலை CURLOPT_IGNORE_CONTENT_LENGTH:
    s->ignorecl = enabled;
    நிறுத்து;
  நிலை CURLOPT_SSL_SESSIONID_CACHE:
    s->ssl.cache_session = enabled;
#ifndef CURL_DISABLE_PROXY
    s->proxy_ssl.cache_session = s->ssl.cache_session;
#endif
    நிறுத்து;
#ifdef USE_SSH
  நிலை CURLOPT_SSH_COMPRESSION:
    s->ssh_compression = enabled;
    நிறுத்து;
#endif /* !USE_SSH */
#ifndef CURL_DISABLE_SMTP
  நிலை CURLOPT_MAIL_RCPT_ALLOWFAILS:
    /* allow RCPT TO command to fail for some recipients */
    s->mail_rcpt_allowfails = enabled;
    நிறுத்து;
#endif /* !CURL_DISABLE_SMTP */
  நிலை CURLOPT_SASL_IR:
    /* Enable/disable SASL initial response */
    s->sasl_ir = enabled;
    நிறுத்து;
  நிலை CURLOPT_TCP_KEEPALIVE:
    s->tcp_keepalive = enabled;
    நிறுத்து;
  நிலை CURLOPT_TCP_FASTOPEN:
#எனில் defined(CONNECT_DATA_IDEMPOTENT) || defined(MSG_FASTOPEN) || \
  defined(TCP_FASTOPEN_CONNECT)
    s->tcp_fastopen = enabled;
    நிறுத்து;
#இல்லையெனில்
    திரும்பு CURLE_NOT_BUILT_IN;
#endif
  நிலை CURLOPT_SSL_ENABLE_ALPN:
    s->ssl_enable_alpn = enabled;
    நிறுத்து;
  நிலை CURLOPT_PATH_AS_IS:
    s->path_as_is = enabled;
    நிறுத்து;
  நிலை CURLOPT_PIPEWAIT:
    s->pipewait = enabled;
    நிறுத்து;
  நிலை CURLOPT_SUPPRESS_CONNECT_HEADERS:
    s->suppress_connect_headers = enabled;
    நிறுத்து;
#ifndef CURL_DISABLE_SHUFFLE_DNS
  நிலை CURLOPT_DNS_SHUFFLE_ADDRESSES:
    s->dns_shuffle_addresses = enabled;
    நிறுத்து;
#endif
  நிலை CURLOPT_DISALLOW_USERNAME_IN_URL:
    s->disallow_username_in_url = enabled;
    நிறுத்து;
  நிலை CURLOPT_QUICK_EXIT:
    s->quick_exit = enabled;
    நிறுத்து;
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }
  எனில்((arg > ok) || (arg < 0))
    /* reserve other values for future use */
    infof(data, "boolean setopt(%d) got unsupported argument %ld,"
          " treated as %d", (முழு)option, arg, enabled);

  திரும்பு CURLE_OK;
}

நிலையான CURLcode மதிப்பு_range(long *மதிப்பு, long below_error, long min, long max)
{
  எனில்(*மதிப்பு < below_error)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  இல்லையெனில் எனில்(*மதிப்பு < min)
    *மதிப்பு = min;
  இல்லையெனில் எனில்(*மதிப்பு > max)
    *மதிப்பு = max;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode setopt_long_net(கட்டமைப்பு Curl_easy *data, CURLoption option,
                                long arg)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு UserDefined *s = &data->set;

  தேர்வு(option) {
  நிலை CURLOPT_DNS_CACHE_TIMEOUT:
    எனில்(arg != -1)
      திரும்பு setopt_set_timeout_sec(&s->dns_cache_timeout_ms, arg);
    s->dns_cache_timeout_ms = -1;
    நிறுத்து;
  நிலை CURLOPT_MAXCONNECTS:
    result = மதிப்பு_range(&arg, 0, 0, INT_MAX);
    எனில்(!result)
      s->maxconnects = arg ? (uint32_t)arg : DEFAULT_CONNCACHE_SIZE;
    நிறுத்து;
  நிலை CURLOPT_SERVER_RESPONSE_TIMEOUT:
    திரும்பு setopt_set_timeout_sec(&s->server_response_timeout, arg);
  நிலை CURLOPT_SERVER_RESPONSE_TIMEOUT_MS:
    திரும்பு setopt_set_timeout_ms(&s->server_response_timeout, arg);
  நிலை CURLOPT_கீழ்_SPEED_LIMIT:
    எனில்(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    இல்லையெனில்
      s->கீழ்_speed_limit = arg;
    நிறுத்து;
  நிலை CURLOPT_கீழ்_SPEED_TIME:
    result = மதிப்பு_range(&arg, 0, 0, USHRT_MAX);
    எனில்(!result)
      s->கீழ்_speed_time = (uint16_t)arg;
    நிறுத்து;
  நிலை CURLOPT_PORT:
    எனில்((arg < 0) || (arg > 65535))
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    s->use_port = (unsigned short)arg;
    நிறுத்து;
  நிலை CURLOPT_TIMEOUT:
    திரும்பு setopt_set_timeout_sec(&s->timeout, arg);
  நிலை CURLOPT_TIMEOUT_MS:
    திரும்பு setopt_set_timeout_ms(&s->timeout, arg);
  நிலை CURLOPT_CONNECTTIMEOUT:
    திரும்பு setopt_set_timeout_sec(&s->connecttimeout, arg);
  நிலை CURLOPT_CONNECTTIMEOUT_MS:
    திரும்பு setopt_set_timeout_ms(&s->connecttimeout, arg);
#ifndef CURL_DISABLE_BINDLOCAL
  நிலை CURLOPT_LOCALPORT:
    எனில்((arg < 0) || (arg > 65535))
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    s->localport = curlx_sltous(arg);
    நிறுத்து;
  நிலை CURLOPT_LOCALPORTRANGE:
    எனில்((arg < 0) || (arg > 65535))
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    s->localportrange = curlx_sltous(arg);
    நிறுத்து;
#endif
  நிலை CURLOPT_BUFFERSIZE:
    result = மதிப்பு_range(&arg, 0, READBUFFER_MIN, READBUFFER_MAX);
    எனில்(!result)
      s->buffer_size = (unsigned முழு)arg;
    நிறுத்து;
  நிலை CURLOPT_UPLOAD_BUFFERSIZE:
    result = மதிப்பு_range(&arg, 0, UPLOADBUFFER_MIN, UPLOADBUFFER_MAX);
    எனில்(!result)
      s->upload_buffer_size = (unsigned முழு)arg;
    நிறுத்து;
  நிலை CURLOPT_MAXFILESIZE:
    எனில்(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    இல்லையெனில்
      s->max_filesize = arg;
    நிறுத்து;
  நிலை CURLOPT_IPRESOLVE:
    எனில்((arg < CURL_IPRESOLVE_WHATEVER) || (arg > CURL_IPRESOLVE_V6))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    இல்லையெனில்
      s->ipver = (unsigned எழுத்து)arg;
    நிறுத்து;
  நிலை CURLOPT_CONNECT_ONLY:
    எனில்(arg < 0 || arg > 2)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    இல்லையெனில் {
      s->connect_only = !!arg;
      s->connect_only_ws = (arg == 2);
    }
    நிறுத்து;
#ifdef USE_IPV6
  நிலை CURLOPT_ADDRESS_SCOPE:
#எனில் SIZEOF_LONG > 4
    எனில்((unsigned long)arg > UINT_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    இல்லையெனில்
#endif
    s->scope_id = (unsigned முழு)arg;
    நிறுத்து;
#endif
  நிலை CURLOPT_TCP_KEEPIDLE:
    result = மதிப்பு_range(&arg, 0, 0, INT_MAX);
    எனில்(!result)
      s->tcp_keepidle = (முழு)arg;
    நிறுத்து;
  நிலை CURLOPT_TCP_KEEPINTVL:
    result = மதிப்பு_range(&arg, 0, 0, INT_MAX);
    எனில்(!result)
      s->tcp_keepintvl = (முழு)arg;
    நிறுத்து;
  நிலை CURLOPT_TCP_KEEPCNT:
    result = மதிப்பு_range(&arg, 0, 0, INT_MAX);
    எனில்(!result)
      s->tcp_keepcnt = (முழு)arg;
    நிறுத்து;
  நிலை CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
    திரும்பு setopt_set_timeout_ms(&s->happy_eyeballs_timeout, arg);
  நிலை CURLOPT_UPKEEP_INTERVAL_MS:
    திரும்பு setopt_set_timeout_ms(&s->upkeep_interval_ms, arg);
  நிலை CURLOPT_MAXAGE_CONN:
    திரும்பு setopt_set_timeout_sec(&s->conn_max_idle_ms, arg);
  நிலை CURLOPT_MAXLIFETIME_CONN:
    திரும்பு setopt_set_timeout_sec(&s->conn_max_age_ms, arg);
  நிலை CURLOPT_DNS_USE_GLOBAL_CACHE:
    /* deprecated */
    நிறுத்து;
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }
  திரும்பு result;
}

நிலையான CURLcode setopt_long_ssl(கட்டமைப்பு Curl_easy *data, CURLoption option,
                                long arg)
{
#ifdef USE_SSL
  CURLcode result = CURLE_OK;
  கட்டமைப்பு UserDefined *s = &data->set;
  தேர்வு(option) {
  நிலை CURLOPT_CA_CACHE_TIMEOUT:
    எனில்(Curl_ssl_supports(data, SSLSUPP_CA_CACHE)) {
      result = மதிப்பு_range(&arg, -1, -1, INT_MAX);
      எனில்(!result)
        s->ssl_ca_cache_timeout = (முழு)arg;
    }
    இல்லையெனில்
      result = CURLE_NOT_BUILT_IN;
    நிறுத்து;
  நிலை CURLOPT_SSLVERSION:
#ifndef CURL_DISABLE_PROXY
  நிலை CURLOPT_PROXY_SSLVERSION:
#endif
    திரும்பு Curl_setopt_SSLVERSION(data, option, arg);
  நிலை CURLOPT_SSL_FALSESTART:
    result = CURLE_NOT_BUILT_IN;
    நிறுத்து;
  நிலை CURLOPT_USE_SSL:
    எனில்((arg < CURLUSESSL_NONE) || (arg >= CURLUSESSL_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    இல்லையெனில்
      s->use_ssl = (unsigned எழுத்து)arg;
    நிறுத்து;
  நிலை CURLOPT_SSL_OPTIONS:
    s->ssl.ssl_options = (unsigned எழுத்து)(arg & 0xff);
    நிறுத்து;
#ifndef CURL_DISABLE_PROXY
  நிலை CURLOPT_PROXY_SSL_OPTIONS:
    s->proxy_ssl.ssl_options = (unsigned எழுத்து)(arg & 0xff);
    நிறுத்து;
#endif
  நிலை CURLOPT_SSL_ENABLE_NPN:
    நிறுத்து;
  நிலை CURLOPT_SSLENGINE_DEFAULT:
#ifdef USE_OPENSSL
    எனில்(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      திரும்பு CURLE_NOT_BUILT_IN;
    data->set.engine_default = (arg == 1);
#இல்லையெனில்
    திரும்பு CURLE_NOT_BUILT_IN;
#endif
    நிறுத்து;
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }
  திரும்பு result;
#இல்லையெனில்  /* USE_SSL */
  (வெறுமை)data;
  (வெறுமை)option;
  (வெறுமை)arg;
  திரும்பு CURLE_UNKNOWN_OPTION;
#endif /* !USE_SSL */
}

#ifndef CURL_DISABLE_PROXY
நிலையான வெறுமை changeproxy(கட்டமைப்பு Curl_easy *data)
{
  Curl_auth_digest_cleanup(&data->state.proxydigest);
  memset(&data->state.authproxy, 0, அளவு(data->state.authproxy));
}

நிலையான CURLcode setopt_long_proxy(கட்டமைப்பு Curl_easy *data, CURLoption option,
                                  long arg)
{
  கட்டமைப்பு UserDefined *s = &data->set;

  தேர்வு(option) {
  நிலை CURLOPT_PROXYPORT:
    எனில்((arg < 0) || (arg > UINT16_MAX))
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    எனில்(arg != s->proxyport)
      changeproxy(data);
    s->proxyport = (uint16_t)arg;
    நிறுத்து;
  நிலை CURLOPT_PROXYAUTH:
    திரும்பு httpauth(data, TRUE, (unsigned long)arg);
  நிலை CURLOPT_PROXYTYPE:
    எனில்((arg < CURLPROXY_HTTP) || (arg > CURLPROXY_HTTPS3))
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef USE_PROXY_HTTP3
    எனில்(arg == CURLPROXY_HTTPS3)
      திரும்பு CURLE_NOT_BUILT_IN;
#endif
    s->proxytype = (unsigned எழுத்து)arg;
    நிறுத்து;
  நிலை CURLOPT_SOCKS5_AUTH: {
    unsigned long auth = (unsigned long)arg;
    எனில்(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI | CURLAUTH_ONLY))
      திரும்பு CURLE_NOT_BUILT_IN;
    எனில்((auth & CURLAUTH_ONLY) && !(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI)))
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    s->socks5auth = (unsigned எழுத்து)(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI));
    s->socks5_auth_only = !!(auth & CURLAUTH_ONLY);
    நிறுத்து;
  }
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }
  திரும்பு CURLE_OK;
}
#இல்லையெனில்
நிலையான CURLcode setopt_long_proxy(கட்டமைப்பு Curl_easy *data, CURLoption option,
                                  long arg)
{
  (வெறுமை)data;
  (வெறுமை)option;
  (வெறுமை)arg;
  திரும்பு CURLE_UNKNOWN_OPTION;
}
#endif

நிலையான CURLcode setopt_long_http(கட்டமைப்பு Curl_easy *data, CURLoption option,
                                 long arg)
{
#ifndef CURL_DISABLE_HTTP
  CURLcode result = CURLE_OK;
  கட்டமைப்பு UserDefined *s = &data->set;

  தேர்வு(option) {
  நிலை CURLOPT_FOLLOWLOCATION:
    எனில்((unsigned long)arg > 3)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    இல்லையெனில்
      s->http_follow_mode = (unsigned எழுத்து)arg;
    நிறுத்து;
  நிலை CURLOPT_MAXREDIRS:
    result = மதிப்பு_range(&arg, -1, -1, 0x7fff);
    எனில்(!result)
      s->maxredirs = (short)arg;
    நிறுத்து;
  நிலை CURLOPT_POSTREDIR:
    எனில்(arg < CURL_REDIR_GET_ALL)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    இல்லையெனில் {
      s->post301 = !!(arg & CURL_REDIR_POST_301);
      s->post302 = !!(arg & CURL_REDIR_POST_302);
      s->post303 = !!(arg & CURL_REDIR_POST_303);
    }
    நிறுத்து;
  நிலை CURLOPT_HEADEROPT:
    s->sep_headers = !!(arg & CURLHEADER_SEPARATE);
    நிறுத்து;
  நிலை CURLOPT_HTTPAUTH:
    திரும்பு httpauth(data, FALSE, (unsigned long)arg);
  நிலை CURLOPT_HTTP_VERSION:
    திரும்பு setopt_HTTP_VERSION(data, arg);
  நிலை CURLOPT_EXPECT_100_TIMEOUT_MS:
    result = மதிப்பு_range(&arg, 0, 0, 0xffff);
    எனில்(!result)
      s->expect_100_timeout = (unsigned short)arg;
    நிறுத்து;
  நிலை CURLOPT_STREAM_WEIGHT:
#எனில் defined(USE_HTTP2) || defined(USE_HTTP3)
    எனில்((arg >= 1) && (arg <= 256))
      s->weight = (முழு)arg;
    நிறுத்து;
#இல்லையெனில்
    result = CURLE_NOT_BUILT_IN;
    நிறுத்து;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  நிலை CURLOPT_HTTPSIG_ALGORITHM:
    எனில்(arg != CURLHTTPSIG_NONE &&
       arg != CURLHTTPSIG_ED25519 &&
       arg != CURLHTTPSIG_HMAC_SHA256)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    s->httpsig_algorithm = (uint8_t)arg;
    எனில்(arg)
      s->httpauth = (uint32_t)CURLAUTH_HTTPSIG;
    இல்லையெனில்
      s->httpauth &= ~(uint32_t)CURLAUTH_HTTPSIG;
    நிறுத்து;
#endif
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }
  திரும்பு result;
#இல்லையெனில்
  (வெறுமை)data;
  (வெறுமை)option;
  (வெறுமை)arg;
  திரும்பு CURLE_UNKNOWN_OPTION;
#endif
}

நிலையான CURLcode setopt_long_proto(கட்டமைப்பு Curl_easy *data, CURLoption option,
                                  long arg)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு UserDefined *s = &data->set;

  தேர்வு(option) {
#ifndef CURL_DISABLE_TFTP
  நிலை CURLOPT_TFTP_BLKSIZE:
    result = மதிப்பு_range(&arg, 0, TFTP_BLKSIZE_MIN, TFTP_BLKSIZE_MAX);
    எனில்(!result)
      s->tftp_blksize = (unsigned short)arg;
    நிறுத்து;
#endif
#ifndef CURL_DISABLE_NETRC
  நிலை CURLOPT_NETRC:
    எனில்((arg < CURL_NETRC_IGNORED) || (arg >= CURL_NETRC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    இல்லையெனில்
      s->use_netrc = (unsigned எழுத்து)arg;
    நிறுத்து;
#endif
#ifndef CURL_DISABLE_FTP
  நிலை CURLOPT_FTP_FILEMETHOD:
    எனில்((arg < CURLFTPMETHOD_DEFAULT) || (arg >= CURLFTPMETHOD_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    இல்லையெனில்
      s->ftp_filemethod = (unsigned எழுத்து)arg;
    நிறுத்து;
  நிலை CURLOPT_FTP_SSL_CCC:
    எனில்((arg < CURLFTPSSL_CCC_NONE) || (arg >= CURLFTPSSL_CCC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    இல்லையெனில்
      s->ftp_ccc = (unsigned எழுத்து)arg;
    நிறுத்து;
  நிலை CURLOPT_FTPSSLAUTH:
    எனில்((arg < CURLFTPAUTH_DEFAULT) || (arg >= CURLFTPAUTH_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    இல்லையெனில்
      s->ftpsslauth = (unsigned எழுத்து)arg;
    நிறுத்து;
  நிலை CURLOPT_ACCEPTTIMEOUT_MS:
    திரும்பு setopt_set_timeout_ms(&s->accepttimeout, arg);
#endif
#எனில் !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  நிலை CURLOPT_FTP_CREATE_MISSING_DIRS:
    எனில்((arg < CURLFTP_CREATE_DIR_NONE) || (arg > CURLFTP_CREATE_DIR_RETRY))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    இல்லையெனில்
      s->ftp_create_missing_dirs = (unsigned எழுத்து)arg;
    நிறுத்து;
  நிலை CURLOPT_NEW_FILE_PERMS:
    எனில்((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    இல்லையெனில்
      s->new_file_perms = (unsigned முழு)arg;
    நிறுத்து;
#endif
#ifndef CURL_DISABLE_RTSP
  நிலை CURLOPT_RTSP_REQUEST:
    திரும்பு setopt_RTSP_REQUEST(data, arg);
  நிலை CURLOPT_RTSP_CLIENT_CSEQ:
    result = மதிப்பு_range(&arg, 0, 0, INT_MAX);
    எனில்(!result)
      data->state.rtsp_next_client_CSeq = (uint32_t)arg;
    நிறுத்து;
  நிலை CURLOPT_RTSP_SERVER_CSEQ:
    result = மதிப்பு_range(&arg, 0, 0, INT_MAX);
    எனில்(!result)
      data->state.rtsp_next_server_CSeq = (uint32_t)arg;
    நிறுத்து;
#endif
#ifdef USE_SSH
  நிலை CURLOPT_SSH_AUTH_TYPES:
    s->ssh_auth_types = (uint32_t)arg;
    நிறுத்து;
  நிலை CURLOPT_NEW_DIRECTORY_PERMS:
    எனில்((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    இல்லையெனில்
      s->new_directory_perms = (unsigned முழு)arg;
    நிறுத்து;
#endif
  நிலை CURLOPT_PROTOCOLS:
    s->allowed_protocols = (curl_prot_t)arg;
    நிறுத்து;
  நிலை CURLOPT_REDIR_PROTOCOLS:
    s->redir_protocols = (curl_prot_t)arg;
    நிறுத்து;
#ifndef CURL_DISABLE_WEBSOCKETS
  நிலை CURLOPT_WS_OPTIONS:
    s->ws_raw_mode = !!(arg & CURLWS_RAW_MODE);
    s->ws_no_auto_pong = !!(arg & CURLWS_NOAUTOPONG);
    நிறுத்து;
#endif
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }
  திரும்பு result;
}

நிலையான CURLcode setopt_long_misc(கட்டமைப்பு Curl_easy *data, CURLoption option,
                                 long arg)
{
  கட்டமைப்பு UserDefined *s = &data->set;

  தேர்வு(option) {
  நிலை CURLOPT_TIMECONDITION:
    எனில்((arg < CURL_TIMECOND_NONE) || (arg >= CURL_TIMECOND_LAST))
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    s->timecondition = (unsigned எழுத்து)arg;
    நிறுத்து;
  நிலை CURLOPT_TIMEVALUE:
    s->timevalue = (time_t)arg;
    நிறுத்து;
  நிலை CURLOPT_POSTFIELDSIZE:
    எனில்(arg < -1)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    எனில்(s->postfieldsize < arg && s->str_copypostfields) {
      curlx_safefree(s->str_copypostfields);
      s->postfields = வெற்று;
    }
    s->postfieldsize = arg;
    நிறுத்து;
  நிலை CURLOPT_INFILESIZE:
    எனில்(arg < -1)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = arg;
    நிறுத்து;
  நிலை CURLOPT_RESUME_FROM:
    எனில்(arg < -1)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = arg;
    நிறுத்து;
  நிலை CURLOPT_UPLOAD_FLAGS:
    s->upload_flags = (unsigned எழுத்து)arg;
    நிறுத்து;
#ifndef CURL_DISABLE_MIME
  நிலை CURLOPT_MIME_OPTIONS:
    s->mime_formescape = !!(arg & CURLMIMEOPT_FORMESCAPE);
    நிறுத்து;
#endif
#ifndef CURL_DISABLE_HSTS
  நிலை CURLOPT_HSTS_CTRL:
    எனில்(arg & CURLHSTS_ENABLE) {
      எனில்(!data->hsts) {
        data->hsts = Curl_hsts_init();
        எனில்(!data->hsts)
          திரும்பு CURLE_OUT_OF_MEMORY;
      }
    }
    இல்லையெனில் எனில்(!data->share || !data->share->hsts) {
      /* throw away the HSTS cache unless shared */
      Curl_hsts_cleanup(&data->hsts);
      /* flush all the entries */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = வெற்று;
    }
    இல்லையெனில்
      /* detach from shared HSTS cache without freeing it */
      data->hsts = வெற்று;
    நிறுத்து;
#endif
#ifndef CURL_DISABLE_ALTSVC
  நிலை CURLOPT_ALTSVC_CTRL:
    திரும்பு Curl_altsvc_ctrl(data, arg);
#endif
#எனில் defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  நிலை CURLOPT_GSSAPI_DELEGATION:
    s->gssapi_delegation = (unsigned எழுத்து)arg &
      (CURLGSSAPI_DELEGATION_POLICY_FLAG | CURLGSSAPI_DELEGATION_FLAG);
    நிறுத்து;
#endif
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode setopt_long(கட்டமைப்பு Curl_easy *data, CURLoption option,
                            long arg)
{
  வகைவரை CURLcode (*setoptfunc)(கட்டமைப்பு Curl_easy *data,
                                 CURLoption option, long arg);
  நிலையான மாறிலி setoptfunc setopt_call[] = {
    setopt_long_bool,
    setopt_long_net,
    setopt_long_http,
    setopt_long_proxy,
    setopt_long_ssl,
    setopt_long_proto,
    setopt_long_misc
  };
  size_t i;

  க்கு(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, arg);
    எனில்(result != CURLE_UNKNOWN_OPTION)
      திரும்பு result;
  }
  திரும்பு CURLE_UNKNOWN_OPTION;
}

நிலையான CURLcode setopt_slist(கட்டமைப்பு Curl_easy *data, CURLoption option,
                             கட்டமைப்பு curl_slist *slist)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு UserDefined *s = &data->set;
  தேர்வு(option) {
#ifndef CURL_DISABLE_PROXY
  நிலை CURLOPT_PROXYHEADER:
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
    நிறுத்து;
#endif
#ifndef CURL_DISABLE_HTTP
  நிலை CURLOPT_HTTP200ALIASES:
    /*
     * Set a list of aliases for HTTP 200 in response header
     */
    s->http200aliases = slist;
    நிறுத்து;
#endif
#எனில் !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  நிலை CURLOPT_POSTQUOTE:
    /*
     * List of RAW FTP commands to use after a transfer
     */
    s->postquote = slist;
    நிறுத்து;
  நிலை CURLOPT_PREQUOTE:
    /*
     * List of RAW FTP commands to use prior to RETR (Wesley Laxton)
     */
    s->prequote = slist;
    நிறுத்து;
  நிலை CURLOPT_QUOTE:
    /*
     * List of RAW FTP commands to use before a transfer
     */
    s->quote = slist;
    நிறுத்து;
#endif
  நிலை CURLOPT_RESOLVE:
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
    நிறுத்து;
#எனில் !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MIME)
  நிலை CURLOPT_HTTPHEADER:
    /*
     * Set a list with HTTP headers to use (or replace internals with)
     */
    s->headers = slist;
    நிறுத்து;
#endif
#ifndef CURL_DISABLE_TELNET
  நிலை CURLOPT_TELNETOPTIONS:
    /*
     * Set a linked list of telnet options
     */
    s->telnet_options = slist;
    நிறுத்து;
#endif
#ifndef CURL_DISABLE_SMTP
  நிலை CURLOPT_MAIL_RCPT:
    /* Set the list of mail recipients */
    s->mail_rcpt = slist;
    நிறுத்து;
#endif
  நிலை CURLOPT_CONNECT_TO:
    s->connect_to = slist;
    நிறுத்து;
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }
  திரும்பு result;
}

#எனில் !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
நிலையான CURLcode setopt_mimepost(கட்டமைப்பு Curl_easy *data, curl_mime *mimep)
{
  /*
   * Set to make us do MIME POST
   */
  CURLcode result;
  கட்டமைப்பு UserDefined *s = &data->set;
  எனில்(!s->mimepostp) {
    s->mimepostp = curlx_malloc(அளவு(*s->mimepostp));
    எனில்(!s->mimepostp)
      திரும்பு CURLE_OUT_OF_MEMORY;
    Curl_mime_initpart(s->mimepostp);
  }

  result = Curl_mime_set_subparts(s->mimepostp, mimep, FALSE);
  எனில்(!result) {
    s->method = HTTPREQ_POST_MIME;
    s->opt_no_body = FALSE; /* this is implied */
#ifndef CURL_DISABLE_FORM_API
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = வெற்று;
#endif
  }
  திரும்பு result;
}
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */

நிலையான CURLcode setopt_share(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு Curl_share *set)
{
  CURLcode result;

  எனில்(data->conn) {
    /* As this handle already has a connection attached, changing share now
       would be complicated and error-prone */
    infof(data, "Cannot change share object while in use");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
  }
  இல்லையெனில் {
    /* disconnect from old share, if any and possible */
    result = Curl_share_easy_unlink(data);
    எனில்(!result && GOOD_SHARE_HANDLE(set))
      /* use new share if it set */
      result = Curl_share_easy_link(data, set);
  }
  திரும்பு result;
}

/* assorted pointer type arguments */
நிலையான CURLcode setopt_pointers(கட்டமைப்பு Curl_easy *data, CURLoption option,
                                va_list param)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு UserDefined *s = &data->set;
  தேர்வு(option) {
  நிலை CURLOPT_CURLU:
    /*
     * pass CURLU to set URL
     */
    Curl_bufref_free(&data->state.url);
    CURL_EASY_STR_CLEAR(data, STRING_SET_URL);
    s->uh = va_arg(param, CURLU *);
    நிறுத்து;
#ifndef CURL_DISABLE_HTTP
#ifndef CURL_DISABLE_FORM_API
  நிலை CURLOPT_HTTPPOST:
    /*
     * Set to make us do HTTP POST. Legacy API-style.
     */
    s->httppost = va_arg(param, கட்டமைப்பு curl_httppost *);
    s->method = HTTPREQ_POST_FORM;
    s->opt_no_body = FALSE; /* this is implied */
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = வெற்று;
    நிறுத்து;
#endif /* !CURL_DISABLE_FORM_API */
#endif /* !CURL_DISABLE_HTTP */
#எனில் !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
  நிலை CURLOPT_MIMEPOST:
    result = setopt_mimepost(data, va_arg(param, curl_mime *));
    நிறுத்து;
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */
  நிலை CURLOPT_STDERR:
    /*
     * Set to a FILE * that should receive all error writes. This
     * defaults to stderr for normal operations.
     */
    s->err = va_arg(param, FILE *);
    எனில்(!s->err)
      s->err = stderr;
    நிறுத்து;
  நிலை CURLOPT_SHARE:
    திரும்பு setopt_share(data, va_arg(param, கட்டமைப்பு Curl_share *));

  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }
  திரும்பு result;
}

#ifndef CURL_DISABLE_COOKIES
நிலையான CURLcode cookielist(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *ptr)
{
  CURLcode result = CURLE_OK;
  எனில்(!ptr)
    திரும்பு CURLE_OK;

  எனில்(curl_strequal(ptr, "ALL")) {
    /* clear all cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearall(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  இல்லையெனில் எனில்(curl_strequal(ptr, "SESS")) {
    /* clear session cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearsess(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  இல்லையெனில் எனில்(curl_strequal(ptr, "FLUSH")) {
    /* flush cookies to file, takes care of the locking */
    Curl_flush_cookies(data, FALSE);
  }
  இல்லையெனில் எனில்(curl_strequal(ptr, "RELOAD")) {
    /* reload cookies from file */
    திரும்பு Curl_cookie_loadfiles(data, COOKIE_NOPSL |
                                 (data->set.cookiesession ?
                                  COOKIE_NOSESSION : 0));
  }
  இல்லையெனில் {
    எனில்(!data->cookies) {
      /* if cookie engine was not running, activate it */
      data->cookies = Curl_cookie_init();
      எனில்(!data->cookies)
        திரும்பு CURLE_OUT_OF_MEMORY;
      data->state.cookie_engine = TRUE;
    }

    /* general protection against mistakes and abuse */
    எனில்(strlen(ptr) > CURL_MAX_உள்ளீடு_LENGTH)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

    /* Adding these cookies without the PSL check, because the PSL is not
       initialized until *perform() time, and this might be called before
       that */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    எனில்(checkprefix("Set-Cookie:", ptr))
      /* HTTP Header format line */
      result = Curl_cookie_add(data, data->cookies, ptr + 11,
                               வெற்று, வெற்று,
                               COOKIE_HTTPHEADER | COOKIE_SECURE |
                               COOKIE_NOPSL);
    இல்லையெனில்
      /* Netscape format line */
      result = Curl_cookie_add(data, data->cookies, ptr, வெற்று,
                               வெற்று, COOKIE_SECURE | COOKIE_NOPSL);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  திரும்பு result;
}

நிலையான CURLcode cookiefile(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *ptr)
{
  /*
   * Set cookie file to read and parse. Can be used multiple times.
   */
  எனில்(ptr) {
    கட்டமைப்பு curl_slist *cl;
    /* general protection against mistakes and abuse */
    எனில்(strlen(ptr) > CURL_MAX_உள்ளீடு_LENGTH)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    /* append the cookie filename to the list of filenames, and deal with
       them later */
    cl = curl_slist_append(data->state.cookielist, ptr);
    எனில்(!cl) {
      curl_slist_free_all(data->state.cookielist);
      data->state.cookielist = வெற்று;
      திரும்பு CURLE_OUT_OF_MEMORY;
    }
    data->state.cookielist = cl; /* store the list for later use */
  }
  இல்லையெனில் {
    /* clear the list of cookie files */
    curl_slist_free_all(data->state.cookielist);
    data->state.cookielist = வெற்று;

    எனில்(!data->share || !data->share->cookies) {
      /* throw away all existing cookies if this is not a shared cookie
         container */
      Curl_cookie_clearall(data->cookies);
      Curl_cookie_cleanup(data->cookies);
    }
    /* disable the cookie engine */
    data->cookies = வெற்று;
  }
  திரும்பு CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_PROXY
நிலையான CURLcode setproxy(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *proxy)
{
  மாறிலி எழுத்து *str = CURL_EASY_STR(data, STRING_PROXY);
  எனில்(str && proxy &&
     /* there was one set, is this a new one? */
     !strcmp(str, proxy))
    திரும்பு CURLE_OK; /* same one as before */

  changeproxy(data);
  திரும்பு Curl_setstropt(data, STRING_PROXY, proxy);
}

நிலையான CURLcode setopt_cptr_proxy(கட்டமைப்பு Curl_easy *data, CURLoption option,
                                  எழுத்து *ptr)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு UserDefined *s = &data->set;
  தேர்வு(option) {
  நிலை CURLOPT_PROXYUSERPWD: {
    /*
     * user:password needed to use the proxy
     */
    எழுத்து *u = வெற்று;
    எழுத்து *p = வெற்று;
    result = setstropt_userpwd(ptr, &u, &p);

    /* URL decode the components */
    எனில்(!result) {
      எழுத்து *str = வெற்று;
      CURL_EASY_STR_CLEAR(data, STRING_PROXYUSERNAME);
      CURL_EASY_STR_CLEAR(data, STRING_PROXYPASSWORD);
      எனில்(u) {
        result = Curl_urldecode(u, 0, &str, வெற்று, REJECT_ZERO);
        எனில்(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYUSERNAME, str);
      }
      எனில்(!result && p) {
        str = வெற்று;
        result = Curl_urldecode(p, 0, &str, வெற்று, REJECT_ZERO);
        எனில்(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYPASSWORD, str);
      }
    }
    curlx_free(u);
    curlx_strzero(p);
    curlx_free(p);
    நிறுத்து;
  }
  நிலை CURLOPT_PROXYUSERNAME:
    /*
     * authentication username to use in the operation
     */
    திரும்பு Curl_setstropt(data, STRING_PROXYUSERNAME, ptr);

  நிலை CURLOPT_PROXYPASSWORD:
    /*
     * authentication password to use in the operation
     */
    திரும்பு Curl_setstropt(data, STRING_PROXYPASSWORD, ptr);

  நிலை CURLOPT_NOPROXY:
    /*
     * proxy exception list
     */
    திரும்பு Curl_setstropt(data, STRING_NOPROXY, ptr);
  நிலை CURLOPT_PROXY_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use for proxy
     */
    திரும்பு Curl_setstropt(data, STRING_CERT_PROXY, ptr);
  நிலை CURLOPT_PROXY_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use for proxy
     */
    திரும்பு Curl_setstropt(data, STRING_CERT_TYPE_PROXY, ptr);
  நிலை CURLOPT_PROXY_SSLKEY:
    /*
     * String that holds filename of the SSL key to use for proxy
     */
    திரும்பு Curl_setstropt(data, STRING_சாவி_PROXY, ptr);
  நிலை CURLOPT_PROXY_KEYPASSWD:
    /*
     * String that holds the SSL private key password for proxy.
     */
    திரும்பு Curl_setstropt(data, STRING_சாவி_PASSWD_PROXY, ptr);
  நிலை CURLOPT_PROXY_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use for proxy
     */
    திரும்பு Curl_setstropt(data, STRING_சாவி_TYPE_PROXY, ptr);
  நிலை CURLOPT_PROXY_SSL_CIPHER_LIST:
    எனில்(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST)) {
      /* set a list of cipher we want to use in the SSL connection for proxy */
      திரும்பு Curl_setstropt(data, STRING_SSL_CIPHER_LIST_PROXY, ptr);
    }
    இல்லையெனில்
      திரும்பு CURLE_NOT_BUILT_IN;
  நிலை CURLOPT_PROXY_TLS13_CIPHERS:
    எனில்(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites for proxy */
      திரும்பு Curl_setstropt(data, STRING_SSL_CIPHER13_LIST_PROXY, ptr);
    இல்லையெனில்
      திரும்பு CURLE_NOT_BUILT_IN;
  நிலை CURLOPT_PROXY:
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
    திரும்பு setproxy(data, ptr);
  நிலை CURLOPT_PRE_PROXY:
    /*
     * Set proxy server:port to use as SOCKS proxy.
     *
     * If the proxy is set to "" or NULL we explicitly say that we do not want
     * to use the socks proxy.
     */
    திரும்பு Curl_setstropt(data, STRING_PRE_PROXY, ptr);
  நிலை CURLOPT_SOCKS5_GSSAPI_SERVICE:
  நிலை CURLOPT_PROXY_SERVICE_NAME:
    /*
     * Set proxy authentication service name for Kerberos 5 and SPNEGO
     */
    திரும்பு Curl_setstropt(data, STRING_PROXY_SERVICE_NAME, ptr);
  நிலை CURLOPT_PROXY_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
#ifdef USE_SSL
    எனில்(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      திரும்பு Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY_PROXY, ptr);
#endif
    திரும்பு CURLE_NOT_BUILT_IN;

  நிலை CURLOPT_HAPROXY_CLIENT_IP:
    /*
     * Set the client IP to send through HAProxy PROXY protocol
     */
    result = Curl_setstropt(data, STRING_HAPROXY_CLIENT_IP, ptr);

    /* enable the HAProxy protocol if an IP is provided */
    s->haproxyprotocol = !!CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
    நிறுத்து;
  நிலை CURLOPT_PROXY_CAINFO:
    /*
     * Set CA info SSL connection for proxy. Specify filename of the
     * CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE_PROXY, ptr);
    எனில்(!result)
      s->proxy_ssl.custom_cafile = TRUE;
    திரும்பு result;
  நிலை CURLOPT_PROXY_CRLFILE:
    /*
     * Set CRL file info for SSL connection for proxy. Specify filename of the
     * CRL to check certificates revocation
     */
    எனில்(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      திரும்பு Curl_setstropt(data, STRING_SSL_CRLFILE_PROXY, ptr);
    திரும்பு CURLE_NOT_BUILT_IN;
  நிலை CURLOPT_PROXY_ISSUERCERT:
    /*
     * Set Issuer certificate file to check certificates issuer
     */
    எனில்(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      திரும்பு Curl_setstropt(data, STRING_SSL_ISSUERCERT_PROXY, ptr);
    திரும்பு CURLE_NOT_BUILT_IN;
  நிலை CURLOPT_PROXY_CAPATH:
    /*
     * Set CA path info for SSL connection proxy. Specify directory name of the
     * CA certificates which have been prepared using openssl c_rehash utility.
     */
#ifdef USE_SSL
    எனில்(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH_PROXY, ptr);
      எனில்(!result)
        s->proxy_ssl.custom_capath = TRUE;
      திரும்பு result;
    }
#endif
    திரும்பு CURLE_NOT_BUILT_IN;
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }
  திரும்பு result;
}
#endif

#எனில் !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
/*
 * A string with POST data. Makes curl HTTP POST. Even if it is NULL. If
 * needed, CURLOPT_POSTFIELDSIZE must have been set prior to
 * CURLOPT_COPYPOSTFIELDS and not altered later.
 */
நிலையான CURLcode setopt_copypostfields(மாறிலி எழுத்து *ptr, கட்டமைப்பு UserDefined *s)
{
  CURLcode result = CURLE_OK;
  எனில்(s->postfieldsize < -1)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  எனில்(!ptr || s->postfieldsize == -1) {
    எனில்(ptr && (strlen(ptr) > CURL_MAX_உள்ளீடு_LENGTH))
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_safefree(s->str_copypostfields);
    எனில்(ptr) {
      s->str_copypostfields = curlx_strdup(ptr);
      எனில்(!s->str_copypostfields)
        திரும்பு CURLE_OUT_OF_MEMORY;
    }
  }
  இல்லையெனில் {
    size_t pflen = curlx_sotouz_range(s->postfieldsize, 0, SIZE_MAX);
    எனில்(pflen == SIZE_MAX)
      திரும்பு CURLE_OUT_OF_MEMORY;
    இல்லையெனில் {
      /* Allocate even when size == 0. This satisfies the need of possible
         later address compare to detect the COPYPOSTFIELDS mode, and to mark
         that postfields is used rather than read function or form data. */
      எழுத்து *p = curlx_memdup0(ptr, pflen);
      எனில்(!p)
        திரும்பு CURLE_OUT_OF_MEMORY;
      இல்லையெனில் {
        curlx_free(s->str_copypostfields);
        s->str_copypostfields = p;
      }
    }
  }

  s->postfields = s->str_copypostfields;
  s->method = HTTPREQ_POST;
  திரும்பு result;
}
#endif

#ifdef USE_ECH
நிலையான CURLcode setopt_ech(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *ptr)
{
  கட்டமைப்பு UserDefined *s = &data->set;
  CURLcode result = CURLE_OK;

  எனில்(!ptr || !strcmp(ptr, "false"))
    s->ssl.ech = CURLECH_DISABLE;
  இல்லையெனில் {
    size_t plen = strlen(ptr);
    எனில்(plen > CURL_MAX_உள்ளீடு_LENGTH)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    இல்லையெனில் {
      எனில்(!strcmp(ptr, "grease"))
        s->ssl.ech = CURLECH_GREASE;
      இல்லையெனில் எனில்(!strcmp(ptr, "true"))
        s->ssl.ech = CURLECH_ENABLE;
      இல்லையெனில் எனில்(!strcmp(ptr, "hard"))
        s->ssl.ech = CURLECH_HARD;
      இல்லையெனில் எனில்(plen > 4 && !strncmp(ptr, "ecl:", 4)) {
        எனில்(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_CONFIG, ptr + 4);
      }
      இல்லையெனில் எனில்(plen > 3 && !strncmp(ptr, "pn:", 3)) {
        எனில்(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_PUBLIC, ptr + 3);
      }
      இல்லையெனில்
        result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  திரும்பு result;
}
#இல்லையெனில்
#define setopt_ech(x, y) CURLE_NOT_BUILT_IN
#endif

#எனில் defined(USE_SSL) || defined(USE_SSH)
/* One of the options is used for both TLS and SSH */
நிலையான CURLcode setopt_cptr_ssl(கட்டமைப்பு Curl_easy *data, CURLoption option,
                                எழுத்து *ptr)
{
  CURLcode result = CURLE_OK;

  தேர்வு(option) {
  நிலை CURLOPT_KEYPASSWD:
    /*
     * String that holds the SSL or SSH private key password.
     */
    result = Curl_setstropt(data, STRING_சாவி_PASSWD, ptr);
    நிறுத்து;
#ifdef USE_SSL
  நிலை CURLOPT_CAINFO:
    /*
     * Set CA info for SSL connection. Specify filename of the CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE, ptr);
    எனில்(!result)
      data->set.ssl.custom_cafile = TRUE;
    திரும்பு result;
  நிலை CURLOPT_CAPATH:
    /*
     * Set CA path info for SSL connection. Specify directory name of the CA
     * certificates which have been prepared using openssl c_rehash utility.
     */
    எனில்(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH, ptr);
      எனில்(!result)
        data->set.ssl.custom_capath = TRUE;
      திரும்பு result;
    }
    திரும்பு CURLE_NOT_BUILT_IN;
  நிலை CURLOPT_CRLFILE:
    /*
     * Set CRL file info for SSL connection. Specify filename of the CRL
     * to check certificates revocation
     */
    எனில்(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      திரும்பு Curl_setstropt(data, STRING_SSL_CRLFILE, ptr);
    திரும்பு CURLE_NOT_BUILT_IN;
  நிலை CURLOPT_SSL_CIPHER_LIST:
    எனில்(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST))
      /* set a list of cipher we want to use in the SSL connection */
      திரும்பு Curl_setstropt(data, STRING_SSL_CIPHER_LIST, ptr);
    இல்லையெனில்
      திரும்பு CURLE_NOT_BUILT_IN;
  நிலை CURLOPT_TLS13_CIPHERS:
    எனில்(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites */
      திரும்பு Curl_setstropt(data, STRING_SSL_CIPHER13_LIST, ptr);
    இல்லையெனில்
      திரும்பு CURLE_NOT_BUILT_IN;
  நிலை CURLOPT_RANDOM_FILE:
    நிறுத்து;
  நிலை CURLOPT_EGDSOCKET:
    நிறுத்து;
  நிலை CURLOPT_SSL_CTX_DATA:
    /*
     * Set an SSL_CTX callback parameter pointer
     */
    எனில்(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      data->set.ssl_fsslctxp = ptr;
      நிறுத்து;
    }
    இல்லையெனில்
      திரும்பு CURLE_NOT_BUILT_IN;
  நிலை CURLOPT_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use
     */
    திரும்பு Curl_setstropt(data, STRING_CERT, ptr);
  நிலை CURLOPT_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use
     */
    திரும்பு Curl_setstropt(data, STRING_CERT_TYPE, ptr);
  நிலை CURLOPT_SSLKEY:
    /*
     * String that holds filename of the SSL key to use
     */
    திரும்பு Curl_setstropt(data, STRING_சாவி, ptr);
  நிலை CURLOPT_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use
     */
    திரும்பு Curl_setstropt(data, STRING_சாவி_TYPE, ptr);
  நிலை CURLOPT_SSLENGINE:
    /*
     * String that holds the SSL crypto engine.
     */
    எனில்(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      திரும்பு CURLE_NOT_BUILT_IN;
    result = Curl_setstropt(data, STRING_SSL_ENGINE, ptr);
    நிறுத்து;
  நிலை CURLOPT_ISSUERCERT:
    /*
     * Set Issuer certificate file
     * to check certificates issuer
     */
    எனில்(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      திரும்பு Curl_setstropt(data, STRING_SSL_ISSUERCERT, ptr);
    திரும்பு CURLE_NOT_BUILT_IN;
  நிலை CURLOPT_SSL_EC_CURVES:
    /*
     * Set accepted curves in SSL connection setup.
     * Specify colon-delimited list of curve algorithm names.
     */
    எனில்(Curl_ssl_supports(data, SSLSUPP_SSL_EC_CURVES))
      திரும்பு Curl_setstropt(data, STRING_SSL_EC_CURVES, ptr);
    திரும்பு CURLE_NOT_BUILT_IN;
  நிலை CURLOPT_SSL_SIGNATURE_ALGORITHMS:
    /*
     * Set accepted signature algorithms.
     * Specify colon-delimited list of signature scheme names.
     */
    எனில்(Curl_ssl_supports(data, SSLSUPP_SIGNATURE_ALGORITHMS))
      திரும்பு Curl_setstropt(data, STRING_SSL_SIGNATURE_ALGORITHMS, ptr);
    திரும்பு CURLE_NOT_BUILT_IN;
  நிலை CURLOPT_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
    எனில்(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      திரும்பு Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY, ptr);
    திரும்பு CURLE_NOT_BUILT_IN;
  நிலை CURLOPT_ECH:
    திரும்பு setopt_ech(data, ptr);
#endif
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }
  திரும்பு result;
}
#endif

#எனில் !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
நிலையான CURLcode setopt_cptr_http_mqtt(கட்டமைப்பு Curl_easy *data,
                                      CURLoption option, எழுத்து *ptr)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு UserDefined *s = &data->set;

  தேர்வு(option) {
  நிலை CURLOPT_COPYPOSTFIELDS:
    திரும்பு setopt_copypostfields(ptr, s);

  நிலை CURLOPT_POSTFIELDS:
    /*
     * Like above, but use static data instead of copying it.
     */
    s->postfields = ptr;
    /* Release old copied data. */
    curlx_safefree(s->str_copypostfields);
    s->method = HTTPREQ_POST;
    நிறுத்து;

#ifndef CURL_DISABLE_HTTP
  நிலை CURLOPT_TRAILERDATA:
    s->trailer_data = ptr;
    நிறுத்து;
  நிலை CURLOPT_ACCEPT_ENCODING:
    /*
     * String to use at the value of Accept-Encoding header.
     *
     * If the encoding is set to "" we use an Accept-Encoding header that
     * encompasses all the encodings we support.
     * If the encoding is set to NULL we do not send an Accept-Encoding header
     * and ignore any received Content-Encoding header.
     *
     */
    எனில்(ptr && !*ptr) {
      ptr = Curl_get_content_encodings();
      எனில்(ptr)
        result = CURL_EASY_STR_SETN(data, STRING_ENCODING, ptr);
      இல்லையெனில்
        result = CURLE_OUT_OF_MEMORY;
      திரும்பு result;
    }
    திரும்பு Curl_setstropt(data, STRING_ENCODING, ptr);

#ifndef CURL_DISABLE_AWS
  நிலை CURLOPT_AWS_SIGV4:
    /*
     * String that is merged to some authentication
     * parameters are used by the algorithm.
     */
    result = Curl_setstropt(data, STRING_AWS_SIGV4, ptr);
    /*
     * Basic has been set by default; it needs to be unset here.
     */
    எனில்(CURL_EASY_STR(data, STRING_AWS_SIGV4))
      s->httpauth = CURLAUTH_AWS_SIGV4;
    இல்லையெனில்
      s->httpauth &= ~(uint32_t)CURLAUTH_AWS_SIGV4;
    நிறுத்து;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  நிலை CURLOPT_HTTPSIG_சாவி:
    result = Curl_setstropt(data, STRING_HTTPSIG_சாவி, ptr);
    நிறுத்து;
  நிலை CURLOPT_HTTPSIG_KEYID:
    result = Curl_setstropt(data, STRING_HTTPSIG_KEYID, ptr);
    நிறுத்து;
  நிலை CURLOPT_HTTPSIG_HEADERS:
    result = Curl_setstropt(data, STRING_HTTPSIG_HEADERS, ptr);
    நிறுத்து;
#endif
  நிலை CURLOPT_REFERER: {
    /*
     * String to set in the HTTP Referer: field.
     */
    கட்டமைப்பு bufref *oldref = &data->state.referer;
    /* free the old after the storing the new in case the input is actually
       pointing back to this */
    result = Curl_setstropt(data, STRING_SET_REFERER, ptr);
    Curl_bufref_free(oldref);
    நிறுத்து;
  }

  நிலை CURLOPT_USERAGENT:
    /*
     * String to use in the HTTP User-Agent field
     */
    திரும்பு Curl_setstropt(data, STRING_USERAGENT, ptr);

#ifndef CURL_DISABLE_COOKIES
  நிலை CURLOPT_COOKIE:
    /*
     * Cookie string to send to the remote server in the request.
     */
    திரும்பு Curl_setstropt(data, STRING_COOKIE, ptr);

  நிலை CURLOPT_COOKIEFILE:
    திரும்பு cookiefile(data, ptr);

  நிலை CURLOPT_COOKIEJAR:
    /*
     * Set cookie filename to dump all cookies to when we are done.
     */
    result = Curl_setstropt(data, STRING_COOKIEJAR, ptr);
    எனில்(!result) {
      /*
       * Activate the cookie parser. This may or may not already
       * have been made.
       */
      எனில்(!data->cookies)
        data->cookies = Curl_cookie_init();
      எனில்(!data->cookies)
        result = CURLE_OUT_OF_MEMORY;
      இல்லையெனில்
        data->state.cookie_engine = TRUE;
    }
    நிறுத்து;

  நிலை CURLOPT_COOKIELIST:
    திரும்பு cookielist(data, ptr);
#endif /* !CURL_DISABLE_COOKIES */

#endif /* !CURL_DISABLE_HTTP */
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }
  திரும்பு result;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_MQTT */

#ifdef USE_SSH
நிலையான CURLcode setopt_cptr_ssh(கட்டமைப்பு Curl_easy *data, CURLoption option,
                                எழுத்து *ptr)
{
  கட்டமைப்பு UserDefined *s = &data->set;
  தேர்வு(option) {
  நிலை CURLOPT_SSH_PUBLIC_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa.pub file
     */
    திரும்பு Curl_setstropt(data, STRING_SSH_PUBLIC_சாவி, ptr);
  நிலை CURLOPT_SSH_PRIVATE_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa file
     */
    திரும்பு Curl_setstropt(data, STRING_SSH_PRIVATE_சாவி, ptr);
  நிலை CURLOPT_SSH_KEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_keyfunc_userp = ptr;
    நிறுத்து;
  நிலை CURLOPT_SSH_HOST_PUBLIC_சாவி_MD5:
    /*
     * Option to allow for the MD5 of the host public key to be checked
     * for validation purposes.
     */
    திரும்பு Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_சாவி_MD5, ptr);
  நிலை CURLOPT_SSH_HOST_PUBLIC_சாவி_SHA256:
    /*
     * Option to allow for the SHA256 of the host public key to be checked
     * for validation purposes.
     */
    திரும்பு Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_சாவி_SHA256, ptr);
  நிலை CURLOPT_SSH_KNOWNHOSTS:
    /*
     * Store the filename to read known hosts from.
     */
    திரும்பு Curl_setstropt(data, STRING_SSH_KNOWNHOSTS, ptr);
#ifdef USE_LIBSSH2
  நிலை CURLOPT_SSH_HOSTKEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_hostkeyfunc_userp = ptr;
    நிறுத்து;
#endif /* USE_LIBSSH2 */
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }
  திரும்பு CURLE_OK;
}
#endif /* USE_SSH */

#ifndef CURL_DISABLE_FTP
நிலையான CURLcode setopt_cptr_ftp(கட்டமைப்பு Curl_easy *data, CURLoption option,
                                எழுத்து *ptr)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு UserDefined *s = &data->set;
  தேர்வு(option) {
  நிலை CURLOPT_FTPPORT:
    /*
     * Use FTP PORT, this also specifies which IP address to use
     */
    result = Curl_setstropt(data, STRING_FTPPORT, ptr);
    s->ftp_use_port = !!CURL_EASY_STR(data, STRING_FTPPORT);
    நிறுத்து;

  நிலை CURLOPT_FTP_ACCOUNT:
    திரும்பு Curl_setstropt(data, STRING_FTP_ACCOUNT, ptr);

  நிலை CURLOPT_FTP_ALTERNATIVE_TO_USER:
    திரும்பு Curl_setstropt(data, STRING_FTP_ALTERNATIVE_TO_USER, ptr);

  நிலை CURLOPT_KRBLEVEL:
    திரும்பு CURLE_NOT_BUILT_IN; /* removed in 8.17.0 */
  நிலை CURLOPT_CHUNK_DATA:
    s->wildcardptr = ptr;
    நிறுத்து;
  நிலை CURLOPT_FNMATCH_DATA:
    s->fnmatch_data = ptr;
    நிறுத்து;
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }
  திரும்பு result;
}
#endif /* !CURL_DISABLE_FTP */

நிலையான CURLcode setopt_cptr_net(கட்டமைப்பு Curl_easy *data, CURLoption option,
                                எழுத்து *ptr)
{
  தேர்வு(option) {
  நிலை CURLOPT_INTERFACE:
    /*
     * Set what interface or address/hostname to bind the socket to when
     * performing an operation and thus what from-IP your connection will use.
     */
    திரும்பு setstropt_interface(data, ptr);

#ifdef USE_RESOLV_ARES
  நிலை CURLOPT_DNS_SERVERS:
    திரும்பு Curl_setstropt(data, STRING_DNS_SERVERS, ptr);

  நிலை CURLOPT_DNS_INTERFACE:
    திரும்பு Curl_setstropt(data, STRING_DNS_INTERFACE, ptr);

  நிலை CURLOPT_DNS_LOCAL_IP4:
    திரும்பு Curl_setstropt(data, STRING_DNS_LOCAL_IP4, ptr);

  நிலை CURLOPT_DNS_LOCAL_IP6:
    திரும்பு Curl_setstropt(data, STRING_DNS_LOCAL_IP6, ptr);
#endif
#ifdef USE_UNIX_SOCKETS
  நிலை CURLOPT_UNIX_SOCKET_PATH:
    data->set.abstract_unix_socket = FALSE;
    திரும்பு Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);

  நிலை CURLOPT_ABSTRACT_UNIX_SOCKET:
    data->set.abstract_unix_socket = TRUE;
    திரும்பு Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);
#endif
#ifndef CURL_DISABLE_DOH
  நிலை CURLOPT_DOH_URL:
    {
      CURLcode result = Curl_setstropt(data, STRING_DOH, ptr);
      data->set.doh = !!CURL_EASY_STR(data, STRING_DOH);
      திரும்பு result;
    }
#endif
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }
}

நிலையான CURLcode setopt_cptr_misc(கட்டமைப்பு Curl_easy *data, CURLoption option,
                                 எழுத்து *ptr)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு UserDefined *s = &data->set;

  தேர்வு(option) {
  நிலை CURLOPT_REQUEST_TARGET:
    திரும்பு Curl_setstropt(data, STRING_TARGET, ptr);
#ifndef CURL_DISABLE_NETRC
  நிலை CURLOPT_NETRC_FILE:
    திரும்பு Curl_setstropt(data, STRING_NETRC_FILE, ptr);
#endif
  நிலை CURLOPT_CUSTOMREQUEST:
    திரும்பு Curl_setstropt(data, STRING_CUSTOMREQUEST, ptr);

    /* we do not set s->method = HTTPREQ_CUSTOM; here, we continue as if we
       were using the already set type and this changes the actual request
       keyword */
  நிலை CURLOPT_SERVICE_NAME:
    திரும்பு Curl_setstropt(data, STRING_SERVICE_NAME, ptr);

  நிலை CURLOPT_HEADERDATA:
    s->writeheader = ptr;
    நிறுத்து;
  நிலை CURLOPT_READDATA:
    s->in_set = ptr;
    நிறுத்து;
  நிலை CURLOPT_WRITEDATA:
    s->out = ptr;
    நிறுத்து;
  நிலை CURLOPT_DEBUGDATA:
    s->debugdata = ptr;
    நிறுத்து;
  நிலை CURLOPT_PROGRESSDATA:
    s->progress_client = ptr;
    நிறுத்து;
  நிலை CURLOPT_SEEKDATA:
    s->seek_client = ptr;
    நிறுத்து;
  நிலை CURLOPT_IOCTLDATA:
    s->ioctl_client = ptr;
    நிறுத்து;
  நிலை CURLOPT_SOCKOPTDATA:
    s->sockopt_client = ptr;
    நிறுத்து;
  நிலை CURLOPT_OPENSOCKETDATA:
    s->opensocket_client = ptr;
    நிறுத்து;
  நிலை CURLOPT_RESOLVER_START_DATA:
    s->resolver_start_client = ptr;
    நிறுத்து;
  நிலை CURLOPT_CLOSESOCKETDATA:
    s->closesocket_client = ptr;
    நிறுத்து;
  நிலை CURLOPT_PREREQDATA:
    s->prereq_userp = ptr;
    நிறுத்து;
  நிலை CURLOPT_ERRORBUFFER:
    s->errorbuffer = ptr;
    நிறுத்து;
  நிலை CURLOPT_URL:
    result = Curl_setstropt(data, STRING_SET_URL, ptr);
    Curl_bufref_set(&data->state.url,
                    CURL_EASY_STR(data, STRING_SET_URL), 0, வெற்று);
    நிறுத்து;

  நிலை CURLOPT_USERPWD: {
    எழுத்து *u = வெற்று, *p = வெற்று;
    result = setstropt_userpwd(ptr, &u, &p);
    எனில்(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_USERNAME, u);
      u = வெற்று;
    }
    எனில்(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_PASSWORD, p);
      p = வெற்று;
    }
    curlx_free(u);
    curlx_free(p);
    திரும்பு result;
  }

  நிலை CURLOPT_USERNAME:
    திரும்பு Curl_setstropt(data, STRING_USERNAME, ptr);

  நிலை CURLOPT_PASSWORD:
    திரும்பு Curl_setstropt(data, STRING_PASSWORD, ptr);

  நிலை CURLOPT_LOGIN_OPTIONS:
    திரும்பு Curl_setstropt(data, STRING_OPTIONS, ptr);

  நிலை CURLOPT_XOAUTH2_BEARER:
    திரும்பு Curl_setstropt(data, STRING_BEARER, ptr);
  நிலை CURLOPT_RANGE:
    திரும்பு Curl_setstropt(data, STRING_SET_RANGE, ptr);
  நிலை CURLOPT_PRIVATE:
    s->private_data = ptr;
    நிறுத்து;
  நிலை CURLOPT_PROTOCOLS_STR:
    எனில்(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      எனில்(!result)
        s->allowed_protocols = protos;
    }
    இல்லையெனில்
      /* make a NULL argument reset to default */
      s->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
    நிறுத்து;
  நிலை CURLOPT_REDIR_PROTOCOLS_STR:
    எனில்(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      எனில்(!result)
        s->redir_protocols = protos;
    }
    இல்லையெனில்
      /* make a NULL argument reset to default */
      s->redir_protocols = (curl_prot_t)CURLPROTO_REDIR;
    நிறுத்து;
  நிலை CURLOPT_DEFAULT_PROTOCOL:
    /* Set the protocol to use when the URL does not include any protocol */
    திரும்பு Curl_setstropt(data, STRING_DEFAULT_PROTOCOL, ptr);
#ifndef CURL_DISABLE_SMTP
  நிலை CURLOPT_MAIL_FROM:
    /* Set the SMTP mail originator */
    திரும்பு Curl_setstropt(data, STRING_MAIL_FROM, ptr);
  நிலை CURLOPT_MAIL_AUTH:
    /* Set the SMTP auth originator */
    திரும்பு Curl_setstropt(data, STRING_MAIL_AUTH, ptr);
#endif
  நிலை CURLOPT_SASL_AUTHZID:
    /* Authorization identity (identity to act as) */
    திரும்பு Curl_setstropt(data, STRING_SASL_AUTHZID, ptr);
#ifndef CURL_DISABLE_RTSP
  நிலை CURLOPT_RTSP_SESSION_ID:
    Curl_peer_unlink(&data->state.rtsp_session_origin);
    திரும்பு Curl_setstropt(data, STRING_RTSP_SESSION_ID, ptr);
  நிலை CURLOPT_RTSP_STREAM_URI:
    திரும்பு Curl_setstropt(data, STRING_RTSP_STREAM_URI, ptr);
  நிலை CURLOPT_RTSP_TRANSPORT:
    திரும்பு Curl_setstropt(data, STRING_RTSP_TRANSPORT, ptr);
  நிலை CURLOPT_INTERLEAVEDATA:
    s->rtp_out = ptr;
    நிறுத்து;
#endif /* !CURL_DISABLE_RTSP */
  நிலை CURLOPT_TLSAUTH_USERNAME:
  நிலை CURLOPT_TLSAUTH_PASSWORD:
  நிலை CURLOPT_TLSAUTH_TYPE:
  நிலை CURLOPT_PROXY_TLSAUTH_USERNAME:
  நிலை CURLOPT_PROXY_TLSAUTH_PASSWORD:
  நிலை CURLOPT_PROXY_TLSAUTH_TYPE:
    திரும்பு CURLE_NOT_BUILT_IN;
#ifndef CURL_DISABLE_HSTS
  நிலை CURLOPT_HSTSREADDATA:
    s->hsts_read_userp = ptr;
    நிறுத்து;
  நிலை CURLOPT_HSTSWRITEDATA:
    s->hsts_write_userp = ptr;
    நிறுத்து;
  நிலை CURLOPT_HSTS: {
    கட்டமைப்பு curl_slist *h;
    எனில்(!data->hsts) {
      data->hsts = Curl_hsts_init();
      எனில்(!data->hsts)
        திரும்பு CURLE_OUT_OF_MEMORY;
    }
    எனில்(ptr) {
      result = Curl_setstropt(data, STRING_HSTS, ptr);
      எனில்(result)
        திரும்பு result;
      /* this needs to build a list of filenames to read from, so that it can
         read them later, as we might get a shared HSTS handle to load them
         into */
      h = curl_slist_append(data->state.hstslist, ptr);
      எனில்(!h) {
        curl_slist_free_all(data->state.hstslist);
        data->state.hstslist = வெற்று;
        திரும்பு CURLE_OUT_OF_MEMORY;
      }
      data->state.hstslist = h; /* store the list for later use */
    }
    இல்லையெனில் {
      /* clear the list of HSTS files */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = வெற்று;
      எனில்(!data->share || !data->share->hsts)
        /* throw away the HSTS cache unless shared */
        Curl_hsts_cleanup(&data->hsts);
    }
    நிறுத்து;
  }
#endif /* !CURL_DISABLE_HSTS */
#ifndef CURL_DISABLE_ALTSVC
  நிலை CURLOPT_ALTSVC:
    எனில்(!data->asi) {
      data->asi = Curl_altsvc_init();
      எனில்(!data->asi)
        திரும்பு CURLE_OUT_OF_MEMORY;
    }
    result = Curl_setstropt(data, STRING_ALTSVC, ptr);
    எனில்(result)
      நிறுத்து;
    எனில்(ptr)
      திரும்பு Curl_altsvc_load(data->asi, ptr);
    நிறுத்து;
#endif /* !CURL_DISABLE_ALTSVC */
  நிலை CURLOPT_ECH:
    திரும்பு setopt_ech(data, ptr);
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }
  திரும்பு result;
}

நிலையான CURLcode setopt_cptr(கட்டமைப்பு Curl_easy *data, CURLoption option,
                            எழுத்து *ptr)
{
  வகைவரை CURLcode (*ptrfunc)(கட்டமைப்பு Curl_easy *data, CURLoption option,
                              எழுத்து *ptr);
  /* Order by likeliness */
  நிலையான மாறிலி ptrfunc setopt_call[] = {
    setopt_cptr_misc,
#எனில் defined(USE_SSL) || defined(USE_SSH)
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
#எனில் !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
    setopt_cptr_http_mqtt,
#endif
  };
  size_t i;

  க்கு(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, ptr);
    எனில்(result != CURLE_UNKNOWN_OPTION)
      திரும்பு result;
  }
  திரும்பு CURLE_UNKNOWN_OPTION;
}

நிலையான CURLcode setopt_func(கட்டமைப்பு Curl_easy *data, CURLoption option,
                            va_list param)
{
  கட்டமைப்பு UserDefined *s = &data->set;
  தேர்வு(option) {
  நிலை CURLOPT_PROGRESSFUNCTION:
    /*
     * Progress callback function
     */
    s->fprogress = va_arg(param, curl_progress_callback);
    எனில்(s->fprogress)
      data->progress.callback = TRUE; /* no longer internal */
    இல்லையெனில்
      data->progress.callback = FALSE; /* NULL enforces internal */
    நிறுத்து;

  நிலை CURLOPT_XFERINFOFUNCTION:
    /*
     * Transfer info callback function
     */
    s->fxferinfo = va_arg(param, curl_xferinfo_callback);
    எனில்(s->fxferinfo)
      data->progress.callback = TRUE; /* no longer internal */
    இல்லையெனில்
      data->progress.callback = FALSE; /* NULL enforces internal */

    நிறுத்து;
  நிலை CURLOPT_DEBUGFUNCTION:
    /*
     * stderr write callback.
     */
    s->fdebug = va_arg(param, curl_debug_callback);
    /*
     * if the callback provided is NULL, it will use the default callback
     */
    நிறுத்து;
  நிலை CURLOPT_HEADERFUNCTION:
    /*
     * Set header write callback
     */
    s->fwrite_header = va_arg(param, curl_write_callback);
    நிறுத்து;
  நிலை CURLOPT_WRITEFUNCTION:
    /*
     * Set data write callback
     */
    s->fwrite_func = va_arg(param, curl_write_callback);
    எனில்(!s->fwrite_func)
#எனில் defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic தள்ளு
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fwrite_func = (curl_write_callback)fwrite;
#எனில் defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic எடு
#endif
    நிறுத்து;
  நிலை CURLOPT_READFUNCTION:
    /*
     * Read data callback
     */
    s->fread_func_set = va_arg(param, curl_read_callback);
    எனில்(!s->fread_func_set) {
      s->is_fread_set = 0;
#எனில் defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic தள்ளு
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fread_func_set = (curl_read_callback)fread;
#எனில் defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic எடு
#endif
    }
    இல்லையெனில்
      s->is_fread_set = 1;
    நிறுத்து;
  நிலை CURLOPT_SEEKFUNCTION:
    /*
     * Seek callback. Might be NULL.
     */
    s->seek_func = va_arg(param, curl_seek_callback);
    நிறுத்து;
  நிலை CURLOPT_IOCTLFUNCTION:
    /*
     * I/O control callback. Might be NULL.
     */
    s->ioctl_func = va_arg(param, curl_ioctl_callback);
    நிறுத்து;
  நிலை CURLOPT_SSL_CTX_FUNCTION:
    /*
     * Set an SSL_CTX callback
     */
#ifdef USE_SSL
    எனில்(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      s->ssl_fsslctx = va_arg(param, curl_ssl_ctx_callback);
      நிறுத்து;
    }
    இல்லையெனில்
#endif
      திரும்பு CURLE_NOT_BUILT_IN;

  நிலை CURLOPT_SOCKOPTFUNCTION:
    /*
     * socket callback function: called after socket() but before connect()
     */
    s->fsockopt = va_arg(param, curl_sockopt_callback);
    நிறுத்து;

  நிலை CURLOPT_OPENSOCKETFUNCTION:
    /*
     * open/create socket callback function: called instead of socket(),
     * before connect()
     */
    s->fopensocket = va_arg(param, curl_opensocket_callback);
    நிறுத்து;

  நிலை CURLOPT_CLOSESOCKETFUNCTION:
    /*
     * close socket callback function: called instead of close()
     * when shutting down a connection
     */
    s->fclosesocket = va_arg(param, curl_closesocket_callback);
    நிறுத்து;

  நிலை CURLOPT_RESOLVER_START_FUNCTION:
    /*
     * resolver start callback function: called before a new resolver request
     * is started
     */
    s->resolver_start = va_arg(param, curl_resolver_start_callback);
    நிறுத்து;

#ifdef USE_SSH
#ifdef USE_LIBSSH2
  நிலை CURLOPT_SSH_HOSTKEYFUNCTION:
    /* the callback to check the hostkey without the knownhost file */
    s->ssh_hostkeyfunc = va_arg(param, curl_sshhostkeycallback);
    நிறுத்து;
#endif

  நிலை CURLOPT_SSH_KEYFUNCTION:
    /* setting to NULL is fine since the ssh.c functions themselves will
       then revert to use the internal default */
    s->ssh_keyfunc = va_arg(param, curl_sshkeycallback);
    நிறுத்து;

#endif /* USE_SSH */

#ifndef CURL_DISABLE_RTSP
  நிலை CURLOPT_INTERLEAVEFUNCTION:
    /* Set the user defined RTP write function */
    s->fwrite_rtp = va_arg(param, curl_write_callback);
    நிறுத்து;
#endif
#ifndef CURL_DISABLE_FTP
  நிலை CURLOPT_CHUNK_BGN_FUNCTION:
    s->chunk_bgn = va_arg(param, curl_chunk_bgn_callback);
    நிறுத்து;
  நிலை CURLOPT_CHUNK_END_FUNCTION:
    s->chunk_end = va_arg(param, curl_chunk_end_callback);
    நிறுத்து;
  நிலை CURLOPT_FNMATCH_FUNCTION:
    s->fnmatch = va_arg(param, curl_fnmatch_callback);
    நிறுத்து;
#endif
#ifndef CURL_DISABLE_HTTP
  நிலை CURLOPT_TRAILERFUNCTION:
    s->trailer_callback = va_arg(param, curl_trailer_callback);
    நிறுத்து;
#endif
#ifndef CURL_DISABLE_HSTS
  நிலை CURLOPT_HSTSREADFUNCTION:
    s->hsts_read = va_arg(param, curl_hstsread_callback);
    நிறுத்து;
  நிலை CURLOPT_HSTSWRITEFUNCTION:
    s->hsts_write = va_arg(param, curl_hstswrite_callback);
    நிறுத்து;
#endif
  நிலை CURLOPT_PREREQFUNCTION:
    s->fprereq = va_arg(param, curl_prereq_callback);
    நிறுத்து;
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode setopt_offt(கட்டமைப்பு Curl_easy *data, CURLoption option,
                            curl_off_t offt)
{
  கட்டமைப்பு UserDefined *s = &data->set;
  தேர்வு(option) {
  நிலை CURLOPT_TIMEVALUE_LARGE:
    /*
     * This is the value to compare with the remote document with the
     * method set with CURLOPT_TIMECONDITION
     */
    s->timevalue = (time_t)offt;
    நிறுத்து;

    /* MQTT "borrows" some of the HTTP options */
  நிலை CURLOPT_POSTFIELDSIZE_LARGE:
    /*
     * The size of the POSTFIELD data to prevent libcurl to do strlen() to
     * figure it out. Enables binary posts.
     */
    எனில்(offt < -1)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

    எனில்(s->postfieldsize < offt && s->str_copypostfields) {
      /* Previous CURLOPT_COPYPOSTFIELDS is no longer valid. */
      curlx_safefree(s->str_copypostfields);
      s->postfields = வெற்று;
    }
    s->postfieldsize = offt;
    நிறுத்து;
  நிலை CURLOPT_INFILESIZE_LARGE:
    /*
     * If known, this should inform curl about the file size of the
     * to-be-uploaded file.
     */
    எனில்(offt < -1)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = offt;
    நிறுத்து;
  நிலை CURLOPT_MAX_SEND_SPEED_LARGE:
    /*
     * When transfer uploads are faster than CURLOPT_MAX_SEND_SPEED_LARGE
     * bytes per second the transfer is throttled..
     */
    எனில்(offt < 0)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_send_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.ul.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    நிறுத்து;
  நிலை CURLOPT_MAX_RECV_SPEED_LARGE:
    /*
     * When receiving data faster than CURLOPT_MAX_RECV_SPEED_LARGE bytes per
     * second the transfer is throttled..
     */
    எனில்(offt < 0)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_recv_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.dl.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    நிறுத்து;
  நிலை CURLOPT_RESUME_FROM_LARGE:
    /*
     * Resume transfer at the given file position
     */
    எனில்(offt < -1)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = offt;
    நிறுத்து;
  நிலை CURLOPT_MAXFILESIZE_LARGE:
    /*
     * Set the maximum size of a file to download.
     */
    எனில்(offt < 0)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_filesize = offt;
    நிறுத்து;

  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode setopt_blob(கட்டமைப்பு Curl_easy *data, CURLoption option,
                            கட்டமைப்பு curl_blob *blob)
{
  கட்டமைப்பு UserDefined *s = &data->set;
  தேர்வு(option) {
  நிலை CURLOPT_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use
     */
    திரும்பு Curl_setblobopt(&s->blobs[BLOB_CERT], blob);
#ifndef CURL_DISABLE_PROXY
  நிலை CURLOPT_PROXY_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use for proxy
     */
    திரும்பு Curl_setblobopt(&s->blobs[BLOB_CERT_PROXY], blob);
  நிலை CURLOPT_PROXY_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use for proxy
     */
    திரும்பு Curl_setblobopt(&s->blobs[BLOB_சாவி_PROXY], blob);
  நிலை CURLOPT_PROXY_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection proxy.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    எனில்(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO_PROXY], blob);
      எனில்(!result)
        s->proxy_ssl.custom_cablob = TRUE;
      திரும்பு result;
    }
#endif
    திரும்பு CURLE_NOT_BUILT_IN;
  நிலை CURLOPT_PROXY_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    எனில்(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      திரும்பு Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT_PROXY], blob);
    திரும்பு CURLE_NOT_BUILT_IN;
#endif
  நிலை CURLOPT_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use
     */
    திரும்பு Curl_setblobopt(&s->blobs[BLOB_சாவி], blob);
  நிலை CURLOPT_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    எனில்(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO], blob);
      எனில்(!result)
        s->ssl.custom_cablob = TRUE;
      திரும்பு result;
    }
#endif
    திரும்பு CURLE_NOT_BUILT_IN;
  நிலை CURLOPT_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    எனில்(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      திரும்பு Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT], blob);
    திரும்பு CURLE_NOT_BUILT_IN;

  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }
  /* unreachable */
}

/*
 * Do not make Curl_vsetopt() static: it is called from
 * projects/OS400/ccsidcurl.c.
 */
CURLcode Curl_vsetopt(கட்டமைப்பு Curl_easy *data, CURLoption option, va_list param)
{
  எனில்(option < CURLOPTTYPE_OBJECTPOINT)
    திரும்பு setopt_long(data, option, va_arg(param, long));
  இல்லையெனில் எனில்(option < CURLOPTTYPE_FUNCTIONPOINT) {
    /* unfortunately, different pointer types cannot be identified any other
       way than being listed explicitly */
    தேர்வு(option) {
    நிலை CURLOPT_HTTPHEADER:
    நிலை CURLOPT_QUOTE:
    நிலை CURLOPT_POSTQUOTE:
    நிலை CURLOPT_TELNETOPTIONS:
    நிலை CURLOPT_PREQUOTE:
    நிலை CURLOPT_HTTP200ALIASES:
    நிலை CURLOPT_MAIL_RCPT:
    நிலை CURLOPT_RESOLVE:
    நிலை CURLOPT_PROXYHEADER:
    நிலை CURLOPT_CONNECT_TO:
      திரும்பு setopt_slist(data, option, va_arg(param, கட்டமைப்பு curl_slist *));
    நிலை CURLOPT_HTTPPOST:         /* curl_httppost * */
    நிலை CURLOPT_MIMEPOST:         /* curl_mime * */
    நிலை CURLOPT_STDERR:           /* FILE * */
    நிலை CURLOPT_SHARE:            /* CURLSH * */
    நிலை CURLOPT_CURLU:            /* CURLU * */
      திரும்பு setopt_pointers(data, option, param);
    நிலை CURLOPT_STREAM_DEPENDS:   /* CURL * */
    நிலை CURLOPT_STREAM_DEPENDS_E: /* CURL * */
      திரும்பு CURLE_OK;
    இயல்பு:
      நிறுத்து;
    }
    /* the char pointer options */
    திரும்பு setopt_cptr(data, option, va_arg(param, எழுத்து *));
  }
  இல்லையெனில் எனில்(option < CURLOPTTYPE_OFF_T)
    திரும்பு setopt_func(data, option, param);
  இல்லையெனில் எனில்(option < CURLOPTTYPE_BLOB)
    திரும்பு setopt_offt(data, option, va_arg(param, curl_off_t));
  திரும்பு setopt_blob(data, option, va_arg(param, கட்டமைப்பு curl_blob *));
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
  கட்டமைப்பு Curl_eapi_guard guard;
  CURLcode result;

  எனில்(CURL_EAPI_ENTER(&guard, curl, easy_setopt, &result)) {
    கட்டமைப்பு Curl_easy *data = curl;
    va_list arg;

    va_start(arg, option);

    result = Curl_vsetopt(data, option, arg);

    va_end(arg);
    எனில்(result == CURLE_BAD_FUNCTION_ARGUMENT)
      failf(data, "setopt 0x%x got bad argument", (unsigned முழு)option);
  }
  CURL_EAPI_LEAVE(&guard);
  திரும்பு result;
}
