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
স্থির ধ্রুবক কাঠামো curl_easyoption *lookup(ধ্রুবক অক্ষর *name, CURLoption id)
{
  DEBUGASSERT(name || id);
  DEBUGASSERT(!Curl_easyopts_check());
  যদি(name || id) {
    ধ্রুবক কাঠামো curl_easyoption *o = &Curl_easyopts[0];
    করো {
      যদি(name) {
        যদি(curl_strequal(o->name, name))
          ফেরত o;
      }
      অন্যথা {
        যদি((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          ফেরত o;
      }
      o++;
    } যখন(o->name);
  }
  ফেরত ফাঁকা;
}

ধ্রুবক কাঠামো curl_easyoption *curl_easy_option_by_name(ধ্রুবক অক্ষর *name)
{
  /* when name is used, the id argument is ignored */
  ফেরত lookup(name, CURLOPT_LASTENTRY);
}

ধ্রুবক কাঠামো curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  ফেরত lookup(ফাঁকা, id);
}

/* Iterates over available options */
ধ্রুবক কাঠামো curl_easyoption *curl_easy_option_পরবর্তী(
  ধ্রুবক কাঠামো curl_easyoption *পূর্ব)
{
  যদি(পূর্ব && পূর্ব->name) {
    পূর্ব++;
    যদি(পূর্ব->name)
      ফেরত পূর্ব;
  }
  অন্যথা যদি(!পূর্ব)
    ফেরত &Curl_easyopts[0];
  ফেরত ফাঁকা;
}

#অন্যথা
ধ্রুবক কাঠামো curl_easyoption *curl_easy_option_by_name(ধ্রুবক অক্ষর *name)
{
  (শূন্য)name;
  ফেরত ফাঁকা;
}

ধ্রুবক কাঠামো curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (শূন্য)id;
  ফেরত ফাঁকা;
}

ধ্রুবক কাঠামো curl_easyoption *curl_easy_option_পরবর্তী(
  ধ্রুবক কাঠামো curl_easyoption *পূর্ব)
{
  (শূন্য)পূর্ব;
  ফেরত ফাঁকা;
}
#endif
