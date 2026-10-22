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

#define MAX_NETRC_पङ्क्ति  16384
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
  अचर वर्ण *content;
  अचर वर्ण *pos;
  संरचना dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#यदि NETRC_DEBUG
स्थिर अचर वर्ण *netrc_tokenstr(curl_netrc_token token)
{
  विकल्प(token) {
  पक्ष NETRC_TOK_EOF:
    फर्काऊ "[EOF]";
  पक्ष NETRC_TOK_LITERAL:
    फर्काऊ "[LITERAL]";
  पक्ष NETRC_TOK_MACHINE:
    फर्काऊ "[MACHINE]";
  पक्ष NETRC_TOK_DEFAULT:
    फर्काऊ "[DEFAULT]";
  पक्ष NETRC_TOK_ACCOUNT:
    फर्काऊ "[ACCOUNT]";
  पक्ष NETRC_TOK_LOGIN:
    फर्काऊ "[LOGIN]";
  पक्ष NETRC_TOK_PASSWD:
    फर्काऊ "[PASSWORD]";
  पक्ष NETRC_TOK_MACDEF:
    फर्काऊ "[MACDEF]";
  पक्ष NETRC_TOK_JUNK:
    फर्काऊ "[JUNK]";
  स्वभाव:
    फर्काऊ "[???]";
  }
}
#endif

स्थिर शून्य netrc_lexer_init(संरचना netrc_lexer *lexer,
                             संरचना Curl_easy *data,
                             अचर वर्ण *content)
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
  अचर वर्ण *s = lexer->pos;
  जबसम्म(*s) {
    curlx_str_passblanks(&s);
    जबसम्म(*s == '\r')
      ++s;
    यदि(*s == '\n') {
      ++s;
    }
    अन्यथा
      रोक;
  }
  lexer->pos = s;
}

स्थिर शून्य netrc_skip_to_empty_पङ्क्ति(संरचना netrc_lexer *lexer)
{
  अचर वर्ण *s = lexer->pos;
  जबसम्म(*s) {
    यदि(*s == '\r')
      ++s;
    अन्यथा यदि(*s == '\n') {
      ++s;
      जबसम्म(*s == '\r')
        ++s;
      यदि(*s == '\n')
        गच्छ out;
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
  अचर वर्ण *s = lexer->pos;
  bool escape = FALSE;
  CURLcode result;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  जबसम्म(*s) {
    वर्ण c = *s;
    यदि(escape) {
      escape = FALSE;
      विकल्प(c) {
      पक्ष 'n':
        c = '\n';
        रोक;
      पक्ष 'r':
        c = '\r';
        रोक;
      पक्ष 't':
        c = '\t';
        रोक;
      }
    }
    अन्यथा यदि(c == '\\') {
      escape = TRUE;
      ++s;
      जारी;
    }
    अन्यथा यदि(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      result = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(result);
      गच्छ out;
    }
    result = curlx_dyn_addn(&lexer->literal, &c, 1);
    यदि(result) {
      rc = curl2netrc(result);
      गच्छ out;
    }
    ++s;
  }
out:
  lexer->pos = s;
  फर्काऊ rc;
}

स्थिर शून्य netrc_lexer_थप(संरचना netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

स्थिर NETRCcode netrc_lexer_next(संरचना netrc_lexer *lexer,
                                  bool want_literal)
{
  अचर वर्ण *s = lexer->pos, *start;
  NETRCcode rc = NETRC_OK;
  माप_प्रकार slen;
  CURLcode result;

  यदि(lexer->pushed) {
    lexer->pushed = FALSE;
    गच्छ out;
  }

  curlx_dyn_reset(&lexer->literal);
  netrc_skip_blanks(lexer);
  s = lexer->pos;

  विकल्प(*s) {
  पक्ष 0:
    lexer->token = NETRC_TOK_EOF;
    रोक;
  पक्ष '\"':
    rc = netrc_lexer_quoted(lexer);
    lexer->token = NETRC_TOK_LITERAL;
    s = lexer->pos;
    रोक;
  स्वभाव:
    /* unquoted token */
    start = s;
    जबसम्म(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - start;
    यदि(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    यदि(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      result = curlx_dyn_addn(&lexer->literal, start, slen);
      rc = curl2netrc(result);
    }
    अन्यथा यदि((slen == 7) && curl_strnequal(start, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    अन्यथा यदि((slen == 7) && curl_strnequal(start, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    अन्यथा यदि((slen == 7) && curl_strnequal(start, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    अन्यथा यदि((slen == 5) && curl_strnequal(start, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    अन्यथा यदि((slen == 8) && curl_strnequal(start, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    अन्यथा यदि((slen == 6) && curl_strnequal(start, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    अन्यथा {
      lexer->token = NETRC_TOK_JUNK;
    }
    रोक;
  }

out:
#यदि NETRC_DEBUG
  CURL_TRC_M(lexer->data, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#endif
  lexer->pos = s;
  फर्काऊ rc;
}

संरचना netrc_scanner {
  संरचना netrc_lexer lexer;
  अचर वर्ण *hostname; /* non-NULL, machine to scan for */
  अचर वर्ण *user; /* maybe NULL, login to scan for */
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
                            अचर वर्ण *content,
                            अचर वर्ण *hostname,
                            अचर वर्ण *user)
{
  स_रखो(sc, 0, परिमाण(*sc));
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
  यदि(!rc) {
    यदि(sc->lexer.token == NETRC_TOK_LITERAL) {
      यदि(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        यदि(!*pdest)
          rc = NETRC_OUT_OF_MEMORY;
      }
    }
    अन्यथा
      netrc_lexer_थप(&sc->lexer);
  }
  फर्काऊ rc;
}

स्थिर NETRCcode netrc_scan_end_entry(संरचना netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#यदि NETRC_DEBUG
  CURL_TRC_M(sc->lexer.data,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#endif
  यदि(sc->matches_host) {
    यदि(sc->login) {
      यदि(sc->user) {
        यदि(Curl_timestrcmp(sc->user, sc->login))
          गच्छ out;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->found = !!sc->passwd;
      }
      अन्यथा {
        sc->found = TRUE;
      }
    }
    अन्यथा यदि(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->found = TRUE;
    }
    अन्यथा {
      /* entry has nothing interesting */
    }
    यदि(sc->found) {
#यदि NETRC_DEBUG
      CURL_TRC_M(sc->lexer.data, "[NETRC] entry match found");
#endif
      यदि(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           रिक्त, रिक्त, रिक्त, CREDS_NETRC, &sc->creds))
        rc = NETRC_OUT_OF_MEMORY;
    }
  }
out:
  netrc_scan_reset(sc);
  फर्काऊ rc;
}

स्थिर NETRCcode netrc_scan(संरचना Curl_easy *data,
                            अचर वर्ण *content,
                            अचर वर्ण *hostname,
                            अचर वर्ण *user,
                            संरचना Curl_creds **pcreds)
{
  संरचना netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_scan_init(&sc, data, content, hostname, user);

  जबसम्म(!rc && !sc.found) {
    rc = netrc_lexer_next(&sc.lexer, FALSE);
    यदि(!rc) {
      /* Does this token end any previous entry? */
      विकल्प(sc.lexer.token) {
      पक्ष NETRC_TOK_EOF:
      पक्ष NETRC_TOK_MACHINE:
      पक्ष NETRC_TOK_DEFAULT:
      पक्ष NETRC_TOK_MACDEF:
        rc = netrc_scan_end_entry(&sc);
        यदि(rc || sc.found)
          गच्छ out;
        रोक;
      स्वभाव:
        रोक;
      }

      विकल्प(sc.lexer.token) {
      पक्ष NETRC_TOK_EOF:
        गच्छ out;
      पक्ष NETRC_TOK_MACHINE:
        rc = netrc_lexer_next(&sc.lexer, TRUE);
        यदि(!rc) {
          यदि(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          अन्यथा {
            sc.matches_host = FALSE;
            netrc_lexer_थप(&sc.lexer);
          }
        }
        रोक;
      पक्ष NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        रोक;
      पक्ष NETRC_TOK_ACCOUNT:
        rc = netrc_scan_literal(&sc, रिक्त); /* ignore, not used */
        रोक;
      पक्ष NETRC_TOK_LOGIN:
        rc = netrc_scan_literal(&sc, &sc.login);
        रोक;
      पक्ष NETRC_TOK_PASSWD:
        rc = netrc_scan_literal(&sc, &sc.passwd);
        रोक;
      पक्ष NETRC_TOK_MACDEF:
        netrc_skip_to_empty_पङ्क्ति(&sc.lexer);
        रोक;
      पक्ष NETRC_TOK_LITERAL:
      पक्ष NETRC_TOK_JUNK:
      स्वभाव:
        /* skip this */
        रोक;
      }
    }
  }

out:
  यदि(!rc) {
    यदि(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    अन्यथा
      rc = NETRC_NO_MATCH;
  }
  netrc_scan_cleanup(&sc);
  फर्काऊ rc;
}

स्थिर NETRCcode file2memory(अचर वर्ण *filename, संरचना dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  खाता *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  यदि(file) {
    curlx_struct_stat stat;
    यदि((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      CURLcode result = CURLE_OK;
      bool eof;
      संरचना dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_पङ्क्ति);
      ret = NETRC_OK;
      गर {
        अचर वर्ण *पङ्क्ति;
        /* Curl_get_line always returns lines ending with a newline */
        result = Curl_get_पङ्क्ति(&linebuf, file, &eof);
        यदि(!result) {
          पङ्क्ति = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&पङ्क्ति);
          यदि(*पङ्क्ति == '#')
            जारी;
          result = curlx_dyn_add(filebuf, पङ्क्ति);
        }
        यदि(result) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(result);
          रोक;
        }
      } जबसम्म(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  फर्काऊ ret;
}

स्थिर NETRCcode netrc_scan_file(संरचना Curl_easy *data,
                                 संरचना store_netrc *store,
                                 अचर वर्ण *hostname,
                                 अचर वर्ण *user,
                                 अचर वर्ण *netrcfile,
                                 संरचना Curl_creds **pcreds)
{
  संरचना dynbuf *filebuf = &store->filebuf;

  यदि(!store->loaded || म_भेद(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    यदि(ret) {
      CURL_TRC_M(data, "[NETRC] could not load '%s'", netrcfile);
      फर्काऊ ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    यदि(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      फर्काऊ NETRC_OUT_OF_MEMORY;
    }
    store->loaded = TRUE;
  }
  यदि(!curlx_dyn_len(filebuf))
    फर्काऊ NETRC_NO_MATCH;

  फर्काऊ netrc_scan(data, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_scan(संरचना Curl_easy *data,
                          संरचना store_netrc *store,
                          अचर वर्ण *hostname,
                          अचर वर्ण *user,
                          अचर वर्ण *netrcfile,
                          संरचना Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(data, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  यदि(!netrcfile) {
    वर्ण *home = रिक्त;
    वर्ण *homea = रिक्त;
    वर्ण *filealloc = रिक्त;
#यदि defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    वर्ण pwbuf[1024];
#endif
    filealloc = curl_getenv("NETRC");
    यदि(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      यदि(homea) {
        home = homea;
#यदि defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      अन्यथा {
        संरचना passwd pw, *pw_res;
        यदि(!getpwuid_r(geteuid(), &pw, pwbuf, परिमाण(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#अथवायदि defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      अन्यथा {
        संरचना passwd *pw;
        pw = getpwuid(geteuid());
        यदि(pw) {
          home = pw->pw_dir;
        }
#अथवायदि defined(_WIN32)
      }
      अन्यथा {
        homea = curl_getenv("USERPROFILE");
        यदि(homea) {
          home = homea;
        }
#endif
      }

      यदि(!home)
        फर्काऊ NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      यदि(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_OUT_OF_MEMORY;
        गच्छ out;
      }
    }
    retcode = netrc_scan_file(
      data, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#ifdef _WIN32
    यदि(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      यदि(!filealloc) {
        curlx_free(homea);
        फर्काऊ NETRC_OUT_OF_MEMORY;
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
  यदि(retcode)
    Curl_creds_unlink(pcreds);
  फर्काऊ retcode;
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

अचर वर्ण *Curl_netrc_strerror(NETRCcode ret)
{
  विकल्प(ret) {
  स्वभाव:
    फर्काऊ ""; /* not a legit error */
  पक्ष NETRC_FILE_MISSING:
    फर्काऊ "no such file";
  पक्ष NETRC_NO_MATCH:
    फर्काऊ "no matching entry";
  पक्ष NETRC_OUT_OF_MEMORY:
    फर्काऊ "out of memory";
  पक्ष NETRC_SYNTAX_ERROR:
    फर्काऊ "syntax error";
  }
  /* never reached */
}

#endif /* !CURL_DISABLE_NETRC */
