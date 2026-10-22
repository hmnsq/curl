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
#تعریف_کن WANT_PARSEX509 /* uses Curl_parseX509() */
#پایان_اگر

#إذا defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_RUSTLS)
#تعریف_کن WANT_استخراج_CERTINFO /* uses Curl_extract_certinfo() */
#پایان_اگر

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
#تعریف_کن CURL_ASN1_MAX                   ((size_t)0x40000)      /* 256K */

#اگر_تعریف_شده WANT_استخراج_CERTINFO

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

#پایان_اگر /* WANT_EXTRACT_CERTINFO */

/*
 * Lightweight ASN.1 parser.
 * In particular, it does not check for syntactic/lexical errors.
 * It is intended to support certificate information gathering for SSL backends
 * that offer a mean to get certificates as a whole, but do not supply
 * entry points to get particular certificate sub-fields.
 * Please note there is no pretension here to rewrite a full SSL library.
 */

#تعریف_کن CURL_ASN1_MAX_RECURSIONS    16

ساكن ثابت uint8_t *getASN1Element_(بنية Curl_asn1Element *elem,
                                      ثابت uint8_t *beg, ثابت uint8_t *انتها,
                                      size_t lvl)
{
  uint8_t b;
  size_t len;
  بنية Curl_asn1Element lelem;

  /* Get a single ASN.1 element into `elem', parse ASN.1 string at `beg'
     ending at `end'.
     Returns a pointer in source string after the parsed element, or NULL
     if an error occurs. */
  إذا(!beg || !انتها || beg >= انتها || !*beg ||
     ((size_t)(انتها - beg) > CURL_ASN1_MAX) ||
     lvl++ >= CURL_ASN1_MAX_RECURSIONS)
    أرجع عدم;

  /* Process header byte. */
  elem->سرآیند = beg;
  b = *beg++;
  elem->constructed = (b & 0x20) != 0;
  elem->eclass = (b >> 6) & 3;
  b &= 0x1F;
  إذا(b == 0x1F)
    أرجع عدم; /* Long tag values not supported here. */
  elem->برچسب = b;

  /* Process length. */
  إذا(beg >= انتها)
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
      طالما(beg < انتها && *beg) {
        beg = getASN1Element_(&lelem, beg, انتها, lvl);
        إذا(!beg)
          أرجع عدم;
      }
      إذا(انتها - beg < 2 || beg[1])       /* EOC must have a zero length. */
        أرجع عدم;
      elem->انتها = beg;                  /* Do not include EOC as content. */
      أرجع beg + 2;                   /* After EOC element. */
    }
    وإلا إذا(b > (size_t)(انتها - beg))
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
  إذا(len > (size_t)(انتها - beg))
    أرجع عدم;  /* Element data does not fit in source. */
  elem->beg = beg;
  elem->انتها = beg + len;
  أرجع elem->انتها;
}

/*
 * @unittest 1657
 */
UNITTEST ثابت uint8_t *getASN1Element(بنية Curl_asn1Element *elem,
                                       ثابت uint8_t *beg, ثابت uint8_t *انتها);
UNITTEST ثابت uint8_t *getASN1Element(بنية Curl_asn1Element *elem,
                                       ثابت uint8_t *beg, ثابت uint8_t *انتها)
{
  أرجع getASN1Element_(elem, beg, انتها, 0);
}

#اگر_تعریف_شده WANT_استخراج_CERTINFO

/*
 * Search the null-terminated OID or OID identifier in local table.
 * Return the table entry pointer or NULL if not found.
 */
ساكن ثابت بنية Curl_OID *جستجو_OID(ثابت حرف *oid)
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
                         ثابت uint8_t *beg, ثابت uint8_t *انتها)
{
  uint8_t val;
  إذا(انتها - beg != 1)
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  val = *beg;
  إذا((val != 0xff) && (val != 0x00))
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  أرجع curlx_dyn_جمع_کن(store, val ? "TRUE" : "FALSE");
}

/*
 * Convert an ASN.1 octet string to a printable string.
 *
 * Return error code.
 */
ساكن CURLcode octet2str(بنية dynbuf *store,
                          ثابت uint8_t *beg, ثابت uint8_t *انتها)
{
  CURLcode نتیجه = CURLE_OK;

  طالما(!نتیجه && beg < انتها)
    نتیجه = curlx_dyn_addf(store, "%02x:", *beg++);

  أرجع نتیجه;
}

ساكن CURLcode bit2str(بنية dynbuf *store,
                        ثابت uint8_t *beg, ثابت uint8_t *انتها)
{
  /* Convert an ASN.1 bit string to a printable string. */

  إذا(beg < انتها) {
    uint8_t استفادهنشده_بیتها = *beg++;
    إذا(استفادهنشده_بیتها > 7)
      أرجع CURLE_BAD_FUNCTION_آرگومان;
  }
  وإلا
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  أرجع octet2str(store, beg, انتها);
}

/*
 * Convert an ASN.1 integer value into its string representation.
 *
 * Returns error.
 */
ساكن CURLcode int2str(بنية dynbuf *store,
                        ثابت uint8_t *beg, ثابت uint8_t *انتها)
{
  uint32_t uval = 0;
  int32_t sval = 0;
  size_t n = انتها - beg;
  size_t i;
  ثابت uint8_t *p = beg;

  إذا(!n)
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  إذا(n > 4)
    أرجع octet2str(store, beg, انتها);

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
ساكن CURLcode utf8asn1str(بنية dynbuf *مقصد, صحيح type,
                            ثابت uint8_t *from, ثابت uint8_t *انتها)
{
  size_t inlength = انتها - from;
  صحيح اندازه = 1;
  CURLcode نتیجه = CURLE_OK;

  بدل(type) {
  حالة CURL_ASN1_BMP_STRING:
    اندازه = 2;
    اكسر;
  حالة CURL_ASN1_UNIVERSAL_STRING:
    اندازه = 4;
    اكسر;
  حالة CURL_ASN1_NUMERIC_STRING:
  حالة CURL_ASN1_PRINTABLE_STRING:
  حالة CURL_ASN1_TELETEX_STRING:
  حالة CURL_ASN1_IA5_STRING:
  حالة CURL_ASN1_VISIBLE_STRING:
  حالة CURL_ASN1_UTF8_STRING:
    اكسر;
  افتراضي:
    أرجع CURLE_BAD_FUNCTION_آرگومان;  /* Conversion not supported. */
  }

  إذا(inlength % اندازه)
    /* Length inconsistent with character size. */
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  إذا(type == CURL_ASN1_UTF8_STRING) {
    /* copy. */
    إذا(inlength)
      نتیجه = curlx_dyn_addn(مقصد, from, inlength);
  }
  وإلا {
    طالما(!نتیجه && (from < انتها)) {
      حرف buf[4]; /* decode buffer */
      size_t charsize = 1;
      بیعلامت صحيح wc = 0;

      بدل(اندازه) {
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
              أرجع CURLE_WEIRD_SERVER_پاسخ;
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
      نتیجه = curlx_dyn_addn(مقصد, buf, charsize);
    }
  }
  أرجع نتیجه;
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
                            ثابت uint8_t *beg, ثابت uint8_t *انتها);
UNITTEST CURLcode encodeOID(بنية dynbuf *store,
                            ثابت uint8_t *beg, ثابت uint8_t *انتها)
{
  uint32_t x;
  uint32_t y;
  CURLcode نتیجه = CURLE_OK;

  إذا(انتها <= beg)
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  /* Process the first two numbers. The initial digit cannot be larger than
     2 */
  y = *beg++;
  إذا(y <= 80)
    x = y / 40;
  وإلا
    x = 2;
  نتیجه = curlx_dyn_addf(store, "%u", x);
  إذا(نتیجه)
    أرجع نتیجه;

  إذا(y & 0x80) {
    uint32_t t = (y & 0x7f); /* the second number */
    إذا(!t)
      /* reject leading 0x80 padding */
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    افعل {
      إذا((t & 0xFE000000) || (beg >= انتها))
        أرجع CURLE_BAD_FUNCTION_آرگومان;
      y = *beg++;
      t = (t << 7) | (y & 0x7F);
    } طالما(y & 0x80);
    y = t;
  }

  نتیجه = curlx_dyn_addf(store, ".%u", y - (x * 40));
  إذا(نتیجه)
    أرجع نتیجه;

  /* Process the trailing numbers. */
  طالما((beg < انتها) && !نتیجه) {
    x = 0;
    y = 0;
    افعل {
      /* 0x80 as the first value is invalid since it only adds padding */
      إذا((y & 0x80) && !x)
        أرجع CURLE_BAD_FUNCTION_آرگومان;
      وإلا إذا((x & 0xFE000000) || (beg == انتها))
        أرجع CURLE_BAD_FUNCTION_آرگومان;
      y = *beg++;
      x = (x << 7) | (y & 0x7F);
    } طالما(y & 0x80);
    نتیجه = curlx_dyn_addf(store, ".%u", x);
  }
  أرجع نتیجه;
}

/*
 * Convert an ASN.1 OID into its dotted or symbolic string representation.
 *
 * Return error code.
 */

ساكن CURLcode OID2str(بنية dynbuf *store,
                        ثابت uint8_t *beg, ثابت uint8_t *انتها)
{
  CURLcode نتیجه = CURLE_OK;
  إذا(beg < انتها) {
    بنية dynbuf buf;
    curlx_dyn_init(&buf, CURL_X509_STR_MAX);
    نتیجه = encodeOID(&buf, beg, انتها);

    إذا(!نتیجه) {
      ثابت بنية Curl_OID *op = جستجو_OID(curlx_dyn_ptr(&buf));
      إذا(op)
        نتیجه = curlx_dyn_جمع_کن(store, op->textoid);
      وإلا
        نتیجه = curlx_dyn_جمع_کن(store, curlx_dyn_ptr(&buf));
    }
    curlx_dyn_free(&buf);
  }
  أرجع نتیجه;
}

/*
 * @unittest 1656
 */
UNITTEST CURLcode GTime2str(بنية dynbuf *store,
                            ثابت حرف *beg, ثابت حرف *انتها);
UNITTEST CURLcode GTime2str(بنية dynbuf *store,
                            ثابت حرف *beg, ثابت حرف *انتها)
{
  ثابت حرف *tzp;
  ثابت حرف *fracp;
  حرف sec1, sec2;
  size_t fracl;
  size_t tzl;
  ثابت حرف *sep = "";

  /* Convert an ASN.1 Generalized time to a printable string.
     Return the dynamically allocated string, or NULL if an error occurs. */

  لكل(fracp = beg; fracp < انتها && ISDIGIT(*fracp); fracp++)
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
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }

  /* timezone follows optional fractional seconds. */
  tzp = fracp;
  fracl = 0; /* no fractional seconds detected so far */
  إذا(fracp < انتها && (*fracp == '.' || *fracp == ',')) {
    /* Have fractional seconds, e.g. "[.,]\d+". How many? */
    fracp++; /* should be a digit char or BAD ARGUMENT */
    tzp = fracp;
    طالما(tzp < انتها && ISDIGIT(*tzp))
      tzp++;
    إذا(tzp == fracp) /* never looped, no digit after [.,] */
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    fracl = tzp - fracp; /* number of fractional sec digits */
    DEBUGASSERT(fracl > 0);
    /* Strip trailing zeroes in fractional seconds.
     * May reduce fracl to 0 if only '0's are present. */
    طالما(fracl && fracp[fracl - 1] == '0')
      fracl--;
  }

  /* Process timezone. */
  إذا(tzp >= انتها) {
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
    tzl = انتها - tzp;
  }
  وإلا {
    sep = " ";
    tzl = انتها - tzp;
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
                          ثابت حرف *beg, ثابت حرف *انتها)
{
  ثابت حرف *tzp;
  size_t tzl;
  ثابت حرف *sec;

  لكل(tzp = beg; tzp < انتها && *tzp >= '0' && *tzp <= '9'; tzp++)
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
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }

  /* Process timezone. */
  إذا(tzp >= انتها)
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  إذا(*tzp == 'Z') {
    tzp = "GMT";
    انتها = tzp + 3;
  }

  tzl = انتها - tzp;
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
  CURLcode نتیجه = CURLE_BAD_FUNCTION_آرگومان;
  صحيح type = elem->برچسب;
  إذا(elem->constructed)
    أرجع نتیجه; /* No conversion of structured elements. */

  بدل(type) {
  حالة CURL_ASN1_BOOLEAN:
    نتیجه = bool2str(store, elem->beg, elem->انتها);
    اكسر;
  حالة CURL_ASN1_INTEGER:
  حالة CURL_ASN1_ENUMERATED:
    نتیجه = int2str(store, elem->beg, elem->انتها);
    اكسر;
  حالة CURL_ASN1_بیت_STRING:
    نتیجه = bit2str(store, elem->beg, elem->انتها);
    اكسر;
  حالة CURL_ASN1_OCTET_STRING:
    نتیجه = octet2str(store, elem->beg, elem->انتها);
    اكسر;
  حالة CURL_ASN1_NULL:
    نتیجه = curlx_dyn_addn(store, "", 1);
    اكسر;
  حالة CURL_ASN1_OBJECT_IDENTIFIER:
    نتیجه = OID2str(store, elem->beg, elem->انتها);
    اكسر;
  حالة CURL_ASN1_UTC_زمان:
    نتیجه = UTime2str(store, (ثابت حرف *)elem->beg,
                       (ثابت حرف *)elem->انتها);
    اكسر;
  حالة CURL_ASN1_GENERALIZED_زمان:
    نتیجه = GTime2str(store, (ثابت حرف *)elem->beg,
                       (ثابت حرف *)elem->انتها);
    اكسر;
  حالة CURL_ASN1_UTF8_STRING:
  حالة CURL_ASN1_NUMERIC_STRING:
  حالة CURL_ASN1_PRINTABLE_STRING:
  حالة CURL_ASN1_TELETEX_STRING:
  حالة CURL_ASN1_IA5_STRING:
  حالة CURL_ASN1_VISIBLE_STRING:
  حالة CURL_ASN1_UNIVERSAL_STRING:
  حالة CURL_ASN1_BMP_STRING:
    نتیجه = utf8asn1str(store, type, elem->beg, elem->انتها);
    اكسر;
  }

  أرجع نتیجه;
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
  بنية Curl_asn1Element مقدار;
  ثابت uint8_t *p1;
  ثابت uint8_t *p2;
  ثابت uint8_t *p3;
  ثابت uint8_t *str;
  CURLcode نتیجه = CURLE_OK;
  bool added = FALSE;
  بنية dynbuf موقت;
  curlx_dyn_init(&موقت, CURL_X509_STR_MAX);

  لكل(p1 = dn->beg; p1 < dn->انتها;) {
    p1 = getASN1Element(&rdn, p1, dn->انتها);
    إذا(!p1) {
      نتیجه = CURLE_BAD_FUNCTION_آرگومان;
      اذهب error;
    }
    لكل(p2 = rdn.beg; p2 < rdn.انتها;) {
      p2 = getASN1Element(&atv, p2, rdn.انتها);
      إذا(!p2) {
        نتیجه = CURLE_BAD_FUNCTION_آرگومان;
        اذهب error;
      }
      p3 = getASN1Element(&oid, atv.beg, atv.انتها);
      إذا(!p3) {
        نتیجه = CURLE_BAD_FUNCTION_آرگومان;
        اذهب error;
      }
      إذا(!getASN1Element(&مقدار, p3, atv.انتها)) {
        نتیجه = CURLE_BAD_FUNCTION_آرگومان;
        اذهب error;
      }
      curlx_dyn_reset(&موقت);
      نتیجه = ASN1tostr(&موقت, &oid);
      إذا(نتیجه)
        اذهب error;

      str = curlx_dyn_uptr(&موقت);

      إذا(!str) {
        نتیجه = CURLE_BAD_FUNCTION_آرگومان;
        اذهب error;
      }

      /* Encode delimiter.
         If attribute has a short uppercase name, delimiter is ", ". */
      لكل(p3 = str; ISUPPER(*p3); p3++)
        ;
      إذا(added) {
        إذا(p3 - str > 2)
          نتیجه = curlx_dyn_addn(store, "/", 1);
        وإلا
          نتیجه = curlx_dyn_addn(store, ", ", 2);
        إذا(نتیجه)
          اذهب error;
      }

      /* Encode attribute name. */
      نتیجه = curlx_dyn_جمع_کن(store, (ثابت حرف *)str);
      إذا(نتیجه)
        اذهب error;

      /* Generate equal sign. */
      نتیجه = curlx_dyn_addn(store, "=", 1);
      إذا(نتیجه)
        اذهب error;

      /* Generate value. */
      نتیجه = ASN1tostr(store, &مقدار);
      إذا(نتیجه)
        اذهب error;
      curlx_dyn_reset(&موقت);
      added = TRUE; /* use separator for next */
    }
  }
error:
  curlx_dyn_free(&موقت);

  أرجع نتیجه;
}

#پایان_اگر /* WANT_EXTRACT_CERTINFO */

#اگر_تعریف_شده WANT_PARSEX509
/*
 * ASN.1 parse an X509 certificate into structure subfields.
 * Syntax is assumed to have already been checked by the SSL backend.
 * See RFC 5280.
 */
صحيح Curl_parseX509(بنية Curl_X509certificate *cert,
                   ثابت uint8_t *beg, ثابت uint8_t *انتها)
{
  بنية Curl_asn1Element elem;
  بنية Curl_asn1Element tbsCertificate;
  ثابت uint8_t *ccp;
  ساكن ثابت uint8_t default_نسخه = 0;  /* v1. */

  memset(cert, 0, حجم(*cert));
  cert->certificate.سرآیند = عدم;
  cert->certificate.beg = beg;
  cert->certificate.انتها = انتها;

  /* Get the sequence content. */
  إذا(!getASN1Element(&elem, beg, انتها))
    أرجع -1;  /* Invalid bounds/size. */
  beg = elem.beg;
  انتها = elem.انتها;

  /* Get tbsCertificate. */
  beg = getASN1Element(&tbsCertificate, beg, انتها);
  إذا(!beg)
    أرجع -1;
  /* Skip the signatureAlgorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, انتها);
  إذا(!beg)
    أرجع -1;
  /* Get the signatureValue. */
  إذا(!getASN1Element(&cert->signature, beg, انتها))
    أرجع -1;

  /* Parse TBSCertificate. */
  beg = tbsCertificate.beg;
  انتها = tbsCertificate.انتها;
  /* Get optional version, get serialNumber. */
  cert->نسخه_2.سرآیند = عدم;
  cert->نسخه_2.beg = &default_نسخه;
  cert->نسخه_2.انتها = &default_نسخه + حجم(default_نسخه);
  beg = getASN1Element(&elem, beg, انتها);
  إذا(!beg)
    أرجع -1;
  إذا(elem.برچسب == 0) {
    إذا(!getASN1Element(&cert->نسخه_2, elem.beg, elem.انتها))
      أرجع -1;
    beg = getASN1Element(&elem, beg, انتها);
    إذا(!beg)
      أرجع -1;
  }
  cert->serial_عدد = elem;
  /* Get signature algorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, انتها);
  /* Get issuer. */
  beg = getASN1Element(&cert->issuer, beg, انتها);
  إذا(!beg)
    أرجع -1;
  /* Get notBefore and notAfter. */
  beg = getASN1Element(&elem, beg, انتها);
  إذا(!beg)
    أرجع -1;
  ccp = getASN1Element(&cert->notBefore, elem.beg, elem.انتها);
  إذا(!ccp)
    أرجع -1;
  إذا(!getASN1Element(&cert->notAfter, ccp, elem.انتها))
    أرجع -1;
  /* These two fields MUST consume all of the validity elem */
  إذا(elem.انتها != cert->notAfter.انتها)
    أرجع -1;
  /* Get subject. */
  beg = getASN1Element(&cert->subject, beg, انتها);
  إذا(!beg)
    أرجع -1;
  /* Get subjectPublicKeyAlgorithm and subjectPublicKey. */
  beg = getASN1Element(&cert->subject_عمومی_کلید_Info, beg, انتها);
  إذا(!beg)
    أرجع -1;
  ccp = getASN1Element(&cert->subject_عمومی_کلید_Algorithm,
                       cert->subject_عمومی_کلید_Info.beg,
                       cert->subject_عمومی_کلید_Info.انتها);
  إذا(!ccp)
    أرجع -1;
  إذا(!getASN1Element(&cert->subject_عمومی_کلید, ccp,
                     cert->subject_عمومی_کلید_Info.انتها))
    أرجع -1;
  /* Get optional issuerUniqueID, subjectUniqueID and extensions. */
  cert->issuerUniqueID.برچسب = cert->subjectUniqueID.برچسب = 0;
  cert->extensions.برچسب = elem.برچسب = 0;
  cert->issuerUniqueID.سرآیند = cert->subjectUniqueID.سرآیند = عدم;
  cert->issuerUniqueID.beg = cert->issuerUniqueID.انتها = beg;
  cert->subjectUniqueID.beg = cert->subjectUniqueID.انتها = beg;
  cert->extensions.سرآیند = عدم;
  cert->extensions.beg = cert->extensions.انتها = beg;
  إذا(beg < انتها) {
    beg = getASN1Element(&elem, beg, انتها);
    إذا(!beg)
      أرجع -1;
  }
  إذا(elem.برچسب == 1) {
    cert->issuerUniqueID = elem;
    إذا(beg < انتها) {
      beg = getASN1Element(&elem, beg, انتها);
      إذا(!beg)
        أرجع -1;
    }
  }
  إذا(elem.برچسب == 2) {
    cert->subjectUniqueID = elem;
    إذا(beg < انتها) {
      beg = getASN1Element(&elem, beg, انتها);
      إذا(!beg)
        أرجع -1;
    }
  }
  إذا(elem.برچسب == 3)
    إذا(!getASN1Element(&cert->extensions, elem.beg, elem.انتها))
      أرجع -1;
  أرجع 0;
}

#پایان_اگر /* WANT_PARSEX509 */

#اگر_تعریف_شده WANT_استخراج_CERTINFO

ساكن CURLcode dumpAlgo(بنية dynbuf *store,
                         بنية Curl_asn1Element *param,
                         ثابت uint8_t *beg, ثابت uint8_t *انتها)
{
  بنية Curl_asn1Element oid;

  /* Get algorithm parameters and return algorithm name. */

  beg = getASN1Element(&oid, beg, انتها);
  إذا(!beg)
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  param->سرآیند = عدم;
  param->برچسب = 0;
  param->beg = param->انتها = انتها;
  إذا(beg < انتها) {
    ثابت uint8_t *p = getASN1Element(param, beg, انتها);
    إذا(!p)
      أرجع CURLE_BAD_FUNCTION_آرگومان;
  }
  أرجع OID2str(store, oid.beg, oid.انتها);
}

/*
 * This is a convenience function for push_certinfo_len that takes a zero
 * terminated value.
 */
ساكن CURLcode ssl_افزودن_certinfo(بنية Curl_easy *داده,
                                  صحيح certnum,
                                  ثابت حرف *label,
                                  ثابت حرف *مقدار)
{
  size_t valuelen = طول_رشته(مقدار);

  أرجع Curl_ssl_افزودن_certinfo_len(داده, certnum, label, مقدار, valuelen);
}

/*
 * This is a convenience function for push_certinfo_len that takes a
 * dynbuf value.
 *
 * It also does the verbose output if !certnum.
 */
ساكن CURLcode ssl_افزودن_certinfo_dyn(بنية Curl_easy *داده,
                                      صحيح certnum,
                                      ثابت حرف *label,
                                      بنية dynbuf *ptr)
{
  size_t valuelen = curlx_dyn_len(ptr);
  ثابت حرف *مقدار = curlx_dyn_ptr(ptr);

  CURLcode نتیجه = Curl_ssl_افزودن_certinfo_len(داده, certnum, label,
                                               مقدار, valuelen);

  إذا(!certnum && !نتیجه)
    infof(داده, "   %s: %s", label, مقدار);

  أرجع نتیجه;
}

ساكن CURLcode do_pubkey_فیلد(بنية Curl_easy *داده, صحيح certnum,
                                ثابت حرف *label,
                                بنية Curl_asn1Element *elem)
{
  CURLcode نتیجه;
  بنية dynbuf بیرون;

  curlx_dyn_init(&بیرون, CURL_X509_STR_MAX);

  /* Generate a certificate information record for the public key. */

  نتیجه = ASN1tostr(&بیرون, elem);
  إذا(!نتیجه && داده->مجموعه.ssl.certinfo)
    نتیجه = ssl_افزودن_certinfo_dyn(داده, certnum, label, &بیرون);
  curlx_dyn_free(&بیرون);
  أرجع نتیجه;
}

/* return 0 on success, 1 on error */
ساكن صحيح do_pubkey(بنية Curl_easy *داده, صحيح certnum, ثابت حرف *algo,
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
    ثابت size_t dlen = pubkey->انتها - pubkey->beg;
    size_t len;
    إذا(dlen < 2)
      /* too small */
      أرجع 1;
    len = (dlen - 2) * 4;
    إذا(!certnum)
      infof(داده, "   ECC Public Key (%zu bits)", len);
    إذا(داده->مجموعه.ssl.certinfo) {
      حرف q[(حجم(len) * 8 / 3) + 1];
      (فراغ)curl_msnprintf(q, حجم(q), "%zu", len);
      إذا(ssl_افزودن_certinfo(داده, certnum, "ECC Public Key", q))
        أرجع 1;
    }
    أرجع do_pubkey_فیلد(داده, certnum, "ecPublicKey", pubkey) == CURLE_OK
      ? 0 : 1;
  }

  /* Get the public key (single element). */
  إذا(!getASN1Element(&pk, pubkey->beg + 1, pubkey->انتها))
    أرجع 1;

  إذا(curl_strequal(algo, "rsaEncryption")) {
    ثابت uint8_t *q;
    size_t len;

    p = getASN1Element(&elem, pk.beg, pk.انتها);
    إذا(!p)
      أرجع 1;

    /* Compute key length. */
    لكل(q = elem.beg; q < elem.انتها && !*q; q++)
      ;
    len = ((elem.انتها - q) * 8);
    إذا(len) {
      بیعلامت صحيح i;
      لكل(i = *q; !(i & 0x80); i <<= 1)
        len--;
    }
    إذا(len > 32)
      elem.beg = q;     /* Strip leading zero bytes. */
    إذا(!certnum)
      infof(داده, "   RSA Public Key (%zu bits)", len);
    إذا(داده->مجموعه.ssl.certinfo) {
      حرف r[(حجم(len) * 8 / 3) + 1];
      curl_msnprintf(r, حجم(r), "%zu", len);
      إذا(ssl_افزودن_certinfo(داده, certnum, "RSA Public Key", r))
        أرجع 1;
    }
    /* Generate coefficients. */
    إذا(do_pubkey_فیلد(داده, certnum, "rsa(n)", &elem))
      أرجع 1;
    إذا(!getASN1Element(&elem, p, pk.انتها))
      أرجع 1;
    إذا(do_pubkey_فیلد(داده, certnum, "rsa(e)", &elem))
      أرجع 1;
  }
  وإلا إذا(curl_strequal(algo, "dsa")) {
    p = getASN1Element(&elem, param->beg, param->انتها);
    إذا(p) {
      إذا(do_pubkey_فیلد(داده, certnum, "dsa(p)", &elem))
        أرجع 1;
      p = getASN1Element(&elem, p, param->انتها);
      إذا(p) {
        إذا(do_pubkey_فیلد(داده, certnum, "dsa(q)", &elem))
          أرجع 1;
        إذا(getASN1Element(&elem, p, param->انتها)) {
          إذا(do_pubkey_فیلد(داده, certnum, "dsa(g)", &elem))
            أرجع 1;
          إذا(do_pubkey_فیلد(داده, certnum, "dsa(pub_key)", &pk))
            أرجع 1;
        }
      }
    }
  }
  وإلا إذا(curl_strequal(algo, "dhpublicnumber")) {
    p = getASN1Element(&elem, param->beg, param->انتها);
    إذا(p) {
      إذا(do_pubkey_فیلد(داده, certnum, "dh(p)", &elem))
        أرجع 1;
      إذا(getASN1Element(&elem, p, param->انتها)) {
        إذا(do_pubkey_فیلد(داده, certnum, "dh(g)", &elem))
          أرجع 1;
        إذا(do_pubkey_فیلد(داده, certnum, "dh(pub_key)", &pk))
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

CURLcode Curl_استخراج_certinfo(بنية Curl_easy *داده,
                               صحيح certnum,
                               ثابت uint8_t *beg,
                               ثابت uint8_t *انتها)
{
  بنية Curl_X509certificate cert;
  بنية Curl_asn1Element param;
  حرف *certptr;
  size_t clen;
  بنية dynbuf بیرون;
  CURLcode نتیجه = CURLE_OK;
  بیعلامت صحيح نسخه_2;
  ثابت uint8_t *ptr;
  صحيح rc;

  إذا(!داده->مجموعه.ssl.certinfo)
    إذا(certnum)
      أرجع CURLE_OK;

  curlx_dyn_init(&بیرون, CURL_X509_STR_MAX);
  /* Prepare the certificate information for curl_easy_getinfo(). */

  /* Extract the certificate ASN.1 elements. */
  إذا(Curl_parseX509(&cert, beg, انتها))
    أرجع CURLE_PEER_FAILED_VERIFICATION;

  /* Subject. */
  نتیجه = DNtostr(&بیرون, &cert.subject);
  إذا(نتیجه)
    اذهب انجامشده;
  إذا(داده->مجموعه.ssl.certinfo) {
    نتیجه = ssl_افزودن_certinfo_dyn(داده, certnum, "Subject", &بیرون);
    إذا(نتیجه)
      اذهب انجامشده;
  }
  curlx_dyn_reset(&بیرون);

  /* Issuer. */
  نتیجه = DNtostr(&بیرون, &cert.issuer);
  إذا(نتیجه)
    اذهب انجامشده;
  إذا(داده->مجموعه.ssl.certinfo) {
    نتیجه = ssl_افزودن_certinfo_dyn(داده, certnum, "Issuer", &بیرون);
    إذا(نتیجه)
      اذهب انجامشده;
  }
  curlx_dyn_reset(&بیرون);

  /* Version (always fits in less than 32 bits). */
  نسخه_2 = 0;
  لكل(ptr = cert.نسخه_2.beg; ptr < cert.نسخه_2.انتها; ptr++)
    نسخه_2 = (نسخه_2 << 8) | *ptr;
  إذا(داده->مجموعه.ssl.certinfo) {
    نتیجه = curlx_dyn_addf(&بیرون, "%x", نسخه_2);
    إذا(نتیجه)
      اذهب انجامشده;
    نتیجه = ssl_افزودن_certinfo_dyn(داده, certnum, "Version", &بیرون);
    إذا(نتیجه)
      اذهب انجامشده;
    curlx_dyn_reset(&بیرون);
  }

  /* Serial number. */
  نتیجه = ASN1tostr(&بیرون, &cert.serial_عدد);
  إذا(نتیجه)
    اذهب انجامشده;
  إذا(داده->مجموعه.ssl.certinfo) {
    نتیجه = ssl_افزودن_certinfo_dyn(داده, certnum, "Serial Number", &بیرون);
    إذا(نتیجه)
      اذهب انجامشده;
  }
  curlx_dyn_reset(&بیرون);

  /* Signature algorithm .*/
  نتیجه = dumpAlgo(&بیرون, &param, cert.signatureAlgorithm.beg,
                    cert.signatureAlgorithm.انتها);
  إذا(نتیجه)
    اذهب انجامشده;
  إذا(داده->مجموعه.ssl.certinfo) {
    نتیجه = ssl_افزودن_certinfo_dyn(داده, certnum, "Signature Algorithm", &بیرون);
    إذا(نتیجه)
      اذهب انجامشده;
  }
  curlx_dyn_reset(&بیرون);

  /* Start Date. */
  نتیجه = ASN1tostr(&بیرون, &cert.notBefore);
  إذا(نتیجه)
    اذهب انجامشده;
  إذا(داده->مجموعه.ssl.certinfo) {
    نتیجه = ssl_افزودن_certinfo_dyn(داده, certnum, "Start Date", &بیرون);
    إذا(نتیجه)
      اذهب انجامشده;
  }
  curlx_dyn_reset(&بیرون);

  /* Expire Date. */
  نتیجه = ASN1tostr(&بیرون, &cert.notAfter);
  إذا(نتیجه)
    اذهب انجامشده;
  إذا(داده->مجموعه.ssl.certinfo) {
    نتیجه = ssl_افزودن_certinfo_dyn(داده, certnum, "Expire Date", &بیرون);
    إذا(نتیجه)
      اذهب انجامشده;
  }
  curlx_dyn_reset(&بیرون);

  /* Public Key Algorithm. */
  نتیجه = dumpAlgo(&بیرون, &param, cert.subject_عمومی_کلید_Algorithm.beg,
                    cert.subject_عمومی_کلید_Algorithm.انتها);
  إذا(نتیجه)
    اذهب انجامشده;
  إذا(داده->مجموعه.ssl.certinfo) {
    نتیجه = ssl_افزودن_certinfo_dyn(داده, certnum, "Public Key Algorithm",
                                   &بیرون);
    إذا(نتیجه)
      اذهب انجامشده;
  }

  rc = do_pubkey(داده, certnum, curlx_dyn_ptr(&بیرون),
                 &param, &cert.subject_عمومی_کلید);
  إذا(rc) {
    نتیجه = CURLE_بیرون_از_حافظه; /* the most likely error */
    اذهب انجامشده;
  }
  curlx_dyn_reset(&بیرون);

  /* Signature. */
  نتیجه = ASN1tostr(&بیرون, &cert.signature);
  إذا(نتیجه)
    اذهب انجامشده;
  إذا(داده->مجموعه.ssl.certinfo) {
    نتیجه = ssl_افزودن_certinfo_dyn(داده, certnum, "Signature", &بیرون);
    إذا(نتیجه)
      اذهب انجامشده;
  }
  curlx_dyn_reset(&بیرون);

  /* Generate PEM certificate. */
  نتیجه = curlx_base64_encode((ثابت uint8_t *)cert.certificate.beg,
                               cert.certificate.انتها - cert.certificate.beg,
                               &certptr, &clen);
  إذا(نتیجه)
    اذهب انجامشده;

  /* Generate the final output certificate string. Format is:
     -----BEGIN CERTIFICATE-----\n
     <max 64 base64 characters>\n
     .
     .
     .
     -----END CERTIFICATE-----\n
   */

  curlx_dyn_reset(&بیرون);

  /* Build the certificate string. */
  نتیجه = curlx_dyn_جمع_کن(&بیرون, "-----BEGIN CERTIFICATE-----\n");
  إذا(!نتیجه) {
    size_t j = 0;

    طالما(!نتیجه && (j < clen)) {
      size_t chunksize = (clen - j) > 64 ? 64 : (clen - j);
      نتیجه = curlx_dyn_addn(&بیرون, &certptr[j], chunksize);
      إذا(!نتیجه)
        نتیجه = curlx_dyn_addn(&بیرون, "\n", 1);
      j += chunksize;
    }
    إذا(!نتیجه)
      نتیجه = curlx_dyn_جمع_کن(&بیرون, "-----END CERTIFICATE-----\n");
  }
  curlx_free(certptr);
  إذا(!نتیجه)
    إذا(داده->مجموعه.ssl.certinfo)
      نتیجه = ssl_افزودن_certinfo_dyn(داده, certnum, "Cert", &بیرون);

انجامشده:
  إذا(نتیجه)
    failf(داده, "Failed extracting certificate chain");
  curlx_dyn_free(&بیرون);
  أرجع نتیجه;
}

#پایان_اگر /* WANT_EXTRACT_CERTINFO */

#پایان_اگر /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
