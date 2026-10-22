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
#شامل "curl_setup.h"

#شامل "slist.h"

/* returns last node in linked list */
ساکن ساخت curl_slist *slist_get_آخری(ساخت curl_slist *list)
{
  ساخت curl_slist *شے;

  /* if caller passed us a NULL, return now */
  اگر(!list)
    واپس تہی;

  /* loop through to find the last item */
  شے = list;
  جب(شے->اگلا) {
    شے = شے->اگلا;
  }
  واپس شے;
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
ساخت curl_slist *Curl_slist_append_nodup(ساخت curl_slist *list,
                                           ثابت حرف *ڈیٹا)
{
  ساخت curl_slist *آخری;
  ساخت curl_slist *new_شے;

  DEBUGASSERT(ڈیٹا);

  new_شے = curlx_malloc(حجم(ساخت curl_slist));
  اگر(!new_شے)
    واپس تہی;

  new_شے->اگلا = تہی;
  new_شے->ڈیٹا = CURL_UNCONST(ڈیٹا);

  /* if this is the first item, then new_item *is* the list */
  اگر(!list)
    واپس new_شے;

  آخری = slist_get_آخری(list);
  آخری->اگلا = new_شے;
  واپس list;
}

/*
 * curl_slist_append() appends a string to the linked list. It always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then create a separate _init function and call it
 * appropriately from within the program.
 */
ساخت curl_slist *curl_slist_append(ساخت curl_slist *list, ثابت حرف *ڈیٹا)
{
  حرف *dupdata = curlx_strdup(ڈیٹا);

  اگر(!dupdata)
    واپس تہی;

  list = Curl_slist_append_nodup(list, dupdata);
  اگر(!list)
    curlx_free(dupdata);

  واپس list;
}

/*
 * Curl_slist_duplicate() duplicates a linked list. It always returns the
 * address of the first record of the cloned list or NULL in case of an
 * error (or if the input list was NULL).
 */
ساخت curl_slist *Curl_slist_duplicate(ساخت curl_slist *inlist)
{
  ساخت curl_slist *outlist = تہی;
  ساخت curl_slist **دم = &outlist;

  جب(inlist) {
    ساخت curl_slist *tmp = curl_slist_append(تہی, inlist->ڈیٹا);

    اگر(!tmp) {
      curl_slist_free_all(outlist);
      واپس تہی;
    }

    *دم = tmp;
    دم = &tmp->اگلا;
    inlist = inlist->اگلا;
  }
  واپس outlist;
}

/* be nice and clean up resources */
خالی curl_slist_free_all(ساخت curl_slist *list)
{
  ساخت curl_slist *اگلا;
  ساخت curl_slist *شے;

  اگر(!list)
    واپس;

  شے = list;
  کرو {
    اگلا = شے->اگلا;
    curlx_safefree(شے->ڈیٹا);
    curlx_free(شے);
    شے = اگلا;
  } جب(اگلا);
}
