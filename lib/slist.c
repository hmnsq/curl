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

#অন্তর্ভাব "slist.h"

/* returns last node in linked list */
স্থির সংরচনা curl_slist *slist_get_last(সংরচনা curl_slist *list)
{
  সংরচনা curl_slist *item;

  /* if caller passed us a NULL, return now */
  যদি(!list)
    প্রত্যাগম রিক্ত;

  /* loop through to find the last item */
  item = list;
  যাবত্(item->next) {
    item = item->next;
  }
  প্রত্যাগম item;
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
সংরচনা curl_slist *Curl_slist_append_nodup(সংরচনা curl_slist *list,
                                           ধ্রুব বর্ণ *data)
{
  সংরচনা curl_slist *last;
  সংরচনা curl_slist *new_item;

  DEBUGASSERT(data);

  new_item = curlx_malloc(পরিমাণ(সংরচনা curl_slist));
  যদি(!new_item)
    প্রত্যাগম রিক্ত;

  new_item->next = রিক্ত;
  new_item->data = CURL_UNCONST(data);

  /* if this is the first item, then new_item *is* the list */
  যদি(!list)
    প্রত্যাগম new_item;

  last = slist_get_last(list);
  last->next = new_item;
  প্রত্যাগম list;
}

/*
 * curl_slist_append() appends a string to the linked list. It always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then create a separate _init function and call it
 * appropriately from within the program.
 */
সংরচনা curl_slist *curl_slist_append(সংরচনা curl_slist *list, ধ্রুব বর্ণ *data)
{
  বর্ণ *dupdata = curlx_strdup(data);

  যদি(!dupdata)
    প্রত্যাগম রিক্ত;

  list = Curl_slist_append_nodup(list, dupdata);
  যদি(!list)
    curlx_free(dupdata);

  প্রত্যাগম list;
}

/*
 * Curl_slist_duplicate() duplicates a linked list. It always returns the
 * address of the first record of the cloned list or NULL in case of an
 * error (or if the input list was NULL).
 */
সংরচনা curl_slist *Curl_slist_duplicate(সংরচনা curl_slist *inlist)
{
  সংরচনা curl_slist *outlist = রিক্ত;
  সংরচনা curl_slist **tail = &outlist;

  যাবত্(inlist) {
    সংরচনা curl_slist *tmp = curl_slist_append(রিক্ত, inlist->data);

    যদি(!tmp) {
      curl_slist_free_all(outlist);
      প্রত্যাগম রিক্ত;
    }

    *tail = tmp;
    tail = &tmp->next;
    inlist = inlist->next;
  }
  প্রত্যাগম outlist;
}

/* be nice and clean up resources */
শূন্য curl_slist_free_all(সংরচনা curl_slist *list)
{
  সংরচনা curl_slist *next;
  সংরচনা curl_slist *item;

  যদি(!list)
    প্রত্যাগম;

  item = list;
  কুরু {
    next = item->next;
    curlx_safefree(item->data);
    curlx_free(item);
    item = next;
  } যাবত্(next);
}
