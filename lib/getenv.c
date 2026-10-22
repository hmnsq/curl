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
#అన్తర్భావ "curl_setup.h"

అక్షరం *curl_getenv(స్థిరరాశి అక్షరం *variable)
{
#ఒకవేళ defined(CURL_WINDOWS_UWP) || \
  defined(__ORBIS__) || defined(__PROSPERO__) /* PlayStation 4 and 5 */
  (శూన్య)variable;
  తిరిగి ఖాళీ;
#లేదావేళ defined(_WIN32)
  /* This uses Windows API instead of C runtime getenv() to get the environment
     variable since some changes are not always visible to the latter. #4774 */
  అక్షరం *buf = ఖాళీ;
  అక్షరం *tmp;
  DWORD bufsize;
  DWORD rc = 1;
  స్థిరరాశి DWORD max = 32768; /* max env var size from MSCRT source */

  కొరకు(;;) {
    tmp = curlx_realloc(buf, rc);
    ఒకవేళ(!tmp) {
      curlx_free(buf);
      తిరిగి ఖాళీ;
    }

    buf = tmp;
    bufsize = rc;

    /* it is possible for rc to be 0 if the variable was found but empty.
       Since getenv does not make that distinction we ignore it as well. */
    rc = GetEnvironmentVariableA(variable, buf, bufsize);
    ఒకవేళ(!rc || rc == bufsize || rc > max) {
      curlx_free(buf);
      తిరిగి ఖాళీ;
    }

    /* if rc < bufsize then rc is bytes written not including
       the null-terminator */
    ఒకవేళ(rc < bufsize)
      తిరిగి buf;

    /* else rc is bytes needed, try again */
  }
#లేకపోతే
  అక్షరం *env = getenv(variable);
  తిరిగి (env && env[0]) ? curlx_strdup(env) : ఖాళీ;
#అయితే_ముగింపు
}
