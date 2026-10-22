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
#অন্তর্ভাৱ "curl_setup.h"

#যদি defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#যদি defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_RUSTLS)
#define WANT_PARSEX509 /* uses Curl_parseX509() */
#endif

#যদি defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_RUSTLS)
#define WANT_EXTRACT_CERTINFO /* uses Curl_extract_certinfo() */
#endif

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "vtls/vtls.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "curlx/base64.h"
#অন্তর্ভাৱ "vtls/x509asn1.h"
#অন্তর্ভাৱ "curlx/dynbuf.h"

/*
 * Constants.
 */

/* Largest supported ASN.1 structure. */
#define CURL_ASN1_MAX                   ((size_t)0x40000)      /* 256K */

#ifdef WANT_EXTRACT_CERTINFO

/* ASN.1 OID table entry. */
সংরচনা Curl_OID {
  ধ্রুৱ ৱর্ণ *numoid;  /* Dotted-numeric OID. */
  ধ্রুৱ ৱর্ণ *textoid; /* OID name. */
};

/* ASN.1 OIDs. */
স্থির ধ্রুৱ সংরচনা Curl_OID OIDtable[] = {
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
  { রিক্ত,                       রিক্ত }
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

স্থির ধ্রুৱ uint8_t *getASN1Element_(সংরচনা Curl_asn1Element *elem,
                                      ধ্রুৱ uint8_t *beg, ধ্রুৱ uint8_t *end,
                                      size_t lvl)
{
  uint8_t b;
  size_t len;
  সংরচনা Curl_asn1Element lelem;

  /* Get a single ASN.1 element into `elem', parse ASN.1 string at `beg'
     ending at `end'.
     Returns a pointer in source string after the parsed element, or NULL
     if an error occurs. */
  যদি(!beg || !end || beg >= end || !*beg ||
     ((size_t)(end - beg) > CURL_ASN1_MAX) ||
     lvl++ >= CURL_ASN1_MAX_RECURSIONS)
    ঘূৰাই রিক্ত;

  /* Process header byte. */
  elem->header = beg;
  b = *beg++;
  elem->constructed = (b & 0x20) != 0;
  elem->eclass = (b >> 6) & 3;
  b &= 0x1F;
  যদি(b == 0x1F)
    ঘূৰাই রিক্ত; /* Long tag values not supported here. */
  elem->tag = b;

  /* Process length. */
  যদি(beg >= end)
    ঘূৰাই রিক্ত;
  b = *beg++;
  যদি(!(b & 0x80))
    len = b;
  অন্যথা {
    b &= 0x7F;
    যদি(!b) {
      /* Unspecified length. Since we have all the data, we can determine the
         effective length by skipping element until an EOC element is found. */
      যদি(!elem->constructed)
        ঘূৰাই রিক্ত;
      elem->beg = beg;
      যেতিয়ালৈ(beg < end && *beg) {
        beg = getASN1Element_(&lelem, beg, end, lvl);
        যদি(!beg)
          ঘূৰাই রিক্ত;
      }
      যদি(end - beg < 2 || beg[1])       /* EOC must have a zero length. */
        ঘূৰাই রিক্ত;
      elem->end = beg;                  /* Do not include EOC as content. */
      ঘূৰাই beg + 2;                   /* After EOC element. */
    }
    অন্যথা যদি(b > (size_t)(end - beg))
      ঘূৰাই রিক্ত; /* Does not fit in source. */
    অন্যথা {
      /* Get long length. */
      len = 0;
      কৰা {
        যদি(len & 0xFF000000L)
          ঘূৰাই রিক্ত;  /* Lengths > 32 bits are not supported. */
        len = (len << 8) | *beg++;
      } যেতিয়ালৈ(--b);
    }
  }
  যদি(len > (size_t)(end - beg))
    ঘূৰাই রিক্ত;  /* Element data does not fit in source. */
  elem->beg = beg;
  elem->end = beg + len;
  ঘূৰাই elem->end;
}

/*
 * @unittest 1657
 */
UNITTEST ধ্রুৱ uint8_t *getASN1Element(সংরচনা Curl_asn1Element *elem,
                                       ধ্রুৱ uint8_t *beg, ধ্রুৱ uint8_t *end);
UNITTEST ধ্রুৱ uint8_t *getASN1Element(সংরচনা Curl_asn1Element *elem,
                                       ধ্রুৱ uint8_t *beg, ধ্রুৱ uint8_t *end)
{
  ঘূৰাই getASN1Element_(elem, beg, end, 0);
}

#ifdef WANT_EXTRACT_CERTINFO

/*
 * Search the null-terminated OID or OID identifier in local table.
 * Return the table entry pointer or NULL if not found.
 */
স্থির ধ্রুৱ সংরচনা Curl_OID *অনুসন্ধান_OID(ধ্রুৱ ৱর্ণ *oid)
{
  ধ্রুৱ সংরচনা Curl_OID *op;
  বাবে(op = OIDtable; op->numoid; op++)
    যদি(!strcmp(op->numoid, oid) || curl_strequal(op->textoid, oid))
      ঘূৰাই op;

  ঘূৰাই রিক্ত;
}

/*
 * Convert an ASN.1 Boolean value into its string representation.
 *
 * Return error code.
 */

স্থির CURLcode bool2str(সংরচনা dynbuf *store,
                         ধ্রুৱ uint8_t *beg, ধ্রুৱ uint8_t *end)
{
  uint8_t val;
  যদি(end - beg != 1)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  val = *beg;
  যদি((val != 0xff) && (val != 0x00))
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  ঘূৰাই curlx_dyn_add(store, val ? "TRUE" : "FALSE");
}

/*
 * Convert an ASN.1 octet string to a printable string.
 *
 * Return error code.
 */
স্থির CURLcode octet2str(সংরচনা dynbuf *store,
                          ধ্রুৱ uint8_t *beg, ধ্রুৱ uint8_t *end)
{
  CURLcode result = CURLE_OK;

  যেতিয়ালৈ(!result && beg < end)
    result = curlx_dyn_addf(store, "%02x:", *beg++);

  ঘূৰাই result;
}

স্থির CURLcode bit2str(সংরচনা dynbuf *store,
                        ধ্রুৱ uint8_t *beg, ধ্রুৱ uint8_t *end)
{
  /* Convert an ASN.1 bit string to a printable string. */

  যদি(beg < end) {
    uint8_t unused_bits = *beg++;
    যদি(unused_bits > 7)
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  }
  অন্যথা
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  ঘূৰাই octet2str(store, beg, end);
}

/*
 * Convert an ASN.1 integer value into its string representation.
 *
 * Returns error.
 */
স্থির CURLcode int2str(সংরচনা dynbuf *store,
                        ধ্রুৱ uint8_t *beg, ধ্রুৱ uint8_t *end)
{
  uint32_t uval = 0;
  int32_t sval = 0;
  size_t n = end - beg;
  size_t i;
  ধ্রুৱ uint8_t *p = beg;

  যদি(!n)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(n > 4)
    ঘূৰাই octet2str(store, beg, end);

  যদি(p[0] & 0x80)
    uval = ~0U;

  বাবে(i = 0; i < n; i++)
    uval = (uval << 8) | p[i];

  sval = (int32_t)uval;

  /* small values are easier to read as decimals */
  যদি(sval > -10000 && sval < 10000)
    ঘূৰাই curlx_dyn_addf(store, "%d", sval);

  /* represent larger values as hex */
  ঘূৰাই curlx_dyn_addf(store, "0x%x", uval);
}

/*
 * Convert from an ASN.1 typed string to UTF8.
 *
 * The result is stored in a dynbuf that is inited by the user of this
 * function.
 *
 * Returns error.
 */
স্থির CURLcode utf8asn1str(সংরচনা dynbuf *to, পূর্ণাঙ্ক type,
                            ধ্রুৱ uint8_t *from, ধ্রুৱ uint8_t *end)
{
  size_t inlength = end - from;
  পূর্ণাঙ্ক size = 1;
  CURLcode result = CURLE_OK;

  ৱিকল্প(type) {
  পক্ষ CURL_ASN1_BMP_STRING:
    size = 2;
    ভঙ্গ;
  পক্ষ CURL_ASN1_UNIVERSAL_STRING:
    size = 4;
    ভঙ্গ;
  পক্ষ CURL_ASN1_NUMERIC_STRING:
  পক্ষ CURL_ASN1_PRINTABLE_STRING:
  পক্ষ CURL_ASN1_TELETEX_STRING:
  পক্ষ CURL_ASN1_IA5_STRING:
  পক্ষ CURL_ASN1_VISIBLE_STRING:
  পক্ষ CURL_ASN1_UTF8_STRING:
    ভঙ্গ;
  স্ৱভাৱ:
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;  /* Conversion not supported. */
  }

  যদি(inlength % size)
    /* Length inconsistent with character size. */
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(type == CURL_ASN1_UTF8_STRING) {
    /* copy. */
    যদি(inlength)
      result = curlx_dyn_addn(to, from, inlength);
  }
  অন্যথা {
    যেতিয়ালৈ(!result && (from < end)) {
      ৱর্ণ buf[4]; /* decode buffer */
      size_t charsize = 1;
      unsigned পূর্ণাঙ্ক wc = 0;

      ৱিকল্প(size) {
      পক্ষ 4:
        wc = (wc << 8) | *from++;
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      পক্ষ 2:
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      স্ৱভাৱ: /* case 1: */
        wc = (wc << 8) | *from++;
      }
      যদি(wc >= 0x00000080) {
        যদি(wc >= 0x00000800) {
          যদি(wc >= 0x00010000) {
            যদি(wc >= 0x00200000) {
              /* Invalid char. size for target encoding. */
              ঘূৰাই CURLE_WEIRD_SERVER_REPLY;
            }
            buf[3] = (ৱর্ণ)(0x80 | (wc & 0x3F));
            wc = (wc >> 6) | 0x00010000;
            charsize++;
          }
          buf[2] = (ৱর্ণ)(0x80 | (wc & 0x3F));
          wc = (wc >> 6) | 0x00000800;
          charsize++;
        }
        buf[1] = (ৱর্ণ)(0x80 | (wc & 0x3F));
        wc = (wc >> 6) | 0x000000C0;
        charsize++;
      }
      buf[0] = (ৱর্ণ)wc;
      result = curlx_dyn_addn(to, buf, charsize);
    }
  }
  ঘূৰাই result;
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
UNITTEST CURLcode encodeOID(সংরচনা dynbuf *store,
                            ধ্রুৱ uint8_t *beg, ধ্রুৱ uint8_t *end);
UNITTEST CURLcode encodeOID(সংরচনা dynbuf *store,
                            ধ্রুৱ uint8_t *beg, ধ্রুৱ uint8_t *end)
{
  uint32_t x;
  uint32_t y;
  CURLcode result = CURLE_OK;

  যদি(end <= beg)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  /* Process the first two numbers. The initial digit cannot be larger than
     2 */
  y = *beg++;
  যদি(y <= 80)
    x = y / 40;
  অন্যথা
    x = 2;
  result = curlx_dyn_addf(store, "%u", x);
  যদি(result)
    ঘূৰাই result;

  যদি(y & 0x80) {
    uint32_t t = (y & 0x7f); /* the second number */
    যদি(!t)
      /* reject leading 0x80 padding */
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
    কৰা {
      যদি((t & 0xFE000000) || (beg >= end))
        ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      t = (t << 7) | (y & 0x7F);
    } যেতিয়ালৈ(y & 0x80);
    y = t;
  }

  result = curlx_dyn_addf(store, ".%u", y - (x * 40));
  যদি(result)
    ঘূৰাই result;

  /* Process the trailing numbers. */
  যেতিয়ালৈ((beg < end) && !result) {
    x = 0;
    y = 0;
    কৰা {
      /* 0x80 as the first value is invalid since it only adds padding */
      যদি((y & 0x80) && !x)
        ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
      অন্যথা যদি((x & 0xFE000000) || (beg == end))
        ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      x = (x << 7) | (y & 0x7F);
    } যেতিয়ালৈ(y & 0x80);
    result = curlx_dyn_addf(store, ".%u", x);
  }
  ঘূৰাই result;
}

/*
 * Convert an ASN.1 OID into its dotted or symbolic string representation.
 *
 * Return error code.
 */

স্থির CURLcode OID2str(সংরচনা dynbuf *store,
                        ধ্রুৱ uint8_t *beg, ধ্রুৱ uint8_t *end)
{
  CURLcode result = CURLE_OK;
  যদি(beg < end) {
    সংরচনা dynbuf buf;
    curlx_dyn_init(&buf, CURL_X509_STR_MAX);
    result = encodeOID(&buf, beg, end);

    যদি(!result) {
      ধ্রুৱ সংরচনা Curl_OID *op = অনুসন্ধান_OID(curlx_dyn_ptr(&buf));
      যদি(op)
        result = curlx_dyn_add(store, op->textoid);
      অন্যথা
        result = curlx_dyn_add(store, curlx_dyn_ptr(&buf));
    }
    curlx_dyn_free(&buf);
  }
  ঘূৰাই result;
}

/*
 * @unittest 1656
 */
UNITTEST CURLcode GTime2str(সংরচনা dynbuf *store,
                            ধ্রুৱ ৱর্ণ *beg, ধ্রুৱ ৱর্ণ *end);
UNITTEST CURLcode GTime2str(সংরচনা dynbuf *store,
                            ধ্রুৱ ৱর্ণ *beg, ধ্রুৱ ৱর্ণ *end)
{
  ধ্রুৱ ৱর্ণ *tzp;
  ধ্রুৱ ৱর্ণ *fracp;
  ৱর্ণ sec1, sec2;
  size_t fracl;
  size_t tzl;
  ধ্রুৱ ৱর্ণ *sep = "";

  /* Convert an ASN.1 Generalized time to a printable string.
     Return the dynamically allocated string, or NULL if an error occurs. */

  বাবে(fracp = beg; fracp < end && ISDIGIT(*fracp); fracp++)
    ;

  /* Get seconds digits. */
  sec1 = '0';
  ৱিকল্প(fracp - beg - 12) {
  পক্ষ 0:
    sec2 = '0';
    ভঙ্গ;
  পক্ষ 2:
    sec1 = fracp[-2];
    FALLTHROUGH();
  পক্ষ 1:
    sec2 = fracp[-1];
    ভঙ্গ;
  স্ৱভাৱ:
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* timezone follows optional fractional seconds. */
  tzp = fracp;
  fracl = 0; /* no fractional seconds detected so far */
  যদি(fracp < end && (*fracp == '.' || *fracp == ',')) {
    /* Have fractional seconds, e.g. "[.,]\d+". How many? */
    fracp++; /* should be a digit char or BAD ARGUMENT */
    tzp = fracp;
    যেতিয়ালৈ(tzp < end && ISDIGIT(*tzp))
      tzp++;
    যদি(tzp == fracp) /* never looped, no digit after [.,] */
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
    fracl = tzp - fracp; /* number of fractional sec digits */
    DEBUGASSERT(fracl > 0);
    /* Strip trailing zeroes in fractional seconds.
     * May reduce fracl to 0 if only '0's are present. */
    যেতিয়ালৈ(fracl && fracp[fracl - 1] == '0')
      fracl--;
  }

  /* Process timezone. */
  যদি(tzp >= end) {
    tzp = "";
    tzl = 0;
  }
  অন্যথা যদি(*tzp == 'Z') {
    sep = " ";
    tzp = "GMT";
    tzl = 3;
  }
  অন্যথা যদি((*tzp == '+') || (*tzp == '-')) {
    sep = " UTC";
    tzl = end - tzp;
  }
  অন্যথা {
    sep = " ";
    tzl = end - tzp;
  }

  ঘূৰাই curlx_dyn_addf(store,
                        "%.4s-%.2s-%.2s %.2s:%.2s:%c%c%s%.*s%s%.*s",
                        beg, beg + 4, beg + 6,
                        beg + 8, beg + 10, sec1, sec2,
                        fracl ? "." : "", (পূর্ণাঙ্ক)fracl, fracp,
                        sep, (পূর্ণাঙ্ক)tzl, tzp);
}

/*
 * Convert an ASN.1 UTC time to a printable string.
 *
 * Return error code.
 */
স্থির CURLcode UTime2str(সংরচনা dynbuf *store,
                          ধ্রুৱ ৱর্ণ *beg, ধ্রুৱ ৱর্ণ *end)
{
  ধ্রুৱ ৱর্ণ *tzp;
  size_t tzl;
  ধ্রুৱ ৱর্ণ *sec;

  বাবে(tzp = beg; tzp < end && *tzp >= '0' && *tzp <= '9'; tzp++)
    ;
  /* Get the seconds. */
  sec = beg + 10;
  ৱিকল্প(tzp - sec) {
  পক্ষ 0:
    sec = "00";
    FALLTHROUGH();
  পক্ষ 2:
    ভঙ্গ;
  স্ৱভাৱ:
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Process timezone. */
  যদি(tzp >= end)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  যদি(*tzp == 'Z') {
    tzp = "GMT";
    end = tzp + 3;
  }

  tzl = end - tzp;
  ঘূৰাই curlx_dyn_addf(store, "%d%.2s-%.2s-%.2s %.2s:%.2s:%.2s %.*s",
                        20 - (*beg >= '5'), beg, beg + 2, beg + 4,
                        beg + 6, beg + 8, sec,
                        (পূর্ণাঙ্ক)tzl, tzp);
}

/*
 * Convert an ASN.1 element to a printable string.
 *
 * Return error
 *
 * @unittest 1667
 */
UNITTEST CURLcode ASN1tostr(সংরচনা dynbuf *store,
                            সংরচনা Curl_asn1Element *elem);
UNITTEST CURLcode ASN1tostr(সংরচনা dynbuf *store,
                            সংরচনা Curl_asn1Element *elem)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  পূর্ণাঙ্ক type = elem->tag;
  যদি(elem->constructed)
    ঘূৰাই result; /* No conversion of structured elements. */

  ৱিকল্প(type) {
  পক্ষ CURL_ASN1_BOOLEAN:
    result = bool2str(store, elem->beg, elem->end);
    ভঙ্গ;
  পক্ষ CURL_ASN1_INTEGER:
  পক্ষ CURL_ASN1_ENUMERATED:
    result = int2str(store, elem->beg, elem->end);
    ভঙ্গ;
  পক্ষ CURL_ASN1_BIT_STRING:
    result = bit2str(store, elem->beg, elem->end);
    ভঙ্গ;
  পক্ষ CURL_ASN1_OCTET_STRING:
    result = octet2str(store, elem->beg, elem->end);
    ভঙ্গ;
  পক্ষ CURL_ASN1_NULL:
    result = curlx_dyn_addn(store, "", 1);
    ভঙ্গ;
  পক্ষ CURL_ASN1_OBJECT_IDENTIFIER:
    result = OID2str(store, elem->beg, elem->end);
    ভঙ্গ;
  পক্ষ CURL_ASN1_UTC_TIME:
    result = UTime2str(store, (ধ্রুৱ ৱর্ণ *)elem->beg,
                       (ধ্রুৱ ৱর্ণ *)elem->end);
    ভঙ্গ;
  পক্ষ CURL_ASN1_GENERALIZED_TIME:
    result = GTime2str(store, (ধ্রুৱ ৱর্ণ *)elem->beg,
                       (ধ্রুৱ ৱর্ণ *)elem->end);
    ভঙ্গ;
  পক্ষ CURL_ASN1_UTF8_STRING:
  পক্ষ CURL_ASN1_NUMERIC_STRING:
  পক্ষ CURL_ASN1_PRINTABLE_STRING:
  পক্ষ CURL_ASN1_TELETEX_STRING:
  পক্ষ CURL_ASN1_IA5_STRING:
  পক্ষ CURL_ASN1_VISIBLE_STRING:
  পক্ষ CURL_ASN1_UNIVERSAL_STRING:
  পক্ষ CURL_ASN1_BMP_STRING:
    result = utf8asn1str(store, type, elem->beg, elem->end);
    ভঙ্গ;
  }

  ঘূৰাই result;
}

/*
 * ASCII encode distinguished name at `dn' into the store dynbuf.
 *
 * Returns error.
 */
স্থির CURLcode encodeDN(সংরচনা dynbuf *store, সংরচনা Curl_asn1Element *dn)
{
  সংরচনা Curl_asn1Element rdn;
  সংরচনা Curl_asn1Element atv;
  সংরচনা Curl_asn1Element oid;
  সংরচনা Curl_asn1Element মান;
  ধ্রুৱ uint8_t *p1;
  ধ্রুৱ uint8_t *p2;
  ধ্রুৱ uint8_t *p3;
  ধ্রুৱ uint8_t *str;
  CURLcode result = CURLE_OK;
  bool added = FALSE;
  সংরচনা dynbuf temp;
  curlx_dyn_init(&temp, CURL_X509_STR_MAX);

  বাবে(p1 = dn->beg; p1 < dn->end;) {
    p1 = getASN1Element(&rdn, p1, dn->end);
    যদি(!p1) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      গচ্ছ error;
    }
    বাবে(p2 = rdn.beg; p2 < rdn.end;) {
      p2 = getASN1Element(&atv, p2, rdn.end);
      যদি(!p2) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        গচ্ছ error;
      }
      p3 = getASN1Element(&oid, atv.beg, atv.end);
      যদি(!p3) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        গচ্ছ error;
      }
      যদি(!getASN1Element(&মান, p3, atv.end)) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        গচ্ছ error;
      }
      curlx_dyn_reset(&temp);
      result = ASN1tostr(&temp, &oid);
      যদি(result)
        গচ্ছ error;

      str = curlx_dyn_uptr(&temp);

      যদি(!str) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        গচ্ছ error;
      }

      /* Encode delimiter.
         If attribute has a short uppercase name, delimiter is ", ". */
      বাবে(p3 = str; ISUPPER(*p3); p3++)
        ;
      যদি(added) {
        যদি(p3 - str > 2)
          result = curlx_dyn_addn(store, "/", 1);
        অন্যথা
          result = curlx_dyn_addn(store, ", ", 2);
        যদি(result)
          গচ্ছ error;
      }

      /* Encode attribute name. */
      result = curlx_dyn_add(store, (ধ্রুৱ ৱর্ণ *)str);
      যদি(result)
        গচ্ছ error;

      /* Generate equal sign. */
      result = curlx_dyn_addn(store, "=", 1);
      যদি(result)
        গচ্ছ error;

      /* Generate value. */
      result = ASN1tostr(store, &মান);
      যদি(result)
        গচ্ছ error;
      curlx_dyn_reset(&temp);
      added = TRUE; /* use separator for next */
    }
  }
error:
  curlx_dyn_free(&temp);

  ঘূৰাই result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#ifdef WANT_PARSEX509
/*
 * ASN.1 parse an X509 certificate into structure subfields.
 * Syntax is assumed to have already been checked by the SSL backend.
 * See RFC 5280.
 */
পূর্ণাঙ্ক Curl_parseX509(সংরচনা Curl_X509certificate *cert,
                   ধ্রুৱ uint8_t *beg, ধ্রুৱ uint8_t *end)
{
  সংরচনা Curl_asn1Element elem;
  সংরচনা Curl_asn1Element tbsCertificate;
  ধ্রুৱ uint8_t *ccp;
  স্থির ধ্রুৱ uint8_t defaultVersion = 0;  /* v1. */

  memset(cert, 0, পরিমাণ(*cert));
  cert->certificate.header = রিক্ত;
  cert->certificate.beg = beg;
  cert->certificate.end = end;

  /* Get the sequence content. */
  যদি(!getASN1Element(&elem, beg, end))
    ঘূৰাই -1;  /* Invalid bounds/size. */
  beg = elem.beg;
  end = elem.end;

  /* Get tbsCertificate. */
  beg = getASN1Element(&tbsCertificate, beg, end);
  যদি(!beg)
    ঘূৰাই -1;
  /* Skip the signatureAlgorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  যদি(!beg)
    ঘূৰাই -1;
  /* Get the signatureValue. */
  যদি(!getASN1Element(&cert->signature, beg, end))
    ঘূৰাই -1;

  /* Parse TBSCertificate. */
  beg = tbsCertificate.beg;
  end = tbsCertificate.end;
  /* Get optional version, get serialNumber. */
  cert->version.header = রিক্ত;
  cert->version.beg = &defaultVersion;
  cert->version.end = &defaultVersion + পরিমাণ(defaultVersion);
  beg = getASN1Element(&elem, beg, end);
  যদি(!beg)
    ঘূৰাই -1;
  যদি(elem.tag == 0) {
    যদি(!getASN1Element(&cert->version, elem.beg, elem.end))
      ঘূৰাই -1;
    beg = getASN1Element(&elem, beg, end);
    যদি(!beg)
      ঘূৰাই -1;
  }
  cert->serial_সংখ্যা = elem;
  /* Get signature algorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  /* Get issuer. */
  beg = getASN1Element(&cert->issuer, beg, end);
  যদি(!beg)
    ঘূৰাই -1;
  /* Get notBefore and notAfter. */
  beg = getASN1Element(&elem, beg, end);
  যদি(!beg)
    ঘূৰাই -1;
  ccp = getASN1Element(&cert->notBefore, elem.beg, elem.end);
  যদি(!ccp)
    ঘূৰাই -1;
  যদি(!getASN1Element(&cert->notAfter, ccp, elem.end))
    ঘূৰাই -1;
  /* These two fields MUST consume all of the validity elem */
  যদি(elem.end != cert->notAfter.end)
    ঘূৰাই -1;
  /* Get subject. */
  beg = getASN1Element(&cert->subject, beg, end);
  যদি(!beg)
    ঘূৰাই -1;
  /* Get subjectPublicKeyAlgorithm and subjectPublicKey. */
  beg = getASN1Element(&cert->subject_Public_চাবি_Info, beg, end);
  যদি(!beg)
    ঘূৰাই -1;
  ccp = getASN1Element(&cert->subject_Public_চাবি_Algorithm,
                       cert->subject_Public_চাবি_Info.beg,
                       cert->subject_Public_চাবি_Info.end);
  যদি(!ccp)
    ঘূৰাই -1;
  যদি(!getASN1Element(&cert->subject_Public_চাবি, ccp,
                     cert->subject_Public_চাবি_Info.end))
    ঘূৰাই -1;
  /* Get optional issuerUniqueID, subjectUniqueID and extensions. */
  cert->issuerUniqueID.tag = cert->subjectUniqueID.tag = 0;
  cert->extensions.tag = elem.tag = 0;
  cert->issuerUniqueID.header = cert->subjectUniqueID.header = রিক্ত;
  cert->issuerUniqueID.beg = cert->issuerUniqueID.end = beg;
  cert->subjectUniqueID.beg = cert->subjectUniqueID.end = beg;
  cert->extensions.header = রিক্ত;
  cert->extensions.beg = cert->extensions.end = beg;
  যদি(beg < end) {
    beg = getASN1Element(&elem, beg, end);
    যদি(!beg)
      ঘূৰাই -1;
  }
  যদি(elem.tag == 1) {
    cert->issuerUniqueID = elem;
    যদি(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      যদি(!beg)
        ঘূৰাই -1;
    }
  }
  যদি(elem.tag == 2) {
    cert->subjectUniqueID = elem;
    যদি(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      যদি(!beg)
        ঘূৰাই -1;
    }
  }
  যদি(elem.tag == 3)
    যদি(!getASN1Element(&cert->extensions, elem.beg, elem.end))
      ঘূৰাই -1;
  ঘূৰাই 0;
}

#endif /* WANT_PARSEX509 */

#ifdef WANT_EXTRACT_CERTINFO

স্থির CURLcode dumpAlgo(সংরচনা dynbuf *store,
                         সংরচনা Curl_asn1Element *param,
                         ধ্রুৱ uint8_t *beg, ধ্রুৱ uint8_t *end)
{
  সংরচনা Curl_asn1Element oid;

  /* Get algorithm parameters and return algorithm name. */

  beg = getASN1Element(&oid, beg, end);
  যদি(!beg)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  param->header = রিক্ত;
  param->tag = 0;
  param->beg = param->end = end;
  যদি(beg < end) {
    ধ্রুৱ uint8_t *p = getASN1Element(param, beg, end);
    যদি(!p)
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  }
  ঘূৰাই OID2str(store, oid.beg, oid.end);
}

/*
 * This is a convenience function for push_certinfo_len that takes a zero
 * terminated value.
 */
স্থির CURLcode ssl_ঠেলো_certinfo(সংরচনা Curl_easy *data,
                                  পূর্ণাঙ্ক certnum,
                                  ধ্রুৱ ৱর্ণ *label,
                                  ধ্রুৱ ৱর্ণ *মান)
{
  size_t valuelen = strlen(মান);

  ঘূৰাই Curl_ssl_ঠেলো_certinfo_len(data, certnum, label, মান, valuelen);
}

/*
 * This is a convenience function for push_certinfo_len that takes a
 * dynbuf value.
 *
 * It also does the verbose output if !certnum.
 */
স্থির CURLcode ssl_ঠেলো_certinfo_dyn(সংরচনা Curl_easy *data,
                                      পূর্ণাঙ্ক certnum,
                                      ধ্রুৱ ৱর্ণ *label,
                                      সংরচনা dynbuf *ptr)
{
  size_t valuelen = curlx_dyn_len(ptr);
  ধ্রুৱ ৱর্ণ *মান = curlx_dyn_ptr(ptr);

  CURLcode result = Curl_ssl_ঠেলো_certinfo_len(data, certnum, label,
                                               মান, valuelen);

  যদি(!certnum && !result)
    infof(data, "   %s: %s", label, মান);

  ঘূৰাই result;
}

স্থির CURLcode do_pubkey_field(সংরচনা Curl_easy *data, পূর্ণাঙ্ক certnum,
                                ধ্রুৱ ৱর্ণ *label,
                                সংরচনা Curl_asn1Element *elem)
{
  CURLcode result;
  সংরচনা dynbuf out;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);

  /* Generate a certificate information record for the public key. */

  result = ASN1tostr(&out, elem);
  যদি(!result && data->set.ssl.certinfo)
    result = ssl_ঠেলো_certinfo_dyn(data, certnum, label, &out);
  curlx_dyn_free(&out);
  ঘূৰাই result;
}

/* return 0 on success, 1 on error */
স্থির পূর্ণাঙ্ক do_pubkey(সংরচনা Curl_easy *data, পূর্ণাঙ্ক certnum, ধ্রুৱ ৱর্ণ *algo,
                     সংরচনা Curl_asn1Element *param,
                     সংরচনা Curl_asn1Element *pubkey)
{
  সংরচনা Curl_asn1Element elem;
  সংরচনা Curl_asn1Element pk;
  ধ্রুৱ uint8_t *p;

  /* Generate all information records for the public key. */

  যদি(curl_strequal(algo, "ecPublicKey")) {
    /*
     * ECC public key is all the data, a value of type BIT STRING mapped to
     * OCTET STRING and should not be parsed as an ASN.1 value.
     */
    ধ্রুৱ size_t dlen = pubkey->end - pubkey->beg;
    size_t len;
    যদি(dlen < 2)
      /* too small */
      ঘূৰাই 1;
    len = (dlen - 2) * 4;
    যদি(!certnum)
      infof(data, "   ECC Public Key (%zu bits)", len);
    যদি(data->set.ssl.certinfo) {
      ৱর্ণ q[(পরিমাণ(len) * 8 / 3) + 1];
      (শূন্য)curl_msnprintf(q, পরিমাণ(q), "%zu", len);
      যদি(ssl_ঠেলো_certinfo(data, certnum, "ECC Public Key", q))
        ঘূৰাই 1;
    }
    ঘূৰাই do_pubkey_field(data, certnum, "ecPublicKey", pubkey) == CURLE_OK
      ? 0 : 1;
  }

  /* Get the public key (single element). */
  যদি(!getASN1Element(&pk, pubkey->beg + 1, pubkey->end))
    ঘূৰাই 1;

  যদি(curl_strequal(algo, "rsaEncryption")) {
    ধ্রুৱ uint8_t *q;
    size_t len;

    p = getASN1Element(&elem, pk.beg, pk.end);
    যদি(!p)
      ঘূৰাই 1;

    /* Compute key length. */
    বাবে(q = elem.beg; q < elem.end && !*q; q++)
      ;
    len = ((elem.end - q) * 8);
    যদি(len) {
      unsigned পূর্ণাঙ্ক i;
      বাবে(i = *q; !(i & 0x80); i <<= 1)
        len--;
    }
    যদি(len > 32)
      elem.beg = q;     /* Strip leading zero bytes. */
    যদি(!certnum)
      infof(data, "   RSA Public Key (%zu bits)", len);
    যদি(data->set.ssl.certinfo) {
      ৱর্ণ r[(পরিমাণ(len) * 8 / 3) + 1];
      curl_msnprintf(r, পরিমাণ(r), "%zu", len);
      যদি(ssl_ঠেলো_certinfo(data, certnum, "RSA Public Key", r))
        ঘূৰাই 1;
    }
    /* Generate coefficients. */
    যদি(do_pubkey_field(data, certnum, "rsa(n)", &elem))
      ঘূৰাই 1;
    যদি(!getASN1Element(&elem, p, pk.end))
      ঘূৰাই 1;
    যদি(do_pubkey_field(data, certnum, "rsa(e)", &elem))
      ঘূৰাই 1;
  }
  অন্যথা যদি(curl_strequal(algo, "dsa")) {
    p = getASN1Element(&elem, param->beg, param->end);
    যদি(p) {
      যদি(do_pubkey_field(data, certnum, "dsa(p)", &elem))
        ঘূৰাই 1;
      p = getASN1Element(&elem, p, param->end);
      যদি(p) {
        যদি(do_pubkey_field(data, certnum, "dsa(q)", &elem))
          ঘূৰাই 1;
        যদি(getASN1Element(&elem, p, param->end)) {
          যদি(do_pubkey_field(data, certnum, "dsa(g)", &elem))
            ঘূৰাই 1;
          যদি(do_pubkey_field(data, certnum, "dsa(pub_key)", &pk))
            ঘূৰাই 1;
        }
      }
    }
  }
  অন্যথা যদি(curl_strequal(algo, "dhpublicnumber")) {
    p = getASN1Element(&elem, param->beg, param->end);
    যদি(p) {
      যদি(do_pubkey_field(data, certnum, "dh(p)", &elem))
        ঘূৰাই 1;
      যদি(getASN1Element(&elem, p, param->end)) {
        যদি(do_pubkey_field(data, certnum, "dh(g)", &elem))
          ঘূৰাই 1;
        যদি(do_pubkey_field(data, certnum, "dh(pub_key)", &pk))
          ঘূৰাই 1;
      }
    }
  }
  ঘূৰাই 0;
}

/*
 * Convert an ASN.1 distinguished name into a printable string.
 * Return error.
 */
স্থির CURLcode DNtostr(সংরচনা dynbuf *store, সংরচনা Curl_asn1Element *dn)
{
  ঘূৰাই encodeDN(store, dn);
}

CURLcode Curl_extract_certinfo(সংরচনা Curl_easy *data,
                               পূর্ণাঙ্ক certnum,
                               ধ্রুৱ uint8_t *beg,
                               ধ্রুৱ uint8_t *end)
{
  সংরচনা Curl_X509certificate cert;
  সংরচনা Curl_asn1Element param;
  ৱর্ণ *certptr;
  size_t clen;
  সংরচনা dynbuf out;
  CURLcode result = CURLE_OK;
  unsigned পূর্ণাঙ্ক version;
  ধ্রুৱ uint8_t *ptr;
  পূর্ণাঙ্ক rc;

  যদি(!data->set.ssl.certinfo)
    যদি(certnum)
      ঘূৰাই CURLE_OK;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);
  /* Prepare the certificate information for curl_easy_getinfo(). */

  /* Extract the certificate ASN.1 elements. */
  যদি(Curl_parseX509(&cert, beg, end))
    ঘূৰাই CURLE_PEER_FAILED_VERIFICATION;

  /* Subject. */
  result = DNtostr(&out, &cert.subject);
  যদি(result)
    গচ্ছ done;
  যদি(data->set.ssl.certinfo) {
    result = ssl_ঠেলো_certinfo_dyn(data, certnum, "Subject", &out);
    যদি(result)
      গচ্ছ done;
  }
  curlx_dyn_reset(&out);

  /* Issuer. */
  result = DNtostr(&out, &cert.issuer);
  যদি(result)
    গচ্ছ done;
  যদি(data->set.ssl.certinfo) {
    result = ssl_ঠেলো_certinfo_dyn(data, certnum, "Issuer", &out);
    যদি(result)
      গচ্ছ done;
  }
  curlx_dyn_reset(&out);

  /* Version (always fits in less than 32 bits). */
  version = 0;
  বাবে(ptr = cert.version.beg; ptr < cert.version.end; ptr++)
    version = (version << 8) | *ptr;
  যদি(data->set.ssl.certinfo) {
    result = curlx_dyn_addf(&out, "%x", version);
    যদি(result)
      গচ্ছ done;
    result = ssl_ঠেলো_certinfo_dyn(data, certnum, "Version", &out);
    যদি(result)
      গচ্ছ done;
    curlx_dyn_reset(&out);
  }

  /* Serial number. */
  result = ASN1tostr(&out, &cert.serial_সংখ্যা);
  যদি(result)
    গচ্ছ done;
  যদি(data->set.ssl.certinfo) {
    result = ssl_ঠেলো_certinfo_dyn(data, certnum, "Serial Number", &out);
    যদি(result)
      গচ্ছ done;
  }
  curlx_dyn_reset(&out);

  /* Signature algorithm .*/
  result = dumpAlgo(&out, &param, cert.signatureAlgorithm.beg,
                    cert.signatureAlgorithm.end);
  যদি(result)
    গচ্ছ done;
  যদি(data->set.ssl.certinfo) {
    result = ssl_ঠেলো_certinfo_dyn(data, certnum, "Signature Algorithm", &out);
    যদি(result)
      গচ্ছ done;
  }
  curlx_dyn_reset(&out);

  /* Start Date. */
  result = ASN1tostr(&out, &cert.notBefore);
  যদি(result)
    গচ্ছ done;
  যদি(data->set.ssl.certinfo) {
    result = ssl_ঠেলো_certinfo_dyn(data, certnum, "Start Date", &out);
    যদি(result)
      গচ্ছ done;
  }
  curlx_dyn_reset(&out);

  /* Expire Date. */
  result = ASN1tostr(&out, &cert.notAfter);
  যদি(result)
    গচ্ছ done;
  যদি(data->set.ssl.certinfo) {
    result = ssl_ঠেলো_certinfo_dyn(data, certnum, "Expire Date", &out);
    যদি(result)
      গচ্ছ done;
  }
  curlx_dyn_reset(&out);

  /* Public Key Algorithm. */
  result = dumpAlgo(&out, &param, cert.subject_Public_চাবি_Algorithm.beg,
                    cert.subject_Public_চাবি_Algorithm.end);
  যদি(result)
    গচ্ছ done;
  যদি(data->set.ssl.certinfo) {
    result = ssl_ঠেলো_certinfo_dyn(data, certnum, "Public Key Algorithm",
                                   &out);
    যদি(result)
      গচ্ছ done;
  }

  rc = do_pubkey(data, certnum, curlx_dyn_ptr(&out),
                 &param, &cert.subject_Public_চাবি);
  যদি(rc) {
    result = CURLE_OUT_OF_MEMORY; /* the most likely error */
    গচ্ছ done;
  }
  curlx_dyn_reset(&out);

  /* Signature. */
  result = ASN1tostr(&out, &cert.signature);
  যদি(result)
    গচ্ছ done;
  যদি(data->set.ssl.certinfo) {
    result = ssl_ঠেলো_certinfo_dyn(data, certnum, "Signature", &out);
    যদি(result)
      গচ্ছ done;
  }
  curlx_dyn_reset(&out);

  /* Generate PEM certificate. */
  result = curlx_base64_encode((ধ্রুৱ uint8_t *)cert.certificate.beg,
                               cert.certificate.end - cert.certificate.beg,
                               &certptr, &clen);
  যদি(result)
    গচ্ছ done;

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
  যদি(!result) {
    size_t j = 0;

    যেতিয়ালৈ(!result && (j < clen)) {
      size_t chunksize = (clen - j) > 64 ? 64 : (clen - j);
      result = curlx_dyn_addn(&out, &certptr[j], chunksize);
      যদি(!result)
        result = curlx_dyn_addn(&out, "\n", 1);
      j += chunksize;
    }
    যদি(!result)
      result = curlx_dyn_add(&out, "-----END CERTIFICATE-----\n");
  }
  curlx_free(certptr);
  যদি(!result)
    যদি(data->set.ssl.certinfo)
      result = ssl_ঠেলো_certinfo_dyn(data, certnum, "Cert", &out);

done:
  যদি(result)
    failf(data, "Failed extracting certificate chain");
  curlx_dyn_free(&out);
  ঘূৰাই result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#endif /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
