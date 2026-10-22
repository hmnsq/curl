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

#جيڪڏهن_وصف_ٿيل USE_LIBIDN2
#ضمن <idn2.h>

#إذا defined(_WIN32) && defined(UNICODE)
#وصف_ڪريو IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((ثابت uint8_t *)name, (uint8_t **)host, flags)
#وإلا
#وصف_ڪريو IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((ثابت حرف *)(name), (حرف **)(host), flags)
#جيڪڏهن_پورو
#جيڪڏهن_پورو /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#جيڪڏهن_وصف_ٿيل USE_APPLE_IDN
#ضمن <unicode/uidna.h>
#ضمن <iconv.h>
#ضمن <langinfo.h>

#وصف_ڪريو MAX_HOST_ڊيگهائي 512

ساكن CURLcode iconv_منزل_utf8(ثابت حرف *اندريون, size_t inlen,
                              حرف **ٻاهريون, size_t *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  إذا(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    حرف *iconv_اندريون = (حرف *)CURL_UNCONST(اندريون);
    size_t iconv_inlen = inlen;
    size_t iconv_نتيجو = iconv(cd, &iconv_اندريون, &iconv_inlen,
                                ٻاهريون, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    إذا(iconv_نتيجو == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      إذا(errno == ENOMEM)
        أرجع CURLE_ٻاهريون_جو_يادگيري;
      وإلا
        أرجع CURLE_URL_MALFORMAT;
    }

    أرجع CURLE_OK;
  }
  وإلا {
    /* !checksrc! disable ERRNOVAR 1 */
    إذا(errno == ENOMEM)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    وإلا
      أرجع CURLE_FAILED_INIT;
  }
}

ساكن CURLcode mac_idn_منزل_ascii(ثابت حرف *اندريون, حرف **ٻاهريون)
{
  size_t inlen = لڙي_ڊيگهه(اندريون);
  إذا(inlen < MAX_HOST_ڊيگهائي) {
    حرف iconv_بفر[MAX_HOST_ڊيگهائي] = { 0 };
    حرف *iconv_outptr = iconv_بفر;
    size_t iconv_outlen = حجم(iconv_بفر);
    CURLcode iconv_نتيجو = iconv_منزل_utf8(اندريون, inlen,
                                          &iconv_outptr, &iconv_outlen);
    إذا(!iconv_نتيجو) {
      UError_ڪوڊ err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_منزل_ASCII, &err);
      إذا(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        حرف بفر[MAX_HOST_ڊيگهائي] = { 0 };
        (فراغ)uidna_name_منزل_ASCII_UTF8(idna, iconv_بفر, (صحيح)iconv_outlen,
                                     بفر, حجم(بفر) - 1, &info, &err);
        uidna_close(idna);
        إذا(!U_FAILURE(err) && !info.errors) {
          *ٻاهريون = curlx_strdup(بفر);
          إذا(*ٻاهريون)
            أرجع CURLE_OK;
          وإلا
            أرجع CURLE_ٻاهريون_جو_يادگيري;
        }
      }
    }
    وإلا
      أرجع iconv_نتيجو;
  }
  أرجع CURLE_URL_MALFORMAT;
}

ساكن CURLcode mac_ascii_منزل_idn(ثابت حرف *اندريون, حرف **ٻاهريون)
{
  size_t inlen = لڙي_ڊيگهه(اندريون);
  إذا(inlen < MAX_HOST_ڊيگهائي) {
    UError_ڪوڊ err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_منزل_UNICODE, &err);
    إذا(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      حرف بفر[MAX_HOST_ڊيگهائي] = { 0 };
      (فراغ)uidna_name_منزل_Unicode_UTF8(idna, اندريون, -1, بفر,
                                    حجم(بفر) - 1, &info, &err);
      uidna_close(idna);
      إذا(!U_FAILURE(err)) {
        *ٻاهريون = curlx_strdup(بفر);
        إذا(*ٻاهريون)
          أرجع CURLE_OK;
        وإلا
          أرجع CURLE_ٻاهريون_جو_يادگيري;
      }
    }
  }
  أرجع CURLE_URL_MALFORMAT;
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#وصف_ڪريو IDN_MAX_ڊيگهائي 255

ساكن حرف *idn_curlx_convert_wchar_منزل_UTF8(ثابت wchar_t *str_w, صحيح chars)
{
  حرف *str_utf8 = عدم;
  صحيح بائيٽون = Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, str_w, chars, عدم, 0,
                                  عدم, عدم);
  إذا(بائيٽون > 0) {
    str_utf8 = curlx_malloc(بائيٽون);
    إذا(str_utf8) {
      إذا(Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, str_w, chars, str_utf8, بائيٽون,
                             عدم, عدم) == 0) {
        curlx_free(str_utf8);
        أرجع عدم;
      }
    }
  }
  أرجع str_utf8;
}

ساكن CURLcode win32_idn_منزل_ascii(ثابت حرف *اندريون, حرف **ٻاهريون)
{
  wchar_t اندريون_w[IDN_MAX_ڊيگهائي];
  صحيح اندريون_w_len;
  *ٻاهريون = عدم;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  اندريون_w_len = Multi_Byte_منزل_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 اندريون, -1, اندريون_w, IDN_MAX_ڊيگهائي);
  إذا(اندريون_w_len) {
    wchar_t punycode[IDN_MAX_ڊيگهائي];
    صحيح chars = Idn_منزل_Ascii(0, اندريون_w, اندريون_w_len, punycode, IDN_MAX_ڊيگهائي);
    إذا(chars > 0) {
      *ٻاهريون = idn_curlx_convert_wchar_منزل_UTF8(punycode, chars);
      إذا(!*ٻاهريون)
        أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
    وإلا
      أرجع CURLE_URL_MALFORMAT;
  }
  وإلا
    أرجع CURLE_URL_MALFORMAT;

  أرجع CURLE_OK;
}

ساكن CURLcode win32_ascii_منزل_idn(ثابت حرف *اندريون, حرف **ٻاهريون)
{
  wchar_t اندريون_w[IDN_MAX_ڊيگهائي];
  صحيح اندريون_w_len;
  *ٻاهريون = عدم;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  اندريون_w_len = Multi_Byte_منزل_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 اندريون, -1, اندريون_w, IDN_MAX_ڊيگهائي);
  إذا(اندريون_w_len) {
    WCHAR idn[IDN_MAX_ڊيگهائي]; /* stores a UTF-16 string */
    صحيح chars = Idn_منزل_Unicode(0, اندريون_w, اندريون_w_len, idn, IDN_MAX_ڊيگهائي);
    إذا(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *ٻاهريون = idn_curlx_convert_wchar_منزل_UTF8(idn, chars);
      إذا(!*ٻاهريون)
        أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
    وإلا
      أرجع CURLE_URL_MALFORMAT;
  }
  وإلا
    أرجع CURLE_URL_MALFORMAT;

  أرجع CURLE_OK;
}

#جيڪڏهن_پورو /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(ثابت حرف *hostname)
{
  إذا(hostname) {
    بنية Curl_str s;
    s.str = hostname;
    s.len = لڙي_ڊيگهه(hostname);
    أرجع Curl_is_ASCII_str(&s);
  }
  أرجع TRUE;
}

bool Curl_is_ASCII_str(بنية Curl_str *s)
{
  إذا(s && s->len) {
    ثابت بنانشان حرف *ch = (ثابت بنانشان حرف *)s->str;
    size_t i;
    لكل(i = 0; i < s->len; ++i) {
      إذا(ch[i] & 0x80)
        أرجع FALSE;
    }
  }
  أرجع TRUE;
}

#جيڪڏهن_وصف_ٿيل USE_IDN
/*
 * Curl_idn_decode() returns an allocated IDN decoded string if it was
 * possible. NULL on error.
 *
 * CURLE_URL_MALFORMAT - the hostname could not be converted
 * CURLE_OUT_OF_MEMORY - memory problem
 *
 */
ساكن CURLcode idn_decode(ثابت حرف *داخلا, حرف **output)
{
  حرف *decoded = عدم;
  CURLcode نتيجو = CURLE_OK;
#جيڪڏهن_وصف_ٿيل USE_LIBIDN2
  إذا(idn2_check_نسخو(IDN2_نسخو)) {
    صحيح flags = IDN2_NFC_داخلا
#إذا IDN2_نسخو_عدد >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#جيڪڏهن_پورو
      ;
    صحيح rc = IDN2_LOOKUP(داخلا, &decoded, flags);
    إذا(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(داخلا, &decoded, IDN2_TRANSITIONAL);
    إذا(rc != IDN2_OK)
      نتيجو = CURLE_URL_MALFORMAT;
  }
  وإلا
    /* a too old libidn2 version */
    نتيجو = CURLE_NOT_BUILT_اندريون;
#وإلاإذا defined(USE_WIN32_IDN)
  نتيجو = win32_idn_منزل_ascii(داخلا, &decoded);
#وإلاإذا defined(USE_APPLE_IDN)
  نتيجو = mac_idn_منزل_ascii(داخلا, &decoded);
#جيڪڏهن_پورو
  إذا(!نتيجو)
    *output = decoded;
  أرجع نتيجو;
}

ساكن CURLcode idn_encode(ثابت حرف *puny, حرف **output)
{
  حرف *enc = عدم;
#جيڪڏهن_وصف_ٿيل USE_LIBIDN2
  صحيح rc = idn2_منزل_unicode_8z8z(puny, &enc, 0);
  إذا(rc != IDNA_SUCCESS)
    أرجع rc == IDNA_MALLOC_ERROR ? CURLE_ٻاهريون_جو_يادگيري : CURLE_URL_MALFORMAT;
#وإلاإذا defined(USE_WIN32_IDN)
  CURLcode نتيجو = win32_ascii_منزل_idn(puny, &enc);
  إذا(نتيجو)
    أرجع نتيجو;
#وإلاإذا defined(USE_APPLE_IDN)
  CURLcode نتيجو = mac_ascii_منزل_idn(puny, &enc);
  إذا(نتيجو)
    أرجع نتيجو;
#جيڪڏهن_پورو
  *output = enc;
  أرجع CURLE_OK;
}

CURLcode Curl_idn_decode(ثابت حرف *داخلا, حرف **output)
{
  حرف *d = عدم;
  CURLcode نتيجو = idn_decode(داخلا, &d);
#جيڪڏهن_وصف_ٿيل USE_LIBIDN2
  إذا(!نتيجو) {
    حرف *c = curlx_strdup(d);
    idn2_free(d);
    إذا(c)
      d = c;
    وإلا
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  }
#جيڪڏهن_پورو
  إذا(!نتيجو) {
    إذا(!d[0]) { /* ended up zero length, not acceptable */
      نتيجو = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    وإلا
      *output = d;
  }
  أرجع نتيجو;
}

CURLcode Curl_idn_encode(ثابت حرف *puny, حرف **output)
{
  حرف *d = عدم;
  CURLcode نتيجو = idn_encode(puny, &d);
#جيڪڏهن_وصف_ٿيل USE_LIBIDN2
  إذا(!نتيجو) {
    حرف *c = curlx_strdup(d);
    idn2_free(d);
    إذا(c)
      d = c;
    وإلا
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  }
#جيڪڏهن_پورو
  إذا(!نتيجو)
    *output = d;
  أرجع نتيجو;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
فراغ Curl_free_idnconverted_hostname(بنية hostname *host)
{
  curlx_safefree(host->encalloc);
}

#جيڪڏهن_پورو /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(بنية hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#جيڪڏهن_وصف_ٿيل USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  إذا(!Curl_is_ASCII_name(host->name)) {
    حرف *decoded;
    CURLcode نتيجو = Curl_idn_decode(host->name, &decoded);
    إذا(نتيجو)
      أرجع نتيجو;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#جيڪڏهن_پورو
  أرجع CURLE_OK;
}
