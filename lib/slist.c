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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "slist.h"

/* returns last node in linked list */
സ്ഥിതം ഘടന curl_slist *slist_get_last(ഘടന curl_slist *list)
{
  ഘടന curl_slist *item;

  /* if caller passed us a NULL, return now */
  എങ്കിൽ(!list)
    തിരികെ ശൂന്യ;

  /* loop through to find the last item */
  item = list;
  ആകുമ്പോൾ(item->next) {
    item = item->next;
  }
  തിരികെ item;
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
ഘടന curl_slist *Curl_slist_append_nodup(ഘടന curl_slist *list,
                                           സ്ഥിരം അക്ഷരം *data)
{
  ഘടന curl_slist *last;
  ഘടന curl_slist *new_item;

  DEBUGASSERT(data);

  new_item = curlx_malloc(വലുപ്പം(ഘടന curl_slist));
  എങ്കിൽ(!new_item)
    തിരികെ ശൂന്യ;

  new_item->next = ശൂന്യ;
  new_item->data = CURL_UNCONST(data);

  /* if this is the first item, then new_item *is* the list */
  എങ്കിൽ(!list)
    തിരികെ new_item;

  last = slist_get_last(list);
  last->next = new_item;
  തിരികെ list;
}

/*
 * curl_slist_append() appends a string to the linked list. It always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then create a separate _init function and call it
 * appropriately from within the program.
 */
ഘടന curl_slist *curl_slist_append(ഘടന curl_slist *list, സ്ഥിരം അക്ഷരം *data)
{
  അക്ഷരം *dupdata = curlx_strdup(data);

  എങ്കിൽ(!dupdata)
    തിരികെ ശൂന്യ;

  list = Curl_slist_append_nodup(list, dupdata);
  എങ്കിൽ(!list)
    curlx_free(dupdata);

  തിരികെ list;
}

/*
 * Curl_slist_duplicate() duplicates a linked list. It always returns the
 * address of the first record of the cloned list or NULL in case of an
 * error (or if the input list was NULL).
 */
ഘടന curl_slist *Curl_slist_duplicate(ഘടന curl_slist *inlist)
{
  ഘടന curl_slist *outlist = ശൂന്യ;
  ഘടന curl_slist **tail = &outlist;

  ആകുമ്പോൾ(inlist) {
    ഘടന curl_slist *tmp = curl_slist_append(ശൂന്യ, inlist->data);

    എങ്കിൽ(!tmp) {
      curl_slist_free_all(outlist);
      തിരികെ ശൂന്യ;
    }

    *tail = tmp;
    tail = &tmp->next;
    inlist = inlist->next;
  }
  തിരികെ outlist;
}

/* be nice and clean up resources */
ശൂന്യം curl_slist_free_all(ഘടന curl_slist *list)
{
  ഘടന curl_slist *next;
  ഘടന curl_slist *item;

  എങ്കിൽ(!list)
    തിരികെ;

  item = list;
  ചെയ്യുക {
    next = item->next;
    curlx_safefree(item->data);
    curlx_free(item);
    item = next;
  } ആകുമ്പോൾ(next);
}
