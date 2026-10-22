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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "slist.h"

/* returns last node in linked list */
ਸਥਿਰ ਬਣਤਰ curl_slist *slist_get_ਆਖਰੀ(ਬਣਤਰ curl_slist *list)
{
  ਬਣਤਰ curl_slist *ਵਸਤੂ;

  /* if caller passed us a NULL, return now */
  ਜੇ(!list)
    ਵਾਪਸ ਨਿਰਾ;

  /* loop through to find the last item */
  ਵਸਤੂ = list;
  ਜਦੋਂ(ਵਸਤੂ->ਅਗਲਾ) {
    ਵਸਤੂ = ਵਸਤੂ->ਅਗਲਾ;
  }
  ਵਾਪਸ ਵਸਤੂ;
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
ਬਣਤਰ curl_slist *Curl_slist_append_nodup(ਬਣਤਰ curl_slist *list,
                                           ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ)
{
  ਬਣਤਰ curl_slist *ਆਖਰੀ;
  ਬਣਤਰ curl_slist *new_ਵਸਤੂ;

  DEBUGASSERT(ਅੰਕੜੇ);

  new_ਵਸਤੂ = curlx_malloc(ਆਕਾਰ(ਬਣਤਰ curl_slist));
  ਜੇ(!new_ਵਸਤੂ)
    ਵਾਪਸ ਨਿਰਾ;

  new_ਵਸਤੂ->ਅਗਲਾ = ਨਿਰਾ;
  new_ਵਸਤੂ->ਅੰਕੜੇ = CURL_UNCONST(ਅੰਕੜੇ);

  /* if this is the first item, then new_item *is* the list */
  ਜੇ(!list)
    ਵਾਪਸ new_ਵਸਤੂ;

  ਆਖਰੀ = slist_get_ਆਖਰੀ(list);
  ਆਖਰੀ->ਅਗਲਾ = new_ਵਸਤੂ;
  ਵਾਪਸ list;
}

/*
 * curl_slist_append() appends a string to the linked list. It always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then create a separate _init function and call it
 * appropriately from within the program.
 */
ਬਣਤਰ curl_slist *curl_slist_append(ਬਣਤਰ curl_slist *list, ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ)
{
  ਅੱਖਰ *dupdata = curlx_strdup(ਅੰਕੜੇ);

  ਜੇ(!dupdata)
    ਵਾਪਸ ਨਿਰਾ;

  list = Curl_slist_append_nodup(list, dupdata);
  ਜੇ(!list)
    curlx_free(dupdata);

  ਵਾਪਸ list;
}

/*
 * Curl_slist_duplicate() duplicates a linked list. It always returns the
 * address of the first record of the cloned list or NULL in case of an
 * error (or if the input list was NULL).
 */
ਬਣਤਰ curl_slist *Curl_slist_duplicate(ਬਣਤਰ curl_slist *inlist)
{
  ਬਣਤਰ curl_slist *outlist = ਨਿਰਾ;
  ਬਣਤਰ curl_slist **ਪੂਛ = &outlist;

  ਜਦੋਂ(inlist) {
    ਬਣਤਰ curl_slist *tmp = curl_slist_append(ਨਿਰਾ, inlist->ਅੰਕੜੇ);

    ਜੇ(!tmp) {
      curl_slist_free_all(outlist);
      ਵਾਪਸ ਨਿਰਾ;
    }

    *ਪੂਛ = tmp;
    ਪੂਛ = &tmp->ਅਗਲਾ;
    inlist = inlist->ਅਗਲਾ;
  }
  ਵਾਪਸ outlist;
}

/* be nice and clean up resources */
ਖਾਲੀ curl_slist_free_all(ਬਣਤਰ curl_slist *list)
{
  ਬਣਤਰ curl_slist *ਅਗਲਾ;
  ਬਣਤਰ curl_slist *ਵਸਤੂ;

  ਜੇ(!list)
    ਵਾਪਸ;

  ਵਸਤੂ = list;
  ਕਰੋ {
    ਅਗਲਾ = ਵਸਤੂ->ਅਗਲਾ;
    curlx_safefree(ਵਸਤੂ->ਅੰਕੜੇ);
    curlx_free(ਵਸਤੂ);
    ਵਸਤੂ = ਅਗਲਾ;
  } ਜਦੋਂ(ਅਗਲਾ);
}
