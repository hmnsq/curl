#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_X509_ASN1_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_X509_ASN1_H

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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

/* forward-declare to make it visible in all builds */
ರಚನೆ Curl_asn1Element;

#ಆದರೆ defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"

/* ASN.1 classes. */
/* #define CURL_ASN1_UNIVERSAL             0 */
/* #define CURL_ASN1_APPLICATION           1 */
/* #define CURL_ASN1_CONTEXT_SPECIFIC      2 */
/* #define CURL_ASN1_PRIVATE               3 */

/* ASN.1 types. */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ASN1_BOOLEAN               1
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ASN1_INTEGER               2
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ASN1_ಬಿಟ್_STRING            3
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ASN1_OCTET_STRING          4
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ASN1_NULL                  5
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ASN1_OBJECT_IDENTIFIER     6
/* #define CURL_ASN1_OBJECT_DESCRIPTOR     7 */
/* #define CURL_ASN1_INSTANCE_OF           8 */
/* #define CURL_ASN1_REAL                  9 */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ASN1_ENUMERATED            10
/* #define CURL_ASN1_EMBEDDED              11 */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ASN1_UTF8_STRING           12
/* #define CURL_ASN1_RELATIVE_OID          13 */
/* #define CURL_ASN1_SEQUENCE              16 */
/* #define CURL_ASN1_SET                   17 */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ASN1_NUMERIC_STRING        18
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ASN1_PRINTABLE_STRING      19
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ASN1_TELETEX_STRING        20
/* #define CURL_ASN1_VIDEOTEX_STRING       21 */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ASN1_IA5_STRING            22
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ASN1_UTC_ಸಮಯ              23
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ASN1_GENERALIZED_ಸಮಯ      24
/* #define CURL_ASN1_GRAPHIC_STRING        25 */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ASN1_VISIBLE_STRING        26
/* #define CURL_ASN1_GENERAL_STRING        27 */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ASN1_UNIVERSAL_STRING      28
/* #define CURL_ASN1_CHARACTER_STRING      29 */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ASN1_BMP_STRING            30

/*
 * Types.
 */

/* ASN.1 parsed element. */
ರಚನೆ Curl_asn1Element {
  ಸ್ಥಿರಾಂಕ uint8_t *ಶೀರ್ಷಿಕೆ;      /* Pointer to header byte. */
  ಸ್ಥಿರಾಂಕ uint8_t *beg;         /* Pointer to element data. */
  ಸ್ಥಿರಾಂಕ uint8_t *ಕೊನೆ;         /* Pointer to 1st byte after element. */
  uint8_t eclass;             /* ASN.1 element class. */
  uint8_t ಟ್ಯಾಗ್;                /* ASN.1 element tag. */
  ಬಿಟ್(constructed);           /* Element is constructed. */
};

/* X509 certificate: RFC 5280. */
ರಚನೆ Curl_X509certificate {
  ರಚನೆ Curl_asn1Element certificate;
  ರಚನೆ Curl_asn1Element ಆವೃತ್ತಿ_2;
  ರಚನೆ Curl_asn1Element serial_ಸಂಖ್ಯೆ;
  ರಚನೆ Curl_asn1Element signatureAlgorithm;
  ರಚನೆ Curl_asn1Element signature;
  ರಚನೆ Curl_asn1Element issuer;
  ರಚನೆ Curl_asn1Element notBefore;
  ರಚನೆ Curl_asn1Element notAfter;
  ರಚನೆ Curl_asn1Element subject;
  ರಚನೆ Curl_asn1Element subject_ಬಹಿರಂಗ_ಕೀಲಿ_Info;
  ರಚನೆ Curl_asn1Element subject_ಬಹಿರಂಗ_ಕೀಲಿ_Algorithm;
  ರಚನೆ Curl_asn1Element subject_ಬಹಿರಂಗ_ಕೀಲಿ;
  ರಚನೆ Curl_asn1Element issuerUniqueID;
  ರಚನೆ Curl_asn1Element subjectUniqueID;
  ರಚನೆ Curl_asn1Element extensions;
};

/*
 * Prototypes.
 */

ಪೂರ್ಣಾಂಕ Curl_parseX509(ರಚನೆ Curl_X509certificate *cert,
                   ಸ್ಥಿರಾಂಕ uint8_t *beg, ಸ್ಥಿರಾಂಕ uint8_t *ಕೊನೆ);
CURLcode Curl_ಹೊರತೆಗೆ_certinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ certnum,
                               ಸ್ಥಿರಾಂಕ uint8_t *beg, ಸ್ಥಿರಾಂಕ uint8_t *ಕೊನೆ);
CURLcode Curl_verifyhost(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ಸ್ಥಿರಾಂಕ uint8_t *beg, ಸ್ಥಿರಾಂಕ uint8_t *ಕೊನೆ);
#ಆದರೆ_ಅಂತ್ಯ /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_X509ASN1_H */
