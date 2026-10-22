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
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

ଶୂନ୍ୟ curlx_str_init(ସଂରଚନା Curl_str *out)
{
  out->str = ଖାଲି;
  out->len = 0;
}

ଶୂନ୍ୟ curlx_str_assign(ସଂରଚନା Curl_str *out, ଅଚଳ ଅକ୍ଷର *str, size_t len)
{
  out->str = str;
  out->len = len;
}

/* remove bytes from the end of the string, never remove more bytes than what
   the string holds! */
ଶୂନ୍ୟ curlx_str_trim(ସଂରଚନା Curl_str *out, size_t len)
{
  DEBUGASSERT(out);
  DEBUGASSERT(out->len >= len);
  out->len -= len;
}

/* Get a word until the first DELIM or end of string. At least one byte long.
   return non-zero on error. If 'max' is zero, it will always return error. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_until(ଅଚଳ ଅକ୍ଷର **linep, ସଂରଚନା Curl_str *out,
                    ଅଚଳ size_t max, ଅକ୍ଷର delim)
{
  ଅଚଳ ଅକ୍ଷର *s;
  size_t len = 0;
  DEBUGASSERT(linep);
  DEBUGASSERT(*linep);
  DEBUGASSERT(out);
  DEBUGASSERT(delim);
  s = *linep;

  curlx_str_init(out);
  ଯେତେବେଳେ(*s && (*s != delim)) {
    s++;
    ଯଦି(++len > max) {
      ଫେରନ୍ତୁ STRE_BIG;
    }
  }
  ଯଦି(!len)
    ଫେରନ୍ତୁ STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  ଫେରନ୍ତୁ STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_word(ଅଚଳ ଅକ୍ଷର **linep, ସଂରଚନା Curl_str *out, ଅଚଳ size_t max)
{
  ଫେରନ୍ତୁ curlx_str_until(linep, out, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_untilnl(ଅଚଳ ଅକ୍ଷର **linep, ସଂରଚନା Curl_str *out,
                      ଅଚଳ size_t max)
{
  ଅଚଳ ଅକ୍ଷର *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  ଯେତେବେଳେ(*s && !ISNEWLINE(*s)) {
    s++;
    ଯଦି(++len > max)
      ଫେରନ୍ତୁ STRE_BIG;
  }
  ଯଦି(!len)
    ଫେରନ୍ତୁ STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  ଫେରନ୍ତୁ STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_quotedword(ଅଚଳ ଅକ୍ଷର **linep, ସଂରଚନା Curl_str *out,
                         ଅଚଳ size_t max)
{
  ଅଚଳ ଅକ୍ଷର *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  ଯଦି(*s != '\"')
    ଫେରନ୍ତୁ STRE_BEGQUOTE;
  s++;
  ଯେତେବେଳେ(*s && (*s != '\"')) {
    ଯଦି(*s == '\\' && s[1]) {
      s++;
      ଯଦି(++len > max)
        ଫେରନ୍ତୁ STRE_BIG;
    }
    s++;
    ଯଦି(++len > max)
      ଫେରନ୍ତୁ STRE_BIG;
  }
  ଯଦି(*s != '\"')
    ଫେରନ୍ତୁ STRE_ENDQUOTE;
  out->str = (*linep) + 1;
  out->len = len;
  *linep = s + 1;
  ଫେରନ୍ତୁ STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_single(ଅଚଳ ଅକ୍ଷର **linep, ଅକ୍ଷର byte)
{
  DEBUGASSERT(linep && *linep);
  ଯଦି(**linep != byte)
    ଫେରନ୍ତୁ STRE_BYTE;
  (*linep)++; /* move over it */
  ଫେରନ୍ତୁ STRE_OK;
}

/* Advance over a single space.
   return non-zero on error */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_singlespace(ଅଚଳ ଅକ୍ଷର **linep)
{
  ଫେରନ୍ତୁ curlx_str_single(linep, ' ');
}

/* given an ASCII character and max ascii, return TRUE if valid */
#define valid_digit(x, m) \
  (((x) >= '0') && ((x) <= (m)) && curlx_hexasciitable[(x) - '0'])

/* We use 16 for the zero index (and the necessary bitwise AND in the loop)
   to be able to have a non-zero value there to make valid_digit() able to
   use the info */
ଅଚଳ unsigned ଅକ୍ଷର curlx_hexasciitable[] = {
  16, 1, 2, 3, 4, 5, 6, 7, 8, 9, /* 0x30: 0 - 9 */
  0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15,        /* 0x41: A - F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15         /* 0x61: a - f */
};

/* no support for 0x prefix nor leading spaces */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା str_ଅଙ୍କ_base(ଅଚଳ ଅକ୍ଷର **linep, curl_off_t *nump, curl_off_t max,
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା base) /* 8, 10 or 16, nothing else */
{
  curl_off_t ଅଙ୍କ = 0;
  ଅଚଳ ଅକ୍ଷର *p;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା m = (base == 10) ? '9' :   /* the largest digit possible */
    (base == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((base == 8) || (base == 10) || (base == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  ଯଦି(!valid_digit(*p, m))
    ଫେରନ୍ତୁ STRE_NO_ଅଙ୍କ;
  ଯଦି(max < base) {
    /* special-case low max scenario because check needs to be different */
    କର {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = curlx_hexval(*p++);
      ଅଙ୍କ = (ଅଙ୍କ * base) + n;
      ଯଦି(ଅଙ୍କ > max)
        ଫେରନ୍ତୁ STRE_OVERFLOW;
    } ଯେତେବେଳେ(valid_digit(*p, m));
  }
  ଅନ୍ୟଥା {
    curl_off_t cutoff = (max - (base - 1)) / base;
    ଯେତେବେଳେ(valid_digit(*p, m)) {
      ଯଦି(ଅଙ୍କ <= cutoff)
        ଅଙ୍କ = (ଅଙ୍କ * base) + curlx_hexval(*p++);
      ଅନ୍ୟଥା {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = curlx_hexval(*p++);
        ଯଦି(ଅଙ୍କ > ((max - n) / base))
          ଫେରନ୍ତୁ STRE_OVERFLOW;
        ଅଙ୍କ = (ଅଙ୍କ * base) + n;
      }
    }
  }
  *nump = ଅଙ୍କ;
  *linep = p;
  ଫେରନ୍ତୁ STRE_OK;
}

/* Get an unsigned decimal number with no leading space or minus. Leading
   zeroes are accepted. return non-zero on error */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_ସଂଖ୍ୟା(ଅଚଳ ଅକ୍ଷର **linep, curl_off_t *nump, curl_off_t max)
{
  ଫେରନ୍ତୁ str_ଅଙ୍କ_base(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_hex(ଅଚଳ ଅକ୍ଷର **linep, curl_off_t *nump, curl_off_t max)
{
  ଫେରନ୍ତୁ str_ଅଙ୍କ_base(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_octal(ଅଚଳ ଅକ୍ଷର **linep, curl_off_t *nump, curl_off_t max)
{
  ଫେରନ୍ତୁ str_ଅଙ୍କ_base(linep, nump, max, 8);
}

/*
 * Parse a positive number up to 63-bit number written in ASCII. Skip leading
 * blanks. No support for prefixes.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_numblanks(ଅଚଳ ଅକ୍ଷର **str, curl_off_t *ଅଙ୍କ)
{
  curlx_str_passblanks(str);
  ଫେରନ୍ତୁ curlx_str_ସଂଖ୍ୟା(str, ଅଙ୍କ, CURL_OFF_T_MAX);
}

/* CR or LF
   return non-zero on error */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_newline(ଅଚଳ ଅକ୍ଷର **linep)
{
  DEBUGASSERT(linep && *linep);
  ଯଦି(ISNEWLINE(**linep)) {
    (*linep)++;
    ଫେରନ୍ତୁ STRE_OK; /* yessir */
  }
  ଫେରନ୍ତୁ STRE_NEWLINE;
}

#ifndef WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_casecompare(ସଂରଚନା Curl_str *str, ଅଚଳ ଅକ୍ଷର *check)
{
  size_t clen = check ? strlen(check) : 0;
  ଫେରନ୍ତୁ ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#endif

/* case sensitive string compare. Returns non-zero on match. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_cmp(ସଂରଚନା Curl_str *str, ଅଚଳ ଅକ୍ଷର *check)
{
  ଯଦି(check) {
    size_t clen = strlen(check);
    ଫେରନ୍ତୁ ((str->len == clen) && !strncmp(str->str, check, clen));
  }
  ଫେରନ୍ତୁ !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_nudge(ସଂରଚନା Curl_str *str, size_t ଅଙ୍କ)
{
  ଯଦି(ଅଙ୍କ <= str->len) {
    str->str += ଅଙ୍କ;
    str->len -= ଅଙ୍କ;
    ଫେରନ୍ତୁ STRE_OK;
  }
  ଫେରନ୍ତୁ STRE_OVERFLOW;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_str_cspn(ଅଚଳ ଅକ୍ଷର **linep, ସଂରଚନା Curl_str *out,
                   ଅଚଳ ଅକ୍ଷର *reject)
{
  ଅଚଳ ଅକ୍ଷର *s = *linep;
  size_t len;
  DEBUGASSERT(linep && *linep);

  len = strcspn(s, reject);
  ଯଦି(len) {
    out->str = s;
    out->len = len;
    *linep = &s[len];
    ଫେରନ୍ତୁ STRE_OK;
  }
  curlx_str_init(out);
  ଫେରନ୍ତୁ STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
ଶୂନ୍ୟ curlx_str_trimblanks(ସଂରଚନା Curl_str *out)
{
  ଯେତେବେଳେ(out->len && ISBLANK(*out->str))
    curlx_str_nudge(out, 1);

  /* trim trailing spaces and tabs */
  ଯେତେବେଳେ(out->len && ISBLANK(out->str[out->len - 1]))
    out->len--;
}

/* increase the pointer until it has moved over all blanks */
ଶୂନ୍ୟ curlx_str_passblanks(ଅଚଳ ଅକ୍ଷର **linep)
{
  ଯେତେବେଳେ(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
