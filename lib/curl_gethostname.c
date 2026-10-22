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

#ಅನ್ತರ್ಭಾವ "curl_gethostname.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

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

ಪೂರ್ಣಾಙ್ಕ Curl_gethostname(ವರ್ಣ * ಧ್ರುವ name, GETHOSTNAME_TYPE_ARG2 namelen)
{
#ifndef HAVE_GETHOSTNAME

  /* Allow compilation and return failure when unavailable */
  (ಶೂನ್ಯ)name;
  (ಶೂನ್ಯ)namelen;
  ಪ್ರತ್ಯಾಗಮ -1;

#ಅನ್ಯಥಾ
  ಪೂರ್ಣಾಙ್ಕ err;
  ವರ್ಣ *dot;

#ifdef DEBUGBUILD

  /* Override hostname when environment variable CURL_GETHOSTNAME is set */
  ಧ್ರುವ ವರ್ಣ *force_hostname = getenv("CURL_GETHOSTNAME");
  ಯದಿ(force_hostname) {
    ಯದಿ(strlen(force_hostname) < (size_t)namelen)
      curlx_strcopy(name, namelen, force_hostname, strlen(force_hostname));
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ 1; /* cannot do it */
    err = 0;
  }
  ಅನ್ಯಥಾ {
    name[0] = '\0';
    err = gethostname(name, namelen);
  }

#ಅನ್ಯಥಾ /* DEBUGBUILD */

  name[0] = '\0';
#ifdef __AMIGA__
  err = gethostname((unsigned ವರ್ಣ *)name, namelen);
#ಅನ್ಯಥಾ
  err = gethostname(name, namelen);
#endif

#endif

  name[namelen - 1] = '\0';

  ಯದಿ(err)
    ಪ್ರತ್ಯಾಗಮ err;

  /* Truncate domain, leave only machine name */
  dot = strchr(name, '.');
  ಯದಿ(dot)
    *dot = '\0';

  ಪ್ರತ್ಯಾಗಮ 0;
#endif
}
