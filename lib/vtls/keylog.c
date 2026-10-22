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
#അന്തര്ഭാവ "curl_setup.h"
#അന്തര്ഭാവ "vtls/keylog.h"

#എങ്കിൽ defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  defined(USE_RUSTLS)

#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "curlx/win32-fopen.h"

/* The fp for the open SSLKEYLOGFILE, or NULL if not open */
സ്ഥിതം FILE *keylog_file_fp;
/* Used for verbose logging */
സ്ഥിതം അക്ഷരം *keylog_file_name;

ശൂന്യം Curl_tls_keylog_open(ശൂന്യം)
{
  എങ്കിൽ(!keylog_file_fp) {
    keylog_file_name = getenv("SSLKEYLOGFILE");
    എങ്കിൽ(keylog_file_name) {
      keylog_file_fp = curlx_fopen(keylog_file_name, FOPEN_APPENDTEXT);
      എങ്കിൽ(keylog_file_fp) {
#ifdef _WIN32
        എങ്കിൽ(setvbuf(keylog_file_fp, ശൂന്യ, _IONBF, 0))
#അല്ലെങ്കിൽ
        എങ്കിൽ(setvbuf(keylog_file_fp, ശൂന്യ, _IOLBF, 4096))
#endif
        {
          curlx_fclose(keylog_file_fp);
          keylog_file_fp = ശൂന്യ;
        }
      }
    }
  }
}

ശൂന്യം Curl_tls_keylog_close(ശൂന്യം)
{
  എങ്കിൽ(keylog_file_fp) {
    curlx_fclose(keylog_file_fp);
    keylog_file_fp = ശൂന്യ;
  }
}

bool Curl_tls_keylog_enabled(ശൂന്യം)
{
  തിരികെ !!keylog_file_fp;
}

സ്ഥിരം അക്ഷരം *Curl_tls_keylog_file_name(ശൂന്യം)
{
  തിരികെ keylog_file_name;
}

bool Curl_tls_keylog_write_വരി(സ്ഥിരം അക്ഷരം *വരി)
{
  /* The current maximum valid keylog line length LF and NUL is 195. */
  size_t linelen;
  അക്ഷരം buf[256];

  എങ്കിൽ(!keylog_file_fp || !വരി) {
    തിരികെ FALSE;
  }

  linelen = strlen(വരി);
  എങ്കിൽ(linelen == 0 || linelen > വലുപ്പം(buf) - 2) {
    /* Empty line or too big to fit in an LF and NUL. */
    തിരികെ FALSE;
  }

  memcpy(buf, വരി, linelen);
  എങ്കിൽ(വരി[linelen - 1] != '\n') {
    buf[linelen++] = '\n';
  }
  buf[linelen] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs(buf, keylog_file_fp);
  തിരികെ TRUE;
}

bool Curl_tls_keylog_write(സ്ഥിരം അക്ഷരം *label,
                           സ്ഥിരം unsigned അക്ഷരം *client_random,
                           size_t random_size,
                           സ്ഥിരം unsigned അക്ഷരം *secret, size_t secretlen)
{
  size_t pos, i;
  unsigned അക്ഷരം വരി[KEYLOG_LABEL_MAXLEN + 1 +
                     (2 * CLIENT_RANDOM_SIZE) + 1 +
                     (2 * SECRET_MAXLEN) + 1 + 1];
  DEBUGASSERT(random_size >= CLIENT_RANDOM_SIZE);
  എങ്കിൽ(random_size < CLIENT_RANDOM_SIZE)
    തിരികെ FALSE;
  എങ്കിൽ(!keylog_file_fp)
    തിരികെ FALSE;

  pos = strlen(label);
  എങ്കിൽ(pos > KEYLOG_LABEL_MAXLEN || !secretlen || secretlen > SECRET_MAXLEN) {
    /* Should never happen - sanity check anyway. */
    തിരികെ FALSE;
  }

  memcpy(വരി, label, pos);
  വരി[pos++] = ' ';

  /* Client Random */
  വേണ്ടി(i = 0; i < CLIENT_RANDOM_SIZE; i++) {
    Curl_hexbyte(&വരി[pos], client_random[i]);
    pos += 2;
  }
  വരി[pos++] = ' ';

  /* Secret */
  വേണ്ടി(i = 0; i < secretlen; i++) {
    Curl_hexbyte(&വരി[pos], secret[i]);
    pos += 2;
  }
  വരി[pos++] = '\n';
  വരി[pos] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs((അക്ഷരം *)വരി, keylog_file_fp);
  തിരികെ TRUE;
}

#അല്ലെങ്കിൽ /* TLS backend */

bool Curl_tls_keylog_enabled(ശൂന്യം)
{
  തിരികെ FALSE;
}

സ്ഥിരം അക്ഷരം *Curl_tls_keylog_file_name(ശൂന്യം)
{
  തിരികെ ശൂന്യ;
}

#endif  /* TLS backend */
