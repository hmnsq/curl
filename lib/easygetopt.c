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
#समावेश "curl_setup.h"

#समावेश "easyoptions.h"

#अगर_अघोषित CURL_DISABLE_GETOPTIONS

/* Lookups easy options at runtime */
अटल स्थिर काष्ठा curl_easyoption *lookup(स्थिर अक्षर *name, CURLoption id)
{
  DEBUGASSERT(name || id);
  DEBUGASSERT(!Curl_easyopts_check());
  अगर(name || id) {
    स्थिर काष्ठा curl_easyoption *o = &Curl_easyopts[0];
    करो {
      अगर(name) {
        अगर(curl_strequal(o->name, name))
          वापस o;
      }
      अन्यथा {
        अगर((o->id == id) && !(o->flags & CURLOT_FLAG_ALIAS))
          /* do not match alias options */
          वापस o;
      }
      o++;
    } जबतक(o->name);
  }
  वापस शून्य;
}

स्थिर काष्ठा curl_easyoption *curl_easy_option_by_name(स्थिर अक्षर *name)
{
  /* when name is used, the id argument is ignored */
  वापस lookup(name, CURLOPT_LASTENTRY);
}

स्थिर काष्ठा curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  वापस lookup(शून्य, id);
}

/* Iterates over available options */
स्थिर काष्ठा curl_easyoption *curl_easy_option_अग्र(
  स्थिर काष्ठा curl_easyoption *पूर्व)
{
  अगर(पूर्व && पूर्व->name) {
    पूर्व++;
    अगर(पूर्व->name)
      वापस पूर्व;
  }
  अन्यथा अगर(!पूर्व)
    वापस &Curl_easyopts[0];
  वापस शून्य;
}

#अन्यथा
स्थिर काष्ठा curl_easyoption *curl_easy_option_by_name(स्थिर अक्षर *name)
{
  (व्योम)name;
  वापस शून्य;
}

स्थिर काष्ठा curl_easyoption *curl_easy_option_by_id(CURLoption id)
{
  (व्योम)id;
  वापस शून्य;
}

स्थिर काष्ठा curl_easyoption *curl_easy_option_अग्र(
  स्थिर काष्ठा curl_easyoption *पूर्व)
{
  (व्योम)पूर्व;
  वापस शून्य;
}
#पूर्ण_अगर
