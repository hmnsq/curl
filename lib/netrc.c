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
#અન્તર્ભાવ "curl_setup.h"

#ifndef CURL_DISABLE_NETRC

#ifdef HAVE_PWD_H
#ifdef __AMIGA__
#undef __NO_NET_API /* required for AmigaOS to declare getpwuid() */
#endif
#અન્તર્ભાવ <pwd.h>
#ifdef __AMIGA__
#define __NO_NET_API
#endif
#endif

#અન્તર્ભાવ "netrc.h"
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "creds.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "strcase.h"
#અન્તર્ભાવ "curl_get_line.h"
#અન્તર્ભાવ "curlx/win32-fopen.h"
#અન્તર્ભાવ "curlx/strparse.h"


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

#define MAX_NETRC_લીટી  16384
#define MAX_NETRC_FILE  (128 * 1024)
#define MAX_NETRC_TOKEN 4096

#define NETRC_DEBUG   0

/* convert a dynbuf call CURLcode error to a NETRCcode error */
#define curl2netrc(r)                                  \
  ((!(r)) ? NETRC_OK : (((r) == CURLE_OUT_OF_MEMORY) ? \
   NETRC_OUT_OF_MEMORY : NETRC_SYNTAX_ERROR))

પ્રકારવ્યાખ્યા ગણના {
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

સંરચના netrc_lexer {
  સંરચના Curl_easy *data;
  અચલ અક્ષર *content;
  અચલ અક્ષર *pos;
  સંરચના dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#જો NETRC_DEBUG
સ્થિર અચલ અક્ષર *netrc_tokenstr(curl_netrc_token token)
{
  પસંદગી(token) {
  સ્થિતિ NETRC_TOK_EOF:
    પાછા "[EOF]";
  સ્થિતિ NETRC_TOK_LITERAL:
    પાછા "[LITERAL]";
  સ્થિતિ NETRC_TOK_MACHINE:
    પાછા "[MACHINE]";
  સ્થિતિ NETRC_TOK_DEFAULT:
    પાછા "[DEFAULT]";
  સ્થિતિ NETRC_TOK_ACCOUNT:
    પાછા "[ACCOUNT]";
  સ્થિતિ NETRC_TOK_LOGIN:
    પાછા "[LOGIN]";
  સ્થિતિ NETRC_TOK_PASSWD:
    પાછા "[PASSWORD]";
  સ્થિતિ NETRC_TOK_MACDEF:
    પાછા "[MACDEF]";
  સ્થિતિ NETRC_TOK_JUNK:
    પાછા "[JUNK]";
  મૂળભૂત:
    પાછા "[???]";
  }
}
#endif

સ્થિર શૂન્ય netrc_lexer_init(સંરચના netrc_lexer *lexer,
                             સંરચના Curl_easy *data,
                             અચલ અક્ષર *content)
{
  curlx_dyn_init(&lexer->literal, MAX_NETRC_TOKEN);
  lexer->data = data;
  lexer->content = lexer->pos = content;
}

સ્થિર શૂન્ય netrc_lexer_cleanup(સંરચના netrc_lexer *lexer)
{
  lexer->content = lexer->pos = ખાલી;
  lexer->data = ખાલી;
  curlx_dyn_free(&lexer->literal);
}

સ્થિર શૂન્ય netrc_skip_blanks(સંરચના netrc_lexer *lexer)
{
  અચલ અક્ષર *s = lexer->pos;
  જ્યારે(*s) {
    curlx_str_passblanks(&s);
    જ્યારે(*s == '\r')
      ++s;
    જો(*s == '\n') {
      ++s;
    }
    નહીંતર
      તોડો;
  }
  lexer->pos = s;
}

સ્થિર શૂન્ય netrc_skip_to_empty_લીટી(સંરચના netrc_lexer *lexer)
{
  અચલ અક્ષર *s = lexer->pos;
  જ્યારે(*s) {
    જો(*s == '\r')
      ++s;
    નહીંતર જો(*s == '\n') {
      ++s;
      જ્યારે(*s == '\r')
        ++s;
      જો(*s == '\n')
        જાઓ out;
    }
    નહીંતર
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
સ્થિર NETRCcode netrc_lexer_quoted(સંરચના netrc_lexer *lexer)
{
  NETRCcode rc = NETRC_SYNTAX_ERROR;
  અચલ અક્ષર *s = lexer->pos;
  bool escape = FALSE;
  CURLcode result;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  જ્યારે(*s) {
    અક્ષર c = *s;
    જો(escape) {
      escape = FALSE;
      પસંદગી(c) {
      સ્થિતિ 'n':
        c = '\n';
        તોડો;
      સ્થિતિ 'r':
        c = '\r';
        તોડો;
      સ્થિતિ 't':
        c = '\t';
        તોડો;
      }
    }
    નહીંતર જો(c == '\\') {
      escape = TRUE;
      ++s;
      ચાલુ;
    }
    નહીંતર જો(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      result = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(result);
      જાઓ out;
    }
    result = curlx_dyn_addn(&lexer->literal, &c, 1);
    જો(result) {
      rc = curl2netrc(result);
      જાઓ out;
    }
    ++s;
  }
out:
  lexer->pos = s;
  પાછા rc;
}

સ્થિર શૂન્ય netrc_lexer_ધકેલો(સંરચના netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

સ્થિર NETRCcode netrc_lexer_next(સંરચના netrc_lexer *lexer,
                                  bool want_literal)
{
  અચલ અક્ષર *s = lexer->pos, *start;
  NETRCcode rc = NETRC_OK;
  size_t slen;
  CURLcode result;

  જો(lexer->pushed) {
    lexer->pushed = FALSE;
    જાઓ out;
  }

  curlx_dyn_reset(&lexer->literal);
  netrc_skip_blanks(lexer);
  s = lexer->pos;

  પસંદગી(*s) {
  સ્થિતિ 0:
    lexer->token = NETRC_TOK_EOF;
    તોડો;
  સ્થિતિ '\"':
    rc = netrc_lexer_quoted(lexer);
    lexer->token = NETRC_TOK_LITERAL;
    s = lexer->pos;
    તોડો;
  મૂળભૂત:
    /* unquoted token */
    start = s;
    જ્યારે(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - start;
    જો(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    જો(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      result = curlx_dyn_addn(&lexer->literal, start, slen);
      rc = curl2netrc(result);
    }
    નહીંતર જો((slen == 7) && curl_strnequal(start, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    નહીંતર જો((slen == 7) && curl_strnequal(start, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    નહીંતર જો((slen == 7) && curl_strnequal(start, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    નહીંતર જો((slen == 5) && curl_strnequal(start, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    નહીંતર જો((slen == 8) && curl_strnequal(start, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    નહીંતર જો((slen == 6) && curl_strnequal(start, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    નહીંતર {
      lexer->token = NETRC_TOK_JUNK;
    }
    તોડો;
  }

out:
#જો NETRC_DEBUG
  CURL_TRC_M(lexer->data, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#endif
  lexer->pos = s;
  પાછા rc;
}

સંરચના netrc_scanner {
  સંરચના netrc_lexer lexer;
  અચલ અક્ષર *hostname; /* non-NULL, machine to scan for */
  અચલ અક્ષર *user; /* maybe NULL, login to scan for */
  અક્ષર *login;
  અક્ષર *passwd;
  સંરચના Curl_creds *creds;
  bool matches_host;
  bool found;
};

સ્થિર શૂન્ય netrc_scan_reset(સંરચના netrc_scanner *sc)
{
  curlx_safefree(sc->login);
  curlx_safefree(sc->passwd);
  sc->matches_host = FALSE;
}

સ્થિર શૂન્ય netrc_scan_init(સંરચના netrc_scanner *sc,
                            સંરચના Curl_easy *data,
                            અચલ અક્ષર *content,
                            અચલ અક્ષર *hostname,
                            અચલ અક્ષર *user)
{
  memset(sc, 0, કદ(*sc));
  netrc_lexer_init(&sc->lexer, data, content);
  sc->hostname = hostname;
  sc->user = (user && user[0]) ? user : ખાલી;
  netrc_scan_reset(sc);
}

સ્થિર શૂન્ય netrc_scan_cleanup(સંરચના netrc_scanner *sc)
{
  netrc_scan_reset(sc);
  sc->hostname = ખાલી;
  sc->user = ખાલી;
  Curl_creds_unlink(&sc->creds);
  netrc_lexer_cleanup(&sc->lexer);
}

સ્થિર NETRCcode netrc_scan_literal(સંરચના netrc_scanner *sc,
                                    અક્ષર **pdest)
{
  NETRCcode rc = netrc_lexer_next(&sc->lexer, TRUE);
  જો(!rc) {
    જો(sc->lexer.token == NETRC_TOK_LITERAL) {
      જો(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        જો(!*pdest)
          rc = NETRC_OUT_OF_MEMORY;
      }
    }
    નહીંતર
      netrc_lexer_ધકેલો(&sc->lexer);
  }
  પાછા rc;
}

સ્થિર NETRCcode netrc_scan_end_entry(સંરચના netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#જો NETRC_DEBUG
  CURL_TRC_M(sc->lexer.data,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#endif
  જો(sc->matches_host) {
    જો(sc->login) {
      જો(sc->user) {
        જો(Curl_timestrcmp(sc->user, sc->login))
          જાઓ out;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->found = !!sc->passwd;
      }
      નહીંતર {
        sc->found = TRUE;
      }
    }
    નહીંતર જો(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->found = TRUE;
    }
    નહીંતર {
      /* entry has nothing interesting */
    }
    જો(sc->found) {
#જો NETRC_DEBUG
      CURL_TRC_M(sc->lexer.data, "[NETRC] entry match found");
#endif
      જો(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           ખાલી, ખાલી, ખાલી, CREDS_NETRC, &sc->creds))
        rc = NETRC_OUT_OF_MEMORY;
    }
  }
out:
  netrc_scan_reset(sc);
  પાછા rc;
}

સ્થિર NETRCcode netrc_scan(સંરચના Curl_easy *data,
                            અચલ અક્ષર *content,
                            અચલ અક્ષર *hostname,
                            અચલ અક્ષર *user,
                            સંરચના Curl_creds **pcreds)
{
  સંરચના netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_scan_init(&sc, data, content, hostname, user);

  જ્યારે(!rc && !sc.found) {
    rc = netrc_lexer_next(&sc.lexer, FALSE);
    જો(!rc) {
      /* Does this token end any previous entry? */
      પસંદગી(sc.lexer.token) {
      સ્થિતિ NETRC_TOK_EOF:
      સ્થિતિ NETRC_TOK_MACHINE:
      સ્થિતિ NETRC_TOK_DEFAULT:
      સ્થિતિ NETRC_TOK_MACDEF:
        rc = netrc_scan_end_entry(&sc);
        જો(rc || sc.found)
          જાઓ out;
        તોડો;
      મૂળભૂત:
        તોડો;
      }

      પસંદગી(sc.lexer.token) {
      સ્થિતિ NETRC_TOK_EOF:
        જાઓ out;
      સ્થિતિ NETRC_TOK_MACHINE:
        rc = netrc_lexer_next(&sc.lexer, TRUE);
        જો(!rc) {
          જો(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          નહીંતર {
            sc.matches_host = FALSE;
            netrc_lexer_ધકેલો(&sc.lexer);
          }
        }
        તોડો;
      સ્થિતિ NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        તોડો;
      સ્થિતિ NETRC_TOK_ACCOUNT:
        rc = netrc_scan_literal(&sc, ખાલી); /* ignore, not used */
        તોડો;
      સ્થિતિ NETRC_TOK_LOGIN:
        rc = netrc_scan_literal(&sc, &sc.login);
        તોડો;
      સ્થિતિ NETRC_TOK_PASSWD:
        rc = netrc_scan_literal(&sc, &sc.passwd);
        તોડો;
      સ્થિતિ NETRC_TOK_MACDEF:
        netrc_skip_to_empty_લીટી(&sc.lexer);
        તોડો;
      સ્થિતિ NETRC_TOK_LITERAL:
      સ્થિતિ NETRC_TOK_JUNK:
      મૂળભૂત:
        /* skip this */
        તોડો;
      }
    }
  }

out:
  જો(!rc) {
    જો(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    નહીંતર
      rc = NETRC_NO_MATCH;
  }
  netrc_scan_cleanup(&sc);
  પાછા rc;
}

સ્થિર NETRCcode file2memory(અચલ અક્ષર *filename, સંરચના dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  જો(file) {
    curlx_struct_stat stat;
    જો((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      CURLcode result = CURLE_OK;
      bool eof;
      સંરચના dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_લીટી);
      ret = NETRC_OK;
      કરો {
        અચલ અક્ષર *લીટી;
        /* Curl_get_line always returns lines ending with a newline */
        result = Curl_get_લીટી(&linebuf, file, &eof);
        જો(!result) {
          લીટી = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&લીટી);
          જો(*લીટી == '#')
            ચાલુ;
          result = curlx_dyn_add(filebuf, લીટી);
        }
        જો(result) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(result);
          તોડો;
        }
      } જ્યારે(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  પાછા ret;
}

સ્થિર NETRCcode netrc_scan_file(સંરચના Curl_easy *data,
                                 સંરચના store_netrc *store,
                                 અચલ અક્ષર *hostname,
                                 અચલ અક્ષર *user,
                                 અચલ અક્ષર *netrcfile,
                                 સંરચના Curl_creds **pcreds)
{
  સંરચના dynbuf *filebuf = &store->filebuf;

  જો(!store->loaded || strcmp(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    જો(ret) {
      CURL_TRC_M(data, "[NETRC] could not load '%s'", netrcfile);
      પાછા ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    જો(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      પાછા NETRC_OUT_OF_MEMORY;
    }
    store->loaded = TRUE;
  }
  જો(!curlx_dyn_len(filebuf))
    પાછા NETRC_NO_MATCH;

  પાછા netrc_scan(data, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_scan(સંરચના Curl_easy *data,
                          સંરચના store_netrc *store,
                          અચલ અક્ષર *hostname,
                          અચલ અક્ષર *user,
                          અચલ અક્ષર *netrcfile,
                          સંરચના Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(data, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  જો(!netrcfile) {
    અક્ષર *home = ખાલી;
    અક્ષર *homea = ખાલી;
    અક્ષર *filealloc = ખાલી;
#જો defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    અક્ષર pwbuf[1024];
#endif
    filealloc = curl_getenv("NETRC");
    જો(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      જો(homea) {
        home = homea;
#જો defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      નહીંતર {
        સંરચના passwd pw, *pw_res;
        જો(!getpwuid_r(geteuid(), &pw, pwbuf, કદ(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#અથવાજો defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      નહીંતર {
        સંરચના passwd *pw;
        pw = getpwuid(geteuid());
        જો(pw) {
          home = pw->pw_dir;
        }
#અથવાજો defined(_WIN32)
      }
      નહીંતર {
        homea = curl_getenv("USERPROFILE");
        જો(homea) {
          home = homea;
        }
#endif
      }

      જો(!home)
        પાછા NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      જો(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_OUT_OF_MEMORY;
        જાઓ out;
      }
    }
    retcode = netrc_scan_file(
      data, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#ifdef _WIN32
    જો(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      જો(!filealloc) {
        curlx_free(homea);
        પાછા NETRC_OUT_OF_MEMORY;
      }
      retcode = netrc_scan_file(
        data, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#endif
    curlx_free(homea);
  }
  નહીંતર
    retcode = netrc_scan_file(
      data, store, hostname, user, netrcfile, pcreds);

out:
  જો(retcode)
    Curl_creds_unlink(pcreds);
  પાછા retcode;
}

શૂન્ય Curl_netrc_init(સંરચના store_netrc *store)
{
  curlx_dyn_init(&store->filebuf, MAX_NETRC_FILE);
  store->loaded = FALSE;
  store->filename = ખાલી;
}
શૂન્ય Curl_netrc_cleanup(સંરચના store_netrc *store)
{
  curlx_dyn_free(&store->filebuf);
  curlx_safefree(store->filename);
  store->loaded = FALSE;
}

અચલ અક્ષર *Curl_netrc_strerror(NETRCcode ret)
{
  પસંદગી(ret) {
  મૂળભૂત:
    પાછા ""; /* not a legit error */
  સ્થિતિ NETRC_FILE_MISSING:
    પાછા "no such file";
  સ્થિતિ NETRC_NO_MATCH:
    પાછા "no matching entry";
  સ્થિતિ NETRC_OUT_OF_MEMORY:
    પાછા "out of memory";
  સ્થિતિ NETRC_SYNTAX_ERROR:
    પાછા "syntax error";
  }
  /* never reached */
}

#endif /* !CURL_DISABLE_NETRC */
