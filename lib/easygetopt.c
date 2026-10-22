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
स्थिर अचर संरचना curl_easyoption *lookup(अचर वर्ण *name, CURLoption id)
{
  DEBUGASSERT(name || id);
  DEBUGASSERT(!Curl_easyopts_check());
  यदि(name || id) {
    अचर संरचना curl_easyoption *o = &Curl_easyopts[0];
    गर {
      यदि(name) {
        यदि(curl_strequal(o->name, name))
          फर्काऊ o;
      }
      अन्यथा {
        यदि((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          फर्काऊ o;
      }
      o++;
    } जबसम्म(o->name);
  }
  फर्काऊ रिक्त;
}

अचर संरचना curl_easyoption *curl_easy_option_by_name(अचर वर्ण *name)
{
  /* when name is used, the id argument is ignored */
  फर्काऊ lookup(name, CURLOPT_LASTENTRY);
}

अचर संरचना curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  फर्काऊ lookup(रिक्त, id);
}

/* Iterates over available options */
अचर संरचना curl_easyoption *curl_easy_option_next(
  अचर संरचना curl_easyoption *prev)
{
  यदि(prev && prev->name) {
    prev++;
    यदि(prev->name)
      फर्काऊ prev;
  }
  अन्यथा यदि(!prev)
    फर्काऊ &Curl_easyopts[0];
  फर्काऊ रिक्त;
}

#अन्यथा
अचर संरचना curl_easyoption *curl_easy_option_by_name(अचर वर्ण *name)
{
  (शून्य)name;
  फर्काऊ रिक्त;
}

अचर संरचना curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (शून्य)id;
  फर्काऊ रिक्त;
}

अचर संरचना curl_easyoption *curl_easy_option_next(
  अचर संरचना curl_easyoption *prev)
{
  (शून्य)prev;
  फर्काऊ रिक्त;
}
#endif
