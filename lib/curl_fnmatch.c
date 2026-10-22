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

#ifndef CURL_DISABLE_FTP

#ضمن "curl_fnmatch.h"

#ifndef HAVE_FNMATCH

#define CURLFNM_CHARSET_LEN (حجم(حرف) * 256)
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
  CCLASS_UPPER,
  CCLASS_LOWER
} char_class;

#define SETCHARSET_OK     1
#define SETCHARSET_FAIL   0

ساكن صحيح parsekeyword(ثابت unsigned حرف **pattern, unsigned حرف *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  حرف keyword[10] = { 0 };
  size_t i;
  ثابت unsigned حرف *p = *pattern;
  bool found = FALSE;
  עבור(i = 0; !found; i++) {
    حرف c = (حرف)*p++;
    אם(i >= حجم(keyword))
      החזר SETCHARSET_FAIL;
    بدل(state) {
    حالة CURLFNM_PKW_INIT:
      אם(ISLOWER(c))
        keyword[i] = c;
      אחרת אם(c == ':')
        state = CURLFNM_PKW_DDOT;
      אחרת
        החזר SETCHARSET_FAIL;
      اكسر;
    حالة CURLFNM_PKW_DDOT:
      אם(c == ']')
        found = TRUE;
      אחרת
        החזר SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  אם(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  אחרת אם(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  אחרת אם(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  אחרת אם(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  אחרת אם(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  אחרת אם(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  אחרת אם(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  אחרת אם(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  אחרת אם(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  אחרת אם(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  אחרת
    החזר SETCHARSET_FAIL;
  החזר SETCHARSET_OK;
}

/* Return the character class. */
ساكن char_class charclass(unsigned حرف c)
{
  אם(ISUPPER(c))
    החזר CCLASS_UPPER;
  אם(ISLOWER(c))
    החזר CCLASS_LOWER;
  אם(ISDIGIT(c))
    החזר CCLASS_DIGIT;
  החזר CCLASS_OTHER;
}

/* Include a character or a range in set. */
ساكن فراغ setcharorrange(ثابت unsigned حرف **pp, unsigned حرف *charset)
{
  ثابت unsigned حرف *p = (*pp)++;
  unsigned حرف c = *p++;

  charset[c] = 1;
  אם(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned حرف endrange = *p++;

    אם(endrange == '\\')
      endrange = *p++;
    אם(endrange >= c && charclass(endrange) == cc) {
      כלעוד(c++ != endrange)
        אם(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
ساكن صحيح setcharset(ثابت unsigned حرف **p, unsigned حرف *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  unsigned حرف c;

  memset(charset, 0, CURLFNM_CHSET_SIZE);
  עבור(;;) {
    c = **p;
    אם(!c)
      החזר SETCHARSET_FAIL;

    بدل(state) {
    حالة CURLFNM_SCHS_DEFAULT:
      אם(c == ']') {
        אם(something_found)
          החזר SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      אחרת אם(c == '[') {
        ثابت unsigned حرف *pp = *p + 1;

        אם(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        אחרת {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      אחרת אם(c == '^' || c == '!') {
        אם(!something_found) {
          אם(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          אחרת
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        אחרת
          charset[c] = 1;
        (*p)++;
      }
      אחרת אם(c == '\\') {
        c = *(++(*p));
        אם(c)
          setcharorrange(p, charset);
        אחרת
          charset['\\'] = 1;
        something_found = TRUE;
      }
      אחרת {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      اكسر;
    حالة CURLFNM_SCHS_RIGHTBR:
      אם(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      אחרת אם(c == ']') {
        החזר SETCHARSET_OK;
      }
      אחרת אם(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      אחרת
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        اذهب fail;
      اكسر;
    حالة CURLFNM_SCHS_RIGHTBRLEFTBR:
      אם(c == ']')
        החזר SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      اكسر;
    }
  }
fail:
  החזר SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
ساكن صحيح matchtoken(ثابت unsigned حرف **pp, ثابت unsigned حرف **sp)
{
  ثابت unsigned حرف *p = *pp;
  ثابت unsigned حرف *s = *sp;

  بدل(*p) {
  حالة '?':
    اكسر;
  حالة '\\':
    אם(p[1])
      p++;
    אם(*s != *p)
      החזר 0;
    اكسر;
  حالة '[': {
    unsigned حرف charset[CURLFNM_CHSET_SIZE];
    ثابت unsigned حرف *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    אם(!setcharset(&pp2, charset))
      החזר -1; /* Syntax error in set; mismatch! */
    אם(charset[(unsigned صحيح)*s])
      found = TRUE;
    אחרת אם(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    אחרת אם(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    אחרת אם(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    אחרת אם(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    אחרת אם(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    אחרת אם(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    אחרת אם(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    אחרת אם(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    אחרת אם(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    אחרת אם(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    אם(charset[CURLFNM_NEGATE])
      found = !found;

    אם(!found)
      החזר 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    החזר 1;
  }
  افتراضي:
    אם(*p != *s)
      החזר 0;
    اكسر;
  }
  *pp = p + 1;
  *sp = s + 1;
  החזר 1;
}

/* greedy match with backtracking to the most recent '*' */
ساكن صحيح loop(ثابت unsigned حرف *pattern, ثابت unsigned حرف *string)
{
  ثابت unsigned حرف *p = pattern;
  ثابت unsigned حرف *s = string;
  ثابت unsigned حرف *star_p = عدم;
  ثابت unsigned حرف *star_s = عدم;
  صحيح maxstars = 2;

  כלעוד(*s) {
    אם(*p == '*') {
      אם(!maxstars)
        החזר CURL_FNMATCH_NOMATCH;
      maxstars--;
      עבור(;;) {
        p++;
        אם(!*p)
          החזר CURL_FNMATCH_MATCH;
        אם(*p == '?') {
          אם(!*s)
            החזר CURL_FNMATCH_NOMATCH;
          s++;
        }
        אחרת אם(*p != '*')
          اكسر;
      }
      star_p = p;
      star_s = s;
    }
    אחרת אם(*p) {
      ثابت unsigned حرف *np = p;
      ثابت unsigned حرف *ns = s;
      صحيح r = matchtoken(&np, &ns);
      אם(r < 0)
        החזר CURL_FNMATCH_NOMATCH;
      אם(r) {
        p = np;
        s = ns;
        استمر;
      }
      אם(!star_p)
        החזר CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    אחרת אם(star_p) {
      p = star_p;
      s = ++star_s;
    }
    אחרת
      החזר CURL_FNMATCH_NOMATCH;
  }
  כלעוד(*p == '*') {
    אם(!maxstars)
      החזר CURL_FNMATCH_NOMATCH;
    maxstars--;
    עשה {
      p++;
      אם(*p == '?')
        החזר CURL_FNMATCH_NOMATCH;
    } כלעוד(*p == '*');
  }
  החזר *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
صحيح Curl_fnmatch(فراغ *ptr, ثابت حرف *pattern, ثابت حرف *string)
{
  (فراغ)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  אם(!pattern || !string) {
    החזר CURL_FNMATCH_FAIL;
  }
  החזר loop((ثابت unsigned حرف *)pattern,
              (ثابت unsigned حرف *)string);
}
#אחרת /* HAVE_FNMATCH */

#ضمن <fnmatch.h>
/*
 * @unittest: 1307
 */
صحيح Curl_fnmatch(فراغ *ptr, ثابت حرف *pattern, ثابت حرف *string)
{
  (فراغ)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  אם(!pattern || !string) {
    החזר CURL_FNMATCH_FAIL;
  }

  بدل(fnmatch(pattern, string, 0)) {
  حالة 0:
    החזר CURL_FNMATCH_MATCH;
  حالة FNM_NOMATCH:
    החזר CURL_FNMATCH_NOMATCH;
  افتراضي:
    החזר CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
