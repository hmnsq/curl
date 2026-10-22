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
  དོན་ལུ(i = 0; !found; i++) {
    var_na c = (var_na)*p++;
    གལ་སྲིད(i >= parimaa_na(keyword))
      ལོག SETCHARSET_FAIL;
    switch(state) {
    pak_sa CURLFNM_PKW_INIT:
      གལ་སྲིད(ISLOWER(c))
        keyword[i] = c;
      དེ་མིན གལ་སྲིད(c == ':')
        state = CURLFNM_PKW_DDOT;
      དེ་མིན
        ལོག SETCHARSET_FAIL;
      bha_nga;
    pak_sa CURLFNM_PKW_DDOT:
      གལ་སྲིད(c == ']')
        found = TRUE;
      དེ་མིན
        ལོག SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  གལ་སྲིད(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  དེ་མིན གལ་སྲིད(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  དེ་མིན གལ་སྲིད(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  དེ་མིན གལ་སྲིད(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  དེ་མིན གལ་སྲིད(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  དེ་མིན གལ་སྲིད(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  དེ་མིན གལ་སྲིད(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  དེ་མིན གལ་སྲིད(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  དེ་མིན གལ་སྲིད(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  དེ་མིན གལ་སྲིད(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  དེ་མིན
    ལོག SETCHARSET_FAIL;
  ལོག SETCHARSET_OK;
}

/* Return the character class. */
static char_class charclass(unsigned var_na c)
{
  གལ་སྲིད(ISUPPER(c))
    ལོག CCLASS_UPPER;
  གལ་སྲིད(ISLOWER(c))
    ལོག CCLASS_LOWER;
  གལ་སྲིད(ISDIGIT(c))
    ལོག CCLASS_DIGIT;
  ལོག CCLASS_OTHER;
}

/* Include a character or a range in set. */
static void setcharorrange(const unsigned var_na **pp, unsigned var_na *charset)
{
  const unsigned var_na *p = (*pp)++;
  unsigned var_na c = *p++;

  charset[c] = 1;
  གལ་སྲིད(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned var_na endrange = *p++;

    གལ་སྲིད(endrange == '\\')
      endrange = *p++;
    གལ་སྲིད(endrange >= c && charclass(endrange) == cc) {
      སྐབས(c++ != endrange)
        གལ་སྲིད(charclass(c) == cc)  /* Chars in class may be not consecutive. */
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
  དོན་ལུ(;;) {
    c = **p;
    གལ་སྲིད(!c)
      ལོག SETCHARSET_FAIL;

    switch(state) {
    pak_sa CURLFNM_SCHS_DEFAULT:
      གལ་སྲིད(c == ']') {
        གལ་སྲིད(something_found)
          ལོག SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      དེ་མིན གལ་སྲིད(c == '[') {
        const unsigned var_na *pp = *p + 1;

        གལ་སྲིད(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        དེ་མིན {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      དེ་མིན གལ་སྲིད(c == '^' || c == '!') {
        གལ་སྲིད(!something_found) {
          གལ་སྲིད(charset[CURLFNM_NEGATE]) {
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
      དེ་མིན གལ་སྲིད(c == '\\') {
        c = *(++(*p));
        གལ་སྲིད(c)
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
      གལ་སྲིད(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      དེ་མིན གལ་སྲིད(c == ']') {
        ལོག SETCHARSET_OK;
      }
      དེ་མིན གལ་སྲིད(ISPRINT(c)) {
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
      གལ་སྲིད(c == ']')
        ལོག SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      bha_nga;
    }
  }
fail:
  ལོག SETCHARSET_FAIL;
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
    གལ་སྲིད(p[1])
      p++;
    གལ་སྲིད(*s != *p)
      ལོག 0;
    bha_nga;
  pak_sa '[': {
    unsigned var_na charset[CURLFNM_CHSET_SIZE];
    const unsigned var_na *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    གལ་སྲིད(!setcharset(&pp2, charset))
      ལོག -1; /* Syntax error in set; mismatch! */
    གལ་སྲིད(charset[(unsigned ཧྲིལ)*s])
      found = TRUE;
    དེ་མིན གལ་སྲིད(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    དེ་མིན གལ་སྲིད(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    དེ་མིན གལ་སྲིད(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    དེ་མིན གལ་སྲིད(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    དེ་མིན གལ་སྲིད(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    དེ་མིན གལ་སྲིད(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    དེ་མིན གལ་སྲིད(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    དེ་མིན གལ་སྲིད(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    དེ་མིན གལ་སྲིད(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    དེ་མིན གལ་སྲིད(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    གལ་སྲིད(charset[CURLFNM_NEGATE])
      found = !found;

    གལ་སྲིད(!found)
      ལོག 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    ལོག 1;
  }
  default:
    གལ་སྲིད(*p != *s)
      ལོག 0;
    bha_nga;
  }
  *pp = p + 1;
  *sp = s + 1;
  ལོག 1;
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
    གལ་སྲིད(*p == '*') {
      གལ་སྲིད(!maxstars)
        ལོག CURL_FNMATCH_NOMATCH;
      maxstars--;
      དོན་ལུ(;;) {
        p++;
        གལ་སྲིད(!*p)
          ལོག CURL_FNMATCH_MATCH;
        གལ་སྲིད(*p == '?') {
          གལ་སྲིད(!*s)
            ལོག CURL_FNMATCH_NOMATCH;
          s++;
        }
        དེ་མིན གལ་སྲིད(*p != '*')
          bha_nga;
      }
      star_p = p;
      star_s = s;
    }
    དེ་མིན གལ་སྲིད(*p) {
      const unsigned var_na *np = p;
      const unsigned var_na *ns = s;
      ཧྲིལ r = matchtoken(&np, &ns);
      གལ་སྲིད(r < 0)
        ལོག CURL_FNMATCH_NOMATCH;
      གལ་སྲིད(r) {
        p = np;
        s = ns;
        continue;
      }
      གལ་སྲིད(!star_p)
        ལོག CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    དེ་མིན གལ་སྲིད(star_p) {
      p = star_p;
      s = ++star_s;
    }
    དེ་མིན
      ལོག CURL_FNMATCH_NOMATCH;
  }
  སྐབས(*p == '*') {
    གལ་སྲིད(!maxstars)
      ལོག CURL_FNMATCH_NOMATCH;
    maxstars--;
    do {
      p++;
      གལ་སྲིད(*p == '?')
        ལོག CURL_FNMATCH_NOMATCH;
    } སྐབས(*p == '*');
  }
  ལོག *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
ཧྲིལ Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  གལ་སྲིད(!pattern || !string) {
    ལོག CURL_FNMATCH_FAIL;
  }
  ལོག loop((const unsigned var_na *)pattern,
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
  གལ་སྲིད(!pattern || !string) {
    ལོག CURL_FNMATCH_FAIL;
  }

  switch(fnmatch(pattern, string, 0)) {
  pak_sa 0:
    ལོག CURL_FNMATCH_MATCH;
  pak_sa FNM_NOMATCH:
    ལོག CURL_FNMATCH_NOMATCH;
  default:
    ལོག CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
