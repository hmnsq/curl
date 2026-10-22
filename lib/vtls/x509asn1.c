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
#અન્તર્ભાવ "curl_setup.h"

#જો defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#જો defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_RUSTLS)
#define WANT_PARSEX509 /* uses Curl_parseX509() */
#endif

#જો defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_RUSTLS)
#define WANT_EXTRACT_CERTINFO /* uses Curl_extract_certinfo() */
#endif

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "curlx/base64.h"
#અન્તર્ભાવ "vtls/x509asn1.h"
#અન્તર્ભાવ "curlx/dynbuf.h"

/*
 * Constants.
 */

/* Largest supported ASN.1 structure. */
#define CURL_ASN1_MAX                   ((size_t)0x40000)      /* 256K */

#ifdef WANT_EXTRACT_CERTINFO

/* ASN.1 OID table entry. */
સંરચના Curl_OID {
  અચલ અક્ષર *numoid;  /* Dotted-numeric OID. */
  અચલ અક્ષર *textoid; /* OID name. */
};

/* ASN.1 OIDs. */
સ્થિર અચલ સંરચના Curl_OID OIDtable[] = {
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
  { ખાલી,                       ખાલી }
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

સ્થિર અચલ uint8_t *getASN1Element_(સંરચના Curl_asn1Element *elem,
                                      અચલ uint8_t *beg, અચલ uint8_t *end,
                                      size_t lvl)
{
  uint8_t b;
  size_t len;
  સંરચના Curl_asn1Element lelem;

  /* Get a single ASN.1 element into `elem', parse ASN.1 string at `beg'
     ending at `end'.
     Returns a pointer in source string after the parsed element, or NULL
     if an error occurs. */
  જો(!beg || !end || beg >= end || !*beg ||
     ((size_t)(end - beg) > CURL_ASN1_MAX) ||
     lvl++ >= CURL_ASN1_MAX_RECURSIONS)
    પાછા ખાલી;

  /* Process header byte. */
  elem->header = beg;
  b = *beg++;
  elem->constructed = (b & 0x20) != 0;
  elem->eclass = (b >> 6) & 3;
  b &= 0x1F;
  જો(b == 0x1F)
    પાછા ખાલી; /* Long tag values not supported here. */
  elem->tag = b;

  /* Process length. */
  જો(beg >= end)
    પાછા ખાલી;
  b = *beg++;
  જો(!(b & 0x80))
    len = b;
  નહીંતર {
    b &= 0x7F;
    જો(!b) {
      /* Unspecified length. Since we have all the data, we can determine the
         effective length by skipping element until an EOC element is found. */
      જો(!elem->constructed)
        પાછા ખાલી;
      elem->beg = beg;
      જ્યારે(beg < end && *beg) {
        beg = getASN1Element_(&lelem, beg, end, lvl);
        જો(!beg)
          પાછા ખાલી;
      }
      જો(end - beg < 2 || beg[1])       /* EOC must have a zero length. */
        પાછા ખાલી;
      elem->end = beg;                  /* Do not include EOC as content. */
      પાછા beg + 2;                   /* After EOC element. */
    }
    નહીંતર જો(b > (size_t)(end - beg))
      પાછા ખાલી; /* Does not fit in source. */
    નહીંતર {
      /* Get long length. */
      len = 0;
      કરો {
        જો(len & 0xFF000000L)
          પાછા ખાલી;  /* Lengths > 32 bits are not supported. */
        len = (len << 8) | *beg++;
      } જ્યારે(--b);
    }
  }
  જો(len > (size_t)(end - beg))
    પાછા ખાલી;  /* Element data does not fit in source. */
  elem->beg = beg;
  elem->end = beg + len;
  પાછા elem->end;
}

/*
 * @unittest 1657
 */
UNITTEST અચલ uint8_t *getASN1Element(સંરચના Curl_asn1Element *elem,
                                       અચલ uint8_t *beg, અચલ uint8_t *end);
UNITTEST અચલ uint8_t *getASN1Element(સંરચના Curl_asn1Element *elem,
                                       અચલ uint8_t *beg, અચલ uint8_t *end)
{
  પાછા getASN1Element_(elem, beg, end, 0);
}

#ifdef WANT_EXTRACT_CERTINFO

/*
 * Search the null-terminated OID or OID identifier in local table.
 * Return the table entry pointer or NULL if not found.
 */
સ્થિર અચલ સંરચના Curl_OID *શોધ_OID(અચલ અક્ષર *oid)
{
  અચલ સંરચના Curl_OID *op;
  માટે(op = OIDtable; op->numoid; op++)
    જો(!strcmp(op->numoid, oid) || curl_strequal(op->textoid, oid))
      પાછા op;

  પાછા ખાલી;
}

/*
 * Convert an ASN.1 Boolean value into its string representation.
 *
 * Return error code.
 */

સ્થિર CURLcode bool2str(સંરચના dynbuf *store,
                         અચલ uint8_t *beg, અચલ uint8_t *end)
{
  uint8_t val;
  જો(end - beg != 1)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  val = *beg;
  જો((val != 0xff) && (val != 0x00))
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  પાછા curlx_dyn_add(store, val ? "TRUE" : "FALSE");
}

/*
 * Convert an ASN.1 octet string to a printable string.
 *
 * Return error code.
 */
સ્થિર CURLcode octet2str(સંરચના dynbuf *store,
                          અચલ uint8_t *beg, અચલ uint8_t *end)
{
  CURLcode result = CURLE_OK;

  જ્યારે(!result && beg < end)
    result = curlx_dyn_addf(store, "%02x:", *beg++);

  પાછા result;
}

સ્થિર CURLcode bit2str(સંરચના dynbuf *store,
                        અચલ uint8_t *beg, અચલ uint8_t *end)
{
  /* Convert an ASN.1 bit string to a printable string. */

  જો(beg < end) {
    uint8_t unused_bits = *beg++;
    જો(unused_bits > 7)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }
  નહીંતર
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  પાછા octet2str(store, beg, end);
}

/*
 * Convert an ASN.1 integer value into its string representation.
 *
 * Returns error.
 */
સ્થિર CURLcode int2str(સંરચના dynbuf *store,
                        અચલ uint8_t *beg, અચલ uint8_t *end)
{
  uint32_t uval = 0;
  int32_t sval = 0;
  size_t n = end - beg;
  size_t i;
  અચલ uint8_t *p = beg;

  જો(!n)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  જો(n > 4)
    પાછા octet2str(store, beg, end);

  જો(p[0] & 0x80)
    uval = ~0U;

  માટે(i = 0; i < n; i++)
    uval = (uval << 8) | p[i];

  sval = (int32_t)uval;

  /* small values are easier to read as decimals */
  જો(sval > -10000 && sval < 10000)
    પાછા curlx_dyn_addf(store, "%d", sval);

  /* represent larger values as hex */
  પાછા curlx_dyn_addf(store, "0x%x", uval);
}

/*
 * Convert from an ASN.1 typed string to UTF8.
 *
 * The result is stored in a dynbuf that is inited by the user of this
 * function.
 *
 * Returns error.
 */
સ્થિર CURLcode utf8asn1str(સંરચના dynbuf *to, પૂર્ણાંક type,
                            અચલ uint8_t *from, અચલ uint8_t *end)
{
  size_t inlength = end - from;
  પૂર્ણાંક size = 1;
  CURLcode result = CURLE_OK;

  પસંદગી(type) {
  સ્થિતિ CURL_ASN1_BMP_STRING:
    size = 2;
    તોડો;
  સ્થિતિ CURL_ASN1_UNIVERSAL_STRING:
    size = 4;
    તોડો;
  સ્થિતિ CURL_ASN1_NUMERIC_STRING:
  સ્થિતિ CURL_ASN1_PRINTABLE_STRING:
  સ્થિતિ CURL_ASN1_TELETEX_STRING:
  સ્થિતિ CURL_ASN1_IA5_STRING:
  સ્થિતિ CURL_ASN1_VISIBLE_STRING:
  સ્થિતિ CURL_ASN1_UTF8_STRING:
    તોડો;
  મૂળભૂત:
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;  /* Conversion not supported. */
  }

  જો(inlength % size)
    /* Length inconsistent with character size. */
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  જો(type == CURL_ASN1_UTF8_STRING) {
    /* copy. */
    જો(inlength)
      result = curlx_dyn_addn(to, from, inlength);
  }
  નહીંતર {
    જ્યારે(!result && (from < end)) {
      અક્ષર buf[4]; /* decode buffer */
      size_t charsize = 1;
      unsigned પૂર્ણાંક wc = 0;

      પસંદગી(size) {
      સ્થિતિ 4:
        wc = (wc << 8) | *from++;
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      સ્થિતિ 2:
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      મૂળભૂત: /* case 1: */
        wc = (wc << 8) | *from++;
      }
      જો(wc >= 0x00000080) {
        જો(wc >= 0x00000800) {
          જો(wc >= 0x00010000) {
            જો(wc >= 0x00200000) {
              /* Invalid char. size for target encoding. */
              પાછા CURLE_WEIRD_SERVER_REPLY;
            }
            buf[3] = (અક્ષર)(0x80 | (wc & 0x3F));
            wc = (wc >> 6) | 0x00010000;
            charsize++;
          }
          buf[2] = (અક્ષર)(0x80 | (wc & 0x3F));
          wc = (wc >> 6) | 0x00000800;
          charsize++;
        }
        buf[1] = (અક્ષર)(0x80 | (wc & 0x3F));
        wc = (wc >> 6) | 0x000000C0;
        charsize++;
      }
      buf[0] = (અક્ષર)wc;
      result = curlx_dyn_addn(to, buf, charsize);
    }
  }
  પાછા result;
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
UNITTEST CURLcode encodeOID(સંરચના dynbuf *store,
                            અચલ uint8_t *beg, અચલ uint8_t *end);
UNITTEST CURLcode encodeOID(સંરચના dynbuf *store,
                            અચલ uint8_t *beg, અચલ uint8_t *end)
{
  uint32_t x;
  uint32_t y;
  CURLcode result = CURLE_OK;

  જો(end <= beg)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  /* Process the first two numbers. The initial digit cannot be larger than
     2 */
  y = *beg++;
  જો(y <= 80)
    x = y / 40;
  નહીંતર
    x = 2;
  result = curlx_dyn_addf(store, "%u", x);
  જો(result)
    પાછા result;

  જો(y & 0x80) {
    uint32_t t = (y & 0x7f); /* the second number */
    જો(!t)
      /* reject leading 0x80 padding */
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    કરો {
      જો((t & 0xFE000000) || (beg >= end))
        પાછા CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      t = (t << 7) | (y & 0x7F);
    } જ્યારે(y & 0x80);
    y = t;
  }

  result = curlx_dyn_addf(store, ".%u", y - (x * 40));
  જો(result)
    પાછા result;

  /* Process the trailing numbers. */
  જ્યારે((beg < end) && !result) {
    x = 0;
    y = 0;
    કરો {
      /* 0x80 as the first value is invalid since it only adds padding */
      જો((y & 0x80) && !x)
        પાછા CURLE_BAD_FUNCTION_ARGUMENT;
      નહીંતર જો((x & 0xFE000000) || (beg == end))
        પાછા CURLE_BAD_FUNCTION_ARGUMENT;
      y = *beg++;
      x = (x << 7) | (y & 0x7F);
    } જ્યારે(y & 0x80);
    result = curlx_dyn_addf(store, ".%u", x);
  }
  પાછા result;
}

/*
 * Convert an ASN.1 OID into its dotted or symbolic string representation.
 *
 * Return error code.
 */

સ્થિર CURLcode OID2str(સંરચના dynbuf *store,
                        અચલ uint8_t *beg, અચલ uint8_t *end)
{
  CURLcode result = CURLE_OK;
  જો(beg < end) {
    સંરચના dynbuf buf;
    curlx_dyn_init(&buf, CURL_X509_STR_MAX);
    result = encodeOID(&buf, beg, end);

    જો(!result) {
      અચલ સંરચના Curl_OID *op = શોધ_OID(curlx_dyn_ptr(&buf));
      જો(op)
        result = curlx_dyn_add(store, op->textoid);
      નહીંતર
        result = curlx_dyn_add(store, curlx_dyn_ptr(&buf));
    }
    curlx_dyn_free(&buf);
  }
  પાછા result;
}

/*
 * @unittest 1656
 */
UNITTEST CURLcode GTime2str(સંરચના dynbuf *store,
                            અચલ અક્ષર *beg, અચલ અક્ષર *end);
UNITTEST CURLcode GTime2str(સંરચના dynbuf *store,
                            અચલ અક્ષર *beg, અચલ અક્ષર *end)
{
  અચલ અક્ષર *tzp;
  અચલ અક્ષર *fracp;
  અક્ષર sec1, sec2;
  size_t fracl;
  size_t tzl;
  અચલ અક્ષર *sep = "";

  /* Convert an ASN.1 Generalized time to a printable string.
     Return the dynamically allocated string, or NULL if an error occurs. */

  માટે(fracp = beg; fracp < end && ISDIGIT(*fracp); fracp++)
    ;

  /* Get seconds digits. */
  sec1 = '0';
  પસંદગી(fracp - beg - 12) {
  સ્થિતિ 0:
    sec2 = '0';
    તોડો;
  સ્થિતિ 2:
    sec1 = fracp[-2];
    FALLTHROUGH();
  સ્થિતિ 1:
    sec2 = fracp[-1];
    તોડો;
  મૂળભૂત:
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* timezone follows optional fractional seconds. */
  tzp = fracp;
  fracl = 0; /* no fractional seconds detected so far */
  જો(fracp < end && (*fracp == '.' || *fracp == ',')) {
    /* Have fractional seconds, e.g. "[.,]\d+". How many? */
    fracp++; /* should be a digit char or BAD ARGUMENT */
    tzp = fracp;
    જ્યારે(tzp < end && ISDIGIT(*tzp))
      tzp++;
    જો(tzp == fracp) /* never looped, no digit after [.,] */
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    fracl = tzp - fracp; /* number of fractional sec digits */
    DEBUGASSERT(fracl > 0);
    /* Strip trailing zeroes in fractional seconds.
     * May reduce fracl to 0 if only '0's are present. */
    જ્યારે(fracl && fracp[fracl - 1] == '0')
      fracl--;
  }

  /* Process timezone. */
  જો(tzp >= end) {
    tzp = "";
    tzl = 0;
  }
  નહીંતર જો(*tzp == 'Z') {
    sep = " ";
    tzp = "GMT";
    tzl = 3;
  }
  નહીંતર જો((*tzp == '+') || (*tzp == '-')) {
    sep = " UTC";
    tzl = end - tzp;
  }
  નહીંતર {
    sep = " ";
    tzl = end - tzp;
  }

  પાછા curlx_dyn_addf(store,
                        "%.4s-%.2s-%.2s %.2s:%.2s:%c%c%s%.*s%s%.*s",
                        beg, beg + 4, beg + 6,
                        beg + 8, beg + 10, sec1, sec2,
                        fracl ? "." : "", (પૂર્ણાંક)fracl, fracp,
                        sep, (પૂર્ણાંક)tzl, tzp);
}

/*
 * Convert an ASN.1 UTC time to a printable string.
 *
 * Return error code.
 */
સ્થિર CURLcode UTime2str(સંરચના dynbuf *store,
                          અચલ અક્ષર *beg, અચલ અક્ષર *end)
{
  અચલ અક્ષર *tzp;
  size_t tzl;
  અચલ અક્ષર *sec;

  માટે(tzp = beg; tzp < end && *tzp >= '0' && *tzp <= '9'; tzp++)
    ;
  /* Get the seconds. */
  sec = beg + 10;
  પસંદગી(tzp - sec) {
  સ્થિતિ 0:
    sec = "00";
    FALLTHROUGH();
  સ્થિતિ 2:
    તોડો;
  મૂળભૂત:
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Process timezone. */
  જો(tzp >= end)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  જો(*tzp == 'Z') {
    tzp = "GMT";
    end = tzp + 3;
  }

  tzl = end - tzp;
  પાછા curlx_dyn_addf(store, "%d%.2s-%.2s-%.2s %.2s:%.2s:%.2s %.*s",
                        20 - (*beg >= '5'), beg, beg + 2, beg + 4,
                        beg + 6, beg + 8, sec,
                        (પૂર્ણાંક)tzl, tzp);
}

/*
 * Convert an ASN.1 element to a printable string.
 *
 * Return error
 *
 * @unittest 1667
 */
UNITTEST CURLcode ASN1tostr(સંરચના dynbuf *store,
                            સંરચના Curl_asn1Element *elem);
UNITTEST CURLcode ASN1tostr(સંરચના dynbuf *store,
                            સંરચના Curl_asn1Element *elem)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  પૂર્ણાંક type = elem->tag;
  જો(elem->constructed)
    પાછા result; /* No conversion of structured elements. */

  પસંદગી(type) {
  સ્થિતિ CURL_ASN1_BOOLEAN:
    result = bool2str(store, elem->beg, elem->end);
    તોડો;
  સ્થિતિ CURL_ASN1_INTEGER:
  સ્થિતિ CURL_ASN1_ENUMERATED:
    result = int2str(store, elem->beg, elem->end);
    તોડો;
  સ્થિતિ CURL_ASN1_BIT_STRING:
    result = bit2str(store, elem->beg, elem->end);
    તોડો;
  સ્થિતિ CURL_ASN1_OCTET_STRING:
    result = octet2str(store, elem->beg, elem->end);
    તોડો;
  સ્થિતિ CURL_ASN1_NULL:
    result = curlx_dyn_addn(store, "", 1);
    તોડો;
  સ્થિતિ CURL_ASN1_OBJECT_IDENTIFIER:
    result = OID2str(store, elem->beg, elem->end);
    તોડો;
  સ્થિતિ CURL_ASN1_UTC_TIME:
    result = UTime2str(store, (અચલ અક્ષર *)elem->beg,
                       (અચલ અક્ષર *)elem->end);
    તોડો;
  સ્થિતિ CURL_ASN1_GENERALIZED_TIME:
    result = GTime2str(store, (અચલ અક્ષર *)elem->beg,
                       (અચલ અક્ષર *)elem->end);
    તોડો;
  સ્થિતિ CURL_ASN1_UTF8_STRING:
  સ્થિતિ CURL_ASN1_NUMERIC_STRING:
  સ્થિતિ CURL_ASN1_PRINTABLE_STRING:
  સ્થિતિ CURL_ASN1_TELETEX_STRING:
  સ્થિતિ CURL_ASN1_IA5_STRING:
  સ્થિતિ CURL_ASN1_VISIBLE_STRING:
  સ્થિતિ CURL_ASN1_UNIVERSAL_STRING:
  સ્થિતિ CURL_ASN1_BMP_STRING:
    result = utf8asn1str(store, type, elem->beg, elem->end);
    તોડો;
  }

  પાછા result;
}

/*
 * ASCII encode distinguished name at `dn' into the store dynbuf.
 *
 * Returns error.
 */
સ્થિર CURLcode encodeDN(સંરચના dynbuf *store, સંરચના Curl_asn1Element *dn)
{
  સંરચના Curl_asn1Element rdn;
  સંરચના Curl_asn1Element atv;
  સંરચના Curl_asn1Element oid;
  સંરચના Curl_asn1Element કિંમત;
  અચલ uint8_t *p1;
  અચલ uint8_t *p2;
  અચલ uint8_t *p3;
  અચલ uint8_t *str;
  CURLcode result = CURLE_OK;
  bool added = FALSE;
  સંરચના dynbuf temp;
  curlx_dyn_init(&temp, CURL_X509_STR_MAX);

  માટે(p1 = dn->beg; p1 < dn->end;) {
    p1 = getASN1Element(&rdn, p1, dn->end);
    જો(!p1) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      જાઓ error;
    }
    માટે(p2 = rdn.beg; p2 < rdn.end;) {
      p2 = getASN1Element(&atv, p2, rdn.end);
      જો(!p2) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        જાઓ error;
      }
      p3 = getASN1Element(&oid, atv.beg, atv.end);
      જો(!p3) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        જાઓ error;
      }
      જો(!getASN1Element(&કિંમત, p3, atv.end)) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        જાઓ error;
      }
      curlx_dyn_reset(&temp);
      result = ASN1tostr(&temp, &oid);
      જો(result)
        જાઓ error;

      str = curlx_dyn_uptr(&temp);

      જો(!str) {
        result = CURLE_BAD_FUNCTION_ARGUMENT;
        જાઓ error;
      }

      /* Encode delimiter.
         If attribute has a short uppercase name, delimiter is ", ". */
      માટે(p3 = str; ISUPPER(*p3); p3++)
        ;
      જો(added) {
        જો(p3 - str > 2)
          result = curlx_dyn_addn(store, "/", 1);
        નહીંતર
          result = curlx_dyn_addn(store, ", ", 2);
        જો(result)
          જાઓ error;
      }

      /* Encode attribute name. */
      result = curlx_dyn_add(store, (અચલ અક્ષર *)str);
      જો(result)
        જાઓ error;

      /* Generate equal sign. */
      result = curlx_dyn_addn(store, "=", 1);
      જો(result)
        જાઓ error;

      /* Generate value. */
      result = ASN1tostr(store, &કિંમત);
      જો(result)
        જાઓ error;
      curlx_dyn_reset(&temp);
      added = TRUE; /* use separator for next */
    }
  }
error:
  curlx_dyn_free(&temp);

  પાછા result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#ifdef WANT_PARSEX509
/*
 * ASN.1 parse an X509 certificate into structure subfields.
 * Syntax is assumed to have already been checked by the SSL backend.
 * See RFC 5280.
 */
પૂર્ણાંક Curl_parseX509(સંરચના Curl_X509certificate *cert,
                   અચલ uint8_t *beg, અચલ uint8_t *end)
{
  સંરચના Curl_asn1Element elem;
  સંરચના Curl_asn1Element tbsCertificate;
  અચલ uint8_t *ccp;
  સ્થિર અચલ uint8_t defaultVersion = 0;  /* v1. */

  memset(cert, 0, કદ(*cert));
  cert->certificate.header = ખાલી;
  cert->certificate.beg = beg;
  cert->certificate.end = end;

  /* Get the sequence content. */
  જો(!getASN1Element(&elem, beg, end))
    પાછા -1;  /* Invalid bounds/size. */
  beg = elem.beg;
  end = elem.end;

  /* Get tbsCertificate. */
  beg = getASN1Element(&tbsCertificate, beg, end);
  જો(!beg)
    પાછા -1;
  /* Skip the signatureAlgorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  જો(!beg)
    પાછા -1;
  /* Get the signatureValue. */
  જો(!getASN1Element(&cert->signature, beg, end))
    પાછા -1;

  /* Parse TBSCertificate. */
  beg = tbsCertificate.beg;
  end = tbsCertificate.end;
  /* Get optional version, get serialNumber. */
  cert->version.header = ખાલી;
  cert->version.beg = &defaultVersion;
  cert->version.end = &defaultVersion + કદ(defaultVersion);
  beg = getASN1Element(&elem, beg, end);
  જો(!beg)
    પાછા -1;
  જો(elem.tag == 0) {
    જો(!getASN1Element(&cert->version, elem.beg, elem.end))
      પાછા -1;
    beg = getASN1Element(&elem, beg, end);
    જો(!beg)
      પાછા -1;
  }
  cert->serial_સંખ્યા = elem;
  /* Get signature algorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, end);
  /* Get issuer. */
  beg = getASN1Element(&cert->issuer, beg, end);
  જો(!beg)
    પાછા -1;
  /* Get notBefore and notAfter. */
  beg = getASN1Element(&elem, beg, end);
  જો(!beg)
    પાછા -1;
  ccp = getASN1Element(&cert->notBefore, elem.beg, elem.end);
  જો(!ccp)
    પાછા -1;
  જો(!getASN1Element(&cert->notAfter, ccp, elem.end))
    પાછા -1;
  /* These two fields MUST consume all of the validity elem */
  જો(elem.end != cert->notAfter.end)
    પાછા -1;
  /* Get subject. */
  beg = getASN1Element(&cert->subject, beg, end);
  જો(!beg)
    પાછા -1;
  /* Get subjectPublicKeyAlgorithm and subjectPublicKey. */
  beg = getASN1Element(&cert->subject_Public_ચાવી_Info, beg, end);
  જો(!beg)
    પાછા -1;
  ccp = getASN1Element(&cert->subject_Public_ચાવી_Algorithm,
                       cert->subject_Public_ચાવી_Info.beg,
                       cert->subject_Public_ચાવી_Info.end);
  જો(!ccp)
    પાછા -1;
  જો(!getASN1Element(&cert->subject_Public_ચાવી, ccp,
                     cert->subject_Public_ચાવી_Info.end))
    પાછા -1;
  /* Get optional issuerUniqueID, subjectUniqueID and extensions. */
  cert->issuerUniqueID.tag = cert->subjectUniqueID.tag = 0;
  cert->extensions.tag = elem.tag = 0;
  cert->issuerUniqueID.header = cert->subjectUniqueID.header = ખાલી;
  cert->issuerUniqueID.beg = cert->issuerUniqueID.end = beg;
  cert->subjectUniqueID.beg = cert->subjectUniqueID.end = beg;
  cert->extensions.header = ખાલી;
  cert->extensions.beg = cert->extensions.end = beg;
  જો(beg < end) {
    beg = getASN1Element(&elem, beg, end);
    જો(!beg)
      પાછા -1;
  }
  જો(elem.tag == 1) {
    cert->issuerUniqueID = elem;
    જો(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      જો(!beg)
        પાછા -1;
    }
  }
  જો(elem.tag == 2) {
    cert->subjectUniqueID = elem;
    જો(beg < end) {
      beg = getASN1Element(&elem, beg, end);
      જો(!beg)
        પાછા -1;
    }
  }
  જો(elem.tag == 3)
    જો(!getASN1Element(&cert->extensions, elem.beg, elem.end))
      પાછા -1;
  પાછા 0;
}

#endif /* WANT_PARSEX509 */

#ifdef WANT_EXTRACT_CERTINFO

સ્થિર CURLcode dumpAlgo(સંરચના dynbuf *store,
                         સંરચના Curl_asn1Element *param,
                         અચલ uint8_t *beg, અચલ uint8_t *end)
{
  સંરચના Curl_asn1Element oid;

  /* Get algorithm parameters and return algorithm name. */

  beg = getASN1Element(&oid, beg, end);
  જો(!beg)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  param->header = ખાલી;
  param->tag = 0;
  param->beg = param->end = end;
  જો(beg < end) {
    અચલ uint8_t *p = getASN1Element(param, beg, end);
    જો(!p)
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }
  પાછા OID2str(store, oid.beg, oid.end);
}

/*
 * This is a convenience function for push_certinfo_len that takes a zero
 * terminated value.
 */
સ્થિર CURLcode ssl_ધકેલો_certinfo(સંરચના Curl_easy *data,
                                  પૂર્ણાંક certnum,
                                  અચલ અક્ષર *label,
                                  અચલ અક્ષર *કિંમત)
{
  size_t valuelen = strlen(કિંમત);

  પાછા Curl_ssl_ધકેલો_certinfo_len(data, certnum, label, કિંમત, valuelen);
}

/*
 * This is a convenience function for push_certinfo_len that takes a
 * dynbuf value.
 *
 * It also does the verbose output if !certnum.
 */
સ્થિર CURLcode ssl_ધકેલો_certinfo_dyn(સંરચના Curl_easy *data,
                                      પૂર્ણાંક certnum,
                                      અચલ અક્ષર *label,
                                      સંરચના dynbuf *ptr)
{
  size_t valuelen = curlx_dyn_len(ptr);
  અચલ અક્ષર *કિંમત = curlx_dyn_ptr(ptr);

  CURLcode result = Curl_ssl_ધકેલો_certinfo_len(data, certnum, label,
                                               કિંમત, valuelen);

  જો(!certnum && !result)
    infof(data, "   %s: %s", label, કિંમત);

  પાછા result;
}

સ્થિર CURLcode do_pubkey_field(સંરચના Curl_easy *data, પૂર્ણાંક certnum,
                                અચલ અક્ષર *label,
                                સંરચના Curl_asn1Element *elem)
{
  CURLcode result;
  સંરચના dynbuf out;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);

  /* Generate a certificate information record for the public key. */

  result = ASN1tostr(&out, elem);
  જો(!result && data->set.ssl.certinfo)
    result = ssl_ધકેલો_certinfo_dyn(data, certnum, label, &out);
  curlx_dyn_free(&out);
  પાછા result;
}

/* return 0 on success, 1 on error */
સ્થિર પૂર્ણાંક do_pubkey(સંરચના Curl_easy *data, પૂર્ણાંક certnum, અચલ અક્ષર *algo,
                     સંરચના Curl_asn1Element *param,
                     સંરચના Curl_asn1Element *pubkey)
{
  સંરચના Curl_asn1Element elem;
  સંરચના Curl_asn1Element pk;
  અચલ uint8_t *p;

  /* Generate all information records for the public key. */

  જો(curl_strequal(algo, "ecPublicKey")) {
    /*
     * ECC public key is all the data, a value of type BIT STRING mapped to
     * OCTET STRING and should not be parsed as an ASN.1 value.
     */
    અચલ size_t dlen = pubkey->end - pubkey->beg;
    size_t len;
    જો(dlen < 2)
      /* too small */
      પાછા 1;
    len = (dlen - 2) * 4;
    જો(!certnum)
      infof(data, "   ECC Public Key (%zu bits)", len);
    જો(data->set.ssl.certinfo) {
      અક્ષર q[(કદ(len) * 8 / 3) + 1];
      (શૂન્ય)curl_msnprintf(q, કદ(q), "%zu", len);
      જો(ssl_ધકેલો_certinfo(data, certnum, "ECC Public Key", q))
        પાછા 1;
    }
    પાછા do_pubkey_field(data, certnum, "ecPublicKey", pubkey) == CURLE_OK
      ? 0 : 1;
  }

  /* Get the public key (single element). */
  જો(!getASN1Element(&pk, pubkey->beg + 1, pubkey->end))
    પાછા 1;

  જો(curl_strequal(algo, "rsaEncryption")) {
    અચલ uint8_t *q;
    size_t len;

    p = getASN1Element(&elem, pk.beg, pk.end);
    જો(!p)
      પાછા 1;

    /* Compute key length. */
    માટે(q = elem.beg; q < elem.end && !*q; q++)
      ;
    len = ((elem.end - q) * 8);
    જો(len) {
      unsigned પૂર્ણાંક i;
      માટે(i = *q; !(i & 0x80); i <<= 1)
        len--;
    }
    જો(len > 32)
      elem.beg = q;     /* Strip leading zero bytes. */
    જો(!certnum)
      infof(data, "   RSA Public Key (%zu bits)", len);
    જો(data->set.ssl.certinfo) {
      અક્ષર r[(કદ(len) * 8 / 3) + 1];
      curl_msnprintf(r, કદ(r), "%zu", len);
      જો(ssl_ધકેલો_certinfo(data, certnum, "RSA Public Key", r))
        પાછા 1;
    }
    /* Generate coefficients. */
    જો(do_pubkey_field(data, certnum, "rsa(n)", &elem))
      પાછા 1;
    જો(!getASN1Element(&elem, p, pk.end))
      પાછા 1;
    જો(do_pubkey_field(data, certnum, "rsa(e)", &elem))
      પાછા 1;
  }
  નહીંતર જો(curl_strequal(algo, "dsa")) {
    p = getASN1Element(&elem, param->beg, param->end);
    જો(p) {
      જો(do_pubkey_field(data, certnum, "dsa(p)", &elem))
        પાછા 1;
      p = getASN1Element(&elem, p, param->end);
      જો(p) {
        જો(do_pubkey_field(data, certnum, "dsa(q)", &elem))
          પાછા 1;
        જો(getASN1Element(&elem, p, param->end)) {
          જો(do_pubkey_field(data, certnum, "dsa(g)", &elem))
            પાછા 1;
          જો(do_pubkey_field(data, certnum, "dsa(pub_key)", &pk))
            પાછા 1;
        }
      }
    }
  }
  નહીંતર જો(curl_strequal(algo, "dhpublicnumber")) {
    p = getASN1Element(&elem, param->beg, param->end);
    જો(p) {
      જો(do_pubkey_field(data, certnum, "dh(p)", &elem))
        પાછા 1;
      જો(getASN1Element(&elem, p, param->end)) {
        જો(do_pubkey_field(data, certnum, "dh(g)", &elem))
          પાછા 1;
        જો(do_pubkey_field(data, certnum, "dh(pub_key)", &pk))
          પાછા 1;
      }
    }
  }
  પાછા 0;
}

/*
 * Convert an ASN.1 distinguished name into a printable string.
 * Return error.
 */
સ્થિર CURLcode DNtostr(સંરચના dynbuf *store, સંરચના Curl_asn1Element *dn)
{
  પાછા encodeDN(store, dn);
}

CURLcode Curl_extract_certinfo(સંરચના Curl_easy *data,
                               પૂર્ણાંક certnum,
                               અચલ uint8_t *beg,
                               અચલ uint8_t *end)
{
  સંરચના Curl_X509certificate cert;
  સંરચના Curl_asn1Element param;
  અક્ષર *certptr;
  size_t clen;
  સંરચના dynbuf out;
  CURLcode result = CURLE_OK;
  unsigned પૂર્ણાંક version;
  અચલ uint8_t *ptr;
  પૂર્ણાંક rc;

  જો(!data->set.ssl.certinfo)
    જો(certnum)
      પાછા CURLE_OK;

  curlx_dyn_init(&out, CURL_X509_STR_MAX);
  /* Prepare the certificate information for curl_easy_getinfo(). */

  /* Extract the certificate ASN.1 elements. */
  જો(Curl_parseX509(&cert, beg, end))
    પાછા CURLE_PEER_FAILED_VERIFICATION;

  /* Subject. */
  result = DNtostr(&out, &cert.subject);
  જો(result)
    જાઓ done;
  જો(data->set.ssl.certinfo) {
    result = ssl_ધકેલો_certinfo_dyn(data, certnum, "Subject", &out);
    જો(result)
      જાઓ done;
  }
  curlx_dyn_reset(&out);

  /* Issuer. */
  result = DNtostr(&out, &cert.issuer);
  જો(result)
    જાઓ done;
  જો(data->set.ssl.certinfo) {
    result = ssl_ધકેલો_certinfo_dyn(data, certnum, "Issuer", &out);
    જો(result)
      જાઓ done;
  }
  curlx_dyn_reset(&out);

  /* Version (always fits in less than 32 bits). */
  version = 0;
  માટે(ptr = cert.version.beg; ptr < cert.version.end; ptr++)
    version = (version << 8) | *ptr;
  જો(data->set.ssl.certinfo) {
    result = curlx_dyn_addf(&out, "%x", version);
    જો(result)
      જાઓ done;
    result = ssl_ધકેલો_certinfo_dyn(data, certnum, "Version", &out);
    જો(result)
      જાઓ done;
    curlx_dyn_reset(&out);
  }

  /* Serial number. */
  result = ASN1tostr(&out, &cert.serial_સંખ્યા);
  જો(result)
    જાઓ done;
  જો(data->set.ssl.certinfo) {
    result = ssl_ધકેલો_certinfo_dyn(data, certnum, "Serial Number", &out);
    જો(result)
      જાઓ done;
  }
  curlx_dyn_reset(&out);

  /* Signature algorithm .*/
  result = dumpAlgo(&out, &param, cert.signatureAlgorithm.beg,
                    cert.signatureAlgorithm.end);
  જો(result)
    જાઓ done;
  જો(data->set.ssl.certinfo) {
    result = ssl_ધકેલો_certinfo_dyn(data, certnum, "Signature Algorithm", &out);
    જો(result)
      જાઓ done;
  }
  curlx_dyn_reset(&out);

  /* Start Date. */
  result = ASN1tostr(&out, &cert.notBefore);
  જો(result)
    જાઓ done;
  જો(data->set.ssl.certinfo) {
    result = ssl_ધકેલો_certinfo_dyn(data, certnum, "Start Date", &out);
    જો(result)
      જાઓ done;
  }
  curlx_dyn_reset(&out);

  /* Expire Date. */
  result = ASN1tostr(&out, &cert.notAfter);
  જો(result)
    જાઓ done;
  જો(data->set.ssl.certinfo) {
    result = ssl_ધકેલો_certinfo_dyn(data, certnum, "Expire Date", &out);
    જો(result)
      જાઓ done;
  }
  curlx_dyn_reset(&out);

  /* Public Key Algorithm. */
  result = dumpAlgo(&out, &param, cert.subject_Public_ચાવી_Algorithm.beg,
                    cert.subject_Public_ચાવી_Algorithm.end);
  જો(result)
    જાઓ done;
  જો(data->set.ssl.certinfo) {
    result = ssl_ધકેલો_certinfo_dyn(data, certnum, "Public Key Algorithm",
                                   &out);
    જો(result)
      જાઓ done;
  }

  rc = do_pubkey(data, certnum, curlx_dyn_ptr(&out),
                 &param, &cert.subject_Public_ચાવી);
  જો(rc) {
    result = CURLE_OUT_OF_MEMORY; /* the most likely error */
    જાઓ done;
  }
  curlx_dyn_reset(&out);

  /* Signature. */
  result = ASN1tostr(&out, &cert.signature);
  જો(result)
    જાઓ done;
  જો(data->set.ssl.certinfo) {
    result = ssl_ધકેલો_certinfo_dyn(data, certnum, "Signature", &out);
    જો(result)
      જાઓ done;
  }
  curlx_dyn_reset(&out);

  /* Generate PEM certificate. */
  result = curlx_base64_encode((અચલ uint8_t *)cert.certificate.beg,
                               cert.certificate.end - cert.certificate.beg,
                               &certptr, &clen);
  જો(result)
    જાઓ done;

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
  જો(!result) {
    size_t j = 0;

    જ્યારે(!result && (j < clen)) {
      size_t chunksize = (clen - j) > 64 ? 64 : (clen - j);
      result = curlx_dyn_addn(&out, &certptr[j], chunksize);
      જો(!result)
        result = curlx_dyn_addn(&out, "\n", 1);
      j += chunksize;
    }
    જો(!result)
      result = curlx_dyn_add(&out, "-----END CERTIFICATE-----\n");
  }
  curlx_free(certptr);
  જો(!result)
    જો(data->set.ssl.certinfo)
      result = ssl_ધકેલો_certinfo_dyn(data, certnum, "Cert", &out);

done:
  જો(result)
    failf(data, "Failed extracting certificate chain");
  curlx_dyn_free(&out);
  પાછા result;
}

#endif /* WANT_EXTRACT_CERTINFO */

#endif /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
