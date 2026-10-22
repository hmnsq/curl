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
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

ಶೂನ್ಯ curlx_str_init(ಸಂರಚನಾ Curl_str *out)
{
  out->str = ರಿಕ್ತ;
  out->len = 0;
}

ಶೂನ್ಯ curlx_str_assign(ಸಂರಚನಾ Curl_str *out, ಧ್ರುವ ವರ್ಣ *str, size_t len)
{
  out->str = str;
  out->len = len;
}

/* remove bytes from the end of the string, never remove more bytes than what
   the string holds! */
ಶೂನ್ಯ curlx_str_trim(ಸಂರಚನಾ Curl_str *out, size_t len)
{
  DEBUGASSERT(out);
  DEBUGASSERT(out->len >= len);
  out->len -= len;
}

/* Get a word until the first DELIM or end of string. At least one byte long.
   return non-zero on error. If 'max' is zero, it will always return error. */
ಪೂರ್ಣಾಙ್ಕ curlx_str_until(ಧ್ರುವ ವರ್ಣ **linep, ಸಂರಚನಾ Curl_str *out,
                    ಧ್ರುವ size_t max, ವರ್ಣ delim)
{
  ಧ್ರುವ ವರ್ಣ *s;
  size_t len = 0;
  DEBUGASSERT(linep);
  DEBUGASSERT(*linep);
  DEBUGASSERT(out);
  DEBUGASSERT(delim);
  s = *linep;

  curlx_str_init(out);
  ಯಾವತ್(*s && (*s != delim)) {
    s++;
    ಯದಿ(++len > max) {
      ಪ್ರತ್ಯಾಗಮ STRE_BIG;
    }
  }
  ಯದಿ(!len)
    ಪ್ರತ್ಯಾಗಮ STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  ಪ್ರತ್ಯಾಗಮ STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
ಪೂರ್ಣಾಙ್ಕ curlx_str_word(ಧ್ರುವ ವರ್ಣ **linep, ಸಂರಚನಾ Curl_str *out, ಧ್ರುವ size_t max)
{
  ಪ್ರತ್ಯಾಗಮ curlx_str_until(linep, out, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
ಪೂರ್ಣಾಙ್ಕ curlx_str_untilnl(ಧ್ರುವ ವರ್ಣ **linep, ಸಂರಚನಾ Curl_str *out,
                      ಧ್ರುವ size_t max)
{
  ಧ್ರುವ ವರ್ಣ *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  ಯಾವತ್(*s && !ISNEWLINE(*s)) {
    s++;
    ಯದಿ(++len > max)
      ಪ್ರತ್ಯಾಗಮ STRE_BIG;
  }
  ಯದಿ(!len)
    ಪ್ರತ್ಯಾಗಮ STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  ಪ್ರತ್ಯಾಗಮ STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
ಪೂರ್ಣಾಙ್ಕ curlx_str_quotedword(ಧ್ರುವ ವರ್ಣ **linep, ಸಂರಚನಾ Curl_str *out,
                         ಧ್ರುವ size_t max)
{
  ಧ್ರುವ ವರ್ಣ *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  ಯದಿ(*s != '\"')
    ಪ್ರತ್ಯಾಗಮ STRE_BEGQUOTE;
  s++;
  ಯಾವತ್(*s && (*s != '\"')) {
    ಯದಿ(*s == '\\' && s[1]) {
      s++;
      ಯದಿ(++len > max)
        ಪ್ರತ್ಯಾಗಮ STRE_BIG;
    }
    s++;
    ಯದಿ(++len > max)
      ಪ್ರತ್ಯಾಗಮ STRE_BIG;
  }
  ಯದಿ(*s != '\"')
    ಪ್ರತ್ಯಾಗಮ STRE_ENDQUOTE;
  out->str = (*linep) + 1;
  out->len = len;
  *linep = s + 1;
  ಪ್ರತ್ಯಾಗಮ STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
ಪೂರ್ಣಾಙ್ಕ curlx_str_single(ಧ್ರುವ ವರ್ಣ **linep, ವರ್ಣ byte)
{
  DEBUGASSERT(linep && *linep);
  ಯದಿ(**linep != byte)
    ಪ್ರತ್ಯಾಗಮ STRE_BYTE;
  (*linep)++; /* move over it */
  ಪ್ರತ್ಯಾಗಮ STRE_OK;
}

/* Advance over a single space.
   return non-zero on error */
ಪೂರ್ಣಾಙ್ಕ curlx_str_singlespace(ಧ್ರುವ ವರ್ಣ **linep)
{
  ಪ್ರತ್ಯಾಗಮ curlx_str_single(linep, ' ');
}

/* given an ASCII character and max ascii, return TRUE if valid */
#define valid_digit(x, m) \
  (((x) >= '0') && ((x) <= (m)) && curlx_hexasciitable[(x) - '0'])

/* We use 16 for the zero index (and the necessary bitwise AND in the loop)
   to be able to have a non-zero value there to make valid_digit() able to
   use the info */
ಧ್ರುವ unsigned ವರ್ಣ curlx_hexasciitable[] = {
  16, 1, 2, 3, 4, 5, 6, 7, 8, 9, /* 0x30: 0 - 9 */
  0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15,        /* 0x41: A - F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15         /* 0x61: a - f */
};

/* no support for 0x prefix nor leading spaces */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ str_ಅಂಕೆ_base(ಧ್ರುವ ವರ್ಣ **linep, curl_off_t *nump, curl_off_t max,
                        ಪೂರ್ಣಾಙ್ಕ base) /* 8, 10 or 16, nothing else */
{
  curl_off_t ಅಂಕೆ = 0;
  ಧ್ರುವ ವರ್ಣ *p;
  ಪೂರ್ಣಾಙ್ಕ m = (base == 10) ? '9' :   /* the largest digit possible */
    (base == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((base == 8) || (base == 10) || (base == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  ಯದಿ(!valid_digit(*p, m))
    ಪ್ರತ್ಯಾಗಮ STRE_NO_ಅಂಕೆ;
  ಯದಿ(max < base) {
    /* special-case low max scenario because check needs to be different */
    ಕುರು {
      ಪೂರ್ಣಾಙ್ಕ n = curlx_hexval(*p++);
      ಅಂಕೆ = (ಅಂಕೆ * base) + n;
      ಯದಿ(ಅಂಕೆ > max)
        ಪ್ರತ್ಯಾಗಮ STRE_OVERFLOW;
    } ಯಾವತ್(valid_digit(*p, m));
  }
  ಅನ್ಯಥಾ {
    curl_off_t cutoff = (max - (base - 1)) / base;
    ಯಾವತ್(valid_digit(*p, m)) {
      ಯದಿ(ಅಂಕೆ <= cutoff)
        ಅಂಕೆ = (ಅಂಕೆ * base) + curlx_hexval(*p++);
      ಅನ್ಯಥಾ {
        ಪೂರ್ಣಾಙ್ಕ n = curlx_hexval(*p++);
        ಯದಿ(ಅಂಕೆ > ((max - n) / base))
          ಪ್ರತ್ಯಾಗಮ STRE_OVERFLOW;
        ಅಂಕೆ = (ಅಂಕೆ * base) + n;
      }
    }
  }
  *nump = ಅಂಕೆ;
  *linep = p;
  ಪ್ರತ್ಯಾಗಮ STRE_OK;
}

/* Get an unsigned decimal number with no leading space or minus. Leading
   zeroes are accepted. return non-zero on error */
ಪೂರ್ಣಾಙ್ಕ curlx_str_ಸಂಖ್ಯೆ(ಧ್ರುವ ವರ್ಣ **linep, curl_off_t *nump, curl_off_t max)
{
  ಪ್ರತ್ಯಾಗಮ str_ಅಂಕೆ_base(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
ಪೂರ್ಣಾಙ್ಕ curlx_str_hex(ಧ್ರುವ ವರ್ಣ **linep, curl_off_t *nump, curl_off_t max)
{
  ಪ್ರತ್ಯಾಗಮ str_ಅಂಕೆ_base(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
ಪೂರ್ಣಾಙ್ಕ curlx_str_octal(ಧ್ರುವ ವರ್ಣ **linep, curl_off_t *nump, curl_off_t max)
{
  ಪ್ರತ್ಯಾಗಮ str_ಅಂಕೆ_base(linep, nump, max, 8);
}

/*
 * Parse a positive number up to 63-bit number written in ASCII. Skip leading
 * blanks. No support for prefixes.
 */
ಪೂರ್ಣಾಙ್ಕ curlx_str_numblanks(ಧ್ರುವ ವರ್ಣ **str, curl_off_t *ಅಂಕೆ)
{
  curlx_str_passblanks(str);
  ಪ್ರತ್ಯಾಗಮ curlx_str_ಸಂಖ್ಯೆ(str, ಅಂಕೆ, CURL_OFF_T_MAX);
}

/* CR or LF
   return non-zero on error */
ಪೂರ್ಣಾಙ್ಕ curlx_str_newline(ಧ್ರುವ ವರ್ಣ **linep)
{
  DEBUGASSERT(linep && *linep);
  ಯದಿ(ISNEWLINE(**linep)) {
    (*linep)++;
    ಪ್ರತ್ಯಾಗಮ STRE_OK; /* yessir */
  }
  ಪ್ರತ್ಯಾಗಮ STRE_NEWLINE;
}

#ifndef WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
ಪೂರ್ಣಾಙ್ಕ curlx_str_casecompare(ಸಂರಚನಾ Curl_str *str, ಧ್ರುವ ವರ್ಣ *check)
{
  size_t clen = check ? strlen(check) : 0;
  ಪ್ರತ್ಯಾಗಮ ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#endif

/* case sensitive string compare. Returns non-zero on match. */
ಪೂರ್ಣಾಙ್ಕ curlx_str_cmp(ಸಂರಚನಾ Curl_str *str, ಧ್ರುವ ವರ್ಣ *check)
{
  ಯದಿ(check) {
    size_t clen = strlen(check);
    ಪ್ರತ್ಯಾಗಮ ((str->len == clen) && !strncmp(str->str, check, clen));
  }
  ಪ್ರತ್ಯಾಗಮ !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
ಪೂರ್ಣಾಙ್ಕ curlx_str_nudge(ಸಂರಚನಾ Curl_str *str, size_t ಅಂಕೆ)
{
  ಯದಿ(ಅಂಕೆ <= str->len) {
    str->str += ಅಂಕೆ;
    str->len -= ಅಂಕೆ;
    ಪ್ರತ್ಯಾಗಮ STRE_OK;
  }
  ಪ್ರತ್ಯಾಗಮ STRE_OVERFLOW;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
ಪೂರ್ಣಾಙ್ಕ curlx_str_cspn(ಧ್ರುವ ವರ್ಣ **linep, ಸಂರಚನಾ Curl_str *out,
                   ಧ್ರುವ ವರ್ಣ *reject)
{
  ಧ್ರುವ ವರ್ಣ *s = *linep;
  size_t len;
  DEBUGASSERT(linep && *linep);

  len = strcspn(s, reject);
  ಯದಿ(len) {
    out->str = s;
    out->len = len;
    *linep = &s[len];
    ಪ್ರತ್ಯಾಗಮ STRE_OK;
  }
  curlx_str_init(out);
  ಪ್ರತ್ಯಾಗಮ STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
ಶೂನ್ಯ curlx_str_trimblanks(ಸಂರಚನಾ Curl_str *out)
{
  ಯಾವತ್(out->len && ISBLANK(*out->str))
    curlx_str_nudge(out, 1);

  /* trim trailing spaces and tabs */
  ಯಾವತ್(out->len && ISBLANK(out->str[out->len - 1]))
    out->len--;
}

/* increase the pointer until it has moved over all blanks */
ಶೂನ್ಯ curlx_str_passblanks(ಧ್ರುವ ವರ್ಣ **linep)
{
  ಯಾವತ್(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
