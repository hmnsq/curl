#अगर_अघोषित HEADER_CURL_X509ASN1_H
#घोषणा HEADER_CURL_X509ASN1_H

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
#समावेश "curl_setup.h"

/* forward-declare to make it visible in all builds */
काष्ठा Curl_asn1Element;

#अगर defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#समावेश "cfilters.h"
#समावेश "urldata.h"

/* ASN.1 classes. */
/* #define CURL_ASN1_UNIVERSAL             0 */
/* #define CURL_ASN1_APPLICATION           1 */
/* #define CURL_ASN1_CONTEXT_SPECIFIC      2 */
/* #define CURL_ASN1_PRIVATE               3 */

/* ASN.1 types. */
#घोषणा CURL_ASN1_BOOLEAN               1
#घोषणा CURL_ASN1_INTEGER               2
#घोषणा CURL_ASN1_BIT_STRING            3
#घोषणा CURL_ASN1_OCTET_STRING          4
#घोषणा CURL_ASN1_NULL                  5
#घोषणा CURL_ASN1_OBJECT_IDENTIFIER     6
/* #define CURL_ASN1_OBJECT_DESCRIPTOR     7 */
/* #define CURL_ASN1_INSTANCE_OF           8 */
/* #define CURL_ASN1_REAL                  9 */
#घोषणा CURL_ASN1_ENUMERATED            10
/* #define CURL_ASN1_EMBEDDED              11 */
#घोषणा CURL_ASN1_UTF8_STRING           12
/* #define CURL_ASN1_RELATIVE_OID          13 */
/* #define CURL_ASN1_SEQUENCE              16 */
/* #define CURL_ASN1_SET                   17 */
#घोषणा CURL_ASN1_NUMERIC_STRING        18
#घोषणा CURL_ASN1_PRINTABLE_STRING      19
#घोषणा CURL_ASN1_TELETEX_STRING        20
/* #define CURL_ASN1_VIDEOTEX_STRING       21 */
#घोषणा CURL_ASN1_IA5_STRING            22
#घोषणा CURL_ASN1_UTC_TIME              23
#घोषणा CURL_ASN1_GENERALIZED_TIME      24
/* #define CURL_ASN1_GRAPHIC_STRING        25 */
#घोषणा CURL_ASN1_VISIBLE_STRING        26
/* #define CURL_ASN1_GENERAL_STRING        27 */
#घोषणा CURL_ASN1_UNIVERSAL_STRING      28
/* #define CURL_ASN1_CHARACTER_STRING      29 */
#घोषणा CURL_ASN1_BMP_STRING            30

/*
 * Types.
 */

/* ASN.1 parsed element. */
काष्ठा Curl_asn1Element {
  स्थिर uint8_t *header;      /* Pointer to header byte. */
  स्थिर uint8_t *beg;         /* Pointer to element data. */
  स्थिर uint8_t *end;         /* Pointer to 1st byte after element. */
  uint8_t eclass;             /* ASN.1 element class. */
  uint8_t tag;                /* ASN.1 element tag. */
  BIT(constructed);           /* Element is constructed. */
};

/* X509 certificate: RFC 5280. */
काष्ठा Curl_X509certificate {
  काष्ठा Curl_asn1Element certificate;
  काष्ठा Curl_asn1Element version;
  काष्ठा Curl_asn1Element serial_संख्या;
  काष्ठा Curl_asn1Element signatureAlgorithm;
  काष्ठा Curl_asn1Element signature;
  काष्ठा Curl_asn1Element issuer;
  काष्ठा Curl_asn1Element notBefore;
  काष्ठा Curl_asn1Element notAfter;
  काष्ठा Curl_asn1Element subject;
  काष्ठा Curl_asn1Element subject_Public_कुंजी_Info;
  काष्ठा Curl_asn1Element subject_Public_कुंजी_Algorithm;
  काष्ठा Curl_asn1Element subject_Public_कुंजी;
  काष्ठा Curl_asn1Element issuerUniqueID;
  काष्ठा Curl_asn1Element subjectUniqueID;
  काष्ठा Curl_asn1Element extensions;
};

/*
 * Prototypes.
 */

पूर्णांक Curl_parseX509(काष्ठा Curl_X509certificate *cert,
                   स्थिर uint8_t *beg, स्थिर uint8_t *end);
CURLcode Curl_extract_certinfo(काष्ठा Curl_easy *तथ्य, पूर्णांक certnum,
                               स्थिर uint8_t *beg, स्थिर uint8_t *end);
CURLcode Curl_verifyhost(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                         स्थिर uint8_t *beg, स्थिर uint8_t *end);
#पूर्ण_अगर /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
#पूर्ण_अगर /* HEADER_CURL_X509ASN1_H */
