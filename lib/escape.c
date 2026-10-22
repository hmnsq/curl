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
/* Escape and unescape URL encoding in strings. The functions return a new
 * allocated string or NULL if an error occurred. */
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

ರಚನೆ Curl_easy;

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "curl_printf.h"

/* for ABI-compatibility with previous versions */
ಅಕ್ಷರ *curl_escape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string, ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ)
{
  ಹಿಂತಿರುಗಿ curl_easy_escape(ಖಾಲಿ, string, ಉದ್ದಳತೆ);
}

/* for ABI-compatibility with previous versions */
ಅಕ್ಷರ *curl_unescape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string, ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ)
{
  ಹಿಂತಿರುಗಿ curl_easy_unescape(ಖಾಲಿ, string, ಉದ್ದಳತೆ, ಖಾಲಿ);
}

#ವ್ಯಾಖ್ಯಾನಿಸು NOPE 0xff
#ವ್ಯಾಖ್ಯಾನಿಸು _OK_ 0x1f /* octet not needing encoding but not a hexadecimal
                     character */

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ hextable[256] = {
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, /* 00 */
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, /* 08 */
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, /* 10 */
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, /* 18 */
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, /* 20 */
  NOPE, NOPE, NOPE, NOPE, NOPE, _OK_, _OK_, NOPE, /* 28 */
  0,    1,    2,    3,    4,    5,    6,    7,    /* 30 */
  8,    9,    NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, /* 38 */
  NOPE, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, _OK_, /* 40 */
  _OK_, _OK_, _OK_, _OK_, _OK_, _OK_, _OK_, _OK_, /* 48 */
  _OK_, _OK_, _OK_, _OK_, _OK_, _OK_, _OK_, _OK_, /* 50 */
  _OK_, _OK_, _OK_, NOPE, NOPE, NOPE, NOPE, _OK_, /* 58 */
  NOPE, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, _OK_, /* 60 */
  _OK_, _OK_, _OK_, _OK_, _OK_, _OK_, _OK_, _OK_, /* 68 */
  _OK_, _OK_, _OK_, _OK_, _OK_, _OK_, _OK_, _OK_, /* 70 */
  _OK_, _OK_, _OK_, NOPE, NOPE, NOPE, _OK_, NOPE, /* 78 */
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, /* 80 - not ASCII */
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE,
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE,
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE,
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE,
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE,
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE,
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE,
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE,
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE,
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE,
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE,
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE,
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE,
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE,
  NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE, NOPE
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ hexpairs[][2] = {
  {'0', '0'}, {'0', '1'}, {'0', '2'}, {'0', '3'},
  {'0', '4'}, {'0', '5'}, {'0', '6'}, {'0', '7'},
  {'0', '8'}, {'0', '9'}, {'0', 'A'}, {'0', 'B'},
  {'0', 'C'}, {'0', 'D'}, {'0', 'E'}, {'0', 'F'},
  {'1', '0'}, {'1', '1'}, {'1', '2'}, {'1', '3'},
  {'1', '4'}, {'1', '5'}, {'1', '6'}, {'1', '7'},
  {'1', '8'}, {'1', '9'}, {'1', 'A'}, {'1', 'B'},
  {'1', 'C'}, {'1', 'D'}, {'1', 'E'}, {'1', 'F'},
  {'2', '0'}, {'2', '1'}, {'2', '2'}, {'2', '3'},
  {'2', '4'}, {'2', '5'}, {'2', '6'}, {'2', '7'},
  {'2', '8'}, {'2', '9'}, {'2', 'A'}, {'2', 'B'},
  {'2', 'C'}, {'2', 'D'}, {'2', 'E'}, {'2', 'F'},
  {'3', '0'}, {'3', '1'}, {'3', '2'}, {'3', '3'},
  {'3', '4'}, {'3', '5'}, {'3', '6'}, {'3', '7'},
  {'3', '8'}, {'3', '9'}, {'3', 'A'}, {'3', 'B'},
  {'3', 'C'}, {'3', 'D'}, {'3', 'E'}, {'3', 'F'},
  {'4', '0'}, {'4', '1'}, {'4', '2'}, {'4', '3'},
  {'4', '4'}, {'4', '5'}, {'4', '6'}, {'4', '7'},
  {'4', '8'}, {'4', '9'}, {'4', 'A'}, {'4', 'B'},
  {'4', 'C'}, {'4', 'D'}, {'4', 'E'}, {'4', 'F'},
  {'5', '0'}, {'5', '1'}, {'5', '2'}, {'5', '3'},
  {'5', '4'}, {'5', '5'}, {'5', '6'}, {'5', '7'},
  {'5', '8'}, {'5', '9'}, {'5', 'A'}, {'5', 'B'},
  {'5', 'C'}, {'5', 'D'}, {'5', 'E'}, {'5', 'F'},
  {'6', '0'}, {'6', '1'}, {'6', '2'}, {'6', '3'},
  {'6', '4'}, {'6', '5'}, {'6', '6'}, {'6', '7'},
  {'6', '8'}, {'6', '9'}, {'6', 'A'}, {'6', 'B'},
  {'6', 'C'}, {'6', 'D'}, {'6', 'E'}, {'6', 'F'},
  {'7', '0'}, {'7', '1'}, {'7', '2'}, {'7', '3'},
  {'7', '4'}, {'7', '5'}, {'7', '6'}, {'7', '7'},
  {'7', '8'}, {'7', '9'}, {'7', 'A'}, {'7', 'B'},
  {'7', 'C'}, {'7', 'D'}, {'7', 'E'}, {'7', 'F'},
  {'8', '0'}, {'8', '1'}, {'8', '2'}, {'8', '3'},
  {'8', '4'}, {'8', '5'}, {'8', '6'}, {'8', '7'},
  {'8', '8'}, {'8', '9'}, {'8', 'A'}, {'8', 'B'},
  {'8', 'C'}, {'8', 'D'}, {'8', 'E'}, {'8', 'F'},
  {'9', '0'}, {'9', '1'}, {'9', '2'}, {'9', '3'},
  {'9', '4'}, {'9', '5'}, {'9', '6'}, {'9', '7'},
  {'9', '8'}, {'9', '9'}, {'9', 'A'}, {'9', 'B'},
  {'9', 'C'}, {'9', 'D'}, {'9', 'E'}, {'9', 'F'},
  {'A', '0'}, {'A', '1'}, {'A', '2'}, {'A', '3'},
  {'A', '4'}, {'A', '5'}, {'A', '6'}, {'A', '7'},
  {'A', '8'}, {'A', '9'}, {'A', 'A'}, {'A', 'B'},
  {'A', 'C'}, {'A', 'D'}, {'A', 'E'}, {'A', 'F'},
  {'B', '0'}, {'B', '1'}, {'B', '2'}, {'B', '3'},
  {'B', '4'}, {'B', '5'}, {'B', '6'}, {'B', '7'},
  {'B', '8'}, {'B', '9'}, {'B', 'A'}, {'B', 'B'},
  {'B', 'C'}, {'B', 'D'}, {'B', 'E'}, {'B', 'F'},
  {'C', '0'}, {'C', '1'}, {'C', '2'}, {'C', '3'},
  {'C', '4'}, {'C', '5'}, {'C', '6'}, {'C', '7'},
  {'C', '8'}, {'C', '9'}, {'C', 'A'}, {'C', 'B'},
  {'C', 'C'}, {'C', 'D'}, {'C', 'E'}, {'C', 'F'},
  {'D', '0'}, {'D', '1'}, {'D', '2'}, {'D', '3'},
  {'D', '4'}, {'D', '5'}, {'D', '6'}, {'D', '7'},
  {'D', '8'}, {'D', '9'}, {'D', 'A'}, {'D', 'B'},
  {'D', 'C'}, {'D', 'D'}, {'D', 'E'}, {'D', 'F'},
  {'E', '0'}, {'E', '1'}, {'E', '2'}, {'E', '3'},
  {'E', '4'}, {'E', '5'}, {'E', '6'}, {'E', '7'},
  {'E', '8'}, {'E', '9'}, {'E', 'A'}, {'E', 'B'},
  {'E', 'C'}, {'E', 'D'}, {'E', 'E'}, {'E', 'F'},
  {'F', '0'}, {'F', '1'}, {'F', '2'}, {'F', '3'},
  {'F', '4'}, {'F', '5'}, {'F', '6'}, {'F', '7'},
  {'F', '8'}, {'F', '9'}, {'F', 'A'}, {'F', 'B'},
  {'F', 'C'}, {'F', 'D'}, {'F', 'E'}, {'F', 'F'}
};

/* Escapes for URL the given unescaped string of given length.
 * 'data' is ignored since 7.82.0.
 */
ಅಕ್ಷರ *curl_easy_escape(CURL *curl, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string, ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ)
{
  size_t len;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f = string;
  size_t nappend = 0;
  ಅಕ್ಷರ *ಗುರಿ;
  ಅಕ್ಷರ *encoded;
  (ಶೂನ್ಯ)curl;

  ಆದರೆ(!string || (ಉದ್ದಳತೆ < 0))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  len = (ಉದ್ದಳತೆ ? (size_t)ಉದ್ದಳತೆ : ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(string));
  ಆದರೆ(!len)
    ಹಿಂತಿರುಗಿ curlx_strdup("");

  ಆದರೆ(len > SIZE_MAX / 16)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  encoded = ಗುರಿ = curlx_malloc((len * 3) + 1);
  ಆದರೆ(!ಗುರಿ)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  ಆಗ(len--) {
    uint8_t ಒಳ = (uint8_t)*string++;

    ಆದರೆ(!(hextable[ಒಳ] & 0x80))
      /* append this */
      nappend++;
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(nappend) {
        ಸ್ಮರಣೆ_ನಕಲು(ಗುರಿ, f, nappend);
        ಗುರಿ += nappend;
        nappend = 0;
      }
      f = string;
      /* encode it */
      ಗುರಿ[0] = '%';
      ಸ್ಮರಣೆ_ನಕಲು(ಗುರಿ + 1, hexpairs[ಒಳ], 2);
      ಗುರಿ += 3;
    }
  }
  ಆದರೆ(nappend) {
    ಸ್ಮರಣೆ_ನಕಲು(ಗುರಿ, f, nappend);
    ಗುರಿ += nappend;
  }
  *ಗುರಿ = '\0'; /* null-terminate */

  ಹಿಂತಿರುಗಿ encoded;
}

/*
 * Curl_urldecode() URL decodes the given string.
 *
 * Returns a pointer to a malloced string in *ostring with length given in
 * *olen. If length == 0, the length is assumed to be strlen(string).
 *
 * ctrl options:
 * - REJECT_NADA: accept everything
 * - REJECT_CTRL: rejects control characters (byte codes lower than 32) in
 *                the data
 * - REJECT_ZERO: rejects decoded zero bytes
 *
 * The values for the enum starts at 2, to make the assert detect legacy
 * invokes that used TRUE/FALSE (0 and 1).
 */

CURLcode Curl_urldecode(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string, size_t ಉದ್ದಳತೆ,
                        ಅಕ್ಷರ **ostring, size_t *olen,
                        ಎಣಿಕೆ urlreject ctrl)
{
  size_t alloc;
  ಅಕ್ಷರ *ns;
  uint8_t reject_ಎಲ್ಲೆ;

  DEBUGASSERT(string);
  DEBUGASSERT(ctrl >= REJECT_NADA); /* crash on TRUE/FALSE */

  alloc = (ಉದ್ದಳತೆ ? ಉದ್ದಳತೆ : ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(string));
  ns = curlx_malloc(alloc + 1);

  ಆದರೆ(!ns)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* store output string */
  *ostring = ns;

  reject_ಎಲ್ಲೆ = (ctrl == REJECT_CTRL) ? 0x20 :
    (ctrl == REJECT_ZERO) ? 1 : 0;

  ಆಗ(alloc) {
    uint8_t ಒಳ;

    ಆದರೆ(*string == '%') {
      ಆದರೆ(alloc > 2) {
        uint8_t h1 = hextable[(uint8_t)string[1]];
        uint8_t h2 = hextable[(uint8_t)string[2]];
        ಆದರೆ(!((h1 | h2) & 0xf0)) {
          ಒಳ = (uint8_t)((h1 << 4) | h2);
          string += 3;
          alloc -= 3;
          ಆದರೆ(ಒಳ < reject_ಎಲ್ಲೆ)
            ಹೋಗು error;
          *ns++ = (ಅಕ್ಷರ)ಒಳ;
          ಮುಂದುವರಿ;
        }
      }
      ಒಳ = '%';
      string++;
      alloc--;
      *ns++ = (ಅಕ್ಷರ)ಒಳ;
      ಮುಂದುವರಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = memchr(string + 1, '%', alloc - 1);
      size_t n = p ? (size_t)(p - string) : alloc;

      ಆದರೆ(reject_ಎಲ್ಲೆ) {
        ಆದರೆ(reject_ಎಲ್ಲೆ == 1) {
          ಆದರೆ(memchr(string, 0, n))
            ಹೋಗು error;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          size_t i;
          ಗಾಗಿ(i = 0; i < n; i++) {
            ಆದರೆ((uint8_t)string[i] < 0x20)
              ಹೋಗು error;
          }
        }
      }

      ಸ್ಮರಣೆ_ನಕಲು(ns, string, n);
      ns += n;
      string += n;
      alloc -= n;
    }
  }
  *ns = 0; /* terminate it */

  ಆದರೆ(olen)
    /* store output size */
    *olen = (size_t)(ns - *ostring);

  ಹಿಂತಿರುಗಿ CURLE_OK;
error:
  curlx_safefree(*ostring);
  ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
}

/*
 * Unescapes the given URL escaped string of given length. Returns a
 * pointer to a malloced string with length given in *olen.
 * If length == 0, the length is assumed to be strlen(string).
 * If olen == NULL, no output length is stored.
 * 'data' is ignored since 7.82.0.
 */
ಅಕ್ಷರ *curl_easy_unescape(CURL *curl, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string, ಪೂರ್ಣಾಂಕ inlength,
                         ಪೂರ್ಣಾಂಕ *outlength)
{
  ಅಕ್ಷರ *str = ಖಾಲಿ;
  (ಶೂನ್ಯ)curl;
  ಆದರೆ(string && (inlength >= 0)) {
    size_t inputlen = (size_t)inlength;
    size_t outputlen;
    CURLcode res = Curl_urldecode(string, inputlen, &str, &outputlen,
                                  REJECT_NADA);
    ಆದರೆ(res)
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ(outlength) {
      ಆದರೆ(outputlen <= (size_t)ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
        *outlength = curlx_uztosi(outputlen);
      ಇಲ್ಲದಿದ್ದರೆ
        /* too large to return in an int, fail! */
        curlx_safefree(str);
    }
  }
  ಹಿಂತಿರುಗಿ str;
}

/* For operating systems/environments that use different malloc/free
   systems for the app and for this library, we provide a free that uses
   the library's memory system */
ಶೂನ್ಯ curl_free(ಶೂನ್ಯ *p)
{
  curlx_free(p);
}

/*
 * Curl_hexencode()
 *
 * Converts binary input to lowercase hex-encoded ASCII output.
 * null-terminated.
 */
ಶೂನ್ಯ Curl_hexencode(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *src, size_t len, /* input length */
                    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಹೊರಹೋಗು, size_t olen) /* output buffer size */
{
  DEBUGASSERT(src && len && (olen >= 3));
  ಆದರೆ(src && len && (olen >= 3)) {
    ಆಗ(len-- && (olen >= 3)) {
      ಹೊರಹೋಗು[0] = Curl_ldigits[*src >> 4];
      ಹೊರಹೋಗು[1] = Curl_ldigits[*src & 0x0F];
      ++src;
      ಹೊರಹೋಗು += 2;
      olen -= 2;
    }
    *ಹೊರಹೋಗು = 0;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(olen)
    *ಹೊರಹೋಗು = 0;
}

/* Curl_hexbyte
 *
 * Output a single unsigned char as a two-digit UPPERCASE hex number.
 */
ಶೂನ್ಯ Curl_hexbyte(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *dest, /* must fit two bytes */
                  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ val)
{
  dest[0] = Curl_udigits[val >> 4];
  dest[1] = Curl_udigits[val & 0x0F];
}
