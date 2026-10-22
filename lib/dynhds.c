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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "dynhds.h"
#அந்தர்பாவ "strcase.h"

நிலையான கட்டமைப்பு dynhds_entry *entry_new(மாறிலி எழுத்து *name, size_t namelen,
                                      மாறிலி எழுத்து *மதிப்பு, size_t valuelen,
                                      முழு opts)
{
  கட்டமைப்பு dynhds_entry *e;
  எழுத்து *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  எனில்((SIZE_MAX - namelen - valuelen) < (அளவு(*e) + 2))
    திரும்பு வெற்று;
  e = curlx_calloc(1, அளவு(*e) + namelen + valuelen + 2);
  எனில்(!e)
    திரும்பு வெற்று;
  e->name = p = (எழுத்து *)e + அளவு(*e);
  memcpy(p, name, namelen);
  e->namelen = namelen;
  e->மதிப்பு = p += namelen + 1; /* leave a \0 at the end of name */
  எனில்(valuelen) {
    memcpy(p, மதிப்பு, valuelen);
    e->valuelen = valuelen;
  }
  எனில்(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  திரும்பு e;
}

நிலையான வெறுமை entry_free(கட்டமைப்பு dynhds_entry *e)
{
  curlx_free(e);
}

வெறுமை Curl_dynhds_init(கட்டமைப்பு dynhds *dynhds, size_t max_entries,
                      size_t max_strs_size)
{
  DEBUGASSERT(dynhds);
  DEBUGASSERT(max_strs_size);
  dynhds->hds = வெற்று;
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
  dynhds->max_entries = max_entries;
  dynhds->max_strs_size = max_strs_size;
  dynhds->opts = 0;
}

வெறுமை Curl_dynhds_free(கட்டமைப்பு dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  எனில்(dynhds->hds && dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    க்கு(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

வெறுமை Curl_dynhds_reset(கட்டமைப்பு dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  எனில்(dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    க்கு(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
      dynhds->hds[i] = வெற்று;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_மொத்தம்(கட்டமைப்பு dynhds *dynhds)
{
  திரும்பு dynhds->hds_len;
}

வெறுமை Curl_dynhds_set_opts(கட்டமைப்பு dynhds *dynhds, முழு opts)
{
  dynhds->opts = opts;
}

கட்டமைப்பு dynhds_entry *Curl_dynhds_getn(கட்டமைப்பு dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  திரும்பு (n < dynhds->hds_len) ? dynhds->hds[n] : வெற்று;
}

கட்டமைப்பு dynhds_entry *Curl_dynhds_get(கட்டமைப்பு dynhds *dynhds, மாறிலி எழுத்து *name,
                                     size_t namelen)
{
  size_t i;
  க்கு(i = 0; i < dynhds->hds_len; ++i) {
    எனில்(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      திரும்பு dynhds->hds[i];
    }
  }
  திரும்பு வெற்று;
}

கட்டமைப்பு dynhds_entry *Curl_dynhds_cget(கட்டமைப்பு dynhds *dynhds, மாறிலி எழுத்து *name)
{
  திரும்பு Curl_dynhds_get(dynhds, name, strlen(name));
}

CURLcode Curl_dynhds_add(கட்டமைப்பு dynhds *dynhds,
                         மாறிலி எழுத்து *name, size_t namelen,
                         மாறிலி எழுத்து *மதிப்பு, size_t valuelen)
{
  கட்டமைப்பு dynhds_entry *entry = வெற்று;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* Accept NULL value with length 0, name must have length */
  எனில்(!dynhds || !name || !namelen || (!மதிப்பு && valuelen)) {
    DEBUGASSERT(0);
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }
  /* Does the new header fit into the limits? */
  எனில்((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_size - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_size - dynhds->strs_len - namelen < valuelen))
    திரும்பு CURLE_OUT_OF_MEMORY;

  entry = entry_new(name, namelen, மதிப்பு, valuelen, dynhds->opts);
  எனில்(!entry)
    செல் out;

  எனில்(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    கட்டமைப்பு dynhds_entry **nhds;

    எனில்(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, அளவு(கட்டமைப்பு dynhds_entry *));
    எனில்(!nhds)
      செல் out;
    எனில்(dynhds->hds) {
      memcpy(nhds, dynhds->hds,
             dynhds->hds_len * அளவு(கட்டமைப்பு dynhds_entry *));
      curlx_safefree(dynhds->hds);
    }
    dynhds->hds = nhds;
    dynhds->hds_allc = nallc;
  }
  dynhds->hds[dynhds->hds_len++] = entry;
  entry = வெற்று;
  dynhds->strs_len += namelen + valuelen;
  result = CURLE_OK;

out:
  எனில்(entry)
    entry_free(entry);
  திரும்பு result;
}

CURLcode Curl_dynhds_cadd(கட்டமைப்பு dynhds *dynhds,
                          மாறிலி எழுத்து *name, மாறிலி எழுத்து *மதிப்பு)
{
  எனில்(!name)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  திரும்பு Curl_dynhds_add(dynhds, name, strlen(name),
                         மதிப்பு, மதிப்பு ? strlen(மதிப்பு) : 0);
}

CURLcode Curl_dynhds_h1_add_வரி(கட்டமைப்பு dynhds *dynhds,
                                 மாறிலி எழுத்து *வரி, size_t வரி_len)
{
  மாறிலி எழுத்து *p;
  மாறிலி எழுத்து *name;
  size_t namelen;
  மாறிலி எழுத்து *மதிப்பு;
  size_t valuelen, i;

  எனில்(!வரி || !வரி_len)
    திரும்பு CURLE_OK;

  p = memchr(வரி, ':', வரி_len);
  எனில்(!p || p == வரி)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  name = வரி;
  namelen = p - வரி;
  p++; /* move past the colon */
  க்கு(i = namelen + 1; i < வரி_len; ++i, ++p) {
    எனில்(!ISBLANK(*p))
      நிறுத்து;
  }
  மதிப்பு = p;
  valuelen = வரி_len - i;

  p = memchr(மதிப்பு, '\r', valuelen);
  எனில்(!p)
    p = memchr(மதிப்பு, '\n', valuelen);
  எனில்(p)
    valuelen = (size_t)(p - மதிப்பு);

  திரும்பு Curl_dynhds_add(dynhds, name, namelen, மதிப்பு, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_வரி(கட்டமைப்பு dynhds *dynhds, மாறிலி எழுத்து *வரி)
{
  திரும்பு Curl_dynhds_h1_add_வரி(dynhds, வரி, வரி ? strlen(வரி) : 0);
}

#ifdef UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_contains(கட்டமைப்பு dynhds *dynhds,
                              மாறிலி எழுத்து *name, size_t namelen);
UNITTEST bool dynhds_contains(கட்டமைப்பு dynhds *dynhds,
                              மாறிலி எழுத்து *name, size_t namelen)
{
  திரும்பு !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(கட்டமைப்பு dynhds *dynhds, மாறிலி எழுத்து *name);
UNITTEST bool dynhds_ccontains(கட்டமைப்பு dynhds *dynhds, மாறிலி எழுத்து *name)
{
  திரும்பு dynhds_contains(dynhds, name, strlen(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_மொத்தம்_name(கட்டமைப்பு dynhds *dynhds,
                                  மாறிலி எழுத்து *name, size_t namelen);
UNITTEST size_t dynhds_மொத்தம்_name(கட்டமைப்பு dynhds *dynhds,
                                  மாறிலி எழுத்து *name, size_t namelen)
{
  size_t n = 0;
  எனில்(dynhds->hds_len) {
    size_t i;
    க்கு(i = 0; i < dynhds->hds_len; ++i) {
      எனில்((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  திரும்பு n;
}

/**
 * Return how often the given null-terminated name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ccount_name(கட்டமைப்பு dynhds *dynhds,
                                        மாறிலி எழுத்து *name);
UNITTEST size_t dynhds_ccount_name(கட்டமைப்பு dynhds *dynhds,
                                        மாறிலி எழுத்து *name)
{
  திரும்பு dynhds_மொத்தம்_name(dynhds, name, strlen(name));
}

/**
 * Remove all entries with the given name.
 * Returns number of entries removed.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_remove(கட்டமைப்பு dynhds *dynhds,
                              மாறிலி எழுத்து *name, size_t namelen);
UNITTEST size_t dynhds_remove(கட்டமைப்பு dynhds *dynhds,
                              மாறிலி எழுத்து *name, size_t namelen)
{
  size_t n = 0;
  எனில்(dynhds->hds_len) {
    size_t i, len;
    க்கு(i = 0; i < dynhds->hds_len; ++i) {
      எனில்((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        entry_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        எனில்(len) {
          memmove(&dynhds->hds[i], &dynhds->hds[i + 1],
                  len * அளவு(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  திரும்பு n;
}

/**
 * Set the give header name and value, replacing any entries with
 * the same name. The header is added at the end of all (remaining)
 * entries.
 *
 * @unittest 2602
 */
UNITTEST CURLcode dynhds_set(கட்டமைப்பு dynhds *dynhds,
                             மாறிலி எழுத்து *name, size_t namelen,
                             மாறிலி எழுத்து *மதிப்பு, size_t valuelen);
UNITTEST CURLcode dynhds_set(கட்டமைப்பு dynhds *dynhds,
                             மாறிலி எழுத்து *name, size_t namelen,
                             மாறிலி எழுத்து *மதிப்பு, size_t valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  திரும்பு Curl_dynhds_add(dynhds, name, namelen, மதிப்பு, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(கட்டமைப்பு dynhds *dynhds, மாறிலி எழுத்து *name);
UNITTEST size_t dynhds_cremove(கட்டமைப்பு dynhds *dynhds, மாறிலி எழுத்து *name)
{
  திரும்பு dynhds_remove(dynhds, name, strlen(name));
}

#endif /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(கட்டமைப்பு dynhds *dynhds, கட்டமைப்பு dynbuf *dbuf)
{
  CURLcode result = CURLE_OK;
  size_t i;

  எனில்(!dynhds->hds_len)
    திரும்பு result;

  க்கு(i = 0; i < dynhds->hds_len; ++i) {
    result = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    எனில்(!result)
      result = curlx_dyn_addn(dbuf, STRCONST(": "));
    எனில்(!result)
      result = curlx_dyn_addn(dbuf, dynhds->hds[i]->மதிப்பு,
                              dynhds->hds[i]->valuelen);
    எனில்(!result)
      result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    எனில்(result)
      நிறுத்து;
  }

  திரும்பு result;
}

#ifdef USE_NGHTTP2

nghttp2_nv *Curl_dynhds_to_nva(கட்டமைப்பு dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, அளவு(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  எனில்(!nva)
    திரும்பு வெற்று;

  க்கு(i = 0; i < dynhds->hds_len; ++i) {
    கட்டமைப்பு dynhds_entry *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (unsigned எழுத்து *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].மதிப்பு = (unsigned எழுத்து *)e->மதிப்பு;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  திரும்பு nva;
}

#endif /* USE_NGHTTP2 */
