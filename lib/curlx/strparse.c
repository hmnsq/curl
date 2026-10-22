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

ಶೂನ್ಯ curlx_str_init(ರಚನೆ Curl_str *ಹೊರಹೋಗು)
{
  ಹೊರಹೋಗು->str = ಖಾಲಿ;
  ಹೊರಹೋಗು->len = 0;
}

ಶೂನ್ಯ curlx_str_ನಿಯೋಜಿಸು(ರಚನೆ Curl_str *ಹೊರಹೋಗು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, size_t len)
{
  ಹೊರಹೋಗು->str = str;
  ಹೊರಹೋಗು->len = len;
}

/* remove bytes from the end of the string, never remove more bytes than what
   the string holds! */
ಶೂನ್ಯ curlx_str_trim(ರಚನೆ Curl_str *ಹೊರಹೋಗು, size_t len)
{
  DEBUGASSERT(ಹೊರಹೋಗು);
  DEBUGASSERT(ಹೊರಹೋಗು->len >= len);
  ಹೊರಹೋಗು->len -= len;
}

/* Get a word until the first DELIM or end of string. At least one byte long.
   return non-zero on error. If 'max' is zero, it will always return error. */
ಪೂರ್ಣಾಂಕ curlx_str_until(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep, ರಚನೆ Curl_str *ಹೊರಹೋಗು,
                    ಸ್ಥಿರಾಂಕ size_t max, ಅಕ್ಷರ delim)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s;
  size_t len = 0;
  DEBUGASSERT(linep);
  DEBUGASSERT(*linep);
  DEBUGASSERT(ಹೊರಹೋಗು);
  DEBUGASSERT(delim);
  s = *linep;

  curlx_str_init(ಹೊರಹೋಗು);
  ಆಗ(*s && (*s != delim)) {
    s++;
    ಆದರೆ(++len > max) {
      ಹಿಂತಿರುಗಿ STRE_BIG;
    }
  }
  ಆದರೆ(!len)
    ಹಿಂತಿರುಗಿ STRE_SHORT;
  ಹೊರಹೋಗು->str = *linep;
  ಹೊರಹೋಗು->len = len;
  *linep = s; /* point to the first byte after the word */
  ಹಿಂತಿರುಗಿ STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
ಪೂರ್ಣಾಂಕ curlx_str_ಶಬ್ದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep, ರಚನೆ Curl_str *ಹೊರಹೋಗು, ಸ್ಥಿರಾಂಕ size_t max)
{
  ಹಿಂತಿರುಗಿ curlx_str_until(linep, ಹೊರಹೋಗು, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
ಪೂರ್ಣಾಂಕ curlx_str_untilnl(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep, ರಚನೆ Curl_str *ಹೊರಹೋಗು,
                      ಸ್ಥಿರಾಂಕ size_t max)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && ಹೊರಹೋಗು && max);

  curlx_str_init(ಹೊರಹೋಗು);
  ಆಗ(*s && !ISNEWLINE(*s)) {
    s++;
    ಆದರೆ(++len > max)
      ಹಿಂತಿರುಗಿ STRE_BIG;
  }
  ಆದರೆ(!len)
    ಹಿಂತಿರುಗಿ STRE_SHORT;
  ಹೊರಹೋಗು->str = *linep;
  ಹೊರಹೋಗು->len = len;
  *linep = s; /* point to the first byte after the word */
  ಹಿಂತಿರುಗಿ STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
ಪೂರ್ಣಾಂಕ curlx_str_quotedword(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep, ರಚನೆ Curl_str *ಹೊರಹೋಗು,
                         ಸ್ಥಿರಾಂಕ size_t max)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && ಹೊರಹೋಗು && max);

  curlx_str_init(ಹೊರಹೋಗು);
  ಆದರೆ(*s != '\"')
    ಹಿಂತಿರುಗಿ STRE_BEGQUOTE;
  s++;
  ಆಗ(*s && (*s != '\"')) {
    ಆದರೆ(*s == '\\' && s[1]) {
      s++;
      ಆದರೆ(++len > max)
        ಹಿಂತಿರುಗಿ STRE_BIG;
    }
    s++;
    ಆದರೆ(++len > max)
      ಹಿಂತಿರುಗಿ STRE_BIG;
  }
  ಆದರೆ(*s != '\"')
    ಹಿಂತಿರುಗಿ STRE_ENDQUOTE;
  ಹೊರಹೋಗು->str = (*linep) + 1;
  ಹೊರಹೋಗು->len = len;
  *linep = s + 1;
  ಹಿಂತಿರುಗಿ STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
ಪೂರ್ಣಾಂಕ curlx_str_single(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep, ಅಕ್ಷರ byte)
{
  DEBUGASSERT(linep && *linep);
  ಆದರೆ(**linep != byte)
    ಹಿಂತಿರುಗಿ STRE_BYTE;
  (*linep)++; /* move over it */
  ಹಿಂತಿರುಗಿ STRE_OK;
}

/* Advance over a single space.
   return non-zero on error */
ಪೂರ್ಣಾಂಕ curlx_str_singlespace(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep)
{
  ಹಿಂತಿರುಗಿ curlx_str_single(linep, ' ');
}

/* given an ASCII character and max ascii, return TRUE if valid */
#ವ್ಯಾಖ್ಯಾನಿಸು valid_digit(x, m) \
  (((x) >= '0') && ((x) <= (m)) && curlx_hexasciitable[(x) - '0'])

/* We use 16 for the zero index (and the necessary bitwise AND in the loop)
   to be able to have a non-zero value there to make valid_digit() able to
   use the info */
ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ curlx_hexasciitable[] = {
  16, 1, 2, 3, 4, 5, 6, 7, 8, 9, /* 0x30: 0 - 9 */
  0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15,        /* 0x41: A - F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15         /* 0x61: a - f */
};

/* no support for 0x prefix nor leading spaces */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ str_ಅಂಕೆ_ತಳ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep, curl_off_t *nump, curl_off_t max,
                        ಪೂರ್ಣಾಂಕ ತಳ) /* 8, 10 or 16, nothing else */
{
  curl_off_t ಅಂಕೆ = 0;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
  ಪೂರ್ಣಾಂಕ m = (ತಳ == 10) ? '9' :   /* the largest digit possible */
    (ತಳ == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((ತಳ == 8) || (ತಳ == 10) || (ತಳ == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  ಆದರೆ(!valid_digit(*p, m))
    ಹಿಂತಿರುಗಿ STRE_NO_ಅಂಕೆ;
  ಆದರೆ(max < ತಳ) {
    /* special-case low max scenario because check needs to be different */
    ಮಾಡು {
      ಪೂರ್ಣಾಂಕ n = curlx_hexval(*p++);
      ಅಂಕೆ = (ಅಂಕೆ * ತಳ) + n;
      ಆದರೆ(ಅಂಕೆ > max)
        ಹಿಂತಿರುಗಿ STRE_ಅತಿಪ್ರವಾಹ;
    } ಆಗ(valid_digit(*p, m));
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    curl_off_t cutoff = (max - (ತಳ - 1)) / ತಳ;
    ಆಗ(valid_digit(*p, m)) {
      ಆದರೆ(ಅಂಕೆ <= cutoff)
        ಅಂಕೆ = (ಅಂಕೆ * ತಳ) + curlx_hexval(*p++);
      ಇಲ್ಲದಿದ್ದರೆ {
        ಪೂರ್ಣಾಂಕ n = curlx_hexval(*p++);
        ಆದರೆ(ಅಂಕೆ > ((max - n) / ತಳ))
          ಹಿಂತಿರುಗಿ STRE_ಅತಿಪ್ರವಾಹ;
        ಅಂಕೆ = (ಅಂಕೆ * ತಳ) + n;
      }
    }
  }
  *nump = ಅಂಕೆ;
  *linep = p;
  ಹಿಂತಿರುಗಿ STRE_OK;
}

/* Get an unsigned decimal number with no leading space or minus. Leading
   zeroes are accepted. return non-zero on error */
ಪೂರ್ಣಾಂಕ curlx_str_ಸಂಖ್ಯೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep, curl_off_t *nump, curl_off_t max)
{
  ಹಿಂತಿರುಗಿ str_ಅಂಕೆ_ತಳ(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
ಪೂರ್ಣಾಂಕ curlx_str_hex(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep, curl_off_t *nump, curl_off_t max)
{
  ಹಿಂತಿರುಗಿ str_ಅಂಕೆ_ತಳ(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
ಪೂರ್ಣಾಂಕ curlx_str_octal(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep, curl_off_t *nump, curl_off_t max)
{
  ಹಿಂತಿರುಗಿ str_ಅಂಕೆ_ತಳ(linep, nump, max, 8);
}

/*
 * Parse a positive number up to 63-bit number written in ASCII. Skip leading
 * blanks. No support for prefixes.
 */
ಪೂರ್ಣಾಂಕ curlx_str_numblanks(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **str, curl_off_t *ಅಂಕೆ)
{
  curlx_str_passblanks(str);
  ಹಿಂತಿರುಗಿ curlx_str_ಸಂಖ್ಯೆ(str, ಅಂಕೆ, CURL_OFF_T_MAX);
}

/* CR or LF
   return non-zero on error */
ಪೂರ್ಣಾಂಕ curlx_str_newline(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep)
{
  DEBUGASSERT(linep && *linep);
  ಆದರೆ(ISNEWLINE(**linep)) {
    (*linep)++;
    ಹಿಂತಿರುಗಿ STRE_OK; /* yessir */
  }
  ಹಿಂತಿರುಗಿ STRE_NEWLINE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
ಪೂರ್ಣಾಂಕ curlx_str_casecompare(ರಚನೆ Curl_str *str, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *check)
{
  size_t clen = check ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(check) : 0;
  ಹಿಂತಿರುಗಿ ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#ಆದರೆ_ಅಂತ್ಯ

/* case sensitive string compare. Returns non-zero on match. */
ಪೂರ್ಣಾಂಕ curlx_str_cmp(ರಚನೆ Curl_str *str, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *check)
{
  ಆದರೆ(check) {
    size_t clen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(check);
    ಹಿಂತಿರುಗಿ ((str->len == clen) && !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(str->str, check, clen));
  }
  ಹಿಂತಿರುಗಿ !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
ಪೂರ್ಣಾಂಕ curlx_str_nudge(ರಚನೆ Curl_str *str, size_t ಅಂಕೆ)
{
  ಆದರೆ(ಅಂಕೆ <= str->len) {
    str->str += ಅಂಕೆ;
    str->len -= ಅಂಕೆ;
    ಹಿಂತಿರುಗಿ STRE_OK;
  }
  ಹಿಂತಿರುಗಿ STRE_ಅತಿಪ್ರವಾಹ;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
ಪೂರ್ಣಾಂಕ curlx_str_cspn(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep, ರಚನೆ Curl_str *ಹೊರಹೋಗು,
                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *reject)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = *linep;
  size_t len;
  DEBUGASSERT(linep && *linep);

  len = strcspn(s, reject);
  ಆದರೆ(len) {
    ಹೊರಹೋಗು->str = s;
    ಹೊರಹೋಗು->len = len;
    *linep = &s[len];
    ಹಿಂತಿರುಗಿ STRE_OK;
  }
  curlx_str_init(ಹೊರಹೋಗು);
  ಹಿಂತಿರುಗಿ STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
ಶೂನ್ಯ curlx_str_trimblanks(ರಚನೆ Curl_str *ಹೊರಹೋಗು)
{
  ಆಗ(ಹೊರಹೋಗು->len && ISBLANK(*ಹೊರಹೋಗು->str))
    curlx_str_nudge(ಹೊರಹೋಗು, 1);

  /* trim trailing spaces and tabs */
  ಆಗ(ಹೊರಹೋಗು->len && ISBLANK(ಹೊರಹೋಗು->str[ಹೊರಹೋಗು->len - 1]))
    ಹೊರಹೋಗು->len--;
}

/* increase the pointer until it has moved over all blanks */
ಶೂನ್ಯ curlx_str_passblanks(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **linep)
{
  ಆಗ(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
