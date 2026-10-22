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
#അന്തര്ഭാവ "curl_setup.h"

അക്ഷരം *curl_getenv(സ്ഥിരം അക്ഷരം *variable)
{
#എങ്കിൽ defined(CURL_WINDOWS_UWP) || \
  defined(__ORBIS__) || defined(__PROSPERO__) /* PlayStation 4 and 5 */
  (ശൂന്യം)variable;
  തിരികെ ശൂന്യ;
#അല്ലെങ്കിലെങ്കിൽ defined(_WIN32)
  /* This uses Windows API instead of C runtime getenv() to get the environment
     variable since some changes are not always visible to the latter. #4774 */
  അക്ഷരം *buf = ശൂന്യ;
  അക്ഷരം *tmp;
  DWORD bufsize;
  DWORD rc = 1;
  സ്ഥിരം DWORD max = 32768; /* max env var size from MSCRT source */

  വേണ്ടി(;;) {
    tmp = curlx_realloc(buf, rc);
    എങ്കിൽ(!tmp) {
      curlx_free(buf);
      തിരികെ ശൂന്യ;
    }

    buf = tmp;
    bufsize = rc;

    /* it is possible for rc to be 0 if the variable was found but empty.
       Since getenv does not make that distinction we ignore it as well. */
    rc = GetEnvironmentVariableA(variable, buf, bufsize);
    എങ്കിൽ(!rc || rc == bufsize || rc > max) {
      curlx_free(buf);
      തിരികെ ശൂന്യ;
    }

    /* if rc < bufsize then rc is bytes written not including
       the null-terminator */
    എങ്കിൽ(rc < bufsize)
      തിരികെ buf;

    /* else rc is bytes needed, try again */
  }
#അല്ലെങ്കിൽ
  അക്ഷരം *env = getenv(variable);
  തിരികെ (env && env[0]) ? curlx_strdup(env) : ശൂന്യ;
#endif
}
