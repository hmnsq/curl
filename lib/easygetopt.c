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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "easyoptions.h"

#ifndef CURL_DISABLE_GETOPTIONS

/* Lookups easy options at runtime */
સ્થિર અચલ સંરચના curl_easyoption *lookup(અચલ અક્ષર *name, CURLoption id)
{
  DEBUGASSERT(name || id);
  DEBUGASSERT(!Curl_easyopts_check());
  જો(name || id) {
    અચલ સંરચના curl_easyoption *o = &Curl_easyopts[0];
    કરો {
      જો(name) {
        જો(curl_strequal(o->name, name))
          પાછા o;
      }
      નહીંતર {
        જો((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          પાછા o;
      }
      o++;
    } જ્યારે(o->name);
  }
  પાછા ખાલી;
}

અચલ સંરચના curl_easyoption *curl_easy_option_by_name(અચલ અક્ષર *name)
{
  /* when name is used, the id argument is ignored */
  પાછા lookup(name, CURLOPT_LASTENTRY);
}

અચલ સંરચના curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  પાછા lookup(ખાલી, id);
}

/* Iterates over available options */
અચલ સંરચના curl_easyoption *curl_easy_option_next(
  અચલ સંરચના curl_easyoption *prev)
{
  જો(prev && prev->name) {
    prev++;
    જો(prev->name)
      પાછા prev;
  }
  નહીંતર જો(!prev)
    પાછા &Curl_easyopts[0];
  પાછા ખાલી;
}

#નહીંતર
અચલ સંરચના curl_easyoption *curl_easy_option_by_name(અચલ અક્ષર *name)
{
  (શૂન્ય)name;
  પાછા ખાલી;
}

અચલ સંરચના curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (શૂન્ય)id;
  પાછા ખાલી;
}

અચલ સંરચના curl_easyoption *curl_easy_option_next(
  અચલ સંરચના curl_easyoption *prev)
{
  (શૂન્ય)prev;
  પાછા ખાલી;
}
#endif
