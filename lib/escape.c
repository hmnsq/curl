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
#شامل "curl_setup.h"

ساخت Curl_easy;

#شامل "urldata.h"
#شامل "escape.h"
#شامل "curlx/strparse.h"
#شامل "curl_printf.h"

/* for ABI-compatibility with previous versions */
حرف *curl_escape(ثابت حرف *string, صحیح لمبائی)
{
  واپس curl_easy_escape(تہی, string, لمبائی);
}

/* for ABI-compatibility with previous versions */
حرف *curl_unescape(ثابت حرف *string, صحیح لمبائی)
{
  واپس curl_easy_unescape(تہی, string, لمبائی, تہی);
}

#تعریف_کرو NOPE 0xff
#تعریف_کرو _OK_ 0x1f /* octet not needing encoding but not a hexadecimal
                     character */

ساکن ثابت غیرعلامتی حرف hextable[256] = {
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

ساکن ثابت حرف hexpairs[][2] = {
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
حرف *curl_easy_escape(CURL *curl, ثابت حرف *string, صحیح لمبائی)
{
  size_t len;
  ثابت حرف *f = string;
  size_t nappend = 0;
  حرف *ہدف;
  حرف *encoded;
  (خالی)curl;

  اگر(!string || (لمبائی < 0))
    واپس تہی;

  len = (لمبائی ? (size_t)لمبائی : لڑی_لمبائی(string));
  اگر(!len)
    واپس curlx_strdup("");

  اگر(len > SIZE_MAX / 16)
    واپس تہی;

  encoded = ہدف = curlx_malloc((len * 3) + 1);
  اگر(!ہدف)
    واپس تہی;

  جب(len--) {
    uint8_t اندر = (uint8_t)*string++;

    اگر(!(hextable[اندر] & 0x80))
      /* append this */
      nappend++;
    ورنہ {
      اگر(nappend) {
        حافظہ_نقل(ہدف, f, nappend);
        ہدف += nappend;
        nappend = 0;
      }
      f = string;
      /* encode it */
      ہدف[0] = '%';
      حافظہ_نقل(ہدف + 1, hexpairs[اندر], 2);
      ہدف += 3;
    }
  }
  اگر(nappend) {
    حافظہ_نقل(ہدف, f, nappend);
    ہدف += nappend;
  }
  *ہدف = '\0'; /* null-terminate */

  واپس encoded;
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

CURLcode Curl_urldecode(ثابت حرف *string, size_t لمبائی,
                        حرف **ostring, size_t *olen,
                        شمار urlreject ctrl)
{
  size_t alloc;
  حرف *ns;
  uint8_t reject_حد_بندی;

  DEBUGASSERT(string);
  DEBUGASSERT(ctrl >= REJECT_NADA); /* crash on TRUE/FALSE */

  alloc = (لمبائی ? لمبائی : لڑی_لمبائی(string));
  ns = curlx_malloc(alloc + 1);

  اگر(!ns)
    واپس CURLE_برآمد_کا_حافظہ;

  /* store output string */
  *ostring = ns;

  reject_حد_بندی = (ctrl == REJECT_CTRL) ? 0x20 :
    (ctrl == REJECT_ZERO) ? 1 : 0;

  جب(alloc) {
    uint8_t اندر;

    اگر(*string == '%') {
      اگر(alloc > 2) {
        uint8_t h1 = hextable[(uint8_t)string[1]];
        uint8_t h2 = hextable[(uint8_t)string[2]];
        اگر(!((h1 | h2) & 0xf0)) {
          اندر = (uint8_t)((h1 << 4) | h2);
          string += 3;
          alloc -= 3;
          اگر(اندر < reject_حد_بندی)
            جاؤ error;
          *ns++ = (حرف)اندر;
          جاری;
        }
      }
      اندر = '%';
      string++;
      alloc--;
      *ns++ = (حرف)اندر;
      جاری;
    }
    ورنہ {
      ثابت حرف *p = memchr(string + 1, '%', alloc - 1);
      size_t n = p ? (size_t)(p - string) : alloc;

      اگر(reject_حد_بندی) {
        اگر(reject_حد_بندی == 1) {
          اگر(memchr(string, 0, n))
            جاؤ error;
        }
        ورنہ {
          size_t i;
          برائے(i = 0; i < n; i++) {
            اگر((uint8_t)string[i] < 0x20)
              جاؤ error;
          }
        }
      }

      حافظہ_نقل(ns, string, n);
      ns += n;
      string += n;
      alloc -= n;
    }
  }
  *ns = 0; /* terminate it */

  اگر(olen)
    /* store output size */
    *olen = (size_t)(ns - *ostring);

  واپس CURLE_OK;
error:
  curlx_safefree(*ostring);
  واپس CURLE_URL_MALFORMAT;
}

/*
 * Unescapes the given URL escaped string of given length. Returns a
 * pointer to a malloced string with length given in *olen.
 * If length == 0, the length is assumed to be strlen(string).
 * If olen == NULL, no output length is stored.
 * 'data' is ignored since 7.82.0.
 */
حرف *curl_easy_unescape(CURL *curl, ثابت حرف *string, صحیح inlength,
                         صحیح *outlength)
{
  حرف *str = تہی;
  (خالی)curl;
  اگر(string && (inlength >= 0)) {
    size_t inputlen = (size_t)inlength;
    size_t outputlen;
    CURLcode res = Curl_urldecode(string, inputlen, &str, &outputlen,
                                  REJECT_NADA);
    اگر(res)
      واپس تہی;

    اگر(outlength) {
      اگر(outputlen <= (size_t)زیادہ_سے_زیادہ_عدد)
        *outlength = curlx_uztosi(outputlen);
      ورنہ
        /* too large to return in an int, fail! */
        curlx_safefree(str);
    }
  }
  واپس str;
}

/* For operating systems/environments that use different malloc/free
   systems for the app and for this library, we provide a free that uses
   the library's memory system */
خالی curl_free(خالی *p)
{
  curlx_free(p);
}

/*
 * Curl_hexencode()
 *
 * Converts binary input to lowercase hex-encoded ASCII output.
 * null-terminated.
 */
خالی Curl_hexencode(ثابت غیرعلامتی حرف *src, size_t len, /* input length */
                    غیرعلامتی حرف *برآمد, size_t olen) /* output buffer size */
{
  DEBUGASSERT(src && len && (olen >= 3));
  اگر(src && len && (olen >= 3)) {
    جب(len-- && (olen >= 3)) {
      برآمد[0] = Curl_ldigits[*src >> 4];
      برآمد[1] = Curl_ldigits[*src & 0x0F];
      ++src;
      برآمد += 2;
      olen -= 2;
    }
    *برآمد = 0;
  }
  ورنہ اگر(olen)
    *برآمد = 0;
}

/* Curl_hexbyte
 *
 * Output a single unsigned char as a two-digit UPPERCASE hex number.
 */
خالی Curl_hexbyte(غیرعلامتی حرف *dest, /* must fit two bytes */
                  غیرعلامتی حرف val)
{
  dest[0] = Curl_udigits[val >> 4];
  dest[1] = Curl_udigits[val & 0x0F];
}
