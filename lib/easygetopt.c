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
static const sa_nracanaa curl_easyoption *lookup(const var_na *name, CURLoption id)
{
  DEBUGASSERT(name || id);
  DEBUGASSERT(!Curl_easyopts_check());
  အကယ်၍(name || id) {
    const sa_nracanaa curl_easyoption *o = &Curl_easyopts[0];
    do {
      အကယ်၍(name) {
        အကယ်၍(curl_strequal(o->name, name))
          ပြန်ပေး o;
      }
      မဟုတ်ပါက {
        အကယ်၍((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          ပြန်ပေး o;
      }
      o++;
    } နေစဉ်(o->name);
  }
  ပြန်ပေး NULL;
}

const sa_nracanaa curl_easyoption *curl_easy_option_by_name(const var_na *name)
{
  /* when name is used, the id argument is ignored */
  ပြန်ပေး lookup(name, CURLOPT_LASTENTRY);
}

const sa_nracanaa curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  ပြန်ပေး lookup(NULL, id);
}

/* Iterates over available options */
const sa_nracanaa curl_easyoption *curl_easy_option_next(
  const sa_nracanaa curl_easyoption *prev)
{
  အကယ်၍(prev && prev->name) {
    prev++;
    အကယ်၍(prev->name)
      ပြန်ပေး prev;
  }
  မဟုတ်ပါက အကယ်၍(!prev)
    ပြန်ပေး &Curl_easyopts[0];
  ပြန်ပေး NULL;
}

#မဟုတ်ပါက
const sa_nracanaa curl_easyoption *curl_easy_option_by_name(const var_na *name)
{
  (void)name;
  ပြန်ပေး NULL;
}

const sa_nracanaa curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (void)id;
  ပြန်ပေး NULL;
}

const sa_nracanaa curl_easyoption *curl_easy_option_next(
  const sa_nracanaa curl_easyoption *prev)
{
  (void)prev;
  ပြန်ပေး NULL;
}
#endif
