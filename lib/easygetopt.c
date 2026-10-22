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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "easyoptions.h"

#ifndef CURL_DISABLE_GETOPTIONS

/* Lookups easy options at runtime */
ସ୍ଥିର ଅଚଳ ସଂରଚନା curl_easyoption *lookup(ଅଚଳ ଅକ୍ଷର *name, CURLoption id)
{
  DEBUGASSERT(name || id);
  DEBUGASSERT(!Curl_easyopts_check());
  ଯଦି(name || id) {
    ଅଚଳ ସଂରଚନା curl_easyoption *o = &Curl_easyopts[0];
    କର {
      ଯଦି(name) {
        ଯଦି(curl_strequal(o->name, name))
          ଫେରନ୍ତୁ o;
      }
      ଅନ୍ୟଥା {
        ଯଦି((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          ଫେରନ୍ତୁ o;
      }
      o++;
    } ଯେତେବେଳେ(o->name);
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

ଅଚଳ ସଂରଚନା curl_easyoption *curl_easy_option_by_name(ଅଚଳ ଅକ୍ଷର *name)
{
  /* when name is used, the id argument is ignored */
  ଫେରନ୍ତୁ lookup(name, CURLOPT_LASTENTRY);
}

ଅଚଳ ସଂରଚନା curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  ଫେରନ୍ତୁ lookup(ଖାଲି, id);
}

/* Iterates over available options */
ଅଚଳ ସଂରଚନା curl_easyoption *curl_easy_option_next(
  ଅଚଳ ସଂରଚନା curl_easyoption *prev)
{
  ଯଦି(prev && prev->name) {
    prev++;
    ଯଦି(prev->name)
      ଫେରନ୍ତୁ prev;
  }
  ଅନ୍ୟଥା ଯଦି(!prev)
    ଫେରନ୍ତୁ &Curl_easyopts[0];
  ଫେରନ୍ତୁ ଖାଲି;
}

#ଅନ୍ୟଥା
ଅଚଳ ସଂରଚନା curl_easyoption *curl_easy_option_by_name(ଅଚଳ ଅକ୍ଷର *name)
{
  (ଶୂନ୍ୟ)name;
  ଫେରନ୍ତୁ ଖାଲି;
}

ଅଚଳ ସଂରଚନା curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (ଶୂନ୍ୟ)id;
  ଫେରନ୍ତୁ ଖାଲି;
}

ଅଚଳ ସଂରଚନା curl_easyoption *curl_easy_option_next(
  ଅଚଳ ସଂରଚନା curl_easyoption *prev)
{
  (ଶୂନ୍ୟ)prev;
  ଫେରନ୍ତୁ ଖାଲି;
}
#endif
