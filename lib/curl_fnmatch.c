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
#অন্তর্ভাৱ "curl_setup.h"

#ifndef CURL_DISABLE_FTP

#অন্তর্ভাৱ "curl_fnmatch.h"

#ifndef HAVE_FNMATCH

#define CURLFNM_CHARSET_LEN (পরিমাণ(ৱর্ণ) * 256)
#define CURLFNM_CHSET_SIZE (CURLFNM_CHARSET_LEN + 15)

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

প্রকারনির্দেশ গণনা {
  CURLFNM_SCHS_DEFAULT = 0,
  CURLFNM_SCHS_RIGHTBR,
  CURLFNM_SCHS_RIGHTBRLEFTBR
} setcharset_state;

প্রকারনির্দেশ গণনা {
  CURLFNM_PKW_INIT = 0,
  CURLFNM_PKW_DDOT
} parsekey_state;

প্রকারনির্দেশ গণনা {
  CCLASS_OTHER = 0,
  CCLASS_DIGIT,
  CCLASS_UPPER,
  CCLASS_LOWER
} char_class;

#define SETCHARSET_OK     1
#define SETCHARSET_FAIL   0

স্থির পূর্ণাঙ্ক parsekeyword(ধ্রুৱ unsigned ৱর্ণ **pattern, unsigned ৱর্ণ *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  ৱর্ণ keyword[10] = { 0 };
  size_t i;
  ধ্রুৱ unsigned ৱর্ণ *p = *pattern;
  bool found = FALSE;
  বাবে(i = 0; !found; i++) {
    ৱর্ণ c = (ৱর্ণ)*p++;
    যদি(i >= পরিমাণ(keyword))
      ঘূৰাই SETCHARSET_FAIL;
    ৱিকল্প(state) {
    পক্ষ CURLFNM_PKW_INIT:
      যদি(ISLOWER(c))
        keyword[i] = c;
      অন্যথা যদি(c == ':')
        state = CURLFNM_PKW_DDOT;
      অন্যথা
        ঘূৰাই SETCHARSET_FAIL;
      ভঙ্গ;
    পক্ষ CURLFNM_PKW_DDOT:
      যদি(c == ']')
        found = TRUE;
      অন্যথা
        ঘূৰাই SETCHARSET_FAIL;
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
    ঘূৰাই SETCHARSET_FAIL;
  ঘূৰাই SETCHARSET_OK;
}

/* Return the character class. */
স্থির char_class charclass(unsigned ৱর্ণ c)
{
  যদি(ISUPPER(c))
    ঘূৰাই CCLASS_UPPER;
  যদি(ISLOWER(c))
    ঘূৰাই CCLASS_LOWER;
  যদি(ISDIGIT(c))
    ঘূৰাই CCLASS_DIGIT;
  ঘূৰাই CCLASS_OTHER;
}

/* Include a character or a range in set. */
স্থির শূন্য setcharorrange(ধ্রুৱ unsigned ৱর্ণ **pp, unsigned ৱর্ণ *charset)
{
  ধ্রুৱ unsigned ৱর্ণ *p = (*pp)++;
  unsigned ৱর্ণ c = *p++;

  charset[c] = 1;
  যদি(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned ৱর্ণ endrange = *p++;

    যদি(endrange == '\\')
      endrange = *p++;
    যদি(endrange >= c && charclass(endrange) == cc) {
      যেতিয়ালৈ(c++ != endrange)
        যদি(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
স্থির পূর্ণাঙ্ক setcharset(ধ্রুৱ unsigned ৱর্ণ **p, unsigned ৱর্ণ *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  unsigned ৱর্ণ c;

  memset(charset, 0, CURLFNM_CHSET_SIZE);
  বাবে(;;) {
    c = **p;
    যদি(!c)
      ঘূৰাই SETCHARSET_FAIL;

    ৱিকল্প(state) {
    পক্ষ CURLFNM_SCHS_DEFAULT:
      যদি(c == ']') {
        যদি(something_found)
          ঘূৰাই SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      অন্যথা যদি(c == '[') {
        ধ্রুৱ unsigned ৱর্ণ *pp = *p + 1;

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
      ভঙ্গ;
    পক্ষ CURLFNM_SCHS_RIGHTBR:
      যদি(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      অন্যথা যদি(c == ']') {
        ঘূৰাই SETCHARSET_OK;
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
        গচ্ছ fail;
      ভঙ্গ;
    পক্ষ CURLFNM_SCHS_RIGHTBRLEFTBR:
      যদি(c == ']')
        ঘূৰাই SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      ভঙ্গ;
    }
  }
fail:
  ঘূৰাই SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
স্থির পূর্ণাঙ্ক matchtoken(ধ্রুৱ unsigned ৱর্ণ **pp, ধ্রুৱ unsigned ৱর্ণ **sp)
{
  ধ্রুৱ unsigned ৱর্ণ *p = *pp;
  ধ্রুৱ unsigned ৱর্ণ *s = *sp;

  ৱিকল্প(*p) {
  পক্ষ '?':
    ভঙ্গ;
  পক্ষ '\\':
    যদি(p[1])
      p++;
    যদি(*s != *p)
      ঘূৰাই 0;
    ভঙ্গ;
  পক্ষ '[': {
    unsigned ৱর্ণ charset[CURLFNM_CHSET_SIZE];
    ধ্রুৱ unsigned ৱর্ণ *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    যদি(!setcharset(&pp2, charset))
      ঘূৰাই -1; /* Syntax error in set; mismatch! */
    যদি(charset[(unsigned পূর্ণাঙ্ক)*s])
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
      ঘূৰাই 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    ঘূৰাই 1;
  }
  স্ৱভাৱ:
    যদি(*p != *s)
      ঘূৰাই 0;
    ভঙ্গ;
  }
  *pp = p + 1;
  *sp = s + 1;
  ঘূৰাই 1;
}

/* greedy match with backtracking to the most recent '*' */
স্থির পূর্ণাঙ্ক loop(ধ্রুৱ unsigned ৱর্ণ *pattern, ধ্রুৱ unsigned ৱর্ণ *string)
{
  ধ্রুৱ unsigned ৱর্ণ *p = pattern;
  ধ্রুৱ unsigned ৱর্ণ *s = string;
  ধ্রুৱ unsigned ৱর্ণ *star_p = রিক্ত;
  ধ্রুৱ unsigned ৱর্ণ *star_s = রিক্ত;
  পূর্ণাঙ্ক maxstars = 2;

  যেতিয়ালৈ(*s) {
    যদি(*p == '*') {
      যদি(!maxstars)
        ঘূৰাই CURL_FNMATCH_NOMATCH;
      maxstars--;
      বাবে(;;) {
        p++;
        যদি(!*p)
          ঘূৰাই CURL_FNMATCH_MATCH;
        যদি(*p == '?') {
          যদি(!*s)
            ঘূৰাই CURL_FNMATCH_NOMATCH;
          s++;
        }
        অন্যথা যদি(*p != '*')
          ভঙ্গ;
      }
      star_p = p;
      star_s = s;
    }
    অন্যথা যদি(*p) {
      ধ্রুৱ unsigned ৱর্ণ *np = p;
      ধ্রুৱ unsigned ৱর্ণ *ns = s;
      পূর্ণাঙ্ক r = matchtoken(&np, &ns);
      যদি(r < 0)
        ঘূৰাই CURL_FNMATCH_NOMATCH;
      যদি(r) {
        p = np;
        s = ns;
        অনুৱর্ত;
      }
      যদি(!star_p)
        ঘূৰাই CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    অন্যথা যদি(star_p) {
      p = star_p;
      s = ++star_s;
    }
    অন্যথা
      ঘূৰাই CURL_FNMATCH_NOMATCH;
  }
  যেতিয়ালৈ(*p == '*') {
    যদি(!maxstars)
      ঘূৰাই CURL_FNMATCH_NOMATCH;
    maxstars--;
    কৰা {
      p++;
      যদি(*p == '?')
        ঘূৰাই CURL_FNMATCH_NOMATCH;
    } যেতিয়ালৈ(*p == '*');
  }
  ঘূৰাই *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
পূর্ণাঙ্ক Curl_fnmatch(শূন্য *ptr, ধ্রুৱ ৱর্ণ *pattern, ধ্রুৱ ৱর্ণ *string)
{
  (শূন্য)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  যদি(!pattern || !string) {
    ঘূৰাই CURL_FNMATCH_FAIL;
  }
  ঘূৰাই loop((ধ্রুৱ unsigned ৱর্ণ *)pattern,
              (ধ্রুৱ unsigned ৱর্ণ *)string);
}
#অন্যথা /* HAVE_FNMATCH */

#অন্তর্ভাৱ <fnmatch.h>
/*
 * @unittest: 1307
 */
পূর্ণাঙ্ক Curl_fnmatch(শূন্য *ptr, ধ্রুৱ ৱর্ণ *pattern, ধ্রুৱ ৱর্ণ *string)
{
  (শূন্য)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  যদি(!pattern || !string) {
    ঘূৰাই CURL_FNMATCH_FAIL;
  }

  ৱিকল্প(fnmatch(pattern, string, 0)) {
  পক্ষ 0:
    ঘূৰাই CURL_FNMATCH_MATCH;
  পক্ষ FNM_NOMATCH:
    ঘূৰাই CURL_FNMATCH_NOMATCH;
  স্ৱভাৱ:
    ঘূৰাই CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
