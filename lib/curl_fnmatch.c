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

स्थिर पूर्णाङ्क parsekeyword(अचर अचिन्हित वर्ण **pattern, अचिन्हित वर्ण *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  वर्ण keyword[10] = { 0 };
  माप_प्रकार i;
  अचर अचिन्हित वर्ण *p = *pattern;
  bool found = FALSE;
  लागि(i = 0; !found; i++) {
    वर्ण c = (वर्ण)*p++;
    यदि(i >= परिमाण(keyword))
      फर्काऊ SETCHARSET_FAIL;
    विकल्प(state) {
    पक्ष CURLFNM_PKW_INIT:
      यदि(ISLOWER(c))
        keyword[i] = c;
      अन्यथा यदि(c == ':')
        state = CURLFNM_PKW_DDOT;
      अन्यथा
        फर्काऊ SETCHARSET_FAIL;
      रोक;
    पक्ष CURLFNM_PKW_DDOT:
      यदि(c == ']')
        found = TRUE;
      अन्यथा
        फर्काऊ SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  यदि(!म_भेद(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  अन्यथा यदि(!म_भेद(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  अन्यथा यदि(!म_भेद(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  अन्यथा यदि(!म_भेद(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  अन्यथा यदि(!म_भेद(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  अन्यथा यदि(!म_भेद(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  अन्यथा यदि(!म_भेद(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  अन्यथा यदि(!म_भेद(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  अन्यथा यदि(!म_भेद(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  अन्यथा यदि(!म_भेद(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  अन्यथा
    फर्काऊ SETCHARSET_FAIL;
  फर्काऊ SETCHARSET_OK;
}

/* Return the character class. */
स्थिर char_class charclass(अचिन्हित वर्ण c)
{
  यदि(ISUPPER(c))
    फर्काऊ CCLASS_UPPER;
  यदि(ISLOWER(c))
    फर्काऊ CCLASS_LOWER;
  यदि(ISDIGIT(c))
    फर्काऊ CCLASS_DIGIT;
  फर्काऊ CCLASS_OTHER;
}

/* Include a character or a range in set. */
स्थिर शून्य setcharorrange(अचर अचिन्हित वर्ण **pp, अचिन्हित वर्ण *charset)
{
  अचर अचिन्हित वर्ण *p = (*pp)++;
  अचिन्हित वर्ण c = *p++;

  charset[c] = 1;
  यदि(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    अचिन्हित वर्ण endrange = *p++;

    यदि(endrange == '\\')
      endrange = *p++;
    यदि(endrange >= c && charclass(endrange) == cc) {
      जबसम्म(c++ != endrange)
        यदि(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
स्थिर पूर्णाङ्क setcharset(अचर अचिन्हित वर्ण **p, अचिन्हित वर्ण *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  अचिन्हित वर्ण c;

  स_रखो(charset, 0, CURLFNM_CHSET_SIZE);
  लागि(;;) {
    c = **p;
    यदि(!c)
      फर्काऊ SETCHARSET_FAIL;

    विकल्प(state) {
    पक्ष CURLFNM_SCHS_DEFAULT:
      यदि(c == ']') {
        यदि(something_found)
          फर्काऊ SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      अन्यथा यदि(c == '[') {
        अचर अचिन्हित वर्ण *pp = *p + 1;

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
      रोक;
    पक्ष CURLFNM_SCHS_RIGHTBR:
      यदि(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      अन्यथा यदि(c == ']') {
        फर्काऊ SETCHARSET_OK;
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
      रोक;
    पक्ष CURLFNM_SCHS_RIGHTBRLEFTBR:
      यदि(c == ']')
        फर्काऊ SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      रोक;
    }
  }
fail:
  फर्काऊ SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
स्थिर पूर्णाङ्क matchtoken(अचर अचिन्हित वर्ण **pp, अचर अचिन्हित वर्ण **sp)
{
  अचर अचिन्हित वर्ण *p = *pp;
  अचर अचिन्हित वर्ण *s = *sp;

  विकल्प(*p) {
  पक्ष '?':
    रोक;
  पक्ष '\\':
    यदि(p[1])
      p++;
    यदि(*s != *p)
      फर्काऊ 0;
    रोक;
  पक्ष '[': {
    अचिन्हित वर्ण charset[CURLFNM_CHSET_SIZE];
    अचर अचिन्हित वर्ण *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    यदि(!setcharset(&pp2, charset))
      फर्काऊ -1; /* Syntax error in set; mismatch! */
    यदि(charset[(अचिन्हित पूर्णाङ्क)*s])
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
      फर्काऊ 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    फर्काऊ 1;
  }
  स्वभाव:
    यदि(*p != *s)
      फर्काऊ 0;
    रोक;
  }
  *pp = p + 1;
  *sp = s + 1;
  फर्काऊ 1;
}

/* greedy match with backtracking to the most recent '*' */
स्थिर पूर्णाङ्क loop(अचर अचिन्हित वर्ण *pattern, अचर अचिन्हित वर्ण *string)
{
  अचर अचिन्हित वर्ण *p = pattern;
  अचर अचिन्हित वर्ण *s = string;
  अचर अचिन्हित वर्ण *star_p = रिक्त;
  अचर अचिन्हित वर्ण *star_s = रिक्त;
  पूर्णाङ्क maxstars = 2;

  जबसम्म(*s) {
    यदि(*p == '*') {
      यदि(!maxstars)
        फर्काऊ CURL_FNMATCH_NOMATCH;
      maxstars--;
      लागि(;;) {
        p++;
        यदि(!*p)
          फर्काऊ CURL_FNMATCH_MATCH;
        यदि(*p == '?') {
          यदि(!*s)
            फर्काऊ CURL_FNMATCH_NOMATCH;
          s++;
        }
        अन्यथा यदि(*p != '*')
          रोक;
      }
      star_p = p;
      star_s = s;
    }
    अन्यथा यदि(*p) {
      अचर अचिन्हित वर्ण *np = p;
      अचर अचिन्हित वर्ण *ns = s;
      पूर्णाङ्क r = matchtoken(&np, &ns);
      यदि(r < 0)
        फर्काऊ CURL_FNMATCH_NOMATCH;
      यदि(r) {
        p = np;
        s = ns;
        जारी;
      }
      यदि(!star_p)
        फर्काऊ CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    अन्यथा यदि(star_p) {
      p = star_p;
      s = ++star_s;
    }
    अन्यथा
      फर्काऊ CURL_FNMATCH_NOMATCH;
  }
  जबसम्म(*p == '*') {
    यदि(!maxstars)
      फर्काऊ CURL_FNMATCH_NOMATCH;
    maxstars--;
    गर {
      p++;
      यदि(*p == '?')
        फर्काऊ CURL_FNMATCH_NOMATCH;
    } जबसम्म(*p == '*');
  }
  फर्काऊ *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
पूर्णाङ्क Curl_fnmatch(शून्य *ptr, अचर वर्ण *pattern, अचर वर्ण *string)
{
  (शून्य)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  यदि(!pattern || !string) {
    फर्काऊ CURL_FNMATCH_FAIL;
  }
  फर्काऊ loop((अचर अचिन्हित वर्ण *)pattern,
              (अचर अचिन्हित वर्ण *)string);
}
#अन्यथा /* HAVE_FNMATCH */

#अन्तर्भाव <fnmatch.h>
/*
 * @unittest: 1307
 */
पूर्णाङ्क Curl_fnmatch(शून्य *ptr, अचर वर्ण *pattern, अचर वर्ण *string)
{
  (शून्य)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  यदि(!pattern || !string) {
    फर्काऊ CURL_FNMATCH_FAIL;
  }

  विकल्प(fnmatch(pattern, string, 0)) {
  पक्ष 0:
    फर्काऊ CURL_FNMATCH_MATCH;
  पक्ष FNM_NOMATCH:
    फर्काऊ CURL_FNMATCH_NOMATCH;
  स्वभाव:
    फर्काऊ CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
