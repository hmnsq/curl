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
#include "curl_setup.h"

#include "easyoptions.h"

#ifndef CURL_DISABLE_GETOPTIONS

/* Lookups easy options at runtime */
static const struct curl_easyoption *lookup(const char *name, CURLoption id)
{
  DEBUGASSERT(name || id);
  DEBUGASSERT(!Curl_easyopts_check());
  nếu(name || id) {
    const struct curl_easyoption *o = &Curl_easyopts[0];
    do {
      nếu(name) {
        nếu(curl_strequal(o->name, name))
          trả_về o;
      }
      không_thì {
        nếu((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          trả_về o;
      }
      o++;
    } trong_khi(o->name);
  }
  trả_về NULL;
}

const struct curl_easyoption *curl_easy_option_by_name(const char *name)
{
  /* when name is used, the id argument is ignored */
  trả_về lookup(name, CURLOPT_LASTENTRY);
}

const struct curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  trả_về lookup(NULL, id);
}

/* Iterates over available options */
const struct curl_easyoption *curl_easy_option_next(
  const struct curl_easyoption *prev)
{
  nếu(prev && prev->name) {
    prev++;
    nếu(prev->name)
      trả_về prev;
  }
  không_thì nếu(!prev)
    trả_về &Curl_easyopts[0];
  trả_về NULL;
}

#không_thì
const struct curl_easyoption *curl_easy_option_by_name(const char *name)
{
  (void)name;
  trả_về NULL;
}

const struct curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (void)id;
  trả_về NULL;
}

const struct curl_easyoption *curl_easy_option_next(
  const struct curl_easyoption *prev)
{
  (void)prev;
  trả_về NULL;
}
#endif
