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

#यदि defined(_WIN32) && defined(UNICODE)
#define IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((ध्रुव uint8_t *)name, (uint8_t **)host, flags)
#अन्यथा
#define IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((ध्रुव वर्ण *)(name), (वर्ण **)(host), flags)
#endif
#endif /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#ifdef USE_APPLE_IDN
#अन्तर्भाव <unicode/uidna.h>
#अन्तर्भाव <iconv.h>
#अन्तर्भाव <langinfo.h>

#define MAX_HOST_LENGTH 512

स्थिर CURLcode iconv_to_utf8(ध्रुव वर्ण *in, माप_प्रकार inlen,
                              वर्ण **out, माप_प्रकार *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  यदि(cd != (iconv_t)-1) {
    माप_प्रकार iconv_outlen = *outlen;
    वर्ण *iconv_in = (वर्ण *)CURL_UNCONST(in);
    माप_प्रकार iconv_inlen = inlen;
    माप_प्रकार iconv_result = iconv(cd, &iconv_in, &iconv_inlen,
                                out, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    यदि(iconv_result == (माप_प्रकार)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      यदि(त्रुटि_सं == ENOMEM)
        प्रत्यागम CURLE_OUT_OF_MEMORY;
      अन्यथा
        प्रत्यागम CURLE_URL_MALFORMAT;
    }

    प्रत्यागम CURLE_OK;
  }
  अन्यथा {
    /* !checksrc! disable ERRNOVAR 1 */
    यदि(त्रुटि_सं == ENOMEM)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    अन्यथा
      प्रत्यागम CURLE_FAILED_INIT;
  }
}

स्थिर CURLcode mac_idn_to_ascii(ध्रुव वर्ण *in, वर्ण **out)
{
  माप_प्रकार inlen = म_माप(in);
  यदि(inlen < MAX_HOST_LENGTH) {
    वर्ण iconv_buffer[MAX_HOST_LENGTH] = { 0 };
    वर्ण *iconv_outptr = iconv_buffer;
    माप_प्रकार iconv_outlen = परिमाण(iconv_buffer);
    CURLcode iconv_result = iconv_to_utf8(in, inlen,
                                          &iconv_outptr, &iconv_outlen);
    यदि(!iconv_result) {
      UErrorCode err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_ASCII, &err);
      यदि(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        वर्ण buffer[MAX_HOST_LENGTH] = { 0 };
        (शून्य)uidna_nameToASCII_UTF8(idna, iconv_buffer, (पूर्णाङ्क)iconv_outlen,
                                     buffer, परिमाण(buffer) - 1, &info, &err);
        uidna_close(idna);
        यदि(!U_FAILURE(err) && !info.errors) {
          *out = curlx_strdup(buffer);
          यदि(*out)
            प्रत्यागम CURLE_OK;
          अन्यथा
            प्रत्यागम CURLE_OUT_OF_MEMORY;
        }
      }
    }
    अन्यथा
      प्रत्यागम iconv_result;
  }
  प्रत्यागम CURLE_URL_MALFORMAT;
}

स्थिर CURLcode mac_ascii_to_idn(ध्रुव वर्ण *in, वर्ण **out)
{
  माप_प्रकार inlen = म_माप(in);
  यदि(inlen < MAX_HOST_LENGTH) {
    UErrorCode err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_TO_UNICODE, &err);
    यदि(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      वर्ण buffer[MAX_HOST_LENGTH] = { 0 };
      (शून्य)uidna_nameToUnicodeUTF8(idna, in, -1, buffer,
                                    परिमाण(buffer) - 1, &info, &err);
      uidna_close(idna);
      यदि(!U_FAILURE(err)) {
        *out = curlx_strdup(buffer);
        यदि(*out)
          प्रत्यागम CURLE_OK;
        अन्यथा
          प्रत्यागम CURLE_OUT_OF_MEMORY;
      }
    }
  }
  प्रत्यागम CURLE_URL_MALFORMAT;
}
#endif

#ifdef USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#define IDN_MAX_LENGTH 255

स्थिर वर्ण *idn_curlx_convert_wchar_to_UTF8(ध्रुव ब_अक्षर_प्रकार *str_w, पूर्णाङ्क chars)
{
  वर्ण *str_utf8 = रिक्त;
  पूर्णाङ्क bytes = WideCharToMultiByte(CP_UTF8, 0, str_w, chars, रिक्त, 0,
                                  रिक्त, रिक्त);
  यदि(bytes > 0) {
    str_utf8 = curlx_malloc(bytes);
    यदि(str_utf8) {
      यदि(WideCharToMultiByte(CP_UTF8, 0, str_w, chars, str_utf8, bytes,
                             रिक्त, रिक्त) == 0) {
        curlx_free(str_utf8);
        प्रत्यागम रिक्त;
      }
    }
  }
  प्रत्यागम str_utf8;
}

स्थिर CURLcode win32_idn_to_ascii(ध्रुव वर्ण *in, वर्ण **out)
{
  ब_अक्षर_प्रकार in_w[IDN_MAX_LENGTH];
  पूर्णाङ्क in_w_len;
  *out = रिक्त;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  यदि(in_w_len) {
    ब_अक्षर_प्रकार punycode[IDN_MAX_LENGTH];
    पूर्णाङ्क chars = IdnToAscii(0, in_w, in_w_len, punycode, IDN_MAX_LENGTH);
    यदि(chars > 0) {
      *out = idn_curlx_convert_wchar_to_UTF8(punycode, chars);
      यदि(!*out)
        प्रत्यागम CURLE_OUT_OF_MEMORY;
    }
    अन्यथा
      प्रत्यागम CURLE_URL_MALFORMAT;
  }
  अन्यथा
    प्रत्यागम CURLE_URL_MALFORMAT;

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode win32_ascii_to_idn(ध्रुव वर्ण *in, वर्ण **out)
{
  ब_अक्षर_प्रकार in_w[IDN_MAX_LENGTH];
  पूर्णाङ्क in_w_len;
  *out = रिक्त;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  in_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 in, -1, in_w, IDN_MAX_LENGTH);
  यदि(in_w_len) {
    WCHAR idn[IDN_MAX_LENGTH]; /* stores a UTF-16 string */
    पूर्णाङ्क chars = IdnToUnicode(0, in_w, in_w_len, idn, IDN_MAX_LENGTH);
    यदि(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *out = idn_curlx_convert_wchar_to_UTF8(idn, chars);
      यदि(!*out)
        प्रत्यागम CURLE_OUT_OF_MEMORY;
    }
    अन्यथा
      प्रत्यागम CURLE_URL_MALFORMAT;
  }
  अन्यथा
    प्रत्यागम CURLE_URL_MALFORMAT;

  प्रत्यागम CURLE_OK;
}

#endif /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(ध्रुव वर्ण *hostname)
{
  यदि(hostname) {
    संरचना Curl_str s;
    s.str = hostname;
    s.len = म_माप(hostname);
    प्रत्यागम Curl_is_ASCII_str(&s);
  }
  प्रत्यागम TRUE;
}

bool Curl_is_ASCII_str(संरचना Curl_str *s)
{
  यदि(s && s->len) {
    ध्रुव अचिन्हित वर्ण *ch = (ध्रुव अचिन्हित वर्ण *)s->str;
    माप_प्रकार i;
    प्रति(i = 0; i < s->len; ++i) {
      यदि(ch[i] & 0x80)
        प्रत्यागम FALSE;
    }
  }
  प्रत्यागम TRUE;
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
स्थिर CURLcode idn_decode(ध्रुव वर्ण *निवेश, वर्ण **output)
{
  वर्ण *decoded = रिक्त;
  CURLcode result = CURLE_OK;
#ifdef USE_LIBIDN2
  यदि(idn2_check_version(IDN2_VERSION)) {
    पूर्णाङ्क flags = IDN2_NFC_निवेश
#यदि IDN2_VERSION_संख्या >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#endif
      ;
    पूर्णाङ्क rc = IDN2_LOOKUP(निवेश, &decoded, flags);
    यदि(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(निवेश, &decoded, IDN2_TRANSITIONAL);
    यदि(rc != IDN2_OK)
      result = CURLE_URL_MALFORMAT;
  }
  अन्यथा
    /* a too old libidn2 version */
    result = CURLE_NOT_BUILT_IN;
#नोचेद्यदि defined(USE_WIN32_IDN)
  result = win32_idn_to_ascii(निवेश, &decoded);
#नोचेद्यदि defined(USE_APPLE_IDN)
  result = mac_idn_to_ascii(निवेश, &decoded);
#endif
  यदि(!result)
    *output = decoded;
  प्रत्यागम result;
}

स्थिर CURLcode idn_encode(ध्रुव वर्ण *puny, वर्ण **output)
{
  वर्ण *enc = रिक्त;
#ifdef USE_LIBIDN2
  पूर्णाङ्क rc = idn2_to_unicode_8z8z(puny, &enc, 0);
  यदि(rc != IDNA_SUCCESS)
    प्रत्यागम rc == IDNA_MALLOC_ERROR ? CURLE_OUT_OF_MEMORY : CURLE_URL_MALFORMAT;
#नोचेद्यदि defined(USE_WIN32_IDN)
  CURLcode result = win32_ascii_to_idn(puny, &enc);
  यदि(result)
    प्रत्यागम result;
#नोचेद्यदि defined(USE_APPLE_IDN)
  CURLcode result = mac_ascii_to_idn(puny, &enc);
  यदि(result)
    प्रत्यागम result;
#endif
  *output = enc;
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_idn_decode(ध्रुव वर्ण *निवेश, वर्ण **output)
{
  वर्ण *d = रिक्त;
  CURLcode result = idn_decode(निवेश, &d);
#ifdef USE_LIBIDN2
  यदि(!result) {
    वर्ण *c = curlx_strdup(d);
    idn2_free(d);
    यदि(c)
      d = c;
    अन्यथा
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  यदि(!result) {
    यदि(!d[0]) { /* ended up zero length, not acceptable */
      result = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    अन्यथा
      *output = d;
  }
  प्रत्यागम result;
}

CURLcode Curl_idn_encode(ध्रुव वर्ण *puny, वर्ण **output)
{
  वर्ण *d = रिक्त;
  CURLcode result = idn_encode(puny, &d);
#ifdef USE_LIBIDN2
  यदि(!result) {
    वर्ण *c = curlx_strdup(d);
    idn2_free(d);
    यदि(c)
      d = c;
    अन्यथा
      result = CURLE_OUT_OF_MEMORY;
  }
#endif
  यदि(!result)
    *output = d;
  प्रत्यागम result;
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
  यदि(!Curl_is_ASCII_name(host->name)) {
    वर्ण *decoded;
    CURLcode result = Curl_idn_decode(host->name, &decoded);
    यदि(result)
      प्रत्यागम result;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#endif
  प्रत्यागम CURLE_OK;
}
