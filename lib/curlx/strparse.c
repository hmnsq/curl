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
#അന്തര്ഭാവ "curlx/strparse.h"

ശൂന്യം curlx_str_init(ഘടന Curl_str *out)
{
  out->str = ശൂന്യ;
  out->len = 0;
}

ശൂന്യം curlx_str_assign(ഘടന Curl_str *out, സ്ഥിരം അക്ഷരം *str, size_t len)
{
  out->str = str;
  out->len = len;
}

/* remove bytes from the end of the string, never remove more bytes than what
   the string holds! */
ശൂന്യം curlx_str_trim(ഘടന Curl_str *out, size_t len)
{
  DEBUGASSERT(out);
  DEBUGASSERT(out->len >= len);
  out->len -= len;
}

/* Get a word until the first DELIM or end of string. At least one byte long.
   return non-zero on error. If 'max' is zero, it will always return error. */
പൂർണ്ണസംഖ്യ curlx_str_until(സ്ഥിരം അക്ഷരം **linep, ഘടന Curl_str *out,
                    സ്ഥിരം size_t max, അക്ഷരം delim)
{
  സ്ഥിരം അക്ഷരം *s;
  size_t len = 0;
  DEBUGASSERT(linep);
  DEBUGASSERT(*linep);
  DEBUGASSERT(out);
  DEBUGASSERT(delim);
  s = *linep;

  curlx_str_init(out);
  ആകുമ്പോൾ(*s && (*s != delim)) {
    s++;
    എങ്കിൽ(++len > max) {
      തിരികെ STRE_BIG;
    }
  }
  എങ്കിൽ(!len)
    തിരികെ STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  തിരികെ STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
പൂർണ്ണസംഖ്യ curlx_str_word(സ്ഥിരം അക്ഷരം **linep, ഘടന Curl_str *out, സ്ഥിരം size_t max)
{
  തിരികെ curlx_str_until(linep, out, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
പൂർണ്ണസംഖ്യ curlx_str_untilnl(സ്ഥിരം അക്ഷരം **linep, ഘടന Curl_str *out,
                      സ്ഥിരം size_t max)
{
  സ്ഥിരം അക്ഷരം *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  ആകുമ്പോൾ(*s && !ISNEWLINE(*s)) {
    s++;
    എങ്കിൽ(++len > max)
      തിരികെ STRE_BIG;
  }
  എങ്കിൽ(!len)
    തിരികെ STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  തിരികെ STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
പൂർണ്ണസംഖ്യ curlx_str_quotedword(സ്ഥിരം അക്ഷരം **linep, ഘടന Curl_str *out,
                         സ്ഥിരം size_t max)
{
  സ്ഥിരം അക്ഷരം *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  എങ്കിൽ(*s != '\"')
    തിരികെ STRE_BEGQUOTE;
  s++;
  ആകുമ്പോൾ(*s && (*s != '\"')) {
    എങ്കിൽ(*s == '\\' && s[1]) {
      s++;
      എങ്കിൽ(++len > max)
        തിരികെ STRE_BIG;
    }
    s++;
    എങ്കിൽ(++len > max)
      തിരികെ STRE_BIG;
  }
  എങ്കിൽ(*s != '\"')
    തിരികെ STRE_ENDQUOTE;
  out->str = (*linep) + 1;
  out->len = len;
  *linep = s + 1;
  തിരികെ STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
പൂർണ്ണസംഖ്യ curlx_str_single(സ്ഥിരം അക്ഷരം **linep, അക്ഷരം byte)
{
  DEBUGASSERT(linep && *linep);
  എങ്കിൽ(**linep != byte)
    തിരികെ STRE_BYTE;
  (*linep)++; /* move over it */
  തിരികെ STRE_OK;
}

/* Advance over a single space.
   return non-zero on error */
പൂർണ്ണസംഖ്യ curlx_str_singlespace(സ്ഥിരം അക്ഷരം **linep)
{
  തിരികെ curlx_str_single(linep, ' ');
}

/* given an ASCII character and max ascii, return TRUE if valid */
#define valid_digit(x, m) \
  (((x) >= '0') && ((x) <= (m)) && curlx_hexasciitable[(x) - '0'])

/* We use 16 for the zero index (and the necessary bitwise AND in the loop)
   to be able to have a non-zero value there to make valid_digit() able to
   use the info */
സ്ഥിരം unsigned അക്ഷരം curlx_hexasciitable[] = {
  16, 1, 2, 3, 4, 5, 6, 7, 8, 9, /* 0x30: 0 - 9 */
  0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15,        /* 0x41: A - F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15         /* 0x61: a - f */
};

/* no support for 0x prefix nor leading spaces */
സ്ഥിതം പൂർണ്ണസംഖ്യ str_അക്കം_base(സ്ഥിരം അക്ഷരം **linep, curl_off_t *nump, curl_off_t max,
                        പൂർണ്ണസംഖ്യ base) /* 8, 10 or 16, nothing else */
{
  curl_off_t അക്കം = 0;
  സ്ഥിരം അക്ഷരം *p;
  പൂർണ്ണസംഖ്യ m = (base == 10) ? '9' :   /* the largest digit possible */
    (base == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((base == 8) || (base == 10) || (base == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  എങ്കിൽ(!valid_digit(*p, m))
    തിരികെ STRE_NO_അക്കം;
  എങ്കിൽ(max < base) {
    /* special-case low max scenario because check needs to be different */
    ചെയ്യുക {
      പൂർണ്ണസംഖ്യ n = curlx_hexval(*p++);
      അക്കം = (അക്കം * base) + n;
      എങ്കിൽ(അക്കം > max)
        തിരികെ STRE_OVERFLOW;
    } ആകുമ്പോൾ(valid_digit(*p, m));
  }
  അല്ലെങ്കിൽ {
    curl_off_t cutoff = (max - (base - 1)) / base;
    ആകുമ്പോൾ(valid_digit(*p, m)) {
      എങ്കിൽ(അക്കം <= cutoff)
        അക്കം = (അക്കം * base) + curlx_hexval(*p++);
      അല്ലെങ്കിൽ {
        പൂർണ്ണസംഖ്യ n = curlx_hexval(*p++);
        എങ്കിൽ(അക്കം > ((max - n) / base))
          തിരികെ STRE_OVERFLOW;
        അക്കം = (അക്കം * base) + n;
      }
    }
  }
  *nump = അക്കം;
  *linep = p;
  തിരികെ STRE_OK;
}

/* Get an unsigned decimal number with no leading space or minus. Leading
   zeroes are accepted. return non-zero on error */
പൂർണ്ണസംഖ്യ curlx_str_സംഖ്യ(സ്ഥിരം അക്ഷരം **linep, curl_off_t *nump, curl_off_t max)
{
  തിരികെ str_അക്കം_base(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
പൂർണ്ണസംഖ്യ curlx_str_hex(സ്ഥിരം അക്ഷരം **linep, curl_off_t *nump, curl_off_t max)
{
  തിരികെ str_അക്കം_base(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
പൂർണ്ണസംഖ്യ curlx_str_octal(സ്ഥിരം അക്ഷരം **linep, curl_off_t *nump, curl_off_t max)
{
  തിരികെ str_അക്കം_base(linep, nump, max, 8);
}

/*
 * Parse a positive number up to 63-bit number written in ASCII. Skip leading
 * blanks. No support for prefixes.
 */
പൂർണ്ണസംഖ്യ curlx_str_numblanks(സ്ഥിരം അക്ഷരം **str, curl_off_t *അക്കം)
{
  curlx_str_passblanks(str);
  തിരികെ curlx_str_സംഖ്യ(str, അക്കം, CURL_OFF_T_MAX);
}

/* CR or LF
   return non-zero on error */
പൂർണ്ണസംഖ്യ curlx_str_newline(സ്ഥിരം അക്ഷരം **linep)
{
  DEBUGASSERT(linep && *linep);
  എങ്കിൽ(ISNEWLINE(**linep)) {
    (*linep)++;
    തിരികെ STRE_OK; /* yessir */
  }
  തിരികെ STRE_NEWLINE;
}

#ifndef WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
പൂർണ്ണസംഖ്യ curlx_str_casecompare(ഘടന Curl_str *str, സ്ഥിരം അക്ഷരം *check)
{
  size_t clen = check ? strlen(check) : 0;
  തിരികെ ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#endif

/* case sensitive string compare. Returns non-zero on match. */
പൂർണ്ണസംഖ്യ curlx_str_cmp(ഘടന Curl_str *str, സ്ഥിരം അക്ഷരം *check)
{
  എങ്കിൽ(check) {
    size_t clen = strlen(check);
    തിരികെ ((str->len == clen) && !strncmp(str->str, check, clen));
  }
  തിരികെ !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
പൂർണ്ണസംഖ്യ curlx_str_nudge(ഘടന Curl_str *str, size_t അക്കം)
{
  എങ്കിൽ(അക്കം <= str->len) {
    str->str += അക്കം;
    str->len -= അക്കം;
    തിരികെ STRE_OK;
  }
  തിരികെ STRE_OVERFLOW;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
പൂർണ്ണസംഖ്യ curlx_str_cspn(സ്ഥിരം അക്ഷരം **linep, ഘടന Curl_str *out,
                   സ്ഥിരം അക്ഷരം *reject)
{
  സ്ഥിരം അക്ഷരം *s = *linep;
  size_t len;
  DEBUGASSERT(linep && *linep);

  len = strcspn(s, reject);
  എങ്കിൽ(len) {
    out->str = s;
    out->len = len;
    *linep = &s[len];
    തിരികെ STRE_OK;
  }
  curlx_str_init(out);
  തിരികെ STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
ശൂന്യം curlx_str_trimblanks(ഘടന Curl_str *out)
{
  ആകുമ്പോൾ(out->len && ISBLANK(*out->str))
    curlx_str_nudge(out, 1);

  /* trim trailing spaces and tabs */
  ആകുമ്പോൾ(out->len && ISBLANK(out->str[out->len - 1]))
    out->len--;
}

/* increase the pointer until it has moved over all blanks */
ശൂന്യം curlx_str_passblanks(സ്ഥിരം അക്ഷരം **linep)
{
  ആകുമ്പോൾ(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
