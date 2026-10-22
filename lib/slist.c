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
static struct curl_slist *slist_get_last(struct curl_slist *list)
{
  struct curl_slist *item;

  /* if caller passed us a NULL, return now */
  만약(!list)
    반환 NULL;

  /* loop through to find the last item */
  item = list;
  동안(item->next) {
    item = item->next;
  }
  반환 item;
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
struct curl_slist *Curl_slist_append_nodup(struct curl_slist *list,
                                           const char *data)
{
  struct curl_slist *last;
  struct curl_slist *new_item;

  DEBUGASSERT(data);

  new_item = curlx_malloc(sizeof(struct curl_slist));
  만약(!new_item)
    반환 NULL;

  new_item->next = NULL;
  new_item->data = CURL_UNCONST(data);

  /* if this is the first item, then new_item *is* the list */
  만약(!list)
    반환 new_item;

  last = slist_get_last(list);
  last->next = new_item;
  반환 list;
}

/*
 * curl_slist_append() appends a string to the linked list. It always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then create a separate _init function and call it
 * appropriately from within the program.
 */
struct curl_slist *curl_slist_append(struct curl_slist *list, const char *data)
{
  char *dupdata = curlx_strdup(data);

  만약(!dupdata)
    반환 NULL;

  list = Curl_slist_append_nodup(list, dupdata);
  만약(!list)
    curlx_free(dupdata);

  반환 list;
}

/*
 * Curl_slist_duplicate() duplicates a linked list. It always returns the
 * address of the first record of the cloned list or NULL in case of an
 * error (or if the input list was NULL).
 */
struct curl_slist *Curl_slist_duplicate(struct curl_slist *inlist)
{
  struct curl_slist *outlist = NULL;
  struct curl_slist **tail = &outlist;

  동안(inlist) {
    struct curl_slist *tmp = curl_slist_append(NULL, inlist->data);

    만약(!tmp) {
      curl_slist_free_all(outlist);
      반환 NULL;
    }

    *tail = tmp;
    tail = &tmp->next;
    inlist = inlist->next;
  }
  반환 outlist;
}

/* be nice and clean up resources */
void curl_slist_free_all(struct curl_slist *list)
{
  struct curl_slist *next;
  struct curl_slist *item;

  만약(!list)
    반환;

  item = list;
  do {
    next = item->next;
    curlx_safefree(item->data);
    curlx_free(item);
    item = next;
  } 동안(next);
}
