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

#ಅನ್ತರ್ಭಾವ "easyoptions.h"

#ifndef CURL_DISABLE_GETOPTIONS

/* Lookups easy options at runtime */
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ curl_easyoption *lookup(ಧ್ರುವ ವರ್ಣ *name, CURLoption id)
{
  DEBUGASSERT(name || id);
  DEBUGASSERT(!Curl_easyopts_check());
  ಯದಿ(name || id) {
    ಧ್ರುವ ಸಂರಚನಾ curl_easyoption *o = &Curl_easyopts[0];
    ಕುರು {
      ಯದಿ(name) {
        ಯದಿ(curl_strequal(o->name, name))
          ಪ್ರತ್ಯಾಗಮ o;
      }
      ಅನ್ಯಥಾ {
        ಯದಿ((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          ಪ್ರತ್ಯಾಗಮ o;
      }
      o++;
    } ಯಾವತ್(o->name);
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಧ್ರುವ ಸಂರಚನಾ curl_easyoption *curl_easy_option_by_name(ಧ್ರುವ ವರ್ಣ *name)
{
  /* when name is used, the id argument is ignored */
  ಪ್ರತ್ಯಾಗಮ lookup(name, CURLOPT_LASTENTRY);
}

ಧ್ರುವ ಸಂರಚನಾ curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  ಪ್ರತ್ಯಾಗಮ lookup(ರಿಕ್ತ, id);
}

/* Iterates over available options */
ಧ್ರುವ ಸಂರಚನಾ curl_easyoption *curl_easy_option_next(
  ಧ್ರುವ ಸಂರಚನಾ curl_easyoption *prev)
{
  ಯದಿ(prev && prev->name) {
    prev++;
    ಯದಿ(prev->name)
      ಪ್ರತ್ಯಾಗಮ prev;
  }
  ಅನ್ಯಥಾ ಯದಿ(!prev)
    ಪ್ರತ್ಯಾಗಮ &Curl_easyopts[0];
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ಅನ್ಯಥಾ
ಧ್ರುವ ಸಂರಚನಾ curl_easyoption *curl_easy_option_by_name(ಧ್ರುವ ವರ್ಣ *name)
{
  (ಶೂನ್ಯ)name;
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಧ್ರುವ ಸಂರಚನಾ curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (ಶೂನ್ಯ)id;
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಧ್ರುವ ಸಂರಚನಾ curl_easyoption *curl_easy_option_next(
  ಧ್ರುವ ಸಂರಚನಾ curl_easyoption *prev)
{
  (ಶೂನ್ಯ)prev;
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif
