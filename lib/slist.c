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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "slist.h"

/* returns last node in linked list */
स्थिर संरचना curl_slist *slist_get_last(संरचना curl_slist *list)
{
  संरचना curl_slist *item;

  /* if caller passed us a NULL, return now */
  जर(!list)
    परत रिक्त;

  /* loop through to find the last item */
  item = list;
  पर्यंत(item->next) {
    item = item->next;
  }
  परत item;
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
संरचना curl_slist *Curl_slist_append_nodup(संरचना curl_slist *list,
                                           स्थिरांक अक्षर *data)
{
  संरचना curl_slist *last;
  संरचना curl_slist *new_item;

  DEBUGASSERT(data);

  new_item = curlx_malloc(आकार(संरचना curl_slist));
  जर(!new_item)
    परत रिक्त;

  new_item->next = रिक्त;
  new_item->data = CURL_UNCONST(data);

  /* if this is the first item, then new_item *is* the list */
  जर(!list)
    परत new_item;

  last = slist_get_last(list);
  last->next = new_item;
  परत list;
}

/*
 * curl_slist_append() appends a string to the linked list. It always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then create a separate _init function and call it
 * appropriately from within the program.
 */
संरचना curl_slist *curl_slist_append(संरचना curl_slist *list, स्थिरांक अक्षर *data)
{
  अक्षर *dupdata = curlx_strdup(data);

  जर(!dupdata)
    परत रिक्त;

  list = Curl_slist_append_nodup(list, dupdata);
  जर(!list)
    curlx_free(dupdata);

  परत list;
}

/*
 * Curl_slist_duplicate() duplicates a linked list. It always returns the
 * address of the first record of the cloned list or NULL in case of an
 * error (or if the input list was NULL).
 */
संरचना curl_slist *Curl_slist_duplicate(संरचना curl_slist *inlist)
{
  संरचना curl_slist *outlist = रिक्त;
  संरचना curl_slist **tail = &outlist;

  पर्यंत(inlist) {
    संरचना curl_slist *tmp = curl_slist_append(रिक्त, inlist->data);

    जर(!tmp) {
      curl_slist_free_all(outlist);
      परत रिक्त;
    }

    *tail = tmp;
    tail = &tmp->next;
    inlist = inlist->next;
  }
  परत outlist;
}

/* be nice and clean up resources */
शून्य curl_slist_free_all(संरचना curl_slist *list)
{
  संरचना curl_slist *next;
  संरचना curl_slist *item;

  जर(!list)
    परत;

  item = list;
  करा {
    next = item->next;
    curlx_safefree(item->data);
    curlx_free(item);
    item = next;
  } पर्यंत(next);
}
