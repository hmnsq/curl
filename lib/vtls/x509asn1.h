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
#અન્તર્ભાવ "curl_setup.h"

/* forward-declare to make it visible in all builds */
સંરચના Curl_asn1Element;

#જો defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "urldata.h"

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
સંરચના Curl_asn1Element {
  અચલ uint8_t *header;      /* Pointer to header byte. */
  અચલ uint8_t *beg;         /* Pointer to element data. */
  અચલ uint8_t *end;         /* Pointer to 1st byte after element. */
  uint8_t eclass;             /* ASN.1 element class. */
  uint8_t tag;                /* ASN.1 element tag. */
  BIT(constructed);           /* Element is constructed. */
};

/* X509 certificate: RFC 5280. */
સંરચના Curl_X509certificate {
  સંરચના Curl_asn1Element certificate;
  સંરચના Curl_asn1Element version;
  સંરચના Curl_asn1Element serial_સંખ્યા;
  સંરચના Curl_asn1Element signatureAlgorithm;
  સંરચના Curl_asn1Element signature;
  સંરચના Curl_asn1Element issuer;
  સંરચના Curl_asn1Element notBefore;
  સંરચના Curl_asn1Element notAfter;
  સંરચના Curl_asn1Element subject;
  સંરચના Curl_asn1Element subject_Public_ચાવી_Info;
  સંરચના Curl_asn1Element subject_Public_ચાવી_Algorithm;
  સંરચના Curl_asn1Element subject_Public_ચાવી;
  સંરચના Curl_asn1Element issuerUniqueID;
  સંરચના Curl_asn1Element subjectUniqueID;
  સંરચના Curl_asn1Element extensions;
};

/*
 * Prototypes.
 */

પૂર્ણાંક Curl_parseX509(સંરચના Curl_X509certificate *cert,
                   અચલ uint8_t *beg, અચલ uint8_t *end);
CURLcode Curl_extract_certinfo(સંરચના Curl_easy *data, પૂર્ણાંક certnum,
                               અચલ uint8_t *beg, અચલ uint8_t *end);
CURLcode Curl_verifyhost(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                         અચલ uint8_t *beg, અચલ uint8_t *end);
#endif /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
#endif /* HEADER_CURL_X509ASN1_H */
