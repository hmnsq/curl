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

#ಅನ್ತರ್ಭಾವ "dynhds.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"

ಸ್ಥಿರ ಸಂರಚನಾ dynhds_entry *entry_new(ಧ್ರುವ ವರ್ಣ *name, size_t namelen,
                                      ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, size_t valuelen,
                                      ಪೂರ್ಣಾಙ್ಕ opts)
{
  ಸಂರಚನಾ dynhds_entry *e;
  ವರ್ಣ *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  ಯದಿ((SIZE_MAX - namelen - valuelen) < (ಪರಿಮಾಣ(*e) + 2))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  e = curlx_calloc(1, ಪರಿಮಾಣ(*e) + namelen + valuelen + 2);
  ಯದಿ(!e)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  e->name = p = (ವರ್ಣ *)e + ಪರಿಮಾಣ(*e);
  memcpy(p, name, namelen);
  e->namelen = namelen;
  e->ಮೌಲ್ಯ = p += namelen + 1; /* leave a \0 at the end of name */
  ಯದಿ(valuelen) {
    memcpy(p, ಮೌಲ್ಯ, valuelen);
    e->valuelen = valuelen;
  }
  ಯದಿ(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  ಪ್ರತ್ಯಾಗಮ e;
}

ಸ್ಥಿರ ಶೂನ್ಯ entry_free(ಸಂರಚನಾ dynhds_entry *e)
{
  curlx_free(e);
}

ಶೂನ್ಯ Curl_dynhds_init(ಸಂರಚನಾ dynhds *dynhds, size_t max_entries,
                      size_t max_strs_size)
{
  DEBUGASSERT(dynhds);
  DEBUGASSERT(max_strs_size);
  dynhds->hds = ರಿಕ್ತ;
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
  dynhds->max_entries = max_entries;
  dynhds->max_strs_size = max_strs_size;
  dynhds->opts = 0;
}

ಶೂನ್ಯ Curl_dynhds_free(ಸಂರಚನಾ dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  ಯದಿ(dynhds->hds && dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    ಪ್ರತಿ(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

ಶೂನ್ಯ Curl_dynhds_reset(ಸಂರಚನಾ dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  ಯದಿ(dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    ಪ್ರತಿ(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
      dynhds->hds[i] = ರಿಕ್ತ;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_ಎಣಿಕೆ(ಸಂರಚನಾ dynhds *dynhds)
{
  ಪ್ರತ್ಯಾಗಮ dynhds->hds_len;
}

ಶೂನ್ಯ Curl_dynhds_set_opts(ಸಂರಚನಾ dynhds *dynhds, ಪೂರ್ಣಾಙ್ಕ opts)
{
  dynhds->opts = opts;
}

ಸಂರಚನಾ dynhds_entry *Curl_dynhds_getn(ಸಂರಚನಾ dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  ಪ್ರತ್ಯಾಗಮ (n < dynhds->hds_len) ? dynhds->hds[n] : ರಿಕ್ತ;
}

ಸಂರಚನಾ dynhds_entry *Curl_dynhds_get(ಸಂರಚನಾ dynhds *dynhds, ಧ್ರುವ ವರ್ಣ *name,
                                     size_t namelen)
{
  size_t i;
  ಪ್ರತಿ(i = 0; i < dynhds->hds_len; ++i) {
    ಯದಿ(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      ಪ್ರತ್ಯಾಗಮ dynhds->hds[i];
    }
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸಂರಚನಾ dynhds_entry *Curl_dynhds_cget(ಸಂರಚನಾ dynhds *dynhds, ಧ್ರುವ ವರ್ಣ *name)
{
  ಪ್ರತ್ಯಾಗಮ Curl_dynhds_get(dynhds, name, strlen(name));
}

CURLcode Curl_dynhds_add(ಸಂರಚನಾ dynhds *dynhds,
                         ಧ್ರುವ ವರ್ಣ *name, size_t namelen,
                         ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, size_t valuelen)
{
  ಸಂರಚನಾ dynhds_entry *entry = ರಿಕ್ತ;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* Accept NULL value with length 0, name must have length */
  ಯದಿ(!dynhds || !name || !namelen || (!ಮೌಲ್ಯ && valuelen)) {
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  /* Does the new header fit into the limits? */
  ಯದಿ((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_size - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_size - dynhds->strs_len - namelen < valuelen))
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  entry = entry_new(name, namelen, ಮೌಲ್ಯ, valuelen, dynhds->opts);
  ಯದಿ(!entry)
    ಗಚ್ಛ out;

  ಯದಿ(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    ಸಂರಚನಾ dynhds_entry **nhds;

    ಯದಿ(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, ಪರಿಮಾಣ(ಸಂರಚನಾ dynhds_entry *));
    ಯದಿ(!nhds)
      ಗಚ್ಛ out;
    ಯದಿ(dynhds->hds) {
      memcpy(nhds, dynhds->hds,
             dynhds->hds_len * ಪರಿಮಾಣ(ಸಂರಚನಾ dynhds_entry *));
      curlx_safefree(dynhds->hds);
    }
    dynhds->hds = nhds;
    dynhds->hds_allc = nallc;
  }
  dynhds->hds[dynhds->hds_len++] = entry;
  entry = ರಿಕ್ತ;
  dynhds->strs_len += namelen + valuelen;
  result = CURLE_OK;

out:
  ಯದಿ(entry)
    entry_free(entry);
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_dynhds_cadd(ಸಂರಚನಾ dynhds *dynhds,
                          ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ)
{
  ಯದಿ(!name)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ಪ್ರತ್ಯಾಗಮ Curl_dynhds_add(dynhds, name, strlen(name),
                         ಮೌಲ್ಯ, ಮೌಲ್ಯ ? strlen(ಮೌಲ್ಯ) : 0);
}

CURLcode Curl_dynhds_h1_add_ಸಾಲು(ಸಂರಚನಾ dynhds *dynhds,
                                 ಧ್ರುವ ವರ್ಣ *ಸಾಲು, size_t ಸಾಲು_len)
{
  ಧ್ರುವ ವರ್ಣ *p;
  ಧ್ರುವ ವರ್ಣ *name;
  size_t namelen;
  ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ;
  size_t valuelen, i;

  ಯದಿ(!ಸಾಲು || !ಸಾಲು_len)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  p = memchr(ಸಾಲು, ':', ಸಾಲು_len);
  ಯದಿ(!p || p == ಸಾಲು)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  name = ಸಾಲು;
  namelen = p - ಸಾಲು;
  p++; /* move past the colon */
  ಪ್ರತಿ(i = namelen + 1; i < ಸಾಲು_len; ++i, ++p) {
    ಯದಿ(!ISBLANK(*p))
      ಭಙ್ಗ;
  }
  ಮೌಲ್ಯ = p;
  valuelen = ಸಾಲು_len - i;

  p = memchr(ಮೌಲ್ಯ, '\r', valuelen);
  ಯದಿ(!p)
    p = memchr(ಮೌಲ್ಯ, '\n', valuelen);
  ಯದಿ(p)
    valuelen = (size_t)(p - ಮೌಲ್ಯ);

  ಪ್ರತ್ಯಾಗಮ Curl_dynhds_add(dynhds, name, namelen, ಮೌಲ್ಯ, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_ಸಾಲು(ಸಂರಚನಾ dynhds *dynhds, ಧ್ರುವ ವರ್ಣ *ಸಾಲು)
{
  ಪ್ರತ್ಯಾಗಮ Curl_dynhds_h1_add_ಸಾಲು(dynhds, ಸಾಲು, ಸಾಲು ? strlen(ಸಾಲು) : 0);
}

#ifdef UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_contains(ಸಂರಚನಾ dynhds *dynhds,
                              ಧ್ರುವ ವರ್ಣ *name, size_t namelen);
UNITTEST bool dynhds_contains(ಸಂರಚನಾ dynhds *dynhds,
                              ಧ್ರುವ ವರ್ಣ *name, size_t namelen)
{
  ಪ್ರತ್ಯಾಗಮ !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(ಸಂರಚನಾ dynhds *dynhds, ಧ್ರುವ ವರ್ಣ *name);
UNITTEST bool dynhds_ccontains(ಸಂರಚನಾ dynhds *dynhds, ಧ್ರುವ ವರ್ಣ *name)
{
  ಪ್ರತ್ಯಾಗಮ dynhds_contains(dynhds, name, strlen(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ಎಣಿಕೆ_name(ಸಂರಚನಾ dynhds *dynhds,
                                  ಧ್ರುವ ವರ್ಣ *name, size_t namelen);
UNITTEST size_t dynhds_ಎಣಿಕೆ_name(ಸಂರಚನಾ dynhds *dynhds,
                                  ಧ್ರುವ ವರ್ಣ *name, size_t namelen)
{
  size_t n = 0;
  ಯದಿ(dynhds->hds_len) {
    size_t i;
    ಪ್ರತಿ(i = 0; i < dynhds->hds_len; ++i) {
      ಯದಿ((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  ಪ್ರತ್ಯಾಗಮ n;
}

/**
 * Return how often the given null-terminated name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ccount_name(ಸಂರಚನಾ dynhds *dynhds,
                                        ಧ್ರುವ ವರ್ಣ *name);
UNITTEST size_t dynhds_ccount_name(ಸಂರಚನಾ dynhds *dynhds,
                                        ಧ್ರುವ ವರ್ಣ *name)
{
  ಪ್ರತ್ಯಾಗಮ dynhds_ಎಣಿಕೆ_name(dynhds, name, strlen(name));
}

/**
 * Remove all entries with the given name.
 * Returns number of entries removed.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_remove(ಸಂರಚನಾ dynhds *dynhds,
                              ಧ್ರುವ ವರ್ಣ *name, size_t namelen);
UNITTEST size_t dynhds_remove(ಸಂರಚನಾ dynhds *dynhds,
                              ಧ್ರುವ ವರ್ಣ *name, size_t namelen)
{
  size_t n = 0;
  ಯದಿ(dynhds->hds_len) {
    size_t i, len;
    ಪ್ರತಿ(i = 0; i < dynhds->hds_len; ++i) {
      ಯದಿ((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        entry_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        ಯದಿ(len) {
          memmove(&dynhds->hds[i], &dynhds->hds[i + 1],
                  len * ಪರಿಮಾಣ(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ n;
}

/**
 * Set the give header name and value, replacing any entries with
 * the same name. The header is added at the end of all (remaining)
 * entries.
 *
 * @unittest 2602
 */
UNITTEST CURLcode dynhds_set(ಸಂರಚನಾ dynhds *dynhds,
                             ಧ್ರುವ ವರ್ಣ *name, size_t namelen,
                             ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, size_t valuelen);
UNITTEST CURLcode dynhds_set(ಸಂರಚನಾ dynhds *dynhds,
                             ಧ್ರುವ ವರ್ಣ *name, size_t namelen,
                             ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, size_t valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  ಪ್ರತ್ಯಾಗಮ Curl_dynhds_add(dynhds, name, namelen, ಮೌಲ್ಯ, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(ಸಂರಚನಾ dynhds *dynhds, ಧ್ರುವ ವರ್ಣ *name);
UNITTEST size_t dynhds_cremove(ಸಂರಚನಾ dynhds *dynhds, ಧ್ರುವ ವರ್ಣ *name)
{
  ಪ್ರತ್ಯಾಗಮ dynhds_remove(dynhds, name, strlen(name));
}

#endif /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(ಸಂರಚನಾ dynhds *dynhds, ಸಂರಚನಾ dynbuf *dbuf)
{
  CURLcode result = CURLE_OK;
  size_t i;

  ಯದಿ(!dynhds->hds_len)
    ಪ್ರತ್ಯಾಗಮ result;

  ಪ್ರತಿ(i = 0; i < dynhds->hds_len; ++i) {
    result = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    ಯದಿ(!result)
      result = curlx_dyn_addn(dbuf, STRCONST(": "));
    ಯದಿ(!result)
      result = curlx_dyn_addn(dbuf, dynhds->hds[i]->ಮೌಲ್ಯ,
                              dynhds->hds[i]->valuelen);
    ಯದಿ(!result)
      result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    ಯದಿ(result)
      ಭಙ್ಗ;
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef USE_NGHTTP2

nghttp2_nv *Curl_dynhds_to_nva(ಸಂರಚನಾ dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, ಪರಿಮಾಣ(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  ಯದಿ(!nva)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  ಪ್ರತಿ(i = 0; i < dynhds->hds_len; ++i) {
    ಸಂರಚನಾ dynhds_entry *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (unsigned ವರ್ಣ *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].ಮೌಲ್ಯ = (unsigned ವರ್ಣ *)e->ಮೌಲ್ಯ;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  ಪ್ರತ್ಯಾಗಮ nva;
}

#endif /* USE_NGHTTP2 */
