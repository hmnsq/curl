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

#که_تعریف_شوی USE_LIBIDN2
#ضمن <idn2.h>

#إذا defined(_WIN32) && defined(UNICODE)
#تعریف_کړه IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((ثابت uint8_t *)name, (uint8_t **)host, flags)
#وإلا
#تعریف_کړه IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((ثابت حرف *)(name), (حرف **)(host), flags)
#د_که_پای
#د_که_پای /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#که_تعریف_شوی USE_APPLE_IDN
#ضمن <unicode/uidna.h>
#ضمن <iconv.h>
#ضمن <langinfo.h>

#تعریف_کړه MAX_HOST_اوږدتیا 512

ساكن CURLcode iconv_منزل_utf8(ثابت حرف *دننه, size_t inlen,
                              حرف **بهر, size_t *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  إذا(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    حرف *iconv_دننه = (حرف *)CURL_UNCONST(دننه);
    size_t iconv_inlen = inlen;
    size_t iconv_پایله = iconv(cd, &iconv_دننه, &iconv_inlen,
                                بهر, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    إذا(iconv_پایله == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      إذا(errno == ENOMEM)
        أرجع CURLE_بهر_د_حافظه;
      وإلا
        أرجع CURLE_URL_MALFORMAT;
    }

    أرجع CURLE_OK;
  }
  وإلا {
    /* !checksrc! disable ERRNOVAR 1 */
    إذا(errno == ENOMEM)
      أرجع CURLE_بهر_د_حافظه;
    وإلا
      أرجع CURLE_FAILED_INIT;
  }
}

ساكن CURLcode mac_idn_منزل_ascii(ثابت حرف *دننه, حرف **بهر)
{
  size_t inlen = مزي_اوږدوالی(دننه);
  إذا(inlen < MAX_HOST_اوږدتیا) {
    حرف iconv_بفر[MAX_HOST_اوږدتیا] = { 0 };
    حرف *iconv_outptr = iconv_بفر;
    size_t iconv_outlen = حجم(iconv_بفر);
    CURLcode iconv_پایله = iconv_منزل_utf8(دننه, inlen,
                                          &iconv_outptr, &iconv_outlen);
    إذا(!iconv_پایله) {
      UError_کوډ err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_منزل_ASCII, &err);
      إذا(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        حرف بفر[MAX_HOST_اوږدتیا] = { 0 };
        (فراغ)uidna_name_منزل_ASCII_UTF8(idna, iconv_بفر, (صحيح)iconv_outlen,
                                     بفر, حجم(بفر) - 1, &info, &err);
        uidna_close(idna);
        إذا(!U_FAILURE(err) && !info.errors) {
          *بهر = curlx_strdup(بفر);
          إذا(*بهر)
            أرجع CURLE_OK;
          وإلا
            أرجع CURLE_بهر_د_حافظه;
        }
      }
    }
    وإلا
      أرجع iconv_پایله;
  }
  أرجع CURLE_URL_MALFORMAT;
}

ساكن CURLcode mac_ascii_منزل_idn(ثابت حرف *دننه, حرف **بهر)
{
  size_t inlen = مزي_اوږدوالی(دننه);
  إذا(inlen < MAX_HOST_اوږدتیا) {
    UError_کوډ err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_منزل_UNICODE, &err);
    إذا(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      حرف بفر[MAX_HOST_اوږدتیا] = { 0 };
      (فراغ)uidna_name_منزل_Unicode_UTF8(idna, دننه, -1, بفر,
                                    حجم(بفر) - 1, &info, &err);
      uidna_close(idna);
      إذا(!U_FAILURE(err)) {
        *بهر = curlx_strdup(بفر);
        إذا(*بهر)
          أرجع CURLE_OK;
        وإلا
          أرجع CURLE_بهر_د_حافظه;
      }
    }
  }
  أرجع CURLE_URL_MALFORMAT;
}
#د_که_پای

#که_تعریف_شوی USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#تعریف_کړه IDN_MAX_اوږدتیا 255

ساكن حرف *idn_curlx_convert_wchar_منزل_UTF8(ثابت wchar_t *str_w, صحيح chars)
{
  حرف *str_utf8 = عدم;
  صحيح بایټونه = Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, str_w, chars, عدم, 0,
                                  عدم, عدم);
  إذا(بایټونه > 0) {
    str_utf8 = curlx_malloc(بایټونه);
    إذا(str_utf8) {
      إذا(Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, str_w, chars, str_utf8, بایټونه,
                             عدم, عدم) == 0) {
        curlx_free(str_utf8);
        أرجع عدم;
      }
    }
  }
  أرجع str_utf8;
}

ساكن CURLcode win32_idn_منزل_ascii(ثابت حرف *دننه, حرف **بهر)
{
  wchar_t دننه_w[IDN_MAX_اوږدتیا];
  صحيح دننه_w_len;
  *بهر = عدم;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  دننه_w_len = Multi_Byte_منزل_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 دننه, -1, دننه_w, IDN_MAX_اوږدتیا);
  إذا(دننه_w_len) {
    wchar_t punycode[IDN_MAX_اوږدتیا];
    صحيح chars = Idn_منزل_Ascii(0, دننه_w, دننه_w_len, punycode, IDN_MAX_اوږدتیا);
    إذا(chars > 0) {
      *بهر = idn_curlx_convert_wchar_منزل_UTF8(punycode, chars);
      إذا(!*بهر)
        أرجع CURLE_بهر_د_حافظه;
    }
    وإلا
      أرجع CURLE_URL_MALFORMAT;
  }
  وإلا
    أرجع CURLE_URL_MALFORMAT;

  أرجع CURLE_OK;
}

ساكن CURLcode win32_ascii_منزل_idn(ثابت حرف *دننه, حرف **بهر)
{
  wchar_t دننه_w[IDN_MAX_اوږدتیا];
  صحيح دننه_w_len;
  *بهر = عدم;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  دننه_w_len = Multi_Byte_منزل_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 دننه, -1, دننه_w, IDN_MAX_اوږدتیا);
  إذا(دننه_w_len) {
    WCHAR idn[IDN_MAX_اوږدتیا]; /* stores a UTF-16 string */
    صحيح chars = Idn_منزل_Unicode(0, دننه_w, دننه_w_len, idn, IDN_MAX_اوږدتیا);
    إذا(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *بهر = idn_curlx_convert_wchar_منزل_UTF8(idn, chars);
      إذا(!*بهر)
        أرجع CURLE_بهر_د_حافظه;
    }
    وإلا
      أرجع CURLE_URL_MALFORMAT;
  }
  وإلا
    أرجع CURLE_URL_MALFORMAT;

  أرجع CURLE_OK;
}

#د_که_پای /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(ثابت حرف *hostname)
{
  إذا(hostname) {
    بنية Curl_str s;
    s.str = hostname;
    s.len = مزي_اوږدوالی(hostname);
    أرجع Curl_is_ASCII_str(&s);
  }
  أرجع TRUE;
}

bool Curl_is_ASCII_str(بنية Curl_str *s)
{
  إذا(s && s->len) {
    ثابت بینښه حرف *ch = (ثابت بینښه حرف *)s->str;
    size_t i;
    لكل(i = 0; i < s->len; ++i) {
      إذا(ch[i] & 0x80)
        أرجع FALSE;
    }
  }
  أرجع TRUE;
}

#که_تعریف_شوی USE_IDN
/*
 * Curl_idn_decode() returns an allocated IDN decoded string if it was
 * possible. NULL on error.
 *
 * CURLE_URL_MALFORMAT - the hostname could not be converted
 * CURLE_OUT_OF_MEMORY - memory problem
 *
 */
ساكن CURLcode idn_decode(ثابت حرف *ننوت, حرف **output)
{
  حرف *decoded = عدم;
  CURLcode پایله = CURLE_OK;
#که_تعریف_شوی USE_LIBIDN2
  إذا(idn2_check_نسخه(IDN2_نسخه)) {
    صحيح flags = IDN2_NFC_ننوت
#إذا IDN2_نسخه_شمېره >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#د_که_پای
      ;
    صحيح rc = IDN2_LOOKUP(ننوت, &decoded, flags);
    إذا(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(ننوت, &decoded, IDN2_TRANSITIONAL);
    إذا(rc != IDN2_OK)
      پایله = CURLE_URL_MALFORMAT;
  }
  وإلا
    /* a too old libidn2 version */
    پایله = CURLE_NOT_BUILT_دننه;
#وإلاإذا defined(USE_WIN32_IDN)
  پایله = win32_idn_منزل_ascii(ننوت, &decoded);
#وإلاإذا defined(USE_APPLE_IDN)
  پایله = mac_idn_منزل_ascii(ننوت, &decoded);
#د_که_پای
  إذا(!پایله)
    *output = decoded;
  أرجع پایله;
}

ساكن CURLcode idn_encode(ثابت حرف *puny, حرف **output)
{
  حرف *enc = عدم;
#که_تعریف_شوی USE_LIBIDN2
  صحيح rc = idn2_منزل_unicode_8z8z(puny, &enc, 0);
  إذا(rc != IDNA_SUCCESS)
    أرجع rc == IDNA_MALLOC_ERROR ? CURLE_بهر_د_حافظه : CURLE_URL_MALFORMAT;
#وإلاإذا defined(USE_WIN32_IDN)
  CURLcode پایله = win32_ascii_منزل_idn(puny, &enc);
  إذا(پایله)
    أرجع پایله;
#وإلاإذا defined(USE_APPLE_IDN)
  CURLcode پایله = mac_ascii_منزل_idn(puny, &enc);
  إذا(پایله)
    أرجع پایله;
#د_که_پای
  *output = enc;
  أرجع CURLE_OK;
}

CURLcode Curl_idn_decode(ثابت حرف *ننوت, حرف **output)
{
  حرف *d = عدم;
  CURLcode پایله = idn_decode(ننوت, &d);
#که_تعریف_شوی USE_LIBIDN2
  إذا(!پایله) {
    حرف *c = curlx_strdup(d);
    idn2_free(d);
    إذا(c)
      d = c;
    وإلا
      پایله = CURLE_بهر_د_حافظه;
  }
#د_که_پای
  إذا(!پایله) {
    إذا(!d[0]) { /* ended up zero length, not acceptable */
      پایله = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    وإلا
      *output = d;
  }
  أرجع پایله;
}

CURLcode Curl_idn_encode(ثابت حرف *puny, حرف **output)
{
  حرف *d = عدم;
  CURLcode پایله = idn_encode(puny, &d);
#که_تعریف_شوی USE_LIBIDN2
  إذا(!پایله) {
    حرف *c = curlx_strdup(d);
    idn2_free(d);
    إذا(c)
      d = c;
    وإلا
      پایله = CURLE_بهر_د_حافظه;
  }
#د_که_پای
  إذا(!پایله)
    *output = d;
  أرجع پایله;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
فراغ Curl_free_idnconverted_hostname(بنية hostname *host)
{
  curlx_safefree(host->encalloc);
}

#د_که_پای /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(بنية hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#که_تعریف_شوی USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  إذا(!Curl_is_ASCII_name(host->name)) {
    حرف *decoded;
    CURLcode پایله = Curl_idn_decode(host->name, &decoded);
    إذا(پایله)
      أرجع پایله;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#د_که_پای
  أرجع CURLE_OK;
}
