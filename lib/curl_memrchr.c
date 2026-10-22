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

#అన్తర్భావ "curl_memrchr.h"

#నిర్వచితం_కాకపోతే HAVE_MEMRCHR
/*
 * Curl_memrchr()
 *
 * Our memrchr() function clone for systems which lack this function. The
 * memrchr() function is like the memchr() function, except that it searches
 * backwards from the end of the n bytes pointed to by s instead of forward
 * from the beginning.
 */
శూన్య *Curl_memrchr(స్థిరరాశి శూన్య *s, పూర్ణాంకం c, size_t n)
{
  స్థిరరాశి గుర్తులేని అక్షరం *p = s;

  అయితే(n) {
    ఒకవేళ(p[--n] == (గుర్తులేని అక్షరం)c)
      తిరిగి CURL_UNCONST(p + n);
  }
  తిరిగి ఖాళీ;
}
#అయితే_ముగింపు /* HAVE_MEMRCHR */
