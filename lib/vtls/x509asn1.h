#यदि_अपरिभाषितम् शीर्षकम्_CURL_X509_ASN1_H
#परिभाषय शीर्षकम्_CURL_X509_ASN1_H

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
#अन्तर्भाव "curl_setup.h"

/* forward-declare to make it visible in all builds */
संरचना Curl_asn1Element;

#यदि defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "urldata.h"

/* ASN.1 classes. */
/* #define CURL_ASN1_UNIVERSAL             0 */
/* #define CURL_ASN1_APPLICATION           1 */
/* #define CURL_ASN1_CONTEXT_SPECIFIC      2 */
/* #define CURL_ASN1_PRIVATE               3 */

/* ASN.1 types. */
#परिभाषय CURL_ASN1_BOOLEAN               1
#परिभाषय CURL_ASN1_INTEGER               2
#परिभाषय CURL_ASN1_BIT_STRING            3
#परिभाषय CURL_ASN1_OCTET_STRING          4
#परिभाषय CURL_ASN1_NULL                  5
#परिभाषय CURL_ASN1_OBJECT_IDENTIFIER     6
/* #define CURL_ASN1_OBJECT_DESCRIPTOR     7 */
/* #define CURL_ASN1_INSTANCE_OF           8 */
/* #define CURL_ASN1_REAL                  9 */
#परिभाषय CURL_ASN1_ENUMERATED            10
/* #define CURL_ASN1_EMBEDDED              11 */
#परिभाषय CURL_ASN1_UTF8_STRING           12
/* #define CURL_ASN1_RELATIVE_OID          13 */
/* #define CURL_ASN1_SEQUENCE              16 */
/* #define CURL_ASN1_SET                   17 */
#परिभाषय CURL_ASN1_NUMERIC_STRING        18
#परिभाषय CURL_ASN1_PRINTABLE_STRING      19
#परिभाषय CURL_ASN1_TELETEX_STRING        20
/* #define CURL_ASN1_VIDEOTEX_STRING       21 */
#परिभाषय CURL_ASN1_IA5_STRING            22
#परिभाषय CURL_ASN1_UTC_TIME              23
#परिभाषय CURL_ASN1_GENERALIZED_TIME      24
/* #define CURL_ASN1_GRAPHIC_STRING        25 */
#परिभाषय CURL_ASN1_VISIBLE_STRING        26
/* #define CURL_ASN1_GENERAL_STRING        27 */
#परिभाषय CURL_ASN1_UNIVERSAL_STRING      28
/* #define CURL_ASN1_CHARACTER_STRING      29 */
#परिभाषय CURL_ASN1_BMP_STRING            30

/*
 * Types.
 */

/* ASN.1 parsed element. */
संरचना Curl_asn1Element {
  ध्रुव uint8_t *शीर्षकम्;      /* Pointer to header byte. */
  ध्रुव uint8_t *beg;         /* Pointer to element data. */
  ध्रुव uint8_t *अन्तः;         /* Pointer to 1st byte after element. */
  uint8_t eclass;             /* ASN.1 element class. */
  uint8_t चिह्नकम्;                /* ASN.1 element tag. */
  BIT(constructed);           /* Element is constructed. */
};

/* X509 certificate: RFC 5280. */
संरचना Curl_X509certificate {
  संरचना Curl_asn1Element certificate;
  संरचना Curl_asn1Element संस्करणम्_2;
  संरचना Curl_asn1Element serial_सङ्ख्या;
  संरचना Curl_asn1Element signatureAlgorithm;
  संरचना Curl_asn1Element signature;
  संरचना Curl_asn1Element issuer;
  संरचना Curl_asn1Element notBefore;
  संरचना Curl_asn1Element notAfter;
  संरचना Curl_asn1Element subject;
  संरचना Curl_asn1Element subject_लौकिकम्_कुञ्चिका_Info;
  संरचना Curl_asn1Element subject_लौकिकम्_कुञ्चिका_Algorithm;
  संरचना Curl_asn1Element subject_लौकिकम्_कुञ्चिका;
  संरचना Curl_asn1Element issuerUniqueID;
  संरचना Curl_asn1Element subjectUniqueID;
  संरचना Curl_asn1Element extensions;
};

/*
 * Prototypes.
 */

पूर्णाङ्क Curl_parseX509(संरचना Curl_X509certificate *cert,
                   ध्रुव uint8_t *beg, ध्रुव uint8_t *अन्तः);
CURLcode Curl_extract_certinfo(संरचना Curl_easy *दत्त, पूर्णाङ्क certnum,
                               ध्रुव uint8_t *beg, ध्रुव uint8_t *अन्तः);
CURLcode Curl_verifyhost(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                         ध्रुव uint8_t *beg, ध्रुव uint8_t *अन्तः);
#यदि_अन्तः /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
#यदि_अन्तः /* HEADER_CURL_X509ASN1_H */
