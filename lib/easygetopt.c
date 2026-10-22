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
  यदि(name || id) {
    ध्रुव संरचना curl_easyoption *o = &Curl_easyopts[0];
    कुरु {
      यदि(name) {
        यदि(curl_strequal(o->name, name))
          प्रत्यागम o;
      }
      अन्यथा {
        यदि((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          प्रत्यागम o;
      }
      o++;
    } यावत्(o->name);
  }
  प्रत्यागम रिक्त;
}

ध्रुव संरचना curl_easyoption *curl_easy_option_by_name(ध्रुव वर्ण *name)
{
  /* when name is used, the id argument is ignored */
  प्रत्यागम lookup(name, CURLOPT_LASTENTRY);
}

ध्रुव संरचना curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  प्रत्यागम lookup(रिक्त, id);
}

/* Iterates over available options */
ध्रुव संरचना curl_easyoption *curl_easy_option_next(
  ध्रुव संरचना curl_easyoption *prev)
{
  यदि(prev && prev->name) {
    prev++;
    यदि(prev->name)
      प्रत्यागम prev;
  }
  अन्यथा यदि(!prev)
    प्रत्यागम &Curl_easyopts[0];
  प्रत्यागम रिक्त;
}

#अन्यथा
ध्रुव संरचना curl_easyoption *curl_easy_option_by_name(ध्रुव वर्ण *name)
{
  (शून्य)name;
  प्रत्यागम रिक्त;
}

ध्रुव संरचना curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (शून्य)id;
  प्रत्यागम रिक्त;
}

ध्रुव संरचना curl_easyoption *curl_easy_option_next(
  ध्रुव संरचना curl_easyoption *prev)
{
  (शून्य)prev;
  प्रत्यागम रिक्त;
}
#endif
