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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifndef CURL_DISABLE_NETRC

#ifdef HAVE_PWD_H
#ifdef __AMIGA__
#undef __NO_NET_API /* required for AmigaOS to declare getpwuid() */
#endif
#ଅନ୍ତର୍ଭାଵ <pwd.h>
#ifdef __AMIGA__
#define __NO_NET_API
#endif
#endif

#ଅନ୍ତର୍ଭାଵ "netrc.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "creds.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "curl_get_line.h"
#ଅନ୍ତର୍ଭାଵ "curlx/win32-fopen.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"


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

#define MAX_NETRC_ଧାଡ଼ି  16384
#define MAX_NETRC_FILE  (128 * 1024)
#define MAX_NETRC_TOKEN 4096

#define NETRC_DEBUG   0

/* convert a dynbuf call CURLcode error to a NETRCcode error */
#define curl2netrc(r)                                  \
  ((!(r)) ? NETRC_OK : (((r) == CURLE_OUT_OF_MEMORY) ? \
   NETRC_OUT_OF_MEMORY : NETRC_SYNTAX_ERROR))

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
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

ସଂରଚନା netrc_lexer {
  ସଂରଚନା Curl_easy *data;
  ଅଚଳ ଅକ୍ଷର *content;
  ଅଚଳ ଅକ୍ଷର *pos;
  ସଂରଚନା dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#ଯଦି NETRC_DEBUG
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *netrc_tokenstr(curl_netrc_token token)
{
  ବାଛ(token) {
  ସ୍ଥିତି NETRC_TOK_EOF:
    ଫେରନ୍ତୁ "[EOF]";
  ସ୍ଥିତି NETRC_TOK_LITERAL:
    ଫେରନ୍ତୁ "[LITERAL]";
  ସ୍ଥିତି NETRC_TOK_MACHINE:
    ଫେରନ୍ତୁ "[MACHINE]";
  ସ୍ଥିତି NETRC_TOK_DEFAULT:
    ଫେରନ୍ତୁ "[DEFAULT]";
  ସ୍ଥିତି NETRC_TOK_ACCOUNT:
    ଫେରନ୍ତୁ "[ACCOUNT]";
  ସ୍ଥିତି NETRC_TOK_LOGIN:
    ଫେରନ୍ତୁ "[LOGIN]";
  ସ୍ଥିତି NETRC_TOK_PASSWD:
    ଫେରନ୍ତୁ "[PASSWORD]";
  ସ୍ଥିତି NETRC_TOK_MACDEF:
    ଫେରନ୍ତୁ "[MACDEF]";
  ସ୍ଥିତି NETRC_TOK_JUNK:
    ଫେରନ୍ତୁ "[JUNK]";
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ "[???]";
  }
}
#endif

ସ୍ଥିର ଶୂନ୍ୟ netrc_lexer_init(ସଂରଚନା netrc_lexer *lexer,
                             ସଂରଚନା Curl_easy *data,
                             ଅଚଳ ଅକ୍ଷର *content)
{
  curlx_dyn_init(&lexer->literal, MAX_NETRC_TOKEN);
  lexer->data = data;
  lexer->content = lexer->pos = content;
}

ସ୍ଥିର ଶୂନ୍ୟ netrc_lexer_cleanup(ସଂରଚନା netrc_lexer *lexer)
{
  lexer->content = lexer->pos = ଖାଲି;
  lexer->data = ଖାଲି;
  curlx_dyn_free(&lexer->literal);
}

ସ୍ଥିର ଶୂନ୍ୟ netrc_skip_blanks(ସଂରଚନା netrc_lexer *lexer)
{
  ଅଚଳ ଅକ୍ଷର *s = lexer->pos;
  ଯେତେବେଳେ(*s) {
    curlx_str_passblanks(&s);
    ଯେତେବେଳେ(*s == '\r')
      ++s;
    ଯଦି(*s == '\n') {
      ++s;
    }
    ଅନ୍ୟଥା
      ଭାଙ୍ଗ;
  }
  lexer->pos = s;
}

ସ୍ଥିର ଶୂନ୍ୟ netrc_skip_to_empty_ଧାଡ଼ି(ସଂରଚନା netrc_lexer *lexer)
{
  ଅଚଳ ଅକ୍ଷର *s = lexer->pos;
  ଯେତେବେଳେ(*s) {
    ଯଦି(*s == '\r')
      ++s;
    ଅନ୍ୟଥା ଯଦି(*s == '\n') {
      ++s;
      ଯେତେବେଳେ(*s == '\r')
        ++s;
      ଯଦି(*s == '\n')
        ଯାଅ out;
    }
    ଅନ୍ୟଥା
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
ସ୍ଥିର NETRCcode netrc_lexer_quoted(ସଂରଚନା netrc_lexer *lexer)
{
  NETRCcode rc = NETRC_SYNTAX_ERROR;
  ଅଚଳ ଅକ୍ଷର *s = lexer->pos;
  bool escape = FALSE;
  CURLcode result;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  ଯେତେବେଳେ(*s) {
    ଅକ୍ଷର c = *s;
    ଯଦି(escape) {
      escape = FALSE;
      ବାଛ(c) {
      ସ୍ଥିତି 'n':
        c = '\n';
        ଭାଙ୍ଗ;
      ସ୍ଥିତି 'r':
        c = '\r';
        ଭାଙ୍ଗ;
      ସ୍ଥିତି 't':
        c = '\t';
        ଭାଙ୍ଗ;
      }
    }
    ଅନ୍ୟଥା ଯଦି(c == '\\') {
      escape = TRUE;
      ++s;
      ଜାରି;
    }
    ଅନ୍ୟଥା ଯଦି(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      result = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(result);
      ଯାଅ out;
    }
    result = curlx_dyn_addn(&lexer->literal, &c, 1);
    ଯଦି(result) {
      rc = curl2netrc(result);
      ଯାଅ out;
    }
    ++s;
  }
out:
  lexer->pos = s;
  ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ଶୂନ୍ୟ netrc_lexer_ଠେଲ(ସଂରଚନା netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

ସ୍ଥିର NETRCcode netrc_lexer_next(ସଂରଚନା netrc_lexer *lexer,
                                  bool want_literal)
{
  ଅଚଳ ଅକ୍ଷର *s = lexer->pos, *start;
  NETRCcode rc = NETRC_OK;
  size_t slen;
  CURLcode result;

  ଯଦି(lexer->pushed) {
    lexer->pushed = FALSE;
    ଯାଅ out;
  }

  curlx_dyn_reset(&lexer->literal);
  netrc_skip_blanks(lexer);
  s = lexer->pos;

  ବାଛ(*s) {
  ସ୍ଥିତି 0:
    lexer->token = NETRC_TOK_EOF;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି '\"':
    rc = netrc_lexer_quoted(lexer);
    lexer->token = NETRC_TOK_LITERAL;
    s = lexer->pos;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    /* unquoted token */
    start = s;
    ଯେତେବେଳେ(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - start;
    ଯଦି(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    ଯଦି(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      result = curlx_dyn_addn(&lexer->literal, start, slen);
      rc = curl2netrc(result);
    }
    ଅନ୍ୟଥା ଯଦି((slen == 7) && curl_strnequal(start, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    ଅନ୍ୟଥା ଯଦି((slen == 7) && curl_strnequal(start, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    ଅନ୍ୟଥା ଯଦି((slen == 7) && curl_strnequal(start, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    ଅନ୍ୟଥା ଯଦି((slen == 5) && curl_strnequal(start, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    ଅନ୍ୟଥା ଯଦି((slen == 8) && curl_strnequal(start, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    ଅନ୍ୟଥା ଯଦି((slen == 6) && curl_strnequal(start, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    ଅନ୍ୟଥା {
      lexer->token = NETRC_TOK_JUNK;
    }
    ଭାଙ୍ଗ;
  }

out:
#ଯଦି NETRC_DEBUG
  CURL_TRC_M(lexer->data, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#endif
  lexer->pos = s;
  ଫେରନ୍ତୁ rc;
}

ସଂରଚନା netrc_scanner {
  ସଂରଚନା netrc_lexer lexer;
  ଅଚଳ ଅକ୍ଷର *hostname; /* non-NULL, machine to scan for */
  ଅଚଳ ଅକ୍ଷର *user; /* maybe NULL, login to scan for */
  ଅକ୍ଷର *login;
  ଅକ୍ଷର *passwd;
  ସଂରଚନା Curl_creds *creds;
  bool matches_host;
  bool found;
};

ସ୍ଥିର ଶୂନ୍ୟ netrc_scan_reset(ସଂରଚନା netrc_scanner *sc)
{
  curlx_safefree(sc->login);
  curlx_safefree(sc->passwd);
  sc->matches_host = FALSE;
}

ସ୍ଥିର ଶୂନ୍ୟ netrc_scan_init(ସଂରଚନା netrc_scanner *sc,
                            ସଂରଚନା Curl_easy *data,
                            ଅଚଳ ଅକ୍ଷର *content,
                            ଅଚଳ ଅକ୍ଷର *hostname,
                            ଅଚଳ ଅକ୍ଷର *user)
{
  memset(sc, 0, ଆକାର(*sc));
  netrc_lexer_init(&sc->lexer, data, content);
  sc->hostname = hostname;
  sc->user = (user && user[0]) ? user : ଖାଲି;
  netrc_scan_reset(sc);
}

ସ୍ଥିର ଶୂନ୍ୟ netrc_scan_cleanup(ସଂରଚନା netrc_scanner *sc)
{
  netrc_scan_reset(sc);
  sc->hostname = ଖାଲି;
  sc->user = ଖାଲି;
  Curl_creds_unlink(&sc->creds);
  netrc_lexer_cleanup(&sc->lexer);
}

ସ୍ଥିର NETRCcode netrc_scan_literal(ସଂରଚନା netrc_scanner *sc,
                                    ଅକ୍ଷର **pdest)
{
  NETRCcode rc = netrc_lexer_next(&sc->lexer, TRUE);
  ଯଦି(!rc) {
    ଯଦି(sc->lexer.token == NETRC_TOK_LITERAL) {
      ଯଦି(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        ଯଦି(!*pdest)
          rc = NETRC_OUT_OF_MEMORY;
      }
    }
    ଅନ୍ୟଥା
      netrc_lexer_ଠେଲ(&sc->lexer);
  }
  ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର NETRCcode netrc_scan_end_entry(ସଂରଚନା netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#ଯଦି NETRC_DEBUG
  CURL_TRC_M(sc->lexer.data,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#endif
  ଯଦି(sc->matches_host) {
    ଯଦି(sc->login) {
      ଯଦି(sc->user) {
        ଯଦି(Curl_timestrcmp(sc->user, sc->login))
          ଯାଅ out;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->found = !!sc->passwd;
      }
      ଅନ୍ୟଥା {
        sc->found = TRUE;
      }
    }
    ଅନ୍ୟଥା ଯଦି(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->found = TRUE;
    }
    ଅନ୍ୟଥା {
      /* entry has nothing interesting */
    }
    ଯଦି(sc->found) {
#ଯଦି NETRC_DEBUG
      CURL_TRC_M(sc->lexer.data, "[NETRC] entry match found");
#endif
      ଯଦି(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           ଖାଲି, ଖାଲି, ଖାଲି, CREDS_NETRC, &sc->creds))
        rc = NETRC_OUT_OF_MEMORY;
    }
  }
out:
  netrc_scan_reset(sc);
  ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର NETRCcode netrc_scan(ସଂରଚନା Curl_easy *data,
                            ଅଚଳ ଅକ୍ଷର *content,
                            ଅଚଳ ଅକ୍ଷର *hostname,
                            ଅଚଳ ଅକ୍ଷର *user,
                            ସଂରଚନା Curl_creds **pcreds)
{
  ସଂରଚନା netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_scan_init(&sc, data, content, hostname, user);

  ଯେତେବେଳେ(!rc && !sc.found) {
    rc = netrc_lexer_next(&sc.lexer, FALSE);
    ଯଦି(!rc) {
      /* Does this token end any previous entry? */
      ବାଛ(sc.lexer.token) {
      ସ୍ଥିତି NETRC_TOK_EOF:
      ସ୍ଥିତି NETRC_TOK_MACHINE:
      ସ୍ଥିତି NETRC_TOK_DEFAULT:
      ସ୍ଥିତି NETRC_TOK_MACDEF:
        rc = netrc_scan_end_entry(&sc);
        ଯଦି(rc || sc.found)
          ଯାଅ out;
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଭାଙ୍ଗ;
      }

      ବାଛ(sc.lexer.token) {
      ସ୍ଥିତି NETRC_TOK_EOF:
        ଯାଅ out;
      ସ୍ଥିତି NETRC_TOK_MACHINE:
        rc = netrc_lexer_next(&sc.lexer, TRUE);
        ଯଦି(!rc) {
          ଯଦି(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          ଅନ୍ୟଥା {
            sc.matches_host = FALSE;
            netrc_lexer_ଠେଲ(&sc.lexer);
          }
        }
        ଭାଙ୍ଗ;
      ସ୍ଥିତି NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି NETRC_TOK_ACCOUNT:
        rc = netrc_scan_literal(&sc, ଖାଲି); /* ignore, not used */
        ଭାଙ୍ଗ;
      ସ୍ଥିତି NETRC_TOK_LOGIN:
        rc = netrc_scan_literal(&sc, &sc.login);
        ଭାଙ୍ଗ;
      ସ୍ଥିତି NETRC_TOK_PASSWD:
        rc = netrc_scan_literal(&sc, &sc.passwd);
        ଭାଙ୍ଗ;
      ସ୍ଥିତି NETRC_TOK_MACDEF:
        netrc_skip_to_empty_ଧାଡ଼ି(&sc.lexer);
        ଭାଙ୍ଗ;
      ସ୍ଥିତି NETRC_TOK_LITERAL:
      ସ୍ଥିତି NETRC_TOK_JUNK:
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        /* skip this */
        ଭାଙ୍ଗ;
      }
    }
  }

out:
  ଯଦି(!rc) {
    ଯଦି(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    ଅନ୍ୟଥା
      rc = NETRC_NO_MATCH;
  }
  netrc_scan_cleanup(&sc);
  ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର NETRCcode file2memory(ଅଚଳ ଅକ୍ଷର *filename, ସଂରଚନା dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  ଯଦି(file) {
    curlx_struct_stat stat;
    ଯଦି((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      CURLcode result = CURLE_OK;
      bool eof;
      ସଂରଚନା dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_ଧାଡ଼ି);
      ret = NETRC_OK;
      କର {
        ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି;
        /* Curl_get_line always returns lines ending with a newline */
        result = Curl_get_ଧାଡ଼ି(&linebuf, file, &eof);
        ଯଦି(!result) {
          ଧାଡ଼ି = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&ଧାଡ଼ି);
          ଯଦି(*ଧାଡ଼ି == '#')
            ଜାରି;
          result = curlx_dyn_add(filebuf, ଧାଡ଼ି);
        }
        ଯଦି(result) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(result);
          ଭାଙ୍ଗ;
        }
      } ଯେତେବେଳେ(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର NETRCcode netrc_scan_file(ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା store_netrc *store,
                                 ଅଚଳ ଅକ୍ଷର *hostname,
                                 ଅଚଳ ଅକ୍ଷର *user,
                                 ଅଚଳ ଅକ୍ଷର *netrcfile,
                                 ସଂରଚନା Curl_creds **pcreds)
{
  ସଂରଚନା dynbuf *filebuf = &store->filebuf;

  ଯଦି(!store->loaded || strcmp(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    ଯଦି(ret) {
      CURL_TRC_M(data, "[NETRC] could not load '%s'", netrcfile);
      ଫେରନ୍ତୁ ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    ଯଦି(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      ଫେରନ୍ତୁ NETRC_OUT_OF_MEMORY;
    }
    store->loaded = TRUE;
  }
  ଯଦି(!curlx_dyn_len(filebuf))
    ଫେରନ୍ତୁ NETRC_NO_MATCH;

  ଫେରନ୍ତୁ netrc_scan(data, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_scan(ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା store_netrc *store,
                          ଅଚଳ ଅକ୍ଷର *hostname,
                          ଅଚଳ ଅକ୍ଷର *user,
                          ଅଚଳ ଅକ୍ଷର *netrcfile,
                          ସଂରଚନା Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(data, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  ଯଦି(!netrcfile) {
    ଅକ୍ଷର *home = ଖାଲି;
    ଅକ୍ଷର *homea = ଖାଲି;
    ଅକ୍ଷର *filealloc = ଖାଲି;
#ଯଦି defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    ଅକ୍ଷର pwbuf[1024];
#endif
    filealloc = curl_getenv("NETRC");
    ଯଦି(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      ଯଦି(homea) {
        home = homea;
#ଯଦି defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      ଅନ୍ୟଥା {
        ସଂରଚନା passwd pw, *pw_res;
        ଯଦି(!getpwuid_r(geteuid(), &pw, pwbuf, ଆକାର(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#ନଚେତଯଦି defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      ଅନ୍ୟଥା {
        ସଂରଚନା passwd *pw;
        pw = getpwuid(geteuid());
        ଯଦି(pw) {
          home = pw->pw_dir;
        }
#ନଚେତଯଦି defined(_WIN32)
      }
      ଅନ୍ୟଥା {
        homea = curl_getenv("USERPROFILE");
        ଯଦି(homea) {
          home = homea;
        }
#endif
      }

      ଯଦି(!home)
        ଫେରନ୍ତୁ NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      ଯଦି(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_OUT_OF_MEMORY;
        ଯାଅ out;
      }
    }
    retcode = netrc_scan_file(
      data, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#ifdef _WIN32
    ଯଦି(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      ଯଦି(!filealloc) {
        curlx_free(homea);
        ଫେରନ୍ତୁ NETRC_OUT_OF_MEMORY;
      }
      retcode = netrc_scan_file(
        data, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#endif
    curlx_free(homea);
  }
  ଅନ୍ୟଥା
    retcode = netrc_scan_file(
      data, store, hostname, user, netrcfile, pcreds);

out:
  ଯଦି(retcode)
    Curl_creds_unlink(pcreds);
  ଫେରନ୍ତୁ retcode;
}

ଶୂନ୍ୟ Curl_netrc_init(ସଂରଚନା store_netrc *store)
{
  curlx_dyn_init(&store->filebuf, MAX_NETRC_FILE);
  store->loaded = FALSE;
  store->filename = ଖାଲି;
}
ଶୂନ୍ୟ Curl_netrc_cleanup(ସଂରଚନା store_netrc *store)
{
  curlx_dyn_free(&store->filebuf);
  curlx_safefree(store->filename);
  store->loaded = FALSE;
}

ଅଚଳ ଅକ୍ଷର *Curl_netrc_strerror(NETRCcode ret)
{
  ବାଛ(ret) {
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ ""; /* not a legit error */
  ସ୍ଥିତି NETRC_FILE_MISSING:
    ଫେରନ୍ତୁ "no such file";
  ସ୍ଥିତି NETRC_NO_MATCH:
    ଫେରନ୍ତୁ "no matching entry";
  ସ୍ଥିତି NETRC_OUT_OF_MEMORY:
    ଫେରନ୍ତୁ "out of memory";
  ସ୍ଥିତି NETRC_SYNTAX_ERROR:
    ଫେରନ୍ତୁ "syntax error";
  }
  /* never reached */
}

#endif /* !CURL_DISABLE_NETRC */
