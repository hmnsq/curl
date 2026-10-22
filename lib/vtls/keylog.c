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
ساکن FILE *keylog_file_fp;
/* Used for verbose logging */
ساکن حرف *keylog_file_name;

خالی Curl_tls_keylog_open(خالی)
{
  اگر(!keylog_file_fp) {
    keylog_file_name = getenv("SSLKEYLOGFILE");
    اگر(keylog_file_name) {
      keylog_file_fp = curlx_fopen(keylog_file_name, FOPEN_APPENDTEXT);
      اگر(keylog_file_fp) {
#اگر_تعریف_شدہ _WIN32
        اگر(setvbuf(keylog_file_fp, تہی, _IONBF, 0))
#ورنہ
        اگر(setvbuf(keylog_file_fp, تہی, _IOLBF, 4096))
#اگر_ختم
        {
          curlx_fclose(keylog_file_fp);
          keylog_file_fp = تہی;
        }
      }
    }
  }
}

خالی Curl_tls_keylog_close(خالی)
{
  اگر(keylog_file_fp) {
    curlx_fclose(keylog_file_fp);
    keylog_file_fp = تہی;
  }
}

bool Curl_tls_keylog_enabled(خالی)
{
  واپس !!keylog_file_fp;
}

ثابت حرف *Curl_tls_keylog_file_name(خالی)
{
  واپس keylog_file_name;
}

bool Curl_tls_keylog_write_سطر(ثابت حرف *سطر)
{
  /* The current maximum valid keylog line length LF and NUL is 195. */
  size_t linelen;
  حرف buf[256];

  اگر(!keylog_file_fp || !سطر) {
    واپس FALSE;
  }

  linelen = لڑی_لمبائی(سطر);
  اگر(linelen == 0 || linelen > حجم(buf) - 2) {
    /* Empty line or too big to fit in an LF and NUL. */
    واپس FALSE;
  }

  حافظہ_نقل(buf, سطر, linelen);
  اگر(سطر[linelen - 1] != '\n') {
    buf[linelen++] = '\n';
  }
  buf[linelen] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs(buf, keylog_file_fp);
  واپس TRUE;
}

bool Curl_tls_keylog_write(ثابت حرف *label,
                           ثابت غیرعلامتی حرف *client_random,
                           size_t random_سائز,
                           ثابت غیرعلامتی حرف *secret, size_t secretlen)
{
  size_t pos, i;
  غیرعلامتی حرف سطر[KEYLOG_LABEL_MAXLEN + 1 +
                     (2 * CLIENT_RANDOM_سائز) + 1 +
                     (2 * SECRET_MAXLEN) + 1 + 1];
  DEBUGASSERT(random_سائز >= CLIENT_RANDOM_سائز);
  اگر(random_سائز < CLIENT_RANDOM_سائز)
    واپس FALSE;
  اگر(!keylog_file_fp)
    واپس FALSE;

  pos = لڑی_لمبائی(label);
  اگر(pos > KEYLOG_LABEL_MAXLEN || !secretlen || secretlen > SECRET_MAXLEN) {
    /* Should never happen - sanity check anyway. */
    واپس FALSE;
  }

  حافظہ_نقل(سطر, label, pos);
  سطر[pos++] = ' ';

  /* Client Random */
  برائے(i = 0; i < CLIENT_RANDOM_سائز; i++) {
    Curl_hexbyte(&سطر[pos], client_random[i]);
    pos += 2;
  }
  سطر[pos++] = ' ';

  /* Secret */
  برائے(i = 0; i < secretlen; i++) {
    Curl_hexbyte(&سطر[pos], secret[i]);
    pos += 2;
  }
  سطر[pos++] = '\n';
  سطر[pos] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs((حرف *)سطر, keylog_file_fp);
  واپس TRUE;
}

#ورنہ /* TLS backend */

bool Curl_tls_keylog_enabled(خالی)
{
  واپس FALSE;
}

ثابت حرف *Curl_tls_keylog_file_name(خالی)
{
  واپس تہی;
}

#اگر_ختم  /* TLS backend */
