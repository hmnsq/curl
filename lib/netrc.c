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
#অন্তর্ভাব "curl_setup.h"

#ifndef CURL_DISABLE_NETRC

#ifdef HAVE_PWD_H
#ifdef __AMIGA__
#undef __NO_NET_API /* required for AmigaOS to declare getpwuid() */
#endif
#অন্তর্ভাব <pwd.h>
#ifdef __AMIGA__
#define __NO_NET_API
#endif
#endif

#অন্তর্ভাব "netrc.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "creds.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "curl_get_line.h"
#অন্তর্ভাব "curlx/win32-fopen.h"
#অন্তর্ভাব "curlx/strparse.h"


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

#define MAX_NETRC_সারি  16384
#define MAX_NETRC_FILE  (128 * 1024)
#define MAX_NETRC_TOKEN 4096

#define NETRC_DEBUG   0

/* convert a dynbuf call CURLcode error to a NETRCcode error */
#define curl2netrc(r)                                  \
  ((!(r)) ? NETRC_OK : (((r) == CURLE_OUT_OF_MEMORY) ? \
   NETRC_OUT_OF_MEMORY : NETRC_SYNTAX_ERROR))

প্রকারলেখ গণনা {
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

কাঠামো netrc_lexer {
  কাঠামো Curl_easy *তথ্য;
  ধ্রুবক অক্ষর *content;
  ধ্রুবক অক্ষর *pos;
  কাঠামো dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#যদি NETRC_DEBUG
স্থির ধ্রুবক অক্ষর *netrc_tokenstr(curl_netrc_token token)
{
  নির্বাচন(token) {
  ক্ষেত্র NETRC_TOK_EOF:
    ফেরত "[EOF]";
  ক্ষেত্র NETRC_TOK_LITERAL:
    ফেরত "[LITERAL]";
  ক্ষেত্র NETRC_TOK_MACHINE:
    ফেরত "[MACHINE]";
  ক্ষেত্র NETRC_TOK_DEFAULT:
    ফেরত "[DEFAULT]";
  ক্ষেত্র NETRC_TOK_ACCOUNT:
    ফেরত "[ACCOUNT]";
  ক্ষেত্র NETRC_TOK_LOGIN:
    ফেরত "[LOGIN]";
  ক্ষেত্র NETRC_TOK_PASSWD:
    ফেরত "[PASSWORD]";
  ক্ষেত্র NETRC_TOK_MACDEF:
    ফেরত "[MACDEF]";
  ক্ষেত্র NETRC_TOK_JUNK:
    ফেরত "[JUNK]";
  পূর্বনির্ধারিত:
    ফেরত "[???]";
  }
}
#endif

স্থির শূন্য netrc_lexer_init(কাঠামো netrc_lexer *lexer,
                             কাঠামো Curl_easy *তথ্য,
                             ধ্রুবক অক্ষর *content)
{
  curlx_dyn_init(&lexer->literal, MAX_NETRC_TOKEN);
  lexer->তথ্য = তথ্য;
  lexer->content = lexer->pos = content;
}

স্থির শূন্য netrc_lexer_cleanup(কাঠামো netrc_lexer *lexer)
{
  lexer->content = lexer->pos = ফাঁকা;
  lexer->তথ্য = ফাঁকা;
  curlx_dyn_free(&lexer->literal);
}

স্থির শূন্য netrc_skip_blanks(কাঠামো netrc_lexer *lexer)
{
  ধ্রুবক অক্ষর *s = lexer->pos;
  যখন(*s) {
    curlx_str_passblanks(&s);
    যখন(*s == '\r')
      ++s;
    যদি(*s == '\n') {
      ++s;
    }
    অন্যথা
      বিরতি;
  }
  lexer->pos = s;
}

স্থির শূন্য netrc_skip_to_empty_সারি(কাঠামো netrc_lexer *lexer)
{
  ধ্রুবক অক্ষর *s = lexer->pos;
  যখন(*s) {
    যদি(*s == '\r')
      ++s;
    অন্যথা যদি(*s == '\n') {
      ++s;
      যখন(*s == '\r')
        ++s;
      যদি(*s == '\n')
        যাও out;
    }
    অন্যথা
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
স্থির NETRCcode netrc_lexer_quoted(কাঠামো netrc_lexer *lexer)
{
  NETRCcode rc = NETRC_SYNTAX_ERROR;
  ধ্রুবক অক্ষর *s = lexer->pos;
  bool escape = FALSE;
  CURLcode ফলাফল;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  যখন(*s) {
    অক্ষর c = *s;
    যদি(escape) {
      escape = FALSE;
      নির্বাচন(c) {
      ক্ষেত্র 'n':
        c = '\n';
        বিরতি;
      ক্ষেত্র 'r':
        c = '\r';
        বিরতি;
      ক্ষেত্র 't':
        c = '\t';
        বিরতি;
      }
    }
    অন্যথা যদি(c == '\\') {
      escape = TRUE;
      ++s;
      চালু;
    }
    অন্যথা যদি(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      ফলাফল = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(ফলাফল);
      যাও out;
    }
    ফলাফল = curlx_dyn_addn(&lexer->literal, &c, 1);
    যদি(ফলাফল) {
      rc = curl2netrc(ফলাফল);
      যাও out;
    }
    ++s;
  }
out:
  lexer->pos = s;
  ফেরত rc;
}

স্থির শূন্য netrc_lexer_ঠেলো(কাঠামো netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

স্থির NETRCcode netrc_lexer_পরবর্তী(কাঠামো netrc_lexer *lexer,
                                  bool want_literal)
{
  ধ্রুবক অক্ষর *s = lexer->pos, *start;
  NETRCcode rc = NETRC_OK;
  size_t slen;
  CURLcode ফলাফল;

  যদি(lexer->pushed) {
    lexer->pushed = FALSE;
    যাও out;
  }

  curlx_dyn_reset(&lexer->literal);
  netrc_skip_blanks(lexer);
  s = lexer->pos;

  নির্বাচন(*s) {
  ক্ষেত্র 0:
    lexer->token = NETRC_TOK_EOF;
    বিরতি;
  ক্ষেত্র '\"':
    rc = netrc_lexer_quoted(lexer);
    lexer->token = NETRC_TOK_LITERAL;
    s = lexer->pos;
    বিরতি;
  পূর্বনির্ধারিত:
    /* unquoted token */
    start = s;
    যখন(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - start;
    যদি(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    যদি(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      ফলাফল = curlx_dyn_addn(&lexer->literal, start, slen);
      rc = curl2netrc(ফলাফল);
    }
    অন্যথা যদি((slen == 7) && curl_strnequal(start, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    অন্যথা যদি((slen == 7) && curl_strnequal(start, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    অন্যথা যদি((slen == 7) && curl_strnequal(start, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    অন্যথা যদি((slen == 5) && curl_strnequal(start, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    অন্যথা যদি((slen == 8) && curl_strnequal(start, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    অন্যথা যদি((slen == 6) && curl_strnequal(start, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    অন্যথা {
      lexer->token = NETRC_TOK_JUNK;
    }
    বিরতি;
  }

out:
#যদি NETRC_DEBUG
  CURL_TRC_M(lexer->তথ্য, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#endif
  lexer->pos = s;
  ফেরত rc;
}

কাঠামো netrc_scanner {
  কাঠামো netrc_lexer lexer;
  ধ্রুবক অক্ষর *hostname; /* non-NULL, machine to scan for */
  ধ্রুবক অক্ষর *user; /* maybe NULL, login to scan for */
  অক্ষর *login;
  অক্ষর *passwd;
  কাঠামো Curl_creds *creds;
  bool matches_host;
  bool found;
};

স্থির শূন্য netrc_scan_reset(কাঠামো netrc_scanner *sc)
{
  curlx_safefree(sc->login);
  curlx_safefree(sc->passwd);
  sc->matches_host = FALSE;
}

স্থির শূন্য netrc_scan_init(কাঠামো netrc_scanner *sc,
                            কাঠামো Curl_easy *তথ্য,
                            ধ্রুবক অক্ষর *content,
                            ধ্রুবক অক্ষর *hostname,
                            ধ্রুবক অক্ষর *user)
{
  memset(sc, 0, আকার(*sc));
  netrc_lexer_init(&sc->lexer, তথ্য, content);
  sc->hostname = hostname;
  sc->user = (user && user[0]) ? user : ফাঁকা;
  netrc_scan_reset(sc);
}

স্থির শূন্য netrc_scan_cleanup(কাঠামো netrc_scanner *sc)
{
  netrc_scan_reset(sc);
  sc->hostname = ফাঁকা;
  sc->user = ফাঁকা;
  Curl_creds_unlink(&sc->creds);
  netrc_lexer_cleanup(&sc->lexer);
}

স্থির NETRCcode netrc_scan_literal(কাঠামো netrc_scanner *sc,
                                    অক্ষর **pdest)
{
  NETRCcode rc = netrc_lexer_পরবর্তী(&sc->lexer, TRUE);
  যদি(!rc) {
    যদি(sc->lexer.token == NETRC_TOK_LITERAL) {
      যদি(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        যদি(!*pdest)
          rc = NETRC_OUT_OF_MEMORY;
      }
    }
    অন্যথা
      netrc_lexer_ঠেলো(&sc->lexer);
  }
  ফেরত rc;
}

স্থির NETRCcode netrc_scan_end_entry(কাঠামো netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#যদি NETRC_DEBUG
  CURL_TRC_M(sc->lexer.তথ্য,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#endif
  যদি(sc->matches_host) {
    যদি(sc->login) {
      যদি(sc->user) {
        যদি(Curl_timestrcmp(sc->user, sc->login))
          যাও out;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->found = !!sc->passwd;
      }
      অন্যথা {
        sc->found = TRUE;
      }
    }
    অন্যথা যদি(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->found = TRUE;
    }
    অন্যথা {
      /* entry has nothing interesting */
    }
    যদি(sc->found) {
#যদি NETRC_DEBUG
      CURL_TRC_M(sc->lexer.তথ্য, "[NETRC] entry match found");
#endif
      যদি(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           ফাঁকা, ফাঁকা, ফাঁকা, CREDS_NETRC, &sc->creds))
        rc = NETRC_OUT_OF_MEMORY;
    }
  }
out:
  netrc_scan_reset(sc);
  ফেরত rc;
}

স্থির NETRCcode netrc_scan(কাঠামো Curl_easy *তথ্য,
                            ধ্রুবক অক্ষর *content,
                            ধ্রুবক অক্ষর *hostname,
                            ধ্রুবক অক্ষর *user,
                            কাঠামো Curl_creds **pcreds)
{
  কাঠামো netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_scan_init(&sc, তথ্য, content, hostname, user);

  যখন(!rc && !sc.found) {
    rc = netrc_lexer_পরবর্তী(&sc.lexer, FALSE);
    যদি(!rc) {
      /* Does this token end any previous entry? */
      নির্বাচন(sc.lexer.token) {
      ক্ষেত্র NETRC_TOK_EOF:
      ক্ষেত্র NETRC_TOK_MACHINE:
      ক্ষেত্র NETRC_TOK_DEFAULT:
      ক্ষেত্র NETRC_TOK_MACDEF:
        rc = netrc_scan_end_entry(&sc);
        যদি(rc || sc.found)
          যাও out;
        বিরতি;
      পূর্বনির্ধারিত:
        বিরতি;
      }

      নির্বাচন(sc.lexer.token) {
      ক্ষেত্র NETRC_TOK_EOF:
        যাও out;
      ক্ষেত্র NETRC_TOK_MACHINE:
        rc = netrc_lexer_পরবর্তী(&sc.lexer, TRUE);
        যদি(!rc) {
          যদি(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          অন্যথা {
            sc.matches_host = FALSE;
            netrc_lexer_ঠেলো(&sc.lexer);
          }
        }
        বিরতি;
      ক্ষেত্র NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        বিরতি;
      ক্ষেত্র NETRC_TOK_ACCOUNT:
        rc = netrc_scan_literal(&sc, ফাঁকা); /* ignore, not used */
        বিরতি;
      ক্ষেত্র NETRC_TOK_LOGIN:
        rc = netrc_scan_literal(&sc, &sc.login);
        বিরতি;
      ক্ষেত্র NETRC_TOK_PASSWD:
        rc = netrc_scan_literal(&sc, &sc.passwd);
        বিরতি;
      ক্ষেত্র NETRC_TOK_MACDEF:
        netrc_skip_to_empty_সারি(&sc.lexer);
        বিরতি;
      ক্ষেত্র NETRC_TOK_LITERAL:
      ক্ষেত্র NETRC_TOK_JUNK:
      পূর্বনির্ধারিত:
        /* skip this */
        বিরতি;
      }
    }
  }

out:
  যদি(!rc) {
    যদি(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    অন্যথা
      rc = NETRC_NO_MATCH;
  }
  netrc_scan_cleanup(&sc);
  ফেরত rc;
}

স্থির NETRCcode file2memory(ধ্রুবক অক্ষর *filename, কাঠামো dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  যদি(file) {
    curlx_struct_stat stat;
    যদি((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      CURLcode ফলাফল = CURLE_OK;
      bool eof;
      কাঠামো dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_সারি);
      ret = NETRC_OK;
      করো {
        ধ্রুবক অক্ষর *সারি;
        /* Curl_get_line always returns lines ending with a newline */
        ফলাফল = Curl_get_সারি(&linebuf, file, &eof);
        যদি(!ফলাফল) {
          সারি = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&সারি);
          যদি(*সারি == '#')
            চালু;
          ফলাফল = curlx_dyn_add(filebuf, সারি);
        }
        যদি(ফলাফল) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(ফলাফল);
          বিরতি;
        }
      } যখন(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  ফেরত ret;
}

স্থির NETRCcode netrc_scan_file(কাঠামো Curl_easy *তথ্য,
                                 কাঠামো store_netrc *store,
                                 ধ্রুবক অক্ষর *hostname,
                                 ধ্রুবক অক্ষর *user,
                                 ধ্রুবক অক্ষর *netrcfile,
                                 কাঠামো Curl_creds **pcreds)
{
  কাঠামো dynbuf *filebuf = &store->filebuf;

  যদি(!store->loaded || strcmp(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    যদি(ret) {
      CURL_TRC_M(তথ্য, "[NETRC] could not load '%s'", netrcfile);
      ফেরত ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    যদি(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      ফেরত NETRC_OUT_OF_MEMORY;
    }
    store->loaded = TRUE;
  }
  যদি(!curlx_dyn_len(filebuf))
    ফেরত NETRC_NO_MATCH;

  ফেরত netrc_scan(তথ্য, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_scan(কাঠামো Curl_easy *তথ্য,
                          কাঠামো store_netrc *store,
                          ধ্রুবক অক্ষর *hostname,
                          ধ্রুবক অক্ষর *user,
                          ধ্রুবক অক্ষর *netrcfile,
                          কাঠামো Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(তথ্য, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  যদি(!netrcfile) {
    অক্ষর *home = ফাঁকা;
    অক্ষর *homea = ফাঁকা;
    অক্ষর *filealloc = ফাঁকা;
#যদি defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    অক্ষর pwbuf[1024];
#endif
    filealloc = curl_getenv("NETRC");
    যদি(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      যদি(homea) {
        home = homea;
#যদি defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      অন্যথা {
        কাঠামো passwd pw, *pw_res;
        যদি(!getpwuid_r(geteuid(), &pw, pwbuf, আকার(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#নতুবা_যদি defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      অন্যথা {
        কাঠামো passwd *pw;
        pw = getpwuid(geteuid());
        যদি(pw) {
          home = pw->pw_dir;
        }
#নতুবা_যদি defined(_WIN32)
      }
      অন্যথা {
        homea = curl_getenv("USERPROFILE");
        যদি(homea) {
          home = homea;
        }
#endif
      }

      যদি(!home)
        ফেরত NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      যদি(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_OUT_OF_MEMORY;
        যাও out;
      }
    }
    retcode = netrc_scan_file(
      তথ্য, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#ifdef _WIN32
    যদি(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      যদি(!filealloc) {
        curlx_free(homea);
        ফেরত NETRC_OUT_OF_MEMORY;
      }
      retcode = netrc_scan_file(
        তথ্য, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#endif
    curlx_free(homea);
  }
  অন্যথা
    retcode = netrc_scan_file(
      তথ্য, store, hostname, user, netrcfile, pcreds);

out:
  যদি(retcode)
    Curl_creds_unlink(pcreds);
  ফেরত retcode;
}

শূন্য Curl_netrc_init(কাঠামো store_netrc *store)
{
  curlx_dyn_init(&store->filebuf, MAX_NETRC_FILE);
  store->loaded = FALSE;
  store->filename = ফাঁকা;
}
শূন্য Curl_netrc_cleanup(কাঠামো store_netrc *store)
{
  curlx_dyn_free(&store->filebuf);
  curlx_safefree(store->filename);
  store->loaded = FALSE;
}

ধ্রুবক অক্ষর *Curl_netrc_strerror(NETRCcode ret)
{
  নির্বাচন(ret) {
  পূর্বনির্ধারিত:
    ফেরত ""; /* not a legit error */
  ক্ষেত্র NETRC_FILE_MISSING:
    ফেরত "no such file";
  ক্ষেত্র NETRC_NO_MATCH:
    ফেরত "no matching entry";
  ক্ষেত্র NETRC_OUT_OF_MEMORY:
    ফেরত "out of memory";
  ক্ষেত্র NETRC_SYNTAX_ERROR:
    ফেরত "syntax error";
  }
  /* never reached */
}

#endif /* !CURL_DISABLE_NETRC */
