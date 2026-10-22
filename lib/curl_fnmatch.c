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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifndef CURL_DISABLE_FTP

#ଅନ୍ତର୍ଭାଵ "curl_fnmatch.h"

#ifndef HAVE_FNMATCH

#define CURLFNM_CHARSET_LEN (ଆକାର(ଅକ୍ଷର) * 256)
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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  CURLFNM_SCHS_DEFAULT = 0,
  CURLFNM_SCHS_RIGHTBR,
  CURLFNM_SCHS_RIGHTBRLEFTBR
} setcharset_state;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  CURLFNM_PKW_INIT = 0,
  CURLFNM_PKW_DDOT
} parsekey_state;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  CCLASS_OTHER = 0,
  CCLASS_DIGIT,
  CCLASS_UPPER,
  CCLASS_LOWER
} char_class;

#define SETCHARSET_OK     1
#define SETCHARSET_FAIL   0

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parsekeyword(ଅଚଳ unsigned ଅକ୍ଷର **pattern, unsigned ଅକ୍ଷର *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  ଅକ୍ଷର keyword[10] = { 0 };
  size_t i;
  ଅଚଳ unsigned ଅକ୍ଷର *p = *pattern;
  bool found = FALSE;
  ପାଇଁ(i = 0; !found; i++) {
    ଅକ୍ଷର c = (ଅକ୍ଷର)*p++;
    ଯଦି(i >= ଆକାର(keyword))
      ଫେରନ୍ତୁ SETCHARSET_FAIL;
    ବାଛ(state) {
    ସ୍ଥିତି CURLFNM_PKW_INIT:
      ଯଦି(ISLOWER(c))
        keyword[i] = c;
      ଅନ୍ୟଥା ଯଦି(c == ':')
        state = CURLFNM_PKW_DDOT;
      ଅନ୍ୟଥା
        ଫେରନ୍ତୁ SETCHARSET_FAIL;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURLFNM_PKW_DDOT:
      ଯଦି(c == ']')
        found = TRUE;
      ଅନ୍ୟଥା
        ଫେରନ୍ତୁ SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  ଯଦି(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  ଅନ୍ୟଥା ଯଦି(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  ଅନ୍ୟଥା ଯଦି(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  ଅନ୍ୟଥା ଯଦି(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  ଅନ୍ୟଥା ଯଦି(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  ଅନ୍ୟଥା ଯଦି(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  ଅନ୍ୟଥା ଯଦି(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  ଅନ୍ୟଥା ଯଦି(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  ଅନ୍ୟଥା ଯଦି(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  ଅନ୍ୟଥା ଯଦି(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ SETCHARSET_FAIL;
  ଫେରନ୍ତୁ SETCHARSET_OK;
}

/* Return the character class. */
ସ୍ଥିର char_class charclass(unsigned ଅକ୍ଷର c)
{
  ଯଦି(ISUPPER(c))
    ଫେରନ୍ତୁ CCLASS_UPPER;
  ଯଦି(ISLOWER(c))
    ଫେରନ୍ତୁ CCLASS_LOWER;
  ଯଦି(ISDIGIT(c))
    ଫେରନ୍ତୁ CCLASS_DIGIT;
  ଫେରନ୍ତୁ CCLASS_OTHER;
}

/* Include a character or a range in set. */
ସ୍ଥିର ଶୂନ୍ୟ setcharorrange(ଅଚଳ unsigned ଅକ୍ଷର **pp, unsigned ଅକ୍ଷର *charset)
{
  ଅଚଳ unsigned ଅକ୍ଷର *p = (*pp)++;
  unsigned ଅକ୍ଷର c = *p++;

  charset[c] = 1;
  ଯଦି(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned ଅକ୍ଷର endrange = *p++;

    ଯଦି(endrange == '\\')
      endrange = *p++;
    ଯଦି(endrange >= c && charclass(endrange) == cc) {
      ଯେତେବେଳେ(c++ != endrange)
        ଯଦି(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା setcharset(ଅଚଳ unsigned ଅକ୍ଷର **p, unsigned ଅକ୍ଷର *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  unsigned ଅକ୍ଷର c;

  memset(charset, 0, CURLFNM_CHSET_SIZE);
  ପାଇଁ(;;) {
    c = **p;
    ଯଦି(!c)
      ଫେରନ୍ତୁ SETCHARSET_FAIL;

    ବାଛ(state) {
    ସ୍ଥିତି CURLFNM_SCHS_DEFAULT:
      ଯଦି(c == ']') {
        ଯଦି(something_found)
          ଫେରନ୍ତୁ SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      ଅନ୍ୟଥା ଯଦି(c == '[') {
        ଅଚଳ unsigned ଅକ୍ଷର *pp = *p + 1;

        ଯଦି(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        ଅନ୍ୟଥା {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      ଅନ୍ୟଥା ଯଦି(c == '^' || c == '!') {
        ଯଦି(!something_found) {
          ଯଦି(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          ଅନ୍ୟଥା
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        ଅନ୍ୟଥା
          charset[c] = 1;
        (*p)++;
      }
      ଅନ୍ୟଥା ଯଦି(c == '\\') {
        c = *(++(*p));
        ଯଦି(c)
          setcharorrange(p, charset);
        ଅନ୍ୟଥା
          charset['\\'] = 1;
        something_found = TRUE;
      }
      ଅନ୍ୟଥା {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURLFNM_SCHS_RIGHTBR:
      ଯଦି(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      ଅନ୍ୟଥା ଯଦି(c == ']') {
        ଫେରନ୍ତୁ SETCHARSET_OK;
      }
      ଅନ୍ୟଥା ଯଦି(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      ଅନ୍ୟଥା
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        ଯାଅ fail;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURLFNM_SCHS_RIGHTBRLEFTBR:
      ଯଦି(c == ']')
        ଫେରନ୍ତୁ SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      ଭାଙ୍ଗ;
    }
  }
fail:
  ଫେରନ୍ତୁ SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା matchtoken(ଅଚଳ unsigned ଅକ୍ଷର **pp, ଅଚଳ unsigned ଅକ୍ଷର **sp)
{
  ଅଚଳ unsigned ଅକ୍ଷର *p = *pp;
  ଅଚଳ unsigned ଅକ୍ଷର *s = *sp;

  ବାଛ(*p) {
  ସ୍ଥିତି '?':
    ଭାଙ୍ଗ;
  ସ୍ଥିତି '\\':
    ଯଦି(p[1])
      p++;
    ଯଦି(*s != *p)
      ଫେରନ୍ତୁ 0;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି '[': {
    unsigned ଅକ୍ଷର charset[CURLFNM_CHSET_SIZE];
    ଅଚଳ unsigned ଅକ୍ଷର *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    ଯଦି(!setcharset(&pp2, charset))
      ଫେରନ୍ତୁ -1; /* Syntax error in set; mismatch! */
    ଯଦି(charset[(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*s])
      found = TRUE;
    ଅନ୍ୟଥା ଯଦି(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    ଅନ୍ୟଥା ଯଦି(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    ଅନ୍ୟଥା ଯଦି(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    ଅନ୍ୟଥା ଯଦି(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    ଅନ୍ୟଥା ଯଦି(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    ଅନ୍ୟଥା ଯଦି(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    ଅନ୍ୟଥା ଯଦି(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    ଅନ୍ୟଥା ଯଦି(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    ଅନ୍ୟଥା ଯଦି(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    ଅନ୍ୟଥା ଯଦି(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    ଯଦି(charset[CURLFNM_NEGATE])
      found = !found;

    ଯଦି(!found)
      ଫେରନ୍ତୁ 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    ଫେରନ୍ତୁ 1;
  }
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଯଦି(*p != *s)
      ଫେରନ୍ତୁ 0;
    ଭାଙ୍ଗ;
  }
  *pp = p + 1;
  *sp = s + 1;
  ଫେରନ୍ତୁ 1;
}

/* greedy match with backtracking to the most recent '*' */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା loop(ଅଚଳ unsigned ଅକ୍ଷର *pattern, ଅଚଳ unsigned ଅକ୍ଷର *string)
{
  ଅଚଳ unsigned ଅକ୍ଷର *p = pattern;
  ଅଚଳ unsigned ଅକ୍ଷର *s = string;
  ଅଚଳ unsigned ଅକ୍ଷର *star_p = ଖାଲି;
  ଅଚଳ unsigned ଅକ୍ଷର *star_s = ଖାଲି;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxstars = 2;

  ଯେତେବେଳେ(*s) {
    ଯଦି(*p == '*') {
      ଯଦି(!maxstars)
        ଫେରନ୍ତୁ CURL_FNMATCH_NOMATCH;
      maxstars--;
      ପାଇଁ(;;) {
        p++;
        ଯଦି(!*p)
          ଫେରନ୍ତୁ CURL_FNMATCH_MATCH;
        ଯଦି(*p == '?') {
          ଯଦି(!*s)
            ଫେରନ୍ତୁ CURL_FNMATCH_NOMATCH;
          s++;
        }
        ଅନ୍ୟଥା ଯଦି(*p != '*')
          ଭାଙ୍ଗ;
      }
      star_p = p;
      star_s = s;
    }
    ଅନ୍ୟଥା ଯଦି(*p) {
      ଅଚଳ unsigned ଅକ୍ଷର *np = p;
      ଅଚଳ unsigned ଅକ୍ଷର *ns = s;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = matchtoken(&np, &ns);
      ଯଦି(r < 0)
        ଫେରନ୍ତୁ CURL_FNMATCH_NOMATCH;
      ଯଦି(r) {
        p = np;
        s = ns;
        ଜାରି;
      }
      ଯଦି(!star_p)
        ଫେରନ୍ତୁ CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    ଅନ୍ୟଥା ଯଦି(star_p) {
      p = star_p;
      s = ++star_s;
    }
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ CURL_FNMATCH_NOMATCH;
  }
  ଯେତେବେଳେ(*p == '*') {
    ଯଦି(!maxstars)
      ଫେରନ୍ତୁ CURL_FNMATCH_NOMATCH;
    maxstars--;
    କର {
      p++;
      ଯଦି(*p == '?')
        ଫେରନ୍ତୁ CURL_FNMATCH_NOMATCH;
    } ଯେତେବେଳେ(*p == '*');
  }
  ଫେରନ୍ତୁ *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_fnmatch(ଶୂନ୍ୟ *ptr, ଅଚଳ ଅକ୍ଷର *pattern, ଅଚଳ ଅକ୍ଷର *string)
{
  (ଶୂନ୍ୟ)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ଯଦି(!pattern || !string) {
    ଫେରନ୍ତୁ CURL_FNMATCH_FAIL;
  }
  ଫେରନ୍ତୁ loop((ଅଚଳ unsigned ଅକ୍ଷର *)pattern,
              (ଅଚଳ unsigned ଅକ୍ଷର *)string);
}
#ଅନ୍ୟଥା /* HAVE_FNMATCH */

#ଅନ୍ତର୍ଭାଵ <fnmatch.h>
/*
 * @unittest: 1307
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_fnmatch(ଶୂନ୍ୟ *ptr, ଅଚଳ ଅକ୍ଷର *pattern, ଅଚଳ ଅକ୍ଷର *string)
{
  (ଶୂନ୍ୟ)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ଯଦି(!pattern || !string) {
    ଫେରନ୍ତୁ CURL_FNMATCH_FAIL;
  }

  ବାଛ(fnmatch(pattern, string, 0)) {
  ସ୍ଥିତି 0:
    ଫେରନ୍ତୁ CURL_FNMATCH_MATCH;
  ସ୍ଥିତି FNM_NOMATCH:
    ଫେରନ୍ତୁ CURL_FNMATCH_NOMATCH;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
