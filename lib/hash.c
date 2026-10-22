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

#ਅਨ੍ਤਰ੍ਭਾਵ <stddef.h> /* for offsetof() */

#ਅਨ੍ਤਰ੍ਭਾਵ "hash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rand.h"

/* random patterns for API verification */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASHINIT 0x7017e781
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ITERINIT 0x5FEDCBA9
#ਜੇ_ਅੰਤ


ਕਿਸਮਪਰਿਭਾਸ਼ਾ size_t (*hash_function)(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ,
                                size_t ਕੁੰਜੀ_ਲੰਮਾਈ,
                                size_t slots_ਅੰਕ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ size_t (*comp_function)(ਅਚਲ ਖਾਲੀ *key1,
                                size_t key1_len,
                                ਅਚਲ ਖਾਲੀ *key2,
                                size_t key2_len);

/* Curl_hash stores its type in a uint8_t. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਅੱਖਰ hash_types_fit_uint8[
  ((CURL_HASH_TYPE_ਆਖਰੀ - 1) <= UINT8_MAX) ? 1 : -1];

ਬਣਤਰ hash_functions {
  hash_function hash;
  comp_function compare;
};

/* set at global init, read-only afterwards */
ਸਥਿਰ size_t hash_ਬੀਜ;

CURLcode Curl_hash_global_init(ਖਾਲੀ)
{
  ਸਥਿਰ bool seeded;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ buf[ਆਕਾਰ(size_t)];
  size_t ਬੀਜ = 0;
  size_t i;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(seeded)
    ਵਾਪਸ CURLE_OK;

  ਨਤੀਜਾ = Curl_rand_ਬਾਈਟਾਂ(ਨਿਰਾ,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
                           FALSE,
#ਜੇ_ਅੰਤ
                           buf, ਆਕਾਰ(buf));
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਲਈ(i = 0; i < ਆਕਾਰ(buf); i++)
    ਬੀਜ = (ਬੀਜ << 8) | buf[i];

  hash_ਬੀਜ = ਬੀਜ ? ਬੀਜ : 1;
  seeded = TRUE;
  ਵਾਪਸ CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST size_t hash_str_seeded(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t ਕੁੰਜੀ_ਲੰਮਾਈ,
                                size_t slots_ਅੰਕ, size_t ਬੀਜ);
UNITTEST size_t hash_str_seeded(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t ਕੁੰਜੀ_ਲੰਮਾਈ,
                                size_t slots_ਅੰਕ, size_t ਬੀਜ)
{
  ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ_str = (ਅਚਲ ਅੱਖਰ *)ਕੁੰਜੀ;
  ਅਚਲ ਅੱਖਰ *ਸਿਰਾ = ਕੁੰਜੀ_str + ਕੁੰਜੀ_ਲੰਮਾਈ;
  size_t h = 5381 ^ ਬੀਜ;

  ਜਦੋਂ(ਕੁੰਜੀ_str < ਸਿਰਾ) {
    size_t j = (size_t)*ਕੁੰਜੀ_str++;
    h += h << 5;
    h ^= j;
  }

  ਵਾਪਸ (h % slots_ਅੰਕ);
}

/* @unittest 1603
 */
UNITTEST size_t hash_str(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t ਕੁੰਜੀ_ਲੰਮਾਈ, size_t slots_ਅੰਕ);
UNITTEST size_t hash_str(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t ਕੁੰਜੀ_ਲੰਮਾਈ, size_t slots_ਅੰਕ)
{
  ਵਾਪਸ hash_str_seeded(ਕੁੰਜੀ, ਕੁੰਜੀ_ਲੰਮਾਈ, slots_ਅੰਕ, hash_ਬੀਜ);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
ਸਥਿਰ ਅੱਖਰ *hash_elem_ਕੁੰਜੀ(ਬਣਤਰ Curl_hash_element *he)
{
  ਵਾਪਸ (ਅੱਖਰ *)he + offsetof(ਬਣਤਰ Curl_hash_element, ਕੁੰਜੀ);
}

ਸਥਿਰ size_t hash_socket(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t ਕੁੰਜੀ_ਲੰਮਾਈ,
                          size_t slots_ਅੰਕ)
{
  curl_socket_t socket;

  DEBUGASSERT(ਕੁੰਜੀ_ਲੰਮਾਈ == ਆਕਾਰ(socket));
  ਜੇ(ਕੁੰਜੀ_ਲੰਮਾਈ != ਆਕਾਰ(socket))
    ਵਾਪਸ 0;
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&socket, ਕੁੰਜੀ, ਆਕਾਰ(socket));
  ਵਾਪਸ (size_t)(socket % (curl_socket_t)slots_ਅੰਕ);
}

ਸਥਿਰ size_t compare_socket(ਅਚਲ ਖਾਲੀ *key1, size_t key1_len,
                             ਅਚਲ ਖਾਲੀ *key2, size_t key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  ਜੇ((key1_len != ਆਕਾਰ(socket1)) || (key2_len != ਆਕਾਰ(socket2)))
    ਵਾਪਸ 0;
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&socket1, key1, ਆਕਾਰ(socket1));
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&socket2, key2, ਆਕਾਰ(socket2));
  ਵਾਪਸ socket1 == socket2;
}

ਸਥਿਰ size_t compare_ਬਾਈਟਾਂ(ਅਚਲ ਖਾਲੀ *key1, size_t key1_len,
                            ਅਚਲ ਖਾਲੀ *key2, size_t key2_len)
{
  ਵਾਪਸ (key1_len == key2_len) && !memcmp(key1, key2, key1_len);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ hash_functions hash_functions[] = {
  { hash_str, compare_ਬਾਈਟਾਂ },
  { hash_socket, compare_socket }
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_ਆਖਰੀ);
  ਜੇ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)type >= CURL_HASH_TYPE_ਆਖਰੀ)
    type = CURL_HASH_TYPE_ਬਾਈਟਾਂ;
  ਵਾਪਸ &hash_functions[type];
}

ਸਥਿਰ bool hash_ਕੁੰਜੀ_is_valid(uint8_t type, size_t ਕੁੰਜੀ_len)
{
  ਵਾਪਸ (type == CURL_HASH_TYPE_ਬਾਈਟਾਂ) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (ਕੁੰਜੀ_len == ਆਕਾਰ(curl_socket_t)));
}

#ਜੇ 0 /* useful function for debugging hashes and their contents */
ਖਾਲੀ Curl_hash_print(ਬਣਤਰ Curl_hash *h, ਖਾਲੀ (*func)(ਖਾਲੀ *))
{
  ਬਣਤਰ Curl_hash_iterator iter;
  ਬਣਤਰ Curl_hash_element *he;
  size_t ਆਖਰੀ_ਸੂਚਕਾਂਕ = UINT_MAX;

  ਜੇ(!h)
    ਵਾਪਸ;

  curl_mfprintf(stderr, "=Hash dump=\n");

  Curl_hash_ਸ਼ੁਰੂ_iterate(h, &iter);

  he = Curl_hash_ਅਗਲਾ_element(&iter);
  ਜਦੋਂ(he) {
    ਜੇ(iter.ਥਾਂ_ਸੂਚਕਾਂਕ != ਆਖਰੀ_ਸੂਚਕਾਂਕ) {
      curl_mfprintf(stderr, "index %d:", (ਪੂਰਨ)iter.ਥਾਂ_ਸੂਚਕਾਂਕ);
      ਜੇ(ਆਖਰੀ_ਸੂਚਕਾਂਕ != UINT_MAX) {
        curl_mfprintf(stderr, "\n");
      }
      ਆਖਰੀ_ਸੂਚਕਾਂਕ = iter.ਥਾਂ_ਸੂਚਕਾਂਕ;
    }

    ਜੇ(func)
      func(he->ptr);
    ਨਹੀਂਤਾਂ
      curl_mfprintf(stderr, " [key=%.*s, he=%p, ptr=%p]",
                    (ਪੂਰਨ)he->ਕੁੰਜੀ_len, hash_elem_ਕੁੰਜੀ(he),
                    (ਖਾਲੀ *)he, (ਖਾਲੀ *)he->ptr);

    he = Curl_hash_ਅਗਲਾ_element(&iter);
  }
  curl_mfprintf(stderr, "\n");
}
#ਜੇ_ਅੰਤ

/* Initializes a hash structure.
 *
 * @unittest: 1602
 * @unittest: 1603
 */
ਖਾਲੀ Curl_hash_init(ਬਣਤਰ Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor)
{
  DEBUGASSERT(h);
  DEBUGASSERT(slots);
  DEBUGASSERT((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)type < CURL_HASH_TYPE_ਆਖਰੀ);
  DEBUGASSERT(dtor);

  h->ਸਾਰਣੀ = ਨਿਰਾ;
  h->dtor = dtor;
  h->ਮਾਪ = 0;
  h->slots = slots;
  h->type = (uint8_t)type;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  h->init = HASHINIT;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਬਣਤਰ Curl_hash_element *hash_elem_create(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ,
                                                  size_t ਕੁੰਜੀ_len,
                                                  ਖਾਲੀ *p,
                                                  Curl_hash_elem_dtor dtor)
{
  ਬਣਤਰ Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(ਆਕਾਰ(ਬਣਤਰ Curl_hash_element) + ਕੁੰਜੀ_len);
  ਜੇ(he) {
    he->ਅਗਲਾ = ਨਿਰਾ;
    /* copy the key */
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(hash_elem_ਕੁੰਜੀ(he), ਕੁੰਜੀ, ਕੁੰਜੀ_len);
    he->ਕੁੰਜੀ_len = ਕੁੰਜੀ_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  ਵਾਪਸ he;
}

ਸਥਿਰ ਖਾਲੀ hash_elem_clear_ptr(ਬਣਤਰ Curl_hash *h,
                                ਬਣਤਰ Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  ਜੇ(he->ptr) {
    ਜੇ(he->dtor)
      he->dtor(hash_elem_ਕੁੰਜੀ(he), he->ਕੁੰਜੀ_len, he->ptr);
    ਨਹੀਂਤਾਂ
      h->dtor(he->ptr);
    he->ptr = ਨਿਰਾ;
  }
}

ਸਥਿਰ ਖਾਲੀ hash_elem_destroy(ਬਣਤਰ Curl_hash *h,
                              ਬਣਤਰ Curl_hash_element *he)
{
  hash_elem_clear_ptr(h, he);
  curlx_free(he);
}

ਸਥਿਰ ਖਾਲੀ hash_elem_unlink(ਬਣਤਰ Curl_hash *h,
                             ਬਣਤਰ Curl_hash_element **he_anchor,
                             ਬਣਤਰ Curl_hash_element *he)
{
  *he_anchor = he->ਅਗਲਾ;
  --h->ਮਾਪ;
}

ਸਥਿਰ ਖਾਲੀ hash_elem_link(ਬਣਤਰ Curl_hash *h,
                           ਬਣਤਰ Curl_hash_element **he_anchor,
                           ਬਣਤਰ Curl_hash_element *he)
{
  he->ਅਗਲਾ = *he_anchor;
  *he_anchor = he;
  ++h->ਮਾਪ;
}

ਖਾਲੀ *Curl_hash_add2(ਬਣਤਰ Curl_hash *h,
                     ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t ਕੁੰਜੀ_len, ਖਾਲੀ *p,
                     Curl_hash_elem_dtor dtor)
{
  ਅਚਲ ਬਣਤਰ hash_functions *functions;
  ਬਣਤਰ Curl_hash_element *he, **ਥਾਂ;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  ਜੇ(!hash_ਕੁੰਜੀ_is_valid(h->type, ਕੁੰਜੀ_len))
    ਵਾਪਸ ਨਿਰਾ;
  ਜੇ(!h->ਸਾਰਣੀ) {
    h->ਸਾਰਣੀ = curlx_calloc(h->slots, ਆਕਾਰ(ਬਣਤਰ Curl_hash_element *));
    ਜੇ(!h->ਸਾਰਣੀ)
      ਵਾਪਸ ਨਿਰਾ; /* OOM */
  }

  functions = hash_get_functions(h->type);
  ਥਾਂ = &h->ਸਾਰਣੀ[functions->hash(ਕੁੰਜੀ, ਕੁੰਜੀ_len, h->slots)];
  ਲਈ(he = *ਥਾਂ; he; he = he->ਅਗਲਾ) {
    ਜੇ(functions->compare(hash_elem_ਕੁੰਜੀ(he), he->ਕੁੰਜੀ_len, ਕੁੰਜੀ, ਕੁੰਜੀ_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      ਵਾਪਸ p;
    }
  }

  he = hash_elem_create(ਕੁੰਜੀ, ਕੁੰਜੀ_len, p, dtor);
  ਜੇ(!he)
    ਵਾਪਸ ਨਿਰਾ; /* OOM */

  hash_elem_link(h, ਥਾਂ, he);
  ਵਾਪਸ p; /* return the new entry */
}

/* Insert the data in the hash. If there already was a match in the hash, that
 * data is replaced. This function also "lazily" allocates the table if
 * needed, as it is not done in the _init function (anymore).
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
ਖਾਲੀ *Curl_hash_ਜੋੜੋ(ਬਣਤਰ Curl_hash *h,
                    ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t ਕੁੰਜੀ_len, ਖਾਲੀ *p)
{
  ਵਾਪਸ Curl_hash_add2(h, ਕੁੰਜੀ, ਕੁੰਜੀ_len, p, ਨਿਰਾ);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
ਪੂਰਨ Curl_hash_delete(ਬਣਤਰ Curl_hash *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t ਕੁੰਜੀ_len)
{
  ਅਚਲ ਬਣਤਰ hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  ਜੇ(!hash_ਕੁੰਜੀ_is_valid(h->type, ਕੁੰਜੀ_len))
    ਵਾਪਸ 1;
  ਜੇ(h->ਸਾਰਣੀ) {
    ਬਣਤਰ Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->ਸਾਰਣੀ[functions->hash(ਕੁੰਜੀ, ਕੁੰਜੀ_len, h->slots)];
    ਜਦੋਂ(*he_anchor) {
      he = *he_anchor;
      ਜੇ(functions->compare(hash_elem_ਕੁੰਜੀ(he), he->ਕੁੰਜੀ_len, ਕੁੰਜੀ, ਕੁੰਜੀ_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        ਵਾਪਸ 0;
      }
      he_anchor = &he->ਅਗਲਾ;
    }
  }
  ਵਾਪਸ 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
ਖਾਲੀ *Curl_hash_pick(ਬਣਤਰ Curl_hash *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t ਕੁੰਜੀ_len)
{
  ਅਚਲ ਬਣਤਰ hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  ਜੇ(!hash_ਕੁੰਜੀ_is_valid(h->type, ਕੁੰਜੀ_len))
    ਵਾਪਸ ਨਿਰਾ;
  ਜੇ(h->ਸਾਰਣੀ) {
    ਬਣਤਰ Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->ਸਾਰਣੀ[functions->hash(ਕੁੰਜੀ, ਕੁੰਜੀ_len, h->slots)];
    ਜਦੋਂ(he) {
      ਜੇ(functions->compare(hash_elem_ਕੁੰਜੀ(he), he->ਕੁੰਜੀ_len, ਕੁੰਜੀ, ਕੁੰਜੀ_len)) {
        ਵਾਪਸ he->ptr;
      }
      he = he->ਅਗਲਾ;
    }
  }
  ਵਾਪਸ ਨਿਰਾ;
}

/* Destroys all the entries in the given hash and resets its attributes,
 * prepping the given hash for [static|dynamic] deallocation.
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
ਖਾਲੀ Curl_hash_destroy(ਬਣਤਰ Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  ਜੇ(h->ਸਾਰਣੀ) {
    Curl_hash_clean(h);
    curlx_safefree(h->ਸਾਰਣੀ);
  }
  DEBUGASSERT(h->ਮਾਪ == 0);
  h->slots = 0;
}

/* Removes all the entries in the given hash.
 *
 * @unittest: 1602
 */
ਖਾਲੀ Curl_hash_clean(ਬਣਤਰ Curl_hash *h)
{
  ਜੇ(h && h->ਸਾਰਣੀ) {
    ਬਣਤਰ Curl_hash_element *he, **he_anchor;
    size_t i;
    DEBUGASSERT(h->init == HASHINIT);
    ਲਈ(i = 0; i < h->slots; ++i) {
      he_anchor = &h->ਸਾਰਣੀ[i];
      ਜਦੋਂ(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

size_t Curl_hash_ਗਿਣਤੀ(ਬਣਤਰ Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  ਵਾਪਸ h->ਮਾਪ;
}

/* Cleans all entries that pass the comp function criteria. */
ਖਾਲੀ Curl_hash_clean_with_criterium(ਬਣਤਰ Curl_hash *h, ਖਾਲੀ *user,
                                    ਪੂਰਨ (*comp)(ਖਾਲੀ *, ਖਾਲੀ *))
{
  size_t i;

  ਜੇ(!h || !h->ਸਾਰਣੀ)
    ਵਾਪਸ;

  DEBUGASSERT(h->init == HASHINIT);
  ਲਈ(i = 0; i < h->slots; ++i) {
    ਬਣਤਰ Curl_hash_element *he, **he_anchor = &h->ਸਾਰਣੀ[i];
    ਜਦੋਂ(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      ਜੇ(!comp || comp(user, (*he_anchor)->ptr)) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
      ਨਹੀਂਤਾਂ
        he_anchor = &(*he_anchor)->ਅਗਲਾ;
    }
  }
}

ਖਾਲੀ Curl_hash_ਸ਼ੁਰੂ_iterate(ਬਣਤਰ Curl_hash *hash,
                             ਬਣਤਰ Curl_hash_iterator *iter)
{
  DEBUGASSERT(hash->init == HASHINIT);
  iter->hash = hash;
  iter->ਥਾਂ_ਸੂਚਕਾਂਕ = 0;
  iter->current = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  iter->init = ITERINIT;
#ਜੇ_ਅੰਤ
}

ਬਣਤਰ Curl_hash_element *Curl_hash_ਅਗਲਾ_element(
  ਬਣਤਰ Curl_hash_iterator *iter)
{
  ਬਣਤਰ Curl_hash *h;
  DEBUGASSERT(iter->init == ITERINIT);
  h = iter->hash;
  ਜੇ(!h->ਸਾਰਣੀ)
    ਵਾਪਸ ਨਿਰਾ; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  ਜੇ(iter->current)
    iter->current = iter->current->ਅਗਲਾ;

  /* If we have reached the end of the list, find the next one */
  ਜੇ(!iter->current) {
    size_t i;
    ਲਈ(i = iter->ਥਾਂ_ਸੂਚਕਾਂਕ; i < h->slots; i++) {
      ਜੇ(h->ਸਾਰਣੀ[i]) {
        iter->current = h->ਸਾਰਣੀ[i];
        iter->ਥਾਂ_ਸੂਚਕਾਂਕ = i + 1;
        ਰੋਕੋ;
      }
    }
  }

  ਵਾਪਸ iter->current;
}
