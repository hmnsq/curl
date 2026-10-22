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
#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "curlx/strparse.h"
#شامل "idn.h"

#اگر_تعریف_شدہ USE_LIBIDN2
#شامل <idn2.h>

#اگر defined(_WIN32) && defined(UNICODE)
#تعریف_کرو IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((ثابت uint8_t *)name, (uint8_t **)host, flags)
#ورنہ
#تعریف_کرو IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((ثابت حرف *)(name), (حرف **)(host), flags)
#اگر_ختم
#اگر_ختم /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#اگر_تعریف_شدہ USE_APPLE_IDN
#شامل <unicode/uidna.h>
#شامل <iconv.h>
#شامل <langinfo.h>

#تعریف_کرو MAX_HOST_لمبائی 512

ساکن CURLcode iconv_مقصد_utf8(ثابت حرف *اندر, size_t inlen,
                              حرف **برآمد, size_t *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  اگر(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    حرف *iconv_اندر = (حرف *)CURL_UNCONST(اندر);
    size_t iconv_inlen = inlen;
    size_t iconv_نتیجہ = iconv(cd, &iconv_اندر, &iconv_inlen,
                                برآمد, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    اگر(iconv_نتیجہ == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      اگر(errno == ENOMEM)
        واپس CURLE_برآمد_کا_حافظہ;
      ورنہ
        واپس CURLE_URL_MALFORMAT;
    }

    واپس CURLE_OK;
  }
  ورنہ {
    /* !checksrc! disable ERRNOVAR 1 */
    اگر(errno == ENOMEM)
      واپس CURLE_برآمد_کا_حافظہ;
    ورنہ
      واپس CURLE_FAILED_INIT;
  }
}

ساکن CURLcode mac_idn_مقصد_ascii(ثابت حرف *اندر, حرف **برآمد)
{
  size_t inlen = لڑی_لمبائی(اندر);
  اگر(inlen < MAX_HOST_لمبائی) {
    حرف iconv_بفر[MAX_HOST_لمبائی] = { 0 };
    حرف *iconv_outptr = iconv_بفر;
    size_t iconv_outlen = حجم(iconv_بفر);
    CURLcode iconv_نتیجہ = iconv_مقصد_utf8(اندر, inlen,
                                          &iconv_outptr, &iconv_outlen);
    اگر(!iconv_نتیجہ) {
      UError_رمز err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_مقصد_ASCII, &err);
      اگر(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        حرف بفر[MAX_HOST_لمبائی] = { 0 };
        (خالی)uidna_name_مقصد_ASCII_UTF8(idna, iconv_بفر, (صحیح)iconv_outlen,
                                     بفر, حجم(بفر) - 1, &info, &err);
        uidna_close(idna);
        اگر(!U_FAILURE(err) && !info.errors) {
          *برآمد = curlx_strdup(بفر);
          اگر(*برآمد)
            واپس CURLE_OK;
          ورنہ
            واپس CURLE_برآمد_کا_حافظہ;
        }
      }
    }
    ورنہ
      واپس iconv_نتیجہ;
  }
  واپس CURLE_URL_MALFORMAT;
}

ساکن CURLcode mac_ascii_مقصد_idn(ثابت حرف *اندر, حرف **برآمد)
{
  size_t inlen = لڑی_لمبائی(اندر);
  اگر(inlen < MAX_HOST_لمبائی) {
    UError_رمز err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_مقصد_UNICODE, &err);
    اگر(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      حرف بفر[MAX_HOST_لمبائی] = { 0 };
      (خالی)uidna_name_مقصد_Unicode_UTF8(idna, اندر, -1, بفر,
                                    حجم(بفر) - 1, &info, &err);
      uidna_close(idna);
      اگر(!U_FAILURE(err)) {
        *برآمد = curlx_strdup(بفر);
        اگر(*برآمد)
          واپس CURLE_OK;
        ورنہ
          واپس CURLE_برآمد_کا_حافظہ;
      }
    }
  }
  واپس CURLE_URL_MALFORMAT;
}
#اگر_ختم

#اگر_تعریف_شدہ USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#تعریف_کرو IDN_MAX_لمبائی 255

ساکن حرف *idn_curlx_convert_wchar_مقصد_UTF8(ثابت wchar_t *str_w, صحیح chars)
{
  حرف *str_utf8 = تہی;
  صحیح بائٹیں = Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0, str_w, chars, تہی, 0,
                                  تہی, تہی);
  اگر(بائٹیں > 0) {
    str_utf8 = curlx_malloc(بائٹیں);
    اگر(str_utf8) {
      اگر(Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0, str_w, chars, str_utf8, بائٹیں,
                             تہی, تہی) == 0) {
        curlx_free(str_utf8);
        واپس تہی;
      }
    }
  }
  واپس str_utf8;
}

ساکن CURLcode win32_idn_مقصد_ascii(ثابت حرف *اندر, حرف **برآمد)
{
  wchar_t اندر_w[IDN_MAX_لمبائی];
  صحیح اندر_w_len;
  *برآمد = تہی;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  اندر_w_len = Multi_Byte_مقصد_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 اندر, -1, اندر_w, IDN_MAX_لمبائی);
  اگر(اندر_w_len) {
    wchar_t punycode[IDN_MAX_لمبائی];
    صحیح chars = Idn_مقصد_Ascii(0, اندر_w, اندر_w_len, punycode, IDN_MAX_لمبائی);
    اگر(chars > 0) {
      *برآمد = idn_curlx_convert_wchar_مقصد_UTF8(punycode, chars);
      اگر(!*برآمد)
        واپس CURLE_برآمد_کا_حافظہ;
    }
    ورنہ
      واپس CURLE_URL_MALFORMAT;
  }
  ورنہ
    واپس CURLE_URL_MALFORMAT;

  واپس CURLE_OK;
}

ساکن CURLcode win32_ascii_مقصد_idn(ثابت حرف *اندر, حرف **برآمد)
{
  wchar_t اندر_w[IDN_MAX_لمبائی];
  صحیح اندر_w_len;
  *برآمد = تہی;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  اندر_w_len = Multi_Byte_مقصد_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 اندر, -1, اندر_w, IDN_MAX_لمبائی);
  اگر(اندر_w_len) {
    WCHAR idn[IDN_MAX_لمبائی]; /* stores a UTF-16 string */
    صحیح chars = Idn_مقصد_Unicode(0, اندر_w, اندر_w_len, idn, IDN_MAX_لمبائی);
    اگر(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *برآمد = idn_curlx_convert_wchar_مقصد_UTF8(idn, chars);
      اگر(!*برآمد)
        واپس CURLE_برآمد_کا_حافظہ;
    }
    ورنہ
      واپس CURLE_URL_MALFORMAT;
  }
  ورنہ
    واپس CURLE_URL_MALFORMAT;

  واپس CURLE_OK;
}

#اگر_ختم /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(ثابت حرف *hostname)
{
  اگر(hostname) {
    ساخت Curl_str s;
    s.str = hostname;
    s.len = لڑی_لمبائی(hostname);
    واپس Curl_is_ASCII_str(&s);
  }
  واپس TRUE;
}

bool Curl_is_ASCII_str(ساخت Curl_str *s)
{
  اگر(s && s->len) {
    ثابت غیرعلامتی حرف *ch = (ثابت غیرعلامتی حرف *)s->str;
    size_t i;
    برائے(i = 0; i < s->len; ++i) {
      اگر(ch[i] & 0x80)
        واپس FALSE;
    }
  }
  واپس TRUE;
}

#اگر_تعریف_شدہ USE_IDN
/*
 * Curl_idn_decode() returns an allocated IDN decoded string if it was
 * possible. NULL on error.
 *
 * CURLE_URL_MALFORMAT - the hostname could not be converted
 * CURLE_OUT_OF_MEMORY - memory problem
 *
 */
ساکن CURLcode idn_decode(ثابت حرف *داخلہ, حرف **output)
{
  حرف *decoded = تہی;
  CURLcode نتیجہ = CURLE_OK;
#اگر_تعریف_شدہ USE_LIBIDN2
  اگر(idn2_check_نسخہ(IDN2_نسخہ)) {
    صحیح flags = IDN2_NFC_داخلہ
#اگر IDN2_نسخہ_نمبر >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#اگر_ختم
      ;
    صحیح rc = IDN2_LOOKUP(داخلہ, &decoded, flags);
    اگر(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(داخلہ, &decoded, IDN2_TRANSITIONAL);
    اگر(rc != IDN2_OK)
      نتیجہ = CURLE_URL_MALFORMAT;
  }
  ورنہ
    /* a too old libidn2 version */
    نتیجہ = CURLE_NOT_BUILT_اندر;
#وگرنہ_اگر defined(USE_WIN32_IDN)
  نتیجہ = win32_idn_مقصد_ascii(داخلہ, &decoded);
#وگرنہ_اگر defined(USE_APPLE_IDN)
  نتیجہ = mac_idn_مقصد_ascii(داخلہ, &decoded);
#اگر_ختم
  اگر(!نتیجہ)
    *output = decoded;
  واپس نتیجہ;
}

ساکن CURLcode idn_encode(ثابت حرف *puny, حرف **output)
{
  حرف *enc = تہی;
#اگر_تعریف_شدہ USE_LIBIDN2
  صحیح rc = idn2_مقصد_unicode_8z8z(puny, &enc, 0);
  اگر(rc != IDNA_SUCCESS)
    واپس rc == IDNA_MALLOC_ERROR ? CURLE_برآمد_کا_حافظہ : CURLE_URL_MALFORMAT;
#وگرنہ_اگر defined(USE_WIN32_IDN)
  CURLcode نتیجہ = win32_ascii_مقصد_idn(puny, &enc);
  اگر(نتیجہ)
    واپس نتیجہ;
#وگرنہ_اگر defined(USE_APPLE_IDN)
  CURLcode نتیجہ = mac_ascii_مقصد_idn(puny, &enc);
  اگر(نتیجہ)
    واپس نتیجہ;
#اگر_ختم
  *output = enc;
  واپس CURLE_OK;
}

CURLcode Curl_idn_decode(ثابت حرف *داخلہ, حرف **output)
{
  حرف *d = تہی;
  CURLcode نتیجہ = idn_decode(داخلہ, &d);
#اگر_تعریف_شدہ USE_LIBIDN2
  اگر(!نتیجہ) {
    حرف *c = curlx_strdup(d);
    idn2_free(d);
    اگر(c)
      d = c;
    ورنہ
      نتیجہ = CURLE_برآمد_کا_حافظہ;
  }
#اگر_ختم
  اگر(!نتیجہ) {
    اگر(!d[0]) { /* ended up zero length, not acceptable */
      نتیجہ = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    ورنہ
      *output = d;
  }
  واپس نتیجہ;
}

CURLcode Curl_idn_encode(ثابت حرف *puny, حرف **output)
{
  حرف *d = تہی;
  CURLcode نتیجہ = idn_encode(puny, &d);
#اگر_تعریف_شدہ USE_LIBIDN2
  اگر(!نتیجہ) {
    حرف *c = curlx_strdup(d);
    idn2_free(d);
    اگر(c)
      d = c;
    ورنہ
      نتیجہ = CURLE_برآمد_کا_حافظہ;
  }
#اگر_ختم
  اگر(!نتیجہ)
    *output = d;
  واپس نتیجہ;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
خالی Curl_free_idnconverted_hostname(ساخت hostname *host)
{
  curlx_safefree(host->encalloc);
}

#اگر_ختم /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(ساخت hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#اگر_تعریف_شدہ USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  اگر(!Curl_is_ASCII_name(host->name)) {
    حرف *decoded;
    CURLcode نتیجہ = Curl_idn_decode(host->name, &decoded);
    اگر(نتیجہ)
      واپس نتیجہ;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#اگر_ختم
  واپس CURLE_OK;
}
