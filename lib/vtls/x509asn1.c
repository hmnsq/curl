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
#अन्तर्भाव "curl_setup.h"

#यदि defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#यदि defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_RUSTLS)
#define WANT_PARSEX509 /* uses Curl_parseX509() */
#endif

#यदि defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_RUSTLS)
#define WANT_EXTRACT_CERTINFO /* uses Curl_extract_certinfo() */
#endif

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "curlx/base64.h"
#अन्तर्भाव "vtls/x509asn1.h"
#अन्तर्भाव "curlx/dynbuf.h"

/*
 * Constants.
 */

/* Largest supported ASN.1 structure. */
#define CURL_ASN1_MAX                   ((माप_प्रकार)0x40000)      /* 256K */

#ifdef WANT_EXTRACT_CERTINFO

/* ASN.1 OID table entry. */
संरचना Curl_OID {
  अचर वर्ण *numoid;  /* Dotted-numeric OID. */
  अचर वर्ण *textoid; /* OID name. */
};

/* ASN.1 OIDs. */
स्थिर अचर संरचना Curl_OID OIDtable[] = {
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
  { रिक्त,                       रिक्त }
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

स्थिर अचर uint8_t *getASN1Element_(संरचना Curl_asn1Element *elem,
                                      अचर uint8_t *beg, अचर uint8_t *end,
                                      माप_प्रकार lvl)
{
  uint8_t b;
  माप_प्रकार len;
  संरचना Curl_asn1Element lelem;

  /* Get a single ASN.1 element into `elem', parse ASN.1 string at `beg'
     ending at `end'.
     Returns a pointer in source string after the parsed element, or NULL
     if an error occurs. */
  यदि(!beg || !end || beg >= end || !*beg ||
     ((माप_प्रकार)(end - beg) > CURL_ASN1_MAX) ||
     lvl++ >= CURL_ASN1_MAX_RECURSIONS)
    फर्काऊ रिक्त;

  /* Process header byte. */
  elem->header = beg;
  b = *beg++;
  elem->constructed = (b & 0x20) != 0;
  elem->eclass = (b >> 6) & 3;
  b &= 0x1F;
  यदि(b == 0x1F)
    फर्काऊ रिक्त; /* Long tag values not supported here. */
  elem->tag = b;

  /* Process length. */
  यदि(beg >= end)
    फर्काऊ रिक्त;
  b = *beg++;
  यदि(!(b & 0x80))
    len = b;
  अन्यथा {
    b &= 0x7F;
    यदि(!b) {
      /* Unspecified length. Since we have all the data, we can determine the
         effective length by skipping element until an EOC element is found. */
      यदि(!elem->constructed)
        फर्काऊ रिक्त;
      elem->beg = beg;
      जबसम्म(beg < end && *beg) {
        beg = getASN1Element_(&lelem, beg, end, lvl);
        यदि(!beg)
          फर्काऊ रिक्त;
      }
      यदि(end - beg < 2 || beg[1])       /* EOC must have a zero length. */
        फर्काऊ रिक्त;
      elem->end = beg;                  /* Do not include EOC as content. */
      फर्काऊ beg + 2;                   /* After EOC element. */
    }
    अन्यथा यदि(b > (माप_प्रकार)(end - beg))
      फर्काऊ रिक्त; /* Does not fit in source. */
    अन्यथा {
      /* Get long length. */
      len = 0;
      गर {
        यदि(len & 0xFF000000L)
          फर्काऊ रिक्त;  /* Lengths > 32 bits are not supported. */
        len = (len << 8) | *beg++;
      } जबसम्म(--b);
    }
  }
  यदि(len > (माप_प्रकार)(end - beg))
    फर्काऊ रिक्त;  /* Element data does not fit in source. */
  elem->beg = beg;
  elem->end = beg + len;
  फर्काऊ elem->end;
}

/*
 * @unittest 1657
 */
UNITTEST अचर uint8_t *getASN1Element(संरचना Curl_asn1Element *elem,
                                       अचर uint8_t *beg, अचर uint8_t *end);
UNITTEST अचर uint8_t *getASN1Element(संरचना Curl_asn1Element *elem,
                                       अचर uint8_t *beg, अचर uint8_t *end)
{
  फर्काऊ getASN1Element_(elem, beg, end, 0);
}

#ifdef WANT_EXTRACT_CERTINFO

/*
 * Search the null-terminated OID or OID identifier in local table.
 * Return the table entry pointer or NULL if not found.
 */
स्थिर अचर संरचना Curl_OID *खोज_OID(अचर वर्ण *oid)
{
  अचर संरचना Curl_OID *op;
  लागि(op = OIDtable; op->numoid; op++)
    यदि(!म_भेद(op->numoid, oid) || curl_strequal(op->textoid, oid))
      फर्काऊ op;

  फर्काऊ रिक्त;
}

/*
 * Convert an ASN.1 Boolean value into its string representation.
 *
 * Return error code.
 */

स्थिर CURLcode bool2str(संरचना dynbuf *store,
                         अचर uint8_t *beg, अचर uint8_t *end)
{
  uint8_t val;
  यदि(end - beg != 1)
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  val = *beg;
  यदि((val != 0xff) && (val != 0x00))
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  फर्काऊ curlx_dyn_add(store, val ? "TRUE" : "FALSE");
}

/*
 * Convert an ASN.1 octet string to a printable string.
 *
 * Return error code.
 */
स्थिर CURLcode octet2str(संरचना dynbuf *store,
                          अचर uint8_t *beg, अचर uint8_t *end)
{
  CURLcode result = CURLE_OK;

  जबसम्म(!result && beg < end)
    result = curlx_dyn_addf(store, "%02x:", *beg++);

  फर्काऊ result;
}

स्थिर CURLcode bit2str(संरचना dynbuf *store,
                        अचर uint8_t *beg, अचर uint8_t *end)
{
  /* Convert an ASN.1 bit string to a printable string. */

  यदि(beg < end) {
    uint8_t unused_bits = *beg++;
    यदि(unused_bits > 7)
      फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  अन्यथा
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  फर्काऊ octet2str(store, beg, end);
}

/*
 * Convert an ASN.1 integer value into its string representation.
 *
 * Returns error.
 */
स्थिर CURLcode int2str(संरचना dynbuf *store,
                        अचर uint8_t *beg, अचर uint8_t *end)
{
  uint32_t uval = 0;
  int32_t sval = 0;
  माप_प्रकार n = end - beg;
  माप_प्रकार i;
  अचर uint8_t *p = beg;

  यदि(!n)
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;

  यदि(n > 4)
    फर्काऊ octet2str(store, beg, end);

  यदि(p[0] & 0x80)
    uval = ~0U;

  लागि(i = 0; i < n; i++)
    uval = (uval << 8) | p[i];

  sval = (int32_t)uval;

  /* small values are easier to read as decimals */
  यदि(sval > -10000 && sval < 10000)
    फर्काऊ curlx_dyn_addf(store, "%d", sval);

  /* represent larger values as hex */
  फर्काऊ curlx_dyn_addf(store, "0x%x", uval);
}

/*
 * Convert from an ASN.1 typed string to UTF8.
 *
 * The result is stored in a dynbuf that is inited by the user of this
 * function.
 *
 * Returns error.
 */
स्थिर CURLcode utf8asn1str(संरचना dynbuf *to, पूर्णाङ्क type,
                            अचर uint8_t *from, अचर uint8_t *end)
{
  माप_प्रकार inlength = end - from;
  पूर्णाङ्क size = 1;
  CURLcode result = CURLE_OK;

  विकल्प(type) {
  पक्ष CURL_ASN1_BMP_STRING:
    size = 2;
    रोक;
  पक्ष CURL_ASN1_UNIVERSAL_STRING:
    size = 4;
    रोक;
  पक्ष CURL_ASN1_NUMERIC_STRING:
  पक्ष CURL_ASN1_PRINTABLE_STRING:
  पक्ष CURL_ASN1_TELETEX_STRING:
  पक्ष CURL_ASN1_IA5_STRING:
  पक्ष CURL_ASN1_VISIBLE_STRING:
  पक्ष CURL_ASN1_UTF8_STRING:
    रोक;
  स्वभाव:
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;  /* Conversion not supported. */
  }

  यदि(inlength % size)
    /* Length inconsistent with character size. */
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;

  यदि(type == CURL_ASN1_UTF8_STRING) {
    /* copy. */
    यदि(inlength)
      result = curlx_dyn_addn(to, from, inlength);
  }
  अन्यथा {
    जबसम्म(!result && (from < end)) {
      वर्ण buf[4]; /* decode buffer */
      माप_प्रकार charsize = 1;
      अचिन्हित पूर्णाङ्क wc = 0;

      विकल्प(size) {
      पक्ष 4:
        wc = (wc << 8) | *from++;
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      पक्ष 2:
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      स्वभाव: /* case 1: */
        wc = (wc << 8) | *from++;
      }
      यदि(wc >= 0x00000080) {
        यदि(wc >= 0x00000800) {
          यदि(wc >= 0x00010000) {
            यदि(wc >= 0x00200000) {
              /* Invalid char. size for target encoding. */
              फर्काऊ CURLE_WEIRD_SERVER_REPLY;
            }
            buf[3] = (वर्ण)(0x80 | (wc & 0x3F));
            wc = (wc >> 6) | 0x00010000;
            charsize++;
          }
          buf[2] = (वर्ण)(0x80 | (wc & 0x3F));
          wc = (wc >> 6) | 0x00000800;
          charsize++;
        }
        buf[1] = (वर्ण)(0x80 | (wc & 0x3F));
        wc = (wc >> 6) | 0x000000C0;
        charsize++;
      }
      buf[0] = (वर्ण)wc;
      result = curlx_dyn_addn(to, buf, charsize);
    }
  }
  फर्काऊ result;
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
UNITTEST CURLcode encodeOID(संरचना dynbuf *store,
                            अचर uint8_t *beg, अचर uint8_t *end);
UNITTEST CURLcode encodeOID(संरचना dynbuf *store,
                            अचर uint8_t *beg, अचर uint8_t *end)
{
  uint32_t x;
  uint32_t y;
  CURLcode result = CURLE_OK;

  यदि(end <= beg)
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;

  /* Process the first two numbers. The initial digit cannot be larger than
     2 */
  y = *beg++;
  यदि(y <= 80)
    x = y / 40;
  अन्यथा
    x = 2;
  result = curlx_dyn_addf(store, "%u", x);
  यदि(result)
    फर्काऊ result;

  यदि(y & 0x80) {
    uint32_t t = (y & 0x7f); /* the second number */
    यदि(!t)
      /* reject leading 0x80 padding */
      फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
    गर {
      यदि((t & 0xFE000000) || (beg >= end))
        फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      t = (t << 7) | (y & 0x7F);
    } जबसम्म(y & 0x80);
    y = t;
  }

  result = curlx_dyn_addf(store, ".%u", y - (x * 40));
  यदि(result)
    फर्काऊ result;

  /* Process the trailing numbers. */
  जबसम्म((beg < end) && !result) {
    x = 0;
    y = 0;
    गर {
      /* 0x80 as the first value is invalid since it only adds padding */
      यदि((y & 0x80) && !x)
        फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
      अन्यथा यदि((x & 0xFE000000) || (beg == end))
        फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      x = (x << 7) | (y & 0x7F);
    } जबसम्म(y & 0x80);
    result = curlx_dyn_addf(store, ".%u", x);
  }
  फर्काऊ result;
}

/*
 * Convert an ASN.1 OID into its dotted or symbolic string representation.
 *
 * Return error code.
 */

स्थिर CURLcode OID2str(संरचना dynbuf *store,
                        अचर uint8_t *beg, अचर uint8_t *end)
{
  CURLcode result = CURLE_OK;
  यदि(beg < end) {
    संरचना dynbuf buf;
    curlx_dyn_init(&buf, CURL_X509_STR_MAX);
    result = encodeOID(&buf, beg, end);

    यदि(!result) {
      अचर संरचना Curl_OID *op = खोज_OID(curlx_dyn_ptr(&buf));
      यदि(op)
        result = curlx_dyn_add(store, op->textoid);
      अन्यथा
        result = curlx_dyn_add(store, curlx_dyn_ptr(&buf));
    }
    curlx_dyn_free(&buf);
  }
  फर्काऊ result;
}

/*
 * @unittest 1656
 */
UNITTEST CURLcode GTime2str(संरचना dynbuf *store,
                            अचर वर्ण *beg, अचर वर्ण *end);
UNITTEST CURLcode GTime2str(संरचना dynbuf *store,
                            अचर वर्ण *beg, अचर वर्ण *end)
{
  अचर वर्ण *tzp;
  अचर वर्ण *fracp;
  वर्ण sec1, sec2;
  माप_प्रकार fracl;
  माप_प्रकार tzl;
  अचर वर्ण *sep = "";

  /* Convert an ASN.1 Generalized time to a printable string.
     Return the dynamically allocated string, or NULL if an error occurs. */

  लागि(fracp = beg; fracp < end && ISDIGIT(*fracp); fracp++)
    ;

  /* Get seconds digits. */
  sec1 = '0';
  विकल्प(fracp - beg - 12) {
  पक्ष 0:
    sec2 = '0';
    रोक;
  पक्ष 2:
    sec1 = fracp[-2];
    FALLTHROUGH();
  पक्ष 1:
    sec2 = fracp[-1];
    रोक;
  स्वभाव:
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* timezone follows optional fractional seconds. */
  tzp = fracp;
  fracl = 0; /* no fractional seconds detected so far */
  यदि(fracp < end && (*fracp == '.' || *fracp == ',')) {
    /* Have fractional seconds, e.g. "[.,]\d+". How many? */
    fracp++; /* should be a digit char or BAD ARGUMENT */
    tzp = fracp;
    जबसम्म(tzp < end && ISDIGIT(*tzp))
      tzp++;
    यदि(tzp == fracp) /* never looped, no digit after [.,] */
      फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
    fracl = tzp - fracp; /* number of fractional sec digits */
    DEBUGASSERT(fracl > 0);
    /* Strip trailing zeroes in fractional seconds.
     * May reduce fracl to 0 if only '0's are present. */
    जबसम्म(fracl && fracp[fracl - 1] == '0')
      fracl--;
  }

  /* Process timezone. */
  यदि(tzp >= end) {
    tzp = "";
    tzl = 0;
  }
  अन्यथा यदि(*tzp == 'Z') {
    sep = " ";
    tzp = "GMT";
    tzl = 3;
  }
  अन्यथा यदि((*tzp == '+') || (*tzp == '-')) {
    sep = " UTC";
    tzl = end - tzp;
  }
  अन्यथा {
    sep = " ";
    tzl = end - tzp;
  }

  फर्काऊ curlx_dyn_addf(store,
                        "%.4s-%.2s-%.2s %.2s:%.2s:%c%c%s%.*s%s%.*s",
                        beg, beg + 4, beg + 6,
                        beg + 8, beg + 10, sec1, sec2,
                        fracl ? "." : "", (पूर्णाङ्क)fracl, fracp,
                        sep, (पूर्णाङ्क)tzl, tzp);
}

/*
 * Convert an ASN.1 UTC time to a printable string.
 *
 * Return error code.
 */
स्थिर CURLcode UTime2str(संरचना dynbuf *store,
                          अचर वर्ण *beg, अचर वर्ण *end)
{
  अचर वर्ण *tzp;
  माप_प्रकार tzl;
  अचर वर्ण *sec;

  लागि(tzp = beg; tzp < end && *tzp >= '0' && *tzp <= '9'; tzp++)
    ;
  /* Get the seconds. */
  sec = beg + 10;
  विकल्प(tzp - sec) {
  पक्ष 0:
    sec = "00";
    FALLTHROUGH();
  पक्ष 2:
    रोक;
  स्वभाव:
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Process timezone. */
  यदि(tzp >= end)
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  यदि(*tzp == 'Z') {
    tzp = "GMT";
    end = tzp + 3;
  }

  tzl = end - tzp;
  फर्काऊ curlx_dyn_addf(store, "%d%.2s-%.2s-%.2s %.2s:%.2s:%.2s %.*s",
                        20 - (*beg >= '5'), beg, beg + 2, beg + 4,
                        beg + 6, beg + 8, sec,
                        (पूर्णाङ्क)tzl, tzp);
}

/*
 * Convert an ASN.1 element to a printable string.
 *
 * Return error
 *
 * @unittest 1667
 */
UNITTEST CURLcode ASN1tostr(संरचना dynbuf *store,
                            संरचना Curl_asn1Element *elem);
UNITTEST CURLcode ASN1tostr(संरचना dynbuf *store,
                            संरचना Curl_asn1Element *elem)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  पूर्णाङ्क type = elem->tag;
  यदि(elem->constructed)
    फर्काऊ result; /* No conversion of structured elements. */

  विकल्प(type) {
  पक्ष CURL_ASN1_BOOLEAN:
    result = bool2str(store, elem->beg, elem->end);
    रोक;
  पक्ष CURL_ASN1_INTEGER:
  पक्ष CURL_ASN1_ENUMERATED:
    result = int2str(store, elem->beg, elem->end);
    रोक;
  पक्ष CURL_ASN1_BIT_STRING:
    result = bit2str(store, elem->beg, elem->end);
    रोक;
  पक्ष CURL_ASN1_OCTET_STRING:
    result = octet2str(store, elem->beg, elem->end);
    रोक;
  पक्ष CURL_ASN1_NULL:
    result = curlx_dyn_addn(store, "", 1);
    रोक;
  पक्ष CURL_ASN1_OBJECT_IDENTIFIER:
    result = OID2str(store, elem->beg, elem->end);
    रोक;
  पक्ष CURL_ASN1_UTC_TIME:
    result = UTime2str(store, (अचर वर्ण *)elem->beg,
                       (अचर वर्ण *)elem->end);
    रोक;
  पक्ष CURL_ASN1_GENERALIZED_TIME:
    result = GTime2str(store, (अचर वर्ण *)elem->beg,
                       (अचर वर्ण *)elem->end);
    रोक;
  पक्ष CURL_ASN1_UTF8_STRING:
  पक्ष CURL_ASN1_NUMERIC_STRING:
  पक्ष CURL_ASN1_PRINTABLE_STRING:
  पक्ष CURL_ASN1_TELETEX_STRING:
  पक्ष CURL_ASN1_IA5_STRING:
  पक्ष CURL_ASN1_VISIBLE_STRING:
  पक्ष CURL_ASN1_UNIVERSAL_STRING:
  पक्ष CURL_ASN1_BMP_STRING:
    result = utf8asn1str(store, type, elem->beg, elem->end);
    रोक;
  }

  फर्काऊ result;
}

/*
 * ASCII encode distinguished name at `dn' into the store dynbuf.
 *
 * Returns error.
 */
स्थिर CURLcode encodeDN(संरचना dynbuf *store, संरचना Curl_asn1Element *dn)
{
  संरचना Curl_asn1Element rdn;
  संरचना Curl_asn1Element atv;
  संरचना Curl_asn1Element oid;
  संरचना Curl_asn1Element मान;
  अचर uint8_t *p1;
  अचर uint8_t *p2;
  अचर uint8_t *p3;
  अचर uint8_t *str;
  CURLcode result = CURLE_OK;
  bool added = FALSE;
  संरचना dynbuf temp;
  curlx_dyn_init(&temp, CURL_X509_STR_MAX);

  लागि(p1 = dn->beg; p1 < dn->end;) {
    p1 = getASN1Element(&rdn, p1, dn->end);
    यदि(!p1) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      गच्छ error;
    }
    लागि(p2 = rdn.beg; p2 < rdn.end;) {
      p2 = getASN1Element(&atv, p2, rdn.end);
      यदि(!p2) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        गच्छ error;
      }
      p3 = getASN1Element(&oid, atv.beg, atv.end);
      यदि(!p3) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        गच्छ error;
      }
      यदि(!getASN1Element(&मान, p3, atv.end)) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        गच्छ error;
      }
      curlx_dyn_reset(&temp);
      result = ASN1tostr(&temp, &oid);
      यदि(result)
        गच्छ error;

      str = curlx_dyn_uptr(&temp);

      यदि(!str) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        गच्छ error;
      }

      /* Encode delimiter.
         If attribute has a short uppercase name, delimiter is ", ". */
      लागि(p3 = str; ISUPPER(*p3); p3++)
        ;
      यदि(added) {
        यदि(p3 - str > 2)
          result = curlx_dyn_addn(store, "/", 1);
        अन्यथा
          result = curlx_dyn_addn(store, ", ", 2);
        यदि(result)
          गच्छ error;
      }

      /* Encode attribute name. */
      result = curlx_dyn_add(store, (अचर वर्ण *)str);
      यदि(result)
        गच्छ error;

      /* Generate equal sign. */
      result = curlx_dyn_addn(store, "=", 1);
      यदि(result)
        गच्छ error;

      /* Generate value. */
      result = ASN1tostr(store, &मान);
      यदि(result)
        गच्छ error;
      curlx_dyn_reset(&temp);
      added = TRUE; /* use separator for next */
    }
  }
error:
  curlx_dyn_free(&temp);

  फर्काऊ result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#ifdef WANT_PARSEX509
/*
 * ASN.1 parse an X509 certificate into structure subfields.
 * Syntax is assumed to have already been checked by the SSL backend.
 * See RFC 5280.
 */
पूर्णाङ्क Curl_parseX509(संरचना Curl_X509certificate *cert,
                   अचर uint8_t *beg, अचर uint8_t *end)
{
  संरचना Curl_asn1Element elem;
  संरचना Curl_asn1Element tbsCertificate;
  अचर uint8_t *ccp;
  स्थिर अचर uint8_t defaultVersion = 0;  /* v1. */

  स_रखो(cert, 0, परिमाण(*cert));
  cert->certificate.header = रिक्त;
  cert->certificate.beg = beg;
  cert->certificate.end = end;

  /* Get the sequence content. */
  यदि(!getASN1Element(&elem, beg, end))
    फर्काऊ -1;  /* Invalid bounds/size. */
  beg = elem.beg;
  end = elem.end;

  /* Get tbsCertificate. */
  beg = getASN1Element(&tbsCertificate, beg, end);
  यदि(!beg)
    फर्काऊ -1;
  /* Skip the signatureAlgorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  यदि(!beg)
    फर्काऊ -1;
  /* Get the signatureValue. */
  यदि(!getASN1Element(&cert->signature, beg, end))
    फर्काऊ -1;

  /* Parse TBSCertificate. */
  beg = tbsCertificate.beg;
  end = tbsCertificate.end;
  /* Get optional version, get serialNumber. */
  cert->version.header = रिक्त;
  cert->version.beg = &defaultVersion;
  cert->version.end = &defaultVersion + परिमाण(defaultVersion);
  beg = getASN1Element(&elem, beg, end);
  यदि(!beg)
    फर्काऊ -1;
  यदि(elem.tag == 0) {
    यदि(!getASN1Element(&cert->version, elem.beg, elem.end))
      फर्काऊ -1;
    beg = getASN1Element(&elem, beg, end);
    यदि(!beg)
      फर्काऊ -1;
  }
  cert->serial_सङ्ख्या = elem;
  /* Get signature algorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  /* Get issuer. */
  beg = getASN1Element(&cert->issuer, beg, end);
  यदि(!beg)
    फर्काऊ -1;
  /* Get notBefore and notAfter. */
  beg = getASN1Element(&elem, beg, end);
  यदि(!beg)
    फर्काऊ -1;
  ccp = getASN1Element(&cert->notBefore, elem.beg, elem.end);
  यदि(!ccp)
    फर्काऊ -1;
  यदि(!getASN1Element(&cert->notAfter, ccp, elem.end))
    फर्काऊ -1;
  /* These two fields MUST consume all of the validity elem */
  यदि(elem.end != cert->notAfter.end)
    फर्काऊ -1;
  /* Get subject. */
  beg = getASN1Element(&cert->subject, beg, end);
  यदि(!beg)
    फर्काऊ -1;
  /* Get subjectPublicKeyAlgorithm and subjectPublicKey. */
  beg = getASN1Element(&cert->subject_Public_साँचो_Info, beg, end);
  यदि(!beg)
    फर्काऊ -1;
  ccp = getASN1Element(&cert->subject_Public_साँचो_Algorithm,
                       cert->subject_Public_साँचो_Info.beg,
                       cert->subject_Public_साँचो_Info.end);
  यदि(!ccp)
    फर्काऊ -1;
  यदि(!getASN1Element(&cert->subject_Public_साँचो, ccp,
                     cert->subject_Public_साँचो_Info.end))
    फर्काऊ -1;
  /* Get optional issuerUniqueID, subjectUniqueID and extensions. */
  cert->issuerUniqueID.tag = cert->subjectUniqueID.tag = 0;
  cert->extensions.tag = elem.tag = 0;
  cert->issuerUniqueID.header = cert->subjectUniqueID.header = रिक्त;
  cert->issuerUniqueID.beg = cert->issuerUniqueID.end = beg;
  cert->subjectUniqueID.beg = cert->subjectUniqueID.end = beg;
  cert->extensions.header = रिक्त;
  cert->extensions.beg = cert->extensions.end = beg;
  यदि(beg < end) {
    beg = getASN1Element(&elem, beg, end);
    यदि(!beg)
      फर्काऊ -1;
  }
  यदि(elem.tag == 1) {
    cert->issuerUniqueID = elem;
    यदि(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      यदि(!beg)
        फर्काऊ -1;
    }
  }
  यदि(elem.tag == 2) {
    cert->subjectUniqueID = elem;
    यदि(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      यदि(!beg)
        फर्काऊ -1;
    }
  }
  यदि(elem.tag == 3)
    यदि(!getASN1Element(&cert->extensions, elem.beg, elem.end))
      फर्काऊ -1;
  फर्काऊ 0;
}

#endif /* WANT_PARSEX509 */

#ifdef WANT_EXTRACT_CERTINFO

स्थिर CURLcode dumpAlgo(संरचना dynbuf *store,
                         संरचना Curl_asn1Element *param,
                         अचर uint8_t *beg, अचर uint8_t *end)
{
  संरचना Curl_asn1Element oid;

  /* Get algorithm parameters and return algorithm name. */

  beg = getASN1Element(&oid, beg, end);
  यदि(!beg)
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  param->header = रिक्त;
  param->tag = 0;
  param->beg = param->end = end;
  यदि(beg < end) {
    अचर uint8_t *p = getASN1Element(param, beg, end);
    यदि(!p)
      फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  फर्काऊ OID2str(store, oid.beg, oid.end);
}

/*
 * This is a convenience function for push_certinfo_len that takes a zero
 * terminated value.
 */
स्थिर CURLcode ssl_थप_certinfo(संरचना Curl_easy *data,
                                  पूर्णाङ्क certnum,
                                  अचर वर्ण *label,
                                  अचर वर्ण *मान)
{
  माप_प्रकार valuelen = म_माप(मान);

  फर्काऊ Curl_ssl_थप_certinfo_len(data, certnum, label, मान, valuelen);
}

/*
 * This is a convenience function for push_certinfo_len that takes a
 * dynbuf value.
 *
 * It also does the verbose output if !certnum.
 */
स्थिर CURLcode ssl_थप_certinfo_dyn(संरचना Curl_easy *data,
                                      पूर्णाङ्क certnum,
                                      अचर वर्ण *label,
                                      संरचना dynbuf *ptr)
{
  माप_प्रकार valuelen = curlx_dyn_len(ptr);
  अचर वर्ण *मान = curlx_dyn_ptr(ptr);

  CURLcode result = Curl_ssl_थप_certinfo_len(data, certnum, label,
                                               मान, valuelen);

  यदि(!certnum && !result)
    infof(data, "   %s: %s", label, मान);

  फर्काऊ result;
}

स्थिर CURLcode do_pubkey_field(संरचना Curl_easy *data, पूर्णाङ्क certnum,
                                अचर वर्ण *label,
                                संरचना Curl_asn1Element *elem)
{
  CURLcode result;
  संरचना dynbuf out;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);

  /* Generate a certificate information record for the public key. */

  result = ASN1tostr(&out, elem);
  यदि(!result && data->set.ssl.certinfo)
    result = ssl_थप_certinfo_dyn(data, certnum, label, &out);
  curlx_dyn_free(&out);
  फर्काऊ result;
}

/* return 0 on success, 1 on error */
स्थिर पूर्णाङ्क do_pubkey(संरचना Curl_easy *data, पूर्णाङ्क certnum, अचर वर्ण *algo,
                     संरचना Curl_asn1Element *param,
                     संरचना Curl_asn1Element *pubkey)
{
  संरचना Curl_asn1Element elem;
  संरचना Curl_asn1Element pk;
  अचर uint8_t *p;

  /* Generate all information records for the public key. */

  यदि(curl_strequal(algo, "ecPublicKey")) {
    /*
     * ECC public key is all the data, a value of type BIT STRING mapped to
     * OCTET STRING and should not be parsed as an ASN.1 value.
     */
    अचर माप_प्रकार dlen = pubkey->end - pubkey->beg;
    माप_प्रकार len;
    यदि(dlen < 2)
      /* too small */
      फर्काऊ 1;
    len = (dlen - 2) * 4;
    यदि(!certnum)
      infof(data, "   ECC Public Key (%zu bits)", len);
    यदि(data->set.ssl.certinfo) {
      वर्ण q[(परिमाण(len) * 8 / 3) + 1];
      (शून्य)curl_msnprintf(q, परिमाण(q), "%zu", len);
      यदि(ssl_थप_certinfo(data, certnum, "ECC Public Key", q))
        फर्काऊ 1;
    }
    फर्काऊ do_pubkey_field(data, certnum, "ecPublicKey", pubkey) == CURLE_OK
      ? 0 : 1;
  }

  /* Get the public key (single element). */
  यदि(!getASN1Element(&pk, pubkey->beg + 1, pubkey->end))
    फर्काऊ 1;

  यदि(curl_strequal(algo, "rsaEncryption")) {
    अचर uint8_t *q;
    माप_प्रकार len;

    p = getASN1Element(&elem, pk.beg, pk.end);
    यदि(!p)
      फर्काऊ 1;

    /* Compute key length. */
    लागि(q = elem.beg; q < elem.end && !*q; q++)
      ;
    len = ((elem.end - q) * 8);
    यदि(len) {
      अचिन्हित पूर्णाङ्क i;
      लागि(i = *q; !(i & 0x80); i <<= 1)
        len--;
    }
    यदि(len > 32)
      elem.beg = q;     /* Strip leading zero bytes. */
    यदि(!certnum)
      infof(data, "   RSA Public Key (%zu bits)", len);
    यदि(data->set.ssl.certinfo) {
      वर्ण r[(परिमाण(len) * 8 / 3) + 1];
      curl_msnprintf(r, परिमाण(r), "%zu", len);
      यदि(ssl_थप_certinfo(data, certnum, "RSA Public Key", r))
        फर्काऊ 1;
    }
    /* Generate coefficients. */
    यदि(do_pubkey_field(data, certnum, "rsa(n)", &elem))
      फर्काऊ 1;
    यदि(!getASN1Element(&elem, p, pk.end))
      फर्काऊ 1;
    यदि(do_pubkey_field(data, certnum, "rsa(e)", &elem))
      फर्काऊ 1;
  }
  अन्यथा यदि(curl_strequal(algo, "dsa")) {
    p = getASN1Element(&elem, param->beg, param->end);
    यदि(p) {
      यदि(do_pubkey_field(data, certnum, "dsa(p)", &elem))
        फर्काऊ 1;
      p = getASN1Element(&elem, p, param->end);
      यदि(p) {
        यदि(do_pubkey_field(data, certnum, "dsa(q)", &elem))
          फर्काऊ 1;
        यदि(getASN1Element(&elem, p, param->end)) {
          यदि(do_pubkey_field(data, certnum, "dsa(g)", &elem))
            फर्काऊ 1;
          यदि(do_pubkey_field(data, certnum, "dsa(pub_key)", &pk))
            फर्काऊ 1;
        }
      }
    }
  }
  अन्यथा यदि(curl_strequal(algo, "dhpublicnumber")) {
    p = getASN1Element(&elem, param->beg, param->end);
    यदि(p) {
      यदि(do_pubkey_field(data, certnum, "dh(p)", &elem))
        फर्काऊ 1;
      यदि(getASN1Element(&elem, p, param->end)) {
        यदि(do_pubkey_field(data, certnum, "dh(g)", &elem))
          फर्काऊ 1;
        यदि(do_pubkey_field(data, certnum, "dh(pub_key)", &pk))
          फर्काऊ 1;
      }
    }
  }
  फर्काऊ 0;
}

/*
 * Convert an ASN.1 distinguished name into a printable string.
 * Return error.
 */
स्थिर CURLcode DNtostr(संरचना dynbuf *store, संरचना Curl_asn1Element *dn)
{
  फर्काऊ encodeDN(store, dn);
}

CURLcode Curl_extract_certinfo(संरचना Curl_easy *data,
                               पूर्णाङ्क certnum,
                               अचर uint8_t *beg,
                               अचर uint8_t *end)
{
  संरचना Curl_X509certificate cert;
  संरचना Curl_asn1Element param;
  वर्ण *certptr;
  माप_प्रकार clen;
  संरचना dynbuf out;
  CURLcode result = CURLE_OK;
  अचिन्हित पूर्णाङ्क version;
  अचर uint8_t *ptr;
  पूर्णाङ्क rc;

  यदि(!data->set.ssl.certinfo)
    यदि(certnum)
      फर्काऊ CURLE_OK;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);
  /* Prepare the certificate information for curl_easy_getinfo(). */

  /* Extract the certificate ASN.1 elements. */
  यदि(Curl_parseX509(&cert, beg, end))
    फर्काऊ CURLE_PEER_FAILED_VERIFICATION;

  /* Subject. */
  result = DNtostr(&out, &cert.subject);
  यदि(result)
    गच्छ done;
  यदि(data->set.ssl.certinfo) {
    result = ssl_थप_certinfo_dyn(data, certnum, "Subject", &out);
    यदि(result)
      गच्छ done;
  }
  curlx_dyn_reset(&out);

  /* Issuer. */
  result = DNtostr(&out, &cert.issuer);
  यदि(result)
    गच्छ done;
  यदि(data->set.ssl.certinfo) {
    result = ssl_थप_certinfo_dyn(data, certnum, "Issuer", &out);
    यदि(result)
      गच्छ done;
  }
  curlx_dyn_reset(&out);

  /* Version (always fits in less than 32 bits). */
  version = 0;
  लागि(ptr = cert.version.beg; ptr < cert.version.end; ptr++)
    version = (version << 8) | *ptr;
  यदि(data->set.ssl.certinfo) {
    result = curlx_dyn_addf(&out, "%x", version);
    यदि(result)
      गच्छ done;
    result = ssl_थप_certinfo_dyn(data, certnum, "Version", &out);
    यदि(result)
      गच्छ done;
    curlx_dyn_reset(&out);
  }

  /* Serial number. */
  result = ASN1tostr(&out, &cert.serial_सङ्ख्या);
  यदि(result)
    गच्छ done;
  यदि(data->set.ssl.certinfo) {
    result = ssl_थप_certinfo_dyn(data, certnum, "Serial Number", &out);
    यदि(result)
      गच्छ done;
  }
  curlx_dyn_reset(&out);

  /* Signature algorithm .*/
  result = dumpAlgo(&out, &param, cert.signatureAlgorithm.beg,
                    cert.signatureAlgorithm.end);
  यदि(result)
    गच्छ done;
  यदि(data->set.ssl.certinfo) {
    result = ssl_थप_certinfo_dyn(data, certnum, "Signature Algorithm", &out);
    यदि(result)
      गच्छ done;
  }
  curlx_dyn_reset(&out);

  /* Start Date. */
  result = ASN1tostr(&out, &cert.notBefore);
  यदि(result)
    गच्छ done;
  यदि(data->set.ssl.certinfo) {
    result = ssl_थप_certinfo_dyn(data, certnum, "Start Date", &out);
    यदि(result)
      गच्छ done;
  }
  curlx_dyn_reset(&out);

  /* Expire Date. */
  result = ASN1tostr(&out, &cert.notAfter);
  यदि(result)
    गच्छ done;
  यदि(data->set.ssl.certinfo) {
    result = ssl_थप_certinfo_dyn(data, certnum, "Expire Date", &out);
    यदि(result)
      गच्छ done;
  }
  curlx_dyn_reset(&out);

  /* Public Key Algorithm. */
  result = dumpAlgo(&out, &param, cert.subject_Public_साँचो_Algorithm.beg,
                    cert.subject_Public_साँचो_Algorithm.end);
  यदि(result)
    गच्छ done;
  यदि(data->set.ssl.certinfo) {
    result = ssl_थप_certinfo_dyn(data, certnum, "Public Key Algorithm",
                                   &out);
    यदि(result)
      गच्छ done;
  }

  rc = do_pubkey(data, certnum, curlx_dyn_ptr(&out),
                 &param, &cert.subject_Public_साँचो);
  यदि(rc) {
    result = CURLE_OUT_OF_MEMORY; /* the most likely error */
    गच्छ done;
  }
  curlx_dyn_reset(&out);

  /* Signature. */
  result = ASN1tostr(&out, &cert.signature);
  यदि(result)
    गच्छ done;
  यदि(data->set.ssl.certinfo) {
    result = ssl_थप_certinfo_dyn(data, certnum, "Signature", &out);
    यदि(result)
      गच्छ done;
  }
  curlx_dyn_reset(&out);

  /* Generate PEM certificate. */
  result = curlx_base64_encode((अचर uint8_t *)cert.certificate.beg,
                               cert.certificate.end - cert.certificate.beg,
                               &certptr, &clen);
  यदि(result)
    गच्छ done;

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
  यदि(!result) {
    माप_प्रकार j = 0;

    जबसम्म(!result && (j < clen)) {
      माप_प्रकार chunksize = (clen - j) > 64 ? 64 : (clen - j);
      result = curlx_dyn_addn(&out, &certptr[j], chunksize);
      यदि(!result)
        result = curlx_dyn_addn(&out, "\n", 1);
      j += chunksize;
    }
    यदि(!result)
      result = curlx_dyn_add(&out, "-----END CERTIFICATE-----\n");
  }
  curlx_free(certptr);
  यदि(!result)
    यदि(data->set.ssl.certinfo)
      result = ssl_थप_certinfo_dyn(data, certnum, "Cert", &out);

done:
  यदि(result)
    failf(data, "Failed extracting certificate chain");
  curlx_dyn_free(&out);
  फर्काऊ result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#endif /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
