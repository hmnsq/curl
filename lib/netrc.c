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

#nếu NETRC_DEBUG
static const char *netrc_tokenstr(curl_netrc_token token)
{
  switch(token) {
  case NETRC_TOK_EOF:
    trả_về "[EOF]";
  case NETRC_TOK_LITERAL:
    trả_về "[LITERAL]";
  case NETRC_TOK_MACHINE:
    trả_về "[MACHINE]";
  case NETRC_TOK_DEFAULT:
    trả_về "[DEFAULT]";
  case NETRC_TOK_ACCOUNT:
    trả_về "[ACCOUNT]";
  case NETRC_TOK_LOGIN:
    trả_về "[LOGIN]";
  case NETRC_TOK_PASSWD:
    trả_về "[PASSWORD]";
  case NETRC_TOK_MACDEF:
    trả_về "[MACDEF]";
  case NETRC_TOK_JUNK:
    trả_về "[JUNK]";
  default:
    trả_về "[???]";
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
  trong_khi(*s) {
    curlx_str_passblanks(&s);
    trong_khi(*s == '\r')
      ++s;
    nếu(*s == '\n') {
      ++s;
    }
    không_thì
      break;
  }
  lexer->pos = s;
}

static void netrc_skip_to_empty_line(struct netrc_lexer *lexer)
{
  const char *s = lexer->pos;
  trong_khi(*s) {
    nếu(*s == '\r')
      ++s;
    không_thì nếu(*s == '\n') {
      ++s;
      trong_khi(*s == '\r')
        ++s;
      nếu(*s == '\n')
        goto out;
    }
    không_thì
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
  trong_khi(*s) {
    char c = *s;
    nếu(escape) {
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
    không_thì nếu(c == '\\') {
      escape = TRUE;
      ++s;
      continue;
    }
    không_thì nếu(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      result = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(result);
      goto out;
    }
    result = curlx_dyn_addn(&lexer->literal, &c, 1);
    nếu(result) {
      rc = curl2netrc(result);
      goto out;
    }
    ++s;
  }
out:
  lexer->pos = s;
  trả_về rc;
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

  nếu(lexer->pushed) {
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
    trong_khi(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - start;
    nếu(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    nếu(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      result = curlx_dyn_addn(&lexer->literal, start, slen);
      rc = curl2netrc(result);
    }
    không_thì nếu((slen == 7) && curl_strnequal(start, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    không_thì nếu((slen == 7) && curl_strnequal(start, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    không_thì nếu((slen == 7) && curl_strnequal(start, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    không_thì nếu((slen == 5) && curl_strnequal(start, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    không_thì nếu((slen == 8) && curl_strnequal(start, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    không_thì nếu((slen == 6) && curl_strnequal(start, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    không_thì {
      lexer->token = NETRC_TOK_JUNK;
    }
    break;
  }

out:
#nếu NETRC_DEBUG
  CURL_TRC_M(lexer->data, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#endif
  lexer->pos = s;
  trả_về rc;
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
  nếu(!rc) {
    nếu(sc->lexer.token == NETRC_TOK_LITERAL) {
      nếu(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        nếu(!*pdest)
          rc = NETRC_OUT_OF_MEMORY;
      }
    }
    không_thì
      netrc_lexer_push(&sc->lexer);
  }
  trả_về rc;
}

static NETRCcode netrc_scan_end_entry(struct netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#nếu NETRC_DEBUG
  CURL_TRC_M(sc->lexer.data,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#endif
  nếu(sc->matches_host) {
    nếu(sc->login) {
      nếu(sc->user) {
        nếu(Curl_timestrcmp(sc->user, sc->login))
          goto out;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->found = !!sc->passwd;
      }
      không_thì {
        sc->found = TRUE;
      }
    }
    không_thì nếu(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->found = TRUE;
    }
    không_thì {
      /* entry has nothing interesting */
    }
    nếu(sc->found) {
#nếu NETRC_DEBUG
      CURL_TRC_M(sc->lexer.data, "[NETRC] entry match found");
#endif
      nếu(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           NULL, NULL, NULL, CREDS_NETRC, &sc->creds))
        rc = NETRC_OUT_OF_MEMORY;
    }
  }
out:
  netrc_scan_reset(sc);
  trả_về rc;
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

  trong_khi(!rc && !sc.found) {
    rc = netrc_lexer_next(&sc.lexer, FALSE);
    nếu(!rc) {
      /* Does this token end any previous entry? */
      switch(sc.lexer.token) {
      case NETRC_TOK_EOF:
      case NETRC_TOK_MACHINE:
      case NETRC_TOK_DEFAULT:
      case NETRC_TOK_MACDEF:
        rc = netrc_scan_end_entry(&sc);
        nếu(rc || sc.found)
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
        nếu(!rc) {
          nếu(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          không_thì {
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
  nếu(!rc) {
    nếu(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    không_thì
      rc = NETRC_NO_MATCH;
  }
  netrc_scan_cleanup(&sc);
  trả_về rc;
}

static NETRCcode file2memory(const char *filename, struct dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  nếu(file) {
    curlx_struct_stat stat;
    nếu((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      CURLcode result = CURLE_OK;
      bool eof;
      struct dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_LINE);
      ret = NETRC_OK;
      do {
        const char *line;
        /* Curl_get_line always returns lines ending with a newline */
        result = Curl_get_line(&linebuf, file, &eof);
        nếu(!result) {
          line = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&line);
          nếu(*line == '#')
            continue;
          result = curlx_dyn_add(filebuf, line);
        }
        nếu(result) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(result);
          break;
        }
      } trong_khi(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  trả_về ret;
}

static NETRCcode netrc_scan_file(struct Curl_easy *data,
                                 struct store_netrc *store,
                                 const char *hostname,
                                 const char *user,
                                 const char *netrcfile,
                                 struct Curl_creds **pcreds)
{
  struct dynbuf *filebuf = &store->filebuf;

  nếu(!store->loaded || strcmp(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    nếu(ret) {
      CURL_TRC_M(data, "[NETRC] could not load '%s'", netrcfile);
      trả_về ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    nếu(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      trả_về NETRC_OUT_OF_MEMORY;
    }
    store->loaded = TRUE;
  }
  nếu(!curlx_dyn_len(filebuf))
    trả_về NETRC_NO_MATCH;

  trả_về netrc_scan(data, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
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
  nếu(!netrcfile) {
    char *home = NULL;
    char *homea = NULL;
    char *filealloc = NULL;
#nếu defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    char pwbuf[1024];
#endif
    filealloc = curl_getenv("NETRC");
    nếu(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      nếu(homea) {
        home = homea;
#nếu defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      không_thì {
        struct passwd pw, *pw_res;
        nếu(!getpwuid_r(geteuid(), &pw, pwbuf, sizeof(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#elif defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      không_thì {
        struct passwd *pw;
        pw = getpwuid(geteuid());
        nếu(pw) {
          home = pw->pw_dir;
        }
#elif defined(_WIN32)
      }
      không_thì {
        homea = curl_getenv("USERPROFILE");
        nếu(homea) {
          home = homea;
        }
#endif
      }

      nếu(!home)
        trả_về NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      nếu(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_OUT_OF_MEMORY;
        goto out;
      }
    }
    retcode = netrc_scan_file(
      data, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#ifdef _WIN32
    nếu(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      nếu(!filealloc) {
        curlx_free(homea);
        trả_về NETRC_OUT_OF_MEMORY;
      }
      retcode = netrc_scan_file(
        data, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#endif
    curlx_free(homea);
  }
  không_thì
    retcode = netrc_scan_file(
      data, store, hostname, user, netrcfile, pcreds);

out:
  nếu(retcode)
    Curl_creds_unlink(pcreds);
  trả_về retcode;
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
    trả_về ""; /* not a legit error */
  case NETRC_FILE_MISSING:
    trả_về "no such file";
  case NETRC_NO_MATCH:
    trả_về "no matching entry";
  case NETRC_OUT_OF_MEMORY:
    trả_về "out of memory";
  case NETRC_SYNTAX_ERROR:
    trả_về "syntax error";
  }
  /* never reached */
}

#endif /* !CURL_DISABLE_NETRC */
