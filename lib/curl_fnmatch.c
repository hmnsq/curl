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
#അന്തര്ഭാവ "curl_setup.h"

#ifndef CURL_DISABLE_FTP

#അന്തര്ഭാവ "curl_fnmatch.h"

#ifndef HAVE_FNMATCH

#define CURLFNM_CHARSET_LEN (വലുപ്പം(അക്ഷരം) * 256)
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

തരനിർവചനം എണ്ണൽ {
  CURLFNM_SCHS_DEFAULT = 0,
  CURLFNM_SCHS_RIGHTBR,
  CURLFNM_SCHS_RIGHTBRLEFTBR
} setcharset_state;

തരനിർവചനം എണ്ണൽ {
  CURLFNM_PKW_INIT = 0,
  CURLFNM_PKW_DDOT
} parsekey_state;

തരനിർവചനം എണ്ണൽ {
  CCLASS_OTHER = 0,
  CCLASS_DIGIT,
  CCLASS_UPPER,
  CCLASS_LOWER
} char_class;

#define SETCHARSET_OK     1
#define SETCHARSET_FAIL   0

സ്ഥിതം പൂർണ്ണസംഖ്യ parsekeyword(സ്ഥിരം unsigned അക്ഷരം **pattern, unsigned അക്ഷരം *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  അക്ഷരം keyword[10] = { 0 };
  size_t i;
  സ്ഥിരം unsigned അക്ഷരം *p = *pattern;
  bool found = FALSE;
  വേണ്ടി(i = 0; !found; i++) {
    അക്ഷരം c = (അക്ഷരം)*p++;
    എങ്കിൽ(i >= വലുപ്പം(keyword))
      തിരികെ SETCHARSET_FAIL;
    തിരഞ്ഞെടുപ്പ്(state) {
    അവസ്ഥ CURLFNM_PKW_INIT:
      എങ്കിൽ(ISLOWER(c))
        keyword[i] = c;
      അല്ലെങ്കിൽ എങ്കിൽ(c == ':')
        state = CURLFNM_PKW_DDOT;
      അല്ലെങ്കിൽ
        തിരികെ SETCHARSET_FAIL;
      നിർത്തുക;
    അവസ്ഥ CURLFNM_PKW_DDOT:
      എങ്കിൽ(c == ']')
        found = TRUE;
      അല്ലെങ്കിൽ
        തിരികെ SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  എങ്കിൽ(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  അല്ലെങ്കിൽ
    തിരികെ SETCHARSET_FAIL;
  തിരികെ SETCHARSET_OK;
}

/* Return the character class. */
സ്ഥിതം char_class charclass(unsigned അക്ഷരം c)
{
  എങ്കിൽ(ISUPPER(c))
    തിരികെ CCLASS_UPPER;
  എങ്കിൽ(ISLOWER(c))
    തിരികെ CCLASS_LOWER;
  എങ്കിൽ(ISDIGIT(c))
    തിരികെ CCLASS_DIGIT;
  തിരികെ CCLASS_OTHER;
}

/* Include a character or a range in set. */
സ്ഥിതം ശൂന്യം setcharorrange(സ്ഥിരം unsigned അക്ഷരം **pp, unsigned അക്ഷരം *charset)
{
  സ്ഥിരം unsigned അക്ഷരം *p = (*pp)++;
  unsigned അക്ഷരം c = *p++;

  charset[c] = 1;
  എങ്കിൽ(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned അക്ഷരം endrange = *p++;

    എങ്കിൽ(endrange == '\\')
      endrange = *p++;
    എങ്കിൽ(endrange >= c && charclass(endrange) == cc) {
      ആകുമ്പോൾ(c++ != endrange)
        എങ്കിൽ(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
സ്ഥിതം പൂർണ്ണസംഖ്യ setcharset(സ്ഥിരം unsigned അക്ഷരം **p, unsigned അക്ഷരം *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  unsigned അക്ഷരം c;

  memset(charset, 0, CURLFNM_CHSET_SIZE);
  വേണ്ടി(;;) {
    c = **p;
    എങ്കിൽ(!c)
      തിരികെ SETCHARSET_FAIL;

    തിരഞ്ഞെടുപ്പ്(state) {
    അവസ്ഥ CURLFNM_SCHS_DEFAULT:
      എങ്കിൽ(c == ']') {
        എങ്കിൽ(something_found)
          തിരികെ SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(c == '[') {
        സ്ഥിരം unsigned അക്ഷരം *pp = *p + 1;

        എങ്കിൽ(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        അല്ലെങ്കിൽ {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(c == '^' || c == '!') {
        എങ്കിൽ(!something_found) {
          എങ്കിൽ(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          അല്ലെങ്കിൽ
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        അല്ലെങ്കിൽ
          charset[c] = 1;
        (*p)++;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(c == '\\') {
        c = *(++(*p));
        എങ്കിൽ(c)
          setcharorrange(p, charset);
        അല്ലെങ്കിൽ
          charset['\\'] = 1;
        something_found = TRUE;
      }
      അല്ലെങ്കിൽ {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      നിർത്തുക;
    അവസ്ഥ CURLFNM_SCHS_RIGHTBR:
      എങ്കിൽ(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(c == ']') {
        തിരികെ SETCHARSET_OK;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      അല്ലെങ്കിൽ
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        പോകുക fail;
      നിർത്തുക;
    അവസ്ഥ CURLFNM_SCHS_RIGHTBRLEFTBR:
      എങ്കിൽ(c == ']')
        തിരികെ SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      നിർത്തുക;
    }
  }
fail:
  തിരികെ SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
സ്ഥിതം പൂർണ്ണസംഖ്യ matchtoken(സ്ഥിരം unsigned അക്ഷരം **pp, സ്ഥിരം unsigned അക്ഷരം **sp)
{
  സ്ഥിരം unsigned അക്ഷരം *p = *pp;
  സ്ഥിരം unsigned അക്ഷരം *s = *sp;

  തിരഞ്ഞെടുപ്പ്(*p) {
  അവസ്ഥ '?':
    നിർത്തുക;
  അവസ്ഥ '\\':
    എങ്കിൽ(p[1])
      p++;
    എങ്കിൽ(*s != *p)
      തിരികെ 0;
    നിർത്തുക;
  അവസ്ഥ '[': {
    unsigned അക്ഷരം charset[CURLFNM_CHSET_SIZE];
    സ്ഥിരം unsigned അക്ഷരം *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    എങ്കിൽ(!setcharset(&pp2, charset))
      തിരികെ -1; /* Syntax error in set; mismatch! */
    എങ്കിൽ(charset[(unsigned പൂർണ്ണസംഖ്യ)*s])
      found = TRUE;
    അല്ലെങ്കിൽ എങ്കിൽ(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    അല്ലെങ്കിൽ എങ്കിൽ(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    അല്ലെങ്കിൽ എങ്കിൽ(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    അല്ലെങ്കിൽ എങ്കിൽ(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    അല്ലെങ്കിൽ എങ്കിൽ(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    അല്ലെങ്കിൽ എങ്കിൽ(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    അല്ലെങ്കിൽ എങ്കിൽ(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    അല്ലെങ്കിൽ എങ്കിൽ(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    അല്ലെങ്കിൽ എങ്കിൽ(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    അല്ലെങ്കിൽ എങ്കിൽ(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    എങ്കിൽ(charset[CURLFNM_NEGATE])
      found = !found;

    എങ്കിൽ(!found)
      തിരികെ 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    തിരികെ 1;
  }
  സ്ഥിരസ്ഥിതി:
    എങ്കിൽ(*p != *s)
      തിരികെ 0;
    നിർത്തുക;
  }
  *pp = p + 1;
  *sp = s + 1;
  തിരികെ 1;
}

/* greedy match with backtracking to the most recent '*' */
സ്ഥിതം പൂർണ്ണസംഖ്യ loop(സ്ഥിരം unsigned അക്ഷരം *pattern, സ്ഥിരം unsigned അക്ഷരം *string)
{
  സ്ഥിരം unsigned അക്ഷരം *p = pattern;
  സ്ഥിരം unsigned അക്ഷരം *s = string;
  സ്ഥിരം unsigned അക്ഷരം *star_p = ശൂന്യ;
  സ്ഥിരം unsigned അക്ഷരം *star_s = ശൂന്യ;
  പൂർണ്ണസംഖ്യ maxstars = 2;

  ആകുമ്പോൾ(*s) {
    എങ്കിൽ(*p == '*') {
      എങ്കിൽ(!maxstars)
        തിരികെ CURL_FNMATCH_NOMATCH;
      maxstars--;
      വേണ്ടി(;;) {
        p++;
        എങ്കിൽ(!*p)
          തിരികെ CURL_FNMATCH_MATCH;
        എങ്കിൽ(*p == '?') {
          എങ്കിൽ(!*s)
            തിരികെ CURL_FNMATCH_NOMATCH;
          s++;
        }
        അല്ലെങ്കിൽ എങ്കിൽ(*p != '*')
          നിർത്തുക;
      }
      star_p = p;
      star_s = s;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(*p) {
      സ്ഥിരം unsigned അക്ഷരം *np = p;
      സ്ഥിരം unsigned അക്ഷരം *ns = s;
      പൂർണ്ണസംഖ്യ r = matchtoken(&np, &ns);
      എങ്കിൽ(r < 0)
        തിരികെ CURL_FNMATCH_NOMATCH;
      എങ്കിൽ(r) {
        p = np;
        s = ns;
        തുടരുക;
      }
      എങ്കിൽ(!star_p)
        തിരികെ CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(star_p) {
      p = star_p;
      s = ++star_s;
    }
    അല്ലെങ്കിൽ
      തിരികെ CURL_FNMATCH_NOMATCH;
  }
  ആകുമ്പോൾ(*p == '*') {
    എങ്കിൽ(!maxstars)
      തിരികെ CURL_FNMATCH_NOMATCH;
    maxstars--;
    ചെയ്യുക {
      p++;
      എങ്കിൽ(*p == '?')
        തിരികെ CURL_FNMATCH_NOMATCH;
    } ആകുമ്പോൾ(*p == '*');
  }
  തിരികെ *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
പൂർണ്ണസംഖ്യ Curl_fnmatch(ശൂന്യം *ptr, സ്ഥിരം അക്ഷരം *pattern, സ്ഥിരം അക്ഷരം *string)
{
  (ശൂന്യം)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  എങ്കിൽ(!pattern || !string) {
    തിരികെ CURL_FNMATCH_FAIL;
  }
  തിരികെ loop((സ്ഥിരം unsigned അക്ഷരം *)pattern,
              (സ്ഥിരം unsigned അക്ഷരം *)string);
}
#അല്ലെങ്കിൽ /* HAVE_FNMATCH */

#അന്തര്ഭാവ <fnmatch.h>
/*
 * @unittest: 1307
 */
പൂർണ്ണസംഖ്യ Curl_fnmatch(ശൂന്യം *ptr, സ്ഥിരം അക്ഷരം *pattern, സ്ഥിരം അക്ഷരം *string)
{
  (ശൂന്യം)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  എങ്കിൽ(!pattern || !string) {
    തിരികെ CURL_FNMATCH_FAIL;
  }

  തിരഞ്ഞെടുപ്പ്(fnmatch(pattern, string, 0)) {
  അവസ്ഥ 0:
    തിരികെ CURL_FNMATCH_MATCH;
  അവസ്ഥ FNM_NOMATCH:
    തിരികെ CURL_FNMATCH_NOMATCH;
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
