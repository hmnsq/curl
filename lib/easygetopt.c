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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "easyoptions.h"

#ifndef CURL_DISABLE_GETOPTIONS

/* Lookups easy options at runtime */
स्थिर स्थिरांक संरचना curl_easyoption *lookup(स्थिरांक अक्षर *name, CURLoption id)
{
  DEBUGASSERT(name || id);
  DEBUGASSERT(!Curl_easyopts_check());
  जर(name || id) {
    स्थिरांक संरचना curl_easyoption *o = &Curl_easyopts[0];
    करा {
      जर(name) {
        जर(curl_strequal(o->name, name))
          परत o;
      }
      अन्यथा {
        जर((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          परत o;
      }
      o++;
    } पर्यंत(o->name);
  }
  परत रिक्त;
}

स्थिरांक संरचना curl_easyoption *curl_easy_option_by_name(स्थिरांक अक्षर *name)
{
  /* when name is used, the id argument is ignored */
  परत lookup(name, CURLOPT_LASTENTRY);
}

स्थिरांक संरचना curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  परत lookup(रिक्त, id);
}

/* Iterates over available options */
स्थिरांक संरचना curl_easyoption *curl_easy_option_next(
  स्थिरांक संरचना curl_easyoption *prev)
{
  जर(prev && prev->name) {
    prev++;
    जर(prev->name)
      परत prev;
  }
  अन्यथा जर(!prev)
    परत &Curl_easyopts[0];
  परत रिक्त;
}

#अन्यथा
स्थिरांक संरचना curl_easyoption *curl_easy_option_by_name(स्थिरांक अक्षर *name)
{
  (शून्य)name;
  परत रिक्त;
}

स्थिरांक संरचना curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (शून्य)id;
  परत रिक्त;
}

स्थिरांक संरचना curl_easyoption *curl_easy_option_next(
  स्थिरांक संरचना curl_easyoption *prev)
{
  (शून्य)prev;
  परत रिक्त;
}
#endif
