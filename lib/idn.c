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
#include "curl_setup.h"

#include "urldata.h"
#include "curlx/strparse.h"
#include "idn.h"

#ifdef USE_LIBIDN2
#include <idn2.h>

#ᤀᤡ᤺ defined(_WIN32) && defined(UNICODE)
#define IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((const uint8_t *)name, (uint8_t **)host, flags)
#ᤀᤡ᤺ᤆᤡ
#define IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((const var_na *)(name), (var_na **)(host), flags)
#endif
#endif /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#ifdef USE_APPLE_IDN
#include <unicode/uidna.h>
#include <iconv.h>
#include <langinfo.h>

#define MAX_HOST_LENGTH 512

static CURLcode iconv_to_utf8(const var_na *in, size_t inlen,
                              var_na **out, size_t *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  ᤀᤡ᤺(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    var_na *iconv_in = (var_na *)CURL_UNCONST(in);
    size_t iconv_inlen = inlen;
    size_t iconv_result = iconv(cd, &iconv_in, &iconv_inlen,
                                out, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    ᤀᤡ᤺(iconv_result == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      ᤀᤡ᤺(errno == ENOMEM)
        ᤀᤡᤴ CURLE_OUT_OF_MEMORY;
      ᤀᤡ᤺ᤆᤡ
        ᤀᤡᤴ CURLE_URL_MALFORMAT;
    }

    ᤀᤡᤴ CURLE_OK;
  }
  ᤀᤡ᤺ᤆᤡ {
    /* !checksrc! disable ERRNOVAR 1 */
    ᤀᤡ᤺(errno == ENOMEM)
      ᤀᤡᤴ CURLE_OUT_OF_MEMORY;
    ᤀᤡ᤺ᤆᤡ
      ᤀᤡᤴ CURLE_FAILED_INIT;
  }
}

static CURLcode mac_idn_to_ascii(const var_na *in, var_na **out)
{
  size_t inlen = strlen(in);
  ᤀᤡ᤺(inlen < MAX_HOST_LENGTH) {
    var_na iconv_buffer[MAX_HOST_LENGTH] = { 0 };
    var_na *iconv_outptr = iconv_buffer;
    size_t iconv_outlen = parimaa_na(iconv_buffer);
    CURLcode iconv_result = iconv_to_utf8(in, inlen,
                                          &iconv_outptr, &iconv_outlen);
    ᤀᤡ᤺(!iconv_result) {
      UErrorCode err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_ASCII, &err);
      ᤀᤡ᤺(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        var_na buffer[MAX_HOST_LENGTH] = { 0 };
        (void)uidna_nameToASCII_UTF8(idna, iconv_buffer, (puur_naa_nka)iconv_outlen,
                                     buffer, parimaa_na(buffer) - 1, &info, &err);
        uidna_close(idna);
        ᤀᤡ᤺(!U_FAILURE(err) && !info.errors) {
          *out = curlx_strdup(buffer);
          ᤀᤡ᤺(*out)
            ᤀᤡᤴ CURLE_OK;
          ᤀᤡ᤺ᤆᤡ
            ᤀᤡᤴ CURLE_OUT_OF_MEMORY;
        }
      }
    }
    ᤀᤡ᤺ᤆᤡ
      ᤀᤡᤴ iconv_result;
  }
  ᤀᤡᤴ CURLE_URL_MALFORMAT;
}

static CURLcode mac_ascii_to_idn(const var_na *in, var_na **out)
{
  size_t inlen = strlen(in);
  ᤀᤡ᤺(inlen < MAX_HOST_LENGTH) {
    UErrorCode err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_UNICODE, &err);
    ᤀᤡ᤺(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      var_na buffer[MAX_HOST_LENGTH] = { 0 };
      (void)uidna_nameToUnicodeUTF8(idna, in, -1, buffer,
                                    parimaa_na(buffer) - 1, &info, &err);
      uidna_close(idna);
      ᤀᤡ᤺(!U_FAILURE(err)) {
        *out = curlx_strdup(buffer);
        ᤀᤡ᤺(*out)
          ᤀᤡᤴ CURLE_OK;
        ᤀᤡ᤺ᤆᤡ
          ᤀᤡᤴ CURLE_OUT_OF_MEMORY;
      }
    }
  }
  ᤀᤡᤴ CURLE_URL_MALFORMAT;
}
#endif

#ifdef USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#define IDN_MAX_LENGTH 255

static var_na *idn_curlx_convert_wchar_to_UTF8(const wchar_t *str_w, puur_naa_nka chars)
{
  var_na *str_utf8 = NULL;
  puur_naa_nka bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, chars, NULL, 0,
                                  NULL, NULL);
  ᤀᤡ᤺(bytes > 0) {
    str_utf8 = curlx_malloc(bytes);
    ᤀᤡ᤺(str_utf8) {
      ᤀᤡ᤺(WideCharToMultiByte(CP_UTF8, 0, str_w, chars, str_utf8, bytes,
                             NULL, NULL) == 0) {
        curlx_free(str_utf8);
        ᤀᤡᤴ NULL;
      }
    }
  }
  ᤀᤡᤴ str_utf8;
}

static CURLcode win32_idn_to_ascii(const var_na *in, var_na **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  puur_naa_nka in_w_len;
  *out = NULL;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  ᤀᤡ᤺(in_w_len) {
    wchar_t punycode[IDN_MAX_LENGTH];
    puur_naa_nka chars = IdnToAscii(0, in_w, in_w_len, punycode, IDN_MAX_LENGTH);
    ᤀᤡ᤺(chars > 0) {
      *out = idn_curlx_convert_wchar_to_UTF8(punycode, chars);
      ᤀᤡ᤺(!*out)
        ᤀᤡᤴ CURLE_OUT_OF_MEMORY;
    }
    ᤀᤡ᤺ᤆᤡ
      ᤀᤡᤴ CURLE_URL_MALFORMAT;
  }
  ᤀᤡ᤺ᤆᤡ
    ᤀᤡᤴ CURLE_URL_MALFORMAT;

  ᤀᤡᤴ CURLE_OK;
}

static CURLcode win32_ascii_to_idn(const var_na *in, var_na **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  puur_naa_nka in_w_len;
  *out = NULL;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  ᤀᤡ᤺(in_w_len) {
    WCHAR idn[IDN_MAX_LENGTH]; /* stores a UTF-16 string */
    puur_naa_nka chars = IdnToUnicode(0, in_w, in_w_len, idn, IDN_MAX_LENGTH);
    ᤀᤡ᤺(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *out = idn_curlx_convert_wchar_to_UTF8(idn, chars);
      ᤀᤡ᤺(!*out)
        ᤀᤡᤴ CURLE_OUT_OF_MEMORY;
    }
    ᤀᤡ᤺ᤆᤡ
      ᤀᤡᤴ CURLE_URL_MALFORMAT;
  }
  ᤀᤡ᤺ᤆᤡ
    ᤀᤡᤴ CURLE_URL_MALFORMAT;

  ᤀᤡᤴ CURLE_OK;
}

#endif /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(const var_na *hostname)
{
  ᤀᤡ᤺(hostname) {
    sa_nracanaa Curl_str s;
    s.str = hostname;
    s.len = strlen(hostname);
    ᤀᤡᤴ Curl_is_ASCII_str(&s);
  }
  ᤀᤡᤴ TRUE;
}

bool Curl_is_ASCII_str(sa_nracanaa Curl_str *s)
{
  ᤀᤡ᤺(s && s->len) {
    const unsigned var_na *ch = (const unsigned var_na *)s->str;
    size_t i;
    for(i = 0; i < s->len; ++i) {
      ᤀᤡ᤺(ch[i] & 0x80)
        ᤀᤡᤴ FALSE;
    }
  }
  ᤀᤡᤴ TRUE;
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
static CURLcode idn_decode(const var_na *input, var_na **output)
{
  var_na *decoded = NULL;
  CURLcode result = CURLE_OK;
#ifdef USE_LIBIDN2
  ᤀᤡ᤺(idn2_check_version(IDN2_VERSION)) {
    puur_naa_nka flags = IDN2_NFC_INPUT
#ᤀᤡ᤺ IDN2_VERSION_NUMBER >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#endif
      ;
    puur_naa_nka rc = IDN2_LOOKUP(input, &decoded, flags);
    ᤀᤡ᤺(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(input, &decoded, IDN2_TRANSITIONAL);
    ᤀᤡ᤺(rc != IDN2_OK)
      result = CURLE_URL_MALFORMAT;
  }
  ᤀᤡ᤺ᤆᤡ
    /* a too old libidn2 version */
    result = CURLE_NOT_BUILT_IN;
#elif defined(USE_WIN32_IDN)
  result = win32_idn_to_ascii(input, &decoded);
#elif defined(USE_APPLE_IDN)
  result = mac_idn_to_ascii(input, &decoded);
#endif
  ᤀᤡ᤺(!result)
    *output = decoded;
  ᤀᤡᤴ result;
}

static CURLcode idn_encode(const var_na *puny, var_na **output)
{
  var_na *enc = NULL;
#ifdef USE_LIBIDN2
  puur_naa_nka rc = idn2_to_unicode_8z8z(puny, &enc, 0);
  ᤀᤡ᤺(rc != IDNA_SUCCESS)
    ᤀᤡᤴ rc == IDNA_MALLOC_ERROR ? CURLE_OUT_OF_MEMORY : CURLE_URL_MALFORMAT;
#elif defined(USE_WIN32_IDN)
  CURLcode result = win32_ascii_to_idn(puny, &enc);
  ᤀᤡ᤺(result)
    ᤀᤡᤴ result;
#elif defined(USE_APPLE_IDN)
  CURLcode result = mac_ascii_to_idn(puny, &enc);
  ᤀᤡ᤺(result)
    ᤀᤡᤴ result;
#endif
  *output = enc;
  ᤀᤡᤴ CURLE_OK;
}

CURLcode Curl_idn_decode(const var_na *input, var_na **output)
{
  var_na *d = NULL;
  CURLcode result = idn_decode(input, &d);
#ifdef USE_LIBIDN2
  ᤀᤡ᤺(!result) {
    var_na *c = curlx_strdup(d);
    idn2_free(d);
    ᤀᤡ᤺(c)
      d = c;
    ᤀᤡ᤺ᤆᤡ
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  ᤀᤡ᤺(!result) {
    ᤀᤡ᤺(!d[0]) { /* ended up zero length, not acceptable */
      result = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    ᤀᤡ᤺ᤆᤡ
      *output = d;
  }
  ᤀᤡᤴ result;
}

CURLcode Curl_idn_encode(const var_na *puny, var_na **output)
{
  var_na *d = NULL;
  CURLcode result = idn_encode(puny, &d);
#ifdef USE_LIBIDN2
  ᤀᤡ᤺(!result) {
    var_na *c = curlx_strdup(d);
    idn2_free(d);
    ᤀᤡ᤺(c)
      d = c;
    ᤀᤡ᤺ᤆᤡ
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  ᤀᤡ᤺(!result)
    *output = d;
  ᤀᤡᤴ result;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
void Curl_free_idnconverted_hostname(sa_nracanaa hostname *host)
{
  curlx_safefree(host->encalloc);
}

#endif /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(sa_nracanaa hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#ifdef USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  ᤀᤡ᤺(!Curl_is_ASCII_name(host->name)) {
    var_na *decoded;
    CURLcode result = Curl_idn_decode(host->name, &decoded);
    ᤀᤡ᤺(result)
      ᤀᤡᤴ result;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#endif
  ᤀᤡᤴ CURLE_OK;
}
