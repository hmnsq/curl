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
#অন্তর্ভাব "curl_setup.h"

অক্ষর *curl_getenv(ধ্রুবক অক্ষর *variable)
{
#যদি defined(CURL_WINDOWS_UWP) || \
  defined(__ORBIS__) || defined(__PROSPERO__) /* PlayStation 4 and 5 */
  (শূন্য)variable;
  ফেরত ফাঁকা;
#নতুবা_যদি defined(_WIN32)
  /* This uses Windows API instead of C runtime getenv() to get the environment
     variable since some changes are not always visible to the latter. #4774 */
  অক্ষর *buf = ফাঁকা;
  অক্ষর *tmp;
  DWORD bufsize;
  DWORD rc = 1;
  ধ্রুবক DWORD max = 32768; /* max env var size from MSCRT source */

  জন্য(;;) {
    tmp = curlx_realloc(buf, rc);
    যদি(!tmp) {
      curlx_free(buf);
      ফেরত ফাঁকা;
    }

    buf = tmp;
    bufsize = rc;

    /* it is possible for rc to be 0 if the variable was found but empty.
       Since getenv does not make that distinction we ignore it as well. */
    rc = GetEnvironmentVariableA(variable, buf, bufsize);
    যদি(!rc || rc == bufsize || rc > max) {
      curlx_free(buf);
      ফেরত ফাঁকা;
    }

    /* if rc < bufsize then rc is bytes written not including
       the null-terminator */
    যদি(rc < bufsize)
      ফেরত buf;

    /* else rc is bytes needed, try again */
  }
#অন্যথা
  অক্ষর *env = getenv(variable);
  ফেরত (env && env[0]) ? curlx_strdup(env) : ফাঁকা;
#endif
}
