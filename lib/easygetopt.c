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

#அந்தர்பாவ "easyoptions.h"

#ifndef CURL_DISABLE_GETOPTIONS

/* Lookups easy options at runtime */
நிலையான மாறிலி கட்டமைப்பு curl_easyoption *lookup(மாறிலி எழுத்து *name, CURLoption id)
{
  DEBUGASSERT(name || id);
  DEBUGASSERT(!Curl_easyopts_check());
  எனில்(name || id) {
    மாறிலி கட்டமைப்பு curl_easyoption *o = &Curl_easyopts[0];
    செய் {
      எனில்(name) {
        எனில்(curl_strequal(o->name, name))
          திரும்பு o;
      }
      இல்லையெனில் {
        எனில்((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          திரும்பு o;
      }
      o++;
    } போது(o->name);
  }
  திரும்பு வெற்று;
}

மாறிலி கட்டமைப்பு curl_easyoption *curl_easy_option_by_name(மாறிலி எழுத்து *name)
{
  /* when name is used, the id argument is ignored */
  திரும்பு lookup(name, CURLOPT_LASTENTRY);
}

மாறிலி கட்டமைப்பு curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  திரும்பு lookup(வெற்று, id);
}

/* Iterates over available options */
மாறிலி கட்டமைப்பு curl_easyoption *curl_easy_option_next(
  மாறிலி கட்டமைப்பு curl_easyoption *prev)
{
  எனில்(prev && prev->name) {
    prev++;
    எனில்(prev->name)
      திரும்பு prev;
  }
  இல்லையெனில் எனில்(!prev)
    திரும்பு &Curl_easyopts[0];
  திரும்பு வெற்று;
}

#இல்லையெனில்
மாறிலி கட்டமைப்பு curl_easyoption *curl_easy_option_by_name(மாறிலி எழுத்து *name)
{
  (வெறுமை)name;
  திரும்பு வெற்று;
}

மாறிலி கட்டமைப்பு curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (வெறுமை)id;
  திரும்பு வெற்று;
}

மாறிலி கட்டமைப்பு curl_easyoption *curl_easy_option_next(
  மாறிலி கட்டமைப்பு curl_easyoption *prev)
{
  (வெறுமை)prev;
  திரும்பு வெற்று;
}
#endif
