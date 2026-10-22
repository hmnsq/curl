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
#अन्तर्भाव "curlx/strparse.h"

शून्य curlx_str_init(संरचना Curl_str *out)
{
  out->str = रिक्त;
  out->len = 0;
}

शून्य curlx_str_assign(संरचना Curl_str *out, स्थिरांक अक्षर *str, माप_प्रकार len)
{
  out->str = str;
  out->len = len;
}

/* remove bytes from the end of the string, never remove more bytes than what
   the string holds! */
शून्य curlx_str_trim(संरचना Curl_str *out, माप_प्रकार len)
{
  DEBUGASSERT(out);
  DEBUGASSERT(out->len >= len);
  out->len -= len;
}

/* Get a word until the first DELIM or end of string. At least one byte long.
   return non-zero on error. If 'max' is zero, it will always return error. */
पूर्णांक curlx_str_until(स्थिरांक अक्षर **linep, संरचना Curl_str *out,
                    स्थिरांक माप_प्रकार max, अक्षर delim)
{
  स्थिरांक अक्षर *s;
  माप_प्रकार len = 0;
  DEBUGASSERT(linep);
  DEBUGASSERT(*linep);
  DEBUGASSERT(out);
  DEBUGASSERT(delim);
  s = *linep;

  curlx_str_init(out);
  पर्यंत(*s && (*s != delim)) {
    s++;
    जर(++len > max) {
      परत STRE_BIG;
    }
  }
  जर(!len)
    परत STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  परत STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
पूर्णांक curlx_str_word(स्थिरांक अक्षर **linep, संरचना Curl_str *out, स्थिरांक माप_प्रकार max)
{
  परत curlx_str_until(linep, out, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
पूर्णांक curlx_str_untilnl(स्थिरांक अक्षर **linep, संरचना Curl_str *out,
                      स्थिरांक माप_प्रकार max)
{
  स्थिरांक अक्षर *s = *linep;
  माप_प्रकार len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  पर्यंत(*s && !ISNEWLINE(*s)) {
    s++;
    जर(++len > max)
      परत STRE_BIG;
  }
  जर(!len)
    परत STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  परत STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
पूर्णांक curlx_str_quotedword(स्थिरांक अक्षर **linep, संरचना Curl_str *out,
                         स्थिरांक माप_प्रकार max)
{
  स्थिरांक अक्षर *s = *linep;
  माप_प्रकार len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  जर(*s != '\"')
    परत STRE_BEGQUOTE;
  s++;
  पर्यंत(*s && (*s != '\"')) {
    जर(*s == '\\' && s[1]) {
      s++;
      जर(++len > max)
        परत STRE_BIG;
    }
    s++;
    जर(++len > max)
      परत STRE_BIG;
  }
  जर(*s != '\"')
    परत STRE_ENDQUOTE;
  out->str = (*linep) + 1;
  out->len = len;
  *linep = s + 1;
  परत STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
पूर्णांक curlx_str_single(स्थिरांक अक्षर **linep, अक्षर byte)
{
  DEBUGASSERT(linep && *linep);
  जर(**linep != byte)
    परत STRE_BYTE;
  (*linep)++; /* move over it */
  परत STRE_OK;
}

/* Advance over a single space.
   return non-zero on error */
पूर्णांक curlx_str_singlespace(स्थिरांक अक्षर **linep)
{
  परत curlx_str_single(linep, ' ');
}

/* given an ASCII character and max ascii, return TRUE if valid */
#define valid_digit(x, m) \
  (((x) >= '0') && ((x) <= (m)) && curlx_hexasciitable[(x) - '0'])

/* We use 16 for the zero index (and the necessary bitwise AND in the loop)
   to be able to have a non-zero value there to make valid_digit() able to
   use the info */
स्थिरांक अचिन्हित अक्षर curlx_hexasciitable[] = {
  16, 1, 2, 3, 4, 5, 6, 7, 8, 9, /* 0x30: 0 - 9 */
  0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15,        /* 0x41: A - F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15         /* 0x61: a - f */
};

/* no support for 0x prefix nor leading spaces */
स्थिर पूर्णांक str_अंक_base(स्थिरांक अक्षर **linep, curl_off_t *nump, curl_off_t max,
                        पूर्णांक base) /* 8, 10 or 16, nothing else */
{
  curl_off_t अंक = 0;
  स्थिरांक अक्षर *p;
  पूर्णांक m = (base == 10) ? '9' :   /* the largest digit possible */
    (base == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((base == 8) || (base == 10) || (base == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  जर(!valid_digit(*p, m))
    परत STRE_NO_अंक;
  जर(max < base) {
    /* special-case low max scenario because check needs to be different */
    करा {
      पूर्णांक n = curlx_hexval(*p++);
      अंक = (अंक * base) + n;
      जर(अंक > max)
        परत STRE_OVERFLOW;
    } पर्यंत(valid_digit(*p, m));
  }
  अन्यथा {
    curl_off_t cutoff = (max - (base - 1)) / base;
    पर्यंत(valid_digit(*p, m)) {
      जर(अंक <= cutoff)
        अंक = (अंक * base) + curlx_hexval(*p++);
      अन्यथा {
        पूर्णांक n = curlx_hexval(*p++);
        जर(अंक > ((max - n) / base))
          परत STRE_OVERFLOW;
        अंक = (अंक * base) + n;
      }
    }
  }
  *nump = अंक;
  *linep = p;
  परत STRE_OK;
}

/* Get an unsigned decimal number with no leading space or minus. Leading
   zeroes are accepted. return non-zero on error */
पूर्णांक curlx_str_संख्या(स्थिरांक अक्षर **linep, curl_off_t *nump, curl_off_t max)
{
  परत str_अंक_base(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
पूर्णांक curlx_str_hex(स्थिरांक अक्षर **linep, curl_off_t *nump, curl_off_t max)
{
  परत str_अंक_base(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
पूर्णांक curlx_str_octal(स्थिरांक अक्षर **linep, curl_off_t *nump, curl_off_t max)
{
  परत str_अंक_base(linep, nump, max, 8);
}

/*
 * Parse a positive number up to 63-bit number written in ASCII. Skip leading
 * blanks. No support for prefixes.
 */
पूर्णांक curlx_str_numblanks(स्थिरांक अक्षर **str, curl_off_t *अंक)
{
  curlx_str_passblanks(str);
  परत curlx_str_संख्या(str, अंक, CURL_OFF_T_MAX);
}

/* CR or LF
   return non-zero on error */
पूर्णांक curlx_str_newline(स्थिरांक अक्षर **linep)
{
  DEBUGASSERT(linep && *linep);
  जर(ISNEWLINE(**linep)) {
    (*linep)++;
    परत STRE_OK; /* yessir */
  }
  परत STRE_NEWLINE;
}

#ifndef WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
पूर्णांक curlx_str_casecompare(संरचना Curl_str *str, स्थिरांक अक्षर *check)
{
  माप_प्रकार clen = check ? म_माप(check) : 0;
  परत ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#endif

/* case sensitive string compare. Returns non-zero on match. */
पूर्णांक curlx_str_cmp(संरचना Curl_str *str, स्थिरांक अक्षर *check)
{
  जर(check) {
    माप_प्रकार clen = म_माप(check);
    परत ((str->len == clen) && !म_भेदन(str->str, check, clen));
  }
  परत !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
पूर्णांक curlx_str_nudge(संरचना Curl_str *str, माप_प्रकार अंक)
{
  जर(अंक <= str->len) {
    str->str += अंक;
    str->len -= अंक;
    परत STRE_OK;
  }
  परत STRE_OVERFLOW;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
पूर्णांक curlx_str_cspn(स्थिरांक अक्षर **linep, संरचना Curl_str *out,
                   स्थिरांक अक्षर *reject)
{
  स्थिरांक अक्षर *s = *linep;
  माप_प्रकार len;
  DEBUGASSERT(linep && *linep);

  len = म_खोज(s, reject);
  जर(len) {
    out->str = s;
    out->len = len;
    *linep = &s[len];
    परत STRE_OK;
  }
  curlx_str_init(out);
  परत STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
शून्य curlx_str_trimblanks(संरचना Curl_str *out)
{
  पर्यंत(out->len && ISBLANK(*out->str))
    curlx_str_nudge(out, 1);

  /* trim trailing spaces and tabs */
  पर्यंत(out->len && ISBLANK(out->str[out->len - 1]))
    out->len--;
}

/* increase the pointer until it has moved over all blanks */
शून्य curlx_str_passblanks(स्थिरांक अक्षर **linep)
{
  पर्यंत(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
