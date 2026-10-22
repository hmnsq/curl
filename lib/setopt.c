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
#किंवाजर defined(HAVE_NETINET_TCP_H)
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

स्थिर CURLcode setopt_set_timeout_sec(timediff_t *ptimeout_ms, दीर्घ secs)
{
  जर(secs < 0)
    परत CURLE_BAD_FUNCTION_ARGUMENT;
#जर दीर्घ_उच्च > (TIMEDIFF_T_MAX / 1000)
  जर(secs > (TIMEDIFF_T_MAX / 1000)) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    परत CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)secs * 1000;
  परत CURLE_OK;
}

स्थिर CURLcode setopt_set_timeout_ms(timediff_t *ptimeout_ms, दीर्घ ms)
{
  जर(ms < 0)
    परत CURLE_BAD_FUNCTION_ARGUMENT;
#जर दीर्घ_उच्च > TIMEDIFF_T_MAX
  जर(ms > TIMEDIFF_T_MAX) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    परत CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)ms;
  परत CURLE_OK;
}

CURLcode Curl_setstropt(संरचना Curl_easy *data,
                        गणना dupstring id, स्थिरांक अक्षर *s)
{
  माप_प्रकार slen = s ? म_माप(s) : 0;
  DEBUGASSERT((अचिन्हित)id <= UINT8_MAX);
  जर(s && (slen > CURL_MAX_आदान_LENGTH))
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  परत CURL_EASY_STR_SET(data, (uint8_t)id, s, slen);
}

CURLcode Curl_setblobopt(संरचना curl_blob **blobp,
                         स्थिरांक संरचना curl_blob *blob)
{
  /* free the previous storage at `blobp' and replace by a dynamic storage
     copy of blob. If CURL_BLOB_COPY is set, the data is copied. */

  curlx_safefree(*blobp);

  जर(blob) {
    संरचना curl_blob *nblob;
    जर(!blob->data || !blob->len || (blob->len > CURL_MAX_आदान_LENGTH))
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    nblob = (संरचना curl_blob *)
      curlx_malloc(आकार(संरचना curl_blob) +
                   ((blob->flags & CURL_BLOB_COPY) ? blob->len : 0));
    जर(!nblob)
      परत CURLE_OUT_OF_MEMORY;
    *nblob = *blob;
    जर(blob->flags & CURL_BLOB_COPY) {
      /* put the data after the blob struct in memory */
      nblob->data = (अक्षर *)nblob + आकार(संरचना curl_blob);
      स_नकल(nblob->data, blob->data, blob->len);
    }

    *blobp = nblob;
    परत CURLE_OK;
  }

  परत CURLE_OK;
}

स्थिर CURLcode setstropt_userpwd(स्थिरांक अक्षर *option, अक्षर **userp,
                                  अक्षर **passwdp)
{
  अक्षर *user = रिक्त;
  अक्षर *passwd = रिक्त;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Parse the login details if specified. If not, then we treat NULL as a
     hint to clear the existing data */
  जर(option) {
    माप_प्रकार len = म_माप(option);
    CURLcode result;
    जर(len > CURL_MAX_आदान_LENGTH)
      परत CURLE_BAD_FUNCTION_ARGUMENT;

    result = Curl_parse_login_details(option, len, &user, &passwd, रिक्त);
    जर(result)
      परत result;
  }

  curlx_free(*userp);
  *userp = user;

  curlx_strzero(*passwdp);
  curlx_free(*passwdp);
  *passwdp = passwd;

  परत CURLE_OK;
}

स्थिर CURLcode setstropt_interface(संरचना Curl_easy *data, अक्षर *option)
{
  जर(option && !option[0])
    option = रिक्त;
  जर(option) {
    /* Parse for checking syntax */
    CURLcode result = Curl_parse_interface(option, रिक्त, रिक्त, रिक्त);
    जर(result)
      परत result;
    परत CURL_EASY_STR_SET(data, STRING_INTERFACE_OPT,
                             option, म_माप(option));
  }
  CURL_EASY_STR_CLEAR(data, STRING_INTERFACE_OPT);
  परत CURLE_OK;
}

#ifdef USE_SSL
#define C_SSLVERSION_मूल्य(x)     ((x) & 0xffff)
#define C_SSLVERSION_MAX_मूल्य(x) ((अचिन्हित दीर्घ)(x) & 0xffff0000)
#endif

स्थिर CURLcode protocol2num(स्थिरांक अक्षर *str, curl_prot_t *val)
{
  /*
   * We are asked to cherry-pick protocols, so play it safe and disallow all
   * protocols to start with, and re-add the wanted ones back in.
   */
  *val = 0;

  जर(!str)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  जर(curl_strequal(str, "all")) {
    *val = ~(curl_prot_t)0;
    परत CURLE_OK;
  }

  करा {
    स्थिरांक अक्षर *token = str;
    माप_प्रकार tlen;

    str = म_अक्षर(str, ',');
    tlen = str ? (माप_प्रकार)(str - token) : म_माप(token);
    जर(tlen) {
      स्थिरांक संरचना Curl_scheme *h = Curl_getn_scheme(token, tlen);

      जर(!h || !h->run)
        परत CURLE_UNSUPPORTED_PROTOCOL;

      *val |= h->protocol;
    }
  } पर्यंत(str && str++);

  जर(!*val)
    /* no protocol listed */
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  परत CURLE_OK;
}

#जर !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
स्थिर CURLcode httpauth(संरचना Curl_easy *data, bool proxy,
                         अचिन्हित दीर्घ auth)
{
  जर(auth != CURLAUTH_NONE) {
    पूर्णांक bitcheck = 0;
    bool authbits = FALSE;
    जर(auth & CURLAUTH_DIGEST_IE) {
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
    पर्यंत(bitcheck < 31) {
      जर(auth & (1UL << bitcheck++)) {
        authbits = TRUE;
        थांबा;
      }
    }
    जर(!authbits)
      परत CURLE_NOT_BUILT_IN; /* no supported types left! */
  }
  जर(proxy)
    data->set.proxyauth = (uint32_t)auth;
  अन्यथा
    data->set.httpauth = (uint32_t)auth;
  परत CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */

#ifndef CURL_DISABLE_HTTP
स्थिर CURLcode setopt_HTTP_VERSION(संरचना Curl_easy *data, दीर्घ arg)
{
  /*
   * This sets a requested HTTP version to be used. The value is one of
   * the listed enums in curl/curl.h.
   */
  निवड(arg) {
  स्थिती CURL_HTTP_VERSION_NONE:
    /* accepted */
    थांबा;
  स्थिती CURL_HTTP_VERSION_1_0:
  स्थिती CURL_HTTP_VERSION_1_1:
    /* accepted */
    थांबा;
#ifdef USE_HTTP2
  स्थिती CURL_HTTP_VERSION_2_0:
  स्थिती CURL_HTTP_VERSION_2TLS:
  स्थिती CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    /* accepted */
    थांबा;
#endif
#ifdef USE_HTTP3
  स्थिती CURL_HTTP_VERSION_3:
  स्थिती CURL_HTTP_VERSION_3ONLY:
    /* accepted */
    थांबा;
#endif
  पूर्वनिर्धारित:
    /* not accepted */
    जर(arg < CURL_HTTP_VERSION_NONE)
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    परत CURLE_UNSUPPORTED_PROTOCOL;
  }
  data->set.httpwant = (अचिन्हित अक्षर)arg;
  परत CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP */

#ifdef USE_SSL
CURLcode Curl_setopt_SSLVERSION(संरचना Curl_easy *data, CURLoption option,
                                दीर्घ arg)
{
  /*
   * Set explicit SSL version to try to connect with, as some SSL
   * implementations are lame.
   */
  {
    दीर्घ version, version_max;
    संरचना ssl_easy_config *sslc = &data->set.ssl;
#ifndef CURL_DISABLE_PROXY
    जर(option != CURLOPT_SSLVERSION)
      sslc = &data->set.proxy_ssl;
#अन्यथा
    (शून्य)option; /* unused */
#endif
    version = C_SSLVERSION_मूल्य(arg);
    version_max = (दीर्घ)C_SSLVERSION_MAX_मूल्य(arg);

    जर(version < CURL_SSLVERSION_DEFAULT ||
       version == CURL_SSLVERSION_SSLv2 ||
       version == CURL_SSLVERSION_SSLv3 ||
       version >= CURL_SSLVERSION_LAST ||
       version_max < CURL_SSLVERSION_MAX_NONE ||
       version_max >= CURL_SSLVERSION_MAX_LAST)
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    जर(version == CURL_SSLVERSION_DEFAULT)
      version = CURL_SSLVERSION_TLSv1_2;

    sslc->version = (अचिन्हित अक्षर)version;
    sslc->version_max = (अचिन्हित पूर्णांक)version_max;
  }
  परत CURLE_OK;
}
#endif /* !USE_SSL */

#ifndef CURL_DISABLE_RTSP
स्थिर CURLcode setopt_RTSP_REQUEST(संरचना Curl_easy *data, दीर्घ arg)
{
  /*
   * Set the RTSP request method (OPTIONS, SETUP, PLAY, etc...) Would this be
   * better if the RTSPREQ_* were moved into here?
   */
  जर((arg <= CURL_RTSPREQ_NONE) || (arg >= CURL_RTSPREQ_LAST))
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  data->set.rtspreq = (अचिन्हित अक्षर)arg;
  परत CURLE_OK;
}
#endif /* !CURL_DISABLE_RTSP */

स्थिर CURLcode setopt_long_bool(संरचना Curl_easy *data, CURLoption option,
                                 दीर्घ arg)
{
  bool enabled = !!arg;
  पूर्णांक ok = 1;
  संरचना UserDefined *s = &data->set;
  निवड(option) {
  स्थिती CURLOPT_FORBID_REUSE:
    /*
     * When this transfer is done, it must not be left to be reused by a
     * subsequent transfer but shall be closed immediately.
     */
    s->reuse_forbid = enabled;
    थांबा;
  स्थिती CURLOPT_FRESH_CONNECT:
    /*
     * This transfer shall not use a previously cached connection but
     * should be made with a fresh new connect!
     */
    s->reuse_fresh = enabled;
    थांबा;
  स्थिती CURLOPT_VERBOSE:
    /*
     * Verbose means infof() calls that give a lot of information about
     * the connection and transfer procedures as well as internal choices.
     */
    s->verbose = enabled;
    थांबा;
  स्थिती CURLOPT_HEADER:
    /*
     * Set to include the header in the general data output stream.
     */
    s->include_header = enabled;
    थांबा;
  स्थिती CURLOPT_NOPROGRESS:
    /*
     * Shut off the internal supported progress meter
     */
    data->progress.hide = enabled;
    थांबा;
  स्थिती CURLOPT_NOBODY:
    /*
     * Do not include the body part in the output data stream.
     */
    s->opt_no_body = enabled;
#ifndef CURL_DISABLE_HTTP
    जर(s->opt_no_body)
      /* in HTTP lingo, no body means using the HEAD request... */
      s->method = HTTPREQ_HEAD;
    अन्यथा जर(s->method == HTTPREQ_HEAD)
      s->method = HTTPREQ_GET;
#endif
    थांबा;
  स्थिती CURLOPT_FAILONERROR:
    /*
     * Do not output the >=400 error code HTML-page, but instead only
     * return error.
     */
    s->http_fail_on_error = enabled;
    थांबा;
  स्थिती CURLOPT_KEEP_SENDING_ON_ERROR:
    s->http_keep_sending_on_error = enabled;
    थांबा;
  स्थिती CURLOPT_UPLOAD:
  स्थिती CURLOPT_PUT:
    /*
     * We want to send data to the remote host. If this is HTTP, that equals
     * using the PUT request.
     */
    जर(enabled) {
      /* If this is HTTP, PUT is what's needed to "upload" */
      s->method = HTTPREQ_PUT;
      s->opt_no_body = FALSE; /* this is implied */
    }
    अन्यथा
      /* In HTTP, the opposite of upload is GET (unless NOBODY is true as
         then this can be changed to HEAD later on) */
      s->method = HTTPREQ_GET;
    थांबा;
  स्थिती CURLOPT_FILETIME:
    /*
     * Try to get the file time of the remote document. The time will
     * later (possibly) become available using curl_easy_getinfo().
     */
    s->get_filetime = enabled;
    थांबा;
#ifndef CURL_DISABLE_HTTP
  स्थिती CURLOPT_HTTP09_ALLOWED:
    s->http09_allowed = enabled;
    थांबा;
#ifndef CURL_DISABLE_COOKIES
  स्थिती CURLOPT_COOKIESESSION:
    /*
     * Set this option to TRUE to start a new "cookie session". It will
     * prevent the forthcoming read-cookies-from-file actions to accept
     * cookies that are marked as being session cookies, as they belong to a
     * previous session.
     */
    s->cookiesession = enabled;
    थांबा;
#endif
  स्थिती CURLOPT_AUTOREFERER:
    /*
     * Switch on automatic referer that gets set if curl follows locations.
     */
    s->http_auto_referer = enabled;
    थांबा;
  स्थिती CURLOPT_TRANSFER_ENCODING:
    s->http_transfer_encoding = enabled;
    थांबा;
  स्थिती CURLOPT_UNRESTRICTED_AUTH:
    /*
     * Send authentication (user+password) when following locations, even when
     * hostname changed.
     */
    s->allow_auth_to_other_hosts = enabled;
    थांबा;
  स्थिती CURLOPT_HTTP_TRANSFER_DECODING:
    /*
     * disable libcurl transfer encoding is used
     */
    s->http_te_skip = !enabled; /* reversed */
    थांबा;
  स्थिती CURLOPT_HTTP_CONTENT_DECODING:
    /*
     * raw data passed to the application when content encoding is used
     */
    s->http_ce_skip = !enabled; /* reversed */
    थांबा;
  स्थिती CURLOPT_HTTPGET:
    /*
     * Set to force us do HTTP GET
     */
    जर(enabled) {
      s->method = HTTPREQ_GET;
      s->opt_no_body = FALSE; /* this is implied */
    }
    थांबा;
  स्थिती CURLOPT_POST:
    /* Does this option serve a purpose anymore? Yes it does, when
       CURLOPT_POSTFIELDS is not used and the POST data is read off the
       callback! */
    जर(enabled) {
      s->method = HTTPREQ_POST;
      s->opt_no_body = FALSE; /* this is implied */
    }
    अन्यथा
      s->method = HTTPREQ_GET;
    थांबा;
#endif /* !CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_PROXY
  स्थिती CURLOPT_HTTPPROXYTUNNEL:
    /*
     * Tunnel operations through the proxy instead of normal proxy use
     */
    s->tunnel_thru_httpproxy = enabled;
    थांबा;
  स्थिती CURLOPT_HAPROXYPROTOCOL:
    /*
     * Set to send the HAProxy Proxy Protocol header
     */
    s->haproxyprotocol = enabled;
    थांबा;
  स्थिती CURLOPT_PROXY_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for proxy.
     */
    s->proxy_ssl.verifypeer = enabled;

    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    थांबा;
  स्थिती CURLOPT_PROXY_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for proxy
     */
    s->proxy_ssl.verifyhost = enabled;
    ok = 2;
    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    थांबा;
  स्थिती CURLOPT_PROXY_TRANSFER_MODE:
    /*
     * set transfer mode (;type=<a|i>) when doing FTP via an HTTP proxy
     */
    s->proxy_transfer_mode = enabled;
    थांबा;
#endif /* !CURL_DISABLE_PROXY */
#जर defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  स्थिती CURLOPT_SOCKS5_GSSAPI_NEC:
    /*
     * Set flag for NEC SOCKS5 support
     */
    s->socks5_gssapi_nec = enabled;
    थांबा;
#endif
#ifdef CURL_LIST_ONLY_PROTOCOL
  स्थिती CURLOPT_DIRLISTONLY:
    /*
     * An option that changes the command to one that asks for a list only, no
     * file info details. Used for FTP, POP3 and SFTP.
     */
    s->list_only = enabled;
    थांबा;
#endif
  स्थिती CURLOPT_APPEND:
    /*
     * We want to upload and append to an existing file. Used for FTP and
     * SFTP.
     */
    s->remote_append = enabled;
    थांबा;
#ifndef CURL_DISABLE_FTP
  स्थिती CURLOPT_FTP_USE_EPRT:
    s->ftp_use_eprt = enabled;
    थांबा;
  स्थिती CURLOPT_FTP_USE_EPSV:
    s->ftp_use_epsv = enabled;
    थांबा;
  स्थिती CURLOPT_FTP_USE_PRET:
    s->ftp_use_pret = enabled;
    थांबा;
  स्थिती CURLOPT_FTP_SKIP_PASV_IP:
    /*
     * Enable or disable FTP_SKIP_PASV_IP, which will disable/enable the
     * bypass of the IP address in PASV responses.
     */
    s->ftp_skip_ip = enabled;
    थांबा;
  स्थिती CURLOPT_WILDCARDMATCH:
    s->wildcard_enabled = enabled;
    थांबा;
#endif
  स्थिती CURLOPT_CRLF:
    /*
     * Kludgy option to enable CRLF conversions. Subject for removal.
     */
    s->crlf = enabled;
    थांबा;
#ifndef CURL_DISABLE_TFTP
  स्थिती CURLOPT_TFTP_NO_OPTIONS:
    /*
     * Option that prevents libcurl from sending TFTP option requests to the
     * server.
     */
    s->tftp_no_options = enabled;
    थांबा;
#endif /* !CURL_DISABLE_TFTP */
  स्थिती CURLOPT_TRANSFERTEXT:
    /*
     * This option was previously named 'FTPASCII'. Renamed to work with
     * more protocols than merely FTP.
     *
     * Transfer using ASCII (instead of BINARY).
     */
    s->prefer_ascii = enabled;
    थांबा;
  स्थिती CURLOPT_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying.
     */
    s->ssl.verifypeer = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    थांबा;
#ifndef CURL_DISABLE_DOH
  स्थिती CURLOPT_DOH_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for DoH.
     */
    s->doh_verifypeer = enabled;
    थांबा;
  स्थिती CURLOPT_DOH_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for DoH
     */
    s->doh_verifyhost = enabled;
    ok = 2;
    थांबा;
  स्थिती CURLOPT_DOH_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying for DoH.
     */
    जर(!Curl_ssl_cert_status_request())
      परत CURLE_NOT_BUILT_IN;

    s->doh_verifystatus = enabled;
    ok = 2;
    थांबा;
#endif /* !CURL_DISABLE_DOH */
  स्थिती CURLOPT_SSL_VERIFYHOST:
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
    थांबा;
  स्थिती CURLOPT_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying.
     */
    जर(!Curl_ssl_cert_status_request())
      परत CURLE_NOT_BUILT_IN;

    s->ssl.verifystatus = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    थांबा;
  स्थिती CURLOPT_CERTINFO:
#ifdef USE_SSL
    जर(Curl_ssl_supports(data, SSLSUPP_CERTINFO))
      s->ssl.certinfo = enabled;
    अन्यथा
#endif
      परत CURLE_NOT_BUILT_IN;
    थांबा;
  स्थिती CURLOPT_NOSIGNAL:
    /*
     * The application asks not to set any signal() or alarm() handlers,
     * even when using a timeout.
     */
    s->no_signal = enabled;
    थांबा;
  स्थिती CURLOPT_TCP_NODELAY:
    /*
     * Enable or disable TCP_NODELAY, which will disable/enable the Nagle
     * algorithm
     */
    s->tcp_nodelay = enabled;
    थांबा;
  स्थिती CURLOPT_IGNORE_CONTENT_LENGTH:
    s->ignorecl = enabled;
    थांबा;
  स्थिती CURLOPT_SSL_SESSIONID_CACHE:
    s->ssl.cache_session = enabled;
#ifndef CURL_DISABLE_PROXY
    s->proxy_ssl.cache_session = s->ssl.cache_session;
#endif
    थांबा;
#ifdef USE_SSH
  स्थिती CURLOPT_SSH_COMPRESSION:
    s->ssh_compression = enabled;
    थांबा;
#endif /* !USE_SSH */
#ifndef CURL_DISABLE_SMTP
  स्थिती CURLOPT_MAIL_RCPT_ALLOWFAILS:
    /* allow RCPT TO command to fail for some recipients */
    s->mail_rcpt_allowfails = enabled;
    थांबा;
#endif /* !CURL_DISABLE_SMTP */
  स्थिती CURLOPT_SASL_IR:
    /* Enable/disable SASL initial response */
    s->sasl_ir = enabled;
    थांबा;
  स्थिती CURLOPT_TCP_KEEPALIVE:
    s->tcp_keepalive = enabled;
    थांबा;
  स्थिती CURLOPT_TCP_FASTOPEN:
#जर defined(CONNECT_DATA_IDEMPOTENT) || defined(MSG_FASTOPEN) || \
  defined(TCP_FASTOPEN_CONNECT)
    s->tcp_fastopen = enabled;
    थांबा;
#अन्यथा
    परत CURLE_NOT_BUILT_IN;
#endif
  स्थिती CURLOPT_SSL_ENABLE_ALPN:
    s->ssl_enable_alpn = enabled;
    थांबा;
  स्थिती CURLOPT_PATH_AS_IS:
    s->path_as_is = enabled;
    थांबा;
  स्थिती CURLOPT_PIPEWAIT:
    s->pipewait = enabled;
    थांबा;
  स्थिती CURLOPT_SUPPRESS_CONNECT_HEADERS:
    s->suppress_connect_headers = enabled;
    थांबा;
#ifndef CURL_DISABLE_SHUFFLE_DNS
  स्थिती CURLOPT_DNS_SHUFFLE_ADDRESSES:
    s->dns_shuffle_addresses = enabled;
    थांबा;
#endif
  स्थिती CURLOPT_DISALLOW_USERNAME_IN_URL:
    s->disallow_username_in_url = enabled;
    थांबा;
  स्थिती CURLOPT_QUICK_EXIT:
    s->quick_exit = enabled;
    थांबा;
  पूर्वनिर्धारित:
    परत CURLE_UNKNOWN_OPTION;
  }
  जर((arg > ok) || (arg < 0))
    /* reserve other values for future use */
    infof(data, "boolean setopt(%d) got unsupported argument %ld,"
          " treated as %d", (पूर्णांक)option, arg, enabled);

  परत CURLE_OK;
}

स्थिर CURLcode मूल्य_range(दीर्घ *मूल्य, दीर्घ below_error, दीर्घ min, दीर्घ max)
{
  जर(*मूल्य < below_error)
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  अन्यथा जर(*मूल्य < min)
    *मूल्य = min;
  अन्यथा जर(*मूल्य > max)
    *मूल्य = max;
  परत CURLE_OK;
}

स्थिर CURLcode setopt_long_net(संरचना Curl_easy *data, CURLoption option,
                                दीर्घ arg)
{
  CURLcode result = CURLE_OK;
  संरचना UserDefined *s = &data->set;

  निवड(option) {
  स्थिती CURLOPT_DNS_CACHE_TIMEOUT:
    जर(arg != -1)
      परत setopt_set_timeout_sec(&s->dns_cache_timeout_ms, arg);
    s->dns_cache_timeout_ms = -1;
    थांबा;
  स्थिती CURLOPT_MAXCONNECTS:
    result = मूल्य_range(&arg, 0, 0, पूर्णांक_उच्च);
    जर(!result)
      s->maxconnects = arg ? (uint32_t)arg : DEFAULT_CONNCACHE_SIZE;
    थांबा;
  स्थिती CURLOPT_SERVER_RESPONSE_TIMEOUT:
    परत setopt_set_timeout_sec(&s->server_response_timeout, arg);
  स्थिती CURLOPT_SERVER_RESPONSE_TIMEOUT_MS:
    परत setopt_set_timeout_ms(&s->server_response_timeout, arg);
  स्थिती CURLOPT_खालचा_SPEED_LIMIT:
    जर(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->खालचा_speed_limit = arg;
    थांबा;
  स्थिती CURLOPT_खालचा_SPEED_TIME:
    result = मूल्य_range(&arg, 0, 0, अच_लघु_उच्च);
    जर(!result)
      s->खालचा_speed_time = (uint16_t)arg;
    थांबा;
  स्थिती CURLOPT_PORT:
    जर((arg < 0) || (arg > 65535))
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    s->use_port = (अचिन्हित लघु)arg;
    थांबा;
  स्थिती CURLOPT_TIMEOUT:
    परत setopt_set_timeout_sec(&s->timeout, arg);
  स्थिती CURLOPT_TIMEOUT_MS:
    परत setopt_set_timeout_ms(&s->timeout, arg);
  स्थिती CURLOPT_CONNECTTIMEOUT:
    परत setopt_set_timeout_sec(&s->connecttimeout, arg);
  स्थिती CURLOPT_CONNECTTIMEOUT_MS:
    परत setopt_set_timeout_ms(&s->connecttimeout, arg);
#ifndef CURL_DISABLE_BINDLOCAL
  स्थिती CURLOPT_LOCALPORT:
    जर((arg < 0) || (arg > 65535))
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    s->localport = curlx_sltous(arg);
    थांबा;
  स्थिती CURLOPT_LOCALPORTRANGE:
    जर((arg < 0) || (arg > 65535))
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    s->localportrange = curlx_sltous(arg);
    थांबा;
#endif
  स्थिती CURLOPT_BUFFERSIZE:
    result = मूल्य_range(&arg, 0, READBUFFER_MIN, READBUFFER_MAX);
    जर(!result)
      s->buffer_size = (अचिन्हित पूर्णांक)arg;
    थांबा;
  स्थिती CURLOPT_UPLOAD_BUFFERSIZE:
    result = मूल्य_range(&arg, 0, UPLOADBUFFER_MIN, UPLOADBUFFER_MAX);
    जर(!result)
      s->upload_buffer_size = (अचिन्हित पूर्णांक)arg;
    थांबा;
  स्थिती CURLOPT_MAXFILESIZE:
    जर(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->max_filesize = arg;
    थांबा;
  स्थिती CURLOPT_IPRESOLVE:
    जर((arg < CURL_IPRESOLVE_WHATEVER) || (arg > CURL_IPRESOLVE_V6))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->ipver = (अचिन्हित अक्षर)arg;
    थांबा;
  स्थिती CURLOPT_CONNECT_ONLY:
    जर(arg < 0 || arg > 2)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा {
      s->connect_only = !!arg;
      s->connect_only_ws = (arg == 2);
    }
    थांबा;
#ifdef USE_IPV6
  स्थिती CURLOPT_ADDRESS_SCOPE:
#जर SIZEOF_LONG > 4
    जर((अचिन्हित दीर्घ)arg > अच_पूर्णांक_उच्च)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
#endif
    s->scope_id = (अचिन्हित पूर्णांक)arg;
    थांबा;
#endif
  स्थिती CURLOPT_TCP_KEEPIDLE:
    result = मूल्य_range(&arg, 0, 0, पूर्णांक_उच्च);
    जर(!result)
      s->tcp_keepidle = (पूर्णांक)arg;
    थांबा;
  स्थिती CURLOPT_TCP_KEEPINTVL:
    result = मूल्य_range(&arg, 0, 0, पूर्णांक_उच्च);
    जर(!result)
      s->tcp_keepintvl = (पूर्णांक)arg;
    थांबा;
  स्थिती CURLOPT_TCP_KEEPCNT:
    result = मूल्य_range(&arg, 0, 0, पूर्णांक_उच्च);
    जर(!result)
      s->tcp_keepcnt = (पूर्णांक)arg;
    थांबा;
  स्थिती CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
    परत setopt_set_timeout_ms(&s->happy_eyeballs_timeout, arg);
  स्थिती CURLOPT_UPKEEP_INTERVAL_MS:
    परत setopt_set_timeout_ms(&s->upkeep_interval_ms, arg);
  स्थिती CURLOPT_MAXAGE_CONN:
    परत setopt_set_timeout_sec(&s->conn_max_idle_ms, arg);
  स्थिती CURLOPT_MAXLIFETIME_CONN:
    परत setopt_set_timeout_sec(&s->conn_max_age_ms, arg);
  स्थिती CURLOPT_DNS_USE_GLOBAL_CACHE:
    /* deprecated */
    थांबा;
  पूर्वनिर्धारित:
    परत CURLE_UNKNOWN_OPTION;
  }
  परत result;
}

स्थिर CURLcode setopt_long_ssl(संरचना Curl_easy *data, CURLoption option,
                                दीर्घ arg)
{
#ifdef USE_SSL
  CURLcode result = CURLE_OK;
  संरचना UserDefined *s = &data->set;
  निवड(option) {
  स्थिती CURLOPT_CA_CACHE_TIMEOUT:
    जर(Curl_ssl_supports(data, SSLSUPP_CA_CACHE)) {
      result = मूल्य_range(&arg, -1, -1, पूर्णांक_उच्च);
      जर(!result)
        s->ssl_ca_cache_timeout = (पूर्णांक)arg;
    }
    अन्यथा
      result = CURLE_NOT_BUILT_IN;
    थांबा;
  स्थिती CURLOPT_SSLVERSION:
#ifndef CURL_DISABLE_PROXY
  स्थिती CURLOPT_PROXY_SSLVERSION:
#endif
    परत Curl_setopt_SSLVERSION(data, option, arg);
  स्थिती CURLOPT_SSL_FALSESTART:
    result = CURLE_NOT_BUILT_IN;
    थांबा;
  स्थिती CURLOPT_USE_SSL:
    जर((arg < CURLUSESSL_NONE) || (arg >= CURLUSESSL_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->use_ssl = (अचिन्हित अक्षर)arg;
    थांबा;
  स्थिती CURLOPT_SSL_OPTIONS:
    s->ssl.ssl_options = (अचिन्हित अक्षर)(arg & 0xff);
    थांबा;
#ifndef CURL_DISABLE_PROXY
  स्थिती CURLOPT_PROXY_SSL_OPTIONS:
    s->proxy_ssl.ssl_options = (अचिन्हित अक्षर)(arg & 0xff);
    थांबा;
#endif
  स्थिती CURLOPT_SSL_ENABLE_NPN:
    थांबा;
  स्थिती CURLOPT_SSLENGINE_DEFAULT:
#ifdef USE_OPENSSL
    जर(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      परत CURLE_NOT_BUILT_IN;
    data->set.engine_default = (arg == 1);
#अन्यथा
    परत CURLE_NOT_BUILT_IN;
#endif
    थांबा;
  पूर्वनिर्धारित:
    परत CURLE_UNKNOWN_OPTION;
  }
  परत result;
#अन्यथा  /* USE_SSL */
  (शून्य)data;
  (शून्य)option;
  (शून्य)arg;
  परत CURLE_UNKNOWN_OPTION;
#endif /* !USE_SSL */
}

#ifndef CURL_DISABLE_PROXY
स्थिर शून्य changeproxy(संरचना Curl_easy *data)
{
  Curl_auth_digest_cleanup(&data->state.proxydigest);
  स_रखो(&data->state.authproxy, 0, आकार(data->state.authproxy));
}

स्थिर CURLcode setopt_long_proxy(संरचना Curl_easy *data, CURLoption option,
                                  दीर्घ arg)
{
  संरचना UserDefined *s = &data->set;

  निवड(option) {
  स्थिती CURLOPT_PROXYPORT:
    जर((arg < 0) || (arg > UINT16_MAX))
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    जर(arg != s->proxyport)
      changeproxy(data);
    s->proxyport = (uint16_t)arg;
    थांबा;
  स्थिती CURLOPT_PROXYAUTH:
    परत httpauth(data, TRUE, (अचिन्हित दीर्घ)arg);
  स्थिती CURLOPT_PROXYTYPE:
    जर((arg < CURLPROXY_HTTP) || (arg > CURLPROXY_HTTPS3))
      परत CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef USE_PROXY_HTTP3
    जर(arg == CURLPROXY_HTTPS3)
      परत CURLE_NOT_BUILT_IN;
#endif
    s->proxytype = (अचिन्हित अक्षर)arg;
    थांबा;
  स्थिती CURLOPT_SOCKS5_AUTH: {
    अचिन्हित दीर्घ auth = (अचिन्हित दीर्घ)arg;
    जर(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI | CURLAUTH_ONLY))
      परत CURLE_NOT_BUILT_IN;
    जर((auth & CURLAUTH_ONLY) && !(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI)))
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    s->socks5auth = (अचिन्हित अक्षर)(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI));
    s->socks5_auth_only = !!(auth & CURLAUTH_ONLY);
    थांबा;
  }
  पूर्वनिर्धारित:
    परत CURLE_UNKNOWN_OPTION;
  }
  परत CURLE_OK;
}
#अन्यथा
स्थिर CURLcode setopt_long_proxy(संरचना Curl_easy *data, CURLoption option,
                                  दीर्घ arg)
{
  (शून्य)data;
  (शून्य)option;
  (शून्य)arg;
  परत CURLE_UNKNOWN_OPTION;
}
#endif

स्थिर CURLcode setopt_long_http(संरचना Curl_easy *data, CURLoption option,
                                 दीर्घ arg)
{
#ifndef CURL_DISABLE_HTTP
  CURLcode result = CURLE_OK;
  संरचना UserDefined *s = &data->set;

  निवड(option) {
  स्थिती CURLOPT_FOLLOWLOCATION:
    जर((अचिन्हित दीर्घ)arg > 3)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->http_follow_mode = (अचिन्हित अक्षर)arg;
    थांबा;
  स्थिती CURLOPT_MAXREDIRS:
    result = मूल्य_range(&arg, -1, -1, 0x7fff);
    जर(!result)
      s->maxredirs = (लघु)arg;
    थांबा;
  स्थिती CURLOPT_POSTREDIR:
    जर(arg < CURL_REDIR_GET_ALL)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा {
      s->post301 = !!(arg & CURL_REDIR_POST_301);
      s->post302 = !!(arg & CURL_REDIR_POST_302);
      s->post303 = !!(arg & CURL_REDIR_POST_303);
    }
    थांबा;
  स्थिती CURLOPT_HEADEROPT:
    s->sep_headers = !!(arg & CURLHEADER_SEPARATE);
    थांबा;
  स्थिती CURLOPT_HTTPAUTH:
    परत httpauth(data, FALSE, (अचिन्हित दीर्घ)arg);
  स्थिती CURLOPT_HTTP_VERSION:
    परत setopt_HTTP_VERSION(data, arg);
  स्थिती CURLOPT_EXPECT_100_TIMEOUT_MS:
    result = मूल्य_range(&arg, 0, 0, 0xffff);
    जर(!result)
      s->expect_100_timeout = (अचिन्हित लघु)arg;
    थांबा;
  स्थिती CURLOPT_STREAM_WEIGHT:
#जर defined(USE_HTTP2) || defined(USE_HTTP3)
    जर((arg >= 1) && (arg <= 256))
      s->weight = (पूर्णांक)arg;
    थांबा;
#अन्यथा
    result = CURLE_NOT_BUILT_IN;
    थांबा;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  स्थिती CURLOPT_HTTPSIG_ALGORITHM:
    जर(arg != CURLHTTPSIG_NONE &&
       arg != CURLHTTPSIG_ED25519 &&
       arg != CURLHTTPSIG_HMAC_SHA256)
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    s->httpsig_algorithm = (uint8_t)arg;
    जर(arg)
      s->httpauth = (uint32_t)CURLAUTH_HTTPSIG;
    अन्यथा
      s->httpauth &= ~(uint32_t)CURLAUTH_HTTPSIG;
    थांबा;
#endif
  पूर्वनिर्धारित:
    परत CURLE_UNKNOWN_OPTION;
  }
  परत result;
#अन्यथा
  (शून्य)data;
  (शून्य)option;
  (शून्य)arg;
  परत CURLE_UNKNOWN_OPTION;
#endif
}

स्थिर CURLcode setopt_long_proto(संरचना Curl_easy *data, CURLoption option,
                                  दीर्घ arg)
{
  CURLcode result = CURLE_OK;
  संरचना UserDefined *s = &data->set;

  निवड(option) {
#ifndef CURL_DISABLE_TFTP
  स्थिती CURLOPT_TFTP_BLKSIZE:
    result = मूल्य_range(&arg, 0, TFTP_BLKSIZE_MIN, TFTP_BLKSIZE_MAX);
    जर(!result)
      s->tftp_blksize = (अचिन्हित लघु)arg;
    थांबा;
#endif
#ifndef CURL_DISABLE_NETRC
  स्थिती CURLOPT_NETRC:
    जर((arg < CURL_NETRC_IGNORED) || (arg >= CURL_NETRC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->use_netrc = (अचिन्हित अक्षर)arg;
    थांबा;
#endif
#ifndef CURL_DISABLE_FTP
  स्थिती CURLOPT_FTP_FILEMETHOD:
    जर((arg < CURLFTPMETHOD_DEFAULT) || (arg >= CURLFTPMETHOD_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->ftp_filemethod = (अचिन्हित अक्षर)arg;
    थांबा;
  स्थिती CURLOPT_FTP_SSL_CCC:
    जर((arg < CURLFTPSSL_CCC_NONE) || (arg >= CURLFTPSSL_CCC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->ftp_ccc = (अचिन्हित अक्षर)arg;
    थांबा;
  स्थिती CURLOPT_FTPSSLAUTH:
    जर((arg < CURLFTPAUTH_DEFAULT) || (arg >= CURLFTPAUTH_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->ftpsslauth = (अचिन्हित अक्षर)arg;
    थांबा;
  स्थिती CURLOPT_ACCEPTTIMEOUT_MS:
    परत setopt_set_timeout_ms(&s->accepttimeout, arg);
#endif
#जर !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  स्थिती CURLOPT_FTP_CREATE_MISSING_DIRS:
    जर((arg < CURLFTP_CREATE_DIR_NONE) || (arg > CURLFTP_CREATE_DIR_RETRY))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->ftp_create_missing_dirs = (अचिन्हित अक्षर)arg;
    थांबा;
  स्थिती CURLOPT_NEW_FILE_PERMS:
    जर((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->new_file_perms = (अचिन्हित पूर्णांक)arg;
    थांबा;
#endif
#ifndef CURL_DISABLE_RTSP
  स्थिती CURLOPT_RTSP_REQUEST:
    परत setopt_RTSP_REQUEST(data, arg);
  स्थिती CURLOPT_RTSP_CLIENT_CSEQ:
    result = मूल्य_range(&arg, 0, 0, पूर्णांक_उच्च);
    जर(!result)
      data->state.rtsp_next_client_CSeq = (uint32_t)arg;
    थांबा;
  स्थिती CURLOPT_RTSP_SERVER_CSEQ:
    result = मूल्य_range(&arg, 0, 0, पूर्णांक_उच्च);
    जर(!result)
      data->state.rtsp_next_server_CSeq = (uint32_t)arg;
    थांबा;
#endif
#ifdef USE_SSH
  स्थिती CURLOPT_SSH_AUTH_TYPES:
    s->ssh_auth_types = (uint32_t)arg;
    थांबा;
  स्थिती CURLOPT_NEW_DIRECTORY_PERMS:
    जर((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->new_directory_perms = (अचिन्हित पूर्णांक)arg;
    थांबा;
#endif
  स्थिती CURLOPT_PROTOCOLS:
    s->allowed_protocols = (curl_prot_t)arg;
    थांबा;
  स्थिती CURLOPT_REDIR_PROTOCOLS:
    s->redir_protocols = (curl_prot_t)arg;
    थांबा;
#ifndef CURL_DISABLE_WEBSOCKETS
  स्थिती CURLOPT_WS_OPTIONS:
    s->ws_raw_mode = !!(arg & CURLWS_RAW_MODE);
    s->ws_no_auto_pong = !!(arg & CURLWS_NOAUTOPONG);
    थांबा;
#endif
  पूर्वनिर्धारित:
    परत CURLE_UNKNOWN_OPTION;
  }
  परत result;
}

स्थिर CURLcode setopt_long_misc(संरचना Curl_easy *data, CURLoption option,
                                 दीर्घ arg)
{
  संरचना UserDefined *s = &data->set;

  निवड(option) {
  स्थिती CURLOPT_TIMECONDITION:
    जर((arg < CURL_TIMECOND_NONE) || (arg >= CURL_TIMECOND_LAST))
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    s->timecondition = (अचिन्हित अक्षर)arg;
    थांबा;
  स्थिती CURLOPT_TIMEVALUE:
    s->timevalue = (समय_प्रकार)arg;
    थांबा;
  स्थिती CURLOPT_POSTFIELDSIZE:
    जर(arg < -1)
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    जर(s->postfieldsize < arg && s->str_copypostfields) {
      curlx_safefree(s->str_copypostfields);
      s->postfields = रिक्त;
    }
    s->postfieldsize = arg;
    थांबा;
  स्थिती CURLOPT_INFILESIZE:
    जर(arg < -1)
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = arg;
    थांबा;
  स्थिती CURLOPT_RESUME_FROM:
    जर(arg < -1)
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = arg;
    थांबा;
  स्थिती CURLOPT_UPLOAD_FLAGS:
    s->upload_flags = (अचिन्हित अक्षर)arg;
    थांबा;
#ifndef CURL_DISABLE_MIME
  स्थिती CURLOPT_MIME_OPTIONS:
    s->mime_formescape = !!(arg & CURLMIMEOPT_FORMESCAPE);
    थांबा;
#endif
#ifndef CURL_DISABLE_HSTS
  स्थिती CURLOPT_HSTS_CTRL:
    जर(arg & CURLHSTS_ENABLE) {
      जर(!data->hsts) {
        data->hsts = Curl_hsts_init();
        जर(!data->hsts)
          परत CURLE_OUT_OF_MEMORY;
      }
    }
    अन्यथा जर(!data->share || !data->share->hsts) {
      /* throw away the HSTS cache unless shared */
      Curl_hsts_cleanup(&data->hsts);
      /* flush all the entries */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = रिक्त;
    }
    अन्यथा
      /* detach from shared HSTS cache without freeing it */
      data->hsts = रिक्त;
    थांबा;
#endif
#ifndef CURL_DISABLE_ALTSVC
  स्थिती CURLOPT_ALTSVC_CTRL:
    परत Curl_altsvc_ctrl(data, arg);
#endif
#जर defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  स्थिती CURLOPT_GSSAPI_DELEGATION:
    s->gssapi_delegation = (अचिन्हित अक्षर)arg &
      (CURLGSSAPI_DELEGATION_POLICY_FLAG | CURLGSSAPI_DELEGATION_FLAG);
    थांबा;
#endif
  पूर्वनिर्धारित:
    परत CURLE_UNKNOWN_OPTION;
  }
  परत CURLE_OK;
}

स्थिर CURLcode setopt_long(संरचना Curl_easy *data, CURLoption option,
                            दीर्घ arg)
{
  प्रकारव्याख्या CURLcode (*setoptfunc)(संरचना Curl_easy *data,
                                 CURLoption option, दीर्घ arg);
  स्थिर स्थिरांक setoptfunc setopt_call[] = {
    setopt_long_bool,
    setopt_long_net,
    setopt_long_http,
    setopt_long_proxy,
    setopt_long_ssl,
    setopt_long_proto,
    setopt_long_misc
  };
  माप_प्रकार i;

  साठी(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, arg);
    जर(result != CURLE_UNKNOWN_OPTION)
      परत result;
  }
  परत CURLE_UNKNOWN_OPTION;
}

स्थिर CURLcode setopt_slist(संरचना Curl_easy *data, CURLoption option,
                             संरचना curl_slist *slist)
{
  CURLcode result = CURLE_OK;
  संरचना UserDefined *s = &data->set;
  निवड(option) {
#ifndef CURL_DISABLE_PROXY
  स्थिती CURLOPT_PROXYHEADER:
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
    थांबा;
#endif
#ifndef CURL_DISABLE_HTTP
  स्थिती CURLOPT_HTTP200ALIASES:
    /*
     * Set a list of aliases for HTTP 200 in response header
     */
    s->http200aliases = slist;
    थांबा;
#endif
#जर !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  स्थिती CURLOPT_POSTQUOTE:
    /*
     * List of RAW FTP commands to use after a transfer
     */
    s->postquote = slist;
    थांबा;
  स्थिती CURLOPT_PREQUOTE:
    /*
     * List of RAW FTP commands to use prior to RETR (Wesley Laxton)
     */
    s->prequote = slist;
    थांबा;
  स्थिती CURLOPT_QUOTE:
    /*
     * List of RAW FTP commands to use before a transfer
     */
    s->quote = slist;
    थांबा;
#endif
  स्थिती CURLOPT_RESOLVE:
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
    थांबा;
#जर !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MIME)
  स्थिती CURLOPT_HTTPHEADER:
    /*
     * Set a list with HTTP headers to use (or replace internals with)
     */
    s->headers = slist;
    थांबा;
#endif
#ifndef CURL_DISABLE_TELNET
  स्थिती CURLOPT_TELNETOPTIONS:
    /*
     * Set a linked list of telnet options
     */
    s->telnet_options = slist;
    थांबा;
#endif
#ifndef CURL_DISABLE_SMTP
  स्थिती CURLOPT_MAIL_RCPT:
    /* Set the list of mail recipients */
    s->mail_rcpt = slist;
    थांबा;
#endif
  स्थिती CURLOPT_CONNECT_TO:
    s->connect_to = slist;
    थांबा;
  पूर्वनिर्धारित:
    परत CURLE_UNKNOWN_OPTION;
  }
  परत result;
}

#जर !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
स्थिर CURLcode setopt_mimepost(संरचना Curl_easy *data, curl_mime *mimep)
{
  /*
   * Set to make us do MIME POST
   */
  CURLcode result;
  संरचना UserDefined *s = &data->set;
  जर(!s->mimepostp) {
    s->mimepostp = curlx_malloc(आकार(*s->mimepostp));
    जर(!s->mimepostp)
      परत CURLE_OUT_OF_MEMORY;
    Curl_mime_initpart(s->mimepostp);
  }

  result = Curl_mime_set_subparts(s->mimepostp, mimep, FALSE);
  जर(!result) {
    s->method = HTTPREQ_POST_MIME;
    s->opt_no_body = FALSE; /* this is implied */
#ifndef CURL_DISABLE_FORM_API
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = रिक्त;
#endif
  }
  परत result;
}
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */

स्थिर CURLcode setopt_share(संरचना Curl_easy *data, संरचना Curl_share *set)
{
  CURLcode result;

  जर(data->conn) {
    /* As this handle already has a connection attached, changing share now
       would be complicated and error-prone */
    infof(data, "Cannot change share object while in use");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
  }
  अन्यथा {
    /* disconnect from old share, if any and possible */
    result = Curl_share_easy_unlink(data);
    जर(!result && GOOD_SHARE_HANDLE(set))
      /* use new share if it set */
      result = Curl_share_easy_link(data, set);
  }
  परत result;
}

/* assorted pointer type arguments */
स्थिर CURLcode setopt_pointers(संरचना Curl_easy *data, CURLoption option,
                                बहु_सूची param)
{
  CURLcode result = CURLE_OK;
  संरचना UserDefined *s = &data->set;
  निवड(option) {
  स्थिती CURLOPT_CURLU:
    /*
     * pass CURLU to set URL
     */
    Curl_bufref_free(&data->state.url);
    CURL_EASY_STR_CLEAR(data, STRING_SET_URL);
    s->uh = बहु_तर्क(param, CURLU *);
    थांबा;
#ifndef CURL_DISABLE_HTTP
#ifndef CURL_DISABLE_FORM_API
  स्थिती CURLOPT_HTTPPOST:
    /*
     * Set to make us do HTTP POST. Legacy API-style.
     */
    s->httppost = बहु_तर्क(param, संरचना curl_httppost *);
    s->method = HTTPREQ_POST_FORM;
    s->opt_no_body = FALSE; /* this is implied */
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = रिक्त;
    थांबा;
#endif /* !CURL_DISABLE_FORM_API */
#endif /* !CURL_DISABLE_HTTP */
#जर !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
  स्थिती CURLOPT_MIMEPOST:
    result = setopt_mimepost(data, बहु_तर्क(param, curl_mime *));
    थांबा;
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */
  स्थिती CURLOPT_STDERR:
    /*
     * Set to a FILE * that should receive all error writes. This
     * defaults to stderr for normal operations.
     */
    s->err = बहु_तर्क(param, खाता *);
    जर(!s->err)
      s->err = मानक_त्रुटि;
    थांबा;
  स्थिती CURLOPT_SHARE:
    परत setopt_share(data, बहु_तर्क(param, संरचना Curl_share *));

  पूर्वनिर्धारित:
    परत CURLE_UNKNOWN_OPTION;
  }
  परत result;
}

#ifndef CURL_DISABLE_COOKIES
स्थिर CURLcode cookielist(संरचना Curl_easy *data, स्थिरांक अक्षर *ptr)
{
  CURLcode result = CURLE_OK;
  जर(!ptr)
    परत CURLE_OK;

  जर(curl_strequal(ptr, "ALL")) {
    /* clear all cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearall(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  अन्यथा जर(curl_strequal(ptr, "SESS")) {
    /* clear session cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearsess(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  अन्यथा जर(curl_strequal(ptr, "FLUSH")) {
    /* flush cookies to file, takes care of the locking */
    Curl_flush_cookies(data, FALSE);
  }
  अन्यथा जर(curl_strequal(ptr, "RELOAD")) {
    /* reload cookies from file */
    परत Curl_cookie_loadfiles(data, COOKIE_NOPSL |
                                 (data->set.cookiesession ?
                                  COOKIE_NOSESSION : 0));
  }
  अन्यथा {
    जर(!data->cookies) {
      /* if cookie engine was not running, activate it */
      data->cookies = Curl_cookie_init();
      जर(!data->cookies)
        परत CURLE_OUT_OF_MEMORY;
      data->state.cookie_engine = TRUE;
    }

    /* general protection against mistakes and abuse */
    जर(म_माप(ptr) > CURL_MAX_आदान_LENGTH)
      परत CURLE_BAD_FUNCTION_ARGUMENT;

    /* Adding these cookies without the PSL check, because the PSL is not
       initialized until *perform() time, and this might be called before
       that */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    जर(checkprefix("Set-Cookie:", ptr))
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
  परत result;
}

स्थिर CURLcode cookiefile(संरचना Curl_easy *data, स्थिरांक अक्षर *ptr)
{
  /*
   * Set cookie file to read and parse. Can be used multiple times.
   */
  जर(ptr) {
    संरचना curl_slist *cl;
    /* general protection against mistakes and abuse */
    जर(म_माप(ptr) > CURL_MAX_आदान_LENGTH)
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    /* append the cookie filename to the list of filenames, and deal with
       them later */
    cl = curl_slist_append(data->state.cookielist, ptr);
    जर(!cl) {
      curl_slist_free_all(data->state.cookielist);
      data->state.cookielist = रिक्त;
      परत CURLE_OUT_OF_MEMORY;
    }
    data->state.cookielist = cl; /* store the list for later use */
  }
  अन्यथा {
    /* clear the list of cookie files */
    curl_slist_free_all(data->state.cookielist);
    data->state.cookielist = रिक्त;

    जर(!data->share || !data->share->cookies) {
      /* throw away all existing cookies if this is not a shared cookie
         container */
      Curl_cookie_clearall(data->cookies);
      Curl_cookie_cleanup(data->cookies);
    }
    /* disable the cookie engine */
    data->cookies = रिक्त;
  }
  परत CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_PROXY
स्थिर CURLcode setproxy(संरचना Curl_easy *data, स्थिरांक अक्षर *proxy)
{
  स्थिरांक अक्षर *str = CURL_EASY_STR(data, STRING_PROXY);
  जर(str && proxy &&
     /* there was one set, is this a new one? */
     !म_भेद(str, proxy))
    परत CURLE_OK; /* same one as before */

  changeproxy(data);
  परत Curl_setstropt(data, STRING_PROXY, proxy);
}

स्थिर CURLcode setopt_cptr_proxy(संरचना Curl_easy *data, CURLoption option,
                                  अक्षर *ptr)
{
  CURLcode result = CURLE_OK;
  संरचना UserDefined *s = &data->set;
  निवड(option) {
  स्थिती CURLOPT_PROXYUSERPWD: {
    /*
     * user:password needed to use the proxy
     */
    अक्षर *u = रिक्त;
    अक्षर *p = रिक्त;
    result = setstropt_userpwd(ptr, &u, &p);

    /* URL decode the components */
    जर(!result) {
      अक्षर *str = रिक्त;
      CURL_EASY_STR_CLEAR(data, STRING_PROXYUSERNAME);
      CURL_EASY_STR_CLEAR(data, STRING_PROXYPASSWORD);
      जर(u) {
        result = Curl_urldecode(u, 0, &str, रिक्त, REJECT_ZERO);
        जर(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYUSERNAME, str);
      }
      जर(!result && p) {
        str = रिक्त;
        result = Curl_urldecode(p, 0, &str, रिक्त, REJECT_ZERO);
        जर(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYPASSWORD, str);
      }
    }
    curlx_free(u);
    curlx_strzero(p);
    curlx_free(p);
    थांबा;
  }
  स्थिती CURLOPT_PROXYUSERNAME:
    /*
     * authentication username to use in the operation
     */
    परत Curl_setstropt(data, STRING_PROXYUSERNAME, ptr);

  स्थिती CURLOPT_PROXYPASSWORD:
    /*
     * authentication password to use in the operation
     */
    परत Curl_setstropt(data, STRING_PROXYPASSWORD, ptr);

  स्थिती CURLOPT_NOPROXY:
    /*
     * proxy exception list
     */
    परत Curl_setstropt(data, STRING_NOPROXY, ptr);
  स्थिती CURLOPT_PROXY_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use for proxy
     */
    परत Curl_setstropt(data, STRING_CERT_PROXY, ptr);
  स्थिती CURLOPT_PROXY_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use for proxy
     */
    परत Curl_setstropt(data, STRING_CERT_TYPE_PROXY, ptr);
  स्थिती CURLOPT_PROXY_SSLKEY:
    /*
     * String that holds filename of the SSL key to use for proxy
     */
    परत Curl_setstropt(data, STRING_किल्ली_PROXY, ptr);
  स्थिती CURLOPT_PROXY_KEYPASSWD:
    /*
     * String that holds the SSL private key password for proxy.
     */
    परत Curl_setstropt(data, STRING_किल्ली_PASSWD_PROXY, ptr);
  स्थिती CURLOPT_PROXY_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use for proxy
     */
    परत Curl_setstropt(data, STRING_किल्ली_TYPE_PROXY, ptr);
  स्थिती CURLOPT_PROXY_SSL_CIPHER_LIST:
    जर(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST)) {
      /* set a list of cipher we want to use in the SSL connection for proxy */
      परत Curl_setstropt(data, STRING_SSL_CIPHER_LIST_PROXY, ptr);
    }
    अन्यथा
      परत CURLE_NOT_BUILT_IN;
  स्थिती CURLOPT_PROXY_TLS13_CIPHERS:
    जर(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites for proxy */
      परत Curl_setstropt(data, STRING_SSL_CIPHER13_LIST_PROXY, ptr);
    अन्यथा
      परत CURLE_NOT_BUILT_IN;
  स्थिती CURLOPT_PROXY:
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
    परत setproxy(data, ptr);
  स्थिती CURLOPT_PRE_PROXY:
    /*
     * Set proxy server:port to use as SOCKS proxy.
     *
     * If the proxy is set to "" or NULL we explicitly say that we do not want
     * to use the socks proxy.
     */
    परत Curl_setstropt(data, STRING_PRE_PROXY, ptr);
  स्थिती CURLOPT_SOCKS5_GSSAPI_SERVICE:
  स्थिती CURLOPT_PROXY_SERVICE_NAME:
    /*
     * Set proxy authentication service name for Kerberos 5 and SPNEGO
     */
    परत Curl_setstropt(data, STRING_PROXY_SERVICE_NAME, ptr);
  स्थिती CURLOPT_PROXY_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
#ifdef USE_SSL
    जर(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      परत Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY_PROXY, ptr);
#endif
    परत CURLE_NOT_BUILT_IN;

  स्थिती CURLOPT_HAPROXY_CLIENT_IP:
    /*
     * Set the client IP to send through HAProxy PROXY protocol
     */
    result = Curl_setstropt(data, STRING_HAPROXY_CLIENT_IP, ptr);

    /* enable the HAProxy protocol if an IP is provided */
    s->haproxyprotocol = !!CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
    थांबा;
  स्थिती CURLOPT_PROXY_CAINFO:
    /*
     * Set CA info SSL connection for proxy. Specify filename of the
     * CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE_PROXY, ptr);
    जर(!result)
      s->proxy_ssl.custom_cafile = TRUE;
    परत result;
  स्थिती CURLOPT_PROXY_CRLFILE:
    /*
     * Set CRL file info for SSL connection for proxy. Specify filename of the
     * CRL to check certificates revocation
     */
    जर(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      परत Curl_setstropt(data, STRING_SSL_CRLFILE_PROXY, ptr);
    परत CURLE_NOT_BUILT_IN;
  स्थिती CURLOPT_PROXY_ISSUERCERT:
    /*
     * Set Issuer certificate file to check certificates issuer
     */
    जर(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      परत Curl_setstropt(data, STRING_SSL_ISSUERCERT_PROXY, ptr);
    परत CURLE_NOT_BUILT_IN;
  स्थिती CURLOPT_PROXY_CAPATH:
    /*
     * Set CA path info for SSL connection proxy. Specify directory name of the
     * CA certificates which have been prepared using openssl c_rehash utility.
     */
#ifdef USE_SSL
    जर(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH_PROXY, ptr);
      जर(!result)
        s->proxy_ssl.custom_capath = TRUE;
      परत result;
    }
#endif
    परत CURLE_NOT_BUILT_IN;
  पूर्वनिर्धारित:
    परत CURLE_UNKNOWN_OPTION;
  }
  परत result;
}
#endif

#जर !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
/*
 * A string with POST data. Makes curl HTTP POST. Even if it is NULL. If
 * needed, CURLOPT_POSTFIELDSIZE must have been set prior to
 * CURLOPT_COPYPOSTFIELDS and not altered later.
 */
स्थिर CURLcode setopt_copypostfields(स्थिरांक अक्षर *ptr, संरचना UserDefined *s)
{
  CURLcode result = CURLE_OK;
  जर(s->postfieldsize < -1)
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  जर(!ptr || s->postfieldsize == -1) {
    जर(ptr && (म_माप(ptr) > CURL_MAX_आदान_LENGTH))
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_safefree(s->str_copypostfields);
    जर(ptr) {
      s->str_copypostfields = curlx_strdup(ptr);
      जर(!s->str_copypostfields)
        परत CURLE_OUT_OF_MEMORY;
    }
  }
  अन्यथा {
    माप_प्रकार pflen = curlx_sotouz_range(s->postfieldsize, 0, SIZE_MAX);
    जर(pflen == SIZE_MAX)
      परत CURLE_OUT_OF_MEMORY;
    अन्यथा {
      /* Allocate even when size == 0. This satisfies the need of possible
         later address compare to detect the COPYPOSTFIELDS mode, and to mark
         that postfields is used rather than read function or form data. */
      अक्षर *p = curlx_memdup0(ptr, pflen);
      जर(!p)
        परत CURLE_OUT_OF_MEMORY;
      अन्यथा {
        curlx_free(s->str_copypostfields);
        s->str_copypostfields = p;
      }
    }
  }

  s->postfields = s->str_copypostfields;
  s->method = HTTPREQ_POST;
  परत result;
}
#endif

#ifdef USE_ECH
स्थिर CURLcode setopt_ech(संरचना Curl_easy *data, स्थिरांक अक्षर *ptr)
{
  संरचना UserDefined *s = &data->set;
  CURLcode result = CURLE_OK;

  जर(!ptr || !म_भेद(ptr, "false"))
    s->ssl.ech = CURLECH_DISABLE;
  अन्यथा {
    माप_प्रकार plen = म_माप(ptr);
    जर(plen > CURL_MAX_आदान_LENGTH)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा {
      जर(!म_भेद(ptr, "grease"))
        s->ssl.ech = CURLECH_GREASE;
      अन्यथा जर(!म_भेद(ptr, "true"))
        s->ssl.ech = CURLECH_ENABLE;
      अन्यथा जर(!म_भेद(ptr, "hard"))
        s->ssl.ech = CURLECH_HARD;
      अन्यथा जर(plen > 4 && !म_भेदन(ptr, "ecl:", 4)) {
        जर(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_CONFIG, ptr + 4);
      }
      अन्यथा जर(plen > 3 && !म_भेदन(ptr, "pn:", 3)) {
        जर(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_PUBLIC, ptr + 3);
      }
      अन्यथा
        result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  परत result;
}
#अन्यथा
#define setopt_ech(x, y) CURLE_NOT_BUILT_IN
#endif

#जर defined(USE_SSL) || defined(USE_SSH)
/* One of the options is used for both TLS and SSH */
स्थिर CURLcode setopt_cptr_ssl(संरचना Curl_easy *data, CURLoption option,
                                अक्षर *ptr)
{
  CURLcode result = CURLE_OK;

  निवड(option) {
  स्थिती CURLOPT_KEYPASSWD:
    /*
     * String that holds the SSL or SSH private key password.
     */
    result = Curl_setstropt(data, STRING_किल्ली_PASSWD, ptr);
    थांबा;
#ifdef USE_SSL
  स्थिती CURLOPT_CAINFO:
    /*
     * Set CA info for SSL connection. Specify filename of the CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE, ptr);
    जर(!result)
      data->set.ssl.custom_cafile = TRUE;
    परत result;
  स्थिती CURLOPT_CAPATH:
    /*
     * Set CA path info for SSL connection. Specify directory name of the CA
     * certificates which have been prepared using openssl c_rehash utility.
     */
    जर(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH, ptr);
      जर(!result)
        data->set.ssl.custom_capath = TRUE;
      परत result;
    }
    परत CURLE_NOT_BUILT_IN;
  स्थिती CURLOPT_CRLFILE:
    /*
     * Set CRL file info for SSL connection. Specify filename of the CRL
     * to check certificates revocation
     */
    जर(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      परत Curl_setstropt(data, STRING_SSL_CRLFILE, ptr);
    परत CURLE_NOT_BUILT_IN;
  स्थिती CURLOPT_SSL_CIPHER_LIST:
    जर(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST))
      /* set a list of cipher we want to use in the SSL connection */
      परत Curl_setstropt(data, STRING_SSL_CIPHER_LIST, ptr);
    अन्यथा
      परत CURLE_NOT_BUILT_IN;
  स्थिती CURLOPT_TLS13_CIPHERS:
    जर(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites */
      परत Curl_setstropt(data, STRING_SSL_CIPHER13_LIST, ptr);
    अन्यथा
      परत CURLE_NOT_BUILT_IN;
  स्थिती CURLOPT_RANDOM_FILE:
    थांबा;
  स्थिती CURLOPT_EGDSOCKET:
    थांबा;
  स्थिती CURLOPT_SSL_CTX_DATA:
    /*
     * Set an SSL_CTX callback parameter pointer
     */
    जर(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      data->set.ssl_fsslctxp = ptr;
      थांबा;
    }
    अन्यथा
      परत CURLE_NOT_BUILT_IN;
  स्थिती CURLOPT_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use
     */
    परत Curl_setstropt(data, STRING_CERT, ptr);
  स्थिती CURLOPT_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use
     */
    परत Curl_setstropt(data, STRING_CERT_TYPE, ptr);
  स्थिती CURLOPT_SSLKEY:
    /*
     * String that holds filename of the SSL key to use
     */
    परत Curl_setstropt(data, STRING_किल्ली, ptr);
  स्थिती CURLOPT_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use
     */
    परत Curl_setstropt(data, STRING_किल्ली_TYPE, ptr);
  स्थिती CURLOPT_SSLENGINE:
    /*
     * String that holds the SSL crypto engine.
     */
    जर(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      परत CURLE_NOT_BUILT_IN;
    result = Curl_setstropt(data, STRING_SSL_ENGINE, ptr);
    थांबा;
  स्थिती CURLOPT_ISSUERCERT:
    /*
     * Set Issuer certificate file
     * to check certificates issuer
     */
    जर(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      परत Curl_setstropt(data, STRING_SSL_ISSUERCERT, ptr);
    परत CURLE_NOT_BUILT_IN;
  स्थिती CURLOPT_SSL_EC_CURVES:
    /*
     * Set accepted curves in SSL connection setup.
     * Specify colon-delimited list of curve algorithm names.
     */
    जर(Curl_ssl_supports(data, SSLSUPP_SSL_EC_CURVES))
      परत Curl_setstropt(data, STRING_SSL_EC_CURVES, ptr);
    परत CURLE_NOT_BUILT_IN;
  स्थिती CURLOPT_SSL_SIGNATURE_ALGORITHMS:
    /*
     * Set accepted signature algorithms.
     * Specify colon-delimited list of signature scheme names.
     */
    जर(Curl_ssl_supports(data, SSLSUPP_SIGNATURE_ALGORITHMS))
      परत Curl_setstropt(data, STRING_SSL_SIGNATURE_ALGORITHMS, ptr);
    परत CURLE_NOT_BUILT_IN;
  स्थिती CURLOPT_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
    जर(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      परत Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY, ptr);
    परत CURLE_NOT_BUILT_IN;
  स्थिती CURLOPT_ECH:
    परत setopt_ech(data, ptr);
#endif
  पूर्वनिर्धारित:
    परत CURLE_UNKNOWN_OPTION;
  }
  परत result;
}
#endif

#जर !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
स्थिर CURLcode setopt_cptr_http_mqtt(संरचना Curl_easy *data,
                                      CURLoption option, अक्षर *ptr)
{
  CURLcode result = CURLE_OK;
  संरचना UserDefined *s = &data->set;

  निवड(option) {
  स्थिती CURLOPT_COPYPOSTFIELDS:
    परत setopt_copypostfields(ptr, s);

  स्थिती CURLOPT_POSTFIELDS:
    /*
     * Like above, but use static data instead of copying it.
     */
    s->postfields = ptr;
    /* Release old copied data. */
    curlx_safefree(s->str_copypostfields);
    s->method = HTTPREQ_POST;
    थांबा;

#ifndef CURL_DISABLE_HTTP
  स्थिती CURLOPT_TRAILERDATA:
    s->trailer_data = ptr;
    थांबा;
  स्थिती CURLOPT_ACCEPT_ENCODING:
    /*
     * String to use at the value of Accept-Encoding header.
     *
     * If the encoding is set to "" we use an Accept-Encoding header that
     * encompasses all the encodings we support.
     * If the encoding is set to NULL we do not send an Accept-Encoding header
     * and ignore any received Content-Encoding header.
     *
     */
    जर(ptr && !*ptr) {
      ptr = Curl_get_content_encodings();
      जर(ptr)
        result = CURL_EASY_STR_SETN(data, STRING_ENCODING, ptr);
      अन्यथा
        result = CURLE_OUT_OF_MEMORY;
      परत result;
    }
    परत Curl_setstropt(data, STRING_ENCODING, ptr);

#ifndef CURL_DISABLE_AWS
  स्थिती CURLOPT_AWS_SIGV4:
    /*
     * String that is merged to some authentication
     * parameters are used by the algorithm.
     */
    result = Curl_setstropt(data, STRING_AWS_SIGV4, ptr);
    /*
     * Basic has been set by default; it needs to be unset here.
     */
    जर(CURL_EASY_STR(data, STRING_AWS_SIGV4))
      s->httpauth = CURLAUTH_AWS_SIGV4;
    अन्यथा
      s->httpauth &= ~(uint32_t)CURLAUTH_AWS_SIGV4;
    थांबा;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  स्थिती CURLOPT_HTTPSIG_किल्ली:
    result = Curl_setstropt(data, STRING_HTTPSIG_किल्ली, ptr);
    थांबा;
  स्थिती CURLOPT_HTTPSIG_KEYID:
    result = Curl_setstropt(data, STRING_HTTPSIG_KEYID, ptr);
    थांबा;
  स्थिती CURLOPT_HTTPSIG_HEADERS:
    result = Curl_setstropt(data, STRING_HTTPSIG_HEADERS, ptr);
    थांबा;
#endif
  स्थिती CURLOPT_REFERER: {
    /*
     * String to set in the HTTP Referer: field.
     */
    संरचना bufref *oldref = &data->state.referer;
    /* free the old after the storing the new in case the input is actually
       pointing back to this */
    result = Curl_setstropt(data, STRING_SET_REFERER, ptr);
    Curl_bufref_free(oldref);
    थांबा;
  }

  स्थिती CURLOPT_USERAGENT:
    /*
     * String to use in the HTTP User-Agent field
     */
    परत Curl_setstropt(data, STRING_USERAGENT, ptr);

#ifndef CURL_DISABLE_COOKIES
  स्थिती CURLOPT_COOKIE:
    /*
     * Cookie string to send to the remote server in the request.
     */
    परत Curl_setstropt(data, STRING_COOKIE, ptr);

  स्थिती CURLOPT_COOKIEFILE:
    परत cookiefile(data, ptr);

  स्थिती CURLOPT_COOKIEJAR:
    /*
     * Set cookie filename to dump all cookies to when we are done.
     */
    result = Curl_setstropt(data, STRING_COOKIEJAR, ptr);
    जर(!result) {
      /*
       * Activate the cookie parser. This may or may not already
       * have been made.
       */
      जर(!data->cookies)
        data->cookies = Curl_cookie_init();
      जर(!data->cookies)
        result = CURLE_OUT_OF_MEMORY;
      अन्यथा
        data->state.cookie_engine = TRUE;
    }
    थांबा;

  स्थिती CURLOPT_COOKIELIST:
    परत cookielist(data, ptr);
#endif /* !CURL_DISABLE_COOKIES */

#endif /* !CURL_DISABLE_HTTP */
  पूर्वनिर्धारित:
    परत CURLE_UNKNOWN_OPTION;
  }
  परत result;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_MQTT */

#ifdef USE_SSH
स्थिर CURLcode setopt_cptr_ssh(संरचना Curl_easy *data, CURLoption option,
                                अक्षर *ptr)
{
  संरचना UserDefined *s = &data->set;
  निवड(option) {
  स्थिती CURLOPT_SSH_PUBLIC_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa.pub file
     */
    परत Curl_setstropt(data, STRING_SSH_PUBLIC_किल्ली, ptr);
  स्थिती CURLOPT_SSH_PRIVATE_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa file
     */
    परत Curl_setstropt(data, STRING_SSH_PRIVATE_किल्ली, ptr);
  स्थिती CURLOPT_SSH_KEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_keyfunc_userp = ptr;
    थांबा;
  स्थिती CURLOPT_SSH_HOST_PUBLIC_किल्ली_MD5:
    /*
     * Option to allow for the MD5 of the host public key to be checked
     * for validation purposes.
     */
    परत Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_किल्ली_MD5, ptr);
  स्थिती CURLOPT_SSH_HOST_PUBLIC_किल्ली_SHA256:
    /*
     * Option to allow for the SHA256 of the host public key to be checked
     * for validation purposes.
     */
    परत Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_किल्ली_SHA256, ptr);
  स्थिती CURLOPT_SSH_KNOWNHOSTS:
    /*
     * Store the filename to read known hosts from.
     */
    परत Curl_setstropt(data, STRING_SSH_KNOWNHOSTS, ptr);
#ifdef USE_LIBSSH2
  स्थिती CURLOPT_SSH_HOSTKEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_hostkeyfunc_userp = ptr;
    थांबा;
#endif /* USE_LIBSSH2 */
  पूर्वनिर्धारित:
    परत CURLE_UNKNOWN_OPTION;
  }
  परत CURLE_OK;
}
#endif /* USE_SSH */

#ifndef CURL_DISABLE_FTP
स्थिर CURLcode setopt_cptr_ftp(संरचना Curl_easy *data, CURLoption option,
                                अक्षर *ptr)
{
  CURLcode result = CURLE_OK;
  संरचना UserDefined *s = &data->set;
  निवड(option) {
  स्थिती CURLOPT_FTPPORT:
    /*
     * Use FTP PORT, this also specifies which IP address to use
     */
    result = Curl_setstropt(data, STRING_FTPPORT, ptr);
    s->ftp_use_port = !!CURL_EASY_STR(data, STRING_FTPPORT);
    थांबा;

  स्थिती CURLOPT_FTP_ACCOUNT:
    परत Curl_setstropt(data, STRING_FTP_ACCOUNT, ptr);

  स्थिती CURLOPT_FTP_ALTERNATIVE_TO_USER:
    परत Curl_setstropt(data, STRING_FTP_ALTERNATIVE_TO_USER, ptr);

  स्थिती CURLOPT_KRBLEVEL:
    परत CURLE_NOT_BUILT_IN; /* removed in 8.17.0 */
  स्थिती CURLOPT_CHUNK_DATA:
    s->wildcardptr = ptr;
    थांबा;
  स्थिती CURLOPT_FNMATCH_DATA:
    s->fnmatch_data = ptr;
    थांबा;
  पूर्वनिर्धारित:
    परत CURLE_UNKNOWN_OPTION;
  }
  परत result;
}
#endif /* !CURL_DISABLE_FTP */

स्थिर CURLcode setopt_cptr_net(संरचना Curl_easy *data, CURLoption option,
                                अक्षर *ptr)
{
  निवड(option) {
  स्थिती CURLOPT_INTERFACE:
    /*
     * Set what interface or address/hostname to bind the socket to when
     * performing an operation and thus what from-IP your connection will use.
     */
    परत setstropt_interface(data, ptr);

#ifdef USE_RESOLV_ARES
  स्थिती CURLOPT_DNS_SERVERS:
    परत Curl_setstropt(data, STRING_DNS_SERVERS, ptr);

  स्थिती CURLOPT_DNS_INTERFACE:
    परत Curl_setstropt(data, STRING_DNS_INTERFACE, ptr);

  स्थिती CURLOPT_DNS_LOCAL_IP4:
    परत Curl_setstropt(data, STRING_DNS_LOCAL_IP4, ptr);

  स्थिती CURLOPT_DNS_LOCAL_IP6:
    परत Curl_setstropt(data, STRING_DNS_LOCAL_IP6, ptr);
#endif
#ifdef USE_UNIX_SOCKETS
  स्थिती CURLOPT_UNIX_SOCKET_PATH:
    data->set.abstract_unix_socket = FALSE;
    परत Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);

  स्थिती CURLOPT_ABSTRACT_UNIX_SOCKET:
    data->set.abstract_unix_socket = TRUE;
    परत Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);
#endif
#ifndef CURL_DISABLE_DOH
  स्थिती CURLOPT_DOH_URL:
    {
      CURLcode result = Curl_setstropt(data, STRING_DOH, ptr);
      data->set.doh = !!CURL_EASY_STR(data, STRING_DOH);
      परत result;
    }
#endif
  पूर्वनिर्धारित:
    परत CURLE_UNKNOWN_OPTION;
  }
}

स्थिर CURLcode setopt_cptr_misc(संरचना Curl_easy *data, CURLoption option,
                                 अक्षर *ptr)
{
  CURLcode result = CURLE_OK;
  संरचना UserDefined *s = &data->set;

  निवड(option) {
  स्थिती CURLOPT_REQUEST_TARGET:
    परत Curl_setstropt(data, STRING_TARGET, ptr);
#ifndef CURL_DISABLE_NETRC
  स्थिती CURLOPT_NETRC_FILE:
    परत Curl_setstropt(data, STRING_NETRC_FILE, ptr);
#endif
  स्थिती CURLOPT_CUSTOMREQUEST:
    परत Curl_setstropt(data, STRING_CUSTOMREQUEST, ptr);

    /* we do not set s->method = HTTPREQ_CUSTOM; here, we continue as if we
       were using the already set type and this changes the actual request
       keyword */
  स्थिती CURLOPT_SERVICE_NAME:
    परत Curl_setstropt(data, STRING_SERVICE_NAME, ptr);

  स्थिती CURLOPT_HEADERDATA:
    s->writeheader = ptr;
    थांबा;
  स्थिती CURLOPT_READDATA:
    s->in_set = ptr;
    थांबा;
  स्थिती CURLOPT_WRITEDATA:
    s->out = ptr;
    थांबा;
  स्थिती CURLOPT_DEBUGDATA:
    s->debugdata = ptr;
    थांबा;
  स्थिती CURLOPT_PROGRESSDATA:
    s->progress_client = ptr;
    थांबा;
  स्थिती CURLOPT_SEEKDATA:
    s->seek_client = ptr;
    थांबा;
  स्थिती CURLOPT_IOCTLDATA:
    s->ioctl_client = ptr;
    थांबा;
  स्थिती CURLOPT_SOCKOPTDATA:
    s->sockopt_client = ptr;
    थांबा;
  स्थिती CURLOPT_OPENSOCKETDATA:
    s->opensocket_client = ptr;
    थांबा;
  स्थिती CURLOPT_RESOLVER_START_DATA:
    s->resolver_start_client = ptr;
    थांबा;
  स्थिती CURLOPT_CLOSESOCKETDATA:
    s->closesocket_client = ptr;
    थांबा;
  स्थिती CURLOPT_PREREQDATA:
    s->prereq_userp = ptr;
    थांबा;
  स्थिती CURLOPT_ERRORBUFFER:
    s->errorbuffer = ptr;
    थांबा;
  स्थिती CURLOPT_URL:
    result = Curl_setstropt(data, STRING_SET_URL, ptr);
    Curl_bufref_set(&data->state.url,
                    CURL_EASY_STR(data, STRING_SET_URL), 0, रिक्त);
    थांबा;

  स्थिती CURLOPT_USERPWD: {
    अक्षर *u = रिक्त, *p = रिक्त;
    result = setstropt_userpwd(ptr, &u, &p);
    जर(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_USERNAME, u);
      u = रिक्त;
    }
    जर(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_PASSWORD, p);
      p = रिक्त;
    }
    curlx_free(u);
    curlx_free(p);
    परत result;
  }

  स्थिती CURLOPT_USERNAME:
    परत Curl_setstropt(data, STRING_USERNAME, ptr);

  स्थिती CURLOPT_PASSWORD:
    परत Curl_setstropt(data, STRING_PASSWORD, ptr);

  स्थिती CURLOPT_LOGIN_OPTIONS:
    परत Curl_setstropt(data, STRING_OPTIONS, ptr);

  स्थिती CURLOPT_XOAUTH2_BEARER:
    परत Curl_setstropt(data, STRING_BEARER, ptr);
  स्थिती CURLOPT_RANGE:
    परत Curl_setstropt(data, STRING_SET_RANGE, ptr);
  स्थिती CURLOPT_PRIVATE:
    s->private_data = ptr;
    थांबा;
  स्थिती CURLOPT_PROTOCOLS_STR:
    जर(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      जर(!result)
        s->allowed_protocols = protos;
    }
    अन्यथा
      /* make a NULL argument reset to default */
      s->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
    थांबा;
  स्थिती CURLOPT_REDIR_PROTOCOLS_STR:
    जर(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      जर(!result)
        s->redir_protocols = protos;
    }
    अन्यथा
      /* make a NULL argument reset to default */
      s->redir_protocols = (curl_prot_t)CURLPROTO_REDIR;
    थांबा;
  स्थिती CURLOPT_DEFAULT_PROTOCOL:
    /* Set the protocol to use when the URL does not include any protocol */
    परत Curl_setstropt(data, STRING_DEFAULT_PROTOCOL, ptr);
#ifndef CURL_DISABLE_SMTP
  स्थिती CURLOPT_MAIL_FROM:
    /* Set the SMTP mail originator */
    परत Curl_setstropt(data, STRING_MAIL_FROM, ptr);
  स्थिती CURLOPT_MAIL_AUTH:
    /* Set the SMTP auth originator */
    परत Curl_setstropt(data, STRING_MAIL_AUTH, ptr);
#endif
  स्थिती CURLOPT_SASL_AUTHZID:
    /* Authorization identity (identity to act as) */
    परत Curl_setstropt(data, STRING_SASL_AUTHZID, ptr);
#ifndef CURL_DISABLE_RTSP
  स्थिती CURLOPT_RTSP_SESSION_ID:
    Curl_peer_unlink(&data->state.rtsp_session_origin);
    परत Curl_setstropt(data, STRING_RTSP_SESSION_ID, ptr);
  स्थिती CURLOPT_RTSP_STREAM_URI:
    परत Curl_setstropt(data, STRING_RTSP_STREAM_URI, ptr);
  स्थिती CURLOPT_RTSP_TRANSPORT:
    परत Curl_setstropt(data, STRING_RTSP_TRANSPORT, ptr);
  स्थिती CURLOPT_INTERLEAVEDATA:
    s->rtp_out = ptr;
    थांबा;
#endif /* !CURL_DISABLE_RTSP */
  स्थिती CURLOPT_TLSAUTH_USERNAME:
  स्थिती CURLOPT_TLSAUTH_PASSWORD:
  स्थिती CURLOPT_TLSAUTH_TYPE:
  स्थिती CURLOPT_PROXY_TLSAUTH_USERNAME:
  स्थिती CURLOPT_PROXY_TLSAUTH_PASSWORD:
  स्थिती CURLOPT_PROXY_TLSAUTH_TYPE:
    परत CURLE_NOT_BUILT_IN;
#ifndef CURL_DISABLE_HSTS
  स्थिती CURLOPT_HSTSREADDATA:
    s->hsts_read_userp = ptr;
    थांबा;
  स्थिती CURLOPT_HSTSWRITEDATA:
    s->hsts_write_userp = ptr;
    थांबा;
  स्थिती CURLOPT_HSTS: {
    संरचना curl_slist *h;
    जर(!data->hsts) {
      data->hsts = Curl_hsts_init();
      जर(!data->hsts)
        परत CURLE_OUT_OF_MEMORY;
    }
    जर(ptr) {
      result = Curl_setstropt(data, STRING_HSTS, ptr);
      जर(result)
        परत result;
      /* this needs to build a list of filenames to read from, so that it can
         read them later, as we might get a shared HSTS handle to load them
         into */
      h = curl_slist_append(data->state.hstslist, ptr);
      जर(!h) {
        curl_slist_free_all(data->state.hstslist);
        data->state.hstslist = रिक्त;
        परत CURLE_OUT_OF_MEMORY;
      }
      data->state.hstslist = h; /* store the list for later use */
    }
    अन्यथा {
      /* clear the list of HSTS files */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = रिक्त;
      जर(!data->share || !data->share->hsts)
        /* throw away the HSTS cache unless shared */
        Curl_hsts_cleanup(&data->hsts);
    }
    थांबा;
  }
#endif /* !CURL_DISABLE_HSTS */
#ifndef CURL_DISABLE_ALTSVC
  स्थिती CURLOPT_ALTSVC:
    जर(!data->asi) {
      data->asi = Curl_altsvc_init();
      जर(!data->asi)
        परत CURLE_OUT_OF_MEMORY;
    }
    result = Curl_setstropt(data, STRING_ALTSVC, ptr);
    जर(result)
      थांबा;
    जर(ptr)
      परत Curl_altsvc_load(data->asi, ptr);
    थांबा;
#endif /* !CURL_DISABLE_ALTSVC */
  स्थिती CURLOPT_ECH:
    परत setopt_ech(data, ptr);
  पूर्वनिर्धारित:
    परत CURLE_UNKNOWN_OPTION;
  }
  परत result;
}

स्थिर CURLcode setopt_cptr(संरचना Curl_easy *data, CURLoption option,
                            अक्षर *ptr)
{
  प्रकारव्याख्या CURLcode (*ptrfunc)(संरचना Curl_easy *data, CURLoption option,
                              अक्षर *ptr);
  /* Order by likeliness */
  स्थिर स्थिरांक ptrfunc setopt_call[] = {
    setopt_cptr_misc,
#जर defined(USE_SSL) || defined(USE_SSH)
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
#जर !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
    setopt_cptr_http_mqtt,
#endif
  };
  माप_प्रकार i;

  साठी(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, ptr);
    जर(result != CURLE_UNKNOWN_OPTION)
      परत result;
  }
  परत CURLE_UNKNOWN_OPTION;
}

स्थिर CURLcode setopt_func(संरचना Curl_easy *data, CURLoption option,
                            बहु_सूची param)
{
  संरचना UserDefined *s = &data->set;
  निवड(option) {
  स्थिती CURLOPT_PROGRESSFUNCTION:
    /*
     * Progress callback function
     */
    s->fprogress = बहु_तर्क(param, curl_progress_callback);
    जर(s->fprogress)
      data->progress.callback = TRUE; /* no longer internal */
    अन्यथा
      data->progress.callback = FALSE; /* NULL enforces internal */
    थांबा;

  स्थिती CURLOPT_XFERINFOFUNCTION:
    /*
     * Transfer info callback function
     */
    s->fxferinfo = बहु_तर्क(param, curl_xferinfo_callback);
    जर(s->fxferinfo)
      data->progress.callback = TRUE; /* no longer internal */
    अन्यथा
      data->progress.callback = FALSE; /* NULL enforces internal */

    थांबा;
  स्थिती CURLOPT_DEBUGFUNCTION:
    /*
     * stderr write callback.
     */
    s->fdebug = बहु_तर्क(param, curl_debug_callback);
    /*
     * if the callback provided is NULL, it will use the default callback
     */
    थांबा;
  स्थिती CURLOPT_HEADERFUNCTION:
    /*
     * Set header write callback
     */
    s->fwrite_header = बहु_तर्क(param, curl_write_callback);
    थांबा;
  स्थिती CURLOPT_WRITEFUNCTION:
    /*
     * Set data write callback
     */
    s->fwrite_func = बहु_तर्क(param, curl_write_callback);
    जर(!s->fwrite_func)
#जर defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ढकला
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fwrite_func = (curl_write_callback)ख_डालो;
#जर defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic काढा
#endif
    थांबा;
  स्थिती CURLOPT_READFUNCTION:
    /*
     * Read data callback
     */
    s->fread_func_set = बहु_तर्क(param, curl_read_callback);
    जर(!s->fread_func_set) {
      s->is_fread_set = 0;
#जर defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ढकला
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fread_func_set = (curl_read_callback)ख_पढ़ो;
#जर defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic काढा
#endif
    }
    अन्यथा
      s->is_fread_set = 1;
    थांबा;
  स्थिती CURLOPT_SEEKFUNCTION:
    /*
     * Seek callback. Might be NULL.
     */
    s->seek_func = बहु_तर्क(param, curl_seek_callback);
    थांबा;
  स्थिती CURLOPT_IOCTLFUNCTION:
    /*
     * I/O control callback. Might be NULL.
     */
    s->ioctl_func = बहु_तर्क(param, curl_ioctl_callback);
    थांबा;
  स्थिती CURLOPT_SSL_CTX_FUNCTION:
    /*
     * Set an SSL_CTX callback
     */
#ifdef USE_SSL
    जर(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      s->ssl_fsslctx = बहु_तर्क(param, curl_ssl_ctx_callback);
      थांबा;
    }
    अन्यथा
#endif
      परत CURLE_NOT_BUILT_IN;

  स्थिती CURLOPT_SOCKOPTFUNCTION:
    /*
     * socket callback function: called after socket() but before connect()
     */
    s->fsockopt = बहु_तर्क(param, curl_sockopt_callback);
    थांबा;

  स्थिती CURLOPT_OPENSOCKETFUNCTION:
    /*
     * open/create socket callback function: called instead of socket(),
     * before connect()
     */
    s->fopensocket = बहु_तर्क(param, curl_opensocket_callback);
    थांबा;

  स्थिती CURLOPT_CLOSESOCKETFUNCTION:
    /*
     * close socket callback function: called instead of close()
     * when shutting down a connection
     */
    s->fclosesocket = बहु_तर्क(param, curl_closesocket_callback);
    थांबा;

  स्थिती CURLOPT_RESOLVER_START_FUNCTION:
    /*
     * resolver start callback function: called before a new resolver request
     * is started
     */
    s->resolver_start = बहु_तर्क(param, curl_resolver_start_callback);
    थांबा;

#ifdef USE_SSH
#ifdef USE_LIBSSH2
  स्थिती CURLOPT_SSH_HOSTKEYFUNCTION:
    /* the callback to check the hostkey without the knownhost file */
    s->ssh_hostkeyfunc = बहु_तर्क(param, curl_sshhostkeycallback);
    थांबा;
#endif

  स्थिती CURLOPT_SSH_KEYFUNCTION:
    /* setting to NULL is fine since the ssh.c functions themselves will
       then revert to use the internal default */
    s->ssh_keyfunc = बहु_तर्क(param, curl_sshkeycallback);
    थांबा;

#endif /* USE_SSH */

#ifndef CURL_DISABLE_RTSP
  स्थिती CURLOPT_INTERLEAVEFUNCTION:
    /* Set the user defined RTP write function */
    s->fwrite_rtp = बहु_तर्क(param, curl_write_callback);
    थांबा;
#endif
#ifndef CURL_DISABLE_FTP
  स्थिती CURLOPT_CHUNK_BGN_FUNCTION:
    s->chunk_bgn = बहु_तर्क(param, curl_chunk_bgn_callback);
    थांबा;
  स्थिती CURLOPT_CHUNK_END_FUNCTION:
    s->chunk_end = बहु_तर्क(param, curl_chunk_end_callback);
    थांबा;
  स्थिती CURLOPT_FNMATCH_FUNCTION:
    s->fnmatch = बहु_तर्क(param, curl_fnmatch_callback);
    थांबा;
#endif
#ifndef CURL_DISABLE_HTTP
  स्थिती CURLOPT_TRAILERFUNCTION:
    s->trailer_callback = बहु_तर्क(param, curl_trailer_callback);
    थांबा;
#endif
#ifndef CURL_DISABLE_HSTS
  स्थिती CURLOPT_HSTSREADFUNCTION:
    s->hsts_read = बहु_तर्क(param, curl_hstsread_callback);
    थांबा;
  स्थिती CURLOPT_HSTSWRITEFUNCTION:
    s->hsts_write = बहु_तर्क(param, curl_hstswrite_callback);
    थांबा;
#endif
  स्थिती CURLOPT_PREREQFUNCTION:
    s->fprereq = बहु_तर्क(param, curl_prereq_callback);
    थांबा;
  पूर्वनिर्धारित:
    परत CURLE_UNKNOWN_OPTION;
  }
  परत CURLE_OK;
}

स्थिर CURLcode setopt_offt(संरचना Curl_easy *data, CURLoption option,
                            curl_off_t offt)
{
  संरचना UserDefined *s = &data->set;
  निवड(option) {
  स्थिती CURLOPT_TIMEVALUE_LARGE:
    /*
     * This is the value to compare with the remote document with the
     * method set with CURLOPT_TIMECONDITION
     */
    s->timevalue = (समय_प्रकार)offt;
    थांबा;

    /* MQTT "borrows" some of the HTTP options */
  स्थिती CURLOPT_POSTFIELDSIZE_LARGE:
    /*
     * The size of the POSTFIELD data to prevent libcurl to do strlen() to
     * figure it out. Enables binary posts.
     */
    जर(offt < -1)
      परत CURLE_BAD_FUNCTION_ARGUMENT;

    जर(s->postfieldsize < offt && s->str_copypostfields) {
      /* Previous CURLOPT_COPYPOSTFIELDS is no longer valid. */
      curlx_safefree(s->str_copypostfields);
      s->postfields = रिक्त;
    }
    s->postfieldsize = offt;
    थांबा;
  स्थिती CURLOPT_INFILESIZE_LARGE:
    /*
     * If known, this should inform curl about the file size of the
     * to-be-uploaded file.
     */
    जर(offt < -1)
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = offt;
    थांबा;
  स्थिती CURLOPT_MAX_SEND_SPEED_LARGE:
    /*
     * When transfer uploads are faster than CURLOPT_MAX_SEND_SPEED_LARGE
     * bytes per second the transfer is throttled..
     */
    जर(offt < 0)
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_send_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.ul.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    थांबा;
  स्थिती CURLOPT_MAX_RECV_SPEED_LARGE:
    /*
     * When receiving data faster than CURLOPT_MAX_RECV_SPEED_LARGE bytes per
     * second the transfer is throttled..
     */
    जर(offt < 0)
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_recv_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.dl.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    थांबा;
  स्थिती CURLOPT_RESUME_FROM_LARGE:
    /*
     * Resume transfer at the given file position
     */
    जर(offt < -1)
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = offt;
    थांबा;
  स्थिती CURLOPT_MAXFILESIZE_LARGE:
    /*
     * Set the maximum size of a file to download.
     */
    जर(offt < 0)
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_filesize = offt;
    थांबा;

  पूर्वनिर्धारित:
    परत CURLE_UNKNOWN_OPTION;
  }
  परत CURLE_OK;
}

स्थिर CURLcode setopt_blob(संरचना Curl_easy *data, CURLoption option,
                            संरचना curl_blob *blob)
{
  संरचना UserDefined *s = &data->set;
  निवड(option) {
  स्थिती CURLOPT_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use
     */
    परत Curl_setblobopt(&s->blobs[BLOB_CERT], blob);
#ifndef CURL_DISABLE_PROXY
  स्थिती CURLOPT_PROXY_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use for proxy
     */
    परत Curl_setblobopt(&s->blobs[BLOB_CERT_PROXY], blob);
  स्थिती CURLOPT_PROXY_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use for proxy
     */
    परत Curl_setblobopt(&s->blobs[BLOB_किल्ली_PROXY], blob);
  स्थिती CURLOPT_PROXY_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection proxy.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    जर(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO_PROXY], blob);
      जर(!result)
        s->proxy_ssl.custom_cablob = TRUE;
      परत result;
    }
#endif
    परत CURLE_NOT_BUILT_IN;
  स्थिती CURLOPT_PROXY_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    जर(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      परत Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT_PROXY], blob);
    परत CURLE_NOT_BUILT_IN;
#endif
  स्थिती CURLOPT_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use
     */
    परत Curl_setblobopt(&s->blobs[BLOB_किल्ली], blob);
  स्थिती CURLOPT_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    जर(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO], blob);
      जर(!result)
        s->ssl.custom_cablob = TRUE;
      परत result;
    }
#endif
    परत CURLE_NOT_BUILT_IN;
  स्थिती CURLOPT_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    जर(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      परत Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT], blob);
    परत CURLE_NOT_BUILT_IN;

  पूर्वनिर्धारित:
    परत CURLE_UNKNOWN_OPTION;
  }
  /* unreachable */
}

/*
 * Do not make Curl_vsetopt() static: it is called from
 * projects/OS400/ccsidcurl.c.
 */
CURLcode Curl_vsetopt(संरचना Curl_easy *data, CURLoption option, बहु_सूची param)
{
  जर(option < CURLOPTTYPE_OBJECTPOINT)
    परत setopt_long(data, option, बहु_तर्क(param, दीर्घ));
  अन्यथा जर(option < CURLOPTTYPE_FUNCTIONPOINT) {
    /* unfortunately, different pointer types cannot be identified any other
       way than being listed explicitly */
    निवड(option) {
    स्थिती CURLOPT_HTTPHEADER:
    स्थिती CURLOPT_QUOTE:
    स्थिती CURLOPT_POSTQUOTE:
    स्थिती CURLOPT_TELNETOPTIONS:
    स्थिती CURLOPT_PREQUOTE:
    स्थिती CURLOPT_HTTP200ALIASES:
    स्थिती CURLOPT_MAIL_RCPT:
    स्थिती CURLOPT_RESOLVE:
    स्थिती CURLOPT_PROXYHEADER:
    स्थिती CURLOPT_CONNECT_TO:
      परत setopt_slist(data, option, बहु_तर्क(param, संरचना curl_slist *));
    स्थिती CURLOPT_HTTPPOST:         /* curl_httppost * */
    स्थिती CURLOPT_MIMEPOST:         /* curl_mime * */
    स्थिती CURLOPT_STDERR:           /* FILE * */
    स्थिती CURLOPT_SHARE:            /* CURLSH * */
    स्थिती CURLOPT_CURLU:            /* CURLU * */
      परत setopt_pointers(data, option, param);
    स्थिती CURLOPT_STREAM_DEPENDS:   /* CURL * */
    स्थिती CURLOPT_STREAM_DEPENDS_E: /* CURL * */
      परत CURLE_OK;
    पूर्वनिर्धारित:
      थांबा;
    }
    /* the char pointer options */
    परत setopt_cptr(data, option, बहु_तर्क(param, अक्षर *));
  }
  अन्यथा जर(option < CURLOPTTYPE_OFF_T)
    परत setopt_func(data, option, param);
  अन्यथा जर(option < CURLOPTTYPE_BLOB)
    परत setopt_offt(data, option, बहु_तर्क(param, curl_off_t));
  परत setopt_blob(data, option, बहु_तर्क(param, संरचना curl_blob *));
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

  जर(CURL_EAPI_ENTER(&guard, curl, easy_setopt, &result)) {
    संरचना Curl_easy *data = curl;
    बहु_सूची arg;

    बहु_शुरू(arg, option);

    result = Curl_vsetopt(data, option, arg);

    बहु_पूर्ण(arg);
    जर(result == CURLE_BAD_FUNCTION_ARGUMENT)
      failf(data, "setopt 0x%x got bad argument", (अचिन्हित पूर्णांक)option);
  }
  CURL_EAPI_LEAVE(&guard);
  परत result;
}
