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

#जर defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#जर defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_RUSTLS)
#define WANT_PARSEX509 /* uses Curl_parseX509() */
#endif

#जर defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
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
  स्थिरांक अक्षर *numoid;  /* Dotted-numeric OID. */
  स्थिरांक अक्षर *textoid; /* OID name. */
};

/* ASN.1 OIDs. */
स्थिर स्थिरांक संरचना Curl_OID OIDtable[] = {
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

स्थिर स्थिरांक uint8_t *getASN1Element_(संरचना Curl_asn1Element *elem,
                                      स्थिरांक uint8_t *beg, स्थिरांक uint8_t *end,
                                      माप_प्रकार lvl)
{
  uint8_t b;
  माप_प्रकार len;
  संरचना Curl_asn1Element lelem;

  /* Get a single ASN.1 element into `elem', parse ASN.1 string at `beg'
     ending at `end'.
     Returns a pointer in source string after the parsed element, or NULL
     if an error occurs. */
  जर(!beg || !end || beg >= end || !*beg ||
     ((माप_प्रकार)(end - beg) > CURL_ASN1_MAX) ||
     lvl++ >= CURL_ASN1_MAX_RECURSIONS)
    परत रिक्त;

  /* Process header byte. */
  elem->header = beg;
  b = *beg++;
  elem->constructed = (b & 0x20) != 0;
  elem->eclass = (b >> 6) & 3;
  b &= 0x1F;
  जर(b == 0x1F)
    परत रिक्त; /* Long tag values not supported here. */
  elem->tag = b;

  /* Process length. */
  जर(beg >= end)
    परत रिक्त;
  b = *beg++;
  जर(!(b & 0x80))
    len = b;
  अन्यथा {
    b &= 0x7F;
    जर(!b) {
      /* Unspecified length. Since we have all the data, we can determine the
         effective length by skipping element until an EOC element is found. */
      जर(!elem->constructed)
        परत रिक्त;
      elem->beg = beg;
      पर्यंत(beg < end && *beg) {
        beg = getASN1Element_(&lelem, beg, end, lvl);
        जर(!beg)
          परत रिक्त;
      }
      जर(end - beg < 2 || beg[1])       /* EOC must have a zero length. */
        परत रिक्त;
      elem->end = beg;                  /* Do not include EOC as content. */
      परत beg + 2;                   /* After EOC element. */
    }
    अन्यथा जर(b > (माप_प्रकार)(end - beg))
      परत रिक्त; /* Does not fit in source. */
    अन्यथा {
      /* Get long length. */
      len = 0;
      करा {
        जर(len & 0xFF000000L)
          परत रिक्त;  /* Lengths > 32 bits are not supported. */
        len = (len << 8) | *beg++;
      } पर्यंत(--b);
    }
  }
  जर(len > (माप_प्रकार)(end - beg))
    परत रिक्त;  /* Element data does not fit in source. */
  elem->beg = beg;
  elem->end = beg + len;
  परत elem->end;
}

/*
 * @unittest 1657
 */
UNITTEST स्थिरांक uint8_t *getASN1Element(संरचना Curl_asn1Element *elem,
                                       स्थिरांक uint8_t *beg, स्थिरांक uint8_t *end);
UNITTEST स्थिरांक uint8_t *getASN1Element(संरचना Curl_asn1Element *elem,
                                       स्थिरांक uint8_t *beg, स्थिरांक uint8_t *end)
{
  परत getASN1Element_(elem, beg, end, 0);
}

#ifdef WANT_EXTRACT_CERTINFO

/*
 * Search the null-terminated OID or OID identifier in local table.
 * Return the table entry pointer or NULL if not found.
 */
स्थिर स्थिरांक संरचना Curl_OID *शोध_OID(स्थिरांक अक्षर *oid)
{
  स्थिरांक संरचना Curl_OID *op;
  साठी(op = OIDtable; op->numoid; op++)
    जर(!म_भेद(op->numoid, oid) || curl_strequal(op->textoid, oid))
      परत op;

  परत रिक्त;
}

/*
 * Convert an ASN.1 Boolean value into its string representation.
 *
 * Return error code.
 */

स्थिर CURLcode bool2str(संरचना dynbuf *store,
                         स्थिरांक uint8_t *beg, स्थिरांक uint8_t *end)
{
  uint8_t val;
  जर(end - beg != 1)
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  val = *beg;
  जर((val != 0xff) && (val != 0x00))
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  परत curlx_dyn_add(store, val ? "TRUE" : "FALSE");
}

/*
 * Convert an ASN.1 octet string to a printable string.
 *
 * Return error code.
 */
स्थिर CURLcode octet2str(संरचना dynbuf *store,
                          स्थिरांक uint8_t *beg, स्थिरांक uint8_t *end)
{
  CURLcode result = CURLE_OK;

  पर्यंत(!result && beg < end)
    result = curlx_dyn_addf(store, "%02x:", *beg++);

  परत result;
}

स्थिर CURLcode bit2str(संरचना dynbuf *store,
                        स्थिरांक uint8_t *beg, स्थिरांक uint8_t *end)
{
  /* Convert an ASN.1 bit string to a printable string. */

  जर(beg < end) {
    uint8_t unused_bits = *beg++;
    जर(unused_bits > 7)
      परत CURLE_BAD_FUNCTION_ARGUMENT;
  }
  अन्यथा
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  परत octet2str(store, beg, end);
}

/*
 * Convert an ASN.1 integer value into its string representation.
 *
 * Returns error.
 */
स्थिर CURLcode int2str(संरचना dynbuf *store,
                        स्थिरांक uint8_t *beg, स्थिरांक uint8_t *end)
{
  uint32_t uval = 0;
  int32_t sval = 0;
  माप_प्रकार n = end - beg;
  माप_प्रकार i;
  स्थिरांक uint8_t *p = beg;

  जर(!n)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  जर(n > 4)
    परत octet2str(store, beg, end);

  जर(p[0] & 0x80)
    uval = ~0U;

  साठी(i = 0; i < n; i++)
    uval = (uval << 8) | p[i];

  sval = (int32_t)uval;

  /* small values are easier to read as decimals */
  जर(sval > -10000 && sval < 10000)
    परत curlx_dyn_addf(store, "%d", sval);

  /* represent larger values as hex */
  परत curlx_dyn_addf(store, "0x%x", uval);
}

/*
 * Convert from an ASN.1 typed string to UTF8.
 *
 * The result is stored in a dynbuf that is inited by the user of this
 * function.
 *
 * Returns error.
 */
स्थिर CURLcode utf8asn1str(संरचना dynbuf *to, पूर्णांक type,
                            स्थिरांक uint8_t *from, स्थिरांक uint8_t *end)
{
  माप_प्रकार inlength = end - from;
  पूर्णांक size = 1;
  CURLcode result = CURLE_OK;

  निवड(type) {
  स्थिती CURL_ASN1_BMP_STRING:
    size = 2;
    थांबा;
  स्थिती CURL_ASN1_UNIVERSAL_STRING:
    size = 4;
    थांबा;
  स्थिती CURL_ASN1_NUMERIC_STRING:
  स्थिती CURL_ASN1_PRINTABLE_STRING:
  स्थिती CURL_ASN1_TELETEX_STRING:
  स्थिती CURL_ASN1_IA5_STRING:
  स्थिती CURL_ASN1_VISIBLE_STRING:
  स्थिती CURL_ASN1_UTF8_STRING:
    थांबा;
  पूर्वनिर्धारित:
    परत CURLE_BAD_FUNCTION_ARGUMENT;  /* Conversion not supported. */
  }

  जर(inlength % size)
    /* Length inconsistent with character size. */
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  जर(type == CURL_ASN1_UTF8_STRING) {
    /* copy. */
    जर(inlength)
      result = curlx_dyn_addn(to, from, inlength);
  }
  अन्यथा {
    पर्यंत(!result && (from < end)) {
      अक्षर buf[4]; /* decode buffer */
      माप_प्रकार charsize = 1;
      अचिन्हित पूर्णांक wc = 0;

      निवड(size) {
      स्थिती 4:
        wc = (wc << 8) | *from++;
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      स्थिती 2:
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      पूर्वनिर्धारित: /* case 1: */
        wc = (wc << 8) | *from++;
      }
      जर(wc >= 0x00000080) {
        जर(wc >= 0x00000800) {
          जर(wc >= 0x00010000) {
            जर(wc >= 0x00200000) {
              /* Invalid char. size for target encoding. */
              परत CURLE_WEIRD_SERVER_REPLY;
            }
            buf[3] = (अक्षर)(0x80 | (wc & 0x3F));
            wc = (wc >> 6) | 0x00010000;
            charsize++;
          }
          buf[2] = (अक्षर)(0x80 | (wc & 0x3F));
          wc = (wc >> 6) | 0x00000800;
          charsize++;
        }
        buf[1] = (अक्षर)(0x80 | (wc & 0x3F));
        wc = (wc >> 6) | 0x000000C0;
        charsize++;
      }
      buf[0] = (अक्षर)wc;
      result = curlx_dyn_addn(to, buf, charsize);
    }
  }
  परत result;
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
                            स्थिरांक uint8_t *beg, स्थिरांक uint8_t *end);
UNITTEST CURLcode encodeOID(संरचना dynbuf *store,
                            स्थिरांक uint8_t *beg, स्थिरांक uint8_t *end)
{
  uint32_t x;
  uint32_t y;
  CURLcode result = CURLE_OK;

  जर(end <= beg)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  /* Process the first two numbers. The initial digit cannot be larger than
     2 */
  y = *beg++;
  जर(y <= 80)
    x = y / 40;
  अन्यथा
    x = 2;
  result = curlx_dyn_addf(store, "%u", x);
  जर(result)
    परत result;

  जर(y & 0x80) {
    uint32_t t = (y & 0x7f); /* the second number */
    जर(!t)
      /* reject leading 0x80 padding */
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    करा {
      जर((t & 0xFE000000) || (beg >= end))
        परत CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      t = (t << 7) | (y & 0x7F);
    } पर्यंत(y & 0x80);
    y = t;
  }

  result = curlx_dyn_addf(store, ".%u", y - (x * 40));
  जर(result)
    परत result;

  /* Process the trailing numbers. */
  पर्यंत((beg < end) && !result) {
    x = 0;
    y = 0;
    करा {
      /* 0x80 as the first value is invalid since it only adds padding */
      जर((y & 0x80) && !x)
        परत CURLE_BAD_FUNCTION_ARGUMENT;
      अन्यथा जर((x & 0xFE000000) || (beg == end))
        परत CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      x = (x << 7) | (y & 0x7F);
    } पर्यंत(y & 0x80);
    result = curlx_dyn_addf(store, ".%u", x);
  }
  परत result;
}

/*
 * Convert an ASN.1 OID into its dotted or symbolic string representation.
 *
 * Return error code.
 */

स्थिर CURLcode OID2str(संरचना dynbuf *store,
                        स्थिरांक uint8_t *beg, स्थिरांक uint8_t *end)
{
  CURLcode result = CURLE_OK;
  जर(beg < end) {
    संरचना dynbuf buf;
    curlx_dyn_init(&buf, CURL_X509_STR_MAX);
    result = encodeOID(&buf, beg, end);

    जर(!result) {
      स्थिरांक संरचना Curl_OID *op = शोध_OID(curlx_dyn_ptr(&buf));
      जर(op)
        result = curlx_dyn_add(store, op->textoid);
      अन्यथा
        result = curlx_dyn_add(store, curlx_dyn_ptr(&buf));
    }
    curlx_dyn_free(&buf);
  }
  परत result;
}

/*
 * @unittest 1656
 */
UNITTEST CURLcode GTime2str(संरचना dynbuf *store,
                            स्थिरांक अक्षर *beg, स्थिरांक अक्षर *end);
UNITTEST CURLcode GTime2str(संरचना dynbuf *store,
                            स्थिरांक अक्षर *beg, स्थिरांक अक्षर *end)
{
  स्थिरांक अक्षर *tzp;
  स्थिरांक अक्षर *fracp;
  अक्षर sec1, sec2;
  माप_प्रकार fracl;
  माप_प्रकार tzl;
  स्थिरांक अक्षर *sep = "";

  /* Convert an ASN.1 Generalized time to a printable string.
     Return the dynamically allocated string, or NULL if an error occurs. */

  साठी(fracp = beg; fracp < end && ISDIGIT(*fracp); fracp++)
    ;

  /* Get seconds digits. */
  sec1 = '0';
  निवड(fracp - beg - 12) {
  स्थिती 0:
    sec2 = '0';
    थांबा;
  स्थिती 2:
    sec1 = fracp[-2];
    FALLTHROUGH();
  स्थिती 1:
    sec2 = fracp[-1];
    थांबा;
  पूर्वनिर्धारित:
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* timezone follows optional fractional seconds. */
  tzp = fracp;
  fracl = 0; /* no fractional seconds detected so far */
  जर(fracp < end && (*fracp == '.' || *fracp == ',')) {
    /* Have fractional seconds, e.g. "[.,]\d+". How many? */
    fracp++; /* should be a digit char or BAD ARGUMENT */
    tzp = fracp;
    पर्यंत(tzp < end && ISDIGIT(*tzp))
      tzp++;
    जर(tzp == fracp) /* never looped, no digit after [.,] */
      परत CURLE_BAD_FUNCTION_ARGUMENT;
    fracl = tzp - fracp; /* number of fractional sec digits */
    DEBUGASSERT(fracl > 0);
    /* Strip trailing zeroes in fractional seconds.
     * May reduce fracl to 0 if only '0's are present. */
    पर्यंत(fracl && fracp[fracl - 1] == '0')
      fracl--;
  }

  /* Process timezone. */
  जर(tzp >= end) {
    tzp = "";
    tzl = 0;
  }
  अन्यथा जर(*tzp == 'Z') {
    sep = " ";
    tzp = "GMT";
    tzl = 3;
  }
  अन्यथा जर((*tzp == '+') || (*tzp == '-')) {
    sep = " UTC";
    tzl = end - tzp;
  }
  अन्यथा {
    sep = " ";
    tzl = end - tzp;
  }

  परत curlx_dyn_addf(store,
                        "%.4s-%.2s-%.2s %.2s:%.2s:%c%c%s%.*s%s%.*s",
                        beg, beg + 4, beg + 6,
                        beg + 8, beg + 10, sec1, sec2,
                        fracl ? "." : "", (पूर्णांक)fracl, fracp,
                        sep, (पूर्णांक)tzl, tzp);
}

/*
 * Convert an ASN.1 UTC time to a printable string.
 *
 * Return error code.
 */
स्थिर CURLcode UTime2str(संरचना dynbuf *store,
                          स्थिरांक अक्षर *beg, स्थिरांक अक्षर *end)
{
  स्थिरांक अक्षर *tzp;
  माप_प्रकार tzl;
  स्थिरांक अक्षर *sec;

  साठी(tzp = beg; tzp < end && *tzp >= '0' && *tzp <= '9'; tzp++)
    ;
  /* Get the seconds. */
  sec = beg + 10;
  निवड(tzp - sec) {
  स्थिती 0:
    sec = "00";
    FALLTHROUGH();
  स्थिती 2:
    थांबा;
  पूर्वनिर्धारित:
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Process timezone. */
  जर(tzp >= end)
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  जर(*tzp == 'Z') {
    tzp = "GMT";
    end = tzp + 3;
  }

  tzl = end - tzp;
  परत curlx_dyn_addf(store, "%d%.2s-%.2s-%.2s %.2s:%.2s:%.2s %.*s",
                        20 - (*beg >= '5'), beg, beg + 2, beg + 4,
                        beg + 6, beg + 8, sec,
                        (पूर्णांक)tzl, tzp);
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
  पूर्णांक type = elem->tag;
  जर(elem->constructed)
    परत result; /* No conversion of structured elements. */

  निवड(type) {
  स्थिती CURL_ASN1_BOOLEAN:
    result = bool2str(store, elem->beg, elem->end);
    थांबा;
  स्थिती CURL_ASN1_INTEGER:
  स्थिती CURL_ASN1_ENUMERATED:
    result = int2str(store, elem->beg, elem->end);
    थांबा;
  स्थिती CURL_ASN1_BIT_STRING:
    result = bit2str(store, elem->beg, elem->end);
    थांबा;
  स्थिती CURL_ASN1_OCTET_STRING:
    result = octet2str(store, elem->beg, elem->end);
    थांबा;
  स्थिती CURL_ASN1_NULL:
    result = curlx_dyn_addn(store, "", 1);
    थांबा;
  स्थिती CURL_ASN1_OBJECT_IDENTIFIER:
    result = OID2str(store, elem->beg, elem->end);
    थांबा;
  स्थिती CURL_ASN1_UTC_TIME:
    result = UTime2str(store, (स्थिरांक अक्षर *)elem->beg,
                       (स्थिरांक अक्षर *)elem->end);
    थांबा;
  स्थिती CURL_ASN1_GENERALIZED_TIME:
    result = GTime2str(store, (स्थिरांक अक्षर *)elem->beg,
                       (स्थिरांक अक्षर *)elem->end);
    थांबा;
  स्थिती CURL_ASN1_UTF8_STRING:
  स्थिती CURL_ASN1_NUMERIC_STRING:
  स्थिती CURL_ASN1_PRINTABLE_STRING:
  स्थिती CURL_ASN1_TELETEX_STRING:
  स्थिती CURL_ASN1_IA5_STRING:
  स्थिती CURL_ASN1_VISIBLE_STRING:
  स्थिती CURL_ASN1_UNIVERSAL_STRING:
  स्थिती CURL_ASN1_BMP_STRING:
    result = utf8asn1str(store, type, elem->beg, elem->end);
    थांबा;
  }

  परत result;
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
  संरचना Curl_asn1Element मूल्य;
  स्थिरांक uint8_t *p1;
  स्थिरांक uint8_t *p2;
  स्थिरांक uint8_t *p3;
  स्थिरांक uint8_t *str;
  CURLcode result = CURLE_OK;
  bool added = FALSE;
  संरचना dynbuf temp;
  curlx_dyn_init(&temp, CURL_X509_STR_MAX);

  साठी(p1 = dn->beg; p1 < dn->end;) {
    p1 = getASN1Element(&rdn, p1, dn->end);
    जर(!p1) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      जा error;
    }
    साठी(p2 = rdn.beg; p2 < rdn.end;) {
      p2 = getASN1Element(&atv, p2, rdn.end);
      जर(!p2) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        जा error;
      }
      p3 = getASN1Element(&oid, atv.beg, atv.end);
      जर(!p3) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        जा error;
      }
      जर(!getASN1Element(&मूल्य, p3, atv.end)) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        जा error;
      }
      curlx_dyn_reset(&temp);
      result = ASN1tostr(&temp, &oid);
      जर(result)
        जा error;

      str = curlx_dyn_uptr(&temp);

      जर(!str) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        जा error;
      }

      /* Encode delimiter.
         If attribute has a short uppercase name, delimiter is ", ". */
      साठी(p3 = str; ISUPPER(*p3); p3++)
        ;
      जर(added) {
        जर(p3 - str > 2)
          result = curlx_dyn_addn(store, "/", 1);
        अन्यथा
          result = curlx_dyn_addn(store, ", ", 2);
        जर(result)
          जा error;
      }

      /* Encode attribute name. */
      result = curlx_dyn_add(store, (स्थिरांक अक्षर *)str);
      जर(result)
        जा error;

      /* Generate equal sign. */
      result = curlx_dyn_addn(store, "=", 1);
      जर(result)
        जा error;

      /* Generate value. */
      result = ASN1tostr(store, &मूल्य);
      जर(result)
        जा error;
      curlx_dyn_reset(&temp);
      added = TRUE; /* use separator for next */
    }
  }
error:
  curlx_dyn_free(&temp);

  परत result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#ifdef WANT_PARSEX509
/*
 * ASN.1 parse an X509 certificate into structure subfields.
 * Syntax is assumed to have already been checked by the SSL backend.
 * See RFC 5280.
 */
पूर्णांक Curl_parseX509(संरचना Curl_X509certificate *cert,
                   स्थिरांक uint8_t *beg, स्थिरांक uint8_t *end)
{
  संरचना Curl_asn1Element elem;
  संरचना Curl_asn1Element tbsCertificate;
  स्थिरांक uint8_t *ccp;
  स्थिर स्थिरांक uint8_t defaultVersion = 0;  /* v1. */

  स_रखो(cert, 0, आकार(*cert));
  cert->certificate.header = रिक्त;
  cert->certificate.beg = beg;
  cert->certificate.end = end;

  /* Get the sequence content. */
  जर(!getASN1Element(&elem, beg, end))
    परत -1;  /* Invalid bounds/size. */
  beg = elem.beg;
  end = elem.end;

  /* Get tbsCertificate. */
  beg = getASN1Element(&tbsCertificate, beg, end);
  जर(!beg)
    परत -1;
  /* Skip the signatureAlgorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  जर(!beg)
    परत -1;
  /* Get the signatureValue. */
  जर(!getASN1Element(&cert->signature, beg, end))
    परत -1;

  /* Parse TBSCertificate. */
  beg = tbsCertificate.beg;
  end = tbsCertificate.end;
  /* Get optional version, get serialNumber. */
  cert->version.header = रिक्त;
  cert->version.beg = &defaultVersion;
  cert->version.end = &defaultVersion + आकार(defaultVersion);
  beg = getASN1Element(&elem, beg, end);
  जर(!beg)
    परत -1;
  जर(elem.tag == 0) {
    जर(!getASN1Element(&cert->version, elem.beg, elem.end))
      परत -1;
    beg = getASN1Element(&elem, beg, end);
    जर(!beg)
      परत -1;
  }
  cert->serial_संख्या = elem;
  /* Get signature algorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  /* Get issuer. */
  beg = getASN1Element(&cert->issuer, beg, end);
  जर(!beg)
    परत -1;
  /* Get notBefore and notAfter. */
  beg = getASN1Element(&elem, beg, end);
  जर(!beg)
    परत -1;
  ccp = getASN1Element(&cert->notBefore, elem.beg, elem.end);
  जर(!ccp)
    परत -1;
  जर(!getASN1Element(&cert->notAfter, ccp, elem.end))
    परत -1;
  /* These two fields MUST consume all of the validity elem */
  जर(elem.end != cert->notAfter.end)
    परत -1;
  /* Get subject. */
  beg = getASN1Element(&cert->subject, beg, end);
  जर(!beg)
    परत -1;
  /* Get subjectPublicKeyAlgorithm and subjectPublicKey. */
  beg = getASN1Element(&cert->subject_Public_किल्ली_Info, beg, end);
  जर(!beg)
    परत -1;
  ccp = getASN1Element(&cert->subject_Public_किल्ली_Algorithm,
                       cert->subject_Public_किल्ली_Info.beg,
                       cert->subject_Public_किल्ली_Info.end);
  जर(!ccp)
    परत -1;
  जर(!getASN1Element(&cert->subject_Public_किल्ली, ccp,
                     cert->subject_Public_किल्ली_Info.end))
    परत -1;
  /* Get optional issuerUniqueID, subjectUniqueID and extensions. */
  cert->issuerUniqueID.tag = cert->subjectUniqueID.tag = 0;
  cert->extensions.tag = elem.tag = 0;
  cert->issuerUniqueID.header = cert->subjectUniqueID.header = रिक्त;
  cert->issuerUniqueID.beg = cert->issuerUniqueID.end = beg;
  cert->subjectUniqueID.beg = cert->subjectUniqueID.end = beg;
  cert->extensions.header = रिक्त;
  cert->extensions.beg = cert->extensions.end = beg;
  जर(beg < end) {
    beg = getASN1Element(&elem, beg, end);
    जर(!beg)
      परत -1;
  }
  जर(elem.tag == 1) {
    cert->issuerUniqueID = elem;
    जर(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      जर(!beg)
        परत -1;
    }
  }
  जर(elem.tag == 2) {
    cert->subjectUniqueID = elem;
    जर(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      जर(!beg)
        परत -1;
    }
  }
  जर(elem.tag == 3)
    जर(!getASN1Element(&cert->extensions, elem.beg, elem.end))
      परत -1;
  परत 0;
}

#endif /* WANT_PARSEX509 */

#ifdef WANT_EXTRACT_CERTINFO

स्थिर CURLcode dumpAlgo(संरचना dynbuf *store,
                         संरचना Curl_asn1Element *param,
                         स्थिरांक uint8_t *beg, स्थिरांक uint8_t *end)
{
  संरचना Curl_asn1Element oid;

  /* Get algorithm parameters and return algorithm name. */

  beg = getASN1Element(&oid, beg, end);
  जर(!beg)
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  param->header = रिक्त;
  param->tag = 0;
  param->beg = param->end = end;
  जर(beg < end) {
    स्थिरांक uint8_t *p = getASN1Element(param, beg, end);
    जर(!p)
      परत CURLE_BAD_FUNCTION_ARGUMENT;
  }
  परत OID2str(store, oid.beg, oid.end);
}

/*
 * This is a convenience function for push_certinfo_len that takes a zero
 * terminated value.
 */
स्थिर CURLcode ssl_ढकला_certinfo(संरचना Curl_easy *data,
                                  पूर्णांक certnum,
                                  स्थिरांक अक्षर *label,
                                  स्थिरांक अक्षर *मूल्य)
{
  माप_प्रकार valuelen = म_माप(मूल्य);

  परत Curl_ssl_ढकला_certinfo_len(data, certnum, label, मूल्य, valuelen);
}

/*
 * This is a convenience function for push_certinfo_len that takes a
 * dynbuf value.
 *
 * It also does the verbose output if !certnum.
 */
स्थिर CURLcode ssl_ढकला_certinfo_dyn(संरचना Curl_easy *data,
                                      पूर्णांक certnum,
                                      स्थिरांक अक्षर *label,
                                      संरचना dynbuf *ptr)
{
  माप_प्रकार valuelen = curlx_dyn_len(ptr);
  स्थिरांक अक्षर *मूल्य = curlx_dyn_ptr(ptr);

  CURLcode result = Curl_ssl_ढकला_certinfo_len(data, certnum, label,
                                               मूल्य, valuelen);

  जर(!certnum && !result)
    infof(data, "   %s: %s", label, मूल्य);

  परत result;
}

स्थिर CURLcode do_pubkey_field(संरचना Curl_easy *data, पूर्णांक certnum,
                                स्थिरांक अक्षर *label,
                                संरचना Curl_asn1Element *elem)
{
  CURLcode result;
  संरचना dynbuf out;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);

  /* Generate a certificate information record for the public key. */

  result = ASN1tostr(&out, elem);
  जर(!result && data->set.ssl.certinfo)
    result = ssl_ढकला_certinfo_dyn(data, certnum, label, &out);
  curlx_dyn_free(&out);
  परत result;
}

/* return 0 on success, 1 on error */
स्थिर पूर्णांक do_pubkey(संरचना Curl_easy *data, पूर्णांक certnum, स्थिरांक अक्षर *algo,
                     संरचना Curl_asn1Element *param,
                     संरचना Curl_asn1Element *pubkey)
{
  संरचना Curl_asn1Element elem;
  संरचना Curl_asn1Element pk;
  स्थिरांक uint8_t *p;

  /* Generate all information records for the public key. */

  जर(curl_strequal(algo, "ecPublicKey")) {
    /*
     * ECC public key is all the data, a value of type BIT STRING mapped to
     * OCTET STRING and should not be parsed as an ASN.1 value.
     */
    स्थिरांक माप_प्रकार dlen = pubkey->end - pubkey->beg;
    माप_प्रकार len;
    जर(dlen < 2)
      /* too small */
      परत 1;
    len = (dlen - 2) * 4;
    जर(!certnum)
      infof(data, "   ECC Public Key (%zu bits)", len);
    जर(data->set.ssl.certinfo) {
      अक्षर q[(आकार(len) * 8 / 3) + 1];
      (शून्य)curl_msnprintf(q, आकार(q), "%zu", len);
      जर(ssl_ढकला_certinfo(data, certnum, "ECC Public Key", q))
        परत 1;
    }
    परत do_pubkey_field(data, certnum, "ecPublicKey", pubkey) == CURLE_OK
      ? 0 : 1;
  }

  /* Get the public key (single element). */
  जर(!getASN1Element(&pk, pubkey->beg + 1, pubkey->end))
    परत 1;

  जर(curl_strequal(algo, "rsaEncryption")) {
    स्थिरांक uint8_t *q;
    माप_प्रकार len;

    p = getASN1Element(&elem, pk.beg, pk.end);
    जर(!p)
      परत 1;

    /* Compute key length. */
    साठी(q = elem.beg; q < elem.end && !*q; q++)
      ;
    len = ((elem.end - q) * 8);
    जर(len) {
      अचिन्हित पूर्णांक i;
      साठी(i = *q; !(i & 0x80); i <<= 1)
        len--;
    }
    जर(len > 32)
      elem.beg = q;     /* Strip leading zero bytes. */
    जर(!certnum)
      infof(data, "   RSA Public Key (%zu bits)", len);
    जर(data->set.ssl.certinfo) {
      अक्षर r[(आकार(len) * 8 / 3) + 1];
      curl_msnprintf(r, आकार(r), "%zu", len);
      जर(ssl_ढकला_certinfo(data, certnum, "RSA Public Key", r))
        परत 1;
    }
    /* Generate coefficients. */
    जर(do_pubkey_field(data, certnum, "rsa(n)", &elem))
      परत 1;
    जर(!getASN1Element(&elem, p, pk.end))
      परत 1;
    जर(do_pubkey_field(data, certnum, "rsa(e)", &elem))
      परत 1;
  }
  अन्यथा जर(curl_strequal(algo, "dsa")) {
    p = getASN1Element(&elem, param->beg, param->end);
    जर(p) {
      जर(do_pubkey_field(data, certnum, "dsa(p)", &elem))
        परत 1;
      p = getASN1Element(&elem, p, param->end);
      जर(p) {
        जर(do_pubkey_field(data, certnum, "dsa(q)", &elem))
          परत 1;
        जर(getASN1Element(&elem, p, param->end)) {
          जर(do_pubkey_field(data, certnum, "dsa(g)", &elem))
            परत 1;
          जर(do_pubkey_field(data, certnum, "dsa(pub_key)", &pk))
            परत 1;
        }
      }
    }
  }
  अन्यथा जर(curl_strequal(algo, "dhpublicnumber")) {
    p = getASN1Element(&elem, param->beg, param->end);
    जर(p) {
      जर(do_pubkey_field(data, certnum, "dh(p)", &elem))
        परत 1;
      जर(getASN1Element(&elem, p, param->end)) {
        जर(do_pubkey_field(data, certnum, "dh(g)", &elem))
          परत 1;
        जर(do_pubkey_field(data, certnum, "dh(pub_key)", &pk))
          परत 1;
      }
    }
  }
  परत 0;
}

/*
 * Convert an ASN.1 distinguished name into a printable string.
 * Return error.
 */
स्थिर CURLcode DNtostr(संरचना dynbuf *store, संरचना Curl_asn1Element *dn)
{
  परत encodeDN(store, dn);
}

CURLcode Curl_extract_certinfo(संरचना Curl_easy *data,
                               पूर्णांक certnum,
                               स्थिरांक uint8_t *beg,
                               स्थिरांक uint8_t *end)
{
  संरचना Curl_X509certificate cert;
  संरचना Curl_asn1Element param;
  अक्षर *certptr;
  माप_प्रकार clen;
  संरचना dynbuf out;
  CURLcode result = CURLE_OK;
  अचिन्हित पूर्णांक version;
  स्थिरांक uint8_t *ptr;
  पूर्णांक rc;

  जर(!data->set.ssl.certinfo)
    जर(certnum)
      परत CURLE_OK;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);
  /* Prepare the certificate information for curl_easy_getinfo(). */

  /* Extract the certificate ASN.1 elements. */
  जर(Curl_parseX509(&cert, beg, end))
    परत CURLE_PEER_FAILED_VERIFICATION;

  /* Subject. */
  result = DNtostr(&out, &cert.subject);
  जर(result)
    जा done;
  जर(data->set.ssl.certinfo) {
    result = ssl_ढकला_certinfo_dyn(data, certnum, "Subject", &out);
    जर(result)
      जा done;
  }
  curlx_dyn_reset(&out);

  /* Issuer. */
  result = DNtostr(&out, &cert.issuer);
  जर(result)
    जा done;
  जर(data->set.ssl.certinfo) {
    result = ssl_ढकला_certinfo_dyn(data, certnum, "Issuer", &out);
    जर(result)
      जा done;
  }
  curlx_dyn_reset(&out);

  /* Version (always fits in less than 32 bits). */
  version = 0;
  साठी(ptr = cert.version.beg; ptr < cert.version.end; ptr++)
    version = (version << 8) | *ptr;
  जर(data->set.ssl.certinfo) {
    result = curlx_dyn_addf(&out, "%x", version);
    जर(result)
      जा done;
    result = ssl_ढकला_certinfo_dyn(data, certnum, "Version", &out);
    जर(result)
      जा done;
    curlx_dyn_reset(&out);
  }

  /* Serial number. */
  result = ASN1tostr(&out, &cert.serial_संख्या);
  जर(result)
    जा done;
  जर(data->set.ssl.certinfo) {
    result = ssl_ढकला_certinfo_dyn(data, certnum, "Serial Number", &out);
    जर(result)
      जा done;
  }
  curlx_dyn_reset(&out);

  /* Signature algorithm .*/
  result = dumpAlgo(&out, &param, cert.signatureAlgorithm.beg,
                    cert.signatureAlgorithm.end);
  जर(result)
    जा done;
  जर(data->set.ssl.certinfo) {
    result = ssl_ढकला_certinfo_dyn(data, certnum, "Signature Algorithm", &out);
    जर(result)
      जा done;
  }
  curlx_dyn_reset(&out);

  /* Start Date. */
  result = ASN1tostr(&out, &cert.notBefore);
  जर(result)
    जा done;
  जर(data->set.ssl.certinfo) {
    result = ssl_ढकला_certinfo_dyn(data, certnum, "Start Date", &out);
    जर(result)
      जा done;
  }
  curlx_dyn_reset(&out);

  /* Expire Date. */
  result = ASN1tostr(&out, &cert.notAfter);
  जर(result)
    जा done;
  जर(data->set.ssl.certinfo) {
    result = ssl_ढकला_certinfo_dyn(data, certnum, "Expire Date", &out);
    जर(result)
      जा done;
  }
  curlx_dyn_reset(&out);

  /* Public Key Algorithm. */
  result = dumpAlgo(&out, &param, cert.subject_Public_किल्ली_Algorithm.beg,
                    cert.subject_Public_किल्ली_Algorithm.end);
  जर(result)
    जा done;
  जर(data->set.ssl.certinfo) {
    result = ssl_ढकला_certinfo_dyn(data, certnum, "Public Key Algorithm",
                                   &out);
    जर(result)
      जा done;
  }

  rc = do_pubkey(data, certnum, curlx_dyn_ptr(&out),
                 &param, &cert.subject_Public_किल्ली);
  जर(rc) {
    result = CURLE_OUT_OF_MEMORY; /* the most likely error */
    जा done;
  }
  curlx_dyn_reset(&out);

  /* Signature. */
  result = ASN1tostr(&out, &cert.signature);
  जर(result)
    जा done;
  जर(data->set.ssl.certinfo) {
    result = ssl_ढकला_certinfo_dyn(data, certnum, "Signature", &out);
    जर(result)
      जा done;
  }
  curlx_dyn_reset(&out);

  /* Generate PEM certificate. */
  result = curlx_base64_encode((स्थिरांक uint8_t *)cert.certificate.beg,
                               cert.certificate.end - cert.certificate.beg,
                               &certptr, &clen);
  जर(result)
    जा done;

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
  जर(!result) {
    माप_प्रकार j = 0;

    पर्यंत(!result && (j < clen)) {
      माप_प्रकार chunksize = (clen - j) > 64 ? 64 : (clen - j);
      result = curlx_dyn_addn(&out, &certptr[j], chunksize);
      जर(!result)
        result = curlx_dyn_addn(&out, "\n", 1);
      j += chunksize;
    }
    जर(!result)
      result = curlx_dyn_add(&out, "-----END CERTIFICATE-----\n");
  }
  curlx_free(certptr);
  जर(!result)
    जर(data->set.ssl.certinfo)
      result = ssl_ढकला_certinfo_dyn(data, certnum, "Cert", &out);

done:
  जर(result)
    failf(data, "Failed extracting certificate chain");
  curlx_dyn_free(&out);
  परत result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#endif /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
