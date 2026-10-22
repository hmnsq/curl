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
#include "curl_setup.h"

#如果 defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#如果 defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_RUSTLS)
#define WANT_PARSEX509 /* uses Curl_parseX509() */
#endif

#如果 defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_RUSTLS)
#define WANT_EXTRACT_CERTINFO /* uses Curl_extract_certinfo() */
#endif

#include "urldata.h"
#include "vtls/vtls.h"
#include "curl_trc.h"
#include "curlx/base64.h"
#include "vtls/x509asn1.h"
#include "curlx/dynbuf.h"

/*
 * Constants.
 */

/* Largest supported ASN.1 structure. */
#define CURL_ASN1_MAX                   ((size_t)0x40000)      /* 256K */

#ifdef WANT_EXTRACT_CERTINFO

/* ASN.1 OID table entry. */
struct Curl_OID {
  const char *numoid;  /* Dotted-numeric OID. */
  const char *textoid; /* OID name. */
};

/* ASN.1 OIDs. */
static const struct Curl_OID OIDtable[] = {
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
  { NULL,                       NULL }
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

static const uint8_t *getASN1Element_(struct Curl_asn1Element *elem,
                                      const uint8_t *beg, const uint8_t *end,
                                      size_t lvl)
{
  uint8_t b;
  size_t len;
  struct Curl_asn1Element lelem;

  /* Get a single ASN.1 element into `elem', parse ASN.1 string at `beg'
     ending at `end'.
     Returns a pointer in source string after the parsed element, or NULL
     if an error occurs. */
  如果(!beg || !end || beg >= end || !*beg ||
     ((size_t)(end - beg) > CURL_ASN1_MAX) ||
     lvl++ >= CURL_ASN1_MAX_RECURSIONS)
    返回 NULL;

  /* Process header byte. */
  elem->header = beg;
  b = *beg++;
  elem->constructed = (b & 0x20) != 0;
  elem->eclass = (b >> 6) & 3;
  b &= 0x1F;
  如果(b == 0x1F)
    返回 NULL; /* Long tag values not supported here. */
  elem->tag = b;

  /* Process length. */
  如果(beg >= end)
    返回 NULL;
  b = *beg++;
  如果(!(b & 0x80))
    len = b;
  否则 {
    b &= 0x7F;
    如果(!b) {
      /* Unspecified length. Since we have all the data, we can determine the
         effective length by skipping element until an EOC element is found. */
      如果(!elem->constructed)
        返回 NULL;
      elem->beg = beg;
      当(beg < end && *beg) {
        beg = getASN1Element_(&lelem, beg, end, lvl);
        如果(!beg)
          返回 NULL;
      }
      如果(end - beg < 2 || beg[1])       /* EOC must have a zero length. */
        返回 NULL;
      elem->end = beg;                  /* Do not include EOC as content. */
      返回 beg + 2;                   /* After EOC element. */
    }
    否则 如果(b > (size_t)(end - beg))
      返回 NULL; /* Does not fit in source. */
    否则 {
      /* Get long length. */
      len = 0;
      do {
        如果(len & 0xFF000000L)
          返回 NULL;  /* Lengths > 32 bits are not supported. */
        len = (len << 8) | *beg++;
      } 当(--b);
    }
  }
  如果(len > (size_t)(end - beg))
    返回 NULL;  /* Element data does not fit in source. */
  elem->beg = beg;
  elem->end = beg + len;
  返回 elem->end;
}

/*
 * @unittest 1657
 */
UNITTEST const uint8_t *getASN1Element(struct Curl_asn1Element *elem,
                                       const uint8_t *beg, const uint8_t *end);
UNITTEST const uint8_t *getASN1Element(struct Curl_asn1Element *elem,
                                       const uint8_t *beg, const uint8_t *end)
{
  返回 getASN1Element_(elem, beg, end, 0);
}

#ifdef WANT_EXTRACT_CERTINFO

/*
 * Search the null-terminated OID or OID identifier in local table.
 * Return the table entry pointer or NULL if not found.
 */
static const struct Curl_OID *searchOID(const char *oid)
{
  const struct Curl_OID *op;
  为(op = OIDtable; op->numoid; op++)
    如果(!strcmp(op->numoid, oid) || curl_strequal(op->textoid, oid))
      返回 op;

  返回 NULL;
}

/*
 * Convert an ASN.1 Boolean value into its string representation.
 *
 * Return error code.
 */

static CURLcode bool2str(struct dynbuf *store,
                         const uint8_t *beg, const uint8_t *end)
{
  uint8_t val;
  如果(end - beg != 1)
    返回 CURLE_BAD_FUNCTION_ARGUMENT;
  val = *beg;
  如果((val != 0xff) && (val != 0x00))
    返回 CURLE_BAD_FUNCTION_ARGUMENT;
  返回 curlx_dyn_add(store, val ? "TRUE" : "FALSE");
}

/*
 * Convert an ASN.1 octet string to a printable string.
 *
 * Return error code.
 */
static CURLcode octet2str(struct dynbuf *store,
                          const uint8_t *beg, const uint8_t *end)
{
  CURLcode result = CURLE_OK;

  当(!result && beg < end)
    result = curlx_dyn_addf(store, "%02x:", *beg++);

  返回 result;
}

static CURLcode bit2str(struct dynbuf *store,
                        const uint8_t *beg, const uint8_t *end)
{
  /* Convert an ASN.1 bit string to a printable string. */

  如果(beg < end) {
    uint8_t unused_bits = *beg++;
    如果(unused_bits > 7)
      返回 CURLE_BAD_FUNCTION_ARGUMENT;
  }
  否则
    返回 CURLE_BAD_FUNCTION_ARGUMENT;
  返回 octet2str(store, beg, end);
}

/*
 * Convert an ASN.1 integer value into its string representation.
 *
 * Returns error.
 */
static CURLcode int2str(struct dynbuf *store,
                        const uint8_t *beg, const uint8_t *end)
{
  uint32_t uval = 0;
  int32_t sval = 0;
  size_t n = end - beg;
  size_t i;
  const uint8_t *p = beg;

  如果(!n)
    返回 CURLE_BAD_FUNCTION_ARGUMENT;

  如果(n > 4)
    返回 octet2str(store, beg, end);

  如果(p[0] & 0x80)
    uval = ~0U;

  为(i = 0; i < n; i++)
    uval = (uval << 8) | p[i];

  sval = (int32_t)uval;

  /* small values are easier to read as decimals */
  如果(sval > -10000 && sval < 10000)
    返回 curlx_dyn_addf(store, "%d", sval);

  /* represent larger values as hex */
  返回 curlx_dyn_addf(store, "0x%x", uval);
}

/*
 * Convert from an ASN.1 typed string to UTF8.
 *
 * The result is stored in a dynbuf that is inited by the user of this
 * function.
 *
 * Returns error.
 */
static CURLcode utf8asn1str(struct dynbuf *to, 整数 type,
                            const uint8_t *from, const uint8_t *end)
{
  size_t inlength = end - from;
  整数 size = 1;
  CURLcode result = CURLE_OK;

  switch(type) {
  case CURL_ASN1_BMP_STRING:
    size = 2;
    break;
  case CURL_ASN1_UNIVERSAL_STRING:
    size = 4;
    break;
  case CURL_ASN1_NUMERIC_STRING:
  case CURL_ASN1_PRINTABLE_STRING:
  case CURL_ASN1_TELETEX_STRING:
  case CURL_ASN1_IA5_STRING:
  case CURL_ASN1_VISIBLE_STRING:
  case CURL_ASN1_UTF8_STRING:
    break;
  default:
    返回 CURLE_BAD_FUNCTION_ARGUMENT;  /* Conversion not supported. */
  }

  如果(inlength % size)
    /* Length inconsistent with character size. */
    返回 CURLE_BAD_FUNCTION_ARGUMENT;

  如果(type == CURL_ASN1_UTF8_STRING) {
    /* copy. */
    如果(inlength)
      result = curlx_dyn_addn(to, from, inlength);
  }
  否则 {
    当(!result && (from < end)) {
      char buf[4]; /* decode buffer */
      size_t charsize = 1;
      unsigned 整数 wc = 0;

      switch(size) {
      case 4:
        wc = (wc << 8) | *from++;
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      case 2:
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      default: /* case 1: */
        wc = (wc << 8) | *from++;
      }
      如果(wc >= 0x00000080) {
        如果(wc >= 0x00000800) {
          如果(wc >= 0x00010000) {
            如果(wc >= 0x00200000) {
              /* Invalid char. size for target encoding. */
              返回 CURLE_WEIRD_SERVER_REPLY;
            }
            buf[3] = (char)(0x80 | (wc & 0x3F));
            wc = (wc >> 6) | 0x00010000;
            charsize++;
          }
          buf[2] = (char)(0x80 | (wc & 0x3F));
          wc = (wc >> 6) | 0x00000800;
          charsize++;
        }
        buf[1] = (char)(0x80 | (wc & 0x3F));
        wc = (wc >> 6) | 0x000000C0;
        charsize++;
      }
      buf[0] = (char)wc;
      result = curlx_dyn_addn(to, buf, charsize);
    }
  }
  返回 result;
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
UNITTEST CURLcode encodeOID(struct dynbuf *store,
                            const uint8_t *beg, const uint8_t *end);
UNITTEST CURLcode encodeOID(struct dynbuf *store,
                            const uint8_t *beg, const uint8_t *end)
{
  uint32_t x;
  uint32_t y;
  CURLcode result = CURLE_OK;

  如果(end <= beg)
    返回 CURLE_BAD_FUNCTION_ARGUMENT;

  /* Process the first two numbers. The initial digit cannot be larger than
     2 */
  y = *beg++;
  如果(y <= 80)
    x = y / 40;
  否则
    x = 2;
  result = curlx_dyn_addf(store, "%u", x);
  如果(result)
    返回 result;

  如果(y & 0x80) {
    uint32_t t = (y & 0x7f); /* the second number */
    如果(!t)
      /* reject leading 0x80 padding */
      返回 CURLE_BAD_FUNCTION_ARGUMENT;
    do {
      如果((t & 0xFE000000) || (beg >= end))
        返回 CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      t = (t << 7) | (y & 0x7F);
    } 当(y & 0x80);
    y = t;
  }

  result = curlx_dyn_addf(store, ".%u", y - (x * 40));
  如果(result)
    返回 result;

  /* Process the trailing numbers. */
  当((beg < end) && !result) {
    x = 0;
    y = 0;
    do {
      /* 0x80 as the first value is invalid since it only adds padding */
      如果((y & 0x80) && !x)
        返回 CURLE_BAD_FUNCTION_ARGUMENT;
      否则 如果((x & 0xFE000000) || (beg == end))
        返回 CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      x = (x << 7) | (y & 0x7F);
    } 当(y & 0x80);
    result = curlx_dyn_addf(store, ".%u", x);
  }
  返回 result;
}

/*
 * Convert an ASN.1 OID into its dotted or symbolic string representation.
 *
 * Return error code.
 */

static CURLcode OID2str(struct dynbuf *store,
                        const uint8_t *beg, const uint8_t *end)
{
  CURLcode result = CURLE_OK;
  如果(beg < end) {
    struct dynbuf buf;
    curlx_dyn_init(&buf, CURL_X509_STR_MAX);
    result = encodeOID(&buf, beg, end);

    如果(!result) {
      const struct Curl_OID *op = searchOID(curlx_dyn_ptr(&buf));
      如果(op)
        result = curlx_dyn_add(store, op->textoid);
      否则
        result = curlx_dyn_add(store, curlx_dyn_ptr(&buf));
    }
    curlx_dyn_free(&buf);
  }
  返回 result;
}

/*
 * @unittest 1656
 */
UNITTEST CURLcode GTime2str(struct dynbuf *store,
                            const char *beg, const char *end);
UNITTEST CURLcode GTime2str(struct dynbuf *store,
                            const char *beg, const char *end)
{
  const char *tzp;
  const char *fracp;
  char sec1, sec2;
  size_t fracl;
  size_t tzl;
  const char *sep = "";

  /* Convert an ASN.1 Generalized time to a printable string.
     Return the dynamically allocated string, or NULL if an error occurs. */

  为(fracp = beg; fracp < end && ISDIGIT(*fracp); fracp++)
    ;

  /* Get seconds digits. */
  sec1 = '0';
  switch(fracp - beg - 12) {
  case 0:
    sec2 = '0';
    break;
  case 2:
    sec1 = fracp[-2];
    FALLTHROUGH();
  case 1:
    sec2 = fracp[-1];
    break;
  default:
    返回 CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* timezone follows optional fractional seconds. */
  tzp = fracp;
  fracl = 0; /* no fractional seconds detected so far */
  如果(fracp < end && (*fracp == '.' || *fracp == ',')) {
    /* Have fractional seconds, e.g. "[.,]\d+". How many? */
    fracp++; /* should be a digit char or BAD ARGUMENT */
    tzp = fracp;
    当(tzp < end && ISDIGIT(*tzp))
      tzp++;
    如果(tzp == fracp) /* never looped, no digit after [.,] */
      返回 CURLE_BAD_FUNCTION_ARGUMENT;
    fracl = tzp - fracp; /* number of fractional sec digits */
    DEBUGASSERT(fracl > 0);
    /* Strip trailing zeroes in fractional seconds.
     * May reduce fracl to 0 if only '0's are present. */
    当(fracl && fracp[fracl - 1] == '0')
      fracl--;
  }

  /* Process timezone. */
  如果(tzp >= end) {
    tzp = "";
    tzl = 0;
  }
  否则 如果(*tzp == 'Z') {
    sep = " ";
    tzp = "GMT";
    tzl = 3;
  }
  否则 如果((*tzp == '+') || (*tzp == '-')) {
    sep = " UTC";
    tzl = end - tzp;
  }
  否则 {
    sep = " ";
    tzl = end - tzp;
  }

  返回 curlx_dyn_addf(store,
                        "%.4s-%.2s-%.2s %.2s:%.2s:%c%c%s%.*s%s%.*s",
                        beg, beg + 4, beg + 6,
                        beg + 8, beg + 10, sec1, sec2,
                        fracl ? "." : "", (整数)fracl, fracp,
                        sep, (整数)tzl, tzp);
}

/*
 * Convert an ASN.1 UTC time to a printable string.
 *
 * Return error code.
 */
static CURLcode UTime2str(struct dynbuf *store,
                          const char *beg, const char *end)
{
  const char *tzp;
  size_t tzl;
  const char *sec;

  为(tzp = beg; tzp < end && *tzp >= '0' && *tzp <= '9'; tzp++)
    ;
  /* Get the seconds. */
  sec = beg + 10;
  switch(tzp - sec) {
  case 0:
    sec = "00";
    FALLTHROUGH();
  case 2:
    break;
  default:
    返回 CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Process timezone. */
  如果(tzp >= end)
    返回 CURLE_BAD_FUNCTION_ARGUMENT;
  如果(*tzp == 'Z') {
    tzp = "GMT";
    end = tzp + 3;
  }

  tzl = end - tzp;
  返回 curlx_dyn_addf(store, "%d%.2s-%.2s-%.2s %.2s:%.2s:%.2s %.*s",
                        20 - (*beg >= '5'), beg, beg + 2, beg + 4,
                        beg + 6, beg + 8, sec,
                        (整数)tzl, tzp);
}

/*
 * Convert an ASN.1 element to a printable string.
 *
 * Return error
 *
 * @unittest 1667
 */
UNITTEST CURLcode ASN1tostr(struct dynbuf *store,
                            struct Curl_asn1Element *elem);
UNITTEST CURLcode ASN1tostr(struct dynbuf *store,
                            struct Curl_asn1Element *elem)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  整数 type = elem->tag;
  如果(elem->constructed)
    返回 result; /* No conversion of structured elements. */

  switch(type) {
  case CURL_ASN1_BOOLEAN:
    result = bool2str(store, elem->beg, elem->end);
    break;
  case CURL_ASN1_INTEGER:
  case CURL_ASN1_ENUMERATED:
    result = int2str(store, elem->beg, elem->end);
    break;
  case CURL_ASN1_BIT_STRING:
    result = bit2str(store, elem->beg, elem->end);
    break;
  case CURL_ASN1_OCTET_STRING:
    result = octet2str(store, elem->beg, elem->end);
    break;
  case CURL_ASN1_NULL:
    result = curlx_dyn_addn(store, "", 1);
    break;
  case CURL_ASN1_OBJECT_IDENTIFIER:
    result = OID2str(store, elem->beg, elem->end);
    break;
  case CURL_ASN1_UTC_TIME:
    result = UTime2str(store, (const char *)elem->beg,
                       (const char *)elem->end);
    break;
  case CURL_ASN1_GENERALIZED_TIME:
    result = GTime2str(store, (const char *)elem->beg,
                       (const char *)elem->end);
    break;
  case CURL_ASN1_UTF8_STRING:
  case CURL_ASN1_NUMERIC_STRING:
  case CURL_ASN1_PRINTABLE_STRING:
  case CURL_ASN1_TELETEX_STRING:
  case CURL_ASN1_IA5_STRING:
  case CURL_ASN1_VISIBLE_STRING:
  case CURL_ASN1_UNIVERSAL_STRING:
  case CURL_ASN1_BMP_STRING:
    result = utf8asn1str(store, type, elem->beg, elem->end);
    break;
  }

  返回 result;
}

/*
 * ASCII encode distinguished name at `dn' into the store dynbuf.
 *
 * Returns error.
 */
static CURLcode encodeDN(struct dynbuf *store, struct Curl_asn1Element *dn)
{
  struct Curl_asn1Element rdn;
  struct Curl_asn1Element atv;
  struct Curl_asn1Element oid;
  struct Curl_asn1Element value;
  const uint8_t *p1;
  const uint8_t *p2;
  const uint8_t *p3;
  const uint8_t *str;
  CURLcode result = CURLE_OK;
  bool added = FALSE;
  struct dynbuf temp;
  curlx_dyn_init(&temp, CURL_X509_STR_MAX);

  为(p1 = dn->beg; p1 < dn->end;) {
    p1 = getASN1Element(&rdn, p1, dn->end);
    如果(!p1) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      goto error;
    }
    为(p2 = rdn.beg; p2 < rdn.end;) {
      p2 = getASN1Element(&atv, p2, rdn.end);
      如果(!p2) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        goto error;
      }
      p3 = getASN1Element(&oid, atv.beg, atv.end);
      如果(!p3) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        goto error;
      }
      如果(!getASN1Element(&value, p3, atv.end)) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        goto error;
      }
      curlx_dyn_reset(&temp);
      result = ASN1tostr(&temp, &oid);
      如果(result)
        goto error;

      str = curlx_dyn_uptr(&temp);

      如果(!str) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        goto error;
      }

      /* Encode delimiter.
         If attribute has a short uppercase name, delimiter is ", ". */
      为(p3 = str; ISUPPER(*p3); p3++)
        ;
      如果(added) {
        如果(p3 - str > 2)
          result = curlx_dyn_addn(store, "/", 1);
        否则
          result = curlx_dyn_addn(store, ", ", 2);
        如果(result)
          goto error;
      }

      /* Encode attribute name. */
      result = curlx_dyn_add(store, (const char *)str);
      如果(result)
        goto error;

      /* Generate equal sign. */
      result = curlx_dyn_addn(store, "=", 1);
      如果(result)
        goto error;

      /* Generate value. */
      result = ASN1tostr(store, &value);
      如果(result)
        goto error;
      curlx_dyn_reset(&temp);
      added = TRUE; /* use separator for next */
    }
  }
error:
  curlx_dyn_free(&temp);

  返回 result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#ifdef WANT_PARSEX509
/*
 * ASN.1 parse an X509 certificate into structure subfields.
 * Syntax is assumed to have already been checked by the SSL backend.
 * See RFC 5280.
 */
整数 Curl_parseX509(struct Curl_X509certificate *cert,
                   const uint8_t *beg, const uint8_t *end)
{
  struct Curl_asn1Element elem;
  struct Curl_asn1Element tbsCertificate;
  const uint8_t *ccp;
  static const uint8_t defaultVersion = 0;  /* v1. */

  memset(cert, 0, sizeof(*cert));
  cert->certificate.header = NULL;
  cert->certificate.beg = beg;
  cert->certificate.end = end;

  /* Get the sequence content. */
  如果(!getASN1Element(&elem, beg, end))
    返回 -1;  /* Invalid bounds/size. */
  beg = elem.beg;
  end = elem.end;

  /* Get tbsCertificate. */
  beg = getASN1Element(&tbsCertificate, beg, end);
  如果(!beg)
    返回 -1;
  /* Skip the signatureAlgorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  如果(!beg)
    返回 -1;
  /* Get the signatureValue. */
  如果(!getASN1Element(&cert->signature, beg, end))
    返回 -1;

  /* Parse TBSCertificate. */
  beg = tbsCertificate.beg;
  end = tbsCertificate.end;
  /* Get optional version, get serialNumber. */
  cert->version.header = NULL;
  cert->version.beg = &defaultVersion;
  cert->version.end = &defaultVersion + sizeof(defaultVersion);
  beg = getASN1Element(&elem, beg, end);
  如果(!beg)
    返回 -1;
  如果(elem.tag == 0) {
    如果(!getASN1Element(&cert->version, elem.beg, elem.end))
      返回 -1;
    beg = getASN1Element(&elem, beg, end);
    如果(!beg)
      返回 -1;
  }
  cert->serialNumber = elem;
  /* Get signature algorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  /* Get issuer. */
  beg = getASN1Element(&cert->issuer, beg, end);
  如果(!beg)
    返回 -1;
  /* Get notBefore and notAfter. */
  beg = getASN1Element(&elem, beg, end);
  如果(!beg)
    返回 -1;
  ccp = getASN1Element(&cert->notBefore, elem.beg, elem.end);
  如果(!ccp)
    返回 -1;
  如果(!getASN1Element(&cert->notAfter, ccp, elem.end))
    返回 -1;
  /* These two fields MUST consume all of the validity elem */
  如果(elem.end != cert->notAfter.end)
    返回 -1;
  /* Get subject. */
  beg = getASN1Element(&cert->subject, beg, end);
  如果(!beg)
    返回 -1;
  /* Get subjectPublicKeyAlgorithm and subjectPublicKey. */
  beg = getASN1Element(&cert->subjectPublicKeyInfo, beg, end);
  如果(!beg)
    返回 -1;
  ccp = getASN1Element(&cert->subjectPublicKeyAlgorithm,
                       cert->subjectPublicKeyInfo.beg,
                       cert->subjectPublicKeyInfo.end);
  如果(!ccp)
    返回 -1;
  如果(!getASN1Element(&cert->subjectPublicKey, ccp,
                     cert->subjectPublicKeyInfo.end))
    返回 -1;
  /* Get optional issuerUniqueID, subjectUniqueID and extensions. */
  cert->issuerUniqueID.tag = cert->subjectUniqueID.tag = 0;
  cert->extensions.tag = elem.tag = 0;
  cert->issuerUniqueID.header = cert->subjectUniqueID.header = NULL;
  cert->issuerUniqueID.beg = cert->issuerUniqueID.end = beg;
  cert->subjectUniqueID.beg = cert->subjectUniqueID.end = beg;
  cert->extensions.header = NULL;
  cert->extensions.beg = cert->extensions.end = beg;
  如果(beg < end) {
    beg = getASN1Element(&elem, beg, end);
    如果(!beg)
      返回 -1;
  }
  如果(elem.tag == 1) {
    cert->issuerUniqueID = elem;
    如果(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      如果(!beg)
        返回 -1;
    }
  }
  如果(elem.tag == 2) {
    cert->subjectUniqueID = elem;
    如果(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      如果(!beg)
        返回 -1;
    }
  }
  如果(elem.tag == 3)
    如果(!getASN1Element(&cert->extensions, elem.beg, elem.end))
      返回 -1;
  返回 0;
}

#endif /* WANT_PARSEX509 */

#ifdef WANT_EXTRACT_CERTINFO

static CURLcode dumpAlgo(struct dynbuf *store,
                         struct Curl_asn1Element *param,
                         const uint8_t *beg, const uint8_t *end)
{
  struct Curl_asn1Element oid;

  /* Get algorithm parameters and return algorithm name. */

  beg = getASN1Element(&oid, beg, end);
  如果(!beg)
    返回 CURLE_BAD_FUNCTION_ARGUMENT;
  param->header = NULL;
  param->tag = 0;
  param->beg = param->end = end;
  如果(beg < end) {
    const uint8_t *p = getASN1Element(param, beg, end);
    如果(!p)
      返回 CURLE_BAD_FUNCTION_ARGUMENT;
  }
  返回 OID2str(store, oid.beg, oid.end);
}

/*
 * This is a convenience function for push_certinfo_len that takes a zero
 * terminated value.
 */
static CURLcode ssl_push_certinfo(struct Curl_easy *data,
                                  整数 certnum,
                                  const char *label,
                                  const char *value)
{
  size_t valuelen = strlen(value);

  返回 Curl_ssl_push_certinfo_len(data, certnum, label, value, valuelen);
}

/*
 * This is a convenience function for push_certinfo_len that takes a
 * dynbuf value.
 *
 * It also does the verbose output if !certnum.
 */
static CURLcode ssl_push_certinfo_dyn(struct Curl_easy *data,
                                      整数 certnum,
                                      const char *label,
                                      struct dynbuf *ptr)
{
  size_t valuelen = curlx_dyn_len(ptr);
  const char *value = curlx_dyn_ptr(ptr);

  CURLcode result = Curl_ssl_push_certinfo_len(data, certnum, label,
                                               value, valuelen);

  如果(!certnum && !result)
    infof(data, "   %s: %s", label, value);

  返回 result;
}

static CURLcode do_pubkey_field(struct Curl_easy *data, 整数 certnum,
                                const char *label,
                                struct Curl_asn1Element *elem)
{
  CURLcode result;
  struct dynbuf out;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);

  /* Generate a certificate information record for the public key. */

  result = ASN1tostr(&out, elem);
  如果(!result && data->set.ssl.certinfo)
    result = ssl_push_certinfo_dyn(data, certnum, label, &out);
  curlx_dyn_free(&out);
  返回 result;
}

/* return 0 on success, 1 on error */
static 整数 do_pubkey(struct Curl_easy *data, 整数 certnum, const char *algo,
                     struct Curl_asn1Element *param,
                     struct Curl_asn1Element *pubkey)
{
  struct Curl_asn1Element elem;
  struct Curl_asn1Element pk;
  const uint8_t *p;

  /* Generate all information records for the public key. */

  如果(curl_strequal(algo, "ecPublicKey")) {
    /*
     * ECC public key is all the data, a value of type BIT STRING mapped to
     * OCTET STRING and should not be parsed as an ASN.1 value.
     */
    const size_t dlen = pubkey->end - pubkey->beg;
    size_t len;
    如果(dlen < 2)
      /* too small */
      返回 1;
    len = (dlen - 2) * 4;
    如果(!certnum)
      infof(data, "   ECC Public Key (%zu bits)", len);
    如果(data->set.ssl.certinfo) {
      char q[(sizeof(len) * 8 / 3) + 1];
      (void)curl_msnprintf(q, sizeof(q), "%zu", len);
      如果(ssl_push_certinfo(data, certnum, "ECC Public Key", q))
        返回 1;
    }
    返回 do_pubkey_field(data, certnum, "ecPublicKey", pubkey) == CURLE_OK
      ? 0 : 1;
  }

  /* Get the public key (single element). */
  如果(!getASN1Element(&pk, pubkey->beg + 1, pubkey->end))
    返回 1;

  如果(curl_strequal(algo, "rsaEncryption")) {
    const uint8_t *q;
    size_t len;

    p = getASN1Element(&elem, pk.beg, pk.end);
    如果(!p)
      返回 1;

    /* Compute key length. */
    为(q = elem.beg; q < elem.end && !*q; q++)
      ;
    len = ((elem.end - q) * 8);
    如果(len) {
      unsigned 整数 i;
      为(i = *q; !(i & 0x80); i <<= 1)
        len--;
    }
    如果(len > 32)
      elem.beg = q;     /* Strip leading zero bytes. */
    如果(!certnum)
      infof(data, "   RSA Public Key (%zu bits)", len);
    如果(data->set.ssl.certinfo) {
      char r[(sizeof(len) * 8 / 3) + 1];
      curl_msnprintf(r, sizeof(r), "%zu", len);
      如果(ssl_push_certinfo(data, certnum, "RSA Public Key", r))
        返回 1;
    }
    /* Generate coefficients. */
    如果(do_pubkey_field(data, certnum, "rsa(n)", &elem))
      返回 1;
    如果(!getASN1Element(&elem, p, pk.end))
      返回 1;
    如果(do_pubkey_field(data, certnum, "rsa(e)", &elem))
      返回 1;
  }
  否则 如果(curl_strequal(algo, "dsa")) {
    p = getASN1Element(&elem, param->beg, param->end);
    如果(p) {
      如果(do_pubkey_field(data, certnum, "dsa(p)", &elem))
        返回 1;
      p = getASN1Element(&elem, p, param->end);
      如果(p) {
        如果(do_pubkey_field(data, certnum, "dsa(q)", &elem))
          返回 1;
        如果(getASN1Element(&elem, p, param->end)) {
          如果(do_pubkey_field(data, certnum, "dsa(g)", &elem))
            返回 1;
          如果(do_pubkey_field(data, certnum, "dsa(pub_key)", &pk))
            返回 1;
        }
      }
    }
  }
  否则 如果(curl_strequal(algo, "dhpublicnumber")) {
    p = getASN1Element(&elem, param->beg, param->end);
    如果(p) {
      如果(do_pubkey_field(data, certnum, "dh(p)", &elem))
        返回 1;
      如果(getASN1Element(&elem, p, param->end)) {
        如果(do_pubkey_field(data, certnum, "dh(g)", &elem))
          返回 1;
        如果(do_pubkey_field(data, certnum, "dh(pub_key)", &pk))
          返回 1;
      }
    }
  }
  返回 0;
}

/*
 * Convert an ASN.1 distinguished name into a printable string.
 * Return error.
 */
static CURLcode DNtostr(struct dynbuf *store, struct Curl_asn1Element *dn)
{
  返回 encodeDN(store, dn);
}

CURLcode Curl_extract_certinfo(struct Curl_easy *data,
                               整数 certnum,
                               const uint8_t *beg,
                               const uint8_t *end)
{
  struct Curl_X509certificate cert;
  struct Curl_asn1Element param;
  char *certptr;
  size_t clen;
  struct dynbuf out;
  CURLcode result = CURLE_OK;
  unsigned 整数 version;
  const uint8_t *ptr;
  整数 rc;

  如果(!data->set.ssl.certinfo)
    如果(certnum)
      返回 CURLE_OK;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);
  /* Prepare the certificate information for curl_easy_getinfo(). */

  /* Extract the certificate ASN.1 elements. */
  如果(Curl_parseX509(&cert, beg, end))
    返回 CURLE_PEER_FAILED_VERIFICATION;

  /* Subject. */
  result = DNtostr(&out, &cert.subject);
  如果(result)
    goto done;
  如果(data->set.ssl.certinfo) {
    result = ssl_push_certinfo_dyn(data, certnum, "Subject", &out);
    如果(result)
      goto done;
  }
  curlx_dyn_reset(&out);

  /* Issuer. */
  result = DNtostr(&out, &cert.issuer);
  如果(result)
    goto done;
  如果(data->set.ssl.certinfo) {
    result = ssl_push_certinfo_dyn(data, certnum, "Issuer", &out);
    如果(result)
      goto done;
  }
  curlx_dyn_reset(&out);

  /* Version (always fits in less than 32 bits). */
  version = 0;
  为(ptr = cert.version.beg; ptr < cert.version.end; ptr++)
    version = (version << 8) | *ptr;
  如果(data->set.ssl.certinfo) {
    result = curlx_dyn_addf(&out, "%x", version);
    如果(result)
      goto done;
    result = ssl_push_certinfo_dyn(data, certnum, "Version", &out);
    如果(result)
      goto done;
    curlx_dyn_reset(&out);
  }

  /* Serial number. */
  result = ASN1tostr(&out, &cert.serialNumber);
  如果(result)
    goto done;
  如果(data->set.ssl.certinfo) {
    result = ssl_push_certinfo_dyn(data, certnum, "Serial Number", &out);
    如果(result)
      goto done;
  }
  curlx_dyn_reset(&out);

  /* Signature algorithm .*/
  result = dumpAlgo(&out, &param, cert.signatureAlgorithm.beg,
                    cert.signatureAlgorithm.end);
  如果(result)
    goto done;
  如果(data->set.ssl.certinfo) {
    result = ssl_push_certinfo_dyn(data, certnum, "Signature Algorithm", &out);
    如果(result)
      goto done;
  }
  curlx_dyn_reset(&out);

  /* Start Date. */
  result = ASN1tostr(&out, &cert.notBefore);
  如果(result)
    goto done;
  如果(data->set.ssl.certinfo) {
    result = ssl_push_certinfo_dyn(data, certnum, "Start Date", &out);
    如果(result)
      goto done;
  }
  curlx_dyn_reset(&out);

  /* Expire Date. */
  result = ASN1tostr(&out, &cert.notAfter);
  如果(result)
    goto done;
  如果(data->set.ssl.certinfo) {
    result = ssl_push_certinfo_dyn(data, certnum, "Expire Date", &out);
    如果(result)
      goto done;
  }
  curlx_dyn_reset(&out);

  /* Public Key Algorithm. */
  result = dumpAlgo(&out, &param, cert.subjectPublicKeyAlgorithm.beg,
                    cert.subjectPublicKeyAlgorithm.end);
  如果(result)
    goto done;
  如果(data->set.ssl.certinfo) {
    result = ssl_push_certinfo_dyn(data, certnum, "Public Key Algorithm",
                                   &out);
    如果(result)
      goto done;
  }

  rc = do_pubkey(data, certnum, curlx_dyn_ptr(&out),
                 &param, &cert.subjectPublicKey);
  如果(rc) {
    result = CURLE_OUT_OF_MEMORY; /* the most likely error */
    goto done;
  }
  curlx_dyn_reset(&out);

  /* Signature. */
  result = ASN1tostr(&out, &cert.signature);
  如果(result)
    goto done;
  如果(data->set.ssl.certinfo) {
    result = ssl_push_certinfo_dyn(data, certnum, "Signature", &out);
    如果(result)
      goto done;
  }
  curlx_dyn_reset(&out);

  /* Generate PEM certificate. */
  result = curlx_base64_encode((const uint8_t *)cert.certificate.beg,
                               cert.certificate.end - cert.certificate.beg,
                               &certptr, &clen);
  如果(result)
    goto done;

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
  如果(!result) {
    size_t j = 0;

    当(!result && (j < clen)) {
      size_t chunksize = (clen - j) > 64 ? 64 : (clen - j);
      result = curlx_dyn_addn(&out, &certptr[j], chunksize);
      如果(!result)
        result = curlx_dyn_addn(&out, "\n", 1);
      j += chunksize;
    }
    如果(!result)
      result = curlx_dyn_add(&out, "-----END CERTIFICATE-----\n");
  }
  curlx_free(certptr);
  如果(!result)
    如果(data->set.ssl.certinfo)
      result = ssl_push_certinfo_dyn(data, certnum, "Cert", &out);

done:
  如果(result)
    failf(data, "Failed extracting certificate chain");
  curlx_dyn_free(&out);
  返回 result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#endif /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
