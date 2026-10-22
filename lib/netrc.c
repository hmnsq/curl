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

#define MAX_NETRC_पङ्क्तिः  16384
#define MAX_NETRC_FILE  (128 * 1024)
#define MAX_NETRC_TOKEN 4096

#define NETRC_DEBUG   0

/* convert a dynbuf call CURLcode error to a NETRCcode error */
#define curl2netrc(r)                                  \
  ((!(r)) ? NETRC_OK : (((r) == CURLE_OUT_OF_MEMORY) ? \
   NETRC_OUT_OF_MEMORY : NETRC_SYNTAX_ERROR))

प्रकारनिर्देश गणना {
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
  ध्रुव वर्ण *content;
  ध्रुव वर्ण *pos;
  संरचना dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#जइ NETRC_DEBUG
स्थिर ध्रुव वर्ण *netrc_tokenstr(curl_netrc_token token)
{
  विकल्प(token) {
  पक्ष NETRC_TOK_EOF:
    प्रत्यागम "[EOF]";
  पक्ष NETRC_TOK_LITERAL:
    प्रत्यागम "[LITERAL]";
  पक्ष NETRC_TOK_MACHINE:
    प्रत्यागम "[MACHINE]";
  पक्ष NETRC_TOK_DEFAULT:
    प्रत्यागम "[DEFAULT]";
  पक्ष NETRC_TOK_ACCOUNT:
    प्रत्यागम "[ACCOUNT]";
  पक्ष NETRC_TOK_LOGIN:
    प्रत्यागम "[LOGIN]";
  पक्ष NETRC_TOK_PASSWD:
    प्रत्यागम "[PASSWORD]";
  पक्ष NETRC_TOK_MACDEF:
    प्रत्यागम "[MACDEF]";
  पक्ष NETRC_TOK_JUNK:
    प्रत्यागम "[JUNK]";
  स्वभाव:
    प्रत्यागम "[???]";
  }
}
#endif

स्थिर शून्य netrc_lexer_init(संरचना netrc_lexer *lexer,
                             संरचना Curl_easy *data,
                             ध्रुव वर्ण *content)
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
  ध्रुव वर्ण *s = lexer->pos;
  जाव(*s) {
    curlx_str_passblanks(&s);
    जाव(*s == '\r')
      ++s;
    जइ(*s == '\n') {
      ++s;
    }
    अण्णहा
      भङ्ग;
  }
  lexer->pos = s;
}

स्थिर शून्य netrc_skip_to_empty_पङ्क्तिः(संरचना netrc_lexer *lexer)
{
  ध्रुव वर्ण *s = lexer->pos;
  जाव(*s) {
    जइ(*s == '\r')
      ++s;
    अण्णहा जइ(*s == '\n') {
      ++s;
      जाव(*s == '\r')
        ++s;
      जइ(*s == '\n')
        गच्छ out;
    }
    अण्णहा
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
  ध्रुव वर्ण *s = lexer->pos;
  bool escape = FALSE;
  CURLcode result;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  जाव(*s) {
    वर्ण c = *s;
    जइ(escape) {
      escape = FALSE;
      विकल्प(c) {
      पक्ष 'n':
        c = '\n';
        भङ्ग;
      पक्ष 'r':
        c = '\r';
        भङ्ग;
      पक्ष 't':
        c = '\t';
        भङ्ग;
      }
    }
    अण्णहा जइ(c == '\\') {
      escape = TRUE;
      ++s;
      अनुवर्त;
    }
    अण्णहा जइ(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      result = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(result);
      गच्छ out;
    }
    result = curlx_dyn_addn(&lexer->literal, &c, 1);
    जइ(result) {
      rc = curl2netrc(result);
      गच्छ out;
    }
    ++s;
  }
out:
  lexer->pos = s;
  प्रत्यागम rc;
}

स्थिर शून्य netrc_lexer_प्रक्षिप(संरचना netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

स्थिर NETRCcode netrc_lexer_next(संरचना netrc_lexer *lexer,
                                  bool want_literal)
{
  ध्रुव वर्ण *s = lexer->pos, *start;
  NETRCcode rc = NETRC_OK;
  size_t slen;
  CURLcode result;

  जइ(lexer->pushed) {
    lexer->pushed = FALSE;
    गच्छ out;
  }

  curlx_dyn_reset(&lexer->literal);
  netrc_skip_blanks(lexer);
  s = lexer->pos;

  विकल्प(*s) {
  पक्ष 0:
    lexer->token = NETRC_TOK_EOF;
    भङ्ग;
  पक्ष '\"':
    rc = netrc_lexer_quoted(lexer);
    lexer->token = NETRC_TOK_LITERAL;
    s = lexer->pos;
    भङ्ग;
  स्वभाव:
    /* unquoted token */
    start = s;
    जाव(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - start;
    जइ(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    जइ(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      result = curlx_dyn_addn(&lexer->literal, start, slen);
      rc = curl2netrc(result);
    }
    अण्णहा जइ((slen == 7) && curl_strnequal(start, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    अण्णहा जइ((slen == 7) && curl_strnequal(start, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    अण्णहा जइ((slen == 7) && curl_strnequal(start, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    अण्णहा जइ((slen == 5) && curl_strnequal(start, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    अण्णहा जइ((slen == 8) && curl_strnequal(start, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    अण्णहा जइ((slen == 6) && curl_strnequal(start, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    अण्णहा {
      lexer->token = NETRC_TOK_JUNK;
    }
    भङ्ग;
  }

out:
#जइ NETRC_DEBUG
  CURL_TRC_M(lexer->data, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#endif
  lexer->pos = s;
  प्रत्यागम rc;
}

संरचना netrc_scanner {
  संरचना netrc_lexer lexer;
  ध्रुव वर्ण *hostname; /* non-NULL, machine to scan for */
  ध्रुव वर्ण *user; /* maybe NULL, login to scan for */
  वर्ण *login;
  वर्ण *passwd;
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
                            ध्रुव वर्ण *content,
                            ध्रुव वर्ण *hostname,
                            ध्रुव वर्ण *user)
{
  memset(sc, 0, परिमाण(*sc));
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
                                    वर्ण **pdest)
{
  NETRCcode rc = netrc_lexer_next(&sc->lexer, TRUE);
  जइ(!rc) {
    जइ(sc->lexer.token == NETRC_TOK_LITERAL) {
      जइ(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        जइ(!*pdest)
          rc = NETRC_OUT_OF_MEMORY;
      }
    }
    अण्णहा
      netrc_lexer_प्रक्षिप(&sc->lexer);
  }
  प्रत्यागम rc;
}

स्थिर NETRCcode netrc_scan_end_entry(संरचना netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#जइ NETRC_DEBUG
  CURL_TRC_M(sc->lexer.data,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#endif
  जइ(sc->matches_host) {
    जइ(sc->login) {
      जइ(sc->user) {
        जइ(Curl_timestrcmp(sc->user, sc->login))
          गच्छ out;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->found = !!sc->passwd;
      }
      अण्णहा {
        sc->found = TRUE;
      }
    }
    अण्णहा जइ(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->found = TRUE;
    }
    अण्णहा {
      /* entry has nothing interesting */
    }
    जइ(sc->found) {
#जइ NETRC_DEBUG
      CURL_TRC_M(sc->lexer.data, "[NETRC] entry match found");
#endif
      जइ(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           रिक्त, रिक्त, रिक्त, CREDS_NETRC, &sc->creds))
        rc = NETRC_OUT_OF_MEMORY;
    }
  }
out:
  netrc_scan_reset(sc);
  प्रत्यागम rc;
}

स्थिर NETRCcode netrc_scan(संरचना Curl_easy *data,
                            ध्रुव वर्ण *content,
                            ध्रुव वर्ण *hostname,
                            ध्रुव वर्ण *user,
                            संरचना Curl_creds **pcreds)
{
  संरचना netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_scan_init(&sc, data, content, hostname, user);

  जाव(!rc && !sc.found) {
    rc = netrc_lexer_next(&sc.lexer, FALSE);
    जइ(!rc) {
      /* Does this token end any previous entry? */
      विकल्प(sc.lexer.token) {
      पक्ष NETRC_TOK_EOF:
      पक्ष NETRC_TOK_MACHINE:
      पक्ष NETRC_TOK_DEFAULT:
      पक्ष NETRC_TOK_MACDEF:
        rc = netrc_scan_end_entry(&sc);
        जइ(rc || sc.found)
          गच्छ out;
        भङ्ग;
      स्वभाव:
        भङ्ग;
      }

      विकल्प(sc.lexer.token) {
      पक्ष NETRC_TOK_EOF:
        गच्छ out;
      पक्ष NETRC_TOK_MACHINE:
        rc = netrc_lexer_next(&sc.lexer, TRUE);
        जइ(!rc) {
          जइ(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          अण्णहा {
            sc.matches_host = FALSE;
            netrc_lexer_प्रक्षिप(&sc.lexer);
          }
        }
        भङ्ग;
      पक्ष NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        भङ्ग;
      पक्ष NETRC_TOK_ACCOUNT:
        rc = netrc_scan_literal(&sc, रिक्त); /* ignore, not used */
        भङ्ग;
      पक्ष NETRC_TOK_LOGIN:
        rc = netrc_scan_literal(&sc, &sc.login);
        भङ्ग;
      पक्ष NETRC_TOK_PASSWD:
        rc = netrc_scan_literal(&sc, &sc.passwd);
        भङ्ग;
      पक्ष NETRC_TOK_MACDEF:
        netrc_skip_to_empty_पङ्क्तिः(&sc.lexer);
        भङ्ग;
      पक्ष NETRC_TOK_LITERAL:
      पक्ष NETRC_TOK_JUNK:
      स्वभाव:
        /* skip this */
        भङ्ग;
      }
    }
  }

out:
  जइ(!rc) {
    जइ(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    अण्णहा
      rc = NETRC_NO_MATCH;
  }
  netrc_scan_cleanup(&sc);
  प्रत्यागम rc;
}

स्थिर NETRCcode file2memory(ध्रुव वर्ण *filename, संरचना dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  जइ(file) {
    curlx_struct_stat stat;
    जइ((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      CURLcode result = CURLE_OK;
      bool eof;
      संरचना dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_पङ्क्तिः);
      ret = NETRC_OK;
      करेहि {
        ध्रुव वर्ण *पङ्क्तिः;
        /* Curl_get_line always returns lines ending with a newline */
        result = Curl_get_पङ्क्तिः(&linebuf, file, &eof);
        जइ(!result) {
          पङ्क्तिः = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&पङ्क्तिः);
          जइ(*पङ्क्तिः == '#')
            अनुवर्त;
          result = curlx_dyn_add(filebuf, पङ्क्तिः);
        }
        जइ(result) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(result);
          भङ्ग;
        }
      } जाव(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  प्रत्यागम ret;
}

स्थिर NETRCcode netrc_scan_file(संरचना Curl_easy *data,
                                 संरचना store_netrc *store,
                                 ध्रुव वर्ण *hostname,
                                 ध्रुव वर्ण *user,
                                 ध्रुव वर्ण *netrcfile,
                                 संरचना Curl_creds **pcreds)
{
  संरचना dynbuf *filebuf = &store->filebuf;

  जइ(!store->loaded || strcmp(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    जइ(ret) {
      CURL_TRC_M(data, "[NETRC] could not load '%s'", netrcfile);
      प्रत्यागम ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    जइ(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      प्रत्यागम NETRC_OUT_OF_MEMORY;
    }
    store->loaded = TRUE;
  }
  जइ(!curlx_dyn_len(filebuf))
    प्रत्यागम NETRC_NO_MATCH;

  प्रत्यागम netrc_scan(data, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_scan(संरचना Curl_easy *data,
                          संरचना store_netrc *store,
                          ध्रुव वर्ण *hostname,
                          ध्रुव वर्ण *user,
                          ध्रुव वर्ण *netrcfile,
                          संरचना Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(data, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  जइ(!netrcfile) {
    वर्ण *home = रिक्त;
    वर्ण *homea = रिक्त;
    वर्ण *filealloc = रिक्त;
#जइ defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    वर्ण pwbuf[1024];
#endif
    filealloc = curl_getenv("NETRC");
    जइ(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      जइ(homea) {
        home = homea;
#जइ defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      अण्णहा {
        संरचना passwd pw, *pw_res;
        जइ(!getpwuid_r(geteuid(), &pw, pwbuf, परिमाण(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#नोचेद्यदि defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      अण्णहा {
        संरचना passwd *pw;
        pw = getpwuid(geteuid());
        जइ(pw) {
          home = pw->pw_dir;
        }
#नोचेद्यदि defined(_WIN32)
      }
      अण्णहा {
        homea = curl_getenv("USERPROFILE");
        जइ(homea) {
          home = homea;
        }
#endif
      }

      जइ(!home)
        प्रत्यागम NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      जइ(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_OUT_OF_MEMORY;
        गच्छ out;
      }
    }
    retcode = netrc_scan_file(
      data, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#ifdef _WIN32
    जइ(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      जइ(!filealloc) {
        curlx_free(homea);
        प्रत्यागम NETRC_OUT_OF_MEMORY;
      }
      retcode = netrc_scan_file(
        data, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#endif
    curlx_free(homea);
  }
  अण्णहा
    retcode = netrc_scan_file(
      data, store, hostname, user, netrcfile, pcreds);

out:
  जइ(retcode)
    Curl_creds_unlink(pcreds);
  प्रत्यागम retcode;
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

ध्रुव वर्ण *Curl_netrc_strerror(NETRCcode ret)
{
  विकल्प(ret) {
  स्वभाव:
    प्रत्यागम ""; /* not a legit error */
  पक्ष NETRC_FILE_MISSING:
    प्रत्यागम "no such file";
  पक्ष NETRC_NO_MATCH:
    प्रत्यागम "no matching entry";
  पक्ष NETRC_OUT_OF_MEMORY:
    प्रत्यागम "out of memory";
  पक्ष NETRC_SYNTAX_ERROR:
    प्रत्यागम "syntax error";
  }
  /* never reached */
}

#endif /* !CURL_DISABLE_NETRC */
