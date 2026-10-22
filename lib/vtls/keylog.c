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
#समावेश "curl_setup.h"
#समावेश "vtls/keylog.h"

#अगर defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  defined(USE_RUSTLS)

#समावेश "escape.h"
#समावेश "curlx/win32-fopen.h"

/* The fp for the open SSLKEYLOGFILE, or NULL if not open */
अटल खाता *keylog_file_fp;
/* Used for verbose logging */
अटल अक्षर *keylog_file_name;

व्योम Curl_tls_keylog_open(व्योम)
{
  अगर(!keylog_file_fp) {
    keylog_file_name = दो_पर्या("SSLKEYLOGFILE");
    अगर(keylog_file_name) {
      keylog_file_fp = curlx_fopen(keylog_file_name, FOPEN_APPENDTEXT);
      अगर(keylog_file_fp) {
#अगर_घोषित _WIN32
        अगर(रखो_भबफ(keylog_file_fp, शून्य, _IONBF, 0))
#अन्यथा
        अगर(रखो_भबफ(keylog_file_fp, शून्य, _IOLBF, 4096))
#पूर्ण_अगर
        {
          curlx_fclose(keylog_file_fp);
          keylog_file_fp = शून्य;
        }
      }
    }
  }
}

व्योम Curl_tls_keylog_close(व्योम)
{
  अगर(keylog_file_fp) {
    curlx_fclose(keylog_file_fp);
    keylog_file_fp = शून्य;
  }
}

bool Curl_tls_keylog_enabled(व्योम)
{
  वापस !!keylog_file_fp;
}

स्थिर अक्षर *Curl_tls_keylog_file_name(व्योम)
{
  वापस keylog_file_name;
}

bool Curl_tls_keylog_write_पंक्ति(स्थिर अक्षर *पंक्ति)
{
  /* The current maximum valid keylog line length LF and NUL is 195. */
  माप_प्रकार linelen;
  अक्षर buf[256];

  अगर(!keylog_file_fp || !पंक्ति) {
    वापस FALSE;
  }

  linelen = म_माप(पंक्ति);
  अगर(linelen == 0 || linelen > माप(buf) - 2) {
    /* Empty line or too big to fit in an LF and NUL. */
    वापस FALSE;
  }

  स_नकल(buf, पंक्ति, linelen);
  अगर(पंक्ति[linelen - 1] != '\n') {
    buf[linelen++] = '\n';
  }
  buf[linelen] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  ख_माला_दो(buf, keylog_file_fp);
  वापस TRUE;
}

bool Curl_tls_keylog_write(स्थिर अक्षर *label,
                           स्थिर अचिन्हित अक्षर *client_random,
                           माप_प्रकार random_आकार,
                           स्थिर अचिन्हित अक्षर *secret, माप_प्रकार secretlen)
{
  माप_प्रकार pos, i;
  अचिन्हित अक्षर पंक्ति[KEYLOG_LABEL_MAXLEN + 1 +
                     (2 * CLIENT_RANDOM_आकार) + 1 +
                     (2 * SECRET_MAXLEN) + 1 + 1];
  DEBUGASSERT(random_आकार >= CLIENT_RANDOM_आकार);
  अगर(random_आकार < CLIENT_RANDOM_आकार)
    वापस FALSE;
  अगर(!keylog_file_fp)
    वापस FALSE;

  pos = म_माप(label);
  अगर(pos > KEYLOG_LABEL_MAXLEN || !secretlen || secretlen > SECRET_MAXLEN) {
    /* Should never happen - sanity check anyway. */
    वापस FALSE;
  }

  स_नकल(पंक्ति, label, pos);
  पंक्ति[pos++] = ' ';

  /* Client Random */
  क्रम(i = 0; i < CLIENT_RANDOM_आकार; i++) {
    Curl_hexbyte(&पंक्ति[pos], client_random[i]);
    pos += 2;
  }
  पंक्ति[pos++] = ' ';

  /* Secret */
  क्रम(i = 0; i < secretlen; i++) {
    Curl_hexbyte(&पंक्ति[pos], secret[i]);
    pos += 2;
  }
  पंक्ति[pos++] = '\n';
  पंक्ति[pos] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  ख_माला_दो((अक्षर *)पंक्ति, keylog_file_fp);
  वापस TRUE;
}

#अन्यथा /* TLS backend */

bool Curl_tls_keylog_enabled(व्योम)
{
  वापस FALSE;
}

स्थिर अक्षर *Curl_tls_keylog_file_name(व्योम)
{
  वापस शून्य;
}

#पूर्ण_अगर  /* TLS backend */
