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

static ຈຳນວນເຕັມ parsekeyword(const unsigned var_na **pattern, unsigned var_na *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  var_na keyword[10] = { 0 };
  size_t i;
  const unsigned var_na *p = *pattern;
  bool found = FALSE;
  ສຳລັບ(i = 0; !found; i++) {
    var_na c = (var_na)*p++;
    ຖ້າ(i >= parimaa_na(keyword))
      ສົ່ງຄືນ SETCHARSET_FAIL;
    switch(state) {
    pak_sa CURLFNM_PKW_INIT:
      ຖ້າ(ISLOWER(c))
        keyword[i] = c;
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(c == ':')
        state = CURLFNM_PKW_DDOT;
      ບໍ່ດັ່ງນັ້ນ
        ສົ່ງຄືນ SETCHARSET_FAIL;
      bha_nga;
    pak_sa CURLFNM_PKW_DDOT:
      ຖ້າ(c == ']')
        found = TRUE;
      ບໍ່ດັ່ງນັ້ນ
        ສົ່ງຄືນ SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  ຖ້າ(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  ບໍ່ດັ່ງນັ້ນ
    ສົ່ງຄືນ SETCHARSET_FAIL;
  ສົ່ງຄືນ SETCHARSET_OK;
}

/* Return the character class. */
static char_class charclass(unsigned var_na c)
{
  ຖ້າ(ISUPPER(c))
    ສົ່ງຄືນ CCLASS_UPPER;
  ຖ້າ(ISLOWER(c))
    ສົ່ງຄືນ CCLASS_LOWER;
  ຖ້າ(ISDIGIT(c))
    ສົ່ງຄືນ CCLASS_DIGIT;
  ສົ່ງຄືນ CCLASS_OTHER;
}

/* Include a character or a range in set. */
static void setcharorrange(const unsigned var_na **pp, unsigned var_na *charset)
{
  const unsigned var_na *p = (*pp)++;
  unsigned var_na c = *p++;

  charset[c] = 1;
  ຖ້າ(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned var_na endrange = *p++;

    ຖ້າ(endrange == '\\')
      endrange = *p++;
    ຖ້າ(endrange >= c && charclass(endrange) == cc) {
      ໃນຂະນະທີ່(c++ != endrange)
        ຖ້າ(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
static ຈຳນວນເຕັມ setcharset(const unsigned var_na **p, unsigned var_na *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  unsigned var_na c;

  memset(charset, 0, CURLFNM_CHSET_SIZE);
  ສຳລັບ(;;) {
    c = **p;
    ຖ້າ(!c)
      ສົ່ງຄືນ SETCHARSET_FAIL;

    switch(state) {
    pak_sa CURLFNM_SCHS_DEFAULT:
      ຖ້າ(c == ']') {
        ຖ້າ(something_found)
          ສົ່ງຄືນ SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(c == '[') {
        const unsigned var_na *pp = *p + 1;

        ຖ້າ(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        ບໍ່ດັ່ງນັ້ນ {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(c == '^' || c == '!') {
        ຖ້າ(!something_found) {
          ຖ້າ(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          ບໍ່ດັ່ງນັ້ນ
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        ບໍ່ດັ່ງນັ້ນ
          charset[c] = 1;
        (*p)++;
      }
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(c == '\\') {
        c = *(++(*p));
        ຖ້າ(c)
          setcharorrange(p, charset);
        ບໍ່ດັ່ງນັ້ນ
          charset['\\'] = 1;
        something_found = TRUE;
      }
      ບໍ່ດັ່ງນັ້ນ {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      bha_nga;
    pak_sa CURLFNM_SCHS_RIGHTBR:
      ຖ້າ(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(c == ']') {
        ສົ່ງຄືນ SETCHARSET_OK;
      }
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      ບໍ່ດັ່ງນັ້ນ
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        goto fail;
      bha_nga;
    pak_sa CURLFNM_SCHS_RIGHTBRLEFTBR:
      ຖ້າ(c == ']')
        ສົ່ງຄືນ SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      bha_nga;
    }
  }
fail:
  ສົ່ງຄືນ SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
static ຈຳນວນເຕັມ matchtoken(const unsigned var_na **pp, const unsigned var_na **sp)
{
  const unsigned var_na *p = *pp;
  const unsigned var_na *s = *sp;

  switch(*p) {
  pak_sa '?':
    bha_nga;
  pak_sa '\\':
    ຖ້າ(p[1])
      p++;
    ຖ້າ(*s != *p)
      ສົ່ງຄືນ 0;
    bha_nga;
  pak_sa '[': {
    unsigned var_na charset[CURLFNM_CHSET_SIZE];
    const unsigned var_na *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    ຖ້າ(!setcharset(&pp2, charset))
      ສົ່ງຄືນ -1; /* Syntax error in set; mismatch! */
    ຖ້າ(charset[(unsigned ຈຳນວນເຕັມ)*s])
      found = TRUE;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    ຖ້າ(charset[CURLFNM_NEGATE])
      found = !found;

    ຖ້າ(!found)
      ສົ່ງຄືນ 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    ສົ່ງຄືນ 1;
  }
  default:
    ຖ້າ(*p != *s)
      ສົ່ງຄືນ 0;
    bha_nga;
  }
  *pp = p + 1;
  *sp = s + 1;
  ສົ່ງຄືນ 1;
}

/* greedy match with backtracking to the most recent '*' */
static ຈຳນວນເຕັມ loop(const unsigned var_na *pattern, const unsigned var_na *string)
{
  const unsigned var_na *p = pattern;
  const unsigned var_na *s = string;
  const unsigned var_na *star_p = NULL;
  const unsigned var_na *star_s = NULL;
  ຈຳນວນເຕັມ maxstars = 2;

  ໃນຂະນະທີ່(*s) {
    ຖ້າ(*p == '*') {
      ຖ້າ(!maxstars)
        ສົ່ງຄືນ CURL_FNMATCH_NOMATCH;
      maxstars--;
      ສຳລັບ(;;) {
        p++;
        ຖ້າ(!*p)
          ສົ່ງຄືນ CURL_FNMATCH_MATCH;
        ຖ້າ(*p == '?') {
          ຖ້າ(!*s)
            ສົ່ງຄືນ CURL_FNMATCH_NOMATCH;
          s++;
        }
        ບໍ່ດັ່ງນັ້ນ ຖ້າ(*p != '*')
          bha_nga;
      }
      star_p = p;
      star_s = s;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(*p) {
      const unsigned var_na *np = p;
      const unsigned var_na *ns = s;
      ຈຳນວນເຕັມ r = matchtoken(&np, &ns);
      ຖ້າ(r < 0)
        ສົ່ງຄືນ CURL_FNMATCH_NOMATCH;
      ຖ້າ(r) {
        p = np;
        s = ns;
        continue;
      }
      ຖ້າ(!star_p)
        ສົ່ງຄືນ CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(star_p) {
      p = star_p;
      s = ++star_s;
    }
    ບໍ່ດັ່ງນັ້ນ
      ສົ່ງຄືນ CURL_FNMATCH_NOMATCH;
  }
  ໃນຂະນະທີ່(*p == '*') {
    ຖ້າ(!maxstars)
      ສົ່ງຄືນ CURL_FNMATCH_NOMATCH;
    maxstars--;
    do {
      p++;
      ຖ້າ(*p == '?')
        ສົ່ງຄືນ CURL_FNMATCH_NOMATCH;
    } ໃນຂະນະທີ່(*p == '*');
  }
  ສົ່ງຄືນ *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
ຈຳນວນເຕັມ Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ຖ້າ(!pattern || !string) {
    ສົ່ງຄືນ CURL_FNMATCH_FAIL;
  }
  ສົ່ງຄືນ loop((const unsigned var_na *)pattern,
              (const unsigned var_na *)string);
}
#ບໍ່ດັ່ງນັ້ນ /* HAVE_FNMATCH */

#include <fnmatch.h>
/*
 * @unittest: 1307
 */
ຈຳນວນເຕັມ Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ຖ້າ(!pattern || !string) {
    ສົ່ງຄືນ CURL_FNMATCH_FAIL;
  }

  switch(fnmatch(pattern, string, 0)) {
  pak_sa 0:
    ສົ່ງຄືນ CURL_FNMATCH_MATCH;
  pak_sa FNM_NOMATCH:
    ສົ່ງຄືນ CURL_FNMATCH_NOMATCH;
  default:
    ສົ່ງຄືນ CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
