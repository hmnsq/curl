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

#यदि defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  defined(USE_RUSTLS)

#अन्तर्भाव "escape.h"
#अन्तर्भाव "curlx/win32-fopen.h"

/* The fp for the open SSLKEYLOGFILE, or NULL if not open */
स्थिर खाता *keylog_file_fp;
/* Used for verbose logging */
स्थिर वर्ण *keylog_file_name;

शून्य Curl_tls_keylog_open(शून्य)
{
  यदि(!keylog_file_fp) {
    keylog_file_name = दो_पर्या("SSLKEYLOGFILE");
    यदि(keylog_file_name) {
      keylog_file_fp = curlx_fopen(keylog_file_name, FOPEN_APPENDTEXT);
      यदि(keylog_file_fp) {
#ifdef _WIN32
        यदि(रखो_भबफ(keylog_file_fp, रिक्त, _IONBF, 0))
#अन्यथा
        यदि(रखो_भबफ(keylog_file_fp, रिक्त, _IOLBF, 4096))
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
  यदि(keylog_file_fp) {
    curlx_fclose(keylog_file_fp);
    keylog_file_fp = रिक्त;
  }
}

bool Curl_tls_keylog_enabled(शून्य)
{
  फर्काऊ !!keylog_file_fp;
}

अचर वर्ण *Curl_tls_keylog_file_name(शून्य)
{
  फर्काऊ keylog_file_name;
}

bool Curl_tls_keylog_write_पङ्क्ति(अचर वर्ण *पङ्क्ति)
{
  /* The current maximum valid keylog line length LF and NUL is 195. */
  माप_प्रकार linelen;
  वर्ण buf[256];

  यदि(!keylog_file_fp || !पङ्क्ति) {
    फर्काऊ FALSE;
  }

  linelen = म_माप(पङ्क्ति);
  यदि(linelen == 0 || linelen > परिमाण(buf) - 2) {
    /* Empty line or too big to fit in an LF and NUL. */
    फर्काऊ FALSE;
  }

  स_नकल(buf, पङ्क्ति, linelen);
  यदि(पङ्क्ति[linelen - 1] != '\n') {
    buf[linelen++] = '\n';
  }
  buf[linelen] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  ख_माला_दो(buf, keylog_file_fp);
  फर्काऊ TRUE;
}

bool Curl_tls_keylog_write(अचर वर्ण *label,
                           अचर अचिन्हित वर्ण *client_random,
                           माप_प्रकार random_size,
                           अचर अचिन्हित वर्ण *secret, माप_प्रकार secretlen)
{
  माप_प्रकार pos, i;
  अचिन्हित वर्ण पङ्क्ति[KEYLOG_LABEL_MAXLEN + 1 +
                     (2 * CLIENT_RANDOM_SIZE) + 1 +
                     (2 * SECRET_MAXLEN) + 1 + 1];
  DEBUGASSERT(random_size >= CLIENT_RANDOM_SIZE);
  यदि(random_size < CLIENT_RANDOM_SIZE)
    फर्काऊ FALSE;
  यदि(!keylog_file_fp)
    फर्काऊ FALSE;

  pos = म_माप(label);
  यदि(pos > KEYLOG_LABEL_MAXLEN || !secretlen || secretlen > SECRET_MAXLEN) {
    /* Should never happen - sanity check anyway. */
    फर्काऊ FALSE;
  }

  स_नकल(पङ्क्ति, label, pos);
  पङ्क्ति[pos++] = ' ';

  /* Client Random */
  लागि(i = 0; i < CLIENT_RANDOM_SIZE; i++) {
    Curl_hexbyte(&पङ्क्ति[pos], client_random[i]);
    pos += 2;
  }
  पङ्क्ति[pos++] = ' ';

  /* Secret */
  लागि(i = 0; i < secretlen; i++) {
    Curl_hexbyte(&पङ्क्ति[pos], secret[i]);
    pos += 2;
  }
  पङ्क्ति[pos++] = '\n';
  पङ्क्ति[pos] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  ख_माला_दो((वर्ण *)पङ्क्ति, keylog_file_fp);
  फर्काऊ TRUE;
}

#अन्यथा /* TLS backend */

bool Curl_tls_keylog_enabled(शून्य)
{
  फर्काऊ FALSE;
}

अचर वर्ण *Curl_tls_keylog_file_name(शून्य)
{
  फर्काऊ रिक्त;
}

#endif  /* TLS backend */
