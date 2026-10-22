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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifndef CURL_DISABLE_FTP

#ಅನ್ತರ್ಭಾವ "curl_fnmatch.h"

#ifndef HAVE_FNMATCH

#define CURLFNM_CHARSET_LEN (ಪರಿಮಾಣ(ವರ್ಣ) * 256)
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

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  CURLFNM_SCHS_DEFAULT = 0,
  CURLFNM_SCHS_RIGHTBR,
  CURLFNM_SCHS_RIGHTBRLEFTBR
} setcharset_state;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  CURLFNM_PKW_INIT = 0,
  CURLFNM_PKW_DDOT
} parsekey_state;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  CCLASS_OTHER = 0,
  CCLASS_DIGIT,
  CCLASS_UPPER,
  CCLASS_LOWER
} char_class;

#define SETCHARSET_OK     1
#define SETCHARSET_FAIL   0

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parsekeyword(ಧ್ರುವ unsigned ವರ್ಣ **pattern, unsigned ವರ್ಣ *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  ವರ್ಣ keyword[10] = { 0 };
  size_t i;
  ಧ್ರುವ unsigned ವರ್ಣ *p = *pattern;
  bool found = FALSE;
  ಪ್ರತಿ(i = 0; !found; i++) {
    ವರ್ಣ c = (ವರ್ಣ)*p++;
    ಯದಿ(i >= ಪರಿಮಾಣ(keyword))
      ಪ್ರತ್ಯಾಗಮ SETCHARSET_FAIL;
    ವಿಕಲ್ಪ(state) {
    ಪಕ್ಷ CURLFNM_PKW_INIT:
      ಯದಿ(ISLOWER(c))
        keyword[i] = c;
      ಅನ್ಯಥಾ ಯದಿ(c == ':')
        state = CURLFNM_PKW_DDOT;
      ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ SETCHARSET_FAIL;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLFNM_PKW_DDOT:
      ಯದಿ(c == ']')
        found = TRUE;
      ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  ಯದಿ(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  ಅನ್ಯಥಾ ಯದಿ(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  ಅನ್ಯಥಾ ಯದಿ(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  ಅನ್ಯಥಾ ಯದಿ(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  ಅನ್ಯಥಾ ಯದಿ(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  ಅನ್ಯಥಾ ಯದಿ(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  ಅನ್ಯಥಾ ಯದಿ(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  ಅನ್ಯಥಾ ಯದಿ(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  ಅನ್ಯಥಾ ಯದಿ(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  ಅನ್ಯಥಾ ಯದಿ(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ SETCHARSET_FAIL;
  ಪ್ರತ್ಯಾಗಮ SETCHARSET_OK;
}

/* Return the character class. */
ಸ್ಥಿರ char_class charclass(unsigned ವರ್ಣ c)
{
  ಯದಿ(ISUPPER(c))
    ಪ್ರತ್ಯಾಗಮ CCLASS_UPPER;
  ಯದಿ(ISLOWER(c))
    ಪ್ರತ್ಯಾಗಮ CCLASS_LOWER;
  ಯದಿ(ISDIGIT(c))
    ಪ್ರತ್ಯಾಗಮ CCLASS_DIGIT;
  ಪ್ರತ್ಯಾಗಮ CCLASS_OTHER;
}

/* Include a character or a range in set. */
ಸ್ಥಿರ ಶೂನ್ಯ setcharorrange(ಧ್ರುವ unsigned ವರ್ಣ **pp, unsigned ವರ್ಣ *charset)
{
  ಧ್ರುವ unsigned ವರ್ಣ *p = (*pp)++;
  unsigned ವರ್ಣ c = *p++;

  charset[c] = 1;
  ಯದಿ(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned ವರ್ಣ endrange = *p++;

    ಯದಿ(endrange == '\\')
      endrange = *p++;
    ಯದಿ(endrange >= c && charclass(endrange) == cc) {
      ಯಾವತ್(c++ != endrange)
        ಯದಿ(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ setcharset(ಧ್ರುವ unsigned ವರ್ಣ **p, unsigned ವರ್ಣ *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  unsigned ವರ್ಣ c;

  memset(charset, 0, CURLFNM_CHSET_SIZE);
  ಪ್ರತಿ(;;) {
    c = **p;
    ಯದಿ(!c)
      ಪ್ರತ್ಯಾಗಮ SETCHARSET_FAIL;

    ವಿಕಲ್ಪ(state) {
    ಪಕ್ಷ CURLFNM_SCHS_DEFAULT:
      ಯದಿ(c == ']') {
        ಯದಿ(something_found)
          ಪ್ರತ್ಯಾಗಮ SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      ಅನ್ಯಥಾ ಯದಿ(c == '[') {
        ಧ್ರುವ unsigned ವರ್ಣ *pp = *p + 1;

        ಯದಿ(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        ಅನ್ಯಥಾ {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      ಅನ್ಯಥಾ ಯದಿ(c == '^' || c == '!') {
        ಯದಿ(!something_found) {
          ಯದಿ(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          ಅನ್ಯಥಾ
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        ಅನ್ಯಥಾ
          charset[c] = 1;
        (*p)++;
      }
      ಅನ್ಯಥಾ ಯದಿ(c == '\\') {
        c = *(++(*p));
        ಯದಿ(c)
          setcharorrange(p, charset);
        ಅನ್ಯಥಾ
          charset['\\'] = 1;
        something_found = TRUE;
      }
      ಅನ್ಯಥಾ {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLFNM_SCHS_RIGHTBR:
      ಯದಿ(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      ಅನ್ಯಥಾ ಯದಿ(c == ']') {
        ಪ್ರತ್ಯಾಗಮ SETCHARSET_OK;
      }
      ಅನ್ಯಥಾ ಯದಿ(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      ಅನ್ಯಥಾ
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        ಗಚ್ಛ fail;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLFNM_SCHS_RIGHTBRLEFTBR:
      ಯದಿ(c == ']')
        ಪ್ರತ್ಯಾಗಮ SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      ಭಙ್ಗ;
    }
  }
fail:
  ಪ್ರತ್ಯಾಗಮ SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ matchtoken(ಧ್ರುವ unsigned ವರ್ಣ **pp, ಧ್ರುವ unsigned ವರ್ಣ **sp)
{
  ಧ್ರುವ unsigned ವರ್ಣ *p = *pp;
  ಧ್ರುವ unsigned ವರ್ಣ *s = *sp;

  ವಿಕಲ್ಪ(*p) {
  ಪಕ್ಷ '?':
    ಭಙ್ಗ;
  ಪಕ್ಷ '\\':
    ಯದಿ(p[1])
      p++;
    ಯದಿ(*s != *p)
      ಪ್ರತ್ಯಾಗಮ 0;
    ಭಙ್ಗ;
  ಪಕ್ಷ '[': {
    unsigned ವರ್ಣ charset[CURLFNM_CHSET_SIZE];
    ಧ್ರುವ unsigned ವರ್ಣ *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    ಯದಿ(!setcharset(&pp2, charset))
      ಪ್ರತ್ಯಾಗಮ -1; /* Syntax error in set; mismatch! */
    ಯದಿ(charset[(unsigned ಪೂರ್ಣಾಙ್ಕ)*s])
      found = TRUE;
    ಅನ್ಯಥಾ ಯದಿ(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    ಅನ್ಯಥಾ ಯದಿ(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    ಅನ್ಯಥಾ ಯದಿ(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    ಅನ್ಯಥಾ ಯದಿ(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    ಅನ್ಯಥಾ ಯದಿ(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    ಅನ್ಯಥಾ ಯದಿ(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    ಅನ್ಯಥಾ ಯದಿ(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    ಅನ್ಯಥಾ ಯದಿ(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    ಅನ್ಯಥಾ ಯದಿ(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    ಅನ್ಯಥಾ ಯದಿ(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    ಯದಿ(charset[CURLFNM_NEGATE])
      found = !found;

    ಯದಿ(!found)
      ಪ್ರತ್ಯಾಗಮ 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    ಪ್ರತ್ಯಾಗಮ 1;
  }
  ಸ್ವಭಾವ:
    ಯದಿ(*p != *s)
      ಪ್ರತ್ಯಾಗಮ 0;
    ಭಙ್ಗ;
  }
  *pp = p + 1;
  *sp = s + 1;
  ಪ್ರತ್ಯಾಗಮ 1;
}

/* greedy match with backtracking to the most recent '*' */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ loop(ಧ್ರುವ unsigned ವರ್ಣ *pattern, ಧ್ರುವ unsigned ವರ್ಣ *string)
{
  ಧ್ರುವ unsigned ವರ್ಣ *p = pattern;
  ಧ್ರುವ unsigned ವರ್ಣ *s = string;
  ಧ್ರುವ unsigned ವರ್ಣ *star_p = ರಿಕ್ತ;
  ಧ್ರುವ unsigned ವರ್ಣ *star_s = ರಿಕ್ತ;
  ಪೂರ್ಣಾಙ್ಕ maxstars = 2;

  ಯಾವತ್(*s) {
    ಯದಿ(*p == '*') {
      ಯದಿ(!maxstars)
        ಪ್ರತ್ಯಾಗಮ CURL_FNMATCH_NOMATCH;
      maxstars--;
      ಪ್ರತಿ(;;) {
        p++;
        ಯದಿ(!*p)
          ಪ್ರತ್ಯಾಗಮ CURL_FNMATCH_MATCH;
        ಯದಿ(*p == '?') {
          ಯದಿ(!*s)
            ಪ್ರತ್ಯಾಗಮ CURL_FNMATCH_NOMATCH;
          s++;
        }
        ಅನ್ಯಥಾ ಯದಿ(*p != '*')
          ಭಙ್ಗ;
      }
      star_p = p;
      star_s = s;
    }
    ಅನ್ಯಥಾ ಯದಿ(*p) {
      ಧ್ರುವ unsigned ವರ್ಣ *np = p;
      ಧ್ರುವ unsigned ವರ್ಣ *ns = s;
      ಪೂರ್ಣಾಙ್ಕ r = matchtoken(&np, &ns);
      ಯದಿ(r < 0)
        ಪ್ರತ್ಯಾಗಮ CURL_FNMATCH_NOMATCH;
      ಯದಿ(r) {
        p = np;
        s = ns;
        ಅನುವರ್ತ;
      }
      ಯದಿ(!star_p)
        ಪ್ರತ್ಯಾಗಮ CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    ಅನ್ಯಥಾ ಯದಿ(star_p) {
      p = star_p;
      s = ++star_s;
    }
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ CURL_FNMATCH_NOMATCH;
  }
  ಯಾವತ್(*p == '*') {
    ಯದಿ(!maxstars)
      ಪ್ರತ್ಯಾಗಮ CURL_FNMATCH_NOMATCH;
    maxstars--;
    ಕುರು {
      p++;
      ಯದಿ(*p == '?')
        ಪ್ರತ್ಯಾಗಮ CURL_FNMATCH_NOMATCH;
    } ಯಾವತ್(*p == '*');
  }
  ಪ್ರತ್ಯಾಗಮ *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
ಪೂರ್ಣಾಙ್ಕ Curl_fnmatch(ಶೂನ್ಯ *ptr, ಧ್ರುವ ವರ್ಣ *pattern, ಧ್ರುವ ವರ್ಣ *string)
{
  (ಶೂನ್ಯ)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ಯದಿ(!pattern || !string) {
    ಪ್ರತ್ಯಾಗಮ CURL_FNMATCH_FAIL;
  }
  ಪ್ರತ್ಯಾಗಮ loop((ಧ್ರುವ unsigned ವರ್ಣ *)pattern,
              (ಧ್ರುವ unsigned ವರ್ಣ *)string);
}
#ಅನ್ಯಥಾ /* HAVE_FNMATCH */

#ಅನ್ತರ್ಭಾವ <fnmatch.h>
/*
 * @unittest: 1307
 */
ಪೂರ್ಣಾಙ್ಕ Curl_fnmatch(ಶೂನ್ಯ *ptr, ಧ್ರುವ ವರ್ಣ *pattern, ಧ್ರುವ ವರ್ಣ *string)
{
  (ಶೂನ್ಯ)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ಯದಿ(!pattern || !string) {
    ಪ್ರತ್ಯಾಗಮ CURL_FNMATCH_FAIL;
  }

  ವಿಕಲ್ಪ(fnmatch(pattern, string, 0)) {
  ಪಕ್ಷ 0:
    ಪ್ರತ್ಯಾಗಮ CURL_FNMATCH_MATCH;
  ಪಕ್ಷ FNM_NOMATCH:
    ಪ್ರತ್ಯಾಗಮ CURL_FNMATCH_NOMATCH;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
