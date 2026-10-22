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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"
#ಅನ್ತರ್ಭಾವ "vtls/keylog.h"

#ಯದಿ defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  defined(USE_RUSTLS)

#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"

/* The fp for the open SSLKEYLOGFILE, or NULL if not open */
ಸ್ಥಿರ FILE *keylog_file_fp;
/* Used for verbose logging */
ಸ್ಥಿರ ವರ್ಣ *keylog_file_name;

ಶೂನ್ಯ Curl_tls_keylog_open(ಶೂನ್ಯ)
{
  ಯದಿ(!keylog_file_fp) {
    keylog_file_name = getenv("SSLKEYLOGFILE");
    ಯದಿ(keylog_file_name) {
      keylog_file_fp = curlx_fopen(keylog_file_name, FOPEN_APPENDTEXT);
      ಯದಿ(keylog_file_fp) {
#ifdef _WIN32
        ಯದಿ(setvbuf(keylog_file_fp, ರಿಕ್ತ, _IONBF, 0))
#ಅನ್ಯಥಾ
        ಯದಿ(setvbuf(keylog_file_fp, ರಿಕ್ತ, _IOLBF, 4096))
#endif
        {
          curlx_fclose(keylog_file_fp);
          keylog_file_fp = ರಿಕ್ತ;
        }
      }
    }
  }
}

ಶೂನ್ಯ Curl_tls_keylog_close(ಶೂನ್ಯ)
{
  ಯದಿ(keylog_file_fp) {
    curlx_fclose(keylog_file_fp);
    keylog_file_fp = ರಿಕ್ತ;
  }
}

bool Curl_tls_keylog_enabled(ಶೂನ್ಯ)
{
  ಪ್ರತ್ಯಾಗಮ !!keylog_file_fp;
}

ಧ್ರುವ ವರ್ಣ *Curl_tls_keylog_file_name(ಶೂನ್ಯ)
{
  ಪ್ರತ್ಯಾಗಮ keylog_file_name;
}

bool Curl_tls_keylog_write_ಸಾಲು(ಧ್ರುವ ವರ್ಣ *ಸಾಲು)
{
  /* The current maximum valid keylog line length LF and NUL is 195. */
  size_t linelen;
  ವರ್ಣ buf[256];

  ಯದಿ(!keylog_file_fp || !ಸಾಲು) {
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }

  linelen = strlen(ಸಾಲು);
  ಯದಿ(linelen == 0 || linelen > ಪರಿಮಾಣ(buf) - 2) {
    /* Empty line or too big to fit in an LF and NUL. */
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }

  memcpy(buf, ಸಾಲು, linelen);
  ಯದಿ(ಸಾಲು[linelen - 1] != '\n') {
    buf[linelen++] = '\n';
  }
  buf[linelen] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs(buf, keylog_file_fp);
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

bool Curl_tls_keylog_write(ಧ್ರುವ ವರ್ಣ *label,
                           ಧ್ರುವ unsigned ವರ್ಣ *client_random,
                           size_t random_size,
                           ಧ್ರುವ unsigned ವರ್ಣ *secret, size_t secretlen)
{
  size_t pos, i;
  unsigned ವರ್ಣ ಸಾಲು[KEYLOG_LABEL_MAXLEN + 1 +
                     (2 * CLIENT_RANDOM_SIZE) + 1 +
                     (2 * SECRET_MAXLEN) + 1 + 1];
  DEBUGASSERT(random_size >= CLIENT_RANDOM_SIZE);
  ಯದಿ(random_size < CLIENT_RANDOM_SIZE)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಯದಿ(!keylog_file_fp)
    ಪ್ರತ್ಯಾಗಮ FALSE;

  pos = strlen(label);
  ಯದಿ(pos > KEYLOG_LABEL_MAXLEN || !secretlen || secretlen > SECRET_MAXLEN) {
    /* Should never happen - sanity check anyway. */
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }

  memcpy(ಸಾಲು, label, pos);
  ಸಾಲು[pos++] = ' ';

  /* Client Random */
  ಪ್ರತಿ(i = 0; i < CLIENT_RANDOM_SIZE; i++) {
    Curl_hexbyte(&ಸಾಲು[pos], client_random[i]);
    pos += 2;
  }
  ಸಾಲು[pos++] = ' ';

  /* Secret */
  ಪ್ರತಿ(i = 0; i < secretlen; i++) {
    Curl_hexbyte(&ಸಾಲು[pos], secret[i]);
    pos += 2;
  }
  ಸಾಲು[pos++] = '\n';
  ಸಾಲು[pos] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs((ವರ್ಣ *)ಸಾಲು, keylog_file_fp);
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

#ಅನ್ಯಥಾ /* TLS backend */

bool Curl_tls_keylog_enabled(ಶೂನ್ಯ)
{
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಧ್ರುವ ವರ್ಣ *Curl_tls_keylog_file_name(ಶೂನ್ಯ)
{
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#endif  /* TLS backend */
