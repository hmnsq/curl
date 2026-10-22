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
स्थिर संरचना curl_slist *slist_get_चरमः(संरचना curl_slist *list)
{
  संरचना curl_slist *item;

  /* if caller passed us a NULL, return now */
  यदि(!list)
    प्रत्यागम रिक्त;

  /* loop through to find the last item */
  item = list;
  यावत्(item->अग्र) {
    item = item->अग्र;
  }
  प्रत्यागम item;
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
                                           ध्रुव वर्ण *दत्त)
{
  संरचना curl_slist *चरमः;
  संरचना curl_slist *new_item;

  DEBUGASSERT(दत्त);

  new_item = curlx_malloc(परिमाण(संरचना curl_slist));
  यदि(!new_item)
    प्रत्यागम रिक्त;

  new_item->अग्र = रिक्त;
  new_item->दत्त = CURL_UNCONST(दत्त);

  /* if this is the first item, then new_item *is* the list */
  यदि(!list)
    प्रत्यागम new_item;

  चरमः = slist_get_चरमः(list);
  चरमः->अग्र = new_item;
  प्रत्यागम list;
}

/*
 * curl_slist_append() appends a string to the linked list. It always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then create a separate _init function and call it
 * appropriately from within the program.
 */
संरचना curl_slist *curl_slist_append(संरचना curl_slist *list, ध्रुव वर्ण *दत्त)
{
  वर्ण *dupdata = curlx_strdup(दत्त);

  यदि(!dupdata)
    प्रत्यागम रिक्त;

  list = Curl_slist_append_nodup(list, dupdata);
  यदि(!list)
    curlx_free(dupdata);

  प्रत्यागम list;
}

/*
 * Curl_slist_duplicate() duplicates a linked list. It always returns the
 * address of the first record of the cloned list or NULL in case of an
 * error (or if the input list was NULL).
 */
संरचना curl_slist *Curl_slist_duplicate(संरचना curl_slist *inlist)
{
  संरचना curl_slist *outlist = रिक्त;
  संरचना curl_slist **पुच्छ = &outlist;

  यावत्(inlist) {
    संरचना curl_slist *tmp = curl_slist_append(रिक्त, inlist->दत्त);

    यदि(!tmp) {
      curl_slist_free_all(outlist);
      प्रत्यागम रिक्त;
    }

    *पुच्छ = tmp;
    पुच्छ = &tmp->अग्र;
    inlist = inlist->अग्र;
  }
  प्रत्यागम outlist;
}

/* be nice and clean up resources */
शून्य curl_slist_free_all(संरचना curl_slist *list)
{
  संरचना curl_slist *अग्र;
  संरचना curl_slist *item;

  यदि(!list)
    प्रत्यागम;

  item = list;
  कुरु {
    अग्र = item->अग्र;
    curlx_safefree(item->दत्त);
    curlx_free(item);
    item = अग्र;
  } यावत्(अग्र);
}
