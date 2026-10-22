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
    ᯀᯮᯝ(i >= parimaa_na(keyword))
      ᯀᯮᯞ SETCHARSET_FAIL;
    switch(state) {
    pak_sa CURLFNM_PKW_INIT:
      ᯀᯮᯝ(ISLOWER(c))
        keyword[i] = c;
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(c == ':')
        state = CURLFNM_PKW_DDOT;
      ᯀᯮᯝᯂᯪ
        ᯀᯮᯞ SETCHARSET_FAIL;
      bha_nga;
    pak_sa CURLFNM_PKW_DDOT:
      ᯀᯮᯝ(c == ']')
        found = TRUE;
      ᯀᯮᯝᯂᯪ
        ᯀᯮᯞ SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  ᯀᯮᯝ(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  ᯀᯮᯝᯂᯪ
    ᯀᯮᯞ SETCHARSET_FAIL;
  ᯀᯮᯞ SETCHARSET_OK;
}

/* Return the character class. */
static char_class charclass(unsigned var_na c)
{
  ᯀᯮᯝ(ISUPPER(c))
    ᯀᯮᯞ CCLASS_UPPER;
  ᯀᯮᯝ(ISLOWER(c))
    ᯀᯮᯞ CCLASS_LOWER;
  ᯀᯮᯝ(ISDIGIT(c))
    ᯀᯮᯞ CCLASS_DIGIT;
  ᯀᯮᯞ CCLASS_OTHER;
}

/* Include a character or a range in set. */
static void setcharorrange(const unsigned var_na **pp, unsigned var_na *charset)
{
  const unsigned var_na *p = (*pp)++;
  unsigned var_na c = *p++;

  charset[c] = 1;
  ᯀᯮᯝ(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned var_na endrange = *p++;

    ᯀᯮᯝ(endrange == '\\')
      endrange = *p++;
    ᯀᯮᯝ(endrange >= c && charclass(endrange) == cc) {
      ᯀᯬᯝ(c++ != endrange)
        ᯀᯮᯝ(charclass(c) == cc)  /* Chars in class may be not consecutive. */
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
    ᯀᯮᯝ(!c)
      ᯀᯮᯞ SETCHARSET_FAIL;

    switch(state) {
    pak_sa CURLFNM_SCHS_DEFAULT:
      ᯀᯮᯝ(c == ']') {
        ᯀᯮᯝ(something_found)
          ᯀᯮᯞ SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(c == '[') {
        const unsigned var_na *pp = *p + 1;

        ᯀᯮᯝ(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        ᯀᯮᯝᯂᯪ {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(c == '^' || c == '!') {
        ᯀᯮᯝ(!something_found) {
          ᯀᯮᯝ(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          ᯀᯮᯝᯂᯪ
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        ᯀᯮᯝᯂᯪ
          charset[c] = 1;
        (*p)++;
      }
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(c == '\\') {
        c = *(++(*p));
        ᯀᯮᯝ(c)
          setcharorrange(p, charset);
        ᯀᯮᯝᯂᯪ
          charset['\\'] = 1;
        something_found = TRUE;
      }
      ᯀᯮᯝᯂᯪ {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      bha_nga;
    pak_sa CURLFNM_SCHS_RIGHTBR:
      ᯀᯮᯝ(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(c == ']') {
        ᯀᯮᯞ SETCHARSET_OK;
      }
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      ᯀᯮᯝᯂᯪ
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        goto fail;
      bha_nga;
    pak_sa CURLFNM_SCHS_RIGHTBRLEFTBR:
      ᯀᯮᯝ(c == ']')
        ᯀᯮᯞ SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      bha_nga;
    }
  }
fail:
  ᯀᯮᯞ SETCHARSET_FAIL;
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
    ᯀᯮᯝ(p[1])
      p++;
    ᯀᯮᯝ(*s != *p)
      ᯀᯮᯞ 0;
    bha_nga;
  pak_sa '[': {
    unsigned var_na charset[CURLFNM_CHSET_SIZE];
    const unsigned var_na *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    ᯀᯮᯝ(!setcharset(&pp2, charset))
      ᯀᯮᯞ -1; /* Syntax error in set; mismatch! */
    ᯀᯮᯝ(charset[(unsigned puur_naa_nka)*s])
      found = TRUE;
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    ᯀᯮᯝ(charset[CURLFNM_NEGATE])
      found = !found;

    ᯀᯮᯝ(!found)
      ᯀᯮᯞ 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    ᯀᯮᯞ 1;
  }
  default:
    ᯀᯮᯝ(*p != *s)
      ᯀᯮᯞ 0;
    bha_nga;
  }
  *pp = p + 1;
  *sp = s + 1;
  ᯀᯮᯞ 1;
}

/* greedy match with backtracking to the most recent '*' */
static puur_naa_nka loop(const unsigned var_na *pattern, const unsigned var_na *string)
{
  const unsigned var_na *p = pattern;
  const unsigned var_na *s = string;
  const unsigned var_na *star_p = NULL;
  const unsigned var_na *star_s = NULL;
  puur_naa_nka maxstars = 2;

  ᯀᯬᯝ(*s) {
    ᯀᯮᯝ(*p == '*') {
      ᯀᯮᯝ(!maxstars)
        ᯀᯮᯞ CURL_FNMATCH_NOMATCH;
      maxstars--;
      for(;;) {
        p++;
        ᯀᯮᯝ(!*p)
          ᯀᯮᯞ CURL_FNMATCH_MATCH;
        ᯀᯮᯝ(*p == '?') {
          ᯀᯮᯝ(!*s)
            ᯀᯮᯞ CURL_FNMATCH_NOMATCH;
          s++;
        }
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ(*p != '*')
          bha_nga;
      }
      star_p = p;
      star_s = s;
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(*p) {
      const unsigned var_na *np = p;
      const unsigned var_na *ns = s;
      puur_naa_nka r = matchtoken(&np, &ns);
      ᯀᯮᯝ(r < 0)
        ᯀᯮᯞ CURL_FNMATCH_NOMATCH;
      ᯀᯮᯝ(r) {
        p = np;
        s = ns;
        continue;
      }
      ᯀᯮᯝ(!star_p)
        ᯀᯮᯞ CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(star_p) {
      p = star_p;
      s = ++star_s;
    }
    ᯀᯮᯝᯂᯪ
      ᯀᯮᯞ CURL_FNMATCH_NOMATCH;
  }
  ᯀᯬᯝ(*p == '*') {
    ᯀᯮᯝ(!maxstars)
      ᯀᯮᯞ CURL_FNMATCH_NOMATCH;
    maxstars--;
    do {
      p++;
      ᯀᯮᯝ(*p == '?')
        ᯀᯮᯞ CURL_FNMATCH_NOMATCH;
    } ᯀᯬᯝ(*p == '*');
  }
  ᯀᯮᯞ *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
puur_naa_nka Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ᯀᯮᯝ(!pattern || !string) {
    ᯀᯮᯞ CURL_FNMATCH_FAIL;
  }
  ᯀᯮᯞ loop((const unsigned var_na *)pattern,
              (const unsigned var_na *)string);
}
#ᯀᯮᯝᯂᯪ /* HAVE_FNMATCH */

#include <fnmatch.h>
/*
 * @unittest: 1307
 */
puur_naa_nka Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ᯀᯮᯝ(!pattern || !string) {
    ᯀᯮᯞ CURL_FNMATCH_FAIL;
  }

  switch(fnmatch(pattern, string, 0)) {
  pak_sa 0:
    ᯀᯮᯞ CURL_FNMATCH_MATCH;
  pak_sa FNM_NOMATCH:
    ᯀᯮᯞ CURL_FNMATCH_NOMATCH;
  default:
    ᯀᯮᯞ CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
