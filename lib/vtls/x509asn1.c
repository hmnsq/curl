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

#ಆದರೆ defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#ಆದರೆ defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_RUSTLS)
#ವ್ಯಾಖ್ಯಾನಿಸು WANT_PARSEX509 /* uses Curl_parseX509() */
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_RUSTLS)
#ವ್ಯಾಖ್ಯಾನಿಸು WANT_ಹೊರತೆಗೆ_CERTINFO /* uses Curl_extract_certinfo() */
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#ಅನ್ತರ್ಭಾವ "vtls/x509asn1.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"

/*
 * Constants.
 */

/* Largest supported ASN.1 structure. */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ASN1_MAX                   ((size_t)0x40000)      /* 256K */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WANT_ಹೊರತೆಗೆ_CERTINFO

/* ASN.1 OID table entry. */
ರಚನೆ Curl_OID {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *numoid;  /* Dotted-numeric OID. */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *textoid; /* OID name. */
};

/* ASN.1 OIDs. */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_OID OIDtable[] = {
  { "1.2.840.10040.4.1",        "dsa" },
  { "1.2.840.10040.4.3",        "dsa-with-sha1" },
  { "1.2.840.10045.2.1",        "ecPublicKey" },
  { "1.2.840.10045.3.0.1",      "c2pnb163v1" },
  { "1.2.840.10045.4.1",        "ecdsa-with-SHA1" },
  { "1.2.840.10045.4.3.1",      "ecdsa-with-SHA224" },
  { "1.2.840.10045.4.3.2",      "ecdsa-with-SHA256" },
  { "1.2.840.10045.4.3.3",      "ecdsa-with-SHA384" },
  { "1.2.840.10045.4.3.4",      "ecdsa-with-SHA512" },
  { "1.2.840.10046.2.1",        "dhpublicnumber" },
  { "1.2.840.113549.1.1.1",     "rsaEncryption" },
  { "1.2.840.113549.1.1.2",     "md2WithRSAEncryption" },
  { "1.2.840.113549.1.1.4",     "md5WithRSAEncryption" },
  { "1.2.840.113549.1.1.5",     "sha1WithRSAEncryption" },
  { "1.2.840.113549.1.1.10",    "RSASSA-PSS" },
  { "1.2.840.113549.1.1.14",    "sha224WithRSAEncryption" },
  { "1.2.840.113549.1.1.11",    "sha256WithRSAEncryption" },
  { "1.2.840.113549.1.1.12",    "sha384WithRSAEncryption" },
  { "1.2.840.113549.1.1.13",    "sha512WithRSAEncryption" },
  { "1.2.840.113549.2.2",       "md2" },
  { "1.2.840.113549.2.5",       "md5" },
  { "1.3.14.3.2.26",            "sha1" },
  { "2.5.4.3",                  "CN" },
  { "2.5.4.4",                  "SN" },
  { "2.5.4.5",                  "serialNumber" },
  { "2.5.4.6",                  "C" },
  { "2.5.4.7",                  "L" },
  { "2.5.4.8",                  "ST" },
  { "2.5.4.9",                  "streetAddress" },
  { "2.5.4.10",                 "O" },
  { "2.5.4.11",                 "OU" },
  { "2.5.4.12",                 "title" },
  { "2.5.4.13",                 "description" },
  { "2.5.4.17",                 "postalCode" },
  { "2.5.4.41",                 "name" },
  { "2.5.4.42",                 "givenName" },
  { "2.5.4.43",                 "initials" },
  { "2.5.4.44",                 "generationQualifier" },
  { "2.5.4.45",                 "X500UniqueIdentifier" },
  { "2.5.4.46",                 "dnQualifier" },
  { "2.5.4.65",                 "pseudonym" },
  { "1.2.840.113549.1.9.1",     "emailAddress" },
  { "2.5.4.72",                 "role" },
  { "2.5.29.17",                "subjectAltName" },
  { "2.5.29.18",                "issuerAltName" },
  { "2.5.29.19",                "basicConstraints" },
  { "2.16.840.1.101.3.4.2.4",   "sha224" },
  { "2.16.840.1.101.3.4.2.1",   "sha256" },
  { "2.16.840.1.101.3.4.2.2",   "sha384" },
  { "2.16.840.1.101.3.4.2.3",   "sha512" },
  { "1.2.840.113549.1.9.2",     "unstructuredName" },
  { ಖಾಲಿ,                       ಖಾಲಿ }
};

#ಆದರೆ_ಅಂತ್ಯ /* WANT_EXTRACT_CERTINFO */

/*
 * Lightweight ASN.1 parser.
 * In particular, it does not check for syntactic/lexical errors.
 * It is intended to support certificate information gathering for SSL backends
 * that offer a mean to get certificates as a whole, but do not supply
 * entry points to get particular certificate sub-fields.
 * Please note there is no pretension here to rewrite a full SSL library.
 */

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ASN1_MAX_RECURSIONS    16

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t *getASN1Element_(ರಚನೆ Curl_asn1Element *elem,
                                      ಸ್ಥಿರಾಂಕ uint8_t *beg, ಸ್ಥಿರಾಂಕ uint8_t *ಕೊನೆ,
                                      size_t lvl)
{
  uint8_t b;
  size_t len;
  ರಚನೆ Curl_asn1Element lelem;

  /* Get a single ASN.1 element into `elem', parse ASN.1 string at `beg'
     ending at `end'.
     Returns a pointer in source string after the parsed element, or NULL
     if an error occurs. */
  ಆದರೆ(!beg || !ಕೊನೆ || beg >= ಕೊನೆ || !*beg ||
     ((size_t)(ಕೊನೆ - beg) > CURL_ASN1_MAX) ||
     lvl++ >= CURL_ASN1_MAX_RECURSIONS)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  /* Process header byte. */
  elem->ಶೀರ್ಷಿಕೆ = beg;
  b = *beg++;
  elem->constructed = (b & 0x20) != 0;
  elem->eclass = (b >> 6) & 3;
  b &= 0x1F;
  ಆದರೆ(b == 0x1F)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* Long tag values not supported here. */
  elem->ಟ್ಯಾಗ್ = b;

  /* Process length. */
  ಆದರೆ(beg >= ಕೊನೆ)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  b = *beg++;
  ಆದರೆ(!(b & 0x80))
    len = b;
  ಇಲ್ಲದಿದ್ದರೆ {
    b &= 0x7F;
    ಆದರೆ(!b) {
      /* Unspecified length. Since we have all the data, we can determine the
         effective length by skipping element until an EOC element is found. */
      ಆದರೆ(!elem->constructed)
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
      elem->beg = beg;
      ಆಗ(beg < ಕೊನೆ && *beg) {
        beg = getASN1Element_(&lelem, beg, ಕೊನೆ, lvl);
        ಆದರೆ(!beg)
          ಹಿಂತಿರುಗಿ ಖಾಲಿ;
      }
      ಆದರೆ(ಕೊನೆ - beg < 2 || beg[1])       /* EOC must have a zero length. */
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
      elem->ಕೊನೆ = beg;                  /* Do not include EOC as content. */
      ಹಿಂತಿರುಗಿ beg + 2;                   /* After EOC element. */
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b > (size_t)(ಕೊನೆ - beg))
      ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* Does not fit in source. */
    ಇಲ್ಲದಿದ್ದರೆ {
      /* Get long length. */
      len = 0;
      ಮಾಡು {
        ಆದರೆ(len & 0xFF000000L)
          ಹಿಂತಿರುಗಿ ಖಾಲಿ;  /* Lengths > 32 bits are not supported. */
        len = (len << 8) | *beg++;
      } ಆಗ(--b);
    }
  }
  ಆದರೆ(len > (size_t)(ಕೊನೆ - beg))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;  /* Element data does not fit in source. */
  elem->beg = beg;
  elem->ಕೊನೆ = beg + len;
  ಹಿಂತಿರುಗಿ elem->ಕೊನೆ;
}

/*
 * @unittest 1657
 */
UNITTEST ಸ್ಥಿರಾಂಕ uint8_t *getASN1Element(ರಚನೆ Curl_asn1Element *elem,
                                       ಸ್ಥಿರಾಂಕ uint8_t *beg, ಸ್ಥಿರಾಂಕ uint8_t *ಕೊನೆ);
UNITTEST ಸ್ಥಿರಾಂಕ uint8_t *getASN1Element(ರಚನೆ Curl_asn1Element *elem,
                                       ಸ್ಥಿರಾಂಕ uint8_t *beg, ಸ್ಥಿರಾಂಕ uint8_t *ಕೊನೆ)
{
  ಹಿಂತಿರುಗಿ getASN1Element_(elem, beg, ಕೊನೆ, 0);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WANT_ಹೊರತೆಗೆ_CERTINFO

/*
 * Search the null-terminated OID or OID identifier in local table.
 * Return the table entry pointer or NULL if not found.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_OID *ಹುಡುಕು_OID(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *oid)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_OID *op;
  ಗಾಗಿ(op = OIDtable; op->numoid; op++)
    ಆದರೆ(!strcmp(op->numoid, oid) || curl_strequal(op->textoid, oid))
      ಹಿಂತಿರುಗಿ op;

  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Convert an ASN.1 Boolean value into its string representation.
 *
 * Return error code.
 */

ಸ್ಥಿರ CURLcode bool2str(ರಚನೆ dynbuf *store,
                         ಸ್ಥಿರಾಂಕ uint8_t *beg, ಸ್ಥಿರಾಂಕ uint8_t *ಕೊನೆ)
{
  uint8_t val;
  ಆದರೆ(ಕೊನೆ - beg != 1)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  val = *beg;
  ಆದರೆ((val != 0xff) && (val != 0x00))
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಹಿಂತಿರುಗಿ curlx_dyn_ಕೂಡಿಸು(store, val ? "TRUE" : "FALSE");
}

/*
 * Convert an ASN.1 octet string to a printable string.
 *
 * Return error code.
 */
ಸ್ಥಿರ CURLcode octet2str(ರಚನೆ dynbuf *store,
                          ಸ್ಥಿರಾಂಕ uint8_t *beg, ಸ್ಥಿರಾಂಕ uint8_t *ಕೊನೆ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆಗ(!ಫಲಿತಾಂಶ && beg < ಕೊನೆ)
    ಫಲಿತಾಂಶ = curlx_dyn_addf(store, "%02x:", *beg++);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode bit2str(ರಚನೆ dynbuf *store,
                        ಸ್ಥಿರಾಂಕ uint8_t *beg, ಸ್ಥಿರಾಂಕ uint8_t *ಕೊನೆ)
{
  /* Convert an ASN.1 bit string to a printable string. */

  ಆದರೆ(beg < ಕೊನೆ) {
    uint8_t ಬಳಸದ_ಬಿಟ್ಟುಗಳು = *beg++;
    ಆದರೆ(ಬಳಸದ_ಬಿಟ್ಟುಗಳು > 7)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಹಿಂತಿರುಗಿ octet2str(store, beg, ಕೊನೆ);
}

/*
 * Convert an ASN.1 integer value into its string representation.
 *
 * Returns error.
 */
ಸ್ಥಿರ CURLcode int2str(ರಚನೆ dynbuf *store,
                        ಸ್ಥಿರಾಂಕ uint8_t *beg, ಸ್ಥಿರಾಂಕ uint8_t *ಕೊನೆ)
{
  uint32_t uval = 0;
  int32_t sval = 0;
  size_t n = ಕೊನೆ - beg;
  size_t i;
  ಸ್ಥಿರಾಂಕ uint8_t *p = beg;

  ಆದರೆ(!n)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  ಆದರೆ(n > 4)
    ಹಿಂತಿರುಗಿ octet2str(store, beg, ಕೊನೆ);

  ಆದರೆ(p[0] & 0x80)
    uval = ~0U;

  ಗಾಗಿ(i = 0; i < n; i++)
    uval = (uval << 8) | p[i];

  sval = (int32_t)uval;

  /* small values are easier to read as decimals */
  ಆದರೆ(sval > -10000 && sval < 10000)
    ಹಿಂತಿರುಗಿ curlx_dyn_addf(store, "%d", sval);

  /* represent larger values as hex */
  ಹಿಂತಿರುಗಿ curlx_dyn_addf(store, "0x%x", uval);
}

/*
 * Convert from an ASN.1 typed string to UTF8.
 *
 * The result is stored in a dynbuf that is inited by the user of this
 * function.
 *
 * Returns error.
 */
ಸ್ಥಿರ CURLcode utf8asn1str(ರಚನೆ dynbuf *ಗಮ್ಯ, ಪೂರ್ಣಾಂಕ type,
                            ಸ್ಥಿರಾಂಕ uint8_t *from, ಸ್ಥಿರಾಂಕ uint8_t *ಕೊನೆ)
{
  size_t inlength = ಕೊನೆ - from;
  ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2 = 1;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆಯ್ಕೆ(type) {
  ಸ್ಥಿತಿ CURL_ASN1_BMP_STRING:
    ಗಾತ್ರ_2 = 2;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_ASN1_UNIVERSAL_STRING:
    ಗಾತ್ರ_2 = 4;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_ASN1_NUMERIC_STRING:
  ಸ್ಥಿತಿ CURL_ASN1_PRINTABLE_STRING:
  ಸ್ಥಿತಿ CURL_ASN1_TELETEX_STRING:
  ಸ್ಥಿತಿ CURL_ASN1_IA5_STRING:
  ಸ್ಥಿತಿ CURL_ASN1_VISIBLE_STRING:
  ಸ್ಥಿತಿ CURL_ASN1_UTF8_STRING:
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;  /* Conversion not supported. */
  }

  ಆದರೆ(inlength % ಗಾತ್ರ_2)
    /* Length inconsistent with character size. */
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  ಆದರೆ(type == CURL_ASN1_UTF8_STRING) {
    /* copy. */
    ಆದರೆ(inlength)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(ಗಮ್ಯ, from, inlength);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆಗ(!ಫಲಿತಾಂಶ && (from < ಕೊನೆ)) {
      ಅಕ್ಷರ buf[4]; /* decode buffer */
      size_t charsize = 1;
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ wc = 0;

      ಆಯ್ಕೆ(ಗಾತ್ರ_2) {
      ಸ್ಥಿತಿ 4:
        wc = (wc << 8) | *from++;
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      ಸ್ಥಿತಿ 2:
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      ಪೂರ್ವನಿಯೋಜಿತ: /* case 1: */
        wc = (wc << 8) | *from++;
      }
      ಆದರೆ(wc >= 0x00000080) {
        ಆದರೆ(wc >= 0x00000800) {
          ಆದರೆ(wc >= 0x00010000) {
            ಆದರೆ(wc >= 0x00200000) {
              /* Invalid char. size for target encoding. */
              ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
            }
            buf[3] = (ಅಕ್ಷರ)(0x80 | (wc & 0x3F));
            wc = (wc >> 6) | 0x00010000;
            charsize++;
          }
          buf[2] = (ಅಕ್ಷರ)(0x80 | (wc & 0x3F));
          wc = (wc >> 6) | 0x00000800;
          charsize++;
        }
        buf[1] = (ಅಕ್ಷರ)(0x80 | (wc & 0x3F));
        wc = (wc >> 6) | 0x000000C0;
        charsize++;
      }
      buf[0] = (ಅಕ್ಷರ)wc;
      ಫಲಿತಾಂಶ = curlx_dyn_addn(ಗಮ್ಯ, buf, charsize);
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Convert an ASN.1 OID into its dotted string representation. Each field is
 * limited to unsigned 32-bit values.
 *
 * A minor limitation is in the second number, which can be no larger than 32
 * bits - 80 == 4294967215.
 *
 * Return error code.
 *
 * @unittest 1666
 */
UNITTEST CURLcode encodeOID(ರಚನೆ dynbuf *store,
                            ಸ್ಥಿರಾಂಕ uint8_t *beg, ಸ್ಥಿರಾಂಕ uint8_t *ಕೊನೆ);
UNITTEST CURLcode encodeOID(ರಚನೆ dynbuf *store,
                            ಸ್ಥಿರಾಂಕ uint8_t *beg, ಸ್ಥಿರಾಂಕ uint8_t *ಕೊನೆ)
{
  uint32_t x;
  uint32_t y;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(ಕೊನೆ <= beg)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  /* Process the first two numbers. The initial digit cannot be larger than
     2 */
  y = *beg++;
  ಆದರೆ(y <= 80)
    x = y / 40;
  ಇಲ್ಲದಿದ್ದರೆ
    x = 2;
  ಫಲಿತಾಂಶ = curlx_dyn_addf(store, "%u", x);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಆದರೆ(y & 0x80) {
    uint32_t t = (y & 0x7f); /* the second number */
    ಆದರೆ(!t)
      /* reject leading 0x80 padding */
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    ಮಾಡು {
      ಆದರೆ((t & 0xFE000000) || (beg >= ಕೊನೆ))
        ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
      y = *beg++;
      t = (t << 7) | (y & 0x7F);
    } ಆಗ(y & 0x80);
    y = t;
  }

  ಫಲಿತಾಂಶ = curlx_dyn_addf(store, ".%u", y - (x * 40));
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* Process the trailing numbers. */
  ಆಗ((beg < ಕೊನೆ) && !ಫಲಿತಾಂಶ) {
    x = 0;
    y = 0;
    ಮಾಡು {
      /* 0x80 as the first value is invalid since it only adds padding */
      ಆದರೆ((y & 0x80) && !x)
        ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((x & 0xFE000000) || (beg == ಕೊನೆ))
        ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
      y = *beg++;
      x = (x << 7) | (y & 0x7F);
    } ಆಗ(y & 0x80);
    ಫಲಿತಾಂಶ = curlx_dyn_addf(store, ".%u", x);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Convert an ASN.1 OID into its dotted or symbolic string representation.
 *
 * Return error code.
 */

ಸ್ಥಿರ CURLcode OID2str(ರಚನೆ dynbuf *store,
                        ಸ್ಥಿರಾಂಕ uint8_t *beg, ಸ್ಥಿರಾಂಕ uint8_t *ಕೊನೆ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಆದರೆ(beg < ಕೊನೆ) {
    ರಚನೆ dynbuf buf;
    curlx_dyn_init(&buf, CURL_X509_STR_MAX);
    ಫಲಿತಾಂಶ = encodeOID(&buf, beg, ಕೊನೆ);

    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_OID *op = ಹುಡುಕು_OID(curlx_dyn_ptr(&buf));
      ಆದರೆ(op)
        ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(store, op->textoid);
      ಇಲ್ಲದಿದ್ದರೆ
        ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(store, curlx_dyn_ptr(&buf));
    }
    curlx_dyn_free(&buf);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * @unittest 1656
 */
UNITTEST CURLcode GTime2str(ರಚನೆ dynbuf *store,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *beg, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ);
UNITTEST CURLcode GTime2str(ರಚನೆ dynbuf *store,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *beg, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tzp;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fracp;
  ಅಕ್ಷರ sec1, sec2;
  size_t fracl;
  size_t tzl;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sep = "";

  /* Convert an ASN.1 Generalized time to a printable string.
     Return the dynamically allocated string, or NULL if an error occurs. */

  ಗಾಗಿ(fracp = beg; fracp < ಕೊನೆ && ISDIGIT(*fracp); fracp++)
    ;

  /* Get seconds digits. */
  sec1 = '0';
  ಆಯ್ಕೆ(fracp - beg - 12) {
  ಸ್ಥಿತಿ 0:
    sec2 = '0';
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 2:
    sec1 = fracp[-2];
    FALLTHROUGH();
  ಸ್ಥಿತಿ 1:
    sec2 = fracp[-1];
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }

  /* timezone follows optional fractional seconds. */
  tzp = fracp;
  fracl = 0; /* no fractional seconds detected so far */
  ಆದರೆ(fracp < ಕೊನೆ && (*fracp == '.' || *fracp == ',')) {
    /* Have fractional seconds, e.g. "[.,]\d+". How many? */
    fracp++; /* should be a digit char or BAD ARGUMENT */
    tzp = fracp;
    ಆಗ(tzp < ಕೊನೆ && ISDIGIT(*tzp))
      tzp++;
    ಆದರೆ(tzp == fracp) /* never looped, no digit after [.,] */
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    fracl = tzp - fracp; /* number of fractional sec digits */
    DEBUGASSERT(fracl > 0);
    /* Strip trailing zeroes in fractional seconds.
     * May reduce fracl to 0 if only '0's are present. */
    ಆಗ(fracl && fracp[fracl - 1] == '0')
      fracl--;
  }

  /* Process timezone. */
  ಆದರೆ(tzp >= ಕೊನೆ) {
    tzp = "";
    tzl = 0;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*tzp == 'Z') {
    sep = " ";
    tzp = "GMT";
    tzl = 3;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((*tzp == '+') || (*tzp == '-')) {
    sep = " UTC";
    tzl = ಕೊನೆ - tzp;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    sep = " ";
    tzl = ಕೊನೆ - tzp;
  }

  ಹಿಂತಿರುಗಿ curlx_dyn_addf(store,
                        "%.4s-%.2s-%.2s %.2s:%.2s:%c%c%s%.*s%s%.*s",
                        beg, beg + 4, beg + 6,
                        beg + 8, beg + 10, sec1, sec2,
                        fracl ? "." : "", (ಪೂರ್ಣಾಂಕ)fracl, fracp,
                        sep, (ಪೂರ್ಣಾಂಕ)tzl, tzp);
}

/*
 * Convert an ASN.1 UTC time to a printable string.
 *
 * Return error code.
 */
ಸ್ಥಿರ CURLcode UTime2str(ರಚನೆ dynbuf *store,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *beg, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tzp;
  size_t tzl;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sec;

  ಗಾಗಿ(tzp = beg; tzp < ಕೊನೆ && *tzp >= '0' && *tzp <= '9'; tzp++)
    ;
  /* Get the seconds. */
  sec = beg + 10;
  ಆಯ್ಕೆ(tzp - sec) {
  ಸ್ಥಿತಿ 0:
    sec = "00";
    FALLTHROUGH();
  ಸ್ಥಿತಿ 2:
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }

  /* Process timezone. */
  ಆದರೆ(tzp >= ಕೊನೆ)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಆದರೆ(*tzp == 'Z') {
    tzp = "GMT";
    ಕೊನೆ = tzp + 3;
  }

  tzl = ಕೊನೆ - tzp;
  ಹಿಂತಿರುಗಿ curlx_dyn_addf(store, "%d%.2s-%.2s-%.2s %.2s:%.2s:%.2s %.*s",
                        20 - (*beg >= '5'), beg, beg + 2, beg + 4,
                        beg + 6, beg + 8, sec,
                        (ಪೂರ್ಣಾಂಕ)tzl, tzp);
}

/*
 * Convert an ASN.1 element to a printable string.
 *
 * Return error
 *
 * @unittest 1667
 */
UNITTEST CURLcode ASN1tostr(ರಚನೆ dynbuf *store,
                            ರಚನೆ Curl_asn1Element *elem);
UNITTEST CURLcode ASN1tostr(ರಚನೆ dynbuf *store,
                            ರಚನೆ Curl_asn1Element *elem)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
  ಪೂರ್ಣಾಂಕ type = elem->ಟ್ಯಾಗ್;
  ಆದರೆ(elem->constructed)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ; /* No conversion of structured elements. */

  ಆಯ್ಕೆ(type) {
  ಸ್ಥಿತಿ CURL_ASN1_BOOLEAN:
    ಫಲಿತಾಂಶ = bool2str(store, elem->beg, elem->ಕೊನೆ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_ASN1_INTEGER:
  ಸ್ಥಿತಿ CURL_ASN1_ENUMERATED:
    ಫಲಿತಾಂಶ = int2str(store, elem->beg, elem->ಕೊನೆ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_ASN1_ಬಿಟ್_STRING:
    ಫಲಿತಾಂಶ = bit2str(store, elem->beg, elem->ಕೊನೆ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_ASN1_OCTET_STRING:
    ಫಲಿತಾಂಶ = octet2str(store, elem->beg, elem->ಕೊನೆ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_ASN1_NULL:
    ಫಲಿತಾಂಶ = curlx_dyn_addn(store, "", 1);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_ASN1_OBJECT_IDENTIFIER:
    ಫಲಿತಾಂಶ = OID2str(store, elem->beg, elem->ಕೊನೆ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_ASN1_UTC_ಸಮಯ:
    ಫಲಿತಾಂಶ = UTime2str(store, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)elem->beg,
                       (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)elem->ಕೊನೆ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_ASN1_GENERALIZED_ಸಮಯ:
    ಫಲಿತಾಂಶ = GTime2str(store, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)elem->beg,
                       (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)elem->ಕೊನೆ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_ASN1_UTF8_STRING:
  ಸ್ಥಿತಿ CURL_ASN1_NUMERIC_STRING:
  ಸ್ಥಿತಿ CURL_ASN1_PRINTABLE_STRING:
  ಸ್ಥಿತಿ CURL_ASN1_TELETEX_STRING:
  ಸ್ಥಿತಿ CURL_ASN1_IA5_STRING:
  ಸ್ಥಿತಿ CURL_ASN1_VISIBLE_STRING:
  ಸ್ಥಿತಿ CURL_ASN1_UNIVERSAL_STRING:
  ಸ್ಥಿತಿ CURL_ASN1_BMP_STRING:
    ಫಲಿತಾಂಶ = utf8asn1str(store, type, elem->beg, elem->ಕೊನೆ);
    ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * ASCII encode distinguished name at `dn' into the store dynbuf.
 *
 * Returns error.
 */
ಸ್ಥಿರ CURLcode encodeDN(ರಚನೆ dynbuf *store, ರಚನೆ Curl_asn1Element *dn)
{
  ರಚನೆ Curl_asn1Element rdn;
  ರಚನೆ Curl_asn1Element atv;
  ರಚನೆ Curl_asn1Element oid;
  ರಚನೆ Curl_asn1Element ಮೌಲ್ಯ;
  ಸ್ಥಿರಾಂಕ uint8_t *p1;
  ಸ್ಥಿರಾಂಕ uint8_t *p2;
  ಸ್ಥಿರಾಂಕ uint8_t *p3;
  ಸ್ಥಿರಾಂಕ uint8_t *str;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  bool added = FALSE;
  ರಚನೆ dynbuf ತಾತ್ಕಾಲಿಕ;
  curlx_dyn_init(&ತಾತ್ಕಾಲಿಕ, CURL_X509_STR_MAX);

  ಗಾಗಿ(p1 = dn->beg; p1 < dn->ಕೊನೆ;) {
    p1 = getASN1Element(&rdn, p1, dn->ಕೊನೆ);
    ಆದರೆ(!p1) {
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
      ಹೋಗು error;
    }
    ಗಾಗಿ(p2 = rdn.beg; p2 < rdn.ಕೊನೆ;) {
      p2 = getASN1Element(&atv, p2, rdn.ಕೊನೆ);
      ಆದರೆ(!p2) {
        ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
        ಹೋಗು error;
      }
      p3 = getASN1Element(&oid, atv.beg, atv.ಕೊನೆ);
      ಆದರೆ(!p3) {
        ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
        ಹೋಗು error;
      }
      ಆದರೆ(!getASN1Element(&ಮೌಲ್ಯ, p3, atv.ಕೊನೆ)) {
        ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
        ಹೋಗು error;
      }
      curlx_dyn_reset(&ತಾತ್ಕಾಲಿಕ);
      ಫಲಿತಾಂಶ = ASN1tostr(&ತಾತ್ಕಾಲಿಕ, &oid);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು error;

      str = curlx_dyn_uptr(&ತಾತ್ಕಾಲಿಕ);

      ಆದರೆ(!str) {
        ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
        ಹೋಗು error;
      }

      /* Encode delimiter.
         If attribute has a short uppercase name, delimiter is ", ". */
      ಗಾಗಿ(p3 = str; ISUPPER(*p3); p3++)
        ;
      ಆದರೆ(added) {
        ಆದರೆ(p3 - str > 2)
          ಫಲಿತಾಂಶ = curlx_dyn_addn(store, "/", 1);
        ಇಲ್ಲದಿದ್ದರೆ
          ಫಲಿತಾಂಶ = curlx_dyn_addn(store, ", ", 2);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹೋಗು error;
      }

      /* Encode attribute name. */
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(store, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)str);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು error;

      /* Generate equal sign. */
      ಫಲಿತಾಂಶ = curlx_dyn_addn(store, "=", 1);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು error;

      /* Generate value. */
      ಫಲಿತಾಂಶ = ASN1tostr(store, &ಮೌಲ್ಯ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು error;
      curlx_dyn_reset(&ತಾತ್ಕಾಲಿಕ);
      added = TRUE; /* use separator for next */
    }
  }
error:
  curlx_dyn_free(&ತಾತ್ಕಾಲಿಕ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* WANT_EXTRACT_CERTINFO */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WANT_PARSEX509
/*
 * ASN.1 parse an X509 certificate into structure subfields.
 * Syntax is assumed to have already been checked by the SSL backend.
 * See RFC 5280.
 */
ಪೂರ್ಣಾಂಕ Curl_parseX509(ರಚನೆ Curl_X509certificate *cert,
                   ಸ್ಥಿರಾಂಕ uint8_t *beg, ಸ್ಥಿರಾಂಕ uint8_t *ಕೊನೆ)
{
  ರಚನೆ Curl_asn1Element elem;
  ರಚನೆ Curl_asn1Element tbsCertificate;
  ಸ್ಥಿರಾಂಕ uint8_t *ccp;
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t default_ಆವೃತ್ತಿ = 0;  /* v1. */

  memset(cert, 0, ಗಾತ್ರ(*cert));
  cert->certificate.ಶೀರ್ಷಿಕೆ = ಖಾಲಿ;
  cert->certificate.beg = beg;
  cert->certificate.ಕೊನೆ = ಕೊನೆ;

  /* Get the sequence content. */
  ಆದರೆ(!getASN1Element(&elem, beg, ಕೊನೆ))
    ಹಿಂತಿರುಗಿ -1;  /* Invalid bounds/size. */
  beg = elem.beg;
  ಕೊನೆ = elem.ಕೊನೆ;

  /* Get tbsCertificate. */
  beg = getASN1Element(&tbsCertificate, beg, ಕೊನೆ);
  ಆದರೆ(!beg)
    ಹಿಂತಿರುಗಿ -1;
  /* Skip the signatureAlgorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, ಕೊನೆ);
  ಆದರೆ(!beg)
    ಹಿಂತಿರುಗಿ -1;
  /* Get the signatureValue. */
  ಆದರೆ(!getASN1Element(&cert->signature, beg, ಕೊನೆ))
    ಹಿಂತಿರುಗಿ -1;

  /* Parse TBSCertificate. */
  beg = tbsCertificate.beg;
  ಕೊನೆ = tbsCertificate.ಕೊನೆ;
  /* Get optional version, get serialNumber. */
  cert->ಆವೃತ್ತಿ_2.ಶೀರ್ಷಿಕೆ = ಖಾಲಿ;
  cert->ಆವೃತ್ತಿ_2.beg = &default_ಆವೃತ್ತಿ;
  cert->ಆವೃತ್ತಿ_2.ಕೊನೆ = &default_ಆವೃತ್ತಿ + ಗಾತ್ರ(default_ಆವೃತ್ತಿ);
  beg = getASN1Element(&elem, beg, ಕೊನೆ);
  ಆದರೆ(!beg)
    ಹಿಂತಿರುಗಿ -1;
  ಆದರೆ(elem.ಟ್ಯಾಗ್ == 0) {
    ಆದರೆ(!getASN1Element(&cert->ಆವೃತ್ತಿ_2, elem.beg, elem.ಕೊನೆ))
      ಹಿಂತಿರುಗಿ -1;
    beg = getASN1Element(&elem, beg, ಕೊನೆ);
    ಆದರೆ(!beg)
      ಹಿಂತಿರುಗಿ -1;
  }
  cert->serial_ಸಂಖ್ಯೆ = elem;
  /* Get signature algorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, ಕೊನೆ);
  /* Get issuer. */
  beg = getASN1Element(&cert->issuer, beg, ಕೊನೆ);
  ಆದರೆ(!beg)
    ಹಿಂತಿರುಗಿ -1;
  /* Get notBefore and notAfter. */
  beg = getASN1Element(&elem, beg, ಕೊನೆ);
  ಆದರೆ(!beg)
    ಹಿಂತಿರುಗಿ -1;
  ccp = getASN1Element(&cert->notBefore, elem.beg, elem.ಕೊನೆ);
  ಆದರೆ(!ccp)
    ಹಿಂತಿರುಗಿ -1;
  ಆದರೆ(!getASN1Element(&cert->notAfter, ccp, elem.ಕೊನೆ))
    ಹಿಂತಿರುಗಿ -1;
  /* These two fields MUST consume all of the validity elem */
  ಆದರೆ(elem.ಕೊನೆ != cert->notAfter.ಕೊನೆ)
    ಹಿಂತಿರುಗಿ -1;
  /* Get subject. */
  beg = getASN1Element(&cert->subject, beg, ಕೊನೆ);
  ಆದರೆ(!beg)
    ಹಿಂತಿರುಗಿ -1;
  /* Get subjectPublicKeyAlgorithm and subjectPublicKey. */
  beg = getASN1Element(&cert->subject_ಬಹಿರಂಗ_ಕೀಲಿ_Info, beg, ಕೊನೆ);
  ಆದರೆ(!beg)
    ಹಿಂತಿರುಗಿ -1;
  ccp = getASN1Element(&cert->subject_ಬಹಿರಂಗ_ಕೀಲಿ_Algorithm,
                       cert->subject_ಬಹಿರಂಗ_ಕೀಲಿ_Info.beg,
                       cert->subject_ಬಹಿರಂಗ_ಕೀಲಿ_Info.ಕೊನೆ);
  ಆದರೆ(!ccp)
    ಹಿಂತಿರುಗಿ -1;
  ಆದರೆ(!getASN1Element(&cert->subject_ಬಹಿರಂಗ_ಕೀಲಿ, ccp,
                     cert->subject_ಬಹಿರಂಗ_ಕೀಲಿ_Info.ಕೊನೆ))
    ಹಿಂತಿರುಗಿ -1;
  /* Get optional issuerUniqueID, subjectUniqueID and extensions. */
  cert->issuerUniqueID.ಟ್ಯಾಗ್ = cert->subjectUniqueID.ಟ್ಯಾಗ್ = 0;
  cert->extensions.ಟ್ಯಾಗ್ = elem.ಟ್ಯಾಗ್ = 0;
  cert->issuerUniqueID.ಶೀರ್ಷಿಕೆ = cert->subjectUniqueID.ಶೀರ್ಷಿಕೆ = ಖಾಲಿ;
  cert->issuerUniqueID.beg = cert->issuerUniqueID.ಕೊನೆ = beg;
  cert->subjectUniqueID.beg = cert->subjectUniqueID.ಕೊನೆ = beg;
  cert->extensions.ಶೀರ್ಷಿಕೆ = ಖಾಲಿ;
  cert->extensions.beg = cert->extensions.ಕೊನೆ = beg;
  ಆದರೆ(beg < ಕೊನೆ) {
    beg = getASN1Element(&elem, beg, ಕೊನೆ);
    ಆದರೆ(!beg)
      ಹಿಂತಿರುಗಿ -1;
  }
  ಆದರೆ(elem.ಟ್ಯಾಗ್ == 1) {
    cert->issuerUniqueID = elem;
    ಆದರೆ(beg < ಕೊನೆ) {
      beg = getASN1Element(&elem, beg, ಕೊನೆ);
      ಆದರೆ(!beg)
        ಹಿಂತಿರುಗಿ -1;
    }
  }
  ಆದರೆ(elem.ಟ್ಯಾಗ್ == 2) {
    cert->subjectUniqueID = elem;
    ಆದರೆ(beg < ಕೊನೆ) {
      beg = getASN1Element(&elem, beg, ಕೊನೆ);
      ಆದರೆ(!beg)
        ಹಿಂತಿರುಗಿ -1;
    }
  }
  ಆದರೆ(elem.ಟ್ಯಾಗ್ == 3)
    ಆದರೆ(!getASN1Element(&cert->extensions, elem.beg, elem.ಕೊನೆ))
      ಹಿಂತಿರುಗಿ -1;
  ಹಿಂತಿರುಗಿ 0;
}

#ಆದರೆ_ಅಂತ್ಯ /* WANT_PARSEX509 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WANT_ಹೊರತೆಗೆ_CERTINFO

ಸ್ಥಿರ CURLcode dumpAlgo(ರಚನೆ dynbuf *store,
                         ರಚನೆ Curl_asn1Element *param,
                         ಸ್ಥಿರಾಂಕ uint8_t *beg, ಸ್ಥಿರಾಂಕ uint8_t *ಕೊನೆ)
{
  ರಚನೆ Curl_asn1Element oid;

  /* Get algorithm parameters and return algorithm name. */

  beg = getASN1Element(&oid, beg, ಕೊನೆ);
  ಆದರೆ(!beg)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  param->ಶೀರ್ಷಿಕೆ = ಖಾಲಿ;
  param->ಟ್ಯಾಗ್ = 0;
  param->beg = param->ಕೊನೆ = ಕೊನೆ;
  ಆದರೆ(beg < ಕೊನೆ) {
    ಸ್ಥಿರಾಂಕ uint8_t *p = getASN1Element(param, beg, ಕೊನೆ);
    ಆದರೆ(!p)
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }
  ಹಿಂತಿರುಗಿ OID2str(store, oid.beg, oid.ಕೊನೆ);
}

/*
 * This is a convenience function for push_certinfo_len that takes a zero
 * terminated value.
 */
ಸ್ಥಿರ CURLcode ssl_ತಳ್ಳು_certinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಪೂರ್ಣಾಂಕ certnum,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ)
{
  size_t valuelen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮೌಲ್ಯ);

  ಹಿಂತಿರುಗಿ Curl_ssl_ತಳ್ಳು_certinfo_len(ದತ್ತಾಂಶ, certnum, label, ಮೌಲ್ಯ, valuelen);
}

/*
 * This is a convenience function for push_certinfo_len that takes a
 * dynbuf value.
 *
 * It also does the verbose output if !certnum.
 */
ಸ್ಥಿರ CURLcode ssl_ತಳ್ಳು_certinfo_dyn(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ಪೂರ್ಣಾಂಕ certnum,
                                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label,
                                      ರಚನೆ dynbuf *ptr)
{
  size_t valuelen = curlx_dyn_len(ptr);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ = curlx_dyn_ptr(ptr);

  CURLcode ಫಲಿತಾಂಶ = Curl_ssl_ತಳ್ಳು_certinfo_len(ದತ್ತಾಂಶ, certnum, label,
                                               ಮೌಲ್ಯ, valuelen);

  ಆದರೆ(!certnum && !ಫಲಿತಾಂಶ)
    infof(ದತ್ತಾಂಶ, "   %s: %s", label, ಮೌಲ್ಯ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode do_pubkey_ಕ್ಷೇತ್ರ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ certnum,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label,
                                ರಚನೆ Curl_asn1Element *elem)
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ dynbuf ಹೊರಹೋಗು;

  curlx_dyn_init(&ಹೊರಹೋಗು, CURL_X509_STR_MAX);

  /* Generate a certificate information record for the public key. */

  ಫಲಿತಾಂಶ = ASN1tostr(&ಹೊರಹೋಗು, elem);
  ಆದರೆ(!ಫಲಿತಾಂಶ && ದತ್ತಾಂಶ->ಗಣ.ssl.certinfo)
    ಫಲಿತಾಂಶ = ssl_ತಳ್ಳು_certinfo_dyn(ದತ್ತಾಂಶ, certnum, label, &ಹೊರಹೋಗು);
  curlx_dyn_free(&ಹೊರಹೋಗು);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* return 0 on success, 1 on error */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ do_pubkey(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ certnum, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *algo,
                     ರಚನೆ Curl_asn1Element *param,
                     ರಚನೆ Curl_asn1Element *pubkey)
{
  ರಚನೆ Curl_asn1Element elem;
  ರಚನೆ Curl_asn1Element pk;
  ಸ್ಥಿರಾಂಕ uint8_t *p;

  /* Generate all information records for the public key. */

  ಆದರೆ(curl_strequal(algo, "ecPublicKey")) {
    /*
     * ECC public key is all the data, a value of type BIT STRING mapped to
     * OCTET STRING and should not be parsed as an ASN.1 value.
     */
    ಸ್ಥಿರಾಂಕ size_t dlen = pubkey->ಕೊನೆ - pubkey->beg;
    size_t len;
    ಆದರೆ(dlen < 2)
      /* too small */
      ಹಿಂತಿರುಗಿ 1;
    len = (dlen - 2) * 4;
    ಆದರೆ(!certnum)
      infof(ದತ್ತಾಂಶ, "   ECC Public Key (%zu bits)", len);
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl.certinfo) {
      ಅಕ್ಷರ q[(ಗಾತ್ರ(len) * 8 / 3) + 1];
      (ಶೂನ್ಯ)curl_msnprintf(q, ಗಾತ್ರ(q), "%zu", len);
      ಆದರೆ(ssl_ತಳ್ಳು_certinfo(ದತ್ತಾಂಶ, certnum, "ECC Public Key", q))
        ಹಿಂತಿರುಗಿ 1;
    }
    ಹಿಂತಿರುಗಿ do_pubkey_ಕ್ಷೇತ್ರ(ದತ್ತಾಂಶ, certnum, "ecPublicKey", pubkey) == CURLE_OK
      ? 0 : 1;
  }

  /* Get the public key (single element). */
  ಆದರೆ(!getASN1Element(&pk, pubkey->beg + 1, pubkey->ಕೊನೆ))
    ಹಿಂತಿರುಗಿ 1;

  ಆದರೆ(curl_strequal(algo, "rsaEncryption")) {
    ಸ್ಥಿರಾಂಕ uint8_t *q;
    size_t len;

    p = getASN1Element(&elem, pk.beg, pk.ಕೊನೆ);
    ಆದರೆ(!p)
      ಹಿಂತಿರುಗಿ 1;

    /* Compute key length. */
    ಗಾಗಿ(q = elem.beg; q < elem.ಕೊನೆ && !*q; q++)
      ;
    len = ((elem.ಕೊನೆ - q) * 8);
    ಆದರೆ(len) {
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
      ಗಾಗಿ(i = *q; !(i & 0x80); i <<= 1)
        len--;
    }
    ಆದರೆ(len > 32)
      elem.beg = q;     /* Strip leading zero bytes. */
    ಆದರೆ(!certnum)
      infof(ದತ್ತಾಂಶ, "   RSA Public Key (%zu bits)", len);
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl.certinfo) {
      ಅಕ್ಷರ r[(ಗಾತ್ರ(len) * 8 / 3) + 1];
      curl_msnprintf(r, ಗಾತ್ರ(r), "%zu", len);
      ಆದರೆ(ssl_ತಳ್ಳು_certinfo(ದತ್ತಾಂಶ, certnum, "RSA Public Key", r))
        ಹಿಂತಿರುಗಿ 1;
    }
    /* Generate coefficients. */
    ಆದರೆ(do_pubkey_ಕ್ಷೇತ್ರ(ದತ್ತಾಂಶ, certnum, "rsa(n)", &elem))
      ಹಿಂತಿರುಗಿ 1;
    ಆದರೆ(!getASN1Element(&elem, p, pk.ಕೊನೆ))
      ಹಿಂತಿರುಗಿ 1;
    ಆದರೆ(do_pubkey_ಕ್ಷೇತ್ರ(ದತ್ತಾಂಶ, certnum, "rsa(e)", &elem))
      ಹಿಂತಿರುಗಿ 1;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal(algo, "dsa")) {
    p = getASN1Element(&elem, param->beg, param->ಕೊನೆ);
    ಆದರೆ(p) {
      ಆದರೆ(do_pubkey_ಕ್ಷೇತ್ರ(ದತ್ತಾಂಶ, certnum, "dsa(p)", &elem))
        ಹಿಂತಿರುಗಿ 1;
      p = getASN1Element(&elem, p, param->ಕೊನೆ);
      ಆದರೆ(p) {
        ಆದರೆ(do_pubkey_ಕ್ಷೇತ್ರ(ದತ್ತಾಂಶ, certnum, "dsa(q)", &elem))
          ಹಿಂತಿರುಗಿ 1;
        ಆದರೆ(getASN1Element(&elem, p, param->ಕೊನೆ)) {
          ಆದರೆ(do_pubkey_ಕ್ಷೇತ್ರ(ದತ್ತಾಂಶ, certnum, "dsa(g)", &elem))
            ಹಿಂತಿರುಗಿ 1;
          ಆದರೆ(do_pubkey_ಕ್ಷೇತ್ರ(ದತ್ತಾಂಶ, certnum, "dsa(pub_key)", &pk))
            ಹಿಂತಿರುಗಿ 1;
        }
      }
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strequal(algo, "dhpublicnumber")) {
    p = getASN1Element(&elem, param->beg, param->ಕೊನೆ);
    ಆದರೆ(p) {
      ಆದರೆ(do_pubkey_ಕ್ಷೇತ್ರ(ದತ್ತಾಂಶ, certnum, "dh(p)", &elem))
        ಹಿಂತಿರುಗಿ 1;
      ಆದರೆ(getASN1Element(&elem, p, param->ಕೊನೆ)) {
        ಆದರೆ(do_pubkey_ಕ್ಷೇತ್ರ(ದತ್ತಾಂಶ, certnum, "dh(g)", &elem))
          ಹಿಂತಿರುಗಿ 1;
        ಆದರೆ(do_pubkey_ಕ್ಷೇತ್ರ(ದತ್ತಾಂಶ, certnum, "dh(pub_key)", &pk))
          ಹಿಂತಿರುಗಿ 1;
      }
    }
  }
  ಹಿಂತಿರುಗಿ 0;
}

/*
 * Convert an ASN.1 distinguished name into a printable string.
 * Return error.
 */
ಸ್ಥಿರ CURLcode DNtostr(ರಚನೆ dynbuf *store, ರಚನೆ Curl_asn1Element *dn)
{
  ಹಿಂತಿರುಗಿ encodeDN(store, dn);
}

CURLcode Curl_ಹೊರತೆಗೆ_certinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ಪೂರ್ಣಾಂಕ certnum,
                               ಸ್ಥಿರಾಂಕ uint8_t *beg,
                               ಸ್ಥಿರಾಂಕ uint8_t *ಕೊನೆ)
{
  ರಚನೆ Curl_X509certificate cert;
  ರಚನೆ Curl_asn1Element param;
  ಅಕ್ಷರ *certptr;
  size_t clen;
  ರಚನೆ dynbuf ಹೊರಹೋಗು;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಆವೃತ್ತಿ_2;
  ಸ್ಥಿರಾಂಕ uint8_t *ptr;
  ಪೂರ್ಣಾಂಕ rc;

  ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.ssl.certinfo)
    ಆದರೆ(certnum)
      ಹಿಂತಿರುಗಿ CURLE_OK;

  curlx_dyn_init(&ಹೊರಹೋಗು, CURL_X509_STR_MAX);
  /* Prepare the certificate information for curl_easy_getinfo(). */

  /* Extract the certificate ASN.1 elements. */
  ಆದರೆ(Curl_parseX509(&cert, beg, ಕೊನೆ))
    ಹಿಂತಿರುಗಿ CURLE_PEER_FAILED_VERIFICATION;

  /* Subject. */
  ಫಲಿತಾಂಶ = DNtostr(&ಹೊರಹೋಗು, &cert.subject);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಮುಗಿದ;
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl.certinfo) {
    ಫಲಿತಾಂಶ = ssl_ತಳ್ಳು_certinfo_dyn(ದತ್ತಾಂಶ, certnum, "Subject", &ಹೊರಹೋಗು);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಮುಗಿದ;
  }
  curlx_dyn_reset(&ಹೊರಹೋಗು);

  /* Issuer. */
  ಫಲಿತಾಂಶ = DNtostr(&ಹೊರಹೋಗು, &cert.issuer);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಮುಗಿದ;
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl.certinfo) {
    ಫಲಿತಾಂಶ = ssl_ತಳ್ಳು_certinfo_dyn(ದತ್ತಾಂಶ, certnum, "Issuer", &ಹೊರಹೋಗು);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಮುಗಿದ;
  }
  curlx_dyn_reset(&ಹೊರಹೋಗು);

  /* Version (always fits in less than 32 bits). */
  ಆವೃತ್ತಿ_2 = 0;
  ಗಾಗಿ(ptr = cert.ಆವೃತ್ತಿ_2.beg; ptr < cert.ಆವೃತ್ತಿ_2.ಕೊನೆ; ptr++)
    ಆವೃತ್ತಿ_2 = (ಆವೃತ್ತಿ_2 << 8) | *ptr;
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl.certinfo) {
    ಫಲಿತಾಂಶ = curlx_dyn_addf(&ಹೊರಹೋಗು, "%x", ಆವೃತ್ತಿ_2);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಮುಗಿದ;
    ಫಲಿತಾಂಶ = ssl_ತಳ್ಳು_certinfo_dyn(ದತ್ತಾಂಶ, certnum, "Version", &ಹೊರಹೋಗು);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಮುಗಿದ;
    curlx_dyn_reset(&ಹೊರಹೋಗು);
  }

  /* Serial number. */
  ಫಲಿತಾಂಶ = ASN1tostr(&ಹೊರಹೋಗು, &cert.serial_ಸಂಖ್ಯೆ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಮುಗಿದ;
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl.certinfo) {
    ಫಲಿತಾಂಶ = ssl_ತಳ್ಳು_certinfo_dyn(ದತ್ತಾಂಶ, certnum, "Serial Number", &ಹೊರಹೋಗು);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಮುಗಿದ;
  }
  curlx_dyn_reset(&ಹೊರಹೋಗು);

  /* Signature algorithm .*/
  ಫಲಿತಾಂಶ = dumpAlgo(&ಹೊರಹೋಗು, &param, cert.signatureAlgorithm.beg,
                    cert.signatureAlgorithm.ಕೊನೆ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಮುಗಿದ;
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl.certinfo) {
    ಫಲಿತಾಂಶ = ssl_ತಳ್ಳು_certinfo_dyn(ದತ್ತಾಂಶ, certnum, "Signature Algorithm", &ಹೊರಹೋಗು);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಮುಗಿದ;
  }
  curlx_dyn_reset(&ಹೊರಹೋಗು);

  /* Start Date. */
  ಫಲಿತಾಂಶ = ASN1tostr(&ಹೊರಹೋಗು, &cert.notBefore);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಮುಗಿದ;
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl.certinfo) {
    ಫಲಿತಾಂಶ = ssl_ತಳ್ಳು_certinfo_dyn(ದತ್ತಾಂಶ, certnum, "Start Date", &ಹೊರಹೋಗು);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಮುಗಿದ;
  }
  curlx_dyn_reset(&ಹೊರಹೋಗು);

  /* Expire Date. */
  ಫಲಿತಾಂಶ = ASN1tostr(&ಹೊರಹೋಗು, &cert.notAfter);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಮುಗಿದ;
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl.certinfo) {
    ಫಲಿತಾಂಶ = ssl_ತಳ್ಳು_certinfo_dyn(ದತ್ತಾಂಶ, certnum, "Expire Date", &ಹೊರಹೋಗು);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಮುಗಿದ;
  }
  curlx_dyn_reset(&ಹೊರಹೋಗು);

  /* Public Key Algorithm. */
  ಫಲಿತಾಂಶ = dumpAlgo(&ಹೊರಹೋಗು, &param, cert.subject_ಬಹಿರಂಗ_ಕೀಲಿ_Algorithm.beg,
                    cert.subject_ಬಹಿರಂಗ_ಕೀಲಿ_Algorithm.ಕೊನೆ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಮುಗಿದ;
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl.certinfo) {
    ಫಲಿತಾಂಶ = ssl_ತಳ್ಳು_certinfo_dyn(ದತ್ತಾಂಶ, certnum, "Public Key Algorithm",
                                   &ಹೊರಹೋಗು);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಮುಗಿದ;
  }

  rc = do_pubkey(ದತ್ತಾಂಶ, certnum, curlx_dyn_ptr(&ಹೊರಹೋಗು),
                 &param, &cert.subject_ಬಹಿರಂಗ_ಕೀಲಿ);
  ಆದರೆ(rc) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ; /* the most likely error */
    ಹೋಗು ಮುಗಿದ;
  }
  curlx_dyn_reset(&ಹೊರಹೋಗು);

  /* Signature. */
  ಫಲಿತಾಂಶ = ASN1tostr(&ಹೊರಹೋಗು, &cert.signature);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಮುಗಿದ;
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl.certinfo) {
    ಫಲಿತಾಂಶ = ssl_ತಳ್ಳು_certinfo_dyn(ದತ್ತಾಂಶ, certnum, "Signature", &ಹೊರಹೋಗು);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಮುಗಿದ;
  }
  curlx_dyn_reset(&ಹೊರಹೋಗು);

  /* Generate PEM certificate. */
  ಫಲಿತಾಂಶ = curlx_base64_encode((ಸ್ಥಿರಾಂಕ uint8_t *)cert.certificate.beg,
                               cert.certificate.ಕೊನೆ - cert.certificate.beg,
                               &certptr, &clen);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಮುಗಿದ;

  /* Generate the final output certificate string. Format is:
     -----BEGIN CERTIFICATE-----\n
     <max 64 base64 characters>\n
     .
     .
     .
     -----END CERTIFICATE-----\n
   */

  curlx_dyn_reset(&ಹೊರಹೋಗು);

  /* Build the certificate string. */
  ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&ಹೊರಹೋಗು, "-----BEGIN CERTIFICATE-----\n");
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    size_t j = 0;

    ಆಗ(!ಫಲಿತಾಂಶ && (j < clen)) {
      size_t chunksize = (clen - j) > 64 ? 64 : (clen - j);
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&ಹೊರಹೋಗು, &certptr[j], chunksize);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = curlx_dyn_addn(&ಹೊರಹೋಗು, "\n", 1);
      j += chunksize;
    }
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&ಹೊರಹೋಗು, "-----END CERTIFICATE-----\n");
  }
  curlx_free(certptr);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl.certinfo)
      ಫಲಿತಾಂಶ = ssl_ತಳ್ಳು_certinfo_dyn(ದತ್ತಾಂಶ, certnum, "Cert", &ಹೊರಹೋಗು);

ಮುಗಿದ:
  ಆದರೆ(ಫಲಿತಾಂಶ)
    failf(ದತ್ತಾಂಶ, "Failed extracting certificate chain");
  curlx_dyn_free(&ಹೊರಹೋಗು);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* WANT_EXTRACT_CERTINFO */

#ಆದರೆ_ಅಂತ್ಯ /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
