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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#എങ്കിൽ defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_RUSTLS)
#define WANT_PARSEX509 /* uses Curl_parseX509() */
#endif

#എങ്കിൽ defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_RUSTLS)
#define WANT_EXTRACT_CERTINFO /* uses Curl_extract_certinfo() */
#endif

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "curlx/base64.h"
#അന്തര്ഭാവ "vtls/x509asn1.h"
#അന്തര്ഭാവ "curlx/dynbuf.h"

/*
 * Constants.
 */

/* Largest supported ASN.1 structure. */
#define CURL_ASN1_MAX                   ((size_t)0x40000)      /* 256K */

#ifdef WANT_EXTRACT_CERTINFO

/* ASN.1 OID table entry. */
ഘടന Curl_OID {
  സ്ഥിരം അക്ഷരം *numoid;  /* Dotted-numeric OID. */
  സ്ഥിരം അക്ഷരം *textoid; /* OID name. */
};

/* ASN.1 OIDs. */
സ്ഥിതം സ്ഥിരം ഘടന Curl_OID OIDtable[] = {
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
  { ശൂന്യ,                       ശൂന്യ }
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

സ്ഥിതം സ്ഥിരം uint8_t *getASN1Element_(ഘടന Curl_asn1Element *elem,
                                      സ്ഥിരം uint8_t *beg, സ്ഥിരം uint8_t *end,
                                      size_t lvl)
{
  uint8_t b;
  size_t len;
  ഘടന Curl_asn1Element lelem;

  /* Get a single ASN.1 element into `elem', parse ASN.1 string at `beg'
     ending at `end'.
     Returns a pointer in source string after the parsed element, or NULL
     if an error occurs. */
  എങ്കിൽ(!beg || !end || beg >= end || !*beg ||
     ((size_t)(end - beg) > CURL_ASN1_MAX) ||
     lvl++ >= CURL_ASN1_MAX_RECURSIONS)
    തിരികെ ശൂന്യ;

  /* Process header byte. */
  elem->header = beg;
  b = *beg++;
  elem->constructed = (b & 0x20) != 0;
  elem->eclass = (b >> 6) & 3;
  b &= 0x1F;
  എങ്കിൽ(b == 0x1F)
    തിരികെ ശൂന്യ; /* Long tag values not supported here. */
  elem->tag = b;

  /* Process length. */
  എങ്കിൽ(beg >= end)
    തിരികെ ശൂന്യ;
  b = *beg++;
  എങ്കിൽ(!(b & 0x80))
    len = b;
  അല്ലെങ്കിൽ {
    b &= 0x7F;
    എങ്കിൽ(!b) {
      /* Unspecified length. Since we have all the data, we can determine the
         effective length by skipping element until an EOC element is found. */
      എങ്കിൽ(!elem->constructed)
        തിരികെ ശൂന്യ;
      elem->beg = beg;
      ആകുമ്പോൾ(beg < end && *beg) {
        beg = getASN1Element_(&lelem, beg, end, lvl);
        എങ്കിൽ(!beg)
          തിരികെ ശൂന്യ;
      }
      എങ്കിൽ(end - beg < 2 || beg[1])       /* EOC must have a zero length. */
        തിരികെ ശൂന്യ;
      elem->end = beg;                  /* Do not include EOC as content. */
      തിരികെ beg + 2;                   /* After EOC element. */
    }
    അല്ലെങ്കിൽ എങ്കിൽ(b > (size_t)(end - beg))
      തിരികെ ശൂന്യ; /* Does not fit in source. */
    അല്ലെങ്കിൽ {
      /* Get long length. */
      len = 0;
      ചെയ്യുക {
        എങ്കിൽ(len & 0xFF000000L)
          തിരികെ ശൂന്യ;  /* Lengths > 32 bits are not supported. */
        len = (len << 8) | *beg++;
      } ആകുമ്പോൾ(--b);
    }
  }
  എങ്കിൽ(len > (size_t)(end - beg))
    തിരികെ ശൂന്യ;  /* Element data does not fit in source. */
  elem->beg = beg;
  elem->end = beg + len;
  തിരികെ elem->end;
}

/*
 * @unittest 1657
 */
UNITTEST സ്ഥിരം uint8_t *getASN1Element(ഘടന Curl_asn1Element *elem,
                                       സ്ഥിരം uint8_t *beg, സ്ഥിരം uint8_t *end);
UNITTEST സ്ഥിരം uint8_t *getASN1Element(ഘടന Curl_asn1Element *elem,
                                       സ്ഥിരം uint8_t *beg, സ്ഥിരം uint8_t *end)
{
  തിരികെ getASN1Element_(elem, beg, end, 0);
}

#ifdef WANT_EXTRACT_CERTINFO

/*
 * Search the null-terminated OID or OID identifier in local table.
 * Return the table entry pointer or NULL if not found.
 */
സ്ഥിതം സ്ഥിരം ഘടന Curl_OID *തിരയൽ_OID(സ്ഥിരം അക്ഷരം *oid)
{
  സ്ഥിരം ഘടന Curl_OID *op;
  വേണ്ടി(op = OIDtable; op->numoid; op++)
    എങ്കിൽ(!strcmp(op->numoid, oid) || curl_strequal(op->textoid, oid))
      തിരികെ op;

  തിരികെ ശൂന്യ;
}

/*
 * Convert an ASN.1 Boolean value into its string representation.
 *
 * Return error code.
 */

സ്ഥിതം CURLcode bool2str(ഘടന dynbuf *store,
                         സ്ഥിരം uint8_t *beg, സ്ഥിരം uint8_t *end)
{
  uint8_t val;
  എങ്കിൽ(end - beg != 1)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  val = *beg;
  എങ്കിൽ((val != 0xff) && (val != 0x00))
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  തിരികെ curlx_dyn_add(store, val ? "TRUE" : "FALSE");
}

/*
 * Convert an ASN.1 octet string to a printable string.
 *
 * Return error code.
 */
സ്ഥിതം CURLcode octet2str(ഘടന dynbuf *store,
                          സ്ഥിരം uint8_t *beg, സ്ഥിരം uint8_t *end)
{
  CURLcode result = CURLE_OK;

  ആകുമ്പോൾ(!result && beg < end)
    result = curlx_dyn_addf(store, "%02x:", *beg++);

  തിരികെ result;
}

സ്ഥിതം CURLcode bit2str(ഘടന dynbuf *store,
                        സ്ഥിരം uint8_t *beg, സ്ഥിരം uint8_t *end)
{
  /* Convert an ASN.1 bit string to a printable string. */

  എങ്കിൽ(beg < end) {
    uint8_t unused_bits = *beg++;
    എങ്കിൽ(unused_bits > 7)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  അല്ലെങ്കിൽ
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  തിരികെ octet2str(store, beg, end);
}

/*
 * Convert an ASN.1 integer value into its string representation.
 *
 * Returns error.
 */
സ്ഥിതം CURLcode int2str(ഘടന dynbuf *store,
                        സ്ഥിരം uint8_t *beg, സ്ഥിരം uint8_t *end)
{
  uint32_t uval = 0;
  int32_t sval = 0;
  size_t n = end - beg;
  size_t i;
  സ്ഥിരം uint8_t *p = beg;

  എങ്കിൽ(!n)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  എങ്കിൽ(n > 4)
    തിരികെ octet2str(store, beg, end);

  എങ്കിൽ(p[0] & 0x80)
    uval = ~0U;

  വേണ്ടി(i = 0; i < n; i++)
    uval = (uval << 8) | p[i];

  sval = (int32_t)uval;

  /* small values are easier to read as decimals */
  എങ്കിൽ(sval > -10000 && sval < 10000)
    തിരികെ curlx_dyn_addf(store, "%d", sval);

  /* represent larger values as hex */
  തിരികെ curlx_dyn_addf(store, "0x%x", uval);
}

/*
 * Convert from an ASN.1 typed string to UTF8.
 *
 * The result is stored in a dynbuf that is inited by the user of this
 * function.
 *
 * Returns error.
 */
സ്ഥിതം CURLcode utf8asn1str(ഘടന dynbuf *to, പൂർണ്ണസംഖ്യ type,
                            സ്ഥിരം uint8_t *from, സ്ഥിരം uint8_t *end)
{
  size_t inlength = end - from;
  പൂർണ്ണസംഖ്യ size = 1;
  CURLcode result = CURLE_OK;

  തിരഞ്ഞെടുപ്പ്(type) {
  അവസ്ഥ CURL_ASN1_BMP_STRING:
    size = 2;
    നിർത്തുക;
  അവസ്ഥ CURL_ASN1_UNIVERSAL_STRING:
    size = 4;
    നിർത്തുക;
  അവസ്ഥ CURL_ASN1_NUMERIC_STRING:
  അവസ്ഥ CURL_ASN1_PRINTABLE_STRING:
  അവസ്ഥ CURL_ASN1_TELETEX_STRING:
  അവസ്ഥ CURL_ASN1_IA5_STRING:
  അവസ്ഥ CURL_ASN1_VISIBLE_STRING:
  അവസ്ഥ CURL_ASN1_UTF8_STRING:
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;  /* Conversion not supported. */
  }

  എങ്കിൽ(inlength % size)
    /* Length inconsistent with character size. */
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  എങ്കിൽ(type == CURL_ASN1_UTF8_STRING) {
    /* copy. */
    എങ്കിൽ(inlength)
      result = curlx_dyn_addn(to, from, inlength);
  }
  അല്ലെങ്കിൽ {
    ആകുമ്പോൾ(!result && (from < end)) {
      അക്ഷരം buf[4]; /* decode buffer */
      size_t charsize = 1;
      unsigned പൂർണ്ണസംഖ്യ wc = 0;

      തിരഞ്ഞെടുപ്പ്(size) {
      അവസ്ഥ 4:
        wc = (wc << 8) | *from++;
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      അവസ്ഥ 2:
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      സ്ഥിരസ്ഥിതി: /* case 1: */
        wc = (wc << 8) | *from++;
      }
      എങ്കിൽ(wc >= 0x00000080) {
        എങ്കിൽ(wc >= 0x00000800) {
          എങ്കിൽ(wc >= 0x00010000) {
            എങ്കിൽ(wc >= 0x00200000) {
              /* Invalid char. size for target encoding. */
              തിരികെ CURLE_WEIRD_SERVER_REPLY;
            }
            buf[3] = (അക്ഷരം)(0x80 | (wc & 0x3F));
            wc = (wc >> 6) | 0x00010000;
            charsize++;
          }
          buf[2] = (അക്ഷരം)(0x80 | (wc & 0x3F));
          wc = (wc >> 6) | 0x00000800;
          charsize++;
        }
        buf[1] = (അക്ഷരം)(0x80 | (wc & 0x3F));
        wc = (wc >> 6) | 0x000000C0;
        charsize++;
      }
      buf[0] = (അക്ഷരം)wc;
      result = curlx_dyn_addn(to, buf, charsize);
    }
  }
  തിരികെ result;
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
UNITTEST CURLcode encodeOID(ഘടന dynbuf *store,
                            സ്ഥിരം uint8_t *beg, സ്ഥിരം uint8_t *end);
UNITTEST CURLcode encodeOID(ഘടന dynbuf *store,
                            സ്ഥിരം uint8_t *beg, സ്ഥിരം uint8_t *end)
{
  uint32_t x;
  uint32_t y;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(end <= beg)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  /* Process the first two numbers. The initial digit cannot be larger than
     2 */
  y = *beg++;
  എങ്കിൽ(y <= 80)
    x = y / 40;
  അല്ലെങ്കിൽ
    x = 2;
  result = curlx_dyn_addf(store, "%u", x);
  എങ്കിൽ(result)
    തിരികെ result;

  എങ്കിൽ(y & 0x80) {
    uint32_t t = (y & 0x7f); /* the second number */
    എങ്കിൽ(!t)
      /* reject leading 0x80 padding */
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    ചെയ്യുക {
      എങ്കിൽ((t & 0xFE000000) || (beg >= end))
        തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      t = (t << 7) | (y & 0x7F);
    } ആകുമ്പോൾ(y & 0x80);
    y = t;
  }

  result = curlx_dyn_addf(store, ".%u", y - (x * 40));
  എങ്കിൽ(result)
    തിരികെ result;

  /* Process the trailing numbers. */
  ആകുമ്പോൾ((beg < end) && !result) {
    x = 0;
    y = 0;
    ചെയ്യുക {
      /* 0x80 as the first value is invalid since it only adds padding */
      എങ്കിൽ((y & 0x80) && !x)
        തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
      അല്ലെങ്കിൽ എങ്കിൽ((x & 0xFE000000) || (beg == end))
        തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      x = (x << 7) | (y & 0x7F);
    } ആകുമ്പോൾ(y & 0x80);
    result = curlx_dyn_addf(store, ".%u", x);
  }
  തിരികെ result;
}

/*
 * Convert an ASN.1 OID into its dotted or symbolic string representation.
 *
 * Return error code.
 */

സ്ഥിതം CURLcode OID2str(ഘടന dynbuf *store,
                        സ്ഥിരം uint8_t *beg, സ്ഥിരം uint8_t *end)
{
  CURLcode result = CURLE_OK;
  എങ്കിൽ(beg < end) {
    ഘടന dynbuf buf;
    curlx_dyn_init(&buf, CURL_X509_STR_MAX);
    result = encodeOID(&buf, beg, end);

    എങ്കിൽ(!result) {
      സ്ഥിരം ഘടന Curl_OID *op = തിരയൽ_OID(curlx_dyn_ptr(&buf));
      എങ്കിൽ(op)
        result = curlx_dyn_add(store, op->textoid);
      അല്ലെങ്കിൽ
        result = curlx_dyn_add(store, curlx_dyn_ptr(&buf));
    }
    curlx_dyn_free(&buf);
  }
  തിരികെ result;
}

/*
 * @unittest 1656
 */
UNITTEST CURLcode GTime2str(ഘടന dynbuf *store,
                            സ്ഥിരം അക്ഷരം *beg, സ്ഥിരം അക്ഷരം *end);
UNITTEST CURLcode GTime2str(ഘടന dynbuf *store,
                            സ്ഥിരം അക്ഷരം *beg, സ്ഥിരം അക്ഷരം *end)
{
  സ്ഥിരം അക്ഷരം *tzp;
  സ്ഥിരം അക്ഷരം *fracp;
  അക്ഷരം sec1, sec2;
  size_t fracl;
  size_t tzl;
  സ്ഥിരം അക്ഷരം *sep = "";

  /* Convert an ASN.1 Generalized time to a printable string.
     Return the dynamically allocated string, or NULL if an error occurs. */

  വേണ്ടി(fracp = beg; fracp < end && ISDIGIT(*fracp); fracp++)
    ;

  /* Get seconds digits. */
  sec1 = '0';
  തിരഞ്ഞെടുപ്പ്(fracp - beg - 12) {
  അവസ്ഥ 0:
    sec2 = '0';
    നിർത്തുക;
  അവസ്ഥ 2:
    sec1 = fracp[-2];
    FALLTHROUGH();
  അവസ്ഥ 1:
    sec2 = fracp[-1];
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* timezone follows optional fractional seconds. */
  tzp = fracp;
  fracl = 0; /* no fractional seconds detected so far */
  എങ്കിൽ(fracp < end && (*fracp == '.' || *fracp == ',')) {
    /* Have fractional seconds, e.g. "[.,]\d+". How many? */
    fracp++; /* should be a digit char or BAD ARGUMENT */
    tzp = fracp;
    ആകുമ്പോൾ(tzp < end && ISDIGIT(*tzp))
      tzp++;
    എങ്കിൽ(tzp == fracp) /* never looped, no digit after [.,] */
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    fracl = tzp - fracp; /* number of fractional sec digits */
    DEBUGASSERT(fracl > 0);
    /* Strip trailing zeroes in fractional seconds.
     * May reduce fracl to 0 if only '0's are present. */
    ആകുമ്പോൾ(fracl && fracp[fracl - 1] == '0')
      fracl--;
  }

  /* Process timezone. */
  എങ്കിൽ(tzp >= end) {
    tzp = "";
    tzl = 0;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(*tzp == 'Z') {
    sep = " ";
    tzp = "GMT";
    tzl = 3;
  }
  അല്ലെങ്കിൽ എങ്കിൽ((*tzp == '+') || (*tzp == '-')) {
    sep = " UTC";
    tzl = end - tzp;
  }
  അല്ലെങ്കിൽ {
    sep = " ";
    tzl = end - tzp;
  }

  തിരികെ curlx_dyn_addf(store,
                        "%.4s-%.2s-%.2s %.2s:%.2s:%c%c%s%.*s%s%.*s",
                        beg, beg + 4, beg + 6,
                        beg + 8, beg + 10, sec1, sec2,
                        fracl ? "." : "", (പൂർണ്ണസംഖ്യ)fracl, fracp,
                        sep, (പൂർണ്ണസംഖ്യ)tzl, tzp);
}

/*
 * Convert an ASN.1 UTC time to a printable string.
 *
 * Return error code.
 */
സ്ഥിതം CURLcode UTime2str(ഘടന dynbuf *store,
                          സ്ഥിരം അക്ഷരം *beg, സ്ഥിരം അക്ഷരം *end)
{
  സ്ഥിരം അക്ഷരം *tzp;
  size_t tzl;
  സ്ഥിരം അക്ഷരം *sec;

  വേണ്ടി(tzp = beg; tzp < end && *tzp >= '0' && *tzp <= '9'; tzp++)
    ;
  /* Get the seconds. */
  sec = beg + 10;
  തിരഞ്ഞെടുപ്പ്(tzp - sec) {
  അവസ്ഥ 0:
    sec = "00";
    FALLTHROUGH();
  അവസ്ഥ 2:
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Process timezone. */
  എങ്കിൽ(tzp >= end)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  എങ്കിൽ(*tzp == 'Z') {
    tzp = "GMT";
    end = tzp + 3;
  }

  tzl = end - tzp;
  തിരികെ curlx_dyn_addf(store, "%d%.2s-%.2s-%.2s %.2s:%.2s:%.2s %.*s",
                        20 - (*beg >= '5'), beg, beg + 2, beg + 4,
                        beg + 6, beg + 8, sec,
                        (പൂർണ്ണസംഖ്യ)tzl, tzp);
}

/*
 * Convert an ASN.1 element to a printable string.
 *
 * Return error
 *
 * @unittest 1667
 */
UNITTEST CURLcode ASN1tostr(ഘടന dynbuf *store,
                            ഘടന Curl_asn1Element *elem);
UNITTEST CURLcode ASN1tostr(ഘടന dynbuf *store,
                            ഘടന Curl_asn1Element *elem)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  പൂർണ്ണസംഖ്യ type = elem->tag;
  എങ്കിൽ(elem->constructed)
    തിരികെ result; /* No conversion of structured elements. */

  തിരഞ്ഞെടുപ്പ്(type) {
  അവസ്ഥ CURL_ASN1_BOOLEAN:
    result = bool2str(store, elem->beg, elem->end);
    നിർത്തുക;
  അവസ്ഥ CURL_ASN1_INTEGER:
  അവസ്ഥ CURL_ASN1_ENUMERATED:
    result = int2str(store, elem->beg, elem->end);
    നിർത്തുക;
  അവസ്ഥ CURL_ASN1_BIT_STRING:
    result = bit2str(store, elem->beg, elem->end);
    നിർത്തുക;
  അവസ്ഥ CURL_ASN1_OCTET_STRING:
    result = octet2str(store, elem->beg, elem->end);
    നിർത്തുക;
  അവസ്ഥ CURL_ASN1_NULL:
    result = curlx_dyn_addn(store, "", 1);
    നിർത്തുക;
  അവസ്ഥ CURL_ASN1_OBJECT_IDENTIFIER:
    result = OID2str(store, elem->beg, elem->end);
    നിർത്തുക;
  അവസ്ഥ CURL_ASN1_UTC_TIME:
    result = UTime2str(store, (സ്ഥിരം അക്ഷരം *)elem->beg,
                       (സ്ഥിരം അക്ഷരം *)elem->end);
    നിർത്തുക;
  അവസ്ഥ CURL_ASN1_GENERALIZED_TIME:
    result = GTime2str(store, (സ്ഥിരം അക്ഷരം *)elem->beg,
                       (സ്ഥിരം അക്ഷരം *)elem->end);
    നിർത്തുക;
  അവസ്ഥ CURL_ASN1_UTF8_STRING:
  അവസ്ഥ CURL_ASN1_NUMERIC_STRING:
  അവസ്ഥ CURL_ASN1_PRINTABLE_STRING:
  അവസ്ഥ CURL_ASN1_TELETEX_STRING:
  അവസ്ഥ CURL_ASN1_IA5_STRING:
  അവസ്ഥ CURL_ASN1_VISIBLE_STRING:
  അവസ്ഥ CURL_ASN1_UNIVERSAL_STRING:
  അവസ്ഥ CURL_ASN1_BMP_STRING:
    result = utf8asn1str(store, type, elem->beg, elem->end);
    നിർത്തുക;
  }

  തിരികെ result;
}

/*
 * ASCII encode distinguished name at `dn' into the store dynbuf.
 *
 * Returns error.
 */
സ്ഥിതം CURLcode encodeDN(ഘടന dynbuf *store, ഘടന Curl_asn1Element *dn)
{
  ഘടന Curl_asn1Element rdn;
  ഘടന Curl_asn1Element atv;
  ഘടന Curl_asn1Element oid;
  ഘടന Curl_asn1Element മൂല്യം;
  സ്ഥിരം uint8_t *p1;
  സ്ഥിരം uint8_t *p2;
  സ്ഥിരം uint8_t *p3;
  സ്ഥിരം uint8_t *str;
  CURLcode result = CURLE_OK;
  bool added = FALSE;
  ഘടന dynbuf temp;
  curlx_dyn_init(&temp, CURL_X509_STR_MAX);

  വേണ്ടി(p1 = dn->beg; p1 < dn->end;) {
    p1 = getASN1Element(&rdn, p1, dn->end);
    എങ്കിൽ(!p1) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      പോകുക error;
    }
    വേണ്ടി(p2 = rdn.beg; p2 < rdn.end;) {
      p2 = getASN1Element(&atv, p2, rdn.end);
      എങ്കിൽ(!p2) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        പോകുക error;
      }
      p3 = getASN1Element(&oid, atv.beg, atv.end);
      എങ്കിൽ(!p3) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        പോകുക error;
      }
      എങ്കിൽ(!getASN1Element(&മൂല്യം, p3, atv.end)) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        പോകുക error;
      }
      curlx_dyn_reset(&temp);
      result = ASN1tostr(&temp, &oid);
      എങ്കിൽ(result)
        പോകുക error;

      str = curlx_dyn_uptr(&temp);

      എങ്കിൽ(!str) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        പോകുക error;
      }

      /* Encode delimiter.
         If attribute has a short uppercase name, delimiter is ", ". */
      വേണ്ടി(p3 = str; ISUPPER(*p3); p3++)
        ;
      എങ്കിൽ(added) {
        എങ്കിൽ(p3 - str > 2)
          result = curlx_dyn_addn(store, "/", 1);
        അല്ലെങ്കിൽ
          result = curlx_dyn_addn(store, ", ", 2);
        എങ്കിൽ(result)
          പോകുക error;
      }

      /* Encode attribute name. */
      result = curlx_dyn_add(store, (സ്ഥിരം അക്ഷരം *)str);
      എങ്കിൽ(result)
        പോകുക error;

      /* Generate equal sign. */
      result = curlx_dyn_addn(store, "=", 1);
      എങ്കിൽ(result)
        പോകുക error;

      /* Generate value. */
      result = ASN1tostr(store, &മൂല്യം);
      എങ്കിൽ(result)
        പോകുക error;
      curlx_dyn_reset(&temp);
      added = TRUE; /* use separator for next */
    }
  }
error:
  curlx_dyn_free(&temp);

  തിരികെ result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#ifdef WANT_PARSEX509
/*
 * ASN.1 parse an X509 certificate into structure subfields.
 * Syntax is assumed to have already been checked by the SSL backend.
 * See RFC 5280.
 */
പൂർണ്ണസംഖ്യ Curl_parseX509(ഘടന Curl_X509certificate *cert,
                   സ്ഥിരം uint8_t *beg, സ്ഥിരം uint8_t *end)
{
  ഘടന Curl_asn1Element elem;
  ഘടന Curl_asn1Element tbsCertificate;
  സ്ഥിരം uint8_t *ccp;
  സ്ഥിതം സ്ഥിരം uint8_t defaultVersion = 0;  /* v1. */

  memset(cert, 0, വലുപ്പം(*cert));
  cert->certificate.header = ശൂന്യ;
  cert->certificate.beg = beg;
  cert->certificate.end = end;

  /* Get the sequence content. */
  എങ്കിൽ(!getASN1Element(&elem, beg, end))
    തിരികെ -1;  /* Invalid bounds/size. */
  beg = elem.beg;
  end = elem.end;

  /* Get tbsCertificate. */
  beg = getASN1Element(&tbsCertificate, beg, end);
  എങ്കിൽ(!beg)
    തിരികെ -1;
  /* Skip the signatureAlgorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  എങ്കിൽ(!beg)
    തിരികെ -1;
  /* Get the signatureValue. */
  എങ്കിൽ(!getASN1Element(&cert->signature, beg, end))
    തിരികെ -1;

  /* Parse TBSCertificate. */
  beg = tbsCertificate.beg;
  end = tbsCertificate.end;
  /* Get optional version, get serialNumber. */
  cert->version.header = ശൂന്യ;
  cert->version.beg = &defaultVersion;
  cert->version.end = &defaultVersion + വലുപ്പം(defaultVersion);
  beg = getASN1Element(&elem, beg, end);
  എങ്കിൽ(!beg)
    തിരികെ -1;
  എങ്കിൽ(elem.tag == 0) {
    എങ്കിൽ(!getASN1Element(&cert->version, elem.beg, elem.end))
      തിരികെ -1;
    beg = getASN1Element(&elem, beg, end);
    എങ്കിൽ(!beg)
      തിരികെ -1;
  }
  cert->serial_സംഖ്യ = elem;
  /* Get signature algorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  /* Get issuer. */
  beg = getASN1Element(&cert->issuer, beg, end);
  എങ്കിൽ(!beg)
    തിരികെ -1;
  /* Get notBefore and notAfter. */
  beg = getASN1Element(&elem, beg, end);
  എങ്കിൽ(!beg)
    തിരികെ -1;
  ccp = getASN1Element(&cert->notBefore, elem.beg, elem.end);
  എങ്കിൽ(!ccp)
    തിരികെ -1;
  എങ്കിൽ(!getASN1Element(&cert->notAfter, ccp, elem.end))
    തിരികെ -1;
  /* These two fields MUST consume all of the validity elem */
  എങ്കിൽ(elem.end != cert->notAfter.end)
    തിരികെ -1;
  /* Get subject. */
  beg = getASN1Element(&cert->subject, beg, end);
  എങ്കിൽ(!beg)
    തിരികെ -1;
  /* Get subjectPublicKeyAlgorithm and subjectPublicKey. */
  beg = getASN1Element(&cert->subject_Public_താക്കോൽ_Info, beg, end);
  എങ്കിൽ(!beg)
    തിരികെ -1;
  ccp = getASN1Element(&cert->subject_Public_താക്കോൽ_Algorithm,
                       cert->subject_Public_താക്കോൽ_Info.beg,
                       cert->subject_Public_താക്കോൽ_Info.end);
  എങ്കിൽ(!ccp)
    തിരികെ -1;
  എങ്കിൽ(!getASN1Element(&cert->subject_Public_താക്കോൽ, ccp,
                     cert->subject_Public_താക്കോൽ_Info.end))
    തിരികെ -1;
  /* Get optional issuerUniqueID, subjectUniqueID and extensions. */
  cert->issuerUniqueID.tag = cert->subjectUniqueID.tag = 0;
  cert->extensions.tag = elem.tag = 0;
  cert->issuerUniqueID.header = cert->subjectUniqueID.header = ശൂന്യ;
  cert->issuerUniqueID.beg = cert->issuerUniqueID.end = beg;
  cert->subjectUniqueID.beg = cert->subjectUniqueID.end = beg;
  cert->extensions.header = ശൂന്യ;
  cert->extensions.beg = cert->extensions.end = beg;
  എങ്കിൽ(beg < end) {
    beg = getASN1Element(&elem, beg, end);
    എങ്കിൽ(!beg)
      തിരികെ -1;
  }
  എങ്കിൽ(elem.tag == 1) {
    cert->issuerUniqueID = elem;
    എങ്കിൽ(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      എങ്കിൽ(!beg)
        തിരികെ -1;
    }
  }
  എങ്കിൽ(elem.tag == 2) {
    cert->subjectUniqueID = elem;
    എങ്കിൽ(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      എങ്കിൽ(!beg)
        തിരികെ -1;
    }
  }
  എങ്കിൽ(elem.tag == 3)
    എങ്കിൽ(!getASN1Element(&cert->extensions, elem.beg, elem.end))
      തിരികെ -1;
  തിരികെ 0;
}

#endif /* WANT_PARSEX509 */

#ifdef WANT_EXTRACT_CERTINFO

സ്ഥിതം CURLcode dumpAlgo(ഘടന dynbuf *store,
                         ഘടന Curl_asn1Element *param,
                         സ്ഥിരം uint8_t *beg, സ്ഥിരം uint8_t *end)
{
  ഘടന Curl_asn1Element oid;

  /* Get algorithm parameters and return algorithm name. */

  beg = getASN1Element(&oid, beg, end);
  എങ്കിൽ(!beg)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  param->header = ശൂന്യ;
  param->tag = 0;
  param->beg = param->end = end;
  എങ്കിൽ(beg < end) {
    സ്ഥിരം uint8_t *p = getASN1Element(param, beg, end);
    എങ്കിൽ(!p)
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  തിരികെ OID2str(store, oid.beg, oid.end);
}

/*
 * This is a convenience function for push_certinfo_len that takes a zero
 * terminated value.
 */
സ്ഥിതം CURLcode ssl_തള്ളുക_certinfo(ഘടന Curl_easy *data,
                                  പൂർണ്ണസംഖ്യ certnum,
                                  സ്ഥിരം അക്ഷരം *label,
                                  സ്ഥിരം അക്ഷരം *മൂല്യം)
{
  size_t valuelen = strlen(മൂല്യം);

  തിരികെ Curl_ssl_തള്ളുക_certinfo_len(data, certnum, label, മൂല്യം, valuelen);
}

/*
 * This is a convenience function for push_certinfo_len that takes a
 * dynbuf value.
 *
 * It also does the verbose output if !certnum.
 */
സ്ഥിതം CURLcode ssl_തള്ളുക_certinfo_dyn(ഘടന Curl_easy *data,
                                      പൂർണ്ണസംഖ്യ certnum,
                                      സ്ഥിരം അക്ഷരം *label,
                                      ഘടന dynbuf *ptr)
{
  size_t valuelen = curlx_dyn_len(ptr);
  സ്ഥിരം അക്ഷരം *മൂല്യം = curlx_dyn_ptr(ptr);

  CURLcode result = Curl_ssl_തള്ളുക_certinfo_len(data, certnum, label,
                                               മൂല്യം, valuelen);

  എങ്കിൽ(!certnum && !result)
    infof(data, "   %s: %s", label, മൂല്യം);

  തിരികെ result;
}

സ്ഥിതം CURLcode do_pubkey_field(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ certnum,
                                സ്ഥിരം അക്ഷരം *label,
                                ഘടന Curl_asn1Element *elem)
{
  CURLcode result;
  ഘടന dynbuf out;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);

  /* Generate a certificate information record for the public key. */

  result = ASN1tostr(&out, elem);
  എങ്കിൽ(!result && data->set.ssl.certinfo)
    result = ssl_തള്ളുക_certinfo_dyn(data, certnum, label, &out);
  curlx_dyn_free(&out);
  തിരികെ result;
}

/* return 0 on success, 1 on error */
സ്ഥിതം പൂർണ്ണസംഖ്യ do_pubkey(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ certnum, സ്ഥിരം അക്ഷരം *algo,
                     ഘടന Curl_asn1Element *param,
                     ഘടന Curl_asn1Element *pubkey)
{
  ഘടന Curl_asn1Element elem;
  ഘടന Curl_asn1Element pk;
  സ്ഥിരം uint8_t *p;

  /* Generate all information records for the public key. */

  എങ്കിൽ(curl_strequal(algo, "ecPublicKey")) {
    /*
     * ECC public key is all the data, a value of type BIT STRING mapped to
     * OCTET STRING and should not be parsed as an ASN.1 value.
     */
    സ്ഥിരം size_t dlen = pubkey->end - pubkey->beg;
    size_t len;
    എങ്കിൽ(dlen < 2)
      /* too small */
      തിരികെ 1;
    len = (dlen - 2) * 4;
    എങ്കിൽ(!certnum)
      infof(data, "   ECC Public Key (%zu bits)", len);
    എങ്കിൽ(data->set.ssl.certinfo) {
      അക്ഷരം q[(വലുപ്പം(len) * 8 / 3) + 1];
      (ശൂന്യം)curl_msnprintf(q, വലുപ്പം(q), "%zu", len);
      എങ്കിൽ(ssl_തള്ളുക_certinfo(data, certnum, "ECC Public Key", q))
        തിരികെ 1;
    }
    തിരികെ do_pubkey_field(data, certnum, "ecPublicKey", pubkey) == CURLE_OK
      ? 0 : 1;
  }

  /* Get the public key (single element). */
  എങ്കിൽ(!getASN1Element(&pk, pubkey->beg + 1, pubkey->end))
    തിരികെ 1;

  എങ്കിൽ(curl_strequal(algo, "rsaEncryption")) {
    സ്ഥിരം uint8_t *q;
    size_t len;

    p = getASN1Element(&elem, pk.beg, pk.end);
    എങ്കിൽ(!p)
      തിരികെ 1;

    /* Compute key length. */
    വേണ്ടി(q = elem.beg; q < elem.end && !*q; q++)
      ;
    len = ((elem.end - q) * 8);
    എങ്കിൽ(len) {
      unsigned പൂർണ്ണസംഖ്യ i;
      വേണ്ടി(i = *q; !(i & 0x80); i <<= 1)
        len--;
    }
    എങ്കിൽ(len > 32)
      elem.beg = q;     /* Strip leading zero bytes. */
    എങ്കിൽ(!certnum)
      infof(data, "   RSA Public Key (%zu bits)", len);
    എങ്കിൽ(data->set.ssl.certinfo) {
      അക്ഷരം r[(വലുപ്പം(len) * 8 / 3) + 1];
      curl_msnprintf(r, വലുപ്പം(r), "%zu", len);
      എങ്കിൽ(ssl_തള്ളുക_certinfo(data, certnum, "RSA Public Key", r))
        തിരികെ 1;
    }
    /* Generate coefficients. */
    എങ്കിൽ(do_pubkey_field(data, certnum, "rsa(n)", &elem))
      തിരികെ 1;
    എങ്കിൽ(!getASN1Element(&elem, p, pk.end))
      തിരികെ 1;
    എങ്കിൽ(do_pubkey_field(data, certnum, "rsa(e)", &elem))
      തിരികെ 1;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal(algo, "dsa")) {
    p = getASN1Element(&elem, param->beg, param->end);
    എങ്കിൽ(p) {
      എങ്കിൽ(do_pubkey_field(data, certnum, "dsa(p)", &elem))
        തിരികെ 1;
      p = getASN1Element(&elem, p, param->end);
      എങ്കിൽ(p) {
        എങ്കിൽ(do_pubkey_field(data, certnum, "dsa(q)", &elem))
          തിരികെ 1;
        എങ്കിൽ(getASN1Element(&elem, p, param->end)) {
          എങ്കിൽ(do_pubkey_field(data, certnum, "dsa(g)", &elem))
            തിരികെ 1;
          എങ്കിൽ(do_pubkey_field(data, certnum, "dsa(pub_key)", &pk))
            തിരികെ 1;
        }
      }
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(curl_strequal(algo, "dhpublicnumber")) {
    p = getASN1Element(&elem, param->beg, param->end);
    എങ്കിൽ(p) {
      എങ്കിൽ(do_pubkey_field(data, certnum, "dh(p)", &elem))
        തിരികെ 1;
      എങ്കിൽ(getASN1Element(&elem, p, param->end)) {
        എങ്കിൽ(do_pubkey_field(data, certnum, "dh(g)", &elem))
          തിരികെ 1;
        എങ്കിൽ(do_pubkey_field(data, certnum, "dh(pub_key)", &pk))
          തിരികെ 1;
      }
    }
  }
  തിരികെ 0;
}

/*
 * Convert an ASN.1 distinguished name into a printable string.
 * Return error.
 */
സ്ഥിതം CURLcode DNtostr(ഘടന dynbuf *store, ഘടന Curl_asn1Element *dn)
{
  തിരികെ encodeDN(store, dn);
}

CURLcode Curl_extract_certinfo(ഘടന Curl_easy *data,
                               പൂർണ്ണസംഖ്യ certnum,
                               സ്ഥിരം uint8_t *beg,
                               സ്ഥിരം uint8_t *end)
{
  ഘടന Curl_X509certificate cert;
  ഘടന Curl_asn1Element param;
  അക്ഷരം *certptr;
  size_t clen;
  ഘടന dynbuf out;
  CURLcode result = CURLE_OK;
  unsigned പൂർണ്ണസംഖ്യ version;
  സ്ഥിരം uint8_t *ptr;
  പൂർണ്ണസംഖ്യ rc;

  എങ്കിൽ(!data->set.ssl.certinfo)
    എങ്കിൽ(certnum)
      തിരികെ CURLE_OK;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);
  /* Prepare the certificate information for curl_easy_getinfo(). */

  /* Extract the certificate ASN.1 elements. */
  എങ്കിൽ(Curl_parseX509(&cert, beg, end))
    തിരികെ CURLE_PEER_FAILED_VERIFICATION;

  /* Subject. */
  result = DNtostr(&out, &cert.subject);
  എങ്കിൽ(result)
    പോകുക done;
  എങ്കിൽ(data->set.ssl.certinfo) {
    result = ssl_തള്ളുക_certinfo_dyn(data, certnum, "Subject", &out);
    എങ്കിൽ(result)
      പോകുക done;
  }
  curlx_dyn_reset(&out);

  /* Issuer. */
  result = DNtostr(&out, &cert.issuer);
  എങ്കിൽ(result)
    പോകുക done;
  എങ്കിൽ(data->set.ssl.certinfo) {
    result = ssl_തള്ളുക_certinfo_dyn(data, certnum, "Issuer", &out);
    എങ്കിൽ(result)
      പോകുക done;
  }
  curlx_dyn_reset(&out);

  /* Version (always fits in less than 32 bits). */
  version = 0;
  വേണ്ടി(ptr = cert.version.beg; ptr < cert.version.end; ptr++)
    version = (version << 8) | *ptr;
  എങ്കിൽ(data->set.ssl.certinfo) {
    result = curlx_dyn_addf(&out, "%x", version);
    എങ്കിൽ(result)
      പോകുക done;
    result = ssl_തള്ളുക_certinfo_dyn(data, certnum, "Version", &out);
    എങ്കിൽ(result)
      പോകുക done;
    curlx_dyn_reset(&out);
  }

  /* Serial number. */
  result = ASN1tostr(&out, &cert.serial_സംഖ്യ);
  എങ്കിൽ(result)
    പോകുക done;
  എങ്കിൽ(data->set.ssl.certinfo) {
    result = ssl_തള്ളുക_certinfo_dyn(data, certnum, "Serial Number", &out);
    എങ്കിൽ(result)
      പോകുക done;
  }
  curlx_dyn_reset(&out);

  /* Signature algorithm .*/
  result = dumpAlgo(&out, &param, cert.signatureAlgorithm.beg,
                    cert.signatureAlgorithm.end);
  എങ്കിൽ(result)
    പോകുക done;
  എങ്കിൽ(data->set.ssl.certinfo) {
    result = ssl_തള്ളുക_certinfo_dyn(data, certnum, "Signature Algorithm", &out);
    എങ്കിൽ(result)
      പോകുക done;
  }
  curlx_dyn_reset(&out);

  /* Start Date. */
  result = ASN1tostr(&out, &cert.notBefore);
  എങ്കിൽ(result)
    പോകുക done;
  എങ്കിൽ(data->set.ssl.certinfo) {
    result = ssl_തള്ളുക_certinfo_dyn(data, certnum, "Start Date", &out);
    എങ്കിൽ(result)
      പോകുക done;
  }
  curlx_dyn_reset(&out);

  /* Expire Date. */
  result = ASN1tostr(&out, &cert.notAfter);
  എങ്കിൽ(result)
    പോകുക done;
  എങ്കിൽ(data->set.ssl.certinfo) {
    result = ssl_തള്ളുക_certinfo_dyn(data, certnum, "Expire Date", &out);
    എങ്കിൽ(result)
      പോകുക done;
  }
  curlx_dyn_reset(&out);

  /* Public Key Algorithm. */
  result = dumpAlgo(&out, &param, cert.subject_Public_താക്കോൽ_Algorithm.beg,
                    cert.subject_Public_താക്കോൽ_Algorithm.end);
  എങ്കിൽ(result)
    പോകുക done;
  എങ്കിൽ(data->set.ssl.certinfo) {
    result = ssl_തള്ളുക_certinfo_dyn(data, certnum, "Public Key Algorithm",
                                   &out);
    എങ്കിൽ(result)
      പോകുക done;
  }

  rc = do_pubkey(data, certnum, curlx_dyn_ptr(&out),
                 &param, &cert.subject_Public_താക്കോൽ);
  എങ്കിൽ(rc) {
    result = CURLE_OUT_OF_MEMORY; /* the most likely error */
    പോകുക done;
  }
  curlx_dyn_reset(&out);

  /* Signature. */
  result = ASN1tostr(&out, &cert.signature);
  എങ്കിൽ(result)
    പോകുക done;
  എങ്കിൽ(data->set.ssl.certinfo) {
    result = ssl_തള്ളുക_certinfo_dyn(data, certnum, "Signature", &out);
    എങ്കിൽ(result)
      പോകുക done;
  }
  curlx_dyn_reset(&out);

  /* Generate PEM certificate. */
  result = curlx_base64_encode((സ്ഥിരം uint8_t *)cert.certificate.beg,
                               cert.certificate.end - cert.certificate.beg,
                               &certptr, &clen);
  എങ്കിൽ(result)
    പോകുക done;

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
  എങ്കിൽ(!result) {
    size_t j = 0;

    ആകുമ്പോൾ(!result && (j < clen)) {
      size_t chunksize = (clen - j) > 64 ? 64 : (clen - j);
      result = curlx_dyn_addn(&out, &certptr[j], chunksize);
      എങ്കിൽ(!result)
        result = curlx_dyn_addn(&out, "\n", 1);
      j += chunksize;
    }
    എങ്കിൽ(!result)
      result = curlx_dyn_add(&out, "-----END CERTIFICATE-----\n");
  }
  curlx_free(certptr);
  എങ്കിൽ(!result)
    എങ്കിൽ(data->set.ssl.certinfo)
      result = ssl_തള്ളുക_certinfo_dyn(data, certnum, "Cert", &out);

done:
  എങ്കിൽ(result)
    failf(data, "Failed extracting certificate chain");
  curlx_dyn_free(&out);
  തിരികെ result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#endif /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
