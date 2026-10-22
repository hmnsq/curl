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

शून्य curlx_str_assign(संरचना Curl_str *out, ध्रुव वर्ण *str, size_t len)
{
  out->str = str;
  out->len = len;
}

/* remove bytes from the end of the string, never remove more bytes than what
   the string holds! */
शून्य curlx_str_trim(संरचना Curl_str *out, size_t len)
{
  DEBUGASSERT(out);
  DEBUGASSERT(out->len >= len);
  out->len -= len;
}

/* Get a word until the first DELIM or end of string. At least one byte long.
   return non-zero on error. If 'max' is zero, it will always return error. */
पूर्णाङ्क curlx_str_until(ध्रुव वर्ण **linep, संरचना Curl_str *out,
                    ध्रुव size_t max, वर्ण delim)
{
  ध्रुव वर्ण *s;
  size_t len = 0;
  DEBUGASSERT(linep);
  DEBUGASSERT(*linep);
  DEBUGASSERT(out);
  DEBUGASSERT(delim);
  s = *linep;

  curlx_str_init(out);
  जाव(*s && (*s != delim)) {
    s++;
    जइ(++len > max) {
      प्रत्यागम STRE_BIG;
    }
  }
  जइ(!len)
    प्रत्यागम STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  प्रत्यागम STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
पूर्णाङ्क curlx_str_word(ध्रुव वर्ण **linep, संरचना Curl_str *out, ध्रुव size_t max)
{
  प्रत्यागम curlx_str_until(linep, out, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
पूर्णाङ्क curlx_str_untilnl(ध्रुव वर्ण **linep, संरचना Curl_str *out,
                      ध्रुव size_t max)
{
  ध्रुव वर्ण *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  जाव(*s && !ISNEWLINE(*s)) {
    s++;
    जइ(++len > max)
      प्रत्यागम STRE_BIG;
  }
  जइ(!len)
    प्रत्यागम STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  प्रत्यागम STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
पूर्णाङ्क curlx_str_quotedword(ध्रुव वर्ण **linep, संरचना Curl_str *out,
                         ध्रुव size_t max)
{
  ध्रुव वर्ण *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  जइ(*s != '\"')
    प्रत्यागम STRE_BEGQUOTE;
  s++;
  जाव(*s && (*s != '\"')) {
    जइ(*s == '\\' && s[1]) {
      s++;
      जइ(++len > max)
        प्रत्यागम STRE_BIG;
    }
    s++;
    जइ(++len > max)
      प्रत्यागम STRE_BIG;
  }
  जइ(*s != '\"')
    प्रत्यागम STRE_ENDQUOTE;
  out->str = (*linep) + 1;
  out->len = len;
  *linep = s + 1;
  प्रत्यागम STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
पूर्णाङ्क curlx_str_single(ध्रुव वर्ण **linep, वर्ण byte)
{
  DEBUGASSERT(linep && *linep);
  जइ(**linep != byte)
    प्रत्यागम STRE_BYTE;
  (*linep)++; /* move over it */
  प्रत्यागम STRE_OK;
}

/* Advance over a single space.
   return non-zero on error */
पूर्णाङ्क curlx_str_singlespace(ध्रुव वर्ण **linep)
{
  प्रत्यागम curlx_str_single(linep, ' ');
}

/* given an ASCII character and max ascii, return TRUE if valid */
#define valid_digit(x, m) \
  (((x) >= '0') && ((x) <= (m)) && curlx_hexasciitable[(x) - '0'])

/* We use 16 for the zero index (and the necessary bitwise AND in the loop)
   to be able to have a non-zero value there to make valid_digit() able to
   use the info */
ध्रुव unsigned वर्ण curlx_hexasciitable[] = {
  16, 1, 2, 3, 4, 5, 6, 7, 8, 9, /* 0x30: 0 - 9 */
  0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15,        /* 0x41: A - F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15         /* 0x61: a - f */
};

/* no support for 0x prefix nor leading spaces */
स्थिर पूर्णाङ्क str_अङ्कः_base(ध्रुव वर्ण **linep, curl_off_t *nump, curl_off_t max,
                        पूर्णाङ्क base) /* 8, 10 or 16, nothing else */
{
  curl_off_t अङ्कः = 0;
  ध्रुव वर्ण *p;
  पूर्णाङ्क m = (base == 10) ? '9' :   /* the largest digit possible */
    (base == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((base == 8) || (base == 10) || (base == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  जइ(!valid_digit(*p, m))
    प्रत्यागम STRE_NO_अङ्कः;
  जइ(max < base) {
    /* special-case low max scenario because check needs to be different */
    करेहि {
      पूर्णाङ्क n = curlx_hexval(*p++);
      अङ्कः = (अङ्कः * base) + n;
      जइ(अङ्कः > max)
        प्रत्यागम STRE_OVERFLOW;
    } जाव(valid_digit(*p, m));
  }
  अण्णहा {
    curl_off_t cutoff = (max - (base - 1)) / base;
    जाव(valid_digit(*p, m)) {
      जइ(अङ्कः <= cutoff)
        अङ्कः = (अङ्कः * base) + curlx_hexval(*p++);
      अण्णहा {
        पूर्णाङ्क n = curlx_hexval(*p++);
        जइ(अङ्कः > ((max - n) / base))
          प्रत्यागम STRE_OVERFLOW;
        अङ्कः = (अङ्कः * base) + n;
      }
    }
  }
  *nump = अङ्कः;
  *linep = p;
  प्रत्यागम STRE_OK;
}

/* Get an unsigned decimal number with no leading space or minus. Leading
   zeroes are accepted. return non-zero on error */
पूर्णाङ्क curlx_str_सङ्ख्या(ध्रुव वर्ण **linep, curl_off_t *nump, curl_off_t max)
{
  प्रत्यागम str_अङ्कः_base(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
पूर्णाङ्क curlx_str_hex(ध्रुव वर्ण **linep, curl_off_t *nump, curl_off_t max)
{
  प्रत्यागम str_अङ्कः_base(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
पूर्णाङ्क curlx_str_octal(ध्रुव वर्ण **linep, curl_off_t *nump, curl_off_t max)
{
  प्रत्यागम str_अङ्कः_base(linep, nump, max, 8);
}

/*
 * Parse a positive number up to 63-bit number written in ASCII. Skip leading
 * blanks. No support for prefixes.
 */
पूर्णाङ्क curlx_str_numblanks(ध्रुव वर्ण **str, curl_off_t *अङ्कः)
{
  curlx_str_passblanks(str);
  प्रत्यागम curlx_str_सङ्ख्या(str, अङ्कः, CURL_OFF_T_MAX);
}

/* CR or LF
   return non-zero on error */
पूर्णाङ्क curlx_str_newline(ध्रुव वर्ण **linep)
{
  DEBUGASSERT(linep && *linep);
  जइ(ISNEWLINE(**linep)) {
    (*linep)++;
    प्रत्यागम STRE_OK; /* yessir */
  }
  प्रत्यागम STRE_NEWLINE;
}

#ifndef WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
पूर्णाङ्क curlx_str_casecompare(संरचना Curl_str *str, ध्रुव वर्ण *check)
{
  size_t clen = check ? strlen(check) : 0;
  प्रत्यागम ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#endif

/* case sensitive string compare. Returns non-zero on match. */
पूर्णाङ्क curlx_str_cmp(संरचना Curl_str *str, ध्रुव वर्ण *check)
{
  जइ(check) {
    size_t clen = strlen(check);
    प्रत्यागम ((str->len == clen) && !strncmp(str->str, check, clen));
  }
  प्रत्यागम !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
पूर्णाङ्क curlx_str_nudge(संरचना Curl_str *str, size_t अङ्कः)
{
  जइ(अङ्कः <= str->len) {
    str->str += अङ्कः;
    str->len -= अङ्कः;
    प्रत्यागम STRE_OK;
  }
  प्रत्यागम STRE_OVERFLOW;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
पूर्णाङ्क curlx_str_cspn(ध्रुव वर्ण **linep, संरचना Curl_str *out,
                   ध्रुव वर्ण *reject)
{
  ध्रुव वर्ण *s = *linep;
  size_t len;
  DEBUGASSERT(linep && *linep);

  len = strcspn(s, reject);
  जइ(len) {
    out->str = s;
    out->len = len;
    *linep = &s[len];
    प्रत्यागम STRE_OK;
  }
  curlx_str_init(out);
  प्रत्यागम STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
शून्य curlx_str_trimblanks(संरचना Curl_str *out)
{
  जाव(out->len && ISBLANK(*out->str))
    curlx_str_nudge(out, 1);

  /* trim trailing spaces and tabs */
  जाव(out->len && ISBLANK(out->str[out->len - 1]))
    out->len--;
}

/* increase the pointer until it has moved over all blanks */
शून्य curlx_str_passblanks(ध्रुव वर्ण **linep)
{
  जाव(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
