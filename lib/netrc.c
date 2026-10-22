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

#اگر_غیر_تعریف_شدہ CURL_DISABLE_NETRC

#اگر_تعریف_شدہ HAVE_PWD_H
#اگر_تعریف_شدہ __AMIGA__
#تعریف_منسوخ __NO_NET_API /* required for AmigaOS to declare getpwuid() */
#اگر_ختم
#شامل <pwd.h>
#اگر_تعریف_شدہ __AMIGA__
#تعریف_کرو __NO_NET_API
#اگر_ختم
#اگر_ختم

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

#تعریف_کرو MAX_NETRC_سطر  16384
#تعریف_کرو MAX_NETRC_FILE  (128 * 1024)
#تعریف_کرو MAX_NETRC_TOKEN 4096

#تعریف_کرو NETRC_DEBUG   0

/* convert a dynbuf call CURLcode error to a NETRCcode error */
#تعریف_کرو curl2netrc(r)                                  \
  ((!(r)) ? NETRC_OK : (((r) == CURLE_برآمد_کا_حافظہ) ? \
   NETRC_برآمد_کا_حافظہ : NETRC_SYNTAX_ERROR))

قسمتعریف شمار {
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

ساخت netrc_lexer {
  ساخت Curl_easy *ڈیٹا;
  ثابت حرف *content;
  ثابت حرف *pos;
  ساخت dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#اگر NETRC_DEBUG
ساکن ثابت حرف *netrc_tokenstr(curl_netrc_token token)
{
  انتخاب(token) {
  حالت NETRC_TOK_EOF:
    واپس "[EOF]";
  حالت NETRC_TOK_LITERAL:
    واپس "[LITERAL]";
  حالت NETRC_TOK_MACHINE:
    واپس "[MACHINE]";
  حالت NETRC_TOK_DEFAULT:
    واپس "[DEFAULT]";
  حالت NETRC_TOK_ACCOUNT:
    واپس "[ACCOUNT]";
  حالت NETRC_TOK_LOGIN:
    واپس "[LOGIN]";
  حالت NETRC_TOK_PASSWD:
    واپس "[PASSWORD]";
  حالت NETRC_TOK_MACDEF:
    واپس "[MACDEF]";
  حالت NETRC_TOK_JUNK:
    واپس "[JUNK]";
  طےشدہ:
    واپس "[???]";
  }
}
#اگر_ختم

ساکن خالی netrc_lexer_init(ساخت netrc_lexer *lexer,
                             ساخت Curl_easy *ڈیٹا,
                             ثابت حرف *content)
{
  curlx_dyn_init(&lexer->literal, MAX_NETRC_TOKEN);
  lexer->ڈیٹا = ڈیٹا;
  lexer->content = lexer->pos = content;
}

ساکن خالی netrc_lexer_cleanup(ساخت netrc_lexer *lexer)
{
  lexer->content = lexer->pos = تہی;
  lexer->ڈیٹا = تہی;
  curlx_dyn_free(&lexer->literal);
}

ساکن خالی netrc_skip_blanks(ساخت netrc_lexer *lexer)
{
  ثابت حرف *s = lexer->pos;
  جب(*s) {
    curlx_str_passblanks(&s);
    جب(*s == '\r')
      ++s;
    اگر(*s == '\n') {
      ++s;
    }
    ورنہ
      توڑو;
  }
  lexer->pos = s;
}

ساکن خالی netrc_skip_مقصد_خالی_خانے_سطر(ساخت netrc_lexer *lexer)
{
  ثابت حرف *s = lexer->pos;
  جب(*s) {
    اگر(*s == '\r')
      ++s;
    ورنہ اگر(*s == '\n') {
      ++s;
      جب(*s == '\r')
        ++s;
      اگر(*s == '\n')
        جاؤ برآمد;
    }
    ورنہ
      ++s;
  }
برآمد:
  lexer->pos = s;
}

/*
 * Parse a quoted token starting after the opening '"'. Handles \n, \r, \t
 * escape sequences. Advances *tok_endp past the closing '"'.
 *
 * Returns NETRC_OK or error.
 */
ساکن NETRCcode netrc_lexer_quoted(ساخت netrc_lexer *lexer)
{
  NETRCcode rc = NETRC_SYNTAX_ERROR;
  ثابت حرف *s = lexer->pos;
  bool escape = FALSE;
  CURLcode نتیجہ;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  جب(*s) {
    حرف c = *s;
    اگر(escape) {
      escape = FALSE;
      انتخاب(c) {
      حالت 'n':
        c = '\n';
        توڑو;
      حالت 'r':
        c = '\r';
        توڑو;
      حالت 't':
        c = '\t';
        توڑو;
      }
    }
    ورنہ اگر(c == '\\') {
      escape = TRUE;
      ++s;
      جاری;
    }
    ورنہ اگر(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      نتیجہ = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(نتیجہ);
      جاؤ برآمد;
    }
    نتیجہ = curlx_dyn_addn(&lexer->literal, &c, 1);
    اگر(نتیجہ) {
      rc = curl2netrc(نتیجہ);
      جاؤ برآمد;
    }
    ++s;
  }
برآمد:
  lexer->pos = s;
  واپس rc;
}

ساکن خالی netrc_lexer_ڈالو(ساخت netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

ساکن NETRCcode netrc_lexer_اگلا(ساخت netrc_lexer *lexer,
                                  bool want_literal)
{
  ثابت حرف *s = lexer->pos, *آغاز;
  NETRCcode rc = NETRC_OK;
  size_t slen;
  CURLcode نتیجہ;

  اگر(lexer->pushed) {
    lexer->pushed = FALSE;
    جاؤ برآمد;
  }

  curlx_dyn_reset(&lexer->literal);
  netrc_skip_blanks(lexer);
  s = lexer->pos;

  انتخاب(*s) {
  حالت 0:
    lexer->token = NETRC_TOK_EOF;
    توڑو;
  حالت '\"':
    rc = netrc_lexer_quoted(lexer);
    lexer->token = NETRC_TOK_LITERAL;
    s = lexer->pos;
    توڑو;
  طےشدہ:
    /* unquoted token */
    آغاز = s;
    جب(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - آغاز;
    اگر(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    اگر(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      نتیجہ = curlx_dyn_addn(&lexer->literal, آغاز, slen);
      rc = curl2netrc(نتیجہ);
    }
    ورنہ اگر((slen == 7) && curl_strnequal(آغاز, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    ورنہ اگر((slen == 7) && curl_strnequal(آغاز, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    ورنہ اگر((slen == 7) && curl_strnequal(آغاز, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    ورنہ اگر((slen == 5) && curl_strnequal(آغاز, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    ورنہ اگر((slen == 8) && curl_strnequal(آغاز, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    ورنہ اگر((slen == 6) && curl_strnequal(آغاز, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    ورنہ {
      lexer->token = NETRC_TOK_JUNK;
    }
    توڑو;
  }

برآمد:
#اگر NETRC_DEBUG
  CURL_TRC_M(lexer->ڈیٹا, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#اگر_ختم
  lexer->pos = s;
  واپس rc;
}

ساخت netrc_scanner {
  ساخت netrc_lexer lexer;
  ثابت حرف *hostname; /* non-NULL, machine to scan for */
  ثابت حرف *user; /* maybe NULL, login to scan for */
  حرف *login;
  حرف *passwd;
  ساخت Curl_creds *creds;
  bool matches_host;
  bool ملا;
};

ساکن خالی netrc_جائزہ_reset(ساخت netrc_scanner *sc)
{
  curlx_safefree(sc->login);
  curlx_safefree(sc->passwd);
  sc->matches_host = FALSE;
}

ساکن خالی netrc_جائزہ_init(ساخت netrc_scanner *sc,
                            ساخت Curl_easy *ڈیٹا,
                            ثابت حرف *content,
                            ثابت حرف *hostname,
                            ثابت حرف *user)
{
  memset(sc, 0, حجم(*sc));
  netrc_lexer_init(&sc->lexer, ڈیٹا, content);
  sc->hostname = hostname;
  sc->user = (user && user[0]) ? user : تہی;
  netrc_جائزہ_reset(sc);
}

ساکن خالی netrc_جائزہ_cleanup(ساخت netrc_scanner *sc)
{
  netrc_جائزہ_reset(sc);
  sc->hostname = تہی;
  sc->user = تہی;
  Curl_creds_unlink(&sc->creds);
  netrc_lexer_cleanup(&sc->lexer);
}

ساکن NETRCcode netrc_جائزہ_literal(ساخت netrc_scanner *sc,
                                    حرف **pdest)
{
  NETRCcode rc = netrc_lexer_اگلا(&sc->lexer, TRUE);
  اگر(!rc) {
    اگر(sc->lexer.token == NETRC_TOK_LITERAL) {
      اگر(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        اگر(!*pdest)
          rc = NETRC_برآمد_کا_حافظہ;
      }
    }
    ورنہ
      netrc_lexer_ڈالو(&sc->lexer);
  }
  واپس rc;
}

ساکن NETRCcode netrc_جائزہ_انتہا_اندراج(ساخت netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#اگر NETRC_DEBUG
  CURL_TRC_M(sc->lexer.ڈیٹا,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#اگر_ختم
  اگر(sc->matches_host) {
    اگر(sc->login) {
      اگر(sc->user) {
        اگر(Curl_timestrcmp(sc->user, sc->login))
          جاؤ برآمد;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->ملا = !!sc->passwd;
      }
      ورنہ {
        sc->ملا = TRUE;
      }
    }
    ورنہ اگر(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->ملا = TRUE;
    }
    ورنہ {
      /* entry has nothing interesting */
    }
    اگر(sc->ملا) {
#اگر NETRC_DEBUG
      CURL_TRC_M(sc->lexer.ڈیٹا, "[NETRC] entry match found");
#اگر_ختم
      اگر(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           تہی, تہی, تہی, CREDS_NETRC, &sc->creds))
        rc = NETRC_برآمد_کا_حافظہ;
    }
  }
برآمد:
  netrc_جائزہ_reset(sc);
  واپس rc;
}

ساکن NETRCcode netrc_جائزہ(ساخت Curl_easy *ڈیٹا,
                            ثابت حرف *content,
                            ثابت حرف *hostname,
                            ثابت حرف *user,
                            ساخت Curl_creds **pcreds)
{
  ساخت netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_جائزہ_init(&sc, ڈیٹا, content, hostname, user);

  جب(!rc && !sc.ملا) {
    rc = netrc_lexer_اگلا(&sc.lexer, FALSE);
    اگر(!rc) {
      /* Does this token end any previous entry? */
      انتخاب(sc.lexer.token) {
      حالت NETRC_TOK_EOF:
      حالت NETRC_TOK_MACHINE:
      حالت NETRC_TOK_DEFAULT:
      حالت NETRC_TOK_MACDEF:
        rc = netrc_جائزہ_انتہا_اندراج(&sc);
        اگر(rc || sc.ملا)
          جاؤ برآمد;
        توڑو;
      طےشدہ:
        توڑو;
      }

      انتخاب(sc.lexer.token) {
      حالت NETRC_TOK_EOF:
        جاؤ برآمد;
      حالت NETRC_TOK_MACHINE:
        rc = netrc_lexer_اگلا(&sc.lexer, TRUE);
        اگر(!rc) {
          اگر(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          ورنہ {
            sc.matches_host = FALSE;
            netrc_lexer_ڈالو(&sc.lexer);
          }
        }
        توڑو;
      حالت NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        توڑو;
      حالت NETRC_TOK_ACCOUNT:
        rc = netrc_جائزہ_literal(&sc, تہی); /* ignore, not used */
        توڑو;
      حالت NETRC_TOK_LOGIN:
        rc = netrc_جائزہ_literal(&sc, &sc.login);
        توڑو;
      حالت NETRC_TOK_PASSWD:
        rc = netrc_جائزہ_literal(&sc, &sc.passwd);
        توڑو;
      حالت NETRC_TOK_MACDEF:
        netrc_skip_مقصد_خالی_خانے_سطر(&sc.lexer);
        توڑو;
      حالت NETRC_TOK_LITERAL:
      حالت NETRC_TOK_JUNK:
      طےشدہ:
        /* skip this */
        توڑو;
      }
    }
  }

برآمد:
  اگر(!rc) {
    اگر(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    ورنہ
      rc = NETRC_NO_مطابقت;
  }
  netrc_جائزہ_cleanup(&sc);
  واپس rc;
}

ساکن NETRCcode file2memory(ثابت حرف *filename, ساخت dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  اگر(file) {
    curlx_struct_stat stat;
    اگر((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_منوال)) {
      CURLcode نتیجہ = CURLE_OK;
      bool eof;
      ساخت dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_سطر);
      ret = NETRC_OK;
      کرو {
        ثابت حرف *سطر;
        /* Curl_get_line always returns lines ending with a newline */
        نتیجہ = Curl_get_سطر(&linebuf, file, &eof);
        اگر(!نتیجہ) {
          سطر = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&سطر);
          اگر(*سطر == '#')
            جاری;
          نتیجہ = curlx_dyn_جوڑو(filebuf, سطر);
        }
        اگر(نتیجہ) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(نتیجہ);
          توڑو;
        }
      } جب(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  واپس ret;
}

ساکن NETRCcode netrc_جائزہ_file(ساخت Curl_easy *ڈیٹا,
                                 ساخت store_netrc *store,
                                 ثابت حرف *hostname,
                                 ثابت حرف *user,
                                 ثابت حرف *netrcfile,
                                 ساخت Curl_creds **pcreds)
{
  ساخت dynbuf *filebuf = &store->filebuf;

  اگر(!store->loaded || strcmp(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    اگر(ret) {
      CURL_TRC_M(ڈیٹا, "[NETRC] could not load '%s'", netrcfile);
      واپس ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    اگر(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      واپس NETRC_برآمد_کا_حافظہ;
    }
    store->loaded = TRUE;
  }
  اگر(!curlx_dyn_len(filebuf))
    واپس NETRC_NO_مطابقت;

  واپس netrc_جائزہ(ڈیٹا, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_جائزہ(ساخت Curl_easy *ڈیٹا,
                          ساخت store_netrc *store,
                          ثابت حرف *hostname,
                          ثابت حرف *user,
                          ثابت حرف *netrcfile,
                          ساخت Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(ڈیٹا, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  اگر(!netrcfile) {
    حرف *home = تہی;
    حرف *homea = تہی;
    حرف *filealloc = تہی;
#اگر defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    حرف pwbuf[1024];
#اگر_ختم
    filealloc = curl_getenv("NETRC");
    اگر(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      اگر(homea) {
        home = homea;
#اگر defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      ورنہ {
        ساخت passwd pw, *pw_res;
        اگر(!getpwuid_r(geteuid(), &pw, pwbuf, حجم(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#وگرنہ_اگر defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      ورنہ {
        ساخت passwd *pw;
        pw = getpwuid(geteuid());
        اگر(pw) {
          home = pw->pw_dir;
        }
#وگرنہ_اگر defined(_WIN32)
      }
      ورنہ {
        homea = curl_getenv("USERPROFILE");
        اگر(homea) {
          home = homea;
        }
#اگر_ختم
      }

      اگر(!home)
        واپس NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      اگر(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_برآمد_کا_حافظہ;
        جاؤ برآمد;
      }
    }
    retcode = netrc_جائزہ_file(
      ڈیٹا, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#اگر_تعریف_شدہ _WIN32
    اگر(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      اگر(!filealloc) {
        curlx_free(homea);
        واپس NETRC_برآمد_کا_حافظہ;
      }
      retcode = netrc_جائزہ_file(
        ڈیٹا, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#اگر_ختم
    curlx_free(homea);
  }
  ورنہ
    retcode = netrc_جائزہ_file(
      ڈیٹا, store, hostname, user, netrcfile, pcreds);

برآمد:
  اگر(retcode)
    Curl_creds_unlink(pcreds);
  واپس retcode;
}

خالی Curl_netrc_init(ساخت store_netrc *store)
{
  curlx_dyn_init(&store->filebuf, MAX_NETRC_FILE);
  store->loaded = FALSE;
  store->filename = تہی;
}
خالی Curl_netrc_cleanup(ساخت store_netrc *store)
{
  curlx_dyn_free(&store->filebuf);
  curlx_safefree(store->filename);
  store->loaded = FALSE;
}

ثابت حرف *Curl_netrc_strerror(NETRCcode ret)
{
  انتخاب(ret) {
  طےشدہ:
    واپس ""; /* not a legit error */
  حالت NETRC_FILE_MISSING:
    واپس "no such file";
  حالت NETRC_NO_مطابقت:
    واپس "no matching entry";
  حالت NETRC_برآمد_کا_حافظہ:
    واپس "out of memory";
  حالت NETRC_SYNTAX_ERROR:
    واپس "syntax error";
  }
  /* never reached */
}

#اگر_ختم /* !CURL_DISABLE_NETRC */
