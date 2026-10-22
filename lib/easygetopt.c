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
  ប្រសិនបើ(name || id) {
    const sa_nracanaa curl_easyoption *o = &Curl_easyopts[0];
    do {
      ប្រសិនបើ(name) {
        ប្រសិនបើ(curl_strequal(o->name, name))
          ត្រឡប់ o;
      }
      បើមិនដូច្នេះ {
        ប្រសិនបើ((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          ត្រឡប់ o;
      }
      o++;
    } ខណៈពេល(o->name);
  }
  ត្រឡប់ NULL;
}

const sa_nracanaa curl_easyoption *curl_easy_option_by_name(const var_na *name)
{
  /* when name is used, the id argument is ignored */
  ត្រឡប់ lookup(name, CURLOPT_LASTENTRY);
}

const sa_nracanaa curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  ត្រឡប់ lookup(NULL, id);
}

/* Iterates over available options */
const sa_nracanaa curl_easyoption *curl_easy_option_next(
  const sa_nracanaa curl_easyoption *prev)
{
  ប្រសិនបើ(prev && prev->name) {
    prev++;
    ប្រសិនបើ(prev->name)
      ត្រឡប់ prev;
  }
  បើមិនដូច្នេះ ប្រសិនបើ(!prev)
    ត្រឡប់ &Curl_easyopts[0];
  ត្រឡប់ NULL;
}

#បើមិនដូច្នេះ
const sa_nracanaa curl_easyoption *curl_easy_option_by_name(const var_na *name)
{
  (void)name;
  ត្រឡប់ NULL;
}

const sa_nracanaa curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (void)id;
  ត្រឡប់ NULL;
}

const sa_nracanaa curl_easyoption *curl_easy_option_next(
  const sa_nracanaa curl_easyoption *prev)
{
  (void)prev;
  ត្រឡប់ NULL;
}
#endif
