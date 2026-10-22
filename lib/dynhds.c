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

#ਅਨ੍ਤਰ੍ਭਾਵ "dynhds.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"

ਸਥਿਰ ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ_new(ਅਚਲ ਅੱਖਰ *name, size_t namelen,
                                      ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, size_t valuelen,
                                      ਪੂਰਨ opts)
{
  ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *e;
  ਅੱਖਰ *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  ਜੇ((SIZE_MAX - namelen - valuelen) < (ਆਕਾਰ(*e) + 2))
    ਵਾਪਸ ਨਿਰਾ;
  e = curlx_calloc(1, ਆਕਾਰ(*e) + namelen + valuelen + 2);
  ਜੇ(!e)
    ਵਾਪਸ ਨਿਰਾ;
  e->name = p = (ਅੱਖਰ *)e + ਆਕਾਰ(*e);
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(p, name, namelen);
  e->namelen = namelen;
  e->ਮੁੱਲ = p += namelen + 1; /* leave a \0 at the end of name */
  ਜੇ(valuelen) {
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(p, ਮੁੱਲ, valuelen);
    e->valuelen = valuelen;
  }
  ਜੇ(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  ਵਾਪਸ e;
}

ਸਥਿਰ ਖਾਲੀ ਇੰਦਰਾਜ_free(ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *e)
{
  curlx_free(e);
}

ਖਾਲੀ Curl_dynhds_init(ਬਣਤਰ dynhds *dynhds, size_t max_entries,
                      size_t max_strs_ਮਾਪ)
{
  DEBUGASSERT(dynhds);
  DEBUGASSERT(max_strs_ਮਾਪ);
  dynhds->hds = ਨਿਰਾ;
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
  dynhds->max_entries = max_entries;
  dynhds->max_strs_ਮਾਪ = max_strs_ਮਾਪ;
  dynhds->opts = 0;
}

ਖਾਲੀ Curl_dynhds_free(ਬਣਤਰ dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  ਜੇ(dynhds->hds && dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    ਲਈ(i = 0; i < dynhds->hds_len; ++i) {
      ਇੰਦਰਾਜ_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

ਖਾਲੀ Curl_dynhds_reset(ਬਣਤਰ dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  ਜੇ(dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    ਲਈ(i = 0; i < dynhds->hds_len; ++i) {
      ਇੰਦਰਾਜ_free(dynhds->hds[i]);
      dynhds->hds[i] = ਨਿਰਾ;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_ਗਿਣਤੀ(ਬਣਤਰ dynhds *dynhds)
{
  ਵਾਪਸ dynhds->hds_len;
}

ਖਾਲੀ Curl_dynhds_ਸੈੱਟ_opts(ਬਣਤਰ dynhds *dynhds, ਪੂਰਨ opts)
{
  dynhds->opts = opts;
}

ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *Curl_dynhds_getn(ਬਣਤਰ dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  ਵਾਪਸ (n < dynhds->hds_len) ? dynhds->hds[n] : ਨਿਰਾ;
}

ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *Curl_dynhds_get(ਬਣਤਰ dynhds *dynhds, ਅਚਲ ਅੱਖਰ *name,
                                     size_t namelen)
{
  size_t i;
  ਲਈ(i = 0; i < dynhds->hds_len; ++i) {
    ਜੇ(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      ਵਾਪਸ dynhds->hds[i];
    }
  }
  ਵਾਪਸ ਨਿਰਾ;
}

ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *Curl_dynhds_cget(ਬਣਤਰ dynhds *dynhds, ਅਚਲ ਅੱਖਰ *name)
{
  ਵਾਪਸ Curl_dynhds_get(dynhds, name, ਲੜੀ_ਲੰਬਾਈ(name));
}

CURLcode Curl_dynhds_ਜੋੜੋ(ਬਣਤਰ dynhds *dynhds,
                         ਅਚਲ ਅੱਖਰ *name, size_t namelen,
                         ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, size_t valuelen)
{
  ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* Accept NULL value with length 0, name must have length */
  ਜੇ(!dynhds || !name || !namelen || (!ਮੁੱਲ && valuelen)) {
    DEBUGASSERT(0);
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }
  /* Does the new header fit into the limits? */
  ਜੇ((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_ਮਾਪ - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_ਮਾਪ - dynhds->strs_len - namelen < valuelen))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਇੰਦਰਾਜ = ਇੰਦਰਾਜ_new(name, namelen, ਮੁੱਲ, valuelen, dynhds->opts);
  ਜੇ(!ਇੰਦਰਾਜ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    ਬਣਤਰ dynhds_ਇੰਦਰਾਜ **nhds;

    ਜੇ(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, ਆਕਾਰ(ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *));
    ਜੇ(!nhds)
      ਜਾਓ ਬਾਹਰਲਾ;
    ਜੇ(dynhds->hds) {
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(nhds, dynhds->hds,
             dynhds->hds_len * ਆਕਾਰ(ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *));
      curlx_safefree(dynhds->hds);
    }
    dynhds->hds = nhds;
    dynhds->hds_allc = nallc;
  }
  dynhds->hds[dynhds->hds_len++] = ਇੰਦਰਾਜ;
  ਇੰਦਰਾਜ = ਨਿਰਾ;
  dynhds->strs_len += namelen + valuelen;
  ਨਤੀਜਾ = CURLE_OK;

ਬਾਹਰਲਾ:
  ਜੇ(ਇੰਦਰਾਜ)
    ਇੰਦਰਾਜ_free(ਇੰਦਰਾਜ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_dynhds_cadd(ਬਣਤਰ dynhds *dynhds,
                          ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ)
{
  ਜੇ(!name)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਵਾਪਸ Curl_dynhds_ਜੋੜੋ(dynhds, name, ਲੜੀ_ਲੰਬਾਈ(name),
                         ਮੁੱਲ, ਮੁੱਲ ? ਲੜੀ_ਲੰਬਾਈ(ਮੁੱਲ) : 0);
}

CURLcode Curl_dynhds_h1_ਜੋੜੋ_ਸਤਰ(ਬਣਤਰ dynhds *dynhds,
                                 ਅਚਲ ਅੱਖਰ *ਸਤਰ, size_t ਸਤਰ_len)
{
  ਅਚਲ ਅੱਖਰ *p;
  ਅਚਲ ਅੱਖਰ *name;
  size_t namelen;
  ਅਚਲ ਅੱਖਰ *ਮੁੱਲ;
  size_t valuelen, i;

  ਜੇ(!ਸਤਰ || !ਸਤਰ_len)
    ਵਾਪਸ CURLE_OK;

  p = memchr(ਸਤਰ, ':', ਸਤਰ_len);
  ਜੇ(!p || p == ਸਤਰ)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  name = ਸਤਰ;
  namelen = p - ਸਤਰ;
  p++; /* move past the colon */
  ਲਈ(i = namelen + 1; i < ਸਤਰ_len; ++i, ++p) {
    ਜੇ(!ISBLANK(*p))
      ਰੋਕੋ;
  }
  ਮੁੱਲ = p;
  valuelen = ਸਤਰ_len - i;

  p = memchr(ਮੁੱਲ, '\r', valuelen);
  ਜੇ(!p)
    p = memchr(ਮੁੱਲ, '\n', valuelen);
  ਜੇ(p)
    valuelen = (size_t)(p - ਮੁੱਲ);

  ਵਾਪਸ Curl_dynhds_ਜੋੜੋ(dynhds, name, namelen, ਮੁੱਲ, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_ਸਤਰ(ਬਣਤਰ dynhds *dynhds, ਅਚਲ ਅੱਖਰ *ਸਤਰ)
{
  ਵਾਪਸ Curl_dynhds_h1_ਜੋੜੋ_ਸਤਰ(dynhds, ਸਤਰ, ਸਤਰ ? ਲੜੀ_ਲੰਬਾਈ(ਸਤਰ) : 0);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_ਸ਼ਾਮਲ_ਹੈ(ਬਣਤਰ dynhds *dynhds,
                              ਅਚਲ ਅੱਖਰ *name, size_t namelen);
UNITTEST bool dynhds_ਸ਼ਾਮਲ_ਹੈ(ਬਣਤਰ dynhds *dynhds,
                              ਅਚਲ ਅੱਖਰ *name, size_t namelen)
{
  ਵਾਪਸ !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(ਬਣਤਰ dynhds *dynhds, ਅਚਲ ਅੱਖਰ *name);
UNITTEST bool dynhds_ccontains(ਬਣਤਰ dynhds *dynhds, ਅਚਲ ਅੱਖਰ *name)
{
  ਵਾਪਸ dynhds_ਸ਼ਾਮਲ_ਹੈ(dynhds, name, ਲੜੀ_ਲੰਬਾਈ(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ਗਿਣਤੀ_name(ਬਣਤਰ dynhds *dynhds,
                                  ਅਚਲ ਅੱਖਰ *name, size_t namelen);
UNITTEST size_t dynhds_ਗਿਣਤੀ_name(ਬਣਤਰ dynhds *dynhds,
                                  ਅਚਲ ਅੱਖਰ *name, size_t namelen)
{
  size_t n = 0;
  ਜੇ(dynhds->hds_len) {
    size_t i;
    ਲਈ(i = 0; i < dynhds->hds_len; ++i) {
      ਜੇ((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  ਵਾਪਸ n;
}

/**
 * Return how often the given null-terminated name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ccount_name(ਬਣਤਰ dynhds *dynhds,
                                        ਅਚਲ ਅੱਖਰ *name);
UNITTEST size_t dynhds_ccount_name(ਬਣਤਰ dynhds *dynhds,
                                        ਅਚਲ ਅੱਖਰ *name)
{
  ਵਾਪਸ dynhds_ਗਿਣਤੀ_name(dynhds, name, ਲੜੀ_ਲੰਬਾਈ(name));
}

/**
 * Remove all entries with the given name.
 * Returns number of entries removed.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_remove(ਬਣਤਰ dynhds *dynhds,
                              ਅਚਲ ਅੱਖਰ *name, size_t namelen);
UNITTEST size_t dynhds_remove(ਬਣਤਰ dynhds *dynhds,
                              ਅਚਲ ਅੱਖਰ *name, size_t namelen)
{
  size_t n = 0;
  ਜੇ(dynhds->hds_len) {
    size_t i, len;
    ਲਈ(i = 0; i < dynhds->hds_len; ++i) {
      ਜੇ((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        ਇੰਦਰਾਜ_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        ਜੇ(len) {
          memmove(&dynhds->hds[i], &dynhds->hds[i + 1],
                  len * ਆਕਾਰ(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  ਵਾਪਸ n;
}

/**
 * Set the give header name and value, replacing any entries with
 * the same name. The header is added at the end of all (remaining)
 * entries.
 *
 * @unittest 2602
 */
UNITTEST CURLcode dynhds_ਸੈੱਟ(ਬਣਤਰ dynhds *dynhds,
                             ਅਚਲ ਅੱਖਰ *name, size_t namelen,
                             ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, size_t valuelen);
UNITTEST CURLcode dynhds_ਸੈੱਟ(ਬਣਤਰ dynhds *dynhds,
                             ਅਚਲ ਅੱਖਰ *name, size_t namelen,
                             ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, size_t valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  ਵਾਪਸ Curl_dynhds_ਜੋੜੋ(dynhds, name, namelen, ਮੁੱਲ, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(ਬਣਤਰ dynhds *dynhds, ਅਚਲ ਅੱਖਰ *name);
UNITTEST size_t dynhds_cremove(ਬਣਤਰ dynhds *dynhds, ਅਚਲ ਅੱਖਰ *name)
{
  ਵਾਪਸ dynhds_remove(dynhds, name, ਲੜੀ_ਲੰਬਾਈ(name));
}

#ਜੇ_ਅੰਤ /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(ਬਣਤਰ dynhds *dynhds, ਬਣਤਰ dynbuf *dbuf)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t i;

  ਜੇ(!dynhds->hds_len)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਲਈ(i = 0; i < dynhds->hds_len; ++i) {
    ਨਤੀਜਾ = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_addn(dbuf, STRCONST(": "));
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_addn(dbuf, dynhds->hds[i]->ਮੁੱਲ,
                              dynhds->hds[i]->valuelen);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NGHTTP2

nghttp2_nv *Curl_dynhds_ਮੰਜ਼ਿਲ_nva(ਬਣਤਰ dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, ਆਕਾਰ(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  ਜੇ(!nva)
    ਵਾਪਸ ਨਿਰਾ;

  ਲਈ(i = 0; i < dynhds->hds_len; ++i) {
    ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].ਮੁੱਲ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)e->ਮੁੱਲ;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  ਵਾਪਸ nva;
}

#ਜੇ_ਅੰਤ /* USE_NGHTTP2 */
