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

#అన్తర్భావ <stddef.h> /* for offsetof() */

#అన్తర్భావ "hash.h"
#అన్తర్భావ "rand.h"

/* random patterns for API verification */
#నిర్వచితమైతే DEBUGBUILD
#నిర్వచించు HASHINIT 0x7017e781
#నిర్వచించు ITERINIT 0x5FEDCBA9
#అయితే_ముగింపు


రకనిర్వచనం size_t (*hash_function)(స్థిరరాశి శూన్య *కీలకం,
                                size_t కీలకం_నిడివి,
                                size_t slots_అంకె);

రకనిర్వచనం size_t (*comp_function)(స్థిరరాశి శూన్య *key1,
                                size_t key1_len,
                                స్థిరరాశి శూన్య *key2,
                                size_t key2_len);

/* Curl_hash stores its type in a uint8_t. */
రకనిర్వచనం అక్షరం hash_types_fit_uint8[
  ((CURL_HASH_TYPE_చివరి - 1) <= UINT8_MAX) ? 1 : -1];

నిర్మాణం hash_functions {
  hash_function hash;
  comp_function compare;
};

/* set at global init, read-only afterwards */
స్థిర size_t hash_విత్తనం;

CURLcode Curl_hash_global_init(శూన్య)
{
  స్థిర bool seeded;
  గుర్తులేని అక్షరం buf[పరిమాణం(size_t)];
  size_t విత్తనం = 0;
  size_t i;
  CURLcode ఫలితం;

  ఒకవేళ(seeded)
    తిరిగి CURLE_OK;

  ఫలితం = Curl_rand_బైట్లు(ఖాళీ,
#నిర్వచితమైతే DEBUGBUILD
                           FALSE,
#అయితే_ముగింపు
                           buf, పరిమాణం(buf));
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  కొరకు(i = 0; i < పరిమాణం(buf); i++)
    విత్తనం = (విత్తనం << 8) | buf[i];

  hash_విత్తనం = విత్తనం ? విత్తనం : 1;
  seeded = TRUE;
  తిరిగి CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST size_t hash_str_seeded(స్థిరరాశి శూన్య *కీలకం, size_t కీలకం_నిడివి,
                                size_t slots_అంకె, size_t విత్తనం);
UNITTEST size_t hash_str_seeded(స్థిరరాశి శూన్య *కీలకం, size_t కీలకం_నిడివి,
                                size_t slots_అంకె, size_t విత్తనం)
{
  స్థిరరాశి అక్షరం *కీలకం_str = (స్థిరరాశి అక్షరం *)కీలకం;
  స్థిరరాశి అక్షరం *చివర = కీలకం_str + కీలకం_నిడివి;
  size_t h = 5381 ^ విత్తనం;

  అయితే(కీలకం_str < చివర) {
    size_t j = (size_t)*కీలకం_str++;
    h += h << 5;
    h ^= j;
  }

  తిరిగి (h % slots_అంకె);
}

/* @unittest 1603
 */
UNITTEST size_t hash_str(స్థిరరాశి శూన్య *కీలకం, size_t కీలకం_నిడివి, size_t slots_అంకె);
UNITTEST size_t hash_str(స్థిరరాశి శూన్య *కీలకం, size_t కీలకం_నిడివి, size_t slots_అంకె)
{
  తిరిగి hash_str_seeded(కీలకం, కీలకం_నిడివి, slots_అంకె, hash_విత్తనం);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
స్థిర అక్షరం *hash_elem_కీలకం(నిర్మాణం Curl_hash_element *he)
{
  తిరిగి (అక్షరం *)he + offsetof(నిర్మాణం Curl_hash_element, కీలకం);
}

స్థిర size_t hash_socket(స్థిరరాశి శూన్య *కీలకం, size_t కీలకం_నిడివి,
                          size_t slots_అంకె)
{
  curl_socket_t socket;

  DEBUGASSERT(కీలకం_నిడివి == పరిమాణం(socket));
  ఒకవేళ(కీలకం_నిడివి != పరిమాణం(socket))
    తిరిగి 0;
  జ్ఞాపకం_నకలు(&socket, కీలకం, పరిమాణం(socket));
  తిరిగి (size_t)(socket % (curl_socket_t)slots_అంకె);
}

స్థిర size_t compare_socket(స్థిరరాశి శూన్య *key1, size_t key1_len,
                             స్థిరరాశి శూన్య *key2, size_t key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  ఒకవేళ((key1_len != పరిమాణం(socket1)) || (key2_len != పరిమాణం(socket2)))
    తిరిగి 0;
  జ్ఞాపకం_నకలు(&socket1, key1, పరిమాణం(socket1));
  జ్ఞాపకం_నకలు(&socket2, key2, పరిమాణం(socket2));
  తిరిగి socket1 == socket2;
}

స్థిర size_t compare_బైట్లు(స్థిరరాశి శూన్య *key1, size_t key1_len,
                            స్థిరరాశి శూన్య *key2, size_t key2_len)
{
  తిరిగి (key1_len == key2_len) && !memcmp(key1, key2, key1_len);
}

స్థిర స్థిరరాశి నిర్మాణం hash_functions hash_functions[] = {
  { hash_str, compare_బైట్లు },
  { hash_socket, compare_socket }
};

స్థిర స్థిరరాశి నిర్మాణం hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_చివరి);
  ఒకవేళ((గుర్తులేని పూర్ణాంకం)type >= CURL_HASH_TYPE_చివరి)
    type = CURL_HASH_TYPE_బైట్లు;
  తిరిగి &hash_functions[type];
}

స్థిర bool hash_కీలకం_is_valid(uint8_t type, size_t కీలకం_len)
{
  తిరిగి (type == CURL_HASH_TYPE_బైట్లు) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (కీలకం_len == పరిమాణం(curl_socket_t)));
}

#ఒకవేళ 0 /* useful function for debugging hashes and their contents */
శూన్య Curl_hash_print(నిర్మాణం Curl_hash *h, శూన్య (*func)(శూన్య *))
{
  నిర్మాణం Curl_hash_iterator iter;
  నిర్మాణం Curl_hash_element *he;
  size_t చివరి_సూచీ = UINT_MAX;

  ఒకవేళ(!h)
    తిరిగి;

  curl_mfprintf(stderr, "=Hash dump=\n");

  Curl_hash_ప్రారంభం_iterate(h, &iter);

  he = Curl_hash_తదుపరి_element(&iter);
  అయితే(he) {
    ఒకవేళ(iter.స్థానం_సూచీ != చివరి_సూచీ) {
      curl_mfprintf(stderr, "index %d:", (పూర్ణాంకం)iter.స్థానం_సూచీ);
      ఒకవేళ(చివరి_సూచీ != UINT_MAX) {
        curl_mfprintf(stderr, "\n");
      }
      చివరి_సూచీ = iter.స్థానం_సూచీ;
    }

    ఒకవేళ(func)
      func(he->ptr);
    లేకపోతే
      curl_mfprintf(stderr, " [key=%.*s, he=%p, ptr=%p]",
                    (పూర్ణాంకం)he->కీలకం_len, hash_elem_కీలకం(he),
                    (శూన్య *)he, (శూన్య *)he->ptr);

    he = Curl_hash_తదుపరి_element(&iter);
  }
  curl_mfprintf(stderr, "\n");
}
#అయితే_ముగింపు

/* Initializes a hash structure.
 *
 * @unittest: 1602
 * @unittest: 1603
 */
శూన్య Curl_hash_init(నిర్మాణం Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor)
{
  DEBUGASSERT(h);
  DEBUGASSERT(slots);
  DEBUGASSERT((గుర్తులేని పూర్ణాంకం)type < CURL_HASH_TYPE_చివరి);
  DEBUGASSERT(dtor);

  h->పట్టిక = ఖాళీ;
  h->dtor = dtor;
  h->కొలత = 0;
  h->slots = slots;
  h->type = (uint8_t)type;
#నిర్వచితమైతే DEBUGBUILD
  h->init = HASHINIT;
#అయితే_ముగింపు
}

స్థిర నిర్మాణం Curl_hash_element *hash_elem_create(స్థిరరాశి శూన్య *కీలకం,
                                                  size_t కీలకం_len,
                                                  శూన్య *p,
                                                  Curl_hash_elem_dtor dtor)
{
  నిర్మాణం Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(పరిమాణం(నిర్మాణం Curl_hash_element) + కీలకం_len);
  ఒకవేళ(he) {
    he->తదుపరి = ఖాళీ;
    /* copy the key */
    జ్ఞాపకం_నకలు(hash_elem_కీలకం(he), కీలకం, కీలకం_len);
    he->కీలకం_len = కీలకం_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  తిరిగి he;
}

స్థిర శూన్య hash_elem_clear_ptr(నిర్మాణం Curl_hash *h,
                                నిర్మాణం Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  ఒకవేళ(he->ptr) {
    ఒకవేళ(he->dtor)
      he->dtor(hash_elem_కీలకం(he), he->కీలకం_len, he->ptr);
    లేకపోతే
      h->dtor(he->ptr);
    he->ptr = ఖాళీ;
  }
}

స్థిర శూన్య hash_elem_destroy(నిర్మాణం Curl_hash *h,
                              నిర్మాణం Curl_hash_element *he)
{
  hash_elem_clear_ptr(h, he);
  curlx_free(he);
}

స్థిర శూన్య hash_elem_unlink(నిర్మాణం Curl_hash *h,
                             నిర్మాణం Curl_hash_element **he_anchor,
                             నిర్మాణం Curl_hash_element *he)
{
  *he_anchor = he->తదుపరి;
  --h->కొలత;
}

స్థిర శూన్య hash_elem_link(నిర్మాణం Curl_hash *h,
                           నిర్మాణం Curl_hash_element **he_anchor,
                           నిర్మాణం Curl_hash_element *he)
{
  he->తదుపరి = *he_anchor;
  *he_anchor = he;
  ++h->కొలత;
}

శూన్య *Curl_hash_add2(నిర్మాణం Curl_hash *h,
                     స్థిరరాశి శూన్య *కీలకం, size_t కీలకం_len, శూన్య *p,
                     Curl_hash_elem_dtor dtor)
{
  స్థిరరాశి నిర్మాణం hash_functions *functions;
  నిర్మాణం Curl_hash_element *he, **స్థానం;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  ఒకవేళ(!hash_కీలకం_is_valid(h->type, కీలకం_len))
    తిరిగి ఖాళీ;
  ఒకవేళ(!h->పట్టిక) {
    h->పట్టిక = curlx_calloc(h->slots, పరిమాణం(నిర్మాణం Curl_hash_element *));
    ఒకవేళ(!h->పట్టిక)
      తిరిగి ఖాళీ; /* OOM */
  }

  functions = hash_get_functions(h->type);
  స్థానం = &h->పట్టిక[functions->hash(కీలకం, కీలకం_len, h->slots)];
  కొరకు(he = *స్థానం; he; he = he->తదుపరి) {
    ఒకవేళ(functions->compare(hash_elem_కీలకం(he), he->కీలకం_len, కీలకం, కీలకం_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      తిరిగి p;
    }
  }

  he = hash_elem_create(కీలకం, కీలకం_len, p, dtor);
  ఒకవేళ(!he)
    తిరిగి ఖాళీ; /* OOM */

  hash_elem_link(h, స్థానం, he);
  తిరిగి p; /* return the new entry */
}

/* Insert the data in the hash. If there already was a match in the hash, that
 * data is replaced. This function also "lazily" allocates the table if
 * needed, as it is not done in the _init function (anymore).
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
శూన్య *Curl_hash_కూడు(నిర్మాణం Curl_hash *h,
                    స్థిరరాశి శూన్య *కీలకం, size_t కీలకం_len, శూన్య *p)
{
  తిరిగి Curl_hash_add2(h, కీలకం, కీలకం_len, p, ఖాళీ);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
పూర్ణాంకం Curl_hash_delete(నిర్మాణం Curl_hash *h, స్థిరరాశి శూన్య *కీలకం, size_t కీలకం_len)
{
  స్థిరరాశి నిర్మాణం hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  ఒకవేళ(!hash_కీలకం_is_valid(h->type, కీలకం_len))
    తిరిగి 1;
  ఒకవేళ(h->పట్టిక) {
    నిర్మాణం Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->పట్టిక[functions->hash(కీలకం, కీలకం_len, h->slots)];
    అయితే(*he_anchor) {
      he = *he_anchor;
      ఒకవేళ(functions->compare(hash_elem_కీలకం(he), he->కీలకం_len, కీలకం, కీలకం_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        తిరిగి 0;
      }
      he_anchor = &he->తదుపరి;
    }
  }
  తిరిగి 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
శూన్య *Curl_hash_pick(నిర్మాణం Curl_hash *h, స్థిరరాశి శూన్య *కీలకం, size_t కీలకం_len)
{
  స్థిరరాశి నిర్మాణం hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  ఒకవేళ(!hash_కీలకం_is_valid(h->type, కీలకం_len))
    తిరిగి ఖాళీ;
  ఒకవేళ(h->పట్టిక) {
    నిర్మాణం Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->పట్టిక[functions->hash(కీలకం, కీలకం_len, h->slots)];
    అయితే(he) {
      ఒకవేళ(functions->compare(hash_elem_కీలకం(he), he->కీలకం_len, కీలకం, కీలకం_len)) {
        తిరిగి he->ptr;
      }
      he = he->తదుపరి;
    }
  }
  తిరిగి ఖాళీ;
}

/* Destroys all the entries in the given hash and resets its attributes,
 * prepping the given hash for [static|dynamic] deallocation.
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
శూన్య Curl_hash_destroy(నిర్మాణం Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  ఒకవేళ(h->పట్టిక) {
    Curl_hash_clean(h);
    curlx_safefree(h->పట్టిక);
  }
  DEBUGASSERT(h->కొలత == 0);
  h->slots = 0;
}

/* Removes all the entries in the given hash.
 *
 * @unittest: 1602
 */
శూన్య Curl_hash_clean(నిర్మాణం Curl_hash *h)
{
  ఒకవేళ(h && h->పట్టిక) {
    నిర్మాణం Curl_hash_element *he, **he_anchor;
    size_t i;
    DEBUGASSERT(h->init == HASHINIT);
    కొరకు(i = 0; i < h->slots; ++i) {
      he_anchor = &h->పట్టిక[i];
      అయితే(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

size_t Curl_hash_లెక్క(నిర్మాణం Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  తిరిగి h->కొలత;
}

/* Cleans all entries that pass the comp function criteria. */
శూన్య Curl_hash_clean_with_criterium(నిర్మాణం Curl_hash *h, శూన్య *user,
                                    పూర్ణాంకం (*comp)(శూన్య *, శూన్య *))
{
  size_t i;

  ఒకవేళ(!h || !h->పట్టిక)
    తిరిగి;

  DEBUGASSERT(h->init == HASHINIT);
  కొరకు(i = 0; i < h->slots; ++i) {
    నిర్మాణం Curl_hash_element *he, **he_anchor = &h->పట్టిక[i];
    అయితే(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      ఒకవేళ(!comp || comp(user, (*he_anchor)->ptr)) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
      లేకపోతే
        he_anchor = &(*he_anchor)->తదుపరి;
    }
  }
}

శూన్య Curl_hash_ప్రారంభం_iterate(నిర్మాణం Curl_hash *hash,
                             నిర్మాణం Curl_hash_iterator *iter)
{
  DEBUGASSERT(hash->init == HASHINIT);
  iter->hash = hash;
  iter->స్థానం_సూచీ = 0;
  iter->current = ఖాళీ;
#నిర్వచితమైతే DEBUGBUILD
  iter->init = ITERINIT;
#అయితే_ముగింపు
}

నిర్మాణం Curl_hash_element *Curl_hash_తదుపరి_element(
  నిర్మాణం Curl_hash_iterator *iter)
{
  నిర్మాణం Curl_hash *h;
  DEBUGASSERT(iter->init == ITERINIT);
  h = iter->hash;
  ఒకవేళ(!h->పట్టిక)
    తిరిగి ఖాళీ; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  ఒకవేళ(iter->current)
    iter->current = iter->current->తదుపరి;

  /* If we have reached the end of the list, find the next one */
  ఒకవేళ(!iter->current) {
    size_t i;
    కొరకు(i = iter->స్థానం_సూచీ; i < h->slots; i++) {
      ఒకవేళ(h->పట్టిక[i]) {
        iter->current = h->పట్టిక[i];
        iter->స్థానం_సూచీ = i + 1;
        ఆపు;
      }
    }
  }

  తిరిగి iter->current;
}
