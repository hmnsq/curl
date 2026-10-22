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

ಪೂರ್ಣಾಂಕ Curl_gethostname(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ name, GETHOSTNAME_TYPE_ARG2 namelen)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_GETHOSTNAME

  /* Allow compilation and return failure when unavailable */
  (ಶೂನ್ಯ)name;
  (ಶೂನ್ಯ)namelen;
  ಹಿಂತಿರುಗಿ -1;

#ಇಲ್ಲದಿದ್ದರೆ
  ಪೂರ್ಣಾಂಕ err;
  ಅಕ್ಷರ *dot;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD

  /* Override hostname when environment variable CURL_GETHOSTNAME is set */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *force_hostname = getenv("CURL_GETHOSTNAME");
  ಆದರೆ(force_hostname) {
    ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(force_hostname) < (size_t)namelen)
      curlx_strcopy(name, namelen, force_hostname, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(force_hostname));
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ 1; /* cannot do it */
    err = 0;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    name[0] = '\0';
    err = gethostname(name, namelen);
  }

#ಇಲ್ಲದಿದ್ದರೆ /* DEBUGBUILD */

  name[0] = '\0';
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __AMIGA__
  err = gethostname((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)name, namelen);
#ಇಲ್ಲದಿದ್ದರೆ
  err = gethostname(name, namelen);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ

  name[namelen - 1] = '\0';

  ಆದರೆ(err)
    ಹಿಂತಿರುಗಿ err;

  /* Truncate domain, leave only machine name */
  dot = strchr(name, '.');
  ಆದರೆ(dot)
    *dot = '\0';

  ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
}
