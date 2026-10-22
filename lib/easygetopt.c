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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "easyoptions.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_GETOPTIONS

/* Lookups easy options at runtime */
ਸਥਿਰ ਅਚਲ ਬਣਤਰ curl_easyoption *lookup(ਅਚਲ ਅੱਖਰ *name, CURLoption id)
{
  DEBUGASSERT(name || id);
  DEBUGASSERT(!Curl_easyopts_check());
  ਜੇ(name || id) {
    ਅਚਲ ਬਣਤਰ curl_easyoption *o = &Curl_easyopts[0];
    ਕਰੋ {
      ਜੇ(name) {
        ਜੇ(curl_strequal(o->name, name))
          ਵਾਪਸ o;
      }
      ਨਹੀਂਤਾਂ {
        ਜੇ((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          ਵਾਪਸ o;
      }
      o++;
    } ਜਦੋਂ(o->name);
  }
  ਵਾਪਸ ਨਿਰਾ;
}

ਅਚਲ ਬਣਤਰ curl_easyoption *curl_easy_option_by_name(ਅਚਲ ਅੱਖਰ *name)
{
  /* when name is used, the id argument is ignored */
  ਵਾਪਸ lookup(name, CURLOPT_LASTENTRY);
}

ਅਚਲ ਬਣਤਰ curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  ਵਾਪਸ lookup(ਨਿਰਾ, id);
}

/* Iterates over available options */
ਅਚਲ ਬਣਤਰ curl_easyoption *curl_easy_option_ਅਗਲਾ(
  ਅਚਲ ਬਣਤਰ curl_easyoption *prev)
{
  ਜੇ(prev && prev->name) {
    prev++;
    ਜੇ(prev->name)
      ਵਾਪਸ prev;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!prev)
    ਵਾਪਸ &Curl_easyopts[0];
  ਵਾਪਸ ਨਿਰਾ;
}

#ਨਹੀਂਤਾਂ
ਅਚਲ ਬਣਤਰ curl_easyoption *curl_easy_option_by_name(ਅਚਲ ਅੱਖਰ *name)
{
  (ਖਾਲੀ)name;
  ਵਾਪਸ ਨਿਰਾ;
}

ਅਚਲ ਬਣਤਰ curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (ਖਾਲੀ)id;
  ਵਾਪਸ ਨਿਰਾ;
}

ਅਚਲ ਬਣਤਰ curl_easyoption *curl_easy_option_ਅਗਲਾ(
  ਅਚਲ ਬਣਤਰ curl_easyoption *prev)
{
  (ਖਾਲੀ)prev;
  ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ
