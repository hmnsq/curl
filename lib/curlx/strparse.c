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
#ضمن "curlx/strparse.h"

فراغ curlx_str_init(بنية Curl_str *out)
{
  out->str = عدم;
  out->len = 0;
}

فراغ curlx_str_assign(بنية Curl_str *out, ثابت حرف *str, size_t len)
{
  out->str = str;
  out->len = len;
}

/* remove bytes from the end of the string, never remove more bytes than what
   the string holds! */
فراغ curlx_str_trim(بنية Curl_str *out, size_t len)
{
  DEBUGASSERT(out);
  DEBUGASSERT(out->len >= len);
  out->len -= len;
}

/* Get a word until the first DELIM or end of string. At least one byte long.
   return non-zero on error. If 'max' is zero, it will always return error. */
صحيح curlx_str_until(ثابت حرف **linep, بنية Curl_str *out,
                    ثابت size_t max, حرف delim)
{
  ثابت حرف *s;
  size_t len = 0;
  DEBUGASSERT(linep);
  DEBUGASSERT(*linep);
  DEBUGASSERT(out);
  DEBUGASSERT(delim);
  s = *linep;

  curlx_str_init(out);
  طالما(*s && (*s != delim)) {
    s++;
    𐡄𐡍(++len > max) {
      أرجع STRE_BIG;
    }
  }
  𐡄𐡍(!len)
    أرجع STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  أرجع STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
صحيح curlx_str_word(ثابت حرف **linep, بنية Curl_str *out, ثابت size_t max)
{
  أرجع curlx_str_until(linep, out, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
صحيح curlx_str_untilnl(ثابت حرف **linep, بنية Curl_str *out,
                      ثابت size_t max)
{
  ثابت حرف *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  طالما(*s && !ISNEWLINE(*s)) {
    s++;
    𐡄𐡍(++len > max)
      أرجع STRE_BIG;
  }
  𐡄𐡍(!len)
    أرجع STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  أرجع STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
صحيح curlx_str_quotedword(ثابت حرف **linep, بنية Curl_str *out,
                         ثابت size_t max)
{
  ثابت حرف *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  𐡄𐡍(*s != '\"')
    أرجع STRE_BEGQUOTE;
  s++;
  طالما(*s && (*s != '\"')) {
    𐡄𐡍(*s == '\\' && s[1]) {
      s++;
      𐡄𐡍(++len > max)
        أرجع STRE_BIG;
    }
    s++;
    𐡄𐡍(++len > max)
      أرجع STRE_BIG;
  }
  𐡄𐡍(*s != '\"')
    أرجع STRE_ENDQUOTE;
  out->str = (*linep) + 1;
  out->len = len;
  *linep = s + 1;
  أرجع STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
صحيح curlx_str_single(ثابت حرف **linep, حرف byte)
{
  DEBUGASSERT(linep && *linep);
  𐡄𐡍(**linep != byte)
    أرجع STRE_BYTE;
  (*linep)++; /* move over it */
  أرجع STRE_OK;
}

/* Advance over a single space.
   return non-zero on error */
صحيح curlx_str_singlespace(ثابت حرف **linep)
{
  أرجع curlx_str_single(linep, ' ');
}

/* given an ASCII character and max ascii, return TRUE if valid */
#define valid_digit(x, m) \
  (((x) >= '0') && ((x) <= (m)) && curlx_hexasciitable[(x) - '0'])

/* We use 16 for the zero index (and the necessary bitwise AND in the loop)
   to be able to have a non-zero value there to make valid_digit() able to
   use the info */
ثابت unsigned حرف curlx_hexasciitable[] = {
  16, 1, 2, 3, 4, 5, 6, 7, 8, 9, /* 0x30: 0 - 9 */
  0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15,        /* 0x41: A - F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15         /* 0x61: a - f */
};

/* no support for 0x prefix nor leading spaces */
ساكن صحيح str_num_base(ثابت حرف **linep, curl_off_t *nump, curl_off_t max,
                        صحيح base) /* 8, 10 or 16, nothing else */
{
  curl_off_t num = 0;
  ثابت حرف *p;
  صحيح m = (base == 10) ? '9' :   /* the largest digit possible */
    (base == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((base == 8) || (base == 10) || (base == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  𐡄𐡍(!valid_digit(*p, m))
    أرجع STRE_NO_NUM;
  𐡄𐡍(max < base) {
    /* special-case low max scenario because check needs to be different */
    𐡏𐡁𐡃 {
      صحيح n = curlx_hexval(*p++);
      num = (num * base) + n;
      𐡄𐡍(num > max)
        أرجع STRE_OVERFLOW;
    } طالما(valid_digit(*p, m));
  }
  وإلا {
    curl_off_t cutoff = (max - (base - 1)) / base;
    طالما(valid_digit(*p, m)) {
      𐡄𐡍(num <= cutoff)
        num = (num * base) + curlx_hexval(*p++);
      وإلا {
        صحيح n = curlx_hexval(*p++);
        𐡄𐡍(num > ((max - n) / base))
          أرجع STRE_OVERFLOW;
        num = (num * base) + n;
      }
    }
  }
  *nump = num;
  *linep = p;
  أرجع STRE_OK;
}

/* Get an unsigned decimal number with no leading space or minus. Leading
   zeroes are accepted. return non-zero on error */
صحيح curlx_str_number(ثابت حرف **linep, curl_off_t *nump, curl_off_t max)
{
  أرجع str_num_base(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
صحيح curlx_str_hex(ثابت حرف **linep, curl_off_t *nump, curl_off_t max)
{
  أرجع str_num_base(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
صحيح curlx_str_octal(ثابت حرف **linep, curl_off_t *nump, curl_off_t max)
{
  أرجع str_num_base(linep, nump, max, 8);
}

/*
 * Parse a positive number up to 63-bit number written in ASCII. Skip leading
 * blanks. No support for prefixes.
 */
صحيح curlx_str_numblanks(ثابت حرف **str, curl_off_t *num)
{
  curlx_str_passblanks(str);
  أرجع curlx_str_number(str, num, CURL_OFF_T_MAX);
}

/* CR or LF
   return non-zero on error */
صحيح curlx_str_newline(ثابت حرف **linep)
{
  DEBUGASSERT(linep && *linep);
  𐡄𐡍(ISNEWLINE(**linep)) {
    (*linep)++;
    أرجع STRE_OK; /* yessir */
  }
  أرجع STRE_NEWLINE;
}

#ifndef WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
صحيح curlx_str_casecompare(بنية Curl_str *str, ثابت حرف *check)
{
  size_t clen = check ? strlen(check) : 0;
  أرجع ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#endif

/* case sensitive string compare. Returns non-zero on match. */
صحيح curlx_str_cmp(بنية Curl_str *str, ثابت حرف *check)
{
  𐡄𐡍(check) {
    size_t clen = strlen(check);
    أرجع ((str->len == clen) && !strncmp(str->str, check, clen));
  }
  أرجع !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
صحيح curlx_str_nudge(بنية Curl_str *str, size_t num)
{
  𐡄𐡍(num <= str->len) {
    str->str += num;
    str->len -= num;
    أرجع STRE_OK;
  }
  أرجع STRE_OVERFLOW;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
صحيح curlx_str_cspn(ثابت حرف **linep, بنية Curl_str *out,
                   ثابت حرف *reject)
{
  ثابت حرف *s = *linep;
  size_t len;
  DEBUGASSERT(linep && *linep);

  len = strcspn(s, reject);
  𐡄𐡍(len) {
    out->str = s;
    out->len = len;
    *linep = &s[len];
    أرجع STRE_OK;
  }
  curlx_str_init(out);
  أرجع STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
فراغ curlx_str_trimblanks(بنية Curl_str *out)
{
  طالما(out->len && ISBLANK(*out->str))
    curlx_str_nudge(out, 1);

  /* trim trailing spaces and tabs */
  طالما(out->len && ISBLANK(out->str[out->len - 1]))
    out->len--;
}

/* increase the pointer until it has moved over all blanks */
فراغ curlx_str_passblanks(ثابت حرف **linep)
{
  طالما(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
