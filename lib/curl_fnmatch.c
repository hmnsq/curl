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

static ціле parsekeyword(const unsigned char **pattern, unsigned char *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  char keyword[10] = { 0 };
  size_t i;
  const unsigned char *p = *pattern;
  bool found = FALSE;
  для(i = 0; !found; i++) {
    char c = (char)*p++;
    якщо(i >= sizeof(keyword))
      повернути SETCHARSET_FAIL;
    switch(state) {
    case CURLFNM_PKW_INIT:
      якщо(ISLOWER(c))
        keyword[i] = c;
      інакше якщо(c == ':')
        state = CURLFNM_PKW_DDOT;
      інакше
        повернути SETCHARSET_FAIL;
      break;
    case CURLFNM_PKW_DDOT:
      якщо(c == ']')
        found = TRUE;
      інакше
        повернути SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  якщо(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  інакше якщо(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  інакше якщо(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  інакше якщо(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  інакше якщо(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  інакше якщо(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  інакше якщо(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  інакше якщо(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  інакше якщо(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  інакше якщо(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  інакше
    повернути SETCHARSET_FAIL;
  повернути SETCHARSET_OK;
}

/* Return the character class. */
static char_class charclass(unsigned char c)
{
  якщо(ISUPPER(c))
    повернути CCLASS_UPPER;
  якщо(ISLOWER(c))
    повернути CCLASS_LOWER;
  якщо(ISDIGIT(c))
    повернути CCLASS_DIGIT;
  повернути CCLASS_OTHER;
}

/* Include a character or a range in set. */
static void setcharorrange(const unsigned char **pp, unsigned char *charset)
{
  const unsigned char *p = (*pp)++;
  unsigned char c = *p++;

  charset[c] = 1;
  якщо(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned char endrange = *p++;

    якщо(endrange == '\\')
      endrange = *p++;
    якщо(endrange >= c && charclass(endrange) == cc) {
      поки(c++ != endrange)
        якщо(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
static ціле setcharset(const unsigned char **p, unsigned char *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  unsigned char c;

  memset(charset, 0, CURLFNM_CHSET_SIZE);
  для(;;) {
    c = **p;
    якщо(!c)
      повернути SETCHARSET_FAIL;

    switch(state) {
    case CURLFNM_SCHS_DEFAULT:
      якщо(c == ']') {
        якщо(something_found)
          повернути SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      інакше якщо(c == '[') {
        const unsigned char *pp = *p + 1;

        якщо(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        інакше {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      інакше якщо(c == '^' || c == '!') {
        якщо(!something_found) {
          якщо(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          інакше
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        інакше
          charset[c] = 1;
        (*p)++;
      }
      інакше якщо(c == '\\') {
        c = *(++(*p));
        якщо(c)
          setcharorrange(p, charset);
        інакше
          charset['\\'] = 1;
        something_found = TRUE;
      }
      інакше {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      break;
    case CURLFNM_SCHS_RIGHTBR:
      якщо(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      інакше якщо(c == ']') {
        повернути SETCHARSET_OK;
      }
      інакше якщо(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      інакше
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        goto fail;
      break;
    case CURLFNM_SCHS_RIGHTBRLEFTBR:
      якщо(c == ']')
        повернути SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      break;
    }
  }
fail:
  повернути SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
static ціле matchtoken(const unsigned char **pp, const unsigned char **sp)
{
  const unsigned char *p = *pp;
  const unsigned char *s = *sp;

  switch(*p) {
  case '?':
    break;
  case '\\':
    якщо(p[1])
      p++;
    якщо(*s != *p)
      повернути 0;
    break;
  case '[': {
    unsigned char charset[CURLFNM_CHSET_SIZE];
    const unsigned char *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    якщо(!setcharset(&pp2, charset))
      повернути -1; /* Syntax error in set; mismatch! */
    якщо(charset[(unsigned ціле)*s])
      found = TRUE;
    інакше якщо(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    інакше якщо(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    інакше якщо(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    інакше якщо(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    інакше якщо(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    інакше якщо(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    інакше якщо(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    інакше якщо(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    інакше якщо(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    інакше якщо(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    якщо(charset[CURLFNM_NEGATE])
      found = !found;

    якщо(!found)
      повернути 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    повернути 1;
  }
  default:
    якщо(*p != *s)
      повернути 0;
    break;
  }
  *pp = p + 1;
  *sp = s + 1;
  повернути 1;
}

/* greedy match with backtracking to the most recent '*' */
static ціле loop(const unsigned char *pattern, const unsigned char *string)
{
  const unsigned char *p = pattern;
  const unsigned char *s = string;
  const unsigned char *star_p = NULL;
  const unsigned char *star_s = NULL;
  ціле maxstars = 2;

  поки(*s) {
    якщо(*p == '*') {
      якщо(!maxstars)
        повернути CURL_FNMATCH_NOMATCH;
      maxstars--;
      для(;;) {
        p++;
        якщо(!*p)
          повернути CURL_FNMATCH_MATCH;
        якщо(*p == '?') {
          якщо(!*s)
            повернути CURL_FNMATCH_NOMATCH;
          s++;
        }
        інакше якщо(*p != '*')
          break;
      }
      star_p = p;
      star_s = s;
    }
    інакше якщо(*p) {
      const unsigned char *np = p;
      const unsigned char *ns = s;
      ціле r = matchtoken(&np, &ns);
      якщо(r < 0)
        повернути CURL_FNMATCH_NOMATCH;
      якщо(r) {
        p = np;
        s = ns;
        continue;
      }
      якщо(!star_p)
        повернути CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    інакше якщо(star_p) {
      p = star_p;
      s = ++star_s;
    }
    інакше
      повернути CURL_FNMATCH_NOMATCH;
  }
  поки(*p == '*') {
    якщо(!maxstars)
      повернути CURL_FNMATCH_NOMATCH;
    maxstars--;
    do {
      p++;
      якщо(*p == '?')
        повернути CURL_FNMATCH_NOMATCH;
    } поки(*p == '*');
  }
  повернути *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
ціле Curl_fnmatch(void *ptr, const char *pattern, const char *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  якщо(!pattern || !string) {
    повернути CURL_FNMATCH_FAIL;
  }
  повернути loop((const unsigned char *)pattern,
              (const unsigned char *)string);
}
#інакше /* HAVE_FNMATCH */

#include <fnmatch.h>
/*
 * @unittest: 1307
 */
ціле Curl_fnmatch(void *ptr, const char *pattern, const char *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  якщо(!pattern || !string) {
    повернути CURL_FNMATCH_FAIL;
  }

  switch(fnmatch(pattern, string, 0)) {
  case 0:
    повернути CURL_FNMATCH_MATCH;
  case FNM_NOMATCH:
    повернути CURL_FNMATCH_NOMATCH;
  default:
    повернути CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
