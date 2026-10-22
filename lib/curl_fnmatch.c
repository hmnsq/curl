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
    ᮚᮦᮔ᮪(i >= parimaa_na(keyword))
      ᮝᮜᮤ SETCHARSET_FAIL;
    switch(state) {
    pak_sa CURLFNM_PKW_INIT:
      ᮚᮦᮔ᮪(ISLOWER(c))
        keyword[i] = c;
      ᮜᮤᮚ ᮚᮦᮔ᮪(c == ':')
        state = CURLFNM_PKW_DDOT;
      ᮜᮤᮚ
        ᮝᮜᮤ SETCHARSET_FAIL;
      bha_nga;
    pak_sa CURLFNM_PKW_DDOT:
      ᮚᮦᮔ᮪(c == ']')
        found = TRUE;
      ᮜᮤᮚ
        ᮝᮜᮤ SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  ᮚᮦᮔ᮪(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  ᮜᮤᮚ ᮚᮦᮔ᮪(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  ᮜᮤᮚ ᮚᮦᮔ᮪(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  ᮜᮤᮚ ᮚᮦᮔ᮪(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  ᮜᮤᮚ ᮚᮦᮔ᮪(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  ᮜᮤᮚ ᮚᮦᮔ᮪(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  ᮜᮤᮚ ᮚᮦᮔ᮪(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  ᮜᮤᮚ ᮚᮦᮔ᮪(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  ᮜᮤᮚ ᮚᮦᮔ᮪(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  ᮜᮤᮚ ᮚᮦᮔ᮪(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  ᮜᮤᮚ
    ᮝᮜᮤ SETCHARSET_FAIL;
  ᮝᮜᮤ SETCHARSET_OK;
}

/* Return the character class. */
static char_class charclass(unsigned var_na c)
{
  ᮚᮦᮔ᮪(ISUPPER(c))
    ᮝᮜᮤ CCLASS_UPPER;
  ᮚᮦᮔ᮪(ISLOWER(c))
    ᮝᮜᮤ CCLASS_LOWER;
  ᮚᮦᮔ᮪(ISDIGIT(c))
    ᮝᮜᮤ CCLASS_DIGIT;
  ᮝᮜᮤ CCLASS_OTHER;
}

/* Include a character or a range in set. */
static void setcharorrange(const unsigned var_na **pp, unsigned var_na *charset)
{
  const unsigned var_na *p = (*pp)++;
  unsigned var_na c = *p++;

  charset[c] = 1;
  ᮚᮦᮔ᮪(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned var_na endrange = *p++;

    ᮚᮦᮔ᮪(endrange == '\\')
      endrange = *p++;
    ᮚᮦᮔ᮪(endrange >= c && charclass(endrange) == cc) {
      ᮞᮨᮓᮨᮜ(c++ != endrange)
        ᮚᮦᮔ᮪(charclass(c) == cc)  /* Chars in class may be not consecutive. */
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
    ᮚᮦᮔ᮪(!c)
      ᮝᮜᮤ SETCHARSET_FAIL;

    switch(state) {
    pak_sa CURLFNM_SCHS_DEFAULT:
      ᮚᮦᮔ᮪(c == ']') {
        ᮚᮦᮔ᮪(something_found)
          ᮝᮜᮤ SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      ᮜᮤᮚ ᮚᮦᮔ᮪(c == '[') {
        const unsigned var_na *pp = *p + 1;

        ᮚᮦᮔ᮪(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        ᮜᮤᮚ {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      ᮜᮤᮚ ᮚᮦᮔ᮪(c == '^' || c == '!') {
        ᮚᮦᮔ᮪(!something_found) {
          ᮚᮦᮔ᮪(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          ᮜᮤᮚ
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        ᮜᮤᮚ
          charset[c] = 1;
        (*p)++;
      }
      ᮜᮤᮚ ᮚᮦᮔ᮪(c == '\\') {
        c = *(++(*p));
        ᮚᮦᮔ᮪(c)
          setcharorrange(p, charset);
        ᮜᮤᮚ
          charset['\\'] = 1;
        something_found = TRUE;
      }
      ᮜᮤᮚ {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      bha_nga;
    pak_sa CURLFNM_SCHS_RIGHTBR:
      ᮚᮦᮔ᮪(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      ᮜᮤᮚ ᮚᮦᮔ᮪(c == ']') {
        ᮝᮜᮤ SETCHARSET_OK;
      }
      ᮜᮤᮚ ᮚᮦᮔ᮪(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      ᮜᮤᮚ
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        goto fail;
      bha_nga;
    pak_sa CURLFNM_SCHS_RIGHTBRLEFTBR:
      ᮚᮦᮔ᮪(c == ']')
        ᮝᮜᮤ SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      bha_nga;
    }
  }
fail:
  ᮝᮜᮤ SETCHARSET_FAIL;
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
    ᮚᮦᮔ᮪(p[1])
      p++;
    ᮚᮦᮔ᮪(*s != *p)
      ᮝᮜᮤ 0;
    bha_nga;
  pak_sa '[': {
    unsigned var_na charset[CURLFNM_CHSET_SIZE];
    const unsigned var_na *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    ᮚᮦᮔ᮪(!setcharset(&pp2, charset))
      ᮝᮜᮤ -1; /* Syntax error in set; mismatch! */
    ᮚᮦᮔ᮪(charset[(unsigned puur_naa_nka)*s])
      found = TRUE;
    ᮜᮤᮚ ᮚᮦᮔ᮪(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    ᮜᮤᮚ ᮚᮦᮔ᮪(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    ᮜᮤᮚ ᮚᮦᮔ᮪(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    ᮜᮤᮚ ᮚᮦᮔ᮪(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    ᮜᮤᮚ ᮚᮦᮔ᮪(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    ᮜᮤᮚ ᮚᮦᮔ᮪(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    ᮜᮤᮚ ᮚᮦᮔ᮪(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    ᮜᮤᮚ ᮚᮦᮔ᮪(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    ᮜᮤᮚ ᮚᮦᮔ᮪(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    ᮜᮤᮚ ᮚᮦᮔ᮪(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    ᮚᮦᮔ᮪(charset[CURLFNM_NEGATE])
      found = !found;

    ᮚᮦᮔ᮪(!found)
      ᮝᮜᮤ 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    ᮝᮜᮤ 1;
  }
  default:
    ᮚᮦᮔ᮪(*p != *s)
      ᮝᮜᮤ 0;
    bha_nga;
  }
  *pp = p + 1;
  *sp = s + 1;
  ᮝᮜᮤ 1;
}

/* greedy match with backtracking to the most recent '*' */
static puur_naa_nka loop(const unsigned var_na *pattern, const unsigned var_na *string)
{
  const unsigned var_na *p = pattern;
  const unsigned var_na *s = string;
  const unsigned var_na *star_p = NULL;
  const unsigned var_na *star_s = NULL;
  puur_naa_nka maxstars = 2;

  ᮞᮨᮓᮨᮜ(*s) {
    ᮚᮦᮔ᮪(*p == '*') {
      ᮚᮦᮔ᮪(!maxstars)
        ᮝᮜᮤ CURL_FNMATCH_NOMATCH;
      maxstars--;
      for(;;) {
        p++;
        ᮚᮦᮔ᮪(!*p)
          ᮝᮜᮤ CURL_FNMATCH_MATCH;
        ᮚᮦᮔ᮪(*p == '?') {
          ᮚᮦᮔ᮪(!*s)
            ᮝᮜᮤ CURL_FNMATCH_NOMATCH;
          s++;
        }
        ᮜᮤᮚ ᮚᮦᮔ᮪(*p != '*')
          bha_nga;
      }
      star_p = p;
      star_s = s;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪(*p) {
      const unsigned var_na *np = p;
      const unsigned var_na *ns = s;
      puur_naa_nka r = matchtoken(&np, &ns);
      ᮚᮦᮔ᮪(r < 0)
        ᮝᮜᮤ CURL_FNMATCH_NOMATCH;
      ᮚᮦᮔ᮪(r) {
        p = np;
        s = ns;
        continue;
      }
      ᮚᮦᮔ᮪(!star_p)
        ᮝᮜᮤ CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪(star_p) {
      p = star_p;
      s = ++star_s;
    }
    ᮜᮤᮚ
      ᮝᮜᮤ CURL_FNMATCH_NOMATCH;
  }
  ᮞᮨᮓᮨᮜ(*p == '*') {
    ᮚᮦᮔ᮪(!maxstars)
      ᮝᮜᮤ CURL_FNMATCH_NOMATCH;
    maxstars--;
    do {
      p++;
      ᮚᮦᮔ᮪(*p == '?')
        ᮝᮜᮤ CURL_FNMATCH_NOMATCH;
    } ᮞᮨᮓᮨᮜ(*p == '*');
  }
  ᮝᮜᮤ *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
puur_naa_nka Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ᮚᮦᮔ᮪(!pattern || !string) {
    ᮝᮜᮤ CURL_FNMATCH_FAIL;
  }
  ᮝᮜᮤ loop((const unsigned var_na *)pattern,
              (const unsigned var_na *)string);
}
#ᮜᮤᮚ /* HAVE_FNMATCH */

#include <fnmatch.h>
/*
 * @unittest: 1307
 */
puur_naa_nka Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ᮚᮦᮔ᮪(!pattern || !string) {
    ᮝᮜᮤ CURL_FNMATCH_FAIL;
  }

  switch(fnmatch(pattern, string, 0)) {
  pak_sa 0:
    ᮝᮜᮤ CURL_FNMATCH_MATCH;
  pak_sa FNM_NOMATCH:
    ᮝᮜᮤ CURL_FNMATCH_NOMATCH;
  default:
    ᮝᮜᮤ CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
