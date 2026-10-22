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

حرف *curl_getenv(ثابت حرف *variable)
{
#اگر defined(CURL_WINDOWS_UWP) || \
  defined(__ORBIS__) || defined(__PROSPERO__) /* PlayStation 4 and 5 */
  (خالی)variable;
  واپس تہی;
#وگرنہ_اگر defined(_WIN32)
  /* This uses Windows API instead of C runtime getenv() to get the environment
     variable since some changes are not always visible to the latter. #4774 */
  حرف *buf = تہی;
  حرف *tmp;
  DWORD bufsize;
  DWORD rc = 1;
  ثابت DWORD max = 32768; /* max env var size from MSCRT source */

  برائے(;;) {
    tmp = curlx_realloc(buf, rc);
    اگر(!tmp) {
      curlx_free(buf);
      واپس تہی;
    }

    buf = tmp;
    bufsize = rc;

    /* it is possible for rc to be 0 if the variable was found but empty.
       Since getenv does not make that distinction we ignore it as well. */
    rc = GetEnvironmentVariableA(variable, buf, bufsize);
    اگر(!rc || rc == bufsize || rc > max) {
      curlx_free(buf);
      واپس تہی;
    }

    /* if rc < bufsize then rc is bytes written not including
       the null-terminator */
    اگر(rc < bufsize)
      واپس buf;

    /* else rc is bytes needed, try again */
  }
#ورنہ
  حرف *env = getenv(variable);
  واپس (env && env[0]) ? curlx_strdup(env) : تہی;
#اگر_ختم
}
