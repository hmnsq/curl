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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#ఒకవేళ defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_RUSTLS)
#నిర్వచించు WANT_PARSEX509 /* uses Curl_parseX509() */
#అయితే_ముగింపు

#ఒకవేళ defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_RUSTLS)
#నిర్వచించు WANT_వెలికితీయి_CERTINFO /* uses Curl_extract_certinfo() */
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "curlx/base64.h"
#అన్తర్భావ "vtls/x509asn1.h"
#అన్తర్భావ "curlx/dynbuf.h"

/*
 * Constants.
 */

/* Largest supported ASN.1 structure. */
#నిర్వచించు CURL_ASN1_MAX                   ((size_t)0x40000)      /* 256K */

#నిర్వచితమైతే WANT_వెలికితీయి_CERTINFO

/* ASN.1 OID table entry. */
నిర్మాణం Curl_OID {
  స్థిరరాశి అక్షరం *numoid;  /* Dotted-numeric OID. */
  స్థిరరాశి అక్షరం *textoid; /* OID name. */
};

/* ASN.1 OIDs. */
స్థిర స్థిరరాశి నిర్మాణం Curl_OID OIDtable[] = {
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
  { ఖాళీ,                       ఖాళీ }
};

#అయితే_ముగింపు /* WANT_EXTRACT_CERTINFO */

/*
 * Lightweight ASN.1 parser.
 * In particular, it does not check for syntactic/lexical errors.
 * It is intended to support certificate information gathering for SSL backends
 * that offer a mean to get certificates as a whole, but do not supply
 * entry points to get particular certificate sub-fields.
 * Please note there is no pretension here to rewrite a full SSL library.
 */

#నిర్వచించు CURL_ASN1_MAX_RECURSIONS    16

స్థిర స్థిరరాశి uint8_t *getASN1Element_(నిర్మాణం Curl_asn1Element *elem,
                                      స్థిరరాశి uint8_t *beg, స్థిరరాశి uint8_t *చివర,
                                      size_t lvl)
{
  uint8_t b;
  size_t len;
  నిర్మాణం Curl_asn1Element lelem;

  /* Get a single ASN.1 element into `elem', parse ASN.1 string at `beg'
     ending at `end'.
     Returns a pointer in source string after the parsed element, or NULL
     if an error occurs. */
  ఒకవేళ(!beg || !చివర || beg >= చివర || !*beg ||
     ((size_t)(చివర - beg) > CURL_ASN1_MAX) ||
     lvl++ >= CURL_ASN1_MAX_RECURSIONS)
    తిరిగి ఖాళీ;

  /* Process header byte. */
  elem->శీర్షిక = beg;
  b = *beg++;
  elem->constructed = (b & 0x20) != 0;
  elem->eclass = (b >> 6) & 3;
  b &= 0x1F;
  ఒకవేళ(b == 0x1F)
    తిరిగి ఖాళీ; /* Long tag values not supported here. */
  elem->ట్యాగ్ = b;

  /* Process length. */
  ఒకవేళ(beg >= చివర)
    తిరిగి ఖాళీ;
  b = *beg++;
  ఒకవేళ(!(b & 0x80))
    len = b;
  లేకపోతే {
    b &= 0x7F;
    ఒకవేళ(!b) {
      /* Unspecified length. Since we have all the data, we can determine the
         effective length by skipping element until an EOC element is found. */
      ఒకవేళ(!elem->constructed)
        తిరిగి ఖాళీ;
      elem->beg = beg;
      అయితే(beg < చివర && *beg) {
        beg = getASN1Element_(&lelem, beg, చివర, lvl);
        ఒకవేళ(!beg)
          తిరిగి ఖాళీ;
      }
      ఒకవేళ(చివర - beg < 2 || beg[1])       /* EOC must have a zero length. */
        తిరిగి ఖాళీ;
      elem->చివర = beg;                  /* Do not include EOC as content. */
      తిరిగి beg + 2;                   /* After EOC element. */
    }
    లేకపోతే ఒకవేళ(b > (size_t)(చివర - beg))
      తిరిగి ఖాళీ; /* Does not fit in source. */
    లేకపోతే {
      /* Get long length. */
      len = 0;
      చేయి {
        ఒకవేళ(len & 0xFF000000L)
          తిరిగి ఖాళీ;  /* Lengths > 32 bits are not supported. */
        len = (len << 8) | *beg++;
      } అయితే(--b);
    }
  }
  ఒకవేళ(len > (size_t)(చివర - beg))
    తిరిగి ఖాళీ;  /* Element data does not fit in source. */
  elem->beg = beg;
  elem->చివర = beg + len;
  తిరిగి elem->చివర;
}

/*
 * @unittest 1657
 */
UNITTEST స్థిరరాశి uint8_t *getASN1Element(నిర్మాణం Curl_asn1Element *elem,
                                       స్థిరరాశి uint8_t *beg, స్థిరరాశి uint8_t *చివర);
UNITTEST స్థిరరాశి uint8_t *getASN1Element(నిర్మాణం Curl_asn1Element *elem,
                                       స్థిరరాశి uint8_t *beg, స్థిరరాశి uint8_t *చివర)
{
  తిరిగి getASN1Element_(elem, beg, చివర, 0);
}

#నిర్వచితమైతే WANT_వెలికితీయి_CERTINFO

/*
 * Search the null-terminated OID or OID identifier in local table.
 * Return the table entry pointer or NULL if not found.
 */
స్థిర స్థిరరాశి నిర్మాణం Curl_OID *శోధన_OID(స్థిరరాశి అక్షరం *oid)
{
  స్థిరరాశి నిర్మాణం Curl_OID *op;
  కొరకు(op = OIDtable; op->numoid; op++)
    ఒకవేళ(!strcmp(op->numoid, oid) || curl_strequal(op->textoid, oid))
      తిరిగి op;

  తిరిగి ఖాళీ;
}

/*
 * Convert an ASN.1 Boolean value into its string representation.
 *
 * Return error code.
 */

స్థిర CURLcode bool2str(నిర్మాణం dynbuf *store,
                         స్థిరరాశి uint8_t *beg, స్థిరరాశి uint8_t *చివర)
{
  uint8_t val;
  ఒకవేళ(చివర - beg != 1)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  val = *beg;
  ఒకవేళ((val != 0xff) && (val != 0x00))
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  తిరిగి curlx_dyn_కూడు(store, val ? "TRUE" : "FALSE");
}

/*
 * Convert an ASN.1 octet string to a printable string.
 *
 * Return error code.
 */
స్థిర CURLcode octet2str(నిర్మాణం dynbuf *store,
                          స్థిరరాశి uint8_t *beg, స్థిరరాశి uint8_t *చివర)
{
  CURLcode ఫలితం = CURLE_OK;

  అయితే(!ఫలితం && beg < చివర)
    ఫలితం = curlx_dyn_addf(store, "%02x:", *beg++);

  తిరిగి ఫలితం;
}

స్థిర CURLcode bit2str(నిర్మాణం dynbuf *store,
                        స్థిరరాశి uint8_t *beg, స్థిరరాశి uint8_t *చివర)
{
  /* Convert an ASN.1 bit string to a printable string. */

  ఒకవేళ(beg < చివర) {
    uint8_t వాడని_బిట్లు = *beg++;
    ఒకవేళ(వాడని_బిట్లు > 7)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }
  లేకపోతే
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  తిరిగి octet2str(store, beg, చివర);
}

/*
 * Convert an ASN.1 integer value into its string representation.
 *
 * Returns error.
 */
స్థిర CURLcode int2str(నిర్మాణం dynbuf *store,
                        స్థిరరాశి uint8_t *beg, స్థిరరాశి uint8_t *చివర)
{
  uint32_t uval = 0;
  int32_t sval = 0;
  size_t n = చివర - beg;
  size_t i;
  స్థిరరాశి uint8_t *p = beg;

  ఒకవేళ(!n)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  ఒకవేళ(n > 4)
    తిరిగి octet2str(store, beg, చివర);

  ఒకవేళ(p[0] & 0x80)
    uval = ~0U;

  కొరకు(i = 0; i < n; i++)
    uval = (uval << 8) | p[i];

  sval = (int32_t)uval;

  /* small values are easier to read as decimals */
  ఒకవేళ(sval > -10000 && sval < 10000)
    తిరిగి curlx_dyn_addf(store, "%d", sval);

  /* represent larger values as hex */
  తిరిగి curlx_dyn_addf(store, "0x%x", uval);
}

/*
 * Convert from an ASN.1 typed string to UTF8.
 *
 * The result is stored in a dynbuf that is inited by the user of this
 * function.
 *
 * Returns error.
 */
స్థిర CURLcode utf8asn1str(నిర్మాణం dynbuf *గమ్యం, పూర్ణాంకం type,
                            స్థిరరాశి uint8_t *from, స్థిరరాశి uint8_t *చివర)
{
  size_t inlength = చివర - from;
  పూర్ణాంకం కొలత = 1;
  CURLcode ఫలితం = CURLE_OK;

  ఎంపిక(type) {
  స్థితి CURL_ASN1_BMP_STRING:
    కొలత = 2;
    ఆపు;
  స్థితి CURL_ASN1_UNIVERSAL_STRING:
    కొలత = 4;
    ఆపు;
  స్థితి CURL_ASN1_NUMERIC_STRING:
  స్థితి CURL_ASN1_PRINTABLE_STRING:
  స్థితి CURL_ASN1_TELETEX_STRING:
  స్థితి CURL_ASN1_IA5_STRING:
  స్థితి CURL_ASN1_VISIBLE_STRING:
  స్థితి CURL_ASN1_UTF8_STRING:
    ఆపు;
  అప్రమేయ:
    తిరిగి CURLE_BAD_FUNCTION_వాదం;  /* Conversion not supported. */
  }

  ఒకవేళ(inlength % కొలత)
    /* Length inconsistent with character size. */
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  ఒకవేళ(type == CURL_ASN1_UTF8_STRING) {
    /* copy. */
    ఒకవేళ(inlength)
      ఫలితం = curlx_dyn_addn(గమ్యం, from, inlength);
  }
  లేకపోతే {
    అయితే(!ఫలితం && (from < చివర)) {
      అక్షరం buf[4]; /* decode buffer */
      size_t charsize = 1;
      గుర్తులేని పూర్ణాంకం wc = 0;

      ఎంపిక(కొలత) {
      స్థితి 4:
        wc = (wc << 8) | *from++;
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      స్థితి 2:
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      అప్రమేయ: /* case 1: */
        wc = (wc << 8) | *from++;
      }
      ఒకవేళ(wc >= 0x00000080) {
        ఒకవేళ(wc >= 0x00000800) {
          ఒకవేళ(wc >= 0x00010000) {
            ఒకవేళ(wc >= 0x00200000) {
              /* Invalid char. size for target encoding. */
              తిరిగి CURLE_WEIRD_SERVER_జవాబు;
            }
            buf[3] = (అక్షరం)(0x80 | (wc & 0x3F));
            wc = (wc >> 6) | 0x00010000;
            charsize++;
          }
          buf[2] = (అక్షరం)(0x80 | (wc & 0x3F));
          wc = (wc >> 6) | 0x00000800;
          charsize++;
        }
        buf[1] = (అక్షరం)(0x80 | (wc & 0x3F));
        wc = (wc >> 6) | 0x000000C0;
        charsize++;
      }
      buf[0] = (అక్షరం)wc;
      ఫలితం = curlx_dyn_addn(గమ్యం, buf, charsize);
    }
  }
  తిరిగి ఫలితం;
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
UNITTEST CURLcode encodeOID(నిర్మాణం dynbuf *store,
                            స్థిరరాశి uint8_t *beg, స్థిరరాశి uint8_t *చివర);
UNITTEST CURLcode encodeOID(నిర్మాణం dynbuf *store,
                            స్థిరరాశి uint8_t *beg, స్థిరరాశి uint8_t *చివర)
{
  uint32_t x;
  uint32_t y;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(చివర <= beg)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  /* Process the first two numbers. The initial digit cannot be larger than
     2 */
  y = *beg++;
  ఒకవేళ(y <= 80)
    x = y / 40;
  లేకపోతే
    x = 2;
  ఫలితం = curlx_dyn_addf(store, "%u", x);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఒకవేళ(y & 0x80) {
    uint32_t t = (y & 0x7f); /* the second number */
    ఒకవేళ(!t)
      /* reject leading 0x80 padding */
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    చేయి {
      ఒకవేళ((t & 0xFE000000) || (beg >= చివర))
        తిరిగి CURLE_BAD_FUNCTION_వాదం;
      y = *beg++;
      t = (t << 7) | (y & 0x7F);
    } అయితే(y & 0x80);
    y = t;
  }

  ఫలితం = curlx_dyn_addf(store, ".%u", y - (x * 40));
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* Process the trailing numbers. */
  అయితే((beg < చివర) && !ఫలితం) {
    x = 0;
    y = 0;
    చేయి {
      /* 0x80 as the first value is invalid since it only adds padding */
      ఒకవేళ((y & 0x80) && !x)
        తిరిగి CURLE_BAD_FUNCTION_వాదం;
      లేకపోతే ఒకవేళ((x & 0xFE000000) || (beg == చివర))
        తిరిగి CURLE_BAD_FUNCTION_వాదం;
      y = *beg++;
      x = (x << 7) | (y & 0x7F);
    } అయితే(y & 0x80);
    ఫలితం = curlx_dyn_addf(store, ".%u", x);
  }
  తిరిగి ఫలితం;
}

/*
 * Convert an ASN.1 OID into its dotted or symbolic string representation.
 *
 * Return error code.
 */

స్థిర CURLcode OID2str(నిర్మాణం dynbuf *store,
                        స్థిరరాశి uint8_t *beg, స్థిరరాశి uint8_t *చివర)
{
  CURLcode ఫలితం = CURLE_OK;
  ఒకవేళ(beg < చివర) {
    నిర్మాణం dynbuf buf;
    curlx_dyn_init(&buf, CURL_X509_STR_MAX);
    ఫలితం = encodeOID(&buf, beg, చివర);

    ఒకవేళ(!ఫలితం) {
      స్థిరరాశి నిర్మాణం Curl_OID *op = శోధన_OID(curlx_dyn_ptr(&buf));
      ఒకవేళ(op)
        ఫలితం = curlx_dyn_కూడు(store, op->textoid);
      లేకపోతే
        ఫలితం = curlx_dyn_కూడు(store, curlx_dyn_ptr(&buf));
    }
    curlx_dyn_free(&buf);
  }
  తిరిగి ఫలితం;
}

/*
 * @unittest 1656
 */
UNITTEST CURLcode GTime2str(నిర్మాణం dynbuf *store,
                            స్థిరరాశి అక్షరం *beg, స్థిరరాశి అక్షరం *చివర);
UNITTEST CURLcode GTime2str(నిర్మాణం dynbuf *store,
                            స్థిరరాశి అక్షరం *beg, స్థిరరాశి అక్షరం *చివర)
{
  స్థిరరాశి అక్షరం *tzp;
  స్థిరరాశి అక్షరం *fracp;
  అక్షరం sec1, sec2;
  size_t fracl;
  size_t tzl;
  స్థిరరాశి అక్షరం *sep = "";

  /* Convert an ASN.1 Generalized time to a printable string.
     Return the dynamically allocated string, or NULL if an error occurs. */

  కొరకు(fracp = beg; fracp < చివర && ISDIGIT(*fracp); fracp++)
    ;

  /* Get seconds digits. */
  sec1 = '0';
  ఎంపిక(fracp - beg - 12) {
  స్థితి 0:
    sec2 = '0';
    ఆపు;
  స్థితి 2:
    sec1 = fracp[-2];
    FALLTHROUGH();
  స్థితి 1:
    sec2 = fracp[-1];
    ఆపు;
  అప్రమేయ:
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }

  /* timezone follows optional fractional seconds. */
  tzp = fracp;
  fracl = 0; /* no fractional seconds detected so far */
  ఒకవేళ(fracp < చివర && (*fracp == '.' || *fracp == ',')) {
    /* Have fractional seconds, e.g. "[.,]\d+". How many? */
    fracp++; /* should be a digit char or BAD ARGUMENT */
    tzp = fracp;
    అయితే(tzp < చివర && ISDIGIT(*tzp))
      tzp++;
    ఒకవేళ(tzp == fracp) /* never looped, no digit after [.,] */
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    fracl = tzp - fracp; /* number of fractional sec digits */
    DEBUGASSERT(fracl > 0);
    /* Strip trailing zeroes in fractional seconds.
     * May reduce fracl to 0 if only '0's are present. */
    అయితే(fracl && fracp[fracl - 1] == '0')
      fracl--;
  }

  /* Process timezone. */
  ఒకవేళ(tzp >= చివర) {
    tzp = "";
    tzl = 0;
  }
  లేకపోతే ఒకవేళ(*tzp == 'Z') {
    sep = " ";
    tzp = "GMT";
    tzl = 3;
  }
  లేకపోతే ఒకవేళ((*tzp == '+') || (*tzp == '-')) {
    sep = " UTC";
    tzl = చివర - tzp;
  }
  లేకపోతే {
    sep = " ";
    tzl = చివర - tzp;
  }

  తిరిగి curlx_dyn_addf(store,
                        "%.4s-%.2s-%.2s %.2s:%.2s:%c%c%s%.*s%s%.*s",
                        beg, beg + 4, beg + 6,
                        beg + 8, beg + 10, sec1, sec2,
                        fracl ? "." : "", (పూర్ణాంకం)fracl, fracp,
                        sep, (పూర్ణాంకం)tzl, tzp);
}

/*
 * Convert an ASN.1 UTC time to a printable string.
 *
 * Return error code.
 */
స్థిర CURLcode UTime2str(నిర్మాణం dynbuf *store,
                          స్థిరరాశి అక్షరం *beg, స్థిరరాశి అక్షరం *చివర)
{
  స్థిరరాశి అక్షరం *tzp;
  size_t tzl;
  స్థిరరాశి అక్షరం *sec;

  కొరకు(tzp = beg; tzp < చివర && *tzp >= '0' && *tzp <= '9'; tzp++)
    ;
  /* Get the seconds. */
  sec = beg + 10;
  ఎంపిక(tzp - sec) {
  స్థితి 0:
    sec = "00";
    FALLTHROUGH();
  స్థితి 2:
    ఆపు;
  అప్రమేయ:
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }

  /* Process timezone. */
  ఒకవేళ(tzp >= చివర)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  ఒకవేళ(*tzp == 'Z') {
    tzp = "GMT";
    చివర = tzp + 3;
  }

  tzl = చివర - tzp;
  తిరిగి curlx_dyn_addf(store, "%d%.2s-%.2s-%.2s %.2s:%.2s:%.2s %.*s",
                        20 - (*beg >= '5'), beg, beg + 2, beg + 4,
                        beg + 6, beg + 8, sec,
                        (పూర్ణాంకం)tzl, tzp);
}

/*
 * Convert an ASN.1 element to a printable string.
 *
 * Return error
 *
 * @unittest 1667
 */
UNITTEST CURLcode ASN1tostr(నిర్మాణం dynbuf *store,
                            నిర్మాణం Curl_asn1Element *elem);
UNITTEST CURLcode ASN1tostr(నిర్మాణం dynbuf *store,
                            నిర్మాణం Curl_asn1Element *elem)
{
  CURLcode ఫలితం = CURLE_BAD_FUNCTION_వాదం;
  పూర్ణాంకం type = elem->ట్యాగ్;
  ఒకవేళ(elem->constructed)
    తిరిగి ఫలితం; /* No conversion of structured elements. */

  ఎంపిక(type) {
  స్థితి CURL_ASN1_BOOLEAN:
    ఫలితం = bool2str(store, elem->beg, elem->చివర);
    ఆపు;
  స్థితి CURL_ASN1_INTEGER:
  స్థితి CURL_ASN1_ENUMERATED:
    ఫలితం = int2str(store, elem->beg, elem->చివర);
    ఆపు;
  స్థితి CURL_ASN1_బిట్టు_STRING:
    ఫలితం = bit2str(store, elem->beg, elem->చివర);
    ఆపు;
  స్థితి CURL_ASN1_OCTET_STRING:
    ఫలితం = octet2str(store, elem->beg, elem->చివర);
    ఆపు;
  స్థితి CURL_ASN1_NULL:
    ఫలితం = curlx_dyn_addn(store, "", 1);
    ఆపు;
  స్థితి CURL_ASN1_OBJECT_IDENTIFIER:
    ఫలితం = OID2str(store, elem->beg, elem->చివర);
    ఆపు;
  స్థితి CURL_ASN1_UTC_సమయం:
    ఫలితం = UTime2str(store, (స్థిరరాశి అక్షరం *)elem->beg,
                       (స్థిరరాశి అక్షరం *)elem->చివర);
    ఆపు;
  స్థితి CURL_ASN1_GENERALIZED_సమయం:
    ఫలితం = GTime2str(store, (స్థిరరాశి అక్షరం *)elem->beg,
                       (స్థిరరాశి అక్షరం *)elem->చివర);
    ఆపు;
  స్థితి CURL_ASN1_UTF8_STRING:
  స్థితి CURL_ASN1_NUMERIC_STRING:
  స్థితి CURL_ASN1_PRINTABLE_STRING:
  స్థితి CURL_ASN1_TELETEX_STRING:
  స్థితి CURL_ASN1_IA5_STRING:
  స్థితి CURL_ASN1_VISIBLE_STRING:
  స్థితి CURL_ASN1_UNIVERSAL_STRING:
  స్థితి CURL_ASN1_BMP_STRING:
    ఫలితం = utf8asn1str(store, type, elem->beg, elem->చివర);
    ఆపు;
  }

  తిరిగి ఫలితం;
}

/*
 * ASCII encode distinguished name at `dn' into the store dynbuf.
 *
 * Returns error.
 */
స్థిర CURLcode encodeDN(నిర్మాణం dynbuf *store, నిర్మాణం Curl_asn1Element *dn)
{
  నిర్మాణం Curl_asn1Element rdn;
  నిర్మాణం Curl_asn1Element atv;
  నిర్మాణం Curl_asn1Element oid;
  నిర్మాణం Curl_asn1Element విలువ;
  స్థిరరాశి uint8_t *p1;
  స్థిరరాశి uint8_t *p2;
  స్థిరరాశి uint8_t *p3;
  స్థిరరాశి uint8_t *str;
  CURLcode ఫలితం = CURLE_OK;
  bool added = FALSE;
  నిర్మాణం dynbuf తాత్కాలిక;
  curlx_dyn_init(&తాత్కాలిక, CURL_X509_STR_MAX);

  కొరకు(p1 = dn->beg; p1 < dn->చివర;) {
    p1 = getASN1Element(&rdn, p1, dn->చివర);
    ఒకవేళ(!p1) {
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
      వెళ్ళు error;
    }
    కొరకు(p2 = rdn.beg; p2 < rdn.చివర;) {
      p2 = getASN1Element(&atv, p2, rdn.చివర);
      ఒకవేళ(!p2) {
        ఫలితం = CURLE_BAD_FUNCTION_వాదం;
        వెళ్ళు error;
      }
      p3 = getASN1Element(&oid, atv.beg, atv.చివర);
      ఒకవేళ(!p3) {
        ఫలితం = CURLE_BAD_FUNCTION_వాదం;
        వెళ్ళు error;
      }
      ఒకవేళ(!getASN1Element(&విలువ, p3, atv.చివర)) {
        ఫలితం = CURLE_BAD_FUNCTION_వాదం;
        వెళ్ళు error;
      }
      curlx_dyn_reset(&తాత్కాలిక);
      ఫలితం = ASN1tostr(&తాత్కాలిక, &oid);
      ఒకవేళ(ఫలితం)
        వెళ్ళు error;

      str = curlx_dyn_uptr(&తాత్కాలిక);

      ఒకవేళ(!str) {
        ఫలితం = CURLE_BAD_FUNCTION_వాదం;
        వెళ్ళు error;
      }

      /* Encode delimiter.
         If attribute has a short uppercase name, delimiter is ", ". */
      కొరకు(p3 = str; ISUPPER(*p3); p3++)
        ;
      ఒకవేళ(added) {
        ఒకవేళ(p3 - str > 2)
          ఫలితం = curlx_dyn_addn(store, "/", 1);
        లేకపోతే
          ఫలితం = curlx_dyn_addn(store, ", ", 2);
        ఒకవేళ(ఫలితం)
          వెళ్ళు error;
      }

      /* Encode attribute name. */
      ఫలితం = curlx_dyn_కూడు(store, (స్థిరరాశి అక్షరం *)str);
      ఒకవేళ(ఫలితం)
        వెళ్ళు error;

      /* Generate equal sign. */
      ఫలితం = curlx_dyn_addn(store, "=", 1);
      ఒకవేళ(ఫలితం)
        వెళ్ళు error;

      /* Generate value. */
      ఫలితం = ASN1tostr(store, &విలువ);
      ఒకవేళ(ఫలితం)
        వెళ్ళు error;
      curlx_dyn_reset(&తాత్కాలిక);
      added = TRUE; /* use separator for next */
    }
  }
error:
  curlx_dyn_free(&తాత్కాలిక);

  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* WANT_EXTRACT_CERTINFO */

#నిర్వచితమైతే WANT_PARSEX509
/*
 * ASN.1 parse an X509 certificate into structure subfields.
 * Syntax is assumed to have already been checked by the SSL backend.
 * See RFC 5280.
 */
పూర్ణాంకం Curl_parseX509(నిర్మాణం Curl_X509certificate *cert,
                   స్థిరరాశి uint8_t *beg, స్థిరరాశి uint8_t *చివర)
{
  నిర్మాణం Curl_asn1Element elem;
  నిర్మాణం Curl_asn1Element tbsCertificate;
  స్థిరరాశి uint8_t *ccp;
  స్థిర స్థిరరాశి uint8_t default_సంస్కరణ = 0;  /* v1. */

  memset(cert, 0, పరిమాణం(*cert));
  cert->certificate.శీర్షిక = ఖాళీ;
  cert->certificate.beg = beg;
  cert->certificate.చివర = చివర;

  /* Get the sequence content. */
  ఒకవేళ(!getASN1Element(&elem, beg, చివర))
    తిరిగి -1;  /* Invalid bounds/size. */
  beg = elem.beg;
  చివర = elem.చివర;

  /* Get tbsCertificate. */
  beg = getASN1Element(&tbsCertificate, beg, చివర);
  ఒకవేళ(!beg)
    తిరిగి -1;
  /* Skip the signatureAlgorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, చివర);
  ఒకవేళ(!beg)
    తిరిగి -1;
  /* Get the signatureValue. */
  ఒకవేళ(!getASN1Element(&cert->signature, beg, చివర))
    తిరిగి -1;

  /* Parse TBSCertificate. */
  beg = tbsCertificate.beg;
  చివర = tbsCertificate.చివర;
  /* Get optional version, get serialNumber. */
  cert->సంస్కరణ_2.శీర్షిక = ఖాళీ;
  cert->సంస్కరణ_2.beg = &default_సంస్కరణ;
  cert->సంస్కరణ_2.చివర = &default_సంస్కరణ + పరిమాణం(default_సంస్కరణ);
  beg = getASN1Element(&elem, beg, చివర);
  ఒకవేళ(!beg)
    తిరిగి -1;
  ఒకవేళ(elem.ట్యాగ్ == 0) {
    ఒకవేళ(!getASN1Element(&cert->సంస్కరణ_2, elem.beg, elem.చివర))
      తిరిగి -1;
    beg = getASN1Element(&elem, beg, చివర);
    ఒకవేళ(!beg)
      తిరిగి -1;
  }
  cert->serial_సంఖ్య = elem;
  /* Get signature algorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, చివర);
  /* Get issuer. */
  beg = getASN1Element(&cert->issuer, beg, చివర);
  ఒకవేళ(!beg)
    తిరిగి -1;
  /* Get notBefore and notAfter. */
  beg = getASN1Element(&elem, beg, చివర);
  ఒకవేళ(!beg)
    తిరిగి -1;
  ccp = getASN1Element(&cert->notBefore, elem.beg, elem.చివర);
  ఒకవేళ(!ccp)
    తిరిగి -1;
  ఒకవేళ(!getASN1Element(&cert->notAfter, ccp, elem.చివర))
    తిరిగి -1;
  /* These two fields MUST consume all of the validity elem */
  ఒకవేళ(elem.చివర != cert->notAfter.చివర)
    తిరిగి -1;
  /* Get subject. */
  beg = getASN1Element(&cert->subject, beg, చివర);
  ఒకవేళ(!beg)
    తిరిగి -1;
  /* Get subjectPublicKeyAlgorithm and subjectPublicKey. */
  beg = getASN1Element(&cert->subject_ప్రజా_కీలకం_Info, beg, చివర);
  ఒకవేళ(!beg)
    తిరిగి -1;
  ccp = getASN1Element(&cert->subject_ప్రజా_కీలకం_Algorithm,
                       cert->subject_ప్రజా_కీలకం_Info.beg,
                       cert->subject_ప్రజా_కీలకం_Info.చివర);
  ఒకవేళ(!ccp)
    తిరిగి -1;
  ఒకవేళ(!getASN1Element(&cert->subject_ప్రజా_కీలకం, ccp,
                     cert->subject_ప్రజా_కీలకం_Info.చివర))
    తిరిగి -1;
  /* Get optional issuerUniqueID, subjectUniqueID and extensions. */
  cert->issuerUniqueID.ట్యాగ్ = cert->subjectUniqueID.ట్యాగ్ = 0;
  cert->extensions.ట్యాగ్ = elem.ట్యాగ్ = 0;
  cert->issuerUniqueID.శీర్షిక = cert->subjectUniqueID.శీర్షిక = ఖాళీ;
  cert->issuerUniqueID.beg = cert->issuerUniqueID.చివర = beg;
  cert->subjectUniqueID.beg = cert->subjectUniqueID.చివర = beg;
  cert->extensions.శీర్షిక = ఖాళీ;
  cert->extensions.beg = cert->extensions.చివర = beg;
  ఒకవేళ(beg < చివర) {
    beg = getASN1Element(&elem, beg, చివర);
    ఒకవేళ(!beg)
      తిరిగి -1;
  }
  ఒకవేళ(elem.ట్యాగ్ == 1) {
    cert->issuerUniqueID = elem;
    ఒకవేళ(beg < చివర) {
      beg = getASN1Element(&elem, beg, చివర);
      ఒకవేళ(!beg)
        తిరిగి -1;
    }
  }
  ఒకవేళ(elem.ట్యాగ్ == 2) {
    cert->subjectUniqueID = elem;
    ఒకవేళ(beg < చివర) {
      beg = getASN1Element(&elem, beg, చివర);
      ఒకవేళ(!beg)
        తిరిగి -1;
    }
  }
  ఒకవేళ(elem.ట్యాగ్ == 3)
    ఒకవేళ(!getASN1Element(&cert->extensions, elem.beg, elem.చివర))
      తిరిగి -1;
  తిరిగి 0;
}

#అయితే_ముగింపు /* WANT_PARSEX509 */

#నిర్వచితమైతే WANT_వెలికితీయి_CERTINFO

స్థిర CURLcode dumpAlgo(నిర్మాణం dynbuf *store,
                         నిర్మాణం Curl_asn1Element *param,
                         స్థిరరాశి uint8_t *beg, స్థిరరాశి uint8_t *చివర)
{
  నిర్మాణం Curl_asn1Element oid;

  /* Get algorithm parameters and return algorithm name. */

  beg = getASN1Element(&oid, beg, చివర);
  ఒకవేళ(!beg)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  param->శీర్షిక = ఖాళీ;
  param->ట్యాగ్ = 0;
  param->beg = param->చివర = చివర;
  ఒకవేళ(beg < చివర) {
    స్థిరరాశి uint8_t *p = getASN1Element(param, beg, చివర);
    ఒకవేళ(!p)
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }
  తిరిగి OID2str(store, oid.beg, oid.చివర);
}

/*
 * This is a convenience function for push_certinfo_len that takes a zero
 * terminated value.
 */
స్థిర CURLcode ssl_నెట్టు_certinfo(నిర్మాణం Curl_easy *దత్తాంశం,
                                  పూర్ణాంకం certnum,
                                  స్థిరరాశి అక్షరం *label,
                                  స్థిరరాశి అక్షరం *విలువ)
{
  size_t valuelen = అక్షరశ్రేణి_పొడవు(విలువ);

  తిరిగి Curl_ssl_నెట్టు_certinfo_len(దత్తాంశం, certnum, label, విలువ, valuelen);
}

/*
 * This is a convenience function for push_certinfo_len that takes a
 * dynbuf value.
 *
 * It also does the verbose output if !certnum.
 */
స్థిర CURLcode ssl_నెట్టు_certinfo_dyn(నిర్మాణం Curl_easy *దత్తాంశం,
                                      పూర్ణాంకం certnum,
                                      స్థిరరాశి అక్షరం *label,
                                      నిర్మాణం dynbuf *ptr)
{
  size_t valuelen = curlx_dyn_len(ptr);
  స్థిరరాశి అక్షరం *విలువ = curlx_dyn_ptr(ptr);

  CURLcode ఫలితం = Curl_ssl_నెట్టు_certinfo_len(దత్తాంశం, certnum, label,
                                               విలువ, valuelen);

  ఒకవేళ(!certnum && !ఫలితం)
    infof(దత్తాంశం, "   %s: %s", label, విలువ);

  తిరిగి ఫలితం;
}

స్థిర CURLcode do_pubkey_క్షేత్రం(నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం certnum,
                                స్థిరరాశి అక్షరం *label,
                                నిర్మాణం Curl_asn1Element *elem)
{
  CURLcode ఫలితం;
  నిర్మాణం dynbuf బహిర్గతం;

  curlx_dyn_init(&బహిర్గతం, CURL_X509_STR_MAX);

  /* Generate a certificate information record for the public key. */

  ఫలితం = ASN1tostr(&బహిర్గతం, elem);
  ఒకవేళ(!ఫలితం && దత్తాంశం->సమితి.ssl.certinfo)
    ఫలితం = ssl_నెట్టు_certinfo_dyn(దత్తాంశం, certnum, label, &బహిర్గతం);
  curlx_dyn_free(&బహిర్గతం);
  తిరిగి ఫలితం;
}

/* return 0 on success, 1 on error */
స్థిర పూర్ణాంకం do_pubkey(నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం certnum, స్థిరరాశి అక్షరం *algo,
                     నిర్మాణం Curl_asn1Element *param,
                     నిర్మాణం Curl_asn1Element *pubkey)
{
  నిర్మాణం Curl_asn1Element elem;
  నిర్మాణం Curl_asn1Element pk;
  స్థిరరాశి uint8_t *p;

  /* Generate all information records for the public key. */

  ఒకవేళ(curl_strequal(algo, "ecPublicKey")) {
    /*
     * ECC public key is all the data, a value of type BIT STRING mapped to
     * OCTET STRING and should not be parsed as an ASN.1 value.
     */
    స్థిరరాశి size_t dlen = pubkey->చివర - pubkey->beg;
    size_t len;
    ఒకవేళ(dlen < 2)
      /* too small */
      తిరిగి 1;
    len = (dlen - 2) * 4;
    ఒకవేళ(!certnum)
      infof(దత్తాంశం, "   ECC Public Key (%zu bits)", len);
    ఒకవేళ(దత్తాంశం->సమితి.ssl.certinfo) {
      అక్షరం q[(పరిమాణం(len) * 8 / 3) + 1];
      (శూన్య)curl_msnprintf(q, పరిమాణం(q), "%zu", len);
      ఒకవేళ(ssl_నెట్టు_certinfo(దత్తాంశం, certnum, "ECC Public Key", q))
        తిరిగి 1;
    }
    తిరిగి do_pubkey_క్షేత్రం(దత్తాంశం, certnum, "ecPublicKey", pubkey) == CURLE_OK
      ? 0 : 1;
  }

  /* Get the public key (single element). */
  ఒకవేళ(!getASN1Element(&pk, pubkey->beg + 1, pubkey->చివర))
    తిరిగి 1;

  ఒకవేళ(curl_strequal(algo, "rsaEncryption")) {
    స్థిరరాశి uint8_t *q;
    size_t len;

    p = getASN1Element(&elem, pk.beg, pk.చివర);
    ఒకవేళ(!p)
      తిరిగి 1;

    /* Compute key length. */
    కొరకు(q = elem.beg; q < elem.చివర && !*q; q++)
      ;
    len = ((elem.చివర - q) * 8);
    ఒకవేళ(len) {
      గుర్తులేని పూర్ణాంకం i;
      కొరకు(i = *q; !(i & 0x80); i <<= 1)
        len--;
    }
    ఒకవేళ(len > 32)
      elem.beg = q;     /* Strip leading zero bytes. */
    ఒకవేళ(!certnum)
      infof(దత్తాంశం, "   RSA Public Key (%zu bits)", len);
    ఒకవేళ(దత్తాంశం->సమితి.ssl.certinfo) {
      అక్షరం r[(పరిమాణం(len) * 8 / 3) + 1];
      curl_msnprintf(r, పరిమాణం(r), "%zu", len);
      ఒకవేళ(ssl_నెట్టు_certinfo(దత్తాంశం, certnum, "RSA Public Key", r))
        తిరిగి 1;
    }
    /* Generate coefficients. */
    ఒకవేళ(do_pubkey_క్షేత్రం(దత్తాంశం, certnum, "rsa(n)", &elem))
      తిరిగి 1;
    ఒకవేళ(!getASN1Element(&elem, p, pk.చివర))
      తిరిగి 1;
    ఒకవేళ(do_pubkey_క్షేత్రం(దత్తాంశం, certnum, "rsa(e)", &elem))
      తిరిగి 1;
  }
  లేకపోతే ఒకవేళ(curl_strequal(algo, "dsa")) {
    p = getASN1Element(&elem, param->beg, param->చివర);
    ఒకవేళ(p) {
      ఒకవేళ(do_pubkey_క్షేత్రం(దత్తాంశం, certnum, "dsa(p)", &elem))
        తిరిగి 1;
      p = getASN1Element(&elem, p, param->చివర);
      ఒకవేళ(p) {
        ఒకవేళ(do_pubkey_క్షేత్రం(దత్తాంశం, certnum, "dsa(q)", &elem))
          తిరిగి 1;
        ఒకవేళ(getASN1Element(&elem, p, param->చివర)) {
          ఒకవేళ(do_pubkey_క్షేత్రం(దత్తాంశం, certnum, "dsa(g)", &elem))
            తిరిగి 1;
          ఒకవేళ(do_pubkey_క్షేత్రం(దత్తాంశం, certnum, "dsa(pub_key)", &pk))
            తిరిగి 1;
        }
      }
    }
  }
  లేకపోతే ఒకవేళ(curl_strequal(algo, "dhpublicnumber")) {
    p = getASN1Element(&elem, param->beg, param->చివర);
    ఒకవేళ(p) {
      ఒకవేళ(do_pubkey_క్షేత్రం(దత్తాంశం, certnum, "dh(p)", &elem))
        తిరిగి 1;
      ఒకవేళ(getASN1Element(&elem, p, param->చివర)) {
        ఒకవేళ(do_pubkey_క్షేత్రం(దత్తాంశం, certnum, "dh(g)", &elem))
          తిరిగి 1;
        ఒకవేళ(do_pubkey_క్షేత్రం(దత్తాంశం, certnum, "dh(pub_key)", &pk))
          తిరిగి 1;
      }
    }
  }
  తిరిగి 0;
}

/*
 * Convert an ASN.1 distinguished name into a printable string.
 * Return error.
 */
స్థిర CURLcode DNtostr(నిర్మాణం dynbuf *store, నిర్మాణం Curl_asn1Element *dn)
{
  తిరిగి encodeDN(store, dn);
}

CURLcode Curl_వెలికితీయి_certinfo(నిర్మాణం Curl_easy *దత్తాంశం,
                               పూర్ణాంకం certnum,
                               స్థిరరాశి uint8_t *beg,
                               స్థిరరాశి uint8_t *చివర)
{
  నిర్మాణం Curl_X509certificate cert;
  నిర్మాణం Curl_asn1Element param;
  అక్షరం *certptr;
  size_t clen;
  నిర్మాణం dynbuf బహిర్గతం;
  CURLcode ఫలితం = CURLE_OK;
  గుర్తులేని పూర్ణాంకం సంస్కరణ_2;
  స్థిరరాశి uint8_t *ptr;
  పూర్ణాంకం rc;

  ఒకవేళ(!దత్తాంశం->సమితి.ssl.certinfo)
    ఒకవేళ(certnum)
      తిరిగి CURLE_OK;

  curlx_dyn_init(&బహిర్గతం, CURL_X509_STR_MAX);
  /* Prepare the certificate information for curl_easy_getinfo(). */

  /* Extract the certificate ASN.1 elements. */
  ఒకవేళ(Curl_parseX509(&cert, beg, చివర))
    తిరిగి CURLE_PEER_FAILED_VERIFICATION;

  /* Subject. */
  ఫలితం = DNtostr(&బహిర్గతం, &cert.subject);
  ఒకవేళ(ఫలితం)
    వెళ్ళు పూర్తి;
  ఒకవేళ(దత్తాంశం->సమితి.ssl.certinfo) {
    ఫలితం = ssl_నెట్టు_certinfo_dyn(దత్తాంశం, certnum, "Subject", &బహిర్గతం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు పూర్తి;
  }
  curlx_dyn_reset(&బహిర్గతం);

  /* Issuer. */
  ఫలితం = DNtostr(&బహిర్గతం, &cert.issuer);
  ఒకవేళ(ఫలితం)
    వెళ్ళు పూర్తి;
  ఒకవేళ(దత్తాంశం->సమితి.ssl.certinfo) {
    ఫలితం = ssl_నెట్టు_certinfo_dyn(దత్తాంశం, certnum, "Issuer", &బహిర్గతం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు పూర్తి;
  }
  curlx_dyn_reset(&బహిర్గతం);

  /* Version (always fits in less than 32 bits). */
  సంస్కరణ_2 = 0;
  కొరకు(ptr = cert.సంస్కరణ_2.beg; ptr < cert.సంస్కరణ_2.చివర; ptr++)
    సంస్కరణ_2 = (సంస్కరణ_2 << 8) | *ptr;
  ఒకవేళ(దత్తాంశం->సమితి.ssl.certinfo) {
    ఫలితం = curlx_dyn_addf(&బహిర్గతం, "%x", సంస్కరణ_2);
    ఒకవేళ(ఫలితం)
      వెళ్ళు పూర్తి;
    ఫలితం = ssl_నెట్టు_certinfo_dyn(దత్తాంశం, certnum, "Version", &బహిర్గతం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు పూర్తి;
    curlx_dyn_reset(&బహిర్గతం);
  }

  /* Serial number. */
  ఫలితం = ASN1tostr(&బహిర్గతం, &cert.serial_సంఖ్య);
  ఒకవేళ(ఫలితం)
    వెళ్ళు పూర్తి;
  ఒకవేళ(దత్తాంశం->సమితి.ssl.certinfo) {
    ఫలితం = ssl_నెట్టు_certinfo_dyn(దత్తాంశం, certnum, "Serial Number", &బహిర్గతం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు పూర్తి;
  }
  curlx_dyn_reset(&బహిర్గతం);

  /* Signature algorithm .*/
  ఫలితం = dumpAlgo(&బహిర్గతం, &param, cert.signatureAlgorithm.beg,
                    cert.signatureAlgorithm.చివర);
  ఒకవేళ(ఫలితం)
    వెళ్ళు పూర్తి;
  ఒకవేళ(దత్తాంశం->సమితి.ssl.certinfo) {
    ఫలితం = ssl_నెట్టు_certinfo_dyn(దత్తాంశం, certnum, "Signature Algorithm", &బహిర్గతం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు పూర్తి;
  }
  curlx_dyn_reset(&బహిర్గతం);

  /* Start Date. */
  ఫలితం = ASN1tostr(&బహిర్గతం, &cert.notBefore);
  ఒకవేళ(ఫలితం)
    వెళ్ళు పూర్తి;
  ఒకవేళ(దత్తాంశం->సమితి.ssl.certinfo) {
    ఫలితం = ssl_నెట్టు_certinfo_dyn(దత్తాంశం, certnum, "Start Date", &బహిర్గతం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు పూర్తి;
  }
  curlx_dyn_reset(&బహిర్గతం);

  /* Expire Date. */
  ఫలితం = ASN1tostr(&బహిర్గతం, &cert.notAfter);
  ఒకవేళ(ఫలితం)
    వెళ్ళు పూర్తి;
  ఒకవేళ(దత్తాంశం->సమితి.ssl.certinfo) {
    ఫలితం = ssl_నెట్టు_certinfo_dyn(దత్తాంశం, certnum, "Expire Date", &బహిర్గతం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు పూర్తి;
  }
  curlx_dyn_reset(&బహిర్గతం);

  /* Public Key Algorithm. */
  ఫలితం = dumpAlgo(&బహిర్గతం, &param, cert.subject_ప్రజా_కీలకం_Algorithm.beg,
                    cert.subject_ప్రజా_కీలకం_Algorithm.చివర);
  ఒకవేళ(ఫలితం)
    వెళ్ళు పూర్తి;
  ఒకవేళ(దత్తాంశం->సమితి.ssl.certinfo) {
    ఫలితం = ssl_నెట్టు_certinfo_dyn(దత్తాంశం, certnum, "Public Key Algorithm",
                                   &బహిర్గతం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు పూర్తి;
  }

  rc = do_pubkey(దత్తాంశం, certnum, curlx_dyn_ptr(&బహిర్గతం),
                 &param, &cert.subject_ప్రజా_కీలకం);
  ఒకవేళ(rc) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం; /* the most likely error */
    వెళ్ళు పూర్తి;
  }
  curlx_dyn_reset(&బహిర్గతం);

  /* Signature. */
  ఫలితం = ASN1tostr(&బహిర్గతం, &cert.signature);
  ఒకవేళ(ఫలితం)
    వెళ్ళు పూర్తి;
  ఒకవేళ(దత్తాంశం->సమితి.ssl.certinfo) {
    ఫలితం = ssl_నెట్టు_certinfo_dyn(దత్తాంశం, certnum, "Signature", &బహిర్గతం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు పూర్తి;
  }
  curlx_dyn_reset(&బహిర్గతం);

  /* Generate PEM certificate. */
  ఫలితం = curlx_base64_encode((స్థిరరాశి uint8_t *)cert.certificate.beg,
                               cert.certificate.చివర - cert.certificate.beg,
                               &certptr, &clen);
  ఒకవేళ(ఫలితం)
    వెళ్ళు పూర్తి;

  /* Generate the final output certificate string. Format is:
     -----BEGIN CERTIFICATE-----\n
     <max 64 base64 characters>\n
     .
     .
     .
     -----END CERTIFICATE-----\n
   */

  curlx_dyn_reset(&బహిర్గతం);

  /* Build the certificate string. */
  ఫలితం = curlx_dyn_కూడు(&బహిర్గతం, "-----BEGIN CERTIFICATE-----\n");
  ఒకవేళ(!ఫలితం) {
    size_t j = 0;

    అయితే(!ఫలితం && (j < clen)) {
      size_t chunksize = (clen - j) > 64 ? 64 : (clen - j);
      ఫలితం = curlx_dyn_addn(&బహిర్గతం, &certptr[j], chunksize);
      ఒకవేళ(!ఫలితం)
        ఫలితం = curlx_dyn_addn(&బహిర్గతం, "\n", 1);
      j += chunksize;
    }
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_కూడు(&బహిర్గతం, "-----END CERTIFICATE-----\n");
  }
  curlx_free(certptr);
  ఒకవేళ(!ఫలితం)
    ఒకవేళ(దత్తాంశం->సమితి.ssl.certinfo)
      ఫలితం = ssl_నెట్టు_certinfo_dyn(దత్తాంశం, certnum, "Cert", &బహిర్గతం);

పూర్తి:
  ఒకవేళ(ఫలితం)
    failf(దత్తాంశం, "Failed extracting certificate chain");
  curlx_dyn_free(&బహిర్గతం);
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* WANT_EXTRACT_CERTINFO */

#అయితే_ముగింపు /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
