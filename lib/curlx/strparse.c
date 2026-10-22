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
#અન્તર્ભાવ "curlx/strparse.h"

શૂન્ય curlx_str_init(સંરચના Curl_str *out)
{
  out->str = ખાલી;
  out->len = 0;
}

શૂન્ય curlx_str_assign(સંરચના Curl_str *out, અચલ અક્ષર *str, size_t len)
{
  out->str = str;
  out->len = len;
}

/* remove bytes from the end of the string, never remove more bytes than what
   the string holds! */
શૂન્ય curlx_str_trim(સંરચના Curl_str *out, size_t len)
{
  DEBUGASSERT(out);
  DEBUGASSERT(out->len >= len);
  out->len -= len;
}

/* Get a word until the first DELIM or end of string. At least one byte long.
   return non-zero on error. If 'max' is zero, it will always return error. */
પૂર્ણાંક curlx_str_until(અચલ અક્ષર **linep, સંરચના Curl_str *out,
                    અચલ size_t max, અક્ષર delim)
{
  અચલ અક્ષર *s;
  size_t len = 0;
  DEBUGASSERT(linep);
  DEBUGASSERT(*linep);
  DEBUGASSERT(out);
  DEBUGASSERT(delim);
  s = *linep;

  curlx_str_init(out);
  જ્યારે(*s && (*s != delim)) {
    s++;
    જો(++len > max) {
      પાછા STRE_BIG;
    }
  }
  જો(!len)
    પાછા STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  પાછા STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
પૂર્ણાંક curlx_str_word(અચલ અક્ષર **linep, સંરચના Curl_str *out, અચલ size_t max)
{
  પાછા curlx_str_until(linep, out, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
પૂર્ણાંક curlx_str_untilnl(અચલ અક્ષર **linep, સંરચના Curl_str *out,
                      અચલ size_t max)
{
  અચલ અક્ષર *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  જ્યારે(*s && !ISNEWLINE(*s)) {
    s++;
    જો(++len > max)
      પાછા STRE_BIG;
  }
  જો(!len)
    પાછા STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  પાછા STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
પૂર્ણાંક curlx_str_quotedword(અચલ અક્ષર **linep, સંરચના Curl_str *out,
                         અચલ size_t max)
{
  અચલ અક્ષર *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  જો(*s != '\"')
    પાછા STRE_BEGQUOTE;
  s++;
  જ્યારે(*s && (*s != '\"')) {
    જો(*s == '\\' && s[1]) {
      s++;
      જો(++len > max)
        પાછા STRE_BIG;
    }
    s++;
    જો(++len > max)
      પાછા STRE_BIG;
  }
  જો(*s != '\"')
    પાછા STRE_ENDQUOTE;
  out->str = (*linep) + 1;
  out->len = len;
  *linep = s + 1;
  પાછા STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
પૂર્ણાંક curlx_str_single(અચલ અક્ષર **linep, અક્ષર byte)
{
  DEBUGASSERT(linep && *linep);
  જો(**linep != byte)
    પાછા STRE_BYTE;
  (*linep)++; /* move over it */
  પાછા STRE_OK;
}

/* Advance over a single space.
   return non-zero on error */
પૂર્ણાંક curlx_str_singlespace(અચલ અક્ષર **linep)
{
  પાછા curlx_str_single(linep, ' ');
}

/* given an ASCII character and max ascii, return TRUE if valid */
#define valid_digit(x, m) \
  (((x) >= '0') && ((x) <= (m)) && curlx_hexasciitable[(x) - '0'])

/* We use 16 for the zero index (and the necessary bitwise AND in the loop)
   to be able to have a non-zero value there to make valid_digit() able to
   use the info */
અચલ unsigned અક્ષર curlx_hexasciitable[] = {
  16, 1, 2, 3, 4, 5, 6, 7, 8, 9, /* 0x30: 0 - 9 */
  0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15,        /* 0x41: A - F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15         /* 0x61: a - f */
};

/* no support for 0x prefix nor leading spaces */
સ્થિર પૂર્ણાંક str_અંક_base(અચલ અક્ષર **linep, curl_off_t *nump, curl_off_t max,
                        પૂર્ણાંક base) /* 8, 10 or 16, nothing else */
{
  curl_off_t અંક = 0;
  અચલ અક્ષર *p;
  પૂર્ણાંક m = (base == 10) ? '9' :   /* the largest digit possible */
    (base == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((base == 8) || (base == 10) || (base == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  જો(!valid_digit(*p, m))
    પાછા STRE_NO_અંક;
  જો(max < base) {
    /* special-case low max scenario because check needs to be different */
    કરો {
      પૂર્ણાંક n = curlx_hexval(*p++);
      અંક = (અંક * base) + n;
      જો(અંક > max)
        પાછા STRE_OVERFLOW;
    } જ્યારે(valid_digit(*p, m));
  }
  નહીંતર {
    curl_off_t cutoff = (max - (base - 1)) / base;
    જ્યારે(valid_digit(*p, m)) {
      જો(અંક <= cutoff)
        અંક = (અંક * base) + curlx_hexval(*p++);
      નહીંતર {
        પૂર્ણાંક n = curlx_hexval(*p++);
        જો(અંક > ((max - n) / base))
          પાછા STRE_OVERFLOW;
        અંક = (અંક * base) + n;
      }
    }
  }
  *nump = અંક;
  *linep = p;
  પાછા STRE_OK;
}

/* Get an unsigned decimal number with no leading space or minus. Leading
   zeroes are accepted. return non-zero on error */
પૂર્ણાંક curlx_str_સંખ્યા(અચલ અક્ષર **linep, curl_off_t *nump, curl_off_t max)
{
  પાછા str_અંક_base(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
પૂર્ણાંક curlx_str_hex(અચલ અક્ષર **linep, curl_off_t *nump, curl_off_t max)
{
  પાછા str_અંક_base(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
પૂર્ણાંક curlx_str_octal(અચલ અક્ષર **linep, curl_off_t *nump, curl_off_t max)
{
  પાછા str_અંક_base(linep, nump, max, 8);
}

/*
 * Parse a positive number up to 63-bit number written in ASCII. Skip leading
 * blanks. No support for prefixes.
 */
પૂર્ણાંક curlx_str_numblanks(અચલ અક્ષર **str, curl_off_t *અંક)
{
  curlx_str_passblanks(str);
  પાછા curlx_str_સંખ્યા(str, અંક, CURL_OFF_T_MAX);
}

/* CR or LF
   return non-zero on error */
પૂર્ણાંક curlx_str_newline(અચલ અક્ષર **linep)
{
  DEBUGASSERT(linep && *linep);
  જો(ISNEWLINE(**linep)) {
    (*linep)++;
    પાછા STRE_OK; /* yessir */
  }
  પાછા STRE_NEWLINE;
}

#ifndef WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
પૂર્ણાંક curlx_str_casecompare(સંરચના Curl_str *str, અચલ અક્ષર *check)
{
  size_t clen = check ? strlen(check) : 0;
  પાછા ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#endif

/* case sensitive string compare. Returns non-zero on match. */
પૂર્ણાંક curlx_str_cmp(સંરચના Curl_str *str, અચલ અક્ષર *check)
{
  જો(check) {
    size_t clen = strlen(check);
    પાછા ((str->len == clen) && !strncmp(str->str, check, clen));
  }
  પાછા !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
પૂર્ણાંક curlx_str_nudge(સંરચના Curl_str *str, size_t અંક)
{
  જો(અંક <= str->len) {
    str->str += અંક;
    str->len -= અંક;
    પાછા STRE_OK;
  }
  પાછા STRE_OVERFLOW;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
પૂર્ણાંક curlx_str_cspn(અચલ અક્ષર **linep, સંરચના Curl_str *out,
                   અચલ અક્ષર *reject)
{
  અચલ અક્ષર *s = *linep;
  size_t len;
  DEBUGASSERT(linep && *linep);

  len = strcspn(s, reject);
  જો(len) {
    out->str = s;
    out->len = len;
    *linep = &s[len];
    પાછા STRE_OK;
  }
  curlx_str_init(out);
  પાછા STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
શૂન્ય curlx_str_trimblanks(સંરચના Curl_str *out)
{
  જ્યારે(out->len && ISBLANK(*out->str))
    curlx_str_nudge(out, 1);

  /* trim trailing spaces and tabs */
  જ્યારે(out->len && ISBLANK(out->str[out->len - 1]))
    out->len--;
}

/* increase the pointer until it has moved over all blanks */
શૂન્ય curlx_str_passblanks(અચલ અક્ષર **linep)
{
  જ્યારે(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
