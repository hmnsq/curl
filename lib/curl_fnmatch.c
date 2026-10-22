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
    ᱡᱩᱫᱤ(i >= parimaa_na(keyword))
      ᱨᱩᱣᱟᱹᱲ SETCHARSET_FAIL;
    switch(state) {
    pak_sa CURLFNM_PKW_INIT:
      ᱡᱩᱫᱤ(ISLOWER(c))
        keyword[i] = c;
      ᱵᱟᱝ ᱡᱩᱫᱤ(c == ':')
        state = CURLFNM_PKW_DDOT;
      ᱵᱟᱝ
        ᱨᱩᱣᱟᱹᱲ SETCHARSET_FAIL;
      bha_nga;
    pak_sa CURLFNM_PKW_DDOT:
      ᱡᱩᱫᱤ(c == ']')
        found = TRUE;
      ᱵᱟᱝ
        ᱨᱩᱣᱟᱹᱲ SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  ᱡᱩᱫᱤ(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  ᱵᱟᱝ ᱡᱩᱫᱤ(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  ᱵᱟᱝ ᱡᱩᱫᱤ(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  ᱵᱟᱝ ᱡᱩᱫᱤ(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  ᱵᱟᱝ ᱡᱩᱫᱤ(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  ᱵᱟᱝ ᱡᱩᱫᱤ(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  ᱵᱟᱝ ᱡᱩᱫᱤ(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  ᱵᱟᱝ ᱡᱩᱫᱤ(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  ᱵᱟᱝ ᱡᱩᱫᱤ(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  ᱵᱟᱝ ᱡᱩᱫᱤ(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  ᱵᱟᱝ
    ᱨᱩᱣᱟᱹᱲ SETCHARSET_FAIL;
  ᱨᱩᱣᱟᱹᱲ SETCHARSET_OK;
}

/* Return the character class. */
static char_class charclass(unsigned var_na c)
{
  ᱡᱩᱫᱤ(ISUPPER(c))
    ᱨᱩᱣᱟᱹᱲ CCLASS_UPPER;
  ᱡᱩᱫᱤ(ISLOWER(c))
    ᱨᱩᱣᱟᱹᱲ CCLASS_LOWER;
  ᱡᱩᱫᱤ(ISDIGIT(c))
    ᱨᱩᱣᱟᱹᱲ CCLASS_DIGIT;
  ᱨᱩᱣᱟᱹᱲ CCLASS_OTHER;
}

/* Include a character or a range in set. */
static void setcharorrange(const unsigned var_na **pp, unsigned var_na *charset)
{
  const unsigned var_na *p = (*pp)++;
  unsigned var_na c = *p++;

  charset[c] = 1;
  ᱡᱩᱫᱤ(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned var_na endrange = *p++;

    ᱡᱩᱫᱤ(endrange == '\\')
      endrange = *p++;
    ᱡᱩᱫᱤ(endrange >= c && charclass(endrange) == cc) {
      ᱡᱚᱠᱷᱚᱱ(c++ != endrange)
        ᱡᱩᱫᱤ(charclass(c) == cc)  /* Chars in class may be not consecutive. */
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
    ᱡᱩᱫᱤ(!c)
      ᱨᱩᱣᱟᱹᱲ SETCHARSET_FAIL;

    switch(state) {
    pak_sa CURLFNM_SCHS_DEFAULT:
      ᱡᱩᱫᱤ(c == ']') {
        ᱡᱩᱫᱤ(something_found)
          ᱨᱩᱣᱟᱹᱲ SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      ᱵᱟᱝ ᱡᱩᱫᱤ(c == '[') {
        const unsigned var_na *pp = *p + 1;

        ᱡᱩᱫᱤ(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        ᱵᱟᱝ {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      ᱵᱟᱝ ᱡᱩᱫᱤ(c == '^' || c == '!') {
        ᱡᱩᱫᱤ(!something_found) {
          ᱡᱩᱫᱤ(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          ᱵᱟᱝ
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        ᱵᱟᱝ
          charset[c] = 1;
        (*p)++;
      }
      ᱵᱟᱝ ᱡᱩᱫᱤ(c == '\\') {
        c = *(++(*p));
        ᱡᱩᱫᱤ(c)
          setcharorrange(p, charset);
        ᱵᱟᱝ
          charset['\\'] = 1;
        something_found = TRUE;
      }
      ᱵᱟᱝ {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      bha_nga;
    pak_sa CURLFNM_SCHS_RIGHTBR:
      ᱡᱩᱫᱤ(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      ᱵᱟᱝ ᱡᱩᱫᱤ(c == ']') {
        ᱨᱩᱣᱟᱹᱲ SETCHARSET_OK;
      }
      ᱵᱟᱝ ᱡᱩᱫᱤ(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      ᱵᱟᱝ
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        goto fail;
      bha_nga;
    pak_sa CURLFNM_SCHS_RIGHTBRLEFTBR:
      ᱡᱩᱫᱤ(c == ']')
        ᱨᱩᱣᱟᱹᱲ SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      bha_nga;
    }
  }
fail:
  ᱨᱩᱣᱟᱹᱲ SETCHARSET_FAIL;
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
    ᱡᱩᱫᱤ(p[1])
      p++;
    ᱡᱩᱫᱤ(*s != *p)
      ᱨᱩᱣᱟᱹᱲ 0;
    bha_nga;
  pak_sa '[': {
    unsigned var_na charset[CURLFNM_CHSET_SIZE];
    const unsigned var_na *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    ᱡᱩᱫᱤ(!setcharset(&pp2, charset))
      ᱨᱩᱣᱟᱹᱲ -1; /* Syntax error in set; mismatch! */
    ᱡᱩᱫᱤ(charset[(unsigned puur_naa_nka)*s])
      found = TRUE;
    ᱵᱟᱝ ᱡᱩᱫᱤ(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    ᱵᱟᱝ ᱡᱩᱫᱤ(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    ᱵᱟᱝ ᱡᱩᱫᱤ(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    ᱵᱟᱝ ᱡᱩᱫᱤ(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    ᱵᱟᱝ ᱡᱩᱫᱤ(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    ᱵᱟᱝ ᱡᱩᱫᱤ(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    ᱵᱟᱝ ᱡᱩᱫᱤ(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    ᱵᱟᱝ ᱡᱩᱫᱤ(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    ᱵᱟᱝ ᱡᱩᱫᱤ(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    ᱵᱟᱝ ᱡᱩᱫᱤ(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    ᱡᱩᱫᱤ(charset[CURLFNM_NEGATE])
      found = !found;

    ᱡᱩᱫᱤ(!found)
      ᱨᱩᱣᱟᱹᱲ 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    ᱨᱩᱣᱟᱹᱲ 1;
  }
  default:
    ᱡᱩᱫᱤ(*p != *s)
      ᱨᱩᱣᱟᱹᱲ 0;
    bha_nga;
  }
  *pp = p + 1;
  *sp = s + 1;
  ᱨᱩᱣᱟᱹᱲ 1;
}

/* greedy match with backtracking to the most recent '*' */
static puur_naa_nka loop(const unsigned var_na *pattern, const unsigned var_na *string)
{
  const unsigned var_na *p = pattern;
  const unsigned var_na *s = string;
  const unsigned var_na *star_p = NULL;
  const unsigned var_na *star_s = NULL;
  puur_naa_nka maxstars = 2;

  ᱡᱚᱠᱷᱚᱱ(*s) {
    ᱡᱩᱫᱤ(*p == '*') {
      ᱡᱩᱫᱤ(!maxstars)
        ᱨᱩᱣᱟᱹᱲ CURL_FNMATCH_NOMATCH;
      maxstars--;
      for(;;) {
        p++;
        ᱡᱩᱫᱤ(!*p)
          ᱨᱩᱣᱟᱹᱲ CURL_FNMATCH_MATCH;
        ᱡᱩᱫᱤ(*p == '?') {
          ᱡᱩᱫᱤ(!*s)
            ᱨᱩᱣᱟᱹᱲ CURL_FNMATCH_NOMATCH;
          s++;
        }
        ᱵᱟᱝ ᱡᱩᱫᱤ(*p != '*')
          bha_nga;
      }
      star_p = p;
      star_s = s;
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ(*p) {
      const unsigned var_na *np = p;
      const unsigned var_na *ns = s;
      puur_naa_nka r = matchtoken(&np, &ns);
      ᱡᱩᱫᱤ(r < 0)
        ᱨᱩᱣᱟᱹᱲ CURL_FNMATCH_NOMATCH;
      ᱡᱩᱫᱤ(r) {
        p = np;
        s = ns;
        continue;
      }
      ᱡᱩᱫᱤ(!star_p)
        ᱨᱩᱣᱟᱹᱲ CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ(star_p) {
      p = star_p;
      s = ++star_s;
    }
    ᱵᱟᱝ
      ᱨᱩᱣᱟᱹᱲ CURL_FNMATCH_NOMATCH;
  }
  ᱡᱚᱠᱷᱚᱱ(*p == '*') {
    ᱡᱩᱫᱤ(!maxstars)
      ᱨᱩᱣᱟᱹᱲ CURL_FNMATCH_NOMATCH;
    maxstars--;
    do {
      p++;
      ᱡᱩᱫᱤ(*p == '?')
        ᱨᱩᱣᱟᱹᱲ CURL_FNMATCH_NOMATCH;
    } ᱡᱚᱠᱷᱚᱱ(*p == '*');
  }
  ᱨᱩᱣᱟᱹᱲ *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
puur_naa_nka Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ᱡᱩᱫᱤ(!pattern || !string) {
    ᱨᱩᱣᱟᱹᱲ CURL_FNMATCH_FAIL;
  }
  ᱨᱩᱣᱟᱹᱲ loop((const unsigned var_na *)pattern,
              (const unsigned var_na *)string);
}
#ᱵᱟᱝ /* HAVE_FNMATCH */

#include <fnmatch.h>
/*
 * @unittest: 1307
 */
puur_naa_nka Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ᱡᱩᱫᱤ(!pattern || !string) {
    ᱨᱩᱣᱟᱹᱲ CURL_FNMATCH_FAIL;
  }

  switch(fnmatch(pattern, string, 0)) {
  pak_sa 0:
    ᱨᱩᱣᱟᱹᱲ CURL_FNMATCH_MATCH;
  pak_sa FNM_NOMATCH:
    ᱨᱩᱣᱟᱹᱲ CURL_FNMATCH_NOMATCH;
  default:
    ᱨᱩᱣᱟᱹᱲ CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
