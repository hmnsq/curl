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

#அந்தர்பாவ "curl_gethostname.h"
#அந்தர்பாவ "curlx/strcopy.h"

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

முழு Curl_gethostname(எழுத்து * மாறிலி name, GETHOSTNAME_TYPE_ARG2 namelen)
{
#ifndef HAVE_GETHOSTNAME

  /* Allow compilation and return failure when unavailable */
  (வெறுமை)name;
  (வெறுமை)namelen;
  திரும்பு -1;

#இல்லையெனில்
  முழு err;
  எழுத்து *dot;

#ifdef DEBUGBUILD

  /* Override hostname when environment variable CURL_GETHOSTNAME is set */
  மாறிலி எழுத்து *force_hostname = getenv("CURL_GETHOSTNAME");
  எனில்(force_hostname) {
    எனில்(strlen(force_hostname) < (size_t)namelen)
      curlx_strcopy(name, namelen, force_hostname, strlen(force_hostname));
    இல்லையெனில்
      திரும்பு 1; /* cannot do it */
    err = 0;
  }
  இல்லையெனில் {
    name[0] = '\0';
    err = gethostname(name, namelen);
  }

#இல்லையெனில் /* DEBUGBUILD */

  name[0] = '\0';
#ifdef __AMIGA__
  err = gethostname((unsigned எழுத்து *)name, namelen);
#இல்லையெனில்
  err = gethostname(name, namelen);
#endif

#endif

  name[namelen - 1] = '\0';

  எனில்(err)
    திரும்பு err;

  /* Truncate domain, leave only machine name */
  dot = strchr(name, '.');
  எனில்(dot)
    *dot = '\0';

  திரும்பு 0;
#endif
}
