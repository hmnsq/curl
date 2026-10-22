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
ಸ್ಥಿರ ರಚನೆ curl_slist *slist_get_ಕೊನೆಯ(ರಚನೆ curl_slist *list)
{
  ರಚನೆ curl_slist *ಅಂಶ;

  /* if caller passed us a NULL, return now */
  ಆದರೆ(!list)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  /* loop through to find the last item */
  ಅಂಶ = list;
  ಆಗ(ಅಂಶ->ಮುಂದಿನ) {
    ಅಂಶ = ಅಂಶ->ಮುಂದಿನ;
  }
  ಹಿಂತಿರುಗಿ ಅಂಶ;
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
ರಚನೆ curl_slist *Curl_slist_append_nodup(ರಚನೆ curl_slist *list,
                                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ)
{
  ರಚನೆ curl_slist *ಕೊನೆಯ;
  ರಚನೆ curl_slist *new_ಅಂಶ;

  DEBUGASSERT(ದತ್ತಾಂಶ);

  new_ಅಂಶ = curlx_malloc(ಗಾತ್ರ(ರಚನೆ curl_slist));
  ಆದರೆ(!new_ಅಂಶ)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  new_ಅಂಶ->ಮುಂದಿನ = ಖಾಲಿ;
  new_ಅಂಶ->ದತ್ತಾಂಶ = CURL_UNCONST(ದತ್ತಾಂಶ);

  /* if this is the first item, then new_item *is* the list */
  ಆದರೆ(!list)
    ಹಿಂತಿರುಗಿ new_ಅಂಶ;

  ಕೊನೆಯ = slist_get_ಕೊನೆಯ(list);
  ಕೊನೆಯ->ಮುಂದಿನ = new_ಅಂಶ;
  ಹಿಂತಿರುಗಿ list;
}

/*
 * curl_slist_append() appends a string to the linked list. It always returns
 * the address of the first record, so that you can use this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then create a separate _init function and call it
 * appropriately from within the program.
 */
ರಚನೆ curl_slist *curl_slist_append(ರಚನೆ curl_slist *list, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ)
{
  ಅಕ್ಷರ *dupdata = curlx_strdup(ದತ್ತಾಂಶ);

  ಆದರೆ(!dupdata)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  list = Curl_slist_append_nodup(list, dupdata);
  ಆದರೆ(!list)
    curlx_free(dupdata);

  ಹಿಂತಿರುಗಿ list;
}

/*
 * Curl_slist_duplicate() duplicates a linked list. It always returns the
 * address of the first record of the cloned list or NULL in case of an
 * error (or if the input list was NULL).
 */
ರಚನೆ curl_slist *Curl_slist_duplicate(ರಚನೆ curl_slist *inlist)
{
  ರಚನೆ curl_slist *outlist = ಖಾಲಿ;
  ರಚನೆ curl_slist **ಬಾಲ = &outlist;

  ಆಗ(inlist) {
    ರಚನೆ curl_slist *tmp = curl_slist_append(ಖಾಲಿ, inlist->ದತ್ತಾಂಶ);

    ಆದರೆ(!tmp) {
      curl_slist_free_all(outlist);
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    *ಬಾಲ = tmp;
    ಬಾಲ = &tmp->ಮುಂದಿನ;
    inlist = inlist->ಮುಂದಿನ;
  }
  ಹಿಂತಿರುಗಿ outlist;
}

/* be nice and clean up resources */
ಶೂನ್ಯ curl_slist_free_all(ರಚನೆ curl_slist *list)
{
  ರಚನೆ curl_slist *ಮುಂದಿನ;
  ರಚನೆ curl_slist *ಅಂಶ;

  ಆದರೆ(!list)
    ಹಿಂತಿರುಗಿ;

  ಅಂಶ = list;
  ಮಾಡು {
    ಮುಂದಿನ = ಅಂಶ->ಮುಂದಿನ;
    curlx_safefree(ಅಂಶ->ದತ್ತಾಂಶ);
    curlx_free(ಅಂಶ);
    ಅಂಶ = ಮುಂದಿನ;
  } ಆಗ(ಮುಂದಿನ);
}
