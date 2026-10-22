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
#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "curlx/strparse.h"
#ضمن "idn.h"

#جے_تعریف_شدہ USE_LIBIDN2
#ضمن <idn2.h>

#إذا defined(_WIN32) && defined(UNICODE)
#تعریف_کرو IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((ثابت uint8_t *)name, (uint8_t **)host, flags)
#وإلا
#تعریف_کرو IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((ثابت حرف *)(name), (حرف **)(host), flags)
#جے_انت
#جے_انت /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#جے_تعریف_شدہ USE_APPLE_IDN
#ضمن <unicode/uidna.h>
#ضمن <iconv.h>
#ضمن <langinfo.h>

#تعریف_کرو MAX_HOST_لمائی 512

ساكن CURLcode iconv_منزل_utf8(ثابت حرف *وچ, size_t inlen,
                              حرف **باہرلا, size_t *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  إذا(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    حرف *iconv_وچ = (حرف *)CURL_UNCONST(وچ);
    size_t iconv_inlen = inlen;
    size_t iconv_نتیجہ = iconv(cd, &iconv_وچ, &iconv_inlen,
                                باہرلا, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    إذا(iconv_نتیجہ == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      إذا(errno == ENOMEM)
        أرجع CURLE_باہرلا_دا_حافظہ;
      وإلا
        أرجع CURLE_URL_MALFORMAT;
    }

    أرجع CURLE_OK;
  }
  وإلا {
    /* !checksrc! disable ERRNOVAR 1 */
    إذا(errno == ENOMEM)
      أرجع CURLE_باہرلا_دا_حافظہ;
    وإلا
      أرجع CURLE_FAILED_INIT;
  }
}

ساكن CURLcode mac_idn_منزل_ascii(ثابت حرف *وچ, حرف **باہرلا)
{
  size_t inlen = لڑی_لمبائی(وچ);
  إذا(inlen < MAX_HOST_لمائی) {
    حرف iconv_بفر[MAX_HOST_لمائی] = { 0 };
    حرف *iconv_outptr = iconv_بفر;
    size_t iconv_outlen = حجم(iconv_بفر);
    CURLcode iconv_نتیجہ = iconv_منزل_utf8(وچ, inlen,
                                          &iconv_outptr, &iconv_outlen);
    إذا(!iconv_نتیجہ) {
      UError_کوڈ err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_منزل_ASCII, &err);
      إذا(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        حرف بفر[MAX_HOST_لمائی] = { 0 };
        (فراغ)uidna_name_منزل_ASCII_UTF8(idna, iconv_بفر, (صحيح)iconv_outlen,
                                     بفر, حجم(بفر) - 1, &info, &err);
        uidna_close(idna);
        إذا(!U_FAILURE(err) && !info.errors) {
          *باہرلا = curlx_strdup(بفر);
          إذا(*باہرلا)
            أرجع CURLE_OK;
          وإلا
            أرجع CURLE_باہرلا_دا_حافظہ;
        }
      }
    }
    وإلا
      أرجع iconv_نتیجہ;
  }
  أرجع CURLE_URL_MALFORMAT;
}

ساكن CURLcode mac_ascii_منزل_idn(ثابت حرف *وچ, حرف **باہرلا)
{
  size_t inlen = لڑی_لمبائی(وچ);
  إذا(inlen < MAX_HOST_لمائی) {
    UError_کوڈ err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_منزل_UNICODE, &err);
    إذا(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      حرف بفر[MAX_HOST_لمائی] = { 0 };
      (فراغ)uidna_name_منزل_Unicode_UTF8(idna, وچ, -1, بفر,
                                    حجم(بفر) - 1, &info, &err);
      uidna_close(idna);
      إذا(!U_FAILURE(err)) {
        *باہرلا = curlx_strdup(بفر);
        إذا(*باہرلا)
          أرجع CURLE_OK;
        وإلا
          أرجع CURLE_باہرلا_دا_حافظہ;
      }
    }
  }
  أرجع CURLE_URL_MALFORMAT;
}
#جے_انت

#جے_تعریف_شدہ USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#تعریف_کرو IDN_MAX_لمائی 255

ساكن حرف *idn_curlx_convert_wchar_منزل_UTF8(ثابت wchar_t *str_w, صحيح chars)
{
  حرف *str_utf8 = عدم;
  صحيح بائٹاں = Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, str_w, chars, عدم, 0,
                                  عدم, عدم);
  إذا(بائٹاں > 0) {
    str_utf8 = curlx_malloc(بائٹاں);
    إذا(str_utf8) {
      إذا(Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, str_w, chars, str_utf8, بائٹاں,
                             عدم, عدم) == 0) {
        curlx_free(str_utf8);
        أرجع عدم;
      }
    }
  }
  أرجع str_utf8;
}

ساكن CURLcode win32_idn_منزل_ascii(ثابت حرف *وچ, حرف **باہرلا)
{
  wchar_t وچ_w[IDN_MAX_لمائی];
  صحيح وچ_w_len;
  *باہرلا = عدم;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  وچ_w_len = Multi_Byte_منزل_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 وچ, -1, وچ_w, IDN_MAX_لمائی);
  إذا(وچ_w_len) {
    wchar_t punycode[IDN_MAX_لمائی];
    صحيح chars = Idn_منزل_Ascii(0, وچ_w, وچ_w_len, punycode, IDN_MAX_لمائی);
    إذا(chars > 0) {
      *باہرلا = idn_curlx_convert_wchar_منزل_UTF8(punycode, chars);
      إذا(!*باہرلا)
        أرجع CURLE_باہرلا_دا_حافظہ;
    }
    وإلا
      أرجع CURLE_URL_MALFORMAT;
  }
  وإلا
    أرجع CURLE_URL_MALFORMAT;

  أرجع CURLE_OK;
}

ساكن CURLcode win32_ascii_منزل_idn(ثابت حرف *وچ, حرف **باہرلا)
{
  wchar_t وچ_w[IDN_MAX_لمائی];
  صحيح وچ_w_len;
  *باہرلا = عدم;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  وچ_w_len = Multi_Byte_منزل_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 وچ, -1, وچ_w, IDN_MAX_لمائی);
  إذا(وچ_w_len) {
    WCHAR idn[IDN_MAX_لمائی]; /* stores a UTF-16 string */
    صحيح chars = Idn_منزل_Unicode(0, وچ_w, وچ_w_len, idn, IDN_MAX_لمائی);
    إذا(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *باہرلا = idn_curlx_convert_wchar_منزل_UTF8(idn, chars);
      إذا(!*باہرلا)
        أرجع CURLE_باہرلا_دا_حافظہ;
    }
    وإلا
      أرجع CURLE_URL_MALFORMAT;
  }
  وإلا
    أرجع CURLE_URL_MALFORMAT;

  أرجع CURLE_OK;
}

#جے_انت /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(ثابت حرف *hostname)
{
  إذا(hostname) {
    بنية Curl_str s;
    s.str = hostname;
    s.len = لڑی_لمبائی(hostname);
    أرجع Curl_is_ASCII_str(&s);
  }
  أرجع TRUE;
}

bool Curl_is_ASCII_str(بنية Curl_str *s)
{
  إذا(s && s->len) {
    ثابت بنانشانی حرف *ch = (ثابت بنانشانی حرف *)s->str;
    size_t i;
    لكل(i = 0; i < s->len; ++i) {
      إذا(ch[i] & 0x80)
        أرجع FALSE;
    }
  }
  أرجع TRUE;
}

#جے_تعریف_شدہ USE_IDN
/*
 * Curl_idn_decode() returns an allocated IDN decoded string if it was
 * possible. NULL on error.
 *
 * CURLE_URL_MALFORMAT - the hostname could not be converted
 * CURLE_OUT_OF_MEMORY - memory problem
 *
 */
ساكن CURLcode idn_decode(ثابت حرف *داخلہ, حرف **output)
{
  حرف *decoded = عدم;
  CURLcode نتیجہ = CURLE_OK;
#جے_تعریف_شدہ USE_LIBIDN2
  إذا(idn2_check_نسخہ(IDN2_نسخہ)) {
    صحيح flags = IDN2_NFC_داخلہ
#إذا IDN2_نسخہ_نمبر >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#جے_انت
      ;
    صحيح rc = IDN2_LOOKUP(داخلہ, &decoded, flags);
    إذا(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(داخلہ, &decoded, IDN2_TRANSITIONAL);
    إذا(rc != IDN2_OK)
      نتیجہ = CURLE_URL_MALFORMAT;
  }
  وإلا
    /* a too old libidn2 version */
    نتیجہ = CURLE_NOT_BUILT_وچ;
#وإلاإذا defined(USE_WIN32_IDN)
  نتیجہ = win32_idn_منزل_ascii(داخلہ, &decoded);
#وإلاإذا defined(USE_APPLE_IDN)
  نتیجہ = mac_idn_منزل_ascii(داخلہ, &decoded);
#جے_انت
  إذا(!نتیجہ)
    *output = decoded;
  أرجع نتیجہ;
}

ساكن CURLcode idn_encode(ثابت حرف *puny, حرف **output)
{
  حرف *enc = عدم;
#جے_تعریف_شدہ USE_LIBIDN2
  صحيح rc = idn2_منزل_unicode_8z8z(puny, &enc, 0);
  إذا(rc != IDNA_SUCCESS)
    أرجع rc == IDNA_MALLOC_ERROR ? CURLE_باہرلا_دا_حافظہ : CURLE_URL_MALFORMAT;
#وإلاإذا defined(USE_WIN32_IDN)
  CURLcode نتیجہ = win32_ascii_منزل_idn(puny, &enc);
  إذا(نتیجہ)
    أرجع نتیجہ;
#وإلاإذا defined(USE_APPLE_IDN)
  CURLcode نتیجہ = mac_ascii_منزل_idn(puny, &enc);
  إذا(نتیجہ)
    أرجع نتیجہ;
#جے_انت
  *output = enc;
  أرجع CURLE_OK;
}

CURLcode Curl_idn_decode(ثابت حرف *داخلہ, حرف **output)
{
  حرف *d = عدم;
  CURLcode نتیجہ = idn_decode(داخلہ, &d);
#جے_تعریف_شدہ USE_LIBIDN2
  إذا(!نتیجہ) {
    حرف *c = curlx_strdup(d);
    idn2_free(d);
    إذا(c)
      d = c;
    وإلا
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
  }
#جے_انت
  إذا(!نتیجہ) {
    إذا(!d[0]) { /* ended up zero length, not acceptable */
      نتیجہ = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    وإلا
      *output = d;
  }
  أرجع نتیجہ;
}

CURLcode Curl_idn_encode(ثابت حرف *puny, حرف **output)
{
  حرف *d = عدم;
  CURLcode نتیجہ = idn_encode(puny, &d);
#جے_تعریف_شدہ USE_LIBIDN2
  إذا(!نتیجہ) {
    حرف *c = curlx_strdup(d);
    idn2_free(d);
    إذا(c)
      d = c;
    وإلا
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
  }
#جے_انت
  إذا(!نتیجہ)
    *output = d;
  أرجع نتیجہ;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
فراغ Curl_free_idnconverted_hostname(بنية hostname *host)
{
  curlx_safefree(host->encalloc);
}

#جے_انت /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(بنية hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#جے_تعریف_شدہ USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  إذا(!Curl_is_ASCII_name(host->name)) {
    حرف *decoded;
    CURLcode نتیجہ = Curl_idn_decode(host->name, &decoded);
    إذا(نتیجہ)
      أرجع نتیجہ;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#جے_انت
  أرجع CURLE_OK;
}
