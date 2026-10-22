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
#অন্তর্ভাব "curl_setup.h"

#ifndef CURL_DISABLE_FTP

#অন্তর্ভাব "curl_fnmatch.h"

#ifndef HAVE_FNMATCH

#define CURLFNM_CHARSET_LEN (আকার(অক্ষর) * 256)
#define CURLFNM_CHSET_আকার (CURLFNM_CHARSET_LEN + 15)

#define CURLFNM_NEGATE  CURLFNM_CHARSET_LEN

#define CURLFNM_ALNUM   (CURLFNM_CHARSET_LEN + 1)
#define CURLFNM_DIGIT   (CURLFNM_CHARSET_LEN + 2)
#define CURLFNM_XDIGIT  (CURLFNM_CHARSET_LEN + 3)
#define CURLFNM_ALPHA   (CURLFNM_CHARSET_LEN + 4)
#define CURLFNM_PRINT   (CURLFNM_CHARSET_LEN + 5)
#define CURLFNM_BLANK   (CURLFNM_CHARSET_LEN + 6)
#define CURLFNM_LOWER   (CURLFNM_CHARSET_LEN + 7)
#define CURLFNM_GRAPH   (CURLFNM_CHARSET_LEN + 8)
#define CURLFNM_SPACE   (CURLFNM_CHARSET_LEN + 9)
#define CURLFNM_UPPER   (CURLFNM_CHARSET_LEN + 10)

প্রকারলেখ গণনা {
  CURLFNM_SCHS_DEFAULT = 0,
  CURLFNM_SCHS_RIGHTBR,
  CURLFNM_SCHS_RIGHTBRLEFTBR
} setcharset_state;

প্রকারলেখ গণনা {
  CURLFNM_PKW_INIT = 0,
  CURLFNM_PKW_DDOT
} parsekey_state;

প্রকারলেখ গণনা {
  CCLASS_OTHER = 0,
  CCLASS_DIGIT,
  CCLASS_UPPER,
  CCLASS_LOWER
} char_class;

#define SETCHARSET_OK     1
#define SETCHARSET_FAIL   0

স্থির পূর্ণসংখ্যা parsekeyword(ধ্রুবক unsigned অক্ষর **pattern, unsigned অক্ষর *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  অক্ষর keyword[10] = { 0 };
  size_t i;
  ধ্রুবক unsigned অক্ষর *p = *pattern;
  bool found = FALSE;
  জন্য(i = 0; !found; i++) {
    অক্ষর c = (অক্ষর)*p++;
    যদি(i >= আকার(keyword))
      ফেরত SETCHARSET_FAIL;
    নির্বাচন(state) {
    ক্ষেত্র CURLFNM_PKW_INIT:
      যদি(ISLOWER(c))
        keyword[i] = c;
      অন্যথা যদি(c == ':')
        state = CURLFNM_PKW_DDOT;
      অন্যথা
        ফেরত SETCHARSET_FAIL;
      বিরতি;
    ক্ষেত্র CURLFNM_PKW_DDOT:
      যদি(c == ']')
        found = TRUE;
      অন্যথা
        ফেরত SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  যদি(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  অন্যথা যদি(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  অন্যথা যদি(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  অন্যথা যদি(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  অন্যথা যদি(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  অন্যথা যদি(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  অন্যথা যদি(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  অন্যথা যদি(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  অন্যথা যদি(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  অন্যথা যদি(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  অন্যথা
    ফেরত SETCHARSET_FAIL;
  ফেরত SETCHARSET_OK;
}

/* Return the character class. */
স্থির char_class charclass(unsigned অক্ষর c)
{
  যদি(ISUPPER(c))
    ফেরত CCLASS_UPPER;
  যদি(ISLOWER(c))
    ফেরত CCLASS_LOWER;
  যদি(ISDIGIT(c))
    ফেরত CCLASS_DIGIT;
  ফেরত CCLASS_OTHER;
}

/* Include a character or a range in set. */
স্থির শূন্য setcharorrange(ধ্রুবক unsigned অক্ষর **pp, unsigned অক্ষর *charset)
{
  ধ্রুবক unsigned অক্ষর *p = (*pp)++;
  unsigned অক্ষর c = *p++;

  charset[c] = 1;
  যদি(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned অক্ষর endrange = *p++;

    যদি(endrange == '\\')
      endrange = *p++;
    যদি(endrange >= c && charclass(endrange) == cc) {
      যখন(c++ != endrange)
        যদি(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
স্থির পূর্ণসংখ্যা setcharset(ধ্রুবক unsigned অক্ষর **p, unsigned অক্ষর *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  unsigned অক্ষর c;

  memset(charset, 0, CURLFNM_CHSET_আকার);
  জন্য(;;) {
    c = **p;
    যদি(!c)
      ফেরত SETCHARSET_FAIL;

    নির্বাচন(state) {
    ক্ষেত্র CURLFNM_SCHS_DEFAULT:
      যদি(c == ']') {
        যদি(something_found)
          ফেরত SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      অন্যথা যদি(c == '[') {
        ধ্রুবক unsigned অক্ষর *pp = *p + 1;

        যদি(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        অন্যথা {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      অন্যথা যদি(c == '^' || c == '!') {
        যদি(!something_found) {
          যদি(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          অন্যথা
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        অন্যথা
          charset[c] = 1;
        (*p)++;
      }
      অন্যথা যদি(c == '\\') {
        c = *(++(*p));
        যদি(c)
          setcharorrange(p, charset);
        অন্যথা
          charset['\\'] = 1;
        something_found = TRUE;
      }
      অন্যথা {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      বিরতি;
    ক্ষেত্র CURLFNM_SCHS_RIGHTBR:
      যদি(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      অন্যথা যদি(c == ']') {
        ফেরত SETCHARSET_OK;
      }
      অন্যথা যদি(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      অন্যথা
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        যাও fail;
      বিরতি;
    ক্ষেত্র CURLFNM_SCHS_RIGHTBRLEFTBR:
      যদি(c == ']')
        ফেরত SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      বিরতি;
    }
  }
fail:
  ফেরত SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
স্থির পূর্ণসংখ্যা matchtoken(ধ্রুবক unsigned অক্ষর **pp, ধ্রুবক unsigned অক্ষর **sp)
{
  ধ্রুবক unsigned অক্ষর *p = *pp;
  ধ্রুবক unsigned অক্ষর *s = *sp;

  নির্বাচন(*p) {
  ক্ষেত্র '?':
    বিরতি;
  ক্ষেত্র '\\':
    যদি(p[1])
      p++;
    যদি(*s != *p)
      ফেরত 0;
    বিরতি;
  ক্ষেত্র '[': {
    unsigned অক্ষর charset[CURLFNM_CHSET_আকার];
    ধ্রুবক unsigned অক্ষর *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    যদি(!setcharset(&pp2, charset))
      ফেরত -1; /* Syntax error in set; mismatch! */
    যদি(charset[(unsigned পূর্ণসংখ্যা)*s])
      found = TRUE;
    অন্যথা যদি(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    অন্যথা যদি(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    অন্যথা যদি(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    অন্যথা যদি(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    অন্যথা যদি(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    অন্যথা যদি(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    অন্যথা যদি(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    অন্যথা যদি(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    অন্যথা যদি(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    অন্যথা যদি(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    যদি(charset[CURLFNM_NEGATE])
      found = !found;

    যদি(!found)
      ফেরত 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    ফেরত 1;
  }
  পূর্বনির্ধারিত:
    যদি(*p != *s)
      ফেরত 0;
    বিরতি;
  }
  *pp = p + 1;
  *sp = s + 1;
  ফেরত 1;
}

/* greedy match with backtracking to the most recent '*' */
স্থির পূর্ণসংখ্যা loop(ধ্রুবক unsigned অক্ষর *pattern, ধ্রুবক unsigned অক্ষর *string)
{
  ধ্রুবক unsigned অক্ষর *p = pattern;
  ধ্রুবক unsigned অক্ষর *s = string;
  ধ্রুবক unsigned অক্ষর *star_p = ফাঁকা;
  ধ্রুবক unsigned অক্ষর *star_s = ফাঁকা;
  পূর্ণসংখ্যা maxstars = 2;

  যখন(*s) {
    যদি(*p == '*') {
      যদি(!maxstars)
        ফেরত CURL_FNMATCH_NOMATCH;
      maxstars--;
      জন্য(;;) {
        p++;
        যদি(!*p)
          ফেরত CURL_FNMATCH_MATCH;
        যদি(*p == '?') {
          যদি(!*s)
            ফেরত CURL_FNMATCH_NOMATCH;
          s++;
        }
        অন্যথা যদি(*p != '*')
          বিরতি;
      }
      star_p = p;
      star_s = s;
    }
    অন্যথা যদি(*p) {
      ধ্রুবক unsigned অক্ষর *np = p;
      ধ্রুবক unsigned অক্ষর *ns = s;
      পূর্ণসংখ্যা r = matchtoken(&np, &ns);
      যদি(r < 0)
        ফেরত CURL_FNMATCH_NOMATCH;
      যদি(r) {
        p = np;
        s = ns;
        চালু;
      }
      যদি(!star_p)
        ফেরত CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    অন্যথা যদি(star_p) {
      p = star_p;
      s = ++star_s;
    }
    অন্যথা
      ফেরত CURL_FNMATCH_NOMATCH;
  }
  যখন(*p == '*') {
    যদি(!maxstars)
      ফেরত CURL_FNMATCH_NOMATCH;
    maxstars--;
    করো {
      p++;
      যদি(*p == '?')
        ফেরত CURL_FNMATCH_NOMATCH;
    } যখন(*p == '*');
  }
  ফেরত *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
পূর্ণসংখ্যা Curl_fnmatch(শূন্য *ptr, ধ্রুবক অক্ষর *pattern, ধ্রুবক অক্ষর *string)
{
  (শূন্য)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  যদি(!pattern || !string) {
    ফেরত CURL_FNMATCH_FAIL;
  }
  ফেরত loop((ধ্রুবক unsigned অক্ষর *)pattern,
              (ধ্রুবক unsigned অক্ষর *)string);
}
#অন্যথা /* HAVE_FNMATCH */

#অন্তর্ভাব <fnmatch.h>
/*
 * @unittest: 1307
 */
পূর্ণসংখ্যা Curl_fnmatch(শূন্য *ptr, ধ্রুবক অক্ষর *pattern, ধ্রুবক অক্ষর *string)
{
  (শূন্য)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  যদি(!pattern || !string) {
    ফেরত CURL_FNMATCH_FAIL;
  }

  নির্বাচন(fnmatch(pattern, string, 0)) {
  ক্ষেত্র 0:
    ফেরত CURL_FNMATCH_MATCH;
  ক্ষেত্র FNM_NOMATCH:
    ফেরত CURL_FNMATCH_NOMATCH;
  পূর্বনির্ধারিত:
    ফেরত CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
