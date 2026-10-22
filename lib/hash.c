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

#ಅನ್ತರ್ಭಾವ <stddef.h> /* for offsetof() */

#ಅನ್ತರ್ಭಾವ "hash.h"
#ಅನ್ತರ್ಭಾವ "rand.h"

/* random patterns for API verification */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
#ವ್ಯಾಖ್ಯಾನಿಸು HASHINIT 0x7017e781
#ವ್ಯಾಖ್ಯಾನಿಸು ITERINIT 0x5FEDCBA9
#ಆದರೆ_ಅಂತ್ಯ


ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ size_t (*hash_function)(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ,
                                size_t ಕೀಲಿ_ಉದ್ದಳತೆ,
                                size_t slots_ಅಂಕೆ);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ size_t (*comp_function)(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *key1,
                                size_t key1_len,
                                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *key2,
                                size_t key2_len);

/* Curl_hash stores its type in a uint8_t. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಅಕ್ಷರ hash_types_fit_uint8[
  ((CURL_HASH_TYPE_ಕೊನೆಯ - 1) <= UINT8_MAX) ? 1 : -1];

ರಚನೆ hash_functions {
  hash_function hash;
  comp_function compare;
};

/* set at global init, read-only afterwards */
ಸ್ಥಿರ size_t hash_ಬೀಜ;

CURLcode Curl_hash_global_init(ಶೂನ್ಯ)
{
  ಸ್ಥಿರ bool seeded;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ buf[ಗಾತ್ರ(size_t)];
  size_t ಬೀಜ = 0;
  size_t i;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(seeded)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಫಲಿತಾಂಶ = Curl_rand_ಬೈಟುಗಳು(ಖಾಲಿ,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
                           FALSE,
#ಆದರೆ_ಅಂತ್ಯ
                           buf, ಗಾತ್ರ(buf));
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಗಾಗಿ(i = 0; i < ಗಾತ್ರ(buf); i++)
    ಬೀಜ = (ಬೀಜ << 8) | buf[i];

  hash_ಬೀಜ = ಬೀಜ ? ಬೀಜ : 1;
  seeded = TRUE;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST size_t hash_str_seeded(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_ಉದ್ದಳತೆ,
                                size_t slots_ಅಂಕೆ, size_t ಬೀಜ);
UNITTEST size_t hash_str_seeded(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_ಉದ್ದಳತೆ,
                                size_t slots_ಅಂಕೆ, size_t ಬೀಜ)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ_str = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಕೀಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ = ಕೀಲಿ_str + ಕೀಲಿ_ಉದ್ದಳತೆ;
  size_t h = 5381 ^ ಬೀಜ;

  ಆಗ(ಕೀಲಿ_str < ಕೊನೆ) {
    size_t j = (size_t)*ಕೀಲಿ_str++;
    h += h << 5;
    h ^= j;
  }

  ಹಿಂತಿರುಗಿ (h % slots_ಅಂಕೆ);
}

/* @unittest 1603
 */
UNITTEST size_t hash_str(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_ಉದ್ದಳತೆ, size_t slots_ಅಂಕೆ);
UNITTEST size_t hash_str(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_ಉದ್ದಳತೆ, size_t slots_ಅಂಕೆ)
{
  ಹಿಂತಿರುಗಿ hash_str_seeded(ಕೀಲಿ, ಕೀಲಿ_ಉದ್ದಳತೆ, slots_ಅಂಕೆ, hash_ಬೀಜ);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
ಸ್ಥಿರ ಅಕ್ಷರ *hash_elem_ಕೀಲಿ(ರಚನೆ Curl_hash_element *he)
{
  ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ *)he + offsetof(ರಚನೆ Curl_hash_element, ಕೀಲಿ);
}

ಸ್ಥಿರ size_t hash_socket(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_ಉದ್ದಳತೆ,
                          size_t slots_ಅಂಕೆ)
{
  curl_socket_t socket;

  DEBUGASSERT(ಕೀಲಿ_ಉದ್ದಳತೆ == ಗಾತ್ರ(socket));
  ಆದರೆ(ಕೀಲಿ_ಉದ್ದಳತೆ != ಗಾತ್ರ(socket))
    ಹಿಂತಿರುಗಿ 0;
  ಸ್ಮರಣೆ_ನಕಲು(&socket, ಕೀಲಿ, ಗಾತ್ರ(socket));
  ಹಿಂತಿರುಗಿ (size_t)(socket % (curl_socket_t)slots_ಅಂಕೆ);
}

ಸ್ಥಿರ size_t compare_socket(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *key1, size_t key1_len,
                             ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *key2, size_t key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  ಆದರೆ((key1_len != ಗಾತ್ರ(socket1)) || (key2_len != ಗಾತ್ರ(socket2)))
    ಹಿಂತಿರುಗಿ 0;
  ಸ್ಮರಣೆ_ನಕಲು(&socket1, key1, ಗಾತ್ರ(socket1));
  ಸ್ಮರಣೆ_ನಕಲು(&socket2, key2, ಗಾತ್ರ(socket2));
  ಹಿಂತಿರುಗಿ socket1 == socket2;
}

ಸ್ಥಿರ size_t compare_ಬೈಟುಗಳು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *key1, size_t key1_len,
                            ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *key2, size_t key2_len)
{
  ಹಿಂತಿರುಗಿ (key1_len == key2_len) && !memcmp(key1, key2, key1_len);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_functions hash_functions[] = {
  { hash_str, compare_ಬೈಟುಗಳು },
  { hash_socket, compare_socket }
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_ಕೊನೆಯ);
  ಆದರೆ((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)type >= CURL_HASH_TYPE_ಕೊನೆಯ)
    type = CURL_HASH_TYPE_ಬೈಟುಗಳು;
  ಹಿಂತಿರುಗಿ &hash_functions[type];
}

ಸ್ಥಿರ bool hash_ಕೀಲಿ_is_valid(uint8_t type, size_t ಕೀಲಿ_len)
{
  ಹಿಂತಿರುಗಿ (type == CURL_HASH_TYPE_ಬೈಟುಗಳು) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (ಕೀಲಿ_len == ಗಾತ್ರ(curl_socket_t)));
}

#ಆದರೆ 0 /* useful function for debugging hashes and their contents */
ಶೂನ್ಯ Curl_hash_print(ರಚನೆ Curl_hash *h, ಶೂನ್ಯ (*func)(ಶೂನ್ಯ *))
{
  ರಚನೆ Curl_hash_iterator iter;
  ರಚನೆ Curl_hash_element *he;
  size_t ಕೊನೆಯ_ಸೂಚ್ಯಂಕ = UINT_MAX;

  ಆದರೆ(!h)
    ಹಿಂತಿರುಗಿ;

  curl_mfprintf(stderr, "=Hash dump=\n");

  Curl_hash_ಆರಂಭ_iterate(h, &iter);

  he = Curl_hash_ಮುಂದಿನ_element(&iter);
  ಆಗ(he) {
    ಆದರೆ(iter.ಸ್ಥಾನ_ಸೂಚ್ಯಂಕ != ಕೊನೆಯ_ಸೂಚ್ಯಂಕ) {
      curl_mfprintf(stderr, "index %d:", (ಪೂರ್ಣಾಂಕ)iter.ಸ್ಥಾನ_ಸೂಚ್ಯಂಕ);
      ಆದರೆ(ಕೊನೆಯ_ಸೂಚ್ಯಂಕ != UINT_MAX) {
        curl_mfprintf(stderr, "\n");
      }
      ಕೊನೆಯ_ಸೂಚ್ಯಂಕ = iter.ಸ್ಥಾನ_ಸೂಚ್ಯಂಕ;
    }

    ಆದರೆ(func)
      func(he->ptr);
    ಇಲ್ಲದಿದ್ದರೆ
      curl_mfprintf(stderr, " [key=%.*s, he=%p, ptr=%p]",
                    (ಪೂರ್ಣಾಂಕ)he->ಕೀಲಿ_len, hash_elem_ಕೀಲಿ(he),
                    (ಶೂನ್ಯ *)he, (ಶೂನ್ಯ *)he->ptr);

    he = Curl_hash_ಮುಂದಿನ_element(&iter);
  }
  curl_mfprintf(stderr, "\n");
}
#ಆದರೆ_ಅಂತ್ಯ

/* Initializes a hash structure.
 *
 * @unittest: 1602
 * @unittest: 1603
 */
ಶೂನ್ಯ Curl_hash_init(ರಚನೆ Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor)
{
  DEBUGASSERT(h);
  DEBUGASSERT(slots);
  DEBUGASSERT((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)type < CURL_HASH_TYPE_ಕೊನೆಯ);
  DEBUGASSERT(dtor);

  h->ಕೋಷ್ಟಕ = ಖಾಲಿ;
  h->dtor = dtor;
  h->ಗಾತ್ರ_2 = 0;
  h->slots = slots;
  h->type = (uint8_t)type;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  h->init = HASHINIT;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ರಚನೆ Curl_hash_element *hash_elem_create(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ,
                                                  size_t ಕೀಲಿ_len,
                                                  ಶೂನ್ಯ *p,
                                                  Curl_hash_elem_dtor dtor)
{
  ರಚನೆ Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(ಗಾತ್ರ(ರಚನೆ Curl_hash_element) + ಕೀಲಿ_len);
  ಆದರೆ(he) {
    he->ಮುಂದಿನ = ಖಾಲಿ;
    /* copy the key */
    ಸ್ಮರಣೆ_ನಕಲು(hash_elem_ಕೀಲಿ(he), ಕೀಲಿ, ಕೀಲಿ_len);
    he->ಕೀಲಿ_len = ಕೀಲಿ_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  ಹಿಂತಿರುಗಿ he;
}

ಸ್ಥಿರ ಶೂನ್ಯ hash_elem_clear_ptr(ರಚನೆ Curl_hash *h,
                                ರಚನೆ Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  ಆದರೆ(he->ptr) {
    ಆದರೆ(he->dtor)
      he->dtor(hash_elem_ಕೀಲಿ(he), he->ಕೀಲಿ_len, he->ptr);
    ಇಲ್ಲದಿದ್ದರೆ
      h->dtor(he->ptr);
    he->ptr = ಖಾಲಿ;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ hash_elem_destroy(ರಚನೆ Curl_hash *h,
                              ರಚನೆ Curl_hash_element *he)
{
  hash_elem_clear_ptr(h, he);
  curlx_free(he);
}

ಸ್ಥಿರ ಶೂನ್ಯ hash_elem_unlink(ರಚನೆ Curl_hash *h,
                             ರಚನೆ Curl_hash_element **he_anchor,
                             ರಚನೆ Curl_hash_element *he)
{
  *he_anchor = he->ಮುಂದಿನ;
  --h->ಗಾತ್ರ_2;
}

ಸ್ಥಿರ ಶೂನ್ಯ hash_elem_link(ರಚನೆ Curl_hash *h,
                           ರಚನೆ Curl_hash_element **he_anchor,
                           ರಚನೆ Curl_hash_element *he)
{
  he->ಮುಂದಿನ = *he_anchor;
  *he_anchor = he;
  ++h->ಗಾತ್ರ_2;
}

ಶೂನ್ಯ *Curl_hash_add2(ರಚನೆ Curl_hash *h,
                     ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len, ಶೂನ್ಯ *p,
                     Curl_hash_elem_dtor dtor)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ hash_functions *functions;
  ರಚನೆ Curl_hash_element *he, **ಸ್ಥಾನ;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  ಆದರೆ(!hash_ಕೀಲಿ_is_valid(h->type, ಕೀಲಿ_len))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ಆದರೆ(!h->ಕೋಷ್ಟಕ) {
    h->ಕೋಷ್ಟಕ = curlx_calloc(h->slots, ಗಾತ್ರ(ರಚನೆ Curl_hash_element *));
    ಆದರೆ(!h->ಕೋಷ್ಟಕ)
      ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* OOM */
  }

  functions = hash_get_functions(h->type);
  ಸ್ಥಾನ = &h->ಕೋಷ್ಟಕ[functions->hash(ಕೀಲಿ, ಕೀಲಿ_len, h->slots)];
  ಗಾಗಿ(he = *ಸ್ಥಾನ; he; he = he->ಮುಂದಿನ) {
    ಆದರೆ(functions->compare(hash_elem_ಕೀಲಿ(he), he->ಕೀಲಿ_len, ಕೀಲಿ, ಕೀಲಿ_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      ಹಿಂತಿರುಗಿ p;
    }
  }

  he = hash_elem_create(ಕೀಲಿ, ಕೀಲಿ_len, p, dtor);
  ಆದರೆ(!he)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* OOM */

  hash_elem_link(h, ಸ್ಥಾನ, he);
  ಹಿಂತಿರುಗಿ p; /* return the new entry */
}

/* Insert the data in the hash. If there already was a match in the hash, that
 * data is replaced. This function also "lazily" allocates the table if
 * needed, as it is not done in the _init function (anymore).
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
ಶೂನ್ಯ *Curl_hash_ಕೂಡಿಸು(ರಚನೆ Curl_hash *h,
                    ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len, ಶೂನ್ಯ *p)
{
  ಹಿಂತಿರುಗಿ Curl_hash_add2(h, ಕೀಲಿ, ಕೀಲಿ_len, p, ಖಾಲಿ);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
ಪೂರ್ಣಾಂಕ Curl_hash_delete(ರಚನೆ Curl_hash *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  ಆದರೆ(!hash_ಕೀಲಿ_is_valid(h->type, ಕೀಲಿ_len))
    ಹಿಂತಿರುಗಿ 1;
  ಆದರೆ(h->ಕೋಷ್ಟಕ) {
    ರಚನೆ Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->ಕೋಷ್ಟಕ[functions->hash(ಕೀಲಿ, ಕೀಲಿ_len, h->slots)];
    ಆಗ(*he_anchor) {
      he = *he_anchor;
      ಆದರೆ(functions->compare(hash_elem_ಕೀಲಿ(he), he->ಕೀಲಿ_len, ಕೀಲಿ, ಕೀಲಿ_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        ಹಿಂತಿರುಗಿ 0;
      }
      he_anchor = &he->ಮುಂದಿನ;
    }
  }
  ಹಿಂತಿರುಗಿ 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
ಶೂನ್ಯ *Curl_hash_pick(ರಚನೆ Curl_hash *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  ಆದರೆ(!hash_ಕೀಲಿ_is_valid(h->type, ಕೀಲಿ_len))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ಆದರೆ(h->ಕೋಷ್ಟಕ) {
    ರಚನೆ Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->ಕೋಷ್ಟಕ[functions->hash(ಕೀಲಿ, ಕೀಲಿ_len, h->slots)];
    ಆಗ(he) {
      ಆದರೆ(functions->compare(hash_elem_ಕೀಲಿ(he), he->ಕೀಲಿ_len, ಕೀಲಿ, ಕೀಲಿ_len)) {
        ಹಿಂತಿರುಗಿ he->ptr;
      }
      he = he->ಮುಂದಿನ;
    }
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/* Destroys all the entries in the given hash and resets its attributes,
 * prepping the given hash for [static|dynamic] deallocation.
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
ಶೂನ್ಯ Curl_hash_destroy(ರಚನೆ Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  ಆದರೆ(h->ಕೋಷ್ಟಕ) {
    Curl_hash_clean(h);
    curlx_safefree(h->ಕೋಷ್ಟಕ);
  }
  DEBUGASSERT(h->ಗಾತ್ರ_2 == 0);
  h->slots = 0;
}

/* Removes all the entries in the given hash.
 *
 * @unittest: 1602
 */
ಶೂನ್ಯ Curl_hash_clean(ರಚನೆ Curl_hash *h)
{
  ಆದರೆ(h && h->ಕೋಷ್ಟಕ) {
    ರಚನೆ Curl_hash_element *he, **he_anchor;
    size_t i;
    DEBUGASSERT(h->init == HASHINIT);
    ಗಾಗಿ(i = 0; i < h->slots; ++i) {
      he_anchor = &h->ಕೋಷ್ಟಕ[i];
      ಆಗ(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

size_t Curl_hash_ಎಣಿಕೆ(ರಚನೆ Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  ಹಿಂತಿರುಗಿ h->ಗಾತ್ರ_2;
}

/* Cleans all entries that pass the comp function criteria. */
ಶೂನ್ಯ Curl_hash_clean_with_criterium(ರಚನೆ Curl_hash *h, ಶೂನ್ಯ *user,
                                    ಪೂರ್ಣಾಂಕ (*comp)(ಶೂನ್ಯ *, ಶೂನ್ಯ *))
{
  size_t i;

  ಆದರೆ(!h || !h->ಕೋಷ್ಟಕ)
    ಹಿಂತಿರುಗಿ;

  DEBUGASSERT(h->init == HASHINIT);
  ಗಾಗಿ(i = 0; i < h->slots; ++i) {
    ರಚನೆ Curl_hash_element *he, **he_anchor = &h->ಕೋಷ್ಟಕ[i];
    ಆಗ(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      ಆದರೆ(!comp || comp(user, (*he_anchor)->ptr)) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
      ಇಲ್ಲದಿದ್ದರೆ
        he_anchor = &(*he_anchor)->ಮುಂದಿನ;
    }
  }
}

ಶೂನ್ಯ Curl_hash_ಆರಂಭ_iterate(ರಚನೆ Curl_hash *hash,
                             ರಚನೆ Curl_hash_iterator *iter)
{
  DEBUGASSERT(hash->init == HASHINIT);
  iter->hash = hash;
  iter->ಸ್ಥಾನ_ಸೂಚ್ಯಂಕ = 0;
  iter->current = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  iter->init = ITERINIT;
#ಆದರೆ_ಅಂತ್ಯ
}

ರಚನೆ Curl_hash_element *Curl_hash_ಮುಂದಿನ_element(
  ರಚನೆ Curl_hash_iterator *iter)
{
  ರಚನೆ Curl_hash *h;
  DEBUGASSERT(iter->init == ITERINIT);
  h = iter->hash;
  ಆದರೆ(!h->ಕೋಷ್ಟಕ)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  ಆದರೆ(iter->current)
    iter->current = iter->current->ಮುಂದಿನ;

  /* If we have reached the end of the list, find the next one */
  ಆದರೆ(!iter->current) {
    size_t i;
    ಗಾಗಿ(i = iter->ಸ್ಥಾನ_ಸೂಚ್ಯಂಕ; i < h->slots; i++) {
      ಆದರೆ(h->ಕೋಷ್ಟಕ[i]) {
        iter->current = h->ಕೋಷ್ಟಕ[i];
        iter->ಸ್ಥಾನ_ಸೂಚ್ಯಂಕ = i + 1;
        ನಿಲ್ಲಿಸು;
      }
    }
  }

  ಹಿಂತಿರುಗಿ iter->current;
}
