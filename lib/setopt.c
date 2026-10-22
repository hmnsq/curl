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
#અન્તર્ભાવ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif

#ifdef HAVE_LINUX_TCP_H
#અન્તર્ભાવ <linux/tcp.h>
#અથવાજો defined(HAVE_NETINET_TCP_H)
#અન્તર્ભાવ <netinet/tcp.h>
#endif

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "content_encoding.h"
#અન્તર્ભાવ "strcase.h"
#અન્તર્ભાવ "curl_share.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "setopt.h"
#અન્તર્ભાવ "altsvc.h"
#અન્તર્ભાવ "hsts.h"
#અન્તર્ભાવ "tftp.h"
#અન્તર્ભાવ "curlx/strdup.h"
#અન્તર્ભાવ "escape.h"
#અન્તર્ભાવ "bufref.h"
#અન્તર્ભાવ "vauth/vauth.h"

સ્થિર CURLcode setopt_set_timeout_sec(timediff_t *ptimeout_ms, long secs)
{
  જો(secs < 0)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
#જો LONG_MAX > (TIMEDIFF_T_MAX / 1000)
  જો(secs > (TIMEDIFF_T_MAX / 1000)) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    પાછા CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)secs * 1000;
  પાછા CURLE_OK;
}

સ્થિર CURLcode setopt_set_timeout_ms(timediff_t *ptimeout_ms, long ms)
{
  જો(ms < 0)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
#જો LONG_MAX > TIMEDIFF_T_MAX
  જો(ms > TIMEDIFF_T_MAX) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    પાછા CURLE_OK;
  }
#endif
  *ptimeout_ms = (timediff_t)ms;
  પાછા CURLE_OK;
}

CURLcode Curl_setstropt(સંરચના Curl_easy *data,
                        ગણના dupstring id, અચલ અક્ષર *s)
{
  size_t slen = s ? strlen(s) : 0;
  DEBUGASSERT((unsigned)id <= UINT8_MAX);
  જો(s && (slen > CURL_MAX_આગત_LENGTH))
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  પાછા CURL_EASY_STR_SET(data, (uint8_t)id, s, slen);
}

CURLcode Curl_setblobopt(સંરચના curl_blob **blobp,
                         અચલ સંરચના curl_blob *blob)
{
  /* free the previous storage at `blobp' and replace by a dynamic storage
     copy of blob. If CURL_BLOB_COPY is set, the data is copied. */

  curlx_safefree(*blobp);

  જો(blob) {
    સંરચના curl_blob *nblob;
    જો(!blob->data || !blob->len || (blob->len > CURL_MAX_આગત_LENGTH))
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    nblob = (સંરચના curl_blob *)
      curlx_malloc(કદ(સંરચના curl_blob) +
                   ((blob->flags & CURL_BLOB_COPY) ? blob->len : 0));
    જો(!nblob)
      પાછા CURLE_OUT_OF_MEMORY;
    *nblob = *blob;
    જો(blob->flags & CURL_BLOB_COPY) {
      /* put the data after the blob struct in memory */
      nblob->data = (અક્ષર *)nblob + કદ(સંરચના curl_blob);
      memcpy(nblob->data, blob->data, blob->len);
    }

    *blobp = nblob;
    પાછા CURLE_OK;
  }

  પાછા CURLE_OK;
}

સ્થિર CURLcode setstropt_userpwd(અચલ અક્ષર *option, અક્ષર **userp,
                                  અક્ષર **passwdp)
{
  અક્ષર *user = ખાલી;
  અક્ષર *passwd = ખાલી;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Parse the login details if specified. If not, then we treat NULL as a
     hint to clear the existing data */
  જો(option) {
    size_t len = strlen(option);
    CURLcode result;
    જો(len > CURL_MAX_આગત_LENGTH)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;

    result = Curl_parse_login_details(option, len, &user, &passwd, ખાલી);
    જો(result)
      પાછા result;
  }

  curlx_free(*userp);
  *userp = user;

  curlx_strzero(*passwdp);
  curlx_free(*passwdp);
  *passwdp = passwd;

  પાછા CURLE_OK;
}

સ્થિર CURLcode setstropt_interface(સંરચના Curl_easy *data, અક્ષર *option)
{
  જો(option && !option[0])
    option = ખાલી;
  જો(option) {
    /* Parse for checking syntax */
    CURLcode result = Curl_parse_interface(option, ખાલી, ખાલી, ખાલી);
    જો(result)
      પાછા result;
    પાછા CURL_EASY_STR_SET(data, STRING_INTERFACE_OPT,
                             option, strlen(option));
  }
  CURL_EASY_STR_CLEAR(data, STRING_INTERFACE_OPT);
  પાછા CURLE_OK;
}

#ifdef USE_SSL
#define C_SSLVERSION_કિંમત(x)     ((x) & 0xffff)
#define C_SSLVERSION_MAX_કિંમત(x) ((unsigned long)(x) & 0xffff0000)
#endif

સ્થિર CURLcode protocol2num(અચલ અક્ષર *str, curl_prot_t *val)
{
  /*
   * We are asked to cherry-pick protocols, so play it safe and disallow all
   * protocols to start with, and re-add the wanted ones back in.
   */
  *val = 0;

  જો(!str)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  જો(curl_strequal(str, "all")) {
    *val = ~(curl_prot_t)0;
    પાછા CURLE_OK;
  }

  કરો {
    અચલ અક્ષર *token = str;
    size_t tlen;

    str = strchr(str, ',');
    tlen = str ? (size_t)(str - token) : strlen(token);
    જો(tlen) {
      અચલ સંરચના Curl_scheme *h = Curl_getn_scheme(token, tlen);

      જો(!h || !h->run)
        પાછા CURLE_UNSUPPORTED_PROTOCOL;

      *val |= h->protocol;
    }
  } જ્યારે(str && str++);

  જો(!*val)
    /* no protocol listed */
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  પાછા CURLE_OK;
}

#જો !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
સ્થિર CURLcode httpauth(સંરચના Curl_easy *data, bool proxy,
                         unsigned long auth)
{
  જો(auth != CURLAUTH_NONE) {
    પૂર્ણાંક bitcheck = 0;
    bool authbits = FALSE;
    જો(auth & CURLAUTH_DIGEST_IE) {
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
    જ્યારે(bitcheck < 31) {
      જો(auth & (1UL << bitcheck++)) {
        authbits = TRUE;
        તોડો;
      }
    }
    જો(!authbits)
      પાછા CURLE_NOT_BUILT_IN; /* no supported types left! */
  }
  જો(proxy)
    data->set.proxyauth = (uint32_t)auth;
  નહીંતર
    data->set.httpauth = (uint32_t)auth;
  પાછા CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */

#ifndef CURL_DISABLE_HTTP
સ્થિર CURLcode setopt_HTTP_VERSION(સંરચના Curl_easy *data, long arg)
{
  /*
   * This sets a requested HTTP version to be used. The value is one of
   * the listed enums in curl/curl.h.
   */
  પસંદગી(arg) {
  સ્થિતિ CURL_HTTP_VERSION_NONE:
    /* accepted */
    તોડો;
  સ્થિતિ CURL_HTTP_VERSION_1_0:
  સ્થિતિ CURL_HTTP_VERSION_1_1:
    /* accepted */
    તોડો;
#ifdef USE_HTTP2
  સ્થિતિ CURL_HTTP_VERSION_2_0:
  સ્થિતિ CURL_HTTP_VERSION_2TLS:
  સ્થિતિ CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE:
    /* accepted */
    તોડો;
#endif
#ifdef USE_HTTP3
  સ્થિતિ CURL_HTTP_VERSION_3:
  સ્થિતિ CURL_HTTP_VERSION_3ONLY:
    /* accepted */
    તોડો;
#endif
  મૂળભૂત:
    /* not accepted */
    જો(arg < CURL_HTTP_VERSION_NONE)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    પાછા CURLE_UNSUPPORTED_PROTOCOL;
  }
  data->set.httpwant = (unsigned અક્ષર)arg;
  પાછા CURLE_OK;
}
#endif /* !CURL_DISABLE_HTTP */

#ifdef USE_SSL
CURLcode Curl_setopt_SSLVERSION(સંરચના Curl_easy *data, CURLoption option,
                                long arg)
{
  /*
   * Set explicit SSL version to try to connect with, as some SSL
   * implementations are lame.
   */
  {
    long version, version_max;
    સંરચના ssl_easy_config *sslc = &data->set.ssl;
#ifndef CURL_DISABLE_PROXY
    જો(option != CURLOPT_SSLVERSION)
      sslc = &data->set.proxy_ssl;
#નહીંતર
    (શૂન્ય)option; /* unused */
#endif
    version = C_SSLVERSION_કિંમત(arg);
    version_max = (long)C_SSLVERSION_MAX_કિંમત(arg);

    જો(version < CURL_SSLVERSION_DEFAULT ||
       version == CURL_SSLVERSION_SSLv2 ||
       version == CURL_SSLVERSION_SSLv3 ||
       version >= CURL_SSLVERSION_LAST ||
       version_max < CURL_SSLVERSION_MAX_NONE ||
       version_max >= CURL_SSLVERSION_MAX_LAST)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    જો(version == CURL_SSLVERSION_DEFAULT)
      version = CURL_SSLVERSION_TLSv1_2;

    sslc->version = (unsigned અક્ષર)version;
    sslc->version_max = (unsigned પૂર્ણાંક)version_max;
  }
  પાછા CURLE_OK;
}
#endif /* !USE_SSL */

#ifndef CURL_DISABLE_RTSP
સ્થિર CURLcode setopt_RTSP_REQUEST(સંરચના Curl_easy *data, long arg)
{
  /*
   * Set the RTSP request method (OPTIONS, SETUP, PLAY, etc...) Would this be
   * better if the RTSPREQ_* were moved into here?
   */
  જો((arg <= CURL_RTSPREQ_NONE) || (arg >= CURL_RTSPREQ_LAST))
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  data->set.rtspreq = (unsigned અક્ષર)arg;
  પાછા CURLE_OK;
}
#endif /* !CURL_DISABLE_RTSP */

સ્થિર CURLcode setopt_long_bool(સંરચના Curl_easy *data, CURLoption option,
                                 long arg)
{
  bool enabled = !!arg;
  પૂર્ણાંક ok = 1;
  સંરચના UserDefined *s = &data->set;
  પસંદગી(option) {
  સ્થિતિ CURLOPT_FORBID_REUSE:
    /*
     * When this transfer is done, it must not be left to be reused by a
     * subsequent transfer but shall be closed immediately.
     */
    s->reuse_forbid = enabled;
    તોડો;
  સ્થિતિ CURLOPT_FRESH_CONNECT:
    /*
     * This transfer shall not use a previously cached connection but
     * should be made with a fresh new connect!
     */
    s->reuse_fresh = enabled;
    તોડો;
  સ્થિતિ CURLOPT_VERBOSE:
    /*
     * Verbose means infof() calls that give a lot of information about
     * the connection and transfer procedures as well as internal choices.
     */
    s->verbose = enabled;
    તોડો;
  સ્થિતિ CURLOPT_HEADER:
    /*
     * Set to include the header in the general data output stream.
     */
    s->include_header = enabled;
    તોડો;
  સ્થિતિ CURLOPT_NOPROGRESS:
    /*
     * Shut off the internal supported progress meter
     */
    data->progress.hide = enabled;
    તોડો;
  સ્થિતિ CURLOPT_NOBODY:
    /*
     * Do not include the body part in the output data stream.
     */
    s->opt_no_body = enabled;
#ifndef CURL_DISABLE_HTTP
    જો(s->opt_no_body)
      /* in HTTP lingo, no body means using the HEAD request... */
      s->method = HTTPREQ_HEAD;
    નહીંતર જો(s->method == HTTPREQ_HEAD)
      s->method = HTTPREQ_GET;
#endif
    તોડો;
  સ્થિતિ CURLOPT_FAILONERROR:
    /*
     * Do not output the >=400 error code HTML-page, but instead only
     * return error.
     */
    s->http_fail_on_error = enabled;
    તોડો;
  સ્થિતિ CURLOPT_KEEP_SENDING_ON_ERROR:
    s->http_keep_sending_on_error = enabled;
    તોડો;
  સ્થિતિ CURLOPT_UPLOAD:
  સ્થિતિ CURLOPT_PUT:
    /*
     * We want to send data to the remote host. If this is HTTP, that equals
     * using the PUT request.
     */
    જો(enabled) {
      /* If this is HTTP, PUT is what's needed to "upload" */
      s->method = HTTPREQ_PUT;
      s->opt_no_body = FALSE; /* this is implied */
    }
    નહીંતર
      /* In HTTP, the opposite of upload is GET (unless NOBODY is true as
         then this can be changed to HEAD later on) */
      s->method = HTTPREQ_GET;
    તોડો;
  સ્થિતિ CURLOPT_FILETIME:
    /*
     * Try to get the file time of the remote document. The time will
     * later (possibly) become available using curl_easy_getinfo().
     */
    s->get_filetime = enabled;
    તોડો;
#ifndef CURL_DISABLE_HTTP
  સ્થિતિ CURLOPT_HTTP09_ALLOWED:
    s->http09_allowed = enabled;
    તોડો;
#ifndef CURL_DISABLE_COOKIES
  સ્થિતિ CURLOPT_COOKIESESSION:
    /*
     * Set this option to TRUE to start a new "cookie session". It will
     * prevent the forthcoming read-cookies-from-file actions to accept
     * cookies that are marked as being session cookies, as they belong to a
     * previous session.
     */
    s->cookiesession = enabled;
    તોડો;
#endif
  સ્થિતિ CURLOPT_AUTOREFERER:
    /*
     * Switch on automatic referer that gets set if curl follows locations.
     */
    s->http_auto_referer = enabled;
    તોડો;
  સ્થિતિ CURLOPT_TRANSFER_ENCODING:
    s->http_transfer_encoding = enabled;
    તોડો;
  સ્થિતિ CURLOPT_UNRESTRICTED_AUTH:
    /*
     * Send authentication (user+password) when following locations, even when
     * hostname changed.
     */
    s->allow_auth_to_other_hosts = enabled;
    તોડો;
  સ્થિતિ CURLOPT_HTTP_TRANSFER_DECODING:
    /*
     * disable libcurl transfer encoding is used
     */
    s->http_te_skip = !enabled; /* reversed */
    તોડો;
  સ્થિતિ CURLOPT_HTTP_CONTENT_DECODING:
    /*
     * raw data passed to the application when content encoding is used
     */
    s->http_ce_skip = !enabled; /* reversed */
    તોડો;
  સ્થિતિ CURLOPT_HTTPGET:
    /*
     * Set to force us do HTTP GET
     */
    જો(enabled) {
      s->method = HTTPREQ_GET;
      s->opt_no_body = FALSE; /* this is implied */
    }
    તોડો;
  સ્થિતિ CURLOPT_POST:
    /* Does this option serve a purpose anymore? Yes it does, when
       CURLOPT_POSTFIELDS is not used and the POST data is read off the
       callback! */
    જો(enabled) {
      s->method = HTTPREQ_POST;
      s->opt_no_body = FALSE; /* this is implied */
    }
    નહીંતર
      s->method = HTTPREQ_GET;
    તોડો;
#endif /* !CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_PROXY
  સ્થિતિ CURLOPT_HTTPPROXYTUNNEL:
    /*
     * Tunnel operations through the proxy instead of normal proxy use
     */
    s->tunnel_thru_httpproxy = enabled;
    તોડો;
  સ્થિતિ CURLOPT_HAPROXYPROTOCOL:
    /*
     * Set to send the HAProxy Proxy Protocol header
     */
    s->haproxyprotocol = enabled;
    તોડો;
  સ્થિતિ CURLOPT_PROXY_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for proxy.
     */
    s->proxy_ssl.verifypeer = enabled;

    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    તોડો;
  સ્થિતિ CURLOPT_PROXY_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for proxy
     */
    s->proxy_ssl.verifyhost = enabled;
    ok = 2;
    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(data, TRUE);
    તોડો;
  સ્થિતિ CURLOPT_PROXY_TRANSFER_MODE:
    /*
     * set transfer mode (;type=<a|i>) when doing FTP via an HTTP proxy
     */
    s->proxy_transfer_mode = enabled;
    તોડો;
#endif /* !CURL_DISABLE_PROXY */
#જો defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  સ્થિતિ CURLOPT_SOCKS5_GSSAPI_NEC:
    /*
     * Set flag for NEC SOCKS5 support
     */
    s->socks5_gssapi_nec = enabled;
    તોડો;
#endif
#ifdef CURL_LIST_ONLY_PROTOCOL
  સ્થિતિ CURLOPT_DIRLISTONLY:
    /*
     * An option that changes the command to one that asks for a list only, no
     * file info details. Used for FTP, POP3 and SFTP.
     */
    s->list_only = enabled;
    તોડો;
#endif
  સ્થિતિ CURLOPT_APPEND:
    /*
     * We want to upload and append to an existing file. Used for FTP and
     * SFTP.
     */
    s->remote_append = enabled;
    તોડો;
#ifndef CURL_DISABLE_FTP
  સ્થિતિ CURLOPT_FTP_USE_EPRT:
    s->ftp_use_eprt = enabled;
    તોડો;
  સ્થિતિ CURLOPT_FTP_USE_EPSV:
    s->ftp_use_epsv = enabled;
    તોડો;
  સ્થિતિ CURLOPT_FTP_USE_PRET:
    s->ftp_use_pret = enabled;
    તોડો;
  સ્થિતિ CURLOPT_FTP_SKIP_PASV_IP:
    /*
     * Enable or disable FTP_SKIP_PASV_IP, which will disable/enable the
     * bypass of the IP address in PASV responses.
     */
    s->ftp_skip_ip = enabled;
    તોડો;
  સ્થિતિ CURLOPT_WILDCARDMATCH:
    s->wildcard_enabled = enabled;
    તોડો;
#endif
  સ્થિતિ CURLOPT_CRLF:
    /*
     * Kludgy option to enable CRLF conversions. Subject for removal.
     */
    s->crlf = enabled;
    તોડો;
#ifndef CURL_DISABLE_TFTP
  સ્થિતિ CURLOPT_TFTP_NO_OPTIONS:
    /*
     * Option that prevents libcurl from sending TFTP option requests to the
     * server.
     */
    s->tftp_no_options = enabled;
    તોડો;
#endif /* !CURL_DISABLE_TFTP */
  સ્થિતિ CURLOPT_TRANSFERTEXT:
    /*
     * This option was previously named 'FTPASCII'. Renamed to work with
     * more protocols than merely FTP.
     *
     * Transfer using ASCII (instead of BINARY).
     */
    s->prefer_ascii = enabled;
    તોડો;
  સ્થિતિ CURLOPT_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying.
     */
    s->ssl.verifypeer = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    તોડો;
#ifndef CURL_DISABLE_DOH
  સ્થિતિ CURLOPT_DOH_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for DoH.
     */
    s->doh_verifypeer = enabled;
    તોડો;
  સ્થિતિ CURLOPT_DOH_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for DoH
     */
    s->doh_verifyhost = enabled;
    ok = 2;
    તોડો;
  સ્થિતિ CURLOPT_DOH_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying for DoH.
     */
    જો(!Curl_ssl_cert_status_request())
      પાછા CURLE_NOT_BUILT_IN;

    s->doh_verifystatus = enabled;
    ok = 2;
    તોડો;
#endif /* !CURL_DISABLE_DOH */
  સ્થિતિ CURLOPT_SSL_VERIFYHOST:
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
    તોડો;
  સ્થિતિ CURLOPT_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying.
     */
    જો(!Curl_ssl_cert_status_request())
      પાછા CURLE_NOT_BUILT_IN;

    s->ssl.verifystatus = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(data, FALSE);
    તોડો;
  સ્થિતિ CURLOPT_CERTINFO:
#ifdef USE_SSL
    જો(Curl_ssl_supports(data, SSLSUPP_CERTINFO))
      s->ssl.certinfo = enabled;
    નહીંતર
#endif
      પાછા CURLE_NOT_BUILT_IN;
    તોડો;
  સ્થિતિ CURLOPT_NOSIGNAL:
    /*
     * The application asks not to set any signal() or alarm() handlers,
     * even when using a timeout.
     */
    s->no_signal = enabled;
    તોડો;
  સ્થિતિ CURLOPT_TCP_NODELAY:
    /*
     * Enable or disable TCP_NODELAY, which will disable/enable the Nagle
     * algorithm
     */
    s->tcp_nodelay = enabled;
    તોડો;
  સ્થિતિ CURLOPT_IGNORE_CONTENT_LENGTH:
    s->ignorecl = enabled;
    તોડો;
  સ્થિતિ CURLOPT_SSL_SESSIONID_CACHE:
    s->ssl.cache_session = enabled;
#ifndef CURL_DISABLE_PROXY
    s->proxy_ssl.cache_session = s->ssl.cache_session;
#endif
    તોડો;
#ifdef USE_SSH
  સ્થિતિ CURLOPT_SSH_COMPRESSION:
    s->ssh_compression = enabled;
    તોડો;
#endif /* !USE_SSH */
#ifndef CURL_DISABLE_SMTP
  સ્થિતિ CURLOPT_MAIL_RCPT_ALLOWFAILS:
    /* allow RCPT TO command to fail for some recipients */
    s->mail_rcpt_allowfails = enabled;
    તોડો;
#endif /* !CURL_DISABLE_SMTP */
  સ્થિતિ CURLOPT_SASL_IR:
    /* Enable/disable SASL initial response */
    s->sasl_ir = enabled;
    તોડો;
  સ્થિતિ CURLOPT_TCP_KEEPALIVE:
    s->tcp_keepalive = enabled;
    તોડો;
  સ્થિતિ CURLOPT_TCP_FASTOPEN:
#જો defined(CONNECT_DATA_IDEMPOTENT) || defined(MSG_FASTOPEN) || \
  defined(TCP_FASTOPEN_CONNECT)
    s->tcp_fastopen = enabled;
    તોડો;
#નહીંતર
    પાછા CURLE_NOT_BUILT_IN;
#endif
  સ્થિતિ CURLOPT_SSL_ENABLE_ALPN:
    s->ssl_enable_alpn = enabled;
    તોડો;
  સ્થિતિ CURLOPT_PATH_AS_IS:
    s->path_as_is = enabled;
    તોડો;
  સ્થિતિ CURLOPT_PIPEWAIT:
    s->pipewait = enabled;
    તોડો;
  સ્થિતિ CURLOPT_SUPPRESS_CONNECT_HEADERS:
    s->suppress_connect_headers = enabled;
    તોડો;
#ifndef CURL_DISABLE_SHUFFLE_DNS
  સ્થિતિ CURLOPT_DNS_SHUFFLE_ADDRESSES:
    s->dns_shuffle_addresses = enabled;
    તોડો;
#endif
  સ્થિતિ CURLOPT_DISALLOW_USERNAME_IN_URL:
    s->disallow_username_in_url = enabled;
    તોડો;
  સ્થિતિ CURLOPT_QUICK_EXIT:
    s->quick_exit = enabled;
    તોડો;
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }
  જો((arg > ok) || (arg < 0))
    /* reserve other values for future use */
    infof(data, "boolean setopt(%d) got unsupported argument %ld,"
          " treated as %d", (પૂર્ણાંક)option, arg, enabled);

  પાછા CURLE_OK;
}

સ્થિર CURLcode કિંમત_range(long *કિંમત, long below_error, long min, long max)
{
  જો(*કિંમત < below_error)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  નહીંતર જો(*કિંમત < min)
    *કિંમત = min;
  નહીંતર જો(*કિંમત > max)
    *કિંમત = max;
  પાછા CURLE_OK;
}

સ્થિર CURLcode setopt_long_net(સંરચના Curl_easy *data, CURLoption option,
                                long arg)
{
  CURLcode result = CURLE_OK;
  સંરચના UserDefined *s = &data->set;

  પસંદગી(option) {
  સ્થિતિ CURLOPT_DNS_CACHE_TIMEOUT:
    જો(arg != -1)
      પાછા setopt_set_timeout_sec(&s->dns_cache_timeout_ms, arg);
    s->dns_cache_timeout_ms = -1;
    તોડો;
  સ્થિતિ CURLOPT_MAXCONNECTS:
    result = કિંમત_range(&arg, 0, 0, INT_MAX);
    જો(!result)
      s->maxconnects = arg ? (uint32_t)arg : DEFAULT_CONNCACHE_SIZE;
    તોડો;
  સ્થિતિ CURLOPT_SERVER_RESPONSE_TIMEOUT:
    પાછા setopt_set_timeout_sec(&s->server_response_timeout, arg);
  સ્થિતિ CURLOPT_SERVER_RESPONSE_TIMEOUT_MS:
    પાછા setopt_set_timeout_ms(&s->server_response_timeout, arg);
  સ્થિતિ CURLOPT_નીચલું_SPEED_LIMIT:
    જો(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    નહીંતર
      s->નીચલું_speed_limit = arg;
    તોડો;
  સ્થિતિ CURLOPT_નીચલું_SPEED_TIME:
    result = કિંમત_range(&arg, 0, 0, USHRT_MAX);
    જો(!result)
      s->નીચલું_speed_time = (uint16_t)arg;
    તોડો;
  સ્થિતિ CURLOPT_PORT:
    જો((arg < 0) || (arg > 65535))
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    s->use_port = (unsigned short)arg;
    તોડો;
  સ્થિતિ CURLOPT_TIMEOUT:
    પાછા setopt_set_timeout_sec(&s->timeout, arg);
  સ્થિતિ CURLOPT_TIMEOUT_MS:
    પાછા setopt_set_timeout_ms(&s->timeout, arg);
  સ્થિતિ CURLOPT_CONNECTTIMEOUT:
    પાછા setopt_set_timeout_sec(&s->connecttimeout, arg);
  સ્થિતિ CURLOPT_CONNECTTIMEOUT_MS:
    પાછા setopt_set_timeout_ms(&s->connecttimeout, arg);
#ifndef CURL_DISABLE_BINDLOCAL
  સ્થિતિ CURLOPT_LOCALPORT:
    જો((arg < 0) || (arg > 65535))
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    s->localport = curlx_sltous(arg);
    તોડો;
  સ્થિતિ CURLOPT_LOCALPORTRANGE:
    જો((arg < 0) || (arg > 65535))
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    s->localportrange = curlx_sltous(arg);
    તોડો;
#endif
  સ્થિતિ CURLOPT_BUFFERSIZE:
    result = કિંમત_range(&arg, 0, READBUFFER_MIN, READBUFFER_MAX);
    જો(!result)
      s->buffer_size = (unsigned પૂર્ણાંક)arg;
    તોડો;
  સ્થિતિ CURLOPT_UPLOAD_BUFFERSIZE:
    result = કિંમત_range(&arg, 0, UPLOADBUFFER_MIN, UPLOADBUFFER_MAX);
    જો(!result)
      s->upload_buffer_size = (unsigned પૂર્ણાંક)arg;
    તોડો;
  સ્થિતિ CURLOPT_MAXFILESIZE:
    જો(arg < 0)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    નહીંતર
      s->max_filesize = arg;
    તોડો;
  સ્થિતિ CURLOPT_IPRESOLVE:
    જો((arg < CURL_IPRESOLVE_WHATEVER) || (arg > CURL_IPRESOLVE_V6))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    નહીંતર
      s->ipver = (unsigned અક્ષર)arg;
    તોડો;
  સ્થિતિ CURLOPT_CONNECT_ONLY:
    જો(arg < 0 || arg > 2)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    નહીંતર {
      s->connect_only = !!arg;
      s->connect_only_ws = (arg == 2);
    }
    તોડો;
#ifdef USE_IPV6
  સ્થિતિ CURLOPT_ADDRESS_SCOPE:
#જો SIZEOF_LONG > 4
    જો((unsigned long)arg > UINT_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    નહીંતર
#endif
    s->scope_id = (unsigned પૂર્ણાંક)arg;
    તોડો;
#endif
  સ્થિતિ CURLOPT_TCP_KEEPIDLE:
    result = કિંમત_range(&arg, 0, 0, INT_MAX);
    જો(!result)
      s->tcp_keepidle = (પૂર્ણાંક)arg;
    તોડો;
  સ્થિતિ CURLOPT_TCP_KEEPINTVL:
    result = કિંમત_range(&arg, 0, 0, INT_MAX);
    જો(!result)
      s->tcp_keepintvl = (પૂર્ણાંક)arg;
    તોડો;
  સ્થિતિ CURLOPT_TCP_KEEPCNT:
    result = કિંમત_range(&arg, 0, 0, INT_MAX);
    જો(!result)
      s->tcp_keepcnt = (પૂર્ણાંક)arg;
    તોડો;
  સ્થિતિ CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
    પાછા setopt_set_timeout_ms(&s->happy_eyeballs_timeout, arg);
  સ્થિતિ CURLOPT_UPKEEP_INTERVAL_MS:
    પાછા setopt_set_timeout_ms(&s->upkeep_interval_ms, arg);
  સ્થિતિ CURLOPT_MAXAGE_CONN:
    પાછા setopt_set_timeout_sec(&s->conn_max_idle_ms, arg);
  સ્થિતિ CURLOPT_MAXLIFETIME_CONN:
    પાછા setopt_set_timeout_sec(&s->conn_max_age_ms, arg);
  સ્થિતિ CURLOPT_DNS_USE_GLOBAL_CACHE:
    /* deprecated */
    તોડો;
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }
  પાછા result;
}

સ્થિર CURLcode setopt_long_ssl(સંરચના Curl_easy *data, CURLoption option,
                                long arg)
{
#ifdef USE_SSL
  CURLcode result = CURLE_OK;
  સંરચના UserDefined *s = &data->set;
  પસંદગી(option) {
  સ્થિતિ CURLOPT_CA_CACHE_TIMEOUT:
    જો(Curl_ssl_supports(data, SSLSUPP_CA_CACHE)) {
      result = કિંમત_range(&arg, -1, -1, INT_MAX);
      જો(!result)
        s->ssl_ca_cache_timeout = (પૂર્ણાંક)arg;
    }
    નહીંતર
      result = CURLE_NOT_BUILT_IN;
    તોડો;
  સ્થિતિ CURLOPT_SSLVERSION:
#ifndef CURL_DISABLE_PROXY
  સ્થિતિ CURLOPT_PROXY_SSLVERSION:
#endif
    પાછા Curl_setopt_SSLVERSION(data, option, arg);
  સ્થિતિ CURLOPT_SSL_FALSESTART:
    result = CURLE_NOT_BUILT_IN;
    તોડો;
  સ્થિતિ CURLOPT_USE_SSL:
    જો((arg < CURLUSESSL_NONE) || (arg >= CURLUSESSL_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    નહીંતર
      s->use_ssl = (unsigned અક્ષર)arg;
    તોડો;
  સ્થિતિ CURLOPT_SSL_OPTIONS:
    s->ssl.ssl_options = (unsigned અક્ષર)(arg & 0xff);
    તોડો;
#ifndef CURL_DISABLE_PROXY
  સ્થિતિ CURLOPT_PROXY_SSL_OPTIONS:
    s->proxy_ssl.ssl_options = (unsigned અક્ષર)(arg & 0xff);
    તોડો;
#endif
  સ્થિતિ CURLOPT_SSL_ENABLE_NPN:
    તોડો;
  સ્થિતિ CURLOPT_SSLENGINE_DEFAULT:
#ifdef USE_OPENSSL
    જો(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      પાછા CURLE_NOT_BUILT_IN;
    data->set.engine_default = (arg == 1);
#નહીંતર
    પાછા CURLE_NOT_BUILT_IN;
#endif
    તોડો;
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }
  પાછા result;
#નહીંતર  /* USE_SSL */
  (શૂન્ય)data;
  (શૂન્ય)option;
  (શૂન્ય)arg;
  પાછા CURLE_UNKNOWN_OPTION;
#endif /* !USE_SSL */
}

#ifndef CURL_DISABLE_PROXY
સ્થિર શૂન્ય changeproxy(સંરચના Curl_easy *data)
{
  Curl_auth_digest_cleanup(&data->state.proxydigest);
  memset(&data->state.authproxy, 0, કદ(data->state.authproxy));
}

સ્થિર CURLcode setopt_long_proxy(સંરચના Curl_easy *data, CURLoption option,
                                  long arg)
{
  સંરચના UserDefined *s = &data->set;

  પસંદગી(option) {
  સ્થિતિ CURLOPT_PROXYPORT:
    જો((arg < 0) || (arg > UINT16_MAX))
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    જો(arg != s->proxyport)
      changeproxy(data);
    s->proxyport = (uint16_t)arg;
    તોડો;
  સ્થિતિ CURLOPT_PROXYAUTH:
    પાછા httpauth(data, TRUE, (unsigned long)arg);
  સ્થિતિ CURLOPT_PROXYTYPE:
    જો((arg < CURLPROXY_HTTP) || (arg > CURLPROXY_HTTPS3))
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef USE_PROXY_HTTP3
    જો(arg == CURLPROXY_HTTPS3)
      પાછા CURLE_NOT_BUILT_IN;
#endif
    s->proxytype = (unsigned અક્ષર)arg;
    તોડો;
  સ્થિતિ CURLOPT_SOCKS5_AUTH: {
    unsigned long auth = (unsigned long)arg;
    જો(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI | CURLAUTH_ONLY))
      પાછા CURLE_NOT_BUILT_IN;
    જો((auth & CURLAUTH_ONLY) && !(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI)))
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    s->socks5auth = (unsigned અક્ષર)(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI));
    s->socks5_auth_only = !!(auth & CURLAUTH_ONLY);
    તોડો;
  }
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }
  પાછા CURLE_OK;
}
#નહીંતર
સ્થિર CURLcode setopt_long_proxy(સંરચના Curl_easy *data, CURLoption option,
                                  long arg)
{
  (શૂન્ય)data;
  (શૂન્ય)option;
  (શૂન્ય)arg;
  પાછા CURLE_UNKNOWN_OPTION;
}
#endif

સ્થિર CURLcode setopt_long_http(સંરચના Curl_easy *data, CURLoption option,
                                 long arg)
{
#ifndef CURL_DISABLE_HTTP
  CURLcode result = CURLE_OK;
  સંરચના UserDefined *s = &data->set;

  પસંદગી(option) {
  સ્થિતિ CURLOPT_FOLLOWLOCATION:
    જો((unsigned long)arg > 3)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    નહીંતર
      s->http_follow_mode = (unsigned અક્ષર)arg;
    તોડો;
  સ્થિતિ CURLOPT_MAXREDIRS:
    result = કિંમત_range(&arg, -1, -1, 0x7fff);
    જો(!result)
      s->maxredirs = (short)arg;
    તોડો;
  સ્થિતિ CURLOPT_POSTREDIR:
    જો(arg < CURL_REDIR_GET_ALL)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    નહીંતર {
      s->post301 = !!(arg & CURL_REDIR_POST_301);
      s->post302 = !!(arg & CURL_REDIR_POST_302);
      s->post303 = !!(arg & CURL_REDIR_POST_303);
    }
    તોડો;
  સ્થિતિ CURLOPT_HEADEROPT:
    s->sep_headers = !!(arg & CURLHEADER_SEPARATE);
    તોડો;
  સ્થિતિ CURLOPT_HTTPAUTH:
    પાછા httpauth(data, FALSE, (unsigned long)arg);
  સ્થિતિ CURLOPT_HTTP_VERSION:
    પાછા setopt_HTTP_VERSION(data, arg);
  સ્થિતિ CURLOPT_EXPECT_100_TIMEOUT_MS:
    result = કિંમત_range(&arg, 0, 0, 0xffff);
    જો(!result)
      s->expect_100_timeout = (unsigned short)arg;
    તોડો;
  સ્થિતિ CURLOPT_STREAM_WEIGHT:
#જો defined(USE_HTTP2) || defined(USE_HTTP3)
    જો((arg >= 1) && (arg <= 256))
      s->weight = (પૂર્ણાંક)arg;
    તોડો;
#નહીંતર
    result = CURLE_NOT_BUILT_IN;
    તોડો;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  સ્થિતિ CURLOPT_HTTPSIG_ALGORITHM:
    જો(arg != CURLHTTPSIG_NONE &&
       arg != CURLHTTPSIG_ED25519 &&
       arg != CURLHTTPSIG_HMAC_SHA256)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    s->httpsig_algorithm = (uint8_t)arg;
    જો(arg)
      s->httpauth = (uint32_t)CURLAUTH_HTTPSIG;
    નહીંતર
      s->httpauth &= ~(uint32_t)CURLAUTH_HTTPSIG;
    તોડો;
#endif
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }
  પાછા result;
#નહીંતર
  (શૂન્ય)data;
  (શૂન્ય)option;
  (શૂન્ય)arg;
  પાછા CURLE_UNKNOWN_OPTION;
#endif
}

સ્થિર CURLcode setopt_long_proto(સંરચના Curl_easy *data, CURLoption option,
                                  long arg)
{
  CURLcode result = CURLE_OK;
  સંરચના UserDefined *s = &data->set;

  પસંદગી(option) {
#ifndef CURL_DISABLE_TFTP
  સ્થિતિ CURLOPT_TFTP_BLKSIZE:
    result = કિંમત_range(&arg, 0, TFTP_BLKSIZE_MIN, TFTP_BLKSIZE_MAX);
    જો(!result)
      s->tftp_blksize = (unsigned short)arg;
    તોડો;
#endif
#ifndef CURL_DISABLE_NETRC
  સ્થિતિ CURLOPT_NETRC:
    જો((arg < CURL_NETRC_IGNORED) || (arg >= CURL_NETRC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    નહીંતર
      s->use_netrc = (unsigned અક્ષર)arg;
    તોડો;
#endif
#ifndef CURL_DISABLE_FTP
  સ્થિતિ CURLOPT_FTP_FILEMETHOD:
    જો((arg < CURLFTPMETHOD_DEFAULT) || (arg >= CURLFTPMETHOD_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    નહીંતર
      s->ftp_filemethod = (unsigned અક્ષર)arg;
    તોડો;
  સ્થિતિ CURLOPT_FTP_SSL_CCC:
    જો((arg < CURLFTPSSL_CCC_NONE) || (arg >= CURLFTPSSL_CCC_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    નહીંતર
      s->ftp_ccc = (unsigned અક્ષર)arg;
    તોડો;
  સ્થિતિ CURLOPT_FTPSSLAUTH:
    જો((arg < CURLFTPAUTH_DEFAULT) || (arg >= CURLFTPAUTH_LAST))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    નહીંતર
      s->ftpsslauth = (unsigned અક્ષર)arg;
    તોડો;
  સ્થિતિ CURLOPT_ACCEPTTIMEOUT_MS:
    પાછા setopt_set_timeout_ms(&s->accepttimeout, arg);
#endif
#જો !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  સ્થિતિ CURLOPT_FTP_CREATE_MISSING_DIRS:
    જો((arg < CURLFTP_CREATE_DIR_NONE) || (arg > CURLFTP_CREATE_DIR_RETRY))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    નહીંતર
      s->ftp_create_missing_dirs = (unsigned અક્ષર)arg;
    તોડો;
  સ્થિતિ CURLOPT_NEW_FILE_PERMS:
    જો((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    નહીંતર
      s->new_file_perms = (unsigned પૂર્ણાંક)arg;
    તોડો;
#endif
#ifndef CURL_DISABLE_RTSP
  સ્થિતિ CURLOPT_RTSP_REQUEST:
    પાછા setopt_RTSP_REQUEST(data, arg);
  સ્થિતિ CURLOPT_RTSP_CLIENT_CSEQ:
    result = કિંમત_range(&arg, 0, 0, INT_MAX);
    જો(!result)
      data->state.rtsp_next_client_CSeq = (uint32_t)arg;
    તોડો;
  સ્થિતિ CURLOPT_RTSP_SERVER_CSEQ:
    result = કિંમત_range(&arg, 0, 0, INT_MAX);
    જો(!result)
      data->state.rtsp_next_server_CSeq = (uint32_t)arg;
    તોડો;
#endif
#ifdef USE_SSH
  સ્થિતિ CURLOPT_SSH_AUTH_TYPES:
    s->ssh_auth_types = (uint32_t)arg;
    તોડો;
  સ્થિતિ CURLOPT_NEW_DIRECTORY_PERMS:
    જો((arg < 0) || (arg > 0777))
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    નહીંતર
      s->new_directory_perms = (unsigned પૂર્ણાંક)arg;
    તોડો;
#endif
  સ્થિતિ CURLOPT_PROTOCOLS:
    s->allowed_protocols = (curl_prot_t)arg;
    તોડો;
  સ્થિતિ CURLOPT_REDIR_PROTOCOLS:
    s->redir_protocols = (curl_prot_t)arg;
    તોડો;
#ifndef CURL_DISABLE_WEBSOCKETS
  સ્થિતિ CURLOPT_WS_OPTIONS:
    s->ws_raw_mode = !!(arg & CURLWS_RAW_MODE);
    s->ws_no_auto_pong = !!(arg & CURLWS_NOAUTOPONG);
    તોડો;
#endif
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }
  પાછા result;
}

સ્થિર CURLcode setopt_long_misc(સંરચના Curl_easy *data, CURLoption option,
                                 long arg)
{
  સંરચના UserDefined *s = &data->set;

  પસંદગી(option) {
  સ્થિતિ CURLOPT_TIMECONDITION:
    જો((arg < CURL_TIMECOND_NONE) || (arg >= CURL_TIMECOND_LAST))
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    s->timecondition = (unsigned અક્ષર)arg;
    તોડો;
  સ્થિતિ CURLOPT_TIMEVALUE:
    s->timevalue = (time_t)arg;
    તોડો;
  સ્થિતિ CURLOPT_POSTFIELDSIZE:
    જો(arg < -1)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    જો(s->postfieldsize < arg && s->str_copypostfields) {
      curlx_safefree(s->str_copypostfields);
      s->postfields = ખાલી;
    }
    s->postfieldsize = arg;
    તોડો;
  સ્થિતિ CURLOPT_INFILESIZE:
    જો(arg < -1)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = arg;
    તોડો;
  સ્થિતિ CURLOPT_RESUME_FROM:
    જો(arg < -1)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = arg;
    તોડો;
  સ્થિતિ CURLOPT_UPLOAD_FLAGS:
    s->upload_flags = (unsigned અક્ષર)arg;
    તોડો;
#ifndef CURL_DISABLE_MIME
  સ્થિતિ CURLOPT_MIME_OPTIONS:
    s->mime_formescape = !!(arg & CURLMIMEOPT_FORMESCAPE);
    તોડો;
#endif
#ifndef CURL_DISABLE_HSTS
  સ્થિતિ CURLOPT_HSTS_CTRL:
    જો(arg & CURLHSTS_ENABLE) {
      જો(!data->hsts) {
        data->hsts = Curl_hsts_init();
        જો(!data->hsts)
          પાછા CURLE_OUT_OF_MEMORY;
      }
    }
    નહીંતર જો(!data->share || !data->share->hsts) {
      /* throw away the HSTS cache unless shared */
      Curl_hsts_cleanup(&data->hsts);
      /* flush all the entries */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = ખાલી;
    }
    નહીંતર
      /* detach from shared HSTS cache without freeing it */
      data->hsts = ખાલી;
    તોડો;
#endif
#ifndef CURL_DISABLE_ALTSVC
  સ્થિતિ CURLOPT_ALTSVC_CTRL:
    પાછા Curl_altsvc_ctrl(data, arg);
#endif
#જો defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  સ્થિતિ CURLOPT_GSSAPI_DELEGATION:
    s->gssapi_delegation = (unsigned અક્ષર)arg &
      (CURLGSSAPI_DELEGATION_POLICY_FLAG | CURLGSSAPI_DELEGATION_FLAG);
    તોડો;
#endif
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode setopt_long(સંરચના Curl_easy *data, CURLoption option,
                            long arg)
{
  પ્રકારવ્યાખ્યા CURLcode (*setoptfunc)(સંરચના Curl_easy *data,
                                 CURLoption option, long arg);
  સ્થિર અચલ setoptfunc setopt_call[] = {
    setopt_long_bool,
    setopt_long_net,
    setopt_long_http,
    setopt_long_proxy,
    setopt_long_ssl,
    setopt_long_proto,
    setopt_long_misc
  };
  size_t i;

  માટે(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, arg);
    જો(result != CURLE_UNKNOWN_OPTION)
      પાછા result;
  }
  પાછા CURLE_UNKNOWN_OPTION;
}

સ્થિર CURLcode setopt_slist(સંરચના Curl_easy *data, CURLoption option,
                             સંરચના curl_slist *slist)
{
  CURLcode result = CURLE_OK;
  સંરચના UserDefined *s = &data->set;
  પસંદગી(option) {
#ifndef CURL_DISABLE_PROXY
  સ્થિતિ CURLOPT_PROXYHEADER:
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
    તોડો;
#endif
#ifndef CURL_DISABLE_HTTP
  સ્થિતિ CURLOPT_HTTP200ALIASES:
    /*
     * Set a list of aliases for HTTP 200 in response header
     */
    s->http200aliases = slist;
    તોડો;
#endif
#જો !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  સ્થિતિ CURLOPT_POSTQUOTE:
    /*
     * List of RAW FTP commands to use after a transfer
     */
    s->postquote = slist;
    તોડો;
  સ્થિતિ CURLOPT_PREQUOTE:
    /*
     * List of RAW FTP commands to use prior to RETR (Wesley Laxton)
     */
    s->prequote = slist;
    તોડો;
  સ્થિતિ CURLOPT_QUOTE:
    /*
     * List of RAW FTP commands to use before a transfer
     */
    s->quote = slist;
    તોડો;
#endif
  સ્થિતિ CURLOPT_RESOLVE:
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
    તોડો;
#જો !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MIME)
  સ્થિતિ CURLOPT_HTTPHEADER:
    /*
     * Set a list with HTTP headers to use (or replace internals with)
     */
    s->headers = slist;
    તોડો;
#endif
#ifndef CURL_DISABLE_TELNET
  સ્થિતિ CURLOPT_TELNETOPTIONS:
    /*
     * Set a linked list of telnet options
     */
    s->telnet_options = slist;
    તોડો;
#endif
#ifndef CURL_DISABLE_SMTP
  સ્થિતિ CURLOPT_MAIL_RCPT:
    /* Set the list of mail recipients */
    s->mail_rcpt = slist;
    તોડો;
#endif
  સ્થિતિ CURLOPT_CONNECT_TO:
    s->connect_to = slist;
    તોડો;
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }
  પાછા result;
}

#જો !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
સ્થિર CURLcode setopt_mimepost(સંરચના Curl_easy *data, curl_mime *mimep)
{
  /*
   * Set to make us do MIME POST
   */
  CURLcode result;
  સંરચના UserDefined *s = &data->set;
  જો(!s->mimepostp) {
    s->mimepostp = curlx_malloc(કદ(*s->mimepostp));
    જો(!s->mimepostp)
      પાછા CURLE_OUT_OF_MEMORY;
    Curl_mime_initpart(s->mimepostp);
  }

  result = Curl_mime_set_subparts(s->mimepostp, mimep, FALSE);
  જો(!result) {
    s->method = HTTPREQ_POST_MIME;
    s->opt_no_body = FALSE; /* this is implied */
#ifndef CURL_DISABLE_FORM_API
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = ખાલી;
#endif
  }
  પાછા result;
}
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */

સ્થિર CURLcode setopt_share(સંરચના Curl_easy *data, સંરચના Curl_share *set)
{
  CURLcode result;

  જો(data->conn) {
    /* As this handle already has a connection attached, changing share now
       would be complicated and error-prone */
    infof(data, "Cannot change share object while in use");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
  }
  નહીંતર {
    /* disconnect from old share, if any and possible */
    result = Curl_share_easy_unlink(data);
    જો(!result && GOOD_SHARE_HANDLE(set))
      /* use new share if it set */
      result = Curl_share_easy_link(data, set);
  }
  પાછા result;
}

/* assorted pointer type arguments */
સ્થિર CURLcode setopt_pointers(સંરચના Curl_easy *data, CURLoption option,
                                va_list param)
{
  CURLcode result = CURLE_OK;
  સંરચના UserDefined *s = &data->set;
  પસંદગી(option) {
  સ્થિતિ CURLOPT_CURLU:
    /*
     * pass CURLU to set URL
     */
    Curl_bufref_free(&data->state.url);
    CURL_EASY_STR_CLEAR(data, STRING_SET_URL);
    s->uh = va_arg(param, CURLU *);
    તોડો;
#ifndef CURL_DISABLE_HTTP
#ifndef CURL_DISABLE_FORM_API
  સ્થિતિ CURLOPT_HTTPPOST:
    /*
     * Set to make us do HTTP POST. Legacy API-style.
     */
    s->httppost = va_arg(param, સંરચના curl_httppost *);
    s->method = HTTPREQ_POST_FORM;
    s->opt_no_body = FALSE; /* this is implied */
    Curl_mime_cleanpart(data->state.formp);
    curlx_safefree(data->state.formp);
    data->state.mimepost = ખાલી;
    તોડો;
#endif /* !CURL_DISABLE_FORM_API */
#endif /* !CURL_DISABLE_HTTP */
#જો !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ifndef CURL_DISABLE_MIME
  સ્થિતિ CURLOPT_MIMEPOST:
    result = setopt_mimepost(data, va_arg(param, curl_mime *));
    તોડો;
#endif /* !CURL_DISABLE_MIME */
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */
  સ્થિતિ CURLOPT_STDERR:
    /*
     * Set to a FILE * that should receive all error writes. This
     * defaults to stderr for normal operations.
     */
    s->err = va_arg(param, FILE *);
    જો(!s->err)
      s->err = stderr;
    તોડો;
  સ્થિતિ CURLOPT_SHARE:
    પાછા setopt_share(data, va_arg(param, સંરચના Curl_share *));

  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }
  પાછા result;
}

#ifndef CURL_DISABLE_COOKIES
સ્થિર CURLcode cookielist(સંરચના Curl_easy *data, અચલ અક્ષર *ptr)
{
  CURLcode result = CURLE_OK;
  જો(!ptr)
    પાછા CURLE_OK;

  જો(curl_strequal(ptr, "ALL")) {
    /* clear all cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearall(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  નહીંતર જો(curl_strequal(ptr, "SESS")) {
    /* clear session cookies */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    Curl_cookie_clearsess(data->cookies);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  નહીંતર જો(curl_strequal(ptr, "FLUSH")) {
    /* flush cookies to file, takes care of the locking */
    Curl_flush_cookies(data, FALSE);
  }
  નહીંતર જો(curl_strequal(ptr, "RELOAD")) {
    /* reload cookies from file */
    પાછા Curl_cookie_loadfiles(data, COOKIE_NOPSL |
                                 (data->set.cookiesession ?
                                  COOKIE_NOSESSION : 0));
  }
  નહીંતર {
    જો(!data->cookies) {
      /* if cookie engine was not running, activate it */
      data->cookies = Curl_cookie_init();
      જો(!data->cookies)
        પાછા CURLE_OUT_OF_MEMORY;
      data->state.cookie_engine = TRUE;
    }

    /* general protection against mistakes and abuse */
    જો(strlen(ptr) > CURL_MAX_આગત_LENGTH)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;

    /* Adding these cookies without the PSL check, because the PSL is not
       initialized until *perform() time, and this might be called before
       that */
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    જો(checkprefix("Set-Cookie:", ptr))
      /* HTTP Header format line */
      result = Curl_cookie_add(data, data->cookies, ptr + 11,
                               ખાલી, ખાલી,
                               COOKIE_HTTPHEADER | COOKIE_SECURE |
                               COOKIE_NOPSL);
    નહીંતર
      /* Netscape format line */
      result = Curl_cookie_add(data, data->cookies, ptr, ખાલી,
                               ખાલી, COOKIE_SECURE | COOKIE_NOPSL);
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  પાછા result;
}

સ્થિર CURLcode cookiefile(સંરચના Curl_easy *data, અચલ અક્ષર *ptr)
{
  /*
   * Set cookie file to read and parse. Can be used multiple times.
   */
  જો(ptr) {
    સંરચના curl_slist *cl;
    /* general protection against mistakes and abuse */
    જો(strlen(ptr) > CURL_MAX_આગત_LENGTH)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    /* append the cookie filename to the list of filenames, and deal with
       them later */
    cl = curl_slist_append(data->state.cookielist, ptr);
    જો(!cl) {
      curl_slist_free_all(data->state.cookielist);
      data->state.cookielist = ખાલી;
      પાછા CURLE_OUT_OF_MEMORY;
    }
    data->state.cookielist = cl; /* store the list for later use */
  }
  નહીંતર {
    /* clear the list of cookie files */
    curl_slist_free_all(data->state.cookielist);
    data->state.cookielist = ખાલી;

    જો(!data->share || !data->share->cookies) {
      /* throw away all existing cookies if this is not a shared cookie
         container */
      Curl_cookie_clearall(data->cookies);
      Curl_cookie_cleanup(data->cookies);
    }
    /* disable the cookie engine */
    data->cookies = ખાલી;
  }
  પાછા CURLE_OK;
}
#endif

#ifndef CURL_DISABLE_PROXY
સ્થિર CURLcode setproxy(સંરચના Curl_easy *data, અચલ અક્ષર *proxy)
{
  અચલ અક્ષર *str = CURL_EASY_STR(data, STRING_PROXY);
  જો(str && proxy &&
     /* there was one set, is this a new one? */
     !strcmp(str, proxy))
    પાછા CURLE_OK; /* same one as before */

  changeproxy(data);
  પાછા Curl_setstropt(data, STRING_PROXY, proxy);
}

સ્થિર CURLcode setopt_cptr_proxy(સંરચના Curl_easy *data, CURLoption option,
                                  અક્ષર *ptr)
{
  CURLcode result = CURLE_OK;
  સંરચના UserDefined *s = &data->set;
  પસંદગી(option) {
  સ્થિતિ CURLOPT_PROXYUSERPWD: {
    /*
     * user:password needed to use the proxy
     */
    અક્ષર *u = ખાલી;
    અક્ષર *p = ખાલી;
    result = setstropt_userpwd(ptr, &u, &p);

    /* URL decode the components */
    જો(!result) {
      અક્ષર *str = ખાલી;
      CURL_EASY_STR_CLEAR(data, STRING_PROXYUSERNAME);
      CURL_EASY_STR_CLEAR(data, STRING_PROXYPASSWORD);
      જો(u) {
        result = Curl_urldecode(u, 0, &str, ખાલી, REJECT_ZERO);
        જો(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYUSERNAME, str);
      }
      જો(!result && p) {
        str = ખાલી;
        result = Curl_urldecode(p, 0, &str, ખાલી, REJECT_ZERO);
        જો(!result)
          result = Curl_u8_strset_setn(&s->strings, STRING_PROXYPASSWORD, str);
      }
    }
    curlx_free(u);
    curlx_strzero(p);
    curlx_free(p);
    તોડો;
  }
  સ્થિતિ CURLOPT_PROXYUSERNAME:
    /*
     * authentication username to use in the operation
     */
    પાછા Curl_setstropt(data, STRING_PROXYUSERNAME, ptr);

  સ્થિતિ CURLOPT_PROXYPASSWORD:
    /*
     * authentication password to use in the operation
     */
    પાછા Curl_setstropt(data, STRING_PROXYPASSWORD, ptr);

  સ્થિતિ CURLOPT_NOPROXY:
    /*
     * proxy exception list
     */
    પાછા Curl_setstropt(data, STRING_NOPROXY, ptr);
  સ્થિતિ CURLOPT_PROXY_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use for proxy
     */
    પાછા Curl_setstropt(data, STRING_CERT_PROXY, ptr);
  સ્થિતિ CURLOPT_PROXY_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use for proxy
     */
    પાછા Curl_setstropt(data, STRING_CERT_TYPE_PROXY, ptr);
  સ્થિતિ CURLOPT_PROXY_SSLKEY:
    /*
     * String that holds filename of the SSL key to use for proxy
     */
    પાછા Curl_setstropt(data, STRING_ચાવી_PROXY, ptr);
  સ્થિતિ CURLOPT_PROXY_KEYPASSWD:
    /*
     * String that holds the SSL private key password for proxy.
     */
    પાછા Curl_setstropt(data, STRING_ચાવી_PASSWD_PROXY, ptr);
  સ્થિતિ CURLOPT_PROXY_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use for proxy
     */
    પાછા Curl_setstropt(data, STRING_ચાવી_TYPE_PROXY, ptr);
  સ્થિતિ CURLOPT_PROXY_SSL_CIPHER_LIST:
    જો(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST)) {
      /* set a list of cipher we want to use in the SSL connection for proxy */
      પાછા Curl_setstropt(data, STRING_SSL_CIPHER_LIST_PROXY, ptr);
    }
    નહીંતર
      પાછા CURLE_NOT_BUILT_IN;
  સ્થિતિ CURLOPT_PROXY_TLS13_CIPHERS:
    જો(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites for proxy */
      પાછા Curl_setstropt(data, STRING_SSL_CIPHER13_LIST_PROXY, ptr);
    નહીંતર
      પાછા CURLE_NOT_BUILT_IN;
  સ્થિતિ CURLOPT_PROXY:
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
    પાછા setproxy(data, ptr);
  સ્થિતિ CURLOPT_PRE_PROXY:
    /*
     * Set proxy server:port to use as SOCKS proxy.
     *
     * If the proxy is set to "" or NULL we explicitly say that we do not want
     * to use the socks proxy.
     */
    પાછા Curl_setstropt(data, STRING_PRE_PROXY, ptr);
  સ્થિતિ CURLOPT_SOCKS5_GSSAPI_SERVICE:
  સ્થિતિ CURLOPT_PROXY_SERVICE_NAME:
    /*
     * Set proxy authentication service name for Kerberos 5 and SPNEGO
     */
    પાછા Curl_setstropt(data, STRING_PROXY_SERVICE_NAME, ptr);
  સ્થિતિ CURLOPT_PROXY_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
#ifdef USE_SSL
    જો(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      પાછા Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY_PROXY, ptr);
#endif
    પાછા CURLE_NOT_BUILT_IN;

  સ્થિતિ CURLOPT_HAPROXY_CLIENT_IP:
    /*
     * Set the client IP to send through HAProxy PROXY protocol
     */
    result = Curl_setstropt(data, STRING_HAPROXY_CLIENT_IP, ptr);

    /* enable the HAProxy protocol if an IP is provided */
    s->haproxyprotocol = !!CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
    તોડો;
  સ્થિતિ CURLOPT_PROXY_CAINFO:
    /*
     * Set CA info SSL connection for proxy. Specify filename of the
     * CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE_PROXY, ptr);
    જો(!result)
      s->proxy_ssl.custom_cafile = TRUE;
    પાછા result;
  સ્થિતિ CURLOPT_PROXY_CRLFILE:
    /*
     * Set CRL file info for SSL connection for proxy. Specify filename of the
     * CRL to check certificates revocation
     */
    જો(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      પાછા Curl_setstropt(data, STRING_SSL_CRLFILE_PROXY, ptr);
    પાછા CURLE_NOT_BUILT_IN;
  સ્થિતિ CURLOPT_PROXY_ISSUERCERT:
    /*
     * Set Issuer certificate file to check certificates issuer
     */
    જો(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      પાછા Curl_setstropt(data, STRING_SSL_ISSUERCERT_PROXY, ptr);
    પાછા CURLE_NOT_BUILT_IN;
  સ્થિતિ CURLOPT_PROXY_CAPATH:
    /*
     * Set CA path info for SSL connection proxy. Specify directory name of the
     * CA certificates which have been prepared using openssl c_rehash utility.
     */
#ifdef USE_SSL
    જો(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH_PROXY, ptr);
      જો(!result)
        s->proxy_ssl.custom_capath = TRUE;
      પાછા result;
    }
#endif
    પાછા CURLE_NOT_BUILT_IN;
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }
  પાછા result;
}
#endif

#જો !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
/*
 * A string with POST data. Makes curl HTTP POST. Even if it is NULL. If
 * needed, CURLOPT_POSTFIELDSIZE must have been set prior to
 * CURLOPT_COPYPOSTFIELDS and not altered later.
 */
સ્થિર CURLcode setopt_copypostfields(અચલ અક્ષર *ptr, સંરચના UserDefined *s)
{
  CURLcode result = CURLE_OK;
  જો(s->postfieldsize < -1)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  જો(!ptr || s->postfieldsize == -1) {
    જો(ptr && (strlen(ptr) > CURL_MAX_આગત_LENGTH))
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    curlx_safefree(s->str_copypostfields);
    જો(ptr) {
      s->str_copypostfields = curlx_strdup(ptr);
      જો(!s->str_copypostfields)
        પાછા CURLE_OUT_OF_MEMORY;
    }
  }
  નહીંતર {
    size_t pflen = curlx_sotouz_range(s->postfieldsize, 0, SIZE_MAX);
    જો(pflen == SIZE_MAX)
      પાછા CURLE_OUT_OF_MEMORY;
    નહીંતર {
      /* Allocate even when size == 0. This satisfies the need of possible
         later address compare to detect the COPYPOSTFIELDS mode, and to mark
         that postfields is used rather than read function or form data. */
      અક્ષર *p = curlx_memdup0(ptr, pflen);
      જો(!p)
        પાછા CURLE_OUT_OF_MEMORY;
      નહીંતર {
        curlx_free(s->str_copypostfields);
        s->str_copypostfields = p;
      }
    }
  }

  s->postfields = s->str_copypostfields;
  s->method = HTTPREQ_POST;
  પાછા result;
}
#endif

#ifdef USE_ECH
સ્થિર CURLcode setopt_ech(સંરચના Curl_easy *data, અચલ અક્ષર *ptr)
{
  સંરચના UserDefined *s = &data->set;
  CURLcode result = CURLE_OK;

  જો(!ptr || !strcmp(ptr, "false"))
    s->ssl.ech = CURLECH_DISABLE;
  નહીંતર {
    size_t plen = strlen(ptr);
    જો(plen > CURL_MAX_આગત_LENGTH)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    નહીંતર {
      જો(!strcmp(ptr, "grease"))
        s->ssl.ech = CURLECH_GREASE;
      નહીંતર જો(!strcmp(ptr, "true"))
        s->ssl.ech = CURLECH_ENABLE;
      નહીંતર જો(!strcmp(ptr, "hard"))
        s->ssl.ech = CURLECH_HARD;
      નહીંતર જો(plen > 4 && !strncmp(ptr, "ecl:", 4)) {
        જો(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_CONFIG, ptr + 4);
      }
      નહીંતર જો(plen > 3 && !strncmp(ptr, "pn:", 3)) {
        જો(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        result = Curl_setstropt(data, STRING_ECH_PUBLIC, ptr + 3);
      }
      નહીંતર
        result = CURLE_BAD_FUNCTION_ARGUMENT;
    }
  }
  પાછા result;
}
#નહીંતર
#define setopt_ech(x, y) CURLE_NOT_BUILT_IN
#endif

#જો defined(USE_SSL) || defined(USE_SSH)
/* One of the options is used for both TLS and SSH */
સ્થિર CURLcode setopt_cptr_ssl(સંરચના Curl_easy *data, CURLoption option,
                                અક્ષર *ptr)
{
  CURLcode result = CURLE_OK;

  પસંદગી(option) {
  સ્થિતિ CURLOPT_KEYPASSWD:
    /*
     * String that holds the SSL or SSH private key password.
     */
    result = Curl_setstropt(data, STRING_ચાવી_PASSWD, ptr);
    તોડો;
#ifdef USE_SSL
  સ્થિતિ CURLOPT_CAINFO:
    /*
     * Set CA info for SSL connection. Specify filename of the CA certificate
     */
    result = Curl_setstropt(data, STRING_SSL_CAFILE, ptr);
    જો(!result)
      data->set.ssl.custom_cafile = TRUE;
    પાછા result;
  સ્થિતિ CURLOPT_CAPATH:
    /*
     * Set CA path info for SSL connection. Specify directory name of the CA
     * certificates which have been prepared using openssl c_rehash utility.
     */
    જો(Curl_ssl_supports(data, SSLSUPP_CA_PATH)) {
      /* This does not work on Windows. */
      result = Curl_setstropt(data, STRING_SSL_CAPATH, ptr);
      જો(!result)
        data->set.ssl.custom_capath = TRUE;
      પાછા result;
    }
    પાછા CURLE_NOT_BUILT_IN;
  સ્થિતિ CURLOPT_CRLFILE:
    /*
     * Set CRL file info for SSL connection. Specify filename of the CRL
     * to check certificates revocation
     */
    જો(Curl_ssl_supports(data, SSLSUPP_CRLFILE))
      પાછા Curl_setstropt(data, STRING_SSL_CRLFILE, ptr);
    પાછા CURLE_NOT_BUILT_IN;
  સ્થિતિ CURLOPT_SSL_CIPHER_LIST:
    જો(Curl_ssl_supports(data, SSLSUPP_CIPHER_LIST))
      /* set a list of cipher we want to use in the SSL connection */
      પાછા Curl_setstropt(data, STRING_SSL_CIPHER_LIST, ptr);
    નહીંતર
      પાછા CURLE_NOT_BUILT_IN;
  સ્થિતિ CURLOPT_TLS13_CIPHERS:
    જો(Curl_ssl_supports(data, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites */
      પાછા Curl_setstropt(data, STRING_SSL_CIPHER13_LIST, ptr);
    નહીંતર
      પાછા CURLE_NOT_BUILT_IN;
  સ્થિતિ CURLOPT_RANDOM_FILE:
    તોડો;
  સ્થિતિ CURLOPT_EGDSOCKET:
    તોડો;
  સ્થિતિ CURLOPT_SSL_CTX_DATA:
    /*
     * Set an SSL_CTX callback parameter pointer
     */
    જો(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      data->set.ssl_fsslctxp = ptr;
      તોડો;
    }
    નહીંતર
      પાછા CURLE_NOT_BUILT_IN;
  સ્થિતિ CURLOPT_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use
     */
    પાછા Curl_setstropt(data, STRING_CERT, ptr);
  સ્થિતિ CURLOPT_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use
     */
    પાછા Curl_setstropt(data, STRING_CERT_TYPE, ptr);
  સ્થિતિ CURLOPT_SSLKEY:
    /*
     * String that holds filename of the SSL key to use
     */
    પાછા Curl_setstropt(data, STRING_ચાવી, ptr);
  સ્થિતિ CURLOPT_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use
     */
    પાછા Curl_setstropt(data, STRING_ચાવી_TYPE, ptr);
  સ્થિતિ CURLOPT_SSLENGINE:
    /*
     * String that holds the SSL crypto engine.
     */
    જો(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      પાછા CURLE_NOT_BUILT_IN;
    result = Curl_setstropt(data, STRING_SSL_ENGINE, ptr);
    તોડો;
  સ્થિતિ CURLOPT_ISSUERCERT:
    /*
     * Set Issuer certificate file
     * to check certificates issuer
     */
    જો(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT))
      પાછા Curl_setstropt(data, STRING_SSL_ISSUERCERT, ptr);
    પાછા CURLE_NOT_BUILT_IN;
  સ્થિતિ CURLOPT_SSL_EC_CURVES:
    /*
     * Set accepted curves in SSL connection setup.
     * Specify colon-delimited list of curve algorithm names.
     */
    જો(Curl_ssl_supports(data, SSLSUPP_SSL_EC_CURVES))
      પાછા Curl_setstropt(data, STRING_SSL_EC_CURVES, ptr);
    પાછા CURLE_NOT_BUILT_IN;
  સ્થિતિ CURLOPT_SSL_SIGNATURE_ALGORITHMS:
    /*
     * Set accepted signature algorithms.
     * Specify colon-delimited list of signature scheme names.
     */
    જો(Curl_ssl_supports(data, SSLSUPP_SIGNATURE_ALGORITHMS))
      પાછા Curl_setstropt(data, STRING_SSL_SIGNATURE_ALGORITHMS, ptr);
    પાછા CURLE_NOT_BUILT_IN;
  સ્થિતિ CURLOPT_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
    જો(Curl_ssl_supports(data, SSLSUPP_PINNEDPUBKEY))
      પાછા Curl_setstropt(data, STRING_SSL_PINNEDPUBLICKEY, ptr);
    પાછા CURLE_NOT_BUILT_IN;
  સ્થિતિ CURLOPT_ECH:
    પાછા setopt_ech(data, ptr);
#endif
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }
  પાછા result;
}
#endif

#જો !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
સ્થિર CURLcode setopt_cptr_http_mqtt(સંરચના Curl_easy *data,
                                      CURLoption option, અક્ષર *ptr)
{
  CURLcode result = CURLE_OK;
  સંરચના UserDefined *s = &data->set;

  પસંદગી(option) {
  સ્થિતિ CURLOPT_COPYPOSTFIELDS:
    પાછા setopt_copypostfields(ptr, s);

  સ્થિતિ CURLOPT_POSTFIELDS:
    /*
     * Like above, but use static data instead of copying it.
     */
    s->postfields = ptr;
    /* Release old copied data. */
    curlx_safefree(s->str_copypostfields);
    s->method = HTTPREQ_POST;
    તોડો;

#ifndef CURL_DISABLE_HTTP
  સ્થિતિ CURLOPT_TRAILERDATA:
    s->trailer_data = ptr;
    તોડો;
  સ્થિતિ CURLOPT_ACCEPT_ENCODING:
    /*
     * String to use at the value of Accept-Encoding header.
     *
     * If the encoding is set to "" we use an Accept-Encoding header that
     * encompasses all the encodings we support.
     * If the encoding is set to NULL we do not send an Accept-Encoding header
     * and ignore any received Content-Encoding header.
     *
     */
    જો(ptr && !*ptr) {
      ptr = Curl_get_content_encodings();
      જો(ptr)
        result = CURL_EASY_STR_SETN(data, STRING_ENCODING, ptr);
      નહીંતર
        result = CURLE_OUT_OF_MEMORY;
      પાછા result;
    }
    પાછા Curl_setstropt(data, STRING_ENCODING, ptr);

#ifndef CURL_DISABLE_AWS
  સ્થિતિ CURLOPT_AWS_SIGV4:
    /*
     * String that is merged to some authentication
     * parameters are used by the algorithm.
     */
    result = Curl_setstropt(data, STRING_AWS_SIGV4, ptr);
    /*
     * Basic has been set by default; it needs to be unset here.
     */
    જો(CURL_EASY_STR(data, STRING_AWS_SIGV4))
      s->httpauth = CURLAUTH_AWS_SIGV4;
    નહીંતર
      s->httpauth &= ~(uint32_t)CURLAUTH_AWS_SIGV4;
    તોડો;
#endif
#ifndef CURL_DISABLE_HTTPSIG
  સ્થિતિ CURLOPT_HTTPSIG_ચાવી:
    result = Curl_setstropt(data, STRING_HTTPSIG_ચાવી, ptr);
    તોડો;
  સ્થિતિ CURLOPT_HTTPSIG_KEYID:
    result = Curl_setstropt(data, STRING_HTTPSIG_KEYID, ptr);
    તોડો;
  સ્થિતિ CURLOPT_HTTPSIG_HEADERS:
    result = Curl_setstropt(data, STRING_HTTPSIG_HEADERS, ptr);
    તોડો;
#endif
  સ્થિતિ CURLOPT_REFERER: {
    /*
     * String to set in the HTTP Referer: field.
     */
    સંરચના bufref *oldref = &data->state.referer;
    /* free the old after the storing the new in case the input is actually
       pointing back to this */
    result = Curl_setstropt(data, STRING_SET_REFERER, ptr);
    Curl_bufref_free(oldref);
    તોડો;
  }

  સ્થિતિ CURLOPT_USERAGENT:
    /*
     * String to use in the HTTP User-Agent field
     */
    પાછા Curl_setstropt(data, STRING_USERAGENT, ptr);

#ifndef CURL_DISABLE_COOKIES
  સ્થિતિ CURLOPT_COOKIE:
    /*
     * Cookie string to send to the remote server in the request.
     */
    પાછા Curl_setstropt(data, STRING_COOKIE, ptr);

  સ્થિતિ CURLOPT_COOKIEFILE:
    પાછા cookiefile(data, ptr);

  સ્થિતિ CURLOPT_COOKIEJAR:
    /*
     * Set cookie filename to dump all cookies to when we are done.
     */
    result = Curl_setstropt(data, STRING_COOKIEJAR, ptr);
    જો(!result) {
      /*
       * Activate the cookie parser. This may or may not already
       * have been made.
       */
      જો(!data->cookies)
        data->cookies = Curl_cookie_init();
      જો(!data->cookies)
        result = CURLE_OUT_OF_MEMORY;
      નહીંતર
        data->state.cookie_engine = TRUE;
    }
    તોડો;

  સ્થિતિ CURLOPT_COOKIELIST:
    પાછા cookielist(data, ptr);
#endif /* !CURL_DISABLE_COOKIES */

#endif /* !CURL_DISABLE_HTTP */
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }
  પાછા result;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_MQTT */

#ifdef USE_SSH
સ્થિર CURLcode setopt_cptr_ssh(સંરચના Curl_easy *data, CURLoption option,
                                અક્ષર *ptr)
{
  સંરચના UserDefined *s = &data->set;
  પસંદગી(option) {
  સ્થિતિ CURLOPT_SSH_PUBLIC_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa.pub file
     */
    પાછા Curl_setstropt(data, STRING_SSH_PUBLIC_ચાવી, ptr);
  સ્થિતિ CURLOPT_SSH_PRIVATE_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa file
     */
    પાછા Curl_setstropt(data, STRING_SSH_PRIVATE_ચાવી, ptr);
  સ્થિતિ CURLOPT_SSH_KEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_keyfunc_userp = ptr;
    તોડો;
  સ્થિતિ CURLOPT_SSH_HOST_PUBLIC_ચાવી_MD5:
    /*
     * Option to allow for the MD5 of the host public key to be checked
     * for validation purposes.
     */
    પાછા Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_ચાવી_MD5, ptr);
  સ્થિતિ CURLOPT_SSH_HOST_PUBLIC_ચાવી_SHA256:
    /*
     * Option to allow for the SHA256 of the host public key to be checked
     * for validation purposes.
     */
    પાછા Curl_setstropt(data, STRING_SSH_HOST_PUBLIC_ચાવી_SHA256, ptr);
  સ્થિતિ CURLOPT_SSH_KNOWNHOSTS:
    /*
     * Store the filename to read known hosts from.
     */
    પાછા Curl_setstropt(data, STRING_SSH_KNOWNHOSTS, ptr);
#ifdef USE_LIBSSH2
  સ્થિતિ CURLOPT_SSH_HOSTKEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_hostkeyfunc_userp = ptr;
    તોડો;
#endif /* USE_LIBSSH2 */
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }
  પાછા CURLE_OK;
}
#endif /* USE_SSH */

#ifndef CURL_DISABLE_FTP
સ્થિર CURLcode setopt_cptr_ftp(સંરચના Curl_easy *data, CURLoption option,
                                અક્ષર *ptr)
{
  CURLcode result = CURLE_OK;
  સંરચના UserDefined *s = &data->set;
  પસંદગી(option) {
  સ્થિતિ CURLOPT_FTPPORT:
    /*
     * Use FTP PORT, this also specifies which IP address to use
     */
    result = Curl_setstropt(data, STRING_FTPPORT, ptr);
    s->ftp_use_port = !!CURL_EASY_STR(data, STRING_FTPPORT);
    તોડો;

  સ્થિતિ CURLOPT_FTP_ACCOUNT:
    પાછા Curl_setstropt(data, STRING_FTP_ACCOUNT, ptr);

  સ્થિતિ CURLOPT_FTP_ALTERNATIVE_TO_USER:
    પાછા Curl_setstropt(data, STRING_FTP_ALTERNATIVE_TO_USER, ptr);

  સ્થિતિ CURLOPT_KRBLEVEL:
    પાછા CURLE_NOT_BUILT_IN; /* removed in 8.17.0 */
  સ્થિતિ CURLOPT_CHUNK_DATA:
    s->wildcardptr = ptr;
    તોડો;
  સ્થિતિ CURLOPT_FNMATCH_DATA:
    s->fnmatch_data = ptr;
    તોડો;
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }
  પાછા result;
}
#endif /* !CURL_DISABLE_FTP */

સ્થિર CURLcode setopt_cptr_net(સંરચના Curl_easy *data, CURLoption option,
                                અક્ષર *ptr)
{
  પસંદગી(option) {
  સ્થિતિ CURLOPT_INTERFACE:
    /*
     * Set what interface or address/hostname to bind the socket to when
     * performing an operation and thus what from-IP your connection will use.
     */
    પાછા setstropt_interface(data, ptr);

#ifdef USE_RESOLV_ARES
  સ્થિતિ CURLOPT_DNS_SERVERS:
    પાછા Curl_setstropt(data, STRING_DNS_SERVERS, ptr);

  સ્થિતિ CURLOPT_DNS_INTERFACE:
    પાછા Curl_setstropt(data, STRING_DNS_INTERFACE, ptr);

  સ્થિતિ CURLOPT_DNS_LOCAL_IP4:
    પાછા Curl_setstropt(data, STRING_DNS_LOCAL_IP4, ptr);

  સ્થિતિ CURLOPT_DNS_LOCAL_IP6:
    પાછા Curl_setstropt(data, STRING_DNS_LOCAL_IP6, ptr);
#endif
#ifdef USE_UNIX_SOCKETS
  સ્થિતિ CURLOPT_UNIX_SOCKET_PATH:
    data->set.abstract_unix_socket = FALSE;
    પાછા Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);

  સ્થિતિ CURLOPT_ABSTRACT_UNIX_SOCKET:
    data->set.abstract_unix_socket = TRUE;
    પાછા Curl_setstropt(data, STRING_UNIX_SOCKET_PATH, ptr);
#endif
#ifndef CURL_DISABLE_DOH
  સ્થિતિ CURLOPT_DOH_URL:
    {
      CURLcode result = Curl_setstropt(data, STRING_DOH, ptr);
      data->set.doh = !!CURL_EASY_STR(data, STRING_DOH);
      પાછા result;
    }
#endif
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }
}

સ્થિર CURLcode setopt_cptr_misc(સંરચના Curl_easy *data, CURLoption option,
                                 અક્ષર *ptr)
{
  CURLcode result = CURLE_OK;
  સંરચના UserDefined *s = &data->set;

  પસંદગી(option) {
  સ્થિતિ CURLOPT_REQUEST_TARGET:
    પાછા Curl_setstropt(data, STRING_TARGET, ptr);
#ifndef CURL_DISABLE_NETRC
  સ્થિતિ CURLOPT_NETRC_FILE:
    પાછા Curl_setstropt(data, STRING_NETRC_FILE, ptr);
#endif
  સ્થિતિ CURLOPT_CUSTOMREQUEST:
    પાછા Curl_setstropt(data, STRING_CUSTOMREQUEST, ptr);

    /* we do not set s->method = HTTPREQ_CUSTOM; here, we continue as if we
       were using the already set type and this changes the actual request
       keyword */
  સ્થિતિ CURLOPT_SERVICE_NAME:
    પાછા Curl_setstropt(data, STRING_SERVICE_NAME, ptr);

  સ્થિતિ CURLOPT_HEADERDATA:
    s->writeheader = ptr;
    તોડો;
  સ્થિતિ CURLOPT_READDATA:
    s->in_set = ptr;
    તોડો;
  સ્થિતિ CURLOPT_WRITEDATA:
    s->out = ptr;
    તોડો;
  સ્થિતિ CURLOPT_DEBUGDATA:
    s->debugdata = ptr;
    તોડો;
  સ્થિતિ CURLOPT_PROGRESSDATA:
    s->progress_client = ptr;
    તોડો;
  સ્થિતિ CURLOPT_SEEKDATA:
    s->seek_client = ptr;
    તોડો;
  સ્થિતિ CURLOPT_IOCTLDATA:
    s->ioctl_client = ptr;
    તોડો;
  સ્થિતિ CURLOPT_SOCKOPTDATA:
    s->sockopt_client = ptr;
    તોડો;
  સ્થિતિ CURLOPT_OPENSOCKETDATA:
    s->opensocket_client = ptr;
    તોડો;
  સ્થિતિ CURLOPT_RESOLVER_START_DATA:
    s->resolver_start_client = ptr;
    તોડો;
  સ્થિતિ CURLOPT_CLOSESOCKETDATA:
    s->closesocket_client = ptr;
    તોડો;
  સ્થિતિ CURLOPT_PREREQDATA:
    s->prereq_userp = ptr;
    તોડો;
  સ્થિતિ CURLOPT_ERRORBUFFER:
    s->errorbuffer = ptr;
    તોડો;
  સ્થિતિ CURLOPT_URL:
    result = Curl_setstropt(data, STRING_SET_URL, ptr);
    Curl_bufref_set(&data->state.url,
                    CURL_EASY_STR(data, STRING_SET_URL), 0, ખાલી);
    તોડો;

  સ્થિતિ CURLOPT_USERPWD: {
    અક્ષર *u = ખાલી, *p = ખાલી;
    result = setstropt_userpwd(ptr, &u, &p);
    જો(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_USERNAME, u);
      u = ખાલી;
    }
    જો(!result) {
      result = CURL_EASY_STR_SETN(data, STRING_PASSWORD, p);
      p = ખાલી;
    }
    curlx_free(u);
    curlx_free(p);
    પાછા result;
  }

  સ્થિતિ CURLOPT_USERNAME:
    પાછા Curl_setstropt(data, STRING_USERNAME, ptr);

  સ્થિતિ CURLOPT_PASSWORD:
    પાછા Curl_setstropt(data, STRING_PASSWORD, ptr);

  સ્થિતિ CURLOPT_LOGIN_OPTIONS:
    પાછા Curl_setstropt(data, STRING_OPTIONS, ptr);

  સ્થિતિ CURLOPT_XOAUTH2_BEARER:
    પાછા Curl_setstropt(data, STRING_BEARER, ptr);
  સ્થિતિ CURLOPT_RANGE:
    પાછા Curl_setstropt(data, STRING_SET_RANGE, ptr);
  સ્થિતિ CURLOPT_PRIVATE:
    s->private_data = ptr;
    તોડો;
  સ્થિતિ CURLOPT_PROTOCOLS_STR:
    જો(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      જો(!result)
        s->allowed_protocols = protos;
    }
    નહીંતર
      /* make a NULL argument reset to default */
      s->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
    તોડો;
  સ્થિતિ CURLOPT_REDIR_PROTOCOLS_STR:
    જો(ptr) {
      curl_prot_t protos;
      result = protocol2num(ptr, &protos);
      જો(!result)
        s->redir_protocols = protos;
    }
    નહીંતર
      /* make a NULL argument reset to default */
      s->redir_protocols = (curl_prot_t)CURLPROTO_REDIR;
    તોડો;
  સ્થિતિ CURLOPT_DEFAULT_PROTOCOL:
    /* Set the protocol to use when the URL does not include any protocol */
    પાછા Curl_setstropt(data, STRING_DEFAULT_PROTOCOL, ptr);
#ifndef CURL_DISABLE_SMTP
  સ્થિતિ CURLOPT_MAIL_FROM:
    /* Set the SMTP mail originator */
    પાછા Curl_setstropt(data, STRING_MAIL_FROM, ptr);
  સ્થિતિ CURLOPT_MAIL_AUTH:
    /* Set the SMTP auth originator */
    પાછા Curl_setstropt(data, STRING_MAIL_AUTH, ptr);
#endif
  સ્થિતિ CURLOPT_SASL_AUTHZID:
    /* Authorization identity (identity to act as) */
    પાછા Curl_setstropt(data, STRING_SASL_AUTHZID, ptr);
#ifndef CURL_DISABLE_RTSP
  સ્થિતિ CURLOPT_RTSP_SESSION_ID:
    Curl_peer_unlink(&data->state.rtsp_session_origin);
    પાછા Curl_setstropt(data, STRING_RTSP_SESSION_ID, ptr);
  સ્થિતિ CURLOPT_RTSP_STREAM_URI:
    પાછા Curl_setstropt(data, STRING_RTSP_STREAM_URI, ptr);
  સ્થિતિ CURLOPT_RTSP_TRANSPORT:
    પાછા Curl_setstropt(data, STRING_RTSP_TRANSPORT, ptr);
  સ્થિતિ CURLOPT_INTERLEAVEDATA:
    s->rtp_out = ptr;
    તોડો;
#endif /* !CURL_DISABLE_RTSP */
  સ્થિતિ CURLOPT_TLSAUTH_USERNAME:
  સ્થિતિ CURLOPT_TLSAUTH_PASSWORD:
  સ્થિતિ CURLOPT_TLSAUTH_TYPE:
  સ્થિતિ CURLOPT_PROXY_TLSAUTH_USERNAME:
  સ્થિતિ CURLOPT_PROXY_TLSAUTH_PASSWORD:
  સ્થિતિ CURLOPT_PROXY_TLSAUTH_TYPE:
    પાછા CURLE_NOT_BUILT_IN;
#ifndef CURL_DISABLE_HSTS
  સ્થિતિ CURLOPT_HSTSREADDATA:
    s->hsts_read_userp = ptr;
    તોડો;
  સ્થિતિ CURLOPT_HSTSWRITEDATA:
    s->hsts_write_userp = ptr;
    તોડો;
  સ્થિતિ CURLOPT_HSTS: {
    સંરચના curl_slist *h;
    જો(!data->hsts) {
      data->hsts = Curl_hsts_init();
      જો(!data->hsts)
        પાછા CURLE_OUT_OF_MEMORY;
    }
    જો(ptr) {
      result = Curl_setstropt(data, STRING_HSTS, ptr);
      જો(result)
        પાછા result;
      /* this needs to build a list of filenames to read from, so that it can
         read them later, as we might get a shared HSTS handle to load them
         into */
      h = curl_slist_append(data->state.hstslist, ptr);
      જો(!h) {
        curl_slist_free_all(data->state.hstslist);
        data->state.hstslist = ખાલી;
        પાછા CURLE_OUT_OF_MEMORY;
      }
      data->state.hstslist = h; /* store the list for later use */
    }
    નહીંતર {
      /* clear the list of HSTS files */
      curl_slist_free_all(data->state.hstslist);
      data->state.hstslist = ખાલી;
      જો(!data->share || !data->share->hsts)
        /* throw away the HSTS cache unless shared */
        Curl_hsts_cleanup(&data->hsts);
    }
    તોડો;
  }
#endif /* !CURL_DISABLE_HSTS */
#ifndef CURL_DISABLE_ALTSVC
  સ્થિતિ CURLOPT_ALTSVC:
    જો(!data->asi) {
      data->asi = Curl_altsvc_init();
      જો(!data->asi)
        પાછા CURLE_OUT_OF_MEMORY;
    }
    result = Curl_setstropt(data, STRING_ALTSVC, ptr);
    જો(result)
      તોડો;
    જો(ptr)
      પાછા Curl_altsvc_load(data->asi, ptr);
    તોડો;
#endif /* !CURL_DISABLE_ALTSVC */
  સ્થિતિ CURLOPT_ECH:
    પાછા setopt_ech(data, ptr);
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }
  પાછા result;
}

સ્થિર CURLcode setopt_cptr(સંરચના Curl_easy *data, CURLoption option,
                            અક્ષર *ptr)
{
  પ્રકારવ્યાખ્યા CURLcode (*ptrfunc)(સંરચના Curl_easy *data, CURLoption option,
                              અક્ષર *ptr);
  /* Order by likeliness */
  સ્થિર અચલ ptrfunc setopt_call[] = {
    setopt_cptr_misc,
#જો defined(USE_SSL) || defined(USE_SSH)
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
#જો !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
    setopt_cptr_http_mqtt,
#endif
  };
  size_t i;

  માટે(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode result = setopt_call[i](data, option, ptr);
    જો(result != CURLE_UNKNOWN_OPTION)
      પાછા result;
  }
  પાછા CURLE_UNKNOWN_OPTION;
}

સ્થિર CURLcode setopt_func(સંરચના Curl_easy *data, CURLoption option,
                            va_list param)
{
  સંરચના UserDefined *s = &data->set;
  પસંદગી(option) {
  સ્થિતિ CURLOPT_PROGRESSFUNCTION:
    /*
     * Progress callback function
     */
    s->fprogress = va_arg(param, curl_progress_callback);
    જો(s->fprogress)
      data->progress.callback = TRUE; /* no longer internal */
    નહીંતર
      data->progress.callback = FALSE; /* NULL enforces internal */
    તોડો;

  સ્થિતિ CURLOPT_XFERINFOFUNCTION:
    /*
     * Transfer info callback function
     */
    s->fxferinfo = va_arg(param, curl_xferinfo_callback);
    જો(s->fxferinfo)
      data->progress.callback = TRUE; /* no longer internal */
    નહીંતર
      data->progress.callback = FALSE; /* NULL enforces internal */

    તોડો;
  સ્થિતિ CURLOPT_DEBUGFUNCTION:
    /*
     * stderr write callback.
     */
    s->fdebug = va_arg(param, curl_debug_callback);
    /*
     * if the callback provided is NULL, it will use the default callback
     */
    તોડો;
  સ્થિતિ CURLOPT_HEADERFUNCTION:
    /*
     * Set header write callback
     */
    s->fwrite_header = va_arg(param, curl_write_callback);
    તોડો;
  સ્થિતિ CURLOPT_WRITEFUNCTION:
    /*
     * Set data write callback
     */
    s->fwrite_func = va_arg(param, curl_write_callback);
    જો(!s->fwrite_func)
#જો defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ધકેલો
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fwrite_func = (curl_write_callback)fwrite;
#જો defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic કાઢો
#endif
    તોડો;
  સ્થિતિ CURLOPT_READFUNCTION:
    /*
     * Read data callback
     */
    s->fread_func_set = va_arg(param, curl_read_callback);
    જો(!s->fread_func_set) {
      s->is_fread_set = 0;
#જો defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ધકેલો
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
      /* When set to NULL, reset to our internal default function */
      s->fread_func_set = (curl_read_callback)fread;
#જો defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic કાઢો
#endif
    }
    નહીંતર
      s->is_fread_set = 1;
    તોડો;
  સ્થિતિ CURLOPT_SEEKFUNCTION:
    /*
     * Seek callback. Might be NULL.
     */
    s->seek_func = va_arg(param, curl_seek_callback);
    તોડો;
  સ્થિતિ CURLOPT_IOCTLFUNCTION:
    /*
     * I/O control callback. Might be NULL.
     */
    s->ioctl_func = va_arg(param, curl_ioctl_callback);
    તોડો;
  સ્થિતિ CURLOPT_SSL_CTX_FUNCTION:
    /*
     * Set an SSL_CTX callback
     */
#ifdef USE_SSL
    જો(Curl_ssl_supports(data, SSLSUPP_SSL_CTX)) {
      s->ssl_fsslctx = va_arg(param, curl_ssl_ctx_callback);
      તોડો;
    }
    નહીંતર
#endif
      પાછા CURLE_NOT_BUILT_IN;

  સ્થિતિ CURLOPT_SOCKOPTFUNCTION:
    /*
     * socket callback function: called after socket() but before connect()
     */
    s->fsockopt = va_arg(param, curl_sockopt_callback);
    તોડો;

  સ્થિતિ CURLOPT_OPENSOCKETFUNCTION:
    /*
     * open/create socket callback function: called instead of socket(),
     * before connect()
     */
    s->fopensocket = va_arg(param, curl_opensocket_callback);
    તોડો;

  સ્થિતિ CURLOPT_CLOSESOCKETFUNCTION:
    /*
     * close socket callback function: called instead of close()
     * when shutting down a connection
     */
    s->fclosesocket = va_arg(param, curl_closesocket_callback);
    તોડો;

  સ્થિતિ CURLOPT_RESOLVER_START_FUNCTION:
    /*
     * resolver start callback function: called before a new resolver request
     * is started
     */
    s->resolver_start = va_arg(param, curl_resolver_start_callback);
    તોડો;

#ifdef USE_SSH
#ifdef USE_LIBSSH2
  સ્થિતિ CURLOPT_SSH_HOSTKEYFUNCTION:
    /* the callback to check the hostkey without the knownhost file */
    s->ssh_hostkeyfunc = va_arg(param, curl_sshhostkeycallback);
    તોડો;
#endif

  સ્થિતિ CURLOPT_SSH_KEYFUNCTION:
    /* setting to NULL is fine since the ssh.c functions themselves will
       then revert to use the internal default */
    s->ssh_keyfunc = va_arg(param, curl_sshkeycallback);
    તોડો;

#endif /* USE_SSH */

#ifndef CURL_DISABLE_RTSP
  સ્થિતિ CURLOPT_INTERLEAVEFUNCTION:
    /* Set the user defined RTP write function */
    s->fwrite_rtp = va_arg(param, curl_write_callback);
    તોડો;
#endif
#ifndef CURL_DISABLE_FTP
  સ્થિતિ CURLOPT_CHUNK_BGN_FUNCTION:
    s->chunk_bgn = va_arg(param, curl_chunk_bgn_callback);
    તોડો;
  સ્થિતિ CURLOPT_CHUNK_END_FUNCTION:
    s->chunk_end = va_arg(param, curl_chunk_end_callback);
    તોડો;
  સ્થિતિ CURLOPT_FNMATCH_FUNCTION:
    s->fnmatch = va_arg(param, curl_fnmatch_callback);
    તોડો;
#endif
#ifndef CURL_DISABLE_HTTP
  સ્થિતિ CURLOPT_TRAILERFUNCTION:
    s->trailer_callback = va_arg(param, curl_trailer_callback);
    તોડો;
#endif
#ifndef CURL_DISABLE_HSTS
  સ્થિતિ CURLOPT_HSTSREADFUNCTION:
    s->hsts_read = va_arg(param, curl_hstsread_callback);
    તોડો;
  સ્થિતિ CURLOPT_HSTSWRITEFUNCTION:
    s->hsts_write = va_arg(param, curl_hstswrite_callback);
    તોડો;
#endif
  સ્થિતિ CURLOPT_PREREQFUNCTION:
    s->fprereq = va_arg(param, curl_prereq_callback);
    તોડો;
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode setopt_offt(સંરચના Curl_easy *data, CURLoption option,
                            curl_off_t offt)
{
  સંરચના UserDefined *s = &data->set;
  પસંદગી(option) {
  સ્થિતિ CURLOPT_TIMEVALUE_LARGE:
    /*
     * This is the value to compare with the remote document with the
     * method set with CURLOPT_TIMECONDITION
     */
    s->timevalue = (time_t)offt;
    તોડો;

    /* MQTT "borrows" some of the HTTP options */
  સ્થિતિ CURLOPT_POSTFIELDSIZE_LARGE:
    /*
     * The size of the POSTFIELD data to prevent libcurl to do strlen() to
     * figure it out. Enables binary posts.
     */
    જો(offt < -1)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;

    જો(s->postfieldsize < offt && s->str_copypostfields) {
      /* Previous CURLOPT_COPYPOSTFIELDS is no longer valid. */
      curlx_safefree(s->str_copypostfields);
      s->postfields = ખાલી;
    }
    s->postfieldsize = offt;
    તોડો;
  સ્થિતિ CURLOPT_INFILESIZE_LARGE:
    /*
     * If known, this should inform curl about the file size of the
     * to-be-uploaded file.
     */
    જો(offt < -1)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    s->filesize = offt;
    તોડો;
  સ્થિતિ CURLOPT_MAX_SEND_SPEED_LARGE:
    /*
     * When transfer uploads are faster than CURLOPT_MAX_SEND_SPEED_LARGE
     * bytes per second the transfer is throttled..
     */
    જો(offt < 0)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_send_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.ul.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    તોડો;
  સ્થિતિ CURLOPT_MAX_RECV_SPEED_LARGE:
    /*
     * When receiving data faster than CURLOPT_MAX_RECV_SPEED_LARGE bytes per
     * second the transfer is throttled..
     */
    જો(offt < 0)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_recv_speed = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&data->progress.dl.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    તોડો;
  સ્થિતિ CURLOPT_RESUME_FROM_LARGE:
    /*
     * Resume transfer at the given file position
     */
    જો(offt < -1)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    s->set_resume_from = offt;
    તોડો;
  સ્થિતિ CURLOPT_MAXFILESIZE_LARGE:
    /*
     * Set the maximum size of a file to download.
     */
    જો(offt < 0)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    s->max_filesize = offt;
    તોડો;

  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode setopt_blob(સંરચના Curl_easy *data, CURLoption option,
                            સંરચના curl_blob *blob)
{
  સંરચના UserDefined *s = &data->set;
  પસંદગી(option) {
  સ્થિતિ CURLOPT_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use
     */
    પાછા Curl_setblobopt(&s->blobs[BLOB_CERT], blob);
#ifndef CURL_DISABLE_PROXY
  સ્થિતિ CURLOPT_PROXY_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use for proxy
     */
    પાછા Curl_setblobopt(&s->blobs[BLOB_CERT_PROXY], blob);
  સ્થિતિ CURLOPT_PROXY_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use for proxy
     */
    પાછા Curl_setblobopt(&s->blobs[BLOB_ચાવી_PROXY], blob);
  સ્થિતિ CURLOPT_PROXY_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection proxy.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    જો(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO_PROXY], blob);
      જો(!result)
        s->proxy_ssl.custom_cablob = TRUE;
      પાછા result;
    }
#endif
    પાછા CURLE_NOT_BUILT_IN;
  સ્થિતિ CURLOPT_PROXY_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    જો(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      પાછા Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT_PROXY], blob);
    પાછા CURLE_NOT_BUILT_IN;
#endif
  સ્થિતિ CURLOPT_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use
     */
    પાછા Curl_setblobopt(&s->blobs[BLOB_ચાવી], blob);
  સ્થિતિ CURLOPT_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection.
     * Specify entire PEM of the CA certificate
     */
#ifdef USE_SSL
    જો(Curl_ssl_supports(data, SSLSUPP_CAINFO_BLOB)) {
      CURLcode result = Curl_setblobopt(&s->blobs[BLOB_CAINFO], blob);
      જો(!result)
        s->ssl.custom_cablob = TRUE;
      પાછા result;
    }
#endif
    પાછા CURLE_NOT_BUILT_IN;
  સ્થિતિ CURLOPT_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    જો(Curl_ssl_supports(data, SSLSUPP_ISSUERCERT_BLOB))
      પાછા Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT], blob);
    પાછા CURLE_NOT_BUILT_IN;

  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }
  /* unreachable */
}

/*
 * Do not make Curl_vsetopt() static: it is called from
 * projects/OS400/ccsidcurl.c.
 */
CURLcode Curl_vsetopt(સંરચના Curl_easy *data, CURLoption option, va_list param)
{
  જો(option < CURLOPTTYPE_OBJECTPOINT)
    પાછા setopt_long(data, option, va_arg(param, long));
  નહીંતર જો(option < CURLOPTTYPE_FUNCTIONPOINT) {
    /* unfortunately, different pointer types cannot be identified any other
       way than being listed explicitly */
    પસંદગી(option) {
    સ્થિતિ CURLOPT_HTTPHEADER:
    સ્થિતિ CURLOPT_QUOTE:
    સ્થિતિ CURLOPT_POSTQUOTE:
    સ્થિતિ CURLOPT_TELNETOPTIONS:
    સ્થિતિ CURLOPT_PREQUOTE:
    સ્થિતિ CURLOPT_HTTP200ALIASES:
    સ્થિતિ CURLOPT_MAIL_RCPT:
    સ્થિતિ CURLOPT_RESOLVE:
    સ્થિતિ CURLOPT_PROXYHEADER:
    સ્થિતિ CURLOPT_CONNECT_TO:
      પાછા setopt_slist(data, option, va_arg(param, સંરચના curl_slist *));
    સ્થિતિ CURLOPT_HTTPPOST:         /* curl_httppost * */
    સ્થિતિ CURLOPT_MIMEPOST:         /* curl_mime * */
    સ્થિતિ CURLOPT_STDERR:           /* FILE * */
    સ્થિતિ CURLOPT_SHARE:            /* CURLSH * */
    સ્થિતિ CURLOPT_CURLU:            /* CURLU * */
      પાછા setopt_pointers(data, option, param);
    સ્થિતિ CURLOPT_STREAM_DEPENDS:   /* CURL * */
    સ્થિતિ CURLOPT_STREAM_DEPENDS_E: /* CURL * */
      પાછા CURLE_OK;
    મૂળભૂત:
      તોડો;
    }
    /* the char pointer options */
    પાછા setopt_cptr(data, option, va_arg(param, અક્ષર *));
  }
  નહીંતર જો(option < CURLOPTTYPE_OFF_T)
    પાછા setopt_func(data, option, param);
  નહીંતર જો(option < CURLOPTTYPE_BLOB)
    પાછા setopt_offt(data, option, va_arg(param, curl_off_t));
  પાછા setopt_blob(data, option, va_arg(param, સંરચના curl_blob *));
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
  સંરચના Curl_eapi_guard guard;
  CURLcode result;

  જો(CURL_EAPI_ENTER(&guard, curl, easy_setopt, &result)) {
    સંરચના Curl_easy *data = curl;
    va_list arg;

    va_start(arg, option);

    result = Curl_vsetopt(data, option, arg);

    va_end(arg);
    જો(result == CURLE_BAD_FUNCTION_ARGUMENT)
      failf(data, "setopt 0x%x got bad argument", (unsigned પૂર્ણાંક)option);
  }
  CURL_EAPI_LEAVE(&guard);
  પાછા result;
}
