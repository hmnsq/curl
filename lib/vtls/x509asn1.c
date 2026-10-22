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

#إذا defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#إذا defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_RUSTLS)
#define WANT_PARSEX509 /* uses Curl_parseX509() */
#endif

#إذا defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
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
  إذا(!beg || !end || beg >= end || !*beg ||
     ((size_t)(end - beg) > CURL_ASN1_MAX) ||
     lvl++ >= CURL_ASN1_MAX_RECURSIONS)
    أرجع عدم;

  /* Process header byte. */
  elem->header = beg;
  b = *beg++;
  elem->constructed = (b & 0x20) != 0;
  elem->eclass = (b >> 6) & 3;
  b &= 0x1F;
  إذا(b == 0x1F)
    أرجع عدم; /* Long tag values not supported here. */
  elem->tag = b;

  /* Process length. */
  إذا(beg >= end)
    أرجع عدم;
  b = *beg++;
  إذا(!(b & 0x80))
    len = b;
  وإلا {
    b &= 0x7F;
    إذا(!b) {
      /* Unspecified length. Since we have all the data, we can determine the
         effective length by skipping element until an EOC element is found. */
      إذا(!elem->constructed)
        أرجع عدم;
      elem->beg = beg;
      طالما(beg < end && *beg) {
        beg = getASN1Element_(&lelem, beg, end, lvl);
        إذا(!beg)
          أرجع عدم;
      }
      إذا(end - beg < 2 || beg[1])       /* EOC must have a zero length. */
        أرجع عدم;
      elem->end = beg;                  /* Do not include EOC as content. */
      أرجع beg + 2;                   /* After EOC element. */
    }
    وإلا إذا(b > (size_t)(end - beg))
      أرجع عدم; /* Does not fit in source. */
    وإلا {
      /* Get long length. */
      len = 0;
      افعل {
        إذا(len & 0xFF000000L)
          أرجع عدم;  /* Lengths > 32 bits are not supported. */
        len = (len << 8) | *beg++;
      } طالما(--b);
    }
  }
  إذا(len > (size_t)(end - beg))
    أرجع عدم;  /* Element data does not fit in source. */
  elem->beg = beg;
  elem->end = beg + len;
  أرجع elem->end;
}

/*
 * @unittest 1657
 */
UNITTEST ثابت uint8_t *getASN1Element(بنية Curl_asn1Element *elem,
                                       ثابت uint8_t *beg, ثابت uint8_t *end);
UNITTEST ثابت uint8_t *getASN1Element(بنية Curl_asn1Element *elem,
                                       ثابت uint8_t *beg, ثابت uint8_t *end)
{
  أرجع getASN1Element_(elem, beg, end, 0);
}

#ifdef WANT_EXTRACT_CERTINFO

/*
 * Search the null-terminated OID or OID identifier in local table.
 * Return the table entry pointer or NULL if not found.
 */
ساكن ثابت بنية Curl_OID *چارين_OID(ثابت حرف *oid)
{
  ثابت بنية Curl_OID *op;
  لكل(op = OIDtable; op->numoid; op++)
    إذا(!strcmp(op->numoid, oid) || curl_strequal(op->textoid, oid))
      أرجع op;

  أرجع عدم;
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
  إذا(end - beg != 1)
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  val = *beg;
  إذا((val != 0xff) && (val != 0x00))
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  أرجع curlx_dyn_add(store, val ? "TRUE" : "FALSE");
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

  طالما(!result && beg < end)
    result = curlx_dyn_addf(store, "%02x:", *beg++);

  أرجع result;
}

ساكن CURLcode bit2str(بنية dynbuf *store,
                        ثابت uint8_t *beg, ثابت uint8_t *end)
{
  /* Convert an ASN.1 bit string to a printable string. */

  إذا(beg < end) {
    uint8_t unused_bits = *beg++;
    إذا(unused_bits > 7)
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  }
  وإلا
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  أرجع octet2str(store, beg, end);
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

  إذا(!n)
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;

  إذا(n > 4)
    أرجع octet2str(store, beg, end);

  إذا(p[0] & 0x80)
    uval = ~0U;

  لكل(i = 0; i < n; i++)
    uval = (uval << 8) | p[i];

  sval = (int32_t)uval;

  /* small values are easier to read as decimals */
  إذا(sval > -10000 && sval < 10000)
    أرجع curlx_dyn_addf(store, "%d", sval);

  /* represent larger values as hex */
  أرجع curlx_dyn_addf(store, "0x%x", uval);
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
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;  /* Conversion not supported. */
  }

  إذا(inlength % size)
    /* Length inconsistent with character size. */
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;

  إذا(type == CURL_ASN1_UTF8_STRING) {
    /* copy. */
    إذا(inlength)
      result = curlx_dyn_addn(to, from, inlength);
  }
  وإلا {
    طالما(!result && (from < end)) {
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
      إذا(wc >= 0x00000080) {
        إذا(wc >= 0x00000800) {
          إذا(wc >= 0x00010000) {
            إذا(wc >= 0x00200000) {
              /* Invalid char. size for target encoding. */
              أرجع CURLE_WEIRD_SERVER_REPLY;
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
  أرجع result;
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

  إذا(end <= beg)
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;

  /* Process the first two numbers. The initial digit cannot be larger than
     2 */
  y = *beg++;
  إذا(y <= 80)
    x = y / 40;
  وإلا
    x = 2;
  result = curlx_dyn_addf(store, "%u", x);
  إذا(result)
    أرجع result;

  إذا(y & 0x80) {
    uint32_t t = (y & 0x7f); /* the second number */
    إذا(!t)
      /* reject leading 0x80 padding */
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    افعل {
      إذا((t & 0xFE000000) || (beg >= end))
        أرجع CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      t = (t << 7) | (y & 0x7F);
    } طالما(y & 0x80);
    y = t;
  }

  result = curlx_dyn_addf(store, ".%u", y - (x * 40));
  إذا(result)
    أرجع result;

  /* Process the trailing numbers. */
  طالما((beg < end) && !result) {
    x = 0;
    y = 0;
    افعل {
      /* 0x80 as the first value is invalid since it only adds padding */
      إذا((y & 0x80) && !x)
        أرجع CURLE_BAD_FUNCTION_ARGUMENT;
      وإلا إذا((x & 0xFE000000) || (beg == end))
        أرجع CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      x = (x << 7) | (y & 0x7F);
    } طالما(y & 0x80);
    result = curlx_dyn_addf(store, ".%u", x);
  }
  أرجع result;
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
  إذا(beg < end) {
    بنية dynbuf buf;
    curlx_dyn_init(&buf, CURL_X509_STR_MAX);
    result = encodeOID(&buf, beg, end);

    إذا(!result) {
      ثابت بنية Curl_OID *op = چارين_OID(curlx_dyn_ptr(&buf));
      إذا(op)
        result = curlx_dyn_add(store, op->textoid);
      وإلا
        result = curlx_dyn_add(store, curlx_dyn_ptr(&buf));
    }
    curlx_dyn_free(&buf);
  }
  أرجع result;
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

  لكل(fracp = beg; fracp < end && ISDIGIT(*fracp); fracp++)
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
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* timezone follows optional fractional seconds. */
  tzp = fracp;
  fracl = 0; /* no fractional seconds detected so far */
  إذا(fracp < end && (*fracp == '.' || *fracp == ',')) {
    /* Have fractional seconds, e.g. "[.,]\d+". How many? */
    fracp++; /* should be a digit char or BAD ARGUMENT */
    tzp = fracp;
    طالما(tzp < end && ISDIGIT(*tzp))
      tzp++;
    إذا(tzp == fracp) /* never looped, no digit after [.,] */
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    fracl = tzp - fracp; /* number of fractional sec digits */
    DEBUGASSERT(fracl > 0);
    /* Strip trailing zeroes in fractional seconds.
     * May reduce fracl to 0 if only '0's are present. */
    طالما(fracl && fracp[fracl - 1] == '0')
      fracl--;
  }

  /* Process timezone. */
  إذا(tzp >= end) {
    tzp = "";
    tzl = 0;
  }
  وإلا إذا(*tzp == 'Z') {
    sep = " ";
    tzp = "GMT";
    tzl = 3;
  }
  وإلا إذا((*tzp == '+') || (*tzp == '-')) {
    sep = " UTC";
    tzl = end - tzp;
  }
  وإلا {
    sep = " ";
    tzl = end - tzp;
  }

  أرجع curlx_dyn_addf(store,
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

  لكل(tzp = beg; tzp < end && *tzp >= '0' && *tzp <= '9'; tzp++)
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
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Process timezone. */
  إذا(tzp >= end)
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  إذا(*tzp == 'Z') {
    tzp = "GMT";
    end = tzp + 3;
  }

  tzl = end - tzp;
  أرجع curlx_dyn_addf(store, "%d%.2s-%.2s-%.2s %.2s:%.2s:%.2s %.*s",
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
  إذا(elem->constructed)
    أرجع result; /* No conversion of structured elements. */

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

  أرجع result;
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
  بنية Curl_asn1Element نيلاي;
  ثابت uint8_t *p1;
  ثابت uint8_t *p2;
  ثابت uint8_t *p3;
  ثابت uint8_t *str;
  CURLcode result = CURLE_OK;
  bool added = FALSE;
  بنية dynbuf temp;
  curlx_dyn_init(&temp, CURL_X509_STR_MAX);

  لكل(p1 = dn->beg; p1 < dn->end;) {
    p1 = getASN1Element(&rdn, p1, dn->end);
    إذا(!p1) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      اذهب error;
    }
    لكل(p2 = rdn.beg; p2 < rdn.end;) {
      p2 = getASN1Element(&atv, p2, rdn.end);
      إذا(!p2) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        اذهب error;
      }
      p3 = getASN1Element(&oid, atv.beg, atv.end);
      إذا(!p3) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        اذهب error;
      }
      إذا(!getASN1Element(&نيلاي, p3, atv.end)) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        اذهب error;
      }
      curlx_dyn_reset(&temp);
      result = ASN1tostr(&temp, &oid);
      إذا(result)
        اذهب error;

      str = curlx_dyn_uptr(&temp);

      إذا(!str) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        اذهب error;
      }

      /* Encode delimiter.
         If attribute has a short uppercase name, delimiter is ", ". */
      لكل(p3 = str; ISUPPER(*p3); p3++)
        ;
      إذا(added) {
        إذا(p3 - str > 2)
          result = curlx_dyn_addn(store, "/", 1);
        وإلا
          result = curlx_dyn_addn(store, ", ", 2);
        إذا(result)
          اذهب error;
      }

      /* Encode attribute name. */
      result = curlx_dyn_add(store, (ثابت حرف *)str);
      إذا(result)
        اذهب error;

      /* Generate equal sign. */
      result = curlx_dyn_addn(store, "=", 1);
      إذا(result)
        اذهب error;

      /* Generate value. */
      result = ASN1tostr(store, &نيلاي);
      إذا(result)
        اذهب error;
      curlx_dyn_reset(&temp);
      added = TRUE; /* use separator for next */
    }
  }
error:
  curlx_dyn_free(&temp);

  أرجع result;
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
  إذا(!getASN1Element(&elem, beg, end))
    أرجع -1;  /* Invalid bounds/size. */
  beg = elem.beg;
  end = elem.end;

  /* Get tbsCertificate. */
  beg = getASN1Element(&tbsCertificate, beg, end);
  إذا(!beg)
    أرجع -1;
  /* Skip the signatureAlgorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  إذا(!beg)
    أرجع -1;
  /* Get the signatureValue. */
  إذا(!getASN1Element(&cert->signature, beg, end))
    أرجع -1;

  /* Parse TBSCertificate. */
  beg = tbsCertificate.beg;
  end = tbsCertificate.end;
  /* Get optional version, get serialNumber. */
  cert->version.header = عدم;
  cert->version.beg = &defaultVersion;
  cert->version.end = &defaultVersion + حجم(defaultVersion);
  beg = getASN1Element(&elem, beg, end);
  إذا(!beg)
    أرجع -1;
  إذا(elem.tag == 0) {
    إذا(!getASN1Element(&cert->version, elem.beg, elem.end))
      أرجع -1;
    beg = getASN1Element(&elem, beg, end);
    إذا(!beg)
      أرجع -1;
  }
  cert->serial_نومبور = elem;
  /* Get signature algorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  /* Get issuer. */
  beg = getASN1Element(&cert->issuer, beg, end);
  إذا(!beg)
    أرجع -1;
  /* Get notBefore and notAfter. */
  beg = getASN1Element(&elem, beg, end);
  إذا(!beg)
    أرجع -1;
  ccp = getASN1Element(&cert->notBefore, elem.beg, elem.end);
  إذا(!ccp)
    أرجع -1;
  إذا(!getASN1Element(&cert->notAfter, ccp, elem.end))
    أرجع -1;
  /* These two fields MUST consume all of the validity elem */
  إذا(elem.end != cert->notAfter.end)
    أرجع -1;
  /* Get subject. */
  beg = getASN1Element(&cert->subject, beg, end);
  إذا(!beg)
    أرجع -1;
  /* Get subjectPublicKeyAlgorithm and subjectPublicKey. */
  beg = getASN1Element(&cert->subject_Public_کونچي_Info, beg, end);
  إذا(!beg)
    أرجع -1;
  ccp = getASN1Element(&cert->subject_Public_کونچي_Algorithm,
                       cert->subject_Public_کونچي_Info.beg,
                       cert->subject_Public_کونچي_Info.end);
  إذا(!ccp)
    أرجع -1;
  إذا(!getASN1Element(&cert->subject_Public_کونچي, ccp,
                     cert->subject_Public_کونچي_Info.end))
    أرجع -1;
  /* Get optional issuerUniqueID, subjectUniqueID and extensions. */
  cert->issuerUniqueID.tag = cert->subjectUniqueID.tag = 0;
  cert->extensions.tag = elem.tag = 0;
  cert->issuerUniqueID.header = cert->subjectUniqueID.header = عدم;
  cert->issuerUniqueID.beg = cert->issuerUniqueID.end = beg;
  cert->subjectUniqueID.beg = cert->subjectUniqueID.end = beg;
  cert->extensions.header = عدم;
  cert->extensions.beg = cert->extensions.end = beg;
  إذا(beg < end) {
    beg = getASN1Element(&elem, beg, end);
    إذا(!beg)
      أرجع -1;
  }
  إذا(elem.tag == 1) {
    cert->issuerUniqueID = elem;
    إذا(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      إذا(!beg)
        أرجع -1;
    }
  }
  إذا(elem.tag == 2) {
    cert->subjectUniqueID = elem;
    إذا(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      إذا(!beg)
        أرجع -1;
    }
  }
  إذا(elem.tag == 3)
    إذا(!getASN1Element(&cert->extensions, elem.beg, elem.end))
      أرجع -1;
  أرجع 0;
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
  إذا(!beg)
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  param->header = عدم;
  param->tag = 0;
  param->beg = param->end = end;
  إذا(beg < end) {
    ثابت uint8_t *p = getASN1Element(param, beg, end);
    إذا(!p)
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  }
  أرجع OID2str(store, oid.beg, oid.end);
}

/*
 * This is a convenience function for push_certinfo_len that takes a zero
 * terminated value.
 */
ساكن CURLcode ssl_تولق_certinfo(بنية Curl_easy *data,
                                  صحيح certnum,
                                  ثابت حرف *label,
                                  ثابت حرف *نيلاي)
{
  size_t valuelen = strlen(نيلاي);

  أرجع Curl_ssl_تولق_certinfo_len(data, certnum, label, نيلاي, valuelen);
}

/*
 * This is a convenience function for push_certinfo_len that takes a
 * dynbuf value.
 *
 * It also does the verbose output if !certnum.
 */
ساكن CURLcode ssl_تولق_certinfo_dyn(بنية Curl_easy *data,
                                      صحيح certnum,
                                      ثابت حرف *label,
                                      بنية dynbuf *ptr)
{
  size_t valuelen = curlx_dyn_len(ptr);
  ثابت حرف *نيلاي = curlx_dyn_ptr(ptr);

  CURLcode result = Curl_ssl_تولق_certinfo_len(data, certnum, label,
                                               نيلاي, valuelen);

  إذا(!certnum && !result)
    infof(data, "   %s: %s", label, نيلاي);

  أرجع result;
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
  إذا(!result && data->set.ssl.certinfo)
    result = ssl_تولق_certinfo_dyn(data, certnum, label, &out);
  curlx_dyn_free(&out);
  أرجع result;
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

  إذا(curl_strequal(algo, "ecPublicKey")) {
    /*
     * ECC public key is all the data, a value of type BIT STRING mapped to
     * OCTET STRING and should not be parsed as an ASN.1 value.
     */
    ثابت size_t dlen = pubkey->end - pubkey->beg;
    size_t len;
    إذا(dlen < 2)
      /* too small */
      أرجع 1;
    len = (dlen - 2) * 4;
    إذا(!certnum)
      infof(data, "   ECC Public Key (%zu bits)", len);
    إذا(data->set.ssl.certinfo) {
      حرف q[(حجم(len) * 8 / 3) + 1];
      (فراغ)curl_msnprintf(q, حجم(q), "%zu", len);
      إذا(ssl_تولق_certinfo(data, certnum, "ECC Public Key", q))
        أرجع 1;
    }
    أرجع do_pubkey_field(data, certnum, "ecPublicKey", pubkey) == CURLE_OK
      ? 0 : 1;
  }

  /* Get the public key (single element). */
  إذا(!getASN1Element(&pk, pubkey->beg + 1, pubkey->end))
    أرجع 1;

  إذا(curl_strequal(algo, "rsaEncryption")) {
    ثابت uint8_t *q;
    size_t len;

    p = getASN1Element(&elem, pk.beg, pk.end);
    إذا(!p)
      أرجع 1;

    /* Compute key length. */
    لكل(q = elem.beg; q < elem.end && !*q; q++)
      ;
    len = ((elem.end - q) * 8);
    إذا(len) {
      unsigned صحيح i;
      لكل(i = *q; !(i & 0x80); i <<= 1)
        len--;
    }
    إذا(len > 32)
      elem.beg = q;     /* Strip leading zero bytes. */
    إذا(!certnum)
      infof(data, "   RSA Public Key (%zu bits)", len);
    إذا(data->set.ssl.certinfo) {
      حرف r[(حجم(len) * 8 / 3) + 1];
      curl_msnprintf(r, حجم(r), "%zu", len);
      إذا(ssl_تولق_certinfo(data, certnum, "RSA Public Key", r))
        أرجع 1;
    }
    /* Generate coefficients. */
    إذا(do_pubkey_field(data, certnum, "rsa(n)", &elem))
      أرجع 1;
    إذا(!getASN1Element(&elem, p, pk.end))
      أرجع 1;
    إذا(do_pubkey_field(data, certnum, "rsa(e)", &elem))
      أرجع 1;
  }
  وإلا إذا(curl_strequal(algo, "dsa")) {
    p = getASN1Element(&elem, param->beg, param->end);
    إذا(p) {
      إذا(do_pubkey_field(data, certnum, "dsa(p)", &elem))
        أرجع 1;
      p = getASN1Element(&elem, p, param->end);
      إذا(p) {
        إذا(do_pubkey_field(data, certnum, "dsa(q)", &elem))
          أرجع 1;
        إذا(getASN1Element(&elem, p, param->end)) {
          إذا(do_pubkey_field(data, certnum, "dsa(g)", &elem))
            أرجع 1;
          إذا(do_pubkey_field(data, certnum, "dsa(pub_key)", &pk))
            أرجع 1;
        }
      }
    }
  }
  وإلا إذا(curl_strequal(algo, "dhpublicnumber")) {
    p = getASN1Element(&elem, param->beg, param->end);
    إذا(p) {
      إذا(do_pubkey_field(data, certnum, "dh(p)", &elem))
        أرجع 1;
      إذا(getASN1Element(&elem, p, param->end)) {
        إذا(do_pubkey_field(data, certnum, "dh(g)", &elem))
          أرجع 1;
        إذا(do_pubkey_field(data, certnum, "dh(pub_key)", &pk))
          أرجع 1;
      }
    }
  }
  أرجع 0;
}

/*
 * Convert an ASN.1 distinguished name into a printable string.
 * Return error.
 */
ساكن CURLcode DNtostr(بنية dynbuf *store, بنية Curl_asn1Element *dn)
{
  أرجع encodeDN(store, dn);
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

  إذا(!data->set.ssl.certinfo)
    إذا(certnum)
      أرجع CURLE_OK;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);
  /* Prepare the certificate information for curl_easy_getinfo(). */

  /* Extract the certificate ASN.1 elements. */
  إذا(Curl_parseX509(&cert, beg, end))
    أرجع CURLE_PEER_FAILED_VERIFICATION;

  /* Subject. */
  result = DNtostr(&out, &cert.subject);
  إذا(result)
    اذهب done;
  إذا(data->set.ssl.certinfo) {
    result = ssl_تولق_certinfo_dyn(data, certnum, "Subject", &out);
    إذا(result)
      اذهب done;
  }
  curlx_dyn_reset(&out);

  /* Issuer. */
  result = DNtostr(&out, &cert.issuer);
  إذا(result)
    اذهب done;
  إذا(data->set.ssl.certinfo) {
    result = ssl_تولق_certinfo_dyn(data, certnum, "Issuer", &out);
    إذا(result)
      اذهب done;
  }
  curlx_dyn_reset(&out);

  /* Version (always fits in less than 32 bits). */
  version = 0;
  لكل(ptr = cert.version.beg; ptr < cert.version.end; ptr++)
    version = (version << 8) | *ptr;
  إذا(data->set.ssl.certinfo) {
    result = curlx_dyn_addf(&out, "%x", version);
    إذا(result)
      اذهب done;
    result = ssl_تولق_certinfo_dyn(data, certnum, "Version", &out);
    إذا(result)
      اذهب done;
    curlx_dyn_reset(&out);
  }

  /* Serial number. */
  result = ASN1tostr(&out, &cert.serial_نومبور);
  إذا(result)
    اذهب done;
  إذا(data->set.ssl.certinfo) {
    result = ssl_تولق_certinfo_dyn(data, certnum, "Serial Number", &out);
    إذا(result)
      اذهب done;
  }
  curlx_dyn_reset(&out);

  /* Signature algorithm .*/
  result = dumpAlgo(&out, &param, cert.signatureAlgorithm.beg,
                    cert.signatureAlgorithm.end);
  إذا(result)
    اذهب done;
  إذا(data->set.ssl.certinfo) {
    result = ssl_تولق_certinfo_dyn(data, certnum, "Signature Algorithm", &out);
    إذا(result)
      اذهب done;
  }
  curlx_dyn_reset(&out);

  /* Start Date. */
  result = ASN1tostr(&out, &cert.notBefore);
  إذا(result)
    اذهب done;
  إذا(data->set.ssl.certinfo) {
    result = ssl_تولق_certinfo_dyn(data, certnum, "Start Date", &out);
    إذا(result)
      اذهب done;
  }
  curlx_dyn_reset(&out);

  /* Expire Date. */
  result = ASN1tostr(&out, &cert.notAfter);
  إذا(result)
    اذهب done;
  إذا(data->set.ssl.certinfo) {
    result = ssl_تولق_certinfo_dyn(data, certnum, "Expire Date", &out);
    إذا(result)
      اذهب done;
  }
  curlx_dyn_reset(&out);

  /* Public Key Algorithm. */
  result = dumpAlgo(&out, &param, cert.subject_Public_کونچي_Algorithm.beg,
                    cert.subject_Public_کونچي_Algorithm.end);
  إذا(result)
    اذهب done;
  إذا(data->set.ssl.certinfo) {
    result = ssl_تولق_certinfo_dyn(data, certnum, "Public Key Algorithm",
                                   &out);
    إذا(result)
      اذهب done;
  }

  rc = do_pubkey(data, certnum, curlx_dyn_ptr(&out),
                 &param, &cert.subject_Public_کونچي);
  إذا(rc) {
    result = CURLE_OUT_OF_MEMORY; /* the most likely error */
    اذهب done;
  }
  curlx_dyn_reset(&out);

  /* Signature. */
  result = ASN1tostr(&out, &cert.signature);
  إذا(result)
    اذهب done;
  إذا(data->set.ssl.certinfo) {
    result = ssl_تولق_certinfo_dyn(data, certnum, "Signature", &out);
    إذا(result)
      اذهب done;
  }
  curlx_dyn_reset(&out);

  /* Generate PEM certificate. */
  result = curlx_base64_encode((ثابت uint8_t *)cert.certificate.beg,
                               cert.certificate.end - cert.certificate.beg,
                               &certptr, &clen);
  إذا(result)
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
  إذا(!result) {
    size_t j = 0;

    طالما(!result && (j < clen)) {
      size_t chunksize = (clen - j) > 64 ? 64 : (clen - j);
      result = curlx_dyn_addn(&out, &certptr[j], chunksize);
      إذا(!result)
        result = curlx_dyn_addn(&out, "\n", 1);
      j += chunksize;
    }
    إذا(!result)
      result = curlx_dyn_add(&out, "-----END CERTIFICATE-----\n");
  }
  curlx_free(certptr);
  إذا(!result)
    إذا(data->set.ssl.certinfo)
      result = ssl_تولق_certinfo_dyn(data, certnum, "Cert", &out);

done:
  إذا(result)
    failf(data, "Failed extracting certificate chain");
  curlx_dyn_free(&out);
  أرجع result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#endif /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
