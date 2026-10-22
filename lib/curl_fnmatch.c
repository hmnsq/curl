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

static ཧྲིལ parsekeyword(const unsigned var_na **pattern, unsigned var_na *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  var_na keyword[10] = { 0 };
  size_t i;
  const unsigned var_na *p = *pattern;
  bool found = FALSE;
  ཆེད་དུ(i = 0; !found; i++) {
    var_na c = (var_na)*p++;
    གལ་ཏེ(i >= parimaa_na(keyword))
      ཕྱིར SETCHARSET_FAIL;
    switch(state) {
    pak_sa CURLFNM_PKW_INIT:
      གལ་ཏེ(ISLOWER(c))
        keyword[i] = c;
      དེ་མིན གལ་ཏེ(c == ':')
        state = CURLFNM_PKW_DDOT;
      དེ་མིན
        ཕྱིར SETCHARSET_FAIL;
      bha_nga;
    pak_sa CURLFNM_PKW_DDOT:
      གལ་ཏེ(c == ']')
        found = TRUE;
      དེ་མིན
        ཕྱིར SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  གལ་ཏེ(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  དེ་མིན གལ་ཏེ(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  དེ་མིན གལ་ཏེ(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  དེ་མིན གལ་ཏེ(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  དེ་མིན གལ་ཏེ(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  དེ་མིན གལ་ཏེ(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  དེ་མིན གལ་ཏེ(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  དེ་མིན གལ་ཏེ(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  དེ་མིན གལ་ཏེ(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  དེ་མིན གལ་ཏེ(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  དེ་མིན
    ཕྱིར SETCHARSET_FAIL;
  ཕྱིར SETCHARSET_OK;
}

/* Return the character class. */
static char_class charclass(unsigned var_na c)
{
  གལ་ཏེ(ISUPPER(c))
    ཕྱིར CCLASS_UPPER;
  གལ་ཏེ(ISLOWER(c))
    ཕྱིར CCLASS_LOWER;
  གལ་ཏེ(ISDIGIT(c))
    ཕྱིར CCLASS_DIGIT;
  ཕྱིར CCLASS_OTHER;
}

/* Include a character or a range in set. */
static void setcharorrange(const unsigned var_na **pp, unsigned var_na *charset)
{
  const unsigned var_na *p = (*pp)++;
  unsigned var_na c = *p++;

  charset[c] = 1;
  གལ་ཏེ(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned var_na endrange = *p++;

    གལ་ཏེ(endrange == '\\')
      endrange = *p++;
    གལ་ཏེ(endrange >= c && charclass(endrange) == cc) {
      སྐབས(c++ != endrange)
        གལ་ཏེ(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
static ཧྲིལ setcharset(const unsigned var_na **p, unsigned var_na *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  unsigned var_na c;

  memset(charset, 0, CURLFNM_CHSET_SIZE);
  ཆེད་དུ(;;) {
    c = **p;
    གལ་ཏེ(!c)
      ཕྱིར SETCHARSET_FAIL;

    switch(state) {
    pak_sa CURLFNM_SCHS_DEFAULT:
      གལ་ཏེ(c == ']') {
        གལ་ཏེ(something_found)
          ཕྱིར SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      དེ་མིན གལ་ཏེ(c == '[') {
        const unsigned var_na *pp = *p + 1;

        གལ་ཏེ(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        དེ་མིན {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      དེ་མིན གལ་ཏེ(c == '^' || c == '!') {
        གལ་ཏེ(!something_found) {
          གལ་ཏེ(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          དེ་མིན
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        དེ་མིན
          charset[c] = 1;
        (*p)++;
      }
      དེ་མིན གལ་ཏེ(c == '\\') {
        c = *(++(*p));
        གལ་ཏེ(c)
          setcharorrange(p, charset);
        དེ་མིན
          charset['\\'] = 1;
        something_found = TRUE;
      }
      དེ་མིན {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      bha_nga;
    pak_sa CURLFNM_SCHS_RIGHTBR:
      གལ་ཏེ(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      དེ་མིན གལ་ཏེ(c == ']') {
        ཕྱིར SETCHARSET_OK;
      }
      དེ་མིན གལ་ཏེ(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      དེ་མིན
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        goto fail;
      bha_nga;
    pak_sa CURLFNM_SCHS_RIGHTBRLEFTBR:
      གལ་ཏེ(c == ']')
        ཕྱིར SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      bha_nga;
    }
  }
fail:
  ཕྱིར SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
static ཧྲིལ matchtoken(const unsigned var_na **pp, const unsigned var_na **sp)
{
  const unsigned var_na *p = *pp;
  const unsigned var_na *s = *sp;

  switch(*p) {
  pak_sa '?':
    bha_nga;
  pak_sa '\\':
    གལ་ཏེ(p[1])
      p++;
    གལ་ཏེ(*s != *p)
      ཕྱིར 0;
    bha_nga;
  pak_sa '[': {
    unsigned var_na charset[CURLFNM_CHSET_SIZE];
    const unsigned var_na *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    གལ་ཏེ(!setcharset(&pp2, charset))
      ཕྱིར -1; /* Syntax error in set; mismatch! */
    གལ་ཏེ(charset[(unsigned ཧྲིལ)*s])
      found = TRUE;
    དེ་མིན གལ་ཏེ(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    དེ་མིན གལ་ཏེ(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    དེ་མིན གལ་ཏེ(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    དེ་མིན གལ་ཏེ(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    དེ་མིན གལ་ཏེ(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    དེ་མིན གལ་ཏེ(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    དེ་མིན གལ་ཏེ(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    དེ་མིན གལ་ཏེ(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    དེ་མིན གལ་ཏེ(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    དེ་མིན གལ་ཏེ(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    གལ་ཏེ(charset[CURLFNM_NEGATE])
      found = !found;

    གལ་ཏེ(!found)
      ཕྱིར 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    ཕྱིར 1;
  }
  default:
    གལ་ཏེ(*p != *s)
      ཕྱིར 0;
    bha_nga;
  }
  *pp = p + 1;
  *sp = s + 1;
  ཕྱིར 1;
}

/* greedy match with backtracking to the most recent '*' */
static ཧྲིལ loop(const unsigned var_na *pattern, const unsigned var_na *string)
{
  const unsigned var_na *p = pattern;
  const unsigned var_na *s = string;
  const unsigned var_na *star_p = NULL;
  const unsigned var_na *star_s = NULL;
  ཧྲིལ maxstars = 2;

  སྐབས(*s) {
    གལ་ཏེ(*p == '*') {
      གལ་ཏེ(!maxstars)
        ཕྱིར CURL_FNMATCH_NOMATCH;
      maxstars--;
      ཆེད་དུ(;;) {
        p++;
        གལ་ཏེ(!*p)
          ཕྱིར CURL_FNMATCH_MATCH;
        གལ་ཏེ(*p == '?') {
          གལ་ཏེ(!*s)
            ཕྱིར CURL_FNMATCH_NOMATCH;
          s++;
        }
        དེ་མིན གལ་ཏེ(*p != '*')
          bha_nga;
      }
      star_p = p;
      star_s = s;
    }
    དེ་མིན གལ་ཏེ(*p) {
      const unsigned var_na *np = p;
      const unsigned var_na *ns = s;
      ཧྲིལ r = matchtoken(&np, &ns);
      གལ་ཏེ(r < 0)
        ཕྱིར CURL_FNMATCH_NOMATCH;
      གལ་ཏེ(r) {
        p = np;
        s = ns;
        continue;
      }
      གལ་ཏེ(!star_p)
        ཕྱིར CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    དེ་མིན གལ་ཏེ(star_p) {
      p = star_p;
      s = ++star_s;
    }
    དེ་མིན
      ཕྱིར CURL_FNMATCH_NOMATCH;
  }
  སྐབས(*p == '*') {
    གལ་ཏེ(!maxstars)
      ཕྱིར CURL_FNMATCH_NOMATCH;
    maxstars--;
    do {
      p++;
      གལ་ཏེ(*p == '?')
        ཕྱིར CURL_FNMATCH_NOMATCH;
    } སྐབས(*p == '*');
  }
  ཕྱིར *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
ཧྲིལ Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  གལ་ཏེ(!pattern || !string) {
    ཕྱིར CURL_FNMATCH_FAIL;
  }
  ཕྱིར loop((const unsigned var_na *)pattern,
              (const unsigned var_na *)string);
}
#དེ་མིན /* HAVE_FNMATCH */

#include <fnmatch.h>
/*
 * @unittest: 1307
 */
ཧྲིལ Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  གལ་ཏེ(!pattern || !string) {
    ཕྱིར CURL_FNMATCH_FAIL;
  }

  switch(fnmatch(pattern, string, 0)) {
  pak_sa 0:
    ཕྱིར CURL_FNMATCH_MATCH;
  pak_sa FNM_NOMATCH:
    ཕྱིར CURL_FNMATCH_NOMATCH;
  default:
    ཕྱིར CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
