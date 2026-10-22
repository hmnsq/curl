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
#समावेश "curlx/strparse.h"

व्योम curlx_str_init(काष्ठा Curl_str *out)
{
  out->str = शून्य;
  out->len = 0;
}

व्योम curlx_str_assign(काष्ठा Curl_str *out, स्थिर अक्षर *str, माप_प्रकार len)
{
  out->str = str;
  out->len = len;
}

/* remove bytes from the end of the string, never remove more bytes than what
   the string holds! */
व्योम curlx_str_trim(काष्ठा Curl_str *out, माप_प्रकार len)
{
  DEBUGASSERT(out);
  DEBUGASSERT(out->len >= len);
  out->len -= len;
}

/* Get a word until the first DELIM or end of string. At least one byte long.
   return non-zero on error. If 'max' is zero, it will always return error. */
पूर्णांक curlx_str_until(स्थिर अक्षर **linep, काष्ठा Curl_str *out,
                    स्थिर माप_प्रकार max, अक्षर delim)
{
  स्थिर अक्षर *s;
  माप_प्रकार len = 0;
  DEBUGASSERT(linep);
  DEBUGASSERT(*linep);
  DEBUGASSERT(out);
  DEBUGASSERT(delim);
  s = *linep;

  curlx_str_init(out);
  जबतक(*s && (*s != delim)) {
    s++;
    अगर(++len > max) {
      वापस STRE_BIG;
    }
  }
  अगर(!len)
    वापस STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  वापस STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
पूर्णांक curlx_str_word(स्थिर अक्षर **linep, काष्ठा Curl_str *out, स्थिर माप_प्रकार max)
{
  वापस curlx_str_until(linep, out, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
पूर्णांक curlx_str_untilnl(स्थिर अक्षर **linep, काष्ठा Curl_str *out,
                      स्थिर माप_प्रकार max)
{
  स्थिर अक्षर *s = *linep;
  माप_प्रकार len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  जबतक(*s && !ISNEWLINE(*s)) {
    s++;
    अगर(++len > max)
      वापस STRE_BIG;
  }
  अगर(!len)
    वापस STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  वापस STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
पूर्णांक curlx_str_quotedword(स्थिर अक्षर **linep, काष्ठा Curl_str *out,
                         स्थिर माप_प्रकार max)
{
  स्थिर अक्षर *s = *linep;
  माप_प्रकार len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  अगर(*s != '\"')
    वापस STRE_BEGQUOTE;
  s++;
  जबतक(*s && (*s != '\"')) {
    अगर(*s == '\\' && s[1]) {
      s++;
      अगर(++len > max)
        वापस STRE_BIG;
    }
    s++;
    अगर(++len > max)
      वापस STRE_BIG;
  }
  अगर(*s != '\"')
    वापस STRE_ENDQUOTE;
  out->str = (*linep) + 1;
  out->len = len;
  *linep = s + 1;
  वापस STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
पूर्णांक curlx_str_single(स्थिर अक्षर **linep, अक्षर byte)
{
  DEBUGASSERT(linep && *linep);
  अगर(**linep != byte)
    वापस STRE_BYTE;
  (*linep)++; /* move over it */
  वापस STRE_OK;
}

/* Advance over a single space.
   return non-zero on error */
पूर्णांक curlx_str_singlespace(स्थिर अक्षर **linep)
{
  वापस curlx_str_single(linep, ' ');
}

/* given an ASCII character and max ascii, return TRUE if valid */
#घोषणा valid_digit(x, m) \
  (((x) >= '0') && ((x) <= (m)) && curlx_hexasciitable[(x) - '0'])

/* We use 16 for the zero index (and the necessary bitwise AND in the loop)
   to be able to have a non-zero value there to make valid_digit() able to
   use the info */
स्थिर अचिन्हित अक्षर curlx_hexasciitable[] = {
  16, 1, 2, 3, 4, 5, 6, 7, 8, 9, /* 0x30: 0 - 9 */
  0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15,        /* 0x41: A - F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15         /* 0x61: a - f */
};

/* no support for 0x prefix nor leading spaces */
अटल पूर्णांक str_अंक_base(स्थिर अक्षर **linep, curl_off_t *nump, curl_off_t max,
                        पूर्णांक base) /* 8, 10 or 16, nothing else */
{
  curl_off_t अंक = 0;
  स्थिर अक्षर *p;
  पूर्णांक m = (base == 10) ? '9' :   /* the largest digit possible */
    (base == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((base == 8) || (base == 10) || (base == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  अगर(!valid_digit(*p, m))
    वापस STRE_NO_अंक;
  अगर(max < base) {
    /* special-case low max scenario because check needs to be different */
    करो {
      पूर्णांक n = curlx_hexval(*p++);
      अंक = (अंक * base) + n;
      अगर(अंक > max)
        वापस STRE_OVERFLOW;
    } जबतक(valid_digit(*p, m));
  }
  अन्यथा {
    curl_off_t cutoff = (max - (base - 1)) / base;
    जबतक(valid_digit(*p, m)) {
      अगर(अंक <= cutoff)
        अंक = (अंक * base) + curlx_hexval(*p++);
      अन्यथा {
        पूर्णांक n = curlx_hexval(*p++);
        अगर(अंक > ((max - n) / base))
          वापस STRE_OVERFLOW;
        अंक = (अंक * base) + n;
      }
    }
  }
  *nump = अंक;
  *linep = p;
  वापस STRE_OK;
}

/* Get an unsigned decimal number with no leading space or minus. Leading
   zeroes are accepted. return non-zero on error */
पूर्णांक curlx_str_संख्या(स्थिर अक्षर **linep, curl_off_t *nump, curl_off_t max)
{
  वापस str_अंक_base(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
पूर्णांक curlx_str_hex(स्थिर अक्षर **linep, curl_off_t *nump, curl_off_t max)
{
  वापस str_अंक_base(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
पूर्णांक curlx_str_octal(स्थिर अक्षर **linep, curl_off_t *nump, curl_off_t max)
{
  वापस str_अंक_base(linep, nump, max, 8);
}

/*
 * Parse a positive number up to 63-bit number written in ASCII. Skip leading
 * blanks. No support for prefixes.
 */
पूर्णांक curlx_str_numblanks(स्थिर अक्षर **str, curl_off_t *अंक)
{
  curlx_str_passblanks(str);
  वापस curlx_str_संख्या(str, अंक, CURL_OFF_T_MAX);
}

/* CR or LF
   return non-zero on error */
पूर्णांक curlx_str_newline(स्थिर अक्षर **linep)
{
  DEBUGASSERT(linep && *linep);
  अगर(ISNEWLINE(**linep)) {
    (*linep)++;
    वापस STRE_OK; /* yessir */
  }
  वापस STRE_NEWLINE;
}

#अगर_अघोषित WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
पूर्णांक curlx_str_casecompare(काष्ठा Curl_str *str, स्थिर अक्षर *check)
{
  माप_प्रकार clen = check ? म_माप(check) : 0;
  वापस ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#पूर्ण_अगर

/* case sensitive string compare. Returns non-zero on match. */
पूर्णांक curlx_str_cmp(काष्ठा Curl_str *str, स्थिर अक्षर *check)
{
  अगर(check) {
    माप_प्रकार clen = म_माप(check);
    वापस ((str->len == clen) && !म_भेदन(str->str, check, clen));
  }
  वापस !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
पूर्णांक curlx_str_nudge(काष्ठा Curl_str *str, माप_प्रकार अंक)
{
  अगर(अंक <= str->len) {
    str->str += अंक;
    str->len -= अंक;
    वापस STRE_OK;
  }
  वापस STRE_OVERFLOW;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
पूर्णांक curlx_str_cspn(स्थिर अक्षर **linep, काष्ठा Curl_str *out,
                   स्थिर अक्षर *reject)
{
  स्थिर अक्षर *s = *linep;
  माप_प्रकार len;
  DEBUGASSERT(linep && *linep);

  len = म_खोज(s, reject);
  अगर(len) {
    out->str = s;
    out->len = len;
    *linep = &s[len];
    वापस STRE_OK;
  }
  curlx_str_init(out);
  वापस STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
व्योम curlx_str_trimblanks(काष्ठा Curl_str *out)
{
  जबतक(out->len && ISBLANK(*out->str))
    curlx_str_nudge(out, 1);

  /* trim trailing spaces and tabs */
  जबतक(out->len && ISBLANK(out->str[out->len - 1]))
    out->len--;
}

/* increase the pointer until it has moved over all blanks */
व्योम curlx_str_passblanks(स्थिर अक्षर **linep)
{
  जबतक(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
