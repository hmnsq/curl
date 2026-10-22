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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "dynhds.h"
#অন্তর্ভাৱ "strcase.h"

স্থির সংরচনা dynhds_entry *entry_new(ধ্রুৱ ৱর্ণ *name, size_t namelen,
                                      ধ্রুৱ ৱর্ণ *মান, size_t valuelen,
                                      পূর্ণাঙ্ক opts)
{
  সংরচনা dynhds_entry *e;
  ৱর্ণ *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  যদি((SIZE_MAX - namelen - valuelen) < (পরিমাণ(*e) + 2))
    ঘূৰাই রিক্ত;
  e = curlx_calloc(1, পরিমাণ(*e) + namelen + valuelen + 2);
  যদি(!e)
    ঘূৰাই রিক্ত;
  e->name = p = (ৱর্ণ *)e + পরিমাণ(*e);
  memcpy(p, name, namelen);
  e->namelen = namelen;
  e->মান = p += namelen + 1; /* leave a \0 at the end of name */
  যদি(valuelen) {
    memcpy(p, মান, valuelen);
    e->valuelen = valuelen;
  }
  যদি(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  ঘূৰাই e;
}

স্থির শূন্য entry_free(সংরচনা dynhds_entry *e)
{
  curlx_free(e);
}

শূন্য Curl_dynhds_init(সংরচনা dynhds *dynhds, size_t max_entries,
                      size_t max_strs_size)
{
  DEBUGASSERT(dynhds);
  DEBUGASSERT(max_strs_size);
  dynhds->hds = রিক্ত;
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
  dynhds->max_entries = max_entries;
  dynhds->max_strs_size = max_strs_size;
  dynhds->opts = 0;
}

শূন্য Curl_dynhds_free(সংরচনা dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  যদি(dynhds->hds && dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    বাবে(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

শূন্য Curl_dynhds_reset(সংরচনা dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  যদি(dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    বাবে(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
      dynhds->hds[i] = রিক্ত;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_গুনতি(সংরচনা dynhds *dynhds)
{
  ঘূৰাই dynhds->hds_len;
}

শূন্য Curl_dynhds_set_opts(সংরচনা dynhds *dynhds, পূর্ণাঙ্ক opts)
{
  dynhds->opts = opts;
}

সংরচনা dynhds_entry *Curl_dynhds_getn(সংরচনা dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  ঘূৰাই (n < dynhds->hds_len) ? dynhds->hds[n] : রিক্ত;
}

সংরচনা dynhds_entry *Curl_dynhds_get(সংরচনা dynhds *dynhds, ধ্রুৱ ৱর্ণ *name,
                                     size_t namelen)
{
  size_t i;
  বাবে(i = 0; i < dynhds->hds_len; ++i) {
    যদি(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      ঘূৰাই dynhds->hds[i];
    }
  }
  ঘূৰাই রিক্ত;
}

সংরচনা dynhds_entry *Curl_dynhds_cget(সংরচনা dynhds *dynhds, ধ্রুৱ ৱর্ণ *name)
{
  ঘূৰাই Curl_dynhds_get(dynhds, name, strlen(name));
}

CURLcode Curl_dynhds_add(সংরচনা dynhds *dynhds,
                         ধ্রুৱ ৱর্ণ *name, size_t namelen,
                         ধ্রুৱ ৱর্ণ *মান, size_t valuelen)
{
  সংরচনা dynhds_entry *entry = রিক্ত;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* Accept NULL value with length 0, name must have length */
  যদি(!dynhds || !name || !namelen || (!মান && valuelen)) {
    DEBUGASSERT(0);
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  }
  /* Does the new header fit into the limits? */
  যদি((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_size - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_size - dynhds->strs_len - namelen < valuelen))
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  entry = entry_new(name, namelen, মান, valuelen, dynhds->opts);
  যদি(!entry)
    গচ্ছ out;

  যদি(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    সংরচনা dynhds_entry **nhds;

    যদি(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, পরিমাণ(সংরচনা dynhds_entry *));
    যদি(!nhds)
      গচ্ছ out;
    যদি(dynhds->hds) {
      memcpy(nhds, dynhds->hds,
             dynhds->hds_len * পরিমাণ(সংরচনা dynhds_entry *));
      curlx_safefree(dynhds->hds);
    }
    dynhds->hds = nhds;
    dynhds->hds_allc = nallc;
  }
  dynhds->hds[dynhds->hds_len++] = entry;
  entry = রিক্ত;
  dynhds->strs_len += namelen + valuelen;
  result = CURLE_OK;

out:
  যদি(entry)
    entry_free(entry);
  ঘূৰাই result;
}

CURLcode Curl_dynhds_cadd(সংরচনা dynhds *dynhds,
                          ধ্রুৱ ৱর্ণ *name, ধ্রুৱ ৱর্ণ *মান)
{
  যদি(!name)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  ঘূৰাই Curl_dynhds_add(dynhds, name, strlen(name),
                         মান, মান ? strlen(মান) : 0);
}

CURLcode Curl_dynhds_h1_add_সাৰি(সংরচনা dynhds *dynhds,
                                 ধ্রুৱ ৱর্ণ *সাৰি, size_t সাৰি_len)
{
  ধ্রুৱ ৱর্ণ *p;
  ধ্রুৱ ৱর্ণ *name;
  size_t namelen;
  ধ্রুৱ ৱর্ণ *মান;
  size_t valuelen, i;

  যদি(!সাৰি || !সাৰি_len)
    ঘূৰাই CURLE_OK;

  p = memchr(সাৰি, ':', সাৰি_len);
  যদি(!p || p == সাৰি)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  name = সাৰি;
  namelen = p - সাৰি;
  p++; /* move past the colon */
  বাবে(i = namelen + 1; i < সাৰি_len; ++i, ++p) {
    যদি(!ISBLANK(*p))
      ভঙ্গ;
  }
  মান = p;
  valuelen = সাৰি_len - i;

  p = memchr(মান, '\r', valuelen);
  যদি(!p)
    p = memchr(মান, '\n', valuelen);
  যদি(p)
    valuelen = (size_t)(p - মান);

  ঘূৰাই Curl_dynhds_add(dynhds, name, namelen, মান, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_সাৰি(সংরচনা dynhds *dynhds, ধ্রুৱ ৱর্ণ *সাৰি)
{
  ঘূৰাই Curl_dynhds_h1_add_সাৰি(dynhds, সাৰি, সাৰি ? strlen(সাৰি) : 0);
}

#ifdef UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_contains(সংরচনা dynhds *dynhds,
                              ধ্রুৱ ৱর্ণ *name, size_t namelen);
UNITTEST bool dynhds_contains(সংরচনা dynhds *dynhds,
                              ধ্রুৱ ৱর্ণ *name, size_t namelen)
{
  ঘূৰাই !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(সংরচনা dynhds *dynhds, ধ্রুৱ ৱর্ণ *name);
UNITTEST bool dynhds_ccontains(সংরচনা dynhds *dynhds, ধ্রুৱ ৱর্ণ *name)
{
  ঘূৰাই dynhds_contains(dynhds, name, strlen(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_গুনতি_name(সংরচনা dynhds *dynhds,
                                  ধ্রুৱ ৱর্ণ *name, size_t namelen);
UNITTEST size_t dynhds_গুনতি_name(সংরচনা dynhds *dynhds,
                                  ধ্রুৱ ৱর্ণ *name, size_t namelen)
{
  size_t n = 0;
  যদি(dynhds->hds_len) {
    size_t i;
    বাবে(i = 0; i < dynhds->hds_len; ++i) {
      যদি((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  ঘূৰাই n;
}

/**
 * Return how often the given null-terminated name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ccount_name(সংরচনা dynhds *dynhds,
                                        ধ্রুৱ ৱর্ণ *name);
UNITTEST size_t dynhds_ccount_name(সংরচনা dynhds *dynhds,
                                        ধ্রুৱ ৱর্ণ *name)
{
  ঘূৰাই dynhds_গুনতি_name(dynhds, name, strlen(name));
}

/**
 * Remove all entries with the given name.
 * Returns number of entries removed.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_remove(সংরচনা dynhds *dynhds,
                              ধ্রুৱ ৱর্ণ *name, size_t namelen);
UNITTEST size_t dynhds_remove(সংরচনা dynhds *dynhds,
                              ধ্রুৱ ৱর্ণ *name, size_t namelen)
{
  size_t n = 0;
  যদি(dynhds->hds_len) {
    size_t i, len;
    বাবে(i = 0; i < dynhds->hds_len; ++i) {
      যদি((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        entry_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        যদি(len) {
          memmove(&dynhds->hds[i], &dynhds->hds[i + 1],
                  len * পরিমাণ(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  ঘূৰাই n;
}

/**
 * Set the give header name and value, replacing any entries with
 * the same name. The header is added at the end of all (remaining)
 * entries.
 *
 * @unittest 2602
 */
UNITTEST CURLcode dynhds_set(সংরচনা dynhds *dynhds,
                             ধ্রুৱ ৱর্ণ *name, size_t namelen,
                             ধ্রুৱ ৱর্ণ *মান, size_t valuelen);
UNITTEST CURLcode dynhds_set(সংরচনা dynhds *dynhds,
                             ধ্রুৱ ৱর্ণ *name, size_t namelen,
                             ধ্রুৱ ৱর্ণ *মান, size_t valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  ঘূৰাই Curl_dynhds_add(dynhds, name, namelen, মান, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(সংরচনা dynhds *dynhds, ধ্রুৱ ৱর্ণ *name);
UNITTEST size_t dynhds_cremove(সংরচনা dynhds *dynhds, ধ্রুৱ ৱর্ণ *name)
{
  ঘূৰাই dynhds_remove(dynhds, name, strlen(name));
}

#endif /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(সংরচনা dynhds *dynhds, সংরচনা dynbuf *dbuf)
{
  CURLcode result = CURLE_OK;
  size_t i;

  যদি(!dynhds->hds_len)
    ঘূৰাই result;

  বাবে(i = 0; i < dynhds->hds_len; ++i) {
    result = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    যদি(!result)
      result = curlx_dyn_addn(dbuf, STRCONST(": "));
    যদি(!result)
      result = curlx_dyn_addn(dbuf, dynhds->hds[i]->মান,
                              dynhds->hds[i]->valuelen);
    যদি(!result)
      result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    যদি(result)
      ভঙ্গ;
  }

  ঘূৰাই result;
}

#ifdef USE_NGHTTP2

nghttp2_nv *Curl_dynhds_to_nva(সংরচনা dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, পরিমাণ(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  যদি(!nva)
    ঘূৰাই রিক্ত;

  বাবে(i = 0; i < dynhds->hds_len; ++i) {
    সংরচনা dynhds_entry *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (unsigned ৱর্ণ *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].মান = (unsigned ৱর্ণ *)e->মান;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  ঘূৰাই nva;
}

#endif /* USE_NGHTTP2 */
