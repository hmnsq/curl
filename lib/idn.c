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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "idn.h"

#ifdef USE_LIBIDN2
#ଅନ୍ତର୍ଭାଵ <idn2.h>

#ଯଦି defined(_WIN32) && defined(UNICODE)
#define IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((ଅଚଳ uint8_t *)name, (uint8_t **)host, flags)
#ଅନ୍ୟଥା
#define IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((ଅଚଳ ଅକ୍ଷର *)(name), (ଅକ୍ଷର **)(host), flags)
#endif
#endif /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#ifdef USE_APPLE_IDN
#ଅନ୍ତର୍ଭାଵ <unicode/uidna.h>
#ଅନ୍ତର୍ଭାଵ <iconv.h>
#ଅନ୍ତର୍ଭାଵ <langinfo.h>

#define MAX_HOST_LENGTH 512

ସ୍ଥିର CURLcode iconv_to_utf8(ଅଚଳ ଅକ୍ଷର *in, size_t inlen,
                              ଅକ୍ଷର **out, size_t *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  ଯଦି(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    ଅକ୍ଷର *iconv_in = (ଅକ୍ଷର *)CURL_UNCONST(in);
    size_t iconv_inlen = inlen;
    size_t iconv_result = iconv(cd, &iconv_in, &iconv_inlen,
                                out, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    ଯଦି(iconv_result == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      ଯଦି(errno == ENOMEM)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      ଅନ୍ୟଥା
        ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
    }

    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଅନ୍ୟଥା {
    /* !checksrc! disable ERRNOVAR 1 */
    ଯଦି(errno == ENOMEM)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }
}

ସ୍ଥିର CURLcode mac_idn_to_ascii(ଅଚଳ ଅକ୍ଷର *in, ଅକ୍ଷର **out)
{
  size_t inlen = strlen(in);
  ଯଦି(inlen < MAX_HOST_LENGTH) {
    ଅକ୍ଷର iconv_buffer[MAX_HOST_LENGTH] = { 0 };
    ଅକ୍ଷର *iconv_outptr = iconv_buffer;
    size_t iconv_outlen = ଆକାର(iconv_buffer);
    CURLcode iconv_result = iconv_to_utf8(in, inlen,
                                          &iconv_outptr, &iconv_outlen);
    ଯଦି(!iconv_result) {
      UErrorCode err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_ASCII, &err);
      ଯଦି(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        ଅକ୍ଷର buffer[MAX_HOST_LENGTH] = { 0 };
        (ଶୂନ୍ୟ)uidna_nameToASCII_UTF8(idna, iconv_buffer, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)iconv_outlen,
                                     buffer, ଆକାର(buffer) - 1, &info, &err);
        uidna_close(idna);
        ଯଦି(!U_FAILURE(err) && !info.errors) {
          *out = curlx_strdup(buffer);
          ଯଦି(*out)
            ଫେରନ୍ତୁ CURLE_OK;
          ଅନ୍ୟଥା
            ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
        }
      }
    }
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ iconv_result;
  }
  ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
}

ସ୍ଥିର CURLcode mac_ascii_to_idn(ଅଚଳ ଅକ୍ଷର *in, ଅକ୍ଷର **out)
{
  size_t inlen = strlen(in);
  ଯଦି(inlen < MAX_HOST_LENGTH) {
    UErrorCode err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_UNICODE, &err);
    ଯଦି(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      ଅକ୍ଷର buffer[MAX_HOST_LENGTH] = { 0 };
      (ଶୂନ୍ୟ)uidna_nameToUnicodeUTF8(idna, in, -1, buffer,
                                    ଆକାର(buffer) - 1, &info, &err);
      uidna_close(idna);
      ଯଦି(!U_FAILURE(err)) {
        *out = curlx_strdup(buffer);
        ଯଦି(*out)
          ଫେରନ୍ତୁ CURLE_OK;
        ଅନ୍ୟଥା
          ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      }
    }
  }
  ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
}
#endif

#ifdef USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#define IDN_MAX_LENGTH 255

ସ୍ଥିର ଅକ୍ଷର *idn_curlx_convert_wchar_to_UTF8(ଅଚଳ wchar_t *str_w, ପୂର୍ଣ୍ଣସଂଖ୍ୟା chars)
{
  ଅକ୍ଷର *str_utf8 = ଖାଲି;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, chars, ଖାଲି, 0,
                                  ଖାଲି, ଖାଲି);
  ଯଦି(bytes > 0) {
    str_utf8 = curlx_malloc(bytes);
    ଯଦି(str_utf8) {
      ଯଦି(WideCharToMultiByte(CP_UTF8, 0, str_w, chars, str_utf8, bytes,
                             ଖାଲି, ଖାଲି) == 0) {
        curlx_free(str_utf8);
        ଫେରନ୍ତୁ ଖାଲି;
      }
    }
  }
  ଫେରନ୍ତୁ str_utf8;
}

ସ୍ଥିର CURLcode win32_idn_to_ascii(ଅଚଳ ଅକ୍ଷର *in, ଅକ୍ଷର **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_w_len;
  *out = ଖାଲି;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  ଯଦି(in_w_len) {
    wchar_t punycode[IDN_MAX_LENGTH];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା chars = IdnToAscii(0, in_w, in_w_len, punycode, IDN_MAX_LENGTH);
    ଯଦି(chars > 0) {
      *out = idn_curlx_convert_wchar_to_UTF8(punycode, chars);
      ଯଦି(!*out)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
  }
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode win32_ascii_to_idn(ଅଚଳ ଅକ୍ଷର *in, ଅକ୍ଷର **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_w_len;
  *out = ଖାଲି;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  ଯଦି(in_w_len) {
    WCHAR idn[IDN_MAX_LENGTH]; /* stores a UTF-16 string */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା chars = IdnToUnicode(0, in_w, in_w_len, idn, IDN_MAX_LENGTH);
    ଯଦି(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *out = idn_curlx_convert_wchar_to_UTF8(idn, chars);
      ଯଦି(!*out)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
  }
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;

  ଫେରନ୍ତୁ CURLE_OK;
}

#endif /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(ଅଚଳ ଅକ୍ଷର *hostname)
{
  ଯଦି(hostname) {
    ସଂରଚନା Curl_str s;
    s.str = hostname;
    s.len = strlen(hostname);
    ଫେରନ୍ତୁ Curl_is_ASCII_str(&s);
  }
  ଫେରନ୍ତୁ TRUE;
}

bool Curl_is_ASCII_str(ସଂରଚନା Curl_str *s)
{
  ଯଦି(s && s->len) {
    ଅଚଳ unsigned ଅକ୍ଷର *ch = (ଅଚଳ unsigned ଅକ୍ଷର *)s->str;
    size_t i;
    ପାଇଁ(i = 0; i < s->len; ++i) {
      ଯଦି(ch[i] & 0x80)
        ଫେରନ୍ତୁ FALSE;
    }
  }
  ଫେରନ୍ତୁ TRUE;
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
ସ୍ଥିର CURLcode idn_decode(ଅଚଳ ଅକ୍ଷର *ନିବେଶ, ଅକ୍ଷର **output)
{
  ଅକ୍ଷର *decoded = ଖାଲି;
  CURLcode result = CURLE_OK;
#ifdef USE_LIBIDN2
  ଯଦି(idn2_check_version(IDN2_VERSION)) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = IDN2_NFC_ନିବେଶ
#ଯଦି IDN2_VERSION_ସଂଖ୍ୟା >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#endif
      ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = IDN2_LOOKUP(ନିବେଶ, &decoded, flags);
    ଯଦି(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(ନିବେଶ, &decoded, IDN2_TRANSITIONAL);
    ଯଦି(rc != IDN2_OK)
      result = CURLE_URL_MALFORMAT;
  }
  ଅନ୍ୟଥା
    /* a too old libidn2 version */
    result = CURLE_NOT_BUILT_IN;
#ନଚେତଯଦି defined(USE_WIN32_IDN)
  result = win32_idn_to_ascii(ନିବେଶ, &decoded);
#ନଚେତଯଦି defined(USE_APPLE_IDN)
  result = mac_idn_to_ascii(ନିବେଶ, &decoded);
#endif
  ଯଦି(!result)
    *output = decoded;
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode idn_encode(ଅଚଳ ଅକ୍ଷର *puny, ଅକ୍ଷର **output)
{
  ଅକ୍ଷର *enc = ଖାଲି;
#ifdef USE_LIBIDN2
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = idn2_to_unicode_8z8z(puny, &enc, 0);
  ଯଦି(rc != IDNA_SUCCESS)
    ଫେରନ୍ତୁ rc == IDNA_MALLOC_ERROR ? CURLE_OUT_OF_MEMORY : CURLE_URL_MALFORMAT;
#ନଚେତଯଦି defined(USE_WIN32_IDN)
  CURLcode result = win32_ascii_to_idn(puny, &enc);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
#ନଚେତଯଦି defined(USE_APPLE_IDN)
  CURLcode result = mac_ascii_to_idn(puny, &enc);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
#endif
  *output = enc;
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_idn_decode(ଅଚଳ ଅକ୍ଷର *ନିବେଶ, ଅକ୍ଷର **output)
{
  ଅକ୍ଷର *d = ଖାଲି;
  CURLcode result = idn_decode(ନିବେଶ, &d);
#ifdef USE_LIBIDN2
  ଯଦି(!result) {
    ଅକ୍ଷର *c = curlx_strdup(d);
    idn2_free(d);
    ଯଦି(c)
      d = c;
    ଅନ୍ୟଥା
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  ଯଦି(!result) {
    ଯଦି(!d[0]) { /* ended up zero length, not acceptable */
      result = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    ଅନ୍ୟଥା
      *output = d;
  }
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_idn_encode(ଅଚଳ ଅକ୍ଷର *puny, ଅକ୍ଷର **output)
{
  ଅକ୍ଷର *d = ଖାଲି;
  CURLcode result = idn_encode(puny, &d);
#ifdef USE_LIBIDN2
  ଯଦି(!result) {
    ଅକ୍ଷର *c = curlx_strdup(d);
    idn2_free(d);
    ଯଦି(c)
      d = c;
    ଅନ୍ୟଥା
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  ଯଦି(!result)
    *output = d;
  ଫେରନ୍ତୁ result;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
ଶୂନ୍ୟ Curl_free_idnconverted_hostname(ସଂରଚନା hostname *host)
{
  curlx_safefree(host->encalloc);
}

#endif /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(ସଂରଚନା hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#ifdef USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  ଯଦି(!Curl_is_ASCII_name(host->name)) {
    ଅକ୍ଷର *decoded;
    CURLcode result = Curl_idn_decode(host->name, &decoded);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#endif
  ଫେରନ୍ତୁ CURLE_OK;
}
