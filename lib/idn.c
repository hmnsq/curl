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
#समावेश "curl_setup.h"

#समावेश "urldata.h"
#समावेश "curlx/strparse.h"
#समावेश "idn.h"

#अगर_घोषित USE_LIBIDN2
#समावेश <idn2.h>

#अगर defined(_WIN32) && defined(UNICODE)
#घोषणा IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((स्थिर uint8_t *)name, (uint8_t **)host, flags)
#अन्यथा
#घोषणा IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((स्थिर अक्षर *)(name), (अक्षर **)(host), flags)
#पूर्ण_अगर
#पूर्ण_अगर /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#अगर_घोषित USE_APPLE_IDN
#समावेश <unicode/uidna.h>
#समावेश <iconv.h>
#समावेश <langinfo.h>

#घोषणा MAX_HOST_लंबाई 512

अटल CURLcode iconv_to_utf8(स्थिर अक्षर *in, माप_प्रकार inlen,
                              अक्षर **out, माप_प्रकार *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  अगर(cd != (iconv_t)-1) {
    माप_प्रकार iconv_outlen = *outlen;
    अक्षर *iconv_in = (अक्षर *)CURL_UNCONST(in);
    माप_प्रकार iconv_inlen = inlen;
    माप_प्रकार iconv_परिणाम = iconv(cd, &iconv_in, &iconv_inlen,
                                out, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    अगर(iconv_परिणाम == (माप_प्रकार)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      अगर(त्रुटि_सं == ENOMEM)
        वापस CURLE_OUT_OF_MEMORY;
      अन्यथा
        वापस CURLE_URL_MALFORMAT;
    }

    वापस CURLE_OK;
  }
  अन्यथा {
    /* !checksrc! disable ERRNOVAR 1 */
    अगर(त्रुटि_सं == ENOMEM)
      वापस CURLE_OUT_OF_MEMORY;
    अन्यथा
      वापस CURLE_FAILED_INIT;
  }
}

अटल CURLcode mac_idn_to_ascii(स्थिर अक्षर *in, अक्षर **out)
{
  माप_प्रकार inlen = म_माप(in);
  अगर(inlen < MAX_HOST_लंबाई) {
    अक्षर iconv_बफर[MAX_HOST_लंबाई] = { 0 };
    अक्षर *iconv_outptr = iconv_बफर;
    माप_प्रकार iconv_outlen = माप(iconv_बफर);
    CURLcode iconv_परिणाम = iconv_to_utf8(in, inlen,
                                          &iconv_outptr, &iconv_outlen);
    अगर(!iconv_परिणाम) {
      UErrorCode err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_ASCII, &err);
      अगर(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        अक्षर बफर[MAX_HOST_लंबाई] = { 0 };
        (व्योम)uidna_nameToASCII_UTF8(idna, iconv_बफर, (पूर्णांक)iconv_outlen,
                                     बफर, माप(बफर) - 1, &info, &err);
        uidna_close(idna);
        अगर(!U_FAILURE(err) && !info.errors) {
          *out = curlx_strdup(बफर);
          अगर(*out)
            वापस CURLE_OK;
          अन्यथा
            वापस CURLE_OUT_OF_MEMORY;
        }
      }
    }
    अन्यथा
      वापस iconv_परिणाम;
  }
  वापस CURLE_URL_MALFORMAT;
}

अटल CURLcode mac_ascii_to_idn(स्थिर अक्षर *in, अक्षर **out)
{
  माप_प्रकार inlen = म_माप(in);
  अगर(inlen < MAX_HOST_लंबाई) {
    UErrorCode err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_UNICODE, &err);
    अगर(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      अक्षर बफर[MAX_HOST_लंबाई] = { 0 };
      (व्योम)uidna_nameToUnicodeUTF8(idna, in, -1, बफर,
                                    माप(बफर) - 1, &info, &err);
      uidna_close(idna);
      अगर(!U_FAILURE(err)) {
        *out = curlx_strdup(बफर);
        अगर(*out)
          वापस CURLE_OK;
        अन्यथा
          वापस CURLE_OUT_OF_MEMORY;
      }
    }
  }
  वापस CURLE_URL_MALFORMAT;
}
#पूर्ण_अगर

#अगर_घोषित USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#घोषणा IDN_MAX_लंबाई 255

अटल अक्षर *idn_curlx_convert_wchar_to_UTF8(स्थिर ब_अक्षर_प्रकार *str_w, पूर्णांक chars)
{
  अक्षर *str_utf8 = शून्य;
  पूर्णांक bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, chars, शून्य, 0,
                                  शून्य, शून्य);
  अगर(bytes > 0) {
    str_utf8 = curlx_malloc(bytes);
    अगर(str_utf8) {
      अगर(WideCharToMultiByte(CP_UTF8, 0, str_w, chars, str_utf8, bytes,
                             शून्य, शून्य) == 0) {
        curlx_free(str_utf8);
        वापस शून्य;
      }
    }
  }
  वापस str_utf8;
}

अटल CURLcode win32_idn_to_ascii(स्थिर अक्षर *in, अक्षर **out)
{
  ब_अक्षर_प्रकार in_w[IDN_MAX_लंबाई];
  पूर्णांक in_w_len;
  *out = शून्य;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_लंबाई);
  अगर(in_w_len) {
    ब_अक्षर_प्रकार punycode[IDN_MAX_लंबाई];
    पूर्णांक chars = IdnToAscii(0, in_w, in_w_len, punycode, IDN_MAX_लंबाई);
    अगर(chars > 0) {
      *out = idn_curlx_convert_wchar_to_UTF8(punycode, chars);
      अगर(!*out)
        वापस CURLE_OUT_OF_MEMORY;
    }
    अन्यथा
      वापस CURLE_URL_MALFORMAT;
  }
  अन्यथा
    वापस CURLE_URL_MALFORMAT;

  वापस CURLE_OK;
}

अटल CURLcode win32_ascii_to_idn(स्थिर अक्षर *in, अक्षर **out)
{
  ब_अक्षर_प्रकार in_w[IDN_MAX_लंबाई];
  पूर्णांक in_w_len;
  *out = शून्य;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_लंबाई);
  अगर(in_w_len) {
    WCHAR idn[IDN_MAX_लंबाई]; /* stores a UTF-16 string */
    पूर्णांक chars = IdnToUnicode(0, in_w, in_w_len, idn, IDN_MAX_लंबाई);
    अगर(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *out = idn_curlx_convert_wchar_to_UTF8(idn, chars);
      अगर(!*out)
        वापस CURLE_OUT_OF_MEMORY;
    }
    अन्यथा
      वापस CURLE_URL_MALFORMAT;
  }
  अन्यथा
    वापस CURLE_URL_MALFORMAT;

  वापस CURLE_OK;
}

#पूर्ण_अगर /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(स्थिर अक्षर *hostname)
{
  अगर(hostname) {
    काष्ठा Curl_str s;
    s.str = hostname;
    s.len = म_माप(hostname);
    वापस Curl_is_ASCII_str(&s);
  }
  वापस TRUE;
}

bool Curl_is_ASCII_str(काष्ठा Curl_str *s)
{
  अगर(s && s->len) {
    स्थिर अचिन्हित अक्षर *ch = (स्थिर अचिन्हित अक्षर *)s->str;
    माप_प्रकार i;
    क्रम(i = 0; i < s->len; ++i) {
      अगर(ch[i] & 0x80)
        वापस FALSE;
    }
  }
  वापस TRUE;
}

#अगर_घोषित USE_IDN
/*
 * Curl_idn_decode() returns an allocated IDN decoded string if it was
 * possible. NULL on error.
 *
 * CURLE_URL_MALFORMAT - the hostname could not be converted
 * CURLE_OUT_OF_MEMORY - memory problem
 *
 */
अटल CURLcode idn_decode(स्थिर अक्षर *निवेश, अक्षर **output)
{
  अक्षर *decoded = शून्य;
  CURLcode परिणाम = CURLE_OK;
#अगर_घोषित USE_LIBIDN2
  अगर(idn2_check_version(IDN2_VERSION)) {
    पूर्णांक flags = IDN2_NFC_निवेश
#अगर IDN2_VERSION_संख्या >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#पूर्ण_अगर
      ;
    पूर्णांक rc = IDN2_LOOKUP(निवेश, &decoded, flags);
    अगर(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(निवेश, &decoded, IDN2_TRANSITIONAL);
    अगर(rc != IDN2_OK)
      परिणाम = CURLE_URL_MALFORMAT;
  }
  अन्यथा
    /* a too old libidn2 version */
    परिणाम = CURLE_NOT_BUILT_IN;
#या_अगर defined(USE_WIN32_IDN)
  परिणाम = win32_idn_to_ascii(निवेश, &decoded);
#या_अगर defined(USE_APPLE_IDN)
  परिणाम = mac_idn_to_ascii(निवेश, &decoded);
#पूर्ण_अगर
  अगर(!परिणाम)
    *output = decoded;
  वापस परिणाम;
}

अटल CURLcode idn_encode(स्थिर अक्षर *puny, अक्षर **output)
{
  अक्षर *enc = शून्य;
#अगर_घोषित USE_LIBIDN2
  पूर्णांक rc = idn2_to_unicode_8z8z(puny, &enc, 0);
  अगर(rc != IDNA_SUCCESS)
    वापस rc == IDNA_MALLOC_ERROR ? CURLE_OUT_OF_MEMORY : CURLE_URL_MALFORMAT;
#या_अगर defined(USE_WIN32_IDN)
  CURLcode परिणाम = win32_ascii_to_idn(puny, &enc);
  अगर(परिणाम)
    वापस परिणाम;
#या_अगर defined(USE_APPLE_IDN)
  CURLcode परिणाम = mac_ascii_to_idn(puny, &enc);
  अगर(परिणाम)
    वापस परिणाम;
#पूर्ण_अगर
  *output = enc;
  वापस CURLE_OK;
}

CURLcode Curl_idn_decode(स्थिर अक्षर *निवेश, अक्षर **output)
{
  अक्षर *d = शून्य;
  CURLcode परिणाम = idn_decode(निवेश, &d);
#अगर_घोषित USE_LIBIDN2
  अगर(!परिणाम) {
    अक्षर *c = curlx_strdup(d);
    idn2_free(d);
    अगर(c)
      d = c;
    अन्यथा
      परिणाम = CURLE_OUT_OF_MEMORY;
  }
#पूर्ण_अगर
  अगर(!परिणाम) {
    अगर(!d[0]) { /* ended up zero length, not acceptable */
      परिणाम = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    अन्यथा
      *output = d;
  }
  वापस परिणाम;
}

CURLcode Curl_idn_encode(स्थिर अक्षर *puny, अक्षर **output)
{
  अक्षर *d = शून्य;
  CURLcode परिणाम = idn_encode(puny, &d);
#अगर_घोषित USE_LIBIDN2
  अगर(!परिणाम) {
    अक्षर *c = curlx_strdup(d);
    idn2_free(d);
    अगर(c)
      d = c;
    अन्यथा
      परिणाम = CURLE_OUT_OF_MEMORY;
  }
#पूर्ण_अगर
  अगर(!परिणाम)
    *output = d;
  वापस परिणाम;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
व्योम Curl_free_idnconverted_hostname(काष्ठा hostname *host)
{
  curlx_safefree(host->encalloc);
}

#पूर्ण_अगर /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(काष्ठा hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#अगर_घोषित USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  अगर(!Curl_is_ASCII_name(host->name)) {
    अक्षर *decoded;
    CURLcode परिणाम = Curl_idn_decode(host->name, &decoded);
    अगर(परिणाम)
      वापस परिणाम;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#पूर्ण_अगर
  वापस CURLE_OK;
}
