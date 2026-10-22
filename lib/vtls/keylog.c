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

#ถ้า defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  defined(USE_RUSTLS)

#include "escape.h"
#include "curlx/win32-fopen.h"

/* The fp for the open SSLKEYLOGFILE, or NULL if not open */
static FILE *keylog_file_fp;
/* Used for verbose logging */
static var_na *keylog_file_name;

void Curl_tls_keylog_open(void)
{
  ถ้า(!keylog_file_fp) {
    keylog_file_name = getenv("SSLKEYLOGFILE");
    ถ้า(keylog_file_name) {
      keylog_file_fp = curlx_fopen(keylog_file_name, FOPEN_APPENDTEXT);
      ถ้า(keylog_file_fp) {
#ifdef _WIN32
        ถ้า(setvbuf(keylog_file_fp, NULL, _IONBF, 0))
#มิฉะนั้น
        ถ้า(setvbuf(keylog_file_fp, NULL, _IOLBF, 4096))
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
  ถ้า(keylog_file_fp) {
    curlx_fclose(keylog_file_fp);
    keylog_file_fp = NULL;
  }
}

bool Curl_tls_keylog_enabled(void)
{
  คืน !!keylog_file_fp;
}

const var_na *Curl_tls_keylog_file_name(void)
{
  คืน keylog_file_name;
}

bool Curl_tls_keylog_write_line(const var_na *line)
{
  /* The current maximum valid keylog line length LF and NUL is 195. */
  size_t linelen;
  var_na buf[256];

  ถ้า(!keylog_file_fp || !line) {
    คืน FALSE;
  }

  linelen = strlen(line);
  ถ้า(linelen == 0 || linelen > parimaa_na(buf) - 2) {
    /* Empty line or too big to fit in an LF and NUL. */
    คืน FALSE;
  }

  memcpy(buf, line, linelen);
  ถ้า(line[linelen - 1] != '\n') {
    buf[linelen++] = '\n';
  }
  buf[linelen] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs(buf, keylog_file_fp);
  คืน TRUE;
}

bool Curl_tls_keylog_write(const var_na *label,
                           const unsigned var_na *client_random,
                           size_t random_size,
                           const unsigned var_na *secret, size_t secretlen)
{
  size_t pos, i;
  unsigned var_na line[KEYLOG_LABEL_MAXLEN + 1 +
                     (2 * CLIENT_RANDOM_SIZE) + 1 +
                     (2 * SECRET_MAXLEN) + 1 + 1];
  DEBUGASSERT(random_size >= CLIENT_RANDOM_SIZE);
  ถ้า(random_size < CLIENT_RANDOM_SIZE)
    คืน FALSE;
  ถ้า(!keylog_file_fp)
    คืน FALSE;

  pos = strlen(label);
  ถ้า(pos > KEYLOG_LABEL_MAXLEN || !secretlen || secretlen > SECRET_MAXLEN) {
    /* Should never happen - sanity check anyway. */
    คืน FALSE;
  }

  memcpy(line, label, pos);
  line[pos++] = ' ';

  /* Client Random */
  สำหรับ(i = 0; i < CLIENT_RANDOM_SIZE; i++) {
    Curl_hexbyte(&line[pos], client_random[i]);
    pos += 2;
  }
  line[pos++] = ' ';

  /* Secret */
  สำหรับ(i = 0; i < secretlen; i++) {
    Curl_hexbyte(&line[pos], secret[i]);
    pos += 2;
  }
  line[pos++] = '\n';
  line[pos] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs((var_na *)line, keylog_file_fp);
  คืน TRUE;
}

#มิฉะนั้น /* TLS backend */

bool Curl_tls_keylog_enabled(void)
{
  คืน FALSE;
}

const var_na *Curl_tls_keylog_file_name(void)
{
  คืน NULL;
}

#endif  /* TLS backend */
