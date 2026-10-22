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
#समावेश "curl_setup.h"

#अगर_अघोषित CURL_DISABLE_FTP

#समावेश "curl_fnmatch.h"

#अगर_अघोषित HAVE_FNMATCH

#घोषणा CURLFNM_CHARSET_LEN (माप(अक्षर) * 256)
#घोषणा CURLFNM_CHSET_आकार (CURLFNM_CHARSET_LEN + 15)

#घोषणा CURLFNM_NEGATE  CURLFNM_CHARSET_LEN

#घोषणा CURLFNM_ALNUM   (CURLFNM_CHARSET_LEN + 1)
#घोषणा CURLFNM_DIGIT   (CURLFNM_CHARSET_LEN + 2)
#घोषणा CURLFNM_XDIGIT  (CURLFNM_CHARSET_LEN + 3)
#घोषणा CURLFNM_ALPHA   (CURLFNM_CHARSET_LEN + 4)
#घोषणा CURLFNM_PRINT   (CURLFNM_CHARSET_LEN + 5)
#घोषणा CURLFNM_BLANK   (CURLFNM_CHARSET_LEN + 6)
#घोषणा CURLFNM_LOWER   (CURLFNM_CHARSET_LEN + 7)
#घोषणा CURLFNM_GRAPH   (CURLFNM_CHARSET_LEN + 8)
#घोषणा CURLFNM_SPACE   (CURLFNM_CHARSET_LEN + 9)
#घोषणा CURLFNM_UPPER   (CURLFNM_CHARSET_LEN + 10)

प्रकार क्रमागत {
  CURLFNM_SCHS_DEFAULT = 0,
  CURLFNM_SCHS_RIGHTBR,
  CURLFNM_SCHS_RIGHTBRLEFTBR
} setcharset_state;

प्रकार क्रमागत {
  CURLFNM_PKW_INIT = 0,
  CURLFNM_PKW_DDOT
} parsekey_state;

प्रकार क्रमागत {
  CCLASS_OTHER = 0,
  CCLASS_DIGIT,
  CCLASS_UPPER,
  CCLASS_LOWER
} char_class;

#घोषणा SETCHARSET_OK     1
#घोषणा SETCHARSET_FAIL   0

अटल पूर्णांक parsekeyword(स्थिर अचिन्हित अक्षर **pattern, अचिन्हित अक्षर *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  अक्षर keyword[10] = { 0 };
  माप_प्रकार i;
  स्थिर अचिन्हित अक्षर *p = *pattern;
  bool found = FALSE;
  क्रम(i = 0; !found; i++) {
    अक्षर c = (अक्षर)*p++;
    अगर(i >= माप(keyword))
      वापस SETCHARSET_FAIL;
    चयन(state) {
    हाल CURLFNM_PKW_INIT:
      अगर(ISLOWER(c))
        keyword[i] = c;
      अन्यथा अगर(c == ':')
        state = CURLFNM_PKW_DDOT;
      अन्यथा
        वापस SETCHARSET_FAIL;
      अवरोध;
    हाल CURLFNM_PKW_DDOT:
      अगर(c == ']')
        found = TRUE;
      अन्यथा
        वापस SETCHARSET_FAIL;
    }
  }
#अघोषित KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  अगर(!म_भेद(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  अन्यथा अगर(!म_भेद(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  अन्यथा अगर(!म_भेद(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  अन्यथा अगर(!म_भेद(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  अन्यथा अगर(!म_भेद(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  अन्यथा अगर(!म_भेद(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  अन्यथा अगर(!म_भेद(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  अन्यथा अगर(!म_भेद(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  अन्यथा अगर(!म_भेद(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  अन्यथा अगर(!म_भेद(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  अन्यथा
    वापस SETCHARSET_FAIL;
  वापस SETCHARSET_OK;
}

/* Return the character class. */
अटल char_class charclass(अचिन्हित अक्षर c)
{
  अगर(ISUPPER(c))
    वापस CCLASS_UPPER;
  अगर(ISLOWER(c))
    वापस CCLASS_LOWER;
  अगर(ISDIGIT(c))
    वापस CCLASS_DIGIT;
  वापस CCLASS_OTHER;
}

/* Include a character or a range in set. */
अटल व्योम setcharorrange(स्थिर अचिन्हित अक्षर **pp, अचिन्हित अक्षर *charset)
{
  स्थिर अचिन्हित अक्षर *p = (*pp)++;
  अचिन्हित अक्षर c = *p++;

  charset[c] = 1;
  अगर(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    अचिन्हित अक्षर endrange = *p++;

    अगर(endrange == '\\')
      endrange = *p++;
    अगर(endrange >= c && charclass(endrange) == cc) {
      जबतक(c++ != endrange)
        अगर(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
अटल पूर्णांक setcharset(स्थिर अचिन्हित अक्षर **p, अचिन्हित अक्षर *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  अचिन्हित अक्षर c;

  स_रखो(charset, 0, CURLFNM_CHSET_आकार);
  क्रम(;;) {
    c = **p;
    अगर(!c)
      वापस SETCHARSET_FAIL;

    चयन(state) {
    हाल CURLFNM_SCHS_DEFAULT:
      अगर(c == ']') {
        अगर(something_found)
          वापस SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      अन्यथा अगर(c == '[') {
        स्थिर अचिन्हित अक्षर *pp = *p + 1;

        अगर(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        अन्यथा {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      अन्यथा अगर(c == '^' || c == '!') {
        अगर(!something_found) {
          अगर(charset[CURLFNM_NEGATE]) {
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
      अन्यथा अगर(c == '\\') {
        c = *(++(*p));
        अगर(c)
          setcharorrange(p, charset);
        अन्यथा
          charset['\\'] = 1;
        something_found = TRUE;
      }
      अन्यथा {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      अवरोध;
    हाल CURLFNM_SCHS_RIGHTBR:
      अगर(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      अन्यथा अगर(c == ']') {
        वापस SETCHARSET_OK;
      }
      अन्यथा अगर(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      अन्यथा
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        जाओ fail;
      अवरोध;
    हाल CURLFNM_SCHS_RIGHTBRLEFTBR:
      अगर(c == ']')
        वापस SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      अवरोध;
    }
  }
fail:
  वापस SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
अटल पूर्णांक matchtoken(स्थिर अचिन्हित अक्षर **pp, स्थिर अचिन्हित अक्षर **sp)
{
  स्थिर अचिन्हित अक्षर *p = *pp;
  स्थिर अचिन्हित अक्षर *s = *sp;

  चयन(*p) {
  हाल '?':
    अवरोध;
  हाल '\\':
    अगर(p[1])
      p++;
    अगर(*s != *p)
      वापस 0;
    अवरोध;
  हाल '[': {
    अचिन्हित अक्षर charset[CURLFNM_CHSET_आकार];
    स्थिर अचिन्हित अक्षर *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    अगर(!setcharset(&pp2, charset))
      वापस -1; /* Syntax error in set; mismatch! */
    अगर(charset[(अचिन्हित पूर्णांक)*s])
      found = TRUE;
    अन्यथा अगर(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    अन्यथा अगर(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    अन्यथा अगर(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    अन्यथा अगर(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    अन्यथा अगर(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    अन्यथा अगर(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    अन्यथा अगर(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    अन्यथा अगर(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    अन्यथा अगर(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    अन्यथा अगर(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    अगर(charset[CURLFNM_NEGATE])
      found = !found;

    अगर(!found)
      वापस 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    वापस 1;
  }
  शेष:
    अगर(*p != *s)
      वापस 0;
    अवरोध;
  }
  *pp = p + 1;
  *sp = s + 1;
  वापस 1;
}

/* greedy match with backtracking to the most recent '*' */
अटल पूर्णांक loop(स्थिर अचिन्हित अक्षर *pattern, स्थिर अचिन्हित अक्षर *string)
{
  स्थिर अचिन्हित अक्षर *p = pattern;
  स्थिर अचिन्हित अक्षर *s = string;
  स्थिर अचिन्हित अक्षर *star_p = शून्य;
  स्थिर अचिन्हित अक्षर *star_s = शून्य;
  पूर्णांक maxstars = 2;

  जबतक(*s) {
    अगर(*p == '*') {
      अगर(!maxstars)
        वापस CURL_FNMATCH_NOMATCH;
      maxstars--;
      क्रम(;;) {
        p++;
        अगर(!*p)
          वापस CURL_FNMATCH_MATCH;
        अगर(*p == '?') {
          अगर(!*s)
            वापस CURL_FNMATCH_NOMATCH;
          s++;
        }
        अन्यथा अगर(*p != '*')
          अवरोध;
      }
      star_p = p;
      star_s = s;
    }
    अन्यथा अगर(*p) {
      स्थिर अचिन्हित अक्षर *np = p;
      स्थिर अचिन्हित अक्षर *ns = s;
      पूर्णांक r = matchtoken(&np, &ns);
      अगर(r < 0)
        वापस CURL_FNMATCH_NOMATCH;
      अगर(r) {
        p = np;
        s = ns;
        जारी;
      }
      अगर(!star_p)
        वापस CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    अन्यथा अगर(star_p) {
      p = star_p;
      s = ++star_s;
    }
    अन्यथा
      वापस CURL_FNMATCH_NOMATCH;
  }
  जबतक(*p == '*') {
    अगर(!maxstars)
      वापस CURL_FNMATCH_NOMATCH;
    maxstars--;
    करो {
      p++;
      अगर(*p == '?')
        वापस CURL_FNMATCH_NOMATCH;
    } जबतक(*p == '*');
  }
  वापस *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
पूर्णांक Curl_fnmatch(व्योम *ptr, स्थिर अक्षर *pattern, स्थिर अक्षर *string)
{
  (व्योम)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  अगर(!pattern || !string) {
    वापस CURL_FNMATCH_FAIL;
  }
  वापस loop((स्थिर अचिन्हित अक्षर *)pattern,
              (स्थिर अचिन्हित अक्षर *)string);
}
#अन्यथा /* HAVE_FNMATCH */

#समावेश <fnmatch.h>
/*
 * @unittest: 1307
 */
पूर्णांक Curl_fnmatch(व्योम *ptr, स्थिर अक्षर *pattern, स्थिर अक्षर *string)
{
  (व्योम)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  अगर(!pattern || !string) {
    वापस CURL_FNMATCH_FAIL;
  }

  चयन(fnmatch(pattern, string, 0)) {
  हाल 0:
    वापस CURL_FNMATCH_MATCH;
  हाल FNM_NOMATCH:
    वापस CURL_FNMATCH_NOMATCH;
  शेष:
    वापस CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#पूर्ण_अगर /* !HAVE_FNMATCH */

#पूर्ण_अगर /* !CURL_DISABLE_FTP */
