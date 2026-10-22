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

#અન્તર્ભાવ "slist.h"

/* returns last node in linked list */
સ્થિર સંરચના curl_slist *slist_get_last(સંરચના curl_slist *list)
{
  સંરચના curl_slist *item;

  /* if caller passed us a NULL, return now */
  જો(!list)
    પાછા ખાલી;

  /* loop through to find the last item */
  item = list;
  જ્યારે(item->next) {
    item = item->next;
  }
  પાછા item;
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
સંરચના curl_slist *Curl_slist_append_nodup(સંરચના curl_slist *list,
                                           અચલ અક્ષર *data)
{
  સંરચના curl_slist *last;
  સંરચના curl_slist *new_item;

  DEBUGASSERT(data);

  new_item = curlx_malloc(કદ(સંરચના curl_slist));
  જો(!new_item)
    પાછા ખાલી;

  new_item->next = ખાલી;
  new_item->data = CURL_UNCONST(data);

  /* if this is the first item, then new_item *is* the list */
  જો(!list)
    પાછા new_item;

  last = slist_get_last(list);
  last->next = new_item;
  પાછા list;
}

/*
 * curl_slist_append() appends a string to the linked list. It always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then create a separate _init function and call it
 * appropriately from within the program.
 */
સંરચના curl_slist *curl_slist_append(સંરચના curl_slist *list, અચલ અક્ષર *data)
{
  અક્ષર *dupdata = curlx_strdup(data);

  જો(!dupdata)
    પાછા ખાલી;

  list = Curl_slist_append_nodup(list, dupdata);
  જો(!list)
    curlx_free(dupdata);

  પાછા list;
}

/*
 * Curl_slist_duplicate() duplicates a linked list. It always returns the
 * address of the first record of the cloned list or NULL in case of an
 * error (or if the input list was NULL).
 */
સંરચના curl_slist *Curl_slist_duplicate(સંરચના curl_slist *inlist)
{
  સંરચના curl_slist *outlist = ખાલી;
  સંરચના curl_slist **tail = &outlist;

  જ્યારે(inlist) {
    સંરચના curl_slist *tmp = curl_slist_append(ખાલી, inlist->data);

    જો(!tmp) {
      curl_slist_free_all(outlist);
      પાછા ખાલી;
    }

    *tail = tmp;
    tail = &tmp->next;
    inlist = inlist->next;
  }
  પાછા outlist;
}

/* be nice and clean up resources */
શૂન્ય curl_slist_free_all(સંરચના curl_slist *list)
{
  સંરચના curl_slist *next;
  સંરચના curl_slist *item;

  જો(!list)
    પાછા;

  item = list;
  કરો {
    next = item->next;
    curlx_safefree(item->data);
    curlx_free(item);
    item = next;
  } જ્યારે(next);
}
