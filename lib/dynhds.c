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

ಸ್ಥಿರ ರಚನೆ dynhds_ನಮೂದು *ನಮೂದು_new(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, size_t namelen,
                                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, size_t valuelen,
                                      ಪೂರ್ಣಾಂಕ opts)
{
  ರಚನೆ dynhds_ನಮೂದು *e;
  ಅಕ್ಷರ *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  ಆದರೆ((SIZE_MAX - namelen - valuelen) < (ಗಾತ್ರ(*e) + 2))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  e = curlx_calloc(1, ಗಾತ್ರ(*e) + namelen + valuelen + 2);
  ಆದರೆ(!e)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  e->name = p = (ಅಕ್ಷರ *)e + ಗಾತ್ರ(*e);
  ಸ್ಮರಣೆ_ನಕಲು(p, name, namelen);
  e->namelen = namelen;
  e->ಮೌಲ್ಯ = p += namelen + 1; /* leave a \0 at the end of name */
  ಆದರೆ(valuelen) {
    ಸ್ಮರಣೆ_ನಕಲು(p, ಮೌಲ್ಯ, valuelen);
    e->valuelen = valuelen;
  }
  ಆದರೆ(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  ಹಿಂತಿರುಗಿ e;
}

ಸ್ಥಿರ ಶೂನ್ಯ ನಮೂದು_free(ರಚನೆ dynhds_ನಮೂದು *e)
{
  curlx_free(e);
}

ಶೂನ್ಯ Curl_dynhds_init(ರಚನೆ dynhds *dynhds, size_t max_entries,
                      size_t max_strs_ಗಾತ್ರ)
{
  DEBUGASSERT(dynhds);
  DEBUGASSERT(max_strs_ಗಾತ್ರ);
  dynhds->hds = ಖಾಲಿ;
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
  dynhds->max_entries = max_entries;
  dynhds->max_strs_ಗಾತ್ರ = max_strs_ಗಾತ್ರ;
  dynhds->opts = 0;
}

ಶೂನ್ಯ Curl_dynhds_free(ರಚನೆ dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  ಆದರೆ(dynhds->hds && dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    ಗಾಗಿ(i = 0; i < dynhds->hds_len; ++i) {
      ನಮೂದು_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

ಶೂನ್ಯ Curl_dynhds_reset(ರಚನೆ dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  ಆದರೆ(dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    ಗಾಗಿ(i = 0; i < dynhds->hds_len; ++i) {
      ನಮೂದು_free(dynhds->hds[i]);
      dynhds->hds[i] = ಖಾಲಿ;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_ಎಣಿಕೆ(ರಚನೆ dynhds *dynhds)
{
  ಹಿಂತಿರುಗಿ dynhds->hds_len;
}

ಶೂನ್ಯ Curl_dynhds_ಗಣ_opts(ರಚನೆ dynhds *dynhds, ಪೂರ್ಣಾಂಕ opts)
{
  dynhds->opts = opts;
}

ರಚನೆ dynhds_ನಮೂದು *Curl_dynhds_getn(ರಚನೆ dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  ಹಿಂತಿರುಗಿ (n < dynhds->hds_len) ? dynhds->hds[n] : ಖಾಲಿ;
}

ರಚನೆ dynhds_ನಮೂದು *Curl_dynhds_get(ರಚನೆ dynhds *dynhds, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                                     size_t namelen)
{
  size_t i;
  ಗಾಗಿ(i = 0; i < dynhds->hds_len; ++i) {
    ಆದರೆ(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      ಹಿಂತಿರುಗಿ dynhds->hds[i];
    }
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ರಚನೆ dynhds_ನಮೂದು *Curl_dynhds_cget(ರಚನೆ dynhds *dynhds, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
  ಹಿಂತಿರುಗಿ Curl_dynhds_get(dynhds, name, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name));
}

CURLcode Curl_dynhds_ಕೂಡಿಸು(ರಚನೆ dynhds *dynhds,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, size_t namelen,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, size_t valuelen)
{
  ರಚನೆ dynhds_ನಮೂದು *ನಮೂದು = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* Accept NULL value with length 0, name must have length */
  ಆದರೆ(!dynhds || !name || !namelen || (!ಮೌಲ್ಯ && valuelen)) {
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }
  /* Does the new header fit into the limits? */
  ಆದರೆ((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_ಗಾತ್ರ - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_ಗಾತ್ರ - dynhds->strs_len - namelen < valuelen))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ನಮೂದು = ನಮೂದು_new(name, namelen, ಮೌಲ್ಯ, valuelen, dynhds->opts);
  ಆದರೆ(!ನಮೂದು)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    ರಚನೆ dynhds_ನಮೂದು **nhds;

    ಆದರೆ(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, ಗಾತ್ರ(ರಚನೆ dynhds_ನಮೂದು *));
    ಆದರೆ(!nhds)
      ಹೋಗು ಹೊರಹೋಗು;
    ಆದರೆ(dynhds->hds) {
      ಸ್ಮರಣೆ_ನಕಲು(nhds, dynhds->hds,
             dynhds->hds_len * ಗಾತ್ರ(ರಚನೆ dynhds_ನಮೂದು *));
      curlx_safefree(dynhds->hds);
    }
    dynhds->hds = nhds;
    dynhds->hds_allc = nallc;
  }
  dynhds->hds[dynhds->hds_len++] = ನಮೂದು;
  ನಮೂದು = ಖಾಲಿ;
  dynhds->strs_len += namelen + valuelen;
  ಫಲಿತಾಂಶ = CURLE_OK;

ಹೊರಹೋಗು:
  ಆದರೆ(ನಮೂದು)
    ನಮೂದು_free(ನಮೂದು);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_dynhds_cadd(ರಚನೆ dynhds *dynhds,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ)
{
  ಆದರೆ(!name)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಹಿಂತಿರುಗಿ Curl_dynhds_ಕೂಡಿಸು(dynhds, name, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name),
                         ಮೌಲ್ಯ, ಮೌಲ್ಯ ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮೌಲ್ಯ) : 0);
}

CURLcode Curl_dynhds_h1_ಕೂಡಿಸು_ಸಾಲು(ರಚನೆ dynhds *dynhds,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, size_t ಸಾಲು_len)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
  size_t namelen;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ;
  size_t valuelen, i;

  ಆದರೆ(!ಸಾಲು || !ಸಾಲು_len)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  p = memchr(ಸಾಲು, ':', ಸಾಲು_len);
  ಆದರೆ(!p || p == ಸಾಲು)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  name = ಸಾಲು;
  namelen = p - ಸಾಲು;
  p++; /* move past the colon */
  ಗಾಗಿ(i = namelen + 1; i < ಸಾಲು_len; ++i, ++p) {
    ಆದರೆ(!ISBLANK(*p))
      ನಿಲ್ಲಿಸು;
  }
  ಮೌಲ್ಯ = p;
  valuelen = ಸಾಲು_len - i;

  p = memchr(ಮೌಲ್ಯ, '\r', valuelen);
  ಆದರೆ(!p)
    p = memchr(ಮೌಲ್ಯ, '\n', valuelen);
  ಆದರೆ(p)
    valuelen = (size_t)(p - ಮೌಲ್ಯ);

  ಹಿಂತಿರುಗಿ Curl_dynhds_ಕೂಡಿಸು(dynhds, name, namelen, ಮೌಲ್ಯ, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_ಸಾಲು(ರಚನೆ dynhds *dynhds, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು)
{
  ಹಿಂತಿರುಗಿ Curl_dynhds_h1_ಕೂಡಿಸು_ಸಾಲು(dynhds, ಸಾಲು, ಸಾಲು ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಸಾಲು) : 0);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_ಒಳಗೊಂಡಿದೆ(ರಚನೆ dynhds *dynhds,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, size_t namelen);
UNITTEST bool dynhds_ಒಳಗೊಂಡಿದೆ(ರಚನೆ dynhds *dynhds,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, size_t namelen)
{
  ಹಿಂತಿರುಗಿ !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(ರಚನೆ dynhds *dynhds, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);
UNITTEST bool dynhds_ccontains(ರಚನೆ dynhds *dynhds, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
  ಹಿಂತಿರುಗಿ dynhds_ಒಳಗೊಂಡಿದೆ(dynhds, name, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ಎಣಿಕೆ_name(ರಚನೆ dynhds *dynhds,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, size_t namelen);
UNITTEST size_t dynhds_ಎಣಿಕೆ_name(ರಚನೆ dynhds *dynhds,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, size_t namelen)
{
  size_t n = 0;
  ಆದರೆ(dynhds->hds_len) {
    size_t i;
    ಗಾಗಿ(i = 0; i < dynhds->hds_len; ++i) {
      ಆದರೆ((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  ಹಿಂತಿರುಗಿ n;
}

/**
 * Return how often the given null-terminated name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ccount_name(ರಚನೆ dynhds *dynhds,
                                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);
UNITTEST size_t dynhds_ccount_name(ರಚನೆ dynhds *dynhds,
                                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
  ಹಿಂತಿರುಗಿ dynhds_ಎಣಿಕೆ_name(dynhds, name, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name));
}

/**
 * Remove all entries with the given name.
 * Returns number of entries removed.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_remove(ರಚನೆ dynhds *dynhds,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, size_t namelen);
UNITTEST size_t dynhds_remove(ರಚನೆ dynhds *dynhds,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, size_t namelen)
{
  size_t n = 0;
  ಆದರೆ(dynhds->hds_len) {
    size_t i, len;
    ಗಾಗಿ(i = 0; i < dynhds->hds_len; ++i) {
      ಆದರೆ((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        ನಮೂದು_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        ಆದರೆ(len) {
          memmove(&dynhds->hds[i], &dynhds->hds[i + 1],
                  len * ಗಾತ್ರ(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  ಹಿಂತಿರುಗಿ n;
}

/**
 * Set the give header name and value, replacing any entries with
 * the same name. The header is added at the end of all (remaining)
 * entries.
 *
 * @unittest 2602
 */
UNITTEST CURLcode dynhds_ಗಣ(ರಚನೆ dynhds *dynhds,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, size_t namelen,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, size_t valuelen);
UNITTEST CURLcode dynhds_ಗಣ(ರಚನೆ dynhds *dynhds,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, size_t namelen,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, size_t valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  ಹಿಂತಿರುಗಿ Curl_dynhds_ಕೂಡಿಸು(dynhds, name, namelen, ಮೌಲ್ಯ, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(ರಚನೆ dynhds *dynhds, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);
UNITTEST size_t dynhds_cremove(ರಚನೆ dynhds *dynhds, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
  ಹಿಂತಿರುಗಿ dynhds_remove(dynhds, name, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name));
}

#ಆದರೆ_ಅಂತ್ಯ /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(ರಚನೆ dynhds *dynhds, ರಚನೆ dynbuf *dbuf)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t i;

  ಆದರೆ(!dynhds->hds_len)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಗಾಗಿ(i = 0; i < dynhds->hds_len; ++i) {
    ಫಲಿತಾಂಶ = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(dbuf, STRCONST(": "));
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(dbuf, dynhds->hds[i]->ಮೌಲ್ಯ,
                              dynhds->hds[i]->valuelen);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NGHTTP2

nghttp2_nv *Curl_dynhds_ಗಮ್ಯ_nva(ರಚನೆ dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, ಗಾತ್ರ(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  ಆದರೆ(!nva)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  ಗಾಗಿ(i = 0; i < dynhds->hds_len; ++i) {
    ರಚನೆ dynhds_ನಮೂದು *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].ಮೌಲ್ಯ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)e->ಮೌಲ್ಯ;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  ಹಿಂತಿರುಗಿ nva;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_NGHTTP2 */
