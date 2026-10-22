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
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

ਖਾਲੀ curlx_str_init(ਬਣਤਰ Curl_str *ਬਾਹਰਲਾ)
{
  ਬਾਹਰਲਾ->str = ਨਿਰਾ;
  ਬਾਹਰਲਾ->len = 0;
}

ਖਾਲੀ curlx_str_ਸੌਂਪੋ(ਬਣਤਰ Curl_str *ਬਾਹਰਲਾ, ਅਚਲ ਅੱਖਰ *str, size_t len)
{
  ਬਾਹਰਲਾ->str = str;
  ਬਾਹਰਲਾ->len = len;
}

/* remove bytes from the end of the string, never remove more bytes than what
   the string holds! */
ਖਾਲੀ curlx_str_trim(ਬਣਤਰ Curl_str *ਬਾਹਰਲਾ, size_t len)
{
  DEBUGASSERT(ਬਾਹਰਲਾ);
  DEBUGASSERT(ਬਾਹਰਲਾ->len >= len);
  ਬਾਹਰਲਾ->len -= len;
}

/* Get a word until the first DELIM or end of string. At least one byte long.
   return non-zero on error. If 'max' is zero, it will always return error. */
ਪੂਰਨ curlx_str_until(ਅਚਲ ਅੱਖਰ **linep, ਬਣਤਰ Curl_str *ਬਾਹਰਲਾ,
                    ਅਚਲ size_t max, ਅੱਖਰ delim)
{
  ਅਚਲ ਅੱਖਰ *s;
  size_t len = 0;
  DEBUGASSERT(linep);
  DEBUGASSERT(*linep);
  DEBUGASSERT(ਬਾਹਰਲਾ);
  DEBUGASSERT(delim);
  s = *linep;

  curlx_str_init(ਬਾਹਰਲਾ);
  ਜਦੋਂ(*s && (*s != delim)) {
    s++;
    ਜੇ(++len > max) {
      ਵਾਪਸ STRE_BIG;
    }
  }
  ਜੇ(!len)
    ਵਾਪਸ STRE_SHORT;
  ਬਾਹਰਲਾ->str = *linep;
  ਬਾਹਰਲਾ->len = len;
  *linep = s; /* point to the first byte after the word */
  ਵਾਪਸ STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
ਪੂਰਨ curlx_str_ਸ਼ਬਦ(ਅਚਲ ਅੱਖਰ **linep, ਬਣਤਰ Curl_str *ਬਾਹਰਲਾ, ਅਚਲ size_t max)
{
  ਵਾਪਸ curlx_str_until(linep, ਬਾਹਰਲਾ, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
ਪੂਰਨ curlx_str_untilnl(ਅਚਲ ਅੱਖਰ **linep, ਬਣਤਰ Curl_str *ਬਾਹਰਲਾ,
                      ਅਚਲ size_t max)
{
  ਅਚਲ ਅੱਖਰ *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && ਬਾਹਰਲਾ && max);

  curlx_str_init(ਬਾਹਰਲਾ);
  ਜਦੋਂ(*s && !ISNEWLINE(*s)) {
    s++;
    ਜੇ(++len > max)
      ਵਾਪਸ STRE_BIG;
  }
  ਜੇ(!len)
    ਵਾਪਸ STRE_SHORT;
  ਬਾਹਰਲਾ->str = *linep;
  ਬਾਹਰਲਾ->len = len;
  *linep = s; /* point to the first byte after the word */
  ਵਾਪਸ STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
ਪੂਰਨ curlx_str_quotedword(ਅਚਲ ਅੱਖਰ **linep, ਬਣਤਰ Curl_str *ਬਾਹਰਲਾ,
                         ਅਚਲ size_t max)
{
  ਅਚਲ ਅੱਖਰ *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && ਬਾਹਰਲਾ && max);

  curlx_str_init(ਬਾਹਰਲਾ);
  ਜੇ(*s != '\"')
    ਵਾਪਸ STRE_BEGQUOTE;
  s++;
  ਜਦੋਂ(*s && (*s != '\"')) {
    ਜੇ(*s == '\\' && s[1]) {
      s++;
      ਜੇ(++len > max)
        ਵਾਪਸ STRE_BIG;
    }
    s++;
    ਜੇ(++len > max)
      ਵਾਪਸ STRE_BIG;
  }
  ਜੇ(*s != '\"')
    ਵਾਪਸ STRE_ENDQUOTE;
  ਬਾਹਰਲਾ->str = (*linep) + 1;
  ਬਾਹਰਲਾ->len = len;
  *linep = s + 1;
  ਵਾਪਸ STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
ਪੂਰਨ curlx_str_single(ਅਚਲ ਅੱਖਰ **linep, ਅੱਖਰ byte)
{
  DEBUGASSERT(linep && *linep);
  ਜੇ(**linep != byte)
    ਵਾਪਸ STRE_BYTE;
  (*linep)++; /* move over it */
  ਵਾਪਸ STRE_OK;
}

/* Advance over a single space.
   return non-zero on error */
ਪੂਰਨ curlx_str_singlespace(ਅਚਲ ਅੱਖਰ **linep)
{
  ਵਾਪਸ curlx_str_single(linep, ' ');
}

/* given an ASCII character and max ascii, return TRUE if valid */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ valid_digit(x, m) \
  (((x) >= '0') && ((x) <= (m)) && curlx_hexasciitable[(x) - '0'])

/* We use 16 for the zero index (and the necessary bitwise AND in the loop)
   to be able to have a non-zero value there to make valid_digit() able to
   use the info */
ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ curlx_hexasciitable[] = {
  16, 1, 2, 3, 4, 5, 6, 7, 8, 9, /* 0x30: 0 - 9 */
  0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15,        /* 0x41: A - F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15         /* 0x61: a - f */
};

/* no support for 0x prefix nor leading spaces */
ਸਥਿਰ ਪੂਰਨ str_ਅੰਕ_ਅਧਾਰ(ਅਚਲ ਅੱਖਰ **linep, curl_off_t *nump, curl_off_t max,
                        ਪੂਰਨ ਅਧਾਰ) /* 8, 10 or 16, nothing else */
{
  curl_off_t ਅੰਕ = 0;
  ਅਚਲ ਅੱਖਰ *p;
  ਪੂਰਨ m = (ਅਧਾਰ == 10) ? '9' :   /* the largest digit possible */
    (ਅਧਾਰ == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((ਅਧਾਰ == 8) || (ਅਧਾਰ == 10) || (ਅਧਾਰ == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  ਜੇ(!valid_digit(*p, m))
    ਵਾਪਸ STRE_NO_ਅੰਕ;
  ਜੇ(max < ਅਧਾਰ) {
    /* special-case low max scenario because check needs to be different */
    ਕਰੋ {
      ਪੂਰਨ n = curlx_hexval(*p++);
      ਅੰਕ = (ਅੰਕ * ਅਧਾਰ) + n;
      ਜੇ(ਅੰਕ > max)
        ਵਾਪਸ STRE_ਉਛਾਲ;
    } ਜਦੋਂ(valid_digit(*p, m));
  }
  ਨਹੀਂਤਾਂ {
    curl_off_t cutoff = (max - (ਅਧਾਰ - 1)) / ਅਧਾਰ;
    ਜਦੋਂ(valid_digit(*p, m)) {
      ਜੇ(ਅੰਕ <= cutoff)
        ਅੰਕ = (ਅੰਕ * ਅਧਾਰ) + curlx_hexval(*p++);
      ਨਹੀਂਤਾਂ {
        ਪੂਰਨ n = curlx_hexval(*p++);
        ਜੇ(ਅੰਕ > ((max - n) / ਅਧਾਰ))
          ਵਾਪਸ STRE_ਉਛਾਲ;
        ਅੰਕ = (ਅੰਕ * ਅਧਾਰ) + n;
      }
    }
  }
  *nump = ਅੰਕ;
  *linep = p;
  ਵਾਪਸ STRE_OK;
}

/* Get an unsigned decimal number with no leading space or minus. Leading
   zeroes are accepted. return non-zero on error */
ਪੂਰਨ curlx_str_ਸੰਖਿਆ(ਅਚਲ ਅੱਖਰ **linep, curl_off_t *nump, curl_off_t max)
{
  ਵਾਪਸ str_ਅੰਕ_ਅਧਾਰ(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
ਪੂਰਨ curlx_str_hex(ਅਚਲ ਅੱਖਰ **linep, curl_off_t *nump, curl_off_t max)
{
  ਵਾਪਸ str_ਅੰਕ_ਅਧਾਰ(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
ਪੂਰਨ curlx_str_octal(ਅਚਲ ਅੱਖਰ **linep, curl_off_t *nump, curl_off_t max)
{
  ਵਾਪਸ str_ਅੰਕ_ਅਧਾਰ(linep, nump, max, 8);
}

/*
 * Parse a positive number up to 63-bit number written in ASCII. Skip leading
 * blanks. No support for prefixes.
 */
ਪੂਰਨ curlx_str_numblanks(ਅਚਲ ਅੱਖਰ **str, curl_off_t *ਅੰਕ)
{
  curlx_str_passblanks(str);
  ਵਾਪਸ curlx_str_ਸੰਖਿਆ(str, ਅੰਕ, CURL_OFF_T_MAX);
}

/* CR or LF
   return non-zero on error */
ਪੂਰਨ curlx_str_newline(ਅਚਲ ਅੱਖਰ **linep)
{
  DEBUGASSERT(linep && *linep);
  ਜੇ(ISNEWLINE(**linep)) {
    (*linep)++;
    ਵਾਪਸ STRE_OK; /* yessir */
  }
  ਵਾਪਸ STRE_NEWLINE;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
ਪੂਰਨ curlx_str_casecompare(ਬਣਤਰ Curl_str *str, ਅਚਲ ਅੱਖਰ *check)
{
  size_t clen = check ? ਲੜੀ_ਲੰਬਾਈ(check) : 0;
  ਵਾਪਸ ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#ਜੇ_ਅੰਤ

/* case sensitive string compare. Returns non-zero on match. */
ਪੂਰਨ curlx_str_cmp(ਬਣਤਰ Curl_str *str, ਅਚਲ ਅੱਖਰ *check)
{
  ਜੇ(check) {
    size_t clen = ਲੜੀ_ਲੰਬਾਈ(check);
    ਵਾਪਸ ((str->len == clen) && !ਲੜੀ_ਤੁਲਨਾ(str->str, check, clen));
  }
  ਵਾਪਸ !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
ਪੂਰਨ curlx_str_nudge(ਬਣਤਰ Curl_str *str, size_t ਅੰਕ)
{
  ਜੇ(ਅੰਕ <= str->len) {
    str->str += ਅੰਕ;
    str->len -= ਅੰਕ;
    ਵਾਪਸ STRE_OK;
  }
  ਵਾਪਸ STRE_ਉਛਾਲ;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
ਪੂਰਨ curlx_str_cspn(ਅਚਲ ਅੱਖਰ **linep, ਬਣਤਰ Curl_str *ਬਾਹਰਲਾ,
                   ਅਚਲ ਅੱਖਰ *reject)
{
  ਅਚਲ ਅੱਖਰ *s = *linep;
  size_t len;
  DEBUGASSERT(linep && *linep);

  len = strcspn(s, reject);
  ਜੇ(len) {
    ਬਾਹਰਲਾ->str = s;
    ਬਾਹਰਲਾ->len = len;
    *linep = &s[len];
    ਵਾਪਸ STRE_OK;
  }
  curlx_str_init(ਬਾਹਰਲਾ);
  ਵਾਪਸ STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
ਖਾਲੀ curlx_str_trimblanks(ਬਣਤਰ Curl_str *ਬਾਹਰਲਾ)
{
  ਜਦੋਂ(ਬਾਹਰਲਾ->len && ISBLANK(*ਬਾਹਰਲਾ->str))
    curlx_str_nudge(ਬਾਹਰਲਾ, 1);

  /* trim trailing spaces and tabs */
  ਜਦੋਂ(ਬਾਹਰਲਾ->len && ISBLANK(ਬਾਹਰਲਾ->str[ਬਾਹਰਲਾ->len - 1]))
    ਬਾਹਰਲਾ->len--;
}

/* increase the pointer until it has moved over all blanks */
ਖਾਲੀ curlx_str_passblanks(ਅਚਲ ਅੱਖਰ **linep)
{
  ਜਦੋਂ(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
