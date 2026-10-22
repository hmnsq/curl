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
#ضمن "curl_setup.h"
#ضمن "vtls/keylog.h"

#ܐܢ defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  defined(USE_RUSTLS)

#ضمن "escape.h"
#ضمن "curlx/win32-fopen.h"

/* The fp for the open SSLKEYLOGFILE, or NULL if not open */
ساكن FILE *keylog_file_fp;
/* Used for verbose logging */
ساكن حرف *keylog_file_name;

فراغ Curl_tls_keylog_open(فراغ)
{
  ܐܢ(!keylog_file_fp) {
    keylog_file_name = getenv("SSLKEYLOGFILE");
    ܐܢ(keylog_file_name) {
      keylog_file_fp = curlx_fopen(keylog_file_name, FOPEN_APPENDTEXT);
      ܐܢ(keylog_file_fp) {
#ifdef _WIN32
        ܐܢ(setvbuf(keylog_file_fp, عدم, _IONBF, 0))
#ܐܚܪܢܐ
        ܐܢ(setvbuf(keylog_file_fp, عدم, _IOLBF, 4096))
#endif
        {
          curlx_fclose(keylog_file_fp);
          keylog_file_fp = عدم;
        }
      }
    }
  }
}

فراغ Curl_tls_keylog_close(فراغ)
{
  ܐܢ(keylog_file_fp) {
    curlx_fclose(keylog_file_fp);
    keylog_file_fp = عدم;
  }
}

bool Curl_tls_keylog_enabled(فراغ)
{
  ܦܢܐ !!keylog_file_fp;
}

ثابت حرف *Curl_tls_keylog_file_name(فراغ)
{
  ܦܢܐ keylog_file_name;
}

bool Curl_tls_keylog_write_line(ثابت حرف *line)
{
  /* The current maximum valid keylog line length LF and NUL is 195. */
  size_t linelen;
  حرف buf[256];

  ܐܢ(!keylog_file_fp || !line) {
    ܦܢܐ FALSE;
  }

  linelen = strlen(line);
  ܐܢ(linelen == 0 || linelen > حجم(buf) - 2) {
    /* Empty line or too big to fit in an LF and NUL. */
    ܦܢܐ FALSE;
  }

  memcpy(buf, line, linelen);
  ܐܢ(line[linelen - 1] != '\n') {
    buf[linelen++] = '\n';
  }
  buf[linelen] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs(buf, keylog_file_fp);
  ܦܢܐ TRUE;
}

bool Curl_tls_keylog_write(ثابت حرف *label,
                           ثابت unsigned حرف *client_random,
                           size_t random_size,
                           ثابت unsigned حرف *secret, size_t secretlen)
{
  size_t pos, i;
  unsigned حرف line[KEYLOG_LABEL_MAXLEN + 1 +
                     (2 * CLIENT_RANDOM_SIZE) + 1 +
                     (2 * SECRET_MAXLEN) + 1 + 1];
  DEBUGASSERT(random_size >= CLIENT_RANDOM_SIZE);
  ܐܢ(random_size < CLIENT_RANDOM_SIZE)
    ܦܢܐ FALSE;
  ܐܢ(!keylog_file_fp)
    ܦܢܐ FALSE;

  pos = strlen(label);
  ܐܢ(pos > KEYLOG_LABEL_MAXLEN || !secretlen || secretlen > SECRET_MAXLEN) {
    /* Should never happen - sanity check anyway. */
    ܦܢܐ FALSE;
  }

  memcpy(line, label, pos);
  line[pos++] = ' ';

  /* Client Random */
  لكل(i = 0; i < CLIENT_RANDOM_SIZE; i++) {
    Curl_hexbyte(&line[pos], client_random[i]);
    pos += 2;
  }
  line[pos++] = ' ';

  /* Secret */
  لكل(i = 0; i < secretlen; i++) {
    Curl_hexbyte(&line[pos], secret[i]);
    pos += 2;
  }
  line[pos++] = '\n';
  line[pos] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs((حرف *)line, keylog_file_fp);
  ܦܢܐ TRUE;
}

#ܐܚܪܢܐ /* TLS backend */

bool Curl_tls_keylog_enabled(فراغ)
{
  ܦܢܐ FALSE;
}

ثابت حرف *Curl_tls_keylog_file_name(فراغ)
{
  ܦܢܐ عدم;
}

#endif  /* TLS backend */
