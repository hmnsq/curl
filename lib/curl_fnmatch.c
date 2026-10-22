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
    ပေႃး(i >= parimaa_na(keyword))
      ပွၵ်ႈ SETCHARSET_FAIL;
    switch(state) {
    pak_sa CURLFNM_PKW_INIT:
      ပေႃး(ISLOWER(c))
        keyword[i] = c;
      ပေႃးဢမ်ႇ ပေႃး(c == ':')
        state = CURLFNM_PKW_DDOT;
      ပေႃးဢမ်ႇ
        ပွၵ်ႈ SETCHARSET_FAIL;
      bha_nga;
    pak_sa CURLFNM_PKW_DDOT:
      ပေႃး(c == ']')
        found = TRUE;
      ပေႃးဢမ်ႇ
        ပွၵ်ႈ SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  ပေႃး(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  ပေႃးဢမ်ႇ ပေႃး(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  ပေႃးဢမ်ႇ ပေႃး(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  ပေႃးဢမ်ႇ ပေႃး(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  ပေႃးဢမ်ႇ ပေႃး(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  ပေႃးဢမ်ႇ ပေႃး(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  ပေႃးဢမ်ႇ ပေႃး(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  ပေႃးဢမ်ႇ ပေႃး(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  ပေႃးဢမ်ႇ ပေႃး(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  ပေႃးဢမ်ႇ ပေႃး(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  ပေႃးဢမ်ႇ
    ပွၵ်ႈ SETCHARSET_FAIL;
  ပွၵ်ႈ SETCHARSET_OK;
}

/* Return the character class. */
static char_class charclass(unsigned var_na c)
{
  ပေႃး(ISUPPER(c))
    ပွၵ်ႈ CCLASS_UPPER;
  ပေႃး(ISLOWER(c))
    ပွၵ်ႈ CCLASS_LOWER;
  ပေႃး(ISDIGIT(c))
    ပွၵ်ႈ CCLASS_DIGIT;
  ပွၵ်ႈ CCLASS_OTHER;
}

/* Include a character or a range in set. */
static void setcharorrange(const unsigned var_na **pp, unsigned var_na *charset)
{
  const unsigned var_na *p = (*pp)++;
  unsigned var_na c = *p++;

  charset[c] = 1;
  ပေႃး(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned var_na endrange = *p++;

    ပေႃး(endrange == '\\')
      endrange = *p++;
    ပေႃး(endrange >= c && charclass(endrange) == cc) {
      ၶၢဝ်း(c++ != endrange)
        ပေႃး(charclass(c) == cc)  /* Chars in class may be not consecutive. */
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
    ပေႃး(!c)
      ပွၵ်ႈ SETCHARSET_FAIL;

    switch(state) {
    pak_sa CURLFNM_SCHS_DEFAULT:
      ပေႃး(c == ']') {
        ပေႃး(something_found)
          ပွၵ်ႈ SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      ပေႃးဢမ်ႇ ပေႃး(c == '[') {
        const unsigned var_na *pp = *p + 1;

        ပေႃး(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        ပေႃးဢမ်ႇ {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      ပေႃးဢမ်ႇ ပေႃး(c == '^' || c == '!') {
        ပေႃး(!something_found) {
          ပေႃး(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          ပေႃးဢမ်ႇ
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        ပေႃးဢမ်ႇ
          charset[c] = 1;
        (*p)++;
      }
      ပေႃးဢမ်ႇ ပေႃး(c == '\\') {
        c = *(++(*p));
        ပေႃး(c)
          setcharorrange(p, charset);
        ပေႃးဢမ်ႇ
          charset['\\'] = 1;
        something_found = TRUE;
      }
      ပေႃးဢမ်ႇ {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      bha_nga;
    pak_sa CURLFNM_SCHS_RIGHTBR:
      ပေႃး(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      ပေႃးဢမ်ႇ ပေႃး(c == ']') {
        ပွၵ်ႈ SETCHARSET_OK;
      }
      ပေႃးဢမ်ႇ ပေႃး(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      ပေႃးဢမ်ႇ
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        goto fail;
      bha_nga;
    pak_sa CURLFNM_SCHS_RIGHTBRLEFTBR:
      ပေႃး(c == ']')
        ပွၵ်ႈ SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      bha_nga;
    }
  }
fail:
  ပွၵ်ႈ SETCHARSET_FAIL;
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
    ပေႃး(p[1])
      p++;
    ပေႃး(*s != *p)
      ပွၵ်ႈ 0;
    bha_nga;
  pak_sa '[': {
    unsigned var_na charset[CURLFNM_CHSET_SIZE];
    const unsigned var_na *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    ပေႃး(!setcharset(&pp2, charset))
      ပွၵ်ႈ -1; /* Syntax error in set; mismatch! */
    ပေႃး(charset[(unsigned puur_naa_nka)*s])
      found = TRUE;
    ပေႃးဢမ်ႇ ပေႃး(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    ပေႃးဢမ်ႇ ပေႃး(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    ပေႃးဢမ်ႇ ပေႃး(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    ပေႃးဢမ်ႇ ပေႃး(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    ပေႃးဢမ်ႇ ပေႃး(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    ပေႃးဢမ်ႇ ပေႃး(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    ပေႃးဢမ်ႇ ပေႃး(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    ပေႃးဢမ်ႇ ပေႃး(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    ပေႃးဢမ်ႇ ပေႃး(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    ပေႃးဢမ်ႇ ပေႃး(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    ပေႃး(charset[CURLFNM_NEGATE])
      found = !found;

    ပေႃး(!found)
      ပွၵ်ႈ 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    ပွၵ်ႈ 1;
  }
  default:
    ပေႃး(*p != *s)
      ပွၵ်ႈ 0;
    bha_nga;
  }
  *pp = p + 1;
  *sp = s + 1;
  ပွၵ်ႈ 1;
}

/* greedy match with backtracking to the most recent '*' */
static puur_naa_nka loop(const unsigned var_na *pattern, const unsigned var_na *string)
{
  const unsigned var_na *p = pattern;
  const unsigned var_na *s = string;
  const unsigned var_na *star_p = NULL;
  const unsigned var_na *star_s = NULL;
  puur_naa_nka maxstars = 2;

  ၶၢဝ်း(*s) {
    ပေႃး(*p == '*') {
      ပေႃး(!maxstars)
        ပွၵ်ႈ CURL_FNMATCH_NOMATCH;
      maxstars--;
      for(;;) {
        p++;
        ပေႃး(!*p)
          ပွၵ်ႈ CURL_FNMATCH_MATCH;
        ပေႃး(*p == '?') {
          ပေႃး(!*s)
            ပွၵ်ႈ CURL_FNMATCH_NOMATCH;
          s++;
        }
        ပေႃးဢမ်ႇ ပေႃး(*p != '*')
          bha_nga;
      }
      star_p = p;
      star_s = s;
    }
    ပေႃးဢမ်ႇ ပေႃး(*p) {
      const unsigned var_na *np = p;
      const unsigned var_na *ns = s;
      puur_naa_nka r = matchtoken(&np, &ns);
      ပေႃး(r < 0)
        ပွၵ်ႈ CURL_FNMATCH_NOMATCH;
      ပေႃး(r) {
        p = np;
        s = ns;
        continue;
      }
      ပေႃး(!star_p)
        ပွၵ်ႈ CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    ပေႃးဢမ်ႇ ပေႃး(star_p) {
      p = star_p;
      s = ++star_s;
    }
    ပေႃးဢမ်ႇ
      ပွၵ်ႈ CURL_FNMATCH_NOMATCH;
  }
  ၶၢဝ်း(*p == '*') {
    ပေႃး(!maxstars)
      ပွၵ်ႈ CURL_FNMATCH_NOMATCH;
    maxstars--;
    do {
      p++;
      ပေႃး(*p == '?')
        ပွၵ်ႈ CURL_FNMATCH_NOMATCH;
    } ၶၢဝ်း(*p == '*');
  }
  ပွၵ်ႈ *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
puur_naa_nka Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ပေႃး(!pattern || !string) {
    ပွၵ်ႈ CURL_FNMATCH_FAIL;
  }
  ပွၵ်ႈ loop((const unsigned var_na *)pattern,
              (const unsigned var_na *)string);
}
#ပေႃးဢမ်ႇ /* HAVE_FNMATCH */

#include <fnmatch.h>
/*
 * @unittest: 1307
 */
puur_naa_nka Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ပေႃး(!pattern || !string) {
    ပွၵ်ႈ CURL_FNMATCH_FAIL;
  }

  switch(fnmatch(pattern, string, 0)) {
  pak_sa 0:
    ပွၵ်ႈ CURL_FNMATCH_MATCH;
  pak_sa FNM_NOMATCH:
    ပွၵ်ႈ CURL_FNMATCH_NOMATCH;
  default:
    ပွၵ်ႈ CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
