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
#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "curlx/strparse.h"
#شامل "idn.h"

#ifdef USE_LIBIDN2
#شامل <idn2.h>

#اگر defined(_WIN32) && defined(UNICODE)
#define IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((ثابت uint8_t *)name, (uint8_t **)host, flags)
#وگرنه
#define IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((ثابت نویسه *)(name), (نویسه **)(host), flags)
#endif
#endif /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#ifdef USE_APPLE_IDN
#شامل <unicode/uidna.h>
#شامل <iconv.h>
#شامل <langinfo.h>

#define MAX_HOST_LENGTH 512

ایستا CURLcode iconv_to_utf8(ثابت نویسه *in, size_t inlen,
                              نویسه **out, size_t *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  اگر(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    نویسه *iconv_in = (نویسه *)CURL_UNCONST(in);
    size_t iconv_inlen = inlen;
    size_t iconv_result = iconv(cd, &iconv_in, &iconv_inlen,
                                out, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    اگر(iconv_result == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      اگر(errno == ENOMEM)
        بازگشت CURLE_OUT_OF_MEMORY;
      وگرنه
        بازگشت CURLE_URL_MALFORMAT;
    }

    بازگشت CURLE_OK;
  }
  وگرنه {
    /* !checksrc! disable ERRNOVAR 1 */
    اگر(errno == ENOMEM)
      بازگشت CURLE_OUT_OF_MEMORY;
    وگرنه
      بازگشت CURLE_FAILED_INIT;
  }
}

ایستا CURLcode mac_idn_to_ascii(ثابت نویسه *in, نویسه **out)
{
  size_t inlen = strlen(in);
  اگر(inlen < MAX_HOST_LENGTH) {
    نویسه iconv_buffer[MAX_HOST_LENGTH] = { 0 };
    نویسه *iconv_outptr = iconv_buffer;
    size_t iconv_outlen = اندازه(iconv_buffer);
    CURLcode iconv_result = iconv_to_utf8(in, inlen,
                                          &iconv_outptr, &iconv_outlen);
    اگر(!iconv_result) {
      UErrorCode err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_ASCII, &err);
      اگر(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        نویسه buffer[MAX_HOST_LENGTH] = { 0 };
        (تهی)uidna_nameToASCII_UTF8(idna, iconv_buffer, (صحیح)iconv_outlen,
                                     buffer, اندازه(buffer) - 1, &info, &err);
        uidna_close(idna);
        اگر(!U_FAILURE(err) && !info.errors) {
          *out = curlx_strdup(buffer);
          اگر(*out)
            بازگشت CURLE_OK;
          وگرنه
            بازگشت CURLE_OUT_OF_MEMORY;
        }
      }
    }
    وگرنه
      بازگشت iconv_result;
  }
  بازگشت CURLE_URL_MALFORMAT;
}

ایستا CURLcode mac_ascii_to_idn(ثابت نویسه *in, نویسه **out)
{
  size_t inlen = strlen(in);
  اگر(inlen < MAX_HOST_LENGTH) {
    UErrorCode err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_UNICODE, &err);
    اگر(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      نویسه buffer[MAX_HOST_LENGTH] = { 0 };
      (تهی)uidna_nameToUnicodeUTF8(idna, in, -1, buffer,
                                    اندازه(buffer) - 1, &info, &err);
      uidna_close(idna);
      اگر(!U_FAILURE(err)) {
        *out = curlx_strdup(buffer);
        اگر(*out)
          بازگشت CURLE_OK;
        وگرنه
          بازگشت CURLE_OUT_OF_MEMORY;
      }
    }
  }
  بازگشت CURLE_URL_MALFORMAT;
}
#endif

#ifdef USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#define IDN_MAX_LENGTH 255

ایستا نویسه *idn_curlx_convert_wchar_to_UTF8(ثابت wchar_t *str_w, صحیح chars)
{
  نویسه *str_utf8 = پوچ;
  صحیح bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, chars, پوچ, 0,
                                  پوچ, پوچ);
  اگر(bytes > 0) {
    str_utf8 = curlx_malloc(bytes);
    اگر(str_utf8) {
      اگر(WideCharToMultiByte(CP_UTF8, 0, str_w, chars, str_utf8, bytes,
                             پوچ, پوچ) == 0) {
        curlx_free(str_utf8);
        بازگشت پوچ;
      }
    }
  }
  بازگشت str_utf8;
}

ایستا CURLcode win32_idn_to_ascii(ثابت نویسه *in, نویسه **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  صحیح in_w_len;
  *out = پوچ;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  اگر(in_w_len) {
    wchar_t punycode[IDN_MAX_LENGTH];
    صحیح chars = IdnToAscii(0, in_w, in_w_len, punycode, IDN_MAX_LENGTH);
    اگر(chars > 0) {
      *out = idn_curlx_convert_wchar_to_UTF8(punycode, chars);
      اگر(!*out)
        بازگشت CURLE_OUT_OF_MEMORY;
    }
    وگرنه
      بازگشت CURLE_URL_MALFORMAT;
  }
  وگرنه
    بازگشت CURLE_URL_MALFORMAT;

  بازگشت CURLE_OK;
}

ایستا CURLcode win32_ascii_to_idn(ثابت نویسه *in, نویسه **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  صحیح in_w_len;
  *out = پوچ;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  اگر(in_w_len) {
    WCHAR idn[IDN_MAX_LENGTH]; /* stores a UTF-16 string */
    صحیح chars = IdnToUnicode(0, in_w, in_w_len, idn, IDN_MAX_LENGTH);
    اگر(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *out = idn_curlx_convert_wchar_to_UTF8(idn, chars);
      اگر(!*out)
        بازگشت CURLE_OUT_OF_MEMORY;
    }
    وگرنه
      بازگشت CURLE_URL_MALFORMAT;
  }
  وگرنه
    بازگشت CURLE_URL_MALFORMAT;

  بازگشت CURLE_OK;
}

#endif /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(ثابت نویسه *hostname)
{
  اگر(hostname) {
    ساختار Curl_str s;
    s.str = hostname;
    s.len = strlen(hostname);
    بازگشت Curl_is_ASCII_str(&s);
  }
  بازگشت TRUE;
}

bool Curl_is_ASCII_str(ساختار Curl_str *s)
{
  اگر(s && s->len) {
    ثابت unsigned نویسه *ch = (ثابت unsigned نویسه *)s->str;
    size_t i;
    برای(i = 0; i < s->len; ++i) {
      اگر(ch[i] & 0x80)
        بازگشت FALSE;
    }
  }
  بازگشت TRUE;
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
ایستا CURLcode idn_decode(ثابت نویسه *درون_داد, نویسه **output)
{
  نویسه *decoded = پوچ;
  CURLcode result = CURLE_OK;
#ifdef USE_LIBIDN2
  اگر(idn2_check_version(IDN2_VERSION)) {
    صحیح flags = IDN2_NFC_درون_داد
#اگر IDN2_VERSION_عدد >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#endif
      ;
    صحیح rc = IDN2_LOOKUP(درون_داد, &decoded, flags);
    اگر(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(درون_داد, &decoded, IDN2_TRANSITIONAL);
    اگر(rc != IDN2_OK)
      result = CURLE_URL_MALFORMAT;
  }
  وگرنه
    /* a too old libidn2 version */
    result = CURLE_NOT_BUILT_IN;
#وگرنهاگر defined(USE_WIN32_IDN)
  result = win32_idn_to_ascii(درون_داد, &decoded);
#وگرنهاگر defined(USE_APPLE_IDN)
  result = mac_idn_to_ascii(درون_داد, &decoded);
#endif
  اگر(!result)
    *output = decoded;
  بازگشت result;
}

ایستا CURLcode idn_encode(ثابت نویسه *puny, نویسه **output)
{
  نویسه *enc = پوچ;
#ifdef USE_LIBIDN2
  صحیح rc = idn2_to_unicode_8z8z(puny, &enc, 0);
  اگر(rc != IDNA_SUCCESS)
    بازگشت rc == IDNA_MALLOC_ERROR ? CURLE_OUT_OF_MEMORY : CURLE_URL_MALFORMAT;
#وگرنهاگر defined(USE_WIN32_IDN)
  CURLcode result = win32_ascii_to_idn(puny, &enc);
  اگر(result)
    بازگشت result;
#وگرنهاگر defined(USE_APPLE_IDN)
  CURLcode result = mac_ascii_to_idn(puny, &enc);
  اگر(result)
    بازگشت result;
#endif
  *output = enc;
  بازگشت CURLE_OK;
}

CURLcode Curl_idn_decode(ثابت نویسه *درون_داد, نویسه **output)
{
  نویسه *d = پوچ;
  CURLcode result = idn_decode(درون_داد, &d);
#ifdef USE_LIBIDN2
  اگر(!result) {
    نویسه *c = curlx_strdup(d);
    idn2_free(d);
    اگر(c)
      d = c;
    وگرنه
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  اگر(!result) {
    اگر(!d[0]) { /* ended up zero length, not acceptable */
      result = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    وگرنه
      *output = d;
  }
  بازگشت result;
}

CURLcode Curl_idn_encode(ثابت نویسه *puny, نویسه **output)
{
  نویسه *d = پوچ;
  CURLcode result = idn_encode(puny, &d);
#ifdef USE_LIBIDN2
  اگر(!result) {
    نویسه *c = curlx_strdup(d);
    idn2_free(d);
    اگر(c)
      d = c;
    وگرنه
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  اگر(!result)
    *output = d;
  بازگشت result;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
تهی Curl_free_idnconverted_hostname(ساختار hostname *host)
{
  curlx_safefree(host->encalloc);
}

#endif /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(ساختار hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#ifdef USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  اگر(!Curl_is_ASCII_name(host->name)) {
    نویسه *decoded;
    CURLcode result = Curl_idn_decode(host->name, &decoded);
    اگر(result)
      بازگشت result;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#endif
  بازگشت CURLE_OK;
}
