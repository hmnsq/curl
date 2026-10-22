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
  ܐܢ(name || id) {
    ثابت بنية curl_easyoption *o = &Curl_easyopts[0];
    ܥܒܕ {
      ܐܢ(name) {
        ܐܢ(curl_strequal(o->name, name))
          ܦܢܐ o;
      }
      ܐܚܪܢܐ {
        ܐܢ((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          ܦܢܐ o;
      }
      o++;
    } ܟܡܐ(o->name);
  }
  ܦܢܐ عدم;
}

ثابت بنية curl_easyoption *curl_easy_option_by_name(ثابت حرف *name)
{
  /* when name is used, the id argument is ignored */
  ܦܢܐ lookup(name, CURLOPT_LASTENTRY);
}

ثابت بنية curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  ܦܢܐ lookup(عدم, id);
}

/* Iterates over available options */
ثابت بنية curl_easyoption *curl_easy_option_next(
  ثابت بنية curl_easyoption *prev)
{
  ܐܢ(prev && prev->name) {
    prev++;
    ܐܢ(prev->name)
      ܦܢܐ prev;
  }
  ܐܚܪܢܐ ܐܢ(!prev)
    ܦܢܐ &Curl_easyopts[0];
  ܦܢܐ عدم;
}

#ܐܚܪܢܐ
ثابت بنية curl_easyoption *curl_easy_option_by_name(ثابت حرف *name)
{
  (فراغ)name;
  ܦܢܐ عدم;
}

ثابت بنية curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (فراغ)id;
  ܦܢܐ عدم;
}

ثابت بنية curl_easyoption *curl_easy_option_next(
  ثابت بنية curl_easyoption *prev)
{
  (فراغ)prev;
  ܦܢܐ عدم;
}
#endif
