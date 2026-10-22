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

#అన్తర్భావ "curl_gethostname.h"
#అన్తర్భావ "curlx/strcopy.h"

/*
 * Curl_gethostname() is a wrapper around gethostname() which allows
 * overriding the hostname that the function would normally return.
 * This capability is used by the test suite to verify exact matching
 * of NTLM authentication, which exercises libcurl's MD4 and DES code
 * as well as by the SMTP module when a hostname is not provided.
 *
 * For libcurl debug enabled builds hostname overriding takes place
 * when environment variable CURL_GETHOSTNAME is set, using the value
 * held by the variable to override returned hostname.
 *
 * Note: The function always returns the un-qualified hostname rather
 * than being provider dependent.
 */

పూర్ణాంకం Curl_gethostname(అక్షరం * స్థిరరాశి name, GETHOSTNAME_TYPE_ARG2 namelen)
{
#నిర్వచితం_కాకపోతే HAVE_GETHOSTNAME

  /* Allow compilation and return failure when unavailable */
  (శూన్య)name;
  (శూన్య)namelen;
  తిరిగి -1;

#లేకపోతే
  పూర్ణాంకం err;
  అక్షరం *dot;

#నిర్వచితమైతే DEBUGBUILD

  /* Override hostname when environment variable CURL_GETHOSTNAME is set */
  స్థిరరాశి అక్షరం *force_hostname = getenv("CURL_GETHOSTNAME");
  ఒకవేళ(force_hostname) {
    ఒకవేళ(అక్షరశ్రేణి_పొడవు(force_hostname) < (size_t)namelen)
      curlx_strcopy(name, namelen, force_hostname, అక్షరశ్రేణి_పొడవు(force_hostname));
    లేకపోతే
      తిరిగి 1; /* cannot do it */
    err = 0;
  }
  లేకపోతే {
    name[0] = '\0';
    err = gethostname(name, namelen);
  }

#లేకపోతే /* DEBUGBUILD */

  name[0] = '\0';
#నిర్వచితమైతే __AMIGA__
  err = gethostname((గుర్తులేని అక్షరం *)name, namelen);
#లేకపోతే
  err = gethostname(name, namelen);
#అయితే_ముగింపు

#అయితే_ముగింపు

  name[namelen - 1] = '\0';

  ఒకవేళ(err)
    తిరిగి err;

  /* Truncate domain, leave only machine name */
  dot = strchr(name, '.');
  ఒకవేళ(dot)
    *dot = '\0';

  తిరిగి 0;
#అయితే_ముగింపు
}
