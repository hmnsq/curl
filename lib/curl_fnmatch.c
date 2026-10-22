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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_fnmatch.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_FNMATCH

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLFNM_CHARSET_LEN (ਆਕਾਰ(ਅੱਖਰ) * 256)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLFNM_CHSET_ਮਾਪ (CURLFNM_CHARSET_LEN + 15)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLFNM_NEGATE  CURLFNM_CHARSET_LEN

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLFNM_ALNUM   (CURLFNM_CHARSET_LEN + 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLFNM_DIGIT   (CURLFNM_CHARSET_LEN + 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLFNM_XDIGIT  (CURLFNM_CHARSET_LEN + 3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLFNM_ALPHA   (CURLFNM_CHARSET_LEN + 4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLFNM_PRINT   (CURLFNM_CHARSET_LEN + 5)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLFNM_BLANK   (CURLFNM_CHARSET_LEN + 6)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLFNM_ਹੇਠਲਾ   (CURLFNM_CHARSET_LEN + 7)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLFNM_ਗ੍ਰਾਫ   (CURLFNM_CHARSET_LEN + 8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLFNM_SPACE   (CURLFNM_CHARSET_LEN + 9)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLFNM_ਉੱਤਲਾ   (CURLFNM_CHARSET_LEN + 10)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  CURLFNM_SCHS_DEFAULT = 0,
  CURLFNM_SCHS_RIGHTBR,
  CURLFNM_SCHS_RIGHTBRLEFTBR
} setcharset_state;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  CURLFNM_PKW_INIT = 0,
  CURLFNM_PKW_DDOT
} parsekey_state;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  CCLASS_OTHER = 0,
  CCLASS_DIGIT,
  CCLASS_ਉੱਤਲਾ,
  CCLASS_ਹੇਠਲਾ
} char_class;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SETCHARSET_OK     1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SETCHARSET_FAIL   0

ਸਥਿਰ ਪੂਰਨ parsekeyword(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ **ਨਮੂਨਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  ਅੱਖਰ keyword[10] = { 0 };
  size_t i;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *p = *ਨਮੂਨਾ;
  bool ਮਿਲਿਆ = FALSE;
  ਲਈ(i = 0; !ਮਿਲਿਆ; i++) {
    ਅੱਖਰ c = (ਅੱਖਰ)*p++;
    ਜੇ(i >= ਆਕਾਰ(keyword))
      ਵਾਪਸ SETCHARSET_FAIL;
    ਚੋਣ(state) {
    ਹਾਲਤ CURLFNM_PKW_INIT:
      ਜੇ(ISLOWER(c))
        keyword[i] = c;
      ਨਹੀਂਤਾਂ ਜੇ(c == ':')
        state = CURLFNM_PKW_DDOT;
      ਨਹੀਂਤਾਂ
        ਵਾਪਸ SETCHARSET_FAIL;
      ਰੋਕੋ;
    ਹਾਲਤ CURLFNM_PKW_DDOT:
      ਜੇ(c == ']')
        ਮਿਲਿਆ = TRUE;
      ਨਹੀਂਤਾਂ
        ਵਾਪਸ SETCHARSET_FAIL;
    }
  }
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ KEYLEN

  *ਨਮੂਨਾ = p; /* move caller's pattern pointer */
  ਜੇ(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  ਨਹੀਂਤਾਂ ਜੇ(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  ਨਹੀਂਤਾਂ ਜੇ(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  ਨਹੀਂਤਾਂ ਜੇ(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  ਨਹੀਂਤਾਂ ਜੇ(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  ਨਹੀਂਤਾਂ ਜੇ(!strcmp(keyword, "graph"))
    charset[CURLFNM_ਗ੍ਰਾਫ] = 1;
  ਨਹੀਂਤਾਂ ਜੇ(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  ਨਹੀਂਤਾਂ ਜੇ(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  ਨਹੀਂਤਾਂ ਜੇ(!strcmp(keyword, "upper"))
    charset[CURLFNM_ਉੱਤਲਾ] = 1;
  ਨਹੀਂਤਾਂ ਜੇ(!strcmp(keyword, "lower"))
    charset[CURLFNM_ਹੇਠਲਾ] = 1;
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ SETCHARSET_FAIL;
  ਵਾਪਸ SETCHARSET_OK;
}

/* Return the character class. */
ਸਥਿਰ char_class charclass(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c)
{
  ਜੇ(ISUPPER(c))
    ਵਾਪਸ CCLASS_ਉੱਤਲਾ;
  ਜੇ(ISLOWER(c))
    ਵਾਪਸ CCLASS_ਹੇਠਲਾ;
  ਜੇ(ISDIGIT(c))
    ਵਾਪਸ CCLASS_DIGIT;
  ਵਾਪਸ CCLASS_OTHER;
}

/* Include a character or a range in set. */
ਸਥਿਰ ਖਾਲੀ setcharorrange(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ **pp, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *charset)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *p = (*pp)++;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c = *p++;

  charset[c] = 1;
  ਜੇ(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ endrange = *p++;

    ਜੇ(endrange == '\\')
      endrange = *p++;
    ਜੇ(endrange >= c && charclass(endrange) == cc) {
      ਜਦੋਂ(c++ != endrange)
        ਜੇ(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
ਸਥਿਰ ਪੂਰਨ setcharset(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ **p, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_ਮਿਲਿਆ = FALSE;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c;

  memset(charset, 0, CURLFNM_CHSET_ਮਾਪ);
  ਲਈ(;;) {
    c = **p;
    ਜੇ(!c)
      ਵਾਪਸ SETCHARSET_FAIL;

    ਚੋਣ(state) {
    ਹਾਲਤ CURLFNM_SCHS_DEFAULT:
      ਜੇ(c == ']') {
        ਜੇ(something_ਮਿਲਿਆ)
          ਵਾਪਸ SETCHARSET_OK;
        something_ਮਿਲਿਆ = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      ਨਹੀਂਤਾਂ ਜੇ(c == '[') {
        ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *pp = *p + 1;

        ਜੇ(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        ਨਹੀਂਤਾਂ {
          charset[c] = 1;
          (*p)++;
        }
        something_ਮਿਲਿਆ = TRUE;
      }
      ਨਹੀਂਤਾਂ ਜੇ(c == '^' || c == '!') {
        ਜੇ(!something_ਮਿਲਿਆ) {
          ਜੇ(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_ਮਿਲਿਆ = TRUE;
          }
          ਨਹੀਂਤਾਂ
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        ਨਹੀਂਤਾਂ
          charset[c] = 1;
        (*p)++;
      }
      ਨਹੀਂਤਾਂ ਜੇ(c == '\\') {
        c = *(++(*p));
        ਜੇ(c)
          setcharorrange(p, charset);
        ਨਹੀਂਤਾਂ
          charset['\\'] = 1;
        something_ਮਿਲਿਆ = TRUE;
      }
      ਨਹੀਂਤਾਂ {
        setcharorrange(p, charset);
        something_ਮਿਲਿਆ = TRUE;
      }
      ਰੋਕੋ;
    ਹਾਲਤ CURLFNM_SCHS_RIGHTBR:
      ਜੇ(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      ਨਹੀਂਤਾਂ ਜੇ(c == ']') {
        ਵਾਪਸ SETCHARSET_OK;
      }
      ਨਹੀਂਤਾਂ ਜੇ(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      ਨਹੀਂਤਾਂ
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        ਜਾਓ fail;
      ਰੋਕੋ;
    ਹਾਲਤ CURLFNM_SCHS_RIGHTBRLEFTBR:
      ਜੇ(c == ']')
        ਵਾਪਸ SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      ਰੋਕੋ;
    }
  }
fail:
  ਵਾਪਸ SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
ਸਥਿਰ ਪੂਰਨ matchtoken(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ **pp, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ **sp)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *p = *pp;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *s = *sp;

  ਚੋਣ(*p) {
  ਹਾਲਤ '?':
    ਰੋਕੋ;
  ਹਾਲਤ '\\':
    ਜੇ(p[1])
      p++;
    ਜੇ(*s != *p)
      ਵਾਪਸ 0;
    ਰੋਕੋ;
  ਹਾਲਤ '[': {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ charset[CURLFNM_CHSET_ਮਾਪ];
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *pp2 = p + 1; /* Copy in case of syntax error. */
    bool ਮਿਲਿਆ = FALSE;
    ਜੇ(!setcharset(&pp2, charset))
      ਵਾਪਸ -1; /* Syntax error in set; mismatch! */
    ਜੇ(charset[(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)*s])
      ਮਿਲਿਆ = TRUE;
    ਨਹੀਂਤਾਂ ਜੇ(charset[CURLFNM_ALNUM])
      ਮਿਲਿਆ = ISALNUM(*s);
    ਨਹੀਂਤਾਂ ਜੇ(charset[CURLFNM_ALPHA])
      ਮਿਲਿਆ = ISALPHA(*s);
    ਨਹੀਂਤਾਂ ਜੇ(charset[CURLFNM_DIGIT])
      ਮਿਲਿਆ = ISDIGIT(*s);
    ਨਹੀਂਤਾਂ ਜੇ(charset[CURLFNM_XDIGIT])
      ਮਿਲਿਆ = ISXDIGIT(*s);
    ਨਹੀਂਤਾਂ ਜੇ(charset[CURLFNM_PRINT])
      ਮਿਲਿਆ = ISPRINT(*s);
    ਨਹੀਂਤਾਂ ਜੇ(charset[CURLFNM_SPACE])
      ਮਿਲਿਆ = ISBLANK(*s);
    ਨਹੀਂਤਾਂ ਜੇ(charset[CURLFNM_ਉੱਤਲਾ])
      ਮਿਲਿਆ = ISUPPER(*s);
    ਨਹੀਂਤਾਂ ਜੇ(charset[CURLFNM_ਹੇਠਲਾ])
      ਮਿਲਿਆ = ISLOWER(*s);
    ਨਹੀਂਤਾਂ ਜੇ(charset[CURLFNM_BLANK])
      ਮਿਲਿਆ = ISBLANK(*s);
    ਨਹੀਂਤਾਂ ਜੇ(charset[CURLFNM_ਗ੍ਰਾਫ])
      ਮਿਲਿਆ = ISGRAPH(*s);

    ਜੇ(charset[CURLFNM_NEGATE])
      ਮਿਲਿਆ = !ਮਿਲਿਆ;

    ਜੇ(!ਮਿਲਿਆ)
      ਵਾਪਸ 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    ਵਾਪਸ 1;
  }
  ਮੂਲ:
    ਜੇ(*p != *s)
      ਵਾਪਸ 0;
    ਰੋਕੋ;
  }
  *pp = p + 1;
  *sp = s + 1;
  ਵਾਪਸ 1;
}

/* greedy match with backtracking to the most recent '*' */
ਸਥਿਰ ਪੂਰਨ loop(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਮੂਨਾ, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *string)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *p = ਨਮੂਨਾ;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *s = string;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਤਾਰਾ_p = ਨਿਰਾ;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਤਾਰਾ_s = ਨਿਰਾ;
  ਪੂਰਨ maxstars = 2;

  ਜਦੋਂ(*s) {
    ਜੇ(*p == '*') {
      ਜੇ(!maxstars)
        ਵਾਪਸ CURL_FNMATCH_NOMATCH;
      maxstars--;
      ਲਈ(;;) {
        p++;
        ਜੇ(!*p)
          ਵਾਪਸ CURL_FNMATCH_ਮੇਲ;
        ਜੇ(*p == '?') {
          ਜੇ(!*s)
            ਵਾਪਸ CURL_FNMATCH_NOMATCH;
          s++;
        }
        ਨਹੀਂਤਾਂ ਜੇ(*p != '*')
          ਰੋਕੋ;
      }
      ਤਾਰਾ_p = p;
      ਤਾਰਾ_s = s;
    }
    ਨਹੀਂਤਾਂ ਜੇ(*p) {
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *np = p;
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ns = s;
      ਪੂਰਨ r = matchtoken(&np, &ns);
      ਜੇ(r < 0)
        ਵਾਪਸ CURL_FNMATCH_NOMATCH;
      ਜੇ(r) {
        p = np;
        s = ns;
        ਜਾਰੀ;
      }
      ਜੇ(!ਤਾਰਾ_p)
        ਵਾਪਸ CURL_FNMATCH_NOMATCH;
      p = ਤਾਰਾ_p;
      s = ++ਤਾਰਾ_s;
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਤਾਰਾ_p) {
      p = ਤਾਰਾ_p;
      s = ++ਤਾਰਾ_s;
    }
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ CURL_FNMATCH_NOMATCH;
  }
  ਜਦੋਂ(*p == '*') {
    ਜੇ(!maxstars)
      ਵਾਪਸ CURL_FNMATCH_NOMATCH;
    maxstars--;
    ਕਰੋ {
      p++;
      ਜੇ(*p == '?')
        ਵਾਪਸ CURL_FNMATCH_NOMATCH;
    } ਜਦੋਂ(*p == '*');
  }
  ਵਾਪਸ *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_ਮੇਲ;
}

/*
 * @unittest: 1307
 */
ਪੂਰਨ Curl_fnmatch(ਖਾਲੀ *ptr, ਅਚਲ ਅੱਖਰ *ਨਮੂਨਾ, ਅਚਲ ਅੱਖਰ *string)
{
  (ਖਾਲੀ)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ਜੇ(!ਨਮੂਨਾ || !string) {
    ਵਾਪਸ CURL_FNMATCH_FAIL;
  }
  ਵਾਪਸ loop((ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ਨਮੂਨਾ,
              (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)string);
}
#ਨਹੀਂਤਾਂ /* HAVE_FNMATCH */

#ਅਨ੍ਤਰ੍ਭਾਵ <fnmatch.h>
/*
 * @unittest: 1307
 */
ਪੂਰਨ Curl_fnmatch(ਖਾਲੀ *ptr, ਅਚਲ ਅੱਖਰ *ਨਮੂਨਾ, ਅਚਲ ਅੱਖਰ *string)
{
  (ਖਾਲੀ)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ਜੇ(!ਨਮੂਨਾ || !string) {
    ਵਾਪਸ CURL_FNMATCH_FAIL;
  }

  ਚੋਣ(fnmatch(ਨਮੂਨਾ, string, 0)) {
  ਹਾਲਤ 0:
    ਵਾਪਸ CURL_FNMATCH_ਮੇਲ;
  ਹਾਲਤ FNM_NOMATCH:
    ਵਾਪਸ CURL_FNMATCH_NOMATCH;
  ਮੂਲ:
    ਵਾਪਸ CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#ਜੇ_ਅੰਤ /* !HAVE_FNMATCH */

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_FTP */
