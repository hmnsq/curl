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
#அந்தர்பாவ "curl_setup.h"

எழுத்து *curl_getenv(மாறிலி எழுத்து *variable)
{
#எனில் defined(CURL_WINDOWS_UWP) || \
  defined(__ORBIS__) || defined(__PROSPERO__) /* PlayStation 4 and 5 */
  (வெறுமை)variable;
  திரும்பு வெற்று;
#அல்லதெனில் defined(_WIN32)
  /* This uses Windows API instead of C runtime getenv() to get the environment
     variable since some changes are not always visible to the latter. #4774 */
  எழுத்து *buf = வெற்று;
  எழுத்து *tmp;
  DWORD bufsize;
  DWORD rc = 1;
  மாறிலி DWORD max = 32768; /* max env var size from MSCRT source */

  க்கு(;;) {
    tmp = curlx_realloc(buf, rc);
    எனில்(!tmp) {
      curlx_free(buf);
      திரும்பு வெற்று;
    }

    buf = tmp;
    bufsize = rc;

    /* it is possible for rc to be 0 if the variable was found but empty.
       Since getenv does not make that distinction we ignore it as well. */
    rc = GetEnvironmentVariableA(variable, buf, bufsize);
    எனில்(!rc || rc == bufsize || rc > max) {
      curlx_free(buf);
      திரும்பு வெற்று;
    }

    /* if rc < bufsize then rc is bytes written not including
       the null-terminator */
    எனில்(rc < bufsize)
      திரும்பு buf;

    /* else rc is bytes needed, try again */
  }
#இல்லையெனில்
  எழுத்து *env = getenv(variable);
  திரும்பு (env && env[0]) ? curlx_strdup(env) : வெற்று;
#endif
}
