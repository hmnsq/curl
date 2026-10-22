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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "idn.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBIDN2
#ਅਨ੍ਤਰ੍ਭਾਵ <idn2.h>

#ਜੇ defined(_WIN32) && defined(UNICODE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((ਅਚਲ uint8_t *)name, (uint8_t **)host, flags)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((ਅਚਲ ਅੱਖਰ *)(name), (ਅੱਖਰ **)(host), flags)
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APPLE_IDN
#ਅਨ੍ਤਰ੍ਭਾਵ <unicode/uidna.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <iconv.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <langinfo.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_HOST_ਲੰਮਾਈ 512

ਸਥਿਰ CURLcode iconv_ਮੰਜ਼ਿਲ_utf8(ਅਚਲ ਅੱਖਰ *ਵਿੱਚ, size_t inlen,
                              ਅੱਖਰ **ਬਾਹਰਲਾ, size_t *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  ਜੇ(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    ਅੱਖਰ *iconv_ਵਿੱਚ = (ਅੱਖਰ *)CURL_UNCONST(ਵਿੱਚ);
    size_t iconv_inlen = inlen;
    size_t iconv_ਨਤੀਜਾ = iconv(cd, &iconv_ਵਿੱਚ, &iconv_inlen,
                                ਬਾਹਰਲਾ, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    ਜੇ(iconv_ਨਤੀਜਾ == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      ਜੇ(errno == ENOMEM)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਨਹੀਂਤਾਂ
        ਵਾਪਸ CURLE_URL_MALFORMAT;
    }

    ਵਾਪਸ CURLE_OK;
  }
  ਨਹੀਂਤਾਂ {
    /* !checksrc! disable ERRNOVAR 1 */
    ਜੇ(errno == ENOMEM)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ CURLE_FAILED_INIT;
  }
}

ਸਥਿਰ CURLcode mac_idn_ਮੰਜ਼ਿਲ_ascii(ਅਚਲ ਅੱਖਰ *ਵਿੱਚ, ਅੱਖਰ **ਬਾਹਰਲਾ)
{
  size_t inlen = ਲੜੀ_ਲੰਬਾਈ(ਵਿੱਚ);
  ਜੇ(inlen < MAX_HOST_ਲੰਮਾਈ) {
    ਅੱਖਰ iconv_ਬਫਰ[MAX_HOST_ਲੰਮਾਈ] = { 0 };
    ਅੱਖਰ *iconv_outptr = iconv_ਬਫਰ;
    size_t iconv_outlen = ਆਕਾਰ(iconv_ਬਫਰ);
    CURLcode iconv_ਨਤੀਜਾ = iconv_ਮੰਜ਼ਿਲ_utf8(ਵਿੱਚ, inlen,
                                          &iconv_outptr, &iconv_outlen);
    ਜੇ(!iconv_ਨਤੀਜਾ) {
      UError_ਕੋਡ err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_ਮੰਜ਼ਿਲ_ASCII, &err);
      ਜੇ(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        ਅੱਖਰ ਬਫਰ[MAX_HOST_ਲੰਮਾਈ] = { 0 };
        (ਖਾਲੀ)uidna_name_ਮੰਜ਼ਿਲ_ASCII_UTF8(idna, iconv_ਬਫਰ, (ਪੂਰਨ)iconv_outlen,
                                     ਬਫਰ, ਆਕਾਰ(ਬਫਰ) - 1, &info, &err);
        uidna_close(idna);
        ਜੇ(!U_FAILURE(err) && !info.errors) {
          *ਬਾਹਰਲਾ = curlx_strdup(ਬਫਰ);
          ਜੇ(*ਬਾਹਰਲਾ)
            ਵਾਪਸ CURLE_OK;
          ਨਹੀਂਤਾਂ
            ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        }
      }
    }
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ iconv_ਨਤੀਜਾ;
  }
  ਵਾਪਸ CURLE_URL_MALFORMAT;
}

ਸਥਿਰ CURLcode mac_ascii_ਮੰਜ਼ਿਲ_idn(ਅਚਲ ਅੱਖਰ *ਵਿੱਚ, ਅੱਖਰ **ਬਾਹਰਲਾ)
{
  size_t inlen = ਲੜੀ_ਲੰਬਾਈ(ਵਿੱਚ);
  ਜੇ(inlen < MAX_HOST_ਲੰਮਾਈ) {
    UError_ਕੋਡ err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_ਮੰਜ਼ਿਲ_UNICODE, &err);
    ਜੇ(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      ਅੱਖਰ ਬਫਰ[MAX_HOST_ਲੰਮਾਈ] = { 0 };
      (ਖਾਲੀ)uidna_name_ਮੰਜ਼ਿਲ_Unicode_UTF8(idna, ਵਿੱਚ, -1, ਬਫਰ,
                                    ਆਕਾਰ(ਬਫਰ) - 1, &info, &err);
      uidna_close(idna);
      ਜੇ(!U_FAILURE(err)) {
        *ਬਾਹਰਲਾ = curlx_strdup(ਬਫਰ);
        ਜੇ(*ਬਾਹਰਲਾ)
          ਵਾਪਸ CURLE_OK;
        ਨਹੀਂਤਾਂ
          ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
    }
  }
  ਵਾਪਸ CURLE_URL_MALFORMAT;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IDN_MAX_ਲੰਮਾਈ 255

ਸਥਿਰ ਅੱਖਰ *idn_curlx_convert_wchar_ਮੰਜ਼ਿਲ_UTF8(ਅਚਲ wchar_t *str_w, ਪੂਰਨ chars)
{
  ਅੱਖਰ *str_utf8 = ਨਿਰਾ;
  ਪੂਰਨ ਬਾਈਟਾਂ = Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(CP_UTF8, 0, str_w, chars, ਨਿਰਾ, 0,
                                  ਨਿਰਾ, ਨਿਰਾ);
  ਜੇ(ਬਾਈਟਾਂ > 0) {
    str_utf8 = curlx_malloc(ਬਾਈਟਾਂ);
    ਜੇ(str_utf8) {
      ਜੇ(Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(CP_UTF8, 0, str_w, chars, str_utf8, ਬਾਈਟਾਂ,
                             ਨਿਰਾ, ਨਿਰਾ) == 0) {
        curlx_free(str_utf8);
        ਵਾਪਸ ਨਿਰਾ;
      }
    }
  }
  ਵਾਪਸ str_utf8;
}

ਸਥਿਰ CURLcode win32_idn_ਮੰਜ਼ਿਲ_ascii(ਅਚਲ ਅੱਖਰ *ਵਿੱਚ, ਅੱਖਰ **ਬਾਹਰਲਾ)
{
  wchar_t ਵਿੱਚ_w[IDN_MAX_ਲੰਮਾਈ];
  ਪੂਰਨ ਵਿੱਚ_w_len;
  *ਬਾਹਰਲਾ = ਨਿਰਾ;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  ਵਿੱਚ_w_len = Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 ਵਿੱਚ, -1, ਵਿੱਚ_w, IDN_MAX_ਲੰਮਾਈ);
  ਜੇ(ਵਿੱਚ_w_len) {
    wchar_t punycode[IDN_MAX_ਲੰਮਾਈ];
    ਪੂਰਨ chars = Idn_ਮੰਜ਼ਿਲ_Ascii(0, ਵਿੱਚ_w, ਵਿੱਚ_w_len, punycode, IDN_MAX_ਲੰਮਾਈ);
    ਜੇ(chars > 0) {
      *ਬਾਹਰਲਾ = idn_curlx_convert_wchar_ਮੰਜ਼ਿਲ_UTF8(punycode, chars);
      ਜੇ(!*ਬਾਹਰਲਾ)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ CURLE_URL_MALFORMAT;
  }
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ CURLE_URL_MALFORMAT;

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode win32_ascii_ਮੰਜ਼ਿਲ_idn(ਅਚਲ ਅੱਖਰ *ਵਿੱਚ, ਅੱਖਰ **ਬਾਹਰਲਾ)
{
  wchar_t ਵਿੱਚ_w[IDN_MAX_ਲੰਮਾਈ];
  ਪੂਰਨ ਵਿੱਚ_w_len;
  *ਬਾਹਰਲਾ = ਨਿਰਾ;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  ਵਿੱਚ_w_len = Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 ਵਿੱਚ, -1, ਵਿੱਚ_w, IDN_MAX_ਲੰਮਾਈ);
  ਜੇ(ਵਿੱਚ_w_len) {
    WCHAR idn[IDN_MAX_ਲੰਮਾਈ]; /* stores a UTF-16 string */
    ਪੂਰਨ chars = Idn_ਮੰਜ਼ਿਲ_Unicode(0, ਵਿੱਚ_w, ਵਿੱਚ_w_len, idn, IDN_MAX_ਲੰਮਾਈ);
    ਜੇ(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *ਬਾਹਰਲਾ = idn_curlx_convert_wchar_ਮੰਜ਼ਿਲ_UTF8(idn, chars);
      ਜੇ(!*ਬਾਹਰਲਾ)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ CURLE_URL_MALFORMAT;
  }
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ CURLE_URL_MALFORMAT;

  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਅੰਤ /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(ਅਚਲ ਅੱਖਰ *hostname)
{
  ਜੇ(hostname) {
    ਬਣਤਰ Curl_str s;
    s.str = hostname;
    s.len = ਲੜੀ_ਲੰਬਾਈ(hostname);
    ਵਾਪਸ Curl_is_ASCII_str(&s);
  }
  ਵਾਪਸ TRUE;
}

bool Curl_is_ASCII_str(ਬਣਤਰ Curl_str *s)
{
  ਜੇ(s && s->len) {
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ch = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)s->str;
    size_t i;
    ਲਈ(i = 0; i < s->len; ++i) {
      ਜੇ(ch[i] & 0x80)
        ਵਾਪਸ FALSE;
    }
  }
  ਵਾਪਸ TRUE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IDN
/*
 * Curl_idn_decode() returns an allocated IDN decoded string if it was
 * possible. NULL on error.
 *
 * CURLE_URL_MALFORMAT - the hostname could not be converted
 * CURLE_OUT_OF_MEMORY - memory problem
 *
 */
ਸਥਿਰ CURLcode idn_decode(ਅਚਲ ਅੱਖਰ *ਨਿਵੇਸ਼, ਅੱਖਰ **output)
{
  ਅੱਖਰ *decoded = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBIDN2
  ਜੇ(idn2_check_ਸੰਸਕਰਣ(IDN2_ਸੰਸਕਰਣ)) {
    ਪੂਰਨ flags = IDN2_NFC_ਨਿਵੇਸ਼
#ਜੇ IDN2_ਸੰਸਕਰਣ_ਸੰਖਿਆ >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#ਜੇ_ਅੰਤ
      ;
    ਪੂਰਨ rc = IDN2_LOOKUP(ਨਿਵੇਸ਼, &decoded, flags);
    ਜੇ(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(ਨਿਵੇਸ਼, &decoded, IDN2_TRANSITIONAL);
    ਜੇ(rc != IDN2_OK)
      ਨਤੀਜਾ = CURLE_URL_MALFORMAT;
  }
  ਨਹੀਂਤਾਂ
    /* a too old libidn2 version */
    ਨਤੀਜਾ = CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜਾਂਜੇ defined(USE_WIN32_IDN)
  ਨਤੀਜਾ = win32_idn_ਮੰਜ਼ਿਲ_ascii(ਨਿਵੇਸ਼, &decoded);
#ਜਾਂਜੇ defined(USE_APPLE_IDN)
  ਨਤੀਜਾ = mac_idn_ਮੰਜ਼ਿਲ_ascii(ਨਿਵੇਸ਼, &decoded);
#ਜੇ_ਅੰਤ
  ਜੇ(!ਨਤੀਜਾ)
    *output = decoded;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode idn_encode(ਅਚਲ ਅੱਖਰ *puny, ਅੱਖਰ **output)
{
  ਅੱਖਰ *enc = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBIDN2
  ਪੂਰਨ rc = idn2_ਮੰਜ਼ਿਲ_unicode_8z8z(puny, &enc, 0);
  ਜੇ(rc != IDNA_SUCCESS)
    ਵਾਪਸ rc == IDNA_MALLOC_ERROR ? CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ : CURLE_URL_MALFORMAT;
#ਜਾਂਜੇ defined(USE_WIN32_IDN)
  CURLcode ਨਤੀਜਾ = win32_ascii_ਮੰਜ਼ਿਲ_idn(puny, &enc);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
#ਜਾਂਜੇ defined(USE_APPLE_IDN)
  CURLcode ਨਤੀਜਾ = mac_ascii_ਮੰਜ਼ਿਲ_idn(puny, &enc);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
#ਜੇ_ਅੰਤ
  *output = enc;
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_idn_decode(ਅਚਲ ਅੱਖਰ *ਨਿਵੇਸ਼, ਅੱਖਰ **output)
{
  ਅੱਖਰ *d = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = idn_decode(ਨਿਵੇਸ਼, &d);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBIDN2
  ਜੇ(!ਨਤੀਜਾ) {
    ਅੱਖਰ *c = curlx_strdup(d);
    idn2_free(d);
    ਜੇ(c)
      d = c;
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
#ਜੇ_ਅੰਤ
  ਜੇ(!ਨਤੀਜਾ) {
    ਜੇ(!d[0]) { /* ended up zero length, not acceptable */
      ਨਤੀਜਾ = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    ਨਹੀਂਤਾਂ
      *output = d;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_idn_encode(ਅਚਲ ਅੱਖਰ *puny, ਅੱਖਰ **output)
{
  ਅੱਖਰ *d = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = idn_encode(puny, &d);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBIDN2
  ਜੇ(!ਨਤੀਜਾ) {
    ਅੱਖਰ *c = curlx_strdup(d);
    idn2_free(d);
    ਜੇ(c)
      d = c;
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
#ਜੇ_ਅੰਤ
  ਜੇ(!ਨਤੀਜਾ)
    *output = d;
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
ਖਾਲੀ Curl_free_idnconverted_hostname(ਬਣਤਰ hostname *host)
{
  curlx_safefree(host->encalloc);
}

#ਜੇ_ਅੰਤ /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(ਬਣਤਰ hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  ਜੇ(!Curl_is_ASCII_name(host->name)) {
    ਅੱਖਰ *decoded;
    CURLcode ਨਤੀਜਾ = Curl_idn_decode(host->name, &decoded);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ CURLE_OK;
}
