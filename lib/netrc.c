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

#که_نه_تعریف_شوی CURL_DISABLE_NETRC

#که_تعریف_شوی HAVE_PWD_H
#که_تعریف_شوی __AMIGA__
#تعریف_لغوه __NO_NET_API /* required for AmigaOS to declare getpwuid() */
#د_که_پای
#ضمن <pwd.h>
#که_تعریف_شوی __AMIGA__
#تعریف_کړه __NO_NET_API
#د_که_پای
#د_که_پای

#ضمن "netrc.h"
#ضمن "urldata.h"
#ضمن "creds.h"
#ضمن "curl_trc.h"
#ضمن "strcase.h"
#ضمن "curl_get_line.h"
#ضمن "curlx/win32-fopen.h"
#ضمن "curlx/strparse.h"


/* .netrc is not really a standard. The GNU definition can be found here:
 * https://www.gnu.org/software/inetutils/manual/html_node/The-_002enetrc-file.html
 * This gives grammar like:
 *
 * LITERAL := \S+ | QUOTED
 * QUOTED  := "(\\[rnt\]|[^"])*"
 * ANYTHING := .
 * EMPTY_LINE := \r*\n\r*\n
 * MACHINE := machine         # case-insensitive
 * LOGIN   := login           # case-insensitive
 * PASSWD  := password        # case-insensitive
 * ACCOUNT := account         # case-insensitive
 * MACDEF  := macdef          # case-insensitive
 * DEFAULT := default         # case-insensitive
 *
 * MACRO   := MACDEF ANYTHING* EMPTY_LINE
 * JUNK    := LITERAL
 * LKEY     := ( LOGIN | PASSWD | ACCOUNT ) LITERAL
 * MENTRY   := MACHINE LITERAL LKEY*
 * DENTRY   := DEFAULT LKEY*
 * NETRC   := (MENTRY | DENTRY | MACRO | JUNK )* EOF
 *
 * Tokens are separated by whitespace or newlines. which have otherwise
 * no special meaning, apart from the empty line ending a MACRO.
 *
 * Parsing is not strict, unmatched LITERALs are ignored
 */

#تعریف_کړه MAX_NETRC_کرښه  16384
#تعریف_کړه MAX_NETRC_FILE  (128 * 1024)
#تعریف_کړه MAX_NETRC_TOKEN 4096

#تعریف_کړه NETRC_DEBUG   0

/* convert a dynbuf call CURLcode error to a NETRCcode error */
#تعریف_کړه curl2netrc(r)                                  \
  ((!(r)) ? NETRC_OK : (((r) == CURLE_بهر_د_حافظه) ? \
   NETRC_بهر_د_حافظه : NETRC_SYNTAX_ERROR))

تعريفنوع تعداد {
  NETRC_TOK_EOF,
  NETRC_TOK_LITERAL,
  NETRC_TOK_MACHINE,
  NETRC_TOK_DEFAULT,
  NETRC_TOK_ACCOUNT,
  NETRC_TOK_LOGIN,
  NETRC_TOK_PASSWD,
  NETRC_TOK_MACDEF,
  NETRC_TOK_JUNK
} curl_netrc_token;

بنية netrc_lexer {
  بنية Curl_easy *معلومات;
  ثابت حرف *content;
  ثابت حرف *pos;
  بنية dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#إذا NETRC_DEBUG
ساكن ثابت حرف *netrc_tokenstr(curl_netrc_token token)
{
  بدل(token) {
  حالة NETRC_TOK_EOF:
    أرجع "[EOF]";
  حالة NETRC_TOK_LITERAL:
    أرجع "[LITERAL]";
  حالة NETRC_TOK_MACHINE:
    أرجع "[MACHINE]";
  حالة NETRC_TOK_DEFAULT:
    أرجع "[DEFAULT]";
  حالة NETRC_TOK_ACCOUNT:
    أرجع "[ACCOUNT]";
  حالة NETRC_TOK_LOGIN:
    أرجع "[LOGIN]";
  حالة NETRC_TOK_PASSWD:
    أرجع "[PASSWORD]";
  حالة NETRC_TOK_MACDEF:
    أرجع "[MACDEF]";
  حالة NETRC_TOK_JUNK:
    أرجع "[JUNK]";
  افتراضي:
    أرجع "[???]";
  }
}
#د_که_پای

ساكن فراغ netrc_lexer_init(بنية netrc_lexer *lexer,
                             بنية Curl_easy *معلومات,
                             ثابت حرف *content)
{
  curlx_dyn_init(&lexer->literal, MAX_NETRC_TOKEN);
  lexer->معلومات = معلومات;
  lexer->content = lexer->pos = content;
}

ساكن فراغ netrc_lexer_cleanup(بنية netrc_lexer *lexer)
{
  lexer->content = lexer->pos = عدم;
  lexer->معلومات = عدم;
  curlx_dyn_free(&lexer->literal);
}

ساكن فراغ netrc_skip_blanks(بنية netrc_lexer *lexer)
{
  ثابت حرف *s = lexer->pos;
  طالما(*s) {
    curlx_str_passblanks(&s);
    طالما(*s == '\r')
      ++s;
    إذا(*s == '\n') {
      ++s;
    }
    وإلا
      اكسر;
  }
  lexer->pos = s;
}

ساكن فراغ netrc_skip_منزل_تشه_کرښه(بنية netrc_lexer *lexer)
{
  ثابت حرف *s = lexer->pos;
  طالما(*s) {
    إذا(*s == '\r')
      ++s;
    وإلا إذا(*s == '\n') {
      ++s;
      طالما(*s == '\r')
        ++s;
      إذا(*s == '\n')
        اذهب بهر;
    }
    وإلا
      ++s;
  }
بهر:
  lexer->pos = s;
}

/*
 * Parse a quoted token starting after the opening '"'. Handles \n, \r, \t
 * escape sequences. Advances *tok_endp past the closing '"'.
 *
 * Returns NETRC_OK or error.
 */
ساكن NETRCcode netrc_lexer_quoted(بنية netrc_lexer *lexer)
{
  NETRCcode rc = NETRC_SYNTAX_ERROR;
  ثابت حرف *s = lexer->pos;
  bool escape = FALSE;
  CURLcode پایله;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  طالما(*s) {
    حرف c = *s;
    إذا(escape) {
      escape = FALSE;
      بدل(c) {
      حالة 'n':
        c = '\n';
        اكسر;
      حالة 'r':
        c = '\r';
        اكسر;
      حالة 't':
        c = '\t';
        اكسر;
      }
    }
    وإلا إذا(c == '\\') {
      escape = TRUE;
      ++s;
      استمر;
    }
    وإلا إذا(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      پایله = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(پایله);
      اذهب بهر;
    }
    پایله = curlx_dyn_addn(&lexer->literal, &c, 1);
    إذا(پایله) {
      rc = curl2netrc(پایله);
      اذهب بهر;
    }
    ++s;
  }
بهر:
  lexer->pos = s;
  أرجع rc;
}

ساكن فراغ netrc_lexer_ټېل(بنية netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

ساكن NETRCcode netrc_lexer_بل(بنية netrc_lexer *lexer,
                                  bool want_literal)
{
  ثابت حرف *s = lexer->pos, *پیل;
  NETRCcode rc = NETRC_OK;
  size_t slen;
  CURLcode پایله;

  إذا(lexer->pushed) {
    lexer->pushed = FALSE;
    اذهب بهر;
  }

  curlx_dyn_reset(&lexer->literal);
  netrc_skip_blanks(lexer);
  s = lexer->pos;

  بدل(*s) {
  حالة 0:
    lexer->token = NETRC_TOK_EOF;
    اكسر;
  حالة '\"':
    rc = netrc_lexer_quoted(lexer);
    lexer->token = NETRC_TOK_LITERAL;
    s = lexer->pos;
    اكسر;
  افتراضي:
    /* unquoted token */
    پیل = s;
    طالما(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - پیل;
    إذا(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    إذا(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      پایله = curlx_dyn_addn(&lexer->literal, پیل, slen);
      rc = curl2netrc(پایله);
    }
    وإلا إذا((slen == 7) && curl_strnequal(پیل, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    وإلا إذا((slen == 7) && curl_strnequal(پیل, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    وإلا إذا((slen == 7) && curl_strnequal(پیل, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    وإلا إذا((slen == 5) && curl_strnequal(پیل, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    وإلا إذا((slen == 8) && curl_strnequal(پیل, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    وإلا إذا((slen == 6) && curl_strnequal(پیل, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    وإلا {
      lexer->token = NETRC_TOK_JUNK;
    }
    اكسر;
  }

بهر:
#إذا NETRC_DEBUG
  CURL_TRC_M(lexer->معلومات, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#د_که_پای
  lexer->pos = s;
  أرجع rc;
}

بنية netrc_scanner {
  بنية netrc_lexer lexer;
  ثابت حرف *hostname; /* non-NULL, machine to scan for */
  ثابت حرف *user; /* maybe NULL, login to scan for */
  حرف *login;
  حرف *passwd;
  بنية Curl_creds *creds;
  bool matches_host;
  bool وموندل;
};

ساكن فراغ netrc_لټون_reset(بنية netrc_scanner *sc)
{
  curlx_safefree(sc->login);
  curlx_safefree(sc->passwd);
  sc->matches_host = FALSE;
}

ساكن فراغ netrc_لټون_init(بنية netrc_scanner *sc,
                            بنية Curl_easy *معلومات,
                            ثابت حرف *content,
                            ثابت حرف *hostname,
                            ثابت حرف *user)
{
  memset(sc, 0, حجم(*sc));
  netrc_lexer_init(&sc->lexer, معلومات, content);
  sc->hostname = hostname;
  sc->user = (user && user[0]) ? user : عدم;
  netrc_لټون_reset(sc);
}

ساكن فراغ netrc_لټون_cleanup(بنية netrc_scanner *sc)
{
  netrc_لټون_reset(sc);
  sc->hostname = عدم;
  sc->user = عدم;
  Curl_creds_unlink(&sc->creds);
  netrc_lexer_cleanup(&sc->lexer);
}

ساكن NETRCcode netrc_لټون_literal(بنية netrc_scanner *sc,
                                    حرف **pdest)
{
  NETRCcode rc = netrc_lexer_بل(&sc->lexer, TRUE);
  إذا(!rc) {
    إذا(sc->lexer.token == NETRC_TOK_LITERAL) {
      إذا(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        إذا(!*pdest)
          rc = NETRC_بهر_د_حافظه;
      }
    }
    وإلا
      netrc_lexer_ټېل(&sc->lexer);
  }
  أرجع rc;
}

ساكن NETRCcode netrc_لټون_پای_ټکی_ثبت(بنية netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#إذا NETRC_DEBUG
  CURL_TRC_M(sc->lexer.معلومات,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#د_که_پای
  إذا(sc->matches_host) {
    إذا(sc->login) {
      إذا(sc->user) {
        إذا(Curl_timestrcmp(sc->user, sc->login))
          اذهب بهر;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->وموندل = !!sc->passwd;
      }
      وإلا {
        sc->وموندل = TRUE;
      }
    }
    وإلا إذا(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->وموندل = TRUE;
    }
    وإلا {
      /* entry has nothing interesting */
    }
    إذا(sc->وموندل) {
#إذا NETRC_DEBUG
      CURL_TRC_M(sc->lexer.معلومات, "[NETRC] entry match found");
#د_که_پای
      إذا(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           عدم, عدم, عدم, CREDS_NETRC, &sc->creds))
        rc = NETRC_بهر_د_حافظه;
    }
  }
بهر:
  netrc_لټون_reset(sc);
  أرجع rc;
}

ساكن NETRCcode netrc_لټون(بنية Curl_easy *معلومات,
                            ثابت حرف *content,
                            ثابت حرف *hostname,
                            ثابت حرف *user,
                            بنية Curl_creds **pcreds)
{
  بنية netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_لټون_init(&sc, معلومات, content, hostname, user);

  طالما(!rc && !sc.وموندل) {
    rc = netrc_lexer_بل(&sc.lexer, FALSE);
    إذا(!rc) {
      /* Does this token end any previous entry? */
      بدل(sc.lexer.token) {
      حالة NETRC_TOK_EOF:
      حالة NETRC_TOK_MACHINE:
      حالة NETRC_TOK_DEFAULT:
      حالة NETRC_TOK_MACDEF:
        rc = netrc_لټون_پای_ټکی_ثبت(&sc);
        إذا(rc || sc.وموندل)
          اذهب بهر;
        اكسر;
      افتراضي:
        اكسر;
      }

      بدل(sc.lexer.token) {
      حالة NETRC_TOK_EOF:
        اذهب بهر;
      حالة NETRC_TOK_MACHINE:
        rc = netrc_lexer_بل(&sc.lexer, TRUE);
        إذا(!rc) {
          إذا(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          وإلا {
            sc.matches_host = FALSE;
            netrc_lexer_ټېل(&sc.lexer);
          }
        }
        اكسر;
      حالة NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        اكسر;
      حالة NETRC_TOK_ACCOUNT:
        rc = netrc_لټون_literal(&sc, عدم); /* ignore, not used */
        اكسر;
      حالة NETRC_TOK_LOGIN:
        rc = netrc_لټون_literal(&sc, &sc.login);
        اكسر;
      حالة NETRC_TOK_PASSWD:
        rc = netrc_لټون_literal(&sc, &sc.passwd);
        اكسر;
      حالة NETRC_TOK_MACDEF:
        netrc_skip_منزل_تشه_کرښه(&sc.lexer);
        اكسر;
      حالة NETRC_TOK_LITERAL:
      حالة NETRC_TOK_JUNK:
      افتراضي:
        /* skip this */
        اكسر;
      }
    }
  }

بهر:
  إذا(!rc) {
    إذا(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    وإلا
      rc = NETRC_NO_سمون;
  }
  netrc_لټون_cleanup(&sc);
  أرجع rc;
}

ساكن NETRCcode file2memory(ثابت حرف *filename, بنية dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  إذا(file) {
    curlx_struct_stat stat;
    إذا((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_ډېرپېښ)) {
      CURLcode پایله = CURLE_OK;
      bool eof;
      بنية dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_کرښه);
      ret = NETRC_OK;
      افعل {
        ثابت حرف *کرښه;
        /* Curl_get_line always returns lines ending with a newline */
        پایله = Curl_get_کرښه(&linebuf, file, &eof);
        إذا(!پایله) {
          کرښه = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&کرښه);
          إذا(*کرښه == '#')
            استمر;
          پایله = curlx_dyn_جمع_کړه(filebuf, کرښه);
        }
        إذا(پایله) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(پایله);
          اكسر;
        }
      } طالما(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  أرجع ret;
}

ساكن NETRCcode netrc_لټون_file(بنية Curl_easy *معلومات,
                                 بنية store_netrc *store,
                                 ثابت حرف *hostname,
                                 ثابت حرف *user,
                                 ثابت حرف *netrcfile,
                                 بنية Curl_creds **pcreds)
{
  بنية dynbuf *filebuf = &store->filebuf;

  إذا(!store->loaded || strcmp(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    إذا(ret) {
      CURL_TRC_M(معلومات, "[NETRC] could not load '%s'", netrcfile);
      أرجع ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    إذا(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      أرجع NETRC_بهر_د_حافظه;
    }
    store->loaded = TRUE;
  }
  إذا(!curlx_dyn_len(filebuf))
    أرجع NETRC_NO_سمون;

  أرجع netrc_لټون(معلومات, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_لټون(بنية Curl_easy *معلومات,
                          بنية store_netrc *store,
                          ثابت حرف *hostname,
                          ثابت حرف *user,
                          ثابت حرف *netrcfile,
                          بنية Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(معلومات, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  إذا(!netrcfile) {
    حرف *home = عدم;
    حرف *homea = عدم;
    حرف *filealloc = عدم;
#إذا defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    حرف pwbuf[1024];
#د_که_پای
    filealloc = curl_getenv("NETRC");
    إذا(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      إذا(homea) {
        home = homea;
#إذا defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      وإلا {
        بنية passwd pw, *pw_res;
        إذا(!getpwuid_r(geteuid(), &pw, pwbuf, حجم(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#وإلاإذا defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      وإلا {
        بنية passwd *pw;
        pw = getpwuid(geteuid());
        إذا(pw) {
          home = pw->pw_dir;
        }
#وإلاإذا defined(_WIN32)
      }
      وإلا {
        homea = curl_getenv("USERPROFILE");
        إذا(homea) {
          home = homea;
        }
#د_که_پای
      }

      إذا(!home)
        أرجع NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      إذا(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_بهر_د_حافظه;
        اذهب بهر;
      }
    }
    retcode = netrc_لټون_file(
      معلومات, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#که_تعریف_شوی _WIN32
    إذا(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      إذا(!filealloc) {
        curlx_free(homea);
        أرجع NETRC_بهر_د_حافظه;
      }
      retcode = netrc_لټون_file(
        معلومات, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#د_که_پای
    curlx_free(homea);
  }
  وإلا
    retcode = netrc_لټون_file(
      معلومات, store, hostname, user, netrcfile, pcreds);

بهر:
  إذا(retcode)
    Curl_creds_unlink(pcreds);
  أرجع retcode;
}

فراغ Curl_netrc_init(بنية store_netrc *store)
{
  curlx_dyn_init(&store->filebuf, MAX_NETRC_FILE);
  store->loaded = FALSE;
  store->filename = عدم;
}
فراغ Curl_netrc_cleanup(بنية store_netrc *store)
{
  curlx_dyn_free(&store->filebuf);
  curlx_safefree(store->filename);
  store->loaded = FALSE;
}

ثابت حرف *Curl_netrc_strerror(NETRCcode ret)
{
  بدل(ret) {
  افتراضي:
    أرجع ""; /* not a legit error */
  حالة NETRC_FILE_MISSING:
    أرجع "no such file";
  حالة NETRC_NO_سمون:
    أرجع "no matching entry";
  حالة NETRC_بهر_د_حافظه:
    أرجع "out of memory";
  حالة NETRC_SYNTAX_ERROR:
    أرجع "syntax error";
  }
  /* never reached */
}

#د_که_پای /* !CURL_DISABLE_NETRC */
