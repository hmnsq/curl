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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LINUX_TCP_H
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/tcp.h>
#ਜਾਂਜੇ defined(HAVE_NETINET_TCP_H)
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/tcp.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "content_encoding.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_share.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "setopt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "altsvc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hsts.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "tftp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"

ਸਥਿਰ CURLcode setopt_ਸੈੱਟ_timeout_sec(timediff_t *ptimeout_ms, ਲੰਮਾ secs)
{
  ਜੇ(secs < 0)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
#ਜੇ LONG_MAX > (TIMEDIFF_T_MAX / 1000)
  ਜੇ(secs > (TIMEDIFF_T_MAX / 1000)) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    ਵਾਪਸ CURLE_OK;
  }
#ਜੇ_ਅੰਤ
  *ptimeout_ms = (timediff_t)secs * 1000;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode setopt_ਸੈੱਟ_timeout_ms(timediff_t *ptimeout_ms, ਲੰਮਾ ms)
{
  ਜੇ(ms < 0)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
#ਜੇ LONG_MAX > TIMEDIFF_T_MAX
  ਜੇ(ms > TIMEDIFF_T_MAX) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    ਵਾਪਸ CURLE_OK;
  }
#ਜੇ_ਅੰਤ
  *ptimeout_ms = (timediff_t)ms;
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_setstropt(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਗਿਣਤੀ dupstring id, ਅਚਲ ਅੱਖਰ *s)
{
  size_t slen = s ? ਲੜੀ_ਲੰਬਾਈ(s) : 0;
  DEBUGASSERT((ਬਿਨਾਨਿਸ਼ਾਨੀ)id <= UINT8_MAX);
  ਜੇ(s && (slen > CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ))
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  ਵਾਪਸ CURL_EASY_STR_ਸੈੱਟ(ਅੰਕੜੇ, (uint8_t)id, s, slen);
}

CURLcode Curl_setblobopt(ਬਣਤਰ curl_blob **blobp,
                         ਅਚਲ ਬਣਤਰ curl_blob *blob)
{
  /* free the previous storage at `blobp' and replace by a dynamic storage
     copy of blob. If CURL_BLOB_COPY is set, the data is copied. */

  curlx_safefree(*blobp);

  ਜੇ(blob) {
    ਬਣਤਰ curl_blob *nblob;
    ਜੇ(!blob->ਅੰਕੜੇ || !blob->len || (blob->len > CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ))
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    nblob = (ਬਣਤਰ curl_blob *)
      curlx_malloc(ਆਕਾਰ(ਬਣਤਰ curl_blob) +
                   ((blob->flags & CURL_BLOB_COPY) ? blob->len : 0));
    ਜੇ(!nblob)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    *nblob = *blob;
    ਜੇ(blob->flags & CURL_BLOB_COPY) {
      /* put the data after the blob struct in memory */
      nblob->ਅੰਕੜੇ = (ਅੱਖਰ *)nblob + ਆਕਾਰ(ਬਣਤਰ curl_blob);
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(nblob->ਅੰਕੜੇ, blob->ਅੰਕੜੇ, blob->len);
    }

    *blobp = nblob;
    ਵਾਪਸ CURLE_OK;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode setstropt_userpwd(ਅਚਲ ਅੱਖਰ *option, ਅੱਖਰ **userp,
                                  ਅੱਖਰ **passwdp)
{
  ਅੱਖਰ *user = ਨਿਰਾ;
  ਅੱਖਰ *passwd = ਨਿਰਾ;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Parse the login details if specified. If not, then we treat NULL as a
     hint to clear the existing data */
  ਜੇ(option) {
    size_t len = ਲੜੀ_ਲੰਬਾਈ(option);
    CURLcode ਨਤੀਜਾ;
    ਜੇ(len > CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

    ਨਤੀਜਾ = Curl_parse_login_details(option, len, &user, &passwd, ਨਿਰਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  curlx_free(*userp);
  *userp = user;

  curlx_strzero(*passwdp);
  curlx_free(*passwdp);
  *passwdp = passwd;

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode setstropt_interface(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅੱਖਰ *option)
{
  ਜੇ(option && !option[0])
    option = ਨਿਰਾ;
  ਜੇ(option) {
    /* Parse for checking syntax */
    CURLcode ਨਤੀਜਾ = Curl_parse_interface(option, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ਵਾਪਸ CURL_EASY_STR_ਸੈੱਟ(ਅੰਕੜੇ, STRING_INTERFACE_OPT,
                             option, ਲੜੀ_ਲੰਬਾਈ(option));
  }
  CURL_EASY_STR_CLEAR(ਅੰਕੜੇ, STRING_INTERFACE_OPT);
  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ C_SSLVERSION_ਮੁੱਲ(x)     ((x) & 0xffff)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ C_SSLVERSION_MAX_ਮੁੱਲ(x) ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(x) & 0xffff0000)
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode protocol2num(ਅਚਲ ਅੱਖਰ *str, curl_prot_t *val)
{
  /*
   * We are asked to cherry-pick protocols, so play it safe and disallow all
   * protocols to start with, and re-add the wanted ones back in.
   */
  *val = 0;

  ਜੇ(!str)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  ਜੇ(curl_strequal(str, "all")) {
    *val = ~(curl_prot_t)0;
    ਵਾਪਸ CURLE_OK;
  }

  ਕਰੋ {
    ਅਚਲ ਅੱਖਰ *token = str;
    size_t tlen;

    str = strchr(str, ',');
    tlen = str ? (size_t)(str - token) : ਲੜੀ_ਲੰਬਾਈ(token);
    ਜੇ(tlen) {
      ਅਚਲ ਬਣਤਰ Curl_scheme *h = Curl_getn_scheme(token, tlen);

      ਜੇ(!h || !h->ਲੜੀਵਾਰ)
        ਵਾਪਸ CURLE_UNSUPPORTED_PROTOCOL;

      *val |= h->protocol;
    }
  } ਜਦੋਂ(str && str++);

  ਜੇ(!*val)
    /* no protocol listed */
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਵਾਪਸ CURLE_OK;
}

#ਜੇ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
ਸਥਿਰ CURLcode httpauth(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool proxy,
                         ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ auth)
{
  ਜੇ(auth != CURLAUTH_NONE) {
    ਪੂਰਨ bitcheck = 0;
    bool authbits = FALSE;
    ਜੇ(auth & CURLAUTH_DIGEST_IE) {
      auth |= CURLAUTH_DIGEST; /* set standard digest bit */
      auth &= ~CURLAUTH_DIGEST_IE; /* drop the legacy bit */
    }

    /* switch off bits we cannot support */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_NTLM
    auth &= ~CURLAUTH_NTLM; /* no NTLM support */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_SPNEGO
    auth &= ~CURLAUTH_NEGOTIATE; /* no Negotiate (SPNEGO) auth without GSS-API
                                    or SSPI */
#ਜੇ_ਅੰਤ

    /* check if any auth bit lower than CURLAUTH_ONLY is still set */
    ਜਦੋਂ(bitcheck < 31) {
      ਜੇ(auth & (1UL << bitcheck++)) {
        authbits = TRUE;
        ਰੋਕੋ;
      }
    }
    ਜੇ(!authbits)
      ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ; /* no supported types left! */
  }
  ਜੇ(proxy)
    ਅੰਕੜੇ->ਸੈੱਟ.proxyauth = (uint32_t)auth;
  ਨਹੀਂਤਾਂ
    ਅੰਕੜੇ->ਸੈੱਟ.httpauth = (uint32_t)auth;
  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
ਸਥਿਰ CURLcode setopt_HTTP_ਸੰਸਕਰਣ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਲੰਮਾ arg)
{
  /*
   * This sets a requested HTTP version to be used. The value is one of
   * the listed enums in curl/curl.h.
   */
  ਚੋਣ(arg) {
  ਹਾਲਤ CURL_HTTP_ਸੰਸਕਰਣ_NONE:
    /* accepted */
    ਰੋਕੋ;
  ਹਾਲਤ CURL_HTTP_ਸੰਸਕਰਣ_1_0:
  ਹਾਲਤ CURL_HTTP_ਸੰਸਕਰਣ_1_1:
    /* accepted */
    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTP2
  ਹਾਲਤ CURL_HTTP_ਸੰਸਕਰਣ_2_0:
  ਹਾਲਤ CURL_HTTP_ਸੰਸਕਰਣ_2_TLS:
  ਹਾਲਤ CURL_HTTP_ਸੰਸਕਰਣ_2_ਪੂਰਵ_KNOWLEDGE:
    /* accepted */
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTP3
  ਹਾਲਤ CURL_HTTP_ਸੰਸਕਰਣ_3:
  ਹਾਲਤ CURL_HTTP_ਸੰਸਕਰਣ_3_ONLY:
    /* accepted */
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਮੂਲ:
    /* not accepted */
    ਜੇ(arg < CURL_HTTP_ਸੰਸਕਰਣ_NONE)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਵਾਪਸ CURLE_UNSUPPORTED_PROTOCOL;
  }
  ਅੰਕੜੇ->ਸੈੱਟ.httpwant = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)arg;
  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
CURLcode Curl_setopt_SSLVERSION(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                                ਲੰਮਾ arg)
{
  /*
   * Set explicit SSL version to try to connect with, as some SSL
   * implementations are lame.
   */
  {
    ਲੰਮਾ ਸੰਸਕਰਣ_2, ਸੰਸਕਰਣ_max;
    ਬਣਤਰ ssl_easy_config *sslc = &ਅੰਕੜੇ->ਸੈੱਟ.ssl;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
    ਜੇ(option != CURLOPT_SSLVERSION)
      sslc = &ਅੰਕੜੇ->ਸੈੱਟ.proxy_ssl;
#ਨਹੀਂਤਾਂ
    (ਖਾਲੀ)option; /*ਅਣਵਰਤਿਆ*/
#ਜੇ_ਅੰਤ
    ਸੰਸਕਰਣ_2 = C_SSLVERSION_ਮੁੱਲ(arg);
    ਸੰਸਕਰਣ_max = (ਲੰਮਾ)C_SSLVERSION_MAX_ਮੁੱਲ(arg);

    ਜੇ(ਸੰਸਕਰਣ_2 < CURL_SSLVERSION_DEFAULT ||
       ਸੰਸਕਰਣ_2 == CURL_SSLVERSION_SSLv2 ||
       ਸੰਸਕਰਣ_2 == CURL_SSLVERSION_SSLv3 ||
       ਸੰਸਕਰਣ_2 >= CURL_SSLVERSION_ਆਖਰੀ ||
       ਸੰਸਕਰਣ_max < CURL_SSLVERSION_MAX_NONE ||
       ਸੰਸਕਰਣ_max >= CURL_SSLVERSION_MAX_ਆਖਰੀ)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜੇ(ਸੰਸਕਰਣ_2 == CURL_SSLVERSION_DEFAULT)
      ਸੰਸਕਰਣ_2 = CURL_SSLVERSION_TLSv1_2;

    sslc->ਸੰਸਕਰਣ_2 = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)ਸੰਸਕਰਣ_2;
    sslc->ਸੰਸਕਰਣ_max = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਸੰਸਕਰਣ_max;
  }
  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ /* !USE_SSL */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_RTSP
ਸਥਿਰ CURLcode setopt_RTSP_REQUEST(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਲੰਮਾ arg)
{
  /*
   * Set the RTSP request method (OPTIONS, SETUP, PLAY, etc...) Would this be
   * better if the RTSPREQ_* were moved into here?
   */
  ਜੇ((arg <= CURL_RTSPREQ_NONE) || (arg >= CURL_RTSPREQ_ਆਖਰੀ))
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  ਅੰਕੜੇ->ਸੈੱਟ.rtspreq = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)arg;
  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_RTSP */

ਸਥਿਰ CURLcode setopt_long_bool(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                                 ਲੰਮਾ arg)
{
  bool enabled = !!arg;
  ਪੂਰਨ ok = 1;
  ਬਣਤਰ UserDefined *s = &ਅੰਕੜੇ->ਸੈੱਟ;
  ਚੋਣ(option) {
  ਹਾਲਤ CURLOPT_FORBID_REUSE:
    /*
     * When this transfer is done, it must not be left to be reused by a
     * subsequent transfer but shall be closed immediately.
     */
    s->reuse_forbid = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_FRESH_CONNECT:
    /*
     * This transfer shall not use a previously cached connection but
     * should be made with a fresh new connect!
     */
    s->reuse_fresh = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_VERBOSE:
    /*
     * Verbose means infof() calls that give a lot of information about
     * the connection and transfer procedures as well as internal choices.
     */
    s->verbose = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_ਸਿਰਲੇਖ:
    /*
     * Set to include the header in the general data output stream.
     */
    s->include_ਸਿਰਲੇਖ = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_NOPROGRESS:
    /*
     * Shut off the internal supported progress meter
     */
    ਅੰਕੜੇ->ਤਰੱਕੀ.hide = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_NOBODY:
    /*
     * Do not include the body part in the output data stream.
     */
    s->opt_no_body = enabled;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
    ਜੇ(s->opt_no_body)
      /* in HTTP lingo, no body means using the HEAD request... */
      s->ਵਿਧੀ = HTTPREQ_ਸਿਰ;
    ਨਹੀਂਤਾਂ ਜੇ(s->ਵਿਧੀ == HTTPREQ_ਸਿਰ)
      s->ਵਿਧੀ = HTTPREQ_GET;
#ਜੇ_ਅੰਤ
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_FAILONERROR:
    /*
     * Do not output the >=400 error code HTML-page, but instead only
     * return error.
     */
    s->http_fail_on_error = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_KEEP_SENDING_ON_ERROR:
    s->http_keep_sending_on_error = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_UPLOAD:
  ਹਾਲਤ CURLOPT_PUT:
    /*
     * We want to send data to the remote host. If this is HTTP, that equals
     * using the PUT request.
     */
    ਜੇ(enabled) {
      /* If this is HTTP, PUT is what's needed to "upload" */
      s->ਵਿਧੀ = HTTPREQ_PUT;
      s->opt_no_body = FALSE; /* this is implied */
    }
    ਨਹੀਂਤਾਂ
      /* In HTTP, the opposite of upload is GET (unless NOBODY is true as
         then this can be changed to HEAD later on) */
      s->ਵਿਧੀ = HTTPREQ_GET;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_FILETIME:
    /*
     * Try to get the file time of the remote document. The time will
     * later (possibly) become available using curl_easy_getinfo().
     */
    s->get_filetime = enabled;
    ਰੋਕੋ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  ਹਾਲਤ CURLOPT_HTTP09_ALLOWED:
    s->http09_allowed = enabled;
    ਰੋਕੋ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_COOKIES
  ਹਾਲਤ CURLOPT_COOKIESESSION:
    /*
     * Set this option to TRUE to start a new "cookie session". It will
     * prevent the forthcoming read-cookies-from-file actions to accept
     * cookies that are marked as being session cookies, as they belong to a
     * previous session.
     */
    s->cookiesession = enabled;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਹਾਲਤ CURLOPT_AUTOREFERER:
    /*
     * Switch on automatic referer that gets set if curl follows locations.
     */
    s->http_auto_referer = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_TRANSFER_ENCODING:
    s->http_transfer_encoding = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_UNRESTRICTED_AUTH:
    /*
     * Send authentication (user+password) when following locations, even when
     * hostname changed.
     */
    s->allow_auth_ਮੰਜ਼ਿਲ_other_hosts = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_HTTP_TRANSFER_DECODING:
    /*
     * disable libcurl transfer encoding is used
     */
    s->http_te_skip = !enabled; /* reversed */
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_HTTP_CONTENT_DECODING:
    /*
     * raw data passed to the application when content encoding is used
     */
    s->http_ce_skip = !enabled; /* reversed */
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_HTTPGET:
    /*
     * Set to force us do HTTP GET
     */
    ਜੇ(enabled) {
      s->ਵਿਧੀ = HTTPREQ_GET;
      s->opt_no_body = FALSE; /* this is implied */
    }
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_POST:
    /* Does this option serve a purpose anymore? Yes it does, when
       CURLOPT_POSTFIELDS is not used and the POST data is read off the
       callback! */
    ਜੇ(enabled) {
      s->ਵਿਧੀ = HTTPREQ_POST;
      s->opt_no_body = FALSE; /* this is implied */
    }
    ਨਹੀਂਤਾਂ
      s->ਵਿਧੀ = HTTPREQ_GET;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਹਾਲਤ CURLOPT_HTTPPROXYTUNNEL:
    /*
     * Tunnel operations through the proxy instead of normal proxy use
     */
    s->tunnel_thru_httpproxy = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_HAPROXYPROTOCOL:
    /*
     * Set to send the HAProxy Proxy Protocol header
     */
    s->haproxyprotocol = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_PROXY_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for proxy.
     */
    s->proxy_ssl.verifypeer = enabled;

    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(ਅੰਕੜੇ, TRUE);
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_PROXY_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for proxy
     */
    s->proxy_ssl.verifyhost = enabled;
    ok = 2;
    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(ਅੰਕੜੇ, TRUE);
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_PROXY_TRANSFER_ਬਹੁਲਕ:
    /*
     * set transfer mode (;type=<a|i>) when doing FTP via an HTTP proxy
     */
    s->proxy_transfer_ਬਹੁਲਕ = enabled;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_PROXY */
#ਜੇ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ਹਾਲਤ CURLOPT_SOCKS5_GSSAPI_NEC:
    /*
     * Set flag for NEC SOCKS5 support
     */
    s->socks5_gssapi_nec = enabled;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_LIST_ONLY_PROTOCOL
  ਹਾਲਤ CURLOPT_DIRLISTONLY:
    /*
     * An option that changes the command to one that asks for a list only, no
     * file info details. Used for FTP, POP3 and SFTP.
     */
    s->list_only = enabled;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਹਾਲਤ CURLOPT_APPEND:
    /*
     * We want to upload and append to an existing file. Used for FTP and
     * SFTP.
     */
    s->remote_append = enabled;
    ਰੋਕੋ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
  ਹਾਲਤ CURLOPT_FTP_USE_EPRT:
    s->ftp_use_eprt = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_FTP_USE_EPSV:
    s->ftp_use_epsv = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_FTP_USE_PRET:
    s->ftp_use_pret = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_FTP_SKIP_PASV_IP:
    /*
     * Enable or disable FTP_SKIP_PASV_IP, which will disable/enable the
     * bypass of the IP address in PASV responses.
     */
    s->ftp_skip_ip = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_WILDCARDMATCH:
    s->wildcard_enabled = enabled;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਹਾਲਤ CURLOPT_CRLF:
    /*
     * Kludgy option to enable CRLF conversions. Subject for removal.
     */
    s->crlf = enabled;
    ਰੋਕੋ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_TFTP
  ਹਾਲਤ CURLOPT_TFTP_NO_OPTIONS:
    /*
     * Option that prevents libcurl from sending TFTP option requests to the
     * server.
     */
    s->tftp_no_options = enabled;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_TFTP */
  ਹਾਲਤ CURLOPT_TRANSFERTEXT:
    /*
     * This option was previously named 'FTPASCII'. Renamed to work with
     * more protocols than merely FTP.
     *
     * Transfer using ASCII (instead of BINARY).
     */
    s->prefer_ascii = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying.
     */
    s->ssl.verifypeer = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(ਅੰਕੜੇ, FALSE);
    ਰੋਕੋ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DOH
  ਹਾਲਤ CURLOPT_DOH_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for DoH.
     */
    s->doh_verifypeer = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_DOH_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for DoH
     */
    s->doh_verifyhost = enabled;
    ok = 2;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_DOH_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying for DoH.
     */
    ਜੇ(!Curl_ssl_cert_ਸਥਿਤੀ_request())
      ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;

    s->doh_verifystatus = enabled;
    ok = 2;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_DOH */
  ਹਾਲਤ CURLOPT_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate
     */

    /* Obviously people are not reading documentation and too many thought
       this argument took a boolean when it was not and misused it.
       Treat 1 and 2 the same */
    s->ssl.verifyhost = enabled;
    ok = 2;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(ਅੰਕੜੇ, FALSE);
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying.
     */
    ਜੇ(!Curl_ssl_cert_ਸਥਿਤੀ_request())
      ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;

    s->ssl.verifystatus = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(ਅੰਕੜੇ, FALSE);
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_CERTINFO:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_CERTINFO))
      s->ssl.certinfo = enabled;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
      ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_NOSIGNAL:
    /*
     * The application asks not to set any signal() or alarm() handlers,
     * even when using a timeout.
     */
    s->no_signal = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_TCP_NODELAY:
    /*
     * Enable or disable TCP_NODELAY, which will disable/enable the Nagle
     * algorithm
     */
    s->tcp_nodelay = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_IGNORE_CONTENT_ਲੰਮਾਈ:
    s->ignorecl = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_SSL_SESSIONID_CACHE:
    s->ssl.cache_session = enabled;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
    s->proxy_ssl.cache_session = s->ssl.cache_session;
#ਜੇ_ਅੰਤ
    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
  ਹਾਲਤ CURLOPT_SSH_COMPRESSION:
    s->ssh_compression = enabled;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ /* !USE_SSH */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_SMTP
  ਹਾਲਤ CURLOPT_MAIL_RCPT_ALLOWFAILS:
    /* allow RCPT TO command to fail for some recipients */
    s->mail_rcpt_allowfails = enabled;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_SMTP */
  ਹਾਲਤ CURLOPT_SASL_IR:
    /* Enable/disable SASL initial response */
    s->sasl_ir = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_TCP_KEEPALIVE:
    s->tcp_keepalive = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_TCP_FASTOPEN:
#ਜੇ defined(CONNECT_ਅੰਕੜੇ_IDEMPOTENT) || defined(MSG_FASTOPEN) || \
  defined(TCP_FASTOPEN_CONNECT)
    s->tcp_fastopen = enabled;
    ਰੋਕੋ;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
  ਹਾਲਤ CURLOPT_SSL_ENABLE_ALPN:
    s->ssl_enable_alpn = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_ਰਸਤਾ_AS_IS:
    s->ਰਸਤਾ_as_is = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_PIPEWAIT:
    s->pipewait = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_SUPPRESS_CONNECT_ਸਿਰਲੇਖਾਂ:
    s->suppress_connect_ਸਿਰਲੇਖਾਂ = enabled;
    ਰੋਕੋ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_SHUFFLE_DNS
  ਹਾਲਤ CURLOPT_DNS_SHUFFLE_ਪਤੇ:
    s->dns_shuffle_ਪਤੇ = enabled;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਹਾਲਤ CURLOPT_DISALLOW_USERNAME_ਵਿੱਚ_URL:
    s->disallow_username_ਵਿੱਚ_url = enabled;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_ਤੇਜ਼_EXIT:
    s->quick_exit = enabled;
    ਰੋਕੋ;
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }
  ਜੇ((arg > ok) || (arg < 0))
    /* reserve other values for future use */
    infof(ਅੰਕੜੇ, "boolean setopt(%d) got unsupported argument %ld,"
          " treated as %d", (ਪੂਰਨ)option, arg, enabled);

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ਮੁੱਲ_range(ਲੰਮਾ *ਮੁੱਲ, ਲੰਮਾ ਹੇਠਾਂ_error, ਲੰਮਾ min, ਲੰਮਾ max)
{
  ਜੇ(*ਮੁੱਲ < ਹੇਠਾਂ_error)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਨਹੀਂਤਾਂ ਜੇ(*ਮੁੱਲ < min)
    *ਮੁੱਲ = min;
  ਨਹੀਂਤਾਂ ਜੇ(*ਮੁੱਲ > max)
    *ਮੁੱਲ = max;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode setopt_long_net(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                                ਲੰਮਾ arg)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ UserDefined *s = &ਅੰਕੜੇ->ਸੈੱਟ;

  ਚੋਣ(option) {
  ਹਾਲਤ CURLOPT_DNS_CACHE_TIMEOUT:
    ਜੇ(arg != -1)
      ਵਾਪਸ setopt_ਸੈੱਟ_timeout_sec(&s->dns_cache_timeout_ms, arg);
    s->dns_cache_timeout_ms = -1;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_MAXCONNECTS:
    ਨਤੀਜਾ = ਮੁੱਲ_range(&arg, 0, 0, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
    ਜੇ(!ਨਤੀਜਾ)
      s->maxconnects = arg ? (uint32_t)arg : DEFAULT_CONNCACHE_ਮਾਪ;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_SERVER_RESPONSE_TIMEOUT:
    ਵਾਪਸ setopt_ਸੈੱਟ_timeout_sec(&s->server_response_timeout, arg);
  ਹਾਲਤ CURLOPT_SERVER_RESPONSE_TIMEOUT_MS:
    ਵਾਪਸ setopt_ਸੈੱਟ_timeout_ms(&s->server_response_timeout, arg);
  ਹਾਲਤ CURLOPT_ਨੀਵਾਂ_ਰਫਤਾਰ_ਹੱਦਬੰਦੀ:
    ਜੇ(arg < 0)
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ
      s->ਨੀਵਾਂ_ਰਫਤਾਰ_ਹੱਦਬੰਦੀ = arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_ਨੀਵਾਂ_ਰਫਤਾਰ_ਸਮਾਂ:
    ਨਤੀਜਾ = ਮੁੱਲ_range(&arg, 0, 0, USHRT_MAX);
    ਜੇ(!ਨਤੀਜਾ)
      s->ਨੀਵਾਂ_ਰਫਤਾਰ_ਸਮਾਂ = (uint16_t)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_ਪੋਰਟ:
    ਜੇ((arg < 0) || (arg > 65535))
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    s->use_ਪੋਰਟ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_TIMEOUT:
    ਵਾਪਸ setopt_ਸੈੱਟ_timeout_sec(&s->timeout, arg);
  ਹਾਲਤ CURLOPT_TIMEOUT_MS:
    ਵਾਪਸ setopt_ਸੈੱਟ_timeout_ms(&s->timeout, arg);
  ਹਾਲਤ CURLOPT_CONNECTTIMEOUT:
    ਵਾਪਸ setopt_ਸੈੱਟ_timeout_sec(&s->connecttimeout, arg);
  ਹਾਲਤ CURLOPT_CONNECTTIMEOUT_MS:
    ਵਾਪਸ setopt_ਸੈੱਟ_timeout_ms(&s->connecttimeout, arg);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_BINDLOCAL
  ਹਾਲਤ CURLOPT_LOCALPORT:
    ਜੇ((arg < 0) || (arg > 65535))
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    s->localport = curlx_sltous(arg);
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_LOCALPORTRANGE:
    ਜੇ((arg < 0) || (arg > 65535))
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    s->localportrange = curlx_sltous(arg);
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਹਾਲਤ CURLOPT_BUFFERSIZE:
    ਨਤੀਜਾ = ਮੁੱਲ_range(&arg, 0, READBUFFER_MIN, READBUFFER_MAX);
    ਜੇ(!ਨਤੀਜਾ)
      s->ਬਫਰ_ਮਾਪ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_UPLOAD_BUFFERSIZE:
    ਨਤੀਜਾ = ਮੁੱਲ_range(&arg, 0, UPLOADBUFFER_MIN, UPLOADBUFFER_MAX);
    ਜੇ(!ਨਤੀਜਾ)
      s->upload_ਬਫਰ_ਮਾਪ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_MAXFILESIZE:
    ਜੇ(arg < 0)
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ
      s->max_filesize = arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_IPRESOLVE:
    ਜੇ((arg < CURL_IPRESOLVE_WHATEVER) || (arg > CURL_IPRESOLVE_V6))
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ
      s->ipver = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_CONNECT_ONLY:
    ਜੇ(arg < 0 || arg > 2)
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ {
      s->connect_only = !!arg;
      s->connect_only_ws = (arg == 2);
    }
    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਹਾਲਤ CURLOPT_ਪਤਾ_SCOPE:
#ਜੇ SIZEOF_LONG > 4
    ਜੇ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)arg > UINT_MAX)
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    s->scope_id = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)arg;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਹਾਲਤ CURLOPT_TCP_KEEPIDLE:
    ਨਤੀਜਾ = ਮੁੱਲ_range(&arg, 0, 0, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
    ਜੇ(!ਨਤੀਜਾ)
      s->tcp_keepidle = (ਪੂਰਨ)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_TCP_KEEPINTVL:
    ਨਤੀਜਾ = ਮੁੱਲ_range(&arg, 0, 0, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
    ਜੇ(!ਨਤੀਜਾ)
      s->tcp_keepintvl = (ਪੂਰਨ)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_TCP_KEEPCNT:
    ਨਤੀਜਾ = ਮੁੱਲ_range(&arg, 0, 0, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
    ਜੇ(!ਨਤੀਜਾ)
      s->tcp_keepcnt = (ਪੂਰਨ)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
    ਵਾਪਸ setopt_ਸੈੱਟ_timeout_ms(&s->happy_eyeballs_timeout, arg);
  ਹਾਲਤ CURLOPT_UPKEEP_INTERVAL_MS:
    ਵਾਪਸ setopt_ਸੈੱਟ_timeout_ms(&s->upkeep_interval_ms, arg);
  ਹਾਲਤ CURLOPT_MAXAGE_CONN:
    ਵਾਪਸ setopt_ਸੈੱਟ_timeout_sec(&s->conn_max_idle_ms, arg);
  ਹਾਲਤ CURLOPT_MAXLIFETIME_CONN:
    ਵਾਪਸ setopt_ਸੈੱਟ_timeout_sec(&s->conn_max_age_ms, arg);
  ਹਾਲਤ CURLOPT_DNS_USE_GLOBAL_CACHE:
    /* deprecated */
    ਰੋਕੋ;
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode setopt_long_ssl(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                                ਲੰਮਾ arg)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ UserDefined *s = &ਅੰਕੜੇ->ਸੈੱਟ;
  ਚੋਣ(option) {
  ਹਾਲਤ CURLOPT_CA_CACHE_TIMEOUT:
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_CA_CACHE)) {
      ਨਤੀਜਾ = ਮੁੱਲ_range(&arg, -1, -1, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
      ਜੇ(!ਨਤੀਜਾ)
        s->ssl_ca_cache_timeout = (ਪੂਰਨ)arg;
    }
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = CURLE_NOT_BUILT_ਵਿੱਚ;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_SSLVERSION:
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਹਾਲਤ CURLOPT_PROXY_SSLVERSION:
#ਜੇ_ਅੰਤ
    ਵਾਪਸ Curl_setopt_SSLVERSION(ਅੰਕੜੇ, option, arg);
  ਹਾਲਤ CURLOPT_SSL_FALSESTART:
    ਨਤੀਜਾ = CURLE_NOT_BUILT_ਵਿੱਚ;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_USE_SSL:
    ਜੇ((arg < CURLUSESSL_NONE) || (arg >= CURLUSESSL_ਆਖਰੀ))
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ
      s->use_ssl = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_SSL_OPTIONS:
    s->ssl.ssl_options = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(arg & 0xff);
    ਰੋਕੋ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਹਾਲਤ CURLOPT_PROXY_SSL_OPTIONS:
    s->proxy_ssl.ssl_options = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(arg & 0xff);
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਹਾਲਤ CURLOPT_SSL_ENABLE_NPN:
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_SSLENGINE_DEFAULT:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
    ਜੇ(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
    ਅੰਕੜੇ->ਸੈੱਟ.engine_default = (arg == 1);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
    ਰੋਕੋ;
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
#ਨਹੀਂਤਾਂ  /* USE_SSL */
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)option;
  (ਖਾਲੀ)arg;
  ਵਾਪਸ CURLE_UNKNOWN_OPTION;
#ਜੇ_ਅੰਤ /* !USE_SSL */
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
ਸਥਿਰ ਖਾਲੀ changeproxy(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  Curl_auth_digest_cleanup(&ਅੰਕੜੇ->state.proxydigest);
  memset(&ਅੰਕੜੇ->state.authproxy, 0, ਆਕਾਰ(ਅੰਕੜੇ->state.authproxy));
}

ਸਥਿਰ CURLcode setopt_long_proxy(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                                  ਲੰਮਾ arg)
{
  ਬਣਤਰ UserDefined *s = &ਅੰਕੜੇ->ਸੈੱਟ;

  ਚੋਣ(option) {
  ਹਾਲਤ CURLOPT_PROXYPORT:
    ਜੇ((arg < 0) || (arg > UINT16_MAX))
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜੇ(arg != s->proxyport)
      changeproxy(ਅੰਕੜੇ);
    s->proxyport = (uint16_t)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_PROXYAUTH:
    ਵਾਪਸ httpauth(ਅੰਕੜੇ, TRUE, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)arg);
  ਹਾਲਤ CURLOPT_PROXYTYPE:
    ਜੇ((arg < CURLPROXY_HTTP) || (arg > CURLPROXY_HTTPS3))
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_PROXY_HTTP3
    ਜੇ(arg == CURLPROXY_HTTPS3)
      ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
    s->proxytype = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_SOCKS5_AUTH: {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ auth = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)arg;
    ਜੇ(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI | CURLAUTH_ONLY))
      ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
    ਜੇ((auth & CURLAUTH_ONLY) && !(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI)))
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    s->socks5auth = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI));
    s->socks5_auth_only = !!(auth & CURLAUTH_ONLY);
    ਰੋਕੋ;
  }
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }
  ਵਾਪਸ CURLE_OK;
}
#ਨਹੀਂਤਾਂ
ਸਥਿਰ CURLcode setopt_long_proxy(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                                  ਲੰਮਾ arg)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)option;
  (ਖਾਲੀ)arg;
  ਵਾਪਸ CURLE_UNKNOWN_OPTION;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode setopt_long_http(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                                 ਲੰਮਾ arg)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ UserDefined *s = &ਅੰਕੜੇ->ਸੈੱਟ;

  ਚੋਣ(option) {
  ਹਾਲਤ CURLOPT_FOLLOWLOCATION:
    ਜੇ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)arg > 3)
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ
      s->http_follow_ਬਹੁਲਕ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_MAXREDIRS:
    ਨਤੀਜਾ = ਮੁੱਲ_range(&arg, -1, -1, 0x7fff);
    ਜੇ(!ਨਤੀਜਾ)
      s->maxredirs = (ਛੋਟਾ)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_POSTREDIR:
    ਜੇ(arg < CURL_REDIR_GET_ALL)
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ {
      s->post301 = !!(arg & CURL_REDIR_POST_301);
      s->post302 = !!(arg & CURL_REDIR_POST_302);
      s->post303 = !!(arg & CURL_REDIR_POST_303);
    }
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_HEADEROPT:
    s->sep_ਸਿਰਲੇਖਾਂ = !!(arg & CURLHEADER_SEPARATE);
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_HTTPAUTH:
    ਵਾਪਸ httpauth(ਅੰਕੜੇ, FALSE, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)arg);
  ਹਾਲਤ CURLOPT_HTTP_ਸੰਸਕਰਣ:
    ਵਾਪਸ setopt_HTTP_ਸੰਸਕਰਣ(ਅੰਕੜੇ, arg);
  ਹਾਲਤ CURLOPT_EXPECT_100_TIMEOUT_MS:
    ਨਤੀਜਾ = ਮੁੱਲ_range(&arg, 0, 0, 0xffff);
    ਜੇ(!ਨਤੀਜਾ)
      s->expect_100_timeout = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_ਧਾਰਾ_ਭਾਰ:
#ਜੇ defined(USE_HTTP2) || defined(USE_HTTP3)
    ਜੇ((arg >= 1) && (arg <= 256))
      s->ਭਾਰ = (ਪੂਰਨ)arg;
    ਰੋਕੋ;
#ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = CURLE_NOT_BUILT_ਵਿੱਚ;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTPSIG
  ਹਾਲਤ CURLOPT_HTTPSIG_ALGORITHM:
    ਜੇ(arg != CURLHTTPSIG_NONE &&
       arg != CURLHTTPSIG_ED25519 &&
       arg != CURLHTTPSIG_HMAC_SHA256)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    s->httpsig_algorithm = (uint8_t)arg;
    ਜੇ(arg)
      s->httpauth = (uint32_t)CURLAUTH_HTTPSIG;
    ਨਹੀਂਤਾਂ
      s->httpauth &= ~(uint32_t)CURLAUTH_HTTPSIG;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)option;
  (ਖਾਲੀ)arg;
  ਵਾਪਸ CURLE_UNKNOWN_OPTION;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ CURLcode setopt_long_proto(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                                  ਲੰਮਾ arg)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ UserDefined *s = &ਅੰਕੜੇ->ਸੈੱਟ;

  ਚੋਣ(option) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_TFTP
  ਹਾਲਤ CURLOPT_TFTP_BLKSIZE:
    ਨਤੀਜਾ = ਮੁੱਲ_range(&arg, 0, TFTP_BLKSIZE_MIN, TFTP_BLKSIZE_MAX);
    ਜੇ(!ਨਤੀਜਾ)
      s->tftp_blksize = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)arg;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_NETRC
  ਹਾਲਤ CURLOPT_NETRC:
    ਜੇ((arg < CURL_NETRC_IGNORED) || (arg >= CURL_NETRC_ਆਖਰੀ))
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ
      s->use_netrc = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)arg;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
  ਹਾਲਤ CURLOPT_FTP_FILEMETHOD:
    ਜੇ((arg < CURLFTPMETHOD_DEFAULT) || (arg >= CURLFTPMETHOD_ਆਖਰੀ))
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ
      s->ftp_filemethod = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_FTP_SSL_CCC:
    ਜੇ((arg < CURLFTPSSL_CCC_NONE) || (arg >= CURLFTPSSL_CCC_ਆਖਰੀ))
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ
      s->ftp_ccc = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_FTPSSLAUTH:
    ਜੇ((arg < CURLFTPAUTH_DEFAULT) || (arg >= CURLFTPAUTH_ਆਖਰੀ))
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ
      s->ftpsslauth = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_ACCEPTTIMEOUT_MS:
    ਵਾਪਸ setopt_ਸੈੱਟ_timeout_ms(&s->accepttimeout, arg);
#ਜੇ_ਅੰਤ
#ਜੇ !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  ਹਾਲਤ CURLOPT_FTP_CREATE_MISSING_DIRS:
    ਜੇ((arg < CURLFTP_CREATE_DIR_NONE) || (arg > CURLFTP_CREATE_DIR_RETRY))
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ
      s->ftp_create_missing_dirs = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_NEW_FILE_PERMS:
    ਜੇ((arg < 0) || (arg > 0777))
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ
      s->new_file_perms = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)arg;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_RTSP
  ਹਾਲਤ CURLOPT_RTSP_REQUEST:
    ਵਾਪਸ setopt_RTSP_REQUEST(ਅੰਕੜੇ, arg);
  ਹਾਲਤ CURLOPT_RTSP_CLIENT_CSEQ:
    ਨਤੀਜਾ = ਮੁੱਲ_range(&arg, 0, 0, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
    ਜੇ(!ਨਤੀਜਾ)
      ਅੰਕੜੇ->state.rtsp_ਅਗਲਾ_client_CSeq = (uint32_t)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_RTSP_SERVER_CSEQ:
    ਨਤੀਜਾ = ਮੁੱਲ_range(&arg, 0, 0, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
    ਜੇ(!ਨਤੀਜਾ)
      ਅੰਕੜੇ->state.rtsp_ਅਗਲਾ_server_CSeq = (uint32_t)arg;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
  ਹਾਲਤ CURLOPT_SSH_AUTH_TYPES:
    s->ssh_auth_types = (uint32_t)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_NEW_DIRECTORY_PERMS:
    ਜੇ((arg < 0) || (arg > 0777))
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ
      s->new_directory_perms = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)arg;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਹਾਲਤ CURLOPT_PROTOCOLS:
    s->allowed_protocols = (curl_prot_t)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_REDIR_PROTOCOLS:
    s->redir_protocols = (curl_prot_t)arg;
    ਰੋਕੋ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_WEBSOCKETS
  ਹਾਲਤ CURLOPT_WS_OPTIONS:
    s->ws_raw_ਬਹੁਲਕ = !!(arg & CURLWS_RAW_ਬਹੁਲਕ);
    s->ws_no_auto_pong = !!(arg & CURLWS_NOAUTOPONG);
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode setopt_long_misc(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                                 ਲੰਮਾ arg)
{
  ਬਣਤਰ UserDefined *s = &ਅੰਕੜੇ->ਸੈੱਟ;

  ਚੋਣ(option) {
  ਹਾਲਤ CURLOPT_TIMECONDITION:
    ਜੇ((arg < CURL_TIMECOND_NONE) || (arg >= CURL_TIMECOND_ਆਖਰੀ))
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    s->timecondition = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_TIMEVALUE:
    s->timevalue = (time_t)arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_POSTFIELDSIZE:
    ਜੇ(arg < -1)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜੇ(s->postfieldsize < arg && s->str_copypostfields) {
      curlx_safefree(s->str_copypostfields);
      s->postfields = ਨਿਰਾ;
    }
    s->postfieldsize = arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_INFILESIZE:
    ਜੇ(arg < -1)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    s->filesize = arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_RESUME_FROM:
    ਜੇ(arg < -1)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    s->ਸੈੱਟ_resume_from = arg;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_UPLOAD_FLAGS:
    s->upload_flags = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)arg;
    ਰੋਕੋ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_MIME
  ਹਾਲਤ CURLOPT_MIME_OPTIONS:
    s->mime_formescape = !!(arg & CURLMIMEOPT_FORMESCAPE);
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HSTS
  ਹਾਲਤ CURLOPT_HSTS_CTRL:
    ਜੇ(arg & CURLHSTS_ENABLE) {
      ਜੇ(!ਅੰਕੜੇ->hsts) {
        ਅੰਕੜੇ->hsts = Curl_hsts_init();
        ਜੇ(!ਅੰਕੜੇ->hsts)
          ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(!ਅੰਕੜੇ->share || !ਅੰਕੜੇ->share->hsts) {
      /* throw away the HSTS cache unless shared */
      Curl_hsts_cleanup(&ਅੰਕੜੇ->hsts);
      /* flush all the entries */
      curl_slist_free_all(ਅੰਕੜੇ->state.hstslist);
      ਅੰਕੜੇ->state.hstslist = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
      /* detach from shared HSTS cache without freeing it */
      ਅੰਕੜੇ->hsts = ਨਿਰਾ;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_ALTSVC
  ਹਾਲਤ CURLOPT_ALTSVC_CTRL:
    ਵਾਪਸ Curl_altsvc_ctrl(ਅੰਕੜੇ, arg);
#ਜੇ_ਅੰਤ
#ਜੇ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ਹਾਲਤ CURLOPT_GSSAPI_DELEGATION:
    s->gssapi_delegation = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)arg &
      (CURLGSSAPI_DELEGATION_POLICY_FLAG | CURLGSSAPI_DELEGATION_FLAG);
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode setopt_long(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                            ਲੰਮਾ arg)
{
  ਕਿਸਮਪਰਿਭਾਸ਼ਾ CURLcode (*setoptfunc)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 CURLoption option, ਲੰਮਾ arg);
  ਸਥਿਰ ਅਚਲ setoptfunc setopt_call[] = {
    setopt_long_bool,
    setopt_long_net,
    setopt_long_http,
    setopt_long_proxy,
    setopt_long_ssl,
    setopt_long_proto,
    setopt_long_misc
  };
  size_t i;

  ਲਈ(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode ਨਤੀਜਾ = setopt_call[i](ਅੰਕੜੇ, option, arg);
    ਜੇ(ਨਤੀਜਾ != CURLE_UNKNOWN_OPTION)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਵਾਪਸ CURLE_UNKNOWN_OPTION;
}

ਸਥਿਰ CURLcode setopt_slist(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                             ਬਣਤਰ curl_slist *slist)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ UserDefined *s = &ਅੰਕੜੇ->ਸੈੱਟ;
  ਚੋਣ(option) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਹਾਲਤ CURLOPT_PROXYHEADER:
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
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  ਹਾਲਤ CURLOPT_HTTP200ALIASES:
    /*
     * Set a list of aliases for HTTP 200 in response header
     */
    s->http200aliases = slist;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  ਹਾਲਤ CURLOPT_POSTQUOTE:
    /*
     * List of RAW FTP commands to use after a transfer
     */
    s->postquote = slist;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_PREQUOTE:
    /*
     * List of RAW FTP commands to use prior to RETR (Wesley Laxton)
     */
    s->prequote = slist;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_QUOTE:
    /*
     * List of RAW FTP commands to use before a transfer
     */
    s->quote = slist;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਹਾਲਤ CURLOPT_RESOLVE:
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
    ਅੰਕੜੇ->state.resolve = s->resolve;
    ਰੋਕੋ;
#ਜੇ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MIME)
  ਹਾਲਤ CURLOPT_HTTPHEADER:
    /*
     * Set a list with HTTP headers to use (or replace internals with)
     */
    s->ਸਿਰਲੇਖਾਂ = slist;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_TELNET
  ਹਾਲਤ CURLOPT_TELNETOPTIONS:
    /*
     * Set a linked list of telnet options
     */
    s->telnet_options = slist;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_SMTP
  ਹਾਲਤ CURLOPT_MAIL_RCPT:
    /* Set the list of mail recipients */
    s->mail_rcpt = slist;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਹਾਲਤ CURLOPT_CONNECT_ਮੰਜ਼ਿਲ:
    s->connect_ਮੰਜ਼ਿਲ = slist;
    ਰੋਕੋ;
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_MIME
ਸਥਿਰ CURLcode setopt_mimepost(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_mime *mimep)
{
  /*
   * Set to make us do MIME POST
   */
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ UserDefined *s = &ਅੰਕੜੇ->ਸੈੱਟ;
  ਜੇ(!s->mimepostp) {
    s->mimepostp = curlx_malloc(ਆਕਾਰ(*s->mimepostp));
    ਜੇ(!s->mimepostp)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    Curl_mime_initpart(s->mimepostp);
  }

  ਨਤੀਜਾ = Curl_mime_ਸੈੱਟ_subparts(s->mimepostp, mimep, FALSE);
  ਜੇ(!ਨਤੀਜਾ) {
    s->ਵਿਧੀ = HTTPREQ_POST_MIME;
    s->opt_no_body = FALSE; /* this is implied */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FORM_API
    Curl_mime_cleanpart(ਅੰਕੜੇ->state.formp);
    curlx_safefree(ਅੰਕੜੇ->state.formp);
    ਅੰਕੜੇ->state.mimepost = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_MIME */
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */

ਸਥਿਰ CURLcode setopt_share(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_share *ਸੈੱਟ)
{
  CURLcode ਨਤੀਜਾ;

  ਜੇ(ਅੰਕੜੇ->conn) {
    /* As this handle already has a connection attached, changing share now
       would be complicated and error-prone */
    infof(ਅੰਕੜੇ, "Cannot change share object while in use");
    ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }
  ਨਹੀਂਤਾਂ {
    /* disconnect from old share, if any and possible */
    ਨਤੀਜਾ = Curl_share_easy_unlink(ਅੰਕੜੇ);
    ਜੇ(!ਨਤੀਜਾ && GOOD_SHARE_HANDLE(ਸੈੱਟ))
      /* use new share if it set */
      ਨਤੀਜਾ = Curl_share_easy_link(ਅੰਕੜੇ, ਸੈੱਟ);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* assorted pointer type arguments */
ਸਥਿਰ CURLcode setopt_ਸੂਚਕ_ਸਾਰੇ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                                va_list param)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ UserDefined *s = &ਅੰਕੜੇ->ਸੈੱਟ;
  ਚੋਣ(option) {
  ਹਾਲਤ CURLOPT_CURLU:
    /*
     * pass CURLU to set URL
     */
    Curl_bufref_free(&ਅੰਕੜੇ->state.url);
    CURL_EASY_STR_CLEAR(ਅੰਕੜੇ, STRING_ਸੈੱਟ_URL);
    s->uh = va_arg(param, CURLU *);
    ਰੋਕੋ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FORM_API
  ਹਾਲਤ CURLOPT_HTTPPOST:
    /*
     * Set to make us do HTTP POST. Legacy API-style.
     */
    s->httppost = va_arg(param, ਬਣਤਰ curl_httppost *);
    s->ਵਿਧੀ = HTTPREQ_POST_FORM;
    s->opt_no_body = FALSE; /* this is implied */
    Curl_mime_cleanpart(ਅੰਕੜੇ->state.formp);
    curlx_safefree(ਅੰਕੜੇ->state.formp);
    ਅੰਕੜੇ->state.mimepost = ਨਿਰਾ;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_FORM_API */
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP */
#ਜੇ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_MIME
  ਹਾਲਤ CURLOPT_MIMEPOST:
    ਨਤੀਜਾ = setopt_mimepost(ਅੰਕੜੇ, va_arg(param, curl_mime *));
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_MIME */
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */
  ਹਾਲਤ CURLOPT_STDERR:
    /*
     * Set to a FILE * that should receive all error writes. This
     * defaults to stderr for normal operations.
     */
    s->err = va_arg(param, FILE *);
    ਜੇ(!s->err)
      s->err = stderr;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_SHARE:
    ਵਾਪਸ setopt_share(ਅੰਕੜੇ, va_arg(param, ਬਣਤਰ Curl_share *));

  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_COOKIES
ਸਥਿਰ CURLcode cookielist(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ptr)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਜੇ(!ptr)
    ਵਾਪਸ CURLE_OK;

  ਜੇ(curl_strequal(ptr, "ALL")) {
    /* clear all cookies */
    Curl_share_ਤਾਲਾ(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_COOKIE, CURL_ਤਾਲਾ_ACCESS_SINGLE);
    Curl_cookie_clearall(ਅੰਕੜੇ->cookies);
    Curl_share_unlock(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_COOKIE);
  }
  ਨਹੀਂਤਾਂ ਜੇ(curl_strequal(ptr, "SESS")) {
    /* clear session cookies */
    Curl_share_ਤਾਲਾ(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_COOKIE, CURL_ਤਾਲਾ_ACCESS_SINGLE);
    Curl_cookie_clearsess(ਅੰਕੜੇ->cookies);
    Curl_share_unlock(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_COOKIE);
  }
  ਨਹੀਂਤਾਂ ਜੇ(curl_strequal(ptr, "FLUSH")) {
    /* flush cookies to file, takes care of the locking */
    Curl_flush_cookies(ਅੰਕੜੇ, FALSE);
  }
  ਨਹੀਂਤਾਂ ਜੇ(curl_strequal(ptr, "RELOAD")) {
    /* reload cookies from file */
    ਵਾਪਸ Curl_cookie_loadfiles(ਅੰਕੜੇ, COOKIE_NOPSL |
                                 (ਅੰਕੜੇ->ਸੈੱਟ.cookiesession ?
                                  COOKIE_NOSESSION : 0));
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(!ਅੰਕੜੇ->cookies) {
      /* if cookie engine was not running, activate it */
      ਅੰਕੜੇ->cookies = Curl_cookie_init();
      ਜੇ(!ਅੰਕੜੇ->cookies)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਅੰਕੜੇ->state.cookie_engine = TRUE;
    }

    /* general protection against mistakes and abuse */
    ਜੇ(ਲੜੀ_ਲੰਬਾਈ(ptr) > CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

    /* Adding these cookies without the PSL check, because the PSL is not
       initialized until *perform() time, and this might be called before
       that */
    Curl_share_ਤਾਲਾ(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_COOKIE, CURL_ਤਾਲਾ_ACCESS_SINGLE);
    ਜੇ(checkprefix("Set-Cookie:", ptr))
      /* HTTP Header format line */
      ਨਤੀਜਾ = Curl_cookie_ਜੋੜੋ(ਅੰਕੜੇ, ਅੰਕੜੇ->cookies, ptr + 11,
                               ਨਿਰਾ, ਨਿਰਾ,
                               COOKIE_HTTPHEADER | COOKIE_SECURE |
                               COOKIE_NOPSL);
    ਨਹੀਂਤਾਂ
      /* Netscape format line */
      ਨਤੀਜਾ = Curl_cookie_ਜੋੜੋ(ਅੰਕੜੇ, ਅੰਕੜੇ->cookies, ptr, ਨਿਰਾ,
                               ਨਿਰਾ, COOKIE_SECURE | COOKIE_NOPSL);
    Curl_share_unlock(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_COOKIE);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cookiefile(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ptr)
{
  /*
   * Set cookie file to read and parse. Can be used multiple times.
   */
  ਜੇ(ptr) {
    ਬਣਤਰ curl_slist *cl;
    /* general protection against mistakes and abuse */
    ਜੇ(ਲੜੀ_ਲੰਬਾਈ(ptr) > CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    /* append the cookie filename to the list of filenames, and deal with
       them later */
    cl = curl_slist_append(ਅੰਕੜੇ->state.cookielist, ptr);
    ਜੇ(!cl) {
      curl_slist_free_all(ਅੰਕੜੇ->state.cookielist);
      ਅੰਕੜੇ->state.cookielist = ਨਿਰਾ;
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    ਅੰਕੜੇ->state.cookielist = cl; /* store the list for later use */
  }
  ਨਹੀਂਤਾਂ {
    /* clear the list of cookie files */
    curl_slist_free_all(ਅੰਕੜੇ->state.cookielist);
    ਅੰਕੜੇ->state.cookielist = ਨਿਰਾ;

    ਜੇ(!ਅੰਕੜੇ->share || !ਅੰਕੜੇ->share->cookies) {
      /* throw away all existing cookies if this is not a shared cookie
         container */
      Curl_cookie_clearall(ਅੰਕੜੇ->cookies);
      Curl_cookie_cleanup(ਅੰਕੜੇ->cookies);
    }
    /* disable the cookie engine */
    ਅੰਕੜੇ->cookies = ਨਿਰਾ;
  }
  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
ਸਥਿਰ CURLcode setproxy(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *proxy)
{
  ਅਚਲ ਅੱਖਰ *str = CURL_EASY_STR(ਅੰਕੜੇ, STRING_PROXY);
  ਜੇ(str && proxy &&
     /* there was one set, is this a new one? */
     !strcmp(str, proxy))
    ਵਾਪਸ CURLE_OK; /* same one as before */

  changeproxy(ਅੰਕੜੇ);
  ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_PROXY, proxy);
}

ਸਥਿਰ CURLcode setopt_cptr_proxy(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                                  ਅੱਖਰ *ptr)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ UserDefined *s = &ਅੰਕੜੇ->ਸੈੱਟ;
  ਚੋਣ(option) {
  ਹਾਲਤ CURLOPT_PROXYUSERPWD: {
    /*
     * user:password needed to use the proxy
     */
    ਅੱਖਰ *u = ਨਿਰਾ;
    ਅੱਖਰ *p = ਨਿਰਾ;
    ਨਤੀਜਾ = setstropt_userpwd(ptr, &u, &p);

    /* URL decode the components */
    ਜੇ(!ਨਤੀਜਾ) {
      ਅੱਖਰ *str = ਨਿਰਾ;
      CURL_EASY_STR_CLEAR(ਅੰਕੜੇ, STRING_PROXYUSERNAME);
      CURL_EASY_STR_CLEAR(ਅੰਕੜੇ, STRING_PROXYPASSWORD);
      ਜੇ(u) {
        ਨਤੀਜਾ = Curl_urldecode(u, 0, &str, ਨਿਰਾ, REJECT_ZERO);
        ਜੇ(!ਨਤੀਜਾ)
          ਨਤੀਜਾ = Curl_u8_strset_setn(&s->ਲੜੀਆਂ, STRING_PROXYUSERNAME, str);
      }
      ਜੇ(!ਨਤੀਜਾ && p) {
        str = ਨਿਰਾ;
        ਨਤੀਜਾ = Curl_urldecode(p, 0, &str, ਨਿਰਾ, REJECT_ZERO);
        ਜੇ(!ਨਤੀਜਾ)
          ਨਤੀਜਾ = Curl_u8_strset_setn(&s->ਲੜੀਆਂ, STRING_PROXYPASSWORD, str);
      }
    }
    curlx_free(u);
    curlx_strzero(p);
    curlx_free(p);
    ਰੋਕੋ;
  }
  ਹਾਲਤ CURLOPT_PROXYUSERNAME:
    /*
     * authentication username to use in the operation
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_PROXYUSERNAME, ptr);

  ਹਾਲਤ CURLOPT_PROXYPASSWORD:
    /*
     * authentication password to use in the operation
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_PROXYPASSWORD, ptr);

  ਹਾਲਤ CURLOPT_NOPROXY:
    /*
     * proxy exception list
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_NOPROXY, ptr);
  ਹਾਲਤ CURLOPT_PROXY_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use for proxy
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_CERT_PROXY, ptr);
  ਹਾਲਤ CURLOPT_PROXY_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use for proxy
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_CERT_TYPE_PROXY, ptr);
  ਹਾਲਤ CURLOPT_PROXY_SSLKEY:
    /*
     * String that holds filename of the SSL key to use for proxy
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_ਕੁੰਜੀ_PROXY, ptr);
  ਹਾਲਤ CURLOPT_PROXY_KEYPASSWD:
    /*
     * String that holds the SSL private key password for proxy.
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_ਕੁੰਜੀ_PASSWD_PROXY, ptr);
  ਹਾਲਤ CURLOPT_PROXY_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use for proxy
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_ਕੁੰਜੀ_TYPE_PROXY, ptr);
  ਹਾਲਤ CURLOPT_PROXY_SSL_CIPHER_LIST:
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_CIPHER_LIST)) {
      /* set a list of cipher we want to use in the SSL connection for proxy */
      ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_CIPHER_LIST_PROXY, ptr);
    }
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  ਹਾਲਤ CURLOPT_PROXY_TLS13_CIPHERS:
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites for proxy */
      ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_CIPHER13_LIST_PROXY, ptr);
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  ਹਾਲਤ CURLOPT_PROXY:
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
    ਵਾਪਸ setproxy(ਅੰਕੜੇ, ptr);
  ਹਾਲਤ CURLOPT_PRE_PROXY:
    /*
     * Set proxy server:port to use as SOCKS proxy.
     *
     * If the proxy is set to "" or NULL we explicitly say that we do not want
     * to use the socks proxy.
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_PRE_PROXY, ptr);
  ਹਾਲਤ CURLOPT_SOCKS5_GSSAPI_SERVICE:
  ਹਾਲਤ CURLOPT_PROXY_SERVICE_NAME:
    /*
     * Set proxy authentication service name for Kerberos 5 and SPNEGO
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_PROXY_SERVICE_NAME, ptr);
  ਹਾਲਤ CURLOPT_PROXY_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_PINNEDPUBKEY))
      ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_PINNEDPUBLICKEY_PROXY, ptr);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;

  ਹਾਲਤ CURLOPT_HAPROXY_CLIENT_IP:
    /*
     * Set the client IP to send through HAProxy PROXY protocol
     */
    ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_HAPROXY_CLIENT_IP, ptr);

    /* enable the HAProxy protocol if an IP is provided */
    s->haproxyprotocol = !!CURL_EASY_STR(ਅੰਕੜੇ, STRING_HAPROXY_CLIENT_IP);
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_PROXY_CAINFO:
    /*
     * Set CA info SSL connection for proxy. Specify filename of the
     * CA certificate
     */
    ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_CAFILE_PROXY, ptr);
    ਜੇ(!ਨਤੀਜਾ)
      s->proxy_ssl.custom_cafile = TRUE;
    ਵਾਪਸ ਨਤੀਜਾ;
  ਹਾਲਤ CURLOPT_PROXY_CRLFILE:
    /*
     * Set CRL file info for SSL connection for proxy. Specify filename of the
     * CRL to check certificates revocation
     */
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_CRLFILE))
      ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_CRLFILE_PROXY, ptr);
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  ਹਾਲਤ CURLOPT_PROXY_ISSUERCERT:
    /*
     * Set Issuer certificate file to check certificates issuer
     */
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_ISSUERCERT))
      ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_ISSUERCERT_PROXY, ptr);
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  ਹਾਲਤ CURLOPT_PROXY_CAPATH:
    /*
     * Set CA path info for SSL connection proxy. Specify directory name of the
     * CA certificates which have been prepared using openssl c_rehash utility.
     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_CA_ਰਸਤਾ)) {
      /* This does not work on Windows. */
      ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_CAPATH_PROXY, ptr);
      ਜੇ(!ਨਤੀਜਾ)
        s->proxy_ssl.custom_capath = TRUE;
      ਵਾਪਸ ਨਤੀਜਾ;
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
/*
 * A string with POST data. Makes curl HTTP POST. Even if it is NULL. If
 * needed, CURLOPT_POSTFIELDSIZE must have been set prior to
 * CURLOPT_COPYPOSTFIELDS and not altered later.
 */
ਸਥਿਰ CURLcode setopt_copypostfields(ਅਚਲ ਅੱਖਰ *ptr, ਬਣਤਰ UserDefined *s)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਜੇ(s->postfieldsize < -1)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਜੇ(!ptr || s->postfieldsize == -1) {
    ਜੇ(ptr && (ਲੜੀ_ਲੰਬਾਈ(ptr) > CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ))
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    curlx_safefree(s->str_copypostfields);
    ਜੇ(ptr) {
      s->str_copypostfields = curlx_strdup(ptr);
      ਜੇ(!s->str_copypostfields)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }
  ਨਹੀਂਤਾਂ {
    size_t pflen = curlx_sotouz_range(s->postfieldsize, 0, SIZE_MAX);
    ਜੇ(pflen == SIZE_MAX)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਨਹੀਂਤਾਂ {
      /* Allocate even when size == 0. This satisfies the need of possible
         later address compare to detect the COPYPOSTFIELDS mode, and to mark
         that postfields is used rather than read function or form data. */
      ਅੱਖਰ *p = curlx_memdup0(ptr, pflen);
      ਜੇ(!p)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਨਹੀਂਤਾਂ {
        curlx_free(s->str_copypostfields);
        s->str_copypostfields = p;
      }
    }
  }

  s->postfields = s->str_copypostfields;
  s->ਵਿਧੀ = HTTPREQ_POST;
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ECH
ਸਥਿਰ CURLcode setopt_ech(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ptr)
{
  ਬਣਤਰ UserDefined *s = &ਅੰਕੜੇ->ਸੈੱਟ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!ptr || !strcmp(ptr, "false"))
    s->ssl.ech = CURLECH_DISABLE;
  ਨਹੀਂਤਾਂ {
    size_t plen = ਲੜੀ_ਲੰਬਾਈ(ptr);
    ਜੇ(plen > CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ)
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ {
      ਜੇ(!strcmp(ptr, "grease"))
        s->ssl.ech = CURLECH_GREASE;
      ਨਹੀਂਤਾਂ ਜੇ(!strcmp(ptr, "true"))
        s->ssl.ech = CURLECH_ENABLE;
      ਨਹੀਂਤਾਂ ਜੇ(!strcmp(ptr, "hard"))
        s->ssl.ech = CURLECH_HARD;
      ਨਹੀਂਤਾਂ ਜੇ(plen > 4 && !ਲੜੀ_ਤੁਲਨਾ(ptr, "ecl:", 4)) {
        ਜੇ(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_ECH_CONFIG, ptr + 4);
      }
      ਨਹੀਂਤਾਂ ਜੇ(plen > 3 && !ਲੜੀ_ਤੁਲਨਾ(ptr, "pn:", 3)) {
        ਜੇ(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_ECH_ਜਨਤਕ, ptr + 3);
      }
      ਨਹੀਂਤਾਂ
        ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ setopt_ech(x, y) CURLE_NOT_BUILT_ਵਿੱਚ
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_SSL) || defined(USE_SSH)
/* One of the options is used for both TLS and SSH */
ਸਥਿਰ CURLcode setopt_cptr_ssl(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                                ਅੱਖਰ *ptr)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਚੋਣ(option) {
  ਹਾਲਤ CURLOPT_KEYPASSWD:
    /*
     * String that holds the SSL or SSH private key password.
     */
    ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_ਕੁੰਜੀ_PASSWD, ptr);
    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  ਹਾਲਤ CURLOPT_CAINFO:
    /*
     * Set CA info for SSL connection. Specify filename of the CA certificate
     */
    ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_CAFILE, ptr);
    ਜੇ(!ਨਤੀਜਾ)
      ਅੰਕੜੇ->ਸੈੱਟ.ssl.custom_cafile = TRUE;
    ਵਾਪਸ ਨਤੀਜਾ;
  ਹਾਲਤ CURLOPT_CAPATH:
    /*
     * Set CA path info for SSL connection. Specify directory name of the CA
     * certificates which have been prepared using openssl c_rehash utility.
     */
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_CA_ਰਸਤਾ)) {
      /* This does not work on Windows. */
      ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_CAPATH, ptr);
      ਜੇ(!ਨਤੀਜਾ)
        ਅੰਕੜੇ->ਸੈੱਟ.ssl.custom_capath = TRUE;
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  ਹਾਲਤ CURLOPT_CRLFILE:
    /*
     * Set CRL file info for SSL connection. Specify filename of the CRL
     * to check certificates revocation
     */
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_CRLFILE))
      ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_CRLFILE, ptr);
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  ਹਾਲਤ CURLOPT_SSL_CIPHER_LIST:
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_CIPHER_LIST))
      /* set a list of cipher we want to use in the SSL connection */
      ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_CIPHER_LIST, ptr);
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  ਹਾਲਤ CURLOPT_TLS13_CIPHERS:
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites */
      ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_CIPHER13_LIST, ptr);
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  ਹਾਲਤ CURLOPT_RANDOM_FILE:
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_EGDSOCKET:
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_SSL_CTX_ਅੰਕੜੇ:
    /*
     * Set an SSL_CTX callback parameter pointer
     */
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_SSL_CTX)) {
      ਅੰਕੜੇ->ਸੈੱਟ.ssl_fsslctxp = ptr;
      ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  ਹਾਲਤ CURLOPT_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_CERT, ptr);
  ਹਾਲਤ CURLOPT_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_CERT_TYPE, ptr);
  ਹਾਲਤ CURLOPT_SSLKEY:
    /*
     * String that holds filename of the SSL key to use
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_ਕੁੰਜੀ, ptr);
  ਹਾਲਤ CURLOPT_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_ਕੁੰਜੀ_TYPE, ptr);
  ਹਾਲਤ CURLOPT_SSLENGINE:
    /*
     * String that holds the SSL crypto engine.
     */
    ਜੇ(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
    ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_ENGINE, ptr);
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_ISSUERCERT:
    /*
     * Set Issuer certificate file
     * to check certificates issuer
     */
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_ISSUERCERT))
      ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_ISSUERCERT, ptr);
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  ਹਾਲਤ CURLOPT_SSL_EC_CURVES:
    /*
     * Set accepted curves in SSL connection setup.
     * Specify colon-delimited list of curve algorithm names.
     */
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_SSL_EC_CURVES))
      ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_EC_CURVES, ptr);
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  ਹਾਲਤ CURLOPT_SSL_SIGNATURE_ALGORITHMS:
    /*
     * Set accepted signature algorithms.
     * Specify colon-delimited list of signature scheme names.
     */
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_SIGNATURE_ALGORITHMS))
      ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_SIGNATURE_ALGORITHMS, ptr);
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  ਹਾਲਤ CURLOPT_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_PINNEDPUBKEY))
      ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_SSL_PINNEDPUBLICKEY, ptr);
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  ਹਾਲਤ CURLOPT_ECH:
    ਵਾਪਸ setopt_ech(ਅੰਕੜੇ, ptr);
#ਜੇ_ਅੰਤ
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
ਸਥਿਰ CURLcode setopt_cptr_http_mqtt(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      CURLoption option, ਅੱਖਰ *ptr)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ UserDefined *s = &ਅੰਕੜੇ->ਸੈੱਟ;

  ਚੋਣ(option) {
  ਹਾਲਤ CURLOPT_COPYPOSTFIELDS:
    ਵਾਪਸ setopt_copypostfields(ptr, s);

  ਹਾਲਤ CURLOPT_POSTFIELDS:
    /*
     * Like above, but use static data instead of copying it.
     */
    s->postfields = ptr;
    /* Release old copied data. */
    curlx_safefree(s->str_copypostfields);
    s->ਵਿਧੀ = HTTPREQ_POST;
    ਰੋਕੋ;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  ਹਾਲਤ CURLOPT_TRAILERDATA:
    s->trailer_ਅੰਕੜੇ = ptr;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_ACCEPT_ENCODING:
    /*
     * String to use at the value of Accept-Encoding header.
     *
     * If the encoding is set to "" we use an Accept-Encoding header that
     * encompasses all the encodings we support.
     * If the encoding is set to NULL we do not send an Accept-Encoding header
     * and ignore any received Content-Encoding header.
     *
     */
    ਜੇ(ptr && !*ptr) {
      ptr = Curl_get_content_encodings();
      ਜੇ(ptr)
        ਨਤੀਜਾ = CURL_EASY_STR_SETN(ਅੰਕੜੇ, STRING_ENCODING, ptr);
      ਨਹੀਂਤਾਂ
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_ENCODING, ptr);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_AWS
  ਹਾਲਤ CURLOPT_AWS_SIGV4:
    /*
     * String that is merged to some authentication
     * parameters are used by the algorithm.
     */
    ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_AWS_SIGV4, ptr);
    /*
     * Basic has been set by default; it needs to be unset here.
     */
    ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_AWS_SIGV4))
      s->httpauth = CURLAUTH_AWS_SIGV4;
    ਨਹੀਂਤਾਂ
      s->httpauth &= ~(uint32_t)CURLAUTH_AWS_SIGV4;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTPSIG
  ਹਾਲਤ CURLOPT_HTTPSIG_ਕੁੰਜੀ:
    ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_HTTPSIG_ਕੁੰਜੀ, ptr);
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_HTTPSIG_KEYID:
    ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_HTTPSIG_KEYID, ptr);
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_HTTPSIG_ਸਿਰਲੇਖਾਂ:
    ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_HTTPSIG_ਸਿਰਲੇਖਾਂ, ptr);
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਹਾਲਤ CURLOPT_REFERER: {
    /*
     * String to set in the HTTP Referer: field.
     */
    ਬਣਤਰ bufref *oldref = &ਅੰਕੜੇ->state.referer;
    /* free the old after the storing the new in case the input is actually
       pointing back to this */
    ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_ਸੈੱਟ_REFERER, ptr);
    Curl_bufref_free(oldref);
    ਰੋਕੋ;
  }

  ਹਾਲਤ CURLOPT_USERAGENT:
    /*
     * String to use in the HTTP User-Agent field
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_USERAGENT, ptr);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_COOKIES
  ਹਾਲਤ CURLOPT_COOKIE:
    /*
     * Cookie string to send to the remote server in the request.
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_COOKIE, ptr);

  ਹਾਲਤ CURLOPT_COOKIEFILE:
    ਵਾਪਸ cookiefile(ਅੰਕੜੇ, ptr);

  ਹਾਲਤ CURLOPT_COOKIEJAR:
    /*
     * Set cookie filename to dump all cookies to when we are done.
     */
    ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_COOKIEJAR, ptr);
    ਜੇ(!ਨਤੀਜਾ) {
      /*
       * Activate the cookie parser. This may or may not already
       * have been made.
       */
      ਜੇ(!ਅੰਕੜੇ->cookies)
        ਅੰਕੜੇ->cookies = Curl_cookie_init();
      ਜੇ(!ਅੰਕੜੇ->cookies)
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਨਹੀਂਤਾਂ
        ਅੰਕੜੇ->state.cookie_engine = TRUE;
    }
    ਰੋਕੋ;

  ਹਾਲਤ CURLOPT_COOKIELIST:
    ਵਾਪਸ cookielist(ਅੰਕੜੇ, ptr);
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_COOKIES */

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP */
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP || !CURL_DISABLE_MQTT */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
ਸਥਿਰ CURLcode setopt_cptr_ssh(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                                ਅੱਖਰ *ptr)
{
  ਬਣਤਰ UserDefined *s = &ਅੰਕੜੇ->ਸੈੱਟ;
  ਚੋਣ(option) {
  ਹਾਲਤ CURLOPT_SSH_ਜਨਤਕ_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa.pub file
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_SSH_ਜਨਤਕ_ਕੁੰਜੀ, ptr);
  ਹਾਲਤ CURLOPT_SSH_ਨਿੱਜੀ_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa file
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_SSH_ਨਿੱਜੀ_ਕੁੰਜੀ, ptr);
  ਹਾਲਤ CURLOPT_SSH_KEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_keyfunc_userp = ptr;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_SSH_HOST_ਜਨਤਕ_ਕੁੰਜੀ_MD5:
    /*
     * Option to allow for the MD5 of the host public key to be checked
     * for validation purposes.
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_SSH_HOST_ਜਨਤਕ_ਕੁੰਜੀ_MD5, ptr);
  ਹਾਲਤ CURLOPT_SSH_HOST_ਜਨਤਕ_ਕੁੰਜੀ_SHA256:
    /*
     * Option to allow for the SHA256 of the host public key to be checked
     * for validation purposes.
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_SSH_HOST_ਜਨਤਕ_ਕੁੰਜੀ_SHA256, ptr);
  ਹਾਲਤ CURLOPT_SSH_KNOWNHOSTS:
    /*
     * Store the filename to read known hosts from.
     */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_SSH_KNOWNHOSTS, ptr);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBSSH2
  ਹਾਲਤ CURLOPT_SSH_HOSTKEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_hostkeyfunc_userp = ptr;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ /* USE_LIBSSH2 */
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }
  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ /* USE_SSH */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
ਸਥਿਰ CURLcode setopt_cptr_ftp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                                ਅੱਖਰ *ptr)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ UserDefined *s = &ਅੰਕੜੇ->ਸੈੱਟ;
  ਚੋਣ(option) {
  ਹਾਲਤ CURLOPT_FTPPORT:
    /*
     * Use FTP PORT, this also specifies which IP address to use
     */
    ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_FTPPORT, ptr);
    s->ftp_use_ਪੋਰਟ = !!CURL_EASY_STR(ਅੰਕੜੇ, STRING_FTPPORT);
    ਰੋਕੋ;

  ਹਾਲਤ CURLOPT_FTP_ACCOUNT:
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_FTP_ACCOUNT, ptr);

  ਹਾਲਤ CURLOPT_FTP_ALTERNATIVE_ਮੰਜ਼ਿਲ_USER:
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_FTP_ALTERNATIVE_ਮੰਜ਼ਿਲ_USER, ptr);

  ਹਾਲਤ CURLOPT_KRBLEVEL:
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ; /* removed in 8.17.0 */
  ਹਾਲਤ CURLOPT_CHUNK_ਅੰਕੜੇ:
    s->wildcardptr = ptr;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_FNMATCH_ਅੰਕੜੇ:
    s->fnmatch_ਅੰਕੜੇ = ptr;
    ਰੋਕੋ;
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_FTP */

ਸਥਿਰ CURLcode setopt_cptr_net(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                                ਅੱਖਰ *ptr)
{
  ਚੋਣ(option) {
  ਹਾਲਤ CURLOPT_INTERFACE:
    /*
     * Set what interface or address/hostname to bind the socket to when
     * performing an operation and thus what from-IP your connection will use.
     */
    ਵਾਪਸ setstropt_interface(ਅੰਕੜੇ, ptr);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLV_ARES
  ਹਾਲਤ CURLOPT_DNS_SERVERS:
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_DNS_SERVERS, ptr);

  ਹਾਲਤ CURLOPT_DNS_INTERFACE:
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_DNS_INTERFACE, ptr);

  ਹਾਲਤ CURLOPT_DNS_LOCAL_IP4:
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_DNS_LOCAL_IP4, ptr);

  ਹਾਲਤ CURLOPT_DNS_LOCAL_IP6:
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_DNS_LOCAL_IP6, ptr);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
  ਹਾਲਤ CURLOPT_UNIX_SOCKET_ਰਸਤਾ:
    ਅੰਕੜੇ->ਸੈੱਟ.abstract_unix_socket = FALSE;
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_UNIX_SOCKET_ਰਸਤਾ, ptr);

  ਹਾਲਤ CURLOPT_ABSTRACT_UNIX_SOCKET:
    ਅੰਕੜੇ->ਸੈੱਟ.abstract_unix_socket = TRUE;
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_UNIX_SOCKET_ਰਸਤਾ, ptr);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DOH
  ਹਾਲਤ CURLOPT_DOH_URL:
    {
      CURLcode ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_DOH, ptr);
      ਅੰਕੜੇ->ਸੈੱਟ.doh = !!CURL_EASY_STR(ਅੰਕੜੇ, STRING_DOH);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
#ਜੇ_ਅੰਤ
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }
}

ਸਥਿਰ CURLcode setopt_cptr_misc(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                                 ਅੱਖਰ *ptr)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ UserDefined *s = &ਅੰਕੜੇ->ਸੈੱਟ;

  ਚੋਣ(option) {
  ਹਾਲਤ CURLOPT_REQUEST_ਨਿਸ਼ਾਨਾ:
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_ਨਿਸ਼ਾਨਾ, ptr);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_NETRC
  ਹਾਲਤ CURLOPT_NETRC_FILE:
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_NETRC_FILE, ptr);
#ਜੇ_ਅੰਤ
  ਹਾਲਤ CURLOPT_CUSTOMREQUEST:
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_CUSTOMREQUEST, ptr);

    /* we do not set s->method = HTTPREQ_CUSTOM; here, we continue as if we
       were using the already set type and this changes the actual request
       keyword */
  ਹਾਲਤ CURLOPT_SERVICE_NAME:
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_SERVICE_NAME, ptr);

  ਹਾਲਤ CURLOPT_HEADERDATA:
    s->writeheader = ptr;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_READDATA:
    s->ਵਿੱਚ_ਸੈੱਟ = ptr;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_WRITEDATA:
    s->ਬਾਹਰਲਾ = ptr;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_DEBUGDATA:
    s->debugdata = ptr;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_PROGRESSDATA:
    s->ਤਰੱਕੀ_client = ptr;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_SEEKDATA:
    s->seek_client = ptr;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_IOCTLDATA:
    s->ioctl_client = ptr;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_SOCKOPTDATA:
    s->sockopt_client = ptr;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_OPENSOCKETDATA:
    s->opensocket_client = ptr;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_RESOLVER_ਸ਼ੁਰੂ_ਅੰਕੜੇ:
    s->resolver_ਸ਼ੁਰੂ_client = ptr;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_CLOSESOCKETDATA:
    s->closesocket_client = ptr;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_PREREQDATA:
    s->prereq_userp = ptr;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_ERRORBUFFER:
    s->errorbuffer = ptr;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_URL:
    ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_ਸੈੱਟ_URL, ptr);
    Curl_bufref_ਸੈੱਟ(&ਅੰਕੜੇ->state.url,
                    CURL_EASY_STR(ਅੰਕੜੇ, STRING_ਸੈੱਟ_URL), 0, ਨਿਰਾ);
    ਰੋਕੋ;

  ਹਾਲਤ CURLOPT_USERPWD: {
    ਅੱਖਰ *u = ਨਿਰਾ, *p = ਨਿਰਾ;
    ਨਤੀਜਾ = setstropt_userpwd(ptr, &u, &p);
    ਜੇ(!ਨਤੀਜਾ) {
      ਨਤੀਜਾ = CURL_EASY_STR_SETN(ਅੰਕੜੇ, STRING_USERNAME, u);
      u = ਨਿਰਾ;
    }
    ਜੇ(!ਨਤੀਜਾ) {
      ਨਤੀਜਾ = CURL_EASY_STR_SETN(ਅੰਕੜੇ, STRING_PASSWORD, p);
      p = ਨਿਰਾ;
    }
    curlx_free(u);
    curlx_free(p);
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਹਾਲਤ CURLOPT_USERNAME:
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_USERNAME, ptr);

  ਹਾਲਤ CURLOPT_PASSWORD:
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_PASSWORD, ptr);

  ਹਾਲਤ CURLOPT_LOGIN_OPTIONS:
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_OPTIONS, ptr);

  ਹਾਲਤ CURLOPT_XOAUTH2_BEARER:
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_BEARER, ptr);
  ਹਾਲਤ CURLOPT_RANGE:
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_ਸੈੱਟ_RANGE, ptr);
  ਹਾਲਤ CURLOPT_ਨਿੱਜੀ:
    s->ਨਿੱਜੀ_ਅੰਕੜੇ = ptr;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_PROTOCOLS_STR:
    ਜੇ(ptr) {
      curl_prot_t protos;
      ਨਤੀਜਾ = protocol2num(ptr, &protos);
      ਜੇ(!ਨਤੀਜਾ)
        s->allowed_protocols = protos;
    }
    ਨਹੀਂਤਾਂ
      /* make a NULL argument reset to default */
      s->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_REDIR_PROTOCOLS_STR:
    ਜੇ(ptr) {
      curl_prot_t protos;
      ਨਤੀਜਾ = protocol2num(ptr, &protos);
      ਜੇ(!ਨਤੀਜਾ)
        s->redir_protocols = protos;
    }
    ਨਹੀਂਤਾਂ
      /* make a NULL argument reset to default */
      s->redir_protocols = (curl_prot_t)CURLPROTO_REDIR;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_DEFAULT_PROTOCOL:
    /* Set the protocol to use when the URL does not include any protocol */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_DEFAULT_PROTOCOL, ptr);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_SMTP
  ਹਾਲਤ CURLOPT_MAIL_FROM:
    /* Set the SMTP mail originator */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_MAIL_FROM, ptr);
  ਹਾਲਤ CURLOPT_MAIL_AUTH:
    /* Set the SMTP auth originator */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_MAIL_AUTH, ptr);
#ਜੇ_ਅੰਤ
  ਹਾਲਤ CURLOPT_SASL_AUTHZID:
    /* Authorization identity (identity to act as) */
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_SASL_AUTHZID, ptr);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_RTSP
  ਹਾਲਤ CURLOPT_RTSP_SESSION_ID:
    Curl_peer_unlink(&ਅੰਕੜੇ->state.rtsp_session_origin);
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_RTSP_SESSION_ID, ptr);
  ਹਾਲਤ CURLOPT_RTSP_ਧਾਰਾ_URI:
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_RTSP_ਧਾਰਾ_URI, ptr);
  ਹਾਲਤ CURLOPT_RTSP_TRANSPORT:
    ਵਾਪਸ Curl_setstropt(ਅੰਕੜੇ, STRING_RTSP_TRANSPORT, ptr);
  ਹਾਲਤ CURLOPT_INTERLEAVEDATA:
    s->rtp_ਬਾਹਰਲਾ = ptr;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_RTSP */
  ਹਾਲਤ CURLOPT_TLSAUTH_USERNAME:
  ਹਾਲਤ CURLOPT_TLSAUTH_PASSWORD:
  ਹਾਲਤ CURLOPT_TLSAUTH_TYPE:
  ਹਾਲਤ CURLOPT_PROXY_TLSAUTH_USERNAME:
  ਹਾਲਤ CURLOPT_PROXY_TLSAUTH_PASSWORD:
  ਹਾਲਤ CURLOPT_PROXY_TLSAUTH_TYPE:
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HSTS
  ਹਾਲਤ CURLOPT_HSTSREADDATA:
    s->hsts_read_userp = ptr;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_HSTSWRITEDATA:
    s->hsts_write_userp = ptr;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_HSTS: {
    ਬਣਤਰ curl_slist *h;
    ਜੇ(!ਅੰਕੜੇ->hsts) {
      ਅੰਕੜੇ->hsts = Curl_hsts_init();
      ਜੇ(!ਅੰਕੜੇ->hsts)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    ਜੇ(ptr) {
      ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_HSTS, ptr);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      /* this needs to build a list of filenames to read from, so that it can
         read them later, as we might get a shared HSTS handle to load them
         into */
      h = curl_slist_append(ਅੰਕੜੇ->state.hstslist, ptr);
      ਜੇ(!h) {
        curl_slist_free_all(ਅੰਕੜੇ->state.hstslist);
        ਅੰਕੜੇ->state.hstslist = ਨਿਰਾ;
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
      ਅੰਕੜੇ->state.hstslist = h; /* store the list for later use */
    }
    ਨਹੀਂਤਾਂ {
      /* clear the list of HSTS files */
      curl_slist_free_all(ਅੰਕੜੇ->state.hstslist);
      ਅੰਕੜੇ->state.hstslist = ਨਿਰਾ;
      ਜੇ(!ਅੰਕੜੇ->share || !ਅੰਕੜੇ->share->hsts)
        /* throw away the HSTS cache unless shared */
        Curl_hsts_cleanup(&ਅੰਕੜੇ->hsts);
    }
    ਰੋਕੋ;
  }
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HSTS */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_ALTSVC
  ਹਾਲਤ CURLOPT_ALTSVC:
    ਜੇ(!ਅੰਕੜੇ->asi) {
      ਅੰਕੜੇ->asi = Curl_altsvc_init();
      ਜੇ(!ਅੰਕੜੇ->asi)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    ਨਤੀਜਾ = Curl_setstropt(ਅੰਕੜੇ, STRING_ALTSVC, ptr);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;
    ਜੇ(ptr)
      ਵਾਪਸ Curl_altsvc_ਲੱਦੋ(ਅੰਕੜੇ->asi, ptr);
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_ALTSVC */
  ਹਾਲਤ CURLOPT_ECH:
    ਵਾਪਸ setopt_ech(ਅੰਕੜੇ, ptr);
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode setopt_cptr(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                            ਅੱਖਰ *ptr)
{
  ਕਿਸਮਪਰਿਭਾਸ਼ਾ CURLcode (*ptrfunc)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                              ਅੱਖਰ *ptr);
  /* Order by likeliness */
  ਸਥਿਰ ਅਚਲ ptrfunc setopt_call[] = {
    setopt_cptr_misc,
#ਜੇ defined(USE_SSL) || defined(USE_SSH)
    setopt_cptr_ssl,
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
    setopt_cptr_proxy,
#ਜੇ_ਅੰਤ
    setopt_cptr_net,
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
    setopt_cptr_ftp,
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
    setopt_cptr_ssh,
#ਜੇ_ਅੰਤ
#ਜੇ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
    setopt_cptr_http_mqtt,
#ਜੇ_ਅੰਤ
  };
  size_t i;

  ਲਈ(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode ਨਤੀਜਾ = setopt_call[i](ਅੰਕੜੇ, option, ptr);
    ਜੇ(ਨਤੀਜਾ != CURLE_UNKNOWN_OPTION)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਵਾਪਸ CURLE_UNKNOWN_OPTION;
}

ਸਥਿਰ CURLcode setopt_func(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                            va_list param)
{
  ਬਣਤਰ UserDefined *s = &ਅੰਕੜੇ->ਸੈੱਟ;
  ਚੋਣ(option) {
  ਹਾਲਤ CURLOPT_PROGRESSFUNCTION:
    /*
     * Progress callback function
     */
    s->fprogress = va_arg(param, curl_ਤਰੱਕੀ_callback);
    ਜੇ(s->fprogress)
      ਅੰਕੜੇ->ਤਰੱਕੀ.callback = TRUE; /* no longer internal */
    ਨਹੀਂਤਾਂ
      ਅੰਕੜੇ->ਤਰੱਕੀ.callback = FALSE; /* NULL enforces internal */
    ਰੋਕੋ;

  ਹਾਲਤ CURLOPT_XFERINFOFUNCTION:
    /*
     * Transfer info callback function
     */
    s->fxferinfo = va_arg(param, curl_xferinfo_callback);
    ਜੇ(s->fxferinfo)
      ਅੰਕੜੇ->ਤਰੱਕੀ.callback = TRUE; /* no longer internal */
    ਨਹੀਂਤਾਂ
      ਅੰਕੜੇ->ਤਰੱਕੀ.callback = FALSE; /* NULL enforces internal */

    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_DEBUGFUNCTION:
    /*
     * stderr write callback.
     */
    s->fdebug = va_arg(param, curl_debug_callback);
    /*
     * if the callback provided is NULL, it will use the default callback
     */
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_HEADERFUNCTION:
    /*
     * Set header write callback
     */
    s->fwrite_ਸਿਰਲੇਖ = va_arg(param, curl_write_callback);
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_WRITEFUNCTION:
    /*
     * Set data write callback
     */
    s->fwrite_func = va_arg(param, curl_write_callback);
    ਜੇ(!s->fwrite_func)
#ਜੇ defined(__clang__) && __clang_major__ >= 16
#ਹਦਾਇਤ clang diagnostic ਧੱਕੋ
#ਹਦਾਇਤ clang diagnostic ignored "-Wcast-function-type-strict"
#ਜੇ_ਅੰਤ
      /* When set to NULL, reset to our internal default function */
      s->fwrite_func = (curl_write_callback)fwrite;
#ਜੇ defined(__clang__) && __clang_major__ >= 16
#ਹਦਾਇਤ clang diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_READFUNCTION:
    /*
     * Read data callback
     */
    s->fread_func_ਸੈੱਟ = va_arg(param, curl_read_callback);
    ਜੇ(!s->fread_func_ਸੈੱਟ) {
      s->is_fread_ਸੈੱਟ = 0;
#ਜੇ defined(__clang__) && __clang_major__ >= 16
#ਹਦਾਇਤ clang diagnostic ਧੱਕੋ
#ਹਦਾਇਤ clang diagnostic ignored "-Wcast-function-type-strict"
#ਜੇ_ਅੰਤ
      /* When set to NULL, reset to our internal default function */
      s->fread_func_ਸੈੱਟ = (curl_read_callback)fread;
#ਜੇ defined(__clang__) && __clang_major__ >= 16
#ਹਦਾਇਤ clang diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
      s->is_fread_ਸੈੱਟ = 1;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_SEEKFUNCTION:
    /*
     * Seek callback. Might be NULL.
     */
    s->seek_func = va_arg(param, curl_seek_callback);
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_IOCTLFUNCTION:
    /*
     * I/O control callback. Might be NULL.
     */
    s->ioctl_func = va_arg(param, curl_ioctl_callback);
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_SSL_CTX_FUNCTION:
    /*
     * Set an SSL_CTX callback
     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_SSL_CTX)) {
      s->ssl_fsslctx = va_arg(param, curl_ssl_ctx_callback);
      ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
      ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;

  ਹਾਲਤ CURLOPT_SOCKOPTFUNCTION:
    /*
     * socket callback function: called after socket() but before connect()
     */
    s->fsockopt = va_arg(param, curl_sockopt_callback);
    ਰੋਕੋ;

  ਹਾਲਤ CURLOPT_OPENSOCKETFUNCTION:
    /*
     * open/create socket callback function: called instead of socket(),
     * before connect()
     */
    s->fopensocket = va_arg(param, curl_opensocket_callback);
    ਰੋਕੋ;

  ਹਾਲਤ CURLOPT_CLOSESOCKETFUNCTION:
    /*
     * close socket callback function: called instead of close()
     * when shutting down a connection
     */
    s->fclosesocket = va_arg(param, curl_closesocket_callback);
    ਰੋਕੋ;

  ਹਾਲਤ CURLOPT_RESOLVER_ਸ਼ੁਰੂ_FUNCTION:
    /*
     * resolver start callback function: called before a new resolver request
     * is started
     */
    s->resolver_ਸ਼ੁਰੂ = va_arg(param, curl_resolver_ਸ਼ੁਰੂ_callback);
    ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBSSH2
  ਹਾਲਤ CURLOPT_SSH_HOSTKEYFUNCTION:
    /* the callback to check the hostkey without the knownhost file */
    s->ssh_hostkeyfunc = va_arg(param, curl_sshhostkeycallback);
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

  ਹਾਲਤ CURLOPT_SSH_KEYFUNCTION:
    /* setting to NULL is fine since the ssh.c functions themselves will
       then revert to use the internal default */
    s->ssh_keyfunc = va_arg(param, curl_sshkeycallback);
    ਰੋਕੋ;

#ਜੇ_ਅੰਤ /* USE_SSH */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_RTSP
  ਹਾਲਤ CURLOPT_INTERLEAVEFUNCTION:
    /* Set the user defined RTP write function */
    s->fwrite_rtp = va_arg(param, curl_write_callback);
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
  ਹਾਲਤ CURLOPT_CHUNK_BGN_FUNCTION:
    s->chunk_bgn = va_arg(param, curl_chunk_bgn_callback);
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_CHUNK_ਸਿਰਾ_FUNCTION:
    s->chunk_ਸਿਰਾ = va_arg(param, curl_chunk_ਸਿਰਾ_callback);
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_FNMATCH_FUNCTION:
    s->fnmatch = va_arg(param, curl_fnmatch_callback);
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  ਹਾਲਤ CURLOPT_TRAILERFUNCTION:
    s->trailer_callback = va_arg(param, curl_trailer_callback);
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HSTS
  ਹਾਲਤ CURLOPT_HSTSREADFUNCTION:
    s->hsts_read = va_arg(param, curl_hstsread_callback);
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_HSTSWRITEFUNCTION:
    s->hsts_write = va_arg(param, curl_hstswrite_callback);
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਹਾਲਤ CURLOPT_PREREQFUNCTION:
    s->fprereq = va_arg(param, curl_prereq_callback);
    ਰੋਕੋ;
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode setopt_offt(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                            curl_off_t offt)
{
  ਬਣਤਰ UserDefined *s = &ਅੰਕੜੇ->ਸੈੱਟ;
  ਚੋਣ(option) {
  ਹਾਲਤ CURLOPT_TIMEVALUE_LARGE:
    /*
     * This is the value to compare with the remote document with the
     * method set with CURLOPT_TIMECONDITION
     */
    s->timevalue = (time_t)offt;
    ਰੋਕੋ;

    /* MQTT "borrows" some of the HTTP options */
  ਹਾਲਤ CURLOPT_POSTFIELDSIZE_LARGE:
    /*
     * The size of the POSTFIELD data to prevent libcurl to do strlen() to
     * figure it out. Enables binary posts.
     */
    ਜੇ(offt < -1)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

    ਜੇ(s->postfieldsize < offt && s->str_copypostfields) {
      /* Previous CURLOPT_COPYPOSTFIELDS is no longer valid. */
      curlx_safefree(s->str_copypostfields);
      s->postfields = ਨਿਰਾ;
    }
    s->postfieldsize = offt;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_INFILESIZE_LARGE:
    /*
     * If known, this should inform curl about the file size of the
     * to-be-uploaded file.
     */
    ਜੇ(offt < -1)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    s->filesize = offt;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_MAX_SEND_ਰਫਤਾਰ_LARGE:
    /*
     * When transfer uploads are faster than CURLOPT_MAX_SEND_SPEED_LARGE
     * bytes per second the transfer is throttled..
     */
    ਜੇ(offt < 0)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    s->max_send_ਰਫਤਾਰ = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&ਅੰਕੜੇ->ਤਰੱਕੀ.ul.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_MAX_RECV_ਰਫਤਾਰ_LARGE:
    /*
     * When receiving data faster than CURLOPT_MAX_RECV_SPEED_LARGE bytes per
     * second the transfer is throttled..
     */
    ਜੇ(offt < 0)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    s->max_recv_ਰਫਤਾਰ = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&ਅੰਕੜੇ->ਤਰੱਕੀ.dl.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_RESUME_FROM_LARGE:
    /*
     * Resume transfer at the given file position
     */
    ਜੇ(offt < -1)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    s->ਸੈੱਟ_resume_from = offt;
    ਰੋਕੋ;
  ਹਾਲਤ CURLOPT_MAXFILESIZE_LARGE:
    /*
     * Set the maximum size of a file to download.
     */
    ਜੇ(offt < 0)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    s->max_filesize = offt;
    ਰੋਕੋ;

  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode setopt_blob(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                            ਬਣਤਰ curl_blob *blob)
{
  ਬਣਤਰ UserDefined *s = &ਅੰਕੜੇ->ਸੈੱਟ;
  ਚੋਣ(option) {
  ਹਾਲਤ CURLOPT_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use
     */
    ਵਾਪਸ Curl_setblobopt(&s->blobs[BLOB_CERT], blob);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਹਾਲਤ CURLOPT_PROXY_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use for proxy
     */
    ਵਾਪਸ Curl_setblobopt(&s->blobs[BLOB_CERT_PROXY], blob);
  ਹਾਲਤ CURLOPT_PROXY_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use for proxy
     */
    ਵਾਪਸ Curl_setblobopt(&s->blobs[BLOB_ਕੁੰਜੀ_PROXY], blob);
  ਹਾਲਤ CURLOPT_PROXY_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection proxy.
     * Specify entire PEM of the CA certificate
     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_CAINFO_BLOB)) {
      CURLcode ਨਤੀਜਾ = Curl_setblobopt(&s->blobs[BLOB_CAINFO_PROXY], blob);
      ਜੇ(!ਨਤੀਜਾ)
        s->proxy_ssl.custom_cablob = TRUE;
      ਵਾਪਸ ਨਤੀਜਾ;
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  ਹਾਲਤ CURLOPT_PROXY_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_ISSUERCERT_BLOB))
      ਵਾਪਸ Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT_PROXY], blob);
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
  ਹਾਲਤ CURLOPT_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use
     */
    ਵਾਪਸ Curl_setblobopt(&s->blobs[BLOB_ਕੁੰਜੀ], blob);
  ਹਾਲਤ CURLOPT_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection.
     * Specify entire PEM of the CA certificate
     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_CAINFO_BLOB)) {
      CURLcode ਨਤੀਜਾ = Curl_setblobopt(&s->blobs[BLOB_CAINFO], blob);
      ਜੇ(!ਨਤੀਜਾ)
        s->ssl.custom_cablob = TRUE;
      ਵਾਪਸ ਨਤੀਜਾ;
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  ਹਾਲਤ CURLOPT_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    ਜੇ(Curl_ssl_supports(ਅੰਕੜੇ, SSLSUPP_ISSUERCERT_BLOB))
      ਵਾਪਸ Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT], blob);
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;

  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }
  /* unreachable */
}

/*
 * Do not make Curl_vsetopt() static: it is called from
 * projects/OS400/ccsidcurl.c.
 */
CURLcode Curl_vsetopt(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option, va_list param)
{
  ਜੇ(option < CURLOPTTYPE_OBJECTPOINT)
    ਵਾਪਸ setopt_long(ਅੰਕੜੇ, option, va_arg(param, ਲੰਮਾ));
  ਨਹੀਂਤਾਂ ਜੇ(option < CURLOPTTYPE_FUNCTIONPOINT) {
    /* unfortunately, different pointer types cannot be identified any other
       way than being listed explicitly */
    ਚੋਣ(option) {
    ਹਾਲਤ CURLOPT_HTTPHEADER:
    ਹਾਲਤ CURLOPT_QUOTE:
    ਹਾਲਤ CURLOPT_POSTQUOTE:
    ਹਾਲਤ CURLOPT_TELNETOPTIONS:
    ਹਾਲਤ CURLOPT_PREQUOTE:
    ਹਾਲਤ CURLOPT_HTTP200ALIASES:
    ਹਾਲਤ CURLOPT_MAIL_RCPT:
    ਹਾਲਤ CURLOPT_RESOLVE:
    ਹਾਲਤ CURLOPT_PROXYHEADER:
    ਹਾਲਤ CURLOPT_CONNECT_ਮੰਜ਼ਿਲ:
      ਵਾਪਸ setopt_slist(ਅੰਕੜੇ, option, va_arg(param, ਬਣਤਰ curl_slist *));
    ਹਾਲਤ CURLOPT_HTTPPOST:         /* curl_httppost * */
    ਹਾਲਤ CURLOPT_MIMEPOST:         /* curl_mime * */
    ਹਾਲਤ CURLOPT_STDERR:           /* FILE * */
    ਹਾਲਤ CURLOPT_SHARE:            /* CURLSH * */
    ਹਾਲਤ CURLOPT_CURLU:            /* CURLU * */
      ਵਾਪਸ setopt_ਸੂਚਕ_ਸਾਰੇ(ਅੰਕੜੇ, option, param);
    ਹਾਲਤ CURLOPT_ਧਾਰਾ_DEPENDS:   /* CURL * */
    ਹਾਲਤ CURLOPT_ਧਾਰਾ_DEPENDS_E: /* CURL * */
      ਵਾਪਸ CURLE_OK;
    ਮੂਲ:
      ਰੋਕੋ;
    }
    /* the char pointer options */
    ਵਾਪਸ setopt_cptr(ਅੰਕੜੇ, option, va_arg(param, ਅੱਖਰ *));
  }
  ਨਹੀਂਤਾਂ ਜੇ(option < CURLOPTTYPE_OFF_T)
    ਵਾਪਸ setopt_func(ਅੰਕੜੇ, option, param);
  ਨਹੀਂਤਾਂ ਜੇ(option < CURLOPTTYPE_BLOB)
    ਵਾਪਸ setopt_offt(ਅੰਕੜੇ, option, va_arg(param, curl_off_t));
  ਵਾਪਸ setopt_blob(ਅੰਕੜੇ, option, va_arg(param, ਬਣਤਰ curl_blob *));
}

/*
 * curl_easy_setopt() is the external interface for setting options on an
 * easy handle.
 *
 * NOTE: This is one of few API functions that are allowed to be called from
 * within a callback.
 */
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ curl_easy_setopt
CURLcode curl_easy_setopt(CURL *curl, CURLoption option, ...)
{
  ਬਣਤਰ Curl_eapi_guard guard;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(CURL_EAPI_ENTER(&guard, curl, easy_setopt, &ਨਤੀਜਾ)) {
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = curl;
    va_list arg;

    va_start(arg, option);

    ਨਤੀਜਾ = Curl_vsetopt(ਅੰਕੜੇ, option, arg);

    va_end(arg);
    ਜੇ(ਨਤੀਜਾ == CURLE_BAD_FUNCTION_ਦਲੀਲ)
      failf(ਅੰਕੜੇ, "setopt 0x%x got bad argument", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)option);
  }
  CURL_EAPI_LEAVE(&guard);
  ਵਾਪਸ ਨਤੀਜਾ;
}
