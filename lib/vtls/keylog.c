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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "vtls/keylog.h"

#सचे defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  defined(USE_RUSTLS)

#अन्तर्भाव "escape.h"
#अन्तर्भाव "curlx/win32-fopen.h"

/* The fp for the open SSLKEYLOGFILE, or NULL if not open */
स्थिर FILE *keylog_file_fp;
/* Used for verbose logging */
स्थिर वर्ण *keylog_file_name;

शून्य Curl_tls_keylog_open(शून्य)
{
  सचे(!keylog_file_fp) {
    keylog_file_name = getenv("SSLKEYLOGFILE");
    सचे(keylog_file_name) {
      keylog_file_fp = curlx_fopen(keylog_file_name, FOPEN_APPENDTEXT);
      सचे(keylog_file_fp) {
#ifdef _WIN32
        सचे(setvbuf(keylog_file_fp, रिक्त, _IONBF, 0))
#अञ्ञथा
        सचे(setvbuf(keylog_file_fp, रिक्त, _IOLBF, 4096))
#endif
        {
          curlx_fclose(keylog_file_fp);
          keylog_file_fp = रिक्त;
        }
      }
    }
  }
}

शून्य Curl_tls_keylog_close(शून्य)
{
  सचे(keylog_file_fp) {
    curlx_fclose(keylog_file_fp);
    keylog_file_fp = रिक्त;
  }
}

bool Curl_tls_keylog_enabled(शून्य)
{
  पटिनिवत्त !!keylog_file_fp;
}

ध्रुव वर्ण *Curl_tls_keylog_file_name(शून्य)
{
  पटिनिवत्त keylog_file_name;
}

bool Curl_tls_keylog_write_पङ्क्तिः(ध्रुव वर्ण *पङ्क्तिः)
{
  /* The current maximum valid keylog line length LF and NUL is 195. */
  size_t linelen;
  वर्ण buf[256];

  सचे(!keylog_file_fp || !पङ्क्तिः) {
    पटिनिवत्त FALSE;
  }

  linelen = strlen(पङ्क्तिः);
  सचे(linelen == 0 || linelen > परिमाण(buf) - 2) {
    /* Empty line or too big to fit in an LF and NUL. */
    पटिनिवत्त FALSE;
  }

  memcpy(buf, पङ्क्तिः, linelen);
  सचे(पङ्क्तिः[linelen - 1] != '\n') {
    buf[linelen++] = '\n';
  }
  buf[linelen] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs(buf, keylog_file_fp);
  पटिनिवत्त TRUE;
}

bool Curl_tls_keylog_write(ध्रुव वर्ण *label,
                           ध्रुव unsigned वर्ण *client_random,
                           size_t random_size,
                           ध्रुव unsigned वर्ण *secret, size_t secretlen)
{
  size_t pos, i;
  unsigned वर्ण पङ्क्तिः[KEYLOG_LABEL_MAXLEN + 1 +
                     (2 * CLIENT_RANDOM_SIZE) + 1 +
                     (2 * SECRET_MAXLEN) + 1 + 1];
  DEBUGASSERT(random_size >= CLIENT_RANDOM_SIZE);
  सचे(random_size < CLIENT_RANDOM_SIZE)
    पटिनिवत्त FALSE;
  सचे(!keylog_file_fp)
    पटिनिवत्त FALSE;

  pos = strlen(label);
  सचे(pos > KEYLOG_LABEL_MAXLEN || !secretlen || secretlen > SECRET_MAXLEN) {
    /* Should never happen - sanity check anyway. */
    पटिनिवत्त FALSE;
  }

  memcpy(पङ्क्तिः, label, pos);
  पङ्क्तिः[pos++] = ' ';

  /* Client Random */
  अत्थाय(i = 0; i < CLIENT_RANDOM_SIZE; i++) {
    Curl_hexbyte(&पङ्क्तिः[pos], client_random[i]);
    pos += 2;
  }
  पङ्क्तिः[pos++] = ' ';

  /* Secret */
  अत्थाय(i = 0; i < secretlen; i++) {
    Curl_hexbyte(&पङ्क्तिः[pos], secret[i]);
    pos += 2;
  }
  पङ्क्तिः[pos++] = '\n';
  पङ्क्तिः[pos] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs((वर्ण *)पङ्क्तिः, keylog_file_fp);
  पटिनिवत्त TRUE;
}

#अञ्ञथा /* TLS backend */

bool Curl_tls_keylog_enabled(शून्य)
{
  पटिनिवत्त FALSE;
}

ध्रुव वर्ण *Curl_tls_keylog_file_name(शून्य)
{
  पटिनिवत्त रिक्त;
}

#endif  /* TLS backend */
