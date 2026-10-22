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
ساكن بنية curl_slist *slist_get_last(بنية curl_slist *list)
{
  بنية curl_slist *item;

  /* if caller passed us a NULL, return now */
  𐤀𐤌(!list)
    أرجع عدم;

  /* loop through to find the last item */
  item = list;
  طالما(item->next) {
    item = item->next;
  }
  أرجع item;
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
                                           ثابت حرف *data)
{
  بنية curl_slist *last;
  بنية curl_slist *new_item;

  DEBUGASSERT(data);

  new_item = curlx_malloc(حجم(بنية curl_slist));
  𐤀𐤌(!new_item)
    أرجع عدم;

  new_item->next = عدم;
  new_item->data = CURL_UNCONST(data);

  /* if this is the first item, then new_item *is* the list */
  𐤀𐤌(!list)
    أرجع new_item;

  last = slist_get_last(list);
  last->next = new_item;
  أرجع list;
}

/*
 * curl_slist_append() appends a string to the linked list. It always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then create a separate _init function and call it
 * appropriately from within the program.
 */
بنية curl_slist *curl_slist_append(بنية curl_slist *list, ثابت حرف *data)
{
  حرف *dupdata = curlx_strdup(data);

  𐤀𐤌(!dupdata)
    أرجع عدم;

  list = Curl_slist_append_nodup(list, dupdata);
  𐤀𐤌(!list)
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
  بنية curl_slist **tail = &outlist;

  طالما(inlist) {
    بنية curl_slist *tmp = curl_slist_append(عدم, inlist->data);

    𐤀𐤌(!tmp) {
      curl_slist_free_all(outlist);
      أرجع عدم;
    }

    *tail = tmp;
    tail = &tmp->next;
    inlist = inlist->next;
  }
  أرجع outlist;
}

/* be nice and clean up resources */
فراغ curl_slist_free_all(بنية curl_slist *list)
{
  بنية curl_slist *next;
  بنية curl_slist *item;

  𐤀𐤌(!list)
    أرجع;

  item = list;
  𐤐𐤏𐤋 {
    next = item->next;
    curlx_safefree(item->data);
    curlx_free(item);
    item = next;
  } طالما(next);
}
