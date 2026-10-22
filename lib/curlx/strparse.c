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

शून्य curlx_str_assign(संरचना Curl_str *out, अचर वर्ण *str, माप_प्रकार len)
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
पूर्णाङ्क curlx_str_until(अचर वर्ण **linep, संरचना Curl_str *out,
                    अचर माप_प्रकार max, वर्ण delim)
{
  अचर वर्ण *s;
  माप_प्रकार len = 0;
  DEBUGASSERT(linep);
  DEBUGASSERT(*linep);
  DEBUGASSERT(out);
  DEBUGASSERT(delim);
  s = *linep;

  curlx_str_init(out);
  जबसम्म(*s && (*s != delim)) {
    s++;
    यदि(++len > max) {
      फर्काऊ STRE_BIG;
    }
  }
  यदि(!len)
    फर्काऊ STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  फर्काऊ STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
पूर्णाङ्क curlx_str_word(अचर वर्ण **linep, संरचना Curl_str *out, अचर माप_प्रकार max)
{
  फर्काऊ curlx_str_until(linep, out, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
पूर्णाङ्क curlx_str_untilnl(अचर वर्ण **linep, संरचना Curl_str *out,
                      अचर माप_प्रकार max)
{
  अचर वर्ण *s = *linep;
  माप_प्रकार len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  जबसम्म(*s && !ISNEWLINE(*s)) {
    s++;
    यदि(++len > max)
      फर्काऊ STRE_BIG;
  }
  यदि(!len)
    फर्काऊ STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  फर्काऊ STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
पूर्णाङ्क curlx_str_quotedword(अचर वर्ण **linep, संरचना Curl_str *out,
                         अचर माप_प्रकार max)
{
  अचर वर्ण *s = *linep;
  माप_प्रकार len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  यदि(*s != '\"')
    फर्काऊ STRE_BEGQUOTE;
  s++;
  जबसम्म(*s && (*s != '\"')) {
    यदि(*s == '\\' && s[1]) {
      s++;
      यदि(++len > max)
        फर्काऊ STRE_BIG;
    }
    s++;
    यदि(++len > max)
      फर्काऊ STRE_BIG;
  }
  यदि(*s != '\"')
    फर्काऊ STRE_ENDQUOTE;
  out->str = (*linep) + 1;
  out->len = len;
  *linep = s + 1;
  फर्काऊ STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
पूर्णाङ्क curlx_str_single(अचर वर्ण **linep, वर्ण byte)
{
  DEBUGASSERT(linep && *linep);
  यदि(**linep != byte)
    फर्काऊ STRE_BYTE;
  (*linep)++; /* move over it */
  फर्काऊ STRE_OK;
}

/* Advance over a single space.
   return non-zero on error */
पूर्णाङ्क curlx_str_singlespace(अचर वर्ण **linep)
{
  फर्काऊ curlx_str_single(linep, ' ');
}

/* given an ASCII character and max ascii, return TRUE if valid */
#define valid_digit(x, m) \
  (((x) >= '0') && ((x) <= (m)) && curlx_hexasciitable[(x) - '0'])

/* We use 16 for the zero index (and the necessary bitwise AND in the loop)
   to be able to have a non-zero value there to make valid_digit() able to
   use the info */
अचर अचिन्हित वर्ण curlx_hexasciitable[] = {
  16, 1, 2, 3, 4, 5, 6, 7, 8, 9, /* 0x30: 0 - 9 */
  0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15,        /* 0x41: A - F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15         /* 0x61: a - f */
};

/* no support for 0x prefix nor leading spaces */
स्थिर पूर्णाङ्क str_अङ्क_base(अचर वर्ण **linep, curl_off_t *nump, curl_off_t max,
                        पूर्णाङ्क base) /* 8, 10 or 16, nothing else */
{
  curl_off_t अङ्क = 0;
  अचर वर्ण *p;
  पूर्णाङ्क m = (base == 10) ? '9' :   /* the largest digit possible */
    (base == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((base == 8) || (base == 10) || (base == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  यदि(!valid_digit(*p, m))
    फर्काऊ STRE_NO_अङ्क;
  यदि(max < base) {
    /* special-case low max scenario because check needs to be different */
    गर {
      पूर्णाङ्क n = curlx_hexval(*p++);
      अङ्क = (अङ्क * base) + n;
      यदि(अङ्क > max)
        फर्काऊ STRE_OVERFLOW;
    } जबसम्म(valid_digit(*p, m));
  }
  अन्यथा {
    curl_off_t cutoff = (max - (base - 1)) / base;
    जबसम्म(valid_digit(*p, m)) {
      यदि(अङ्क <= cutoff)
        अङ्क = (अङ्क * base) + curlx_hexval(*p++);
      अन्यथा {
        पूर्णाङ्क n = curlx_hexval(*p++);
        यदि(अङ्क > ((max - n) / base))
          फर्काऊ STRE_OVERFLOW;
        अङ्क = (अङ्क * base) + n;
      }
    }
  }
  *nump = अङ्क;
  *linep = p;
  फर्काऊ STRE_OK;
}

/* Get an unsigned decimal number with no leading space or minus. Leading
   zeroes are accepted. return non-zero on error */
पूर्णाङ्क curlx_str_सङ्ख्या(अचर वर्ण **linep, curl_off_t *nump, curl_off_t max)
{
  फर्काऊ str_अङ्क_base(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
पूर्णाङ्क curlx_str_hex(अचर वर्ण **linep, curl_off_t *nump, curl_off_t max)
{
  फर्काऊ str_अङ्क_base(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
पूर्णाङ्क curlx_str_octal(अचर वर्ण **linep, curl_off_t *nump, curl_off_t max)
{
  फर्काऊ str_अङ्क_base(linep, nump, max, 8);
}

/*
 * Parse a positive number up to 63-bit number written in ASCII. Skip leading
 * blanks. No support for prefixes.
 */
पूर्णाङ्क curlx_str_numblanks(अचर वर्ण **str, curl_off_t *अङ्क)
{
  curlx_str_passblanks(str);
  फर्काऊ curlx_str_सङ्ख्या(str, अङ्क, CURL_OFF_T_MAX);
}

/* CR or LF
   return non-zero on error */
पूर्णाङ्क curlx_str_newline(अचर वर्ण **linep)
{
  DEBUGASSERT(linep && *linep);
  यदि(ISNEWLINE(**linep)) {
    (*linep)++;
    फर्काऊ STRE_OK; /* yessir */
  }
  फर्काऊ STRE_NEWLINE;
}

#ifndef WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
पूर्णाङ्क curlx_str_casecompare(संरचना Curl_str *str, अचर वर्ण *check)
{
  माप_प्रकार clen = check ? म_माप(check) : 0;
  फर्काऊ ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#endif

/* case sensitive string compare. Returns non-zero on match. */
पूर्णाङ्क curlx_str_cmp(संरचना Curl_str *str, अचर वर्ण *check)
{
  यदि(check) {
    माप_प्रकार clen = म_माप(check);
    फर्काऊ ((str->len == clen) && !म_भेदन(str->str, check, clen));
  }
  फर्काऊ !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
पूर्णाङ्क curlx_str_nudge(संरचना Curl_str *str, माप_प्रकार अङ्क)
{
  यदि(अङ्क <= str->len) {
    str->str += अङ्क;
    str->len -= अङ्क;
    फर्काऊ STRE_OK;
  }
  फर्काऊ STRE_OVERFLOW;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
पूर्णाङ्क curlx_str_cspn(अचर वर्ण **linep, संरचना Curl_str *out,
                   अचर वर्ण *reject)
{
  अचर वर्ण *s = *linep;
  माप_प्रकार len;
  DEBUGASSERT(linep && *linep);

  len = म_खोज(s, reject);
  यदि(len) {
    out->str = s;
    out->len = len;
    *linep = &s[len];
    फर्काऊ STRE_OK;
  }
  curlx_str_init(out);
  फर्काऊ STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
शून्य curlx_str_trimblanks(संरचना Curl_str *out)
{
  जबसम्म(out->len && ISBLANK(*out->str))
    curlx_str_nudge(out, 1);

  /* trim trailing spaces and tabs */
  जबसम्म(out->len && ISBLANK(out->str[out->len - 1]))
    out->len--;
}

/* increase the pointer until it has moved over all blanks */
शून्य curlx_str_passblanks(अचर वर्ण **linep)
{
  जबसम्म(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
