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
#समावेश "curl_setup.h"

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_LINUX_TCP_H
#समावेश <linux/tcp.h>
#या_अगर defined(HAVE_NETINET_TCP_H)
#समावेश <netinet/tcp.h>
#पूर्ण_अगर

#समावेश "urldata.h"
#समावेश "url.h"
#समावेश "progress.h"
#समावेश "content_encoding.h"
#समावेश "strcase.h"
#समावेश "curl_share.h"
#समावेश "vtls/vtls.h"
#समावेश "curl_trc.h"
#समावेश "setopt.h"
#समावेश "altsvc.h"
#समावेश "hsts.h"
#समावेश "tftp.h"
#समावेश "curlx/strdup.h"
#समावेश "escape.h"
#समावेश "bufref.h"
#समावेश "vauth/vauth.h"

अटल CURLcode setopt_set_timeout_sec(timediff_t *ptimeout_ms, दीर्घ secs)
{
  अगर(secs < 0)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
#अगर दीर्घ_उच्च > (TIMEDIFF_T_MAX / 1000)
  अगर(secs > (TIMEDIFF_T_MAX / 1000)) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    वापस CURLE_OK;
  }
#पूर्ण_अगर
  *ptimeout_ms = (timediff_t)secs * 1000;
  वापस CURLE_OK;
}

अटल CURLcode setopt_set_timeout_ms(timediff_t *ptimeout_ms, दीर्घ ms)
{
  अगर(ms < 0)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
#अगर दीर्घ_उच्च > TIMEDIFF_T_MAX
  अगर(ms > TIMEDIFF_T_MAX) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    वापस CURLE_OK;
  }
#पूर्ण_अगर
  *ptimeout_ms = (timediff_t)ms;
  वापस CURLE_OK;
}

CURLcode Curl_setstropt(काष्ठा Curl_easy *तथ्य,
                        क्रमागत dupstring id, स्थिर अक्षर *s)
{
  माप_प्रकार slen = s ? म_माप(s) : 0;
  DEBUGASSERT((अचिन्हित)id <= UINT8_MAX);
  अगर(s && (slen > CURL_MAX_निवेश_लंबाई))
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  वापस CURL_EASY_STR_SET(तथ्य, (uint8_t)id, s, slen);
}

CURLcode Curl_setblobopt(काष्ठा curl_blob **blobp,
                         स्थिर काष्ठा curl_blob *blob)
{
  /* free the previous storage at `blobp' and replace by a dynamic storage
     copy of blob. If CURL_BLOB_COPY is set, the data is copied. */

  curlx_safefree(*blobp);

  अगर(blob) {
    काष्ठा curl_blob *nblob;
    अगर(!blob->तथ्य || !blob->len || (blob->len > CURL_MAX_निवेश_लंबाई))
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    nblob = (काष्ठा curl_blob *)
      curlx_malloc(माप(काष्ठा curl_blob) +
                   ((blob->flags & CURL_BLOB_COPY) ? blob->len : 0));
    अगर(!nblob)
      वापस CURLE_OUT_OF_MEMORY;
    *nblob = *blob;
    अगर(blob->flags & CURL_BLOB_COPY) {
      /* put the data after the blob struct in memory */
      nblob->तथ्य = (अक्षर *)nblob + माप(काष्ठा curl_blob);
      स_नकल(nblob->तथ्य, blob->तथ्य, blob->len);
    }

    *blobp = nblob;
    वापस CURLE_OK;
  }

  वापस CURLE_OK;
}

अटल CURLcode setstropt_userpwd(स्थिर अक्षर *option, अक्षर **userp,
                                  अक्षर **passwdp)
{
  अक्षर *user = शून्य;
  अक्षर *passwd = शून्य;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Parse the login details if specified. If not, then we treat NULL as a
     hint to clear the existing data */
  अगर(option) {
    माप_प्रकार len = म_माप(option);
    CURLcode परिणाम;
    अगर(len > CURL_MAX_निवेश_लंबाई)
      वापस CURLE_BAD_FUNCTION_ARGUMENT;

    परिणाम = Curl_parse_login_details(option, len, &user, &passwd, शून्य);
    अगर(परिणाम)
      वापस परिणाम;
  }

  curlx_free(*userp);
  *userp = user;

  curlx_strzero(*passwdp);
  curlx_free(*passwdp);
  *passwdp = passwd;

  वापस CURLE_OK;
}

अटल CURLcode setstropt_interface(काष्ठा Curl_easy *तथ्य, अक्षर *option)
{
  अगर(option && !option[0])
    option = शून्य;
  अगर(option) {
    /* Parse for checking syntax */
    CURLcode परिणाम = Curl_parse_interface(option, शून्य, शून्य, शून्य);
    अगर(परिणाम)
      वापस परिणाम;
    वापस CURL_EASY_STR_SET(तथ्य, STRING_INTERFACE_OPT,
                             option, म_माप(option));
  }
  CURL_EASY_STR_CLEAR(तथ्य, STRING_INTERFACE_OPT);
  वापस CURLE_OK;
}

#अगर_घोषित USE_SSL
#घोषणा C_SSLVERSION_मूल्य(x)     ((x) & 0xffff)
#घोषणा C_SSLVERSION_MAX_मूल्य(x) ((अचिन्हित दीर्घ)(x) & 0xffff0000)
#पूर्ण_अगर

अटल CURLcode protocol2num(स्थिर अक्षर *str, curl_prot_t *val)
{
  /*
   * We are asked to cherry-pick protocols, so play it safe and disallow all
   * protocols to start with, and re-add the wanted ones back in.
   */
  *val = 0;

  अगर(!str)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  अगर(curl_strequal(str, "all")) {
    *val = ~(curl_prot_t)0;
    वापस CURLE_OK;
  }

  करो {
    स्थिर अक्षर *token = str;
    माप_प्रकार tlen;

    str = म_अक्षर(str, ',');
    tlen = str ? (माप_प्रकार)(str - token) : म_माप(token);
    अगर(tlen) {
      स्थिर काष्ठा Curl_scheme *h = Curl_getn_scheme(token, tlen);

      अगर(!h || !h->run)
        वापस CURLE_UNSUPPORTED_PROTOCOL;

      *val |= h->protocol;
    }
  } जबतक(str && str++);

  अगर(!*val)
    /* no protocol listed */
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  वापस CURLE_OK;
}

#अगर !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
अटल CURLcode httpauth(काष्ठा Curl_easy *तथ्य, bool proxy,
                         अचिन्हित दीर्घ auth)
{
  अगर(auth != CURLAUTH_NONE) {
    पूर्णांक bitcheck = 0;
    bool authbits = FALSE;
    अगर(auth & CURLAUTH_DIGEST_IE) {
      auth |= CURLAUTH_DIGEST; /* set standard digest bit */
      auth &= ~CURLAUTH_DIGEST_IE; /* drop the legacy bit */
    }

    /* switch off bits we cannot support */
#अगर_अघोषित USE_NTLM
    auth &= ~CURLAUTH_NTLM; /* no NTLM support */
#पूर्ण_अगर
#अगर_अघोषित USE_SPNEGO
    auth &= ~CURLAUTH_NEGOTIATE; /* no Negotiate (SPNEGO) auth without GSS-API
                                    or SSPI */
#पूर्ण_अगर

    /* check if any auth bit lower than CURLAUTH_ONLY is still set */
    जबतक(bitcheck < 31) {
      अगर(auth & (1UL << bitcheck++)) {
        authbits = TRUE;
        अवरोध;
      }
    }
    अगर(!authbits)
      वापस CURLE_NOT_BUILT_IN; /* no supported types left! */
  }
  अगर(proxy)
    तथ्य->set.proxyauth = (uint32_t)auth;
  अन्यथा
    तथ्य->set.httpauth = (uint32_t)auth;
  वापस CURLE_OK;
}
#पूर्ण_अगर /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */

#अगर_अघोषित CURL_DISABLE_HTTP
अटल CURLcode setopt_HTTP_VERSION(काष्ठा Curl_easy *तथ्य, दीर्घ arg)
{
  /*
   * This sets a requested HTTP version to be used. The value is one of
   * the listed enums in curl/curl.h.
   */
  चयन(arg) {
  हाल CURL_HTTP_VERSION_NONE:
    /* accepted */
    अवरोध;
  हाल CURL_HTTP_VERSION_1_0:
  हाल CURL_HTTP_VERSION_1_1:
    /* accepted */
    अवरोध;
#अगर_घोषित USE_HTTP2
  हाल CURL_HTTP_VERSION_2_0:
  हाल CURL_HTTP_VERSION_2TLS:
  हाल CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    /* accepted */
    अवरोध;
#पूर्ण_अगर
#अगर_घोषित USE_HTTP3
  हाल CURL_HTTP_VERSION_3:
  हाल CURL_HTTP_VERSION_3ONLY:
    /* accepted */
    अवरोध;
#पूर्ण_अगर
  शेष:
    /* not accepted */
    अगर(arg < CURL_HTTP_VERSION_NONE)
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    वापस CURLE_UNSUPPORTED_PROTOCOL;
  }
  तथ्य->set.httpwant = (अचिन्हित अक्षर)arg;
  वापस CURLE_OK;
}
#पूर्ण_अगर /* !CURL_DISABLE_HTTP */

#अगर_घोषित USE_SSL
CURLcode Curl_setopt_SSLVERSION(काष्ठा Curl_easy *तथ्य, CURLoption option,
                                दीर्घ arg)
{
  /*
   * Set explicit SSL version to try to connect with, as some SSL
   * implementations are lame.
   */
  {
    दीर्घ version, version_max;
    काष्ठा ssl_easy_config *sslc = &तथ्य->set.ssl;
#अगर_अघोषित CURL_DISABLE_PROXY
    अगर(option != CURLOPT_SSLVERSION)
      sslc = &तथ्य->set.proxy_ssl;
#अन्यथा
    (व्योम)option; /* unused */
#पूर्ण_अगर
    version = C_SSLVERSION_मूल्य(arg);
    version_max = (दीर्घ)C_SSLVERSION_MAX_मूल्य(arg);

    अगर(version < CURL_SSLVERSION_DEFAULT ||
       version == CURL_SSLVERSION_SSLv2 ||
       version == CURL_SSLVERSION_SSLv3 ||
       version >= CURL_SSLVERSION_LAST ||
       version_max < CURL_SSLVERSION_MAX_NONE ||
       version_max >= CURL_SSLVERSION_MAX_LAST)
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    अगर(version == CURL_SSLVERSION_DEFAULT)
      version = CURL_SSLVERSION_TLSv1_2;

    sslc->version = (अचिन्हित अक्षर)version;
    sslc->version_max = (अचिन्हित पूर्णांक)version_max;
  }
  वापस CURLE_OK;
}
#पूर्ण_अगर /* !USE_SSL */

#अगर_अघोषित CURL_DISABLE_RTSP
अटल CURLcode setopt_RTSP_REQUEST(काष्ठा Curl_easy *तथ्य, दीर्घ arg)
{
  /*
   * Set the RTSP request method (OPTIONS, SETUP, PLAY, etc...) Would this be
   * better if the RTSPREQ_* were moved into here?
   */
  अगर((arg <= CURL_RTSPREQ_NONE) || (arg >= CURL_RTSPREQ_LAST))
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  तथ्य->set.rtspreq = (अचिन्हित अक्षर)arg;
  वापस CURLE_OK;
}
#पूर्ण_अगर /* !CURL_DISABLE_RTSP */

अटल CURLcode setopt_long_bool(काष्ठा Curl_easy *तथ्य, CURLoption option,
                                 दीर्घ arg)
{
  bool enabled = !!arg;
  पूर्णांक ok = 1;
  काष्ठा UserDefined *s = &तथ्य->set;
  चयन(option) {
  हाल CURLOPT_FORBID_REUSE:
    /*
     * When this transfer is done, it must not be left to be reused by a
     * subsequent transfer but shall be closed immediately.
     */
    s->reuse_forbid = enabled;
    अवरोध;
  हाल CURLOPT_FRESH_CONNECT:
    /*
     * This transfer shall not use a previously cached connection but
     * should be made with a fresh new connect!
     */
    s->reuse_fresh = enabled;
    अवरोध;
  हाल CURLOPT_VERBOSE:
    /*
     * Verbose means infof() calls that give a lot of information about
     * the connection and transfer procedures as well as internal choices.
     */
    s->verbose = enabled;
    अवरोध;
  हाल CURLOPT_HEADER:
    /*
     * Set to include the header in the general data output stream.
     */
    s->include_header = enabled;
    अवरोध;
  हाल CURLOPT_NOPROGRESS:
    /*
     * Shut off the internal supported progress meter
     */
    तथ्य->progress.hide = enabled;
    अवरोध;
  हाल CURLOPT_NOBODY:
    /*
     * Do not include the body part in the output data stream.
     */
    s->opt_no_body = enabled;
#अगर_अघोषित CURL_DISABLE_HTTP
    अगर(s->opt_no_body)
      /* in HTTP lingo, no body means using the HEAD request... */
      s->method = HTTPREQ_शीर्ष;
    अन्यथा अगर(s->method == HTTPREQ_शीर्ष)
      s->method = HTTPREQ_GET;
#पूर्ण_अगर
    अवरोध;
  हाल CURLOPT_FAILONERROR:
    /*
     * Do not output the >=400 error code HTML-page, but instead only
     * return error.
     */
    s->http_fail_on_error = enabled;
    अवरोध;
  हाल CURLOPT_KEEP_SENDING_ON_ERROR:
    s->http_keep_sending_on_error = enabled;
    अवरोध;
  हाल CURLOPT_UPLOAD:
  हाल CURLOPT_PUT:
    /*
     * We want to send data to the remote host. If this is HTTP, that equals
     * using the PUT request.
     */
    अगर(enabled) {
      /* If this is HTTP, PUT is what's needed to "upload" */
      s->method = HTTPREQ_PUT;
      s->opt_no_body = FALSE; /* this is implied */
    }
    अन्यथा
      /* In HTTP, the opposite of upload is GET (unless NOBODY is true as
         then this can be changed to HEAD later on) */
      s->method = HTTPREQ_GET;
    अवरोध;
  हाल CURLOPT_FILETIME:
    /*
     * Try to get the file time of the remote document. The time will
     * later (possibly) become available using curl_easy_getinfo().
     */
    s->get_filetime = enabled;
    अवरोध;
#अगर_अघोषित CURL_DISABLE_HTTP
  हाल CURLOPT_HTTP09_ALLOWED:
    s->http09_allowed = enabled;
    अवरोध;
#अगर_अघोषित CURL_DISABLE_COOKIES
  हाल CURLOPT_COOKIESESSION:
    /*
     * Set this option to TRUE to start a new "cookie session". It will
     * prevent the forthcoming read-cookies-from-file actions to accept
     * cookies that are marked as being session cookies, as they belong to a
     * previous session.
     */
    s->cookiesession = enabled;
    अवरोध;
#पूर्ण_अगर
  हाल CURLOPT_AUTOREFERER:
    /*
     * Switch on automatic referer that gets set if curl follows locations.
     */
    s->http_auto_referer = enabled;
    अवरोध;
  हाल CURLOPT_TRANSFER_ENCODING:
    s->http_transfer_encoding = enabled;
    अवरोध;
  हाल CURLOPT_UNRESTRICTED_AUTH:
    /*
     * Send authentication (user+password) when following locations, even when
     * hostname changed.
     */
    s->allow_auth_to_other_hosts = enabled;
    अवरोध;
  हाल CURLOPT_HTTP_TRANSFER_DECODING:
    /*
     * disable libcurl transfer encoding is used
     */
    s->http_te_skip = !enabled; /* reversed */
    अवरोध;
  हाल CURLOPT_HTTP_CONTENT_DECODING:
    /*
     * raw data passed to the application when content encoding is used
     */
    s->http_ce_skip = !enabled; /* reversed */
    अवरोध;
  हाल CURLOPT_HTTPGET:
    /*
     * Set to force us do HTTP GET
     */
    अगर(enabled) {
      s->method = HTTPREQ_GET;
      s->opt_no_body = FALSE; /* this is implied */
    }
    अवरोध;
  हाल CURLOPT_POST:
    /* Does this option serve a purpose anymore? Yes it does, when
       CURLOPT_POSTFIELDS is not used and the POST data is read off the
       callback! */
    अगर(enabled) {
      s->method = HTTPREQ_POST;
      s->opt_no_body = FALSE; /* this is implied */
    }
    अन्यथा
      s->method = HTTPREQ_GET;
    अवरोध;
#पूर्ण_अगर /* !CURL_DISABLE_HTTP */
#अगर_अघोषित CURL_DISABLE_PROXY
  हाल CURLOPT_HTTPPROXYTUNNEL:
    /*
     * Tunnel operations through the proxy instead of normal proxy use
     */
    s->tunnel_thru_httpproxy = enabled;
    अवरोध;
  हाल CURLOPT_HAPROXYPROTOCOL:
    /*
     * Set to send the HAProxy Proxy Protocol header
     */
    s->haproxyprotocol = enabled;
    अवरोध;
  हाल CURLOPT_PROXY_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for proxy.
     */
    s->proxy_ssl.verifypeer = enabled;

    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(तथ्य, TRUE);
    अवरोध;
  हाल CURLOPT_PROXY_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for proxy
     */
    s->proxy_ssl.verifyhost = enabled;
    ok = 2;
    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(तथ्य, TRUE);
    अवरोध;
  हाल CURLOPT_PROXY_TRANSFER_MODE:
    /*
     * set transfer mode (;type=<a|i>) when doing FTP via an HTTP proxy
     */
    s->proxy_transfer_mode = enabled;
    अवरोध;
#पूर्ण_अगर /* !CURL_DISABLE_PROXY */
#अगर defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  हाल CURLOPT_SOCKS5_GSSAPI_NEC:
    /*
     * Set flag for NEC SOCKS5 support
     */
    s->socks5_gssapi_nec = enabled;
    अवरोध;
#पूर्ण_अगर
#अगर_घोषित CURL_LIST_ONLY_PROTOCOL
  हाल CURLOPT_DIRLISTONLY:
    /*
     * An option that changes the command to one that asks for a list only, no
     * file info details. Used for FTP, POP3 and SFTP.
     */
    s->list_only = enabled;
    अवरोध;
#पूर्ण_अगर
  हाल CURLOPT_APPEND:
    /*
     * We want to upload and append to an existing file. Used for FTP and
     * SFTP.
     */
    s->remote_append = enabled;
    अवरोध;
#अगर_अघोषित CURL_DISABLE_FTP
  हाल CURLOPT_FTP_USE_EPRT:
    s->ftp_use_eprt = enabled;
    अवरोध;
  हाल CURLOPT_FTP_USE_EPSV:
    s->ftp_use_epsv = enabled;
    अवरोध;
  हाल CURLOPT_FTP_USE_PRET:
    s->ftp_use_pret = enabled;
    अवरोध;
  हाल CURLOPT_FTP_SKIP_PASV_IP:
    /*
     * Enable or disable FTP_SKIP_PASV_IP, which will disable/enable the
     * bypass of the IP address in PASV responses.
     */
    s->ftp_skip_ip = enabled;
    अवरोध;
  हाल CURLOPT_WILDCARDMATCH:
    s->wildcard_enabled = enabled;
    अवरोध;
#पूर्ण_अगर
  हाल CURLOPT_CRLF:
    /*
     * Kludgy option to enable CRLF conversions. Subject for removal.
     */
    s->crlf = enabled;
    अवरोध;
#अगर_अघोषित CURL_DISABLE_TFTP
  हाल CURLOPT_TFTP_NO_OPTIONS:
    /*
     * Option that prevents libcurl from sending TFTP option requests to the
     * server.
     */
    s->tftp_no_options = enabled;
    अवरोध;
#पूर्ण_अगर /* !CURL_DISABLE_TFTP */
  हाल CURLOPT_TRANSFERTEXT:
    /*
     * This option was previously named 'FTPASCII'. Renamed to work with
     * more protocols than merely FTP.
     *
     * Transfer using ASCII (instead of BINARY).
     */
    s->prefer_ascii = enabled;
    अवरोध;
  हाल CURLOPT_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying.
     */
    s->ssl.verifypeer = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(तथ्य, FALSE);
    अवरोध;
#अगर_अघोषित CURL_DISABLE_DOH
  हाल CURLOPT_DOH_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for DoH.
     */
    s->doh_verifypeer = enabled;
    अवरोध;
  हाल CURLOPT_DOH_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for DoH
     */
    s->doh_verifyhost = enabled;
    ok = 2;
    अवरोध;
  हाल CURLOPT_DOH_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying for DoH.
     */
    अगर(!Curl_ssl_cert_status_request())
      वापस CURLE_NOT_BUILT_IN;

    s->doh_verifystatus = enabled;
    ok = 2;
    अवरोध;
#पूर्ण_अगर /* !CURL_DISABLE_DOH */
  हाल CURLOPT_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate
     */

    /* Obviously people are not reading documentation and too many thought
       this argument took a boolean when it was not and misused it.
       Treat 1 and 2 the same */
    s->ssl.verifyhost = enabled;
    ok = 2;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(तथ्य, FALSE);
    अवरोध;
  हाल CURLOPT_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying.
     */
    अगर(!Curl_ssl_cert_status_request())
      वापस CURLE_NOT_BUILT_IN;

    s->ssl.verifystatus = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(तथ्य, FALSE);
    अवरोध;
  हाल CURLOPT_CERTINFO:
#अगर_घोषित USE_SSL
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_CERTINFO))
      s->ssl.certinfo = enabled;
    अन्यथा
#पूर्ण_अगर
      वापस CURLE_NOT_BUILT_IN;
    अवरोध;
  हाल CURLOPT_NOSIGNAL:
    /*
     * The application asks not to set any signal() or alarm() handlers,
     * even when using a timeout.
     */
    s->no_signal = enabled;
    अवरोध;
  हाल CURLOPT_TCP_NODELAY:
    /*
     * Enable or disable TCP_NODELAY, which will disable/enable the Nagle
     * algorithm
     */
    s->tcp_nodelay = enabled;
    अवरोध;
  हाल CURLOPT_IGNORE_CONTENT_लंबाई:
    s->ignorecl = enabled;
    अवरोध;
  हाल CURLOPT_SSL_SESSIONID_CACHE:
    s->ssl.cache_session = enabled;
#अगर_अघोषित CURL_DISABLE_PROXY
    s->proxy_ssl.cache_session = s->ssl.cache_session;
#पूर्ण_अगर
    अवरोध;
#अगर_घोषित USE_SSH
  हाल CURLOPT_SSH_COMPRESSION:
    s->ssh_compression = enabled;
    अवरोध;
#पूर्ण_अगर /* !USE_SSH */
#अगर_अघोषित CURL_DISABLE_SMTP
  हाल CURLOPT_MAIL_RCPT_ALLOWFAILS:
    /* allow RCPT TO command to fail for some recipients */
    s->mail_rcpt_allowfails = enabled;
    अवरोध;
#पूर्ण_अगर /* !CURL_DISABLE_SMTP */
  हाल CURLOPT_SASL_IR:
    /* Enable/disable SASL initial response */
    s->sasl_ir = enabled;
    अवरोध;
  हाल CURLOPT_TCP_KEEPALIVE:
    s->tcp_keepalive = enabled;
    अवरोध;
  हाल CURLOPT_TCP_FASTOPEN:
#अगर defined(CONNECT_तथ्य_IDEMPOTENT) || defined(MSG_FASTOPEN) || \
  defined(TCP_FASTOPEN_CONNECT)
    s->tcp_fastopen = enabled;
    अवरोध;
#अन्यथा
    वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
  हाल CURLOPT_SSL_ENABLE_ALPN:
    s->ssl_enable_alpn = enabled;
    अवरोध;
  हाल CURLOPT_PATH_AS_IS:
    s->path_as_is = enabled;
    अवरोध;
  हाल CURLOPT_PIPEWAIT:
    s->pipewait = enabled;
    अवरोध;
  हाल CURLOPT_SUPPRESS_CONNECT_HEADERS:
    s->suppress_connect_headers = enabled;
    अवरोध;
#अगर_अघोषित CURL_DISABLE_SHUFFLE_DNS
  हाल CURLOPT_DNS_SHUFFLE_ADDRESSES:
    s->dns_shuffle_addresses = enabled;
    अवरोध;
#पूर्ण_अगर
  हाल CURLOPT_DISALLOW_USERNAME_IN_URL:
    s->disallow_username_in_url = enabled;
    अवरोध;
  हाल CURLOPT_QUICK_EXIT:
    s->quick_exit = enabled;
    अवरोध;
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }
  अगर((arg > ok) || (arg < 0))
    /* reserve other values for future use */
    infof(तथ्य, "boolean setopt(%d) got unsupported argument %ld,"
          " treated as %d", (पूर्णांक)option, arg, enabled);

  वापस CURLE_OK;
}

अटल CURLcode मूल्य_range(दीर्घ *मूल्य, दीर्घ below_error, दीर्घ min, दीर्घ max)
{
  अगर(*मूल्य < below_error)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  अन्यथा अगर(*मूल्य < min)
    *मूल्य = min;
  अन्यथा अगर(*मूल्य > max)
    *मूल्य = max;
  वापस CURLE_OK;
}

अटल CURLcode setopt_long_net(काष्ठा Curl_easy *तथ्य, CURLoption option,
                                दीर्घ arg)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा UserDefined *s = &तथ्य->set;

  चयन(option) {
  हाल CURLOPT_DNS_CACHE_TIMEOUT:
    अगर(arg != -1)
      वापस setopt_set_timeout_sec(&s->dns_cache_timeout_ms, arg);
    s->dns_cache_timeout_ms = -1;
    अवरोध;
  हाल CURLOPT_MAXCONNECTS:
    परिणाम = मूल्य_range(&arg, 0, 0, पूर्णांक_उच्च);
    अगर(!परिणाम)
      s->maxconnects = arg ? (uint32_t)arg : DEFAULT_CONNCACHE_आकार;
    अवरोध;
  हाल CURLOPT_SERVER_RESPONSE_TIMEOUT:
    वापस setopt_set_timeout_sec(&s->server_response_timeout, arg);
  हाल CURLOPT_SERVER_RESPONSE_TIMEOUT_MS:
    वापस setopt_set_timeout_ms(&s->server_response_timeout, arg);
  हाल CURLOPT_निचला_SPEED_LIMIT:
    अगर(arg < 0)
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->निचला_speed_limit = arg;
    अवरोध;
  हाल CURLOPT_निचला_SPEED_TIME:
    परिणाम = मूल्य_range(&arg, 0, 0, अच_लघु_उच्च);
    अगर(!परिणाम)
      s->निचला_speed_time = (uint16_t)arg;
    अवरोध;
  हाल CURLOPT_PORT:
    अगर((arg < 0) || (arg > 65535))
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    s->use_port = (अचिन्हित लघु)arg;
    अवरोध;
  हाल CURLOPT_TIMEOUT:
    वापस setopt_set_timeout_sec(&s->timeout, arg);
  हाल CURLOPT_TIMEOUT_MS:
    वापस setopt_set_timeout_ms(&s->timeout, arg);
  हाल CURLOPT_CONNECTTIMEOUT:
    वापस setopt_set_timeout_sec(&s->connecttimeout, arg);
  हाल CURLOPT_CONNECTTIMEOUT_MS:
    वापस setopt_set_timeout_ms(&s->connecttimeout, arg);
#अगर_अघोषित CURL_DISABLE_BINDLOCAL
  हाल CURLOPT_LOCALPORT:
    अगर((arg < 0) || (arg > 65535))
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    s->localport = curlx_sltous(arg);
    अवरोध;
  हाल CURLOPT_LOCALPORTRANGE:
    अगर((arg < 0) || (arg > 65535))
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    s->localportrange = curlx_sltous(arg);
    अवरोध;
#पूर्ण_अगर
  हाल CURLOPT_BUFFERSIZE:
    परिणाम = मूल्य_range(&arg, 0, READBUFFER_MIN, READBUFFER_MAX);
    अगर(!परिणाम)
      s->बफर_आकार = (अचिन्हित पूर्णांक)arg;
    अवरोध;
  हाल CURLOPT_UPLOAD_BUFFERSIZE:
    परिणाम = मूल्य_range(&arg, 0, UPLOADBUFFER_MIN, UPLOADBUFFER_MAX);
    अगर(!परिणाम)
      s->upload_बफर_आकार = (अचिन्हित पूर्णांक)arg;
    अवरोध;
  हाल CURLOPT_MAXFILESIZE:
    अगर(arg < 0)
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->max_filesize = arg;
    अवरोध;
  हाल CURLOPT_IPRESOLVE:
    अगर((arg < CURL_IPRESOLVE_WHATEVER) || (arg > CURL_IPRESOLVE_V6))
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->ipver = (अचिन्हित अक्षर)arg;
    अवरोध;
  हाल CURLOPT_CONNECT_ONLY:
    अगर(arg < 0 || arg > 2)
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा {
      s->connect_only = !!arg;
      s->connect_only_ws = (arg == 2);
    }
    अवरोध;
#अगर_घोषित USE_IPV6
  हाल CURLOPT_ADDRESS_SCOPE:
#अगर SIZEOF_LONG > 4
    अगर((अचिन्हित दीर्घ)arg > अच_पूर्णांक_उच्च)
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
#पूर्ण_अगर
    s->scope_id = (अचिन्हित पूर्णांक)arg;
    अवरोध;
#पूर्ण_अगर
  हाल CURLOPT_TCP_KEEPIDLE:
    परिणाम = मूल्य_range(&arg, 0, 0, पूर्णांक_उच्च);
    अगर(!परिणाम)
      s->tcp_keepidle = (पूर्णांक)arg;
    अवरोध;
  हाल CURLOPT_TCP_KEEPINTVL:
    परिणाम = मूल्य_range(&arg, 0, 0, पूर्णांक_उच्च);
    अगर(!परिणाम)
      s->tcp_keepintvl = (पूर्णांक)arg;
    अवरोध;
  हाल CURLOPT_TCP_KEEPCNT:
    परिणाम = मूल्य_range(&arg, 0, 0, पूर्णांक_उच्च);
    अगर(!परिणाम)
      s->tcp_keepcnt = (पूर्णांक)arg;
    अवरोध;
  हाल CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
    वापस setopt_set_timeout_ms(&s->happy_eyeballs_timeout, arg);
  हाल CURLOPT_UPKEEP_INTERVAL_MS:
    वापस setopt_set_timeout_ms(&s->upkeep_interval_ms, arg);
  हाल CURLOPT_MAXAGE_CONN:
    वापस setopt_set_timeout_sec(&s->conn_max_idle_ms, arg);
  हाल CURLOPT_MAXLIFETIME_CONN:
    वापस setopt_set_timeout_sec(&s->conn_max_age_ms, arg);
  हाल CURLOPT_DNS_USE_GLOBAL_CACHE:
    /* deprecated */
    अवरोध;
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }
  वापस परिणाम;
}

अटल CURLcode setopt_long_ssl(काष्ठा Curl_easy *तथ्य, CURLoption option,
                                दीर्घ arg)
{
#अगर_घोषित USE_SSL
  CURLcode परिणाम = CURLE_OK;
  काष्ठा UserDefined *s = &तथ्य->set;
  चयन(option) {
  हाल CURLOPT_CA_CACHE_TIMEOUT:
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_CA_CACHE)) {
      परिणाम = मूल्य_range(&arg, -1, -1, पूर्णांक_उच्च);
      अगर(!परिणाम)
        s->ssl_ca_cache_timeout = (पूर्णांक)arg;
    }
    अन्यथा
      परिणाम = CURLE_NOT_BUILT_IN;
    अवरोध;
  हाल CURLOPT_SSLVERSION:
#अगर_अघोषित CURL_DISABLE_PROXY
  हाल CURLOPT_PROXY_SSLVERSION:
#पूर्ण_अगर
    वापस Curl_setopt_SSLVERSION(तथ्य, option, arg);
  हाल CURLOPT_SSL_FALSESTART:
    परिणाम = CURLE_NOT_BUILT_IN;
    अवरोध;
  हाल CURLOPT_USE_SSL:
    अगर((arg < CURLUSESSL_NONE) || (arg >= CURLUSESSL_LAST))
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->use_ssl = (अचिन्हित अक्षर)arg;
    अवरोध;
  हाल CURLOPT_SSL_OPTIONS:
    s->ssl.ssl_options = (अचिन्हित अक्षर)(arg & 0xff);
    अवरोध;
#अगर_अघोषित CURL_DISABLE_PROXY
  हाल CURLOPT_PROXY_SSL_OPTIONS:
    s->proxy_ssl.ssl_options = (अचिन्हित अक्षर)(arg & 0xff);
    अवरोध;
#पूर्ण_अगर
  हाल CURLOPT_SSL_ENABLE_NPN:
    अवरोध;
  हाल CURLOPT_SSLENGINE_DEFAULT:
#अगर_घोषित USE_OPENSSL
    अगर(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      वापस CURLE_NOT_BUILT_IN;
    तथ्य->set.engine_default = (arg == 1);
#अन्यथा
    वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
    अवरोध;
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }
  वापस परिणाम;
#अन्यथा  /* USE_SSL */
  (व्योम)तथ्य;
  (व्योम)option;
  (व्योम)arg;
  वापस CURLE_UNKNOWN_OPTION;
#पूर्ण_अगर /* !USE_SSL */
}

#अगर_अघोषित CURL_DISABLE_PROXY
अटल व्योम changeproxy(काष्ठा Curl_easy *तथ्य)
{
  Curl_auth_digest_cleanup(&तथ्य->state.proxydigest);
  स_रखो(&तथ्य->state.authproxy, 0, माप(तथ्य->state.authproxy));
}

अटल CURLcode setopt_long_proxy(काष्ठा Curl_easy *तथ्य, CURLoption option,
                                  दीर्घ arg)
{
  काष्ठा UserDefined *s = &तथ्य->set;

  चयन(option) {
  हाल CURLOPT_PROXYPORT:
    अगर((arg < 0) || (arg > UINT16_MAX))
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    अगर(arg != s->proxyport)
      changeproxy(तथ्य);
    s->proxyport = (uint16_t)arg;
    अवरोध;
  हाल CURLOPT_PROXYAUTH:
    वापस httpauth(तथ्य, TRUE, (अचिन्हित दीर्घ)arg);
  हाल CURLOPT_PROXYTYPE:
    अगर((arg < CURLPROXY_HTTP) || (arg > CURLPROXY_HTTPS3))
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
#अगर_अघोषित USE_PROXY_HTTP3
    अगर(arg == CURLPROXY_HTTPS3)
      वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
    s->proxytype = (अचिन्हित अक्षर)arg;
    अवरोध;
  हाल CURLOPT_SOCKS5_AUTH: {
    अचिन्हित दीर्घ auth = (अचिन्हित दीर्घ)arg;
    अगर(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI | CURLAUTH_ONLY))
      वापस CURLE_NOT_BUILT_IN;
    अगर((auth & CURLAUTH_ONLY) && !(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI)))
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    s->socks5auth = (अचिन्हित अक्षर)(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI));
    s->socks5_auth_only = !!(auth & CURLAUTH_ONLY);
    अवरोध;
  }
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }
  वापस CURLE_OK;
}
#अन्यथा
अटल CURLcode setopt_long_proxy(काष्ठा Curl_easy *तथ्य, CURLoption option,
                                  दीर्घ arg)
{
  (व्योम)तथ्य;
  (व्योम)option;
  (व्योम)arg;
  वापस CURLE_UNKNOWN_OPTION;
}
#पूर्ण_अगर

अटल CURLcode setopt_long_http(काष्ठा Curl_easy *तथ्य, CURLoption option,
                                 दीर्घ arg)
{
#अगर_अघोषित CURL_DISABLE_HTTP
  CURLcode परिणाम = CURLE_OK;
  काष्ठा UserDefined *s = &तथ्य->set;

  चयन(option) {
  हाल CURLOPT_FOLLOWLOCATION:
    अगर((अचिन्हित दीर्घ)arg > 3)
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->http_follow_mode = (अचिन्हित अक्षर)arg;
    अवरोध;
  हाल CURLOPT_MAXREDIRS:
    परिणाम = मूल्य_range(&arg, -1, -1, 0x7fff);
    अगर(!परिणाम)
      s->maxredirs = (लघु)arg;
    अवरोध;
  हाल CURLOPT_POSTREDIR:
    अगर(arg < CURL_REDIR_GET_ALL)
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा {
      s->post301 = !!(arg & CURL_REDIR_POST_301);
      s->post302 = !!(arg & CURL_REDIR_POST_302);
      s->post303 = !!(arg & CURL_REDIR_POST_303);
    }
    अवरोध;
  हाल CURLOPT_HEADEROPT:
    s->sep_headers = !!(arg & CURLHEADER_SEPARATE);
    अवरोध;
  हाल CURLOPT_HTTPAUTH:
    वापस httpauth(तथ्य, FALSE, (अचिन्हित दीर्घ)arg);
  हाल CURLOPT_HTTP_VERSION:
    वापस setopt_HTTP_VERSION(तथ्य, arg);
  हाल CURLOPT_EXPECT_100_TIMEOUT_MS:
    परिणाम = मूल्य_range(&arg, 0, 0, 0xffff);
    अगर(!परिणाम)
      s->expect_100_timeout = (अचिन्हित लघु)arg;
    अवरोध;
  हाल CURLOPT_STREAM_WEIGHT:
#अगर defined(USE_HTTP2) || defined(USE_HTTP3)
    अगर((arg >= 1) && (arg <= 256))
      s->weight = (पूर्णांक)arg;
    अवरोध;
#अन्यथा
    परिणाम = CURLE_NOT_BUILT_IN;
    अवरोध;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HTTPSIG
  हाल CURLOPT_HTTPSIG_ALGORITHM:
    अगर(arg != CURLHTTPSIG_NONE &&
       arg != CURLHTTPSIG_ED25519 &&
       arg != CURLHTTPSIG_HMAC_SHA256)
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    s->httpsig_algorithm = (uint8_t)arg;
    अगर(arg)
      s->httpauth = (uint32_t)CURLAUTH_HTTPSIG;
    अन्यथा
      s->httpauth &= ~(uint32_t)CURLAUTH_HTTPSIG;
    अवरोध;
#पूर्ण_अगर
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }
  वापस परिणाम;
#अन्यथा
  (व्योम)तथ्य;
  (व्योम)option;
  (व्योम)arg;
  वापस CURLE_UNKNOWN_OPTION;
#पूर्ण_अगर
}

अटल CURLcode setopt_long_proto(काष्ठा Curl_easy *तथ्य, CURLoption option,
                                  दीर्घ arg)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा UserDefined *s = &तथ्य->set;

  चयन(option) {
#अगर_अघोषित CURL_DISABLE_TFTP
  हाल CURLOPT_TFTP_BLKSIZE:
    परिणाम = मूल्य_range(&arg, 0, TFTP_BLKSIZE_MIN, TFTP_BLKSIZE_MAX);
    अगर(!परिणाम)
      s->tftp_blksize = (अचिन्हित लघु)arg;
    अवरोध;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_NETRC
  हाल CURLOPT_NETRC:
    अगर((arg < CURL_NETRC_IGNORED) || (arg >= CURL_NETRC_LAST))
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->use_netrc = (अचिन्हित अक्षर)arg;
    अवरोध;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_FTP
  हाल CURLOPT_FTP_FILEMETHOD:
    अगर((arg < CURLFTPMETHOD_DEFAULT) || (arg >= CURLFTPMETHOD_LAST))
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->ftp_filemethod = (अचिन्हित अक्षर)arg;
    अवरोध;
  हाल CURLOPT_FTP_SSL_CCC:
    अगर((arg < CURLFTPSSL_CCC_NONE) || (arg >= CURLFTPSSL_CCC_LAST))
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->ftp_ccc = (अचिन्हित अक्षर)arg;
    अवरोध;
  हाल CURLOPT_FTPSSLAUTH:
    अगर((arg < CURLFTPAUTH_DEFAULT) || (arg >= CURLFTPAUTH_LAST))
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->ftpsslauth = (अचिन्हित अक्षर)arg;
    अवरोध;
  हाल CURLOPT_ACCEPTTIMEOUT_MS:
    वापस setopt_set_timeout_ms(&s->accepttimeout, arg);
#पूर्ण_अगर
#अगर !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  हाल CURLOPT_FTP_CREATE_MISSING_DIRS:
    अगर((arg < CURLFTP_CREATE_DIR_NONE) || (arg > CURLFTP_CREATE_DIR_RETRY))
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->ftp_create_missing_dirs = (अचिन्हित अक्षर)arg;
    अवरोध;
  हाल CURLOPT_NEW_FILE_PERMS:
    अगर((arg < 0) || (arg > 0777))
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->new_file_perms = (अचिन्हित पूर्णांक)arg;
    अवरोध;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_RTSP
  हाल CURLOPT_RTSP_REQUEST:
    वापस setopt_RTSP_REQUEST(तथ्य, arg);
  हाल CURLOPT_RTSP_CLIENT_CSEQ:
    परिणाम = मूल्य_range(&arg, 0, 0, पूर्णांक_उच्च);
    अगर(!परिणाम)
      तथ्य->state.rtsp_अग्र_client_CSeq = (uint32_t)arg;
    अवरोध;
  हाल CURLOPT_RTSP_SERVER_CSEQ:
    परिणाम = मूल्य_range(&arg, 0, 0, पूर्णांक_उच्च);
    अगर(!परिणाम)
      तथ्य->state.rtsp_अग्र_server_CSeq = (uint32_t)arg;
    अवरोध;
#पूर्ण_अगर
#अगर_घोषित USE_SSH
  हाल CURLOPT_SSH_AUTH_TYPES:
    s->ssh_auth_types = (uint32_t)arg;
    अवरोध;
  हाल CURLOPT_NEW_DIRECTORY_PERMS:
    अगर((arg < 0) || (arg > 0777))
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      s->new_directory_perms = (अचिन्हित पूर्णांक)arg;
    अवरोध;
#पूर्ण_अगर
  हाल CURLOPT_PROTOCOLS:
    s->allowed_protocols = (curl_prot_t)arg;
    अवरोध;
  हाल CURLOPT_REDIR_PROTOCOLS:
    s->redir_protocols = (curl_prot_t)arg;
    अवरोध;
#अगर_अघोषित CURL_DISABLE_WEBSOCKETS
  हाल CURLOPT_WS_OPTIONS:
    s->ws_raw_mode = !!(arg & CURLWS_RAW_MODE);
    s->ws_no_auto_pong = !!(arg & CURLWS_NOAUTOPONG);
    अवरोध;
#पूर्ण_अगर
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }
  वापस परिणाम;
}

अटल CURLcode setopt_long_misc(काष्ठा Curl_easy *तथ्य, CURLoption option,
                                 दीर्घ arg)
{
  काष्ठा UserDefined *s = &तथ्य->set;

  चयन(option) {
  हाल CURLOPT_TIMECONDITION:
    अगर((arg < CURL_TIMECOND_NONE) || (arg >= CURL_TIMECOND_LAST))
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    s->timecondition = (अचिन्हित अक्षर)arg;
    अवरोध;
  हाल CURLOPT_TIMEVALUE:
    s->timevalue = (समय_प्रकार)arg;
    अवरोध;
  हाल CURLOPT_POSTFIELDSIZE:
    अगर(arg < -1)
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    अगर(s->postfieldsize < arg && s->str_copypostfields) {
      curlx_safefree(s->str_copypostfields);
      s->postfields = शून्य;
    }
    s->postfieldsize = arg;
    अवरोध;
  हाल CURLOPT_INFILESIZE:
    अगर(arg < -1)
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = arg;
    अवरोध;
  हाल CURLOPT_RESUME_FROM:
    अगर(arg < -1)
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = arg;
    अवरोध;
  हाल CURLOPT_UPLOAD_FLAGS:
    s->upload_flags = (अचिन्हित अक्षर)arg;
    अवरोध;
#अगर_अघोषित CURL_DISABLE_MIME
  हाल CURLOPT_MIME_OPTIONS:
    s->mime_formescape = !!(arg & CURLMIMEOPT_FORMESCAPE);
    अवरोध;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HSTS
  हाल CURLOPT_HSTS_CTRL:
    अगर(arg & CURLHSTS_ENABLE) {
      अगर(!तथ्य->hsts) {
        तथ्य->hsts = Curl_hsts_init();
        अगर(!तथ्य->hsts)
          वापस CURLE_OUT_OF_MEMORY;
      }
    }
    अन्यथा अगर(!तथ्य->share || !तथ्य->share->hsts) {
      /* throw away the HSTS cache unless shared */
      Curl_hsts_cleanup(&तथ्य->hsts);
      /* flush all the entries */
      curl_slist_free_all(तथ्य->state.hstslist);
      तथ्य->state.hstslist = शून्य;
    }
    अन्यथा
      /* detach from shared HSTS cache without freeing it */
      तथ्य->hsts = शून्य;
    अवरोध;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_ALTSVC
  हाल CURLOPT_ALTSVC_CTRL:
    वापस Curl_altsvc_ctrl(तथ्य, arg);
#पूर्ण_अगर
#अगर defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  हाल CURLOPT_GSSAPI_DELEGATION:
    s->gssapi_delegation = (अचिन्हित अक्षर)arg &
      (CURLGSSAPI_DELEGATION_POLICY_FLAG | CURLGSSAPI_DELEGATION_FLAG);
    अवरोध;
#पूर्ण_अगर
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }
  वापस CURLE_OK;
}

अटल CURLcode setopt_long(काष्ठा Curl_easy *तथ्य, CURLoption option,
                            दीर्घ arg)
{
  प्रकार CURLcode (*setoptfunc)(काष्ठा Curl_easy *तथ्य,
                                 CURLoption option, दीर्घ arg);
  अटल स्थिर setoptfunc setopt_call[] = {
    setopt_long_bool,
    setopt_long_net,
    setopt_long_http,
    setopt_long_proxy,
    setopt_long_ssl,
    setopt_long_proto,
    setopt_long_misc
  };
  माप_प्रकार i;

  क्रम(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode परिणाम = setopt_call[i](तथ्य, option, arg);
    अगर(परिणाम != CURLE_UNKNOWN_OPTION)
      वापस परिणाम;
  }
  वापस CURLE_UNKNOWN_OPTION;
}

अटल CURLcode setopt_slist(काष्ठा Curl_easy *तथ्य, CURLoption option,
                             काष्ठा curl_slist *slist)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा UserDefined *s = &तथ्य->set;
  चयन(option) {
#अगर_अघोषित CURL_DISABLE_PROXY
  हाल CURLOPT_PROXYHEADER:
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
    अवरोध;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HTTP
  हाल CURLOPT_HTTP200ALIASES:
    /*
     * Set a list of aliases for HTTP 200 in response header
     */
    s->http200aliases = slist;
    अवरोध;
#पूर्ण_अगर
#अगर !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  हाल CURLOPT_POSTQUOTE:
    /*
     * List of RAW FTP commands to use after a transfer
     */
    s->postquote = slist;
    अवरोध;
  हाल CURLOPT_PREQUOTE:
    /*
     * List of RAW FTP commands to use prior to RETR (Wesley Laxton)
     */
    s->prequote = slist;
    अवरोध;
  हाल CURLOPT_QUOTE:
    /*
     * List of RAW FTP commands to use before a transfer
     */
    s->quote = slist;
    अवरोध;
#पूर्ण_अगर
  हाल CURLOPT_RESOLVE:
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
    तथ्य->state.resolve = s->resolve;
    अवरोध;
#अगर !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MIME)
  हाल CURLOPT_HTTPHEADER:
    /*
     * Set a list with HTTP headers to use (or replace internals with)
     */
    s->headers = slist;
    अवरोध;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_TELNET
  हाल CURLOPT_TELNETOPTIONS:
    /*
     * Set a linked list of telnet options
     */
    s->telnet_options = slist;
    अवरोध;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_SMTP
  हाल CURLOPT_MAIL_RCPT:
    /* Set the list of mail recipients */
    s->mail_rcpt = slist;
    अवरोध;
#पूर्ण_अगर
  हाल CURLOPT_CONNECT_TO:
    s->connect_to = slist;
    अवरोध;
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }
  वापस परिणाम;
}

#अगर !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#अगर_अघोषित CURL_DISABLE_MIME
अटल CURLcode setopt_mimepost(काष्ठा Curl_easy *तथ्य, curl_mime *mimep)
{
  /*
   * Set to make us do MIME POST
   */
  CURLcode परिणाम;
  काष्ठा UserDefined *s = &तथ्य->set;
  अगर(!s->mimepostp) {
    s->mimepostp = curlx_malloc(माप(*s->mimepostp));
    अगर(!s->mimepostp)
      वापस CURLE_OUT_OF_MEMORY;
    Curl_mime_initpart(s->mimepostp);
  }

  परिणाम = Curl_mime_set_subparts(s->mimepostp, mimep, FALSE);
  अगर(!परिणाम) {
    s->method = HTTPREQ_POST_MIME;
    s->opt_no_body = FALSE; /* this is implied */
#अगर_अघोषित CURL_DISABLE_FORM_API
    Curl_mime_cleanpart(तथ्य->state.formp);
    curlx_safefree(तथ्य->state.formp);
    तथ्य->state.mimepost = शून्य;
#पूर्ण_अगर
  }
  वापस परिणाम;
}
#पूर्ण_अगर /* !CURL_DISABLE_MIME */
#पूर्ण_अगर /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */

अटल CURLcode setopt_share(काष्ठा Curl_easy *तथ्य, काष्ठा Curl_share *set)
{
  CURLcode परिणाम;

  अगर(तथ्य->conn) {
    /* As this handle already has a connection attached, changing share now
       would be complicated and error-prone */
    infof(तथ्य, "Cannot change share object while in use");
    परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
  }
  अन्यथा {
    /* disconnect from old share, if any and possible */
    परिणाम = Curl_share_easy_unlink(तथ्य);
    अगर(!परिणाम && GOOD_SHARE_HANDLE(set))
      /* use new share if it set */
      परिणाम = Curl_share_easy_link(तथ्य, set);
  }
  वापस परिणाम;
}

/* assorted pointer type arguments */
अटल CURLcode setopt_pointers(काष्ठा Curl_easy *तथ्य, CURLoption option,
                                बहु_सूची param)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा UserDefined *s = &तथ्य->set;
  चयन(option) {
  हाल CURLOPT_CURLU:
    /*
     * pass CURLU to set URL
     */
    Curl_bufref_free(&तथ्य->state.url);
    CURL_EASY_STR_CLEAR(तथ्य, STRING_SET_URL);
    s->uh = बहु_तर्क(param, CURLU *);
    अवरोध;
#अगर_अघोषित CURL_DISABLE_HTTP
#अगर_अघोषित CURL_DISABLE_FORM_API
  हाल CURLOPT_HTTPPOST:
    /*
     * Set to make us do HTTP POST. Legacy API-style.
     */
    s->httppost = बहु_तर्क(param, काष्ठा curl_httppost *);
    s->method = HTTPREQ_POST_FORM;
    s->opt_no_body = FALSE; /* this is implied */
    Curl_mime_cleanpart(तथ्य->state.formp);
    curlx_safefree(तथ्य->state.formp);
    तथ्य->state.mimepost = शून्य;
    अवरोध;
#पूर्ण_अगर /* !CURL_DISABLE_FORM_API */
#पूर्ण_अगर /* !CURL_DISABLE_HTTP */
#अगर !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#अगर_अघोषित CURL_DISABLE_MIME
  हाल CURLOPT_MIMEPOST:
    परिणाम = setopt_mimepost(तथ्य, बहु_तर्क(param, curl_mime *));
    अवरोध;
#पूर्ण_अगर /* !CURL_DISABLE_MIME */
#पूर्ण_अगर /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */
  हाल CURLOPT_STDERR:
    /*
     * Set to a FILE * that should receive all error writes. This
     * defaults to stderr for normal operations.
     */
    s->err = बहु_तर्क(param, खाता *);
    अगर(!s->err)
      s->err = मानक_त्रुटि;
    अवरोध;
  हाल CURLOPT_SHARE:
    वापस setopt_share(तथ्य, बहु_तर्क(param, काष्ठा Curl_share *));

  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }
  वापस परिणाम;
}

#अगर_अघोषित CURL_DISABLE_COOKIES
अटल CURLcode cookielist(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *ptr)
{
  CURLcode परिणाम = CURLE_OK;
  अगर(!ptr)
    वापस CURLE_OK;

  अगर(curl_strequal(ptr, "ALL")) {
    /* clear all cookies */
    Curl_share_lock(तथ्य, CURL_LOCK_तथ्य_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearall(तथ्य->cookies);
    Curl_share_unlock(तथ्य, CURL_LOCK_तथ्य_COOKIE);
  }
  अन्यथा अगर(curl_strequal(ptr, "SESS")) {
    /* clear session cookies */
    Curl_share_lock(तथ्य, CURL_LOCK_तथ्य_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearsess(तथ्य->cookies);
    Curl_share_unlock(तथ्य, CURL_LOCK_तथ्य_COOKIE);
  }
  अन्यथा अगर(curl_strequal(ptr, "FLUSH")) {
    /* flush cookies to file, takes care of the locking */
    Curl_flush_cookies(तथ्य, FALSE);
  }
  अन्यथा अगर(curl_strequal(ptr, "RELOAD")) {
    /* reload cookies from file */
    वापस Curl_cookie_loadfiles(तथ्य, COOKIE_NOPSL |
                                 (तथ्य->set.cookiesession ?
                                  COOKIE_NOSESSION : 0));
  }
  अन्यथा {
    अगर(!तथ्य->cookies) {
      /* if cookie engine was not running, activate it */
      तथ्य->cookies = Curl_cookie_init();
      अगर(!तथ्य->cookies)
        वापस CURLE_OUT_OF_MEMORY;
      तथ्य->state.cookie_engine = TRUE;
    }

    /* general protection against mistakes and abuse */
    अगर(म_माप(ptr) > CURL_MAX_निवेश_लंबाई)
      वापस CURLE_BAD_FUNCTION_ARGUMENT;

    /* Adding these cookies without the PSL check, because the PSL is not
       initialized until *perform() time, and this might be called before
       that */
    Curl_share_lock(तथ्य, CURL_LOCK_तथ्य_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    अगर(checkprefix("Set-Cookie:", ptr))
      /* HTTP Header format line */
      परिणाम = Curl_cookie_add(तथ्य, तथ्य->cookies, ptr + 11,
                               शून्य, शून्य,
                               COOKIE_HTTPHEADER | COOKIE_SECURE |
                               COOKIE_NOPSL);
    अन्यथा
      /* Netscape format line */
      परिणाम = Curl_cookie_add(तथ्य, तथ्य->cookies, ptr, शून्य,
                               शून्य, COOKIE_SECURE | COOKIE_NOPSL);
    Curl_share_unlock(तथ्य, CURL_LOCK_तथ्य_COOKIE);
  }
  वापस परिणाम;
}

अटल CURLcode cookiefile(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *ptr)
{
  /*
   * Set cookie file to read and parse. Can be used multiple times.
   */
  अगर(ptr) {
    काष्ठा curl_slist *cl;
    /* general protection against mistakes and abuse */
    अगर(म_माप(ptr) > CURL_MAX_निवेश_लंबाई)
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    /* append the cookie filename to the list of filenames, and deal with
       them later */
    cl = curl_slist_append(तथ्य->state.cookielist, ptr);
    अगर(!cl) {
      curl_slist_free_all(तथ्य->state.cookielist);
      तथ्य->state.cookielist = शून्य;
      वापस CURLE_OUT_OF_MEMORY;
    }
    तथ्य->state.cookielist = cl; /* store the list for later use */
  }
  अन्यथा {
    /* clear the list of cookie files */
    curl_slist_free_all(तथ्य->state.cookielist);
    तथ्य->state.cookielist = शून्य;

    अगर(!तथ्य->share || !तथ्य->share->cookies) {
      /* throw away all existing cookies if this is not a shared cookie
         container */
      Curl_cookie_clearall(तथ्य->cookies);
      Curl_cookie_cleanup(तथ्य->cookies);
    }
    /* disable the cookie engine */
    तथ्य->cookies = शून्य;
  }
  वापस CURLE_OK;
}
#पूर्ण_अगर

#अगर_अघोषित CURL_DISABLE_PROXY
अटल CURLcode setproxy(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *proxy)
{
  स्थिर अक्षर *str = CURL_EASY_STR(तथ्य, STRING_PROXY);
  अगर(str && proxy &&
     /* there was one set, is this a new one? */
     !म_भेद(str, proxy))
    वापस CURLE_OK; /* same one as before */

  changeproxy(तथ्य);
  वापस Curl_setstropt(तथ्य, STRING_PROXY, proxy);
}

अटल CURLcode setopt_cptr_proxy(काष्ठा Curl_easy *तथ्य, CURLoption option,
                                  अक्षर *ptr)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा UserDefined *s = &तथ्य->set;
  चयन(option) {
  हाल CURLOPT_PROXYUSERPWD: {
    /*
     * user:password needed to use the proxy
     */
    अक्षर *u = शून्य;
    अक्षर *p = शून्य;
    परिणाम = setstropt_userpwd(ptr, &u, &p);

    /* URL decode the components */
    अगर(!परिणाम) {
      अक्षर *str = शून्य;
      CURL_EASY_STR_CLEAR(तथ्य, STRING_PROXYUSERNAME);
      CURL_EASY_STR_CLEAR(तथ्य, STRING_PROXYPASSWORD);
      अगर(u) {
        परिणाम = Curl_urldecode(u, 0, &str, शून्य, REJECT_ZERO);
        अगर(!परिणाम)
          परिणाम = Curl_u8_strset_setn(&s->strings, STRING_PROXYUSERNAME, str);
      }
      अगर(!परिणाम && p) {
        str = शून्य;
        परिणाम = Curl_urldecode(p, 0, &str, शून्य, REJECT_ZERO);
        अगर(!परिणाम)
          परिणाम = Curl_u8_strset_setn(&s->strings, STRING_PROXYPASSWORD, str);
      }
    }
    curlx_free(u);
    curlx_strzero(p);
    curlx_free(p);
    अवरोध;
  }
  हाल CURLOPT_PROXYUSERNAME:
    /*
     * authentication username to use in the operation
     */
    वापस Curl_setstropt(तथ्य, STRING_PROXYUSERNAME, ptr);

  हाल CURLOPT_PROXYPASSWORD:
    /*
     * authentication password to use in the operation
     */
    वापस Curl_setstropt(तथ्य, STRING_PROXYPASSWORD, ptr);

  हाल CURLOPT_NOPROXY:
    /*
     * proxy exception list
     */
    वापस Curl_setstropt(तथ्य, STRING_NOPROXY, ptr);
  हाल CURLOPT_PROXY_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use for proxy
     */
    वापस Curl_setstropt(तथ्य, STRING_CERT_PROXY, ptr);
  हाल CURLOPT_PROXY_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use for proxy
     */
    वापस Curl_setstropt(तथ्य, STRING_CERT_TYPE_PROXY, ptr);
  हाल CURLOPT_PROXY_SSLKEY:
    /*
     * String that holds filename of the SSL key to use for proxy
     */
    वापस Curl_setstropt(तथ्य, STRING_कुंजी_PROXY, ptr);
  हाल CURLOPT_PROXY_KEYPASSWD:
    /*
     * String that holds the SSL private key password for proxy.
     */
    वापस Curl_setstropt(तथ्य, STRING_कुंजी_PASSWD_PROXY, ptr);
  हाल CURLOPT_PROXY_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use for proxy
     */
    वापस Curl_setstropt(तथ्य, STRING_कुंजी_TYPE_PROXY, ptr);
  हाल CURLOPT_PROXY_SSL_CIPHER_LIST:
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_CIPHER_LIST)) {
      /* set a list of cipher we want to use in the SSL connection for proxy */
      वापस Curl_setstropt(तथ्य, STRING_SSL_CIPHER_LIST_PROXY, ptr);
    }
    अन्यथा
      वापस CURLE_NOT_BUILT_IN;
  हाल CURLOPT_PROXY_TLS13_CIPHERS:
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites for proxy */
      वापस Curl_setstropt(तथ्य, STRING_SSL_CIPHER13_LIST_PROXY, ptr);
    अन्यथा
      वापस CURLE_NOT_BUILT_IN;
  हाल CURLOPT_PROXY:
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
    वापस setproxy(तथ्य, ptr);
  हाल CURLOPT_PRE_PROXY:
    /*
     * Set proxy server:port to use as SOCKS proxy.
     *
     * If the proxy is set to "" or NULL we explicitly say that we do not want
     * to use the socks proxy.
     */
    वापस Curl_setstropt(तथ्य, STRING_PRE_PROXY, ptr);
  हाल CURLOPT_SOCKS5_GSSAPI_SERVICE:
  हाल CURLOPT_PROXY_SERVICE_NAME:
    /*
     * Set proxy authentication service name for Kerberos 5 and SPNEGO
     */
    वापस Curl_setstropt(तथ्य, STRING_PROXY_SERVICE_NAME, ptr);
  हाल CURLOPT_PROXY_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
#अगर_घोषित USE_SSL
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_PINNEDPUBKEY))
      वापस Curl_setstropt(तथ्य, STRING_SSL_PINNEDPUBLICKEY_PROXY, ptr);
#पूर्ण_अगर
    वापस CURLE_NOT_BUILT_IN;

  हाल CURLOPT_HAPROXY_CLIENT_IP:
    /*
     * Set the client IP to send through HAProxy PROXY protocol
     */
    परिणाम = Curl_setstropt(तथ्य, STRING_HAPROXY_CLIENT_IP, ptr);

    /* enable the HAProxy protocol if an IP is provided */
    s->haproxyprotocol = !!CURL_EASY_STR(तथ्य, STRING_HAPROXY_CLIENT_IP);
    अवरोध;
  हाल CURLOPT_PROXY_CAINFO:
    /*
     * Set CA info SSL connection for proxy. Specify filename of the
     * CA certificate
     */
    परिणाम = Curl_setstropt(तथ्य, STRING_SSL_CAFILE_PROXY, ptr);
    अगर(!परिणाम)
      s->proxy_ssl.custom_cafile = TRUE;
    वापस परिणाम;
  हाल CURLOPT_PROXY_CRLFILE:
    /*
     * Set CRL file info for SSL connection for proxy. Specify filename of the
     * CRL to check certificates revocation
     */
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_CRLFILE))
      वापस Curl_setstropt(तथ्य, STRING_SSL_CRLFILE_PROXY, ptr);
    वापस CURLE_NOT_BUILT_IN;
  हाल CURLOPT_PROXY_ISSUERCERT:
    /*
     * Set Issuer certificate file to check certificates issuer
     */
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_ISSUERCERT))
      वापस Curl_setstropt(तथ्य, STRING_SSL_ISSUERCERT_PROXY, ptr);
    वापस CURLE_NOT_BUILT_IN;
  हाल CURLOPT_PROXY_CAPATH:
    /*
     * Set CA path info for SSL connection proxy. Specify directory name of the
     * CA certificates which have been prepared using openssl c_rehash utility.
     */
#अगर_घोषित USE_SSL
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      परिणाम = Curl_setstropt(तथ्य, STRING_SSL_CAPATH_PROXY, ptr);
      अगर(!परिणाम)
        s->proxy_ssl.custom_capath = TRUE;
      वापस परिणाम;
    }
#पूर्ण_अगर
    वापस CURLE_NOT_BUILT_IN;
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }
  वापस परिणाम;
}
#पूर्ण_अगर

#अगर !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
/*
 * A string with POST data. Makes curl HTTP POST. Even if it is NULL. If
 * needed, CURLOPT_POSTFIELDSIZE must have been set prior to
 * CURLOPT_COPYPOSTFIELDS and not altered later.
 */
अटल CURLcode setopt_copypostfields(स्थिर अक्षर *ptr, काष्ठा UserDefined *s)
{
  CURLcode परिणाम = CURLE_OK;
  अगर(s->postfieldsize < -1)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  अगर(!ptr || s->postfieldsize == -1) {
    अगर(ptr && (म_माप(ptr) > CURL_MAX_निवेश_लंबाई))
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_safefree(s->str_copypostfields);
    अगर(ptr) {
      s->str_copypostfields = curlx_strdup(ptr);
      अगर(!s->str_copypostfields)
        वापस CURLE_OUT_OF_MEMORY;
    }
  }
  अन्यथा {
    माप_प्रकार pflen = curlx_sotouz_range(s->postfieldsize, 0, SIZE_MAX);
    अगर(pflen == SIZE_MAX)
      वापस CURLE_OUT_OF_MEMORY;
    अन्यथा {
      /* Allocate even when size == 0. This satisfies the need of possible
         later address compare to detect the COPYPOSTFIELDS mode, and to mark
         that postfields is used rather than read function or form data. */
      अक्षर *p = curlx_memdup0(ptr, pflen);
      अगर(!p)
        वापस CURLE_OUT_OF_MEMORY;
      अन्यथा {
        curlx_free(s->str_copypostfields);
        s->str_copypostfields = p;
      }
    }
  }

  s->postfields = s->str_copypostfields;
  s->method = HTTPREQ_POST;
  वापस परिणाम;
}
#पूर्ण_अगर

#अगर_घोषित USE_ECH
अटल CURLcode setopt_ech(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *ptr)
{
  काष्ठा UserDefined *s = &तथ्य->set;
  CURLcode परिणाम = CURLE_OK;

  अगर(!ptr || !म_भेद(ptr, "false"))
    s->ssl.ech = CURLECH_DISABLE;
  अन्यथा {
    माप_प्रकार plen = म_माप(ptr);
    अगर(plen > CURL_MAX_निवेश_लंबाई)
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा {
      अगर(!म_भेद(ptr, "grease"))
        s->ssl.ech = CURLECH_GREASE;
      अन्यथा अगर(!म_भेद(ptr, "true"))
        s->ssl.ech = CURLECH_ENABLE;
      अन्यथा अगर(!म_भेद(ptr, "hard"))
        s->ssl.ech = CURLECH_HARD;
      अन्यथा अगर(plen > 4 && !म_भेदन(ptr, "ecl:", 4)) {
        अगर(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        परिणाम = Curl_setstropt(तथ्य, STRING_ECH_CONFIG, ptr + 4);
      }
      अन्यथा अगर(plen > 3 && !म_भेदन(ptr, "pn:", 3)) {
        अगर(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        परिणाम = Curl_setstropt(तथ्य, STRING_ECH_PUBLIC, ptr + 3);
      }
      अन्यथा
        परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  वापस परिणाम;
}
#अन्यथा
#घोषणा setopt_ech(x, y) CURLE_NOT_BUILT_IN
#पूर्ण_अगर

#अगर defined(USE_SSL) || defined(USE_SSH)
/* One of the options is used for both TLS and SSH */
अटल CURLcode setopt_cptr_ssl(काष्ठा Curl_easy *तथ्य, CURLoption option,
                                अक्षर *ptr)
{
  CURLcode परिणाम = CURLE_OK;

  चयन(option) {
  हाल CURLOPT_KEYPASSWD:
    /*
     * String that holds the SSL or SSH private key password.
     */
    परिणाम = Curl_setstropt(तथ्य, STRING_कुंजी_PASSWD, ptr);
    अवरोध;
#अगर_घोषित USE_SSL
  हाल CURLOPT_CAINFO:
    /*
     * Set CA info for SSL connection. Specify filename of the CA certificate
     */
    परिणाम = Curl_setstropt(तथ्य, STRING_SSL_CAFILE, ptr);
    अगर(!परिणाम)
      तथ्य->set.ssl.custom_cafile = TRUE;
    वापस परिणाम;
  हाल CURLOPT_CAPATH:
    /*
     * Set CA path info for SSL connection. Specify directory name of the CA
     * certificates which have been prepared using openssl c_rehash utility.
     */
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      परिणाम = Curl_setstropt(तथ्य, STRING_SSL_CAPATH, ptr);
      अगर(!परिणाम)
        तथ्य->set.ssl.custom_capath = TRUE;
      वापस परिणाम;
    }
    वापस CURLE_NOT_BUILT_IN;
  हाल CURLOPT_CRLFILE:
    /*
     * Set CRL file info for SSL connection. Specify filename of the CRL
     * to check certificates revocation
     */
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_CRLFILE))
      वापस Curl_setstropt(तथ्य, STRING_SSL_CRLFILE, ptr);
    वापस CURLE_NOT_BUILT_IN;
  हाल CURLOPT_SSL_CIPHER_LIST:
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_CIPHER_LIST))
      /* set a list of cipher we want to use in the SSL connection */
      वापस Curl_setstropt(तथ्य, STRING_SSL_CIPHER_LIST, ptr);
    अन्यथा
      वापस CURLE_NOT_BUILT_IN;
  हाल CURLOPT_TLS13_CIPHERS:
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites */
      वापस Curl_setstropt(तथ्य, STRING_SSL_CIPHER13_LIST, ptr);
    अन्यथा
      वापस CURLE_NOT_BUILT_IN;
  हाल CURLOPT_RANDOM_FILE:
    अवरोध;
  हाल CURLOPT_EGDSOCKET:
    अवरोध;
  हाल CURLOPT_SSL_CTX_तथ्य:
    /*
     * Set an SSL_CTX callback parameter pointer
     */
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_SSL_CTX)) {
      तथ्य->set.ssl_fsslctxp = ptr;
      अवरोध;
    }
    अन्यथा
      वापस CURLE_NOT_BUILT_IN;
  हाल CURLOPT_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use
     */
    वापस Curl_setstropt(तथ्य, STRING_CERT, ptr);
  हाल CURLOPT_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use
     */
    वापस Curl_setstropt(तथ्य, STRING_CERT_TYPE, ptr);
  हाल CURLOPT_SSLKEY:
    /*
     * String that holds filename of the SSL key to use
     */
    वापस Curl_setstropt(तथ्य, STRING_कुंजी, ptr);
  हाल CURLOPT_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use
     */
    वापस Curl_setstropt(तथ्य, STRING_कुंजी_TYPE, ptr);
  हाल CURLOPT_SSLENGINE:
    /*
     * String that holds the SSL crypto engine.
     */
    अगर(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      वापस CURLE_NOT_BUILT_IN;
    परिणाम = Curl_setstropt(तथ्य, STRING_SSL_ENGINE, ptr);
    अवरोध;
  हाल CURLOPT_ISSUERCERT:
    /*
     * Set Issuer certificate file
     * to check certificates issuer
     */
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_ISSUERCERT))
      वापस Curl_setstropt(तथ्य, STRING_SSL_ISSUERCERT, ptr);
    वापस CURLE_NOT_BUILT_IN;
  हाल CURLOPT_SSL_EC_CURVES:
    /*
     * Set accepted curves in SSL connection setup.
     * Specify colon-delimited list of curve algorithm names.
     */
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_SSL_EC_CURVES))
      वापस Curl_setstropt(तथ्य, STRING_SSL_EC_CURVES, ptr);
    वापस CURLE_NOT_BUILT_IN;
  हाल CURLOPT_SSL_SIGNATURE_ALGORITHMS:
    /*
     * Set accepted signature algorithms.
     * Specify colon-delimited list of signature scheme names.
     */
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_SIGNATURE_ALGORITHMS))
      वापस Curl_setstropt(तथ्य, STRING_SSL_SIGNATURE_ALGORITHMS, ptr);
    वापस CURLE_NOT_BUILT_IN;
  हाल CURLOPT_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_PINNEDPUBKEY))
      वापस Curl_setstropt(तथ्य, STRING_SSL_PINNEDPUBLICKEY, ptr);
    वापस CURLE_NOT_BUILT_IN;
  हाल CURLOPT_ECH:
    वापस setopt_ech(तथ्य, ptr);
#पूर्ण_अगर
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }
  वापस परिणाम;
}
#पूर्ण_अगर

#अगर !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
अटल CURLcode setopt_cptr_http_mqtt(काष्ठा Curl_easy *तथ्य,
                                      CURLoption option, अक्षर *ptr)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा UserDefined *s = &तथ्य->set;

  चयन(option) {
  हाल CURLOPT_COPYPOSTFIELDS:
    वापस setopt_copypostfields(ptr, s);

  हाल CURLOPT_POSTFIELDS:
    /*
     * Like above, but use static data instead of copying it.
     */
    s->postfields = ptr;
    /* Release old copied data. */
    curlx_safefree(s->str_copypostfields);
    s->method = HTTPREQ_POST;
    अवरोध;

#अगर_अघोषित CURL_DISABLE_HTTP
  हाल CURLOPT_TRAILERDATA:
    s->trailer_तथ्य = ptr;
    अवरोध;
  हाल CURLOPT_ACCEPT_ENCODING:
    /*
     * String to use at the value of Accept-Encoding header.
     *
     * If the encoding is set to "" we use an Accept-Encoding header that
     * encompasses all the encodings we support.
     * If the encoding is set to NULL we do not send an Accept-Encoding header
     * and ignore any received Content-Encoding header.
     *
     */
    अगर(ptr && !*ptr) {
      ptr = Curl_get_content_encodings();
      अगर(ptr)
        परिणाम = CURL_EASY_STR_SETN(तथ्य, STRING_ENCODING, ptr);
      अन्यथा
        परिणाम = CURLE_OUT_OF_MEMORY;
      वापस परिणाम;
    }
    वापस Curl_setstropt(तथ्य, STRING_ENCODING, ptr);

#अगर_अघोषित CURL_DISABLE_AWS
  हाल CURLOPT_AWS_SIGV4:
    /*
     * String that is merged to some authentication
     * parameters are used by the algorithm.
     */
    परिणाम = Curl_setstropt(तथ्य, STRING_AWS_SIGV4, ptr);
    /*
     * Basic has been set by default; it needs to be unset here.
     */
    अगर(CURL_EASY_STR(तथ्य, STRING_AWS_SIGV4))
      s->httpauth = CURLAUTH_AWS_SIGV4;
    अन्यथा
      s->httpauth &= ~(uint32_t)CURLAUTH_AWS_SIGV4;
    अवरोध;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HTTPSIG
  हाल CURLOPT_HTTPSIG_कुंजी:
    परिणाम = Curl_setstropt(तथ्य, STRING_HTTPSIG_कुंजी, ptr);
    अवरोध;
  हाल CURLOPT_HTTPSIG_KEYID:
    परिणाम = Curl_setstropt(तथ्य, STRING_HTTPSIG_KEYID, ptr);
    अवरोध;
  हाल CURLOPT_HTTPSIG_HEADERS:
    परिणाम = Curl_setstropt(तथ्य, STRING_HTTPSIG_HEADERS, ptr);
    अवरोध;
#पूर्ण_अगर
  हाल CURLOPT_REFERER: {
    /*
     * String to set in the HTTP Referer: field.
     */
    काष्ठा bufref *oldref = &तथ्य->state.referer;
    /* free the old after the storing the new in case the input is actually
       pointing back to this */
    परिणाम = Curl_setstropt(तथ्य, STRING_SET_REFERER, ptr);
    Curl_bufref_free(oldref);
    अवरोध;
  }

  हाल CURLOPT_USERAGENT:
    /*
     * String to use in the HTTP User-Agent field
     */
    वापस Curl_setstropt(तथ्य, STRING_USERAGENT, ptr);

#अगर_अघोषित CURL_DISABLE_COOKIES
  हाल CURLOPT_COOKIE:
    /*
     * Cookie string to send to the remote server in the request.
     */
    वापस Curl_setstropt(तथ्य, STRING_COOKIE, ptr);

  हाल CURLOPT_COOKIEFILE:
    वापस cookiefile(तथ्य, ptr);

  हाल CURLOPT_COOKIEJAR:
    /*
     * Set cookie filename to dump all cookies to when we are done.
     */
    परिणाम = Curl_setstropt(तथ्य, STRING_COOKIEJAR, ptr);
    अगर(!परिणाम) {
      /*
       * Activate the cookie parser. This may or may not already
       * have been made.
       */
      अगर(!तथ्य->cookies)
        तथ्य->cookies = Curl_cookie_init();
      अगर(!तथ्य->cookies)
        परिणाम = CURLE_OUT_OF_MEMORY;
      अन्यथा
        तथ्य->state.cookie_engine = TRUE;
    }
    अवरोध;

  हाल CURLOPT_COOKIELIST:
    वापस cookielist(तथ्य, ptr);
#पूर्ण_अगर /* !CURL_DISABLE_COOKIES */

#पूर्ण_अगर /* !CURL_DISABLE_HTTP */
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }
  वापस परिणाम;
}
#पूर्ण_अगर /* !CURL_DISABLE_HTTP || !CURL_DISABLE_MQTT */

#अगर_घोषित USE_SSH
अटल CURLcode setopt_cptr_ssh(काष्ठा Curl_easy *तथ्य, CURLoption option,
                                अक्षर *ptr)
{
  काष्ठा UserDefined *s = &तथ्य->set;
  चयन(option) {
  हाल CURLOPT_SSH_PUBLIC_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa.pub file
     */
    वापस Curl_setstropt(तथ्य, STRING_SSH_PUBLIC_कुंजी, ptr);
  हाल CURLOPT_SSH_PRIVATE_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa file
     */
    वापस Curl_setstropt(तथ्य, STRING_SSH_PRIVATE_कुंजी, ptr);
  हाल CURLOPT_SSH_KEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_keyfunc_userp = ptr;
    अवरोध;
  हाल CURLOPT_SSH_HOST_PUBLIC_कुंजी_MD5:
    /*
     * Option to allow for the MD5 of the host public key to be checked
     * for validation purposes.
     */
    वापस Curl_setstropt(तथ्य, STRING_SSH_HOST_PUBLIC_कुंजी_MD5, ptr);
  हाल CURLOPT_SSH_HOST_PUBLIC_कुंजी_SHA256:
    /*
     * Option to allow for the SHA256 of the host public key to be checked
     * for validation purposes.
     */
    वापस Curl_setstropt(तथ्य, STRING_SSH_HOST_PUBLIC_कुंजी_SHA256, ptr);
  हाल CURLOPT_SSH_KNOWNHOSTS:
    /*
     * Store the filename to read known hosts from.
     */
    वापस Curl_setstropt(तथ्य, STRING_SSH_KNOWNHOSTS, ptr);
#अगर_घोषित USE_LIBSSH2
  हाल CURLOPT_SSH_HOSTKEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_hostkeyfunc_userp = ptr;
    अवरोध;
#पूर्ण_अगर /* USE_LIBSSH2 */
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }
  वापस CURLE_OK;
}
#पूर्ण_अगर /* USE_SSH */

#अगर_अघोषित CURL_DISABLE_FTP
अटल CURLcode setopt_cptr_ftp(काष्ठा Curl_easy *तथ्य, CURLoption option,
                                अक्षर *ptr)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा UserDefined *s = &तथ्य->set;
  चयन(option) {
  हाल CURLOPT_FTPPORT:
    /*
     * Use FTP PORT, this also specifies which IP address to use
     */
    परिणाम = Curl_setstropt(तथ्य, STRING_FTPPORT, ptr);
    s->ftp_use_port = !!CURL_EASY_STR(तथ्य, STRING_FTPPORT);
    अवरोध;

  हाल CURLOPT_FTP_ACCOUNT:
    वापस Curl_setstropt(तथ्य, STRING_FTP_ACCOUNT, ptr);

  हाल CURLOPT_FTP_ALTERNATIVE_TO_USER:
    वापस Curl_setstropt(तथ्य, STRING_FTP_ALTERNATIVE_TO_USER, ptr);

  हाल CURLOPT_KRBLEVEL:
    वापस CURLE_NOT_BUILT_IN; /* removed in 8.17.0 */
  हाल CURLOPT_CHUNK_तथ्य:
    s->wildcardptr = ptr;
    अवरोध;
  हाल CURLOPT_FNMATCH_तथ्य:
    s->fnmatch_तथ्य = ptr;
    अवरोध;
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }
  वापस परिणाम;
}
#पूर्ण_अगर /* !CURL_DISABLE_FTP */

अटल CURLcode setopt_cptr_net(काष्ठा Curl_easy *तथ्य, CURLoption option,
                                अक्षर *ptr)
{
  चयन(option) {
  हाल CURLOPT_INTERFACE:
    /*
     * Set what interface or address/hostname to bind the socket to when
     * performing an operation and thus what from-IP your connection will use.
     */
    वापस setstropt_interface(तथ्य, ptr);

#अगर_घोषित USE_RESOLV_ARES
  हाल CURLOPT_DNS_SERVERS:
    वापस Curl_setstropt(तथ्य, STRING_DNS_SERVERS, ptr);

  हाल CURLOPT_DNS_INTERFACE:
    वापस Curl_setstropt(तथ्य, STRING_DNS_INTERFACE, ptr);

  हाल CURLOPT_DNS_LOCAL_IP4:
    वापस Curl_setstropt(तथ्य, STRING_DNS_LOCAL_IP4, ptr);

  हाल CURLOPT_DNS_LOCAL_IP6:
    वापस Curl_setstropt(तथ्य, STRING_DNS_LOCAL_IP6, ptr);
#पूर्ण_अगर
#अगर_घोषित USE_UNIX_SOCKETS
  हाल CURLOPT_UNIX_SOCKET_PATH:
    तथ्य->set.abstract_unix_socket = FALSE;
    वापस Curl_setstropt(तथ्य, STRING_UNIX_SOCKET_PATH, ptr);

  हाल CURLOPT_ABSTRACT_UNIX_SOCKET:
    तथ्य->set.abstract_unix_socket = TRUE;
    वापस Curl_setstropt(तथ्य, STRING_UNIX_SOCKET_PATH, ptr);
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_DOH
  हाल CURLOPT_DOH_URL:
    {
      CURLcode परिणाम = Curl_setstropt(तथ्य, STRING_DOH, ptr);
      तथ्य->set.doh = !!CURL_EASY_STR(तथ्य, STRING_DOH);
      वापस परिणाम;
    }
#पूर्ण_अगर
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }
}

अटल CURLcode setopt_cptr_misc(काष्ठा Curl_easy *तथ्य, CURLoption option,
                                 अक्षर *ptr)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा UserDefined *s = &तथ्य->set;

  चयन(option) {
  हाल CURLOPT_REQUEST_TARGET:
    वापस Curl_setstropt(तथ्य, STRING_TARGET, ptr);
#अगर_अघोषित CURL_DISABLE_NETRC
  हाल CURLOPT_NETRC_FILE:
    वापस Curl_setstropt(तथ्य, STRING_NETRC_FILE, ptr);
#पूर्ण_अगर
  हाल CURLOPT_CUSTOMREQUEST:
    वापस Curl_setstropt(तथ्य, STRING_CUSTOMREQUEST, ptr);

    /* we do not set s->method = HTTPREQ_CUSTOM; here, we continue as if we
       were using the already set type and this changes the actual request
       keyword */
  हाल CURLOPT_SERVICE_NAME:
    वापस Curl_setstropt(तथ्य, STRING_SERVICE_NAME, ptr);

  हाल CURLOPT_HEADERDATA:
    s->writeheader = ptr;
    अवरोध;
  हाल CURLOPT_READDATA:
    s->in_set = ptr;
    अवरोध;
  हाल CURLOPT_WRITEDATA:
    s->out = ptr;
    अवरोध;
  हाल CURLOPT_DEBUGDATA:
    s->debugdata = ptr;
    अवरोध;
  हाल CURLOPT_PROGRESSDATA:
    s->progress_client = ptr;
    अवरोध;
  हाल CURLOPT_SEEKDATA:
    s->seek_client = ptr;
    अवरोध;
  हाल CURLOPT_IOCTLDATA:
    s->ioctl_client = ptr;
    अवरोध;
  हाल CURLOPT_SOCKOPTDATA:
    s->sockopt_client = ptr;
    अवरोध;
  हाल CURLOPT_OPENSOCKETDATA:
    s->opensocket_client = ptr;
    अवरोध;
  हाल CURLOPT_RESOLVER_START_तथ्य:
    s->resolver_start_client = ptr;
    अवरोध;
  हाल CURLOPT_CLOSESOCKETDATA:
    s->closesocket_client = ptr;
    अवरोध;
  हाल CURLOPT_PREREQDATA:
    s->prereq_userp = ptr;
    अवरोध;
  हाल CURLOPT_ERRORBUFFER:
    s->errorbuffer = ptr;
    अवरोध;
  हाल CURLOPT_URL:
    परिणाम = Curl_setstropt(तथ्य, STRING_SET_URL, ptr);
    Curl_bufref_set(&तथ्य->state.url,
                    CURL_EASY_STR(तथ्य, STRING_SET_URL), 0, शून्य);
    अवरोध;

  हाल CURLOPT_USERPWD: {
    अक्षर *u = शून्य, *p = शून्य;
    परिणाम = setstropt_userpwd(ptr, &u, &p);
    अगर(!परिणाम) {
      परिणाम = CURL_EASY_STR_SETN(तथ्य, STRING_USERNAME, u);
      u = शून्य;
    }
    अगर(!परिणाम) {
      परिणाम = CURL_EASY_STR_SETN(तथ्य, STRING_PASSWORD, p);
      p = शून्य;
    }
    curlx_free(u);
    curlx_free(p);
    वापस परिणाम;
  }

  हाल CURLOPT_USERNAME:
    वापस Curl_setstropt(तथ्य, STRING_USERNAME, ptr);

  हाल CURLOPT_PASSWORD:
    वापस Curl_setstropt(तथ्य, STRING_PASSWORD, ptr);

  हाल CURLOPT_LOGIN_OPTIONS:
    वापस Curl_setstropt(तथ्य, STRING_OPTIONS, ptr);

  हाल CURLOPT_XOAUTH2_BEARER:
    वापस Curl_setstropt(तथ्य, STRING_BEARER, ptr);
  हाल CURLOPT_RANGE:
    वापस Curl_setstropt(तथ्य, STRING_SET_RANGE, ptr);
  हाल CURLOPT_PRIVATE:
    s->private_तथ्य = ptr;
    अवरोध;
  हाल CURLOPT_PROTOCOLS_STR:
    अगर(ptr) {
      curl_prot_t protos;
      परिणाम = protocol2num(ptr, &protos);
      अगर(!परिणाम)
        s->allowed_protocols = protos;
    }
    अन्यथा
      /* make a NULL argument reset to default */
      s->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
    अवरोध;
  हाल CURLOPT_REDIR_PROTOCOLS_STR:
    अगर(ptr) {
      curl_prot_t protos;
      परिणाम = protocol2num(ptr, &protos);
      अगर(!परिणाम)
        s->redir_protocols = protos;
    }
    अन्यथा
      /* make a NULL argument reset to default */
      s->redir_protocols = (curl_prot_t)CURLPROTO_REDIR;
    अवरोध;
  हाल CURLOPT_DEFAULT_PROTOCOL:
    /* Set the protocol to use when the URL does not include any protocol */
    वापस Curl_setstropt(तथ्य, STRING_DEFAULT_PROTOCOL, ptr);
#अगर_अघोषित CURL_DISABLE_SMTP
  हाल CURLOPT_MAIL_FROM:
    /* Set the SMTP mail originator */
    वापस Curl_setstropt(तथ्य, STRING_MAIL_FROM, ptr);
  हाल CURLOPT_MAIL_AUTH:
    /* Set the SMTP auth originator */
    वापस Curl_setstropt(तथ्य, STRING_MAIL_AUTH, ptr);
#पूर्ण_अगर
  हाल CURLOPT_SASL_AUTHZID:
    /* Authorization identity (identity to act as) */
    वापस Curl_setstropt(तथ्य, STRING_SASL_AUTHZID, ptr);
#अगर_अघोषित CURL_DISABLE_RTSP
  हाल CURLOPT_RTSP_SESSION_ID:
    Curl_peer_unlink(&तथ्य->state.rtsp_session_origin);
    वापस Curl_setstropt(तथ्य, STRING_RTSP_SESSION_ID, ptr);
  हाल CURLOPT_RTSP_STREAM_URI:
    वापस Curl_setstropt(तथ्य, STRING_RTSP_STREAM_URI, ptr);
  हाल CURLOPT_RTSP_TRANSPORT:
    वापस Curl_setstropt(तथ्य, STRING_RTSP_TRANSPORT, ptr);
  हाल CURLOPT_INTERLEAVEDATA:
    s->rtp_out = ptr;
    अवरोध;
#पूर्ण_अगर /* !CURL_DISABLE_RTSP */
  हाल CURLOPT_TLSAUTH_USERNAME:
  हाल CURLOPT_TLSAUTH_PASSWORD:
  हाल CURLOPT_TLSAUTH_TYPE:
  हाल CURLOPT_PROXY_TLSAUTH_USERNAME:
  हाल CURLOPT_PROXY_TLSAUTH_PASSWORD:
  हाल CURLOPT_PROXY_TLSAUTH_TYPE:
    वापस CURLE_NOT_BUILT_IN;
#अगर_अघोषित CURL_DISABLE_HSTS
  हाल CURLOPT_HSTSREADDATA:
    s->hsts_read_userp = ptr;
    अवरोध;
  हाल CURLOPT_HSTSWRITEDATA:
    s->hsts_write_userp = ptr;
    अवरोध;
  हाल CURLOPT_HSTS: {
    काष्ठा curl_slist *h;
    अगर(!तथ्य->hsts) {
      तथ्य->hsts = Curl_hsts_init();
      अगर(!तथ्य->hsts)
        वापस CURLE_OUT_OF_MEMORY;
    }
    अगर(ptr) {
      परिणाम = Curl_setstropt(तथ्य, STRING_HSTS, ptr);
      अगर(परिणाम)
        वापस परिणाम;
      /* this needs to build a list of filenames to read from, so that it can
         read them later, as we might get a shared HSTS handle to load them
         into */
      h = curl_slist_append(तथ्य->state.hstslist, ptr);
      अगर(!h) {
        curl_slist_free_all(तथ्य->state.hstslist);
        तथ्य->state.hstslist = शून्य;
        वापस CURLE_OUT_OF_MEMORY;
      }
      तथ्य->state.hstslist = h; /* store the list for later use */
    }
    अन्यथा {
      /* clear the list of HSTS files */
      curl_slist_free_all(तथ्य->state.hstslist);
      तथ्य->state.hstslist = शून्य;
      अगर(!तथ्य->share || !तथ्य->share->hsts)
        /* throw away the HSTS cache unless shared */
        Curl_hsts_cleanup(&तथ्य->hsts);
    }
    अवरोध;
  }
#पूर्ण_अगर /* !CURL_DISABLE_HSTS */
#अगर_अघोषित CURL_DISABLE_ALTSVC
  हाल CURLOPT_ALTSVC:
    अगर(!तथ्य->asi) {
      तथ्य->asi = Curl_altsvc_init();
      अगर(!तथ्य->asi)
        वापस CURLE_OUT_OF_MEMORY;
    }
    परिणाम = Curl_setstropt(तथ्य, STRING_ALTSVC, ptr);
    अगर(परिणाम)
      अवरोध;
    अगर(ptr)
      वापस Curl_altsvc_load(तथ्य->asi, ptr);
    अवरोध;
#पूर्ण_अगर /* !CURL_DISABLE_ALTSVC */
  हाल CURLOPT_ECH:
    वापस setopt_ech(तथ्य, ptr);
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }
  वापस परिणाम;
}

अटल CURLcode setopt_cptr(काष्ठा Curl_easy *तथ्य, CURLoption option,
                            अक्षर *ptr)
{
  प्रकार CURLcode (*ptrfunc)(काष्ठा Curl_easy *तथ्य, CURLoption option,
                              अक्षर *ptr);
  /* Order by likeliness */
  अटल स्थिर ptrfunc setopt_call[] = {
    setopt_cptr_misc,
#अगर defined(USE_SSL) || defined(USE_SSH)
    setopt_cptr_ssl,
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_PROXY
    setopt_cptr_proxy,
#पूर्ण_अगर
    setopt_cptr_net,
#अगर_अघोषित CURL_DISABLE_FTP
    setopt_cptr_ftp,
#पूर्ण_अगर
#अगर_घोषित USE_SSH
    setopt_cptr_ssh,
#पूर्ण_अगर
#अगर !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
    setopt_cptr_http_mqtt,
#पूर्ण_अगर
  };
  माप_प्रकार i;

  क्रम(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode परिणाम = setopt_call[i](तथ्य, option, ptr);
    अगर(परिणाम != CURLE_UNKNOWN_OPTION)
      वापस परिणाम;
  }
  वापस CURLE_UNKNOWN_OPTION;
}

अटल CURLcode setopt_func(काष्ठा Curl_easy *तथ्य, CURLoption option,
                            बहु_सूची param)
{
  काष्ठा UserDefined *s = &तथ्य->set;
  चयन(option) {
  हाल CURLOPT_PROGRESSFUNCTION:
    /*
     * Progress callback function
     */
    s->fprogress = बहु_तर्क(param, curl_progress_callback);
    अगर(s->fprogress)
      तथ्य->progress.callback = TRUE; /* no longer internal */
    अन्यथा
      तथ्य->progress.callback = FALSE; /* NULL enforces internal */
    अवरोध;

  हाल CURLOPT_XFERINFOFUNCTION:
    /*
     * Transfer info callback function
     */
    s->fxferinfo = बहु_तर्क(param, curl_xferinfo_callback);
    अगर(s->fxferinfo)
      तथ्य->progress.callback = TRUE; /* no longer internal */
    अन्यथा
      तथ्य->progress.callback = FALSE; /* NULL enforces internal */

    अवरोध;
  हाल CURLOPT_DEBUGFUNCTION:
    /*
     * stderr write callback.
     */
    s->fdebug = बहु_तर्क(param, curl_debug_callback);
    /*
     * if the callback provided is NULL, it will use the default callback
     */
    अवरोध;
  हाल CURLOPT_HEADERFUNCTION:
    /*
     * Set header write callback
     */
    s->fwrite_header = बहु_तर्क(param, curl_write_callback);
    अवरोध;
  हाल CURLOPT_WRITEFUNCTION:
    /*
     * Set data write callback
     */
    s->fwrite_func = बहु_तर्क(param, curl_write_callback);
    अगर(!s->fwrite_func)
#अगर defined(__clang__) && __clang_major__ >= 16
#आशय clang diagnostic धकेलो
#आशय clang diagnostic ignored "-Wcast-function-type-strict"
#पूर्ण_अगर
      /* When set to NULL, reset to our internal default function */
      s->fwrite_func = (curl_write_callback)ख_डालो;
#अगर defined(__clang__) && __clang_major__ >= 16
#आशय clang diagnostic निकालो
#पूर्ण_अगर
    अवरोध;
  हाल CURLOPT_READFUNCTION:
    /*
     * Read data callback
     */
    s->fread_func_set = बहु_तर्क(param, curl_read_callback);
    अगर(!s->fread_func_set) {
      s->is_fread_set = 0;
#अगर defined(__clang__) && __clang_major__ >= 16
#आशय clang diagnostic धकेलो
#आशय clang diagnostic ignored "-Wcast-function-type-strict"
#पूर्ण_अगर
      /* When set to NULL, reset to our internal default function */
      s->fread_func_set = (curl_read_callback)ख_पढ़ो;
#अगर defined(__clang__) && __clang_major__ >= 16
#आशय clang diagnostic निकालो
#पूर्ण_अगर
    }
    अन्यथा
      s->is_fread_set = 1;
    अवरोध;
  हाल CURLOPT_SEEKFUNCTION:
    /*
     * Seek callback. Might be NULL.
     */
    s->seek_func = बहु_तर्क(param, curl_seek_callback);
    अवरोध;
  हाल CURLOPT_IOCTLFUNCTION:
    /*
     * I/O control callback. Might be NULL.
     */
    s->ioctl_func = बहु_तर्क(param, curl_ioctl_callback);
    अवरोध;
  हाल CURLOPT_SSL_CTX_FUNCTION:
    /*
     * Set an SSL_CTX callback
     */
#अगर_घोषित USE_SSL
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_SSL_CTX)) {
      s->ssl_fsslctx = बहु_तर्क(param, curl_ssl_ctx_callback);
      अवरोध;
    }
    अन्यथा
#पूर्ण_अगर
      वापस CURLE_NOT_BUILT_IN;

  हाल CURLOPT_SOCKOPTFUNCTION:
    /*
     * socket callback function: called after socket() but before connect()
     */
    s->fsockopt = बहु_तर्क(param, curl_sockopt_callback);
    अवरोध;

  हाल CURLOPT_OPENSOCKETFUNCTION:
    /*
     * open/create socket callback function: called instead of socket(),
     * before connect()
     */
    s->fopensocket = बहु_तर्क(param, curl_opensocket_callback);
    अवरोध;

  हाल CURLOPT_CLOSESOCKETFUNCTION:
    /*
     * close socket callback function: called instead of close()
     * when shutting down a connection
     */
    s->fclosesocket = बहु_तर्क(param, curl_closesocket_callback);
    अवरोध;

  हाल CURLOPT_RESOLVER_START_FUNCTION:
    /*
     * resolver start callback function: called before a new resolver request
     * is started
     */
    s->resolver_start = बहु_तर्क(param, curl_resolver_start_callback);
    अवरोध;

#अगर_घोषित USE_SSH
#अगर_घोषित USE_LIBSSH2
  हाल CURLOPT_SSH_HOSTKEYFUNCTION:
    /* the callback to check the hostkey without the knownhost file */
    s->ssh_hostkeyfunc = बहु_तर्क(param, curl_sshhostkeycallback);
    अवरोध;
#पूर्ण_अगर

  हाल CURLOPT_SSH_KEYFUNCTION:
    /* setting to NULL is fine since the ssh.c functions themselves will
       then revert to use the internal default */
    s->ssh_keyfunc = बहु_तर्क(param, curl_sshkeycallback);
    अवरोध;

#पूर्ण_अगर /* USE_SSH */

#अगर_अघोषित CURL_DISABLE_RTSP
  हाल CURLOPT_INTERLEAVEFUNCTION:
    /* Set the user defined RTP write function */
    s->fwrite_rtp = बहु_तर्क(param, curl_write_callback);
    अवरोध;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_FTP
  हाल CURLOPT_CHUNK_BGN_FUNCTION:
    s->chunk_bgn = बहु_तर्क(param, curl_chunk_bgn_callback);
    अवरोध;
  हाल CURLOPT_CHUNK_END_FUNCTION:
    s->chunk_end = बहु_तर्क(param, curl_chunk_end_callback);
    अवरोध;
  हाल CURLOPT_FNMATCH_FUNCTION:
    s->fnmatch = बहु_तर्क(param, curl_fnmatch_callback);
    अवरोध;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HTTP
  हाल CURLOPT_TRAILERFUNCTION:
    s->trailer_callback = बहु_तर्क(param, curl_trailer_callback);
    अवरोध;
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HSTS
  हाल CURLOPT_HSTSREADFUNCTION:
    s->hsts_read = बहु_तर्क(param, curl_hstsread_callback);
    अवरोध;
  हाल CURLOPT_HSTSWRITEFUNCTION:
    s->hsts_write = बहु_तर्क(param, curl_hstswrite_callback);
    अवरोध;
#पूर्ण_अगर
  हाल CURLOPT_PREREQFUNCTION:
    s->fprereq = बहु_तर्क(param, curl_prereq_callback);
    अवरोध;
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }
  वापस CURLE_OK;
}

अटल CURLcode setopt_offt(काष्ठा Curl_easy *तथ्य, CURLoption option,
                            curl_off_t offt)
{
  काष्ठा UserDefined *s = &तथ्य->set;
  चयन(option) {
  हाल CURLOPT_TIMEVALUE_LARGE:
    /*
     * This is the value to compare with the remote document with the
     * method set with CURLOPT_TIMECONDITION
     */
    s->timevalue = (समय_प्रकार)offt;
    अवरोध;

    /* MQTT "borrows" some of the HTTP options */
  हाल CURLOPT_POSTFIELDSIZE_LARGE:
    /*
     * The size of the POSTFIELD data to prevent libcurl to do strlen() to
     * figure it out. Enables binary posts.
     */
    अगर(offt < -1)
      वापस CURLE_BAD_FUNCTION_ARGUMENT;

    अगर(s->postfieldsize < offt && s->str_copypostfields) {
      /* Previous CURLOPT_COPYPOSTFIELDS is no longer valid. */
      curlx_safefree(s->str_copypostfields);
      s->postfields = शून्य;
    }
    s->postfieldsize = offt;
    अवरोध;
  हाल CURLOPT_INFILESIZE_LARGE:
    /*
     * If known, this should inform curl about the file size of the
     * to-be-uploaded file.
     */
    अगर(offt < -1)
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = offt;
    अवरोध;
  हाल CURLOPT_MAX_SEND_SPEED_LARGE:
    /*
     * When transfer uploads are faster than CURLOPT_MAX_SEND_SPEED_LARGE
     * bytes per second the transfer is throttled..
     */
    अगर(offt < 0)
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_send_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&तथ्य->progress.ul.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    अवरोध;
  हाल CURLOPT_MAX_RECV_SPEED_LARGE:
    /*
     * When receiving data faster than CURLOPT_MAX_RECV_SPEED_LARGE bytes per
     * second the transfer is throttled..
     */
    अगर(offt < 0)
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_recv_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&तथ्य->progress.dl.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    अवरोध;
  हाल CURLOPT_RESUME_FROM_LARGE:
    /*
     * Resume transfer at the given file position
     */
    अगर(offt < -1)
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = offt;
    अवरोध;
  हाल CURLOPT_MAXFILESIZE_LARGE:
    /*
     * Set the maximum size of a file to download.
     */
    अगर(offt < 0)
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_filesize = offt;
    अवरोध;

  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }
  वापस CURLE_OK;
}

अटल CURLcode setopt_blob(काष्ठा Curl_easy *तथ्य, CURLoption option,
                            काष्ठा curl_blob *blob)
{
  काष्ठा UserDefined *s = &तथ्य->set;
  चयन(option) {
  हाल CURLOPT_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use
     */
    वापस Curl_setblobopt(&s->blobs[BLOB_CERT], blob);
#अगर_अघोषित CURL_DISABLE_PROXY
  हाल CURLOPT_PROXY_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use for proxy
     */
    वापस Curl_setblobopt(&s->blobs[BLOB_CERT_PROXY], blob);
  हाल CURLOPT_PROXY_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use for proxy
     */
    वापस Curl_setblobopt(&s->blobs[BLOB_कुंजी_PROXY], blob);
  हाल CURLOPT_PROXY_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection proxy.
     * Specify entire PEM of the CA certificate
     */
#अगर_घोषित USE_SSL
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_CAINFO_BLOB)) {
      CURLcode परिणाम = Curl_setblobopt(&s->blobs[BLOB_CAINFO_PROXY], blob);
      अगर(!परिणाम)
        s->proxy_ssl.custom_cablob = TRUE;
      वापस परिणाम;
    }
#पूर्ण_अगर
    वापस CURLE_NOT_BUILT_IN;
  हाल CURLOPT_PROXY_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_ISSUERCERT_BLOB))
      वापस Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT_PROXY], blob);
    वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
  हाल CURLOPT_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use
     */
    वापस Curl_setblobopt(&s->blobs[BLOB_कुंजी], blob);
  हाल CURLOPT_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection.
     * Specify entire PEM of the CA certificate
     */
#अगर_घोषित USE_SSL
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_CAINFO_BLOB)) {
      CURLcode परिणाम = Curl_setblobopt(&s->blobs[BLOB_CAINFO], blob);
      अगर(!परिणाम)
        s->ssl.custom_cablob = TRUE;
      वापस परिणाम;
    }
#पूर्ण_अगर
    वापस CURLE_NOT_BUILT_IN;
  हाल CURLOPT_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    अगर(Curl_ssl_supports(तथ्य, SSLSUPP_ISSUERCERT_BLOB))
      वापस Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT], blob);
    वापस CURLE_NOT_BUILT_IN;

  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }
  /* unreachable */
}

/*
 * Do not make Curl_vsetopt() static: it is called from
 * projects/OS400/ccsidcurl.c.
 */
CURLcode Curl_vsetopt(काष्ठा Curl_easy *तथ्य, CURLoption option, बहु_सूची param)
{
  अगर(option < CURLOPTTYPE_OBJECTPOINT)
    वापस setopt_long(तथ्य, option, बहु_तर्क(param, दीर्घ));
  अन्यथा अगर(option < CURLOPTTYPE_FUNCTIONPOINT) {
    /* unfortunately, different pointer types cannot be identified any other
       way than being listed explicitly */
    चयन(option) {
    हाल CURLOPT_HTTPHEADER:
    हाल CURLOPT_QUOTE:
    हाल CURLOPT_POSTQUOTE:
    हाल CURLOPT_TELNETOPTIONS:
    हाल CURLOPT_PREQUOTE:
    हाल CURLOPT_HTTP200ALIASES:
    हाल CURLOPT_MAIL_RCPT:
    हाल CURLOPT_RESOLVE:
    हाल CURLOPT_PROXYHEADER:
    हाल CURLOPT_CONNECT_TO:
      वापस setopt_slist(तथ्य, option, बहु_तर्क(param, काष्ठा curl_slist *));
    हाल CURLOPT_HTTPPOST:         /* curl_httppost * */
    हाल CURLOPT_MIMEPOST:         /* curl_mime * */
    हाल CURLOPT_STDERR:           /* FILE * */
    हाल CURLOPT_SHARE:            /* CURLSH * */
    हाल CURLOPT_CURLU:            /* CURLU * */
      वापस setopt_pointers(तथ्य, option, param);
    हाल CURLOPT_STREAM_DEPENDS:   /* CURL * */
    हाल CURLOPT_STREAM_DEPENDS_E: /* CURL * */
      वापस CURLE_OK;
    शेष:
      अवरोध;
    }
    /* the char pointer options */
    वापस setopt_cptr(तथ्य, option, बहु_तर्क(param, अक्षर *));
  }
  अन्यथा अगर(option < CURLOPTTYPE_OFF_T)
    वापस setopt_func(तथ्य, option, param);
  अन्यथा अगर(option < CURLOPTTYPE_BLOB)
    वापस setopt_offt(तथ्य, option, बहु_तर्क(param, curl_off_t));
  वापस setopt_blob(तथ्य, option, बहु_तर्क(param, काष्ठा curl_blob *));
}

/*
 * curl_easy_setopt() is the external interface for setting options on an
 * easy handle.
 *
 * NOTE: This is one of few API functions that are allowed to be called from
 * within a callback.
 */
#अघोषित curl_easy_setopt
CURLcode curl_easy_setopt(CURL *curl, CURLoption option, ...)
{
  काष्ठा Curl_eapi_guard guard;
  CURLcode परिणाम;

  अगर(CURL_EAPI_ENTER(&guard, curl, easy_setopt, &परिणाम)) {
    काष्ठा Curl_easy *तथ्य = curl;
    बहु_सूची arg;

    बहु_शुरू(arg, option);

    परिणाम = Curl_vsetopt(तथ्य, option, arg);

    बहु_पूर्ण(arg);
    अगर(परिणाम == CURLE_BAD_FUNCTION_ARGUMENT)
      failf(तथ्य, "setopt 0x%x got bad argument", (अचिन्हित पूर्णांक)option);
  }
  CURL_EAPI_LEAVE(&guard);
  वापस परिणाम;
}
