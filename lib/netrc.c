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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_NETRC

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_PWD_H
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __AMIGA__
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು __NO_NET_API /* required for AmigaOS to declare getpwuid() */
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <pwd.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __AMIGA__
#ವ್ಯಾಖ್ಯಾನಿಸು __NO_NET_API
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

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

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_NETRC_ಸಾಲು  16384
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_NETRC_FILE  (128 * 1024)
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_NETRC_TOKEN 4096

#ವ್ಯಾಖ್ಯಾನಿಸು NETRC_DEBUG   0

/* convert a dynbuf call CURLcode error to a NETRCcode error */
#ವ್ಯಾಖ್ಯಾನಿಸು curl2netrc(r)                                  \
  ((!(r)) ? NETRC_OK : (((r) == CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ) ? \
   NETRC_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ : NETRC_SYNTAX_ERROR))

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
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

ರಚನೆ netrc_lexer {
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *content;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pos;
  ರಚನೆ dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#ಆದರೆ NETRC_DEBUG
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *netrc_tokenstr(curl_netrc_token token)
{
  ಆಯ್ಕೆ(token) {
  ಸ್ಥಿತಿ NETRC_TOK_EOF:
    ಹಿಂತಿರುಗಿ "[EOF]";
  ಸ್ಥಿತಿ NETRC_TOK_LITERAL:
    ಹಿಂತಿರುಗಿ "[LITERAL]";
  ಸ್ಥಿತಿ NETRC_TOK_MACHINE:
    ಹಿಂತಿರುಗಿ "[MACHINE]";
  ಸ್ಥಿತಿ NETRC_TOK_DEFAULT:
    ಹಿಂತಿರುಗಿ "[DEFAULT]";
  ಸ್ಥಿತಿ NETRC_TOK_ACCOUNT:
    ಹಿಂತಿರುಗಿ "[ACCOUNT]";
  ಸ್ಥಿತಿ NETRC_TOK_LOGIN:
    ಹಿಂತಿರುಗಿ "[LOGIN]";
  ಸ್ಥಿತಿ NETRC_TOK_PASSWD:
    ಹಿಂತಿರುಗಿ "[PASSWORD]";
  ಸ್ಥಿತಿ NETRC_TOK_MACDEF:
    ಹಿಂತಿರುಗಿ "[MACDEF]";
  ಸ್ಥಿತಿ NETRC_TOK_JUNK:
    ಹಿಂತಿರುಗಿ "[JUNK]";
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ "[???]";
  }
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ netrc_lexer_init(ರಚನೆ netrc_lexer *lexer,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *content)
{
  curlx_dyn_init(&lexer->literal, MAX_NETRC_TOKEN);
  lexer->ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;
  lexer->content = lexer->pos = content;
}

ಸ್ಥಿರ ಶೂನ್ಯ netrc_lexer_cleanup(ರಚನೆ netrc_lexer *lexer)
{
  lexer->content = lexer->pos = ಖಾಲಿ;
  lexer->ದತ್ತಾಂಶ = ಖಾಲಿ;
  curlx_dyn_free(&lexer->literal);
}

ಸ್ಥಿರ ಶೂನ್ಯ netrc_skip_blanks(ರಚನೆ netrc_lexer *lexer)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = lexer->pos;
  ಆಗ(*s) {
    curlx_str_passblanks(&s);
    ಆಗ(*s == '\r')
      ++s;
    ಆದರೆ(*s == '\n') {
      ++s;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ನಿಲ್ಲಿಸು;
  }
  lexer->pos = s;
}

ಸ್ಥಿರ ಶೂನ್ಯ netrc_skip_ಗಮ್ಯ_ಖಾಲಿಗಳು_ಸಾಲು(ರಚನೆ netrc_lexer *lexer)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = lexer->pos;
  ಆಗ(*s) {
    ಆದರೆ(*s == '\r')
      ++s;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*s == '\n') {
      ++s;
      ಆಗ(*s == '\r')
        ++s;
      ಆದರೆ(*s == '\n')
        ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ++s;
  }
ಹೊರಹೋಗು:
  lexer->pos = s;
}

/*
 * Parse a quoted token starting after the opening '"'. Handles \n, \r, \t
 * escape sequences. Advances *tok_endp past the closing '"'.
 *
 * Returns NETRC_OK or error.
 */
ಸ್ಥಿರ NETRCcode netrc_lexer_quoted(ರಚನೆ netrc_lexer *lexer)
{
  NETRCcode rc = NETRC_SYNTAX_ERROR;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = lexer->pos;
  bool escape = FALSE;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  ಆಗ(*s) {
    ಅಕ್ಷರ c = *s;
    ಆದರೆ(escape) {
      escape = FALSE;
      ಆಯ್ಕೆ(c) {
      ಸ್ಥಿತಿ 'n':
        c = '\n';
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ 'r':
        c = '\r';
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ 't':
        c = '\t';
        ನಿಲ್ಲಿಸು;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == '\\') {
      escape = TRUE;
      ++s;
      ಮುಂದುವರಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(ಫಲಿತಾಂಶ);
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಫಲಿತಾಂಶ = curlx_dyn_addn(&lexer->literal, &c, 1);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      rc = curl2netrc(ಫಲಿತಾಂಶ);
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ++s;
  }
ಹೊರಹೋಗು:
  lexer->pos = s;
  ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಶೂನ್ಯ netrc_lexer_ತಳ್ಳು(ರಚನೆ netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

ಸ್ಥಿರ NETRCcode netrc_lexer_ಮುಂದಿನ(ರಚನೆ netrc_lexer *lexer,
                                  bool want_literal)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = lexer->pos, *ಆರಂಭ;
  NETRCcode rc = NETRC_OK;
  size_t slen;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(lexer->pushed) {
    lexer->pushed = FALSE;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  curlx_dyn_reset(&lexer->literal);
  netrc_skip_blanks(lexer);
  s = lexer->pos;

  ಆಯ್ಕೆ(*s) {
  ಸ್ಥಿತಿ 0:
    lexer->token = NETRC_TOK_EOF;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ '\"':
    rc = netrc_lexer_quoted(lexer);
    lexer->token = NETRC_TOK_LITERAL;
    s = lexer->pos;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    /* unquoted token */
    ಆರಂಭ = s;
    ಆಗ(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - ಆರಂಭ;
    ಆದರೆ(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    ಆದರೆ(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&lexer->literal, ಆರಂಭ, slen);
      rc = curl2netrc(ಫಲಿತಾಂಶ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((slen == 7) && curl_strnequal(ಆರಂಭ, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((slen == 7) && curl_strnequal(ಆರಂಭ, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((slen == 7) && curl_strnequal(ಆರಂಭ, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((slen == 5) && curl_strnequal(ಆರಂಭ, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((slen == 8) && curl_strnequal(ಆರಂಭ, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((slen == 6) && curl_strnequal(ಆರಂಭ, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      lexer->token = NETRC_TOK_JUNK;
    }
    ನಿಲ್ಲಿಸು;
  }

ಹೊರಹೋಗು:
#ಆದರೆ NETRC_DEBUG
  CURL_TRC_M(lexer->ದತ್ತಾಂಶ, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#ಆದರೆ_ಅಂತ್ಯ
  lexer->pos = s;
  ಹಿಂತಿರುಗಿ rc;
}

ರಚನೆ netrc_scanner {
  ರಚನೆ netrc_lexer lexer;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname; /* non-NULL, machine to scan for */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user; /* maybe NULL, login to scan for */
  ಅಕ್ಷರ *login;
  ಅಕ್ಷರ *passwd;
  ರಚನೆ Curl_creds *creds;
  bool matches_host;
  bool ಸಿಕ್ಕಿತು;
};

ಸ್ಥಿರ ಶೂನ್ಯ netrc_ಪರಿಶೀಲನೆ_reset(ರಚನೆ netrc_scanner *sc)
{
  curlx_safefree(sc->login);
  curlx_safefree(sc->passwd);
  sc->matches_host = FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ netrc_ಪರಿಶೀಲನೆ_init(ರಚನೆ netrc_scanner *sc,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *content,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user)
{
  memset(sc, 0, ಗಾತ್ರ(*sc));
  netrc_lexer_init(&sc->lexer, ದತ್ತಾಂಶ, content);
  sc->hostname = hostname;
  sc->user = (user && user[0]) ? user : ಖಾಲಿ;
  netrc_ಪರಿಶೀಲನೆ_reset(sc);
}

ಸ್ಥಿರ ಶೂನ್ಯ netrc_ಪರಿಶೀಲನೆ_cleanup(ರಚನೆ netrc_scanner *sc)
{
  netrc_ಪರಿಶೀಲನೆ_reset(sc);
  sc->hostname = ಖಾಲಿ;
  sc->user = ಖಾಲಿ;
  Curl_creds_unlink(&sc->creds);
  netrc_lexer_cleanup(&sc->lexer);
}

ಸ್ಥಿರ NETRCcode netrc_ಪರಿಶೀಲನೆ_literal(ರಚನೆ netrc_scanner *sc,
                                    ಅಕ್ಷರ **pdest)
{
  NETRCcode rc = netrc_lexer_ಮುಂದಿನ(&sc->lexer, TRUE);
  ಆದರೆ(!rc) {
    ಆದರೆ(sc->lexer.token == NETRC_TOK_LITERAL) {
      ಆದರೆ(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        ಆದರೆ(!*pdest)
          rc = NETRC_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ
      netrc_lexer_ತಳ್ಳು(&sc->lexer);
  }
  ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ NETRCcode netrc_ಪರಿಶೀಲನೆ_ಕೊನೆ_ನಮೂದು(ರಚನೆ netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#ಆದರೆ NETRC_DEBUG
  CURL_TRC_M(sc->lexer.ದತ್ತಾಂಶ,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(sc->matches_host) {
    ಆದರೆ(sc->login) {
      ಆದರೆ(sc->user) {
        ಆದರೆ(Curl_timestrcmp(sc->user, sc->login))
          ಹೋಗು ಹೊರಹೋಗು;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->ಸಿಕ್ಕಿತು = !!sc->passwd;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        sc->ಸಿಕ್ಕಿತು = TRUE;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->ಸಿಕ್ಕಿತು = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* entry has nothing interesting */
    }
    ಆದರೆ(sc->ಸಿಕ್ಕಿತು) {
#ಆದರೆ NETRC_DEBUG
      CURL_TRC_M(sc->lexer.ದತ್ತಾಂಶ, "[NETRC] entry match found");
#ಆದರೆ_ಅಂತ್ಯ
      ಆದರೆ(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, CREDS_NETRC, &sc->creds))
        rc = NETRC_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }
ಹೊರಹೋಗು:
  netrc_ಪರಿಶೀಲನೆ_reset(sc);
  ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ NETRCcode netrc_ಪರಿಶೀಲನೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *content,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user,
                            ರಚನೆ Curl_creds **pcreds)
{
  ರಚನೆ netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_ಪರಿಶೀಲನೆ_init(&sc, ದತ್ತಾಂಶ, content, hostname, user);

  ಆಗ(!rc && !sc.ಸಿಕ್ಕಿತು) {
    rc = netrc_lexer_ಮುಂದಿನ(&sc.lexer, FALSE);
    ಆದರೆ(!rc) {
      /* Does this token end any previous entry? */
      ಆಯ್ಕೆ(sc.lexer.token) {
      ಸ್ಥಿತಿ NETRC_TOK_EOF:
      ಸ್ಥಿತಿ NETRC_TOK_MACHINE:
      ಸ್ಥಿತಿ NETRC_TOK_DEFAULT:
      ಸ್ಥಿತಿ NETRC_TOK_MACDEF:
        rc = netrc_ಪರಿಶೀಲನೆ_ಕೊನೆ_ನಮೂದು(&sc);
        ಆದರೆ(rc || sc.ಸಿಕ್ಕಿತು)
          ಹೋಗು ಹೊರಹೋಗು;
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು;
      }

      ಆಯ್ಕೆ(sc.lexer.token) {
      ಸ್ಥಿತಿ NETRC_TOK_EOF:
        ಹೋಗು ಹೊರಹೋಗು;
      ಸ್ಥಿತಿ NETRC_TOK_MACHINE:
        rc = netrc_lexer_ಮುಂದಿನ(&sc.lexer, TRUE);
        ಆದರೆ(!rc) {
          ಆದರೆ(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          ಇಲ್ಲದಿದ್ದರೆ {
            sc.matches_host = FALSE;
            netrc_lexer_ತಳ್ಳು(&sc.lexer);
          }
        }
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NETRC_TOK_ACCOUNT:
        rc = netrc_ಪರಿಶೀಲನೆ_literal(&sc, ಖಾಲಿ); /* ignore, not used */
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NETRC_TOK_LOGIN:
        rc = netrc_ಪರಿಶೀಲನೆ_literal(&sc, &sc.login);
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NETRC_TOK_PASSWD:
        rc = netrc_ಪರಿಶೀಲನೆ_literal(&sc, &sc.passwd);
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NETRC_TOK_MACDEF:
        netrc_skip_ಗಮ್ಯ_ಖಾಲಿಗಳು_ಸಾಲು(&sc.lexer);
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NETRC_TOK_LITERAL:
      ಸ್ಥಿತಿ NETRC_TOK_JUNK:
      ಪೂರ್ವನಿಯೋಜಿತ:
        /* skip this */
        ನಿಲ್ಲಿಸು;
      }
    }
  }

ಹೊರಹೋಗು:
  ಆದರೆ(!rc) {
    ಆದರೆ(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    ಇಲ್ಲದಿದ್ದರೆ
      rc = NETRC_NO_ಹೊಂದಿಕೆ;
  }
  netrc_ಪರಿಶೀಲನೆ_cleanup(&sc);
  ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ NETRCcode file2memory(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ರಚನೆ dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  ಆದರೆ(file) {
    curlx_struct_stat stat;
    ಆದರೆ((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_ಬಹುಲಕ)) {
      CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
      bool eof;
      ರಚನೆ dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_ಸಾಲು);
      ret = NETRC_OK;
      ಮಾಡು {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು;
        /* Curl_get_line always returns lines ending with a newline */
        ಫಲಿತಾಂಶ = Curl_get_ಸಾಲು(&linebuf, file, &eof);
        ಆದರೆ(!ಫಲಿತಾಂಶ) {
          ಸಾಲು = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&ಸಾಲು);
          ಆದರೆ(*ಸಾಲು == '#')
            ಮುಂದುವರಿ;
          ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(filebuf, ಸಾಲು);
        }
        ಆದರೆ(ಫಲಿತಾಂಶ) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(ಫಲಿತಾಂಶ);
          ನಿಲ್ಲಿಸು;
        }
      } ಆಗ(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ NETRCcode netrc_ಪರಿಶೀಲನೆ_file(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ store_netrc *store,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *netrcfile,
                                 ರಚನೆ Curl_creds **pcreds)
{
  ರಚನೆ dynbuf *filebuf = &store->filebuf;

  ಆದರೆ(!store->loaded || strcmp(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    ಆದರೆ(ret) {
      CURL_TRC_M(ದತ್ತಾಂಶ, "[NETRC] could not load '%s'", netrcfile);
      ಹಿಂತಿರುಗಿ ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    ಆದರೆ(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      ಹಿಂತಿರುಗಿ NETRC_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    store->loaded = TRUE;
  }
  ಆದರೆ(!curlx_dyn_len(filebuf))
    ಹಿಂತಿರುಗಿ NETRC_NO_ಹೊಂದಿಕೆ;

  ಹಿಂತಿರುಗಿ netrc_ಪರಿಶೀಲನೆ(ದತ್ತಾಂಶ, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_ಪರಿಶೀಲನೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ store_netrc *store,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *netrcfile,
                          ರಚನೆ Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(ದತ್ತಾಂಶ, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  ಆದರೆ(!netrcfile) {
    ಅಕ್ಷರ *home = ಖಾಲಿ;
    ಅಕ್ಷರ *homea = ಖಾಲಿ;
    ಅಕ್ಷರ *filealloc = ಖಾಲಿ;
#ಆದರೆ defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    ಅಕ್ಷರ pwbuf[1024];
#ಆದರೆ_ಅಂತ್ಯ
    filealloc = curl_getenv("NETRC");
    ಆದರೆ(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      ಆದರೆ(homea) {
        home = homea;
#ಆದರೆ defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ರಚನೆ passwd pw, *pw_res;
        ಆದರೆ(!getpwuid_r(geteuid(), &pw, pwbuf, ಗಾತ್ರ(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#ಅಥವಾಆದರೆ defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ರಚನೆ passwd *pw;
        pw = getpwuid(geteuid());
        ಆದರೆ(pw) {
          home = pw->pw_dir;
        }
#ಅಥವಾಆದರೆ defined(_WIN32)
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        homea = curl_getenv("USERPROFILE");
        ಆದರೆ(homea) {
          home = homea;
        }
#ಆದರೆ_ಅಂತ್ಯ
      }

      ಆದರೆ(!home)
        ಹಿಂತಿರುಗಿ NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      ಆದರೆ(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
    retcode = netrc_ಪರಿಶೀಲನೆ_file(
      ದತ್ತಾಂಶ, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
    ಆದರೆ(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      ಆದರೆ(!filealloc) {
        curlx_free(homea);
        ಹಿಂತಿರುಗಿ NETRC_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
      retcode = netrc_ಪರಿಶೀಲನೆ_file(
        ದತ್ತಾಂಶ, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#ಆದರೆ_ಅಂತ್ಯ
    curlx_free(homea);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    retcode = netrc_ಪರಿಶೀಲನೆ_file(
      ದತ್ತಾಂಶ, store, hostname, user, netrcfile, pcreds);

ಹೊರಹೋಗು:
  ಆದರೆ(retcode)
    Curl_creds_unlink(pcreds);
  ಹಿಂತಿರುಗಿ retcode;
}

ಶೂನ್ಯ Curl_netrc_init(ರಚನೆ store_netrc *store)
{
  curlx_dyn_init(&store->filebuf, MAX_NETRC_FILE);
  store->loaded = FALSE;
  store->filename = ಖಾಲಿ;
}
ಶೂನ್ಯ Curl_netrc_cleanup(ರಚನೆ store_netrc *store)
{
  curlx_dyn_free(&store->filebuf);
  curlx_safefree(store->filename);
  store->loaded = FALSE;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_netrc_strerror(NETRCcode ret)
{
  ಆಯ್ಕೆ(ret) {
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ ""; /* not a legit error */
  ಸ್ಥಿತಿ NETRC_FILE_MISSING:
    ಹಿಂತಿರುಗಿ "no such file";
  ಸ್ಥಿತಿ NETRC_NO_ಹೊಂದಿಕೆ:
    ಹಿಂತಿರುಗಿ "no matching entry";
  ಸ್ಥಿತಿ NETRC_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ:
    ಹಿಂತಿರುಗಿ "out of memory";
  ಸ್ಥಿತಿ NETRC_SYNTAX_ERROR:
    ಹಿಂತಿರುಗಿ "syntax error";
  }
  /* never reached */
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_NETRC */
