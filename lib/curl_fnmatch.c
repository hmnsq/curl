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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP

#ಅನ್ತರ್ಭಾವ "curl_fnmatch.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_FNMATCH

#ವ್ಯಾಖ್ಯಾನಿಸು CURLFNM_CHARSET_LEN (ಗಾತ್ರ(ಅಕ್ಷರ) * 256)
#ವ್ಯಾಖ್ಯಾನಿಸು CURLFNM_CHSET_ಗಾತ್ರ (CURLFNM_CHARSET_LEN + 15)

#ವ್ಯಾಖ್ಯಾನಿಸು CURLFNM_NEGATE  CURLFNM_CHARSET_LEN

#ವ್ಯಾಖ್ಯಾನಿಸು CURLFNM_ALNUM   (CURLFNM_CHARSET_LEN + 1)
#ವ್ಯಾಖ್ಯಾನಿಸು CURLFNM_DIGIT   (CURLFNM_CHARSET_LEN + 2)
#ವ್ಯಾಖ್ಯಾನಿಸು CURLFNM_XDIGIT  (CURLFNM_CHARSET_LEN + 3)
#ವ್ಯಾಖ್ಯಾನಿಸು CURLFNM_ALPHA   (CURLFNM_CHARSET_LEN + 4)
#ವ್ಯಾಖ್ಯಾನಿಸು CURLFNM_PRINT   (CURLFNM_CHARSET_LEN + 5)
#ವ್ಯಾಖ್ಯಾನಿಸು CURLFNM_BLANK   (CURLFNM_CHARSET_LEN + 6)
#ವ್ಯಾಖ್ಯಾನಿಸು CURLFNM_ಅಧೋ   (CURLFNM_CHARSET_LEN + 7)
#ವ್ಯಾಖ್ಯಾನಿಸು CURLFNM_ನಕ್ಷೆ   (CURLFNM_CHARSET_LEN + 8)
#ವ್ಯಾಖ್ಯಾನಿಸು CURLFNM_SPACE   (CURLFNM_CHARSET_LEN + 9)
#ವ್ಯಾಖ್ಯಾನಿಸು CURLFNM_ಊರ್ಧ್ವ   (CURLFNM_CHARSET_LEN + 10)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  CURLFNM_SCHS_DEFAULT = 0,
  CURLFNM_SCHS_RIGHTBR,
  CURLFNM_SCHS_RIGHTBRLEFTBR
} setcharset_state;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  CURLFNM_PKW_INIT = 0,
  CURLFNM_PKW_DDOT
} parsekey_state;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  CCLASS_OTHER = 0,
  CCLASS_DIGIT,
  CCLASS_ಊರ್ಧ್ವ,
  CCLASS_ಅಧೋ
} char_class;

#ವ್ಯಾಖ್ಯಾನಿಸು SETCHARSET_OK     1
#ವ್ಯಾಖ್ಯಾನಿಸು SETCHARSET_FAIL   0

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parsekeyword(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ **ನಮೂನೆ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  ಅಕ್ಷರ keyword[10] = { 0 };
  size_t i;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *p = *ನಮೂನೆ;
  bool ಸಿಕ್ಕಿತು = FALSE;
  ಗಾಗಿ(i = 0; !ಸಿಕ್ಕಿತು; i++) {
    ಅಕ್ಷರ c = (ಅಕ್ಷರ)*p++;
    ಆದರೆ(i >= ಗಾತ್ರ(keyword))
      ಹಿಂತಿರುಗಿ SETCHARSET_FAIL;
    ಆಯ್ಕೆ(state) {
    ಸ್ಥಿತಿ CURLFNM_PKW_INIT:
      ಆದರೆ(ISLOWER(c))
        keyword[i] = c;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == ':')
        state = CURLFNM_PKW_DDOT;
      ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ SETCHARSET_FAIL;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLFNM_PKW_DDOT:
      ಆದರೆ(c == ']')
        ಸಿಕ್ಕಿತು = TRUE;
      ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ SETCHARSET_FAIL;
    }
  }
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು KEYLEN

  *ನಮೂನೆ = p; /* move caller's pattern pointer */
  ಆದರೆ(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(keyword, "graph"))
    charset[CURLFNM_ನಕ್ಷೆ] = 1;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(keyword, "upper"))
    charset[CURLFNM_ಊರ್ಧ್ವ] = 1;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(keyword, "lower"))
    charset[CURLFNM_ಅಧೋ] = 1;
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ SETCHARSET_FAIL;
  ಹಿಂತಿರುಗಿ SETCHARSET_OK;
}

/* Return the character class. */
ಸ್ಥಿರ char_class charclass(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c)
{
  ಆದರೆ(ISUPPER(c))
    ಹಿಂತಿರುಗಿ CCLASS_ಊರ್ಧ್ವ;
  ಆದರೆ(ISLOWER(c))
    ಹಿಂತಿರುಗಿ CCLASS_ಅಧೋ;
  ಆದರೆ(ISDIGIT(c))
    ಹಿಂತಿರುಗಿ CCLASS_DIGIT;
  ಹಿಂತಿರುಗಿ CCLASS_OTHER;
}

/* Include a character or a range in set. */
ಸ್ಥಿರ ಶೂನ್ಯ setcharorrange(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ **pp, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *charset)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *p = (*pp)++;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c = *p++;

  charset[c] = 1;
  ಆದರೆ(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ endrange = *p++;

    ಆದರೆ(endrange == '\\')
      endrange = *p++;
    ಆದರೆ(endrange >= c && charclass(endrange) == cc) {
      ಆಗ(c++ != endrange)
        ಆದರೆ(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ setcharset(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ **p, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_ಸಿಕ್ಕಿತು = FALSE;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c;

  memset(charset, 0, CURLFNM_CHSET_ಗಾತ್ರ);
  ಗಾಗಿ(;;) {
    c = **p;
    ಆದರೆ(!c)
      ಹಿಂತಿರುಗಿ SETCHARSET_FAIL;

    ಆಯ್ಕೆ(state) {
    ಸ್ಥಿತಿ CURLFNM_SCHS_DEFAULT:
      ಆದರೆ(c == ']') {
        ಆದರೆ(something_ಸಿಕ್ಕಿತು)
          ಹಿಂತಿರುಗಿ SETCHARSET_OK;
        something_ಸಿಕ್ಕಿತು = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == '[') {
        ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *pp = *p + 1;

        ಆದರೆ(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        ಇಲ್ಲದಿದ್ದರೆ {
          charset[c] = 1;
          (*p)++;
        }
        something_ಸಿಕ್ಕಿತು = TRUE;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == '^' || c == '!') {
        ಆದರೆ(!something_ಸಿಕ್ಕಿತು) {
          ಆದರೆ(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_ಸಿಕ್ಕಿತು = TRUE;
          }
          ಇಲ್ಲದಿದ್ದರೆ
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        ಇಲ್ಲದಿದ್ದರೆ
          charset[c] = 1;
        (*p)++;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == '\\') {
        c = *(++(*p));
        ಆದರೆ(c)
          setcharorrange(p, charset);
        ಇಲ್ಲದಿದ್ದರೆ
          charset['\\'] = 1;
        something_ಸಿಕ್ಕಿತು = TRUE;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        setcharorrange(p, charset);
        something_ಸಿಕ್ಕಿತು = TRUE;
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLFNM_SCHS_RIGHTBR:
      ಆದರೆ(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == ']') {
        ಹಿಂತಿರುಗಿ SETCHARSET_OK;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        ಹೋಗು fail;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLFNM_SCHS_RIGHTBRLEFTBR:
      ಆದರೆ(c == ']')
        ಹಿಂತಿರುಗಿ SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      ನಿಲ್ಲಿಸು;
    }
  }
fail:
  ಹಿಂತಿರುಗಿ SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ matchtoken(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ **pp, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ **sp)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *p = *pp;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *s = *sp;

  ಆಯ್ಕೆ(*p) {
  ಸ್ಥಿತಿ '?':
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ '\\':
    ಆದರೆ(p[1])
      p++;
    ಆದರೆ(*s != *p)
      ಹಿಂತಿರುಗಿ 0;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ '[': {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ charset[CURLFNM_CHSET_ಗಾತ್ರ];
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *pp2 = p + 1; /* Copy in case of syntax error. */
    bool ಸಿಕ್ಕಿತು = FALSE;
    ಆದರೆ(!setcharset(&pp2, charset))
      ಹಿಂತಿರುಗಿ -1; /* Syntax error in set; mismatch! */
    ಆದರೆ(charset[(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)*s])
      ಸಿಕ್ಕಿತು = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(charset[CURLFNM_ALNUM])
      ಸಿಕ್ಕಿತು = ISALNUM(*s);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(charset[CURLFNM_ALPHA])
      ಸಿಕ್ಕಿತು = ISALPHA(*s);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(charset[CURLFNM_DIGIT])
      ಸಿಕ್ಕಿತು = ISDIGIT(*s);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(charset[CURLFNM_XDIGIT])
      ಸಿಕ್ಕಿತು = ISXDIGIT(*s);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(charset[CURLFNM_PRINT])
      ಸಿಕ್ಕಿತು = ISPRINT(*s);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(charset[CURLFNM_SPACE])
      ಸಿಕ್ಕಿತು = ISBLANK(*s);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(charset[CURLFNM_ಊರ್ಧ್ವ])
      ಸಿಕ್ಕಿತು = ISUPPER(*s);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(charset[CURLFNM_ಅಧೋ])
      ಸಿಕ್ಕಿತು = ISLOWER(*s);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(charset[CURLFNM_BLANK])
      ಸಿಕ್ಕಿತು = ISBLANK(*s);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(charset[CURLFNM_ನಕ್ಷೆ])
      ಸಿಕ್ಕಿತು = ISGRAPH(*s);

    ಆದರೆ(charset[CURLFNM_NEGATE])
      ಸಿಕ್ಕಿತು = !ಸಿಕ್ಕಿತು;

    ಆದರೆ(!ಸಿಕ್ಕಿತು)
      ಹಿಂತಿರುಗಿ 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    ಹಿಂತಿರುಗಿ 1;
  }
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಆದರೆ(*p != *s)
      ಹಿಂತಿರುಗಿ 0;
    ನಿಲ್ಲಿಸು;
  }
  *pp = p + 1;
  *sp = s + 1;
  ಹಿಂತಿರುಗಿ 1;
}

/* greedy match with backtracking to the most recent '*' */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ loop(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ನಮೂನೆ, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *string)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *p = ನಮೂನೆ;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *s = string;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ನಕ್ಷತ್ರ_p = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ನಕ್ಷತ್ರ_s = ಖಾಲಿ;
  ಪೂರ್ಣಾಂಕ maxstars = 2;

  ಆಗ(*s) {
    ಆದರೆ(*p == '*') {
      ಆದರೆ(!maxstars)
        ಹಿಂತಿರುಗಿ CURL_FNMATCH_NOMATCH;
      maxstars--;
      ಗಾಗಿ(;;) {
        p++;
        ಆದರೆ(!*p)
          ಹಿಂತಿರುಗಿ CURL_FNMATCH_ಹೊಂದಿಕೆ;
        ಆದರೆ(*p == '?') {
          ಆದರೆ(!*s)
            ಹಿಂತಿರುಗಿ CURL_FNMATCH_NOMATCH;
          s++;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*p != '*')
          ನಿಲ್ಲಿಸು;
      }
      ನಕ್ಷತ್ರ_p = p;
      ನಕ್ಷತ್ರ_s = s;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*p) {
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *np = p;
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ns = s;
      ಪೂರ್ಣಾಂಕ r = matchtoken(&np, &ns);
      ಆದರೆ(r < 0)
        ಹಿಂತಿರುಗಿ CURL_FNMATCH_NOMATCH;
      ಆದರೆ(r) {
        p = np;
        s = ns;
        ಮುಂದುವರಿ;
      }
      ಆದರೆ(!ನಕ್ಷತ್ರ_p)
        ಹಿಂತಿರುಗಿ CURL_FNMATCH_NOMATCH;
      p = ನಕ್ಷತ್ರ_p;
      s = ++ನಕ್ಷತ್ರ_s;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ನಕ್ಷತ್ರ_p) {
      p = ನಕ್ಷತ್ರ_p;
      s = ++ನಕ್ಷತ್ರ_s;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ CURL_FNMATCH_NOMATCH;
  }
  ಆಗ(*p == '*') {
    ಆದರೆ(!maxstars)
      ಹಿಂತಿರುಗಿ CURL_FNMATCH_NOMATCH;
    maxstars--;
    ಮಾಡು {
      p++;
      ಆದರೆ(*p == '?')
        ಹಿಂತಿರುಗಿ CURL_FNMATCH_NOMATCH;
    } ಆಗ(*p == '*');
  }
  ಹಿಂತಿರುಗಿ *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_ಹೊಂದಿಕೆ;
}

/*
 * @unittest: 1307
 */
ಪೂರ್ಣಾಂಕ Curl_fnmatch(ಶೂನ್ಯ *ptr, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ನಮೂನೆ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string)
{
  (ಶೂನ್ಯ)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ಆದರೆ(!ನಮೂನೆ || !string) {
    ಹಿಂತಿರುಗಿ CURL_FNMATCH_FAIL;
  }
  ಹಿಂತಿರುಗಿ loop((ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ನಮೂನೆ,
              (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)string);
}
#ಇಲ್ಲದಿದ್ದರೆ /* HAVE_FNMATCH */

#ಅನ್ತರ್ಭಾವ <fnmatch.h>
/*
 * @unittest: 1307
 */
ಪೂರ್ಣಾಂಕ Curl_fnmatch(ಶೂನ್ಯ *ptr, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ನಮೂನೆ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string)
{
  (ಶೂನ್ಯ)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ಆದರೆ(!ನಮೂನೆ || !string) {
    ಹಿಂತಿರುಗಿ CURL_FNMATCH_FAIL;
  }

  ಆಯ್ಕೆ(fnmatch(ನಮೂನೆ, string, 0)) {
  ಸ್ಥಿತಿ 0:
    ಹಿಂತಿರುಗಿ CURL_FNMATCH_ಹೊಂದಿಕೆ;
  ಸ್ಥಿತಿ FNM_NOMATCH:
    ಹಿಂತಿರುಗಿ CURL_FNMATCH_NOMATCH;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#ಆದರೆ_ಅಂತ್ಯ /* !HAVE_FNMATCH */

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_FTP */
