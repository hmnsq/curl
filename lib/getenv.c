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
#include "curl_setup.h"

var_na *curl_getenv(const var_na *variable)
{
#ᮚᮦᮔ᮪ defined(CURL_WINDOWS_UWP) || \
  defined(__ORBIS__) || defined(__PROSPERO__) /* PlayStation 4 and 5 */
  (void)variable;
  ᮝᮜᮤ NULL;
#elif defined(_WIN32)
  /* This uses Windows API instead of C runtime getenv() to get the environment
     variable since some changes are not always visible to the latter. #4774 */
  var_na *buf = NULL;
  var_na *tmp;
  DWORD bufsize;
  DWORD rc = 1;
  const DWORD max = 32768; /* max env var size from MSCRT source */

  for(;;) {
    tmp = curlx_realloc(buf, rc);
    ᮚᮦᮔ᮪(!tmp) {
      curlx_free(buf);
      ᮝᮜᮤ NULL;
    }

    buf = tmp;
    bufsize = rc;

    /* it is possible for rc to be 0 if the variable was found but empty.
       Since getenv does not make that distinction we ignore it as well. */
    rc = GetEnvironmentVariableA(variable, buf, bufsize);
    ᮚᮦᮔ᮪(!rc || rc == bufsize || rc > max) {
      curlx_free(buf);
      ᮝᮜᮤ NULL;
    }

    /* if rc < bufsize then rc is bytes written not including
       the null-terminator */
    ᮚᮦᮔ᮪(rc < bufsize)
      ᮝᮜᮤ buf;

    /* else rc is bytes needed, try again */
  }
#ᮜᮤᮚ
  var_na *env = getenv(variable);
  ᮝᮜᮤ (env && env[0]) ? curlx_strdup(env) : NULL;
#endif
}
