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
#অন্তর্ভাব "curlx/strparse.h"

শূন্য curlx_str_init(সংরচনা Curl_str *out)
{
  out->str = রিক্ত;
  out->len = 0;
}

শূন্য curlx_str_assign(সংরচনা Curl_str *out, ধ্রুব বর্ণ *str, size_t len)
{
  out->str = str;
  out->len = len;
}

/* remove bytes from the end of the string, never remove more bytes than what
   the string holds! */
শূন্য curlx_str_trim(সংরচনা Curl_str *out, size_t len)
{
  DEBUGASSERT(out);
  DEBUGASSERT(out->len >= len);
  out->len -= len;
}

/* Get a word until the first DELIM or end of string. At least one byte long.
   return non-zero on error. If 'max' is zero, it will always return error. */
পূর্ণাঙ্ক curlx_str_until(ধ্রুব বর্ণ **linep, সংরচনা Curl_str *out,
                    ধ্রুব size_t max, বর্ণ delim)
{
  ধ্রুব বর্ণ *s;
  size_t len = 0;
  DEBUGASSERT(linep);
  DEBUGASSERT(*linep);
  DEBUGASSERT(out);
  DEBUGASSERT(delim);
  s = *linep;

  curlx_str_init(out);
  যাবত্(*s && (*s != delim)) {
    s++;
    যদি(++len > max) {
      প্রত্যাগম STRE_BIG;
    }
  }
  যদি(!len)
    প্রত্যাগম STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  প্রত্যাগম STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
পূর্ণাঙ্ক curlx_str_word(ধ্রুব বর্ণ **linep, সংরচনা Curl_str *out, ধ্রুব size_t max)
{
  প্রত্যাগম curlx_str_until(linep, out, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
পূর্ণাঙ্ক curlx_str_untilnl(ধ্রুব বর্ণ **linep, সংরচনা Curl_str *out,
                      ধ্রুব size_t max)
{
  ধ্রুব বর্ণ *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  যাবত্(*s && !ISNEWLINE(*s)) {
    s++;
    যদি(++len > max)
      প্রত্যাগম STRE_BIG;
  }
  যদি(!len)
    প্রত্যাগম STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  প্রত্যাগম STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
পূর্ণাঙ্ক curlx_str_quotedword(ধ্রুব বর্ণ **linep, সংরচনা Curl_str *out,
                         ধ্রুব size_t max)
{
  ধ্রুব বর্ণ *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  যদি(*s != '\"')
    প্রত্যাগম STRE_BEGQUOTE;
  s++;
  যাবত্(*s && (*s != '\"')) {
    যদি(*s == '\\' && s[1]) {
      s++;
      যদি(++len > max)
        প্রত্যাগম STRE_BIG;
    }
    s++;
    যদি(++len > max)
      প্রত্যাগম STRE_BIG;
  }
  যদি(*s != '\"')
    প্রত্যাগম STRE_ENDQUOTE;
  out->str = (*linep) + 1;
  out->len = len;
  *linep = s + 1;
  প্রত্যাগম STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
পূর্ণাঙ্ক curlx_str_single(ধ্রুব বর্ণ **linep, বর্ণ byte)
{
  DEBUGASSERT(linep && *linep);
  যদি(**linep != byte)
    প্রত্যাগম STRE_BYTE;
  (*linep)++; /* move over it */
  প্রত্যাগম STRE_OK;
}

/* Advance over a single space.
   return non-zero on error */
পূর্ণাঙ্ক curlx_str_singlespace(ধ্রুব বর্ণ **linep)
{
  প্রত্যাগম curlx_str_single(linep, ' ');
}

/* given an ASCII character and max ascii, return TRUE if valid */
#define valid_digit(x, m) \
  (((x) >= '0') && ((x) <= (m)) && curlx_hexasciitable[(x) - '0'])

/* We use 16 for the zero index (and the necessary bitwise AND in the loop)
   to be able to have a non-zero value there to make valid_digit() able to
   use the info */
ধ্রুব unsigned বর্ণ curlx_hexasciitable[] = {
  16, 1, 2, 3, 4, 5, 6, 7, 8, 9, /* 0x30: 0 - 9 */
  0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15,        /* 0x41: A - F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15         /* 0x61: a - f */
};

/* no support for 0x prefix nor leading spaces */
স্থির পূর্ণাঙ্ক str_অঙ্ক_base(ধ্রুব বর্ণ **linep, curl_off_t *nump, curl_off_t max,
                        পূর্ণাঙ্ক base) /* 8, 10 or 16, nothing else */
{
  curl_off_t অঙ্ক = 0;
  ধ্রুব বর্ণ *p;
  পূর্ণাঙ্ক m = (base == 10) ? '9' :   /* the largest digit possible */
    (base == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((base == 8) || (base == 10) || (base == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  যদি(!valid_digit(*p, m))
    প্রত্যাগম STRE_NO_অঙ্ক;
  যদি(max < base) {
    /* special-case low max scenario because check needs to be different */
    কুরু {
      পূর্ণাঙ্ক n = curlx_hexval(*p++);
      অঙ্ক = (অঙ্ক * base) + n;
      যদি(অঙ্ক > max)
        প্রত্যাগম STRE_OVERFLOW;
    } যাবত্(valid_digit(*p, m));
  }
  অন্যথা {
    curl_off_t cutoff = (max - (base - 1)) / base;
    যাবত্(valid_digit(*p, m)) {
      যদি(অঙ্ক <= cutoff)
        অঙ্ক = (অঙ্ক * base) + curlx_hexval(*p++);
      অন্যথা {
        পূর্ণাঙ্ক n = curlx_hexval(*p++);
        যদি(অঙ্ক > ((max - n) / base))
          প্রত্যাগম STRE_OVERFLOW;
        অঙ্ক = (অঙ্ক * base) + n;
      }
    }
  }
  *nump = অঙ্ক;
  *linep = p;
  প্রত্যাগম STRE_OK;
}

/* Get an unsigned decimal number with no leading space or minus. Leading
   zeroes are accepted. return non-zero on error */
পূর্ণাঙ্ক curlx_str_সংখ্যা(ধ্রুব বর্ণ **linep, curl_off_t *nump, curl_off_t max)
{
  প্রত্যাগম str_অঙ্ক_base(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
পূর্ণাঙ্ক curlx_str_hex(ধ্রুব বর্ণ **linep, curl_off_t *nump, curl_off_t max)
{
  প্রত্যাগম str_অঙ্ক_base(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
পূর্ণাঙ্ক curlx_str_octal(ধ্রুব বর্ণ **linep, curl_off_t *nump, curl_off_t max)
{
  প্রত্যাগম str_অঙ্ক_base(linep, nump, max, 8);
}

/*
 * Parse a positive number up to 63-bit number written in ASCII. Skip leading
 * blanks. No support for prefixes.
 */
পূর্ণাঙ্ক curlx_str_numblanks(ধ্রুব বর্ণ **str, curl_off_t *অঙ্ক)
{
  curlx_str_passblanks(str);
  প্রত্যাগম curlx_str_সংখ্যা(str, অঙ্ক, CURL_OFF_T_MAX);
}

/* CR or LF
   return non-zero on error */
পূর্ণাঙ্ক curlx_str_newline(ধ্রুব বর্ণ **linep)
{
  DEBUGASSERT(linep && *linep);
  যদি(ISNEWLINE(**linep)) {
    (*linep)++;
    প্রত্যাগম STRE_OK; /* yessir */
  }
  প্রত্যাগম STRE_NEWLINE;
}

#ifndef WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
পূর্ণাঙ্ক curlx_str_casecompare(সংরচনা Curl_str *str, ধ্রুব বর্ণ *check)
{
  size_t clen = check ? strlen(check) : 0;
  প্রত্যাগম ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#endif

/* case sensitive string compare. Returns non-zero on match. */
পূর্ণাঙ্ক curlx_str_cmp(সংরচনা Curl_str *str, ধ্রুব বর্ণ *check)
{
  যদি(check) {
    size_t clen = strlen(check);
    প্রত্যাগম ((str->len == clen) && !strncmp(str->str, check, clen));
  }
  প্রত্যাগম !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
পূর্ণাঙ্ক curlx_str_nudge(সংরচনা Curl_str *str, size_t অঙ্ক)
{
  যদি(অঙ্ক <= str->len) {
    str->str += অঙ্ক;
    str->len -= অঙ্ক;
    প্রত্যাগম STRE_OK;
  }
  প্রত্যাগম STRE_OVERFLOW;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
পূর্ণাঙ্ক curlx_str_cspn(ধ্রুব বর্ণ **linep, সংরচনা Curl_str *out,
                   ধ্রুব বর্ণ *reject)
{
  ধ্রুব বর্ণ *s = *linep;
  size_t len;
  DEBUGASSERT(linep && *linep);

  len = strcspn(s, reject);
  যদি(len) {
    out->str = s;
    out->len = len;
    *linep = &s[len];
    প্রত্যাগম STRE_OK;
  }
  curlx_str_init(out);
  প্রত্যাগম STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
শূন্য curlx_str_trimblanks(সংরচনা Curl_str *out)
{
  যাবত্(out->len && ISBLANK(*out->str))
    curlx_str_nudge(out, 1);

  /* trim trailing spaces and tabs */
  যাবত্(out->len && ISBLANK(out->str[out->len - 1]))
    out->len--;
}

/* increase the pointer until it has moved over all blanks */
শূন্য curlx_str_passblanks(ধ্রুব বর্ণ **linep)
{
  যাবত্(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
