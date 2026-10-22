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
स्थिर ध्रुव संरचना curl_easyoption *lookup(ध्रुव वर्ण *name, CURLoption id)
{
  DEBUGASSERT(name || id);
  DEBUGASSERT(!Curl_easyopts_check());
  सचे(name || id) {
    ध्रुव संरचना curl_easyoption *o = &Curl_easyopts[0];
    करोहि {
      सचे(name) {
        सचे(curl_strequal(o->name, name))
          पटिनिवत्त o;
      }
      अञ्ञथा {
        सचे((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          पटिनिवत्त o;
      }
      o++;
    } याव(o->name);
  }
  पटिनिवत्त रिक्त;
}

ध्रुव संरचना curl_easyoption *curl_easy_option_by_name(ध्रुव वर्ण *name)
{
  /* when name is used, the id argument is ignored */
  पटिनिवत्त lookup(name, CURLOPT_LASTENTRY);
}

ध्रुव संरचना curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  पटिनिवत्त lookup(रिक्त, id);
}

/* Iterates over available options */
ध्रुव संरचना curl_easyoption *curl_easy_option_next(
  ध्रुव संरचना curl_easyoption *prev)
{
  सचे(prev && prev->name) {
    prev++;
    सचे(prev->name)
      पटिनिवत्त prev;
  }
  अञ्ञथा सचे(!prev)
    पटिनिवत्त &Curl_easyopts[0];
  पटिनिवत्त रिक्त;
}

#अञ्ञथा
ध्रुव संरचना curl_easyoption *curl_easy_option_by_name(ध्रुव वर्ण *name)
{
  (शून्य)name;
  पटिनिवत्त रिक्त;
}

ध्रुव संरचना curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (शून्य)id;
  पटिनिवत्त रिक्त;
}

ध्रुव संरचना curl_easyoption *curl_easy_option_next(
  ध्रुव संरचना curl_easyoption *prev)
{
  (शून्य)prev;
  पटिनिवत्त रिक्त;
}
#endif
