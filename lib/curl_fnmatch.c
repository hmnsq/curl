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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP

#అన్తర్భావ "curl_fnmatch.h"

#నిర్వచితం_కాకపోతే HAVE_FNMATCH

#నిర్వచించు CURLFNM_CHARSET_LEN (పరిమాణం(అక్షరం) * 256)
#నిర్వచించు CURLFNM_CHSET_కొలత (CURLFNM_CHARSET_LEN + 15)

#నిర్వచించు CURLFNM_NEGATE  CURLFNM_CHARSET_LEN

#నిర్వచించు CURLFNM_ALNUM   (CURLFNM_CHARSET_LEN + 1)
#నిర్వచించు CURLFNM_DIGIT   (CURLFNM_CHARSET_LEN + 2)
#నిర్వచించు CURLFNM_XDIGIT  (CURLFNM_CHARSET_LEN + 3)
#నిర్వచించు CURLFNM_ALPHA   (CURLFNM_CHARSET_LEN + 4)
#నిర్వచించు CURLFNM_PRINT   (CURLFNM_CHARSET_LEN + 5)
#నిర్వచించు CURLFNM_BLANK   (CURLFNM_CHARSET_LEN + 6)
#నిర్వచించు CURLFNM_క్రింది   (CURLFNM_CHARSET_LEN + 7)
#నిర్వచించు CURLFNM_లేఖాచిత్రం   (CURLFNM_CHARSET_LEN + 8)
#నిర్వచించు CURLFNM_SPACE   (CURLFNM_CHARSET_LEN + 9)
#నిర్వచించు CURLFNM_పై   (CURLFNM_CHARSET_LEN + 10)

రకనిర్వచనం గణన {
  CURLFNM_SCHS_DEFAULT = 0,
  CURLFNM_SCHS_RIGHTBR,
  CURLFNM_SCHS_RIGHTBRLEFTBR
} setcharset_state;

రకనిర్వచనం గణన {
  CURLFNM_PKW_INIT = 0,
  CURLFNM_PKW_DDOT
} parsekey_state;

రకనిర్వచనం గణన {
  CCLASS_OTHER = 0,
  CCLASS_DIGIT,
  CCLASS_పై,
  CCLASS_క్రింది
} char_class;

#నిర్వచించు SETCHARSET_OK     1
#నిర్వచించు SETCHARSET_FAIL   0

స్థిర పూర్ణాంకం parsekeyword(స్థిరరాశి గుర్తులేని అక్షరం **నమూనా, గుర్తులేని అక్షరం *charset)
{
  parsekey_state state = CURLFNM_PKW_INIT;
  అక్షరం keyword[10] = { 0 };
  size_t i;
  స్థిరరాశి గుర్తులేని అక్షరం *p = *నమూనా;
  bool దొరికింది = FALSE;
  కొరకు(i = 0; !దొరికింది; i++) {
    అక్షరం c = (అక్షరం)*p++;
    ఒకవేళ(i >= పరిమాణం(keyword))
      తిరిగి SETCHARSET_FAIL;
    ఎంపిక(state) {
    స్థితి CURLFNM_PKW_INIT:
      ఒకవేళ(ISLOWER(c))
        keyword[i] = c;
      లేకపోతే ఒకవేళ(c == ':')
        state = CURLFNM_PKW_DDOT;
      లేకపోతే
        తిరిగి SETCHARSET_FAIL;
      ఆపు;
    స్థితి CURLFNM_PKW_DDOT:
      ఒకవేళ(c == ']')
        దొరికింది = TRUE;
      లేకపోతే
        తిరిగి SETCHARSET_FAIL;
    }
  }
#నిర్వచనం_రద్దు KEYLEN

  *నమూనా = p; /* move caller's pattern pointer */
  ఒకవేళ(!strcmp(keyword, "digit"))
    charset[CURLFNM_DIGIT] = 1;
  లేకపోతే ఒకవేళ(!strcmp(keyword, "alnum"))
    charset[CURLFNM_ALNUM] = 1;
  లేకపోతే ఒకవేళ(!strcmp(keyword, "alpha"))
    charset[CURLFNM_ALPHA] = 1;
  లేకపోతే ఒకవేళ(!strcmp(keyword, "xdigit"))
    charset[CURLFNM_XDIGIT] = 1;
  లేకపోతే ఒకవేళ(!strcmp(keyword, "print"))
    charset[CURLFNM_PRINT] = 1;
  లేకపోతే ఒకవేళ(!strcmp(keyword, "graph"))
    charset[CURLFNM_లేఖాచిత్రం] = 1;
  లేకపోతే ఒకవేళ(!strcmp(keyword, "space"))
    charset[CURLFNM_SPACE] = 1;
  లేకపోతే ఒకవేళ(!strcmp(keyword, "blank"))
    charset[CURLFNM_BLANK] = 1;
  లేకపోతే ఒకవేళ(!strcmp(keyword, "upper"))
    charset[CURLFNM_పై] = 1;
  లేకపోతే ఒకవేళ(!strcmp(keyword, "lower"))
    charset[CURLFNM_క్రింది] = 1;
  లేకపోతే
    తిరిగి SETCHARSET_FAIL;
  తిరిగి SETCHARSET_OK;
}

/* Return the character class. */
స్థిర char_class charclass(గుర్తులేని అక్షరం c)
{
  ఒకవేళ(ISUPPER(c))
    తిరిగి CCLASS_పై;
  ఒకవేళ(ISLOWER(c))
    తిరిగి CCLASS_క్రింది;
  ఒకవేళ(ISDIGIT(c))
    తిరిగి CCLASS_DIGIT;
  తిరిగి CCLASS_OTHER;
}

/* Include a character or a range in set. */
స్థిర శూన్య setcharorrange(స్థిరరాశి గుర్తులేని అక్షరం **pp, గుర్తులేని అక్షరం *charset)
{
  స్థిరరాశి గుర్తులేని అక్షరం *p = (*pp)++;
  గుర్తులేని అక్షరం c = *p++;

  charset[c] = 1;
  ఒకవేళ(ISALNUM(c) && *p++ == '-') {
    char_class cc = charclass(c);
    గుర్తులేని అక్షరం endrange = *p++;

    ఒకవేళ(endrange == '\\')
      endrange = *p++;
    ఒకవేళ(endrange >= c && charclass(endrange) == cc) {
      అయితే(c++ != endrange)
        ఒకవేళ(charclass(c) == cc)  /* Chars in class may be not consecutive. */
          charset[c] = 1;
      *pp = p;
    }
  }
}

/* returns 1 (TRUE) if pattern is OK, 0 if is bad ("p" is pattern pointer) */
స్థిర పూర్ణాంకం setcharset(స్థిరరాశి గుర్తులేని అక్షరం **p, గుర్తులేని అక్షరం *charset)
{
  setcharset_state state = CURLFNM_SCHS_DEFAULT;
  bool something_దొరికింది = FALSE;
  గుర్తులేని అక్షరం c;

  memset(charset, 0, CURLFNM_CHSET_కొలత);
  కొరకు(;;) {
    c = **p;
    ఒకవేళ(!c)
      తిరిగి SETCHARSET_FAIL;

    ఎంపిక(state) {
    స్థితి CURLFNM_SCHS_DEFAULT:
      ఒకవేళ(c == ']') {
        ఒకవేళ(something_దొరికింది)
          తిరిగి SETCHARSET_OK;
        something_దొరికింది = TRUE;
        state = CURLFNM_SCHS_RIGHTBR;
        charset[c] = 1;
        (*p)++;
      }
      లేకపోతే ఒకవేళ(c == '[') {
        స్థిరరాశి గుర్తులేని అక్షరం *pp = *p + 1;

        ఒకవేళ(*pp++ == ':' && parsekeyword(&pp, charset))
          *p = pp;
        లేకపోతే {
          charset[c] = 1;
          (*p)++;
        }
        something_దొరికింది = TRUE;
      }
      లేకపోతే ఒకవేళ(c == '^' || c == '!') {
        ఒకవేళ(!something_దొరికింది) {
          ఒకవేళ(charset[CURLFNM_NEGATE]) {
            charset[c] = 1;
            something_దొరికింది = TRUE;
          }
          లేకపోతే
            charset[CURLFNM_NEGATE] = 1; /* negate charset */
        }
        లేకపోతే
          charset[c] = 1;
        (*p)++;
      }
      లేకపోతే ఒకవేళ(c == '\\') {
        c = *(++(*p));
        ఒకవేళ(c)
          setcharorrange(p, charset);
        లేకపోతే
          charset['\\'] = 1;
        something_దొరికింది = TRUE;
      }
      లేకపోతే {
        setcharorrange(p, charset);
        something_దొరికింది = TRUE;
      }
      ఆపు;
    స్థితి CURLFNM_SCHS_RIGHTBR:
      ఒకవేళ(c == '[') {
        state = CURLFNM_SCHS_RIGHTBRLEFTBR;
        charset[c] = 1;
        (*p)++;
      }
      లేకపోతే ఒకవేళ(c == ']') {
        తిరిగి SETCHARSET_OK;
      }
      లేకపోతే ఒకవేళ(ISPRINT(c)) {
        charset[c] = 1;
        (*p)++;
        state = CURLFNM_SCHS_DEFAULT;
      }
      లేకపోతే
        /* used 'goto fail' instead of 'return SETCHARSET_FAIL' to avoid a
         * nonsense warning 'statement not reached' at end of the fnc when
         * compiling on Solaris */
        వెళ్ళు fail;
      ఆపు;
    స్థితి CURLFNM_SCHS_RIGHTBRLEFTBR:
      ఒకవేళ(c == ']')
        తిరిగి SETCHARSET_OK;
      state = CURLFNM_SCHS_DEFAULT;
      charset[c] = 1;
      (*p)++;
      ఆపు;
    }
  }
fail:
  తిరిగి SETCHARSET_FAIL;
}

/* match one pattern token against one string character, advancing both */
స్థిర పూర్ణాంకం matchtoken(స్థిరరాశి గుర్తులేని అక్షరం **pp, స్థిరరాశి గుర్తులేని అక్షరం **sp)
{
  స్థిరరాశి గుర్తులేని అక్షరం *p = *pp;
  స్థిరరాశి గుర్తులేని అక్షరం *s = *sp;

  ఎంపిక(*p) {
  స్థితి '?':
    ఆపు;
  స్థితి '\\':
    ఒకవేళ(p[1])
      p++;
    ఒకవేళ(*s != *p)
      తిరిగి 0;
    ఆపు;
  స్థితి '[': {
    గుర్తులేని అక్షరం charset[CURLFNM_CHSET_కొలత];
    స్థిరరాశి గుర్తులేని అక్షరం *pp2 = p + 1; /* Copy in case of syntax error. */
    bool దొరికింది = FALSE;
    ఒకవేళ(!setcharset(&pp2, charset))
      తిరిగి -1; /* Syntax error in set; mismatch! */
    ఒకవేళ(charset[(గుర్తులేని పూర్ణాంకం)*s])
      దొరికింది = TRUE;
    లేకపోతే ఒకవేళ(charset[CURLFNM_ALNUM])
      దొరికింది = ISALNUM(*s);
    లేకపోతే ఒకవేళ(charset[CURLFNM_ALPHA])
      దొరికింది = ISALPHA(*s);
    లేకపోతే ఒకవేళ(charset[CURLFNM_DIGIT])
      దొరికింది = ISDIGIT(*s);
    లేకపోతే ఒకవేళ(charset[CURLFNM_XDIGIT])
      దొరికింది = ISXDIGIT(*s);
    లేకపోతే ఒకవేళ(charset[CURLFNM_PRINT])
      దొరికింది = ISPRINT(*s);
    లేకపోతే ఒకవేళ(charset[CURLFNM_SPACE])
      దొరికింది = ISBLANK(*s);
    లేకపోతే ఒకవేళ(charset[CURLFNM_పై])
      దొరికింది = ISUPPER(*s);
    లేకపోతే ఒకవేళ(charset[CURLFNM_క్రింది])
      దొరికింది = ISLOWER(*s);
    లేకపోతే ఒకవేళ(charset[CURLFNM_BLANK])
      దొరికింది = ISBLANK(*s);
    లేకపోతే ఒకవేళ(charset[CURLFNM_లేఖాచిత్రం])
      దొరికింది = ISGRAPH(*s);

    ఒకవేళ(charset[CURLFNM_NEGATE])
      దొరికింది = !దొరికింది;

    ఒకవేళ(!దొరికింది)
      తిరిగి 0;
    *pp = pp2 + 1;
    *sp = s + 1;
    తిరిగి 1;
  }
  అప్రమేయ:
    ఒకవేళ(*p != *s)
      తిరిగి 0;
    ఆపు;
  }
  *pp = p + 1;
  *sp = s + 1;
  తిరిగి 1;
}

/* greedy match with backtracking to the most recent '*' */
స్థిర పూర్ణాంకం loop(స్థిరరాశి గుర్తులేని అక్షరం *నమూనా, స్థిరరాశి గుర్తులేని అక్షరం *string)
{
  స్థిరరాశి గుర్తులేని అక్షరం *p = నమూనా;
  స్థిరరాశి గుర్తులేని అక్షరం *s = string;
  స్థిరరాశి గుర్తులేని అక్షరం *నక్షత్రం_p = ఖాళీ;
  స్థిరరాశి గుర్తులేని అక్షరం *నక్షత్రం_s = ఖాళీ;
  పూర్ణాంకం maxstars = 2;

  అయితే(*s) {
    ఒకవేళ(*p == '*') {
      ఒకవేళ(!maxstars)
        తిరిగి CURL_FNMATCH_NOMATCH;
      maxstars--;
      కొరకు(;;) {
        p++;
        ఒకవేళ(!*p)
          తిరిగి CURL_FNMATCH_సరిపోలిక;
        ఒకవేళ(*p == '?') {
          ఒకవేళ(!*s)
            తిరిగి CURL_FNMATCH_NOMATCH;
          s++;
        }
        లేకపోతే ఒకవేళ(*p != '*')
          ఆపు;
      }
      నక్షత్రం_p = p;
      నక్షత్రం_s = s;
    }
    లేకపోతే ఒకవేళ(*p) {
      స్థిరరాశి గుర్తులేని అక్షరం *np = p;
      స్థిరరాశి గుర్తులేని అక్షరం *ns = s;
      పూర్ణాంకం r = matchtoken(&np, &ns);
      ఒకవేళ(r < 0)
        తిరిగి CURL_FNMATCH_NOMATCH;
      ఒకవేళ(r) {
        p = np;
        s = ns;
        కొనసాగు;
      }
      ఒకవేళ(!నక్షత్రం_p)
        తిరిగి CURL_FNMATCH_NOMATCH;
      p = నక్షత్రం_p;
      s = ++నక్షత్రం_s;
    }
    లేకపోతే ఒకవేళ(నక్షత్రం_p) {
      p = నక్షత్రం_p;
      s = ++నక్షత్రం_s;
    }
    లేకపోతే
      తిరిగి CURL_FNMATCH_NOMATCH;
  }
  అయితే(*p == '*') {
    ఒకవేళ(!maxstars)
      తిరిగి CURL_FNMATCH_NOMATCH;
    maxstars--;
    చేయి {
      p++;
      ఒకవేళ(*p == '?')
        తిరిగి CURL_FNMATCH_NOMATCH;
    } అయితే(*p == '*');
  }
  తిరిగి *p ? CURL_FNMATCH_NOMATCH : CURL_FNMATCH_సరిపోలిక;
}

/*
 * @unittest: 1307
 */
పూర్ణాంకం Curl_fnmatch(శూన్య *ptr, స్థిరరాశి అక్షరం *నమూనా, స్థిరరాశి అక్షరం *string)
{
  (శూన్య)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ఒకవేళ(!నమూనా || !string) {
    తిరిగి CURL_FNMATCH_FAIL;
  }
  తిరిగి loop((స్థిరరాశి గుర్తులేని అక్షరం *)నమూనా,
              (స్థిరరాశి గుర్తులేని అక్షరం *)string);
}
#లేకపోతే /* HAVE_FNMATCH */

#అన్తర్భావ <fnmatch.h>
/*
 * @unittest: 1307
 */
పూర్ణాంకం Curl_fnmatch(శూన్య *ptr, స్థిరరాశి అక్షరం *నమూనా, స్థిరరాశి అక్షరం *string)
{
  (శూన్య)ptr; /* the argument is specified by the curl_fnmatch_callback
                prototype, but not used by Curl_fnmatch() */
  ఒకవేళ(!నమూనా || !string) {
    తిరిగి CURL_FNMATCH_FAIL;
  }

  ఎంపిక(fnmatch(నమూనా, string, 0)) {
  స్థితి 0:
    తిరిగి CURL_FNMATCH_సరిపోలిక;
  స్థితి FNM_NOMATCH:
    తిరిగి CURL_FNMATCH_NOMATCH;
  అప్రమేయ:
    తిరిగి CURL_FNMATCH_FAIL;
  }
  /* not reached */
}
#అయితే_ముగింపు /* !HAVE_FNMATCH */

#అయితే_ముగింపు /* !CURL_DISABLE_FTP */
