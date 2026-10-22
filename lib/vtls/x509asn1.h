#నిర్వచితం_కాకపోతే శీర్షిక_CURL_X509_ASN1_H
#నిర్వచించు శీర్షిక_CURL_X509_ASN1_H

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
#అన్తర్భావ "curl_setup.h"

/* forward-declare to make it visible in all builds */
నిర్మాణం Curl_asn1Element;

#ఒకవేళ defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "urldata.h"

/* ASN.1 classes. */
/* #define CURL_ASN1_UNIVERSAL             0 */
/* #define CURL_ASN1_APPLICATION           1 */
/* #define CURL_ASN1_CONTEXT_SPECIFIC      2 */
/* #define CURL_ASN1_PRIVATE               3 */

/* ASN.1 types. */
#నిర్వచించు CURL_ASN1_BOOLEAN               1
#నిర్వచించు CURL_ASN1_INTEGER               2
#నిర్వచించు CURL_ASN1_బిట్టు_STRING            3
#నిర్వచించు CURL_ASN1_OCTET_STRING          4
#నిర్వచించు CURL_ASN1_NULL                  5
#నిర్వచించు CURL_ASN1_OBJECT_IDENTIFIER     6
/* #define CURL_ASN1_OBJECT_DESCRIPTOR     7 */
/* #define CURL_ASN1_INSTANCE_OF           8 */
/* #define CURL_ASN1_REAL                  9 */
#నిర్వచించు CURL_ASN1_ENUMERATED            10
/* #define CURL_ASN1_EMBEDDED              11 */
#నిర్వచించు CURL_ASN1_UTF8_STRING           12
/* #define CURL_ASN1_RELATIVE_OID          13 */
/* #define CURL_ASN1_SEQUENCE              16 */
/* #define CURL_ASN1_SET                   17 */
#నిర్వచించు CURL_ASN1_NUMERIC_STRING        18
#నిర్వచించు CURL_ASN1_PRINTABLE_STRING      19
#నిర్వచించు CURL_ASN1_TELETEX_STRING        20
/* #define CURL_ASN1_VIDEOTEX_STRING       21 */
#నిర్వచించు CURL_ASN1_IA5_STRING            22
#నిర్వచించు CURL_ASN1_UTC_సమయం              23
#నిర్వచించు CURL_ASN1_GENERALIZED_సమయం      24
/* #define CURL_ASN1_GRAPHIC_STRING        25 */
#నిర్వచించు CURL_ASN1_VISIBLE_STRING        26
/* #define CURL_ASN1_GENERAL_STRING        27 */
#నిర్వచించు CURL_ASN1_UNIVERSAL_STRING      28
/* #define CURL_ASN1_CHARACTER_STRING      29 */
#నిర్వచించు CURL_ASN1_BMP_STRING            30

/*
 * Types.
 */

/* ASN.1 parsed element. */
నిర్మాణం Curl_asn1Element {
  స్థిరరాశి uint8_t *శీర్షిక;      /* Pointer to header byte. */
  స్థిరరాశి uint8_t *beg;         /* Pointer to element data. */
  స్థిరరాశి uint8_t *చివర;         /* Pointer to 1st byte after element. */
  uint8_t eclass;             /* ASN.1 element class. */
  uint8_t ట్యాగ్;                /* ASN.1 element tag. */
  బిట్టు(constructed);           /* Element is constructed. */
};

/* X509 certificate: RFC 5280. */
నిర్మాణం Curl_X509certificate {
  నిర్మాణం Curl_asn1Element certificate;
  నిర్మాణం Curl_asn1Element సంస్కరణ_2;
  నిర్మాణం Curl_asn1Element serial_సంఖ్య;
  నిర్మాణం Curl_asn1Element signatureAlgorithm;
  నిర్మాణం Curl_asn1Element signature;
  నిర్మాణం Curl_asn1Element issuer;
  నిర్మాణం Curl_asn1Element notBefore;
  నిర్మాణం Curl_asn1Element notAfter;
  నిర్మాణం Curl_asn1Element subject;
  నిర్మాణం Curl_asn1Element subject_ప్రజా_కీలకం_Info;
  నిర్మాణం Curl_asn1Element subject_ప్రజా_కీలకం_Algorithm;
  నిర్మాణం Curl_asn1Element subject_ప్రజా_కీలకం;
  నిర్మాణం Curl_asn1Element issuerUniqueID;
  నిర్మాణం Curl_asn1Element subjectUniqueID;
  నిర్మాణం Curl_asn1Element extensions;
};

/*
 * Prototypes.
 */

పూర్ణాంకం Curl_parseX509(నిర్మాణం Curl_X509certificate *cert,
                   స్థిరరాశి uint8_t *beg, స్థిరరాశి uint8_t *చివర);
CURLcode Curl_వెలికితీయి_certinfo(నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం certnum,
                               స్థిరరాశి uint8_t *beg, స్థిరరాశి uint8_t *చివర);
CURLcode Curl_verifyhost(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                         స్థిరరాశి uint8_t *beg, స్థిరరాశి uint8_t *చివర);
#అయితే_ముగింపు /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
#అయితే_ముగింపు /* HEADER_CURL_X509ASN1_H */
