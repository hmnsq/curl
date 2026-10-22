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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "easyoptions.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_GETOPTIONS

/* Lookups easy options at runtime */
స్థిర స్థిరరాశి నిర్మాణం curl_easyoption *lookup(స్థిరరాశి అక్షరం *name, CURLoption id)
{
  DEBUGASSERT(name || id);
  DEBUGASSERT(!Curl_easyopts_check());
  ఒకవేళ(name || id) {
    స్థిరరాశి నిర్మాణం curl_easyoption *o = &Curl_easyopts[0];
    చేయి {
      ఒకవేళ(name) {
        ఒకవేళ(curl_strequal(o->name, name))
          తిరిగి o;
      }
      లేకపోతే {
        ఒకవేళ((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          తిరిగి o;
      }
      o++;
    } అయితే(o->name);
  }
  తిరిగి ఖాళీ;
}

స్థిరరాశి నిర్మాణం curl_easyoption *curl_easy_option_by_name(స్థిరరాశి అక్షరం *name)
{
  /* when name is used, the id argument is ignored */
  తిరిగి lookup(name, CURLOPT_LASTENTRY);
}

స్థిరరాశి నిర్మాణం curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  తిరిగి lookup(ఖాళీ, id);
}

/* Iterates over available options */
స్థిరరాశి నిర్మాణం curl_easyoption *curl_easy_option_తదుపరి(
  స్థిరరాశి నిర్మాణం curl_easyoption *prev)
{
  ఒకవేళ(prev && prev->name) {
    prev++;
    ఒకవేళ(prev->name)
      తిరిగి prev;
  }
  లేకపోతే ఒకవేళ(!prev)
    తిరిగి &Curl_easyopts[0];
  తిరిగి ఖాళీ;
}

#లేకపోతే
స్థిరరాశి నిర్మాణం curl_easyoption *curl_easy_option_by_name(స్థిరరాశి అక్షరం *name)
{
  (శూన్య)name;
  తిరిగి ఖాళీ;
}

స్థిరరాశి నిర్మాణం curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (శూన్య)id;
  తిరిగి ఖాళీ;
}

స్థిరరాశి నిర్మాణం curl_easyoption *curl_easy_option_తదుపరి(
  స్థిరరాశి నిర్మాణం curl_easyoption *prev)
{
  (శూన్య)prev;
  తిరిగి ఖాళీ;
}
#అయితే_ముగింపు
