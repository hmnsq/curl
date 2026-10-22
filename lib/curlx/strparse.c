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

خالی curlx_str_init(ساخت Curl_str *برآمد)
{
  برآمد->str = تہی;
  برآمد->len = 0;
}

خالی curlx_str_تفویض(ساخت Curl_str *برآمد, ثابت حرف *str, size_t len)
{
  برآمد->str = str;
  برآمد->len = len;
}

/* remove bytes from the end of the string, never remove more bytes than what
   the string holds! */
خالی curlx_str_trim(ساخت Curl_str *برآمد, size_t len)
{
  DEBUGASSERT(برآمد);
  DEBUGASSERT(برآمد->len >= len);
  برآمد->len -= len;
}

/* Get a word until the first DELIM or end of string. At least one byte long.
   return non-zero on error. If 'max' is zero, it will always return error. */
صحیح curlx_str_until(ثابت حرف **linep, ساخت Curl_str *برآمد,
                    ثابت size_t max, حرف delim)
{
  ثابت حرف *s;
  size_t len = 0;
  DEBUGASSERT(linep);
  DEBUGASSERT(*linep);
  DEBUGASSERT(برآمد);
  DEBUGASSERT(delim);
  s = *linep;

  curlx_str_init(برآمد);
  جب(*s && (*s != delim)) {
    s++;
    اگر(++len > max) {
      واپس STRE_BIG;
    }
  }
  اگر(!len)
    واپس STRE_SHORT;
  برآمد->str = *linep;
  برآمد->len = len;
  *linep = s; /* point to the first byte after the word */
  واپس STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
صحیح curlx_str_لفظ(ثابت حرف **linep, ساخت Curl_str *برآمد, ثابت size_t max)
{
  واپس curlx_str_until(linep, برآمد, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
صحیح curlx_str_untilnl(ثابت حرف **linep, ساخت Curl_str *برآمد,
                      ثابت size_t max)
{
  ثابت حرف *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && برآمد && max);

  curlx_str_init(برآمد);
  جب(*s && !ISNEWLINE(*s)) {
    s++;
    اگر(++len > max)
      واپس STRE_BIG;
  }
  اگر(!len)
    واپس STRE_SHORT;
  برآمد->str = *linep;
  برآمد->len = len;
  *linep = s; /* point to the first byte after the word */
  واپس STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
صحیح curlx_str_quotedword(ثابت حرف **linep, ساخت Curl_str *برآمد,
                         ثابت size_t max)
{
  ثابت حرف *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && برآمد && max);

  curlx_str_init(برآمد);
  اگر(*s != '\"')
    واپس STRE_BEGQUOTE;
  s++;
  جب(*s && (*s != '\"')) {
    اگر(*s == '\\' && s[1]) {
      s++;
      اگر(++len > max)
        واپس STRE_BIG;
    }
    s++;
    اگر(++len > max)
      واپس STRE_BIG;
  }
  اگر(*s != '\"')
    واپس STRE_ENDQUOTE;
  برآمد->str = (*linep) + 1;
  برآمد->len = len;
  *linep = s + 1;
  واپس STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
صحیح curlx_str_single(ثابت حرف **linep, حرف byte)
{
  DEBUGASSERT(linep && *linep);
  اگر(**linep != byte)
    واپس STRE_BYTE;
  (*linep)++; /* move over it */
  واپس STRE_OK;
}

/* Advance over a single space.
   return non-zero on error */
صحیح curlx_str_singlespace(ثابت حرف **linep)
{
  واپس curlx_str_single(linep, ' ');
}

/* given an ASCII character and max ascii, return TRUE if valid */
#تعریف_کرو valid_digit(x, m) \
  (((x) >= '0') && ((x) <= (m)) && curlx_hexasciitable[(x) - '0'])

/* We use 16 for the zero index (and the necessary bitwise AND in the loop)
   to be able to have a non-zero value there to make valid_digit() able to
   use the info */
ثابت غیرعلامتی حرف curlx_hexasciitable[] = {
  16, 1, 2, 3, 4, 5, 6, 7, 8, 9, /* 0x30: 0 - 9 */
  0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15,        /* 0x41: A - F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15         /* 0x61: a - f */
};

/* no support for 0x prefix nor leading spaces */
ساکن صحیح str_عدد_بنیاد(ثابت حرف **linep, curl_off_t *nump, curl_off_t max,
                        صحیح بنیاد) /* 8, 10 or 16, nothing else */
{
  curl_off_t عدد = 0;
  ثابت حرف *p;
  صحیح m = (بنیاد == 10) ? '9' :   /* the largest digit possible */
    (بنیاد == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((بنیاد == 8) || (بنیاد == 10) || (بنیاد == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  اگر(!valid_digit(*p, m))
    واپس STRE_NO_عدد;
  اگر(max < بنیاد) {
    /* special-case low max scenario because check needs to be different */
    کرو {
      صحیح n = curlx_hexval(*p++);
      عدد = (عدد * بنیاد) + n;
      اگر(عدد > max)
        واپس STRE_حد_سے_تجاوز;
    } جب(valid_digit(*p, m));
  }
  ورنہ {
    curl_off_t cutoff = (max - (بنیاد - 1)) / بنیاد;
    جب(valid_digit(*p, m)) {
      اگر(عدد <= cutoff)
        عدد = (عدد * بنیاد) + curlx_hexval(*p++);
      ورنہ {
        صحیح n = curlx_hexval(*p++);
        اگر(عدد > ((max - n) / بنیاد))
          واپس STRE_حد_سے_تجاوز;
        عدد = (عدد * بنیاد) + n;
      }
    }
  }
  *nump = عدد;
  *linep = p;
  واپس STRE_OK;
}

/* Get an unsigned decimal number with no leading space or minus. Leading
   zeroes are accepted. return non-zero on error */
صحیح curlx_str_نمبر(ثابت حرف **linep, curl_off_t *nump, curl_off_t max)
{
  واپس str_عدد_بنیاد(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
صحیح curlx_str_hex(ثابت حرف **linep, curl_off_t *nump, curl_off_t max)
{
  واپس str_عدد_بنیاد(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
صحیح curlx_str_octal(ثابت حرف **linep, curl_off_t *nump, curl_off_t max)
{
  واپس str_عدد_بنیاد(linep, nump, max, 8);
}

/*
 * Parse a positive number up to 63-bit number written in ASCII. Skip leading
 * blanks. No support for prefixes.
 */
صحیح curlx_str_numblanks(ثابت حرف **str, curl_off_t *عدد)
{
  curlx_str_passblanks(str);
  واپس curlx_str_نمبر(str, عدد, CURL_OFF_T_MAX);
}

/* CR or LF
   return non-zero on error */
صحیح curlx_str_newline(ثابت حرف **linep)
{
  DEBUGASSERT(linep && *linep);
  اگر(ISNEWLINE(**linep)) {
    (*linep)++;
    واپس STRE_OK; /* yessir */
  }
  واپس STRE_NEWLINE;
}

#اگر_غیر_تعریف_شدہ WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
صحیح curlx_str_casecompare(ساخت Curl_str *str, ثابت حرف *check)
{
  size_t clen = check ? لڑی_لمبائی(check) : 0;
  واپس ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#اگر_ختم

/* case sensitive string compare. Returns non-zero on match. */
صحیح curlx_str_cmp(ساخت Curl_str *str, ثابت حرف *check)
{
  اگر(check) {
    size_t clen = لڑی_لمبائی(check);
    واپس ((str->len == clen) && !لڑی_موازنہ(str->str, check, clen));
  }
  واپس !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
صحیح curlx_str_nudge(ساخت Curl_str *str, size_t عدد)
{
  اگر(عدد <= str->len) {
    str->str += عدد;
    str->len -= عدد;
    واپس STRE_OK;
  }
  واپس STRE_حد_سے_تجاوز;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
صحیح curlx_str_cspn(ثابت حرف **linep, ساخت Curl_str *برآمد,
                   ثابت حرف *reject)
{
  ثابت حرف *s = *linep;
  size_t len;
  DEBUGASSERT(linep && *linep);

  len = strcspn(s, reject);
  اگر(len) {
    برآمد->str = s;
    برآمد->len = len;
    *linep = &s[len];
    واپس STRE_OK;
  }
  curlx_str_init(برآمد);
  واپس STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
خالی curlx_str_trimblanks(ساخت Curl_str *برآمد)
{
  جب(برآمد->len && ISBLANK(*برآمد->str))
    curlx_str_nudge(برآمد, 1);

  /* trim trailing spaces and tabs */
  جب(برآمد->len && ISBLANK(برآمد->str[برآمد->len - 1]))
    برآمد->len--;
}

/* increase the pointer until it has moved over all blanks */
خالی curlx_str_passblanks(ثابت حرف **linep)
{
  جب(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
