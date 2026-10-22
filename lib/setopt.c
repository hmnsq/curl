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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#ଅନ୍ତର୍ଭାଵ <linux/tcp.h>
#ନଚେତଯଦି defined(HAVE_NETINET_TCP_H)
#ଅନ୍ତର୍ଭାଵ <netinet/tcp.h>
#endif

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "content_encoding.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "curl_share.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "setopt.h"
#ଅନ୍ତର୍ଭାଵ "altsvc.h"
#ଅନ୍ତର୍ଭାଵ "hsts.h"
#ଅନ୍ତର୍ଭାଵ "tftp.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"
#ଅନ୍ତର୍ଭାଵ "bufref.h"
#ଅନ୍ତର୍ଭାଵ "vauth/vauth.h"

ସ୍ଥିର CURLcode setopt_set_timeout_sec(timediff_t *ptimeout_ms, long secs)
{
  ଯଦି(secs < 0)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
#ଯଦି LONG_MAX > (TIMEDIFF_T_MAX / 1000)
  ଯଦି(secs > (TIMEDIFF_T_MAX / 1000)) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    ଫେରନ୍ତୁ CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)secs * 1000;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode setopt_set_timeout_ms(timediff_t *ptimeout_ms, long ms)
{
  ଯଦି(ms < 0)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
#ଯଦି LONG_MAX > TIMEDIFF_T_MAX
  ଯଦି(ms > TIMEDIFF_T_MAX) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    ଫେରନ୍ତୁ CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)ms;
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_setstropt(ସଂରଚନା Curl_easy *data,
                        ଗଣନା dupstring id, ଅଚଳ ଅକ୍ଷର *s)
{
  size_t slen = s ? strlen(s) : 0;
  DEBUGASSERT((unsigned)id <= UINT8_MAX);
  ଯଦି(s && (slen > CURL_MAX_ନିବେଶ_LENGTH))
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  ଫେରନ୍ତୁ CURL_EASY_STR_SET(data, (uint8_t)id, s, slen);
}

CURLcode Curl_setblobopt(ସଂରଚନା curl_blob **blobp,
                         ଅଚଳ ସଂରଚନା curl_blob *blob)
{
  /* free the previous storage at `blobp' and replace by a dynamic storage
     copy of blob. If CURL_BLOB_COPY is set, the data is copied. */

  curlx_safefree(*blobp);

  ଯଦି(blob) {
    ସଂରଚନା curl_blob *nblob;
    ଯଦି(!blob->data || !blob->len || (blob->len > CURL_MAX_ନିବେଶ_LENGTH))
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    nblob = (ସଂରଚନା curl_blob *)
      curlx_malloc(ଆକାର(ସଂରଚନା curl_blob) +
                   ((blob->flags & CURL_BLOB_COPY) ? blob->len : 0));
    ଯଦି(!nblob)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    *nblob = *blob;
    ଯଦି(blob->flags & CURL_BLOB_COPY) {
      /* put the data after the blob struct in memory */
      nblob->data = (ଅକ୍ଷର *)nblob + ଆକାର(ସଂରଚନା curl_blob);
      memcpy(nblob->data, blob->data, blob->len);
    }

    *blobp = nblob;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode setstropt_userpwd(ଅଚଳ ଅକ୍ଷର *option, ଅକ୍ଷର **userp,
                                  ଅକ୍ଷର **passwdp)
{
  ଅକ୍ଷର *user = ଖାଲି;
  ଅକ୍ଷର *passwd = ଖାଲି;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Parse the login details if specified. If not, then we treat NULL as a
     hint to clear the existing data */
  ଯଦି(option) {
    size_t len = strlen(option);
    CURLcode result;
    ଯଦି(len > CURL_MAX_ନିବେଶ_LENGTH)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

    result = Curl_parse_login_details(option, len, &user, &passwd, ଖାଲି);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  curlx_free(*userp);
  *userp = user;

  curlx_strzero(*passwdp);
  curlx_free(*passwdp);
  *passwdp = passwd;

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode setstropt_interface(ସଂରଚନା Curl_easy *data, ଅକ୍ଷର *option)
{
  ଯଦି(option && !option[0])
    option = ଖାଲି;
  ଯଦି(option) {
    /* Parse for checking syntax */
    CURLcode result = Curl_parse_interface(option, ଖାଲି, ଖାଲି, ଖାଲି);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    ଫେରନ୍ତୁ CURL_EASY_STR_SET(data, STRING_INTERFACE_OPT,
                             option, strlen(option));
  }
  CURL_EASY_STR_CLEAR(data, STRING_INTERFACE_OPT);
  ଫେରନ୍ତୁ CURLE_OK;
}

#ifdef USE_SSL
#define C_SSLVERSION_ମୂଲ୍ୟ(x)     ((x) & 0xffff)
#define C_SSLVERSION_MAX_ମୂଲ୍ୟ(x) ((unsigned long)(x) & 0xffff0000)
#endif

ସ୍ଥିର CURLcode protocol2num(ଅଚଳ ଅକ୍ଷର *str, curl_prot_t *val)
{
  /*
   * We are asked to cherry-pick protocols, so play it safe and disallow all
   * protocols to start with, and re-add the wanted ones back in.
   */
  *val = 0;

  ଯଦି(!str)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  ଯଦି(curl_strequal(str, "all")) {
    *val = ~(curl_prot_t)0;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  କର {
    ଅଚଳ ଅକ୍ଷର *token = str;
    size_t tlen;

    str = strchr(str, ',');
    tlen = str ? (size_t)(str - token) : strlen(token);
    ଯଦି(tlen) {
      ଅଚଳ ସଂରଚନା Curl_scheme *h = Curl_getn_scheme(token, tlen);

      ଯଦି(!h || !h->run)
        ଫେରନ୍ତୁ CURLE_UNSUPPORTED_PROTOCOL;

      *val |= h->protocol;
    }
  } ଯେତେବେଳେ(str && str++);

  ଯଦି(!*val)
    /* no protocol listed */
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ଫେରନ୍ତୁ CURLE_OK;
}

#ଯଦି !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
ସ୍ଥିର CURLcode httpauth(ସଂରଚନା Curl_easy *data, bool proxy,
                         unsigned long auth)
{
  ଯଦି(auth != CURLAUTH_NONE) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା bitcheck = 0;
    bool authbits = FALSE;
    ଯଦି(auth & CURLAUTH_DIGEST_IE) {
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
    ଯେତେବେଳେ(bitcheck < 31) {
      ଯଦି(auth & (1UL << bitcheck++)) {
        authbits = TRUE;
        ଭାଙ୍ଗ;
      }
    }
    ଯଦି(!authbits)
      ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN; /* no supported types left! */
  }
  ଯଦି(proxy)
    data->set.proxyauth = (uint32_t)auth;
  ଅନ୍ୟଥା
    data->set.httpauth = (uint32_t)auth;
  ଫେରନ୍ତୁ CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */

#ifndef CURL_DISABLE_HTTP
ସ୍ଥିର CURLcode setopt_HTTP_VERSION(ସଂରଚନା Curl_easy *data, long arg)
{
  /*
   * This sets a requested HTTP version to be used. The value is one of
   * the listed enums in curl/curl.h.
   */
  ବାଛ(arg) {
  ସ୍ଥିତି CURL_HTTP_VERSION_NONE:
    /* accepted */
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_HTTP_VERSION_1_0:
  ସ୍ଥିତି CURL_HTTP_VERSION_1_1:
    /* accepted */
    ଭାଙ୍ଗ;
#ifdef USE_HTTP2
  ସ୍ଥିତି CURL_HTTP_VERSION_2_0:
  ସ୍ଥିତି CURL_HTTP_VERSION_2TLS:
  ସ୍ଥିତି CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    /* accepted */
    ଭାଙ୍ଗ;
#endif
#ifdef USE_HTTP3
  ସ୍ଥିତି CURL_HTTP_VERSION_3:
  ସ୍ଥିତି CURL_HTTP_VERSION_3ONLY:
    /* accepted */
    ଭାଙ୍ଗ;
#endif
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    /* not accepted */
    ଯଦି(arg < CURL_HTTP_VERSION_NONE)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    ଫେରନ୍ତୁ CURLE_UNSUPPORTED_PROTOCOL;
  }
  data->set.httpwant = (unsigned ଅକ୍ଷର)arg;
  ଫେରନ୍ତୁ CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP */

#ifdef USE_SSL
CURLcode Curl_setopt_SSLVERSION(ସଂରଚନା Curl_easy *data, CURLoption option,
                                long arg)
{
  /*
   * Set explicit SSL version to try to connect with, as some SSL
   * implementations are lame.
   */
  {
    long version, version_max;
    ସଂରଚନା ssl_easy_config *sslc = &data->set.ssl;
#ifndef CURL_DISABLE_PROXY
    ଯଦି(option != CURLOPT_SSLVERSION)
      sslc = &data->set.proxy_ssl;
#ଅନ୍ୟଥା
    (ଶୂନ୍ୟ)option; /* unused */
#endif
    version = C_SSLVERSION_ମୂଲ୍ୟ(arg);
    version_max = (long)C_SSLVERSION_MAX_ମୂଲ୍ୟ(arg);

    ଯଦି(version < CURL_SSLVERSION_DEFAULT ||
       version == CURL_SSLVERSION_SSLv2 ||
       version == CURL_SSLVERSION_SSLv3 ||
       version >= CURL_SSLVERSION_LAST ||
       version_max < CURL_SSLVERSION_MAX_NONE ||
       version_max >= CURL_SSLVERSION_MAX_LAST)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    ଯଦି(version == CURL_SSLVERSION_DEFAULT)
      version = CURL_SSLVERSION_TLSv1_2;

    sslc->version = (unsigned ଅକ୍ଷର)version;
    sslc->version_max = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)version_max;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}
#endif /* !USE_SSL */

#ifndef CURL_DISABLE_RTSP
ସ୍ଥିର CURLcode setopt_RTSP_REQUEST(ସଂରଚନା Curl_easy *data, long arg)
{
  /*
   * Set the RTSP request method (OPTIONS, SETUP, PLAY, etc...) Would this be
   * better if the RTSPREQ_* were moved into here?
   */
  ଯଦି((arg <= CURL_RTSPREQ_NONE) || (arg >= CURL_RTSPREQ_LAST))
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  data->set.rtspreq = (unsigned ଅକ୍ଷର)arg;
  ଫେରନ୍ତୁ CURLE_OK;
}
#endif /* !CURL_DISABLE_RTSP */

ସ୍ଥିର CURLcode setopt_long_bool(ସଂରଚନା Curl_easy *data, CURLoption option,
                                 long arg)
{
  bool enabled = !!arg;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ok = 1;
  ସଂରଚନା UserDefined *s = &data->set;
  ବାଛ(option) {
  ସ୍ଥିତି CURLOPT_FORBID_REUSE:
    /*
     * When this transfer is done, it must not be left to be reused by a
     * subsequent transfer but shall be closed immediately.
     */
    s->reuse_forbid = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_FRESH_CONNECT:
    /*
     * This transfer shall not use a previously cached connection but
     * should be made with a fresh new connect!
     */
    s->reuse_fresh = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_VERBOSE:
    /*
     * Verbose means infof() calls that give a lot of information about
     * the connection and transfer procedures as well as internal choices.
     */
    s->verbose = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_HEADER:
    /*
     * Set to include the header in the general data output stream.
     */
    s->include_header = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_NOPROGRESS:
    /*
     * Shut off the internal supported progress meter
     */
    data->progress.hide = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_NOBODY:
    /*
     * Do not include the body part in the output data stream.
     */
    s->opt_no_body = enabled;
#ifndef CURL_DISABLE_HTTP
    ଯଦି(s->opt_no_body)
      /* in HTTP lingo, no body means using the HEAD request... */
      s->method = HTTPREQ_HEAD;
    ଅନ୍ୟଥା ଯଦି(s->method == HTTPREQ_HEAD)
      s->method = HTTPREQ_GET;
#endif
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_FAILONERROR:
    /*
     * Do not output the >=400 error code HTML-page, but instead only
     * return error.
     */
    s->http_fail_on_error = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_KEEP_SENDING_ON_ERROR:
    s->http_keep_sending_on_error = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_UPLOAD:
  ସ୍ଥିତି CURLOPT_PUT:
    /*
     * We want to send data to the remote host. If this is HTTP, that equals
     * using the PUT request.
     */
    ଯଦି(enabled) {
      /* If this is HTTP, PUT is what's needed to "upload" */
      s->method = HTTPREQ_PUT;
      s->opt_no_body = FALSE; /* this is implied */
    }
    ଅନ୍ୟଥା
      /* In HTTP, the opposite of upload is GET (unless NOBODY is true as
         then this can be changed to HEAD later on) */
      s->method = HTTPREQ_GET;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_FILETIME:
    /*
     * Try to get the file time of the remote document. The time will
     * later (possibly) become available using curl_easy_getinfo().
     */
    s->get_filetime = enabled;
    ଭାଙ୍ଗ;
#ifndef CURL_DISABLE_HTTP
  ସ୍ଥିତି CURLOPT_HTTP09_ALLOWED:
    s->http09_allowed = enabled;
    ଭାଙ୍ଗ;
#ifndef CURL_DISABLE_COOKIES
  ସ୍ଥିତି CURLOPT_COOKIESESSION:
    /*
     * Set this option to TRUE to start a new "cookie session". It will
     * prevent the forthcoming read-cookies-from-file actions to accept
     * cookies that are marked as being session cookies, as they belong to a
     * previous session.
     */
    s->cookiesession = enabled;
    ଭାଙ୍ଗ;
#endif
  ସ୍ଥିତି CURLOPT_AUTOREFERER:
    /*
     * Switch on automatic referer that gets set if curl follows locations.
     */
    s->http_auto_referer = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_TRANSFER_ENCODING:
    s->http_transfer_encoding = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_UNRESTRICTED_AUTH:
    /*
     * Send authentication (user+password) when following locations, even when
     * hostname changed.
     */
    s->allow_auth_to_other_hosts = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_HTTP_TRANSFER_DECODING:
    /*
     * disable libcurl transfer encoding is used
     */
    s->http_te_skip = !enabled; /* reversed */
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_HTTP_CONTENT_DECODING:
    /*
     * raw data passed to the application when content encoding is used
     */
    s->http_ce_skip = !enabled; /* reversed */
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_HTTPGET:
    /*
     * Set to force us do HTTP GET
     */
    ଯଦି(enabled) {
      s->method = HTTPREQ_GET;
      s->opt_no_body = FALSE; /* this is implied */
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_POST:
    /* Does this option serve a purpose anymore? Yes it does, when
       CURLOPT_POSTFIELDS is not used and the POST data is read off the
       callback! */
    ଯଦି(enabled) {
      s->method = HTTPREQ_POST;
      s->opt_no_body = FALSE; /* this is implied */
    }
    ଅନ୍ୟଥା
      s->method = HTTPREQ_GET;
    ଭାଙ୍ଗ;
#endif /* !CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_PROXY
  ସ୍ଥିତି CURLOPT_HTTPPROXYTUNNEL:
    /*
     * Tunnel operations through the proxy instead of normal proxy use
     */
    s->tunnel_thru_httpproxy = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_HAPROXYPROTOCOL:
    /*
     * Set to send the HAProxy Proxy Protocol header
     */
    s->haproxyprotocol = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_PROXY_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for proxy.
     */
    s->proxy_ssl.verifypeer = enabled;

    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_PROXY_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for proxy
     */
    s->proxy_ssl.verifyhost = enabled;
    ok = 2;
    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_PROXY_TRANSFER_MODE:
    /*
     * set transfer mode (;type=<a|i>) when doing FTP via an HTTP proxy
     */
    s->proxy_transfer_mode = enabled;
    ଭାଙ୍ଗ;
#endif /* !CURL_DISABLE_PROXY */
#ଯଦି defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ସ୍ଥିତି CURLOPT_SOCKS5_GSSAPI_NEC:
    /*
     * Set flag for NEC SOCKS5 support
     */
    s->socks5_gssapi_nec = enabled;
    ଭାଙ୍ଗ;
#endif
#ifdef CURL_LIST_ONLY_PROTOCOL
  ସ୍ଥିତି CURLOPT_DIRLISTONLY:
    /*
     * An option that changes the command to one that asks for a list only, no
     * file info details. Used for FTP, POP3 and SFTP.
     */
    s->list_only = enabled;
    ଭାଙ୍ଗ;
#endif
  ସ୍ଥିତି CURLOPT_APPEND:
    /*
     * We want to upload and append to an existing file. Used for FTP and
     * SFTP.
     */
    s->remote_append = enabled;
    ଭାଙ୍ଗ;
#ifndef CURL_DISABLE_FTP
  ସ୍ଥିତି CURLOPT_FTP_USE_EPRT:
    s->ftp_use_eprt = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_FTP_USE_EPSV:
    s->ftp_use_epsv = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_FTP_USE_PRET:
    s->ftp_use_pret = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_FTP_SKIP_PASV_IP:
    /*
     * Enable or disable FTP_SKIP_PASV_IP, which will disable/enable the
     * bypass of the IP address in PASV responses.
     */
    s->ftp_skip_ip = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_WILDCARDMATCH:
    s->wildcard_enabled = enabled;
    ଭାଙ୍ଗ;
#endif
  ସ୍ଥିତି CURLOPT_CRLF:
    /*
     * Kludgy option to enable CRLF conversions. Subject for removal.
     */
    s->crlf = enabled;
    ଭାଙ୍ଗ;
#ifndef CURL_DISABLE_TFTP
  ସ୍ଥିତି CURLOPT_TFTP_NO_OPTIONS:
    /*
     * Option that prevents libcurl from sending TFTP option requests to the
     * server.
     */
    s->tftp_no_options = enabled;
    ଭାଙ୍ଗ;
#endif /* !CURL_DISABLE_TFTP */
  ସ୍ଥିତି CURLOPT_TRANSFERTEXT:
    /*
     * This option was previously named 'FTPASCII'. Renamed to work with
     * more protocols than merely FTP.
     *
     * Transfer using ASCII (instead of BINARY).
     */
    s->prefer_ascii = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying.
     */
    s->ssl.verifypeer = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    ଭାଙ୍ଗ;
#ifndef CURL_DISABLE_DOH
  ସ୍ଥିତି CURLOPT_DOH_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for DoH.
     */
    s->doh_verifypeer = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_DOH_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for DoH
     */
    s->doh_verifyhost = enabled;
    ok = 2;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_DOH_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying for DoH.
     */
    ଯଦି(!Curl_ssl_cert_status_request())
      ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;

    s->doh_verifystatus = enabled;
    ok = 2;
    ଭାଙ୍ଗ;
#endif /* !CURL_DISABLE_DOH */
  ସ୍ଥିତି CURLOPT_SSL_VERIFYHOST:
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
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying.
     */
    ଯଦି(!Curl_ssl_cert_status_request())
      ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;

    s->ssl.verifystatus = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_CERTINFO:
#ifdef USE_SSL
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_CERTINFO))
      s->ssl.certinfo = enabled;
    ଅନ୍ୟଥା
#endif
      ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_NOSIGNAL:
    /*
     * The application asks not to set any signal() or alarm() handlers,
     * even when using a timeout.
     */
    s->no_signal = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_TCP_NODELAY:
    /*
     * Enable or disable TCP_NODELAY, which will disable/enable the Nagle
     * algorithm
     */
    s->tcp_nodelay = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_IGNORE_CONTENT_LENGTH:
    s->ignorecl = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_SSL_SESSIONID_CACHE:
    s->ssl.cache_session = enabled;
#ifndef CURL_DISABLE_PROXY
    s->proxy_ssl.cache_session = s->ssl.cache_session;
#endif
    ଭାଙ୍ଗ;
#ifdef USE_SSH
  ସ୍ଥିତି CURLOPT_SSH_COMPRESSION:
    s->ssh_compression = enabled;
    ଭାଙ୍ଗ;
#endif /* !USE_SSH */
#ifndef CURL_DISABLE_SMTP
  ସ୍ଥିତି CURLOPT_MAIL_RCPT_ALLOWFAILS:
    /* allow RCPT TO command to fail for some recipients */
    s->mail_rcpt_allowfails = enabled;
    ଭାଙ୍ଗ;
#endif /* !CURL_DISABLE_SMTP */
  ସ୍ଥିତି CURLOPT_SASL_IR:
    /* Enable/disable SASL initial response */
    s->sasl_ir = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_TCP_KEEPALIVE:
    s->tcp_keepalive = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_TCP_FASTOPEN:
#ଯଦି defined(CONNECT_DATA_IDEMPOTENT) || defined(MSG_FASTOPEN) || \
  defined(TCP_FASTOPEN_CONNECT)
    s->tcp_fastopen = enabled;
    ଭାଙ୍ଗ;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif
  ସ୍ଥିତି CURLOPT_SSL_ENABLE_ALPN:
    s->ssl_enable_alpn = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_PATH_AS_IS:
    s->path_as_is = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_PIPEWAIT:
    s->pipewait = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_SUPPRESS_CONNECT_HEADERS:
    s->suppress_connect_headers = enabled;
    ଭାଙ୍ଗ;
#ifndef CURL_DISABLE_SHUFFLE_DNS
  ସ୍ଥିତି CURLOPT_DNS_SHUFFLE_ADDRESSES:
    s->dns_shuffle_addresses = enabled;
    ଭାଙ୍ଗ;
#endif
  ସ୍ଥିତି CURLOPT_DISALLOW_USERNAME_IN_URL:
    s->disallow_username_in_url = enabled;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_QUICK_EXIT:
    s->quick_exit = enabled;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }
  ଯଦି((arg > ok) || (arg < 0))
    /* reserve other values for future use */
    infof(data, "boolean setopt(%d) got unsupported argument %ld,"
          " treated as %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)option, arg, enabled);

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ମୂଲ୍ୟ_range(long *ମୂଲ୍ୟ, long below_error, long min, long max)
{
  ଯଦି(*ମୂଲ୍ୟ < below_error)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ଅନ୍ୟଥା ଯଦି(*ମୂଲ୍ୟ < min)
    *ମୂଲ୍ୟ = min;
  ଅନ୍ୟଥା ଯଦି(*ମୂଲ୍ୟ > max)
    *ମୂଲ୍ୟ = max;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode setopt_long_net(ସଂରଚନା Curl_easy *data, CURLoption option,
                                long arg)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା UserDefined *s = &data->set;

  ବାଛ(option) {
  ସ୍ଥିତି CURLOPT_DNS_CACHE_TIMEOUT:
    ଯଦି(arg != -1)
      ଫେରନ୍ତୁ setopt_set_timeout_sec(&s->dns_cache_timeout_ms, arg);
    s->dns_cache_timeout_ms = -1;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_MAXCONNECTS:
    result = ମୂଲ୍ୟ_range(&arg, 0, 0, INT_MAX);
    ଯଦି(!result)
      s->maxconnects = arg ? (uint32_t)arg : DEFAULT_CONNCACHE_SIZE;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_SERVER_RESPONSE_TIMEOUT:
    ଫେରନ୍ତୁ setopt_set_timeout_sec(&s->server_response_timeout, arg);
  ସ୍ଥିତି CURLOPT_SERVER_RESPONSE_TIMEOUT_MS:
    ଫେରନ୍ତୁ setopt_set_timeout_ms(&s->server_response_timeout, arg);
  ସ୍ଥିତି CURLOPT_ତଳ_SPEED_LIMIT:
    ଯଦି(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଅନ୍ୟଥା
      s->ତଳ_speed_limit = arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_ତଳ_SPEED_TIME:
    result = ମୂଲ୍ୟ_range(&arg, 0, 0, USHRT_MAX);
    ଯଦି(!result)
      s->ତଳ_speed_time = (uint16_t)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_PORT:
    ଯଦି((arg < 0) || (arg > 65535))
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    s->use_port = (unsigned short)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_TIMEOUT:
    ଫେରନ୍ତୁ setopt_set_timeout_sec(&s->timeout, arg);
  ସ୍ଥିତି CURLOPT_TIMEOUT_MS:
    ଫେରନ୍ତୁ setopt_set_timeout_ms(&s->timeout, arg);
  ସ୍ଥିତି CURLOPT_CONNECTTIMEOUT:
    ଫେରନ୍ତୁ setopt_set_timeout_sec(&s->connecttimeout, arg);
  ସ୍ଥିତି CURLOPT_CONNECTTIMEOUT_MS:
    ଫେରନ୍ତୁ setopt_set_timeout_ms(&s->connecttimeout, arg);
#ifndef CURL_DISABLE_BINDLOCAL
  ସ୍ଥିତି CURLOPT_LOCALPORT:
    ଯଦି((arg < 0) || (arg > 65535))
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    s->localport = curlx_sltous(arg);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_LOCALPORTRANGE:
    ଯଦି((arg < 0) || (arg > 65535))
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    s->localportrange = curlx_sltous(arg);
    ଭାଙ୍ଗ;
#endif
  ସ୍ଥିତି CURLOPT_BUFFERSIZE:
    result = ମୂଲ୍ୟ_range(&arg, 0, READBUFFER_MIN, READBUFFER_MAX);
    ଯଦି(!result)
      s->buffer_size = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_UPLOAD_BUFFERSIZE:
    result = ମୂଲ୍ୟ_range(&arg, 0, UPLOADBUFFER_MIN, UPLOADBUFFER_MAX);
    ଯଦି(!result)
      s->upload_buffer_size = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_MAXFILESIZE:
    ଯଦି(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଅନ୍ୟଥା
      s->max_filesize = arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_IPRESOLVE:
    ଯଦି((arg < CURL_IPRESOLVE_WHATEVER) || (arg > CURL_IPRESOLVE_V6))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଅନ୍ୟଥା
      s->ipver = (unsigned ଅକ୍ଷର)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_CONNECT_ONLY:
    ଯଦି(arg < 0 || arg > 2)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଅନ୍ୟଥା {
      s->connect_only = !!arg;
      s->connect_only_ws = (arg == 2);
    }
    ଭାଙ୍ଗ;
#ifdef USE_IPV6
  ସ୍ଥିତି CURLOPT_ADDRESS_SCOPE:
#ଯଦି SIZEOF_LONG > 4
    ଯଦି((unsigned long)arg > UINT_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଅନ୍ୟଥା
#endif
    s->scope_id = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)arg;
    ଭାଙ୍ଗ;
#endif
  ସ୍ଥିତି CURLOPT_TCP_KEEPIDLE:
    result = ମୂଲ୍ୟ_range(&arg, 0, 0, INT_MAX);
    ଯଦି(!result)
      s->tcp_keepidle = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_TCP_KEEPINTVL:
    result = ମୂଲ୍ୟ_range(&arg, 0, 0, INT_MAX);
    ଯଦି(!result)
      s->tcp_keepintvl = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_TCP_KEEPCNT:
    result = ମୂଲ୍ୟ_range(&arg, 0, 0, INT_MAX);
    ଯଦି(!result)
      s->tcp_keepcnt = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
    ଫେରନ୍ତୁ setopt_set_timeout_ms(&s->happy_eyeballs_timeout, arg);
  ସ୍ଥିତି CURLOPT_UPKEEP_INTERVAL_MS:
    ଫେରନ୍ତୁ setopt_set_timeout_ms(&s->upkeep_interval_ms, arg);
  ସ୍ଥିତି CURLOPT_MAXAGE_CONN:
    ଫେରନ୍ତୁ setopt_set_timeout_sec(&s->conn_max_idle_ms, arg);
  ସ୍ଥିତି CURLOPT_MAXLIFETIME_CONN:
    ଫେରନ୍ତୁ setopt_set_timeout_sec(&s->conn_max_age_ms, arg);
  ସ୍ଥିତି CURLOPT_DNS_USE_GLOBAL_CACHE:
    /* deprecated */
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode setopt_long_ssl(ସଂରଚନା Curl_easy *data, CURLoption option,
                                long arg)
{
#ifdef USE_SSL
  CURLcode result = CURLE_OK;
  ସଂରଚନା UserDefined *s = &data->set;
  ବାଛ(option) {
  ସ୍ଥିତି CURLOPT_CA_CACHE_TIMEOUT:
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_CA_CACHE)) {
      result = ମୂଲ୍ୟ_range(&arg, -1, -1, INT_MAX);
      ଯଦି(!result)
        s->ssl_ca_cache_timeout = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)arg;
    }
    ଅନ୍ୟଥା
      result = CURLE_NOT_BUILT_IN;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_SSLVERSION:
#ifndef CURL_DISABLE_PROXY
  ସ୍ଥିତି CURLOPT_PROXY_SSLVERSION:
#endif
    ଫେରନ୍ତୁ Curl_setopt_SSLVERSION(data, option, arg);
  ସ୍ଥିତି CURLOPT_SSL_FALSESTART:
    result = CURLE_NOT_BUILT_IN;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_USE_SSL:
    ଯଦି((arg < CURLUSESSL_NONE) || (arg >= CURLUSESSL_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଅନ୍ୟଥା
      s->use_ssl = (unsigned ଅକ୍ଷର)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_SSL_OPTIONS:
    s->ssl.ssl_options = (unsigned ଅକ୍ଷର)(arg & 0xff);
    ଭାଙ୍ଗ;
#ifndef CURL_DISABLE_PROXY
  ସ୍ଥିତି CURLOPT_PROXY_SSL_OPTIONS:
    s->proxy_ssl.ssl_options = (unsigned ଅକ୍ଷର)(arg & 0xff);
    ଭାଙ୍ଗ;
#endif
  ସ୍ଥିତି CURLOPT_SSL_ENABLE_NPN:
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_SSLENGINE_DEFAULT:
#ifdef USE_OPENSSL
    ଯଦି(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
    data->set.engine_default = (arg == 1);
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }
  ଫେରନ୍ତୁ result;
#ଅନ୍ୟଥା  /* USE_SSL */
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)option;
  (ଶୂନ୍ୟ)arg;
  ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
#endif /* !USE_SSL */
}

#ifndef CURL_DISABLE_PROXY
ସ୍ଥିର ଶୂନ୍ୟ changeproxy(ସଂରଚନା Curl_easy *data)
{
  Curl_auth_digest_cleanup(&data->state.proxydigest);
  memset(&data->state.authproxy, 0, ଆକାର(data->state.authproxy));
}

ସ୍ଥିର CURLcode setopt_long_proxy(ସଂରଚନା Curl_easy *data, CURLoption option,
                                  long arg)
{
  ସଂରଚନା UserDefined *s = &data->set;

  ବାଛ(option) {
  ସ୍ଥିତି CURLOPT_PROXYPORT:
    ଯଦି((arg < 0) || (arg > UINT16_MAX))
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    ଯଦି(arg != s->proxyport)
      changeproxy(data);
    s->proxyport = (uint16_t)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_PROXYAUTH:
    ଫେରନ୍ତୁ httpauth(data, TRUE, (unsigned long)arg);
  ସ୍ଥିତି CURLOPT_PROXYTYPE:
    ଯଦି((arg < CURLPROXY_HTTP) || (arg > CURLPROXY_HTTPS3))
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef USE_PROXY_HTTP3
    ଯଦି(arg == CURLPROXY_HTTPS3)
      ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif
    s->proxytype = (unsigned ଅକ୍ଷର)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_SOCKS5_AUTH: {
    unsigned long auth = (unsigned long)arg;
    ଯଦି(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI | CURLAUTH_ONLY))
      ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
    ଯଦି((auth & CURLAUTH_ONLY) && !(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI)))
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    s->socks5auth = (unsigned ଅକ୍ଷର)(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI));
    s->socks5_auth_only = !!(auth & CURLAUTH_ONLY);
    ଭାଙ୍ଗ;
  }
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}
#ଅନ୍ୟଥା
ସ୍ଥିର CURLcode setopt_long_proxy(ସଂରଚନା Curl_easy *data, CURLoption option,
                                  long arg)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)option;
  (ଶୂନ୍ୟ)arg;
  ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
}
#endif

ସ୍ଥିର CURLcode setopt_long_http(ସଂରଚନା Curl_easy *data, CURLoption option,
                                 long arg)
{
#ifndef CURL_DISABLE_HTTP
  CURLcode result = CURLE_OK;
  ସଂରଚନା UserDefined *s = &data->set;

  ବାଛ(option) {
  ସ୍ଥିତି CURLOPT_FOLLOWLOCATION:
    ଯଦି((unsigned long)arg > 3)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଅନ୍ୟଥା
      s->http_follow_mode = (unsigned ଅକ୍ଷର)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_MAXREDIRS:
    result = ମୂଲ୍ୟ_range(&arg, -1, -1, 0x7fff);
    ଯଦି(!result)
      s->maxredirs = (short)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_POSTREDIR:
    ଯଦି(arg < CURL_REDIR_GET_ALL)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଅନ୍ୟଥା {
      s->post301 = !!(arg & CURL_REDIR_POST_301);
      s->post302 = !!(arg & CURL_REDIR_POST_302);
      s->post303 = !!(arg & CURL_REDIR_POST_303);
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_HEADEROPT:
    s->sep_headers = !!(arg & CURLHEADER_SEPARATE);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_HTTPAUTH:
    ଫେରନ୍ତୁ httpauth(data, FALSE, (unsigned long)arg);
  ସ୍ଥିତି CURLOPT_HTTP_VERSION:
    ଫେରନ୍ତୁ setopt_HTTP_VERSION(data, arg);
  ସ୍ଥିତି CURLOPT_EXPECT_100_TIMEOUT_MS:
    result = ମୂଲ୍ୟ_range(&arg, 0, 0, 0xffff);
    ଯଦି(!result)
      s->expect_100_timeout = (unsigned short)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_STREAM_WEIGHT:
#ଯଦି defined(USE_HTTP2) || defined(USE_HTTP3)
    ଯଦି((arg >= 1) && (arg <= 256))
      s->weight = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)arg;
    ଭାଙ୍ଗ;
#ଅନ୍ୟଥା
    result = CURLE_NOT_BUILT_IN;
    ଭାଙ୍ଗ;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  ସ୍ଥିତି CURLOPT_HTTPSIG_ALGORITHM:
    ଯଦି(arg != CURLHTTPSIG_NONE &&
       arg != CURLHTTPSIG_ED25519 &&
       arg != CURLHTTPSIG_HMAC_SHA256)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    s->httpsig_algorithm = (uint8_t)arg;
    ଯଦି(arg)
      s->httpauth = (uint32_t)CURLAUTH_HTTPSIG;
    ଅନ୍ୟଥା
      s->httpauth &= ~(uint32_t)CURLAUTH_HTTPSIG;
    ଭାଙ୍ଗ;
#endif
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }
  ଫେରନ୍ତୁ result;
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)option;
  (ଶୂନ୍ୟ)arg;
  ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
#endif
}

ସ୍ଥିର CURLcode setopt_long_proto(ସଂରଚନା Curl_easy *data, CURLoption option,
                                  long arg)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା UserDefined *s = &data->set;

  ବାଛ(option) {
#ifndef CURL_DISABLE_TFTP
  ସ୍ଥିତି CURLOPT_TFTP_BLKSIZE:
    result = ମୂଲ୍ୟ_range(&arg, 0, TFTP_BLKSIZE_MIN, TFTP_BLKSIZE_MAX);
    ଯଦି(!result)
      s->tftp_blksize = (unsigned short)arg;
    ଭାଙ୍ଗ;
#endif
#ifndef CURL_DISABLE_NETRC
  ସ୍ଥିତି CURLOPT_NETRC:
    ଯଦି((arg < CURL_NETRC_IGNORED) || (arg >= CURL_NETRC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଅନ୍ୟଥା
      s->use_netrc = (unsigned ଅକ୍ଷର)arg;
    ଭାଙ୍ଗ;
#endif
#ifndef CURL_DISABLE_FTP
  ସ୍ଥିତି CURLOPT_FTP_FILEMETHOD:
    ଯଦି((arg < CURLFTPMETHOD_DEFAULT) || (arg >= CURLFTPMETHOD_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଅନ୍ୟଥା
      s->ftp_filemethod = (unsigned ଅକ୍ଷର)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_FTP_SSL_CCC:
    ଯଦି((arg < CURLFTPSSL_CCC_NONE) || (arg >= CURLFTPSSL_CCC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଅନ୍ୟଥା
      s->ftp_ccc = (unsigned ଅକ୍ଷର)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_FTPSSLAUTH:
    ଯଦି((arg < CURLFTPAUTH_DEFAULT) || (arg >= CURLFTPAUTH_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଅନ୍ୟଥା
      s->ftpsslauth = (unsigned ଅକ୍ଷର)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_ACCEPTTIMEOUT_MS:
    ଫେରନ୍ତୁ setopt_set_timeout_ms(&s->accepttimeout, arg);
#endif
#ଯଦି !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  ସ୍ଥିତି CURLOPT_FTP_CREATE_MISSING_DIRS:
    ଯଦି((arg < CURLFTP_CREATE_DIR_NONE) || (arg > CURLFTP_CREATE_DIR_RETRY))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଅନ୍ୟଥା
      s->ftp_create_missing_dirs = (unsigned ଅକ୍ଷର)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_NEW_FILE_PERMS:
    ଯଦି((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଅନ୍ୟଥା
      s->new_file_perms = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)arg;
    ଭାଙ୍ଗ;
#endif
#ifndef CURL_DISABLE_RTSP
  ସ୍ଥିତି CURLOPT_RTSP_REQUEST:
    ଫେରନ୍ତୁ setopt_RTSP_REQUEST(data, arg);
  ସ୍ଥିତି CURLOPT_RTSP_CLIENT_CSEQ:
    result = ମୂଲ୍ୟ_range(&arg, 0, 0, INT_MAX);
    ଯଦି(!result)
      data->state.rtsp_next_client_CSeq = (uint32_t)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_RTSP_SERVER_CSEQ:
    result = ମୂଲ୍ୟ_range(&arg, 0, 0, INT_MAX);
    ଯଦି(!result)
      data->state.rtsp_next_server_CSeq = (uint32_t)arg;
    ଭାଙ୍ଗ;
#endif
#ifdef USE_SSH
  ସ୍ଥିତି CURLOPT_SSH_AUTH_TYPES:
    s->ssh_auth_types = (uint32_t)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_NEW_DIRECTORY_PERMS:
    ଯଦି((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଅନ୍ୟଥା
      s->new_directory_perms = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)arg;
    ଭାଙ୍ଗ;
#endif
  ସ୍ଥିତି CURLOPT_PROTOCOLS:
    s->allowed_protocols = (curl_prot_t)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_REDIR_PROTOCOLS:
    s->redir_protocols = (curl_prot_t)arg;
    ଭାଙ୍ଗ;
#ifndef CURL_DISABLE_WEBSOCKETS
  ସ୍ଥିତି CURLOPT_WS_OPTIONS:
    s->ws_raw_mode = !!(arg & CURLWS_RAW_MODE);
    s->ws_no_auto_pong = !!(arg & CURLWS_NOAUTOPONG);
    ଭାଙ୍ଗ;
#endif
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode setopt_long_misc(ସଂରଚନା Curl_easy *data, CURLoption option,
                                 long arg)
{
  ସଂରଚନା UserDefined *s = &data->set;

  ବାଛ(option) {
  ସ୍ଥିତି CURLOPT_TIMECONDITION:
    ଯଦି((arg < CURL_TIMECOND_NONE) || (arg >= CURL_TIMECOND_LAST))
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    s->timecondition = (unsigned ଅକ୍ଷର)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_TIMEVALUE:
    s->timevalue = (time_t)arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_POSTFIELDSIZE:
    ଯଦି(arg < -1)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    ଯଦି(s->postfieldsize < arg && s->str_copypostfields) {
      curlx_safefree(s->str_copypostfields);
      s->postfields = ଖାଲି;
    }
    s->postfieldsize = arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_INFILESIZE:
    ଯଦି(arg < -1)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_RESUME_FROM:
    ଯଦି(arg < -1)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = arg;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_UPLOAD_FLAGS:
    s->upload_flags = (unsigned ଅକ୍ଷର)arg;
    ଭାଙ୍ଗ;
#ifndef CURL_DISABLE_MIME
  ସ୍ଥିତି CURLOPT_MIME_OPTIONS:
    s->mime_formescape = !!(arg & CURLMIMEOPT_FORMESCAPE);
    ଭାଙ୍ଗ;
#endif
#ifndef CURL_DISABLE_HSTS
  ସ୍ଥିତି CURLOPT_HSTS_CTRL:
    ଯଦି(arg & CURLHSTS_ENABLE) {
      ଯଦି(!data->hsts) {
        data->hsts = Curl_hsts_init();
        ଯଦି(!data->hsts)
          ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      }
    }
    ଅନ୍ୟଥା ଯଦି(!data->share || !data->share->hsts) {
      /* throw away the HSTS cache unless shared */
      Curl_hsts_cleanup(&data->hsts);
      /* flush all the entries */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = ଖାଲି;
    }
    ଅନ୍ୟଥା
      /* detach from shared HSTS cache without freeing it */
      data->hsts = ଖାଲି;
    ଭାଙ୍ଗ;
#endif
#ifndef CURL_DISABLE_ALTSVC
  ସ୍ଥିତି CURLOPT_ALTSVC_CTRL:
    ଫେରନ୍ତୁ Curl_altsvc_ctrl(data, arg);
#endif
#ଯଦି defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ସ୍ଥିତି CURLOPT_GSSAPI_DELEGATION:
    s->gssapi_delegation = (unsigned ଅକ୍ଷର)arg &
      (CURLGSSAPI_DELEGATION_POLICY_FLAG | CURLGSSAPI_DELEGATION_FLAG);
    ଭାଙ୍ଗ;
#endif
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode setopt_long(ସଂରଚନା Curl_easy *data, CURLoption option,
                            long arg)
{
  ପ୍ରକାରବ୍ୟାଖ୍ୟା CURLcode (*setoptfunc)(ସଂରଚନା Curl_easy *data,
                                 CURLoption option, long arg);
  ସ୍ଥିର ଅଚଳ setoptfunc setopt_call[] = {
    setopt_long_bool,
    setopt_long_net,
    setopt_long_http,
    setopt_long_proxy,
    setopt_long_ssl,
    setopt_long_proto,
    setopt_long_misc
  };
  size_t i;

  ପାଇଁ(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, arg);
    ଯଦି(result != CURLE_UNKNOWN_OPTION)
      ଫେରନ୍ତୁ result;
  }
  ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
}

ସ୍ଥିର CURLcode setopt_slist(ସଂରଚନା Curl_easy *data, CURLoption option,
                             ସଂରଚନା curl_slist *slist)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା UserDefined *s = &data->set;
  ବାଛ(option) {
#ifndef CURL_DISABLE_PROXY
  ସ୍ଥିତି CURLOPT_PROXYHEADER:
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
    ଭାଙ୍ଗ;
#endif
#ifndef CURL_DISABLE_HTTP
  ସ୍ଥିତି CURLOPT_HTTP200ALIASES:
    /*
     * Set a list of aliases for HTTP 200 in response header
     */
    s->http200aliases = slist;
    ଭାଙ୍ଗ;
#endif
#ଯଦି !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  ସ୍ଥିତି CURLOPT_POSTQUOTE:
    /*
     * List of RAW FTP commands to use after a transfer
     */
    s->postquote = slist;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_PREQUOTE:
    /*
     * List of RAW FTP commands to use prior to RETR (Wesley Laxton)
     */
    s->prequote = slist;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_QUOTE:
    /*
     * List of RAW FTP commands to use before a transfer
     */
    s->quote = slist;
    ଭାଙ୍ଗ;
#endif
  ସ୍ଥିତି CURLOPT_RESOLVE:
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
    ଭାଙ୍ଗ;
#ଯଦି !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MIME)
  ସ୍ଥିତି CURLOPT_HTTPHEADER:
    /*
     * Set a list with HTTP headers to use (or replace internals with)
     */
    s->headers = slist;
    ଭାଙ୍ଗ;
#endif
#ifndef CURL_DISABLE_TELNET
  ସ୍ଥିତି CURLOPT_TELNETOPTIONS:
    /*
     * Set a linked list of telnet options
     */
    s->telnet_options = slist;
    ଭାଙ୍ଗ;
#endif
#ifndef CURL_DISABLE_SMTP
  ସ୍ଥିତି CURLOPT_MAIL_RCPT:
    /* Set the list of mail recipients */
    s->mail_rcpt = slist;
    ଭାଙ୍ଗ;
#endif
  ସ୍ଥିତି CURLOPT_CONNECT_TO:
    s->connect_to = slist;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }
  ଫେରନ୍ତୁ result;
}

#ଯଦି !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
ସ୍ଥିର CURLcode setopt_mimepost(ସଂରଚନା Curl_easy *data, curl_mime *mimep)
{
  /*
   * Set to make us do MIME POST
   */
  CURLcode result;
  ସଂରଚନା UserDefined *s = &data->set;
  ଯଦି(!s->mimepostp) {
    s->mimepostp = curlx_malloc(ଆକାର(*s->mimepostp));
    ଯଦି(!s->mimepostp)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    Curl_mime_initpart(s->mimepostp);
  }

  result = Curl_mime_set_subparts(s->mimepostp, mimep, FALSE);
  ଯଦି(!result) {
    s->method = HTTPREQ_POST_MIME;
    s->opt_no_body = FALSE; /* this is implied */
#ifndef CURL_DISABLE_FORM_API
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = ଖାଲି;
#endif
  }
  ଫେରନ୍ତୁ result;
}
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */

ସ୍ଥିର CURLcode setopt_share(ସଂରଚନା Curl_easy *data, ସଂରଚନା Curl_share *set)
{
  CURLcode result;

  ଯଦି(data->conn) {
    /* As this handle already has a connection attached, changing share now
       would be complicated and error-prone */
    infof(data, "Cannot change share object while in use");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
  }
  ଅନ୍ୟଥା {
    /* disconnect from old share, if any and possible */
    result = Curl_share_easy_unlink(data);
    ଯଦି(!result && GOOD_SHARE_HANDLE(set))
      /* use new share if it set */
      result = Curl_share_easy_link(data, set);
  }
  ଫେରନ୍ତୁ result;
}

/* assorted pointer type arguments */
ସ୍ଥିର CURLcode setopt_pointers(ସଂରଚନା Curl_easy *data, CURLoption option,
                                va_list param)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା UserDefined *s = &data->set;
  ବାଛ(option) {
  ସ୍ଥିତି CURLOPT_CURLU:
    /*
     * pass CURLU to set URL
     */
    Curl_bufref_free(&data->state.url);
    CURL_EASY_STR_CLEAR(data, STRING_SET_URL);
    s->uh = va_arg(param, CURLU *);
    ଭାଙ୍ଗ;
#ifndef CURL_DISABLE_HTTP
#ifndef CURL_DISABLE_FORM_API
  ସ୍ଥିତି CURLOPT_HTTPPOST:
    /*
     * Set to make us do HTTP POST. Legacy API-style.
     */
    s->httppost = va_arg(param, ସଂରଚନା curl_httppost *);
    s->method = HTTPREQ_POST_FORM;
    s->opt_no_body = FALSE; /* this is implied */
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = ଖାଲି;
    ଭାଙ୍ଗ;
#endif /* !CURL_DISABLE_FORM_API */
#endif /* !CURL_DISABLE_HTTP */
#ଯଦି !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
  ସ୍ଥିତି CURLOPT_MIMEPOST:
    result = setopt_mimepost(data, va_arg(param, curl_mime *));
    ଭାଙ୍ଗ;
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */
  ସ୍ଥିତି CURLOPT_STDERR:
    /*
     * Set to a FILE * that should receive all error writes. This
     * defaults to stderr for normal operations.
     */
    s->err = va_arg(param, FILE *);
    ଯଦି(!s->err)
      s->err = stderr;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_SHARE:
    ଫେରନ୍ତୁ setopt_share(data, va_arg(param, ସଂରଚନା Curl_share *));

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }
  ଫେରନ୍ତୁ result;
}

#ifndef CURL_DISABLE_COOKIES
ସ୍ଥିର CURLcode cookielist(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *ptr)
{
  CURLcode result = CURLE_OK;
  ଯଦି(!ptr)
    ଫେରନ୍ତୁ CURLE_OK;

  ଯଦି(curl_strequal(ptr, "ALL")) {
    /* clear all cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearall(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  ଅନ୍ୟଥା ଯଦି(curl_strequal(ptr, "SESS")) {
    /* clear session cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearsess(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  ଅନ୍ୟଥା ଯଦି(curl_strequal(ptr, "FLUSH")) {
    /* flush cookies to file, takes care of the locking */
    Curl_flush_cookies(data, FALSE);
  }
  ଅନ୍ୟଥା ଯଦି(curl_strequal(ptr, "RELOAD")) {
    /* reload cookies from file */
    ଫେରନ୍ତୁ Curl_cookie_loadfiles(data, COOKIE_NOPSL |
                                 (data->set.cookiesession ?
                                  COOKIE_NOSESSION : 0));
  }
  ଅନ୍ୟଥା {
    ଯଦି(!data->cookies) {
      /* if cookie engine was not running, activate it */
      data->cookies = Curl_cookie_init();
      ଯଦି(!data->cookies)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      data->state.cookie_engine = TRUE;
    }

    /* general protection against mistakes and abuse */
    ଯଦି(strlen(ptr) > CURL_MAX_ନିବେଶ_LENGTH)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

    /* Adding these cookies without the PSL check, because the PSL is not
       initialized until *perform() time, and this might be called before
       that */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    ଯଦି(checkprefix("Set-Cookie:", ptr))
      /* HTTP Header format line */
      result = Curl_cookie_add(data, data->cookies, ptr + 11,
                               ଖାଲି, ଖାଲି,
                               COOKIE_HTTPHEADER | COOKIE_SECURE |
                               COOKIE_NOPSL);
    ଅନ୍ୟଥା
      /* Netscape format line */
      result = Curl_cookie_add(data, data->cookies, ptr, ଖାଲି,
                               ଖାଲି, COOKIE_SECURE | COOKIE_NOPSL);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cookiefile(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *ptr)
{
  /*
   * Set cookie file to read and parse. Can be used multiple times.
   */
  ଯଦି(ptr) {
    ସଂରଚନା curl_slist *cl;
    /* general protection against mistakes and abuse */
    ଯଦି(strlen(ptr) > CURL_MAX_ନିବେଶ_LENGTH)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    /* append the cookie filename to the list of filenames, and deal with
       them later */
    cl = curl_slist_append(data->state.cookielist, ptr);
    ଯଦି(!cl) {
      curl_slist_free_all(data->state.cookielist);
      data->state.cookielist = ଖାଲି;
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
    data->state.cookielist = cl; /* store the list for later use */
  }
  ଅନ୍ୟଥା {
    /* clear the list of cookie files */
    curl_slist_free_all(data->state.cookielist);
    data->state.cookielist = ଖାଲି;

    ଯଦି(!data->share || !data->share->cookies) {
      /* throw away all existing cookies if this is not a shared cookie
         container */
      Curl_cookie_clearall(data->cookies);
      Curl_cookie_cleanup(data->cookies);
    }
    /* disable the cookie engine */
    data->cookies = ଖାଲି;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_PROXY
ସ୍ଥିର CURLcode setproxy(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *proxy)
{
  ଅଚଳ ଅକ୍ଷର *str = CURL_EASY_STR(data, STRING_PROXY);
  ଯଦି(str && proxy &&
     /* there was one set, is this a new one? */
     !strcmp(str, proxy))
    ଫେରନ୍ତୁ CURLE_OK; /* same one as before */

  changeproxy(data);
  ଫେରନ୍ତୁ Curl_setstropt(data, STRING_PROXY, proxy);
}

ସ୍ଥିର CURLcode setopt_cptr_proxy(ସଂରଚନା Curl_easy *data, CURLoption option,
                                  ଅକ୍ଷର *ptr)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା UserDefined *s = &data->set;
  ବାଛ(option) {
  ସ୍ଥିତି CURLOPT_PROXYUSERPWD: {
    /*
     * user:password needed to use the proxy
     */
    ଅକ୍ଷର *u = ଖାଲି;
    ଅକ୍ଷର *p = ଖାଲି;
    result = setstropt_userpwd(ptr, &u, &p);

    /* URL decode the components */
    ଯଦି(!result) {
      ଅକ୍ଷର *str = ଖାଲି;
      CURL_EASY_STR_CLEAR(data, STRING_PROXYUSERNAME);
      CURL_EASY_STR_CLEAR(data, STRING_PROXYPASSWORD);
      ଯଦି(u) {
        result = Curl_urldecode(u, 0, &str, ଖାଲି, REJECT_ZERO);
        ଯଦି(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYUSERNAME, str);
      }
      ଯଦି(!result && p) {
        str = ଖାଲି;
        result = Curl_urldecode(p, 0, &str, ଖାଲି, REJECT_ZERO);
        ଯଦି(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYPASSWORD, str);
      }
    }
    curlx_free(u);
    curlx_strzero(p);
    curlx_free(p);
    ଭାଙ୍ଗ;
  }
  ସ୍ଥିତି CURLOPT_PROXYUSERNAME:
    /*
     * authentication username to use in the operation
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_PROXYUSERNAME, ptr);

  ସ୍ଥିତି CURLOPT_PROXYPASSWORD:
    /*
     * authentication password to use in the operation
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_PROXYPASSWORD, ptr);

  ସ୍ଥିତି CURLOPT_NOPROXY:
    /*
     * proxy exception list
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_NOPROXY, ptr);
  ସ୍ଥିତି CURLOPT_PROXY_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use for proxy
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_CERT_PROXY, ptr);
  ସ୍ଥିତି CURLOPT_PROXY_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use for proxy
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_CERT_TYPE_PROXY, ptr);
  ସ୍ଥିତି CURLOPT_PROXY_SSLKEY:
    /*
     * String that holds filename of the SSL key to use for proxy
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_ଚାବି_PROXY, ptr);
  ସ୍ଥିତି CURLOPT_PROXY_KEYPASSWD:
    /*
     * String that holds the SSL private key password for proxy.
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_ଚାବି_PASSWD_PROXY, ptr);
  ସ୍ଥିତି CURLOPT_PROXY_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use for proxy
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_ଚାବି_TYPE_PROXY, ptr);
  ସ୍ଥିତି CURLOPT_PROXY_SSL_CIPHER_LIST:
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST)) {
      /* set a list of cipher we want to use in the SSL connection for proxy */
      ଫେରନ୍ତୁ Curl_setstropt(data, STRING_SSL_CIPHER_LIST_PROXY, ptr);
    }
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  ସ୍ଥିତି CURLOPT_PROXY_TLS13_CIPHERS:
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites for proxy */
      ଫେରନ୍ତୁ Curl_setstropt(data, STRING_SSL_CIPHER13_LIST_PROXY, ptr);
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  ସ୍ଥିତି CURLOPT_PROXY:
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
    ଫେରନ୍ତୁ setproxy(data, ptr);
  ସ୍ଥିତି CURLOPT_PRE_PROXY:
    /*
     * Set proxy server:port to use as SOCKS proxy.
     *
     * If the proxy is set to "" or NULL we explicitly say that we do not want
     * to use the socks proxy.
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_PRE_PROXY, ptr);
  ସ୍ଥିତି CURLOPT_SOCKS5_GSSAPI_SERVICE:
  ସ୍ଥିତି CURLOPT_PROXY_SERVICE_NAME:
    /*
     * Set proxy authentication service name for Kerberos 5 and SPNEGO
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_PROXY_SERVICE_NAME, ptr);
  ସ୍ଥିତି CURLOPT_PROXY_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
#ifdef USE_SSL
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      ଫେରନ୍ତୁ Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY_PROXY, ptr);
#endif
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;

  ସ୍ଥିତି CURLOPT_HAPROXY_CLIENT_IP:
    /*
     * Set the client IP to send through HAProxy PROXY protocol
     */
    result = Curl_setstropt(data, STRING_HAPROXY_CLIENT_IP, ptr);

    /* enable the HAProxy protocol if an IP is provided */
    s->haproxyprotocol = !!CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_PROXY_CAINFO:
    /*
     * Set CA info SSL connection for proxy. Specify filename of the
     * CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE_PROXY, ptr);
    ଯଦି(!result)
      s->proxy_ssl.custom_cafile = TRUE;
    ଫେରନ୍ତୁ result;
  ସ୍ଥିତି CURLOPT_PROXY_CRLFILE:
    /*
     * Set CRL file info for SSL connection for proxy. Specify filename of the
     * CRL to check certificates revocation
     */
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      ଫେରନ୍ତୁ Curl_setstropt(data, STRING_SSL_CRLFILE_PROXY, ptr);
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  ସ୍ଥିତି CURLOPT_PROXY_ISSUERCERT:
    /*
     * Set Issuer certificate file to check certificates issuer
     */
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      ଫେରନ୍ତୁ Curl_setstropt(data, STRING_SSL_ISSUERCERT_PROXY, ptr);
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  ସ୍ଥିତି CURLOPT_PROXY_CAPATH:
    /*
     * Set CA path info for SSL connection proxy. Specify directory name of the
     * CA certificates which have been prepared using openssl c_rehash utility.
     */
#ifdef USE_SSL
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH_PROXY, ptr);
      ଯଦି(!result)
        s->proxy_ssl.custom_capath = TRUE;
      ଫେରନ୍ତୁ result;
    }
#endif
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }
  ଫେରନ୍ତୁ result;
}
#endif

#ଯଦି !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
/*
 * A string with POST data. Makes curl HTTP POST. Even if it is NULL. If
 * needed, CURLOPT_POSTFIELDSIZE must have been set prior to
 * CURLOPT_COPYPOSTFIELDS and not altered later.
 */
ସ୍ଥିର CURLcode setopt_copypostfields(ଅଚଳ ଅକ୍ଷର *ptr, ସଂରଚନା UserDefined *s)
{
  CURLcode result = CURLE_OK;
  ଯଦି(s->postfieldsize < -1)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ଯଦି(!ptr || s->postfieldsize == -1) {
    ଯଦି(ptr && (strlen(ptr) > CURL_MAX_ନିବେଶ_LENGTH))
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_safefree(s->str_copypostfields);
    ଯଦି(ptr) {
      s->str_copypostfields = curlx_strdup(ptr);
      ଯଦି(!s->str_copypostfields)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
  }
  ଅନ୍ୟଥା {
    size_t pflen = curlx_sotouz_range(s->postfieldsize, 0, SIZE_MAX);
    ଯଦି(pflen == SIZE_MAX)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    ଅନ୍ୟଥା {
      /* Allocate even when size == 0. This satisfies the need of possible
         later address compare to detect the COPYPOSTFIELDS mode, and to mark
         that postfields is used rather than read function or form data. */
      ଅକ୍ଷର *p = curlx_memdup0(ptr, pflen);
      ଯଦି(!p)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      ଅନ୍ୟଥା {
        curlx_free(s->str_copypostfields);
        s->str_copypostfields = p;
      }
    }
  }

  s->postfields = s->str_copypostfields;
  s->method = HTTPREQ_POST;
  ଫେରନ୍ତୁ result;
}
#endif

#ifdef USE_ECH
ସ୍ଥିର CURLcode setopt_ech(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *ptr)
{
  ସଂରଚନା UserDefined *s = &data->set;
  CURLcode result = CURLE_OK;

  ଯଦି(!ptr || !strcmp(ptr, "false"))
    s->ssl.ech = CURLECH_DISABLE;
  ଅନ୍ୟଥା {
    size_t plen = strlen(ptr);
    ଯଦି(plen > CURL_MAX_ନିବେଶ_LENGTH)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଅନ୍ୟଥା {
      ଯଦି(!strcmp(ptr, "grease"))
        s->ssl.ech = CURLECH_GREASE;
      ଅନ୍ୟଥା ଯଦି(!strcmp(ptr, "true"))
        s->ssl.ech = CURLECH_ENABLE;
      ଅନ୍ୟଥା ଯଦି(!strcmp(ptr, "hard"))
        s->ssl.ech = CURLECH_HARD;
      ଅନ୍ୟଥା ଯଦି(plen > 4 && !strncmp(ptr, "ecl:", 4)) {
        ଯଦି(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_CONFIG, ptr + 4);
      }
      ଅନ୍ୟଥା ଯଦି(plen > 3 && !strncmp(ptr, "pn:", 3)) {
        ଯଦି(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_PUBLIC, ptr + 3);
      }
      ଅନ୍ୟଥା
        result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  ଫେରନ୍ତୁ result;
}
#ଅନ୍ୟଥା
#define setopt_ech(x, y) CURLE_NOT_BUILT_IN
#endif

#ଯଦି defined(USE_SSL) || defined(USE_SSH)
/* One of the options is used for both TLS and SSH */
ସ୍ଥିର CURLcode setopt_cptr_ssl(ସଂରଚନା Curl_easy *data, CURLoption option,
                                ଅକ୍ଷର *ptr)
{
  CURLcode result = CURLE_OK;

  ବାଛ(option) {
  ସ୍ଥିତି CURLOPT_KEYPASSWD:
    /*
     * String that holds the SSL or SSH private key password.
     */
    result = Curl_setstropt(data, STRING_ଚାବି_PASSWD, ptr);
    ଭାଙ୍ଗ;
#ifdef USE_SSL
  ସ୍ଥିତି CURLOPT_CAINFO:
    /*
     * Set CA info for SSL connection. Specify filename of the CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE, ptr);
    ଯଦି(!result)
      data->set.ssl.custom_cafile = TRUE;
    ଫେରନ୍ତୁ result;
  ସ୍ଥିତି CURLOPT_CAPATH:
    /*
     * Set CA path info for SSL connection. Specify directory name of the CA
     * certificates which have been prepared using openssl c_rehash utility.
     */
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH, ptr);
      ଯଦି(!result)
        data->set.ssl.custom_capath = TRUE;
      ଫେରନ୍ତୁ result;
    }
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  ସ୍ଥିତି CURLOPT_CRLFILE:
    /*
     * Set CRL file info for SSL connection. Specify filename of the CRL
     * to check certificates revocation
     */
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      ଫେରନ୍ତୁ Curl_setstropt(data, STRING_SSL_CRLFILE, ptr);
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  ସ୍ଥିତି CURLOPT_SSL_CIPHER_LIST:
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST))
      /* set a list of cipher we want to use in the SSL connection */
      ଫେରନ୍ତୁ Curl_setstropt(data, STRING_SSL_CIPHER_LIST, ptr);
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  ସ୍ଥିତି CURLOPT_TLS13_CIPHERS:
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites */
      ଫେରନ୍ତୁ Curl_setstropt(data, STRING_SSL_CIPHER13_LIST, ptr);
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  ସ୍ଥିତି CURLOPT_RANDOM_FILE:
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_EGDSOCKET:
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_SSL_CTX_DATA:
    /*
     * Set an SSL_CTX callback parameter pointer
     */
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      data->set.ssl_fsslctxp = ptr;
      ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  ସ୍ଥିତି CURLOPT_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_CERT, ptr);
  ସ୍ଥିତି CURLOPT_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_CERT_TYPE, ptr);
  ସ୍ଥିତି CURLOPT_SSLKEY:
    /*
     * String that holds filename of the SSL key to use
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_ଚାବି, ptr);
  ସ୍ଥିତି CURLOPT_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_ଚାବି_TYPE, ptr);
  ସ୍ଥିତି CURLOPT_SSLENGINE:
    /*
     * String that holds the SSL crypto engine.
     */
    ଯଦି(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
    result = Curl_setstropt(data, STRING_SSL_ENGINE, ptr);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_ISSUERCERT:
    /*
     * Set Issuer certificate file
     * to check certificates issuer
     */
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      ଫେରନ୍ତୁ Curl_setstropt(data, STRING_SSL_ISSUERCERT, ptr);
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  ସ୍ଥିତି CURLOPT_SSL_EC_CURVES:
    /*
     * Set accepted curves in SSL connection setup.
     * Specify colon-delimited list of curve algorithm names.
     */
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_SSL_EC_CURVES))
      ଫେରନ୍ତୁ Curl_setstropt(data, STRING_SSL_EC_CURVES, ptr);
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  ସ୍ଥିତି CURLOPT_SSL_SIGNATURE_ALGORITHMS:
    /*
     * Set accepted signature algorithms.
     * Specify colon-delimited list of signature scheme names.
     */
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_SIGNATURE_ALGORITHMS))
      ଫେରନ୍ତୁ Curl_setstropt(data, STRING_SSL_SIGNATURE_ALGORITHMS, ptr);
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  ସ୍ଥିତି CURLOPT_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      ଫେରନ୍ତୁ Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY, ptr);
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  ସ୍ଥିତି CURLOPT_ECH:
    ଫେରନ୍ତୁ setopt_ech(data, ptr);
#endif
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }
  ଫେରନ୍ତୁ result;
}
#endif

#ଯଦି !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
ସ୍ଥିର CURLcode setopt_cptr_http_mqtt(ସଂରଚନା Curl_easy *data,
                                      CURLoption option, ଅକ୍ଷର *ptr)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା UserDefined *s = &data->set;

  ବାଛ(option) {
  ସ୍ଥିତି CURLOPT_COPYPOSTFIELDS:
    ଫେରନ୍ତୁ setopt_copypostfields(ptr, s);

  ସ୍ଥିତି CURLOPT_POSTFIELDS:
    /*
     * Like above, but use static data instead of copying it.
     */
    s->postfields = ptr;
    /* Release old copied data. */
    curlx_safefree(s->str_copypostfields);
    s->method = HTTPREQ_POST;
    ଭାଙ୍ଗ;

#ifndef CURL_DISABLE_HTTP
  ସ୍ଥିତି CURLOPT_TRAILERDATA:
    s->trailer_data = ptr;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_ACCEPT_ENCODING:
    /*
     * String to use at the value of Accept-Encoding header.
     *
     * If the encoding is set to "" we use an Accept-Encoding header that
     * encompasses all the encodings we support.
     * If the encoding is set to NULL we do not send an Accept-Encoding header
     * and ignore any received Content-Encoding header.
     *
     */
    ଯଦି(ptr && !*ptr) {
      ptr = Curl_get_content_encodings();
      ଯଦି(ptr)
        result = CURL_EASY_STR_SETN(data, STRING_ENCODING, ptr);
      ଅନ୍ୟଥା
        result = CURLE_OUT_OF_MEMORY;
      ଫେରନ୍ତୁ result;
    }
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_ENCODING, ptr);

#ifndef CURL_DISABLE_AWS
  ସ୍ଥିତି CURLOPT_AWS_SIGV4:
    /*
     * String that is merged to some authentication
     * parameters are used by the algorithm.
     */
    result = Curl_setstropt(data, STRING_AWS_SIGV4, ptr);
    /*
     * Basic has been set by default; it needs to be unset here.
     */
    ଯଦି(CURL_EASY_STR(data, STRING_AWS_SIGV4))
      s->httpauth = CURLAUTH_AWS_SIGV4;
    ଅନ୍ୟଥା
      s->httpauth &= ~(uint32_t)CURLAUTH_AWS_SIGV4;
    ଭାଙ୍ଗ;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  ସ୍ଥିତି CURLOPT_HTTPSIG_ଚାବି:
    result = Curl_setstropt(data, STRING_HTTPSIG_ଚାବି, ptr);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_HTTPSIG_KEYID:
    result = Curl_setstropt(data, STRING_HTTPSIG_KEYID, ptr);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_HTTPSIG_HEADERS:
    result = Curl_setstropt(data, STRING_HTTPSIG_HEADERS, ptr);
    ଭାଙ୍ଗ;
#endif
  ସ୍ଥିତି CURLOPT_REFERER: {
    /*
     * String to set in the HTTP Referer: field.
     */
    ସଂରଚନା bufref *oldref = &data->state.referer;
    /* free the old after the storing the new in case the input is actually
       pointing back to this */
    result = Curl_setstropt(data, STRING_SET_REFERER, ptr);
    Curl_bufref_free(oldref);
    ଭାଙ୍ଗ;
  }

  ସ୍ଥିତି CURLOPT_USERAGENT:
    /*
     * String to use in the HTTP User-Agent field
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_USERAGENT, ptr);

#ifndef CURL_DISABLE_COOKIES
  ସ୍ଥିତି CURLOPT_COOKIE:
    /*
     * Cookie string to send to the remote server in the request.
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_COOKIE, ptr);

  ସ୍ଥିତି CURLOPT_COOKIEFILE:
    ଫେରନ୍ତୁ cookiefile(data, ptr);

  ସ୍ଥିତି CURLOPT_COOKIEJAR:
    /*
     * Set cookie filename to dump all cookies to when we are done.
     */
    result = Curl_setstropt(data, STRING_COOKIEJAR, ptr);
    ଯଦି(!result) {
      /*
       * Activate the cookie parser. This may or may not already
       * have been made.
       */
      ଯଦି(!data->cookies)
        data->cookies = Curl_cookie_init();
      ଯଦି(!data->cookies)
        result = CURLE_OUT_OF_MEMORY;
      ଅନ୍ୟଥା
        data->state.cookie_engine = TRUE;
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURLOPT_COOKIELIST:
    ଫେରନ୍ତୁ cookielist(data, ptr);
#endif /* !CURL_DISABLE_COOKIES */

#endif /* !CURL_DISABLE_HTTP */
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }
  ଫେରନ୍ତୁ result;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_MQTT */

#ifdef USE_SSH
ସ୍ଥିର CURLcode setopt_cptr_ssh(ସଂରଚନା Curl_easy *data, CURLoption option,
                                ଅକ୍ଷର *ptr)
{
  ସଂରଚନା UserDefined *s = &data->set;
  ବାଛ(option) {
  ସ୍ଥିତି CURLOPT_SSH_PUBLIC_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa.pub file
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_SSH_PUBLIC_ଚାବି, ptr);
  ସ୍ଥିତି CURLOPT_SSH_PRIVATE_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa file
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_SSH_PRIVATE_ଚାବି, ptr);
  ସ୍ଥିତି CURLOPT_SSH_KEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_keyfunc_userp = ptr;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_SSH_HOST_PUBLIC_ଚାବି_MD5:
    /*
     * Option to allow for the MD5 of the host public key to be checked
     * for validation purposes.
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_ଚାବି_MD5, ptr);
  ସ୍ଥିତି CURLOPT_SSH_HOST_PUBLIC_ଚାବି_SHA256:
    /*
     * Option to allow for the SHA256 of the host public key to be checked
     * for validation purposes.
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_ଚାବି_SHA256, ptr);
  ସ୍ଥିତି CURLOPT_SSH_KNOWNHOSTS:
    /*
     * Store the filename to read known hosts from.
     */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_SSH_KNOWNHOSTS, ptr);
#ifdef USE_LIBSSH2
  ସ୍ଥିତି CURLOPT_SSH_HOSTKEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_hostkeyfunc_userp = ptr;
    ଭାଙ୍ଗ;
#endif /* USE_LIBSSH2 */
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}
#endif /* USE_SSH */

#ifndef CURL_DISABLE_FTP
ସ୍ଥିର CURLcode setopt_cptr_ftp(ସଂରଚନା Curl_easy *data, CURLoption option,
                                ଅକ୍ଷର *ptr)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା UserDefined *s = &data->set;
  ବାଛ(option) {
  ସ୍ଥିତି CURLOPT_FTPPORT:
    /*
     * Use FTP PORT, this also specifies which IP address to use
     */
    result = Curl_setstropt(data, STRING_FTPPORT, ptr);
    s->ftp_use_port = !!CURL_EASY_STR(data, STRING_FTPPORT);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURLOPT_FTP_ACCOUNT:
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_FTP_ACCOUNT, ptr);

  ସ୍ଥିତି CURLOPT_FTP_ALTERNATIVE_TO_USER:
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_FTP_ALTERNATIVE_TO_USER, ptr);

  ସ୍ଥିତି CURLOPT_KRBLEVEL:
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN; /* removed in 8.17.0 */
  ସ୍ଥିତି CURLOPT_CHUNK_DATA:
    s->wildcardptr = ptr;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_FNMATCH_DATA:
    s->fnmatch_data = ptr;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }
  ଫେରନ୍ତୁ result;
}
#endif /* !CURL_DISABLE_FTP */

ସ୍ଥିର CURLcode setopt_cptr_net(ସଂରଚନା Curl_easy *data, CURLoption option,
                                ଅକ୍ଷର *ptr)
{
  ବାଛ(option) {
  ସ୍ଥିତି CURLOPT_INTERFACE:
    /*
     * Set what interface or address/hostname to bind the socket to when
     * performing an operation and thus what from-IP your connection will use.
     */
    ଫେରନ୍ତୁ setstropt_interface(data, ptr);

#ifdef USE_RESOLV_ARES
  ସ୍ଥିତି CURLOPT_DNS_SERVERS:
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_DNS_SERVERS, ptr);

  ସ୍ଥିତି CURLOPT_DNS_INTERFACE:
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_DNS_INTERFACE, ptr);

  ସ୍ଥିତି CURLOPT_DNS_LOCAL_IP4:
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_DNS_LOCAL_IP4, ptr);

  ସ୍ଥିତି CURLOPT_DNS_LOCAL_IP6:
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_DNS_LOCAL_IP6, ptr);
#endif
#ifdef USE_UNIX_SOCKETS
  ସ୍ଥିତି CURLOPT_UNIX_SOCKET_PATH:
    data->set.abstract_unix_socket = FALSE;
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);

  ସ୍ଥିତି CURLOPT_ABSTRACT_UNIX_SOCKET:
    data->set.abstract_unix_socket = TRUE;
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);
#endif
#ifndef CURL_DISABLE_DOH
  ସ୍ଥିତି CURLOPT_DOH_URL:
    {
      CURLcode result = Curl_setstropt(data, STRING_DOH, ptr);
      data->set.doh = !!CURL_EASY_STR(data, STRING_DOH);
      ଫେରନ୍ତୁ result;
    }
#endif
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }
}

ସ୍ଥିର CURLcode setopt_cptr_misc(ସଂରଚନା Curl_easy *data, CURLoption option,
                                 ଅକ୍ଷର *ptr)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା UserDefined *s = &data->set;

  ବାଛ(option) {
  ସ୍ଥିତି CURLOPT_REQUEST_TARGET:
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_TARGET, ptr);
#ifndef CURL_DISABLE_NETRC
  ସ୍ଥିତି CURLOPT_NETRC_FILE:
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_NETRC_FILE, ptr);
#endif
  ସ୍ଥିତି CURLOPT_CUSTOMREQUEST:
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_CUSTOMREQUEST, ptr);

    /* we do not set s->method = HTTPREQ_CUSTOM; here, we continue as if we
       were using the already set type and this changes the actual request
       keyword */
  ସ୍ଥିତି CURLOPT_SERVICE_NAME:
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_SERVICE_NAME, ptr);

  ସ୍ଥିତି CURLOPT_HEADERDATA:
    s->writeheader = ptr;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_READDATA:
    s->in_set = ptr;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_WRITEDATA:
    s->out = ptr;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_DEBUGDATA:
    s->debugdata = ptr;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_PROGRESSDATA:
    s->progress_client = ptr;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_SEEKDATA:
    s->seek_client = ptr;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_IOCTLDATA:
    s->ioctl_client = ptr;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_SOCKOPTDATA:
    s->sockopt_client = ptr;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_OPENSOCKETDATA:
    s->opensocket_client = ptr;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_RESOLVER_START_DATA:
    s->resolver_start_client = ptr;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_CLOSESOCKETDATA:
    s->closesocket_client = ptr;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_PREREQDATA:
    s->prereq_userp = ptr;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_ERRORBUFFER:
    s->errorbuffer = ptr;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_URL:
    result = Curl_setstropt(data, STRING_SET_URL, ptr);
    Curl_bufref_set(&data->state.url,
                    CURL_EASY_STR(data, STRING_SET_URL), 0, ଖାଲି);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURLOPT_USERPWD: {
    ଅକ୍ଷର *u = ଖାଲି, *p = ଖାଲି;
    result = setstropt_userpwd(ptr, &u, &p);
    ଯଦି(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_USERNAME, u);
      u = ଖାଲି;
    }
    ଯଦି(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_PASSWORD, p);
      p = ଖାଲି;
    }
    curlx_free(u);
    curlx_free(p);
    ଫେରନ୍ତୁ result;
  }

  ସ୍ଥିତି CURLOPT_USERNAME:
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_USERNAME, ptr);

  ସ୍ଥିତି CURLOPT_PASSWORD:
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_PASSWORD, ptr);

  ସ୍ଥିତି CURLOPT_LOGIN_OPTIONS:
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_OPTIONS, ptr);

  ସ୍ଥିତି CURLOPT_XOAUTH2_BEARER:
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_BEARER, ptr);
  ସ୍ଥିତି CURLOPT_RANGE:
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_SET_RANGE, ptr);
  ସ୍ଥିତି CURLOPT_PRIVATE:
    s->private_data = ptr;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_PROTOCOLS_STR:
    ଯଦି(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      ଯଦି(!result)
        s->allowed_protocols = protos;
    }
    ଅନ୍ୟଥା
      /* make a NULL argument reset to default */
      s->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_REDIR_PROTOCOLS_STR:
    ଯଦି(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      ଯଦି(!result)
        s->redir_protocols = protos;
    }
    ଅନ୍ୟଥା
      /* make a NULL argument reset to default */
      s->redir_protocols = (curl_prot_t)CURLPROTO_REDIR;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_DEFAULT_PROTOCOL:
    /* Set the protocol to use when the URL does not include any protocol */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_DEFAULT_PROTOCOL, ptr);
#ifndef CURL_DISABLE_SMTP
  ସ୍ଥିତି CURLOPT_MAIL_FROM:
    /* Set the SMTP mail originator */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_MAIL_FROM, ptr);
  ସ୍ଥିତି CURLOPT_MAIL_AUTH:
    /* Set the SMTP auth originator */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_MAIL_AUTH, ptr);
#endif
  ସ୍ଥିତି CURLOPT_SASL_AUTHZID:
    /* Authorization identity (identity to act as) */
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_SASL_AUTHZID, ptr);
#ifndef CURL_DISABLE_RTSP
  ସ୍ଥିତି CURLOPT_RTSP_SESSION_ID:
    Curl_peer_unlink(&data->state.rtsp_session_origin);
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_RTSP_SESSION_ID, ptr);
  ସ୍ଥିତି CURLOPT_RTSP_STREAM_URI:
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_RTSP_STREAM_URI, ptr);
  ସ୍ଥିତି CURLOPT_RTSP_TRANSPORT:
    ଫେରନ୍ତୁ Curl_setstropt(data, STRING_RTSP_TRANSPORT, ptr);
  ସ୍ଥିତି CURLOPT_INTERLEAVEDATA:
    s->rtp_out = ptr;
    ଭାଙ୍ଗ;
#endif /* !CURL_DISABLE_RTSP */
  ସ୍ଥିତି CURLOPT_TLSAUTH_USERNAME:
  ସ୍ଥିତି CURLOPT_TLSAUTH_PASSWORD:
  ସ୍ଥିତି CURLOPT_TLSAUTH_TYPE:
  ସ୍ଥିତି CURLOPT_PROXY_TLSAUTH_USERNAME:
  ସ୍ଥିତି CURLOPT_PROXY_TLSAUTH_PASSWORD:
  ସ୍ଥିତି CURLOPT_PROXY_TLSAUTH_TYPE:
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#ifndef CURL_DISABLE_HSTS
  ସ୍ଥିତି CURLOPT_HSTSREADDATA:
    s->hsts_read_userp = ptr;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_HSTSWRITEDATA:
    s->hsts_write_userp = ptr;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_HSTS: {
    ସଂରଚନା curl_slist *h;
    ଯଦି(!data->hsts) {
      data->hsts = Curl_hsts_init();
      ଯଦି(!data->hsts)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
    ଯଦି(ptr) {
      result = Curl_setstropt(data, STRING_HSTS, ptr);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      /* this needs to build a list of filenames to read from, so that it can
         read them later, as we might get a shared HSTS handle to load them
         into */
      h = curl_slist_append(data->state.hstslist, ptr);
      ଯଦି(!h) {
        curl_slist_free_all(data->state.hstslist);
        data->state.hstslist = ଖାଲି;
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      }
      data->state.hstslist = h; /* store the list for later use */
    }
    ଅନ୍ୟଥା {
      /* clear the list of HSTS files */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = ଖାଲି;
      ଯଦି(!data->share || !data->share->hsts)
        /* throw away the HSTS cache unless shared */
        Curl_hsts_cleanup(&data->hsts);
    }
    ଭାଙ୍ଗ;
  }
#endif /* !CURL_DISABLE_HSTS */
#ifndef CURL_DISABLE_ALTSVC
  ସ୍ଥିତି CURLOPT_ALTSVC:
    ଯଦି(!data->asi) {
      data->asi = Curl_altsvc_init();
      ଯଦି(!data->asi)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
    result = Curl_setstropt(data, STRING_ALTSVC, ptr);
    ଯଦି(result)
      ଭାଙ୍ଗ;
    ଯଦି(ptr)
      ଫେରନ୍ତୁ Curl_altsvc_load(data->asi, ptr);
    ଭାଙ୍ଗ;
#endif /* !CURL_DISABLE_ALTSVC */
  ସ୍ଥିତି CURLOPT_ECH:
    ଫେରନ୍ତୁ setopt_ech(data, ptr);
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode setopt_cptr(ସଂରଚନା Curl_easy *data, CURLoption option,
                            ଅକ୍ଷର *ptr)
{
  ପ୍ରକାରବ୍ୟାଖ୍ୟା CURLcode (*ptrfunc)(ସଂରଚନା Curl_easy *data, CURLoption option,
                              ଅକ୍ଷର *ptr);
  /* Order by likeliness */
  ସ୍ଥିର ଅଚଳ ptrfunc setopt_call[] = {
    setopt_cptr_misc,
#ଯଦି defined(USE_SSL) || defined(USE_SSH)
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
#ଯଦି !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
    setopt_cptr_http_mqtt,
#endif
  };
  size_t i;

  ପାଇଁ(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, ptr);
    ଯଦି(result != CURLE_UNKNOWN_OPTION)
      ଫେରନ୍ତୁ result;
  }
  ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
}

ସ୍ଥିର CURLcode setopt_func(ସଂରଚନା Curl_easy *data, CURLoption option,
                            va_list param)
{
  ସଂରଚନା UserDefined *s = &data->set;
  ବାଛ(option) {
  ସ୍ଥିତି CURLOPT_PROGRESSFUNCTION:
    /*
     * Progress callback function
     */
    s->fprogress = va_arg(param, curl_progress_callback);
    ଯଦି(s->fprogress)
      data->progress.callback = TRUE; /* no longer internal */
    ଅନ୍ୟଥା
      data->progress.callback = FALSE; /* NULL enforces internal */
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURLOPT_XFERINFOFUNCTION:
    /*
     * Transfer info callback function
     */
    s->fxferinfo = va_arg(param, curl_xferinfo_callback);
    ଯଦି(s->fxferinfo)
      data->progress.callback = TRUE; /* no longer internal */
    ଅନ୍ୟଥା
      data->progress.callback = FALSE; /* NULL enforces internal */

    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_DEBUGFUNCTION:
    /*
     * stderr write callback.
     */
    s->fdebug = va_arg(param, curl_debug_callback);
    /*
     * if the callback provided is NULL, it will use the default callback
     */
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_HEADERFUNCTION:
    /*
     * Set header write callback
     */
    s->fwrite_header = va_arg(param, curl_write_callback);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_WRITEFUNCTION:
    /*
     * Set data write callback
     */
    s->fwrite_func = va_arg(param, curl_write_callback);
    ଯଦି(!s->fwrite_func)
#ଯଦି defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ଠେଲ
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fwrite_func = (curl_write_callback)fwrite;
#ଯଦି defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic କାଢ଼
#endif
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_READFUNCTION:
    /*
     * Read data callback
     */
    s->fread_func_set = va_arg(param, curl_read_callback);
    ଯଦି(!s->fread_func_set) {
      s->is_fread_set = 0;
#ଯଦି defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ଠେଲ
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fread_func_set = (curl_read_callback)fread;
#ଯଦି defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic କାଢ଼
#endif
    }
    ଅନ୍ୟଥା
      s->is_fread_set = 1;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_SEEKFUNCTION:
    /*
     * Seek callback. Might be NULL.
     */
    s->seek_func = va_arg(param, curl_seek_callback);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_IOCTLFUNCTION:
    /*
     * I/O control callback. Might be NULL.
     */
    s->ioctl_func = va_arg(param, curl_ioctl_callback);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_SSL_CTX_FUNCTION:
    /*
     * Set an SSL_CTX callback
     */
#ifdef USE_SSL
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      s->ssl_fsslctx = va_arg(param, curl_ssl_ctx_callback);
      ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା
#endif
      ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;

  ସ୍ଥିତି CURLOPT_SOCKOPTFUNCTION:
    /*
     * socket callback function: called after socket() but before connect()
     */
    s->fsockopt = va_arg(param, curl_sockopt_callback);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURLOPT_OPENSOCKETFUNCTION:
    /*
     * open/create socket callback function: called instead of socket(),
     * before connect()
     */
    s->fopensocket = va_arg(param, curl_opensocket_callback);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURLOPT_CLOSESOCKETFUNCTION:
    /*
     * close socket callback function: called instead of close()
     * when shutting down a connection
     */
    s->fclosesocket = va_arg(param, curl_closesocket_callback);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURLOPT_RESOLVER_START_FUNCTION:
    /*
     * resolver start callback function: called before a new resolver request
     * is started
     */
    s->resolver_start = va_arg(param, curl_resolver_start_callback);
    ଭାଙ୍ଗ;

#ifdef USE_SSH
#ifdef USE_LIBSSH2
  ସ୍ଥିତି CURLOPT_SSH_HOSTKEYFUNCTION:
    /* the callback to check the hostkey without the knownhost file */
    s->ssh_hostkeyfunc = va_arg(param, curl_sshhostkeycallback);
    ଭାଙ୍ଗ;
#endif

  ସ୍ଥିତି CURLOPT_SSH_KEYFUNCTION:
    /* setting to NULL is fine since the ssh.c functions themselves will
       then revert to use the internal default */
    s->ssh_keyfunc = va_arg(param, curl_sshkeycallback);
    ଭାଙ୍ଗ;

#endif /* USE_SSH */

#ifndef CURL_DISABLE_RTSP
  ସ୍ଥିତି CURLOPT_INTERLEAVEFUNCTION:
    /* Set the user defined RTP write function */
    s->fwrite_rtp = va_arg(param, curl_write_callback);
    ଭାଙ୍ଗ;
#endif
#ifndef CURL_DISABLE_FTP
  ସ୍ଥିତି CURLOPT_CHUNK_BGN_FUNCTION:
    s->chunk_bgn = va_arg(param, curl_chunk_bgn_callback);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_CHUNK_END_FUNCTION:
    s->chunk_end = va_arg(param, curl_chunk_end_callback);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_FNMATCH_FUNCTION:
    s->fnmatch = va_arg(param, curl_fnmatch_callback);
    ଭାଙ୍ଗ;
#endif
#ifndef CURL_DISABLE_HTTP
  ସ୍ଥିତି CURLOPT_TRAILERFUNCTION:
    s->trailer_callback = va_arg(param, curl_trailer_callback);
    ଭାଙ୍ଗ;
#endif
#ifndef CURL_DISABLE_HSTS
  ସ୍ଥିତି CURLOPT_HSTSREADFUNCTION:
    s->hsts_read = va_arg(param, curl_hstsread_callback);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_HSTSWRITEFUNCTION:
    s->hsts_write = va_arg(param, curl_hstswrite_callback);
    ଭାଙ୍ଗ;
#endif
  ସ୍ଥିତି CURLOPT_PREREQFUNCTION:
    s->fprereq = va_arg(param, curl_prereq_callback);
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode setopt_offt(ସଂରଚନା Curl_easy *data, CURLoption option,
                            curl_off_t offt)
{
  ସଂରଚନା UserDefined *s = &data->set;
  ବାଛ(option) {
  ସ୍ଥିତି CURLOPT_TIMEVALUE_LARGE:
    /*
     * This is the value to compare with the remote document with the
     * method set with CURLOPT_TIMECONDITION
     */
    s->timevalue = (time_t)offt;
    ଭାଙ୍ଗ;

    /* MQTT "borrows" some of the HTTP options */
  ସ୍ଥିତି CURLOPT_POSTFIELDSIZE_LARGE:
    /*
     * The size of the POSTFIELD data to prevent libcurl to do strlen() to
     * figure it out. Enables binary posts.
     */
    ଯଦି(offt < -1)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

    ଯଦି(s->postfieldsize < offt && s->str_copypostfields) {
      /* Previous CURLOPT_COPYPOSTFIELDS is no longer valid. */
      curlx_safefree(s->str_copypostfields);
      s->postfields = ଖାଲି;
    }
    s->postfieldsize = offt;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_INFILESIZE_LARGE:
    /*
     * If known, this should inform curl about the file size of the
     * to-be-uploaded file.
     */
    ଯଦି(offt < -1)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = offt;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_MAX_SEND_SPEED_LARGE:
    /*
     * When transfer uploads are faster than CURLOPT_MAX_SEND_SPEED_LARGE
     * bytes per second the transfer is throttled..
     */
    ଯଦି(offt < 0)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_send_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.ul.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_MAX_RECV_SPEED_LARGE:
    /*
     * When receiving data faster than CURLOPT_MAX_RECV_SPEED_LARGE bytes per
     * second the transfer is throttled..
     */
    ଯଦି(offt < 0)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_recv_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.dl.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_RESUME_FROM_LARGE:
    /*
     * Resume transfer at the given file position
     */
    ଯଦି(offt < -1)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = offt;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLOPT_MAXFILESIZE_LARGE:
    /*
     * Set the maximum size of a file to download.
     */
    ଯଦି(offt < 0)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_filesize = offt;
    ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode setopt_blob(ସଂରଚନା Curl_easy *data, CURLoption option,
                            ସଂରଚନା curl_blob *blob)
{
  ସଂରଚନା UserDefined *s = &data->set;
  ବାଛ(option) {
  ସ୍ଥିତି CURLOPT_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use
     */
    ଫେରନ୍ତୁ Curl_setblobopt(&s->blobs[BLOB_CERT], blob);
#ifndef CURL_DISABLE_PROXY
  ସ୍ଥିତି CURLOPT_PROXY_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use for proxy
     */
    ଫେରନ୍ତୁ Curl_setblobopt(&s->blobs[BLOB_CERT_PROXY], blob);
  ସ୍ଥିତି CURLOPT_PROXY_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use for proxy
     */
    ଫେରନ୍ତୁ Curl_setblobopt(&s->blobs[BLOB_ଚାବି_PROXY], blob);
  ସ୍ଥିତି CURLOPT_PROXY_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection proxy.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO_PROXY], blob);
      ଯଦି(!result)
        s->proxy_ssl.custom_cablob = TRUE;
      ଫେରନ୍ତୁ result;
    }
#endif
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  ସ୍ଥିତି CURLOPT_PROXY_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      ଫେରନ୍ତୁ Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT_PROXY], blob);
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif
  ସ୍ଥିତି CURLOPT_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use
     */
    ଫେରନ୍ତୁ Curl_setblobopt(&s->blobs[BLOB_ଚାବି], blob);
  ସ୍ଥିତି CURLOPT_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO], blob);
      ଯଦି(!result)
        s->ssl.custom_cablob = TRUE;
      ଫେରନ୍ତୁ result;
    }
#endif
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  ସ୍ଥିତି CURLOPT_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    ଯଦି(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      ଫେରନ୍ତୁ Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT], blob);
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }
  /* unreachable */
}

/*
 * Do not make Curl_vsetopt() static: it is called from
 * projects/OS400/ccsidcurl.c.
 */
CURLcode Curl_vsetopt(ସଂରଚନା Curl_easy *data, CURLoption option, va_list param)
{
  ଯଦି(option < CURLOPTTYPE_OBJECTPOINT)
    ଫେରନ୍ତୁ setopt_long(data, option, va_arg(param, long));
  ଅନ୍ୟଥା ଯଦି(option < CURLOPTTYPE_FUNCTIONPOINT) {
    /* unfortunately, different pointer types cannot be identified any other
       way than being listed explicitly */
    ବାଛ(option) {
    ସ୍ଥିତି CURLOPT_HTTPHEADER:
    ସ୍ଥିତି CURLOPT_QUOTE:
    ସ୍ଥିତି CURLOPT_POSTQUOTE:
    ସ୍ଥିତି CURLOPT_TELNETOPTIONS:
    ସ୍ଥିତି CURLOPT_PREQUOTE:
    ସ୍ଥିତି CURLOPT_HTTP200ALIASES:
    ସ୍ଥିତି CURLOPT_MAIL_RCPT:
    ସ୍ଥିତି CURLOPT_RESOLVE:
    ସ୍ଥିତି CURLOPT_PROXYHEADER:
    ସ୍ଥିତି CURLOPT_CONNECT_TO:
      ଫେରନ୍ତୁ setopt_slist(data, option, va_arg(param, ସଂରଚନା curl_slist *));
    ସ୍ଥିତି CURLOPT_HTTPPOST:         /* curl_httppost * */
    ସ୍ଥିତି CURLOPT_MIMEPOST:         /* curl_mime * */
    ସ୍ଥିତି CURLOPT_STDERR:           /* FILE * */
    ସ୍ଥିତି CURLOPT_SHARE:            /* CURLSH * */
    ସ୍ଥିତି CURLOPT_CURLU:            /* CURLU * */
      ଫେରନ୍ତୁ setopt_pointers(data, option, param);
    ସ୍ଥିତି CURLOPT_STREAM_DEPENDS:   /* CURL * */
    ସ୍ଥିତି CURLOPT_STREAM_DEPENDS_E: /* CURL * */
      ଫେରନ୍ତୁ CURLE_OK;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଭାଙ୍ଗ;
    }
    /* the char pointer options */
    ଫେରନ୍ତୁ setopt_cptr(data, option, va_arg(param, ଅକ୍ଷର *));
  }
  ଅନ୍ୟଥା ଯଦି(option < CURLOPTTYPE_OFF_T)
    ଫେରନ୍ତୁ setopt_func(data, option, param);
  ଅନ୍ୟଥା ଯଦି(option < CURLOPTTYPE_BLOB)
    ଫେରନ୍ତୁ setopt_offt(data, option, va_arg(param, curl_off_t));
  ଫେରନ୍ତୁ setopt_blob(data, option, va_arg(param, ସଂରଚନା curl_blob *));
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
  ସଂରଚନା Curl_eapi_guard guard;
  CURLcode result;

  ଯଦି(CURL_EAPI_ENTER(&guard, curl, easy_setopt, &result)) {
    ସଂରଚନା Curl_easy *data = curl;
    va_list arg;

    va_start(arg, option);

    result = Curl_vsetopt(data, option, arg);

    va_end(arg);
    ଯଦି(result == CURLE_BAD_FUNCTION_ARGUMENT)
      failf(data, "setopt 0x%x got bad argument", (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)option);
  }
  CURL_EAPI_LEAVE(&guard);
  ଫେରନ୍ତୁ result;
}
