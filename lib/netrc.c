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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifndef CURL_DISABLE_NETRC

#ifdef HAVE_PWD_H
#ifdef __AMIGA__
#undef __NO_NET_API /* required for AmigaOS to declare getpwuid() */
#endif
#ಅನ್ತರ್ಭಾವ <pwd.h>
#ifdef __AMIGA__
#define __NO_NET_API
#endif
#endif

#ಅನ್ತರ್ಭಾವ "netrc.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "creds.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "curl_get_line.h"
#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"


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

#define MAX_NETRC_ಸಾಲು  16384
#define MAX_NETRC_FILE  (128 * 1024)
#define MAX_NETRC_TOKEN 4096

#define NETRC_DEBUG   0

/* convert a dynbuf call CURLcode error to a NETRCcode error */
#define curl2netrc(r)                                  \
  ((!(r)) ? NETRC_OK : (((r) == CURLE_OUT_OF_MEMORY) ? \
   NETRC_OUT_OF_MEMORY : NETRC_SYNTAX_ERROR))

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
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

ಸಂರಚನಾ netrc_lexer {
  ಸಂರಚನಾ Curl_easy *data;
  ಧ್ರುವ ವರ್ಣ *content;
  ಧ್ರುವ ವರ್ಣ *pos;
  ಸಂರಚನಾ dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#ಯದಿ NETRC_DEBUG
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *netrc_tokenstr(curl_netrc_token token)
{
  ವಿಕಲ್ಪ(token) {
  ಪಕ್ಷ NETRC_TOK_EOF:
    ಪ್ರತ್ಯಾಗಮ "[EOF]";
  ಪಕ್ಷ NETRC_TOK_LITERAL:
    ಪ್ರತ್ಯಾಗಮ "[LITERAL]";
  ಪಕ್ಷ NETRC_TOK_MACHINE:
    ಪ್ರತ್ಯಾಗಮ "[MACHINE]";
  ಪಕ್ಷ NETRC_TOK_DEFAULT:
    ಪ್ರತ್ಯಾಗಮ "[DEFAULT]";
  ಪಕ್ಷ NETRC_TOK_ACCOUNT:
    ಪ್ರತ್ಯಾಗಮ "[ACCOUNT]";
  ಪಕ್ಷ NETRC_TOK_LOGIN:
    ಪ್ರತ್ಯಾಗಮ "[LOGIN]";
  ಪಕ್ಷ NETRC_TOK_PASSWD:
    ಪ್ರತ್ಯಾಗಮ "[PASSWORD]";
  ಪಕ್ಷ NETRC_TOK_MACDEF:
    ಪ್ರತ್ಯಾಗಮ "[MACDEF]";
  ಪಕ್ಷ NETRC_TOK_JUNK:
    ಪ್ರತ್ಯಾಗಮ "[JUNK]";
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ "[???]";
  }
}
#endif

ಸ್ಥಿರ ಶೂನ್ಯ netrc_lexer_init(ಸಂರಚನಾ netrc_lexer *lexer,
                             ಸಂರಚನಾ Curl_easy *data,
                             ಧ್ರುವ ವರ್ಣ *content)
{
  curlx_dyn_init(&lexer->literal, MAX_NETRC_TOKEN);
  lexer->data = data;
  lexer->content = lexer->pos = content;
}

ಸ್ಥಿರ ಶೂನ್ಯ netrc_lexer_cleanup(ಸಂರಚನಾ netrc_lexer *lexer)
{
  lexer->content = lexer->pos = ರಿಕ್ತ;
  lexer->data = ರಿಕ್ತ;
  curlx_dyn_free(&lexer->literal);
}

ಸ್ಥಿರ ಶೂನ್ಯ netrc_skip_blanks(ಸಂರಚನಾ netrc_lexer *lexer)
{
  ಧ್ರುವ ವರ್ಣ *s = lexer->pos;
  ಯಾವತ್(*s) {
    curlx_str_passblanks(&s);
    ಯಾವತ್(*s == '\r')
      ++s;
    ಯದಿ(*s == '\n') {
      ++s;
    }
    ಅನ್ಯಥಾ
      ಭಙ್ಗ;
  }
  lexer->pos = s;
}

ಸ್ಥಿರ ಶೂನ್ಯ netrc_skip_to_empty_ಸಾಲು(ಸಂರಚನಾ netrc_lexer *lexer)
{
  ಧ್ರುವ ವರ್ಣ *s = lexer->pos;
  ಯಾವತ್(*s) {
    ಯದಿ(*s == '\r')
      ++s;
    ಅನ್ಯಥಾ ಯದಿ(*s == '\n') {
      ++s;
      ಯಾವತ್(*s == '\r')
        ++s;
      ಯದಿ(*s == '\n')
        ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ
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
ಸ್ಥಿರ NETRCcode netrc_lexer_quoted(ಸಂರಚನಾ netrc_lexer *lexer)
{
  NETRCcode rc = NETRC_SYNTAX_ERROR;
  ಧ್ರುವ ವರ್ಣ *s = lexer->pos;
  bool escape = FALSE;
  CURLcode result;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  ಯಾವತ್(*s) {
    ವರ್ಣ c = *s;
    ಯದಿ(escape) {
      escape = FALSE;
      ವಿಕಲ್ಪ(c) {
      ಪಕ್ಷ 'n':
        c = '\n';
        ಭಙ್ಗ;
      ಪಕ್ಷ 'r':
        c = '\r';
        ಭಙ್ಗ;
      ಪಕ್ಷ 't':
        c = '\t';
        ಭಙ್ಗ;
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(c == '\\') {
      escape = TRUE;
      ++s;
      ಅನುವರ್ತ;
    }
    ಅನ್ಯಥಾ ಯದಿ(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      result = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(result);
      ಗಚ್ಛ out;
    }
    result = curlx_dyn_addn(&lexer->literal, &c, 1);
    ಯದಿ(result) {
      rc = curl2netrc(result);
      ಗಚ್ಛ out;
    }
    ++s;
  }
out:
  lexer->pos = s;
  ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಶೂನ್ಯ netrc_lexer_ತಳ್ಳು(ಸಂರಚನಾ netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

ಸ್ಥಿರ NETRCcode netrc_lexer_next(ಸಂರಚನಾ netrc_lexer *lexer,
                                  bool want_literal)
{
  ಧ್ರುವ ವರ್ಣ *s = lexer->pos, *start;
  NETRCcode rc = NETRC_OK;
  size_t slen;
  CURLcode result;

  ಯದಿ(lexer->pushed) {
    lexer->pushed = FALSE;
    ಗಚ್ಛ out;
  }

  curlx_dyn_reset(&lexer->literal);
  netrc_skip_blanks(lexer);
  s = lexer->pos;

  ವಿಕಲ್ಪ(*s) {
  ಪಕ್ಷ 0:
    lexer->token = NETRC_TOK_EOF;
    ಭಙ್ಗ;
  ಪಕ್ಷ '\"':
    rc = netrc_lexer_quoted(lexer);
    lexer->token = NETRC_TOK_LITERAL;
    s = lexer->pos;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    /* unquoted token */
    start = s;
    ಯಾವತ್(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - start;
    ಯದಿ(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    ಯದಿ(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      result = curlx_dyn_addn(&lexer->literal, start, slen);
      rc = curl2netrc(result);
    }
    ಅನ್ಯಥಾ ಯದಿ((slen == 7) && curl_strnequal(start, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    ಅನ್ಯಥಾ ಯದಿ((slen == 7) && curl_strnequal(start, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    ಅನ್ಯಥಾ ಯದಿ((slen == 7) && curl_strnequal(start, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    ಅನ್ಯಥಾ ಯದಿ((slen == 5) && curl_strnequal(start, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    ಅನ್ಯಥಾ ಯದಿ((slen == 8) && curl_strnequal(start, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    ಅನ್ಯಥಾ ಯದಿ((slen == 6) && curl_strnequal(start, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    ಅನ್ಯಥಾ {
      lexer->token = NETRC_TOK_JUNK;
    }
    ಭಙ್ಗ;
  }

out:
#ಯದಿ NETRC_DEBUG
  CURL_TRC_M(lexer->data, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#endif
  lexer->pos = s;
  ಪ್ರತ್ಯಾಗಮ rc;
}

ಸಂರಚನಾ netrc_scanner {
  ಸಂರಚನಾ netrc_lexer lexer;
  ಧ್ರುವ ವರ್ಣ *hostname; /* non-NULL, machine to scan for */
  ಧ್ರುವ ವರ್ಣ *user; /* maybe NULL, login to scan for */
  ವರ್ಣ *login;
  ವರ್ಣ *passwd;
  ಸಂರಚನಾ Curl_creds *creds;
  bool matches_host;
  bool found;
};

ಸ್ಥಿರ ಶೂನ್ಯ netrc_scan_reset(ಸಂರಚನಾ netrc_scanner *sc)
{
  curlx_safefree(sc->login);
  curlx_safefree(sc->passwd);
  sc->matches_host = FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ netrc_scan_init(ಸಂರಚನಾ netrc_scanner *sc,
                            ಸಂರಚನಾ Curl_easy *data,
                            ಧ್ರುವ ವರ್ಣ *content,
                            ಧ್ರುವ ವರ್ಣ *hostname,
                            ಧ್ರುವ ವರ್ಣ *user)
{
  memset(sc, 0, ಪರಿಮಾಣ(*sc));
  netrc_lexer_init(&sc->lexer, data, content);
  sc->hostname = hostname;
  sc->user = (user && user[0]) ? user : ರಿಕ್ತ;
  netrc_scan_reset(sc);
}

ಸ್ಥಿರ ಶೂನ್ಯ netrc_scan_cleanup(ಸಂರಚನಾ netrc_scanner *sc)
{
  netrc_scan_reset(sc);
  sc->hostname = ರಿಕ್ತ;
  sc->user = ರಿಕ್ತ;
  Curl_creds_unlink(&sc->creds);
  netrc_lexer_cleanup(&sc->lexer);
}

ಸ್ಥಿರ NETRCcode netrc_scan_literal(ಸಂರಚನಾ netrc_scanner *sc,
                                    ವರ್ಣ **pdest)
{
  NETRCcode rc = netrc_lexer_next(&sc->lexer, TRUE);
  ಯದಿ(!rc) {
    ಯದಿ(sc->lexer.token == NETRC_TOK_LITERAL) {
      ಯದಿ(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        ಯದಿ(!*pdest)
          rc = NETRC_OUT_OF_MEMORY;
      }
    }
    ಅನ್ಯಥಾ
      netrc_lexer_ತಳ್ಳು(&sc->lexer);
  }
  ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ NETRCcode netrc_scan_end_entry(ಸಂರಚನಾ netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#ಯದಿ NETRC_DEBUG
  CURL_TRC_M(sc->lexer.data,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#endif
  ಯದಿ(sc->matches_host) {
    ಯದಿ(sc->login) {
      ಯದಿ(sc->user) {
        ಯದಿ(Curl_timestrcmp(sc->user, sc->login))
          ಗಚ್ಛ out;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->found = !!sc->passwd;
      }
      ಅನ್ಯಥಾ {
        sc->found = TRUE;
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->found = TRUE;
    }
    ಅನ್ಯಥಾ {
      /* entry has nothing interesting */
    }
    ಯದಿ(sc->found) {
#ಯದಿ NETRC_DEBUG
      CURL_TRC_M(sc->lexer.data, "[NETRC] entry match found");
#endif
      ಯದಿ(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, CREDS_NETRC, &sc->creds))
        rc = NETRC_OUT_OF_MEMORY;
    }
  }
out:
  netrc_scan_reset(sc);
  ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ NETRCcode netrc_scan(ಸಂರಚನಾ Curl_easy *data,
                            ಧ್ರುವ ವರ್ಣ *content,
                            ಧ್ರುವ ವರ್ಣ *hostname,
                            ಧ್ರುವ ವರ್ಣ *user,
                            ಸಂರಚನಾ Curl_creds **pcreds)
{
  ಸಂರಚನಾ netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_scan_init(&sc, data, content, hostname, user);

  ಯಾವತ್(!rc && !sc.found) {
    rc = netrc_lexer_next(&sc.lexer, FALSE);
    ಯದಿ(!rc) {
      /* Does this token end any previous entry? */
      ವಿಕಲ್ಪ(sc.lexer.token) {
      ಪಕ್ಷ NETRC_TOK_EOF:
      ಪಕ್ಷ NETRC_TOK_MACHINE:
      ಪಕ್ಷ NETRC_TOK_DEFAULT:
      ಪಕ್ಷ NETRC_TOK_MACDEF:
        rc = netrc_scan_end_entry(&sc);
        ಯದಿ(rc || sc.found)
          ಗಚ್ಛ out;
        ಭಙ್ಗ;
      ಸ್ವಭಾವ:
        ಭಙ್ಗ;
      }

      ವಿಕಲ್ಪ(sc.lexer.token) {
      ಪಕ್ಷ NETRC_TOK_EOF:
        ಗಚ್ಛ out;
      ಪಕ್ಷ NETRC_TOK_MACHINE:
        rc = netrc_lexer_next(&sc.lexer, TRUE);
        ಯದಿ(!rc) {
          ಯದಿ(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          ಅನ್ಯಥಾ {
            sc.matches_host = FALSE;
            netrc_lexer_ತಳ್ಳು(&sc.lexer);
          }
        }
        ಭಙ್ಗ;
      ಪಕ್ಷ NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        ಭಙ್ಗ;
      ಪಕ್ಷ NETRC_TOK_ACCOUNT:
        rc = netrc_scan_literal(&sc, ರಿಕ್ತ); /* ignore, not used */
        ಭಙ್ಗ;
      ಪಕ್ಷ NETRC_TOK_LOGIN:
        rc = netrc_scan_literal(&sc, &sc.login);
        ಭಙ್ಗ;
      ಪಕ್ಷ NETRC_TOK_PASSWD:
        rc = netrc_scan_literal(&sc, &sc.passwd);
        ಭಙ್ಗ;
      ಪಕ್ಷ NETRC_TOK_MACDEF:
        netrc_skip_to_empty_ಸಾಲು(&sc.lexer);
        ಭಙ್ಗ;
      ಪಕ್ಷ NETRC_TOK_LITERAL:
      ಪಕ್ಷ NETRC_TOK_JUNK:
      ಸ್ವಭಾವ:
        /* skip this */
        ಭಙ್ಗ;
      }
    }
  }

out:
  ಯದಿ(!rc) {
    ಯದಿ(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    ಅನ್ಯಥಾ
      rc = NETRC_NO_MATCH;
  }
  netrc_scan_cleanup(&sc);
  ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ NETRCcode file2memory(ಧ್ರುವ ವರ್ಣ *filename, ಸಂರಚನಾ dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  ಯದಿ(file) {
    curlx_struct_stat stat;
    ಯದಿ((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      CURLcode result = CURLE_OK;
      bool eof;
      ಸಂರಚನಾ dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_ಸಾಲು);
      ret = NETRC_OK;
      ಕುರು {
        ಧ್ರುವ ವರ್ಣ *ಸಾಲು;
        /* Curl_get_line always returns lines ending with a newline */
        result = Curl_get_ಸಾಲು(&linebuf, file, &eof);
        ಯದಿ(!result) {
          ಸಾಲು = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&ಸಾಲು);
          ಯದಿ(*ಸಾಲು == '#')
            ಅನುವರ್ತ;
          result = curlx_dyn_add(filebuf, ಸಾಲು);
        }
        ಯದಿ(result) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(result);
          ಭಙ್ಗ;
        }
      } ಯಾವತ್(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ NETRCcode netrc_scan_file(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ store_netrc *store,
                                 ಧ್ರುವ ವರ್ಣ *hostname,
                                 ಧ್ರುವ ವರ್ಣ *user,
                                 ಧ್ರುವ ವರ್ಣ *netrcfile,
                                 ಸಂರಚನಾ Curl_creds **pcreds)
{
  ಸಂರಚನಾ dynbuf *filebuf = &store->filebuf;

  ಯದಿ(!store->loaded || strcmp(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    ಯದಿ(ret) {
      CURL_TRC_M(data, "[NETRC] could not load '%s'", netrcfile);
      ಪ್ರತ್ಯಾಗಮ ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    ಯದಿ(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      ಪ್ರತ್ಯಾಗಮ NETRC_OUT_OF_MEMORY;
    }
    store->loaded = TRUE;
  }
  ಯದಿ(!curlx_dyn_len(filebuf))
    ಪ್ರತ್ಯಾಗಮ NETRC_NO_MATCH;

  ಪ್ರತ್ಯಾಗಮ netrc_scan(data, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_scan(ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ store_netrc *store,
                          ಧ್ರುವ ವರ್ಣ *hostname,
                          ಧ್ರುವ ವರ್ಣ *user,
                          ಧ್ರುವ ವರ್ಣ *netrcfile,
                          ಸಂರಚನಾ Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(data, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  ಯದಿ(!netrcfile) {
    ವರ್ಣ *home = ರಿಕ್ತ;
    ವರ್ಣ *homea = ರಿಕ್ತ;
    ವರ್ಣ *filealloc = ರಿಕ್ತ;
#ಯದಿ defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    ವರ್ಣ pwbuf[1024];
#endif
    filealloc = curl_getenv("NETRC");
    ಯದಿ(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      ಯದಿ(homea) {
        home = homea;
#ಯದಿ defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      ಅನ್ಯಥಾ {
        ಸಂರಚನಾ passwd pw, *pw_res;
        ಯದಿ(!getpwuid_r(geteuid(), &pw, pwbuf, ಪರಿಮಾಣ(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#ನೋಚೇದ್ಯದಿ defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      ಅನ್ಯಥಾ {
        ಸಂರಚನಾ passwd *pw;
        pw = getpwuid(geteuid());
        ಯದಿ(pw) {
          home = pw->pw_dir;
        }
#ನೋಚೇದ್ಯದಿ defined(_WIN32)
      }
      ಅನ್ಯಥಾ {
        homea = curl_getenv("USERPROFILE");
        ಯದಿ(homea) {
          home = homea;
        }
#endif
      }

      ಯದಿ(!home)
        ಪ್ರತ್ಯಾಗಮ NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      ಯದಿ(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_OUT_OF_MEMORY;
        ಗಚ್ಛ out;
      }
    }
    retcode = netrc_scan_file(
      data, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#ifdef _WIN32
    ಯದಿ(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      ಯದಿ(!filealloc) {
        curlx_free(homea);
        ಪ್ರತ್ಯಾಗಮ NETRC_OUT_OF_MEMORY;
      }
      retcode = netrc_scan_file(
        data, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#endif
    curlx_free(homea);
  }
  ಅನ್ಯಥಾ
    retcode = netrc_scan_file(
      data, store, hostname, user, netrcfile, pcreds);

out:
  ಯದಿ(retcode)
    Curl_creds_unlink(pcreds);
  ಪ್ರತ್ಯಾಗಮ retcode;
}

ಶೂನ್ಯ Curl_netrc_init(ಸಂರಚನಾ store_netrc *store)
{
  curlx_dyn_init(&store->filebuf, MAX_NETRC_FILE);
  store->loaded = FALSE;
  store->filename = ರಿಕ್ತ;
}
ಶೂನ್ಯ Curl_netrc_cleanup(ಸಂರಚನಾ store_netrc *store)
{
  curlx_dyn_free(&store->filebuf);
  curlx_safefree(store->filename);
  store->loaded = FALSE;
}

ಧ್ರುವ ವರ್ಣ *Curl_netrc_strerror(NETRCcode ret)
{
  ವಿಕಲ್ಪ(ret) {
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ ""; /* not a legit error */
  ಪಕ್ಷ NETRC_FILE_MISSING:
    ಪ್ರತ್ಯಾಗಮ "no such file";
  ಪಕ್ಷ NETRC_NO_MATCH:
    ಪ್ರತ್ಯಾಗಮ "no matching entry";
  ಪಕ್ಷ NETRC_OUT_OF_MEMORY:
    ಪ್ರತ್ಯಾಗಮ "out of memory";
  ಪಕ್ಷ NETRC_SYNTAX_ERROR:
    ಪ್ರತ್ಯಾಗಮ "syntax error";
  }
  /* never reached */
}

#endif /* !CURL_DISABLE_NETRC */
