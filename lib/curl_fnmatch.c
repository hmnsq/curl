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
#અન્તર્ભાવ "curl_setup.h"

#ifndef CURL_DISABLE_FTP

#અન્તર્ભાવ "curl_fnmatch.h"

#ifndef HAVE_FNMATCH

#define CURLFNM_CHARSET_LEN (કદ(અક્ષર) * 256)
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

પ્રકારવ્યાખ્યા ગણના {
  CURLFNM_SCHS_DEFAULT = 0,
  CURLFNM_SCHS_RIGHTBR,
  CURLFNM_SCHS_RIGHTBRLEFTBR
} setcharset_state;

પ્રકારવ્યાખ્યા ગણના {
  CURLFNM_PKW_INIT = 0,
  CURLFNM_PKW_DDOT
} parsekey_state;

પ્રકારવ્યાખ્યા ગણના {
  CCLASS_OTHER = 0,
  CCLASS_DIGIT,
  CCLASS_UPPER,
  CCLASS_LOWER
} char_class;

#define SETCHARSET_OK     1
#define SETCHARSET_FAIL   0

સ્થિર પૂર્ણાંક parsekeyword(અચલ unsigned અક્ષર **pattern, unsigned અક્ષર *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  અક્ષર keyword[10] = { 0 };
  size_t i;
  અચલ unsigned અક્ષર *p = *pattern;
  bool found = FALSE;
  માટે(i = 0; !found; i++) {
    અક્ષર c = (અક્ષર)*p++;
    જો(i >= કદ(keyword))
      પાછા SETCHARSET_FAIL;
    પસંદગી(state) {
    સ્થિતિ CURLFNM_PKW_INIT:
      જો(ISLOWER(c))
        keyword[i] = c;
      નહીંતર જો(c == ':')
        state = CURLFNM_PKW_DDOT;
      નહીંતર
        પાછા SETCHARSET_FAIL;
      તોડો;
    સ્થિતિ CURLFNM_PKW_DDOT:
      જો(c == ']')
        found = TRUE;
      નહીંતર
        પાછા SETCHARSET_FAIL;
    }
  }
#undef KEYLEN

  *pattern = p; /* move caller's pattern pointer */
  જો(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  નહીંતર જો(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  નહીંતર જો(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  નહીંતર જો(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  નહીંતર જો(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  નહીંતર જો(!strcmp(keyword, "graph"))
    charset[CURLFNM_GRAPH] = 1;
  નહીંતર જો(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  નહીંતર જો(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  નહીંતર જો(!strcmp(keyword, "upper"))
    charset[CURLFNM_UPPER] = 1;
  નહીંતર જો(!strcmp(keyword, "lower"))
    charset[CURLFNM_LOWER] = 1;
  નહીંતર
    પાછા SETCHARSET_FAIL;
  પાછા SETCHARSET_OK;
}

/* Return the character class. */
સ્થિર char_class charclass(unsigned અક્ષર c)
{
  જો(ISUPPER(c))
    પાછા CCLASS_UPPER;
  જો(ISLOWER(c))
    પાછા CCLASS_LOWER;
  જો(ISDIGIT(c))
    પાછા CCLASS_DIGIT;
  પાછા CCLASS_OTHER;
}

/* Include a character or a range in set. */
સ્થિર શૂન્ય setcharorrange(અચલ unsigned અક્ષર **pp, unsigned અક્ષર *charset)
{
  અચલ unsigned અક્ષર *p = (*pp)++;
  unsigned અક્ષર c = *p++;

  charset[c] = 1;
  જો(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    unsigned અક્ષર endrange = *p++;

    જો(endrange == '\\')
      endrange = *p++;
    જો(endrange >= c && charclass(endrange) == cc) {
      જ્યારે(c++ != endrange)
        જો(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
સ્થિર પૂર્ણાંક setcharset(અચલ unsigned અક્ષર **p, unsigned અક્ષર *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_found = FALSE;
  unsigned અક્ષર c;

  memset(charset, 0, CURLFNM_CHSET_SIZE);
  માટે(;;) {
    c = **p;
    જો(!c)
      પાછા SETCHARSET_FAIL;

    પસંદગી(state) {
    સ્થિતિ CURLFNM_SCHS_DEFAULT:
      જો(c == ']') {
        જો(something_found)
          પાછા SETCHARSET_OK;
        something_found = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      નહીંતર જો(c == '[') {
        અચલ unsigned અક્ષર *pp = *p + 1;

        જો(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        નહીંતર {
          charset[c] = 1;
          (*p)++;
        }
        something_found = TRUE;
      }
      નહીંતર જો(c == '^' || c == '!') {
        જો(!something_found) {
          જો(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_found = TRUE;
          }
          નહીંતર
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        નહીંતર
          charset[c] = 1;
        (*p)++;
      }
      નહીંતર જો(c == '\\') {
        c = *(++(*p));
        જો(c)
          setcharorrange(p, charset);
        નહીંતર
          charset['\\'] = 1;
        something_found = TRUE;
      }
      નહીંતર {
        setcharorrange(p, charset);
        something_found = TRUE;
      }
      તોડો;
    સ્થિતિ CURLFNM_SCHS_RIGHTBR:
      જો(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      નહીંતર જો(c == ']') {
        પાછા SETCHARSET_OK;
      }
      નહીંતર જો(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      નહીંતર
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        જાઓ fail;
      તોડો;
    સ્થિતિ CURLFNM_SCHS_RIGHTBRLEFTBR:
      જો(c == ']')
        પાછા SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      તોડો;
    }
  }
fail:
  પાછા SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
સ્થિર પૂર્ણાંક matchtoken(અચલ unsigned અક્ષર **pp, અચલ unsigned અક્ષર **sp)
{
  અચલ unsigned અક્ષર *p = *pp;
  અચલ unsigned અક્ષર *s = *sp;

  પસંદગી(*p) {
  સ્થિતિ '?':
    તોડો;
  સ્થિતિ '\\':
    જો(p[1])
      p++;
    જો(*s != *p)
      પાછા 0;
    તોડો;
  સ્થિતિ '[': {
    unsigned અક્ષર charset[CURLFNM_CHSET_SIZE];
    અચલ unsigned અક્ષર *pp2 = p + 1; /* Copy in case of syntax error. */
    bool found = FALSE;
    જો(!setcharset(&pp2, charset))
      પાછા -1; /* Syntax error in set; mismatch! */
    જો(charset[(unsigned પૂર્ણાંક)*s])
      found = TRUE;
    નહીંતર જો(charset[CURLFNM_ALNUM])
      found = ISALNUM(*s);
    નહીંતર જો(charset[CURLFNM_ALPHA])
      found = ISALPHA(*s);
    નહીંતર જો(charset[CURLFNM_DIGIT])
      found = ISDIGIT(*s);
    નહીંતર જો(charset[CURLFNM_XDIGIT])
      found = ISXDIGIT(*s);
    નહીંતર જો(charset[CURLFNM_PRINT])
      found = ISPRINT(*s);
    નહીંતર જો(charset[CURLFNM_SPACE])
      found = ISBLANK(*s);
    નહીંતર જો(charset[CURLFNM_UPPER])
      found = ISUPPER(*s);
    નહીંતર જો(charset[CURLFNM_LOWER])
      found = ISLOWER(*s);
    નહીંતર જો(charset[CURLFNM_BLANK])
      found = ISBLANK(*s);
    નહીંતર જો(charset[CURLFNM_GRAPH])
      found = ISGRAPH(*s);

    જો(charset[CURLFNM_NEGATE])
      found = !found;

    જો(!found)
      પાછા 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    પાછા 1;
  }
  મૂળભૂત:
    જો(*p != *s)
      પાછા 0;
    તોડો;
  }
  *pp = p + 1;
  *sp = s + 1;
  પાછા 1;
}

/* greedy match with backtracking to the most recent '*' */
સ્થિર પૂર્ણાંક loop(અચલ unsigned અક્ષર *pattern, અચલ unsigned અક્ષર *string)
{
  અચલ unsigned અક્ષર *p = pattern;
  અચલ unsigned અક્ષર *s = string;
  અચલ unsigned અક્ષર *star_p = ખાલી;
  અચલ unsigned અક્ષર *star_s = ખાલી;
  પૂર્ણાંક maxstars = 2;

  જ્યારે(*s) {
    જો(*p == '*') {
      જો(!maxstars)
        પાછા CURL_FNMATCH_NOMATCH;
      maxstars--;
      માટે(;;) {
        p++;
        જો(!*p)
          પાછા CURL_FNMATCH_MATCH;
        જો(*p == '?') {
          જો(!*s)
            પાછા CURL_FNMATCH_NOMATCH;
          s++;
        }
        નહીંતર જો(*p != '*')
          તોડો;
      }
      star_p = p;
      star_s = s;
    }
    નહીંતર જો(*p) {
      અચલ unsigned અક્ષર *np = p;
      અચલ unsigned અક્ષર *ns = s;
      પૂર્ણાંક r = matchtoken(&np, &ns);
      જો(r < 0)
        પાછા CURL_FNMATCH_NOMATCH;
      જો(r) {
        p = np;
        s = ns;
        ચાલુ;
      }
      જો(!star_p)
        પાછા CURL_FNMATCH_NOMATCH;
      p = star_p;
      s = ++star_s;
    }
    નહીંતર જો(star_p) {
      p = star_p;
      s = ++star_s;
    }
    નહીંતર
      પાછા CURL_FNMATCH_NOMATCH;
  }
  જ્યારે(*p == '*') {
    જો(!maxstars)
      પાછા CURL_FNMATCH_NOMATCH;
    maxstars--;
    કરો {
      p++;
      જો(*p == '?')
        પાછા CURL_FNMATCH_NOMATCH;
    } જ્યારે(*p == '*');
  }
  પાછા *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_MATCH;
}

/*
 * @unittest: 1307
 */
પૂર્ણાંક Curl_fnmatch(શૂન્ય *ptr, અચલ અક્ષર *pattern, અચલ અક્ષર *string)
{
  (શૂન્ય)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  જો(!pattern || !string) {
    પાછા CURL_FNMATCH_FAIL;
  }
  પાછા loop((અચલ unsigned અક્ષર *)pattern,
              (અચલ unsigned અક્ષર *)string);
}
#નહીંતર /* HAVE_FNMATCH */

#અન્તર્ભાવ <fnmatch.h>
/*
 * @unittest: 1307
 */
પૂર્ણાંક Curl_fnmatch(શૂન્ય *ptr, અચલ અક્ષર *pattern, અચલ અક્ષર *string)
{
  (શૂન્ય)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  જો(!pattern || !string) {
    પાછા CURL_FNMATCH_FAIL;
  }

  પસંદગી(fnmatch(pattern, string, 0)) {
  સ્થિતિ 0:
    પાછા CURL_FNMATCH_MATCH;
  સ્થિતિ FNM_NOMATCH:
    પાછા CURL_FNMATCH_NOMATCH;
  મૂળભૂત:
    પાછા CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#endif /* !HAVE_FNMATCH */

#endif /* !CURL_DISABLE_FTP */
