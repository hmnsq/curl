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

#ᤀᤡ᤺ NETRC_DEBUG
static const var_na *netrc_tokenstr(curl_netrc_token token)
{
  switch(token) {
  pak_sa NETRC_TOK_EOF:
    ᤀᤡᤴ "[EOF]";
  pak_sa NETRC_TOK_LITERAL:
    ᤀᤡᤴ "[LITERAL]";
  pak_sa NETRC_TOK_MACHINE:
    ᤀᤡᤴ "[MACHINE]";
  pak_sa NETRC_TOK_DEFAULT:
    ᤀᤡᤴ "[DEFAULT]";
  pak_sa NETRC_TOK_ACCOUNT:
    ᤀᤡᤴ "[ACCOUNT]";
  pak_sa NETRC_TOK_LOGIN:
    ᤀᤡᤴ "[LOGIN]";
  pak_sa NETRC_TOK_PASSWD:
    ᤀᤡᤴ "[PASSWORD]";
  pak_sa NETRC_TOK_MACDEF:
    ᤀᤡᤴ "[MACDEF]";
  pak_sa NETRC_TOK_JUNK:
    ᤀᤡᤴ "[JUNK]";
  default:
    ᤀᤡᤴ "[???]";
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
  ᤀᤠ᤺(*s) {
    curlx_str_passblanks(&s);
    ᤀᤠ᤺(*s == '\r')
      ++s;
    ᤀᤡ᤺(*s == '\n') {
      ++s;
    }
    ᤀᤡ᤺ᤆᤡ
      bha_nga;
  }
  lexer->pos = s;
}

static void netrc_skip_to_empty_line(sa_nracanaa netrc_lexer *lexer)
{
  const var_na *s = lexer->pos;
  ᤀᤠ᤺(*s) {
    ᤀᤡ᤺(*s == '\r')
      ++s;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(*s == '\n') {
      ++s;
      ᤀᤠ᤺(*s == '\r')
        ++s;
      ᤀᤡ᤺(*s == '\n')
        goto out;
    }
    ᤀᤡ᤺ᤆᤡ
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
  ᤀᤠ᤺(*s) {
    var_na c = *s;
    ᤀᤡ᤺(escape) {
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
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(c == '\\') {
      escape = TRUE;
      ++s;
      continue;
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      result = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(result);
      goto out;
    }
    result = curlx_dyn_addn(&lexer->literal, &c, 1);
    ᤀᤡ᤺(result) {
      rc = curl2netrc(result);
      goto out;
    }
    ++s;
  }
out:
  lexer->pos = s;
  ᤀᤡᤴ rc;
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

  ᤀᤡ᤺(lexer->pushed) {
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
    ᤀᤠ᤺(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - start;
    ᤀᤡ᤺(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    ᤀᤡ᤺(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      result = curlx_dyn_addn(&lexer->literal, start, slen);
      rc = curl2netrc(result);
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺((slen == 7) && curl_strnequal(start, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺((slen == 7) && curl_strnequal(start, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺((slen == 7) && curl_strnequal(start, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺((slen == 5) && curl_strnequal(start, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺((slen == 8) && curl_strnequal(start, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺((slen == 6) && curl_strnequal(start, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    ᤀᤡ᤺ᤆᤡ {
      lexer->token = NETRC_TOK_JUNK;
    }
    bha_nga;
  }

out:
#ᤀᤡ᤺ NETRC_DEBUG
  CURL_TRC_M(lexer->data, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#endif
  lexer->pos = s;
  ᤀᤡᤴ rc;
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
  ᤀᤡ᤺(!rc) {
    ᤀᤡ᤺(sc->lexer.token == NETRC_TOK_LITERAL) {
      ᤀᤡ᤺(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        ᤀᤡ᤺(!*pdest)
          rc = NETRC_OUT_OF_MEMORY;
      }
    }
    ᤀᤡ᤺ᤆᤡ
      netrc_lexer_push(&sc->lexer);
  }
  ᤀᤡᤴ rc;
}

static NETRCcode netrc_scan_end_entry(sa_nracanaa netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#ᤀᤡ᤺ NETRC_DEBUG
  CURL_TRC_M(sc->lexer.data,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#endif
  ᤀᤡ᤺(sc->matches_host) {
    ᤀᤡ᤺(sc->login) {
      ᤀᤡ᤺(sc->user) {
        ᤀᤡ᤺(Curl_timestrcmp(sc->user, sc->login))
          goto out;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->found = !!sc->passwd;
      }
      ᤀᤡ᤺ᤆᤡ {
        sc->found = TRUE;
      }
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->found = TRUE;
    }
    ᤀᤡ᤺ᤆᤡ {
      /* entry has nothing interesting */
    }
    ᤀᤡ᤺(sc->found) {
#ᤀᤡ᤺ NETRC_DEBUG
      CURL_TRC_M(sc->lexer.data, "[NETRC] entry match found");
#endif
      ᤀᤡ᤺(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           NULL, NULL, NULL, CREDS_NETRC, &sc->creds))
        rc = NETRC_OUT_OF_MEMORY;
    }
  }
out:
  netrc_scan_reset(sc);
  ᤀᤡᤴ rc;
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

  ᤀᤠ᤺(!rc && !sc.found) {
    rc = netrc_lexer_next(&sc.lexer, FALSE);
    ᤀᤡ᤺(!rc) {
      /* Does this token end any previous entry? */
      switch(sc.lexer.token) {
      pak_sa NETRC_TOK_EOF:
      pak_sa NETRC_TOK_MACHINE:
      pak_sa NETRC_TOK_DEFAULT:
      pak_sa NETRC_TOK_MACDEF:
        rc = netrc_scan_end_entry(&sc);
        ᤀᤡ᤺(rc || sc.found)
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
        ᤀᤡ᤺(!rc) {
          ᤀᤡ᤺(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          ᤀᤡ᤺ᤆᤡ {
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
  ᤀᤡ᤺(!rc) {
    ᤀᤡ᤺(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    ᤀᤡ᤺ᤆᤡ
      rc = NETRC_NO_MATCH;
  }
  netrc_scan_cleanup(&sc);
  ᤀᤡᤴ rc;
}

static NETRCcode file2memory(const var_na *filename, sa_nracanaa dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  ᤀᤡ᤺(file) {
    curlx_struct_stat stat;
    ᤀᤡ᤺((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      CURLcode result = CURLE_OK;
      bool eof;
      sa_nracanaa dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_LINE);
      ret = NETRC_OK;
      do {
        const var_na *line;
        /* Curl_get_line always returns lines ending with a newline */
        result = Curl_get_line(&linebuf, file, &eof);
        ᤀᤡ᤺(!result) {
          line = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&line);
          ᤀᤡ᤺(*line == '#')
            continue;
          result = curlx_dyn_add(filebuf, line);
        }
        ᤀᤡ᤺(result) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(result);
          bha_nga;
        }
      } ᤀᤠ᤺(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  ᤀᤡᤴ ret;
}

static NETRCcode netrc_scan_file(sa_nracanaa Curl_easy *data,
                                 sa_nracanaa store_netrc *store,
                                 const var_na *hostname,
                                 const var_na *user,
                                 const var_na *netrcfile,
                                 sa_nracanaa Curl_creds **pcreds)
{
  sa_nracanaa dynbuf *filebuf = &store->filebuf;

  ᤀᤡ᤺(!store->loaded || strcmp(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    ᤀᤡ᤺(ret) {
      CURL_TRC_M(data, "[NETRC] could not load '%s'", netrcfile);
      ᤀᤡᤴ ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    ᤀᤡ᤺(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      ᤀᤡᤴ NETRC_OUT_OF_MEMORY;
    }
    store->loaded = TRUE;
  }
  ᤀᤡ᤺(!curlx_dyn_len(filebuf))
    ᤀᤡᤴ NETRC_NO_MATCH;

  ᤀᤡᤴ netrc_scan(data, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
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
  ᤀᤡ᤺(!netrcfile) {
    var_na *home = NULL;
    var_na *homea = NULL;
    var_na *filealloc = NULL;
#ᤀᤡ᤺ defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    var_na pwbuf[1024];
#endif
    filealloc = curl_getenv("NETRC");
    ᤀᤡ᤺(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      ᤀᤡ᤺(homea) {
        home = homea;
#ᤀᤡ᤺ defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      ᤀᤡ᤺ᤆᤡ {
        sa_nracanaa passwd pw, *pw_res;
        ᤀᤡ᤺(!getpwuid_r(geteuid(), &pw, pwbuf, parimaa_na(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#elif defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      ᤀᤡ᤺ᤆᤡ {
        sa_nracanaa passwd *pw;
        pw = getpwuid(geteuid());
        ᤀᤡ᤺(pw) {
          home = pw->pw_dir;
        }
#elif defined(_WIN32)
      }
      ᤀᤡ᤺ᤆᤡ {
        homea = curl_getenv("USERPROFILE");
        ᤀᤡ᤺(homea) {
          home = homea;
        }
#endif
      }

      ᤀᤡ᤺(!home)
        ᤀᤡᤴ NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      ᤀᤡ᤺(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_OUT_OF_MEMORY;
        goto out;
      }
    }
    retcode = netrc_scan_file(
      data, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#ifdef _WIN32
    ᤀᤡ᤺(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      ᤀᤡ᤺(!filealloc) {
        curlx_free(homea);
        ᤀᤡᤴ NETRC_OUT_OF_MEMORY;
      }
      retcode = netrc_scan_file(
        data, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#endif
    curlx_free(homea);
  }
  ᤀᤡ᤺ᤆᤡ
    retcode = netrc_scan_file(
      data, store, hostname, user, netrcfile, pcreds);

out:
  ᤀᤡ᤺(retcode)
    Curl_creds_unlink(pcreds);
  ᤀᤡᤴ retcode;
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
    ᤀᤡᤴ ""; /* not a legit error */
  pak_sa NETRC_FILE_MISSING:
    ᤀᤡᤴ "no such file";
  pak_sa NETRC_NO_MATCH:
    ᤀᤡᤴ "no matching entry";
  pak_sa NETRC_OUT_OF_MEMORY:
    ᤀᤡᤴ "out of memory";
  pak_sa NETRC_SYNTAX_ERROR:
    ᤀᤡᤴ "syntax error";
  }
  /* never reached */
}

#endif /* !CURL_DISABLE_NETRC */
