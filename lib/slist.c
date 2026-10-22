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
#समावेश "curl_setup.h"

#समावेश "slist.h"

/* returns last node in linked list */
अटल काष्ठा curl_slist *slist_get_last(काष्ठा curl_slist *list)
{
  काष्ठा curl_slist *item;

  /* if caller passed us a NULL, return now */
  अगर(!list)
    वापस शून्य;

  /* loop through to find the last item */
  item = list;
  जबतक(item->अग्र) {
    item = item->अग्र;
  }
  वापस item;
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
काष्ठा curl_slist *Curl_slist_append_nodup(काष्ठा curl_slist *list,
                                           स्थिर अक्षर *तथ्य)
{
  काष्ठा curl_slist *last;
  काष्ठा curl_slist *new_item;

  DEBUGASSERT(तथ्य);

  new_item = curlx_malloc(माप(काष्ठा curl_slist));
  अगर(!new_item)
    वापस शून्य;

  new_item->अग्र = शून्य;
  new_item->तथ्य = CURL_UNCONST(तथ्य);

  /* if this is the first item, then new_item *is* the list */
  अगर(!list)
    वापस new_item;

  last = slist_get_last(list);
  last->अग्र = new_item;
  वापस list;
}

/*
 * curl_slist_append() appends a string to the linked list. It always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then create a separate _init function and call it
 * appropriately from within the program.
 */
काष्ठा curl_slist *curl_slist_append(काष्ठा curl_slist *list, स्थिर अक्षर *तथ्य)
{
  अक्षर *dupdata = curlx_strdup(तथ्य);

  अगर(!dupdata)
    वापस शून्य;

  list = Curl_slist_append_nodup(list, dupdata);
  अगर(!list)
    curlx_free(dupdata);

  वापस list;
}

/*
 * Curl_slist_duplicate() duplicates a linked list. It always returns the
 * address of the first record of the cloned list or NULL in case of an
 * error (or if the input list was NULL).
 */
काष्ठा curl_slist *Curl_slist_duplicate(काष्ठा curl_slist *inlist)
{
  काष्ठा curl_slist *outlist = शून्य;
  काष्ठा curl_slist **पुच्छ = &outlist;

  जबतक(inlist) {
    काष्ठा curl_slist *tmp = curl_slist_append(शून्य, inlist->तथ्य);

    अगर(!tmp) {
      curl_slist_free_all(outlist);
      वापस शून्य;
    }

    *पुच्छ = tmp;
    पुच्छ = &tmp->अग्र;
    inlist = inlist->अग्र;
  }
  वापस outlist;
}

/* be nice and clean up resources */
व्योम curl_slist_free_all(काष्ठा curl_slist *list)
{
  काष्ठा curl_slist *अग्र;
  काष्ठा curl_slist *item;

  अगर(!list)
    वापस;

  item = list;
  करो {
    अग्र = item->अग्र;
    curlx_safefree(item->तथ्य);
    curlx_free(item);
    item = अग्र;
  } जबतक(अग्र);
}
