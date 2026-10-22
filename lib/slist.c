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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "slist.h"

/* returns last node in linked list */
ಸ್ಥಿರ ಸಂರಚನಾ curl_slist *slist_get_last(ಸಂರಚನಾ curl_slist *list)
{
  ಸಂರಚನಾ curl_slist *item;

  /* if caller passed us a NULL, return now */
  ಯದಿ(!list)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  /* loop through to find the last item */
  item = list;
  ಯಾವತ್(item->next) {
    item = item->next;
  }
  ಪ್ರತ್ಯಾಗಮ item;
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
ಸಂರಚನಾ curl_slist *Curl_slist_append_nodup(ಸಂರಚನಾ curl_slist *list,
                                           ಧ್ರುವ ವರ್ಣ *data)
{
  ಸಂರಚನಾ curl_slist *last;
  ಸಂರಚನಾ curl_slist *new_item;

  DEBUGASSERT(data);

  new_item = curlx_malloc(ಪರಿಮಾಣ(ಸಂರಚನಾ curl_slist));
  ಯದಿ(!new_item)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  new_item->next = ರಿಕ್ತ;
  new_item->data = CURL_UNCONST(data);

  /* if this is the first item, then new_item *is* the list */
  ಯದಿ(!list)
    ಪ್ರತ್ಯಾಗಮ new_item;

  last = slist_get_last(list);
  last->next = new_item;
  ಪ್ರತ್ಯಾಗಮ list;
}

/*
 * curl_slist_append() appends a string to the linked list. It always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then create a separate _init function and call it
 * appropriately from within the program.
 */
ಸಂರಚನಾ curl_slist *curl_slist_append(ಸಂರಚನಾ curl_slist *list, ಧ್ರುವ ವರ್ಣ *data)
{
  ವರ್ಣ *dupdata = curlx_strdup(data);

  ಯದಿ(!dupdata)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  list = Curl_slist_append_nodup(list, dupdata);
  ಯದಿ(!list)
    curlx_free(dupdata);

  ಪ್ರತ್ಯಾಗಮ list;
}

/*
 * Curl_slist_duplicate() duplicates a linked list. It always returns the
 * address of the first record of the cloned list or NULL in case of an
 * error (or if the input list was NULL).
 */
ಸಂರಚನಾ curl_slist *Curl_slist_duplicate(ಸಂರಚನಾ curl_slist *inlist)
{
  ಸಂರಚನಾ curl_slist *outlist = ರಿಕ್ತ;
  ಸಂರಚನಾ curl_slist **tail = &outlist;

  ಯಾವತ್(inlist) {
    ಸಂರಚನಾ curl_slist *tmp = curl_slist_append(ರಿಕ್ತ, inlist->data);

    ಯದಿ(!tmp) {
      curl_slist_free_all(outlist);
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    *tail = tmp;
    tail = &tmp->next;
    inlist = inlist->next;
  }
  ಪ್ರತ್ಯಾಗಮ outlist;
}

/* be nice and clean up resources */
ಶೂನ್ಯ curl_slist_free_all(ಸಂರಚನಾ curl_slist *list)
{
  ಸಂರಚನಾ curl_slist *next;
  ಸಂರಚನಾ curl_slist *item;

  ಯದಿ(!list)
    ಪ್ರತ್ಯಾಗಮ;

  item = list;
  ಕುರು {
    next = item->next;
    curlx_safefree(item->data);
    curlx_free(item);
    item = next;
  } ಯಾವತ್(next);
}
