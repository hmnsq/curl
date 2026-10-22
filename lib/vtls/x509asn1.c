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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#ଯଦି defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_RUSTLS)
#define WANT_PARSEX509 /* uses Curl_parseX509() */
#endif

#ଯଦି defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_RUSTLS)
#define WANT_EXTRACT_CERTINFO /* uses Curl_extract_certinfo() */
#endif

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "curlx/base64.h"
#ଅନ୍ତର୍ଭାଵ "vtls/x509asn1.h"
#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"

/*
 * Constants.
 */

/* Largest supported ASN.1 structure. */
#define CURL_ASN1_MAX                   ((size_t)0x40000)      /* 256K */

#ifdef WANT_EXTRACT_CERTINFO

/* ASN.1 OID table entry. */
ସଂରଚନା Curl_OID {
  ଅଚଳ ଅକ୍ଷର *numoid;  /* Dotted-numeric OID. */
  ଅଚଳ ଅକ୍ଷର *textoid; /* OID name. */
};

/* ASN.1 OIDs. */
ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_OID OIDtable[] = {
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
  { ଖାଲି,                       ଖାଲି }
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

ସ୍ଥିର ଅଚଳ uint8_t *getASN1Element_(ସଂରଚନା Curl_asn1Element *elem,
                                      ଅଚଳ uint8_t *beg, ଅଚଳ uint8_t *end,
                                      size_t lvl)
{
  uint8_t b;
  size_t len;
  ସଂରଚନା Curl_asn1Element lelem;

  /* Get a single ASN.1 element into `elem', parse ASN.1 string at `beg'
     ending at `end'.
     Returns a pointer in source string after the parsed element, or NULL
     if an error occurs. */
  ଯଦି(!beg || !end || beg >= end || !*beg ||
     ((size_t)(end - beg) > CURL_ASN1_MAX) ||
     lvl++ >= CURL_ASN1_MAX_RECURSIONS)
    ଫେରନ୍ତୁ ଖାଲି;

  /* Process header byte. */
  elem->header = beg;
  b = *beg++;
  elem->constructed = (b & 0x20) != 0;
  elem->eclass = (b >> 6) & 3;
  b &= 0x1F;
  ଯଦି(b == 0x1F)
    ଫେରନ୍ତୁ ଖାଲି; /* Long tag values not supported here. */
  elem->tag = b;

  /* Process length. */
  ଯଦି(beg >= end)
    ଫେରନ୍ତୁ ଖାଲି;
  b = *beg++;
  ଯଦି(!(b & 0x80))
    len = b;
  ଅନ୍ୟଥା {
    b &= 0x7F;
    ଯଦି(!b) {
      /* Unspecified length. Since we have all the data, we can determine the
         effective length by skipping element until an EOC element is found. */
      ଯଦି(!elem->constructed)
        ଫେରନ୍ତୁ ଖାଲି;
      elem->beg = beg;
      ଯେତେବେଳେ(beg < end && *beg) {
        beg = getASN1Element_(&lelem, beg, end, lvl);
        ଯଦି(!beg)
          ଫେରନ୍ତୁ ଖାଲି;
      }
      ଯଦି(end - beg < 2 || beg[1])       /* EOC must have a zero length. */
        ଫେରନ୍ତୁ ଖାଲି;
      elem->end = beg;                  /* Do not include EOC as content. */
      ଫେରନ୍ତୁ beg + 2;                   /* After EOC element. */
    }
    ଅନ୍ୟଥା ଯଦି(b > (size_t)(end - beg))
      ଫେରନ୍ତୁ ଖାଲି; /* Does not fit in source. */
    ଅନ୍ୟଥା {
      /* Get long length. */
      len = 0;
      କର {
        ଯଦି(len & 0xFF000000L)
          ଫେରନ୍ତୁ ଖାଲି;  /* Lengths > 32 bits are not supported. */
        len = (len << 8) | *beg++;
      } ଯେତେବେଳେ(--b);
    }
  }
  ଯଦି(len > (size_t)(end - beg))
    ଫେରନ୍ତୁ ଖାଲି;  /* Element data does not fit in source. */
  elem->beg = beg;
  elem->end = beg + len;
  ଫେରନ୍ତୁ elem->end;
}

/*
 * @unittest 1657
 */
UNITTEST ଅଚଳ uint8_t *getASN1Element(ସଂରଚନା Curl_asn1Element *elem,
                                       ଅଚଳ uint8_t *beg, ଅଚଳ uint8_t *end);
UNITTEST ଅଚଳ uint8_t *getASN1Element(ସଂରଚନା Curl_asn1Element *elem,
                                       ଅଚଳ uint8_t *beg, ଅଚଳ uint8_t *end)
{
  ଫେରନ୍ତୁ getASN1Element_(elem, beg, end, 0);
}

#ifdef WANT_EXTRACT_CERTINFO

/*
 * Search the null-terminated OID or OID identifier in local table.
 * Return the table entry pointer or NULL if not found.
 */
ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_OID *ସନ୍ଧାନ_OID(ଅଚଳ ଅକ୍ଷର *oid)
{
  ଅଚଳ ସଂରଚନା Curl_OID *op;
  ପାଇଁ(op = OIDtable; op->numoid; op++)
    ଯଦି(!strcmp(op->numoid, oid) || curl_strequal(op->textoid, oid))
      ଫେରନ୍ତୁ op;

  ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Convert an ASN.1 Boolean value into its string representation.
 *
 * Return error code.
 */

ସ୍ଥିର CURLcode bool2str(ସଂରଚନା dynbuf *store,
                         ଅଚଳ uint8_t *beg, ଅଚଳ uint8_t *end)
{
  uint8_t val;
  ଯଦି(end - beg != 1)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  val = *beg;
  ଯଦି((val != 0xff) && (val != 0x00))
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ଫେରନ୍ତୁ curlx_dyn_add(store, val ? "TRUE" : "FALSE");
}

/*
 * Convert an ASN.1 octet string to a printable string.
 *
 * Return error code.
 */
ସ୍ଥିର CURLcode octet2str(ସଂରଚନା dynbuf *store,
                          ଅଚଳ uint8_t *beg, ଅଚଳ uint8_t *end)
{
  CURLcode result = CURLE_OK;

  ଯେତେବେଳେ(!result && beg < end)
    result = curlx_dyn_addf(store, "%02x:", *beg++);

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode bit2str(ସଂରଚନା dynbuf *store,
                        ଅଚଳ uint8_t *beg, ଅଚଳ uint8_t *end)
{
  /* Convert an ASN.1 bit string to a printable string. */

  ଯଦି(beg < end) {
    uint8_t unused_bits = *beg++;
    ଯଦି(unused_bits > 7)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ଫେରନ୍ତୁ octet2str(store, beg, end);
}

/*
 * Convert an ASN.1 integer value into its string representation.
 *
 * Returns error.
 */
ସ୍ଥିର CURLcode int2str(ସଂରଚନା dynbuf *store,
                        ଅଚଳ uint8_t *beg, ଅଚଳ uint8_t *end)
{
  uint32_t uval = 0;
  int32_t sval = 0;
  size_t n = end - beg;
  size_t i;
  ଅଚଳ uint8_t *p = beg;

  ଯଦି(!n)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  ଯଦି(n > 4)
    ଫେରନ୍ତୁ octet2str(store, beg, end);

  ଯଦି(p[0] & 0x80)
    uval = ~0U;

  ପାଇଁ(i = 0; i < n; i++)
    uval = (uval << 8) | p[i];

  sval = (int32_t)uval;

  /* small values are easier to read as decimals */
  ଯଦି(sval > -10000 && sval < 10000)
    ଫେରନ୍ତୁ curlx_dyn_addf(store, "%d", sval);

  /* represent larger values as hex */
  ଫେରନ୍ତୁ curlx_dyn_addf(store, "0x%x", uval);
}

/*
 * Convert from an ASN.1 typed string to UTF8.
 *
 * The result is stored in a dynbuf that is inited by the user of this
 * function.
 *
 * Returns error.
 */
ସ୍ଥିର CURLcode utf8asn1str(ସଂରଚନା dynbuf *to, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
                            ଅଚଳ uint8_t *from, ଅଚଳ uint8_t *end)
{
  size_t inlength = end - from;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା size = 1;
  CURLcode result = CURLE_OK;

  ବାଛ(type) {
  ସ୍ଥିତି CURL_ASN1_BMP_STRING:
    size = 2;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_ASN1_UNIVERSAL_STRING:
    size = 4;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_ASN1_NUMERIC_STRING:
  ସ୍ଥିତି CURL_ASN1_PRINTABLE_STRING:
  ସ୍ଥିତି CURL_ASN1_TELETEX_STRING:
  ସ୍ଥିତି CURL_ASN1_IA5_STRING:
  ସ୍ଥିତି CURL_ASN1_VISIBLE_STRING:
  ସ୍ଥିତି CURL_ASN1_UTF8_STRING:
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;  /* Conversion not supported. */
  }

  ଯଦି(inlength % size)
    /* Length inconsistent with character size. */
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  ଯଦି(type == CURL_ASN1_UTF8_STRING) {
    /* copy. */
    ଯଦି(inlength)
      result = curlx_dyn_addn(to, from, inlength);
  }
  ଅନ୍ୟଥା {
    ଯେତେବେଳେ(!result && (from < end)) {
      ଅକ୍ଷର buf[4]; /* decode buffer */
      size_t charsize = 1;
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା wc = 0;

      ବାଛ(size) {
      ସ୍ଥିତି 4:
        wc = (wc << 8) | *from++;
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      ସ୍ଥିତି 2:
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* case 1: */
        wc = (wc << 8) | *from++;
      }
      ଯଦି(wc >= 0x00000080) {
        ଯଦି(wc >= 0x00000800) {
          ଯଦି(wc >= 0x00010000) {
            ଯଦି(wc >= 0x00200000) {
              /* Invalid char. size for target encoding. */
              ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
            }
            buf[3] = (ଅକ୍ଷର)(0x80 | (wc & 0x3F));
            wc = (wc >> 6) | 0x00010000;
            charsize++;
          }
          buf[2] = (ଅକ୍ଷର)(0x80 | (wc & 0x3F));
          wc = (wc >> 6) | 0x00000800;
          charsize++;
        }
        buf[1] = (ଅକ୍ଷର)(0x80 | (wc & 0x3F));
        wc = (wc >> 6) | 0x000000C0;
        charsize++;
      }
      buf[0] = (ଅକ୍ଷର)wc;
      result = curlx_dyn_addn(to, buf, charsize);
    }
  }
  ଫେରନ୍ତୁ result;
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
UNITTEST CURLcode encodeOID(ସଂରଚନା dynbuf *store,
                            ଅଚଳ uint8_t *beg, ଅଚଳ uint8_t *end);
UNITTEST CURLcode encodeOID(ସଂରଚନା dynbuf *store,
                            ଅଚଳ uint8_t *beg, ଅଚଳ uint8_t *end)
{
  uint32_t x;
  uint32_t y;
  CURLcode result = CURLE_OK;

  ଯଦି(end <= beg)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  /* Process the first two numbers. The initial digit cannot be larger than
     2 */
  y = *beg++;
  ଯଦି(y <= 80)
    x = y / 40;
  ଅନ୍ୟଥା
    x = 2;
  result = curlx_dyn_addf(store, "%u", x);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ଯଦି(y & 0x80) {
    uint32_t t = (y & 0x7f); /* the second number */
    ଯଦି(!t)
      /* reject leading 0x80 padding */
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    କର {
      ଯଦି((t & 0xFE000000) || (beg >= end))
        ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      t = (t << 7) | (y & 0x7F);
    } ଯେତେବେଳେ(y & 0x80);
    y = t;
  }

  result = curlx_dyn_addf(store, ".%u", y - (x * 40));
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* Process the trailing numbers. */
  ଯେତେବେଳେ((beg < end) && !result) {
    x = 0;
    y = 0;
    କର {
      /* 0x80 as the first value is invalid since it only adds padding */
      ଯଦି((y & 0x80) && !x)
        ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
      ଅନ୍ୟଥା ଯଦି((x & 0xFE000000) || (beg == end))
        ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      x = (x << 7) | (y & 0x7F);
    } ଯେତେବେଳେ(y & 0x80);
    result = curlx_dyn_addf(store, ".%u", x);
  }
  ଫେରନ୍ତୁ result;
}

/*
 * Convert an ASN.1 OID into its dotted or symbolic string representation.
 *
 * Return error code.
 */

ସ୍ଥିର CURLcode OID2str(ସଂରଚନା dynbuf *store,
                        ଅଚଳ uint8_t *beg, ଅଚଳ uint8_t *end)
{
  CURLcode result = CURLE_OK;
  ଯଦି(beg < end) {
    ସଂରଚନା dynbuf buf;
    curlx_dyn_init(&buf, CURL_X509_STR_MAX);
    result = encodeOID(&buf, beg, end);

    ଯଦି(!result) {
      ଅଚଳ ସଂରଚନା Curl_OID *op = ସନ୍ଧାନ_OID(curlx_dyn_ptr(&buf));
      ଯଦି(op)
        result = curlx_dyn_add(store, op->textoid);
      ଅନ୍ୟଥା
        result = curlx_dyn_add(store, curlx_dyn_ptr(&buf));
    }
    curlx_dyn_free(&buf);
  }
  ଫେରନ୍ତୁ result;
}

/*
 * @unittest 1656
 */
UNITTEST CURLcode GTime2str(ସଂରଚନା dynbuf *store,
                            ଅଚଳ ଅକ୍ଷର *beg, ଅଚଳ ଅକ୍ଷର *end);
UNITTEST CURLcode GTime2str(ସଂରଚନା dynbuf *store,
                            ଅଚଳ ଅକ୍ଷର *beg, ଅଚଳ ଅକ୍ଷର *end)
{
  ଅଚଳ ଅକ୍ଷର *tzp;
  ଅଚଳ ଅକ୍ଷର *fracp;
  ଅକ୍ଷର sec1, sec2;
  size_t fracl;
  size_t tzl;
  ଅଚଳ ଅକ୍ଷର *sep = "";

  /* Convert an ASN.1 Generalized time to a printable string.
     Return the dynamically allocated string, or NULL if an error occurs. */

  ପାଇଁ(fracp = beg; fracp < end && ISDIGIT(*fracp); fracp++)
    ;

  /* Get seconds digits. */
  sec1 = '0';
  ବାଛ(fracp - beg - 12) {
  ସ୍ଥିତି 0:
    sec2 = '0';
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 2:
    sec1 = fracp[-2];
    FALLTHROUGH();
  ସ୍ଥିତି 1:
    sec2 = fracp[-1];
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* timezone follows optional fractional seconds. */
  tzp = fracp;
  fracl = 0; /* no fractional seconds detected so far */
  ଯଦି(fracp < end && (*fracp == '.' || *fracp == ',')) {
    /* Have fractional seconds, e.g. "[.,]\d+". How many? */
    fracp++; /* should be a digit char or BAD ARGUMENT */
    tzp = fracp;
    ଯେତେବେଳେ(tzp < end && ISDIGIT(*tzp))
      tzp++;
    ଯଦି(tzp == fracp) /* never looped, no digit after [.,] */
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    fracl = tzp - fracp; /* number of fractional sec digits */
    DEBUGASSERT(fracl > 0);
    /* Strip trailing zeroes in fractional seconds.
     * May reduce fracl to 0 if only '0's are present. */
    ଯେତେବେଳେ(fracl && fracp[fracl - 1] == '0')
      fracl--;
  }

  /* Process timezone. */
  ଯଦି(tzp >= end) {
    tzp = "";
    tzl = 0;
  }
  ଅନ୍ୟଥା ଯଦି(*tzp == 'Z') {
    sep = " ";
    tzp = "GMT";
    tzl = 3;
  }
  ଅନ୍ୟଥା ଯଦି((*tzp == '+') || (*tzp == '-')) {
    sep = " UTC";
    tzl = end - tzp;
  }
  ଅନ୍ୟଥା {
    sep = " ";
    tzl = end - tzp;
  }

  ଫେରନ୍ତୁ curlx_dyn_addf(store,
                        "%.4s-%.2s-%.2s %.2s:%.2s:%c%c%s%.*s%s%.*s",
                        beg, beg + 4, beg + 6,
                        beg + 8, beg + 10, sec1, sec2,
                        fracl ? "." : "", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)fracl, fracp,
                        sep, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tzl, tzp);
}

/*
 * Convert an ASN.1 UTC time to a printable string.
 *
 * Return error code.
 */
ସ୍ଥିର CURLcode UTime2str(ସଂରଚନା dynbuf *store,
                          ଅଚଳ ଅକ୍ଷର *beg, ଅଚଳ ଅକ୍ଷର *end)
{
  ଅଚଳ ଅକ୍ଷର *tzp;
  size_t tzl;
  ଅଚଳ ଅକ୍ଷର *sec;

  ପାଇଁ(tzp = beg; tzp < end && *tzp >= '0' && *tzp <= '9'; tzp++)
    ;
  /* Get the seconds. */
  sec = beg + 10;
  ବାଛ(tzp - sec) {
  ସ୍ଥିତି 0:
    sec = "00";
    FALLTHROUGH();
  ସ୍ଥିତି 2:
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Process timezone. */
  ଯଦି(tzp >= end)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ଯଦି(*tzp == 'Z') {
    tzp = "GMT";
    end = tzp + 3;
  }

  tzl = end - tzp;
  ଫେରନ୍ତୁ curlx_dyn_addf(store, "%d%.2s-%.2s-%.2s %.2s:%.2s:%.2s %.*s",
                        20 - (*beg >= '5'), beg, beg + 2, beg + 4,
                        beg + 6, beg + 8, sec,
                        (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tzl, tzp);
}

/*
 * Convert an ASN.1 element to a printable string.
 *
 * Return error
 *
 * @unittest 1667
 */
UNITTEST CURLcode ASN1tostr(ସଂରଚନା dynbuf *store,
                            ସଂରଚନା Curl_asn1Element *elem);
UNITTEST CURLcode ASN1tostr(ସଂରଚନା dynbuf *store,
                            ସଂରଚନା Curl_asn1Element *elem)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା type = elem->tag;
  ଯଦି(elem->constructed)
    ଫେରନ୍ତୁ result; /* No conversion of structured elements. */

  ବାଛ(type) {
  ସ୍ଥିତି CURL_ASN1_BOOLEAN:
    result = bool2str(store, elem->beg, elem->end);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_ASN1_INTEGER:
  ସ୍ଥିତି CURL_ASN1_ENUMERATED:
    result = int2str(store, elem->beg, elem->end);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_ASN1_BIT_STRING:
    result = bit2str(store, elem->beg, elem->end);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_ASN1_OCTET_STRING:
    result = octet2str(store, elem->beg, elem->end);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_ASN1_NULL:
    result = curlx_dyn_addn(store, "", 1);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_ASN1_OBJECT_IDENTIFIER:
    result = OID2str(store, elem->beg, elem->end);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_ASN1_UTC_TIME:
    result = UTime2str(store, (ଅଚଳ ଅକ୍ଷର *)elem->beg,
                       (ଅଚଳ ଅକ୍ଷର *)elem->end);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_ASN1_GENERALIZED_TIME:
    result = GTime2str(store, (ଅଚଳ ଅକ୍ଷର *)elem->beg,
                       (ଅଚଳ ଅକ୍ଷର *)elem->end);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_ASN1_UTF8_STRING:
  ସ୍ଥିତି CURL_ASN1_NUMERIC_STRING:
  ସ୍ଥିତି CURL_ASN1_PRINTABLE_STRING:
  ସ୍ଥିତି CURL_ASN1_TELETEX_STRING:
  ସ୍ଥିତି CURL_ASN1_IA5_STRING:
  ସ୍ଥିତି CURL_ASN1_VISIBLE_STRING:
  ସ୍ଥିତି CURL_ASN1_UNIVERSAL_STRING:
  ସ୍ଥିତି CURL_ASN1_BMP_STRING:
    result = utf8asn1str(store, type, elem->beg, elem->end);
    ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ result;
}

/*
 * ASCII encode distinguished name at `dn' into the store dynbuf.
 *
 * Returns error.
 */
ସ୍ଥିର CURLcode encodeDN(ସଂରଚନା dynbuf *store, ସଂରଚନା Curl_asn1Element *dn)
{
  ସଂରଚନା Curl_asn1Element rdn;
  ସଂରଚନା Curl_asn1Element atv;
  ସଂରଚନା Curl_asn1Element oid;
  ସଂରଚନା Curl_asn1Element ମୂଲ୍ୟ;
  ଅଚଳ uint8_t *p1;
  ଅଚଳ uint8_t *p2;
  ଅଚଳ uint8_t *p3;
  ଅଚଳ uint8_t *str;
  CURLcode result = CURLE_OK;
  bool added = FALSE;
  ସଂରଚନା dynbuf temp;
  curlx_dyn_init(&temp, CURL_X509_STR_MAX);

  ପାଇଁ(p1 = dn->beg; p1 < dn->end;) {
    p1 = getASN1Element(&rdn, p1, dn->end);
    ଯଦି(!p1) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      ଯାଅ error;
    }
    ପାଇଁ(p2 = rdn.beg; p2 < rdn.end;) {
      p2 = getASN1Element(&atv, p2, rdn.end);
      ଯଦି(!p2) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        ଯାଅ error;
      }
      p3 = getASN1Element(&oid, atv.beg, atv.end);
      ଯଦି(!p3) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        ଯାଅ error;
      }
      ଯଦି(!getASN1Element(&ମୂଲ୍ୟ, p3, atv.end)) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        ଯାଅ error;
      }
      curlx_dyn_reset(&temp);
      result = ASN1tostr(&temp, &oid);
      ଯଦି(result)
        ଯାଅ error;

      str = curlx_dyn_uptr(&temp);

      ଯଦି(!str) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        ଯାଅ error;
      }

      /* Encode delimiter.
         If attribute has a short uppercase name, delimiter is ", ". */
      ପାଇଁ(p3 = str; ISUPPER(*p3); p3++)
        ;
      ଯଦି(added) {
        ଯଦି(p3 - str > 2)
          result = curlx_dyn_addn(store, "/", 1);
        ଅନ୍ୟଥା
          result = curlx_dyn_addn(store, ", ", 2);
        ଯଦି(result)
          ଯାଅ error;
      }

      /* Encode attribute name. */
      result = curlx_dyn_add(store, (ଅଚଳ ଅକ୍ଷର *)str);
      ଯଦି(result)
        ଯାଅ error;

      /* Generate equal sign. */
      result = curlx_dyn_addn(store, "=", 1);
      ଯଦି(result)
        ଯାଅ error;

      /* Generate value. */
      result = ASN1tostr(store, &ମୂଲ୍ୟ);
      ଯଦି(result)
        ଯାଅ error;
      curlx_dyn_reset(&temp);
      added = TRUE; /* use separator for next */
    }
  }
error:
  curlx_dyn_free(&temp);

  ଫେରନ୍ତୁ result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#ifdef WANT_PARSEX509
/*
 * ASN.1 parse an X509 certificate into structure subfields.
 * Syntax is assumed to have already been checked by the SSL backend.
 * See RFC 5280.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_parseX509(ସଂରଚନା Curl_X509certificate *cert,
                   ଅଚଳ uint8_t *beg, ଅଚଳ uint8_t *end)
{
  ସଂରଚନା Curl_asn1Element elem;
  ସଂରଚନା Curl_asn1Element tbsCertificate;
  ଅଚଳ uint8_t *ccp;
  ସ୍ଥିର ଅଚଳ uint8_t defaultVersion = 0;  /* v1. */

  memset(cert, 0, ଆକାର(*cert));
  cert->certificate.header = ଖାଲି;
  cert->certificate.beg = beg;
  cert->certificate.end = end;

  /* Get the sequence content. */
  ଯଦି(!getASN1Element(&elem, beg, end))
    ଫେରନ୍ତୁ -1;  /* Invalid bounds/size. */
  beg = elem.beg;
  end = elem.end;

  /* Get tbsCertificate. */
  beg = getASN1Element(&tbsCertificate, beg, end);
  ଯଦି(!beg)
    ଫେରନ୍ତୁ -1;
  /* Skip the signatureAlgorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  ଯଦି(!beg)
    ଫେରନ୍ତୁ -1;
  /* Get the signatureValue. */
  ଯଦି(!getASN1Element(&cert->signature, beg, end))
    ଫେରନ୍ତୁ -1;

  /* Parse TBSCertificate. */
  beg = tbsCertificate.beg;
  end = tbsCertificate.end;
  /* Get optional version, get serialNumber. */
  cert->version.header = ଖାଲି;
  cert->version.beg = &defaultVersion;
  cert->version.end = &defaultVersion + ଆକାର(defaultVersion);
  beg = getASN1Element(&elem, beg, end);
  ଯଦି(!beg)
    ଫେରନ୍ତୁ -1;
  ଯଦି(elem.tag == 0) {
    ଯଦି(!getASN1Element(&cert->version, elem.beg, elem.end))
      ଫେରନ୍ତୁ -1;
    beg = getASN1Element(&elem, beg, end);
    ଯଦି(!beg)
      ଫେରନ୍ତୁ -1;
  }
  cert->serial_ସଂଖ୍ୟା = elem;
  /* Get signature algorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  /* Get issuer. */
  beg = getASN1Element(&cert->issuer, beg, end);
  ଯଦି(!beg)
    ଫେରନ୍ତୁ -1;
  /* Get notBefore and notAfter. */
  beg = getASN1Element(&elem, beg, end);
  ଯଦି(!beg)
    ଫେରନ୍ତୁ -1;
  ccp = getASN1Element(&cert->notBefore, elem.beg, elem.end);
  ଯଦି(!ccp)
    ଫେରନ୍ତୁ -1;
  ଯଦି(!getASN1Element(&cert->notAfter, ccp, elem.end))
    ଫେରନ୍ତୁ -1;
  /* These two fields MUST consume all of the validity elem */
  ଯଦି(elem.end != cert->notAfter.end)
    ଫେରନ୍ତୁ -1;
  /* Get subject. */
  beg = getASN1Element(&cert->subject, beg, end);
  ଯଦି(!beg)
    ଫେରନ୍ତୁ -1;
  /* Get subjectPublicKeyAlgorithm and subjectPublicKey. */
  beg = getASN1Element(&cert->subject_Public_ଚାବି_Info, beg, end);
  ଯଦି(!beg)
    ଫେରନ୍ତୁ -1;
  ccp = getASN1Element(&cert->subject_Public_ଚାବି_Algorithm,
                       cert->subject_Public_ଚାବି_Info.beg,
                       cert->subject_Public_ଚାବି_Info.end);
  ଯଦି(!ccp)
    ଫେରନ୍ତୁ -1;
  ଯଦି(!getASN1Element(&cert->subject_Public_ଚାବି, ccp,
                     cert->subject_Public_ଚାବି_Info.end))
    ଫେରନ୍ତୁ -1;
  /* Get optional issuerUniqueID, subjectUniqueID and extensions. */
  cert->issuerUniqueID.tag = cert->subjectUniqueID.tag = 0;
  cert->extensions.tag = elem.tag = 0;
  cert->issuerUniqueID.header = cert->subjectUniqueID.header = ଖାଲି;
  cert->issuerUniqueID.beg = cert->issuerUniqueID.end = beg;
  cert->subjectUniqueID.beg = cert->subjectUniqueID.end = beg;
  cert->extensions.header = ଖାଲି;
  cert->extensions.beg = cert->extensions.end = beg;
  ଯଦି(beg < end) {
    beg = getASN1Element(&elem, beg, end);
    ଯଦି(!beg)
      ଫେରନ୍ତୁ -1;
  }
  ଯଦି(elem.tag == 1) {
    cert->issuerUniqueID = elem;
    ଯଦି(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      ଯଦି(!beg)
        ଫେରନ୍ତୁ -1;
    }
  }
  ଯଦି(elem.tag == 2) {
    cert->subjectUniqueID = elem;
    ଯଦି(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      ଯଦି(!beg)
        ଫେରନ୍ତୁ -1;
    }
  }
  ଯଦି(elem.tag == 3)
    ଯଦି(!getASN1Element(&cert->extensions, elem.beg, elem.end))
      ଫେରନ୍ତୁ -1;
  ଫେରନ୍ତୁ 0;
}

#endif /* WANT_PARSEX509 */

#ifdef WANT_EXTRACT_CERTINFO

ସ୍ଥିର CURLcode dumpAlgo(ସଂରଚନା dynbuf *store,
                         ସଂରଚନା Curl_asn1Element *param,
                         ଅଚଳ uint8_t *beg, ଅଚଳ uint8_t *end)
{
  ସଂରଚନା Curl_asn1Element oid;

  /* Get algorithm parameters and return algorithm name. */

  beg = getASN1Element(&oid, beg, end);
  ଯଦି(!beg)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  param->header = ଖାଲି;
  param->tag = 0;
  param->beg = param->end = end;
  ଯଦି(beg < end) {
    ଅଚଳ uint8_t *p = getASN1Element(param, beg, end);
    ଯଦି(!p)
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  ଫେରନ୍ତୁ OID2str(store, oid.beg, oid.end);
}

/*
 * This is a convenience function for push_certinfo_len that takes a zero
 * terminated value.
 */
ସ୍ଥିର CURLcode ssl_ଠେଲ_certinfo(ସଂରଚନା Curl_easy *data,
                                  ପୂର୍ଣ୍ଣସଂଖ୍ୟା certnum,
                                  ଅଚଳ ଅକ୍ଷର *label,
                                  ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ)
{
  size_t valuelen = strlen(ମୂଲ୍ୟ);

  ଫେରନ୍ତୁ Curl_ssl_ଠେଲ_certinfo_len(data, certnum, label, ମୂଲ୍ୟ, valuelen);
}

/*
 * This is a convenience function for push_certinfo_len that takes a
 * dynbuf value.
 *
 * It also does the verbose output if !certnum.
 */
ସ୍ଥିର CURLcode ssl_ଠେଲ_certinfo_dyn(ସଂରଚନା Curl_easy *data,
                                      ପୂର୍ଣ୍ଣସଂଖ୍ୟା certnum,
                                      ଅଚଳ ଅକ୍ଷର *label,
                                      ସଂରଚନା dynbuf *ptr)
{
  size_t valuelen = curlx_dyn_len(ptr);
  ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ = curlx_dyn_ptr(ptr);

  CURLcode result = Curl_ssl_ଠେଲ_certinfo_len(data, certnum, label,
                                               ମୂଲ୍ୟ, valuelen);

  ଯଦି(!certnum && !result)
    infof(data, "   %s: %s", label, ମୂଲ୍ୟ);

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode do_pubkey_field(ସଂରଚନା Curl_easy *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା certnum,
                                ଅଚଳ ଅକ୍ଷର *label,
                                ସଂରଚନା Curl_asn1Element *elem)
{
  CURLcode result;
  ସଂରଚନା dynbuf out;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);

  /* Generate a certificate information record for the public key. */

  result = ASN1tostr(&out, elem);
  ଯଦି(!result && data->set.ssl.certinfo)
    result = ssl_ଠେଲ_certinfo_dyn(data, certnum, label, &out);
  curlx_dyn_free(&out);
  ଫେରନ୍ତୁ result;
}

/* return 0 on success, 1 on error */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_pubkey(ସଂରଚନା Curl_easy *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା certnum, ଅଚଳ ଅକ୍ଷର *algo,
                     ସଂରଚନା Curl_asn1Element *param,
                     ସଂରଚନା Curl_asn1Element *pubkey)
{
  ସଂରଚନା Curl_asn1Element elem;
  ସଂରଚନା Curl_asn1Element pk;
  ଅଚଳ uint8_t *p;

  /* Generate all information records for the public key. */

  ଯଦି(curl_strequal(algo, "ecPublicKey")) {
    /*
     * ECC public key is all the data, a value of type BIT STRING mapped to
     * OCTET STRING and should not be parsed as an ASN.1 value.
     */
    ଅଚଳ size_t dlen = pubkey->end - pubkey->beg;
    size_t len;
    ଯଦି(dlen < 2)
      /* too small */
      ଫେରନ୍ତୁ 1;
    len = (dlen - 2) * 4;
    ଯଦି(!certnum)
      infof(data, "   ECC Public Key (%zu bits)", len);
    ଯଦି(data->set.ssl.certinfo) {
      ଅକ୍ଷର q[(ଆକାର(len) * 8 / 3) + 1];
      (ଶୂନ୍ୟ)curl_msnprintf(q, ଆକାର(q), "%zu", len);
      ଯଦି(ssl_ଠେଲ_certinfo(data, certnum, "ECC Public Key", q))
        ଫେରନ୍ତୁ 1;
    }
    ଫେରନ୍ତୁ do_pubkey_field(data, certnum, "ecPublicKey", pubkey) == CURLE_OK
      ? 0 : 1;
  }

  /* Get the public key (single element). */
  ଯଦି(!getASN1Element(&pk, pubkey->beg + 1, pubkey->end))
    ଫେରନ୍ତୁ 1;

  ଯଦି(curl_strequal(algo, "rsaEncryption")) {
    ଅଚଳ uint8_t *q;
    size_t len;

    p = getASN1Element(&elem, pk.beg, pk.end);
    ଯଦି(!p)
      ଫେରନ୍ତୁ 1;

    /* Compute key length. */
    ପାଇଁ(q = elem.beg; q < elem.end && !*q; q++)
      ;
    len = ((elem.end - q) * 8);
    ଯଦି(len) {
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
      ପାଇଁ(i = *q; !(i & 0x80); i <<= 1)
        len--;
    }
    ଯଦି(len > 32)
      elem.beg = q;     /* Strip leading zero bytes. */
    ଯଦି(!certnum)
      infof(data, "   RSA Public Key (%zu bits)", len);
    ଯଦି(data->set.ssl.certinfo) {
      ଅକ୍ଷର r[(ଆକାର(len) * 8 / 3) + 1];
      curl_msnprintf(r, ଆକାର(r), "%zu", len);
      ଯଦି(ssl_ଠେଲ_certinfo(data, certnum, "RSA Public Key", r))
        ଫେରନ୍ତୁ 1;
    }
    /* Generate coefficients. */
    ଯଦି(do_pubkey_field(data, certnum, "rsa(n)", &elem))
      ଫେରନ୍ତୁ 1;
    ଯଦି(!getASN1Element(&elem, p, pk.end))
      ଫେରନ୍ତୁ 1;
    ଯଦି(do_pubkey_field(data, certnum, "rsa(e)", &elem))
      ଫେରନ୍ତୁ 1;
  }
  ଅନ୍ୟଥା ଯଦି(curl_strequal(algo, "dsa")) {
    p = getASN1Element(&elem, param->beg, param->end);
    ଯଦି(p) {
      ଯଦି(do_pubkey_field(data, certnum, "dsa(p)", &elem))
        ଫେରନ୍ତୁ 1;
      p = getASN1Element(&elem, p, param->end);
      ଯଦି(p) {
        ଯଦି(do_pubkey_field(data, certnum, "dsa(q)", &elem))
          ଫେରନ୍ତୁ 1;
        ଯଦି(getASN1Element(&elem, p, param->end)) {
          ଯଦି(do_pubkey_field(data, certnum, "dsa(g)", &elem))
            ଫେରନ୍ତୁ 1;
          ଯଦି(do_pubkey_field(data, certnum, "dsa(pub_key)", &pk))
            ଫେରନ୍ତୁ 1;
        }
      }
    }
  }
  ଅନ୍ୟଥା ଯଦି(curl_strequal(algo, "dhpublicnumber")) {
    p = getASN1Element(&elem, param->beg, param->end);
    ଯଦି(p) {
      ଯଦି(do_pubkey_field(data, certnum, "dh(p)", &elem))
        ଫେରନ୍ତୁ 1;
      ଯଦି(getASN1Element(&elem, p, param->end)) {
        ଯଦି(do_pubkey_field(data, certnum, "dh(g)", &elem))
          ଫେରନ୍ତୁ 1;
        ଯଦି(do_pubkey_field(data, certnum, "dh(pub_key)", &pk))
          ଫେରନ୍ତୁ 1;
      }
    }
  }
  ଫେରନ୍ତୁ 0;
}

/*
 * Convert an ASN.1 distinguished name into a printable string.
 * Return error.
 */
ସ୍ଥିର CURLcode DNtostr(ସଂରଚନା dynbuf *store, ସଂରଚନା Curl_asn1Element *dn)
{
  ଫେରନ୍ତୁ encodeDN(store, dn);
}

CURLcode Curl_extract_certinfo(ସଂରଚନା Curl_easy *data,
                               ପୂର୍ଣ୍ଣସଂଖ୍ୟା certnum,
                               ଅଚଳ uint8_t *beg,
                               ଅଚଳ uint8_t *end)
{
  ସଂରଚନା Curl_X509certificate cert;
  ସଂରଚନା Curl_asn1Element param;
  ଅକ୍ଷର *certptr;
  size_t clen;
  ସଂରଚନା dynbuf out;
  CURLcode result = CURLE_OK;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା version;
  ଅଚଳ uint8_t *ptr;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  ଯଦି(!data->set.ssl.certinfo)
    ଯଦି(certnum)
      ଫେରନ୍ତୁ CURLE_OK;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);
  /* Prepare the certificate information for curl_easy_getinfo(). */

  /* Extract the certificate ASN.1 elements. */
  ଯଦି(Curl_parseX509(&cert, beg, end))
    ଫେରନ୍ତୁ CURLE_PEER_FAILED_VERIFICATION;

  /* Subject. */
  result = DNtostr(&out, &cert.subject);
  ଯଦି(result)
    ଯାଅ done;
  ଯଦି(data->set.ssl.certinfo) {
    result = ssl_ଠେଲ_certinfo_dyn(data, certnum, "Subject", &out);
    ଯଦି(result)
      ଯାଅ done;
  }
  curlx_dyn_reset(&out);

  /* Issuer. */
  result = DNtostr(&out, &cert.issuer);
  ଯଦି(result)
    ଯାଅ done;
  ଯଦି(data->set.ssl.certinfo) {
    result = ssl_ଠେଲ_certinfo_dyn(data, certnum, "Issuer", &out);
    ଯଦି(result)
      ଯାଅ done;
  }
  curlx_dyn_reset(&out);

  /* Version (always fits in less than 32 bits). */
  version = 0;
  ପାଇଁ(ptr = cert.version.beg; ptr < cert.version.end; ptr++)
    version = (version << 8) | *ptr;
  ଯଦି(data->set.ssl.certinfo) {
    result = curlx_dyn_addf(&out, "%x", version);
    ଯଦି(result)
      ଯାଅ done;
    result = ssl_ଠେଲ_certinfo_dyn(data, certnum, "Version", &out);
    ଯଦି(result)
      ଯାଅ done;
    curlx_dyn_reset(&out);
  }

  /* Serial number. */
  result = ASN1tostr(&out, &cert.serial_ସଂଖ୍ୟା);
  ଯଦି(result)
    ଯାଅ done;
  ଯଦି(data->set.ssl.certinfo) {
    result = ssl_ଠେଲ_certinfo_dyn(data, certnum, "Serial Number", &out);
    ଯଦି(result)
      ଯାଅ done;
  }
  curlx_dyn_reset(&out);

  /* Signature algorithm .*/
  result = dumpAlgo(&out, &param, cert.signatureAlgorithm.beg,
                    cert.signatureAlgorithm.end);
  ଯଦି(result)
    ଯାଅ done;
  ଯଦି(data->set.ssl.certinfo) {
    result = ssl_ଠେଲ_certinfo_dyn(data, certnum, "Signature Algorithm", &out);
    ଯଦି(result)
      ଯାଅ done;
  }
  curlx_dyn_reset(&out);

  /* Start Date. */
  result = ASN1tostr(&out, &cert.notBefore);
  ଯଦି(result)
    ଯାଅ done;
  ଯଦି(data->set.ssl.certinfo) {
    result = ssl_ଠେଲ_certinfo_dyn(data, certnum, "Start Date", &out);
    ଯଦି(result)
      ଯାଅ done;
  }
  curlx_dyn_reset(&out);

  /* Expire Date. */
  result = ASN1tostr(&out, &cert.notAfter);
  ଯଦି(result)
    ଯାଅ done;
  ଯଦି(data->set.ssl.certinfo) {
    result = ssl_ଠେଲ_certinfo_dyn(data, certnum, "Expire Date", &out);
    ଯଦି(result)
      ଯାଅ done;
  }
  curlx_dyn_reset(&out);

  /* Public Key Algorithm. */
  result = dumpAlgo(&out, &param, cert.subject_Public_ଚାବି_Algorithm.beg,
                    cert.subject_Public_ଚାବି_Algorithm.end);
  ଯଦି(result)
    ଯାଅ done;
  ଯଦି(data->set.ssl.certinfo) {
    result = ssl_ଠେଲ_certinfo_dyn(data, certnum, "Public Key Algorithm",
                                   &out);
    ଯଦି(result)
      ଯାଅ done;
  }

  rc = do_pubkey(data, certnum, curlx_dyn_ptr(&out),
                 &param, &cert.subject_Public_ଚାବି);
  ଯଦି(rc) {
    result = CURLE_OUT_OF_MEMORY; /* the most likely error */
    ଯାଅ done;
  }
  curlx_dyn_reset(&out);

  /* Signature. */
  result = ASN1tostr(&out, &cert.signature);
  ଯଦି(result)
    ଯାଅ done;
  ଯଦି(data->set.ssl.certinfo) {
    result = ssl_ଠେଲ_certinfo_dyn(data, certnum, "Signature", &out);
    ଯଦି(result)
      ଯାଅ done;
  }
  curlx_dyn_reset(&out);

  /* Generate PEM certificate. */
  result = curlx_base64_encode((ଅଚଳ uint8_t *)cert.certificate.beg,
                               cert.certificate.end - cert.certificate.beg,
                               &certptr, &clen);
  ଯଦି(result)
    ଯାଅ done;

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
  ଯଦି(!result) {
    size_t j = 0;

    ଯେତେବେଳେ(!result && (j < clen)) {
      size_t chunksize = (clen - j) > 64 ? 64 : (clen - j);
      result = curlx_dyn_addn(&out, &certptr[j], chunksize);
      ଯଦି(!result)
        result = curlx_dyn_addn(&out, "\n", 1);
      j += chunksize;
    }
    ଯଦି(!result)
      result = curlx_dyn_add(&out, "-----END CERTIFICATE-----\n");
  }
  curlx_free(certptr);
  ଯଦି(!result)
    ଯଦି(data->set.ssl.certinfo)
      result = ssl_ଠେଲ_certinfo_dyn(data, certnum, "Cert", &out);

done:
  ଯଦି(result)
    failf(data, "Failed extracting certificate chain");
  curlx_dyn_free(&out);
  ଫେରନ୍ତୁ result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#endif /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
