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
#شامل "curlx/strparse.h"

تهی curlx_str_init(ساختار Curl_str *out)
{
  out->str = پوچ;
  out->len = 0;
}

تهی curlx_str_assign(ساختار Curl_str *out, ثابت نویسه *str, size_t len)
{
  out->str = str;
  out->len = len;
}

/* remove bytes from the end of the string, never remove more bytes than what
   the string holds! */
تهی curlx_str_trim(ساختار Curl_str *out, size_t len)
{
  DEBUGASSERT(out);
  DEBUGASSERT(out->len >= len);
  out->len -= len;
}

/* Get a word until the first DELIM or end of string. At least one byte long.
   return non-zero on error. If 'max' is zero, it will always return error. */
صحیح curlx_str_until(ثابت نویسه **linep, ساختار Curl_str *out,
                    ثابت size_t max, نویسه delim)
{
  ثابت نویسه *s;
  size_t len = 0;
  DEBUGASSERT(linep);
  DEBUGASSERT(*linep);
  DEBUGASSERT(out);
  DEBUGASSERT(delim);
  s = *linep;

  curlx_str_init(out);
  وقتی(*s && (*s != delim)) {
    s++;
    اگر(++len > max) {
      بازگشت STRE_BIG;
    }
  }
  اگر(!len)
    بازگشت STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  بازگشت STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
صحیح curlx_str_word(ثابت نویسه **linep, ساختار Curl_str *out, ثابت size_t max)
{
  بازگشت curlx_str_until(linep, out, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
صحیح curlx_str_untilnl(ثابت نویسه **linep, ساختار Curl_str *out,
                      ثابت size_t max)
{
  ثابت نویسه *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  وقتی(*s && !ISNEWLINE(*s)) {
    s++;
    اگر(++len > max)
      بازگشت STRE_BIG;
  }
  اگر(!len)
    بازگشت STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  بازگشت STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
صحیح curlx_str_quotedword(ثابت نویسه **linep, ساختار Curl_str *out,
                         ثابت size_t max)
{
  ثابت نویسه *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  اگر(*s != '\"')
    بازگشت STRE_BEGQUOTE;
  s++;
  وقتی(*s && (*s != '\"')) {
    اگر(*s == '\\' && s[1]) {
      s++;
      اگر(++len > max)
        بازگشت STRE_BIG;
    }
    s++;
    اگر(++len > max)
      بازگشت STRE_BIG;
  }
  اگر(*s != '\"')
    بازگشت STRE_ENDQUOTE;
  out->str = (*linep) + 1;
  out->len = len;
  *linep = s + 1;
  بازگشت STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
صحیح curlx_str_single(ثابت نویسه **linep, نویسه byte)
{
  DEBUGASSERT(linep && *linep);
  اگر(**linep != byte)
    بازگشت STRE_BYTE;
  (*linep)++; /* move over it */
  بازگشت STRE_OK;
}

/* Advance over a single space.
   return non-zero on error */
صحیح curlx_str_singlespace(ثابت نویسه **linep)
{
  بازگشت curlx_str_single(linep, ' ');
}

/* given an ASCII character and max ascii, return TRUE if valid */
#define valid_digit(x, m) \
  (((x) >= '0') && ((x) <= (m)) && curlx_hexasciitable[(x) - '0'])

/* We use 16 for the zero index (and the necessary bitwise AND in the loop)
   to be able to have a non-zero value there to make valid_digit() able to
   use the info */
ثابت unsigned نویسه curlx_hexasciitable[] = {
  16, 1, 2, 3, 4, 5, 6, 7, 8, 9, /* 0x30: 0 - 9 */
  0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15,        /* 0x41: A - F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15         /* 0x61: a - f */
};

/* no support for 0x prefix nor leading spaces */
ایستا صحیح str_رقم_base(ثابت نویسه **linep, curl_off_t *nump, curl_off_t max,
                        صحیح base) /* 8, 10 or 16, nothing else */
{
  curl_off_t رقم = 0;
  ثابت نویسه *p;
  صحیح m = (base == 10) ? '9' :   /* the largest digit possible */
    (base == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((base == 8) || (base == 10) || (base == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  اگر(!valid_digit(*p, m))
    بازگشت STRE_NO_رقم;
  اگر(max < base) {
    /* special-case low max scenario because check needs to be different */
    انجامبده {
      صحیح n = curlx_hexval(*p++);
      رقم = (رقم * base) + n;
      اگر(رقم > max)
        بازگشت STRE_OVERFLOW;
    } وقتی(valid_digit(*p, m));
  }
  وگرنه {
    curl_off_t cutoff = (max - (base - 1)) / base;
    وقتی(valid_digit(*p, m)) {
      اگر(رقم <= cutoff)
        رقم = (رقم * base) + curlx_hexval(*p++);
      وگرنه {
        صحیح n = curlx_hexval(*p++);
        اگر(رقم > ((max - n) / base))
          بازگشت STRE_OVERFLOW;
        رقم = (رقم * base) + n;
      }
    }
  }
  *nump = رقم;
  *linep = p;
  بازگشت STRE_OK;
}

/* Get an unsigned decimal number with no leading space or minus. Leading
   zeroes are accepted. return non-zero on error */
صحیح curlx_str_عدد(ثابت نویسه **linep, curl_off_t *nump, curl_off_t max)
{
  بازگشت str_رقم_base(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
صحیح curlx_str_hex(ثابت نویسه **linep, curl_off_t *nump, curl_off_t max)
{
  بازگشت str_رقم_base(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
صحیح curlx_str_octal(ثابت نویسه **linep, curl_off_t *nump, curl_off_t max)
{
  بازگشت str_رقم_base(linep, nump, max, 8);
}

/*
 * Parse a positive number up to 63-bit number written in ASCII. Skip leading
 * blanks. No support for prefixes.
 */
صحیح curlx_str_numblanks(ثابت نویسه **str, curl_off_t *رقم)
{
  curlx_str_passblanks(str);
  بازگشت curlx_str_عدد(str, رقم, CURL_OFF_T_MAX);
}

/* CR or LF
   return non-zero on error */
صحیح curlx_str_newline(ثابت نویسه **linep)
{
  DEBUGASSERT(linep && *linep);
  اگر(ISNEWLINE(**linep)) {
    (*linep)++;
    بازگشت STRE_OK; /* yessir */
  }
  بازگشت STRE_NEWLINE;
}

#ifndef WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
صحیح curlx_str_casecompare(ساختار Curl_str *str, ثابت نویسه *check)
{
  size_t clen = check ? strlen(check) : 0;
  بازگشت ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#endif

/* case sensitive string compare. Returns non-zero on match. */
صحیح curlx_str_cmp(ساختار Curl_str *str, ثابت نویسه *check)
{
  اگر(check) {
    size_t clen = strlen(check);
    بازگشت ((str->len == clen) && !strncmp(str->str, check, clen));
  }
  بازگشت !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
صحیح curlx_str_nudge(ساختار Curl_str *str, size_t رقم)
{
  اگر(رقم <= str->len) {
    str->str += رقم;
    str->len -= رقم;
    بازگشت STRE_OK;
  }
  بازگشت STRE_OVERFLOW;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
صحیح curlx_str_cspn(ثابت نویسه **linep, ساختار Curl_str *out,
                   ثابت نویسه *reject)
{
  ثابت نویسه *s = *linep;
  size_t len;
  DEBUGASSERT(linep && *linep);

  len = strcspn(s, reject);
  اگر(len) {
    out->str = s;
    out->len = len;
    *linep = &s[len];
    بازگشت STRE_OK;
  }
  curlx_str_init(out);
  بازگشت STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
تهی curlx_str_trimblanks(ساختار Curl_str *out)
{
  وقتی(out->len && ISBLANK(*out->str))
    curlx_str_nudge(out, 1);

  /* trim trailing spaces and tabs */
  وقتی(out->len && ISBLANK(out->str[out->len - 1]))
    out->len--;
}

/* increase the pointer until it has moved over all blanks */
تهی curlx_str_passblanks(ثابت نویسه **linep)
{
  وقتی(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
