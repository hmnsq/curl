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
ایستا ساختار curl_slist *slist_get_last(ساختار curl_slist *list)
{
  ساختار curl_slist *item;

  /* if caller passed us a NULL, return now */
  اگر(!list)
    بازگشت پوچ;

  /* loop through to find the last item */
  item = list;
  وقتی(item->next) {
    item = item->next;
  }
  بازگشت item;
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
ساختار curl_slist *Curl_slist_append_nodup(ساختار curl_slist *list,
                                           ثابت نویسه *data)
{
  ساختار curl_slist *last;
  ساختار curl_slist *new_item;

  DEBUGASSERT(data);

  new_item = curlx_malloc(اندازه(ساختار curl_slist));
  اگر(!new_item)
    بازگشت پوچ;

  new_item->next = پوچ;
  new_item->data = CURL_UNCONST(data);

  /* if this is the first item, then new_item *is* the list */
  اگر(!list)
    بازگشت new_item;

  last = slist_get_last(list);
  last->next = new_item;
  بازگشت list;
}

/*
 * curl_slist_append() appends a string to the linked list. It always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then create a separate _init function and call it
 * appropriately from within the program.
 */
ساختار curl_slist *curl_slist_append(ساختار curl_slist *list, ثابت نویسه *data)
{
  نویسه *dupdata = curlx_strdup(data);

  اگر(!dupdata)
    بازگشت پوچ;

  list = Curl_slist_append_nodup(list, dupdata);
  اگر(!list)
    curlx_free(dupdata);

  بازگشت list;
}

/*
 * Curl_slist_duplicate() duplicates a linked list. It always returns the
 * address of the first record of the cloned list or NULL in case of an
 * error (or if the input list was NULL).
 */
ساختار curl_slist *Curl_slist_duplicate(ساختار curl_slist *inlist)
{
  ساختار curl_slist *outlist = پوچ;
  ساختار curl_slist **tail = &outlist;

  وقتی(inlist) {
    ساختار curl_slist *tmp = curl_slist_append(پوچ, inlist->data);

    اگر(!tmp) {
      curl_slist_free_all(outlist);
      بازگشت پوچ;
    }

    *tail = tmp;
    tail = &tmp->next;
    inlist = inlist->next;
  }
  بازگشت outlist;
}

/* be nice and clean up resources */
تهی curl_slist_free_all(ساختار curl_slist *list)
{
  ساختار curl_slist *next;
  ساختار curl_slist *item;

  اگر(!list)
    بازگشت;

  item = list;
  انجامبده {
    next = item->next;
    curlx_safefree(item->data);
    curlx_free(item);
    item = next;
  } وقتی(next);
}
