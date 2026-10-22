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
#ضمن "curl_setup.h"

#ضمن "easyoptions.h"

#ifndef CURL_DISABLE_GETOPTIONS

/* Lookups easy options at runtime */
ساكن ثابت بنية curl_easyoption *lookup(ثابت حرف *name, CURLoption id)
{
  DEBUGASSERT(name || id);
  DEBUGASSERT(!Curl_easyopts_check());
  אם(name || id) {
    ثابت بنية curl_easyoption *o = &Curl_easyopts[0];
    עשה {
      אם(name) {
        אם(curl_strequal(o->name, name))
          החזר o;
      }
      אחרת {
        אם((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          החזר o;
      }
      o++;
    } כלעוד(o->name);
  }
  החזר عدم;
}

ثابت بنية curl_easyoption *curl_easy_option_by_name(ثابت حرف *name)
{
  /* when name is used, the id argument is ignored */
  החזר lookup(name, CURLOPT_LASTENTRY);
}

ثابت بنية curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  החזר lookup(عدم, id);
}

/* Iterates over available options */
ثابت بنية curl_easyoption *curl_easy_option_next(
  ثابت بنية curl_easyoption *prev)
{
  אם(prev && prev->name) {
    prev++;
    אם(prev->name)
      החזר prev;
  }
  אחרת אם(!prev)
    החזר &Curl_easyopts[0];
  החזר عدم;
}

#אחרת
ثابت بنية curl_easyoption *curl_easy_option_by_name(ثابت حرف *name)
{
  (فراغ)name;
  החזר عدم;
}

ثابت بنية curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (فراغ)id;
  החזר عدم;
}

ثابت بنية curl_easyoption *curl_easy_option_next(
  ثابت بنية curl_easyoption *prev)
{
  (فراغ)prev;
  החזר عدم;
}
#endif
