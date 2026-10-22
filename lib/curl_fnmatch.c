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
#شامل "curl_setup.h"

#ifndef CURL_DISABLE_FTP

#شامل "curl_fnmatch.h"

#ifndef HAVE_FNMATCH

#define CURLFNM_CHARSET_LEN (اندازه(نویسه) * 256)
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

تعریفنوع شمارش {
  CURLFNM_SCHS_DEFAULT = 0,
  CURLFNM_SCHS_RIGHTBR,
  CURLFNM_SCHS_RIGHTBRLEFTBR
} setcharset_state;

تعریفنوع شمارش {
  CURLFNM_PKW_INIT = 0,
  CURLFNM_PKW_DDOT
} parsekey_state;

تعریفنوع شمارش {
  CCLASS_OTHER = 0,
  CCLASS_DIGIT,
  CCLASS_UPPER,
  CCLASS_LOWER
} char_class;

#define SETCHARSET_OK     1
#define SETCHARSET_FAIL   0

ایستا صحیح parsekeyword(ثابت unsigned نویسه **pattern, unsigned نویسه *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  نویسه keyword[10] = { 0 };
  size_t i;
  ثابت unsigned نویسه *p = *pattern;
  bool found = FALSE;
  برای(i = 0; !found; i++) {
    نویسه c = (نویسه)*p++;
    اگر(i >= اندازه(keyword))
      بازگشت SETCHARSET_FAIL;
    انتخاب(state) {
    حالت CURLFNM_PKW_INIT:
      اگر(ISLOWER(c))
        keyword[i] = c;
      وگرنه اگر(c == ':')
        state = CURLFNM_PKW_DDOT;
      وگرنه
        بازگشت SETCHARSET_FAIL;
      بشکن;
    حالت CURLFNM_PKW_DDOT:
      اگر(c == ']')
        found = TRUE;
      وگرنه
        بازگشت SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  اگر(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  وگرنه اگر(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  وگرنه اگر(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  وگرنه اگر(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  وگرنه اگر(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  وگرنه اگر(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  وگرنه اگر(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  وگرنه اگر(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  وگرنه اگر(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  وگرنه اگر(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  وگرنه
    بازگشت SETCHARSET_FAIL;
  بازگشت SETCHARSET_OK;
}

/* Return the character class. */
ایستا char_class charclass(unsigned نویسه c)
{
  اگر(ISUPPER(c))
    بازگشت CCLASS_UPPER;
  اگر(ISLOWER(c))
    بازگشت CCLASS_LOWER;
  اگر(ISDIGIT(c))
    بازگشت CCLASS_DIGIT;
  بازگشت CCLASS_OTHER;
}

/* Include a character or a range in set. */
ایستا تهی setcharorrange(ثابت unsigned نویسه **pp, unsigned نویسه *charset)
{
  ثابت unsigned نویسه *p = (*pp)++;
  unsigned نویسه c = *p++;

  charset[c] = 1;
  اگر(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned نویسه endrange = *p++;

    اگر(endrange == '\\')
      endrange = *p++;
    اگر(endrange >= c && charclass(endrange) == cc) {
      وقتی(c++ != endrange)
        اگر(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
ایستا صحیح setcharset(ثابت unsigned نویسه **p, unsigned نویسه *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  unsigned نویسه c;

  memset(charset, 0, CURLFNM_CHSET_SIZE);
  برای(;;) {
    c = **p;
    اگر(!c)
      بازگشت SETCHARSET_FAIL;

    انتخاب(state) {
    حالت CURLFNM_SCHS_DEFAULT:
      اگر(c == ']') {
        اگر(something_found)
          بازگشت SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      وگرنه اگر(c == '[') {
        ثابت unsigned نویسه *pp = *p + 1;

        اگر(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        وگرنه {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      وگرنه اگر(c == '^' || c == '!') {
        اگر(!something_found) {
          اگر(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          وگرنه
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        وگرنه
          charset[c] = 1;
        (*p)++;
      }
      وگرنه اگر(c == '\\') {
        c = *(++(*p));
        اگر(c)
          setcharorrange(p, charset);
        وگرنه
          charset['\\'] = 1;
        something_found = TRUE;
      }
      وگرنه {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      بشکن;
    حالت CURLFNM_SCHS_RIGHTBR:
      اگر(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      وگرنه اگر(c == ']') {
        بازگشت SETCHARSET_OK;
      }
      وگرنه اگر(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      وگرنه
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        برو fail;
      بشکن;
    حالت CURLFNM_SCHS_RIGHTBRLEFTBR:
      اگر(c == ']')
        بازگشت SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      بشکن;
    }
  }
fail:
  بازگشت SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
ایستا صحیح matchtoken(ثابت unsigned نویسه **pp, ثابت unsigned نویسه **sp)
{
  ثابت unsigned نویسه *p = *pp;
  ثابت unsigned نویسه *s = *sp;

  انتخاب(*p) {
  حالت '?':
    بشکن;
  حالت '\\':
    اگر(p[1])
      p++;
    اگر(*s != *p)
      بازگشت 0;
    بشکن;
  حالت '[': {
    unsigned نویسه charset[CURLFNM_CHSET_SIZE];
    ثابت unsigned نویسه *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    اگر(!setcharset(&pp2, charset))
      بازگشت -1; /* Syntax error in set; mismatch! */
    اگر(charset[(unsigned صحیح)*s])
      found = TRUE;
    وگرنه اگر(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    وگرنه اگر(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    وگرنه اگر(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    وگرنه اگر(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    وگرنه اگر(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    وگرنه اگر(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    وگرنه اگر(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    وگرنه اگر(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    وگرنه اگر(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    وگرنه اگر(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    اگر(charset[CURLFNM_NEGATE])
      found = !found;

    اگر(!found)
      بازگشت 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    بازگشت 1;
  }
  پیشفرض:
    اگر(*p != *s)
      بازگشت 0;
    بشکن;
  }
  *pp = p + 1;
  *sp = s + 1;
  بازگشت 1;
}

/* greedy match with backtracking to the most recent '*' */
ایستا صحیح loop(ثابت unsigned نویسه *pattern, ثابت unsigned نویسه *string)
{
  ثابت unsigned نویسه *p = pattern;
  ثابت unsigned نویسه *s = string;
  ثابت unsigned نویسه *star_p = پوچ;
  ثابت unsigned نویسه *star_s = پوچ;
  صحیح maxstars = 2;

  وقتی(*s) {
    اگر(*p == '*') {
      اگر(!maxstars)
        بازگشت CURL_FNMATCH_NOMATCH;
      maxstars--;
      برای(;;) {
        p++;
        اگر(!*p)
          بازگشت CURL_FNMATCH_MATCH;
        اگر(*p == '?') {
          اگر(!*s)
            بازگشت CURL_FNMATCH_NOMATCH;
          s++;
        }
        وگرنه اگر(*p != '*')
          بشکن;
      }
      star_p = p;
      star_s = s;
    }
    وگرنه اگر(*p) {
      ثابت unsigned نویسه *np = p;
      ثابت unsigned نویسه *ns = s;
      صحیح r = matchtoken(&np, &ns);
      اگر(r < 0)
        بازگشت CURL_FNMATCH_NOMATCH;
      اگر(r) {
        p = np;
        s = ns;
        ادامه;
      }
      اگر(!star_p)
        بازگشت CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    وگرنه اگر(star_p) {
      p = star_p;
      s = ++star_s;
    }
    وگرنه
      بازگشت CURL_FNMATCH_NOMATCH;
  }
  وقتی(*p == '*') {
    اگر(!maxstars)
      بازگشت CURL_FNMATCH_NOMATCH;
    maxstars--;
    انجامبده {
      p++;
      اگر(*p == '?')
        بازگشت CURL_FNMATCH_NOMATCH;
    } وقتی(*p == '*');
  }
  بازگشت *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
صحیح Curl_fnmatch(تهی *ptr, ثابت نویسه *pattern, ثابت نویسه *string)
{
  (تهی)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  اگر(!pattern || !string) {
    بازگشت CURL_FNMATCH_FAIL;
  }
  بازگشت loop((ثابت unsigned نویسه *)pattern,
              (ثابت unsigned نویسه *)string);
}
#وگرنه /* HAVE_FNMATCH */

#شامل <fnmatch.h>
/*
 * @unittest: 1307
 */
صحیح Curl_fnmatch(تهی *ptr, ثابت نویسه *pattern, ثابت نویسه *string)
{
  (تهی)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  اگر(!pattern || !string) {
    بازگشت CURL_FNMATCH_FAIL;
  }

  انتخاب(fnmatch(pattern, string, 0)) {
  حالت 0:
    بازگشت CURL_FNMATCH_MATCH;
  حالت FNM_NOMATCH:
    بازگشت CURL_FNMATCH_NOMATCH;
  پیشفرض:
    بازگشت CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
