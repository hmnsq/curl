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
#وصف_ڪريو WANT_PARSEX509 /* uses Curl_parseX509() */
#جيڪڏهن_پورو

#إذا defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_RUSTLS)
#وصف_ڪريو WANT_ڪڍو_CERTINFO /* uses Curl_extract_certinfo() */
#جيڪڏهن_پورو

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
#وصف_ڪريو CURL_ASN1_MAX                   ((size_t)0x40000)      /* 256K */

#جيڪڏهن_وصف_ٿيل WANT_ڪڍو_CERTINFO

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

#جيڪڏهن_پورو /* WANT_EXTRACT_CERTINFO */

/*
 * Lightweight ASN.1 parser.
 * In particular, it does not check for syntactic/lexical errors.
 * It is intended to support certificate information gathering for SSL backends
 * that offer a mean to get certificates as a whole, but do not supply
 * entry points to get particular certificate sub-fields.
 * Please note there is no pretension here to rewrite a full SSL library.
 */

#وصف_ڪريو CURL_ASN1_MAX_RECURSIONS    16

ساكن ثابت uint8_t *getASN1Element_(بنية Curl_asn1Element *elem,
                                      ثابت uint8_t *beg, ثابت uint8_t *پڇاڙي,
                                      size_t lvl)
{
  uint8_t b;
  size_t len;
  بنية Curl_asn1Element lelem;

  /* Get a single ASN.1 element into `elem', parse ASN.1 string at `beg'
     ending at `end'.
     Returns a pointer in source string after the parsed element, or NULL
     if an error occurs. */
  إذا(!beg || !پڇاڙي || beg >= پڇاڙي || !*beg ||
     ((size_t)(پڇاڙي - beg) > CURL_ASN1_MAX) ||
     lvl++ >= CURL_ASN1_MAX_RECURSIONS)
    أرجع عدم;

  /* Process header byte. */
  elem->سرو = beg;
  b = *beg++;
  elem->constructed = (b & 0x20) != 0;
  elem->eclass = (b >> 6) & 3;
  b &= 0x1F;
  إذا(b == 0x1F)
    أرجع عدم; /* Long tag values not supported here. */
  elem->ٽيگ = b;

  /* Process length. */
  إذا(beg >= پڇاڙي)
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
      طالما(beg < پڇاڙي && *beg) {
        beg = getASN1Element_(&lelem, beg, پڇاڙي, lvl);
        إذا(!beg)
          أرجع عدم;
      }
      إذا(پڇاڙي - beg < 2 || beg[1])       /* EOC must have a zero length. */
        أرجع عدم;
      elem->پڇاڙي = beg;                  /* Do not include EOC as content. */
      أرجع beg + 2;                   /* After EOC element. */
    }
    وإلا إذا(b > (size_t)(پڇاڙي - beg))
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
  إذا(len > (size_t)(پڇاڙي - beg))
    أرجع عدم;  /* Element data does not fit in source. */
  elem->beg = beg;
  elem->پڇاڙي = beg + len;
  أرجع elem->پڇاڙي;
}

/*
 * @unittest 1657
 */
UNITTEST ثابت uint8_t *getASN1Element(بنية Curl_asn1Element *elem,
                                       ثابت uint8_t *beg, ثابت uint8_t *پڇاڙي);
UNITTEST ثابت uint8_t *getASN1Element(بنية Curl_asn1Element *elem,
                                       ثابت uint8_t *beg, ثابت uint8_t *پڇاڙي)
{
  أرجع getASN1Element_(elem, beg, پڇاڙي, 0);
}

#جيڪڏهن_وصف_ٿيل WANT_ڪڍو_CERTINFO

/*
 * Search the null-terminated OID or OID identifier in local table.
 * Return the table entry pointer or NULL if not found.
 */
ساكن ثابت بنية Curl_OID *ڳولا_OID(ثابت حرف *oid)
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
                         ثابت uint8_t *beg, ثابت uint8_t *پڇاڙي)
{
  uint8_t val;
  إذا(پڇاڙي - beg != 1)
    أرجع CURLE_BAD_FUNCTION_دليل;
  val = *beg;
  إذا((val != 0xff) && (val != 0x00))
    أرجع CURLE_BAD_FUNCTION_دليل;
  أرجع curlx_dyn_جوڙيو(store, val ? "TRUE" : "FALSE");
}

/*
 * Convert an ASN.1 octet string to a printable string.
 *
 * Return error code.
 */
ساكن CURLcode octet2str(بنية dynbuf *store,
                          ثابت uint8_t *beg, ثابت uint8_t *پڇاڙي)
{
  CURLcode نتيجو = CURLE_OK;

  طالما(!نتيجو && beg < پڇاڙي)
    نتيجو = curlx_dyn_addf(store, "%02x:", *beg++);

  أرجع نتيجو;
}

ساكن CURLcode bit2str(بنية dynbuf *store,
                        ثابت uint8_t *beg, ثابت uint8_t *پڇاڙي)
{
  /* Convert an ASN.1 bit string to a printable string. */

  إذا(beg < پڇاڙي) {
    uint8_t اڻ_استعمال_ٿيل_بٽ = *beg++;
    إذا(اڻ_استعمال_ٿيل_بٽ > 7)
      أرجع CURLE_BAD_FUNCTION_دليل;
  }
  وإلا
    أرجع CURLE_BAD_FUNCTION_دليل;
  أرجع octet2str(store, beg, پڇاڙي);
}

/*
 * Convert an ASN.1 integer value into its string representation.
 *
 * Returns error.
 */
ساكن CURLcode int2str(بنية dynbuf *store,
                        ثابت uint8_t *beg, ثابت uint8_t *پڇاڙي)
{
  uint32_t uval = 0;
  int32_t sval = 0;
  size_t n = پڇاڙي - beg;
  size_t i;
  ثابت uint8_t *p = beg;

  إذا(!n)
    أرجع CURLE_BAD_FUNCTION_دليل;

  إذا(n > 4)
    أرجع octet2str(store, beg, پڇاڙي);

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
                            ثابت uint8_t *from, ثابت uint8_t *پڇاڙي)
{
  size_t inlength = پڇاڙي - from;
  صحيح سائيز = 1;
  CURLcode نتيجو = CURLE_OK;

  بدل(type) {
  حالة CURL_ASN1_BMP_STRING:
    سائيز = 2;
    اكسر;
  حالة CURL_ASN1_UNIVERSAL_STRING:
    سائيز = 4;
    اكسر;
  حالة CURL_ASN1_NUMERIC_STRING:
  حالة CURL_ASN1_PRINTABLE_STRING:
  حالة CURL_ASN1_TELETEX_STRING:
  حالة CURL_ASN1_IA5_STRING:
  حالة CURL_ASN1_VISIBLE_STRING:
  حالة CURL_ASN1_UTF8_STRING:
    اكسر;
  افتراضي:
    أرجع CURLE_BAD_FUNCTION_دليل;  /* Conversion not supported. */
  }

  إذا(inlength % سائيز)
    /* Length inconsistent with character size. */
    أرجع CURLE_BAD_FUNCTION_دليل;

  إذا(type == CURL_ASN1_UTF8_STRING) {
    /* copy. */
    إذا(inlength)
      نتيجو = curlx_dyn_addn(منزل, from, inlength);
  }
  وإلا {
    طالما(!نتيجو && (from < پڇاڙي)) {
      حرف buf[4]; /* decode buffer */
      size_t charsize = 1;
      بنانشان صحيح wc = 0;

      بدل(سائيز) {
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
      نتيجو = curlx_dyn_addn(منزل, buf, charsize);
    }
  }
  أرجع نتيجو;
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
                            ثابت uint8_t *beg, ثابت uint8_t *پڇاڙي);
UNITTEST CURLcode encodeOID(بنية dynbuf *store,
                            ثابت uint8_t *beg, ثابت uint8_t *پڇاڙي)
{
  uint32_t x;
  uint32_t y;
  CURLcode نتيجو = CURLE_OK;

  إذا(پڇاڙي <= beg)
    أرجع CURLE_BAD_FUNCTION_دليل;

  /* Process the first two numbers. The initial digit cannot be larger than
     2 */
  y = *beg++;
  إذا(y <= 80)
    x = y / 40;
  وإلا
    x = 2;
  نتيجو = curlx_dyn_addf(store, "%u", x);
  إذا(نتيجو)
    أرجع نتيجو;

  إذا(y & 0x80) {
    uint32_t t = (y & 0x7f); /* the second number */
    إذا(!t)
      /* reject leading 0x80 padding */
      أرجع CURLE_BAD_FUNCTION_دليل;
    افعل {
      إذا((t & 0xFE000000) || (beg >= پڇاڙي))
        أرجع CURLE_BAD_FUNCTION_دليل;
      y = *beg++;
      t = (t << 7) | (y & 0x7F);
    } طالما(y & 0x80);
    y = t;
  }

  نتيجو = curlx_dyn_addf(store, ".%u", y - (x * 40));
  إذا(نتيجو)
    أرجع نتيجو;

  /* Process the trailing numbers. */
  طالما((beg < پڇاڙي) && !نتيجو) {
    x = 0;
    y = 0;
    افعل {
      /* 0x80 as the first value is invalid since it only adds padding */
      إذا((y & 0x80) && !x)
        أرجع CURLE_BAD_FUNCTION_دليل;
      وإلا إذا((x & 0xFE000000) || (beg == پڇاڙي))
        أرجع CURLE_BAD_FUNCTION_دليل;
      y = *beg++;
      x = (x << 7) | (y & 0x7F);
    } طالما(y & 0x80);
    نتيجو = curlx_dyn_addf(store, ".%u", x);
  }
  أرجع نتيجو;
}

/*
 * Convert an ASN.1 OID into its dotted or symbolic string representation.
 *
 * Return error code.
 */

ساكن CURLcode OID2str(بنية dynbuf *store,
                        ثابت uint8_t *beg, ثابت uint8_t *پڇاڙي)
{
  CURLcode نتيجو = CURLE_OK;
  إذا(beg < پڇاڙي) {
    بنية dynbuf buf;
    curlx_dyn_init(&buf, CURL_X509_STR_MAX);
    نتيجو = encodeOID(&buf, beg, پڇاڙي);

    إذا(!نتيجو) {
      ثابت بنية Curl_OID *op = ڳولا_OID(curlx_dyn_ptr(&buf));
      إذا(op)
        نتيجو = curlx_dyn_جوڙيو(store, op->textoid);
      وإلا
        نتيجو = curlx_dyn_جوڙيو(store, curlx_dyn_ptr(&buf));
    }
    curlx_dyn_free(&buf);
  }
  أرجع نتيجو;
}

/*
 * @unittest 1656
 */
UNITTEST CURLcode GTime2str(بنية dynbuf *store,
                            ثابت حرف *beg, ثابت حرف *پڇاڙي);
UNITTEST CURLcode GTime2str(بنية dynbuf *store,
                            ثابت حرف *beg, ثابت حرف *پڇاڙي)
{
  ثابت حرف *tzp;
  ثابت حرف *fracp;
  حرف sec1, sec2;
  size_t fracl;
  size_t tzl;
  ثابت حرف *sep = "";

  /* Convert an ASN.1 Generalized time to a printable string.
     Return the dynamically allocated string, or NULL if an error occurs. */

  لكل(fracp = beg; fracp < پڇاڙي && ISDIGIT(*fracp); fracp++)
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
    أرجع CURLE_BAD_FUNCTION_دليل;
  }

  /* timezone follows optional fractional seconds. */
  tzp = fracp;
  fracl = 0; /* no fractional seconds detected so far */
  إذا(fracp < پڇاڙي && (*fracp == '.' || *fracp == ',')) {
    /* Have fractional seconds, e.g. "[.,]\d+". How many? */
    fracp++; /* should be a digit char or BAD ARGUMENT */
    tzp = fracp;
    طالما(tzp < پڇاڙي && ISDIGIT(*tzp))
      tzp++;
    إذا(tzp == fracp) /* never looped, no digit after [.,] */
      أرجع CURLE_BAD_FUNCTION_دليل;
    fracl = tzp - fracp; /* number of fractional sec digits */
    DEBUGASSERT(fracl > 0);
    /* Strip trailing zeroes in fractional seconds.
     * May reduce fracl to 0 if only '0's are present. */
    طالما(fracl && fracp[fracl - 1] == '0')
      fracl--;
  }

  /* Process timezone. */
  إذا(tzp >= پڇاڙي) {
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
    tzl = پڇاڙي - tzp;
  }
  وإلا {
    sep = " ";
    tzl = پڇاڙي - tzp;
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
                          ثابت حرف *beg, ثابت حرف *پڇاڙي)
{
  ثابت حرف *tzp;
  size_t tzl;
  ثابت حرف *sec;

  لكل(tzp = beg; tzp < پڇاڙي && *tzp >= '0' && *tzp <= '9'; tzp++)
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
    أرجع CURLE_BAD_FUNCTION_دليل;
  }

  /* Process timezone. */
  إذا(tzp >= پڇاڙي)
    أرجع CURLE_BAD_FUNCTION_دليل;
  إذا(*tzp == 'Z') {
    tzp = "GMT";
    پڇاڙي = tzp + 3;
  }

  tzl = پڇاڙي - tzp;
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
  CURLcode نتيجو = CURLE_BAD_FUNCTION_دليل;
  صحيح type = elem->ٽيگ;
  إذا(elem->constructed)
    أرجع نتيجو; /* No conversion of structured elements. */

  بدل(type) {
  حالة CURL_ASN1_BOOLEAN:
    نتيجو = bool2str(store, elem->beg, elem->پڇاڙي);
    اكسر;
  حالة CURL_ASN1_INTEGER:
  حالة CURL_ASN1_ENUMERATED:
    نتيجو = int2str(store, elem->beg, elem->پڇاڙي);
    اكسر;
  حالة CURL_ASN1_ٻٽو_انگ_STRING:
    نتيجو = bit2str(store, elem->beg, elem->پڇاڙي);
    اكسر;
  حالة CURL_ASN1_OCTET_STRING:
    نتيجو = octet2str(store, elem->beg, elem->پڇاڙي);
    اكسر;
  حالة CURL_ASN1_NULL:
    نتيجو = curlx_dyn_addn(store, "", 1);
    اكسر;
  حالة CURL_ASN1_OBJECT_IDENTIFIER:
    نتيجو = OID2str(store, elem->beg, elem->پڇاڙي);
    اكسر;
  حالة CURL_ASN1_UTC_وقت:
    نتيجو = UTime2str(store, (ثابت حرف *)elem->beg,
                       (ثابت حرف *)elem->پڇاڙي);
    اكسر;
  حالة CURL_ASN1_GENERALIZED_وقت:
    نتيجو = GTime2str(store, (ثابت حرف *)elem->beg,
                       (ثابت حرف *)elem->پڇاڙي);
    اكسر;
  حالة CURL_ASN1_UTF8_STRING:
  حالة CURL_ASN1_NUMERIC_STRING:
  حالة CURL_ASN1_PRINTABLE_STRING:
  حالة CURL_ASN1_TELETEX_STRING:
  حالة CURL_ASN1_IA5_STRING:
  حالة CURL_ASN1_VISIBLE_STRING:
  حالة CURL_ASN1_UNIVERSAL_STRING:
  حالة CURL_ASN1_BMP_STRING:
    نتيجو = utf8asn1str(store, type, elem->beg, elem->پڇاڙي);
    اكسر;
  }

  أرجع نتيجو;
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
  CURLcode نتيجو = CURLE_OK;
  bool added = FALSE;
  بنية dynbuf عارضي;
  curlx_dyn_init(&عارضي, CURL_X509_STR_MAX);

  لكل(p1 = dn->beg; p1 < dn->پڇاڙي;) {
    p1 = getASN1Element(&rdn, p1, dn->پڇاڙي);
    إذا(!p1) {
      نتيجو = CURLE_BAD_FUNCTION_دليل;
      اذهب error;
    }
    لكل(p2 = rdn.beg; p2 < rdn.پڇاڙي;) {
      p2 = getASN1Element(&atv, p2, rdn.پڇاڙي);
      إذا(!p2) {
        نتيجو = CURLE_BAD_FUNCTION_دليل;
        اذهب error;
      }
      p3 = getASN1Element(&oid, atv.beg, atv.پڇاڙي);
      إذا(!p3) {
        نتيجو = CURLE_BAD_FUNCTION_دليل;
        اذهب error;
      }
      إذا(!getASN1Element(&قدر, p3, atv.پڇاڙي)) {
        نتيجو = CURLE_BAD_FUNCTION_دليل;
        اذهب error;
      }
      curlx_dyn_reset(&عارضي);
      نتيجو = ASN1tostr(&عارضي, &oid);
      إذا(نتيجو)
        اذهب error;

      str = curlx_dyn_uptr(&عارضي);

      إذا(!str) {
        نتيجو = CURLE_BAD_FUNCTION_دليل;
        اذهب error;
      }

      /* Encode delimiter.
         If attribute has a short uppercase name, delimiter is ", ". */
      لكل(p3 = str; ISUPPER(*p3); p3++)
        ;
      إذا(added) {
        إذا(p3 - str > 2)
          نتيجو = curlx_dyn_addn(store, "/", 1);
        وإلا
          نتيجو = curlx_dyn_addn(store, ", ", 2);
        إذا(نتيجو)
          اذهب error;
      }

      /* Encode attribute name. */
      نتيجو = curlx_dyn_جوڙيو(store, (ثابت حرف *)str);
      إذا(نتيجو)
        اذهب error;

      /* Generate equal sign. */
      نتيجو = curlx_dyn_addn(store, "=", 1);
      إذا(نتيجو)
        اذهب error;

      /* Generate value. */
      نتيجو = ASN1tostr(store, &قدر);
      إذا(نتيجو)
        اذهب error;
      curlx_dyn_reset(&عارضي);
      added = TRUE; /* use separator for next */
    }
  }
error:
  curlx_dyn_free(&عارضي);

  أرجع نتيجو;
}

#جيڪڏهن_پورو /* WANT_EXTRACT_CERTINFO */

#جيڪڏهن_وصف_ٿيل WANT_PARSEX509
/*
 * ASN.1 parse an X509 certificate into structure subfields.
 * Syntax is assumed to have already been checked by the SSL backend.
 * See RFC 5280.
 */
صحيح Curl_parseX509(بنية Curl_X509certificate *cert,
                   ثابت uint8_t *beg, ثابت uint8_t *پڇاڙي)
{
  بنية Curl_asn1Element elem;
  بنية Curl_asn1Element tbsCertificate;
  ثابت uint8_t *ccp;
  ساكن ثابت uint8_t default_نسخو = 0;  /* v1. */

  memset(cert, 0, حجم(*cert));
  cert->certificate.سرو = عدم;
  cert->certificate.beg = beg;
  cert->certificate.پڇاڙي = پڇاڙي;

  /* Get the sequence content. */
  إذا(!getASN1Element(&elem, beg, پڇاڙي))
    أرجع -1;  /* Invalid bounds/size. */
  beg = elem.beg;
  پڇاڙي = elem.پڇاڙي;

  /* Get tbsCertificate. */
  beg = getASN1Element(&tbsCertificate, beg, پڇاڙي);
  إذا(!beg)
    أرجع -1;
  /* Skip the signatureAlgorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, پڇاڙي);
  إذا(!beg)
    أرجع -1;
  /* Get the signatureValue. */
  إذا(!getASN1Element(&cert->signature, beg, پڇاڙي))
    أرجع -1;

  /* Parse TBSCertificate. */
  beg = tbsCertificate.beg;
  پڇاڙي = tbsCertificate.پڇاڙي;
  /* Get optional version, get serialNumber. */
  cert->نسخو_2.سرو = عدم;
  cert->نسخو_2.beg = &default_نسخو;
  cert->نسخو_2.پڇاڙي = &default_نسخو + حجم(default_نسخو);
  beg = getASN1Element(&elem, beg, پڇاڙي);
  إذا(!beg)
    أرجع -1;
  إذا(elem.ٽيگ == 0) {
    إذا(!getASN1Element(&cert->نسخو_2, elem.beg, elem.پڇاڙي))
      أرجع -1;
    beg = getASN1Element(&elem, beg, پڇاڙي);
    إذا(!beg)
      أرجع -1;
  }
  cert->serial_عدد = elem;
  /* Get signature algorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, پڇاڙي);
  /* Get issuer. */
  beg = getASN1Element(&cert->issuer, beg, پڇاڙي);
  إذا(!beg)
    أرجع -1;
  /* Get notBefore and notAfter. */
  beg = getASN1Element(&elem, beg, پڇاڙي);
  إذا(!beg)
    أرجع -1;
  ccp = getASN1Element(&cert->notBefore, elem.beg, elem.پڇاڙي);
  إذا(!ccp)
    أرجع -1;
  إذا(!getASN1Element(&cert->notAfter, ccp, elem.پڇاڙي))
    أرجع -1;
  /* These two fields MUST consume all of the validity elem */
  إذا(elem.پڇاڙي != cert->notAfter.پڇاڙي)
    أرجع -1;
  /* Get subject. */
  beg = getASN1Element(&cert->subject, beg, پڇاڙي);
  إذا(!beg)
    أرجع -1;
  /* Get subjectPublicKeyAlgorithm and subjectPublicKey. */
  beg = getASN1Element(&cert->subject_عوامي_ڪنجي_Info, beg, پڇاڙي);
  إذا(!beg)
    أرجع -1;
  ccp = getASN1Element(&cert->subject_عوامي_ڪنجي_Algorithm,
                       cert->subject_عوامي_ڪنجي_Info.beg,
                       cert->subject_عوامي_ڪنجي_Info.پڇاڙي);
  إذا(!ccp)
    أرجع -1;
  إذا(!getASN1Element(&cert->subject_عوامي_ڪنجي, ccp,
                     cert->subject_عوامي_ڪنجي_Info.پڇاڙي))
    أرجع -1;
  /* Get optional issuerUniqueID, subjectUniqueID and extensions. */
  cert->issuerUniqueID.ٽيگ = cert->subjectUniqueID.ٽيگ = 0;
  cert->extensions.ٽيگ = elem.ٽيگ = 0;
  cert->issuerUniqueID.سرو = cert->subjectUniqueID.سرو = عدم;
  cert->issuerUniqueID.beg = cert->issuerUniqueID.پڇاڙي = beg;
  cert->subjectUniqueID.beg = cert->subjectUniqueID.پڇاڙي = beg;
  cert->extensions.سرو = عدم;
  cert->extensions.beg = cert->extensions.پڇاڙي = beg;
  إذا(beg < پڇاڙي) {
    beg = getASN1Element(&elem, beg, پڇاڙي);
    إذا(!beg)
      أرجع -1;
  }
  إذا(elem.ٽيگ == 1) {
    cert->issuerUniqueID = elem;
    إذا(beg < پڇاڙي) {
      beg = getASN1Element(&elem, beg, پڇاڙي);
      إذا(!beg)
        أرجع -1;
    }
  }
  إذا(elem.ٽيگ == 2) {
    cert->subjectUniqueID = elem;
    إذا(beg < پڇاڙي) {
      beg = getASN1Element(&elem, beg, پڇاڙي);
      إذا(!beg)
        أرجع -1;
    }
  }
  إذا(elem.ٽيگ == 3)
    إذا(!getASN1Element(&cert->extensions, elem.beg, elem.پڇاڙي))
      أرجع -1;
  أرجع 0;
}

#جيڪڏهن_پورو /* WANT_PARSEX509 */

#جيڪڏهن_وصف_ٿيل WANT_ڪڍو_CERTINFO

ساكن CURLcode dumpAlgo(بنية dynbuf *store,
                         بنية Curl_asn1Element *param,
                         ثابت uint8_t *beg, ثابت uint8_t *پڇاڙي)
{
  بنية Curl_asn1Element oid;

  /* Get algorithm parameters and return algorithm name. */

  beg = getASN1Element(&oid, beg, پڇاڙي);
  إذا(!beg)
    أرجع CURLE_BAD_FUNCTION_دليل;
  param->سرو = عدم;
  param->ٽيگ = 0;
  param->beg = param->پڇاڙي = پڇاڙي;
  إذا(beg < پڇاڙي) {
    ثابت uint8_t *p = getASN1Element(param, beg, پڇاڙي);
    إذا(!p)
      أرجع CURLE_BAD_FUNCTION_دليل;
  }
  أرجع OID2str(store, oid.beg, oid.پڇاڙي);
}

/*
 * This is a convenience function for push_certinfo_len that takes a zero
 * terminated value.
 */
ساكن CURLcode ssl_ڌڪو_certinfo(بنية Curl_easy *معلومات,
                                  صحيح certnum,
                                  ثابت حرف *label,
                                  ثابت حرف *قدر)
{
  size_t valuelen = لڙي_ڊيگهه(قدر);

  أرجع Curl_ssl_ڌڪو_certinfo_len(معلومات, certnum, label, قدر, valuelen);
}

/*
 * This is a convenience function for push_certinfo_len that takes a
 * dynbuf value.
 *
 * It also does the verbose output if !certnum.
 */
ساكن CURLcode ssl_ڌڪو_certinfo_dyn(بنية Curl_easy *معلومات,
                                      صحيح certnum,
                                      ثابت حرف *label,
                                      بنية dynbuf *ptr)
{
  size_t valuelen = curlx_dyn_len(ptr);
  ثابت حرف *قدر = curlx_dyn_ptr(ptr);

  CURLcode نتيجو = Curl_ssl_ڌڪو_certinfo_len(معلومات, certnum, label,
                                               قدر, valuelen);

  إذا(!certnum && !نتيجو)
    infof(معلومات, "   %s: %s", label, قدر);

  أرجع نتيجو;
}

ساكن CURLcode do_pubkey_ميدان(بنية Curl_easy *معلومات, صحيح certnum,
                                ثابت حرف *label,
                                بنية Curl_asn1Element *elem)
{
  CURLcode نتيجو;
  بنية dynbuf ٻاهريون;

  curlx_dyn_init(&ٻاهريون, CURL_X509_STR_MAX);

  /* Generate a certificate information record for the public key. */

  نتيجو = ASN1tostr(&ٻاهريون, elem);
  إذا(!نتيجو && معلومات->سيٽ.ssl.certinfo)
    نتيجو = ssl_ڌڪو_certinfo_dyn(معلومات, certnum, label, &ٻاهريون);
  curlx_dyn_free(&ٻاهريون);
  أرجع نتيجو;
}

/* return 0 on success, 1 on error */
ساكن صحيح do_pubkey(بنية Curl_easy *معلومات, صحيح certnum, ثابت حرف *algo,
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
    ثابت size_t dlen = pubkey->پڇاڙي - pubkey->beg;
    size_t len;
    إذا(dlen < 2)
      /* too small */
      أرجع 1;
    len = (dlen - 2) * 4;
    إذا(!certnum)
      infof(معلومات, "   ECC Public Key (%zu bits)", len);
    إذا(معلومات->سيٽ.ssl.certinfo) {
      حرف q[(حجم(len) * 8 / 3) + 1];
      (فراغ)curl_msnprintf(q, حجم(q), "%zu", len);
      إذا(ssl_ڌڪو_certinfo(معلومات, certnum, "ECC Public Key", q))
        أرجع 1;
    }
    أرجع do_pubkey_ميدان(معلومات, certnum, "ecPublicKey", pubkey) == CURLE_OK
      ? 0 : 1;
  }

  /* Get the public key (single element). */
  إذا(!getASN1Element(&pk, pubkey->beg + 1, pubkey->پڇاڙي))
    أرجع 1;

  إذا(curl_strequal(algo, "rsaEncryption")) {
    ثابت uint8_t *q;
    size_t len;

    p = getASN1Element(&elem, pk.beg, pk.پڇاڙي);
    إذا(!p)
      أرجع 1;

    /* Compute key length. */
    لكل(q = elem.beg; q < elem.پڇاڙي && !*q; q++)
      ;
    len = ((elem.پڇاڙي - q) * 8);
    إذا(len) {
      بنانشان صحيح i;
      لكل(i = *q; !(i & 0x80); i <<= 1)
        len--;
    }
    إذا(len > 32)
      elem.beg = q;     /* Strip leading zero bytes. */
    إذا(!certnum)
      infof(معلومات, "   RSA Public Key (%zu bits)", len);
    إذا(معلومات->سيٽ.ssl.certinfo) {
      حرف r[(حجم(len) * 8 / 3) + 1];
      curl_msnprintf(r, حجم(r), "%zu", len);
      إذا(ssl_ڌڪو_certinfo(معلومات, certnum, "RSA Public Key", r))
        أرجع 1;
    }
    /* Generate coefficients. */
    إذا(do_pubkey_ميدان(معلومات, certnum, "rsa(n)", &elem))
      أرجع 1;
    إذا(!getASN1Element(&elem, p, pk.پڇاڙي))
      أرجع 1;
    إذا(do_pubkey_ميدان(معلومات, certnum, "rsa(e)", &elem))
      أرجع 1;
  }
  وإلا إذا(curl_strequal(algo, "dsa")) {
    p = getASN1Element(&elem, param->beg, param->پڇاڙي);
    إذا(p) {
      إذا(do_pubkey_ميدان(معلومات, certnum, "dsa(p)", &elem))
        أرجع 1;
      p = getASN1Element(&elem, p, param->پڇاڙي);
      إذا(p) {
        إذا(do_pubkey_ميدان(معلومات, certnum, "dsa(q)", &elem))
          أرجع 1;
        إذا(getASN1Element(&elem, p, param->پڇاڙي)) {
          إذا(do_pubkey_ميدان(معلومات, certnum, "dsa(g)", &elem))
            أرجع 1;
          إذا(do_pubkey_ميدان(معلومات, certnum, "dsa(pub_key)", &pk))
            أرجع 1;
        }
      }
    }
  }
  وإلا إذا(curl_strequal(algo, "dhpublicnumber")) {
    p = getASN1Element(&elem, param->beg, param->پڇاڙي);
    إذا(p) {
      إذا(do_pubkey_ميدان(معلومات, certnum, "dh(p)", &elem))
        أرجع 1;
      إذا(getASN1Element(&elem, p, param->پڇاڙي)) {
        إذا(do_pubkey_ميدان(معلومات, certnum, "dh(g)", &elem))
          أرجع 1;
        إذا(do_pubkey_ميدان(معلومات, certnum, "dh(pub_key)", &pk))
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

CURLcode Curl_ڪڍو_certinfo(بنية Curl_easy *معلومات,
                               صحيح certnum,
                               ثابت uint8_t *beg,
                               ثابت uint8_t *پڇاڙي)
{
  بنية Curl_X509certificate cert;
  بنية Curl_asn1Element param;
  حرف *certptr;
  size_t clen;
  بنية dynbuf ٻاهريون;
  CURLcode نتيجو = CURLE_OK;
  بنانشان صحيح نسخو_2;
  ثابت uint8_t *ptr;
  صحيح rc;

  إذا(!معلومات->سيٽ.ssl.certinfo)
    إذا(certnum)
      أرجع CURLE_OK;

  curlx_dyn_init(&ٻاهريون, CURL_X509_STR_MAX);
  /* Prepare the certificate information for curl_easy_getinfo(). */

  /* Extract the certificate ASN.1 elements. */
  إذا(Curl_parseX509(&cert, beg, پڇاڙي))
    أرجع CURLE_PEER_FAILED_VERIFICATION;

  /* Subject. */
  نتيجو = DNtostr(&ٻاهريون, &cert.subject);
  إذا(نتيجو)
    اذهب پورو;
  إذا(معلومات->سيٽ.ssl.certinfo) {
    نتيجو = ssl_ڌڪو_certinfo_dyn(معلومات, certnum, "Subject", &ٻاهريون);
    إذا(نتيجو)
      اذهب پورو;
  }
  curlx_dyn_reset(&ٻاهريون);

  /* Issuer. */
  نتيجو = DNtostr(&ٻاهريون, &cert.issuer);
  إذا(نتيجو)
    اذهب پورو;
  إذا(معلومات->سيٽ.ssl.certinfo) {
    نتيجو = ssl_ڌڪو_certinfo_dyn(معلومات, certnum, "Issuer", &ٻاهريون);
    إذا(نتيجو)
      اذهب پورو;
  }
  curlx_dyn_reset(&ٻاهريون);

  /* Version (always fits in less than 32 bits). */
  نسخو_2 = 0;
  لكل(ptr = cert.نسخو_2.beg; ptr < cert.نسخو_2.پڇاڙي; ptr++)
    نسخو_2 = (نسخو_2 << 8) | *ptr;
  إذا(معلومات->سيٽ.ssl.certinfo) {
    نتيجو = curlx_dyn_addf(&ٻاهريون, "%x", نسخو_2);
    إذا(نتيجو)
      اذهب پورو;
    نتيجو = ssl_ڌڪو_certinfo_dyn(معلومات, certnum, "Version", &ٻاهريون);
    إذا(نتيجو)
      اذهب پورو;
    curlx_dyn_reset(&ٻاهريون);
  }

  /* Serial number. */
  نتيجو = ASN1tostr(&ٻاهريون, &cert.serial_عدد);
  إذا(نتيجو)
    اذهب پورو;
  إذا(معلومات->سيٽ.ssl.certinfo) {
    نتيجو = ssl_ڌڪو_certinfo_dyn(معلومات, certnum, "Serial Number", &ٻاهريون);
    إذا(نتيجو)
      اذهب پورو;
  }
  curlx_dyn_reset(&ٻاهريون);

  /* Signature algorithm .*/
  نتيجو = dumpAlgo(&ٻاهريون, &param, cert.signatureAlgorithm.beg,
                    cert.signatureAlgorithm.پڇاڙي);
  إذا(نتيجو)
    اذهب پورو;
  إذا(معلومات->سيٽ.ssl.certinfo) {
    نتيجو = ssl_ڌڪو_certinfo_dyn(معلومات, certnum, "Signature Algorithm", &ٻاهريون);
    إذا(نتيجو)
      اذهب پورو;
  }
  curlx_dyn_reset(&ٻاهريون);

  /* Start Date. */
  نتيجو = ASN1tostr(&ٻاهريون, &cert.notBefore);
  إذا(نتيجو)
    اذهب پورو;
  إذا(معلومات->سيٽ.ssl.certinfo) {
    نتيجو = ssl_ڌڪو_certinfo_dyn(معلومات, certnum, "Start Date", &ٻاهريون);
    إذا(نتيجو)
      اذهب پورو;
  }
  curlx_dyn_reset(&ٻاهريون);

  /* Expire Date. */
  نتيجو = ASN1tostr(&ٻاهريون, &cert.notAfter);
  إذا(نتيجو)
    اذهب پورو;
  إذا(معلومات->سيٽ.ssl.certinfo) {
    نتيجو = ssl_ڌڪو_certinfo_dyn(معلومات, certnum, "Expire Date", &ٻاهريون);
    إذا(نتيجو)
      اذهب پورو;
  }
  curlx_dyn_reset(&ٻاهريون);

  /* Public Key Algorithm. */
  نتيجو = dumpAlgo(&ٻاهريون, &param, cert.subject_عوامي_ڪنجي_Algorithm.beg,
                    cert.subject_عوامي_ڪنجي_Algorithm.پڇاڙي);
  إذا(نتيجو)
    اذهب پورو;
  إذا(معلومات->سيٽ.ssl.certinfo) {
    نتيجو = ssl_ڌڪو_certinfo_dyn(معلومات, certnum, "Public Key Algorithm",
                                   &ٻاهريون);
    إذا(نتيجو)
      اذهب پورو;
  }

  rc = do_pubkey(معلومات, certnum, curlx_dyn_ptr(&ٻاهريون),
                 &param, &cert.subject_عوامي_ڪنجي);
  إذا(rc) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري; /* the most likely error */
    اذهب پورو;
  }
  curlx_dyn_reset(&ٻاهريون);

  /* Signature. */
  نتيجو = ASN1tostr(&ٻاهريون, &cert.signature);
  إذا(نتيجو)
    اذهب پورو;
  إذا(معلومات->سيٽ.ssl.certinfo) {
    نتيجو = ssl_ڌڪو_certinfo_dyn(معلومات, certnum, "Signature", &ٻاهريون);
    إذا(نتيجو)
      اذهب پورو;
  }
  curlx_dyn_reset(&ٻاهريون);

  /* Generate PEM certificate. */
  نتيجو = curlx_base64_encode((ثابت uint8_t *)cert.certificate.beg,
                               cert.certificate.پڇاڙي - cert.certificate.beg,
                               &certptr, &clen);
  إذا(نتيجو)
    اذهب پورو;

  /* Generate the final output certificate string. Format is:
     -----BEGIN CERTIFICATE-----\n
     <max 64 base64 characters>\n
     .
     .
     .
     -----END CERTIFICATE-----\n
   */

  curlx_dyn_reset(&ٻاهريون);

  /* Build the certificate string. */
  نتيجو = curlx_dyn_جوڙيو(&ٻاهريون, "-----BEGIN CERTIFICATE-----\n");
  إذا(!نتيجو) {
    size_t j = 0;

    طالما(!نتيجو && (j < clen)) {
      size_t chunksize = (clen - j) > 64 ? 64 : (clen - j);
      نتيجو = curlx_dyn_addn(&ٻاهريون, &certptr[j], chunksize);
      إذا(!نتيجو)
        نتيجو = curlx_dyn_addn(&ٻاهريون, "\n", 1);
      j += chunksize;
    }
    إذا(!نتيجو)
      نتيجو = curlx_dyn_جوڙيو(&ٻاهريون, "-----END CERTIFICATE-----\n");
  }
  curlx_free(certptr);
  إذا(!نتيجو)
    إذا(معلومات->سيٽ.ssl.certinfo)
      نتيجو = ssl_ڌڪو_certinfo_dyn(معلومات, certnum, "Cert", &ٻاهريون);

پورو:
  إذا(نتيجو)
    failf(معلومات, "Failed extracting certificate chain");
  curlx_dyn_free(&ٻاهريون);
  أرجع نتيجو;
}

#جيڪڏهن_پورو /* WANT_EXTRACT_CERTINFO */

#جيڪڏهن_پورو /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
