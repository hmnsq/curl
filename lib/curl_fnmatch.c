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

#define CURLFNM_CHARSET_LEN (sizeof(char) * 256)
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

typedef enum {
  CURLFNM_SCHS_DEFAULT = 0,
  CURLFNM_SCHS_RIGHTBR,
  CURLFNM_SCHS_RIGHTBRLEFTBR
} setcharset_state;

typedef enum {
  CURLFNM_PKW_INIT = 0,
  CURLFNM_PKW_DDOT
} parsekey_state;

typedef enum {
  CCLASS_OTHER = 0,
  CCLASS_DIGIT,
  CCLASS_UPPER,
  CCLASS_LOWER
} char_class;

#define SETCHARSET_OK     1
#define SETCHARSET_FAIL   0

static số_nguyên parsekeyword(const unsigned char **pattern, unsigned char *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  char keyword[10] = { 0 };
  size_t i;
  const unsigned char *p = *pattern;
  bool found = FALSE;
  cho(i = 0; !found; i++) {
    char c = (char)*p++;
    nếu(i >= sizeof(keyword))
      trả_về SETCHARSET_FAIL;
    switch(state) {
    case CURLFNM_PKW_INIT:
      nếu(ISLOWER(c))
        keyword[i] = c;
      không_thì nếu(c == ':')
        state = CURLFNM_PKW_DDOT;
      không_thì
        trả_về SETCHARSET_FAIL;
      break;
    case CURLFNM_PKW_DDOT:
      nếu(c == ']')
        found = TRUE;
      không_thì
        trả_về SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  nếu(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  không_thì nếu(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  không_thì nếu(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  không_thì nếu(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  không_thì nếu(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  không_thì nếu(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  không_thì nếu(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  không_thì nếu(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  không_thì nếu(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  không_thì nếu(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  không_thì
    trả_về SETCHARSET_FAIL;
  trả_về SETCHARSET_OK;
}

/* Return the character class. */
static char_class charclass(unsigned char c)
{
  nếu(ISUPPER(c))
    trả_về CCLASS_UPPER;
  nếu(ISLOWER(c))
    trả_về CCLASS_LOWER;
  nếu(ISDIGIT(c))
    trả_về CCLASS_DIGIT;
  trả_về CCLASS_OTHER;
}

/* Include a character or a range in set. */
static void setcharorrange(const unsigned char **pp, unsigned char *charset)
{
  const unsigned char *p = (*pp)++;
  unsigned char c = *p++;

  charset[c] = 1;
  nếu(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned char endrange = *p++;

    nếu(endrange == '\\')
      endrange = *p++;
    nếu(endrange >= c && charclass(endrange) == cc) {
      trong_khi(c++ != endrange)
        nếu(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
static số_nguyên setcharset(const unsigned char **p, unsigned char *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  unsigned char c;

  memset(charset, 0, CURLFNM_CHSET_SIZE);
  cho(;;) {
    c = **p;
    nếu(!c)
      trả_về SETCHARSET_FAIL;

    switch(state) {
    case CURLFNM_SCHS_DEFAULT:
      nếu(c == ']') {
        nếu(something_found)
          trả_về SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      không_thì nếu(c == '[') {
        const unsigned char *pp = *p + 1;

        nếu(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        không_thì {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      không_thì nếu(c == '^' || c == '!') {
        nếu(!something_found) {
          nếu(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          không_thì
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        không_thì
          charset[c] = 1;
        (*p)++;
      }
      không_thì nếu(c == '\\') {
        c = *(++(*p));
        nếu(c)
          setcharorrange(p, charset);
        không_thì
          charset['\\'] = 1;
        something_found = TRUE;
      }
      không_thì {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      break;
    case CURLFNM_SCHS_RIGHTBR:
      nếu(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      không_thì nếu(c == ']') {
        trả_về SETCHARSET_OK;
      }
      không_thì nếu(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      không_thì
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        goto fail;
      break;
    case CURLFNM_SCHS_RIGHTBRLEFTBR:
      nếu(c == ']')
        trả_về SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      break;
    }
  }
fail:
  trả_về SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
static số_nguyên matchtoken(const unsigned char **pp, const unsigned char **sp)
{
  const unsigned char *p = *pp;
  const unsigned char *s = *sp;

  switch(*p) {
  case '?':
    break;
  case '\\':
    nếu(p[1])
      p++;
    nếu(*s != *p)
      trả_về 0;
    break;
  case '[': {
    unsigned char charset[CURLFNM_CHSET_SIZE];
    const unsigned char *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    nếu(!setcharset(&pp2, charset))
      trả_về -1; /* Syntax error in set; mismatch! */
    nếu(charset[(unsigned số_nguyên)*s])
      found = TRUE;
    không_thì nếu(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    không_thì nếu(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    không_thì nếu(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    không_thì nếu(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    không_thì nếu(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    không_thì nếu(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    không_thì nếu(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    không_thì nếu(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    không_thì nếu(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    không_thì nếu(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    nếu(charset[CURLFNM_NEGATE])
      found = !found;

    nếu(!found)
      trả_về 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    trả_về 1;
  }
  default:
    nếu(*p != *s)
      trả_về 0;
    break;
  }
  *pp = p + 1;
  *sp = s + 1;
  trả_về 1;
}

/* greedy match with backtracking to the most recent '*' */
static số_nguyên loop(const unsigned char *pattern, const unsigned char *string)
{
  const unsigned char *p = pattern;
  const unsigned char *s = string;
  const unsigned char *star_p = NULL;
  const unsigned char *star_s = NULL;
  số_nguyên maxstars = 2;

  trong_khi(*s) {
    nếu(*p == '*') {
      nếu(!maxstars)
        trả_về CURL_FNMATCH_NOMATCH;
      maxstars--;
      cho(;;) {
        p++;
        nếu(!*p)
          trả_về CURL_FNMATCH_MATCH;
        nếu(*p == '?') {
          nếu(!*s)
            trả_về CURL_FNMATCH_NOMATCH;
          s++;
        }
        không_thì nếu(*p != '*')
          break;
      }
      star_p = p;
      star_s = s;
    }
    không_thì nếu(*p) {
      const unsigned char *np = p;
      const unsigned char *ns = s;
      số_nguyên r = matchtoken(&np, &ns);
      nếu(r < 0)
        trả_về CURL_FNMATCH_NOMATCH;
      nếu(r) {
        p = np;
        s = ns;
        continue;
      }
      nếu(!star_p)
        trả_về CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    không_thì nếu(star_p) {
      p = star_p;
      s = ++star_s;
    }
    không_thì
      trả_về CURL_FNMATCH_NOMATCH;
  }
  trong_khi(*p == '*') {
    nếu(!maxstars)
      trả_về CURL_FNMATCH_NOMATCH;
    maxstars--;
    do {
      p++;
      nếu(*p == '?')
        trả_về CURL_FNMATCH_NOMATCH;
    } trong_khi(*p == '*');
  }
  trả_về *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
số_nguyên Curl_fnmatch(void *ptr, const char *pattern, const char *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  nếu(!pattern || !string) {
    trả_về CURL_FNMATCH_FAIL;
  }
  trả_về loop((const unsigned char *)pattern,
              (const unsigned char *)string);
}
#không_thì /* HAVE_FNMATCH */

#include <fnmatch.h>
/*
 * @unittest: 1307
 */
số_nguyên Curl_fnmatch(void *ptr, const char *pattern, const char *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  nếu(!pattern || !string) {
    trả_về CURL_FNMATCH_FAIL;
  }

  switch(fnmatch(pattern, string, 0)) {
  case 0:
    trả_về CURL_FNMATCH_MATCH;
  case FNM_NOMATCH:
    trả_về CURL_FNMATCH_NOMATCH;
  default:
    trả_về CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
