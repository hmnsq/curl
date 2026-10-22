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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "dynhds.h"
#অন্তর্ভাব "strcase.h"

স্থির কাঠামো dynhds_entry *entry_new(ধ্রুবক অক্ষর *name, size_t namelen,
                                      ধ্রুবক অক্ষর *মান, size_t valuelen,
                                      পূর্ণসংখ্যা opts)
{
  কাঠামো dynhds_entry *e;
  অক্ষর *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  যদি((SIZE_MAX - namelen - valuelen) < (আকার(*e) + 2))
    ফেরত ফাঁকা;
  e = curlx_calloc(1, আকার(*e) + namelen + valuelen + 2);
  যদি(!e)
    ফেরত ফাঁকা;
  e->name = p = (অক্ষর *)e + আকার(*e);
  memcpy(p, name, namelen);
  e->namelen = namelen;
  e->মান = p += namelen + 1; /* leave a \0 at the end of name */
  যদি(valuelen) {
    memcpy(p, মান, valuelen);
    e->valuelen = valuelen;
  }
  যদি(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  ফেরত e;
}

স্থির শূন্য entry_free(কাঠামো dynhds_entry *e)
{
  curlx_free(e);
}

শূন্য Curl_dynhds_init(কাঠামো dynhds *dynhds, size_t max_entries,
                      size_t max_strs_আকার)
{
  DEBUGASSERT(dynhds);
  DEBUGASSERT(max_strs_আকার);
  dynhds->hds = ফাঁকা;
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
  dynhds->max_entries = max_entries;
  dynhds->max_strs_আকার = max_strs_আকার;
  dynhds->opts = 0;
}

শূন্য Curl_dynhds_free(কাঠামো dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  যদি(dynhds->hds && dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    জন্য(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

শূন্য Curl_dynhds_reset(কাঠামো dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  যদি(dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    জন্য(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
      dynhds->hds[i] = ফাঁকা;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_গুনতি(কাঠামো dynhds *dynhds)
{
  ফেরত dynhds->hds_len;
}

শূন্য Curl_dynhds_set_opts(কাঠামো dynhds *dynhds, পূর্ণসংখ্যা opts)
{
  dynhds->opts = opts;
}

কাঠামো dynhds_entry *Curl_dynhds_getn(কাঠামো dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  ফেরত (n < dynhds->hds_len) ? dynhds->hds[n] : ফাঁকা;
}

কাঠামো dynhds_entry *Curl_dynhds_get(কাঠামো dynhds *dynhds, ধ্রুবক অক্ষর *name,
                                     size_t namelen)
{
  size_t i;
  জন্য(i = 0; i < dynhds->hds_len; ++i) {
    যদি(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      ফেরত dynhds->hds[i];
    }
  }
  ফেরত ফাঁকা;
}

কাঠামো dynhds_entry *Curl_dynhds_cget(কাঠামো dynhds *dynhds, ধ্রুবক অক্ষর *name)
{
  ফেরত Curl_dynhds_get(dynhds, name, strlen(name));
}

CURLcode Curl_dynhds_add(কাঠামো dynhds *dynhds,
                         ধ্রুবক অক্ষর *name, size_t namelen,
                         ধ্রুবক অক্ষর *মান, size_t valuelen)
{
  কাঠামো dynhds_entry *entry = ফাঁকা;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;

  /* Accept NULL value with length 0, name must have length */
  যদি(!dynhds || !name || !namelen || (!মান && valuelen)) {
    DEBUGASSERT(0);
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }
  /* Does the new header fit into the limits? */
  যদি((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_আকার - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_আকার - dynhds->strs_len - namelen < valuelen))
    ফেরত CURLE_OUT_OF_MEMORY;

  entry = entry_new(name, namelen, মান, valuelen, dynhds->opts);
  যদি(!entry)
    যাও out;

  যদি(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    কাঠামো dynhds_entry **nhds;

    যদি(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, আকার(কাঠামো dynhds_entry *));
    যদি(!nhds)
      যাও out;
    যদি(dynhds->hds) {
      memcpy(nhds, dynhds->hds,
             dynhds->hds_len * আকার(কাঠামো dynhds_entry *));
      curlx_safefree(dynhds->hds);
    }
    dynhds->hds = nhds;
    dynhds->hds_allc = nallc;
  }
  dynhds->hds[dynhds->hds_len++] = entry;
  entry = ফাঁকা;
  dynhds->strs_len += namelen + valuelen;
  ফলাফল = CURLE_OK;

out:
  যদি(entry)
    entry_free(entry);
  ফেরত ফলাফল;
}

CURLcode Curl_dynhds_cadd(কাঠামো dynhds *dynhds,
                          ধ্রুবক অক্ষর *name, ধ্রুবক অক্ষর *মান)
{
  যদি(!name)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  ফেরত Curl_dynhds_add(dynhds, name, strlen(name),
                         মান, মান ? strlen(মান) : 0);
}

CURLcode Curl_dynhds_h1_add_সারি(কাঠামো dynhds *dynhds,
                                 ধ্রুবক অক্ষর *সারি, size_t সারি_len)
{
  ধ্রুবক অক্ষর *p;
  ধ্রুবক অক্ষর *name;
  size_t namelen;
  ধ্রুবক অক্ষর *মান;
  size_t valuelen, i;

  যদি(!সারি || !সারি_len)
    ফেরত CURLE_OK;

  p = memchr(সারি, ':', সারি_len);
  যদি(!p || p == সারি)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  name = সারি;
  namelen = p - সারি;
  p++; /* move past the colon */
  জন্য(i = namelen + 1; i < সারি_len; ++i, ++p) {
    যদি(!ISBLANK(*p))
      বিরতি;
  }
  মান = p;
  valuelen = সারি_len - i;

  p = memchr(মান, '\r', valuelen);
  যদি(!p)
    p = memchr(মান, '\n', valuelen);
  যদি(p)
    valuelen = (size_t)(p - মান);

  ফেরত Curl_dynhds_add(dynhds, name, namelen, মান, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_সারি(কাঠামো dynhds *dynhds, ধ্রুবক অক্ষর *সারি)
{
  ফেরত Curl_dynhds_h1_add_সারি(dynhds, সারি, সারি ? strlen(সারি) : 0);
}

#ifdef UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_contains(কাঠামো dynhds *dynhds,
                              ধ্রুবক অক্ষর *name, size_t namelen);
UNITTEST bool dynhds_contains(কাঠামো dynhds *dynhds,
                              ধ্রুবক অক্ষর *name, size_t namelen)
{
  ফেরত !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(কাঠামো dynhds *dynhds, ধ্রুবক অক্ষর *name);
UNITTEST bool dynhds_ccontains(কাঠামো dynhds *dynhds, ধ্রুবক অক্ষর *name)
{
  ফেরত dynhds_contains(dynhds, name, strlen(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_গুনতি_name(কাঠামো dynhds *dynhds,
                                  ধ্রুবক অক্ষর *name, size_t namelen);
UNITTEST size_t dynhds_গুনতি_name(কাঠামো dynhds *dynhds,
                                  ধ্রুবক অক্ষর *name, size_t namelen)
{
  size_t n = 0;
  যদি(dynhds->hds_len) {
    size_t i;
    জন্য(i = 0; i < dynhds->hds_len; ++i) {
      যদি((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  ফেরত n;
}

/**
 * Return how often the given null-terminated name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ccount_name(কাঠামো dynhds *dynhds,
                                        ধ্রুবক অক্ষর *name);
UNITTEST size_t dynhds_ccount_name(কাঠামো dynhds *dynhds,
                                        ধ্রুবক অক্ষর *name)
{
  ফেরত dynhds_গুনতি_name(dynhds, name, strlen(name));
}

/**
 * Remove all entries with the given name.
 * Returns number of entries removed.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_remove(কাঠামো dynhds *dynhds,
                              ধ্রুবক অক্ষর *name, size_t namelen);
UNITTEST size_t dynhds_remove(কাঠামো dynhds *dynhds,
                              ধ্রুবক অক্ষর *name, size_t namelen)
{
  size_t n = 0;
  যদি(dynhds->hds_len) {
    size_t i, len;
    জন্য(i = 0; i < dynhds->hds_len; ++i) {
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
                  len * আকার(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  ফেরত n;
}

/**
 * Set the give header name and value, replacing any entries with
 * the same name. The header is added at the end of all (remaining)
 * entries.
 *
 * @unittest 2602
 */
UNITTEST CURLcode dynhds_set(কাঠামো dynhds *dynhds,
                             ধ্রুবক অক্ষর *name, size_t namelen,
                             ধ্রুবক অক্ষর *মান, size_t valuelen);
UNITTEST CURLcode dynhds_set(কাঠামো dynhds *dynhds,
                             ধ্রুবক অক্ষর *name, size_t namelen,
                             ধ্রুবক অক্ষর *মান, size_t valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  ফেরত Curl_dynhds_add(dynhds, name, namelen, মান, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(কাঠামো dynhds *dynhds, ধ্রুবক অক্ষর *name);
UNITTEST size_t dynhds_cremove(কাঠামো dynhds *dynhds, ধ্রুবক অক্ষর *name)
{
  ফেরত dynhds_remove(dynhds, name, strlen(name));
}

#endif /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(কাঠামো dynhds *dynhds, কাঠামো dynbuf *dbuf)
{
  CURLcode ফলাফল = CURLE_OK;
  size_t i;

  যদি(!dynhds->hds_len)
    ফেরত ফলাফল;

  জন্য(i = 0; i < dynhds->hds_len; ++i) {
    ফলাফল = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_addn(dbuf, STRCONST(": "));
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_addn(dbuf, dynhds->hds[i]->মান,
                              dynhds->hds[i]->valuelen);
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    যদি(ফলাফল)
      বিরতি;
  }

  ফেরত ফলাফল;
}

#ifdef USE_NGHTTP2

nghttp2_nv *Curl_dynhds_to_nva(কাঠামো dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, আকার(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  যদি(!nva)
    ফেরত ফাঁকা;

  জন্য(i = 0; i < dynhds->hds_len; ++i) {
    কাঠামো dynhds_entry *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (unsigned অক্ষর *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].মান = (unsigned অক্ষর *)e->মান;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  ফেরত nva;
}

#endif /* USE_NGHTTP2 */
