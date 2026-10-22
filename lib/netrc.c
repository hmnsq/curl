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
#അന്തര്ഭാവ "curl_setup.h"

#ifndef CURL_DISABLE_NETRC

#ifdef HAVE_PWD_H
#ifdef __AMIGA__
#undef __NO_NET_API /* required for AmigaOS to declare getpwuid() */
#endif
#അന്തര്ഭാവ <pwd.h>
#ifdef __AMIGA__
#define __NO_NET_API
#endif
#endif

#അന്തര്ഭാവ "netrc.h"
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "creds.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "strcase.h"
#അന്തര്ഭാവ "curl_get_line.h"
#അന്തര്ഭാവ "curlx/win32-fopen.h"
#അന്തര്ഭാവ "curlx/strparse.h"


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

#define MAX_NETRC_വരി  16384
#define MAX_NETRC_FILE  (128 * 1024)
#define MAX_NETRC_TOKEN 4096

#define NETRC_DEBUG   0

/* convert a dynbuf call CURLcode error to a NETRCcode error */
#define curl2netrc(r)                                  \
  ((!(r)) ? NETRC_OK : (((r) == CURLE_OUT_OF_MEMORY) ? \
   NETRC_OUT_OF_MEMORY : NETRC_SYNTAX_ERROR))

തരനിർവചനം എണ്ണൽ {
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

ഘടന netrc_lexer {
  ഘടന Curl_easy *data;
  സ്ഥിരം അക്ഷരം *content;
  സ്ഥിരം അക്ഷരം *pos;
  ഘടന dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#എങ്കിൽ NETRC_DEBUG
സ്ഥിതം സ്ഥിരം അക്ഷരം *netrc_tokenstr(curl_netrc_token token)
{
  തിരഞ്ഞെടുപ്പ്(token) {
  അവസ്ഥ NETRC_TOK_EOF:
    തിരികെ "[EOF]";
  അവസ്ഥ NETRC_TOK_LITERAL:
    തിരികെ "[LITERAL]";
  അവസ്ഥ NETRC_TOK_MACHINE:
    തിരികെ "[MACHINE]";
  അവസ്ഥ NETRC_TOK_DEFAULT:
    തിരികെ "[DEFAULT]";
  അവസ്ഥ NETRC_TOK_ACCOUNT:
    തിരികെ "[ACCOUNT]";
  അവസ്ഥ NETRC_TOK_LOGIN:
    തിരികെ "[LOGIN]";
  അവസ്ഥ NETRC_TOK_PASSWD:
    തിരികെ "[PASSWORD]";
  അവസ്ഥ NETRC_TOK_MACDEF:
    തിരികെ "[MACDEF]";
  അവസ്ഥ NETRC_TOK_JUNK:
    തിരികെ "[JUNK]";
  സ്ഥിരസ്ഥിതി:
    തിരികെ "[???]";
  }
}
#endif

സ്ഥിതം ശൂന്യം netrc_lexer_init(ഘടന netrc_lexer *lexer,
                             ഘടന Curl_easy *data,
                             സ്ഥിരം അക്ഷരം *content)
{
  curlx_dyn_init(&lexer->literal, MAX_NETRC_TOKEN);
  lexer->data = data;
  lexer->content = lexer->pos = content;
}

സ്ഥിതം ശൂന്യം netrc_lexer_cleanup(ഘടന netrc_lexer *lexer)
{
  lexer->content = lexer->pos = ശൂന്യ;
  lexer->data = ശൂന്യ;
  curlx_dyn_free(&lexer->literal);
}

സ്ഥിതം ശൂന്യം netrc_skip_blanks(ഘടന netrc_lexer *lexer)
{
  സ്ഥിരം അക്ഷരം *s = lexer->pos;
  ആകുമ്പോൾ(*s) {
    curlx_str_passblanks(&s);
    ആകുമ്പോൾ(*s == '\r')
      ++s;
    എങ്കിൽ(*s == '\n') {
      ++s;
    }
    അല്ലെങ്കിൽ
      നിർത്തുക;
  }
  lexer->pos = s;
}

സ്ഥിതം ശൂന്യം netrc_skip_to_empty_വരി(ഘടന netrc_lexer *lexer)
{
  സ്ഥിരം അക്ഷരം *s = lexer->pos;
  ആകുമ്പോൾ(*s) {
    എങ്കിൽ(*s == '\r')
      ++s;
    അല്ലെങ്കിൽ എങ്കിൽ(*s == '\n') {
      ++s;
      ആകുമ്പോൾ(*s == '\r')
        ++s;
      എങ്കിൽ(*s == '\n')
        പോകുക out;
    }
    അല്ലെങ്കിൽ
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
സ്ഥിതം NETRCcode netrc_lexer_quoted(ഘടന netrc_lexer *lexer)
{
  NETRCcode rc = NETRC_SYNTAX_ERROR;
  സ്ഥിരം അക്ഷരം *s = lexer->pos;
  bool escape = FALSE;
  CURLcode result;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  ആകുമ്പോൾ(*s) {
    അക്ഷരം c = *s;
    എങ്കിൽ(escape) {
      escape = FALSE;
      തിരഞ്ഞെടുപ്പ്(c) {
      അവസ്ഥ 'n':
        c = '\n';
        നിർത്തുക;
      അവസ്ഥ 'r':
        c = '\r';
        നിർത്തുക;
      അവസ്ഥ 't':
        c = '\t';
        നിർത്തുക;
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(c == '\\') {
      escape = TRUE;
      ++s;
      തുടരുക;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      result = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(result);
      പോകുക out;
    }
    result = curlx_dyn_addn(&lexer->literal, &c, 1);
    എങ്കിൽ(result) {
      rc = curl2netrc(result);
      പോകുക out;
    }
    ++s;
  }
out:
  lexer->pos = s;
  തിരികെ rc;
}

സ്ഥിതം ശൂന്യം netrc_lexer_തള്ളുക(ഘടന netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

സ്ഥിതം NETRCcode netrc_lexer_next(ഘടന netrc_lexer *lexer,
                                  bool want_literal)
{
  സ്ഥിരം അക്ഷരം *s = lexer->pos, *start;
  NETRCcode rc = NETRC_OK;
  size_t slen;
  CURLcode result;

  എങ്കിൽ(lexer->pushed) {
    lexer->pushed = FALSE;
    പോകുക out;
  }

  curlx_dyn_reset(&lexer->literal);
  netrc_skip_blanks(lexer);
  s = lexer->pos;

  തിരഞ്ഞെടുപ്പ്(*s) {
  അവസ്ഥ 0:
    lexer->token = NETRC_TOK_EOF;
    നിർത്തുക;
  അവസ്ഥ '\"':
    rc = netrc_lexer_quoted(lexer);
    lexer->token = NETRC_TOK_LITERAL;
    s = lexer->pos;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    /* unquoted token */
    start = s;
    ആകുമ്പോൾ(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - start;
    എങ്കിൽ(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    എങ്കിൽ(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      result = curlx_dyn_addn(&lexer->literal, start, slen);
      rc = curl2netrc(result);
    }
    അല്ലെങ്കിൽ എങ്കിൽ((slen == 7) && curl_strnequal(start, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ((slen == 7) && curl_strnequal(start, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    അല്ലെങ്കിൽ എങ്കിൽ((slen == 7) && curl_strnequal(start, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    അല്ലെങ്കിൽ എങ്കിൽ((slen == 5) && curl_strnequal(start, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    അല്ലെങ്കിൽ എങ്കിൽ((slen == 8) && curl_strnequal(start, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    അല്ലെങ്കിൽ എങ്കിൽ((slen == 6) && curl_strnequal(start, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    അല്ലെങ്കിൽ {
      lexer->token = NETRC_TOK_JUNK;
    }
    നിർത്തുക;
  }

out:
#എങ്കിൽ NETRC_DEBUG
  CURL_TRC_M(lexer->data, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#endif
  lexer->pos = s;
  തിരികെ rc;
}

ഘടന netrc_scanner {
  ഘടന netrc_lexer lexer;
  സ്ഥിരം അക്ഷരം *hostname; /* non-NULL, machine to scan for */
  സ്ഥിരം അക്ഷരം *user; /* maybe NULL, login to scan for */
  അക്ഷരം *login;
  അക്ഷരം *passwd;
  ഘടന Curl_creds *creds;
  bool matches_host;
  bool found;
};

സ്ഥിതം ശൂന്യം netrc_scan_reset(ഘടന netrc_scanner *sc)
{
  curlx_safefree(sc->login);
  curlx_safefree(sc->passwd);
  sc->matches_host = FALSE;
}

സ്ഥിതം ശൂന്യം netrc_scan_init(ഘടന netrc_scanner *sc,
                            ഘടന Curl_easy *data,
                            സ്ഥിരം അക്ഷരം *content,
                            സ്ഥിരം അക്ഷരം *hostname,
                            സ്ഥിരം അക്ഷരം *user)
{
  memset(sc, 0, വലുപ്പം(*sc));
  netrc_lexer_init(&sc->lexer, data, content);
  sc->hostname = hostname;
  sc->user = (user && user[0]) ? user : ശൂന്യ;
  netrc_scan_reset(sc);
}

സ്ഥിതം ശൂന്യം netrc_scan_cleanup(ഘടന netrc_scanner *sc)
{
  netrc_scan_reset(sc);
  sc->hostname = ശൂന്യ;
  sc->user = ശൂന്യ;
  Curl_creds_unlink(&sc->creds);
  netrc_lexer_cleanup(&sc->lexer);
}

സ്ഥിതം NETRCcode netrc_scan_literal(ഘടന netrc_scanner *sc,
                                    അക്ഷരം **pdest)
{
  NETRCcode rc = netrc_lexer_next(&sc->lexer, TRUE);
  എങ്കിൽ(!rc) {
    എങ്കിൽ(sc->lexer.token == NETRC_TOK_LITERAL) {
      എങ്കിൽ(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        എങ്കിൽ(!*pdest)
          rc = NETRC_OUT_OF_MEMORY;
      }
    }
    അല്ലെങ്കിൽ
      netrc_lexer_തള്ളുക(&sc->lexer);
  }
  തിരികെ rc;
}

സ്ഥിതം NETRCcode netrc_scan_end_entry(ഘടന netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#എങ്കിൽ NETRC_DEBUG
  CURL_TRC_M(sc->lexer.data,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#endif
  എങ്കിൽ(sc->matches_host) {
    എങ്കിൽ(sc->login) {
      എങ്കിൽ(sc->user) {
        എങ്കിൽ(Curl_timestrcmp(sc->user, sc->login))
          പോകുക out;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->found = !!sc->passwd;
      }
      അല്ലെങ്കിൽ {
        sc->found = TRUE;
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->found = TRUE;
    }
    അല്ലെങ്കിൽ {
      /* entry has nothing interesting */
    }
    എങ്കിൽ(sc->found) {
#എങ്കിൽ NETRC_DEBUG
      CURL_TRC_M(sc->lexer.data, "[NETRC] entry match found");
#endif
      എങ്കിൽ(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           ശൂന്യ, ശൂന്യ, ശൂന്യ, CREDS_NETRC, &sc->creds))
        rc = NETRC_OUT_OF_MEMORY;
    }
  }
out:
  netrc_scan_reset(sc);
  തിരികെ rc;
}

സ്ഥിതം NETRCcode netrc_scan(ഘടന Curl_easy *data,
                            സ്ഥിരം അക്ഷരം *content,
                            സ്ഥിരം അക്ഷരം *hostname,
                            സ്ഥിരം അക്ഷരം *user,
                            ഘടന Curl_creds **pcreds)
{
  ഘടന netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_scan_init(&sc, data, content, hostname, user);

  ആകുമ്പോൾ(!rc && !sc.found) {
    rc = netrc_lexer_next(&sc.lexer, FALSE);
    എങ്കിൽ(!rc) {
      /* Does this token end any previous entry? */
      തിരഞ്ഞെടുപ്പ്(sc.lexer.token) {
      അവസ്ഥ NETRC_TOK_EOF:
      അവസ്ഥ NETRC_TOK_MACHINE:
      അവസ്ഥ NETRC_TOK_DEFAULT:
      അവസ്ഥ NETRC_TOK_MACDEF:
        rc = netrc_scan_end_entry(&sc);
        എങ്കിൽ(rc || sc.found)
          പോകുക out;
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി:
        നിർത്തുക;
      }

      തിരഞ്ഞെടുപ്പ്(sc.lexer.token) {
      അവസ്ഥ NETRC_TOK_EOF:
        പോകുക out;
      അവസ്ഥ NETRC_TOK_MACHINE:
        rc = netrc_lexer_next(&sc.lexer, TRUE);
        എങ്കിൽ(!rc) {
          എങ്കിൽ(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          അല്ലെങ്കിൽ {
            sc.matches_host = FALSE;
            netrc_lexer_തള്ളുക(&sc.lexer);
          }
        }
        നിർത്തുക;
      അവസ്ഥ NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        നിർത്തുക;
      അവസ്ഥ NETRC_TOK_ACCOUNT:
        rc = netrc_scan_literal(&sc, ശൂന്യ); /* ignore, not used */
        നിർത്തുക;
      അവസ്ഥ NETRC_TOK_LOGIN:
        rc = netrc_scan_literal(&sc, &sc.login);
        നിർത്തുക;
      അവസ്ഥ NETRC_TOK_PASSWD:
        rc = netrc_scan_literal(&sc, &sc.passwd);
        നിർത്തുക;
      അവസ്ഥ NETRC_TOK_MACDEF:
        netrc_skip_to_empty_വരി(&sc.lexer);
        നിർത്തുക;
      അവസ്ഥ NETRC_TOK_LITERAL:
      അവസ്ഥ NETRC_TOK_JUNK:
      സ്ഥിരസ്ഥിതി:
        /* skip this */
        നിർത്തുക;
      }
    }
  }

out:
  എങ്കിൽ(!rc) {
    എങ്കിൽ(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    അല്ലെങ്കിൽ
      rc = NETRC_NO_MATCH;
  }
  netrc_scan_cleanup(&sc);
  തിരികെ rc;
}

സ്ഥിതം NETRCcode file2memory(സ്ഥിരം അക്ഷരം *filename, ഘടന dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  എങ്കിൽ(file) {
    curlx_struct_stat stat;
    എങ്കിൽ((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      CURLcode result = CURLE_OK;
      bool eof;
      ഘടന dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_വരി);
      ret = NETRC_OK;
      ചെയ്യുക {
        സ്ഥിരം അക്ഷരം *വരി;
        /* Curl_get_line always returns lines ending with a newline */
        result = Curl_get_വരി(&linebuf, file, &eof);
        എങ്കിൽ(!result) {
          വരി = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&വരി);
          എങ്കിൽ(*വരി == '#')
            തുടരുക;
          result = curlx_dyn_add(filebuf, വരി);
        }
        എങ്കിൽ(result) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(result);
          നിർത്തുക;
        }
      } ആകുമ്പോൾ(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  തിരികെ ret;
}

സ്ഥിതം NETRCcode netrc_scan_file(ഘടന Curl_easy *data,
                                 ഘടന store_netrc *store,
                                 സ്ഥിരം അക്ഷരം *hostname,
                                 സ്ഥിരം അക്ഷരം *user,
                                 സ്ഥിരം അക്ഷരം *netrcfile,
                                 ഘടന Curl_creds **pcreds)
{
  ഘടന dynbuf *filebuf = &store->filebuf;

  എങ്കിൽ(!store->loaded || strcmp(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    എങ്കിൽ(ret) {
      CURL_TRC_M(data, "[NETRC] could not load '%s'", netrcfile);
      തിരികെ ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    എങ്കിൽ(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      തിരികെ NETRC_OUT_OF_MEMORY;
    }
    store->loaded = TRUE;
  }
  എങ്കിൽ(!curlx_dyn_len(filebuf))
    തിരികെ NETRC_NO_MATCH;

  തിരികെ netrc_scan(data, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_scan(ഘടന Curl_easy *data,
                          ഘടന store_netrc *store,
                          സ്ഥിരം അക്ഷരം *hostname,
                          സ്ഥിരം അക്ഷരം *user,
                          സ്ഥിരം അക്ഷരം *netrcfile,
                          ഘടന Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(data, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  എങ്കിൽ(!netrcfile) {
    അക്ഷരം *home = ശൂന്യ;
    അക്ഷരം *homea = ശൂന്യ;
    അക്ഷരം *filealloc = ശൂന്യ;
#എങ്കിൽ defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    അക്ഷരം pwbuf[1024];
#endif
    filealloc = curl_getenv("NETRC");
    എങ്കിൽ(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      എങ്കിൽ(homea) {
        home = homea;
#എങ്കിൽ defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      അല്ലെങ്കിൽ {
        ഘടന passwd pw, *pw_res;
        എങ്കിൽ(!getpwuid_r(geteuid(), &pw, pwbuf, വലുപ്പം(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      അല്ലെങ്കിൽ {
        ഘടന passwd *pw;
        pw = getpwuid(geteuid());
        എങ്കിൽ(pw) {
          home = pw->pw_dir;
        }
#അല്ലെങ്കിലെങ്കിൽ defined(_WIN32)
      }
      അല്ലെങ്കിൽ {
        homea = curl_getenv("USERPROFILE");
        എങ്കിൽ(homea) {
          home = homea;
        }
#endif
      }

      എങ്കിൽ(!home)
        തിരികെ NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      എങ്കിൽ(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_OUT_OF_MEMORY;
        പോകുക out;
      }
    }
    retcode = netrc_scan_file(
      data, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#ifdef _WIN32
    എങ്കിൽ(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      എങ്കിൽ(!filealloc) {
        curlx_free(homea);
        തിരികെ NETRC_OUT_OF_MEMORY;
      }
      retcode = netrc_scan_file(
        data, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#endif
    curlx_free(homea);
  }
  അല്ലെങ്കിൽ
    retcode = netrc_scan_file(
      data, store, hostname, user, netrcfile, pcreds);

out:
  എങ്കിൽ(retcode)
    Curl_creds_unlink(pcreds);
  തിരികെ retcode;
}

ശൂന്യം Curl_netrc_init(ഘടന store_netrc *store)
{
  curlx_dyn_init(&store->filebuf, MAX_NETRC_FILE);
  store->loaded = FALSE;
  store->filename = ശൂന്യ;
}
ശൂന്യം Curl_netrc_cleanup(ഘടന store_netrc *store)
{
  curlx_dyn_free(&store->filebuf);
  curlx_safefree(store->filename);
  store->loaded = FALSE;
}

സ്ഥിരം അക്ഷരം *Curl_netrc_strerror(NETRCcode ret)
{
  തിരഞ്ഞെടുപ്പ്(ret) {
  സ്ഥിരസ്ഥിതി:
    തിരികെ ""; /* not a legit error */
  അവസ്ഥ NETRC_FILE_MISSING:
    തിരികെ "no such file";
  അവസ്ഥ NETRC_NO_MATCH:
    തിരികെ "no matching entry";
  അവസ്ഥ NETRC_OUT_OF_MEMORY:
    തിരികെ "out of memory";
  അവസ്ഥ NETRC_SYNTAX_ERROR:
    തിരികെ "syntax error";
  }
  /* never reached */
}

#endif /* !CURL_DISABLE_NETRC */
