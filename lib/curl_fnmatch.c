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

static puur_naa_nka parsekeyword(const unsigned var_na **pattern, unsigned var_na *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  var_na keyword[10] = { 0 };
  size_t i;
  const unsigned var_na *p = *pattern;
  bool found = FALSE;
  for(i = 0; !found; i++) {
    var_na c = (var_na)*p++;
    ᰀᰧ(i >= parimaa_na(keyword))
      ᰀᰧᰌ SETCHARSET_FAIL;
    switch(state) {
    pak_sa CURLFNM_PKW_INIT:
      ᰀᰧ(ISLOWER(c))
        keyword[i] = c;
      ᰀᰧᰀ ᰀᰧ(c == ':')
        state = CURLFNM_PKW_DDOT;
      ᰀᰧᰀ
        ᰀᰧᰌ SETCHARSET_FAIL;
      bha_nga;
    pak_sa CURLFNM_PKW_DDOT:
      ᰀᰧ(c == ']')
        found = TRUE;
      ᰀᰧᰀ
        ᰀᰧᰌ SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  ᰀᰧ(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  ᰀᰧᰀ ᰀᰧ(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  ᰀᰧᰀ ᰀᰧ(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  ᰀᰧᰀ ᰀᰧ(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  ᰀᰧᰀ ᰀᰧ(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  ᰀᰧᰀ ᰀᰧ(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  ᰀᰧᰀ ᰀᰧ(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  ᰀᰧᰀ ᰀᰧ(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  ᰀᰧᰀ ᰀᰧ(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  ᰀᰧᰀ ᰀᰧ(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  ᰀᰧᰀ
    ᰀᰧᰌ SETCHARSET_FAIL;
  ᰀᰧᰌ SETCHARSET_OK;
}

/* Return the character class. */
static char_class charclass(unsigned var_na c)
{
  ᰀᰧ(ISUPPER(c))
    ᰀᰧᰌ CCLASS_UPPER;
  ᰀᰧ(ISLOWER(c))
    ᰀᰧᰌ CCLASS_LOWER;
  ᰀᰧ(ISDIGIT(c))
    ᰀᰧᰌ CCLASS_DIGIT;
  ᰀᰧᰌ CCLASS_OTHER;
}

/* Include a character or a range in set. */
static void setcharorrange(const unsigned var_na **pp, unsigned var_na *charset)
{
  const unsigned var_na *p = (*pp)++;
  unsigned var_na c = *p++;

  charset[c] = 1;
  ᰀᰧ(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned var_na endrange = *p++;

    ᰀᰧ(endrange == '\\')
      endrange = *p++;
    ᰀᰧ(endrange >= c && charclass(endrange) == cc) {
      ᰀᰬ(c++ != endrange)
        ᰀᰧ(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
static puur_naa_nka setcharset(const unsigned var_na **p, unsigned var_na *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  unsigned var_na c;

  memset(charset, 0, CURLFNM_CHSET_SIZE);
  for(;;) {
    c = **p;
    ᰀᰧ(!c)
      ᰀᰧᰌ SETCHARSET_FAIL;

    switch(state) {
    pak_sa CURLFNM_SCHS_DEFAULT:
      ᰀᰧ(c == ']') {
        ᰀᰧ(something_found)
          ᰀᰧᰌ SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      ᰀᰧᰀ ᰀᰧ(c == '[') {
        const unsigned var_na *pp = *p + 1;

        ᰀᰧ(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        ᰀᰧᰀ {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      ᰀᰧᰀ ᰀᰧ(c == '^' || c == '!') {
        ᰀᰧ(!something_found) {
          ᰀᰧ(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          ᰀᰧᰀ
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        ᰀᰧᰀ
          charset[c] = 1;
        (*p)++;
      }
      ᰀᰧᰀ ᰀᰧ(c == '\\') {
        c = *(++(*p));
        ᰀᰧ(c)
          setcharorrange(p, charset);
        ᰀᰧᰀ
          charset['\\'] = 1;
        something_found = TRUE;
      }
      ᰀᰧᰀ {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      bha_nga;
    pak_sa CURLFNM_SCHS_RIGHTBR:
      ᰀᰧ(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      ᰀᰧᰀ ᰀᰧ(c == ']') {
        ᰀᰧᰌ SETCHARSET_OK;
      }
      ᰀᰧᰀ ᰀᰧ(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      ᰀᰧᰀ
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        goto fail;
      bha_nga;
    pak_sa CURLFNM_SCHS_RIGHTBRLEFTBR:
      ᰀᰧ(c == ']')
        ᰀᰧᰌ SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      bha_nga;
    }
  }
fail:
  ᰀᰧᰌ SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
static puur_naa_nka matchtoken(const unsigned var_na **pp, const unsigned var_na **sp)
{
  const unsigned var_na *p = *pp;
  const unsigned var_na *s = *sp;

  switch(*p) {
  pak_sa '?':
    bha_nga;
  pak_sa '\\':
    ᰀᰧ(p[1])
      p++;
    ᰀᰧ(*s != *p)
      ᰀᰧᰌ 0;
    bha_nga;
  pak_sa '[': {
    unsigned var_na charset[CURLFNM_CHSET_SIZE];
    const unsigned var_na *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    ᰀᰧ(!setcharset(&pp2, charset))
      ᰀᰧᰌ -1; /* Syntax error in set; mismatch! */
    ᰀᰧ(charset[(unsigned puur_naa_nka)*s])
      found = TRUE;
    ᰀᰧᰀ ᰀᰧ(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    ᰀᰧᰀ ᰀᰧ(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    ᰀᰧᰀ ᰀᰧ(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    ᰀᰧᰀ ᰀᰧ(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    ᰀᰧᰀ ᰀᰧ(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    ᰀᰧᰀ ᰀᰧ(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    ᰀᰧᰀ ᰀᰧ(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    ᰀᰧᰀ ᰀᰧ(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    ᰀᰧᰀ ᰀᰧ(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    ᰀᰧᰀ ᰀᰧ(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    ᰀᰧ(charset[CURLFNM_NEGATE])
      found = !found;

    ᰀᰧ(!found)
      ᰀᰧᰌ 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    ᰀᰧᰌ 1;
  }
  default:
    ᰀᰧ(*p != *s)
      ᰀᰧᰌ 0;
    bha_nga;
  }
  *pp = p + 1;
  *sp = s + 1;
  ᰀᰧᰌ 1;
}

/* greedy match with backtracking to the most recent '*' */
static puur_naa_nka loop(const unsigned var_na *pattern, const unsigned var_na *string)
{
  const unsigned var_na *p = pattern;
  const unsigned var_na *s = string;
  const unsigned var_na *star_p = NULL;
  const unsigned var_na *star_s = NULL;
  puur_naa_nka maxstars = 2;

  ᰀᰬ(*s) {
    ᰀᰧ(*p == '*') {
      ᰀᰧ(!maxstars)
        ᰀᰧᰌ CURL_FNMATCH_NOMATCH;
      maxstars--;
      for(;;) {
        p++;
        ᰀᰧ(!*p)
          ᰀᰧᰌ CURL_FNMATCH_MATCH;
        ᰀᰧ(*p == '?') {
          ᰀᰧ(!*s)
            ᰀᰧᰌ CURL_FNMATCH_NOMATCH;
          s++;
        }
        ᰀᰧᰀ ᰀᰧ(*p != '*')
          bha_nga;
      }
      star_p = p;
      star_s = s;
    }
    ᰀᰧᰀ ᰀᰧ(*p) {
      const unsigned var_na *np = p;
      const unsigned var_na *ns = s;
      puur_naa_nka r = matchtoken(&np, &ns);
      ᰀᰧ(r < 0)
        ᰀᰧᰌ CURL_FNMATCH_NOMATCH;
      ᰀᰧ(r) {
        p = np;
        s = ns;
        continue;
      }
      ᰀᰧ(!star_p)
        ᰀᰧᰌ CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    ᰀᰧᰀ ᰀᰧ(star_p) {
      p = star_p;
      s = ++star_s;
    }
    ᰀᰧᰀ
      ᰀᰧᰌ CURL_FNMATCH_NOMATCH;
  }
  ᰀᰬ(*p == '*') {
    ᰀᰧ(!maxstars)
      ᰀᰧᰌ CURL_FNMATCH_NOMATCH;
    maxstars--;
    do {
      p++;
      ᰀᰧ(*p == '?')
        ᰀᰧᰌ CURL_FNMATCH_NOMATCH;
    } ᰀᰬ(*p == '*');
  }
  ᰀᰧᰌ *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
puur_naa_nka Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ᰀᰧ(!pattern || !string) {
    ᰀᰧᰌ CURL_FNMATCH_FAIL;
  }
  ᰀᰧᰌ loop((const unsigned var_na *)pattern,
              (const unsigned var_na *)string);
}
#ᰀᰧᰀ /* HAVE_FNMATCH */

#include <fnmatch.h>
/*
 * @unittest: 1307
 */
puur_naa_nka Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ᰀᰧ(!pattern || !string) {
    ᰀᰧᰌ CURL_FNMATCH_FAIL;
  }

  switch(fnmatch(pattern, string, 0)) {
  pak_sa 0:
    ᰀᰧᰌ CURL_FNMATCH_MATCH;
  pak_sa FNM_NOMATCH:
    ᰀᰧᰌ CURL_FNMATCH_NOMATCH;
  default:
    ᰀᰧᰌ CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
