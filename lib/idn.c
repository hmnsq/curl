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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "idn.h"

#ifdef USE_LIBIDN2
#अन्तर्भाव <idn2.h>

#जर defined(_WIN32) && defined(UNICODE)
#define IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((स्थिरांक uint8_t *)name, (uint8_t **)host, flags)
#अन्यथा
#define IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((स्थिरांक अक्षर *)(name), (अक्षर **)(host), flags)
#endif
#endif /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#ifdef USE_APPLE_IDN
#अन्तर्भाव <unicode/uidna.h>
#अन्तर्भाव <iconv.h>
#अन्तर्भाव <langinfo.h>

#define MAX_HOST_LENGTH 512

स्थिर CURLcode iconv_to_utf8(स्थिरांक अक्षर *in, माप_प्रकार inlen,
                              अक्षर **out, माप_प्रकार *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  जर(cd != (iconv_t)-1) {
    माप_प्रकार iconv_outlen = *outlen;
    अक्षर *iconv_in = (अक्षर *)CURL_UNCONST(in);
    माप_प्रकार iconv_inlen = inlen;
    माप_प्रकार iconv_result = iconv(cd, &iconv_in, &iconv_inlen,
                                out, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    जर(iconv_result == (माप_प्रकार)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      जर(त्रुटि_सं == ENOMEM)
        परत CURLE_OUT_OF_MEMORY;
      अन्यथा
        परत CURLE_URL_MALFORMAT;
    }

    परत CURLE_OK;
  }
  अन्यथा {
    /* !checksrc! disable ERRNOVAR 1 */
    जर(त्रुटि_सं == ENOMEM)
      परत CURLE_OUT_OF_MEMORY;
    अन्यथा
      परत CURLE_FAILED_INIT;
  }
}

स्थिर CURLcode mac_idn_to_ascii(स्थिरांक अक्षर *in, अक्षर **out)
{
  माप_प्रकार inlen = म_माप(in);
  जर(inlen < MAX_HOST_LENGTH) {
    अक्षर iconv_buffer[MAX_HOST_LENGTH] = { 0 };
    अक्षर *iconv_outptr = iconv_buffer;
    माप_प्रकार iconv_outlen = आकार(iconv_buffer);
    CURLcode iconv_result = iconv_to_utf8(in, inlen,
                                          &iconv_outptr, &iconv_outlen);
    जर(!iconv_result) {
      UErrorCode err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_ASCII, &err);
      जर(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        अक्षर buffer[MAX_HOST_LENGTH] = { 0 };
        (शून्य)uidna_nameToASCII_UTF8(idna, iconv_buffer, (पूर्णांक)iconv_outlen,
                                     buffer, आकार(buffer) - 1, &info, &err);
        uidna_close(idna);
        जर(!U_FAILURE(err) && !info.errors) {
          *out = curlx_strdup(buffer);
          जर(*out)
            परत CURLE_OK;
          अन्यथा
            परत CURLE_OUT_OF_MEMORY;
        }
      }
    }
    अन्यथा
      परत iconv_result;
  }
  परत CURLE_URL_MALFORMAT;
}

स्थिर CURLcode mac_ascii_to_idn(स्थिरांक अक्षर *in, अक्षर **out)
{
  माप_प्रकार inlen = म_माप(in);
  जर(inlen < MAX_HOST_LENGTH) {
    UErrorCode err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_UNICODE, &err);
    जर(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      अक्षर buffer[MAX_HOST_LENGTH] = { 0 };
      (शून्य)uidna_nameToUnicodeUTF8(idna, in, -1, buffer,
                                    आकार(buffer) - 1, &info, &err);
      uidna_close(idna);
      जर(!U_FAILURE(err)) {
        *out = curlx_strdup(buffer);
        जर(*out)
          परत CURLE_OK;
        अन्यथा
          परत CURLE_OUT_OF_MEMORY;
      }
    }
  }
  परत CURLE_URL_MALFORMAT;
}
#endif

#ifdef USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#define IDN_MAX_LENGTH 255

स्थिर अक्षर *idn_curlx_convert_wchar_to_UTF8(स्थिरांक ब_अक्षर_प्रकार *str_w, पूर्णांक chars)
{
  अक्षर *str_utf8 = रिक्त;
  पूर्णांक bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, chars, रिक्त, 0,
                                  रिक्त, रिक्त);
  जर(bytes > 0) {
    str_utf8 = curlx_malloc(bytes);
    जर(str_utf8) {
      जर(WideCharToMultiByte(CP_UTF8, 0, str_w, chars, str_utf8, bytes,
                             रिक्त, रिक्त) == 0) {
        curlx_free(str_utf8);
        परत रिक्त;
      }
    }
  }
  परत str_utf8;
}

स्थिर CURLcode win32_idn_to_ascii(स्थिरांक अक्षर *in, अक्षर **out)
{
  ब_अक्षर_प्रकार in_w[IDN_MAX_LENGTH];
  पूर्णांक in_w_len;
  *out = रिक्त;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  जर(in_w_len) {
    ब_अक्षर_प्रकार punycode[IDN_MAX_LENGTH];
    पूर्णांक chars = IdnToAscii(0, in_w, in_w_len, punycode, IDN_MAX_LENGTH);
    जर(chars > 0) {
      *out = idn_curlx_convert_wchar_to_UTF8(punycode, chars);
      जर(!*out)
        परत CURLE_OUT_OF_MEMORY;
    }
    अन्यथा
      परत CURLE_URL_MALFORMAT;
  }
  अन्यथा
    परत CURLE_URL_MALFORMAT;

  परत CURLE_OK;
}

स्थिर CURLcode win32_ascii_to_idn(स्थिरांक अक्षर *in, अक्षर **out)
{
  ब_अक्षर_प्रकार in_w[IDN_MAX_LENGTH];
  पूर्णांक in_w_len;
  *out = रिक्त;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  जर(in_w_len) {
    WCHAR idn[IDN_MAX_LENGTH]; /* stores a UTF-16 string */
    पूर्णांक chars = IdnToUnicode(0, in_w, in_w_len, idn, IDN_MAX_LENGTH);
    जर(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *out = idn_curlx_convert_wchar_to_UTF8(idn, chars);
      जर(!*out)
        परत CURLE_OUT_OF_MEMORY;
    }
    अन्यथा
      परत CURLE_URL_MALFORMAT;
  }
  अन्यथा
    परत CURLE_URL_MALFORMAT;

  परत CURLE_OK;
}

#endif /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(स्थिरांक अक्षर *hostname)
{
  जर(hostname) {
    संरचना Curl_str s;
    s.str = hostname;
    s.len = म_माप(hostname);
    परत Curl_is_ASCII_str(&s);
  }
  परत TRUE;
}

bool Curl_is_ASCII_str(संरचना Curl_str *s)
{
  जर(s && s->len) {
    स्थिरांक अचिन्हित अक्षर *ch = (स्थिरांक अचिन्हित अक्षर *)s->str;
    माप_प्रकार i;
    साठी(i = 0; i < s->len; ++i) {
      जर(ch[i] & 0x80)
        परत FALSE;
    }
  }
  परत TRUE;
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
स्थिर CURLcode idn_decode(स्थिरांक अक्षर *आदान, अक्षर **output)
{
  अक्षर *decoded = रिक्त;
  CURLcode result = CURLE_OK;
#ifdef USE_LIBIDN2
  जर(idn2_check_version(IDN2_VERSION)) {
    पूर्णांक flags = IDN2_NFC_आदान
#जर IDN2_VERSION_संख्या >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#endif
      ;
    पूर्णांक rc = IDN2_LOOKUP(आदान, &decoded, flags);
    जर(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(आदान, &decoded, IDN2_TRANSITIONAL);
    जर(rc != IDN2_OK)
      result = CURLE_URL_MALFORMAT;
  }
  अन्यथा
    /* a too old libidn2 version */
    result = CURLE_NOT_BUILT_IN;
#किंवाजर defined(USE_WIN32_IDN)
  result = win32_idn_to_ascii(आदान, &decoded);
#किंवाजर defined(USE_APPLE_IDN)
  result = mac_idn_to_ascii(आदान, &decoded);
#endif
  जर(!result)
    *output = decoded;
  परत result;
}

स्थिर CURLcode idn_encode(स्थिरांक अक्षर *puny, अक्षर **output)
{
  अक्षर *enc = रिक्त;
#ifdef USE_LIBIDN2
  पूर्णांक rc = idn2_to_unicode_8z8z(puny, &enc, 0);
  जर(rc != IDNA_SUCCESS)
    परत rc == IDNA_MALLOC_ERROR ? CURLE_OUT_OF_MEMORY : CURLE_URL_MALFORMAT;
#किंवाजर defined(USE_WIN32_IDN)
  CURLcode result = win32_ascii_to_idn(puny, &enc);
  जर(result)
    परत result;
#किंवाजर defined(USE_APPLE_IDN)
  CURLcode result = mac_ascii_to_idn(puny, &enc);
  जर(result)
    परत result;
#endif
  *output = enc;
  परत CURLE_OK;
}

CURLcode Curl_idn_decode(स्थिरांक अक्षर *आदान, अक्षर **output)
{
  अक्षर *d = रिक्त;
  CURLcode result = idn_decode(आदान, &d);
#ifdef USE_LIBIDN2
  जर(!result) {
    अक्षर *c = curlx_strdup(d);
    idn2_free(d);
    जर(c)
      d = c;
    अन्यथा
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  जर(!result) {
    जर(!d[0]) { /* ended up zero length, not acceptable */
      result = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    अन्यथा
      *output = d;
  }
  परत result;
}

CURLcode Curl_idn_encode(स्थिरांक अक्षर *puny, अक्षर **output)
{
  अक्षर *d = रिक्त;
  CURLcode result = idn_encode(puny, &d);
#ifdef USE_LIBIDN2
  जर(!result) {
    अक्षर *c = curlx_strdup(d);
    idn2_free(d);
    जर(c)
      d = c;
    अन्यथा
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  जर(!result)
    *output = d;
  परत result;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
शून्य Curl_free_idnconverted_hostname(संरचना hostname *host)
{
  curlx_safefree(host->encalloc);
}

#endif /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(संरचना hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#ifdef USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  जर(!Curl_is_ASCII_name(host->name)) {
    अक्षर *decoded;
    CURLcode result = Curl_idn_decode(host->name, &decoded);
    जर(result)
      परत result;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#endif
  परत CURLE_OK;
}
