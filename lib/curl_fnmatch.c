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
#அந்தர்பாவ "curl_setup.h"

#ifndef CURL_DISABLE_FTP

#அந்தர்பாவ "curl_fnmatch.h"

#ifndef HAVE_FNMATCH

#define CURLFNM_CHARSET_LEN (அளவு(எழுத்து) * 256)
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

வகைவரை எண்ணிக்கை {
  CURLFNM_SCHS_DEFAULT = 0,
  CURLFNM_SCHS_RIGHTBR,
  CURLFNM_SCHS_RIGHTBRLEFTBR
} setcharset_state;

வகைவரை எண்ணிக்கை {
  CURLFNM_PKW_INIT = 0,
  CURLFNM_PKW_DDOT
} parsekey_state;

வகைவரை எண்ணிக்கை {
  CCLASS_OTHER = 0,
  CCLASS_DIGIT,
  CCLASS_UPPER,
  CCLASS_LOWER
} char_class;

#define SETCHARSET_OK     1
#define SETCHARSET_FAIL   0

நிலையான முழு parsekeyword(மாறிலி unsigned எழுத்து **pattern, unsigned எழுத்து *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  எழுத்து keyword[10] = { 0 };
  size_t i;
  மாறிலி unsigned எழுத்து *p = *pattern;
  bool found = FALSE;
  க்கு(i = 0; !found; i++) {
    எழுத்து c = (எழுத்து)*p++;
    எனில்(i >= அளவு(keyword))
      திரும்பு SETCHARSET_FAIL;
    தேர்வு(state) {
    நிலை CURLFNM_PKW_INIT:
      எனில்(ISLOWER(c))
        keyword[i] = c;
      இல்லையெனில் எனில்(c == ':')
        state = CURLFNM_PKW_DDOT;
      இல்லையெனில்
        திரும்பு SETCHARSET_FAIL;
      நிறுத்து;
    நிலை CURLFNM_PKW_DDOT:
      எனில்(c == ']')
        found = TRUE;
      இல்லையெனில்
        திரும்பு SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  எனில்(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  இல்லையெனில் எனில்(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  இல்லையெனில் எனில்(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  இல்லையெனில் எனில்(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  இல்லையெனில் எனில்(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  இல்லையெனில் எனில்(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  இல்லையெனில் எனில்(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  இல்லையெனில் எனில்(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  இல்லையெனில் எனில்(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  இல்லையெனில் எனில்(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  இல்லையெனில்
    திரும்பு SETCHARSET_FAIL;
  திரும்பு SETCHARSET_OK;
}

/* Return the character class. */
நிலையான char_class charclass(unsigned எழுத்து c)
{
  எனில்(ISUPPER(c))
    திரும்பு CCLASS_UPPER;
  எனில்(ISLOWER(c))
    திரும்பு CCLASS_LOWER;
  எனில்(ISDIGIT(c))
    திரும்பு CCLASS_DIGIT;
  திரும்பு CCLASS_OTHER;
}

/* Include a character or a range in set. */
நிலையான வெறுமை setcharorrange(மாறிலி unsigned எழுத்து **pp, unsigned எழுத்து *charset)
{
  மாறிலி unsigned எழுத்து *p = (*pp)++;
  unsigned எழுத்து c = *p++;

  charset[c] = 1;
  எனில்(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned எழுத்து endrange = *p++;

    எனில்(endrange == '\\')
      endrange = *p++;
    எனில்(endrange >= c && charclass(endrange) == cc) {
      போது(c++ != endrange)
        எனில்(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
நிலையான முழு setcharset(மாறிலி unsigned எழுத்து **p, unsigned எழுத்து *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  unsigned எழுத்து c;

  memset(charset, 0, CURLFNM_CHSET_SIZE);
  க்கு(;;) {
    c = **p;
    எனில்(!c)
      திரும்பு SETCHARSET_FAIL;

    தேர்வு(state) {
    நிலை CURLFNM_SCHS_DEFAULT:
      எனில்(c == ']') {
        எனில்(something_found)
          திரும்பு SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      இல்லையெனில் எனில்(c == '[') {
        மாறிலி unsigned எழுத்து *pp = *p + 1;

        எனில்(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        இல்லையெனில் {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      இல்லையெனில் எனில்(c == '^' || c == '!') {
        எனில்(!something_found) {
          எனில்(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          இல்லையெனில்
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        இல்லையெனில்
          charset[c] = 1;
        (*p)++;
      }
      இல்லையெனில் எனில்(c == '\\') {
        c = *(++(*p));
        எனில்(c)
          setcharorrange(p, charset);
        இல்லையெனில்
          charset['\\'] = 1;
        something_found = TRUE;
      }
      இல்லையெனில் {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      நிறுத்து;
    நிலை CURLFNM_SCHS_RIGHTBR:
      எனில்(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      இல்லையெனில் எனில்(c == ']') {
        திரும்பு SETCHARSET_OK;
      }
      இல்லையெனில் எனில்(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      இல்லையெனில்
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        செல் fail;
      நிறுத்து;
    நிலை CURLFNM_SCHS_RIGHTBRLEFTBR:
      எனில்(c == ']')
        திரும்பு SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      நிறுத்து;
    }
  }
fail:
  திரும்பு SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
நிலையான முழு matchtoken(மாறிலி unsigned எழுத்து **pp, மாறிலி unsigned எழுத்து **sp)
{
  மாறிலி unsigned எழுத்து *p = *pp;
  மாறிலி unsigned எழுத்து *s = *sp;

  தேர்வு(*p) {
  நிலை '?':
    நிறுத்து;
  நிலை '\\':
    எனில்(p[1])
      p++;
    எனில்(*s != *p)
      திரும்பு 0;
    நிறுத்து;
  நிலை '[': {
    unsigned எழுத்து charset[CURLFNM_CHSET_SIZE];
    மாறிலி unsigned எழுத்து *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    எனில்(!setcharset(&pp2, charset))
      திரும்பு -1; /* Syntax error in set; mismatch! */
    எனில்(charset[(unsigned முழு)*s])
      found = TRUE;
    இல்லையெனில் எனில்(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    இல்லையெனில் எனில்(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    இல்லையெனில் எனில்(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    இல்லையெனில் எனில்(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    இல்லையெனில் எனில்(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    இல்லையெனில் எனில்(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    இல்லையெனில் எனில்(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    இல்லையெனில் எனில்(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    இல்லையெனில் எனில்(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    இல்லையெனில் எனில்(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    எனில்(charset[CURLFNM_NEGATE])
      found = !found;

    எனில்(!found)
      திரும்பு 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    திரும்பு 1;
  }
  இயல்பு:
    எனில்(*p != *s)
      திரும்பு 0;
    நிறுத்து;
  }
  *pp = p + 1;
  *sp = s + 1;
  திரும்பு 1;
}

/* greedy match with backtracking to the most recent '*' */
நிலையான முழு loop(மாறிலி unsigned எழுத்து *pattern, மாறிலி unsigned எழுத்து *string)
{
  மாறிலி unsigned எழுத்து *p = pattern;
  மாறிலி unsigned எழுத்து *s = string;
  மாறிலி unsigned எழுத்து *star_p = வெற்று;
  மாறிலி unsigned எழுத்து *star_s = வெற்று;
  முழு maxstars = 2;

  போது(*s) {
    எனில்(*p == '*') {
      எனில்(!maxstars)
        திரும்பு CURL_FNMATCH_NOMATCH;
      maxstars--;
      க்கு(;;) {
        p++;
        எனில்(!*p)
          திரும்பு CURL_FNMATCH_MATCH;
        எனில்(*p == '?') {
          எனில்(!*s)
            திரும்பு CURL_FNMATCH_NOMATCH;
          s++;
        }
        இல்லையெனில் எனில்(*p != '*')
          நிறுத்து;
      }
      star_p = p;
      star_s = s;
    }
    இல்லையெனில் எனில்(*p) {
      மாறிலி unsigned எழுத்து *np = p;
      மாறிலி unsigned எழுத்து *ns = s;
      முழு r = matchtoken(&np, &ns);
      எனில்(r < 0)
        திரும்பு CURL_FNMATCH_NOMATCH;
      எனில்(r) {
        p = np;
        s = ns;
        தொடர்;
      }
      எனில்(!star_p)
        திரும்பு CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    இல்லையெனில் எனில்(star_p) {
      p = star_p;
      s = ++star_s;
    }
    இல்லையெனில்
      திரும்பு CURL_FNMATCH_NOMATCH;
  }
  போது(*p == '*') {
    எனில்(!maxstars)
      திரும்பு CURL_FNMATCH_NOMATCH;
    maxstars--;
    செய் {
      p++;
      எனில்(*p == '?')
        திரும்பு CURL_FNMATCH_NOMATCH;
    } போது(*p == '*');
  }
  திரும்பு *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
முழு Curl_fnmatch(வெறுமை *ptr, மாறிலி எழுத்து *pattern, மாறிலி எழுத்து *string)
{
  (வெறுமை)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  எனில்(!pattern || !string) {
    திரும்பு CURL_FNMATCH_FAIL;
  }
  திரும்பு loop((மாறிலி unsigned எழுத்து *)pattern,
              (மாறிலி unsigned எழுத்து *)string);
}
#இல்லையெனில் /* HAVE_FNMATCH */

#அந்தர்பாவ <fnmatch.h>
/*
 * @unittest: 1307
 */
முழு Curl_fnmatch(வெறுமை *ptr, மாறிலி எழுத்து *pattern, மாறிலி எழுத்து *string)
{
  (வெறுமை)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  எனில்(!pattern || !string) {
    திரும்பு CURL_FNMATCH_FAIL;
  }

  தேர்வு(fnmatch(pattern, string, 0)) {
  நிலை 0:
    திரும்பு CURL_FNMATCH_MATCH;
  நிலை FNM_NOMATCH:
    திரும்பு CURL_FNMATCH_NOMATCH;
  இயல்பு:
    திரும்பு CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
