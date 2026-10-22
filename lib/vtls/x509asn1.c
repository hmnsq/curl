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
#অন্তর্ভাব "curl_setup.h"

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

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "curlx/base64.h"
#অন্তর্ভাব "vtls/x509asn1.h"
#অন্তর্ভাব "curlx/dynbuf.h"

/*
 * Constants.
 */

/* Largest supported ASN.1 structure. */
#define CURL_ASN1_MAX                   ((size_t)0x40000)      /* 256K */

#ifdef WANT_EXTRACT_CERTINFO

/* ASN.1 OID table entry. */
কাঠামো Curl_OID {
  ধ্রুবক অক্ষর *numoid;  /* Dotted-numeric OID. */
  ধ্রুবক অক্ষর *textoid; /* OID name. */
};

/* ASN.1 OIDs. */
স্থির ধ্রুবক কাঠামো Curl_OID OIDtable[] = {
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
  { ফাঁকা,                       ফাঁকা }
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

স্থির ধ্রুবক uint8_t *getASN1Element_(কাঠামো Curl_asn1Element *elem,
                                      ধ্রুবক uint8_t *beg, ধ্রুবক uint8_t *end,
                                      size_t lvl)
{
  uint8_t b;
  size_t len;
  কাঠামো Curl_asn1Element lelem;

  /* Get a single ASN.1 element into `elem', parse ASN.1 string at `beg'
     ending at `end'.
     Returns a pointer in source string after the parsed element, or NULL
     if an error occurs. */
  যদি(!beg || !end || beg >= end || !*beg ||
     ((size_t)(end - beg) > CURL_ASN1_MAX) ||
     lvl++ >= CURL_ASN1_MAX_RECURSIONS)
    ফেরত ফাঁকা;

  /* Process header byte. */
  elem->header = beg;
  b = *beg++;
  elem->constructed = (b & 0x20) != 0;
  elem->eclass = (b >> 6) & 3;
  b &= 0x1F;
  যদি(b == 0x1F)
    ফেরত ফাঁকা; /* Long tag values not supported here. */
  elem->tag = b;

  /* Process length. */
  যদি(beg >= end)
    ফেরত ফাঁকা;
  b = *beg++;
  যদি(!(b & 0x80))
    len = b;
  অন্যথা {
    b &= 0x7F;
    যদি(!b) {
      /* Unspecified length. Since we have all the data, we can determine the
         effective length by skipping element until an EOC element is found. */
      যদি(!elem->constructed)
        ফেরত ফাঁকা;
      elem->beg = beg;
      যখন(beg < end && *beg) {
        beg = getASN1Element_(&lelem, beg, end, lvl);
        যদি(!beg)
          ফেরত ফাঁকা;
      }
      যদি(end - beg < 2 || beg[1])       /* EOC must have a zero length. */
        ফেরত ফাঁকা;
      elem->end = beg;                  /* Do not include EOC as content. */
      ফেরত beg + 2;                   /* After EOC element. */
    }
    অন্যথা যদি(b > (size_t)(end - beg))
      ফেরত ফাঁকা; /* Does not fit in source. */
    অন্যথা {
      /* Get long length. */
      len = 0;
      করো {
        যদি(len & 0xFF000000L)
          ফেরত ফাঁকা;  /* Lengths > 32 bits are not supported. */
        len = (len << 8) | *beg++;
      } যখন(--b);
    }
  }
  যদি(len > (size_t)(end - beg))
    ফেরত ফাঁকা;  /* Element data does not fit in source. */
  elem->beg = beg;
  elem->end = beg + len;
  ফেরত elem->end;
}

/*
 * @unittest 1657
 */
UNITTEST ধ্রুবক uint8_t *getASN1Element(কাঠামো Curl_asn1Element *elem,
                                       ধ্রুবক uint8_t *beg, ধ্রুবক uint8_t *end);
UNITTEST ধ্রুবক uint8_t *getASN1Element(কাঠামো Curl_asn1Element *elem,
                                       ধ্রুবক uint8_t *beg, ধ্রুবক uint8_t *end)
{
  ফেরত getASN1Element_(elem, beg, end, 0);
}

#ifdef WANT_EXTRACT_CERTINFO

/*
 * Search the null-terminated OID or OID identifier in local table.
 * Return the table entry pointer or NULL if not found.
 */
স্থির ধ্রুবক কাঠামো Curl_OID *অনুসন্ধান_OID(ধ্রুবক অক্ষর *oid)
{
  ধ্রুবক কাঠামো Curl_OID *op;
  জন্য(op = OIDtable; op->numoid; op++)
    যদি(!strcmp(op->numoid, oid) || curl_strequal(op->textoid, oid))
      ফেরত op;

  ফেরত ফাঁকা;
}

/*
 * Convert an ASN.1 Boolean value into its string representation.
 *
 * Return error code.
 */

স্থির CURLcode bool2str(কাঠামো dynbuf *store,
                         ধ্রুবক uint8_t *beg, ধ্রুবক uint8_t *end)
{
  uint8_t val;
  যদি(end - beg != 1)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  val = *beg;
  যদি((val != 0xff) && (val != 0x00))
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  ফেরত curlx_dyn_add(store, val ? "TRUE" : "FALSE");
}

/*
 * Convert an ASN.1 octet string to a printable string.
 *
 * Return error code.
 */
স্থির CURLcode octet2str(কাঠামো dynbuf *store,
                          ধ্রুবক uint8_t *beg, ধ্রুবক uint8_t *end)
{
  CURLcode ফলাফল = CURLE_OK;

  যখন(!ফলাফল && beg < end)
    ফলাফল = curlx_dyn_addf(store, "%02x:", *beg++);

  ফেরত ফলাফল;
}

স্থির CURLcode bit2str(কাঠামো dynbuf *store,
                        ধ্রুবক uint8_t *beg, ধ্রুবক uint8_t *end)
{
  /* Convert an ASN.1 bit string to a printable string. */

  যদি(beg < end) {
    uint8_t unused_bits = *beg++;
    যদি(unused_bits > 7)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }
  অন্যথা
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  ফেরত octet2str(store, beg, end);
}

/*
 * Convert an ASN.1 integer value into its string representation.
 *
 * Returns error.
 */
স্থির CURLcode int2str(কাঠামো dynbuf *store,
                        ধ্রুবক uint8_t *beg, ধ্রুবক uint8_t *end)
{
  uint32_t uval = 0;
  int32_t sval = 0;
  size_t n = end - beg;
  size_t i;
  ধ্রুবক uint8_t *p = beg;

  যদি(!n)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(n > 4)
    ফেরত octet2str(store, beg, end);

  যদি(p[0] & 0x80)
    uval = ~0U;

  জন্য(i = 0; i < n; i++)
    uval = (uval << 8) | p[i];

  sval = (int32_t)uval;

  /* small values are easier to read as decimals */
  যদি(sval > -10000 && sval < 10000)
    ফেরত curlx_dyn_addf(store, "%d", sval);

  /* represent larger values as hex */
  ফেরত curlx_dyn_addf(store, "0x%x", uval);
}

/*
 * Convert from an ASN.1 typed string to UTF8.
 *
 * The result is stored in a dynbuf that is inited by the user of this
 * function.
 *
 * Returns error.
 */
স্থির CURLcode utf8asn1str(কাঠামো dynbuf *to, পূর্ণসংখ্যা type,
                            ধ্রুবক uint8_t *from, ধ্রুবক uint8_t *end)
{
  size_t inlength = end - from;
  পূর্ণসংখ্যা আকার_2 = 1;
  CURLcode ফলাফল = CURLE_OK;

  নির্বাচন(type) {
  ক্ষেত্র CURL_ASN1_BMP_STRING:
    আকার_2 = 2;
    বিরতি;
  ক্ষেত্র CURL_ASN1_UNIVERSAL_STRING:
    আকার_2 = 4;
    বিরতি;
  ক্ষেত্র CURL_ASN1_NUMERIC_STRING:
  ক্ষেত্র CURL_ASN1_PRINTABLE_STRING:
  ক্ষেত্র CURL_ASN1_TELETEX_STRING:
  ক্ষেত্র CURL_ASN1_IA5_STRING:
  ক্ষেত্র CURL_ASN1_VISIBLE_STRING:
  ক্ষেত্র CURL_ASN1_UTF8_STRING:
    বিরতি;
  পূর্বনির্ধারিত:
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;  /* Conversion not supported. */
  }

  যদি(inlength % আকার_2)
    /* Length inconsistent with character size. */
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(type == CURL_ASN1_UTF8_STRING) {
    /* copy. */
    যদি(inlength)
      ফলাফল = curlx_dyn_addn(to, from, inlength);
  }
  অন্যথা {
    যখন(!ফলাফল && (from < end)) {
      অক্ষর buf[4]; /* decode buffer */
      size_t charsize = 1;
      unsigned পূর্ণসংখ্যা wc = 0;

      নির্বাচন(আকার_2) {
      ক্ষেত্র 4:
        wc = (wc << 8) | *from++;
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      ক্ষেত্র 2:
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      পূর্বনির্ধারিত: /* case 1: */
        wc = (wc << 8) | *from++;
      }
      যদি(wc >= 0x00000080) {
        যদি(wc >= 0x00000800) {
          যদি(wc >= 0x00010000) {
            যদি(wc >= 0x00200000) {
              /* Invalid char. size for target encoding. */
              ফেরত CURLE_WEIRD_SERVER_REPLY;
            }
            buf[3] = (অক্ষর)(0x80 | (wc & 0x3F));
            wc = (wc >> 6) | 0x00010000;
            charsize++;
          }
          buf[2] = (অক্ষর)(0x80 | (wc & 0x3F));
          wc = (wc >> 6) | 0x00000800;
          charsize++;
        }
        buf[1] = (অক্ষর)(0x80 | (wc & 0x3F));
        wc = (wc >> 6) | 0x000000C0;
        charsize++;
      }
      buf[0] = (অক্ষর)wc;
      ফলাফল = curlx_dyn_addn(to, buf, charsize);
    }
  }
  ফেরত ফলাফল;
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
UNITTEST CURLcode encodeOID(কাঠামো dynbuf *store,
                            ধ্রুবক uint8_t *beg, ধ্রুবক uint8_t *end);
UNITTEST CURLcode encodeOID(কাঠামো dynbuf *store,
                            ধ্রুবক uint8_t *beg, ধ্রুবক uint8_t *end)
{
  uint32_t x;
  uint32_t y;
  CURLcode ফলাফল = CURLE_OK;

  যদি(end <= beg)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  /* Process the first two numbers. The initial digit cannot be larger than
     2 */
  y = *beg++;
  যদি(y <= 80)
    x = y / 40;
  অন্যথা
    x = 2;
  ফলাফল = curlx_dyn_addf(store, "%u", x);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  যদি(y & 0x80) {
    uint32_t t = (y & 0x7f); /* the second number */
    যদি(!t)
      /* reject leading 0x80 padding */
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    করো {
      যদি((t & 0xFE000000) || (beg >= end))
        ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      t = (t << 7) | (y & 0x7F);
    } যখন(y & 0x80);
    y = t;
  }

  ফলাফল = curlx_dyn_addf(store, ".%u", y - (x * 40));
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* Process the trailing numbers. */
  যখন((beg < end) && !ফলাফল) {
    x = 0;
    y = 0;
    করো {
      /* 0x80 as the first value is invalid since it only adds padding */
      যদি((y & 0x80) && !x)
        ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
      অন্যথা যদি((x & 0xFE000000) || (beg == end))
        ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      x = (x << 7) | (y & 0x7F);
    } যখন(y & 0x80);
    ফলাফল = curlx_dyn_addf(store, ".%u", x);
  }
  ফেরত ফলাফল;
}

/*
 * Convert an ASN.1 OID into its dotted or symbolic string representation.
 *
 * Return error code.
 */

স্থির CURLcode OID2str(কাঠামো dynbuf *store,
                        ধ্রুবক uint8_t *beg, ধ্রুবক uint8_t *end)
{
  CURLcode ফলাফল = CURLE_OK;
  যদি(beg < end) {
    কাঠামো dynbuf buf;
    curlx_dyn_init(&buf, CURL_X509_STR_MAX);
    ফলাফল = encodeOID(&buf, beg, end);

    যদি(!ফলাফল) {
      ধ্রুবক কাঠামো Curl_OID *op = অনুসন্ধান_OID(curlx_dyn_ptr(&buf));
      যদি(op)
        ফলাফল = curlx_dyn_add(store, op->textoid);
      অন্যথা
        ফলাফল = curlx_dyn_add(store, curlx_dyn_ptr(&buf));
    }
    curlx_dyn_free(&buf);
  }
  ফেরত ফলাফল;
}

/*
 * @unittest 1656
 */
UNITTEST CURLcode GTime2str(কাঠামো dynbuf *store,
                            ধ্রুবক অক্ষর *beg, ধ্রুবক অক্ষর *end);
UNITTEST CURLcode GTime2str(কাঠামো dynbuf *store,
                            ধ্রুবক অক্ষর *beg, ধ্রুবক অক্ষর *end)
{
  ধ্রুবক অক্ষর *tzp;
  ধ্রুবক অক্ষর *fracp;
  অক্ষর sec1, sec2;
  size_t fracl;
  size_t tzl;
  ধ্রুবক অক্ষর *sep = "";

  /* Convert an ASN.1 Generalized time to a printable string.
     Return the dynamically allocated string, or NULL if an error occurs. */

  জন্য(fracp = beg; fracp < end && ISDIGIT(*fracp); fracp++)
    ;

  /* Get seconds digits. */
  sec1 = '0';
  নির্বাচন(fracp - beg - 12) {
  ক্ষেত্র 0:
    sec2 = '0';
    বিরতি;
  ক্ষেত্র 2:
    sec1 = fracp[-2];
    FALLTHROUGH();
  ক্ষেত্র 1:
    sec2 = fracp[-1];
    বিরতি;
  পূর্বনির্ধারিত:
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* timezone follows optional fractional seconds. */
  tzp = fracp;
  fracl = 0; /* no fractional seconds detected so far */
  যদি(fracp < end && (*fracp == '.' || *fracp == ',')) {
    /* Have fractional seconds, e.g. "[.,]\d+". How many? */
    fracp++; /* should be a digit char or BAD ARGUMENT */
    tzp = fracp;
    যখন(tzp < end && ISDIGIT(*tzp))
      tzp++;
    যদি(tzp == fracp) /* never looped, no digit after [.,] */
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    fracl = tzp - fracp; /* number of fractional sec digits */
    DEBUGASSERT(fracl > 0);
    /* Strip trailing zeroes in fractional seconds.
     * May reduce fracl to 0 if only '0's are present. */
    যখন(fracl && fracp[fracl - 1] == '0')
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

  ফেরত curlx_dyn_addf(store,
                        "%.4s-%.2s-%.2s %.2s:%.2s:%c%c%s%.*s%s%.*s",
                        beg, beg + 4, beg + 6,
                        beg + 8, beg + 10, sec1, sec2,
                        fracl ? "." : "", (পূর্ণসংখ্যা)fracl, fracp,
                        sep, (পূর্ণসংখ্যা)tzl, tzp);
}

/*
 * Convert an ASN.1 UTC time to a printable string.
 *
 * Return error code.
 */
স্থির CURLcode UTime2str(কাঠামো dynbuf *store,
                          ধ্রুবক অক্ষর *beg, ধ্রুবক অক্ষর *end)
{
  ধ্রুবক অক্ষর *tzp;
  size_t tzl;
  ধ্রুবক অক্ষর *sec;

  জন্য(tzp = beg; tzp < end && *tzp >= '0' && *tzp <= '9'; tzp++)
    ;
  /* Get the seconds. */
  sec = beg + 10;
  নির্বাচন(tzp - sec) {
  ক্ষেত্র 0:
    sec = "00";
    FALLTHROUGH();
  ক্ষেত্র 2:
    বিরতি;
  পূর্বনির্ধারিত:
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Process timezone. */
  যদি(tzp >= end)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  যদি(*tzp == 'Z') {
    tzp = "GMT";
    end = tzp + 3;
  }

  tzl = end - tzp;
  ফেরত curlx_dyn_addf(store, "%d%.2s-%.2s-%.2s %.2s:%.2s:%.2s %.*s",
                        20 - (*beg >= '5'), beg, beg + 2, beg + 4,
                        beg + 6, beg + 8, sec,
                        (পূর্ণসংখ্যা)tzl, tzp);
}

/*
 * Convert an ASN.1 element to a printable string.
 *
 * Return error
 *
 * @unittest 1667
 */
UNITTEST CURLcode ASN1tostr(কাঠামো dynbuf *store,
                            কাঠামো Curl_asn1Element *elem);
UNITTEST CURLcode ASN1tostr(কাঠামো dynbuf *store,
                            কাঠামো Curl_asn1Element *elem)
{
  CURLcode ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
  পূর্ণসংখ্যা type = elem->tag;
  যদি(elem->constructed)
    ফেরত ফলাফল; /* No conversion of structured elements. */

  নির্বাচন(type) {
  ক্ষেত্র CURL_ASN1_BOOLEAN:
    ফলাফল = bool2str(store, elem->beg, elem->end);
    বিরতি;
  ক্ষেত্র CURL_ASN1_INTEGER:
  ক্ষেত্র CURL_ASN1_ENUMERATED:
    ফলাফল = int2str(store, elem->beg, elem->end);
    বিরতি;
  ক্ষেত্র CURL_ASN1_BIT_STRING:
    ফলাফল = bit2str(store, elem->beg, elem->end);
    বিরতি;
  ক্ষেত্র CURL_ASN1_OCTET_STRING:
    ফলাফল = octet2str(store, elem->beg, elem->end);
    বিরতি;
  ক্ষেত্র CURL_ASN1_NULL:
    ফলাফল = curlx_dyn_addn(store, "", 1);
    বিরতি;
  ক্ষেত্র CURL_ASN1_OBJECT_IDENTIFIER:
    ফলাফল = OID2str(store, elem->beg, elem->end);
    বিরতি;
  ক্ষেত্র CURL_ASN1_UTC_TIME:
    ফলাফল = UTime2str(store, (ধ্রুবক অক্ষর *)elem->beg,
                       (ধ্রুবক অক্ষর *)elem->end);
    বিরতি;
  ক্ষেত্র CURL_ASN1_GENERALIZED_TIME:
    ফলাফল = GTime2str(store, (ধ্রুবক অক্ষর *)elem->beg,
                       (ধ্রুবক অক্ষর *)elem->end);
    বিরতি;
  ক্ষেত্র CURL_ASN1_UTF8_STRING:
  ক্ষেত্র CURL_ASN1_NUMERIC_STRING:
  ক্ষেত্র CURL_ASN1_PRINTABLE_STRING:
  ক্ষেত্র CURL_ASN1_TELETEX_STRING:
  ক্ষেত্র CURL_ASN1_IA5_STRING:
  ক্ষেত্র CURL_ASN1_VISIBLE_STRING:
  ক্ষেত্র CURL_ASN1_UNIVERSAL_STRING:
  ক্ষেত্র CURL_ASN1_BMP_STRING:
    ফলাফল = utf8asn1str(store, type, elem->beg, elem->end);
    বিরতি;
  }

  ফেরত ফলাফল;
}

/*
 * ASCII encode distinguished name at `dn' into the store dynbuf.
 *
 * Returns error.
 */
স্থির CURLcode encodeDN(কাঠামো dynbuf *store, কাঠামো Curl_asn1Element *dn)
{
  কাঠামো Curl_asn1Element rdn;
  কাঠামো Curl_asn1Element atv;
  কাঠামো Curl_asn1Element oid;
  কাঠামো Curl_asn1Element মান;
  ধ্রুবক uint8_t *p1;
  ধ্রুবক uint8_t *p2;
  ধ্রুবক uint8_t *p3;
  ধ্রুবক uint8_t *str;
  CURLcode ফলাফল = CURLE_OK;
  bool added = FALSE;
  কাঠামো dynbuf অস্থায়ী;
  curlx_dyn_init(&অস্থায়ী, CURL_X509_STR_MAX);

  জন্য(p1 = dn->beg; p1 < dn->end;) {
    p1 = getASN1Element(&rdn, p1, dn->end);
    যদি(!p1) {
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
      যাও error;
    }
    জন্য(p2 = rdn.beg; p2 < rdn.end;) {
      p2 = getASN1Element(&atv, p2, rdn.end);
      যদি(!p2) {
        ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
        যাও error;
      }
      p3 = getASN1Element(&oid, atv.beg, atv.end);
      যদি(!p3) {
        ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
        যাও error;
      }
      যদি(!getASN1Element(&মান, p3, atv.end)) {
        ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
        যাও error;
      }
      curlx_dyn_reset(&অস্থায়ী);
      ফলাফল = ASN1tostr(&অস্থায়ী, &oid);
      যদি(ফলাফল)
        যাও error;

      str = curlx_dyn_uptr(&অস্থায়ী);

      যদি(!str) {
        ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
        যাও error;
      }

      /* Encode delimiter.
         If attribute has a short uppercase name, delimiter is ", ". */
      জন্য(p3 = str; ISUPPER(*p3); p3++)
        ;
      যদি(added) {
        যদি(p3 - str > 2)
          ফলাফল = curlx_dyn_addn(store, "/", 1);
        অন্যথা
          ফলাফল = curlx_dyn_addn(store, ", ", 2);
        যদি(ফলাফল)
          যাও error;
      }

      /* Encode attribute name. */
      ফলাফল = curlx_dyn_add(store, (ধ্রুবক অক্ষর *)str);
      যদি(ফলাফল)
        যাও error;

      /* Generate equal sign. */
      ফলাফল = curlx_dyn_addn(store, "=", 1);
      যদি(ফলাফল)
        যাও error;

      /* Generate value. */
      ফলাফল = ASN1tostr(store, &মান);
      যদি(ফলাফল)
        যাও error;
      curlx_dyn_reset(&অস্থায়ী);
      added = TRUE; /* use separator for next */
    }
  }
error:
  curlx_dyn_free(&অস্থায়ী);

  ফেরত ফলাফল;
}

#endif /* WANT_EXTRACT_CERTINFO */

#ifdef WANT_PARSEX509
/*
 * ASN.1 parse an X509 certificate into structure subfields.
 * Syntax is assumed to have already been checked by the SSL backend.
 * See RFC 5280.
 */
পূর্ণসংখ্যা Curl_parseX509(কাঠামো Curl_X509certificate *cert,
                   ধ্রুবক uint8_t *beg, ধ্রুবক uint8_t *end)
{
  কাঠামো Curl_asn1Element elem;
  কাঠামো Curl_asn1Element tbsCertificate;
  ধ্রুবক uint8_t *ccp;
  স্থির ধ্রুবক uint8_t defaultVersion = 0;  /* v1. */

  memset(cert, 0, আকার(*cert));
  cert->certificate.header = ফাঁকা;
  cert->certificate.beg = beg;
  cert->certificate.end = end;

  /* Get the sequence content. */
  যদি(!getASN1Element(&elem, beg, end))
    ফেরত -1;  /* Invalid bounds/size. */
  beg = elem.beg;
  end = elem.end;

  /* Get tbsCertificate. */
  beg = getASN1Element(&tbsCertificate, beg, end);
  যদি(!beg)
    ফেরত -1;
  /* Skip the signatureAlgorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  যদি(!beg)
    ফেরত -1;
  /* Get the signatureValue. */
  যদি(!getASN1Element(&cert->signature, beg, end))
    ফেরত -1;

  /* Parse TBSCertificate. */
  beg = tbsCertificate.beg;
  end = tbsCertificate.end;
  /* Get optional version, get serialNumber. */
  cert->version.header = ফাঁকা;
  cert->version.beg = &defaultVersion;
  cert->version.end = &defaultVersion + আকার(defaultVersion);
  beg = getASN1Element(&elem, beg, end);
  যদি(!beg)
    ফেরত -1;
  যদি(elem.tag == 0) {
    যদি(!getASN1Element(&cert->version, elem.beg, elem.end))
      ফেরত -1;
    beg = getASN1Element(&elem, beg, end);
    যদি(!beg)
      ফেরত -1;
  }
  cert->serial_সংখ্যা = elem;
  /* Get signature algorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  /* Get issuer. */
  beg = getASN1Element(&cert->issuer, beg, end);
  যদি(!beg)
    ফেরত -1;
  /* Get notBefore and notAfter. */
  beg = getASN1Element(&elem, beg, end);
  যদি(!beg)
    ফেরত -1;
  ccp = getASN1Element(&cert->notBefore, elem.beg, elem.end);
  যদি(!ccp)
    ফেরত -1;
  যদি(!getASN1Element(&cert->notAfter, ccp, elem.end))
    ফেরত -1;
  /* These two fields MUST consume all of the validity elem */
  যদি(elem.end != cert->notAfter.end)
    ফেরত -1;
  /* Get subject. */
  beg = getASN1Element(&cert->subject, beg, end);
  যদি(!beg)
    ফেরত -1;
  /* Get subjectPublicKeyAlgorithm and subjectPublicKey. */
  beg = getASN1Element(&cert->subject_Public_চাবি_Info, beg, end);
  যদি(!beg)
    ফেরত -1;
  ccp = getASN1Element(&cert->subject_Public_চাবি_Algorithm,
                       cert->subject_Public_চাবি_Info.beg,
                       cert->subject_Public_চাবি_Info.end);
  যদি(!ccp)
    ফেরত -1;
  যদি(!getASN1Element(&cert->subject_Public_চাবি, ccp,
                     cert->subject_Public_চাবি_Info.end))
    ফেরত -1;
  /* Get optional issuerUniqueID, subjectUniqueID and extensions. */
  cert->issuerUniqueID.tag = cert->subjectUniqueID.tag = 0;
  cert->extensions.tag = elem.tag = 0;
  cert->issuerUniqueID.header = cert->subjectUniqueID.header = ফাঁকা;
  cert->issuerUniqueID.beg = cert->issuerUniqueID.end = beg;
  cert->subjectUniqueID.beg = cert->subjectUniqueID.end = beg;
  cert->extensions.header = ফাঁকা;
  cert->extensions.beg = cert->extensions.end = beg;
  যদি(beg < end) {
    beg = getASN1Element(&elem, beg, end);
    যদি(!beg)
      ফেরত -1;
  }
  যদি(elem.tag == 1) {
    cert->issuerUniqueID = elem;
    যদি(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      যদি(!beg)
        ফেরত -1;
    }
  }
  যদি(elem.tag == 2) {
    cert->subjectUniqueID = elem;
    যদি(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      যদি(!beg)
        ফেরত -1;
    }
  }
  যদি(elem.tag == 3)
    যদি(!getASN1Element(&cert->extensions, elem.beg, elem.end))
      ফেরত -1;
  ফেরত 0;
}

#endif /* WANT_PARSEX509 */

#ifdef WANT_EXTRACT_CERTINFO

স্থির CURLcode dumpAlgo(কাঠামো dynbuf *store,
                         কাঠামো Curl_asn1Element *param,
                         ধ্রুবক uint8_t *beg, ধ্রুবক uint8_t *end)
{
  কাঠামো Curl_asn1Element oid;

  /* Get algorithm parameters and return algorithm name. */

  beg = getASN1Element(&oid, beg, end);
  যদি(!beg)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  param->header = ফাঁকা;
  param->tag = 0;
  param->beg = param->end = end;
  যদি(beg < end) {
    ধ্রুবক uint8_t *p = getASN1Element(param, beg, end);
    যদি(!p)
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }
  ফেরত OID2str(store, oid.beg, oid.end);
}

/*
 * This is a convenience function for push_certinfo_len that takes a zero
 * terminated value.
 */
স্থির CURLcode ssl_ঠেলো_certinfo(কাঠামো Curl_easy *তথ্য,
                                  পূর্ণসংখ্যা certnum,
                                  ধ্রুবক অক্ষর *label,
                                  ধ্রুবক অক্ষর *মান)
{
  size_t valuelen = strlen(মান);

  ফেরত Curl_ssl_ঠেলো_certinfo_len(তথ্য, certnum, label, মান, valuelen);
}

/*
 * This is a convenience function for push_certinfo_len that takes a
 * dynbuf value.
 *
 * It also does the verbose output if !certnum.
 */
স্থির CURLcode ssl_ঠেলো_certinfo_dyn(কাঠামো Curl_easy *তথ্য,
                                      পূর্ণসংখ্যা certnum,
                                      ধ্রুবক অক্ষর *label,
                                      কাঠামো dynbuf *ptr)
{
  size_t valuelen = curlx_dyn_len(ptr);
  ধ্রুবক অক্ষর *মান = curlx_dyn_ptr(ptr);

  CURLcode ফলাফল = Curl_ssl_ঠেলো_certinfo_len(তথ্য, certnum, label,
                                               মান, valuelen);

  যদি(!certnum && !ফলাফল)
    infof(তথ্য, "   %s: %s", label, মান);

  ফেরত ফলাফল;
}

স্থির CURLcode do_pubkey_field(কাঠামো Curl_easy *তথ্য, পূর্ণসংখ্যা certnum,
                                ধ্রুবক অক্ষর *label,
                                কাঠামো Curl_asn1Element *elem)
{
  CURLcode ফলাফল;
  কাঠামো dynbuf out;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);

  /* Generate a certificate information record for the public key. */

  ফলাফল = ASN1tostr(&out, elem);
  যদি(!ফলাফল && তথ্য->set.ssl.certinfo)
    ফলাফল = ssl_ঠেলো_certinfo_dyn(তথ্য, certnum, label, &out);
  curlx_dyn_free(&out);
  ফেরত ফলাফল;
}

/* return 0 on success, 1 on error */
স্থির পূর্ণসংখ্যা do_pubkey(কাঠামো Curl_easy *তথ্য, পূর্ণসংখ্যা certnum, ধ্রুবক অক্ষর *algo,
                     কাঠামো Curl_asn1Element *param,
                     কাঠামো Curl_asn1Element *pubkey)
{
  কাঠামো Curl_asn1Element elem;
  কাঠামো Curl_asn1Element pk;
  ধ্রুবক uint8_t *p;

  /* Generate all information records for the public key. */

  যদি(curl_strequal(algo, "ecPublicKey")) {
    /*
     * ECC public key is all the data, a value of type BIT STRING mapped to
     * OCTET STRING and should not be parsed as an ASN.1 value.
     */
    ধ্রুবক size_t dlen = pubkey->end - pubkey->beg;
    size_t len;
    যদি(dlen < 2)
      /* too small */
      ফেরত 1;
    len = (dlen - 2) * 4;
    যদি(!certnum)
      infof(তথ্য, "   ECC Public Key (%zu bits)", len);
    যদি(তথ্য->set.ssl.certinfo) {
      অক্ষর q[(আকার(len) * 8 / 3) + 1];
      (শূন্য)curl_msnprintf(q, আকার(q), "%zu", len);
      যদি(ssl_ঠেলো_certinfo(তথ্য, certnum, "ECC Public Key", q))
        ফেরত 1;
    }
    ফেরত do_pubkey_field(তথ্য, certnum, "ecPublicKey", pubkey) == CURLE_OK
      ? 0 : 1;
  }

  /* Get the public key (single element). */
  যদি(!getASN1Element(&pk, pubkey->beg + 1, pubkey->end))
    ফেরত 1;

  যদি(curl_strequal(algo, "rsaEncryption")) {
    ধ্রুবক uint8_t *q;
    size_t len;

    p = getASN1Element(&elem, pk.beg, pk.end);
    যদি(!p)
      ফেরত 1;

    /* Compute key length. */
    জন্য(q = elem.beg; q < elem.end && !*q; q++)
      ;
    len = ((elem.end - q) * 8);
    যদি(len) {
      unsigned পূর্ণসংখ্যা i;
      জন্য(i = *q; !(i & 0x80); i <<= 1)
        len--;
    }
    যদি(len > 32)
      elem.beg = q;     /* Strip leading zero bytes. */
    যদি(!certnum)
      infof(তথ্য, "   RSA Public Key (%zu bits)", len);
    যদি(তথ্য->set.ssl.certinfo) {
      অক্ষর r[(আকার(len) * 8 / 3) + 1];
      curl_msnprintf(r, আকার(r), "%zu", len);
      যদি(ssl_ঠেলো_certinfo(তথ্য, certnum, "RSA Public Key", r))
        ফেরত 1;
    }
    /* Generate coefficients. */
    যদি(do_pubkey_field(তথ্য, certnum, "rsa(n)", &elem))
      ফেরত 1;
    যদি(!getASN1Element(&elem, p, pk.end))
      ফেরত 1;
    যদি(do_pubkey_field(তথ্য, certnum, "rsa(e)", &elem))
      ফেরত 1;
  }
  অন্যথা যদি(curl_strequal(algo, "dsa")) {
    p = getASN1Element(&elem, param->beg, param->end);
    যদি(p) {
      যদি(do_pubkey_field(তথ্য, certnum, "dsa(p)", &elem))
        ফেরত 1;
      p = getASN1Element(&elem, p, param->end);
      যদি(p) {
        যদি(do_pubkey_field(তথ্য, certnum, "dsa(q)", &elem))
          ফেরত 1;
        যদি(getASN1Element(&elem, p, param->end)) {
          যদি(do_pubkey_field(তথ্য, certnum, "dsa(g)", &elem))
            ফেরত 1;
          যদি(do_pubkey_field(তথ্য, certnum, "dsa(pub_key)", &pk))
            ফেরত 1;
        }
      }
    }
  }
  অন্যথা যদি(curl_strequal(algo, "dhpublicnumber")) {
    p = getASN1Element(&elem, param->beg, param->end);
    যদি(p) {
      যদি(do_pubkey_field(তথ্য, certnum, "dh(p)", &elem))
        ফেরত 1;
      যদি(getASN1Element(&elem, p, param->end)) {
        যদি(do_pubkey_field(তথ্য, certnum, "dh(g)", &elem))
          ফেরত 1;
        যদি(do_pubkey_field(তথ্য, certnum, "dh(pub_key)", &pk))
          ফেরত 1;
      }
    }
  }
  ফেরত 0;
}

/*
 * Convert an ASN.1 distinguished name into a printable string.
 * Return error.
 */
স্থির CURLcode DNtostr(কাঠামো dynbuf *store, কাঠামো Curl_asn1Element *dn)
{
  ফেরত encodeDN(store, dn);
}

CURLcode Curl_extract_certinfo(কাঠামো Curl_easy *তথ্য,
                               পূর্ণসংখ্যা certnum,
                               ধ্রুবক uint8_t *beg,
                               ধ্রুবক uint8_t *end)
{
  কাঠামো Curl_X509certificate cert;
  কাঠামো Curl_asn1Element param;
  অক্ষর *certptr;
  size_t clen;
  কাঠামো dynbuf out;
  CURLcode ফলাফল = CURLE_OK;
  unsigned পূর্ণসংখ্যা version;
  ধ্রুবক uint8_t *ptr;
  পূর্ণসংখ্যা rc;

  যদি(!তথ্য->set.ssl.certinfo)
    যদি(certnum)
      ফেরত CURLE_OK;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);
  /* Prepare the certificate information for curl_easy_getinfo(). */

  /* Extract the certificate ASN.1 elements. */
  যদি(Curl_parseX509(&cert, beg, end))
    ফেরত CURLE_PEER_FAILED_VERIFICATION;

  /* Subject. */
  ফলাফল = DNtostr(&out, &cert.subject);
  যদি(ফলাফল)
    যাও done;
  যদি(তথ্য->set.ssl.certinfo) {
    ফলাফল = ssl_ঠেলো_certinfo_dyn(তথ্য, certnum, "Subject", &out);
    যদি(ফলাফল)
      যাও done;
  }
  curlx_dyn_reset(&out);

  /* Issuer. */
  ফলাফল = DNtostr(&out, &cert.issuer);
  যদি(ফলাফল)
    যাও done;
  যদি(তথ্য->set.ssl.certinfo) {
    ফলাফল = ssl_ঠেলো_certinfo_dyn(তথ্য, certnum, "Issuer", &out);
    যদি(ফলাফল)
      যাও done;
  }
  curlx_dyn_reset(&out);

  /* Version (always fits in less than 32 bits). */
  version = 0;
  জন্য(ptr = cert.version.beg; ptr < cert.version.end; ptr++)
    version = (version << 8) | *ptr;
  যদি(তথ্য->set.ssl.certinfo) {
    ফলাফল = curlx_dyn_addf(&out, "%x", version);
    যদি(ফলাফল)
      যাও done;
    ফলাফল = ssl_ঠেলো_certinfo_dyn(তথ্য, certnum, "Version", &out);
    যদি(ফলাফল)
      যাও done;
    curlx_dyn_reset(&out);
  }

  /* Serial number. */
  ফলাফল = ASN1tostr(&out, &cert.serial_সংখ্যা);
  যদি(ফলাফল)
    যাও done;
  যদি(তথ্য->set.ssl.certinfo) {
    ফলাফল = ssl_ঠেলো_certinfo_dyn(তথ্য, certnum, "Serial Number", &out);
    যদি(ফলাফল)
      যাও done;
  }
  curlx_dyn_reset(&out);

  /* Signature algorithm .*/
  ফলাফল = dumpAlgo(&out, &param, cert.signatureAlgorithm.beg,
                    cert.signatureAlgorithm.end);
  যদি(ফলাফল)
    যাও done;
  যদি(তথ্য->set.ssl.certinfo) {
    ফলাফল = ssl_ঠেলো_certinfo_dyn(তথ্য, certnum, "Signature Algorithm", &out);
    যদি(ফলাফল)
      যাও done;
  }
  curlx_dyn_reset(&out);

  /* Start Date. */
  ফলাফল = ASN1tostr(&out, &cert.notBefore);
  যদি(ফলাফল)
    যাও done;
  যদি(তথ্য->set.ssl.certinfo) {
    ফলাফল = ssl_ঠেলো_certinfo_dyn(তথ্য, certnum, "Start Date", &out);
    যদি(ফলাফল)
      যাও done;
  }
  curlx_dyn_reset(&out);

  /* Expire Date. */
  ফলাফল = ASN1tostr(&out, &cert.notAfter);
  যদি(ফলাফল)
    যাও done;
  যদি(তথ্য->set.ssl.certinfo) {
    ফলাফল = ssl_ঠেলো_certinfo_dyn(তথ্য, certnum, "Expire Date", &out);
    যদি(ফলাফল)
      যাও done;
  }
  curlx_dyn_reset(&out);

  /* Public Key Algorithm. */
  ফলাফল = dumpAlgo(&out, &param, cert.subject_Public_চাবি_Algorithm.beg,
                    cert.subject_Public_চাবি_Algorithm.end);
  যদি(ফলাফল)
    যাও done;
  যদি(তথ্য->set.ssl.certinfo) {
    ফলাফল = ssl_ঠেলো_certinfo_dyn(তথ্য, certnum, "Public Key Algorithm",
                                   &out);
    যদি(ফলাফল)
      যাও done;
  }

  rc = do_pubkey(তথ্য, certnum, curlx_dyn_ptr(&out),
                 &param, &cert.subject_Public_চাবি);
  যদি(rc) {
    ফলাফল = CURLE_OUT_OF_MEMORY; /* the most likely error */
    যাও done;
  }
  curlx_dyn_reset(&out);

  /* Signature. */
  ফলাফল = ASN1tostr(&out, &cert.signature);
  যদি(ফলাফল)
    যাও done;
  যদি(তথ্য->set.ssl.certinfo) {
    ফলাফল = ssl_ঠেলো_certinfo_dyn(তথ্য, certnum, "Signature", &out);
    যদি(ফলাফল)
      যাও done;
  }
  curlx_dyn_reset(&out);

  /* Generate PEM certificate. */
  ফলাফল = curlx_base64_encode((ধ্রুবক uint8_t *)cert.certificate.beg,
                               cert.certificate.end - cert.certificate.beg,
                               &certptr, &clen);
  যদি(ফলাফল)
    যাও done;

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
  ফলাফল = curlx_dyn_add(&out, "-----BEGIN CERTIFICATE-----\n");
  যদি(!ফলাফল) {
    size_t j = 0;

    যখন(!ফলাফল && (j < clen)) {
      size_t chunksize = (clen - j) > 64 ? 64 : (clen - j);
      ফলাফল = curlx_dyn_addn(&out, &certptr[j], chunksize);
      যদি(!ফলাফল)
        ফলাফল = curlx_dyn_addn(&out, "\n", 1);
      j += chunksize;
    }
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_add(&out, "-----END CERTIFICATE-----\n");
  }
  curlx_free(certptr);
  যদি(!ফলাফল)
    যদি(তথ্য->set.ssl.certinfo)
      ফলাফল = ssl_ঠেলো_certinfo_dyn(তথ্য, certnum, "Cert", &out);

done:
  যদি(ফলাফল)
    failf(তথ্য, "Failed extracting certificate chain");
  curlx_dyn_free(&out);
  ফেরত ফলাফল;
}

#endif /* WANT_EXTRACT_CERTINFO */

#endif /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
