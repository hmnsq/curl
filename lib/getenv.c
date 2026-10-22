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
#অন্তর্ভাৱ "curl_setup.h"

ৱর্ণ *curl_getenv(ধ্রুৱ ৱর্ণ *variable)
{
#যদি defined(CURL_WINDOWS_UWP) || \
  defined(__ORBIS__) || defined(__PROSPERO__) /* PlayStation 4 and 5 */
  (শূন্য)variable;
  ঘূৰাই রিক্ত;
#নোচেদ্যদি defined(_WIN32)
  /* This uses Windows API instead of C runtime getenv() to get the environment
     variable since some changes are not always visible to the latter. #4774 */
  ৱর্ণ *buf = রিক্ত;
  ৱর্ণ *tmp;
  DWORD bufsize;
  DWORD rc = 1;
  ধ্রুৱ DWORD max = 32768; /* max env var size from MSCRT source */

  বাবে(;;) {
    tmp = curlx_realloc(buf, rc);
    যদি(!tmp) {
      curlx_free(buf);
      ঘূৰাই রিক্ত;
    }

    buf = tmp;
    bufsize = rc;

    /* it is possible for rc to be 0 if the variable was found but empty.
       Since getenv does not make that distinction we ignore it as well. */
    rc = GetEnvironmentVariableA(variable, buf, bufsize);
    যদি(!rc || rc == bufsize || rc > max) {
      curlx_free(buf);
      ঘূৰাই রিক্ত;
    }

    /* if rc < bufsize then rc is bytes written not including
       the null-terminator */
    যদি(rc < bufsize)
      ঘূৰাই buf;

    /* else rc is bytes needed, try again */
  }
#অন্যথা
  ৱর্ণ *env = getenv(variable);
  ঘূৰাই (env && env[0]) ? curlx_strdup(env) : রিক্ত;
#endif
}
