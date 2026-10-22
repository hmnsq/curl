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
#ضمن "curlx/strparse.h"

فراغ curlx_str_init(بنية Curl_str *بیرون)
{
  بیرون->str = عدم;
  بیرون->len = 0;
}

فراغ curlx_str_تخصیص_بده(بنية Curl_str *بیرون, ثابت حرف *str, size_t len)
{
  بیرون->str = str;
  بیرون->len = len;
}

/* remove bytes from the end of the string, never remove more bytes than what
   the string holds! */
فراغ curlx_str_trim(بنية Curl_str *بیرون, size_t len)
{
  DEBUGASSERT(بیرون);
  DEBUGASSERT(بیرون->len >= len);
  بیرون->len -= len;
}

/* Get a word until the first DELIM or end of string. At least one byte long.
   return non-zero on error. If 'max' is zero, it will always return error. */
صحيح curlx_str_until(ثابت حرف **linep, بنية Curl_str *بیرون,
                    ثابت size_t max, حرف delim)
{
  ثابت حرف *s;
  size_t len = 0;
  DEBUGASSERT(linep);
  DEBUGASSERT(*linep);
  DEBUGASSERT(بیرون);
  DEBUGASSERT(delim);
  s = *linep;

  curlx_str_init(بیرون);
  طالما(*s && (*s != delim)) {
    s++;
    إذا(++len > max) {
      أرجع STRE_BIG;
    }
  }
  إذا(!len)
    أرجع STRE_SHORT;
  بیرون->str = *linep;
  بیرون->len = len;
  *linep = s; /* point to the first byte after the word */
  أرجع STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
صحيح curlx_str_کلمه(ثابت حرف **linep, بنية Curl_str *بیرون, ثابت size_t max)
{
  أرجع curlx_str_until(linep, بیرون, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
صحيح curlx_str_untilnl(ثابت حرف **linep, بنية Curl_str *بیرون,
                      ثابت size_t max)
{
  ثابت حرف *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && بیرون && max);

  curlx_str_init(بیرون);
  طالما(*s && !ISNEWLINE(*s)) {
    s++;
    إذا(++len > max)
      أرجع STRE_BIG;
  }
  إذا(!len)
    أرجع STRE_SHORT;
  بیرون->str = *linep;
  بیرون->len = len;
  *linep = s; /* point to the first byte after the word */
  أرجع STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
صحيح curlx_str_quotedword(ثابت حرف **linep, بنية Curl_str *بیرون,
                         ثابت size_t max)
{
  ثابت حرف *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && بیرون && max);

  curlx_str_init(بیرون);
  إذا(*s != '\"')
    أرجع STRE_BEGQUOTE;
  s++;
  طالما(*s && (*s != '\"')) {
    إذا(*s == '\\' && s[1]) {
      s++;
      إذا(++len > max)
        أرجع STRE_BIG;
    }
    s++;
    إذا(++len > max)
      أرجع STRE_BIG;
  }
  إذا(*s != '\"')
    أرجع STRE_ENDQUOTE;
  بیرون->str = (*linep) + 1;
  بیرون->len = len;
  *linep = s + 1;
  أرجع STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
صحيح curlx_str_single(ثابت حرف **linep, حرف byte)
{
  DEBUGASSERT(linep && *linep);
  إذا(**linep != byte)
    أرجع STRE_BYTE;
  (*linep)++; /* move over it */
  أرجع STRE_OK;
}

/* Advance over a single space.
   return non-zero on error */
صحيح curlx_str_singlespace(ثابت حرف **linep)
{
  أرجع curlx_str_single(linep, ' ');
}

/* given an ASCII character and max ascii, return TRUE if valid */
#تعریف_کن valid_digit(x, m) \
  (((x) >= '0') && ((x) <= (m)) && curlx_hexasciitable[(x) - '0'])

/* We use 16 for the zero index (and the necessary bitwise AND in the loop)
   to be able to have a non-zero value there to make valid_digit() able to
   use the info */
ثابت بیعلامت حرف curlx_hexasciitable[] = {
  16, 1, 2, 3, 4, 5, 6, 7, 8, 9, /* 0x30: 0 - 9 */
  0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15,        /* 0x41: A - F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15         /* 0x61: a - f */
};

/* no support for 0x prefix nor leading spaces */
ساكن صحيح str_رقم_پایه(ثابت حرف **linep, curl_off_t *nump, curl_off_t max,
                        صحيح پایه) /* 8, 10 or 16, nothing else */
{
  curl_off_t رقم = 0;
  ثابت حرف *p;
  صحيح m = (پایه == 10) ? '9' :   /* the largest digit possible */
    (پایه == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((پایه == 8) || (پایه == 10) || (پایه == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  إذا(!valid_digit(*p, m))
    أرجع STRE_NO_رقم;
  إذا(max < پایه) {
    /* special-case low max scenario because check needs to be different */
    افعل {
      صحيح n = curlx_hexval(*p++);
      رقم = (رقم * پایه) + n;
      إذا(رقم > max)
        أرجع STRE_سرریز;
    } طالما(valid_digit(*p, m));
  }
  وإلا {
    curl_off_t cutoff = (max - (پایه - 1)) / پایه;
    طالما(valid_digit(*p, m)) {
      إذا(رقم <= cutoff)
        رقم = (رقم * پایه) + curlx_hexval(*p++);
      وإلا {
        صحيح n = curlx_hexval(*p++);
        إذا(رقم > ((max - n) / پایه))
          أرجع STRE_سرریز;
        رقم = (رقم * پایه) + n;
      }
    }
  }
  *nump = رقم;
  *linep = p;
  أرجع STRE_OK;
}

/* Get an unsigned decimal number with no leading space or minus. Leading
   zeroes are accepted. return non-zero on error */
صحيح curlx_str_عدد(ثابت حرف **linep, curl_off_t *nump, curl_off_t max)
{
  أرجع str_رقم_پایه(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
صحيح curlx_str_hex(ثابت حرف **linep, curl_off_t *nump, curl_off_t max)
{
  أرجع str_رقم_پایه(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
صحيح curlx_str_octal(ثابت حرف **linep, curl_off_t *nump, curl_off_t max)
{
  أرجع str_رقم_پایه(linep, nump, max, 8);
}

/*
 * Parse a positive number up to 63-bit number written in ASCII. Skip leading
 * blanks. No support for prefixes.
 */
صحيح curlx_str_numblanks(ثابت حرف **str, curl_off_t *رقم)
{
  curlx_str_passblanks(str);
  أرجع curlx_str_عدد(str, رقم, CURL_OFF_T_MAX);
}

/* CR or LF
   return non-zero on error */
صحيح curlx_str_newline(ثابت حرف **linep)
{
  DEBUGASSERT(linep && *linep);
  إذا(ISNEWLINE(**linep)) {
    (*linep)++;
    أرجع STRE_OK; /* yessir */
  }
  أرجع STRE_NEWLINE;
}

#اگر_تعریف_نشده WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
صحيح curlx_str_casecompare(بنية Curl_str *str, ثابت حرف *check)
{
  size_t clen = check ? طول_رشته(check) : 0;
  أرجع ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#پایان_اگر

/* case sensitive string compare. Returns non-zero on match. */
صحيح curlx_str_cmp(بنية Curl_str *str, ثابت حرف *check)
{
  إذا(check) {
    size_t clen = طول_رشته(check);
    أرجع ((str->len == clen) && !مقایسه_رشته(str->str, check, clen));
  }
  أرجع !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
صحيح curlx_str_nudge(بنية Curl_str *str, size_t رقم)
{
  إذا(رقم <= str->len) {
    str->str += رقم;
    str->len -= رقم;
    أرجع STRE_OK;
  }
  أرجع STRE_سرریز;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
صحيح curlx_str_cspn(ثابت حرف **linep, بنية Curl_str *بیرون,
                   ثابت حرف *reject)
{
  ثابت حرف *s = *linep;
  size_t len;
  DEBUGASSERT(linep && *linep);

  len = strcspn(s, reject);
  إذا(len) {
    بیرون->str = s;
    بیرون->len = len;
    *linep = &s[len];
    أرجع STRE_OK;
  }
  curlx_str_init(بیرون);
  أرجع STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
فراغ curlx_str_trimblanks(بنية Curl_str *بیرون)
{
  طالما(بیرون->len && ISBLANK(*بیرون->str))
    curlx_str_nudge(بیرون, 1);

  /* trim trailing spaces and tabs */
  طالما(بیرون->len && ISBLANK(بیرون->str[بیرون->len - 1]))
    بیرون->len--;
}

/* increase the pointer until it has moved over all blanks */
فراغ curlx_str_passblanks(ثابت حرف **linep)
{
  طالما(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
