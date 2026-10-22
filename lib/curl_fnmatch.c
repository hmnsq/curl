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

static 정수 parsekeyword(const unsigned char **pattern, unsigned char *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  char keyword[10] = { 0 };
  size_t i;
  const unsigned char *p = *pattern;
  bool found = FALSE;
  위해(i = 0; !found; i++) {
    char c = (char)*p++;
    만약(i >= sizeof(keyword))
      반환 SETCHARSET_FAIL;
    switch(state) {
    case CURLFNM_PKW_INIT:
      만약(ISLOWER(c))
        keyword[i] = c;
      아니면 만약(c == ':')
        state = CURLFNM_PKW_DDOT;
      아니면
        반환 SETCHARSET_FAIL;
      break;
    case CURLFNM_PKW_DDOT:
      만약(c == ']')
        found = TRUE;
      아니면
        반환 SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  만약(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  아니면 만약(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  아니면 만약(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  아니면 만약(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  아니면 만약(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  아니면 만약(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  아니면 만약(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  아니면 만약(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  아니면 만약(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  아니면 만약(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  아니면
    반환 SETCHARSET_FAIL;
  반환 SETCHARSET_OK;
}

/* Return the character class. */
static char_class charclass(unsigned char c)
{
  만약(ISUPPER(c))
    반환 CCLASS_UPPER;
  만약(ISLOWER(c))
    반환 CCLASS_LOWER;
  만약(ISDIGIT(c))
    반환 CCLASS_DIGIT;
  반환 CCLASS_OTHER;
}

/* Include a character or a range in set. */
static void setcharorrange(const unsigned char **pp, unsigned char *charset)
{
  const unsigned char *p = (*pp)++;
  unsigned char c = *p++;

  charset[c] = 1;
  만약(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned char endrange = *p++;

    만약(endrange == '\\')
      endrange = *p++;
    만약(endrange >= c && charclass(endrange) == cc) {
      동안(c++ != endrange)
        만약(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
static 정수 setcharset(const unsigned char **p, unsigned char *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  unsigned char c;

  memset(charset, 0, CURLFNM_CHSET_SIZE);
  위해(;;) {
    c = **p;
    만약(!c)
      반환 SETCHARSET_FAIL;

    switch(state) {
    case CURLFNM_SCHS_DEFAULT:
      만약(c == ']') {
        만약(something_found)
          반환 SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      아니면 만약(c == '[') {
        const unsigned char *pp = *p + 1;

        만약(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        아니면 {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      아니면 만약(c == '^' || c == '!') {
        만약(!something_found) {
          만약(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          아니면
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        아니면
          charset[c] = 1;
        (*p)++;
      }
      아니면 만약(c == '\\') {
        c = *(++(*p));
        만약(c)
          setcharorrange(p, charset);
        아니면
          charset['\\'] = 1;
        something_found = TRUE;
      }
      아니면 {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      break;
    case CURLFNM_SCHS_RIGHTBR:
      만약(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      아니면 만약(c == ']') {
        반환 SETCHARSET_OK;
      }
      아니면 만약(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      아니면
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        goto fail;
      break;
    case CURLFNM_SCHS_RIGHTBRLEFTBR:
      만약(c == ']')
        반환 SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      break;
    }
  }
fail:
  반환 SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
static 정수 matchtoken(const unsigned char **pp, const unsigned char **sp)
{
  const unsigned char *p = *pp;
  const unsigned char *s = *sp;

  switch(*p) {
  case '?':
    break;
  case '\\':
    만약(p[1])
      p++;
    만약(*s != *p)
      반환 0;
    break;
  case '[': {
    unsigned char charset[CURLFNM_CHSET_SIZE];
    const unsigned char *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    만약(!setcharset(&pp2, charset))
      반환 -1; /* Syntax error in set; mismatch! */
    만약(charset[(unsigned 정수)*s])
      found = TRUE;
    아니면 만약(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    아니면 만약(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    아니면 만약(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    아니면 만약(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    아니면 만약(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    아니면 만약(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    아니면 만약(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    아니면 만약(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    아니면 만약(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    아니면 만약(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    만약(charset[CURLFNM_NEGATE])
      found = !found;

    만약(!found)
      반환 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    반환 1;
  }
  default:
    만약(*p != *s)
      반환 0;
    break;
  }
  *pp = p + 1;
  *sp = s + 1;
  반환 1;
}

/* greedy match with backtracking to the most recent '*' */
static 정수 loop(const unsigned char *pattern, const unsigned char *string)
{
  const unsigned char *p = pattern;
  const unsigned char *s = string;
  const unsigned char *star_p = NULL;
  const unsigned char *star_s = NULL;
  정수 maxstars = 2;

  동안(*s) {
    만약(*p == '*') {
      만약(!maxstars)
        반환 CURL_FNMATCH_NOMATCH;
      maxstars--;
      위해(;;) {
        p++;
        만약(!*p)
          반환 CURL_FNMATCH_MATCH;
        만약(*p == '?') {
          만약(!*s)
            반환 CURL_FNMATCH_NOMATCH;
          s++;
        }
        아니면 만약(*p != '*')
          break;
      }
      star_p = p;
      star_s = s;
    }
    아니면 만약(*p) {
      const unsigned char *np = p;
      const unsigned char *ns = s;
      정수 r = matchtoken(&np, &ns);
      만약(r < 0)
        반환 CURL_FNMATCH_NOMATCH;
      만약(r) {
        p = np;
        s = ns;
        continue;
      }
      만약(!star_p)
        반환 CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    아니면 만약(star_p) {
      p = star_p;
      s = ++star_s;
    }
    아니면
      반환 CURL_FNMATCH_NOMATCH;
  }
  동안(*p == '*') {
    만약(!maxstars)
      반환 CURL_FNMATCH_NOMATCH;
    maxstars--;
    do {
      p++;
      만약(*p == '?')
        반환 CURL_FNMATCH_NOMATCH;
    } 동안(*p == '*');
  }
  반환 *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
정수 Curl_fnmatch(void *ptr, const char *pattern, const char *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  만약(!pattern || !string) {
    반환 CURL_FNMATCH_FAIL;
  }
  반환 loop((const unsigned char *)pattern,
              (const unsigned char *)string);
}
#아니면 /* HAVE_FNMATCH */

#include <fnmatch.h>
/*
 * @unittest: 1307
 */
정수 Curl_fnmatch(void *ptr, const char *pattern, const char *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  만약(!pattern || !string) {
    반환 CURL_FNMATCH_FAIL;
  }

  switch(fnmatch(pattern, string, 0)) {
  case 0:
    반환 CURL_FNMATCH_MATCH;
  case FNM_NOMATCH:
    반환 CURL_FNMATCH_NOMATCH;
  default:
    반환 CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
