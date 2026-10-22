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
#அந்தர்பாவ "curl_setup.h"

#ifndef CURL_DISABLE_NETRC

#ifdef HAVE_PWD_H
#ifdef __AMIGA__
#undef __NO_NET_API /* required for AmigaOS to declare getpwuid() */
#endif
#அந்தர்பாவ <pwd.h>
#ifdef __AMIGA__
#define __NO_NET_API
#endif
#endif

#அந்தர்பாவ "netrc.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "creds.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "strcase.h"
#அந்தர்பாவ "curl_get_line.h"
#அந்தர்பாவ "curlx/win32-fopen.h"
#அந்தர்பாவ "curlx/strparse.h"


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

#define MAX_NETRC_வரி  16384
#define MAX_NETRC_FILE  (128 * 1024)
#define MAX_NETRC_TOKEN 4096

#define NETRC_DEBUG   0

/* convert a dynbuf call CURLcode error to a NETRCcode error */
#define curl2netrc(r)                                  \
  ((!(r)) ? NETRC_OK : (((r) == CURLE_OUT_OF_MEMORY) ? \
   NETRC_OUT_OF_MEMORY : NETRC_SYNTAX_ERROR))

வகைவரை எண்ணிக்கை {
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

கட்டமைப்பு netrc_lexer {
  கட்டமைப்பு Curl_easy *data;
  மாறிலி எழுத்து *content;
  மாறிலி எழுத்து *pos;
  கட்டமைப்பு dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#எனில் NETRC_DEBUG
நிலையான மாறிலி எழுத்து *netrc_tokenstr(curl_netrc_token token)
{
  தேர்வு(token) {
  நிலை NETRC_TOK_EOF:
    திரும்பு "[EOF]";
  நிலை NETRC_TOK_LITERAL:
    திரும்பு "[LITERAL]";
  நிலை NETRC_TOK_MACHINE:
    திரும்பு "[MACHINE]";
  நிலை NETRC_TOK_DEFAULT:
    திரும்பு "[DEFAULT]";
  நிலை NETRC_TOK_ACCOUNT:
    திரும்பு "[ACCOUNT]";
  நிலை NETRC_TOK_LOGIN:
    திரும்பு "[LOGIN]";
  நிலை NETRC_TOK_PASSWD:
    திரும்பு "[PASSWORD]";
  நிலை NETRC_TOK_MACDEF:
    திரும்பு "[MACDEF]";
  நிலை NETRC_TOK_JUNK:
    திரும்பு "[JUNK]";
  இயல்பு:
    திரும்பு "[???]";
  }
}
#endif

நிலையான வெறுமை netrc_lexer_init(கட்டமைப்பு netrc_lexer *lexer,
                             கட்டமைப்பு Curl_easy *data,
                             மாறிலி எழுத்து *content)
{
  curlx_dyn_init(&lexer->literal, MAX_NETRC_TOKEN);
  lexer->data = data;
  lexer->content = lexer->pos = content;
}

நிலையான வெறுமை netrc_lexer_cleanup(கட்டமைப்பு netrc_lexer *lexer)
{
  lexer->content = lexer->pos = வெற்று;
  lexer->data = வெற்று;
  curlx_dyn_free(&lexer->literal);
}

நிலையான வெறுமை netrc_skip_blanks(கட்டமைப்பு netrc_lexer *lexer)
{
  மாறிலி எழுத்து *s = lexer->pos;
  போது(*s) {
    curlx_str_passblanks(&s);
    போது(*s == '\r')
      ++s;
    எனில்(*s == '\n') {
      ++s;
    }
    இல்லையெனில்
      நிறுத்து;
  }
  lexer->pos = s;
}

நிலையான வெறுமை netrc_skip_to_empty_வரி(கட்டமைப்பு netrc_lexer *lexer)
{
  மாறிலி எழுத்து *s = lexer->pos;
  போது(*s) {
    எனில்(*s == '\r')
      ++s;
    இல்லையெனில் எனில்(*s == '\n') {
      ++s;
      போது(*s == '\r')
        ++s;
      எனில்(*s == '\n')
        செல் out;
    }
    இல்லையெனில்
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
நிலையான NETRCcode netrc_lexer_quoted(கட்டமைப்பு netrc_lexer *lexer)
{
  NETRCcode rc = NETRC_SYNTAX_ERROR;
  மாறிலி எழுத்து *s = lexer->pos;
  bool escape = FALSE;
  CURLcode result;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  போது(*s) {
    எழுத்து c = *s;
    எனில்(escape) {
      escape = FALSE;
      தேர்வு(c) {
      நிலை 'n':
        c = '\n';
        நிறுத்து;
      நிலை 'r':
        c = '\r';
        நிறுத்து;
      நிலை 't':
        c = '\t';
        நிறுத்து;
      }
    }
    இல்லையெனில் எனில்(c == '\\') {
      escape = TRUE;
      ++s;
      தொடர்;
    }
    இல்லையெனில் எனில்(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      result = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(result);
      செல் out;
    }
    result = curlx_dyn_addn(&lexer->literal, &c, 1);
    எனில்(result) {
      rc = curl2netrc(result);
      செல் out;
    }
    ++s;
  }
out:
  lexer->pos = s;
  திரும்பு rc;
}

நிலையான வெறுமை netrc_lexer_தள்ளு(கட்டமைப்பு netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

நிலையான NETRCcode netrc_lexer_next(கட்டமைப்பு netrc_lexer *lexer,
                                  bool want_literal)
{
  மாறிலி எழுத்து *s = lexer->pos, *start;
  NETRCcode rc = NETRC_OK;
  size_t slen;
  CURLcode result;

  எனில்(lexer->pushed) {
    lexer->pushed = FALSE;
    செல் out;
  }

  curlx_dyn_reset(&lexer->literal);
  netrc_skip_blanks(lexer);
  s = lexer->pos;

  தேர்வு(*s) {
  நிலை 0:
    lexer->token = NETRC_TOK_EOF;
    நிறுத்து;
  நிலை '\"':
    rc = netrc_lexer_quoted(lexer);
    lexer->token = NETRC_TOK_LITERAL;
    s = lexer->pos;
    நிறுத்து;
  இயல்பு:
    /* unquoted token */
    start = s;
    போது(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - start;
    எனில்(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    எனில்(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      result = curlx_dyn_addn(&lexer->literal, start, slen);
      rc = curl2netrc(result);
    }
    இல்லையெனில் எனில்((slen == 7) && curl_strnequal(start, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    இல்லையெனில் எனில்((slen == 7) && curl_strnequal(start, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    இல்லையெனில் எனில்((slen == 7) && curl_strnequal(start, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    இல்லையெனில் எனில்((slen == 5) && curl_strnequal(start, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    இல்லையெனில் எனில்((slen == 8) && curl_strnequal(start, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    இல்லையெனில் எனில்((slen == 6) && curl_strnequal(start, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    இல்லையெனில் {
      lexer->token = NETRC_TOK_JUNK;
    }
    நிறுத்து;
  }

out:
#எனில் NETRC_DEBUG
  CURL_TRC_M(lexer->data, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#endif
  lexer->pos = s;
  திரும்பு rc;
}

கட்டமைப்பு netrc_scanner {
  கட்டமைப்பு netrc_lexer lexer;
  மாறிலி எழுத்து *hostname; /* non-NULL, machine to scan for */
  மாறிலி எழுத்து *user; /* maybe NULL, login to scan for */
  எழுத்து *login;
  எழுத்து *passwd;
  கட்டமைப்பு Curl_creds *creds;
  bool matches_host;
  bool found;
};

நிலையான வெறுமை netrc_scan_reset(கட்டமைப்பு netrc_scanner *sc)
{
  curlx_safefree(sc->login);
  curlx_safefree(sc->passwd);
  sc->matches_host = FALSE;
}

நிலையான வெறுமை netrc_scan_init(கட்டமைப்பு netrc_scanner *sc,
                            கட்டமைப்பு Curl_easy *data,
                            மாறிலி எழுத்து *content,
                            மாறிலி எழுத்து *hostname,
                            மாறிலி எழுத்து *user)
{
  memset(sc, 0, அளவு(*sc));
  netrc_lexer_init(&sc->lexer, data, content);
  sc->hostname = hostname;
  sc->user = (user && user[0]) ? user : வெற்று;
  netrc_scan_reset(sc);
}

நிலையான வெறுமை netrc_scan_cleanup(கட்டமைப்பு netrc_scanner *sc)
{
  netrc_scan_reset(sc);
  sc->hostname = வெற்று;
  sc->user = வெற்று;
  Curl_creds_unlink(&sc->creds);
  netrc_lexer_cleanup(&sc->lexer);
}

நிலையான NETRCcode netrc_scan_literal(கட்டமைப்பு netrc_scanner *sc,
                                    எழுத்து **pdest)
{
  NETRCcode rc = netrc_lexer_next(&sc->lexer, TRUE);
  எனில்(!rc) {
    எனில்(sc->lexer.token == NETRC_TOK_LITERAL) {
      எனில்(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        எனில்(!*pdest)
          rc = NETRC_OUT_OF_MEMORY;
      }
    }
    இல்லையெனில்
      netrc_lexer_தள்ளு(&sc->lexer);
  }
  திரும்பு rc;
}

நிலையான NETRCcode netrc_scan_end_entry(கட்டமைப்பு netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#எனில் NETRC_DEBUG
  CURL_TRC_M(sc->lexer.data,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#endif
  எனில்(sc->matches_host) {
    எனில்(sc->login) {
      எனில்(sc->user) {
        எனில்(Curl_timestrcmp(sc->user, sc->login))
          செல் out;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->found = !!sc->passwd;
      }
      இல்லையெனில் {
        sc->found = TRUE;
      }
    }
    இல்லையெனில் எனில்(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->found = TRUE;
    }
    இல்லையெனில் {
      /* entry has nothing interesting */
    }
    எனில்(sc->found) {
#எனில் NETRC_DEBUG
      CURL_TRC_M(sc->lexer.data, "[NETRC] entry match found");
#endif
      எனில்(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           வெற்று, வெற்று, வெற்று, CREDS_NETRC, &sc->creds))
        rc = NETRC_OUT_OF_MEMORY;
    }
  }
out:
  netrc_scan_reset(sc);
  திரும்பு rc;
}

நிலையான NETRCcode netrc_scan(கட்டமைப்பு Curl_easy *data,
                            மாறிலி எழுத்து *content,
                            மாறிலி எழுத்து *hostname,
                            மாறிலி எழுத்து *user,
                            கட்டமைப்பு Curl_creds **pcreds)
{
  கட்டமைப்பு netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_scan_init(&sc, data, content, hostname, user);

  போது(!rc && !sc.found) {
    rc = netrc_lexer_next(&sc.lexer, FALSE);
    எனில்(!rc) {
      /* Does this token end any previous entry? */
      தேர்வு(sc.lexer.token) {
      நிலை NETRC_TOK_EOF:
      நிலை NETRC_TOK_MACHINE:
      நிலை NETRC_TOK_DEFAULT:
      நிலை NETRC_TOK_MACDEF:
        rc = netrc_scan_end_entry(&sc);
        எனில்(rc || sc.found)
          செல் out;
        நிறுத்து;
      இயல்பு:
        நிறுத்து;
      }

      தேர்வு(sc.lexer.token) {
      நிலை NETRC_TOK_EOF:
        செல் out;
      நிலை NETRC_TOK_MACHINE:
        rc = netrc_lexer_next(&sc.lexer, TRUE);
        எனில்(!rc) {
          எனில்(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          இல்லையெனில் {
            sc.matches_host = FALSE;
            netrc_lexer_தள்ளு(&sc.lexer);
          }
        }
        நிறுத்து;
      நிலை NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        நிறுத்து;
      நிலை NETRC_TOK_ACCOUNT:
        rc = netrc_scan_literal(&sc, வெற்று); /* ignore, not used */
        நிறுத்து;
      நிலை NETRC_TOK_LOGIN:
        rc = netrc_scan_literal(&sc, &sc.login);
        நிறுத்து;
      நிலை NETRC_TOK_PASSWD:
        rc = netrc_scan_literal(&sc, &sc.passwd);
        நிறுத்து;
      நிலை NETRC_TOK_MACDEF:
        netrc_skip_to_empty_வரி(&sc.lexer);
        நிறுத்து;
      நிலை NETRC_TOK_LITERAL:
      நிலை NETRC_TOK_JUNK:
      இயல்பு:
        /* skip this */
        நிறுத்து;
      }
    }
  }

out:
  எனில்(!rc) {
    எனில்(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    இல்லையெனில்
      rc = NETRC_NO_MATCH;
  }
  netrc_scan_cleanup(&sc);
  திரும்பு rc;
}

நிலையான NETRCcode file2memory(மாறிலி எழுத்து *filename, கட்டமைப்பு dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  எனில்(file) {
    curlx_struct_stat stat;
    எனில்((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      CURLcode result = CURLE_OK;
      bool eof;
      கட்டமைப்பு dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_வரி);
      ret = NETRC_OK;
      செய் {
        மாறிலி எழுத்து *வரி;
        /* Curl_get_line always returns lines ending with a newline */
        result = Curl_get_வரி(&linebuf, file, &eof);
        எனில்(!result) {
          வரி = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&வரி);
          எனில்(*வரி == '#')
            தொடர்;
          result = curlx_dyn_add(filebuf, வரி);
        }
        எனில்(result) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(result);
          நிறுத்து;
        }
      } போது(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  திரும்பு ret;
}

நிலையான NETRCcode netrc_scan_file(கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு store_netrc *store,
                                 மாறிலி எழுத்து *hostname,
                                 மாறிலி எழுத்து *user,
                                 மாறிலி எழுத்து *netrcfile,
                                 கட்டமைப்பு Curl_creds **pcreds)
{
  கட்டமைப்பு dynbuf *filebuf = &store->filebuf;

  எனில்(!store->loaded || strcmp(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    எனில்(ret) {
      CURL_TRC_M(data, "[NETRC] could not load '%s'", netrcfile);
      திரும்பு ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    எனில்(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      திரும்பு NETRC_OUT_OF_MEMORY;
    }
    store->loaded = TRUE;
  }
  எனில்(!curlx_dyn_len(filebuf))
    திரும்பு NETRC_NO_MATCH;

  திரும்பு netrc_scan(data, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_scan(கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு store_netrc *store,
                          மாறிலி எழுத்து *hostname,
                          மாறிலி எழுத்து *user,
                          மாறிலி எழுத்து *netrcfile,
                          கட்டமைப்பு Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(data, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  எனில்(!netrcfile) {
    எழுத்து *home = வெற்று;
    எழுத்து *homea = வெற்று;
    எழுத்து *filealloc = வெற்று;
#எனில் defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    எழுத்து pwbuf[1024];
#endif
    filealloc = curl_getenv("NETRC");
    எனில்(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      எனில்(homea) {
        home = homea;
#எனில் defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      இல்லையெனில் {
        கட்டமைப்பு passwd pw, *pw_res;
        எனில்(!getpwuid_r(geteuid(), &pw, pwbuf, அளவு(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#அல்லதெனில் defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      இல்லையெனில் {
        கட்டமைப்பு passwd *pw;
        pw = getpwuid(geteuid());
        எனில்(pw) {
          home = pw->pw_dir;
        }
#அல்லதெனில் defined(_WIN32)
      }
      இல்லையெனில் {
        homea = curl_getenv("USERPROFILE");
        எனில்(homea) {
          home = homea;
        }
#endif
      }

      எனில்(!home)
        திரும்பு NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      எனில்(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_OUT_OF_MEMORY;
        செல் out;
      }
    }
    retcode = netrc_scan_file(
      data, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#ifdef _WIN32
    எனில்(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      எனில்(!filealloc) {
        curlx_free(homea);
        திரும்பு NETRC_OUT_OF_MEMORY;
      }
      retcode = netrc_scan_file(
        data, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#endif
    curlx_free(homea);
  }
  இல்லையெனில்
    retcode = netrc_scan_file(
      data, store, hostname, user, netrcfile, pcreds);

out:
  எனில்(retcode)
    Curl_creds_unlink(pcreds);
  திரும்பு retcode;
}

வெறுமை Curl_netrc_init(கட்டமைப்பு store_netrc *store)
{
  curlx_dyn_init(&store->filebuf, MAX_NETRC_FILE);
  store->loaded = FALSE;
  store->filename = வெற்று;
}
வெறுமை Curl_netrc_cleanup(கட்டமைப்பு store_netrc *store)
{
  curlx_dyn_free(&store->filebuf);
  curlx_safefree(store->filename);
  store->loaded = FALSE;
}

மாறிலி எழுத்து *Curl_netrc_strerror(NETRCcode ret)
{
  தேர்வு(ret) {
  இயல்பு:
    திரும்பு ""; /* not a legit error */
  நிலை NETRC_FILE_MISSING:
    திரும்பு "no such file";
  நிலை NETRC_NO_MATCH:
    திரும்பு "no matching entry";
  நிலை NETRC_OUT_OF_MEMORY:
    திரும்பு "out of memory";
  நிலை NETRC_SYNTAX_ERROR:
    திரும்பு "syntax error";
  }
  /* never reached */
}

#endif /* !CURL_DISABLE_NETRC */
