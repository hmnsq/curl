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

#اگر_غیر_تعریف_شدہ CURL_DISABLE_GETOPTIONS

/* Lookups easy options at runtime */
ساکن ثابت ساخت curl_easyoption *lookup(ثابت حرف *name, CURLoption id)
{
  DEBUGASSERT(name || id);
  DEBUGASSERT(!Curl_easyopts_check());
  اگر(name || id) {
    ثابت ساخت curl_easyoption *o = &Curl_easyopts[0];
    کرو {
      اگر(name) {
        اگر(curl_strequal(o->name, name))
          واپس o;
      }
      ورنہ {
        اگر((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          واپس o;
      }
      o++;
    } جب(o->name);
  }
  واپس تہی;
}

ثابت ساخت curl_easyoption *curl_easy_option_by_name(ثابت حرف *name)
{
  /* when name is used, the id argument is ignored */
  واپس lookup(name, CURLOPT_LASTENTRY);
}

ثابت ساخت curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  واپس lookup(تہی, id);
}

/* Iterates over available options */
ثابت ساخت curl_easyoption *curl_easy_option_اگلا(
  ثابت ساخت curl_easyoption *پچھلا)
{
  اگر(پچھلا && پچھلا->name) {
    پچھلا++;
    اگر(پچھلا->name)
      واپس پچھلا;
  }
  ورنہ اگر(!پچھلا)
    واپس &Curl_easyopts[0];
  واپس تہی;
}

#ورنہ
ثابت ساخت curl_easyoption *curl_easy_option_by_name(ثابت حرف *name)
{
  (خالی)name;
  واپس تہی;
}

ثابت ساخت curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (خالی)id;
  واپس تہی;
}

ثابت ساخت curl_easyoption *curl_easy_option_اگلا(
  ثابت ساخت curl_easyoption *پچھلا)
{
  (خالی)پچھلا;
  واپس تہی;
}
#اگر_ختم
