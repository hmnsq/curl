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
/*
 * IDN conversions
 */
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "idn.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBIDN2
#ಅನ್ತರ್ಭಾವ <idn2.h>

#ಆದರೆ defined(_WIN32) && defined(UNICODE)
#ವ್ಯಾಖ್ಯಾನಿಸು IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((ಸ್ಥಿರಾಂಕ uint8_t *)name, (uint8_t **)host, flags)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(name), (ಅಕ್ಷರ **)(host), flags)
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APPLE_IDN
#ಅನ್ತರ್ಭಾವ <unicode/uidna.h>
#ಅನ್ತರ್ಭಾವ <iconv.h>
#ಅನ್ತರ್ಭಾವ <langinfo.h>

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_HOST_ಉದ್ದಳತೆ 512

ಸ್ಥಿರ CURLcode iconv_ಗಮ್ಯ_utf8(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳ, size_t inlen,
                              ಅಕ್ಷರ **ಹೊರಹೋಗು, size_t *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  ಆದರೆ(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    ಅಕ್ಷರ *iconv_ಒಳ = (ಅಕ್ಷರ *)CURL_UNCONST(ಒಳ);
    size_t iconv_inlen = inlen;
    size_t iconv_ಫಲಿತಾಂಶ = iconv(cd, &iconv_ಒಳ, &iconv_inlen,
                                ಹೊರಹೋಗು, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    ಆದರೆ(iconv_ಫಲಿತಾಂಶ == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      ಆದರೆ(errno == ENOMEM)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
    }

    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* !checksrc! disable ERRNOVAR 1 */
    ಆದರೆ(errno == ENOMEM)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
}

ಸ್ಥಿರ CURLcode mac_idn_ಗಮ್ಯ_ascii(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳ, ಅಕ್ಷರ **ಹೊರಹೋಗು)
{
  size_t inlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಒಳ);
  ಆದರೆ(inlen < MAX_HOST_ಉದ್ದಳತೆ) {
    ಅಕ್ಷರ iconv_ಬಫರ್[MAX_HOST_ಉದ್ದಳತೆ] = { 0 };
    ಅಕ್ಷರ *iconv_outptr = iconv_ಬಫರ್;
    size_t iconv_outlen = ಗಾತ್ರ(iconv_ಬಫರ್);
    CURLcode iconv_ಫಲಿತಾಂಶ = iconv_ಗಮ್ಯ_utf8(ಒಳ, inlen,
                                          &iconv_outptr, &iconv_outlen);
    ಆದರೆ(!iconv_ಫಲಿತಾಂಶ) {
      UError_ಸಂಕೇತ err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_ಗಮ್ಯ_ASCII, &err);
      ಆದರೆ(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        ಅಕ್ಷರ ಬಫರ್[MAX_HOST_ಉದ್ದಳತೆ] = { 0 };
        (ಶೂನ್ಯ)uidna_name_ಗಮ್ಯ_ASCII_UTF8(idna, iconv_ಬಫರ್, (ಪೂರ್ಣಾಂಕ)iconv_outlen,
                                     ಬಫರ್, ಗಾತ್ರ(ಬಫರ್) - 1, &info, &err);
        uidna_close(idna);
        ಆದರೆ(!U_FAILURE(err) && !info.errors) {
          *ಹೊರಹೋಗು = curlx_strdup(ಬಫರ್);
          ಆದರೆ(*ಹೊರಹೋಗು)
            ಹಿಂತಿರುಗಿ CURLE_OK;
          ಇಲ್ಲದಿದ್ದರೆ
            ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        }
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ iconv_ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
}

ಸ್ಥಿರ CURLcode mac_ascii_ಗಮ್ಯ_idn(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳ, ಅಕ್ಷರ **ಹೊರಹೋಗು)
{
  size_t inlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಒಳ);
  ಆದರೆ(inlen < MAX_HOST_ಉದ್ದಳತೆ) {
    UError_ಸಂಕೇತ err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_ಗಮ್ಯ_UNICODE, &err);
    ಆದರೆ(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      ಅಕ್ಷರ ಬಫರ್[MAX_HOST_ಉದ್ದಳತೆ] = { 0 };
      (ಶೂನ್ಯ)uidna_name_ಗಮ್ಯ_Unicode_UTF8(idna, ಒಳ, -1, ಬಫರ್,
                                    ಗಾತ್ರ(ಬಫರ್) - 1, &info, &err);
      uidna_close(idna);
      ಆದರೆ(!U_FAILURE(err)) {
        *ಹೊರಹೋಗು = curlx_strdup(ಬಫರ್);
        ಆದರೆ(*ಹೊರಹೋಗು)
          ಹಿಂತಿರುಗಿ CURLE_OK;
        ಇಲ್ಲದಿದ್ದರೆ
          ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#ವ್ಯಾಖ್ಯಾನಿಸು IDN_MAX_ಉದ್ದಳತೆ 255

ಸ್ಥಿರ ಅಕ್ಷರ *idn_curlx_convert_wchar_ಗಮ್ಯ_UTF8(ಸ್ಥಿರಾಂಕ wchar_t *str_w, ಪೂರ್ಣಾಂಕ chars)
{
  ಅಕ್ಷರ *str_utf8 = ಖಾಲಿ;
  ಪೂರ್ಣಾಂಕ ಬೈಟುಗಳು = Wide_Char_ಗಮ್ಯ_Multi_Byte(CP_UTF8, 0, str_w, chars, ಖಾಲಿ, 0,
                                  ಖಾಲಿ, ಖಾಲಿ);
  ಆದರೆ(ಬೈಟುಗಳು > 0) {
    str_utf8 = curlx_malloc(ಬೈಟುಗಳು);
    ಆದರೆ(str_utf8) {
      ಆದರೆ(Wide_Char_ಗಮ್ಯ_Multi_Byte(CP_UTF8, 0, str_w, chars, str_utf8, ಬೈಟುಗಳು,
                             ಖಾಲಿ, ಖಾಲಿ) == 0) {
        curlx_free(str_utf8);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
      }
    }
  }
  ಹಿಂತಿರುಗಿ str_utf8;
}

ಸ್ಥಿರ CURLcode win32_idn_ಗಮ್ಯ_ascii(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳ, ಅಕ್ಷರ **ಹೊರಹೋಗು)
{
  wchar_t ಒಳ_w[IDN_MAX_ಉದ್ದಳತೆ];
  ಪೂರ್ಣಾಂಕ ಒಳ_w_len;
  *ಹೊರಹೋಗು = ಖಾಲಿ;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  ಒಳ_w_len = Multi_Byte_ಗಮ್ಯ_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 ಒಳ, -1, ಒಳ_w, IDN_MAX_ಉದ್ದಳತೆ);
  ಆದರೆ(ಒಳ_w_len) {
    wchar_t punycode[IDN_MAX_ಉದ್ದಳತೆ];
    ಪೂರ್ಣಾಂಕ chars = Idn_ಗಮ್ಯ_Ascii(0, ಒಳ_w, ಒಳ_w_len, punycode, IDN_MAX_ಉದ್ದಳತೆ);
    ಆದರೆ(chars > 0) {
      *ಹೊರಹೋಗು = idn_curlx_convert_wchar_ಗಮ್ಯ_UTF8(punycode, chars);
      ಆದರೆ(!*ಹೊರಹೋಗು)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode win32_ascii_ಗಮ್ಯ_idn(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳ, ಅಕ್ಷರ **ಹೊರಹೋಗು)
{
  wchar_t ಒಳ_w[IDN_MAX_ಉದ್ದಳತೆ];
  ಪೂರ್ಣಾಂಕ ಒಳ_w_len;
  *ಹೊರಹೋಗು = ಖಾಲಿ;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  ಒಳ_w_len = Multi_Byte_ಗಮ್ಯ_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 ಒಳ, -1, ಒಳ_w, IDN_MAX_ಉದ್ದಳತೆ);
  ಆದರೆ(ಒಳ_w_len) {
    WCHAR idn[IDN_MAX_ಉದ್ದಳತೆ]; /* stores a UTF-16 string */
    ಪೂರ್ಣಾಂಕ chars = Idn_ಗಮ್ಯ_Unicode(0, ಒಳ_w, ಒಳ_w_len, idn, IDN_MAX_ಉದ್ದಳತೆ);
    ಆದರೆ(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *ಹೊರಹೋಗು = idn_curlx_convert_wchar_ಗಮ್ಯ_UTF8(idn, chars);
      ಆದರೆ(!*ಹೊರಹೋಗು)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname)
{
  ಆದರೆ(hostname) {
    ರಚನೆ Curl_str s;
    s.str = hostname;
    s.len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(hostname);
    ಹಿಂತಿರುಗಿ Curl_is_ASCII_str(&s);
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

bool Curl_is_ASCII_str(ರಚನೆ Curl_str *s)
{
  ಆದರೆ(s && s->len) {
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ch = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)s->str;
    size_t i;
    ಗಾಗಿ(i = 0; i < s->len; ++i) {
      ಆದರೆ(ch[i] & 0x80)
        ಹಿಂತಿರುಗಿ FALSE;
    }
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IDN
/*
 * Curl_idn_decode() returns an allocated IDN decoded string if it was
 * possible. NULL on error.
 *
 * CURLE_URL_MALFORMAT - the hostname could not be converted
 * CURLE_OUT_OF_MEMORY - memory problem
 *
 */
ಸ್ಥಿರ CURLcode idn_decode(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳಹರಿವು, ಅಕ್ಷರ **output)
{
  ಅಕ್ಷರ *decoded = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBIDN2
  ಆದರೆ(idn2_check_ಆವೃತ್ತಿ(IDN2_ಆವೃತ್ತಿ)) {
    ಪೂರ್ಣಾಂಕ flags = IDN2_NFC_ಒಳಹರಿವು
#ಆದರೆ IDN2_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#ಆದರೆ_ಅಂತ್ಯ
      ;
    ಪೂರ್ಣಾಂಕ rc = IDN2_LOOKUP(ಒಳಹರಿವು, &decoded, flags);
    ಆದರೆ(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(ಒಳಹರಿವು, &decoded, IDN2_TRANSITIONAL);
    ಆದರೆ(rc != IDN2_OK)
      ಫಲಿತಾಂಶ = CURLE_URL_MALFORMAT;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    /* a too old libidn2 version */
    ಫಲಿತಾಂಶ = CURLE_NOT_BUILT_ಒಳ;
#ಅಥವಾಆದರೆ defined(USE_WIN32_IDN)
  ಫಲಿತಾಂಶ = win32_idn_ಗಮ್ಯ_ascii(ಒಳಹರಿವು, &decoded);
#ಅಥವಾಆದರೆ defined(USE_APPLE_IDN)
  ಫಲಿತಾಂಶ = mac_idn_ಗಮ್ಯ_ascii(ಒಳಹರಿವು, &decoded);
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    *output = decoded;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode idn_encode(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *puny, ಅಕ್ಷರ **output)
{
  ಅಕ್ಷರ *enc = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBIDN2
  ಪೂರ್ಣಾಂಕ rc = idn2_ಗಮ್ಯ_unicode_8z8z(puny, &enc, 0);
  ಆದರೆ(rc != IDNA_SUCCESS)
    ಹಿಂತಿರುಗಿ rc == IDNA_MALLOC_ERROR ? CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ : CURLE_URL_MALFORMAT;
#ಅಥವಾಆದರೆ defined(USE_WIN32_IDN)
  CURLcode ಫಲಿತಾಂಶ = win32_ascii_ಗಮ್ಯ_idn(puny, &enc);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
#ಅಥವಾಆದರೆ defined(USE_APPLE_IDN)
  CURLcode ಫಲಿತಾಂಶ = mac_ascii_ಗಮ್ಯ_idn(puny, &enc);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ
  *output = enc;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_idn_decode(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳಹರಿವು, ಅಕ್ಷರ **output)
{
  ಅಕ್ಷರ *d = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = idn_decode(ಒಳಹರಿವು, &d);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBIDN2
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ಅಕ್ಷರ *c = curlx_strdup(d);
    idn2_free(d);
    ಆದರೆ(c)
      d = c;
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ಆದರೆ(!d[0]) { /* ended up zero length, not acceptable */
      ಫಲಿತಾಂಶ = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      *output = d;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_idn_encode(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *puny, ಅಕ್ಷರ **output)
{
  ಅಕ್ಷರ *d = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = idn_encode(puny, &d);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBIDN2
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ಅಕ್ಷರ *c = curlx_strdup(d);
    idn2_free(d);
    ಆದರೆ(c)
      d = c;
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    *output = d;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
ಶೂನ್ಯ Curl_free_idnconverted_hostname(ರಚನೆ hostname *host)
{
  curlx_safefree(host->encalloc);
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(ರಚನೆ hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  ಆದರೆ(!Curl_is_ASCII_name(host->name)) {
    ಅಕ್ಷರ *decoded;
    CURLcode ಫಲಿತಾಂಶ = Curl_idn_decode(host->name, &decoded);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
