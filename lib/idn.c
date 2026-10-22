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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "curlx/strparse.h"
#অন্তর্ভাৱ "idn.h"

#ifdef USE_LIBIDN2
#অন্তর্ভাৱ <idn2.h>

#যদি defined(_WIN32) && defined(UNICODE)
#define IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((ধ্রুৱ uint8_t *)name, (uint8_t **)host, flags)
#অন্যথা
#define IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((ধ্রুৱ ৱর্ণ *)(name), (ৱর্ণ **)(host), flags)
#endif
#endif /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#ifdef USE_APPLE_IDN
#অন্তর্ভাৱ <unicode/uidna.h>
#অন্তর্ভাৱ <iconv.h>
#অন্তর্ভাৱ <langinfo.h>

#define MAX_HOST_LENGTH 512

স্থির CURLcode iconv_to_utf8(ধ্রুৱ ৱর্ণ *in, size_t inlen,
                              ৱর্ণ **out, size_t *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  যদি(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    ৱর্ণ *iconv_in = (ৱর্ণ *)CURL_UNCONST(in);
    size_t iconv_inlen = inlen;
    size_t iconv_result = iconv(cd, &iconv_in, &iconv_inlen,
                                out, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    যদি(iconv_result == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      যদি(errno == ENOMEM)
        ঘূৰাই CURLE_OUT_OF_MEMORY;
      অন্যথা
        ঘূৰাই CURLE_URL_MALFORMAT;
    }

    ঘূৰাই CURLE_OK;
  }
  অন্যথা {
    /* !checksrc! disable ERRNOVAR 1 */
    যদি(errno == ENOMEM)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    অন্যথা
      ঘূৰাই CURLE_FAILED_INIT;
  }
}

স্থির CURLcode mac_idn_to_ascii(ধ্রুৱ ৱর্ণ *in, ৱর্ণ **out)
{
  size_t inlen = strlen(in);
  যদি(inlen < MAX_HOST_LENGTH) {
    ৱর্ণ iconv_buffer[MAX_HOST_LENGTH] = { 0 };
    ৱর্ণ *iconv_outptr = iconv_buffer;
    size_t iconv_outlen = পরিমাণ(iconv_buffer);
    CURLcode iconv_result = iconv_to_utf8(in, inlen,
                                          &iconv_outptr, &iconv_outlen);
    যদি(!iconv_result) {
      UErrorCode err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_ASCII, &err);
      যদি(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        ৱর্ণ buffer[MAX_HOST_LENGTH] = { 0 };
        (শূন্য)uidna_nameToASCII_UTF8(idna, iconv_buffer, (পূর্ণাঙ্ক)iconv_outlen,
                                     buffer, পরিমাণ(buffer) - 1, &info, &err);
        uidna_close(idna);
        যদি(!U_FAILURE(err) && !info.errors) {
          *out = curlx_strdup(buffer);
          যদি(*out)
            ঘূৰাই CURLE_OK;
          অন্যথা
            ঘূৰাই CURLE_OUT_OF_MEMORY;
        }
      }
    }
    অন্যথা
      ঘূৰাই iconv_result;
  }
  ঘূৰাই CURLE_URL_MALFORMAT;
}

স্থির CURLcode mac_ascii_to_idn(ধ্রুৱ ৱর্ণ *in, ৱর্ণ **out)
{
  size_t inlen = strlen(in);
  যদি(inlen < MAX_HOST_LENGTH) {
    UErrorCode err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_UNICODE, &err);
    যদি(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      ৱর্ণ buffer[MAX_HOST_LENGTH] = { 0 };
      (শূন্য)uidna_nameToUnicodeUTF8(idna, in, -1, buffer,
                                    পরিমাণ(buffer) - 1, &info, &err);
      uidna_close(idna);
      যদি(!U_FAILURE(err)) {
        *out = curlx_strdup(buffer);
        যদি(*out)
          ঘূৰাই CURLE_OK;
        অন্যথা
          ঘূৰাই CURLE_OUT_OF_MEMORY;
      }
    }
  }
  ঘূৰাই CURLE_URL_MALFORMAT;
}
#endif

#ifdef USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#define IDN_MAX_LENGTH 255

স্থির ৱর্ণ *idn_curlx_convert_wchar_to_UTF8(ধ্রুৱ wchar_t *str_w, পূর্ণাঙ্ক chars)
{
  ৱর্ণ *str_utf8 = রিক্ত;
  পূর্ণাঙ্ক bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, chars, রিক্ত, 0,
                                  রিক্ত, রিক্ত);
  যদি(bytes > 0) {
    str_utf8 = curlx_malloc(bytes);
    যদি(str_utf8) {
      যদি(WideCharToMultiByte(CP_UTF8, 0, str_w, chars, str_utf8, bytes,
                             রিক্ত, রিক্ত) == 0) {
        curlx_free(str_utf8);
        ঘূৰাই রিক্ত;
      }
    }
  }
  ঘূৰাই str_utf8;
}

স্থির CURLcode win32_idn_to_ascii(ধ্রুৱ ৱর্ণ *in, ৱর্ণ **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  পূর্ণাঙ্ক in_w_len;
  *out = রিক্ত;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  যদি(in_w_len) {
    wchar_t punycode[IDN_MAX_LENGTH];
    পূর্ণাঙ্ক chars = IdnToAscii(0, in_w, in_w_len, punycode, IDN_MAX_LENGTH);
    যদি(chars > 0) {
      *out = idn_curlx_convert_wchar_to_UTF8(punycode, chars);
      যদি(!*out)
        ঘূৰাই CURLE_OUT_OF_MEMORY;
    }
    অন্যথা
      ঘূৰাই CURLE_URL_MALFORMAT;
  }
  অন্যথা
    ঘূৰাই CURLE_URL_MALFORMAT;

  ঘূৰাই CURLE_OK;
}

স্থির CURLcode win32_ascii_to_idn(ধ্রুৱ ৱর্ণ *in, ৱর্ণ **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  পূর্ণাঙ্ক in_w_len;
  *out = রিক্ত;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  যদি(in_w_len) {
    WCHAR idn[IDN_MAX_LENGTH]; /* stores a UTF-16 string */
    পূর্ণাঙ্ক chars = IdnToUnicode(0, in_w, in_w_len, idn, IDN_MAX_LENGTH);
    যদি(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *out = idn_curlx_convert_wchar_to_UTF8(idn, chars);
      যদি(!*out)
        ঘূৰাই CURLE_OUT_OF_MEMORY;
    }
    অন্যথা
      ঘূৰাই CURLE_URL_MALFORMAT;
  }
  অন্যথা
    ঘূৰাই CURLE_URL_MALFORMAT;

  ঘূৰাই CURLE_OK;
}

#endif /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(ধ্রুৱ ৱর্ণ *hostname)
{
  যদি(hostname) {
    সংরচনা Curl_str s;
    s.str = hostname;
    s.len = strlen(hostname);
    ঘূৰাই Curl_is_ASCII_str(&s);
  }
  ঘূৰাই TRUE;
}

bool Curl_is_ASCII_str(সংরচনা Curl_str *s)
{
  যদি(s && s->len) {
    ধ্রুৱ unsigned ৱর্ণ *ch = (ধ্রুৱ unsigned ৱর্ণ *)s->str;
    size_t i;
    বাবে(i = 0; i < s->len; ++i) {
      যদি(ch[i] & 0x80)
        ঘূৰাই FALSE;
    }
  }
  ঘূৰাই TRUE;
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
স্থির CURLcode idn_decode(ধ্রুৱ ৱর্ণ *প্ৰবেশ, ৱর্ণ **output)
{
  ৱর্ণ *decoded = রিক্ত;
  CURLcode result = CURLE_OK;
#ifdef USE_LIBIDN2
  যদি(idn2_check_version(IDN2_VERSION)) {
    পূর্ণাঙ্ক flags = IDN2_NFC_প্ৰবেশ
#যদি IDN2_VERSION_সংখ্যা >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#endif
      ;
    পূর্ণাঙ্ক rc = IDN2_LOOKUP(প্ৰবেশ, &decoded, flags);
    যদি(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(প্ৰবেশ, &decoded, IDN2_TRANSITIONAL);
    যদি(rc != IDN2_OK)
      result = CURLE_URL_MALFORMAT;
  }
  অন্যথা
    /* a too old libidn2 version */
    result = CURLE_NOT_BUILT_IN;
#নোচেদ্যদি defined(USE_WIN32_IDN)
  result = win32_idn_to_ascii(প্ৰবেশ, &decoded);
#নোচেদ্যদি defined(USE_APPLE_IDN)
  result = mac_idn_to_ascii(প্ৰবেশ, &decoded);
#endif
  যদি(!result)
    *output = decoded;
  ঘূৰাই result;
}

স্থির CURLcode idn_encode(ধ্রুৱ ৱর্ণ *puny, ৱর্ণ **output)
{
  ৱর্ণ *enc = রিক্ত;
#ifdef USE_LIBIDN2
  পূর্ণাঙ্ক rc = idn2_to_unicode_8z8z(puny, &enc, 0);
  যদি(rc != IDNA_SUCCESS)
    ঘূৰাই rc == IDNA_MALLOC_ERROR ? CURLE_OUT_OF_MEMORY : CURLE_URL_MALFORMAT;
#নোচেদ্যদি defined(USE_WIN32_IDN)
  CURLcode result = win32_ascii_to_idn(puny, &enc);
  যদি(result)
    ঘূৰাই result;
#নোচেদ্যদি defined(USE_APPLE_IDN)
  CURLcode result = mac_ascii_to_idn(puny, &enc);
  যদি(result)
    ঘূৰাই result;
#endif
  *output = enc;
  ঘূৰাই CURLE_OK;
}

CURLcode Curl_idn_decode(ধ্রুৱ ৱর্ণ *প্ৰবেশ, ৱর্ণ **output)
{
  ৱর্ণ *d = রিক্ত;
  CURLcode result = idn_decode(প্ৰবেশ, &d);
#ifdef USE_LIBIDN2
  যদি(!result) {
    ৱর্ণ *c = curlx_strdup(d);
    idn2_free(d);
    যদি(c)
      d = c;
    অন্যথা
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  যদি(!result) {
    যদি(!d[0]) { /* ended up zero length, not acceptable */
      result = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    অন্যথা
      *output = d;
  }
  ঘূৰাই result;
}

CURLcode Curl_idn_encode(ধ্রুৱ ৱর্ণ *puny, ৱর্ণ **output)
{
  ৱর্ণ *d = রিক্ত;
  CURLcode result = idn_encode(puny, &d);
#ifdef USE_LIBIDN2
  যদি(!result) {
    ৱর্ণ *c = curlx_strdup(d);
    idn2_free(d);
    যদি(c)
      d = c;
    অন্যথা
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  যদি(!result)
    *output = d;
  ঘূৰাই result;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
শূন্য Curl_free_idnconverted_hostname(সংরচনা hostname *host)
{
  curlx_safefree(host->encalloc);
}

#endif /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(সংরচনা hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#ifdef USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  যদি(!Curl_is_ASCII_name(host->name)) {
    ৱর্ণ *decoded;
    CURLcode result = Curl_idn_decode(host->name, &decoded);
    যদি(result)
      ঘূৰাই result;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#endif
  ঘূৰাই CURLE_OK;
}
