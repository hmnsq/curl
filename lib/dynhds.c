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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "dynhds.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"

ସ୍ଥିର ସଂରଚନା dynhds_entry *entry_new(ଅଚଳ ଅକ୍ଷର *name, size_t namelen,
                                      ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, size_t valuelen,
                                      ପୂର୍ଣ୍ଣସଂଖ୍ୟା opts)
{
  ସଂରଚନା dynhds_entry *e;
  ଅକ୍ଷର *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  ଯଦି((SIZE_MAX - namelen - valuelen) < (ଆକାର(*e) + 2))
    ଫେରନ୍ତୁ ଖାଲି;
  e = curlx_calloc(1, ଆକାର(*e) + namelen + valuelen + 2);
  ଯଦି(!e)
    ଫେରନ୍ତୁ ଖାଲି;
  e->name = p = (ଅକ୍ଷର *)e + ଆକାର(*e);
  memcpy(p, name, namelen);
  e->namelen = namelen;
  e->ମୂଲ୍ୟ = p += namelen + 1; /* leave a \0 at the end of name */
  ଯଦି(valuelen) {
    memcpy(p, ମୂଲ୍ୟ, valuelen);
    e->valuelen = valuelen;
  }
  ଯଦି(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  ଫେରନ୍ତୁ e;
}

ସ୍ଥିର ଶୂନ୍ୟ entry_free(ସଂରଚନା dynhds_entry *e)
{
  curlx_free(e);
}

ଶୂନ୍ୟ Curl_dynhds_init(ସଂରଚନା dynhds *dynhds, size_t max_entries,
                      size_t max_strs_size)
{
  DEBUGASSERT(dynhds);
  DEBUGASSERT(max_strs_size);
  dynhds->hds = ଖାଲି;
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
  dynhds->max_entries = max_entries;
  dynhds->max_strs_size = max_strs_size;
  dynhds->opts = 0;
}

ଶୂନ୍ୟ Curl_dynhds_free(ସଂରଚନା dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  ଯଦି(dynhds->hds && dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    ପାଇଁ(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

ଶୂନ୍ୟ Curl_dynhds_reset(ସଂରଚନା dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  ଯଦି(dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    ପାଇଁ(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
      dynhds->hds[i] = ଖାଲି;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_ଗଣନା(ସଂରଚନା dynhds *dynhds)
{
  ଫେରନ୍ତୁ dynhds->hds_len;
}

ଶୂନ୍ୟ Curl_dynhds_set_opts(ସଂରଚନା dynhds *dynhds, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opts)
{
  dynhds->opts = opts;
}

ସଂରଚନା dynhds_entry *Curl_dynhds_getn(ସଂରଚନା dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  ଫେରନ୍ତୁ (n < dynhds->hds_len) ? dynhds->hds[n] : ଖାଲି;
}

ସଂରଚନା dynhds_entry *Curl_dynhds_get(ସଂରଚନା dynhds *dynhds, ଅଚଳ ଅକ୍ଷର *name,
                                     size_t namelen)
{
  size_t i;
  ପାଇଁ(i = 0; i < dynhds->hds_len; ++i) {
    ଯଦି(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      ଫେରନ୍ତୁ dynhds->hds[i];
    }
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

ସଂରଚନା dynhds_entry *Curl_dynhds_cget(ସଂରଚନା dynhds *dynhds, ଅଚଳ ଅକ୍ଷର *name)
{
  ଫେରନ୍ତୁ Curl_dynhds_get(dynhds, name, strlen(name));
}

CURLcode Curl_dynhds_add(ସଂରଚନା dynhds *dynhds,
                         ଅଚଳ ଅକ୍ଷର *name, size_t namelen,
                         ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, size_t valuelen)
{
  ସଂରଚନା dynhds_entry *entry = ଖାଲି;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* Accept NULL value with length 0, name must have length */
  ଯଦି(!dynhds || !name || !namelen || (!ମୂଲ୍ୟ && valuelen)) {
    DEBUGASSERT(0);
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  /* Does the new header fit into the limits? */
  ଯଦି((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_size - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_size - dynhds->strs_len - namelen < valuelen))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  entry = entry_new(name, namelen, ମୂଲ୍ୟ, valuelen, dynhds->opts);
  ଯଦି(!entry)
    ଯାଅ out;

  ଯଦି(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    ସଂରଚନା dynhds_entry **nhds;

    ଯଦି(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, ଆକାର(ସଂରଚନା dynhds_entry *));
    ଯଦି(!nhds)
      ଯାଅ out;
    ଯଦି(dynhds->hds) {
      memcpy(nhds, dynhds->hds,
             dynhds->hds_len * ଆକାର(ସଂରଚନା dynhds_entry *));
      curlx_safefree(dynhds->hds);
    }
    dynhds->hds = nhds;
    dynhds->hds_allc = nallc;
  }
  dynhds->hds[dynhds->hds_len++] = entry;
  entry = ଖାଲି;
  dynhds->strs_len += namelen + valuelen;
  result = CURLE_OK;

out:
  ଯଦି(entry)
    entry_free(entry);
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_dynhds_cadd(ସଂରଚନା dynhds *dynhds,
                          ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ)
{
  ଯଦି(!name)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ଫେରନ୍ତୁ Curl_dynhds_add(dynhds, name, strlen(name),
                         ମୂଲ୍ୟ, ମୂଲ୍ୟ ? strlen(ମୂଲ୍ୟ) : 0);
}

CURLcode Curl_dynhds_h1_add_ଧାଡ଼ି(ସଂରଚନା dynhds *dynhds,
                                 ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି, size_t ଧାଡ଼ି_len)
{
  ଅଚଳ ଅକ୍ଷର *p;
  ଅଚଳ ଅକ୍ଷର *name;
  size_t namelen;
  ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ;
  size_t valuelen, i;

  ଯଦି(!ଧାଡ଼ି || !ଧାଡ଼ି_len)
    ଫେରନ୍ତୁ CURLE_OK;

  p = memchr(ଧାଡ଼ି, ':', ଧାଡ଼ି_len);
  ଯଦି(!p || p == ଧାଡ଼ି)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  name = ଧାଡ଼ି;
  namelen = p - ଧାଡ଼ି;
  p++; /* move past the colon */
  ପାଇଁ(i = namelen + 1; i < ଧାଡ଼ି_len; ++i, ++p) {
    ଯଦି(!ISBLANK(*p))
      ଭାଙ୍ଗ;
  }
  ମୂଲ୍ୟ = p;
  valuelen = ଧାଡ଼ି_len - i;

  p = memchr(ମୂଲ୍ୟ, '\r', valuelen);
  ଯଦି(!p)
    p = memchr(ମୂଲ୍ୟ, '\n', valuelen);
  ଯଦି(p)
    valuelen = (size_t)(p - ମୂଲ୍ୟ);

  ଫେରନ୍ତୁ Curl_dynhds_add(dynhds, name, namelen, ମୂଲ୍ୟ, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_ଧାଡ଼ି(ସଂରଚନା dynhds *dynhds, ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି)
{
  ଫେରନ୍ତୁ Curl_dynhds_h1_add_ଧାଡ଼ି(dynhds, ଧାଡ଼ି, ଧାଡ଼ି ? strlen(ଧାଡ଼ି) : 0);
}

#ifdef UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_contains(ସଂରଚନା dynhds *dynhds,
                              ଅଚଳ ଅକ୍ଷର *name, size_t namelen);
UNITTEST bool dynhds_contains(ସଂରଚନା dynhds *dynhds,
                              ଅଚଳ ଅକ୍ଷର *name, size_t namelen)
{
  ଫେରନ୍ତୁ !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(ସଂରଚନା dynhds *dynhds, ଅଚଳ ଅକ୍ଷର *name);
UNITTEST bool dynhds_ccontains(ସଂରଚନା dynhds *dynhds, ଅଚଳ ଅକ୍ଷର *name)
{
  ଫେରନ୍ତୁ dynhds_contains(dynhds, name, strlen(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ଗଣନା_name(ସଂରଚନା dynhds *dynhds,
                                  ଅଚଳ ଅକ୍ଷର *name, size_t namelen);
UNITTEST size_t dynhds_ଗଣନା_name(ସଂରଚନା dynhds *dynhds,
                                  ଅଚଳ ଅକ୍ଷର *name, size_t namelen)
{
  size_t n = 0;
  ଯଦି(dynhds->hds_len) {
    size_t i;
    ପାଇଁ(i = 0; i < dynhds->hds_len; ++i) {
      ଯଦି((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  ଫେରନ୍ତୁ n;
}

/**
 * Return how often the given null-terminated name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ccount_name(ସଂରଚନା dynhds *dynhds,
                                        ଅଚଳ ଅକ୍ଷର *name);
UNITTEST size_t dynhds_ccount_name(ସଂରଚନା dynhds *dynhds,
                                        ଅଚଳ ଅକ୍ଷର *name)
{
  ଫେରନ୍ତୁ dynhds_ଗଣନା_name(dynhds, name, strlen(name));
}

/**
 * Remove all entries with the given name.
 * Returns number of entries removed.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_remove(ସଂରଚନା dynhds *dynhds,
                              ଅଚଳ ଅକ୍ଷର *name, size_t namelen);
UNITTEST size_t dynhds_remove(ସଂରଚନା dynhds *dynhds,
                              ଅଚଳ ଅକ୍ଷର *name, size_t namelen)
{
  size_t n = 0;
  ଯଦି(dynhds->hds_len) {
    size_t i, len;
    ପାଇଁ(i = 0; i < dynhds->hds_len; ++i) {
      ଯଦି((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        entry_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        ଯଦି(len) {
          memmove(&dynhds->hds[i], &dynhds->hds[i + 1],
                  len * ଆକାର(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  ଫେରନ୍ତୁ n;
}

/**
 * Set the give header name and value, replacing any entries with
 * the same name. The header is added at the end of all (remaining)
 * entries.
 *
 * @unittest 2602
 */
UNITTEST CURLcode dynhds_set(ସଂରଚନା dynhds *dynhds,
                             ଅଚଳ ଅକ୍ଷର *name, size_t namelen,
                             ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, size_t valuelen);
UNITTEST CURLcode dynhds_set(ସଂରଚନା dynhds *dynhds,
                             ଅଚଳ ଅକ୍ଷର *name, size_t namelen,
                             ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, size_t valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  ଫେରନ୍ତୁ Curl_dynhds_add(dynhds, name, namelen, ମୂଲ୍ୟ, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(ସଂରଚନା dynhds *dynhds, ଅଚଳ ଅକ୍ଷର *name);
UNITTEST size_t dynhds_cremove(ସଂରଚନା dynhds *dynhds, ଅଚଳ ଅକ୍ଷର *name)
{
  ଫେରନ୍ତୁ dynhds_remove(dynhds, name, strlen(name));
}

#endif /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(ସଂରଚନା dynhds *dynhds, ସଂରଚନା dynbuf *dbuf)
{
  CURLcode result = CURLE_OK;
  size_t i;

  ଯଦି(!dynhds->hds_len)
    ଫେରନ୍ତୁ result;

  ପାଇଁ(i = 0; i < dynhds->hds_len; ++i) {
    result = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    ଯଦି(!result)
      result = curlx_dyn_addn(dbuf, STRCONST(": "));
    ଯଦି(!result)
      result = curlx_dyn_addn(dbuf, dynhds->hds[i]->ମୂଲ୍ୟ,
                              dynhds->hds[i]->valuelen);
    ଯଦି(!result)
      result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    ଯଦି(result)
      ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ result;
}

#ifdef USE_NGHTTP2

nghttp2_nv *Curl_dynhds_to_nva(ସଂରଚନା dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, ଆକାର(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  ଯଦି(!nva)
    ଫେରନ୍ତୁ ଖାଲି;

  ପାଇଁ(i = 0; i < dynhds->hds_len; ++i) {
    ସଂରଚନା dynhds_entry *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (unsigned ଅକ୍ଷର *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].ମୂଲ୍ୟ = (unsigned ଅକ୍ଷର *)e->ମୂଲ୍ୟ;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  ଫେରନ୍ତୁ nva;
}

#endif /* USE_NGHTTP2 */
