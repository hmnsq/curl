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
#تعریف_کرو WANT_PARSEX509 /* uses Curl_parseX509() */
#جے_انت

#إذا defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_RUSTLS)
#تعریف_کرو WANT_کڈھو_CERTINFO /* uses Curl_extract_certinfo() */
#جے_انت

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
#تعریف_کرو CURL_ASN1_MAX                   ((size_t)0x40000)      /* 256K */

#جے_تعریف_شدہ WANT_کڈھو_CERTINFO

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

#جے_انت /* WANT_EXTRACT_CERTINFO */

/*
 * Lightweight ASN.1 parser.
 * In particular, it does not check for syntactic/lexical errors.
 * It is intended to support certificate information gathering for SSL backends
 * that offer a mean to get certificates as a whole, but do not supply
 * entry points to get particular certificate sub-fields.
 * Please note there is no pretension here to rewrite a full SSL library.
 */

#تعریف_کرو CURL_ASN1_MAX_RECURSIONS    16

ساكن ثابت uint8_t *getASN1Element_(بنية Curl_asn1Element *elem,
                                      ثابت uint8_t *beg, ثابت uint8_t *سرا,
                                      size_t lvl)
{
  uint8_t b;
  size_t len;
  بنية Curl_asn1Element lelem;

  /* Get a single ASN.1 element into `elem', parse ASN.1 string at `beg'
     ending at `end'.
     Returns a pointer in source string after the parsed element, or NULL
     if an error occurs. */
  إذا(!beg || !سرا || beg >= سرا || !*beg ||
     ((size_t)(سرا - beg) > CURL_ASN1_MAX) ||
     lvl++ >= CURL_ASN1_MAX_RECURSIONS)
    أرجع عدم;

  /* Process header byte. */
  elem->سرخی = beg;
  b = *beg++;
  elem->constructed = (b & 0x20) != 0;
  elem->eclass = (b >> 6) & 3;
  b &= 0x1F;
  إذا(b == 0x1F)
    أرجع عدم; /* Long tag values not supported here. */
  elem->ٹیگ = b;

  /* Process length. */
  إذا(beg >= سرا)
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
      طالما(beg < سرا && *beg) {
        beg = getASN1Element_(&lelem, beg, سرا, lvl);
        إذا(!beg)
          أرجع عدم;
      }
      إذا(سرا - beg < 2 || beg[1])       /* EOC must have a zero length. */
        أرجع عدم;
      elem->سرا = beg;                  /* Do not include EOC as content. */
      أرجع beg + 2;                   /* After EOC element. */
    }
    وإلا إذا(b > (size_t)(سرا - beg))
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
  إذا(len > (size_t)(سرا - beg))
    أرجع عدم;  /* Element data does not fit in source. */
  elem->beg = beg;
  elem->سرا = beg + len;
  أرجع elem->سرا;
}

/*
 * @unittest 1657
 */
UNITTEST ثابت uint8_t *getASN1Element(بنية Curl_asn1Element *elem,
                                       ثابت uint8_t *beg, ثابت uint8_t *سرا);
UNITTEST ثابت uint8_t *getASN1Element(بنية Curl_asn1Element *elem,
                                       ثابت uint8_t *beg, ثابت uint8_t *سرا)
{
  أرجع getASN1Element_(elem, beg, سرا, 0);
}

#جے_تعریف_شدہ WANT_کڈھو_CERTINFO

/*
 * Search the null-terminated OID or OID identifier in local table.
 * Return the table entry pointer or NULL if not found.
 */
ساكن ثابت بنية Curl_OID *تلاش_OID(ثابت حرف *oid)
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
                         ثابت uint8_t *beg, ثابت uint8_t *سرا)
{
  uint8_t val;
  إذا(سرا - beg != 1)
    أرجع CURLE_BAD_FUNCTION_دلیل;
  val = *beg;
  إذا((val != 0xff) && (val != 0x00))
    أرجع CURLE_BAD_FUNCTION_دلیل;
  أرجع curlx_dyn_جوڑو(store, val ? "TRUE" : "FALSE");
}

/*
 * Convert an ASN.1 octet string to a printable string.
 *
 * Return error code.
 */
ساكن CURLcode octet2str(بنية dynbuf *store,
                          ثابت uint8_t *beg, ثابت uint8_t *سرا)
{
  CURLcode نتیجہ = CURLE_OK;

  طالما(!نتیجہ && beg < سرا)
    نتیجہ = curlx_dyn_addf(store, "%02x:", *beg++);

  أرجع نتیجہ;
}

ساكن CURLcode bit2str(بنية dynbuf *store,
                        ثابت uint8_t *beg, ثابت uint8_t *سرا)
{
  /* Convert an ASN.1 bit string to a printable string. */

  إذا(beg < سرا) {
    uint8_t بن_ورتیا_بٹ = *beg++;
    إذا(بن_ورتیا_بٹ > 7)
      أرجع CURLE_BAD_FUNCTION_دلیل;
  }
  وإلا
    أرجع CURLE_BAD_FUNCTION_دلیل;
  أرجع octet2str(store, beg, سرا);
}

/*
 * Convert an ASN.1 integer value into its string representation.
 *
 * Returns error.
 */
ساكن CURLcode int2str(بنية dynbuf *store,
                        ثابت uint8_t *beg, ثابت uint8_t *سرا)
{
  uint32_t uval = 0;
  int32_t sval = 0;
  size_t n = سرا - beg;
  size_t i;
  ثابت uint8_t *p = beg;

  إذا(!n)
    أرجع CURLE_BAD_FUNCTION_دلیل;

  إذا(n > 4)
    أرجع octet2str(store, beg, سرا);

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
ساكن CURLcode utf8asn1str(بنية dynbuf *منزل, صحيح type,
                            ثابت uint8_t *from, ثابت uint8_t *سرا)
{
  size_t inlength = سرا - from;
  صحيح ناپ = 1;
  CURLcode نتیجہ = CURLE_OK;

  بدل(type) {
  حالة CURL_ASN1_BMP_STRING:
    ناپ = 2;
    اكسر;
  حالة CURL_ASN1_UNIVERSAL_STRING:
    ناپ = 4;
    اكسر;
  حالة CURL_ASN1_NUMERIC_STRING:
  حالة CURL_ASN1_PRINTABLE_STRING:
  حالة CURL_ASN1_TELETEX_STRING:
  حالة CURL_ASN1_IA5_STRING:
  حالة CURL_ASN1_VISIBLE_STRING:
  حالة CURL_ASN1_UTF8_STRING:
    اكسر;
  افتراضي:
    أرجع CURLE_BAD_FUNCTION_دلیل;  /* Conversion not supported. */
  }

  إذا(inlength % ناپ)
    /* Length inconsistent with character size. */
    أرجع CURLE_BAD_FUNCTION_دلیل;

  إذا(type == CURL_ASN1_UTF8_STRING) {
    /* copy. */
    إذا(inlength)
      نتیجہ = curlx_dyn_addn(منزل, from, inlength);
  }
  وإلا {
    طالما(!نتیجہ && (from < سرا)) {
      حرف buf[4]; /* decode buffer */
      size_t charsize = 1;
      بنانشانی صحيح wc = 0;

      بدل(ناپ) {
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
              أرجع CURLE_WEIRD_SERVER_جواب;
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
      نتیجہ = curlx_dyn_addn(منزل, buf, charsize);
    }
  }
  أرجع نتیجہ;
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
                            ثابت uint8_t *beg, ثابت uint8_t *سرا);
UNITTEST CURLcode encodeOID(بنية dynbuf *store,
                            ثابت uint8_t *beg, ثابت uint8_t *سرا)
{
  uint32_t x;
  uint32_t y;
  CURLcode نتیجہ = CURLE_OK;

  إذا(سرا <= beg)
    أرجع CURLE_BAD_FUNCTION_دلیل;

  /* Process the first two numbers. The initial digit cannot be larger than
     2 */
  y = *beg++;
  إذا(y <= 80)
    x = y / 40;
  وإلا
    x = 2;
  نتیجہ = curlx_dyn_addf(store, "%u", x);
  إذا(نتیجہ)
    أرجع نتیجہ;

  إذا(y & 0x80) {
    uint32_t t = (y & 0x7f); /* the second number */
    إذا(!t)
      /* reject leading 0x80 padding */
      أرجع CURLE_BAD_FUNCTION_دلیل;
    افعل {
      إذا((t & 0xFE000000) || (beg >= سرا))
        أرجع CURLE_BAD_FUNCTION_دلیل;
      y = *beg++;
      t = (t << 7) | (y & 0x7F);
    } طالما(y & 0x80);
    y = t;
  }

  نتیجہ = curlx_dyn_addf(store, ".%u", y - (x * 40));
  إذا(نتیجہ)
    أرجع نتیجہ;

  /* Process the trailing numbers. */
  طالما((beg < سرا) && !نتیجہ) {
    x = 0;
    y = 0;
    افعل {
      /* 0x80 as the first value is invalid since it only adds padding */
      إذا((y & 0x80) && !x)
        أرجع CURLE_BAD_FUNCTION_دلیل;
      وإلا إذا((x & 0xFE000000) || (beg == سرا))
        أرجع CURLE_BAD_FUNCTION_دلیل;
      y = *beg++;
      x = (x << 7) | (y & 0x7F);
    } طالما(y & 0x80);
    نتیجہ = curlx_dyn_addf(store, ".%u", x);
  }
  أرجع نتیجہ;
}

/*
 * Convert an ASN.1 OID into its dotted or symbolic string representation.
 *
 * Return error code.
 */

ساكن CURLcode OID2str(بنية dynbuf *store,
                        ثابت uint8_t *beg, ثابت uint8_t *سرا)
{
  CURLcode نتیجہ = CURLE_OK;
  إذا(beg < سرا) {
    بنية dynbuf buf;
    curlx_dyn_init(&buf, CURL_X509_STR_MAX);
    نتیجہ = encodeOID(&buf, beg, سرا);

    إذا(!نتیجہ) {
      ثابت بنية Curl_OID *op = تلاش_OID(curlx_dyn_ptr(&buf));
      إذا(op)
        نتیجہ = curlx_dyn_جوڑو(store, op->textoid);
      وإلا
        نتیجہ = curlx_dyn_جوڑو(store, curlx_dyn_ptr(&buf));
    }
    curlx_dyn_free(&buf);
  }
  أرجع نتیجہ;
}

/*
 * @unittest 1656
 */
UNITTEST CURLcode GTime2str(بنية dynbuf *store,
                            ثابت حرف *beg, ثابت حرف *سرا);
UNITTEST CURLcode GTime2str(بنية dynbuf *store,
                            ثابت حرف *beg, ثابت حرف *سرا)
{
  ثابت حرف *tzp;
  ثابت حرف *fracp;
  حرف sec1, sec2;
  size_t fracl;
  size_t tzl;
  ثابت حرف *sep = "";

  /* Convert an ASN.1 Generalized time to a printable string.
     Return the dynamically allocated string, or NULL if an error occurs. */

  لكل(fracp = beg; fracp < سرا && ISDIGIT(*fracp); fracp++)
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
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  /* timezone follows optional fractional seconds. */
  tzp = fracp;
  fracl = 0; /* no fractional seconds detected so far */
  إذا(fracp < سرا && (*fracp == '.' || *fracp == ',')) {
    /* Have fractional seconds, e.g. "[.,]\d+". How many? */
    fracp++; /* should be a digit char or BAD ARGUMENT */
    tzp = fracp;
    طالما(tzp < سرا && ISDIGIT(*tzp))
      tzp++;
    إذا(tzp == fracp) /* never looped, no digit after [.,] */
      أرجع CURLE_BAD_FUNCTION_دلیل;
    fracl = tzp - fracp; /* number of fractional sec digits */
    DEBUGASSERT(fracl > 0);
    /* Strip trailing zeroes in fractional seconds.
     * May reduce fracl to 0 if only '0's are present. */
    طالما(fracl && fracp[fracl - 1] == '0')
      fracl--;
  }

  /* Process timezone. */
  إذا(tzp >= سرا) {
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
    tzl = سرا - tzp;
  }
  وإلا {
    sep = " ";
    tzl = سرا - tzp;
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
                          ثابت حرف *beg, ثابت حرف *سرا)
{
  ثابت حرف *tzp;
  size_t tzl;
  ثابت حرف *sec;

  لكل(tzp = beg; tzp < سرا && *tzp >= '0' && *tzp <= '9'; tzp++)
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
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  /* Process timezone. */
  إذا(tzp >= سرا)
    أرجع CURLE_BAD_FUNCTION_دلیل;
  إذا(*tzp == 'Z') {
    tzp = "GMT";
    سرا = tzp + 3;
  }

  tzl = سرا - tzp;
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
  CURLcode نتیجہ = CURLE_BAD_FUNCTION_دلیل;
  صحيح type = elem->ٹیگ;
  إذا(elem->constructed)
    أرجع نتیجہ; /* No conversion of structured elements. */

  بدل(type) {
  حالة CURL_ASN1_BOOLEAN:
    نتیجہ = bool2str(store, elem->beg, elem->سرا);
    اكسر;
  حالة CURL_ASN1_INTEGER:
  حالة CURL_ASN1_ENUMERATED:
    نتیجہ = int2str(store, elem->beg, elem->سرا);
    اكسر;
  حالة CURL_ASN1_ثنائی_ہندسہ_STRING:
    نتیجہ = bit2str(store, elem->beg, elem->سرا);
    اكسر;
  حالة CURL_ASN1_OCTET_STRING:
    نتیجہ = octet2str(store, elem->beg, elem->سرا);
    اكسر;
  حالة CURL_ASN1_NULL:
    نتیجہ = curlx_dyn_addn(store, "", 1);
    اكسر;
  حالة CURL_ASN1_OBJECT_IDENTIFIER:
    نتیجہ = OID2str(store, elem->beg, elem->سرا);
    اكسر;
  حالة CURL_ASN1_UTC_ویلا:
    نتیجہ = UTime2str(store, (ثابت حرف *)elem->beg,
                       (ثابت حرف *)elem->سرا);
    اكسر;
  حالة CURL_ASN1_GENERALIZED_ویلا:
    نتیجہ = GTime2str(store, (ثابت حرف *)elem->beg,
                       (ثابت حرف *)elem->سرا);
    اكسر;
  حالة CURL_ASN1_UTF8_STRING:
  حالة CURL_ASN1_NUMERIC_STRING:
  حالة CURL_ASN1_PRINTABLE_STRING:
  حالة CURL_ASN1_TELETEX_STRING:
  حالة CURL_ASN1_IA5_STRING:
  حالة CURL_ASN1_VISIBLE_STRING:
  حالة CURL_ASN1_UNIVERSAL_STRING:
  حالة CURL_ASN1_BMP_STRING:
    نتیجہ = utf8asn1str(store, type, elem->beg, elem->سرا);
    اكسر;
  }

  أرجع نتیجہ;
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
  بنية Curl_asn1Element قدر;
  ثابت uint8_t *p1;
  ثابت uint8_t *p2;
  ثابت uint8_t *p3;
  ثابت uint8_t *str;
  CURLcode نتیجہ = CURLE_OK;
  bool added = FALSE;
  بنية dynbuf عارضی;
  curlx_dyn_init(&عارضی, CURL_X509_STR_MAX);

  لكل(p1 = dn->beg; p1 < dn->سرا;) {
    p1 = getASN1Element(&rdn, p1, dn->سرا);
    إذا(!p1) {
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
      اذهب error;
    }
    لكل(p2 = rdn.beg; p2 < rdn.سرا;) {
      p2 = getASN1Element(&atv, p2, rdn.سرا);
      إذا(!p2) {
        نتیجہ = CURLE_BAD_FUNCTION_دلیل;
        اذهب error;
      }
      p3 = getASN1Element(&oid, atv.beg, atv.سرا);
      إذا(!p3) {
        نتیجہ = CURLE_BAD_FUNCTION_دلیل;
        اذهب error;
      }
      إذا(!getASN1Element(&قدر, p3, atv.سرا)) {
        نتیجہ = CURLE_BAD_FUNCTION_دلیل;
        اذهب error;
      }
      curlx_dyn_reset(&عارضی);
      نتیجہ = ASN1tostr(&عارضی, &oid);
      إذا(نتیجہ)
        اذهب error;

      str = curlx_dyn_uptr(&عارضی);

      إذا(!str) {
        نتیجہ = CURLE_BAD_FUNCTION_دلیل;
        اذهب error;
      }

      /* Encode delimiter.
         If attribute has a short uppercase name, delimiter is ", ". */
      لكل(p3 = str; ISUPPER(*p3); p3++)
        ;
      إذا(added) {
        إذا(p3 - str > 2)
          نتیجہ = curlx_dyn_addn(store, "/", 1);
        وإلا
          نتیجہ = curlx_dyn_addn(store, ", ", 2);
        إذا(نتیجہ)
          اذهب error;
      }

      /* Encode attribute name. */
      نتیجہ = curlx_dyn_جوڑو(store, (ثابت حرف *)str);
      إذا(نتیجہ)
        اذهب error;

      /* Generate equal sign. */
      نتیجہ = curlx_dyn_addn(store, "=", 1);
      إذا(نتیجہ)
        اذهب error;

      /* Generate value. */
      نتیجہ = ASN1tostr(store, &قدر);
      إذا(نتیجہ)
        اذهب error;
      curlx_dyn_reset(&عارضی);
      added = TRUE; /* use separator for next */
    }
  }
error:
  curlx_dyn_free(&عارضی);

  أرجع نتیجہ;
}

#جے_انت /* WANT_EXTRACT_CERTINFO */

#جے_تعریف_شدہ WANT_PARSEX509
/*
 * ASN.1 parse an X509 certificate into structure subfields.
 * Syntax is assumed to have already been checked by the SSL backend.
 * See RFC 5280.
 */
صحيح Curl_parseX509(بنية Curl_X509certificate *cert,
                   ثابت uint8_t *beg, ثابت uint8_t *سرا)
{
  بنية Curl_asn1Element elem;
  بنية Curl_asn1Element tbsCertificate;
  ثابت uint8_t *ccp;
  ساكن ثابت uint8_t default_نسخہ = 0;  /* v1. */

  memset(cert, 0, حجم(*cert));
  cert->certificate.سرخی = عدم;
  cert->certificate.beg = beg;
  cert->certificate.سرا = سرا;

  /* Get the sequence content. */
  إذا(!getASN1Element(&elem, beg, سرا))
    أرجع -1;  /* Invalid bounds/size. */
  beg = elem.beg;
  سرا = elem.سرا;

  /* Get tbsCertificate. */
  beg = getASN1Element(&tbsCertificate, beg, سرا);
  إذا(!beg)
    أرجع -1;
  /* Skip the signatureAlgorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, سرا);
  إذا(!beg)
    أرجع -1;
  /* Get the signatureValue. */
  إذا(!getASN1Element(&cert->signature, beg, سرا))
    أرجع -1;

  /* Parse TBSCertificate. */
  beg = tbsCertificate.beg;
  سرا = tbsCertificate.سرا;
  /* Get optional version, get serialNumber. */
  cert->نسخہ_2.سرخی = عدم;
  cert->نسخہ_2.beg = &default_نسخہ;
  cert->نسخہ_2.سرا = &default_نسخہ + حجم(default_نسخہ);
  beg = getASN1Element(&elem, beg, سرا);
  إذا(!beg)
    أرجع -1;
  إذا(elem.ٹیگ == 0) {
    إذا(!getASN1Element(&cert->نسخہ_2, elem.beg, elem.سرا))
      أرجع -1;
    beg = getASN1Element(&elem, beg, سرا);
    إذا(!beg)
      أرجع -1;
  }
  cert->serial_نمبر = elem;
  /* Get signature algorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, سرا);
  /* Get issuer. */
  beg = getASN1Element(&cert->issuer, beg, سرا);
  إذا(!beg)
    أرجع -1;
  /* Get notBefore and notAfter. */
  beg = getASN1Element(&elem, beg, سرا);
  إذا(!beg)
    أرجع -1;
  ccp = getASN1Element(&cert->notBefore, elem.beg, elem.سرا);
  إذا(!ccp)
    أرجع -1;
  إذا(!getASN1Element(&cert->notAfter, ccp, elem.سرا))
    أرجع -1;
  /* These two fields MUST consume all of the validity elem */
  إذا(elem.سرا != cert->notAfter.سرا)
    أرجع -1;
  /* Get subject. */
  beg = getASN1Element(&cert->subject, beg, سرا);
  إذا(!beg)
    أرجع -1;
  /* Get subjectPublicKeyAlgorithm and subjectPublicKey. */
  beg = getASN1Element(&cert->subject_عوامی_کلید_Info, beg, سرا);
  إذا(!beg)
    أرجع -1;
  ccp = getASN1Element(&cert->subject_عوامی_کلید_Algorithm,
                       cert->subject_عوامی_کلید_Info.beg,
                       cert->subject_عوامی_کلید_Info.سرا);
  إذا(!ccp)
    أرجع -1;
  إذا(!getASN1Element(&cert->subject_عوامی_کلید, ccp,
                     cert->subject_عوامی_کلید_Info.سرا))
    أرجع -1;
  /* Get optional issuerUniqueID, subjectUniqueID and extensions. */
  cert->issuerUniqueID.ٹیگ = cert->subjectUniqueID.ٹیگ = 0;
  cert->extensions.ٹیگ = elem.ٹیگ = 0;
  cert->issuerUniqueID.سرخی = cert->subjectUniqueID.سرخی = عدم;
  cert->issuerUniqueID.beg = cert->issuerUniqueID.سرا = beg;
  cert->subjectUniqueID.beg = cert->subjectUniqueID.سرا = beg;
  cert->extensions.سرخی = عدم;
  cert->extensions.beg = cert->extensions.سرا = beg;
  إذا(beg < سرا) {
    beg = getASN1Element(&elem, beg, سرا);
    إذا(!beg)
      أرجع -1;
  }
  إذا(elem.ٹیگ == 1) {
    cert->issuerUniqueID = elem;
    إذا(beg < سرا) {
      beg = getASN1Element(&elem, beg, سرا);
      إذا(!beg)
        أرجع -1;
    }
  }
  إذا(elem.ٹیگ == 2) {
    cert->subjectUniqueID = elem;
    إذا(beg < سرا) {
      beg = getASN1Element(&elem, beg, سرا);
      إذا(!beg)
        أرجع -1;
    }
  }
  إذا(elem.ٹیگ == 3)
    إذا(!getASN1Element(&cert->extensions, elem.beg, elem.سرا))
      أرجع -1;
  أرجع 0;
}

#جے_انت /* WANT_PARSEX509 */

#جے_تعریف_شدہ WANT_کڈھو_CERTINFO

ساكن CURLcode dumpAlgo(بنية dynbuf *store,
                         بنية Curl_asn1Element *param,
                         ثابت uint8_t *beg, ثابت uint8_t *سرا)
{
  بنية Curl_asn1Element oid;

  /* Get algorithm parameters and return algorithm name. */

  beg = getASN1Element(&oid, beg, سرا);
  إذا(!beg)
    أرجع CURLE_BAD_FUNCTION_دلیل;
  param->سرخی = عدم;
  param->ٹیگ = 0;
  param->beg = param->سرا = سرا;
  إذا(beg < سرا) {
    ثابت uint8_t *p = getASN1Element(param, beg, سرا);
    إذا(!p)
      أرجع CURLE_BAD_FUNCTION_دلیل;
  }
  أرجع OID2str(store, oid.beg, oid.سرا);
}

/*
 * This is a convenience function for push_certinfo_len that takes a zero
 * terminated value.
 */
ساكن CURLcode ssl_ڈالو_certinfo(بنية Curl_easy *انکڑے,
                                  صحيح certnum,
                                  ثابت حرف *label,
                                  ثابت حرف *قدر)
{
  size_t valuelen = لڑی_لمبائی(قدر);

  أرجع Curl_ssl_ڈالو_certinfo_len(انکڑے, certnum, label, قدر, valuelen);
}

/*
 * This is a convenience function for push_certinfo_len that takes a
 * dynbuf value.
 *
 * It also does the verbose output if !certnum.
 */
ساكن CURLcode ssl_ڈالو_certinfo_dyn(بنية Curl_easy *انکڑے,
                                      صحيح certnum,
                                      ثابت حرف *label,
                                      بنية dynbuf *ptr)
{
  size_t valuelen = curlx_dyn_len(ptr);
  ثابت حرف *قدر = curlx_dyn_ptr(ptr);

  CURLcode نتیجہ = Curl_ssl_ڈالو_certinfo_len(انکڑے, certnum, label,
                                               قدر, valuelen);

  إذا(!certnum && !نتیجہ)
    infof(انکڑے, "   %s: %s", label, قدر);

  أرجع نتیجہ;
}

ساكن CURLcode do_pubkey_میدان(بنية Curl_easy *انکڑے, صحيح certnum,
                                ثابت حرف *label,
                                بنية Curl_asn1Element *elem)
{
  CURLcode نتیجہ;
  بنية dynbuf باہرلا;

  curlx_dyn_init(&باہرلا, CURL_X509_STR_MAX);

  /* Generate a certificate information record for the public key. */

  نتیجہ = ASN1tostr(&باہرلا, elem);
  إذا(!نتیجہ && انکڑے->سیٹ.ssl.certinfo)
    نتیجہ = ssl_ڈالو_certinfo_dyn(انکڑے, certnum, label, &باہرلا);
  curlx_dyn_free(&باہرلا);
  أرجع نتیجہ;
}

/* return 0 on success, 1 on error */
ساكن صحيح do_pubkey(بنية Curl_easy *انکڑے, صحيح certnum, ثابت حرف *algo,
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
    ثابت size_t dlen = pubkey->سرا - pubkey->beg;
    size_t len;
    إذا(dlen < 2)
      /* too small */
      أرجع 1;
    len = (dlen - 2) * 4;
    إذا(!certnum)
      infof(انکڑے, "   ECC Public Key (%zu bits)", len);
    إذا(انکڑے->سیٹ.ssl.certinfo) {
      حرف q[(حجم(len) * 8 / 3) + 1];
      (فراغ)curl_msnprintf(q, حجم(q), "%zu", len);
      إذا(ssl_ڈالو_certinfo(انکڑے, certnum, "ECC Public Key", q))
        أرجع 1;
    }
    أرجع do_pubkey_میدان(انکڑے, certnum, "ecPublicKey", pubkey) == CURLE_OK
      ? 0 : 1;
  }

  /* Get the public key (single element). */
  إذا(!getASN1Element(&pk, pubkey->beg + 1, pubkey->سرا))
    أرجع 1;

  إذا(curl_strequal(algo, "rsaEncryption")) {
    ثابت uint8_t *q;
    size_t len;

    p = getASN1Element(&elem, pk.beg, pk.سرا);
    إذا(!p)
      أرجع 1;

    /* Compute key length. */
    لكل(q = elem.beg; q < elem.سرا && !*q; q++)
      ;
    len = ((elem.سرا - q) * 8);
    إذا(len) {
      بنانشانی صحيح i;
      لكل(i = *q; !(i & 0x80); i <<= 1)
        len--;
    }
    إذا(len > 32)
      elem.beg = q;     /* Strip leading zero bytes. */
    إذا(!certnum)
      infof(انکڑے, "   RSA Public Key (%zu bits)", len);
    إذا(انکڑے->سیٹ.ssl.certinfo) {
      حرف r[(حجم(len) * 8 / 3) + 1];
      curl_msnprintf(r, حجم(r), "%zu", len);
      إذا(ssl_ڈالو_certinfo(انکڑے, certnum, "RSA Public Key", r))
        أرجع 1;
    }
    /* Generate coefficients. */
    إذا(do_pubkey_میدان(انکڑے, certnum, "rsa(n)", &elem))
      أرجع 1;
    إذا(!getASN1Element(&elem, p, pk.سرا))
      أرجع 1;
    إذا(do_pubkey_میدان(انکڑے, certnum, "rsa(e)", &elem))
      أرجع 1;
  }
  وإلا إذا(curl_strequal(algo, "dsa")) {
    p = getASN1Element(&elem, param->beg, param->سرا);
    إذا(p) {
      إذا(do_pubkey_میدان(انکڑے, certnum, "dsa(p)", &elem))
        أرجع 1;
      p = getASN1Element(&elem, p, param->سرا);
      إذا(p) {
        إذا(do_pubkey_میدان(انکڑے, certnum, "dsa(q)", &elem))
          أرجع 1;
        إذا(getASN1Element(&elem, p, param->سرا)) {
          إذا(do_pubkey_میدان(انکڑے, certnum, "dsa(g)", &elem))
            أرجع 1;
          إذا(do_pubkey_میدان(انکڑے, certnum, "dsa(pub_key)", &pk))
            أرجع 1;
        }
      }
    }
  }
  وإلا إذا(curl_strequal(algo, "dhpublicnumber")) {
    p = getASN1Element(&elem, param->beg, param->سرا);
    إذا(p) {
      إذا(do_pubkey_میدان(انکڑے, certnum, "dh(p)", &elem))
        أرجع 1;
      إذا(getASN1Element(&elem, p, param->سرا)) {
        إذا(do_pubkey_میدان(انکڑے, certnum, "dh(g)", &elem))
          أرجع 1;
        إذا(do_pubkey_میدان(انکڑے, certnum, "dh(pub_key)", &pk))
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

CURLcode Curl_کڈھو_certinfo(بنية Curl_easy *انکڑے,
                               صحيح certnum,
                               ثابت uint8_t *beg,
                               ثابت uint8_t *سرا)
{
  بنية Curl_X509certificate cert;
  بنية Curl_asn1Element param;
  حرف *certptr;
  size_t clen;
  بنية dynbuf باہرلا;
  CURLcode نتیجہ = CURLE_OK;
  بنانشانی صحيح نسخہ_2;
  ثابت uint8_t *ptr;
  صحيح rc;

  إذا(!انکڑے->سیٹ.ssl.certinfo)
    إذا(certnum)
      أرجع CURLE_OK;

  curlx_dyn_init(&باہرلا, CURL_X509_STR_MAX);
  /* Prepare the certificate information for curl_easy_getinfo(). */

  /* Extract the certificate ASN.1 elements. */
  إذا(Curl_parseX509(&cert, beg, سرا))
    أرجع CURLE_PEER_FAILED_VERIFICATION;

  /* Subject. */
  نتیجہ = DNtostr(&باہرلا, &cert.subject);
  إذا(نتیجہ)
    اذهب پورا;
  إذا(انکڑے->سیٹ.ssl.certinfo) {
    نتیجہ = ssl_ڈالو_certinfo_dyn(انکڑے, certnum, "Subject", &باہرلا);
    إذا(نتیجہ)
      اذهب پورا;
  }
  curlx_dyn_reset(&باہرلا);

  /* Issuer. */
  نتیجہ = DNtostr(&باہرلا, &cert.issuer);
  إذا(نتیجہ)
    اذهب پورا;
  إذا(انکڑے->سیٹ.ssl.certinfo) {
    نتیجہ = ssl_ڈالو_certinfo_dyn(انکڑے, certnum, "Issuer", &باہرلا);
    إذا(نتیجہ)
      اذهب پورا;
  }
  curlx_dyn_reset(&باہرلا);

  /* Version (always fits in less than 32 bits). */
  نسخہ_2 = 0;
  لكل(ptr = cert.نسخہ_2.beg; ptr < cert.نسخہ_2.سرا; ptr++)
    نسخہ_2 = (نسخہ_2 << 8) | *ptr;
  إذا(انکڑے->سیٹ.ssl.certinfo) {
    نتیجہ = curlx_dyn_addf(&باہرلا, "%x", نسخہ_2);
    إذا(نتیجہ)
      اذهب پورا;
    نتیجہ = ssl_ڈالو_certinfo_dyn(انکڑے, certnum, "Version", &باہرلا);
    إذا(نتیجہ)
      اذهب پورا;
    curlx_dyn_reset(&باہرلا);
  }

  /* Serial number. */
  نتیجہ = ASN1tostr(&باہرلا, &cert.serial_نمبر);
  إذا(نتیجہ)
    اذهب پورا;
  إذا(انکڑے->سیٹ.ssl.certinfo) {
    نتیجہ = ssl_ڈالو_certinfo_dyn(انکڑے, certnum, "Serial Number", &باہرلا);
    إذا(نتیجہ)
      اذهب پورا;
  }
  curlx_dyn_reset(&باہرلا);

  /* Signature algorithm .*/
  نتیجہ = dumpAlgo(&باہرلا, &param, cert.signatureAlgorithm.beg,
                    cert.signatureAlgorithm.سرا);
  إذا(نتیجہ)
    اذهب پورا;
  إذا(انکڑے->سیٹ.ssl.certinfo) {
    نتیجہ = ssl_ڈالو_certinfo_dyn(انکڑے, certnum, "Signature Algorithm", &باہرلا);
    إذا(نتیجہ)
      اذهب پورا;
  }
  curlx_dyn_reset(&باہرلا);

  /* Start Date. */
  نتیجہ = ASN1tostr(&باہرلا, &cert.notBefore);
  إذا(نتیجہ)
    اذهب پورا;
  إذا(انکڑے->سیٹ.ssl.certinfo) {
    نتیجہ = ssl_ڈالو_certinfo_dyn(انکڑے, certnum, "Start Date", &باہرلا);
    إذا(نتیجہ)
      اذهب پورا;
  }
  curlx_dyn_reset(&باہرلا);

  /* Expire Date. */
  نتیجہ = ASN1tostr(&باہرلا, &cert.notAfter);
  إذا(نتیجہ)
    اذهب پورا;
  إذا(انکڑے->سیٹ.ssl.certinfo) {
    نتیجہ = ssl_ڈالو_certinfo_dyn(انکڑے, certnum, "Expire Date", &باہرلا);
    إذا(نتیجہ)
      اذهب پورا;
  }
  curlx_dyn_reset(&باہرلا);

  /* Public Key Algorithm. */
  نتیجہ = dumpAlgo(&باہرلا, &param, cert.subject_عوامی_کلید_Algorithm.beg,
                    cert.subject_عوامی_کلید_Algorithm.سرا);
  إذا(نتیجہ)
    اذهب پورا;
  إذا(انکڑے->سیٹ.ssl.certinfo) {
    نتیجہ = ssl_ڈالو_certinfo_dyn(انکڑے, certnum, "Public Key Algorithm",
                                   &باہرلا);
    إذا(نتیجہ)
      اذهب پورا;
  }

  rc = do_pubkey(انکڑے, certnum, curlx_dyn_ptr(&باہرلا),
                 &param, &cert.subject_عوامی_کلید);
  إذا(rc) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ; /* the most likely error */
    اذهب پورا;
  }
  curlx_dyn_reset(&باہرلا);

  /* Signature. */
  نتیجہ = ASN1tostr(&باہرلا, &cert.signature);
  إذا(نتیجہ)
    اذهب پورا;
  إذا(انکڑے->سیٹ.ssl.certinfo) {
    نتیجہ = ssl_ڈالو_certinfo_dyn(انکڑے, certnum, "Signature", &باہرلا);
    إذا(نتیجہ)
      اذهب پورا;
  }
  curlx_dyn_reset(&باہرلا);

  /* Generate PEM certificate. */
  نتیجہ = curlx_base64_encode((ثابت uint8_t *)cert.certificate.beg,
                               cert.certificate.سرا - cert.certificate.beg,
                               &certptr, &clen);
  إذا(نتیجہ)
    اذهب پورا;

  /* Generate the final output certificate string. Format is:
     -----BEGIN CERTIFICATE-----\n
     <max 64 base64 characters>\n
     .
     .
     .
     -----END CERTIFICATE-----\n
   */

  curlx_dyn_reset(&باہرلا);

  /* Build the certificate string. */
  نتیجہ = curlx_dyn_جوڑو(&باہرلا, "-----BEGIN CERTIFICATE-----\n");
  إذا(!نتیجہ) {
    size_t j = 0;

    طالما(!نتیجہ && (j < clen)) {
      size_t chunksize = (clen - j) > 64 ? 64 : (clen - j);
      نتیجہ = curlx_dyn_addn(&باہرلا, &certptr[j], chunksize);
      إذا(!نتیجہ)
        نتیجہ = curlx_dyn_addn(&باہرلا, "\n", 1);
      j += chunksize;
    }
    إذا(!نتیجہ)
      نتیجہ = curlx_dyn_جوڑو(&باہرلا, "-----END CERTIFICATE-----\n");
  }
  curlx_free(certptr);
  إذا(!نتیجہ)
    إذا(انکڑے->سیٹ.ssl.certinfo)
      نتیجہ = ssl_ڈالو_certinfo_dyn(انکڑے, certnum, "Cert", &باہرلا);

پورا:
  إذا(نتیجہ)
    failf(انکڑے, "Failed extracting certificate chain");
  curlx_dyn_free(&باہرلا);
  أرجع نتیجہ;
}

#جے_انت /* WANT_EXTRACT_CERTINFO */

#جے_انت /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
