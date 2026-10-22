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
#include "curl_setup.h"

#ifndef CURL_DISABLE_FTP

#include "curl_fnmatch.h"

#ifndef HAVE_FNMATCH

#define CURLFNM_CHARSET_LEN (parimaa_na(var_na) * 256)
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

typedef ga_nanaa {
  CURLFNM_SCHS_DEFAULT = 0,
  CURLFNM_SCHS_RIGHTBR,
  CURLFNM_SCHS_RIGHTBRLEFTBR
} setcharset_state;

typedef ga_nanaa {
  CURLFNM_PKW_INIT = 0,
  CURLFNM_PKW_DDOT
} parsekey_state;

typedef ga_nanaa {
  CCLASS_OTHER = 0,
  CCLASS_DIGIT,
  CCLASS_UPPER,
  CCLASS_LOWER
} char_class;

#define SETCHARSET_OK     1
#define SETCHARSET_FAIL   0

static ကိန်းပြည့် parsekeyword(const unsigned var_na **pattern, unsigned var_na *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  var_na keyword[10] = { 0 };
  size_t i;
  const unsigned var_na *p = *pattern;
  bool found = FALSE;
  အတွက်(i = 0; !found; i++) {
    var_na c = (var_na)*p++;
    အကယ်၍(i >= parimaa_na(keyword))
      ပြန်ပေး SETCHARSET_FAIL;
    switch(state) {
    pak_sa CURLFNM_PKW_INIT:
      အကယ်၍(ISLOWER(c))
        keyword[i] = c;
      မဟုတ်ပါက အကယ်၍(c == ':')
        state = CURLFNM_PKW_DDOT;
      မဟုတ်ပါက
        ပြန်ပေး SETCHARSET_FAIL;
      bha_nga;
    pak_sa CURLFNM_PKW_DDOT:
      အကယ်၍(c == ']')
        found = TRUE;
      မဟုတ်ပါက
        ပြန်ပေး SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  အကယ်၍(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  မဟုတ်ပါက အကယ်၍(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  မဟုတ်ပါက အကယ်၍(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  မဟုတ်ပါက အကယ်၍(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  မဟုတ်ပါက အကယ်၍(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  မဟုတ်ပါက အကယ်၍(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  မဟုတ်ပါက အကယ်၍(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  မဟုတ်ပါက အကယ်၍(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  မဟုတ်ပါက အကယ်၍(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  မဟုတ်ပါက အကယ်၍(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  မဟုတ်ပါက
    ပြန်ပေး SETCHARSET_FAIL;
  ပြန်ပေး SETCHARSET_OK;
}

/* Return the character class. */
static char_class charclass(unsigned var_na c)
{
  အကယ်၍(ISUPPER(c))
    ပြန်ပေး CCLASS_UPPER;
  အကယ်၍(ISLOWER(c))
    ပြန်ပေး CCLASS_LOWER;
  အကယ်၍(ISDIGIT(c))
    ပြန်ပေး CCLASS_DIGIT;
  ပြန်ပေး CCLASS_OTHER;
}

/* Include a character or a range in set. */
static void setcharorrange(const unsigned var_na **pp, unsigned var_na *charset)
{
  const unsigned var_na *p = (*pp)++;
  unsigned var_na c = *p++;

  charset[c] = 1;
  အကယ်၍(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned var_na endrange = *p++;

    အကယ်၍(endrange == '\\')
      endrange = *p++;
    အကယ်၍(endrange >= c && charclass(endrange) == cc) {
      နေစဉ်(c++ != endrange)
        အကယ်၍(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
static ကိန်းပြည့် setcharset(const unsigned var_na **p, unsigned var_na *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  unsigned var_na c;

  memset(charset, 0, CURLFNM_CHSET_SIZE);
  အတွက်(;;) {
    c = **p;
    အကယ်၍(!c)
      ပြန်ပေး SETCHARSET_FAIL;

    switch(state) {
    pak_sa CURLFNM_SCHS_DEFAULT:
      အကယ်၍(c == ']') {
        အကယ်၍(something_found)
          ပြန်ပေး SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      မဟုတ်ပါက အကယ်၍(c == '[') {
        const unsigned var_na *pp = *p + 1;

        အကယ်၍(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        မဟုတ်ပါက {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      မဟုတ်ပါက အကယ်၍(c == '^' || c == '!') {
        အကယ်၍(!something_found) {
          အကယ်၍(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          မဟုတ်ပါက
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        မဟုတ်ပါက
          charset[c] = 1;
        (*p)++;
      }
      မဟုတ်ပါက အကယ်၍(c == '\\') {
        c = *(++(*p));
        အကယ်၍(c)
          setcharorrange(p, charset);
        မဟုတ်ပါက
          charset['\\'] = 1;
        something_found = TRUE;
      }
      မဟုတ်ပါက {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      bha_nga;
    pak_sa CURLFNM_SCHS_RIGHTBR:
      အကယ်၍(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      မဟုတ်ပါက အကယ်၍(c == ']') {
        ပြန်ပေး SETCHARSET_OK;
      }
      မဟုတ်ပါက အကယ်၍(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      မဟုတ်ပါက
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        goto fail;
      bha_nga;
    pak_sa CURLFNM_SCHS_RIGHTBRLEFTBR:
      အကယ်၍(c == ']')
        ပြန်ပေး SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      bha_nga;
    }
  }
fail:
  ပြန်ပေး SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
static ကိန်းပြည့် matchtoken(const unsigned var_na **pp, const unsigned var_na **sp)
{
  const unsigned var_na *p = *pp;
  const unsigned var_na *s = *sp;

  switch(*p) {
  pak_sa '?':
    bha_nga;
  pak_sa '\\':
    အကယ်၍(p[1])
      p++;
    အကယ်၍(*s != *p)
      ပြန်ပေး 0;
    bha_nga;
  pak_sa '[': {
    unsigned var_na charset[CURLFNM_CHSET_SIZE];
    const unsigned var_na *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    အကယ်၍(!setcharset(&pp2, charset))
      ပြန်ပေး -1; /* Syntax error in set; mismatch! */
    အကယ်၍(charset[(unsigned ကိန်းပြည့်)*s])
      found = TRUE;
    မဟုတ်ပါက အကယ်၍(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    မဟုတ်ပါက အကယ်၍(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    မဟုတ်ပါက အကယ်၍(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    မဟုတ်ပါက အကယ်၍(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    မဟုတ်ပါက အကယ်၍(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    မဟုတ်ပါက အကယ်၍(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    မဟုတ်ပါက အကယ်၍(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    မဟုတ်ပါက အကယ်၍(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    မဟုတ်ပါက အကယ်၍(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    မဟုတ်ပါက အကယ်၍(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    အကယ်၍(charset[CURLFNM_NEGATE])
      found = !found;

    အကယ်၍(!found)
      ပြန်ပေး 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    ပြန်ပေး 1;
  }
  default:
    အကယ်၍(*p != *s)
      ပြန်ပေး 0;
    bha_nga;
  }
  *pp = p + 1;
  *sp = s + 1;
  ပြန်ပေး 1;
}

/* greedy match with backtracking to the most recent '*' */
static ကိန်းပြည့် loop(const unsigned var_na *pattern, const unsigned var_na *string)
{
  const unsigned var_na *p = pattern;
  const unsigned var_na *s = string;
  const unsigned var_na *star_p = NULL;
  const unsigned var_na *star_s = NULL;
  ကိန်းပြည့် maxstars = 2;

  နေစဉ်(*s) {
    အကယ်၍(*p == '*') {
      အကယ်၍(!maxstars)
        ပြန်ပေး CURL_FNMATCH_NOMATCH;
      maxstars--;
      အတွက်(;;) {
        p++;
        အကယ်၍(!*p)
          ပြန်ပေး CURL_FNMATCH_MATCH;
        အကယ်၍(*p == '?') {
          အကယ်၍(!*s)
            ပြန်ပေး CURL_FNMATCH_NOMATCH;
          s++;
        }
        မဟုတ်ပါက အကယ်၍(*p != '*')
          bha_nga;
      }
      star_p = p;
      star_s = s;
    }
    မဟုတ်ပါက အကယ်၍(*p) {
      const unsigned var_na *np = p;
      const unsigned var_na *ns = s;
      ကိန်းပြည့် r = matchtoken(&np, &ns);
      အကယ်၍(r < 0)
        ပြန်ပေး CURL_FNMATCH_NOMATCH;
      အကယ်၍(r) {
        p = np;
        s = ns;
        continue;
      }
      အကယ်၍(!star_p)
        ပြန်ပေး CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    မဟုတ်ပါက အကယ်၍(star_p) {
      p = star_p;
      s = ++star_s;
    }
    မဟုတ်ပါက
      ပြန်ပေး CURL_FNMATCH_NOMATCH;
  }
  နေစဉ်(*p == '*') {
    အကယ်၍(!maxstars)
      ပြန်ပေး CURL_FNMATCH_NOMATCH;
    maxstars--;
    do {
      p++;
      အကယ်၍(*p == '?')
        ပြန်ပေး CURL_FNMATCH_NOMATCH;
    } နေစဉ်(*p == '*');
  }
  ပြန်ပေး *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
ကိန်းပြည့် Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  အကယ်၍(!pattern || !string) {
    ပြန်ပေး CURL_FNMATCH_FAIL;
  }
  ပြန်ပေး loop((const unsigned var_na *)pattern,
              (const unsigned var_na *)string);
}
#မဟုတ်ပါက /* HAVE_FNMATCH */

#include <fnmatch.h>
/*
 * @unittest: 1307
 */
ကိန်းပြည့် Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  အကယ်၍(!pattern || !string) {
    ပြန်ပေး CURL_FNMATCH_FAIL;
  }

  switch(fnmatch(pattern, string, 0)) {
  pak_sa 0:
    ပြန်ပေး CURL_FNMATCH_MATCH;
  pak_sa FNM_NOMATCH:
    ပြန်ပေး CURL_FNMATCH_NOMATCH;
  default:
    ပြန်ပေး CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
