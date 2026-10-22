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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_gethostname.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strcopy.h"

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

ਪੂਰਨ Curl_gethostname(ਅੱਖਰ * ਅਚਲ name, GETHOSTNAME_TYPE_ARG2 namelen)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_GETHOSTNAME

  /* Allow compilation and return failure when unavailable */
  (ਖਾਲੀ)name;
  (ਖਾਲੀ)namelen;
  ਵਾਪਸ -1;

#ਨਹੀਂਤਾਂ
  ਪੂਰਨ err;
  ਅੱਖਰ *dot;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD

  /* Override hostname when environment variable CURL_GETHOSTNAME is set */
  ਅਚਲ ਅੱਖਰ *force_hostname = getenv("CURL_GETHOSTNAME");
  ਜੇ(force_hostname) {
    ਜੇ(ਲੜੀ_ਲੰਬਾਈ(force_hostname) < (size_t)namelen)
      curlx_strcopy(name, namelen, force_hostname, ਲੜੀ_ਲੰਬਾਈ(force_hostname));
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ 1; /* cannot do it */
    err = 0;
  }
  ਨਹੀਂਤਾਂ {
    name[0] = '\0';
    err = gethostname(name, namelen);
  }

#ਨਹੀਂਤਾਂ /* DEBUGBUILD */

  name[0] = '\0';
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __AMIGA__
  err = gethostname((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)name, namelen);
#ਨਹੀਂਤਾਂ
  err = gethostname(name, namelen);
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ

  name[namelen - 1] = '\0';

  ਜੇ(err)
    ਵਾਪਸ err;

  /* Truncate domain, leave only machine name */
  dot = strchr(name, '.');
  ਜੇ(dot)
    *dot = '\0';

  ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
}
