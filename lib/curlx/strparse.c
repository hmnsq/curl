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
#அந்தர்பாவ "curlx/strparse.h"

வெறுமை curlx_str_init(கட்டமைப்பு Curl_str *out)
{
  out->str = வெற்று;
  out->len = 0;
}

வெறுமை curlx_str_assign(கட்டமைப்பு Curl_str *out, மாறிலி எழுத்து *str, size_t len)
{
  out->str = str;
  out->len = len;
}

/* remove bytes from the end of the string, never remove more bytes than what
   the string holds! */
வெறுமை curlx_str_trim(கட்டமைப்பு Curl_str *out, size_t len)
{
  DEBUGASSERT(out);
  DEBUGASSERT(out->len >= len);
  out->len -= len;
}

/* Get a word until the first DELIM or end of string. At least one byte long.
   return non-zero on error. If 'max' is zero, it will always return error. */
முழு curlx_str_until(மாறிலி எழுத்து **linep, கட்டமைப்பு Curl_str *out,
                    மாறிலி size_t max, எழுத்து delim)
{
  மாறிலி எழுத்து *s;
  size_t len = 0;
  DEBUGASSERT(linep);
  DEBUGASSERT(*linep);
  DEBUGASSERT(out);
  DEBUGASSERT(delim);
  s = *linep;

  curlx_str_init(out);
  போது(*s && (*s != delim)) {
    s++;
    எனில்(++len > max) {
      திரும்பு STRE_BIG;
    }
  }
  எனில்(!len)
    திரும்பு STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  திரும்பு STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
முழு curlx_str_word(மாறிலி எழுத்து **linep, கட்டமைப்பு Curl_str *out, மாறிலி size_t max)
{
  திரும்பு curlx_str_until(linep, out, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
முழு curlx_str_untilnl(மாறிலி எழுத்து **linep, கட்டமைப்பு Curl_str *out,
                      மாறிலி size_t max)
{
  மாறிலி எழுத்து *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  போது(*s && !ISNEWLINE(*s)) {
    s++;
    எனில்(++len > max)
      திரும்பு STRE_BIG;
  }
  எனில்(!len)
    திரும்பு STRE_SHORT;
  out->str = *linep;
  out->len = len;
  *linep = s; /* point to the first byte after the word */
  திரும்பு STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
முழு curlx_str_quotedword(மாறிலி எழுத்து **linep, கட்டமைப்பு Curl_str *out,
                         மாறிலி size_t max)
{
  மாறிலி எழுத்து *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && out && max);

  curlx_str_init(out);
  எனில்(*s != '\"')
    திரும்பு STRE_BEGQUOTE;
  s++;
  போது(*s && (*s != '\"')) {
    எனில்(*s == '\\' && s[1]) {
      s++;
      எனில்(++len > max)
        திரும்பு STRE_BIG;
    }
    s++;
    எனில்(++len > max)
      திரும்பு STRE_BIG;
  }
  எனில்(*s != '\"')
    திரும்பு STRE_ENDQUOTE;
  out->str = (*linep) + 1;
  out->len = len;
  *linep = s + 1;
  திரும்பு STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
முழு curlx_str_single(மாறிலி எழுத்து **linep, எழுத்து byte)
{
  DEBUGASSERT(linep && *linep);
  எனில்(**linep != byte)
    திரும்பு STRE_BYTE;
  (*linep)++; /* move over it */
  திரும்பு STRE_OK;
}

/* Advance over a single space.
   return non-zero on error */
முழு curlx_str_singlespace(மாறிலி எழுத்து **linep)
{
  திரும்பு curlx_str_single(linep, ' ');
}

/* given an ASCII character and max ascii, return TRUE if valid */
#define valid_digit(x, m) \
  (((x) >= '0') && ((x) <= (m)) && curlx_hexasciitable[(x) - '0'])

/* We use 16 for the zero index (and the necessary bitwise AND in the loop)
   to be able to have a non-zero value there to make valid_digit() able to
   use the info */
மாறிலி unsigned எழுத்து curlx_hexasciitable[] = {
  16, 1, 2, 3, 4, 5, 6, 7, 8, 9, /* 0x30: 0 - 9 */
  0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15,        /* 0x41: A - F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15         /* 0x61: a - f */
};

/* no support for 0x prefix nor leading spaces */
நிலையான முழு str_இலக்கம்_base(மாறிலி எழுத்து **linep, curl_off_t *nump, curl_off_t max,
                        முழு base) /* 8, 10 or 16, nothing else */
{
  curl_off_t இலக்கம் = 0;
  மாறிலி எழுத்து *p;
  முழு m = (base == 10) ? '9' :   /* the largest digit possible */
    (base == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((base == 8) || (base == 10) || (base == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  எனில்(!valid_digit(*p, m))
    திரும்பு STRE_NO_இலக்கம்;
  எனில்(max < base) {
    /* special-case low max scenario because check needs to be different */
    செய் {
      முழு n = curlx_hexval(*p++);
      இலக்கம் = (இலக்கம் * base) + n;
      எனில்(இலக்கம் > max)
        திரும்பு STRE_OVERFLOW;
    } போது(valid_digit(*p, m));
  }
  இல்லையெனில் {
    curl_off_t cutoff = (max - (base - 1)) / base;
    போது(valid_digit(*p, m)) {
      எனில்(இலக்கம் <= cutoff)
        இலக்கம் = (இலக்கம் * base) + curlx_hexval(*p++);
      இல்லையெனில் {
        முழு n = curlx_hexval(*p++);
        எனில்(இலக்கம் > ((max - n) / base))
          திரும்பு STRE_OVERFLOW;
        இலக்கம் = (இலக்கம் * base) + n;
      }
    }
  }
  *nump = இலக்கம்;
  *linep = p;
  திரும்பு STRE_OK;
}

/* Get an unsigned decimal number with no leading space or minus. Leading
   zeroes are accepted. return non-zero on error */
முழு curlx_str_எண்(மாறிலி எழுத்து **linep, curl_off_t *nump, curl_off_t max)
{
  திரும்பு str_இலக்கம்_base(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
முழு curlx_str_hex(மாறிலி எழுத்து **linep, curl_off_t *nump, curl_off_t max)
{
  திரும்பு str_இலக்கம்_base(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
முழு curlx_str_octal(மாறிலி எழுத்து **linep, curl_off_t *nump, curl_off_t max)
{
  திரும்பு str_இலக்கம்_base(linep, nump, max, 8);
}

/*
 * Parse a positive number up to 63-bit number written in ASCII. Skip leading
 * blanks. No support for prefixes.
 */
முழு curlx_str_numblanks(மாறிலி எழுத்து **str, curl_off_t *இலக்கம்)
{
  curlx_str_passblanks(str);
  திரும்பு curlx_str_எண்(str, இலக்கம், CURL_OFF_T_MAX);
}

/* CR or LF
   return non-zero on error */
முழு curlx_str_newline(மாறிலி எழுத்து **linep)
{
  DEBUGASSERT(linep && *linep);
  எனில்(ISNEWLINE(**linep)) {
    (*linep)++;
    திரும்பு STRE_OK; /* yessir */
  }
  திரும்பு STRE_NEWLINE;
}

#ifndef WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
முழு curlx_str_casecompare(கட்டமைப்பு Curl_str *str, மாறிலி எழுத்து *check)
{
  size_t clen = check ? strlen(check) : 0;
  திரும்பு ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#endif

/* case sensitive string compare. Returns non-zero on match. */
முழு curlx_str_cmp(கட்டமைப்பு Curl_str *str, மாறிலி எழுத்து *check)
{
  எனில்(check) {
    size_t clen = strlen(check);
    திரும்பு ((str->len == clen) && !strncmp(str->str, check, clen));
  }
  திரும்பு !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
முழு curlx_str_nudge(கட்டமைப்பு Curl_str *str, size_t இலக்கம்)
{
  எனில்(இலக்கம் <= str->len) {
    str->str += இலக்கம்;
    str->len -= இலக்கம்;
    திரும்பு STRE_OK;
  }
  திரும்பு STRE_OVERFLOW;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
முழு curlx_str_cspn(மாறிலி எழுத்து **linep, கட்டமைப்பு Curl_str *out,
                   மாறிலி எழுத்து *reject)
{
  மாறிலி எழுத்து *s = *linep;
  size_t len;
  DEBUGASSERT(linep && *linep);

  len = strcspn(s, reject);
  எனில்(len) {
    out->str = s;
    out->len = len;
    *linep = &s[len];
    திரும்பு STRE_OK;
  }
  curlx_str_init(out);
  திரும்பு STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
வெறுமை curlx_str_trimblanks(கட்டமைப்பு Curl_str *out)
{
  போது(out->len && ISBLANK(*out->str))
    curlx_str_nudge(out, 1);

  /* trim trailing spaces and tabs */
  போது(out->len && ISBLANK(out->str[out->len - 1]))
    out->len--;
}

/* increase the pointer until it has moved over all blanks */
வெறுமை curlx_str_passblanks(மாறிலி எழுத்து **linep)
{
  போது(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
