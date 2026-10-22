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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "idn.h"

#ifdef USE_LIBIDN2
#અન્તર્ભાવ <idn2.h>

#જો defined(_WIN32) && defined(UNICODE)
#define IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((અચલ uint8_t *)name, (uint8_t **)host, flags)
#નહીંતર
#define IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((અચલ અક્ષર *)(name), (અક્ષર **)(host), flags)
#endif
#endif /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#ifdef USE_APPLE_IDN
#અન્તર્ભાવ <unicode/uidna.h>
#અન્તર્ભાવ <iconv.h>
#અન્તર્ભાવ <langinfo.h>

#define MAX_HOST_LENGTH 512

સ્થિર CURLcode iconv_to_utf8(અચલ અક્ષર *in, size_t inlen,
                              અક્ષર **out, size_t *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  જો(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    અક્ષર *iconv_in = (અક્ષર *)CURL_UNCONST(in);
    size_t iconv_inlen = inlen;
    size_t iconv_result = iconv(cd, &iconv_in, &iconv_inlen,
                                out, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    જો(iconv_result == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      જો(errno == ENOMEM)
        પાછા CURLE_OUT_OF_MEMORY;
      નહીંતર
        પાછા CURLE_URL_MALFORMAT;
    }

    પાછા CURLE_OK;
  }
  નહીંતર {
    /* !checksrc! disable ERRNOVAR 1 */
    જો(errno == ENOMEM)
      પાછા CURLE_OUT_OF_MEMORY;
    નહીંતર
      પાછા CURLE_FAILED_INIT;
  }
}

સ્થિર CURLcode mac_idn_to_ascii(અચલ અક્ષર *in, અક્ષર **out)
{
  size_t inlen = strlen(in);
  જો(inlen < MAX_HOST_LENGTH) {
    અક્ષર iconv_buffer[MAX_HOST_LENGTH] = { 0 };
    અક્ષર *iconv_outptr = iconv_buffer;
    size_t iconv_outlen = કદ(iconv_buffer);
    CURLcode iconv_result = iconv_to_utf8(in, inlen,
                                          &iconv_outptr, &iconv_outlen);
    જો(!iconv_result) {
      UErrorCode err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_ASCII, &err);
      જો(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        અક્ષર buffer[MAX_HOST_LENGTH] = { 0 };
        (શૂન્ય)uidna_nameToASCII_UTF8(idna, iconv_buffer, (પૂર્ણાંક)iconv_outlen,
                                     buffer, કદ(buffer) - 1, &info, &err);
        uidna_close(idna);
        જો(!U_FAILURE(err) && !info.errors) {
          *out = curlx_strdup(buffer);
          જો(*out)
            પાછા CURLE_OK;
          નહીંતર
            પાછા CURLE_OUT_OF_MEMORY;
        }
      }
    }
    નહીંતર
      પાછા iconv_result;
  }
  પાછા CURLE_URL_MALFORMAT;
}

સ્થિર CURLcode mac_ascii_to_idn(અચલ અક્ષર *in, અક્ષર **out)
{
  size_t inlen = strlen(in);
  જો(inlen < MAX_HOST_LENGTH) {
    UErrorCode err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_UNICODE, &err);
    જો(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      અક્ષર buffer[MAX_HOST_LENGTH] = { 0 };
      (શૂન્ય)uidna_nameToUnicodeUTF8(idna, in, -1, buffer,
                                    કદ(buffer) - 1, &info, &err);
      uidna_close(idna);
      જો(!U_FAILURE(err)) {
        *out = curlx_strdup(buffer);
        જો(*out)
          પાછા CURLE_OK;
        નહીંતર
          પાછા CURLE_OUT_OF_MEMORY;
      }
    }
  }
  પાછા CURLE_URL_MALFORMAT;
}
#endif

#ifdef USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#define IDN_MAX_LENGTH 255

સ્થિર અક્ષર *idn_curlx_convert_wchar_to_UTF8(અચલ wchar_t *str_w, પૂર્ણાંક chars)
{
  અક્ષર *str_utf8 = ખાલી;
  પૂર્ણાંક bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, chars, ખાલી, 0,
                                  ખાલી, ખાલી);
  જો(bytes > 0) {
    str_utf8 = curlx_malloc(bytes);
    જો(str_utf8) {
      જો(WideCharToMultiByte(CP_UTF8, 0, str_w, chars, str_utf8, bytes,
                             ખાલી, ખાલી) == 0) {
        curlx_free(str_utf8);
        પાછા ખાલી;
      }
    }
  }
  પાછા str_utf8;
}

સ્થિર CURLcode win32_idn_to_ascii(અચલ અક્ષર *in, અક્ષર **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  પૂર્ણાંક in_w_len;
  *out = ખાલી;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  જો(in_w_len) {
    wchar_t punycode[IDN_MAX_LENGTH];
    પૂર્ણાંક chars = IdnToAscii(0, in_w, in_w_len, punycode, IDN_MAX_LENGTH);
    જો(chars > 0) {
      *out = idn_curlx_convert_wchar_to_UTF8(punycode, chars);
      જો(!*out)
        પાછા CURLE_OUT_OF_MEMORY;
    }
    નહીંતર
      પાછા CURLE_URL_MALFORMAT;
  }
  નહીંતર
    પાછા CURLE_URL_MALFORMAT;

  પાછા CURLE_OK;
}

સ્થિર CURLcode win32_ascii_to_idn(અચલ અક્ષર *in, અક્ષર **out)
{
  wchar_t in_w[IDN_MAX_LENGTH];
  પૂર્ણાંક in_w_len;
  *out = ખાલી;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  જો(in_w_len) {
    WCHAR idn[IDN_MAX_LENGTH]; /* stores a UTF-16 string */
    પૂર્ણાંક chars = IdnToUnicode(0, in_w, in_w_len, idn, IDN_MAX_LENGTH);
    જો(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *out = idn_curlx_convert_wchar_to_UTF8(idn, chars);
      જો(!*out)
        પાછા CURLE_OUT_OF_MEMORY;
    }
    નહીંતર
      પાછા CURLE_URL_MALFORMAT;
  }
  નહીંતર
    પાછા CURLE_URL_MALFORMAT;

  પાછા CURLE_OK;
}

#endif /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(અચલ અક્ષર *hostname)
{
  જો(hostname) {
    સંરચના Curl_str s;
    s.str = hostname;
    s.len = strlen(hostname);
    પાછા Curl_is_ASCII_str(&s);
  }
  પાછા TRUE;
}

bool Curl_is_ASCII_str(સંરચના Curl_str *s)
{
  જો(s && s->len) {
    અચલ unsigned અક્ષર *ch = (અચલ unsigned અક્ષર *)s->str;
    size_t i;
    માટે(i = 0; i < s->len; ++i) {
      જો(ch[i] & 0x80)
        પાછા FALSE;
    }
  }
  પાછા TRUE;
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
સ્થિર CURLcode idn_decode(અચલ અક્ષર *આગત, અક્ષર **output)
{
  અક્ષર *decoded = ખાલી;
  CURLcode result = CURLE_OK;
#ifdef USE_LIBIDN2
  જો(idn2_check_version(IDN2_VERSION)) {
    પૂર્ણાંક flags = IDN2_NFC_આગત
#જો IDN2_VERSION_સંખ્યા >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#endif
      ;
    પૂર્ણાંક rc = IDN2_LOOKUP(આગત, &decoded, flags);
    જો(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(આગત, &decoded, IDN2_TRANSITIONAL);
    જો(rc != IDN2_OK)
      result = CURLE_URL_MALFORMAT;
  }
  નહીંતર
    /* a too old libidn2 version */
    result = CURLE_NOT_BUILT_IN;
#અથવાજો defined(USE_WIN32_IDN)
  result = win32_idn_to_ascii(આગત, &decoded);
#અથવાજો defined(USE_APPLE_IDN)
  result = mac_idn_to_ascii(આગત, &decoded);
#endif
  જો(!result)
    *output = decoded;
  પાછા result;
}

સ્થિર CURLcode idn_encode(અચલ અક્ષર *puny, અક્ષર **output)
{
  અક્ષર *enc = ખાલી;
#ifdef USE_LIBIDN2
  પૂર્ણાંક rc = idn2_to_unicode_8z8z(puny, &enc, 0);
  જો(rc != IDNA_SUCCESS)
    પાછા rc == IDNA_MALLOC_ERROR ? CURLE_OUT_OF_MEMORY : CURLE_URL_MALFORMAT;
#અથવાજો defined(USE_WIN32_IDN)
  CURLcode result = win32_ascii_to_idn(puny, &enc);
  જો(result)
    પાછા result;
#અથવાજો defined(USE_APPLE_IDN)
  CURLcode result = mac_ascii_to_idn(puny, &enc);
  જો(result)
    પાછા result;
#endif
  *output = enc;
  પાછા CURLE_OK;
}

CURLcode Curl_idn_decode(અચલ અક્ષર *આગત, અક્ષર **output)
{
  અક્ષર *d = ખાલી;
  CURLcode result = idn_decode(આગત, &d);
#ifdef USE_LIBIDN2
  જો(!result) {
    અક્ષર *c = curlx_strdup(d);
    idn2_free(d);
    જો(c)
      d = c;
    નહીંતર
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  જો(!result) {
    જો(!d[0]) { /* ended up zero length, not acceptable */
      result = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    નહીંતર
      *output = d;
  }
  પાછા result;
}

CURLcode Curl_idn_encode(અચલ અક્ષર *puny, અક્ષર **output)
{
  અક્ષર *d = ખાલી;
  CURLcode result = idn_encode(puny, &d);
#ifdef USE_LIBIDN2
  જો(!result) {
    અક્ષર *c = curlx_strdup(d);
    idn2_free(d);
    જો(c)
      d = c;
    નહીંતર
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  જો(!result)
    *output = d;
  પાછા result;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
શૂન્ય Curl_free_idnconverted_hostname(સંરચના hostname *host)
{
  curlx_safefree(host->encalloc);
}

#endif /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(સંરચના hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#ifdef USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  જો(!Curl_is_ASCII_name(host->name)) {
    અક્ષર *decoded;
    CURLcode result = Curl_idn_decode(host->name, &decoded);
    જો(result)
      પાછા result;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#endif
  પાછા CURLE_OK;
}
