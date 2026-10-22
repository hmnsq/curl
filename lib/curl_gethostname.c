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
#समावेश "curl_setup.h"

#समावेश "curl_gethostname.h"
#समावेश "curlx/strcopy.h"

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

पूर्णांक Curl_gethostname(अक्षर * स्थिर name, GETHOSTNAME_TYPE_ARG2 namelen)
{
#अगर_अघोषित HAVE_GETHOSTNAME

  /* Allow compilation and return failure when unavailable */
  (व्योम)name;
  (व्योम)namelen;
  वापस -1;

#अन्यथा
  पूर्णांक err;
  अक्षर *dot;

#अगर_घोषित DEBUGBUILD

  /* Override hostname when environment variable CURL_GETHOSTNAME is set */
  स्थिर अक्षर *force_hostname = दो_पर्या("CURL_GETHOSTNAME");
  अगर(force_hostname) {
    अगर(म_माप(force_hostname) < (माप_प्रकार)namelen)
      curlx_strcopy(name, namelen, force_hostname, म_माप(force_hostname));
    अन्यथा
      वापस 1; /* cannot do it */
    err = 0;
  }
  अन्यथा {
    name[0] = '\0';
    err = gethostname(name, namelen);
  }

#अन्यथा /* DEBUGBUILD */

  name[0] = '\0';
#अगर_घोषित __AMIGA__
  err = gethostname((अचिन्हित अक्षर *)name, namelen);
#अन्यथा
  err = gethostname(name, namelen);
#पूर्ण_अगर

#पूर्ण_अगर

  name[namelen - 1] = '\0';

  अगर(err)
    वापस err;

  /* Truncate domain, leave only machine name */
  dot = म_अक्षर(name, '.');
  अगर(dot)
    *dot = '\0';

  वापस 0;
#पूर्ण_अगर
}
