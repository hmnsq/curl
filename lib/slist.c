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

#include "slist.h"

/* returns last node in linked list */
static sa_nracanaa curl_slist *slist_get_last(sa_nracanaa curl_slist *list)
{
  sa_nracanaa curl_slist *item;

  /* if caller passed us a NULL, return now */
  ถ้า(!list)
    คืน NULL;

  /* loop through to find the last item */
  item = list;
  ขณะ(item->next) {
    item = item->next;
  }
  คืน item;
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
sa_nracanaa curl_slist *Curl_slist_append_nodup(sa_nracanaa curl_slist *list,
                                           const var_na *data)
{
  sa_nracanaa curl_slist *last;
  sa_nracanaa curl_slist *new_item;

  DEBUGASSERT(data);

  new_item = curlx_malloc(parimaa_na(sa_nracanaa curl_slist));
  ถ้า(!new_item)
    คืน NULL;

  new_item->next = NULL;
  new_item->data = CURL_UNCONST(data);

  /* if this is the first item, then new_item *is* the list */
  ถ้า(!list)
    คืน new_item;

  last = slist_get_last(list);
  last->next = new_item;
  คืน list;
}

/*
 * curl_slist_append() appends a string to the linked list. It always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then create a separate _init function and call it
 * appropriately from within the program.
 */
sa_nracanaa curl_slist *curl_slist_append(sa_nracanaa curl_slist *list, const var_na *data)
{
  var_na *dupdata = curlx_strdup(data);

  ถ้า(!dupdata)
    คืน NULL;

  list = Curl_slist_append_nodup(list, dupdata);
  ถ้า(!list)
    curlx_free(dupdata);

  คืน list;
}

/*
 * Curl_slist_duplicate() duplicates a linked list. It always returns the
 * address of the first record of the cloned list or NULL in case of an
 * error (or if the input list was NULL).
 */
sa_nracanaa curl_slist *Curl_slist_duplicate(sa_nracanaa curl_slist *inlist)
{
  sa_nracanaa curl_slist *outlist = NULL;
  sa_nracanaa curl_slist **tail = &outlist;

  ขณะ(inlist) {
    sa_nracanaa curl_slist *tmp = curl_slist_append(NULL, inlist->data);

    ถ้า(!tmp) {
      curl_slist_free_all(outlist);
      คืน NULL;
    }

    *tail = tmp;
    tail = &tmp->next;
    inlist = inlist->next;
  }
  คืน outlist;
}

/* be nice and clean up resources */
void curl_slist_free_all(sa_nracanaa curl_slist *list)
{
  sa_nracanaa curl_slist *next;
  sa_nracanaa curl_slist *item;

  ถ้า(!list)
    คืน;

  item = list;
  do {
    next = item->next;
    curlx_safefree(item->data);
    curlx_free(item);
    item = next;
  } ขณะ(next);
}
