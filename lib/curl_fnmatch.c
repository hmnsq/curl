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

#define CURLFNM_CHARSET_LEN (आकार(अक्षर) * 256)
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

प्रकारव्याख्या गणना {
  CURLFNM_SCHS_DEFAULT = 0,
  CURLFNM_SCHS_RIGHTBR,
  CURLFNM_SCHS_RIGHTBRLEFTBR
} setcharset_state;

प्रकारव्याख्या गणना {
  CURLFNM_PKW_INIT = 0,
  CURLFNM_PKW_DDOT
} parsekey_state;

प्रकारव्याख्या गणना {
  CCLASS_OTHER = 0,
  CCLASS_DIGIT,
  CCLASS_UPPER,
  CCLASS_LOWER
} char_class;

#define SETCHARSET_OK     1
#define SETCHARSET_FAIL   0

स्थिर पूर्णांक parsekeyword(स्थिरांक अचिन्हित अक्षर **pattern, अचिन्हित अक्षर *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  अक्षर keyword[10] = { 0 };
  माप_प्रकार i;
  स्थिरांक अचिन्हित अक्षर *p = *pattern;
  bool found = FALSE;
  साठी(i = 0; !found; i++) {
    अक्षर c = (अक्षर)*p++;
    जर(i >= आकार(keyword))
      परत SETCHARSET_FAIL;
    निवड(state) {
    स्थिती CURLFNM_PKW_INIT:
      जर(ISLOWER(c))
        keyword[i] = c;
      अन्यथा जर(c == ':')
        state = CURLFNM_PKW_DDOT;
      अन्यथा
        परत SETCHARSET_FAIL;
      थांबा;
    स्थिती CURLFNM_PKW_DDOT:
      जर(c == ']')
        found = TRUE;
      अन्यथा
        परत SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  जर(!म_भेद(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  अन्यथा जर(!म_भेद(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  अन्यथा जर(!म_भेद(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  अन्यथा जर(!म_भेद(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  अन्यथा जर(!म_भेद(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  अन्यथा जर(!म_भेद(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  अन्यथा जर(!म_भेद(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  अन्यथा जर(!म_भेद(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  अन्यथा जर(!म_भेद(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  अन्यथा जर(!म_भेद(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  अन्यथा
    परत SETCHARSET_FAIL;
  परत SETCHARSET_OK;
}

/* Return the character class. */
स्थिर char_class charclass(अचिन्हित अक्षर c)
{
  जर(ISUPPER(c))
    परत CCLASS_UPPER;
  जर(ISLOWER(c))
    परत CCLASS_LOWER;
  जर(ISDIGIT(c))
    परत CCLASS_DIGIT;
  परत CCLASS_OTHER;
}

/* Include a character or a range in set. */
स्थिर शून्य setcharorrange(स्थिरांक अचिन्हित अक्षर **pp, अचिन्हित अक्षर *charset)
{
  स्थिरांक अचिन्हित अक्षर *p = (*pp)++;
  अचिन्हित अक्षर c = *p++;

  charset[c] = 1;
  जर(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    अचिन्हित अक्षर endrange = *p++;

    जर(endrange == '\\')
      endrange = *p++;
    जर(endrange >= c && charclass(endrange) == cc) {
      पर्यंत(c++ != endrange)
        जर(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
स्थिर पूर्णांक setcharset(स्थिरांक अचिन्हित अक्षर **p, अचिन्हित अक्षर *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  अचिन्हित अक्षर c;

  स_रखो(charset, 0, CURLFNM_CHSET_SIZE);
  साठी(;;) {
    c = **p;
    जर(!c)
      परत SETCHARSET_FAIL;

    निवड(state) {
    स्थिती CURLFNM_SCHS_DEFAULT:
      जर(c == ']') {
        जर(something_found)
          परत SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      अन्यथा जर(c == '[') {
        स्थिरांक अचिन्हित अक्षर *pp = *p + 1;

        जर(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        अन्यथा {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      अन्यथा जर(c == '^' || c == '!') {
        जर(!something_found) {
          जर(charset[CURLFNM_NEGATE]) {
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
      अन्यथा जर(c == '\\') {
        c = *(++(*p));
        जर(c)
          setcharorrange(p, charset);
        अन्यथा
          charset['\\'] = 1;
        something_found = TRUE;
      }
      अन्यथा {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      थांबा;
    स्थिती CURLFNM_SCHS_RIGHTBR:
      जर(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      अन्यथा जर(c == ']') {
        परत SETCHARSET_OK;
      }
      अन्यथा जर(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      अन्यथा
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        जा fail;
      थांबा;
    स्थिती CURLFNM_SCHS_RIGHTBRLEFTBR:
      जर(c == ']')
        परत SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      थांबा;
    }
  }
fail:
  परत SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
स्थिर पूर्णांक matchtoken(स्थिरांक अचिन्हित अक्षर **pp, स्थिरांक अचिन्हित अक्षर **sp)
{
  स्थिरांक अचिन्हित अक्षर *p = *pp;
  स्थिरांक अचिन्हित अक्षर *s = *sp;

  निवड(*p) {
  स्थिती '?':
    थांबा;
  स्थिती '\\':
    जर(p[1])
      p++;
    जर(*s != *p)
      परत 0;
    थांबा;
  स्थिती '[': {
    अचिन्हित अक्षर charset[CURLFNM_CHSET_SIZE];
    स्थिरांक अचिन्हित अक्षर *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    जर(!setcharset(&pp2, charset))
      परत -1; /* Syntax error in set; mismatch! */
    जर(charset[(अचिन्हित पूर्णांक)*s])
      found = TRUE;
    अन्यथा जर(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    अन्यथा जर(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    अन्यथा जर(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    अन्यथा जर(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    अन्यथा जर(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    अन्यथा जर(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    अन्यथा जर(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    अन्यथा जर(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    अन्यथा जर(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    अन्यथा जर(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    जर(charset[CURLFNM_NEGATE])
      found = !found;

    जर(!found)
      परत 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    परत 1;
  }
  पूर्वनिर्धारित:
    जर(*p != *s)
      परत 0;
    थांबा;
  }
  *pp = p + 1;
  *sp = s + 1;
  परत 1;
}

/* greedy match with backtracking to the most recent '*' */
स्थिर पूर्णांक loop(स्थिरांक अचिन्हित अक्षर *pattern, स्थिरांक अचिन्हित अक्षर *string)
{
  स्थिरांक अचिन्हित अक्षर *p = pattern;
  स्थिरांक अचिन्हित अक्षर *s = string;
  स्थिरांक अचिन्हित अक्षर *star_p = रिक्त;
  स्थिरांक अचिन्हित अक्षर *star_s = रिक्त;
  पूर्णांक maxstars = 2;

  पर्यंत(*s) {
    जर(*p == '*') {
      जर(!maxstars)
        परत CURL_FNMATCH_NOMATCH;
      maxstars--;
      साठी(;;) {
        p++;
        जर(!*p)
          परत CURL_FNMATCH_MATCH;
        जर(*p == '?') {
          जर(!*s)
            परत CURL_FNMATCH_NOMATCH;
          s++;
        }
        अन्यथा जर(*p != '*')
          थांबा;
      }
      star_p = p;
      star_s = s;
    }
    अन्यथा जर(*p) {
      स्थिरांक अचिन्हित अक्षर *np = p;
      स्थिरांक अचिन्हित अक्षर *ns = s;
      पूर्णांक r = matchtoken(&np, &ns);
      जर(r < 0)
        परत CURL_FNMATCH_NOMATCH;
      जर(r) {
        p = np;
        s = ns;
        सुरूठेवा;
      }
      जर(!star_p)
        परत CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    अन्यथा जर(star_p) {
      p = star_p;
      s = ++star_s;
    }
    अन्यथा
      परत CURL_FNMATCH_NOMATCH;
  }
  पर्यंत(*p == '*') {
    जर(!maxstars)
      परत CURL_FNMATCH_NOMATCH;
    maxstars--;
    करा {
      p++;
      जर(*p == '?')
        परत CURL_FNMATCH_NOMATCH;
    } पर्यंत(*p == '*');
  }
  परत *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
पूर्णांक Curl_fnmatch(शून्य *ptr, स्थिरांक अक्षर *pattern, स्थिरांक अक्षर *string)
{
  (शून्य)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  जर(!pattern || !string) {
    परत CURL_FNMATCH_FAIL;
  }
  परत loop((स्थिरांक अचिन्हित अक्षर *)pattern,
              (स्थिरांक अचिन्हित अक्षर *)string);
}
#अन्यथा /* HAVE_FNMATCH */

#अन्तर्भाव <fnmatch.h>
/*
 * @unittest: 1307
 */
पूर्णांक Curl_fnmatch(शून्य *ptr, स्थिरांक अक्षर *pattern, स्थिरांक अक्षर *string)
{
  (शून्य)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  जर(!pattern || !string) {
    परत CURL_FNMATCH_FAIL;
  }

  निवड(fnmatch(pattern, string, 0)) {
  स्थिती 0:
    परत CURL_FNMATCH_MATCH;
  स्थिती FNM_NOMATCH:
    परत CURL_FNMATCH_NOMATCH;
  पूर्वनिर्धारित:
    परत CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
