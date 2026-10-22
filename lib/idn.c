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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "idn.h"

#ifdef USE_LIBIDN2
#அந்தர்பாவ <idn2.h>

#எனில் defined(_WIN32) && defined(UNICODE)
#define IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((மாறிலி uint8_t *)name, (uint8_t **)host, flags)
#இல்லையெனில்
#define IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((மாறிலி எழுத்து *)(name), (எழுத்து **)(host), flags)
#endif
#endif /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#ifdef USE_APPLE_IDN
#அந்தர்பாவ <unicode/uidna.h>
#அந்தர்பாவ <iconv.h>
#அந்தர்பாவ <langinfo.h>

#define MAX_HOST_LENGTH 512

நிலையான CURLcode iconv_to_utf8(மாறிலி எழுத்து *in, size_t inlen,
                              எழுத்து **out, size_t *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  எனில்(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    எழுத்து *iconv_in = (எழுத்து *)CURL_UNCONST(in);
    size_t iconv_inlen = inlen;
    size_t iconv_result = iconv(cd, &iconv_in, &iconv_inlen,
                                out, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    எனில்(iconv_result == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      எனில்(errno == ENOMEM)
        திரும்பு CURLE_OUT_OF_MEMORY;
      இல்லையெனில்
        திரும்பு CURLE_URL_MALFORMAT;
    }

    திரும்பு CURLE_OK;
  }
  இல்லையெனில் {
    /* !checksrc! disable ERRNOVAR 1 */
    எனில்(errno == ENOMEM)
      திரும்பு CURLE_OUT_OF_MEMORY;
    இல்லையெனில்
      திரும்பு CURLE_FAILED_INIT;
  }
}

நிலையான CURLcode mac_idn_to_ascii(மாறிலி எழுத்து *in, எழுத்து **out)
{
  size_t inlen = strlen(in);
  எனில்(inlen < MAX_HOST_LENGTH) {
    எழுத்து iconv_buffer[MAX_HOST_LENGTH] = { 0 };
    எழுத்து *iconv_outptr = iconv_buffer;
    size_t iconv_outlen = அளவு(iconv_buffer);
    CURLcode iconv_result = iconv_to_utf8(in, inlen,
                                          &iconv_outptr, &iconv_outlen);
    எனில்(!iconv_result) {
      UErrorCode err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_ASCII, &err);
      எனில்(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        எழுத்து buffer[MAX_HOST_LENGTH] = { 0 };
        (வெறுமை)uidna_nameToASCII_UTF8(idna, iconv_buffer, (முழு)iconv_outlen,
                                     buffer, அளவு(buffer) - 1, &info, &err);
        uidna_close(idna);
        எனில்(!U_FAILURE(err) && !info.errors) {
          *out = curlx_strdup(buffer);
          எனில்(*out)
            திரும்பு CURLE_OK;
          இல்லையெனில்
            திரும்பு CURLE_OUT_OF_MEMORY;
        }
      }
    }
    இல்லையெனில்
      திரும்பு iconv_result;
  }
  திரும்பு CURLE_URL_MALFORMAT;
}

நிலையான CURLcode mac_ascii_to_idn(மாறிலி எழுத்து *in, எழுத்து **out)
{
  size_t inlen = strlen(in);
  எனில்(inlen < MAX_HOST_LENGTH) {
    UErrorCode err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_UNICODE, &err);
    எனில்(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      எழுத்து buffer[MAX_HOST_LENGTH] = { 0 };
      (வெறுமை)uidna_nameToUnicodeUTF8(idna, in, -1, buffer,
                                    அளவு(buffer) - 1, &info, &err);
      uidna_close(idna);
      எனில்(!U_FAILURE(err)) {
        *out = curlx_strdup(buffer);
        எனில்(*out)
          திரும்பு CURLE_OK;
        இல்லையெனில்
          திரும்பு CURLE_OUT_OF_MEMORY;
      }
    }
  }
  திரும்பு CURLE_URL_MALFORMAT;
}
#endif

#ifdef USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#define IDN_MAX_LENGTH 255

நிலையான எழுத்து *idn_curlx_convert_wchar_to_UTF8(மாறிலி wchar_t *str_w, முழு chars)
{
  எழுத்து *str_utf8 = வெற்று;
  முழு bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, chars, வெற்று, 0,
                                  வெற்று, வெற்று);
  எனில்(bytes > 0) {
    str_utf8 = curlx_malloc(bytes);
    எனில்(str_utf8) {
      எனில்(WideCharToMultiByte(CP_UTF8, 0, str_w, chars, str_utf8, bytes,
                             வெற்று, வெற்று) == 0) {
        curlx_free(str_utf8);
        திரும்பு வெற்று;
      }
    }
  }
  திரும்பு str_utf8;
}

நிலையான CURLcode win32_idn_to_ascii(மாறிலி எழுத்து *in, எழுத்து **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  முழு in_w_len;
  *out = வெற்று;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  எனில்(in_w_len) {
    wchar_t punycode[IDN_MAX_LENGTH];
    முழு chars = IdnToAscii(0, in_w, in_w_len, punycode, IDN_MAX_LENGTH);
    எனில்(chars > 0) {
      *out = idn_curlx_convert_wchar_to_UTF8(punycode, chars);
      எனில்(!*out)
        திரும்பு CURLE_OUT_OF_MEMORY;
    }
    இல்லையெனில்
      திரும்பு CURLE_URL_MALFORMAT;
  }
  இல்லையெனில்
    திரும்பு CURLE_URL_MALFORMAT;

  திரும்பு CURLE_OK;
}

நிலையான CURLcode win32_ascii_to_idn(மாறிலி எழுத்து *in, எழுத்து **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  முழு in_w_len;
  *out = வெற்று;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  எனில்(in_w_len) {
    WCHAR idn[IDN_MAX_LENGTH]; /* stores a UTF-16 string */
    முழு chars = IdnToUnicode(0, in_w, in_w_len, idn, IDN_MAX_LENGTH);
    எனில்(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *out = idn_curlx_convert_wchar_to_UTF8(idn, chars);
      எனில்(!*out)
        திரும்பு CURLE_OUT_OF_MEMORY;
    }
    இல்லையெனில்
      திரும்பு CURLE_URL_MALFORMAT;
  }
  இல்லையெனில்
    திரும்பு CURLE_URL_MALFORMAT;

  திரும்பு CURLE_OK;
}

#endif /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(மாறிலி எழுத்து *hostname)
{
  எனில்(hostname) {
    கட்டமைப்பு Curl_str s;
    s.str = hostname;
    s.len = strlen(hostname);
    திரும்பு Curl_is_ASCII_str(&s);
  }
  திரும்பு TRUE;
}

bool Curl_is_ASCII_str(கட்டமைப்பு Curl_str *s)
{
  எனில்(s && s->len) {
    மாறிலி unsigned எழுத்து *ch = (மாறிலி unsigned எழுத்து *)s->str;
    size_t i;
    க்கு(i = 0; i < s->len; ++i) {
      எனில்(ch[i] & 0x80)
        திரும்பு FALSE;
    }
  }
  திரும்பு TRUE;
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
நிலையான CURLcode idn_decode(மாறிலி எழுத்து *உள்ளீடு, எழுத்து **output)
{
  எழுத்து *decoded = வெற்று;
  CURLcode result = CURLE_OK;
#ifdef USE_LIBIDN2
  எனில்(idn2_check_version(IDN2_VERSION)) {
    முழு flags = IDN2_NFC_உள்ளீடு
#எனில் IDN2_VERSION_எண் >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#endif
      ;
    முழு rc = IDN2_LOOKUP(உள்ளீடு, &decoded, flags);
    எனில்(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(உள்ளீடு, &decoded, IDN2_TRANSITIONAL);
    எனில்(rc != IDN2_OK)
      result = CURLE_URL_MALFORMAT;
  }
  இல்லையெனில்
    /* a too old libidn2 version */
    result = CURLE_NOT_BUILT_IN;
#அல்லதெனில் defined(USE_WIN32_IDN)
  result = win32_idn_to_ascii(உள்ளீடு, &decoded);
#அல்லதெனில் defined(USE_APPLE_IDN)
  result = mac_idn_to_ascii(உள்ளீடு, &decoded);
#endif
  எனில்(!result)
    *output = decoded;
  திரும்பு result;
}

நிலையான CURLcode idn_encode(மாறிலி எழுத்து *puny, எழுத்து **output)
{
  எழுத்து *enc = வெற்று;
#ifdef USE_LIBIDN2
  முழு rc = idn2_to_unicode_8z8z(puny, &enc, 0);
  எனில்(rc != IDNA_SUCCESS)
    திரும்பு rc == IDNA_MALLOC_ERROR ? CURLE_OUT_OF_MEMORY : CURLE_URL_MALFORMAT;
#அல்லதெனில் defined(USE_WIN32_IDN)
  CURLcode result = win32_ascii_to_idn(puny, &enc);
  எனில்(result)
    திரும்பு result;
#அல்லதெனில் defined(USE_APPLE_IDN)
  CURLcode result = mac_ascii_to_idn(puny, &enc);
  எனில்(result)
    திரும்பு result;
#endif
  *output = enc;
  திரும்பு CURLE_OK;
}

CURLcode Curl_idn_decode(மாறிலி எழுத்து *உள்ளீடு, எழுத்து **output)
{
  எழுத்து *d = வெற்று;
  CURLcode result = idn_decode(உள்ளீடு, &d);
#ifdef USE_LIBIDN2
  எனில்(!result) {
    எழுத்து *c = curlx_strdup(d);
    idn2_free(d);
    எனில்(c)
      d = c;
    இல்லையெனில்
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  எனில்(!result) {
    எனில்(!d[0]) { /* ended up zero length, not acceptable */
      result = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    இல்லையெனில்
      *output = d;
  }
  திரும்பு result;
}

CURLcode Curl_idn_encode(மாறிலி எழுத்து *puny, எழுத்து **output)
{
  எழுத்து *d = வெற்று;
  CURLcode result = idn_encode(puny, &d);
#ifdef USE_LIBIDN2
  எனில்(!result) {
    எழுத்து *c = curlx_strdup(d);
    idn2_free(d);
    எனில்(c)
      d = c;
    இல்லையெனில்
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  எனில்(!result)
    *output = d;
  திரும்பு result;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
வெறுமை Curl_free_idnconverted_hostname(கட்டமைப்பு hostname *host)
{
  curlx_safefree(host->encalloc);
}

#endif /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(கட்டமைப்பு hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#ifdef USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  எனில்(!Curl_is_ASCII_name(host->name)) {
    எழுத்து *decoded;
    CURLcode result = Curl_idn_decode(host->name, &decoded);
    எனில்(result)
      திரும்பு result;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#endif
  திரும்பு CURLE_OK;
}
