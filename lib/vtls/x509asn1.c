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

#ಯದಿ defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#ಯದಿ defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_RUSTLS)
#define WANT_PARSEX509 /* uses Curl_parseX509() */
#endif

#ಯದಿ defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_RUSTLS)
#define WANT_EXTRACT_CERTINFO /* uses Curl_extract_certinfo() */
#endif

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
#define CURL_ASN1_MAX                   ((size_t)0x40000)      /* 256K */

#ifdef WANT_EXTRACT_CERTINFO

/* ASN.1 OID table entry. */
ಸಂರಚನಾ Curl_OID {
  ಧ್ರುವ ವರ್ಣ *numoid;  /* Dotted-numeric OID. */
  ಧ್ರುವ ವರ್ಣ *textoid; /* OID name. */
};

/* ASN.1 OIDs. */
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_OID OIDtable[] = {
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
  { ರಿಕ್ತ,                       ರಿಕ್ತ }
};

#endif /* WANT_EXTRACT_CERTINFO */

/*
 * Lightweight ASN.1 parser.
 * In particular, it does not check for syntactic/lexical errors.
 * It is intended to support certificate information gathering for SSL backends
 * that offer a mean to get certificates as a whole, but do not supply
 * entry points to get particular certificate sub-fields.
 * Please note there is no pretension here to rewrite a full SSL library.
 */

#define CURL_ASN1_MAX_RECURSIONS    16

ಸ್ಥಿರ ಧ್ರುವ uint8_t *getASN1Element_(ಸಂರಚನಾ Curl_asn1Element *elem,
                                      ಧ್ರುವ uint8_t *beg, ಧ್ರುವ uint8_t *end,
                                      size_t lvl)
{
  uint8_t b;
  size_t len;
  ಸಂರಚನಾ Curl_asn1Element lelem;

  /* Get a single ASN.1 element into `elem', parse ASN.1 string at `beg'
     ending at `end'.
     Returns a pointer in source string after the parsed element, or NULL
     if an error occurs. */
  ಯದಿ(!beg || !end || beg >= end || !*beg ||
     ((size_t)(end - beg) > CURL_ASN1_MAX) ||
     lvl++ >= CURL_ASN1_MAX_RECURSIONS)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  /* Process header byte. */
  elem->header = beg;
  b = *beg++;
  elem->constructed = (b & 0x20) != 0;
  elem->eclass = (b >> 6) & 3;
  b &= 0x1F;
  ಯದಿ(b == 0x1F)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; /* Long tag values not supported here. */
  elem->tag = b;

  /* Process length. */
  ಯದಿ(beg >= end)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  b = *beg++;
  ಯದಿ(!(b & 0x80))
    len = b;
  ಅನ್ಯಥಾ {
    b &= 0x7F;
    ಯದಿ(!b) {
      /* Unspecified length. Since we have all the data, we can determine the
         effective length by skipping element until an EOC element is found. */
      ಯದಿ(!elem->constructed)
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
      elem->beg = beg;
      ಯಾವತ್(beg < end && *beg) {
        beg = getASN1Element_(&lelem, beg, end, lvl);
        ಯದಿ(!beg)
          ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
      }
      ಯದಿ(end - beg < 2 || beg[1])       /* EOC must have a zero length. */
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
      elem->end = beg;                  /* Do not include EOC as content. */
      ಪ್ರತ್ಯಾಗಮ beg + 2;                   /* After EOC element. */
    }
    ಅನ್ಯಥಾ ಯದಿ(b > (size_t)(end - beg))
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; /* Does not fit in source. */
    ಅನ್ಯಥಾ {
      /* Get long length. */
      len = 0;
      ಕುರು {
        ಯದಿ(len & 0xFF000000L)
          ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;  /* Lengths > 32 bits are not supported. */
        len = (len << 8) | *beg++;
      } ಯಾವತ್(--b);
    }
  }
  ಯದಿ(len > (size_t)(end - beg))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;  /* Element data does not fit in source. */
  elem->beg = beg;
  elem->end = beg + len;
  ಪ್ರತ್ಯಾಗಮ elem->end;
}

/*
 * @unittest 1657
 */
UNITTEST ಧ್ರುವ uint8_t *getASN1Element(ಸಂರಚನಾ Curl_asn1Element *elem,
                                       ಧ್ರುವ uint8_t *beg, ಧ್ರುವ uint8_t *end);
UNITTEST ಧ್ರುವ uint8_t *getASN1Element(ಸಂರಚನಾ Curl_asn1Element *elem,
                                       ಧ್ರುವ uint8_t *beg, ಧ್ರುವ uint8_t *end)
{
  ಪ್ರತ್ಯಾಗಮ getASN1Element_(elem, beg, end, 0);
}

#ifdef WANT_EXTRACT_CERTINFO

/*
 * Search the null-terminated OID or OID identifier in local table.
 * Return the table entry pointer or NULL if not found.
 */
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_OID *ಹುಡುಕು_OID(ಧ್ರುವ ವರ್ಣ *oid)
{
  ಧ್ರುವ ಸಂರಚನಾ Curl_OID *op;
  ಪ್ರತಿ(op = OIDtable; op->numoid; op++)
    ಯದಿ(!strcmp(op->numoid, oid) || curl_strequal(op->textoid, oid))
      ಪ್ರತ್ಯಾಗಮ op;

  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Convert an ASN.1 Boolean value into its string representation.
 *
 * Return error code.
 */

ಸ್ಥಿರ CURLcode bool2str(ಸಂರಚನಾ dynbuf *store,
                         ಧ್ರುವ uint8_t *beg, ಧ್ರುವ uint8_t *end)
{
  uint8_t val;
  ಯದಿ(end - beg != 1)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  val = *beg;
  ಯದಿ((val != 0xff) && (val != 0x00))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ಪ್ರತ್ಯಾಗಮ curlx_dyn_add(store, val ? "TRUE" : "FALSE");
}

/*
 * Convert an ASN.1 octet string to a printable string.
 *
 * Return error code.
 */
ಸ್ಥಿರ CURLcode octet2str(ಸಂರಚನಾ dynbuf *store,
                          ಧ್ರುವ uint8_t *beg, ಧ್ರುವ uint8_t *end)
{
  CURLcode result = CURLE_OK;

  ಯಾವತ್(!result && beg < end)
    result = curlx_dyn_addf(store, "%02x:", *beg++);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode bit2str(ಸಂರಚನಾ dynbuf *store,
                        ಧ್ರುವ uint8_t *beg, ಧ್ರುವ uint8_t *end)
{
  /* Convert an ASN.1 bit string to a printable string. */

  ಯದಿ(beg < end) {
    uint8_t unused_bits = *beg++;
    ಯದಿ(unused_bits > 7)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ಪ್ರತ್ಯಾಗಮ octet2str(store, beg, end);
}

/*
 * Convert an ASN.1 integer value into its string representation.
 *
 * Returns error.
 */
ಸ್ಥಿರ CURLcode int2str(ಸಂರಚನಾ dynbuf *store,
                        ಧ್ರುವ uint8_t *beg, ಧ್ರುವ uint8_t *end)
{
  uint32_t uval = 0;
  int32_t sval = 0;
  size_t n = end - beg;
  size_t i;
  ಧ್ರುವ uint8_t *p = beg;

  ಯದಿ(!n)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  ಯದಿ(n > 4)
    ಪ್ರತ್ಯಾಗಮ octet2str(store, beg, end);

  ಯದಿ(p[0] & 0x80)
    uval = ~0U;

  ಪ್ರತಿ(i = 0; i < n; i++)
    uval = (uval << 8) | p[i];

  sval = (int32_t)uval;

  /* small values are easier to read as decimals */
  ಯದಿ(sval > -10000 && sval < 10000)
    ಪ್ರತ್ಯಾಗಮ curlx_dyn_addf(store, "%d", sval);

  /* represent larger values as hex */
  ಪ್ರತ್ಯಾಗಮ curlx_dyn_addf(store, "0x%x", uval);
}

/*
 * Convert from an ASN.1 typed string to UTF8.
 *
 * The result is stored in a dynbuf that is inited by the user of this
 * function.
 *
 * Returns error.
 */
ಸ್ಥಿರ CURLcode utf8asn1str(ಸಂರಚನಾ dynbuf *to, ಪೂರ್ಣಾಙ್ಕ type,
                            ಧ್ರುವ uint8_t *from, ಧ್ರುವ uint8_t *end)
{
  size_t inlength = end - from;
  ಪೂರ್ಣಾಙ್ಕ size = 1;
  CURLcode result = CURLE_OK;

  ವಿಕಲ್ಪ(type) {
  ಪಕ್ಷ CURL_ASN1_BMP_STRING:
    size = 2;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_ASN1_UNIVERSAL_STRING:
    size = 4;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_ASN1_NUMERIC_STRING:
  ಪಕ್ಷ CURL_ASN1_PRINTABLE_STRING:
  ಪಕ್ಷ CURL_ASN1_TELETEX_STRING:
  ಪಕ್ಷ CURL_ASN1_IA5_STRING:
  ಪಕ್ಷ CURL_ASN1_VISIBLE_STRING:
  ಪಕ್ಷ CURL_ASN1_UTF8_STRING:
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;  /* Conversion not supported. */
  }

  ಯದಿ(inlength % size)
    /* Length inconsistent with character size. */
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  ಯದಿ(type == CURL_ASN1_UTF8_STRING) {
    /* copy. */
    ಯದಿ(inlength)
      result = curlx_dyn_addn(to, from, inlength);
  }
  ಅನ್ಯಥಾ {
    ಯಾವತ್(!result && (from < end)) {
      ವರ್ಣ buf[4]; /* decode buffer */
      size_t charsize = 1;
      unsigned ಪೂರ್ಣಾಙ್ಕ wc = 0;

      ವಿಕಲ್ಪ(size) {
      ಪಕ್ಷ 4:
        wc = (wc << 8) | *from++;
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      ಪಕ್ಷ 2:
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      ಸ್ವಭಾವ: /* case 1: */
        wc = (wc << 8) | *from++;
      }
      ಯದಿ(wc >= 0x00000080) {
        ಯದಿ(wc >= 0x00000800) {
          ಯದಿ(wc >= 0x00010000) {
            ಯದಿ(wc >= 0x00200000) {
              /* Invalid char. size for target encoding. */
              ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
            }
            buf[3] = (ವರ್ಣ)(0x80 | (wc & 0x3F));
            wc = (wc >> 6) | 0x00010000;
            charsize++;
          }
          buf[2] = (ವರ್ಣ)(0x80 | (wc & 0x3F));
          wc = (wc >> 6) | 0x00000800;
          charsize++;
        }
        buf[1] = (ವರ್ಣ)(0x80 | (wc & 0x3F));
        wc = (wc >> 6) | 0x000000C0;
        charsize++;
      }
      buf[0] = (ವರ್ಣ)wc;
      result = curlx_dyn_addn(to, buf, charsize);
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
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
UNITTEST CURLcode encodeOID(ಸಂರಚನಾ dynbuf *store,
                            ಧ್ರುವ uint8_t *beg, ಧ್ರುವ uint8_t *end);
UNITTEST CURLcode encodeOID(ಸಂರಚನಾ dynbuf *store,
                            ಧ್ರುವ uint8_t *beg, ಧ್ರುವ uint8_t *end)
{
  uint32_t x;
  uint32_t y;
  CURLcode result = CURLE_OK;

  ಯದಿ(end <= beg)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  /* Process the first two numbers. The initial digit cannot be larger than
     2 */
  y = *beg++;
  ಯದಿ(y <= 80)
    x = y / 40;
  ಅನ್ಯಥಾ
    x = 2;
  result = curlx_dyn_addf(store, "%u", x);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  ಯದಿ(y & 0x80) {
    uint32_t t = (y & 0x7f); /* the second number */
    ಯದಿ(!t)
      /* reject leading 0x80 padding */
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    ಕುರು {
      ಯದಿ((t & 0xFE000000) || (beg >= end))
        ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      t = (t << 7) | (y & 0x7F);
    } ಯಾವತ್(y & 0x80);
    y = t;
  }

  result = curlx_dyn_addf(store, ".%u", y - (x * 40));
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* Process the trailing numbers. */
  ಯಾವತ್((beg < end) && !result) {
    x = 0;
    y = 0;
    ಕುರು {
      /* 0x80 as the first value is invalid since it only adds padding */
      ಯದಿ((y & 0x80) && !x)
        ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
      ಅನ್ಯಥಾ ಯದಿ((x & 0xFE000000) || (beg == end))
        ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      x = (x << 7) | (y & 0x7F);
    } ಯಾವತ್(y & 0x80);
    result = curlx_dyn_addf(store, ".%u", x);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Convert an ASN.1 OID into its dotted or symbolic string representation.
 *
 * Return error code.
 */

ಸ್ಥಿರ CURLcode OID2str(ಸಂರಚನಾ dynbuf *store,
                        ಧ್ರುವ uint8_t *beg, ಧ್ರುವ uint8_t *end)
{
  CURLcode result = CURLE_OK;
  ಯದಿ(beg < end) {
    ಸಂರಚನಾ dynbuf buf;
    curlx_dyn_init(&buf, CURL_X509_STR_MAX);
    result = encodeOID(&buf, beg, end);

    ಯದಿ(!result) {
      ಧ್ರುವ ಸಂರಚನಾ Curl_OID *op = ಹುಡುಕು_OID(curlx_dyn_ptr(&buf));
      ಯದಿ(op)
        result = curlx_dyn_add(store, op->textoid);
      ಅನ್ಯಥಾ
        result = curlx_dyn_add(store, curlx_dyn_ptr(&buf));
    }
    curlx_dyn_free(&buf);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * @unittest 1656
 */
UNITTEST CURLcode GTime2str(ಸಂರಚನಾ dynbuf *store,
                            ಧ್ರುವ ವರ್ಣ *beg, ಧ್ರುವ ವರ್ಣ *end);
UNITTEST CURLcode GTime2str(ಸಂರಚನಾ dynbuf *store,
                            ಧ್ರುವ ವರ್ಣ *beg, ಧ್ರುವ ವರ್ಣ *end)
{
  ಧ್ರುವ ವರ್ಣ *tzp;
  ಧ್ರುವ ವರ್ಣ *fracp;
  ವರ್ಣ sec1, sec2;
  size_t fracl;
  size_t tzl;
  ಧ್ರುವ ವರ್ಣ *sep = "";

  /* Convert an ASN.1 Generalized time to a printable string.
     Return the dynamically allocated string, or NULL if an error occurs. */

  ಪ್ರತಿ(fracp = beg; fracp < end && ISDIGIT(*fracp); fracp++)
    ;

  /* Get seconds digits. */
  sec1 = '0';
  ವಿಕಲ್ಪ(fracp - beg - 12) {
  ಪಕ್ಷ 0:
    sec2 = '0';
    ಭಙ್ಗ;
  ಪಕ್ಷ 2:
    sec1 = fracp[-2];
    FALLTHROUGH();
  ಪಕ್ಷ 1:
    sec2 = fracp[-1];
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* timezone follows optional fractional seconds. */
  tzp = fracp;
  fracl = 0; /* no fractional seconds detected so far */
  ಯದಿ(fracp < end && (*fracp == '.' || *fracp == ',')) {
    /* Have fractional seconds, e.g. "[.,]\d+". How many? */
    fracp++; /* should be a digit char or BAD ARGUMENT */
    tzp = fracp;
    ಯಾವತ್(tzp < end && ISDIGIT(*tzp))
      tzp++;
    ಯದಿ(tzp == fracp) /* never looped, no digit after [.,] */
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    fracl = tzp - fracp; /* number of fractional sec digits */
    DEBUGASSERT(fracl > 0);
    /* Strip trailing zeroes in fractional seconds.
     * May reduce fracl to 0 if only '0's are present. */
    ಯಾವತ್(fracl && fracp[fracl - 1] == '0')
      fracl--;
  }

  /* Process timezone. */
  ಯದಿ(tzp >= end) {
    tzp = "";
    tzl = 0;
  }
  ಅನ್ಯಥಾ ಯದಿ(*tzp == 'Z') {
    sep = " ";
    tzp = "GMT";
    tzl = 3;
  }
  ಅನ್ಯಥಾ ಯದಿ((*tzp == '+') || (*tzp == '-')) {
    sep = " UTC";
    tzl = end - tzp;
  }
  ಅನ್ಯಥಾ {
    sep = " ";
    tzl = end - tzp;
  }

  ಪ್ರತ್ಯಾಗಮ curlx_dyn_addf(store,
                        "%.4s-%.2s-%.2s %.2s:%.2s:%c%c%s%.*s%s%.*s",
                        beg, beg + 4, beg + 6,
                        beg + 8, beg + 10, sec1, sec2,
                        fracl ? "." : "", (ಪೂರ್ಣಾಙ್ಕ)fracl, fracp,
                        sep, (ಪೂರ್ಣಾಙ್ಕ)tzl, tzp);
}

/*
 * Convert an ASN.1 UTC time to a printable string.
 *
 * Return error code.
 */
ಸ್ಥಿರ CURLcode UTime2str(ಸಂರಚನಾ dynbuf *store,
                          ಧ್ರುವ ವರ್ಣ *beg, ಧ್ರುವ ವರ್ಣ *end)
{
  ಧ್ರುವ ವರ್ಣ *tzp;
  size_t tzl;
  ಧ್ರುವ ವರ್ಣ *sec;

  ಪ್ರತಿ(tzp = beg; tzp < end && *tzp >= '0' && *tzp <= '9'; tzp++)
    ;
  /* Get the seconds. */
  sec = beg + 10;
  ವಿಕಲ್ಪ(tzp - sec) {
  ಪಕ್ಷ 0:
    sec = "00";
    FALLTHROUGH();
  ಪಕ್ಷ 2:
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Process timezone. */
  ಯದಿ(tzp >= end)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ಯದಿ(*tzp == 'Z') {
    tzp = "GMT";
    end = tzp + 3;
  }

  tzl = end - tzp;
  ಪ್ರತ್ಯಾಗಮ curlx_dyn_addf(store, "%d%.2s-%.2s-%.2s %.2s:%.2s:%.2s %.*s",
                        20 - (*beg >= '5'), beg, beg + 2, beg + 4,
                        beg + 6, beg + 8, sec,
                        (ಪೂರ್ಣಾಙ್ಕ)tzl, tzp);
}

/*
 * Convert an ASN.1 element to a printable string.
 *
 * Return error
 *
 * @unittest 1667
 */
UNITTEST CURLcode ASN1tostr(ಸಂರಚನಾ dynbuf *store,
                            ಸಂರಚನಾ Curl_asn1Element *elem);
UNITTEST CURLcode ASN1tostr(ಸಂರಚನಾ dynbuf *store,
                            ಸಂರಚನಾ Curl_asn1Element *elem)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  ಪೂರ್ಣಾಙ್ಕ type = elem->tag;
  ಯದಿ(elem->constructed)
    ಪ್ರತ್ಯಾಗಮ result; /* No conversion of structured elements. */

  ವಿಕಲ್ಪ(type) {
  ಪಕ್ಷ CURL_ASN1_BOOLEAN:
    result = bool2str(store, elem->beg, elem->end);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_ASN1_INTEGER:
  ಪಕ್ಷ CURL_ASN1_ENUMERATED:
    result = int2str(store, elem->beg, elem->end);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_ASN1_BIT_STRING:
    result = bit2str(store, elem->beg, elem->end);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_ASN1_OCTET_STRING:
    result = octet2str(store, elem->beg, elem->end);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_ASN1_NULL:
    result = curlx_dyn_addn(store, "", 1);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_ASN1_OBJECT_IDENTIFIER:
    result = OID2str(store, elem->beg, elem->end);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_ASN1_UTC_TIME:
    result = UTime2str(store, (ಧ್ರುವ ವರ್ಣ *)elem->beg,
                       (ಧ್ರುವ ವರ್ಣ *)elem->end);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_ASN1_GENERALIZED_TIME:
    result = GTime2str(store, (ಧ್ರುವ ವರ್ಣ *)elem->beg,
                       (ಧ್ರುವ ವರ್ಣ *)elem->end);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_ASN1_UTF8_STRING:
  ಪಕ್ಷ CURL_ASN1_NUMERIC_STRING:
  ಪಕ್ಷ CURL_ASN1_PRINTABLE_STRING:
  ಪಕ್ಷ CURL_ASN1_TELETEX_STRING:
  ಪಕ್ಷ CURL_ASN1_IA5_STRING:
  ಪಕ್ಷ CURL_ASN1_VISIBLE_STRING:
  ಪಕ್ಷ CURL_ASN1_UNIVERSAL_STRING:
  ಪಕ್ಷ CURL_ASN1_BMP_STRING:
    result = utf8asn1str(store, type, elem->beg, elem->end);
    ಭಙ್ಗ;
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * ASCII encode distinguished name at `dn' into the store dynbuf.
 *
 * Returns error.
 */
ಸ್ಥಿರ CURLcode encodeDN(ಸಂರಚನಾ dynbuf *store, ಸಂರಚನಾ Curl_asn1Element *dn)
{
  ಸಂರಚನಾ Curl_asn1Element rdn;
  ಸಂರಚನಾ Curl_asn1Element atv;
  ಸಂರಚನಾ Curl_asn1Element oid;
  ಸಂರಚನಾ Curl_asn1Element ಮೌಲ್ಯ;
  ಧ್ರುವ uint8_t *p1;
  ಧ್ರುವ uint8_t *p2;
  ಧ್ರುವ uint8_t *p3;
  ಧ್ರುವ uint8_t *str;
  CURLcode result = CURLE_OK;
  bool added = FALSE;
  ಸಂರಚನಾ dynbuf temp;
  curlx_dyn_init(&temp, CURL_X509_STR_MAX);

  ಪ್ರತಿ(p1 = dn->beg; p1 < dn->end;) {
    p1 = getASN1Element(&rdn, p1, dn->end);
    ಯದಿ(!p1) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      ಗಚ್ಛ error;
    }
    ಪ್ರತಿ(p2 = rdn.beg; p2 < rdn.end;) {
      p2 = getASN1Element(&atv, p2, rdn.end);
      ಯದಿ(!p2) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        ಗಚ್ಛ error;
      }
      p3 = getASN1Element(&oid, atv.beg, atv.end);
      ಯದಿ(!p3) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        ಗಚ್ಛ error;
      }
      ಯದಿ(!getASN1Element(&ಮೌಲ್ಯ, p3, atv.end)) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        ಗಚ್ಛ error;
      }
      curlx_dyn_reset(&temp);
      result = ASN1tostr(&temp, &oid);
      ಯದಿ(result)
        ಗಚ್ಛ error;

      str = curlx_dyn_uptr(&temp);

      ಯದಿ(!str) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        ಗಚ್ಛ error;
      }

      /* Encode delimiter.
         If attribute has a short uppercase name, delimiter is ", ". */
      ಪ್ರತಿ(p3 = str; ISUPPER(*p3); p3++)
        ;
      ಯದಿ(added) {
        ಯದಿ(p3 - str > 2)
          result = curlx_dyn_addn(store, "/", 1);
        ಅನ್ಯಥಾ
          result = curlx_dyn_addn(store, ", ", 2);
        ಯದಿ(result)
          ಗಚ್ಛ error;
      }

      /* Encode attribute name. */
      result = curlx_dyn_add(store, (ಧ್ರುವ ವರ್ಣ *)str);
      ಯದಿ(result)
        ಗಚ್ಛ error;

      /* Generate equal sign. */
      result = curlx_dyn_addn(store, "=", 1);
      ಯದಿ(result)
        ಗಚ್ಛ error;

      /* Generate value. */
      result = ASN1tostr(store, &ಮೌಲ್ಯ);
      ಯದಿ(result)
        ಗಚ್ಛ error;
      curlx_dyn_reset(&temp);
      added = TRUE; /* use separator for next */
    }
  }
error:
  curlx_dyn_free(&temp);

  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#ifdef WANT_PARSEX509
/*
 * ASN.1 parse an X509 certificate into structure subfields.
 * Syntax is assumed to have already been checked by the SSL backend.
 * See RFC 5280.
 */
ಪೂರ್ಣಾಙ್ಕ Curl_parseX509(ಸಂರಚನಾ Curl_X509certificate *cert,
                   ಧ್ರುವ uint8_t *beg, ಧ್ರುವ uint8_t *end)
{
  ಸಂರಚನಾ Curl_asn1Element elem;
  ಸಂರಚನಾ Curl_asn1Element tbsCertificate;
  ಧ್ರುವ uint8_t *ccp;
  ಸ್ಥಿರ ಧ್ರುವ uint8_t defaultVersion = 0;  /* v1. */

  memset(cert, 0, ಪರಿಮಾಣ(*cert));
  cert->certificate.header = ರಿಕ್ತ;
  cert->certificate.beg = beg;
  cert->certificate.end = end;

  /* Get the sequence content. */
  ಯದಿ(!getASN1Element(&elem, beg, end))
    ಪ್ರತ್ಯಾಗಮ -1;  /* Invalid bounds/size. */
  beg = elem.beg;
  end = elem.end;

  /* Get tbsCertificate. */
  beg = getASN1Element(&tbsCertificate, beg, end);
  ಯದಿ(!beg)
    ಪ್ರತ್ಯಾಗಮ -1;
  /* Skip the signatureAlgorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  ಯದಿ(!beg)
    ಪ್ರತ್ಯಾಗಮ -1;
  /* Get the signatureValue. */
  ಯದಿ(!getASN1Element(&cert->signature, beg, end))
    ಪ್ರತ್ಯಾಗಮ -1;

  /* Parse TBSCertificate. */
  beg = tbsCertificate.beg;
  end = tbsCertificate.end;
  /* Get optional version, get serialNumber. */
  cert->version.header = ರಿಕ್ತ;
  cert->version.beg = &defaultVersion;
  cert->version.end = &defaultVersion + ಪರಿಮಾಣ(defaultVersion);
  beg = getASN1Element(&elem, beg, end);
  ಯದಿ(!beg)
    ಪ್ರತ್ಯಾಗಮ -1;
  ಯದಿ(elem.tag == 0) {
    ಯದಿ(!getASN1Element(&cert->version, elem.beg, elem.end))
      ಪ್ರತ್ಯಾಗಮ -1;
    beg = getASN1Element(&elem, beg, end);
    ಯದಿ(!beg)
      ಪ್ರತ್ಯಾಗಮ -1;
  }
  cert->serial_ಸಂಖ್ಯೆ = elem;
  /* Get signature algorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  /* Get issuer. */
  beg = getASN1Element(&cert->issuer, beg, end);
  ಯದಿ(!beg)
    ಪ್ರತ್ಯಾಗಮ -1;
  /* Get notBefore and notAfter. */
  beg = getASN1Element(&elem, beg, end);
  ಯದಿ(!beg)
    ಪ್ರತ್ಯಾಗಮ -1;
  ccp = getASN1Element(&cert->notBefore, elem.beg, elem.end);
  ಯದಿ(!ccp)
    ಪ್ರತ್ಯಾಗಮ -1;
  ಯದಿ(!getASN1Element(&cert->notAfter, ccp, elem.end))
    ಪ್ರತ್ಯಾಗಮ -1;
  /* These two fields MUST consume all of the validity elem */
  ಯದಿ(elem.end != cert->notAfter.end)
    ಪ್ರತ್ಯಾಗಮ -1;
  /* Get subject. */
  beg = getASN1Element(&cert->subject, beg, end);
  ಯದಿ(!beg)
    ಪ್ರತ್ಯಾಗಮ -1;
  /* Get subjectPublicKeyAlgorithm and subjectPublicKey. */
  beg = getASN1Element(&cert->subject_Public_ಕೀಲಿ_Info, beg, end);
  ಯದಿ(!beg)
    ಪ್ರತ್ಯಾಗಮ -1;
  ccp = getASN1Element(&cert->subject_Public_ಕೀಲಿ_Algorithm,
                       cert->subject_Public_ಕೀಲಿ_Info.beg,
                       cert->subject_Public_ಕೀಲಿ_Info.end);
  ಯದಿ(!ccp)
    ಪ್ರತ್ಯಾಗಮ -1;
  ಯದಿ(!getASN1Element(&cert->subject_Public_ಕೀಲಿ, ccp,
                     cert->subject_Public_ಕೀಲಿ_Info.end))
    ಪ್ರತ್ಯಾಗಮ -1;
  /* Get optional issuerUniqueID, subjectUniqueID and extensions. */
  cert->issuerUniqueID.tag = cert->subjectUniqueID.tag = 0;
  cert->extensions.tag = elem.tag = 0;
  cert->issuerUniqueID.header = cert->subjectUniqueID.header = ರಿಕ್ತ;
  cert->issuerUniqueID.beg = cert->issuerUniqueID.end = beg;
  cert->subjectUniqueID.beg = cert->subjectUniqueID.end = beg;
  cert->extensions.header = ರಿಕ್ತ;
  cert->extensions.beg = cert->extensions.end = beg;
  ಯದಿ(beg < end) {
    beg = getASN1Element(&elem, beg, end);
    ಯದಿ(!beg)
      ಪ್ರತ್ಯಾಗಮ -1;
  }
  ಯದಿ(elem.tag == 1) {
    cert->issuerUniqueID = elem;
    ಯದಿ(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      ಯದಿ(!beg)
        ಪ್ರತ್ಯಾಗಮ -1;
    }
  }
  ಯದಿ(elem.tag == 2) {
    cert->subjectUniqueID = elem;
    ಯದಿ(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      ಯದಿ(!beg)
        ಪ್ರತ್ಯಾಗಮ -1;
    }
  }
  ಯದಿ(elem.tag == 3)
    ಯದಿ(!getASN1Element(&cert->extensions, elem.beg, elem.end))
      ಪ್ರತ್ಯಾಗಮ -1;
  ಪ್ರತ್ಯಾಗಮ 0;
}

#endif /* WANT_PARSEX509 */

#ifdef WANT_EXTRACT_CERTINFO

ಸ್ಥಿರ CURLcode dumpAlgo(ಸಂರಚನಾ dynbuf *store,
                         ಸಂರಚನಾ Curl_asn1Element *param,
                         ಧ್ರುವ uint8_t *beg, ಧ್ರುವ uint8_t *end)
{
  ಸಂರಚನಾ Curl_asn1Element oid;

  /* Get algorithm parameters and return algorithm name. */

  beg = getASN1Element(&oid, beg, end);
  ಯದಿ(!beg)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  param->header = ರಿಕ್ತ;
  param->tag = 0;
  param->beg = param->end = end;
  ಯದಿ(beg < end) {
    ಧ್ರುವ uint8_t *p = getASN1Element(param, beg, end);
    ಯದಿ(!p)
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  ಪ್ರತ್ಯಾಗಮ OID2str(store, oid.beg, oid.end);
}

/*
 * This is a convenience function for push_certinfo_len that takes a zero
 * terminated value.
 */
ಸ್ಥಿರ CURLcode ssl_ತಳ್ಳು_certinfo(ಸಂರಚನಾ Curl_easy *data,
                                  ಪೂರ್ಣಾಙ್ಕ certnum,
                                  ಧ್ರುವ ವರ್ಣ *label,
                                  ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ)
{
  size_t valuelen = strlen(ಮೌಲ್ಯ);

  ಪ್ರತ್ಯಾಗಮ Curl_ssl_ತಳ್ಳು_certinfo_len(data, certnum, label, ಮೌಲ್ಯ, valuelen);
}

/*
 * This is a convenience function for push_certinfo_len that takes a
 * dynbuf value.
 *
 * It also does the verbose output if !certnum.
 */
ಸ್ಥಿರ CURLcode ssl_ತಳ್ಳು_certinfo_dyn(ಸಂರಚನಾ Curl_easy *data,
                                      ಪೂರ್ಣಾಙ್ಕ certnum,
                                      ಧ್ರುವ ವರ್ಣ *label,
                                      ಸಂರಚನಾ dynbuf *ptr)
{
  size_t valuelen = curlx_dyn_len(ptr);
  ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ = curlx_dyn_ptr(ptr);

  CURLcode result = Curl_ssl_ತಳ್ಳು_certinfo_len(data, certnum, label,
                                               ಮೌಲ್ಯ, valuelen);

  ಯದಿ(!certnum && !result)
    infof(data, "   %s: %s", label, ಮೌಲ್ಯ);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode do_pubkey_field(ಸಂರಚನಾ Curl_easy *data, ಪೂರ್ಣಾಙ್ಕ certnum,
                                ಧ್ರುವ ವರ್ಣ *label,
                                ಸಂರಚನಾ Curl_asn1Element *elem)
{
  CURLcode result;
  ಸಂರಚನಾ dynbuf out;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);

  /* Generate a certificate information record for the public key. */

  result = ASN1tostr(&out, elem);
  ಯದಿ(!result && data->set.ssl.certinfo)
    result = ssl_ತಳ್ಳು_certinfo_dyn(data, certnum, label, &out);
  curlx_dyn_free(&out);
  ಪ್ರತ್ಯಾಗಮ result;
}

/* return 0 on success, 1 on error */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ do_pubkey(ಸಂರಚನಾ Curl_easy *data, ಪೂರ್ಣಾಙ್ಕ certnum, ಧ್ರುವ ವರ್ಣ *algo,
                     ಸಂರಚನಾ Curl_asn1Element *param,
                     ಸಂರಚನಾ Curl_asn1Element *pubkey)
{
  ಸಂರಚನಾ Curl_asn1Element elem;
  ಸಂರಚನಾ Curl_asn1Element pk;
  ಧ್ರುವ uint8_t *p;

  /* Generate all information records for the public key. */

  ಯದಿ(curl_strequal(algo, "ecPublicKey")) {
    /*
     * ECC public key is all the data, a value of type BIT STRING mapped to
     * OCTET STRING and should not be parsed as an ASN.1 value.
     */
    ಧ್ರುವ size_t dlen = pubkey->end - pubkey->beg;
    size_t len;
    ಯದಿ(dlen < 2)
      /* too small */
      ಪ್ರತ್ಯಾಗಮ 1;
    len = (dlen - 2) * 4;
    ಯದಿ(!certnum)
      infof(data, "   ECC Public Key (%zu bits)", len);
    ಯದಿ(data->set.ssl.certinfo) {
      ವರ್ಣ q[(ಪರಿಮಾಣ(len) * 8 / 3) + 1];
      (ಶೂನ್ಯ)curl_msnprintf(q, ಪರಿಮಾಣ(q), "%zu", len);
      ಯದಿ(ssl_ತಳ್ಳು_certinfo(data, certnum, "ECC Public Key", q))
        ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಪ್ರತ್ಯಾಗಮ do_pubkey_field(data, certnum, "ecPublicKey", pubkey) == CURLE_OK
      ? 0 : 1;
  }

  /* Get the public key (single element). */
  ಯದಿ(!getASN1Element(&pk, pubkey->beg + 1, pubkey->end))
    ಪ್ರತ್ಯಾಗಮ 1;

  ಯದಿ(curl_strequal(algo, "rsaEncryption")) {
    ಧ್ರುವ uint8_t *q;
    size_t len;

    p = getASN1Element(&elem, pk.beg, pk.end);
    ಯದಿ(!p)
      ಪ್ರತ್ಯಾಗಮ 1;

    /* Compute key length. */
    ಪ್ರತಿ(q = elem.beg; q < elem.end && !*q; q++)
      ;
    len = ((elem.end - q) * 8);
    ಯದಿ(len) {
      unsigned ಪೂರ್ಣಾಙ್ಕ i;
      ಪ್ರತಿ(i = *q; !(i & 0x80); i <<= 1)
        len--;
    }
    ಯದಿ(len > 32)
      elem.beg = q;     /* Strip leading zero bytes. */
    ಯದಿ(!certnum)
      infof(data, "   RSA Public Key (%zu bits)", len);
    ಯದಿ(data->set.ssl.certinfo) {
      ವರ್ಣ r[(ಪರಿಮಾಣ(len) * 8 / 3) + 1];
      curl_msnprintf(r, ಪರಿಮಾಣ(r), "%zu", len);
      ಯದಿ(ssl_ತಳ್ಳು_certinfo(data, certnum, "RSA Public Key", r))
        ಪ್ರತ್ಯಾಗಮ 1;
    }
    /* Generate coefficients. */
    ಯದಿ(do_pubkey_field(data, certnum, "rsa(n)", &elem))
      ಪ್ರತ್ಯಾಗಮ 1;
    ಯದಿ(!getASN1Element(&elem, p, pk.end))
      ಪ್ರತ್ಯಾಗಮ 1;
    ಯದಿ(do_pubkey_field(data, certnum, "rsa(e)", &elem))
      ಪ್ರತ್ಯಾಗಮ 1;
  }
  ಅನ್ಯಥಾ ಯದಿ(curl_strequal(algo, "dsa")) {
    p = getASN1Element(&elem, param->beg, param->end);
    ಯದಿ(p) {
      ಯದಿ(do_pubkey_field(data, certnum, "dsa(p)", &elem))
        ಪ್ರತ್ಯಾಗಮ 1;
      p = getASN1Element(&elem, p, param->end);
      ಯದಿ(p) {
        ಯದಿ(do_pubkey_field(data, certnum, "dsa(q)", &elem))
          ಪ್ರತ್ಯಾಗಮ 1;
        ಯದಿ(getASN1Element(&elem, p, param->end)) {
          ಯದಿ(do_pubkey_field(data, certnum, "dsa(g)", &elem))
            ಪ್ರತ್ಯಾಗಮ 1;
          ಯದಿ(do_pubkey_field(data, certnum, "dsa(pub_key)", &pk))
            ಪ್ರತ್ಯಾಗಮ 1;
        }
      }
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(curl_strequal(algo, "dhpublicnumber")) {
    p = getASN1Element(&elem, param->beg, param->end);
    ಯದಿ(p) {
      ಯದಿ(do_pubkey_field(data, certnum, "dh(p)", &elem))
        ಪ್ರತ್ಯಾಗಮ 1;
      ಯದಿ(getASN1Element(&elem, p, param->end)) {
        ಯದಿ(do_pubkey_field(data, certnum, "dh(g)", &elem))
          ಪ್ರತ್ಯಾಗಮ 1;
        ಯದಿ(do_pubkey_field(data, certnum, "dh(pub_key)", &pk))
          ಪ್ರತ್ಯಾಗಮ 1;
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Convert an ASN.1 distinguished name into a printable string.
 * Return error.
 */
ಸ್ಥಿರ CURLcode DNtostr(ಸಂರಚನಾ dynbuf *store, ಸಂರಚನಾ Curl_asn1Element *dn)
{
  ಪ್ರತ್ಯಾಗಮ encodeDN(store, dn);
}

CURLcode Curl_extract_certinfo(ಸಂರಚನಾ Curl_easy *data,
                               ಪೂರ್ಣಾಙ್ಕ certnum,
                               ಧ್ರುವ uint8_t *beg,
                               ಧ್ರುವ uint8_t *end)
{
  ಸಂರಚನಾ Curl_X509certificate cert;
  ಸಂರಚನಾ Curl_asn1Element param;
  ವರ್ಣ *certptr;
  size_t clen;
  ಸಂರಚನಾ dynbuf out;
  CURLcode result = CURLE_OK;
  unsigned ಪೂರ್ಣಾಙ್ಕ version;
  ಧ್ರುವ uint8_t *ptr;
  ಪೂರ್ಣಾಙ್ಕ rc;

  ಯದಿ(!data->set.ssl.certinfo)
    ಯದಿ(certnum)
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);
  /* Prepare the certificate information for curl_easy_getinfo(). */

  /* Extract the certificate ASN.1 elements. */
  ಯದಿ(Curl_parseX509(&cert, beg, end))
    ಪ್ರತ್ಯಾಗಮ CURLE_PEER_FAILED_VERIFICATION;

  /* Subject. */
  result = DNtostr(&out, &cert.subject);
  ಯದಿ(result)
    ಗಚ್ಛ done;
  ಯದಿ(data->set.ssl.certinfo) {
    result = ssl_ತಳ್ಳು_certinfo_dyn(data, certnum, "Subject", &out);
    ಯದಿ(result)
      ಗಚ್ಛ done;
  }
  curlx_dyn_reset(&out);

  /* Issuer. */
  result = DNtostr(&out, &cert.issuer);
  ಯದಿ(result)
    ಗಚ್ಛ done;
  ಯದಿ(data->set.ssl.certinfo) {
    result = ssl_ತಳ್ಳು_certinfo_dyn(data, certnum, "Issuer", &out);
    ಯದಿ(result)
      ಗಚ್ಛ done;
  }
  curlx_dyn_reset(&out);

  /* Version (always fits in less than 32 bits). */
  version = 0;
  ಪ್ರತಿ(ptr = cert.version.beg; ptr < cert.version.end; ptr++)
    version = (version << 8) | *ptr;
  ಯದಿ(data->set.ssl.certinfo) {
    result = curlx_dyn_addf(&out, "%x", version);
    ಯದಿ(result)
      ಗಚ್ಛ done;
    result = ssl_ತಳ್ಳು_certinfo_dyn(data, certnum, "Version", &out);
    ಯದಿ(result)
      ಗಚ್ಛ done;
    curlx_dyn_reset(&out);
  }

  /* Serial number. */
  result = ASN1tostr(&out, &cert.serial_ಸಂಖ್ಯೆ);
  ಯದಿ(result)
    ಗಚ್ಛ done;
  ಯದಿ(data->set.ssl.certinfo) {
    result = ssl_ತಳ್ಳು_certinfo_dyn(data, certnum, "Serial Number", &out);
    ಯದಿ(result)
      ಗಚ್ಛ done;
  }
  curlx_dyn_reset(&out);

  /* Signature algorithm .*/
  result = dumpAlgo(&out, &param, cert.signatureAlgorithm.beg,
                    cert.signatureAlgorithm.end);
  ಯದಿ(result)
    ಗಚ್ಛ done;
  ಯದಿ(data->set.ssl.certinfo) {
    result = ssl_ತಳ್ಳು_certinfo_dyn(data, certnum, "Signature Algorithm", &out);
    ಯದಿ(result)
      ಗಚ್ಛ done;
  }
  curlx_dyn_reset(&out);

  /* Start Date. */
  result = ASN1tostr(&out, &cert.notBefore);
  ಯದಿ(result)
    ಗಚ್ಛ done;
  ಯದಿ(data->set.ssl.certinfo) {
    result = ssl_ತಳ್ಳು_certinfo_dyn(data, certnum, "Start Date", &out);
    ಯದಿ(result)
      ಗಚ್ಛ done;
  }
  curlx_dyn_reset(&out);

  /* Expire Date. */
  result = ASN1tostr(&out, &cert.notAfter);
  ಯದಿ(result)
    ಗಚ್ಛ done;
  ಯದಿ(data->set.ssl.certinfo) {
    result = ssl_ತಳ್ಳು_certinfo_dyn(data, certnum, "Expire Date", &out);
    ಯದಿ(result)
      ಗಚ್ಛ done;
  }
  curlx_dyn_reset(&out);

  /* Public Key Algorithm. */
  result = dumpAlgo(&out, &param, cert.subject_Public_ಕೀಲಿ_Algorithm.beg,
                    cert.subject_Public_ಕೀಲಿ_Algorithm.end);
  ಯದಿ(result)
    ಗಚ್ಛ done;
  ಯದಿ(data->set.ssl.certinfo) {
    result = ssl_ತಳ್ಳು_certinfo_dyn(data, certnum, "Public Key Algorithm",
                                   &out);
    ಯದಿ(result)
      ಗಚ್ಛ done;
  }

  rc = do_pubkey(data, certnum, curlx_dyn_ptr(&out),
                 &param, &cert.subject_Public_ಕೀಲಿ);
  ಯದಿ(rc) {
    result = CURLE_OUT_OF_MEMORY; /* the most likely error */
    ಗಚ್ಛ done;
  }
  curlx_dyn_reset(&out);

  /* Signature. */
  result = ASN1tostr(&out, &cert.signature);
  ಯದಿ(result)
    ಗಚ್ಛ done;
  ಯದಿ(data->set.ssl.certinfo) {
    result = ssl_ತಳ್ಳು_certinfo_dyn(data, certnum, "Signature", &out);
    ಯದಿ(result)
      ಗಚ್ಛ done;
  }
  curlx_dyn_reset(&out);

  /* Generate PEM certificate. */
  result = curlx_base64_encode((ಧ್ರುವ uint8_t *)cert.certificate.beg,
                               cert.certificate.end - cert.certificate.beg,
                               &certptr, &clen);
  ಯದಿ(result)
    ಗಚ್ಛ done;

  /* Generate the final output certificate string. Format is:
     -----BEGIN CERTIFICATE-----\n
     <max 64 base64 characters>\n
     .
     .
     .
     -----END CERTIFICATE-----\n
   */

  curlx_dyn_reset(&out);

  /* Build the certificate string. */
  result = curlx_dyn_add(&out, "-----BEGIN CERTIFICATE-----\n");
  ಯದಿ(!result) {
    size_t j = 0;

    ಯಾವತ್(!result && (j < clen)) {
      size_t chunksize = (clen - j) > 64 ? 64 : (clen - j);
      result = curlx_dyn_addn(&out, &certptr[j], chunksize);
      ಯದಿ(!result)
        result = curlx_dyn_addn(&out, "\n", 1);
      j += chunksize;
    }
    ಯದಿ(!result)
      result = curlx_dyn_add(&out, "-----END CERTIFICATE-----\n");
  }
  curlx_free(certptr);
  ಯದಿ(!result)
    ಯದಿ(data->set.ssl.certinfo)
      result = ssl_ತಳ್ಳು_certinfo_dyn(data, certnum, "Cert", &out);

done:
  ಯದಿ(result)
    failf(data, "Failed extracting certificate chain");
  curlx_dyn_free(&out);
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#endif /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
