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

वर्ण *curl_getenv(ध्रुव वर्ण *variable)
{
#यदि defined(CURL_WINDOWS_UWP) || \
  defined(__ORBIS__) || defined(__PROSPERO__) /* PlayStation 4 and 5 */
  (शून्य)variable;
  प्रत्यागम रिक्त;
#नोचेद्यदि defined(_WIN32)
  /* This uses Windows API instead of C runtime getenv() to get the environment
     variable since some changes are not always visible to the latter. #4774 */
  वर्ण *buf = रिक्त;
  वर्ण *tmp;
  DWORD bufsize;
  DWORD rc = 1;
  ध्रुव DWORD max = 32768; /* max env var size from MSCRT source */

  प्रति(;;) {
    tmp = curlx_realloc(buf, rc);
    यदि(!tmp) {
      curlx_free(buf);
      प्रत्यागम रिक्त;
    }

    buf = tmp;
    bufsize = rc;

    /* it is possible for rc to be 0 if the variable was found but empty.
       Since getenv does not make that distinction we ignore it as well. */
    rc = GetEnvironmentVariableA(variable, buf, bufsize);
    यदि(!rc || rc == bufsize || rc > max) {
      curlx_free(buf);
      प्रत्यागम रिक्त;
    }

    /* if rc < bufsize then rc is bytes written not including
       the null-terminator */
    यदि(rc < bufsize)
      प्रत्यागम buf;

    /* else rc is bytes needed, try again */
  }
#अन्यथा
  वर्ण *env = getenv(variable);
  प्रत्यागम (env && env[0]) ? curlx_strdup(env) : रिक्त;
#endif
}
