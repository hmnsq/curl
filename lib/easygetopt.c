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
  ꦪꦼꦤ꧀(name || id) {
    const sa_nracanaa curl_easyoption *o = &Curl_easyopts[0];
    do {
      ꦪꦼꦤ꧀(name) {
        ꦪꦼꦤ꧀(curl_strequal(o->name, name))
          ꦮꦭꦶ o;
      }
      ꦭꦶꦪ {
        ꦪꦼꦤ꧀((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          ꦮꦭꦶ o;
      }
      o++;
    } ꦱꦼꦢꦼꦭ(o->name);
  }
  ꦮꦭꦶ NULL;
}

const sa_nracanaa curl_easyoption *curl_easy_option_by_name(const var_na *name)
{
  /* when name is used, the id argument is ignored */
  ꦮꦭꦶ lookup(name, CURLOPT_LASTENTRY);
}

const sa_nracanaa curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  ꦮꦭꦶ lookup(NULL, id);
}

/* Iterates over available options */
const sa_nracanaa curl_easyoption *curl_easy_option_next(
  const sa_nracanaa curl_easyoption *prev)
{
  ꦪꦼꦤ꧀(prev && prev->name) {
    prev++;
    ꦪꦼꦤ꧀(prev->name)
      ꦮꦭꦶ prev;
  }
  ꦭꦶꦪ ꦪꦼꦤ꧀(!prev)
    ꦮꦭꦶ &Curl_easyopts[0];
  ꦮꦭꦶ NULL;
}

#ꦭꦶꦪ
const sa_nracanaa curl_easyoption *curl_easy_option_by_name(const var_na *name)
{
  (void)name;
  ꦮꦭꦶ NULL;
}

const sa_nracanaa curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (void)id;
  ꦮꦭꦶ NULL;
}

const sa_nracanaa curl_easyoption *curl_easy_option_next(
  const sa_nracanaa curl_easyoption *prev)
{
  (void)prev;
  ꦮꦭꦶ NULL;
}
#endif
