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
#अन्तर्भाव "curl_setup.h"

#ifndef CURL_DISABLE_FTP

#अन्तर्भाव "curl_fnmatch.h"

#ifndef HAVE_FNMATCH

#define CURLFNM_CHARSET_LEN (परिमाण(वर्ण) * 256)
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

प्रकारनिर्देश गणना {
  CURLFNM_SCHS_DEFAULT = 0,
  CURLFNM_SCHS_RIGHTBR,
  CURLFNM_SCHS_RIGHTBRLEFTBR
} setcharset_state;

प्रकारनिर्देश गणना {
  CURLFNM_PKW_INIT = 0,
  CURLFNM_PKW_DDOT
} parsekey_state;

प्रकारनिर्देश गणना {
  CCLASS_OTHER = 0,
  CCLASS_DIGIT,
  CCLASS_UPPER,
  CCLASS_LOWER
} char_class;

#define SETCHARSET_OK     1
#define SETCHARSET_FAIL   0

स्थिर पूर्णाङ्क parsekeyword(ध्रुव unsigned वर्ण **pattern, unsigned वर्ण *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  वर्ण keyword[10] = { 0 };
  size_t i;
  ध्रुव unsigned वर्ण *p = *pattern;
  bool found = FALSE;
  प्रति(i = 0; !found; i++) {
    वर्ण c = (वर्ण)*p++;
    जइ(i >= परिमाण(keyword))
      प्रत्यागम SETCHARSET_FAIL;
    विकल्प(state) {
    पक्ष CURLFNM_PKW_INIT:
      जइ(ISLOWER(c))
        keyword[i] = c;
      अण्णहा जइ(c == ':')
        state = CURLFNM_PKW_DDOT;
      अण्णहा
        प्रत्यागम SETCHARSET_FAIL;
      भङ्ग;
    पक्ष CURLFNM_PKW_DDOT:
      जइ(c == ']')
        found = TRUE;
      अण्णहा
        प्रत्यागम SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  जइ(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  अण्णहा जइ(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  अण्णहा जइ(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  अण्णहा जइ(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  अण्णहा जइ(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  अण्णहा जइ(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  अण्णहा जइ(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  अण्णहा जइ(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  अण्णहा जइ(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  अण्णहा जइ(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  अण्णहा
    प्रत्यागम SETCHARSET_FAIL;
  प्रत्यागम SETCHARSET_OK;
}

/* Return the character class. */
स्थिर char_class charclass(unsigned वर्ण c)
{
  जइ(ISUPPER(c))
    प्रत्यागम CCLASS_UPPER;
  जइ(ISLOWER(c))
    प्रत्यागम CCLASS_LOWER;
  जइ(ISDIGIT(c))
    प्रत्यागम CCLASS_DIGIT;
  प्रत्यागम CCLASS_OTHER;
}

/* Include a character or a range in set. */
स्थिर शून्य setcharorrange(ध्रुव unsigned वर्ण **pp, unsigned वर्ण *charset)
{
  ध्रुव unsigned वर्ण *p = (*pp)++;
  unsigned वर्ण c = *p++;

  charset[c] = 1;
  जइ(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned वर्ण endrange = *p++;

    जइ(endrange == '\\')
      endrange = *p++;
    जइ(endrange >= c && charclass(endrange) == cc) {
      जाव(c++ != endrange)
        जइ(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
स्थिर पूर्णाङ्क setcharset(ध्रुव unsigned वर्ण **p, unsigned वर्ण *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  unsigned वर्ण c;

  memset(charset, 0, CURLFNM_CHSET_SIZE);
  प्रति(;;) {
    c = **p;
    जइ(!c)
      प्रत्यागम SETCHARSET_FAIL;

    विकल्प(state) {
    पक्ष CURLFNM_SCHS_DEFAULT:
      जइ(c == ']') {
        जइ(something_found)
          प्रत्यागम SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      अण्णहा जइ(c == '[') {
        ध्रुव unsigned वर्ण *pp = *p + 1;

        जइ(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        अण्णहा {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      अण्णहा जइ(c == '^' || c == '!') {
        जइ(!something_found) {
          जइ(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          अण्णहा
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        अण्णहा
          charset[c] = 1;
        (*p)++;
      }
      अण्णहा जइ(c == '\\') {
        c = *(++(*p));
        जइ(c)
          setcharorrange(p, charset);
        अण्णहा
          charset['\\'] = 1;
        something_found = TRUE;
      }
      अण्णहा {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      भङ्ग;
    पक्ष CURLFNM_SCHS_RIGHTBR:
      जइ(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      अण्णहा जइ(c == ']') {
        प्रत्यागम SETCHARSET_OK;
      }
      अण्णहा जइ(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      अण्णहा
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        गच्छ fail;
      भङ्ग;
    पक्ष CURLFNM_SCHS_RIGHTBRLEFTBR:
      जइ(c == ']')
        प्रत्यागम SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      भङ्ग;
    }
  }
fail:
  प्रत्यागम SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
स्थिर पूर्णाङ्क matchtoken(ध्रुव unsigned वर्ण **pp, ध्रुव unsigned वर्ण **sp)
{
  ध्रुव unsigned वर्ण *p = *pp;
  ध्रुव unsigned वर्ण *s = *sp;

  विकल्प(*p) {
  पक्ष '?':
    भङ्ग;
  पक्ष '\\':
    जइ(p[1])
      p++;
    जइ(*s != *p)
      प्रत्यागम 0;
    भङ्ग;
  पक्ष '[': {
    unsigned वर्ण charset[CURLFNM_CHSET_SIZE];
    ध्रुव unsigned वर्ण *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    जइ(!setcharset(&pp2, charset))
      प्रत्यागम -1; /* Syntax error in set; mismatch! */
    जइ(charset[(unsigned पूर्णाङ्क)*s])
      found = TRUE;
    अण्णहा जइ(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    अण्णहा जइ(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    अण्णहा जइ(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    अण्णहा जइ(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    अण्णहा जइ(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    अण्णहा जइ(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    अण्णहा जइ(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    अण्णहा जइ(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    अण्णहा जइ(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    अण्णहा जइ(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    जइ(charset[CURLFNM_NEGATE])
      found = !found;

    जइ(!found)
      प्रत्यागम 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    प्रत्यागम 1;
  }
  स्वभाव:
    जइ(*p != *s)
      प्रत्यागम 0;
    भङ्ग;
  }
  *pp = p + 1;
  *sp = s + 1;
  प्रत्यागम 1;
}

/* greedy match with backtracking to the most recent '*' */
स्थिर पूर्णाङ्क loop(ध्रुव unsigned वर्ण *pattern, ध्रुव unsigned वर्ण *string)
{
  ध्रुव unsigned वर्ण *p = pattern;
  ध्रुव unsigned वर्ण *s = string;
  ध्रुव unsigned वर्ण *star_p = रिक्त;
  ध्रुव unsigned वर्ण *star_s = रिक्त;
  पूर्णाङ्क maxstars = 2;

  जाव(*s) {
    जइ(*p == '*') {
      जइ(!maxstars)
        प्रत्यागम CURL_FNMATCH_NOMATCH;
      maxstars--;
      प्रति(;;) {
        p++;
        जइ(!*p)
          प्रत्यागम CURL_FNMATCH_MATCH;
        जइ(*p == '?') {
          जइ(!*s)
            प्रत्यागम CURL_FNMATCH_NOMATCH;
          s++;
        }
        अण्णहा जइ(*p != '*')
          भङ्ग;
      }
      star_p = p;
      star_s = s;
    }
    अण्णहा जइ(*p) {
      ध्रुव unsigned वर्ण *np = p;
      ध्रुव unsigned वर्ण *ns = s;
      पूर्णाङ्क r = matchtoken(&np, &ns);
      जइ(r < 0)
        प्रत्यागम CURL_FNMATCH_NOMATCH;
      जइ(r) {
        p = np;
        s = ns;
        अनुवर्त;
      }
      जइ(!star_p)
        प्रत्यागम CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    अण्णहा जइ(star_p) {
      p = star_p;
      s = ++star_s;
    }
    अण्णहा
      प्रत्यागम CURL_FNMATCH_NOMATCH;
  }
  जाव(*p == '*') {
    जइ(!maxstars)
      प्रत्यागम CURL_FNMATCH_NOMATCH;
    maxstars--;
    करेहि {
      p++;
      जइ(*p == '?')
        प्रत्यागम CURL_FNMATCH_NOMATCH;
    } जाव(*p == '*');
  }
  प्रत्यागम *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
पूर्णाङ्क Curl_fnmatch(शून्य *ptr, ध्रुव वर्ण *pattern, ध्रुव वर्ण *string)
{
  (शून्य)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  जइ(!pattern || !string) {
    प्रत्यागम CURL_FNMATCH_FAIL;
  }
  प्रत्यागम loop((ध्रुव unsigned वर्ण *)pattern,
              (ध्रुव unsigned वर्ण *)string);
}
#अण्णहा /* HAVE_FNMATCH */

#अन्तर्भाव <fnmatch.h>
/*
 * @unittest: 1307
 */
पूर्णाङ्क Curl_fnmatch(शून्य *ptr, ध्रुव वर्ण *pattern, ध्रुव वर्ण *string)
{
  (शून्य)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  जइ(!pattern || !string) {
    प्रत्यागम CURL_FNMATCH_FAIL;
  }

  विकल्प(fnmatch(pattern, string, 0)) {
  पक्ष 0:
    प्रत्यागम CURL_FNMATCH_MATCH;
  पक्ष FNM_NOMATCH:
    प्रत्यागम CURL_FNMATCH_NOMATCH;
  स्वभाव:
    प्रत्यागम CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
