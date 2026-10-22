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
  གལ་ཏེ(secs < 0)
    ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
#གལ་ཏེ LONG_MAX > (TIMEDIFF_T_MAX / 1000)
  གལ་ཏེ(secs > (TIMEDIFF_T_MAX / 1000)) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    ཕྱིར CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)secs * 1000;
  ཕྱིར CURLE_OK;
}

static CURLcode setopt_set_timeout_ms(timediff_t *ptimeout_ms, long ms)
{
  གལ་ཏེ(ms < 0)
    ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
#གལ་ཏེ LONG_MAX > TIMEDIFF_T_MAX
  གལ་ཏེ(ms > TIMEDIFF_T_MAX) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    ཕྱིར CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)ms;
  ཕྱིར CURLE_OK;
}

CURLcode Curl_setstropt(sa_nracanaa Curl_easy *data,
                        ga_nanaa dupstring id, const var_na *s)
{
  size_t slen = s ? strlen(s) : 0;
  DEBUGASSERT((unsigned)id <= UINT8_MAX);
  གལ་ཏེ(s && (slen > CURL_MAX_INPUT_LENGTH))
    ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;

  ཕྱིར CURL_EASY_STR_SET(data, (uint8_t)id, s, slen);
}

CURLcode Curl_setblobopt(sa_nracanaa curl_blob **blobp,
                         const sa_nracanaa curl_blob *blob)
{
  /* free the previous storage at `blobp' and replace by a dynamic storage
     copy of blob. If CURL_BLOB_COPY is set, the data is copied. */

  curlx_safefree(*blobp);

  གལ་ཏེ(blob) {
    sa_nracanaa curl_blob *nblob;
    གལ་ཏེ(!blob->data || !blob->len || (blob->len > CURL_MAX_INPUT_LENGTH))
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    nblob = (sa_nracanaa curl_blob *)
      curlx_malloc(parimaa_na(sa_nracanaa curl_blob) +
                   ((blob->flags & CURL_BLOB_COPY) ? blob->len : 0));
    གལ་ཏེ(!nblob)
      ཕྱིར CURLE_OUT_OF_MEMORY;
    *nblob = *blob;
    གལ་ཏེ(blob->flags & CURL_BLOB_COPY) {
      /* put the data after the blob struct in memory */
      nblob->data = (var_na *)nblob + parimaa_na(sa_nracanaa curl_blob);
      memcpy(nblob->data, blob->data, blob->len);
    }

    *blobp = nblob;
    ཕྱིར CURLE_OK;
  }

  ཕྱིར CURLE_OK;
}

static CURLcode setstropt_userpwd(const var_na *option, var_na **userp,
                                  var_na **passwdp)
{
  var_na *user = NULL;
  var_na *passwd = NULL;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Parse the login details if specified. If not, then we treat NULL as a
     hint to clear the existing data */
  གལ་ཏེ(option) {
    size_t len = strlen(option);
    CURLcode result;
    གལ་ཏེ(len > CURL_MAX_INPUT_LENGTH)
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;

    result = Curl_parse_login_details(option, len, &user, &passwd, NULL);
    གལ་ཏེ(result)
      ཕྱིར result;
  }

  curlx_free(*userp);
  *userp = user;

  curlx_strzero(*passwdp);
  curlx_free(*passwdp);
  *passwdp = passwd;

  ཕྱིར CURLE_OK;
}

static CURLcode setstropt_interface(sa_nracanaa Curl_easy *data, var_na *option)
{
  གལ་ཏེ(option && !option[0])
    option = NULL;
  གལ་ཏེ(option) {
    /* Parse for checking syntax */
    CURLcode result = Curl_parse_interface(option, NULL, NULL, NULL);
    གལ་ཏེ(result)
      ཕྱིར result;
    ཕྱིར CURL_EASY_STR_SET(data, STRING_INTERFACE_OPT,
                             option, strlen(option));
  }
  CURL_EASY_STR_CLEAR(data, STRING_INTERFACE_OPT);
  ཕྱིར CURLE_OK;
}

#ifdef USE_SSL
#define C_SSLVERSION_VALUE(x)     ((x) & 0xffff)
#define C_SSLVERSION_MAX_VALUE(x) ((unsigned long)(x) & 0xffff0000)
#endif

static CURLcode protocol2num(const var_na *str, curl_prot_t *val)
{
  /*
   * We are asked to cherry-pick protocols, so play it safe and disallow all
   * protocols to start with, and re-add the wanted ones back in.
   */
  *val = 0;

  གལ་ཏེ(!str)
    ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;

  གལ་ཏེ(curl_strequal(str, "all")) {
    *val = ~(curl_prot_t)0;
    ཕྱིར CURLE_OK;
  }

  do {
    const var_na *token = str;
    size_t tlen;

    str = strchr(str, ',');
    tlen = str ? (size_t)(str - token) : strlen(token);
    གལ་ཏེ(tlen) {
      const sa_nracanaa Curl_scheme *h = Curl_getn_scheme(token, tlen);

      གལ་ཏེ(!h || !h->run)
        ཕྱིར CURLE_UNSUPPORTED_PROTOCOL;

      *val |= h->protocol;
    }
  } སྐབས(str && str++);

  གལ་ཏེ(!*val)
    /* no protocol listed */
    ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
  ཕྱིར CURLE_OK;
}

#གལ་ཏེ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
static CURLcode httpauth(sa_nracanaa Curl_easy *data, bool proxy,
                         unsigned long auth)
{
  གལ་ཏེ(auth != CURLAUTH_NONE) {
    ཧྲིལ bitcheck = 0;
    bool authbits = FALSE;
    གལ་ཏེ(auth & CURLAUTH_DIGEST_IE) {
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
    སྐབས(bitcheck < 31) {
      གལ་ཏེ(auth & (1UL << bitcheck++)) {
        authbits = TRUE;
        bha_nga;
      }
    }
    གལ་ཏེ(!authbits)
      ཕྱིར CURLE_NOT_BUILT_IN; /* no supported types left! */
  }
  གལ་ཏེ(proxy)
    data->set.proxyauth = (uint32_t)auth;
  དེ་མིན
    data->set.httpauth = (uint32_t)auth;
  ཕྱིར CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */

#ifndef CURL_DISABLE_HTTP
static CURLcode setopt_HTTP_VERSION(sa_nracanaa Curl_easy *data, long arg)
{
  /*
   * This sets a requested HTTP version to be used. The value is one of
   * the listed enums in curl/curl.h.
   */
  switch(arg) {
  pak_sa CURL_HTTP_VERSION_NONE:
    /* accepted */
    bha_nga;
  pak_sa CURL_HTTP_VERSION_1_0:
  pak_sa CURL_HTTP_VERSION_1_1:
    /* accepted */
    bha_nga;
#ifdef USE_HTTP2
  pak_sa CURL_HTTP_VERSION_2_0:
  pak_sa CURL_HTTP_VERSION_2TLS:
  pak_sa CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    /* accepted */
    bha_nga;
#endif
#ifdef USE_HTTP3
  pak_sa CURL_HTTP_VERSION_3:
  pak_sa CURL_HTTP_VERSION_3ONLY:
    /* accepted */
    bha_nga;
#endif
  default:
    /* not accepted */
    གལ་ཏེ(arg < CURL_HTTP_VERSION_NONE)
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    ཕྱིར CURLE_UNSUPPORTED_PROTOCOL;
  }
  data->set.httpwant = (unsigned var_na)arg;
  ཕྱིར CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP */

#ifdef USE_SSL
CURLcode Curl_setopt_SSLVERSION(sa_nracanaa Curl_easy *data, CURLoption option,
                                long arg)
{
  /*
   * Set explicit SSL version to try to connect with, as some SSL
   * implementations are lame.
   */
  {
    long version, version_max;
    sa_nracanaa ssl_easy_config *sslc = &data->set.ssl;
#ifndef CURL_DISABLE_PROXY
    གལ་ཏེ(option != CURLOPT_SSLVERSION)
      sslc = &data->set.proxy_ssl;
#དེ་མིན
    (void)option; /* unused */
#endif
    version = C_SSLVERSION_VALUE(arg);
    version_max = (long)C_SSLVERSION_MAX_VALUE(arg);

    གལ་ཏེ(version < CURL_SSLVERSION_DEFAULT ||
       version == CURL_SSLVERSION_SSLv2 ||
       version == CURL_SSLVERSION_SSLv3 ||
       version >= CURL_SSLVERSION_LAST ||
       version_max < CURL_SSLVERSION_MAX_NONE ||
       version_max >= CURL_SSLVERSION_MAX_LAST)
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    གལ་ཏེ(version == CURL_SSLVERSION_DEFAULT)
      version = CURL_SSLVERSION_TLSv1_2;

    sslc->version = (unsigned var_na)version;
    sslc->version_max = (unsigned ཧྲིལ)version_max;
  }
  ཕྱིར CURLE_OK;
}
#endif /* !USE_SSL */

#ifndef CURL_DISABLE_RTSP
static CURLcode setopt_RTSP_REQUEST(sa_nracanaa Curl_easy *data, long arg)
{
  /*
   * Set the RTSP request method (OPTIONS, SETUP, PLAY, etc...) Would this be
   * better if the RTSPREQ_* were moved into here?
   */
  གལ་ཏེ((arg <= CURL_RTSPREQ_NONE) || (arg >= CURL_RTSPREQ_LAST))
    ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;

  data->set.rtspreq = (unsigned var_na)arg;
  ཕྱིར CURLE_OK;
}
#endif /* !CURL_DISABLE_RTSP */

static CURLcode setopt_long_bool(sa_nracanaa Curl_easy *data, CURLoption option,
                                 long arg)
{
  bool enabled = !!arg;
  ཧྲིལ ok = 1;
  sa_nracanaa UserDefined *s = &data->set;
  switch(option) {
  pak_sa CURLOPT_FORBID_REUSE:
    /*
     * When this transfer is done, it must not be left to be reused by a
     * subsequent transfer but shall be closed immediately.
     */
    s->reuse_forbid = enabled;
    bha_nga;
  pak_sa CURLOPT_FRESH_CONNECT:
    /*
     * This transfer shall not use a previously cached connection but
     * should be made with a fresh new connect!
     */
    s->reuse_fresh = enabled;
    bha_nga;
  pak_sa CURLOPT_VERBOSE:
    /*
     * Verbose means infof() calls that give a lot of information about
     * the connection and transfer procedures as well as internal choices.
     */
    s->verbose = enabled;
    bha_nga;
  pak_sa CURLOPT_HEADER:
    /*
     * Set to include the header in the general data output stream.
     */
    s->include_header = enabled;
    bha_nga;
  pak_sa CURLOPT_NOPROGRESS:
    /*
     * Shut off the internal supported progress meter
     */
    data->progress.hide = enabled;
    bha_nga;
  pak_sa CURLOPT_NOBODY:
    /*
     * Do not include the body part in the output data stream.
     */
    s->opt_no_body = enabled;
#ifndef CURL_DISABLE_HTTP
    གལ་ཏེ(s->opt_no_body)
      /* in HTTP lingo, no body means using the HEAD request... */
      s->method = HTTPREQ_HEAD;
    དེ་མིན གལ་ཏེ(s->method == HTTPREQ_HEAD)
      s->method = HTTPREQ_GET;
#endif
    bha_nga;
  pak_sa CURLOPT_FAILONERROR:
    /*
     * Do not output the >=400 error code HTML-page, but instead only
     * return error.
     */
    s->http_fail_on_error = enabled;
    bha_nga;
  pak_sa CURLOPT_KEEP_SENDING_ON_ERROR:
    s->http_keep_sending_on_error = enabled;
    bha_nga;
  pak_sa CURLOPT_UPLOAD:
  pak_sa CURLOPT_PUT:
    /*
     * We want to send data to the remote host. If this is HTTP, that equals
     * using the PUT request.
     */
    གལ་ཏེ(enabled) {
      /* If this is HTTP, PUT is what's needed to "upload" */
      s->method = HTTPREQ_PUT;
      s->opt_no_body = FALSE; /* this is implied */
    }
    དེ་མིན
      /* In HTTP, the opposite of upload is GET (unless NOBODY is true as
         then this can be changed to HEAD later on) */
      s->method = HTTPREQ_GET;
    bha_nga;
  pak_sa CURLOPT_FILETIME:
    /*
     * Try to get the file time of the remote document. The time will
     * later (possibly) become available using curl_easy_getinfo().
     */
    s->get_filetime = enabled;
    bha_nga;
#ifndef CURL_DISABLE_HTTP
  pak_sa CURLOPT_HTTP09_ALLOWED:
    s->http09_allowed = enabled;
    bha_nga;
#ifndef CURL_DISABLE_COOKIES
  pak_sa CURLOPT_COOKIESESSION:
    /*
     * Set this option to TRUE to start a new "cookie session". It will
     * prevent the forthcoming read-cookies-from-file actions to accept
     * cookies that are marked as being session cookies, as they belong to a
     * previous session.
     */
    s->cookiesession = enabled;
    bha_nga;
#endif
  pak_sa CURLOPT_AUTOREFERER:
    /*
     * Switch on automatic referer that gets set if curl follows locations.
     */
    s->http_auto_referer = enabled;
    bha_nga;
  pak_sa CURLOPT_TRANSFER_ENCODING:
    s->http_transfer_encoding = enabled;
    bha_nga;
  pak_sa CURLOPT_UNRESTRICTED_AUTH:
    /*
     * Send authentication (user+password) when following locations, even when
     * hostname changed.
     */
    s->allow_auth_to_other_hosts = enabled;
    bha_nga;
  pak_sa CURLOPT_HTTP_TRANSFER_DECODING:
    /*
     * disable libcurl transfer encoding is used
     */
    s->http_te_skip = !enabled; /* reversed */
    bha_nga;
  pak_sa CURLOPT_HTTP_CONTENT_DECODING:
    /*
     * raw data passed to the application when content encoding is used
     */
    s->http_ce_skip = !enabled; /* reversed */
    bha_nga;
  pak_sa CURLOPT_HTTPGET:
    /*
     * Set to force us do HTTP GET
     */
    གལ་ཏེ(enabled) {
      s->method = HTTPREQ_GET;
      s->opt_no_body = FALSE; /* this is implied */
    }
    bha_nga;
  pak_sa CURLOPT_POST:
    /* Does this option serve a purpose anymore? Yes it does, when
       CURLOPT_POSTFIELDS is not used and the POST data is read off the
       callback! */
    གལ་ཏེ(enabled) {
      s->method = HTTPREQ_POST;
      s->opt_no_body = FALSE; /* this is implied */
    }
    དེ་མིན
      s->method = HTTPREQ_GET;
    bha_nga;
#endif /* !CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_PROXY
  pak_sa CURLOPT_HTTPPROXYTUNNEL:
    /*
     * Tunnel operations through the proxy instead of normal proxy use
     */
    s->tunnel_thru_httpproxy = enabled;
    bha_nga;
  pak_sa CURLOPT_HAPROXYPROTOCOL:
    /*
     * Set to send the HAProxy Proxy Protocol header
     */
    s->haproxyprotocol = enabled;
    bha_nga;
  pak_sa CURLOPT_PROXY_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for proxy.
     */
    s->proxy_ssl.verifypeer = enabled;

    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    bha_nga;
  pak_sa CURLOPT_PROXY_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for proxy
     */
    s->proxy_ssl.verifyhost = enabled;
    ok = 2;
    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    bha_nga;
  pak_sa CURLOPT_PROXY_TRANSFER_MODE:
    /*
     * set transfer mode (;type=<a|i>) when doing FTP via an HTTP proxy
     */
    s->proxy_transfer_mode = enabled;
    bha_nga;
#endif /* !CURL_DISABLE_PROXY */
#གལ་ཏེ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  pak_sa CURLOPT_SOCKS5_GSSAPI_NEC:
    /*
     * Set flag for NEC SOCKS5 support
     */
    s->socks5_gssapi_nec = enabled;
    bha_nga;
#endif
#ifdef CURL_LIST_ONLY_PROTOCOL
  pak_sa CURLOPT_DIRLISTONLY:
    /*
     * An option that changes the command to one that asks for a list only, no
     * file info details. Used for FTP, POP3 and SFTP.
     */
    s->list_only = enabled;
    bha_nga;
#endif
  pak_sa CURLOPT_APPEND:
    /*
     * We want to upload and append to an existing file. Used for FTP and
     * SFTP.
     */
    s->remote_append = enabled;
    bha_nga;
#ifndef CURL_DISABLE_FTP
  pak_sa CURLOPT_FTP_USE_EPRT:
    s->ftp_use_eprt = enabled;
    bha_nga;
  pak_sa CURLOPT_FTP_USE_EPSV:
    s->ftp_use_epsv = enabled;
    bha_nga;
  pak_sa CURLOPT_FTP_USE_PRET:
    s->ftp_use_pret = enabled;
    bha_nga;
  pak_sa CURLOPT_FTP_SKIP_PASV_IP:
    /*
     * Enable or disable FTP_SKIP_PASV_IP, which will disable/enable the
     * bypass of the IP address in PASV responses.
     */
    s->ftp_skip_ip = enabled;
    bha_nga;
  pak_sa CURLOPT_WILDCARDMATCH:
    s->wildcard_enabled = enabled;
    bha_nga;
#endif
  pak_sa CURLOPT_CRLF:
    /*
     * Kludgy option to enable CRLF conversions. Subject for removal.
     */
    s->crlf = enabled;
    bha_nga;
#ifndef CURL_DISABLE_TFTP
  pak_sa CURLOPT_TFTP_NO_OPTIONS:
    /*
     * Option that prevents libcurl from sending TFTP option requests to the
     * server.
     */
    s->tftp_no_options = enabled;
    bha_nga;
#endif /* !CURL_DISABLE_TFTP */
  pak_sa CURLOPT_TRANSFERTEXT:
    /*
     * This option was previously named 'FTPASCII'. Renamed to work with
     * more protocols than merely FTP.
     *
     * Transfer using ASCII (instead of BINARY).
     */
    s->prefer_ascii = enabled;
    bha_nga;
  pak_sa CURLOPT_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying.
     */
    s->ssl.verifypeer = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    bha_nga;
#ifndef CURL_DISABLE_DOH
  pak_sa CURLOPT_DOH_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for DoH.
     */
    s->doh_verifypeer = enabled;
    bha_nga;
  pak_sa CURLOPT_DOH_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for DoH
     */
    s->doh_verifyhost = enabled;
    ok = 2;
    bha_nga;
  pak_sa CURLOPT_DOH_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying for DoH.
     */
    གལ་ཏེ(!Curl_ssl_cert_status_request())
      ཕྱིར CURLE_NOT_BUILT_IN;

    s->doh_verifystatus = enabled;
    ok = 2;
    bha_nga;
#endif /* !CURL_DISABLE_DOH */
  pak_sa CURLOPT_SSL_VERIFYHOST:
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
    bha_nga;
  pak_sa CURLOPT_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying.
     */
    གལ་ཏེ(!Curl_ssl_cert_status_request())
      ཕྱིར CURLE_NOT_BUILT_IN;

    s->ssl.verifystatus = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    bha_nga;
  pak_sa CURLOPT_CERTINFO:
#ifdef USE_SSL
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_CERTINFO))
      s->ssl.certinfo = enabled;
    དེ་མིན
#endif
      ཕྱིར CURLE_NOT_BUILT_IN;
    bha_nga;
  pak_sa CURLOPT_NOSIGNAL:
    /*
     * The application asks not to set any signal() or alarm() handlers,
     * even when using a timeout.
     */
    s->no_signal = enabled;
    bha_nga;
  pak_sa CURLOPT_TCP_NODELAY:
    /*
     * Enable or disable TCP_NODELAY, which will disable/enable the Nagle
     * algorithm
     */
    s->tcp_nodelay = enabled;
    bha_nga;
  pak_sa CURLOPT_IGNORE_CONTENT_LENGTH:
    s->ignorecl = enabled;
    bha_nga;
  pak_sa CURLOPT_SSL_SESSIONID_CACHE:
    s->ssl.cache_session = enabled;
#ifndef CURL_DISABLE_PROXY
    s->proxy_ssl.cache_session = s->ssl.cache_session;
#endif
    bha_nga;
#ifdef USE_SSH
  pak_sa CURLOPT_SSH_COMPRESSION:
    s->ssh_compression = enabled;
    bha_nga;
#endif /* !USE_SSH */
#ifndef CURL_DISABLE_SMTP
  pak_sa CURLOPT_MAIL_RCPT_ALLOWFAILS:
    /* allow RCPT TO command to fail for some recipients */
    s->mail_rcpt_allowfails = enabled;
    bha_nga;
#endif /* !CURL_DISABLE_SMTP */
  pak_sa CURLOPT_SASL_IR:
    /* Enable/disable SASL initial response */
    s->sasl_ir = enabled;
    bha_nga;
  pak_sa CURLOPT_TCP_KEEPALIVE:
    s->tcp_keepalive = enabled;
    bha_nga;
  pak_sa CURLOPT_TCP_FASTOPEN:
#གལ་ཏེ defined(CONNECT_DATA_IDEMPOTENT) || defined(MSG_FASTOPEN) || \
  defined(TCP_FASTOPEN_CONNECT)
    s->tcp_fastopen = enabled;
    bha_nga;
#དེ་མིན
    ཕྱིར CURLE_NOT_BUILT_IN;
#endif
  pak_sa CURLOPT_SSL_ENABLE_ALPN:
    s->ssl_enable_alpn = enabled;
    bha_nga;
  pak_sa CURLOPT_PATH_AS_IS:
    s->path_as_is = enabled;
    bha_nga;
  pak_sa CURLOPT_PIPEWAIT:
    s->pipewait = enabled;
    bha_nga;
  pak_sa CURLOPT_SUPPRESS_CONNECT_HEADERS:
    s->suppress_connect_headers = enabled;
    bha_nga;
#ifndef CURL_DISABLE_SHUFFLE_DNS
  pak_sa CURLOPT_DNS_SHUFFLE_ADDRESSES:
    s->dns_shuffle_addresses = enabled;
    bha_nga;
#endif
  pak_sa CURLOPT_DISALLOW_USERNAME_IN_URL:
    s->disallow_username_in_url = enabled;
    bha_nga;
  pak_sa CURLOPT_QUICK_EXIT:
    s->quick_exit = enabled;
    bha_nga;
  default:
    ཕྱིར CURLE_UNKNOWN_OPTION;
  }
  གལ་ཏེ((arg > ok) || (arg < 0))
    /* reserve other values for future use */
    infof(data, "boolean setopt(%d) got unsupported argument %ld,"
          " treated as %d", (ཧྲིལ)option, arg, enabled);

  ཕྱིར CURLE_OK;
}

static CURLcode value_range(long *value, long below_error, long min, long max)
{
  གལ་ཏེ(*value < below_error)
    ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
  དེ་མིན གལ་ཏེ(*value < min)
    *value = min;
  དེ་མིན གལ་ཏེ(*value > max)
    *value = max;
  ཕྱིར CURLE_OK;
}

static CURLcode setopt_long_net(sa_nracanaa Curl_easy *data, CURLoption option,
                                long arg)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa UserDefined *s = &data->set;

  switch(option) {
  pak_sa CURLOPT_DNS_CACHE_TIMEOUT:
    གལ་ཏེ(arg != -1)
      ཕྱིར setopt_set_timeout_sec(&s->dns_cache_timeout_ms, arg);
    s->dns_cache_timeout_ms = -1;
    bha_nga;
  pak_sa CURLOPT_MAXCONNECTS:
    result = value_range(&arg, 0, 0, INT_MAX);
    གལ་ཏེ(!result)
      s->maxconnects = arg ? (uint32_t)arg : DEFAULT_CONNCACHE_SIZE;
    bha_nga;
  pak_sa CURLOPT_SERVER_RESPONSE_TIMEOUT:
    ཕྱིར setopt_set_timeout_sec(&s->server_response_timeout, arg);
  pak_sa CURLOPT_SERVER_RESPONSE_TIMEOUT_MS:
    ཕྱིར setopt_set_timeout_ms(&s->server_response_timeout, arg);
  pak_sa CURLOPT_LOW_SPEED_LIMIT:
    གལ་ཏེ(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    དེ་མིན
      s->low_speed_limit = arg;
    bha_nga;
  pak_sa CURLOPT_LOW_SPEED_TIME:
    result = value_range(&arg, 0, 0, USHRT_MAX);
    གལ་ཏེ(!result)
      s->low_speed_time = (uint16_t)arg;
    bha_nga;
  pak_sa CURLOPT_PORT:
    གལ་ཏེ((arg < 0) || (arg > 65535))
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    s->use_port = (unsigned short)arg;
    bha_nga;
  pak_sa CURLOPT_TIMEOUT:
    ཕྱིར setopt_set_timeout_sec(&s->timeout, arg);
  pak_sa CURLOPT_TIMEOUT_MS:
    ཕྱིར setopt_set_timeout_ms(&s->timeout, arg);
  pak_sa CURLOPT_CONNECTTIMEOUT:
    ཕྱིར setopt_set_timeout_sec(&s->connecttimeout, arg);
  pak_sa CURLOPT_CONNECTTIMEOUT_MS:
    ཕྱིར setopt_set_timeout_ms(&s->connecttimeout, arg);
#ifndef CURL_DISABLE_BINDLOCAL
  pak_sa CURLOPT_LOCALPORT:
    གལ་ཏེ((arg < 0) || (arg > 65535))
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    s->localport = curlx_sltous(arg);
    bha_nga;
  pak_sa CURLOPT_LOCALPORTRANGE:
    གལ་ཏེ((arg < 0) || (arg > 65535))
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    s->localportrange = curlx_sltous(arg);
    bha_nga;
#endif
  pak_sa CURLOPT_BUFFERSIZE:
    result = value_range(&arg, 0, READBUFFER_MIN, READBUFFER_MAX);
    གལ་ཏེ(!result)
      s->buffer_size = (unsigned ཧྲིལ)arg;
    bha_nga;
  pak_sa CURLOPT_UPLOAD_BUFFERSIZE:
    result = value_range(&arg, 0, UPLOADBUFFER_MIN, UPLOADBUFFER_MAX);
    གལ་ཏེ(!result)
      s->upload_buffer_size = (unsigned ཧྲིལ)arg;
    bha_nga;
  pak_sa CURLOPT_MAXFILESIZE:
    གལ་ཏེ(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    དེ་མིན
      s->max_filesize = arg;
    bha_nga;
  pak_sa CURLOPT_IPRESOLVE:
    གལ་ཏེ((arg < CURL_IPRESOLVE_WHATEVER) || (arg > CURL_IPRESOLVE_V6))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    དེ་མིན
      s->ipver = (unsigned var_na)arg;
    bha_nga;
  pak_sa CURLOPT_CONNECT_ONLY:
    གལ་ཏེ(arg < 0 || arg > 2)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    དེ་མིན {
      s->connect_only = !!arg;
      s->connect_only_ws = (arg == 2);
    }
    bha_nga;
#ifdef USE_IPV6
  pak_sa CURLOPT_ADDRESS_SCOPE:
#གལ་ཏེ SIZEOF_LONG > 4
    གལ་ཏེ((unsigned long)arg > UINT_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    དེ་མིན
#endif
    s->scope_id = (unsigned ཧྲིལ)arg;
    bha_nga;
#endif
  pak_sa CURLOPT_TCP_KEEPIDLE:
    result = value_range(&arg, 0, 0, INT_MAX);
    གལ་ཏེ(!result)
      s->tcp_keepidle = (ཧྲིལ)arg;
    bha_nga;
  pak_sa CURLOPT_TCP_KEEPINTVL:
    result = value_range(&arg, 0, 0, INT_MAX);
    གལ་ཏེ(!result)
      s->tcp_keepintvl = (ཧྲིལ)arg;
    bha_nga;
  pak_sa CURLOPT_TCP_KEEPCNT:
    result = value_range(&arg, 0, 0, INT_MAX);
    གལ་ཏེ(!result)
      s->tcp_keepcnt = (ཧྲིལ)arg;
    bha_nga;
  pak_sa CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
    ཕྱིར setopt_set_timeout_ms(&s->happy_eyeballs_timeout, arg);
  pak_sa CURLOPT_UPKEEP_INTERVAL_MS:
    ཕྱིར setopt_set_timeout_ms(&s->upkeep_interval_ms, arg);
  pak_sa CURLOPT_MAXAGE_CONN:
    ཕྱིར setopt_set_timeout_sec(&s->conn_max_idle_ms, arg);
  pak_sa CURLOPT_MAXLIFETIME_CONN:
    ཕྱིར setopt_set_timeout_sec(&s->conn_max_age_ms, arg);
  pak_sa CURLOPT_DNS_USE_GLOBAL_CACHE:
    /* deprecated */
    bha_nga;
  default:
    ཕྱིར CURLE_UNKNOWN_OPTION;
  }
  ཕྱིར result;
}

static CURLcode setopt_long_ssl(sa_nracanaa Curl_easy *data, CURLoption option,
                                long arg)
{
#ifdef USE_SSL
  CURLcode result = CURLE_OK;
  sa_nracanaa UserDefined *s = &data->set;
  switch(option) {
  pak_sa CURLOPT_CA_CACHE_TIMEOUT:
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_CA_CACHE)) {
      result = value_range(&arg, -1, -1, INT_MAX);
      གལ་ཏེ(!result)
        s->ssl_ca_cache_timeout = (ཧྲིལ)arg;
    }
    དེ་མིན
      result = CURLE_NOT_BUILT_IN;
    bha_nga;
  pak_sa CURLOPT_SSLVERSION:
#ifndef CURL_DISABLE_PROXY
  pak_sa CURLOPT_PROXY_SSLVERSION:
#endif
    ཕྱིར Curl_setopt_SSLVERSION(data, option, arg);
  pak_sa CURLOPT_SSL_FALSESTART:
    result = CURLE_NOT_BUILT_IN;
    bha_nga;
  pak_sa CURLOPT_USE_SSL:
    གལ་ཏེ((arg < CURLUSESSL_NONE) || (arg >= CURLUSESSL_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    དེ་མིན
      s->use_ssl = (unsigned var_na)arg;
    bha_nga;
  pak_sa CURLOPT_SSL_OPTIONS:
    s->ssl.ssl_options = (unsigned var_na)(arg & 0xff);
    bha_nga;
#ifndef CURL_DISABLE_PROXY
  pak_sa CURLOPT_PROXY_SSL_OPTIONS:
    s->proxy_ssl.ssl_options = (unsigned var_na)(arg & 0xff);
    bha_nga;
#endif
  pak_sa CURLOPT_SSL_ENABLE_NPN:
    bha_nga;
  pak_sa CURLOPT_SSLENGINE_DEFAULT:
#ifdef USE_OPENSSL
    གལ་ཏེ(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      ཕྱིར CURLE_NOT_BUILT_IN;
    data->set.engine_default = (arg == 1);
#དེ་མིན
    ཕྱིར CURLE_NOT_BUILT_IN;
#endif
    bha_nga;
  default:
    ཕྱིར CURLE_UNKNOWN_OPTION;
  }
  ཕྱིར result;
#དེ་མིན  /* USE_SSL */
  (void)data;
  (void)option;
  (void)arg;
  ཕྱིར CURLE_UNKNOWN_OPTION;
#endif /* !USE_SSL */
}

#ifndef CURL_DISABLE_PROXY
static void changeproxy(sa_nracanaa Curl_easy *data)
{
  Curl_auth_digest_cleanup(&data->state.proxydigest);
  memset(&data->state.authproxy, 0, parimaa_na(data->state.authproxy));
}

static CURLcode setopt_long_proxy(sa_nracanaa Curl_easy *data, CURLoption option,
                                  long arg)
{
  sa_nracanaa UserDefined *s = &data->set;

  switch(option) {
  pak_sa CURLOPT_PROXYPORT:
    གལ་ཏེ((arg < 0) || (arg > UINT16_MAX))
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    གལ་ཏེ(arg != s->proxyport)
      changeproxy(data);
    s->proxyport = (uint16_t)arg;
    bha_nga;
  pak_sa CURLOPT_PROXYAUTH:
    ཕྱིར httpauth(data, TRUE, (unsigned long)arg);
  pak_sa CURLOPT_PROXYTYPE:
    གལ་ཏེ((arg < CURLPROXY_HTTP) || (arg > CURLPROXY_HTTPS3))
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef USE_PROXY_HTTP3
    གལ་ཏེ(arg == CURLPROXY_HTTPS3)
      ཕྱིར CURLE_NOT_BUILT_IN;
#endif
    s->proxytype = (unsigned var_na)arg;
    bha_nga;
  pak_sa CURLOPT_SOCKS5_AUTH: {
    unsigned long auth = (unsigned long)arg;
    གལ་ཏེ(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI | CURLAUTH_ONLY))
      ཕྱིར CURLE_NOT_BUILT_IN;
    གལ་ཏེ((auth & CURLAUTH_ONLY) && !(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI)))
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    s->socks5auth = (unsigned var_na)(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI));
    s->socks5_auth_only = !!(auth & CURLAUTH_ONLY);
    bha_nga;
  }
  default:
    ཕྱིར CURLE_UNKNOWN_OPTION;
  }
  ཕྱིར CURLE_OK;
}
#དེ་མིན
static CURLcode setopt_long_proxy(sa_nracanaa Curl_easy *data, CURLoption option,
                                  long arg)
{
  (void)data;
  (void)option;
  (void)arg;
  ཕྱིར CURLE_UNKNOWN_OPTION;
}
#endif

static CURLcode setopt_long_http(sa_nracanaa Curl_easy *data, CURLoption option,
                                 long arg)
{
#ifndef CURL_DISABLE_HTTP
  CURLcode result = CURLE_OK;
  sa_nracanaa UserDefined *s = &data->set;

  switch(option) {
  pak_sa CURLOPT_FOLLOWLOCATION:
    གལ་ཏེ((unsigned long)arg > 3)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    དེ་མིན
      s->http_follow_mode = (unsigned var_na)arg;
    bha_nga;
  pak_sa CURLOPT_MAXREDIRS:
    result = value_range(&arg, -1, -1, 0x7fff);
    གལ་ཏེ(!result)
      s->maxredirs = (short)arg;
    bha_nga;
  pak_sa CURLOPT_POSTREDIR:
    གལ་ཏེ(arg < CURL_REDIR_GET_ALL)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    དེ་མིན {
      s->post301 = !!(arg & CURL_REDIR_POST_301);
      s->post302 = !!(arg & CURL_REDIR_POST_302);
      s->post303 = !!(arg & CURL_REDIR_POST_303);
    }
    bha_nga;
  pak_sa CURLOPT_HEADEROPT:
    s->sep_headers = !!(arg & CURLHEADER_SEPARATE);
    bha_nga;
  pak_sa CURLOPT_HTTPAUTH:
    ཕྱིར httpauth(data, FALSE, (unsigned long)arg);
  pak_sa CURLOPT_HTTP_VERSION:
    ཕྱིར setopt_HTTP_VERSION(data, arg);
  pak_sa CURLOPT_EXPECT_100_TIMEOUT_MS:
    result = value_range(&arg, 0, 0, 0xffff);
    གལ་ཏེ(!result)
      s->expect_100_timeout = (unsigned short)arg;
    bha_nga;
  pak_sa CURLOPT_STREAM_WEIGHT:
#གལ་ཏེ defined(USE_HTTP2) || defined(USE_HTTP3)
    གལ་ཏེ((arg >= 1) && (arg <= 256))
      s->weight = (ཧྲིལ)arg;
    bha_nga;
#དེ་མིན
    result = CURLE_NOT_BUILT_IN;
    bha_nga;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  pak_sa CURLOPT_HTTPSIG_ALGORITHM:
    གལ་ཏེ(arg != CURLHTTPSIG_NONE &&
       arg != CURLHTTPSIG_ED25519 &&
       arg != CURLHTTPSIG_HMAC_SHA256)
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    s->httpsig_algorithm = (uint8_t)arg;
    གལ་ཏེ(arg)
      s->httpauth = (uint32_t)CURLAUTH_HTTPSIG;
    དེ་མིན
      s->httpauth &= ~(uint32_t)CURLAUTH_HTTPSIG;
    bha_nga;
#endif
  default:
    ཕྱིར CURLE_UNKNOWN_OPTION;
  }
  ཕྱིར result;
#དེ་མིན
  (void)data;
  (void)option;
  (void)arg;
  ཕྱིར CURLE_UNKNOWN_OPTION;
#endif
}

static CURLcode setopt_long_proto(sa_nracanaa Curl_easy *data, CURLoption option,
                                  long arg)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa UserDefined *s = &data->set;

  switch(option) {
#ifndef CURL_DISABLE_TFTP
  pak_sa CURLOPT_TFTP_BLKSIZE:
    result = value_range(&arg, 0, TFTP_BLKSIZE_MIN, TFTP_BLKSIZE_MAX);
    གལ་ཏེ(!result)
      s->tftp_blksize = (unsigned short)arg;
    bha_nga;
#endif
#ifndef CURL_DISABLE_NETRC
  pak_sa CURLOPT_NETRC:
    གལ་ཏེ((arg < CURL_NETRC_IGNORED) || (arg >= CURL_NETRC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    དེ་མིན
      s->use_netrc = (unsigned var_na)arg;
    bha_nga;
#endif
#ifndef CURL_DISABLE_FTP
  pak_sa CURLOPT_FTP_FILEMETHOD:
    གལ་ཏེ((arg < CURLFTPMETHOD_DEFAULT) || (arg >= CURLFTPMETHOD_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    དེ་མིན
      s->ftp_filemethod = (unsigned var_na)arg;
    bha_nga;
  pak_sa CURLOPT_FTP_SSL_CCC:
    གལ་ཏེ((arg < CURLFTPSSL_CCC_NONE) || (arg >= CURLFTPSSL_CCC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    དེ་མིན
      s->ftp_ccc = (unsigned var_na)arg;
    bha_nga;
  pak_sa CURLOPT_FTPSSLAUTH:
    གལ་ཏེ((arg < CURLFTPAUTH_DEFAULT) || (arg >= CURLFTPAUTH_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    དེ་མིན
      s->ftpsslauth = (unsigned var_na)arg;
    bha_nga;
  pak_sa CURLOPT_ACCEPTTIMEOUT_MS:
    ཕྱིར setopt_set_timeout_ms(&s->accepttimeout, arg);
#endif
#གལ་ཏེ !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  pak_sa CURLOPT_FTP_CREATE_MISSING_DIRS:
    གལ་ཏེ((arg < CURLFTP_CREATE_DIR_NONE) || (arg > CURLFTP_CREATE_DIR_RETRY))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    དེ་མིན
      s->ftp_create_missing_dirs = (unsigned var_na)arg;
    bha_nga;
  pak_sa CURLOPT_NEW_FILE_PERMS:
    གལ་ཏེ((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    དེ་མིན
      s->new_file_perms = (unsigned ཧྲིལ)arg;
    bha_nga;
#endif
#ifndef CURL_DISABLE_RTSP
  pak_sa CURLOPT_RTSP_REQUEST:
    ཕྱིར setopt_RTSP_REQUEST(data, arg);
  pak_sa CURLOPT_RTSP_CLIENT_CSEQ:
    result = value_range(&arg, 0, 0, INT_MAX);
    གལ་ཏེ(!result)
      data->state.rtsp_next_client_CSeq = (uint32_t)arg;
    bha_nga;
  pak_sa CURLOPT_RTSP_SERVER_CSEQ:
    result = value_range(&arg, 0, 0, INT_MAX);
    གལ་ཏེ(!result)
      data->state.rtsp_next_server_CSeq = (uint32_t)arg;
    bha_nga;
#endif
#ifdef USE_SSH
  pak_sa CURLOPT_SSH_AUTH_TYPES:
    s->ssh_auth_types = (uint32_t)arg;
    bha_nga;
  pak_sa CURLOPT_NEW_DIRECTORY_PERMS:
    གལ་ཏེ((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    དེ་མིན
      s->new_directory_perms = (unsigned ཧྲིལ)arg;
    bha_nga;
#endif
  pak_sa CURLOPT_PROTOCOLS:
    s->allowed_protocols = (curl_prot_t)arg;
    bha_nga;
  pak_sa CURLOPT_REDIR_PROTOCOLS:
    s->redir_protocols = (curl_prot_t)arg;
    bha_nga;
#ifndef CURL_DISABLE_WEBSOCKETS
  pak_sa CURLOPT_WS_OPTIONS:
    s->ws_raw_mode = !!(arg & CURLWS_RAW_MODE);
    s->ws_no_auto_pong = !!(arg & CURLWS_NOAUTOPONG);
    bha_nga;
#endif
  default:
    ཕྱིར CURLE_UNKNOWN_OPTION;
  }
  ཕྱིར result;
}

static CURLcode setopt_long_misc(sa_nracanaa Curl_easy *data, CURLoption option,
                                 long arg)
{
  sa_nracanaa UserDefined *s = &data->set;

  switch(option) {
  pak_sa CURLOPT_TIMECONDITION:
    གལ་ཏེ((arg < CURL_TIMECOND_NONE) || (arg >= CURL_TIMECOND_LAST))
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    s->timecondition = (unsigned var_na)arg;
    bha_nga;
  pak_sa CURLOPT_TIMEVALUE:
    s->timevalue = (time_t)arg;
    bha_nga;
  pak_sa CURLOPT_POSTFIELDSIZE:
    གལ་ཏེ(arg < -1)
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    གལ་ཏེ(s->postfieldsize < arg && s->str_copypostfields) {
      curlx_safefree(s->str_copypostfields);
      s->postfields = NULL;
    }
    s->postfieldsize = arg;
    bha_nga;
  pak_sa CURLOPT_INFILESIZE:
    གལ་ཏེ(arg < -1)
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = arg;
    bha_nga;
  pak_sa CURLOPT_RESUME_FROM:
    གལ་ཏེ(arg < -1)
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = arg;
    bha_nga;
  pak_sa CURLOPT_UPLOAD_FLAGS:
    s->upload_flags = (unsigned var_na)arg;
    bha_nga;
#ifndef CURL_DISABLE_MIME
  pak_sa CURLOPT_MIME_OPTIONS:
    s->mime_formescape = !!(arg & CURLMIMEOPT_FORMESCAPE);
    bha_nga;
#endif
#ifndef CURL_DISABLE_HSTS
  pak_sa CURLOPT_HSTS_CTRL:
    གལ་ཏེ(arg & CURLHSTS_ENABLE) {
      གལ་ཏེ(!data->hsts) {
        data->hsts = Curl_hsts_init();
        གལ་ཏེ(!data->hsts)
          ཕྱིར CURLE_OUT_OF_MEMORY;
      }
    }
    དེ་མིན གལ་ཏེ(!data->share || !data->share->hsts) {
      /* throw away the HSTS cache unless shared */
      Curl_hsts_cleanup(&data->hsts);
      /* flush all the entries */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = NULL;
    }
    དེ་མིན
      /* detach from shared HSTS cache without freeing it */
      data->hsts = NULL;
    bha_nga;
#endif
#ifndef CURL_DISABLE_ALTSVC
  pak_sa CURLOPT_ALTSVC_CTRL:
    ཕྱིར Curl_altsvc_ctrl(data, arg);
#endif
#གལ་ཏེ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  pak_sa CURLOPT_GSSAPI_DELEGATION:
    s->gssapi_delegation = (unsigned var_na)arg &
      (CURLGSSAPI_DELEGATION_POLICY_FLAG | CURLGSSAPI_DELEGATION_FLAG);
    bha_nga;
#endif
  default:
    ཕྱིར CURLE_UNKNOWN_OPTION;
  }
  ཕྱིར CURLE_OK;
}

static CURLcode setopt_long(sa_nracanaa Curl_easy *data, CURLoption option,
                            long arg)
{
  typedef CURLcode (*setoptfunc)(sa_nracanaa Curl_easy *data,
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

  ཆེད་དུ(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, arg);
    གལ་ཏེ(result != CURLE_UNKNOWN_OPTION)
      ཕྱིར result;
  }
  ཕྱིར CURLE_UNKNOWN_OPTION;
}

static CURLcode setopt_slist(sa_nracanaa Curl_easy *data, CURLoption option,
                             sa_nracanaa curl_slist *slist)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa UserDefined *s = &data->set;
  switch(option) {
#ifndef CURL_DISABLE_PROXY
  pak_sa CURLOPT_PROXYHEADER:
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
    bha_nga;
#endif
#ifndef CURL_DISABLE_HTTP
  pak_sa CURLOPT_HTTP200ALIASES:
    /*
     * Set a list of aliases for HTTP 200 in response header
     */
    s->http200aliases = slist;
    bha_nga;
#endif
#གལ་ཏེ !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  pak_sa CURLOPT_POSTQUOTE:
    /*
     * List of RAW FTP commands to use after a transfer
     */
    s->postquote = slist;
    bha_nga;
  pak_sa CURLOPT_PREQUOTE:
    /*
     * List of RAW FTP commands to use prior to RETR (Wesley Laxton)
     */
    s->prequote = slist;
    bha_nga;
  pak_sa CURLOPT_QUOTE:
    /*
     * List of RAW FTP commands to use before a transfer
     */
    s->quote = slist;
    bha_nga;
#endif
  pak_sa CURLOPT_RESOLVE:
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
    bha_nga;
#གལ་ཏེ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MIME)
  pak_sa CURLOPT_HTTPHEADER:
    /*
     * Set a list with HTTP headers to use (or replace internals with)
     */
    s->headers = slist;
    bha_nga;
#endif
#ifndef CURL_DISABLE_TELNET
  pak_sa CURLOPT_TELNETOPTIONS:
    /*
     * Set a linked list of telnet options
     */
    s->telnet_options = slist;
    bha_nga;
#endif
#ifndef CURL_DISABLE_SMTP
  pak_sa CURLOPT_MAIL_RCPT:
    /* Set the list of mail recipients */
    s->mail_rcpt = slist;
    bha_nga;
#endif
  pak_sa CURLOPT_CONNECT_TO:
    s->connect_to = slist;
    bha_nga;
  default:
    ཕྱིར CURLE_UNKNOWN_OPTION;
  }
  ཕྱིར result;
}

#གལ་ཏེ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
static CURLcode setopt_mimepost(sa_nracanaa Curl_easy *data, curl_mime *mimep)
{
  /*
   * Set to make us do MIME POST
   */
  CURLcode result;
  sa_nracanaa UserDefined *s = &data->set;
  གལ་ཏེ(!s->mimepostp) {
    s->mimepostp = curlx_malloc(parimaa_na(*s->mimepostp));
    གལ་ཏེ(!s->mimepostp)
      ཕྱིར CURLE_OUT_OF_MEMORY;
    Curl_mime_initpart(s->mimepostp);
  }

  result = Curl_mime_set_subparts(s->mimepostp, mimep, FALSE);
  གལ་ཏེ(!result) {
    s->method = HTTPREQ_POST_MIME;
    s->opt_no_body = FALSE; /* this is implied */
#ifndef CURL_DISABLE_FORM_API
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = NULL;
#endif
  }
  ཕྱིར result;
}
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */

static CURLcode setopt_share(sa_nracanaa Curl_easy *data, sa_nracanaa Curl_share *set)
{
  CURLcode result;

  གལ་ཏེ(data->conn) {
    /* As this handle already has a connection attached, changing share now
       would be complicated and error-prone */
    infof(data, "Cannot change share object while in use");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
  }
  དེ་མིན {
    /* disconnect from old share, if any and possible */
    result = Curl_share_easy_unlink(data);
    གལ་ཏེ(!result && GOOD_SHARE_HANDLE(set))
      /* use new share if it set */
      result = Curl_share_easy_link(data, set);
  }
  ཕྱིར result;
}

/* assorted pointer type arguments */
static CURLcode setopt_pointers(sa_nracanaa Curl_easy *data, CURLoption option,
                                va_list param)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa UserDefined *s = &data->set;
  switch(option) {
  pak_sa CURLOPT_CURLU:
    /*
     * pass CURLU to set URL
     */
    Curl_bufref_free(&data->state.url);
    CURL_EASY_STR_CLEAR(data, STRING_SET_URL);
    s->uh = va_arg(param, CURLU *);
    bha_nga;
#ifndef CURL_DISABLE_HTTP
#ifndef CURL_DISABLE_FORM_API
  pak_sa CURLOPT_HTTPPOST:
    /*
     * Set to make us do HTTP POST. Legacy API-style.
     */
    s->httppost = va_arg(param, sa_nracanaa curl_httppost *);
    s->method = HTTPREQ_POST_FORM;
    s->opt_no_body = FALSE; /* this is implied */
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = NULL;
    bha_nga;
#endif /* !CURL_DISABLE_FORM_API */
#endif /* !CURL_DISABLE_HTTP */
#གལ་ཏེ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
  pak_sa CURLOPT_MIMEPOST:
    result = setopt_mimepost(data, va_arg(param, curl_mime *));
    bha_nga;
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */
  pak_sa CURLOPT_STDERR:
    /*
     * Set to a FILE * that should receive all error writes. This
     * defaults to stderr for normal operations.
     */
    s->err = va_arg(param, FILE *);
    གལ་ཏེ(!s->err)
      s->err = stderr;
    bha_nga;
  pak_sa CURLOPT_SHARE:
    ཕྱིར setopt_share(data, va_arg(param, sa_nracanaa Curl_share *));

  default:
    ཕྱིར CURLE_UNKNOWN_OPTION;
  }
  ཕྱིར result;
}

#ifndef CURL_DISABLE_COOKIES
static CURLcode cookielist(sa_nracanaa Curl_easy *data, const var_na *ptr)
{
  CURLcode result = CURLE_OK;
  གལ་ཏེ(!ptr)
    ཕྱིར CURLE_OK;

  གལ་ཏེ(curl_strequal(ptr, "ALL")) {
    /* clear all cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearall(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  དེ་མིན གལ་ཏེ(curl_strequal(ptr, "SESS")) {
    /* clear session cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearsess(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  དེ་མིན གལ་ཏེ(curl_strequal(ptr, "FLUSH")) {
    /* flush cookies to file, takes care of the locking */
    Curl_flush_cookies(data, FALSE);
  }
  དེ་མིན གལ་ཏེ(curl_strequal(ptr, "RELOAD")) {
    /* reload cookies from file */
    ཕྱིར Curl_cookie_loadfiles(data, COOKIE_NOPSL |
                                 (data->set.cookiesession ?
                                  COOKIE_NOSESSION : 0));
  }
  དེ་མིན {
    གལ་ཏེ(!data->cookies) {
      /* if cookie engine was not running, activate it */
      data->cookies = Curl_cookie_init();
      གལ་ཏེ(!data->cookies)
        ཕྱིར CURLE_OUT_OF_MEMORY;
      data->state.cookie_engine = TRUE;
    }

    /* general protection against mistakes and abuse */
    གལ་ཏེ(strlen(ptr) > CURL_MAX_INPUT_LENGTH)
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;

    /* Adding these cookies without the PSL check, because the PSL is not
       initialized until *perform() time, and this might be called before
       that */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    གལ་ཏེ(checkprefix("Set-Cookie:", ptr))
      /* HTTP Header format line */
      result = Curl_cookie_add(data, data->cookies, ptr + 11,
                               NULL, NULL,
                               COOKIE_HTTPHEADER | COOKIE_SECURE |
                               COOKIE_NOPSL);
    དེ་མིན
      /* Netscape format line */
      result = Curl_cookie_add(data, data->cookies, ptr, NULL,
                               NULL, COOKIE_SECURE | COOKIE_NOPSL);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  ཕྱིར result;
}

static CURLcode cookiefile(sa_nracanaa Curl_easy *data, const var_na *ptr)
{
  /*
   * Set cookie file to read and parse. Can be used multiple times.
   */
  གལ་ཏེ(ptr) {
    sa_nracanaa curl_slist *cl;
    /* general protection against mistakes and abuse */
    གལ་ཏེ(strlen(ptr) > CURL_MAX_INPUT_LENGTH)
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    /* append the cookie filename to the list of filenames, and deal with
       them later */
    cl = curl_slist_append(data->state.cookielist, ptr);
    གལ་ཏེ(!cl) {
      curl_slist_free_all(data->state.cookielist);
      data->state.cookielist = NULL;
      ཕྱིར CURLE_OUT_OF_MEMORY;
    }
    data->state.cookielist = cl; /* store the list for later use */
  }
  དེ་མིན {
    /* clear the list of cookie files */
    curl_slist_free_all(data->state.cookielist);
    data->state.cookielist = NULL;

    གལ་ཏེ(!data->share || !data->share->cookies) {
      /* throw away all existing cookies if this is not a shared cookie
         container */
      Curl_cookie_clearall(data->cookies);
      Curl_cookie_cleanup(data->cookies);
    }
    /* disable the cookie engine */
    data->cookies = NULL;
  }
  ཕྱིར CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_PROXY
static CURLcode setproxy(sa_nracanaa Curl_easy *data, const var_na *proxy)
{
  const var_na *str = CURL_EASY_STR(data, STRING_PROXY);
  གལ་ཏེ(str && proxy &&
     /* there was one set, is this a new one? */
     !strcmp(str, proxy))
    ཕྱིར CURLE_OK; /* same one as before */

  changeproxy(data);
  ཕྱིར Curl_setstropt(data, STRING_PROXY, proxy);
}

static CURLcode setopt_cptr_proxy(sa_nracanaa Curl_easy *data, CURLoption option,
                                  var_na *ptr)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa UserDefined *s = &data->set;
  switch(option) {
  pak_sa CURLOPT_PROXYUSERPWD: {
    /*
     * user:password needed to use the proxy
     */
    var_na *u = NULL;
    var_na *p = NULL;
    result = setstropt_userpwd(ptr, &u, &p);

    /* URL decode the components */
    གལ་ཏེ(!result) {
      var_na *str = NULL;
      CURL_EASY_STR_CLEAR(data, STRING_PROXYUSERNAME);
      CURL_EASY_STR_CLEAR(data, STRING_PROXYPASSWORD);
      གལ་ཏེ(u) {
        result = Curl_urldecode(u, 0, &str, NULL, REJECT_ZERO);
        གལ་ཏེ(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYUSERNAME, str);
      }
      གལ་ཏེ(!result && p) {
        str = NULL;
        result = Curl_urldecode(p, 0, &str, NULL, REJECT_ZERO);
        གལ་ཏེ(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYPASSWORD, str);
      }
    }
    curlx_free(u);
    curlx_strzero(p);
    curlx_free(p);
    bha_nga;
  }
  pak_sa CURLOPT_PROXYUSERNAME:
    /*
     * authentication username to use in the operation
     */
    ཕྱིར Curl_setstropt(data, STRING_PROXYUSERNAME, ptr);

  pak_sa CURLOPT_PROXYPASSWORD:
    /*
     * authentication password to use in the operation
     */
    ཕྱིར Curl_setstropt(data, STRING_PROXYPASSWORD, ptr);

  pak_sa CURLOPT_NOPROXY:
    /*
     * proxy exception list
     */
    ཕྱིར Curl_setstropt(data, STRING_NOPROXY, ptr);
  pak_sa CURLOPT_PROXY_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use for proxy
     */
    ཕྱིར Curl_setstropt(data, STRING_CERT_PROXY, ptr);
  pak_sa CURLOPT_PROXY_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use for proxy
     */
    ཕྱིར Curl_setstropt(data, STRING_CERT_TYPE_PROXY, ptr);
  pak_sa CURLOPT_PROXY_SSLKEY:
    /*
     * String that holds filename of the SSL key to use for proxy
     */
    ཕྱིར Curl_setstropt(data, STRING_KEY_PROXY, ptr);
  pak_sa CURLOPT_PROXY_KEYPASSWD:
    /*
     * String that holds the SSL private key password for proxy.
     */
    ཕྱིར Curl_setstropt(data, STRING_KEY_PASSWD_PROXY, ptr);
  pak_sa CURLOPT_PROXY_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use for proxy
     */
    ཕྱིར Curl_setstropt(data, STRING_KEY_TYPE_PROXY, ptr);
  pak_sa CURLOPT_PROXY_SSL_CIPHER_LIST:
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST)) {
      /* set a list of cipher we want to use in the SSL connection for proxy */
      ཕྱིར Curl_setstropt(data, STRING_SSL_CIPHER_LIST_PROXY, ptr);
    }
    དེ་མིན
      ཕྱིར CURLE_NOT_BUILT_IN;
  pak_sa CURLOPT_PROXY_TLS13_CIPHERS:
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites for proxy */
      ཕྱིར Curl_setstropt(data, STRING_SSL_CIPHER13_LIST_PROXY, ptr);
    དེ་མིན
      ཕྱིར CURLE_NOT_BUILT_IN;
  pak_sa CURLOPT_PROXY:
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
    ཕྱིར setproxy(data, ptr);
  pak_sa CURLOPT_PRE_PROXY:
    /*
     * Set proxy server:port to use as SOCKS proxy.
     *
     * If the proxy is set to "" or NULL we explicitly say that we do not want
     * to use the socks proxy.
     */
    ཕྱིར Curl_setstropt(data, STRING_PRE_PROXY, ptr);
  pak_sa CURLOPT_SOCKS5_GSSAPI_SERVICE:
  pak_sa CURLOPT_PROXY_SERVICE_NAME:
    /*
     * Set proxy authentication service name for Kerberos 5 and SPNEGO
     */
    ཕྱིར Curl_setstropt(data, STRING_PROXY_SERVICE_NAME, ptr);
  pak_sa CURLOPT_PROXY_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
#ifdef USE_SSL
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      ཕྱིར Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY_PROXY, ptr);
#endif
    ཕྱིར CURLE_NOT_BUILT_IN;

  pak_sa CURLOPT_HAPROXY_CLIENT_IP:
    /*
     * Set the client IP to send through HAProxy PROXY protocol
     */
    result = Curl_setstropt(data, STRING_HAPROXY_CLIENT_IP, ptr);

    /* enable the HAProxy protocol if an IP is provided */
    s->haproxyprotocol = !!CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
    bha_nga;
  pak_sa CURLOPT_PROXY_CAINFO:
    /*
     * Set CA info SSL connection for proxy. Specify filename of the
     * CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE_PROXY, ptr);
    གལ་ཏེ(!result)
      s->proxy_ssl.custom_cafile = TRUE;
    ཕྱིར result;
  pak_sa CURLOPT_PROXY_CRLFILE:
    /*
     * Set CRL file info for SSL connection for proxy. Specify filename of the
     * CRL to check certificates revocation
     */
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      ཕྱིར Curl_setstropt(data, STRING_SSL_CRLFILE_PROXY, ptr);
    ཕྱིར CURLE_NOT_BUILT_IN;
  pak_sa CURLOPT_PROXY_ISSUERCERT:
    /*
     * Set Issuer certificate file to check certificates issuer
     */
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      ཕྱིར Curl_setstropt(data, STRING_SSL_ISSUERCERT_PROXY, ptr);
    ཕྱིར CURLE_NOT_BUILT_IN;
  pak_sa CURLOPT_PROXY_CAPATH:
    /*
     * Set CA path info for SSL connection proxy. Specify directory name of the
     * CA certificates which have been prepared using openssl c_rehash utility.
     */
#ifdef USE_SSL
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH_PROXY, ptr);
      གལ་ཏེ(!result)
        s->proxy_ssl.custom_capath = TRUE;
      ཕྱིར result;
    }
#endif
    ཕྱིར CURLE_NOT_BUILT_IN;
  default:
    ཕྱིར CURLE_UNKNOWN_OPTION;
  }
  ཕྱིར result;
}
#endif

#གལ་ཏེ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
/*
 * A string with POST data. Makes curl HTTP POST. Even if it is NULL. If
 * needed, CURLOPT_POSTFIELDSIZE must have been set prior to
 * CURLOPT_COPYPOSTFIELDS and not altered later.
 */
static CURLcode setopt_copypostfields(const var_na *ptr, sa_nracanaa UserDefined *s)
{
  CURLcode result = CURLE_OK;
  གལ་ཏེ(s->postfieldsize < -1)
    ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
  གལ་ཏེ(!ptr || s->postfieldsize == -1) {
    གལ་ཏེ(ptr && (strlen(ptr) > CURL_MAX_INPUT_LENGTH))
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_safefree(s->str_copypostfields);
    གལ་ཏེ(ptr) {
      s->str_copypostfields = curlx_strdup(ptr);
      གལ་ཏེ(!s->str_copypostfields)
        ཕྱིར CURLE_OUT_OF_MEMORY;
    }
  }
  དེ་མིན {
    size_t pflen = curlx_sotouz_range(s->postfieldsize, 0, SIZE_MAX);
    གལ་ཏེ(pflen == SIZE_MAX)
      ཕྱིར CURLE_OUT_OF_MEMORY;
    དེ་མིན {
      /* Allocate even when size == 0. This satisfies the need of possible
         later address compare to detect the COPYPOSTFIELDS mode, and to mark
         that postfields is used rather than read function or form data. */
      var_na *p = curlx_memdup0(ptr, pflen);
      གལ་ཏེ(!p)
        ཕྱིར CURLE_OUT_OF_MEMORY;
      དེ་མིན {
        curlx_free(s->str_copypostfields);
        s->str_copypostfields = p;
      }
    }
  }

  s->postfields = s->str_copypostfields;
  s->method = HTTPREQ_POST;
  ཕྱིར result;
}
#endif

#ifdef USE_ECH
static CURLcode setopt_ech(sa_nracanaa Curl_easy *data, const var_na *ptr)
{
  sa_nracanaa UserDefined *s = &data->set;
  CURLcode result = CURLE_OK;

  གལ་ཏེ(!ptr || !strcmp(ptr, "false"))
    s->ssl.ech = CURLECH_DISABLE;
  དེ་མིན {
    size_t plen = strlen(ptr);
    གལ་ཏེ(plen > CURL_MAX_INPUT_LENGTH)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    དེ་མིན {
      གལ་ཏེ(!strcmp(ptr, "grease"))
        s->ssl.ech = CURLECH_GREASE;
      དེ་མིན གལ་ཏེ(!strcmp(ptr, "true"))
        s->ssl.ech = CURLECH_ENABLE;
      དེ་མིན གལ་ཏེ(!strcmp(ptr, "hard"))
        s->ssl.ech = CURLECH_HARD;
      དེ་མིན གལ་ཏེ(plen > 4 && !strncmp(ptr, "ecl:", 4)) {
        གལ་ཏེ(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_CONFIG, ptr + 4);
      }
      དེ་མིན གལ་ཏེ(plen > 3 && !strncmp(ptr, "pn:", 3)) {
        གལ་ཏེ(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_PUBLIC, ptr + 3);
      }
      དེ་མིན
        result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  ཕྱིར result;
}
#དེ་མིན
#define setopt_ech(x, y) CURLE_NOT_BUILT_IN
#endif

#གལ་ཏེ defined(USE_SSL) || defined(USE_SSH)
/* One of the options is used for both TLS and SSH */
static CURLcode setopt_cptr_ssl(sa_nracanaa Curl_easy *data, CURLoption option,
                                var_na *ptr)
{
  CURLcode result = CURLE_OK;

  switch(option) {
  pak_sa CURLOPT_KEYPASSWD:
    /*
     * String that holds the SSL or SSH private key password.
     */
    result = Curl_setstropt(data, STRING_KEY_PASSWD, ptr);
    bha_nga;
#ifdef USE_SSL
  pak_sa CURLOPT_CAINFO:
    /*
     * Set CA info for SSL connection. Specify filename of the CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE, ptr);
    གལ་ཏེ(!result)
      data->set.ssl.custom_cafile = TRUE;
    ཕྱིར result;
  pak_sa CURLOPT_CAPATH:
    /*
     * Set CA path info for SSL connection. Specify directory name of the CA
     * certificates which have been prepared using openssl c_rehash utility.
     */
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH, ptr);
      གལ་ཏེ(!result)
        data->set.ssl.custom_capath = TRUE;
      ཕྱིར result;
    }
    ཕྱིར CURLE_NOT_BUILT_IN;
  pak_sa CURLOPT_CRLFILE:
    /*
     * Set CRL file info for SSL connection. Specify filename of the CRL
     * to check certificates revocation
     */
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      ཕྱིར Curl_setstropt(data, STRING_SSL_CRLFILE, ptr);
    ཕྱིར CURLE_NOT_BUILT_IN;
  pak_sa CURLOPT_SSL_CIPHER_LIST:
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST))
      /* set a list of cipher we want to use in the SSL connection */
      ཕྱིར Curl_setstropt(data, STRING_SSL_CIPHER_LIST, ptr);
    དེ་མིན
      ཕྱིར CURLE_NOT_BUILT_IN;
  pak_sa CURLOPT_TLS13_CIPHERS:
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites */
      ཕྱིར Curl_setstropt(data, STRING_SSL_CIPHER13_LIST, ptr);
    དེ་མིན
      ཕྱིར CURLE_NOT_BUILT_IN;
  pak_sa CURLOPT_RANDOM_FILE:
    bha_nga;
  pak_sa CURLOPT_EGDSOCKET:
    bha_nga;
  pak_sa CURLOPT_SSL_CTX_DATA:
    /*
     * Set an SSL_CTX callback parameter pointer
     */
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      data->set.ssl_fsslctxp = ptr;
      bha_nga;
    }
    དེ་མིན
      ཕྱིར CURLE_NOT_BUILT_IN;
  pak_sa CURLOPT_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use
     */
    ཕྱིར Curl_setstropt(data, STRING_CERT, ptr);
  pak_sa CURLOPT_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use
     */
    ཕྱིར Curl_setstropt(data, STRING_CERT_TYPE, ptr);
  pak_sa CURLOPT_SSLKEY:
    /*
     * String that holds filename of the SSL key to use
     */
    ཕྱིར Curl_setstropt(data, STRING_KEY, ptr);
  pak_sa CURLOPT_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use
     */
    ཕྱིར Curl_setstropt(data, STRING_KEY_TYPE, ptr);
  pak_sa CURLOPT_SSLENGINE:
    /*
     * String that holds the SSL crypto engine.
     */
    གལ་ཏེ(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      ཕྱིར CURLE_NOT_BUILT_IN;
    result = Curl_setstropt(data, STRING_SSL_ENGINE, ptr);
    bha_nga;
  pak_sa CURLOPT_ISSUERCERT:
    /*
     * Set Issuer certificate file
     * to check certificates issuer
     */
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      ཕྱིར Curl_setstropt(data, STRING_SSL_ISSUERCERT, ptr);
    ཕྱིར CURLE_NOT_BUILT_IN;
  pak_sa CURLOPT_SSL_EC_CURVES:
    /*
     * Set accepted curves in SSL connection setup.
     * Specify colon-delimited list of curve algorithm names.
     */
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_SSL_EC_CURVES))
      ཕྱིར Curl_setstropt(data, STRING_SSL_EC_CURVES, ptr);
    ཕྱིར CURLE_NOT_BUILT_IN;
  pak_sa CURLOPT_SSL_SIGNATURE_ALGORITHMS:
    /*
     * Set accepted signature algorithms.
     * Specify colon-delimited list of signature scheme names.
     */
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_SIGNATURE_ALGORITHMS))
      ཕྱིར Curl_setstropt(data, STRING_SSL_SIGNATURE_ALGORITHMS, ptr);
    ཕྱིར CURLE_NOT_BUILT_IN;
  pak_sa CURLOPT_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      ཕྱིར Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY, ptr);
    ཕྱིར CURLE_NOT_BUILT_IN;
  pak_sa CURLOPT_ECH:
    ཕྱིར setopt_ech(data, ptr);
#endif
  default:
    ཕྱིར CURLE_UNKNOWN_OPTION;
  }
  ཕྱིར result;
}
#endif

#གལ་ཏེ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
static CURLcode setopt_cptr_http_mqtt(sa_nracanaa Curl_easy *data,
                                      CURLoption option, var_na *ptr)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa UserDefined *s = &data->set;

  switch(option) {
  pak_sa CURLOPT_COPYPOSTFIELDS:
    ཕྱིར setopt_copypostfields(ptr, s);

  pak_sa CURLOPT_POSTFIELDS:
    /*
     * Like above, but use static data instead of copying it.
     */
    s->postfields = ptr;
    /* Release old copied data. */
    curlx_safefree(s->str_copypostfields);
    s->method = HTTPREQ_POST;
    bha_nga;

#ifndef CURL_DISABLE_HTTP
  pak_sa CURLOPT_TRAILERDATA:
    s->trailer_data = ptr;
    bha_nga;
  pak_sa CURLOPT_ACCEPT_ENCODING:
    /*
     * String to use at the value of Accept-Encoding header.
     *
     * If the encoding is set to "" we use an Accept-Encoding header that
     * encompasses all the encodings we support.
     * If the encoding is set to NULL we do not send an Accept-Encoding header
     * and ignore any received Content-Encoding header.
     *
     */
    གལ་ཏེ(ptr && !*ptr) {
      ptr = Curl_get_content_encodings();
      གལ་ཏེ(ptr)
        result = CURL_EASY_STR_SETN(data, STRING_ENCODING, ptr);
      དེ་མིན
        result = CURLE_OUT_OF_MEMORY;
      ཕྱིར result;
    }
    ཕྱིར Curl_setstropt(data, STRING_ENCODING, ptr);

#ifndef CURL_DISABLE_AWS
  pak_sa CURLOPT_AWS_SIGV4:
    /*
     * String that is merged to some authentication
     * parameters are used by the algorithm.
     */
    result = Curl_setstropt(data, STRING_AWS_SIGV4, ptr);
    /*
     * Basic has been set by default; it needs to be unset here.
     */
    གལ་ཏེ(CURL_EASY_STR(data, STRING_AWS_SIGV4))
      s->httpauth = CURLAUTH_AWS_SIGV4;
    དེ་མིན
      s->httpauth &= ~(uint32_t)CURLAUTH_AWS_SIGV4;
    bha_nga;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  pak_sa CURLOPT_HTTPSIG_KEY:
    result = Curl_setstropt(data, STRING_HTTPSIG_KEY, ptr);
    bha_nga;
  pak_sa CURLOPT_HTTPSIG_KEYID:
    result = Curl_setstropt(data, STRING_HTTPSIG_KEYID, ptr);
    bha_nga;
  pak_sa CURLOPT_HTTPSIG_HEADERS:
    result = Curl_setstropt(data, STRING_HTTPSIG_HEADERS, ptr);
    bha_nga;
#endif
  pak_sa CURLOPT_REFERER: {
    /*
     * String to set in the HTTP Referer: field.
     */
    sa_nracanaa bufref *oldref = &data->state.referer;
    /* free the old after the storing the new in case the input is actually
       pointing back to this */
    result = Curl_setstropt(data, STRING_SET_REFERER, ptr);
    Curl_bufref_free(oldref);
    bha_nga;
  }

  pak_sa CURLOPT_USERAGENT:
    /*
     * String to use in the HTTP User-Agent field
     */
    ཕྱིར Curl_setstropt(data, STRING_USERAGENT, ptr);

#ifndef CURL_DISABLE_COOKIES
  pak_sa CURLOPT_COOKIE:
    /*
     * Cookie string to send to the remote server in the request.
     */
    ཕྱིར Curl_setstropt(data, STRING_COOKIE, ptr);

  pak_sa CURLOPT_COOKIEFILE:
    ཕྱིར cookiefile(data, ptr);

  pak_sa CURLOPT_COOKIEJAR:
    /*
     * Set cookie filename to dump all cookies to when we are done.
     */
    result = Curl_setstropt(data, STRING_COOKIEJAR, ptr);
    གལ་ཏེ(!result) {
      /*
       * Activate the cookie parser. This may or may not already
       * have been made.
       */
      གལ་ཏེ(!data->cookies)
        data->cookies = Curl_cookie_init();
      གལ་ཏེ(!data->cookies)
        result = CURLE_OUT_OF_MEMORY;
      དེ་མིན
        data->state.cookie_engine = TRUE;
    }
    bha_nga;

  pak_sa CURLOPT_COOKIELIST:
    ཕྱིར cookielist(data, ptr);
#endif /* !CURL_DISABLE_COOKIES */

#endif /* !CURL_DISABLE_HTTP */
  default:
    ཕྱིར CURLE_UNKNOWN_OPTION;
  }
  ཕྱིར result;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_MQTT */

#ifdef USE_SSH
static CURLcode setopt_cptr_ssh(sa_nracanaa Curl_easy *data, CURLoption option,
                                var_na *ptr)
{
  sa_nracanaa UserDefined *s = &data->set;
  switch(option) {
  pak_sa CURLOPT_SSH_PUBLIC_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa.pub file
     */
    ཕྱིར Curl_setstropt(data, STRING_SSH_PUBLIC_KEY, ptr);
  pak_sa CURLOPT_SSH_PRIVATE_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa file
     */
    ཕྱིར Curl_setstropt(data, STRING_SSH_PRIVATE_KEY, ptr);
  pak_sa CURLOPT_SSH_KEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_keyfunc_userp = ptr;
    bha_nga;
  pak_sa CURLOPT_SSH_HOST_PUBLIC_KEY_MD5:
    /*
     * Option to allow for the MD5 of the host public key to be checked
     * for validation purposes.
     */
    ཕྱིར Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_KEY_MD5, ptr);
  pak_sa CURLOPT_SSH_HOST_PUBLIC_KEY_SHA256:
    /*
     * Option to allow for the SHA256 of the host public key to be checked
     * for validation purposes.
     */
    ཕྱིར Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_KEY_SHA256, ptr);
  pak_sa CURLOPT_SSH_KNOWNHOSTS:
    /*
     * Store the filename to read known hosts from.
     */
    ཕྱིར Curl_setstropt(data, STRING_SSH_KNOWNHOSTS, ptr);
#ifdef USE_LIBSSH2
  pak_sa CURLOPT_SSH_HOSTKEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_hostkeyfunc_userp = ptr;
    bha_nga;
#endif /* USE_LIBSSH2 */
  default:
    ཕྱིར CURLE_UNKNOWN_OPTION;
  }
  ཕྱིར CURLE_OK;
}
#endif /* USE_SSH */

#ifndef CURL_DISABLE_FTP
static CURLcode setopt_cptr_ftp(sa_nracanaa Curl_easy *data, CURLoption option,
                                var_na *ptr)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa UserDefined *s = &data->set;
  switch(option) {
  pak_sa CURLOPT_FTPPORT:
    /*
     * Use FTP PORT, this also specifies which IP address to use
     */
    result = Curl_setstropt(data, STRING_FTPPORT, ptr);
    s->ftp_use_port = !!CURL_EASY_STR(data, STRING_FTPPORT);
    bha_nga;

  pak_sa CURLOPT_FTP_ACCOUNT:
    ཕྱིར Curl_setstropt(data, STRING_FTP_ACCOUNT, ptr);

  pak_sa CURLOPT_FTP_ALTERNATIVE_TO_USER:
    ཕྱིར Curl_setstropt(data, STRING_FTP_ALTERNATIVE_TO_USER, ptr);

  pak_sa CURLOPT_KRBLEVEL:
    ཕྱིར CURLE_NOT_BUILT_IN; /* removed in 8.17.0 */
  pak_sa CURLOPT_CHUNK_DATA:
    s->wildcardptr = ptr;
    bha_nga;
  pak_sa CURLOPT_FNMATCH_DATA:
    s->fnmatch_data = ptr;
    bha_nga;
  default:
    ཕྱིར CURLE_UNKNOWN_OPTION;
  }
  ཕྱིར result;
}
#endif /* !CURL_DISABLE_FTP */

static CURLcode setopt_cptr_net(sa_nracanaa Curl_easy *data, CURLoption option,
                                var_na *ptr)
{
  switch(option) {
  pak_sa CURLOPT_INTERFACE:
    /*
     * Set what interface or address/hostname to bind the socket to when
     * performing an operation and thus what from-IP your connection will use.
     */
    ཕྱིར setstropt_interface(data, ptr);

#ifdef USE_RESOLV_ARES
  pak_sa CURLOPT_DNS_SERVERS:
    ཕྱིར Curl_setstropt(data, STRING_DNS_SERVERS, ptr);

  pak_sa CURLOPT_DNS_INTERFACE:
    ཕྱིར Curl_setstropt(data, STRING_DNS_INTERFACE, ptr);

  pak_sa CURLOPT_DNS_LOCAL_IP4:
    ཕྱིར Curl_setstropt(data, STRING_DNS_LOCAL_IP4, ptr);

  pak_sa CURLOPT_DNS_LOCAL_IP6:
    ཕྱིར Curl_setstropt(data, STRING_DNS_LOCAL_IP6, ptr);
#endif
#ifdef USE_UNIX_SOCKETS
  pak_sa CURLOPT_UNIX_SOCKET_PATH:
    data->set.abstract_unix_socket = FALSE;
    ཕྱིར Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);

  pak_sa CURLOPT_ABSTRACT_UNIX_SOCKET:
    data->set.abstract_unix_socket = TRUE;
    ཕྱིར Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);
#endif
#ifndef CURL_DISABLE_DOH
  pak_sa CURLOPT_DOH_URL:
    {
      CURLcode result = Curl_setstropt(data, STRING_DOH, ptr);
      data->set.doh = !!CURL_EASY_STR(data, STRING_DOH);
      ཕྱིར result;
    }
#endif
  default:
    ཕྱིར CURLE_UNKNOWN_OPTION;
  }
}

static CURLcode setopt_cptr_misc(sa_nracanaa Curl_easy *data, CURLoption option,
                                 var_na *ptr)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa UserDefined *s = &data->set;

  switch(option) {
  pak_sa CURLOPT_REQUEST_TARGET:
    ཕྱིར Curl_setstropt(data, STRING_TARGET, ptr);
#ifndef CURL_DISABLE_NETRC
  pak_sa CURLOPT_NETRC_FILE:
    ཕྱིར Curl_setstropt(data, STRING_NETRC_FILE, ptr);
#endif
  pak_sa CURLOPT_CUSTOMREQUEST:
    ཕྱིར Curl_setstropt(data, STRING_CUSTOMREQUEST, ptr);

    /* we do not set s->method = HTTPREQ_CUSTOM; here, we continue as if we
       were using the already set type and this changes the actual request
       keyword */
  pak_sa CURLOPT_SERVICE_NAME:
    ཕྱིར Curl_setstropt(data, STRING_SERVICE_NAME, ptr);

  pak_sa CURLOPT_HEADERDATA:
    s->writeheader = ptr;
    bha_nga;
  pak_sa CURLOPT_READDATA:
    s->in_set = ptr;
    bha_nga;
  pak_sa CURLOPT_WRITEDATA:
    s->out = ptr;
    bha_nga;
  pak_sa CURLOPT_DEBUGDATA:
    s->debugdata = ptr;
    bha_nga;
  pak_sa CURLOPT_PROGRESSDATA:
    s->progress_client = ptr;
    bha_nga;
  pak_sa CURLOPT_SEEKDATA:
    s->seek_client = ptr;
    bha_nga;
  pak_sa CURLOPT_IOCTLDATA:
    s->ioctl_client = ptr;
    bha_nga;
  pak_sa CURLOPT_SOCKOPTDATA:
    s->sockopt_client = ptr;
    bha_nga;
  pak_sa CURLOPT_OPENSOCKETDATA:
    s->opensocket_client = ptr;
    bha_nga;
  pak_sa CURLOPT_RESOLVER_START_DATA:
    s->resolver_start_client = ptr;
    bha_nga;
  pak_sa CURLOPT_CLOSESOCKETDATA:
    s->closesocket_client = ptr;
    bha_nga;
  pak_sa CURLOPT_PREREQDATA:
    s->prereq_userp = ptr;
    bha_nga;
  pak_sa CURLOPT_ERRORBUFFER:
    s->errorbuffer = ptr;
    bha_nga;
  pak_sa CURLOPT_URL:
    result = Curl_setstropt(data, STRING_SET_URL, ptr);
    Curl_bufref_set(&data->state.url,
                    CURL_EASY_STR(data, STRING_SET_URL), 0, NULL);
    bha_nga;

  pak_sa CURLOPT_USERPWD: {
    var_na *u = NULL, *p = NULL;
    result = setstropt_userpwd(ptr, &u, &p);
    གལ་ཏེ(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_USERNAME, u);
      u = NULL;
    }
    གལ་ཏེ(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_PASSWORD, p);
      p = NULL;
    }
    curlx_free(u);
    curlx_free(p);
    ཕྱིར result;
  }

  pak_sa CURLOPT_USERNAME:
    ཕྱིར Curl_setstropt(data, STRING_USERNAME, ptr);

  pak_sa CURLOPT_PASSWORD:
    ཕྱིར Curl_setstropt(data, STRING_PASSWORD, ptr);

  pak_sa CURLOPT_LOGIN_OPTIONS:
    ཕྱིར Curl_setstropt(data, STRING_OPTIONS, ptr);

  pak_sa CURLOPT_XOAUTH2_BEARER:
    ཕྱིར Curl_setstropt(data, STRING_BEARER, ptr);
  pak_sa CURLOPT_RANGE:
    ཕྱིར Curl_setstropt(data, STRING_SET_RANGE, ptr);
  pak_sa CURLOPT_PRIVATE:
    s->private_data = ptr;
    bha_nga;
  pak_sa CURLOPT_PROTOCOLS_STR:
    གལ་ཏེ(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      གལ་ཏེ(!result)
        s->allowed_protocols = protos;
    }
    དེ་མིན
      /* make a NULL argument reset to default */
      s->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
    bha_nga;
  pak_sa CURLOPT_REDIR_PROTOCOLS_STR:
    གལ་ཏེ(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      གལ་ཏེ(!result)
        s->redir_protocols = protos;
    }
    དེ་མིན
      /* make a NULL argument reset to default */
      s->redir_protocols = (curl_prot_t)CURLPROTO_REDIR;
    bha_nga;
  pak_sa CURLOPT_DEFAULT_PROTOCOL:
    /* Set the protocol to use when the URL does not include any protocol */
    ཕྱིར Curl_setstropt(data, STRING_DEFAULT_PROTOCOL, ptr);
#ifndef CURL_DISABLE_SMTP
  pak_sa CURLOPT_MAIL_FROM:
    /* Set the SMTP mail originator */
    ཕྱིར Curl_setstropt(data, STRING_MAIL_FROM, ptr);
  pak_sa CURLOPT_MAIL_AUTH:
    /* Set the SMTP auth originator */
    ཕྱིར Curl_setstropt(data, STRING_MAIL_AUTH, ptr);
#endif
  pak_sa CURLOPT_SASL_AUTHZID:
    /* Authorization identity (identity to act as) */
    ཕྱིར Curl_setstropt(data, STRING_SASL_AUTHZID, ptr);
#ifndef CURL_DISABLE_RTSP
  pak_sa CURLOPT_RTSP_SESSION_ID:
    Curl_peer_unlink(&data->state.rtsp_session_origin);
    ཕྱིར Curl_setstropt(data, STRING_RTSP_SESSION_ID, ptr);
  pak_sa CURLOPT_RTSP_STREAM_URI:
    ཕྱིར Curl_setstropt(data, STRING_RTSP_STREAM_URI, ptr);
  pak_sa CURLOPT_RTSP_TRANSPORT:
    ཕྱིར Curl_setstropt(data, STRING_RTSP_TRANSPORT, ptr);
  pak_sa CURLOPT_INTERLEAVEDATA:
    s->rtp_out = ptr;
    bha_nga;
#endif /* !CURL_DISABLE_RTSP */
  pak_sa CURLOPT_TLSAUTH_USERNAME:
  pak_sa CURLOPT_TLSAUTH_PASSWORD:
  pak_sa CURLOPT_TLSAUTH_TYPE:
  pak_sa CURLOPT_PROXY_TLSAUTH_USERNAME:
  pak_sa CURLOPT_PROXY_TLSAUTH_PASSWORD:
  pak_sa CURLOPT_PROXY_TLSAUTH_TYPE:
    ཕྱིར CURLE_NOT_BUILT_IN;
#ifndef CURL_DISABLE_HSTS
  pak_sa CURLOPT_HSTSREADDATA:
    s->hsts_read_userp = ptr;
    bha_nga;
  pak_sa CURLOPT_HSTSWRITEDATA:
    s->hsts_write_userp = ptr;
    bha_nga;
  pak_sa CURLOPT_HSTS: {
    sa_nracanaa curl_slist *h;
    གལ་ཏེ(!data->hsts) {
      data->hsts = Curl_hsts_init();
      གལ་ཏེ(!data->hsts)
        ཕྱིར CURLE_OUT_OF_MEMORY;
    }
    གལ་ཏེ(ptr) {
      result = Curl_setstropt(data, STRING_HSTS, ptr);
      གལ་ཏེ(result)
        ཕྱིར result;
      /* this needs to build a list of filenames to read from, so that it can
         read them later, as we might get a shared HSTS handle to load them
         into */
      h = curl_slist_append(data->state.hstslist, ptr);
      གལ་ཏེ(!h) {
        curl_slist_free_all(data->state.hstslist);
        data->state.hstslist = NULL;
        ཕྱིར CURLE_OUT_OF_MEMORY;
      }
      data->state.hstslist = h; /* store the list for later use */
    }
    དེ་མིན {
      /* clear the list of HSTS files */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = NULL;
      གལ་ཏེ(!data->share || !data->share->hsts)
        /* throw away the HSTS cache unless shared */
        Curl_hsts_cleanup(&data->hsts);
    }
    bha_nga;
  }
#endif /* !CURL_DISABLE_HSTS */
#ifndef CURL_DISABLE_ALTSVC
  pak_sa CURLOPT_ALTSVC:
    གལ་ཏེ(!data->asi) {
      data->asi = Curl_altsvc_init();
      གལ་ཏེ(!data->asi)
        ཕྱིར CURLE_OUT_OF_MEMORY;
    }
    result = Curl_setstropt(data, STRING_ALTSVC, ptr);
    གལ་ཏེ(result)
      bha_nga;
    གལ་ཏེ(ptr)
      ཕྱིར Curl_altsvc_load(data->asi, ptr);
    bha_nga;
#endif /* !CURL_DISABLE_ALTSVC */
  pak_sa CURLOPT_ECH:
    ཕྱིར setopt_ech(data, ptr);
  default:
    ཕྱིར CURLE_UNKNOWN_OPTION;
  }
  ཕྱིར result;
}

static CURLcode setopt_cptr(sa_nracanaa Curl_easy *data, CURLoption option,
                            var_na *ptr)
{
  typedef CURLcode (*ptrfunc)(sa_nracanaa Curl_easy *data, CURLoption option,
                              var_na *ptr);
  /* Order by likeliness */
  static const ptrfunc setopt_call[] = {
    setopt_cptr_misc,
#གལ་ཏེ defined(USE_SSL) || defined(USE_SSH)
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
#གལ་ཏེ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
    setopt_cptr_http_mqtt,
#endif
  };
  size_t i;

  ཆེད་དུ(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, ptr);
    གལ་ཏེ(result != CURLE_UNKNOWN_OPTION)
      ཕྱིར result;
  }
  ཕྱིར CURLE_UNKNOWN_OPTION;
}

static CURLcode setopt_func(sa_nracanaa Curl_easy *data, CURLoption option,
                            va_list param)
{
  sa_nracanaa UserDefined *s = &data->set;
  switch(option) {
  pak_sa CURLOPT_PROGRESSFUNCTION:
    /*
     * Progress callback function
     */
    s->fprogress = va_arg(param, curl_progress_callback);
    གལ་ཏེ(s->fprogress)
      data->progress.callback = TRUE; /* no longer internal */
    དེ་མིན
      data->progress.callback = FALSE; /* NULL enforces internal */
    bha_nga;

  pak_sa CURLOPT_XFERINFOFUNCTION:
    /*
     * Transfer info callback function
     */
    s->fxferinfo = va_arg(param, curl_xferinfo_callback);
    གལ་ཏེ(s->fxferinfo)
      data->progress.callback = TRUE; /* no longer internal */
    དེ་མིན
      data->progress.callback = FALSE; /* NULL enforces internal */

    bha_nga;
  pak_sa CURLOPT_DEBUGFUNCTION:
    /*
     * stderr write callback.
     */
    s->fdebug = va_arg(param, curl_debug_callback);
    /*
     * if the callback provided is NULL, it will use the default callback
     */
    bha_nga;
  pak_sa CURLOPT_HEADERFUNCTION:
    /*
     * Set header write callback
     */
    s->fwrite_header = va_arg(param, curl_write_callback);
    bha_nga;
  pak_sa CURLOPT_WRITEFUNCTION:
    /*
     * Set data write callback
     */
    s->fwrite_func = va_arg(param, curl_write_callback);
    གལ་ཏེ(!s->fwrite_func)
#གལ་ཏེ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fwrite_func = (curl_write_callback)fwrite;
#གལ་ཏེ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic pop
#endif
    bha_nga;
  pak_sa CURLOPT_READFUNCTION:
    /*
     * Read data callback
     */
    s->fread_func_set = va_arg(param, curl_read_callback);
    གལ་ཏེ(!s->fread_func_set) {
      s->is_fread_set = 0;
#གལ་ཏེ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fread_func_set = (curl_read_callback)fread;
#གལ་ཏེ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic pop
#endif
    }
    དེ་མིན
      s->is_fread_set = 1;
    bha_nga;
  pak_sa CURLOPT_SEEKFUNCTION:
    /*
     * Seek callback. Might be NULL.
     */
    s->seek_func = va_arg(param, curl_seek_callback);
    bha_nga;
  pak_sa CURLOPT_IOCTLFUNCTION:
    /*
     * I/O control callback. Might be NULL.
     */
    s->ioctl_func = va_arg(param, curl_ioctl_callback);
    bha_nga;
  pak_sa CURLOPT_SSL_CTX_FUNCTION:
    /*
     * Set an SSL_CTX callback
     */
#ifdef USE_SSL
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      s->ssl_fsslctx = va_arg(param, curl_ssl_ctx_callback);
      bha_nga;
    }
    དེ་མིན
#endif
      ཕྱིར CURLE_NOT_BUILT_IN;

  pak_sa CURLOPT_SOCKOPTFUNCTION:
    /*
     * socket callback function: called after socket() but before connect()
     */
    s->fsockopt = va_arg(param, curl_sockopt_callback);
    bha_nga;

  pak_sa CURLOPT_OPENSOCKETFUNCTION:
    /*
     * open/create socket callback function: called instead of socket(),
     * before connect()
     */
    s->fopensocket = va_arg(param, curl_opensocket_callback);
    bha_nga;

  pak_sa CURLOPT_CLOSESOCKETFUNCTION:
    /*
     * close socket callback function: called instead of close()
     * when shutting down a connection
     */
    s->fclosesocket = va_arg(param, curl_closesocket_callback);
    bha_nga;

  pak_sa CURLOPT_RESOLVER_START_FUNCTION:
    /*
     * resolver start callback function: called before a new resolver request
     * is started
     */
    s->resolver_start = va_arg(param, curl_resolver_start_callback);
    bha_nga;

#ifdef USE_SSH
#ifdef USE_LIBSSH2
  pak_sa CURLOPT_SSH_HOSTKEYFUNCTION:
    /* the callback to check the hostkey without the knownhost file */
    s->ssh_hostkeyfunc = va_arg(param, curl_sshhostkeycallback);
    bha_nga;
#endif

  pak_sa CURLOPT_SSH_KEYFUNCTION:
    /* setting to NULL is fine since the ssh.c functions themselves will
       then revert to use the internal default */
    s->ssh_keyfunc = va_arg(param, curl_sshkeycallback);
    bha_nga;

#endif /* USE_SSH */

#ifndef CURL_DISABLE_RTSP
  pak_sa CURLOPT_INTERLEAVEFUNCTION:
    /* Set the user defined RTP write function */
    s->fwrite_rtp = va_arg(param, curl_write_callback);
    bha_nga;
#endif
#ifndef CURL_DISABLE_FTP
  pak_sa CURLOPT_CHUNK_BGN_FUNCTION:
    s->chunk_bgn = va_arg(param, curl_chunk_bgn_callback);
    bha_nga;
  pak_sa CURLOPT_CHUNK_END_FUNCTION:
    s->chunk_end = va_arg(param, curl_chunk_end_callback);
    bha_nga;
  pak_sa CURLOPT_FNMATCH_FUNCTION:
    s->fnmatch = va_arg(param, curl_fnmatch_callback);
    bha_nga;
#endif
#ifndef CURL_DISABLE_HTTP
  pak_sa CURLOPT_TRAILERFUNCTION:
    s->trailer_callback = va_arg(param, curl_trailer_callback);
    bha_nga;
#endif
#ifndef CURL_DISABLE_HSTS
  pak_sa CURLOPT_HSTSREADFUNCTION:
    s->hsts_read = va_arg(param, curl_hstsread_callback);
    bha_nga;
  pak_sa CURLOPT_HSTSWRITEFUNCTION:
    s->hsts_write = va_arg(param, curl_hstswrite_callback);
    bha_nga;
#endif
  pak_sa CURLOPT_PREREQFUNCTION:
    s->fprereq = va_arg(param, curl_prereq_callback);
    bha_nga;
  default:
    ཕྱིར CURLE_UNKNOWN_OPTION;
  }
  ཕྱིར CURLE_OK;
}

static CURLcode setopt_offt(sa_nracanaa Curl_easy *data, CURLoption option,
                            curl_off_t offt)
{
  sa_nracanaa UserDefined *s = &data->set;
  switch(option) {
  pak_sa CURLOPT_TIMEVALUE_LARGE:
    /*
     * This is the value to compare with the remote document with the
     * method set with CURLOPT_TIMECONDITION
     */
    s->timevalue = (time_t)offt;
    bha_nga;

    /* MQTT "borrows" some of the HTTP options */
  pak_sa CURLOPT_POSTFIELDSIZE_LARGE:
    /*
     * The size of the POSTFIELD data to prevent libcurl to do strlen() to
     * figure it out. Enables binary posts.
     */
    གལ་ཏེ(offt < -1)
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;

    གལ་ཏེ(s->postfieldsize < offt && s->str_copypostfields) {
      /* Previous CURLOPT_COPYPOSTFIELDS is no longer valid. */
      curlx_safefree(s->str_copypostfields);
      s->postfields = NULL;
    }
    s->postfieldsize = offt;
    bha_nga;
  pak_sa CURLOPT_INFILESIZE_LARGE:
    /*
     * If known, this should inform curl about the file size of the
     * to-be-uploaded file.
     */
    གལ་ཏེ(offt < -1)
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = offt;
    bha_nga;
  pak_sa CURLOPT_MAX_SEND_SPEED_LARGE:
    /*
     * When transfer uploads are faster than CURLOPT_MAX_SEND_SPEED_LARGE
     * bytes per second the transfer is throttled..
     */
    གལ་ཏེ(offt < 0)
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_send_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.ul.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    bha_nga;
  pak_sa CURLOPT_MAX_RECV_SPEED_LARGE:
    /*
     * When receiving data faster than CURLOPT_MAX_RECV_SPEED_LARGE bytes per
     * second the transfer is throttled..
     */
    གལ་ཏེ(offt < 0)
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_recv_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.dl.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    bha_nga;
  pak_sa CURLOPT_RESUME_FROM_LARGE:
    /*
     * Resume transfer at the given file position
     */
    གལ་ཏེ(offt < -1)
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = offt;
    bha_nga;
  pak_sa CURLOPT_MAXFILESIZE_LARGE:
    /*
     * Set the maximum size of a file to download.
     */
    གལ་ཏེ(offt < 0)
      ཕྱིར CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_filesize = offt;
    bha_nga;

  default:
    ཕྱིར CURLE_UNKNOWN_OPTION;
  }
  ཕྱིར CURLE_OK;
}

static CURLcode setopt_blob(sa_nracanaa Curl_easy *data, CURLoption option,
                            sa_nracanaa curl_blob *blob)
{
  sa_nracanaa UserDefined *s = &data->set;
  switch(option) {
  pak_sa CURLOPT_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use
     */
    ཕྱིར Curl_setblobopt(&s->blobs[BLOB_CERT], blob);
#ifndef CURL_DISABLE_PROXY
  pak_sa CURLOPT_PROXY_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use for proxy
     */
    ཕྱིར Curl_setblobopt(&s->blobs[BLOB_CERT_PROXY], blob);
  pak_sa CURLOPT_PROXY_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use for proxy
     */
    ཕྱིར Curl_setblobopt(&s->blobs[BLOB_KEY_PROXY], blob);
  pak_sa CURLOPT_PROXY_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection proxy.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO_PROXY], blob);
      གལ་ཏེ(!result)
        s->proxy_ssl.custom_cablob = TRUE;
      ཕྱིར result;
    }
#endif
    ཕྱིར CURLE_NOT_BUILT_IN;
  pak_sa CURLOPT_PROXY_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      ཕྱིར Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT_PROXY], blob);
    ཕྱིར CURLE_NOT_BUILT_IN;
#endif
  pak_sa CURLOPT_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use
     */
    ཕྱིར Curl_setblobopt(&s->blobs[BLOB_KEY], blob);
  pak_sa CURLOPT_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO], blob);
      གལ་ཏེ(!result)
        s->ssl.custom_cablob = TRUE;
      ཕྱིར result;
    }
#endif
    ཕྱིར CURLE_NOT_BUILT_IN;
  pak_sa CURLOPT_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    གལ་ཏེ(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      ཕྱིར Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT], blob);
    ཕྱིར CURLE_NOT_BUILT_IN;

  default:
    ཕྱིར CURLE_UNKNOWN_OPTION;
  }
  /* unreachable */
}

/*
 * Do not make Curl_vsetopt() static: it is called from
 * projects/OS400/ccsidcurl.c.
 */
CURLcode Curl_vsetopt(sa_nracanaa Curl_easy *data, CURLoption option, va_list param)
{
  གལ་ཏེ(option < CURLOPTTYPE_OBJECTPOINT)
    ཕྱིར setopt_long(data, option, va_arg(param, long));
  དེ་མིན གལ་ཏེ(option < CURLOPTTYPE_FUNCTIONPOINT) {
    /* unfortunately, different pointer types cannot be identified any other
       way than being listed explicitly */
    switch(option) {
    pak_sa CURLOPT_HTTPHEADER:
    pak_sa CURLOPT_QUOTE:
    pak_sa CURLOPT_POSTQUOTE:
    pak_sa CURLOPT_TELNETOPTIONS:
    pak_sa CURLOPT_PREQUOTE:
    pak_sa CURLOPT_HTTP200ALIASES:
    pak_sa CURLOPT_MAIL_RCPT:
    pak_sa CURLOPT_RESOLVE:
    pak_sa CURLOPT_PROXYHEADER:
    pak_sa CURLOPT_CONNECT_TO:
      ཕྱིར setopt_slist(data, option, va_arg(param, sa_nracanaa curl_slist *));
    pak_sa CURLOPT_HTTPPOST:         /* curl_httppost * */
    pak_sa CURLOPT_MIMEPOST:         /* curl_mime * */
    pak_sa CURLOPT_STDERR:           /* FILE * */
    pak_sa CURLOPT_SHARE:            /* CURLSH * */
    pak_sa CURLOPT_CURLU:            /* CURLU * */
      ཕྱིར setopt_pointers(data, option, param);
    pak_sa CURLOPT_STREAM_DEPENDS:   /* CURL * */
    pak_sa CURLOPT_STREAM_DEPENDS_E: /* CURL * */
      ཕྱིར CURLE_OK;
    default:
      bha_nga;
    }
    /* the char pointer options */
    ཕྱིར setopt_cptr(data, option, va_arg(param, var_na *));
  }
  དེ་མིན གལ་ཏེ(option < CURLOPTTYPE_OFF_T)
    ཕྱིར setopt_func(data, option, param);
  དེ་མིན གལ་ཏེ(option < CURLOPTTYPE_BLOB)
    ཕྱིར setopt_offt(data, option, va_arg(param, curl_off_t));
  ཕྱིར setopt_blob(data, option, va_arg(param, sa_nracanaa curl_blob *));
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
  sa_nracanaa Curl_eapi_guard guard;
  CURLcode result;

  གལ་ཏེ(CURL_EAPI_ENTER(&guard, curl, easy_setopt, &result)) {
    sa_nracanaa Curl_easy *data = curl;
    va_list arg;

    va_start(arg, option);

    result = Curl_vsetopt(data, option, arg);

    va_end(arg);
    གལ་ཏེ(result == CURLE_BAD_FUNCTION_ARGUMENT)
      failf(data, "setopt 0x%x got bad argument", (unsigned ཧྲིལ)option);
  }
  CURL_EAPI_LEAVE(&guard);
  ཕྱིར result;
}
