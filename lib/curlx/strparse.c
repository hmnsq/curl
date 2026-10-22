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

void curlx_str_init(struct Curl_str *out)
{
  out->str = NULL;
  out->len = 0;
}

void curlx_str_assign(struct Curl_str *out, const char *str, size_t len)
{
  out->str = str;
  out->len = len;
}

/* remove bytes from the end of the string, never remove more bytes than what
   the string holds! */
void curlx_str_trim(struct Curl_str *out, size_t len)
{
  DEBUGASSERT(out);
  DEBUGASSERT(out->len >= len);
  out->len -= len;
}

/* Get a word until the first DELIM or end of string. At least one byte long.
   return non-zero on error. If 'max' is zero, it will always return error. */
정수 curlx_str_until(const char **linep, struct Curl_str *out,
                    const size_t max, char delim)
{
  const char *s;
  size_t len = 0;
  DEBUGASSERT(linep);
  DEBUGASSERT(*linep);
  DEBUGASSERT(out);
  DEBUGASSERT(delim);
  s = *linep;

  curlx_str_init(out);
  동안(*s && (*s != delim)) {
    s++;
    만약(++len > max) {
      반환 STRE_BIG;
    }
  }
  만약(!len)
    반환 STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  반환 STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
정수 curlx_str_word(const char **linep, struct Curl_str *out, const size_t max)
{
  반환 curlx_str_until(linep, out, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
정수 curlx_str_untilnl(const char **linep, struct Curl_str *out,
                      const size_t max)
{
  const char *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  동안(*s && !ISNEWLINE(*s)) {
    s++;
    만약(++len > max)
      반환 STRE_BIG;
  }
  만약(!len)
    반환 STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  반환 STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
정수 curlx_str_quotedword(const char **linep, struct Curl_str *out,
                         const size_t max)
{
  const char *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  만약(*s != '\"')
    반환 STRE_BEGQUOTE;
  s++;
  동안(*s && (*s != '\"')) {
    만약(*s == '\\' && s[1]) {
      s++;
      만약(++len > max)
        반환 STRE_BIG;
    }
    s++;
    만약(++len > max)
      반환 STRE_BIG;
  }
  만약(*s != '\"')
    반환 STRE_ENDQUOTE;
  out->str = (*linep) + 1;
  out->len = len;
  *linep = s + 1;
  반환 STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
정수 curlx_str_single(const char **linep, char byte)
{
  DEBUGASSERT(linep && *linep);
  만약(**linep != byte)
    반환 STRE_BYTE;
  (*linep)++; /* move over it */
  반환 STRE_OK;
}

/* Advance over a single space.
   return non-zero on error */
정수 curlx_str_singlespace(const char **linep)
{
  반환 curlx_str_single(linep, ' ');
}

/* given an ASCII character and max ascii, return TRUE if valid */
#define valid_digit(x, m) \
  (((x) >= '0') && ((x) <= (m)) && curlx_hexasciitable[(x) - '0'])

/* We use 16 for the zero index (and the necessary bitwise AND in the loop)
   to be able to have a non-zero value there to make valid_digit() able to
   use the info */
const unsigned char curlx_hexasciitable[] = {
  16, 1, 2, 3, 4, 5, 6, 7, 8, 9, /* 0x30: 0 - 9 */
  0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15,        /* 0x41: A - F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15         /* 0x61: a - f */
};

/* no support for 0x prefix nor leading spaces */
static 정수 str_num_base(const char **linep, curl_off_t *nump, curl_off_t max,
                        정수 base) /* 8, 10 or 16, nothing else */
{
  curl_off_t num = 0;
  const char *p;
  정수 m = (base == 10) ? '9' :   /* the largest digit possible */
    (base == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((base == 8) || (base == 10) || (base == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  만약(!valid_digit(*p, m))
    반환 STRE_NO_NUM;
  만약(max < base) {
    /* special-case low max scenario because check needs to be different */
    do {
      정수 n = curlx_hexval(*p++);
      num = (num * base) + n;
      만약(num > max)
        반환 STRE_OVERFLOW;
    } 동안(valid_digit(*p, m));
  }
  아니면 {
    curl_off_t cutoff = (max - (base - 1)) / base;
    동안(valid_digit(*p, m)) {
      만약(num <= cutoff)
        num = (num * base) + curlx_hexval(*p++);
      아니면 {
        정수 n = curlx_hexval(*p++);
        만약(num > ((max - n) / base))
          반환 STRE_OVERFLOW;
        num = (num * base) + n;
      }
    }
  }
  *nump = num;
  *linep = p;
  반환 STRE_OK;
}

/* Get an unsigned decimal number with no leading space or minus. Leading
   zeroes are accepted. return non-zero on error */
정수 curlx_str_number(const char **linep, curl_off_t *nump, curl_off_t max)
{
  반환 str_num_base(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
정수 curlx_str_hex(const char **linep, curl_off_t *nump, curl_off_t max)
{
  반환 str_num_base(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
정수 curlx_str_octal(const char **linep, curl_off_t *nump, curl_off_t max)
{
  반환 str_num_base(linep, nump, max, 8);
}

/*
 * Parse a positive number up to 63-bit number written in ASCII. Skip leading
 * blanks. No support for prefixes.
 */
정수 curlx_str_numblanks(const char **str, curl_off_t *num)
{
  curlx_str_passblanks(str);
  반환 curlx_str_number(str, num, CURL_OFF_T_MAX);
}

/* CR or LF
   return non-zero on error */
정수 curlx_str_newline(const char **linep)
{
  DEBUGASSERT(linep && *linep);
  만약(ISNEWLINE(**linep)) {
    (*linep)++;
    반환 STRE_OK; /* yessir */
  }
  반환 STRE_NEWLINE;
}

#ifndef WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
정수 curlx_str_casecompare(struct Curl_str *str, const char *check)
{
  size_t clen = check ? strlen(check) : 0;
  반환 ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#endif

/* case sensitive string compare. Returns non-zero on match. */
정수 curlx_str_cmp(struct Curl_str *str, const char *check)
{
  만약(check) {
    size_t clen = strlen(check);
    반환 ((str->len == clen) && !strncmp(str->str, check, clen));
  }
  반환 !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
정수 curlx_str_nudge(struct Curl_str *str, size_t num)
{
  만약(num <= str->len) {
    str->str += num;
    str->len -= num;
    반환 STRE_OK;
  }
  반환 STRE_OVERFLOW;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
정수 curlx_str_cspn(const char **linep, struct Curl_str *out,
                   const char *reject)
{
  const char *s = *linep;
  size_t len;
  DEBUGASSERT(linep && *linep);

  len = strcspn(s, reject);
  만약(len) {
    out->str = s;
    out->len = len;
    *linep = &s[len];
    반환 STRE_OK;
  }
  curlx_str_init(out);
  반환 STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
void curlx_str_trimblanks(struct Curl_str *out)
{
  동안(out->len && ISBLANK(*out->str))
    curlx_str_nudge(out, 1);

  /* trim trailing spaces and tabs */
  동안(out->len && ISBLANK(out->str[out->len - 1]))
    out->len--;
}

/* increase the pointer until it has moved over all blanks */
void curlx_str_passblanks(const char **linep)
{
  동안(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
