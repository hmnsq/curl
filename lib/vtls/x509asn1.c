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

#ਜੇ defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#ਜੇ defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_RUSTLS)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WANT_PARSEX509 /* uses Curl_parseX509() */
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_RUSTLS)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WANT_ਕੱਢੋ_CERTINFO /* uses Curl_extract_certinfo() */
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/base64.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/x509asn1.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"

/*
 * Constants.
 */

/* Largest supported ASN.1 structure. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ASN1_MAX                   ((size_t)0x40000)      /* 256K */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WANT_ਕੱਢੋ_CERTINFO

/* ASN.1 OID table entry. */
ਬਣਤਰ Curl_OID {
  ਅਚਲ ਅੱਖਰ *numoid;  /* Dotted-numeric OID. */
  ਅਚਲ ਅੱਖਰ *textoid; /* OID name. */
};

/* ASN.1 OIDs. */
ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_OID OIDtable[] = {
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
  { ਨਿਰਾ,                       ਨਿਰਾ }
};

#ਜੇ_ਅੰਤ /* WANT_EXTRACT_CERTINFO */

/*
 * Lightweight ASN.1 parser.
 * In particular, it does not check for syntactic/lexical errors.
 * It is intended to support certificate information gathering for SSL backends
 * that offer a mean to get certificates as a whole, but do not supply
 * entry points to get particular certificate sub-fields.
 * Please note there is no pretension here to rewrite a full SSL library.
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ASN1_MAX_RECURSIONS    16

ਸਥਿਰ ਅਚਲ uint8_t *getASN1Element_(ਬਣਤਰ Curl_asn1Element *elem,
                                      ਅਚਲ uint8_t *beg, ਅਚਲ uint8_t *ਸਿਰਾ,
                                      size_t lvl)
{
  uint8_t b;
  size_t len;
  ਬਣਤਰ Curl_asn1Element lelem;

  /* Get a single ASN.1 element into `elem', parse ASN.1 string at `beg'
     ending at `end'.
     Returns a pointer in source string after the parsed element, or NULL
     if an error occurs. */
  ਜੇ(!beg || !ਸਿਰਾ || beg >= ਸਿਰਾ || !*beg ||
     ((size_t)(ਸਿਰਾ - beg) > CURL_ASN1_MAX) ||
     lvl++ >= CURL_ASN1_MAX_RECURSIONS)
    ਵਾਪਸ ਨਿਰਾ;

  /* Process header byte. */
  elem->ਸਿਰਲੇਖ = beg;
  b = *beg++;
  elem->constructed = (b & 0x20) != 0;
  elem->eclass = (b >> 6) & 3;
  b &= 0x1F;
  ਜੇ(b == 0x1F)
    ਵਾਪਸ ਨਿਰਾ; /* Long tag values not supported here. */
  elem->ਟੈਗ = b;

  /* Process length. */
  ਜੇ(beg >= ਸਿਰਾ)
    ਵਾਪਸ ਨਿਰਾ;
  b = *beg++;
  ਜੇ(!(b & 0x80))
    len = b;
  ਨਹੀਂਤਾਂ {
    b &= 0x7F;
    ਜੇ(!b) {
      /* Unspecified length. Since we have all the data, we can determine the
         effective length by skipping element until an EOC element is found. */
      ਜੇ(!elem->constructed)
        ਵਾਪਸ ਨਿਰਾ;
      elem->beg = beg;
      ਜਦੋਂ(beg < ਸਿਰਾ && *beg) {
        beg = getASN1Element_(&lelem, beg, ਸਿਰਾ, lvl);
        ਜੇ(!beg)
          ਵਾਪਸ ਨਿਰਾ;
      }
      ਜੇ(ਸਿਰਾ - beg < 2 || beg[1])       /* EOC must have a zero length. */
        ਵਾਪਸ ਨਿਰਾ;
      elem->ਸਿਰਾ = beg;                  /* Do not include EOC as content. */
      ਵਾਪਸ beg + 2;                   /* After EOC element. */
    }
    ਨਹੀਂਤਾਂ ਜੇ(b > (size_t)(ਸਿਰਾ - beg))
      ਵਾਪਸ ਨਿਰਾ; /* Does not fit in source. */
    ਨਹੀਂਤਾਂ {
      /* Get long length. */
      len = 0;
      ਕਰੋ {
        ਜੇ(len & 0xFF000000L)
          ਵਾਪਸ ਨਿਰਾ;  /* Lengths > 32 bits are not supported. */
        len = (len << 8) | *beg++;
      } ਜਦੋਂ(--b);
    }
  }
  ਜੇ(len > (size_t)(ਸਿਰਾ - beg))
    ਵਾਪਸ ਨਿਰਾ;  /* Element data does not fit in source. */
  elem->beg = beg;
  elem->ਸਿਰਾ = beg + len;
  ਵਾਪਸ elem->ਸਿਰਾ;
}

/*
 * @unittest 1657
 */
UNITTEST ਅਚਲ uint8_t *getASN1Element(ਬਣਤਰ Curl_asn1Element *elem,
                                       ਅਚਲ uint8_t *beg, ਅਚਲ uint8_t *ਸਿਰਾ);
UNITTEST ਅਚਲ uint8_t *getASN1Element(ਬਣਤਰ Curl_asn1Element *elem,
                                       ਅਚਲ uint8_t *beg, ਅਚਲ uint8_t *ਸਿਰਾ)
{
  ਵਾਪਸ getASN1Element_(elem, beg, ਸਿਰਾ, 0);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WANT_ਕੱਢੋ_CERTINFO

/*
 * Search the null-terminated OID or OID identifier in local table.
 * Return the table entry pointer or NULL if not found.
 */
ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_OID *ਖੋਜ_OID(ਅਚਲ ਅੱਖਰ *oid)
{
  ਅਚਲ ਬਣਤਰ Curl_OID *op;
  ਲਈ(op = OIDtable; op->numoid; op++)
    ਜੇ(!strcmp(op->numoid, oid) || curl_strequal(op->textoid, oid))
      ਵਾਪਸ op;

  ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Convert an ASN.1 Boolean value into its string representation.
 *
 * Return error code.
 */

ਸਥਿਰ CURLcode bool2str(ਬਣਤਰ dynbuf *store,
                         ਅਚਲ uint8_t *beg, ਅਚਲ uint8_t *ਸਿਰਾ)
{
  uint8_t val;
  ਜੇ(ਸਿਰਾ - beg != 1)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  val = *beg;
  ਜੇ((val != 0xff) && (val != 0x00))
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਵਾਪਸ curlx_dyn_ਜੋੜੋ(store, val ? "TRUE" : "FALSE");
}

/*
 * Convert an ASN.1 octet string to a printable string.
 *
 * Return error code.
 */
ਸਥਿਰ CURLcode octet2str(ਬਣਤਰ dynbuf *store,
                          ਅਚਲ uint8_t *beg, ਅਚਲ uint8_t *ਸਿਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜਦੋਂ(!ਨਤੀਜਾ && beg < ਸਿਰਾ)
    ਨਤੀਜਾ = curlx_dyn_addf(store, "%02x:", *beg++);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode bit2str(ਬਣਤਰ dynbuf *store,
                        ਅਚਲ uint8_t *beg, ਅਚਲ uint8_t *ਸਿਰਾ)
{
  /* Convert an ASN.1 bit string to a printable string. */

  ਜੇ(beg < ਸਿਰਾ) {
    uint8_t ਅਣਵਰਤਿਆ_ਬਿੱਟ = *beg++;
    ਜੇ(ਅਣਵਰਤਿਆ_ਬਿੱਟ > 7)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਵਾਪਸ octet2str(store, beg, ਸਿਰਾ);
}

/*
 * Convert an ASN.1 integer value into its string representation.
 *
 * Returns error.
 */
ਸਥਿਰ CURLcode int2str(ਬਣਤਰ dynbuf *store,
                        ਅਚਲ uint8_t *beg, ਅਚਲ uint8_t *ਸਿਰਾ)
{
  uint32_t uval = 0;
  int32_t sval = 0;
  size_t n = ਸਿਰਾ - beg;
  size_t i;
  ਅਚਲ uint8_t *p = beg;

  ਜੇ(!n)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  ਜੇ(n > 4)
    ਵਾਪਸ octet2str(store, beg, ਸਿਰਾ);

  ਜੇ(p[0] & 0x80)
    uval = ~0U;

  ਲਈ(i = 0; i < n; i++)
    uval = (uval << 8) | p[i];

  sval = (int32_t)uval;

  /* small values are easier to read as decimals */
  ਜੇ(sval > -10000 && sval < 10000)
    ਵਾਪਸ curlx_dyn_addf(store, "%d", sval);

  /* represent larger values as hex */
  ਵਾਪਸ curlx_dyn_addf(store, "0x%x", uval);
}

/*
 * Convert from an ASN.1 typed string to UTF8.
 *
 * The result is stored in a dynbuf that is inited by the user of this
 * function.
 *
 * Returns error.
 */
ਸਥਿਰ CURLcode utf8asn1str(ਬਣਤਰ dynbuf *ਮੰਜ਼ਿਲ, ਪੂਰਨ type,
                            ਅਚਲ uint8_t *from, ਅਚਲ uint8_t *ਸਿਰਾ)
{
  size_t inlength = ਸਿਰਾ - from;
  ਪੂਰਨ ਮਾਪ = 1;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਚੋਣ(type) {
  ਹਾਲਤ CURL_ASN1_BMP_STRING:
    ਮਾਪ = 2;
    ਰੋਕੋ;
  ਹਾਲਤ CURL_ASN1_UNIVERSAL_STRING:
    ਮਾਪ = 4;
    ਰੋਕੋ;
  ਹਾਲਤ CURL_ASN1_NUMERIC_STRING:
  ਹਾਲਤ CURL_ASN1_PRINTABLE_STRING:
  ਹਾਲਤ CURL_ASN1_TELETEX_STRING:
  ਹਾਲਤ CURL_ASN1_IA5_STRING:
  ਹਾਲਤ CURL_ASN1_VISIBLE_STRING:
  ਹਾਲਤ CURL_ASN1_UTF8_STRING:
    ਰੋਕੋ;
  ਮੂਲ:
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;  /* Conversion not supported. */
  }

  ਜੇ(inlength % ਮਾਪ)
    /* Length inconsistent with character size. */
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  ਜੇ(type == CURL_ASN1_UTF8_STRING) {
    /* copy. */
    ਜੇ(inlength)
      ਨਤੀਜਾ = curlx_dyn_addn(ਮੰਜ਼ਿਲ, from, inlength);
  }
  ਨਹੀਂਤਾਂ {
    ਜਦੋਂ(!ਨਤੀਜਾ && (from < ਸਿਰਾ)) {
      ਅੱਖਰ buf[4]; /* decode buffer */
      size_t charsize = 1;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ wc = 0;

      ਚੋਣ(ਮਾਪ) {
      ਹਾਲਤ 4:
        wc = (wc << 8) | *from++;
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      ਹਾਲਤ 2:
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      ਮੂਲ: /* case 1: */
        wc = (wc << 8) | *from++;
      }
      ਜੇ(wc >= 0x00000080) {
        ਜੇ(wc >= 0x00000800) {
          ਜੇ(wc >= 0x00010000) {
            ਜੇ(wc >= 0x00200000) {
              /* Invalid char. size for target encoding. */
              ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
            }
            buf[3] = (ਅੱਖਰ)(0x80 | (wc & 0x3F));
            wc = (wc >> 6) | 0x00010000;
            charsize++;
          }
          buf[2] = (ਅੱਖਰ)(0x80 | (wc & 0x3F));
          wc = (wc >> 6) | 0x00000800;
          charsize++;
        }
        buf[1] = (ਅੱਖਰ)(0x80 | (wc & 0x3F));
        wc = (wc >> 6) | 0x000000C0;
        charsize++;
      }
      buf[0] = (ਅੱਖਰ)wc;
      ਨਤੀਜਾ = curlx_dyn_addn(ਮੰਜ਼ਿਲ, buf, charsize);
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
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
UNITTEST CURLcode encodeOID(ਬਣਤਰ dynbuf *store,
                            ਅਚਲ uint8_t *beg, ਅਚਲ uint8_t *ਸਿਰਾ);
UNITTEST CURLcode encodeOID(ਬਣਤਰ dynbuf *store,
                            ਅਚਲ uint8_t *beg, ਅਚਲ uint8_t *ਸਿਰਾ)
{
  uint32_t x;
  uint32_t y;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(ਸਿਰਾ <= beg)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  /* Process the first two numbers. The initial digit cannot be larger than
     2 */
  y = *beg++;
  ਜੇ(y <= 80)
    x = y / 40;
  ਨਹੀਂਤਾਂ
    x = 2;
  ਨਤੀਜਾ = curlx_dyn_addf(store, "%u", x);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਜੇ(y & 0x80) {
    uint32_t t = (y & 0x7f); /* the second number */
    ਜੇ(!t)
      /* reject leading 0x80 padding */
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਕਰੋ {
      ਜੇ((t & 0xFE000000) || (beg >= ਸਿਰਾ))
        ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
      y = *beg++;
      t = (t << 7) | (y & 0x7F);
    } ਜਦੋਂ(y & 0x80);
    y = t;
  }

  ਨਤੀਜਾ = curlx_dyn_addf(store, ".%u", y - (x * 40));
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* Process the trailing numbers. */
  ਜਦੋਂ((beg < ਸਿਰਾ) && !ਨਤੀਜਾ) {
    x = 0;
    y = 0;
    ਕਰੋ {
      /* 0x80 as the first value is invalid since it only adds padding */
      ਜੇ((y & 0x80) && !x)
        ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
      ਨਹੀਂਤਾਂ ਜੇ((x & 0xFE000000) || (beg == ਸਿਰਾ))
        ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
      y = *beg++;
      x = (x << 7) | (y & 0x7F);
    } ਜਦੋਂ(y & 0x80);
    ਨਤੀਜਾ = curlx_dyn_addf(store, ".%u", x);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Convert an ASN.1 OID into its dotted or symbolic string representation.
 *
 * Return error code.
 */

ਸਥਿਰ CURLcode OID2str(ਬਣਤਰ dynbuf *store,
                        ਅਚਲ uint8_t *beg, ਅਚਲ uint8_t *ਸਿਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਜੇ(beg < ਸਿਰਾ) {
    ਬਣਤਰ dynbuf buf;
    curlx_dyn_init(&buf, CURL_X509_STR_MAX);
    ਨਤੀਜਾ = encodeOID(&buf, beg, ਸਿਰਾ);

    ਜੇ(!ਨਤੀਜਾ) {
      ਅਚਲ ਬਣਤਰ Curl_OID *op = ਖੋਜ_OID(curlx_dyn_ptr(&buf));
      ਜੇ(op)
        ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(store, op->textoid);
      ਨਹੀਂਤਾਂ
        ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(store, curlx_dyn_ptr(&buf));
    }
    curlx_dyn_free(&buf);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * @unittest 1656
 */
UNITTEST CURLcode GTime2str(ਬਣਤਰ dynbuf *store,
                            ਅਚਲ ਅੱਖਰ *beg, ਅਚਲ ਅੱਖਰ *ਸਿਰਾ);
UNITTEST CURLcode GTime2str(ਬਣਤਰ dynbuf *store,
                            ਅਚਲ ਅੱਖਰ *beg, ਅਚਲ ਅੱਖਰ *ਸਿਰਾ)
{
  ਅਚਲ ਅੱਖਰ *tzp;
  ਅਚਲ ਅੱਖਰ *fracp;
  ਅੱਖਰ sec1, sec2;
  size_t fracl;
  size_t tzl;
  ਅਚਲ ਅੱਖਰ *sep = "";

  /* Convert an ASN.1 Generalized time to a printable string.
     Return the dynamically allocated string, or NULL if an error occurs. */

  ਲਈ(fracp = beg; fracp < ਸਿਰਾ && ISDIGIT(*fracp); fracp++)
    ;

  /* Get seconds digits. */
  sec1 = '0';
  ਚੋਣ(fracp - beg - 12) {
  ਹਾਲਤ 0:
    sec2 = '0';
    ਰੋਕੋ;
  ਹਾਲਤ 2:
    sec1 = fracp[-2];
    FALLTHROUGH();
  ਹਾਲਤ 1:
    sec2 = fracp[-1];
    ਰੋਕੋ;
  ਮੂਲ:
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }

  /* timezone follows optional fractional seconds. */
  tzp = fracp;
  fracl = 0; /* no fractional seconds detected so far */
  ਜੇ(fracp < ਸਿਰਾ && (*fracp == '.' || *fracp == ',')) {
    /* Have fractional seconds, e.g. "[.,]\d+". How many? */
    fracp++; /* should be a digit char or BAD ARGUMENT */
    tzp = fracp;
    ਜਦੋਂ(tzp < ਸਿਰਾ && ISDIGIT(*tzp))
      tzp++;
    ਜੇ(tzp == fracp) /* never looped, no digit after [.,] */
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    fracl = tzp - fracp; /* number of fractional sec digits */
    DEBUGASSERT(fracl > 0);
    /* Strip trailing zeroes in fractional seconds.
     * May reduce fracl to 0 if only '0's are present. */
    ਜਦੋਂ(fracl && fracp[fracl - 1] == '0')
      fracl--;
  }

  /* Process timezone. */
  ਜੇ(tzp >= ਸਿਰਾ) {
    tzp = "";
    tzl = 0;
  }
  ਨਹੀਂਤਾਂ ਜੇ(*tzp == 'Z') {
    sep = " ";
    tzp = "GMT";
    tzl = 3;
  }
  ਨਹੀਂਤਾਂ ਜੇ((*tzp == '+') || (*tzp == '-')) {
    sep = " UTC";
    tzl = ਸਿਰਾ - tzp;
  }
  ਨਹੀਂਤਾਂ {
    sep = " ";
    tzl = ਸਿਰਾ - tzp;
  }

  ਵਾਪਸ curlx_dyn_addf(store,
                        "%.4s-%.2s-%.2s %.2s:%.2s:%c%c%s%.*s%s%.*s",
                        beg, beg + 4, beg + 6,
                        beg + 8, beg + 10, sec1, sec2,
                        fracl ? "." : "", (ਪੂਰਨ)fracl, fracp,
                        sep, (ਪੂਰਨ)tzl, tzp);
}

/*
 * Convert an ASN.1 UTC time to a printable string.
 *
 * Return error code.
 */
ਸਥਿਰ CURLcode UTime2str(ਬਣਤਰ dynbuf *store,
                          ਅਚਲ ਅੱਖਰ *beg, ਅਚਲ ਅੱਖਰ *ਸਿਰਾ)
{
  ਅਚਲ ਅੱਖਰ *tzp;
  size_t tzl;
  ਅਚਲ ਅੱਖਰ *sec;

  ਲਈ(tzp = beg; tzp < ਸਿਰਾ && *tzp >= '0' && *tzp <= '9'; tzp++)
    ;
  /* Get the seconds. */
  sec = beg + 10;
  ਚੋਣ(tzp - sec) {
  ਹਾਲਤ 0:
    sec = "00";
    FALLTHROUGH();
  ਹਾਲਤ 2:
    ਰੋਕੋ;
  ਮੂਲ:
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }

  /* Process timezone. */
  ਜੇ(tzp >= ਸਿਰਾ)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਜੇ(*tzp == 'Z') {
    tzp = "GMT";
    ਸਿਰਾ = tzp + 3;
  }

  tzl = ਸਿਰਾ - tzp;
  ਵਾਪਸ curlx_dyn_addf(store, "%d%.2s-%.2s-%.2s %.2s:%.2s:%.2s %.*s",
                        20 - (*beg >= '5'), beg, beg + 2, beg + 4,
                        beg + 6, beg + 8, sec,
                        (ਪੂਰਨ)tzl, tzp);
}

/*
 * Convert an ASN.1 element to a printable string.
 *
 * Return error
 *
 * @unittest 1667
 */
UNITTEST CURLcode ASN1tostr(ਬਣਤਰ dynbuf *store,
                            ਬਣਤਰ Curl_asn1Element *elem);
UNITTEST CURLcode ASN1tostr(ਬਣਤਰ dynbuf *store,
                            ਬਣਤਰ Curl_asn1Element *elem)
{
  CURLcode ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਪੂਰਨ type = elem->ਟੈਗ;
  ਜੇ(elem->constructed)
    ਵਾਪਸ ਨਤੀਜਾ; /* No conversion of structured elements. */

  ਚੋਣ(type) {
  ਹਾਲਤ CURL_ASN1_BOOLEAN:
    ਨਤੀਜਾ = bool2str(store, elem->beg, elem->ਸਿਰਾ);
    ਰੋਕੋ;
  ਹਾਲਤ CURL_ASN1_INTEGER:
  ਹਾਲਤ CURL_ASN1_ENUMERATED:
    ਨਤੀਜਾ = int2str(store, elem->beg, elem->ਸਿਰਾ);
    ਰੋਕੋ;
  ਹਾਲਤ CURL_ASN1_ਦੋਆਧਾਰੀ_ਅੰਕ_STRING:
    ਨਤੀਜਾ = bit2str(store, elem->beg, elem->ਸਿਰਾ);
    ਰੋਕੋ;
  ਹਾਲਤ CURL_ASN1_OCTET_STRING:
    ਨਤੀਜਾ = octet2str(store, elem->beg, elem->ਸਿਰਾ);
    ਰੋਕੋ;
  ਹਾਲਤ CURL_ASN1_NULL:
    ਨਤੀਜਾ = curlx_dyn_addn(store, "", 1);
    ਰੋਕੋ;
  ਹਾਲਤ CURL_ASN1_OBJECT_IDENTIFIER:
    ਨਤੀਜਾ = OID2str(store, elem->beg, elem->ਸਿਰਾ);
    ਰੋਕੋ;
  ਹਾਲਤ CURL_ASN1_UTC_ਸਮਾਂ:
    ਨਤੀਜਾ = UTime2str(store, (ਅਚਲ ਅੱਖਰ *)elem->beg,
                       (ਅਚਲ ਅੱਖਰ *)elem->ਸਿਰਾ);
    ਰੋਕੋ;
  ਹਾਲਤ CURL_ASN1_GENERALIZED_ਸਮਾਂ:
    ਨਤੀਜਾ = GTime2str(store, (ਅਚਲ ਅੱਖਰ *)elem->beg,
                       (ਅਚਲ ਅੱਖਰ *)elem->ਸਿਰਾ);
    ਰੋਕੋ;
  ਹਾਲਤ CURL_ASN1_UTF8_STRING:
  ਹਾਲਤ CURL_ASN1_NUMERIC_STRING:
  ਹਾਲਤ CURL_ASN1_PRINTABLE_STRING:
  ਹਾਲਤ CURL_ASN1_TELETEX_STRING:
  ਹਾਲਤ CURL_ASN1_IA5_STRING:
  ਹਾਲਤ CURL_ASN1_VISIBLE_STRING:
  ਹਾਲਤ CURL_ASN1_UNIVERSAL_STRING:
  ਹਾਲਤ CURL_ASN1_BMP_STRING:
    ਨਤੀਜਾ = utf8asn1str(store, type, elem->beg, elem->ਸਿਰਾ);
    ਰੋਕੋ;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * ASCII encode distinguished name at `dn' into the store dynbuf.
 *
 * Returns error.
 */
ਸਥਿਰ CURLcode encodeDN(ਬਣਤਰ dynbuf *store, ਬਣਤਰ Curl_asn1Element *dn)
{
  ਬਣਤਰ Curl_asn1Element rdn;
  ਬਣਤਰ Curl_asn1Element atv;
  ਬਣਤਰ Curl_asn1Element oid;
  ਬਣਤਰ Curl_asn1Element ਮੁੱਲ;
  ਅਚਲ uint8_t *p1;
  ਅਚਲ uint8_t *p2;
  ਅਚਲ uint8_t *p3;
  ਅਚਲ uint8_t *str;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  bool added = FALSE;
  ਬਣਤਰ dynbuf ਆਰਜ਼ੀ;
  curlx_dyn_init(&ਆਰਜ਼ੀ, CURL_X509_STR_MAX);

  ਲਈ(p1 = dn->beg; p1 < dn->ਸਿਰਾ;) {
    p1 = getASN1Element(&rdn, p1, dn->ਸਿਰਾ);
    ਜੇ(!p1) {
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
      ਜਾਓ error;
    }
    ਲਈ(p2 = rdn.beg; p2 < rdn.ਸਿਰਾ;) {
      p2 = getASN1Element(&atv, p2, rdn.ਸਿਰਾ);
      ਜੇ(!p2) {
        ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
        ਜਾਓ error;
      }
      p3 = getASN1Element(&oid, atv.beg, atv.ਸਿਰਾ);
      ਜੇ(!p3) {
        ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
        ਜਾਓ error;
      }
      ਜੇ(!getASN1Element(&ਮੁੱਲ, p3, atv.ਸਿਰਾ)) {
        ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
        ਜਾਓ error;
      }
      curlx_dyn_reset(&ਆਰਜ਼ੀ);
      ਨਤੀਜਾ = ASN1tostr(&ਆਰਜ਼ੀ, &oid);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ error;

      str = curlx_dyn_uptr(&ਆਰਜ਼ੀ);

      ਜੇ(!str) {
        ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
        ਜਾਓ error;
      }

      /* Encode delimiter.
         If attribute has a short uppercase name, delimiter is ", ". */
      ਲਈ(p3 = str; ISUPPER(*p3); p3++)
        ;
      ਜੇ(added) {
        ਜੇ(p3 - str > 2)
          ਨਤੀਜਾ = curlx_dyn_addn(store, "/", 1);
        ਨਹੀਂਤਾਂ
          ਨਤੀਜਾ = curlx_dyn_addn(store, ", ", 2);
        ਜੇ(ਨਤੀਜਾ)
          ਜਾਓ error;
      }

      /* Encode attribute name. */
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(store, (ਅਚਲ ਅੱਖਰ *)str);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ error;

      /* Generate equal sign. */
      ਨਤੀਜਾ = curlx_dyn_addn(store, "=", 1);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ error;

      /* Generate value. */
      ਨਤੀਜਾ = ASN1tostr(store, &ਮੁੱਲ);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ error;
      curlx_dyn_reset(&ਆਰਜ਼ੀ);
      added = TRUE; /* use separator for next */
    }
  }
error:
  curlx_dyn_free(&ਆਰਜ਼ੀ);

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* WANT_EXTRACT_CERTINFO */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WANT_PARSEX509
/*
 * ASN.1 parse an X509 certificate into structure subfields.
 * Syntax is assumed to have already been checked by the SSL backend.
 * See RFC 5280.
 */
ਪੂਰਨ Curl_parseX509(ਬਣਤਰ Curl_X509certificate *cert,
                   ਅਚਲ uint8_t *beg, ਅਚਲ uint8_t *ਸਿਰਾ)
{
  ਬਣਤਰ Curl_asn1Element elem;
  ਬਣਤਰ Curl_asn1Element tbsCertificate;
  ਅਚਲ uint8_t *ccp;
  ਸਥਿਰ ਅਚਲ uint8_t default_ਸੰਸਕਰਣ = 0;  /* v1. */

  memset(cert, 0, ਆਕਾਰ(*cert));
  cert->certificate.ਸਿਰਲੇਖ = ਨਿਰਾ;
  cert->certificate.beg = beg;
  cert->certificate.ਸਿਰਾ = ਸਿਰਾ;

  /* Get the sequence content. */
  ਜੇ(!getASN1Element(&elem, beg, ਸਿਰਾ))
    ਵਾਪਸ -1;  /* Invalid bounds/size. */
  beg = elem.beg;
  ਸਿਰਾ = elem.ਸਿਰਾ;

  /* Get tbsCertificate. */
  beg = getASN1Element(&tbsCertificate, beg, ਸਿਰਾ);
  ਜੇ(!beg)
    ਵਾਪਸ -1;
  /* Skip the signatureAlgorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, ਸਿਰਾ);
  ਜੇ(!beg)
    ਵਾਪਸ -1;
  /* Get the signatureValue. */
  ਜੇ(!getASN1Element(&cert->signature, beg, ਸਿਰਾ))
    ਵਾਪਸ -1;

  /* Parse TBSCertificate. */
  beg = tbsCertificate.beg;
  ਸਿਰਾ = tbsCertificate.ਸਿਰਾ;
  /* Get optional version, get serialNumber. */
  cert->ਸੰਸਕਰਣ_2.ਸਿਰਲੇਖ = ਨਿਰਾ;
  cert->ਸੰਸਕਰਣ_2.beg = &default_ਸੰਸਕਰਣ;
  cert->ਸੰਸਕਰਣ_2.ਸਿਰਾ = &default_ਸੰਸਕਰਣ + ਆਕਾਰ(default_ਸੰਸਕਰਣ);
  beg = getASN1Element(&elem, beg, ਸਿਰਾ);
  ਜੇ(!beg)
    ਵਾਪਸ -1;
  ਜੇ(elem.ਟੈਗ == 0) {
    ਜੇ(!getASN1Element(&cert->ਸੰਸਕਰਣ_2, elem.beg, elem.ਸਿਰਾ))
      ਵਾਪਸ -1;
    beg = getASN1Element(&elem, beg, ਸਿਰਾ);
    ਜੇ(!beg)
      ਵਾਪਸ -1;
  }
  cert->serial_ਸੰਖਿਆ = elem;
  /* Get signature algorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, ਸਿਰਾ);
  /* Get issuer. */
  beg = getASN1Element(&cert->issuer, beg, ਸਿਰਾ);
  ਜੇ(!beg)
    ਵਾਪਸ -1;
  /* Get notBefore and notAfter. */
  beg = getASN1Element(&elem, beg, ਸਿਰਾ);
  ਜੇ(!beg)
    ਵਾਪਸ -1;
  ccp = getASN1Element(&cert->notBefore, elem.beg, elem.ਸਿਰਾ);
  ਜੇ(!ccp)
    ਵਾਪਸ -1;
  ਜੇ(!getASN1Element(&cert->notAfter, ccp, elem.ਸਿਰਾ))
    ਵਾਪਸ -1;
  /* These two fields MUST consume all of the validity elem */
  ਜੇ(elem.ਸਿਰਾ != cert->notAfter.ਸਿਰਾ)
    ਵਾਪਸ -1;
  /* Get subject. */
  beg = getASN1Element(&cert->subject, beg, ਸਿਰਾ);
  ਜੇ(!beg)
    ਵਾਪਸ -1;
  /* Get subjectPublicKeyAlgorithm and subjectPublicKey. */
  beg = getASN1Element(&cert->subject_ਜਨਤਕ_ਕੁੰਜੀ_Info, beg, ਸਿਰਾ);
  ਜੇ(!beg)
    ਵਾਪਸ -1;
  ccp = getASN1Element(&cert->subject_ਜਨਤਕ_ਕੁੰਜੀ_Algorithm,
                       cert->subject_ਜਨਤਕ_ਕੁੰਜੀ_Info.beg,
                       cert->subject_ਜਨਤਕ_ਕੁੰਜੀ_Info.ਸਿਰਾ);
  ਜੇ(!ccp)
    ਵਾਪਸ -1;
  ਜੇ(!getASN1Element(&cert->subject_ਜਨਤਕ_ਕੁੰਜੀ, ccp,
                     cert->subject_ਜਨਤਕ_ਕੁੰਜੀ_Info.ਸਿਰਾ))
    ਵਾਪਸ -1;
  /* Get optional issuerUniqueID, subjectUniqueID and extensions. */
  cert->issuerUniqueID.ਟੈਗ = cert->subjectUniqueID.ਟੈਗ = 0;
  cert->extensions.ਟੈਗ = elem.ਟੈਗ = 0;
  cert->issuerUniqueID.ਸਿਰਲੇਖ = cert->subjectUniqueID.ਸਿਰਲੇਖ = ਨਿਰਾ;
  cert->issuerUniqueID.beg = cert->issuerUniqueID.ਸਿਰਾ = beg;
  cert->subjectUniqueID.beg = cert->subjectUniqueID.ਸਿਰਾ = beg;
  cert->extensions.ਸਿਰਲੇਖ = ਨਿਰਾ;
  cert->extensions.beg = cert->extensions.ਸਿਰਾ = beg;
  ਜੇ(beg < ਸਿਰਾ) {
    beg = getASN1Element(&elem, beg, ਸਿਰਾ);
    ਜੇ(!beg)
      ਵਾਪਸ -1;
  }
  ਜੇ(elem.ਟੈਗ == 1) {
    cert->issuerUniqueID = elem;
    ਜੇ(beg < ਸਿਰਾ) {
      beg = getASN1Element(&elem, beg, ਸਿਰਾ);
      ਜੇ(!beg)
        ਵਾਪਸ -1;
    }
  }
  ਜੇ(elem.ਟੈਗ == 2) {
    cert->subjectUniqueID = elem;
    ਜੇ(beg < ਸਿਰਾ) {
      beg = getASN1Element(&elem, beg, ਸਿਰਾ);
      ਜੇ(!beg)
        ਵਾਪਸ -1;
    }
  }
  ਜੇ(elem.ਟੈਗ == 3)
    ਜੇ(!getASN1Element(&cert->extensions, elem.beg, elem.ਸਿਰਾ))
      ਵਾਪਸ -1;
  ਵਾਪਸ 0;
}

#ਜੇ_ਅੰਤ /* WANT_PARSEX509 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WANT_ਕੱਢੋ_CERTINFO

ਸਥਿਰ CURLcode dumpAlgo(ਬਣਤਰ dynbuf *store,
                         ਬਣਤਰ Curl_asn1Element *param,
                         ਅਚਲ uint8_t *beg, ਅਚਲ uint8_t *ਸਿਰਾ)
{
  ਬਣਤਰ Curl_asn1Element oid;

  /* Get algorithm parameters and return algorithm name. */

  beg = getASN1Element(&oid, beg, ਸਿਰਾ);
  ਜੇ(!beg)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  param->ਸਿਰਲੇਖ = ਨਿਰਾ;
  param->ਟੈਗ = 0;
  param->beg = param->ਸਿਰਾ = ਸਿਰਾ;
  ਜੇ(beg < ਸਿਰਾ) {
    ਅਚਲ uint8_t *p = getASN1Element(param, beg, ਸਿਰਾ);
    ਜੇ(!p)
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }
  ਵਾਪਸ OID2str(store, oid.beg, oid.ਸਿਰਾ);
}

/*
 * This is a convenience function for push_certinfo_len that takes a zero
 * terminated value.
 */
ਸਥਿਰ CURLcode ssl_ਧੱਕੋ_certinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਪੂਰਨ certnum,
                                  ਅਚਲ ਅੱਖਰ *label,
                                  ਅਚਲ ਅੱਖਰ *ਮੁੱਲ)
{
  size_t valuelen = ਲੜੀ_ਲੰਬਾਈ(ਮੁੱਲ);

  ਵਾਪਸ Curl_ssl_ਧੱਕੋ_certinfo_len(ਅੰਕੜੇ, certnum, label, ਮੁੱਲ, valuelen);
}

/*
 * This is a convenience function for push_certinfo_len that takes a
 * dynbuf value.
 *
 * It also does the verbose output if !certnum.
 */
ਸਥਿਰ CURLcode ssl_ਧੱਕੋ_certinfo_dyn(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਪੂਰਨ certnum,
                                      ਅਚਲ ਅੱਖਰ *label,
                                      ਬਣਤਰ dynbuf *ptr)
{
  size_t valuelen = curlx_dyn_len(ptr);
  ਅਚਲ ਅੱਖਰ *ਮੁੱਲ = curlx_dyn_ptr(ptr);

  CURLcode ਨਤੀਜਾ = Curl_ssl_ਧੱਕੋ_certinfo_len(ਅੰਕੜੇ, certnum, label,
                                               ਮੁੱਲ, valuelen);

  ਜੇ(!certnum && !ਨਤੀਜਾ)
    infof(ਅੰਕੜੇ, "   %s: %s", label, ਮੁੱਲ);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode do_pubkey_ਖੇਤਰ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ certnum,
                                ਅਚਲ ਅੱਖਰ *label,
                                ਬਣਤਰ Curl_asn1Element *elem)
{
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ dynbuf ਬਾਹਰਲਾ;

  curlx_dyn_init(&ਬਾਹਰਲਾ, CURL_X509_STR_MAX);

  /* Generate a certificate information record for the public key. */

  ਨਤੀਜਾ = ASN1tostr(&ਬਾਹਰਲਾ, elem);
  ਜੇ(!ਨਤੀਜਾ && ਅੰਕੜੇ->ਸੈੱਟ.ssl.certinfo)
    ਨਤੀਜਾ = ssl_ਧੱਕੋ_certinfo_dyn(ਅੰਕੜੇ, certnum, label, &ਬਾਹਰਲਾ);
  curlx_dyn_free(&ਬਾਹਰਲਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* return 0 on success, 1 on error */
ਸਥਿਰ ਪੂਰਨ do_pubkey(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ certnum, ਅਚਲ ਅੱਖਰ *algo,
                     ਬਣਤਰ Curl_asn1Element *param,
                     ਬਣਤਰ Curl_asn1Element *pubkey)
{
  ਬਣਤਰ Curl_asn1Element elem;
  ਬਣਤਰ Curl_asn1Element pk;
  ਅਚਲ uint8_t *p;

  /* Generate all information records for the public key. */

  ਜੇ(curl_strequal(algo, "ecPublicKey")) {
    /*
     * ECC public key is all the data, a value of type BIT STRING mapped to
     * OCTET STRING and should not be parsed as an ASN.1 value.
     */
    ਅਚਲ size_t dlen = pubkey->ਸਿਰਾ - pubkey->beg;
    size_t len;
    ਜੇ(dlen < 2)
      /* too small */
      ਵਾਪਸ 1;
    len = (dlen - 2) * 4;
    ਜੇ(!certnum)
      infof(ਅੰਕੜੇ, "   ECC Public Key (%zu bits)", len);
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl.certinfo) {
      ਅੱਖਰ q[(ਆਕਾਰ(len) * 8 / 3) + 1];
      (ਖਾਲੀ)curl_msnprintf(q, ਆਕਾਰ(q), "%zu", len);
      ਜੇ(ssl_ਧੱਕੋ_certinfo(ਅੰਕੜੇ, certnum, "ECC Public Key", q))
        ਵਾਪਸ 1;
    }
    ਵਾਪਸ do_pubkey_ਖੇਤਰ(ਅੰਕੜੇ, certnum, "ecPublicKey", pubkey) == CURLE_OK
      ? 0 : 1;
  }

  /* Get the public key (single element). */
  ਜੇ(!getASN1Element(&pk, pubkey->beg + 1, pubkey->ਸਿਰਾ))
    ਵਾਪਸ 1;

  ਜੇ(curl_strequal(algo, "rsaEncryption")) {
    ਅਚਲ uint8_t *q;
    size_t len;

    p = getASN1Element(&elem, pk.beg, pk.ਸਿਰਾ);
    ਜੇ(!p)
      ਵਾਪਸ 1;

    /* Compute key length. */
    ਲਈ(q = elem.beg; q < elem.ਸਿਰਾ && !*q; q++)
      ;
    len = ((elem.ਸਿਰਾ - q) * 8);
    ਜੇ(len) {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
      ਲਈ(i = *q; !(i & 0x80); i <<= 1)
        len--;
    }
    ਜੇ(len > 32)
      elem.beg = q;     /* Strip leading zero bytes. */
    ਜੇ(!certnum)
      infof(ਅੰਕੜੇ, "   RSA Public Key (%zu bits)", len);
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl.certinfo) {
      ਅੱਖਰ r[(ਆਕਾਰ(len) * 8 / 3) + 1];
      curl_msnprintf(r, ਆਕਾਰ(r), "%zu", len);
      ਜੇ(ssl_ਧੱਕੋ_certinfo(ਅੰਕੜੇ, certnum, "RSA Public Key", r))
        ਵਾਪਸ 1;
    }
    /* Generate coefficients. */
    ਜੇ(do_pubkey_ਖੇਤਰ(ਅੰਕੜੇ, certnum, "rsa(n)", &elem))
      ਵਾਪਸ 1;
    ਜੇ(!getASN1Element(&elem, p, pk.ਸਿਰਾ))
      ਵਾਪਸ 1;
    ਜੇ(do_pubkey_ਖੇਤਰ(ਅੰਕੜੇ, certnum, "rsa(e)", &elem))
      ਵਾਪਸ 1;
  }
  ਨਹੀਂਤਾਂ ਜੇ(curl_strequal(algo, "dsa")) {
    p = getASN1Element(&elem, param->beg, param->ਸਿਰਾ);
    ਜੇ(p) {
      ਜੇ(do_pubkey_ਖੇਤਰ(ਅੰਕੜੇ, certnum, "dsa(p)", &elem))
        ਵਾਪਸ 1;
      p = getASN1Element(&elem, p, param->ਸਿਰਾ);
      ਜੇ(p) {
        ਜੇ(do_pubkey_ਖੇਤਰ(ਅੰਕੜੇ, certnum, "dsa(q)", &elem))
          ਵਾਪਸ 1;
        ਜੇ(getASN1Element(&elem, p, param->ਸਿਰਾ)) {
          ਜੇ(do_pubkey_ਖੇਤਰ(ਅੰਕੜੇ, certnum, "dsa(g)", &elem))
            ਵਾਪਸ 1;
          ਜੇ(do_pubkey_ਖੇਤਰ(ਅੰਕੜੇ, certnum, "dsa(pub_key)", &pk))
            ਵਾਪਸ 1;
        }
      }
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(curl_strequal(algo, "dhpublicnumber")) {
    p = getASN1Element(&elem, param->beg, param->ਸਿਰਾ);
    ਜੇ(p) {
      ਜੇ(do_pubkey_ਖੇਤਰ(ਅੰਕੜੇ, certnum, "dh(p)", &elem))
        ਵਾਪਸ 1;
      ਜੇ(getASN1Element(&elem, p, param->ਸਿਰਾ)) {
        ਜੇ(do_pubkey_ਖੇਤਰ(ਅੰਕੜੇ, certnum, "dh(g)", &elem))
          ਵਾਪਸ 1;
        ਜੇ(do_pubkey_ਖੇਤਰ(ਅੰਕੜੇ, certnum, "dh(pub_key)", &pk))
          ਵਾਪਸ 1;
      }
    }
  }
  ਵਾਪਸ 0;
}

/*
 * Convert an ASN.1 distinguished name into a printable string.
 * Return error.
 */
ਸਥਿਰ CURLcode DNtostr(ਬਣਤਰ dynbuf *store, ਬਣਤਰ Curl_asn1Element *dn)
{
  ਵਾਪਸ encodeDN(store, dn);
}

CURLcode Curl_ਕੱਢੋ_certinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਪੂਰਨ certnum,
                               ਅਚਲ uint8_t *beg,
                               ਅਚਲ uint8_t *ਸਿਰਾ)
{
  ਬਣਤਰ Curl_X509certificate cert;
  ਬਣਤਰ Curl_asn1Element param;
  ਅੱਖਰ *certptr;
  size_t clen;
  ਬਣਤਰ dynbuf ਬਾਹਰਲਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸੰਸਕਰਣ_2;
  ਅਚਲ uint8_t *ptr;
  ਪੂਰਨ rc;

  ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.ssl.certinfo)
    ਜੇ(certnum)
      ਵਾਪਸ CURLE_OK;

  curlx_dyn_init(&ਬਾਹਰਲਾ, CURL_X509_STR_MAX);
  /* Prepare the certificate information for curl_easy_getinfo(). */

  /* Extract the certificate ASN.1 elements. */
  ਜੇ(Curl_parseX509(&cert, beg, ਸਿਰਾ))
    ਵਾਪਸ CURLE_PEER_FAILED_VERIFICATION;

  /* Subject. */
  ਨਤੀਜਾ = DNtostr(&ਬਾਹਰਲਾ, &cert.subject);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਪੂਰਾ;
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl.certinfo) {
    ਨਤੀਜਾ = ssl_ਧੱਕੋ_certinfo_dyn(ਅੰਕੜੇ, certnum, "Subject", &ਬਾਹਰਲਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਪੂਰਾ;
  }
  curlx_dyn_reset(&ਬਾਹਰਲਾ);

  /* Issuer. */
  ਨਤੀਜਾ = DNtostr(&ਬਾਹਰਲਾ, &cert.issuer);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਪੂਰਾ;
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl.certinfo) {
    ਨਤੀਜਾ = ssl_ਧੱਕੋ_certinfo_dyn(ਅੰਕੜੇ, certnum, "Issuer", &ਬਾਹਰਲਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਪੂਰਾ;
  }
  curlx_dyn_reset(&ਬਾਹਰਲਾ);

  /* Version (always fits in less than 32 bits). */
  ਸੰਸਕਰਣ_2 = 0;
  ਲਈ(ptr = cert.ਸੰਸਕਰਣ_2.beg; ptr < cert.ਸੰਸਕਰਣ_2.ਸਿਰਾ; ptr++)
    ਸੰਸਕਰਣ_2 = (ਸੰਸਕਰਣ_2 << 8) | *ptr;
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl.certinfo) {
    ਨਤੀਜਾ = curlx_dyn_addf(&ਬਾਹਰਲਾ, "%x", ਸੰਸਕਰਣ_2);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਪੂਰਾ;
    ਨਤੀਜਾ = ssl_ਧੱਕੋ_certinfo_dyn(ਅੰਕੜੇ, certnum, "Version", &ਬਾਹਰਲਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਪੂਰਾ;
    curlx_dyn_reset(&ਬਾਹਰਲਾ);
  }

  /* Serial number. */
  ਨਤੀਜਾ = ASN1tostr(&ਬਾਹਰਲਾ, &cert.serial_ਸੰਖਿਆ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਪੂਰਾ;
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl.certinfo) {
    ਨਤੀਜਾ = ssl_ਧੱਕੋ_certinfo_dyn(ਅੰਕੜੇ, certnum, "Serial Number", &ਬਾਹਰਲਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਪੂਰਾ;
  }
  curlx_dyn_reset(&ਬਾਹਰਲਾ);

  /* Signature algorithm .*/
  ਨਤੀਜਾ = dumpAlgo(&ਬਾਹਰਲਾ, &param, cert.signatureAlgorithm.beg,
                    cert.signatureAlgorithm.ਸਿਰਾ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਪੂਰਾ;
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl.certinfo) {
    ਨਤੀਜਾ = ssl_ਧੱਕੋ_certinfo_dyn(ਅੰਕੜੇ, certnum, "Signature Algorithm", &ਬਾਹਰਲਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਪੂਰਾ;
  }
  curlx_dyn_reset(&ਬਾਹਰਲਾ);

  /* Start Date. */
  ਨਤੀਜਾ = ASN1tostr(&ਬਾਹਰਲਾ, &cert.notBefore);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਪੂਰਾ;
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl.certinfo) {
    ਨਤੀਜਾ = ssl_ਧੱਕੋ_certinfo_dyn(ਅੰਕੜੇ, certnum, "Start Date", &ਬਾਹਰਲਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਪੂਰਾ;
  }
  curlx_dyn_reset(&ਬਾਹਰਲਾ);

  /* Expire Date. */
  ਨਤੀਜਾ = ASN1tostr(&ਬਾਹਰਲਾ, &cert.notAfter);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਪੂਰਾ;
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl.certinfo) {
    ਨਤੀਜਾ = ssl_ਧੱਕੋ_certinfo_dyn(ਅੰਕੜੇ, certnum, "Expire Date", &ਬਾਹਰਲਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਪੂਰਾ;
  }
  curlx_dyn_reset(&ਬਾਹਰਲਾ);

  /* Public Key Algorithm. */
  ਨਤੀਜਾ = dumpAlgo(&ਬਾਹਰਲਾ, &param, cert.subject_ਜਨਤਕ_ਕੁੰਜੀ_Algorithm.beg,
                    cert.subject_ਜਨਤਕ_ਕੁੰਜੀ_Algorithm.ਸਿਰਾ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਪੂਰਾ;
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl.certinfo) {
    ਨਤੀਜਾ = ssl_ਧੱਕੋ_certinfo_dyn(ਅੰਕੜੇ, certnum, "Public Key Algorithm",
                                   &ਬਾਹਰਲਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਪੂਰਾ;
  }

  rc = do_pubkey(ਅੰਕੜੇ, certnum, curlx_dyn_ptr(&ਬਾਹਰਲਾ),
                 &param, &cert.subject_ਜਨਤਕ_ਕੁੰਜੀ);
  ਜੇ(rc) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ; /* the most likely error */
    ਜਾਓ ਪੂਰਾ;
  }
  curlx_dyn_reset(&ਬਾਹਰਲਾ);

  /* Signature. */
  ਨਤੀਜਾ = ASN1tostr(&ਬਾਹਰਲਾ, &cert.signature);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਪੂਰਾ;
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl.certinfo) {
    ਨਤੀਜਾ = ssl_ਧੱਕੋ_certinfo_dyn(ਅੰਕੜੇ, certnum, "Signature", &ਬਾਹਰਲਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਪੂਰਾ;
  }
  curlx_dyn_reset(&ਬਾਹਰਲਾ);

  /* Generate PEM certificate. */
  ਨਤੀਜਾ = curlx_base64_encode((ਅਚਲ uint8_t *)cert.certificate.beg,
                               cert.certificate.ਸਿਰਾ - cert.certificate.beg,
                               &certptr, &clen);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਪੂਰਾ;

  /* Generate the final output certificate string. Format is:
     -----BEGIN CERTIFICATE-----\n
     <max 64 base64 characters>\n
     .
     .
     .
     -----END CERTIFICATE-----\n
   */

  curlx_dyn_reset(&ਬਾਹਰਲਾ);

  /* Build the certificate string. */
  ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&ਬਾਹਰਲਾ, "-----BEGIN CERTIFICATE-----\n");
  ਜੇ(!ਨਤੀਜਾ) {
    size_t j = 0;

    ਜਦੋਂ(!ਨਤੀਜਾ && (j < clen)) {
      size_t chunksize = (clen - j) > 64 ? 64 : (clen - j);
      ਨਤੀਜਾ = curlx_dyn_addn(&ਬਾਹਰਲਾ, &certptr[j], chunksize);
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = curlx_dyn_addn(&ਬਾਹਰਲਾ, "\n", 1);
      j += chunksize;
    }
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&ਬਾਹਰਲਾ, "-----END CERTIFICATE-----\n");
  }
  curlx_free(certptr);
  ਜੇ(!ਨਤੀਜਾ)
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl.certinfo)
      ਨਤੀਜਾ = ssl_ਧੱਕੋ_certinfo_dyn(ਅੰਕੜੇ, certnum, "Cert", &ਬਾਹਰਲਾ);

ਪੂਰਾ:
  ਜੇ(ਨਤੀਜਾ)
    failf(ਅੰਕੜੇ, "Failed extracting certificate chain");
  curlx_dyn_free(&ਬਾਹਰਲਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* WANT_EXTRACT_CERTINFO */

#ਜੇ_ਅੰਤ /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
