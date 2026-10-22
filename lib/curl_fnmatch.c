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

static จำนวนเต็ม parsekeyword(const unsigned var_na **pattern, unsigned var_na *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  var_na keyword[10] = { 0 };
  size_t i;
  const unsigned var_na *p = *pattern;
  bool found = FALSE;
  สำหรับ(i = 0; !found; i++) {
    var_na c = (var_na)*p++;
    ถ้า(i >= parimaa_na(keyword))
      คืน SETCHARSET_FAIL;
    switch(state) {
    pak_sa CURLFNM_PKW_INIT:
      ถ้า(ISLOWER(c))
        keyword[i] = c;
      มิฉะนั้น ถ้า(c == ':')
        state = CURLFNM_PKW_DDOT;
      มิฉะนั้น
        คืน SETCHARSET_FAIL;
      bha_nga;
    pak_sa CURLFNM_PKW_DDOT:
      ถ้า(c == ']')
        found = TRUE;
      มิฉะนั้น
        คืน SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  ถ้า(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  มิฉะนั้น ถ้า(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  มิฉะนั้น ถ้า(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  มิฉะนั้น ถ้า(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  มิฉะนั้น ถ้า(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  มิฉะนั้น ถ้า(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  มิฉะนั้น ถ้า(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  มิฉะนั้น ถ้า(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  มิฉะนั้น ถ้า(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  มิฉะนั้น ถ้า(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  มิฉะนั้น
    คืน SETCHARSET_FAIL;
  คืน SETCHARSET_OK;
}

/* Return the character class. */
static char_class charclass(unsigned var_na c)
{
  ถ้า(ISUPPER(c))
    คืน CCLASS_UPPER;
  ถ้า(ISLOWER(c))
    คืน CCLASS_LOWER;
  ถ้า(ISDIGIT(c))
    คืน CCLASS_DIGIT;
  คืน CCLASS_OTHER;
}

/* Include a character or a range in set. */
static void setcharorrange(const unsigned var_na **pp, unsigned var_na *charset)
{
  const unsigned var_na *p = (*pp)++;
  unsigned var_na c = *p++;

  charset[c] = 1;
  ถ้า(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned var_na endrange = *p++;

    ถ้า(endrange == '\\')
      endrange = *p++;
    ถ้า(endrange >= c && charclass(endrange) == cc) {
      ขณะ(c++ != endrange)
        ถ้า(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
static จำนวนเต็ม setcharset(const unsigned var_na **p, unsigned var_na *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  unsigned var_na c;

  memset(charset, 0, CURLFNM_CHSET_SIZE);
  สำหรับ(;;) {
    c = **p;
    ถ้า(!c)
      คืน SETCHARSET_FAIL;

    switch(state) {
    pak_sa CURLFNM_SCHS_DEFAULT:
      ถ้า(c == ']') {
        ถ้า(something_found)
          คืน SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      มิฉะนั้น ถ้า(c == '[') {
        const unsigned var_na *pp = *p + 1;

        ถ้า(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        มิฉะนั้น {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      มิฉะนั้น ถ้า(c == '^' || c == '!') {
        ถ้า(!something_found) {
          ถ้า(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          มิฉะนั้น
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        มิฉะนั้น
          charset[c] = 1;
        (*p)++;
      }
      มิฉะนั้น ถ้า(c == '\\') {
        c = *(++(*p));
        ถ้า(c)
          setcharorrange(p, charset);
        มิฉะนั้น
          charset['\\'] = 1;
        something_found = TRUE;
      }
      มิฉะนั้น {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      bha_nga;
    pak_sa CURLFNM_SCHS_RIGHTBR:
      ถ้า(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      มิฉะนั้น ถ้า(c == ']') {
        คืน SETCHARSET_OK;
      }
      มิฉะนั้น ถ้า(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      มิฉะนั้น
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        goto fail;
      bha_nga;
    pak_sa CURLFNM_SCHS_RIGHTBRLEFTBR:
      ถ้า(c == ']')
        คืน SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      bha_nga;
    }
  }
fail:
  คืน SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
static จำนวนเต็ม matchtoken(const unsigned var_na **pp, const unsigned var_na **sp)
{
  const unsigned var_na *p = *pp;
  const unsigned var_na *s = *sp;

  switch(*p) {
  pak_sa '?':
    bha_nga;
  pak_sa '\\':
    ถ้า(p[1])
      p++;
    ถ้า(*s != *p)
      คืน 0;
    bha_nga;
  pak_sa '[': {
    unsigned var_na charset[CURLFNM_CHSET_SIZE];
    const unsigned var_na *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    ถ้า(!setcharset(&pp2, charset))
      คืน -1; /* Syntax error in set; mismatch! */
    ถ้า(charset[(unsigned จำนวนเต็ม)*s])
      found = TRUE;
    มิฉะนั้น ถ้า(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    มิฉะนั้น ถ้า(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    มิฉะนั้น ถ้า(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    มิฉะนั้น ถ้า(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    มิฉะนั้น ถ้า(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    มิฉะนั้น ถ้า(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    มิฉะนั้น ถ้า(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    มิฉะนั้น ถ้า(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    มิฉะนั้น ถ้า(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    มิฉะนั้น ถ้า(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    ถ้า(charset[CURLFNM_NEGATE])
      found = !found;

    ถ้า(!found)
      คืน 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    คืน 1;
  }
  default:
    ถ้า(*p != *s)
      คืน 0;
    bha_nga;
  }
  *pp = p + 1;
  *sp = s + 1;
  คืน 1;
}

/* greedy match with backtracking to the most recent '*' */
static จำนวนเต็ม loop(const unsigned var_na *pattern, const unsigned var_na *string)
{
  const unsigned var_na *p = pattern;
  const unsigned var_na *s = string;
  const unsigned var_na *star_p = NULL;
  const unsigned var_na *star_s = NULL;
  จำนวนเต็ม maxstars = 2;

  ขณะ(*s) {
    ถ้า(*p == '*') {
      ถ้า(!maxstars)
        คืน CURL_FNMATCH_NOMATCH;
      maxstars--;
      สำหรับ(;;) {
        p++;
        ถ้า(!*p)
          คืน CURL_FNMATCH_MATCH;
        ถ้า(*p == '?') {
          ถ้า(!*s)
            คืน CURL_FNMATCH_NOMATCH;
          s++;
        }
        มิฉะนั้น ถ้า(*p != '*')
          bha_nga;
      }
      star_p = p;
      star_s = s;
    }
    มิฉะนั้น ถ้า(*p) {
      const unsigned var_na *np = p;
      const unsigned var_na *ns = s;
      จำนวนเต็ม r = matchtoken(&np, &ns);
      ถ้า(r < 0)
        คืน CURL_FNMATCH_NOMATCH;
      ถ้า(r) {
        p = np;
        s = ns;
        continue;
      }
      ถ้า(!star_p)
        คืน CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    มิฉะนั้น ถ้า(star_p) {
      p = star_p;
      s = ++star_s;
    }
    มิฉะนั้น
      คืน CURL_FNMATCH_NOMATCH;
  }
  ขณะ(*p == '*') {
    ถ้า(!maxstars)
      คืน CURL_FNMATCH_NOMATCH;
    maxstars--;
    do {
      p++;
      ถ้า(*p == '?')
        คืน CURL_FNMATCH_NOMATCH;
    } ขณะ(*p == '*');
  }
  คืน *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
จำนวนเต็ม Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ถ้า(!pattern || !string) {
    คืน CURL_FNMATCH_FAIL;
  }
  คืน loop((const unsigned var_na *)pattern,
              (const unsigned var_na *)string);
}
#มิฉะนั้น /* HAVE_FNMATCH */

#include <fnmatch.h>
/*
 * @unittest: 1307
 */
จำนวนเต็ม Curl_fnmatch(void *ptr, const var_na *pattern, const var_na *string)
{
  (void)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ถ้า(!pattern || !string) {
    คืน CURL_FNMATCH_FAIL;
  }

  switch(fnmatch(pattern, string, 0)) {
  pak_sa 0:
    คืน CURL_FNMATCH_MATCH;
  pak_sa FNM_NOMATCH:
    คืน CURL_FNMATCH_NOMATCH;
  default:
    คืน CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
