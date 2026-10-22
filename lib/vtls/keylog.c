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

#ಆದರೆ defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  defined(USE_RUSTLS)

#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"

/* The fp for the open SSLKEYLOGFILE, or NULL if not open */
ಸ್ಥಿರ FILE *keylog_file_fp;
/* Used for verbose logging */
ಸ್ಥಿರ ಅಕ್ಷರ *keylog_file_name;

ಶೂನ್ಯ Curl_tls_keylog_open(ಶೂನ್ಯ)
{
  ಆದರೆ(!keylog_file_fp) {
    keylog_file_name = getenv("SSLKEYLOGFILE");
    ಆದರೆ(keylog_file_name) {
      keylog_file_fp = curlx_fopen(keylog_file_name, FOPEN_APPENDTEXT);
      ಆದರೆ(keylog_file_fp) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
        ಆದರೆ(setvbuf(keylog_file_fp, ಖಾಲಿ, _IONBF, 0))
#ಇಲ್ಲದಿದ್ದರೆ
        ಆದರೆ(setvbuf(keylog_file_fp, ಖಾಲಿ, _IOLBF, 4096))
#ಆದರೆ_ಅಂತ್ಯ
        {
          curlx_fclose(keylog_file_fp);
          keylog_file_fp = ಖಾಲಿ;
        }
      }
    }
  }
}

ಶೂನ್ಯ Curl_tls_keylog_close(ಶೂನ್ಯ)
{
  ಆದರೆ(keylog_file_fp) {
    curlx_fclose(keylog_file_fp);
    keylog_file_fp = ಖಾಲಿ;
  }
}

bool Curl_tls_keylog_enabled(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ !!keylog_file_fp;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_tls_keylog_file_name(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ keylog_file_name;
}

bool Curl_tls_keylog_write_ಸಾಲು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು)
{
  /* The current maximum valid keylog line length LF and NUL is 195. */
  size_t linelen;
  ಅಕ್ಷರ buf[256];

  ಆದರೆ(!keylog_file_fp || !ಸಾಲು) {
    ಹಿಂತಿರುಗಿ FALSE;
  }

  linelen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಸಾಲು);
  ಆದರೆ(linelen == 0 || linelen > ಗಾತ್ರ(buf) - 2) {
    /* Empty line or too big to fit in an LF and NUL. */
    ಹಿಂತಿರುಗಿ FALSE;
  }

  ಸ್ಮರಣೆ_ನಕಲು(buf, ಸಾಲು, linelen);
  ಆದರೆ(ಸಾಲು[linelen - 1] != '\n') {
    buf[linelen++] = '\n';
  }
  buf[linelen] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs(buf, keylog_file_fp);
  ಹಿಂತಿರುಗಿ TRUE;
}

bool Curl_tls_keylog_write(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label,
                           ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *client_random,
                           size_t random_ಗಾತ್ರ,
                           ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *secret, size_t secretlen)
{
  size_t pos, i;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಸಾಲು[KEYLOG_LABEL_MAXLEN + 1 +
                     (2 * CLIENT_RANDOM_ಗಾತ್ರ) + 1 +
                     (2 * SECRET_MAXLEN) + 1 + 1];
  DEBUGASSERT(random_ಗಾತ್ರ >= CLIENT_RANDOM_ಗಾತ್ರ);
  ಆದರೆ(random_ಗಾತ್ರ < CLIENT_RANDOM_ಗಾತ್ರ)
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ(!keylog_file_fp)
    ಹಿಂತಿರುಗಿ FALSE;

  pos = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(label);
  ಆದರೆ(pos > KEYLOG_LABEL_MAXLEN || !secretlen || secretlen > SECRET_MAXLEN) {
    /* Should never happen - sanity check anyway. */
    ಹಿಂತಿರುಗಿ FALSE;
  }

  ಸ್ಮರಣೆ_ನಕಲು(ಸಾಲು, label, pos);
  ಸಾಲು[pos++] = ' ';

  /* Client Random */
  ಗಾಗಿ(i = 0; i < CLIENT_RANDOM_ಗಾತ್ರ; i++) {
    Curl_hexbyte(&ಸಾಲು[pos], client_random[i]);
    pos += 2;
  }
  ಸಾಲು[pos++] = ' ';

  /* Secret */
  ಗಾಗಿ(i = 0; i < secretlen; i++) {
    Curl_hexbyte(&ಸಾಲು[pos], secret[i]);
    pos += 2;
  }
  ಸಾಲು[pos++] = '\n';
  ಸಾಲು[pos] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs((ಅಕ್ಷರ *)ಸಾಲು, keylog_file_fp);
  ಹಿಂತಿರುಗಿ TRUE;
}

#ಇಲ್ಲದಿದ್ದರೆ /* TLS backend */

bool Curl_tls_keylog_enabled(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_tls_keylog_file_name(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ_ಅಂತ್ಯ  /* TLS backend */
