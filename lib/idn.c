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

#यदि_परिभाषितम् USE_LIBIDN2
#अन्तर्भाव <idn2.h>

#यदि defined(_WIN32) && defined(UNICODE)
#परिभाषय IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((ध्रुव uint8_t *)name, (uint8_t **)host, flags)
#अन्यथा
#परिभाषय IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((ध्रुव वर्ण *)(name), (वर्ण **)(host), flags)
#यदि_अन्तः
#यदि_अन्तः /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#यदि_परिभाषितम् USE_APPLE_IDN
#अन्तर्भाव <unicode/uidna.h>
#अन्तर्भाव <iconv.h>
#अन्तर्भाव <langinfo.h>

#परिभाषय MAX_HOST_दीर्घता 512

स्थिर CURLcode iconv_गन्तव्यम्_utf8(ध्रुव वर्ण *अन्तर्, size_t inlen,
                              वर्ण **बहिर्गमनम्, size_t *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  यदि(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    वर्ण *iconv_अन्तर् = (वर्ण *)CURL_UNCONST(अन्तर्);
    size_t iconv_inlen = inlen;
    size_t iconv_परिणाम = iconv(cd, &iconv_अन्तर्, &iconv_inlen,
                                बहिर्गमनम्, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    यदि(iconv_परिणाम == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      यदि(errno == ENOMEM)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      अन्यथा
        प्रत्यागम CURLE_URL_MALFORMAT;
    }

    प्रत्यागम CURLE_OK;
  }
  अन्यथा {
    /* !checksrc! disable ERRNOVAR 1 */
    यदि(errno == ENOMEM)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    अन्यथा
      प्रत्यागम CURLE_FAILED_INIT;
  }
}

स्थिर CURLcode mac_idn_गन्तव्यम्_ascii(ध्रुव वर्ण *अन्तर्, वर्ण **बहिर्गमनम्)
{
  size_t inlen = वाक्यदैर्घ्यम्(अन्तर्);
  यदि(inlen < MAX_HOST_दीर्घता) {
    वर्ण iconv_बफर[MAX_HOST_दीर्घता] = { 0 };
    वर्ण *iconv_outptr = iconv_बफर;
    size_t iconv_outlen = परिमाण(iconv_बफर);
    CURLcode iconv_परिणाम = iconv_गन्तव्यम्_utf8(अन्तर्, inlen,
                                          &iconv_outptr, &iconv_outlen);
    यदि(!iconv_परिणाम) {
      UError_संहिता err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_गन्तव्यम्_ASCII, &err);
      यदि(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        वर्ण बफर[MAX_HOST_दीर्घता] = { 0 };
        (शून्य)uidna_name_गन्तव्यम्_ASCII_UTF8(idna, iconv_बफर, (पूर्णाङ्क)iconv_outlen,
                                     बफर, परिमाण(बफर) - 1, &info, &err);
        uidna_close(idna);
        यदि(!U_FAILURE(err) && !info.errors) {
          *बहिर्गमनम् = curlx_strdup(बफर);
          यदि(*बहिर्गमनम्)
            प्रत्यागम CURLE_OK;
          अन्यथा
            प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        }
      }
    }
    अन्यथा
      प्रत्यागम iconv_परिणाम;
  }
  प्रत्यागम CURLE_URL_MALFORMAT;
}

स्थिर CURLcode mac_ascii_गन्तव्यम्_idn(ध्रुव वर्ण *अन्तर्, वर्ण **बहिर्गमनम्)
{
  size_t inlen = वाक्यदैर्घ्यम्(अन्तर्);
  यदि(inlen < MAX_HOST_दीर्घता) {
    UError_संहिता err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_गन्तव्यम्_UNICODE, &err);
    यदि(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      वर्ण बफर[MAX_HOST_दीर्घता] = { 0 };
      (शून्य)uidna_name_गन्तव्यम्_Unicode_UTF8(idna, अन्तर्, -1, बफर,
                                    परिमाण(बफर) - 1, &info, &err);
      uidna_close(idna);
      यदि(!U_FAILURE(err)) {
        *बहिर्गमनम् = curlx_strdup(बफर);
        यदि(*बहिर्गमनम्)
          प्रत्यागम CURLE_OK;
        अन्यथा
          प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
    }
  }
  प्रत्यागम CURLE_URL_MALFORMAT;
}
#यदि_अन्तः

#यदि_परिभाषितम् USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#परिभाषय IDN_MAX_दीर्घता 255

स्थिर वर्ण *idn_curlx_convert_wchar_गन्तव्यम्_UTF8(ध्रुव wchar_t *str_w, पूर्णाङ्क chars)
{
  वर्ण *str_utf8 = रिक्त;
  पूर्णाङ्क अष्टकानि = Wide_Char_गन्तव्यम्_Multi_Byte(CP_UTF8, 0, str_w, chars, रिक्त, 0,
                                  रिक्त, रिक्त);
  यदि(अष्टकानि > 0) {
    str_utf8 = curlx_malloc(अष्टकानि);
    यदि(str_utf8) {
      यदि(Wide_Char_गन्तव्यम्_Multi_Byte(CP_UTF8, 0, str_w, chars, str_utf8, अष्टकानि,
                             रिक्त, रिक्त) == 0) {
        curlx_free(str_utf8);
        प्रत्यागम रिक्त;
      }
    }
  }
  प्रत्यागम str_utf8;
}

स्थिर CURLcode win32_idn_गन्तव्यम्_ascii(ध्रुव वर्ण *अन्तर्, वर्ण **बहिर्गमनम्)
{
  wchar_t अन्तर्_w[IDN_MAX_दीर्घता];
  पूर्णाङ्क अन्तर्_w_len;
  *बहिर्गमनम् = रिक्त;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  अन्तर्_w_len = Multi_Byte_गन्तव्यम्_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 अन्तर्, -1, अन्तर्_w, IDN_MAX_दीर्घता);
  यदि(अन्तर्_w_len) {
    wchar_t punycode[IDN_MAX_दीर्घता];
    पूर्णाङ्क chars = Idn_गन्तव्यम्_Ascii(0, अन्तर्_w, अन्तर्_w_len, punycode, IDN_MAX_दीर्घता);
    यदि(chars > 0) {
      *बहिर्गमनम् = idn_curlx_convert_wchar_गन्तव्यम्_UTF8(punycode, chars);
      यदि(!*बहिर्गमनम्)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
    अन्यथा
      प्रत्यागम CURLE_URL_MALFORMAT;
  }
  अन्यथा
    प्रत्यागम CURLE_URL_MALFORMAT;

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode win32_ascii_गन्तव्यम्_idn(ध्रुव वर्ण *अन्तर्, वर्ण **बहिर्गमनम्)
{
  wchar_t अन्तर्_w[IDN_MAX_दीर्घता];
  पूर्णाङ्क अन्तर्_w_len;
  *बहिर्गमनम् = रिक्त;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  अन्तर्_w_len = Multi_Byte_गन्तव्यम्_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 अन्तर्, -1, अन्तर्_w, IDN_MAX_दीर्घता);
  यदि(अन्तर्_w_len) {
    WCHAR idn[IDN_MAX_दीर्घता]; /* stores a UTF-16 string */
    पूर्णाङ्क chars = Idn_गन्तव्यम्_Unicode(0, अन्तर्_w, अन्तर्_w_len, idn, IDN_MAX_दीर्घता);
    यदि(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *बहिर्गमनम् = idn_curlx_convert_wchar_गन्तव्यम्_UTF8(idn, chars);
      यदि(!*बहिर्गमनम्)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
    अन्यथा
      प्रत्यागम CURLE_URL_MALFORMAT;
  }
  अन्यथा
    प्रत्यागम CURLE_URL_MALFORMAT;

  प्रत्यागम CURLE_OK;
}

#यदि_अन्तः /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(ध्रुव वर्ण *hostname)
{
  यदि(hostname) {
    संरचना Curl_str s;
    s.str = hostname;
    s.len = वाक्यदैर्घ्यम्(hostname);
    प्रत्यागम Curl_is_ASCII_str(&s);
  }
  प्रत्यागम TRUE;
}

bool Curl_is_ASCII_str(संरचना Curl_str *s)
{
  यदि(s && s->len) {
    ध्रुव अचिह्नित वर्ण *ch = (ध्रुव अचिह्नित वर्ण *)s->str;
    size_t i;
    प्रति(i = 0; i < s->len; ++i) {
      यदि(ch[i] & 0x80)
        प्रत्यागम FALSE;
    }
  }
  प्रत्यागम TRUE;
}

#यदि_परिभाषितम् USE_IDN
/*
 * Curl_idn_decode() returns an allocated IDN decoded string if it was
 * possible. NULL on error.
 *
 * CURLE_URL_MALFORMAT - the hostname could not be converted
 * CURLE_OUT_OF_MEMORY - memory problem
 *
 */
स्थिर CURLcode idn_decode(ध्रुव वर्ण *निवेशः, वर्ण **output)
{
  वर्ण *decoded = रिक्त;
  CURLcode परिणाम = CURLE_OK;
#यदि_परिभाषितम् USE_LIBIDN2
  यदि(idn2_check_संस्करणम्(IDN2_संस्करणम्)) {
    पूर्णाङ्क flags = IDN2_NFC_निवेशः
#यदि IDN2_संस्करणम्_सङ्ख्या >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#यदि_अन्तः
      ;
    पूर्णाङ्क rc = IDN2_LOOKUP(निवेशः, &decoded, flags);
    यदि(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(निवेशः, &decoded, IDN2_TRANSITIONAL);
    यदि(rc != IDN2_OK)
      परिणाम = CURLE_URL_MALFORMAT;
  }
  अन्यथा
    /* a too old libidn2 version */
    परिणाम = CURLE_NOT_BUILT_अन्तर्;
#नोचेद्यदि defined(USE_WIN32_IDN)
  परिणाम = win32_idn_गन्तव्यम्_ascii(निवेशः, &decoded);
#नोचेद्यदि defined(USE_APPLE_IDN)
  परिणाम = mac_idn_गन्तव्यम्_ascii(निवेशः, &decoded);
#यदि_अन्तः
  यदि(!परिणाम)
    *output = decoded;
  प्रत्यागम परिणाम;
}

स्थिर CURLcode idn_encode(ध्रुव वर्ण *puny, वर्ण **output)
{
  वर्ण *enc = रिक्त;
#यदि_परिभाषितम् USE_LIBIDN2
  पूर्णाङ्क rc = idn2_गन्तव्यम्_unicode_8z8z(puny, &enc, 0);
  यदि(rc != IDNA_SUCCESS)
    प्रत्यागम rc == IDNA_MALLOC_ERROR ? CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः : CURLE_URL_MALFORMAT;
#नोचेद्यदि defined(USE_WIN32_IDN)
  CURLcode परिणाम = win32_ascii_गन्तव्यम्_idn(puny, &enc);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
#नोचेद्यदि defined(USE_APPLE_IDN)
  CURLcode परिणाम = mac_ascii_गन्तव्यम्_idn(puny, &enc);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
#यदि_अन्तः
  *output = enc;
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_idn_decode(ध्रुव वर्ण *निवेशः, वर्ण **output)
{
  वर्ण *d = रिक्त;
  CURLcode परिणाम = idn_decode(निवेशः, &d);
#यदि_परिभाषितम् USE_LIBIDN2
  यदि(!परिणाम) {
    वर्ण *c = curlx_strdup(d);
    idn2_free(d);
    यदि(c)
      d = c;
    अन्यथा
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
#यदि_अन्तः
  यदि(!परिणाम) {
    यदि(!d[0]) { /* ended up zero length, not acceptable */
      परिणाम = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    अन्यथा
      *output = d;
  }
  प्रत्यागम परिणाम;
}

CURLcode Curl_idn_encode(ध्रुव वर्ण *puny, वर्ण **output)
{
  वर्ण *d = रिक्त;
  CURLcode परिणाम = idn_encode(puny, &d);
#यदि_परिभाषितम् USE_LIBIDN2
  यदि(!परिणाम) {
    वर्ण *c = curlx_strdup(d);
    idn2_free(d);
    यदि(c)
      d = c;
    अन्यथा
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
#यदि_अन्तः
  यदि(!परिणाम)
    *output = d;
  प्रत्यागम परिणाम;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
शून्य Curl_free_idnconverted_hostname(संरचना hostname *host)
{
  curlx_safefree(host->encalloc);
}

#यदि_अन्तः /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(संरचना hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#यदि_परिभाषितम् USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  यदि(!Curl_is_ASCII_name(host->name)) {
    वर्ण *decoded;
    CURLcode परिणाम = Curl_idn_decode(host->name, &decoded);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#यदि_अन्तः
  प्रत्यागम CURLE_OK;
}
