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
#ضمن "curl_setup.h"

#ضمن "slist.h"

/* returns last node in linked list */
ساكن بنية curl_slist *slist_get_وروستی(بنية curl_slist *list)
{
  بنية curl_slist *توکی;

  /* if caller passed us a NULL, return now */
  إذا(!list)
    أرجع عدم;

  /* loop through to find the last item */
  توکی = list;
  طالما(توکی->بل) {
    توکی = توکی->بل;
  }
  أرجع توکی;
}

/*
 * Curl_slist_append_nodup() appends a string to the linked list. Rather than
 * copying the string in dynamic storage, it takes its ownership. The string
 * should have been malloc()ated. Curl_slist_append_nodup always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function.
 * If an error occurs, NULL is returned and the string argument is NOT
 * released.
 */
بنية curl_slist *Curl_slist_append_nodup(بنية curl_slist *list,
                                           ثابت حرف *معلومات)
{
  بنية curl_slist *وروستی;
  بنية curl_slist *new_توکی;

  DEBUGASSERT(معلومات);

  new_توکی = curlx_malloc(حجم(بنية curl_slist));
  إذا(!new_توکی)
    أرجع عدم;

  new_توکی->بل = عدم;
  new_توکی->معلومات = CURL_UNCONST(معلومات);

  /* if this is the first item, then new_item *is* the list */
  إذا(!list)
    أرجع new_توکی;

  وروستی = slist_get_وروستی(list);
  وروستی->بل = new_توکی;
  أرجع list;
}

/*
 * curl_slist_append() appends a string to the linked list. It always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then create a separate _init function and call it
 * appropriately from within the program.
 */
بنية curl_slist *curl_slist_append(بنية curl_slist *list, ثابت حرف *معلومات)
{
  حرف *dupdata = curlx_strdup(معلومات);

  إذا(!dupdata)
    أرجع عدم;

  list = Curl_slist_append_nodup(list, dupdata);
  إذا(!list)
    curlx_free(dupdata);

  أرجع list;
}

/*
 * Curl_slist_duplicate() duplicates a linked list. It always returns the
 * address of the first record of the cloned list or NULL in case of an
 * error (or if the input list was NULL).
 */
بنية curl_slist *Curl_slist_duplicate(بنية curl_slist *inlist)
{
  بنية curl_slist *outlist = عدم;
  بنية curl_slist **لکۍ = &outlist;

  طالما(inlist) {
    بنية curl_slist *tmp = curl_slist_append(عدم, inlist->معلومات);

    إذا(!tmp) {
      curl_slist_free_all(outlist);
      أرجع عدم;
    }

    *لکۍ = tmp;
    لکۍ = &tmp->بل;
    inlist = inlist->بل;
  }
  أرجع outlist;
}

/* be nice and clean up resources */
فراغ curl_slist_free_all(بنية curl_slist *list)
{
  بنية curl_slist *بل;
  بنية curl_slist *توکی;

  إذا(!list)
    أرجع;

  توکی = list;
  افعل {
    بل = توکی->بل;
    curlx_safefree(توکی->معلومات);
    curlx_free(توکی);
    توکی = بل;
  } طالما(بل);
}
