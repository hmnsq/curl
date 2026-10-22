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
#شامل "curl_setup.h"

#ifndef CURL_DISABLE_NETRC

#ifdef HAVE_PWD_H
#ifdef __AMIGA__
#undef __NO_NET_API /* required for AmigaOS to declare getpwuid() */
#endif
#شامل <pwd.h>
#ifdef __AMIGA__
#define __NO_NET_API
#endif
#endif

#شامل "netrc.h"
#شامل "urldata.h"
#شامل "creds.h"
#شامل "curl_trc.h"
#شامل "strcase.h"
#شامل "curl_get_line.h"
#شامل "curlx/win32-fopen.h"
#شامل "curlx/strparse.h"


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

#define MAX_NETRC_سطر  16384
#define MAX_NETRC_FILE  (128 * 1024)
#define MAX_NETRC_TOKEN 4096

#define NETRC_DEBUG   0

/* convert a dynbuf call CURLcode error to a NETRCcode error */
#define curl2netrc(r)                                  \
  ((!(r)) ? NETRC_OK : (((r) == CURLE_OUT_OF_MEMORY) ? \
   NETRC_OUT_OF_MEMORY : NETRC_SYNTAX_ERROR))

تعریفنوع شمارش {
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

ساختار netrc_lexer {
  ساختار Curl_easy *data;
  ثابت نویسه *content;
  ثابت نویسه *pos;
  ساختار dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#اگر NETRC_DEBUG
ایستا ثابت نویسه *netrc_tokenstr(curl_netrc_token token)
{
  انتخاب(token) {
  حالت NETRC_TOK_EOF:
    بازگشت "[EOF]";
  حالت NETRC_TOK_LITERAL:
    بازگشت "[LITERAL]";
  حالت NETRC_TOK_MACHINE:
    بازگشت "[MACHINE]";
  حالت NETRC_TOK_DEFAULT:
    بازگشت "[DEFAULT]";
  حالت NETRC_TOK_ACCOUNT:
    بازگشت "[ACCOUNT]";
  حالت NETRC_TOK_LOGIN:
    بازگشت "[LOGIN]";
  حالت NETRC_TOK_PASSWD:
    بازگشت "[PASSWORD]";
  حالت NETRC_TOK_MACDEF:
    بازگشت "[MACDEF]";
  حالت NETRC_TOK_JUNK:
    بازگشت "[JUNK]";
  پیشفرض:
    بازگشت "[???]";
  }
}
#endif

ایستا تهی netrc_lexer_init(ساختار netrc_lexer *lexer,
                             ساختار Curl_easy *data,
                             ثابت نویسه *content)
{
  curlx_dyn_init(&lexer->literal, MAX_NETRC_TOKEN);
  lexer->data = data;
  lexer->content = lexer->pos = content;
}

ایستا تهی netrc_lexer_cleanup(ساختار netrc_lexer *lexer)
{
  lexer->content = lexer->pos = پوچ;
  lexer->data = پوچ;
  curlx_dyn_free(&lexer->literal);
}

ایستا تهی netrc_skip_blanks(ساختار netrc_lexer *lexer)
{
  ثابت نویسه *s = lexer->pos;
  وقتی(*s) {
    curlx_str_passblanks(&s);
    وقتی(*s == '\r')
      ++s;
    اگر(*s == '\n') {
      ++s;
    }
    وگرنه
      بشکن;
  }
  lexer->pos = s;
}

ایستا تهی netrc_skip_to_empty_سطر(ساختار netrc_lexer *lexer)
{
  ثابت نویسه *s = lexer->pos;
  وقتی(*s) {
    اگر(*s == '\r')
      ++s;
    وگرنه اگر(*s == '\n') {
      ++s;
      وقتی(*s == '\r')
        ++s;
      اگر(*s == '\n')
        برو out;
    }
    وگرنه
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
ایستا NETRCcode netrc_lexer_quoted(ساختار netrc_lexer *lexer)
{
  NETRCcode rc = NETRC_SYNTAX_ERROR;
  ثابت نویسه *s = lexer->pos;
  bool escape = FALSE;
  CURLcode result;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  وقتی(*s) {
    نویسه c = *s;
    اگر(escape) {
      escape = FALSE;
      انتخاب(c) {
      حالت 'n':
        c = '\n';
        بشکن;
      حالت 'r':
        c = '\r';
        بشکن;
      حالت 't':
        c = '\t';
        بشکن;
      }
    }
    وگرنه اگر(c == '\\') {
      escape = TRUE;
      ++s;
      ادامه;
    }
    وگرنه اگر(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      result = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(result);
      برو out;
    }
    result = curlx_dyn_addn(&lexer->literal, &c, 1);
    اگر(result) {
      rc = curl2netrc(result);
      برو out;
    }
    ++s;
  }
out:
  lexer->pos = s;
  بازگشت rc;
}

ایستا تهی netrc_lexer_افزودن(ساختار netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

ایستا NETRCcode netrc_lexer_next(ساختار netrc_lexer *lexer,
                                  bool want_literal)
{
  ثابت نویسه *s = lexer->pos, *start;
  NETRCcode rc = NETRC_OK;
  size_t slen;
  CURLcode result;

  اگر(lexer->pushed) {
    lexer->pushed = FALSE;
    برو out;
  }

  curlx_dyn_reset(&lexer->literal);
  netrc_skip_blanks(lexer);
  s = lexer->pos;

  انتخاب(*s) {
  حالت 0:
    lexer->token = NETRC_TOK_EOF;
    بشکن;
  حالت '\"':
    rc = netrc_lexer_quoted(lexer);
    lexer->token = NETRC_TOK_LITERAL;
    s = lexer->pos;
    بشکن;
  پیشفرض:
    /* unquoted token */
    start = s;
    وقتی(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - start;
    اگر(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    اگر(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      result = curlx_dyn_addn(&lexer->literal, start, slen);
      rc = curl2netrc(result);
    }
    وگرنه اگر((slen == 7) && curl_strnequal(start, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    وگرنه اگر((slen == 7) && curl_strnequal(start, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    وگرنه اگر((slen == 7) && curl_strnequal(start, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    وگرنه اگر((slen == 5) && curl_strnequal(start, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    وگرنه اگر((slen == 8) && curl_strnequal(start, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    وگرنه اگر((slen == 6) && curl_strnequal(start, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    وگرنه {
      lexer->token = NETRC_TOK_JUNK;
    }
    بشکن;
  }

out:
#اگر NETRC_DEBUG
  CURL_TRC_M(lexer->data, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#endif
  lexer->pos = s;
  بازگشت rc;
}

ساختار netrc_scanner {
  ساختار netrc_lexer lexer;
  ثابت نویسه *hostname; /* non-NULL, machine to scan for */
  ثابت نویسه *user; /* maybe NULL, login to scan for */
  نویسه *login;
  نویسه *passwd;
  ساختار Curl_creds *creds;
  bool matches_host;
  bool found;
};

ایستا تهی netrc_scan_reset(ساختار netrc_scanner *sc)
{
  curlx_safefree(sc->login);
  curlx_safefree(sc->passwd);
  sc->matches_host = FALSE;
}

ایستا تهی netrc_scan_init(ساختار netrc_scanner *sc,
                            ساختار Curl_easy *data,
                            ثابت نویسه *content,
                            ثابت نویسه *hostname,
                            ثابت نویسه *user)
{
  memset(sc, 0, اندازه(*sc));
  netrc_lexer_init(&sc->lexer, data, content);
  sc->hostname = hostname;
  sc->user = (user && user[0]) ? user : پوچ;
  netrc_scan_reset(sc);
}

ایستا تهی netrc_scan_cleanup(ساختار netrc_scanner *sc)
{
  netrc_scan_reset(sc);
  sc->hostname = پوچ;
  sc->user = پوچ;
  Curl_creds_unlink(&sc->creds);
  netrc_lexer_cleanup(&sc->lexer);
}

ایستا NETRCcode netrc_scan_literal(ساختار netrc_scanner *sc,
                                    نویسه **pdest)
{
  NETRCcode rc = netrc_lexer_next(&sc->lexer, TRUE);
  اگر(!rc) {
    اگر(sc->lexer.token == NETRC_TOK_LITERAL) {
      اگر(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        اگر(!*pdest)
          rc = NETRC_OUT_OF_MEMORY;
      }
    }
    وگرنه
      netrc_lexer_افزودن(&sc->lexer);
  }
  بازگشت rc;
}

ایستا NETRCcode netrc_scan_end_entry(ساختار netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#اگر NETRC_DEBUG
  CURL_TRC_M(sc->lexer.data,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#endif
  اگر(sc->matches_host) {
    اگر(sc->login) {
      اگر(sc->user) {
        اگر(Curl_timestrcmp(sc->user, sc->login))
          برو out;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->found = !!sc->passwd;
      }
      وگرنه {
        sc->found = TRUE;
      }
    }
    وگرنه اگر(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->found = TRUE;
    }
    وگرنه {
      /* entry has nothing interesting */
    }
    اگر(sc->found) {
#اگر NETRC_DEBUG
      CURL_TRC_M(sc->lexer.data, "[NETRC] entry match found");
#endif
      اگر(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           پوچ, پوچ, پوچ, CREDS_NETRC, &sc->creds))
        rc = NETRC_OUT_OF_MEMORY;
    }
  }
out:
  netrc_scan_reset(sc);
  بازگشت rc;
}

ایستا NETRCcode netrc_scan(ساختار Curl_easy *data,
                            ثابت نویسه *content,
                            ثابت نویسه *hostname,
                            ثابت نویسه *user,
                            ساختار Curl_creds **pcreds)
{
  ساختار netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_scan_init(&sc, data, content, hostname, user);

  وقتی(!rc && !sc.found) {
    rc = netrc_lexer_next(&sc.lexer, FALSE);
    اگر(!rc) {
      /* Does this token end any previous entry? */
      انتخاب(sc.lexer.token) {
      حالت NETRC_TOK_EOF:
      حالت NETRC_TOK_MACHINE:
      حالت NETRC_TOK_DEFAULT:
      حالت NETRC_TOK_MACDEF:
        rc = netrc_scan_end_entry(&sc);
        اگر(rc || sc.found)
          برو out;
        بشکن;
      پیشفرض:
        بشکن;
      }

      انتخاب(sc.lexer.token) {
      حالت NETRC_TOK_EOF:
        برو out;
      حالت NETRC_TOK_MACHINE:
        rc = netrc_lexer_next(&sc.lexer, TRUE);
        اگر(!rc) {
          اگر(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          وگرنه {
            sc.matches_host = FALSE;
            netrc_lexer_افزودن(&sc.lexer);
          }
        }
        بشکن;
      حالت NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        بشکن;
      حالت NETRC_TOK_ACCOUNT:
        rc = netrc_scan_literal(&sc, پوچ); /* ignore, not used */
        بشکن;
      حالت NETRC_TOK_LOGIN:
        rc = netrc_scan_literal(&sc, &sc.login);
        بشکن;
      حالت NETRC_TOK_PASSWD:
        rc = netrc_scan_literal(&sc, &sc.passwd);
        بشکن;
      حالت NETRC_TOK_MACDEF:
        netrc_skip_to_empty_سطر(&sc.lexer);
        بشکن;
      حالت NETRC_TOK_LITERAL:
      حالت NETRC_TOK_JUNK:
      پیشفرض:
        /* skip this */
        بشکن;
      }
    }
  }

out:
  اگر(!rc) {
    اگر(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    وگرنه
      rc = NETRC_NO_MATCH;
  }
  netrc_scan_cleanup(&sc);
  بازگشت rc;
}

ایستا NETRCcode file2memory(ثابت نویسه *filename, ساختار dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  اگر(file) {
    curlx_struct_stat stat;
    اگر((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      CURLcode result = CURLE_OK;
      bool eof;
      ساختار dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_سطر);
      ret = NETRC_OK;
      انجامبده {
        ثابت نویسه *سطر;
        /* Curl_get_line always returns lines ending with a newline */
        result = Curl_get_سطر(&linebuf, file, &eof);
        اگر(!result) {
          سطر = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&سطر);
          اگر(*سطر == '#')
            ادامه;
          result = curlx_dyn_add(filebuf, سطر);
        }
        اگر(result) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(result);
          بشکن;
        }
      } وقتی(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  بازگشت ret;
}

ایستا NETRCcode netrc_scan_file(ساختار Curl_easy *data,
                                 ساختار store_netrc *store,
                                 ثابت نویسه *hostname,
                                 ثابت نویسه *user,
                                 ثابت نویسه *netrcfile,
                                 ساختار Curl_creds **pcreds)
{
  ساختار dynbuf *filebuf = &store->filebuf;

  اگر(!store->loaded || strcmp(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    اگر(ret) {
      CURL_TRC_M(data, "[NETRC] could not load '%s'", netrcfile);
      بازگشت ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    اگر(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      بازگشت NETRC_OUT_OF_MEMORY;
    }
    store->loaded = TRUE;
  }
  اگر(!curlx_dyn_len(filebuf))
    بازگشت NETRC_NO_MATCH;

  بازگشت netrc_scan(data, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_scan(ساختار Curl_easy *data,
                          ساختار store_netrc *store,
                          ثابت نویسه *hostname,
                          ثابت نویسه *user,
                          ثابت نویسه *netrcfile,
                          ساختار Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(data, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  اگر(!netrcfile) {
    نویسه *home = پوچ;
    نویسه *homea = پوچ;
    نویسه *filealloc = پوچ;
#اگر defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    نویسه pwbuf[1024];
#endif
    filealloc = curl_getenv("NETRC");
    اگر(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      اگر(homea) {
        home = homea;
#اگر defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      وگرنه {
        ساختار passwd pw, *pw_res;
        اگر(!getpwuid_r(geteuid(), &pw, pwbuf, اندازه(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#وگرنهاگر defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      وگرنه {
        ساختار passwd *pw;
        pw = getpwuid(geteuid());
        اگر(pw) {
          home = pw->pw_dir;
        }
#وگرنهاگر defined(_WIN32)
      }
      وگرنه {
        homea = curl_getenv("USERPROFILE");
        اگر(homea) {
          home = homea;
        }
#endif
      }

      اگر(!home)
        بازگشت NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      اگر(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_OUT_OF_MEMORY;
        برو out;
      }
    }
    retcode = netrc_scan_file(
      data, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#ifdef _WIN32
    اگر(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      اگر(!filealloc) {
        curlx_free(homea);
        بازگشت NETRC_OUT_OF_MEMORY;
      }
      retcode = netrc_scan_file(
        data, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#endif
    curlx_free(homea);
  }
  وگرنه
    retcode = netrc_scan_file(
      data, store, hostname, user, netrcfile, pcreds);

out:
  اگر(retcode)
    Curl_creds_unlink(pcreds);
  بازگشت retcode;
}

تهی Curl_netrc_init(ساختار store_netrc *store)
{
  curlx_dyn_init(&store->filebuf, MAX_NETRC_FILE);
  store->loaded = FALSE;
  store->filename = پوچ;
}
تهی Curl_netrc_cleanup(ساختار store_netrc *store)
{
  curlx_dyn_free(&store->filebuf);
  curlx_safefree(store->filename);
  store->loaded = FALSE;
}

ثابت نویسه *Curl_netrc_strerror(NETRCcode ret)
{
  انتخاب(ret) {
  پیشفرض:
    بازگشت ""; /* not a legit error */
  حالت NETRC_FILE_MISSING:
    بازگشت "no such file";
  حالت NETRC_NO_MATCH:
    بازگشت "no matching entry";
  حالت NETRC_OUT_OF_MEMORY:
    بازگشت "out of memory";
  حالت NETRC_SYNTAX_ERROR:
    بازگشت "syntax error";
  }
  /* never reached */
}

#endif /* !CURL_DISABLE_NETRC */
