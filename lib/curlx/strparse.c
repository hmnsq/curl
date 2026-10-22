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
#include "curlx/strparse.h"

void curlx_str_init(sa_nracanaa Curl_str *out)
{
  out->str = NULL;
  out->len = 0;
}

void curlx_str_assign(sa_nracanaa Curl_str *out, const var_na *str, size_t len)
{
  out->str = str;
  out->len = len;
}

/* remove bytes from the end of the string, never remove more bytes than what
   the string holds! */
void curlx_str_trim(sa_nracanaa Curl_str *out, size_t len)
{
  DEBUGASSERT(out);
  DEBUGASSERT(out->len >= len);
  out->len -= len;
}

/* Get a word until the first DELIM or end of string. At least one byte long.
   return non-zero on error. If 'max' is zero, it will always return error. */
puur_naa_nka curlx_str_until(const var_na **linep, sa_nracanaa Curl_str *out,
                    const size_t max, var_na delim)
{
  const var_na *s;
  size_t len = 0;
  DEBUGASSERT(linep);
  DEBUGASSERT(*linep);
  DEBUGASSERT(out);
  DEBUGASSERT(delim);
  s = *linep;

  curlx_str_init(out);
  ᱡᱚᱠᱷᱚᱱ(*s && (*s != delim)) {
    s++;
    ᱡᱩᱫᱤ(++len > max) {
      ᱨᱩᱣᱟᱹᱲ STRE_BIG;
    }
  }
  ᱡᱩᱫᱤ(!len)
    ᱨᱩᱣᱟᱹᱲ STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  ᱨᱩᱣᱟᱹᱲ STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
puur_naa_nka curlx_str_word(const var_na **linep, sa_nracanaa Curl_str *out, const size_t max)
{
  ᱨᱩᱣᱟᱹᱲ curlx_str_until(linep, out, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
puur_naa_nka curlx_str_untilnl(const var_na **linep, sa_nracanaa Curl_str *out,
                      const size_t max)
{
  const var_na *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  ᱡᱚᱠᱷᱚᱱ(*s && !ISNEWLINE(*s)) {
    s++;
    ᱡᱩᱫᱤ(++len > max)
      ᱨᱩᱣᱟᱹᱲ STRE_BIG;
  }
  ᱡᱩᱫᱤ(!len)
    ᱨᱩᱣᱟᱹᱲ STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  ᱨᱩᱣᱟᱹᱲ STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
puur_naa_nka curlx_str_quotedword(const var_na **linep, sa_nracanaa Curl_str *out,
                         const size_t max)
{
  const var_na *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  ᱡᱩᱫᱤ(*s != '\"')
    ᱨᱩᱣᱟᱹᱲ STRE_BEGQUOTE;
  s++;
  ᱡᱚᱠᱷᱚᱱ(*s && (*s != '\"')) {
    ᱡᱩᱫᱤ(*s == '\\' && s[1]) {
      s++;
      ᱡᱩᱫᱤ(++len > max)
        ᱨᱩᱣᱟᱹᱲ STRE_BIG;
    }
    s++;
    ᱡᱩᱫᱤ(++len > max)
      ᱨᱩᱣᱟᱹᱲ STRE_BIG;
  }
  ᱡᱩᱫᱤ(*s != '\"')
    ᱨᱩᱣᱟᱹᱲ STRE_ENDQUOTE;
  out->str = (*linep) + 1;
  out->len = len;
  *linep = s + 1;
  ᱨᱩᱣᱟᱹᱲ STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
puur_naa_nka curlx_str_single(const var_na **linep, var_na byte)
{
  DEBUGASSERT(linep && *linep);
  ᱡᱩᱫᱤ(**linep != byte)
    ᱨᱩᱣᱟᱹᱲ STRE_BYTE;
  (*linep)++; /* move over it */
  ᱨᱩᱣᱟᱹᱲ STRE_OK;
}

/* Advance over a single space.
   return non-zero on error */
puur_naa_nka curlx_str_singlespace(const var_na **linep)
{
  ᱨᱩᱣᱟᱹᱲ curlx_str_single(linep, ' ');
}

/* given an ASCII character and max ascii, return TRUE if valid */
#define valid_digit(x, m) \
  (((x) >= '0') && ((x) <= (m)) && curlx_hexasciitable[(x) - '0'])

/* We use 16 for the zero index (and the necessary bitwise AND in the loop)
   to be able to have a non-zero value there to make valid_digit() able to
   use the info */
const unsigned var_na curlx_hexasciitable[] = {
  16, 1, 2, 3, 4, 5, 6, 7, 8, 9, /* 0x30: 0 - 9 */
  0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15,        /* 0x41: A - F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15         /* 0x61: a - f */
};

/* no support for 0x prefix nor leading spaces */
static puur_naa_nka str_num_base(const var_na **linep, curl_off_t *nump, curl_off_t max,
                        puur_naa_nka base) /* 8, 10 or 16, nothing else */
{
  curl_off_t num = 0;
  const var_na *p;
  puur_naa_nka m = (base == 10) ? '9' :   /* the largest digit possible */
    (base == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((base == 8) || (base == 10) || (base == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  ᱡᱩᱫᱤ(!valid_digit(*p, m))
    ᱨᱩᱣᱟᱹᱲ STRE_NO_NUM;
  ᱡᱩᱫᱤ(max < base) {
    /* special-case low max scenario because check needs to be different */
    do {
      puur_naa_nka n = curlx_hexval(*p++);
      num = (num * base) + n;
      ᱡᱩᱫᱤ(num > max)
        ᱨᱩᱣᱟᱹᱲ STRE_OVERFLOW;
    } ᱡᱚᱠᱷᱚᱱ(valid_digit(*p, m));
  }
  ᱵᱟᱝ {
    curl_off_t cutoff = (max - (base - 1)) / base;
    ᱡᱚᱠᱷᱚᱱ(valid_digit(*p, m)) {
      ᱡᱩᱫᱤ(num <= cutoff)
        num = (num * base) + curlx_hexval(*p++);
      ᱵᱟᱝ {
        puur_naa_nka n = curlx_hexval(*p++);
        ᱡᱩᱫᱤ(num > ((max - n) / base))
          ᱨᱩᱣᱟᱹᱲ STRE_OVERFLOW;
        num = (num * base) + n;
      }
    }
  }
  *nump = num;
  *linep = p;
  ᱨᱩᱣᱟᱹᱲ STRE_OK;
}

/* Get an unsigned decimal number with no leading space or minus. Leading
   zeroes are accepted. return non-zero on error */
puur_naa_nka curlx_str_number(const var_na **linep, curl_off_t *nump, curl_off_t max)
{
  ᱨᱩᱣᱟᱹᱲ str_num_base(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
puur_naa_nka curlx_str_hex(const var_na **linep, curl_off_t *nump, curl_off_t max)
{
  ᱨᱩᱣᱟᱹᱲ str_num_base(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
puur_naa_nka curlx_str_octal(const var_na **linep, curl_off_t *nump, curl_off_t max)
{
  ᱨᱩᱣᱟᱹᱲ str_num_base(linep, nump, max, 8);
}

/*
 * Parse a positive number up to 63-bit number written in ASCII. Skip leading
 * blanks. No support for prefixes.
 */
puur_naa_nka curlx_str_numblanks(const var_na **str, curl_off_t *num)
{
  curlx_str_passblanks(str);
  ᱨᱩᱣᱟᱹᱲ curlx_str_number(str, num, CURL_OFF_T_MAX);
}

/* CR or LF
   return non-zero on error */
puur_naa_nka curlx_str_newline(const var_na **linep)
{
  DEBUGASSERT(linep && *linep);
  ᱡᱩᱫᱤ(ISNEWLINE(**linep)) {
    (*linep)++;
    ᱨᱩᱣᱟᱹᱲ STRE_OK; /* yessir */
  }
  ᱨᱩᱣᱟᱹᱲ STRE_NEWLINE;
}

#ifndef WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
puur_naa_nka curlx_str_casecompare(sa_nracanaa Curl_str *str, const var_na *check)
{
  size_t clen = check ? strlen(check) : 0;
  ᱨᱩᱣᱟᱹᱲ ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#endif

/* case sensitive string compare. Returns non-zero on match. */
puur_naa_nka curlx_str_cmp(sa_nracanaa Curl_str *str, const var_na *check)
{
  ᱡᱩᱫᱤ(check) {
    size_t clen = strlen(check);
    ᱨᱩᱣᱟᱹᱲ ((str->len == clen) && !strncmp(str->str, check, clen));
  }
  ᱨᱩᱣᱟᱹᱲ !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
puur_naa_nka curlx_str_nudge(sa_nracanaa Curl_str *str, size_t num)
{
  ᱡᱩᱫᱤ(num <= str->len) {
    str->str += num;
    str->len -= num;
    ᱨᱩᱣᱟᱹᱲ STRE_OK;
  }
  ᱨᱩᱣᱟᱹᱲ STRE_OVERFLOW;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
puur_naa_nka curlx_str_cspn(const var_na **linep, sa_nracanaa Curl_str *out,
                   const var_na *reject)
{
  const var_na *s = *linep;
  size_t len;
  DEBUGASSERT(linep && *linep);

  len = strcspn(s, reject);
  ᱡᱩᱫᱤ(len) {
    out->str = s;
    out->len = len;
    *linep = &s[len];
    ᱨᱩᱣᱟᱹᱲ STRE_OK;
  }
  curlx_str_init(out);
  ᱨᱩᱣᱟᱹᱲ STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
void curlx_str_trimblanks(sa_nracanaa Curl_str *out)
{
  ᱡᱚᱠᱷᱚᱱ(out->len && ISBLANK(*out->str))
    curlx_str_nudge(out, 1);

  /* trim trailing spaces and tabs */
  ᱡᱚᱠᱷᱚᱱ(out->len && ISBLANK(out->str[out->len - 1]))
    out->len--;
}

/* increase the pointer until it has moved over all blanks */
void curlx_str_passblanks(const var_na **linep)
{
  ᱡᱚᱠᱷᱚᱱ(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
