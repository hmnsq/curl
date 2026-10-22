#ifndef HEADER_CURL_X509ASN1_H
#define HEADER_CURL_X509ASN1_H

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
ساختار Curl_asn1Element;

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
#define CURL_ASN1_BOOLEAN               1
#define CURL_ASN1_INTEGER               2
#define CURL_ASN1_BIT_STRING            3
#define CURL_ASN1_OCTET_STRING          4
#define CURL_ASN1_NULL                  5
#define CURL_ASN1_OBJECT_IDENTIFIER     6
/* #define CURL_ASN1_OBJECT_DESCRIPTOR     7 */
/* #define CURL_ASN1_INSTANCE_OF           8 */
/* #define CURL_ASN1_REAL                  9 */
#define CURL_ASN1_ENUMERATED            10
/* #define CURL_ASN1_EMBEDDED              11 */
#define CURL_ASN1_UTF8_STRING           12
/* #define CURL_ASN1_RELATIVE_OID          13 */
/* #define CURL_ASN1_SEQUENCE              16 */
/* #define CURL_ASN1_SET                   17 */
#define CURL_ASN1_NUMERIC_STRING        18
#define CURL_ASN1_PRINTABLE_STRING      19
#define CURL_ASN1_TELETEX_STRING        20
/* #define CURL_ASN1_VIDEOTEX_STRING       21 */
#define CURL_ASN1_IA5_STRING            22
#define CURL_ASN1_UTC_TIME              23
#define CURL_ASN1_GENERALIZED_TIME      24
/* #define CURL_ASN1_GRAPHIC_STRING        25 */
#define CURL_ASN1_VISIBLE_STRING        26
/* #define CURL_ASN1_GENERAL_STRING        27 */
#define CURL_ASN1_UNIVERSAL_STRING      28
/* #define CURL_ASN1_CHARACTER_STRING      29 */
#define CURL_ASN1_BMP_STRING            30

/*
 * Types.
 */

/* ASN.1 parsed element. */
ساختار Curl_asn1Element {
  ثابت uint8_t *header;      /* Pointer to header byte. */
  ثابت uint8_t *beg;         /* Pointer to element data. */
  ثابت uint8_t *end;         /* Pointer to 1st byte after element. */
  uint8_t eclass;             /* ASN.1 element class. */
  uint8_t tag;                /* ASN.1 element tag. */
  BIT(constructed);           /* Element is constructed. */
};

/* X509 certificate: RFC 5280. */
ساختار Curl_X509certificate {
  ساختار Curl_asn1Element certificate;
  ساختار Curl_asn1Element version;
  ساختار Curl_asn1Element serial_عدد;
  ساختار Curl_asn1Element signatureAlgorithm;
  ساختار Curl_asn1Element signature;
  ساختار Curl_asn1Element issuer;
  ساختار Curl_asn1Element notBefore;
  ساختار Curl_asn1Element notAfter;
  ساختار Curl_asn1Element subject;
  ساختار Curl_asn1Element subject_Public_کلید_Info;
  ساختار Curl_asn1Element subject_Public_کلید_Algorithm;
  ساختار Curl_asn1Element subject_Public_کلید;
  ساختار Curl_asn1Element issuerUniqueID;
  ساختار Curl_asn1Element subjectUniqueID;
  ساختار Curl_asn1Element extensions;
};

/*
 * Prototypes.
 */

صحیح Curl_parseX509(ساختار Curl_X509certificate *cert,
                   ثابت uint8_t *beg, ثابت uint8_t *end);
CURLcode Curl_extract_certinfo(ساختار Curl_easy *data, صحیح certnum,
                               ثابت uint8_t *beg, ثابت uint8_t *end);
CURLcode Curl_verifyhost(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                         ثابت uint8_t *beg, ثابت uint8_t *end);
#endif /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
#endif /* HEADER_CURL_X509ASN1_H */
