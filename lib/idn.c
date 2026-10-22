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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "idn.h"

#ifdef USE_LIBIDN2
#ಅನ್ತರ್ಭಾವ <idn2.h>

#ಯದಿ defined(_WIN32) && defined(UNICODE)
#define IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((ಧ್ರುವ uint8_t *)name, (uint8_t **)host, flags)
#ಅನ್ಯಥಾ
#define IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((ಧ್ರುವ ವರ್ಣ *)(name), (ವರ್ಣ **)(host), flags)
#endif
#endif /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#ifdef USE_APPLE_IDN
#ಅನ್ತರ್ಭಾವ <unicode/uidna.h>
#ಅನ್ತರ್ಭಾವ <iconv.h>
#ಅನ್ತರ್ಭಾವ <langinfo.h>

#define MAX_HOST_LENGTH 512

ಸ್ಥಿರ CURLcode iconv_to_utf8(ಧ್ರುವ ವರ್ಣ *in, size_t inlen,
                              ವರ್ಣ **out, size_t *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  ಯದಿ(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    ವರ್ಣ *iconv_in = (ವರ್ಣ *)CURL_UNCONST(in);
    size_t iconv_inlen = inlen;
    size_t iconv_result = iconv(cd, &iconv_in, &iconv_inlen,
                                out, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    ಯದಿ(iconv_result == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      ಯದಿ(errno == ENOMEM)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;
    }

    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಅನ್ಯಥಾ {
    /* !checksrc! disable ERRNOVAR 1 */
    ಯದಿ(errno == ENOMEM)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
}

ಸ್ಥಿರ CURLcode mac_idn_to_ascii(ಧ್ರುವ ವರ್ಣ *in, ವರ್ಣ **out)
{
  size_t inlen = strlen(in);
  ಯದಿ(inlen < MAX_HOST_LENGTH) {
    ವರ್ಣ iconv_buffer[MAX_HOST_LENGTH] = { 0 };
    ವರ್ಣ *iconv_outptr = iconv_buffer;
    size_t iconv_outlen = ಪರಿಮಾಣ(iconv_buffer);
    CURLcode iconv_result = iconv_to_utf8(in, inlen,
                                          &iconv_outptr, &iconv_outlen);
    ಯದಿ(!iconv_result) {
      UErrorCode err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_ASCII, &err);
      ಯದಿ(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        ವರ್ಣ buffer[MAX_HOST_LENGTH] = { 0 };
        (ಶೂನ್ಯ)uidna_nameToASCII_UTF8(idna, iconv_buffer, (ಪೂರ್ಣಾಙ್ಕ)iconv_outlen,
                                     buffer, ಪರಿಮಾಣ(buffer) - 1, &info, &err);
        uidna_close(idna);
        ಯದಿ(!U_FAILURE(err) && !info.errors) {
          *out = curlx_strdup(buffer);
          ಯದಿ(*out)
            ಪ್ರತ್ಯಾಗಮ CURLE_OK;
          ಅನ್ಯಥಾ
            ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
        }
      }
    }
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ iconv_result;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;
}

ಸ್ಥಿರ CURLcode mac_ascii_to_idn(ಧ್ರುವ ವರ್ಣ *in, ವರ್ಣ **out)
{
  size_t inlen = strlen(in);
  ಯದಿ(inlen < MAX_HOST_LENGTH) {
    UErrorCode err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_UNICODE, &err);
    ಯದಿ(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      ವರ್ಣ buffer[MAX_HOST_LENGTH] = { 0 };
      (ಶೂನ್ಯ)uidna_nameToUnicodeUTF8(idna, in, -1, buffer,
                                    ಪರಿಮಾಣ(buffer) - 1, &info, &err);
      uidna_close(idna);
      ಯದಿ(!U_FAILURE(err)) {
        *out = curlx_strdup(buffer);
        ಯದಿ(*out)
          ಪ್ರತ್ಯಾಗಮ CURLE_OK;
        ಅನ್ಯಥಾ
          ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;
}
#endif

#ifdef USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#define IDN_MAX_LENGTH 255

ಸ್ಥಿರ ವರ್ಣ *idn_curlx_convert_wchar_to_UTF8(ಧ್ರುವ wchar_t *str_w, ಪೂರ್ಣಾಙ್ಕ chars)
{
  ವರ್ಣ *str_utf8 = ರಿಕ್ತ;
  ಪೂರ್ಣಾಙ್ಕ bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, chars, ರಿಕ್ತ, 0,
                                  ರಿಕ್ತ, ರಿಕ್ತ);
  ಯದಿ(bytes > 0) {
    str_utf8 = curlx_malloc(bytes);
    ಯದಿ(str_utf8) {
      ಯದಿ(WideCharToMultiByte(CP_UTF8, 0, str_w, chars, str_utf8, bytes,
                             ರಿಕ್ತ, ರಿಕ್ತ) == 0) {
        curlx_free(str_utf8);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ str_utf8;
}

ಸ್ಥಿರ CURLcode win32_idn_to_ascii(ಧ್ರುವ ವರ್ಣ *in, ವರ್ಣ **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  ಪೂರ್ಣಾಙ್ಕ in_w_len;
  *out = ರಿಕ್ತ;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  ಯದಿ(in_w_len) {
    wchar_t punycode[IDN_MAX_LENGTH];
    ಪೂರ್ಣಾಙ್ಕ chars = IdnToAscii(0, in_w, in_w_len, punycode, IDN_MAX_LENGTH);
    ಯದಿ(chars > 0) {
      *out = idn_curlx_convert_wchar_to_UTF8(punycode, chars);
      ಯದಿ(!*out)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;
  }
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode win32_ascii_to_idn(ಧ್ರುವ ವರ್ಣ *in, ವರ್ಣ **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  ಪೂರ್ಣಾಙ್ಕ in_w_len;
  *out = ರಿಕ್ತ;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  ಯದಿ(in_w_len) {
    WCHAR idn[IDN_MAX_LENGTH]; /* stores a UTF-16 string */
    ಪೂರ್ಣಾಙ್ಕ chars = IdnToUnicode(0, in_w, in_w_len, idn, IDN_MAX_LENGTH);
    ಯದಿ(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *out = idn_curlx_convert_wchar_to_UTF8(idn, chars);
      ಯದಿ(!*out)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;
  }
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#endif /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(ಧ್ರುವ ವರ್ಣ *hostname)
{
  ಯದಿ(hostname) {
    ಸಂರಚನಾ Curl_str s;
    s.str = hostname;
    s.len = strlen(hostname);
    ಪ್ರತ್ಯಾಗಮ Curl_is_ASCII_str(&s);
  }
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

bool Curl_is_ASCII_str(ಸಂರಚನಾ Curl_str *s)
{
  ಯದಿ(s && s->len) {
    ಧ್ರುವ unsigned ವರ್ಣ *ch = (ಧ್ರುವ unsigned ವರ್ಣ *)s->str;
    size_t i;
    ಪ್ರತಿ(i = 0; i < s->len; ++i) {
      ಯದಿ(ch[i] & 0x80)
        ಪ್ರತ್ಯಾಗಮ FALSE;
    }
  }
  ಪ್ರತ್ಯಾಗಮ TRUE;
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
ಸ್ಥಿರ CURLcode idn_decode(ಧ್ರುವ ವರ್ಣ *ಒಳಹರಿವು, ವರ್ಣ **output)
{
  ವರ್ಣ *decoded = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;
#ifdef USE_LIBIDN2
  ಯದಿ(idn2_check_version(IDN2_VERSION)) {
    ಪೂರ್ಣಾಙ್ಕ flags = IDN2_NFC_ಒಳಹರಿವು
#ಯದಿ IDN2_VERSION_ಸಂಖ್ಯೆ >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#endif
      ;
    ಪೂರ್ಣಾಙ್ಕ rc = IDN2_LOOKUP(ಒಳಹರಿವು, &decoded, flags);
    ಯದಿ(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(ಒಳಹರಿವು, &decoded, IDN2_TRANSITIONAL);
    ಯದಿ(rc != IDN2_OK)
      result = CURLE_URL_MALFORMAT;
  }
  ಅನ್ಯಥಾ
    /* a too old libidn2 version */
    result = CURLE_NOT_BUILT_IN;
#ನೋಚೇದ್ಯದಿ defined(USE_WIN32_IDN)
  result = win32_idn_to_ascii(ಒಳಹರಿವು, &decoded);
#ನೋಚೇದ್ಯದಿ defined(USE_APPLE_IDN)
  result = mac_idn_to_ascii(ಒಳಹರಿವು, &decoded);
#endif
  ಯದಿ(!result)
    *output = decoded;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode idn_encode(ಧ್ರುವ ವರ್ಣ *puny, ವರ್ಣ **output)
{
  ವರ್ಣ *enc = ರಿಕ್ತ;
#ifdef USE_LIBIDN2
  ಪೂರ್ಣಾಙ್ಕ rc = idn2_to_unicode_8z8z(puny, &enc, 0);
  ಯದಿ(rc != IDNA_SUCCESS)
    ಪ್ರತ್ಯಾಗಮ rc == IDNA_MALLOC_ERROR ? CURLE_OUT_OF_MEMORY : CURLE_URL_MALFORMAT;
#ನೋಚೇದ್ಯದಿ defined(USE_WIN32_IDN)
  CURLcode result = win32_ascii_to_idn(puny, &enc);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
#ನೋಚೇದ್ಯದಿ defined(USE_APPLE_IDN)
  CURLcode result = mac_ascii_to_idn(puny, &enc);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
#endif
  *output = enc;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_idn_decode(ಧ್ರುವ ವರ್ಣ *ಒಳಹರಿವು, ವರ್ಣ **output)
{
  ವರ್ಣ *d = ರಿಕ್ತ;
  CURLcode result = idn_decode(ಒಳಹರಿವು, &d);
#ifdef USE_LIBIDN2
  ಯದಿ(!result) {
    ವರ್ಣ *c = curlx_strdup(d);
    idn2_free(d);
    ಯದಿ(c)
      d = c;
    ಅನ್ಯಥಾ
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  ಯದಿ(!result) {
    ಯದಿ(!d[0]) { /* ended up zero length, not acceptable */
      result = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    ಅನ್ಯಥಾ
      *output = d;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_idn_encode(ಧ್ರುವ ವರ್ಣ *puny, ವರ್ಣ **output)
{
  ವರ್ಣ *d = ರಿಕ್ತ;
  CURLcode result = idn_encode(puny, &d);
#ifdef USE_LIBIDN2
  ಯದಿ(!result) {
    ವರ್ಣ *c = curlx_strdup(d);
    idn2_free(d);
    ಯದಿ(c)
      d = c;
    ಅನ್ಯಥಾ
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  ಯದಿ(!result)
    *output = d;
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
ಶೂನ್ಯ Curl_free_idnconverted_hostname(ಸಂರಚನಾ hostname *host)
{
  curlx_safefree(host->encalloc);
}

#endif /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(ಸಂರಚನಾ hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#ifdef USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  ಯದಿ(!Curl_is_ASCII_name(host->name)) {
    ವರ್ಣ *decoded;
    CURLcode result = Curl_idn_decode(host->name, &decoded);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#endif
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
