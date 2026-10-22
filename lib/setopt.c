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
#include "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#include <linux/tcp.h>
#elif defined(HAVE_NETINET_TCP_H)
#include <netinet/tcp.h>
#endif

#include "urldata.h"
#include "url.h"
#include "progress.h"
#include "content_encoding.h"
#include "strcase.h"
#include "curl_share.h"
#include "vtls/vtls.h"
#include "curl_trc.h"
#include "setopt.h"
#include "altsvc.h"
#include "hsts.h"
#include "tftp.h"
#include "curlx/strdup.h"
#include "escape.h"
#include "bufref.h"
#include "vauth/vauth.h"

static CURLcode setopt_set_timeout_sec(timediff_t *ptimeout_ms, long secs)
{
  ако(secs < 0)
    врати CURLE_BAD_FUNCTION_ARGUMENT;
#ако LONG_MAX > (TIMEDIFF_T_MAX / 1000)
  ако(secs > (TIMEDIFF_T_MAX / 1000)) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    врати CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)secs * 1000;
  врати CURLE_OK;
}

static CURLcode setopt_set_timeout_ms(timediff_t *ptimeout_ms, long ms)
{
  ако(ms < 0)
    врати CURLE_BAD_FUNCTION_ARGUMENT;
#ако LONG_MAX > TIMEDIFF_T_MAX
  ако(ms > TIMEDIFF_T_MAX) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    врати CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)ms;
  врати CURLE_OK;
}

CURLcode Curl_setstropt(struct Curl_easy *data,
                        enum dupstring id, const char *s)
{
  size_t slen = s ? strlen(s) : 0;
  DEBUGASSERT((unsigned)id <= UINT8_MAX);
  ако(s && (slen > CURL_MAX_INPUT_LENGTH))
    врати CURLE_BAD_FUNCTION_ARGUMENT;

  врати CURL_EASY_STR_SET(data, (uint8_t)id, s, slen);
}

CURLcode Curl_setblobopt(struct curl_blob **blobp,
                         const struct curl_blob *blob)
{
  /* free the previous storage at `blobp' and replace by a dynamic storage
     copy of blob. If CURL_BLOB_COPY is set, the data is copied. */

  curlx_safefree(*blobp);

  ако(blob) {
    struct curl_blob *nblob;
    ако(!blob->data || !blob->len || (blob->len > CURL_MAX_INPUT_LENGTH))
      врати CURLE_BAD_FUNCTION_ARGUMENT;
    nblob = (struct curl_blob *)
      curlx_malloc(sizeof(struct curl_blob) +
                   ((blob->flags & CURL_BLOB_COPY) ? blob->len : 0));
    ако(!nblob)
      врати CURLE_OUT_OF_MEMORY;
    *nblob = *blob;
    ако(blob->flags & CURL_BLOB_COPY) {
      /* put the data after the blob struct in memory */
      nblob->data = (char *)nblob + sizeof(struct curl_blob);
      memcpy(nblob->data, blob->data, blob->len);
    }

    *blobp = nblob;
    врати CURLE_OK;
  }

  врати CURLE_OK;
}

static CURLcode setstropt_userpwd(const char *option, char **userp,
                                  char **passwdp)
{
  char *user = NULL;
  char *passwd = NULL;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Parse the login details if specified. If not, then we treat NULL as a
     hint to clear the existing data */
  ако(option) {
    size_t len = strlen(option);
    CURLcode result;
    ако(len > CURL_MAX_INPUT_LENGTH)
      врати CURLE_BAD_FUNCTION_ARGUMENT;

    result = Curl_parse_login_details(option, len, &user, &passwd, NULL);
    ако(result)
      врати result;
  }

  curlx_free(*userp);
  *userp = user;

  curlx_strzero(*passwdp);
  curlx_free(*passwdp);
  *passwdp = passwd;

  врати CURLE_OK;
}

static CURLcode setstropt_interface(struct Curl_easy *data, char *option)
{
  ако(option && !option[0])
    option = NULL;
  ако(option) {
    /* Parse for checking syntax */
    CURLcode result = Curl_parse_interface(option, NULL, NULL, NULL);
    ако(result)
      врати result;
    врати CURL_EASY_STR_SET(data, STRING_INTERFACE_OPT,
                             option, strlen(option));
  }
  CURL_EASY_STR_CLEAR(data, STRING_INTERFACE_OPT);
  врати CURLE_OK;
}

#ifdef USE_SSL
#define C_SSLVERSION_VALUE(x)     ((x) & 0xffff)
#define C_SSLVERSION_MAX_VALUE(x) ((unsigned long)(x) & 0xffff0000)
#endif

static CURLcode protocol2num(const char *str, curl_prot_t *val)
{
  /*
   * We are asked to cherry-pick protocols, so play it safe and disallow all
   * protocols to start with, and re-add the wanted ones back in.
   */
  *val = 0;

  ако(!str)
    врати CURLE_BAD_FUNCTION_ARGUMENT;

  ако(curl_strequal(str, "all")) {
    *val = ~(curl_prot_t)0;
    врати CURLE_OK;
  }

  do {
    const char *token = str;
    size_t tlen;

    str = strchr(str, ',');
    tlen = str ? (size_t)(str - token) : strlen(token);
    ако(tlen) {
      const struct Curl_scheme *h = Curl_getn_scheme(token, tlen);

      ако(!h || !h->run)
        врати CURLE_UNSUPPORTED_PROTOCOL;

      *val |= h->protocol;
    }
  } док(str && str++);

  ако(!*val)
    /* no protocol listed */
    врати CURLE_BAD_FUNCTION_ARGUMENT;
  врати CURLE_OK;
}

#ако !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
static CURLcode httpauth(struct Curl_easy *data, bool proxy,
                         unsigned long auth)
{
  ако(auth != CURLAUTH_NONE) {
    цео bitcheck = 0;
    bool authbits = FALSE;
    ако(auth & CURLAUTH_DIGEST_IE) {
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
    док(bitcheck < 31) {
      ако(auth & (1UL << bitcheck++)) {
        authbits = TRUE;
        break;
      }
    }
    ако(!authbits)
      врати CURLE_NOT_BUILT_IN; /* no supported types left! */
  }
  ако(proxy)
    data->set.proxyauth = (uint32_t)auth;
  инако
    data->set.httpauth = (uint32_t)auth;
  врати CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */

#ifndef CURL_DISABLE_HTTP
static CURLcode setopt_HTTP_VERSION(struct Curl_easy *data, long arg)
{
  /*
   * This sets a requested HTTP version to be used. The value is one of
   * the listed enums in curl/curl.h.
   */
  switch(arg) {
  case CURL_HTTP_VERSION_NONE:
    /* accepted */
    break;
  case CURL_HTTP_VERSION_1_0:
  case CURL_HTTP_VERSION_1_1:
    /* accepted */
    break;
#ifdef USE_HTTP2
  case CURL_HTTP_VERSION_2_0:
  case CURL_HTTP_VERSION_2TLS:
  case CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    /* accepted */
    break;
#endif
#ifdef USE_HTTP3
  case CURL_HTTP_VERSION_3:
  case CURL_HTTP_VERSION_3ONLY:
    /* accepted */
    break;
#endif
  default:
    /* not accepted */
    ако(arg < CURL_HTTP_VERSION_NONE)
      врати CURLE_BAD_FUNCTION_ARGUMENT;
    врати CURLE_UNSUPPORTED_PROTOCOL;
  }
  data->set.httpwant = (unsigned char)arg;
  врати CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP */

#ifdef USE_SSL
CURLcode Curl_setopt_SSLVERSION(struct Curl_easy *data, CURLoption option,
                                long arg)
{
  /*
   * Set explicit SSL version to try to connect with, as some SSL
   * implementations are lame.
   */
  {
    long version, version_max;
    struct ssl_easy_config *sslc = &data->set.ssl;
#ifndef CURL_DISABLE_PROXY
    ако(option != CURLOPT_SSLVERSION)
      sslc = &data->set.proxy_ssl;
#инако
    (void)option; /* unused */
#endif
    version = C_SSLVERSION_VALUE(arg);
    version_max = (long)C_SSLVERSION_MAX_VALUE(arg);

    ако(version < CURL_SSLVERSION_DEFAULT ||
       version == CURL_SSLVERSION_SSLv2 ||
       version == CURL_SSLVERSION_SSLv3 ||
       version >= CURL_SSLVERSION_LAST ||
       version_max < CURL_SSLVERSION_MAX_NONE ||
       version_max >= CURL_SSLVERSION_MAX_LAST)
      врати CURLE_BAD_FUNCTION_ARGUMENT;
    ако(version == CURL_SSLVERSION_DEFAULT)
      version = CURL_SSLVERSION_TLSv1_2;

    sslc->version = (unsigned char)version;
    sslc->version_max = (unsigned цео)version_max;
  }
  врати CURLE_OK;
}
#endif /* !USE_SSL */

#ifndef CURL_DISABLE_RTSP
static CURLcode setopt_RTSP_REQUEST(struct Curl_easy *data, long arg)
{
  /*
   * Set the RTSP request method (OPTIONS, SETUP, PLAY, etc...) Would this be
   * better if the RTSPREQ_* were moved into here?
   */
  ако((arg <= CURL_RTSPREQ_NONE) || (arg >= CURL_RTSPREQ_LAST))
    врати CURLE_BAD_FUNCTION_ARGUMENT;

  data->set.rtspreq = (unsigned char)arg;
  врати CURLE_OK;
}
#endif /* !CURL_DISABLE_RTSP */

static CURLcode setopt_long_bool(struct Curl_easy *data, CURLoption option,
                                 long arg)
{
  bool enabled = !!arg;
  цео ok = 1;
  struct UserDefined *s = &data->set;
  switch(option) {
  case CURLOPT_FORBID_REUSE:
    /*
     * When this transfer is done, it must not be left to be reused by a
     * subsequent transfer but shall be closed immediately.
     */
    s->reuse_forbid = enabled;
    break;
  case CURLOPT_FRESH_CONNECT:
    /*
     * This transfer shall not use a previously cached connection but
     * should be made with a fresh new connect!
     */
    s->reuse_fresh = enabled;
    break;
  case CURLOPT_VERBOSE:
    /*
     * Verbose means infof() calls that give a lot of information about
     * the connection and transfer procedures as well as internal choices.
     */
    s->verbose = enabled;
    break;
  case CURLOPT_HEADER:
    /*
     * Set to include the header in the general data output stream.
     */
    s->include_header = enabled;
    break;
  case CURLOPT_NOPROGRESS:
    /*
     * Shut off the internal supported progress meter
     */
    data->progress.hide = enabled;
    break;
  case CURLOPT_NOBODY:
    /*
     * Do not include the body part in the output data stream.
     */
    s->opt_no_body = enabled;
#ifndef CURL_DISABLE_HTTP
    ако(s->opt_no_body)
      /* in HTTP lingo, no body means using the HEAD request... */
      s->method = HTTPREQ_HEAD;
    инако ако(s->method == HTTPREQ_HEAD)
      s->method = HTTPREQ_GET;
#endif
    break;
  case CURLOPT_FAILONERROR:
    /*
     * Do not output the >=400 error code HTML-page, but instead only
     * return error.
     */
    s->http_fail_on_error = enabled;
    break;
  case CURLOPT_KEEP_SENDING_ON_ERROR:
    s->http_keep_sending_on_error = enabled;
    break;
  case CURLOPT_UPLOAD:
  case CURLOPT_PUT:
    /*
     * We want to send data to the remote host. If this is HTTP, that equals
     * using the PUT request.
     */
    ако(enabled) {
      /* If this is HTTP, PUT is what's needed to "upload" */
      s->method = HTTPREQ_PUT;
      s->opt_no_body = FALSE; /* this is implied */
    }
    инако
      /* In HTTP, the opposite of upload is GET (unless NOBODY is true as
         then this can be changed to HEAD later on) */
      s->method = HTTPREQ_GET;
    break;
  case CURLOPT_FILETIME:
    /*
     * Try to get the file time of the remote document. The time will
     * later (possibly) become available using curl_easy_getinfo().
     */
    s->get_filetime = enabled;
    break;
#ifndef CURL_DISABLE_HTTP
  case CURLOPT_HTTP09_ALLOWED:
    s->http09_allowed = enabled;
    break;
#ifndef CURL_DISABLE_COOKIES
  case CURLOPT_COOKIESESSION:
    /*
     * Set this option to TRUE to start a new "cookie session". It will
     * prevent the forthcoming read-cookies-from-file actions to accept
     * cookies that are marked as being session cookies, as they belong to a
     * previous session.
     */
    s->cookiesession = enabled;
    break;
#endif
  case CURLOPT_AUTOREFERER:
    /*
     * Switch on automatic referer that gets set if curl follows locations.
     */
    s->http_auto_referer = enabled;
    break;
  case CURLOPT_TRANSFER_ENCODING:
    s->http_transfer_encoding = enabled;
    break;
  case CURLOPT_UNRESTRICTED_AUTH:
    /*
     * Send authentication (user+password) when following locations, even when
     * hostname changed.
     */
    s->allow_auth_to_other_hosts = enabled;
    break;
  case CURLOPT_HTTP_TRANSFER_DECODING:
    /*
     * disable libcurl transfer encoding is used
     */
    s->http_te_skip = !enabled; /* reversed */
    break;
  case CURLOPT_HTTP_CONTENT_DECODING:
    /*
     * raw data passed to the application when content encoding is used
     */
    s->http_ce_skip = !enabled; /* reversed */
    break;
  case CURLOPT_HTTPGET:
    /*
     * Set to force us do HTTP GET
     */
    ако(enabled) {
      s->method = HTTPREQ_GET;
      s->opt_no_body = FALSE; /* this is implied */
    }
    break;
  case CURLOPT_POST:
    /* Does this option serve a purpose anymore? Yes it does, when
       CURLOPT_POSTFIELDS is not used and the POST data is read off the
       callback! */
    ако(enabled) {
      s->method = HTTPREQ_POST;
      s->opt_no_body = FALSE; /* this is implied */
    }
    инако
      s->method = HTTPREQ_GET;
    break;
#endif /* !CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_PROXY
  case CURLOPT_HTTPPROXYTUNNEL:
    /*
     * Tunnel operations through the proxy instead of normal proxy use
     */
    s->tunnel_thru_httpproxy = enabled;
    break;
  case CURLOPT_HAPROXYPROTOCOL:
    /*
     * Set to send the HAProxy Proxy Protocol header
     */
    s->haproxyprotocol = enabled;
    break;
  case CURLOPT_PROXY_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for proxy.
     */
    s->proxy_ssl.verifypeer = enabled;

    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    break;
  case CURLOPT_PROXY_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for proxy
     */
    s->proxy_ssl.verifyhost = enabled;
    ok = 2;
    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    break;
  case CURLOPT_PROXY_TRANSFER_MODE:
    /*
     * set transfer mode (;type=<a|i>) when doing FTP via an HTTP proxy
     */
    s->proxy_transfer_mode = enabled;
    break;
#endif /* !CURL_DISABLE_PROXY */
#ако defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  case CURLOPT_SOCKS5_GSSAPI_NEC:
    /*
     * Set flag for NEC SOCKS5 support
     */
    s->socks5_gssapi_nec = enabled;
    break;
#endif
#ifdef CURL_LIST_ONLY_PROTOCOL
  case CURLOPT_DIRLISTONLY:
    /*
     * An option that changes the command to one that asks for a list only, no
     * file info details. Used for FTP, POP3 and SFTP.
     */
    s->list_only = enabled;
    break;
#endif
  case CURLOPT_APPEND:
    /*
     * We want to upload and append to an existing file. Used for FTP and
     * SFTP.
     */
    s->remote_append = enabled;
    break;
#ifndef CURL_DISABLE_FTP
  case CURLOPT_FTP_USE_EPRT:
    s->ftp_use_eprt = enabled;
    break;
  case CURLOPT_FTP_USE_EPSV:
    s->ftp_use_epsv = enabled;
    break;
  case CURLOPT_FTP_USE_PRET:
    s->ftp_use_pret = enabled;
    break;
  case CURLOPT_FTP_SKIP_PASV_IP:
    /*
     * Enable or disable FTP_SKIP_PASV_IP, which will disable/enable the
     * bypass of the IP address in PASV responses.
     */
    s->ftp_skip_ip = enabled;
    break;
  case CURLOPT_WILDCARDMATCH:
    s->wildcard_enabled = enabled;
    break;
#endif
  case CURLOPT_CRLF:
    /*
     * Kludgy option to enable CRLF conversions. Subject for removal.
     */
    s->crlf = enabled;
    break;
#ifndef CURL_DISABLE_TFTP
  case CURLOPT_TFTP_NO_OPTIONS:
    /*
     * Option that prevents libcurl from sending TFTP option requests to the
     * server.
     */
    s->tftp_no_options = enabled;
    break;
#endif /* !CURL_DISABLE_TFTP */
  case CURLOPT_TRANSFERTEXT:
    /*
     * This option was previously named 'FTPASCII'. Renamed to work with
     * more protocols than merely FTP.
     *
     * Transfer using ASCII (instead of BINARY).
     */
    s->prefer_ascii = enabled;
    break;
  case CURLOPT_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying.
     */
    s->ssl.verifypeer = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    break;
#ifndef CURL_DISABLE_DOH
  case CURLOPT_DOH_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for DoH.
     */
    s->doh_verifypeer = enabled;
    break;
  case CURLOPT_DOH_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for DoH
     */
    s->doh_verifyhost = enabled;
    ok = 2;
    break;
  case CURLOPT_DOH_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying for DoH.
     */
    ако(!Curl_ssl_cert_status_request())
      врати CURLE_NOT_BUILT_IN;

    s->doh_verifystatus = enabled;
    ok = 2;
    break;
#endif /* !CURL_DISABLE_DOH */
  case CURLOPT_SSL_VERIFYHOST:
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
    break;
  case CURLOPT_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying.
     */
    ако(!Curl_ssl_cert_status_request())
      врати CURLE_NOT_BUILT_IN;

    s->ssl.verifystatus = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    break;
  case CURLOPT_CERTINFO:
#ifdef USE_SSL
    ако(Curl_ssl_supports(data, SSLSUPP_CERTINFO))
      s->ssl.certinfo = enabled;
    инако
#endif
      врати CURLE_NOT_BUILT_IN;
    break;
  case CURLOPT_NOSIGNAL:
    /*
     * The application asks not to set any signal() or alarm() handlers,
     * even when using a timeout.
     */
    s->no_signal = enabled;
    break;
  case CURLOPT_TCP_NODELAY:
    /*
     * Enable or disable TCP_NODELAY, which will disable/enable the Nagle
     * algorithm
     */
    s->tcp_nodelay = enabled;
    break;
  case CURLOPT_IGNORE_CONTENT_LENGTH:
    s->ignorecl = enabled;
    break;
  case CURLOPT_SSL_SESSIONID_CACHE:
    s->ssl.cache_session = enabled;
#ifndef CURL_DISABLE_PROXY
    s->proxy_ssl.cache_session = s->ssl.cache_session;
#endif
    break;
#ifdef USE_SSH
  case CURLOPT_SSH_COMPRESSION:
    s->ssh_compression = enabled;
    break;
#endif /* !USE_SSH */
#ifndef CURL_DISABLE_SMTP
  case CURLOPT_MAIL_RCPT_ALLOWFAILS:
    /* allow RCPT TO command to fail for some recipients */
    s->mail_rcpt_allowfails = enabled;
    break;
#endif /* !CURL_DISABLE_SMTP */
  case CURLOPT_SASL_IR:
    /* Enable/disable SASL initial response */
    s->sasl_ir = enabled;
    break;
  case CURLOPT_TCP_KEEPALIVE:
    s->tcp_keepalive = enabled;
    break;
  case CURLOPT_TCP_FASTOPEN:
#ако defined(CONNECT_DATA_IDEMPOTENT) || defined(MSG_FASTOPEN) || \
  defined(TCP_FASTOPEN_CONNECT)
    s->tcp_fastopen = enabled;
    break;
#инако
    врати CURLE_NOT_BUILT_IN;
#endif
  case CURLOPT_SSL_ENABLE_ALPN:
    s->ssl_enable_alpn = enabled;
    break;
  case CURLOPT_PATH_AS_IS:
    s->path_as_is = enabled;
    break;
  case CURLOPT_PIPEWAIT:
    s->pipewait = enabled;
    break;
  case CURLOPT_SUPPRESS_CONNECT_HEADERS:
    s->suppress_connect_headers = enabled;
    break;
#ifndef CURL_DISABLE_SHUFFLE_DNS
  case CURLOPT_DNS_SHUFFLE_ADDRESSES:
    s->dns_shuffle_addresses = enabled;
    break;
#endif
  case CURLOPT_DISALLOW_USERNAME_IN_URL:
    s->disallow_username_in_url = enabled;
    break;
  case CURLOPT_QUICK_EXIT:
    s->quick_exit = enabled;
    break;
  default:
    врати CURLE_UNKNOWN_OPTION;
  }
  ако((arg > ok) || (arg < 0))
    /* reserve other values for future use */
    infof(data, "boolean setopt(%d) got unsupported argument %ld,"
          " treated as %d", (цео)option, arg, enabled);

  врати CURLE_OK;
}

static CURLcode value_range(long *value, long below_error, long min, long max)
{
  ако(*value < below_error)
    врати CURLE_BAD_FUNCTION_ARGUMENT;
  инако ако(*value < min)
    *value = min;
  инако ако(*value > max)
    *value = max;
  врати CURLE_OK;
}

static CURLcode setopt_long_net(struct Curl_easy *data, CURLoption option,
                                long arg)
{
  CURLcode result = CURLE_OK;
  struct UserDefined *s = &data->set;

  switch(option) {
  case CURLOPT_DNS_CACHE_TIMEOUT:
    ако(arg != -1)
      врати setopt_set_timeout_sec(&s->dns_cache_timeout_ms, arg);
    s->dns_cache_timeout_ms = -1;
    break;
  case CURLOPT_MAXCONNECTS:
    result = value_range(&arg, 0, 0, INT_MAX);
    ако(!result)
      s->maxconnects = arg ? (uint32_t)arg : DEFAULT_CONNCACHE_SIZE;
    break;
  case CURLOPT_SERVER_RESPONSE_TIMEOUT:
    врати setopt_set_timeout_sec(&s->server_response_timeout, arg);
  case CURLOPT_SERVER_RESPONSE_TIMEOUT_MS:
    врати setopt_set_timeout_ms(&s->server_response_timeout, arg);
  case CURLOPT_LOW_SPEED_LIMIT:
    ако(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    инако
      s->low_speed_limit = arg;
    break;
  case CURLOPT_LOW_SPEED_TIME:
    result = value_range(&arg, 0, 0, USHRT_MAX);
    ако(!result)
      s->low_speed_time = (uint16_t)arg;
    break;
  case CURLOPT_PORT:
    ако((arg < 0) || (arg > 65535))
      врати CURLE_BAD_FUNCTION_ARGUMENT;
    s->use_port = (unsigned short)arg;
    break;
  case CURLOPT_TIMEOUT:
    врати setopt_set_timeout_sec(&s->timeout, arg);
  case CURLOPT_TIMEOUT_MS:
    врати setopt_set_timeout_ms(&s->timeout, arg);
  case CURLOPT_CONNECTTIMEOUT:
    врати setopt_set_timeout_sec(&s->connecttimeout, arg);
  case CURLOPT_CONNECTTIMEOUT_MS:
    врати setopt_set_timeout_ms(&s->connecttimeout, arg);
#ifndef CURL_DISABLE_BINDLOCAL
  case CURLOPT_LOCALPORT:
    ако((arg < 0) || (arg > 65535))
      врати CURLE_BAD_FUNCTION_ARGUMENT;
    s->localport = curlx_sltous(arg);
    break;
  case CURLOPT_LOCALPORTRANGE:
    ако((arg < 0) || (arg > 65535))
      врати CURLE_BAD_FUNCTION_ARGUMENT;
    s->localportrange = curlx_sltous(arg);
    break;
#endif
  case CURLOPT_BUFFERSIZE:
    result = value_range(&arg, 0, READBUFFER_MIN, READBUFFER_MAX);
    ако(!result)
      s->buffer_size = (unsigned цео)arg;
    break;
  case CURLOPT_UPLOAD_BUFFERSIZE:
    result = value_range(&arg, 0, UPLOADBUFFER_MIN, UPLOADBUFFER_MAX);
    ако(!result)
      s->upload_buffer_size = (unsigned цео)arg;
    break;
  case CURLOPT_MAXFILESIZE:
    ако(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    инако
      s->max_filesize = arg;
    break;
  case CURLOPT_IPRESOLVE:
    ако((arg < CURL_IPRESOLVE_WHATEVER) || (arg > CURL_IPRESOLVE_V6))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    инако
      s->ipver = (unsigned char)arg;
    break;
  case CURLOPT_CONNECT_ONLY:
    ако(arg < 0 || arg > 2)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    инако {
      s->connect_only = !!arg;
      s->connect_only_ws = (arg == 2);
    }
    break;
#ifdef USE_IPV6
  case CURLOPT_ADDRESS_SCOPE:
#ако SIZEOF_LONG > 4
    ако((unsigned long)arg > UINT_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    инако
#endif
    s->scope_id = (unsigned цео)arg;
    break;
#endif
  case CURLOPT_TCP_KEEPIDLE:
    result = value_range(&arg, 0, 0, INT_MAX);
    ако(!result)
      s->tcp_keepidle = (цео)arg;
    break;
  case CURLOPT_TCP_KEEPINTVL:
    result = value_range(&arg, 0, 0, INT_MAX);
    ако(!result)
      s->tcp_keepintvl = (цео)arg;
    break;
  case CURLOPT_TCP_KEEPCNT:
    result = value_range(&arg, 0, 0, INT_MAX);
    ако(!result)
      s->tcp_keepcnt = (цео)arg;
    break;
  case CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
    врати setopt_set_timeout_ms(&s->happy_eyeballs_timeout, arg);
  case CURLOPT_UPKEEP_INTERVAL_MS:
    врати setopt_set_timeout_ms(&s->upkeep_interval_ms, arg);
  case CURLOPT_MAXAGE_CONN:
    врати setopt_set_timeout_sec(&s->conn_max_idle_ms, arg);
  case CURLOPT_MAXLIFETIME_CONN:
    врати setopt_set_timeout_sec(&s->conn_max_age_ms, arg);
  case CURLOPT_DNS_USE_GLOBAL_CACHE:
    /* deprecated */
    break;
  default:
    врати CURLE_UNKNOWN_OPTION;
  }
  врати result;
}

static CURLcode setopt_long_ssl(struct Curl_easy *data, CURLoption option,
                                long arg)
{
#ifdef USE_SSL
  CURLcode result = CURLE_OK;
  struct UserDefined *s = &data->set;
  switch(option) {
  case CURLOPT_CA_CACHE_TIMEOUT:
    ако(Curl_ssl_supports(data, SSLSUPP_CA_CACHE)) {
      result = value_range(&arg, -1, -1, INT_MAX);
      ако(!result)
        s->ssl_ca_cache_timeout = (цео)arg;
    }
    инако
      result = CURLE_NOT_BUILT_IN;
    break;
  case CURLOPT_SSLVERSION:
#ifndef CURL_DISABLE_PROXY
  case CURLOPT_PROXY_SSLVERSION:
#endif
    врати Curl_setopt_SSLVERSION(data, option, arg);
  case CURLOPT_SSL_FALSESTART:
    result = CURLE_NOT_BUILT_IN;
    break;
  case CURLOPT_USE_SSL:
    ако((arg < CURLUSESSL_NONE) || (arg >= CURLUSESSL_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    инако
      s->use_ssl = (unsigned char)arg;
    break;
  case CURLOPT_SSL_OPTIONS:
    s->ssl.ssl_options = (unsigned char)(arg & 0xff);
    break;
#ifndef CURL_DISABLE_PROXY
  case CURLOPT_PROXY_SSL_OPTIONS:
    s->proxy_ssl.ssl_options = (unsigned char)(arg & 0xff);
    break;
#endif
  case CURLOPT_SSL_ENABLE_NPN:
    break;
  case CURLOPT_SSLENGINE_DEFAULT:
#ifdef USE_OPENSSL
    ако(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      врати CURLE_NOT_BUILT_IN;
    data->set.engine_default = (arg == 1);
#инако
    врати CURLE_NOT_BUILT_IN;
#endif
    break;
  default:
    врати CURLE_UNKNOWN_OPTION;
  }
  врати result;
#инако  /* USE_SSL */
  (void)data;
  (void)option;
  (void)arg;
  врати CURLE_UNKNOWN_OPTION;
#endif /* !USE_SSL */
}

#ifndef CURL_DISABLE_PROXY
static void changeproxy(struct Curl_easy *data)
{
  Curl_auth_digest_cleanup(&data->state.proxydigest);
  memset(&data->state.authproxy, 0, sizeof(data->state.authproxy));
}

static CURLcode setopt_long_proxy(struct Curl_easy *data, CURLoption option,
                                  long arg)
{
  struct UserDefined *s = &data->set;

  switch(option) {
  case CURLOPT_PROXYPORT:
    ако((arg < 0) || (arg > UINT16_MAX))
      врати CURLE_BAD_FUNCTION_ARGUMENT;
    ако(arg != s->proxyport)
      changeproxy(data);
    s->proxyport = (uint16_t)arg;
    break;
  case CURLOPT_PROXYAUTH:
    врати httpauth(data, TRUE, (unsigned long)arg);
  case CURLOPT_PROXYTYPE:
    ако((arg < CURLPROXY_HTTP) || (arg > CURLPROXY_HTTPS3))
      врати CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef USE_PROXY_HTTP3
    ако(arg == CURLPROXY_HTTPS3)
      врати CURLE_NOT_BUILT_IN;
#endif
    s->proxytype = (unsigned char)arg;
    break;
  case CURLOPT_SOCKS5_AUTH: {
    unsigned long auth = (unsigned long)arg;
    ако(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI | CURLAUTH_ONLY))
      врати CURLE_NOT_BUILT_IN;
    ако((auth & CURLAUTH_ONLY) && !(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI)))
      врати CURLE_BAD_FUNCTION_ARGUMENT;
    s->socks5auth = (unsigned char)(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI));
    s->socks5_auth_only = !!(auth & CURLAUTH_ONLY);
    break;
  }
  default:
    врати CURLE_UNKNOWN_OPTION;
  }
  врати CURLE_OK;
}
#инако
static CURLcode setopt_long_proxy(struct Curl_easy *data, CURLoption option,
                                  long arg)
{
  (void)data;
  (void)option;
  (void)arg;
  врати CURLE_UNKNOWN_OPTION;
}
#endif

static CURLcode setopt_long_http(struct Curl_easy *data, CURLoption option,
                                 long arg)
{
#ifndef CURL_DISABLE_HTTP
  CURLcode result = CURLE_OK;
  struct UserDefined *s = &data->set;

  switch(option) {
  case CURLOPT_FOLLOWLOCATION:
    ако((unsigned long)arg > 3)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    инако
      s->http_follow_mode = (unsigned char)arg;
    break;
  case CURLOPT_MAXREDIRS:
    result = value_range(&arg, -1, -1, 0x7fff);
    ако(!result)
      s->maxredirs = (short)arg;
    break;
  case CURLOPT_POSTREDIR:
    ако(arg < CURL_REDIR_GET_ALL)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    инако {
      s->post301 = !!(arg & CURL_REDIR_POST_301);
      s->post302 = !!(arg & CURL_REDIR_POST_302);
      s->post303 = !!(arg & CURL_REDIR_POST_303);
    }
    break;
  case CURLOPT_HEADEROPT:
    s->sep_headers = !!(arg & CURLHEADER_SEPARATE);
    break;
  case CURLOPT_HTTPAUTH:
    врати httpauth(data, FALSE, (unsigned long)arg);
  case CURLOPT_HTTP_VERSION:
    врати setopt_HTTP_VERSION(data, arg);
  case CURLOPT_EXPECT_100_TIMEOUT_MS:
    result = value_range(&arg, 0, 0, 0xffff);
    ако(!result)
      s->expect_100_timeout = (unsigned short)arg;
    break;
  case CURLOPT_STREAM_WEIGHT:
#ако defined(USE_HTTP2) || defined(USE_HTTP3)
    ако((arg >= 1) && (arg <= 256))
      s->weight = (цео)arg;
    break;
#инако
    result = CURLE_NOT_BUILT_IN;
    break;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  case CURLOPT_HTTPSIG_ALGORITHM:
    ако(arg != CURLHTTPSIG_NONE &&
       arg != CURLHTTPSIG_ED25519 &&
       arg != CURLHTTPSIG_HMAC_SHA256)
      врати CURLE_BAD_FUNCTION_ARGUMENT;
    s->httpsig_algorithm = (uint8_t)arg;
    ако(arg)
      s->httpauth = (uint32_t)CURLAUTH_HTTPSIG;
    инако
      s->httpauth &= ~(uint32_t)CURLAUTH_HTTPSIG;
    break;
#endif
  default:
    врати CURLE_UNKNOWN_OPTION;
  }
  врати result;
#инако
  (void)data;
  (void)option;
  (void)arg;
  врати CURLE_UNKNOWN_OPTION;
#endif
}

static CURLcode setopt_long_proto(struct Curl_easy *data, CURLoption option,
                                  long arg)
{
  CURLcode result = CURLE_OK;
  struct UserDefined *s = &data->set;

  switch(option) {
#ifndef CURL_DISABLE_TFTP
  case CURLOPT_TFTP_BLKSIZE:
    result = value_range(&arg, 0, TFTP_BLKSIZE_MIN, TFTP_BLKSIZE_MAX);
    ако(!result)
      s->tftp_blksize = (unsigned short)arg;
    break;
#endif
#ifndef CURL_DISABLE_NETRC
  case CURLOPT_NETRC:
    ако((arg < CURL_NETRC_IGNORED) || (arg >= CURL_NETRC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    инако
      s->use_netrc = (unsigned char)arg;
    break;
#endif
#ifndef CURL_DISABLE_FTP
  case CURLOPT_FTP_FILEMETHOD:
    ако((arg < CURLFTPMETHOD_DEFAULT) || (arg >= CURLFTPMETHOD_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    инако
      s->ftp_filemethod = (unsigned char)arg;
    break;
  case CURLOPT_FTP_SSL_CCC:
    ако((arg < CURLFTPSSL_CCC_NONE) || (arg >= CURLFTPSSL_CCC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    инако
      s->ftp_ccc = (unsigned char)arg;
    break;
  case CURLOPT_FTPSSLAUTH:
    ако((arg < CURLFTPAUTH_DEFAULT) || (arg >= CURLFTPAUTH_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    инако
      s->ftpsslauth = (unsigned char)arg;
    break;
  case CURLOPT_ACCEPTTIMEOUT_MS:
    врати setopt_set_timeout_ms(&s->accepttimeout, arg);
#endif
#ако !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  case CURLOPT_FTP_CREATE_MISSING_DIRS:
    ако((arg < CURLFTP_CREATE_DIR_NONE) || (arg > CURLFTP_CREATE_DIR_RETRY))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    инако
      s->ftp_create_missing_dirs = (unsigned char)arg;
    break;
  case CURLOPT_NEW_FILE_PERMS:
    ако((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    инако
      s->new_file_perms = (unsigned цео)arg;
    break;
#endif
#ifndef CURL_DISABLE_RTSP
  case CURLOPT_RTSP_REQUEST:
    врати setopt_RTSP_REQUEST(data, arg);
  case CURLOPT_RTSP_CLIENT_CSEQ:
    result = value_range(&arg, 0, 0, INT_MAX);
    ако(!result)
      data->state.rtsp_next_client_CSeq = (uint32_t)arg;
    break;
  case CURLOPT_RTSP_SERVER_CSEQ:
    result = value_range(&arg, 0, 0, INT_MAX);
    ако(!result)
      data->state.rtsp_next_server_CSeq = (uint32_t)arg;
    break;
#endif
#ifdef USE_SSH
  case CURLOPT_SSH_AUTH_TYPES:
    s->ssh_auth_types = (uint32_t)arg;
    break;
  case CURLOPT_NEW_DIRECTORY_PERMS:
    ако((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    инако
      s->new_directory_perms = (unsigned цео)arg;
    break;
#endif
  case CURLOPT_PROTOCOLS:
    s->allowed_protocols = (curl_prot_t)arg;
    break;
  case CURLOPT_REDIR_PROTOCOLS:
    s->redir_protocols = (curl_prot_t)arg;
    break;
#ifndef CURL_DISABLE_WEBSOCKETS
  case CURLOPT_WS_OPTIONS:
    s->ws_raw_mode = !!(arg & CURLWS_RAW_MODE);
    s->ws_no_auto_pong = !!(arg & CURLWS_NOAUTOPONG);
    break;
#endif
  default:
    врати CURLE_UNKNOWN_OPTION;
  }
  врати result;
}

static CURLcode setopt_long_misc(struct Curl_easy *data, CURLoption option,
                                 long arg)
{
  struct UserDefined *s = &data->set;

  switch(option) {
  case CURLOPT_TIMECONDITION:
    ако((arg < CURL_TIMECOND_NONE) || (arg >= CURL_TIMECOND_LAST))
      врати CURLE_BAD_FUNCTION_ARGUMENT;
    s->timecondition = (unsigned char)arg;
    break;
  case CURLOPT_TIMEVALUE:
    s->timevalue = (time_t)arg;
    break;
  case CURLOPT_POSTFIELDSIZE:
    ако(arg < -1)
      врати CURLE_BAD_FUNCTION_ARGUMENT;
    ако(s->postfieldsize < arg && s->str_copypostfields) {
      curlx_safefree(s->str_copypostfields);
      s->postfields = NULL;
    }
    s->postfieldsize = arg;
    break;
  case CURLOPT_INFILESIZE:
    ако(arg < -1)
      врати CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = arg;
    break;
  case CURLOPT_RESUME_FROM:
    ако(arg < -1)
      врати CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = arg;
    break;
  case CURLOPT_UPLOAD_FLAGS:
    s->upload_flags = (unsigned char)arg;
    break;
#ifndef CURL_DISABLE_MIME
  case CURLOPT_MIME_OPTIONS:
    s->mime_formescape = !!(arg & CURLMIMEOPT_FORMESCAPE);
    break;
#endif
#ifndef CURL_DISABLE_HSTS
  case CURLOPT_HSTS_CTRL:
    ако(arg & CURLHSTS_ENABLE) {
      ако(!data->hsts) {
        data->hsts = Curl_hsts_init();
        ако(!data->hsts)
          врати CURLE_OUT_OF_MEMORY;
      }
    }
    инако ако(!data->share || !data->share->hsts) {
      /* throw away the HSTS cache unless shared */
      Curl_hsts_cleanup(&data->hsts);
      /* flush all the entries */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = NULL;
    }
    инако
      /* detach from shared HSTS cache without freeing it */
      data->hsts = NULL;
    break;
#endif
#ifndef CURL_DISABLE_ALTSVC
  case CURLOPT_ALTSVC_CTRL:
    врати Curl_altsvc_ctrl(data, arg);
#endif
#ако defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  case CURLOPT_GSSAPI_DELEGATION:
    s->gssapi_delegation = (unsigned char)arg &
      (CURLGSSAPI_DELEGATION_POLICY_FLAG | CURLGSSAPI_DELEGATION_FLAG);
    break;
#endif
  default:
    врати CURLE_UNKNOWN_OPTION;
  }
  врати CURLE_OK;
}

static CURLcode setopt_long(struct Curl_easy *data, CURLoption option,
                            long arg)
{
  typedef CURLcode (*setoptfunc)(struct Curl_easy *data,
                                 CURLoption option, long arg);
  static const setoptfunc setopt_call[] = {
    setopt_long_bool,
    setopt_long_net,
    setopt_long_http,
    setopt_long_proxy,
    setopt_long_ssl,
    setopt_long_proto,
    setopt_long_misc
  };
  size_t i;

  за(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, arg);
    ако(result != CURLE_UNKNOWN_OPTION)
      врати result;
  }
  врати CURLE_UNKNOWN_OPTION;
}

static CURLcode setopt_slist(struct Curl_easy *data, CURLoption option,
                             struct curl_slist *slist)
{
  CURLcode result = CURLE_OK;
  struct UserDefined *s = &data->set;
  switch(option) {
#ifndef CURL_DISABLE_PROXY
  case CURLOPT_PROXYHEADER:
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
    break;
#endif
#ifndef CURL_DISABLE_HTTP
  case CURLOPT_HTTP200ALIASES:
    /*
     * Set a list of aliases for HTTP 200 in response header
     */
    s->http200aliases = slist;
    break;
#endif
#ако !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  case CURLOPT_POSTQUOTE:
    /*
     * List of RAW FTP commands to use after a transfer
     */
    s->postquote = slist;
    break;
  case CURLOPT_PREQUOTE:
    /*
     * List of RAW FTP commands to use prior to RETR (Wesley Laxton)
     */
    s->prequote = slist;
    break;
  case CURLOPT_QUOTE:
    /*
     * List of RAW FTP commands to use before a transfer
     */
    s->quote = slist;
    break;
#endif
  case CURLOPT_RESOLVE:
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
    break;
#ако !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MIME)
  case CURLOPT_HTTPHEADER:
    /*
     * Set a list with HTTP headers to use (or replace internals with)
     */
    s->headers = slist;
    break;
#endif
#ifndef CURL_DISABLE_TELNET
  case CURLOPT_TELNETOPTIONS:
    /*
     * Set a linked list of telnet options
     */
    s->telnet_options = slist;
    break;
#endif
#ifndef CURL_DISABLE_SMTP
  case CURLOPT_MAIL_RCPT:
    /* Set the list of mail recipients */
    s->mail_rcpt = slist;
    break;
#endif
  case CURLOPT_CONNECT_TO:
    s->connect_to = slist;
    break;
  default:
    врати CURLE_UNKNOWN_OPTION;
  }
  врати result;
}

#ако !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
static CURLcode setopt_mimepost(struct Curl_easy *data, curl_mime *mimep)
{
  /*
   * Set to make us do MIME POST
   */
  CURLcode result;
  struct UserDefined *s = &data->set;
  ако(!s->mimepostp) {
    s->mimepostp = curlx_malloc(sizeof(*s->mimepostp));
    ако(!s->mimepostp)
      врати CURLE_OUT_OF_MEMORY;
    Curl_mime_initpart(s->mimepostp);
  }

  result = Curl_mime_set_subparts(s->mimepostp, mimep, FALSE);
  ако(!result) {
    s->method = HTTPREQ_POST_MIME;
    s->opt_no_body = FALSE; /* this is implied */
#ifndef CURL_DISABLE_FORM_API
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = NULL;
#endif
  }
  врати result;
}
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */

static CURLcode setopt_share(struct Curl_easy *data, struct Curl_share *set)
{
  CURLcode result;

  ако(data->conn) {
    /* As this handle already has a connection attached, changing share now
       would be complicated and error-prone */
    infof(data, "Cannot change share object while in use");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
  }
  инако {
    /* disconnect from old share, if any and possible */
    result = Curl_share_easy_unlink(data);
    ако(!result && GOOD_SHARE_HANDLE(set))
      /* use new share if it set */
      result = Curl_share_easy_link(data, set);
  }
  врати result;
}

/* assorted pointer type arguments */
static CURLcode setopt_pointers(struct Curl_easy *data, CURLoption option,
                                va_list param)
{
  CURLcode result = CURLE_OK;
  struct UserDefined *s = &data->set;
  switch(option) {
  case CURLOPT_CURLU:
    /*
     * pass CURLU to set URL
     */
    Curl_bufref_free(&data->state.url);
    CURL_EASY_STR_CLEAR(data, STRING_SET_URL);
    s->uh = va_arg(param, CURLU *);
    break;
#ifndef CURL_DISABLE_HTTP
#ifndef CURL_DISABLE_FORM_API
  case CURLOPT_HTTPPOST:
    /*
     * Set to make us do HTTP POST. Legacy API-style.
     */
    s->httppost = va_arg(param, struct curl_httppost *);
    s->method = HTTPREQ_POST_FORM;
    s->opt_no_body = FALSE; /* this is implied */
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = NULL;
    break;
#endif /* !CURL_DISABLE_FORM_API */
#endif /* !CURL_DISABLE_HTTP */
#ако !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
  case CURLOPT_MIMEPOST:
    result = setopt_mimepost(data, va_arg(param, curl_mime *));
    break;
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */
  case CURLOPT_STDERR:
    /*
     * Set to a FILE * that should receive all error writes. This
     * defaults to stderr for normal operations.
     */
    s->err = va_arg(param, FILE *);
    ако(!s->err)
      s->err = stderr;
    break;
  case CURLOPT_SHARE:
    врати setopt_share(data, va_arg(param, struct Curl_share *));

  default:
    врати CURLE_UNKNOWN_OPTION;
  }
  врати result;
}

#ifndef CURL_DISABLE_COOKIES
static CURLcode cookielist(struct Curl_easy *data, const char *ptr)
{
  CURLcode result = CURLE_OK;
  ако(!ptr)
    врати CURLE_OK;

  ако(curl_strequal(ptr, "ALL")) {
    /* clear all cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearall(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  инако ако(curl_strequal(ptr, "SESS")) {
    /* clear session cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearsess(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  инако ако(curl_strequal(ptr, "FLUSH")) {
    /* flush cookies to file, takes care of the locking */
    Curl_flush_cookies(data, FALSE);
  }
  инако ако(curl_strequal(ptr, "RELOAD")) {
    /* reload cookies from file */
    врати Curl_cookie_loadfiles(data, COOKIE_NOPSL |
                                 (data->set.cookiesession ?
                                  COOKIE_NOSESSION : 0));
  }
  инако {
    ако(!data->cookies) {
      /* if cookie engine was not running, activate it */
      data->cookies = Curl_cookie_init();
      ако(!data->cookies)
        врати CURLE_OUT_OF_MEMORY;
      data->state.cookie_engine = TRUE;
    }

    /* general protection against mistakes and abuse */
    ако(strlen(ptr) > CURL_MAX_INPUT_LENGTH)
      врати CURLE_BAD_FUNCTION_ARGUMENT;

    /* Adding these cookies without the PSL check, because the PSL is not
       initialized until *perform() time, and this might be called before
       that */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    ако(checkprefix("Set-Cookie:", ptr))
      /* HTTP Header format line */
      result = Curl_cookie_add(data, data->cookies, ptr + 11,
                               NULL, NULL,
                               COOKIE_HTTPHEADER | COOKIE_SECURE |
                               COOKIE_NOPSL);
    инако
      /* Netscape format line */
      result = Curl_cookie_add(data, data->cookies, ptr, NULL,
                               NULL, COOKIE_SECURE | COOKIE_NOPSL);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  врати result;
}

static CURLcode cookiefile(struct Curl_easy *data, const char *ptr)
{
  /*
   * Set cookie file to read and parse. Can be used multiple times.
   */
  ако(ptr) {
    struct curl_slist *cl;
    /* general protection against mistakes and abuse */
    ако(strlen(ptr) > CURL_MAX_INPUT_LENGTH)
      врати CURLE_BAD_FUNCTION_ARGUMENT;
    /* append the cookie filename to the list of filenames, and deal with
       them later */
    cl = curl_slist_append(data->state.cookielist, ptr);
    ако(!cl) {
      curl_slist_free_all(data->state.cookielist);
      data->state.cookielist = NULL;
      врати CURLE_OUT_OF_MEMORY;
    }
    data->state.cookielist = cl; /* store the list for later use */
  }
  инако {
    /* clear the list of cookie files */
    curl_slist_free_all(data->state.cookielist);
    data->state.cookielist = NULL;

    ако(!data->share || !data->share->cookies) {
      /* throw away all existing cookies if this is not a shared cookie
         container */
      Curl_cookie_clearall(data->cookies);
      Curl_cookie_cleanup(data->cookies);
    }
    /* disable the cookie engine */
    data->cookies = NULL;
  }
  врати CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_PROXY
static CURLcode setproxy(struct Curl_easy *data, const char *proxy)
{
  const char *str = CURL_EASY_STR(data, STRING_PROXY);
  ако(str && proxy &&
     /* there was one set, is this a new one? */
     !strcmp(str, proxy))
    врати CURLE_OK; /* same one as before */

  changeproxy(data);
  врати Curl_setstropt(data, STRING_PROXY, proxy);
}

static CURLcode setopt_cptr_proxy(struct Curl_easy *data, CURLoption option,
                                  char *ptr)
{
  CURLcode result = CURLE_OK;
  struct UserDefined *s = &data->set;
  switch(option) {
  case CURLOPT_PROXYUSERPWD: {
    /*
     * user:password needed to use the proxy
     */
    char *u = NULL;
    char *p = NULL;
    result = setstropt_userpwd(ptr, &u, &p);

    /* URL decode the components */
    ако(!result) {
      char *str = NULL;
      CURL_EASY_STR_CLEAR(data, STRING_PROXYUSERNAME);
      CURL_EASY_STR_CLEAR(data, STRING_PROXYPASSWORD);
      ако(u) {
        result = Curl_urldecode(u, 0, &str, NULL, REJECT_ZERO);
        ако(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYUSERNAME, str);
      }
      ако(!result && p) {
        str = NULL;
        result = Curl_urldecode(p, 0, &str, NULL, REJECT_ZERO);
        ако(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYPASSWORD, str);
      }
    }
    curlx_free(u);
    curlx_strzero(p);
    curlx_free(p);
    break;
  }
  case CURLOPT_PROXYUSERNAME:
    /*
     * authentication username to use in the operation
     */
    врати Curl_setstropt(data, STRING_PROXYUSERNAME, ptr);

  case CURLOPT_PROXYPASSWORD:
    /*
     * authentication password to use in the operation
     */
    врати Curl_setstropt(data, STRING_PROXYPASSWORD, ptr);

  case CURLOPT_NOPROXY:
    /*
     * proxy exception list
     */
    врати Curl_setstropt(data, STRING_NOPROXY, ptr);
  case CURLOPT_PROXY_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use for proxy
     */
    врати Curl_setstropt(data, STRING_CERT_PROXY, ptr);
  case CURLOPT_PROXY_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use for proxy
     */
    врати Curl_setstropt(data, STRING_CERT_TYPE_PROXY, ptr);
  case CURLOPT_PROXY_SSLKEY:
    /*
     * String that holds filename of the SSL key to use for proxy
     */
    врати Curl_setstropt(data, STRING_KEY_PROXY, ptr);
  case CURLOPT_PROXY_KEYPASSWD:
    /*
     * String that holds the SSL private key password for proxy.
     */
    врати Curl_setstropt(data, STRING_KEY_PASSWD_PROXY, ptr);
  case CURLOPT_PROXY_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use for proxy
     */
    врати Curl_setstropt(data, STRING_KEY_TYPE_PROXY, ptr);
  case CURLOPT_PROXY_SSL_CIPHER_LIST:
    ако(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST)) {
      /* set a list of cipher we want to use in the SSL connection for proxy */
      врати Curl_setstropt(data, STRING_SSL_CIPHER_LIST_PROXY, ptr);
    }
    инако
      врати CURLE_NOT_BUILT_IN;
  case CURLOPT_PROXY_TLS13_CIPHERS:
    ако(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites for proxy */
      врати Curl_setstropt(data, STRING_SSL_CIPHER13_LIST_PROXY, ptr);
    инако
      врати CURLE_NOT_BUILT_IN;
  case CURLOPT_PROXY:
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
    врати setproxy(data, ptr);
  case CURLOPT_PRE_PROXY:
    /*
     * Set proxy server:port to use as SOCKS proxy.
     *
     * If the proxy is set to "" or NULL we explicitly say that we do not want
     * to use the socks proxy.
     */
    врати Curl_setstropt(data, STRING_PRE_PROXY, ptr);
  case CURLOPT_SOCKS5_GSSAPI_SERVICE:
  case CURLOPT_PROXY_SERVICE_NAME:
    /*
     * Set proxy authentication service name for Kerberos 5 and SPNEGO
     */
    врати Curl_setstropt(data, STRING_PROXY_SERVICE_NAME, ptr);
  case CURLOPT_PROXY_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
#ifdef USE_SSL
    ако(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      врати Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY_PROXY, ptr);
#endif
    врати CURLE_NOT_BUILT_IN;

  case CURLOPT_HAPROXY_CLIENT_IP:
    /*
     * Set the client IP to send through HAProxy PROXY protocol
     */
    result = Curl_setstropt(data, STRING_HAPROXY_CLIENT_IP, ptr);

    /* enable the HAProxy protocol if an IP is provided */
    s->haproxyprotocol = !!CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
    break;
  case CURLOPT_PROXY_CAINFO:
    /*
     * Set CA info SSL connection for proxy. Specify filename of the
     * CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE_PROXY, ptr);
    ако(!result)
      s->proxy_ssl.custom_cafile = TRUE;
    врати result;
  case CURLOPT_PROXY_CRLFILE:
    /*
     * Set CRL file info for SSL connection for proxy. Specify filename of the
     * CRL to check certificates revocation
     */
    ако(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      врати Curl_setstropt(data, STRING_SSL_CRLFILE_PROXY, ptr);
    врати CURLE_NOT_BUILT_IN;
  case CURLOPT_PROXY_ISSUERCERT:
    /*
     * Set Issuer certificate file to check certificates issuer
     */
    ако(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      врати Curl_setstropt(data, STRING_SSL_ISSUERCERT_PROXY, ptr);
    врати CURLE_NOT_BUILT_IN;
  case CURLOPT_PROXY_CAPATH:
    /*
     * Set CA path info for SSL connection proxy. Specify directory name of the
     * CA certificates which have been prepared using openssl c_rehash utility.
     */
#ifdef USE_SSL
    ако(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH_PROXY, ptr);
      ако(!result)
        s->proxy_ssl.custom_capath = TRUE;
      врати result;
    }
#endif
    врати CURLE_NOT_BUILT_IN;
  default:
    врати CURLE_UNKNOWN_OPTION;
  }
  врати result;
}
#endif

#ако !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
/*
 * A string with POST data. Makes curl HTTP POST. Even if it is NULL. If
 * needed, CURLOPT_POSTFIELDSIZE must have been set prior to
 * CURLOPT_COPYPOSTFIELDS and not altered later.
 */
static CURLcode setopt_copypostfields(const char *ptr, struct UserDefined *s)
{
  CURLcode result = CURLE_OK;
  ако(s->postfieldsize < -1)
    врати CURLE_BAD_FUNCTION_ARGUMENT;
  ако(!ptr || s->postfieldsize == -1) {
    ако(ptr && (strlen(ptr) > CURL_MAX_INPUT_LENGTH))
      врати CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_safefree(s->str_copypostfields);
    ако(ptr) {
      s->str_copypostfields = curlx_strdup(ptr);
      ако(!s->str_copypostfields)
        врати CURLE_OUT_OF_MEMORY;
    }
  }
  инако {
    size_t pflen = curlx_sotouz_range(s->postfieldsize, 0, SIZE_MAX);
    ако(pflen == SIZE_MAX)
      врати CURLE_OUT_OF_MEMORY;
    инако {
      /* Allocate even when size == 0. This satisfies the need of possible
         later address compare to detect the COPYPOSTFIELDS mode, and to mark
         that postfields is used rather than read function or form data. */
      char *p = curlx_memdup0(ptr, pflen);
      ако(!p)
        врати CURLE_OUT_OF_MEMORY;
      инако {
        curlx_free(s->str_copypostfields);
        s->str_copypostfields = p;
      }
    }
  }

  s->postfields = s->str_copypostfields;
  s->method = HTTPREQ_POST;
  врати result;
}
#endif

#ifdef USE_ECH
static CURLcode setopt_ech(struct Curl_easy *data, const char *ptr)
{
  struct UserDefined *s = &data->set;
  CURLcode result = CURLE_OK;

  ако(!ptr || !strcmp(ptr, "false"))
    s->ssl.ech = CURLECH_DISABLE;
  инако {
    size_t plen = strlen(ptr);
    ако(plen > CURL_MAX_INPUT_LENGTH)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    инако {
      ако(!strcmp(ptr, "grease"))
        s->ssl.ech = CURLECH_GREASE;
      инако ако(!strcmp(ptr, "true"))
        s->ssl.ech = CURLECH_ENABLE;
      инако ако(!strcmp(ptr, "hard"))
        s->ssl.ech = CURLECH_HARD;
      инако ако(plen > 4 && !strncmp(ptr, "ecl:", 4)) {
        ако(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_CONFIG, ptr + 4);
      }
      инако ако(plen > 3 && !strncmp(ptr, "pn:", 3)) {
        ако(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_PUBLIC, ptr + 3);
      }
      инако
        result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  врати result;
}
#инако
#define setopt_ech(x, y) CURLE_NOT_BUILT_IN
#endif

#ако defined(USE_SSL) || defined(USE_SSH)
/* One of the options is used for both TLS and SSH */
static CURLcode setopt_cptr_ssl(struct Curl_easy *data, CURLoption option,
                                char *ptr)
{
  CURLcode result = CURLE_OK;

  switch(option) {
  case CURLOPT_KEYPASSWD:
    /*
     * String that holds the SSL or SSH private key password.
     */
    result = Curl_setstropt(data, STRING_KEY_PASSWD, ptr);
    break;
#ifdef USE_SSL
  case CURLOPT_CAINFO:
    /*
     * Set CA info for SSL connection. Specify filename of the CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE, ptr);
    ако(!result)
      data->set.ssl.custom_cafile = TRUE;
    врати result;
  case CURLOPT_CAPATH:
    /*
     * Set CA path info for SSL connection. Specify directory name of the CA
     * certificates which have been prepared using openssl c_rehash utility.
     */
    ако(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH, ptr);
      ако(!result)
        data->set.ssl.custom_capath = TRUE;
      врати result;
    }
    врати CURLE_NOT_BUILT_IN;
  case CURLOPT_CRLFILE:
    /*
     * Set CRL file info for SSL connection. Specify filename of the CRL
     * to check certificates revocation
     */
    ако(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      врати Curl_setstropt(data, STRING_SSL_CRLFILE, ptr);
    врати CURLE_NOT_BUILT_IN;
  case CURLOPT_SSL_CIPHER_LIST:
    ако(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST))
      /* set a list of cipher we want to use in the SSL connection */
      врати Curl_setstropt(data, STRING_SSL_CIPHER_LIST, ptr);
    инако
      врати CURLE_NOT_BUILT_IN;
  case CURLOPT_TLS13_CIPHERS:
    ако(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites */
      врати Curl_setstropt(data, STRING_SSL_CIPHER13_LIST, ptr);
    инако
      врати CURLE_NOT_BUILT_IN;
  case CURLOPT_RANDOM_FILE:
    break;
  case CURLOPT_EGDSOCKET:
    break;
  case CURLOPT_SSL_CTX_DATA:
    /*
     * Set an SSL_CTX callback parameter pointer
     */
    ако(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      data->set.ssl_fsslctxp = ptr;
      break;
    }
    инако
      врати CURLE_NOT_BUILT_IN;
  case CURLOPT_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use
     */
    врати Curl_setstropt(data, STRING_CERT, ptr);
  case CURLOPT_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use
     */
    врати Curl_setstropt(data, STRING_CERT_TYPE, ptr);
  case CURLOPT_SSLKEY:
    /*
     * String that holds filename of the SSL key to use
     */
    врати Curl_setstropt(data, STRING_KEY, ptr);
  case CURLOPT_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use
     */
    врати Curl_setstropt(data, STRING_KEY_TYPE, ptr);
  case CURLOPT_SSLENGINE:
    /*
     * String that holds the SSL crypto engine.
     */
    ако(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      врати CURLE_NOT_BUILT_IN;
    result = Curl_setstropt(data, STRING_SSL_ENGINE, ptr);
    break;
  case CURLOPT_ISSUERCERT:
    /*
     * Set Issuer certificate file
     * to check certificates issuer
     */
    ако(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      врати Curl_setstropt(data, STRING_SSL_ISSUERCERT, ptr);
    врати CURLE_NOT_BUILT_IN;
  case CURLOPT_SSL_EC_CURVES:
    /*
     * Set accepted curves in SSL connection setup.
     * Specify colon-delimited list of curve algorithm names.
     */
    ако(Curl_ssl_supports(data, SSLSUPP_SSL_EC_CURVES))
      врати Curl_setstropt(data, STRING_SSL_EC_CURVES, ptr);
    врати CURLE_NOT_BUILT_IN;
  case CURLOPT_SSL_SIGNATURE_ALGORITHMS:
    /*
     * Set accepted signature algorithms.
     * Specify colon-delimited list of signature scheme names.
     */
    ако(Curl_ssl_supports(data, SSLSUPP_SIGNATURE_ALGORITHMS))
      врати Curl_setstropt(data, STRING_SSL_SIGNATURE_ALGORITHMS, ptr);
    врати CURLE_NOT_BUILT_IN;
  case CURLOPT_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
    ако(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      врати Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY, ptr);
    врати CURLE_NOT_BUILT_IN;
  case CURLOPT_ECH:
    врати setopt_ech(data, ptr);
#endif
  default:
    врати CURLE_UNKNOWN_OPTION;
  }
  врати result;
}
#endif

#ако !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
static CURLcode setopt_cptr_http_mqtt(struct Curl_easy *data,
                                      CURLoption option, char *ptr)
{
  CURLcode result = CURLE_OK;
  struct UserDefined *s = &data->set;

  switch(option) {
  case CURLOPT_COPYPOSTFIELDS:
    врати setopt_copypostfields(ptr, s);

  case CURLOPT_POSTFIELDS:
    /*
     * Like above, but use static data instead of copying it.
     */
    s->postfields = ptr;
    /* Release old copied data. */
    curlx_safefree(s->str_copypostfields);
    s->method = HTTPREQ_POST;
    break;

#ifndef CURL_DISABLE_HTTP
  case CURLOPT_TRAILERDATA:
    s->trailer_data = ptr;
    break;
  case CURLOPT_ACCEPT_ENCODING:
    /*
     * String to use at the value of Accept-Encoding header.
     *
     * If the encoding is set to "" we use an Accept-Encoding header that
     * encompasses all the encodings we support.
     * If the encoding is set to NULL we do not send an Accept-Encoding header
     * and ignore any received Content-Encoding header.
     *
     */
    ако(ptr && !*ptr) {
      ptr = Curl_get_content_encodings();
      ако(ptr)
        result = CURL_EASY_STR_SETN(data, STRING_ENCODING, ptr);
      инако
        result = CURLE_OUT_OF_MEMORY;
      врати result;
    }
    врати Curl_setstropt(data, STRING_ENCODING, ptr);

#ifndef CURL_DISABLE_AWS
  case CURLOPT_AWS_SIGV4:
    /*
     * String that is merged to some authentication
     * parameters are used by the algorithm.
     */
    result = Curl_setstropt(data, STRING_AWS_SIGV4, ptr);
    /*
     * Basic has been set by default; it needs to be unset here.
     */
    ако(CURL_EASY_STR(data, STRING_AWS_SIGV4))
      s->httpauth = CURLAUTH_AWS_SIGV4;
    инако
      s->httpauth &= ~(uint32_t)CURLAUTH_AWS_SIGV4;
    break;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  case CURLOPT_HTTPSIG_KEY:
    result = Curl_setstropt(data, STRING_HTTPSIG_KEY, ptr);
    break;
  case CURLOPT_HTTPSIG_KEYID:
    result = Curl_setstropt(data, STRING_HTTPSIG_KEYID, ptr);
    break;
  case CURLOPT_HTTPSIG_HEADERS:
    result = Curl_setstropt(data, STRING_HTTPSIG_HEADERS, ptr);
    break;
#endif
  case CURLOPT_REFERER: {
    /*
     * String to set in the HTTP Referer: field.
     */
    struct bufref *oldref = &data->state.referer;
    /* free the old after the storing the new in case the input is actually
       pointing back to this */
    result = Curl_setstropt(data, STRING_SET_REFERER, ptr);
    Curl_bufref_free(oldref);
    break;
  }

  case CURLOPT_USERAGENT:
    /*
     * String to use in the HTTP User-Agent field
     */
    врати Curl_setstropt(data, STRING_USERAGENT, ptr);

#ifndef CURL_DISABLE_COOKIES
  case CURLOPT_COOKIE:
    /*
     * Cookie string to send to the remote server in the request.
     */
    врати Curl_setstropt(data, STRING_COOKIE, ptr);

  case CURLOPT_COOKIEFILE:
    врати cookiefile(data, ptr);

  case CURLOPT_COOKIEJAR:
    /*
     * Set cookie filename to dump all cookies to when we are done.
     */
    result = Curl_setstropt(data, STRING_COOKIEJAR, ptr);
    ако(!result) {
      /*
       * Activate the cookie parser. This may or may not already
       * have been made.
       */
      ако(!data->cookies)
        data->cookies = Curl_cookie_init();
      ако(!data->cookies)
        result = CURLE_OUT_OF_MEMORY;
      инако
        data->state.cookie_engine = TRUE;
    }
    break;

  case CURLOPT_COOKIELIST:
    врати cookielist(data, ptr);
#endif /* !CURL_DISABLE_COOKIES */

#endif /* !CURL_DISABLE_HTTP */
  default:
    врати CURLE_UNKNOWN_OPTION;
  }
  врати result;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_MQTT */

#ifdef USE_SSH
static CURLcode setopt_cptr_ssh(struct Curl_easy *data, CURLoption option,
                                char *ptr)
{
  struct UserDefined *s = &data->set;
  switch(option) {
  case CURLOPT_SSH_PUBLIC_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa.pub file
     */
    врати Curl_setstropt(data, STRING_SSH_PUBLIC_KEY, ptr);
  case CURLOPT_SSH_PRIVATE_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa file
     */
    врати Curl_setstropt(data, STRING_SSH_PRIVATE_KEY, ptr);
  case CURLOPT_SSH_KEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_keyfunc_userp = ptr;
    break;
  case CURLOPT_SSH_HOST_PUBLIC_KEY_MD5:
    /*
     * Option to allow for the MD5 of the host public key to be checked
     * for validation purposes.
     */
    врати Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_KEY_MD5, ptr);
  case CURLOPT_SSH_HOST_PUBLIC_KEY_SHA256:
    /*
     * Option to allow for the SHA256 of the host public key to be checked
     * for validation purposes.
     */
    врати Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_KEY_SHA256, ptr);
  case CURLOPT_SSH_KNOWNHOSTS:
    /*
     * Store the filename to read known hosts from.
     */
    врати Curl_setstropt(data, STRING_SSH_KNOWNHOSTS, ptr);
#ifdef USE_LIBSSH2
  case CURLOPT_SSH_HOSTKEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_hostkeyfunc_userp = ptr;
    break;
#endif /* USE_LIBSSH2 */
  default:
    врати CURLE_UNKNOWN_OPTION;
  }
  врати CURLE_OK;
}
#endif /* USE_SSH */

#ifndef CURL_DISABLE_FTP
static CURLcode setopt_cptr_ftp(struct Curl_easy *data, CURLoption option,
                                char *ptr)
{
  CURLcode result = CURLE_OK;
  struct UserDefined *s = &data->set;
  switch(option) {
  case CURLOPT_FTPPORT:
    /*
     * Use FTP PORT, this also specifies which IP address to use
     */
    result = Curl_setstropt(data, STRING_FTPPORT, ptr);
    s->ftp_use_port = !!CURL_EASY_STR(data, STRING_FTPPORT);
    break;

  case CURLOPT_FTP_ACCOUNT:
    врати Curl_setstropt(data, STRING_FTP_ACCOUNT, ptr);

  case CURLOPT_FTP_ALTERNATIVE_TO_USER:
    врати Curl_setstropt(data, STRING_FTP_ALTERNATIVE_TO_USER, ptr);

  case CURLOPT_KRBLEVEL:
    врати CURLE_NOT_BUILT_IN; /* removed in 8.17.0 */
  case CURLOPT_CHUNK_DATA:
    s->wildcardptr = ptr;
    break;
  case CURLOPT_FNMATCH_DATA:
    s->fnmatch_data = ptr;
    break;
  default:
    врати CURLE_UNKNOWN_OPTION;
  }
  врати result;
}
#endif /* !CURL_DISABLE_FTP */

static CURLcode setopt_cptr_net(struct Curl_easy *data, CURLoption option,
                                char *ptr)
{
  switch(option) {
  case CURLOPT_INTERFACE:
    /*
     * Set what interface or address/hostname to bind the socket to when
     * performing an operation and thus what from-IP your connection will use.
     */
    врати setstropt_interface(data, ptr);

#ifdef USE_RESOLV_ARES
  case CURLOPT_DNS_SERVERS:
    врати Curl_setstropt(data, STRING_DNS_SERVERS, ptr);

  case CURLOPT_DNS_INTERFACE:
    врати Curl_setstropt(data, STRING_DNS_INTERFACE, ptr);

  case CURLOPT_DNS_LOCAL_IP4:
    врати Curl_setstropt(data, STRING_DNS_LOCAL_IP4, ptr);

  case CURLOPT_DNS_LOCAL_IP6:
    врати Curl_setstropt(data, STRING_DNS_LOCAL_IP6, ptr);
#endif
#ifdef USE_UNIX_SOCKETS
  case CURLOPT_UNIX_SOCKET_PATH:
    data->set.abstract_unix_socket = FALSE;
    врати Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);

  case CURLOPT_ABSTRACT_UNIX_SOCKET:
    data->set.abstract_unix_socket = TRUE;
    врати Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);
#endif
#ifndef CURL_DISABLE_DOH
  case CURLOPT_DOH_URL:
    {
      CURLcode result = Curl_setstropt(data, STRING_DOH, ptr);
      data->set.doh = !!CURL_EASY_STR(data, STRING_DOH);
      врати result;
    }
#endif
  default:
    врати CURLE_UNKNOWN_OPTION;
  }
}

static CURLcode setopt_cptr_misc(struct Curl_easy *data, CURLoption option,
                                 char *ptr)
{
  CURLcode result = CURLE_OK;
  struct UserDefined *s = &data->set;

  switch(option) {
  case CURLOPT_REQUEST_TARGET:
    врати Curl_setstropt(data, STRING_TARGET, ptr);
#ifndef CURL_DISABLE_NETRC
  case CURLOPT_NETRC_FILE:
    врати Curl_setstropt(data, STRING_NETRC_FILE, ptr);
#endif
  case CURLOPT_CUSTOMREQUEST:
    врати Curl_setstropt(data, STRING_CUSTOMREQUEST, ptr);

    /* we do not set s->method = HTTPREQ_CUSTOM; here, we continue as if we
       were using the already set type and this changes the actual request
       keyword */
  case CURLOPT_SERVICE_NAME:
    врати Curl_setstropt(data, STRING_SERVICE_NAME, ptr);

  case CURLOPT_HEADERDATA:
    s->writeheader = ptr;
    break;
  case CURLOPT_READDATA:
    s->in_set = ptr;
    break;
  case CURLOPT_WRITEDATA:
    s->out = ptr;
    break;
  case CURLOPT_DEBUGDATA:
    s->debugdata = ptr;
    break;
  case CURLOPT_PROGRESSDATA:
    s->progress_client = ptr;
    break;
  case CURLOPT_SEEKDATA:
    s->seek_client = ptr;
    break;
  case CURLOPT_IOCTLDATA:
    s->ioctl_client = ptr;
    break;
  case CURLOPT_SOCKOPTDATA:
    s->sockopt_client = ptr;
    break;
  case CURLOPT_OPENSOCKETDATA:
    s->opensocket_client = ptr;
    break;
  case CURLOPT_RESOLVER_START_DATA:
    s->resolver_start_client = ptr;
    break;
  case CURLOPT_CLOSESOCKETDATA:
    s->closesocket_client = ptr;
    break;
  case CURLOPT_PREREQDATA:
    s->prereq_userp = ptr;
    break;
  case CURLOPT_ERRORBUFFER:
    s->errorbuffer = ptr;
    break;
  case CURLOPT_URL:
    result = Curl_setstropt(data, STRING_SET_URL, ptr);
    Curl_bufref_set(&data->state.url,
                    CURL_EASY_STR(data, STRING_SET_URL), 0, NULL);
    break;

  case CURLOPT_USERPWD: {
    char *u = NULL, *p = NULL;
    result = setstropt_userpwd(ptr, &u, &p);
    ако(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_USERNAME, u);
      u = NULL;
    }
    ако(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_PASSWORD, p);
      p = NULL;
    }
    curlx_free(u);
    curlx_free(p);
    врати result;
  }

  case CURLOPT_USERNAME:
    врати Curl_setstropt(data, STRING_USERNAME, ptr);

  case CURLOPT_PASSWORD:
    врати Curl_setstropt(data, STRING_PASSWORD, ptr);

  case CURLOPT_LOGIN_OPTIONS:
    врати Curl_setstropt(data, STRING_OPTIONS, ptr);

  case CURLOPT_XOAUTH2_BEARER:
    врати Curl_setstropt(data, STRING_BEARER, ptr);
  case CURLOPT_RANGE:
    врати Curl_setstropt(data, STRING_SET_RANGE, ptr);
  case CURLOPT_PRIVATE:
    s->private_data = ptr;
    break;
  case CURLOPT_PROTOCOLS_STR:
    ако(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      ако(!result)
        s->allowed_protocols = protos;
    }
    инако
      /* make a NULL argument reset to default */
      s->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
    break;
  case CURLOPT_REDIR_PROTOCOLS_STR:
    ако(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      ако(!result)
        s->redir_protocols = protos;
    }
    инако
      /* make a NULL argument reset to default */
      s->redir_protocols = (curl_prot_t)CURLPROTO_REDIR;
    break;
  case CURLOPT_DEFAULT_PROTOCOL:
    /* Set the protocol to use when the URL does not include any protocol */
    врати Curl_setstropt(data, STRING_DEFAULT_PROTOCOL, ptr);
#ifndef CURL_DISABLE_SMTP
  case CURLOPT_MAIL_FROM:
    /* Set the SMTP mail originator */
    врати Curl_setstropt(data, STRING_MAIL_FROM, ptr);
  case CURLOPT_MAIL_AUTH:
    /* Set the SMTP auth originator */
    врати Curl_setstropt(data, STRING_MAIL_AUTH, ptr);
#endif
  case CURLOPT_SASL_AUTHZID:
    /* Authorization identity (identity to act as) */
    врати Curl_setstropt(data, STRING_SASL_AUTHZID, ptr);
#ifndef CURL_DISABLE_RTSP
  case CURLOPT_RTSP_SESSION_ID:
    Curl_peer_unlink(&data->state.rtsp_session_origin);
    врати Curl_setstropt(data, STRING_RTSP_SESSION_ID, ptr);
  case CURLOPT_RTSP_STREAM_URI:
    врати Curl_setstropt(data, STRING_RTSP_STREAM_URI, ptr);
  case CURLOPT_RTSP_TRANSPORT:
    врати Curl_setstropt(data, STRING_RTSP_TRANSPORT, ptr);
  case CURLOPT_INTERLEAVEDATA:
    s->rtp_out = ptr;
    break;
#endif /* !CURL_DISABLE_RTSP */
  case CURLOPT_TLSAUTH_USERNAME:
  case CURLOPT_TLSAUTH_PASSWORD:
  case CURLOPT_TLSAUTH_TYPE:
  case CURLOPT_PROXY_TLSAUTH_USERNAME:
  case CURLOPT_PROXY_TLSAUTH_PASSWORD:
  case CURLOPT_PROXY_TLSAUTH_TYPE:
    врати CURLE_NOT_BUILT_IN;
#ifndef CURL_DISABLE_HSTS
  case CURLOPT_HSTSREADDATA:
    s->hsts_read_userp = ptr;
    break;
  case CURLOPT_HSTSWRITEDATA:
    s->hsts_write_userp = ptr;
    break;
  case CURLOPT_HSTS: {
    struct curl_slist *h;
    ако(!data->hsts) {
      data->hsts = Curl_hsts_init();
      ако(!data->hsts)
        врати CURLE_OUT_OF_MEMORY;
    }
    ако(ptr) {
      result = Curl_setstropt(data, STRING_HSTS, ptr);
      ако(result)
        врати result;
      /* this needs to build a list of filenames to read from, so that it can
         read them later, as we might get a shared HSTS handle to load them
         into */
      h = curl_slist_append(data->state.hstslist, ptr);
      ако(!h) {
        curl_slist_free_all(data->state.hstslist);
        data->state.hstslist = NULL;
        врати CURLE_OUT_OF_MEMORY;
      }
      data->state.hstslist = h; /* store the list for later use */
    }
    инако {
      /* clear the list of HSTS files */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = NULL;
      ако(!data->share || !data->share->hsts)
        /* throw away the HSTS cache unless shared */
        Curl_hsts_cleanup(&data->hsts);
    }
    break;
  }
#endif /* !CURL_DISABLE_HSTS */
#ifndef CURL_DISABLE_ALTSVC
  case CURLOPT_ALTSVC:
    ако(!data->asi) {
      data->asi = Curl_altsvc_init();
      ако(!data->asi)
        врати CURLE_OUT_OF_MEMORY;
    }
    result = Curl_setstropt(data, STRING_ALTSVC, ptr);
    ако(result)
      break;
    ако(ptr)
      врати Curl_altsvc_load(data->asi, ptr);
    break;
#endif /* !CURL_DISABLE_ALTSVC */
  case CURLOPT_ECH:
    врати setopt_ech(data, ptr);
  default:
    врати CURLE_UNKNOWN_OPTION;
  }
  врати result;
}

static CURLcode setopt_cptr(struct Curl_easy *data, CURLoption option,
                            char *ptr)
{
  typedef CURLcode (*ptrfunc)(struct Curl_easy *data, CURLoption option,
                              char *ptr);
  /* Order by likeliness */
  static const ptrfunc setopt_call[] = {
    setopt_cptr_misc,
#ако defined(USE_SSL) || defined(USE_SSH)
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
#ако !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
    setopt_cptr_http_mqtt,
#endif
  };
  size_t i;

  за(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, ptr);
    ако(result != CURLE_UNKNOWN_OPTION)
      врати result;
  }
  врати CURLE_UNKNOWN_OPTION;
}

static CURLcode setopt_func(struct Curl_easy *data, CURLoption option,
                            va_list param)
{
  struct UserDefined *s = &data->set;
  switch(option) {
  case CURLOPT_PROGRESSFUNCTION:
    /*
     * Progress callback function
     */
    s->fprogress = va_arg(param, curl_progress_callback);
    ако(s->fprogress)
      data->progress.callback = TRUE; /* no longer internal */
    инако
      data->progress.callback = FALSE; /* NULL enforces internal */
    break;

  case CURLOPT_XFERINFOFUNCTION:
    /*
     * Transfer info callback function
     */
    s->fxferinfo = va_arg(param, curl_xferinfo_callback);
    ако(s->fxferinfo)
      data->progress.callback = TRUE; /* no longer internal */
    инако
      data->progress.callback = FALSE; /* NULL enforces internal */

    break;
  case CURLOPT_DEBUGFUNCTION:
    /*
     * stderr write callback.
     */
    s->fdebug = va_arg(param, curl_debug_callback);
    /*
     * if the callback provided is NULL, it will use the default callback
     */
    break;
  case CURLOPT_HEADERFUNCTION:
    /*
     * Set header write callback
     */
    s->fwrite_header = va_arg(param, curl_write_callback);
    break;
  case CURLOPT_WRITEFUNCTION:
    /*
     * Set data write callback
     */
    s->fwrite_func = va_arg(param, curl_write_callback);
    ако(!s->fwrite_func)
#ако defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fwrite_func = (curl_write_callback)fwrite;
#ако defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic pop
#endif
    break;
  case CURLOPT_READFUNCTION:
    /*
     * Read data callback
     */
    s->fread_func_set = va_arg(param, curl_read_callback);
    ако(!s->fread_func_set) {
      s->is_fread_set = 0;
#ако defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fread_func_set = (curl_read_callback)fread;
#ако defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic pop
#endif
    }
    инако
      s->is_fread_set = 1;
    break;
  case CURLOPT_SEEKFUNCTION:
    /*
     * Seek callback. Might be NULL.
     */
    s->seek_func = va_arg(param, curl_seek_callback);
    break;
  case CURLOPT_IOCTLFUNCTION:
    /*
     * I/O control callback. Might be NULL.
     */
    s->ioctl_func = va_arg(param, curl_ioctl_callback);
    break;
  case CURLOPT_SSL_CTX_FUNCTION:
    /*
     * Set an SSL_CTX callback
     */
#ifdef USE_SSL
    ако(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      s->ssl_fsslctx = va_arg(param, curl_ssl_ctx_callback);
      break;
    }
    инако
#endif
      врати CURLE_NOT_BUILT_IN;

  case CURLOPT_SOCKOPTFUNCTION:
    /*
     * socket callback function: called after socket() but before connect()
     */
    s->fsockopt = va_arg(param, curl_sockopt_callback);
    break;

  case CURLOPT_OPENSOCKETFUNCTION:
    /*
     * open/create socket callback function: called instead of socket(),
     * before connect()
     */
    s->fopensocket = va_arg(param, curl_opensocket_callback);
    break;

  case CURLOPT_CLOSESOCKETFUNCTION:
    /*
     * close socket callback function: called instead of close()
     * when shutting down a connection
     */
    s->fclosesocket = va_arg(param, curl_closesocket_callback);
    break;

  case CURLOPT_RESOLVER_START_FUNCTION:
    /*
     * resolver start callback function: called before a new resolver request
     * is started
     */
    s->resolver_start = va_arg(param, curl_resolver_start_callback);
    break;

#ifdef USE_SSH
#ifdef USE_LIBSSH2
  case CURLOPT_SSH_HOSTKEYFUNCTION:
    /* the callback to check the hostkey without the knownhost file */
    s->ssh_hostkeyfunc = va_arg(param, curl_sshhostkeycallback);
    break;
#endif

  case CURLOPT_SSH_KEYFUNCTION:
    /* setting to NULL is fine since the ssh.c functions themselves will
       then revert to use the internal default */
    s->ssh_keyfunc = va_arg(param, curl_sshkeycallback);
    break;

#endif /* USE_SSH */

#ifndef CURL_DISABLE_RTSP
  case CURLOPT_INTERLEAVEFUNCTION:
    /* Set the user defined RTP write function */
    s->fwrite_rtp = va_arg(param, curl_write_callback);
    break;
#endif
#ifndef CURL_DISABLE_FTP
  case CURLOPT_CHUNK_BGN_FUNCTION:
    s->chunk_bgn = va_arg(param, curl_chunk_bgn_callback);
    break;
  case CURLOPT_CHUNK_END_FUNCTION:
    s->chunk_end = va_arg(param, curl_chunk_end_callback);
    break;
  case CURLOPT_FNMATCH_FUNCTION:
    s->fnmatch = va_arg(param, curl_fnmatch_callback);
    break;
#endif
#ifndef CURL_DISABLE_HTTP
  case CURLOPT_TRAILERFUNCTION:
    s->trailer_callback = va_arg(param, curl_trailer_callback);
    break;
#endif
#ifndef CURL_DISABLE_HSTS
  case CURLOPT_HSTSREADFUNCTION:
    s->hsts_read = va_arg(param, curl_hstsread_callback);
    break;
  case CURLOPT_HSTSWRITEFUNCTION:
    s->hsts_write = va_arg(param, curl_hstswrite_callback);
    break;
#endif
  case CURLOPT_PREREQFUNCTION:
    s->fprereq = va_arg(param, curl_prereq_callback);
    break;
  default:
    врати CURLE_UNKNOWN_OPTION;
  }
  врати CURLE_OK;
}

static CURLcode setopt_offt(struct Curl_easy *data, CURLoption option,
                            curl_off_t offt)
{
  struct UserDefined *s = &data->set;
  switch(option) {
  case CURLOPT_TIMEVALUE_LARGE:
    /*
     * This is the value to compare with the remote document with the
     * method set with CURLOPT_TIMECONDITION
     */
    s->timevalue = (time_t)offt;
    break;

    /* MQTT "borrows" some of the HTTP options */
  case CURLOPT_POSTFIELDSIZE_LARGE:
    /*
     * The size of the POSTFIELD data to prevent libcurl to do strlen() to
     * figure it out. Enables binary posts.
     */
    ако(offt < -1)
      врати CURLE_BAD_FUNCTION_ARGUMENT;

    ако(s->postfieldsize < offt && s->str_copypostfields) {
      /* Previous CURLOPT_COPYPOSTFIELDS is no longer valid. */
      curlx_safefree(s->str_copypostfields);
      s->postfields = NULL;
    }
    s->postfieldsize = offt;
    break;
  case CURLOPT_INFILESIZE_LARGE:
    /*
     * If known, this should inform curl about the file size of the
     * to-be-uploaded file.
     */
    ако(offt < -1)
      врати CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = offt;
    break;
  case CURLOPT_MAX_SEND_SPEED_LARGE:
    /*
     * When transfer uploads are faster than CURLOPT_MAX_SEND_SPEED_LARGE
     * bytes per second the transfer is throttled..
     */
    ако(offt < 0)
      врати CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_send_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.ul.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    break;
  case CURLOPT_MAX_RECV_SPEED_LARGE:
    /*
     * When receiving data faster than CURLOPT_MAX_RECV_SPEED_LARGE bytes per
     * second the transfer is throttled..
     */
    ако(offt < 0)
      врати CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_recv_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.dl.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    break;
  case CURLOPT_RESUME_FROM_LARGE:
    /*
     * Resume transfer at the given file position
     */
    ако(offt < -1)
      врати CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = offt;
    break;
  case CURLOPT_MAXFILESIZE_LARGE:
    /*
     * Set the maximum size of a file to download.
     */
    ако(offt < 0)
      врати CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_filesize = offt;
    break;

  default:
    врати CURLE_UNKNOWN_OPTION;
  }
  врати CURLE_OK;
}

static CURLcode setopt_blob(struct Curl_easy *data, CURLoption option,
                            struct curl_blob *blob)
{
  struct UserDefined *s = &data->set;
  switch(option) {
  case CURLOPT_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use
     */
    врати Curl_setblobopt(&s->blobs[BLOB_CERT], blob);
#ifndef CURL_DISABLE_PROXY
  case CURLOPT_PROXY_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use for proxy
     */
    врати Curl_setblobopt(&s->blobs[BLOB_CERT_PROXY], blob);
  case CURLOPT_PROXY_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use for proxy
     */
    врати Curl_setblobopt(&s->blobs[BLOB_KEY_PROXY], blob);
  case CURLOPT_PROXY_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection proxy.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    ако(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO_PROXY], blob);
      ако(!result)
        s->proxy_ssl.custom_cablob = TRUE;
      врати result;
    }
#endif
    врати CURLE_NOT_BUILT_IN;
  case CURLOPT_PROXY_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    ако(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      врати Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT_PROXY], blob);
    врати CURLE_NOT_BUILT_IN;
#endif
  case CURLOPT_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use
     */
    врати Curl_setblobopt(&s->blobs[BLOB_KEY], blob);
  case CURLOPT_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    ако(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO], blob);
      ако(!result)
        s->ssl.custom_cablob = TRUE;
      врати result;
    }
#endif
    врати CURLE_NOT_BUILT_IN;
  case CURLOPT_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    ако(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      врати Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT], blob);
    врати CURLE_NOT_BUILT_IN;

  default:
    врати CURLE_UNKNOWN_OPTION;
  }
  /* unreachable */
}

/*
 * Do not make Curl_vsetopt() static: it is called from
 * projects/OS400/ccsidcurl.c.
 */
CURLcode Curl_vsetopt(struct Curl_easy *data, CURLoption option, va_list param)
{
  ако(option < CURLOPTTYPE_OBJECTPOINT)
    врати setopt_long(data, option, va_arg(param, long));
  инако ако(option < CURLOPTTYPE_FUNCTIONPOINT) {
    /* unfortunately, different pointer types cannot be identified any other
       way than being listed explicitly */
    switch(option) {
    case CURLOPT_HTTPHEADER:
    case CURLOPT_QUOTE:
    case CURLOPT_POSTQUOTE:
    case CURLOPT_TELNETOPTIONS:
    case CURLOPT_PREQUOTE:
    case CURLOPT_HTTP200ALIASES:
    case CURLOPT_MAIL_RCPT:
    case CURLOPT_RESOLVE:
    case CURLOPT_PROXYHEADER:
    case CURLOPT_CONNECT_TO:
      врати setopt_slist(data, option, va_arg(param, struct curl_slist *));
    case CURLOPT_HTTPPOST:         /* curl_httppost * */
    case CURLOPT_MIMEPOST:         /* curl_mime * */
    case CURLOPT_STDERR:           /* FILE * */
    case CURLOPT_SHARE:            /* CURLSH * */
    case CURLOPT_CURLU:            /* CURLU * */
      врати setopt_pointers(data, option, param);
    case CURLOPT_STREAM_DEPENDS:   /* CURL * */
    case CURLOPT_STREAM_DEPENDS_E: /* CURL * */
      врати CURLE_OK;
    default:
      break;
    }
    /* the char pointer options */
    врати setopt_cptr(data, option, va_arg(param, char *));
  }
  инако ако(option < CURLOPTTYPE_OFF_T)
    врати setopt_func(data, option, param);
  инако ако(option < CURLOPTTYPE_BLOB)
    врати setopt_offt(data, option, va_arg(param, curl_off_t));
  врати setopt_blob(data, option, va_arg(param, struct curl_blob *));
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
  struct Curl_eapi_guard guard;
  CURLcode result;

  ако(CURL_EAPI_ENTER(&guard, curl, easy_setopt, &result)) {
    struct Curl_easy *data = curl;
    va_list arg;

    va_start(arg, option);

    result = Curl_vsetopt(data, option, arg);

    va_end(arg);
    ако(result == CURLE_BAD_FUNCTION_ARGUMENT)
      failf(data, "setopt 0x%x got bad argument", (unsigned цео)option);
  }
  CURL_EAPI_LEAVE(&guard);
  врати result;
}
