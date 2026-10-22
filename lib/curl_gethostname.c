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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "curl_gethostname.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strcopy.h"

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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_gethostname(ଅକ୍ଷର * ଅଚଳ name, GETHOSTNAME_TYPE_ARG2 namelen)
{
#ifndef HAVE_GETHOSTNAME

  /* Allow compilation and return failure when unavailable */
  (ଶୂନ୍ୟ)name;
  (ଶୂନ୍ୟ)namelen;
  ଫେରନ୍ତୁ -1;

#ଅନ୍ୟଥା
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;
  ଅକ୍ଷର *dot;

#ifdef DEBUGBUILD

  /* Override hostname when environment variable CURL_GETHOSTNAME is set */
  ଅଚଳ ଅକ୍ଷର *force_hostname = getenv("CURL_GETHOSTNAME");
  ଯଦି(force_hostname) {
    ଯଦି(strlen(force_hostname) < (size_t)namelen)
      curlx_strcopy(name, namelen, force_hostname, strlen(force_hostname));
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ 1; /* cannot do it */
    err = 0;
  }
  ଅନ୍ୟଥା {
    name[0] = '\0';
    err = gethostname(name, namelen);
  }

#ଅନ୍ୟଥା /* DEBUGBUILD */

  name[0] = '\0';
#ifdef __AMIGA__
  err = gethostname((unsigned ଅକ୍ଷର *)name, namelen);
#ଅନ୍ୟଥା
  err = gethostname(name, namelen);
#endif

#endif

  name[namelen - 1] = '\0';

  ଯଦି(err)
    ଫେରନ୍ତୁ err;

  /* Truncate domain, leave only machine name */
  dot = strchr(name, '.');
  ଯଦି(dot)
    *dot = '\0';

  ଫେରନ୍ତୁ 0;
#endif
}
