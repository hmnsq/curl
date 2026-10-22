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
  अत्थाय(i = 0; !found; i++) {
    वर्ण c = (वर्ण)*p++;
    सचे(i >= परिमाण(keyword))
      पटिनिवत्त SETCHARSET_FAIL;
    विकल्प(state) {
    पक्ष CURLFNM_PKW_INIT:
      सचे(ISLOWER(c))
        keyword[i] = c;
      अञ्ञथा सचे(c == ':')
        state = CURLFNM_PKW_DDOT;
      अञ्ञथा
        पटिनिवत्त SETCHARSET_FAIL;
      भङ्ग;
    पक्ष CURLFNM_PKW_DDOT:
      सचे(c == ']')
        found = TRUE;
      अञ्ञथा
        पटिनिवत्त SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  सचे(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  अञ्ञथा सचे(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  अञ्ञथा सचे(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  अञ्ञथा सचे(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  अञ्ञथा सचे(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  अञ्ञथा सचे(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  अञ्ञथा सचे(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  अञ्ञथा सचे(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  अञ्ञथा सचे(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  अञ्ञथा सचे(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  अञ्ञथा
    पटिनिवत्त SETCHARSET_FAIL;
  पटिनिवत्त SETCHARSET_OK;
}

/* Return the character class. */
स्थिर char_class charclass(unsigned वर्ण c)
{
  सचे(ISUPPER(c))
    पटिनिवत्त CCLASS_UPPER;
  सचे(ISLOWER(c))
    पटिनिवत्त CCLASS_LOWER;
  सचे(ISDIGIT(c))
    पटिनिवत्त CCLASS_DIGIT;
  पटिनिवत्त CCLASS_OTHER;
}

/* Include a character or a range in set. */
स्थिर शून्य setcharorrange(ध्रुव unsigned वर्ण **pp, unsigned वर्ण *charset)
{
  ध्रुव unsigned वर्ण *p = (*pp)++;
  unsigned वर्ण c = *p++;

  charset[c] = 1;
  सचे(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned वर्ण endrange = *p++;

    सचे(endrange == '\\')
      endrange = *p++;
    सचे(endrange >= c && charclass(endrange) == cc) {
      याव(c++ != endrange)
        सचे(charclass(c) == cc)  /* Chars in class may be not consecutive. */
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
  अत्थाय(;;) {
    c = **p;
    सचे(!c)
      पटिनिवत्त SETCHARSET_FAIL;

    विकल्प(state) {
    पक्ष CURLFNM_SCHS_DEFAULT:
      सचे(c == ']') {
        सचे(something_found)
          पटिनिवत्त SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      अञ्ञथा सचे(c == '[') {
        ध्रुव unsigned वर्ण *pp = *p + 1;

        सचे(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        अञ्ञथा {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      अञ्ञथा सचे(c == '^' || c == '!') {
        सचे(!something_found) {
          सचे(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          अञ्ञथा
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        अञ्ञथा
          charset[c] = 1;
        (*p)++;
      }
      अञ्ञथा सचे(c == '\\') {
        c = *(++(*p));
        सचे(c)
          setcharorrange(p, charset);
        अञ्ञथा
          charset['\\'] = 1;
        something_found = TRUE;
      }
      अञ्ञथा {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      भङ्ग;
    पक्ष CURLFNM_SCHS_RIGHTBR:
      सचे(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      अञ्ञथा सचे(c == ']') {
        पटिनिवत्त SETCHARSET_OK;
      }
      अञ्ञथा सचे(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      अञ्ञथा
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        गच्छ fail;
      भङ्ग;
    पक्ष CURLFNM_SCHS_RIGHTBRLEFTBR:
      सचे(c == ']')
        पटिनिवत्त SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      भङ्ग;
    }
  }
fail:
  पटिनिवत्त SETCHARSET_FAIL;
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
    सचे(p[1])
      p++;
    सचे(*s != *p)
      पटिनिवत्त 0;
    भङ्ग;
  पक्ष '[': {
    unsigned वर्ण charset[CURLFNM_CHSET_SIZE];
    ध्रुव unsigned वर्ण *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    सचे(!setcharset(&pp2, charset))
      पटिनिवत्त -1; /* Syntax error in set; mismatch! */
    सचे(charset[(unsigned पूर्णाङ्क)*s])
      found = TRUE;
    अञ्ञथा सचे(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    अञ्ञथा सचे(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    अञ्ञथा सचे(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    अञ्ञथा सचे(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    अञ्ञथा सचे(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    अञ्ञथा सचे(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    अञ्ञथा सचे(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    अञ्ञथा सचे(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    अञ्ञथा सचे(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    अञ्ञथा सचे(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    सचे(charset[CURLFNM_NEGATE])
      found = !found;

    सचे(!found)
      पटिनिवत्त 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    पटिनिवत्त 1;
  }
  स्वभाव:
    सचे(*p != *s)
      पटिनिवत्त 0;
    भङ्ग;
  }
  *pp = p + 1;
  *sp = s + 1;
  पटिनिवत्त 1;
}

/* greedy match with backtracking to the most recent '*' */
स्थिर पूर्णाङ्क loop(ध्रुव unsigned वर्ण *pattern, ध्रुव unsigned वर्ण *string)
{
  ध्रुव unsigned वर्ण *p = pattern;
  ध्रुव unsigned वर्ण *s = string;
  ध्रुव unsigned वर्ण *star_p = रिक्त;
  ध्रुव unsigned वर्ण *star_s = रिक्त;
  पूर्णाङ्क maxstars = 2;

  याव(*s) {
    सचे(*p == '*') {
      सचे(!maxstars)
        पटिनिवत्त CURL_FNMATCH_NOMATCH;
      maxstars--;
      अत्थाय(;;) {
        p++;
        सचे(!*p)
          पटिनिवत्त CURL_FNMATCH_MATCH;
        सचे(*p == '?') {
          सचे(!*s)
            पटिनिवत्त CURL_FNMATCH_NOMATCH;
          s++;
        }
        अञ्ञथा सचे(*p != '*')
          भङ्ग;
      }
      star_p = p;
      star_s = s;
    }
    अञ्ञथा सचे(*p) {
      ध्रुव unsigned वर्ण *np = p;
      ध्रुव unsigned वर्ण *ns = s;
      पूर्णाङ्क r = matchtoken(&np, &ns);
      सचे(r < 0)
        पटिनिवत्त CURL_FNMATCH_NOMATCH;
      सचे(r) {
        p = np;
        s = ns;
        अनुवर्त;
      }
      सचे(!star_p)
        पटिनिवत्त CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    अञ्ञथा सचे(star_p) {
      p = star_p;
      s = ++star_s;
    }
    अञ्ञथा
      पटिनिवत्त CURL_FNMATCH_NOMATCH;
  }
  याव(*p == '*') {
    सचे(!maxstars)
      पटिनिवत्त CURL_FNMATCH_NOMATCH;
    maxstars--;
    करोहि {
      p++;
      सचे(*p == '?')
        पटिनिवत्त CURL_FNMATCH_NOMATCH;
    } याव(*p == '*');
  }
  पटिनिवत्त *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
पूर्णाङ्क Curl_fnmatch(शून्य *ptr, ध्रुव वर्ण *pattern, ध्रुव वर्ण *string)
{
  (शून्य)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  सचे(!pattern || !string) {
    पटिनिवत्त CURL_FNMATCH_FAIL;
  }
  पटिनिवत्त loop((ध्रुव unsigned वर्ण *)pattern,
              (ध्रुव unsigned वर्ण *)string);
}
#अञ्ञथा /* HAVE_FNMATCH */

#अन्तर्भाव <fnmatch.h>
/*
 * @unittest: 1307
 */
पूर्णाङ्क Curl_fnmatch(शून्य *ptr, ध्रुव वर्ण *pattern, ध्रुव वर्ण *string)
{
  (शून्य)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  सचे(!pattern || !string) {
    पटिनिवत्त CURL_FNMATCH_FAIL;
  }

  विकल्प(fnmatch(pattern, string, 0)) {
  पक्ष 0:
    पटिनिवत्त CURL_FNMATCH_MATCH;
  पक्ष FNM_NOMATCH:
    पटिनिवत्त CURL_FNMATCH_NOMATCH;
  स्वभाव:
    पटिनिवत्त CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
