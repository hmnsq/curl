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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "easyoptions.h"

#ifndef CURL_DISABLE_GETOPTIONS

/* Lookups easy options at runtime */
স্থির ধ্রুব সংরচনা curl_easyoption *lookup(ধ্রুব বর্ণ *name, CURLoption id)
{
  DEBUGASSERT(name || id);
  DEBUGASSERT(!Curl_easyopts_check());
  যদি(name || id) {
    ধ্রুব সংরচনা curl_easyoption *o = &Curl_easyopts[0];
    কুরু {
      যদি(name) {
        যদি(curl_strequal(o->name, name))
          প্রত্যাগম o;
      }
      অন্যথা {
        যদি((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          প্রত্যাগম o;
      }
      o++;
    } যাবত্(o->name);
  }
  প্রত্যাগম রিক্ত;
}

ধ্রুব সংরচনা curl_easyoption *curl_easy_option_by_name(ধ্রুব বর্ণ *name)
{
  /* when name is used, the id argument is ignored */
  প্রত্যাগম lookup(name, CURLOPT_LASTENTRY);
}

ধ্রুব সংরচনা curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  প্রত্যাগম lookup(রিক্ত, id);
}

/* Iterates over available options */
ধ্রুব সংরচনা curl_easyoption *curl_easy_option_next(
  ধ্রুব সংরচনা curl_easyoption *prev)
{
  যদি(prev && prev->name) {
    prev++;
    যদি(prev->name)
      প্রত্যাগম prev;
  }
  অন্যথা যদি(!prev)
    প্রত্যাগম &Curl_easyopts[0];
  প্রত্যাগম রিক্ত;
}

#অন্যথা
ধ্রুব সংরচনা curl_easyoption *curl_easy_option_by_name(ধ্রুব বর্ণ *name)
{
  (শূন্য)name;
  প্রত্যাগম রিক্ত;
}

ধ্রুব সংরচনা curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (শূন্য)id;
  প্রত্যাগম রিক্ত;
}

ধ্রুব সংরচনা curl_easyoption *curl_easy_option_next(
  ধ্রুব সংরচনা curl_easyoption *prev)
{
  (শূন্য)prev;
  প্রত্যাগম রিক্ত;
}
#endif
