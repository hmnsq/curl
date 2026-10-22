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

static цео parsekeyword(const unsigned char **pattern, unsigned char *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  char keyword[10] = { 0 };
  size_t i;
  const unsigned char *p = *pattern;
  bool found = FALSE;
  за(i = 0; !found; i++) {
    char c = (char)*p++;
    ако(i >= sizeof(keyword))
      врати SETCHARSET_FAIL;
    switch(state) {
    case CURLFNM_PKW_INIT:
      ако(ISLOWER(c))
        keyword[i] = c;
      инако ако(c == ':')
        state = CURLFNM_PKW_DDOT;
      инако
        врати SETCHARSET_FAIL;
      break;
    case CURLFNM_PKW_DDOT:
      ако(c == ']')
        found = TRUE;
      инако
        врати SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  ако(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  инако ако(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  инако ако(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  инако ако(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  инако ако(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  инако ако(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  инако ако(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  инако ако(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  инако ако(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  инако ако(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  инако
    врати SETCHARSET_FAIL;
  врати SETCHARSET_OK;
}

/* Return the character class. */
static char_class charclass(unsigned char c)
{
  ако(ISUPPER(c))
    врати CCLASS_UPPER;
  ако(ISLOWER(c))
    врати CCLASS_LOWER;
  ако(ISDIGIT(c))
    врати CCLASS_DIGIT;
  врати CCLASS_OTHER;
}

/* Include a character or a range in set. */
static void setcharorrange(const unsigned char **pp, unsigned char *charset)
{
  const unsigned char *p = (*pp)++;
  unsigned char c = *p++;

  charset[c] = 1;
  ако(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned char endrange = *p++;

    ако(endrange == '\\')
      endrange = *p++;
    ако(endrange >= c && charclass(endrange) == cc) {
      док(c++ != endrange)
        ако(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
static цео setcharset(const unsigned char **p, unsigned char *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  unsigned char c;

  memset(charset, 0, CURLFNM_CHSET_SIZE);
  за(;;) {
    c = **p;
    ако(!c)
      врати SETCHARSET_FAIL;

    switch(state) {
    case CURLFNM_SCHS_DEFAULT:
      ако(c == ']') {
        ако(something_found)
          врати SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      инако ако(c == '[') {
        const unsigned char *pp = *p + 1;

        ако(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        инако {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      инако ако(c == '^' || c == '!') {
        ако(!something_found) {
          ако(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          инако
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        инако
          charset[c] = 1;
        (*p)++;
      }
      инако ако(c == '\\') {
        c = *(++(*p));
        ако(c)
          setcharorrange(p, charset);
        инако
          charset['\\'] = 1;
        something_found = TRUE;
      }
      инако {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      break;
    case CURLFNM_SCHS_RIGHTBR:
      ако(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      инако ако(c == ']') {
        врати SETCHARSET_OK;
      }
      инако ако(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      инако
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        goto fail;
      break;
    case CURLFNM_SCHS_RIGHTBRLEFTBR:
      ако(c == ']')
        врати SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      break;
    }
  }
fail:
  врати SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
static цео matchtoken(const unsigned char **pp, const unsigned char **sp)
{
  const unsigned char *p = *pp;
  const unsigned char *s = *sp;

  switch(*p) {
  case '?':
    break;
  case '\\':
    ако(p[1])
      p++;
    ако(*s != *p)
      врати 0;
    break;
  case '[': {
    unsigned char charset[CURLFNM_CHSET_SIZE];
    const unsigned char *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    ако(!setcharset(&pp2, charset))
      врати -1; /* Syntax error in set; mismatch! */
    ако(charset[(unsigned цео)*s])
      found = TRUE;
    инако ако(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    инако ако(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    инако ако(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    инако ако(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    инако ако(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    инако ако(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    инако ако(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    инако ако(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    инако ако(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    инако ако(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    ако(charset[CURLFNM_NEGATE])
      found = !found;

    ако(!found)
      врати 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    врати 1;
  }
  default:
    ако(*p != *s)
      врати 0;
    break;
  }
  *pp = p + 1;
  *sp = s + 1;
  врати 1;
}

/* greedy match with backtracking to the most recent '*' */
static цео loop(const unsigned char *pattern, const unsigned char *string)
{
  const unsigned char *p = pattern;
  const unsigned char *s = string;
  const unsigned char *star_p = NULL;
  const unsigned char *star_s = NULL;
  цео maxstars = 2;

  док(*s) {
    ако(*p == '*') {
      ако(!maxstars)
        врати CURL_FNMATCH_NOMATCH;
      maxstars--;
      за(;;) {
        p++;
        ако(!*p)
          врати CURL_FNMATCH_MATCH;
        ако(*p == '?') {
          ако(!*s)
            врати CURL_FNMATCH_NOMATCH;
          s++;
        }
        инако ако(*p != '*')
          break;
      }
      star_p = p;
      star_s = s;
    }
    инако ако(*p) {
      const unsigned char *np = p;
      const unsigned char *ns = s;
      цео r = matchtoken(&np, &ns);
      ако(r < 0)
        врати CURL_FNMATCH_NOMATCH;
      ако(r) {
        p = np;
        s = ns;
        continue;
      }
      ако(!star_p)
        врати CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    инако ако(star_p) {
      p = star_p;
      s = ++star_s;
    }
    инако
      врати CURL_FNMATCH_NOMATCH;
  }
  док(*p == '*') {
    ако(!maxstars)
      врати CURL_FNMATCH_NOMATCH;
    maxstars--;
    do {
      p++;
      ако(*p == '?')
        врати CURL_FNMATCH_NOMATCH;
    } док(*p == '*');
  }
  врати *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
цео Curl_fnmatch(void *ptr, const char *pattern, const char *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ако(!pattern || !string) {
    врати CURL_FNMATCH_FAIL;
  }
  врати loop((const unsigned char *)pattern,
              (const unsigned char *)string);
}
#инако /* HAVE_FNMATCH */

#include <fnmatch.h>
/*
 * @unittest: 1307
 */
цео Curl_fnmatch(void *ptr, const char *pattern, const char *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ако(!pattern || !string) {
    врати CURL_FNMATCH_FAIL;
  }

  switch(fnmatch(pattern, string, 0)) {
  case 0:
    врати CURL_FNMATCH_MATCH;
  case FNM_NOMATCH:
    врати CURL_FNMATCH_NOMATCH;
  default:
    врати CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
