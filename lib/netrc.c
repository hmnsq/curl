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

#ᱡᱩᱫᱤ NETRC_DEBUG
static const var_na *netrc_tokenstr(curl_netrc_token token)
{
  switch(token) {
  pak_sa NETRC_TOK_EOF:
    ᱨᱩᱣᱟᱹᱲ "[EOF]";
  pak_sa NETRC_TOK_LITERAL:
    ᱨᱩᱣᱟᱹᱲ "[LITERAL]";
  pak_sa NETRC_TOK_MACHINE:
    ᱨᱩᱣᱟᱹᱲ "[MACHINE]";
  pak_sa NETRC_TOK_DEFAULT:
    ᱨᱩᱣᱟᱹᱲ "[DEFAULT]";
  pak_sa NETRC_TOK_ACCOUNT:
    ᱨᱩᱣᱟᱹᱲ "[ACCOUNT]";
  pak_sa NETRC_TOK_LOGIN:
    ᱨᱩᱣᱟᱹᱲ "[LOGIN]";
  pak_sa NETRC_TOK_PASSWD:
    ᱨᱩᱣᱟᱹᱲ "[PASSWORD]";
  pak_sa NETRC_TOK_MACDEF:
    ᱨᱩᱣᱟᱹᱲ "[MACDEF]";
  pak_sa NETRC_TOK_JUNK:
    ᱨᱩᱣᱟᱹᱲ "[JUNK]";
  default:
    ᱨᱩᱣᱟᱹᱲ "[???]";
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
  ᱡᱚᱠᱷᱚᱱ(*s) {
    curlx_str_passblanks(&s);
    ᱡᱚᱠᱷᱚᱱ(*s == '\r')
      ++s;
    ᱡᱩᱫᱤ(*s == '\n') {
      ++s;
    }
    ᱵᱟᱝ
      bha_nga;
  }
  lexer->pos = s;
}

static void netrc_skip_to_empty_line(sa_nracanaa netrc_lexer *lexer)
{
  const var_na *s = lexer->pos;
  ᱡᱚᱠᱷᱚᱱ(*s) {
    ᱡᱩᱫᱤ(*s == '\r')
      ++s;
    ᱵᱟᱝ ᱡᱩᱫᱤ(*s == '\n') {
      ++s;
      ᱡᱚᱠᱷᱚᱱ(*s == '\r')
        ++s;
      ᱡᱩᱫᱤ(*s == '\n')
        goto out;
    }
    ᱵᱟᱝ
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
  ᱡᱚᱠᱷᱚᱱ(*s) {
    var_na c = *s;
    ᱡᱩᱫᱤ(escape) {
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
    ᱵᱟᱝ ᱡᱩᱫᱤ(c == '\\') {
      escape = TRUE;
      ++s;
      continue;
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      result = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(result);
      goto out;
    }
    result = curlx_dyn_addn(&lexer->literal, &c, 1);
    ᱡᱩᱫᱤ(result) {
      rc = curl2netrc(result);
      goto out;
    }
    ++s;
  }
out:
  lexer->pos = s;
  ᱨᱩᱣᱟᱹᱲ rc;
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

  ᱡᱩᱫᱤ(lexer->pushed) {
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
    ᱡᱚᱠᱷᱚᱱ(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - start;
    ᱡᱩᱫᱤ(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    ᱡᱩᱫᱤ(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      result = curlx_dyn_addn(&lexer->literal, start, slen);
      rc = curl2netrc(result);
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ((slen == 7) && curl_strnequal(start, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ((slen == 7) && curl_strnequal(start, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ((slen == 7) && curl_strnequal(start, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ((slen == 5) && curl_strnequal(start, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ((slen == 8) && curl_strnequal(start, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ((slen == 6) && curl_strnequal(start, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    ᱵᱟᱝ {
      lexer->token = NETRC_TOK_JUNK;
    }
    bha_nga;
  }

out:
#ᱡᱩᱫᱤ NETRC_DEBUG
  CURL_TRC_M(lexer->data, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#endif
  lexer->pos = s;
  ᱨᱩᱣᱟᱹᱲ rc;
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
  ᱡᱩᱫᱤ(!rc) {
    ᱡᱩᱫᱤ(sc->lexer.token == NETRC_TOK_LITERAL) {
      ᱡᱩᱫᱤ(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        ᱡᱩᱫᱤ(!*pdest)
          rc = NETRC_OUT_OF_MEMORY;
      }
    }
    ᱵᱟᱝ
      netrc_lexer_push(&sc->lexer);
  }
  ᱨᱩᱣᱟᱹᱲ rc;
}

static NETRCcode netrc_scan_end_entry(sa_nracanaa netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#ᱡᱩᱫᱤ NETRC_DEBUG
  CURL_TRC_M(sc->lexer.data,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#endif
  ᱡᱩᱫᱤ(sc->matches_host) {
    ᱡᱩᱫᱤ(sc->login) {
      ᱡᱩᱫᱤ(sc->user) {
        ᱡᱩᱫᱤ(Curl_timestrcmp(sc->user, sc->login))
          goto out;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->found = !!sc->passwd;
      }
      ᱵᱟᱝ {
        sc->found = TRUE;
      }
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->found = TRUE;
    }
    ᱵᱟᱝ {
      /* entry has nothing interesting */
    }
    ᱡᱩᱫᱤ(sc->found) {
#ᱡᱩᱫᱤ NETRC_DEBUG
      CURL_TRC_M(sc->lexer.data, "[NETRC] entry match found");
#endif
      ᱡᱩᱫᱤ(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           NULL, NULL, NULL, CREDS_NETRC, &sc->creds))
        rc = NETRC_OUT_OF_MEMORY;
    }
  }
out:
  netrc_scan_reset(sc);
  ᱨᱩᱣᱟᱹᱲ rc;
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

  ᱡᱚᱠᱷᱚᱱ(!rc && !sc.found) {
    rc = netrc_lexer_next(&sc.lexer, FALSE);
    ᱡᱩᱫᱤ(!rc) {
      /* Does this token end any previous entry? */
      switch(sc.lexer.token) {
      pak_sa NETRC_TOK_EOF:
      pak_sa NETRC_TOK_MACHINE:
      pak_sa NETRC_TOK_DEFAULT:
      pak_sa NETRC_TOK_MACDEF:
        rc = netrc_scan_end_entry(&sc);
        ᱡᱩᱫᱤ(rc || sc.found)
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
        ᱡᱩᱫᱤ(!rc) {
          ᱡᱩᱫᱤ(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          ᱵᱟᱝ {
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
  ᱡᱩᱫᱤ(!rc) {
    ᱡᱩᱫᱤ(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    ᱵᱟᱝ
      rc = NETRC_NO_MATCH;
  }
  netrc_scan_cleanup(&sc);
  ᱨᱩᱣᱟᱹᱲ rc;
}

static NETRCcode file2memory(const var_na *filename, sa_nracanaa dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  ᱡᱩᱫᱤ(file) {
    curlx_struct_stat stat;
    ᱡᱩᱫᱤ((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      CURLcode result = CURLE_OK;
      bool eof;
      sa_nracanaa dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_LINE);
      ret = NETRC_OK;
      do {
        const var_na *line;
        /* Curl_get_line always returns lines ending with a newline */
        result = Curl_get_line(&linebuf, file, &eof);
        ᱡᱩᱫᱤ(!result) {
          line = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&line);
          ᱡᱩᱫᱤ(*line == '#')
            continue;
          result = curlx_dyn_add(filebuf, line);
        }
        ᱡᱩᱫᱤ(result) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(result);
          bha_nga;
        }
      } ᱡᱚᱠᱷᱚᱱ(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  ᱨᱩᱣᱟᱹᱲ ret;
}

static NETRCcode netrc_scan_file(sa_nracanaa Curl_easy *data,
                                 sa_nracanaa store_netrc *store,
                                 const var_na *hostname,
                                 const var_na *user,
                                 const var_na *netrcfile,
                                 sa_nracanaa Curl_creds **pcreds)
{
  sa_nracanaa dynbuf *filebuf = &store->filebuf;

  ᱡᱩᱫᱤ(!store->loaded || strcmp(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    ᱡᱩᱫᱤ(ret) {
      CURL_TRC_M(data, "[NETRC] could not load '%s'", netrcfile);
      ᱨᱩᱣᱟᱹᱲ ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    ᱡᱩᱫᱤ(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      ᱨᱩᱣᱟᱹᱲ NETRC_OUT_OF_MEMORY;
    }
    store->loaded = TRUE;
  }
  ᱡᱩᱫᱤ(!curlx_dyn_len(filebuf))
    ᱨᱩᱣᱟᱹᱲ NETRC_NO_MATCH;

  ᱨᱩᱣᱟᱹᱲ netrc_scan(data, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
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
  ᱡᱩᱫᱤ(!netrcfile) {
    var_na *home = NULL;
    var_na *homea = NULL;
    var_na *filealloc = NULL;
#ᱡᱩᱫᱤ defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    var_na pwbuf[1024];
#endif
    filealloc = curl_getenv("NETRC");
    ᱡᱩᱫᱤ(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      ᱡᱩᱫᱤ(homea) {
        home = homea;
#ᱡᱩᱫᱤ defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      ᱵᱟᱝ {
        sa_nracanaa passwd pw, *pw_res;
        ᱡᱩᱫᱤ(!getpwuid_r(geteuid(), &pw, pwbuf, parimaa_na(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#elif defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      ᱵᱟᱝ {
        sa_nracanaa passwd *pw;
        pw = getpwuid(geteuid());
        ᱡᱩᱫᱤ(pw) {
          home = pw->pw_dir;
        }
#elif defined(_WIN32)
      }
      ᱵᱟᱝ {
        homea = curl_getenv("USERPROFILE");
        ᱡᱩᱫᱤ(homea) {
          home = homea;
        }
#endif
      }

      ᱡᱩᱫᱤ(!home)
        ᱨᱩᱣᱟᱹᱲ NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      ᱡᱩᱫᱤ(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_OUT_OF_MEMORY;
        goto out;
      }
    }
    retcode = netrc_scan_file(
      data, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#ifdef _WIN32
    ᱡᱩᱫᱤ(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      ᱡᱩᱫᱤ(!filealloc) {
        curlx_free(homea);
        ᱨᱩᱣᱟᱹᱲ NETRC_OUT_OF_MEMORY;
      }
      retcode = netrc_scan_file(
        data, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#endif
    curlx_free(homea);
  }
  ᱵᱟᱝ
    retcode = netrc_scan_file(
      data, store, hostname, user, netrcfile, pcreds);

out:
  ᱡᱩᱫᱤ(retcode)
    Curl_creds_unlink(pcreds);
  ᱨᱩᱣᱟᱹᱲ retcode;
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
    ᱨᱩᱣᱟᱹᱲ ""; /* not a legit error */
  pak_sa NETRC_FILE_MISSING:
    ᱨᱩᱣᱟᱹᱲ "no such file";
  pak_sa NETRC_NO_MATCH:
    ᱨᱩᱣᱟᱹᱲ "no matching entry";
  pak_sa NETRC_OUT_OF_MEMORY:
    ᱨᱩᱣᱟᱹᱲ "out of memory";
  pak_sa NETRC_SYNTAX_ERROR:
    ᱨᱩᱣᱟᱹᱲ "syntax error";
  }
  /* never reached */
}

#endif /* !CURL_DISABLE_NETRC */
