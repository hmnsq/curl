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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "idn.h"

#ifdef USE_LIBIDN2
#অন্তর্ভাব <idn2.h>

#যদি defined(_WIN32) && defined(UNICODE)
#define IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((ধ্রুবক uint8_t *)name, (uint8_t **)host, flags)
#অন্যথা
#define IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((ধ্রুবক অক্ষর *)(name), (অক্ষর **)(host), flags)
#endif
#endif /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#ifdef USE_APPLE_IDN
#অন্তর্ভাব <unicode/uidna.h>
#অন্তর্ভাব <iconv.h>
#অন্তর্ভাব <langinfo.h>

#define MAX_HOST_দৈর্ঘ্য 512

স্থির CURLcode iconv_to_utf8(ধ্রুবক অক্ষর *in, size_t inlen,
                              অক্ষর **out, size_t *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  যদি(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    অক্ষর *iconv_in = (অক্ষর *)CURL_UNCONST(in);
    size_t iconv_inlen = inlen;
    size_t iconv_ফলাফল = iconv(cd, &iconv_in, &iconv_inlen,
                                out, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    যদি(iconv_ফলাফল == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      যদি(errno == ENOMEM)
        ফেরত CURLE_OUT_OF_MEMORY;
      অন্যথা
        ফেরত CURLE_URL_MALFORMAT;
    }

    ফেরত CURLE_OK;
  }
  অন্যথা {
    /* !checksrc! disable ERRNOVAR 1 */
    যদি(errno == ENOMEM)
      ফেরত CURLE_OUT_OF_MEMORY;
    অন্যথা
      ফেরত CURLE_FAILED_INIT;
  }
}

স্থির CURLcode mac_idn_to_ascii(ধ্রুবক অক্ষর *in, অক্ষর **out)
{
  size_t inlen = strlen(in);
  যদি(inlen < MAX_HOST_দৈর্ঘ্য) {
    অক্ষর iconv_বাফার[MAX_HOST_দৈর্ঘ্য] = { 0 };
    অক্ষর *iconv_outptr = iconv_বাফার;
    size_t iconv_outlen = আকার(iconv_বাফার);
    CURLcode iconv_ফলাফল = iconv_to_utf8(in, inlen,
                                          &iconv_outptr, &iconv_outlen);
    যদি(!iconv_ফলাফল) {
      UErrorCode err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_ASCII, &err);
      যদি(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        অক্ষর বাফার[MAX_HOST_দৈর্ঘ্য] = { 0 };
        (শূন্য)uidna_nameToASCII_UTF8(idna, iconv_বাফার, (পূর্ণসংখ্যা)iconv_outlen,
                                     বাফার, আকার(বাফার) - 1, &info, &err);
        uidna_close(idna);
        যদি(!U_FAILURE(err) && !info.errors) {
          *out = curlx_strdup(বাফার);
          যদি(*out)
            ফেরত CURLE_OK;
          অন্যথা
            ফেরত CURLE_OUT_OF_MEMORY;
        }
      }
    }
    অন্যথা
      ফেরত iconv_ফলাফল;
  }
  ফেরত CURLE_URL_MALFORMAT;
}

স্থির CURLcode mac_ascii_to_idn(ধ্রুবক অক্ষর *in, অক্ষর **out)
{
  size_t inlen = strlen(in);
  যদি(inlen < MAX_HOST_দৈর্ঘ্য) {
    UErrorCode err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_UNICODE, &err);
    যদি(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      অক্ষর বাফার[MAX_HOST_দৈর্ঘ্য] = { 0 };
      (শূন্য)uidna_nameToUnicodeUTF8(idna, in, -1, বাফার,
                                    আকার(বাফার) - 1, &info, &err);
      uidna_close(idna);
      যদি(!U_FAILURE(err)) {
        *out = curlx_strdup(বাফার);
        যদি(*out)
          ফেরত CURLE_OK;
        অন্যথা
          ফেরত CURLE_OUT_OF_MEMORY;
      }
    }
  }
  ফেরত CURLE_URL_MALFORMAT;
}
#endif

#ifdef USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#define IDN_MAX_দৈর্ঘ্য 255

স্থির অক্ষর *idn_curlx_convert_wchar_to_UTF8(ধ্রুবক wchar_t *str_w, পূর্ণসংখ্যা chars)
{
  অক্ষর *str_utf8 = ফাঁকা;
  পূর্ণসংখ্যা bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, chars, ফাঁকা, 0,
                                  ফাঁকা, ফাঁকা);
  যদি(bytes > 0) {
    str_utf8 = curlx_malloc(bytes);
    যদি(str_utf8) {
      যদি(WideCharToMultiByte(CP_UTF8, 0, str_w, chars, str_utf8, bytes,
                             ফাঁকা, ফাঁকা) == 0) {
        curlx_free(str_utf8);
        ফেরত ফাঁকা;
      }
    }
  }
  ফেরত str_utf8;
}

স্থির CURLcode win32_idn_to_ascii(ধ্রুবক অক্ষর *in, অক্ষর **out)
{
  wchar_t in_w[IDN_MAX_দৈর্ঘ্য];
  পূর্ণসংখ্যা in_w_len;
  *out = ফাঁকা;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_দৈর্ঘ্য);
  যদি(in_w_len) {
    wchar_t punycode[IDN_MAX_দৈর্ঘ্য];
    পূর্ণসংখ্যা chars = IdnToAscii(0, in_w, in_w_len, punycode, IDN_MAX_দৈর্ঘ্য);
    যদি(chars > 0) {
      *out = idn_curlx_convert_wchar_to_UTF8(punycode, chars);
      যদি(!*out)
        ফেরত CURLE_OUT_OF_MEMORY;
    }
    অন্যথা
      ফেরত CURLE_URL_MALFORMAT;
  }
  অন্যথা
    ফেরত CURLE_URL_MALFORMAT;

  ফেরত CURLE_OK;
}

স্থির CURLcode win32_ascii_to_idn(ধ্রুবক অক্ষর *in, অক্ষর **out)
{
  wchar_t in_w[IDN_MAX_দৈর্ঘ্য];
  পূর্ণসংখ্যা in_w_len;
  *out = ফাঁকা;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_দৈর্ঘ্য);
  যদি(in_w_len) {
    WCHAR idn[IDN_MAX_দৈর্ঘ্য]; /* stores a UTF-16 string */
    পূর্ণসংখ্যা chars = IdnToUnicode(0, in_w, in_w_len, idn, IDN_MAX_দৈর্ঘ্য);
    যদি(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *out = idn_curlx_convert_wchar_to_UTF8(idn, chars);
      যদি(!*out)
        ফেরত CURLE_OUT_OF_MEMORY;
    }
    অন্যথা
      ফেরত CURLE_URL_MALFORMAT;
  }
  অন্যথা
    ফেরত CURLE_URL_MALFORMAT;

  ফেরত CURLE_OK;
}

#endif /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(ধ্রুবক অক্ষর *hostname)
{
  যদি(hostname) {
    কাঠামো Curl_str s;
    s.str = hostname;
    s.len = strlen(hostname);
    ফেরত Curl_is_ASCII_str(&s);
  }
  ফেরত TRUE;
}

bool Curl_is_ASCII_str(কাঠামো Curl_str *s)
{
  যদি(s && s->len) {
    ধ্রুবক unsigned অক্ষর *ch = (ধ্রুবক unsigned অক্ষর *)s->str;
    size_t i;
    জন্য(i = 0; i < s->len; ++i) {
      যদি(ch[i] & 0x80)
        ফেরত FALSE;
    }
  }
  ফেরত TRUE;
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
স্থির CURLcode idn_decode(ধ্রুবক অক্ষর *প্রবেশ, অক্ষর **output)
{
  অক্ষর *decoded = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;
#ifdef USE_LIBIDN2
  যদি(idn2_check_version(IDN2_VERSION)) {
    পূর্ণসংখ্যা flags = IDN2_NFC_প্রবেশ
#যদি IDN2_VERSION_সংখ্যা >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#endif
      ;
    পূর্ণসংখ্যা rc = IDN2_LOOKUP(প্রবেশ, &decoded, flags);
    যদি(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(প্রবেশ, &decoded, IDN2_TRANSITIONAL);
    যদি(rc != IDN2_OK)
      ফলাফল = CURLE_URL_MALFORMAT;
  }
  অন্যথা
    /* a too old libidn2 version */
    ফলাফল = CURLE_NOT_BUILT_IN;
#নতুবা_যদি defined(USE_WIN32_IDN)
  ফলাফল = win32_idn_to_ascii(প্রবেশ, &decoded);
#নতুবা_যদি defined(USE_APPLE_IDN)
  ফলাফল = mac_idn_to_ascii(প্রবেশ, &decoded);
#endif
  যদি(!ফলাফল)
    *output = decoded;
  ফেরত ফলাফল;
}

স্থির CURLcode idn_encode(ধ্রুবক অক্ষর *puny, অক্ষর **output)
{
  অক্ষর *enc = ফাঁকা;
#ifdef USE_LIBIDN2
  পূর্ণসংখ্যা rc = idn2_to_unicode_8z8z(puny, &enc, 0);
  যদি(rc != IDNA_SUCCESS)
    ফেরত rc == IDNA_MALLOC_ERROR ? CURLE_OUT_OF_MEMORY : CURLE_URL_MALFORMAT;
#নতুবা_যদি defined(USE_WIN32_IDN)
  CURLcode ফলাফল = win32_ascii_to_idn(puny, &enc);
  যদি(ফলাফল)
    ফেরত ফলাফল;
#নতুবা_যদি defined(USE_APPLE_IDN)
  CURLcode ফলাফল = mac_ascii_to_idn(puny, &enc);
  যদি(ফলাফল)
    ফেরত ফলাফল;
#endif
  *output = enc;
  ফেরত CURLE_OK;
}

CURLcode Curl_idn_decode(ধ্রুবক অক্ষর *প্রবেশ, অক্ষর **output)
{
  অক্ষর *d = ফাঁকা;
  CURLcode ফলাফল = idn_decode(প্রবেশ, &d);
#ifdef USE_LIBIDN2
  যদি(!ফলাফল) {
    অক্ষর *c = curlx_strdup(d);
    idn2_free(d);
    যদি(c)
      d = c;
    অন্যথা
      ফলাফল = CURLE_OUT_OF_MEMORY;
  }
#endif
  যদি(!ফলাফল) {
    যদি(!d[0]) { /* ended up zero length, not acceptable */
      ফলাফল = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    অন্যথা
      *output = d;
  }
  ফেরত ফলাফল;
}

CURLcode Curl_idn_encode(ধ্রুবক অক্ষর *puny, অক্ষর **output)
{
  অক্ষর *d = ফাঁকা;
  CURLcode ফলাফল = idn_encode(puny, &d);
#ifdef USE_LIBIDN2
  যদি(!ফলাফল) {
    অক্ষর *c = curlx_strdup(d);
    idn2_free(d);
    যদি(c)
      d = c;
    অন্যথা
      ফলাফল = CURLE_OUT_OF_MEMORY;
  }
#endif
  যদি(!ফলাফল)
    *output = d;
  ফেরত ফলাফল;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
শূন্য Curl_free_idnconverted_hostname(কাঠামো hostname *host)
{
  curlx_safefree(host->encalloc);
}

#endif /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(কাঠামো hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#ifdef USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  যদি(!Curl_is_ASCII_name(host->name)) {
    অক্ষর *decoded;
    CURLcode ফলাফল = Curl_idn_decode(host->name, &decoded);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#endif
  ফেরত CURLE_OK;
}
