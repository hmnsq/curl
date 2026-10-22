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
  إذا(name || id) {
    ثابت بنية curl_easyoption *o = &Curl_easyopts[0];
    افعل {
      إذا(name) {
        إذا(curl_strequal(o->name, name))
          أرجع o;
      }
      وإلا {
        إذا((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          أرجع o;
      }
      o++;
    } طالما(o->name);
  }
  أرجع عدم;
}

ثابت بنية curl_easyoption *curl_easy_option_by_name(ثابت حرف *name)
{
  /* when name is used, the id argument is ignored */
  أرجع lookup(name, CURLOPT_LASTENTRY);
}

ثابت بنية curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  أرجع lookup(عدم, id);
}

/* Iterates over available options */
ثابت بنية curl_easyoption *curl_easy_option_next(
  ثابت بنية curl_easyoption *prev)
{
  إذا(prev && prev->name) {
    prev++;
    إذا(prev->name)
      أرجع prev;
  }
  وإلا إذا(!prev)
    أرجع &Curl_easyopts[0];
  أرجع عدم;
}

#وإلا
ثابت بنية curl_easyoption *curl_easy_option_by_name(ثابت حرف *name)
{
  (فراغ)name;
  أرجع عدم;
}

ثابت بنية curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (فراغ)id;
  أرجع عدم;
}

ثابت بنية curl_easyoption *curl_easy_option_next(
  ثابت بنية curl_easyoption *prev)
{
  (فراغ)prev;
  أرجع عدم;
}
#endif
