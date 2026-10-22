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
    यदि(i >= परिमाण(keyword))
      प्रत्यागम SETCHARSET_FAIL;
    विकल्प(state) {
    पक्ष CURLFNM_PKW_INIT:
      यदि(ISLOWER(c))
        keyword[i] = c;
      अन्यथा यदि(c == ':')
        state = CURLFNM_PKW_DDOT;
      अन्यथा
        प्रत्यागम SETCHARSET_FAIL;
      भङ्ग;
    पक्ष CURLFNM_PKW_DDOT:
      यदि(c == ']')
        found = TRUE;
      अन्यथा
        प्रत्यागम SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  यदि(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  अन्यथा यदि(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  अन्यथा यदि(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  अन्यथा यदि(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  अन्यथा यदि(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  अन्यथा यदि(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  अन्यथा यदि(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  अन्यथा यदि(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  अन्यथा यदि(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  अन्यथा यदि(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  अन्यथा
    प्रत्यागम SETCHARSET_FAIL;
  प्रत्यागम SETCHARSET_OK;
}

/* Return the character class. */
स्थिर char_class charclass(unsigned वर्ण c)
{
  यदि(ISUPPER(c))
    प्रत्यागम CCLASS_UPPER;
  यदि(ISLOWER(c))
    प्रत्यागम CCLASS_LOWER;
  यदि(ISDIGIT(c))
    प्रत्यागम CCLASS_DIGIT;
  प्रत्यागम CCLASS_OTHER;
}

/* Include a character or a range in set. */
स्थिर शून्य setcharorrange(ध्रुव unsigned वर्ण **pp, unsigned वर्ण *charset)
{
  ध्रुव unsigned वर्ण *p = (*pp)++;
  unsigned वर्ण c = *p++;

  charset[c] = 1;
  यदि(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned वर्ण endrange = *p++;

    यदि(endrange == '\\')
      endrange = *p++;
    यदि(endrange >= c && charclass(endrange) == cc) {
      यावत्(c++ != endrange)
        यदि(charclass(c) == cc)  /* Chars in class may be not consecutive. */
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
    यदि(!c)
      प्रत्यागम SETCHARSET_FAIL;

    विकल्प(state) {
    पक्ष CURLFNM_SCHS_DEFAULT:
      यदि(c == ']') {
        यदि(something_found)
          प्रत्यागम SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      अन्यथा यदि(c == '[') {
        ध्रुव unsigned वर्ण *pp = *p + 1;

        यदि(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        अन्यथा {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      अन्यथा यदि(c == '^' || c == '!') {
        यदि(!something_found) {
          यदि(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          अन्यथा
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        अन्यथा
          charset[c] = 1;
        (*p)++;
      }
      अन्यथा यदि(c == '\\') {
        c = *(++(*p));
        यदि(c)
          setcharorrange(p, charset);
        अन्यथा
          charset['\\'] = 1;
        something_found = TRUE;
      }
      अन्यथा {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      भङ्ग;
    पक्ष CURLFNM_SCHS_RIGHTBR:
      यदि(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      अन्यथा यदि(c == ']') {
        प्रत्यागम SETCHARSET_OK;
      }
      अन्यथा यदि(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      अन्यथा
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        गच्छ fail;
      भङ्ग;
    पक्ष CURLFNM_SCHS_RIGHTBRLEFTBR:
      यदि(c == ']')
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
    यदि(p[1])
      p++;
    यदि(*s != *p)
      प्रत्यागम 0;
    भङ्ग;
  पक्ष '[': {
    unsigned वर्ण charset[CURLFNM_CHSET_SIZE];
    ध्रुव unsigned वर्ण *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    यदि(!setcharset(&pp2, charset))
      प्रत्यागम -1; /* Syntax error in set; mismatch! */
    यदि(charset[(unsigned पूर्णाङ्क)*s])
      found = TRUE;
    अन्यथा यदि(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    अन्यथा यदि(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    अन्यथा यदि(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    अन्यथा यदि(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    अन्यथा यदि(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    अन्यथा यदि(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    अन्यथा यदि(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    अन्यथा यदि(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    अन्यथा यदि(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    अन्यथा यदि(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    यदि(charset[CURLFNM_NEGATE])
      found = !found;

    यदि(!found)
      प्रत्यागम 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    प्रत्यागम 1;
  }
  स्वभाव:
    यदि(*p != *s)
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

  यावत्(*s) {
    यदि(*p == '*') {
      यदि(!maxstars)
        प्रत्यागम CURL_FNMATCH_NOMATCH;
      maxstars--;
      प्रति(;;) {
        p++;
        यदि(!*p)
          प्रत्यागम CURL_FNMATCH_MATCH;
        यदि(*p == '?') {
          यदि(!*s)
            प्रत्यागम CURL_FNMATCH_NOMATCH;
          s++;
        }
        अन्यथा यदि(*p != '*')
          भङ्ग;
      }
      star_p = p;
      star_s = s;
    }
    अन्यथा यदि(*p) {
      ध्रुव unsigned वर्ण *np = p;
      ध्रुव unsigned वर्ण *ns = s;
      पूर्णाङ्क r = matchtoken(&np, &ns);
      यदि(r < 0)
        प्रत्यागम CURL_FNMATCH_NOMATCH;
      यदि(r) {
        p = np;
        s = ns;
        अनुवर्त;
      }
      यदि(!star_p)
        प्रत्यागम CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    अन्यथा यदि(star_p) {
      p = star_p;
      s = ++star_s;
    }
    अन्यथा
      प्रत्यागम CURL_FNMATCH_NOMATCH;
  }
  यावत्(*p == '*') {
    यदि(!maxstars)
      प्रत्यागम CURL_FNMATCH_NOMATCH;
    maxstars--;
    कुरु {
      p++;
      यदि(*p == '?')
        प्रत्यागम CURL_FNMATCH_NOMATCH;
    } यावत्(*p == '*');
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
  यदि(!pattern || !string) {
    प्रत्यागम CURL_FNMATCH_FAIL;
  }
  प्रत्यागम loop((ध्रुव unsigned वर्ण *)pattern,
              (ध्रुव unsigned वर्ण *)string);
}
#अन्यथा /* HAVE_FNMATCH */

#अन्तर्भाव <fnmatch.h>
/*
 * @unittest: 1307
 */
पूर्णाङ्क Curl_fnmatch(शून्य *ptr, ध्रुव वर्ण *pattern, ध्रुव वर्ण *string)
{
  (शून्य)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  यदि(!pattern || !string) {
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
