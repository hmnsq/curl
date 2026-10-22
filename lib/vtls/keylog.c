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
#அந்தர்பாவ "curl_setup.h"
#அந்தர்பாவ "vtls/keylog.h"

#எனில் defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  defined(USE_RUSTLS)

#அந்தர்பாவ "escape.h"
#அந்தர்பாவ "curlx/win32-fopen.h"

/* The fp for the open SSLKEYLOGFILE, or NULL if not open */
நிலையான FILE *keylog_file_fp;
/* Used for verbose logging */
நிலையான எழுத்து *keylog_file_name;

வெறுமை Curl_tls_keylog_open(வெறுமை)
{
  எனில்(!keylog_file_fp) {
    keylog_file_name = getenv("SSLKEYLOGFILE");
    எனில்(keylog_file_name) {
      keylog_file_fp = curlx_fopen(keylog_file_name, FOPEN_APPENDTEXT);
      எனில்(keylog_file_fp) {
#ifdef _WIN32
        எனில்(setvbuf(keylog_file_fp, வெற்று, _IONBF, 0))
#இல்லையெனில்
        எனில்(setvbuf(keylog_file_fp, வெற்று, _IOLBF, 4096))
#endif
        {
          curlx_fclose(keylog_file_fp);
          keylog_file_fp = வெற்று;
        }
      }
    }
  }
}

வெறுமை Curl_tls_keylog_close(வெறுமை)
{
  எனில்(keylog_file_fp) {
    curlx_fclose(keylog_file_fp);
    keylog_file_fp = வெற்று;
  }
}

bool Curl_tls_keylog_enabled(வெறுமை)
{
  திரும்பு !!keylog_file_fp;
}

மாறிலி எழுத்து *Curl_tls_keylog_file_name(வெறுமை)
{
  திரும்பு keylog_file_name;
}

bool Curl_tls_keylog_write_வரி(மாறிலி எழுத்து *வரி)
{
  /* The current maximum valid keylog line length LF and NUL is 195. */
  size_t linelen;
  எழுத்து buf[256];

  எனில்(!keylog_file_fp || !வரி) {
    திரும்பு FALSE;
  }

  linelen = strlen(வரி);
  எனில்(linelen == 0 || linelen > அளவு(buf) - 2) {
    /* Empty line or too big to fit in an LF and NUL. */
    திரும்பு FALSE;
  }

  memcpy(buf, வரி, linelen);
  எனில்(வரி[linelen - 1] != '\n') {
    buf[linelen++] = '\n';
  }
  buf[linelen] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs(buf, keylog_file_fp);
  திரும்பு TRUE;
}

bool Curl_tls_keylog_write(மாறிலி எழுத்து *label,
                           மாறிலி unsigned எழுத்து *client_random,
                           size_t random_size,
                           மாறிலி unsigned எழுத்து *secret, size_t secretlen)
{
  size_t pos, i;
  unsigned எழுத்து வரி[KEYLOG_LABEL_MAXLEN + 1 +
                     (2 * CLIENT_RANDOM_SIZE) + 1 +
                     (2 * SECRET_MAXLEN) + 1 + 1];
  DEBUGASSERT(random_size >= CLIENT_RANDOM_SIZE);
  எனில்(random_size < CLIENT_RANDOM_SIZE)
    திரும்பு FALSE;
  எனில்(!keylog_file_fp)
    திரும்பு FALSE;

  pos = strlen(label);
  எனில்(pos > KEYLOG_LABEL_MAXLEN || !secretlen || secretlen > SECRET_MAXLEN) {
    /* Should never happen - sanity check anyway. */
    திரும்பு FALSE;
  }

  memcpy(வரி, label, pos);
  வரி[pos++] = ' ';

  /* Client Random */
  க்கு(i = 0; i < CLIENT_RANDOM_SIZE; i++) {
    Curl_hexbyte(&வரி[pos], client_random[i]);
    pos += 2;
  }
  வரி[pos++] = ' ';

  /* Secret */
  க்கு(i = 0; i < secretlen; i++) {
    Curl_hexbyte(&வரி[pos], secret[i]);
    pos += 2;
  }
  வரி[pos++] = '\n';
  வரி[pos] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs((எழுத்து *)வரி, keylog_file_fp);
  திரும்பு TRUE;
}

#இல்லையெனில் /* TLS backend */

bool Curl_tls_keylog_enabled(வெறுமை)
{
  திரும்பு FALSE;
}

மாறிலி எழுத்து *Curl_tls_keylog_file_name(வெறுமை)
{
  திரும்பு வெற்று;
}

#endif  /* TLS backend */
