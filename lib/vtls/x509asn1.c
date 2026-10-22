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
#அந்தர்பாவ "curl_setup.h"

#எனில் defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#எனில் defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_RUSTLS)
#define WANT_PARSEX509 /* uses Curl_parseX509() */
#endif

#எனில் defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_RUSTLS)
#define WANT_EXTRACT_CERTINFO /* uses Curl_extract_certinfo() */
#endif

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "curlx/base64.h"
#அந்தர்பாவ "vtls/x509asn1.h"
#அந்தர்பாவ "curlx/dynbuf.h"

/*
 * Constants.
 */

/* Largest supported ASN.1 structure. */
#define CURL_ASN1_MAX                   ((size_t)0x40000)      /* 256K */

#ifdef WANT_EXTRACT_CERTINFO

/* ASN.1 OID table entry. */
கட்டமைப்பு Curl_OID {
  மாறிலி எழுத்து *numoid;  /* Dotted-numeric OID. */
  மாறிலி எழுத்து *textoid; /* OID name. */
};

/* ASN.1 OIDs. */
நிலையான மாறிலி கட்டமைப்பு Curl_OID OIDtable[] = {
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
  { வெற்று,                       வெற்று }
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

நிலையான மாறிலி uint8_t *getASN1Element_(கட்டமைப்பு Curl_asn1Element *elem,
                                      மாறிலி uint8_t *beg, மாறிலி uint8_t *end,
                                      size_t lvl)
{
  uint8_t b;
  size_t len;
  கட்டமைப்பு Curl_asn1Element lelem;

  /* Get a single ASN.1 element into `elem', parse ASN.1 string at `beg'
     ending at `end'.
     Returns a pointer in source string after the parsed element, or NULL
     if an error occurs. */
  எனில்(!beg || !end || beg >= end || !*beg ||
     ((size_t)(end - beg) > CURL_ASN1_MAX) ||
     lvl++ >= CURL_ASN1_MAX_RECURSIONS)
    திரும்பு வெற்று;

  /* Process header byte. */
  elem->header = beg;
  b = *beg++;
  elem->constructed = (b & 0x20) != 0;
  elem->eclass = (b >> 6) & 3;
  b &= 0x1F;
  எனில்(b == 0x1F)
    திரும்பு வெற்று; /* Long tag values not supported here. */
  elem->tag = b;

  /* Process length. */
  எனில்(beg >= end)
    திரும்பு வெற்று;
  b = *beg++;
  எனில்(!(b & 0x80))
    len = b;
  இல்லையெனில் {
    b &= 0x7F;
    எனில்(!b) {
      /* Unspecified length. Since we have all the data, we can determine the
         effective length by skipping element until an EOC element is found. */
      எனில்(!elem->constructed)
        திரும்பு வெற்று;
      elem->beg = beg;
      போது(beg < end && *beg) {
        beg = getASN1Element_(&lelem, beg, end, lvl);
        எனில்(!beg)
          திரும்பு வெற்று;
      }
      எனில்(end - beg < 2 || beg[1])       /* EOC must have a zero length. */
        திரும்பு வெற்று;
      elem->end = beg;                  /* Do not include EOC as content. */
      திரும்பு beg + 2;                   /* After EOC element. */
    }
    இல்லையெனில் எனில்(b > (size_t)(end - beg))
      திரும்பு வெற்று; /* Does not fit in source. */
    இல்லையெனில் {
      /* Get long length. */
      len = 0;
      செய் {
        எனில்(len & 0xFF000000L)
          திரும்பு வெற்று;  /* Lengths > 32 bits are not supported. */
        len = (len << 8) | *beg++;
      } போது(--b);
    }
  }
  எனில்(len > (size_t)(end - beg))
    திரும்பு வெற்று;  /* Element data does not fit in source. */
  elem->beg = beg;
  elem->end = beg + len;
  திரும்பு elem->end;
}

/*
 * @unittest 1657
 */
UNITTEST மாறிலி uint8_t *getASN1Element(கட்டமைப்பு Curl_asn1Element *elem,
                                       மாறிலி uint8_t *beg, மாறிலி uint8_t *end);
UNITTEST மாறிலி uint8_t *getASN1Element(கட்டமைப்பு Curl_asn1Element *elem,
                                       மாறிலி uint8_t *beg, மாறிலி uint8_t *end)
{
  திரும்பு getASN1Element_(elem, beg, end, 0);
}

#ifdef WANT_EXTRACT_CERTINFO

/*
 * Search the null-terminated OID or OID identifier in local table.
 * Return the table entry pointer or NULL if not found.
 */
நிலையான மாறிலி கட்டமைப்பு Curl_OID *தேடல்_OID(மாறிலி எழுத்து *oid)
{
  மாறிலி கட்டமைப்பு Curl_OID *op;
  க்கு(op = OIDtable; op->numoid; op++)
    எனில்(!strcmp(op->numoid, oid) || curl_strequal(op->textoid, oid))
      திரும்பு op;

  திரும்பு வெற்று;
}

/*
 * Convert an ASN.1 Boolean value into its string representation.
 *
 * Return error code.
 */

நிலையான CURLcode bool2str(கட்டமைப்பு dynbuf *store,
                         மாறிலி uint8_t *beg, மாறிலி uint8_t *end)
{
  uint8_t val;
  எனில்(end - beg != 1)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  val = *beg;
  எனில்((val != 0xff) && (val != 0x00))
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  திரும்பு curlx_dyn_add(store, val ? "TRUE" : "FALSE");
}

/*
 * Convert an ASN.1 octet string to a printable string.
 *
 * Return error code.
 */
நிலையான CURLcode octet2str(கட்டமைப்பு dynbuf *store,
                          மாறிலி uint8_t *beg, மாறிலி uint8_t *end)
{
  CURLcode result = CURLE_OK;

  போது(!result && beg < end)
    result = curlx_dyn_addf(store, "%02x:", *beg++);

  திரும்பு result;
}

நிலையான CURLcode bit2str(கட்டமைப்பு dynbuf *store,
                        மாறிலி uint8_t *beg, மாறிலி uint8_t *end)
{
  /* Convert an ASN.1 bit string to a printable string. */

  எனில்(beg < end) {
    uint8_t unused_bits = *beg++;
    எனில்(unused_bits > 7)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }
  இல்லையெனில்
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  திரும்பு octet2str(store, beg, end);
}

/*
 * Convert an ASN.1 integer value into its string representation.
 *
 * Returns error.
 */
நிலையான CURLcode int2str(கட்டமைப்பு dynbuf *store,
                        மாறிலி uint8_t *beg, மாறிலி uint8_t *end)
{
  uint32_t uval = 0;
  int32_t sval = 0;
  size_t n = end - beg;
  size_t i;
  மாறிலி uint8_t *p = beg;

  எனில்(!n)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  எனில்(n > 4)
    திரும்பு octet2str(store, beg, end);

  எனில்(p[0] & 0x80)
    uval = ~0U;

  க்கு(i = 0; i < n; i++)
    uval = (uval << 8) | p[i];

  sval = (int32_t)uval;

  /* small values are easier to read as decimals */
  எனில்(sval > -10000 && sval < 10000)
    திரும்பு curlx_dyn_addf(store, "%d", sval);

  /* represent larger values as hex */
  திரும்பு curlx_dyn_addf(store, "0x%x", uval);
}

/*
 * Convert from an ASN.1 typed string to UTF8.
 *
 * The result is stored in a dynbuf that is inited by the user of this
 * function.
 *
 * Returns error.
 */
நிலையான CURLcode utf8asn1str(கட்டமைப்பு dynbuf *to, முழு type,
                            மாறிலி uint8_t *from, மாறிலி uint8_t *end)
{
  size_t inlength = end - from;
  முழு size = 1;
  CURLcode result = CURLE_OK;

  தேர்வு(type) {
  நிலை CURL_ASN1_BMP_STRING:
    size = 2;
    நிறுத்து;
  நிலை CURL_ASN1_UNIVERSAL_STRING:
    size = 4;
    நிறுத்து;
  நிலை CURL_ASN1_NUMERIC_STRING:
  நிலை CURL_ASN1_PRINTABLE_STRING:
  நிலை CURL_ASN1_TELETEX_STRING:
  நிலை CURL_ASN1_IA5_STRING:
  நிலை CURL_ASN1_VISIBLE_STRING:
  நிலை CURL_ASN1_UTF8_STRING:
    நிறுத்து;
  இயல்பு:
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;  /* Conversion not supported. */
  }

  எனில்(inlength % size)
    /* Length inconsistent with character size. */
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  எனில்(type == CURL_ASN1_UTF8_STRING) {
    /* copy. */
    எனில்(inlength)
      result = curlx_dyn_addn(to, from, inlength);
  }
  இல்லையெனில் {
    போது(!result && (from < end)) {
      எழுத்து buf[4]; /* decode buffer */
      size_t charsize = 1;
      unsigned முழு wc = 0;

      தேர்வு(size) {
      நிலை 4:
        wc = (wc << 8) | *from++;
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      நிலை 2:
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      இயல்பு: /* case 1: */
        wc = (wc << 8) | *from++;
      }
      எனில்(wc >= 0x00000080) {
        எனில்(wc >= 0x00000800) {
          எனில்(wc >= 0x00010000) {
            எனில்(wc >= 0x00200000) {
              /* Invalid char. size for target encoding. */
              திரும்பு CURLE_WEIRD_SERVER_REPLY;
            }
            buf[3] = (எழுத்து)(0x80 | (wc & 0x3F));
            wc = (wc >> 6) | 0x00010000;
            charsize++;
          }
          buf[2] = (எழுத்து)(0x80 | (wc & 0x3F));
          wc = (wc >> 6) | 0x00000800;
          charsize++;
        }
        buf[1] = (எழுத்து)(0x80 | (wc & 0x3F));
        wc = (wc >> 6) | 0x000000C0;
        charsize++;
      }
      buf[0] = (எழுத்து)wc;
      result = curlx_dyn_addn(to, buf, charsize);
    }
  }
  திரும்பு result;
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
UNITTEST CURLcode encodeOID(கட்டமைப்பு dynbuf *store,
                            மாறிலி uint8_t *beg, மாறிலி uint8_t *end);
UNITTEST CURLcode encodeOID(கட்டமைப்பு dynbuf *store,
                            மாறிலி uint8_t *beg, மாறிலி uint8_t *end)
{
  uint32_t x;
  uint32_t y;
  CURLcode result = CURLE_OK;

  எனில்(end <= beg)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  /* Process the first two numbers. The initial digit cannot be larger than
     2 */
  y = *beg++;
  எனில்(y <= 80)
    x = y / 40;
  இல்லையெனில்
    x = 2;
  result = curlx_dyn_addf(store, "%u", x);
  எனில்(result)
    திரும்பு result;

  எனில்(y & 0x80) {
    uint32_t t = (y & 0x7f); /* the second number */
    எனில்(!t)
      /* reject leading 0x80 padding */
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    செய் {
      எனில்((t & 0xFE000000) || (beg >= end))
        திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      t = (t << 7) | (y & 0x7F);
    } போது(y & 0x80);
    y = t;
  }

  result = curlx_dyn_addf(store, ".%u", y - (x * 40));
  எனில்(result)
    திரும்பு result;

  /* Process the trailing numbers. */
  போது((beg < end) && !result) {
    x = 0;
    y = 0;
    செய் {
      /* 0x80 as the first value is invalid since it only adds padding */
      எனில்((y & 0x80) && !x)
        திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
      இல்லையெனில் எனில்((x & 0xFE000000) || (beg == end))
        திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      x = (x << 7) | (y & 0x7F);
    } போது(y & 0x80);
    result = curlx_dyn_addf(store, ".%u", x);
  }
  திரும்பு result;
}

/*
 * Convert an ASN.1 OID into its dotted or symbolic string representation.
 *
 * Return error code.
 */

நிலையான CURLcode OID2str(கட்டமைப்பு dynbuf *store,
                        மாறிலி uint8_t *beg, மாறிலி uint8_t *end)
{
  CURLcode result = CURLE_OK;
  எனில்(beg < end) {
    கட்டமைப்பு dynbuf buf;
    curlx_dyn_init(&buf, CURL_X509_STR_MAX);
    result = encodeOID(&buf, beg, end);

    எனில்(!result) {
      மாறிலி கட்டமைப்பு Curl_OID *op = தேடல்_OID(curlx_dyn_ptr(&buf));
      எனில்(op)
        result = curlx_dyn_add(store, op->textoid);
      இல்லையெனில்
        result = curlx_dyn_add(store, curlx_dyn_ptr(&buf));
    }
    curlx_dyn_free(&buf);
  }
  திரும்பு result;
}

/*
 * @unittest 1656
 */
UNITTEST CURLcode GTime2str(கட்டமைப்பு dynbuf *store,
                            மாறிலி எழுத்து *beg, மாறிலி எழுத்து *end);
UNITTEST CURLcode GTime2str(கட்டமைப்பு dynbuf *store,
                            மாறிலி எழுத்து *beg, மாறிலி எழுத்து *end)
{
  மாறிலி எழுத்து *tzp;
  மாறிலி எழுத்து *fracp;
  எழுத்து sec1, sec2;
  size_t fracl;
  size_t tzl;
  மாறிலி எழுத்து *sep = "";

  /* Convert an ASN.1 Generalized time to a printable string.
     Return the dynamically allocated string, or NULL if an error occurs. */

  க்கு(fracp = beg; fracp < end && ISDIGIT(*fracp); fracp++)
    ;

  /* Get seconds digits. */
  sec1 = '0';
  தேர்வு(fracp - beg - 12) {
  நிலை 0:
    sec2 = '0';
    நிறுத்து;
  நிலை 2:
    sec1 = fracp[-2];
    FALLTHROUGH();
  நிலை 1:
    sec2 = fracp[-1];
    நிறுத்து;
  இயல்பு:
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* timezone follows optional fractional seconds. */
  tzp = fracp;
  fracl = 0; /* no fractional seconds detected so far */
  எனில்(fracp < end && (*fracp == '.' || *fracp == ',')) {
    /* Have fractional seconds, e.g. "[.,]\d+". How many? */
    fracp++; /* should be a digit char or BAD ARGUMENT */
    tzp = fracp;
    போது(tzp < end && ISDIGIT(*tzp))
      tzp++;
    எனில்(tzp == fracp) /* never looped, no digit after [.,] */
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    fracl = tzp - fracp; /* number of fractional sec digits */
    DEBUGASSERT(fracl > 0);
    /* Strip trailing zeroes in fractional seconds.
     * May reduce fracl to 0 if only '0's are present. */
    போது(fracl && fracp[fracl - 1] == '0')
      fracl--;
  }

  /* Process timezone. */
  எனில்(tzp >= end) {
    tzp = "";
    tzl = 0;
  }
  இல்லையெனில் எனில்(*tzp == 'Z') {
    sep = " ";
    tzp = "GMT";
    tzl = 3;
  }
  இல்லையெனில் எனில்((*tzp == '+') || (*tzp == '-')) {
    sep = " UTC";
    tzl = end - tzp;
  }
  இல்லையெனில் {
    sep = " ";
    tzl = end - tzp;
  }

  திரும்பு curlx_dyn_addf(store,
                        "%.4s-%.2s-%.2s %.2s:%.2s:%c%c%s%.*s%s%.*s",
                        beg, beg + 4, beg + 6,
                        beg + 8, beg + 10, sec1, sec2,
                        fracl ? "." : "", (முழு)fracl, fracp,
                        sep, (முழு)tzl, tzp);
}

/*
 * Convert an ASN.1 UTC time to a printable string.
 *
 * Return error code.
 */
நிலையான CURLcode UTime2str(கட்டமைப்பு dynbuf *store,
                          மாறிலி எழுத்து *beg, மாறிலி எழுத்து *end)
{
  மாறிலி எழுத்து *tzp;
  size_t tzl;
  மாறிலி எழுத்து *sec;

  க்கு(tzp = beg; tzp < end && *tzp >= '0' && *tzp <= '9'; tzp++)
    ;
  /* Get the seconds. */
  sec = beg + 10;
  தேர்வு(tzp - sec) {
  நிலை 0:
    sec = "00";
    FALLTHROUGH();
  நிலை 2:
    நிறுத்து;
  இயல்பு:
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Process timezone. */
  எனில்(tzp >= end)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  எனில்(*tzp == 'Z') {
    tzp = "GMT";
    end = tzp + 3;
  }

  tzl = end - tzp;
  திரும்பு curlx_dyn_addf(store, "%d%.2s-%.2s-%.2s %.2s:%.2s:%.2s %.*s",
                        20 - (*beg >= '5'), beg, beg + 2, beg + 4,
                        beg + 6, beg + 8, sec,
                        (முழு)tzl, tzp);
}

/*
 * Convert an ASN.1 element to a printable string.
 *
 * Return error
 *
 * @unittest 1667
 */
UNITTEST CURLcode ASN1tostr(கட்டமைப்பு dynbuf *store,
                            கட்டமைப்பு Curl_asn1Element *elem);
UNITTEST CURLcode ASN1tostr(கட்டமைப்பு dynbuf *store,
                            கட்டமைப்பு Curl_asn1Element *elem)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  முழு type = elem->tag;
  எனில்(elem->constructed)
    திரும்பு result; /* No conversion of structured elements. */

  தேர்வு(type) {
  நிலை CURL_ASN1_BOOLEAN:
    result = bool2str(store, elem->beg, elem->end);
    நிறுத்து;
  நிலை CURL_ASN1_INTEGER:
  நிலை CURL_ASN1_ENUMERATED:
    result = int2str(store, elem->beg, elem->end);
    நிறுத்து;
  நிலை CURL_ASN1_BIT_STRING:
    result = bit2str(store, elem->beg, elem->end);
    நிறுத்து;
  நிலை CURL_ASN1_OCTET_STRING:
    result = octet2str(store, elem->beg, elem->end);
    நிறுத்து;
  நிலை CURL_ASN1_NULL:
    result = curlx_dyn_addn(store, "", 1);
    நிறுத்து;
  நிலை CURL_ASN1_OBJECT_IDENTIFIER:
    result = OID2str(store, elem->beg, elem->end);
    நிறுத்து;
  நிலை CURL_ASN1_UTC_TIME:
    result = UTime2str(store, (மாறிலி எழுத்து *)elem->beg,
                       (மாறிலி எழுத்து *)elem->end);
    நிறுத்து;
  நிலை CURL_ASN1_GENERALIZED_TIME:
    result = GTime2str(store, (மாறிலி எழுத்து *)elem->beg,
                       (மாறிலி எழுத்து *)elem->end);
    நிறுத்து;
  நிலை CURL_ASN1_UTF8_STRING:
  நிலை CURL_ASN1_NUMERIC_STRING:
  நிலை CURL_ASN1_PRINTABLE_STRING:
  நிலை CURL_ASN1_TELETEX_STRING:
  நிலை CURL_ASN1_IA5_STRING:
  நிலை CURL_ASN1_VISIBLE_STRING:
  நிலை CURL_ASN1_UNIVERSAL_STRING:
  நிலை CURL_ASN1_BMP_STRING:
    result = utf8asn1str(store, type, elem->beg, elem->end);
    நிறுத்து;
  }

  திரும்பு result;
}

/*
 * ASCII encode distinguished name at `dn' into the store dynbuf.
 *
 * Returns error.
 */
நிலையான CURLcode encodeDN(கட்டமைப்பு dynbuf *store, கட்டமைப்பு Curl_asn1Element *dn)
{
  கட்டமைப்பு Curl_asn1Element rdn;
  கட்டமைப்பு Curl_asn1Element atv;
  கட்டமைப்பு Curl_asn1Element oid;
  கட்டமைப்பு Curl_asn1Element மதிப்பு;
  மாறிலி uint8_t *p1;
  மாறிலி uint8_t *p2;
  மாறிலி uint8_t *p3;
  மாறிலி uint8_t *str;
  CURLcode result = CURLE_OK;
  bool added = FALSE;
  கட்டமைப்பு dynbuf temp;
  curlx_dyn_init(&temp, CURL_X509_STR_MAX);

  க்கு(p1 = dn->beg; p1 < dn->end;) {
    p1 = getASN1Element(&rdn, p1, dn->end);
    எனில்(!p1) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      செல் error;
    }
    க்கு(p2 = rdn.beg; p2 < rdn.end;) {
      p2 = getASN1Element(&atv, p2, rdn.end);
      எனில்(!p2) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        செல் error;
      }
      p3 = getASN1Element(&oid, atv.beg, atv.end);
      எனில்(!p3) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        செல் error;
      }
      எனில்(!getASN1Element(&மதிப்பு, p3, atv.end)) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        செல் error;
      }
      curlx_dyn_reset(&temp);
      result = ASN1tostr(&temp, &oid);
      எனில்(result)
        செல் error;

      str = curlx_dyn_uptr(&temp);

      எனில்(!str) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        செல் error;
      }

      /* Encode delimiter.
         If attribute has a short uppercase name, delimiter is ", ". */
      க்கு(p3 = str; ISUPPER(*p3); p3++)
        ;
      எனில்(added) {
        எனில்(p3 - str > 2)
          result = curlx_dyn_addn(store, "/", 1);
        இல்லையெனில்
          result = curlx_dyn_addn(store, ", ", 2);
        எனில்(result)
          செல் error;
      }

      /* Encode attribute name. */
      result = curlx_dyn_add(store, (மாறிலி எழுத்து *)str);
      எனில்(result)
        செல் error;

      /* Generate equal sign. */
      result = curlx_dyn_addn(store, "=", 1);
      எனில்(result)
        செல் error;

      /* Generate value. */
      result = ASN1tostr(store, &மதிப்பு);
      எனில்(result)
        செல் error;
      curlx_dyn_reset(&temp);
      added = TRUE; /* use separator for next */
    }
  }
error:
  curlx_dyn_free(&temp);

  திரும்பு result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#ifdef WANT_PARSEX509
/*
 * ASN.1 parse an X509 certificate into structure subfields.
 * Syntax is assumed to have already been checked by the SSL backend.
 * See RFC 5280.
 */
முழு Curl_parseX509(கட்டமைப்பு Curl_X509certificate *cert,
                   மாறிலி uint8_t *beg, மாறிலி uint8_t *end)
{
  கட்டமைப்பு Curl_asn1Element elem;
  கட்டமைப்பு Curl_asn1Element tbsCertificate;
  மாறிலி uint8_t *ccp;
  நிலையான மாறிலி uint8_t defaultVersion = 0;  /* v1. */

  memset(cert, 0, அளவு(*cert));
  cert->certificate.header = வெற்று;
  cert->certificate.beg = beg;
  cert->certificate.end = end;

  /* Get the sequence content. */
  எனில்(!getASN1Element(&elem, beg, end))
    திரும்பு -1;  /* Invalid bounds/size. */
  beg = elem.beg;
  end = elem.end;

  /* Get tbsCertificate. */
  beg = getASN1Element(&tbsCertificate, beg, end);
  எனில்(!beg)
    திரும்பு -1;
  /* Skip the signatureAlgorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  எனில்(!beg)
    திரும்பு -1;
  /* Get the signatureValue. */
  எனில்(!getASN1Element(&cert->signature, beg, end))
    திரும்பு -1;

  /* Parse TBSCertificate. */
  beg = tbsCertificate.beg;
  end = tbsCertificate.end;
  /* Get optional version, get serialNumber. */
  cert->version.header = வெற்று;
  cert->version.beg = &defaultVersion;
  cert->version.end = &defaultVersion + அளவு(defaultVersion);
  beg = getASN1Element(&elem, beg, end);
  எனில்(!beg)
    திரும்பு -1;
  எனில்(elem.tag == 0) {
    எனில்(!getASN1Element(&cert->version, elem.beg, elem.end))
      திரும்பு -1;
    beg = getASN1Element(&elem, beg, end);
    எனில்(!beg)
      திரும்பு -1;
  }
  cert->serial_எண் = elem;
  /* Get signature algorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  /* Get issuer. */
  beg = getASN1Element(&cert->issuer, beg, end);
  எனில்(!beg)
    திரும்பு -1;
  /* Get notBefore and notAfter. */
  beg = getASN1Element(&elem, beg, end);
  எனில்(!beg)
    திரும்பு -1;
  ccp = getASN1Element(&cert->notBefore, elem.beg, elem.end);
  எனில்(!ccp)
    திரும்பு -1;
  எனில்(!getASN1Element(&cert->notAfter, ccp, elem.end))
    திரும்பு -1;
  /* These two fields MUST consume all of the validity elem */
  எனில்(elem.end != cert->notAfter.end)
    திரும்பு -1;
  /* Get subject. */
  beg = getASN1Element(&cert->subject, beg, end);
  எனில்(!beg)
    திரும்பு -1;
  /* Get subjectPublicKeyAlgorithm and subjectPublicKey. */
  beg = getASN1Element(&cert->subject_Public_சாவி_Info, beg, end);
  எனில்(!beg)
    திரும்பு -1;
  ccp = getASN1Element(&cert->subject_Public_சாவி_Algorithm,
                       cert->subject_Public_சாவி_Info.beg,
                       cert->subject_Public_சாவி_Info.end);
  எனில்(!ccp)
    திரும்பு -1;
  எனில்(!getASN1Element(&cert->subject_Public_சாவி, ccp,
                     cert->subject_Public_சாவி_Info.end))
    திரும்பு -1;
  /* Get optional issuerUniqueID, subjectUniqueID and extensions. */
  cert->issuerUniqueID.tag = cert->subjectUniqueID.tag = 0;
  cert->extensions.tag = elem.tag = 0;
  cert->issuerUniqueID.header = cert->subjectUniqueID.header = வெற்று;
  cert->issuerUniqueID.beg = cert->issuerUniqueID.end = beg;
  cert->subjectUniqueID.beg = cert->subjectUniqueID.end = beg;
  cert->extensions.header = வெற்று;
  cert->extensions.beg = cert->extensions.end = beg;
  எனில்(beg < end) {
    beg = getASN1Element(&elem, beg, end);
    எனில்(!beg)
      திரும்பு -1;
  }
  எனில்(elem.tag == 1) {
    cert->issuerUniqueID = elem;
    எனில்(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      எனில்(!beg)
        திரும்பு -1;
    }
  }
  எனில்(elem.tag == 2) {
    cert->subjectUniqueID = elem;
    எனில்(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      எனில்(!beg)
        திரும்பு -1;
    }
  }
  எனில்(elem.tag == 3)
    எனில்(!getASN1Element(&cert->extensions, elem.beg, elem.end))
      திரும்பு -1;
  திரும்பு 0;
}

#endif /* WANT_PARSEX509 */

#ifdef WANT_EXTRACT_CERTINFO

நிலையான CURLcode dumpAlgo(கட்டமைப்பு dynbuf *store,
                         கட்டமைப்பு Curl_asn1Element *param,
                         மாறிலி uint8_t *beg, மாறிலி uint8_t *end)
{
  கட்டமைப்பு Curl_asn1Element oid;

  /* Get algorithm parameters and return algorithm name. */

  beg = getASN1Element(&oid, beg, end);
  எனில்(!beg)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  param->header = வெற்று;
  param->tag = 0;
  param->beg = param->end = end;
  எனில்(beg < end) {
    மாறிலி uint8_t *p = getASN1Element(param, beg, end);
    எனில்(!p)
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }
  திரும்பு OID2str(store, oid.beg, oid.end);
}

/*
 * This is a convenience function for push_certinfo_len that takes a zero
 * terminated value.
 */
நிலையான CURLcode ssl_தள்ளு_certinfo(கட்டமைப்பு Curl_easy *data,
                                  முழு certnum,
                                  மாறிலி எழுத்து *label,
                                  மாறிலி எழுத்து *மதிப்பு)
{
  size_t valuelen = strlen(மதிப்பு);

  திரும்பு Curl_ssl_தள்ளு_certinfo_len(data, certnum, label, மதிப்பு, valuelen);
}

/*
 * This is a convenience function for push_certinfo_len that takes a
 * dynbuf value.
 *
 * It also does the verbose output if !certnum.
 */
நிலையான CURLcode ssl_தள்ளு_certinfo_dyn(கட்டமைப்பு Curl_easy *data,
                                      முழு certnum,
                                      மாறிலி எழுத்து *label,
                                      கட்டமைப்பு dynbuf *ptr)
{
  size_t valuelen = curlx_dyn_len(ptr);
  மாறிலி எழுத்து *மதிப்பு = curlx_dyn_ptr(ptr);

  CURLcode result = Curl_ssl_தள்ளு_certinfo_len(data, certnum, label,
                                               மதிப்பு, valuelen);

  எனில்(!certnum && !result)
    infof(data, "   %s: %s", label, மதிப்பு);

  திரும்பு result;
}

நிலையான CURLcode do_pubkey_field(கட்டமைப்பு Curl_easy *data, முழு certnum,
                                மாறிலி எழுத்து *label,
                                கட்டமைப்பு Curl_asn1Element *elem)
{
  CURLcode result;
  கட்டமைப்பு dynbuf out;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);

  /* Generate a certificate information record for the public key. */

  result = ASN1tostr(&out, elem);
  எனில்(!result && data->set.ssl.certinfo)
    result = ssl_தள்ளு_certinfo_dyn(data, certnum, label, &out);
  curlx_dyn_free(&out);
  திரும்பு result;
}

/* return 0 on success, 1 on error */
நிலையான முழு do_pubkey(கட்டமைப்பு Curl_easy *data, முழு certnum, மாறிலி எழுத்து *algo,
                     கட்டமைப்பு Curl_asn1Element *param,
                     கட்டமைப்பு Curl_asn1Element *pubkey)
{
  கட்டமைப்பு Curl_asn1Element elem;
  கட்டமைப்பு Curl_asn1Element pk;
  மாறிலி uint8_t *p;

  /* Generate all information records for the public key. */

  எனில்(curl_strequal(algo, "ecPublicKey")) {
    /*
     * ECC public key is all the data, a value of type BIT STRING mapped to
     * OCTET STRING and should not be parsed as an ASN.1 value.
     */
    மாறிலி size_t dlen = pubkey->end - pubkey->beg;
    size_t len;
    எனில்(dlen < 2)
      /* too small */
      திரும்பு 1;
    len = (dlen - 2) * 4;
    எனில்(!certnum)
      infof(data, "   ECC Public Key (%zu bits)", len);
    எனில்(data->set.ssl.certinfo) {
      எழுத்து q[(அளவு(len) * 8 / 3) + 1];
      (வெறுமை)curl_msnprintf(q, அளவு(q), "%zu", len);
      எனில்(ssl_தள்ளு_certinfo(data, certnum, "ECC Public Key", q))
        திரும்பு 1;
    }
    திரும்பு do_pubkey_field(data, certnum, "ecPublicKey", pubkey) == CURLE_OK
      ? 0 : 1;
  }

  /* Get the public key (single element). */
  எனில்(!getASN1Element(&pk, pubkey->beg + 1, pubkey->end))
    திரும்பு 1;

  எனில்(curl_strequal(algo, "rsaEncryption")) {
    மாறிலி uint8_t *q;
    size_t len;

    p = getASN1Element(&elem, pk.beg, pk.end);
    எனில்(!p)
      திரும்பு 1;

    /* Compute key length. */
    க்கு(q = elem.beg; q < elem.end && !*q; q++)
      ;
    len = ((elem.end - q) * 8);
    எனில்(len) {
      unsigned முழு i;
      க்கு(i = *q; !(i & 0x80); i <<= 1)
        len--;
    }
    எனில்(len > 32)
      elem.beg = q;     /* Strip leading zero bytes. */
    எனில்(!certnum)
      infof(data, "   RSA Public Key (%zu bits)", len);
    எனில்(data->set.ssl.certinfo) {
      எழுத்து r[(அளவு(len) * 8 / 3) + 1];
      curl_msnprintf(r, அளவு(r), "%zu", len);
      எனில்(ssl_தள்ளு_certinfo(data, certnum, "RSA Public Key", r))
        திரும்பு 1;
    }
    /* Generate coefficients. */
    எனில்(do_pubkey_field(data, certnum, "rsa(n)", &elem))
      திரும்பு 1;
    எனில்(!getASN1Element(&elem, p, pk.end))
      திரும்பு 1;
    எனில்(do_pubkey_field(data, certnum, "rsa(e)", &elem))
      திரும்பு 1;
  }
  இல்லையெனில் எனில்(curl_strequal(algo, "dsa")) {
    p = getASN1Element(&elem, param->beg, param->end);
    எனில்(p) {
      எனில்(do_pubkey_field(data, certnum, "dsa(p)", &elem))
        திரும்பு 1;
      p = getASN1Element(&elem, p, param->end);
      எனில்(p) {
        எனில்(do_pubkey_field(data, certnum, "dsa(q)", &elem))
          திரும்பு 1;
        எனில்(getASN1Element(&elem, p, param->end)) {
          எனில்(do_pubkey_field(data, certnum, "dsa(g)", &elem))
            திரும்பு 1;
          எனில்(do_pubkey_field(data, certnum, "dsa(pub_key)", &pk))
            திரும்பு 1;
        }
      }
    }
  }
  இல்லையெனில் எனில்(curl_strequal(algo, "dhpublicnumber")) {
    p = getASN1Element(&elem, param->beg, param->end);
    எனில்(p) {
      எனில்(do_pubkey_field(data, certnum, "dh(p)", &elem))
        திரும்பு 1;
      எனில்(getASN1Element(&elem, p, param->end)) {
        எனில்(do_pubkey_field(data, certnum, "dh(g)", &elem))
          திரும்பு 1;
        எனில்(do_pubkey_field(data, certnum, "dh(pub_key)", &pk))
          திரும்பு 1;
      }
    }
  }
  திரும்பு 0;
}

/*
 * Convert an ASN.1 distinguished name into a printable string.
 * Return error.
 */
நிலையான CURLcode DNtostr(கட்டமைப்பு dynbuf *store, கட்டமைப்பு Curl_asn1Element *dn)
{
  திரும்பு encodeDN(store, dn);
}

CURLcode Curl_extract_certinfo(கட்டமைப்பு Curl_easy *data,
                               முழு certnum,
                               மாறிலி uint8_t *beg,
                               மாறிலி uint8_t *end)
{
  கட்டமைப்பு Curl_X509certificate cert;
  கட்டமைப்பு Curl_asn1Element param;
  எழுத்து *certptr;
  size_t clen;
  கட்டமைப்பு dynbuf out;
  CURLcode result = CURLE_OK;
  unsigned முழு version;
  மாறிலி uint8_t *ptr;
  முழு rc;

  எனில்(!data->set.ssl.certinfo)
    எனில்(certnum)
      திரும்பு CURLE_OK;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);
  /* Prepare the certificate information for curl_easy_getinfo(). */

  /* Extract the certificate ASN.1 elements. */
  எனில்(Curl_parseX509(&cert, beg, end))
    திரும்பு CURLE_PEER_FAILED_VERIFICATION;

  /* Subject. */
  result = DNtostr(&out, &cert.subject);
  எனில்(result)
    செல் done;
  எனில்(data->set.ssl.certinfo) {
    result = ssl_தள்ளு_certinfo_dyn(data, certnum, "Subject", &out);
    எனில்(result)
      செல் done;
  }
  curlx_dyn_reset(&out);

  /* Issuer. */
  result = DNtostr(&out, &cert.issuer);
  எனில்(result)
    செல் done;
  எனில்(data->set.ssl.certinfo) {
    result = ssl_தள்ளு_certinfo_dyn(data, certnum, "Issuer", &out);
    எனில்(result)
      செல் done;
  }
  curlx_dyn_reset(&out);

  /* Version (always fits in less than 32 bits). */
  version = 0;
  க்கு(ptr = cert.version.beg; ptr < cert.version.end; ptr++)
    version = (version << 8) | *ptr;
  எனில்(data->set.ssl.certinfo) {
    result = curlx_dyn_addf(&out, "%x", version);
    எனில்(result)
      செல் done;
    result = ssl_தள்ளு_certinfo_dyn(data, certnum, "Version", &out);
    எனில்(result)
      செல் done;
    curlx_dyn_reset(&out);
  }

  /* Serial number. */
  result = ASN1tostr(&out, &cert.serial_எண்);
  எனில்(result)
    செல் done;
  எனில்(data->set.ssl.certinfo) {
    result = ssl_தள்ளு_certinfo_dyn(data, certnum, "Serial Number", &out);
    எனில்(result)
      செல் done;
  }
  curlx_dyn_reset(&out);

  /* Signature algorithm .*/
  result = dumpAlgo(&out, &param, cert.signatureAlgorithm.beg,
                    cert.signatureAlgorithm.end);
  எனில்(result)
    செல் done;
  எனில்(data->set.ssl.certinfo) {
    result = ssl_தள்ளு_certinfo_dyn(data, certnum, "Signature Algorithm", &out);
    எனில்(result)
      செல் done;
  }
  curlx_dyn_reset(&out);

  /* Start Date. */
  result = ASN1tostr(&out, &cert.notBefore);
  எனில்(result)
    செல் done;
  எனில்(data->set.ssl.certinfo) {
    result = ssl_தள்ளு_certinfo_dyn(data, certnum, "Start Date", &out);
    எனில்(result)
      செல் done;
  }
  curlx_dyn_reset(&out);

  /* Expire Date. */
  result = ASN1tostr(&out, &cert.notAfter);
  எனில்(result)
    செல் done;
  எனில்(data->set.ssl.certinfo) {
    result = ssl_தள்ளு_certinfo_dyn(data, certnum, "Expire Date", &out);
    எனில்(result)
      செல் done;
  }
  curlx_dyn_reset(&out);

  /* Public Key Algorithm. */
  result = dumpAlgo(&out, &param, cert.subject_Public_சாவி_Algorithm.beg,
                    cert.subject_Public_சாவி_Algorithm.end);
  எனில்(result)
    செல் done;
  எனில்(data->set.ssl.certinfo) {
    result = ssl_தள்ளு_certinfo_dyn(data, certnum, "Public Key Algorithm",
                                   &out);
    எனில்(result)
      செல் done;
  }

  rc = do_pubkey(data, certnum, curlx_dyn_ptr(&out),
                 &param, &cert.subject_Public_சாவி);
  எனில்(rc) {
    result = CURLE_OUT_OF_MEMORY; /* the most likely error */
    செல் done;
  }
  curlx_dyn_reset(&out);

  /* Signature. */
  result = ASN1tostr(&out, &cert.signature);
  எனில்(result)
    செல் done;
  எனில்(data->set.ssl.certinfo) {
    result = ssl_தள்ளு_certinfo_dyn(data, certnum, "Signature", &out);
    எனில்(result)
      செல் done;
  }
  curlx_dyn_reset(&out);

  /* Generate PEM certificate. */
  result = curlx_base64_encode((மாறிலி uint8_t *)cert.certificate.beg,
                               cert.certificate.end - cert.certificate.beg,
                               &certptr, &clen);
  எனில்(result)
    செல் done;

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
  எனில்(!result) {
    size_t j = 0;

    போது(!result && (j < clen)) {
      size_t chunksize = (clen - j) > 64 ? 64 : (clen - j);
      result = curlx_dyn_addn(&out, &certptr[j], chunksize);
      எனில்(!result)
        result = curlx_dyn_addn(&out, "\n", 1);
      j += chunksize;
    }
    எனில்(!result)
      result = curlx_dyn_add(&out, "-----END CERTIFICATE-----\n");
  }
  curlx_free(certptr);
  எனில்(!result)
    எனில்(data->set.ssl.certinfo)
      result = ssl_தள்ளு_certinfo_dyn(data, certnum, "Cert", &out);

done:
  எனில்(result)
    failf(data, "Failed extracting certificate chain");
  curlx_dyn_free(&out);
  திரும்பு result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#endif /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
