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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LINUX_TCP_H
#ಅನ್ತರ್ಭಾವ <linux/tcp.h>
#ಅಥವಾಆದರೆ defined(HAVE_NETINET_TCP_H)
#ಅನ್ತರ್ಭಾವ <netinet/tcp.h>
#ಆದರೆ_ಅಂತ್ಯ

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

ಸ್ಥಿರ CURLcode setopt_ಗಣ_timeout_sec(timediff_t *ptimeout_ms, ದೀರ್ಘ secs)
{
  ಆದರೆ(secs < 0)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
#ಆದರೆ LONG_MAX > (TIMEDIFF_T_MAX / 1000)
  ಆದರೆ(secs > (TIMEDIFF_T_MAX / 1000)) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
#ಆದರೆ_ಅಂತ್ಯ
  *ptimeout_ms = (timediff_t)secs * 1000;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode setopt_ಗಣ_timeout_ms(timediff_t *ptimeout_ms, ದೀರ್ಘ ms)
{
  ಆದರೆ(ms < 0)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
#ಆದರೆ LONG_MAX > TIMEDIFF_T_MAX
  ಆದರೆ(ms > TIMEDIFF_T_MAX) {
    *ptimeout_ms = TIMEDIFF_T_MAX;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
#ಆದರೆ_ಅಂತ್ಯ
  *ptimeout_ms = (timediff_t)ms;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_setstropt(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ಎಣಿಕೆ dupstring id, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s)
{
  size_t slen = s ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s) : 0;
  DEBUGASSERT((ಚಿಹ್ನೆರಹಿತ)id <= UINT8_MAX);
  ಆದರೆ(s && (slen > CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ))
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  ಹಿಂತಿರುಗಿ CURL_EASY_STR_ಗಣ(ದತ್ತಾಂಶ, (uint8_t)id, s, slen);
}

CURLcode Curl_setblobopt(ರಚನೆ curl_blob **blobp,
                         ಸ್ಥಿರಾಂಕ ರಚನೆ curl_blob *blob)
{
  /* free the previous storage at `blobp' and replace by a dynamic storage
     copy of blob. If CURL_BLOB_COPY is set, the data is copied. */

  curlx_safefree(*blobp);

  ಆದರೆ(blob) {
    ರಚನೆ curl_blob *nblob;
    ಆದರೆ(!blob->ದತ್ತಾಂಶ || !blob->len || (blob->len > CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ))
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    nblob = (ರಚನೆ curl_blob *)
      curlx_malloc(ಗಾತ್ರ(ರಚನೆ curl_blob) +
                   ((blob->flags & CURL_BLOB_COPY) ? blob->len : 0));
    ಆದರೆ(!nblob)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    *nblob = *blob;
    ಆದರೆ(blob->flags & CURL_BLOB_COPY) {
      /* put the data after the blob struct in memory */
      nblob->ದತ್ತಾಂಶ = (ಅಕ್ಷರ *)nblob + ಗಾತ್ರ(ರಚನೆ curl_blob);
      ಸ್ಮರಣೆ_ನಕಲು(nblob->ದತ್ತಾಂಶ, blob->ದತ್ತಾಂಶ, blob->len);
    }

    *blobp = nblob;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode setstropt_userpwd(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *option, ಅಕ್ಷರ **userp,
                                  ಅಕ್ಷರ **passwdp)
{
  ಅಕ್ಷರ *user = ಖಾಲಿ;
  ಅಕ್ಷರ *passwd = ಖಾಲಿ;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Parse the login details if specified. If not, then we treat NULL as a
     hint to clear the existing data */
  ಆದರೆ(option) {
    size_t len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(option);
    CURLcode ಫಲಿತಾಂಶ;
    ಆದರೆ(len > CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

    ಫಲಿತಾಂಶ = Curl_parse_login_details(option, len, &user, &passwd, ಖಾಲಿ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  curlx_free(*userp);
  *userp = user;

  curlx_strzero(*passwdp);
  curlx_free(*passwdp);
  *passwdp = passwd;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode setstropt_interface(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಅಕ್ಷರ *option)
{
  ಆದರೆ(option && !option[0])
    option = ಖಾಲಿ;
  ಆದರೆ(option) {
    /* Parse for checking syntax */
    CURLcode ಫಲಿತಾಂಶ = Curl_parse_interface(option, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ಹಿಂತಿರುಗಿ CURL_EASY_STR_ಗಣ(ದತ್ತಾಂಶ, STRING_INTERFACE_OPT,
                             option, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(option));
  }
  CURL_EASY_STR_CLEAR(ದತ್ತಾಂಶ, STRING_INTERFACE_OPT);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
#ವ್ಯಾಖ್ಯಾನಿಸು C_SSLVERSION_ಮೌಲ್ಯ(x)     ((x) & 0xffff)
#ವ್ಯಾಖ್ಯಾನಿಸು C_SSLVERSION_MAX_ಮೌಲ್ಯ(x) ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(x) & 0xffff0000)
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode protocol2num(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, curl_prot_t *val)
{
  /*
   * We are asked to cherry-pick protocols, so play it safe and disallow all
   * protocols to start with, and re-add the wanted ones back in.
   */
  *val = 0;

  ಆದರೆ(!str)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  ಆದರೆ(curl_strequal(str, "all")) {
    *val = ~(curl_prot_t)0;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಮಾಡು {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *token = str;
    size_t tlen;

    str = strchr(str, ',');
    tlen = str ? (size_t)(str - token) : ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(token);
    ಆದರೆ(tlen) {
      ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *h = Curl_getn_scheme(token, tlen);

      ಆದರೆ(!h || !h->ಸತತ)
        ಹಿಂತಿರುಗಿ CURLE_UNSUPPORTED_PROTOCOL;

      *val |= h->protocol;
    }
  } ಆಗ(str && str++);

  ಆದರೆ(!*val)
    /* no protocol listed */
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ಆದರೆ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
ಸ್ಥಿರ CURLcode httpauth(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool proxy,
                         ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ auth)
{
  ಆದರೆ(auth != CURLAUTH_NONE) {
    ಪೂರ್ಣಾಂಕ bitcheck = 0;
    bool authbits = FALSE;
    ಆದರೆ(auth & CURLAUTH_DIGEST_IE) {
      auth |= CURLAUTH_DIGEST; /* set standard digest bit */
      auth &= ~CURLAUTH_DIGEST_IE; /* drop the legacy bit */
    }

    /* switch off bits we cannot support */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_NTLM
    auth &= ~CURLAUTH_NTLM; /* no NTLM support */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_SPNEGO
    auth &= ~CURLAUTH_NEGOTIATE; /* no Negotiate (SPNEGO) auth without GSS-API
                                    or SSPI */
#ಆದರೆ_ಅಂತ್ಯ

    /* check if any auth bit lower than CURLAUTH_ONLY is still set */
    ಆಗ(bitcheck < 31) {
      ಆದರೆ(auth & (1UL << bitcheck++)) {
        authbits = TRUE;
        ನಿಲ್ಲಿಸು;
      }
    }
    ಆದರೆ(!authbits)
      ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ; /* no supported types left! */
  }
  ಆದರೆ(proxy)
    ದತ್ತಾಂಶ->ಗಣ.proxyauth = (uint32_t)auth;
  ಇಲ್ಲದಿದ್ದರೆ
    ದತ್ತಾಂಶ->ಗಣ.httpauth = (uint32_t)auth;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
ಸ್ಥಿರ CURLcode setopt_HTTP_ಆವೃತ್ತಿ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ದೀರ್ಘ arg)
{
  /*
   * This sets a requested HTTP version to be used. The value is one of
   * the listed enums in curl/curl.h.
   */
  ಆಯ್ಕೆ(arg) {
  ಸ್ಥಿತಿ CURL_HTTP_ಆವೃತ್ತಿ_NONE:
    /* accepted */
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_HTTP_ಆವೃತ್ತಿ_1_0:
  ಸ್ಥಿತಿ CURL_HTTP_ಆವೃತ್ತಿ_1_1:
    /* accepted */
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTP2
  ಸ್ಥಿತಿ CURL_HTTP_ಆವೃತ್ತಿ_2_0:
  ಸ್ಥಿತಿ CURL_HTTP_ಆವೃತ್ತಿ_2_TLS:
  ಸ್ಥಿತಿ CURL_HTTP_ಆವೃತ್ತಿ_2_ಪೂರ್ವ_KNOWLEDGE:
    /* accepted */
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTP3
  ಸ್ಥಿತಿ CURL_HTTP_ಆವೃತ್ತಿ_3:
  ಸ್ಥಿತಿ CURL_HTTP_ಆವೃತ್ತಿ_3_ONLY:
    /* accepted */
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ವನಿಯೋಜಿತ:
    /* not accepted */
    ಆದರೆ(arg < CURL_HTTP_ಆವೃತ್ತಿ_NONE)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    ಹಿಂತಿರುಗಿ CURLE_UNSUPPORTED_PROTOCOL;
  }
  ದತ್ತಾಂಶ->ಗಣ.httpwant = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)arg;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
CURLcode Curl_setopt_SSLVERSION(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                                ದೀರ್ಘ arg)
{
  /*
   * Set explicit SSL version to try to connect with, as some SSL
   * implementations are lame.
   */
  {
    ದೀರ್ಘ ಆವೃತ್ತಿ_2, ಆವೃತ್ತಿ_max;
    ರಚನೆ ssl_easy_config *sslc = &ದತ್ತಾಂಶ->ಗಣ.ssl;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
    ಆದರೆ(option != CURLOPT_SSLVERSION)
      sslc = &ದತ್ತಾಂಶ->ಗಣ.proxy_ssl;
#ಇಲ್ಲದಿದ್ದರೆ
    (ಶೂನ್ಯ)option; /*ಬಳಸದ*/
#ಆದರೆ_ಅಂತ್ಯ
    ಆವೃತ್ತಿ_2 = C_SSLVERSION_ಮೌಲ್ಯ(arg);
    ಆವೃತ್ತಿ_max = (ದೀರ್ಘ)C_SSLVERSION_MAX_ಮೌಲ್ಯ(arg);

    ಆದರೆ(ಆವೃತ್ತಿ_2 < CURL_SSLVERSION_DEFAULT ||
       ಆವೃತ್ತಿ_2 == CURL_SSLVERSION_SSLv2 ||
       ಆವೃತ್ತಿ_2 == CURL_SSLVERSION_SSLv3 ||
       ಆವೃತ್ತಿ_2 >= CURL_SSLVERSION_ಕೊನೆಯ ||
       ಆವೃತ್ತಿ_max < CURL_SSLVERSION_MAX_NONE ||
       ಆವೃತ್ತಿ_max >= CURL_SSLVERSION_MAX_ಕೊನೆಯ)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    ಆದರೆ(ಆವೃತ್ತಿ_2 == CURL_SSLVERSION_DEFAULT)
      ಆವೃತ್ತಿ_2 = CURL_SSLVERSION_TLSv1_2;

    sslc->ಆವೃತ್ತಿ_2 = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)ಆವೃತ್ತಿ_2;
    sslc->ಆವೃತ್ತಿ_max = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಆವೃತ್ತಿ_max;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ /* !USE_SSL */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_RTSP
ಸ್ಥಿರ CURLcode setopt_RTSP_REQUEST(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ದೀರ್ಘ arg)
{
  /*
   * Set the RTSP request method (OPTIONS, SETUP, PLAY, etc...) Would this be
   * better if the RTSPREQ_* were moved into here?
   */
  ಆದರೆ((arg <= CURL_RTSPREQ_NONE) || (arg >= CURL_RTSPREQ_ಕೊನೆಯ))
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  ದತ್ತಾಂಶ->ಗಣ.rtspreq = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)arg;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_RTSP */

ಸ್ಥಿರ CURLcode setopt_long_bool(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                                 ದೀರ್ಘ arg)
{
  bool enabled = !!arg;
  ಪೂರ್ಣಾಂಕ ok = 1;
  ರಚನೆ UserDefined *s = &ದತ್ತಾಂಶ->ಗಣ;
  ಆಯ್ಕೆ(option) {
  ಸ್ಥಿತಿ CURLOPT_FORBID_REUSE:
    /*
     * When this transfer is done, it must not be left to be reused by a
     * subsequent transfer but shall be closed immediately.
     */
    s->reuse_forbid = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_FRESH_CONNECT:
    /*
     * This transfer shall not use a previously cached connection but
     * should be made with a fresh new connect!
     */
    s->reuse_fresh = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_VERBOSE:
    /*
     * Verbose means infof() calls that give a lot of information about
     * the connection and transfer procedures as well as internal choices.
     */
    s->verbose = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_ಶೀರ್ಷಿಕೆ:
    /*
     * Set to include the header in the general data output stream.
     */
    s->include_ಶೀರ್ಷಿಕೆ = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_NOPROGRESS:
    /*
     * Shut off the internal supported progress meter
     */
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.hide = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_NOBODY:
    /*
     * Do not include the body part in the output data stream.
     */
    s->opt_no_body = enabled;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
    ಆದರೆ(s->opt_no_body)
      /* in HTTP lingo, no body means using the HEAD request... */
      s->ವಿಧಾನ = HTTPREQ_ತಲೆ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(s->ವಿಧಾನ == HTTPREQ_ತಲೆ)
      s->ವಿಧಾನ = HTTPREQ_GET;
#ಆದರೆ_ಅಂತ್ಯ
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_FAILONERROR:
    /*
     * Do not output the >=400 error code HTML-page, but instead only
     * return error.
     */
    s->http_fail_on_error = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_KEEP_SENDING_ON_ERROR:
    s->http_keep_sending_on_error = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_UPLOAD:
  ಸ್ಥಿತಿ CURLOPT_PUT:
    /*
     * We want to send data to the remote host. If this is HTTP, that equals
     * using the PUT request.
     */
    ಆದರೆ(enabled) {
      /* If this is HTTP, PUT is what's needed to "upload" */
      s->ವಿಧಾನ = HTTPREQ_PUT;
      s->opt_no_body = FALSE; /* this is implied */
    }
    ಇಲ್ಲದಿದ್ದರೆ
      /* In HTTP, the opposite of upload is GET (unless NOBODY is true as
         then this can be changed to HEAD later on) */
      s->ವಿಧಾನ = HTTPREQ_GET;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_FILETIME:
    /*
     * Try to get the file time of the remote document. The time will
     * later (possibly) become available using curl_easy_getinfo().
     */
    s->get_filetime = enabled;
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  ಸ್ಥಿತಿ CURLOPT_HTTP09_ALLOWED:
    s->http09_allowed = enabled;
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_COOKIES
  ಸ್ಥಿತಿ CURLOPT_COOKIESESSION:
    /*
     * Set this option to TRUE to start a new "cookie session". It will
     * prevent the forthcoming read-cookies-from-file actions to accept
     * cookies that are marked as being session cookies, as they belong to a
     * previous session.
     */
    s->cookiesession = enabled;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ CURLOPT_AUTOREFERER:
    /*
     * Switch on automatic referer that gets set if curl follows locations.
     */
    s->http_auto_referer = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_TRANSFER_ENCODING:
    s->http_transfer_encoding = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_UNRESTRICTED_AUTH:
    /*
     * Send authentication (user+password) when following locations, even when
     * hostname changed.
     */
    s->allow_auth_ಗಮ್ಯ_other_hosts = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_HTTP_TRANSFER_DECODING:
    /*
     * disable libcurl transfer encoding is used
     */
    s->http_te_skip = !enabled; /* reversed */
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_HTTP_CONTENT_DECODING:
    /*
     * raw data passed to the application when content encoding is used
     */
    s->http_ce_skip = !enabled; /* reversed */
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_HTTPGET:
    /*
     * Set to force us do HTTP GET
     */
    ಆದರೆ(enabled) {
      s->ವಿಧಾನ = HTTPREQ_GET;
      s->opt_no_body = FALSE; /* this is implied */
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_POST:
    /* Does this option serve a purpose anymore? Yes it does, when
       CURLOPT_POSTFIELDS is not used and the POST data is read off the
       callback! */
    ಆದರೆ(enabled) {
      s->ವಿಧಾನ = HTTPREQ_POST;
      s->opt_no_body = FALSE; /* this is implied */
    }
    ಇಲ್ಲದಿದ್ದರೆ
      s->ವಿಧಾನ = HTTPREQ_GET;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಸ್ಥಿತಿ CURLOPT_HTTPPROXYTUNNEL:
    /*
     * Tunnel operations through the proxy instead of normal proxy use
     */
    s->tunnel_thru_httpproxy = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_HAPROXYPROTOCOL:
    /*
     * Set to send the HAProxy Proxy Protocol header
     */
    s->haproxyprotocol = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_PROXY_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for proxy.
     */
    s->proxy_ssl.verifypeer = enabled;

    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(ದತ್ತಾಂಶ, TRUE);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_PROXY_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for proxy
     */
    s->proxy_ssl.verifyhost = enabled;
    ok = 2;
    /* Update the current connection proxy_ssl_config. */
    Curl_ssl_conn_config_update(ದತ್ತಾಂಶ, TRUE);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_PROXY_TRANSFER_ಬಹುಲಕ:
    /*
     * set transfer mode (;type=<a|i>) when doing FTP via an HTTP proxy
     */
    s->proxy_transfer_ಬಹುಲಕ = enabled;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_PROXY */
#ಆದರೆ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ಸ್ಥಿತಿ CURLOPT_SOCKS5_GSSAPI_NEC:
    /*
     * Set flag for NEC SOCKS5 support
     */
    s->socks5_gssapi_nec = enabled;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_LIST_ONLY_PROTOCOL
  ಸ್ಥಿತಿ CURLOPT_DIRLISTONLY:
    /*
     * An option that changes the command to one that asks for a list only, no
     * file info details. Used for FTP, POP3 and SFTP.
     */
    s->list_only = enabled;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ CURLOPT_APPEND:
    /*
     * We want to upload and append to an existing file. Used for FTP and
     * SFTP.
     */
    s->remote_append = enabled;
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
  ಸ್ಥಿತಿ CURLOPT_FTP_USE_EPRT:
    s->ftp_use_eprt = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_FTP_USE_EPSV:
    s->ftp_use_epsv = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_FTP_USE_PRET:
    s->ftp_use_pret = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_FTP_SKIP_PASV_IP:
    /*
     * Enable or disable FTP_SKIP_PASV_IP, which will disable/enable the
     * bypass of the IP address in PASV responses.
     */
    s->ftp_skip_ip = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_WILDCARDMATCH:
    s->wildcard_enabled = enabled;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ CURLOPT_CRLF:
    /*
     * Kludgy option to enable CRLF conversions. Subject for removal.
     */
    s->crlf = enabled;
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_TFTP
  ಸ್ಥಿತಿ CURLOPT_TFTP_NO_OPTIONS:
    /*
     * Option that prevents libcurl from sending TFTP option requests to the
     * server.
     */
    s->tftp_no_options = enabled;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_TFTP */
  ಸ್ಥಿತಿ CURLOPT_TRANSFERTEXT:
    /*
     * This option was previously named 'FTPASCII'. Renamed to work with
     * more protocols than merely FTP.
     *
     * Transfer using ASCII (instead of BINARY).
     */
    s->prefer_ascii = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying.
     */
    s->ssl.verifypeer = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(ದತ್ತಾಂಶ, FALSE);
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DOH
  ಸ್ಥಿತಿ CURLOPT_DOH_SSL_VERIFYPEER:
    /*
     * Enable peer SSL verifying for DoH.
     */
    s->doh_verifypeer = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_DOH_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate for DoH
     */
    s->doh_verifyhost = enabled;
    ok = 2;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_DOH_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying for DoH.
     */
    ಆದರೆ(!Curl_ssl_cert_ಸ್ಥಿತಿ_request())
      ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;

    s->doh_verifystatus = enabled;
    ok = 2;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_DOH */
  ಸ್ಥಿತಿ CURLOPT_SSL_VERIFYHOST:
    /*
     * Enable verification of the hostname in the peer certificate
     */

    /* Obviously people are not reading documentation and too many thought
       this argument took a boolean when it was not and misused it.
       Treat 1 and 2 the same */
    s->ssl.verifyhost = enabled;
    ok = 2;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(ದತ್ತಾಂಶ, FALSE);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_SSL_VERIFYSTATUS:
    /*
     * Enable certificate status verifying.
     */
    ಆದರೆ(!Curl_ssl_cert_ಸ್ಥಿತಿ_request())
      ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;

    s->ssl.verifystatus = enabled;

    /* Update the current connection ssl_config. */
    Curl_ssl_conn_config_update(ದತ್ತಾಂಶ, FALSE);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_CERTINFO:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_CERTINFO))
      s->ssl.certinfo = enabled;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
      ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_NOSIGNAL:
    /*
     * The application asks not to set any signal() or alarm() handlers,
     * even when using a timeout.
     */
    s->no_signal = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_TCP_NODELAY:
    /*
     * Enable or disable TCP_NODELAY, which will disable/enable the Nagle
     * algorithm
     */
    s->tcp_nodelay = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_IGNORE_CONTENT_ಉದ್ದಳತೆ:
    s->ignorecl = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_SSL_SESSIONID_CACHE:
    s->ssl.cache_session = enabled;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
    s->proxy_ssl.cache_session = s->ssl.cache_session;
#ಆದರೆ_ಅಂತ್ಯ
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
  ಸ್ಥಿತಿ CURLOPT_SSH_COMPRESSION:
    s->ssh_compression = enabled;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ /* !USE_SSH */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_SMTP
  ಸ್ಥಿತಿ CURLOPT_MAIL_RCPT_ALLOWFAILS:
    /* allow RCPT TO command to fail for some recipients */
    s->mail_rcpt_allowfails = enabled;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_SMTP */
  ಸ್ಥಿತಿ CURLOPT_SASL_IR:
    /* Enable/disable SASL initial response */
    s->sasl_ir = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_TCP_KEEPALIVE:
    s->tcp_keepalive = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_TCP_FASTOPEN:
#ಆದರೆ defined(CONNECT_ದತ್ತಾಂಶ_IDEMPOTENT) || defined(MSG_FASTOPEN) || \
  defined(TCP_FASTOPEN_CONNECT)
    s->tcp_fastopen = enabled;
    ನಿಲ್ಲಿಸು;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ CURLOPT_SSL_ENABLE_ALPN:
    s->ssl_enable_alpn = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_ಮಾರ್ಗ_AS_IS:
    s->ಮಾರ್ಗ_as_is = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_PIPEWAIT:
    s->pipewait = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_SUPPRESS_CONNECT_ಶೀರ್ಷಿಕೆಗಳು:
    s->suppress_connect_ಶೀರ್ಷಿಕೆಗಳು = enabled;
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_SHUFFLE_DNS
  ಸ್ಥಿತಿ CURLOPT_DNS_SHUFFLE_ವಿಳಾಸಗಳು:
    s->dns_shuffle_ವಿಳಾಸಗಳು = enabled;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ CURLOPT_DISALLOW_USERNAME_ಒಳ_URL:
    s->disallow_username_ಒಳ_url = enabled;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_ಶೀಘ್ರ_EXIT:
    s->quick_exit = enabled;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }
  ಆದರೆ((arg > ok) || (arg < 0))
    /* reserve other values for future use */
    infof(ದತ್ತಾಂಶ, "boolean setopt(%d) got unsupported argument %ld,"
          " treated as %d", (ಪೂರ್ಣಾಂಕ)option, arg, enabled);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ಮೌಲ್ಯ_range(ದೀರ್ಘ *ಮೌಲ್ಯ, ದೀರ್ಘ ಕೆಳಗೆ_error, ದೀರ್ಘ min, ದೀರ್ಘ max)
{
  ಆದರೆ(*ಮೌಲ್ಯ < ಕೆಳಗೆ_error)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*ಮೌಲ್ಯ < min)
    *ಮೌಲ್ಯ = min;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*ಮೌಲ್ಯ > max)
    *ಮೌಲ್ಯ = max;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode setopt_long_net(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                                ದೀರ್ಘ arg)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ UserDefined *s = &ದತ್ತಾಂಶ->ಗಣ;

  ಆಯ್ಕೆ(option) {
  ಸ್ಥಿತಿ CURLOPT_DNS_CACHE_TIMEOUT:
    ಆದರೆ(arg != -1)
      ಹಿಂತಿರುಗಿ setopt_ಗಣ_timeout_sec(&s->dns_cache_timeout_ms, arg);
    s->dns_cache_timeout_ms = -1;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_MAXCONNECTS:
    ಫಲಿತಾಂಶ = ಮೌಲ್ಯ_range(&arg, 0, 0, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      s->maxconnects = arg ? (uint32_t)arg : DEFAULT_CONNCACHE_ಗಾತ್ರ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_SERVER_RESPONSE_TIMEOUT:
    ಹಿಂತಿರುಗಿ setopt_ಗಣ_timeout_sec(&s->server_response_timeout, arg);
  ಸ್ಥಿತಿ CURLOPT_SERVER_RESPONSE_TIMEOUT_MS:
    ಹಿಂತಿರುಗಿ setopt_ಗಣ_timeout_ms(&s->server_response_timeout, arg);
  ಸ್ಥಿತಿ CURLOPT_ಕೆಳಗಿನ_ವೇಗ_ಎಲ್ಲೆ:
    ಆದರೆ(arg < 0)
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ
      s->ಕೆಳಗಿನ_ವೇಗ_ಎಲ್ಲೆ = arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_ಕೆಳಗಿನ_ವೇಗ_ಸಮಯ:
    ಫಲಿತಾಂಶ = ಮೌಲ್ಯ_range(&arg, 0, 0, USHRT_MAX);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      s->ಕೆಳಗಿನ_ವೇಗ_ಸಮಯ = (uint16_t)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_ಪೋರ್ಟ್:
    ಆದರೆ((arg < 0) || (arg > 65535))
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    s->use_ಪೋರ್ಟ್ = (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_TIMEOUT:
    ಹಿಂತಿರುಗಿ setopt_ಗಣ_timeout_sec(&s->timeout, arg);
  ಸ್ಥಿತಿ CURLOPT_TIMEOUT_MS:
    ಹಿಂತಿರುಗಿ setopt_ಗಣ_timeout_ms(&s->timeout, arg);
  ಸ್ಥಿತಿ CURLOPT_CONNECTTIMEOUT:
    ಹಿಂತಿರುಗಿ setopt_ಗಣ_timeout_sec(&s->connecttimeout, arg);
  ಸ್ಥಿತಿ CURLOPT_CONNECTTIMEOUT_MS:
    ಹಿಂತಿರುಗಿ setopt_ಗಣ_timeout_ms(&s->connecttimeout, arg);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_BINDLOCAL
  ಸ್ಥಿತಿ CURLOPT_LOCALPORT:
    ಆದರೆ((arg < 0) || (arg > 65535))
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    s->localport = curlx_sltous(arg);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_LOCALPORTRANGE:
    ಆದರೆ((arg < 0) || (arg > 65535))
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    s->localportrange = curlx_sltous(arg);
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ CURLOPT_BUFFERSIZE:
    ಫಲಿತಾಂಶ = ಮೌಲ್ಯ_range(&arg, 0, READBUFFER_MIN, READBUFFER_MAX);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      s->ಬಫರ್_ಗಾತ್ರ = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_UPLOAD_BUFFERSIZE:
    ಫಲಿತಾಂಶ = ಮೌಲ್ಯ_range(&arg, 0, UPLOADBUFFER_MIN, UPLOADBUFFER_MAX);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      s->upload_ಬಫರ್_ಗಾತ್ರ = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_MAXFILESIZE:
    ಆದರೆ(arg < 0)
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ
      s->max_filesize = arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_IPRESOLVE:
    ಆದರೆ((arg < CURL_IPRESOLVE_WHATEVER) || (arg > CURL_IPRESOLVE_V6))
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ
      s->ipver = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_CONNECT_ONLY:
    ಆದರೆ(arg < 0 || arg > 2)
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ {
      s->connect_only = !!arg;
      s->connect_only_ws = (arg == 2);
    }
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ಸ್ಥಿತಿ CURLOPT_ವಿಳಾಸ_SCOPE:
#ಆದರೆ SIZEOF_LONG > 4
    ಆದರೆ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)arg > UINT_MAX)
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    s->scope_id = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)arg;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ CURLOPT_TCP_KEEPIDLE:
    ಫಲಿತಾಂಶ = ಮೌಲ್ಯ_range(&arg, 0, 0, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      s->tcp_keepidle = (ಪೂರ್ಣಾಂಕ)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_TCP_KEEPINTVL:
    ಫಲಿತಾಂಶ = ಮೌಲ್ಯ_range(&arg, 0, 0, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      s->tcp_keepintvl = (ಪೂರ್ಣಾಂಕ)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_TCP_KEEPCNT:
    ಫಲಿತಾಂಶ = ಮೌಲ್ಯ_range(&arg, 0, 0, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      s->tcp_keepcnt = (ಪೂರ್ಣಾಂಕ)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
    ಹಿಂತಿರುಗಿ setopt_ಗಣ_timeout_ms(&s->happy_eyeballs_timeout, arg);
  ಸ್ಥಿತಿ CURLOPT_UPKEEP_INTERVAL_MS:
    ಹಿಂತಿರುಗಿ setopt_ಗಣ_timeout_ms(&s->upkeep_interval_ms, arg);
  ಸ್ಥಿತಿ CURLOPT_MAXAGE_CONN:
    ಹಿಂತಿರುಗಿ setopt_ಗಣ_timeout_sec(&s->conn_max_idle_ms, arg);
  ಸ್ಥಿತಿ CURLOPT_MAXLIFETIME_CONN:
    ಹಿಂತಿರುಗಿ setopt_ಗಣ_timeout_sec(&s->conn_max_age_ms, arg);
  ಸ್ಥಿತಿ CURLOPT_DNS_USE_GLOBAL_CACHE:
    /* deprecated */
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode setopt_long_ssl(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                                ದೀರ್ಘ arg)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ UserDefined *s = &ದತ್ತಾಂಶ->ಗಣ;
  ಆಯ್ಕೆ(option) {
  ಸ್ಥಿತಿ CURLOPT_CA_CACHE_TIMEOUT:
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_CA_CACHE)) {
      ಫಲಿತಾಂಶ = ಮೌಲ್ಯ_range(&arg, -1, -1, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        s->ssl_ca_cache_timeout = (ಪೂರ್ಣಾಂಕ)arg;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = CURLE_NOT_BUILT_ಒಳ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_SSLVERSION:
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಸ್ಥಿತಿ CURLOPT_PROXY_SSLVERSION:
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ Curl_setopt_SSLVERSION(ದತ್ತಾಂಶ, option, arg);
  ಸ್ಥಿತಿ CURLOPT_SSL_FALSESTART:
    ಫಲಿತಾಂಶ = CURLE_NOT_BUILT_ಒಳ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_USE_SSL:
    ಆದರೆ((arg < CURLUSESSL_NONE) || (arg >= CURLUSESSL_ಕೊನೆಯ))
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ
      s->use_ssl = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_SSL_OPTIONS:
    s->ssl.ssl_options = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(arg & 0xff);
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಸ್ಥಿತಿ CURLOPT_PROXY_SSL_OPTIONS:
    s->proxy_ssl.ssl_options = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(arg & 0xff);
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ CURLOPT_SSL_ENABLE_NPN:
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_SSLENGINE_DEFAULT:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
    ಆದರೆ(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
    ದತ್ತಾಂಶ->ಗಣ.engine_default = (arg == 1);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
#ಇಲ್ಲದಿದ್ದರೆ  /* USE_SSL */
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)option;
  (ಶೂನ್ಯ)arg;
  ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
#ಆದರೆ_ಅಂತ್ಯ /* !USE_SSL */
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
ಸ್ಥಿರ ಶೂನ್ಯ changeproxy(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  Curl_auth_digest_cleanup(&ದತ್ತಾಂಶ->state.proxydigest);
  memset(&ದತ್ತಾಂಶ->state.authproxy, 0, ಗಾತ್ರ(ದತ್ತಾಂಶ->state.authproxy));
}

ಸ್ಥಿರ CURLcode setopt_long_proxy(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                                  ದೀರ್ಘ arg)
{
  ರಚನೆ UserDefined *s = &ದತ್ತಾಂಶ->ಗಣ;

  ಆಯ್ಕೆ(option) {
  ಸ್ಥಿತಿ CURLOPT_PROXYPORT:
    ಆದರೆ((arg < 0) || (arg > UINT16_MAX))
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    ಆದರೆ(arg != s->proxyport)
      changeproxy(ದತ್ತಾಂಶ);
    s->proxyport = (uint16_t)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_PROXYAUTH:
    ಹಿಂತಿರುಗಿ httpauth(ದತ್ತಾಂಶ, TRUE, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)arg);
  ಸ್ಥಿತಿ CURLOPT_PROXYTYPE:
    ಆದರೆ((arg < CURLPROXY_HTTP) || (arg > CURLPROXY_HTTPS3))
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_PROXY_HTTP3
    ಆದರೆ(arg == CURLPROXY_HTTPS3)
      ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
    s->proxytype = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_SOCKS5_AUTH: {
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ auth = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)arg;
    ಆದರೆ(auth & ~(CURLAUTH_BASIC | CURLAUTH_GSSAPI | CURLAUTH_ONLY))
      ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
    ಆದರೆ((auth & CURLAUTH_ONLY) && !(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI)))
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    s->socks5auth = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(auth & (CURLAUTH_BASIC | CURLAUTH_GSSAPI));
    s->socks5_auth_only = !!(auth & CURLAUTH_ONLY);
    ನಿಲ್ಲಿಸು;
  }
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ CURLcode setopt_long_proxy(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                                  ದೀರ್ಘ arg)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)option;
  (ಶೂನ್ಯ)arg;
  ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode setopt_long_http(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                                 ದೀರ್ಘ arg)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ UserDefined *s = &ದತ್ತಾಂಶ->ಗಣ;

  ಆಯ್ಕೆ(option) {
  ಸ್ಥಿತಿ CURLOPT_FOLLOWLOCATION:
    ಆದರೆ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)arg > 3)
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ
      s->http_follow_ಬಹುಲಕ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_MAXREDIRS:
    ಫಲಿತಾಂಶ = ಮೌಲ್ಯ_range(&arg, -1, -1, 0x7fff);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      s->maxredirs = (ಗಿಡ್ಡ)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_POSTREDIR:
    ಆದರೆ(arg < CURL_REDIR_GET_ALL)
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ {
      s->post301 = !!(arg & CURL_REDIR_POST_301);
      s->post302 = !!(arg & CURL_REDIR_POST_302);
      s->post303 = !!(arg & CURL_REDIR_POST_303);
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_HEADEROPT:
    s->sep_ಶೀರ್ಷಿಕೆಗಳು = !!(arg & CURLHEADER_SEPARATE);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_HTTPAUTH:
    ಹಿಂತಿರುಗಿ httpauth(ದತ್ತಾಂಶ, FALSE, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)arg);
  ಸ್ಥಿತಿ CURLOPT_HTTP_ಆವೃತ್ತಿ:
    ಹಿಂತಿರುಗಿ setopt_HTTP_ಆವೃತ್ತಿ(ದತ್ತಾಂಶ, arg);
  ಸ್ಥಿತಿ CURLOPT_EXPECT_100_TIMEOUT_MS:
    ಫಲಿತಾಂಶ = ಮೌಲ್ಯ_range(&arg, 0, 0, 0xffff);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      s->expect_100_timeout = (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_ಹರಿವು_ತೂಕ:
#ಆದರೆ defined(USE_HTTP2) || defined(USE_HTTP3)
    ಆದರೆ((arg >= 1) && (arg <= 256))
      s->ತೂಕ = (ಪೂರ್ಣಾಂಕ)arg;
    ನಿಲ್ಲಿಸು;
#ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = CURLE_NOT_BUILT_ಒಳ;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTPSIG
  ಸ್ಥಿತಿ CURLOPT_HTTPSIG_ALGORITHM:
    ಆದರೆ(arg != CURLHTTPSIG_NONE &&
       arg != CURLHTTPSIG_ED25519 &&
       arg != CURLHTTPSIG_HMAC_SHA256)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    s->httpsig_algorithm = (uint8_t)arg;
    ಆದರೆ(arg)
      s->httpauth = (uint32_t)CURLAUTH_HTTPSIG;
    ಇಲ್ಲದಿದ್ದರೆ
      s->httpauth &= ~(uint32_t)CURLAUTH_HTTPSIG;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)option;
  (ಶೂನ್ಯ)arg;
  ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ CURLcode setopt_long_proto(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                                  ದೀರ್ಘ arg)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ UserDefined *s = &ದತ್ತಾಂಶ->ಗಣ;

  ಆಯ್ಕೆ(option) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_TFTP
  ಸ್ಥಿತಿ CURLOPT_TFTP_BLKSIZE:
    ಫಲಿತಾಂಶ = ಮೌಲ್ಯ_range(&arg, 0, TFTP_BLKSIZE_MIN, TFTP_BLKSIZE_MAX);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      s->tftp_blksize = (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)arg;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_NETRC
  ಸ್ಥಿತಿ CURLOPT_NETRC:
    ಆದರೆ((arg < CURL_NETRC_IGNORED) || (arg >= CURL_NETRC_ಕೊನೆಯ))
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ
      s->use_netrc = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)arg;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
  ಸ್ಥಿತಿ CURLOPT_FTP_FILEMETHOD:
    ಆದರೆ((arg < CURLFTPMETHOD_DEFAULT) || (arg >= CURLFTPMETHOD_ಕೊನೆಯ))
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ
      s->ftp_filemethod = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_FTP_SSL_CCC:
    ಆದರೆ((arg < CURLFTPSSL_CCC_NONE) || (arg >= CURLFTPSSL_CCC_ಕೊನೆಯ))
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ
      s->ftp_ccc = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_FTPSSLAUTH:
    ಆದರೆ((arg < CURLFTPAUTH_DEFAULT) || (arg >= CURLFTPAUTH_ಕೊನೆಯ))
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ
      s->ftpsslauth = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_ACCEPTTIMEOUT_MS:
    ಹಿಂತಿರುಗಿ setopt_ಗಣ_timeout_ms(&s->accepttimeout, arg);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  ಸ್ಥಿತಿ CURLOPT_FTP_CREATE_MISSING_DIRS:
    ಆದರೆ((arg < CURLFTP_CREATE_DIR_NONE) || (arg > CURLFTP_CREATE_DIR_RETRY))
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ
      s->ftp_create_missing_dirs = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_NEW_FILE_PERMS:
    ಆದರೆ((arg < 0) || (arg > 0777))
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ
      s->new_file_perms = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)arg;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_RTSP
  ಸ್ಥಿತಿ CURLOPT_RTSP_REQUEST:
    ಹಿಂತಿರುಗಿ setopt_RTSP_REQUEST(ದತ್ತಾಂಶ, arg);
  ಸ್ಥಿತಿ CURLOPT_RTSP_CLIENT_CSEQ:
    ಫಲಿತಾಂಶ = ಮೌಲ್ಯ_range(&arg, 0, 0, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ದತ್ತಾಂಶ->state.rtsp_ಮುಂದಿನ_client_CSeq = (uint32_t)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_RTSP_SERVER_CSEQ:
    ಫಲಿತಾಂಶ = ಮೌಲ್ಯ_range(&arg, 0, 0, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ದತ್ತಾಂಶ->state.rtsp_ಮುಂದಿನ_server_CSeq = (uint32_t)arg;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
  ಸ್ಥಿತಿ CURLOPT_SSH_AUTH_TYPES:
    s->ssh_auth_types = (uint32_t)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_NEW_DIRECTORY_PERMS:
    ಆದರೆ((arg < 0) || (arg > 0777))
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ
      s->new_directory_perms = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)arg;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ CURLOPT_PROTOCOLS:
    s->allowed_protocols = (curl_prot_t)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_REDIR_PROTOCOLS:
    s->redir_protocols = (curl_prot_t)arg;
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_WEBSOCKETS
  ಸ್ಥಿತಿ CURLOPT_WS_OPTIONS:
    s->ws_raw_ಬಹುಲಕ = !!(arg & CURLWS_RAW_ಬಹುಲಕ);
    s->ws_no_auto_pong = !!(arg & CURLWS_NOAUTOPONG);
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode setopt_long_misc(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                                 ದೀರ್ಘ arg)
{
  ರಚನೆ UserDefined *s = &ದತ್ತಾಂಶ->ಗಣ;

  ಆಯ್ಕೆ(option) {
  ಸ್ಥಿತಿ CURLOPT_TIMECONDITION:
    ಆದರೆ((arg < CURL_TIMECOND_NONE) || (arg >= CURL_TIMECOND_ಕೊನೆಯ))
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    s->timecondition = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_TIMEVALUE:
    s->timevalue = (time_t)arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_POSTFIELDSIZE:
    ಆದರೆ(arg < -1)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    ಆದರೆ(s->postfieldsize < arg && s->str_copypostfields) {
      curlx_safefree(s->str_copypostfields);
      s->postfields = ಖಾಲಿ;
    }
    s->postfieldsize = arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_INFILESIZE:
    ಆದರೆ(arg < -1)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    s->filesize = arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_RESUME_FROM:
    ಆದರೆ(arg < -1)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    s->ಗಣ_resume_from = arg;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_UPLOAD_FLAGS:
    s->upload_flags = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)arg;
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_MIME
  ಸ್ಥಿತಿ CURLOPT_MIME_OPTIONS:
    s->mime_formescape = !!(arg & CURLMIMEOPT_FORMESCAPE);
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HSTS
  ಸ್ಥಿತಿ CURLOPT_HSTS_CTRL:
    ಆದರೆ(arg & CURLHSTS_ENABLE) {
      ಆದರೆ(!ದತ್ತಾಂಶ->hsts) {
        ದತ್ತಾಂಶ->hsts = Curl_hsts_init();
        ಆದರೆ(!ದತ್ತಾಂಶ->hsts)
          ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ದತ್ತಾಂಶ->share || !ದತ್ತಾಂಶ->share->hsts) {
      /* throw away the HSTS cache unless shared */
      Curl_hsts_cleanup(&ದತ್ತಾಂಶ->hsts);
      /* flush all the entries */
      curl_slist_free_all(ದತ್ತಾಂಶ->state.hstslist);
      ದತ್ತಾಂಶ->state.hstslist = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      /* detach from shared HSTS cache without freeing it */
      ದತ್ತಾಂಶ->hsts = ಖಾಲಿ;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_ALTSVC
  ಸ್ಥಿತಿ CURLOPT_ALTSVC_CTRL:
    ಹಿಂತಿರುಗಿ Curl_altsvc_ctrl(ದತ್ತಾಂಶ, arg);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ಸ್ಥಿತಿ CURLOPT_GSSAPI_DELEGATION:
    s->gssapi_delegation = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)arg &
      (CURLGSSAPI_DELEGATION_POLICY_FLAG | CURLGSSAPI_DELEGATION_FLAG);
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode setopt_long(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                            ದೀರ್ಘ arg)
{
  ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode (*setoptfunc)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 CURLoption option, ದೀರ್ಘ arg);
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ setoptfunc setopt_call[] = {
    setopt_long_bool,
    setopt_long_net,
    setopt_long_http,
    setopt_long_proxy,
    setopt_long_ssl,
    setopt_long_proto,
    setopt_long_misc
  };
  size_t i;

  ಗಾಗಿ(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode ಫಲಿತಾಂಶ = setopt_call[i](ದತ್ತಾಂಶ, option, arg);
    ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_UNKNOWN_OPTION)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
}

ಸ್ಥಿರ CURLcode setopt_slist(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                             ರಚನೆ curl_slist *slist)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ UserDefined *s = &ದತ್ತಾಂಶ->ಗಣ;
  ಆಯ್ಕೆ(option) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಸ್ಥಿತಿ CURLOPT_PROXYHEADER:
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
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  ಸ್ಥಿತಿ CURLOPT_HTTP200ALIASES:
    /*
     * Set a list of aliases for HTTP 200 in response header
     */
    s->http200aliases = slist;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(CURL_DISABLE_FTP) || defined(USE_SSH)
  ಸ್ಥಿತಿ CURLOPT_POSTQUOTE:
    /*
     * List of RAW FTP commands to use after a transfer
     */
    s->postquote = slist;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_PREQUOTE:
    /*
     * List of RAW FTP commands to use prior to RETR (Wesley Laxton)
     */
    s->prequote = slist;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_QUOTE:
    /*
     * List of RAW FTP commands to use before a transfer
     */
    s->quote = slist;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ CURLOPT_RESOLVE:
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
    ದತ್ತಾಂಶ->state.resolve = s->resolve;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MIME)
  ಸ್ಥಿತಿ CURLOPT_HTTPHEADER:
    /*
     * Set a list with HTTP headers to use (or replace internals with)
     */
    s->ಶೀರ್ಷಿಕೆಗಳು = slist;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_TELNET
  ಸ್ಥಿತಿ CURLOPT_TELNETOPTIONS:
    /*
     * Set a linked list of telnet options
     */
    s->telnet_options = slist;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_SMTP
  ಸ್ಥಿತಿ CURLOPT_MAIL_RCPT:
    /* Set the list of mail recipients */
    s->mail_rcpt = slist;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ CURLOPT_CONNECT_ಗಮ್ಯ:
    s->connect_ಗಮ್ಯ = slist;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_MIME
ಸ್ಥಿರ CURLcode setopt_mimepost(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_mime *mimep)
{
  /*
   * Set to make us do MIME POST
   */
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ UserDefined *s = &ದತ್ತಾಂಶ->ಗಣ;
  ಆದರೆ(!s->mimepostp) {
    s->mimepostp = curlx_malloc(ಗಾತ್ರ(*s->mimepostp));
    ಆದರೆ(!s->mimepostp)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    Curl_mime_initpart(s->mimepostp);
  }

  ಫಲಿತಾಂಶ = Curl_mime_ಗಣ_subparts(s->mimepostp, mimep, FALSE);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    s->ವಿಧಾನ = HTTPREQ_POST_MIME;
    s->opt_no_body = FALSE; /* this is implied */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FORM_API
    Curl_mime_cleanpart(ದತ್ತಾಂಶ->state.formp);
    curlx_safefree(ದತ್ತಾಂಶ->state.formp);
    ದತ್ತಾಂಶ->state.mimepost = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_MIME */
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */

ಸ್ಥಿರ CURLcode setopt_share(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_share *ಗಣ)
{
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(ದತ್ತಾಂಶ->conn) {
    /* As this handle already has a connection attached, changing share now
       would be complicated and error-prone */
    infof(ದತ್ತಾಂಶ, "Cannot change share object while in use");
    ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* disconnect from old share, if any and possible */
    ಫಲಿತಾಂಶ = Curl_share_easy_unlink(ದತ್ತಾಂಶ);
    ಆದರೆ(!ಫಲಿತಾಂಶ && GOOD_SHARE_HANDLE(ಗಣ))
      /* use new share if it set */
      ಫಲಿತಾಂಶ = Curl_share_easy_link(ದತ್ತಾಂಶ, ಗಣ);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* assorted pointer type arguments */
ಸ್ಥಿರ CURLcode setopt_ಸೂಚಕಗಳು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                                va_list param)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ UserDefined *s = &ದತ್ತಾಂಶ->ಗಣ;
  ಆಯ್ಕೆ(option) {
  ಸ್ಥಿತಿ CURLOPT_CURLU:
    /*
     * pass CURLU to set URL
     */
    Curl_bufref_free(&ದತ್ತಾಂಶ->state.url);
    CURL_EASY_STR_CLEAR(ದತ್ತಾಂಶ, STRING_ಗಣ_URL);
    s->uh = va_arg(param, CURLU *);
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FORM_API
  ಸ್ಥಿತಿ CURLOPT_HTTPPOST:
    /*
     * Set to make us do HTTP POST. Legacy API-style.
     */
    s->httppost = va_arg(param, ರಚನೆ curl_httppost *);
    s->ವಿಧಾನ = HTTPREQ_POST_FORM;
    s->opt_no_body = FALSE; /* this is implied */
    Curl_mime_cleanpart(ದತ್ತಾಂಶ->state.formp);
    curlx_safefree(ದತ್ತಾಂಶ->state.formp);
    ದತ್ತಾಂಶ->state.mimepost = ಖಾಲಿ;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_FORM_API */
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP */
#ಆದರೆ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_SMTP) ||       \
  !defined(CURL_DISABLE_IMAP)
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_MIME
  ಸ್ಥಿತಿ CURLOPT_MIMEPOST:
    ಫಲಿತಾಂಶ = setopt_mimepost(ದತ್ತಾಂಶ, va_arg(param, curl_mime *));
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_MIME */
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP || !CURL_DISABLE_SMTP || !CURL_DISABLE_IMAP */
  ಸ್ಥಿತಿ CURLOPT_STDERR:
    /*
     * Set to a FILE * that should receive all error writes. This
     * defaults to stderr for normal operations.
     */
    s->err = va_arg(param, FILE *);
    ಆದರೆ(!s->err)
      s->err = stderr;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_SHARE:
    ಹಿಂತಿರುಗಿ setopt_share(ದತ್ತಾಂಶ, va_arg(param, ರಚನೆ Curl_share *));

  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_COOKIES
ಸ್ಥಿರ CURLcode cookielist(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಆದರೆ(!ptr)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಆದರೆ(curl_strequal(ptr, "ALL")) {
    /* clear all cookies */
    Curl_share_ಬೀಗ(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_COOKIE, CURL_ಬೀಗ_ACCESS_SINGLE);
    Curl_cookie_clearall(ದತ್ತಾಂಶ->cookies);
    Curl_share_unlock(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_COOKIE);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal(ptr, "SESS")) {
    /* clear session cookies */
    Curl_share_ಬೀಗ(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_COOKIE, CURL_ಬೀಗ_ACCESS_SINGLE);
    Curl_cookie_clearsess(ದತ್ತಾಂಶ->cookies);
    Curl_share_unlock(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_COOKIE);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal(ptr, "FLUSH")) {
    /* flush cookies to file, takes care of the locking */
    Curl_flush_cookies(ದತ್ತಾಂಶ, FALSE);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal(ptr, "RELOAD")) {
    /* reload cookies from file */
    ಹಿಂತಿರುಗಿ Curl_cookie_loadfiles(ದತ್ತಾಂಶ, COOKIE_NOPSL |
                                 (ದತ್ತಾಂಶ->ಗಣ.cookiesession ?
                                  COOKIE_NOSESSION : 0));
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(!ದತ್ತಾಂಶ->cookies) {
      /* if cookie engine was not running, activate it */
      ದತ್ತಾಂಶ->cookies = Curl_cookie_init();
      ಆದರೆ(!ದತ್ತಾಂಶ->cookies)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ದತ್ತಾಂಶ->state.cookie_engine = TRUE;
    }

    /* general protection against mistakes and abuse */
    ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ptr) > CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

    /* Adding these cookies without the PSL check, because the PSL is not
       initialized until *perform() time, and this might be called before
       that */
    Curl_share_ಬೀಗ(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_COOKIE, CURL_ಬೀಗ_ACCESS_SINGLE);
    ಆದರೆ(checkprefix("Set-Cookie:", ptr))
      /* HTTP Header format line */
      ಫಲಿತಾಂಶ = Curl_cookie_ಕೂಡಿಸು(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->cookies, ptr + 11,
                               ಖಾಲಿ, ಖಾಲಿ,
                               COOKIE_HTTPHEADER | COOKIE_SECURE |
                               COOKIE_NOPSL);
    ಇಲ್ಲದಿದ್ದರೆ
      /* Netscape format line */
      ಫಲಿತಾಂಶ = Curl_cookie_ಕೂಡಿಸು(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->cookies, ptr, ಖಾಲಿ,
                               ಖಾಲಿ, COOKIE_SECURE | COOKIE_NOPSL);
    Curl_share_unlock(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_COOKIE);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cookiefile(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr)
{
  /*
   * Set cookie file to read and parse. Can be used multiple times.
   */
  ಆದರೆ(ptr) {
    ರಚನೆ curl_slist *cl;
    /* general protection against mistakes and abuse */
    ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ptr) > CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    /* append the cookie filename to the list of filenames, and deal with
       them later */
    cl = curl_slist_append(ದತ್ತಾಂಶ->state.cookielist, ptr);
    ಆದರೆ(!cl) {
      curl_slist_free_all(ದತ್ತಾಂಶ->state.cookielist);
      ದತ್ತಾಂಶ->state.cookielist = ಖಾಲಿ;
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    ದತ್ತಾಂಶ->state.cookielist = cl; /* store the list for later use */
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* clear the list of cookie files */
    curl_slist_free_all(ದತ್ತಾಂಶ->state.cookielist);
    ದತ್ತಾಂಶ->state.cookielist = ಖಾಲಿ;

    ಆದರೆ(!ದತ್ತಾಂಶ->share || !ದತ್ತಾಂಶ->share->cookies) {
      /* throw away all existing cookies if this is not a shared cookie
         container */
      Curl_cookie_clearall(ದತ್ತಾಂಶ->cookies);
      Curl_cookie_cleanup(ದತ್ತಾಂಶ->cookies);
    }
    /* disable the cookie engine */
    ದತ್ತಾಂಶ->cookies = ಖಾಲಿ;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
ಸ್ಥಿರ CURLcode setproxy(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *proxy)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_PROXY);
  ಆದರೆ(str && proxy &&
     /* there was one set, is this a new one? */
     !strcmp(str, proxy))
    ಹಿಂತಿರುಗಿ CURLE_OK; /* same one as before */

  changeproxy(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_PROXY, proxy);
}

ಸ್ಥಿರ CURLcode setopt_cptr_proxy(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                                  ಅಕ್ಷರ *ptr)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ UserDefined *s = &ದತ್ತಾಂಶ->ಗಣ;
  ಆಯ್ಕೆ(option) {
  ಸ್ಥಿತಿ CURLOPT_PROXYUSERPWD: {
    /*
     * user:password needed to use the proxy
     */
    ಅಕ್ಷರ *u = ಖಾಲಿ;
    ಅಕ್ಷರ *p = ಖಾಲಿ;
    ಫಲಿತಾಂಶ = setstropt_userpwd(ptr, &u, &p);

    /* URL decode the components */
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      ಅಕ್ಷರ *str = ಖಾಲಿ;
      CURL_EASY_STR_CLEAR(ದತ್ತಾಂಶ, STRING_PROXYUSERNAME);
      CURL_EASY_STR_CLEAR(ದತ್ತಾಂಶ, STRING_PROXYPASSWORD);
      ಆದರೆ(u) {
        ಫಲಿತಾಂಶ = Curl_urldecode(u, 0, &str, ಖಾಲಿ, REJECT_ZERO);
        ಆದರೆ(!ಫಲಿತಾಂಶ)
          ಫಲಿತಾಂಶ = Curl_u8_strset_setn(&s->ಅಕ್ಷರಸರಣಿಗಳು, STRING_PROXYUSERNAME, str);
      }
      ಆದರೆ(!ಫಲಿತಾಂಶ && p) {
        str = ಖಾಲಿ;
        ಫಲಿತಾಂಶ = Curl_urldecode(p, 0, &str, ಖಾಲಿ, REJECT_ZERO);
        ಆದರೆ(!ಫಲಿತಾಂಶ)
          ಫಲಿತಾಂಶ = Curl_u8_strset_setn(&s->ಅಕ್ಷರಸರಣಿಗಳು, STRING_PROXYPASSWORD, str);
      }
    }
    curlx_free(u);
    curlx_strzero(p);
    curlx_free(p);
    ನಿಲ್ಲಿಸು;
  }
  ಸ್ಥಿತಿ CURLOPT_PROXYUSERNAME:
    /*
     * authentication username to use in the operation
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_PROXYUSERNAME, ptr);

  ಸ್ಥಿತಿ CURLOPT_PROXYPASSWORD:
    /*
     * authentication password to use in the operation
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_PROXYPASSWORD, ptr);

  ಸ್ಥಿತಿ CURLOPT_NOPROXY:
    /*
     * proxy exception list
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_NOPROXY, ptr);
  ಸ್ಥಿತಿ CURLOPT_PROXY_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use for proxy
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_CERT_PROXY, ptr);
  ಸ್ಥಿತಿ CURLOPT_PROXY_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use for proxy
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_CERT_TYPE_PROXY, ptr);
  ಸ್ಥಿತಿ CURLOPT_PROXY_SSLKEY:
    /*
     * String that holds filename of the SSL key to use for proxy
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_ಕೀಲಿ_PROXY, ptr);
  ಸ್ಥಿತಿ CURLOPT_PROXY_KEYPASSWD:
    /*
     * String that holds the SSL private key password for proxy.
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_ಕೀಲಿ_PASSWD_PROXY, ptr);
  ಸ್ಥಿತಿ CURLOPT_PROXY_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use for proxy
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_ಕೀಲಿ_TYPE_PROXY, ptr);
  ಸ್ಥಿತಿ CURLOPT_PROXY_SSL_CIPHER_LIST:
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_CIPHER_LIST)) {
      /* set a list of cipher we want to use in the SSL connection for proxy */
      ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_CIPHER_LIST_PROXY, ptr);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  ಸ್ಥಿತಿ CURLOPT_PROXY_TLS13_CIPHERS:
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites for proxy */
      ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_CIPHER13_LIST_PROXY, ptr);
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  ಸ್ಥಿತಿ CURLOPT_PROXY:
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
    ಹಿಂತಿರುಗಿ setproxy(ದತ್ತಾಂಶ, ptr);
  ಸ್ಥಿತಿ CURLOPT_PRE_PROXY:
    /*
     * Set proxy server:port to use as SOCKS proxy.
     *
     * If the proxy is set to "" or NULL we explicitly say that we do not want
     * to use the socks proxy.
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_PRE_PROXY, ptr);
  ಸ್ಥಿತಿ CURLOPT_SOCKS5_GSSAPI_SERVICE:
  ಸ್ಥಿತಿ CURLOPT_PROXY_SERVICE_NAME:
    /*
     * Set proxy authentication service name for Kerberos 5 and SPNEGO
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_PROXY_SERVICE_NAME, ptr);
  ಸ್ಥಿತಿ CURLOPT_PROXY_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_PINNEDPUBKEY))
      ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_PINNEDPUBLICKEY_PROXY, ptr);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;

  ಸ್ಥಿತಿ CURLOPT_HAPROXY_CLIENT_IP:
    /*
     * Set the client IP to send through HAProxy PROXY protocol
     */
    ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_HAPROXY_CLIENT_IP, ptr);

    /* enable the HAProxy protocol if an IP is provided */
    s->haproxyprotocol = !!CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_HAPROXY_CLIENT_IP);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_PROXY_CAINFO:
    /*
     * Set CA info SSL connection for proxy. Specify filename of the
     * CA certificate
     */
    ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_CAFILE_PROXY, ptr);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      s->proxy_ssl.custom_cafile = TRUE;
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  ಸ್ಥಿತಿ CURLOPT_PROXY_CRLFILE:
    /*
     * Set CRL file info for SSL connection for proxy. Specify filename of the
     * CRL to check certificates revocation
     */
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_CRLFILE))
      ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_CRLFILE_PROXY, ptr);
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  ಸ್ಥಿತಿ CURLOPT_PROXY_ISSUERCERT:
    /*
     * Set Issuer certificate file to check certificates issuer
     */
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_ISSUERCERT))
      ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_ISSUERCERT_PROXY, ptr);
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  ಸ್ಥಿತಿ CURLOPT_PROXY_CAPATH:
    /*
     * Set CA path info for SSL connection proxy. Specify directory name of the
     * CA certificates which have been prepared using openssl c_rehash utility.
     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_CA_ಮಾರ್ಗ)) {
      /* This does not work on Windows. */
      ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_CAPATH_PROXY, ptr);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        s->proxy_ssl.custom_capath = TRUE;
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
/*
 * A string with POST data. Makes curl HTTP POST. Even if it is NULL. If
 * needed, CURLOPT_POSTFIELDSIZE must have been set prior to
 * CURLOPT_COPYPOSTFIELDS and not altered later.
 */
ಸ್ಥಿರ CURLcode setopt_copypostfields(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr, ರಚನೆ UserDefined *s)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಆದರೆ(s->postfieldsize < -1)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಆದರೆ(!ptr || s->postfieldsize == -1) {
    ಆದರೆ(ptr && (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ptr) > CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ))
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    curlx_safefree(s->str_copypostfields);
    ಆದರೆ(ptr) {
      s->str_copypostfields = curlx_strdup(ptr);
      ಆದರೆ(!s->str_copypostfields)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    size_t pflen = curlx_sotouz_range(s->postfieldsize, 0, SIZE_MAX);
    ಆದರೆ(pflen == SIZE_MAX)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಇಲ್ಲದಿದ್ದರೆ {
      /* Allocate even when size == 0. This satisfies the need of possible
         later address compare to detect the COPYPOSTFIELDS mode, and to mark
         that postfields is used rather than read function or form data. */
      ಅಕ್ಷರ *p = curlx_memdup0(ptr, pflen);
      ಆದರೆ(!p)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಇಲ್ಲದಿದ್ದರೆ {
        curlx_free(s->str_copypostfields);
        s->str_copypostfields = p;
      }
    }
  }

  s->postfields = s->str_copypostfields;
  s->ವಿಧಾನ = HTTPREQ_POST;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ECH
ಸ್ಥಿರ CURLcode setopt_ech(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr)
{
  ರಚನೆ UserDefined *s = &ದತ್ತಾಂಶ->ಗಣ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!ptr || !strcmp(ptr, "false"))
    s->ssl.ech = CURLECH_DISABLE;
  ಇಲ್ಲದಿದ್ದರೆ {
    size_t plen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ptr);
    ಆದರೆ(plen > CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ)
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(!strcmp(ptr, "grease"))
        s->ssl.ech = CURLECH_GREASE;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(ptr, "true"))
        s->ssl.ech = CURLECH_ENABLE;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(ptr, "hard"))
        s->ssl.ech = CURLECH_HARD;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(plen > 4 && !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(ptr, "ecl:", 4)) {
        ಆದರೆ(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_ECH_CONFIG, ptr + 4);
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(plen > 3 && !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(ptr, "pn:", 3)) {
        ಆದರೆ(!s->ssl.ech)
          s->ssl.ech = CURLECH_HARD;
        ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_ECH_ಬಹಿರಂಗ, ptr + 3);
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು setopt_ech(x, y) CURLE_NOT_BUILT_ಒಳ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_SSL) || defined(USE_SSH)
/* One of the options is used for both TLS and SSH */
ಸ್ಥಿರ CURLcode setopt_cptr_ssl(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                                ಅಕ್ಷರ *ptr)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆಯ್ಕೆ(option) {
  ಸ್ಥಿತಿ CURLOPT_KEYPASSWD:
    /*
     * String that holds the SSL or SSH private key password.
     */
    ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_ಕೀಲಿ_PASSWD, ptr);
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  ಸ್ಥಿತಿ CURLOPT_CAINFO:
    /*
     * Set CA info for SSL connection. Specify filename of the CA certificate
     */
    ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_CAFILE, ptr);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ದತ್ತಾಂಶ->ಗಣ.ssl.custom_cafile = TRUE;
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  ಸ್ಥಿತಿ CURLOPT_CAPATH:
    /*
     * Set CA path info for SSL connection. Specify directory name of the CA
     * certificates which have been prepared using openssl c_rehash utility.
     */
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_CA_ಮಾರ್ಗ)) {
      /* This does not work on Windows. */
      ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_CAPATH, ptr);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ದತ್ತಾಂಶ->ಗಣ.ssl.custom_capath = TRUE;
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  ಸ್ಥಿತಿ CURLOPT_CRLFILE:
    /*
     * Set CRL file info for SSL connection. Specify filename of the CRL
     * to check certificates revocation
     */
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_CRLFILE))
      ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_CRLFILE, ptr);
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  ಸ್ಥಿತಿ CURLOPT_SSL_CIPHER_LIST:
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_CIPHER_LIST))
      /* set a list of cipher we want to use in the SSL connection */
      ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_CIPHER_LIST, ptr);
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  ಸ್ಥಿತಿ CURLOPT_TLS13_CIPHERS:
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_TLS13_CIPHERSUITES))
      /* set preferred list of TLS 1.3 cipher suites */
      ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_CIPHER13_LIST, ptr);
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  ಸ್ಥಿತಿ CURLOPT_RANDOM_FILE:
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_EGDSOCKET:
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_SSL_CTX_ದತ್ತಾಂಶ:
    /*
     * Set an SSL_CTX callback parameter pointer
     */
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_SSL_CTX)) {
      ದತ್ತಾಂಶ->ಗಣ.ssl_fsslctxp = ptr;
      ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  ಸ್ಥಿತಿ CURLOPT_SSLCERT:
    /*
     * String that holds filename of the SSL certificate to use
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_CERT, ptr);
  ಸ್ಥಿತಿ CURLOPT_SSLCERTTYPE:
    /*
     * String that holds file type of the SSL certificate to use
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_CERT_TYPE, ptr);
  ಸ್ಥಿತಿ CURLOPT_SSLKEY:
    /*
     * String that holds filename of the SSL key to use
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_ಕೀಲಿ, ptr);
  ಸ್ಥಿತಿ CURLOPT_SSLKEYTYPE:
    /*
     * String that holds file type of the SSL key to use
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_ಕೀಲಿ_TYPE, ptr);
  ಸ್ಥಿತಿ CURLOPT_SSLENGINE:
    /*
     * String that holds the SSL crypto engine.
     */
    ಆದರೆ(Curl_ssl_backend() != CURLSSLBACKEND_OPENSSL)
      ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
    ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_ENGINE, ptr);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_ISSUERCERT:
    /*
     * Set Issuer certificate file
     * to check certificates issuer
     */
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_ISSUERCERT))
      ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_ISSUERCERT, ptr);
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  ಸ್ಥಿತಿ CURLOPT_SSL_EC_CURVES:
    /*
     * Set accepted curves in SSL connection setup.
     * Specify colon-delimited list of curve algorithm names.
     */
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_SSL_EC_CURVES))
      ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_EC_CURVES, ptr);
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  ಸ್ಥಿತಿ CURLOPT_SSL_SIGNATURE_ALGORITHMS:
    /*
     * Set accepted signature algorithms.
     * Specify colon-delimited list of signature scheme names.
     */
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_SIGNATURE_ALGORITHMS))
      ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_SIGNATURE_ALGORITHMS, ptr);
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  ಸ್ಥಿತಿ CURLOPT_PINNEDPUBLICKEY:
    /*
     * Set pinned public key for SSL connection.
     * Specify filename of the public key in DER format.
     */
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_PINNEDPUBKEY))
      ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSL_PINNEDPUBLICKEY, ptr);
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  ಸ್ಥಿತಿ CURLOPT_ECH:
    ಹಿಂತಿರುಗಿ setopt_ech(ದತ್ತಾಂಶ, ptr);
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
ಸ್ಥಿರ CURLcode setopt_cptr_http_mqtt(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      CURLoption option, ಅಕ್ಷರ *ptr)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ UserDefined *s = &ದತ್ತಾಂಶ->ಗಣ;

  ಆಯ್ಕೆ(option) {
  ಸ್ಥಿತಿ CURLOPT_COPYPOSTFIELDS:
    ಹಿಂತಿರುಗಿ setopt_copypostfields(ptr, s);

  ಸ್ಥಿತಿ CURLOPT_POSTFIELDS:
    /*
     * Like above, but use static data instead of copying it.
     */
    s->postfields = ptr;
    /* Release old copied data. */
    curlx_safefree(s->str_copypostfields);
    s->ವಿಧಾನ = HTTPREQ_POST;
    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  ಸ್ಥಿತಿ CURLOPT_TRAILERDATA:
    s->trailer_ದತ್ತಾಂಶ = ptr;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_ACCEPT_ENCODING:
    /*
     * String to use at the value of Accept-Encoding header.
     *
     * If the encoding is set to "" we use an Accept-Encoding header that
     * encompasses all the encodings we support.
     * If the encoding is set to NULL we do not send an Accept-Encoding header
     * and ignore any received Content-Encoding header.
     *
     */
    ಆದರೆ(ptr && !*ptr) {
      ptr = Curl_get_content_encodings();
      ಆದರೆ(ptr)
        ಫಲಿತಾಂಶ = CURL_EASY_STR_SETN(ದತ್ತಾಂಶ, STRING_ENCODING, ptr);
      ಇಲ್ಲದಿದ್ದರೆ
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_ENCODING, ptr);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_AWS
  ಸ್ಥಿತಿ CURLOPT_AWS_SIGV4:
    /*
     * String that is merged to some authentication
     * parameters are used by the algorithm.
     */
    ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_AWS_SIGV4, ptr);
    /*
     * Basic has been set by default; it needs to be unset here.
     */
    ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_AWS_SIGV4))
      s->httpauth = CURLAUTH_AWS_SIGV4;
    ಇಲ್ಲದಿದ್ದರೆ
      s->httpauth &= ~(uint32_t)CURLAUTH_AWS_SIGV4;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTPSIG
  ಸ್ಥಿತಿ CURLOPT_HTTPSIG_ಕೀಲಿ:
    ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_HTTPSIG_ಕೀಲಿ, ptr);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_HTTPSIG_KEYID:
    ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_HTTPSIG_KEYID, ptr);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_HTTPSIG_ಶೀರ್ಷಿಕೆಗಳು:
    ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_HTTPSIG_ಶೀರ್ಷಿಕೆಗಳು, ptr);
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ CURLOPT_REFERER: {
    /*
     * String to set in the HTTP Referer: field.
     */
    ರಚನೆ bufref *oldref = &ದತ್ತಾಂಶ->state.referer;
    /* free the old after the storing the new in case the input is actually
       pointing back to this */
    ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_ಗಣ_REFERER, ptr);
    Curl_bufref_free(oldref);
    ನಿಲ್ಲಿಸು;
  }

  ಸ್ಥಿತಿ CURLOPT_USERAGENT:
    /*
     * String to use in the HTTP User-Agent field
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_USERAGENT, ptr);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_COOKIES
  ಸ್ಥಿತಿ CURLOPT_COOKIE:
    /*
     * Cookie string to send to the remote server in the request.
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_COOKIE, ptr);

  ಸ್ಥಿತಿ CURLOPT_COOKIEFILE:
    ಹಿಂತಿರುಗಿ cookiefile(ದತ್ತಾಂಶ, ptr);

  ಸ್ಥಿತಿ CURLOPT_COOKIEJAR:
    /*
     * Set cookie filename to dump all cookies to when we are done.
     */
    ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_COOKIEJAR, ptr);
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      /*
       * Activate the cookie parser. This may or may not already
       * have been made.
       */
      ಆದರೆ(!ದತ್ತಾಂಶ->cookies)
        ದತ್ತಾಂಶ->cookies = Curl_cookie_init();
      ಆದರೆ(!ದತ್ತಾಂಶ->cookies)
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಇಲ್ಲದಿದ್ದರೆ
        ದತ್ತಾಂಶ->state.cookie_engine = TRUE;
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURLOPT_COOKIELIST:
    ಹಿಂತಿರುಗಿ cookielist(ದತ್ತಾಂಶ, ptr);
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_COOKIES */

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP */
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP || !CURL_DISABLE_MQTT */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
ಸ್ಥಿರ CURLcode setopt_cptr_ssh(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                                ಅಕ್ಷರ *ptr)
{
  ರಚನೆ UserDefined *s = &ದತ್ತಾಂಶ->ಗಣ;
  ಆಯ್ಕೆ(option) {
  ಸ್ಥಿತಿ CURLOPT_SSH_ಬಹಿರಂಗ_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa.pub file
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSH_ಬಹಿರಂಗ_ಕೀಲಿ, ptr);
  ಸ್ಥಿತಿ CURLOPT_SSH_ಖಾಸಗಿ_KEYFILE:
    /*
     * Use this file instead of the $HOME/.ssh/id_dsa file
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSH_ಖಾಸಗಿ_ಕೀಲಿ, ptr);
  ಸ್ಥಿತಿ CURLOPT_SSH_KEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_keyfunc_userp = ptr;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_SSH_HOST_ಬಹಿರಂಗ_ಕೀಲಿ_MD5:
    /*
     * Option to allow for the MD5 of the host public key to be checked
     * for validation purposes.
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSH_HOST_ಬಹಿರಂಗ_ಕೀಲಿ_MD5, ptr);
  ಸ್ಥಿತಿ CURLOPT_SSH_HOST_ಬಹಿರಂಗ_ಕೀಲಿ_SHA256:
    /*
     * Option to allow for the SHA256 of the host public key to be checked
     * for validation purposes.
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSH_HOST_ಬಹಿರಂಗ_ಕೀಲಿ_SHA256, ptr);
  ಸ್ಥಿತಿ CURLOPT_SSH_KNOWNHOSTS:
    /*
     * Store the filename to read known hosts from.
     */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_SSH_KNOWNHOSTS, ptr);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBSSH2
  ಸ್ಥಿತಿ CURLOPT_SSH_HOSTKEYDATA:
    /*
     * Custom client data to pass to the SSH keyfunc callback
     */
    s->ssh_hostkeyfunc_userp = ptr;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ /* USE_LIBSSH2 */
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_SSH */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
ಸ್ಥಿರ CURLcode setopt_cptr_ftp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                                ಅಕ್ಷರ *ptr)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ UserDefined *s = &ದತ್ತಾಂಶ->ಗಣ;
  ಆಯ್ಕೆ(option) {
  ಸ್ಥಿತಿ CURLOPT_FTPPORT:
    /*
     * Use FTP PORT, this also specifies which IP address to use
     */
    ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_FTPPORT, ptr);
    s->ftp_use_ಪೋರ್ಟ್ = !!CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_FTPPORT);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURLOPT_FTP_ACCOUNT:
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_FTP_ACCOUNT, ptr);

  ಸ್ಥಿತಿ CURLOPT_FTP_ALTERNATIVE_ಗಮ್ಯ_USER:
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_FTP_ALTERNATIVE_ಗಮ್ಯ_USER, ptr);

  ಸ್ಥಿತಿ CURLOPT_KRBLEVEL:
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ; /* removed in 8.17.0 */
  ಸ್ಥಿತಿ CURLOPT_CHUNK_ದತ್ತಾಂಶ:
    s->wildcardptr = ptr;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_FNMATCH_ದತ್ತಾಂಶ:
    s->fnmatch_ದತ್ತಾಂಶ = ptr;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_FTP */

ಸ್ಥಿರ CURLcode setopt_cptr_net(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                                ಅಕ್ಷರ *ptr)
{
  ಆಯ್ಕೆ(option) {
  ಸ್ಥಿತಿ CURLOPT_INTERFACE:
    /*
     * Set what interface or address/hostname to bind the socket to when
     * performing an operation and thus what from-IP your connection will use.
     */
    ಹಿಂತಿರುಗಿ setstropt_interface(ದತ್ತಾಂಶ, ptr);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLV_ARES
  ಸ್ಥಿತಿ CURLOPT_DNS_SERVERS:
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_DNS_SERVERS, ptr);

  ಸ್ಥಿತಿ CURLOPT_DNS_INTERFACE:
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_DNS_INTERFACE, ptr);

  ಸ್ಥಿತಿ CURLOPT_DNS_LOCAL_IP4:
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_DNS_LOCAL_IP4, ptr);

  ಸ್ಥಿತಿ CURLOPT_DNS_LOCAL_IP6:
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_DNS_LOCAL_IP6, ptr);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
  ಸ್ಥಿತಿ CURLOPT_UNIX_SOCKET_ಮಾರ್ಗ:
    ದತ್ತಾಂಶ->ಗಣ.abstract_unix_socket = FALSE;
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_UNIX_SOCKET_ಮಾರ್ಗ, ptr);

  ಸ್ಥಿತಿ CURLOPT_ABSTRACT_UNIX_SOCKET:
    ದತ್ತಾಂಶ->ಗಣ.abstract_unix_socket = TRUE;
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_UNIX_SOCKET_ಮಾರ್ಗ, ptr);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DOH
  ಸ್ಥಿತಿ CURLOPT_DOH_URL:
    {
      CURLcode ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_DOH, ptr);
      ದತ್ತಾಂಶ->ಗಣ.doh = !!CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_DOH);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }
}

ಸ್ಥಿರ CURLcode setopt_cptr_misc(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                                 ಅಕ್ಷರ *ptr)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ UserDefined *s = &ದತ್ತಾಂಶ->ಗಣ;

  ಆಯ್ಕೆ(option) {
  ಸ್ಥಿತಿ CURLOPT_REQUEST_ಗುರಿ:
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_ಗುರಿ, ptr);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_NETRC
  ಸ್ಥಿತಿ CURLOPT_NETRC_FILE:
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_NETRC_FILE, ptr);
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ CURLOPT_CUSTOMREQUEST:
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_CUSTOMREQUEST, ptr);

    /* we do not set s->method = HTTPREQ_CUSTOM; here, we continue as if we
       were using the already set type and this changes the actual request
       keyword */
  ಸ್ಥಿತಿ CURLOPT_SERVICE_NAME:
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_SERVICE_NAME, ptr);

  ಸ್ಥಿತಿ CURLOPT_HEADERDATA:
    s->writeheader = ptr;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_READDATA:
    s->ಒಳ_ಗಣ = ptr;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_WRITEDATA:
    s->ಹೊರಹೋಗು = ptr;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_DEBUGDATA:
    s->debugdata = ptr;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_PROGRESSDATA:
    s->ಪ್ರಗತಿ_client = ptr;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_SEEKDATA:
    s->seek_client = ptr;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_IOCTLDATA:
    s->ioctl_client = ptr;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_SOCKOPTDATA:
    s->sockopt_client = ptr;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_OPENSOCKETDATA:
    s->opensocket_client = ptr;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_RESOLVER_ಆರಂಭ_ದತ್ತಾಂಶ:
    s->resolver_ಆರಂಭ_client = ptr;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_CLOSESOCKETDATA:
    s->closesocket_client = ptr;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_PREREQDATA:
    s->prereq_userp = ptr;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_ERRORBUFFER:
    s->errorbuffer = ptr;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_URL:
    ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_ಗಣ_URL, ptr);
    Curl_bufref_ಗಣ(&ದತ್ತಾಂಶ->state.url,
                    CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_ಗಣ_URL), 0, ಖಾಲಿ);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURLOPT_USERPWD: {
    ಅಕ್ಷರ *u = ಖಾಲಿ, *p = ಖಾಲಿ;
    ಫಲಿತಾಂಶ = setstropt_userpwd(ptr, &u, &p);
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      ಫಲಿತಾಂಶ = CURL_EASY_STR_SETN(ದತ್ತಾಂಶ, STRING_USERNAME, u);
      u = ಖಾಲಿ;
    }
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      ಫಲಿತಾಂಶ = CURL_EASY_STR_SETN(ದತ್ತಾಂಶ, STRING_PASSWORD, p);
      p = ಖಾಲಿ;
    }
    curlx_free(u);
    curlx_free(p);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಸ್ಥಿತಿ CURLOPT_USERNAME:
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_USERNAME, ptr);

  ಸ್ಥಿತಿ CURLOPT_PASSWORD:
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_PASSWORD, ptr);

  ಸ್ಥಿತಿ CURLOPT_LOGIN_OPTIONS:
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_OPTIONS, ptr);

  ಸ್ಥಿತಿ CURLOPT_XOAUTH2_BEARER:
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_BEARER, ptr);
  ಸ್ಥಿತಿ CURLOPT_RANGE:
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_ಗಣ_RANGE, ptr);
  ಸ್ಥಿತಿ CURLOPT_ಖಾಸಗಿ:
    s->ಖಾಸಗಿ_ದತ್ತಾಂಶ = ptr;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_PROTOCOLS_STR:
    ಆದರೆ(ptr) {
      curl_prot_t protos;
      ಫಲಿತಾಂಶ = protocol2num(ptr, &protos);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        s->allowed_protocols = protos;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      /* make a NULL argument reset to default */
      s->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_REDIR_PROTOCOLS_STR:
    ಆದರೆ(ptr) {
      curl_prot_t protos;
      ಫಲಿತಾಂಶ = protocol2num(ptr, &protos);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        s->redir_protocols = protos;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      /* make a NULL argument reset to default */
      s->redir_protocols = (curl_prot_t)CURLPROTO_REDIR;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_DEFAULT_PROTOCOL:
    /* Set the protocol to use when the URL does not include any protocol */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_DEFAULT_PROTOCOL, ptr);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_SMTP
  ಸ್ಥಿತಿ CURLOPT_MAIL_FROM:
    /* Set the SMTP mail originator */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_MAIL_FROM, ptr);
  ಸ್ಥಿತಿ CURLOPT_MAIL_AUTH:
    /* Set the SMTP auth originator */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_MAIL_AUTH, ptr);
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ CURLOPT_SASL_AUTHZID:
    /* Authorization identity (identity to act as) */
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_SASL_AUTHZID, ptr);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_RTSP
  ಸ್ಥಿತಿ CURLOPT_RTSP_SESSION_ID:
    Curl_peer_unlink(&ದತ್ತಾಂಶ->state.rtsp_session_origin);
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_RTSP_SESSION_ID, ptr);
  ಸ್ಥಿತಿ CURLOPT_RTSP_ಹರಿವು_URI:
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_RTSP_ಹರಿವು_URI, ptr);
  ಸ್ಥಿತಿ CURLOPT_RTSP_TRANSPORT:
    ಹಿಂತಿರುಗಿ Curl_setstropt(ದತ್ತಾಂಶ, STRING_RTSP_TRANSPORT, ptr);
  ಸ್ಥಿತಿ CURLOPT_INTERLEAVEDATA:
    s->rtp_ಹೊರಹೋಗು = ptr;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_RTSP */
  ಸ್ಥಿತಿ CURLOPT_TLSAUTH_USERNAME:
  ಸ್ಥಿತಿ CURLOPT_TLSAUTH_PASSWORD:
  ಸ್ಥಿತಿ CURLOPT_TLSAUTH_TYPE:
  ಸ್ಥಿತಿ CURLOPT_PROXY_TLSAUTH_USERNAME:
  ಸ್ಥಿತಿ CURLOPT_PROXY_TLSAUTH_PASSWORD:
  ಸ್ಥಿತಿ CURLOPT_PROXY_TLSAUTH_TYPE:
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HSTS
  ಸ್ಥಿತಿ CURLOPT_HSTSREADDATA:
    s->hsts_read_userp = ptr;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_HSTSWRITEDATA:
    s->hsts_write_userp = ptr;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_HSTS: {
    ರಚನೆ curl_slist *h;
    ಆದರೆ(!ದತ್ತಾಂಶ->hsts) {
      ದತ್ತಾಂಶ->hsts = Curl_hsts_init();
      ಆದರೆ(!ದತ್ತಾಂಶ->hsts)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    ಆದರೆ(ptr) {
      ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_HSTS, ptr);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      /* this needs to build a list of filenames to read from, so that it can
         read them later, as we might get a shared HSTS handle to load them
         into */
      h = curl_slist_append(ದತ್ತಾಂಶ->state.hstslist, ptr);
      ಆದರೆ(!h) {
        curl_slist_free_all(ದತ್ತಾಂಶ->state.hstslist);
        ದತ್ತಾಂಶ->state.hstslist = ಖಾಲಿ;
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
      ದತ್ತಾಂಶ->state.hstslist = h; /* store the list for later use */
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* clear the list of HSTS files */
      curl_slist_free_all(ದತ್ತಾಂಶ->state.hstslist);
      ದತ್ತಾಂಶ->state.hstslist = ಖಾಲಿ;
      ಆದರೆ(!ದತ್ತಾಂಶ->share || !ದತ್ತಾಂಶ->share->hsts)
        /* throw away the HSTS cache unless shared */
        Curl_hsts_cleanup(&ದತ್ತಾಂಶ->hsts);
    }
    ನಿಲ್ಲಿಸು;
  }
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HSTS */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_ALTSVC
  ಸ್ಥಿತಿ CURLOPT_ALTSVC:
    ಆದರೆ(!ದತ್ತಾಂಶ->asi) {
      ದತ್ತಾಂಶ->asi = Curl_altsvc_init();
      ಆದರೆ(!ದತ್ತಾಂಶ->asi)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    ಫಲಿತಾಂಶ = Curl_setstropt(ದತ್ತಾಂಶ, STRING_ALTSVC, ptr);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;
    ಆದರೆ(ptr)
      ಹಿಂತಿರುಗಿ Curl_altsvc_ತುಂಬು(ದತ್ತಾಂಶ->asi, ptr);
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_ALTSVC */
  ಸ್ಥಿತಿ CURLOPT_ECH:
    ಹಿಂತಿರುಗಿ setopt_ech(ದತ್ತಾಂಶ, ptr);
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode setopt_cptr(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                            ಅಕ್ಷರ *ptr)
{
  ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode (*ptrfunc)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                              ಅಕ್ಷರ *ptr);
  /* Order by likeliness */
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ptrfunc setopt_call[] = {
    setopt_cptr_misc,
#ಆದರೆ defined(USE_SSL) || defined(USE_SSH)
    setopt_cptr_ssl,
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
    setopt_cptr_proxy,
#ಆದರೆ_ಅಂತ್ಯ
    setopt_cptr_net,
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
    setopt_cptr_ftp,
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
    setopt_cptr_ssh,
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_MQTT)
    setopt_cptr_http_mqtt,
#ಆದರೆ_ಅಂತ್ಯ
  };
  size_t i;

  ಗಾಗಿ(i = 0; i < CURL_ARRAYSIZE(setopt_call); i++) {
    CURLcode ಫಲಿತಾಂಶ = setopt_call[i](ದತ್ತಾಂಶ, option, ptr);
    ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_UNKNOWN_OPTION)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
}

ಸ್ಥಿರ CURLcode setopt_func(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                            va_list param)
{
  ರಚನೆ UserDefined *s = &ದತ್ತಾಂಶ->ಗಣ;
  ಆಯ್ಕೆ(option) {
  ಸ್ಥಿತಿ CURLOPT_PROGRESSFUNCTION:
    /*
     * Progress callback function
     */
    s->fprogress = va_arg(param, curl_ಪ್ರಗತಿ_callback);
    ಆದರೆ(s->fprogress)
      ದತ್ತಾಂಶ->ಪ್ರಗತಿ.callback = TRUE; /* no longer internal */
    ಇಲ್ಲದಿದ್ದರೆ
      ದತ್ತಾಂಶ->ಪ್ರಗತಿ.callback = FALSE; /* NULL enforces internal */
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURLOPT_XFERINFOFUNCTION:
    /*
     * Transfer info callback function
     */
    s->fxferinfo = va_arg(param, curl_xferinfo_callback);
    ಆದರೆ(s->fxferinfo)
      ದತ್ತಾಂಶ->ಪ್ರಗತಿ.callback = TRUE; /* no longer internal */
    ಇಲ್ಲದಿದ್ದರೆ
      ದತ್ತಾಂಶ->ಪ್ರಗತಿ.callback = FALSE; /* NULL enforces internal */

    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_DEBUGFUNCTION:
    /*
     * stderr write callback.
     */
    s->fdebug = va_arg(param, curl_debug_callback);
    /*
     * if the callback provided is NULL, it will use the default callback
     */
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_HEADERFUNCTION:
    /*
     * Set header write callback
     */
    s->fwrite_ಶೀರ್ಷಿಕೆ = va_arg(param, curl_write_callback);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_WRITEFUNCTION:
    /*
     * Set data write callback
     */
    s->fwrite_func = va_arg(param, curl_write_callback);
    ಆದರೆ(!s->fwrite_func)
#ಆದರೆ defined(__clang__) && __clang_major__ >= 16
#ಸೂಚನೆ clang diagnostic ತಳ್ಳು
#ಸೂಚನೆ clang diagnostic ignored "-Wcast-function-type-strict"
#ಆದರೆ_ಅಂತ್ಯ
      /* When set to NULL, reset to our internal default function */
      s->fwrite_func = (curl_write_callback)fwrite;
#ಆದರೆ defined(__clang__) && __clang_major__ >= 16
#ಸೂಚನೆ clang diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_READFUNCTION:
    /*
     * Read data callback
     */
    s->fread_func_ಗಣ = va_arg(param, curl_read_callback);
    ಆದರೆ(!s->fread_func_ಗಣ) {
      s->is_fread_ಗಣ = 0;
#ಆದರೆ defined(__clang__) && __clang_major__ >= 16
#ಸೂಚನೆ clang diagnostic ತಳ್ಳು
#ಸೂಚನೆ clang diagnostic ignored "-Wcast-function-type-strict"
#ಆದರೆ_ಅಂತ್ಯ
      /* When set to NULL, reset to our internal default function */
      s->fread_func_ಗಣ = (curl_read_callback)fread;
#ಆದರೆ defined(__clang__) && __clang_major__ >= 16
#ಸೂಚನೆ clang diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
      s->is_fread_ಗಣ = 1;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_SEEKFUNCTION:
    /*
     * Seek callback. Might be NULL.
     */
    s->seek_func = va_arg(param, curl_seek_callback);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_IOCTLFUNCTION:
    /*
     * I/O control callback. Might be NULL.
     */
    s->ioctl_func = va_arg(param, curl_ioctl_callback);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_SSL_CTX_FUNCTION:
    /*
     * Set an SSL_CTX callback
     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_SSL_CTX)) {
      s->ssl_fsslctx = va_arg(param, curl_ssl_ctx_callback);
      ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
      ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;

  ಸ್ಥಿತಿ CURLOPT_SOCKOPTFUNCTION:
    /*
     * socket callback function: called after socket() but before connect()
     */
    s->fsockopt = va_arg(param, curl_sockopt_callback);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURLOPT_OPENSOCKETFUNCTION:
    /*
     * open/create socket callback function: called instead of socket(),
     * before connect()
     */
    s->fopensocket = va_arg(param, curl_opensocket_callback);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURLOPT_CLOSESOCKETFUNCTION:
    /*
     * close socket callback function: called instead of close()
     * when shutting down a connection
     */
    s->fclosesocket = va_arg(param, curl_closesocket_callback);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURLOPT_RESOLVER_ಆರಂಭ_FUNCTION:
    /*
     * resolver start callback function: called before a new resolver request
     * is started
     */
    s->resolver_ಆರಂಭ = va_arg(param, curl_resolver_ಆರಂಭ_callback);
    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBSSH2
  ಸ್ಥಿತಿ CURLOPT_SSH_HOSTKEYFUNCTION:
    /* the callback to check the hostkey without the knownhost file */
    s->ssh_hostkeyfunc = va_arg(param, curl_sshhostkeycallback);
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

  ಸ್ಥಿತಿ CURLOPT_SSH_KEYFUNCTION:
    /* setting to NULL is fine since the ssh.c functions themselves will
       then revert to use the internal default */
    s->ssh_keyfunc = va_arg(param, curl_sshkeycallback);
    ನಿಲ್ಲಿಸು;

#ಆದರೆ_ಅಂತ್ಯ /* USE_SSH */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_RTSP
  ಸ್ಥಿತಿ CURLOPT_INTERLEAVEFUNCTION:
    /* Set the user defined RTP write function */
    s->fwrite_rtp = va_arg(param, curl_write_callback);
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
  ಸ್ಥಿತಿ CURLOPT_CHUNK_BGN_FUNCTION:
    s->chunk_bgn = va_arg(param, curl_chunk_bgn_callback);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_CHUNK_ಕೊನೆ_FUNCTION:
    s->chunk_ಕೊನೆ = va_arg(param, curl_chunk_ಕೊನೆ_callback);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_FNMATCH_FUNCTION:
    s->fnmatch = va_arg(param, curl_fnmatch_callback);
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  ಸ್ಥಿತಿ CURLOPT_TRAILERFUNCTION:
    s->trailer_callback = va_arg(param, curl_trailer_callback);
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HSTS
  ಸ್ಥಿತಿ CURLOPT_HSTSREADFUNCTION:
    s->hsts_read = va_arg(param, curl_hstsread_callback);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_HSTSWRITEFUNCTION:
    s->hsts_write = va_arg(param, curl_hstswrite_callback);
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ CURLOPT_PREREQFUNCTION:
    s->fprereq = va_arg(param, curl_prereq_callback);
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode setopt_offt(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                            curl_off_t offt)
{
  ರಚನೆ UserDefined *s = &ದತ್ತಾಂಶ->ಗಣ;
  ಆಯ್ಕೆ(option) {
  ಸ್ಥಿತಿ CURLOPT_TIMEVALUE_LARGE:
    /*
     * This is the value to compare with the remote document with the
     * method set with CURLOPT_TIMECONDITION
     */
    s->timevalue = (time_t)offt;
    ನಿಲ್ಲಿಸು;

    /* MQTT "borrows" some of the HTTP options */
  ಸ್ಥಿತಿ CURLOPT_POSTFIELDSIZE_LARGE:
    /*
     * The size of the POSTFIELD data to prevent libcurl to do strlen() to
     * figure it out. Enables binary posts.
     */
    ಆದರೆ(offt < -1)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

    ಆದರೆ(s->postfieldsize < offt && s->str_copypostfields) {
      /* Previous CURLOPT_COPYPOSTFIELDS is no longer valid. */
      curlx_safefree(s->str_copypostfields);
      s->postfields = ಖಾಲಿ;
    }
    s->postfieldsize = offt;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_INFILESIZE_LARGE:
    /*
     * If known, this should inform curl about the file size of the
     * to-be-uploaded file.
     */
    ಆದರೆ(offt < -1)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    s->filesize = offt;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_MAX_SEND_ವೇಗ_LARGE:
    /*
     * When transfer uploads are faster than CURLOPT_MAX_SEND_SPEED_LARGE
     * bytes per second the transfer is throttled..
     */
    ಆದರೆ(offt < 0)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    s->max_send_ವೇಗ = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_MAX_RECV_ವೇಗ_LARGE:
    /*
     * When receiving data faster than CURLOPT_MAX_RECV_SPEED_LARGE bytes per
     * second the transfer is throttled..
     */
    ಆದರೆ(offt < 0)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    s->max_recv_ವೇಗ = offt;
    /* use minimal burst rate of 32k. some protocol batch IO */
    Curl_rlimit_change(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.rlimit, offt,
                     CURLMAX(offt, (32 * 1024)));
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_RESUME_FROM_LARGE:
    /*
     * Resume transfer at the given file position
     */
    ಆದರೆ(offt < -1)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    s->ಗಣ_resume_from = offt;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLOPT_MAXFILESIZE_LARGE:
    /*
     * Set the maximum size of a file to download.
     */
    ಆದರೆ(offt < 0)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    s->max_filesize = offt;
    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode setopt_blob(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                            ರಚನೆ curl_blob *blob)
{
  ರಚನೆ UserDefined *s = &ದತ್ತಾಂಶ->ಗಣ;
  ಆಯ್ಕೆ(option) {
  ಸ್ಥಿತಿ CURLOPT_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use
     */
    ಹಿಂತಿರುಗಿ Curl_setblobopt(&s->blobs[BLOB_CERT], blob);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಸ್ಥಿತಿ CURLOPT_PROXY_SSLCERT_BLOB:
    /*
     * Blob that holds file content of the SSL certificate to use for proxy
     */
    ಹಿಂತಿರುಗಿ Curl_setblobopt(&s->blobs[BLOB_CERT_PROXY], blob);
  ಸ್ಥಿತಿ CURLOPT_PROXY_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use for proxy
     */
    ಹಿಂತಿರುಗಿ Curl_setblobopt(&s->blobs[BLOB_ಕೀಲಿ_PROXY], blob);
  ಸ್ಥಿತಿ CURLOPT_PROXY_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection proxy.
     * Specify entire PEM of the CA certificate
     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_CAINFO_BLOB)) {
      CURLcode ಫಲಿತಾಂಶ = Curl_setblobopt(&s->blobs[BLOB_CAINFO_PROXY], blob);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        s->proxy_ssl.custom_cablob = TRUE;
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  ಸ್ಥಿತಿ CURLOPT_PROXY_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_ISSUERCERT_BLOB))
      ಹಿಂತಿರುಗಿ Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT_PROXY], blob);
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ CURLOPT_SSLKEY_BLOB:
    /*
     * Blob that holds file content of the SSL key to use
     */
    ಹಿಂತಿರುಗಿ Curl_setblobopt(&s->blobs[BLOB_ಕೀಲಿ], blob);
  ಸ್ಥಿತಿ CURLOPT_CAINFO_BLOB:
    /*
     * Blob that holds CA info for SSL connection.
     * Specify entire PEM of the CA certificate
     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_CAINFO_BLOB)) {
      CURLcode ಫಲಿತಾಂಶ = Curl_setblobopt(&s->blobs[BLOB_CAINFO], blob);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        s->ssl.custom_cablob = TRUE;
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  ಸ್ಥಿತಿ CURLOPT_ISSUERCERT_BLOB:
    /*
     * Blob that holds Issuer certificate to check certificates issuer
     */
    ಆದರೆ(Curl_ssl_supports(ದತ್ತಾಂಶ, SSLSUPP_ISSUERCERT_BLOB))
      ಹಿಂತಿರುಗಿ Curl_setblobopt(&s->blobs[BLOB_SSL_ISSUERCERT], blob);
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;

  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }
  /* unreachable */
}

/*
 * Do not make Curl_vsetopt() static: it is called from
 * projects/OS400/ccsidcurl.c.
 */
CURLcode Curl_vsetopt(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option, va_list param)
{
  ಆದರೆ(option < CURLOPTTYPE_OBJECTPOINT)
    ಹಿಂತಿರುಗಿ setopt_long(ದತ್ತಾಂಶ, option, va_arg(param, ದೀರ್ಘ));
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(option < CURLOPTTYPE_FUNCTIONPOINT) {
    /* unfortunately, different pointer types cannot be identified any other
       way than being listed explicitly */
    ಆಯ್ಕೆ(option) {
    ಸ್ಥಿತಿ CURLOPT_HTTPHEADER:
    ಸ್ಥಿತಿ CURLOPT_QUOTE:
    ಸ್ಥಿತಿ CURLOPT_POSTQUOTE:
    ಸ್ಥಿತಿ CURLOPT_TELNETOPTIONS:
    ಸ್ಥಿತಿ CURLOPT_PREQUOTE:
    ಸ್ಥಿತಿ CURLOPT_HTTP200ALIASES:
    ಸ್ಥಿತಿ CURLOPT_MAIL_RCPT:
    ಸ್ಥಿತಿ CURLOPT_RESOLVE:
    ಸ್ಥಿತಿ CURLOPT_PROXYHEADER:
    ಸ್ಥಿತಿ CURLOPT_CONNECT_ಗಮ್ಯ:
      ಹಿಂತಿರುಗಿ setopt_slist(ದತ್ತಾಂಶ, option, va_arg(param, ರಚನೆ curl_slist *));
    ಸ್ಥಿತಿ CURLOPT_HTTPPOST:         /* curl_httppost * */
    ಸ್ಥಿತಿ CURLOPT_MIMEPOST:         /* curl_mime * */
    ಸ್ಥಿತಿ CURLOPT_STDERR:           /* FILE * */
    ಸ್ಥಿತಿ CURLOPT_SHARE:            /* CURLSH * */
    ಸ್ಥಿತಿ CURLOPT_CURLU:            /* CURLU * */
      ಹಿಂತಿರುಗಿ setopt_ಸೂಚಕಗಳು(ದತ್ತಾಂಶ, option, param);
    ಸ್ಥಿತಿ CURLOPT_ಹರಿವು_DEPENDS:   /* CURL * */
    ಸ್ಥಿತಿ CURLOPT_ಹರಿವು_DEPENDS_E: /* CURL * */
      ಹಿಂತಿರುಗಿ CURLE_OK;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ನಿಲ್ಲಿಸು;
    }
    /* the char pointer options */
    ಹಿಂತಿರುಗಿ setopt_cptr(ದತ್ತಾಂಶ, option, va_arg(param, ಅಕ್ಷರ *));
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(option < CURLOPTTYPE_OFF_T)
    ಹಿಂತಿರುಗಿ setopt_func(ದತ್ತಾಂಶ, option, param);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(option < CURLOPTTYPE_BLOB)
    ಹಿಂತಿರುಗಿ setopt_offt(ದತ್ತಾಂಶ, option, va_arg(param, curl_off_t));
  ಹಿಂತಿರುಗಿ setopt_blob(ದತ್ತಾಂಶ, option, va_arg(param, ರಚನೆ curl_blob *));
}

/*
 * curl_easy_setopt() is the external interface for setting options on an
 * easy handle.
 *
 * NOTE: This is one of few API functions that are allowed to be called from
 * within a callback.
 */
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು curl_easy_setopt
CURLcode curl_easy_setopt(CURL *curl, CURLoption option, ...)
{
  ರಚನೆ Curl_eapi_guard guard;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(CURL_EAPI_ENTER(&guard, curl, easy_setopt, &ಫಲಿತಾಂಶ)) {
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = curl;
    va_list arg;

    va_start(arg, option);

    ಫಲಿತಾಂಶ = Curl_vsetopt(ದತ್ತಾಂಶ, option, arg);

    va_end(arg);
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_BAD_FUNCTION_ವಾದ)
      failf(ದತ್ತಾಂಶ, "setopt 0x%x got bad argument", (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)option);
  }
  CURL_EAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
