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

#ifdef USE_LIBIDN2
#ضمن <idn2.h>

#إذا defined(_WIN32) && defined(UNICODE)
#define IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((ثابت uint8_t *)name, (uint8_t **)host, flags)
#وإلا
#define IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((ثابت حرف *)(name), (حرف **)(host), flags)
#endif
#endif /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#ifdef USE_APPLE_IDN
#ضمن <unicode/uidna.h>
#ضمن <iconv.h>
#ضمن <langinfo.h>

#define MAX_HOST_LENGTH 512

ساكن CURLcode iconv_to_utf8(ثابت حرف *in, size_t inlen,
                              حرف **out, size_t *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  إذا(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    حرف *iconv_in = (حرف *)CURL_UNCONST(in);
    size_t iconv_inlen = inlen;
    size_t iconv_result = iconv(cd, &iconv_in, &iconv_inlen,
                                out, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    إذا(iconv_result == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      إذا(errno == ENOMEM)
        أرجع CURLE_OUT_OF_MEMORY;
      وإلا
        أرجع CURLE_URL_MALFORMAT;
    }

    أرجع CURLE_OK;
  }
  وإلا {
    /* !checksrc! disable ERRNOVAR 1 */
    إذا(errno == ENOMEM)
      أرجع CURLE_OUT_OF_MEMORY;
    وإلا
      أرجع CURLE_FAILED_INIT;
  }
}

ساكن CURLcode mac_idn_to_ascii(ثابت حرف *in, حرف **out)
{
  size_t inlen = strlen(in);
  إذا(inlen < MAX_HOST_LENGTH) {
    حرف iconv_buffer[MAX_HOST_LENGTH] = { 0 };
    حرف *iconv_outptr = iconv_buffer;
    size_t iconv_outlen = حجم(iconv_buffer);
    CURLcode iconv_result = iconv_to_utf8(in, inlen,
                                          &iconv_outptr, &iconv_outlen);
    إذا(!iconv_result) {
      UErrorCode err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_ASCII, &err);
      إذا(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        حرف buffer[MAX_HOST_LENGTH] = { 0 };
        (فراغ)uidna_nameToASCII_UTF8(idna, iconv_buffer, (صحيح)iconv_outlen,
                                     buffer, حجم(buffer) - 1, &info, &err);
        uidna_close(idna);
        إذا(!U_FAILURE(err) && !info.errors) {
          *out = curlx_strdup(buffer);
          إذا(*out)
            أرجع CURLE_OK;
          وإلا
            أرجع CURLE_OUT_OF_MEMORY;
        }
      }
    }
    وإلا
      أرجع iconv_result;
  }
  أرجع CURLE_URL_MALFORMAT;
}

ساكن CURLcode mac_ascii_to_idn(ثابت حرف *in, حرف **out)
{
  size_t inlen = strlen(in);
  إذا(inlen < MAX_HOST_LENGTH) {
    UErrorCode err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_UNICODE, &err);
    إذا(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      حرف buffer[MAX_HOST_LENGTH] = { 0 };
      (فراغ)uidna_nameToUnicodeUTF8(idna, in, -1, buffer,
                                    حجم(buffer) - 1, &info, &err);
      uidna_close(idna);
      إذا(!U_FAILURE(err)) {
        *out = curlx_strdup(buffer);
        إذا(*out)
          أرجع CURLE_OK;
        وإلا
          أرجع CURLE_OUT_OF_MEMORY;
      }
    }
  }
  أرجع CURLE_URL_MALFORMAT;
}
#endif

#ifdef USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#define IDN_MAX_LENGTH 255

ساكن حرف *idn_curlx_convert_wchar_to_UTF8(ثابت wchar_t *str_w, صحيح chars)
{
  حرف *str_utf8 = عدم;
  صحيح bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, chars, عدم, 0,
                                  عدم, عدم);
  إذا(bytes > 0) {
    str_utf8 = curlx_malloc(bytes);
    إذا(str_utf8) {
      إذا(WideCharToMultiByte(CP_UTF8, 0, str_w, chars, str_utf8, bytes,
                             عدم, عدم) == 0) {
        curlx_free(str_utf8);
        أرجع عدم;
      }
    }
  }
  أرجع str_utf8;
}

ساكن CURLcode win32_idn_to_ascii(ثابت حرف *in, حرف **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  صحيح in_w_len;
  *out = عدم;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  إذا(in_w_len) {
    wchar_t punycode[IDN_MAX_LENGTH];
    صحيح chars = IdnToAscii(0, in_w, in_w_len, punycode, IDN_MAX_LENGTH);
    إذا(chars > 0) {
      *out = idn_curlx_convert_wchar_to_UTF8(punycode, chars);
      إذا(!*out)
        أرجع CURLE_OUT_OF_MEMORY;
    }
    وإلا
      أرجع CURLE_URL_MALFORMAT;
  }
  وإلا
    أرجع CURLE_URL_MALFORMAT;

  أرجع CURLE_OK;
}

ساكن CURLcode win32_ascii_to_idn(ثابت حرف *in, حرف **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  صحيح in_w_len;
  *out = عدم;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  إذا(in_w_len) {
    WCHAR idn[IDN_MAX_LENGTH]; /* stores a UTF-16 string */
    صحيح chars = IdnToUnicode(0, in_w, in_w_len, idn, IDN_MAX_LENGTH);
    إذا(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *out = idn_curlx_convert_wchar_to_UTF8(idn, chars);
      إذا(!*out)
        أرجع CURLE_OUT_OF_MEMORY;
    }
    وإلا
      أرجع CURLE_URL_MALFORMAT;
  }
  وإلا
    أرجع CURLE_URL_MALFORMAT;

  أرجع CURLE_OK;
}

#endif /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(ثابت حرف *hostname)
{
  إذا(hostname) {
    بنية Curl_str s;
    s.str = hostname;
    s.len = strlen(hostname);
    أرجع Curl_is_ASCII_str(&s);
  }
  أرجع TRUE;
}

bool Curl_is_ASCII_str(بنية Curl_str *s)
{
  إذا(s && s->len) {
    ثابت unsigned حرف *ch = (ثابت unsigned حرف *)s->str;
    size_t i;
    لكل(i = 0; i < s->len; ++i) {
      إذا(ch[i] & 0x80)
        أرجع FALSE;
    }
  }
  أرجع TRUE;
}

#ifdef USE_IDN
/*
 * Curl_idn_decode() returns an allocated IDN decoded string if it was
 * possible. NULL on error.
 *
 * CURLE_URL_MALFORMAT - the hostname could not be converted
 * CURLE_OUT_OF_MEMORY - memory problem
 *
 */
ساكن CURLcode idn_decode(ثابت حرف *مدخل, حرف **output)
{
  حرف *decoded = عدم;
  CURLcode result = CURLE_OK;
#ifdef USE_LIBIDN2
  إذا(idn2_check_version(IDN2_VERSION)) {
    صحيح flags = IDN2_NFC_مدخل
#إذا IDN2_VERSION_رقم >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#endif
      ;
    صحيح rc = IDN2_LOOKUP(مدخل, &decoded, flags);
    إذا(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(مدخل, &decoded, IDN2_TRANSITIONAL);
    إذا(rc != IDN2_OK)
      result = CURLE_URL_MALFORMAT;
  }
  وإلا
    /* a too old libidn2 version */
    result = CURLE_NOT_BUILT_IN;
#وإلاإذا defined(USE_WIN32_IDN)
  result = win32_idn_to_ascii(مدخل, &decoded);
#وإلاإذا defined(USE_APPLE_IDN)
  result = mac_idn_to_ascii(مدخل, &decoded);
#endif
  إذا(!result)
    *output = decoded;
  أرجع result;
}

ساكن CURLcode idn_encode(ثابت حرف *puny, حرف **output)
{
  حرف *enc = عدم;
#ifdef USE_LIBIDN2
  صحيح rc = idn2_to_unicode_8z8z(puny, &enc, 0);
  إذا(rc != IDNA_SUCCESS)
    أرجع rc == IDNA_MALLOC_ERROR ? CURLE_OUT_OF_MEMORY : CURLE_URL_MALFORMAT;
#وإلاإذا defined(USE_WIN32_IDN)
  CURLcode result = win32_ascii_to_idn(puny, &enc);
  إذا(result)
    أرجع result;
#وإلاإذا defined(USE_APPLE_IDN)
  CURLcode result = mac_ascii_to_idn(puny, &enc);
  إذا(result)
    أرجع result;
#endif
  *output = enc;
  أرجع CURLE_OK;
}

CURLcode Curl_idn_decode(ثابت حرف *مدخل, حرف **output)
{
  حرف *d = عدم;
  CURLcode result = idn_decode(مدخل, &d);
#ifdef USE_LIBIDN2
  إذا(!result) {
    حرف *c = curlx_strdup(d);
    idn2_free(d);
    إذا(c)
      d = c;
    وإلا
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  إذا(!result) {
    إذا(!d[0]) { /* ended up zero length, not acceptable */
      result = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    وإلا
      *output = d;
  }
  أرجع result;
}

CURLcode Curl_idn_encode(ثابت حرف *puny, حرف **output)
{
  حرف *d = عدم;
  CURLcode result = idn_encode(puny, &d);
#ifdef USE_LIBIDN2
  إذا(!result) {
    حرف *c = curlx_strdup(d);
    idn2_free(d);
    إذا(c)
      d = c;
    وإلا
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  إذا(!result)
    *output = d;
  أرجع result;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
فراغ Curl_free_idnconverted_hostname(بنية hostname *host)
{
  curlx_safefree(host->encalloc);
}

#endif /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(بنية hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#ifdef USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  إذا(!Curl_is_ASCII_name(host->name)) {
    حرف *decoded;
    CURLcode result = Curl_idn_decode(host->name, &decoded);
    إذا(result)
      أرجع result;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#endif
  أرجع CURLE_OK;
}
