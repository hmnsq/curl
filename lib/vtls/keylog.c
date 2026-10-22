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

#जर defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  defined(USE_RUSTLS)

#अन्तर्भाव "escape.h"
#अन्तर्भाव "curlx/win32-fopen.h"

/* The fp for the open SSLKEYLOGFILE, or NULL if not open */
स्थिर खाता *keylog_file_fp;
/* Used for verbose logging */
स्थिर अक्षर *keylog_file_name;

शून्य Curl_tls_keylog_open(शून्य)
{
  जर(!keylog_file_fp) {
    keylog_file_name = दो_पर्या("SSLKEYLOGFILE");
    जर(keylog_file_name) {
      keylog_file_fp = curlx_fopen(keylog_file_name, FOPEN_APPENDTEXT);
      जर(keylog_file_fp) {
#ifdef _WIN32
        जर(रखो_भबफ(keylog_file_fp, रिक्त, _IONBF, 0))
#अन्यथा
        जर(रखो_भबफ(keylog_file_fp, रिक्त, _IOLBF, 4096))
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
  जर(keylog_file_fp) {
    curlx_fclose(keylog_file_fp);
    keylog_file_fp = रिक्त;
  }
}

bool Curl_tls_keylog_enabled(शून्य)
{
  परत !!keylog_file_fp;
}

स्थिरांक अक्षर *Curl_tls_keylog_file_name(शून्य)
{
  परत keylog_file_name;
}

bool Curl_tls_keylog_write_ओळ(स्थिरांक अक्षर *ओळ)
{
  /* The current maximum valid keylog line length LF and NUL is 195. */
  माप_प्रकार linelen;
  अक्षर buf[256];

  जर(!keylog_file_fp || !ओळ) {
    परत FALSE;
  }

  linelen = म_माप(ओळ);
  जर(linelen == 0 || linelen > आकार(buf) - 2) {
    /* Empty line or too big to fit in an LF and NUL. */
    परत FALSE;
  }

  स_नकल(buf, ओळ, linelen);
  जर(ओळ[linelen - 1] != '\n') {
    buf[linelen++] = '\n';
  }
  buf[linelen] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  ख_माला_दो(buf, keylog_file_fp);
  परत TRUE;
}

bool Curl_tls_keylog_write(स्थिरांक अक्षर *label,
                           स्थिरांक अचिन्हित अक्षर *client_random,
                           माप_प्रकार random_size,
                           स्थिरांक अचिन्हित अक्षर *secret, माप_प्रकार secretlen)
{
  माप_प्रकार pos, i;
  अचिन्हित अक्षर ओळ[KEYLOG_LABEL_MAXLEN + 1 +
                     (2 * CLIENT_RANDOM_SIZE) + 1 +
                     (2 * SECRET_MAXLEN) + 1 + 1];
  DEBUGASSERT(random_size >= CLIENT_RANDOM_SIZE);
  जर(random_size < CLIENT_RANDOM_SIZE)
    परत FALSE;
  जर(!keylog_file_fp)
    परत FALSE;

  pos = म_माप(label);
  जर(pos > KEYLOG_LABEL_MAXLEN || !secretlen || secretlen > SECRET_MAXLEN) {
    /* Should never happen - sanity check anyway. */
    परत FALSE;
  }

  स_नकल(ओळ, label, pos);
  ओळ[pos++] = ' ';

  /* Client Random */
  साठी(i = 0; i < CLIENT_RANDOM_SIZE; i++) {
    Curl_hexbyte(&ओळ[pos], client_random[i]);
    pos += 2;
  }
  ओळ[pos++] = ' ';

  /* Secret */
  साठी(i = 0; i < secretlen; i++) {
    Curl_hexbyte(&ओळ[pos], secret[i]);
    pos += 2;
  }
  ओळ[pos++] = '\n';
  ओळ[pos] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  ख_माला_दो((अक्षर *)ओळ, keylog_file_fp);
  परत TRUE;
}

#अन्यथा /* TLS backend */

bool Curl_tls_keylog_enabled(शून्य)
{
  परत FALSE;
}

स्थिरांक अक्षर *Curl_tls_keylog_file_name(शून्य)
{
  परत रिक्त;
}

#endif  /* TLS backend */
