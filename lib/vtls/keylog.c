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
#include "curl_setup.h"
#include "vtls/keylog.h"

#만약 defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  defined(USE_RUSTLS)

#include "escape.h"
#include "curlx/win32-fopen.h"

/* The fp for the open SSLKEYLOGFILE, or NULL if not open */
static FILE *keylog_file_fp;
/* Used for verbose logging */
static char *keylog_file_name;

void Curl_tls_keylog_open(void)
{
  만약(!keylog_file_fp) {
    keylog_file_name = getenv("SSLKEYLOGFILE");
    만약(keylog_file_name) {
      keylog_file_fp = curlx_fopen(keylog_file_name, FOPEN_APPENDTEXT);
      만약(keylog_file_fp) {
#ifdef _WIN32
        만약(setvbuf(keylog_file_fp, NULL, _IONBF, 0))
#아니면
        만약(setvbuf(keylog_file_fp, NULL, _IOLBF, 4096))
#endif
        {
          curlx_fclose(keylog_file_fp);
          keylog_file_fp = NULL;
        }
      }
    }
  }
}

void Curl_tls_keylog_close(void)
{
  만약(keylog_file_fp) {
    curlx_fclose(keylog_file_fp);
    keylog_file_fp = NULL;
  }
}

bool Curl_tls_keylog_enabled(void)
{
  반환 !!keylog_file_fp;
}

const char *Curl_tls_keylog_file_name(void)
{
  반환 keylog_file_name;
}

bool Curl_tls_keylog_write_line(const char *line)
{
  /* The current maximum valid keylog line length LF and NUL is 195. */
  size_t linelen;
  char buf[256];

  만약(!keylog_file_fp || !line) {
    반환 FALSE;
  }

  linelen = strlen(line);
  만약(linelen == 0 || linelen > sizeof(buf) - 2) {
    /* Empty line or too big to fit in an LF and NUL. */
    반환 FALSE;
  }

  memcpy(buf, line, linelen);
  만약(line[linelen - 1] != '\n') {
    buf[linelen++] = '\n';
  }
  buf[linelen] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs(buf, keylog_file_fp);
  반환 TRUE;
}

bool Curl_tls_keylog_write(const char *label,
                           const unsigned char *client_random,
                           size_t random_size,
                           const unsigned char *secret, size_t secretlen)
{
  size_t pos, i;
  unsigned char line[KEYLOG_LABEL_MAXLEN + 1 +
                     (2 * CLIENT_RANDOM_SIZE) + 1 +
                     (2 * SECRET_MAXLEN) + 1 + 1];
  DEBUGASSERT(random_size >= CLIENT_RANDOM_SIZE);
  만약(random_size < CLIENT_RANDOM_SIZE)
    반환 FALSE;
  만약(!keylog_file_fp)
    반환 FALSE;

  pos = strlen(label);
  만약(pos > KEYLOG_LABEL_MAXLEN || !secretlen || secretlen > SECRET_MAXLEN) {
    /* Should never happen - sanity check anyway. */
    반환 FALSE;
  }

  memcpy(line, label, pos);
  line[pos++] = ' ';

  /* Client Random */
  위해(i = 0; i < CLIENT_RANDOM_SIZE; i++) {
    Curl_hexbyte(&line[pos], client_random[i]);
    pos += 2;
  }
  line[pos++] = ' ';

  /* Secret */
  위해(i = 0; i < secretlen; i++) {
    Curl_hexbyte(&line[pos], secret[i]);
    pos += 2;
  }
  line[pos++] = '\n';
  line[pos] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs((char *)line, keylog_file_fp);
  반환 TRUE;
}

#아니면 /* TLS backend */

bool Curl_tls_keylog_enabled(void)
{
  반환 FALSE;
}

const char *Curl_tls_keylog_file_name(void)
{
  반환 NULL;
}

#endif  /* TLS backend */
