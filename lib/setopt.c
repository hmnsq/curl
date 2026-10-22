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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#അന്തര്ഭാവ <linux/tcp.h>
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_NETINET_TCP_H)
#അന്തര്ഭാവ <netinet/tcp.h>
#endif

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "content_encoding.h"
#അന്തര്ഭാവ "strcase.h"
#അന്തര്ഭാവ "curl_share.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "setopt.h"
#അന്തര്ഭാവ "altsvc.h"
#അന്തര്ഭാവ "hsts.h"
#അന്തര്ഭാവ "tftp.h"
#അന്തര്ഭാവ "curlx/strdup.h"
#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "bufref.h"
#അന്തര്ഭാവ "vauth/vauth.h"

സ്ഥിതം CURLcode setopt_set_timeout_sec(timediff_t *ptimeout_ms, long secs)
{
  എങ്കിൽ(secs < 0)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
#എങ്കിൽ LONG_MAX > (TIMEDIFF_T_MAX / 1000)
  എങ്കിൽ(secs > (TIMEDIFF_T_MAX / 1000)) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    തിരികെ CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)secs * 1000;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode setopt_set_timeout_ms(timediff_t *ptimeout_ms, long ms)
{
  എങ്കിൽ(ms < 0)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
#എങ്കിൽ LONG_MAX > TIMEDIFF_T_MAX
  എങ്കിൽ(ms > TIMEDIFF_T_MAX) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    തിരികെ CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)ms;
  തിരികെ CURLE_OK;
}

CURLcode Curl_setstropt(ഘടന Curl_easy *data,
                        എണ്ണൽ dupstring id, സ്ഥിരം അക്ഷരം *s)
{
  size_t slen = s ? strlen(s) : 0;
  DEBUGASSERT((unsigned)id <= UINT8_MAX);
  എങ്കിൽ(s && (slen > CURL_MAX_നിവേശം_LENGTH))
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  തിരികെ CURL_EASY_STR_SET(data, (uint8_t)id, s, slen);
}

CURLcode Curl_setblobopt(ഘടന curl_blob **blobp,
                         സ്ഥിരം ഘടന curl_blob *blob)
{
  /* free the previous storage at `blobp' and replace by a dynamic storage
     copy of blob. If CURL_BLOB_COPY is set, the data is copied. */

  curlx_safefree(*blobp);

  എങ്കിൽ(blob) {
    ഘടന curl_blob *nblob;
    എങ്കിൽ(!blob->data || !blob->len || (blob->len > CURL_MAX_നിവേശം_LENGTH))
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    nblob = (ഘടന curl_blob *)
      curlx_malloc(വലുപ്പം(ഘടന curl_blob) +
                   ((blob->flags & CURL_BLOB_COPY) ? blob->len : 0));
    എങ്കിൽ(!nblob)
      തിരികെ CURLE_OUT_OF_MEMORY;
    *nblob = *blob;
    എങ്കിൽ(blob->flags & CURL_BLOB_COPY) {
      /* put the data after the blob struct in memory */
      nblob->data = (അക്ഷരം *)nblob + വലുപ്പം(ഘടന curl_blob);
      memcpy(nblob->data, blob->data, blob->len);
    }

    *blobp = nblob;
    തിരികെ CURLE_OK;
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode setstropt_userpwd(സ്ഥിരം അക്ഷരം *option, അക്ഷരം **userp,
                                  അക്ഷരം **passwdp)
{
  അക്ഷരം *user = ശൂന്യ;
  അക്ഷരം *passwd = ശൂന്യ;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Parse the login details if specified. If not, then we treat NULL as a
     hint to clear the existing data */
  എങ്കിൽ(option) {
    size_t len = strlen(option);
    CURLcode result;
    എങ്കിൽ(len > CURL_MAX_നിവേശം_LENGTH)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

    result = Curl_parse_login_details(option, len, &user, &passwd, ശൂന്യ);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  curlx_free(*userp);
  *userp = user;

  curlx_strzero(*passwdp);
  curlx_free(*passwdp);
  *passwdp = passwd;

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode setstropt_interface(ഘടന Curl_easy *data, അക്ഷരം *option)
{
  എങ്കിൽ(option && !option[0])
    option = ശൂന്യ;
  എങ്കിൽ(option) {
    /* Parse for checking syntax */
    CURLcode result = Curl_parse_interface(option, ശൂന്യ, ശൂന്യ, ശൂന്യ);
    എങ്കിൽ(result)
      തിരികെ result;
    തിരികെ CURL_EASY_STR_SET(data, STRING_INTERFACE_OPT,
                             option, strlen(option));
  }
  CURL_EASY_STR_CLEAR(data, STRING_INTERFACE_OPT);
  തിരികെ CURLE_OK;
}

#ifdef USE_SSL
#define C_SSLVERSION_മൂല്യം(x)     ((x) & 0xffff)
#define C_SSLVERSION_MAX_മൂല്യം(x) ((unsigned long)(x) & 0xffff0000)
#endif

സ്ഥിതം CURLcode protocol2num(സ്ഥിരം അക്ഷരം *str, curl_prot_t *val)
{
  /*
   * We are asked to cherry-pick protocols, so play it safe and disallow all
   * protocols to start with, and re-add the wanted ones back in.
   */
  *val = 0;

  എങ്കിൽ(!str)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  എങ്കിൽ(curl_strequal(str, "all")) {
    *val = ~(curl_prot_t)0;
    തിരികെ CURLE_OK;
  }

  ചെയ്യുക {
    സ്ഥിരം അക്ഷരം *token = str;
    size_t tlen;

    str = strchr(str, ',');
    tlen = str ? (size_t)(str - token) : strlen(token);
    എങ്കിൽ(tlen) {
      സ്ഥിരം ഘടന Curl_scheme *h = Curl_getn_scheme(token, tlen);

      എങ്കിൽ(!h || !h->run)
        തിരികെ CURLE_UNSUPPORTED_PROTOCOL;

      *val |= h->protocol;
    }
  } ആകുമ്പോൾ(str && str++);

  എങ്കിൽ(!*val)
    /* no protocol listed */
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  തിരികെ CURLE_OK;
}

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
സ്ഥിതം CURLcode httpauth(ഘടന Curl_easy *data, bool proxy,
                         unsigned long auth)
{
  എങ്കിൽ(auth != CURLAUTH_NONE) {
    പൂർണ്ണസംഖ്യ bitcheck = 0;
    bool authbits = FALSE;
    എങ്കിൽ(auth & CURLAUTH_DIGEST_IE) {
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
    ആകുമ്പോൾ(bitcheck < 31) {
      എങ്കിൽ(auth & (1UL << bitcheck++)) {
        authbits = TRUE;
        നിർത്തുക;
      }
    }
    എങ്കിൽ(!authbits)
      തിരികെ CURLE_NOT_BUILT_IN; /* no supported types left! */
  }
  എങ്കിൽ(proxy)
    data->set.proxyauth = (uint32_t)auth;
  അല്ലെങ്കിൽ
    data->set.httpauth = (uint32_t)auth;
  തിരികെ CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */

#ifndef CURL_DISABLE_HTTP
സ്ഥിതം CURLcode setopt_HTTP_VERSION(ഘടന Curl_easy *data, long arg)
{
  /*
   * This sets a requested HTTP version to be used. The value is one of
   * the listed enums in curl/curl.h.
   */
  തിരഞ്ഞെടുപ്പ്(arg) {
  അവസ്ഥ CURL_HTTP_VERSION_NONE:
    /* accepted */
    നിർത്തുക;
  അവസ്ഥ CURL_HTTP_VERSION_1_0:
  അവസ്ഥ CURL_HTTP_VERSION_1_1:
    /* accepted */
    നിർത്തുക;
#ifdef USE_HTTP2
  അവസ്ഥ CURL_HTTP_VERSION_2_0:
  അവസ്ഥ CURL_HTTP_VERSION_2TLS:
  അവസ്ഥ CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    /* accepted */
    നിർത്തുക;
#endif
#ifdef USE_HTTP3
  അവസ്ഥ CURL_HTTP_VERSION_3:
  അവസ്ഥ CURL_HTTP_VERSION_3ONLY:
    /* accepted */
    നിർത്തുക;
#endif
  സ്ഥിരസ്ഥിതി:
    /* not accepted */
    എങ്കിൽ(arg < CURL_HTTP_VERSION_NONE)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    തിരികെ CURLE_UNSUPPORTED_PROTOCOL;
  }
  data->set.httpwant = (unsigned അക്ഷരം)arg;
  തിരികെ CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP */

#ifdef USE_SSL
CURLcode Curl_setopt_SSLVERSION(ഘടന Curl_easy *data, CURLoption option,
                                long arg)
{
  /*
   * Set explicit SSL version to try to connect with, as some SSL
   * implementations are lame.
   */
  {
    long version, version_max;
    ഘടന ssl_easy_config *sslc = &data->set.ssl;
#ifndef CURL_DISABLE_PROXY
    എങ്കിൽ(option != CURLOPT_SSLVERSION)
      sslc = &data->set.proxy_ssl;
#അല്ലെങ്കിൽ
    (ശൂന്യം)option; /* unused */
#endif
    version = C_SSLVERSION_മൂല്യം(arg);
    version_max = (long)C_SSLVERSION_MAX_മൂല്യം(arg);

    എങ്കിൽ(version < CURL_SSLVERSION_DEFAULT ||
       version == CURL_SSLVERSION_SSLv2 ||
       version == CURL_SSLVERSION_SSLv3 ||
       version >= CURL_SSLVERSION_LAST ||
       version_max < CURL_SSLVERSION_MAX_NONE ||
       version_max >= CURL_SSLVERSION_MAX_LAST)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    എങ്കിൽ(version == CURL_SSLVERSION_DEFAULT)
      version = CURL_SSLVERSION_TLSv1_2;

    sslc->version = (unsigned അക്ഷരം)version;
    sslc->version_max = (unsigned പൂർണ്ണസംഖ്യ)version_max;
  }
  തിരികെ CURLE_OK;
}
#endif /* !USE_SSL */

#ifndef CURL_DISABLE_RTSP
സ്ഥിതം CURLcode setopt_RTSP_REQUEST(ഘടന Curl_easy *data, long arg)
{
  /*
   * Set the RTSP request method (OPTIONS, SETUP, PLAY, etc...) Would this be
   * better if the RTSPREQ_* were moved into here?
   */
  എങ്കിൽ((arg <= CURL_RTSPREQ_NONE) || (arg >= CURL_RTSPREQ_LAST))
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  data->set.rtspreq = (unsigned അക്ഷരം)arg;
  തിരികെ CURLE_OK;
}
#endif /* !CURL_DISABLE_RTSP */

സ്ഥിതം CURLcode setopt_long_bool(ഘടന Curl_easy *data, CURLoption option,
                                 long arg)
{
  bool enabled = !!arg;
  പൂർണ്ണസംഖ്യ ok = 1;
  ഘടന UserDefined *s = &data->set;
  തിരഞ്ഞെടുപ്പ്(option) {
  അവസ്ഥ CURLOPT_FORBID_REUSE:
    /*
     * When this transfer is done, it must not be left to be reused by a
     * subsequent transfer but shall be closed immediately.
     */
    s->reuse_forbid = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_FRESH_CONNECT:
    /*
     * This transfer shall not use a previously cached connection but
     * should be made with a fresh new connect!
     */
    s->reuse_fresh = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_VERBOSE:
    /*
     * Verbose means infof() calls that give a lot of information about
     * the connection and transfer procedures as well as internal choices.
     */
    s->verbose = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_HEADER:
    /*
     * Set to include the header in the general data output stream.
     */
    s->include_header = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_NOPROGRESS:
    /*
     * Shut off the internal supported progress meter
     */
    data->progress.hide = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_NOBODY:
    /*
     * Do not include the body part in the output data stream.
     */
    s->opt_no_body = enabled;
#ifndef CURL_DISABLE_HTTP
    എങ്കിൽ(s->opt_no_body)
      /* in HTTP lingo, no body means using the HEAD request... */
      s->method = HTTPREQ_HEAD;
    അല്ലെങ്കിൽ എങ്കിൽ(s->method == HTTPREQ_HEAD)
      s->method = HTTPREQ_GET;
#endif
    നിർത്തുക;
  അവസ്ഥ CURLOPT_FAILONERROR:
    /*
     * Do not output the >=400 error code HTML-page, but instead only
     * return error.
     */
    s->http_fail_on_error = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_KEEP_SENDING_ON_ERROR:
    s->http_keep_sending_on_error = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_UPLOAD:
  അവസ്ഥ CURLOPT_PUT:
    /*
     * We want to send data to the remote host. If this is HTTP, that equals
     * using the PUT request.
     */
    എങ്കിൽ(enabled) {
      /* If this is HTTP, PUT is what's needed to "upload" */
      s->method = HTTPREQ_PUT;
      s->opt_no_body = FALSE; /* this is implied */
    }
    അല്ലെങ്കിൽ
      /* In HTTP, the opposite of upload is GET (unless NOBODY is true as
         then this can be changed to HEAD later on) */
      s->method = HTTPREQ_GET;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_FILETIME:
    /*
     * Try to get the file time of the remote document. The time will
     * later (possibly) become available using curl_easy_getinfo().
     */
    s->get_filetime = enabled;
    നിർത്തുക;
#ifndef CURL_DISABLE_HTTP
  അവസ്ഥ CURLOPT_HTTP09_ALLOWED:
    s->http09_allowed = enabled;
    നിർത്തുക;
#ifndef CURL_DISABLE_COOKIES
  അവസ്ഥ CURLOPT_COOKIESESSION:
    /*
     * Set this option to TRUE to start a new "cookie session". It will
     * prevent the forthcoming read-cookies-from-file actions to accept
     * cookies that are marked as being session cookies, as they belong to a
     * previous session.
     */
    s->cookiesession = enabled;
    നിർത്തുക;
#endif
  അവസ്ഥ CURLOPT_AUTOREFERER:
    /*
     * Switch on automatic referer that gets set if curl follows locations.
     */
    s->http_auto_referer = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_TRANSFER_ENCODING:
    s->http_transfer_encoding = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_UNRESTRICTED_AUTH:
    /*
     * Send authentication (user+password) when following locations, even when
     * hostname changed.
     */
    s->allow_auth_to_other_hosts = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_HTTP_TRANSFER_DECODING:
    /*
     * disable libcurl transfer encoding is used
     */
    s->http_te_skip = !enabled; /* reversed */
    നിർത്തുക;
  അവസ്ഥ CURLOPT_HTTP_CONTENT_DECODING:
    /*
     * raw data passed to the application when content encoding is used
     */
    s->http_ce_skip = !enabled; /* reversed */
    നിർത്തുക;
  അവസ്ഥ CURLOPT_HTTPGET:
    /*
     * Set to force us do HTTP GET
     */
    എങ്കിൽ(enabled) {
      s->method = HTTPREQ_GET;
      s->opt_no_body = FALSE; /* this is implied */
    }
    നിർത്തുക;
  അവസ്ഥ CURLOPT_POST:
    /* Does this option serve a purpose anymore? Yes it does, when
       CURLOPT_POSTFIELDS is not used and the POST data is read off the
       callback! */
    എങ്കിൽ(enabled) {
      s->method = HTTPREQ_POST;
      s->opt_no_body = FALSE; /* this is implied */
    }
    അല്ലെങ്കിൽ
      s->method = HTTPREQ_GET;
    നിർത്തുക;
#endif /* !CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_PROXY
  അവസ്ഥ CURLOPT_HTTPPROXYTUNNEL:
    /*
     * Tunnel operations through the proxy instead of normal proxy use
     */
    s->tunnel_thru_httpproxy = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_HAPROXYPROTOCOL:
    /*
     * Set to send the HAProxy Proxy Protocol header
     */
    s->haproxyprotocol = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_PROXY_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for proxy.
     */
    s->proxy_ssl.verifypeer = enabled;

    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    നിർത്തുക;
  അവസ്ഥ CURLOPT_PROXY_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for proxy
     */
    s->proxy_ssl.verifyhost = enabled;
    ok = 2;
    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    നിർത്തുക;
  അവസ്ഥ CURLOPT_PROXY_TRANSFER_MODE:
    /*
     * set transfer mode (;type=<a|i>) when doing FTP via an HTTP proxy
     */
    s->proxy_transfer_mode = enabled;
    നിർത്തുക;
#endif /* !CURL_DISABLE_PROXY */
#എങ്കിൽ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  അവസ്ഥ CURLOPT_SOCKS5_GSSAPI_NEC:
    /*
     * Set flag for NEC SOCKS5 support
     */
    s->socks5_gssapi_nec = enabled;
    നിർത്തുക;
#endif
#ifdef CURL_LIST_ONLY_PROTOCOL
  അവസ്ഥ CURLOPT_DIRLISTONLY:
    /*
     * An option that changes the command to one that asks for a list only, no
     * file info details. Used for FTP, POP3 and SFTP.
     */
    s->list_only = enabled;
    നിർത്തുക;
#endif
  അവസ്ഥ CURLOPT_APPEND:
    /*
     * We want to upload and append to an existing file. Used for FTP and
     * SFTP.
     */
    s->remote_append = enabled;
    നിർത്തുക;
#ifndef CURL_DISABLE_FTP
  അവസ്ഥ CURLOPT_FTP_USE_EPRT:
    s->ftp_use_eprt = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_FTP_USE_EPSV:
    s->ftp_use_epsv = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_FTP_USE_PRET:
    s->ftp_use_pret = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_FTP_SKIP_PASV_IP:
    /*
     * Enable or disable FTP_SKIP_PASV_IP, which will disable/enable the
     * bypass of the IP address in PASV responses.
     */
    s->ftp_skip_ip = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_WILDCARDMATCH:
    s->wildcard_enabled = enabled;
    നിർത്തുക;
#endif
  അവസ്ഥ CURLOPT_CRLF:
    /*
     * Kludgy option to enable CRLF conversions. Subject for removal.
     */
    s->crlf = enabled;
    നിർത്തുക;
#ifndef CURL_DISABLE_TFTP
  അവസ്ഥ CURLOPT_TFTP_NO_OPTIONS:
    /*
     * Option that prevents libcurl from sending TFTP option requests to the
     * server.
     */
    s->tftp_no_options = enabled;
    നിർത്തുക;
#endif /* !CURL_DISABLE_TFTP */
  അവസ്ഥ CURLOPT_TRANSFERTEXT:
    /*
     * This option was previously named 'FTPASCII'. Renamed to work with
     * more protocols than merely FTP.
     *
     * Transfer using ASCII (instead of BINARY).
     */
    s->prefer_ascii = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying.
     */
    s->ssl.verifypeer = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    നിർത്തുക;
#ifndef CURL_DISABLE_DOH
  അവസ്ഥ CURLOPT_DOH_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for DoH.
     */
    s->doh_verifypeer = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_DOH_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for DoH
     */
    s->doh_verifyhost = enabled;
    ok = 2;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_DOH_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying for DoH.
     */
    എങ്കിൽ(!Curl_ssl_cert_status_request())
      തിരികെ CURLE_NOT_BUILT_IN;

    s->doh_verifystatus = enabled;
    ok = 2;
    നിർത്തുക;
#endif /* !CURL_DISABLE_DOH */
  അവസ്ഥ CURLOPT_SSL_VERIFYHOST:
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
    നിർത്തുക;
  അവസ്ഥ CURLOPT_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying.
     */
    എങ്കിൽ(!Curl_ssl_cert_status_request())
      തിരികെ CURLE_NOT_BUILT_IN;

    s->ssl.verifystatus = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    നിർത്തുക;
  അവസ്ഥ CURLOPT_CERTINFO:
#ifdef USE_SSL
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_CERTINFO))
      s->ssl.certinfo = enabled;
    അല്ലെങ്കിൽ
#endif
      തിരികെ CURLE_NOT_BUILT_IN;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_NOSIGNAL:
    /*
     * The application asks not to set any signal() or alarm() handlers,
     * even when using a timeout.
     */
    s->no_signal = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_TCP_NODELAY:
    /*
     * Enable or disable TCP_NODELAY, which will disable/enable the Nagle
     * algorithm
     */
    s->tcp_nodelay = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_IGNORE_CONTENT_LENGTH:
    s->ignorecl = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_SSL_SESSIONID_CACHE:
    s->ssl.cache_session = enabled;
#ifndef CURL_DISABLE_PROXY
    s->proxy_ssl.cache_session = s->ssl.cache_session;
#endif
    നിർത്തുക;
#ifdef USE_SSH
  അവസ്ഥ CURLOPT_SSH_COMPRESSION:
    s->ssh_compression = enabled;
    നിർത്തുക;
#endif /* !USE_SSH */
#ifndef CURL_DISABLE_SMTP
  അവസ്ഥ CURLOPT_MAIL_RCPT_ALLOWFAILS:
    /* allow RCPT TO command to fail for some recipients */
    s->mail_rcpt_allowfails = enabled;
    നിർത്തുക;
#endif /* !CURL_DISABLE_SMTP */
  അവസ്ഥ CURLOPT_SASL_IR:
    /* Enable/disable SASL initial response */
    s->sasl_ir = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_TCP_KEEPALIVE:
    s->tcp_keepalive = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_TCP_FASTOPEN:
#എങ്കിൽ defined(CONNECT_DATA_IDEMPOTENT) || defined(MSG_FASTOPEN) || \
  defined(TCP_FASTOPEN_CONNECT)
    s->tcp_fastopen = enabled;
    നിർത്തുക;
#അല്ലെങ്കിൽ
    തിരികെ CURLE_NOT_BUILT_IN;
#endif
  അവസ്ഥ CURLOPT_SSL_ENABLE_ALPN:
    s->ssl_enable_alpn = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_PATH_AS_IS:
    s->path_as_is = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_PIPEWAIT:
    s->pipewait = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_SUPPRESS_CONNECT_HEADERS:
    s->suppress_connect_headers = enabled;
    നിർത്തുക;
#ifndef CURL_DISABLE_SHUFFLE_DNS
  അവസ്ഥ CURLOPT_DNS_SHUFFLE_ADDRESSES:
    s->dns_shuffle_addresses = enabled;
    നിർത്തുക;
#endif
  അവസ്ഥ CURLOPT_DISALLOW_USERNAME_IN_URL:
    s->disallow_username_in_url = enabled;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_QUICK_EXIT:
    s->quick_exit = enabled;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }
  എങ്കിൽ((arg > ok) || (arg < 0))
    /* reserve other values for future use */
    infof(data, "boolean setopt(%d) got unsupported argument %ld,"
          " treated as %d", (പൂർണ്ണസംഖ്യ)option, arg, enabled);

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode മൂല്യം_range(long *മൂല്യം, long below_error, long min, long max)
{
  എങ്കിൽ(*മൂല്യം < below_error)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  അല്ലെങ്കിൽ എങ്കിൽ(*മൂല്യം < min)
    *മൂല്യം = min;
  അല്ലെങ്കിൽ എങ്കിൽ(*മൂല്യം > max)
    *മൂല്യം = max;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode setopt_long_net(ഘടന Curl_easy *data, CURLoption option,
                                long arg)
{
  CURLcode result = CURLE_OK;
  ഘടന UserDefined *s = &data->set;

  തിരഞ്ഞെടുപ്പ്(option) {
  അവസ്ഥ CURLOPT_DNS_CACHE_TIMEOUT:
    എങ്കിൽ(arg != -1)
      തിരികെ setopt_set_timeout_sec(&s->dns_cache_timeout_ms, arg);
    s->dns_cache_timeout_ms = -1;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_MAXCONNECTS:
    result = മൂല്യം_range(&arg, 0, 0, INT_MAX);
    എങ്കിൽ(!result)
      s->maxconnects = arg ? (uint32_t)arg : DEFAULT_CONNCACHE_SIZE;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_SERVER_RESPONSE_TIMEOUT:
    തിരികെ setopt_set_timeout_sec(&s->server_response_timeout, arg);
  അവസ്ഥ CURLOPT_SERVER_RESPONSE_TIMEOUT_MS:
    തിരികെ setopt_set_timeout_ms(&s->server_response_timeout, arg);
  അവസ്ഥ CURLOPT_താഴെയുള്ള_SPEED_LIMIT:
    എങ്കിൽ(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    അല്ലെങ്കിൽ
      s->താഴെയുള്ള_speed_limit = arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_താഴെയുള്ള_SPEED_TIME:
    result = മൂല്യം_range(&arg, 0, 0, USHRT_MAX);
    എങ്കിൽ(!result)
      s->താഴെയുള്ള_speed_time = (uint16_t)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_PORT:
    എങ്കിൽ((arg < 0) || (arg > 65535))
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    s->use_port = (unsigned short)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_TIMEOUT:
    തിരികെ setopt_set_timeout_sec(&s->timeout, arg);
  അവസ്ഥ CURLOPT_TIMEOUT_MS:
    തിരികെ setopt_set_timeout_ms(&s->timeout, arg);
  അവസ്ഥ CURLOPT_CONNECTTIMEOUT:
    തിരികെ setopt_set_timeout_sec(&s->connecttimeout, arg);
  അവസ്ഥ CURLOPT_CONNECTTIMEOUT_MS:
    തിരികെ setopt_set_timeout_ms(&s->connecttimeout, arg);
#ifndef CURL_DISABLE_BINDLOCAL
  അവസ്ഥ CURLOPT_LOCALPORT:
    എങ്കിൽ((arg < 0) || (arg > 65535))
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    s->localport = curlx_sltous(arg);
    നിർത്തുക;
  അവസ്ഥ CURLOPT_LOCALPORTRANGE:
    എങ്കിൽ((arg < 0) || (arg > 65535))
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    s->localportrange = curlx_sltous(arg);
    നിർത്തുക;
#endif
  അവസ്ഥ CURLOPT_BUFFERSIZE:
    result = മൂല്യം_range(&arg, 0, READBUFFER_MIN, READBUFFER_MAX);
    എങ്കിൽ(!result)
      s->buffer_size = (unsigned പൂർണ്ണസംഖ്യ)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_UPLOAD_BUFFERSIZE:
    result = മൂല്യം_range(&arg, 0, UPLOADBUFFER_MIN, UPLOADBUFFER_MAX);
    എങ്കിൽ(!result)
      s->upload_buffer_size = (unsigned പൂർണ്ണസംഖ്യ)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_MAXFILESIZE:
    എങ്കിൽ(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    അല്ലെങ്കിൽ
      s->max_filesize = arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_IPRESOLVE:
    എങ്കിൽ((arg < CURL_IPRESOLVE_WHATEVER) || (arg > CURL_IPRESOLVE_V6))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    അല്ലെങ്കിൽ
      s->ipver = (unsigned അക്ഷരം)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_CONNECT_ONLY:
    എങ്കിൽ(arg < 0 || arg > 2)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    അല്ലെങ്കിൽ {
      s->connect_only = !!arg;
      s->connect_only_ws = (arg == 2);
    }
    നിർത്തുക;
#ifdef USE_IPV6
  അവസ്ഥ CURLOPT_ADDRESS_SCOPE:
#എങ്കിൽ SIZEOF_LONG > 4
    എങ്കിൽ((unsigned long)arg > UINT_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    അല്ലെങ്കിൽ
#endif
    s->scope_id = (unsigned പൂർണ്ണസംഖ്യ)arg;
    നിർത്തുക;
#endif
  അവസ്ഥ CURLOPT_TCP_KEEPIDLE:
    result = മൂല്യം_range(&arg, 0, 0, INT_MAX);
    എങ്കിൽ(!result)
      s->tcp_keepidle = (പൂർണ്ണസംഖ്യ)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_TCP_KEEPINTVL:
    result = മൂല്യം_range(&arg, 0, 0, INT_MAX);
    എങ്കിൽ(!result)
      s->tcp_keepintvl = (പൂർണ്ണസംഖ്യ)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_TCP_KEEPCNT:
    result = മൂല്യം_range(&arg, 0, 0, INT_MAX);
    എങ്കിൽ(!result)
      s->tcp_keepcnt = (പൂർണ്ണസംഖ്യ)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
    തിരികെ setopt_set_timeout_ms(&s->happy_eyeballs_timeout, arg);
  അവസ്ഥ CURLOPT_UPKEEP_INTERVAL_MS:
    തിരികെ setopt_set_timeout_ms(&s->upkeep_interval_ms, arg);
  അവസ്ഥ CURLOPT_MAXAGE_CONN:
    തിരികെ setopt_set_timeout_sec(&s->conn_max_idle_ms, arg);
  അവസ്ഥ CURLOPT_MAXLIFETIME_CONN:
    തിരികെ setopt_set_timeout_sec(&s->conn_max_age_ms, arg);
  അവസ്ഥ CURLOPT_DNS_USE_GLOBAL_CACHE:
    /* deprecated */
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode setopt_long_ssl(ഘടന Curl_easy *data, CURLoption option,
                                long arg)
{
#ifdef USE_SSL
  CURLcode result = CURLE_OK;
  ഘടന UserDefined *s = &data->set;
  തിരഞ്ഞെടുപ്പ്(option) {
  അവസ്ഥ CURLOPT_CA_CACHE_TIMEOUT:
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_CA_CACHE)) {
      result = മൂല്യം_range(&arg, -1, -1, INT_MAX);
      എങ്കിൽ(!result)
        s->ssl_ca_cache_timeout = (പൂർണ്ണസംഖ്യ)arg;
    }
    അല്ലെങ്കിൽ
      result = CURLE_NOT_BUILT_IN;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_SSLVERSION:
#ifndef CURL_DISABLE_PROXY
  അവസ്ഥ CURLOPT_PROXY_SSLVERSION:
#endif
    തിരികെ Curl_setopt_SSLVERSION(data, option, arg);
  അവസ്ഥ CURLOPT_SSL_FALSESTART:
    result = CURLE_NOT_BUILT_IN;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_USE_SSL:
    എങ്കിൽ((arg < CURLUSESSL_NONE) || (arg >= CURLUSESSL_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    അല്ലെങ്കിൽ
      s->use_ssl = (unsigned അക്ഷരം)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_SSL_OPTIONS:
    s->ssl.ssl_options = (unsigned അക്ഷരം)(arg & 0xff);
    നിർത്തുക;
#ifndef CURL_DISABLE_PROXY
  അവസ്ഥ CURLOPT_PROXY_SSL_OPTIONS:
    s->proxy_ssl.ssl_options = (unsigned അക്ഷരം)(arg & 0xff);
    നിർത്തുക;
#endif
  അവസ്ഥ CURLOPT_SSL_ENABLE_NPN:
    നിർത്തുക;
  അവസ്ഥ CURLOPT_SSLENGINE_DEFAULT:
#ifdef USE_OPENSSL
    എങ്കിൽ(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      തിരികെ CURLE_NOT_BUILT_IN;
    data->set.engine_default = (arg == 1);
#അല്ലെങ്കിൽ
    തിരികെ CURLE_NOT_BUILT_IN;
#endif
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }
  തിരികെ result;
#അല്ലെങ്കിൽ  /* USE_SSL */
  (ശൂന്യം)data;
  (ശൂന്യം)option;
  (ശൂന്യം)arg;
  തിരികെ CURLE_UNKNOWN_OPTION;
#endif /* !USE_SSL */
}

#ifndef CURL_DISABLE_PROXY
സ്ഥിതം ശൂന്യം changeproxy(ഘടന Curl_easy *data)
{
  Curl_auth_digest_cleanup(&data->state.proxydigest);
  memset(&data->state.authproxy, 0, വലുപ്പം(data->state.authproxy));
}

സ്ഥിതം CURLcode setopt_long_proxy(ഘടന Curl_easy *data, CURLoption option,
                                  long arg)
{
  ഘടന UserDefined *s = &data->set;

  തിരഞ്ഞെടുപ്പ്(option) {
  അവസ്ഥ CURLOPT_PROXYPORT:
    എങ്കിൽ((arg < 0) || (arg > UINT16_MAX))
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    എങ്കിൽ(arg != s->proxyport)
      changeproxy(data);
    s->proxyport = (uint16_t)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_PROXYAUTH:
    തിരികെ httpauth(data, TRUE, (unsigned long)arg);
  അവസ്ഥ CURLOPT_PROXYTYPE:
    എങ്കിൽ((arg < CURLPROXY_HTTP) || (arg > CURLPROXY_HTTPS3))
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef USE_PROXY_HTTP3
    എങ്കിൽ(arg == CURLPROXY_HTTPS3)
      തിരികെ CURLE_NOT_BUILT_IN;
#endif
    s->proxytype = (unsigned അക്ഷരം)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_SOCKS5_AUTH: {
    unsigned long auth = (unsigned long)arg;
    എങ്കിൽ(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI | CURLAUTH_ONLY))
      തിരികെ CURLE_NOT_BUILT_IN;
    എങ്കിൽ((auth & CURLAUTH_ONLY) && !(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI)))
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    s->socks5auth = (unsigned അക്ഷരം)(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI));
    s->socks5_auth_only = !!(auth & CURLAUTH_ONLY);
    നിർത്തുക;
  }
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }
  തിരികെ CURLE_OK;
}
#അല്ലെങ്കിൽ
സ്ഥിതം CURLcode setopt_long_proxy(ഘടന Curl_easy *data, CURLoption option,
                                  long arg)
{
  (ശൂന്യം)data;
  (ശൂന്യം)option;
  (ശൂന്യം)arg;
  തിരികെ CURLE_UNKNOWN_OPTION;
}
#endif

സ്ഥിതം CURLcode setopt_long_http(ഘടന Curl_easy *data, CURLoption option,
                                 long arg)
{
#ifndef CURL_DISABLE_HTTP
  CURLcode result = CURLE_OK;
  ഘടന UserDefined *s = &data->set;

  തിരഞ്ഞെടുപ്പ്(option) {
  അവസ്ഥ CURLOPT_FOLLOWLOCATION:
    എങ്കിൽ((unsigned long)arg > 3)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    അല്ലെങ്കിൽ
      s->http_follow_mode = (unsigned അക്ഷരം)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_MAXREDIRS:
    result = മൂല്യം_range(&arg, -1, -1, 0x7fff);
    എങ്കിൽ(!result)
      s->maxredirs = (short)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_POSTREDIR:
    എങ്കിൽ(arg < CURL_REDIR_GET_ALL)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    അല്ലെങ്കിൽ {
      s->post301 = !!(arg & CURL_REDIR_POST_301);
      s->post302 = !!(arg & CURL_REDIR_POST_302);
      s->post303 = !!(arg & CURL_REDIR_POST_303);
    }
    നിർത്തുക;
  അവസ്ഥ CURLOPT_HEADEROPT:
    s->sep_headers = !!(arg & CURLHEADER_SEPARATE);
    നിർത്തുക;
  അവസ്ഥ CURLOPT_HTTPAUTH:
    തിരികെ httpauth(data, FALSE, (unsigned long)arg);
  അവസ്ഥ CURLOPT_HTTP_VERSION:
    തിരികെ setopt_HTTP_VERSION(data, arg);
  അവസ്ഥ CURLOPT_EXPECT_100_TIMEOUT_MS:
    result = മൂല്യം_range(&arg, 0, 0, 0xffff);
    എങ്കിൽ(!result)
      s->expect_100_timeout = (unsigned short)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_STREAM_WEIGHT:
#എങ്കിൽ defined(USE_HTTP2) || defined(USE_HTTP3)
    എങ്കിൽ((arg >= 1) && (arg <= 256))
      s->weight = (പൂർണ്ണസംഖ്യ)arg;
    നിർത്തുക;
#അല്ലെങ്കിൽ
    result = CURLE_NOT_BUILT_IN;
    നിർത്തുക;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  അവസ്ഥ CURLOPT_HTTPSIG_ALGORITHM:
    എങ്കിൽ(arg != CURLHTTPSIG_NONE &&
       arg != CURLHTTPSIG_ED25519 &&
       arg != CURLHTTPSIG_HMAC_SHA256)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    s->httpsig_algorithm = (uint8_t)arg;
    എങ്കിൽ(arg)
      s->httpauth = (uint32_t)CURLAUTH_HTTPSIG;
    അല്ലെങ്കിൽ
      s->httpauth &= ~(uint32_t)CURLAUTH_HTTPSIG;
    നിർത്തുക;
#endif
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }
  തിരികെ result;
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
  (ശൂന്യം)option;
  (ശൂന്യം)arg;
  തിരികെ CURLE_UNKNOWN_OPTION;
#endif
}

സ്ഥിതം CURLcode setopt_long_proto(ഘടന Curl_easy *data, CURLoption option,
                                  long arg)
{
  CURLcode result = CURLE_OK;
  ഘടന UserDefined *s = &data->set;

  തിരഞ്ഞെടുപ്പ്(option) {
#ifndef CURL_DISABLE_TFTP
  അവസ്ഥ CURLOPT_TFTP_BLKSIZE:
    result = മൂല്യം_range(&arg, 0, TFTP_BLKSIZE_MIN, TFTP_BLKSIZE_MAX);
    എങ്കിൽ(!result)
      s->tftp_blksize = (unsigned short)arg;
    നിർത്തുക;
#endif
#ifndef CURL_DISABLE_NETRC
  അവസ്ഥ CURLOPT_NETRC:
    എങ്കിൽ((arg < CURL_NETRC_IGNORED) || (arg >= CURL_NETRC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    അല്ലെങ്കിൽ
      s->use_netrc = (unsigned അക്ഷരം)arg;
    നിർത്തുക;
#endif
#ifndef CURL_DISABLE_FTP
  അവസ്ഥ CURLOPT_FTP_FILEMETHOD:
    എങ്കിൽ((arg < CURLFTPMETHOD_DEFAULT) || (arg >= CURLFTPMETHOD_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    അല്ലെങ്കിൽ
      s->ftp_filemethod = (unsigned അക്ഷരം)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_FTP_SSL_CCC:
    എങ്കിൽ((arg < CURLFTPSSL_CCC_NONE) || (arg >= CURLFTPSSL_CCC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    അല്ലെങ്കിൽ
      s->ftp_ccc = (unsigned അക്ഷരം)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_FTPSSLAUTH:
    എങ്കിൽ((arg < CURLFTPAUTH_DEFAULT) || (arg >= CURLFTPAUTH_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    അല്ലെങ്കിൽ
      s->ftpsslauth = (unsigned അക്ഷരം)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_ACCEPTTIMEOUT_MS:
    തിരികെ setopt_set_timeout_ms(&s->accepttimeout, arg);
#endif
#എങ്കിൽ !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  അവസ്ഥ CURLOPT_FTP_CREATE_MISSING_DIRS:
    എങ്കിൽ((arg < CURLFTP_CREATE_DIR_NONE) || (arg > CURLFTP_CREATE_DIR_RETRY))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    അല്ലെങ്കിൽ
      s->ftp_create_missing_dirs = (unsigned അക്ഷരം)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_NEW_FILE_PERMS:
    എങ്കിൽ((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    അല്ലെങ്കിൽ
      s->new_file_perms = (unsigned പൂർണ്ണസംഖ്യ)arg;
    നിർത്തുക;
#endif
#ifndef CURL_DISABLE_RTSP
  അവസ്ഥ CURLOPT_RTSP_REQUEST:
    തിരികെ setopt_RTSP_REQUEST(data, arg);
  അവസ്ഥ CURLOPT_RTSP_CLIENT_CSEQ:
    result = മൂല്യം_range(&arg, 0, 0, INT_MAX);
    എങ്കിൽ(!result)
      data->state.rtsp_next_client_CSeq = (uint32_t)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_RTSP_SERVER_CSEQ:
    result = മൂല്യം_range(&arg, 0, 0, INT_MAX);
    എങ്കിൽ(!result)
      data->state.rtsp_next_server_CSeq = (uint32_t)arg;
    നിർത്തുക;
#endif
#ifdef USE_SSH
  അവസ്ഥ CURLOPT_SSH_AUTH_TYPES:
    s->ssh_auth_types = (uint32_t)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_NEW_DIRECTORY_PERMS:
    എങ്കിൽ((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    അല്ലെങ്കിൽ
      s->new_directory_perms = (unsigned പൂർണ്ണസംഖ്യ)arg;
    നിർത്തുക;
#endif
  അവസ്ഥ CURLOPT_PROTOCOLS:
    s->allowed_protocols = (curl_prot_t)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_REDIR_PROTOCOLS:
    s->redir_protocols = (curl_prot_t)arg;
    നിർത്തുക;
#ifndef CURL_DISABLE_WEBSOCKETS
  അവസ്ഥ CURLOPT_WS_OPTIONS:
    s->ws_raw_mode = !!(arg & CURLWS_RAW_MODE);
    s->ws_no_auto_pong = !!(arg & CURLWS_NOAUTOPONG);
    നിർത്തുക;
#endif
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode setopt_long_misc(ഘടന Curl_easy *data, CURLoption option,
                                 long arg)
{
  ഘടന UserDefined *s = &data->set;

  തിരഞ്ഞെടുപ്പ്(option) {
  അവസ്ഥ CURLOPT_TIMECONDITION:
    എങ്കിൽ((arg < CURL_TIMECOND_NONE) || (arg >= CURL_TIMECOND_LAST))
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    s->timecondition = (unsigned അക്ഷരം)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_TIMEVALUE:
    s->timevalue = (time_t)arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_POSTFIELDSIZE:
    എങ്കിൽ(arg < -1)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    എങ്കിൽ(s->postfieldsize < arg && s->str_copypostfields) {
      curlx_safefree(s->str_copypostfields);
      s->postfields = ശൂന്യ;
    }
    s->postfieldsize = arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_INFILESIZE:
    എങ്കിൽ(arg < -1)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_RESUME_FROM:
    എങ്കിൽ(arg < -1)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = arg;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_UPLOAD_FLAGS:
    s->upload_flags = (unsigned അക്ഷരം)arg;
    നിർത്തുക;
#ifndef CURL_DISABLE_MIME
  അവസ്ഥ CURLOPT_MIME_OPTIONS:
    s->mime_formescape = !!(arg & CURLMIMEOPT_FORMESCAPE);
    നിർത്തുക;
#endif
#ifndef CURL_DISABLE_HSTS
  അവസ്ഥ CURLOPT_HSTS_CTRL:
    എങ്കിൽ(arg & CURLHSTS_ENABLE) {
      എങ്കിൽ(!data->hsts) {
        data->hsts = Curl_hsts_init();
        എങ്കിൽ(!data->hsts)
          തിരികെ CURLE_OUT_OF_MEMORY;
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!data->share || !data->share->hsts) {
      /* throw away the HSTS cache unless shared */
      Curl_hsts_cleanup(&data->hsts);
      /* flush all the entries */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = ശൂന്യ;
    }
    അല്ലെങ്കിൽ
      /* detach from shared HSTS cache without freeing it */
      data->hsts = ശൂന്യ;
    നിർത്തുക;
#endif
#ifndef CURL_DISABLE_ALTSVC
  അവസ്ഥ CURLOPT_ALTSVC_CTRL:
    തിരികെ Curl_altsvc_ctrl(data, arg);
#endif
#എങ്കിൽ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  അവസ്ഥ CURLOPT_GSSAPI_DELEGATION:
    s->gssapi_delegation = (unsigned അക്ഷരം)arg &
      (CURLGSSAPI_DELEGATION_POLICY_FLAG | CURLGSSAPI_DELEGATION_FLAG);
    നിർത്തുക;
#endif
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode setopt_long(ഘടന Curl_easy *data, CURLoption option,
                            long arg)
{
  തരനിർവചനം CURLcode (*setoptfunc)(ഘടന Curl_easy *data,
                                 CURLoption option, long arg);
  സ്ഥിതം സ്ഥിരം setoptfunc setopt_call[] = {
    setopt_long_bool,
    setopt_long_net,
    setopt_long_http,
    setopt_long_proxy,
    setopt_long_ssl,
    setopt_long_proto,
    setopt_long_misc
  };
  size_t i;

  വേണ്ടി(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, arg);
    എങ്കിൽ(result != CURLE_UNKNOWN_OPTION)
      തിരികെ result;
  }
  തിരികെ CURLE_UNKNOWN_OPTION;
}

സ്ഥിതം CURLcode setopt_slist(ഘടന Curl_easy *data, CURLoption option,
                             ഘടന curl_slist *slist)
{
  CURLcode result = CURLE_OK;
  ഘടന UserDefined *s = &data->set;
  തിരഞ്ഞെടുപ്പ്(option) {
#ifndef CURL_DISABLE_PROXY
  അവസ്ഥ CURLOPT_PROXYHEADER:
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
    നിർത്തുക;
#endif
#ifndef CURL_DISABLE_HTTP
  അവസ്ഥ CURLOPT_HTTP200ALIASES:
    /*
     * Set a list of aliases for HTTP 200 in response header
     */
    s->http200aliases = slist;
    നിർത്തുക;
#endif
#എങ്കിൽ !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  അവസ്ഥ CURLOPT_POSTQUOTE:
    /*
     * List of RAW FTP commands to use after a transfer
     */
    s->postquote = slist;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_PREQUOTE:
    /*
     * List of RAW FTP commands to use prior to RETR (Wesley Laxton)
     */
    s->prequote = slist;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_QUOTE:
    /*
     * List of RAW FTP commands to use before a transfer
     */
    s->quote = slist;
    നിർത്തുക;
#endif
  അവസ്ഥ CURLOPT_RESOLVE:
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
    നിർത്തുക;
#എങ്കിൽ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MIME)
  അവസ്ഥ CURLOPT_HTTPHEADER:
    /*
     * Set a list with HTTP headers to use (or replace internals with)
     */
    s->headers = slist;
    നിർത്തുക;
#endif
#ifndef CURL_DISABLE_TELNET
  അവസ്ഥ CURLOPT_TELNETOPTIONS:
    /*
     * Set a linked list of telnet options
     */
    s->telnet_options = slist;
    നിർത്തുക;
#endif
#ifndef CURL_DISABLE_SMTP
  അവസ്ഥ CURLOPT_MAIL_RCPT:
    /* Set the list of mail recipients */
    s->mail_rcpt = slist;
    നിർത്തുക;
#endif
  അവസ്ഥ CURLOPT_CONNECT_TO:
    s->connect_to = slist;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }
  തിരികെ result;
}

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
സ്ഥിതം CURLcode setopt_mimepost(ഘടന Curl_easy *data, curl_mime *mimep)
{
  /*
   * Set to make us do MIME POST
   */
  CURLcode result;
  ഘടന UserDefined *s = &data->set;
  എങ്കിൽ(!s->mimepostp) {
    s->mimepostp = curlx_malloc(വലുപ്പം(*s->mimepostp));
    എങ്കിൽ(!s->mimepostp)
      തിരികെ CURLE_OUT_OF_MEMORY;
    Curl_mime_initpart(s->mimepostp);
  }

  result = Curl_mime_set_subparts(s->mimepostp, mimep, FALSE);
  എങ്കിൽ(!result) {
    s->method = HTTPREQ_POST_MIME;
    s->opt_no_body = FALSE; /* this is implied */
#ifndef CURL_DISABLE_FORM_API
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = ശൂന്യ;
#endif
  }
  തിരികെ result;
}
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */

സ്ഥിതം CURLcode setopt_share(ഘടന Curl_easy *data, ഘടന Curl_share *set)
{
  CURLcode result;

  എങ്കിൽ(data->conn) {
    /* As this handle already has a connection attached, changing share now
       would be complicated and error-prone */
    infof(data, "Cannot change share object while in use");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
  }
  അല്ലെങ്കിൽ {
    /* disconnect from old share, if any and possible */
    result = Curl_share_easy_unlink(data);
    എങ്കിൽ(!result && GOOD_SHARE_HANDLE(set))
      /* use new share if it set */
      result = Curl_share_easy_link(data, set);
  }
  തിരികെ result;
}

/* assorted pointer type arguments */
സ്ഥിതം CURLcode setopt_pointers(ഘടന Curl_easy *data, CURLoption option,
                                va_list param)
{
  CURLcode result = CURLE_OK;
  ഘടന UserDefined *s = &data->set;
  തിരഞ്ഞെടുപ്പ്(option) {
  അവസ്ഥ CURLOPT_CURLU:
    /*
     * pass CURLU to set URL
     */
    Curl_bufref_free(&data->state.url);
    CURL_EASY_STR_CLEAR(data, STRING_SET_URL);
    s->uh = va_arg(param, CURLU *);
    നിർത്തുക;
#ifndef CURL_DISABLE_HTTP
#ifndef CURL_DISABLE_FORM_API
  അവസ്ഥ CURLOPT_HTTPPOST:
    /*
     * Set to make us do HTTP POST. Legacy API-style.
     */
    s->httppost = va_arg(param, ഘടന curl_httppost *);
    s->method = HTTPREQ_POST_FORM;
    s->opt_no_body = FALSE; /* this is implied */
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = ശൂന്യ;
    നിർത്തുക;
#endif /* !CURL_DISABLE_FORM_API */
#endif /* !CURL_DISABLE_HTTP */
#എങ്കിൽ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
  അവസ്ഥ CURLOPT_MIMEPOST:
    result = setopt_mimepost(data, va_arg(param, curl_mime *));
    നിർത്തുക;
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */
  അവസ്ഥ CURLOPT_STDERR:
    /*
     * Set to a FILE * that should receive all error writes. This
     * defaults to stderr for normal operations.
     */
    s->err = va_arg(param, FILE *);
    എങ്കിൽ(!s->err)
      s->err = stderr;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_SHARE:
    തിരികെ setopt_share(data, va_arg(param, ഘടന Curl_share *));

  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }
  തിരികെ result;
}

#ifndef CURL_DISABLE_COOKIES
സ്ഥിതം CURLcode cookielist(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *ptr)
{
  CURLcode result = CURLE_OK;
  എങ്കിൽ(!ptr)
    തിരികെ CURLE_OK;

  എങ്കിൽ(curl_strequal(ptr, "ALL")) {
    /* clear all cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearall(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal(ptr, "SESS")) {
    /* clear session cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearsess(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal(ptr, "FLUSH")) {
    /* flush cookies to file, takes care of the locking */
    Curl_flush_cookies(data, FALSE);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal(ptr, "RELOAD")) {
    /* reload cookies from file */
    തിരികെ Curl_cookie_loadfiles(data, COOKIE_NOPSL |
                                 (data->set.cookiesession ?
                                  COOKIE_NOSESSION : 0));
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(!data->cookies) {
      /* if cookie engine was not running, activate it */
      data->cookies = Curl_cookie_init();
      എങ്കിൽ(!data->cookies)
        തിരികെ CURLE_OUT_OF_MEMORY;
      data->state.cookie_engine = TRUE;
    }

    /* general protection against mistakes and abuse */
    എങ്കിൽ(strlen(ptr) > CURL_MAX_നിവേശം_LENGTH)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

    /* Adding these cookies without the PSL check, because the PSL is not
       initialized until *perform() time, and this might be called before
       that */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    എങ്കിൽ(checkprefix("Set-Cookie:", ptr))
      /* HTTP Header format line */
      result = Curl_cookie_add(data, data->cookies, ptr + 11,
                               ശൂന്യ, ശൂന്യ,
                               COOKIE_HTTPHEADER | COOKIE_SECURE |
                               COOKIE_NOPSL);
    അല്ലെങ്കിൽ
      /* Netscape format line */
      result = Curl_cookie_add(data, data->cookies, ptr, ശൂന്യ,
                               ശൂന്യ, COOKIE_SECURE | COOKIE_NOPSL);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode cookiefile(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *ptr)
{
  /*
   * Set cookie file to read and parse. Can be used multiple times.
   */
  എങ്കിൽ(ptr) {
    ഘടന curl_slist *cl;
    /* general protection against mistakes and abuse */
    എങ്കിൽ(strlen(ptr) > CURL_MAX_നിവേശം_LENGTH)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    /* append the cookie filename to the list of filenames, and deal with
       them later */
    cl = curl_slist_append(data->state.cookielist, ptr);
    എങ്കിൽ(!cl) {
      curl_slist_free_all(data->state.cookielist);
      data->state.cookielist = ശൂന്യ;
      തിരികെ CURLE_OUT_OF_MEMORY;
    }
    data->state.cookielist = cl; /* store the list for later use */
  }
  അല്ലെങ്കിൽ {
    /* clear the list of cookie files */
    curl_slist_free_all(data->state.cookielist);
    data->state.cookielist = ശൂന്യ;

    എങ്കിൽ(!data->share || !data->share->cookies) {
      /* throw away all existing cookies if this is not a shared cookie
         container */
      Curl_cookie_clearall(data->cookies);
      Curl_cookie_cleanup(data->cookies);
    }
    /* disable the cookie engine */
    data->cookies = ശൂന്യ;
  }
  തിരികെ CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_PROXY
സ്ഥിതം CURLcode setproxy(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *proxy)
{
  സ്ഥിരം അക്ഷരം *str = CURL_EASY_STR(data, STRING_PROXY);
  എങ്കിൽ(str && proxy &&
     /* there was one set, is this a new one? */
     !strcmp(str, proxy))
    തിരികെ CURLE_OK; /* same one as before */

  changeproxy(data);
  തിരികെ Curl_setstropt(data, STRING_PROXY, proxy);
}

സ്ഥിതം CURLcode setopt_cptr_proxy(ഘടന Curl_easy *data, CURLoption option,
                                  അക്ഷരം *ptr)
{
  CURLcode result = CURLE_OK;
  ഘടന UserDefined *s = &data->set;
  തിരഞ്ഞെടുപ്പ്(option) {
  അവസ്ഥ CURLOPT_PROXYUSERPWD: {
    /*
     * user:password needed to use the proxy
     */
    അക്ഷരം *u = ശൂന്യ;
    അക്ഷരം *p = ശൂന്യ;
    result = setstropt_userpwd(ptr, &u, &p);

    /* URL decode the components */
    എങ്കിൽ(!result) {
      അക്ഷരം *str = ശൂന്യ;
      CURL_EASY_STR_CLEAR(data, STRING_PROXYUSERNAME);
      CURL_EASY_STR_CLEAR(data, STRING_PROXYPASSWORD);
      എങ്കിൽ(u) {
        result = Curl_urldecode(u, 0, &str, ശൂന്യ, REJECT_ZERO);
        എങ്കിൽ(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYUSERNAME, str);
      }
      എങ്കിൽ(!result && p) {
        str = ശൂന്യ;
        result = Curl_urldecode(p, 0, &str, ശൂന്യ, REJECT_ZERO);
        എങ്കിൽ(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYPASSWORD, str);
      }
    }
    curlx_free(u);
    curlx_strzero(p);
    curlx_free(p);
    നിർത്തുക;
  }
  അവസ്ഥ CURLOPT_PROXYUSERNAME:
    /*
     * authentication username to use in the operation
     */
    തിരികെ Curl_setstropt(data, STRING_PROXYUSERNAME, ptr);

  അവസ്ഥ CURLOPT_PROXYPASSWORD:
    /*
     * authentication password to use in the operation
     */
    തിരികെ Curl_setstropt(data, STRING_PROXYPASSWORD, ptr);

  അവസ്ഥ CURLOPT_NOPROXY:
    /*
     * proxy exception list
     */
    തിരികെ Curl_setstropt(data, STRING_NOPROXY, ptr);
  അവസ്ഥ CURLOPT_PROXY_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use for proxy
     */
    തിരികെ Curl_setstropt(data, STRING_CERT_PROXY, ptr);
  അവസ്ഥ CURLOPT_PROXY_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use for proxy
     */
    തിരികെ Curl_setstropt(data, STRING_CERT_TYPE_PROXY, ptr);
  അവസ്ഥ CURLOPT_PROXY_SSLKEY:
    /*
     * String that holds filename of the SSL key to use for proxy
     */
    തിരികെ Curl_setstropt(data, STRING_താക്കോൽ_PROXY, ptr);
  അവസ്ഥ CURLOPT_PROXY_KEYPASSWD:
    /*
     * String that holds the SSL private key password for proxy.
     */
    തിരികെ Curl_setstropt(data, STRING_താക്കോൽ_PASSWD_PROXY, ptr);
  അവസ്ഥ CURLOPT_PROXY_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use for proxy
     */
    തിരികെ Curl_setstropt(data, STRING_താക്കോൽ_TYPE_PROXY, ptr);
  അവസ്ഥ CURLOPT_PROXY_SSL_CIPHER_LIST:
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST)) {
      /* set a list of cipher we want to use in the SSL connection for proxy */
      തിരികെ Curl_setstropt(data, STRING_SSL_CIPHER_LIST_PROXY, ptr);
    }
    അല്ലെങ്കിൽ
      തിരികെ CURLE_NOT_BUILT_IN;
  അവസ്ഥ CURLOPT_PROXY_TLS13_CIPHERS:
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites for proxy */
      തിരികെ Curl_setstropt(data, STRING_SSL_CIPHER13_LIST_PROXY, ptr);
    അല്ലെങ്കിൽ
      തിരികെ CURLE_NOT_BUILT_IN;
  അവസ്ഥ CURLOPT_PROXY:
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
    തിരികെ setproxy(data, ptr);
  അവസ്ഥ CURLOPT_PRE_PROXY:
    /*
     * Set proxy server:port to use as SOCKS proxy.
     *
     * If the proxy is set to "" or NULL we explicitly say that we do not want
     * to use the socks proxy.
     */
    തിരികെ Curl_setstropt(data, STRING_PRE_PROXY, ptr);
  അവസ്ഥ CURLOPT_SOCKS5_GSSAPI_SERVICE:
  അവസ്ഥ CURLOPT_PROXY_SERVICE_NAME:
    /*
     * Set proxy authentication service name for Kerberos 5 and SPNEGO
     */
    തിരികെ Curl_setstropt(data, STRING_PROXY_SERVICE_NAME, ptr);
  അവസ്ഥ CURLOPT_PROXY_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
#ifdef USE_SSL
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      തിരികെ Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY_PROXY, ptr);
#endif
    തിരികെ CURLE_NOT_BUILT_IN;

  അവസ്ഥ CURLOPT_HAPROXY_CLIENT_IP:
    /*
     * Set the client IP to send through HAProxy PROXY protocol
     */
    result = Curl_setstropt(data, STRING_HAPROXY_CLIENT_IP, ptr);

    /* enable the HAProxy protocol if an IP is provided */
    s->haproxyprotocol = !!CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
    നിർത്തുക;
  അവസ്ഥ CURLOPT_PROXY_CAINFO:
    /*
     * Set CA info SSL connection for proxy. Specify filename of the
     * CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE_PROXY, ptr);
    എങ്കിൽ(!result)
      s->proxy_ssl.custom_cafile = TRUE;
    തിരികെ result;
  അവസ്ഥ CURLOPT_PROXY_CRLFILE:
    /*
     * Set CRL file info for SSL connection for proxy. Specify filename of the
     * CRL to check certificates revocation
     */
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      തിരികെ Curl_setstropt(data, STRING_SSL_CRLFILE_PROXY, ptr);
    തിരികെ CURLE_NOT_BUILT_IN;
  അവസ്ഥ CURLOPT_PROXY_ISSUERCERT:
    /*
     * Set Issuer certificate file to check certificates issuer
     */
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      തിരികെ Curl_setstropt(data, STRING_SSL_ISSUERCERT_PROXY, ptr);
    തിരികെ CURLE_NOT_BUILT_IN;
  അവസ്ഥ CURLOPT_PROXY_CAPATH:
    /*
     * Set CA path info for SSL connection proxy. Specify directory name of the
     * CA certificates which have been prepared using openssl c_rehash utility.
     */
#ifdef USE_SSL
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH_PROXY, ptr);
      എങ്കിൽ(!result)
        s->proxy_ssl.custom_capath = TRUE;
      തിരികെ result;
    }
#endif
    തിരികെ CURLE_NOT_BUILT_IN;
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }
  തിരികെ result;
}
#endif

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
/*
 * A string with POST data. Makes curl HTTP POST. Even if it is NULL. If
 * needed, CURLOPT_POSTFIELDSIZE must have been set prior to
 * CURLOPT_COPYPOSTFIELDS and not altered later.
 */
സ്ഥിതം CURLcode setopt_copypostfields(സ്ഥിരം അക്ഷരം *ptr, ഘടന UserDefined *s)
{
  CURLcode result = CURLE_OK;
  എങ്കിൽ(s->postfieldsize < -1)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  എങ്കിൽ(!ptr || s->postfieldsize == -1) {
    എങ്കിൽ(ptr && (strlen(ptr) > CURL_MAX_നിവേശം_LENGTH))
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_safefree(s->str_copypostfields);
    എങ്കിൽ(ptr) {
      s->str_copypostfields = curlx_strdup(ptr);
      എങ്കിൽ(!s->str_copypostfields)
        തിരികെ CURLE_OUT_OF_MEMORY;
    }
  }
  അല്ലെങ്കിൽ {
    size_t pflen = curlx_sotouz_range(s->postfieldsize, 0, SIZE_MAX);
    എങ്കിൽ(pflen == SIZE_MAX)
      തിരികെ CURLE_OUT_OF_MEMORY;
    അല്ലെങ്കിൽ {
      /* Allocate even when size == 0. This satisfies the need of possible
         later address compare to detect the COPYPOSTFIELDS mode, and to mark
         that postfields is used rather than read function or form data. */
      അക്ഷരം *p = curlx_memdup0(ptr, pflen);
      എങ്കിൽ(!p)
        തിരികെ CURLE_OUT_OF_MEMORY;
      അല്ലെങ്കിൽ {
        curlx_free(s->str_copypostfields);
        s->str_copypostfields = p;
      }
    }
  }

  s->postfields = s->str_copypostfields;
  s->method = HTTPREQ_POST;
  തിരികെ result;
}
#endif

#ifdef USE_ECH
സ്ഥിതം CURLcode setopt_ech(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *ptr)
{
  ഘടന UserDefined *s = &data->set;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!ptr || !strcmp(ptr, "false"))
    s->ssl.ech = CURLECH_DISABLE;
  അല്ലെങ്കിൽ {
    size_t plen = strlen(ptr);
    എങ്കിൽ(plen > CURL_MAX_നിവേശം_LENGTH)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    അല്ലെങ്കിൽ {
      എങ്കിൽ(!strcmp(ptr, "grease"))
        s->ssl.ech = CURLECH_GREASE;
      അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(ptr, "true"))
        s->ssl.ech = CURLECH_ENABLE;
      അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(ptr, "hard"))
        s->ssl.ech = CURLECH_HARD;
      അല്ലെങ്കിൽ എങ്കിൽ(plen > 4 && !strncmp(ptr, "ecl:", 4)) {
        എങ്കിൽ(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_CONFIG, ptr + 4);
      }
      അല്ലെങ്കിൽ എങ്കിൽ(plen > 3 && !strncmp(ptr, "pn:", 3)) {
        എങ്കിൽ(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_PUBLIC, ptr + 3);
      }
      അല്ലെങ്കിൽ
        result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  തിരികെ result;
}
#അല്ലെങ്കിൽ
#define setopt_ech(x, y) CURLE_NOT_BUILT_IN
#endif

#എങ്കിൽ defined(USE_SSL) || defined(USE_SSH)
/* One of the options is used for both TLS and SSH */
സ്ഥിതം CURLcode setopt_cptr_ssl(ഘടന Curl_easy *data, CURLoption option,
                                അക്ഷരം *ptr)
{
  CURLcode result = CURLE_OK;

  തിരഞ്ഞെടുപ്പ്(option) {
  അവസ്ഥ CURLOPT_KEYPASSWD:
    /*
     * String that holds the SSL or SSH private key password.
     */
    result = Curl_setstropt(data, STRING_താക്കോൽ_PASSWD, ptr);
    നിർത്തുക;
#ifdef USE_SSL
  അവസ്ഥ CURLOPT_CAINFO:
    /*
     * Set CA info for SSL connection. Specify filename of the CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE, ptr);
    എങ്കിൽ(!result)
      data->set.ssl.custom_cafile = TRUE;
    തിരികെ result;
  അവസ്ഥ CURLOPT_CAPATH:
    /*
     * Set CA path info for SSL connection. Specify directory name of the CA
     * certificates which have been prepared using openssl c_rehash utility.
     */
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH, ptr);
      എങ്കിൽ(!result)
        data->set.ssl.custom_capath = TRUE;
      തിരികെ result;
    }
    തിരികെ CURLE_NOT_BUILT_IN;
  അവസ്ഥ CURLOPT_CRLFILE:
    /*
     * Set CRL file info for SSL connection. Specify filename of the CRL
     * to check certificates revocation
     */
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      തിരികെ Curl_setstropt(data, STRING_SSL_CRLFILE, ptr);
    തിരികെ CURLE_NOT_BUILT_IN;
  അവസ്ഥ CURLOPT_SSL_CIPHER_LIST:
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST))
      /* set a list of cipher we want to use in the SSL connection */
      തിരികെ Curl_setstropt(data, STRING_SSL_CIPHER_LIST, ptr);
    അല്ലെങ്കിൽ
      തിരികെ CURLE_NOT_BUILT_IN;
  അവസ്ഥ CURLOPT_TLS13_CIPHERS:
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites */
      തിരികെ Curl_setstropt(data, STRING_SSL_CIPHER13_LIST, ptr);
    അല്ലെങ്കിൽ
      തിരികെ CURLE_NOT_BUILT_IN;
  അവസ്ഥ CURLOPT_RANDOM_FILE:
    നിർത്തുക;
  അവസ്ഥ CURLOPT_EGDSOCKET:
    നിർത്തുക;
  അവസ്ഥ CURLOPT_SSL_CTX_DATA:
    /*
     * Set an SSL_CTX callback parameter pointer
     */
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      data->set.ssl_fsslctxp = ptr;
      നിർത്തുക;
    }
    അല്ലെങ്കിൽ
      തിരികെ CURLE_NOT_BUILT_IN;
  അവസ്ഥ CURLOPT_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use
     */
    തിരികെ Curl_setstropt(data, STRING_CERT, ptr);
  അവസ്ഥ CURLOPT_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use
     */
    തിരികെ Curl_setstropt(data, STRING_CERT_TYPE, ptr);
  അവസ്ഥ CURLOPT_SSLKEY:
    /*
     * String that holds filename of the SSL key to use
     */
    തിരികെ Curl_setstropt(data, STRING_താക്കോൽ, ptr);
  അവസ്ഥ CURLOPT_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use
     */
    തിരികെ Curl_setstropt(data, STRING_താക്കോൽ_TYPE, ptr);
  അവസ്ഥ CURLOPT_SSLENGINE:
    /*
     * String that holds the SSL crypto engine.
     */
    എങ്കിൽ(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      തിരികെ CURLE_NOT_BUILT_IN;
    result = Curl_setstropt(data, STRING_SSL_ENGINE, ptr);
    നിർത്തുക;
  അവസ്ഥ CURLOPT_ISSUERCERT:
    /*
     * Set Issuer certificate file
     * to check certificates issuer
     */
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      തിരികെ Curl_setstropt(data, STRING_SSL_ISSUERCERT, ptr);
    തിരികെ CURLE_NOT_BUILT_IN;
  അവസ്ഥ CURLOPT_SSL_EC_CURVES:
    /*
     * Set accepted curves in SSL connection setup.
     * Specify colon-delimited list of curve algorithm names.
     */
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_SSL_EC_CURVES))
      തിരികെ Curl_setstropt(data, STRING_SSL_EC_CURVES, ptr);
    തിരികെ CURLE_NOT_BUILT_IN;
  അവസ്ഥ CURLOPT_SSL_SIGNATURE_ALGORITHMS:
    /*
     * Set accepted signature algorithms.
     * Specify colon-delimited list of signature scheme names.
     */
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_SIGNATURE_ALGORITHMS))
      തിരികെ Curl_setstropt(data, STRING_SSL_SIGNATURE_ALGORITHMS, ptr);
    തിരികെ CURLE_NOT_BUILT_IN;
  അവസ്ഥ CURLOPT_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      തിരികെ Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY, ptr);
    തിരികെ CURLE_NOT_BUILT_IN;
  അവസ്ഥ CURLOPT_ECH:
    തിരികെ setopt_ech(data, ptr);
#endif
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }
  തിരികെ result;
}
#endif

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
സ്ഥിതം CURLcode setopt_cptr_http_mqtt(ഘടന Curl_easy *data,
                                      CURLoption option, അക്ഷരം *ptr)
{
  CURLcode result = CURLE_OK;
  ഘടന UserDefined *s = &data->set;

  തിരഞ്ഞെടുപ്പ്(option) {
  അവസ്ഥ CURLOPT_COPYPOSTFIELDS:
    തിരികെ setopt_copypostfields(ptr, s);

  അവസ്ഥ CURLOPT_POSTFIELDS:
    /*
     * Like above, but use static data instead of copying it.
     */
    s->postfields = ptr;
    /* Release old copied data. */
    curlx_safefree(s->str_copypostfields);
    s->method = HTTPREQ_POST;
    നിർത്തുക;

#ifndef CURL_DISABLE_HTTP
  അവസ്ഥ CURLOPT_TRAILERDATA:
    s->trailer_data = ptr;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_ACCEPT_ENCODING:
    /*
     * String to use at the value of Accept-Encoding header.
     *
     * If the encoding is set to "" we use an Accept-Encoding header that
     * encompasses all the encodings we support.
     * If the encoding is set to NULL we do not send an Accept-Encoding header
     * and ignore any received Content-Encoding header.
     *
     */
    എങ്കിൽ(ptr && !*ptr) {
      ptr = Curl_get_content_encodings();
      എങ്കിൽ(ptr)
        result = CURL_EASY_STR_SETN(data, STRING_ENCODING, ptr);
      അല്ലെങ്കിൽ
        result = CURLE_OUT_OF_MEMORY;
      തിരികെ result;
    }
    തിരികെ Curl_setstropt(data, STRING_ENCODING, ptr);

#ifndef CURL_DISABLE_AWS
  അവസ്ഥ CURLOPT_AWS_SIGV4:
    /*
     * String that is merged to some authentication
     * parameters are used by the algorithm.
     */
    result = Curl_setstropt(data, STRING_AWS_SIGV4, ptr);
    /*
     * Basic has been set by default; it needs to be unset here.
     */
    എങ്കിൽ(CURL_EASY_STR(data, STRING_AWS_SIGV4))
      s->httpauth = CURLAUTH_AWS_SIGV4;
    അല്ലെങ്കിൽ
      s->httpauth &= ~(uint32_t)CURLAUTH_AWS_SIGV4;
    നിർത്തുക;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  അവസ്ഥ CURLOPT_HTTPSIG_താക്കോൽ:
    result = Curl_setstropt(data, STRING_HTTPSIG_താക്കോൽ, ptr);
    നിർത്തുക;
  അവസ്ഥ CURLOPT_HTTPSIG_KEYID:
    result = Curl_setstropt(data, STRING_HTTPSIG_KEYID, ptr);
    നിർത്തുക;
  അവസ്ഥ CURLOPT_HTTPSIG_HEADERS:
    result = Curl_setstropt(data, STRING_HTTPSIG_HEADERS, ptr);
    നിർത്തുക;
#endif
  അവസ്ഥ CURLOPT_REFERER: {
    /*
     * String to set in the HTTP Referer: field.
     */
    ഘടന bufref *oldref = &data->state.referer;
    /* free the old after the storing the new in case the input is actually
       pointing back to this */
    result = Curl_setstropt(data, STRING_SET_REFERER, ptr);
    Curl_bufref_free(oldref);
    നിർത്തുക;
  }

  അവസ്ഥ CURLOPT_USERAGENT:
    /*
     * String to use in the HTTP User-Agent field
     */
    തിരികെ Curl_setstropt(data, STRING_USERAGENT, ptr);

#ifndef CURL_DISABLE_COOKIES
  അവസ്ഥ CURLOPT_COOKIE:
    /*
     * Cookie string to send to the remote server in the request.
     */
    തിരികെ Curl_setstropt(data, STRING_COOKIE, ptr);

  അവസ്ഥ CURLOPT_COOKIEFILE:
    തിരികെ cookiefile(data, ptr);

  അവസ്ഥ CURLOPT_COOKIEJAR:
    /*
     * Set cookie filename to dump all cookies to when we are done.
     */
    result = Curl_setstropt(data, STRING_COOKIEJAR, ptr);
    എങ്കിൽ(!result) {
      /*
       * Activate the cookie parser. This may or may not already
       * have been made.
       */
      എങ്കിൽ(!data->cookies)
        data->cookies = Curl_cookie_init();
      എങ്കിൽ(!data->cookies)
        result = CURLE_OUT_OF_MEMORY;
      അല്ലെങ്കിൽ
        data->state.cookie_engine = TRUE;
    }
    നിർത്തുക;

  അവസ്ഥ CURLOPT_COOKIELIST:
    തിരികെ cookielist(data, ptr);
#endif /* !CURL_DISABLE_COOKIES */

#endif /* !CURL_DISABLE_HTTP */
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }
  തിരികെ result;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_MQTT */

#ifdef USE_SSH
സ്ഥിതം CURLcode setopt_cptr_ssh(ഘടന Curl_easy *data, CURLoption option,
                                അക്ഷരം *ptr)
{
  ഘടന UserDefined *s = &data->set;
  തിരഞ്ഞെടുപ്പ്(option) {
  അവസ്ഥ CURLOPT_SSH_PUBLIC_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa.pub file
     */
    തിരികെ Curl_setstropt(data, STRING_SSH_PUBLIC_താക്കോൽ, ptr);
  അവസ്ഥ CURLOPT_SSH_PRIVATE_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa file
     */
    തിരികെ Curl_setstropt(data, STRING_SSH_PRIVATE_താക്കോൽ, ptr);
  അവസ്ഥ CURLOPT_SSH_KEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_keyfunc_userp = ptr;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_SSH_HOST_PUBLIC_താക്കോൽ_MD5:
    /*
     * Option to allow for the MD5 of the host public key to be checked
     * for validation purposes.
     */
    തിരികെ Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_താക്കോൽ_MD5, ptr);
  അവസ്ഥ CURLOPT_SSH_HOST_PUBLIC_താക്കോൽ_SHA256:
    /*
     * Option to allow for the SHA256 of the host public key to be checked
     * for validation purposes.
     */
    തിരികെ Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_താക്കോൽ_SHA256, ptr);
  അവസ്ഥ CURLOPT_SSH_KNOWNHOSTS:
    /*
     * Store the filename to read known hosts from.
     */
    തിരികെ Curl_setstropt(data, STRING_SSH_KNOWNHOSTS, ptr);
#ifdef USE_LIBSSH2
  അവസ്ഥ CURLOPT_SSH_HOSTKEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_hostkeyfunc_userp = ptr;
    നിർത്തുക;
#endif /* USE_LIBSSH2 */
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }
  തിരികെ CURLE_OK;
}
#endif /* USE_SSH */

#ifndef CURL_DISABLE_FTP
സ്ഥിതം CURLcode setopt_cptr_ftp(ഘടന Curl_easy *data, CURLoption option,
                                അക്ഷരം *ptr)
{
  CURLcode result = CURLE_OK;
  ഘടന UserDefined *s = &data->set;
  തിരഞ്ഞെടുപ്പ്(option) {
  അവസ്ഥ CURLOPT_FTPPORT:
    /*
     * Use FTP PORT, this also specifies which IP address to use
     */
    result = Curl_setstropt(data, STRING_FTPPORT, ptr);
    s->ftp_use_port = !!CURL_EASY_STR(data, STRING_FTPPORT);
    നിർത്തുക;

  അവസ്ഥ CURLOPT_FTP_ACCOUNT:
    തിരികെ Curl_setstropt(data, STRING_FTP_ACCOUNT, ptr);

  അവസ്ഥ CURLOPT_FTP_ALTERNATIVE_TO_USER:
    തിരികെ Curl_setstropt(data, STRING_FTP_ALTERNATIVE_TO_USER, ptr);

  അവസ്ഥ CURLOPT_KRBLEVEL:
    തിരികെ CURLE_NOT_BUILT_IN; /* removed in 8.17.0 */
  അവസ്ഥ CURLOPT_CHUNK_DATA:
    s->wildcardptr = ptr;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_FNMATCH_DATA:
    s->fnmatch_data = ptr;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }
  തിരികെ result;
}
#endif /* !CURL_DISABLE_FTP */

സ്ഥിതം CURLcode setopt_cptr_net(ഘടന Curl_easy *data, CURLoption option,
                                അക്ഷരം *ptr)
{
  തിരഞ്ഞെടുപ്പ്(option) {
  അവസ്ഥ CURLOPT_INTERFACE:
    /*
     * Set what interface or address/hostname to bind the socket to when
     * performing an operation and thus what from-IP your connection will use.
     */
    തിരികെ setstropt_interface(data, ptr);

#ifdef USE_RESOLV_ARES
  അവസ്ഥ CURLOPT_DNS_SERVERS:
    തിരികെ Curl_setstropt(data, STRING_DNS_SERVERS, ptr);

  അവസ്ഥ CURLOPT_DNS_INTERFACE:
    തിരികെ Curl_setstropt(data, STRING_DNS_INTERFACE, ptr);

  അവസ്ഥ CURLOPT_DNS_LOCAL_IP4:
    തിരികെ Curl_setstropt(data, STRING_DNS_LOCAL_IP4, ptr);

  അവസ്ഥ CURLOPT_DNS_LOCAL_IP6:
    തിരികെ Curl_setstropt(data, STRING_DNS_LOCAL_IP6, ptr);
#endif
#ifdef USE_UNIX_SOCKETS
  അവസ്ഥ CURLOPT_UNIX_SOCKET_PATH:
    data->set.abstract_unix_socket = FALSE;
    തിരികെ Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);

  അവസ്ഥ CURLOPT_ABSTRACT_UNIX_SOCKET:
    data->set.abstract_unix_socket = TRUE;
    തിരികെ Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);
#endif
#ifndef CURL_DISABLE_DOH
  അവസ്ഥ CURLOPT_DOH_URL:
    {
      CURLcode result = Curl_setstropt(data, STRING_DOH, ptr);
      data->set.doh = !!CURL_EASY_STR(data, STRING_DOH);
      തിരികെ result;
    }
#endif
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }
}

സ്ഥിതം CURLcode setopt_cptr_misc(ഘടന Curl_easy *data, CURLoption option,
                                 അക്ഷരം *ptr)
{
  CURLcode result = CURLE_OK;
  ഘടന UserDefined *s = &data->set;

  തിരഞ്ഞെടുപ്പ്(option) {
  അവസ്ഥ CURLOPT_REQUEST_TARGET:
    തിരികെ Curl_setstropt(data, STRING_TARGET, ptr);
#ifndef CURL_DISABLE_NETRC
  അവസ്ഥ CURLOPT_NETRC_FILE:
    തിരികെ Curl_setstropt(data, STRING_NETRC_FILE, ptr);
#endif
  അവസ്ഥ CURLOPT_CUSTOMREQUEST:
    തിരികെ Curl_setstropt(data, STRING_CUSTOMREQUEST, ptr);

    /* we do not set s->method = HTTPREQ_CUSTOM; here, we continue as if we
       were using the already set type and this changes the actual request
       keyword */
  അവസ്ഥ CURLOPT_SERVICE_NAME:
    തിരികെ Curl_setstropt(data, STRING_SERVICE_NAME, ptr);

  അവസ്ഥ CURLOPT_HEADERDATA:
    s->writeheader = ptr;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_READDATA:
    s->in_set = ptr;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_WRITEDATA:
    s->out = ptr;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_DEBUGDATA:
    s->debugdata = ptr;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_PROGRESSDATA:
    s->progress_client = ptr;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_SEEKDATA:
    s->seek_client = ptr;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_IOCTLDATA:
    s->ioctl_client = ptr;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_SOCKOPTDATA:
    s->sockopt_client = ptr;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_OPENSOCKETDATA:
    s->opensocket_client = ptr;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_RESOLVER_START_DATA:
    s->resolver_start_client = ptr;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_CLOSESOCKETDATA:
    s->closesocket_client = ptr;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_PREREQDATA:
    s->prereq_userp = ptr;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_ERRORBUFFER:
    s->errorbuffer = ptr;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_URL:
    result = Curl_setstropt(data, STRING_SET_URL, ptr);
    Curl_bufref_set(&data->state.url,
                    CURL_EASY_STR(data, STRING_SET_URL), 0, ശൂന്യ);
    നിർത്തുക;

  അവസ്ഥ CURLOPT_USERPWD: {
    അക്ഷരം *u = ശൂന്യ, *p = ശൂന്യ;
    result = setstropt_userpwd(ptr, &u, &p);
    എങ്കിൽ(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_USERNAME, u);
      u = ശൂന്യ;
    }
    എങ്കിൽ(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_PASSWORD, p);
      p = ശൂന്യ;
    }
    curlx_free(u);
    curlx_free(p);
    തിരികെ result;
  }

  അവസ്ഥ CURLOPT_USERNAME:
    തിരികെ Curl_setstropt(data, STRING_USERNAME, ptr);

  അവസ്ഥ CURLOPT_PASSWORD:
    തിരികെ Curl_setstropt(data, STRING_PASSWORD, ptr);

  അവസ്ഥ CURLOPT_LOGIN_OPTIONS:
    തിരികെ Curl_setstropt(data, STRING_OPTIONS, ptr);

  അവസ്ഥ CURLOPT_XOAUTH2_BEARER:
    തിരികെ Curl_setstropt(data, STRING_BEARER, ptr);
  അവസ്ഥ CURLOPT_RANGE:
    തിരികെ Curl_setstropt(data, STRING_SET_RANGE, ptr);
  അവസ്ഥ CURLOPT_PRIVATE:
    s->private_data = ptr;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_PROTOCOLS_STR:
    എങ്കിൽ(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      എങ്കിൽ(!result)
        s->allowed_protocols = protos;
    }
    അല്ലെങ്കിൽ
      /* make a NULL argument reset to default */
      s->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_REDIR_PROTOCOLS_STR:
    എങ്കിൽ(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      എങ്കിൽ(!result)
        s->redir_protocols = protos;
    }
    അല്ലെങ്കിൽ
      /* make a NULL argument reset to default */
      s->redir_protocols = (curl_prot_t)CURLPROTO_REDIR;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_DEFAULT_PROTOCOL:
    /* Set the protocol to use when the URL does not include any protocol */
    തിരികെ Curl_setstropt(data, STRING_DEFAULT_PROTOCOL, ptr);
#ifndef CURL_DISABLE_SMTP
  അവസ്ഥ CURLOPT_MAIL_FROM:
    /* Set the SMTP mail originator */
    തിരികെ Curl_setstropt(data, STRING_MAIL_FROM, ptr);
  അവസ്ഥ CURLOPT_MAIL_AUTH:
    /* Set the SMTP auth originator */
    തിരികെ Curl_setstropt(data, STRING_MAIL_AUTH, ptr);
#endif
  അവസ്ഥ CURLOPT_SASL_AUTHZID:
    /* Authorization identity (identity to act as) */
    തിരികെ Curl_setstropt(data, STRING_SASL_AUTHZID, ptr);
#ifndef CURL_DISABLE_RTSP
  അവസ്ഥ CURLOPT_RTSP_SESSION_ID:
    Curl_peer_unlink(&data->state.rtsp_session_origin);
    തിരികെ Curl_setstropt(data, STRING_RTSP_SESSION_ID, ptr);
  അവസ്ഥ CURLOPT_RTSP_STREAM_URI:
    തിരികെ Curl_setstropt(data, STRING_RTSP_STREAM_URI, ptr);
  അവസ്ഥ CURLOPT_RTSP_TRANSPORT:
    തിരികെ Curl_setstropt(data, STRING_RTSP_TRANSPORT, ptr);
  അവസ്ഥ CURLOPT_INTERLEAVEDATA:
    s->rtp_out = ptr;
    നിർത്തുക;
#endif /* !CURL_DISABLE_RTSP */
  അവസ്ഥ CURLOPT_TLSAUTH_USERNAME:
  അവസ്ഥ CURLOPT_TLSAUTH_PASSWORD:
  അവസ്ഥ CURLOPT_TLSAUTH_TYPE:
  അവസ്ഥ CURLOPT_PROXY_TLSAUTH_USERNAME:
  അവസ്ഥ CURLOPT_PROXY_TLSAUTH_PASSWORD:
  അവസ്ഥ CURLOPT_PROXY_TLSAUTH_TYPE:
    തിരികെ CURLE_NOT_BUILT_IN;
#ifndef CURL_DISABLE_HSTS
  അവസ്ഥ CURLOPT_HSTSREADDATA:
    s->hsts_read_userp = ptr;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_HSTSWRITEDATA:
    s->hsts_write_userp = ptr;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_HSTS: {
    ഘടന curl_slist *h;
    എങ്കിൽ(!data->hsts) {
      data->hsts = Curl_hsts_init();
      എങ്കിൽ(!data->hsts)
        തിരികെ CURLE_OUT_OF_MEMORY;
    }
    എങ്കിൽ(ptr) {
      result = Curl_setstropt(data, STRING_HSTS, ptr);
      എങ്കിൽ(result)
        തിരികെ result;
      /* this needs to build a list of filenames to read from, so that it can
         read them later, as we might get a shared HSTS handle to load them
         into */
      h = curl_slist_append(data->state.hstslist, ptr);
      എങ്കിൽ(!h) {
        curl_slist_free_all(data->state.hstslist);
        data->state.hstslist = ശൂന്യ;
        തിരികെ CURLE_OUT_OF_MEMORY;
      }
      data->state.hstslist = h; /* store the list for later use */
    }
    അല്ലെങ്കിൽ {
      /* clear the list of HSTS files */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = ശൂന്യ;
      എങ്കിൽ(!data->share || !data->share->hsts)
        /* throw away the HSTS cache unless shared */
        Curl_hsts_cleanup(&data->hsts);
    }
    നിർത്തുക;
  }
#endif /* !CURL_DISABLE_HSTS */
#ifndef CURL_DISABLE_ALTSVC
  അവസ്ഥ CURLOPT_ALTSVC:
    എങ്കിൽ(!data->asi) {
      data->asi = Curl_altsvc_init();
      എങ്കിൽ(!data->asi)
        തിരികെ CURLE_OUT_OF_MEMORY;
    }
    result = Curl_setstropt(data, STRING_ALTSVC, ptr);
    എങ്കിൽ(result)
      നിർത്തുക;
    എങ്കിൽ(ptr)
      തിരികെ Curl_altsvc_load(data->asi, ptr);
    നിർത്തുക;
#endif /* !CURL_DISABLE_ALTSVC */
  അവസ്ഥ CURLOPT_ECH:
    തിരികെ setopt_ech(data, ptr);
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode setopt_cptr(ഘടന Curl_easy *data, CURLoption option,
                            അക്ഷരം *ptr)
{
  തരനിർവചനം CURLcode (*ptrfunc)(ഘടന Curl_easy *data, CURLoption option,
                              അക്ഷരം *ptr);
  /* Order by likeliness */
  സ്ഥിതം സ്ഥിരം ptrfunc setopt_call[] = {
    setopt_cptr_misc,
#എങ്കിൽ defined(USE_SSL) || defined(USE_SSH)
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
#എങ്കിൽ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
    setopt_cptr_http_mqtt,
#endif
  };
  size_t i;

  വേണ്ടി(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, ptr);
    എങ്കിൽ(result != CURLE_UNKNOWN_OPTION)
      തിരികെ result;
  }
  തിരികെ CURLE_UNKNOWN_OPTION;
}

സ്ഥിതം CURLcode setopt_func(ഘടന Curl_easy *data, CURLoption option,
                            va_list param)
{
  ഘടന UserDefined *s = &data->set;
  തിരഞ്ഞെടുപ്പ്(option) {
  അവസ്ഥ CURLOPT_PROGRESSFUNCTION:
    /*
     * Progress callback function
     */
    s->fprogress = va_arg(param, curl_progress_callback);
    എങ്കിൽ(s->fprogress)
      data->progress.callback = TRUE; /* no longer internal */
    അല്ലെങ്കിൽ
      data->progress.callback = FALSE; /* NULL enforces internal */
    നിർത്തുക;

  അവസ്ഥ CURLOPT_XFERINFOFUNCTION:
    /*
     * Transfer info callback function
     */
    s->fxferinfo = va_arg(param, curl_xferinfo_callback);
    എങ്കിൽ(s->fxferinfo)
      data->progress.callback = TRUE; /* no longer internal */
    അല്ലെങ്കിൽ
      data->progress.callback = FALSE; /* NULL enforces internal */

    നിർത്തുക;
  അവസ്ഥ CURLOPT_DEBUGFUNCTION:
    /*
     * stderr write callback.
     */
    s->fdebug = va_arg(param, curl_debug_callback);
    /*
     * if the callback provided is NULL, it will use the default callback
     */
    നിർത്തുക;
  അവസ്ഥ CURLOPT_HEADERFUNCTION:
    /*
     * Set header write callback
     */
    s->fwrite_header = va_arg(param, curl_write_callback);
    നിർത്തുക;
  അവസ്ഥ CURLOPT_WRITEFUNCTION:
    /*
     * Set data write callback
     */
    s->fwrite_func = va_arg(param, curl_write_callback);
    എങ്കിൽ(!s->fwrite_func)
#എങ്കിൽ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic തള്ളുക
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fwrite_func = (curl_write_callback)fwrite;
#എങ്കിൽ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic എടുക്കുക
#endif
    നിർത്തുക;
  അവസ്ഥ CURLOPT_READFUNCTION:
    /*
     * Read data callback
     */
    s->fread_func_set = va_arg(param, curl_read_callback);
    എങ്കിൽ(!s->fread_func_set) {
      s->is_fread_set = 0;
#എങ്കിൽ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic തള്ളുക
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fread_func_set = (curl_read_callback)fread;
#എങ്കിൽ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic എടുക്കുക
#endif
    }
    അല്ലെങ്കിൽ
      s->is_fread_set = 1;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_SEEKFUNCTION:
    /*
     * Seek callback. Might be NULL.
     */
    s->seek_func = va_arg(param, curl_seek_callback);
    നിർത്തുക;
  അവസ്ഥ CURLOPT_IOCTLFUNCTION:
    /*
     * I/O control callback. Might be NULL.
     */
    s->ioctl_func = va_arg(param, curl_ioctl_callback);
    നിർത്തുക;
  അവസ്ഥ CURLOPT_SSL_CTX_FUNCTION:
    /*
     * Set an SSL_CTX callback
     */
#ifdef USE_SSL
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      s->ssl_fsslctx = va_arg(param, curl_ssl_ctx_callback);
      നിർത്തുക;
    }
    അല്ലെങ്കിൽ
#endif
      തിരികെ CURLE_NOT_BUILT_IN;

  അവസ്ഥ CURLOPT_SOCKOPTFUNCTION:
    /*
     * socket callback function: called after socket() but before connect()
     */
    s->fsockopt = va_arg(param, curl_sockopt_callback);
    നിർത്തുക;

  അവസ്ഥ CURLOPT_OPENSOCKETFUNCTION:
    /*
     * open/create socket callback function: called instead of socket(),
     * before connect()
     */
    s->fopensocket = va_arg(param, curl_opensocket_callback);
    നിർത്തുക;

  അവസ്ഥ CURLOPT_CLOSESOCKETFUNCTION:
    /*
     * close socket callback function: called instead of close()
     * when shutting down a connection
     */
    s->fclosesocket = va_arg(param, curl_closesocket_callback);
    നിർത്തുക;

  അവസ്ഥ CURLOPT_RESOLVER_START_FUNCTION:
    /*
     * resolver start callback function: called before a new resolver request
     * is started
     */
    s->resolver_start = va_arg(param, curl_resolver_start_callback);
    നിർത്തുക;

#ifdef USE_SSH
#ifdef USE_LIBSSH2
  അവസ്ഥ CURLOPT_SSH_HOSTKEYFUNCTION:
    /* the callback to check the hostkey without the knownhost file */
    s->ssh_hostkeyfunc = va_arg(param, curl_sshhostkeycallback);
    നിർത്തുക;
#endif

  അവസ്ഥ CURLOPT_SSH_KEYFUNCTION:
    /* setting to NULL is fine since the ssh.c functions themselves will
       then revert to use the internal default */
    s->ssh_keyfunc = va_arg(param, curl_sshkeycallback);
    നിർത്തുക;

#endif /* USE_SSH */

#ifndef CURL_DISABLE_RTSP
  അവസ്ഥ CURLOPT_INTERLEAVEFUNCTION:
    /* Set the user defined RTP write function */
    s->fwrite_rtp = va_arg(param, curl_write_callback);
    നിർത്തുക;
#endif
#ifndef CURL_DISABLE_FTP
  അവസ്ഥ CURLOPT_CHUNK_BGN_FUNCTION:
    s->chunk_bgn = va_arg(param, curl_chunk_bgn_callback);
    നിർത്തുക;
  അവസ്ഥ CURLOPT_CHUNK_END_FUNCTION:
    s->chunk_end = va_arg(param, curl_chunk_end_callback);
    നിർത്തുക;
  അവസ്ഥ CURLOPT_FNMATCH_FUNCTION:
    s->fnmatch = va_arg(param, curl_fnmatch_callback);
    നിർത്തുക;
#endif
#ifndef CURL_DISABLE_HTTP
  അവസ്ഥ CURLOPT_TRAILERFUNCTION:
    s->trailer_callback = va_arg(param, curl_trailer_callback);
    നിർത്തുക;
#endif
#ifndef CURL_DISABLE_HSTS
  അവസ്ഥ CURLOPT_HSTSREADFUNCTION:
    s->hsts_read = va_arg(param, curl_hstsread_callback);
    നിർത്തുക;
  അവസ്ഥ CURLOPT_HSTSWRITEFUNCTION:
    s->hsts_write = va_arg(param, curl_hstswrite_callback);
    നിർത്തുക;
#endif
  അവസ്ഥ CURLOPT_PREREQFUNCTION:
    s->fprereq = va_arg(param, curl_prereq_callback);
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode setopt_offt(ഘടന Curl_easy *data, CURLoption option,
                            curl_off_t offt)
{
  ഘടന UserDefined *s = &data->set;
  തിരഞ്ഞെടുപ്പ്(option) {
  അവസ്ഥ CURLOPT_TIMEVALUE_LARGE:
    /*
     * This is the value to compare with the remote document with the
     * method set with CURLOPT_TIMECONDITION
     */
    s->timevalue = (time_t)offt;
    നിർത്തുക;

    /* MQTT "borrows" some of the HTTP options */
  അവസ്ഥ CURLOPT_POSTFIELDSIZE_LARGE:
    /*
     * The size of the POSTFIELD data to prevent libcurl to do strlen() to
     * figure it out. Enables binary posts.
     */
    എങ്കിൽ(offt < -1)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

    എങ്കിൽ(s->postfieldsize < offt && s->str_copypostfields) {
      /* Previous CURLOPT_COPYPOSTFIELDS is no longer valid. */
      curlx_safefree(s->str_copypostfields);
      s->postfields = ശൂന്യ;
    }
    s->postfieldsize = offt;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_INFILESIZE_LARGE:
    /*
     * If known, this should inform curl about the file size of the
     * to-be-uploaded file.
     */
    എങ്കിൽ(offt < -1)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = offt;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_MAX_SEND_SPEED_LARGE:
    /*
     * When transfer uploads are faster than CURLOPT_MAX_SEND_SPEED_LARGE
     * bytes per second the transfer is throttled..
     */
    എങ്കിൽ(offt < 0)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_send_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.ul.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    നിർത്തുക;
  അവസ്ഥ CURLOPT_MAX_RECV_SPEED_LARGE:
    /*
     * When receiving data faster than CURLOPT_MAX_RECV_SPEED_LARGE bytes per
     * second the transfer is throttled..
     */
    എങ്കിൽ(offt < 0)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_recv_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.dl.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    നിർത്തുക;
  അവസ്ഥ CURLOPT_RESUME_FROM_LARGE:
    /*
     * Resume transfer at the given file position
     */
    എങ്കിൽ(offt < -1)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = offt;
    നിർത്തുക;
  അവസ്ഥ CURLOPT_MAXFILESIZE_LARGE:
    /*
     * Set the maximum size of a file to download.
     */
    എങ്കിൽ(offt < 0)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_filesize = offt;
    നിർത്തുക;

  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode setopt_blob(ഘടന Curl_easy *data, CURLoption option,
                            ഘടന curl_blob *blob)
{
  ഘടന UserDefined *s = &data->set;
  തിരഞ്ഞെടുപ്പ്(option) {
  അവസ്ഥ CURLOPT_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use
     */
    തിരികെ Curl_setblobopt(&s->blobs[BLOB_CERT], blob);
#ifndef CURL_DISABLE_PROXY
  അവസ്ഥ CURLOPT_PROXY_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use for proxy
     */
    തിരികെ Curl_setblobopt(&s->blobs[BLOB_CERT_PROXY], blob);
  അവസ്ഥ CURLOPT_PROXY_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use for proxy
     */
    തിരികെ Curl_setblobopt(&s->blobs[BLOB_താക്കോൽ_PROXY], blob);
  അവസ്ഥ CURLOPT_PROXY_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection proxy.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO_PROXY], blob);
      എങ്കിൽ(!result)
        s->proxy_ssl.custom_cablob = TRUE;
      തിരികെ result;
    }
#endif
    തിരികെ CURLE_NOT_BUILT_IN;
  അവസ്ഥ CURLOPT_PROXY_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      തിരികെ Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT_PROXY], blob);
    തിരികെ CURLE_NOT_BUILT_IN;
#endif
  അവസ്ഥ CURLOPT_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use
     */
    തിരികെ Curl_setblobopt(&s->blobs[BLOB_താക്കോൽ], blob);
  അവസ്ഥ CURLOPT_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO], blob);
      എങ്കിൽ(!result)
        s->ssl.custom_cablob = TRUE;
      തിരികെ result;
    }
#endif
    തിരികെ CURLE_NOT_BUILT_IN;
  അവസ്ഥ CURLOPT_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    എങ്കിൽ(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      തിരികെ Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT], blob);
    തിരികെ CURLE_NOT_BUILT_IN;

  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }
  /* unreachable */
}

/*
 * Do not make Curl_vsetopt() static: it is called from
 * projects/OS400/ccsidcurl.c.
 */
CURLcode Curl_vsetopt(ഘടന Curl_easy *data, CURLoption option, va_list param)
{
  എങ്കിൽ(option < CURLOPTTYPE_OBJECTPOINT)
    തിരികെ setopt_long(data, option, va_arg(param, long));
  അല്ലെങ്കിൽ എങ്കിൽ(option < CURLOPTTYPE_FUNCTIONPOINT) {
    /* unfortunately, different pointer types cannot be identified any other
       way than being listed explicitly */
    തിരഞ്ഞെടുപ്പ്(option) {
    അവസ്ഥ CURLOPT_HTTPHEADER:
    അവസ്ഥ CURLOPT_QUOTE:
    അവസ്ഥ CURLOPT_POSTQUOTE:
    അവസ്ഥ CURLOPT_TELNETOPTIONS:
    അവസ്ഥ CURLOPT_PREQUOTE:
    അവസ്ഥ CURLOPT_HTTP200ALIASES:
    അവസ്ഥ CURLOPT_MAIL_RCPT:
    അവസ്ഥ CURLOPT_RESOLVE:
    അവസ്ഥ CURLOPT_PROXYHEADER:
    അവസ്ഥ CURLOPT_CONNECT_TO:
      തിരികെ setopt_slist(data, option, va_arg(param, ഘടന curl_slist *));
    അവസ്ഥ CURLOPT_HTTPPOST:         /* curl_httppost * */
    അവസ്ഥ CURLOPT_MIMEPOST:         /* curl_mime * */
    അവസ്ഥ CURLOPT_STDERR:           /* FILE * */
    അവസ്ഥ CURLOPT_SHARE:            /* CURLSH * */
    അവസ്ഥ CURLOPT_CURLU:            /* CURLU * */
      തിരികെ setopt_pointers(data, option, param);
    അവസ്ഥ CURLOPT_STREAM_DEPENDS:   /* CURL * */
    അവസ്ഥ CURLOPT_STREAM_DEPENDS_E: /* CURL * */
      തിരികെ CURLE_OK;
    സ്ഥിരസ്ഥിതി:
      നിർത്തുക;
    }
    /* the char pointer options */
    തിരികെ setopt_cptr(data, option, va_arg(param, അക്ഷരം *));
  }
  അല്ലെങ്കിൽ എങ്കിൽ(option < CURLOPTTYPE_OFF_T)
    തിരികെ setopt_func(data, option, param);
  അല്ലെങ്കിൽ എങ്കിൽ(option < CURLOPTTYPE_BLOB)
    തിരികെ setopt_offt(data, option, va_arg(param, curl_off_t));
  തിരികെ setopt_blob(data, option, va_arg(param, ഘടന curl_blob *));
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
  ഘടന Curl_eapi_guard guard;
  CURLcode result;

  എങ്കിൽ(CURL_EAPI_ENTER(&guard, curl, easy_setopt, &result)) {
    ഘടന Curl_easy *data = curl;
    va_list arg;

    va_start(arg, option);

    result = Curl_vsetopt(data, option, arg);

    va_end(arg);
    എങ്കിൽ(result == CURLE_BAD_FUNCTION_ARGUMENT)
      failf(data, "setopt 0x%x got bad argument", (unsigned പൂർണ്ണസംഖ്യ)option);
  }
  CURL_EAPI_LEAVE(&guard);
  തിരികെ result;
}
