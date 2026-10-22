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
#ضمن "curl_setup.h"

#که_نه_تعریف_شوی CURL_DISABLE_FTP

#ضمن "curl_fnmatch.h"

#که_نه_تعریف_شوی HAVE_FNMATCH

#تعریف_کړه CURLFNM_CHARSET_LEN (حجم(حرف) * 256)
#تعریف_کړه CURLFNM_CHSET_اندازه (CURLFNM_CHARSET_LEN + 15)

#تعریف_کړه CURLFNM_NEGATE  CURLFNM_CHARSET_LEN

#تعریف_کړه CURLFNM_ALNUM   (CURLFNM_CHARSET_LEN + 1)
#تعریف_کړه CURLFNM_DIGIT   (CURLFNM_CHARSET_LEN + 2)
#تعریف_کړه CURLFNM_XDIGIT  (CURLFNM_CHARSET_LEN + 3)
#تعریف_کړه CURLFNM_ALPHA   (CURLFNM_CHARSET_LEN + 4)
#تعریف_کړه CURLFNM_PRINT   (CURLFNM_CHARSET_LEN + 5)
#تعریف_کړه CURLFNM_BLANK   (CURLFNM_CHARSET_LEN + 6)
#تعریف_کړه CURLFNM_ښکتنی   (CURLFNM_CHARSET_LEN + 7)
#تعریف_کړه CURLFNM_ګراف   (CURLFNM_CHARSET_LEN + 8)
#تعریف_کړه CURLFNM_SPACE   (CURLFNM_CHARSET_LEN + 9)
#تعریف_کړه CURLFNM_پاسنی   (CURLFNM_CHARSET_LEN + 10)

تعريفنوع تعداد {
  CURLFNM_SCHS_DEFAULT = 0,
  CURLFNM_SCHS_RIGHTBR,
  CURLFNM_SCHS_RIGHTBRLEFTBR
} setcharset_state;

تعريفنوع تعداد {
  CURLFNM_PKW_INIT = 0,
  CURLFNM_PKW_DDOT
} parsekey_state;

تعريفنوع تعداد {
  CCLASS_OTHER = 0,
  CCLASS_DIGIT,
  CCLASS_پاسنی,
  CCLASS_ښکتنی
} char_class;

#تعریف_کړه SETCHARSET_OK     1
#تعریف_کړه SETCHARSET_FAIL   0

ساكن صحيح parsekeyword(ثابت بینښه حرف **نمونه, بینښه حرف *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  حرف keyword[10] = { 0 };
  size_t i;
  ثابت بینښه حرف *p = *نمونه;
  bool وموندل = FALSE;
  لكل(i = 0; !وموندل; i++) {
    حرف c = (حرف)*p++;
    إذا(i >= حجم(keyword))
      أرجع SETCHARSET_FAIL;
    بدل(state) {
    حالة CURLFNM_PKW_INIT:
      إذا(ISLOWER(c))
        keyword[i] = c;
      وإلا إذا(c == ':')
        state = CURLFNM_PKW_DDOT;
      وإلا
        أرجع SETCHARSET_FAIL;
      اكسر;
    حالة CURLFNM_PKW_DDOT:
      إذا(c == ']')
        وموندل = TRUE;
      وإلا
        أرجع SETCHARSET_FAIL;
    }
  }
#تعریف_لغوه KEYLEN

  *نمونه = p; /* move caller's pattern pointer */
  إذا(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  وإلا إذا(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  وإلا إذا(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  وإلا إذا(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  وإلا إذا(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  وإلا إذا(!strcmp(keyword, "graph"))
    charset[CURLFNM_ګراف] = 1;
  وإلا إذا(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  وإلا إذا(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  وإلا إذا(!strcmp(keyword, "upper"))
    charset[CURLFNM_پاسنی] = 1;
  وإلا إذا(!strcmp(keyword, "lower"))
    charset[CURLFNM_ښکتنی] = 1;
  وإلا
    أرجع SETCHARSET_FAIL;
  أرجع SETCHARSET_OK;
}

/* Return the character class. */
ساكن char_class charclass(بینښه حرف c)
{
  إذا(ISUPPER(c))
    أرجع CCLASS_پاسنی;
  إذا(ISLOWER(c))
    أرجع CCLASS_ښکتنی;
  إذا(ISDIGIT(c))
    أرجع CCLASS_DIGIT;
  أرجع CCLASS_OTHER;
}

/* Include a character or a range in set. */
ساكن فراغ setcharorrange(ثابت بینښه حرف **pp, بینښه حرف *charset)
{
  ثابت بینښه حرف *p = (*pp)++;
  بینښه حرف c = *p++;

  charset[c] = 1;
  إذا(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    بینښه حرف endrange = *p++;

    إذا(endrange == '\\')
      endrange = *p++;
    إذا(endrange >= c && charclass(endrange) == cc) {
      طالما(c++ != endrange)
        إذا(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
ساكن صحيح setcharset(ثابت بینښه حرف **p, بینښه حرف *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_وموندل = FALSE;
  بینښه حرف c;

  memset(charset, 0, CURLFNM_CHSET_اندازه);
  لكل(;;) {
    c = **p;
    إذا(!c)
      أرجع SETCHARSET_FAIL;

    بدل(state) {
    حالة CURLFNM_SCHS_DEFAULT:
      إذا(c == ']') {
        إذا(something_وموندل)
          أرجع SETCHARSET_OK;
        something_وموندل = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      وإلا إذا(c == '[') {
        ثابت بینښه حرف *pp = *p + 1;

        إذا(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        وإلا {
          charset[c] = 1;
          (*p)++;
        }
        something_وموندل = TRUE;
      }
      وإلا إذا(c == '^' || c == '!') {
        إذا(!something_وموندل) {
          إذا(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_وموندل = TRUE;
          }
          وإلا
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        وإلا
          charset[c] = 1;
        (*p)++;
      }
      وإلا إذا(c == '\\') {
        c = *(++(*p));
        إذا(c)
          setcharorrange(p, charset);
        وإلا
          charset['\\'] = 1;
        something_وموندل = TRUE;
      }
      وإلا {
        setcharorrange(p, charset);
        something_وموندل = TRUE;
      }
      اكسر;
    حالة CURLFNM_SCHS_RIGHTBR:
      إذا(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      وإلا إذا(c == ']') {
        أرجع SETCHARSET_OK;
      }
      وإلا إذا(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      وإلا
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        اذهب fail;
      اكسر;
    حالة CURLFNM_SCHS_RIGHTBRLEFTBR:
      إذا(c == ']')
        أرجع SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      اكسر;
    }
  }
fail:
  أرجع SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
ساكن صحيح matchtoken(ثابت بینښه حرف **pp, ثابت بینښه حرف **sp)
{
  ثابت بینښه حرف *p = *pp;
  ثابت بینښه حرف *s = *sp;

  بدل(*p) {
  حالة '?':
    اكسر;
  حالة '\\':
    إذا(p[1])
      p++;
    إذا(*s != *p)
      أرجع 0;
    اكسر;
  حالة '[': {
    بینښه حرف charset[CURLFNM_CHSET_اندازه];
    ثابت بینښه حرف *pp2 = p + 1; /* Copy in case of syntax error. */
    bool وموندل = FALSE;
    إذا(!setcharset(&pp2, charset))
      أرجع -1; /* Syntax error in set; mismatch! */
    إذا(charset[(بینښه صحيح)*s])
      وموندل = TRUE;
    وإلا إذا(charset[CURLFNM_ALNUM])
      وموندل = ISALNUM(*s);
    وإلا إذا(charset[CURLFNM_ALPHA])
      وموندل = ISALPHA(*s);
    وإلا إذا(charset[CURLFNM_DIGIT])
      وموندل = ISDIGIT(*s);
    وإلا إذا(charset[CURLFNM_XDIGIT])
      وموندل = ISXDIGIT(*s);
    وإلا إذا(charset[CURLFNM_PRINT])
      وموندل = ISPRINT(*s);
    وإلا إذا(charset[CURLFNM_SPACE])
      وموندل = ISBLANK(*s);
    وإلا إذا(charset[CURLFNM_پاسنی])
      وموندل = ISUPPER(*s);
    وإلا إذا(charset[CURLFNM_ښکتنی])
      وموندل = ISLOWER(*s);
    وإلا إذا(charset[CURLFNM_BLANK])
      وموندل = ISBLANK(*s);
    وإلا إذا(charset[CURLFNM_ګراف])
      وموندل = ISGRAPH(*s);

    إذا(charset[CURLFNM_NEGATE])
      وموندل = !وموندل;

    إذا(!وموندل)
      أرجع 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    أرجع 1;
  }
  افتراضي:
    إذا(*p != *s)
      أرجع 0;
    اكسر;
  }
  *pp = p + 1;
  *sp = s + 1;
  أرجع 1;
}

/* greedy match with backtracking to the most recent '*' */
ساكن صحيح loop(ثابت بینښه حرف *نمونه, ثابت بینښه حرف *string)
{
  ثابت بینښه حرف *p = نمونه;
  ثابت بینښه حرف *s = string;
  ثابت بینښه حرف *ستوری_p = عدم;
  ثابت بینښه حرف *ستوری_s = عدم;
  صحيح maxstars = 2;

  طالما(*s) {
    إذا(*p == '*') {
      إذا(!maxstars)
        أرجع CURL_FNMATCH_NOMATCH;
      maxstars--;
      لكل(;;) {
        p++;
        إذا(!*p)
          أرجع CURL_FNMATCH_سمون;
        إذا(*p == '?') {
          إذا(!*s)
            أرجع CURL_FNMATCH_NOMATCH;
          s++;
        }
        وإلا إذا(*p != '*')
          اكسر;
      }
      ستوری_p = p;
      ستوری_s = s;
    }
    وإلا إذا(*p) {
      ثابت بینښه حرف *np = p;
      ثابت بینښه حرف *ns = s;
      صحيح r = matchtoken(&np, &ns);
      إذا(r < 0)
        أرجع CURL_FNMATCH_NOMATCH;
      إذا(r) {
        p = np;
        s = ns;
        استمر;
      }
      إذا(!ستوری_p)
        أرجع CURL_FNMATCH_NOMATCH;
      p = ستوری_p;
      s = ++ستوری_s;
    }
    وإلا إذا(ستوری_p) {
      p = ستوری_p;
      s = ++ستوری_s;
    }
    وإلا
      أرجع CURL_FNMATCH_NOMATCH;
  }
  طالما(*p == '*') {
    إذا(!maxstars)
      أرجع CURL_FNMATCH_NOMATCH;
    maxstars--;
    افعل {
      p++;
      إذا(*p == '?')
        أرجع CURL_FNMATCH_NOMATCH;
    } طالما(*p == '*');
  }
  أرجع *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_سمون;
}

/*
 * @unittest: 1307
 */
صحيح Curl_fnmatch(فراغ *ptr, ثابت حرف *نمونه, ثابت حرف *string)
{
  (فراغ)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  إذا(!نمونه || !string) {
    أرجع CURL_FNMATCH_FAIL;
  }
  أرجع loop((ثابت بینښه حرف *)نمونه,
              (ثابت بینښه حرف *)string);
}
#وإلا /* HAVE_FNMATCH */

#ضمن <fnmatch.h>
/*
 * @unittest: 1307
 */
صحيح Curl_fnmatch(فراغ *ptr, ثابت حرف *نمونه, ثابت حرف *string)
{
  (فراغ)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  إذا(!نمونه || !string) {
    أرجع CURL_FNMATCH_FAIL;
  }

  بدل(fnmatch(نمونه, string, 0)) {
  حالة 0:
    أرجع CURL_FNMATCH_سمون;
  حالة FNM_NOMATCH:
    أرجع CURL_FNMATCH_NOMATCH;
  افتراضي:
    أرجع CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#د_که_پای /* !HAVE_FNMATCH */

#د_که_پای /* !CURL_DISABLE_FTP */
