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
#अन्तर्भाव "curl_setup.h"

#ifndef CURL_DISABLE_NETRC

#ifdef HAVE_PWD_H
#ifdef __AMIGA__
#undef __NO_NET_API /* required for AmigaOS to declare getpwuid() */
#endif
#अन्तर्भाव <pwd.h>
#ifdef __AMIGA__
#define __NO_NET_API
#endif
#endif

#अन्तर्भाव "netrc.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "creds.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "curl_get_line.h"
#अन्तर्भाव "curlx/win32-fopen.h"
#अन्तर्भाव "curlx/strparse.h"


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

#define MAX_NETRC_ओळ  16384
#define MAX_NETRC_FILE  (128 * 1024)
#define MAX_NETRC_TOKEN 4096

#define NETRC_DEBUG   0

/* convert a dynbuf call CURLcode error to a NETRCcode error */
#define curl2netrc(r)                                  \
  ((!(r)) ? NETRC_OK : (((r) == CURLE_OUT_OF_MEMORY) ? \
   NETRC_OUT_OF_MEMORY : NETRC_SYNTAX_ERROR))

प्रकारव्याख्या गणना {
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

संरचना netrc_lexer {
  संरचना Curl_easy *data;
  स्थिरांक अक्षर *content;
  स्थिरांक अक्षर *pos;
  संरचना dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#जर NETRC_DEBUG
स्थिर स्थिरांक अक्षर *netrc_tokenstr(curl_netrc_token token)
{
  निवड(token) {
  स्थिती NETRC_TOK_EOF:
    परत "[EOF]";
  स्थिती NETRC_TOK_LITERAL:
    परत "[LITERAL]";
  स्थिती NETRC_TOK_MACHINE:
    परत "[MACHINE]";
  स्थिती NETRC_TOK_DEFAULT:
    परत "[DEFAULT]";
  स्थिती NETRC_TOK_ACCOUNT:
    परत "[ACCOUNT]";
  स्थिती NETRC_TOK_LOGIN:
    परत "[LOGIN]";
  स्थिती NETRC_TOK_PASSWD:
    परत "[PASSWORD]";
  स्थिती NETRC_TOK_MACDEF:
    परत "[MACDEF]";
  स्थिती NETRC_TOK_JUNK:
    परत "[JUNK]";
  पूर्वनिर्धारित:
    परत "[???]";
  }
}
#endif

स्थिर शून्य netrc_lexer_init(संरचना netrc_lexer *lexer,
                             संरचना Curl_easy *data,
                             स्थिरांक अक्षर *content)
{
  curlx_dyn_init(&lexer->literal, MAX_NETRC_TOKEN);
  lexer->data = data;
  lexer->content = lexer->pos = content;
}

स्थिर शून्य netrc_lexer_cleanup(संरचना netrc_lexer *lexer)
{
  lexer->content = lexer->pos = रिक्त;
  lexer->data = रिक्त;
  curlx_dyn_free(&lexer->literal);
}

स्थिर शून्य netrc_skip_blanks(संरचना netrc_lexer *lexer)
{
  स्थिरांक अक्षर *s = lexer->pos;
  पर्यंत(*s) {
    curlx_str_passblanks(&s);
    पर्यंत(*s == '\r')
      ++s;
    जर(*s == '\n') {
      ++s;
    }
    अन्यथा
      थांबा;
  }
  lexer->pos = s;
}

स्थिर शून्य netrc_skip_to_empty_ओळ(संरचना netrc_lexer *lexer)
{
  स्थिरांक अक्षर *s = lexer->pos;
  पर्यंत(*s) {
    जर(*s == '\r')
      ++s;
    अन्यथा जर(*s == '\n') {
      ++s;
      पर्यंत(*s == '\r')
        ++s;
      जर(*s == '\n')
        जा out;
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
स्थिर NETRCcode netrc_lexer_quoted(संरचना netrc_lexer *lexer)
{
  NETRCcode rc = NETRC_SYNTAX_ERROR;
  स्थिरांक अक्षर *s = lexer->pos;
  bool escape = FALSE;
  CURLcode result;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  पर्यंत(*s) {
    अक्षर c = *s;
    जर(escape) {
      escape = FALSE;
      निवड(c) {
      स्थिती 'n':
        c = '\n';
        थांबा;
      स्थिती 'r':
        c = '\r';
        थांबा;
      स्थिती 't':
        c = '\t';
        थांबा;
      }
    }
    अन्यथा जर(c == '\\') {
      escape = TRUE;
      ++s;
      सुरूठेवा;
    }
    अन्यथा जर(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      result = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(result);
      जा out;
    }
    result = curlx_dyn_addn(&lexer->literal, &c, 1);
    जर(result) {
      rc = curl2netrc(result);
      जा out;
    }
    ++s;
  }
out:
  lexer->pos = s;
  परत rc;
}

स्थिर शून्य netrc_lexer_ढकला(संरचना netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

स्थिर NETRCcode netrc_lexer_next(संरचना netrc_lexer *lexer,
                                  bool want_literal)
{
  स्थिरांक अक्षर *s = lexer->pos, *start;
  NETRCcode rc = NETRC_OK;
  माप_प्रकार slen;
  CURLcode result;

  जर(lexer->pushed) {
    lexer->pushed = FALSE;
    जा out;
  }

  curlx_dyn_reset(&lexer->literal);
  netrc_skip_blanks(lexer);
  s = lexer->pos;

  निवड(*s) {
  स्थिती 0:
    lexer->token = NETRC_TOK_EOF;
    थांबा;
  स्थिती '\"':
    rc = netrc_lexer_quoted(lexer);
    lexer->token = NETRC_TOK_LITERAL;
    s = lexer->pos;
    थांबा;
  पूर्वनिर्धारित:
    /* unquoted token */
    start = s;
    पर्यंत(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - start;
    जर(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    जर(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      result = curlx_dyn_addn(&lexer->literal, start, slen);
      rc = curl2netrc(result);
    }
    अन्यथा जर((slen == 7) && curl_strnequal(start, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    अन्यथा जर((slen == 7) && curl_strnequal(start, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    अन्यथा जर((slen == 7) && curl_strnequal(start, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    अन्यथा जर((slen == 5) && curl_strnequal(start, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    अन्यथा जर((slen == 8) && curl_strnequal(start, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    अन्यथा जर((slen == 6) && curl_strnequal(start, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    अन्यथा {
      lexer->token = NETRC_TOK_JUNK;
    }
    थांबा;
  }

out:
#जर NETRC_DEBUG
  CURL_TRC_M(lexer->data, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#endif
  lexer->pos = s;
  परत rc;
}

संरचना netrc_scanner {
  संरचना netrc_lexer lexer;
  स्थिरांक अक्षर *hostname; /* non-NULL, machine to scan for */
  स्थिरांक अक्षर *user; /* maybe NULL, login to scan for */
  अक्षर *login;
  अक्षर *passwd;
  संरचना Curl_creds *creds;
  bool matches_host;
  bool found;
};

स्थिर शून्य netrc_scan_reset(संरचना netrc_scanner *sc)
{
  curlx_safefree(sc->login);
  curlx_safefree(sc->passwd);
  sc->matches_host = FALSE;
}

स्थिर शून्य netrc_scan_init(संरचना netrc_scanner *sc,
                            संरचना Curl_easy *data,
                            स्थिरांक अक्षर *content,
                            स्थिरांक अक्षर *hostname,
                            स्थिरांक अक्षर *user)
{
  स_रखो(sc, 0, आकार(*sc));
  netrc_lexer_init(&sc->lexer, data, content);
  sc->hostname = hostname;
  sc->user = (user && user[0]) ? user : रिक्त;
  netrc_scan_reset(sc);
}

स्थिर शून्य netrc_scan_cleanup(संरचना netrc_scanner *sc)
{
  netrc_scan_reset(sc);
  sc->hostname = रिक्त;
  sc->user = रिक्त;
  Curl_creds_unlink(&sc->creds);
  netrc_lexer_cleanup(&sc->lexer);
}

स्थिर NETRCcode netrc_scan_literal(संरचना netrc_scanner *sc,
                                    अक्षर **pdest)
{
  NETRCcode rc = netrc_lexer_next(&sc->lexer, TRUE);
  जर(!rc) {
    जर(sc->lexer.token == NETRC_TOK_LITERAL) {
      जर(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        जर(!*pdest)
          rc = NETRC_OUT_OF_MEMORY;
      }
    }
    अन्यथा
      netrc_lexer_ढकला(&sc->lexer);
  }
  परत rc;
}

स्थिर NETRCcode netrc_scan_end_entry(संरचना netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#जर NETRC_DEBUG
  CURL_TRC_M(sc->lexer.data,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#endif
  जर(sc->matches_host) {
    जर(sc->login) {
      जर(sc->user) {
        जर(Curl_timestrcmp(sc->user, sc->login))
          जा out;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->found = !!sc->passwd;
      }
      अन्यथा {
        sc->found = TRUE;
      }
    }
    अन्यथा जर(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->found = TRUE;
    }
    अन्यथा {
      /* entry has nothing interesting */
    }
    जर(sc->found) {
#जर NETRC_DEBUG
      CURL_TRC_M(sc->lexer.data, "[NETRC] entry match found");
#endif
      जर(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           रिक्त, रिक्त, रिक्त, CREDS_NETRC, &sc->creds))
        rc = NETRC_OUT_OF_MEMORY;
    }
  }
out:
  netrc_scan_reset(sc);
  परत rc;
}

स्थिर NETRCcode netrc_scan(संरचना Curl_easy *data,
                            स्थिरांक अक्षर *content,
                            स्थिरांक अक्षर *hostname,
                            स्थिरांक अक्षर *user,
                            संरचना Curl_creds **pcreds)
{
  संरचना netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_scan_init(&sc, data, content, hostname, user);

  पर्यंत(!rc && !sc.found) {
    rc = netrc_lexer_next(&sc.lexer, FALSE);
    जर(!rc) {
      /* Does this token end any previous entry? */
      निवड(sc.lexer.token) {
      स्थिती NETRC_TOK_EOF:
      स्थिती NETRC_TOK_MACHINE:
      स्थिती NETRC_TOK_DEFAULT:
      स्थिती NETRC_TOK_MACDEF:
        rc = netrc_scan_end_entry(&sc);
        जर(rc || sc.found)
          जा out;
        थांबा;
      पूर्वनिर्धारित:
        थांबा;
      }

      निवड(sc.lexer.token) {
      स्थिती NETRC_TOK_EOF:
        जा out;
      स्थिती NETRC_TOK_MACHINE:
        rc = netrc_lexer_next(&sc.lexer, TRUE);
        जर(!rc) {
          जर(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          अन्यथा {
            sc.matches_host = FALSE;
            netrc_lexer_ढकला(&sc.lexer);
          }
        }
        थांबा;
      स्थिती NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        थांबा;
      स्थिती NETRC_TOK_ACCOUNT:
        rc = netrc_scan_literal(&sc, रिक्त); /* ignore, not used */
        थांबा;
      स्थिती NETRC_TOK_LOGIN:
        rc = netrc_scan_literal(&sc, &sc.login);
        थांबा;
      स्थिती NETRC_TOK_PASSWD:
        rc = netrc_scan_literal(&sc, &sc.passwd);
        थांबा;
      स्थिती NETRC_TOK_MACDEF:
        netrc_skip_to_empty_ओळ(&sc.lexer);
        थांबा;
      स्थिती NETRC_TOK_LITERAL:
      स्थिती NETRC_TOK_JUNK:
      पूर्वनिर्धारित:
        /* skip this */
        थांबा;
      }
    }
  }

out:
  जर(!rc) {
    जर(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    अन्यथा
      rc = NETRC_NO_MATCH;
  }
  netrc_scan_cleanup(&sc);
  परत rc;
}

स्थिर NETRCcode file2memory(स्थिरांक अक्षर *filename, संरचना dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  खाता *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  जर(file) {
    curlx_struct_stat stat;
    जर((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      CURLcode result = CURLE_OK;
      bool eof;
      संरचना dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_ओळ);
      ret = NETRC_OK;
      करा {
        स्थिरांक अक्षर *ओळ;
        /* Curl_get_line always returns lines ending with a newline */
        result = Curl_get_ओळ(&linebuf, file, &eof);
        जर(!result) {
          ओळ = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&ओळ);
          जर(*ओळ == '#')
            सुरूठेवा;
          result = curlx_dyn_add(filebuf, ओळ);
        }
        जर(result) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(result);
          थांबा;
        }
      } पर्यंत(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  परत ret;
}

स्थिर NETRCcode netrc_scan_file(संरचना Curl_easy *data,
                                 संरचना store_netrc *store,
                                 स्थिरांक अक्षर *hostname,
                                 स्थिरांक अक्षर *user,
                                 स्थिरांक अक्षर *netrcfile,
                                 संरचना Curl_creds **pcreds)
{
  संरचना dynbuf *filebuf = &store->filebuf;

  जर(!store->loaded || म_भेद(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    जर(ret) {
      CURL_TRC_M(data, "[NETRC] could not load '%s'", netrcfile);
      परत ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    जर(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      परत NETRC_OUT_OF_MEMORY;
    }
    store->loaded = TRUE;
  }
  जर(!curlx_dyn_len(filebuf))
    परत NETRC_NO_MATCH;

  परत netrc_scan(data, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_scan(संरचना Curl_easy *data,
                          संरचना store_netrc *store,
                          स्थिरांक अक्षर *hostname,
                          स्थिरांक अक्षर *user,
                          स्थिरांक अक्षर *netrcfile,
                          संरचना Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(data, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  जर(!netrcfile) {
    अक्षर *home = रिक्त;
    अक्षर *homea = रिक्त;
    अक्षर *filealloc = रिक्त;
#जर defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    अक्षर pwbuf[1024];
#endif
    filealloc = curl_getenv("NETRC");
    जर(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      जर(homea) {
        home = homea;
#जर defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      अन्यथा {
        संरचना passwd pw, *pw_res;
        जर(!getpwuid_r(geteuid(), &pw, pwbuf, आकार(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#किंवाजर defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      अन्यथा {
        संरचना passwd *pw;
        pw = getpwuid(geteuid());
        जर(pw) {
          home = pw->pw_dir;
        }
#किंवाजर defined(_WIN32)
      }
      अन्यथा {
        homea = curl_getenv("USERPROFILE");
        जर(homea) {
          home = homea;
        }
#endif
      }

      जर(!home)
        परत NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      जर(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_OUT_OF_MEMORY;
        जा out;
      }
    }
    retcode = netrc_scan_file(
      data, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#ifdef _WIN32
    जर(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      जर(!filealloc) {
        curlx_free(homea);
        परत NETRC_OUT_OF_MEMORY;
      }
      retcode = netrc_scan_file(
        data, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#endif
    curlx_free(homea);
  }
  अन्यथा
    retcode = netrc_scan_file(
      data, store, hostname, user, netrcfile, pcreds);

out:
  जर(retcode)
    Curl_creds_unlink(pcreds);
  परत retcode;
}

शून्य Curl_netrc_init(संरचना store_netrc *store)
{
  curlx_dyn_init(&store->filebuf, MAX_NETRC_FILE);
  store->loaded = FALSE;
  store->filename = रिक्त;
}
शून्य Curl_netrc_cleanup(संरचना store_netrc *store)
{
  curlx_dyn_free(&store->filebuf);
  curlx_safefree(store->filename);
  store->loaded = FALSE;
}

स्थिरांक अक्षर *Curl_netrc_strerror(NETRCcode ret)
{
  निवड(ret) {
  पूर्वनिर्धारित:
    परत ""; /* not a legit error */
  स्थिती NETRC_FILE_MISSING:
    परत "no such file";
  स्थिती NETRC_NO_MATCH:
    परत "no matching entry";
  स्थिती NETRC_OUT_OF_MEMORY:
    परत "out of memory";
  स्थिती NETRC_SYNTAX_ERROR:
    परत "syntax error";
  }
  /* never reached */
}

#endif /* !CURL_DISABLE_NETRC */
