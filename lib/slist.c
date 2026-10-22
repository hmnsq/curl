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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "slist.h"

/* returns last node in linked list */
நிலையான கட்டமைப்பு curl_slist *slist_get_last(கட்டமைப்பு curl_slist *list)
{
  கட்டமைப்பு curl_slist *item;

  /* if caller passed us a NULL, return now */
  எனில்(!list)
    திரும்பு வெற்று;

  /* loop through to find the last item */
  item = list;
  போது(item->next) {
    item = item->next;
  }
  திரும்பு item;
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
கட்டமைப்பு curl_slist *Curl_slist_append_nodup(கட்டமைப்பு curl_slist *list,
                                           மாறிலி எழுத்து *data)
{
  கட்டமைப்பு curl_slist *last;
  கட்டமைப்பு curl_slist *new_item;

  DEBUGASSERT(data);

  new_item = curlx_malloc(அளவு(கட்டமைப்பு curl_slist));
  எனில்(!new_item)
    திரும்பு வெற்று;

  new_item->next = வெற்று;
  new_item->data = CURL_UNCONST(data);

  /* if this is the first item, then new_item *is* the list */
  எனில்(!list)
    திரும்பு new_item;

  last = slist_get_last(list);
  last->next = new_item;
  திரும்பு list;
}

/*
 * curl_slist_append() appends a string to the linked list. It always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then create a separate _init function and call it
 * appropriately from within the program.
 */
கட்டமைப்பு curl_slist *curl_slist_append(கட்டமைப்பு curl_slist *list, மாறிலி எழுத்து *data)
{
  எழுத்து *dupdata = curlx_strdup(data);

  எனில்(!dupdata)
    திரும்பு வெற்று;

  list = Curl_slist_append_nodup(list, dupdata);
  எனில்(!list)
    curlx_free(dupdata);

  திரும்பு list;
}

/*
 * Curl_slist_duplicate() duplicates a linked list. It always returns the
 * address of the first record of the cloned list or NULL in case of an
 * error (or if the input list was NULL).
 */
கட்டமைப்பு curl_slist *Curl_slist_duplicate(கட்டமைப்பு curl_slist *inlist)
{
  கட்டமைப்பு curl_slist *outlist = வெற்று;
  கட்டமைப்பு curl_slist **tail = &outlist;

  போது(inlist) {
    கட்டமைப்பு curl_slist *tmp = curl_slist_append(வெற்று, inlist->data);

    எனில்(!tmp) {
      curl_slist_free_all(outlist);
      திரும்பு வெற்று;
    }

    *tail = tmp;
    tail = &tmp->next;
    inlist = inlist->next;
  }
  திரும்பு outlist;
}

/* be nice and clean up resources */
வெறுமை curl_slist_free_all(கட்டமைப்பு curl_slist *list)
{
  கட்டமைப்பு curl_slist *next;
  கட்டமைப்பு curl_slist *item;

  எனில்(!list)
    திரும்பு;

  item = list;
  செய் {
    next = item->next;
    curlx_safefree(item->data);
    curlx_free(item);
    item = next;
  } போது(next);
}
