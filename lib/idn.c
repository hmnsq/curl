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

#ако defined(_WIN32) && defined(UNICODE)
#define IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((const uint8_t *)name, (uint8_t **)host, flags)
#инако
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
  ако(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    char *iconv_in = (char *)CURL_UNCONST(in);
    size_t iconv_inlen = inlen;
    size_t iconv_result = iconv(cd, &iconv_in, &iconv_inlen,
                                out, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    ако(iconv_result == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      ако(errno == ENOMEM)
        врати CURLE_OUT_OF_MEMORY;
      инако
        врати CURLE_URL_MALFORMAT;
    }

    врати CURLE_OK;
  }
  инако {
    /* !checksrc! disable ERRNOVAR 1 */
    ако(errno == ENOMEM)
      врати CURLE_OUT_OF_MEMORY;
    инако
      врати CURLE_FAILED_INIT;
  }
}

static CURLcode mac_idn_to_ascii(const char *in, char **out)
{
  size_t inlen = strlen(in);
  ако(inlen < MAX_HOST_LENGTH) {
    char iconv_buffer[MAX_HOST_LENGTH] = { 0 };
    char *iconv_outptr = iconv_buffer;
    size_t iconv_outlen = sizeof(iconv_buffer);
    CURLcode iconv_result = iconv_to_utf8(in, inlen,
                                          &iconv_outptr, &iconv_outlen);
    ако(!iconv_result) {
      UErrorCode err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_ASCII, &err);
      ако(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        char buffer[MAX_HOST_LENGTH] = { 0 };
        (void)uidna_nameToASCII_UTF8(idna, iconv_buffer, (цео)iconv_outlen,
                                     buffer, sizeof(buffer) - 1, &info, &err);
        uidna_close(idna);
        ако(!U_FAILURE(err) && !info.errors) {
          *out = curlx_strdup(buffer);
          ако(*out)
            врати CURLE_OK;
          инако
            врати CURLE_OUT_OF_MEMORY;
        }
      }
    }
    инако
      врати iconv_result;
  }
  врати CURLE_URL_MALFORMAT;
}

static CURLcode mac_ascii_to_idn(const char *in, char **out)
{
  size_t inlen = strlen(in);
  ако(inlen < MAX_HOST_LENGTH) {
    UErrorCode err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_UNICODE, &err);
    ако(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      char buffer[MAX_HOST_LENGTH] = { 0 };
      (void)uidna_nameToUnicodeUTF8(idna, in, -1, buffer,
                                    sizeof(buffer) - 1, &info, &err);
      uidna_close(idna);
      ако(!U_FAILURE(err)) {
        *out = curlx_strdup(buffer);
        ако(*out)
          врати CURLE_OK;
        инако
          врати CURLE_OUT_OF_MEMORY;
      }
    }
  }
  врати CURLE_URL_MALFORMAT;
}
#endif

#ifdef USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#define IDN_MAX_LENGTH 255

static char *idn_curlx_convert_wchar_to_UTF8(const wchar_t *str_w, цео chars)
{
  char *str_utf8 = NULL;
  цео bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, chars, NULL, 0,
                                  NULL, NULL);
  ако(bytes > 0) {
    str_utf8 = curlx_malloc(bytes);
    ако(str_utf8) {
      ако(WideCharToMultiByte(CP_UTF8, 0, str_w, chars, str_utf8, bytes,
                             NULL, NULL) == 0) {
        curlx_free(str_utf8);
        врати NULL;
      }
    }
  }
  врати str_utf8;
}

static CURLcode win32_idn_to_ascii(const char *in, char **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  цео in_w_len;
  *out = NULL;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  ако(in_w_len) {
    wchar_t punycode[IDN_MAX_LENGTH];
    цео chars = IdnToAscii(0, in_w, in_w_len, punycode, IDN_MAX_LENGTH);
    ако(chars > 0) {
      *out = idn_curlx_convert_wchar_to_UTF8(punycode, chars);
      ако(!*out)
        врати CURLE_OUT_OF_MEMORY;
    }
    инако
      врати CURLE_URL_MALFORMAT;
  }
  инако
    врати CURLE_URL_MALFORMAT;

  врати CURLE_OK;
}

static CURLcode win32_ascii_to_idn(const char *in, char **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  цео in_w_len;
  *out = NULL;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  ако(in_w_len) {
    WCHAR idn[IDN_MAX_LENGTH]; /* stores a UTF-16 string */
    цео chars = IdnToUnicode(0, in_w, in_w_len, idn, IDN_MAX_LENGTH);
    ако(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *out = idn_curlx_convert_wchar_to_UTF8(idn, chars);
      ако(!*out)
        врати CURLE_OUT_OF_MEMORY;
    }
    инако
      врати CURLE_URL_MALFORMAT;
  }
  инако
    врати CURLE_URL_MALFORMAT;

  врати CURLE_OK;
}

#endif /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(const char *hostname)
{
  ако(hostname) {
    struct Curl_str s;
    s.str = hostname;
    s.len = strlen(hostname);
    врати Curl_is_ASCII_str(&s);
  }
  врати TRUE;
}

bool Curl_is_ASCII_str(struct Curl_str *s)
{
  ако(s && s->len) {
    const unsigned char *ch = (const unsigned char *)s->str;
    size_t i;
    за(i = 0; i < s->len; ++i) {
      ако(ch[i] & 0x80)
        врати FALSE;
    }
  }
  врати TRUE;
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
  ако(idn2_check_version(IDN2_VERSION)) {
    цео flags = IDN2_NFC_INPUT
#ако IDN2_VERSION_NUMBER >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#endif
      ;
    цео rc = IDN2_LOOKUP(input, &decoded, flags);
    ако(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(input, &decoded, IDN2_TRANSITIONAL);
    ако(rc != IDN2_OK)
      result = CURLE_URL_MALFORMAT;
  }
  инако
    /* a too old libidn2 version */
    result = CURLE_NOT_BUILT_IN;
#elif defined(USE_WIN32_IDN)
  result = win32_idn_to_ascii(input, &decoded);
#elif defined(USE_APPLE_IDN)
  result = mac_idn_to_ascii(input, &decoded);
#endif
  ако(!result)
    *output = decoded;
  врати result;
}

static CURLcode idn_encode(const char *puny, char **output)
{
  char *enc = NULL;
#ifdef USE_LIBIDN2
  цео rc = idn2_to_unicode_8z8z(puny, &enc, 0);
  ако(rc != IDNA_SUCCESS)
    врати rc == IDNA_MALLOC_ERROR ? CURLE_OUT_OF_MEMORY : CURLE_URL_MALFORMAT;
#elif defined(USE_WIN32_IDN)
  CURLcode result = win32_ascii_to_idn(puny, &enc);
  ако(result)
    врати result;
#elif defined(USE_APPLE_IDN)
  CURLcode result = mac_ascii_to_idn(puny, &enc);
  ако(result)
    врати result;
#endif
  *output = enc;
  врати CURLE_OK;
}

CURLcode Curl_idn_decode(const char *input, char **output)
{
  char *d = NULL;
  CURLcode result = idn_decode(input, &d);
#ifdef USE_LIBIDN2
  ако(!result) {
    char *c = curlx_strdup(d);
    idn2_free(d);
    ако(c)
      d = c;
    инако
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  ако(!result) {
    ако(!d[0]) { /* ended up zero length, not acceptable */
      result = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    инако
      *output = d;
  }
  врати result;
}

CURLcode Curl_idn_encode(const char *puny, char **output)
{
  char *d = NULL;
  CURLcode result = idn_encode(puny, &d);
#ifdef USE_LIBIDN2
  ако(!result) {
    char *c = curlx_strdup(d);
    idn2_free(d);
    ако(c)
      d = c;
    инако
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  ако(!result)
    *output = d;
  врати result;
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
  ако(!Curl_is_ASCII_name(host->name)) {
    char *decoded;
    CURLcode result = Curl_idn_decode(host->name, &decoded);
    ако(result)
      врати result;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#endif
  врати CURLE_OK;
}
