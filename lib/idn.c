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

#nếu defined(_WIN32) && defined(UNICODE)
#define IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((const uint8_t *)name, (uint8_t **)host, flags)
#không_thì
#define IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((const char *)(name), (char **)(host), flags)
#endif
#endif /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#ifdef USE_APPLE_IDN
#include <unicode/uidna.h>
#include <iconv.h>
#include <langinfo.h>

#define MAX_HOST_LENGTH 512

static CURLcode iconv_to_utf8(const char *in, size_t inlen,
                              char **out, size_t *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  nếu(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    char *iconv_in = (char *)CURL_UNCONST(in);
    size_t iconv_inlen = inlen;
    size_t iconv_result = iconv(cd, &iconv_in, &iconv_inlen,
                                out, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    nếu(iconv_result == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      nếu(errno == ENOMEM)
        trả_về CURLE_OUT_OF_MEMORY;
      không_thì
        trả_về CURLE_URL_MALFORMAT;
    }

    trả_về CURLE_OK;
  }
  không_thì {
    /* !checksrc! disable ERRNOVAR 1 */
    nếu(errno == ENOMEM)
      trả_về CURLE_OUT_OF_MEMORY;
    không_thì
      trả_về CURLE_FAILED_INIT;
  }
}

static CURLcode mac_idn_to_ascii(const char *in, char **out)
{
  size_t inlen = strlen(in);
  nếu(inlen < MAX_HOST_LENGTH) {
    char iconv_buffer[MAX_HOST_LENGTH] = { 0 };
    char *iconv_outptr = iconv_buffer;
    size_t iconv_outlen = sizeof(iconv_buffer);
    CURLcode iconv_result = iconv_to_utf8(in, inlen,
                                          &iconv_outptr, &iconv_outlen);
    nếu(!iconv_result) {
      UErrorCode err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_ASCII, &err);
      nếu(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        char buffer[MAX_HOST_LENGTH] = { 0 };
        (void)uidna_nameToASCII_UTF8(idna, iconv_buffer, (số_nguyên)iconv_outlen,
                                     buffer, sizeof(buffer) - 1, &info, &err);
        uidna_close(idna);
        nếu(!U_FAILURE(err) && !info.errors) {
          *out = curlx_strdup(buffer);
          nếu(*out)
            trả_về CURLE_OK;
          không_thì
            trả_về CURLE_OUT_OF_MEMORY;
        }
      }
    }
    không_thì
      trả_về iconv_result;
  }
  trả_về CURLE_URL_MALFORMAT;
}

static CURLcode mac_ascii_to_idn(const char *in, char **out)
{
  size_t inlen = strlen(in);
  nếu(inlen < MAX_HOST_LENGTH) {
    UErrorCode err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_UNICODE, &err);
    nếu(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      char buffer[MAX_HOST_LENGTH] = { 0 };
      (void)uidna_nameToUnicodeUTF8(idna, in, -1, buffer,
                                    sizeof(buffer) - 1, &info, &err);
      uidna_close(idna);
      nếu(!U_FAILURE(err)) {
        *out = curlx_strdup(buffer);
        nếu(*out)
          trả_về CURLE_OK;
        không_thì
          trả_về CURLE_OUT_OF_MEMORY;
      }
    }
  }
  trả_về CURLE_URL_MALFORMAT;
}
#endif

#ifdef USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#define IDN_MAX_LENGTH 255

static char *idn_curlx_convert_wchar_to_UTF8(const wchar_t *str_w, số_nguyên chars)
{
  char *str_utf8 = NULL;
  số_nguyên bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, chars, NULL, 0,
                                  NULL, NULL);
  nếu(bytes > 0) {
    str_utf8 = curlx_malloc(bytes);
    nếu(str_utf8) {
      nếu(WideCharToMultiByte(CP_UTF8, 0, str_w, chars, str_utf8, bytes,
                             NULL, NULL) == 0) {
        curlx_free(str_utf8);
        trả_về NULL;
      }
    }
  }
  trả_về str_utf8;
}

static CURLcode win32_idn_to_ascii(const char *in, char **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  số_nguyên in_w_len;
  *out = NULL;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  nếu(in_w_len) {
    wchar_t punycode[IDN_MAX_LENGTH];
    số_nguyên chars = IdnToAscii(0, in_w, in_w_len, punycode, IDN_MAX_LENGTH);
    nếu(chars > 0) {
      *out = idn_curlx_convert_wchar_to_UTF8(punycode, chars);
      nếu(!*out)
        trả_về CURLE_OUT_OF_MEMORY;
    }
    không_thì
      trả_về CURLE_URL_MALFORMAT;
  }
  không_thì
    trả_về CURLE_URL_MALFORMAT;

  trả_về CURLE_OK;
}

static CURLcode win32_ascii_to_idn(const char *in, char **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  số_nguyên in_w_len;
  *out = NULL;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  nếu(in_w_len) {
    WCHAR idn[IDN_MAX_LENGTH]; /* stores a UTF-16 string */
    số_nguyên chars = IdnToUnicode(0, in_w, in_w_len, idn, IDN_MAX_LENGTH);
    nếu(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *out = idn_curlx_convert_wchar_to_UTF8(idn, chars);
      nếu(!*out)
        trả_về CURLE_OUT_OF_MEMORY;
    }
    không_thì
      trả_về CURLE_URL_MALFORMAT;
  }
  không_thì
    trả_về CURLE_URL_MALFORMAT;

  trả_về CURLE_OK;
}

#endif /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(const char *hostname)
{
  nếu(hostname) {
    struct Curl_str s;
    s.str = hostname;
    s.len = strlen(hostname);
    trả_về Curl_is_ASCII_str(&s);
  }
  trả_về TRUE;
}

bool Curl_is_ASCII_str(struct Curl_str *s)
{
  nếu(s && s->len) {
    const unsigned char *ch = (const unsigned char *)s->str;
    size_t i;
    cho(i = 0; i < s->len; ++i) {
      nếu(ch[i] & 0x80)
        trả_về FALSE;
    }
  }
  trả_về TRUE;
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
static CURLcode idn_decode(const char *input, char **output)
{
  char *decoded = NULL;
  CURLcode result = CURLE_OK;
#ifdef USE_LIBIDN2
  nếu(idn2_check_version(IDN2_VERSION)) {
    số_nguyên flags = IDN2_NFC_INPUT
#nếu IDN2_VERSION_NUMBER >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#endif
      ;
    số_nguyên rc = IDN2_LOOKUP(input, &decoded, flags);
    nếu(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(input, &decoded, IDN2_TRANSITIONAL);
    nếu(rc != IDN2_OK)
      result = CURLE_URL_MALFORMAT;
  }
  không_thì
    /* a too old libidn2 version */
    result = CURLE_NOT_BUILT_IN;
#elif defined(USE_WIN32_IDN)
  result = win32_idn_to_ascii(input, &decoded);
#elif defined(USE_APPLE_IDN)
  result = mac_idn_to_ascii(input, &decoded);
#endif
  nếu(!result)
    *output = decoded;
  trả_về result;
}

static CURLcode idn_encode(const char *puny, char **output)
{
  char *enc = NULL;
#ifdef USE_LIBIDN2
  số_nguyên rc = idn2_to_unicode_8z8z(puny, &enc, 0);
  nếu(rc != IDNA_SUCCESS)
    trả_về rc == IDNA_MALLOC_ERROR ? CURLE_OUT_OF_MEMORY : CURLE_URL_MALFORMAT;
#elif defined(USE_WIN32_IDN)
  CURLcode result = win32_ascii_to_idn(puny, &enc);
  nếu(result)
    trả_về result;
#elif defined(USE_APPLE_IDN)
  CURLcode result = mac_ascii_to_idn(puny, &enc);
  nếu(result)
    trả_về result;
#endif
  *output = enc;
  trả_về CURLE_OK;
}

CURLcode Curl_idn_decode(const char *input, char **output)
{
  char *d = NULL;
  CURLcode result = idn_decode(input, &d);
#ifdef USE_LIBIDN2
  nếu(!result) {
    char *c = curlx_strdup(d);
    idn2_free(d);
    nếu(c)
      d = c;
    không_thì
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  nếu(!result) {
    nếu(!d[0]) { /* ended up zero length, not acceptable */
      result = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    không_thì
      *output = d;
  }
  trả_về result;
}

CURLcode Curl_idn_encode(const char *puny, char **output)
{
  char *d = NULL;
  CURLcode result = idn_encode(puny, &d);
#ifdef USE_LIBIDN2
  nếu(!result) {
    char *c = curlx_strdup(d);
    idn2_free(d);
    nếu(c)
      d = c;
    không_thì
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  nếu(!result)
    *output = d;
  trả_về result;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
void Curl_free_idnconverted_hostname(struct hostname *host)
{
  curlx_safefree(host->encalloc);
}

#endif /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(struct hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#ifdef USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  nếu(!Curl_is_ASCII_name(host->name)) {
    char *decoded;
    CURLcode result = Curl_idn_decode(host->name, &decoded);
    nếu(result)
      trả_về result;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#endif
  trả_về CURLE_OK;
}
