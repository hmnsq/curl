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
#شامل "curl_setup.h"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP

#شامل "curl_fnmatch.h"

#اگر_غیر_تعریف_شدہ HAVE_FNMATCH

#تعریف_کرو CURLFNM_CHARSET_LEN (حجم(حرف) * 256)
#تعریف_کرو CURLFNM_CHSET_سائز (CURLFNM_CHARSET_LEN + 15)

#تعریف_کرو CURLFNM_NEGATE  CURLFNM_CHARSET_LEN

#تعریف_کرو CURLFNM_ALNUM   (CURLFNM_CHARSET_LEN + 1)
#تعریف_کرو CURLFNM_DIGIT   (CURLFNM_CHARSET_LEN + 2)
#تعریف_کرو CURLFNM_XDIGIT  (CURLFNM_CHARSET_LEN + 3)
#تعریف_کرو CURLFNM_ALPHA   (CURLFNM_CHARSET_LEN + 4)
#تعریف_کرو CURLFNM_PRINT   (CURLFNM_CHARSET_LEN + 5)
#تعریف_کرو CURLFNM_BLANK   (CURLFNM_CHARSET_LEN + 6)
#تعریف_کرو CURLFNM_زیریں   (CURLFNM_CHARSET_LEN + 7)
#تعریف_کرو CURLFNM_گراف   (CURLFNM_CHARSET_LEN + 8)
#تعریف_کرو CURLFNM_SPACE   (CURLFNM_CHARSET_LEN + 9)
#تعریف_کرو CURLFNM_بالائی   (CURLFNM_CHARSET_LEN + 10)

قسمتعریف شمار {
  CURLFNM_SCHS_DEFAULT = 0,
  CURLFNM_SCHS_RIGHTBR,
  CURLFNM_SCHS_RIGHTBRLEFTBR
} setcharset_state;

قسمتعریف شمار {
  CURLFNM_PKW_INIT = 0,
  CURLFNM_PKW_DDOT
} parsekey_state;

قسمتعریف شمار {
  CCLASS_OTHER = 0,
  CCLASS_DIGIT,
  CCLASS_بالائی,
  CCLASS_زیریں
} char_class;

#تعریف_کرو SETCHARSET_OK     1
#تعریف_کرو SETCHARSET_FAIL   0

ساکن صحیح parsekeyword(ثابت غیرعلامتی حرف **نمونہ, غیرعلامتی حرف *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  حرف keyword[10] = { 0 };
  size_t i;
  ثابت غیرعلامتی حرف *p = *نمونہ;
  bool ملا = FALSE;
  برائے(i = 0; !ملا; i++) {
    حرف c = (حرف)*p++;
    اگر(i >= حجم(keyword))
      واپس SETCHARSET_FAIL;
    انتخاب(state) {
    حالت CURLFNM_PKW_INIT:
      اگر(ISLOWER(c))
        keyword[i] = c;
      ورنہ اگر(c == ':')
        state = CURLFNM_PKW_DDOT;
      ورنہ
        واپس SETCHARSET_FAIL;
      توڑو;
    حالت CURLFNM_PKW_DDOT:
      اگر(c == ']')
        ملا = TRUE;
      ورنہ
        واپس SETCHARSET_FAIL;
    }
  }
#تعریف_منسوخ KEYLEN

  *نمونہ = p; /* move caller's pattern pointer */
  اگر(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  ورنہ اگر(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  ورنہ اگر(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  ورنہ اگر(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  ورنہ اگر(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  ورنہ اگر(!strcmp(keyword, "graph"))
    charset[CURLFNM_گراف] = 1;
  ورنہ اگر(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  ورنہ اگر(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  ورنہ اگر(!strcmp(keyword, "upper"))
    charset[CURLFNM_بالائی] = 1;
  ورنہ اگر(!strcmp(keyword, "lower"))
    charset[CURLFNM_زیریں] = 1;
  ورنہ
    واپس SETCHARSET_FAIL;
  واپس SETCHARSET_OK;
}

/* Return the character class. */
ساکن char_class charclass(غیرعلامتی حرف c)
{
  اگر(ISUPPER(c))
    واپس CCLASS_بالائی;
  اگر(ISLOWER(c))
    واپس CCLASS_زیریں;
  اگر(ISDIGIT(c))
    واپس CCLASS_DIGIT;
  واپس CCLASS_OTHER;
}

/* Include a character or a range in set. */
ساکن خالی setcharorrange(ثابت غیرعلامتی حرف **pp, غیرعلامتی حرف *charset)
{
  ثابت غیرعلامتی حرف *p = (*pp)++;
  غیرعلامتی حرف c = *p++;

  charset[c] = 1;
  اگر(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    غیرعلامتی حرف endrange = *p++;

    اگر(endrange == '\\')
      endrange = *p++;
    اگر(endrange >= c && charclass(endrange) == cc) {
      جب(c++ != endrange)
        اگر(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
ساکن صحیح setcharset(ثابت غیرعلامتی حرف **p, غیرعلامتی حرف *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_ملا = FALSE;
  غیرعلامتی حرف c;

  memset(charset, 0, CURLFNM_CHSET_سائز);
  برائے(;;) {
    c = **p;
    اگر(!c)
      واپس SETCHARSET_FAIL;

    انتخاب(state) {
    حالت CURLFNM_SCHS_DEFAULT:
      اگر(c == ']') {
        اگر(something_ملا)
          واپس SETCHARSET_OK;
        something_ملا = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      ورنہ اگر(c == '[') {
        ثابت غیرعلامتی حرف *pp = *p + 1;

        اگر(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        ورنہ {
          charset[c] = 1;
          (*p)++;
        }
        something_ملا = TRUE;
      }
      ورنہ اگر(c == '^' || c == '!') {
        اگر(!something_ملا) {
          اگر(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_ملا = TRUE;
          }
          ورنہ
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        ورنہ
          charset[c] = 1;
        (*p)++;
      }
      ورنہ اگر(c == '\\') {
        c = *(++(*p));
        اگر(c)
          setcharorrange(p, charset);
        ورنہ
          charset['\\'] = 1;
        something_ملا = TRUE;
      }
      ورنہ {
        setcharorrange(p, charset);
        something_ملا = TRUE;
      }
      توڑو;
    حالت CURLFNM_SCHS_RIGHTBR:
      اگر(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      ورنہ اگر(c == ']') {
        واپس SETCHARSET_OK;
      }
      ورنہ اگر(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      ورنہ
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        جاؤ fail;
      توڑو;
    حالت CURLFNM_SCHS_RIGHTBRLEFTBR:
      اگر(c == ']')
        واپس SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      توڑو;
    }
  }
fail:
  واپس SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
ساکن صحیح matchtoken(ثابت غیرعلامتی حرف **pp, ثابت غیرعلامتی حرف **sp)
{
  ثابت غیرعلامتی حرف *p = *pp;
  ثابت غیرعلامتی حرف *s = *sp;

  انتخاب(*p) {
  حالت '?':
    توڑو;
  حالت '\\':
    اگر(p[1])
      p++;
    اگر(*s != *p)
      واپس 0;
    توڑو;
  حالت '[': {
    غیرعلامتی حرف charset[CURLFNM_CHSET_سائز];
    ثابت غیرعلامتی حرف *pp2 = p + 1; /* Copy in case of syntax error. */
    bool ملا = FALSE;
    اگر(!setcharset(&pp2, charset))
      واپس -1; /* Syntax error in set; mismatch! */
    اگر(charset[(غیرعلامتی صحیح)*s])
      ملا = TRUE;
    ورنہ اگر(charset[CURLFNM_ALNUM])
      ملا = ISALNUM(*s);
    ورنہ اگر(charset[CURLFNM_ALPHA])
      ملا = ISALPHA(*s);
    ورنہ اگر(charset[CURLFNM_DIGIT])
      ملا = ISDIGIT(*s);
    ورنہ اگر(charset[CURLFNM_XDIGIT])
      ملا = ISXDIGIT(*s);
    ورنہ اگر(charset[CURLFNM_PRINT])
      ملا = ISPRINT(*s);
    ورنہ اگر(charset[CURLFNM_SPACE])
      ملا = ISBLANK(*s);
    ورنہ اگر(charset[CURLFNM_بالائی])
      ملا = ISUPPER(*s);
    ورنہ اگر(charset[CURLFNM_زیریں])
      ملا = ISLOWER(*s);
    ورنہ اگر(charset[CURLFNM_BLANK])
      ملا = ISBLANK(*s);
    ورنہ اگر(charset[CURLFNM_گراف])
      ملا = ISGRAPH(*s);

    اگر(charset[CURLFNM_NEGATE])
      ملا = !ملا;

    اگر(!ملا)
      واپس 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    واپس 1;
  }
  طےشدہ:
    اگر(*p != *s)
      واپس 0;
    توڑو;
  }
  *pp = p + 1;
  *sp = s + 1;
  واپس 1;
}

/* greedy match with backtracking to the most recent '*' */
ساکن صحیح loop(ثابت غیرعلامتی حرف *نمونہ, ثابت غیرعلامتی حرف *string)
{
  ثابت غیرعلامتی حرف *p = نمونہ;
  ثابت غیرعلامتی حرف *s = string;
  ثابت غیرعلامتی حرف *ستارہ_p = تہی;
  ثابت غیرعلامتی حرف *ستارہ_s = تہی;
  صحیح maxstars = 2;

  جب(*s) {
    اگر(*p == '*') {
      اگر(!maxstars)
        واپس CURL_FNMATCH_NOMATCH;
      maxstars--;
      برائے(;;) {
        p++;
        اگر(!*p)
          واپس CURL_FNMATCH_مطابقت;
        اگر(*p == '?') {
          اگر(!*s)
            واپس CURL_FNMATCH_NOMATCH;
          s++;
        }
        ورنہ اگر(*p != '*')
          توڑو;
      }
      ستارہ_p = p;
      ستارہ_s = s;
    }
    ورنہ اگر(*p) {
      ثابت غیرعلامتی حرف *np = p;
      ثابت غیرعلامتی حرف *ns = s;
      صحیح r = matchtoken(&np, &ns);
      اگر(r < 0)
        واپس CURL_FNMATCH_NOMATCH;
      اگر(r) {
        p = np;
        s = ns;
        جاری;
      }
      اگر(!ستارہ_p)
        واپس CURL_FNMATCH_NOMATCH;
      p = ستارہ_p;
      s = ++ستارہ_s;
    }
    ورنہ اگر(ستارہ_p) {
      p = ستارہ_p;
      s = ++ستارہ_s;
    }
    ورنہ
      واپس CURL_FNMATCH_NOMATCH;
  }
  جب(*p == '*') {
    اگر(!maxstars)
      واپس CURL_FNMATCH_NOMATCH;
    maxstars--;
    کرو {
      p++;
      اگر(*p == '?')
        واپس CURL_FNMATCH_NOMATCH;
    } جب(*p == '*');
  }
  واپس *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_مطابقت;
}

/*
 * @unittest: 1307
 */
صحیح Curl_fnmatch(خالی *ptr, ثابت حرف *نمونہ, ثابت حرف *string)
{
  (خالی)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  اگر(!نمونہ || !string) {
    واپس CURL_FNMATCH_FAIL;
  }
  واپس loop((ثابت غیرعلامتی حرف *)نمونہ,
              (ثابت غیرعلامتی حرف *)string);
}
#ورنہ /* HAVE_FNMATCH */

#شامل <fnmatch.h>
/*
 * @unittest: 1307
 */
صحیح Curl_fnmatch(خالی *ptr, ثابت حرف *نمونہ, ثابت حرف *string)
{
  (خالی)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  اگر(!نمونہ || !string) {
    واپس CURL_FNMATCH_FAIL;
  }

  انتخاب(fnmatch(نمونہ, string, 0)) {
  حالت 0:
    واپس CURL_FNMATCH_مطابقت;
  حالت FNM_NOMATCH:
    واپس CURL_FNMATCH_NOMATCH;
  طےشدہ:
    واپس CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#اگر_ختم /* !HAVE_FNMATCH */

#اگر_ختم /* !CURL_DISABLE_FTP */
