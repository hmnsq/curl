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
#అన్తర్భావ "curlx/strparse.h"

శూన్య curlx_str_init(నిర్మాణం Curl_str *బహిర్గతం)
{
  బహిర్గతం->str = ఖాళీ;
  బహిర్గతం->len = 0;
}

శూన్య curlx_str_కేటాయించు(నిర్మాణం Curl_str *బహిర్గతం, స్థిరరాశి అక్షరం *str, size_t len)
{
  బహిర్గతం->str = str;
  బహిర్గతం->len = len;
}

/* remove bytes from the end of the string, never remove more bytes than what
   the string holds! */
శూన్య curlx_str_trim(నిర్మాణం Curl_str *బహిర్గతం, size_t len)
{
  DEBUGASSERT(బహిర్గతం);
  DEBUGASSERT(బహిర్గతం->len >= len);
  బహిర్గతం->len -= len;
}

/* Get a word until the first DELIM or end of string. At least one byte long.
   return non-zero on error. If 'max' is zero, it will always return error. */
పూర్ణాంకం curlx_str_until(స్థిరరాశి అక్షరం **linep, నిర్మాణం Curl_str *బహిర్గతం,
                    స్థిరరాశి size_t max, అక్షరం delim)
{
  స్థిరరాశి అక్షరం *s;
  size_t len = 0;
  DEBUGASSERT(linep);
  DEBUGASSERT(*linep);
  DEBUGASSERT(బహిర్గతం);
  DEBUGASSERT(delim);
  s = *linep;

  curlx_str_init(బహిర్గతం);
  అయితే(*s && (*s != delim)) {
    s++;
    ఒకవేళ(++len > max) {
      తిరిగి STRE_BIG;
    }
  }
  ఒకవేళ(!len)
    తిరిగి STRE_SHORT;
  బహిర్గతం->str = *linep;
  బహిర్గతం->len = len;
  *linep = s; /* point to the first byte after the word */
  తిరిగి STRE_OK;
}

/* Get a word until the first space or end of string. At least one byte long.
   return non-zero on error */
పూర్ణాంకం curlx_str_మాట(స్థిరరాశి అక్షరం **linep, నిర్మాణం Curl_str *బహిర్గతం, స్థిరరాశి size_t max)
{
  తిరిగి curlx_str_until(linep, బహిర్గతం, max, ' ');
}

/* Get a word until a newline byte or end of string. At least one byte long.
   return non-zero on error */
పూర్ణాంకం curlx_str_untilnl(స్థిరరాశి అక్షరం **linep, నిర్మాణం Curl_str *బహిర్గతం,
                      స్థిరరాశి size_t max)
{
  స్థిరరాశి అక్షరం *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && బహిర్గతం && max);

  curlx_str_init(బహిర్గతం);
  అయితే(*s && !ISNEWLINE(*s)) {
    s++;
    ఒకవేళ(++len > max)
      తిరిగి STRE_BIG;
  }
  ఒకవేళ(!len)
    తిరిగి STRE_SHORT;
  బహిర్గతం->str = *linep;
  బహిర్గతం->len = len;
  *linep = s; /* point to the first byte after the word */
  తిరిగి STRE_OK;
}

/* Get a "quoted" word. Escaped quotes are supported.
   return non-zero on error */
పూర్ణాంకం curlx_str_quotedword(స్థిరరాశి అక్షరం **linep, నిర్మాణం Curl_str *బహిర్గతం,
                         స్థిరరాశి size_t max)
{
  స్థిరరాశి అక్షరం *s = *linep;
  size_t len = 0;
  DEBUGASSERT(linep && *linep && బహిర్గతం && max);

  curlx_str_init(బహిర్గతం);
  ఒకవేళ(*s != '\"')
    తిరిగి STRE_BEGQUOTE;
  s++;
  అయితే(*s && (*s != '\"')) {
    ఒకవేళ(*s == '\\' && s[1]) {
      s++;
      ఒకవేళ(++len > max)
        తిరిగి STRE_BIG;
    }
    s++;
    ఒకవేళ(++len > max)
      తిరిగి STRE_BIG;
  }
  ఒకవేళ(*s != '\"')
    తిరిగి STRE_ENDQUOTE;
  బహిర్గతం->str = (*linep) + 1;
  బహిర్గతం->len = len;
  *linep = s + 1;
  తిరిగి STRE_OK;
}

/* Advance over a single character.
   return non-zero on error */
పూర్ణాంకం curlx_str_single(స్థిరరాశి అక్షరం **linep, అక్షరం byte)
{
  DEBUGASSERT(linep && *linep);
  ఒకవేళ(**linep != byte)
    తిరిగి STRE_BYTE;
  (*linep)++; /* move over it */
  తిరిగి STRE_OK;
}

/* Advance over a single space.
   return non-zero on error */
పూర్ణాంకం curlx_str_singlespace(స్థిరరాశి అక్షరం **linep)
{
  తిరిగి curlx_str_single(linep, ' ');
}

/* given an ASCII character and max ascii, return TRUE if valid */
#నిర్వచించు valid_digit(x, m) \
  (((x) >= '0') && ((x) <= (m)) && curlx_hexasciitable[(x) - '0'])

/* We use 16 for the zero index (and the necessary bitwise AND in the loop)
   to be able to have a non-zero value there to make valid_digit() able to
   use the info */
స్థిరరాశి గుర్తులేని అక్షరం curlx_hexasciitable[] = {
  16, 1, 2, 3, 4, 5, 6, 7, 8, 9, /* 0x30: 0 - 9 */
  0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15,        /* 0x41: A - F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  10, 11, 12, 13, 14, 15         /* 0x61: a - f */
};

/* no support for 0x prefix nor leading spaces */
స్థిర పూర్ణాంకం str_అంకె_పునాది(స్థిరరాశి అక్షరం **linep, curl_off_t *nump, curl_off_t max,
                        పూర్ణాంకం పునాది) /* 8, 10 or 16, nothing else */
{
  curl_off_t అంకె = 0;
  స్థిరరాశి అక్షరం *p;
  పూర్ణాంకం m = (పునాది == 10) ? '9' :   /* the largest digit possible */
    (పునాది == 16) ? 'f' : '7';
  DEBUGASSERT(linep && *linep && nump);
  DEBUGASSERT((పునాది == 8) || (పునాది == 10) || (పునాది == 16));
  DEBUGASSERT(max >= 0); /* mostly to catch SIZE_MAX, which is too large */
  *nump = 0;
  p = *linep;
  ఒకవేళ(!valid_digit(*p, m))
    తిరిగి STRE_NO_అంకె;
  ఒకవేళ(max < పునాది) {
    /* special-case low max scenario because check needs to be different */
    చేయి {
      పూర్ణాంకం n = curlx_hexval(*p++);
      అంకె = (అంకె * పునాది) + n;
      ఒకవేళ(అంకె > max)
        తిరిగి STRE_అధికప్రవాహం;
    } అయితే(valid_digit(*p, m));
  }
  లేకపోతే {
    curl_off_t cutoff = (max - (పునాది - 1)) / పునాది;
    అయితే(valid_digit(*p, m)) {
      ఒకవేళ(అంకె <= cutoff)
        అంకె = (అంకె * పునాది) + curlx_hexval(*p++);
      లేకపోతే {
        పూర్ణాంకం n = curlx_hexval(*p++);
        ఒకవేళ(అంకె > ((max - n) / పునాది))
          తిరిగి STRE_అధికప్రవాహం;
        అంకె = (అంకె * పునాది) + n;
      }
    }
  }
  *nump = అంకె;
  *linep = p;
  తిరిగి STRE_OK;
}

/* Get an unsigned decimal number with no leading space or minus. Leading
   zeroes are accepted. return non-zero on error */
పూర్ణాంకం curlx_str_సంఖ్య(స్థిరరాశి అక్షరం **linep, curl_off_t *nump, curl_off_t max)
{
  తిరిగి str_అంకె_పునాది(linep, nump, max, 10);
}

/* Get an unsigned hexadecimal number with no leading space or minus and no
   "0x" support. Leading zeroes are accepted. return non-zero on error */
పూర్ణాంకం curlx_str_hex(స్థిరరాశి అక్షరం **linep, curl_off_t *nump, curl_off_t max)
{
  తిరిగి str_అంకె_పునాది(linep, nump, max, 16);
}

/* Get an unsigned octal number with no leading space or minus and no "0"
   prefix support. Leading zeroes are accepted. return non-zero on error */
పూర్ణాంకం curlx_str_octal(స్థిరరాశి అక్షరం **linep, curl_off_t *nump, curl_off_t max)
{
  తిరిగి str_అంకె_పునాది(linep, nump, max, 8);
}

/*
 * Parse a positive number up to 63-bit number written in ASCII. Skip leading
 * blanks. No support for prefixes.
 */
పూర్ణాంకం curlx_str_numblanks(స్థిరరాశి అక్షరం **str, curl_off_t *అంకె)
{
  curlx_str_passblanks(str);
  తిరిగి curlx_str_సంఖ్య(str, అంకె, CURL_OFF_T_MAX);
}

/* CR or LF
   return non-zero on error */
పూర్ణాంకం curlx_str_newline(స్థిరరాశి అక్షరం **linep)
{
  DEBUGASSERT(linep && *linep);
  ఒకవేళ(ISNEWLINE(**linep)) {
    (*linep)++;
    తిరిగి STRE_OK; /* yessir */
  }
  తిరిగి STRE_NEWLINE;
}

#నిర్వచితం_కాకపోతే WITHOUT_LIBCURL
/* case insensitive compare that the parsed string matches the given string.
   Returns non-zero on match. */
పూర్ణాంకం curlx_str_casecompare(నిర్మాణం Curl_str *str, స్థిరరాశి అక్షరం *check)
{
  size_t clen = check ? అక్షరశ్రేణి_పొడవు(check) : 0;
  తిరిగి ((str->len == clen) && curl_strnequal(str->str, check, clen));
}
#అయితే_ముగింపు

/* case sensitive string compare. Returns non-zero on match. */
పూర్ణాంకం curlx_str_cmp(నిర్మాణం Curl_str *str, స్థిరరాశి అక్షరం *check)
{
  ఒకవేళ(check) {
    size_t clen = అక్షరశ్రేణి_పొడవు(check);
    తిరిగి ((str->len == clen) && !అక్షరశ్రేణి_పోలిక(str->str, check, clen));
  }
  తిరిగి !!str->len;
}

/* Trim off 'num' number of bytes from the beginning (left side) of the
   string. If 'num' is larger than the string, return error. */
పూర్ణాంకం curlx_str_nudge(నిర్మాణం Curl_str *str, size_t అంకె)
{
  ఒకవేళ(అంకె <= str->len) {
    str->str += అంకె;
    str->len -= అంకె;
    తిరిగి STRE_OK;
  }
  తిరిగి STRE_అధికప్రవాహం;
}

/* Get the following character sequence that consists only of bytes not
   present in the 'reject' string. Like strcspn(). */
పూర్ణాంకం curlx_str_cspn(స్థిరరాశి అక్షరం **linep, నిర్మాణం Curl_str *బహిర్గతం,
                   స్థిరరాశి అక్షరం *reject)
{
  స్థిరరాశి అక్షరం *s = *linep;
  size_t len;
  DEBUGASSERT(linep && *linep);

  len = strcspn(s, reject);
  ఒకవేళ(len) {
    బహిర్గతం->str = s;
    బహిర్గతం->len = len;
    *linep = &s[len];
    తిరిగి STRE_OK;
  }
  curlx_str_init(బహిర్గతం);
  తిరిగి STRE_SHORT;
}

/* remove ISBLANK()s from both ends of the string */
శూన్య curlx_str_trimblanks(నిర్మాణం Curl_str *బహిర్గతం)
{
  అయితే(బహిర్గతం->len && ISBLANK(*బహిర్గతం->str))
    curlx_str_nudge(బహిర్గతం, 1);

  /* trim trailing spaces and tabs */
  అయితే(బహిర్గతం->len && ISBLANK(బహిర్గతం->str[బహిర్గతం->len - 1]))
    బహిర్గతం->len--;
}

/* increase the pointer until it has moved over all blanks */
శూన్య curlx_str_passblanks(స్థిరరాశి అక్షరం **linep)
{
  అయితే(ISBLANK(**linep))
    (*linep)++; /* move over it */
}
