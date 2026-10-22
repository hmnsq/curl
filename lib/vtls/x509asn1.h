#اگر_غیر_تعریف_شدہ سرخی_CURL_X509_ASN1_H
#تعریف_کرو سرخی_CURL_X509_ASN1_H

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
#شامل "curl_setup.h"

/* forward-declare to make it visible in all builds */
ساخت Curl_asn1Element;

#اگر defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#شامل "cfilters.h"
#شامل "urldata.h"

/* ASN.1 classes. */
/* #define CURL_ASN1_UNIVERSAL             0 */
/* #define CURL_ASN1_APPLICATION           1 */
/* #define CURL_ASN1_CONTEXT_SPECIFIC      2 */
/* #define CURL_ASN1_PRIVATE               3 */

/* ASN.1 types. */
#تعریف_کرو CURL_ASN1_BOOLEAN               1
#تعریف_کرو CURL_ASN1_INTEGER               2
#تعریف_کرو CURL_ASN1_ثنائی_ہندسہ_STRING            3
#تعریف_کرو CURL_ASN1_OCTET_STRING          4
#تعریف_کرو CURL_ASN1_NULL                  5
#تعریف_کرو CURL_ASN1_OBJECT_IDENTIFIER     6
/* #define CURL_ASN1_OBJECT_DESCRIPTOR     7 */
/* #define CURL_ASN1_INSTANCE_OF           8 */
/* #define CURL_ASN1_REAL                  9 */
#تعریف_کرو CURL_ASN1_ENUMERATED            10
/* #define CURL_ASN1_EMBEDDED              11 */
#تعریف_کرو CURL_ASN1_UTF8_STRING           12
/* #define CURL_ASN1_RELATIVE_OID          13 */
/* #define CURL_ASN1_SEQUENCE              16 */
/* #define CURL_ASN1_SET                   17 */
#تعریف_کرو CURL_ASN1_NUMERIC_STRING        18
#تعریف_کرو CURL_ASN1_PRINTABLE_STRING      19
#تعریف_کرو CURL_ASN1_TELETEX_STRING        20
/* #define CURL_ASN1_VIDEOTEX_STRING       21 */
#تعریف_کرو CURL_ASN1_IA5_STRING            22
#تعریف_کرو CURL_ASN1_UTC_وقت              23
#تعریف_کرو CURL_ASN1_GENERALIZED_وقت      24
/* #define CURL_ASN1_GRAPHIC_STRING        25 */
#تعریف_کرو CURL_ASN1_VISIBLE_STRING        26
/* #define CURL_ASN1_GENERAL_STRING        27 */
#تعریف_کرو CURL_ASN1_UNIVERSAL_STRING      28
/* #define CURL_ASN1_CHARACTER_STRING      29 */
#تعریف_کرو CURL_ASN1_BMP_STRING            30

/*
 * Types.
 */

/* ASN.1 parsed element. */
ساخت Curl_asn1Element {
  ثابت uint8_t *سرخی;      /* Pointer to header byte. */
  ثابت uint8_t *beg;         /* Pointer to element data. */
  ثابت uint8_t *انتہا;         /* Pointer to 1st byte after element. */
  uint8_t eclass;             /* ASN.1 element class. */
  uint8_t لیبل;                /* ASN.1 element tag. */
  ثنائی_ہندسہ(constructed);           /* Element is constructed. */
};

/* X509 certificate: RFC 5280. */
ساخت Curl_X509certificate {
  ساخت Curl_asn1Element certificate;
  ساخت Curl_asn1Element نسخہ_2;
  ساخت Curl_asn1Element serial_نمبر;
  ساخت Curl_asn1Element signatureAlgorithm;
  ساخت Curl_asn1Element signature;
  ساخت Curl_asn1Element issuer;
  ساخت Curl_asn1Element notBefore;
  ساخت Curl_asn1Element notAfter;
  ساخت Curl_asn1Element subject;
  ساخت Curl_asn1Element subject_عوامی_کلید_Info;
  ساخت Curl_asn1Element subject_عوامی_کلید_Algorithm;
  ساخت Curl_asn1Element subject_عوامی_کلید;
  ساخت Curl_asn1Element issuerUniqueID;
  ساخت Curl_asn1Element subjectUniqueID;
  ساخت Curl_asn1Element extensions;
};

/*
 * Prototypes.
 */

صحیح Curl_parseX509(ساخت Curl_X509certificate *cert,
                   ثابت uint8_t *beg, ثابت uint8_t *انتہا);
CURLcode Curl_نکالو_certinfo(ساخت Curl_easy *ڈیٹا, صحیح certnum,
                               ثابت uint8_t *beg, ثابت uint8_t *انتہا);
CURLcode Curl_verifyhost(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                         ثابت uint8_t *beg, ثابت uint8_t *انتہا);
#اگر_ختم /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
#اگر_ختم /* HEADER_CURL_X509ASN1_H */
