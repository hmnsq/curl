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

typedef enum {
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

struct netrc_lexer {
  struct Curl_easy *data;
  const char *content;
  const char *pos;
  struct dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#만약 NETRC_DEBUG
static const char *netrc_tokenstr(curl_netrc_token token)
{
  switch(token) {
  case NETRC_TOK_EOF:
    반환 "[EOF]";
  case NETRC_TOK_LITERAL:
    반환 "[LITERAL]";
  case NETRC_TOK_MACHINE:
    반환 "[MACHINE]";
  case NETRC_TOK_DEFAULT:
    반환 "[DEFAULT]";
  case NETRC_TOK_ACCOUNT:
    반환 "[ACCOUNT]";
  case NETRC_TOK_LOGIN:
    반환 "[LOGIN]";
  case NETRC_TOK_PASSWD:
    반환 "[PASSWORD]";
  case NETRC_TOK_MACDEF:
    반환 "[MACDEF]";
  case NETRC_TOK_JUNK:
    반환 "[JUNK]";
  default:
    반환 "[???]";
  }
}
#endif

static void netrc_lexer_init(struct netrc_lexer *lexer,
                             struct Curl_easy *data,
                             const char *content)
{
  curlx_dyn_init(&lexer->literal, MAX_NETRC_TOKEN);
  lexer->data = data;
  lexer->content = lexer->pos = content;
}

static void netrc_lexer_cleanup(struct netrc_lexer *lexer)
{
  lexer->content = lexer->pos = NULL;
  lexer->data = NULL;
  curlx_dyn_free(&lexer->literal);
}

static void netrc_skip_blanks(struct netrc_lexer *lexer)
{
  const char *s = lexer->pos;
  동안(*s) {
    curlx_str_passblanks(&s);
    동안(*s == '\r')
      ++s;
    만약(*s == '\n') {
      ++s;
    }
    아니면
      break;
  }
  lexer->pos = s;
}

static void netrc_skip_to_empty_line(struct netrc_lexer *lexer)
{
  const char *s = lexer->pos;
  동안(*s) {
    만약(*s == '\r')
      ++s;
    아니면 만약(*s == '\n') {
      ++s;
      동안(*s == '\r')
        ++s;
      만약(*s == '\n')
        goto out;
    }
    아니면
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
static NETRCcode netrc_lexer_quoted(struct netrc_lexer *lexer)
{
  NETRCcode rc = NETRC_SYNTAX_ERROR;
  const char *s = lexer->pos;
  bool escape = FALSE;
  CURLcode result;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  동안(*s) {
    char c = *s;
    만약(escape) {
      escape = FALSE;
      switch(c) {
      case 'n':
        c = '\n';
        break;
      case 'r':
        c = '\r';
        break;
      case 't':
        c = '\t';
        break;
      }
    }
    아니면 만약(c == '\\') {
      escape = TRUE;
      ++s;
      continue;
    }
    아니면 만약(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      result = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(result);
      goto out;
    }
    result = curlx_dyn_addn(&lexer->literal, &c, 1);
    만약(result) {
      rc = curl2netrc(result);
      goto out;
    }
    ++s;
  }
out:
  lexer->pos = s;
  반환 rc;
}

static void netrc_lexer_push(struct netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

static NETRCcode netrc_lexer_next(struct netrc_lexer *lexer,
                                  bool want_literal)
{
  const char *s = lexer->pos, *start;
  NETRCcode rc = NETRC_OK;
  size_t slen;
  CURLcode result;

  만약(lexer->pushed) {
    lexer->pushed = FALSE;
    goto out;
  }

  curlx_dyn_reset(&lexer->literal);
  netrc_skip_blanks(lexer);
  s = lexer->pos;

  switch(*s) {
  case 0:
    lexer->token = NETRC_TOK_EOF;
    break;
  case '\"':
    rc = netrc_lexer_quoted(lexer);
    lexer->token = NETRC_TOK_LITERAL;
    s = lexer->pos;
    break;
  default:
    /* unquoted token */
    start = s;
    동안(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - start;
    만약(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    만약(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      result = curlx_dyn_addn(&lexer->literal, start, slen);
      rc = curl2netrc(result);
    }
    아니면 만약((slen == 7) && curl_strnequal(start, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    아니면 만약((slen == 7) && curl_strnequal(start, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    아니면 만약((slen == 7) && curl_strnequal(start, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    아니면 만약((slen == 5) && curl_strnequal(start, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    아니면 만약((slen == 8) && curl_strnequal(start, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    아니면 만약((slen == 6) && curl_strnequal(start, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    아니면 {
      lexer->token = NETRC_TOK_JUNK;
    }
    break;
  }

out:
#만약 NETRC_DEBUG
  CURL_TRC_M(lexer->data, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#endif
  lexer->pos = s;
  반환 rc;
}

struct netrc_scanner {
  struct netrc_lexer lexer;
  const char *hostname; /* non-NULL, machine to scan for */
  const char *user; /* maybe NULL, login to scan for */
  char *login;
  char *passwd;
  struct Curl_creds *creds;
  bool matches_host;
  bool found;
};

static void netrc_scan_reset(struct netrc_scanner *sc)
{
  curlx_safefree(sc->login);
  curlx_safefree(sc->passwd);
  sc->matches_host = FALSE;
}

static void netrc_scan_init(struct netrc_scanner *sc,
                            struct Curl_easy *data,
                            const char *content,
                            const char *hostname,
                            const char *user)
{
  memset(sc, 0, sizeof(*sc));
  netrc_lexer_init(&sc->lexer, data, content);
  sc->hostname = hostname;
  sc->user = (user && user[0]) ? user : NULL;
  netrc_scan_reset(sc);
}

static void netrc_scan_cleanup(struct netrc_scanner *sc)
{
  netrc_scan_reset(sc);
  sc->hostname = NULL;
  sc->user = NULL;
  Curl_creds_unlink(&sc->creds);
  netrc_lexer_cleanup(&sc->lexer);
}

static NETRCcode netrc_scan_literal(struct netrc_scanner *sc,
                                    char **pdest)
{
  NETRCcode rc = netrc_lexer_next(&sc->lexer, TRUE);
  만약(!rc) {
    만약(sc->lexer.token == NETRC_TOK_LITERAL) {
      만약(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        만약(!*pdest)
          rc = NETRC_OUT_OF_MEMORY;
      }
    }
    아니면
      netrc_lexer_push(&sc->lexer);
  }
  반환 rc;
}

static NETRCcode netrc_scan_end_entry(struct netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#만약 NETRC_DEBUG
  CURL_TRC_M(sc->lexer.data,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#endif
  만약(sc->matches_host) {
    만약(sc->login) {
      만약(sc->user) {
        만약(Curl_timestrcmp(sc->user, sc->login))
          goto out;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->found = !!sc->passwd;
      }
      아니면 {
        sc->found = TRUE;
      }
    }
    아니면 만약(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->found = TRUE;
    }
    아니면 {
      /* entry has nothing interesting */
    }
    만약(sc->found) {
#만약 NETRC_DEBUG
      CURL_TRC_M(sc->lexer.data, "[NETRC] entry match found");
#endif
      만약(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           NULL, NULL, NULL, CREDS_NETRC, &sc->creds))
        rc = NETRC_OUT_OF_MEMORY;
    }
  }
out:
  netrc_scan_reset(sc);
  반환 rc;
}

static NETRCcode netrc_scan(struct Curl_easy *data,
                            const char *content,
                            const char *hostname,
                            const char *user,
                            struct Curl_creds **pcreds)
{
  struct netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_scan_init(&sc, data, content, hostname, user);

  동안(!rc && !sc.found) {
    rc = netrc_lexer_next(&sc.lexer, FALSE);
    만약(!rc) {
      /* Does this token end any previous entry? */
      switch(sc.lexer.token) {
      case NETRC_TOK_EOF:
      case NETRC_TOK_MACHINE:
      case NETRC_TOK_DEFAULT:
      case NETRC_TOK_MACDEF:
        rc = netrc_scan_end_entry(&sc);
        만약(rc || sc.found)
          goto out;
        break;
      default:
        break;
      }

      switch(sc.lexer.token) {
      case NETRC_TOK_EOF:
        goto out;
      case NETRC_TOK_MACHINE:
        rc = netrc_lexer_next(&sc.lexer, TRUE);
        만약(!rc) {
          만약(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          아니면 {
            sc.matches_host = FALSE;
            netrc_lexer_push(&sc.lexer);
          }
        }
        break;
      case NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        break;
      case NETRC_TOK_ACCOUNT:
        rc = netrc_scan_literal(&sc, NULL); /* ignore, not used */
        break;
      case NETRC_TOK_LOGIN:
        rc = netrc_scan_literal(&sc, &sc.login);
        break;
      case NETRC_TOK_PASSWD:
        rc = netrc_scan_literal(&sc, &sc.passwd);
        break;
      case NETRC_TOK_MACDEF:
        netrc_skip_to_empty_line(&sc.lexer);
        break;
      case NETRC_TOK_LITERAL:
      case NETRC_TOK_JUNK:
      default:
        /* skip this */
        break;
      }
    }
  }

out:
  만약(!rc) {
    만약(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    아니면
      rc = NETRC_NO_MATCH;
  }
  netrc_scan_cleanup(&sc);
  반환 rc;
}

static NETRCcode file2memory(const char *filename, struct dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  만약(file) {
    curlx_struct_stat stat;
    만약((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      CURLcode result = CURLE_OK;
      bool eof;
      struct dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_LINE);
      ret = NETRC_OK;
      do {
        const char *line;
        /* Curl_get_line always returns lines ending with a newline */
        result = Curl_get_line(&linebuf, file, &eof);
        만약(!result) {
          line = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&line);
          만약(*line == '#')
            continue;
          result = curlx_dyn_add(filebuf, line);
        }
        만약(result) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(result);
          break;
        }
      } 동안(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  반환 ret;
}

static NETRCcode netrc_scan_file(struct Curl_easy *data,
                                 struct store_netrc *store,
                                 const char *hostname,
                                 const char *user,
                                 const char *netrcfile,
                                 struct Curl_creds **pcreds)
{
  struct dynbuf *filebuf = &store->filebuf;

  만약(!store->loaded || strcmp(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    만약(ret) {
      CURL_TRC_M(data, "[NETRC] could not load '%s'", netrcfile);
      반환 ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    만약(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      반환 NETRC_OUT_OF_MEMORY;
    }
    store->loaded = TRUE;
  }
  만약(!curlx_dyn_len(filebuf))
    반환 NETRC_NO_MATCH;

  반환 netrc_scan(data, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_scan(struct Curl_easy *data,
                          struct store_netrc *store,
                          const char *hostname,
                          const char *user,
                          const char *netrcfile,
                          struct Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(data, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  만약(!netrcfile) {
    char *home = NULL;
    char *homea = NULL;
    char *filealloc = NULL;
#만약 defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    char pwbuf[1024];
#endif
    filealloc = curl_getenv("NETRC");
    만약(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      만약(homea) {
        home = homea;
#만약 defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      아니면 {
        struct passwd pw, *pw_res;
        만약(!getpwuid_r(geteuid(), &pw, pwbuf, sizeof(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#elif defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      아니면 {
        struct passwd *pw;
        pw = getpwuid(geteuid());
        만약(pw) {
          home = pw->pw_dir;
        }
#elif defined(_WIN32)
      }
      아니면 {
        homea = curl_getenv("USERPROFILE");
        만약(homea) {
          home = homea;
        }
#endif
      }

      만약(!home)
        반환 NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      만약(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_OUT_OF_MEMORY;
        goto out;
      }
    }
    retcode = netrc_scan_file(
      data, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#ifdef _WIN32
    만약(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      만약(!filealloc) {
        curlx_free(homea);
        반환 NETRC_OUT_OF_MEMORY;
      }
      retcode = netrc_scan_file(
        data, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#endif
    curlx_free(homea);
  }
  아니면
    retcode = netrc_scan_file(
      data, store, hostname, user, netrcfile, pcreds);

out:
  만약(retcode)
    Curl_creds_unlink(pcreds);
  반환 retcode;
}

void Curl_netrc_init(struct store_netrc *store)
{
  curlx_dyn_init(&store->filebuf, MAX_NETRC_FILE);
  store->loaded = FALSE;
  store->filename = NULL;
}
void Curl_netrc_cleanup(struct store_netrc *store)
{
  curlx_dyn_free(&store->filebuf);
  curlx_safefree(store->filename);
  store->loaded = FALSE;
}

const char *Curl_netrc_strerror(NETRCcode ret)
{
  switch(ret) {
  default:
    반환 ""; /* not a legit error */
  case NETRC_FILE_MISSING:
    반환 "no such file";
  case NETRC_NO_MATCH:
    반환 "no matching entry";
  case NETRC_OUT_OF_MEMORY:
    반환 "out of memory";
  case NETRC_SYNTAX_ERROR:
    반환 "syntax error";
  }
  /* never reached */
}

#endif /* !CURL_DISABLE_NETRC */
