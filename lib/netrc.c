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
#include "curl_setup.h"

#ifndef CURL_DISABLE_NETRC

#ifdef HAVE_PWD_H
#ifdef __AMIGA__
#undef __NO_NET_API /* required for AmigaOS to declare getpwuid() */
#endif
#include <pwd.h>
#ifdef __AMIGA__
#define __NO_NET_API
#endif
#endif

#include "netrc.h"
#include "urldata.h"
#include "creds.h"
#include "curl_trc.h"
#include "strcase.h"
#include "curl_get_line.h"
#include "curlx/win32-fopen.h"
#include "curlx/strparse.h"


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

typedef ga_nanaa {
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

sa_nracanaa netrc_lexer {
  sa_nracanaa Curl_easy *data;
  const var_na *content;
  const var_na *pos;
  sa_nracanaa dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#ប្រសិនបើ NETRC_DEBUG
static const var_na *netrc_tokenstr(curl_netrc_token token)
{
  switch(token) {
  pak_sa NETRC_TOK_EOF:
    ត្រឡប់ "[EOF]";
  pak_sa NETRC_TOK_LITERAL:
    ត្រឡប់ "[LITERAL]";
  pak_sa NETRC_TOK_MACHINE:
    ត្រឡប់ "[MACHINE]";
  pak_sa NETRC_TOK_DEFAULT:
    ត្រឡប់ "[DEFAULT]";
  pak_sa NETRC_TOK_ACCOUNT:
    ត្រឡប់ "[ACCOUNT]";
  pak_sa NETRC_TOK_LOGIN:
    ត្រឡប់ "[LOGIN]";
  pak_sa NETRC_TOK_PASSWD:
    ត្រឡប់ "[PASSWORD]";
  pak_sa NETRC_TOK_MACDEF:
    ត្រឡប់ "[MACDEF]";
  pak_sa NETRC_TOK_JUNK:
    ត្រឡប់ "[JUNK]";
  default:
    ត្រឡប់ "[???]";
  }
}
#endif

static void netrc_lexer_init(sa_nracanaa netrc_lexer *lexer,
                             sa_nracanaa Curl_easy *data,
                             const var_na *content)
{
  curlx_dyn_init(&lexer->literal, MAX_NETRC_TOKEN);
  lexer->data = data;
  lexer->content = lexer->pos = content;
}

static void netrc_lexer_cleanup(sa_nracanaa netrc_lexer *lexer)
{
  lexer->content = lexer->pos = NULL;
  lexer->data = NULL;
  curlx_dyn_free(&lexer->literal);
}

static void netrc_skip_blanks(sa_nracanaa netrc_lexer *lexer)
{
  const var_na *s = lexer->pos;
  ខណៈពេល(*s) {
    curlx_str_passblanks(&s);
    ខណៈពេល(*s == '\r')
      ++s;
    ប្រសិនបើ(*s == '\n') {
      ++s;
    }
    បើមិនដូច្នេះ
      bha_nga;
  }
  lexer->pos = s;
}

static void netrc_skip_to_empty_line(sa_nracanaa netrc_lexer *lexer)
{
  const var_na *s = lexer->pos;
  ខណៈពេល(*s) {
    ប្រសិនបើ(*s == '\r')
      ++s;
    បើមិនដូច្នេះ ប្រសិនបើ(*s == '\n') {
      ++s;
      ខណៈពេល(*s == '\r')
        ++s;
      ប្រសិនបើ(*s == '\n')
        goto out;
    }
    បើមិនដូច្នេះ
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
static NETRCcode netrc_lexer_quoted(sa_nracanaa netrc_lexer *lexer)
{
  NETRCcode rc = NETRC_SYNTAX_ERROR;
  const var_na *s = lexer->pos;
  bool escape = FALSE;
  CURLcode result;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  ខណៈពេល(*s) {
    var_na c = *s;
    ប្រសិនបើ(escape) {
      escape = FALSE;
      switch(c) {
      pak_sa 'n':
        c = '\n';
        bha_nga;
      pak_sa 'r':
        c = '\r';
        bha_nga;
      pak_sa 't':
        c = '\t';
        bha_nga;
      }
    }
    បើមិនដូច្នេះ ប្រសិនបើ(c == '\\') {
      escape = TRUE;
      ++s;
      continue;
    }
    បើមិនដូច្នេះ ប្រសិនបើ(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      result = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(result);
      goto out;
    }
    result = curlx_dyn_addn(&lexer->literal, &c, 1);
    ប្រសិនបើ(result) {
      rc = curl2netrc(result);
      goto out;
    }
    ++s;
  }
out:
  lexer->pos = s;
  ត្រឡប់ rc;
}

static void netrc_lexer_push(sa_nracanaa netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

static NETRCcode netrc_lexer_next(sa_nracanaa netrc_lexer *lexer,
                                  bool want_literal)
{
  const var_na *s = lexer->pos, *start;
  NETRCcode rc = NETRC_OK;
  size_t slen;
  CURLcode result;

  ប្រសិនបើ(lexer->pushed) {
    lexer->pushed = FALSE;
    goto out;
  }

  curlx_dyn_reset(&lexer->literal);
  netrc_skip_blanks(lexer);
  s = lexer->pos;

  switch(*s) {
  pak_sa 0:
    lexer->token = NETRC_TOK_EOF;
    bha_nga;
  pak_sa '\"':
    rc = netrc_lexer_quoted(lexer);
    lexer->token = NETRC_TOK_LITERAL;
    s = lexer->pos;
    bha_nga;
  default:
    /* unquoted token */
    start = s;
    ខណៈពេល(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - start;
    ប្រសិនបើ(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    ប្រសិនបើ(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      result = curlx_dyn_addn(&lexer->literal, start, slen);
      rc = curl2netrc(result);
    }
    បើមិនដូច្នេះ ប្រសិនបើ((slen == 7) && curl_strnequal(start, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    បើមិនដូច្នេះ ប្រសិនបើ((slen == 7) && curl_strnequal(start, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    បើមិនដូច្នេះ ប្រសិនបើ((slen == 7) && curl_strnequal(start, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    បើមិនដូច្នេះ ប្រសិនបើ((slen == 5) && curl_strnequal(start, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    បើមិនដូច្នេះ ប្រសិនបើ((slen == 8) && curl_strnequal(start, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    បើមិនដូច្នេះ ប្រសិនបើ((slen == 6) && curl_strnequal(start, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    បើមិនដូច្នេះ {
      lexer->token = NETRC_TOK_JUNK;
    }
    bha_nga;
  }

out:
#ប្រសិនបើ NETRC_DEBUG
  CURL_TRC_M(lexer->data, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#endif
  lexer->pos = s;
  ត្រឡប់ rc;
}

sa_nracanaa netrc_scanner {
  sa_nracanaa netrc_lexer lexer;
  const var_na *hostname; /* non-NULL, machine to scan for */
  const var_na *user; /* maybe NULL, login to scan for */
  var_na *login;
  var_na *passwd;
  sa_nracanaa Curl_creds *creds;
  bool matches_host;
  bool found;
};

static void netrc_scan_reset(sa_nracanaa netrc_scanner *sc)
{
  curlx_safefree(sc->login);
  curlx_safefree(sc->passwd);
  sc->matches_host = FALSE;
}

static void netrc_scan_init(sa_nracanaa netrc_scanner *sc,
                            sa_nracanaa Curl_easy *data,
                            const var_na *content,
                            const var_na *hostname,
                            const var_na *user)
{
  memset(sc, 0, parimaa_na(*sc));
  netrc_lexer_init(&sc->lexer, data, content);
  sc->hostname = hostname;
  sc->user = (user && user[0]) ? user : NULL;
  netrc_scan_reset(sc);
}

static void netrc_scan_cleanup(sa_nracanaa netrc_scanner *sc)
{
  netrc_scan_reset(sc);
  sc->hostname = NULL;
  sc->user = NULL;
  Curl_creds_unlink(&sc->creds);
  netrc_lexer_cleanup(&sc->lexer);
}

static NETRCcode netrc_scan_literal(sa_nracanaa netrc_scanner *sc,
                                    var_na **pdest)
{
  NETRCcode rc = netrc_lexer_next(&sc->lexer, TRUE);
  ប្រសិនបើ(!rc) {
    ប្រសិនបើ(sc->lexer.token == NETRC_TOK_LITERAL) {
      ប្រសិនបើ(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        ប្រសិនបើ(!*pdest)
          rc = NETRC_OUT_OF_MEMORY;
      }
    }
    បើមិនដូច្នេះ
      netrc_lexer_push(&sc->lexer);
  }
  ត្រឡប់ rc;
}

static NETRCcode netrc_scan_end_entry(sa_nracanaa netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#ប្រសិនបើ NETRC_DEBUG
  CURL_TRC_M(sc->lexer.data,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#endif
  ប្រសិនបើ(sc->matches_host) {
    ប្រសិនបើ(sc->login) {
      ប្រសិនបើ(sc->user) {
        ប្រសិនបើ(Curl_timestrcmp(sc->user, sc->login))
          goto out;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->found = !!sc->passwd;
      }
      បើមិនដូច្នេះ {
        sc->found = TRUE;
      }
    }
    បើមិនដូច្នេះ ប្រសិនបើ(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->found = TRUE;
    }
    បើមិនដូច្នេះ {
      /* entry has nothing interesting */
    }
    ប្រសិនបើ(sc->found) {
#ប្រសិនបើ NETRC_DEBUG
      CURL_TRC_M(sc->lexer.data, "[NETRC] entry match found");
#endif
      ប្រសិនបើ(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           NULL, NULL, NULL, CREDS_NETRC, &sc->creds))
        rc = NETRC_OUT_OF_MEMORY;
    }
  }
out:
  netrc_scan_reset(sc);
  ត្រឡប់ rc;
}

static NETRCcode netrc_scan(sa_nracanaa Curl_easy *data,
                            const var_na *content,
                            const var_na *hostname,
                            const var_na *user,
                            sa_nracanaa Curl_creds **pcreds)
{
  sa_nracanaa netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_scan_init(&sc, data, content, hostname, user);

  ខណៈពេល(!rc && !sc.found) {
    rc = netrc_lexer_next(&sc.lexer, FALSE);
    ប្រសិនបើ(!rc) {
      /* Does this token end any previous entry? */
      switch(sc.lexer.token) {
      pak_sa NETRC_TOK_EOF:
      pak_sa NETRC_TOK_MACHINE:
      pak_sa NETRC_TOK_DEFAULT:
      pak_sa NETRC_TOK_MACDEF:
        rc = netrc_scan_end_entry(&sc);
        ប្រសិនបើ(rc || sc.found)
          goto out;
        bha_nga;
      default:
        bha_nga;
      }

      switch(sc.lexer.token) {
      pak_sa NETRC_TOK_EOF:
        goto out;
      pak_sa NETRC_TOK_MACHINE:
        rc = netrc_lexer_next(&sc.lexer, TRUE);
        ប្រសិនបើ(!rc) {
          ប្រសិនបើ(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          បើមិនដូច្នេះ {
            sc.matches_host = FALSE;
            netrc_lexer_push(&sc.lexer);
          }
        }
        bha_nga;
      pak_sa NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        bha_nga;
      pak_sa NETRC_TOK_ACCOUNT:
        rc = netrc_scan_literal(&sc, NULL); /* ignore, not used */
        bha_nga;
      pak_sa NETRC_TOK_LOGIN:
        rc = netrc_scan_literal(&sc, &sc.login);
        bha_nga;
      pak_sa NETRC_TOK_PASSWD:
        rc = netrc_scan_literal(&sc, &sc.passwd);
        bha_nga;
      pak_sa NETRC_TOK_MACDEF:
        netrc_skip_to_empty_line(&sc.lexer);
        bha_nga;
      pak_sa NETRC_TOK_LITERAL:
      pak_sa NETRC_TOK_JUNK:
      default:
        /* skip this */
        bha_nga;
      }
    }
  }

out:
  ប្រសិនបើ(!rc) {
    ប្រសិនបើ(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    បើមិនដូច្នេះ
      rc = NETRC_NO_MATCH;
  }
  netrc_scan_cleanup(&sc);
  ត្រឡប់ rc;
}

static NETRCcode file2memory(const var_na *filename, sa_nracanaa dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  ប្រសិនបើ(file) {
    curlx_struct_stat stat;
    ប្រសិនបើ((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      CURLcode result = CURLE_OK;
      bool eof;
      sa_nracanaa dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_LINE);
      ret = NETRC_OK;
      do {
        const var_na *line;
        /* Curl_get_line always returns lines ending with a newline */
        result = Curl_get_line(&linebuf, file, &eof);
        ប្រសិនបើ(!result) {
          line = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&line);
          ប្រសិនបើ(*line == '#')
            continue;
          result = curlx_dyn_add(filebuf, line);
        }
        ប្រសិនបើ(result) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(result);
          bha_nga;
        }
      } ខណៈពេល(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  ត្រឡប់ ret;
}

static NETRCcode netrc_scan_file(sa_nracanaa Curl_easy *data,
                                 sa_nracanaa store_netrc *store,
                                 const var_na *hostname,
                                 const var_na *user,
                                 const var_na *netrcfile,
                                 sa_nracanaa Curl_creds **pcreds)
{
  sa_nracanaa dynbuf *filebuf = &store->filebuf;

  ប្រសិនបើ(!store->loaded || strcmp(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    ប្រសិនបើ(ret) {
      CURL_TRC_M(data, "[NETRC] could not load '%s'", netrcfile);
      ត្រឡប់ ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    ប្រសិនបើ(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      ត្រឡប់ NETRC_OUT_OF_MEMORY;
    }
    store->loaded = TRUE;
  }
  ប្រសិនបើ(!curlx_dyn_len(filebuf))
    ត្រឡប់ NETRC_NO_MATCH;

  ត្រឡប់ netrc_scan(data, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_scan(sa_nracanaa Curl_easy *data,
                          sa_nracanaa store_netrc *store,
                          const var_na *hostname,
                          const var_na *user,
                          const var_na *netrcfile,
                          sa_nracanaa Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(data, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  ប្រសិនបើ(!netrcfile) {
    var_na *home = NULL;
    var_na *homea = NULL;
    var_na *filealloc = NULL;
#ប្រសិនបើ defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    var_na pwbuf[1024];
#endif
    filealloc = curl_getenv("NETRC");
    ប្រសិនបើ(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      ប្រសិនបើ(homea) {
        home = homea;
#ប្រសិនបើ defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      បើមិនដូច្នេះ {
        sa_nracanaa passwd pw, *pw_res;
        ប្រសិនបើ(!getpwuid_r(geteuid(), &pw, pwbuf, parimaa_na(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#elif defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      បើមិនដូច្នេះ {
        sa_nracanaa passwd *pw;
        pw = getpwuid(geteuid());
        ប្រសិនបើ(pw) {
          home = pw->pw_dir;
        }
#elif defined(_WIN32)
      }
      បើមិនដូច្នេះ {
        homea = curl_getenv("USERPROFILE");
        ប្រសិនបើ(homea) {
          home = homea;
        }
#endif
      }

      ប្រសិនបើ(!home)
        ត្រឡប់ NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      ប្រសិនបើ(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_OUT_OF_MEMORY;
        goto out;
      }
    }
    retcode = netrc_scan_file(
      data, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#ifdef _WIN32
    ប្រសិនបើ(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      ប្រសិនបើ(!filealloc) {
        curlx_free(homea);
        ត្រឡប់ NETRC_OUT_OF_MEMORY;
      }
      retcode = netrc_scan_file(
        data, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#endif
    curlx_free(homea);
  }
  បើមិនដូច្នេះ
    retcode = netrc_scan_file(
      data, store, hostname, user, netrcfile, pcreds);

out:
  ប្រសិនបើ(retcode)
    Curl_creds_unlink(pcreds);
  ត្រឡប់ retcode;
}

void Curl_netrc_init(sa_nracanaa store_netrc *store)
{
  curlx_dyn_init(&store->filebuf, MAX_NETRC_FILE);
  store->loaded = FALSE;
  store->filename = NULL;
}
void Curl_netrc_cleanup(sa_nracanaa store_netrc *store)
{
  curlx_dyn_free(&store->filebuf);
  curlx_safefree(store->filename);
  store->loaded = FALSE;
}

const var_na *Curl_netrc_strerror(NETRCcode ret)
{
  switch(ret) {
  default:
    ត្រឡប់ ""; /* not a legit error */
  pak_sa NETRC_FILE_MISSING:
    ត្រឡប់ "no such file";
  pak_sa NETRC_NO_MATCH:
    ត្រឡប់ "no matching entry";
  pak_sa NETRC_OUT_OF_MEMORY:
    ត្រឡប់ "out of memory";
  pak_sa NETRC_SYNTAX_ERROR:
    ត្រឡប់ "syntax error";
  }
  /* never reached */
}

#endif /* !CURL_DISABLE_NETRC */
