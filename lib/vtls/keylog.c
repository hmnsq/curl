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
#شامل "curl_setup.h"
#شامل "vtls/keylog.h"

#اگر defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  defined(USE_RUSTLS)

#شامل "escape.h"
#شامل "curlx/win32-fopen.h"

/* The fp for the open SSLKEYLOGFILE, or NULL if not open */
ایستا FILE *keylog_file_fp;
/* Used for verbose logging */
ایستا نویسه *keylog_file_name;

تهی Curl_tls_keylog_open(تهی)
{
  اگر(!keylog_file_fp) {
    keylog_file_name = getenv("SSLKEYLOGFILE");
    اگر(keylog_file_name) {
      keylog_file_fp = curlx_fopen(keylog_file_name, FOPEN_APPENDTEXT);
      اگر(keylog_file_fp) {
#ifdef _WIN32
        اگر(setvbuf(keylog_file_fp, پوچ, _IONBF, 0))
#وگرنه
        اگر(setvbuf(keylog_file_fp, پوچ, _IOLBF, 4096))
#endif
        {
          curlx_fclose(keylog_file_fp);
          keylog_file_fp = پوچ;
        }
      }
    }
  }
}

تهی Curl_tls_keylog_close(تهی)
{
  اگر(keylog_file_fp) {
    curlx_fclose(keylog_file_fp);
    keylog_file_fp = پوچ;
  }
}

bool Curl_tls_keylog_enabled(تهی)
{
  بازگشت !!keylog_file_fp;
}

ثابت نویسه *Curl_tls_keylog_file_name(تهی)
{
  بازگشت keylog_file_name;
}

bool Curl_tls_keylog_write_سطر(ثابت نویسه *سطر)
{
  /* The current maximum valid keylog line length LF and NUL is 195. */
  size_t linelen;
  نویسه buf[256];

  اگر(!keylog_file_fp || !سطر) {
    بازگشت FALSE;
  }

  linelen = strlen(سطر);
  اگر(linelen == 0 || linelen > اندازه(buf) - 2) {
    /* Empty line or too big to fit in an LF and NUL. */
    بازگشت FALSE;
  }

  memcpy(buf, سطر, linelen);
  اگر(سطر[linelen - 1] != '\n') {
    buf[linelen++] = '\n';
  }
  buf[linelen] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs(buf, keylog_file_fp);
  بازگشت TRUE;
}

bool Curl_tls_keylog_write(ثابت نویسه *label,
                           ثابت unsigned نویسه *client_random,
                           size_t random_size,
                           ثابت unsigned نویسه *secret, size_t secretlen)
{
  size_t pos, i;
  unsigned نویسه سطر[KEYLOG_LABEL_MAXLEN + 1 +
                     (2 * CLIENT_RANDOM_SIZE) + 1 +
                     (2 * SECRET_MAXLEN) + 1 + 1];
  DEBUGASSERT(random_size >= CLIENT_RANDOM_SIZE);
  اگر(random_size < CLIENT_RANDOM_SIZE)
    بازگشت FALSE;
  اگر(!keylog_file_fp)
    بازگشت FALSE;

  pos = strlen(label);
  اگر(pos > KEYLOG_LABEL_MAXLEN || !secretlen || secretlen > SECRET_MAXLEN) {
    /* Should never happen - sanity check anyway. */
    بازگشت FALSE;
  }

  memcpy(سطر, label, pos);
  سطر[pos++] = ' ';

  /* Client Random */
  برای(i = 0; i < CLIENT_RANDOM_SIZE; i++) {
    Curl_hexbyte(&سطر[pos], client_random[i]);
    pos += 2;
  }
  سطر[pos++] = ' ';

  /* Secret */
  برای(i = 0; i < secretlen; i++) {
    Curl_hexbyte(&سطر[pos], secret[i]);
    pos += 2;
  }
  سطر[pos++] = '\n';
  سطر[pos] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs((نویسه *)سطر, keylog_file_fp);
  بازگشت TRUE;
}

#وگرنه /* TLS backend */

bool Curl_tls_keylog_enabled(تهی)
{
  بازگشت FALSE;
}

ثابت نویسه *Curl_tls_keylog_file_name(تهی)
{
  بازگشت پوچ;
}

#endif  /* TLS backend */
