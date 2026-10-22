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
#شامل "curl_setup.h"

#اگر defined(USE_GNUTLS) || defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || \
  defined(USE_MBEDTLS) || defined(USE_RUSTLS)

#اگر defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_RUSTLS)
#تعریف_کرو WANT_PARSEX509 /* uses Curl_parseX509() */
#اگر_ختم

#اگر defined(USE_GNUTLS) || defined(USE_SCHANNEL) || defined(USE_MBEDTLS) || \
  defined(USE_RUSTLS)
#تعریف_کرو WANT_نکالو_CERTINFO /* uses Curl_extract_certinfo() */
#اگر_ختم

#شامل "urldata.h"
#شامل "vtls/vtls.h"
#شامل "curl_trc.h"
#شامل "curlx/base64.h"
#شامل "vtls/x509asn1.h"
#شامل "curlx/dynbuf.h"

/*
 * Constants.
 */

/* Largest supported ASN.1 structure. */
#تعریف_کرو CURL_ASN1_MAX                   ((size_t)0x40000)      /* 256K */

#اگر_تعریف_شدہ WANT_نکالو_CERTINFO

/* ASN.1 OID table entry. */
ساخت Curl_OID {
  ثابت حرف *numoid;  /* Dotted-numeric OID. */
  ثابت حرف *textoid; /* OID name. */
};

/* ASN.1 OIDs. */
ساکن ثابت ساخت Curl_OID OIDtable[] = {
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
  { تہی,                       تہی }
};

#اگر_ختم /* WANT_EXTRACT_CERTINFO */

/*
 * Lightweight ASN.1 parser.
 * In particular, it does not check for syntactic/lexical errors.
 * It is intended to support certificate information gathering for SSL backends
 * that offer a mean to get certificates as a whole, but do not supply
 * entry points to get particular certificate sub-fields.
 * Please note there is no pretension here to rewrite a full SSL library.
 */

#تعریف_کرو CURL_ASN1_MAX_RECURSIONS    16

ساکن ثابت uint8_t *getASN1Element_(ساخت Curl_asn1Element *elem,
                                      ثابت uint8_t *beg, ثابت uint8_t *انتہا,
                                      size_t lvl)
{
  uint8_t b;
  size_t len;
  ساخت Curl_asn1Element lelem;

  /* Get a single ASN.1 element into `elem', parse ASN.1 string at `beg'
     ending at `end'.
     Returns a pointer in source string after the parsed element, or NULL
     if an error occurs. */
  اگر(!beg || !انتہا || beg >= انتہا || !*beg ||
     ((size_t)(انتہا - beg) > CURL_ASN1_MAX) ||
     lvl++ >= CURL_ASN1_MAX_RECURSIONS)
    واپس تہی;

  /* Process header byte. */
  elem->سرخی = beg;
  b = *beg++;
  elem->constructed = (b & 0x20) != 0;
  elem->eclass = (b >> 6) & 3;
  b &= 0x1F;
  اگر(b == 0x1F)
    واپس تہی; /* Long tag values not supported here. */
  elem->لیبل = b;

  /* Process length. */
  اگر(beg >= انتہا)
    واپس تہی;
  b = *beg++;
  اگر(!(b & 0x80))
    len = b;
  ورنہ {
    b &= 0x7F;
    اگر(!b) {
      /* Unspecified length. Since we have all the data, we can determine the
         effective length by skipping element until an EOC element is found. */
      اگر(!elem->constructed)
        واپس تہی;
      elem->beg = beg;
      جب(beg < انتہا && *beg) {
        beg = getASN1Element_(&lelem, beg, انتہا, lvl);
        اگر(!beg)
          واپس تہی;
      }
      اگر(انتہا - beg < 2 || beg[1])       /* EOC must have a zero length. */
        واپس تہی;
      elem->انتہا = beg;                  /* Do not include EOC as content. */
      واپس beg + 2;                   /* After EOC element. */
    }
    ورنہ اگر(b > (size_t)(انتہا - beg))
      واپس تہی; /* Does not fit in source. */
    ورنہ {
      /* Get long length. */
      len = 0;
      کرو {
        اگر(len & 0xFF000000L)
          واپس تہی;  /* Lengths > 32 bits are not supported. */
        len = (len << 8) | *beg++;
      } جب(--b);
    }
  }
  اگر(len > (size_t)(انتہا - beg))
    واپس تہی;  /* Element data does not fit in source. */
  elem->beg = beg;
  elem->انتہا = beg + len;
  واپس elem->انتہا;
}

/*
 * @unittest 1657
 */
UNITTEST ثابت uint8_t *getASN1Element(ساخت Curl_asn1Element *elem,
                                       ثابت uint8_t *beg, ثابت uint8_t *انتہا);
UNITTEST ثابت uint8_t *getASN1Element(ساخت Curl_asn1Element *elem,
                                       ثابت uint8_t *beg, ثابت uint8_t *انتہا)
{
  واپس getASN1Element_(elem, beg, انتہا, 0);
}

#اگر_تعریف_شدہ WANT_نکالو_CERTINFO

/*
 * Search the null-terminated OID or OID identifier in local table.
 * Return the table entry pointer or NULL if not found.
 */
ساکن ثابت ساخت Curl_OID *تلاش_OID(ثابت حرف *oid)
{
  ثابت ساخت Curl_OID *op;
  برائے(op = OIDtable; op->numoid; op++)
    اگر(!strcmp(op->numoid, oid) || curl_strequal(op->textoid, oid))
      واپس op;

  واپس تہی;
}

/*
 * Convert an ASN.1 Boolean value into its string representation.
 *
 * Return error code.
 */

ساکن CURLcode bool2str(ساخت dynbuf *store,
                         ثابت uint8_t *beg, ثابت uint8_t *انتہا)
{
  uint8_t val;
  اگر(انتہا - beg != 1)
    واپس CURLE_BAD_FUNCTION_دلیل;
  val = *beg;
  اگر((val != 0xff) && (val != 0x00))
    واپس CURLE_BAD_FUNCTION_دلیل;
  واپس curlx_dyn_جوڑو(store, val ? "TRUE" : "FALSE");
}

/*
 * Convert an ASN.1 octet string to a printable string.
 *
 * Return error code.
 */
ساکن CURLcode octet2str(ساخت dynbuf *store,
                          ثابت uint8_t *beg, ثابت uint8_t *انتہا)
{
  CURLcode نتیجہ = CURLE_OK;

  جب(!نتیجہ && beg < انتہا)
    نتیجہ = curlx_dyn_addf(store, "%02x:", *beg++);

  واپس نتیجہ;
}

ساکن CURLcode bit2str(ساخت dynbuf *store,
                        ثابت uint8_t *beg, ثابت uint8_t *انتہا)
{
  /* Convert an ASN.1 bit string to a printable string. */

  اگر(beg < انتہا) {
    uint8_t غیر_مستعمل_بٹ = *beg++;
    اگر(غیر_مستعمل_بٹ > 7)
      واپس CURLE_BAD_FUNCTION_دلیل;
  }
  ورنہ
    واپس CURLE_BAD_FUNCTION_دلیل;
  واپس octet2str(store, beg, انتہا);
}

/*
 * Convert an ASN.1 integer value into its string representation.
 *
 * Returns error.
 */
ساکن CURLcode int2str(ساخت dynbuf *store,
                        ثابت uint8_t *beg, ثابت uint8_t *انتہا)
{
  uint32_t uval = 0;
  int32_t sval = 0;
  size_t n = انتہا - beg;
  size_t i;
  ثابت uint8_t *p = beg;

  اگر(!n)
    واپس CURLE_BAD_FUNCTION_دلیل;

  اگر(n > 4)
    واپس octet2str(store, beg, انتہا);

  اگر(p[0] & 0x80)
    uval = ~0U;

  برائے(i = 0; i < n; i++)
    uval = (uval << 8) | p[i];

  sval = (int32_t)uval;

  /* small values are easier to read as decimals */
  اگر(sval > -10000 && sval < 10000)
    واپس curlx_dyn_addf(store, "%d", sval);

  /* represent larger values as hex */
  واپس curlx_dyn_addf(store, "0x%x", uval);
}

/*
 * Convert from an ASN.1 typed string to UTF8.
 *
 * The result is stored in a dynbuf that is inited by the user of this
 * function.
 *
 * Returns error.
 */
ساکن CURLcode utf8asn1str(ساخت dynbuf *مقصد, صحیح type,
                            ثابت uint8_t *from, ثابت uint8_t *انتہا)
{
  size_t inlength = انتہا - from;
  صحیح سائز = 1;
  CURLcode نتیجہ = CURLE_OK;

  انتخاب(type) {
  حالت CURL_ASN1_BMP_STRING:
    سائز = 2;
    توڑو;
  حالت CURL_ASN1_UNIVERSAL_STRING:
    سائز = 4;
    توڑو;
  حالت CURL_ASN1_NUMERIC_STRING:
  حالت CURL_ASN1_PRINTABLE_STRING:
  حالت CURL_ASN1_TELETEX_STRING:
  حالت CURL_ASN1_IA5_STRING:
  حالت CURL_ASN1_VISIBLE_STRING:
  حالت CURL_ASN1_UTF8_STRING:
    توڑو;
  طےشدہ:
    واپس CURLE_BAD_FUNCTION_دلیل;  /* Conversion not supported. */
  }

  اگر(inlength % سائز)
    /* Length inconsistent with character size. */
    واپس CURLE_BAD_FUNCTION_دلیل;

  اگر(type == CURL_ASN1_UTF8_STRING) {
    /* copy. */
    اگر(inlength)
      نتیجہ = curlx_dyn_addn(مقصد, from, inlength);
  }
  ورنہ {
    جب(!نتیجہ && (from < انتہا)) {
      حرف buf[4]; /* decode buffer */
      size_t charsize = 1;
      غیرعلامتی صحیح wc = 0;

      انتخاب(سائز) {
      حالت 4:
        wc = (wc << 8) | *from++;
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      حالت 2:
        wc = (wc << 8) | *from++;
        FALLTHROUGH();
      طےشدہ: /* case 1: */
        wc = (wc << 8) | *from++;
      }
      اگر(wc >= 0x00000080) {
        اگر(wc >= 0x00000800) {
          اگر(wc >= 0x00010000) {
            اگر(wc >= 0x00200000) {
              /* Invalid char. size for target encoding. */
              واپس CURLE_WEIRD_SERVER_جواب;
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
      نتیجہ = curlx_dyn_addn(مقصد, buf, charsize);
    }
  }
  واپس نتیجہ;
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
UNITTEST CURLcode encodeOID(ساخت dynbuf *store,
                            ثابت uint8_t *beg, ثابت uint8_t *انتہا);
UNITTEST CURLcode encodeOID(ساخت dynbuf *store,
                            ثابت uint8_t *beg, ثابت uint8_t *انتہا)
{
  uint32_t x;
  uint32_t y;
  CURLcode نتیجہ = CURLE_OK;

  اگر(انتہا <= beg)
    واپس CURLE_BAD_FUNCTION_دلیل;

  /* Process the first two numbers. The initial digit cannot be larger than
     2 */
  y = *beg++;
  اگر(y <= 80)
    x = y / 40;
  ورنہ
    x = 2;
  نتیجہ = curlx_dyn_addf(store, "%u", x);
  اگر(نتیجہ)
    واپس نتیجہ;

  اگر(y & 0x80) {
    uint32_t t = (y & 0x7f); /* the second number */
    اگر(!t)
      /* reject leading 0x80 padding */
      واپس CURLE_BAD_FUNCTION_دلیل;
    کرو {
      اگر((t & 0xFE000000) || (beg >= انتہا))
        واپس CURLE_BAD_FUNCTION_دلیل;
      y = *beg++;
      t = (t << 7) | (y & 0x7F);
    } جب(y & 0x80);
    y = t;
  }

  نتیجہ = curlx_dyn_addf(store, ".%u", y - (x * 40));
  اگر(نتیجہ)
    واپس نتیجہ;

  /* Process the trailing numbers. */
  جب((beg < انتہا) && !نتیجہ) {
    x = 0;
    y = 0;
    کرو {
      /* 0x80 as the first value is invalid since it only adds padding */
      اگر((y & 0x80) && !x)
        واپس CURLE_BAD_FUNCTION_دلیل;
      ورنہ اگر((x & 0xFE000000) || (beg == انتہا))
        واپس CURLE_BAD_FUNCTION_دلیل;
      y = *beg++;
      x = (x << 7) | (y & 0x7F);
    } جب(y & 0x80);
    نتیجہ = curlx_dyn_addf(store, ".%u", x);
  }
  واپس نتیجہ;
}

/*
 * Convert an ASN.1 OID into its dotted or symbolic string representation.
 *
 * Return error code.
 */

ساکن CURLcode OID2str(ساخت dynbuf *store,
                        ثابت uint8_t *beg, ثابت uint8_t *انتہا)
{
  CURLcode نتیجہ = CURLE_OK;
  اگر(beg < انتہا) {
    ساخت dynbuf buf;
    curlx_dyn_init(&buf, CURL_X509_STR_MAX);
    نتیجہ = encodeOID(&buf, beg, انتہا);

    اگر(!نتیجہ) {
      ثابت ساخت Curl_OID *op = تلاش_OID(curlx_dyn_ptr(&buf));
      اگر(op)
        نتیجہ = curlx_dyn_جوڑو(store, op->textoid);
      ورنہ
        نتیجہ = curlx_dyn_جوڑو(store, curlx_dyn_ptr(&buf));
    }
    curlx_dyn_free(&buf);
  }
  واپس نتیجہ;
}

/*
 * @unittest 1656
 */
UNITTEST CURLcode GTime2str(ساخت dynbuf *store,
                            ثابت حرف *beg, ثابت حرف *انتہا);
UNITTEST CURLcode GTime2str(ساخت dynbuf *store,
                            ثابت حرف *beg, ثابت حرف *انتہا)
{
  ثابت حرف *tzp;
  ثابت حرف *fracp;
  حرف sec1, sec2;
  size_t fracl;
  size_t tzl;
  ثابت حرف *sep = "";

  /* Convert an ASN.1 Generalized time to a printable string.
     Return the dynamically allocated string, or NULL if an error occurs. */

  برائے(fracp = beg; fracp < انتہا && ISDIGIT(*fracp); fracp++)
    ;

  /* Get seconds digits. */
  sec1 = '0';
  انتخاب(fracp - beg - 12) {
  حالت 0:
    sec2 = '0';
    توڑو;
  حالت 2:
    sec1 = fracp[-2];
    FALLTHROUGH();
  حالت 1:
    sec2 = fracp[-1];
    توڑو;
  طےشدہ:
    واپس CURLE_BAD_FUNCTION_دلیل;
  }

  /* timezone follows optional fractional seconds. */
  tzp = fracp;
  fracl = 0; /* no fractional seconds detected so far */
  اگر(fracp < انتہا && (*fracp == '.' || *fracp == ',')) {
    /* Have fractional seconds, e.g. "[.,]\d+". How many? */
    fracp++; /* should be a digit char or BAD ARGUMENT */
    tzp = fracp;
    جب(tzp < انتہا && ISDIGIT(*tzp))
      tzp++;
    اگر(tzp == fracp) /* never looped, no digit after [.,] */
      واپس CURLE_BAD_FUNCTION_دلیل;
    fracl = tzp - fracp; /* number of fractional sec digits */
    DEBUGASSERT(fracl > 0);
    /* Strip trailing zeroes in fractional seconds.
     * May reduce fracl to 0 if only '0's are present. */
    جب(fracl && fracp[fracl - 1] == '0')
      fracl--;
  }

  /* Process timezone. */
  اگر(tzp >= انتہا) {
    tzp = "";
    tzl = 0;
  }
  ورنہ اگر(*tzp == 'Z') {
    sep = " ";
    tzp = "GMT";
    tzl = 3;
  }
  ورنہ اگر((*tzp == '+') || (*tzp == '-')) {
    sep = " UTC";
    tzl = انتہا - tzp;
  }
  ورنہ {
    sep = " ";
    tzl = انتہا - tzp;
  }

  واپس curlx_dyn_addf(store,
                        "%.4s-%.2s-%.2s %.2s:%.2s:%c%c%s%.*s%s%.*s",
                        beg, beg + 4, beg + 6,
                        beg + 8, beg + 10, sec1, sec2,
                        fracl ? "." : "", (صحیح)fracl, fracp,
                        sep, (صحیح)tzl, tzp);
}

/*
 * Convert an ASN.1 UTC time to a printable string.
 *
 * Return error code.
 */
ساکن CURLcode UTime2str(ساخت dynbuf *store,
                          ثابت حرف *beg, ثابت حرف *انتہا)
{
  ثابت حرف *tzp;
  size_t tzl;
  ثابت حرف *sec;

  برائے(tzp = beg; tzp < انتہا && *tzp >= '0' && *tzp <= '9'; tzp++)
    ;
  /* Get the seconds. */
  sec = beg + 10;
  انتخاب(tzp - sec) {
  حالت 0:
    sec = "00";
    FALLTHROUGH();
  حالت 2:
    توڑو;
  طےشدہ:
    واپس CURLE_BAD_FUNCTION_دلیل;
  }

  /* Process timezone. */
  اگر(tzp >= انتہا)
    واپس CURLE_BAD_FUNCTION_دلیل;
  اگر(*tzp == 'Z') {
    tzp = "GMT";
    انتہا = tzp + 3;
  }

  tzl = انتہا - tzp;
  واپس curlx_dyn_addf(store, "%d%.2s-%.2s-%.2s %.2s:%.2s:%.2s %.*s",
                        20 - (*beg >= '5'), beg, beg + 2, beg + 4,
                        beg + 6, beg + 8, sec,
                        (صحیح)tzl, tzp);
}

/*
 * Convert an ASN.1 element to a printable string.
 *
 * Return error
 *
 * @unittest 1667
 */
UNITTEST CURLcode ASN1tostr(ساخت dynbuf *store,
                            ساخت Curl_asn1Element *elem);
UNITTEST CURLcode ASN1tostr(ساخت dynbuf *store,
                            ساخت Curl_asn1Element *elem)
{
  CURLcode نتیجہ = CURLE_BAD_FUNCTION_دلیل;
  صحیح type = elem->لیبل;
  اگر(elem->constructed)
    واپس نتیجہ; /* No conversion of structured elements. */

  انتخاب(type) {
  حالت CURL_ASN1_BOOLEAN:
    نتیجہ = bool2str(store, elem->beg, elem->انتہا);
    توڑو;
  حالت CURL_ASN1_INTEGER:
  حالت CURL_ASN1_ENUMERATED:
    نتیجہ = int2str(store, elem->beg, elem->انتہا);
    توڑو;
  حالت CURL_ASN1_ثنائی_ہندسہ_STRING:
    نتیجہ = bit2str(store, elem->beg, elem->انتہا);
    توڑو;
  حالت CURL_ASN1_OCTET_STRING:
    نتیجہ = octet2str(store, elem->beg, elem->انتہا);
    توڑو;
  حالت CURL_ASN1_NULL:
    نتیجہ = curlx_dyn_addn(store, "", 1);
    توڑو;
  حالت CURL_ASN1_OBJECT_IDENTIFIER:
    نتیجہ = OID2str(store, elem->beg, elem->انتہا);
    توڑو;
  حالت CURL_ASN1_UTC_وقت:
    نتیجہ = UTime2str(store, (ثابت حرف *)elem->beg,
                       (ثابت حرف *)elem->انتہا);
    توڑو;
  حالت CURL_ASN1_GENERALIZED_وقت:
    نتیجہ = GTime2str(store, (ثابت حرف *)elem->beg,
                       (ثابت حرف *)elem->انتہا);
    توڑو;
  حالت CURL_ASN1_UTF8_STRING:
  حالت CURL_ASN1_NUMERIC_STRING:
  حالت CURL_ASN1_PRINTABLE_STRING:
  حالت CURL_ASN1_TELETEX_STRING:
  حالت CURL_ASN1_IA5_STRING:
  حالت CURL_ASN1_VISIBLE_STRING:
  حالت CURL_ASN1_UNIVERSAL_STRING:
  حالت CURL_ASN1_BMP_STRING:
    نتیجہ = utf8asn1str(store, type, elem->beg, elem->انتہا);
    توڑو;
  }

  واپس نتیجہ;
}

/*
 * ASCII encode distinguished name at `dn' into the store dynbuf.
 *
 * Returns error.
 */
ساکن CURLcode encodeDN(ساخت dynbuf *store, ساخت Curl_asn1Element *dn)
{
  ساخت Curl_asn1Element rdn;
  ساخت Curl_asn1Element atv;
  ساخت Curl_asn1Element oid;
  ساخت Curl_asn1Element قدر;
  ثابت uint8_t *p1;
  ثابت uint8_t *p2;
  ثابت uint8_t *p3;
  ثابت uint8_t *str;
  CURLcode نتیجہ = CURLE_OK;
  bool added = FALSE;
  ساخت dynbuf عارضی;
  curlx_dyn_init(&عارضی, CURL_X509_STR_MAX);

  برائے(p1 = dn->beg; p1 < dn->انتہا;) {
    p1 = getASN1Element(&rdn, p1, dn->انتہا);
    اگر(!p1) {
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
      جاؤ error;
    }
    برائے(p2 = rdn.beg; p2 < rdn.انتہا;) {
      p2 = getASN1Element(&atv, p2, rdn.انتہا);
      اگر(!p2) {
        نتیجہ = CURLE_BAD_FUNCTION_دلیل;
        جاؤ error;
      }
      p3 = getASN1Element(&oid, atv.beg, atv.انتہا);
      اگر(!p3) {
        نتیجہ = CURLE_BAD_FUNCTION_دلیل;
        جاؤ error;
      }
      اگر(!getASN1Element(&قدر, p3, atv.انتہا)) {
        نتیجہ = CURLE_BAD_FUNCTION_دلیل;
        جاؤ error;
      }
      curlx_dyn_reset(&عارضی);
      نتیجہ = ASN1tostr(&عارضی, &oid);
      اگر(نتیجہ)
        جاؤ error;

      str = curlx_dyn_uptr(&عارضی);

      اگر(!str) {
        نتیجہ = CURLE_BAD_FUNCTION_دلیل;
        جاؤ error;
      }

      /* Encode delimiter.
         If attribute has a short uppercase name, delimiter is ", ". */
      برائے(p3 = str; ISUPPER(*p3); p3++)
        ;
      اگر(added) {
        اگر(p3 - str > 2)
          نتیجہ = curlx_dyn_addn(store, "/", 1);
        ورنہ
          نتیجہ = curlx_dyn_addn(store, ", ", 2);
        اگر(نتیجہ)
          جاؤ error;
      }

      /* Encode attribute name. */
      نتیجہ = curlx_dyn_جوڑو(store, (ثابت حرف *)str);
      اگر(نتیجہ)
        جاؤ error;

      /* Generate equal sign. */
      نتیجہ = curlx_dyn_addn(store, "=", 1);
      اگر(نتیجہ)
        جاؤ error;

      /* Generate value. */
      نتیجہ = ASN1tostr(store, &قدر);
      اگر(نتیجہ)
        جاؤ error;
      curlx_dyn_reset(&عارضی);
      added = TRUE; /* use separator for next */
    }
  }
error:
  curlx_dyn_free(&عارضی);

  واپس نتیجہ;
}

#اگر_ختم /* WANT_EXTRACT_CERTINFO */

#اگر_تعریف_شدہ WANT_PARSEX509
/*
 * ASN.1 parse an X509 certificate into structure subfields.
 * Syntax is assumed to have already been checked by the SSL backend.
 * See RFC 5280.
 */
صحیح Curl_parseX509(ساخت Curl_X509certificate *cert,
                   ثابت uint8_t *beg, ثابت uint8_t *انتہا)
{
  ساخت Curl_asn1Element elem;
  ساخت Curl_asn1Element tbsCertificate;
  ثابت uint8_t *ccp;
  ساکن ثابت uint8_t default_نسخہ = 0;  /* v1. */

  memset(cert, 0, حجم(*cert));
  cert->certificate.سرخی = تہی;
  cert->certificate.beg = beg;
  cert->certificate.انتہا = انتہا;

  /* Get the sequence content. */
  اگر(!getASN1Element(&elem, beg, انتہا))
    واپس -1;  /* Invalid bounds/size. */
  beg = elem.beg;
  انتہا = elem.انتہا;

  /* Get tbsCertificate. */
  beg = getASN1Element(&tbsCertificate, beg, انتہا);
  اگر(!beg)
    واپس -1;
  /* Skip the signatureAlgorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, انتہا);
  اگر(!beg)
    واپس -1;
  /* Get the signatureValue. */
  اگر(!getASN1Element(&cert->signature, beg, انتہا))
    واپس -1;

  /* Parse TBSCertificate. */
  beg = tbsCertificate.beg;
  انتہا = tbsCertificate.انتہا;
  /* Get optional version, get serialNumber. */
  cert->نسخہ_2.سرخی = تہی;
  cert->نسخہ_2.beg = &default_نسخہ;
  cert->نسخہ_2.انتہا = &default_نسخہ + حجم(default_نسخہ);
  beg = getASN1Element(&elem, beg, انتہا);
  اگر(!beg)
    واپس -1;
  اگر(elem.لیبل == 0) {
    اگر(!getASN1Element(&cert->نسخہ_2, elem.beg, elem.انتہا))
      واپس -1;
    beg = getASN1Element(&elem, beg, انتہا);
    اگر(!beg)
      واپس -1;
  }
  cert->serial_نمبر = elem;
  /* Get signature algorithm. */
  beg = getASN1Element(&cert->signatureAlgorithm, beg, انتہا);
  /* Get issuer. */
  beg = getASN1Element(&cert->issuer, beg, انتہا);
  اگر(!beg)
    واپس -1;
  /* Get notBefore and notAfter. */
  beg = getASN1Element(&elem, beg, انتہا);
  اگر(!beg)
    واپس -1;
  ccp = getASN1Element(&cert->notBefore, elem.beg, elem.انتہا);
  اگر(!ccp)
    واپس -1;
  اگر(!getASN1Element(&cert->notAfter, ccp, elem.انتہا))
    واپس -1;
  /* These two fields MUST consume all of the validity elem */
  اگر(elem.انتہا != cert->notAfter.انتہا)
    واپس -1;
  /* Get subject. */
  beg = getASN1Element(&cert->subject, beg, انتہا);
  اگر(!beg)
    واپس -1;
  /* Get subjectPublicKeyAlgorithm and subjectPublicKey. */
  beg = getASN1Element(&cert->subject_عوامی_کلید_Info, beg, انتہا);
  اگر(!beg)
    واپس -1;
  ccp = getASN1Element(&cert->subject_عوامی_کلید_Algorithm,
                       cert->subject_عوامی_کلید_Info.beg,
                       cert->subject_عوامی_کلید_Info.انتہا);
  اگر(!ccp)
    واپس -1;
  اگر(!getASN1Element(&cert->subject_عوامی_کلید, ccp,
                     cert->subject_عوامی_کلید_Info.انتہا))
    واپس -1;
  /* Get optional issuerUniqueID, subjectUniqueID and extensions. */
  cert->issuerUniqueID.لیبل = cert->subjectUniqueID.لیبل = 0;
  cert->extensions.لیبل = elem.لیبل = 0;
  cert->issuerUniqueID.سرخی = cert->subjectUniqueID.سرخی = تہی;
  cert->issuerUniqueID.beg = cert->issuerUniqueID.انتہا = beg;
  cert->subjectUniqueID.beg = cert->subjectUniqueID.انتہا = beg;
  cert->extensions.سرخی = تہی;
  cert->extensions.beg = cert->extensions.انتہا = beg;
  اگر(beg < انتہا) {
    beg = getASN1Element(&elem, beg, انتہا);
    اگر(!beg)
      واپس -1;
  }
  اگر(elem.لیبل == 1) {
    cert->issuerUniqueID = elem;
    اگر(beg < انتہا) {
      beg = getASN1Element(&elem, beg, انتہا);
      اگر(!beg)
        واپس -1;
    }
  }
  اگر(elem.لیبل == 2) {
    cert->subjectUniqueID = elem;
    اگر(beg < انتہا) {
      beg = getASN1Element(&elem, beg, انتہا);
      اگر(!beg)
        واپس -1;
    }
  }
  اگر(elem.لیبل == 3)
    اگر(!getASN1Element(&cert->extensions, elem.beg, elem.انتہا))
      واپس -1;
  واپس 0;
}

#اگر_ختم /* WANT_PARSEX509 */

#اگر_تعریف_شدہ WANT_نکالو_CERTINFO

ساکن CURLcode dumpAlgo(ساخت dynbuf *store,
                         ساخت Curl_asn1Element *param,
                         ثابت uint8_t *beg, ثابت uint8_t *انتہا)
{
  ساخت Curl_asn1Element oid;

  /* Get algorithm parameters and return algorithm name. */

  beg = getASN1Element(&oid, beg, انتہا);
  اگر(!beg)
    واپس CURLE_BAD_FUNCTION_دلیل;
  param->سرخی = تہی;
  param->لیبل = 0;
  param->beg = param->انتہا = انتہا;
  اگر(beg < انتہا) {
    ثابت uint8_t *p = getASN1Element(param, beg, انتہا);
    اگر(!p)
      واپس CURLE_BAD_FUNCTION_دلیل;
  }
  واپس OID2str(store, oid.beg, oid.انتہا);
}

/*
 * This is a convenience function for push_certinfo_len that takes a zero
 * terminated value.
 */
ساکن CURLcode ssl_ڈالو_certinfo(ساخت Curl_easy *ڈیٹا,
                                  صحیح certnum,
                                  ثابت حرف *label,
                                  ثابت حرف *قدر)
{
  size_t valuelen = لڑی_لمبائی(قدر);

  واپس Curl_ssl_ڈالو_certinfo_len(ڈیٹا, certnum, label, قدر, valuelen);
}

/*
 * This is a convenience function for push_certinfo_len that takes a
 * dynbuf value.
 *
 * It also does the verbose output if !certnum.
 */
ساکن CURLcode ssl_ڈالو_certinfo_dyn(ساخت Curl_easy *ڈیٹا,
                                      صحیح certnum,
                                      ثابت حرف *label,
                                      ساخت dynbuf *ptr)
{
  size_t valuelen = curlx_dyn_len(ptr);
  ثابت حرف *قدر = curlx_dyn_ptr(ptr);

  CURLcode نتیجہ = Curl_ssl_ڈالو_certinfo_len(ڈیٹا, certnum, label,
                                               قدر, valuelen);

  اگر(!certnum && !نتیجہ)
    infof(ڈیٹا, "   %s: %s", label, قدر);

  واپس نتیجہ;
}

ساکن CURLcode do_pubkey_میدان(ساخت Curl_easy *ڈیٹا, صحیح certnum,
                                ثابت حرف *label,
                                ساخت Curl_asn1Element *elem)
{
  CURLcode نتیجہ;
  ساخت dynbuf برآمد;

  curlx_dyn_init(&برآمد, CURL_X509_STR_MAX);

  /* Generate a certificate information record for the public key. */

  نتیجہ = ASN1tostr(&برآمد, elem);
  اگر(!نتیجہ && ڈیٹا->سیٹ.ssl.certinfo)
    نتیجہ = ssl_ڈالو_certinfo_dyn(ڈیٹا, certnum, label, &برآمد);
  curlx_dyn_free(&برآمد);
  واپس نتیجہ;
}

/* return 0 on success, 1 on error */
ساکن صحیح do_pubkey(ساخت Curl_easy *ڈیٹا, صحیح certnum, ثابت حرف *algo,
                     ساخت Curl_asn1Element *param,
                     ساخت Curl_asn1Element *pubkey)
{
  ساخت Curl_asn1Element elem;
  ساخت Curl_asn1Element pk;
  ثابت uint8_t *p;

  /* Generate all information records for the public key. */

  اگر(curl_strequal(algo, "ecPublicKey")) {
    /*
     * ECC public key is all the data, a value of type BIT STRING mapped to
     * OCTET STRING and should not be parsed as an ASN.1 value.
     */
    ثابت size_t dlen = pubkey->انتہا - pubkey->beg;
    size_t len;
    اگر(dlen < 2)
      /* too small */
      واپس 1;
    len = (dlen - 2) * 4;
    اگر(!certnum)
      infof(ڈیٹا, "   ECC Public Key (%zu bits)", len);
    اگر(ڈیٹا->سیٹ.ssl.certinfo) {
      حرف q[(حجم(len) * 8 / 3) + 1];
      (خالی)curl_msnprintf(q, حجم(q), "%zu", len);
      اگر(ssl_ڈالو_certinfo(ڈیٹا, certnum, "ECC Public Key", q))
        واپس 1;
    }
    واپس do_pubkey_میدان(ڈیٹا, certnum, "ecPublicKey", pubkey) == CURLE_OK
      ? 0 : 1;
  }

  /* Get the public key (single element). */
  اگر(!getASN1Element(&pk, pubkey->beg + 1, pubkey->انتہا))
    واپس 1;

  اگر(curl_strequal(algo, "rsaEncryption")) {
    ثابت uint8_t *q;
    size_t len;

    p = getASN1Element(&elem, pk.beg, pk.انتہا);
    اگر(!p)
      واپس 1;

    /* Compute key length. */
    برائے(q = elem.beg; q < elem.انتہا && !*q; q++)
      ;
    len = ((elem.انتہا - q) * 8);
    اگر(len) {
      غیرعلامتی صحیح i;
      برائے(i = *q; !(i & 0x80); i <<= 1)
        len--;
    }
    اگر(len > 32)
      elem.beg = q;     /* Strip leading zero bytes. */
    اگر(!certnum)
      infof(ڈیٹا, "   RSA Public Key (%zu bits)", len);
    اگر(ڈیٹا->سیٹ.ssl.certinfo) {
      حرف r[(حجم(len) * 8 / 3) + 1];
      curl_msnprintf(r, حجم(r), "%zu", len);
      اگر(ssl_ڈالو_certinfo(ڈیٹا, certnum, "RSA Public Key", r))
        واپس 1;
    }
    /* Generate coefficients. */
    اگر(do_pubkey_میدان(ڈیٹا, certnum, "rsa(n)", &elem))
      واپس 1;
    اگر(!getASN1Element(&elem, p, pk.انتہا))
      واپس 1;
    اگر(do_pubkey_میدان(ڈیٹا, certnum, "rsa(e)", &elem))
      واپس 1;
  }
  ورنہ اگر(curl_strequal(algo, "dsa")) {
    p = getASN1Element(&elem, param->beg, param->انتہا);
    اگر(p) {
      اگر(do_pubkey_میدان(ڈیٹا, certnum, "dsa(p)", &elem))
        واپس 1;
      p = getASN1Element(&elem, p, param->انتہا);
      اگر(p) {
        اگر(do_pubkey_میدان(ڈیٹا, certnum, "dsa(q)", &elem))
          واپس 1;
        اگر(getASN1Element(&elem, p, param->انتہا)) {
          اگر(do_pubkey_میدان(ڈیٹا, certnum, "dsa(g)", &elem))
            واپس 1;
          اگر(do_pubkey_میدان(ڈیٹا, certnum, "dsa(pub_key)", &pk))
            واپس 1;
        }
      }
    }
  }
  ورنہ اگر(curl_strequal(algo, "dhpublicnumber")) {
    p = getASN1Element(&elem, param->beg, param->انتہا);
    اگر(p) {
      اگر(do_pubkey_میدان(ڈیٹا, certnum, "dh(p)", &elem))
        واپس 1;
      اگر(getASN1Element(&elem, p, param->انتہا)) {
        اگر(do_pubkey_میدان(ڈیٹا, certnum, "dh(g)", &elem))
          واپس 1;
        اگر(do_pubkey_میدان(ڈیٹا, certnum, "dh(pub_key)", &pk))
          واپس 1;
      }
    }
  }
  واپس 0;
}

/*
 * Convert an ASN.1 distinguished name into a printable string.
 * Return error.
 */
ساکن CURLcode DNtostr(ساخت dynbuf *store, ساخت Curl_asn1Element *dn)
{
  واپس encodeDN(store, dn);
}

CURLcode Curl_نکالو_certinfo(ساخت Curl_easy *ڈیٹا,
                               صحیح certnum,
                               ثابت uint8_t *beg,
                               ثابت uint8_t *انتہا)
{
  ساخت Curl_X509certificate cert;
  ساخت Curl_asn1Element param;
  حرف *certptr;
  size_t clen;
  ساخت dynbuf برآمد;
  CURLcode نتیجہ = CURLE_OK;
  غیرعلامتی صحیح نسخہ_2;
  ثابت uint8_t *ptr;
  صحیح rc;

  اگر(!ڈیٹا->سیٹ.ssl.certinfo)
    اگر(certnum)
      واپس CURLE_OK;

  curlx_dyn_init(&برآمد, CURL_X509_STR_MAX);
  /* Prepare the certificate information for curl_easy_getinfo(). */

  /* Extract the certificate ASN.1 elements. */
  اگر(Curl_parseX509(&cert, beg, انتہا))
    واپس CURLE_PEER_FAILED_VERIFICATION;

  /* Subject. */
  نتیجہ = DNtostr(&برآمد, &cert.subject);
  اگر(نتیجہ)
    جاؤ مکمل;
  اگر(ڈیٹا->سیٹ.ssl.certinfo) {
    نتیجہ = ssl_ڈالو_certinfo_dyn(ڈیٹا, certnum, "Subject", &برآمد);
    اگر(نتیجہ)
      جاؤ مکمل;
  }
  curlx_dyn_reset(&برآمد);

  /* Issuer. */
  نتیجہ = DNtostr(&برآمد, &cert.issuer);
  اگر(نتیجہ)
    جاؤ مکمل;
  اگر(ڈیٹا->سیٹ.ssl.certinfo) {
    نتیجہ = ssl_ڈالو_certinfo_dyn(ڈیٹا, certnum, "Issuer", &برآمد);
    اگر(نتیجہ)
      جاؤ مکمل;
  }
  curlx_dyn_reset(&برآمد);

  /* Version (always fits in less than 32 bits). */
  نسخہ_2 = 0;
  برائے(ptr = cert.نسخہ_2.beg; ptr < cert.نسخہ_2.انتہا; ptr++)
    نسخہ_2 = (نسخہ_2 << 8) | *ptr;
  اگر(ڈیٹا->سیٹ.ssl.certinfo) {
    نتیجہ = curlx_dyn_addf(&برآمد, "%x", نسخہ_2);
    اگر(نتیجہ)
      جاؤ مکمل;
    نتیجہ = ssl_ڈالو_certinfo_dyn(ڈیٹا, certnum, "Version", &برآمد);
    اگر(نتیجہ)
      جاؤ مکمل;
    curlx_dyn_reset(&برآمد);
  }

  /* Serial number. */
  نتیجہ = ASN1tostr(&برآمد, &cert.serial_نمبر);
  اگر(نتیجہ)
    جاؤ مکمل;
  اگر(ڈیٹا->سیٹ.ssl.certinfo) {
    نتیجہ = ssl_ڈالو_certinfo_dyn(ڈیٹا, certnum, "Serial Number", &برآمد);
    اگر(نتیجہ)
      جاؤ مکمل;
  }
  curlx_dyn_reset(&برآمد);

  /* Signature algorithm .*/
  نتیجہ = dumpAlgo(&برآمد, &param, cert.signatureAlgorithm.beg,
                    cert.signatureAlgorithm.انتہا);
  اگر(نتیجہ)
    جاؤ مکمل;
  اگر(ڈیٹا->سیٹ.ssl.certinfo) {
    نتیجہ = ssl_ڈالو_certinfo_dyn(ڈیٹا, certnum, "Signature Algorithm", &برآمد);
    اگر(نتیجہ)
      جاؤ مکمل;
  }
  curlx_dyn_reset(&برآمد);

  /* Start Date. */
  نتیجہ = ASN1tostr(&برآمد, &cert.notBefore);
  اگر(نتیجہ)
    جاؤ مکمل;
  اگر(ڈیٹا->سیٹ.ssl.certinfo) {
    نتیجہ = ssl_ڈالو_certinfo_dyn(ڈیٹا, certnum, "Start Date", &برآمد);
    اگر(نتیجہ)
      جاؤ مکمل;
  }
  curlx_dyn_reset(&برآمد);

  /* Expire Date. */
  نتیجہ = ASN1tostr(&برآمد, &cert.notAfter);
  اگر(نتیجہ)
    جاؤ مکمل;
  اگر(ڈیٹا->سیٹ.ssl.certinfo) {
    نتیجہ = ssl_ڈالو_certinfo_dyn(ڈیٹا, certnum, "Expire Date", &برآمد);
    اگر(نتیجہ)
      جاؤ مکمل;
  }
  curlx_dyn_reset(&برآمد);

  /* Public Key Algorithm. */
  نتیجہ = dumpAlgo(&برآمد, &param, cert.subject_عوامی_کلید_Algorithm.beg,
                    cert.subject_عوامی_کلید_Algorithm.انتہا);
  اگر(نتیجہ)
    جاؤ مکمل;
  اگر(ڈیٹا->سیٹ.ssl.certinfo) {
    نتیجہ = ssl_ڈالو_certinfo_dyn(ڈیٹا, certnum, "Public Key Algorithm",
                                   &برآمد);
    اگر(نتیجہ)
      جاؤ مکمل;
  }

  rc = do_pubkey(ڈیٹا, certnum, curlx_dyn_ptr(&برآمد),
                 &param, &cert.subject_عوامی_کلید);
  اگر(rc) {
    نتیجہ = CURLE_برآمد_کا_حافظہ; /* the most likely error */
    جاؤ مکمل;
  }
  curlx_dyn_reset(&برآمد);

  /* Signature. */
  نتیجہ = ASN1tostr(&برآمد, &cert.signature);
  اگر(نتیجہ)
    جاؤ مکمل;
  اگر(ڈیٹا->سیٹ.ssl.certinfo) {
    نتیجہ = ssl_ڈالو_certinfo_dyn(ڈیٹا, certnum, "Signature", &برآمد);
    اگر(نتیجہ)
      جاؤ مکمل;
  }
  curlx_dyn_reset(&برآمد);

  /* Generate PEM certificate. */
  نتیجہ = curlx_base64_encode((ثابت uint8_t *)cert.certificate.beg,
                               cert.certificate.انتہا - cert.certificate.beg,
                               &certptr, &clen);
  اگر(نتیجہ)
    جاؤ مکمل;

  /* Generate the final output certificate string. Format is:
     -----BEGIN CERTIFICATE-----\n
     <max 64 base64 characters>\n
     .
     .
     .
     -----END CERTIFICATE-----\n
   */

  curlx_dyn_reset(&برآمد);

  /* Build the certificate string. */
  نتیجہ = curlx_dyn_جوڑو(&برآمد, "-----BEGIN CERTIFICATE-----\n");
  اگر(!نتیجہ) {
    size_t j = 0;

    جب(!نتیجہ && (j < clen)) {
      size_t chunksize = (clen - j) > 64 ? 64 : (clen - j);
      نتیجہ = curlx_dyn_addn(&برآمد, &certptr[j], chunksize);
      اگر(!نتیجہ)
        نتیجہ = curlx_dyn_addn(&برآمد, "\n", 1);
      j += chunksize;
    }
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_جوڑو(&برآمد, "-----END CERTIFICATE-----\n");
  }
  curlx_free(certptr);
  اگر(!نتیجہ)
    اگر(ڈیٹا->سیٹ.ssl.certinfo)
      نتیجہ = ssl_ڈالو_certinfo_dyn(ڈیٹا, certnum, "Cert", &برآمد);

مکمل:
  اگر(نتیجہ)
    failf(ڈیٹا, "Failed extracting certificate chain");
  curlx_dyn_free(&برآمد);
  واپس نتیجہ;
}

#اگر_ختم /* WANT_EXTRACT_CERTINFO */

#اگر_ختم /* USE_GNUTLS || USE_WOLFSSL || USE_SCHANNEL || USE_MBEDTLS ||
          USE_RUSTLS */
