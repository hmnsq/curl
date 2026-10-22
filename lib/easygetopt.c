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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "easyoptions.h"

#ifndef CURL_DISABLE_GETOPTIONS

/* Lookups easy options at runtime */
സ്ഥിതം സ്ഥിരം ഘടന curl_easyoption *lookup(സ്ഥിരം അക്ഷരം *name, CURLoption id)
{
  DEBUGASSERT(name || id);
  DEBUGASSERT(!Curl_easyopts_check());
  എങ്കിൽ(name || id) {
    സ്ഥിരം ഘടന curl_easyoption *o = &Curl_easyopts[0];
    ചെയ്യുക {
      എങ്കിൽ(name) {
        എങ്കിൽ(curl_strequal(o->name, name))
          തിരികെ o;
      }
      അല്ലെങ്കിൽ {
        എങ്കിൽ((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          തിരികെ o;
      }
      o++;
    } ആകുമ്പോൾ(o->name);
  }
  തിരികെ ശൂന്യ;
}

സ്ഥിരം ഘടന curl_easyoption *curl_easy_option_by_name(സ്ഥിരം അക്ഷരം *name)
{
  /* when name is used, the id argument is ignored */
  തിരികെ lookup(name, CURLOPT_LASTENTRY);
}

സ്ഥിരം ഘടന curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  തിരികെ lookup(ശൂന്യ, id);
}

/* Iterates over available options */
സ്ഥിരം ഘടന curl_easyoption *curl_easy_option_next(
  സ്ഥിരം ഘടന curl_easyoption *prev)
{
  എങ്കിൽ(prev && prev->name) {
    prev++;
    എങ്കിൽ(prev->name)
      തിരികെ prev;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!prev)
    തിരികെ &Curl_easyopts[0];
  തിരികെ ശൂന്യ;
}

#അല്ലെങ്കിൽ
സ്ഥിരം ഘടന curl_easyoption *curl_easy_option_by_name(സ്ഥിരം അക്ഷരം *name)
{
  (ശൂന്യം)name;
  തിരികെ ശൂന്യ;
}

സ്ഥിരം ഘടന curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (ശൂന്യം)id;
  തിരികെ ശൂന്യ;
}

സ്ഥിരം ഘടന curl_easyoption *curl_easy_option_next(
  സ്ഥിരം ഘടന curl_easyoption *prev)
{
  (ശൂന്യം)prev;
  തിരികെ ശൂന്യ;
}
#endif
