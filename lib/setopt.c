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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#ಅನ್ತರ್ಭಾವ <linux/tcp.h>
#ನೋಚೇದ್ಯದಿ defined(HAVE_NETINET_TCP_H)
#ಅನ್ತರ್ಭಾವ <netinet/tcp.h>
#endif

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "content_encoding.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "curl_share.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "setopt.h"
#ಅನ್ತರ್ಭಾವ "altsvc.h"
#ಅನ್ತರ್ಭಾವ "hsts.h"
#ಅನ್ತರ್ಭಾವ "tftp.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"

ಸ್ಥಿರ CURLcode setopt_set_timeout_sec(timediff_t *ptimeout_ms, long secs)
{
  ಯದಿ(secs < 0)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
#ಯದಿ LONG_MAX > (TIMEDIFF_T_MAX / 1000)
  ಯದಿ(secs > (TIMEDIFF_T_MAX / 1000)) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)secs * 1000;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode setopt_set_timeout_ms(timediff_t *ptimeout_ms, long ms)
{
  ಯದಿ(ms < 0)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
#ಯದಿ LONG_MAX > TIMEDIFF_T_MAX
  ಯದಿ(ms > TIMEDIFF_T_MAX) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)ms;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_setstropt(ಸಂರಚನಾ Curl_easy *data,
                        ಗಣನಾ dupstring id, ಧ್ರುವ ವರ್ಣ *s)
{
  size_t slen = s ? strlen(s) : 0;
  DEBUGASSERT((unsigned)id <= UINT8_MAX);
  ಯದಿ(s && (slen > CURL_MAX_ಒಳಹರಿವು_LENGTH))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  ಪ್ರತ್ಯಾಗಮ CURL_EASY_STR_SET(data, (uint8_t)id, s, slen);
}

CURLcode Curl_setblobopt(ಸಂರಚನಾ curl_blob **blobp,
                         ಧ್ರುವ ಸಂರಚನಾ curl_blob *blob)
{
  /* free the previous storage at `blobp' and replace by a dynamic storage
     copy of blob. If CURL_BLOB_COPY is set, the data is copied. */

  curlx_safefree(*blobp);

  ಯದಿ(blob) {
    ಸಂರಚನಾ curl_blob *nblob;
    ಯದಿ(!blob->data || !blob->len || (blob->len > CURL_MAX_ಒಳಹರಿವು_LENGTH))
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    nblob = (ಸಂರಚನಾ curl_blob *)
      curlx_malloc(ಪರಿಮಾಣ(ಸಂರಚನಾ curl_blob) +
                   ((blob->flags & CURL_BLOB_COPY) ? blob->len : 0));
    ಯದಿ(!nblob)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    *nblob = *blob;
    ಯದಿ(blob->flags & CURL_BLOB_COPY) {
      /* put the data after the blob struct in memory */
      nblob->data = (ವರ್ಣ *)nblob + ಪರಿಮಾಣ(ಸಂರಚನಾ curl_blob);
      memcpy(nblob->data, blob->data, blob->len);
    }

    *blobp = nblob;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode setstropt_userpwd(ಧ್ರುವ ವರ್ಣ *option, ವರ್ಣ **userp,
                                  ವರ್ಣ **passwdp)
{
  ವರ್ಣ *user = ರಿಕ್ತ;
  ವರ್ಣ *passwd = ರಿಕ್ತ;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Parse the login details if specified. If not, then we treat NULL as a
     hint to clear the existing data */
  ಯದಿ(option) {
    size_t len = strlen(option);
    CURLcode result;
    ಯದಿ(len > CURL_MAX_ಒಳಹರಿವು_LENGTH)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

    result = Curl_parse_login_details(option, len, &user, &passwd, ರಿಕ್ತ);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  curlx_free(*userp);
  *userp = user;

  curlx_strzero(*passwdp);
  curlx_free(*passwdp);
  *passwdp = passwd;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode setstropt_interface(ಸಂರಚನಾ Curl_easy *data, ವರ್ಣ *option)
{
  ಯದಿ(option && !option[0])
    option = ರಿಕ್ತ;
  ಯದಿ(option) {
    /* Parse for checking syntax */
    CURLcode result = Curl_parse_interface(option, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    ಪ್ರತ್ಯಾಗಮ CURL_EASY_STR_SET(data, STRING_INTERFACE_OPT,
                             option, strlen(option));
  }
  CURL_EASY_STR_CLEAR(data, STRING_INTERFACE_OPT);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ifdef USE_SSL
#define C_SSLVERSION_ಮೌಲ್ಯ(x)     ((x) & 0xffff)
#define C_SSLVERSION_MAX_ಮೌಲ್ಯ(x) ((unsigned long)(x) & 0xffff0000)
#endif

ಸ್ಥಿರ CURLcode protocol2num(ಧ್ರುವ ವರ್ಣ *str, curl_prot_t *val)
{
  /*
   * We are asked to cherry-pick protocols, so play it safe and disallow all
   * protocols to start with, and re-add the wanted ones back in.
   */
  *val = 0;

  ಯದಿ(!str)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  ಯದಿ(curl_strequal(str, "all")) {
    *val = ~(curl_prot_t)0;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  ಕುರು {
    ಧ್ರುವ ವರ್ಣ *token = str;
    size_t tlen;

    str = strchr(str, ',');
    tlen = str ? (size_t)(str - token) : strlen(token);
    ಯದಿ(tlen) {
      ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *h = Curl_getn_scheme(token, tlen);

      ಯದಿ(!h || !h->run)
        ಪ್ರತ್ಯಾಗಮ CURLE_UNSUPPORTED_PROTOCOL;

      *val |= h->protocol;
    }
  } ಯಾವತ್(str && str++);

  ಯದಿ(!*val)
    /* no protocol listed */
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ಯದಿ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
ಸ್ಥಿರ CURLcode httpauth(ಸಂರಚನಾ Curl_easy *data, bool proxy,
                         unsigned long auth)
{
  ಯದಿ(auth != CURLAUTH_NONE) {
    ಪೂರ್ಣಾಙ್ಕ bitcheck = 0;
    bool authbits = FALSE;
    ಯದಿ(auth & CURLAUTH_DIGEST_IE) {
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
    ಯಾವತ್(bitcheck < 31) {
      ಯದಿ(auth & (1UL << bitcheck++)) {
        authbits = TRUE;
        ಭಙ್ಗ;
      }
    }
    ಯದಿ(!authbits)
      ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN; /* no supported types left! */
  }
  ಯದಿ(proxy)
    data->set.proxyauth = (uint32_t)auth;
  ಅನ್ಯಥಾ
    data->set.httpauth = (uint32_t)auth;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */

#ifndef CURL_DISABLE_HTTP
ಸ್ಥಿರ CURLcode setopt_HTTP_VERSION(ಸಂರಚನಾ Curl_easy *data, long arg)
{
  /*
   * This sets a requested HTTP version to be used. The value is one of
   * the listed enums in curl/curl.h.
   */
  ವಿಕಲ್ಪ(arg) {
  ಪಕ್ಷ CURL_HTTP_VERSION_NONE:
    /* accepted */
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_HTTP_VERSION_1_0:
  ಪಕ್ಷ CURL_HTTP_VERSION_1_1:
    /* accepted */
    ಭಙ್ಗ;
#ifdef USE_HTTP2
  ಪಕ್ಷ CURL_HTTP_VERSION_2_0:
  ಪಕ್ಷ CURL_HTTP_VERSION_2TLS:
  ಪಕ್ಷ CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    /* accepted */
    ಭಙ್ಗ;
#endif
#ifdef USE_HTTP3
  ಪಕ್ಷ CURL_HTTP_VERSION_3:
  ಪಕ್ಷ CURL_HTTP_VERSION_3ONLY:
    /* accepted */
    ಭಙ್ಗ;
#endif
  ಸ್ವಭಾವ:
    /* not accepted */
    ಯದಿ(arg < CURL_HTTP_VERSION_NONE)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    ಪ್ರತ್ಯಾಗಮ CURLE_UNSUPPORTED_PROTOCOL;
  }
  data->set.httpwant = (unsigned ವರ್ಣ)arg;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP */

#ifdef USE_SSL
CURLcode Curl_setopt_SSLVERSION(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                                long arg)
{
  /*
   * Set explicit SSL version to try to connect with, as some SSL
   * implementations are lame.
   */
  {
    long version, version_max;
    ಸಂರಚನಾ ssl_easy_config *sslc = &data->set.ssl;
#ifndef CURL_DISABLE_PROXY
    ಯದಿ(option != CURLOPT_SSLVERSION)
      sslc = &data->set.proxy_ssl;
#ಅನ್ಯಥಾ
    (ಶೂನ್ಯ)option; /* unused */
#endif
    version = C_SSLVERSION_ಮೌಲ್ಯ(arg);
    version_max = (long)C_SSLVERSION_MAX_ಮೌಲ್ಯ(arg);

    ಯದಿ(version < CURL_SSLVERSION_DEFAULT ||
       version == CURL_SSLVERSION_SSLv2 ||
       version == CURL_SSLVERSION_SSLv3 ||
       version >= CURL_SSLVERSION_LAST ||
       version_max < CURL_SSLVERSION_MAX_NONE ||
       version_max >= CURL_SSLVERSION_MAX_LAST)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    ಯದಿ(version == CURL_SSLVERSION_DEFAULT)
      version = CURL_SSLVERSION_TLSv1_2;

    sslc->version = (unsigned ವರ್ಣ)version;
    sslc->version_max = (unsigned ಪೂರ್ಣಾಙ್ಕ)version_max;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif /* !USE_SSL */

#ifndef CURL_DISABLE_RTSP
ಸ್ಥಿರ CURLcode setopt_RTSP_REQUEST(ಸಂರಚನಾ Curl_easy *data, long arg)
{
  /*
   * Set the RTSP request method (OPTIONS, SETUP, PLAY, etc...) Would this be
   * better if the RTSPREQ_* were moved into here?
   */
  ಯದಿ((arg <= CURL_RTSPREQ_NONE) || (arg >= CURL_RTSPREQ_LAST))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  data->set.rtspreq = (unsigned ವರ್ಣ)arg;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif /* !CURL_DISABLE_RTSP */

ಸ್ಥಿರ CURLcode setopt_long_bool(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                                 long arg)
{
  bool enabled = !!arg;
  ಪೂರ್ಣಾಙ್ಕ ok = 1;
  ಸಂರಚನಾ UserDefined *s = &data->set;
  ವಿಕಲ್ಪ(option) {
  ಪಕ್ಷ CURLOPT_FORBID_REUSE:
    /*
     * When this transfer is done, it must not be left to be reused by a
     * subsequent transfer but shall be closed immediately.
     */
    s->reuse_forbid = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_FRESH_CONNECT:
    /*
     * This transfer shall not use a previously cached connection but
     * should be made with a fresh new connect!
     */
    s->reuse_fresh = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_VERBOSE:
    /*
     * Verbose means infof() calls that give a lot of information about
     * the connection and transfer procedures as well as internal choices.
     */
    s->verbose = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_HEADER:
    /*
     * Set to include the header in the general data output stream.
     */
    s->include_header = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_NOPROGRESS:
    /*
     * Shut off the internal supported progress meter
     */
    data->progress.hide = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_NOBODY:
    /*
     * Do not include the body part in the output data stream.
     */
    s->opt_no_body = enabled;
#ifndef CURL_DISABLE_HTTP
    ಯದಿ(s->opt_no_body)
      /* in HTTP lingo, no body means using the HEAD request... */
      s->method = HTTPREQ_HEAD;
    ಅನ್ಯಥಾ ಯದಿ(s->method == HTTPREQ_HEAD)
      s->method = HTTPREQ_GET;
#endif
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_FAILONERROR:
    /*
     * Do not output the >=400 error code HTML-page, but instead only
     * return error.
     */
    s->http_fail_on_error = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_KEEP_SENDING_ON_ERROR:
    s->http_keep_sending_on_error = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_UPLOAD:
  ಪಕ್ಷ CURLOPT_PUT:
    /*
     * We want to send data to the remote host. If this is HTTP, that equals
     * using the PUT request.
     */
    ಯದಿ(enabled) {
      /* If this is HTTP, PUT is what's needed to "upload" */
      s->method = HTTPREQ_PUT;
      s->opt_no_body = FALSE; /* this is implied */
    }
    ಅನ್ಯಥಾ
      /* In HTTP, the opposite of upload is GET (unless NOBODY is true as
         then this can be changed to HEAD later on) */
      s->method = HTTPREQ_GET;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_FILETIME:
    /*
     * Try to get the file time of the remote document. The time will
     * later (possibly) become available using curl_easy_getinfo().
     */
    s->get_filetime = enabled;
    ಭಙ್ಗ;
#ifndef CURL_DISABLE_HTTP
  ಪಕ್ಷ CURLOPT_HTTP09_ALLOWED:
    s->http09_allowed = enabled;
    ಭಙ್ಗ;
#ifndef CURL_DISABLE_COOKIES
  ಪಕ್ಷ CURLOPT_COOKIESESSION:
    /*
     * Set this option to TRUE to start a new "cookie session". It will
     * prevent the forthcoming read-cookies-from-file actions to accept
     * cookies that are marked as being session cookies, as they belong to a
     * previous session.
     */
    s->cookiesession = enabled;
    ಭಙ್ಗ;
#endif
  ಪಕ್ಷ CURLOPT_AUTOREFERER:
    /*
     * Switch on automatic referer that gets set if curl follows locations.
     */
    s->http_auto_referer = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_TRANSFER_ENCODING:
    s->http_transfer_encoding = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_UNRESTRICTED_AUTH:
    /*
     * Send authentication (user+password) when following locations, even when
     * hostname changed.
     */
    s->allow_auth_to_other_hosts = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_HTTP_TRANSFER_DECODING:
    /*
     * disable libcurl transfer encoding is used
     */
    s->http_te_skip = !enabled; /* reversed */
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_HTTP_CONTENT_DECODING:
    /*
     * raw data passed to the application when content encoding is used
     */
    s->http_ce_skip = !enabled; /* reversed */
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_HTTPGET:
    /*
     * Set to force us do HTTP GET
     */
    ಯದಿ(enabled) {
      s->method = HTTPREQ_GET;
      s->opt_no_body = FALSE; /* this is implied */
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_POST:
    /* Does this option serve a purpose anymore? Yes it does, when
       CURLOPT_POSTFIELDS is not used and the POST data is read off the
       callback! */
    ಯದಿ(enabled) {
      s->method = HTTPREQ_POST;
      s->opt_no_body = FALSE; /* this is implied */
    }
    ಅನ್ಯಥಾ
      s->method = HTTPREQ_GET;
    ಭಙ್ಗ;
#endif /* !CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_PROXY
  ಪಕ್ಷ CURLOPT_HTTPPROXYTUNNEL:
    /*
     * Tunnel operations through the proxy instead of normal proxy use
     */
    s->tunnel_thru_httpproxy = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_HAPROXYPROTOCOL:
    /*
     * Set to send the HAProxy Proxy Protocol header
     */
    s->haproxyprotocol = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_PROXY_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for proxy.
     */
    s->proxy_ssl.verifypeer = enabled;

    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_PROXY_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for proxy
     */
    s->proxy_ssl.verifyhost = enabled;
    ok = 2;
    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_PROXY_TRANSFER_MODE:
    /*
     * set transfer mode (;type=<a|i>) when doing FTP via an HTTP proxy
     */
    s->proxy_transfer_mode = enabled;
    ಭಙ್ಗ;
#endif /* !CURL_DISABLE_PROXY */
#ಯದಿ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ಪಕ್ಷ CURLOPT_SOCKS5_GSSAPI_NEC:
    /*
     * Set flag for NEC SOCKS5 support
     */
    s->socks5_gssapi_nec = enabled;
    ಭಙ್ಗ;
#endif
#ifdef CURL_LIST_ONLY_PROTOCOL
  ಪಕ್ಷ CURLOPT_DIRLISTONLY:
    /*
     * An option that changes the command to one that asks for a list only, no
     * file info details. Used for FTP, POP3 and SFTP.
     */
    s->list_only = enabled;
    ಭಙ್ಗ;
#endif
  ಪಕ್ಷ CURLOPT_APPEND:
    /*
     * We want to upload and append to an existing file. Used for FTP and
     * SFTP.
     */
    s->remote_append = enabled;
    ಭಙ್ಗ;
#ifndef CURL_DISABLE_FTP
  ಪಕ್ಷ CURLOPT_FTP_USE_EPRT:
    s->ftp_use_eprt = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_FTP_USE_EPSV:
    s->ftp_use_epsv = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_FTP_USE_PRET:
    s->ftp_use_pret = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_FTP_SKIP_PASV_IP:
    /*
     * Enable or disable FTP_SKIP_PASV_IP, which will disable/enable the
     * bypass of the IP address in PASV responses.
     */
    s->ftp_skip_ip = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_WILDCARDMATCH:
    s->wildcard_enabled = enabled;
    ಭಙ್ಗ;
#endif
  ಪಕ್ಷ CURLOPT_CRLF:
    /*
     * Kludgy option to enable CRLF conversions. Subject for removal.
     */
    s->crlf = enabled;
    ಭಙ್ಗ;
#ifndef CURL_DISABLE_TFTP
  ಪಕ್ಷ CURLOPT_TFTP_NO_OPTIONS:
    /*
     * Option that prevents libcurl from sending TFTP option requests to the
     * server.
     */
    s->tftp_no_options = enabled;
    ಭಙ್ಗ;
#endif /* !CURL_DISABLE_TFTP */
  ಪಕ್ಷ CURLOPT_TRANSFERTEXT:
    /*
     * This option was previously named 'FTPASCII'. Renamed to work with
     * more protocols than merely FTP.
     *
     * Transfer using ASCII (instead of BINARY).
     */
    s->prefer_ascii = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying.
     */
    s->ssl.verifypeer = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    ಭಙ್ಗ;
#ifndef CURL_DISABLE_DOH
  ಪಕ್ಷ CURLOPT_DOH_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for DoH.
     */
    s->doh_verifypeer = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_DOH_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for DoH
     */
    s->doh_verifyhost = enabled;
    ok = 2;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_DOH_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying for DoH.
     */
    ಯದಿ(!Curl_ssl_cert_status_request())
      ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;

    s->doh_verifystatus = enabled;
    ok = 2;
    ಭಙ್ಗ;
#endif /* !CURL_DISABLE_DOH */
  ಪಕ್ಷ CURLOPT_SSL_VERIFYHOST:
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
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying.
     */
    ಯದಿ(!Curl_ssl_cert_status_request())
      ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;

    s->ssl.verifystatus = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_CERTINFO:
#ifdef USE_SSL
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_CERTINFO))
      s->ssl.certinfo = enabled;
    ಅನ್ಯಥಾ
#endif
      ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_NOSIGNAL:
    /*
     * The application asks not to set any signal() or alarm() handlers,
     * even when using a timeout.
     */
    s->no_signal = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_TCP_NODELAY:
    /*
     * Enable or disable TCP_NODELAY, which will disable/enable the Nagle
     * algorithm
     */
    s->tcp_nodelay = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_IGNORE_CONTENT_LENGTH:
    s->ignorecl = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_SSL_SESSIONID_CACHE:
    s->ssl.cache_session = enabled;
#ifndef CURL_DISABLE_PROXY
    s->proxy_ssl.cache_session = s->ssl.cache_session;
#endif
    ಭಙ್ಗ;
#ifdef USE_SSH
  ಪಕ್ಷ CURLOPT_SSH_COMPRESSION:
    s->ssh_compression = enabled;
    ಭಙ್ಗ;
#endif /* !USE_SSH */
#ifndef CURL_DISABLE_SMTP
  ಪಕ್ಷ CURLOPT_MAIL_RCPT_ALLOWFAILS:
    /* allow RCPT TO command to fail for some recipients */
    s->mail_rcpt_allowfails = enabled;
    ಭಙ್ಗ;
#endif /* !CURL_DISABLE_SMTP */
  ಪಕ್ಷ CURLOPT_SASL_IR:
    /* Enable/disable SASL initial response */
    s->sasl_ir = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_TCP_KEEPALIVE:
    s->tcp_keepalive = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_TCP_FASTOPEN:
#ಯದಿ defined(CONNECT_DATA_IDEMPOTENT) || defined(MSG_FASTOPEN) || \
  defined(TCP_FASTOPEN_CONNECT)
    s->tcp_fastopen = enabled;
    ಭಙ್ಗ;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif
  ಪಕ್ಷ CURLOPT_SSL_ENABLE_ALPN:
    s->ssl_enable_alpn = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_PATH_AS_IS:
    s->path_as_is = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_PIPEWAIT:
    s->pipewait = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_SUPPRESS_CONNECT_HEADERS:
    s->suppress_connect_headers = enabled;
    ಭಙ್ಗ;
#ifndef CURL_DISABLE_SHUFFLE_DNS
  ಪಕ್ಷ CURLOPT_DNS_SHUFFLE_ADDRESSES:
    s->dns_shuffle_addresses = enabled;
    ಭಙ್ಗ;
#endif
  ಪಕ್ಷ CURLOPT_DISALLOW_USERNAME_IN_URL:
    s->disallow_username_in_url = enabled;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_QUICK_EXIT:
    s->quick_exit = enabled;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }
  ಯದಿ((arg > ok) || (arg < 0))
    /* reserve other values for future use */
    infof(data, "boolean setopt(%d) got unsupported argument %ld,"
          " treated as %d", (ಪೂರ್ಣಾಙ್ಕ)option, arg, enabled);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ಮೌಲ್ಯ_range(long *ಮೌಲ್ಯ, long below_error, long min, long max)
{
  ಯದಿ(*ಮೌಲ್ಯ < below_error)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ಅನ್ಯಥಾ ಯದಿ(*ಮೌಲ್ಯ < min)
    *ಮೌಲ್ಯ = min;
  ಅನ್ಯಥಾ ಯದಿ(*ಮೌಲ್ಯ > max)
    *ಮೌಲ್ಯ = max;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode setopt_long_net(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                                long arg)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ UserDefined *s = &data->set;

  ವಿಕಲ್ಪ(option) {
  ಪಕ್ಷ CURLOPT_DNS_CACHE_TIMEOUT:
    ಯದಿ(arg != -1)
      ಪ್ರತ್ಯಾಗಮ setopt_set_timeout_sec(&s->dns_cache_timeout_ms, arg);
    s->dns_cache_timeout_ms = -1;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_MAXCONNECTS:
    result = ಮೌಲ್ಯ_range(&arg, 0, 0, INT_MAX);
    ಯದಿ(!result)
      s->maxconnects = arg ? (uint32_t)arg : DEFAULT_CONNCACHE_SIZE;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_SERVER_RESPONSE_TIMEOUT:
    ಪ್ರತ್ಯಾಗಮ setopt_set_timeout_sec(&s->server_response_timeout, arg);
  ಪಕ್ಷ CURLOPT_SERVER_RESPONSE_TIMEOUT_MS:
    ಪ್ರತ್ಯಾಗಮ setopt_set_timeout_ms(&s->server_response_timeout, arg);
  ಪಕ್ಷ CURLOPT_ಕೆಳಗಿನ_SPEED_LIMIT:
    ಯದಿ(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಅನ್ಯಥಾ
      s->ಕೆಳಗಿನ_speed_limit = arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_ಕೆಳಗಿನ_SPEED_TIME:
    result = ಮೌಲ್ಯ_range(&arg, 0, 0, USHRT_MAX);
    ಯದಿ(!result)
      s->ಕೆಳಗಿನ_speed_time = (uint16_t)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_PORT:
    ಯದಿ((arg < 0) || (arg > 65535))
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    s->use_port = (unsigned short)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_TIMEOUT:
    ಪ್ರತ್ಯಾಗಮ setopt_set_timeout_sec(&s->timeout, arg);
  ಪಕ್ಷ CURLOPT_TIMEOUT_MS:
    ಪ್ರತ್ಯಾಗಮ setopt_set_timeout_ms(&s->timeout, arg);
  ಪಕ್ಷ CURLOPT_CONNECTTIMEOUT:
    ಪ್ರತ್ಯಾಗಮ setopt_set_timeout_sec(&s->connecttimeout, arg);
  ಪಕ್ಷ CURLOPT_CONNECTTIMEOUT_MS:
    ಪ್ರತ್ಯಾಗಮ setopt_set_timeout_ms(&s->connecttimeout, arg);
#ifndef CURL_DISABLE_BINDLOCAL
  ಪಕ್ಷ CURLOPT_LOCALPORT:
    ಯದಿ((arg < 0) || (arg > 65535))
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    s->localport = curlx_sltous(arg);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_LOCALPORTRANGE:
    ಯದಿ((arg < 0) || (arg > 65535))
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    s->localportrange = curlx_sltous(arg);
    ಭಙ್ಗ;
#endif
  ಪಕ್ಷ CURLOPT_BUFFERSIZE:
    result = ಮೌಲ್ಯ_range(&arg, 0, READBUFFER_MIN, READBUFFER_MAX);
    ಯದಿ(!result)
      s->buffer_size = (unsigned ಪೂರ್ಣಾಙ್ಕ)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_UPLOAD_BUFFERSIZE:
    result = ಮೌಲ್ಯ_range(&arg, 0, UPLOADBUFFER_MIN, UPLOADBUFFER_MAX);
    ಯದಿ(!result)
      s->upload_buffer_size = (unsigned ಪೂರ್ಣಾಙ್ಕ)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_MAXFILESIZE:
    ಯದಿ(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಅನ್ಯಥಾ
      s->max_filesize = arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_IPRESOLVE:
    ಯದಿ((arg < CURL_IPRESOLVE_WHATEVER) || (arg > CURL_IPRESOLVE_V6))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಅನ್ಯಥಾ
      s->ipver = (unsigned ವರ್ಣ)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_CONNECT_ONLY:
    ಯದಿ(arg < 0 || arg > 2)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಅನ್ಯಥಾ {
      s->connect_only = !!arg;
      s->connect_only_ws = (arg == 2);
    }
    ಭಙ್ಗ;
#ifdef USE_IPV6
  ಪಕ್ಷ CURLOPT_ADDRESS_SCOPE:
#ಯದಿ SIZEOF_LONG > 4
    ಯದಿ((unsigned long)arg > UINT_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಅನ್ಯಥಾ
#endif
    s->scope_id = (unsigned ಪೂರ್ಣಾಙ್ಕ)arg;
    ಭಙ್ಗ;
#endif
  ಪಕ್ಷ CURLOPT_TCP_KEEPIDLE:
    result = ಮೌಲ್ಯ_range(&arg, 0, 0, INT_MAX);
    ಯದಿ(!result)
      s->tcp_keepidle = (ಪೂರ್ಣಾಙ್ಕ)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_TCP_KEEPINTVL:
    result = ಮೌಲ್ಯ_range(&arg, 0, 0, INT_MAX);
    ಯದಿ(!result)
      s->tcp_keepintvl = (ಪೂರ್ಣಾಙ್ಕ)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_TCP_KEEPCNT:
    result = ಮೌಲ್ಯ_range(&arg, 0, 0, INT_MAX);
    ಯದಿ(!result)
      s->tcp_keepcnt = (ಪೂರ್ಣಾಙ್ಕ)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
    ಪ್ರತ್ಯಾಗಮ setopt_set_timeout_ms(&s->happy_eyeballs_timeout, arg);
  ಪಕ್ಷ CURLOPT_UPKEEP_INTERVAL_MS:
    ಪ್ರತ್ಯಾಗಮ setopt_set_timeout_ms(&s->upkeep_interval_ms, arg);
  ಪಕ್ಷ CURLOPT_MAXAGE_CONN:
    ಪ್ರತ್ಯಾಗಮ setopt_set_timeout_sec(&s->conn_max_idle_ms, arg);
  ಪಕ್ಷ CURLOPT_MAXLIFETIME_CONN:
    ಪ್ರತ್ಯಾಗಮ setopt_set_timeout_sec(&s->conn_max_age_ms, arg);
  ಪಕ್ಷ CURLOPT_DNS_USE_GLOBAL_CACHE:
    /* deprecated */
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode setopt_long_ssl(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                                long arg)
{
#ifdef USE_SSL
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ UserDefined *s = &data->set;
  ವಿಕಲ್ಪ(option) {
  ಪಕ್ಷ CURLOPT_CA_CACHE_TIMEOUT:
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_CA_CACHE)) {
      result = ಮೌಲ್ಯ_range(&arg, -1, -1, INT_MAX);
      ಯದಿ(!result)
        s->ssl_ca_cache_timeout = (ಪೂರ್ಣಾಙ್ಕ)arg;
    }
    ಅನ್ಯಥಾ
      result = CURLE_NOT_BUILT_IN;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_SSLVERSION:
#ifndef CURL_DISABLE_PROXY
  ಪಕ್ಷ CURLOPT_PROXY_SSLVERSION:
#endif
    ಪ್ರತ್ಯಾಗಮ Curl_setopt_SSLVERSION(data, option, arg);
  ಪಕ್ಷ CURLOPT_SSL_FALSESTART:
    result = CURLE_NOT_BUILT_IN;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_USE_SSL:
    ಯದಿ((arg < CURLUSESSL_NONE) || (arg >= CURLUSESSL_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಅನ್ಯಥಾ
      s->use_ssl = (unsigned ವರ್ಣ)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_SSL_OPTIONS:
    s->ssl.ssl_options = (unsigned ವರ್ಣ)(arg & 0xff);
    ಭಙ್ಗ;
#ifndef CURL_DISABLE_PROXY
  ಪಕ್ಷ CURLOPT_PROXY_SSL_OPTIONS:
    s->proxy_ssl.ssl_options = (unsigned ವರ್ಣ)(arg & 0xff);
    ಭಙ್ಗ;
#endif
  ಪಕ್ಷ CURLOPT_SSL_ENABLE_NPN:
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_SSLENGINE_DEFAULT:
#ifdef USE_OPENSSL
    ಯದಿ(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
    data->set.engine_default = (arg == 1);
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }
  ಪ್ರತ್ಯಾಗಮ result;
#ಅನ್ಯಥಾ  /* USE_SSL */
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)option;
  (ಶೂನ್ಯ)arg;
  ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
#endif /* !USE_SSL */
}

#ifndef CURL_DISABLE_PROXY
ಸ್ಥಿರ ಶೂನ್ಯ changeproxy(ಸಂರಚನಾ Curl_easy *data)
{
  Curl_auth_digest_cleanup(&data->state.proxydigest);
  memset(&data->state.authproxy, 0, ಪರಿಮಾಣ(data->state.authproxy));
}

ಸ್ಥಿರ CURLcode setopt_long_proxy(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                                  long arg)
{
  ಸಂರಚನಾ UserDefined *s = &data->set;

  ವಿಕಲ್ಪ(option) {
  ಪಕ್ಷ CURLOPT_PROXYPORT:
    ಯದಿ((arg < 0) || (arg > UINT16_MAX))
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    ಯದಿ(arg != s->proxyport)
      changeproxy(data);
    s->proxyport = (uint16_t)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_PROXYAUTH:
    ಪ್ರತ್ಯಾಗಮ httpauth(data, TRUE, (unsigned long)arg);
  ಪಕ್ಷ CURLOPT_PROXYTYPE:
    ಯದಿ((arg < CURLPROXY_HTTP) || (arg > CURLPROXY_HTTPS3))
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef USE_PROXY_HTTP3
    ಯದಿ(arg == CURLPROXY_HTTPS3)
      ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif
    s->proxytype = (unsigned ವರ್ಣ)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_SOCKS5_AUTH: {
    unsigned long auth = (unsigned long)arg;
    ಯದಿ(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI | CURLAUTH_ONLY))
      ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
    ಯದಿ((auth & CURLAUTH_ONLY) && !(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI)))
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    s->socks5auth = (unsigned ವರ್ಣ)(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI));
    s->socks5_auth_only = !!(auth & CURLAUTH_ONLY);
    ಭಙ್ಗ;
  }
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#ಅನ್ಯಥಾ
ಸ್ಥಿರ CURLcode setopt_long_proxy(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                                  long arg)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)option;
  (ಶೂನ್ಯ)arg;
  ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
}
#endif

ಸ್ಥಿರ CURLcode setopt_long_http(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                                 long arg)
{
#ifndef CURL_DISABLE_HTTP
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ UserDefined *s = &data->set;

  ವಿಕಲ್ಪ(option) {
  ಪಕ್ಷ CURLOPT_FOLLOWLOCATION:
    ಯದಿ((unsigned long)arg > 3)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಅನ್ಯಥಾ
      s->http_follow_mode = (unsigned ವರ್ಣ)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_MAXREDIRS:
    result = ಮೌಲ್ಯ_range(&arg, -1, -1, 0x7fff);
    ಯದಿ(!result)
      s->maxredirs = (short)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_POSTREDIR:
    ಯದಿ(arg < CURL_REDIR_GET_ALL)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಅನ್ಯಥಾ {
      s->post301 = !!(arg & CURL_REDIR_POST_301);
      s->post302 = !!(arg & CURL_REDIR_POST_302);
      s->post303 = !!(arg & CURL_REDIR_POST_303);
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_HEADEROPT:
    s->sep_headers = !!(arg & CURLHEADER_SEPARATE);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_HTTPAUTH:
    ಪ್ರತ್ಯಾಗಮ httpauth(data, FALSE, (unsigned long)arg);
  ಪಕ್ಷ CURLOPT_HTTP_VERSION:
    ಪ್ರತ್ಯಾಗಮ setopt_HTTP_VERSION(data, arg);
  ಪಕ್ಷ CURLOPT_EXPECT_100_TIMEOUT_MS:
    result = ಮೌಲ್ಯ_range(&arg, 0, 0, 0xffff);
    ಯದಿ(!result)
      s->expect_100_timeout = (unsigned short)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_STREAM_WEIGHT:
#ಯದಿ defined(USE_HTTP2) || defined(USE_HTTP3)
    ಯದಿ((arg >= 1) && (arg <= 256))
      s->weight = (ಪೂರ್ಣಾಙ್ಕ)arg;
    ಭಙ್ಗ;
#ಅನ್ಯಥಾ
    result = CURLE_NOT_BUILT_IN;
    ಭಙ್ಗ;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  ಪಕ್ಷ CURLOPT_HTTPSIG_ALGORITHM:
    ಯದಿ(arg != CURLHTTPSIG_NONE &&
       arg != CURLHTTPSIG_ED25519 &&
       arg != CURLHTTPSIG_HMAC_SHA256)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    s->httpsig_algorithm = (uint8_t)arg;
    ಯದಿ(arg)
      s->httpauth = (uint32_t)CURLAUTH_HTTPSIG;
    ಅನ್ಯಥಾ
      s->httpauth &= ~(uint32_t)CURLAUTH_HTTPSIG;
    ಭಙ್ಗ;
#endif
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }
  ಪ್ರತ್ಯಾಗಮ result;
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)option;
  (ಶೂನ್ಯ)arg;
  ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
#endif
}

ಸ್ಥಿರ CURLcode setopt_long_proto(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                                  long arg)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ UserDefined *s = &data->set;

  ವಿಕಲ್ಪ(option) {
#ifndef CURL_DISABLE_TFTP
  ಪಕ್ಷ CURLOPT_TFTP_BLKSIZE:
    result = ಮೌಲ್ಯ_range(&arg, 0, TFTP_BLKSIZE_MIN, TFTP_BLKSIZE_MAX);
    ಯದಿ(!result)
      s->tftp_blksize = (unsigned short)arg;
    ಭಙ್ಗ;
#endif
#ifndef CURL_DISABLE_NETRC
  ಪಕ್ಷ CURLOPT_NETRC:
    ಯದಿ((arg < CURL_NETRC_IGNORED) || (arg >= CURL_NETRC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಅನ್ಯಥಾ
      s->use_netrc = (unsigned ವರ್ಣ)arg;
    ಭಙ್ಗ;
#endif
#ifndef CURL_DISABLE_FTP
  ಪಕ್ಷ CURLOPT_FTP_FILEMETHOD:
    ಯದಿ((arg < CURLFTPMETHOD_DEFAULT) || (arg >= CURLFTPMETHOD_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಅನ್ಯಥಾ
      s->ftp_filemethod = (unsigned ವರ್ಣ)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_FTP_SSL_CCC:
    ಯದಿ((arg < CURLFTPSSL_CCC_NONE) || (arg >= CURLFTPSSL_CCC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಅನ್ಯಥಾ
      s->ftp_ccc = (unsigned ವರ್ಣ)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_FTPSSLAUTH:
    ಯದಿ((arg < CURLFTPAUTH_DEFAULT) || (arg >= CURLFTPAUTH_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಅನ್ಯಥಾ
      s->ftpsslauth = (unsigned ವರ್ಣ)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_ACCEPTTIMEOUT_MS:
    ಪ್ರತ್ಯಾಗಮ setopt_set_timeout_ms(&s->accepttimeout, arg);
#endif
#ಯದಿ !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  ಪಕ್ಷ CURLOPT_FTP_CREATE_MISSING_DIRS:
    ಯದಿ((arg < CURLFTP_CREATE_DIR_NONE) || (arg > CURLFTP_CREATE_DIR_RETRY))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಅನ್ಯಥಾ
      s->ftp_create_missing_dirs = (unsigned ವರ್ಣ)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_NEW_FILE_PERMS:
    ಯದಿ((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಅನ್ಯಥಾ
      s->new_file_perms = (unsigned ಪೂರ್ಣಾಙ್ಕ)arg;
    ಭಙ್ಗ;
#endif
#ifndef CURL_DISABLE_RTSP
  ಪಕ್ಷ CURLOPT_RTSP_REQUEST:
    ಪ್ರತ್ಯಾಗಮ setopt_RTSP_REQUEST(data, arg);
  ಪಕ್ಷ CURLOPT_RTSP_CLIENT_CSEQ:
    result = ಮೌಲ್ಯ_range(&arg, 0, 0, INT_MAX);
    ಯದಿ(!result)
      data->state.rtsp_next_client_CSeq = (uint32_t)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_RTSP_SERVER_CSEQ:
    result = ಮೌಲ್ಯ_range(&arg, 0, 0, INT_MAX);
    ಯದಿ(!result)
      data->state.rtsp_next_server_CSeq = (uint32_t)arg;
    ಭಙ್ಗ;
#endif
#ifdef USE_SSH
  ಪಕ್ಷ CURLOPT_SSH_AUTH_TYPES:
    s->ssh_auth_types = (uint32_t)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_NEW_DIRECTORY_PERMS:
    ಯದಿ((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಅನ್ಯಥಾ
      s->new_directory_perms = (unsigned ಪೂರ್ಣಾಙ್ಕ)arg;
    ಭಙ್ಗ;
#endif
  ಪಕ್ಷ CURLOPT_PROTOCOLS:
    s->allowed_protocols = (curl_prot_t)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_REDIR_PROTOCOLS:
    s->redir_protocols = (curl_prot_t)arg;
    ಭಙ್ಗ;
#ifndef CURL_DISABLE_WEBSOCKETS
  ಪಕ್ಷ CURLOPT_WS_OPTIONS:
    s->ws_raw_mode = !!(arg & CURLWS_RAW_MODE);
    s->ws_no_auto_pong = !!(arg & CURLWS_NOAUTOPONG);
    ಭಙ್ಗ;
#endif
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode setopt_long_misc(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                                 long arg)
{
  ಸಂರಚನಾ UserDefined *s = &data->set;

  ವಿಕಲ್ಪ(option) {
  ಪಕ್ಷ CURLOPT_TIMECONDITION:
    ಯದಿ((arg < CURL_TIMECOND_NONE) || (arg >= CURL_TIMECOND_LAST))
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    s->timecondition = (unsigned ವರ್ಣ)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_TIMEVALUE:
    s->timevalue = (time_t)arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_POSTFIELDSIZE:
    ಯದಿ(arg < -1)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    ಯದಿ(s->postfieldsize < arg && s->str_copypostfields) {
      curlx_safefree(s->str_copypostfields);
      s->postfields = ರಿಕ್ತ;
    }
    s->postfieldsize = arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_INFILESIZE:
    ಯದಿ(arg < -1)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_RESUME_FROM:
    ಯದಿ(arg < -1)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = arg;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_UPLOAD_FLAGS:
    s->upload_flags = (unsigned ವರ್ಣ)arg;
    ಭಙ್ಗ;
#ifndef CURL_DISABLE_MIME
  ಪಕ್ಷ CURLOPT_MIME_OPTIONS:
    s->mime_formescape = !!(arg & CURLMIMEOPT_FORMESCAPE);
    ಭಙ್ಗ;
#endif
#ifndef CURL_DISABLE_HSTS
  ಪಕ್ಷ CURLOPT_HSTS_CTRL:
    ಯದಿ(arg & CURLHSTS_ENABLE) {
      ಯದಿ(!data->hsts) {
        data->hsts = Curl_hsts_init();
        ಯದಿ(!data->hsts)
          ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(!data->share || !data->share->hsts) {
      /* throw away the HSTS cache unless shared */
      Curl_hsts_cleanup(&data->hsts);
      /* flush all the entries */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
      /* detach from shared HSTS cache without freeing it */
      data->hsts = ರಿಕ್ತ;
    ಭಙ್ಗ;
#endif
#ifndef CURL_DISABLE_ALTSVC
  ಪಕ್ಷ CURLOPT_ALTSVC_CTRL:
    ಪ್ರತ್ಯಾಗಮ Curl_altsvc_ctrl(data, arg);
#endif
#ಯದಿ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ಪಕ್ಷ CURLOPT_GSSAPI_DELEGATION:
    s->gssapi_delegation = (unsigned ವರ್ಣ)arg &
      (CURLGSSAPI_DELEGATION_POLICY_FLAG | CURLGSSAPI_DELEGATION_FLAG);
    ಭಙ್ಗ;
#endif
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode setopt_long(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                            long arg)
{
  ಪ್ರಕಾರನಿರ್ದೇಶ CURLcode (*setoptfunc)(ಸಂರಚನಾ Curl_easy *data,
                                 CURLoption option, long arg);
  ಸ್ಥಿರ ಧ್ರುವ setoptfunc setopt_call[] = {
    setopt_long_bool,
    setopt_long_net,
    setopt_long_http,
    setopt_long_proxy,
    setopt_long_ssl,
    setopt_long_proto,
    setopt_long_misc
  };
  size_t i;

  ಪ್ರತಿ(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, arg);
    ಯದಿ(result != CURLE_UNKNOWN_OPTION)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
}

ಸ್ಥಿರ CURLcode setopt_slist(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                             ಸಂರಚನಾ curl_slist *slist)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ UserDefined *s = &data->set;
  ವಿಕಲ್ಪ(option) {
#ifndef CURL_DISABLE_PROXY
  ಪಕ್ಷ CURLOPT_PROXYHEADER:
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
    ಭಙ್ಗ;
#endif
#ifndef CURL_DISABLE_HTTP
  ಪಕ್ಷ CURLOPT_HTTP200ALIASES:
    /*
     * Set a list of aliases for HTTP 200 in response header
     */
    s->http200aliases = slist;
    ಭಙ್ಗ;
#endif
#ಯದಿ !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  ಪಕ್ಷ CURLOPT_POSTQUOTE:
    /*
     * List of RAW FTP commands to use after a transfer
     */
    s->postquote = slist;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_PREQUOTE:
    /*
     * List of RAW FTP commands to use prior to RETR (Wesley Laxton)
     */
    s->prequote = slist;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_QUOTE:
    /*
     * List of RAW FTP commands to use before a transfer
     */
    s->quote = slist;
    ಭಙ್ಗ;
#endif
  ಪಕ್ಷ CURLOPT_RESOLVE:
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
    ಭಙ್ಗ;
#ಯದಿ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MIME)
  ಪಕ್ಷ CURLOPT_HTTPHEADER:
    /*
     * Set a list with HTTP headers to use (or replace internals with)
     */
    s->headers = slist;
    ಭಙ್ಗ;
#endif
#ifndef CURL_DISABLE_TELNET
  ಪಕ್ಷ CURLOPT_TELNETOPTIONS:
    /*
     * Set a linked list of telnet options
     */
    s->telnet_options = slist;
    ಭಙ್ಗ;
#endif
#ifndef CURL_DISABLE_SMTP
  ಪಕ್ಷ CURLOPT_MAIL_RCPT:
    /* Set the list of mail recipients */
    s->mail_rcpt = slist;
    ಭಙ್ಗ;
#endif
  ಪಕ್ಷ CURLOPT_CONNECT_TO:
    s->connect_to = slist;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

#ಯದಿ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
ಸ್ಥಿರ CURLcode setopt_mimepost(ಸಂರಚನಾ Curl_easy *data, curl_mime *mimep)
{
  /*
   * Set to make us do MIME POST
   */
  CURLcode result;
  ಸಂರಚನಾ UserDefined *s = &data->set;
  ಯದಿ(!s->mimepostp) {
    s->mimepostp = curlx_malloc(ಪರಿಮಾಣ(*s->mimepostp));
    ಯದಿ(!s->mimepostp)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    Curl_mime_initpart(s->mimepostp);
  }

  result = Curl_mime_set_subparts(s->mimepostp, mimep, FALSE);
  ಯದಿ(!result) {
    s->method = HTTPREQ_POST_MIME;
    s->opt_no_body = FALSE; /* this is implied */
#ifndef CURL_DISABLE_FORM_API
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = ರಿಕ್ತ;
#endif
  }
  ಪ್ರತ್ಯಾಗಮ result;
}
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */

ಸ್ಥಿರ CURLcode setopt_share(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ Curl_share *set)
{
  CURLcode result;

  ಯದಿ(data->conn) {
    /* As this handle already has a connection attached, changing share now
       would be complicated and error-prone */
    infof(data, "Cannot change share object while in use");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
  }
  ಅನ್ಯಥಾ {
    /* disconnect from old share, if any and possible */
    result = Curl_share_easy_unlink(data);
    ಯದಿ(!result && GOOD_SHARE_HANDLE(set))
      /* use new share if it set */
      result = Curl_share_easy_link(data, set);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/* assorted pointer type arguments */
ಸ್ಥಿರ CURLcode setopt_pointers(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                                va_list param)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ UserDefined *s = &data->set;
  ವಿಕಲ್ಪ(option) {
  ಪಕ್ಷ CURLOPT_CURLU:
    /*
     * pass CURLU to set URL
     */
    Curl_bufref_free(&data->state.url);
    CURL_EASY_STR_CLEAR(data, STRING_SET_URL);
    s->uh = va_arg(param, CURLU *);
    ಭಙ್ಗ;
#ifndef CURL_DISABLE_HTTP
#ifndef CURL_DISABLE_FORM_API
  ಪಕ್ಷ CURLOPT_HTTPPOST:
    /*
     * Set to make us do HTTP POST. Legacy API-style.
     */
    s->httppost = va_arg(param, ಸಂರಚನಾ curl_httppost *);
    s->method = HTTPREQ_POST_FORM;
    s->opt_no_body = FALSE; /* this is implied */
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = ರಿಕ್ತ;
    ಭಙ್ಗ;
#endif /* !CURL_DISABLE_FORM_API */
#endif /* !CURL_DISABLE_HTTP */
#ಯದಿ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
  ಪಕ್ಷ CURLOPT_MIMEPOST:
    result = setopt_mimepost(data, va_arg(param, curl_mime *));
    ಭಙ್ಗ;
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */
  ಪಕ್ಷ CURLOPT_STDERR:
    /*
     * Set to a FILE * that should receive all error writes. This
     * defaults to stderr for normal operations.
     */
    s->err = va_arg(param, FILE *);
    ಯದಿ(!s->err)
      s->err = stderr;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_SHARE:
    ಪ್ರತ್ಯಾಗಮ setopt_share(data, va_arg(param, ಸಂರಚನಾ Curl_share *));

  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifndef CURL_DISABLE_COOKIES
ಸ್ಥಿರ CURLcode cookielist(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *ptr)
{
  CURLcode result = CURLE_OK;
  ಯದಿ(!ptr)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಯದಿ(curl_strequal(ptr, "ALL")) {
    /* clear all cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearall(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  ಅನ್ಯಥಾ ಯದಿ(curl_strequal(ptr, "SESS")) {
    /* clear session cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearsess(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  ಅನ್ಯಥಾ ಯದಿ(curl_strequal(ptr, "FLUSH")) {
    /* flush cookies to file, takes care of the locking */
    Curl_flush_cookies(data, FALSE);
  }
  ಅನ್ಯಥಾ ಯದಿ(curl_strequal(ptr, "RELOAD")) {
    /* reload cookies from file */
    ಪ್ರತ್ಯಾಗಮ Curl_cookie_loadfiles(data, COOKIE_NOPSL |
                                 (data->set.cookiesession ?
                                  COOKIE_NOSESSION : 0));
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(!data->cookies) {
      /* if cookie engine was not running, activate it */
      data->cookies = Curl_cookie_init();
      ಯದಿ(!data->cookies)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      data->state.cookie_engine = TRUE;
    }

    /* general protection against mistakes and abuse */
    ಯದಿ(strlen(ptr) > CURL_MAX_ಒಳಹರಿವು_LENGTH)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

    /* Adding these cookies without the PSL check, because the PSL is not
       initialized until *perform() time, and this might be called before
       that */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    ಯದಿ(checkprefix("Set-Cookie:", ptr))
      /* HTTP Header format line */
      result = Curl_cookie_add(data, data->cookies, ptr + 11,
                               ರಿಕ್ತ, ರಿಕ್ತ,
                               COOKIE_HTTPHEADER | COOKIE_SECURE |
                               COOKIE_NOPSL);
    ಅನ್ಯಥಾ
      /* Netscape format line */
      result = Curl_cookie_add(data, data->cookies, ptr, ರಿಕ್ತ,
                               ರಿಕ್ತ, COOKIE_SECURE | COOKIE_NOPSL);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cookiefile(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *ptr)
{
  /*
   * Set cookie file to read and parse. Can be used multiple times.
   */
  ಯದಿ(ptr) {
    ಸಂರಚನಾ curl_slist *cl;
    /* general protection against mistakes and abuse */
    ಯದಿ(strlen(ptr) > CURL_MAX_ಒಳಹರಿವು_LENGTH)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    /* append the cookie filename to the list of filenames, and deal with
       them later */
    cl = curl_slist_append(data->state.cookielist, ptr);
    ಯದಿ(!cl) {
      curl_slist_free_all(data->state.cookielist);
      data->state.cookielist = ರಿಕ್ತ;
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
    data->state.cookielist = cl; /* store the list for later use */
  }
  ಅನ್ಯಥಾ {
    /* clear the list of cookie files */
    curl_slist_free_all(data->state.cookielist);
    data->state.cookielist = ರಿಕ್ತ;

    ಯದಿ(!data->share || !data->share->cookies) {
      /* throw away all existing cookies if this is not a shared cookie
         container */
      Curl_cookie_clearall(data->cookies);
      Curl_cookie_cleanup(data->cookies);
    }
    /* disable the cookie engine */
    data->cookies = ರಿಕ್ತ;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_PROXY
ಸ್ಥಿರ CURLcode setproxy(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *proxy)
{
  ಧ್ರುವ ವರ್ಣ *str = CURL_EASY_STR(data, STRING_PROXY);
  ಯದಿ(str && proxy &&
     /* there was one set, is this a new one? */
     !strcmp(str, proxy))
    ಪ್ರತ್ಯಾಗಮ CURLE_OK; /* same one as before */

  changeproxy(data);
  ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_PROXY, proxy);
}

ಸ್ಥಿರ CURLcode setopt_cptr_proxy(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                                  ವರ್ಣ *ptr)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ UserDefined *s = &data->set;
  ವಿಕಲ್ಪ(option) {
  ಪಕ್ಷ CURLOPT_PROXYUSERPWD: {
    /*
     * user:password needed to use the proxy
     */
    ವರ್ಣ *u = ರಿಕ್ತ;
    ವರ್ಣ *p = ರಿಕ್ತ;
    result = setstropt_userpwd(ptr, &u, &p);

    /* URL decode the components */
    ಯದಿ(!result) {
      ವರ್ಣ *str = ರಿಕ್ತ;
      CURL_EASY_STR_CLEAR(data, STRING_PROXYUSERNAME);
      CURL_EASY_STR_CLEAR(data, STRING_PROXYPASSWORD);
      ಯದಿ(u) {
        result = Curl_urldecode(u, 0, &str, ರಿಕ್ತ, REJECT_ZERO);
        ಯದಿ(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYUSERNAME, str);
      }
      ಯದಿ(!result && p) {
        str = ರಿಕ್ತ;
        result = Curl_urldecode(p, 0, &str, ರಿಕ್ತ, REJECT_ZERO);
        ಯದಿ(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYPASSWORD, str);
      }
    }
    curlx_free(u);
    curlx_strzero(p);
    curlx_free(p);
    ಭಙ್ಗ;
  }
  ಪಕ್ಷ CURLOPT_PROXYUSERNAME:
    /*
     * authentication username to use in the operation
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_PROXYUSERNAME, ptr);

  ಪಕ್ಷ CURLOPT_PROXYPASSWORD:
    /*
     * authentication password to use in the operation
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_PROXYPASSWORD, ptr);

  ಪಕ್ಷ CURLOPT_NOPROXY:
    /*
     * proxy exception list
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_NOPROXY, ptr);
  ಪಕ್ಷ CURLOPT_PROXY_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use for proxy
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_CERT_PROXY, ptr);
  ಪಕ್ಷ CURLOPT_PROXY_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use for proxy
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_CERT_TYPE_PROXY, ptr);
  ಪಕ್ಷ CURLOPT_PROXY_SSLKEY:
    /*
     * String that holds filename of the SSL key to use for proxy
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_ಕೀಲಿ_PROXY, ptr);
  ಪಕ್ಷ CURLOPT_PROXY_KEYPASSWD:
    /*
     * String that holds the SSL private key password for proxy.
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_ಕೀಲಿ_PASSWD_PROXY, ptr);
  ಪಕ್ಷ CURLOPT_PROXY_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use for proxy
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_ಕೀಲಿ_TYPE_PROXY, ptr);
  ಪಕ್ಷ CURLOPT_PROXY_SSL_CIPHER_LIST:
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST)) {
      /* set a list of cipher we want to use in the SSL connection for proxy */
      ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_SSL_CIPHER_LIST_PROXY, ptr);
    }
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  ಪಕ್ಷ CURLOPT_PROXY_TLS13_CIPHERS:
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites for proxy */
      ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_SSL_CIPHER13_LIST_PROXY, ptr);
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  ಪಕ್ಷ CURLOPT_PROXY:
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
    ಪ್ರತ್ಯಾಗಮ setproxy(data, ptr);
  ಪಕ್ಷ CURLOPT_PRE_PROXY:
    /*
     * Set proxy server:port to use as SOCKS proxy.
     *
     * If the proxy is set to "" or NULL we explicitly say that we do not want
     * to use the socks proxy.
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_PRE_PROXY, ptr);
  ಪಕ್ಷ CURLOPT_SOCKS5_GSSAPI_SERVICE:
  ಪಕ್ಷ CURLOPT_PROXY_SERVICE_NAME:
    /*
     * Set proxy authentication service name for Kerberos 5 and SPNEGO
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_PROXY_SERVICE_NAME, ptr);
  ಪಕ್ಷ CURLOPT_PROXY_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
#ifdef USE_SSL
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY_PROXY, ptr);
#endif
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;

  ಪಕ್ಷ CURLOPT_HAPROXY_CLIENT_IP:
    /*
     * Set the client IP to send through HAProxy PROXY protocol
     */
    result = Curl_setstropt(data, STRING_HAPROXY_CLIENT_IP, ptr);

    /* enable the HAProxy protocol if an IP is provided */
    s->haproxyprotocol = !!CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_PROXY_CAINFO:
    /*
     * Set CA info SSL connection for proxy. Specify filename of the
     * CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE_PROXY, ptr);
    ಯದಿ(!result)
      s->proxy_ssl.custom_cafile = TRUE;
    ಪ್ರತ್ಯಾಗಮ result;
  ಪಕ್ಷ CURLOPT_PROXY_CRLFILE:
    /*
     * Set CRL file info for SSL connection for proxy. Specify filename of the
     * CRL to check certificates revocation
     */
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_SSL_CRLFILE_PROXY, ptr);
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  ಪಕ್ಷ CURLOPT_PROXY_ISSUERCERT:
    /*
     * Set Issuer certificate file to check certificates issuer
     */
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_SSL_ISSUERCERT_PROXY, ptr);
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  ಪಕ್ಷ CURLOPT_PROXY_CAPATH:
    /*
     * Set CA path info for SSL connection proxy. Specify directory name of the
     * CA certificates which have been prepared using openssl c_rehash utility.
     */
#ifdef USE_SSL
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH_PROXY, ptr);
      ಯದಿ(!result)
        s->proxy_ssl.custom_capath = TRUE;
      ಪ್ರತ್ಯಾಗಮ result;
    }
#endif
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}
#endif

#ಯದಿ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
/*
 * A string with POST data. Makes curl HTTP POST. Even if it is NULL. If
 * needed, CURLOPT_POSTFIELDSIZE must have been set prior to
 * CURLOPT_COPYPOSTFIELDS and not altered later.
 */
ಸ್ಥಿರ CURLcode setopt_copypostfields(ಧ್ರುವ ವರ್ಣ *ptr, ಸಂರಚನಾ UserDefined *s)
{
  CURLcode result = CURLE_OK;
  ಯದಿ(s->postfieldsize < -1)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ಯದಿ(!ptr || s->postfieldsize == -1) {
    ಯದಿ(ptr && (strlen(ptr) > CURL_MAX_ಒಳಹರಿವು_LENGTH))
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_safefree(s->str_copypostfields);
    ಯದಿ(ptr) {
      s->str_copypostfields = curlx_strdup(ptr);
      ಯದಿ(!s->str_copypostfields)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
  }
  ಅನ್ಯಥಾ {
    size_t pflen = curlx_sotouz_range(s->postfieldsize, 0, SIZE_MAX);
    ಯದಿ(pflen == SIZE_MAX)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    ಅನ್ಯಥಾ {
      /* Allocate even when size == 0. This satisfies the need of possible
         later address compare to detect the COPYPOSTFIELDS mode, and to mark
         that postfields is used rather than read function or form data. */
      ವರ್ಣ *p = curlx_memdup0(ptr, pflen);
      ಯದಿ(!p)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      ಅನ್ಯಥಾ {
        curlx_free(s->str_copypostfields);
        s->str_copypostfields = p;
      }
    }
  }

  s->postfields = s->str_copypostfields;
  s->method = HTTPREQ_POST;
  ಪ್ರತ್ಯಾಗಮ result;
}
#endif

#ifdef USE_ECH
ಸ್ಥಿರ CURLcode setopt_ech(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *ptr)
{
  ಸಂರಚನಾ UserDefined *s = &data->set;
  CURLcode result = CURLE_OK;

  ಯದಿ(!ptr || !strcmp(ptr, "false"))
    s->ssl.ech = CURLECH_DISABLE;
  ಅನ್ಯಥಾ {
    size_t plen = strlen(ptr);
    ಯದಿ(plen > CURL_MAX_ಒಳಹರಿವು_LENGTH)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಅನ್ಯಥಾ {
      ಯದಿ(!strcmp(ptr, "grease"))
        s->ssl.ech = CURLECH_GREASE;
      ಅನ್ಯಥಾ ಯದಿ(!strcmp(ptr, "true"))
        s->ssl.ech = CURLECH_ENABLE;
      ಅನ್ಯಥಾ ಯದಿ(!strcmp(ptr, "hard"))
        s->ssl.ech = CURLECH_HARD;
      ಅನ್ಯಥಾ ಯದಿ(plen > 4 && !strncmp(ptr, "ecl:", 4)) {
        ಯದಿ(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_CONFIG, ptr + 4);
      }
      ಅನ್ಯಥಾ ಯದಿ(plen > 3 && !strncmp(ptr, "pn:", 3)) {
        ಯದಿ(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_PUBLIC, ptr + 3);
      }
      ಅನ್ಯಥಾ
        result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}
#ಅನ್ಯಥಾ
#define setopt_ech(x, y) CURLE_NOT_BUILT_IN
#endif

#ಯದಿ defined(USE_SSL) || defined(USE_SSH)
/* One of the options is used for both TLS and SSH */
ಸ್ಥಿರ CURLcode setopt_cptr_ssl(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                                ವರ್ಣ *ptr)
{
  CURLcode result = CURLE_OK;

  ವಿಕಲ್ಪ(option) {
  ಪಕ್ಷ CURLOPT_KEYPASSWD:
    /*
     * String that holds the SSL or SSH private key password.
     */
    result = Curl_setstropt(data, STRING_ಕೀಲಿ_PASSWD, ptr);
    ಭಙ್ಗ;
#ifdef USE_SSL
  ಪಕ್ಷ CURLOPT_CAINFO:
    /*
     * Set CA info for SSL connection. Specify filename of the CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE, ptr);
    ಯದಿ(!result)
      data->set.ssl.custom_cafile = TRUE;
    ಪ್ರತ್ಯಾಗಮ result;
  ಪಕ್ಷ CURLOPT_CAPATH:
    /*
     * Set CA path info for SSL connection. Specify directory name of the CA
     * certificates which have been prepared using openssl c_rehash utility.
     */
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH, ptr);
      ಯದಿ(!result)
        data->set.ssl.custom_capath = TRUE;
      ಪ್ರತ್ಯಾಗಮ result;
    }
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  ಪಕ್ಷ CURLOPT_CRLFILE:
    /*
     * Set CRL file info for SSL connection. Specify filename of the CRL
     * to check certificates revocation
     */
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_SSL_CRLFILE, ptr);
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  ಪಕ್ಷ CURLOPT_SSL_CIPHER_LIST:
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST))
      /* set a list of cipher we want to use in the SSL connection */
      ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_SSL_CIPHER_LIST, ptr);
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  ಪಕ್ಷ CURLOPT_TLS13_CIPHERS:
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites */
      ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_SSL_CIPHER13_LIST, ptr);
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  ಪಕ್ಷ CURLOPT_RANDOM_FILE:
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_EGDSOCKET:
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_SSL_CTX_DATA:
    /*
     * Set an SSL_CTX callback parameter pointer
     */
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      data->set.ssl_fsslctxp = ptr;
      ಭಙ್ಗ;
    }
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  ಪಕ್ಷ CURLOPT_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_CERT, ptr);
  ಪಕ್ಷ CURLOPT_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_CERT_TYPE, ptr);
  ಪಕ್ಷ CURLOPT_SSLKEY:
    /*
     * String that holds filename of the SSL key to use
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_ಕೀಲಿ, ptr);
  ಪಕ್ಷ CURLOPT_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_ಕೀಲಿ_TYPE, ptr);
  ಪಕ್ಷ CURLOPT_SSLENGINE:
    /*
     * String that holds the SSL crypto engine.
     */
    ಯದಿ(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
    result = Curl_setstropt(data, STRING_SSL_ENGINE, ptr);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_ISSUERCERT:
    /*
     * Set Issuer certificate file
     * to check certificates issuer
     */
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_SSL_ISSUERCERT, ptr);
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  ಪಕ್ಷ CURLOPT_SSL_EC_CURVES:
    /*
     * Set accepted curves in SSL connection setup.
     * Specify colon-delimited list of curve algorithm names.
     */
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_SSL_EC_CURVES))
      ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_SSL_EC_CURVES, ptr);
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  ಪಕ್ಷ CURLOPT_SSL_SIGNATURE_ALGORITHMS:
    /*
     * Set accepted signature algorithms.
     * Specify colon-delimited list of signature scheme names.
     */
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_SIGNATURE_ALGORITHMS))
      ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_SSL_SIGNATURE_ALGORITHMS, ptr);
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  ಪಕ್ಷ CURLOPT_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY, ptr);
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  ಪಕ್ಷ CURLOPT_ECH:
    ಪ್ರತ್ಯಾಗಮ setopt_ech(data, ptr);
#endif
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}
#endif

#ಯದಿ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
ಸ್ಥಿರ CURLcode setopt_cptr_http_mqtt(ಸಂರಚನಾ Curl_easy *data,
                                      CURLoption option, ವರ್ಣ *ptr)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ UserDefined *s = &data->set;

  ವಿಕಲ್ಪ(option) {
  ಪಕ್ಷ CURLOPT_COPYPOSTFIELDS:
    ಪ್ರತ್ಯಾಗಮ setopt_copypostfields(ptr, s);

  ಪಕ್ಷ CURLOPT_POSTFIELDS:
    /*
     * Like above, but use static data instead of copying it.
     */
    s->postfields = ptr;
    /* Release old copied data. */
    curlx_safefree(s->str_copypostfields);
    s->method = HTTPREQ_POST;
    ಭಙ್ಗ;

#ifndef CURL_DISABLE_HTTP
  ಪಕ್ಷ CURLOPT_TRAILERDATA:
    s->trailer_data = ptr;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_ACCEPT_ENCODING:
    /*
     * String to use at the value of Accept-Encoding header.
     *
     * If the encoding is set to "" we use an Accept-Encoding header that
     * encompasses all the encodings we support.
     * If the encoding is set to NULL we do not send an Accept-Encoding header
     * and ignore any received Content-Encoding header.
     *
     */
    ಯದಿ(ptr && !*ptr) {
      ptr = Curl_get_content_encodings();
      ಯದಿ(ptr)
        result = CURL_EASY_STR_SETN(data, STRING_ENCODING, ptr);
      ಅನ್ಯಥಾ
        result = CURLE_OUT_OF_MEMORY;
      ಪ್ರತ್ಯಾಗಮ result;
    }
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_ENCODING, ptr);

#ifndef CURL_DISABLE_AWS
  ಪಕ್ಷ CURLOPT_AWS_SIGV4:
    /*
     * String that is merged to some authentication
     * parameters are used by the algorithm.
     */
    result = Curl_setstropt(data, STRING_AWS_SIGV4, ptr);
    /*
     * Basic has been set by default; it needs to be unset here.
     */
    ಯದಿ(CURL_EASY_STR(data, STRING_AWS_SIGV4))
      s->httpauth = CURLAUTH_AWS_SIGV4;
    ಅನ್ಯಥಾ
      s->httpauth &= ~(uint32_t)CURLAUTH_AWS_SIGV4;
    ಭಙ್ಗ;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  ಪಕ್ಷ CURLOPT_HTTPSIG_ಕೀಲಿ:
    result = Curl_setstropt(data, STRING_HTTPSIG_ಕೀಲಿ, ptr);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_HTTPSIG_KEYID:
    result = Curl_setstropt(data, STRING_HTTPSIG_KEYID, ptr);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_HTTPSIG_HEADERS:
    result = Curl_setstropt(data, STRING_HTTPSIG_HEADERS, ptr);
    ಭಙ್ಗ;
#endif
  ಪಕ್ಷ CURLOPT_REFERER: {
    /*
     * String to set in the HTTP Referer: field.
     */
    ಸಂರಚನಾ bufref *oldref = &data->state.referer;
    /* free the old after the storing the new in case the input is actually
       pointing back to this */
    result = Curl_setstropt(data, STRING_SET_REFERER, ptr);
    Curl_bufref_free(oldref);
    ಭಙ್ಗ;
  }

  ಪಕ್ಷ CURLOPT_USERAGENT:
    /*
     * String to use in the HTTP User-Agent field
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_USERAGENT, ptr);

#ifndef CURL_DISABLE_COOKIES
  ಪಕ್ಷ CURLOPT_COOKIE:
    /*
     * Cookie string to send to the remote server in the request.
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_COOKIE, ptr);

  ಪಕ್ಷ CURLOPT_COOKIEFILE:
    ಪ್ರತ್ಯಾಗಮ cookiefile(data, ptr);

  ಪಕ್ಷ CURLOPT_COOKIEJAR:
    /*
     * Set cookie filename to dump all cookies to when we are done.
     */
    result = Curl_setstropt(data, STRING_COOKIEJAR, ptr);
    ಯದಿ(!result) {
      /*
       * Activate the cookie parser. This may or may not already
       * have been made.
       */
      ಯದಿ(!data->cookies)
        data->cookies = Curl_cookie_init();
      ಯದಿ(!data->cookies)
        result = CURLE_OUT_OF_MEMORY;
      ಅನ್ಯಥಾ
        data->state.cookie_engine = TRUE;
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ CURLOPT_COOKIELIST:
    ಪ್ರತ್ಯಾಗಮ cookielist(data, ptr);
#endif /* !CURL_DISABLE_COOKIES */

#endif /* !CURL_DISABLE_HTTP */
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_MQTT */

#ifdef USE_SSH
ಸ್ಥಿರ CURLcode setopt_cptr_ssh(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                                ವರ್ಣ *ptr)
{
  ಸಂರಚನಾ UserDefined *s = &data->set;
  ವಿಕಲ್ಪ(option) {
  ಪಕ್ಷ CURLOPT_SSH_PUBLIC_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa.pub file
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_SSH_PUBLIC_ಕೀಲಿ, ptr);
  ಪಕ್ಷ CURLOPT_SSH_PRIVATE_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa file
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_SSH_PRIVATE_ಕೀಲಿ, ptr);
  ಪಕ್ಷ CURLOPT_SSH_KEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_keyfunc_userp = ptr;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_SSH_HOST_PUBLIC_ಕೀಲಿ_MD5:
    /*
     * Option to allow for the MD5 of the host public key to be checked
     * for validation purposes.
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_ಕೀಲಿ_MD5, ptr);
  ಪಕ್ಷ CURLOPT_SSH_HOST_PUBLIC_ಕೀಲಿ_SHA256:
    /*
     * Option to allow for the SHA256 of the host public key to be checked
     * for validation purposes.
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_ಕೀಲಿ_SHA256, ptr);
  ಪಕ್ಷ CURLOPT_SSH_KNOWNHOSTS:
    /*
     * Store the filename to read known hosts from.
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_SSH_KNOWNHOSTS, ptr);
#ifdef USE_LIBSSH2
  ಪಕ್ಷ CURLOPT_SSH_HOSTKEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_hostkeyfunc_userp = ptr;
    ಭಙ್ಗ;
#endif /* USE_LIBSSH2 */
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif /* USE_SSH */

#ifndef CURL_DISABLE_FTP
ಸ್ಥಿರ CURLcode setopt_cptr_ftp(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                                ವರ್ಣ *ptr)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ UserDefined *s = &data->set;
  ವಿಕಲ್ಪ(option) {
  ಪಕ್ಷ CURLOPT_FTPPORT:
    /*
     * Use FTP PORT, this also specifies which IP address to use
     */
    result = Curl_setstropt(data, STRING_FTPPORT, ptr);
    s->ftp_use_port = !!CURL_EASY_STR(data, STRING_FTPPORT);
    ಭಙ್ಗ;

  ಪಕ್ಷ CURLOPT_FTP_ACCOUNT:
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_FTP_ACCOUNT, ptr);

  ಪಕ್ಷ CURLOPT_FTP_ALTERNATIVE_TO_USER:
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_FTP_ALTERNATIVE_TO_USER, ptr);

  ಪಕ್ಷ CURLOPT_KRBLEVEL:
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN; /* removed in 8.17.0 */
  ಪಕ್ಷ CURLOPT_CHUNK_DATA:
    s->wildcardptr = ptr;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_FNMATCH_DATA:
    s->fnmatch_data = ptr;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}
#endif /* !CURL_DISABLE_FTP */

ಸ್ಥಿರ CURLcode setopt_cptr_net(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                                ವರ್ಣ *ptr)
{
  ವಿಕಲ್ಪ(option) {
  ಪಕ್ಷ CURLOPT_INTERFACE:
    /*
     * Set what interface or address/hostname to bind the socket to when
     * performing an operation and thus what from-IP your connection will use.
     */
    ಪ್ರತ್ಯಾಗಮ setstropt_interface(data, ptr);

#ifdef USE_RESOLV_ARES
  ಪಕ್ಷ CURLOPT_DNS_SERVERS:
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_DNS_SERVERS, ptr);

  ಪಕ್ಷ CURLOPT_DNS_INTERFACE:
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_DNS_INTERFACE, ptr);

  ಪಕ್ಷ CURLOPT_DNS_LOCAL_IP4:
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_DNS_LOCAL_IP4, ptr);

  ಪಕ್ಷ CURLOPT_DNS_LOCAL_IP6:
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_DNS_LOCAL_IP6, ptr);
#endif
#ifdef USE_UNIX_SOCKETS
  ಪಕ್ಷ CURLOPT_UNIX_SOCKET_PATH:
    data->set.abstract_unix_socket = FALSE;
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);

  ಪಕ್ಷ CURLOPT_ABSTRACT_UNIX_SOCKET:
    data->set.abstract_unix_socket = TRUE;
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);
#endif
#ifndef CURL_DISABLE_DOH
  ಪಕ್ಷ CURLOPT_DOH_URL:
    {
      CURLcode result = Curl_setstropt(data, STRING_DOH, ptr);
      data->set.doh = !!CURL_EASY_STR(data, STRING_DOH);
      ಪ್ರತ್ಯಾಗಮ result;
    }
#endif
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }
}

ಸ್ಥಿರ CURLcode setopt_cptr_misc(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                                 ವರ್ಣ *ptr)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ UserDefined *s = &data->set;

  ವಿಕಲ್ಪ(option) {
  ಪಕ್ಷ CURLOPT_REQUEST_TARGET:
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_TARGET, ptr);
#ifndef CURL_DISABLE_NETRC
  ಪಕ್ಷ CURLOPT_NETRC_FILE:
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_NETRC_FILE, ptr);
#endif
  ಪಕ್ಷ CURLOPT_CUSTOMREQUEST:
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_CUSTOMREQUEST, ptr);

    /* we do not set s->method = HTTPREQ_CUSTOM; here, we continue as if we
       were using the already set type and this changes the actual request
       keyword */
  ಪಕ್ಷ CURLOPT_SERVICE_NAME:
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_SERVICE_NAME, ptr);

  ಪಕ್ಷ CURLOPT_HEADERDATA:
    s->writeheader = ptr;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_READDATA:
    s->in_set = ptr;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_WRITEDATA:
    s->out = ptr;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_DEBUGDATA:
    s->debugdata = ptr;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_PROGRESSDATA:
    s->progress_client = ptr;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_SEEKDATA:
    s->seek_client = ptr;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_IOCTLDATA:
    s->ioctl_client = ptr;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_SOCKOPTDATA:
    s->sockopt_client = ptr;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_OPENSOCKETDATA:
    s->opensocket_client = ptr;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_RESOLVER_START_DATA:
    s->resolver_start_client = ptr;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_CLOSESOCKETDATA:
    s->closesocket_client = ptr;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_PREREQDATA:
    s->prereq_userp = ptr;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_ERRORBUFFER:
    s->errorbuffer = ptr;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_URL:
    result = Curl_setstropt(data, STRING_SET_URL, ptr);
    Curl_bufref_set(&data->state.url,
                    CURL_EASY_STR(data, STRING_SET_URL), 0, ರಿಕ್ತ);
    ಭಙ್ಗ;

  ಪಕ್ಷ CURLOPT_USERPWD: {
    ವರ್ಣ *u = ರಿಕ್ತ, *p = ರಿಕ್ತ;
    result = setstropt_userpwd(ptr, &u, &p);
    ಯದಿ(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_USERNAME, u);
      u = ರಿಕ್ತ;
    }
    ಯದಿ(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_PASSWORD, p);
      p = ರಿಕ್ತ;
    }
    curlx_free(u);
    curlx_free(p);
    ಪ್ರತ್ಯಾಗಮ result;
  }

  ಪಕ್ಷ CURLOPT_USERNAME:
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_USERNAME, ptr);

  ಪಕ್ಷ CURLOPT_PASSWORD:
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_PASSWORD, ptr);

  ಪಕ್ಷ CURLOPT_LOGIN_OPTIONS:
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_OPTIONS, ptr);

  ಪಕ್ಷ CURLOPT_XOAUTH2_BEARER:
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_BEARER, ptr);
  ಪಕ್ಷ CURLOPT_RANGE:
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_SET_RANGE, ptr);
  ಪಕ್ಷ CURLOPT_PRIVATE:
    s->private_data = ptr;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_PROTOCOLS_STR:
    ಯದಿ(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      ಯದಿ(!result)
        s->allowed_protocols = protos;
    }
    ಅನ್ಯಥಾ
      /* make a NULL argument reset to default */
      s->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_REDIR_PROTOCOLS_STR:
    ಯದಿ(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      ಯದಿ(!result)
        s->redir_protocols = protos;
    }
    ಅನ್ಯಥಾ
      /* make a NULL argument reset to default */
      s->redir_protocols = (curl_prot_t)CURLPROTO_REDIR;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_DEFAULT_PROTOCOL:
    /* Set the protocol to use when the URL does not include any protocol */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_DEFAULT_PROTOCOL, ptr);
#ifndef CURL_DISABLE_SMTP
  ಪಕ್ಷ CURLOPT_MAIL_FROM:
    /* Set the SMTP mail originator */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_MAIL_FROM, ptr);
  ಪಕ್ಷ CURLOPT_MAIL_AUTH:
    /* Set the SMTP auth originator */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_MAIL_AUTH, ptr);
#endif
  ಪಕ್ಷ CURLOPT_SASL_AUTHZID:
    /* Authorization identity (identity to act as) */
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_SASL_AUTHZID, ptr);
#ifndef CURL_DISABLE_RTSP
  ಪಕ್ಷ CURLOPT_RTSP_SESSION_ID:
    Curl_peer_unlink(&data->state.rtsp_session_origin);
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_RTSP_SESSION_ID, ptr);
  ಪಕ್ಷ CURLOPT_RTSP_STREAM_URI:
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_RTSP_STREAM_URI, ptr);
  ಪಕ್ಷ CURLOPT_RTSP_TRANSPORT:
    ಪ್ರತ್ಯಾಗಮ Curl_setstropt(data, STRING_RTSP_TRANSPORT, ptr);
  ಪಕ್ಷ CURLOPT_INTERLEAVEDATA:
    s->rtp_out = ptr;
    ಭಙ್ಗ;
#endif /* !CURL_DISABLE_RTSP */
  ಪಕ್ಷ CURLOPT_TLSAUTH_USERNAME:
  ಪಕ್ಷ CURLOPT_TLSAUTH_PASSWORD:
  ಪಕ್ಷ CURLOPT_TLSAUTH_TYPE:
  ಪಕ್ಷ CURLOPT_PROXY_TLSAUTH_USERNAME:
  ಪಕ್ಷ CURLOPT_PROXY_TLSAUTH_PASSWORD:
  ಪಕ್ಷ CURLOPT_PROXY_TLSAUTH_TYPE:
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#ifndef CURL_DISABLE_HSTS
  ಪಕ್ಷ CURLOPT_HSTSREADDATA:
    s->hsts_read_userp = ptr;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_HSTSWRITEDATA:
    s->hsts_write_userp = ptr;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_HSTS: {
    ಸಂರಚನಾ curl_slist *h;
    ಯದಿ(!data->hsts) {
      data->hsts = Curl_hsts_init();
      ಯದಿ(!data->hsts)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
    ಯದಿ(ptr) {
      result = Curl_setstropt(data, STRING_HSTS, ptr);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      /* this needs to build a list of filenames to read from, so that it can
         read them later, as we might get a shared HSTS handle to load them
         into */
      h = curl_slist_append(data->state.hstslist, ptr);
      ಯದಿ(!h) {
        curl_slist_free_all(data->state.hstslist);
        data->state.hstslist = ರಿಕ್ತ;
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      }
      data->state.hstslist = h; /* store the list for later use */
    }
    ಅನ್ಯಥಾ {
      /* clear the list of HSTS files */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = ರಿಕ್ತ;
      ಯದಿ(!data->share || !data->share->hsts)
        /* throw away the HSTS cache unless shared */
        Curl_hsts_cleanup(&data->hsts);
    }
    ಭಙ್ಗ;
  }
#endif /* !CURL_DISABLE_HSTS */
#ifndef CURL_DISABLE_ALTSVC
  ಪಕ್ಷ CURLOPT_ALTSVC:
    ಯದಿ(!data->asi) {
      data->asi = Curl_altsvc_init();
      ಯದಿ(!data->asi)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
    result = Curl_setstropt(data, STRING_ALTSVC, ptr);
    ಯದಿ(result)
      ಭಙ್ಗ;
    ಯದಿ(ptr)
      ಪ್ರತ್ಯಾಗಮ Curl_altsvc_load(data->asi, ptr);
    ಭಙ್ಗ;
#endif /* !CURL_DISABLE_ALTSVC */
  ಪಕ್ಷ CURLOPT_ECH:
    ಪ್ರತ್ಯಾಗಮ setopt_ech(data, ptr);
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode setopt_cptr(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                            ವರ್ಣ *ptr)
{
  ಪ್ರಕಾರನಿರ್ದೇಶ CURLcode (*ptrfunc)(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                              ವರ್ಣ *ptr);
  /* Order by likeliness */
  ಸ್ಥಿರ ಧ್ರುವ ptrfunc setopt_call[] = {
    setopt_cptr_misc,
#ಯದಿ defined(USE_SSL) || defined(USE_SSH)
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
#ಯದಿ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
    setopt_cptr_http_mqtt,
#endif
  };
  size_t i;

  ಪ್ರತಿ(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, ptr);
    ಯದಿ(result != CURLE_UNKNOWN_OPTION)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
}

ಸ್ಥಿರ CURLcode setopt_func(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                            va_list param)
{
  ಸಂರಚನಾ UserDefined *s = &data->set;
  ವಿಕಲ್ಪ(option) {
  ಪಕ್ಷ CURLOPT_PROGRESSFUNCTION:
    /*
     * Progress callback function
     */
    s->fprogress = va_arg(param, curl_progress_callback);
    ಯದಿ(s->fprogress)
      data->progress.callback = TRUE; /* no longer internal */
    ಅನ್ಯಥಾ
      data->progress.callback = FALSE; /* NULL enforces internal */
    ಭಙ್ಗ;

  ಪಕ್ಷ CURLOPT_XFERINFOFUNCTION:
    /*
     * Transfer info callback function
     */
    s->fxferinfo = va_arg(param, curl_xferinfo_callback);
    ಯದಿ(s->fxferinfo)
      data->progress.callback = TRUE; /* no longer internal */
    ಅನ್ಯಥಾ
      data->progress.callback = FALSE; /* NULL enforces internal */

    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_DEBUGFUNCTION:
    /*
     * stderr write callback.
     */
    s->fdebug = va_arg(param, curl_debug_callback);
    /*
     * if the callback provided is NULL, it will use the default callback
     */
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_HEADERFUNCTION:
    /*
     * Set header write callback
     */
    s->fwrite_header = va_arg(param, curl_write_callback);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_WRITEFUNCTION:
    /*
     * Set data write callback
     */
    s->fwrite_func = va_arg(param, curl_write_callback);
    ಯದಿ(!s->fwrite_func)
#ಯದಿ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ತಳ್ಳು
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fwrite_func = (curl_write_callback)fwrite;
#ಯದಿ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ತೆಗೆ
#endif
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_READFUNCTION:
    /*
     * Read data callback
     */
    s->fread_func_set = va_arg(param, curl_read_callback);
    ಯದಿ(!s->fread_func_set) {
      s->is_fread_set = 0;
#ಯದಿ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ತಳ್ಳು
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fread_func_set = (curl_read_callback)fread;
#ಯದಿ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ತೆಗೆ
#endif
    }
    ಅನ್ಯಥಾ
      s->is_fread_set = 1;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_SEEKFUNCTION:
    /*
     * Seek callback. Might be NULL.
     */
    s->seek_func = va_arg(param, curl_seek_callback);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_IOCTLFUNCTION:
    /*
     * I/O control callback. Might be NULL.
     */
    s->ioctl_func = va_arg(param, curl_ioctl_callback);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_SSL_CTX_FUNCTION:
    /*
     * Set an SSL_CTX callback
     */
#ifdef USE_SSL
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      s->ssl_fsslctx = va_arg(param, curl_ssl_ctx_callback);
      ಭಙ್ಗ;
    }
    ಅನ್ಯಥಾ
#endif
      ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;

  ಪಕ್ಷ CURLOPT_SOCKOPTFUNCTION:
    /*
     * socket callback function: called after socket() but before connect()
     */
    s->fsockopt = va_arg(param, curl_sockopt_callback);
    ಭಙ್ಗ;

  ಪಕ್ಷ CURLOPT_OPENSOCKETFUNCTION:
    /*
     * open/create socket callback function: called instead of socket(),
     * before connect()
     */
    s->fopensocket = va_arg(param, curl_opensocket_callback);
    ಭಙ್ಗ;

  ಪಕ್ಷ CURLOPT_CLOSESOCKETFUNCTION:
    /*
     * close socket callback function: called instead of close()
     * when shutting down a connection
     */
    s->fclosesocket = va_arg(param, curl_closesocket_callback);
    ಭಙ್ಗ;

  ಪಕ್ಷ CURLOPT_RESOLVER_START_FUNCTION:
    /*
     * resolver start callback function: called before a new resolver request
     * is started
     */
    s->resolver_start = va_arg(param, curl_resolver_start_callback);
    ಭಙ್ಗ;

#ifdef USE_SSH
#ifdef USE_LIBSSH2
  ಪಕ್ಷ CURLOPT_SSH_HOSTKEYFUNCTION:
    /* the callback to check the hostkey without the knownhost file */
    s->ssh_hostkeyfunc = va_arg(param, curl_sshhostkeycallback);
    ಭಙ್ಗ;
#endif

  ಪಕ್ಷ CURLOPT_SSH_KEYFUNCTION:
    /* setting to NULL is fine since the ssh.c functions themselves will
       then revert to use the internal default */
    s->ssh_keyfunc = va_arg(param, curl_sshkeycallback);
    ಭಙ್ಗ;

#endif /* USE_SSH */

#ifndef CURL_DISABLE_RTSP
  ಪಕ್ಷ CURLOPT_INTERLEAVEFUNCTION:
    /* Set the user defined RTP write function */
    s->fwrite_rtp = va_arg(param, curl_write_callback);
    ಭಙ್ಗ;
#endif
#ifndef CURL_DISABLE_FTP
  ಪಕ್ಷ CURLOPT_CHUNK_BGN_FUNCTION:
    s->chunk_bgn = va_arg(param, curl_chunk_bgn_callback);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_CHUNK_END_FUNCTION:
    s->chunk_end = va_arg(param, curl_chunk_end_callback);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_FNMATCH_FUNCTION:
    s->fnmatch = va_arg(param, curl_fnmatch_callback);
    ಭಙ್ಗ;
#endif
#ifndef CURL_DISABLE_HTTP
  ಪಕ್ಷ CURLOPT_TRAILERFUNCTION:
    s->trailer_callback = va_arg(param, curl_trailer_callback);
    ಭಙ್ಗ;
#endif
#ifndef CURL_DISABLE_HSTS
  ಪಕ್ಷ CURLOPT_HSTSREADFUNCTION:
    s->hsts_read = va_arg(param, curl_hstsread_callback);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_HSTSWRITEFUNCTION:
    s->hsts_write = va_arg(param, curl_hstswrite_callback);
    ಭಙ್ಗ;
#endif
  ಪಕ್ಷ CURLOPT_PREREQFUNCTION:
    s->fprereq = va_arg(param, curl_prereq_callback);
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode setopt_offt(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                            curl_off_t offt)
{
  ಸಂರಚನಾ UserDefined *s = &data->set;
  ವಿಕಲ್ಪ(option) {
  ಪಕ್ಷ CURLOPT_TIMEVALUE_LARGE:
    /*
     * This is the value to compare with the remote document with the
     * method set with CURLOPT_TIMECONDITION
     */
    s->timevalue = (time_t)offt;
    ಭಙ್ಗ;

    /* MQTT "borrows" some of the HTTP options */
  ಪಕ್ಷ CURLOPT_POSTFIELDSIZE_LARGE:
    /*
     * The size of the POSTFIELD data to prevent libcurl to do strlen() to
     * figure it out. Enables binary posts.
     */
    ಯದಿ(offt < -1)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

    ಯದಿ(s->postfieldsize < offt && s->str_copypostfields) {
      /* Previous CURLOPT_COPYPOSTFIELDS is no longer valid. */
      curlx_safefree(s->str_copypostfields);
      s->postfields = ರಿಕ್ತ;
    }
    s->postfieldsize = offt;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_INFILESIZE_LARGE:
    /*
     * If known, this should inform curl about the file size of the
     * to-be-uploaded file.
     */
    ಯದಿ(offt < -1)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = offt;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_MAX_SEND_SPEED_LARGE:
    /*
     * When transfer uploads are faster than CURLOPT_MAX_SEND_SPEED_LARGE
     * bytes per second the transfer is throttled..
     */
    ಯದಿ(offt < 0)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_send_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.ul.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_MAX_RECV_SPEED_LARGE:
    /*
     * When receiving data faster than CURLOPT_MAX_RECV_SPEED_LARGE bytes per
     * second the transfer is throttled..
     */
    ಯದಿ(offt < 0)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_recv_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.dl.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_RESUME_FROM_LARGE:
    /*
     * Resume transfer at the given file position
     */
    ಯದಿ(offt < -1)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = offt;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLOPT_MAXFILESIZE_LARGE:
    /*
     * Set the maximum size of a file to download.
     */
    ಯದಿ(offt < 0)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_filesize = offt;
    ಭಙ್ಗ;

  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode setopt_blob(ಸಂರಚನಾ Curl_easy *data, CURLoption option,
                            ಸಂರಚನಾ curl_blob *blob)
{
  ಸಂರಚನಾ UserDefined *s = &data->set;
  ವಿಕಲ್ಪ(option) {
  ಪಕ್ಷ CURLOPT_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setblobopt(&s->blobs[BLOB_CERT], blob);
#ifndef CURL_DISABLE_PROXY
  ಪಕ್ಷ CURLOPT_PROXY_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use for proxy
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setblobopt(&s->blobs[BLOB_CERT_PROXY], blob);
  ಪಕ್ಷ CURLOPT_PROXY_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use for proxy
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setblobopt(&s->blobs[BLOB_ಕೀಲಿ_PROXY], blob);
  ಪಕ್ಷ CURLOPT_PROXY_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection proxy.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO_PROXY], blob);
      ಯದಿ(!result)
        s->proxy_ssl.custom_cablob = TRUE;
      ಪ್ರತ್ಯಾಗಮ result;
    }
#endif
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  ಪಕ್ಷ CURLOPT_PROXY_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      ಪ್ರತ್ಯಾಗಮ Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT_PROXY], blob);
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif
  ಪಕ್ಷ CURLOPT_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use
     */
    ಪ್ರತ್ಯಾಗಮ Curl_setblobopt(&s->blobs[BLOB_ಕೀಲಿ], blob);
  ಪಕ್ಷ CURLOPT_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO], blob);
      ಯದಿ(!result)
        s->ssl.custom_cablob = TRUE;
      ಪ್ರತ್ಯಾಗಮ result;
    }
#endif
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  ಪಕ್ಷ CURLOPT_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    ಯದಿ(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      ಪ್ರತ್ಯಾಗಮ Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT], blob);
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;

  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }
  /* unreachable */
}

/*
 * Do not make Curl_vsetopt() static: it is called from
 * projects/OS400/ccsidcurl.c.
 */
CURLcode Curl_vsetopt(ಸಂರಚನಾ Curl_easy *data, CURLoption option, va_list param)
{
  ಯದಿ(option < CURLOPTTYPE_OBJECTPOINT)
    ಪ್ರತ್ಯಾಗಮ setopt_long(data, option, va_arg(param, long));
  ಅನ್ಯಥಾ ಯದಿ(option < CURLOPTTYPE_FUNCTIONPOINT) {
    /* unfortunately, different pointer types cannot be identified any other
       way than being listed explicitly */
    ವಿಕಲ್ಪ(option) {
    ಪಕ್ಷ CURLOPT_HTTPHEADER:
    ಪಕ್ಷ CURLOPT_QUOTE:
    ಪಕ್ಷ CURLOPT_POSTQUOTE:
    ಪಕ್ಷ CURLOPT_TELNETOPTIONS:
    ಪಕ್ಷ CURLOPT_PREQUOTE:
    ಪಕ್ಷ CURLOPT_HTTP200ALIASES:
    ಪಕ್ಷ CURLOPT_MAIL_RCPT:
    ಪಕ್ಷ CURLOPT_RESOLVE:
    ಪಕ್ಷ CURLOPT_PROXYHEADER:
    ಪಕ್ಷ CURLOPT_CONNECT_TO:
      ಪ್ರತ್ಯಾಗಮ setopt_slist(data, option, va_arg(param, ಸಂರಚನಾ curl_slist *));
    ಪಕ್ಷ CURLOPT_HTTPPOST:         /* curl_httppost * */
    ಪಕ್ಷ CURLOPT_MIMEPOST:         /* curl_mime * */
    ಪಕ್ಷ CURLOPT_STDERR:           /* FILE * */
    ಪಕ್ಷ CURLOPT_SHARE:            /* CURLSH * */
    ಪಕ್ಷ CURLOPT_CURLU:            /* CURLU * */
      ಪ್ರತ್ಯಾಗಮ setopt_pointers(data, option, param);
    ಪಕ್ಷ CURLOPT_STREAM_DEPENDS:   /* CURL * */
    ಪಕ್ಷ CURLOPT_STREAM_DEPENDS_E: /* CURL * */
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    ಸ್ವಭಾವ:
      ಭಙ್ಗ;
    }
    /* the char pointer options */
    ಪ್ರತ್ಯಾಗಮ setopt_cptr(data, option, va_arg(param, ವರ್ಣ *));
  }
  ಅನ್ಯಥಾ ಯದಿ(option < CURLOPTTYPE_OFF_T)
    ಪ್ರತ್ಯಾಗಮ setopt_func(data, option, param);
  ಅನ್ಯಥಾ ಯದಿ(option < CURLOPTTYPE_BLOB)
    ಪ್ರತ್ಯಾಗಮ setopt_offt(data, option, va_arg(param, curl_off_t));
  ಪ್ರತ್ಯಾಗಮ setopt_blob(data, option, va_arg(param, ಸಂರಚನಾ curl_blob *));
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
  ಸಂರಚನಾ Curl_eapi_guard guard;
  CURLcode result;

  ಯದಿ(CURL_EAPI_ENTER(&guard, curl, easy_setopt, &result)) {
    ಸಂರಚನಾ Curl_easy *data = curl;
    va_list arg;

    va_start(arg, option);

    result = Curl_vsetopt(data, option, arg);

    va_end(arg);
    ಯದಿ(result == CURLE_BAD_FUNCTION_ARGUMENT)
      failf(data, "setopt 0x%x got bad argument", (unsigned ಪೂರ್ಣಾಙ್ಕ)option);
  }
  CURL_EAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ result;
}
