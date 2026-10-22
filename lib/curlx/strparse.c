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

शून्य curlx_str_init(संरचना Curl_str *बहिर्गमनम्)
{
  बहिर्गमनम्->str = रिक्त;
  बहिर्गमनम्->len = 0;
}

शून्य curlx_str_नियोजय(संरचना Curl_str *बहिर्गमनम्, ध्रुव वर्ण *str, size_t len)
{
  बहिर्गमनम्->str = str;
  बहिर्गमनम्->len = len;
}

/* remove bytes from the end of the string, never remove more bytes than what
   the string holds! */
शून्य curlx_str_trim(संरचना Curl_str *बहिर्गमनम्, size_t len)
{
  DEBUGASSERT(बहिर्गमनम्);
  DEBUGASSERT(बहिर्गमनम्->len >= len);
  बहिर्गमनम्->len -= len;
}

/* Get a word until the first DELIM or end of string. At least one byte long.
   return non-zero on error. If 'max' is zero, it will always return error. */
पूर्णाङ्क curlx_str_until(ध्रुव वर्ण **linep, संरचना Curl_str *बहिर्गमनम्,
                    ध्रुव size_t max, वर्ण delim)
{
  ध्रुव वर्ण *s;
  size_t len = 0;
  DEBUGASSERT(linep);
  DEBUGASSERT(*linep);
  DEBUGASSERT(बहिर्गमनम्);
  DEBUGASSERT(delim);
  s = *linep;

  curlx_str_init(बहिर्गमनम्);
  यावत्(*s && (*s != delim)) {
    s++;
    यदि(++len > max) {
      प्रत्यागम STRE_BIG;
    }
  }
  यदि(!len)
    प्रत्यागम STRE_SHORT;
  बहिर्गमनम्->str = *linep;
  बहिर्गमनम्->len = len;
  *linep = s; /* point to the first byte after the word */
  प्रत्यागम STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
पूर्णाङ्क curlx_str_word(ध्रुव वर्ण **linep, संरचना Curl_str *बहिर्गमनम्, ध्रुव size_t max)
{
  प्रत्यागम curlx_str_until(linep, बहिर्गमनम्, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
पूर्णाङ्क curlx_str_untilnl(ध्रुव वर्ण **linep, संरचना Curl_str *बहिर्गमनम्,
                      ध्रुव size_t max)
{
  ध्रुव वर्ण *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && बहिर्गमनम् && max);

  curlx_str_init(बहिर्गमनम्);
  यावत्(*s && !ISNEWLINE(*s)) {
    s++;
    यदि(++len > max)
      प्रत्यागम STRE_BIG;
  }
  यदि(!len)
    प्रत्यागम STRE_SHORT;
  बहिर्गमनम्->str = *linep;
  बहिर्गमनम्->len = len;
  *linep = s; /* point to the first byte after the word */
  प्रत्यागम STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
पूर्णाङ्क curlx_str_quotedword(ध्रुव वर्ण **linep, संरचना Curl_str *बहिर्गमनम्,
                         ध्रुव size_t max)
{
  ध्रुव वर्ण *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && बहिर्गमनम् && max);

  curlx_str_init(बहिर्गमनम्);
  यदि(*s != '\"')
    प्रत्यागम STRE_BEGQUOTE;
  s++;
  यावत्(*s && (*s != '\"')) {
    यदि(*s == '\\' && s[1]) {
      s++;
      यदि(++len > max)
        प्रत्यागम STRE_BIG;
    }
    s++;
    यदि(++len > max)
      प्रत्यागम STRE_BIG;
  }
  यदि(*s != '\"')
    प्रत्यागम STRE_ENDQUOTE;
  बहिर्गमनम्->str = (*linep) + 1;
  बहिर्गमनम्->len = len;
  *linep = s + 1;
  प्रत्यागम STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
पूर्णाङ्क curlx_str_single(ध्रुव वर्ण **linep, वर्ण byte)
{
  DEBUGASSERT(linep && *linep);
  यदि(**linep != byte)
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
#परिभाषय valid_digit(x, m) \
  (((x) >= '0') && ((x) <= (m)) && curlx_hexasciitable[(x) - '0'])

/* We use 16 for the zero index (and the necessary bitwise AND in the loop)
   to be able to have a non-zero value there to make valid_digit() able to
   use the info */
ध्रुव अचिह्नित वर्ण curlx_hexasciitable[] = {
  16, 1, 2, 3, 4, 5, 6, 7, 8, 9, /* 0x30: 0 - 9 */
  0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15,        /* 0x41: A - F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15         /* 0x61: a - f */
};

/* no support for 0x prefix nor leading spaces */
स्थिर पूर्णाङ्क str_अङ्कः_आधारः(ध्रुव वर्ण **linep, curl_off_t *nump, curl_off_t max,
                        पूर्णाङ्क आधारः) /* 8, 10 or 16, nothing else */
{
  curl_off_t अङ्कः = 0;
  ध्रुव वर्ण *p;
  पूर्णाङ्क m = (आधारः == 10) ? '9' :   /* the largest digit possible */
    (आधारः == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((आधारः == 8) || (आधारः == 10) || (आधारः == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  यदि(!valid_digit(*p, m))
    प्रत्यागम STRE_NO_अङ्कः;
  यदि(max < आधारः) {
    /* special-case low max scenario because check needs to be different */
    कुरु {
      पूर्णाङ्क n = curlx_hexval(*p++);
      अङ्कः = (अङ्कः * आधारः) + n;
      यदि(अङ्कः > max)
        प्रत्यागम STRE_अतिप्रवाहः;
    } यावत्(valid_digit(*p, m));
  }
  अन्यथा {
    curl_off_t cutoff = (max - (आधारः - 1)) / आधारः;
    यावत्(valid_digit(*p, m)) {
      यदि(अङ्कः <= cutoff)
        अङ्कः = (अङ्कः * आधारः) + curlx_hexval(*p++);
      अन्यथा {
        पूर्णाङ्क n = curlx_hexval(*p++);
        यदि(अङ्कः > ((max - n) / आधारः))
          प्रत्यागम STRE_अतिप्रवाहः;
        अङ्कः = (अङ्कः * आधारः) + n;
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
  प्रत्यागम str_अङ्कः_आधारः(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
पूर्णाङ्क curlx_str_hex(ध्रुव वर्ण **linep, curl_off_t *nump, curl_off_t max)
{
  प्रत्यागम str_अङ्कः_आधारः(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
पूर्णाङ्क curlx_str_octal(ध्रुव वर्ण **linep, curl_off_t *nump, curl_off_t max)
{
  प्रत्यागम str_अङ्कः_आधारः(linep, nump, max, 8);
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
  यदि(ISNEWLINE(**linep)) {
    (*linep)++;
    प्रत्यागम STRE_OK; /* yessir */
  }
  प्रत्यागम STRE_NEWLINE;
}

#यदि_अपरिभाषितम् WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
पूर्णाङ्क curlx_str_casecompare(संरचना Curl_str *str, ध्रुव वर्ण *check)
{
  size_t clen = check ? वाक्यदैर्घ्यम्(check) : 0;
  प्रत्यागम ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#यदि_अन्तः

/* case sensitive string compare. Returns non-zero on match. */
पूर्णाङ्क curlx_str_cmp(संरचना Curl_str *str, ध्रुव वर्ण *check)
{
  यदि(check) {
    size_t clen = वाक्यदैर्घ्यम्(check);
    प्रत्यागम ((str->len == clen) && !वाक्ये_तुलय(str->str, check, clen));
  }
  प्रत्यागम !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
पूर्णाङ्क curlx_str_nudge(संरचना Curl_str *str, size_t अङ्कः)
{
  यदि(अङ्कः <= str->len) {
    str->str += अङ्कः;
    str->len -= अङ्कः;
    प्रत्यागम STRE_OK;
  }
  प्रत्यागम STRE_अतिप्रवाहः;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
पूर्णाङ्क curlx_str_cspn(ध्रुव वर्ण **linep, संरचना Curl_str *बहिर्गमनम्,
                   ध्रुव वर्ण *reject)
{
  ध्रुव वर्ण *s = *linep;
  size_t len;
  DEBUGASSERT(linep && *linep);

  len = strcspn(s, reject);
  यदि(len) {
    बहिर्गमनम्->str = s;
    बहिर्गमनम्->len = len;
    *linep = &s[len];
    प्रत्यागम STRE_OK;
  }
  curlx_str_init(बहिर्गमनम्);
  प्रत्यागम STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
शून्य curlx_str_trimblanks(संरचना Curl_str *बहिर्गमनम्)
{
  यावत्(बहिर्गमनम्->len && ISBLANK(*बहिर्गमनम्->str))
    curlx_str_nudge(बहिर्गमनम्, 1);

  /* trim trailing spaces and tabs */
  यावत्(बहिर्गमनम्->len && ISBLANK(बहिर्गमनम्->str[बहिर्गमनम्->len - 1]))
    बहिर्गमनम्->len--;
}

/* increase the pointer until it has moved over all blanks */
शून्य curlx_str_passblanks(ध्रुव वर्ण **linep)
{
  यावत्(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
