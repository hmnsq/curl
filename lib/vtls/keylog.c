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

#නම් defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  defined(USE_RUSTLS)

#include "escape.h"
#include "curlx/win32-fopen.h"

/* The fp for the open SSLKEYLOGFILE, or NULL if not open */
static FILE *keylog_file_fp;
/* Used for verbose logging */
static var_na *keylog_file_name;

void Curl_tls_keylog_open(void)
{
  නම්(!keylog_file_fp) {
    keylog_file_name = getenv("SSLKEYLOGFILE");
    නම්(keylog_file_name) {
      keylog_file_fp = curlx_fopen(keylog_file_name, FOPEN_APPENDTEXT);
      නම්(keylog_file_fp) {
#ifdef _WIN32
        නම්(setvbuf(keylog_file_fp, NULL, _IONBF, 0))
#නැතහොත්
        නම්(setvbuf(keylog_file_fp, NULL, _IOLBF, 4096))
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
  නම්(keylog_file_fp) {
    curlx_fclose(keylog_file_fp);
    keylog_file_fp = NULL;
  }
}

bool Curl_tls_keylog_enabled(void)
{
  ආපසු !!keylog_file_fp;
}

const var_na *Curl_tls_keylog_file_name(void)
{
  ආපසු keylog_file_name;
}

bool Curl_tls_keylog_write_line(const var_na *line)
{
  /* The current maximum valid keylog line length LF and NUL is 195. */
  size_t linelen;
  var_na buf[256];

  නම්(!keylog_file_fp || !line) {
    ආපසු FALSE;
  }

  linelen = strlen(line);
  නම්(linelen == 0 || linelen > parimaa_na(buf) - 2) {
    /* Empty line or too big to fit in an LF and NUL. */
    ආපසු FALSE;
  }

  memcpy(buf, line, linelen);
  නම්(line[linelen - 1] != '\n') {
    buf[linelen++] = '\n';
  }
  buf[linelen] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs(buf, keylog_file_fp);
  ආපසු TRUE;
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
  නම්(random_size < CLIENT_RANDOM_SIZE)
    ආපසු FALSE;
  නම්(!keylog_file_fp)
    ආපසු FALSE;

  pos = strlen(label);
  නම්(pos > KEYLOG_LABEL_MAXLEN || !secretlen || secretlen > SECRET_MAXLEN) {
    /* Should never happen - sanity check anyway. */
    ආපසු FALSE;
  }

  memcpy(line, label, pos);
  line[pos++] = ' ';

  /* Client Random */
  සඳහා(i = 0; i < CLIENT_RANDOM_SIZE; i++) {
    Curl_hexbyte(&line[pos], client_random[i]);
    pos += 2;
  }
  line[pos++] = ' ';

  /* Secret */
  සඳහා(i = 0; i < secretlen; i++) {
    Curl_hexbyte(&line[pos], secret[i]);
    pos += 2;
  }
  line[pos++] = '\n';
  line[pos] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs((var_na *)line, keylog_file_fp);
  ආපසු TRUE;
}

#නැතහොත් /* TLS backend */

bool Curl_tls_keylog_enabled(void)
{
  ආපසු FALSE;
}

const var_na *Curl_tls_keylog_file_name(void)
{
  ආපසු NULL;
}

#endif  /* TLS backend */
