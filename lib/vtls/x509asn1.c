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
#ضمن "curl_setup.h"

#אם defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#אם defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_RUSTLS)
#define WANT_PARSEX509 /* uses Curl_parseX509() */
#endif

#אם defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_RUSTLS)
#define WANT_EXTRACT_CERTINFO /* uses Curl_extract_certinfo() */
#endif

#ضمن "urldata.h"
#ضمن "vtls/vtls.h"
#ضمن "curl_trc.h"
#ضمن "curlx/base64.h"
#ضمن "vtls/x509asn1.h"
#ضمن "curlx/dynbuf.h"

/*
 * Constants.
 */

/* Largest supported ASN.1 structure. */
#define CURL_ASN1_MAX                   ((size_t)0x40000)      /* 256K */

#ifdef WANT_EXTRACT_CERTINFO

/* ASN.1 OID table entry. */
بنية Curl_OID {
  ثابت حرف *numoid;  /* Dotted-numeric OID. */
  ثابت حرف *textoid; /* OID name. */
};

/* ASN.1 OIDs. */
ساكن ثابت بنية Curl_OID OIDtable[] = {
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
  { عدم,                       عدم }
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

ساكن ثابت uint8_t *getASN1Element_(بنية Curl_asn1Element *elem,
                                      ثابت uint8_t *beg, ثابت uint8_t *end,
                                      size_t lvl)
{
  uint8_t b;
  size_t len;
  بنية Curl_asn1Element lelem;

  /* Get a single ASN.1 element into `elem', parse ASN.1 string at `beg'
     ending at `end'.
     Returns a pointer in source string after the parsed element, or NULL
     if an error occurs. */
  אם(!beg || !end || beg >= end || !*beg ||
     ((size_t)(end - beg) > CURL_ASN1_MAX) ||
     lvl++ >= CURL_ASN1_MAX_RECURSIONS)
    החזר عدم;

  /* Process header byte. */
  elem->header = beg;
  b = *beg++;
  elem->constructed = (b & 0x20) != 0;
  elem->eclass = (b >> 6) & 3;
  b &= 0x1F;
  אם(b == 0x1F)
    החזר عدم; /* Long tag values not supported here. */
  elem->tag = b;

  /* Process length. */
  אם(beg >= end)
    החזר عدم;
  b = *beg++;
  אם(!(b & 0x80))
    len = b;
  אחרת {
    b &= 0x7F;
    אם(!b) {
      /* Unspecified length. Since we have all the data, we can determine the
         effective length by skipping element until an EOC element is found. */
      אם(!elem->constructed)
        החזר عدم;
      elem->beg = beg;
      כלעוד(beg < end && *beg) {
        beg = getASN1Element_(&lelem, beg, end, lvl);
        אם(!beg)
          החזר عدم;
      }
      אם(end - beg < 2 || beg[1])       /* EOC must have a zero length. */
        החזר عدم;
      elem->end = beg;                  /* Do not include EOC as content. */
      החזר beg + 2;                   /* After EOC element. */
    }
    אחרת אם(b > (size_t)(end - beg))
      החזר عدم; /* Does not fit in source. */
    אחרת {
      /* Get long length. */
      len = 0;
      עשה {
        אם(len & 0xFF000000L)
          החזר عدم;  /* Lengths > 32 bits are not supported. */
        len = (len << 8) | *beg++;
      } כלעוד(--b);
    }
  }
  אם(len > (size_t)(end - beg))
    החזר عدم;  /* Element data does not fit in source. */
  elem->beg = beg;
  elem->end = beg + len;
  החזר elem->end;
}

/*
 * @unittest 1657
 */
UNITTEST ثابت uint8_t *getASN1Element(بنية Curl_asn1Element *elem,
                                       ثابت uint8_t *beg, ثابت uint8_t *end);
UNITTEST ثابت uint8_t *getASN1Element(بنية Curl_asn1Element *elem,
                                       ثابت uint8_t *beg, ثابت uint8_t *end)
{
  החזר getASN1Element_(elem, beg, end, 0);
}

#ifdef WANT_EXTRACT_CERTINFO

/*
 * Search the null-terminated OID or OID identifier in local table.
 * Return the table entry pointer or NULL if not found.
 */
ساكن ثابت بنية Curl_OID *searchOID(ثابت حرف *oid)
{
  ثابت بنية Curl_OID *op;
  עבור(op = OIDtable; op->numoid; op++)
    אם(!strcmp(op->numoid, oid) || curl_strequal(op->textoid, oid))
      החזר op;

  החזר عدم;
}

/*
 * Convert an ASN.1 Boolean value into its string representation.
 *
 * Return error code.
 */

ساكن CURLcode bool2str(بنية dynbuf *store,
                         ثابت uint8_t *beg, ثابت uint8_t *end)
{
  uint8_t val;
  אם(end - beg != 1)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
  val = *beg;
  אם((val != 0xff) && (val != 0x00))
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
  החזר curlx_dyn_add(store, val ? "TRUE" : "FALSE");
}

/*
 * Convert an ASN.1 octet string to a printable string.
 *
 * Return error code.
 */
ساكن CURLcode octet2str(بنية dynbuf *store,
                          ثابت uint8_t *beg, ثابت uint8_t *end)
{
  CURLcode result = CURLE_OK;

  כלעוד(!result && beg < end)
    result = curlx_dyn_addf(store, "%02x:", *beg++);

  החזר result;
}

ساكن CURLcode bit2str(بنية dynbuf *store,
                        ثابت uint8_t *beg, ثابت uint8_t *end)
{
  /* Convert an ASN.1 bit string to a printable string. */

  אם(beg < end) {
    uint8_t unused_bits = *beg++;
    אם(unused_bits > 7)
      החזר CURLE_BAD_FUNCTION_ARGUMENT;
  }
  אחרת
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
  החזר octet2str(store, beg, end);
}

/*
 * Convert an ASN.1 integer value into its string representation.
 *
 * Returns error.
 */
ساكن CURLcode int2str(بنية dynbuf *store,
                        ثابت uint8_t *beg, ثابت uint8_t *end)
{
  uint32_t uval = 0;
  int32_t sval = 0;
  size_t n = end - beg;
  size_t i;
  ثابت uint8_t *p = beg;

  אם(!n)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;

  אם(n > 4)
    החזר octet2str(store, beg, end);

  אם(p[0] & 0x80)
    uval = ~0U;

  עבור(i = 0; i < n; i++)
    uval = (uval << 8) | p[i];

  sval = (int32_t)uval;

  /* small values are easier to read as decimals */
  אם(sval > -10000 && sval < 10000)
    החזר curlx_dyn_addf(store, "%d", sval);

  /* represent larger values as hex */
  החזר curlx_dyn_addf(store, "0x%x", uval);
}

/*
 * Convert from an ASN.1 typed string to UTF8.
 *
 * The result is stored in a dynbuf that is inited by the user of this
 * function.
 *
 * Returns error.
 */
ساكن CURLcode utf8asn1str(بنية dynbuf *to, صحيح type,
                            ثابت uint8_t *from, ثابت uint8_t *end)
{
  size_t inlength = end - from;
  صحيح size = 1;
  CURLcode result = CURLE_OK;

  بدل(type) {
  حالة CURL_ASN1_BMP_STRING:
    size = 2;
    اكسر;
  حالة CURL_ASN1_UNIVERSAL_STRING:
    size = 4;
    اكسر;
  حالة CURL_ASN1_NUMERIC_STRING:
  حالة CURL_ASN1_PRINTABLE_STRING:
  حالة CURL_ASN1_TELETEX_STRING:
  حالة CURL_ASN1_IA5_STRING:
  حالة CURL_ASN1_VISIBLE_STRING:
  حالة CURL_ASN1_UTF8_STRING:
    اكسر;
  افتراضي:
    החזר CURLE_BAD_FUNCTION_ARGUMENT;  /* Conversion not supported. */
  }

  אם(inlength % size)
    /* Length inconsistent with character size. */
    החזר CURLE_BAD_FUNCTION_ARGUMENT;

  אם(type == CURL_ASN1_UTF8_STRING) {
    /* copy. */
    אם(inlength)
      result = curlx_dyn_addn(to, from, inlength);
  }
  אחרת {
    כלעוד(!result && (from < end)) {
      حرف buf[4]; /* decode buffer */
      size_t charsize = 1;
      unsigned صحيح wc = 0;

      بدل(size) {
      حالة 4:
        wc = (wc << 8) | *from++;
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      حالة 2:
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      افتراضي: /* case 1: */
        wc = (wc << 8) | *from++;
      }
      אם(wc >= 0x00000080) {
        אם(wc >= 0x00000800) {
          אם(wc >= 0x00010000) {
            אם(wc >= 0x00200000) {
              /* Invalid char. size for target encoding. */
              החזר CURLE_WEIRD_SERVER_REPLY;
            }
            buf[3] = (حرف)(0x80 | (wc & 0x3F));
            wc = (wc >> 6) | 0x00010000;
            charsize++;
          }
          buf[2] = (حرف)(0x80 | (wc & 0x3F));
          wc = (wc >> 6) | 0x00000800;
          charsize++;
        }
        buf[1] = (حرف)(0x80 | (wc & 0x3F));
        wc = (wc >> 6) | 0x000000C0;
        charsize++;
      }
      buf[0] = (حرف)wc;
      result = curlx_dyn_addn(to, buf, charsize);
    }
  }
  החזר result;
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
UNITTEST CURLcode encodeOID(بنية dynbuf *store,
                            ثابت uint8_t *beg, ثابت uint8_t *end);
UNITTEST CURLcode encodeOID(بنية dynbuf *store,
                            ثابت uint8_t *beg, ثابت uint8_t *end)
{
  uint32_t x;
  uint32_t y;
  CURLcode result = CURLE_OK;

  אם(end <= beg)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;

  /* Process the first two numbers. The initial digit cannot be larger than
     2 */
  y = *beg++;
  אם(y <= 80)
    x = y / 40;
  אחרת
    x = 2;
  result = curlx_dyn_addf(store, "%u", x);
  אם(result)
    החזר result;

  אם(y & 0x80) {
    uint32_t t = (y & 0x7f); /* the second number */
    אם(!t)
      /* reject leading 0x80 padding */
      החזר CURLE_BAD_FUNCTION_ARGUMENT;
    עשה {
      אם((t & 0xFE000000) || (beg >= end))
        החזר CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      t = (t << 7) | (y & 0x7F);
    } כלעוד(y & 0x80);
    y = t;
  }

  result = curlx_dyn_addf(store, ".%u", y - (x * 40));
  אם(result)
    החזר result;

  /* Process the trailing numbers. */
  כלעוד((beg < end) && !result) {
    x = 0;
    y = 0;
    עשה {
      /* 0x80 as the first value is invalid since it only adds padding */
      אם((y & 0x80) && !x)
        החזר CURLE_BAD_FUNCTION_ARGUMENT;
      אחרת אם((x & 0xFE000000) || (beg == end))
        החזר CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      x = (x << 7) | (y & 0x7F);
    } כלעוד(y & 0x80);
    result = curlx_dyn_addf(store, ".%u", x);
  }
  החזר result;
}

/*
 * Convert an ASN.1 OID into its dotted or symbolic string representation.
 *
 * Return error code.
 */

ساكن CURLcode OID2str(بنية dynbuf *store,
                        ثابت uint8_t *beg, ثابت uint8_t *end)
{
  CURLcode result = CURLE_OK;
  אם(beg < end) {
    بنية dynbuf buf;
    curlx_dyn_init(&buf, CURL_X509_STR_MAX);
    result = encodeOID(&buf, beg, end);

    אם(!result) {
      ثابت بنية Curl_OID *op = searchOID(curlx_dyn_ptr(&buf));
      אם(op)
        result = curlx_dyn_add(store, op->textoid);
      אחרת
        result = curlx_dyn_add(store, curlx_dyn_ptr(&buf));
    }
    curlx_dyn_free(&buf);
  }
  החזר result;
}

/*
 * @unittest 1656
 */
UNITTEST CURLcode GTime2str(بنية dynbuf *store,
                            ثابت حرف *beg, ثابت حرف *end);
UNITTEST CURLcode GTime2str(بنية dynbuf *store,
                            ثابت حرف *beg, ثابت حرف *end)
{
  ثابت حرف *tzp;
  ثابت حرف *fracp;
  حرف sec1, sec2;
  size_t fracl;
  size_t tzl;
  ثابت حرف *sep = "";

  /* Convert an ASN.1 Generalized time to a printable string.
     Return the dynamically allocated string, or NULL if an error occurs. */

  עבור(fracp = beg; fracp < end && ISDIGIT(*fracp); fracp++)
    ;

  /* Get seconds digits. */
  sec1 = '0';
  بدل(fracp - beg - 12) {
  حالة 0:
    sec2 = '0';
    اكسر;
  حالة 2:
    sec1 = fracp[-2];
    FALLTHROUGH();
  حالة 1:
    sec2 = fracp[-1];
    اكسر;
  افتراضي:
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* timezone follows optional fractional seconds. */
  tzp = fracp;
  fracl = 0; /* no fractional seconds detected so far */
  אם(fracp < end && (*fracp == '.' || *fracp == ',')) {
    /* Have fractional seconds, e.g. "[.,]\d+". How many? */
    fracp++; /* should be a digit char or BAD ARGUMENT */
    tzp = fracp;
    כלעוד(tzp < end && ISDIGIT(*tzp))
      tzp++;
    אם(tzp == fracp) /* never looped, no digit after [.,] */
      החזר CURLE_BAD_FUNCTION_ARGUMENT;
    fracl = tzp - fracp; /* number of fractional sec digits */
    DEBUGASSERT(fracl > 0);
    /* Strip trailing zeroes in fractional seconds.
     * May reduce fracl to 0 if only '0's are present. */
    כלעוד(fracl && fracp[fracl - 1] == '0')
      fracl--;
  }

  /* Process timezone. */
  אם(tzp >= end) {
    tzp = "";
    tzl = 0;
  }
  אחרת אם(*tzp == 'Z') {
    sep = " ";
    tzp = "GMT";
    tzl = 3;
  }
  אחרת אם((*tzp == '+') || (*tzp == '-')) {
    sep = " UTC";
    tzl = end - tzp;
  }
  אחרת {
    sep = " ";
    tzl = end - tzp;
  }

  החזר curlx_dyn_addf(store,
                        "%.4s-%.2s-%.2s %.2s:%.2s:%c%c%s%.*s%s%.*s",
                        beg, beg + 4, beg + 6,
                        beg + 8, beg + 10, sec1, sec2,
                        fracl ? "." : "", (صحيح)fracl, fracp,
                        sep, (صحيح)tzl, tzp);
}

/*
 * Convert an ASN.1 UTC time to a printable string.
 *
 * Return error code.
 */
ساكن CURLcode UTime2str(بنية dynbuf *store,
                          ثابت حرف *beg, ثابت حرف *end)
{
  ثابت حرف *tzp;
  size_t tzl;
  ثابت حرف *sec;

  עבור(tzp = beg; tzp < end && *tzp >= '0' && *tzp <= '9'; tzp++)
    ;
  /* Get the seconds. */
  sec = beg + 10;
  بدل(tzp - sec) {
  حالة 0:
    sec = "00";
    FALLTHROUGH();
  حالة 2:
    اكسر;
  افتراضي:
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Process timezone. */
  אם(tzp >= end)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
  אם(*tzp == 'Z') {
    tzp = "GMT";
    end = tzp + 3;
  }

  tzl = end - tzp;
  החזר curlx_dyn_addf(store, "%d%.2s-%.2s-%.2s %.2s:%.2s:%.2s %.*s",
                        20 - (*beg >= '5'), beg, beg + 2, beg + 4,
                        beg + 6, beg + 8, sec,
                        (صحيح)tzl, tzp);
}

/*
 * Convert an ASN.1 element to a printable string.
 *
 * Return error
 *
 * @unittest 1667
 */
UNITTEST CURLcode ASN1tostr(بنية dynbuf *store,
                            بنية Curl_asn1Element *elem);
UNITTEST CURLcode ASN1tostr(بنية dynbuf *store,
                            بنية Curl_asn1Element *elem)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  صحيح type = elem->tag;
  אם(elem->constructed)
    החזר result; /* No conversion of structured elements. */

  بدل(type) {
  حالة CURL_ASN1_BOOLEAN:
    result = bool2str(store, elem->beg, elem->end);
    اكسر;
  حالة CURL_ASN1_INTEGER:
  حالة CURL_ASN1_ENUMERATED:
    result = int2str(store, elem->beg, elem->end);
    اكسر;
  حالة CURL_ASN1_BIT_STRING:
    result = bit2str(store, elem->beg, elem->end);
    اكسر;
  حالة CURL_ASN1_OCTET_STRING:
    result = octet2str(store, elem->beg, elem->end);
    اكسر;
  حالة CURL_ASN1_NULL:
    result = curlx_dyn_addn(store, "", 1);
    اكسر;
  حالة CURL_ASN1_OBJECT_IDENTIFIER:
    result = OID2str(store, elem->beg, elem->end);
    اكسر;
  حالة CURL_ASN1_UTC_TIME:
    result = UTime2str(store, (ثابت حرف *)elem->beg,
                       (ثابت حرف *)elem->end);
    اكسر;
  حالة CURL_ASN1_GENERALIZED_TIME:
    result = GTime2str(store, (ثابت حرف *)elem->beg,
                       (ثابت حرف *)elem->end);
    اكسر;
  حالة CURL_ASN1_UTF8_STRING:
  حالة CURL_ASN1_NUMERIC_STRING:
  حالة CURL_ASN1_PRINTABLE_STRING:
  حالة CURL_ASN1_TELETEX_STRING:
  حالة CURL_ASN1_IA5_STRING:
  حالة CURL_ASN1_VISIBLE_STRING:
  حالة CURL_ASN1_UNIVERSAL_STRING:
  حالة CURL_ASN1_BMP_STRING:
    result = utf8asn1str(store, type, elem->beg, elem->end);
    اكسر;
  }

  החזר result;
}

/*
 * ASCII encode distinguished name at `dn' into the store dynbuf.
 *
 * Returns error.
 */
ساكن CURLcode encodeDN(بنية dynbuf *store, بنية Curl_asn1Element *dn)
{
  بنية Curl_asn1Element rdn;
  بنية Curl_asn1Element atv;
  بنية Curl_asn1Element oid;
  بنية Curl_asn1Element value;
  ثابت uint8_t *p1;
  ثابت uint8_t *p2;
  ثابت uint8_t *p3;
  ثابت uint8_t *str;
  CURLcode result = CURLE_OK;
  bool added = FALSE;
  بنية dynbuf temp;
  curlx_dyn_init(&temp, CURL_X509_STR_MAX);

  עבור(p1 = dn->beg; p1 < dn->end;) {
    p1 = getASN1Element(&rdn, p1, dn->end);
    אם(!p1) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      اذهب error;
    }
    עבור(p2 = rdn.beg; p2 < rdn.end;) {
      p2 = getASN1Element(&atv, p2, rdn.end);
      אם(!p2) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        اذهب error;
      }
      p3 = getASN1Element(&oid, atv.beg, atv.end);
      אם(!p3) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        اذهب error;
      }
      אם(!getASN1Element(&value, p3, atv.end)) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        اذهب error;
      }
      curlx_dyn_reset(&temp);
      result = ASN1tostr(&temp, &oid);
      אם(result)
        اذهب error;

      str = curlx_dyn_uptr(&temp);

      אם(!str) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        اذهب error;
      }

      /* Encode delimiter.
         If attribute has a short uppercase name, delimiter is ", ". */
      עבור(p3 = str; ISUPPER(*p3); p3++)
        ;
      אם(added) {
        אם(p3 - str > 2)
          result = curlx_dyn_addn(store, "/", 1);
        אחרת
          result = curlx_dyn_addn(store, ", ", 2);
        אם(result)
          اذهب error;
      }

      /* Encode attribute name. */
      result = curlx_dyn_add(store, (ثابت حرف *)str);
      אם(result)
        اذهب error;

      /* Generate equal sign. */
      result = curlx_dyn_addn(store, "=", 1);
      אם(result)
        اذهب error;

      /* Generate value. */
      result = ASN1tostr(store, &value);
      אם(result)
        اذهب error;
      curlx_dyn_reset(&temp);
      added = TRUE; /* use separator for next */
    }
  }
error:
  curlx_dyn_free(&temp);

  החזר result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#ifdef WANT_PARSEX509
/*
 * ASN.1 parse an X509 certificate into structure subfields.
 * Syntax is assumed to have already been checked by the SSL backend.
 * See RFC 5280.
 */
صحيح Curl_parseX509(بنية Curl_X509certificate *cert,
                   ثابت uint8_t *beg, ثابت uint8_t *end)
{
  بنية Curl_asn1Element elem;
  بنية Curl_asn1Element tbsCertificate;
  ثابت uint8_t *ccp;
  ساكن ثابت uint8_t defaultVersion = 0;  /* v1. */

  memset(cert, 0, حجم(*cert));
  cert->certificate.header = عدم;
  cert->certificate.beg = beg;
  cert->certificate.end = end;

  /* Get the sequence content. */
  אם(!getASN1Element(&elem, beg, end))
    החזר -1;  /* Invalid bounds/size. */
  beg = elem.beg;
  end = elem.end;

  /* Get tbsCertificate. */
  beg = getASN1Element(&tbsCertificate, beg, end);
  אם(!beg)
    החזר -1;
  /* Skip the signatureAlgorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  אם(!beg)
    החזר -1;
  /* Get the signatureValue. */
  אם(!getASN1Element(&cert->signature, beg, end))
    החזר -1;

  /* Parse TBSCertificate. */
  beg = tbsCertificate.beg;
  end = tbsCertificate.end;
  /* Get optional version, get serialNumber. */
  cert->version.header = عدم;
  cert->version.beg = &defaultVersion;
  cert->version.end = &defaultVersion + حجم(defaultVersion);
  beg = getASN1Element(&elem, beg, end);
  אם(!beg)
    החזר -1;
  אם(elem.tag == 0) {
    אם(!getASN1Element(&cert->version, elem.beg, elem.end))
      החזר -1;
    beg = getASN1Element(&elem, beg, end);
    אם(!beg)
      החזר -1;
  }
  cert->serialNumber = elem;
  /* Get signature algorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  /* Get issuer. */
  beg = getASN1Element(&cert->issuer, beg, end);
  אם(!beg)
    החזר -1;
  /* Get notBefore and notAfter. */
  beg = getASN1Element(&elem, beg, end);
  אם(!beg)
    החזר -1;
  ccp = getASN1Element(&cert->notBefore, elem.beg, elem.end);
  אם(!ccp)
    החזר -1;
  אם(!getASN1Element(&cert->notAfter, ccp, elem.end))
    החזר -1;
  /* These two fields MUST consume all of the validity elem */
  אם(elem.end != cert->notAfter.end)
    החזר -1;
  /* Get subject. */
  beg = getASN1Element(&cert->subject, beg, end);
  אם(!beg)
    החזר -1;
  /* Get subjectPublicKeyAlgorithm and subjectPublicKey. */
  beg = getASN1Element(&cert->subjectPublicKeyInfo, beg, end);
  אם(!beg)
    החזר -1;
  ccp = getASN1Element(&cert->subjectPublicKeyAlgorithm,
                       cert->subjectPublicKeyInfo.beg,
                       cert->subjectPublicKeyInfo.end);
  אם(!ccp)
    החזר -1;
  אם(!getASN1Element(&cert->subjectPublicKey, ccp,
                     cert->subjectPublicKeyInfo.end))
    החזר -1;
  /* Get optional issuerUniqueID, subjectUniqueID and extensions. */
  cert->issuerUniqueID.tag = cert->subjectUniqueID.tag = 0;
  cert->extensions.tag = elem.tag = 0;
  cert->issuerUniqueID.header = cert->subjectUniqueID.header = عدم;
  cert->issuerUniqueID.beg = cert->issuerUniqueID.end = beg;
  cert->subjectUniqueID.beg = cert->subjectUniqueID.end = beg;
  cert->extensions.header = عدم;
  cert->extensions.beg = cert->extensions.end = beg;
  אם(beg < end) {
    beg = getASN1Element(&elem, beg, end);
    אם(!beg)
      החזר -1;
  }
  אם(elem.tag == 1) {
    cert->issuerUniqueID = elem;
    אם(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      אם(!beg)
        החזר -1;
    }
  }
  אם(elem.tag == 2) {
    cert->subjectUniqueID = elem;
    אם(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      אם(!beg)
        החזר -1;
    }
  }
  אם(elem.tag == 3)
    אם(!getASN1Element(&cert->extensions, elem.beg, elem.end))
      החזר -1;
  החזר 0;
}

#endif /* WANT_PARSEX509 */

#ifdef WANT_EXTRACT_CERTINFO

ساكن CURLcode dumpAlgo(بنية dynbuf *store,
                         بنية Curl_asn1Element *param,
                         ثابت uint8_t *beg, ثابت uint8_t *end)
{
  بنية Curl_asn1Element oid;

  /* Get algorithm parameters and return algorithm name. */

  beg = getASN1Element(&oid, beg, end);
  אם(!beg)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
  param->header = عدم;
  param->tag = 0;
  param->beg = param->end = end;
  אם(beg < end) {
    ثابت uint8_t *p = getASN1Element(param, beg, end);
    אם(!p)
      החזר CURLE_BAD_FUNCTION_ARGUMENT;
  }
  החזר OID2str(store, oid.beg, oid.end);
}

/*
 * This is a convenience function for push_certinfo_len that takes a zero
 * terminated value.
 */
ساكن CURLcode ssl_push_certinfo(بنية Curl_easy *data,
                                  صحيح certnum,
                                  ثابت حرف *label,
                                  ثابت حرف *value)
{
  size_t valuelen = strlen(value);

  החזר Curl_ssl_push_certinfo_len(data, certnum, label, value, valuelen);
}

/*
 * This is a convenience function for push_certinfo_len that takes a
 * dynbuf value.
 *
 * It also does the verbose output if !certnum.
 */
ساكن CURLcode ssl_push_certinfo_dyn(بنية Curl_easy *data,
                                      صحيح certnum,
                                      ثابت حرف *label,
                                      بنية dynbuf *ptr)
{
  size_t valuelen = curlx_dyn_len(ptr);
  ثابت حرف *value = curlx_dyn_ptr(ptr);

  CURLcode result = Curl_ssl_push_certinfo_len(data, certnum, label,
                                               value, valuelen);

  אם(!certnum && !result)
    infof(data, "   %s: %s", label, value);

  החזר result;
}

ساكن CURLcode do_pubkey_field(بنية Curl_easy *data, صحيح certnum,
                                ثابت حرف *label,
                                بنية Curl_asn1Element *elem)
{
  CURLcode result;
  بنية dynbuf out;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);

  /* Generate a certificate information record for the public key. */

  result = ASN1tostr(&out, elem);
  אם(!result && data->set.ssl.certinfo)
    result = ssl_push_certinfo_dyn(data, certnum, label, &out);
  curlx_dyn_free(&out);
  החזר result;
}

/* return 0 on success, 1 on error */
ساكن صحيح do_pubkey(بنية Curl_easy *data, صحيح certnum, ثابت حرف *algo,
                     بنية Curl_asn1Element *param,
                     بنية Curl_asn1Element *pubkey)
{
  بنية Curl_asn1Element elem;
  بنية Curl_asn1Element pk;
  ثابت uint8_t *p;

  /* Generate all information records for the public key. */

  אם(curl_strequal(algo, "ecPublicKey")) {
    /*
     * ECC public key is all the data, a value of type BIT STRING mapped to
     * OCTET STRING and should not be parsed as an ASN.1 value.
     */
    ثابت size_t dlen = pubkey->end - pubkey->beg;
    size_t len;
    אם(dlen < 2)
      /* too small */
      החזר 1;
    len = (dlen - 2) * 4;
    אם(!certnum)
      infof(data, "   ECC Public Key (%zu bits)", len);
    אם(data->set.ssl.certinfo) {
      حرف q[(حجم(len) * 8 / 3) + 1];
      (فراغ)curl_msnprintf(q, حجم(q), "%zu", len);
      אם(ssl_push_certinfo(data, certnum, "ECC Public Key", q))
        החזר 1;
    }
    החזר do_pubkey_field(data, certnum, "ecPublicKey", pubkey) == CURLE_OK
      ? 0 : 1;
  }

  /* Get the public key (single element). */
  אם(!getASN1Element(&pk, pubkey->beg + 1, pubkey->end))
    החזר 1;

  אם(curl_strequal(algo, "rsaEncryption")) {
    ثابت uint8_t *q;
    size_t len;

    p = getASN1Element(&elem, pk.beg, pk.end);
    אם(!p)
      החזר 1;

    /* Compute key length. */
    עבור(q = elem.beg; q < elem.end && !*q; q++)
      ;
    len = ((elem.end - q) * 8);
    אם(len) {
      unsigned صحيح i;
      עבור(i = *q; !(i & 0x80); i <<= 1)
        len--;
    }
    אם(len > 32)
      elem.beg = q;     /* Strip leading zero bytes. */
    אם(!certnum)
      infof(data, "   RSA Public Key (%zu bits)", len);
    אם(data->set.ssl.certinfo) {
      حرف r[(حجم(len) * 8 / 3) + 1];
      curl_msnprintf(r, حجم(r), "%zu", len);
      אם(ssl_push_certinfo(data, certnum, "RSA Public Key", r))
        החזר 1;
    }
    /* Generate coefficients. */
    אם(do_pubkey_field(data, certnum, "rsa(n)", &elem))
      החזר 1;
    אם(!getASN1Element(&elem, p, pk.end))
      החזר 1;
    אם(do_pubkey_field(data, certnum, "rsa(e)", &elem))
      החזר 1;
  }
  אחרת אם(curl_strequal(algo, "dsa")) {
    p = getASN1Element(&elem, param->beg, param->end);
    אם(p) {
      אם(do_pubkey_field(data, certnum, "dsa(p)", &elem))
        החזר 1;
      p = getASN1Element(&elem, p, param->end);
      אם(p) {
        אם(do_pubkey_field(data, certnum, "dsa(q)", &elem))
          החזר 1;
        אם(getASN1Element(&elem, p, param->end)) {
          אם(do_pubkey_field(data, certnum, "dsa(g)", &elem))
            החזר 1;
          אם(do_pubkey_field(data, certnum, "dsa(pub_key)", &pk))
            החזר 1;
        }
      }
    }
  }
  אחרת אם(curl_strequal(algo, "dhpublicnumber")) {
    p = getASN1Element(&elem, param->beg, param->end);
    אם(p) {
      אם(do_pubkey_field(data, certnum, "dh(p)", &elem))
        החזר 1;
      אם(getASN1Element(&elem, p, param->end)) {
        אם(do_pubkey_field(data, certnum, "dh(g)", &elem))
          החזר 1;
        אם(do_pubkey_field(data, certnum, "dh(pub_key)", &pk))
          החזר 1;
      }
    }
  }
  החזר 0;
}

/*
 * Convert an ASN.1 distinguished name into a printable string.
 * Return error.
 */
ساكن CURLcode DNtostr(بنية dynbuf *store, بنية Curl_asn1Element *dn)
{
  החזר encodeDN(store, dn);
}

CURLcode Curl_extract_certinfo(بنية Curl_easy *data,
                               صحيح certnum,
                               ثابت uint8_t *beg,
                               ثابت uint8_t *end)
{
  بنية Curl_X509certificate cert;
  بنية Curl_asn1Element param;
  حرف *certptr;
  size_t clen;
  بنية dynbuf out;
  CURLcode result = CURLE_OK;
  unsigned صحيح version;
  ثابت uint8_t *ptr;
  صحيح rc;

  אם(!data->set.ssl.certinfo)
    אם(certnum)
      החזר CURLE_OK;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);
  /* Prepare the certificate information for curl_easy_getinfo(). */

  /* Extract the certificate ASN.1 elements. */
  אם(Curl_parseX509(&cert, beg, end))
    החזר CURLE_PEER_FAILED_VERIFICATION;

  /* Subject. */
  result = DNtostr(&out, &cert.subject);
  אם(result)
    اذهب done;
  אם(data->set.ssl.certinfo) {
    result = ssl_push_certinfo_dyn(data, certnum, "Subject", &out);
    אם(result)
      اذهب done;
  }
  curlx_dyn_reset(&out);

  /* Issuer. */
  result = DNtostr(&out, &cert.issuer);
  אם(result)
    اذهب done;
  אם(data->set.ssl.certinfo) {
    result = ssl_push_certinfo_dyn(data, certnum, "Issuer", &out);
    אם(result)
      اذهب done;
  }
  curlx_dyn_reset(&out);

  /* Version (always fits in less than 32 bits). */
  version = 0;
  עבור(ptr = cert.version.beg; ptr < cert.version.end; ptr++)
    version = (version << 8) | *ptr;
  אם(data->set.ssl.certinfo) {
    result = curlx_dyn_addf(&out, "%x", version);
    אם(result)
      اذهب done;
    result = ssl_push_certinfo_dyn(data, certnum, "Version", &out);
    אם(result)
      اذهب done;
    curlx_dyn_reset(&out);
  }

  /* Serial number. */
  result = ASN1tostr(&out, &cert.serialNumber);
  אם(result)
    اذهب done;
  אם(data->set.ssl.certinfo) {
    result = ssl_push_certinfo_dyn(data, certnum, "Serial Number", &out);
    אם(result)
      اذهب done;
  }
  curlx_dyn_reset(&out);

  /* Signature algorithm .*/
  result = dumpAlgo(&out, &param, cert.signatureAlgorithm.beg,
                    cert.signatureAlgorithm.end);
  אם(result)
    اذهب done;
  אם(data->set.ssl.certinfo) {
    result = ssl_push_certinfo_dyn(data, certnum, "Signature Algorithm", &out);
    אם(result)
      اذهب done;
  }
  curlx_dyn_reset(&out);

  /* Start Date. */
  result = ASN1tostr(&out, &cert.notBefore);
  אם(result)
    اذهب done;
  אם(data->set.ssl.certinfo) {
    result = ssl_push_certinfo_dyn(data, certnum, "Start Date", &out);
    אם(result)
      اذهب done;
  }
  curlx_dyn_reset(&out);

  /* Expire Date. */
  result = ASN1tostr(&out, &cert.notAfter);
  אם(result)
    اذهب done;
  אם(data->set.ssl.certinfo) {
    result = ssl_push_certinfo_dyn(data, certnum, "Expire Date", &out);
    אם(result)
      اذهب done;
  }
  curlx_dyn_reset(&out);

  /* Public Key Algorithm. */
  result = dumpAlgo(&out, &param, cert.subjectPublicKeyAlgorithm.beg,
                    cert.subjectPublicKeyAlgorithm.end);
  אם(result)
    اذهب done;
  אם(data->set.ssl.certinfo) {
    result = ssl_push_certinfo_dyn(data, certnum, "Public Key Algorithm",
                                   &out);
    אם(result)
      اذهب done;
  }

  rc = do_pubkey(data, certnum, curlx_dyn_ptr(&out),
                 &param, &cert.subjectPublicKey);
  אם(rc) {
    result = CURLE_OUT_OF_MEMORY; /* the most likely error */
    اذهب done;
  }
  curlx_dyn_reset(&out);

  /* Signature. */
  result = ASN1tostr(&out, &cert.signature);
  אם(result)
    اذهب done;
  אם(data->set.ssl.certinfo) {
    result = ssl_push_certinfo_dyn(data, certnum, "Signature", &out);
    אם(result)
      اذهب done;
  }
  curlx_dyn_reset(&out);

  /* Generate PEM certificate. */
  result = curlx_base64_encode((ثابت uint8_t *)cert.certificate.beg,
                               cert.certificate.end - cert.certificate.beg,
                               &certptr, &clen);
  אם(result)
    اذهب done;

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
  אם(!result) {
    size_t j = 0;

    כלעוד(!result && (j < clen)) {
      size_t chunksize = (clen - j) > 64 ? 64 : (clen - j);
      result = curlx_dyn_addn(&out, &certptr[j], chunksize);
      אם(!result)
        result = curlx_dyn_addn(&out, "\n", 1);
      j += chunksize;
    }
    אם(!result)
      result = curlx_dyn_add(&out, "-----END CERTIFICATE-----\n");
  }
  curlx_free(certptr);
  אם(!result)
    אם(data->set.ssl.certinfo)
      result = ssl_push_certinfo_dyn(data, certnum, "Cert", &out);

done:
  אם(result)
    failf(data, "Failed extracting certificate chain");
  curlx_dyn_free(&out);
  החזר result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#endif /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
