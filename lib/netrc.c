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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_NETRC

#నిర్వచితమైతే HAVE_PWD_H
#నిర్వచితమైతే __AMIGA__
#నిర్వచనం_రద్దు __NO_NET_API /* required for AmigaOS to declare getpwuid() */
#అయితే_ముగింపు
#అన్తర్భావ <pwd.h>
#నిర్వచితమైతే __AMIGA__
#నిర్వచించు __NO_NET_API
#అయితే_ముగింపు
#అయితే_ముగింపు

#అన్తర్భావ "netrc.h"
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "creds.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "curl_get_line.h"
#అన్తర్భావ "curlx/win32-fopen.h"
#అన్తర్భావ "curlx/strparse.h"


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

#నిర్వచించు MAX_NETRC_పంక్తి  16384
#నిర్వచించు MAX_NETRC_FILE  (128 * 1024)
#నిర్వచించు MAX_NETRC_TOKEN 4096

#నిర్వచించు NETRC_DEBUG   0

/* convert a dynbuf call CURLcode error to a NETRCcode error */
#నిర్వచించు curl2netrc(r)                                  \
  ((!(r)) ? NETRC_OK : (((r) == CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం) ? \
   NETRC_బహిర్గతం_యొక్క_జ్ఞాపకం : NETRC_SYNTAX_ERROR))

రకనిర్వచనం గణన {
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

నిర్మాణం netrc_lexer {
  నిర్మాణం Curl_easy *దత్తాంశం;
  స్థిరరాశి అక్షరం *content;
  స్థిరరాశి అక్షరం *pos;
  నిర్మాణం dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#ఒకవేళ NETRC_DEBUG
స్థిర స్థిరరాశి అక్షరం *netrc_tokenstr(curl_netrc_token token)
{
  ఎంపిక(token) {
  స్థితి NETRC_TOK_EOF:
    తిరిగి "[EOF]";
  స్థితి NETRC_TOK_LITERAL:
    తిరిగి "[LITERAL]";
  స్థితి NETRC_TOK_MACHINE:
    తిరిగి "[MACHINE]";
  స్థితి NETRC_TOK_DEFAULT:
    తిరిగి "[DEFAULT]";
  స్థితి NETRC_TOK_ACCOUNT:
    తిరిగి "[ACCOUNT]";
  స్థితి NETRC_TOK_LOGIN:
    తిరిగి "[LOGIN]";
  స్థితి NETRC_TOK_PASSWD:
    తిరిగి "[PASSWORD]";
  స్థితి NETRC_TOK_MACDEF:
    తిరిగి "[MACDEF]";
  స్థితి NETRC_TOK_JUNK:
    తిరిగి "[JUNK]";
  అప్రమేయ:
    తిరిగి "[???]";
  }
}
#అయితే_ముగింపు

స్థిర శూన్య netrc_lexer_init(నిర్మాణం netrc_lexer *lexer,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             స్థిరరాశి అక్షరం *content)
{
  curlx_dyn_init(&lexer->literal, MAX_NETRC_TOKEN);
  lexer->దత్తాంశం = దత్తాంశం;
  lexer->content = lexer->pos = content;
}

స్థిర శూన్య netrc_lexer_cleanup(నిర్మాణం netrc_lexer *lexer)
{
  lexer->content = lexer->pos = ఖాళీ;
  lexer->దత్తాంశం = ఖాళీ;
  curlx_dyn_free(&lexer->literal);
}

స్థిర శూన్య netrc_skip_blanks(నిర్మాణం netrc_lexer *lexer)
{
  స్థిరరాశి అక్షరం *s = lexer->pos;
  అయితే(*s) {
    curlx_str_passblanks(&s);
    అయితే(*s == '\r')
      ++s;
    ఒకవేళ(*s == '\n') {
      ++s;
    }
    లేకపోతే
      ఆపు;
  }
  lexer->pos = s;
}

స్థిర శూన్య netrc_skip_గమ్యం_ఖాళీలు_పంక్తి(నిర్మాణం netrc_lexer *lexer)
{
  స్థిరరాశి అక్షరం *s = lexer->pos;
  అయితే(*s) {
    ఒకవేళ(*s == '\r')
      ++s;
    లేకపోతే ఒకవేళ(*s == '\n') {
      ++s;
      అయితే(*s == '\r')
        ++s;
      ఒకవేళ(*s == '\n')
        వెళ్ళు బహిర్గతం;
    }
    లేకపోతే
      ++s;
  }
బహిర్గతం:
  lexer->pos = s;
}

/*
 * Parse a quoted token starting after the opening '"'. Handles \n, \r, \t
 * escape sequences. Advances *tok_endp past the closing '"'.
 *
 * Returns NETRC_OK or error.
 */
స్థిర NETRCcode netrc_lexer_quoted(నిర్మాణం netrc_lexer *lexer)
{
  NETRCcode rc = NETRC_SYNTAX_ERROR;
  స్థిరరాశి అక్షరం *s = lexer->pos;
  bool escape = FALSE;
  CURLcode ఫలితం;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  అయితే(*s) {
    అక్షరం c = *s;
    ఒకవేళ(escape) {
      escape = FALSE;
      ఎంపిక(c) {
      స్థితి 'n':
        c = '\n';
        ఆపు;
      స్థితి 'r':
        c = '\r';
        ఆపు;
      స్థితి 't':
        c = '\t';
        ఆపు;
      }
    }
    లేకపోతే ఒకవేళ(c == '\\') {
      escape = TRUE;
      ++s;
      కొనసాగు;
    }
    లేకపోతే ఒకవేళ(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      ఫలితం = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(ఫలితం);
      వెళ్ళు బహిర్గతం;
    }
    ఫలితం = curlx_dyn_addn(&lexer->literal, &c, 1);
    ఒకవేళ(ఫలితం) {
      rc = curl2netrc(ఫలితం);
      వెళ్ళు బహిర్గతం;
    }
    ++s;
  }
బహిర్గతం:
  lexer->pos = s;
  తిరిగి rc;
}

స్థిర శూన్య netrc_lexer_నెట్టు(నిర్మాణం netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

స్థిర NETRCcode netrc_lexer_తదుపరి(నిర్మాణం netrc_lexer *lexer,
                                  bool want_literal)
{
  స్థిరరాశి అక్షరం *s = lexer->pos, *ప్రారంభం;
  NETRCcode rc = NETRC_OK;
  size_t slen;
  CURLcode ఫలితం;

  ఒకవేళ(lexer->pushed) {
    lexer->pushed = FALSE;
    వెళ్ళు బహిర్గతం;
  }

  curlx_dyn_reset(&lexer->literal);
  netrc_skip_blanks(lexer);
  s = lexer->pos;

  ఎంపిక(*s) {
  స్థితి 0:
    lexer->token = NETRC_TOK_EOF;
    ఆపు;
  స్థితి '\"':
    rc = netrc_lexer_quoted(lexer);
    lexer->token = NETRC_TOK_LITERAL;
    s = lexer->pos;
    ఆపు;
  అప్రమేయ:
    /* unquoted token */
    ప్రారంభం = s;
    అయితే(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - ప్రారంభం;
    ఒకవేళ(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    ఒకవేళ(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      ఫలితం = curlx_dyn_addn(&lexer->literal, ప్రారంభం, slen);
      rc = curl2netrc(ఫలితం);
    }
    లేకపోతే ఒకవేళ((slen == 7) && curl_strnequal(ప్రారంభం, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    లేకపోతే ఒకవేళ((slen == 7) && curl_strnequal(ప్రారంభం, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    లేకపోతే ఒకవేళ((slen == 7) && curl_strnequal(ప్రారంభం, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    లేకపోతే ఒకవేళ((slen == 5) && curl_strnequal(ప్రారంభం, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    లేకపోతే ఒకవేళ((slen == 8) && curl_strnequal(ప్రారంభం, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    లేకపోతే ఒకవేళ((slen == 6) && curl_strnequal(ప్రారంభం, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    లేకపోతే {
      lexer->token = NETRC_TOK_JUNK;
    }
    ఆపు;
  }

బహిర్గతం:
#ఒకవేళ NETRC_DEBUG
  CURL_TRC_M(lexer->దత్తాంశం, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#అయితే_ముగింపు
  lexer->pos = s;
  తిరిగి rc;
}

నిర్మాణం netrc_scanner {
  నిర్మాణం netrc_lexer lexer;
  స్థిరరాశి అక్షరం *hostname; /* non-NULL, machine to scan for */
  స్థిరరాశి అక్షరం *user; /* maybe NULL, login to scan for */
  అక్షరం *login;
  అక్షరం *passwd;
  నిర్మాణం Curl_creds *creds;
  bool matches_host;
  bool దొరికింది;
};

స్థిర శూన్య netrc_పరిశీలన_reset(నిర్మాణం netrc_scanner *sc)
{
  curlx_safefree(sc->login);
  curlx_safefree(sc->passwd);
  sc->matches_host = FALSE;
}

స్థిర శూన్య netrc_పరిశీలన_init(నిర్మాణం netrc_scanner *sc,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            స్థిరరాశి అక్షరం *content,
                            స్థిరరాశి అక్షరం *hostname,
                            స్థిరరాశి అక్షరం *user)
{
  memset(sc, 0, పరిమాణం(*sc));
  netrc_lexer_init(&sc->lexer, దత్తాంశం, content);
  sc->hostname = hostname;
  sc->user = (user && user[0]) ? user : ఖాళీ;
  netrc_పరిశీలన_reset(sc);
}

స్థిర శూన్య netrc_పరిశీలన_cleanup(నిర్మాణం netrc_scanner *sc)
{
  netrc_పరిశీలన_reset(sc);
  sc->hostname = ఖాళీ;
  sc->user = ఖాళీ;
  Curl_creds_unlink(&sc->creds);
  netrc_lexer_cleanup(&sc->lexer);
}

స్థిర NETRCcode netrc_పరిశీలన_literal(నిర్మాణం netrc_scanner *sc,
                                    అక్షరం **pdest)
{
  NETRCcode rc = netrc_lexer_తదుపరి(&sc->lexer, TRUE);
  ఒకవేళ(!rc) {
    ఒకవేళ(sc->lexer.token == NETRC_TOK_LITERAL) {
      ఒకవేళ(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        ఒకవేళ(!*pdest)
          rc = NETRC_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
    }
    లేకపోతే
      netrc_lexer_నెట్టు(&sc->lexer);
  }
  తిరిగి rc;
}

స్థిర NETRCcode netrc_పరిశీలన_చివర_నమోదు(నిర్మాణం netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#ఒకవేళ NETRC_DEBUG
  CURL_TRC_M(sc->lexer.దత్తాంశం,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#అయితే_ముగింపు
  ఒకవేళ(sc->matches_host) {
    ఒకవేళ(sc->login) {
      ఒకవేళ(sc->user) {
        ఒకవేళ(Curl_timestrcmp(sc->user, sc->login))
          వెళ్ళు బహిర్గతం;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->దొరికింది = !!sc->passwd;
      }
      లేకపోతే {
        sc->దొరికింది = TRUE;
      }
    }
    లేకపోతే ఒకవేళ(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->దొరికింది = TRUE;
    }
    లేకపోతే {
      /* entry has nothing interesting */
    }
    ఒకవేళ(sc->దొరికింది) {
#ఒకవేళ NETRC_DEBUG
      CURL_TRC_M(sc->lexer.దత్తాంశం, "[NETRC] entry match found");
#అయితే_ముగింపు
      ఒకవేళ(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           ఖాళీ, ఖాళీ, ఖాళీ, CREDS_NETRC, &sc->creds))
        rc = NETRC_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }
బహిర్గతం:
  netrc_పరిశీలన_reset(sc);
  తిరిగి rc;
}

స్థిర NETRCcode netrc_పరిశీలన(నిర్మాణం Curl_easy *దత్తాంశం,
                            స్థిరరాశి అక్షరం *content,
                            స్థిరరాశి అక్షరం *hostname,
                            స్థిరరాశి అక్షరం *user,
                            నిర్మాణం Curl_creds **pcreds)
{
  నిర్మాణం netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_పరిశీలన_init(&sc, దత్తాంశం, content, hostname, user);

  అయితే(!rc && !sc.దొరికింది) {
    rc = netrc_lexer_తదుపరి(&sc.lexer, FALSE);
    ఒకవేళ(!rc) {
      /* Does this token end any previous entry? */
      ఎంపిక(sc.lexer.token) {
      స్థితి NETRC_TOK_EOF:
      స్థితి NETRC_TOK_MACHINE:
      స్థితి NETRC_TOK_DEFAULT:
      స్థితి NETRC_TOK_MACDEF:
        rc = netrc_పరిశీలన_చివర_నమోదు(&sc);
        ఒకవేళ(rc || sc.దొరికింది)
          వెళ్ళు బహిర్గతం;
        ఆపు;
      అప్రమేయ:
        ఆపు;
      }

      ఎంపిక(sc.lexer.token) {
      స్థితి NETRC_TOK_EOF:
        వెళ్ళు బహిర్గతం;
      స్థితి NETRC_TOK_MACHINE:
        rc = netrc_lexer_తదుపరి(&sc.lexer, TRUE);
        ఒకవేళ(!rc) {
          ఒకవేళ(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          లేకపోతే {
            sc.matches_host = FALSE;
            netrc_lexer_నెట్టు(&sc.lexer);
          }
        }
        ఆపు;
      స్థితి NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        ఆపు;
      స్థితి NETRC_TOK_ACCOUNT:
        rc = netrc_పరిశీలన_literal(&sc, ఖాళీ); /* ignore, not used */
        ఆపు;
      స్థితి NETRC_TOK_LOGIN:
        rc = netrc_పరిశీలన_literal(&sc, &sc.login);
        ఆపు;
      స్థితి NETRC_TOK_PASSWD:
        rc = netrc_పరిశీలన_literal(&sc, &sc.passwd);
        ఆపు;
      స్థితి NETRC_TOK_MACDEF:
        netrc_skip_గమ్యం_ఖాళీలు_పంక్తి(&sc.lexer);
        ఆపు;
      స్థితి NETRC_TOK_LITERAL:
      స్థితి NETRC_TOK_JUNK:
      అప్రమేయ:
        /* skip this */
        ఆపు;
      }
    }
  }

బహిర్గతం:
  ఒకవేళ(!rc) {
    ఒకవేళ(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    లేకపోతే
      rc = NETRC_NO_సరిపోలిక;
  }
  netrc_పరిశీలన_cleanup(&sc);
  తిరిగి rc;
}

స్థిర NETRCcode file2memory(స్థిరరాశి అక్షరం *filename, నిర్మాణం dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  ఒకవేళ(file) {
    curlx_struct_stat stat;
    ఒకవేళ((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_బహుళకం)) {
      CURLcode ఫలితం = CURLE_OK;
      bool eof;
      నిర్మాణం dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_పంక్తి);
      ret = NETRC_OK;
      చేయి {
        స్థిరరాశి అక్షరం *పంక్తి;
        /* Curl_get_line always returns lines ending with a newline */
        ఫలితం = Curl_get_పంక్తి(&linebuf, file, &eof);
        ఒకవేళ(!ఫలితం) {
          పంక్తి = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&పంక్తి);
          ఒకవేళ(*పంక్తి == '#')
            కొనసాగు;
          ఫలితం = curlx_dyn_కూడు(filebuf, పంక్తి);
        }
        ఒకవేళ(ఫలితం) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(ఫలితం);
          ఆపు;
        }
      } అయితే(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  తిరిగి ret;
}

స్థిర NETRCcode netrc_పరిశీలన_file(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం store_netrc *store,
                                 స్థిరరాశి అక్షరం *hostname,
                                 స్థిరరాశి అక్షరం *user,
                                 స్థిరరాశి అక్షరం *netrcfile,
                                 నిర్మాణం Curl_creds **pcreds)
{
  నిర్మాణం dynbuf *filebuf = &store->filebuf;

  ఒకవేళ(!store->loaded || strcmp(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    ఒకవేళ(ret) {
      CURL_TRC_M(దత్తాంశం, "[NETRC] could not load '%s'", netrcfile);
      తిరిగి ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    ఒకవేళ(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      తిరిగి NETRC_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    store->loaded = TRUE;
  }
  ఒకవేళ(!curlx_dyn_len(filebuf))
    తిరిగి NETRC_NO_సరిపోలిక;

  తిరిగి netrc_పరిశీలన(దత్తాంశం, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_పరిశీలన(నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం store_netrc *store,
                          స్థిరరాశి అక్షరం *hostname,
                          స్థిరరాశి అక్షరం *user,
                          స్థిరరాశి అక్షరం *netrcfile,
                          నిర్మాణం Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(దత్తాంశం, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  ఒకవేళ(!netrcfile) {
    అక్షరం *home = ఖాళీ;
    అక్షరం *homea = ఖాళీ;
    అక్షరం *filealloc = ఖాళీ;
#ఒకవేళ defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    అక్షరం pwbuf[1024];
#అయితే_ముగింపు
    filealloc = curl_getenv("NETRC");
    ఒకవేళ(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      ఒకవేళ(homea) {
        home = homea;
#ఒకవేళ defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      లేకపోతే {
        నిర్మాణం passwd pw, *pw_res;
        ఒకవేళ(!getpwuid_r(geteuid(), &pw, pwbuf, పరిమాణం(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#లేదావేళ defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      లేకపోతే {
        నిర్మాణం passwd *pw;
        pw = getpwuid(geteuid());
        ఒకవేళ(pw) {
          home = pw->pw_dir;
        }
#లేదావేళ defined(_WIN32)
      }
      లేకపోతే {
        homea = curl_getenv("USERPROFILE");
        ఒకవేళ(homea) {
          home = homea;
        }
#అయితే_ముగింపు
      }

      ఒకవేళ(!home)
        తిరిగి NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      ఒకవేళ(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు బహిర్గతం;
      }
    }
    retcode = netrc_పరిశీలన_file(
      దత్తాంశం, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#నిర్వచితమైతే _WIN32
    ఒకవేళ(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      ఒకవేళ(!filealloc) {
        curlx_free(homea);
        తిరిగి NETRC_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
      retcode = netrc_పరిశీలన_file(
        దత్తాంశం, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#అయితే_ముగింపు
    curlx_free(homea);
  }
  లేకపోతే
    retcode = netrc_పరిశీలన_file(
      దత్తాంశం, store, hostname, user, netrcfile, pcreds);

బహిర్గతం:
  ఒకవేళ(retcode)
    Curl_creds_unlink(pcreds);
  తిరిగి retcode;
}

శూన్య Curl_netrc_init(నిర్మాణం store_netrc *store)
{
  curlx_dyn_init(&store->filebuf, MAX_NETRC_FILE);
  store->loaded = FALSE;
  store->filename = ఖాళీ;
}
శూన్య Curl_netrc_cleanup(నిర్మాణం store_netrc *store)
{
  curlx_dyn_free(&store->filebuf);
  curlx_safefree(store->filename);
  store->loaded = FALSE;
}

స్థిరరాశి అక్షరం *Curl_netrc_strerror(NETRCcode ret)
{
  ఎంపిక(ret) {
  అప్రమేయ:
    తిరిగి ""; /* not a legit error */
  స్థితి NETRC_FILE_MISSING:
    తిరిగి "no such file";
  స్థితి NETRC_NO_సరిపోలిక:
    తిరిగి "no matching entry";
  స్థితి NETRC_బహిర్గతం_యొక్క_జ్ఞాపకం:
    తిరిగి "out of memory";
  స్థితి NETRC_SYNTAX_ERROR:
    తిరిగి "syntax error";
  }
  /* never reached */
}

#అయితే_ముగింపు /* !CURL_DISABLE_NETRC */
