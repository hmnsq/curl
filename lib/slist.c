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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "slist.h"

/* returns last node in linked list */
స్థిర నిర్మాణం curl_slist *slist_get_చివరి(నిర్మాణం curl_slist *list)
{
  నిర్మాణం curl_slist *అంశం;

  /* if caller passed us a NULL, return now */
  ఒకవేళ(!list)
    తిరిగి ఖాళీ;

  /* loop through to find the last item */
  అంశం = list;
  అయితే(అంశం->తదుపరి) {
    అంశం = అంశం->తదుపరి;
  }
  తిరిగి అంశం;
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
నిర్మాణం curl_slist *Curl_slist_append_nodup(నిర్మాణం curl_slist *list,
                                           స్థిరరాశి అక్షరం *దత్తాంశం)
{
  నిర్మాణం curl_slist *చివరి;
  నిర్మాణం curl_slist *new_అంశం;

  DEBUGASSERT(దత్తాంశం);

  new_అంశం = curlx_malloc(పరిమాణం(నిర్మాణం curl_slist));
  ఒకవేళ(!new_అంశం)
    తిరిగి ఖాళీ;

  new_అంశం->తదుపరి = ఖాళీ;
  new_అంశం->దత్తాంశం = CURL_UNCONST(దత్తాంశం);

  /* if this is the first item, then new_item *is* the list */
  ఒకవేళ(!list)
    తిరిగి new_అంశం;

  చివరి = slist_get_చివరి(list);
  చివరి->తదుపరి = new_అంశం;
  తిరిగి list;
}

/*
 * curl_slist_append() appends a string to the linked list. It always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then create a separate _init function and call it
 * appropriately from within the program.
 */
నిర్మాణం curl_slist *curl_slist_append(నిర్మాణం curl_slist *list, స్థిరరాశి అక్షరం *దత్తాంశం)
{
  అక్షరం *dupdata = curlx_strdup(దత్తాంశం);

  ఒకవేళ(!dupdata)
    తిరిగి ఖాళీ;

  list = Curl_slist_append_nodup(list, dupdata);
  ఒకవేళ(!list)
    curlx_free(dupdata);

  తిరిగి list;
}

/*
 * Curl_slist_duplicate() duplicates a linked list. It always returns the
 * address of the first record of the cloned list or NULL in case of an
 * error (or if the input list was NULL).
 */
నిర్మాణం curl_slist *Curl_slist_duplicate(నిర్మాణం curl_slist *inlist)
{
  నిర్మాణం curl_slist *outlist = ఖాళీ;
  నిర్మాణం curl_slist **తోక = &outlist;

  అయితే(inlist) {
    నిర్మాణం curl_slist *tmp = curl_slist_append(ఖాళీ, inlist->దత్తాంశం);

    ఒకవేళ(!tmp) {
      curl_slist_free_all(outlist);
      తిరిగి ఖాళీ;
    }

    *తోక = tmp;
    తోక = &tmp->తదుపరి;
    inlist = inlist->తదుపరి;
  }
  తిరిగి outlist;
}

/* be nice and clean up resources */
శూన్య curl_slist_free_all(నిర్మాణం curl_slist *list)
{
  నిర్మాణం curl_slist *తదుపరి;
  నిర్మాణం curl_slist *అంశం;

  ఒకవేళ(!list)
    తిరిగి;

  అంశం = list;
  చేయి {
    తదుపరి = అంశం->తదుపరి;
    curlx_safefree(అంశం->దత్తాంశం);
    curlx_free(అంశం);
    అంశం = తదుపరి;
  } అయితే(తదుపరి);
}
