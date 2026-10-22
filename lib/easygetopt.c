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
#شامل "curl_setup.h"

#شامل "easyoptions.h"

#ifndef CURL_DISABLE_GETOPTIONS

/* Lookups easy options at runtime */
ایستا ثابت ساختار curl_easyoption *lookup(ثابت نویسه *name, CURLoption id)
{
  DEBUGASSERT(name || id);
  DEBUGASSERT(!Curl_easyopts_check());
  اگر(name || id) {
    ثابت ساختار curl_easyoption *o = &Curl_easyopts[0];
    انجامبده {
      اگر(name) {
        اگر(curl_strequal(o->name, name))
          بازگشت o;
      }
      وگرنه {
        اگر((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          بازگشت o;
      }
      o++;
    } وقتی(o->name);
  }
  بازگشت پوچ;
}

ثابت ساختار curl_easyoption *curl_easy_option_by_name(ثابت نویسه *name)
{
  /* when name is used, the id argument is ignored */
  بازگشت lookup(name, CURLOPT_LASTENTRY);
}

ثابت ساختار curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  بازگشت lookup(پوچ, id);
}

/* Iterates over available options */
ثابت ساختار curl_easyoption *curl_easy_option_next(
  ثابت ساختار curl_easyoption *prev)
{
  اگر(prev && prev->name) {
    prev++;
    اگر(prev->name)
      بازگشت prev;
  }
  وگرنه اگر(!prev)
    بازگشت &Curl_easyopts[0];
  بازگشت پوچ;
}

#وگرنه
ثابت ساختار curl_easyoption *curl_easy_option_by_name(ثابت نویسه *name)
{
  (تهی)name;
  بازگشت پوچ;
}

ثابت ساختار curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (تهی)id;
  بازگشت پوچ;
}

ثابت ساختار curl_easyoption *curl_easy_option_next(
  ثابت ساختار curl_easyoption *prev)
{
  (تهی)prev;
  بازگشت پوچ;
}
#endif
