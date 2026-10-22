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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "idn.h"

#నిర్వచితమైతే USE_LIBIDN2
#అన్తర్భావ <idn2.h>

#ఒకవేళ defined(_WIN32) && defined(UNICODE)
#నిర్వచించు IDN2_LOOKUP(name, host, flags)                           \
  idn2_lookup_u8((స్థిరరాశి uint8_t *)name, (uint8_t **)host, flags)
#లేకపోతే
#నిర్వచించు IDN2_LOOKUP(name, host, flags)                         \
  idn2_lookup_ul((స్థిరరాశి అక్షరం *)(name), (అక్షరం **)(host), flags)
#అయితే_ముగింపు
#అయితే_ముగింపు /* USE_LIBIDN2 */

/* for macOS and iOS targets */
#నిర్వచితమైతే USE_APPLE_IDN
#అన్తర్భావ <unicode/uidna.h>
#అన్తర్భావ <iconv.h>
#అన్తర్భావ <langinfo.h>

#నిర్వచించు MAX_HOST_నిడివి 512

స్థిర CURLcode iconv_గమ్యం_utf8(స్థిరరాశి అక్షరం *లోపల, size_t inlen,
                              అక్షరం **బహిర్గతం, size_t *outlen)
{
  iconv_t cd = iconv_open("UTF-8", nl_langinfo(CODESET));
  ఒకవేళ(cd != (iconv_t)-1) {
    size_t iconv_outlen = *outlen;
    అక్షరం *iconv_లోపల = (అక్షరం *)CURL_UNCONST(లోపల);
    size_t iconv_inlen = inlen;
    size_t iconv_ఫలితం = iconv(cd, &iconv_లోపల, &iconv_inlen,
                                బహిర్గతం, &iconv_outlen);
    *outlen -= iconv_outlen;
    iconv_close(cd);
    ఒకవేళ(iconv_ఫలితం == (size_t)-1) {
      /* !checksrc! disable ERRNOVAR 1 */
      ఒకవేళ(errno == ENOMEM)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      లేకపోతే
        తిరిగి CURLE_URL_MALFORMAT;
    }

    తిరిగి CURLE_OK;
  }
  లేకపోతే {
    /* !checksrc! disable ERRNOVAR 1 */
    ఒకవేళ(errno == ENOMEM)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    లేకపోతే
      తిరిగి CURLE_FAILED_INIT;
  }
}

స్థిర CURLcode mac_idn_గమ్యం_ascii(స్థిరరాశి అక్షరం *లోపల, అక్షరం **బహిర్గతం)
{
  size_t inlen = అక్షరశ్రేణి_పొడవు(లోపల);
  ఒకవేళ(inlen < MAX_HOST_నిడివి) {
    అక్షరం iconv_బఫరు[MAX_HOST_నిడివి] = { 0 };
    అక్షరం *iconv_outptr = iconv_బఫరు;
    size_t iconv_outlen = పరిమాణం(iconv_బఫరు);
    CURLcode iconv_ఫలితం = iconv_గమ్యం_utf8(లోపల, inlen,
                                          &iconv_outptr, &iconv_outlen);
    ఒకవేళ(!iconv_ఫలితం) {
      UError_సంకేతం err = U_ZERO_ERROR;
      UIDNA *idna = uidna_openUTS46(
        UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_గమ్యం_ASCII, &err);
      ఒకవేళ(!U_FAILURE(err)) {
        UIDNAInfo info = UIDNA_INFO_INITIALIZER;
        అక్షరం బఫరు[MAX_HOST_నిడివి] = { 0 };
        (శూన్య)uidna_name_గమ్యం_ASCII_UTF8(idna, iconv_బఫరు, (పూర్ణాంకం)iconv_outlen,
                                     బఫరు, పరిమాణం(బఫరు) - 1, &info, &err);
        uidna_close(idna);
        ఒకవేళ(!U_FAILURE(err) && !info.errors) {
          *బహిర్గతం = curlx_strdup(బఫరు);
          ఒకవేళ(*బహిర్గతం)
            తిరిగి CURLE_OK;
          లేకపోతే
            తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        }
      }
    }
    లేకపోతే
      తిరిగి iconv_ఫలితం;
  }
  తిరిగి CURLE_URL_MALFORMAT;
}

స్థిర CURLcode mac_ascii_గమ్యం_idn(స్థిరరాశి అక్షరం *లోపల, అక్షరం **బహిర్గతం)
{
  size_t inlen = అక్షరశ్రేణి_పొడవు(లోపల);
  ఒకవేళ(inlen < MAX_HOST_నిడివి) {
    UError_సంకేతం err = U_ZERO_ERROR;
    UIDNA *idna = uidna_openUTS46(
      UIDNA_CHECK_BIDI | UIDNA_NONTRANSITIONAL_గమ్యం_UNICODE, &err);
    ఒకవేళ(!U_FAILURE(err)) {
      UIDNAInfo info = UIDNA_INFO_INITIALIZER;
      అక్షరం బఫరు[MAX_HOST_నిడివి] = { 0 };
      (శూన్య)uidna_name_గమ్యం_Unicode_UTF8(idna, లోపల, -1, బఫరు,
                                    పరిమాణం(బఫరు) - 1, &info, &err);
      uidna_close(idna);
      ఒకవేళ(!U_FAILURE(err)) {
        *బహిర్గతం = curlx_strdup(బఫరు);
        ఒకవేళ(*బహిర్గతం)
          తిరిగి CURLE_OK;
        లేకపోతే
          తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
    }
  }
  తిరిగి CURLE_URL_MALFORMAT;
}
#అయితే_ముగింపు

#నిర్వచితమైతే USE_WIN32_IDN
/* using Windows kernel32 and normaliz libraries. */

#నిర్వచించు IDN_MAX_నిడివి 255

స్థిర అక్షరం *idn_curlx_convert_wchar_గమ్యం_UTF8(స్థిరరాశి wchar_t *str_w, పూర్ణాంకం chars)
{
  అక్షరం *str_utf8 = ఖాళీ;
  పూర్ణాంకం బైట్లు = Wide_Char_గమ్యం_Multi_Byte(CP_UTF8, 0, str_w, chars, ఖాళీ, 0,
                                  ఖాళీ, ఖాళీ);
  ఒకవేళ(బైట్లు > 0) {
    str_utf8 = curlx_malloc(బైట్లు);
    ఒకవేళ(str_utf8) {
      ఒకవేళ(Wide_Char_గమ్యం_Multi_Byte(CP_UTF8, 0, str_w, chars, str_utf8, బైట్లు,
                             ఖాళీ, ఖాళీ) == 0) {
        curlx_free(str_utf8);
        తిరిగి ఖాళీ;
      }
    }
  }
  తిరిగి str_utf8;
}

స్థిర CURLcode win32_idn_గమ్యం_ascii(స్థిరరాశి అక్షరం *లోపల, అక్షరం **బహిర్గతం)
{
  wchar_t లోపల_w[IDN_MAX_నిడివి];
  పూర్ణాంకం లోపల_w_len;
  *బహిర్గతం = ఖాళీ;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  లోపల_w_len = Multi_Byte_గమ్యం_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 లోపల, -1, లోపల_w, IDN_MAX_నిడివి);
  ఒకవేళ(లోపల_w_len) {
    wchar_t punycode[IDN_MAX_నిడివి];
    పూర్ణాంకం chars = Idn_గమ్యం_Ascii(0, లోపల_w, లోపల_w_len, punycode, IDN_MAX_నిడివి);
    ఒకవేళ(chars > 0) {
      *బహిర్గతం = idn_curlx_convert_wchar_గమ్యం_UTF8(punycode, chars);
      ఒకవేళ(!*బహిర్గతం)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    లేకపోతే
      తిరిగి CURLE_URL_MALFORMAT;
  }
  లేకపోతే
    తిరిగి CURLE_URL_MALFORMAT;

  తిరిగి CURLE_OK;
}

స్థిర CURLcode win32_ascii_గమ్యం_idn(స్థిరరాశి అక్షరం *లోపల, అక్షరం **బహిర్గతం)
{
  wchar_t లోపల_w[IDN_MAX_నిడివి];
  పూర్ణాంకం లోపల_w_len;
  *బహిర్గతం = ఖాళీ;
  /* Returned in_w_len includes the null-terminator, which then gets
     preserved across the calls that follow, ending up terminating
     the buffer returned to the caller. */
  లోపల_w_len = Multi_Byte_గమ్యం_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                 లోపల, -1, లోపల_w, IDN_MAX_నిడివి);
  ఒకవేళ(లోపల_w_len) {
    WCHAR idn[IDN_MAX_నిడివి]; /* stores a UTF-16 string */
    పూర్ణాంకం chars = Idn_గమ్యం_Unicode(0, లోపల_w, లోపల_w_len, idn, IDN_MAX_నిడివి);
    ఒకవేళ(chars > 0) {  /* 'chars' is "the number of characters retrieved" */
      *బహిర్గతం = idn_curlx_convert_wchar_గమ్యం_UTF8(idn, chars);
      ఒకవేళ(!*బహిర్గతం)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    లేకపోతే
      తిరిగి CURLE_URL_MALFORMAT;
  }
  లేకపోతే
    తిరిగి CURLE_URL_MALFORMAT;

  తిరిగి CURLE_OK;
}

#అయితే_ముగింపు /* USE_WIN32_IDN */

/*
 * Helpers for IDNA conversions.
 */
bool Curl_is_ASCII_name(స్థిరరాశి అక్షరం *hostname)
{
  ఒకవేళ(hostname) {
    నిర్మాణం Curl_str s;
    s.str = hostname;
    s.len = అక్షరశ్రేణి_పొడవు(hostname);
    తిరిగి Curl_is_ASCII_str(&s);
  }
  తిరిగి TRUE;
}

bool Curl_is_ASCII_str(నిర్మాణం Curl_str *s)
{
  ఒకవేళ(s && s->len) {
    స్థిరరాశి గుర్తులేని అక్షరం *ch = (స్థిరరాశి గుర్తులేని అక్షరం *)s->str;
    size_t i;
    కొరకు(i = 0; i < s->len; ++i) {
      ఒకవేళ(ch[i] & 0x80)
        తిరిగి FALSE;
    }
  }
  తిరిగి TRUE;
}

#నిర్వచితమైతే USE_IDN
/*
 * Curl_idn_decode() returns an allocated IDN decoded string if it was
 * possible. NULL on error.
 *
 * CURLE_URL_MALFORMAT - the hostname could not be converted
 * CURLE_OUT_OF_MEMORY - memory problem
 *
 */
స్థిర CURLcode idn_decode(స్థిరరాశి అక్షరం *ఆదానం, అక్షరం **output)
{
  అక్షరం *decoded = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;
#నిర్వచితమైతే USE_LIBIDN2
  ఒకవేళ(idn2_check_సంస్కరణ(IDN2_సంస్కరణ)) {
    పూర్ణాంకం flags = IDN2_NFC_ఆదానం
#ఒకవేళ IDN2_సంస్కరణ_సంఖ్య >= 0x00140000
      /* IDN2_NFC_INPUT: Normalize input string using normalization form C.
         IDN2_NONTRANSITIONAL: Perform Unicode TR46 non-transitional
         processing. */
      | IDN2_NONTRANSITIONAL
#అయితే_ముగింపు
      ;
    పూర్ణాంకం rc = IDN2_LOOKUP(ఆదానం, &decoded, flags);
    ఒకవేళ(rc != IDN2_OK)
      /* fallback to TR46 Transitional mode for better IDNA2003
         compatibility */
      rc = IDN2_LOOKUP(ఆదానం, &decoded, IDN2_TRANSITIONAL);
    ఒకవేళ(rc != IDN2_OK)
      ఫలితం = CURLE_URL_MALFORMAT;
  }
  లేకపోతే
    /* a too old libidn2 version */
    ఫలితం = CURLE_NOT_BUILT_లోపల;
#లేదావేళ defined(USE_WIN32_IDN)
  ఫలితం = win32_idn_గమ్యం_ascii(ఆదానం, &decoded);
#లేదావేళ defined(USE_APPLE_IDN)
  ఫలితం = mac_idn_గమ్యం_ascii(ఆదానం, &decoded);
#అయితే_ముగింపు
  ఒకవేళ(!ఫలితం)
    *output = decoded;
  తిరిగి ఫలితం;
}

స్థిర CURLcode idn_encode(స్థిరరాశి అక్షరం *puny, అక్షరం **output)
{
  అక్షరం *enc = ఖాళీ;
#నిర్వచితమైతే USE_LIBIDN2
  పూర్ణాంకం rc = idn2_గమ్యం_unicode_8z8z(puny, &enc, 0);
  ఒకవేళ(rc != IDNA_SUCCESS)
    తిరిగి rc == IDNA_MALLOC_ERROR ? CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం : CURLE_URL_MALFORMAT;
#లేదావేళ defined(USE_WIN32_IDN)
  CURLcode ఫలితం = win32_ascii_గమ్యం_idn(puny, &enc);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
#లేదావేళ defined(USE_APPLE_IDN)
  CURLcode ఫలితం = mac_ascii_గమ్యం_idn(puny, &enc);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
#అయితే_ముగింపు
  *output = enc;
  తిరిగి CURLE_OK;
}

CURLcode Curl_idn_decode(స్థిరరాశి అక్షరం *ఆదానం, అక్షరం **output)
{
  అక్షరం *d = ఖాళీ;
  CURLcode ఫలితం = idn_decode(ఆదానం, &d);
#నిర్వచితమైతే USE_LIBIDN2
  ఒకవేళ(!ఫలితం) {
    అక్షరం *c = curlx_strdup(d);
    idn2_free(d);
    ఒకవేళ(c)
      d = c;
    లేకపోతే
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
#అయితే_ముగింపు
  ఒకవేళ(!ఫలితం) {
    ఒకవేళ(!d[0]) { /* ended up zero length, not acceptable */
      ఫలితం = CURLE_URL_MALFORMAT;
      curlx_free(d);
    }
    లేకపోతే
      *output = d;
  }
  తిరిగి ఫలితం;
}

CURLcode Curl_idn_encode(స్థిరరాశి అక్షరం *puny, అక్షరం **output)
{
  అక్షరం *d = ఖాళీ;
  CURLcode ఫలితం = idn_encode(puny, &d);
#నిర్వచితమైతే USE_LIBIDN2
  ఒకవేళ(!ఫలితం) {
    అక్షరం *c = curlx_strdup(d);
    idn2_free(d);
    ఒకవేళ(c)
      d = c;
    లేకపోతే
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
#అయితే_ముగింపు
  ఒకవేళ(!ఫలితం)
    *output = d;
  తిరిగి ఫలితం;
}

/*
 * Frees data allocated by idnconvert_hostname()
 */
శూన్య Curl_free_idnconverted_hostname(నిర్మాణం hostname *host)
{
  curlx_safefree(host->encalloc);
}

#అయితే_ముగింపు /* USE_IDN */

/*
 * Perform any necessary IDN conversion of hostname
 */
CURLcode Curl_idnconvert_hostname(నిర్మాణం hostname *host)
{
  /* set the name we use to display the hostname */
  host->dispname = host->name;

#నిర్వచితమైతే USE_IDN
  /* Check name for non-ASCII and convert hostname if we can */
  ఒకవేళ(!Curl_is_ASCII_name(host->name)) {
    అక్షరం *decoded;
    CURLcode ఫలితం = Curl_idn_decode(host->name, &decoded);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    /* successful */
    host->name = host->encalloc = decoded;
  }
#అయితే_ముగింపు
  తిరిగి CURLE_OK;
}
