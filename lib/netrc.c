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
#समावेश "curl_setup.h"

#अगर_अघोषित CURL_DISABLE_NETRC

#अगर_घोषित HAVE_PWD_H
#अगर_घोषित __AMIGA__
#अघोषित __NO_NET_API /* required for AmigaOS to declare getpwuid() */
#पूर्ण_अगर
#समावेश <pwd.h>
#अगर_घोषित __AMIGA__
#घोषणा __NO_NET_API
#पूर्ण_अगर
#पूर्ण_अगर

#समावेश "netrc.h"
#समावेश "urldata.h"
#समावेश "creds.h"
#समावेश "curl_trc.h"
#समावेश "strcase.h"
#समावेश "curl_get_line.h"
#समावेश "curlx/win32-fopen.h"
#समावेश "curlx/strparse.h"


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

#घोषणा MAX_NETRC_पंक्ति  16384
#घोषणा MAX_NETRC_FILE  (128 * 1024)
#घोषणा MAX_NETRC_TOKEN 4096

#घोषणा NETRC_DEBUG   0

/* convert a dynbuf call CURLcode error to a NETRCcode error */
#घोषणा curl2netrc(r)                                  \
  ((!(r)) ? NETRC_OK : (((r) == CURLE_OUT_OF_MEMORY) ? \
   NETRC_OUT_OF_MEMORY : NETRC_SYNTAX_ERROR))

प्रकार क्रमागत {
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

काष्ठा netrc_lexer {
  काष्ठा Curl_easy *तथ्य;
  स्थिर अक्षर *content;
  स्थिर अक्षर *pos;
  काष्ठा dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#अगर NETRC_DEBUG
अटल स्थिर अक्षर *netrc_tokenstr(curl_netrc_token token)
{
  चयन(token) {
  हाल NETRC_TOK_EOF:
    वापस "[EOF]";
  हाल NETRC_TOK_LITERAL:
    वापस "[LITERAL]";
  हाल NETRC_TOK_MACHINE:
    वापस "[MACHINE]";
  हाल NETRC_TOK_DEFAULT:
    वापस "[DEFAULT]";
  हाल NETRC_TOK_ACCOUNT:
    वापस "[ACCOUNT]";
  हाल NETRC_TOK_LOGIN:
    वापस "[LOGIN]";
  हाल NETRC_TOK_PASSWD:
    वापस "[PASSWORD]";
  हाल NETRC_TOK_MACDEF:
    वापस "[MACDEF]";
  हाल NETRC_TOK_JUNK:
    वापस "[JUNK]";
  शेष:
    वापस "[???]";
  }
}
#पूर्ण_अगर

अटल व्योम netrc_lexer_init(काष्ठा netrc_lexer *lexer,
                             काष्ठा Curl_easy *तथ्य,
                             स्थिर अक्षर *content)
{
  curlx_dyn_init(&lexer->literal, MAX_NETRC_TOKEN);
  lexer->तथ्य = तथ्य;
  lexer->content = lexer->pos = content;
}

अटल व्योम netrc_lexer_cleanup(काष्ठा netrc_lexer *lexer)
{
  lexer->content = lexer->pos = शून्य;
  lexer->तथ्य = शून्य;
  curlx_dyn_free(&lexer->literal);
}

अटल व्योम netrc_skip_blanks(काष्ठा netrc_lexer *lexer)
{
  स्थिर अक्षर *s = lexer->pos;
  जबतक(*s) {
    curlx_str_passblanks(&s);
    जबतक(*s == '\r')
      ++s;
    अगर(*s == '\n') {
      ++s;
    }
    अन्यथा
      अवरोध;
  }
  lexer->pos = s;
}

अटल व्योम netrc_skip_to_empty_पंक्ति(काष्ठा netrc_lexer *lexer)
{
  स्थिर अक्षर *s = lexer->pos;
  जबतक(*s) {
    अगर(*s == '\r')
      ++s;
    अन्यथा अगर(*s == '\n') {
      ++s;
      जबतक(*s == '\r')
        ++s;
      अगर(*s == '\n')
        जाओ out;
    }
    अन्यथा
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
अटल NETRCcode netrc_lexer_quoted(काष्ठा netrc_lexer *lexer)
{
  NETRCcode rc = NETRC_SYNTAX_ERROR;
  स्थिर अक्षर *s = lexer->pos;
  bool escape = FALSE;
  CURLcode परिणाम;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  जबतक(*s) {
    अक्षर c = *s;
    अगर(escape) {
      escape = FALSE;
      चयन(c) {
      हाल 'n':
        c = '\n';
        अवरोध;
      हाल 'r':
        c = '\r';
        अवरोध;
      हाल 't':
        c = '\t';
        अवरोध;
      }
    }
    अन्यथा अगर(c == '\\') {
      escape = TRUE;
      ++s;
      जारी;
    }
    अन्यथा अगर(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      परिणाम = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(परिणाम);
      जाओ out;
    }
    परिणाम = curlx_dyn_addn(&lexer->literal, &c, 1);
    अगर(परिणाम) {
      rc = curl2netrc(परिणाम);
      जाओ out;
    }
    ++s;
  }
out:
  lexer->pos = s;
  वापस rc;
}

अटल व्योम netrc_lexer_धकेलो(काष्ठा netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

अटल NETRCcode netrc_lexer_अग्र(काष्ठा netrc_lexer *lexer,
                                  bool want_literal)
{
  स्थिर अक्षर *s = lexer->pos, *start;
  NETRCcode rc = NETRC_OK;
  माप_प्रकार slen;
  CURLcode परिणाम;

  अगर(lexer->pushed) {
    lexer->pushed = FALSE;
    जाओ out;
  }

  curlx_dyn_reset(&lexer->literal);
  netrc_skip_blanks(lexer);
  s = lexer->pos;

  चयन(*s) {
  हाल 0:
    lexer->token = NETRC_TOK_EOF;
    अवरोध;
  हाल '\"':
    rc = netrc_lexer_quoted(lexer);
    lexer->token = NETRC_TOK_LITERAL;
    s = lexer->pos;
    अवरोध;
  शेष:
    /* unquoted token */
    start = s;
    जबतक(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - start;
    अगर(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    अगर(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      परिणाम = curlx_dyn_addn(&lexer->literal, start, slen);
      rc = curl2netrc(परिणाम);
    }
    अन्यथा अगर((slen == 7) && curl_strnequal(start, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    अन्यथा अगर((slen == 7) && curl_strnequal(start, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    अन्यथा अगर((slen == 7) && curl_strnequal(start, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    अन्यथा अगर((slen == 5) && curl_strnequal(start, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    अन्यथा अगर((slen == 8) && curl_strnequal(start, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    अन्यथा अगर((slen == 6) && curl_strnequal(start, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    अन्यथा {
      lexer->token = NETRC_TOK_JUNK;
    }
    अवरोध;
  }

out:
#अगर NETRC_DEBUG
  CURL_TRC_M(lexer->तथ्य, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#पूर्ण_अगर
  lexer->pos = s;
  वापस rc;
}

काष्ठा netrc_scanner {
  काष्ठा netrc_lexer lexer;
  स्थिर अक्षर *hostname; /* non-NULL, machine to scan for */
  स्थिर अक्षर *user; /* maybe NULL, login to scan for */
  अक्षर *login;
  अक्षर *passwd;
  काष्ठा Curl_creds *creds;
  bool matches_host;
  bool found;
};

अटल व्योम netrc_scan_reset(काष्ठा netrc_scanner *sc)
{
  curlx_safefree(sc->login);
  curlx_safefree(sc->passwd);
  sc->matches_host = FALSE;
}

अटल व्योम netrc_scan_init(काष्ठा netrc_scanner *sc,
                            काष्ठा Curl_easy *तथ्य,
                            स्थिर अक्षर *content,
                            स्थिर अक्षर *hostname,
                            स्थिर अक्षर *user)
{
  स_रखो(sc, 0, माप(*sc));
  netrc_lexer_init(&sc->lexer, तथ्य, content);
  sc->hostname = hostname;
  sc->user = (user && user[0]) ? user : शून्य;
  netrc_scan_reset(sc);
}

अटल व्योम netrc_scan_cleanup(काष्ठा netrc_scanner *sc)
{
  netrc_scan_reset(sc);
  sc->hostname = शून्य;
  sc->user = शून्य;
  Curl_creds_unlink(&sc->creds);
  netrc_lexer_cleanup(&sc->lexer);
}

अटल NETRCcode netrc_scan_literal(काष्ठा netrc_scanner *sc,
                                    अक्षर **pdest)
{
  NETRCcode rc = netrc_lexer_अग्र(&sc->lexer, TRUE);
  अगर(!rc) {
    अगर(sc->lexer.token == NETRC_TOK_LITERAL) {
      अगर(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        अगर(!*pdest)
          rc = NETRC_OUT_OF_MEMORY;
      }
    }
    अन्यथा
      netrc_lexer_धकेलो(&sc->lexer);
  }
  वापस rc;
}

अटल NETRCcode netrc_scan_end_entry(काष्ठा netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#अगर NETRC_DEBUG
  CURL_TRC_M(sc->lexer.तथ्य,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#पूर्ण_अगर
  अगर(sc->matches_host) {
    अगर(sc->login) {
      अगर(sc->user) {
        अगर(Curl_timestrcmp(sc->user, sc->login))
          जाओ out;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->found = !!sc->passwd;
      }
      अन्यथा {
        sc->found = TRUE;
      }
    }
    अन्यथा अगर(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->found = TRUE;
    }
    अन्यथा {
      /* entry has nothing interesting */
    }
    अगर(sc->found) {
#अगर NETRC_DEBUG
      CURL_TRC_M(sc->lexer.तथ्य, "[NETRC] entry match found");
#पूर्ण_अगर
      अगर(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           शून्य, शून्य, शून्य, CREDS_NETRC, &sc->creds))
        rc = NETRC_OUT_OF_MEMORY;
    }
  }
out:
  netrc_scan_reset(sc);
  वापस rc;
}

अटल NETRCcode netrc_scan(काष्ठा Curl_easy *तथ्य,
                            स्थिर अक्षर *content,
                            स्थिर अक्षर *hostname,
                            स्थिर अक्षर *user,
                            काष्ठा Curl_creds **pcreds)
{
  काष्ठा netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_scan_init(&sc, तथ्य, content, hostname, user);

  जबतक(!rc && !sc.found) {
    rc = netrc_lexer_अग्र(&sc.lexer, FALSE);
    अगर(!rc) {
      /* Does this token end any previous entry? */
      चयन(sc.lexer.token) {
      हाल NETRC_TOK_EOF:
      हाल NETRC_TOK_MACHINE:
      हाल NETRC_TOK_DEFAULT:
      हाल NETRC_TOK_MACDEF:
        rc = netrc_scan_end_entry(&sc);
        अगर(rc || sc.found)
          जाओ out;
        अवरोध;
      शेष:
        अवरोध;
      }

      चयन(sc.lexer.token) {
      हाल NETRC_TOK_EOF:
        जाओ out;
      हाल NETRC_TOK_MACHINE:
        rc = netrc_lexer_अग्र(&sc.lexer, TRUE);
        अगर(!rc) {
          अगर(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          अन्यथा {
            sc.matches_host = FALSE;
            netrc_lexer_धकेलो(&sc.lexer);
          }
        }
        अवरोध;
      हाल NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        अवरोध;
      हाल NETRC_TOK_ACCOUNT:
        rc = netrc_scan_literal(&sc, शून्य); /* ignore, not used */
        अवरोध;
      हाल NETRC_TOK_LOGIN:
        rc = netrc_scan_literal(&sc, &sc.login);
        अवरोध;
      हाल NETRC_TOK_PASSWD:
        rc = netrc_scan_literal(&sc, &sc.passwd);
        अवरोध;
      हाल NETRC_TOK_MACDEF:
        netrc_skip_to_empty_पंक्ति(&sc.lexer);
        अवरोध;
      हाल NETRC_TOK_LITERAL:
      हाल NETRC_TOK_JUNK:
      शेष:
        /* skip this */
        अवरोध;
      }
    }
  }

out:
  अगर(!rc) {
    अगर(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    अन्यथा
      rc = NETRC_NO_MATCH;
  }
  netrc_scan_cleanup(&sc);
  वापस rc;
}

अटल NETRCcode file2memory(स्थिर अक्षर *filename, काष्ठा dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  खाता *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  अगर(file) {
    curlx_struct_stat stat;
    अगर((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      CURLcode परिणाम = CURLE_OK;
      bool eof;
      काष्ठा dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_पंक्ति);
      ret = NETRC_OK;
      करो {
        स्थिर अक्षर *पंक्ति;
        /* Curl_get_line always returns lines ending with a newline */
        परिणाम = Curl_get_पंक्ति(&linebuf, file, &eof);
        अगर(!परिणाम) {
          पंक्ति = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&पंक्ति);
          अगर(*पंक्ति == '#')
            जारी;
          परिणाम = curlx_dyn_add(filebuf, पंक्ति);
        }
        अगर(परिणाम) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(परिणाम);
          अवरोध;
        }
      } जबतक(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  वापस ret;
}

अटल NETRCcode netrc_scan_file(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा store_netrc *store,
                                 स्थिर अक्षर *hostname,
                                 स्थिर अक्षर *user,
                                 स्थिर अक्षर *netrcfile,
                                 काष्ठा Curl_creds **pcreds)
{
  काष्ठा dynbuf *filebuf = &store->filebuf;

  अगर(!store->loaded || म_भेद(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    अगर(ret) {
      CURL_TRC_M(तथ्य, "[NETRC] could not load '%s'", netrcfile);
      वापस ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    अगर(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      वापस NETRC_OUT_OF_MEMORY;
    }
    store->loaded = TRUE;
  }
  अगर(!curlx_dyn_len(filebuf))
    वापस NETRC_NO_MATCH;

  वापस netrc_scan(तथ्य, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_scan(काष्ठा Curl_easy *तथ्य,
                          काष्ठा store_netrc *store,
                          स्थिर अक्षर *hostname,
                          स्थिर अक्षर *user,
                          स्थिर अक्षर *netrcfile,
                          काष्ठा Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(तथ्य, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  अगर(!netrcfile) {
    अक्षर *home = शून्य;
    अक्षर *homea = शून्य;
    अक्षर *filealloc = शून्य;
#अगर defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    अक्षर pwbuf[1024];
#पूर्ण_अगर
    filealloc = curl_getenv("NETRC");
    अगर(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      अगर(homea) {
        home = homea;
#अगर defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      अन्यथा {
        काष्ठा passwd pw, *pw_res;
        अगर(!getpwuid_r(geteuid(), &pw, pwbuf, माप(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#या_अगर defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      अन्यथा {
        काष्ठा passwd *pw;
        pw = getpwuid(geteuid());
        अगर(pw) {
          home = pw->pw_dir;
        }
#या_अगर defined(_WIN32)
      }
      अन्यथा {
        homea = curl_getenv("USERPROFILE");
        अगर(homea) {
          home = homea;
        }
#पूर्ण_अगर
      }

      अगर(!home)
        वापस NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      अगर(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_OUT_OF_MEMORY;
        जाओ out;
      }
    }
    retcode = netrc_scan_file(
      तथ्य, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#अगर_घोषित _WIN32
    अगर(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      अगर(!filealloc) {
        curlx_free(homea);
        वापस NETRC_OUT_OF_MEMORY;
      }
      retcode = netrc_scan_file(
        तथ्य, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#पूर्ण_अगर
    curlx_free(homea);
  }
  अन्यथा
    retcode = netrc_scan_file(
      तथ्य, store, hostname, user, netrcfile, pcreds);

out:
  अगर(retcode)
    Curl_creds_unlink(pcreds);
  वापस retcode;
}

व्योम Curl_netrc_init(काष्ठा store_netrc *store)
{
  curlx_dyn_init(&store->filebuf, MAX_NETRC_FILE);
  store->loaded = FALSE;
  store->filename = शून्य;
}
व्योम Curl_netrc_cleanup(काष्ठा store_netrc *store)
{
  curlx_dyn_free(&store->filebuf);
  curlx_safefree(store->filename);
  store->loaded = FALSE;
}

स्थिर अक्षर *Curl_netrc_strerror(NETRCcode ret)
{
  चयन(ret) {
  शेष:
    वापस ""; /* not a legit error */
  हाल NETRC_FILE_MISSING:
    वापस "no such file";
  हाल NETRC_NO_MATCH:
    वापस "no matching entry";
  हाल NETRC_OUT_OF_MEMORY:
    वापस "out of memory";
  हाल NETRC_SYNTAX_ERROR:
    वापस "syntax error";
  }
  /* never reached */
}

#पूर्ण_अगर /* !CURL_DISABLE_NETRC */
