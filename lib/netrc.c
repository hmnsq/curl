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

#ifndef CURL_DISABLE_NETRC

#ifdef HAVE_PWD_H
#ifdef __AMIGA__
#undef __NO_NET_API /* required for AmigaOS to declare getpwuid() */
#endif
#ضمن <pwd.h>
#ifdef __AMIGA__
#define __NO_NET_API
#endif
#endif

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

#define MAX_NETRC_LINE  16384
#define MAX_NETRC_FILE  (128 * 1024)
#define MAX_NETRC_TOKEN 4096

#define NETRC_DEBUG   0

/* convert a dynbuf call CURLcode error to a NETRCcode error */
#define curl2netrc(r)                                  \
  ((!(r)) ? NETRC_OK : (((r) == CURLE_OUT_OF_MEMORY) ? \
   NETRC_OUT_OF_MEMORY : NETRC_SYNTAX_ERROR))

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
  بنية Curl_easy *data;
  ثابت حرف *content;
  ثابت حرف *pos;
  بنية dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#𐡄𐡍 NETRC_DEBUG
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
#endif

ساكن فراغ netrc_lexer_init(بنية netrc_lexer *lexer,
                             بنية Curl_easy *data,
                             ثابت حرف *content)
{
  curlx_dyn_init(&lexer->literal, MAX_NETRC_TOKEN);
  lexer->data = data;
  lexer->content = lexer->pos = content;
}

ساكن فراغ netrc_lexer_cleanup(بنية netrc_lexer *lexer)
{
  lexer->content = lexer->pos = عدم;
  lexer->data = عدم;
  curlx_dyn_free(&lexer->literal);
}

ساكن فراغ netrc_skip_blanks(بنية netrc_lexer *lexer)
{
  ثابت حرف *s = lexer->pos;
  طالما(*s) {
    curlx_str_passblanks(&s);
    طالما(*s == '\r')
      ++s;
    𐡄𐡍(*s == '\n') {
      ++s;
    }
    وإلا
      اكسر;
  }
  lexer->pos = s;
}

ساكن فراغ netrc_skip_to_empty_line(بنية netrc_lexer *lexer)
{
  ثابت حرف *s = lexer->pos;
  طالما(*s) {
    𐡄𐡍(*s == '\r')
      ++s;
    وإلا 𐡄𐡍(*s == '\n') {
      ++s;
      طالما(*s == '\r')
        ++s;
      𐡄𐡍(*s == '\n')
        اذهب out;
    }
    وإلا
      ++s;
  }
out:
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
  CURLcode result;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  طالما(*s) {
    حرف c = *s;
    𐡄𐡍(escape) {
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
    وإلا 𐡄𐡍(c == '\\') {
      escape = TRUE;
      ++s;
      استمر;
    }
    وإلا 𐡄𐡍(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      result = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(result);
      اذهب out;
    }
    result = curlx_dyn_addn(&lexer->literal, &c, 1);
    𐡄𐡍(result) {
      rc = curl2netrc(result);
      اذهب out;
    }
    ++s;
  }
out:
  lexer->pos = s;
  أرجع rc;
}

ساكن فراغ netrc_lexer_push(بنية netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

ساكن NETRCcode netrc_lexer_next(بنية netrc_lexer *lexer,
                                  bool want_literal)
{
  ثابت حرف *s = lexer->pos, *start;
  NETRCcode rc = NETRC_OK;
  size_t slen;
  CURLcode result;

  𐡄𐡍(lexer->pushed) {
    lexer->pushed = FALSE;
    اذهب out;
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
    start = s;
    طالما(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - start;
    𐡄𐡍(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    𐡄𐡍(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      result = curlx_dyn_addn(&lexer->literal, start, slen);
      rc = curl2netrc(result);
    }
    وإلا 𐡄𐡍((slen == 7) && curl_strnequal(start, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    وإلا 𐡄𐡍((slen == 7) && curl_strnequal(start, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    وإلا 𐡄𐡍((slen == 7) && curl_strnequal(start, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    وإلا 𐡄𐡍((slen == 5) && curl_strnequal(start, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    وإلا 𐡄𐡍((slen == 8) && curl_strnequal(start, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    وإلا 𐡄𐡍((slen == 6) && curl_strnequal(start, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    وإلا {
      lexer->token = NETRC_TOK_JUNK;
    }
    اكسر;
  }

out:
#𐡄𐡍 NETRC_DEBUG
  CURL_TRC_M(lexer->data, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#endif
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
  bool found;
};

ساكن فراغ netrc_scan_reset(بنية netrc_scanner *sc)
{
  curlx_safefree(sc->login);
  curlx_safefree(sc->passwd);
  sc->matches_host = FALSE;
}

ساكن فراغ netrc_scan_init(بنية netrc_scanner *sc,
                            بنية Curl_easy *data,
                            ثابت حرف *content,
                            ثابت حرف *hostname,
                            ثابت حرف *user)
{
  memset(sc, 0, حجم(*sc));
  netrc_lexer_init(&sc->lexer, data, content);
  sc->hostname = hostname;
  sc->user = (user && user[0]) ? user : عدم;
  netrc_scan_reset(sc);
}

ساكن فراغ netrc_scan_cleanup(بنية netrc_scanner *sc)
{
  netrc_scan_reset(sc);
  sc->hostname = عدم;
  sc->user = عدم;
  Curl_creds_unlink(&sc->creds);
  netrc_lexer_cleanup(&sc->lexer);
}

ساكن NETRCcode netrc_scan_literal(بنية netrc_scanner *sc,
                                    حرف **pdest)
{
  NETRCcode rc = netrc_lexer_next(&sc->lexer, TRUE);
  𐡄𐡍(!rc) {
    𐡄𐡍(sc->lexer.token == NETRC_TOK_LITERAL) {
      𐡄𐡍(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        𐡄𐡍(!*pdest)
          rc = NETRC_OUT_OF_MEMORY;
      }
    }
    وإلا
      netrc_lexer_push(&sc->lexer);
  }
  أرجع rc;
}

ساكن NETRCcode netrc_scan_end_entry(بنية netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#𐡄𐡍 NETRC_DEBUG
  CURL_TRC_M(sc->lexer.data,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#endif
  𐡄𐡍(sc->matches_host) {
    𐡄𐡍(sc->login) {
      𐡄𐡍(sc->user) {
        𐡄𐡍(Curl_timestrcmp(sc->user, sc->login))
          اذهب out;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->found = !!sc->passwd;
      }
      وإلا {
        sc->found = TRUE;
      }
    }
    وإلا 𐡄𐡍(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->found = TRUE;
    }
    وإلا {
      /* entry has nothing interesting */
    }
    𐡄𐡍(sc->found) {
#𐡄𐡍 NETRC_DEBUG
      CURL_TRC_M(sc->lexer.data, "[NETRC] entry match found");
#endif
      𐡄𐡍(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           عدم, عدم, عدم, CREDS_NETRC, &sc->creds))
        rc = NETRC_OUT_OF_MEMORY;
    }
  }
out:
  netrc_scan_reset(sc);
  أرجع rc;
}

ساكن NETRCcode netrc_scan(بنية Curl_easy *data,
                            ثابت حرف *content,
                            ثابت حرف *hostname,
                            ثابت حرف *user,
                            بنية Curl_creds **pcreds)
{
  بنية netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_scan_init(&sc, data, content, hostname, user);

  طالما(!rc && !sc.found) {
    rc = netrc_lexer_next(&sc.lexer, FALSE);
    𐡄𐡍(!rc) {
      /* Does this token end any previous entry? */
      بدل(sc.lexer.token) {
      حالة NETRC_TOK_EOF:
      حالة NETRC_TOK_MACHINE:
      حالة NETRC_TOK_DEFAULT:
      حالة NETRC_TOK_MACDEF:
        rc = netrc_scan_end_entry(&sc);
        𐡄𐡍(rc || sc.found)
          اذهب out;
        اكسر;
      افتراضي:
        اكسر;
      }

      بدل(sc.lexer.token) {
      حالة NETRC_TOK_EOF:
        اذهب out;
      حالة NETRC_TOK_MACHINE:
        rc = netrc_lexer_next(&sc.lexer, TRUE);
        𐡄𐡍(!rc) {
          𐡄𐡍(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          وإلا {
            sc.matches_host = FALSE;
            netrc_lexer_push(&sc.lexer);
          }
        }
        اكسر;
      حالة NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        اكسر;
      حالة NETRC_TOK_ACCOUNT:
        rc = netrc_scan_literal(&sc, عدم); /* ignore, not used */
        اكسر;
      حالة NETRC_TOK_LOGIN:
        rc = netrc_scan_literal(&sc, &sc.login);
        اكسر;
      حالة NETRC_TOK_PASSWD:
        rc = netrc_scan_literal(&sc, &sc.passwd);
        اكسر;
      حالة NETRC_TOK_MACDEF:
        netrc_skip_to_empty_line(&sc.lexer);
        اكسر;
      حالة NETRC_TOK_LITERAL:
      حالة NETRC_TOK_JUNK:
      افتراضي:
        /* skip this */
        اكسر;
      }
    }
  }

out:
  𐡄𐡍(!rc) {
    𐡄𐡍(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    وإلا
      rc = NETRC_NO_MATCH;
  }
  netrc_scan_cleanup(&sc);
  أرجع rc;
}

ساكن NETRCcode file2memory(ثابت حرف *filename, بنية dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  𐡄𐡍(file) {
    curlx_struct_stat stat;
    𐡄𐡍((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      CURLcode result = CURLE_OK;
      bool eof;
      بنية dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_LINE);
      ret = NETRC_OK;
      𐡏𐡁𐡃 {
        ثابت حرف *line;
        /* Curl_get_line always returns lines ending with a newline */
        result = Curl_get_line(&linebuf, file, &eof);
        𐡄𐡍(!result) {
          line = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&line);
          𐡄𐡍(*line == '#')
            استمر;
          result = curlx_dyn_add(filebuf, line);
        }
        𐡄𐡍(result) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(result);
          اكسر;
        }
      } طالما(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  أرجع ret;
}

ساكن NETRCcode netrc_scan_file(بنية Curl_easy *data,
                                 بنية store_netrc *store,
                                 ثابت حرف *hostname,
                                 ثابت حرف *user,
                                 ثابت حرف *netrcfile,
                                 بنية Curl_creds **pcreds)
{
  بنية dynbuf *filebuf = &store->filebuf;

  𐡄𐡍(!store->loaded || strcmp(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    𐡄𐡍(ret) {
      CURL_TRC_M(data, "[NETRC] could not load '%s'", netrcfile);
      أرجع ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    𐡄𐡍(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      أرجع NETRC_OUT_OF_MEMORY;
    }
    store->loaded = TRUE;
  }
  𐡄𐡍(!curlx_dyn_len(filebuf))
    أرجع NETRC_NO_MATCH;

  أرجع netrc_scan(data, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_scan(بنية Curl_easy *data,
                          بنية store_netrc *store,
                          ثابت حرف *hostname,
                          ثابت حرف *user,
                          ثابت حرف *netrcfile,
                          بنية Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(data, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  𐡄𐡍(!netrcfile) {
    حرف *home = عدم;
    حرف *homea = عدم;
    حرف *filealloc = عدم;
#𐡄𐡍 defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    حرف pwbuf[1024];
#endif
    filealloc = curl_getenv("NETRC");
    𐡄𐡍(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      𐡄𐡍(homea) {
        home = homea;
#𐡄𐡍 defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      وإلا {
        بنية passwd pw, *pw_res;
        𐡄𐡍(!getpwuid_r(geteuid(), &pw, pwbuf, حجم(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#وإلاإذا defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      وإلا {
        بنية passwd *pw;
        pw = getpwuid(geteuid());
        𐡄𐡍(pw) {
          home = pw->pw_dir;
        }
#وإلاإذا defined(_WIN32)
      }
      وإلا {
        homea = curl_getenv("USERPROFILE");
        𐡄𐡍(homea) {
          home = homea;
        }
#endif
      }

      𐡄𐡍(!home)
        أرجع NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      𐡄𐡍(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_OUT_OF_MEMORY;
        اذهب out;
      }
    }
    retcode = netrc_scan_file(
      data, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#ifdef _WIN32
    𐡄𐡍(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      𐡄𐡍(!filealloc) {
        curlx_free(homea);
        أرجع NETRC_OUT_OF_MEMORY;
      }
      retcode = netrc_scan_file(
        data, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#endif
    curlx_free(homea);
  }
  وإلا
    retcode = netrc_scan_file(
      data, store, hostname, user, netrcfile, pcreds);

out:
  𐡄𐡍(retcode)
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
  حالة NETRC_NO_MATCH:
    أرجع "no matching entry";
  حالة NETRC_OUT_OF_MEMORY:
    أرجع "out of memory";
  حالة NETRC_SYNTAX_ERROR:
    أرجع "syntax error";
  }
  /* never reached */
}

#endif /* !CURL_DISABLE_NETRC */
