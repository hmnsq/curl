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
  ᬬᬾᬦ᭄(name || id) {
    const sa_nracanaa curl_easyoption *o = &Curl_easyopts[0];
    do {
      ᬬᬾᬦ᭄(name) {
        ᬬᬾᬦ᭄(curl_strequal(o->name, name))
          ᬯᬮᬶ o;
      }
      ᬮᬶᬬ {
        ᬬᬾᬦ᭄((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          ᬯᬮᬶ o;
      }
      o++;
    } ᬲᭂᬤᭂᬮ(o->name);
  }
  ᬯᬮᬶ NULL;
}

const sa_nracanaa curl_easyoption *curl_easy_option_by_name(const var_na *name)
{
  /* when name is used, the id argument is ignored */
  ᬯᬮᬶ lookup(name, CURLOPT_LASTENTRY);
}

const sa_nracanaa curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  ᬯᬮᬶ lookup(NULL, id);
}

/* Iterates over available options */
const sa_nracanaa curl_easyoption *curl_easy_option_next(
  const sa_nracanaa curl_easyoption *prev)
{
  ᬬᬾᬦ᭄(prev && prev->name) {
    prev++;
    ᬬᬾᬦ᭄(prev->name)
      ᬯᬮᬶ prev;
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(!prev)
    ᬯᬮᬶ &Curl_easyopts[0];
  ᬯᬮᬶ NULL;
}

#ᬮᬶᬬ
const sa_nracanaa curl_easyoption *curl_easy_option_by_name(const var_na *name)
{
  (void)name;
  ᬯᬮᬶ NULL;
}

const sa_nracanaa curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (void)id;
  ᬯᬮᬶ NULL;
}

const sa_nracanaa curl_easyoption *curl_easy_option_next(
  const sa_nracanaa curl_easyoption *prev)
{
  (void)prev;
  ᬯᬮᬶ NULL;
}
#endif
