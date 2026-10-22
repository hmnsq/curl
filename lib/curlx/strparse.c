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
цяло curlx_str_until(const char **linep, struct Curl_str *out,
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
  докато(*s && (*s != delim)) {
    s++;
    ако(++len > max) {
      върни STRE_BIG;
    }
  }
  ако(!len)
    върни STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  върни STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
цяло curlx_str_word(const char **linep, struct Curl_str *out, const size_t max)
{
  върни curlx_str_until(linep, out, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
цяло curlx_str_untilnl(const char **linep, struct Curl_str *out,
                      const size_t max)
{
  const char *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  докато(*s && !ISNEWLINE(*s)) {
    s++;
    ако(++len > max)
      върни STRE_BIG;
  }
  ако(!len)
    върни STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  върни STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
цяло curlx_str_quotedword(const char **linep, struct Curl_str *out,
                         const size_t max)
{
  const char *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  ако(*s != '\"')
    върни STRE_BEGQUOTE;
  s++;
  докато(*s && (*s != '\"')) {
    ако(*s == '\\' && s[1]) {
      s++;
      ако(++len > max)
        върни STRE_BIG;
    }
    s++;
    ако(++len > max)
      върни STRE_BIG;
  }
  ако(*s != '\"')
    върни STRE_ENDQUOTE;
  out->str = (*linep) + 1;
  out->len = len;
  *linep = s + 1;
  върни STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
цяло curlx_str_single(const char **linep, char byte)
{
  DEBUGASSERT(linep && *linep);
  ако(**linep != byte)
    върни STRE_BYTE;
  (*linep)++; /* move over it */
  върни STRE_OK;
}

/* Advance over a single space.
   return non-zero on error */
цяло curlx_str_singlespace(const char **linep)
{
  върни curlx_str_single(linep, ' ');
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
static цяло str_num_base(const char **linep, curl_off_t *nump, curl_off_t max,
                        цяло base) /* 8, 10 or 16, nothing else */
{
  curl_off_t num = 0;
  const char *p;
  цяло m = (base == 10) ? '9' :   /* the largest digit possible */
    (base == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((base == 8) || (base == 10) || (base == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  ако(!valid_digit(*p, m))
    върни STRE_NO_NUM;
  ако(max < base) {
    /* special-case low max scenario because check needs to be different */
    do {
      цяло n = curlx_hexval(*p++);
      num = (num * base) + n;
      ако(num > max)
        върни STRE_OVERFLOW;
    } докато(valid_digit(*p, m));
  }
  иначе {
    curl_off_t cutoff = (max - (base - 1)) / base;
    докато(valid_digit(*p, m)) {
      ако(num <= cutoff)
        num = (num * base) + curlx_hexval(*p++);
      иначе {
        цяло n = curlx_hexval(*p++);
        ако(num > ((max - n) / base))
          върни STRE_OVERFLOW;
        num = (num * base) + n;
      }
    }
  }
  *nump = num;
  *linep = p;
  върни STRE_OK;
}

/* Get an unsigned decimal number with no leading space or minus. Leading
   zeroes are accepted. return non-zero on error */
цяло curlx_str_number(const char **linep, curl_off_t *nump, curl_off_t max)
{
  върни str_num_base(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
цяло curlx_str_hex(const char **linep, curl_off_t *nump, curl_off_t max)
{
  върни str_num_base(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
цяло curlx_str_octal(const char **linep, curl_off_t *nump, curl_off_t max)
{
  върни str_num_base(linep, nump, max, 8);
}

/*
 * Parse a positive number up to 63-bit number written in ASCII. Skip leading
 * blanks. No support for prefixes.
 */
цяло curlx_str_numblanks(const char **str, curl_off_t *num)
{
  curlx_str_passblanks(str);
  върни curlx_str_number(str, num, CURL_OFF_T_MAX);
}

/* CR or LF
   return non-zero on error */
цяло curlx_str_newline(const char **linep)
{
  DEBUGASSERT(linep && *linep);
  ако(ISNEWLINE(**linep)) {
    (*linep)++;
    върни STRE_OK; /* yessir */
  }
  върни STRE_NEWLINE;
}

#ifndef WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
цяло curlx_str_casecompare(struct Curl_str *str, const char *check)
{
  size_t clen = check ? strlen(check) : 0;
  върни ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#endif

/* case sensitive string compare. Returns non-zero on match. */
цяло curlx_str_cmp(struct Curl_str *str, const char *check)
{
  ако(check) {
    size_t clen = strlen(check);
    върни ((str->len == clen) && !strncmp(str->str, check, clen));
  }
  върни !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
цяло curlx_str_nudge(struct Curl_str *str, size_t num)
{
  ако(num <= str->len) {
    str->str += num;
    str->len -= num;
    върни STRE_OK;
  }
  върни STRE_OVERFLOW;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
цяло curlx_str_cspn(const char **linep, struct Curl_str *out,
                   const char *reject)
{
  const char *s = *linep;
  size_t len;
  DEBUGASSERT(linep && *linep);

  len = strcspn(s, reject);
  ако(len) {
    out->str = s;
    out->len = len;
    *linep = &s[len];
    върни STRE_OK;
  }
  curlx_str_init(out);
  върни STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
void curlx_str_trimblanks(struct Curl_str *out)
{
  докато(out->len && ISBLANK(*out->str))
    curlx_str_nudge(out, 1);

  /* trim trailing spaces and tabs */
  докато(out->len && ISBLANK(out->str[out->len - 1]))
    out->len--;
}

/* increase the pointer until it has moved over all blanks */
void curlx_str_passblanks(const char **linep)
{
  докато(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
