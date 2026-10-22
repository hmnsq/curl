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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "idn.h"

#ifdef USE_LIBIDN2
#അന്തര്ഭാവ <idn2.h>

#എങ്കിൽ defined(_WIN32) && defined(UNICODE)
#define IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((സ്ഥിരം uint8_t *)name, (uint8_t **)host, flags)
#അല്ലെങ്കിൽ
#define IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((സ്ഥിരം അക്ഷരം *)(name), (അക്ഷരം **)(host), flags)
#endif
#endif /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#ifdef USE_APPLE_IDN
#അന്തര്ഭാവ <unicode/uidna.h>
#അന്തര്ഭാവ <iconv.h>
#അന്തര്ഭാവ <langinfo.h>

#define MAX_HOST_LENGTH 512

സ്ഥിതം CURLcode iconv_to_utf8(സ്ഥിരം അക്ഷരം *in, size_t inlen,
                              അക്ഷരം **out, size_t *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  എങ്കിൽ(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    അക്ഷരം *iconv_in = (അക്ഷരം *)CURL_UNCONST(in);
    size_t iconv_inlen = inlen;
    size_t iconv_result = iconv(cd, &iconv_in, &iconv_inlen,
                                out, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    എങ്കിൽ(iconv_result == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      എങ്കിൽ(errno == ENOMEM)
        തിരികെ CURLE_OUT_OF_MEMORY;
      അല്ലെങ്കിൽ
        തിരികെ CURLE_URL_MALFORMAT;
    }

    തിരികെ CURLE_OK;
  }
  അല്ലെങ്കിൽ {
    /* !checksrc! disable ERRNOVAR 1 */
    എങ്കിൽ(errno == ENOMEM)
      തിരികെ CURLE_OUT_OF_MEMORY;
    അല്ലെങ്കിൽ
      തിരികെ CURLE_FAILED_INIT;
  }
}

സ്ഥിതം CURLcode mac_idn_to_ascii(സ്ഥിരം അക്ഷരം *in, അക്ഷരം **out)
{
  size_t inlen = strlen(in);
  എങ്കിൽ(inlen < MAX_HOST_LENGTH) {
    അക്ഷരം iconv_buffer[MAX_HOST_LENGTH] = { 0 };
    അക്ഷരം *iconv_outptr = iconv_buffer;
    size_t iconv_outlen = വലുപ്പം(iconv_buffer);
    CURLcode iconv_result = iconv_to_utf8(in, inlen,
                                          &iconv_outptr, &iconv_outlen);
    എങ്കിൽ(!iconv_result) {
      UErrorCode err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_ASCII, &err);
      എങ്കിൽ(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        അക്ഷരം buffer[MAX_HOST_LENGTH] = { 0 };
        (ശൂന്യം)uidna_nameToASCII_UTF8(idna, iconv_buffer, (പൂർണ്ണസംഖ്യ)iconv_outlen,
                                     buffer, വലുപ്പം(buffer) - 1, &info, &err);
        uidna_close(idna);
        എങ്കിൽ(!U_FAILURE(err) && !info.errors) {
          *out = curlx_strdup(buffer);
          എങ്കിൽ(*out)
            തിരികെ CURLE_OK;
          അല്ലെങ്കിൽ
            തിരികെ CURLE_OUT_OF_MEMORY;
        }
      }
    }
    അല്ലെങ്കിൽ
      തിരികെ iconv_result;
  }
  തിരികെ CURLE_URL_MALFORMAT;
}

സ്ഥിതം CURLcode mac_ascii_to_idn(സ്ഥിരം അക്ഷരം *in, അക്ഷരം **out)
{
  size_t inlen = strlen(in);
  എങ്കിൽ(inlen < MAX_HOST_LENGTH) {
    UErrorCode err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_UNICODE, &err);
    എങ്കിൽ(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      അക്ഷരം buffer[MAX_HOST_LENGTH] = { 0 };
      (ശൂന്യം)uidna_nameToUnicodeUTF8(idna, in, -1, buffer,
                                    വലുപ്പം(buffer) - 1, &info, &err);
      uidna_close(idna);
      എങ്കിൽ(!U_FAILURE(err)) {
        *out = curlx_strdup(buffer);
        എങ്കിൽ(*out)
          തിരികെ CURLE_OK;
        അല്ലെങ്കിൽ
          തിരികെ CURLE_OUT_OF_MEMORY;
      }
    }
  }
  തിരികെ CURLE_URL_MALFORMAT;
}
#endif

#ifdef USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#define IDN_MAX_LENGTH 255

സ്ഥിതം അക്ഷരം *idn_curlx_convert_wchar_to_UTF8(സ്ഥിരം wchar_t *str_w, പൂർണ്ണസംഖ്യ chars)
{
  അക്ഷരം *str_utf8 = ശൂന്യ;
  പൂർണ്ണസംഖ്യ bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, chars, ശൂന്യ, 0,
                                  ശൂന്യ, ശൂന്യ);
  എങ്കിൽ(bytes > 0) {
    str_utf8 = curlx_malloc(bytes);
    എങ്കിൽ(str_utf8) {
      എങ്കിൽ(WideCharToMultiByte(CP_UTF8, 0, str_w, chars, str_utf8, bytes,
                             ശൂന്യ, ശൂന്യ) == 0) {
        curlx_free(str_utf8);
        തിരികെ ശൂന്യ;
      }
    }
  }
  തിരികെ str_utf8;
}

സ്ഥിതം CURLcode win32_idn_to_ascii(സ്ഥിരം അക്ഷരം *in, അക്ഷരം **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  പൂർണ്ണസംഖ്യ in_w_len;
  *out = ശൂന്യ;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  എങ്കിൽ(in_w_len) {
    wchar_t punycode[IDN_MAX_LENGTH];
    പൂർണ്ണസംഖ്യ chars = IdnToAscii(0, in_w, in_w_len, punycode, IDN_MAX_LENGTH);
    എങ്കിൽ(chars > 0) {
      *out = idn_curlx_convert_wchar_to_UTF8(punycode, chars);
      എങ്കിൽ(!*out)
        തിരികെ CURLE_OUT_OF_MEMORY;
    }
    അല്ലെങ്കിൽ
      തിരികെ CURLE_URL_MALFORMAT;
  }
  അല്ലെങ്കിൽ
    തിരികെ CURLE_URL_MALFORMAT;

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode win32_ascii_to_idn(സ്ഥിരം അക്ഷരം *in, അക്ഷരം **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  പൂർണ്ണസംഖ്യ in_w_len;
  *out = ശൂന്യ;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  എങ്കിൽ(in_w_len) {
    WCHAR idn[IDN_MAX_LENGTH]; /* stores a UTF-16 string */
    പൂർണ്ണസംഖ്യ chars = IdnToUnicode(0, in_w, in_w_len, idn, IDN_MAX_LENGTH);
    എങ്കിൽ(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *out = idn_curlx_convert_wchar_to_UTF8(idn, chars);
      എങ്കിൽ(!*out)
        തിരികെ CURLE_OUT_OF_MEMORY;
    }
    അല്ലെങ്കിൽ
      തിരികെ CURLE_URL_MALFORMAT;
  }
  അല്ലെങ്കിൽ
    തിരികെ CURLE_URL_MALFORMAT;

  തിരികെ CURLE_OK;
}

#endif /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(സ്ഥിരം അക്ഷരം *hostname)
{
  എങ്കിൽ(hostname) {
    ഘടന Curl_str s;
    s.str = hostname;
    s.len = strlen(hostname);
    തിരികെ Curl_is_ASCII_str(&s);
  }
  തിരികെ TRUE;
}

bool Curl_is_ASCII_str(ഘടന Curl_str *s)
{
  എങ്കിൽ(s && s->len) {
    സ്ഥിരം unsigned അക്ഷരം *ch = (സ്ഥിരം unsigned അക്ഷരം *)s->str;
    size_t i;
    വേണ്ടി(i = 0; i < s->len; ++i) {
      എങ്കിൽ(ch[i] & 0x80)
        തിരികെ FALSE;
    }
  }
  തിരികെ TRUE;
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
സ്ഥിതം CURLcode idn_decode(സ്ഥിരം അക്ഷരം *നിവേശം, അക്ഷരം **output)
{
  അക്ഷരം *decoded = ശൂന്യ;
  CURLcode result = CURLE_OK;
#ifdef USE_LIBIDN2
  എങ്കിൽ(idn2_check_version(IDN2_VERSION)) {
    പൂർണ്ണസംഖ്യ flags = IDN2_NFC_നിവേശം
#എങ്കിൽ IDN2_VERSION_സംഖ്യ >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#endif
      ;
    പൂർണ്ണസംഖ്യ rc = IDN2_LOOKUP(നിവേശം, &decoded, flags);
    എങ്കിൽ(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(നിവേശം, &decoded, IDN2_TRANSITIONAL);
    എങ്കിൽ(rc != IDN2_OK)
      result = CURLE_URL_MALFORMAT;
  }
  അല്ലെങ്കിൽ
    /* a too old libidn2 version */
    result = CURLE_NOT_BUILT_IN;
#അല്ലെങ്കിലെങ്കിൽ defined(USE_WIN32_IDN)
  result = win32_idn_to_ascii(നിവേശം, &decoded);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_APPLE_IDN)
  result = mac_idn_to_ascii(നിവേശം, &decoded);
#endif
  എങ്കിൽ(!result)
    *output = decoded;
  തിരികെ result;
}

സ്ഥിതം CURLcode idn_encode(സ്ഥിരം അക്ഷരം *puny, അക്ഷരം **output)
{
  അക്ഷരം *enc = ശൂന്യ;
#ifdef USE_LIBIDN2
  പൂർണ്ണസംഖ്യ rc = idn2_to_unicode_8z8z(puny, &enc, 0);
  എങ്കിൽ(rc != IDNA_SUCCESS)
    തിരികെ rc == IDNA_MALLOC_ERROR ? CURLE_OUT_OF_MEMORY : CURLE_URL_MALFORMAT;
#അല്ലെങ്കിലെങ്കിൽ defined(USE_WIN32_IDN)
  CURLcode result = win32_ascii_to_idn(puny, &enc);
  എങ്കിൽ(result)
    തിരികെ result;
#അല്ലെങ്കിലെങ്കിൽ defined(USE_APPLE_IDN)
  CURLcode result = mac_ascii_to_idn(puny, &enc);
  എങ്കിൽ(result)
    തിരികെ result;
#endif
  *output = enc;
  തിരികെ CURLE_OK;
}

CURLcode Curl_idn_decode(സ്ഥിരം അക്ഷരം *നിവേശം, അക്ഷരം **output)
{
  അക്ഷരം *d = ശൂന്യ;
  CURLcode result = idn_decode(നിവേശം, &d);
#ifdef USE_LIBIDN2
  എങ്കിൽ(!result) {
    അക്ഷരം *c = curlx_strdup(d);
    idn2_free(d);
    എങ്കിൽ(c)
      d = c;
    അല്ലെങ്കിൽ
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  എങ്കിൽ(!result) {
    എങ്കിൽ(!d[0]) { /* ended up zero length, not acceptable */
      result = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    അല്ലെങ്കിൽ
      *output = d;
  }
  തിരികെ result;
}

CURLcode Curl_idn_encode(സ്ഥിരം അക്ഷരം *puny, അക്ഷരം **output)
{
  അക്ഷരം *d = ശൂന്യ;
  CURLcode result = idn_encode(puny, &d);
#ifdef USE_LIBIDN2
  എങ്കിൽ(!result) {
    അക്ഷരം *c = curlx_strdup(d);
    idn2_free(d);
    എങ്കിൽ(c)
      d = c;
    അല്ലെങ്കിൽ
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  എങ്കിൽ(!result)
    *output = d;
  തിരികെ result;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
ശൂന്യം Curl_free_idnconverted_hostname(ഘടന hostname *host)
{
  curlx_safefree(host->encalloc);
}

#endif /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(ഘടന hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#ifdef USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  എങ്കിൽ(!Curl_is_ASCII_name(host->name)) {
    അക്ഷരം *decoded;
    CURLcode result = Curl_idn_decode(host->name, &decoded);
    എങ്കിൽ(result)
      തിരികെ result;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#endif
  തിരികെ CURLE_OK;
}
