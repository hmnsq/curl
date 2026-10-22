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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_NETRC

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_PWD_H
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __AMIGA__
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ __NO_NET_API /* required for AmigaOS to declare getpwuid() */
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <pwd.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __AMIGA__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __NO_NET_API
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "netrc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "creds.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_get_line.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/win32-fopen.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"


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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_NETRC_ਸਤਰ  16384
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_NETRC_FILE  (128 * 1024)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_NETRC_TOKEN 4096

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NETRC_DEBUG   0

/* convert a dynbuf call CURLcode error to a NETRCcode error */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curl2netrc(r)                                  \
  ((!(r)) ? NETRC_OK : (((r) == CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ) ? \
   NETRC_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ : NETRC_SYNTAX_ERROR))

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
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

ਬਣਤਰ netrc_lexer {
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;
  ਅਚਲ ਅੱਖਰ *content;
  ਅਚਲ ਅੱਖਰ *pos;
  ਬਣਤਰ dynbuf literal;
  curl_netrc_token token;
  bool pushed;
};

#ਜੇ NETRC_DEBUG
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *netrc_tokenstr(curl_netrc_token token)
{
  ਚੋਣ(token) {
  ਹਾਲਤ NETRC_TOK_EOF:
    ਵਾਪਸ "[EOF]";
  ਹਾਲਤ NETRC_TOK_LITERAL:
    ਵਾਪਸ "[LITERAL]";
  ਹਾਲਤ NETRC_TOK_MACHINE:
    ਵਾਪਸ "[MACHINE]";
  ਹਾਲਤ NETRC_TOK_DEFAULT:
    ਵਾਪਸ "[DEFAULT]";
  ਹਾਲਤ NETRC_TOK_ACCOUNT:
    ਵਾਪਸ "[ACCOUNT]";
  ਹਾਲਤ NETRC_TOK_LOGIN:
    ਵਾਪਸ "[LOGIN]";
  ਹਾਲਤ NETRC_TOK_PASSWD:
    ਵਾਪਸ "[PASSWORD]";
  ਹਾਲਤ NETRC_TOK_MACDEF:
    ਵਾਪਸ "[MACDEF]";
  ਹਾਲਤ NETRC_TOK_JUNK:
    ਵਾਪਸ "[JUNK]";
  ਮੂਲ:
    ਵਾਪਸ "[???]";
  }
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ netrc_lexer_init(ਬਣਤਰ netrc_lexer *lexer,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਅਚਲ ਅੱਖਰ *content)
{
  curlx_dyn_init(&lexer->literal, MAX_NETRC_TOKEN);
  lexer->ਅੰਕੜੇ = ਅੰਕੜੇ;
  lexer->content = lexer->pos = content;
}

ਸਥਿਰ ਖਾਲੀ netrc_lexer_cleanup(ਬਣਤਰ netrc_lexer *lexer)
{
  lexer->content = lexer->pos = ਨਿਰਾ;
  lexer->ਅੰਕੜੇ = ਨਿਰਾ;
  curlx_dyn_free(&lexer->literal);
}

ਸਥਿਰ ਖਾਲੀ netrc_skip_blanks(ਬਣਤਰ netrc_lexer *lexer)
{
  ਅਚਲ ਅੱਖਰ *s = lexer->pos;
  ਜਦੋਂ(*s) {
    curlx_str_passblanks(&s);
    ਜਦੋਂ(*s == '\r')
      ++s;
    ਜੇ(*s == '\n') {
      ++s;
    }
    ਨਹੀਂਤਾਂ
      ਰੋਕੋ;
  }
  lexer->pos = s;
}

ਸਥਿਰ ਖਾਲੀ netrc_skip_ਮੰਜ਼ਿਲ_ਖਾਲੀ_ਥਾਵਾਂ_ਸਤਰ(ਬਣਤਰ netrc_lexer *lexer)
{
  ਅਚਲ ਅੱਖਰ *s = lexer->pos;
  ਜਦੋਂ(*s) {
    ਜੇ(*s == '\r')
      ++s;
    ਨਹੀਂਤਾਂ ਜੇ(*s == '\n') {
      ++s;
      ਜਦੋਂ(*s == '\r')
        ++s;
      ਜੇ(*s == '\n')
        ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ
      ++s;
  }
ਬਾਹਰਲਾ:
  lexer->pos = s;
}

/*
 * Parse a quoted token starting after the opening '"'. Handles \n, \r, \t
 * escape sequences. Advances *tok_endp past the closing '"'.
 *
 * Returns NETRC_OK or error.
 */
ਸਥਿਰ NETRCcode netrc_lexer_quoted(ਬਣਤਰ netrc_lexer *lexer)
{
  NETRCcode rc = NETRC_SYNTAX_ERROR;
  ਅਚਲ ਅੱਖਰ *s = lexer->pos;
  bool escape = FALSE;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(*s == '\"');
  ++s; /* pass the leading quote */
  ਜਦੋਂ(*s) {
    ਅੱਖਰ c = *s;
    ਜੇ(escape) {
      escape = FALSE;
      ਚੋਣ(c) {
      ਹਾਲਤ 'n':
        c = '\n';
        ਰੋਕੋ;
      ਹਾਲਤ 'r':
        c = '\r';
        ਰੋਕੋ;
      ਹਾਲਤ 't':
        c = '\t';
        ਰੋਕੋ;
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(c == '\\') {
      escape = TRUE;
      ++s;
      ਜਾਰੀ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(c == '\"') {
      ++s; /* pass the ending quote */
      /* Ensure even an empty literal has a null-terminator. */
      ਨਤੀਜਾ = curlx_dyn_addn(&lexer->literal, "", 0);
      rc = curl2netrc(ਨਤੀਜਾ);
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਤੀਜਾ = curlx_dyn_addn(&lexer->literal, &c, 1);
    ਜੇ(ਨਤੀਜਾ) {
      rc = curl2netrc(ਨਤੀਜਾ);
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ++s;
  }
ਬਾਹਰਲਾ:
  lexer->pos = s;
  ਵਾਪਸ rc;
}

ਸਥਿਰ ਖਾਲੀ netrc_lexer_ਧੱਕੋ(ਬਣਤਰ netrc_lexer *lexer)
{
  lexer->pushed = TRUE;
}

ਸਥਿਰ NETRCcode netrc_lexer_ਅਗਲਾ(ਬਣਤਰ netrc_lexer *lexer,
                                  bool want_literal)
{
  ਅਚਲ ਅੱਖਰ *s = lexer->pos, *ਸ਼ੁਰੂ;
  NETRCcode rc = NETRC_OK;
  size_t slen;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(lexer->pushed) {
    lexer->pushed = FALSE;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  curlx_dyn_reset(&lexer->literal);
  netrc_skip_blanks(lexer);
  s = lexer->pos;

  ਚੋਣ(*s) {
  ਹਾਲਤ 0:
    lexer->token = NETRC_TOK_EOF;
    ਰੋਕੋ;
  ਹਾਲਤ '\"':
    rc = netrc_lexer_quoted(lexer);
    lexer->token = NETRC_TOK_LITERAL;
    s = lexer->pos;
    ਰੋਕੋ;
  ਮੂਲ:
    /* unquoted token */
    ਸ਼ੁਰੂ = s;
    ਜਦੋਂ(*s && !ISBLANK(*s) && !ISNEWLINE(*s))
      ++s;
    slen = s - ਸ਼ੁਰੂ;
    ਜੇ(!slen) {
      rc = NETRC_SYNTAX_ERROR;
    }
    ਜੇ(want_literal) {
      lexer->token = NETRC_TOK_LITERAL;
      ਨਤੀਜਾ = curlx_dyn_addn(&lexer->literal, ਸ਼ੁਰੂ, slen);
      rc = curl2netrc(ਨਤੀਜਾ);
    }
    ਨਹੀਂਤਾਂ ਜੇ((slen == 7) && curl_strnequal(ਸ਼ੁਰੂ, "machine", slen)) {
      lexer->token = NETRC_TOK_MACHINE;
    }
    ਨਹੀਂਤਾਂ ਜੇ((slen == 7) && curl_strnequal(ਸ਼ੁਰੂ, "default", slen)) {
      lexer->token = NETRC_TOK_DEFAULT;
    }
    ਨਹੀਂਤਾਂ ਜੇ((slen == 7) && curl_strnequal(ਸ਼ੁਰੂ, "account", slen)) {
      lexer->token = NETRC_TOK_ACCOUNT;
    }
    ਨਹੀਂਤਾਂ ਜੇ((slen == 5) && curl_strnequal(ਸ਼ੁਰੂ, "login", slen)) {
      lexer->token = NETRC_TOK_LOGIN;
    }
    ਨਹੀਂਤਾਂ ਜੇ((slen == 8) && curl_strnequal(ਸ਼ੁਰੂ, "password", slen)) {
      lexer->token = NETRC_TOK_PASSWD;
    }
    ਨਹੀਂਤਾਂ ਜੇ((slen == 6) && curl_strnequal(ਸ਼ੁਰੂ, "macdef", slen)) {
      lexer->token = NETRC_TOK_MACDEF;
    }
    ਨਹੀਂਤਾਂ {
      lexer->token = NETRC_TOK_JUNK;
    }
    ਰੋਕੋ;
  }

ਬਾਹਰਲਾ:
#ਜੇ NETRC_DEBUG
  CURL_TRC_M(lexer->ਅੰਕੜੇ, "[NETRC] token %s '%s', rc=%d",
             netrc_tokenstr(lexer->token),
             curlx_dyn_ptr(&lexer->literal), rc);
#ਜੇ_ਅੰਤ
  lexer->pos = s;
  ਵਾਪਸ rc;
}

ਬਣਤਰ netrc_scanner {
  ਬਣਤਰ netrc_lexer lexer;
  ਅਚਲ ਅੱਖਰ *hostname; /* non-NULL, machine to scan for */
  ਅਚਲ ਅੱਖਰ *user; /* maybe NULL, login to scan for */
  ਅੱਖਰ *login;
  ਅੱਖਰ *passwd;
  ਬਣਤਰ Curl_creds *creds;
  bool matches_host;
  bool ਮਿਲਿਆ;
};

ਸਥਿਰ ਖਾਲੀ netrc_ਜਾਂਚ_reset(ਬਣਤਰ netrc_scanner *sc)
{
  curlx_safefree(sc->login);
  curlx_safefree(sc->passwd);
  sc->matches_host = FALSE;
}

ਸਥਿਰ ਖਾਲੀ netrc_ਜਾਂਚ_init(ਬਣਤਰ netrc_scanner *sc,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਅਚਲ ਅੱਖਰ *content,
                            ਅਚਲ ਅੱਖਰ *hostname,
                            ਅਚਲ ਅੱਖਰ *user)
{
  memset(sc, 0, ਆਕਾਰ(*sc));
  netrc_lexer_init(&sc->lexer, ਅੰਕੜੇ, content);
  sc->hostname = hostname;
  sc->user = (user && user[0]) ? user : ਨਿਰਾ;
  netrc_ਜਾਂਚ_reset(sc);
}

ਸਥਿਰ ਖਾਲੀ netrc_ਜਾਂਚ_cleanup(ਬਣਤਰ netrc_scanner *sc)
{
  netrc_ਜਾਂਚ_reset(sc);
  sc->hostname = ਨਿਰਾ;
  sc->user = ਨਿਰਾ;
  Curl_creds_unlink(&sc->creds);
  netrc_lexer_cleanup(&sc->lexer);
}

ਸਥਿਰ NETRCcode netrc_ਜਾਂਚ_literal(ਬਣਤਰ netrc_scanner *sc,
                                    ਅੱਖਰ **pdest)
{
  NETRCcode rc = netrc_lexer_ਅਗਲਾ(&sc->lexer, TRUE);
  ਜੇ(!rc) {
    ਜੇ(sc->lexer.token == NETRC_TOK_LITERAL) {
      ਜੇ(pdest && sc->matches_host) {
        curlx_free(*pdest);
        *pdest = curlx_strdup(curlx_dyn_ptr(&sc->lexer.literal));
        ਜੇ(!*pdest)
          rc = NETRC_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
    }
    ਨਹੀਂਤਾਂ
      netrc_lexer_ਧੱਕੋ(&sc->lexer);
  }
  ਵਾਪਸ rc;
}

ਸਥਿਰ NETRCcode netrc_ਜਾਂਚ_ਸਿਰਾ_ਇੰਦਰਾਜ(ਬਣਤਰ netrc_scanner *sc)
{
  NETRCcode rc = NETRC_OK;
#ਜੇ NETRC_DEBUG
  CURL_TRC_M(sc->lexer.ਅੰਕੜੇ,
             "[NETRC] entry matches_host=%d, login='%s', passwd='%s'",
             sc->matches_host, sc->login, sc->passwd);
#ਜੇ_ਅੰਤ
  ਜੇ(sc->matches_host) {
    ਜੇ(sc->login) {
      ਜੇ(sc->user) {
        ਜੇ(Curl_timestrcmp(sc->user, sc->login))
          ਜਾਓ ਬਾਹਰਲਾ;
        /* We look for a specific user,
         * entry is only interesting with password */
        sc->ਮਿਲਿਆ = !!sc->passwd;
      }
      ਨਹੀਂਤਾਂ {
        sc->ਮਿਲਿਆ = TRUE;
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(sc->passwd) {
      /* found a passwd that applies to any user */
      sc->ਮਿਲਿਆ = TRUE;
    }
    ਨਹੀਂਤਾਂ {
      /* entry has nothing interesting */
    }
    ਜੇ(sc->ਮਿਲਿਆ) {
#ਜੇ NETRC_DEBUG
      CURL_TRC_M(sc->lexer.ਅੰਕੜੇ, "[NETRC] entry match found");
#ਜੇ_ਅੰਤ
      ਜੇ(Curl_creds_create(sc->user ? sc->user : sc->login, sc->passwd,
                           ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, CREDS_NETRC, &sc->creds))
        rc = NETRC_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }
ਬਾਹਰਲਾ:
  netrc_ਜਾਂਚ_reset(sc);
  ਵਾਪਸ rc;
}

ਸਥਿਰ NETRCcode netrc_ਜਾਂਚ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਅਚਲ ਅੱਖਰ *content,
                            ਅਚਲ ਅੱਖਰ *hostname,
                            ਅਚਲ ਅੱਖਰ *user,
                            ਬਣਤਰ Curl_creds **pcreds)
{
  ਬਣਤਰ netrc_scanner sc;
  NETRCcode rc = NETRC_OK;

  Curl_creds_unlink(pcreds);
  netrc_ਜਾਂਚ_init(&sc, ਅੰਕੜੇ, content, hostname, user);

  ਜਦੋਂ(!rc && !sc.ਮਿਲਿਆ) {
    rc = netrc_lexer_ਅਗਲਾ(&sc.lexer, FALSE);
    ਜੇ(!rc) {
      /* Does this token end any previous entry? */
      ਚੋਣ(sc.lexer.token) {
      ਹਾਲਤ NETRC_TOK_EOF:
      ਹਾਲਤ NETRC_TOK_MACHINE:
      ਹਾਲਤ NETRC_TOK_DEFAULT:
      ਹਾਲਤ NETRC_TOK_MACDEF:
        rc = netrc_ਜਾਂਚ_ਸਿਰਾ_ਇੰਦਰਾਜ(&sc);
        ਜੇ(rc || sc.ਮਿਲਿਆ)
          ਜਾਓ ਬਾਹਰਲਾ;
        ਰੋਕੋ;
      ਮੂਲ:
        ਰੋਕੋ;
      }

      ਚੋਣ(sc.lexer.token) {
      ਹਾਲਤ NETRC_TOK_EOF:
        ਜਾਓ ਬਾਹਰਲਾ;
      ਹਾਲਤ NETRC_TOK_MACHINE:
        rc = netrc_lexer_ਅਗਲਾ(&sc.lexer, TRUE);
        ਜੇ(!rc) {
          ਜੇ(sc.lexer.token == NETRC_TOK_LITERAL) {
            sc.matches_host = curl_strequal(
              sc.hostname, curlx_dyn_ptr(&sc.lexer.literal));
          }
          ਨਹੀਂਤਾਂ {
            sc.matches_host = FALSE;
            netrc_lexer_ਧੱਕੋ(&sc.lexer);
          }
        }
        ਰੋਕੋ;
      ਹਾਲਤ NETRC_TOK_DEFAULT:
        sc.matches_host = TRUE;
        ਰੋਕੋ;
      ਹਾਲਤ NETRC_TOK_ACCOUNT:
        rc = netrc_ਜਾਂਚ_literal(&sc, ਨਿਰਾ); /* ignore, not used */
        ਰੋਕੋ;
      ਹਾਲਤ NETRC_TOK_LOGIN:
        rc = netrc_ਜਾਂਚ_literal(&sc, &sc.login);
        ਰੋਕੋ;
      ਹਾਲਤ NETRC_TOK_PASSWD:
        rc = netrc_ਜਾਂਚ_literal(&sc, &sc.passwd);
        ਰੋਕੋ;
      ਹਾਲਤ NETRC_TOK_MACDEF:
        netrc_skip_ਮੰਜ਼ਿਲ_ਖਾਲੀ_ਥਾਵਾਂ_ਸਤਰ(&sc.lexer);
        ਰੋਕੋ;
      ਹਾਲਤ NETRC_TOK_LITERAL:
      ਹਾਲਤ NETRC_TOK_JUNK:
      ਮੂਲ:
        /* skip this */
        ਰੋਕੋ;
      }
    }
  }

ਬਾਹਰਲਾ:
  ਜੇ(!rc) {
    ਜੇ(sc.creds)
      Curl_creds_link(pcreds, sc.creds);
    ਨਹੀਂਤਾਂ
      rc = NETRC_NO_ਮੇਲ;
  }
  netrc_ਜਾਂਚ_cleanup(&sc);
  ਵਾਪਸ rc;
}

ਸਥਿਰ NETRCcode file2memory(ਅਚਲ ਅੱਖਰ *filename, ਬਣਤਰ dynbuf *filebuf)
{
  NETRCcode ret = NETRC_FILE_MISSING; /* if it cannot open the file */
  FILE *file = curlx_fopen(filename, FOPEN_READTEXT);

  curlx_dyn_reset(filebuf);
  ਜੇ(file) {
    curlx_struct_stat stat;
    ਜੇ((curlx_fstat(fileno(file), &stat) == -1) || !S_ISDIR(stat.st_ਬਹੁਲਕ)) {
      CURLcode ਨਤੀਜਾ = CURLE_OK;
      bool eof;
      ਬਣਤਰ dynbuf linebuf;
      curlx_dyn_init(&linebuf, MAX_NETRC_ਸਤਰ);
      ret = NETRC_OK;
      ਕਰੋ {
        ਅਚਲ ਅੱਖਰ *ਸਤਰ;
        /* Curl_get_line always returns lines ending with a newline */
        ਨਤੀਜਾ = Curl_get_ਸਤਰ(&linebuf, file, &eof);
        ਜੇ(!ਨਤੀਜਾ) {
          ਸਤਰ = curlx_dyn_ptr(&linebuf);
          /* skip comments on load */
          curlx_str_passblanks(&ਸਤਰ);
          ਜੇ(*ਸਤਰ == '#')
            ਜਾਰੀ;
          ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(filebuf, ਸਤਰ);
        }
        ਜੇ(ਨਤੀਜਾ) {
          curlx_dyn_free(filebuf);
          ret = curl2netrc(ਨਤੀਜਾ);
          ਰੋਕੋ;
        }
      } ਜਦੋਂ(!eof);
      curlx_dyn_free(&linebuf);
    }
    curlx_fclose(file);
  }
  ਵਾਪਸ ret;
}

ਸਥਿਰ NETRCcode netrc_ਜਾਂਚ_file(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ store_netrc *store,
                                 ਅਚਲ ਅੱਖਰ *hostname,
                                 ਅਚਲ ਅੱਖਰ *user,
                                 ਅਚਲ ਅੱਖਰ *netrcfile,
                                 ਬਣਤਰ Curl_creds **pcreds)
{
  ਬਣਤਰ dynbuf *filebuf = &store->filebuf;

  ਜੇ(!store->loaded || strcmp(netrcfile, store->filename)) {
    NETRCcode ret;
    store->loaded = FALSE;
    ret = file2memory(netrcfile, filebuf);
    ਜੇ(ret) {
      CURL_TRC_M(ਅੰਕੜੇ, "[NETRC] could not load '%s'", netrcfile);
      ਵਾਪਸ ret;
    }
    curlx_free(store->filename);
    store->filename = curlx_strdup(netrcfile);
    ਜੇ(!store->filename) {
      curlx_dyn_reset(&store->filebuf);
      ਵਾਪਸ NETRC_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    store->loaded = TRUE;
  }
  ਜੇ(!curlx_dyn_len(filebuf))
    ਵਾਪਸ NETRC_NO_ਮੇਲ;

  ਵਾਪਸ netrc_ਜਾਂਚ(ਅੰਕੜੇ, curlx_dyn_ptr(filebuf), hostname, user, pcreds);
}

/*
 * @unittest: 1304
 *
 * *loginp and *passwordp MUST be allocated if they are not NULL when passed
 * in.
 */
NETRCcode Curl_netrc_ਜਾਂਚ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ store_netrc *store,
                          ਅਚਲ ਅੱਖਰ *hostname,
                          ਅਚਲ ਅੱਖਰ *user,
                          ਅਚਲ ਅੱਖਰ *netrcfile,
                          ਬਣਤਰ Curl_creds **pcreds)
{
  NETRCcode retcode = NETRC_OK;

  CURL_TRC_M(ਅੰਕੜੇ, "[NETRC] scanning '%s' for host '%s' user '%s'",
             netrcfile, hostname, user);
  Curl_creds_unlink(pcreds);
  ਜੇ(!netrcfile) {
    ਅੱਖਰ *home = ਨਿਰਾ;
    ਅੱਖਰ *homea = ਨਿਰਾ;
    ਅੱਖਰ *filealloc = ਨਿਰਾ;
#ਜੇ defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
    ਅੱਖਰ pwbuf[1024];
#ਜੇ_ਅੰਤ
    filealloc = curl_getenv("NETRC");
    ਜੇ(!filealloc) {
      homea = curl_getenv("HOME"); /* portable environment reader */
      ਜੇ(homea) {
        home = homea;
#ਜੇ defined(HAVE_GETPWUID_R) && defined(HAVE_GETEUID)
      }
      ਨਹੀਂਤਾਂ {
        ਬਣਤਰ passwd pw, *pw_res;
        ਜੇ(!getpwuid_r(geteuid(), &pw, pwbuf, ਆਕਾਰ(pwbuf), &pw_res) &&
           pw_res) {
          home = pw.pw_dir;
        }
#ਜਾਂਜੇ defined(HAVE_GETPWUID) && defined(HAVE_GETEUID)
      }
      ਨਹੀਂਤਾਂ {
        ਬਣਤਰ passwd *pw;
        pw = getpwuid(geteuid());
        ਜੇ(pw) {
          home = pw->pw_dir;
        }
#ਜਾਂਜੇ defined(_WIN32)
      }
      ਨਹੀਂਤਾਂ {
        homea = curl_getenv("USERPROFILE");
        ਜੇ(homea) {
          home = homea;
        }
#ਜੇ_ਅੰਤ
      }

      ਜੇ(!home)
        ਵਾਪਸ NETRC_FILE_MISSING; /* no home directory found (or possibly out
                                      of memory) */

      filealloc = curl_maprintf("%s%s.netrc", home, DIR_CHAR);
      ਜੇ(!filealloc) {
        curlx_free(homea);
        retcode = NETRC_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
    retcode = netrc_ਜਾਂਚ_file(
      ਅੰਕੜੇ, store, hostname, user, filealloc, pcreds);
    curlx_free(filealloc);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
    ਜੇ(retcode == NETRC_FILE_MISSING) {
      /* fallback to the old-style "_netrc" file */
      filealloc = curl_maprintf("%s%s_netrc", home, DIR_CHAR);
      ਜੇ(!filealloc) {
        curlx_free(homea);
        ਵਾਪਸ NETRC_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
      retcode = netrc_ਜਾਂਚ_file(
        ਅੰਕੜੇ, store, hostname, user, filealloc, pcreds);
      curlx_free(filealloc);
    }
#ਜੇ_ਅੰਤ
    curlx_free(homea);
  }
  ਨਹੀਂਤਾਂ
    retcode = netrc_ਜਾਂਚ_file(
      ਅੰਕੜੇ, store, hostname, user, netrcfile, pcreds);

ਬਾਹਰਲਾ:
  ਜੇ(retcode)
    Curl_creds_unlink(pcreds);
  ਵਾਪਸ retcode;
}

ਖਾਲੀ Curl_netrc_init(ਬਣਤਰ store_netrc *store)
{
  curlx_dyn_init(&store->filebuf, MAX_NETRC_FILE);
  store->loaded = FALSE;
  store->filename = ਨਿਰਾ;
}
ਖਾਲੀ Curl_netrc_cleanup(ਬਣਤਰ store_netrc *store)
{
  curlx_dyn_free(&store->filebuf);
  curlx_safefree(store->filename);
  store->loaded = FALSE;
}

ਅਚਲ ਅੱਖਰ *Curl_netrc_strerror(NETRCcode ret)
{
  ਚੋਣ(ret) {
  ਮੂਲ:
    ਵਾਪਸ ""; /* not a legit error */
  ਹਾਲਤ NETRC_FILE_MISSING:
    ਵਾਪਸ "no such file";
  ਹਾਲਤ NETRC_NO_ਮੇਲ:
    ਵਾਪਸ "no matching entry";
  ਹਾਲਤ NETRC_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ:
    ਵਾਪਸ "out of memory";
  ਹਾਲਤ NETRC_SYNTAX_ERROR:
    ਵਾਪਸ "syntax error";
  }
  /* never reached */
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_NETRC */
