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
#ضمن "curl_setup.h"

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FTP

#ضمن "curl_fnmatch.h"

#جيڪڏهن_وصف_نه_ٿيل HAVE_FNMATCH

#وصف_ڪريو CURLFNM_CHARSET_LEN (حجم(حرف) * 256)
#وصف_ڪريو CURLFNM_CHSET_سائيز (CURLFNM_CHARSET_LEN + 15)

#وصف_ڪريو CURLFNM_NEGATE  CURLFNM_CHARSET_LEN

#وصف_ڪريو CURLFNM_ALNUM   (CURLFNM_CHARSET_LEN + 1)
#وصف_ڪريو CURLFNM_DIGIT   (CURLFNM_CHARSET_LEN + 2)
#وصف_ڪريو CURLFNM_XDIGIT  (CURLFNM_CHARSET_LEN + 3)
#وصف_ڪريو CURLFNM_ALPHA   (CURLFNM_CHARSET_LEN + 4)
#وصف_ڪريو CURLFNM_PRINT   (CURLFNM_CHARSET_LEN + 5)
#وصف_ڪريو CURLFNM_BLANK   (CURLFNM_CHARSET_LEN + 6)
#وصف_ڪريو CURLFNM_هيٺاهون   (CURLFNM_CHARSET_LEN + 7)
#وصف_ڪريو CURLFNM_گراف   (CURLFNM_CHARSET_LEN + 8)
#وصف_ڪريو CURLFNM_SPACE   (CURLFNM_CHARSET_LEN + 9)
#وصف_ڪريو CURLFNM_مٿانهون   (CURLFNM_CHARSET_LEN + 10)

تعريفنوع تعداد {
  CURLFNM_SCHS_DEFAULT = 0,
  CURLFNM_SCHS_RIGHTBR,
  CURLFNM_SCHS_RIGHTBRLEFTBR
} setcharset_state;

تعريفنوع تعداد {
  CURLFNM_PKW_INIT = 0,
  CURLFNM_PKW_DDOT
} parsekey_state;

تعريفنوع تعداد {
  CCLASS_OTHER = 0,
  CCLASS_DIGIT,
  CCLASS_مٿانهون,
  CCLASS_هيٺاهون
} char_class;

#وصف_ڪريو SETCHARSET_OK     1
#وصف_ڪريو SETCHARSET_FAIL   0

ساكن صحيح parsekeyword(ثابت بنانشان حرف **نمونو, بنانشان حرف *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  حرف keyword[10] = { 0 };
  size_t i;
  ثابت بنانشان حرف *p = *نمونو;
  bool مليو = FALSE;
  لكل(i = 0; !مليو; i++) {
    حرف c = (حرف)*p++;
    إذا(i >= حجم(keyword))
      أرجع SETCHARSET_FAIL;
    بدل(state) {
    حالة CURLFNM_PKW_INIT:
      إذا(ISLOWER(c))
        keyword[i] = c;
      وإلا إذا(c == ':')
        state = CURLFNM_PKW_DDOT;
      وإلا
        أرجع SETCHARSET_FAIL;
      اكسر;
    حالة CURLFNM_PKW_DDOT:
      إذا(c == ']')
        مليو = TRUE;
      وإلا
        أرجع SETCHARSET_FAIL;
    }
  }
#وصف_رد KEYLEN

  *نمونو = p; /* move caller's pattern pointer */
  إذا(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  وإلا إذا(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  وإلا إذا(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  وإلا إذا(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  وإلا إذا(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  وإلا إذا(!strcmp(keyword, "graph"))
    charset[CURLFNM_گراف] = 1;
  وإلا إذا(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  وإلا إذا(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  وإلا إذا(!strcmp(keyword, "upper"))
    charset[CURLFNM_مٿانهون] = 1;
  وإلا إذا(!strcmp(keyword, "lower"))
    charset[CURLFNM_هيٺاهون] = 1;
  وإلا
    أرجع SETCHARSET_FAIL;
  أرجع SETCHARSET_OK;
}

/* Return the character class. */
ساكن char_class charclass(بنانشان حرف c)
{
  إذا(ISUPPER(c))
    أرجع CCLASS_مٿانهون;
  إذا(ISLOWER(c))
    أرجع CCLASS_هيٺاهون;
  إذا(ISDIGIT(c))
    أرجع CCLASS_DIGIT;
  أرجع CCLASS_OTHER;
}

/* Include a character or a range in set. */
ساكن فراغ setcharorrange(ثابت بنانشان حرف **pp, بنانشان حرف *charset)
{
  ثابت بنانشان حرف *p = (*pp)++;
  بنانشان حرف c = *p++;

  charset[c] = 1;
  إذا(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    بنانشان حرف endrange = *p++;

    إذا(endrange == '\\')
      endrange = *p++;
    إذا(endrange >= c && charclass(endrange) == cc) {
      طالما(c++ != endrange)
        إذا(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
ساكن صحيح setcharset(ثابت بنانشان حرف **p, بنانشان حرف *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_مليو = FALSE;
  بنانشان حرف c;

  memset(charset, 0, CURLFNM_CHSET_سائيز);
  لكل(;;) {
    c = **p;
    إذا(!c)
      أرجع SETCHARSET_FAIL;

    بدل(state) {
    حالة CURLFNM_SCHS_DEFAULT:
      إذا(c == ']') {
        إذا(something_مليو)
          أرجع SETCHARSET_OK;
        something_مليو = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      وإلا إذا(c == '[') {
        ثابت بنانشان حرف *pp = *p + 1;

        إذا(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        وإلا {
          charset[c] = 1;
          (*p)++;
        }
        something_مليو = TRUE;
      }
      وإلا إذا(c == '^' || c == '!') {
        إذا(!something_مليو) {
          إذا(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_مليو = TRUE;
          }
          وإلا
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        وإلا
          charset[c] = 1;
        (*p)++;
      }
      وإلا إذا(c == '\\') {
        c = *(++(*p));
        إذا(c)
          setcharorrange(p, charset);
        وإلا
          charset['\\'] = 1;
        something_مليو = TRUE;
      }
      وإلا {
        setcharorrange(p, charset);
        something_مليو = TRUE;
      }
      اكسر;
    حالة CURLFNM_SCHS_RIGHTBR:
      إذا(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      وإلا إذا(c == ']') {
        أرجع SETCHARSET_OK;
      }
      وإلا إذا(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      وإلا
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        اذهب fail;
      اكسر;
    حالة CURLFNM_SCHS_RIGHTBRLEFTBR:
      إذا(c == ']')
        أرجع SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      اكسر;
    }
  }
fail:
  أرجع SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
ساكن صحيح matchtoken(ثابت بنانشان حرف **pp, ثابت بنانشان حرف **sp)
{
  ثابت بنانشان حرف *p = *pp;
  ثابت بنانشان حرف *s = *sp;

  بدل(*p) {
  حالة '?':
    اكسر;
  حالة '\\':
    إذا(p[1])
      p++;
    إذا(*s != *p)
      أرجع 0;
    اكسر;
  حالة '[': {
    بنانشان حرف charset[CURLFNM_CHSET_سائيز];
    ثابت بنانشان حرف *pp2 = p + 1; /* Copy in case of syntax error. */
    bool مليو = FALSE;
    إذا(!setcharset(&pp2, charset))
      أرجع -1; /* Syntax error in set; mismatch! */
    إذا(charset[(بنانشان صحيح)*s])
      مليو = TRUE;
    وإلا إذا(charset[CURLFNM_ALNUM])
      مليو = ISALNUM(*s);
    وإلا إذا(charset[CURLFNM_ALPHA])
      مليو = ISALPHA(*s);
    وإلا إذا(charset[CURLFNM_DIGIT])
      مليو = ISDIGIT(*s);
    وإلا إذا(charset[CURLFNM_XDIGIT])
      مليو = ISXDIGIT(*s);
    وإلا إذا(charset[CURLFNM_PRINT])
      مليو = ISPRINT(*s);
    وإلا إذا(charset[CURLFNM_SPACE])
      مليو = ISBLANK(*s);
    وإلا إذا(charset[CURLFNM_مٿانهون])
      مليو = ISUPPER(*s);
    وإلا إذا(charset[CURLFNM_هيٺاهون])
      مليو = ISLOWER(*s);
    وإلا إذا(charset[CURLFNM_BLANK])
      مليو = ISBLANK(*s);
    وإلا إذا(charset[CURLFNM_گراف])
      مليو = ISGRAPH(*s);

    إذا(charset[CURLFNM_NEGATE])
      مليو = !مليو;

    إذا(!مليو)
      أرجع 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    أرجع 1;
  }
  افتراضي:
    إذا(*p != *s)
      أرجع 0;
    اكسر;
  }
  *pp = p + 1;
  *sp = s + 1;
  أرجع 1;
}

/* greedy match with backtracking to the most recent '*' */
ساكن صحيح loop(ثابت بنانشان حرف *نمونو, ثابت بنانشان حرف *string)
{
  ثابت بنانشان حرف *p = نمونو;
  ثابت بنانشان حرف *s = string;
  ثابت بنانشان حرف *تارو_p = عدم;
  ثابت بنانشان حرف *تارو_s = عدم;
  صحيح maxstars = 2;

  طالما(*s) {
    إذا(*p == '*') {
      إذا(!maxstars)
        أرجع CURL_FNMATCH_NOMATCH;
      maxstars--;
      لكل(;;) {
        p++;
        إذا(!*p)
          أرجع CURL_FNMATCH_ميلاپ;
        إذا(*p == '?') {
          إذا(!*s)
            أرجع CURL_FNMATCH_NOMATCH;
          s++;
        }
        وإلا إذا(*p != '*')
          اكسر;
      }
      تارو_p = p;
      تارو_s = s;
    }
    وإلا إذا(*p) {
      ثابت بنانشان حرف *np = p;
      ثابت بنانشان حرف *ns = s;
      صحيح r = matchtoken(&np, &ns);
      إذا(r < 0)
        أرجع CURL_FNMATCH_NOMATCH;
      إذا(r) {
        p = np;
        s = ns;
        استمر;
      }
      إذا(!تارو_p)
        أرجع CURL_FNMATCH_NOMATCH;
      p = تارو_p;
      s = ++تارو_s;
    }
    وإلا إذا(تارو_p) {
      p = تارو_p;
      s = ++تارو_s;
    }
    وإلا
      أرجع CURL_FNMATCH_NOMATCH;
  }
  طالما(*p == '*') {
    إذا(!maxstars)
      أرجع CURL_FNMATCH_NOMATCH;
    maxstars--;
    افعل {
      p++;
      إذا(*p == '?')
        أرجع CURL_FNMATCH_NOMATCH;
    } طالما(*p == '*');
  }
  أرجع *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_ميلاپ;
}

/*
 * @unittest: 1307
 */
صحيح Curl_fnmatch(فراغ *ptr, ثابت حرف *نمونو, ثابت حرف *string)
{
  (فراغ)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  إذا(!نمونو || !string) {
    أرجع CURL_FNMATCH_FAIL;
  }
  أرجع loop((ثابت بنانشان حرف *)نمونو,
              (ثابت بنانشان حرف *)string);
}
#وإلا /* HAVE_FNMATCH */

#ضمن <fnmatch.h>
/*
 * @unittest: 1307
 */
صحيح Curl_fnmatch(فراغ *ptr, ثابت حرف *نمونو, ثابت حرف *string)
{
  (فراغ)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  إذا(!نمونو || !string) {
    أرجع CURL_FNMATCH_FAIL;
  }

  بدل(fnmatch(نمونو, string, 0)) {
  حالة 0:
    أرجع CURL_FNMATCH_ميلاپ;
  حالة FNM_NOMATCH:
    أرجع CURL_FNMATCH_NOMATCH;
  افتراضي:
    أرجع CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#جيڪڏهن_پورو /* !HAVE_FNMATCH */

#جيڪڏهن_پورو /* !CURL_DISABLE_FTP */
