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
স্থির কাঠামো curl_slist *slist_get_last(কাঠামো curl_slist *list)
{
  কাঠামো curl_slist *item;

  /* if caller passed us a NULL, return now */
  যদি(!list)
    ফেরত ফাঁকা;

  /* loop through to find the last item */
  item = list;
  যখন(item->পরবর্তী) {
    item = item->পরবর্তী;
  }
  ফেরত item;
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
কাঠামো curl_slist *Curl_slist_append_nodup(কাঠামো curl_slist *list,
                                           ধ্রুবক অক্ষর *তথ্য)
{
  কাঠামো curl_slist *last;
  কাঠামো curl_slist *new_item;

  DEBUGASSERT(তথ্য);

  new_item = curlx_malloc(আকার(কাঠামো curl_slist));
  যদি(!new_item)
    ফেরত ফাঁকা;

  new_item->পরবর্তী = ফাঁকা;
  new_item->তথ্য = CURL_UNCONST(তথ্য);

  /* if this is the first item, then new_item *is* the list */
  যদি(!list)
    ফেরত new_item;

  last = slist_get_last(list);
  last->পরবর্তী = new_item;
  ফেরত list;
}

/*
 * curl_slist_append() appends a string to the linked list. It always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then create a separate _init function and call it
 * appropriately from within the program.
 */
কাঠামো curl_slist *curl_slist_append(কাঠামো curl_slist *list, ধ্রুবক অক্ষর *তথ্য)
{
  অক্ষর *dupdata = curlx_strdup(তথ্য);

  যদি(!dupdata)
    ফেরত ফাঁকা;

  list = Curl_slist_append_nodup(list, dupdata);
  যদি(!list)
    curlx_free(dupdata);

  ফেরত list;
}

/*
 * Curl_slist_duplicate() duplicates a linked list. It always returns the
 * address of the first record of the cloned list or NULL in case of an
 * error (or if the input list was NULL).
 */
কাঠামো curl_slist *Curl_slist_duplicate(কাঠামো curl_slist *inlist)
{
  কাঠামো curl_slist *outlist = ফাঁকা;
  কাঠামো curl_slist **লেজ = &outlist;

  যখন(inlist) {
    কাঠামো curl_slist *tmp = curl_slist_append(ফাঁকা, inlist->তথ্য);

    যদি(!tmp) {
      curl_slist_free_all(outlist);
      ফেরত ফাঁকা;
    }

    *লেজ = tmp;
    লেজ = &tmp->পরবর্তী;
    inlist = inlist->পরবর্তী;
  }
  ফেরত outlist;
}

/* be nice and clean up resources */
শূন্য curl_slist_free_all(কাঠামো curl_slist *list)
{
  কাঠামো curl_slist *পরবর্তী;
  কাঠামো curl_slist *item;

  যদি(!list)
    ফেরত;

  item = list;
  করো {
    পরবর্তী = item->পরবর্তী;
    curlx_safefree(item->তথ্য);
    curlx_free(item);
    item = পরবর্তী;
  } যখন(পরবর্তী);
}
