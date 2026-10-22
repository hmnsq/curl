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
#अन्तर्भाव "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#अन्तर्भाव <linux/tcp.h>
#नोचेद्यदि defined(HAVE_NETINET_TCP_H)
#अन्तर्भाव <netinet/tcp.h>
#endif

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "content_encoding.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "curl_share.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "setopt.h"
#अन्तर्भाव "altsvc.h"
#अन्तर्भाव "hsts.h"
#अन्तर्भाव "tftp.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "vauth/vauth.h"

स्थिर CURLcode setopt_set_timeout_sec(timediff_t *ptimeout_ms, long secs)
{
  यदि(secs < 0)
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
#यदि LONG_MAX > (TIMEDIFF_T_MAX / 1000)
  यदि(secs > (TIMEDIFF_T_MAX / 1000)) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    प्रत्यागम CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)secs * 1000;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode setopt_set_timeout_ms(timediff_t *ptimeout_ms, long ms)
{
  यदि(ms < 0)
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
#यदि LONG_MAX > TIMEDIFF_T_MAX
  यदि(ms > TIMEDIFF_T_MAX) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    प्रत्यागम CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)ms;
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_setstropt(संरचना Curl_easy *data,
                        गणना dupstring id, ध्रुव वर्ण *s)
{
  size_t slen = s ? strlen(s) : 0;
  DEBUGASSERT((unsigned)id <= UINT8_MAX);
  यदि(s && (slen > CURL_MAX_निवेश_LENGTH))
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

  प्रत्यागम CURL_EASY_STR_SET(data, (uint8_t)id, s, slen);
}

CURLcode Curl_setblobopt(संरचना curl_blob **blobp,
                         ध्रुव संरचना curl_blob *blob)
{
  /* free the previous storage at `blobp' and replace by a dynamic storage
     copy of blob. If CURL_BLOB_COPY is set, the data is copied. */

  curlx_safefree(*blobp);

  यदि(blob) {
    संरचना curl_blob *nblob;
    यदि(!blob->data || !blob->len || (blob->len > CURL_MAX_निवेश_LENGTH))
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    nblob = (संरचना curl_blob *)
      curlx_malloc(परिमाण(संरचना curl_blob) +
                   ((blob->flags & CURL_BLOB_COPY) ? blob->len : 0));
    यदि(!nblob)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    *nblob = *blob;
    यदि(blob->flags & CURL_BLOB_COPY) {
      /* put the data after the blob struct in memory */
      nblob->data = (वर्ण *)nblob + परिमाण(संरचना curl_blob);
      memcpy(nblob->data, blob->data, blob->len);
    }

    *blobp = nblob;
    प्रत्यागम CURLE_OK;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode setstropt_userpwd(ध्रुव वर्ण *option, वर्ण **userp,
                                  वर्ण **passwdp)
{
  वर्ण *user = रिक्त;
  वर्ण *passwd = रिक्त;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Parse the login details if specified. If not, then we treat NULL as a
     hint to clear the existing data */
  यदि(option) {
    size_t len = strlen(option);
    CURLcode result;
    यदि(len > CURL_MAX_निवेश_LENGTH)
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

    result = Curl_parse_login_details(option, len, &user, &passwd, रिक्त);
    यदि(result)
      प्रत्यागम result;
  }

  curlx_free(*userp);
  *userp = user;

  curlx_strzero(*passwdp);
  curlx_free(*passwdp);
  *passwdp = passwd;

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode setstropt_interface(संरचना Curl_easy *data, वर्ण *option)
{
  यदि(option && !option[0])
    option = रिक्त;
  यदि(option) {
    /* Parse for checking syntax */
    CURLcode result = Curl_parse_interface(option, रिक्त, रिक्त, रिक्त);
    यदि(result)
      प्रत्यागम result;
    प्रत्यागम CURL_EASY_STR_SET(data, STRING_INTERFACE_OPT,
                             option, strlen(option));
  }
  CURL_EASY_STR_CLEAR(data, STRING_INTERFACE_OPT);
  प्रत्यागम CURLE_OK;
}

#ifdef USE_SSL
#define C_SSLVERSION_मूल्य(x)     ((x) & 0xffff)
#define C_SSLVERSION_MAX_मूल्य(x) ((unsigned long)(x) & 0xffff0000)
#endif

स्थिर CURLcode protocol2num(ध्रुव वर्ण *str, curl_prot_t *val)
{
  /*
   * We are asked to cherry-pick protocols, so play it safe and disallow all
   * protocols to start with, and re-add the wanted ones back in.
   */
  *val = 0;

  यदि(!str)
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

  यदि(curl_strequal(str, "all")) {
    *val = ~(curl_prot_t)0;
    प्रत्यागम CURLE_OK;
  }

  कुरु {
    ध्रुव वर्ण *token = str;
    size_t tlen;

    str = strchr(str, ',');
    tlen = str ? (size_t)(str - token) : strlen(token);
    यदि(tlen) {
      ध्रुव संरचना Curl_scheme *h = Curl_getn_scheme(token, tlen);

      यदि(!h || !h->run)
        प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;

      *val |= h->protocol;
    }
  } यावत्(str && str++);

  यदि(!*val)
    /* no protocol listed */
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  प्रत्यागम CURLE_OK;
}

#यदि !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
स्थिर CURLcode httpauth(संरचना Curl_easy *data, bool proxy,
                         unsigned long auth)
{
  यदि(auth != CURLAUTH_NONE) {
    पूर्णाङ्क bitcheck = 0;
    bool authbits = FALSE;
    यदि(auth & CURLAUTH_DIGEST_IE) {
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
    यावत्(bitcheck < 31) {
      यदि(auth & (1UL << bitcheck++)) {
        authbits = TRUE;
        भङ्ग;
      }
    }
    यदि(!authbits)
      प्रत्यागम CURLE_NOT_BUILT_IN; /* no supported types left! */
  }
  यदि(proxy)
    data->set.proxyauth = (uint32_t)auth;
  अन्यथा
    data->set.httpauth = (uint32_t)auth;
  प्रत्यागम CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */

#ifndef CURL_DISABLE_HTTP
स्थिर CURLcode setopt_HTTP_VERSION(संरचना Curl_easy *data, long arg)
{
  /*
   * This sets a requested HTTP version to be used. The value is one of
   * the listed enums in curl/curl.h.
   */
  विकल्प(arg) {
  पक्ष CURL_HTTP_VERSION_NONE:
    /* accepted */
    भङ्ग;
  पक्ष CURL_HTTP_VERSION_1_0:
  पक्ष CURL_HTTP_VERSION_1_1:
    /* accepted */
    भङ्ग;
#ifdef USE_HTTP2
  पक्ष CURL_HTTP_VERSION_2_0:
  पक्ष CURL_HTTP_VERSION_2TLS:
  पक्ष CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    /* accepted */
    भङ्ग;
#endif
#ifdef USE_HTTP3
  पक्ष CURL_HTTP_VERSION_3:
  पक्ष CURL_HTTP_VERSION_3ONLY:
    /* accepted */
    भङ्ग;
#endif
  स्वभाव:
    /* not accepted */
    यदि(arg < CURL_HTTP_VERSION_NONE)
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
  }
  data->set.httpwant = (unsigned वर्ण)arg;
  प्रत्यागम CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP */

#ifdef USE_SSL
CURLcode Curl_setopt_SSLVERSION(संरचना Curl_easy *data, CURLoption option,
                                long arg)
{
  /*
   * Set explicit SSL version to try to connect with, as some SSL
   * implementations are lame.
   */
  {
    long version, version_max;
    संरचना ssl_easy_config *sslc = &data->set.ssl;
#ifndef CURL_DISABLE_PROXY
    यदि(option != CURLOPT_SSLVERSION)
      sslc = &data->set.proxy_ssl;
#अन्यथा
    (शून्य)option; /* unused */
#endif
    version = C_SSLVERSION_मूल्य(arg);
    version_max = (long)C_SSLVERSION_MAX_मूल्य(arg);

    यदि(version < CURL_SSLVERSION_DEFAULT ||
       version == CURL_SSLVERSION_SSLv2 ||
       version == CURL_SSLVERSION_SSLv3 ||
       version >= CURL_SSLVERSION_LAST ||
       version_max < CURL_SSLVERSION_MAX_NONE ||
       version_max >= CURL_SSLVERSION_MAX_LAST)
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    यदि(version == CURL_SSLVERSION_DEFAULT)
      version = CURL_SSLVERSION_TLSv1_2;

    sslc->version = (unsigned वर्ण)version;
    sslc->version_max = (unsigned पूर्णाङ्क)version_max;
  }
  प्रत्यागम CURLE_OK;
}
#endif /* !USE_SSL */

#ifndef CURL_DISABLE_RTSP
स्थिर CURLcode setopt_RTSP_REQUEST(संरचना Curl_easy *data, long arg)
{
  /*
   * Set the RTSP request method (OPTIONS, SETUP, PLAY, etc...) Would this be
   * better if the RTSPREQ_* were moved into here?
   */
  यदि((arg <= CURL_RTSPREQ_NONE) || (arg >= CURL_RTSPREQ_LAST))
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

  data->set.rtspreq = (unsigned वर्ण)arg;
  प्रत्यागम CURLE_OK;
}
#endif /* !CURL_DISABLE_RTSP */

स्थिर CURLcode setopt_long_bool(संरचना Curl_easy *data, CURLoption option,
                                 long arg)
{
  bool enabled = !!arg;
  पूर्णाङ्क ok = 1;
  संरचना UserDefined *s = &data->set;
  विकल्प(option) {
  पक्ष CURLOPT_FORBID_REUSE:
    /*
     * When this transfer is done, it must not be left to be reused by a
     * subsequent transfer but shall be closed immediately.
     */
    s->reuse_forbid = enabled;
    भङ्ग;
  पक्ष CURLOPT_FRESH_CONNECT:
    /*
     * This transfer shall not use a previously cached connection but
     * should be made with a fresh new connect!
     */
    s->reuse_fresh = enabled;
    भङ्ग;
  पक्ष CURLOPT_VERBOSE:
    /*
     * Verbose means infof() calls that give a lot of information about
     * the connection and transfer procedures as well as internal choices.
     */
    s->verbose = enabled;
    भङ्ग;
  पक्ष CURLOPT_HEADER:
    /*
     * Set to include the header in the general data output stream.
     */
    s->include_header = enabled;
    भङ्ग;
  पक्ष CURLOPT_NOPROGRESS:
    /*
     * Shut off the internal supported progress meter
     */
    data->progress.hide = enabled;
    भङ्ग;
  पक्ष CURLOPT_NOBODY:
    /*
     * Do not include the body part in the output data stream.
     */
    s->opt_no_body = enabled;
#ifndef CURL_DISABLE_HTTP
    यदि(s->opt_no_body)
      /* in HTTP lingo, no body means using the HEAD request... */
      s->method = HTTPREQ_HEAD;
    अन्यथा यदि(s->method == HTTPREQ_HEAD)
      s->method = HTTPREQ_GET;
#endif
    भङ्ग;
  पक्ष CURLOPT_FAILONERROR:
    /*
     * Do not output the >=400 error code HTML-page, but instead only
     * return error.
     */
    s->http_fail_on_error = enabled;
    भङ्ग;
  पक्ष CURLOPT_KEEP_SENDING_ON_ERROR:
    s->http_keep_sending_on_error = enabled;
    भङ्ग;
  पक्ष CURLOPT_UPLOAD:
  पक्ष CURLOPT_PUT:
    /*
     * We want to send data to the remote host. If this is HTTP, that equals
     * using the PUT request.
     */
    यदि(enabled) {
      /* If this is HTTP, PUT is what's needed to "upload" */
      s->method = HTTPREQ_PUT;
      s->opt_no_body = FALSE; /* this is implied */
    }
    अन्यथा
      /* In HTTP, the opposite of upload is GET (unless NOBODY is true as
         then this can be changed to HEAD later on) */
      s->method = HTTPREQ_GET;
    भङ्ग;
  पक्ष CURLOPT_FILETIME:
    /*
     * Try to get the file time of the remote document. The time will
     * later (possibly) become available using curl_easy_getinfo().
     */
    s->get_filetime = enabled;
    भङ्ग;
#ifndef CURL_DISABLE_HTTP
  पक्ष CURLOPT_HTTP09_ALLOWED:
    s->http09_allowed = enabled;
    भङ्ग;
#ifndef CURL_DISABLE_COOKIES
  पक्ष CURLOPT_COOKIESESSION:
    /*
     * Set this option to TRUE to start a new "cookie session". It will
     * prevent the forthcoming read-cookies-from-file actions to accept
     * cookies that are marked as being session cookies, as they belong to a
     * previous session.
     */
    s->cookiesession = enabled;
    भङ्ग;
#endif
  पक्ष CURLOPT_AUTOREFERER:
    /*
     * Switch on automatic referer that gets set if curl follows locations.
     */
    s->http_auto_referer = enabled;
    भङ्ग;
  पक्ष CURLOPT_TRANSFER_ENCODING:
    s->http_transfer_encoding = enabled;
    भङ्ग;
  पक्ष CURLOPT_UNRESTRICTED_AUTH:
    /*
     * Send authentication (user+password) when following locations, even when
     * hostname changed.
     */
    s->allow_auth_to_other_hosts = enabled;
    भङ्ग;
  पक्ष CURLOPT_HTTP_TRANSFER_DECODING:
    /*
     * disable libcurl transfer encoding is used
     */
    s->http_te_skip = !enabled; /* reversed */
    भङ्ग;
  पक्ष CURLOPT_HTTP_CONTENT_DECODING:
    /*
     * raw data passed to the application when content encoding is used
     */
    s->http_ce_skip = !enabled; /* reversed */
    भङ्ग;
  पक्ष CURLOPT_HTTPGET:
    /*
     * Set to force us do HTTP GET
     */
    यदि(enabled) {
      s->method = HTTPREQ_GET;
      s->opt_no_body = FALSE; /* this is implied */
    }
    भङ्ग;
  पक्ष CURLOPT_POST:
    /* Does this option serve a purpose anymore? Yes it does, when
       CURLOPT_POSTFIELDS is not used and the POST data is read off the
       callback! */
    यदि(enabled) {
      s->method = HTTPREQ_POST;
      s->opt_no_body = FALSE; /* this is implied */
    }
    अन्यथा
      s->method = HTTPREQ_GET;
    भङ्ग;
#endif /* !CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_PROXY
  पक्ष CURLOPT_HTTPPROXYTUNNEL:
    /*
     * Tunnel operations through the proxy instead of normal proxy use
     */
    s->tunnel_thru_httpproxy = enabled;
    भङ्ग;
  पक्ष CURLOPT_HAPROXYPROTOCOL:
    /*
     * Set to send the HAProxy Proxy Protocol header
     */
    s->haproxyprotocol = enabled;
    भङ्ग;
  पक्ष CURLOPT_PROXY_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for proxy.
     */
    s->proxy_ssl.verifypeer = enabled;

    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    भङ्ग;
  पक्ष CURLOPT_PROXY_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for proxy
     */
    s->proxy_ssl.verifyhost = enabled;
    ok = 2;
    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    भङ्ग;
  पक्ष CURLOPT_PROXY_TRANSFER_MODE:
    /*
     * set transfer mode (;type=<a|i>) when doing FTP via an HTTP proxy
     */
    s->proxy_transfer_mode = enabled;
    भङ्ग;
#endif /* !CURL_DISABLE_PROXY */
#यदि defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  पक्ष CURLOPT_SOCKS5_GSSAPI_NEC:
    /*
     * Set flag for NEC SOCKS5 support
     */
    s->socks5_gssapi_nec = enabled;
    भङ्ग;
#endif
#ifdef CURL_LIST_ONLY_PROTOCOL
  पक्ष CURLOPT_DIRLISTONLY:
    /*
     * An option that changes the command to one that asks for a list only, no
     * file info details. Used for FTP, POP3 and SFTP.
     */
    s->list_only = enabled;
    भङ्ग;
#endif
  पक्ष CURLOPT_APPEND:
    /*
     * We want to upload and append to an existing file. Used for FTP and
     * SFTP.
     */
    s->remote_append = enabled;
    भङ्ग;
#ifndef CURL_DISABLE_FTP
  पक्ष CURLOPT_FTP_USE_EPRT:
    s->ftp_use_eprt = enabled;
    भङ्ग;
  पक्ष CURLOPT_FTP_USE_EPSV:
    s->ftp_use_epsv = enabled;
    भङ्ग;
  पक्ष CURLOPT_FTP_USE_PRET:
    s->ftp_use_pret = enabled;
    भङ्ग;
  पक्ष CURLOPT_FTP_SKIP_PASV_IP:
    /*
     * Enable or disable FTP_SKIP_PASV_IP, which will disable/enable the
     * bypass of the IP address in PASV responses.
     */
    s->ftp_skip_ip = enabled;
    भङ्ग;
  पक्ष CURLOPT_WILDCARDMATCH:
    s->wildcard_enabled = enabled;
    भङ्ग;
#endif
  पक्ष CURLOPT_CRLF:
    /*
     * Kludgy option to enable CRLF conversions. Subject for removal.
     */
    s->crlf = enabled;
    भङ्ग;
#ifndef CURL_DISABLE_TFTP
  पक्ष CURLOPT_TFTP_NO_OPTIONS:
    /*
     * Option that prevents libcurl from sending TFTP option requests to the
     * server.
     */
    s->tftp_no_options = enabled;
    भङ्ग;
#endif /* !CURL_DISABLE_TFTP */
  पक्ष CURLOPT_TRANSFERTEXT:
    /*
     * This option was previously named 'FTPASCII'. Renamed to work with
     * more protocols than merely FTP.
     *
     * Transfer using ASCII (instead of BINARY).
     */
    s->prefer_ascii = enabled;
    भङ्ग;
  पक्ष CURLOPT_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying.
     */
    s->ssl.verifypeer = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    भङ्ग;
#ifndef CURL_DISABLE_DOH
  पक्ष CURLOPT_DOH_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for DoH.
     */
    s->doh_verifypeer = enabled;
    भङ्ग;
  पक्ष CURLOPT_DOH_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for DoH
     */
    s->doh_verifyhost = enabled;
    ok = 2;
    भङ्ग;
  पक्ष CURLOPT_DOH_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying for DoH.
     */
    यदि(!Curl_ssl_cert_status_request())
      प्रत्यागम CURLE_NOT_BUILT_IN;

    s->doh_verifystatus = enabled;
    ok = 2;
    भङ्ग;
#endif /* !CURL_DISABLE_DOH */
  पक्ष CURLOPT_SSL_VERIFYHOST:
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
    भङ्ग;
  पक्ष CURLOPT_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying.
     */
    यदि(!Curl_ssl_cert_status_request())
      प्रत्यागम CURLE_NOT_BUILT_IN;

    s->ssl.verifystatus = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    भङ्ग;
  पक्ष CURLOPT_CERTINFO:
#ifdef USE_SSL
    यदि(Curl_ssl_supports(data, SSLSUPP_CERTINFO))
      s->ssl.certinfo = enabled;
    अन्यथा
#endif
      प्रत्यागम CURLE_NOT_BUILT_IN;
    भङ्ग;
  पक्ष CURLOPT_NOSIGNAL:
    /*
     * The application asks not to set any signal() or alarm() handlers,
     * even when using a timeout.
     */
    s->no_signal = enabled;
    भङ्ग;
  पक्ष CURLOPT_TCP_NODELAY:
    /*
     * Enable or disable TCP_NODELAY, which will disable/enable the Nagle
     * algorithm
     */
    s->tcp_nodelay = enabled;
    भङ्ग;
  पक्ष CURLOPT_IGNORE_CONTENT_LENGTH:
    s->ignorecl = enabled;
    भङ्ग;
  पक्ष CURLOPT_SSL_SESSIONID_CACHE:
    s->ssl.cache_session = enabled;
#ifndef CURL_DISABLE_PROXY
    s->proxy_ssl.cache_session = s->ssl.cache_session;
#endif
    भङ्ग;
#ifdef USE_SSH
  पक्ष CURLOPT_SSH_COMPRESSION:
    s->ssh_compression = enabled;
    भङ्ग;
#endif /* !USE_SSH */
#ifndef CURL_DISABLE_SMTP
  पक्ष CURLOPT_MAIL_RCPT_ALLOWFAILS:
    /* allow RCPT TO command to fail for some recipients */
    s->mail_rcpt_allowfails = enabled;
    भङ्ग;
#endif /* !CURL_DISABLE_SMTP */
  पक्ष CURLOPT_SASL_IR:
    /* Enable/disable SASL initial response */
    s->sasl_ir = enabled;
    भङ्ग;
  पक्ष CURLOPT_TCP_KEEPALIVE:
    s->tcp_keepalive = enabled;
    भङ्ग;
  पक्ष CURLOPT_TCP_FASTOPEN:
#यदि defined(CONNECT_DATA_IDEMPOTENT) || defined(MSG_FASTOPEN) || \
  defined(TCP_FASTOPEN_CONNECT)
    s->tcp_fastopen = enabled;
    भङ्ग;
#अन्यथा
    प्रत्यागम CURLE_NOT_BUILT_IN;
#endif
  पक्ष CURLOPT_SSL_ENABLE_ALPN:
    s->ssl_enable_alpn = enabled;
    भङ्ग;
  पक्ष CURLOPT_PATH_AS_IS:
    s->path_as_is = enabled;
    भङ्ग;
  पक्ष CURLOPT_PIPEWAIT:
    s->pipewait = enabled;
    भङ्ग;
  पक्ष CURLOPT_SUPPRESS_CONNECT_HEADERS:
    s->suppress_connect_headers = enabled;
    भङ्ग;
#ifndef CURL_DISABLE_SHUFFLE_DNS
  पक्ष CURLOPT_DNS_SHUFFLE_ADDRESSES:
    s->dns_shuffle_addresses = enabled;
    भङ्ग;
#endif
  पक्ष CURLOPT_DISALLOW_USERNAME_IN_URL:
    s->disallow_username_in_url = enabled;
    भङ्ग;
  पक्ष CURLOPT_QUICK_EXIT:
    s->quick_exit = enabled;
    भङ्ग;
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }
  यदि((arg > ok) || (arg < 0))
    /* reserve other values for future use */
    infof(data, "boolean setopt(%d) got unsupported argument %ld,"
          " treated as %d", (पूर्णाङ्क)option, arg, enabled);

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode मूल्य_range(long *मूल्य, long below_error, long min, long max)
{
  यदि(*मूल्य < below_error)
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  अन्यथा यदि(*मूल्य < min)
    *मूल्य = min;
  अन्यथा यदि(*मूल्य > max)
    *मूल्य = max;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode setopt_long_net(संरचना Curl_easy *data, CURLoption option,
                                long arg)
{
  CURLcode result = CURLE_OK;
  संरचना UserDefined *s = &data->set;

  विकल्प(option) {
  पक्ष CURLOPT_DNS_CACHE_TIMEOUT:
    यदि(arg != -1)
      प्रत्यागम setopt_set_timeout_sec(&s->dns_cache_timeout_ms, arg);
    s->dns_cache_timeout_ms = -1;
    भङ्ग;
  पक्ष CURLOPT_MAXCONNECTS:
    result = मूल्य_range(&arg, 0, 0, INT_MAX);
    यदि(!result)
      s->maxconnects = arg ? (uint32_t)arg : DEFAULT_CONNCACHE_SIZE;
    भङ्ग;
  पक्ष CURLOPT_SERVER_RESPONSE_TIMEOUT:
    प्रत्यागम setopt_set_timeout_sec(&s->server_response_timeout, arg);
  पक्ष CURLOPT_SERVER_RESPONSE_TIMEOUT_MS:
    प्रत्यागम setopt_set_timeout_ms(&s->server_response_timeout, arg);
  पक्ष CURLOPT_निचला_SPEED_LIMIT:
    यदि(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->निचला_speed_limit = arg;
    भङ्ग;
  पक्ष CURLOPT_निचला_SPEED_TIME:
    result = मूल्य_range(&arg, 0, 0, USHRT_MAX);
    यदि(!result)
      s->निचला_speed_time = (uint16_t)arg;
    भङ्ग;
  पक्ष CURLOPT_PORT:
    यदि((arg < 0) || (arg > 65535))
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    s->use_port = (unsigned short)arg;
    भङ्ग;
  पक्ष CURLOPT_TIMEOUT:
    प्रत्यागम setopt_set_timeout_sec(&s->timeout, arg);
  पक्ष CURLOPT_TIMEOUT_MS:
    प्रत्यागम setopt_set_timeout_ms(&s->timeout, arg);
  पक्ष CURLOPT_CONNECTTIMEOUT:
    प्रत्यागम setopt_set_timeout_sec(&s->connecttimeout, arg);
  पक्ष CURLOPT_CONNECTTIMEOUT_MS:
    प्रत्यागम setopt_set_timeout_ms(&s->connecttimeout, arg);
#ifndef CURL_DISABLE_BINDLOCAL
  पक्ष CURLOPT_LOCALPORT:
    यदि((arg < 0) || (arg > 65535))
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    s->localport = curlx_sltous(arg);
    भङ्ग;
  पक्ष CURLOPT_LOCALPORTRANGE:
    यदि((arg < 0) || (arg > 65535))
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    s->localportrange = curlx_sltous(arg);
    भङ्ग;
#endif
  पक्ष CURLOPT_BUFFERSIZE:
    result = मूल्य_range(&arg, 0, READBUFFER_MIN, READBUFFER_MAX);
    यदि(!result)
      s->buffer_size = (unsigned पूर्णाङ्क)arg;
    भङ्ग;
  पक्ष CURLOPT_UPLOAD_BUFFERSIZE:
    result = मूल्य_range(&arg, 0, UPLOADBUFFER_MIN, UPLOADBUFFER_MAX);
    यदि(!result)
      s->upload_buffer_size = (unsigned पूर्णाङ्क)arg;
    भङ्ग;
  पक्ष CURLOPT_MAXFILESIZE:
    यदि(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->max_filesize = arg;
    भङ्ग;
  पक्ष CURLOPT_IPRESOLVE:
    यदि((arg < CURL_IPRESOLVE_WHATEVER) || (arg > CURL_IPRESOLVE_V6))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->ipver = (unsigned वर्ण)arg;
    भङ्ग;
  पक्ष CURLOPT_CONNECT_ONLY:
    यदि(arg < 0 || arg > 2)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा {
      s->connect_only = !!arg;
      s->connect_only_ws = (arg == 2);
    }
    भङ्ग;
#ifdef USE_IPV6
  पक्ष CURLOPT_ADDRESS_SCOPE:
#यदि SIZEOF_LONG > 4
    यदि((unsigned long)arg > UINT_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
#endif
    s->scope_id = (unsigned पूर्णाङ्क)arg;
    भङ्ग;
#endif
  पक्ष CURLOPT_TCP_KEEPIDLE:
    result = मूल्य_range(&arg, 0, 0, INT_MAX);
    यदि(!result)
      s->tcp_keepidle = (पूर्णाङ्क)arg;
    भङ्ग;
  पक्ष CURLOPT_TCP_KEEPINTVL:
    result = मूल्य_range(&arg, 0, 0, INT_MAX);
    यदि(!result)
      s->tcp_keepintvl = (पूर्णाङ्क)arg;
    भङ्ग;
  पक्ष CURLOPT_TCP_KEEPCNT:
    result = मूल्य_range(&arg, 0, 0, INT_MAX);
    यदि(!result)
      s->tcp_keepcnt = (पूर्णाङ्क)arg;
    भङ्ग;
  पक्ष CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
    प्रत्यागम setopt_set_timeout_ms(&s->happy_eyeballs_timeout, arg);
  पक्ष CURLOPT_UPKEEP_INTERVAL_MS:
    प्रत्यागम setopt_set_timeout_ms(&s->upkeep_interval_ms, arg);
  पक्ष CURLOPT_MAXAGE_CONN:
    प्रत्यागम setopt_set_timeout_sec(&s->conn_max_idle_ms, arg);
  पक्ष CURLOPT_MAXLIFETIME_CONN:
    प्रत्यागम setopt_set_timeout_sec(&s->conn_max_age_ms, arg);
  पक्ष CURLOPT_DNS_USE_GLOBAL_CACHE:
    /* deprecated */
    भङ्ग;
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }
  प्रत्यागम result;
}

स्थिर CURLcode setopt_long_ssl(संरचना Curl_easy *data, CURLoption option,
                                long arg)
{
#ifdef USE_SSL
  CURLcode result = CURLE_OK;
  संरचना UserDefined *s = &data->set;
  विकल्प(option) {
  पक्ष CURLOPT_CA_CACHE_TIMEOUT:
    यदि(Curl_ssl_supports(data, SSLSUPP_CA_CACHE)) {
      result = मूल्य_range(&arg, -1, -1, INT_MAX);
      यदि(!result)
        s->ssl_ca_cache_timeout = (पूर्णाङ्क)arg;
    }
    अन्यथा
      result = CURLE_NOT_BUILT_IN;
    भङ्ग;
  पक्ष CURLOPT_SSLVERSION:
#ifndef CURL_DISABLE_PROXY
  पक्ष CURLOPT_PROXY_SSLVERSION:
#endif
    प्रत्यागम Curl_setopt_SSLVERSION(data, option, arg);
  पक्ष CURLOPT_SSL_FALSESTART:
    result = CURLE_NOT_BUILT_IN;
    भङ्ग;
  पक्ष CURLOPT_USE_SSL:
    यदि((arg < CURLUSESSL_NONE) || (arg >= CURLUSESSL_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->use_ssl = (unsigned वर्ण)arg;
    भङ्ग;
  पक्ष CURLOPT_SSL_OPTIONS:
    s->ssl.ssl_options = (unsigned वर्ण)(arg & 0xff);
    भङ्ग;
#ifndef CURL_DISABLE_PROXY
  पक्ष CURLOPT_PROXY_SSL_OPTIONS:
    s->proxy_ssl.ssl_options = (unsigned वर्ण)(arg & 0xff);
    भङ्ग;
#endif
  पक्ष CURLOPT_SSL_ENABLE_NPN:
    भङ्ग;
  पक्ष CURLOPT_SSLENGINE_DEFAULT:
#ifdef USE_OPENSSL
    यदि(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      प्रत्यागम CURLE_NOT_BUILT_IN;
    data->set.engine_default = (arg == 1);
#अन्यथा
    प्रत्यागम CURLE_NOT_BUILT_IN;
#endif
    भङ्ग;
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }
  प्रत्यागम result;
#अन्यथा  /* USE_SSL */
  (शून्य)data;
  (शून्य)option;
  (शून्य)arg;
  प्रत्यागम CURLE_UNKNOWN_OPTION;
#endif /* !USE_SSL */
}

#ifndef CURL_DISABLE_PROXY
स्थिर शून्य changeproxy(संरचना Curl_easy *data)
{
  Curl_auth_digest_cleanup(&data->state.proxydigest);
  memset(&data->state.authproxy, 0, परिमाण(data->state.authproxy));
}

स्थिर CURLcode setopt_long_proxy(संरचना Curl_easy *data, CURLoption option,
                                  long arg)
{
  संरचना UserDefined *s = &data->set;

  विकल्प(option) {
  पक्ष CURLOPT_PROXYPORT:
    यदि((arg < 0) || (arg > UINT16_MAX))
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    यदि(arg != s->proxyport)
      changeproxy(data);
    s->proxyport = (uint16_t)arg;
    भङ्ग;
  पक्ष CURLOPT_PROXYAUTH:
    प्रत्यागम httpauth(data, TRUE, (unsigned long)arg);
  पक्ष CURLOPT_PROXYTYPE:
    यदि((arg < CURLPROXY_HTTP) || (arg > CURLPROXY_HTTPS3))
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef USE_PROXY_HTTP3
    यदि(arg == CURLPROXY_HTTPS3)
      प्रत्यागम CURLE_NOT_BUILT_IN;
#endif
    s->proxytype = (unsigned वर्ण)arg;
    भङ्ग;
  पक्ष CURLOPT_SOCKS5_AUTH: {
    unsigned long auth = (unsigned long)arg;
    यदि(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI | CURLAUTH_ONLY))
      प्रत्यागम CURLE_NOT_BUILT_IN;
    यदि((auth & CURLAUTH_ONLY) && !(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI)))
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    s->socks5auth = (unsigned वर्ण)(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI));
    s->socks5_auth_only = !!(auth & CURLAUTH_ONLY);
    भङ्ग;
  }
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }
  प्रत्यागम CURLE_OK;
}
#अन्यथा
स्थिर CURLcode setopt_long_proxy(संरचना Curl_easy *data, CURLoption option,
                                  long arg)
{
  (शून्य)data;
  (शून्य)option;
  (शून्य)arg;
  प्रत्यागम CURLE_UNKNOWN_OPTION;
}
#endif

स्थिर CURLcode setopt_long_http(संरचना Curl_easy *data, CURLoption option,
                                 long arg)
{
#ifndef CURL_DISABLE_HTTP
  CURLcode result = CURLE_OK;
  संरचना UserDefined *s = &data->set;

  विकल्प(option) {
  पक्ष CURLOPT_FOLLOWLOCATION:
    यदि((unsigned long)arg > 3)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->http_follow_mode = (unsigned वर्ण)arg;
    भङ्ग;
  पक्ष CURLOPT_MAXREDIRS:
    result = मूल्य_range(&arg, -1, -1, 0x7fff);
    यदि(!result)
      s->maxredirs = (short)arg;
    भङ्ग;
  पक्ष CURLOPT_POSTREDIR:
    यदि(arg < CURL_REDIR_GET_ALL)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा {
      s->post301 = !!(arg & CURL_REDIR_POST_301);
      s->post302 = !!(arg & CURL_REDIR_POST_302);
      s->post303 = !!(arg & CURL_REDIR_POST_303);
    }
    भङ्ग;
  पक्ष CURLOPT_HEADEROPT:
    s->sep_headers = !!(arg & CURLHEADER_SEPARATE);
    भङ्ग;
  पक्ष CURLOPT_HTTPAUTH:
    प्रत्यागम httpauth(data, FALSE, (unsigned long)arg);
  पक्ष CURLOPT_HTTP_VERSION:
    प्रत्यागम setopt_HTTP_VERSION(data, arg);
  पक्ष CURLOPT_EXPECT_100_TIMEOUT_MS:
    result = मूल्य_range(&arg, 0, 0, 0xffff);
    यदि(!result)
      s->expect_100_timeout = (unsigned short)arg;
    भङ्ग;
  पक्ष CURLOPT_STREAM_WEIGHT:
#यदि defined(USE_HTTP2) || defined(USE_HTTP3)
    यदि((arg >= 1) && (arg <= 256))
      s->weight = (पूर्णाङ्क)arg;
    भङ्ग;
#अन्यथा
    result = CURLE_NOT_BUILT_IN;
    भङ्ग;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  पक्ष CURLOPT_HTTPSIG_ALGORITHM:
    यदि(arg != CURLHTTPSIG_NONE &&
       arg != CURLHTTPSIG_ED25519 &&
       arg != CURLHTTPSIG_HMAC_SHA256)
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    s->httpsig_algorithm = (uint8_t)arg;
    यदि(arg)
      s->httpauth = (uint32_t)CURLAUTH_HTTPSIG;
    अन्यथा
      s->httpauth &= ~(uint32_t)CURLAUTH_HTTPSIG;
    भङ्ग;
#endif
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }
  प्रत्यागम result;
#अन्यथा
  (शून्य)data;
  (शून्य)option;
  (शून्य)arg;
  प्रत्यागम CURLE_UNKNOWN_OPTION;
#endif
}

स्थिर CURLcode setopt_long_proto(संरचना Curl_easy *data, CURLoption option,
                                  long arg)
{
  CURLcode result = CURLE_OK;
  संरचना UserDefined *s = &data->set;

  विकल्प(option) {
#ifndef CURL_DISABLE_TFTP
  पक्ष CURLOPT_TFTP_BLKSIZE:
    result = मूल्य_range(&arg, 0, TFTP_BLKSIZE_MIN, TFTP_BLKSIZE_MAX);
    यदि(!result)
      s->tftp_blksize = (unsigned short)arg;
    भङ्ग;
#endif
#ifndef CURL_DISABLE_NETRC
  पक्ष CURLOPT_NETRC:
    यदि((arg < CURL_NETRC_IGNORED) || (arg >= CURL_NETRC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->use_netrc = (unsigned वर्ण)arg;
    भङ्ग;
#endif
#ifndef CURL_DISABLE_FTP
  पक्ष CURLOPT_FTP_FILEMETHOD:
    यदि((arg < CURLFTPMETHOD_DEFAULT) || (arg >= CURLFTPMETHOD_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->ftp_filemethod = (unsigned वर्ण)arg;
    भङ्ग;
  पक्ष CURLOPT_FTP_SSL_CCC:
    यदि((arg < CURLFTPSSL_CCC_NONE) || (arg >= CURLFTPSSL_CCC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->ftp_ccc = (unsigned वर्ण)arg;
    भङ्ग;
  पक्ष CURLOPT_FTPSSLAUTH:
    यदि((arg < CURLFTPAUTH_DEFAULT) || (arg >= CURLFTPAUTH_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->ftpsslauth = (unsigned वर्ण)arg;
    भङ्ग;
  पक्ष CURLOPT_ACCEPTTIMEOUT_MS:
    प्रत्यागम setopt_set_timeout_ms(&s->accepttimeout, arg);
#endif
#यदि !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  पक्ष CURLOPT_FTP_CREATE_MISSING_DIRS:
    यदि((arg < CURLFTP_CREATE_DIR_NONE) || (arg > CURLFTP_CREATE_DIR_RETRY))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->ftp_create_missing_dirs = (unsigned वर्ण)arg;
    भङ्ग;
  पक्ष CURLOPT_NEW_FILE_PERMS:
    यदि((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->new_file_perms = (unsigned पूर्णाङ्क)arg;
    भङ्ग;
#endif
#ifndef CURL_DISABLE_RTSP
  पक्ष CURLOPT_RTSP_REQUEST:
    प्रत्यागम setopt_RTSP_REQUEST(data, arg);
  पक्ष CURLOPT_RTSP_CLIENT_CSEQ:
    result = मूल्य_range(&arg, 0, 0, INT_MAX);
    यदि(!result)
      data->state.rtsp_next_client_CSeq = (uint32_t)arg;
    भङ्ग;
  पक्ष CURLOPT_RTSP_SERVER_CSEQ:
    result = मूल्य_range(&arg, 0, 0, INT_MAX);
    यदि(!result)
      data->state.rtsp_next_server_CSeq = (uint32_t)arg;
    भङ्ग;
#endif
#ifdef USE_SSH
  पक्ष CURLOPT_SSH_AUTH_TYPES:
    s->ssh_auth_types = (uint32_t)arg;
    भङ्ग;
  पक्ष CURLOPT_NEW_DIRECTORY_PERMS:
    यदि((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->new_directory_perms = (unsigned पूर्णाङ्क)arg;
    भङ्ग;
#endif
  पक्ष CURLOPT_PROTOCOLS:
    s->allowed_protocols = (curl_prot_t)arg;
    भङ्ग;
  पक्ष CURLOPT_REDIR_PROTOCOLS:
    s->redir_protocols = (curl_prot_t)arg;
    भङ्ग;
#ifndef CURL_DISABLE_WEBSOCKETS
  पक्ष CURLOPT_WS_OPTIONS:
    s->ws_raw_mode = !!(arg & CURLWS_RAW_MODE);
    s->ws_no_auto_pong = !!(arg & CURLWS_NOAUTOPONG);
    भङ्ग;
#endif
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }
  प्रत्यागम result;
}

स्थिर CURLcode setopt_long_misc(संरचना Curl_easy *data, CURLoption option,
                                 long arg)
{
  संरचना UserDefined *s = &data->set;

  विकल्प(option) {
  पक्ष CURLOPT_TIMECONDITION:
    यदि((arg < CURL_TIMECOND_NONE) || (arg >= CURL_TIMECOND_LAST))
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    s->timecondition = (unsigned वर्ण)arg;
    भङ्ग;
  पक्ष CURLOPT_TIMEVALUE:
    s->timevalue = (time_t)arg;
    भङ्ग;
  पक्ष CURLOPT_POSTFIELDSIZE:
    यदि(arg < -1)
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    यदि(s->postfieldsize < arg && s->str_copypostfields) {
      curlx_safefree(s->str_copypostfields);
      s->postfields = रिक्त;
    }
    s->postfieldsize = arg;
    भङ्ग;
  पक्ष CURLOPT_INFILESIZE:
    यदि(arg < -1)
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = arg;
    भङ्ग;
  पक्ष CURLOPT_RESUME_FROM:
    यदि(arg < -1)
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = arg;
    भङ्ग;
  पक्ष CURLOPT_UPLOAD_FLAGS:
    s->upload_flags = (unsigned वर्ण)arg;
    भङ्ग;
#ifndef CURL_DISABLE_MIME
  पक्ष CURLOPT_MIME_OPTIONS:
    s->mime_formescape = !!(arg & CURLMIMEOPT_FORMESCAPE);
    भङ्ग;
#endif
#ifndef CURL_DISABLE_HSTS
  पक्ष CURLOPT_HSTS_CTRL:
    यदि(arg & CURLHSTS_ENABLE) {
      यदि(!data->hsts) {
        data->hsts = Curl_hsts_init();
        यदि(!data->hsts)
          प्रत्यागम CURLE_OUT_OF_MEMORY;
      }
    }
    अन्यथा यदि(!data->share || !data->share->hsts) {
      /* throw away the HSTS cache unless shared */
      Curl_hsts_cleanup(&data->hsts);
      /* flush all the entries */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = रिक्त;
    }
    अन्यथा
      /* detach from shared HSTS cache without freeing it */
      data->hsts = रिक्त;
    भङ्ग;
#endif
#ifndef CURL_DISABLE_ALTSVC
  पक्ष CURLOPT_ALTSVC_CTRL:
    प्रत्यागम Curl_altsvc_ctrl(data, arg);
#endif
#यदि defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  पक्ष CURLOPT_GSSAPI_DELEGATION:
    s->gssapi_delegation = (unsigned वर्ण)arg &
      (CURLGSSAPI_DELEGATION_POLICY_FLAG | CURLGSSAPI_DELEGATION_FLAG);
    भङ्ग;
#endif
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode setopt_long(संरचना Curl_easy *data, CURLoption option,
                            long arg)
{
  प्रकारनिर्देश CURLcode (*setoptfunc)(संरचना Curl_easy *data,
                                 CURLoption option, long arg);
  स्थिर ध्रुव setoptfunc setopt_call[] = {
    setopt_long_bool,
    setopt_long_net,
    setopt_long_http,
    setopt_long_proxy,
    setopt_long_ssl,
    setopt_long_proto,
    setopt_long_misc
  };
  size_t i;

  प्रति(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, arg);
    यदि(result != CURLE_UNKNOWN_OPTION)
      प्रत्यागम result;
  }
  प्रत्यागम CURLE_UNKNOWN_OPTION;
}

स्थिर CURLcode setopt_slist(संरचना Curl_easy *data, CURLoption option,
                             संरचना curl_slist *slist)
{
  CURLcode result = CURLE_OK;
  संरचना UserDefined *s = &data->set;
  विकल्प(option) {
#ifndef CURL_DISABLE_PROXY
  पक्ष CURLOPT_PROXYHEADER:
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
    भङ्ग;
#endif
#ifndef CURL_DISABLE_HTTP
  पक्ष CURLOPT_HTTP200ALIASES:
    /*
     * Set a list of aliases for HTTP 200 in response header
     */
    s->http200aliases = slist;
    भङ्ग;
#endif
#यदि !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  पक्ष CURLOPT_POSTQUOTE:
    /*
     * List of RAW FTP commands to use after a transfer
     */
    s->postquote = slist;
    भङ्ग;
  पक्ष CURLOPT_PREQUOTE:
    /*
     * List of RAW FTP commands to use prior to RETR (Wesley Laxton)
     */
    s->prequote = slist;
    भङ्ग;
  पक्ष CURLOPT_QUOTE:
    /*
     * List of RAW FTP commands to use before a transfer
     */
    s->quote = slist;
    भङ्ग;
#endif
  पक्ष CURLOPT_RESOLVE:
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
    भङ्ग;
#यदि !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MIME)
  पक्ष CURLOPT_HTTPHEADER:
    /*
     * Set a list with HTTP headers to use (or replace internals with)
     */
    s->headers = slist;
    भङ्ग;
#endif
#ifndef CURL_DISABLE_TELNET
  पक्ष CURLOPT_TELNETOPTIONS:
    /*
     * Set a linked list of telnet options
     */
    s->telnet_options = slist;
    भङ्ग;
#endif
#ifndef CURL_DISABLE_SMTP
  पक्ष CURLOPT_MAIL_RCPT:
    /* Set the list of mail recipients */
    s->mail_rcpt = slist;
    भङ्ग;
#endif
  पक्ष CURLOPT_CONNECT_TO:
    s->connect_to = slist;
    भङ्ग;
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }
  प्रत्यागम result;
}

#यदि !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
स्थिर CURLcode setopt_mimepost(संरचना Curl_easy *data, curl_mime *mimep)
{
  /*
   * Set to make us do MIME POST
   */
  CURLcode result;
  संरचना UserDefined *s = &data->set;
  यदि(!s->mimepostp) {
    s->mimepostp = curlx_malloc(परिमाण(*s->mimepostp));
    यदि(!s->mimepostp)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    Curl_mime_initpart(s->mimepostp);
  }

  result = Curl_mime_set_subparts(s->mimepostp, mimep, FALSE);
  यदि(!result) {
    s->method = HTTPREQ_POST_MIME;
    s->opt_no_body = FALSE; /* this is implied */
#ifndef CURL_DISABLE_FORM_API
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = रिक्त;
#endif
  }
  प्रत्यागम result;
}
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */

स्थिर CURLcode setopt_share(संरचना Curl_easy *data, संरचना Curl_share *set)
{
  CURLcode result;

  यदि(data->conn) {
    /* As this handle already has a connection attached, changing share now
       would be complicated and error-prone */
    infof(data, "Cannot change share object while in use");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
  }
  अन्यथा {
    /* disconnect from old share, if any and possible */
    result = Curl_share_easy_unlink(data);
    यदि(!result && GOOD_SHARE_HANDLE(set))
      /* use new share if it set */
      result = Curl_share_easy_link(data, set);
  }
  प्रत्यागम result;
}

/* assorted pointer type arguments */
स्थिर CURLcode setopt_pointers(संरचना Curl_easy *data, CURLoption option,
                                va_list param)
{
  CURLcode result = CURLE_OK;
  संरचना UserDefined *s = &data->set;
  विकल्प(option) {
  पक्ष CURLOPT_CURLU:
    /*
     * pass CURLU to set URL
     */
    Curl_bufref_free(&data->state.url);
    CURL_EASY_STR_CLEAR(data, STRING_SET_URL);
    s->uh = va_arg(param, CURLU *);
    भङ्ग;
#ifndef CURL_DISABLE_HTTP
#ifndef CURL_DISABLE_FORM_API
  पक्ष CURLOPT_HTTPPOST:
    /*
     * Set to make us do HTTP POST. Legacy API-style.
     */
    s->httppost = va_arg(param, संरचना curl_httppost *);
    s->method = HTTPREQ_POST_FORM;
    s->opt_no_body = FALSE; /* this is implied */
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = रिक्त;
    भङ्ग;
#endif /* !CURL_DISABLE_FORM_API */
#endif /* !CURL_DISABLE_HTTP */
#यदि !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
  पक्ष CURLOPT_MIMEPOST:
    result = setopt_mimepost(data, va_arg(param, curl_mime *));
    भङ्ग;
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */
  पक्ष CURLOPT_STDERR:
    /*
     * Set to a FILE * that should receive all error writes. This
     * defaults to stderr for normal operations.
     */
    s->err = va_arg(param, FILE *);
    यदि(!s->err)
      s->err = stderr;
    भङ्ग;
  पक्ष CURLOPT_SHARE:
    प्रत्यागम setopt_share(data, va_arg(param, संरचना Curl_share *));

  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }
  प्रत्यागम result;
}

#ifndef CURL_DISABLE_COOKIES
स्थिर CURLcode cookielist(संरचना Curl_easy *data, ध्रुव वर्ण *ptr)
{
  CURLcode result = CURLE_OK;
  यदि(!ptr)
    प्रत्यागम CURLE_OK;

  यदि(curl_strequal(ptr, "ALL")) {
    /* clear all cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearall(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  अन्यथा यदि(curl_strequal(ptr, "SESS")) {
    /* clear session cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearsess(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  अन्यथा यदि(curl_strequal(ptr, "FLUSH")) {
    /* flush cookies to file, takes care of the locking */
    Curl_flush_cookies(data, FALSE);
  }
  अन्यथा यदि(curl_strequal(ptr, "RELOAD")) {
    /* reload cookies from file */
    प्रत्यागम Curl_cookie_loadfiles(data, COOKIE_NOPSL |
                                 (data->set.cookiesession ?
                                  COOKIE_NOSESSION : 0));
  }
  अन्यथा {
    यदि(!data->cookies) {
      /* if cookie engine was not running, activate it */
      data->cookies = Curl_cookie_init();
      यदि(!data->cookies)
        प्रत्यागम CURLE_OUT_OF_MEMORY;
      data->state.cookie_engine = TRUE;
    }

    /* general protection against mistakes and abuse */
    यदि(strlen(ptr) > CURL_MAX_निवेश_LENGTH)
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

    /* Adding these cookies without the PSL check, because the PSL is not
       initialized until *perform() time, and this might be called before
       that */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    यदि(checkprefix("Set-Cookie:", ptr))
      /* HTTP Header format line */
      result = Curl_cookie_add(data, data->cookies, ptr + 11,
                               रिक्त, रिक्त,
                               COOKIE_HTTPHEADER | COOKIE_SECURE |
                               COOKIE_NOPSL);
    अन्यथा
      /* Netscape format line */
      result = Curl_cookie_add(data, data->cookies, ptr, रिक्त,
                               रिक्त, COOKIE_SECURE | COOKIE_NOPSL);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  प्रत्यागम result;
}

स्थिर CURLcode cookiefile(संरचना Curl_easy *data, ध्रुव वर्ण *ptr)
{
  /*
   * Set cookie file to read and parse. Can be used multiple times.
   */
  यदि(ptr) {
    संरचना curl_slist *cl;
    /* general protection against mistakes and abuse */
    यदि(strlen(ptr) > CURL_MAX_निवेश_LENGTH)
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    /* append the cookie filename to the list of filenames, and deal with
       them later */
    cl = curl_slist_append(data->state.cookielist, ptr);
    यदि(!cl) {
      curl_slist_free_all(data->state.cookielist);
      data->state.cookielist = रिक्त;
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    }
    data->state.cookielist = cl; /* store the list for later use */
  }
  अन्यथा {
    /* clear the list of cookie files */
    curl_slist_free_all(data->state.cookielist);
    data->state.cookielist = रिक्त;

    यदि(!data->share || !data->share->cookies) {
      /* throw away all existing cookies if this is not a shared cookie
         container */
      Curl_cookie_clearall(data->cookies);
      Curl_cookie_cleanup(data->cookies);
    }
    /* disable the cookie engine */
    data->cookies = रिक्त;
  }
  प्रत्यागम CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_PROXY
स्थिर CURLcode setproxy(संरचना Curl_easy *data, ध्रुव वर्ण *proxy)
{
  ध्रुव वर्ण *str = CURL_EASY_STR(data, STRING_PROXY);
  यदि(str && proxy &&
     /* there was one set, is this a new one? */
     !strcmp(str, proxy))
    प्रत्यागम CURLE_OK; /* same one as before */

  changeproxy(data);
  प्रत्यागम Curl_setstropt(data, STRING_PROXY, proxy);
}

स्थिर CURLcode setopt_cptr_proxy(संरचना Curl_easy *data, CURLoption option,
                                  वर्ण *ptr)
{
  CURLcode result = CURLE_OK;
  संरचना UserDefined *s = &data->set;
  विकल्प(option) {
  पक्ष CURLOPT_PROXYUSERPWD: {
    /*
     * user:password needed to use the proxy
     */
    वर्ण *u = रिक्त;
    वर्ण *p = रिक्त;
    result = setstropt_userpwd(ptr, &u, &p);

    /* URL decode the components */
    यदि(!result) {
      वर्ण *str = रिक्त;
      CURL_EASY_STR_CLEAR(data, STRING_PROXYUSERNAME);
      CURL_EASY_STR_CLEAR(data, STRING_PROXYPASSWORD);
      यदि(u) {
        result = Curl_urldecode(u, 0, &str, रिक्त, REJECT_ZERO);
        यदि(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYUSERNAME, str);
      }
      यदि(!result && p) {
        str = रिक्त;
        result = Curl_urldecode(p, 0, &str, रिक्त, REJECT_ZERO);
        यदि(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYPASSWORD, str);
      }
    }
    curlx_free(u);
    curlx_strzero(p);
    curlx_free(p);
    भङ्ग;
  }
  पक्ष CURLOPT_PROXYUSERNAME:
    /*
     * authentication username to use in the operation
     */
    प्रत्यागम Curl_setstropt(data, STRING_PROXYUSERNAME, ptr);

  पक्ष CURLOPT_PROXYPASSWORD:
    /*
     * authentication password to use in the operation
     */
    प्रत्यागम Curl_setstropt(data, STRING_PROXYPASSWORD, ptr);

  पक्ष CURLOPT_NOPROXY:
    /*
     * proxy exception list
     */
    प्रत्यागम Curl_setstropt(data, STRING_NOPROXY, ptr);
  पक्ष CURLOPT_PROXY_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use for proxy
     */
    प्रत्यागम Curl_setstropt(data, STRING_CERT_PROXY, ptr);
  पक्ष CURLOPT_PROXY_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use for proxy
     */
    प्रत्यागम Curl_setstropt(data, STRING_CERT_TYPE_PROXY, ptr);
  पक्ष CURLOPT_PROXY_SSLKEY:
    /*
     * String that holds filename of the SSL key to use for proxy
     */
    प्रत्यागम Curl_setstropt(data, STRING_कुंजी_PROXY, ptr);
  पक्ष CURLOPT_PROXY_KEYPASSWD:
    /*
     * String that holds the SSL private key password for proxy.
     */
    प्रत्यागम Curl_setstropt(data, STRING_कुंजी_PASSWD_PROXY, ptr);
  पक्ष CURLOPT_PROXY_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use for proxy
     */
    प्रत्यागम Curl_setstropt(data, STRING_कुंजी_TYPE_PROXY, ptr);
  पक्ष CURLOPT_PROXY_SSL_CIPHER_LIST:
    यदि(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST)) {
      /* set a list of cipher we want to use in the SSL connection for proxy */
      प्रत्यागम Curl_setstropt(data, STRING_SSL_CIPHER_LIST_PROXY, ptr);
    }
    अन्यथा
      प्रत्यागम CURLE_NOT_BUILT_IN;
  पक्ष CURLOPT_PROXY_TLS13_CIPHERS:
    यदि(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites for proxy */
      प्रत्यागम Curl_setstropt(data, STRING_SSL_CIPHER13_LIST_PROXY, ptr);
    अन्यथा
      प्रत्यागम CURLE_NOT_BUILT_IN;
  पक्ष CURLOPT_PROXY:
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
    प्रत्यागम setproxy(data, ptr);
  पक्ष CURLOPT_PRE_PROXY:
    /*
     * Set proxy server:port to use as SOCKS proxy.
     *
     * If the proxy is set to "" or NULL we explicitly say that we do not want
     * to use the socks proxy.
     */
    प्रत्यागम Curl_setstropt(data, STRING_PRE_PROXY, ptr);
  पक्ष CURLOPT_SOCKS5_GSSAPI_SERVICE:
  पक्ष CURLOPT_PROXY_SERVICE_NAME:
    /*
     * Set proxy authentication service name for Kerberos 5 and SPNEGO
     */
    प्रत्यागम Curl_setstropt(data, STRING_PROXY_SERVICE_NAME, ptr);
  पक्ष CURLOPT_PROXY_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
#ifdef USE_SSL
    यदि(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      प्रत्यागम Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY_PROXY, ptr);
#endif
    प्रत्यागम CURLE_NOT_BUILT_IN;

  पक्ष CURLOPT_HAPROXY_CLIENT_IP:
    /*
     * Set the client IP to send through HAProxy PROXY protocol
     */
    result = Curl_setstropt(data, STRING_HAPROXY_CLIENT_IP, ptr);

    /* enable the HAProxy protocol if an IP is provided */
    s->haproxyprotocol = !!CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
    भङ्ग;
  पक्ष CURLOPT_PROXY_CAINFO:
    /*
     * Set CA info SSL connection for proxy. Specify filename of the
     * CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE_PROXY, ptr);
    यदि(!result)
      s->proxy_ssl.custom_cafile = TRUE;
    प्रत्यागम result;
  पक्ष CURLOPT_PROXY_CRLFILE:
    /*
     * Set CRL file info for SSL connection for proxy. Specify filename of the
     * CRL to check certificates revocation
     */
    यदि(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      प्रत्यागम Curl_setstropt(data, STRING_SSL_CRLFILE_PROXY, ptr);
    प्रत्यागम CURLE_NOT_BUILT_IN;
  पक्ष CURLOPT_PROXY_ISSUERCERT:
    /*
     * Set Issuer certificate file to check certificates issuer
     */
    यदि(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      प्रत्यागम Curl_setstropt(data, STRING_SSL_ISSUERCERT_PROXY, ptr);
    प्रत्यागम CURLE_NOT_BUILT_IN;
  पक्ष CURLOPT_PROXY_CAPATH:
    /*
     * Set CA path info for SSL connection proxy. Specify directory name of the
     * CA certificates which have been prepared using openssl c_rehash utility.
     */
#ifdef USE_SSL
    यदि(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH_PROXY, ptr);
      यदि(!result)
        s->proxy_ssl.custom_capath = TRUE;
      प्रत्यागम result;
    }
#endif
    प्रत्यागम CURLE_NOT_BUILT_IN;
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }
  प्रत्यागम result;
}
#endif

#यदि !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
/*
 * A string with POST data. Makes curl HTTP POST. Even if it is NULL. If
 * needed, CURLOPT_POSTFIELDSIZE must have been set prior to
 * CURLOPT_COPYPOSTFIELDS and not altered later.
 */
स्थिर CURLcode setopt_copypostfields(ध्रुव वर्ण *ptr, संरचना UserDefined *s)
{
  CURLcode result = CURLE_OK;
  यदि(s->postfieldsize < -1)
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  यदि(!ptr || s->postfieldsize == -1) {
    यदि(ptr && (strlen(ptr) > CURL_MAX_निवेश_LENGTH))
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_safefree(s->str_copypostfields);
    यदि(ptr) {
      s->str_copypostfields = curlx_strdup(ptr);
      यदि(!s->str_copypostfields)
        प्रत्यागम CURLE_OUT_OF_MEMORY;
    }
  }
  अन्यथा {
    size_t pflen = curlx_sotouz_range(s->postfieldsize, 0, SIZE_MAX);
    यदि(pflen == SIZE_MAX)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    अन्यथा {
      /* Allocate even when size == 0. This satisfies the need of possible
         later address compare to detect the COPYPOSTFIELDS mode, and to mark
         that postfields is used rather than read function or form data. */
      वर्ण *p = curlx_memdup0(ptr, pflen);
      यदि(!p)
        प्रत्यागम CURLE_OUT_OF_MEMORY;
      अन्यथा {
        curlx_free(s->str_copypostfields);
        s->str_copypostfields = p;
      }
    }
  }

  s->postfields = s->str_copypostfields;
  s->method = HTTPREQ_POST;
  प्रत्यागम result;
}
#endif

#ifdef USE_ECH
स्थिर CURLcode setopt_ech(संरचना Curl_easy *data, ध्रुव वर्ण *ptr)
{
  संरचना UserDefined *s = &data->set;
  CURLcode result = CURLE_OK;

  यदि(!ptr || !strcmp(ptr, "false"))
    s->ssl.ech = CURLECH_DISABLE;
  अन्यथा {
    size_t plen = strlen(ptr);
    यदि(plen > CURL_MAX_निवेश_LENGTH)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा {
      यदि(!strcmp(ptr, "grease"))
        s->ssl.ech = CURLECH_GREASE;
      अन्यथा यदि(!strcmp(ptr, "true"))
        s->ssl.ech = CURLECH_ENABLE;
      अन्यथा यदि(!strcmp(ptr, "hard"))
        s->ssl.ech = CURLECH_HARD;
      अन्यथा यदि(plen > 4 && !strncmp(ptr, "ecl:", 4)) {
        यदि(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_CONFIG, ptr + 4);
      }
      अन्यथा यदि(plen > 3 && !strncmp(ptr, "pn:", 3)) {
        यदि(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_PUBLIC, ptr + 3);
      }
      अन्यथा
        result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  प्रत्यागम result;
}
#अन्यथा
#define setopt_ech(x, y) CURLE_NOT_BUILT_IN
#endif

#यदि defined(USE_SSL) || defined(USE_SSH)
/* One of the options is used for both TLS and SSH */
स्थिर CURLcode setopt_cptr_ssl(संरचना Curl_easy *data, CURLoption option,
                                वर्ण *ptr)
{
  CURLcode result = CURLE_OK;

  विकल्प(option) {
  पक्ष CURLOPT_KEYPASSWD:
    /*
     * String that holds the SSL or SSH private key password.
     */
    result = Curl_setstropt(data, STRING_कुंजी_PASSWD, ptr);
    भङ्ग;
#ifdef USE_SSL
  पक्ष CURLOPT_CAINFO:
    /*
     * Set CA info for SSL connection. Specify filename of the CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE, ptr);
    यदि(!result)
      data->set.ssl.custom_cafile = TRUE;
    प्रत्यागम result;
  पक्ष CURLOPT_CAPATH:
    /*
     * Set CA path info for SSL connection. Specify directory name of the CA
     * certificates which have been prepared using openssl c_rehash utility.
     */
    यदि(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH, ptr);
      यदि(!result)
        data->set.ssl.custom_capath = TRUE;
      प्रत्यागम result;
    }
    प्रत्यागम CURLE_NOT_BUILT_IN;
  पक्ष CURLOPT_CRLFILE:
    /*
     * Set CRL file info for SSL connection. Specify filename of the CRL
     * to check certificates revocation
     */
    यदि(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      प्रत्यागम Curl_setstropt(data, STRING_SSL_CRLFILE, ptr);
    प्रत्यागम CURLE_NOT_BUILT_IN;
  पक्ष CURLOPT_SSL_CIPHER_LIST:
    यदि(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST))
      /* set a list of cipher we want to use in the SSL connection */
      प्रत्यागम Curl_setstropt(data, STRING_SSL_CIPHER_LIST, ptr);
    अन्यथा
      प्रत्यागम CURLE_NOT_BUILT_IN;
  पक्ष CURLOPT_TLS13_CIPHERS:
    यदि(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites */
      प्रत्यागम Curl_setstropt(data, STRING_SSL_CIPHER13_LIST, ptr);
    अन्यथा
      प्रत्यागम CURLE_NOT_BUILT_IN;
  पक्ष CURLOPT_RANDOM_FILE:
    भङ्ग;
  पक्ष CURLOPT_EGDSOCKET:
    भङ्ग;
  पक्ष CURLOPT_SSL_CTX_DATA:
    /*
     * Set an SSL_CTX callback parameter pointer
     */
    यदि(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      data->set.ssl_fsslctxp = ptr;
      भङ्ग;
    }
    अन्यथा
      प्रत्यागम CURLE_NOT_BUILT_IN;
  पक्ष CURLOPT_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use
     */
    प्रत्यागम Curl_setstropt(data, STRING_CERT, ptr);
  पक्ष CURLOPT_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use
     */
    प्रत्यागम Curl_setstropt(data, STRING_CERT_TYPE, ptr);
  पक्ष CURLOPT_SSLKEY:
    /*
     * String that holds filename of the SSL key to use
     */
    प्रत्यागम Curl_setstropt(data, STRING_कुंजी, ptr);
  पक्ष CURLOPT_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use
     */
    प्रत्यागम Curl_setstropt(data, STRING_कुंजी_TYPE, ptr);
  पक्ष CURLOPT_SSLENGINE:
    /*
     * String that holds the SSL crypto engine.
     */
    यदि(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      प्रत्यागम CURLE_NOT_BUILT_IN;
    result = Curl_setstropt(data, STRING_SSL_ENGINE, ptr);
    भङ्ग;
  पक्ष CURLOPT_ISSUERCERT:
    /*
     * Set Issuer certificate file
     * to check certificates issuer
     */
    यदि(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      प्रत्यागम Curl_setstropt(data, STRING_SSL_ISSUERCERT, ptr);
    प्रत्यागम CURLE_NOT_BUILT_IN;
  पक्ष CURLOPT_SSL_EC_CURVES:
    /*
     * Set accepted curves in SSL connection setup.
     * Specify colon-delimited list of curve algorithm names.
     */
    यदि(Curl_ssl_supports(data, SSLSUPP_SSL_EC_CURVES))
      प्रत्यागम Curl_setstropt(data, STRING_SSL_EC_CURVES, ptr);
    प्रत्यागम CURLE_NOT_BUILT_IN;
  पक्ष CURLOPT_SSL_SIGNATURE_ALGORITHMS:
    /*
     * Set accepted signature algorithms.
     * Specify colon-delimited list of signature scheme names.
     */
    यदि(Curl_ssl_supports(data, SSLSUPP_SIGNATURE_ALGORITHMS))
      प्रत्यागम Curl_setstropt(data, STRING_SSL_SIGNATURE_ALGORITHMS, ptr);
    प्रत्यागम CURLE_NOT_BUILT_IN;
  पक्ष CURLOPT_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
    यदि(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      प्रत्यागम Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY, ptr);
    प्रत्यागम CURLE_NOT_BUILT_IN;
  पक्ष CURLOPT_ECH:
    प्रत्यागम setopt_ech(data, ptr);
#endif
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }
  प्रत्यागम result;
}
#endif

#यदि !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
स्थिर CURLcode setopt_cptr_http_mqtt(संरचना Curl_easy *data,
                                      CURLoption option, वर्ण *ptr)
{
  CURLcode result = CURLE_OK;
  संरचना UserDefined *s = &data->set;

  विकल्प(option) {
  पक्ष CURLOPT_COPYPOSTFIELDS:
    प्रत्यागम setopt_copypostfields(ptr, s);

  पक्ष CURLOPT_POSTFIELDS:
    /*
     * Like above, but use static data instead of copying it.
     */
    s->postfields = ptr;
    /* Release old copied data. */
    curlx_safefree(s->str_copypostfields);
    s->method = HTTPREQ_POST;
    भङ्ग;

#ifndef CURL_DISABLE_HTTP
  पक्ष CURLOPT_TRAILERDATA:
    s->trailer_data = ptr;
    भङ्ग;
  पक्ष CURLOPT_ACCEPT_ENCODING:
    /*
     * String to use at the value of Accept-Encoding header.
     *
     * If the encoding is set to "" we use an Accept-Encoding header that
     * encompasses all the encodings we support.
     * If the encoding is set to NULL we do not send an Accept-Encoding header
     * and ignore any received Content-Encoding header.
     *
     */
    यदि(ptr && !*ptr) {
      ptr = Curl_get_content_encodings();
      यदि(ptr)
        result = CURL_EASY_STR_SETN(data, STRING_ENCODING, ptr);
      अन्यथा
        result = CURLE_OUT_OF_MEMORY;
      प्रत्यागम result;
    }
    प्रत्यागम Curl_setstropt(data, STRING_ENCODING, ptr);

#ifndef CURL_DISABLE_AWS
  पक्ष CURLOPT_AWS_SIGV4:
    /*
     * String that is merged to some authentication
     * parameters are used by the algorithm.
     */
    result = Curl_setstropt(data, STRING_AWS_SIGV4, ptr);
    /*
     * Basic has been set by default; it needs to be unset here.
     */
    यदि(CURL_EASY_STR(data, STRING_AWS_SIGV4))
      s->httpauth = CURLAUTH_AWS_SIGV4;
    अन्यथा
      s->httpauth &= ~(uint32_t)CURLAUTH_AWS_SIGV4;
    भङ्ग;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  पक्ष CURLOPT_HTTPSIG_कुंजी:
    result = Curl_setstropt(data, STRING_HTTPSIG_कुंजी, ptr);
    भङ्ग;
  पक्ष CURLOPT_HTTPSIG_KEYID:
    result = Curl_setstropt(data, STRING_HTTPSIG_KEYID, ptr);
    भङ्ग;
  पक्ष CURLOPT_HTTPSIG_HEADERS:
    result = Curl_setstropt(data, STRING_HTTPSIG_HEADERS, ptr);
    भङ्ग;
#endif
  पक्ष CURLOPT_REFERER: {
    /*
     * String to set in the HTTP Referer: field.
     */
    संरचना bufref *oldref = &data->state.referer;
    /* free the old after the storing the new in case the input is actually
       pointing back to this */
    result = Curl_setstropt(data, STRING_SET_REFERER, ptr);
    Curl_bufref_free(oldref);
    भङ्ग;
  }

  पक्ष CURLOPT_USERAGENT:
    /*
     * String to use in the HTTP User-Agent field
     */
    प्रत्यागम Curl_setstropt(data, STRING_USERAGENT, ptr);

#ifndef CURL_DISABLE_COOKIES
  पक्ष CURLOPT_COOKIE:
    /*
     * Cookie string to send to the remote server in the request.
     */
    प्रत्यागम Curl_setstropt(data, STRING_COOKIE, ptr);

  पक्ष CURLOPT_COOKIEFILE:
    प्रत्यागम cookiefile(data, ptr);

  पक्ष CURLOPT_COOKIEJAR:
    /*
     * Set cookie filename to dump all cookies to when we are done.
     */
    result = Curl_setstropt(data, STRING_COOKIEJAR, ptr);
    यदि(!result) {
      /*
       * Activate the cookie parser. This may or may not already
       * have been made.
       */
      यदि(!data->cookies)
        data->cookies = Curl_cookie_init();
      यदि(!data->cookies)
        result = CURLE_OUT_OF_MEMORY;
      अन्यथा
        data->state.cookie_engine = TRUE;
    }
    भङ्ग;

  पक्ष CURLOPT_COOKIELIST:
    प्रत्यागम cookielist(data, ptr);
#endif /* !CURL_DISABLE_COOKIES */

#endif /* !CURL_DISABLE_HTTP */
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }
  प्रत्यागम result;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_MQTT */

#ifdef USE_SSH
स्थिर CURLcode setopt_cptr_ssh(संरचना Curl_easy *data, CURLoption option,
                                वर्ण *ptr)
{
  संरचना UserDefined *s = &data->set;
  विकल्प(option) {
  पक्ष CURLOPT_SSH_PUBLIC_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa.pub file
     */
    प्रत्यागम Curl_setstropt(data, STRING_SSH_PUBLIC_कुंजी, ptr);
  पक्ष CURLOPT_SSH_PRIVATE_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa file
     */
    प्रत्यागम Curl_setstropt(data, STRING_SSH_PRIVATE_कुंजी, ptr);
  पक्ष CURLOPT_SSH_KEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_keyfunc_userp = ptr;
    भङ्ग;
  पक्ष CURLOPT_SSH_HOST_PUBLIC_कुंजी_MD5:
    /*
     * Option to allow for the MD5 of the host public key to be checked
     * for validation purposes.
     */
    प्रत्यागम Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_कुंजी_MD5, ptr);
  पक्ष CURLOPT_SSH_HOST_PUBLIC_कुंजी_SHA256:
    /*
     * Option to allow for the SHA256 of the host public key to be checked
     * for validation purposes.
     */
    प्रत्यागम Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_कुंजी_SHA256, ptr);
  पक्ष CURLOPT_SSH_KNOWNHOSTS:
    /*
     * Store the filename to read known hosts from.
     */
    प्रत्यागम Curl_setstropt(data, STRING_SSH_KNOWNHOSTS, ptr);
#ifdef USE_LIBSSH2
  पक्ष CURLOPT_SSH_HOSTKEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_hostkeyfunc_userp = ptr;
    भङ्ग;
#endif /* USE_LIBSSH2 */
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }
  प्रत्यागम CURLE_OK;
}
#endif /* USE_SSH */

#ifndef CURL_DISABLE_FTP
स्थिर CURLcode setopt_cptr_ftp(संरचना Curl_easy *data, CURLoption option,
                                वर्ण *ptr)
{
  CURLcode result = CURLE_OK;
  संरचना UserDefined *s = &data->set;
  विकल्प(option) {
  पक्ष CURLOPT_FTPPORT:
    /*
     * Use FTP PORT, this also specifies which IP address to use
     */
    result = Curl_setstropt(data, STRING_FTPPORT, ptr);
    s->ftp_use_port = !!CURL_EASY_STR(data, STRING_FTPPORT);
    भङ्ग;

  पक्ष CURLOPT_FTP_ACCOUNT:
    प्रत्यागम Curl_setstropt(data, STRING_FTP_ACCOUNT, ptr);

  पक्ष CURLOPT_FTP_ALTERNATIVE_TO_USER:
    प्रत्यागम Curl_setstropt(data, STRING_FTP_ALTERNATIVE_TO_USER, ptr);

  पक्ष CURLOPT_KRBLEVEL:
    प्रत्यागम CURLE_NOT_BUILT_IN; /* removed in 8.17.0 */
  पक्ष CURLOPT_CHUNK_DATA:
    s->wildcardptr = ptr;
    भङ्ग;
  पक्ष CURLOPT_FNMATCH_DATA:
    s->fnmatch_data = ptr;
    भङ्ग;
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }
  प्रत्यागम result;
}
#endif /* !CURL_DISABLE_FTP */

स्थिर CURLcode setopt_cptr_net(संरचना Curl_easy *data, CURLoption option,
                                वर्ण *ptr)
{
  विकल्प(option) {
  पक्ष CURLOPT_INTERFACE:
    /*
     * Set what interface or address/hostname to bind the socket to when
     * performing an operation and thus what from-IP your connection will use.
     */
    प्रत्यागम setstropt_interface(data, ptr);

#ifdef USE_RESOLV_ARES
  पक्ष CURLOPT_DNS_SERVERS:
    प्रत्यागम Curl_setstropt(data, STRING_DNS_SERVERS, ptr);

  पक्ष CURLOPT_DNS_INTERFACE:
    प्रत्यागम Curl_setstropt(data, STRING_DNS_INTERFACE, ptr);

  पक्ष CURLOPT_DNS_LOCAL_IP4:
    प्रत्यागम Curl_setstropt(data, STRING_DNS_LOCAL_IP4, ptr);

  पक्ष CURLOPT_DNS_LOCAL_IP6:
    प्रत्यागम Curl_setstropt(data, STRING_DNS_LOCAL_IP6, ptr);
#endif
#ifdef USE_UNIX_SOCKETS
  पक्ष CURLOPT_UNIX_SOCKET_PATH:
    data->set.abstract_unix_socket = FALSE;
    प्रत्यागम Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);

  पक्ष CURLOPT_ABSTRACT_UNIX_SOCKET:
    data->set.abstract_unix_socket = TRUE;
    प्रत्यागम Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);
#endif
#ifndef CURL_DISABLE_DOH
  पक्ष CURLOPT_DOH_URL:
    {
      CURLcode result = Curl_setstropt(data, STRING_DOH, ptr);
      data->set.doh = !!CURL_EASY_STR(data, STRING_DOH);
      प्रत्यागम result;
    }
#endif
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }
}

स्थिर CURLcode setopt_cptr_misc(संरचना Curl_easy *data, CURLoption option,
                                 वर्ण *ptr)
{
  CURLcode result = CURLE_OK;
  संरचना UserDefined *s = &data->set;

  विकल्प(option) {
  पक्ष CURLOPT_REQUEST_TARGET:
    प्रत्यागम Curl_setstropt(data, STRING_TARGET, ptr);
#ifndef CURL_DISABLE_NETRC
  पक्ष CURLOPT_NETRC_FILE:
    प्रत्यागम Curl_setstropt(data, STRING_NETRC_FILE, ptr);
#endif
  पक्ष CURLOPT_CUSTOMREQUEST:
    प्रत्यागम Curl_setstropt(data, STRING_CUSTOMREQUEST, ptr);

    /* we do not set s->method = HTTPREQ_CUSTOM; here, we continue as if we
       were using the already set type and this changes the actual request
       keyword */
  पक्ष CURLOPT_SERVICE_NAME:
    प्रत्यागम Curl_setstropt(data, STRING_SERVICE_NAME, ptr);

  पक्ष CURLOPT_HEADERDATA:
    s->writeheader = ptr;
    भङ्ग;
  पक्ष CURLOPT_READDATA:
    s->in_set = ptr;
    भङ्ग;
  पक्ष CURLOPT_WRITEDATA:
    s->out = ptr;
    भङ्ग;
  पक्ष CURLOPT_DEBUGDATA:
    s->debugdata = ptr;
    भङ्ग;
  पक्ष CURLOPT_PROGRESSDATA:
    s->progress_client = ptr;
    भङ्ग;
  पक्ष CURLOPT_SEEKDATA:
    s->seek_client = ptr;
    भङ्ग;
  पक्ष CURLOPT_IOCTLDATA:
    s->ioctl_client = ptr;
    भङ्ग;
  पक्ष CURLOPT_SOCKOPTDATA:
    s->sockopt_client = ptr;
    भङ्ग;
  पक्ष CURLOPT_OPENSOCKETDATA:
    s->opensocket_client = ptr;
    भङ्ग;
  पक्ष CURLOPT_RESOLVER_START_DATA:
    s->resolver_start_client = ptr;
    भङ्ग;
  पक्ष CURLOPT_CLOSESOCKETDATA:
    s->closesocket_client = ptr;
    भङ्ग;
  पक्ष CURLOPT_PREREQDATA:
    s->prereq_userp = ptr;
    भङ्ग;
  पक्ष CURLOPT_ERRORBUFFER:
    s->errorbuffer = ptr;
    भङ्ग;
  पक्ष CURLOPT_URL:
    result = Curl_setstropt(data, STRING_SET_URL, ptr);
    Curl_bufref_set(&data->state.url,
                    CURL_EASY_STR(data, STRING_SET_URL), 0, रिक्त);
    भङ्ग;

  पक्ष CURLOPT_USERPWD: {
    वर्ण *u = रिक्त, *p = रिक्त;
    result = setstropt_userpwd(ptr, &u, &p);
    यदि(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_USERNAME, u);
      u = रिक्त;
    }
    यदि(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_PASSWORD, p);
      p = रिक्त;
    }
    curlx_free(u);
    curlx_free(p);
    प्रत्यागम result;
  }

  पक्ष CURLOPT_USERNAME:
    प्रत्यागम Curl_setstropt(data, STRING_USERNAME, ptr);

  पक्ष CURLOPT_PASSWORD:
    प्रत्यागम Curl_setstropt(data, STRING_PASSWORD, ptr);

  पक्ष CURLOPT_LOGIN_OPTIONS:
    प्रत्यागम Curl_setstropt(data, STRING_OPTIONS, ptr);

  पक्ष CURLOPT_XOAUTH2_BEARER:
    प्रत्यागम Curl_setstropt(data, STRING_BEARER, ptr);
  पक्ष CURLOPT_RANGE:
    प्रत्यागम Curl_setstropt(data, STRING_SET_RANGE, ptr);
  पक्ष CURLOPT_PRIVATE:
    s->private_data = ptr;
    भङ्ग;
  पक्ष CURLOPT_PROTOCOLS_STR:
    यदि(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      यदि(!result)
        s->allowed_protocols = protos;
    }
    अन्यथा
      /* make a NULL argument reset to default */
      s->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
    भङ्ग;
  पक्ष CURLOPT_REDIR_PROTOCOLS_STR:
    यदि(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      यदि(!result)
        s->redir_protocols = protos;
    }
    अन्यथा
      /* make a NULL argument reset to default */
      s->redir_protocols = (curl_prot_t)CURLPROTO_REDIR;
    भङ्ग;
  पक्ष CURLOPT_DEFAULT_PROTOCOL:
    /* Set the protocol to use when the URL does not include any protocol */
    प्रत्यागम Curl_setstropt(data, STRING_DEFAULT_PROTOCOL, ptr);
#ifndef CURL_DISABLE_SMTP
  पक्ष CURLOPT_MAIL_FROM:
    /* Set the SMTP mail originator */
    प्रत्यागम Curl_setstropt(data, STRING_MAIL_FROM, ptr);
  पक्ष CURLOPT_MAIL_AUTH:
    /* Set the SMTP auth originator */
    प्रत्यागम Curl_setstropt(data, STRING_MAIL_AUTH, ptr);
#endif
  पक्ष CURLOPT_SASL_AUTHZID:
    /* Authorization identity (identity to act as) */
    प्रत्यागम Curl_setstropt(data, STRING_SASL_AUTHZID, ptr);
#ifndef CURL_DISABLE_RTSP
  पक्ष CURLOPT_RTSP_SESSION_ID:
    Curl_peer_unlink(&data->state.rtsp_session_origin);
    प्रत्यागम Curl_setstropt(data, STRING_RTSP_SESSION_ID, ptr);
  पक्ष CURLOPT_RTSP_STREAM_URI:
    प्रत्यागम Curl_setstropt(data, STRING_RTSP_STREAM_URI, ptr);
  पक्ष CURLOPT_RTSP_TRANSPORT:
    प्रत्यागम Curl_setstropt(data, STRING_RTSP_TRANSPORT, ptr);
  पक्ष CURLOPT_INTERLEAVEDATA:
    s->rtp_out = ptr;
    भङ्ग;
#endif /* !CURL_DISABLE_RTSP */
  पक्ष CURLOPT_TLSAUTH_USERNAME:
  पक्ष CURLOPT_TLSAUTH_PASSWORD:
  पक्ष CURLOPT_TLSAUTH_TYPE:
  पक्ष CURLOPT_PROXY_TLSAUTH_USERNAME:
  पक्ष CURLOPT_PROXY_TLSAUTH_PASSWORD:
  पक्ष CURLOPT_PROXY_TLSAUTH_TYPE:
    प्रत्यागम CURLE_NOT_BUILT_IN;
#ifndef CURL_DISABLE_HSTS
  पक्ष CURLOPT_HSTSREADDATA:
    s->hsts_read_userp = ptr;
    भङ्ग;
  पक्ष CURLOPT_HSTSWRITEDATA:
    s->hsts_write_userp = ptr;
    भङ्ग;
  पक्ष CURLOPT_HSTS: {
    संरचना curl_slist *h;
    यदि(!data->hsts) {
      data->hsts = Curl_hsts_init();
      यदि(!data->hsts)
        प्रत्यागम CURLE_OUT_OF_MEMORY;
    }
    यदि(ptr) {
      result = Curl_setstropt(data, STRING_HSTS, ptr);
      यदि(result)
        प्रत्यागम result;
      /* this needs to build a list of filenames to read from, so that it can
         read them later, as we might get a shared HSTS handle to load them
         into */
      h = curl_slist_append(data->state.hstslist, ptr);
      यदि(!h) {
        curl_slist_free_all(data->state.hstslist);
        data->state.hstslist = रिक्त;
        प्रत्यागम CURLE_OUT_OF_MEMORY;
      }
      data->state.hstslist = h; /* store the list for later use */
    }
    अन्यथा {
      /* clear the list of HSTS files */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = रिक्त;
      यदि(!data->share || !data->share->hsts)
        /* throw away the HSTS cache unless shared */
        Curl_hsts_cleanup(&data->hsts);
    }
    भङ्ग;
  }
#endif /* !CURL_DISABLE_HSTS */
#ifndef CURL_DISABLE_ALTSVC
  पक्ष CURLOPT_ALTSVC:
    यदि(!data->asi) {
      data->asi = Curl_altsvc_init();
      यदि(!data->asi)
        प्रत्यागम CURLE_OUT_OF_MEMORY;
    }
    result = Curl_setstropt(data, STRING_ALTSVC, ptr);
    यदि(result)
      भङ्ग;
    यदि(ptr)
      प्रत्यागम Curl_altsvc_load(data->asi, ptr);
    भङ्ग;
#endif /* !CURL_DISABLE_ALTSVC */
  पक्ष CURLOPT_ECH:
    प्रत्यागम setopt_ech(data, ptr);
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }
  प्रत्यागम result;
}

स्थिर CURLcode setopt_cptr(संरचना Curl_easy *data, CURLoption option,
                            वर्ण *ptr)
{
  प्रकारनिर्देश CURLcode (*ptrfunc)(संरचना Curl_easy *data, CURLoption option,
                              वर्ण *ptr);
  /* Order by likeliness */
  स्थिर ध्रुव ptrfunc setopt_call[] = {
    setopt_cptr_misc,
#यदि defined(USE_SSL) || defined(USE_SSH)
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
#यदि !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
    setopt_cptr_http_mqtt,
#endif
  };
  size_t i;

  प्रति(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, ptr);
    यदि(result != CURLE_UNKNOWN_OPTION)
      प्रत्यागम result;
  }
  प्रत्यागम CURLE_UNKNOWN_OPTION;
}

स्थिर CURLcode setopt_func(संरचना Curl_easy *data, CURLoption option,
                            va_list param)
{
  संरचना UserDefined *s = &data->set;
  विकल्प(option) {
  पक्ष CURLOPT_PROGRESSFUNCTION:
    /*
     * Progress callback function
     */
    s->fprogress = va_arg(param, curl_progress_callback);
    यदि(s->fprogress)
      data->progress.callback = TRUE; /* no longer internal */
    अन्यथा
      data->progress.callback = FALSE; /* NULL enforces internal */
    भङ्ग;

  पक्ष CURLOPT_XFERINFOFUNCTION:
    /*
     * Transfer info callback function
     */
    s->fxferinfo = va_arg(param, curl_xferinfo_callback);
    यदि(s->fxferinfo)
      data->progress.callback = TRUE; /* no longer internal */
    अन्यथा
      data->progress.callback = FALSE; /* NULL enforces internal */

    भङ्ग;
  पक्ष CURLOPT_DEBUGFUNCTION:
    /*
     * stderr write callback.
     */
    s->fdebug = va_arg(param, curl_debug_callback);
    /*
     * if the callback provided is NULL, it will use the default callback
     */
    भङ्ग;
  पक्ष CURLOPT_HEADERFUNCTION:
    /*
     * Set header write callback
     */
    s->fwrite_header = va_arg(param, curl_write_callback);
    भङ्ग;
  पक्ष CURLOPT_WRITEFUNCTION:
    /*
     * Set data write callback
     */
    s->fwrite_func = va_arg(param, curl_write_callback);
    यदि(!s->fwrite_func)
#यदि defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic धकेलो
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fwrite_func = (curl_write_callback)fwrite;
#यदि defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic निकालो
#endif
    भङ्ग;
  पक्ष CURLOPT_READFUNCTION:
    /*
     * Read data callback
     */
    s->fread_func_set = va_arg(param, curl_read_callback);
    यदि(!s->fread_func_set) {
      s->is_fread_set = 0;
#यदि defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic धकेलो
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fread_func_set = (curl_read_callback)fread;
#यदि defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic निकालो
#endif
    }
    अन्यथा
      s->is_fread_set = 1;
    भङ्ग;
  पक्ष CURLOPT_SEEKFUNCTION:
    /*
     * Seek callback. Might be NULL.
     */
    s->seek_func = va_arg(param, curl_seek_callback);
    भङ्ग;
  पक्ष CURLOPT_IOCTLFUNCTION:
    /*
     * I/O control callback. Might be NULL.
     */
    s->ioctl_func = va_arg(param, curl_ioctl_callback);
    भङ्ग;
  पक्ष CURLOPT_SSL_CTX_FUNCTION:
    /*
     * Set an SSL_CTX callback
     */
#ifdef USE_SSL
    यदि(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      s->ssl_fsslctx = va_arg(param, curl_ssl_ctx_callback);
      भङ्ग;
    }
    अन्यथा
#endif
      प्रत्यागम CURLE_NOT_BUILT_IN;

  पक्ष CURLOPT_SOCKOPTFUNCTION:
    /*
     * socket callback function: called after socket() but before connect()
     */
    s->fsockopt = va_arg(param, curl_sockopt_callback);
    भङ्ग;

  पक्ष CURLOPT_OPENSOCKETFUNCTION:
    /*
     * open/create socket callback function: called instead of socket(),
     * before connect()
     */
    s->fopensocket = va_arg(param, curl_opensocket_callback);
    भङ्ग;

  पक्ष CURLOPT_CLOSESOCKETFUNCTION:
    /*
     * close socket callback function: called instead of close()
     * when shutting down a connection
     */
    s->fclosesocket = va_arg(param, curl_closesocket_callback);
    भङ्ग;

  पक्ष CURLOPT_RESOLVER_START_FUNCTION:
    /*
     * resolver start callback function: called before a new resolver request
     * is started
     */
    s->resolver_start = va_arg(param, curl_resolver_start_callback);
    भङ्ग;

#ifdef USE_SSH
#ifdef USE_LIBSSH2
  पक्ष CURLOPT_SSH_HOSTKEYFUNCTION:
    /* the callback to check the hostkey without the knownhost file */
    s->ssh_hostkeyfunc = va_arg(param, curl_sshhostkeycallback);
    भङ्ग;
#endif

  पक्ष CURLOPT_SSH_KEYFUNCTION:
    /* setting to NULL is fine since the ssh.c functions themselves will
       then revert to use the internal default */
    s->ssh_keyfunc = va_arg(param, curl_sshkeycallback);
    भङ्ग;

#endif /* USE_SSH */

#ifndef CURL_DISABLE_RTSP
  पक्ष CURLOPT_INTERLEAVEFUNCTION:
    /* Set the user defined RTP write function */
    s->fwrite_rtp = va_arg(param, curl_write_callback);
    भङ्ग;
#endif
#ifndef CURL_DISABLE_FTP
  पक्ष CURLOPT_CHUNK_BGN_FUNCTION:
    s->chunk_bgn = va_arg(param, curl_chunk_bgn_callback);
    भङ्ग;
  पक्ष CURLOPT_CHUNK_END_FUNCTION:
    s->chunk_end = va_arg(param, curl_chunk_end_callback);
    भङ्ग;
  पक्ष CURLOPT_FNMATCH_FUNCTION:
    s->fnmatch = va_arg(param, curl_fnmatch_callback);
    भङ्ग;
#endif
#ifndef CURL_DISABLE_HTTP
  पक्ष CURLOPT_TRAILERFUNCTION:
    s->trailer_callback = va_arg(param, curl_trailer_callback);
    भङ्ग;
#endif
#ifndef CURL_DISABLE_HSTS
  पक्ष CURLOPT_HSTSREADFUNCTION:
    s->hsts_read = va_arg(param, curl_hstsread_callback);
    भङ्ग;
  पक्ष CURLOPT_HSTSWRITEFUNCTION:
    s->hsts_write = va_arg(param, curl_hstswrite_callback);
    भङ्ग;
#endif
  पक्ष CURLOPT_PREREQFUNCTION:
    s->fprereq = va_arg(param, curl_prereq_callback);
    भङ्ग;
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode setopt_offt(संरचना Curl_easy *data, CURLoption option,
                            curl_off_t offt)
{
  संरचना UserDefined *s = &data->set;
  विकल्प(option) {
  पक्ष CURLOPT_TIMEVALUE_LARGE:
    /*
     * This is the value to compare with the remote document with the
     * method set with CURLOPT_TIMECONDITION
     */
    s->timevalue = (time_t)offt;
    भङ्ग;

    /* MQTT "borrows" some of the HTTP options */
  पक्ष CURLOPT_POSTFIELDSIZE_LARGE:
    /*
     * The size of the POSTFIELD data to prevent libcurl to do strlen() to
     * figure it out. Enables binary posts.
     */
    यदि(offt < -1)
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

    यदि(s->postfieldsize < offt && s->str_copypostfields) {
      /* Previous CURLOPT_COPYPOSTFIELDS is no longer valid. */
      curlx_safefree(s->str_copypostfields);
      s->postfields = रिक्त;
    }
    s->postfieldsize = offt;
    भङ्ग;
  पक्ष CURLOPT_INFILESIZE_LARGE:
    /*
     * If known, this should inform curl about the file size of the
     * to-be-uploaded file.
     */
    यदि(offt < -1)
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = offt;
    भङ्ग;
  पक्ष CURLOPT_MAX_SEND_SPEED_LARGE:
    /*
     * When transfer uploads are faster than CURLOPT_MAX_SEND_SPEED_LARGE
     * bytes per second the transfer is throttled..
     */
    यदि(offt < 0)
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_send_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.ul.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    भङ्ग;
  पक्ष CURLOPT_MAX_RECV_SPEED_LARGE:
    /*
     * When receiving data faster than CURLOPT_MAX_RECV_SPEED_LARGE bytes per
     * second the transfer is throttled..
     */
    यदि(offt < 0)
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_recv_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.dl.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    भङ्ग;
  पक्ष CURLOPT_RESUME_FROM_LARGE:
    /*
     * Resume transfer at the given file position
     */
    यदि(offt < -1)
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = offt;
    भङ्ग;
  पक्ष CURLOPT_MAXFILESIZE_LARGE:
    /*
     * Set the maximum size of a file to download.
     */
    यदि(offt < 0)
      प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_filesize = offt;
    भङ्ग;

  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode setopt_blob(संरचना Curl_easy *data, CURLoption option,
                            संरचना curl_blob *blob)
{
  संरचना UserDefined *s = &data->set;
  विकल्प(option) {
  पक्ष CURLOPT_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use
     */
    प्रत्यागम Curl_setblobopt(&s->blobs[BLOB_CERT], blob);
#ifndef CURL_DISABLE_PROXY
  पक्ष CURLOPT_PROXY_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use for proxy
     */
    प्रत्यागम Curl_setblobopt(&s->blobs[BLOB_CERT_PROXY], blob);
  पक्ष CURLOPT_PROXY_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use for proxy
     */
    प्रत्यागम Curl_setblobopt(&s->blobs[BLOB_कुंजी_PROXY], blob);
  पक्ष CURLOPT_PROXY_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection proxy.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    यदि(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO_PROXY], blob);
      यदि(!result)
        s->proxy_ssl.custom_cablob = TRUE;
      प्रत्यागम result;
    }
#endif
    प्रत्यागम CURLE_NOT_BUILT_IN;
  पक्ष CURLOPT_PROXY_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    यदि(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      प्रत्यागम Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT_PROXY], blob);
    प्रत्यागम CURLE_NOT_BUILT_IN;
#endif
  पक्ष CURLOPT_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use
     */
    प्रत्यागम Curl_setblobopt(&s->blobs[BLOB_कुंजी], blob);
  पक्ष CURLOPT_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    यदि(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO], blob);
      यदि(!result)
        s->ssl.custom_cablob = TRUE;
      प्रत्यागम result;
    }
#endif
    प्रत्यागम CURLE_NOT_BUILT_IN;
  पक्ष CURLOPT_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    यदि(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      प्रत्यागम Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT], blob);
    प्रत्यागम CURLE_NOT_BUILT_IN;

  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }
  /* unreachable */
}

/*
 * Do not make Curl_vsetopt() static: it is called from
 * projects/OS400/ccsidcurl.c.
 */
CURLcode Curl_vsetopt(संरचना Curl_easy *data, CURLoption option, va_list param)
{
  यदि(option < CURLOPTTYPE_OBJECTPOINT)
    प्रत्यागम setopt_long(data, option, va_arg(param, long));
  अन्यथा यदि(option < CURLOPTTYPE_FUNCTIONPOINT) {
    /* unfortunately, different pointer types cannot be identified any other
       way than being listed explicitly */
    विकल्प(option) {
    पक्ष CURLOPT_HTTPHEADER:
    पक्ष CURLOPT_QUOTE:
    पक्ष CURLOPT_POSTQUOTE:
    पक्ष CURLOPT_TELNETOPTIONS:
    पक्ष CURLOPT_PREQUOTE:
    पक्ष CURLOPT_HTTP200ALIASES:
    पक्ष CURLOPT_MAIL_RCPT:
    पक्ष CURLOPT_RESOLVE:
    पक्ष CURLOPT_PROXYHEADER:
    पक्ष CURLOPT_CONNECT_TO:
      प्रत्यागम setopt_slist(data, option, va_arg(param, संरचना curl_slist *));
    पक्ष CURLOPT_HTTPPOST:         /* curl_httppost * */
    पक्ष CURLOPT_MIMEPOST:         /* curl_mime * */
    पक्ष CURLOPT_STDERR:           /* FILE * */
    पक्ष CURLOPT_SHARE:            /* CURLSH * */
    पक्ष CURLOPT_CURLU:            /* CURLU * */
      प्रत्यागम setopt_pointers(data, option, param);
    पक्ष CURLOPT_STREAM_DEPENDS:   /* CURL * */
    पक्ष CURLOPT_STREAM_DEPENDS_E: /* CURL * */
      प्रत्यागम CURLE_OK;
    स्वभाव:
      भङ्ग;
    }
    /* the char pointer options */
    प्रत्यागम setopt_cptr(data, option, va_arg(param, वर्ण *));
  }
  अन्यथा यदि(option < CURLOPTTYPE_OFF_T)
    प्रत्यागम setopt_func(data, option, param);
  अन्यथा यदि(option < CURLOPTTYPE_BLOB)
    प्रत्यागम setopt_offt(data, option, va_arg(param, curl_off_t));
  प्रत्यागम setopt_blob(data, option, va_arg(param, संरचना curl_blob *));
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
  संरचना Curl_eapi_guard guard;
  CURLcode result;

  यदि(CURL_EAPI_ENTER(&guard, curl, easy_setopt, &result)) {
    संरचना Curl_easy *data = curl;
    va_list arg;

    va_start(arg, option);

    result = Curl_vsetopt(data, option, arg);

    va_end(arg);
    यदि(result == CURLE_BAD_FUNCTION_ARGUMENT)
      failf(data, "setopt 0x%x got bad argument", (unsigned पूर्णाङ्क)option);
  }
  CURL_EAPI_LEAVE(&guard);
  प्रत्यागम result;
}
