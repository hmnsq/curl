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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "slist.h"

/* returns last node in linked list */
ସ୍ଥିର ସଂରଚନା curl_slist *slist_get_last(ସଂରଚନା curl_slist *list)
{
  ସଂରଚନା curl_slist *item;

  /* if caller passed us a NULL, return now */
  ଯଦି(!list)
    ଫେରନ୍ତୁ ଖାଲି;

  /* loop through to find the last item */
  item = list;
  ଯେତେବେଳେ(item->next) {
    item = item->next;
  }
  ଫେରନ୍ତୁ item;
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
ସଂରଚନା curl_slist *Curl_slist_append_nodup(ସଂରଚନା curl_slist *list,
                                           ଅଚଳ ଅକ୍ଷର *data)
{
  ସଂରଚନା curl_slist *last;
  ସଂରଚନା curl_slist *new_item;

  DEBUGASSERT(data);

  new_item = curlx_malloc(ଆକାର(ସଂରଚନା curl_slist));
  ଯଦି(!new_item)
    ଫେରନ୍ତୁ ଖାଲି;

  new_item->next = ଖାଲି;
  new_item->data = CURL_UNCONST(data);

  /* if this is the first item, then new_item *is* the list */
  ଯଦି(!list)
    ଫେରନ୍ତୁ new_item;

  last = slist_get_last(list);
  last->next = new_item;
  ଫେରନ୍ତୁ list;
}

/*
 * curl_slist_append() appends a string to the linked list. It always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then create a separate _init function and call it
 * appropriately from within the program.
 */
ସଂରଚନା curl_slist *curl_slist_append(ସଂରଚନା curl_slist *list, ଅଚଳ ଅକ୍ଷର *data)
{
  ଅକ୍ଷର *dupdata = curlx_strdup(data);

  ଯଦି(!dupdata)
    ଫେରନ୍ତୁ ଖାଲି;

  list = Curl_slist_append_nodup(list, dupdata);
  ଯଦି(!list)
    curlx_free(dupdata);

  ଫେରନ୍ତୁ list;
}

/*
 * Curl_slist_duplicate() duplicates a linked list. It always returns the
 * address of the first record of the cloned list or NULL in case of an
 * error (or if the input list was NULL).
 */
ସଂରଚନା curl_slist *Curl_slist_duplicate(ସଂରଚନା curl_slist *inlist)
{
  ସଂରଚନା curl_slist *outlist = ଖାଲି;
  ସଂରଚନା curl_slist **tail = &outlist;

  ଯେତେବେଳେ(inlist) {
    ସଂରଚନା curl_slist *tmp = curl_slist_append(ଖାଲି, inlist->data);

    ଯଦି(!tmp) {
      curl_slist_free_all(outlist);
      ଫେରନ୍ତୁ ଖାଲି;
    }

    *tail = tmp;
    tail = &tmp->next;
    inlist = inlist->next;
  }
  ଫେରନ୍ତୁ outlist;
}

/* be nice and clean up resources */
ଶୂନ୍ୟ curl_slist_free_all(ସଂରଚନା curl_slist *list)
{
  ସଂରଚନା curl_slist *next;
  ସଂରଚନା curl_slist *item;

  ଯଦି(!list)
    ଫେରନ୍ତୁ;

  item = list;
  କର {
    next = item->next;
    curlx_safefree(item->data);
    curlx_free(item);
    item = next;
  } ଯେତେବେଳେ(next);
}
