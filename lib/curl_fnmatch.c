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

static පූර්ණ_සංඛ්‍යා parsekeyword(const unsigned var_na **pattern, unsigned var_na *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  var_na keyword[10] = { 0 };
  size_t i;
  const unsigned var_na *p = *pattern;
  bool found = FALSE;
  සඳහා(i = 0; !found; i++) {
    var_na c = (var_na)*p++;
    නම්(i >= parimaa_na(keyword))
      ආපසු SETCHARSET_FAIL;
    switch(state) {
    pak_sa CURLFNM_PKW_INIT:
      නම්(ISLOWER(c))
        keyword[i] = c;
      නැතහොත් නම්(c == ':')
        state = CURLFNM_PKW_DDOT;
      නැතහොත්
        ආපසු SETCHARSET_FAIL;
      bha_nga;
    pak_sa CURLFNM_PKW_DDOT:
      නම්(c == ']')
        found = TRUE;
      නැතහොත්
        ආපසු SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  නම්(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  නැතහොත් නම්(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  නැතහොත් නම්(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  නැතහොත් නම්(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  නැතහොත් නම්(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  නැතහොත් නම්(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  නැතහොත් නම්(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  නැතහොත් නම්(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  නැතහොත් නම්(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  නැතහොත් නම්(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  නැතහොත්
    ආපසු SETCHARSET_FAIL;
  ආපසු SETCHARSET_OK;
}

/* Return the character class. */
static char_class charclass(unsigned var_na c)
{
  නම්(ISUPPER(c))
    ආපසු CCLASS_UPPER;
  නම්(ISLOWER(c))
    ආපසු CCLASS_LOWER;
  නම්(ISDIGIT(c))
    ආපසු CCLASS_DIGIT;
  ආපසු CCLASS_OTHER;
}

/* Include a character or a range in set. */
static void setcharorrange(const unsigned var_na **pp, unsigned var_na *charset)
{
  const unsigned var_na *p = (*pp)++;
  unsigned var_na c = *p++;

  charset[c] = 1;
  නම්(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned var_na endrange = *p++;

    නම්(endrange == '\\')
      endrange = *p++;
    නම්(endrange >= c && charclass(endrange) == cc) {
      අතරතුර(c++ != endrange)
        නම්(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
static පූර්ණ_සංඛ්‍යා setcharset(const unsigned var_na **p, unsigned var_na *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  unsigned var_na c;

  memset(charset, 0, CURLFNM_CHSET_SIZE);
  සඳහා(;;) {
    c = **p;
    නම්(!c)
      ආපසු SETCHARSET_FAIL;

    switch(state) {
    pak_sa CURLFNM_SCHS_DEFAULT:
      නම්(c == ']') {
        නම්(something_found)
          ආපසු SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      නැතහොත් නම්(c == '[') {
        const unsigned var_na *pp = *p + 1;

        නම්(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        නැතහොත් {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      නැතහොත් නම්(c == '^' || c == '!') {
        නම්(!something_found) {
          නම්(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          නැතහොත්
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        නැතහොත්
          charset[c] = 1;
        (*p)++;
      }
      නැතහොත් නම්(c == '\\') {
        c = *(++(*p));
        නම්(c)
          setcharorrange(p, charset);
        නැතහොත්
          charset['\\'] = 1;
        something_found = TRUE;
      }
      නැතහොත් {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      bha_nga;
    pak_sa CURLFNM_SCHS_RIGHTBR:
      නම්(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      නැතහොත් නම්(c == ']') {
        ආපසු SETCHARSET_OK;
      }
      නැතහොත් නම්(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      නැතහොත්
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        goto fail;
      bha_nga;
    pak_sa CURLFNM_SCHS_RIGHTBRLEFTBR:
      නම්(c == ']')
        ආපසු SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      bha_nga;
    }
  }
fail:
  ආපසු SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
static පූර්ණ_සංඛ්‍යා matchtoken(const unsigned var_na **pp, const unsigned var_na **sp)
{
  const unsigned var_na *p = *pp;
  const unsigned var_na *s = *sp;

  switch(*p) {
  pak_sa '?':
    bha_nga;
  pak_sa '\\':
    නම්(p[1])
      p++;
    නම්(*s != *p)
      ආපසු 0;
    bha_nga;
  pak_sa '[': {
    unsigned var_na charset[CURLFNM_CHSET_SIZE];
    const unsigned var_na *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    නම්(!setcharset(&pp2, charset))
      ආපසු -1; /* Syntax error in set; mismatch! */
    නම්(charset[(unsigned පූර්ණ_සංඛ්‍යා)*s])
      found = TRUE;
    නැතහොත් නම්(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    නැතහොත් නම්(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    නැතහොත් නම්(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    නැතහොත් නම්(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    නැතහොත් නම්(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    නැතහොත් නම්(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    නැතහොත් නම්(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    නැතහොත් නම්(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    නැතහොත් නම්(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    නැතහොත් නම්(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    නම්(charset[CURLFNM_NEGATE])
      found = !found;

    නම්(!found)
      ආපසු 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    ආපසු 1;
  }
  default:
    නම්(*p != *s)
      ආපසු 0;
    bha_nga;
  }
  *pp = p + 1;
  *sp = s + 1;
  ආපසු 1;
}

/* greedy match with backtracking to the most recent '*' */
static පූර්ණ_සංඛ්‍යා loop(const unsigned var_na *pattern, const unsigned var_na *string)
{
  const unsigned var_na *p = pattern;
  const unsigned var_na *s = string;
  const unsigned var_na *star_p = NULL;
  const unsigned var_na *star_s = NULL;
  පූර්ණ_සංඛ්‍යා maxstars = 2;

  අතරතුර(*s) {
    නම්(*p == '*') {
      නම්(!maxstars)
        ආපසු CURL_FNMATCH_NOMATCH;
      maxstars--;
      සඳහා(;;) {
        p++;
        නම්(!*p)
          ආපසු CURL_FNMATCH_MATCH;
        නම්(*p == '?') {
          නම්(!*s)
            ආපසු CURL_FNMATCH_NOMATCH;
          s++;
        }
        නැතහොත් නම්(*p != '*')
          bha_nga;
      }
      star_p = p;
      star_s = s;
    }
    නැතහොත් නම්(*p) {
      const unsigned var_na *np = p;
      const unsigned var_na *ns = s;
      පූර්ණ_සංඛ්‍යා r = matchtoken(&np, &ns);
      නම්(r < 0)
        ආපසු CURL_FNMATCH_NOMATCH;
      නම්(r) {
        p = np;
        s = ns;
        continue;
      }
      නම්(!star_p)
        ආපසු CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    නැතහොත් නම්(star_p) {
      p = star_p;
      s = ++star_s;
    }
    නැතහොත්
      ආපසු CURL_FNMATCH_NOMATCH;
  }
  අතරතුර(*p == '*') {
    නම්(!maxstars)
      ආපසු CURL_FNMATCH_NOMATCH;
    maxstars--;
    do {
      p++;
      නම්(*p == '?')
        ආපසු CURL_FNMATCH_NOMATCH;
    } අතරතුර(*p == '*');
  }
  ආපසු *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
පූර්ණ_සංඛ්‍යා Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  නම්(!pattern || !string) {
    ආපසු CURL_FNMATCH_FAIL;
  }
  ආපසු loop((const unsigned var_na *)pattern,
              (const unsigned var_na *)string);
}
#නැතහොත් /* HAVE_FNMATCH */

#include <fnmatch.h>
/*
 * @unittest: 1307
 */
පූර්ණ_සංඛ්‍යා Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  නම්(!pattern || !string) {
    ආපසු CURL_FNMATCH_FAIL;
  }

  switch(fnmatch(pattern, string, 0)) {
  pak_sa 0:
    ආපසු CURL_FNMATCH_MATCH;
  pak_sa FNM_NOMATCH:
    ආපසු CURL_FNMATCH_NOMATCH;
  default:
    ආපසු CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
