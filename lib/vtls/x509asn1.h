#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_X509_ASN1_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_X509_ASN1_H

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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

/* forward-declare to make it visible in all builds */
ਬਣਤਰ Curl_asn1Element;

#ਜੇ defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"

/* ASN.1 classes. */
/* #define CURL_ASN1_UNIVERSAL             0 */
/* #define CURL_ASN1_APPLICATION           1 */
/* #define CURL_ASN1_CONTEXT_SPECIFIC      2 */
/* #define CURL_ASN1_PRIVATE               3 */

/* ASN.1 types. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ASN1_BOOLEAN               1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ASN1_INTEGER               2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ASN1_ਦੋਆਧਾਰੀ_ਅੰਕ_STRING            3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ASN1_OCTET_STRING          4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ASN1_NULL                  5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ASN1_OBJECT_IDENTIFIER     6
/* #define CURL_ASN1_OBJECT_DESCRIPTOR     7 */
/* #define CURL_ASN1_INSTANCE_OF           8 */
/* #define CURL_ASN1_REAL                  9 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ASN1_ENUMERATED            10
/* #define CURL_ASN1_EMBEDDED              11 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ASN1_UTF8_STRING           12
/* #define CURL_ASN1_RELATIVE_OID          13 */
/* #define CURL_ASN1_SEQUENCE              16 */
/* #define CURL_ASN1_SET                   17 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ASN1_NUMERIC_STRING        18
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ASN1_PRINTABLE_STRING      19
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ASN1_TELETEX_STRING        20
/* #define CURL_ASN1_VIDEOTEX_STRING       21 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ASN1_IA5_STRING            22
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ASN1_UTC_ਸਮਾਂ              23
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ASN1_GENERALIZED_ਸਮਾਂ      24
/* #define CURL_ASN1_GRAPHIC_STRING        25 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ASN1_VISIBLE_STRING        26
/* #define CURL_ASN1_GENERAL_STRING        27 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ASN1_UNIVERSAL_STRING      28
/* #define CURL_ASN1_CHARACTER_STRING      29 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ASN1_BMP_STRING            30

/*
 * Types.
 */

/* ASN.1 parsed element. */
ਬਣਤਰ Curl_asn1Element {
  ਅਚਲ uint8_t *ਸਿਰਲੇਖ;      /* Pointer to header byte. */
  ਅਚਲ uint8_t *beg;         /* Pointer to element data. */
  ਅਚਲ uint8_t *ਸਿਰਾ;         /* Pointer to 1st byte after element. */
  uint8_t eclass;             /* ASN.1 element class. */
  uint8_t ਟੈਗ;                /* ASN.1 element tag. */
  ਦੋਆਧਾਰੀ_ਅੰਕ(constructed);           /* Element is constructed. */
};

/* X509 certificate: RFC 5280. */
ਬਣਤਰ Curl_X509certificate {
  ਬਣਤਰ Curl_asn1Element certificate;
  ਬਣਤਰ Curl_asn1Element ਸੰਸਕਰਣ_2;
  ਬਣਤਰ Curl_asn1Element serial_ਸੰਖਿਆ;
  ਬਣਤਰ Curl_asn1Element signatureAlgorithm;
  ਬਣਤਰ Curl_asn1Element signature;
  ਬਣਤਰ Curl_asn1Element issuer;
  ਬਣਤਰ Curl_asn1Element notBefore;
  ਬਣਤਰ Curl_asn1Element notAfter;
  ਬਣਤਰ Curl_asn1Element subject;
  ਬਣਤਰ Curl_asn1Element subject_ਜਨਤਕ_ਕੁੰਜੀ_Info;
  ਬਣਤਰ Curl_asn1Element subject_ਜਨਤਕ_ਕੁੰਜੀ_Algorithm;
  ਬਣਤਰ Curl_asn1Element subject_ਜਨਤਕ_ਕੁੰਜੀ;
  ਬਣਤਰ Curl_asn1Element issuerUniqueID;
  ਬਣਤਰ Curl_asn1Element subjectUniqueID;
  ਬਣਤਰ Curl_asn1Element extensions;
};

/*
 * Prototypes.
 */

ਪੂਰਨ Curl_parseX509(ਬਣਤਰ Curl_X509certificate *cert,
                   ਅਚਲ uint8_t *beg, ਅਚਲ uint8_t *ਸਿਰਾ);
CURLcode Curl_ਕੱਢੋ_certinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ certnum,
                               ਅਚਲ uint8_t *beg, ਅਚਲ uint8_t *ਸਿਰਾ);
CURLcode Curl_verifyhost(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਅਚਲ uint8_t *beg, ਅਚਲ uint8_t *ਸਿਰਾ);
#ਜੇ_ਅੰਤ /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
#ਜੇ_ਅੰਤ /* HEADER_CURL_X509ASN1_H */
