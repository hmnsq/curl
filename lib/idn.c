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

#اگر_تعریف_شده USE_LIBIDN2
#ضمن <idn2.h>

#إذا defined(_WIN32) && defined(UNICODE)
#تعریف_کن IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((ثابت uint8_t *)name, (uint8_t **)host, flags)
#وإلا
#تعریف_کن IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((ثابت حرف *)(name), (حرف **)(host), flags)
#پایان_اگر
#پایان_اگر /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#اگر_تعریف_شده USE_APPLE_IDN
#ضمن <unicode/uidna.h>
#ضمن <iconv.h>
#ضمن <langinfo.h>

#تعریف_کن MAX_HOST_درازی 512

ساكن CURLcode iconv_مقصد_utf8(ثابت حرف *درون, size_t inlen,
                              حرف **بیرون, size_t *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  إذا(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    حرف *iconv_درون = (حرف *)CURL_UNCONST(درون);
    size_t iconv_inlen = inlen;
    size_t iconv_نتیجه = iconv(cd, &iconv_درون, &iconv_inlen,
                                بیرون, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    إذا(iconv_نتیجه == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      إذا(errno == ENOMEM)
        أرجع CURLE_بیرون_از_حافظه;
      وإلا
        أرجع CURLE_URL_MALFORMAT;
    }

    أرجع CURLE_OK;
  }
  وإلا {
    /* !checksrc! disable ERRNOVAR 1 */
    إذا(errno == ENOMEM)
      أرجع CURLE_بیرون_از_حافظه;
    وإلا
      أرجع CURLE_FAILED_INIT;
  }
}

ساكن CURLcode mac_idn_مقصد_ascii(ثابت حرف *درون, حرف **بیرون)
{
  size_t inlen = طول_رشته(درون);
  إذا(inlen < MAX_HOST_درازی) {
    حرف iconv_میانگیر[MAX_HOST_درازی] = { 0 };
    حرف *iconv_outptr = iconv_میانگیر;
    size_t iconv_outlen = حجم(iconv_میانگیر);
    CURLcode iconv_نتیجه = iconv_مقصد_utf8(درون, inlen,
                                          &iconv_outptr, &iconv_outlen);
    إذا(!iconv_نتیجه) {
      UError_کد err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_مقصد_ASCII, &err);
      إذا(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        حرف میانگیر[MAX_HOST_درازی] = { 0 };
        (فراغ)uidna_name_مقصد_ASCII_UTF8(idna, iconv_میانگیر, (صحيح)iconv_outlen,
                                     میانگیر, حجم(میانگیر) - 1, &info, &err);
        uidna_close(idna);
        إذا(!U_FAILURE(err) && !info.errors) {
          *بیرون = curlx_strdup(میانگیر);
          إذا(*بیرون)
            أرجع CURLE_OK;
          وإلا
            أرجع CURLE_بیرون_از_حافظه;
        }
      }
    }
    وإلا
      أرجع iconv_نتیجه;
  }
  أرجع CURLE_URL_MALFORMAT;
}

ساكن CURLcode mac_ascii_مقصد_idn(ثابت حرف *درون, حرف **بیرون)
{
  size_t inlen = طول_رشته(درون);
  إذا(inlen < MAX_HOST_درازی) {
    UError_کد err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_مقصد_UNICODE, &err);
    إذا(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      حرف میانگیر[MAX_HOST_درازی] = { 0 };
      (فراغ)uidna_name_مقصد_Unicode_UTF8(idna, درون, -1, میانگیر,
                                    حجم(میانگیر) - 1, &info, &err);
      uidna_close(idna);
      إذا(!U_FAILURE(err)) {
        *بیرون = curlx_strdup(میانگیر);
        إذا(*بیرون)
          أرجع CURLE_OK;
        وإلا
          أرجع CURLE_بیرون_از_حافظه;
      }
    }
  }
  أرجع CURLE_URL_MALFORMAT;
}
#پایان_اگر

#اگر_تعریف_شده USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#تعریف_کن IDN_MAX_درازی 255

ساكن حرف *idn_curlx_convert_wchar_مقصد_UTF8(ثابت wchar_t *str_w, صحيح chars)
{
  حرف *str_utf8 = عدم;
  صحيح بایتها = Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0, str_w, chars, عدم, 0,
                                  عدم, عدم);
  إذا(بایتها > 0) {
    str_utf8 = curlx_malloc(بایتها);
    إذا(str_utf8) {
      إذا(Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0, str_w, chars, str_utf8, بایتها,
                             عدم, عدم) == 0) {
        curlx_free(str_utf8);
        أرجع عدم;
      }
    }
  }
  أرجع str_utf8;
}

ساكن CURLcode win32_idn_مقصد_ascii(ثابت حرف *درون, حرف **بیرون)
{
  wchar_t درون_w[IDN_MAX_درازی];
  صحيح درون_w_len;
  *بیرون = عدم;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  درون_w_len = Multi_Byte_مقصد_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 درون, -1, درون_w, IDN_MAX_درازی);
  إذا(درون_w_len) {
    wchar_t punycode[IDN_MAX_درازی];
    صحيح chars = Idn_مقصد_Ascii(0, درون_w, درون_w_len, punycode, IDN_MAX_درازی);
    إذا(chars > 0) {
      *بیرون = idn_curlx_convert_wchar_مقصد_UTF8(punycode, chars);
      إذا(!*بیرون)
        أرجع CURLE_بیرون_از_حافظه;
    }
    وإلا
      أرجع CURLE_URL_MALFORMAT;
  }
  وإلا
    أرجع CURLE_URL_MALFORMAT;

  أرجع CURLE_OK;
}

ساكن CURLcode win32_ascii_مقصد_idn(ثابت حرف *درون, حرف **بیرون)
{
  wchar_t درون_w[IDN_MAX_درازی];
  صحيح درون_w_len;
  *بیرون = عدم;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  درون_w_len = Multi_Byte_مقصد_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 درون, -1, درون_w, IDN_MAX_درازی);
  إذا(درون_w_len) {
    WCHAR idn[IDN_MAX_درازی]; /* stores a UTF-16 string */
    صحيح chars = Idn_مقصد_Unicode(0, درون_w, درون_w_len, idn, IDN_MAX_درازی);
    إذا(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *بیرون = idn_curlx_convert_wchar_مقصد_UTF8(idn, chars);
      إذا(!*بیرون)
        أرجع CURLE_بیرون_از_حافظه;
    }
    وإلا
      أرجع CURLE_URL_MALFORMAT;
  }
  وإلا
    أرجع CURLE_URL_MALFORMAT;

  أرجع CURLE_OK;
}

#پایان_اگر /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(ثابت حرف *hostname)
{
  إذا(hostname) {
    بنية Curl_str s;
    s.str = hostname;
    s.len = طول_رشته(hostname);
    أرجع Curl_is_ASCII_str(&s);
  }
  أرجع TRUE;
}

bool Curl_is_ASCII_str(بنية Curl_str *s)
{
  إذا(s && s->len) {
    ثابت بیعلامت حرف *ch = (ثابت بیعلامت حرف *)s->str;
    size_t i;
    لكل(i = 0; i < s->len; ++i) {
      إذا(ch[i] & 0x80)
        أرجع FALSE;
    }
  }
  أرجع TRUE;
}

#اگر_تعریف_شده USE_IDN
/*
 * Curl_idn_decode() returns an allocated IDN decoded string if it was
 * possible. NULL on error.
 *
 * CURLE_URL_MALFORMAT - the hostname could not be converted
 * CURLE_OUT_OF_MEMORY - memory problem
 *
 */
ساكن CURLcode idn_decode(ثابت حرف *درون_داد, حرف **output)
{
  حرف *decoded = عدم;
  CURLcode نتیجه = CURLE_OK;
#اگر_تعریف_شده USE_LIBIDN2
  إذا(idn2_check_نسخه(IDN2_نسخه)) {
    صحيح flags = IDN2_NFC_درون_داد
#إذا IDN2_نسخه_عدد >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#پایان_اگر
      ;
    صحيح rc = IDN2_LOOKUP(درون_داد, &decoded, flags);
    إذا(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(درون_داد, &decoded, IDN2_TRANSITIONAL);
    إذا(rc != IDN2_OK)
      نتیجه = CURLE_URL_MALFORMAT;
  }
  وإلا
    /* a too old libidn2 version */
    نتیجه = CURLE_NOT_BUILT_درون;
#وإلاإذا defined(USE_WIN32_IDN)
  نتیجه = win32_idn_مقصد_ascii(درون_داد, &decoded);
#وإلاإذا defined(USE_APPLE_IDN)
  نتیجه = mac_idn_مقصد_ascii(درون_داد, &decoded);
#پایان_اگر
  إذا(!نتیجه)
    *output = decoded;
  أرجع نتیجه;
}

ساكن CURLcode idn_encode(ثابت حرف *puny, حرف **output)
{
  حرف *enc = عدم;
#اگر_تعریف_شده USE_LIBIDN2
  صحيح rc = idn2_مقصد_unicode_8z8z(puny, &enc, 0);
  إذا(rc != IDNA_SUCCESS)
    أرجع rc == IDNA_MALLOC_ERROR ? CURLE_بیرون_از_حافظه : CURLE_URL_MALFORMAT;
#وإلاإذا defined(USE_WIN32_IDN)
  CURLcode نتیجه = win32_ascii_مقصد_idn(puny, &enc);
  إذا(نتیجه)
    أرجع نتیجه;
#وإلاإذا defined(USE_APPLE_IDN)
  CURLcode نتیجه = mac_ascii_مقصد_idn(puny, &enc);
  إذا(نتیجه)
    أرجع نتیجه;
#پایان_اگر
  *output = enc;
  أرجع CURLE_OK;
}

CURLcode Curl_idn_decode(ثابت حرف *درون_داد, حرف **output)
{
  حرف *d = عدم;
  CURLcode نتیجه = idn_decode(درون_داد, &d);
#اگر_تعریف_شده USE_LIBIDN2
  إذا(!نتیجه) {
    حرف *c = curlx_strdup(d);
    idn2_free(d);
    إذا(c)
      d = c;
    وإلا
      نتیجه = CURLE_بیرون_از_حافظه;
  }
#پایان_اگر
  إذا(!نتیجه) {
    إذا(!d[0]) { /* ended up zero length, not acceptable */
      نتیجه = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    وإلا
      *output = d;
  }
  أرجع نتیجه;
}

CURLcode Curl_idn_encode(ثابت حرف *puny, حرف **output)
{
  حرف *d = عدم;
  CURLcode نتیجه = idn_encode(puny, &d);
#اگر_تعریف_شده USE_LIBIDN2
  إذا(!نتیجه) {
    حرف *c = curlx_strdup(d);
    idn2_free(d);
    إذا(c)
      d = c;
    وإلا
      نتیجه = CURLE_بیرون_از_حافظه;
  }
#پایان_اگر
  إذا(!نتیجه)
    *output = d;
  أرجع نتیجه;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
فراغ Curl_free_idnconverted_hostname(بنية hostname *host)
{
  curlx_safefree(host->encalloc);
}

#پایان_اگر /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(بنية hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#اگر_تعریف_شده USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  إذا(!Curl_is_ASCII_name(host->name)) {
    حرف *decoded;
    CURLcode نتیجه = Curl_idn_decode(host->name, &decoded);
    إذا(نتیجه)
      أرجع نتیجه;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#پایان_اگر
  أرجع CURLE_OK;
}
