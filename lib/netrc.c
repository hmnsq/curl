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
#অন্তর্ভাব "curl_setup.h"

#ifndef CURL_DISABLE_NETRC

#ifdef HAVE_PWD_H
#ifdef __AMIGA__
#undef __NO_NET_API /* required for AmigaOS to declare getpwuid() */
#endif
#অন্তর্ভাব <pwd.h>
#ifdef __AMIGA__
#define __NO_NET_API
#endif
#endif

#অন্তর্ভাব "netrc.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "creds.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "curl_get_line.h"
#অন্তর্ভাব "curlx/win32-fopen.h"
#অন্তর্ভাব "curlx/strparse.h"


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

#define MAX_NETRC_সারি  16384
#define MAX_NETRC_FILE  (128 * 1024)
#define MAX_NETRC_TOKEN 4096

#define NETRC_DEBUG   0

/* convert a dynbuf call CURLcode error to a NETRCcode error */
#define curl2netrc(r)                                  \
  ((!(r)) ? NETRC_OK : (((r) == CURLE_OUT_OF_MEMORY) ? \
   NETRC_OUT_OF_MEMORY : NETRC_SYNTAX_ERROR))

প্রকারনির্দেশ গণনা {
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

সংরচনা netrc_lexer {
  সংরচনা Curl_easy *data;
  ধ্রুব বর্ণ *content;
  ধ্রুব বর্ণ *pos;
  সংরচনা dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#যদি NETRC_DEBUG
স্থির ধ্রুব বর্ণ *netrc_tokenstr(curl_netrc_token token)
{
  বিকল্প(token) {
  পক্ষ NETRC_TOK_EOF:
    প্রত্যাগম "[EOF]";
  পক্ষ NETRC_TOK_LITERAL:
    প্রত্যাগম "[LITERAL]";
  পক্ষ NETRC_TOK_MACHINE:
    প্রত্যাগম "[MACHINE]";
  পক্ষ NETRC_TOK_DEFAULT:
    প্রত্যাগম "[DEFAULT]";
  পক্ষ NETRC_TOK_ACCOUNT:
    প্রত্যাগম "[ACCOUNT]";
  পক্ষ NETRC_TOK_LOGIN:
    প্রত্যাগম "[LOGIN]";
  পক্ষ NETRC_TOK_PASSWD:
    প্রত্যাগম "[PASSWORD]";
  পক্ষ NETRC_TOK_MACDEF:
    প্রত্যাগম "[MACDEF]";
  পক্ষ NETRC_TOK_JUNK:
    প্রত্যাগম "[JUNK]";
  স্বভাব:
    প্রত্যাগম "[???]";
  }
}
#endif

স্থির শূন্য netrc_lexer_init(সংরচনা netrc_lexer *lexer,
                             সংরচনা Curl_easy *data,
                             ধ্রুব বর্ণ *content)
{
  curlx_dyn_init(&lexer->literal, MAX_NETRC_TOKEN);
  lexer->data = data;
  lexer->content = lexer->pos = content;
}

স্থির শূন্য netrc_lexer_cleanup(সংরচনা netrc_lexer *lexer)
{
  lexer->content = lexer->pos = রিক্ত;
  lexer->data = রিক্ত;
  curlx_dyn_free(&lexer->literal);
}

স্থির শূন্য netrc_skip_blanks(সংরচনা netrc_lexer *lexer)
{
  ধ্রুব বর্ণ *s = lexer->pos;
  যাবত্(*s) {
    curlx_str_passblanks(&s);
    যাবত্(*s == '\r')
      ++s;
    যদি(*s == '\n') {
      ++s;
    }
    অন্যথা
      ভঙ্গ;
  }
  lexer->pos = s;
}

স্থির শূন্য netrc_skip_to_empty_সারি(সংরচনা netrc_lexer *lexer)
{
  ধ্রুব বর্ণ *s = lexer->pos;
  যাবত্(*s) {
    যদি(*s == '\r')
      ++s;
    অন্যথা যদি(*s == '\n') {
      ++s;
      যাবত্(*s == '\r')
        ++s;
      যদি(*s == '\n')
        গচ্ছ out;
    }
    অন্যথা
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
স্থির NETRCcode netrc_lexer_quoted(সংরচনা netrc_lexer *lexer)
{
  NETRCcode rc = NETRC_SYNTAX_ERROR;
  ধ্রুব বর্ণ *s = lexer->pos;
  bool escape = FALSE;
  CURLcode result;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  যাবত্(*s) {
    বর্ণ c = *s;
    যদি(escape) {
      escape = FALSE;
      বিকল্প(c) {
      পক্ষ 'n':
        c = '\n';
        ভঙ্গ;
      পক্ষ 'r':
        c = '\r';
        ভঙ্গ;
      পক্ষ 't':
        c = '\t';
        ভঙ্গ;
      }
    }
    অন্যথা যদি(c == '\\') {
      escape = TRUE;
      ++s;
      অনুবর্ত;
    }
    অন্যথা যদি(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      result = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(result);
      গচ্ছ out;
    }
    result = curlx_dyn_addn(&lexer->literal, &c, 1);
    যদি(result) {
      rc = curl2netrc(result);
      গচ্ছ out;
    }
    ++s;
  }
out:
  lexer->pos = s;
  প্রত্যাগম rc;
}

স্থির শূন্য netrc_lexer_ঠেলো(সংরচনা netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

স্থির NETRCcode netrc_lexer_next(সংরচনা netrc_lexer *lexer,
                                  bool want_literal)
{
  ধ্রুব বর্ণ *s = lexer->pos, *start;
  NETRCcode rc = NETRC_OK;
  size_t slen;
  CURLcode result;

  যদি(lexer->pushed) {
    lexer->pushed = FALSE;
    গচ্ছ out;
  }

  curlx_dyn_reset(&lexer->literal);
  netrc_skip_blanks(lexer);
  s = lexer->pos;

  বিকল্প(*s) {
  পক্ষ 0:
    lexer->token = NETRC_TOK_EOF;
    ভঙ্গ;
  পক্ষ '\"':
    rc = netrc_lexer_quoted(lexer);
    lexer->token = NETRC_TOK_LITERAL;
    s = lexer->pos;
    ভঙ্গ;
  স্বভাব:
    /* unquoted token */
    start = s;
    যাবত্(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - start;
    যদি(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    যদি(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      result = curlx_dyn_addn(&lexer->literal, start, slen);
      rc = curl2netrc(result);
    }
    অন্যথা যদি((slen == 7) && curl_strnequal(start, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    অন্যথা যদি((slen == 7) && curl_strnequal(start, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    অন্যথা যদি((slen == 7) && curl_strnequal(start, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    অন্যথা যদি((slen == 5) && curl_strnequal(start, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    অন্যথা যদি((slen == 8) && curl_strnequal(start, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    অন্যথা যদি((slen == 6) && curl_strnequal(start, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    অন্যথা {
      lexer->token = NETRC_TOK_JUNK;
    }
    ভঙ্গ;
  }

out:
#যদি NETRC_DEBUG
  CURL_TRC_M(lexer->data, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#endif
  lexer->pos = s;
  প্রত্যাগম rc;
}

সংরচনা netrc_scanner {
  সংরচনা netrc_lexer lexer;
  ধ্রুব বর্ণ *hostname; /* non-NULL, machine to scan for */
  ধ্রুব বর্ণ *user; /* maybe NULL, login to scan for */
  বর্ণ *login;
  বর্ণ *passwd;
  সংরচনা Curl_creds *creds;
  bool matches_host;
  bool found;
};

স্থির শূন্য netrc_scan_reset(সংরচনা netrc_scanner *sc)
{
  curlx_safefree(sc->login);
  curlx_safefree(sc->passwd);
  sc->matches_host = FALSE;
}

স্থির শূন্য netrc_scan_init(সংরচনা netrc_scanner *sc,
                            সংরচনা Curl_easy *data,
                            ধ্রুব বর্ণ *content,
                            ধ্রুব বর্ণ *hostname,
                            ধ্রুব বর্ণ *user)
{
  memset(sc, 0, পরিমাণ(*sc));
  netrc_lexer_init(&sc->lexer, data, content);
  sc->hostname = hostname;
  sc->user = (user && user[0]) ? user : রিক্ত;
  netrc_scan_reset(sc);
}

স্থির শূন্য netrc_scan_cleanup(সংরচনা netrc_scanner *sc)
{
  netrc_scan_reset(sc);
  sc->hostname = রিক্ত;
  sc->user = রিক্ত;
  Curl_creds_unlink(&sc->creds);
  netrc_lexer_cleanup(&sc->lexer);
}

স্থির NETRCcode netrc_scan_literal(সংরচনা netrc_scanner *sc,
                                    বর্ণ **pdest)
{
  NETRCcode rc = netrc_lexer_next(&sc->lexer, TRUE);
  যদি(!rc) {
    যদি(sc->lexer.token == NETRC_TOK_LITERAL) {
      যদি(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        যদি(!*pdest)
          rc = NETRC_OUT_OF_MEMORY;
      }
    }
    অন্যথা
      netrc_lexer_ঠেলো(&sc->lexer);
  }
  প্রত্যাগম rc;
}

স্থির NETRCcode netrc_scan_end_entry(সংরচনা netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#যদি NETRC_DEBUG
  CURL_TRC_M(sc->lexer.data,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#endif
  যদি(sc->matches_host) {
    যদি(sc->login) {
      যদি(sc->user) {
        যদি(Curl_timestrcmp(sc->user, sc->login))
          গচ্ছ out;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->found = !!sc->passwd;
      }
      অন্যথা {
        sc->found = TRUE;
      }
    }
    অন্যথা যদি(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->found = TRUE;
    }
    অন্যথা {
      /* entry has nothing interesting */
    }
    যদি(sc->found) {
#যদি NETRC_DEBUG
      CURL_TRC_M(sc->lexer.data, "[NETRC] entry match found");
#endif
      যদি(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           রিক্ত, রিক্ত, রিক্ত, CREDS_NETRC, &sc->creds))
        rc = NETRC_OUT_OF_MEMORY;
    }
  }
out:
  netrc_scan_reset(sc);
  প্রত্যাগম rc;
}

স্থির NETRCcode netrc_scan(সংরচনা Curl_easy *data,
                            ধ্রুব বর্ণ *content,
                            ধ্রুব বর্ণ *hostname,
                            ধ্রুব বর্ণ *user,
                            সংরচনা Curl_creds **pcreds)
{
  সংরচনা netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_scan_init(&sc, data, content, hostname, user);

  যাবত্(!rc && !sc.found) {
    rc = netrc_lexer_next(&sc.lexer, FALSE);
    যদি(!rc) {
      /* Does this token end any previous entry? */
      বিকল্প(sc.lexer.token) {
      পক্ষ NETRC_TOK_EOF:
      পক্ষ NETRC_TOK_MACHINE:
      পক্ষ NETRC_TOK_DEFAULT:
      পক্ষ NETRC_TOK_MACDEF:
        rc = netrc_scan_end_entry(&sc);
        যদি(rc || sc.found)
          গচ্ছ out;
        ভঙ্গ;
      স্বভাব:
        ভঙ্গ;
      }

      বিকল্প(sc.lexer.token) {
      পক্ষ NETRC_TOK_EOF:
        গচ্ছ out;
      পক্ষ NETRC_TOK_MACHINE:
        rc = netrc_lexer_next(&sc.lexer, TRUE);
        যদি(!rc) {
          যদি(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          অন্যথা {
            sc.matches_host = FALSE;
            netrc_lexer_ঠেলো(&sc.lexer);
          }
        }
        ভঙ্গ;
      পক্ষ NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        ভঙ্গ;
      পক্ষ NETRC_TOK_ACCOUNT:
        rc = netrc_scan_literal(&sc, রিক্ত); /* ignore, not used */
        ভঙ্গ;
      পক্ষ NETRC_TOK_LOGIN:
        rc = netrc_scan_literal(&sc, &sc.login);
        ভঙ্গ;
      পক্ষ NETRC_TOK_PASSWD:
        rc = netrc_scan_literal(&sc, &sc.passwd);
        ভঙ্গ;
      পক্ষ NETRC_TOK_MACDEF:
        netrc_skip_to_empty_সারি(&sc.lexer);
        ভঙ্গ;
      পক্ষ NETRC_TOK_LITERAL:
      পক্ষ NETRC_TOK_JUNK:
      স্বভাব:
        /* skip this */
        ভঙ্গ;
      }
    }
  }

out:
  যদি(!rc) {
    যদি(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    অন্যথা
      rc = NETRC_NO_MATCH;
  }
  netrc_scan_cleanup(&sc);
  প্রত্যাগম rc;
}

স্থির NETRCcode file2memory(ধ্রুব বর্ণ *filename, সংরচনা dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  যদি(file) {
    curlx_struct_stat stat;
    যদি((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      CURLcode result = CURLE_OK;
      bool eof;
      সংরচনা dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_সারি);
      ret = NETRC_OK;
      কুরু {
        ধ্রুব বর্ণ *সারি;
        /* Curl_get_line always returns lines ending with a newline */
        result = Curl_get_সারি(&linebuf, file, &eof);
        যদি(!result) {
          সারি = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&সারি);
          যদি(*সারি == '#')
            অনুবর্ত;
          result = curlx_dyn_add(filebuf, সারি);
        }
        যদি(result) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(result);
          ভঙ্গ;
        }
      } যাবত্(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  প্রত্যাগম ret;
}

স্থির NETRCcode netrc_scan_file(সংরচনা Curl_easy *data,
                                 সংরচনা store_netrc *store,
                                 ধ্রুব বর্ণ *hostname,
                                 ধ্রুব বর্ণ *user,
                                 ধ্রুব বর্ণ *netrcfile,
                                 সংরচনা Curl_creds **pcreds)
{
  সংরচনা dynbuf *filebuf = &store->filebuf;

  যদি(!store->loaded || strcmp(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    যদি(ret) {
      CURL_TRC_M(data, "[NETRC] could not load '%s'", netrcfile);
      প্রত্যাগম ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    যদি(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      প্রত্যাগম NETRC_OUT_OF_MEMORY;
    }
    store->loaded = TRUE;
  }
  যদি(!curlx_dyn_len(filebuf))
    প্রত্যাগম NETRC_NO_MATCH;

  প্রত্যাগম netrc_scan(data, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_scan(সংরচনা Curl_easy *data,
                          সংরচনা store_netrc *store,
                          ধ্রুব বর্ণ *hostname,
                          ধ্রুব বর্ণ *user,
                          ধ্রুব বর্ণ *netrcfile,
                          সংরচনা Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(data, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  যদি(!netrcfile) {
    বর্ণ *home = রিক্ত;
    বর্ণ *homea = রিক্ত;
    বর্ণ *filealloc = রিক্ত;
#যদি defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    বর্ণ pwbuf[1024];
#endif
    filealloc = curl_getenv("NETRC");
    যদি(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      যদি(homea) {
        home = homea;
#যদি defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      অন্যথা {
        সংরচনা passwd pw, *pw_res;
        যদি(!getpwuid_r(geteuid(), &pw, pwbuf, পরিমাণ(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#নোচেদ্যদি defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      অন্যথা {
        সংরচনা passwd *pw;
        pw = getpwuid(geteuid());
        যদি(pw) {
          home = pw->pw_dir;
        }
#নোচেদ্যদি defined(_WIN32)
      }
      অন্যথা {
        homea = curl_getenv("USERPROFILE");
        যদি(homea) {
          home = homea;
        }
#endif
      }

      যদি(!home)
        প্রত্যাগম NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      যদি(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_OUT_OF_MEMORY;
        গচ্ছ out;
      }
    }
    retcode = netrc_scan_file(
      data, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#ifdef _WIN32
    যদি(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      যদি(!filealloc) {
        curlx_free(homea);
        প্রত্যাগম NETRC_OUT_OF_MEMORY;
      }
      retcode = netrc_scan_file(
        data, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#endif
    curlx_free(homea);
  }
  অন্যথা
    retcode = netrc_scan_file(
      data, store, hostname, user, netrcfile, pcreds);

out:
  যদি(retcode)
    Curl_creds_unlink(pcreds);
  প্রত্যাগম retcode;
}

শূন্য Curl_netrc_init(সংরচনা store_netrc *store)
{
  curlx_dyn_init(&store->filebuf, MAX_NETRC_FILE);
  store->loaded = FALSE;
  store->filename = রিক্ত;
}
শূন্য Curl_netrc_cleanup(সংরচনা store_netrc *store)
{
  curlx_dyn_free(&store->filebuf);
  curlx_safefree(store->filename);
  store->loaded = FALSE;
}

ধ্রুব বর্ণ *Curl_netrc_strerror(NETRCcode ret)
{
  বিকল্প(ret) {
  স্বভাব:
    প্রত্যাগম ""; /* not a legit error */
  পক্ষ NETRC_FILE_MISSING:
    প্রত্যাগম "no such file";
  পক্ষ NETRC_NO_MATCH:
    প্রত্যাগম "no matching entry";
  পক্ষ NETRC_OUT_OF_MEMORY:
    প্রত্যাগম "out of memory";
  পক্ষ NETRC_SYNTAX_ERROR:
    প্রত্যাগম "syntax error";
  }
  /* never reached */
}

#endif /* !CURL_DISABLE_NETRC */
