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
#परिभाषय WANT_PARSEX509 /* uses Curl_parseX509() */
#यदि_अन्तः

#यदि defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_RUSTLS)
#परिभाषय WANT_EXTRACT_CERTINFO /* uses Curl_extract_certinfo() */
#यदि_अन्तः

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
#परिभाषय CURL_ASN1_MAX                   ((size_t)0x40000)      /* 256K */

#यदि_परिभाषितम् WANT_EXTRACT_CERTINFO

/* ASN.1 OID table entry. */
संरचना Curl_OID {
  ध्रुव वर्ण *numoid;  /* Dotted-numeric OID. */
  ध्रुव वर्ण *textoid; /* OID name. */
};

/* ASN.1 OIDs. */
स्थिर ध्रुव संरचना Curl_OID OIDtable[] = {
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

#यदि_अन्तः /* WANT_EXTRACT_CERTINFO */

/*
 * Lightweight ASN.1 parser.
 * In particular, it does not check for syntactic/lexical errors.
 * It is intended to support certificate information gathering for SSL backends
 * that offer a mean to get certificates as a whole, but do not supply
 * entry points to get particular certificate sub-fields.
 * Please note there is no pretension here to rewrite a full SSL library.
 */

#परिभाषय CURL_ASN1_MAX_RECURSIONS    16

स्थिर ध्रुव uint8_t *getASN1Element_(संरचना Curl_asn1Element *elem,
                                      ध्रुव uint8_t *beg, ध्रुव uint8_t *अन्तः,
                                      size_t lvl)
{
  uint8_t b;
  size_t len;
  संरचना Curl_asn1Element lelem;

  /* Get a single ASN.1 element into `elem', parse ASN.1 string at `beg'
     ending at `end'.
     Returns a pointer in source string after the parsed element, or NULL
     if an error occurs. */
  यदि(!beg || !अन्तः || beg >= अन्तः || !*beg ||
     ((size_t)(अन्तः - beg) > CURL_ASN1_MAX) ||
     lvl++ >= CURL_ASN1_MAX_RECURSIONS)
    प्रत्यागम रिक्त;

  /* Process header byte. */
  elem->शीर्षकम् = beg;
  b = *beg++;
  elem->constructed = (b & 0x20) != 0;
  elem->eclass = (b >> 6) & 3;
  b &= 0x1F;
  यदि(b == 0x1F)
    प्रत्यागम रिक्त; /* Long tag values not supported here. */
  elem->चिह्नकम् = b;

  /* Process length. */
  यदि(beg >= अन्तः)
    प्रत्यागम रिक्त;
  b = *beg++;
  यदि(!(b & 0x80))
    len = b;
  अन्यथा {
    b &= 0x7F;
    यदि(!b) {
      /* Unspecified length. Since we have all the data, we can determine the
         effective length by skipping element until an EOC element is found. */
      यदि(!elem->constructed)
        प्रत्यागम रिक्त;
      elem->beg = beg;
      यावत्(beg < अन्तः && *beg) {
        beg = getASN1Element_(&lelem, beg, अन्तः, lvl);
        यदि(!beg)
          प्रत्यागम रिक्त;
      }
      यदि(अन्तः - beg < 2 || beg[1])       /* EOC must have a zero length. */
        प्रत्यागम रिक्त;
      elem->अन्तः = beg;                  /* Do not include EOC as content. */
      प्रत्यागम beg + 2;                   /* After EOC element. */
    }
    अन्यथा यदि(b > (size_t)(अन्तः - beg))
      प्रत्यागम रिक्त; /* Does not fit in source. */
    अन्यथा {
      /* Get long length. */
      len = 0;
      कुरु {
        यदि(len & 0xFF000000L)
          प्रत्यागम रिक्त;  /* Lengths > 32 bits are not supported. */
        len = (len << 8) | *beg++;
      } यावत्(--b);
    }
  }
  यदि(len > (size_t)(अन्तः - beg))
    प्रत्यागम रिक्त;  /* Element data does not fit in source. */
  elem->beg = beg;
  elem->अन्तः = beg + len;
  प्रत्यागम elem->अन्तः;
}

/*
 * @unittest 1657
 */
UNITTEST ध्रुव uint8_t *getASN1Element(संरचना Curl_asn1Element *elem,
                                       ध्रुव uint8_t *beg, ध्रुव uint8_t *अन्तः);
UNITTEST ध्रुव uint8_t *getASN1Element(संरचना Curl_asn1Element *elem,
                                       ध्रुव uint8_t *beg, ध्रुव uint8_t *अन्तः)
{
  प्रत्यागम getASN1Element_(elem, beg, अन्तः, 0);
}

#यदि_परिभाषितम् WANT_EXTRACT_CERTINFO

/*
 * Search the null-terminated OID or OID identifier in local table.
 * Return the table entry pointer or NULL if not found.
 */
स्थिर ध्रुव संरचना Curl_OID *अन्वेषणम्_OID(ध्रुव वर्ण *oid)
{
  ध्रुव संरचना Curl_OID *op;
  प्रति(op = OIDtable; op->numoid; op++)
    यदि(!वाक्यं_तुलय(op->numoid, oid) || curl_strequal(op->textoid, oid))
      प्रत्यागम op;

  प्रत्यागम रिक्त;
}

/*
 * Convert an ASN.1 Boolean value into its string representation.
 *
 * Return error code.
 */

स्थिर CURLcode bool2str(संरचना dynbuf *store,
                         ध्रुव uint8_t *beg, ध्रुव uint8_t *अन्तः)
{
  uint8_t val;
  यदि(अन्तः - beg != 1)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  val = *beg;
  यदि((val != 0xff) && (val != 0x00))
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  प्रत्यागम curlx_dyn_सङ्कलय(store, val ? "TRUE" : "FALSE");
}

/*
 * Convert an ASN.1 octet string to a printable string.
 *
 * Return error code.
 */
स्थिर CURLcode octet2str(संरचना dynbuf *store,
                          ध्रुव uint8_t *beg, ध्रुव uint8_t *अन्तः)
{
  CURLcode परिणाम = CURLE_OK;

  यावत्(!परिणाम && beg < अन्तः)
    परिणाम = curlx_dyn_addf(store, "%02x:", *beg++);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode bit2str(संरचना dynbuf *store,
                        ध्रुव uint8_t *beg, ध्रुव uint8_t *अन्तः)
{
  /* Convert an ASN.1 bit string to a printable string. */

  यदि(beg < अन्तः) {
    uint8_t अप्रयुक्तम्_द्वयङ्काः = *beg++;
    यदि(अप्रयुक्तम्_द्वयङ्काः > 7)
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }
  अन्यथा
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  प्रत्यागम octet2str(store, beg, अन्तः);
}

/*
 * Convert an ASN.1 integer value into its string representation.
 *
 * Returns error.
 */
स्थिर CURLcode int2str(संरचना dynbuf *store,
                        ध्रुव uint8_t *beg, ध्रुव uint8_t *अन्तः)
{
  uint32_t uval = 0;
  int32_t sval = 0;
  size_t n = अन्तः - beg;
  size_t i;
  ध्रुव uint8_t *p = beg;

  यदि(!n)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  यदि(n > 4)
    प्रत्यागम octet2str(store, beg, अन्तः);

  यदि(p[0] & 0x80)
    uval = ~0U;

  प्रति(i = 0; i < n; i++)
    uval = (uval << 8) | p[i];

  sval = (int32_t)uval;

  /* small values are easier to read as decimals */
  यदि(sval > -10000 && sval < 10000)
    प्रत्यागम curlx_dyn_addf(store, "%d", sval);

  /* represent larger values as hex */
  प्रत्यागम curlx_dyn_addf(store, "0x%x", uval);
}

/*
 * Convert from an ASN.1 typed string to UTF8.
 *
 * The result is stored in a dynbuf that is inited by the user of this
 * function.
 *
 * Returns error.
 */
स्थिर CURLcode utf8asn1str(संरचना dynbuf *गन्तव्यम्, पूर्णाङ्क type,
                            ध्रुव uint8_t *from, ध्रुव uint8_t *अन्तः)
{
  size_t inlength = अन्तः - from;
  पूर्णाङ्क आकार = 1;
  CURLcode परिणाम = CURLE_OK;

  विकल्प(type) {
  पक्ष CURL_ASN1_BMP_STRING:
    आकार = 2;
    भङ्ग;
  पक्ष CURL_ASN1_UNIVERSAL_STRING:
    आकार = 4;
    भङ्ग;
  पक्ष CURL_ASN1_NUMERIC_STRING:
  पक्ष CURL_ASN1_PRINTABLE_STRING:
  पक्ष CURL_ASN1_TELETEX_STRING:
  पक्ष CURL_ASN1_IA5_STRING:
  पक्ष CURL_ASN1_VISIBLE_STRING:
  पक्ष CURL_ASN1_UTF8_STRING:
    भङ्ग;
  स्वभाव:
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;  /* Conversion not supported. */
  }

  यदि(inlength % आकार)
    /* Length inconsistent with character size. */
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  यदि(type == CURL_ASN1_UTF8_STRING) {
    /* copy. */
    यदि(inlength)
      परिणाम = curlx_dyn_addn(गन्तव्यम्, from, inlength);
  }
  अन्यथा {
    यावत्(!परिणाम && (from < अन्तः)) {
      वर्ण buf[4]; /* decode buffer */
      size_t charsize = 1;
      अचिह्नित पूर्णाङ्क wc = 0;

      विकल्प(आकार) {
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
              प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
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
      परिणाम = curlx_dyn_addn(गन्तव्यम्, buf, charsize);
    }
  }
  प्रत्यागम परिणाम;
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
                            ध्रुव uint8_t *beg, ध्रुव uint8_t *अन्तः);
UNITTEST CURLcode encodeOID(संरचना dynbuf *store,
                            ध्रुव uint8_t *beg, ध्रुव uint8_t *अन्तः)
{
  uint32_t x;
  uint32_t y;
  CURLcode परिणाम = CURLE_OK;

  यदि(अन्तः <= beg)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  /* Process the first two numbers. The initial digit cannot be larger than
     2 */
  y = *beg++;
  यदि(y <= 80)
    x = y / 40;
  अन्यथा
    x = 2;
  परिणाम = curlx_dyn_addf(store, "%u", x);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  यदि(y & 0x80) {
    uint32_t t = (y & 0x7f); /* the second number */
    यदि(!t)
      /* reject leading 0x80 padding */
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    कुरु {
      यदि((t & 0xFE000000) || (beg >= अन्तः))
        प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
      y = *beg++;
      t = (t << 7) | (y & 0x7F);
    } यावत्(y & 0x80);
    y = t;
  }

  परिणाम = curlx_dyn_addf(store, ".%u", y - (x * 40));
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* Process the trailing numbers. */
  यावत्((beg < अन्तः) && !परिणाम) {
    x = 0;
    y = 0;
    कुरु {
      /* 0x80 as the first value is invalid since it only adds padding */
      यदि((y & 0x80) && !x)
        प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
      अन्यथा यदि((x & 0xFE000000) || (beg == अन्तः))
        प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
      y = *beg++;
      x = (x << 7) | (y & 0x7F);
    } यावत्(y & 0x80);
    परिणाम = curlx_dyn_addf(store, ".%u", x);
  }
  प्रत्यागम परिणाम;
}

/*
 * Convert an ASN.1 OID into its dotted or symbolic string representation.
 *
 * Return error code.
 */

स्थिर CURLcode OID2str(संरचना dynbuf *store,
                        ध्रुव uint8_t *beg, ध्रुव uint8_t *अन्तः)
{
  CURLcode परिणाम = CURLE_OK;
  यदि(beg < अन्तः) {
    संरचना dynbuf buf;
    curlx_dyn_init(&buf, CURL_X509_STR_MAX);
    परिणाम = encodeOID(&buf, beg, अन्तः);

    यदि(!परिणाम) {
      ध्रुव संरचना Curl_OID *op = अन्वेषणम्_OID(curlx_dyn_ptr(&buf));
      यदि(op)
        परिणाम = curlx_dyn_सङ्कलय(store, op->textoid);
      अन्यथा
        परिणाम = curlx_dyn_सङ्कलय(store, curlx_dyn_ptr(&buf));
    }
    curlx_dyn_free(&buf);
  }
  प्रत्यागम परिणाम;
}

/*
 * @unittest 1656
 */
UNITTEST CURLcode GTime2str(संरचना dynbuf *store,
                            ध्रुव वर्ण *beg, ध्रुव वर्ण *अन्तः);
UNITTEST CURLcode GTime2str(संरचना dynbuf *store,
                            ध्रुव वर्ण *beg, ध्रुव वर्ण *अन्तः)
{
  ध्रुव वर्ण *tzp;
  ध्रुव वर्ण *fracp;
  वर्ण sec1, sec2;
  size_t fracl;
  size_t tzl;
  ध्रुव वर्ण *sep = "";

  /* Convert an ASN.1 Generalized time to a printable string.
     Return the dynamically allocated string, or NULL if an error occurs. */

  प्रति(fracp = beg; fracp < अन्तः && ISDIGIT(*fracp); fracp++)
    ;

  /* Get seconds digits. */
  sec1 = '0';
  विकल्प(fracp - beg - 12) {
  पक्ष 0:
    sec2 = '0';
    भङ्ग;
  पक्ष 2:
    sec1 = fracp[-2];
    FALLTHROUGH();
  पक्ष 1:
    sec2 = fracp[-1];
    भङ्ग;
  स्वभाव:
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }

  /* timezone follows optional fractional seconds. */
  tzp = fracp;
  fracl = 0; /* no fractional seconds detected so far */
  यदि(fracp < अन्तः && (*fracp == '.' || *fracp == ',')) {
    /* Have fractional seconds, e.g. "[.,]\d+". How many? */
    fracp++; /* should be a digit char or BAD ARGUMENT */
    tzp = fracp;
    यावत्(tzp < अन्तः && ISDIGIT(*tzp))
      tzp++;
    यदि(tzp == fracp) /* never looped, no digit after [.,] */
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    fracl = tzp - fracp; /* number of fractional sec digits */
    DEBUGASSERT(fracl > 0);
    /* Strip trailing zeroes in fractional seconds.
     * May reduce fracl to 0 if only '0's are present. */
    यावत्(fracl && fracp[fracl - 1] == '0')
      fracl--;
  }

  /* Process timezone. */
  यदि(tzp >= अन्तः) {
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
    tzl = अन्तः - tzp;
  }
  अन्यथा {
    sep = " ";
    tzl = अन्तः - tzp;
  }

  प्रत्यागम curlx_dyn_addf(store,
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
                          ध्रुव वर्ण *beg, ध्रुव वर्ण *अन्तः)
{
  ध्रुव वर्ण *tzp;
  size_t tzl;
  ध्रुव वर्ण *sec;

  प्रति(tzp = beg; tzp < अन्तः && *tzp >= '0' && *tzp <= '9'; tzp++)
    ;
  /* Get the seconds. */
  sec = beg + 10;
  विकल्प(tzp - sec) {
  पक्ष 0:
    sec = "00";
    FALLTHROUGH();
  पक्ष 2:
    भङ्ग;
  स्वभाव:
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }

  /* Process timezone. */
  यदि(tzp >= अन्तः)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  यदि(*tzp == 'Z') {
    tzp = "GMT";
    अन्तः = tzp + 3;
  }

  tzl = अन्तः - tzp;
  प्रत्यागम curlx_dyn_addf(store, "%d%.2s-%.2s-%.2s %.2s:%.2s:%.2s %.*s",
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
  CURLcode परिणाम = CURLE_BAD_FUNCTION_तर्कः;
  पूर्णाङ्क type = elem->चिह्नकम्;
  यदि(elem->constructed)
    प्रत्यागम परिणाम; /* No conversion of structured elements. */

  विकल्प(type) {
  पक्ष CURL_ASN1_BOOLEAN:
    परिणाम = bool2str(store, elem->beg, elem->अन्तः);
    भङ्ग;
  पक्ष CURL_ASN1_INTEGER:
  पक्ष CURL_ASN1_ENUMERATED:
    परिणाम = int2str(store, elem->beg, elem->अन्तः);
    भङ्ग;
  पक्ष CURL_ASN1_BIT_STRING:
    परिणाम = bit2str(store, elem->beg, elem->अन्तः);
    भङ्ग;
  पक्ष CURL_ASN1_OCTET_STRING:
    परिणाम = octet2str(store, elem->beg, elem->अन्तः);
    भङ्ग;
  पक्ष CURL_ASN1_NULL:
    परिणाम = curlx_dyn_addn(store, "", 1);
    भङ्ग;
  पक्ष CURL_ASN1_OBJECT_IDENTIFIER:
    परिणाम = OID2str(store, elem->beg, elem->अन्तः);
    भङ्ग;
  पक्ष CURL_ASN1_UTC_TIME:
    परिणाम = UTime2str(store, (ध्रुव वर्ण *)elem->beg,
                       (ध्रुव वर्ण *)elem->अन्तः);
    भङ्ग;
  पक्ष CURL_ASN1_GENERALIZED_TIME:
    परिणाम = GTime2str(store, (ध्रुव वर्ण *)elem->beg,
                       (ध्रुव वर्ण *)elem->अन्तः);
    भङ्ग;
  पक्ष CURL_ASN1_UTF8_STRING:
  पक्ष CURL_ASN1_NUMERIC_STRING:
  पक्ष CURL_ASN1_PRINTABLE_STRING:
  पक्ष CURL_ASN1_TELETEX_STRING:
  पक्ष CURL_ASN1_IA5_STRING:
  पक्ष CURL_ASN1_VISIBLE_STRING:
  पक्ष CURL_ASN1_UNIVERSAL_STRING:
  पक्ष CURL_ASN1_BMP_STRING:
    परिणाम = utf8asn1str(store, type, elem->beg, elem->अन्तः);
    भङ्ग;
  }

  प्रत्यागम परिणाम;
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
  संरचना Curl_asn1Element मानम्;
  ध्रुव uint8_t *p1;
  ध्रुव uint8_t *p2;
  ध्रुव uint8_t *p3;
  ध्रुव uint8_t *str;
  CURLcode परिणाम = CURLE_OK;
  bool added = FALSE;
  संरचना dynbuf अस्थायी;
  curlx_dyn_init(&अस्थायी, CURL_X509_STR_MAX);

  प्रति(p1 = dn->beg; p1 < dn->अन्तः;) {
    p1 = getASN1Element(&rdn, p1, dn->अन्तः);
    यदि(!p1) {
      परिणाम = CURLE_BAD_FUNCTION_तर्कः;
      गच्छ error;
    }
    प्रति(p2 = rdn.beg; p2 < rdn.अन्तः;) {
      p2 = getASN1Element(&atv, p2, rdn.अन्तः);
      यदि(!p2) {
        परिणाम = CURLE_BAD_FUNCTION_तर्कः;
        गच्छ error;
      }
      p3 = getASN1Element(&oid, atv.beg, atv.अन्तः);
      यदि(!p3) {
        परिणाम = CURLE_BAD_FUNCTION_तर्कः;
        गच्छ error;
      }
      यदि(!getASN1Element(&मानम्, p3, atv.अन्तः)) {
        परिणाम = CURLE_BAD_FUNCTION_तर्कः;
        गच्छ error;
      }
      curlx_dyn_reset(&अस्थायी);
      परिणाम = ASN1tostr(&अस्थायी, &oid);
      यदि(परिणाम)
        गच्छ error;

      str = curlx_dyn_uptr(&अस्थायी);

      यदि(!str) {
        परिणाम = CURLE_BAD_FUNCTION_तर्कः;
        गच्छ error;
      }

      /* Encode delimiter.
         If attribute has a short uppercase name, delimiter is ", ". */
      प्रति(p3 = str; ISUPPER(*p3); p3++)
        ;
      यदि(added) {
        यदि(p3 - str > 2)
          परिणाम = curlx_dyn_addn(store, "/", 1);
        अन्यथा
          परिणाम = curlx_dyn_addn(store, ", ", 2);
        यदि(परिणाम)
          गच्छ error;
      }

      /* Encode attribute name. */
      परिणाम = curlx_dyn_सङ्कलय(store, (ध्रुव वर्ण *)str);
      यदि(परिणाम)
        गच्छ error;

      /* Generate equal sign. */
      परिणाम = curlx_dyn_addn(store, "=", 1);
      यदि(परिणाम)
        गच्छ error;

      /* Generate value. */
      परिणाम = ASN1tostr(store, &मानम्);
      यदि(परिणाम)
        गच्छ error;
      curlx_dyn_reset(&अस्थायी);
      added = TRUE; /* use separator for next */
    }
  }
error:
  curlx_dyn_free(&अस्थायी);

  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* WANT_EXTRACT_CERTINFO */

#यदि_परिभाषितम् WANT_PARSEX509
/*
 * ASN.1 parse an X509 certificate into structure subfields.
 * Syntax is assumed to have already been checked by the SSL backend.
 * See RFC 5280.
 */
पूर्णाङ्क Curl_parseX509(संरचना Curl_X509certificate *cert,
                   ध्रुव uint8_t *beg, ध्रुव uint8_t *अन्तः)
{
  संरचना Curl_asn1Element elem;
  संरचना Curl_asn1Element tbsCertificate;
  ध्रुव uint8_t *ccp;
  स्थिर ध्रुव uint8_t default_संस्करणम् = 0;  /* v1. */

  स्मृतिं_पूरय(cert, 0, परिमाण(*cert));
  cert->certificate.शीर्षकम् = रिक्त;
  cert->certificate.beg = beg;
  cert->certificate.अन्तः = अन्तः;

  /* Get the sequence content. */
  यदि(!getASN1Element(&elem, beg, अन्तः))
    प्रत्यागम -1;  /* Invalid bounds/size. */
  beg = elem.beg;
  अन्तः = elem.अन्तः;

  /* Get tbsCertificate. */
  beg = getASN1Element(&tbsCertificate, beg, अन्तः);
  यदि(!beg)
    प्रत्यागम -1;
  /* Skip the signatureAlgorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, अन्तः);
  यदि(!beg)
    प्रत्यागम -1;
  /* Get the signatureValue. */
  यदि(!getASN1Element(&cert->signature, beg, अन्तः))
    प्रत्यागम -1;

  /* Parse TBSCertificate. */
  beg = tbsCertificate.beg;
  अन्तः = tbsCertificate.अन्तः;
  /* Get optional version, get serialNumber. */
  cert->संस्करणम्_2.शीर्षकम् = रिक्त;
  cert->संस्करणम्_2.beg = &default_संस्करणम्;
  cert->संस्करणम्_2.अन्तः = &default_संस्करणम् + परिमाण(default_संस्करणम्);
  beg = getASN1Element(&elem, beg, अन्तः);
  यदि(!beg)
    प्रत्यागम -1;
  यदि(elem.चिह्नकम् == 0) {
    यदि(!getASN1Element(&cert->संस्करणम्_2, elem.beg, elem.अन्तः))
      प्रत्यागम -1;
    beg = getASN1Element(&elem, beg, अन्तः);
    यदि(!beg)
      प्रत्यागम -1;
  }
  cert->serial_सङ्ख्या = elem;
  /* Get signature algorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, अन्तः);
  /* Get issuer. */
  beg = getASN1Element(&cert->issuer, beg, अन्तः);
  यदि(!beg)
    प्रत्यागम -1;
  /* Get notBefore and notAfter. */
  beg = getASN1Element(&elem, beg, अन्तः);
  यदि(!beg)
    प्रत्यागम -1;
  ccp = getASN1Element(&cert->notBefore, elem.beg, elem.अन्तः);
  यदि(!ccp)
    प्रत्यागम -1;
  यदि(!getASN1Element(&cert->notAfter, ccp, elem.अन्तः))
    प्रत्यागम -1;
  /* These two fields MUST consume all of the validity elem */
  यदि(elem.अन्तः != cert->notAfter.अन्तः)
    प्रत्यागम -1;
  /* Get subject. */
  beg = getASN1Element(&cert->subject, beg, अन्तः);
  यदि(!beg)
    प्रत्यागम -1;
  /* Get subjectPublicKeyAlgorithm and subjectPublicKey. */
  beg = getASN1Element(&cert->subject_लौकिकम्_कुञ्चिका_Info, beg, अन्तः);
  यदि(!beg)
    प्रत्यागम -1;
  ccp = getASN1Element(&cert->subject_लौकिकम्_कुञ्चिका_Algorithm,
                       cert->subject_लौकिकम्_कुञ्चिका_Info.beg,
                       cert->subject_लौकिकम्_कुञ्चिका_Info.अन्तः);
  यदि(!ccp)
    प्रत्यागम -1;
  यदि(!getASN1Element(&cert->subject_लौकिकम्_कुञ्चिका, ccp,
                     cert->subject_लौकिकम्_कुञ्चिका_Info.अन्तः))
    प्रत्यागम -1;
  /* Get optional issuerUniqueID, subjectUniqueID and extensions. */
  cert->issuerUniqueID.चिह्नकम् = cert->subjectUniqueID.चिह्नकम् = 0;
  cert->extensions.चिह्नकम् = elem.चिह्नकम् = 0;
  cert->issuerUniqueID.शीर्षकम् = cert->subjectUniqueID.शीर्षकम् = रिक्त;
  cert->issuerUniqueID.beg = cert->issuerUniqueID.अन्तः = beg;
  cert->subjectUniqueID.beg = cert->subjectUniqueID.अन्तः = beg;
  cert->extensions.शीर्षकम् = रिक्त;
  cert->extensions.beg = cert->extensions.अन्तः = beg;
  यदि(beg < अन्तः) {
    beg = getASN1Element(&elem, beg, अन्तः);
    यदि(!beg)
      प्रत्यागम -1;
  }
  यदि(elem.चिह्नकम् == 1) {
    cert->issuerUniqueID = elem;
    यदि(beg < अन्तः) {
      beg = getASN1Element(&elem, beg, अन्तः);
      यदि(!beg)
        प्रत्यागम -1;
    }
  }
  यदि(elem.चिह्नकम् == 2) {
    cert->subjectUniqueID = elem;
    यदि(beg < अन्तः) {
      beg = getASN1Element(&elem, beg, अन्तः);
      यदि(!beg)
        प्रत्यागम -1;
    }
  }
  यदि(elem.चिह्नकम् == 3)
    यदि(!getASN1Element(&cert->extensions, elem.beg, elem.अन्तः))
      प्रत्यागम -1;
  प्रत्यागम 0;
}

#यदि_अन्तः /* WANT_PARSEX509 */

#यदि_परिभाषितम् WANT_EXTRACT_CERTINFO

स्थिर CURLcode dumpAlgo(संरचना dynbuf *store,
                         संरचना Curl_asn1Element *param,
                         ध्रुव uint8_t *beg, ध्रुव uint8_t *अन्तः)
{
  संरचना Curl_asn1Element oid;

  /* Get algorithm parameters and return algorithm name. */

  beg = getASN1Element(&oid, beg, अन्तः);
  यदि(!beg)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  param->शीर्षकम् = रिक्त;
  param->चिह्नकम् = 0;
  param->beg = param->अन्तः = अन्तः;
  यदि(beg < अन्तः) {
    ध्रुव uint8_t *p = getASN1Element(param, beg, अन्तः);
    यदि(!p)
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }
  प्रत्यागम OID2str(store, oid.beg, oid.अन्तः);
}

/*
 * This is a convenience function for push_certinfo_len that takes a zero
 * terminated value.
 */
स्थिर CURLcode ssl_प्रक्षिप_certinfo(संरचना Curl_easy *दत्त,
                                  पूर्णाङ्क certnum,
                                  ध्रुव वर्ण *label,
                                  ध्रुव वर्ण *मानम्)
{
  size_t valuelen = वाक्यदैर्घ्यम्(मानम्);

  प्रत्यागम Curl_ssl_प्रक्षिप_certinfo_len(दत्त, certnum, label, मानम्, valuelen);
}

/*
 * This is a convenience function for push_certinfo_len that takes a
 * dynbuf value.
 *
 * It also does the verbose output if !certnum.
 */
स्थिर CURLcode ssl_प्रक्षिप_certinfo_dyn(संरचना Curl_easy *दत्त,
                                      पूर्णाङ्क certnum,
                                      ध्रुव वर्ण *label,
                                      संरचना dynbuf *ptr)
{
  size_t valuelen = curlx_dyn_len(ptr);
  ध्रुव वर्ण *मानम् = curlx_dyn_ptr(ptr);

  CURLcode परिणाम = Curl_ssl_प्रक्षिप_certinfo_len(दत्त, certnum, label,
                                               मानम्, valuelen);

  यदि(!certnum && !परिणाम)
    infof(दत्त, "   %s: %s", label, मानम्);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode do_pubkey_क्षेत्रम्(संरचना Curl_easy *दत्त, पूर्णाङ्क certnum,
                                ध्रुव वर्ण *label,
                                संरचना Curl_asn1Element *elem)
{
  CURLcode परिणाम;
  संरचना dynbuf बहिर्गमनम्;

  curlx_dyn_init(&बहिर्गमनम्, CURL_X509_STR_MAX);

  /* Generate a certificate information record for the public key. */

  परिणाम = ASN1tostr(&बहिर्गमनम्, elem);
  यदि(!परिणाम && दत्त->समुच्चयः.ssl.certinfo)
    परिणाम = ssl_प्रक्षिप_certinfo_dyn(दत्त, certnum, label, &बहिर्गमनम्);
  curlx_dyn_free(&बहिर्गमनम्);
  प्रत्यागम परिणाम;
}

/* return 0 on success, 1 on error */
स्थिर पूर्णाङ्क do_pubkey(संरचना Curl_easy *दत्त, पूर्णाङ्क certnum, ध्रुव वर्ण *algo,
                     संरचना Curl_asn1Element *param,
                     संरचना Curl_asn1Element *pubkey)
{
  संरचना Curl_asn1Element elem;
  संरचना Curl_asn1Element pk;
  ध्रुव uint8_t *p;

  /* Generate all information records for the public key. */

  यदि(curl_strequal(algo, "ecPublicKey")) {
    /*
     * ECC public key is all the data, a value of type BIT STRING mapped to
     * OCTET STRING and should not be parsed as an ASN.1 value.
     */
    ध्रुव size_t dlen = pubkey->अन्तः - pubkey->beg;
    size_t len;
    यदि(dlen < 2)
      /* too small */
      प्रत्यागम 1;
    len = (dlen - 2) * 4;
    यदि(!certnum)
      infof(दत्त, "   ECC Public Key (%zu bits)", len);
    यदि(दत्त->समुच्चयः.ssl.certinfo) {
      वर्ण q[(परिमाण(len) * 8 / 3) + 1];
      (शून्य)curl_msnprintf(q, परिमाण(q), "%zu", len);
      यदि(ssl_प्रक्षिप_certinfo(दत्त, certnum, "ECC Public Key", q))
        प्रत्यागम 1;
    }
    प्रत्यागम do_pubkey_क्षेत्रम्(दत्त, certnum, "ecPublicKey", pubkey) == CURLE_OK
      ? 0 : 1;
  }

  /* Get the public key (single element). */
  यदि(!getASN1Element(&pk, pubkey->beg + 1, pubkey->अन्तः))
    प्रत्यागम 1;

  यदि(curl_strequal(algo, "rsaEncryption")) {
    ध्रुव uint8_t *q;
    size_t len;

    p = getASN1Element(&elem, pk.beg, pk.अन्तः);
    यदि(!p)
      प्रत्यागम 1;

    /* Compute key length. */
    प्रति(q = elem.beg; q < elem.अन्तः && !*q; q++)
      ;
    len = ((elem.अन्तः - q) * 8);
    यदि(len) {
      अचिह्नित पूर्णाङ्क i;
      प्रति(i = *q; !(i & 0x80); i <<= 1)
        len--;
    }
    यदि(len > 32)
      elem.beg = q;     /* Strip leading zero bytes. */
    यदि(!certnum)
      infof(दत्त, "   RSA Public Key (%zu bits)", len);
    यदि(दत्त->समुच्चयः.ssl.certinfo) {
      वर्ण r[(परिमाण(len) * 8 / 3) + 1];
      curl_msnprintf(r, परिमाण(r), "%zu", len);
      यदि(ssl_प्रक्षिप_certinfo(दत्त, certnum, "RSA Public Key", r))
        प्रत्यागम 1;
    }
    /* Generate coefficients. */
    यदि(do_pubkey_क्षेत्रम्(दत्त, certnum, "rsa(n)", &elem))
      प्रत्यागम 1;
    यदि(!getASN1Element(&elem, p, pk.अन्तः))
      प्रत्यागम 1;
    यदि(do_pubkey_क्षेत्रम्(दत्त, certnum, "rsa(e)", &elem))
      प्रत्यागम 1;
  }
  अन्यथा यदि(curl_strequal(algo, "dsa")) {
    p = getASN1Element(&elem, param->beg, param->अन्तः);
    यदि(p) {
      यदि(do_pubkey_क्षेत्रम्(दत्त, certnum, "dsa(p)", &elem))
        प्रत्यागम 1;
      p = getASN1Element(&elem, p, param->अन्तः);
      यदि(p) {
        यदि(do_pubkey_क्षेत्रम्(दत्त, certnum, "dsa(q)", &elem))
          प्रत्यागम 1;
        यदि(getASN1Element(&elem, p, param->अन्तः)) {
          यदि(do_pubkey_क्षेत्रम्(दत्त, certnum, "dsa(g)", &elem))
            प्रत्यागम 1;
          यदि(do_pubkey_क्षेत्रम्(दत्त, certnum, "dsa(pub_key)", &pk))
            प्रत्यागम 1;
        }
      }
    }
  }
  अन्यथा यदि(curl_strequal(algo, "dhpublicnumber")) {
    p = getASN1Element(&elem, param->beg, param->अन्तः);
    यदि(p) {
      यदि(do_pubkey_क्षेत्रम्(दत्त, certnum, "dh(p)", &elem))
        प्रत्यागम 1;
      यदि(getASN1Element(&elem, p, param->अन्तः)) {
        यदि(do_pubkey_क्षेत्रम्(दत्त, certnum, "dh(g)", &elem))
          प्रत्यागम 1;
        यदि(do_pubkey_क्षेत्रम्(दत्त, certnum, "dh(pub_key)", &pk))
          प्रत्यागम 1;
      }
    }
  }
  प्रत्यागम 0;
}

/*
 * Convert an ASN.1 distinguished name into a printable string.
 * Return error.
 */
स्थिर CURLcode DNtostr(संरचना dynbuf *store, संरचना Curl_asn1Element *dn)
{
  प्रत्यागम encodeDN(store, dn);
}

CURLcode Curl_extract_certinfo(संरचना Curl_easy *दत्त,
                               पूर्णाङ्क certnum,
                               ध्रुव uint8_t *beg,
                               ध्रुव uint8_t *अन्तः)
{
  संरचना Curl_X509certificate cert;
  संरचना Curl_asn1Element param;
  वर्ण *certptr;
  size_t clen;
  संरचना dynbuf बहिर्गमनम्;
  CURLcode परिणाम = CURLE_OK;
  अचिह्नित पूर्णाङ्क संस्करणम्_2;
  ध्रुव uint8_t *ptr;
  पूर्णाङ्क rc;

  यदि(!दत्त->समुच्चयः.ssl.certinfo)
    यदि(certnum)
      प्रत्यागम CURLE_OK;

  curlx_dyn_init(&बहिर्गमनम्, CURL_X509_STR_MAX);
  /* Prepare the certificate information for curl_easy_getinfo(). */

  /* Extract the certificate ASN.1 elements. */
  यदि(Curl_parseX509(&cert, beg, अन्तः))
    प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;

  /* Subject. */
  परिणाम = DNtostr(&बहिर्गमनम्, &cert.subject);
  यदि(परिणाम)
    गच्छ समाप्तम्;
  यदि(दत्त->समुच्चयः.ssl.certinfo) {
    परिणाम = ssl_प्रक्षिप_certinfo_dyn(दत्त, certnum, "Subject", &बहिर्गमनम्);
    यदि(परिणाम)
      गच्छ समाप्तम्;
  }
  curlx_dyn_reset(&बहिर्गमनम्);

  /* Issuer. */
  परिणाम = DNtostr(&बहिर्गमनम्, &cert.issuer);
  यदि(परिणाम)
    गच्छ समाप्तम्;
  यदि(दत्त->समुच्चयः.ssl.certinfo) {
    परिणाम = ssl_प्रक्षिप_certinfo_dyn(दत्त, certnum, "Issuer", &बहिर्गमनम्);
    यदि(परिणाम)
      गच्छ समाप्तम्;
  }
  curlx_dyn_reset(&बहिर्गमनम्);

  /* Version (always fits in less than 32 bits). */
  संस्करणम्_2 = 0;
  प्रति(ptr = cert.संस्करणम्_2.beg; ptr < cert.संस्करणम्_2.अन्तः; ptr++)
    संस्करणम्_2 = (संस्करणम्_2 << 8) | *ptr;
  यदि(दत्त->समुच्चयः.ssl.certinfo) {
    परिणाम = curlx_dyn_addf(&बहिर्गमनम्, "%x", संस्करणम्_2);
    यदि(परिणाम)
      गच्छ समाप्तम्;
    परिणाम = ssl_प्रक्षिप_certinfo_dyn(दत्त, certnum, "Version", &बहिर्गमनम्);
    यदि(परिणाम)
      गच्छ समाप्तम्;
    curlx_dyn_reset(&बहिर्गमनम्);
  }

  /* Serial number. */
  परिणाम = ASN1tostr(&बहिर्गमनम्, &cert.serial_सङ्ख्या);
  यदि(परिणाम)
    गच्छ समाप्तम्;
  यदि(दत्त->समुच्चयः.ssl.certinfo) {
    परिणाम = ssl_प्रक्षिप_certinfo_dyn(दत्त, certnum, "Serial Number", &बहिर्गमनम्);
    यदि(परिणाम)
      गच्छ समाप्तम्;
  }
  curlx_dyn_reset(&बहिर्गमनम्);

  /* Signature algorithm .*/
  परिणाम = dumpAlgo(&बहिर्गमनम्, &param, cert.signatureAlgorithm.beg,
                    cert.signatureAlgorithm.अन्तः);
  यदि(परिणाम)
    गच्छ समाप्तम्;
  यदि(दत्त->समुच्चयः.ssl.certinfo) {
    परिणाम = ssl_प्रक्षिप_certinfo_dyn(दत्त, certnum, "Signature Algorithm", &बहिर्गमनम्);
    यदि(परिणाम)
      गच्छ समाप्तम्;
  }
  curlx_dyn_reset(&बहिर्गमनम्);

  /* Start Date. */
  परिणाम = ASN1tostr(&बहिर्गमनम्, &cert.notBefore);
  यदि(परिणाम)
    गच्छ समाप्तम्;
  यदि(दत्त->समुच्चयः.ssl.certinfo) {
    परिणाम = ssl_प्रक्षिप_certinfo_dyn(दत्त, certnum, "Start Date", &बहिर्गमनम्);
    यदि(परिणाम)
      गच्छ समाप्तम्;
  }
  curlx_dyn_reset(&बहिर्गमनम्);

  /* Expire Date. */
  परिणाम = ASN1tostr(&बहिर्गमनम्, &cert.notAfter);
  यदि(परिणाम)
    गच्छ समाप्तम्;
  यदि(दत्त->समुच्चयः.ssl.certinfo) {
    परिणाम = ssl_प्रक्षिप_certinfo_dyn(दत्त, certnum, "Expire Date", &बहिर्गमनम्);
    यदि(परिणाम)
      गच्छ समाप्तम्;
  }
  curlx_dyn_reset(&बहिर्गमनम्);

  /* Public Key Algorithm. */
  परिणाम = dumpAlgo(&बहिर्गमनम्, &param, cert.subject_लौकिकम्_कुञ्चिका_Algorithm.beg,
                    cert.subject_लौकिकम्_कुञ्चिका_Algorithm.अन्तः);
  यदि(परिणाम)
    गच्छ समाप्तम्;
  यदि(दत्त->समुच्चयः.ssl.certinfo) {
    परिणाम = ssl_प्रक्षिप_certinfo_dyn(दत्त, certnum, "Public Key Algorithm",
                                   &बहिर्गमनम्);
    यदि(परिणाम)
      गच्छ समाप्तम्;
  }

  rc = do_pubkey(दत्त, certnum, curlx_dyn_ptr(&बहिर्गमनम्),
                 &param, &cert.subject_लौकिकम्_कुञ्चिका);
  यदि(rc) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः; /* the most likely error */
    गच्छ समाप्तम्;
  }
  curlx_dyn_reset(&बहिर्गमनम्);

  /* Signature. */
  परिणाम = ASN1tostr(&बहिर्गमनम्, &cert.signature);
  यदि(परिणाम)
    गच्छ समाप्तम्;
  यदि(दत्त->समुच्चयः.ssl.certinfo) {
    परिणाम = ssl_प्रक्षिप_certinfo_dyn(दत्त, certnum, "Signature", &बहिर्गमनम्);
    यदि(परिणाम)
      गच्छ समाप्तम्;
  }
  curlx_dyn_reset(&बहिर्गमनम्);

  /* Generate PEM certificate. */
  परिणाम = curlx_base64_encode((ध्रुव uint8_t *)cert.certificate.beg,
                               cert.certificate.अन्तः - cert.certificate.beg,
                               &certptr, &clen);
  यदि(परिणाम)
    गच्छ समाप्तम्;

  /* Generate the final output certificate string. Format is:
     -----BEGIN CERTIFICATE-----\n
     <max 64 base64 characters>\n
     .
     .
     .
     -----END CERTIFICATE-----\n
   */

  curlx_dyn_reset(&बहिर्गमनम्);

  /* Build the certificate string. */
  परिणाम = curlx_dyn_सङ्कलय(&बहिर्गमनम्, "-----BEGIN CERTIFICATE-----\n");
  यदि(!परिणाम) {
    size_t j = 0;

    यावत्(!परिणाम && (j < clen)) {
      size_t chunksize = (clen - j) > 64 ? 64 : (clen - j);
      परिणाम = curlx_dyn_addn(&बहिर्गमनम्, &certptr[j], chunksize);
      यदि(!परिणाम)
        परिणाम = curlx_dyn_addn(&बहिर्गमनम्, "\n", 1);
      j += chunksize;
    }
    यदि(!परिणाम)
      परिणाम = curlx_dyn_सङ्कलय(&बहिर्गमनम्, "-----END CERTIFICATE-----\n");
  }
  curlx_free(certptr);
  यदि(!परिणाम)
    यदि(दत्त->समुच्चयः.ssl.certinfo)
      परिणाम = ssl_प्रक्षिप_certinfo_dyn(दत्त, certnum, "Cert", &बहिर्गमनम्);

समाप्तम्:
  यदि(परिणाम)
    failf(दत्त, "Failed extracting certificate chain");
  curlx_dyn_free(&बहिर्गमनम्);
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* WANT_EXTRACT_CERTINFO */

#यदि_अन्तः /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
