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

ವರ್ಣ *curl_getenv(ಧ್ರುವ ವರ್ಣ *variable)
{
#ಯದಿ defined(CURL_WINDOWS_UWP) || \
  defined(__ORBIS__) || defined(__PROSPERO__) /* PlayStation 4 and 5 */
  (ಶೂನ್ಯ)variable;
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#ನೋಚೇದ್ಯದಿ defined(_WIN32)
  /* This uses Windows API instead of C runtime getenv() to get the environment
     variable since some changes are not always visible to the latter. #4774 */
  ವರ್ಣ *buf = ರಿಕ್ತ;
  ವರ್ಣ *tmp;
  DWORD bufsize;
  DWORD rc = 1;
  ಧ್ರುವ DWORD max = 32768; /* max env var size from MSCRT source */

  ಪ್ರತಿ(;;) {
    tmp = curlx_realloc(buf, rc);
    ಯದಿ(!tmp) {
      curlx_free(buf);
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    buf = tmp;
    bufsize = rc;

    /* it is possible for rc to be 0 if the variable was found but empty.
       Since getenv does not make that distinction we ignore it as well. */
    rc = GetEnvironmentVariableA(variable, buf, bufsize);
    ಯದಿ(!rc || rc == bufsize || rc > max) {
      curlx_free(buf);
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    /* if rc < bufsize then rc is bytes written not including
       the null-terminator */
    ಯದಿ(rc < bufsize)
      ಪ್ರತ್ಯಾಗಮ buf;

    /* else rc is bytes needed, try again */
  }
#ಅನ್ಯಥಾ
  ವರ್ಣ *env = getenv(variable);
  ಪ್ರತ್ಯಾಗಮ (env && env[0]) ? curlx_strdup(env) : ರಿಕ್ತ;
#endif
}
