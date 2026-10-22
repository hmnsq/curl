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

#यदि_अपरिभाषितम् CURL_DISABLE_NETRC

#यदि_परिभाषितम् HAVE_PWD_H
#यदि_परिभाषितम् __AMIGA__
#अपरिभाषय __NO_NET_API /* required for AmigaOS to declare getpwuid() */
#यदि_अन्तः
#अन्तर्भाव <pwd.h>
#यदि_परिभाषितम् __AMIGA__
#परिभाषय __NO_NET_API
#यदि_अन्तः
#यदि_अन्तः

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

#परिभाषय MAX_NETRC_पङ्क्तिः  16384
#परिभाषय MAX_NETRC_FILE  (128 * 1024)
#परिभाषय MAX_NETRC_TOKEN 4096

#परिभाषय NETRC_DEBUG   0

/* convert a dynbuf call CURLcode error to a NETRCcode error */
#परिभाषय curl2netrc(r)                                  \
  ((!(r)) ? NETRC_OK : (((r) == CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः) ? \
   NETRC_बहिर्गमनम्_सम्बन्धि_स्मृतिः : NETRC_SYNTAX_ERROR))

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
  संरचना Curl_easy *दत्त;
  ध्रुव वर्ण *content;
  ध्रुव वर्ण *pos;
  संरचना dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#यदि NETRC_DEBUG
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
#यदि_अन्तः

स्थिर शून्य netrc_lexer_init(संरचना netrc_lexer *lexer,
                             संरचना Curl_easy *दत्त,
                             ध्रुव वर्ण *content)
{
  curlx_dyn_init(&lexer->literal, MAX_NETRC_TOKEN);
  lexer->दत्त = दत्त;
  lexer->content = lexer->pos = content;
}

स्थिर शून्य netrc_lexer_cleanup(संरचना netrc_lexer *lexer)
{
  lexer->content = lexer->pos = रिक्त;
  lexer->दत्त = रिक्त;
  curlx_dyn_free(&lexer->literal);
}

स्थिर शून्य netrc_skip_blanks(संरचना netrc_lexer *lexer)
{
  ध्रुव वर्ण *s = lexer->pos;
  यावत्(*s) {
    curlx_str_passblanks(&s);
    यावत्(*s == '\r')
      ++s;
    यदि(*s == '\n') {
      ++s;
    }
    अन्यथा
      भङ्ग;
  }
  lexer->pos = s;
}

स्थिर शून्य netrc_skip_गन्तव्यम्_रिक्तानि_पङ्क्तिः(संरचना netrc_lexer *lexer)
{
  ध्रुव वर्ण *s = lexer->pos;
  यावत्(*s) {
    यदि(*s == '\r')
      ++s;
    अन्यथा यदि(*s == '\n') {
      ++s;
      यावत्(*s == '\r')
        ++s;
      यदि(*s == '\n')
        गच्छ बहिर्गमनम्;
    }
    अन्यथा
      ++s;
  }
बहिर्गमनम्:
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
  CURLcode परिणाम;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  यावत्(*s) {
    वर्ण c = *s;
    यदि(escape) {
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
    अन्यथा यदि(c == '\\') {
      escape = TRUE;
      ++s;
      अनुवर्त;
    }
    अन्यथा यदि(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      परिणाम = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(परिणाम);
      गच्छ बहिर्गमनम्;
    }
    परिणाम = curlx_dyn_addn(&lexer->literal, &c, 1);
    यदि(परिणाम) {
      rc = curl2netrc(परिणाम);
      गच्छ बहिर्गमनम्;
    }
    ++s;
  }
बहिर्गमनम्:
  lexer->pos = s;
  प्रत्यागम rc;
}

स्थिर शून्य netrc_lexer_प्रक्षिप(संरचना netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

स्थिर NETRCcode netrc_lexer_अग्र(संरचना netrc_lexer *lexer,
                                  bool want_literal)
{
  ध्रुव वर्ण *s = lexer->pos, *start;
  NETRCcode rc = NETRC_OK;
  size_t slen;
  CURLcode परिणाम;

  यदि(lexer->pushed) {
    lexer->pushed = FALSE;
    गच्छ बहिर्गमनम्;
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
    यावत्(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - start;
    यदि(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    यदि(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      परिणाम = curlx_dyn_addn(&lexer->literal, start, slen);
      rc = curl2netrc(परिणाम);
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
    भङ्ग;
  }

बहिर्गमनम्:
#यदि NETRC_DEBUG
  CURL_TRC_M(lexer->दत्त, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#यदि_अन्तः
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
  bool प्राप्तम्;
};

स्थिर शून्य netrc_अवलोकनम्_reset(संरचना netrc_scanner *sc)
{
  curlx_safefree(sc->login);
  curlx_safefree(sc->passwd);
  sc->matches_host = FALSE;
}

स्थिर शून्य netrc_अवलोकनम्_init(संरचना netrc_scanner *sc,
                            संरचना Curl_easy *दत्त,
                            ध्रुव वर्ण *content,
                            ध्रुव वर्ण *hostname,
                            ध्रुव वर्ण *user)
{
  स्मृतिं_पूरय(sc, 0, परिमाण(*sc));
  netrc_lexer_init(&sc->lexer, दत्त, content);
  sc->hostname = hostname;
  sc->user = (user && user[0]) ? user : रिक्त;
  netrc_अवलोकनम्_reset(sc);
}

स्थिर शून्य netrc_अवलोकनम्_cleanup(संरचना netrc_scanner *sc)
{
  netrc_अवलोकनम्_reset(sc);
  sc->hostname = रिक्त;
  sc->user = रिक्त;
  Curl_creds_unlink(&sc->creds);
  netrc_lexer_cleanup(&sc->lexer);
}

स्थिर NETRCcode netrc_अवलोकनम्_literal(संरचना netrc_scanner *sc,
                                    वर्ण **pdest)
{
  NETRCcode rc = netrc_lexer_अग्र(&sc->lexer, TRUE);
  यदि(!rc) {
    यदि(sc->lexer.token == NETRC_TOK_LITERAL) {
      यदि(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        यदि(!*pdest)
          rc = NETRC_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
    }
    अन्यथा
      netrc_lexer_प्रक्षिप(&sc->lexer);
  }
  प्रत्यागम rc;
}

स्थिर NETRCcode netrc_अवलोकनम्_अन्तः_प्रविष्टिः(संरचना netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#यदि NETRC_DEBUG
  CURL_TRC_M(sc->lexer.दत्त,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#यदि_अन्तः
  यदि(sc->matches_host) {
    यदि(sc->login) {
      यदि(sc->user) {
        यदि(Curl_timestrcmp(sc->user, sc->login))
          गच्छ बहिर्गमनम्;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->प्राप्तम् = !!sc->passwd;
      }
      अन्यथा {
        sc->प्राप्तम् = TRUE;
      }
    }
    अन्यथा यदि(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->प्राप्तम् = TRUE;
    }
    अन्यथा {
      /* entry has nothing interesting */
    }
    यदि(sc->प्राप्तम्) {
#यदि NETRC_DEBUG
      CURL_TRC_M(sc->lexer.दत्त, "[NETRC] entry match found");
#यदि_अन्तः
      यदि(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           रिक्त, रिक्त, रिक्त, CREDS_NETRC, &sc->creds))
        rc = NETRC_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
  }
बहिर्गमनम्:
  netrc_अवलोकनम्_reset(sc);
  प्रत्यागम rc;
}

स्थिर NETRCcode netrc_अवलोकनम्(संरचना Curl_easy *दत्त,
                            ध्रुव वर्ण *content,
                            ध्रुव वर्ण *hostname,
                            ध्रुव वर्ण *user,
                            संरचना Curl_creds **pcreds)
{
  संरचना netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_अवलोकनम्_init(&sc, दत्त, content, hostname, user);

  यावत्(!rc && !sc.प्राप्तम्) {
    rc = netrc_lexer_अग्र(&sc.lexer, FALSE);
    यदि(!rc) {
      /* Does this token end any previous entry? */
      विकल्प(sc.lexer.token) {
      पक्ष NETRC_TOK_EOF:
      पक्ष NETRC_TOK_MACHINE:
      पक्ष NETRC_TOK_DEFAULT:
      पक्ष NETRC_TOK_MACDEF:
        rc = netrc_अवलोकनम्_अन्तः_प्रविष्टिः(&sc);
        यदि(rc || sc.प्राप्तम्)
          गच्छ बहिर्गमनम्;
        भङ्ग;
      स्वभाव:
        भङ्ग;
      }

      विकल्प(sc.lexer.token) {
      पक्ष NETRC_TOK_EOF:
        गच्छ बहिर्गमनम्;
      पक्ष NETRC_TOK_MACHINE:
        rc = netrc_lexer_अग्र(&sc.lexer, TRUE);
        यदि(!rc) {
          यदि(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          अन्यथा {
            sc.matches_host = FALSE;
            netrc_lexer_प्रक्षिप(&sc.lexer);
          }
        }
        भङ्ग;
      पक्ष NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        भङ्ग;
      पक्ष NETRC_TOK_ACCOUNT:
        rc = netrc_अवलोकनम्_literal(&sc, रिक्त); /* ignore, not used */
        भङ्ग;
      पक्ष NETRC_TOK_LOGIN:
        rc = netrc_अवलोकनम्_literal(&sc, &sc.login);
        भङ्ग;
      पक्ष NETRC_TOK_PASSWD:
        rc = netrc_अवलोकनम्_literal(&sc, &sc.passwd);
        भङ्ग;
      पक्ष NETRC_TOK_MACDEF:
        netrc_skip_गन्तव्यम्_रिक्तानि_पङ्क्तिः(&sc.lexer);
        भङ्ग;
      पक्ष NETRC_TOK_LITERAL:
      पक्ष NETRC_TOK_JUNK:
      स्वभाव:
        /* skip this */
        भङ्ग;
      }
    }
  }

बहिर्गमनम्:
  यदि(!rc) {
    यदि(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    अन्यथा
      rc = NETRC_NO_मेलनम्;
  }
  netrc_अवलोकनम्_cleanup(&sc);
  प्रत्यागम rc;
}

स्थिर NETRCcode file2memory(ध्रुव वर्ण *filename, संरचना dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  यदि(file) {
    curlx_struct_stat stat;
    यदि((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_बहुलकम्)) {
      CURLcode परिणाम = CURLE_OK;
      bool eof;
      संरचना dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_पङ्क्तिः);
      ret = NETRC_OK;
      कुरु {
        ध्रुव वर्ण *पङ्क्तिः;
        /* Curl_get_line always returns lines ending with a newline */
        परिणाम = Curl_get_पङ्क्तिः(&linebuf, file, &eof);
        यदि(!परिणाम) {
          पङ्क्तिः = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&पङ्क्तिः);
          यदि(*पङ्क्तिः == '#')
            अनुवर्त;
          परिणाम = curlx_dyn_सङ्कलय(filebuf, पङ्क्तिः);
        }
        यदि(परिणाम) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(परिणाम);
          भङ्ग;
        }
      } यावत्(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  प्रत्यागम ret;
}

स्थिर NETRCcode netrc_अवलोकनम्_file(संरचना Curl_easy *दत्त,
                                 संरचना store_netrc *store,
                                 ध्रुव वर्ण *hostname,
                                 ध्रुव वर्ण *user,
                                 ध्रुव वर्ण *netrcfile,
                                 संरचना Curl_creds **pcreds)
{
  संरचना dynbuf *filebuf = &store->filebuf;

  यदि(!store->loaded || वाक्यं_तुलय(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    यदि(ret) {
      CURL_TRC_M(दत्त, "[NETRC] could not load '%s'", netrcfile);
      प्रत्यागम ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    यदि(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      प्रत्यागम NETRC_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
    store->loaded = TRUE;
  }
  यदि(!curlx_dyn_len(filebuf))
    प्रत्यागम NETRC_NO_मेलनम्;

  प्रत्यागम netrc_अवलोकनम्(दत्त, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_अवलोकनम्(संरचना Curl_easy *दत्त,
                          संरचना store_netrc *store,
                          ध्रुव वर्ण *hostname,
                          ध्रुव वर्ण *user,
                          ध्रुव वर्ण *netrcfile,
                          संरचना Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(दत्त, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  यदि(!netrcfile) {
    वर्ण *home = रिक्त;
    वर्ण *homea = रिक्त;
    वर्ण *filealloc = रिक्त;
#यदि defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    वर्ण pwbuf[1024];
#यदि_अन्तः
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
#नोचेद्यदि defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      अन्यथा {
        संरचना passwd *pw;
        pw = getpwuid(geteuid());
        यदि(pw) {
          home = pw->pw_dir;
        }
#नोचेद्यदि defined(_WIN32)
      }
      अन्यथा {
        homea = curl_getenv("USERPROFILE");
        यदि(homea) {
          home = homea;
        }
#यदि_अन्तः
      }

      यदि(!home)
        प्रत्यागम NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      यदि(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ बहिर्गमनम्;
      }
    }
    retcode = netrc_अवलोकनम्_file(
      दत्त, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#यदि_परिभाषितम् _WIN32
    यदि(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      यदि(!filealloc) {
        curlx_free(homea);
        प्रत्यागम NETRC_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
      retcode = netrc_अवलोकनम्_file(
        दत्त, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#यदि_अन्तः
    curlx_free(homea);
  }
  अन्यथा
    retcode = netrc_अवलोकनम्_file(
      दत्त, store, hostname, user, netrcfile, pcreds);

बहिर्गमनम्:
  यदि(retcode)
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
  पक्ष NETRC_NO_मेलनम्:
    प्रत्यागम "no matching entry";
  पक्ष NETRC_बहिर्गमनम्_सम्बन्धि_स्मृतिः:
    प्रत्यागम "out of memory";
  पक्ष NETRC_SYNTAX_ERROR:
    प्रत्यागम "syntax error";
  }
  /* never reached */
}

#यदि_अन्तः /* !CURL_DISABLE_NETRC */
