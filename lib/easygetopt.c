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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_GETOPTIONS

/* Lookups easy options at runtime */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ curl_easyoption *lookup(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, CURLoption id)
{
  DEBUGASSERT(name || id);
  DEBUGASSERT(!Curl_easyopts_check());
  ಆದರೆ(name || id) {
    ಸ್ಥಿರಾಂಕ ರಚನೆ curl_easyoption *o = &Curl_easyopts[0];
    ಮಾಡು {
      ಆದರೆ(name) {
        ಆದರೆ(curl_strequal(o->name, name))
          ಹಿಂತಿರುಗಿ o;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          ಹಿಂತಿರುಗಿ o;
      }
      o++;
    } ಆಗ(o->name);
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರಾಂಕ ರಚನೆ curl_easyoption *curl_easy_option_by_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
  /* when name is used, the id argument is ignored */
  ಹಿಂತಿರುಗಿ lookup(name, CURLOPT_LASTENTRY);
}

ಸ್ಥಿರಾಂಕ ರಚನೆ curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  ಹಿಂತಿರುಗಿ lookup(ಖಾಲಿ, id);
}

/* Iterates over available options */
ಸ್ಥಿರಾಂಕ ರಚನೆ curl_easyoption *curl_easy_option_ಮುಂದಿನ(
  ಸ್ಥಿರಾಂಕ ರಚನೆ curl_easyoption *prev)
{
  ಆದರೆ(prev && prev->name) {
    prev++;
    ಆದರೆ(prev->name)
      ಹಿಂತಿರುಗಿ prev;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!prev)
    ಹಿಂತಿರುಗಿ &Curl_easyopts[0];
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರಾಂಕ ರಚನೆ curl_easyoption *curl_easy_option_by_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
  (ಶೂನ್ಯ)name;
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರಾಂಕ ರಚನೆ curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (ಶೂನ್ಯ)id;
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರಾಂಕ ರಚನೆ curl_easyoption *curl_easy_option_ಮುಂದಿನ(
  ಸ್ಥಿರಾಂಕ ರಚನೆ curl_easyoption *prev)
{
  (ಶೂನ್ಯ)prev;
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ
