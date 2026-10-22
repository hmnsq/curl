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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "dynhds.h"
#अन्तर्भाव "strcase.h"

स्थिर संरचना dynhds_entry *entry_new(ध्रुव वर्ण *name, size_t namelen,
                                      ध्रुव वर्ण *मानम्, size_t valuelen,
                                      पूर्णाङ्क opts)
{
  संरचना dynhds_entry *e;
  वर्ण *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  सचे((SIZE_MAX - namelen - valuelen) < (परिमाण(*e) + 2))
    पटिनिवत्त रिक्त;
  e = curlx_calloc(1, परिमाण(*e) + namelen + valuelen + 2);
  सचे(!e)
    पटिनिवत्त रिक्त;
  e->name = p = (वर्ण *)e + परिमाण(*e);
  memcpy(p, name, namelen);
  e->namelen = namelen;
  e->मानम् = p += namelen + 1; /* leave a \0 at the end of name */
  सचे(valuelen) {
    memcpy(p, मानम्, valuelen);
    e->valuelen = valuelen;
  }
  सचे(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  पटिनिवत्त e;
}

स्थिर शून्य entry_free(संरचना dynhds_entry *e)
{
  curlx_free(e);
}

शून्य Curl_dynhds_init(संरचना dynhds *dynhds, size_t max_entries,
                      size_t max_strs_size)
{
  DEBUGASSERT(dynhds);
  DEBUGASSERT(max_strs_size);
  dynhds->hds = रिक्त;
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
  dynhds->max_entries = max_entries;
  dynhds->max_strs_size = max_strs_size;
  dynhds->opts = 0;
}

शून्य Curl_dynhds_free(संरचना dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  सचे(dynhds->hds && dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    अत्थाय(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

शून्य Curl_dynhds_reset(संरचना dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  सचे(dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    अत्थाय(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
      dynhds->hds[i] = रिक्त;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_गणनम्(संरचना dynhds *dynhds)
{
  पटिनिवत्त dynhds->hds_len;
}

शून्य Curl_dynhds_set_opts(संरचना dynhds *dynhds, पूर्णाङ्क opts)
{
  dynhds->opts = opts;
}

संरचना dynhds_entry *Curl_dynhds_getn(संरचना dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  पटिनिवत्त (n < dynhds->hds_len) ? dynhds->hds[n] : रिक्त;
}

संरचना dynhds_entry *Curl_dynhds_get(संरचना dynhds *dynhds, ध्रुव वर्ण *name,
                                     size_t namelen)
{
  size_t i;
  अत्थाय(i = 0; i < dynhds->hds_len; ++i) {
    सचे(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      पटिनिवत्त dynhds->hds[i];
    }
  }
  पटिनिवत्त रिक्त;
}

संरचना dynhds_entry *Curl_dynhds_cget(संरचना dynhds *dynhds, ध्रुव वर्ण *name)
{
  पटिनिवत्त Curl_dynhds_get(dynhds, name, strlen(name));
}

CURLcode Curl_dynhds_add(संरचना dynhds *dynhds,
                         ध्रुव वर्ण *name, size_t namelen,
                         ध्रुव वर्ण *मानम्, size_t valuelen)
{
  संरचना dynhds_entry *entry = रिक्त;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* Accept NULL value with length 0, name must have length */
  सचे(!dynhds || !name || !namelen || (!मानम् && valuelen)) {
    DEBUGASSERT(0);
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
  }
  /* Does the new header fit into the limits? */
  सचे((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_size - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_size - dynhds->strs_len - namelen < valuelen))
    पटिनिवत्त CURLE_OUT_OF_MEMORY;

  entry = entry_new(name, namelen, मानम्, valuelen, dynhds->opts);
  सचे(!entry)
    गच्छ out;

  सचे(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    संरचना dynhds_entry **nhds;

    सचे(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, परिमाण(संरचना dynhds_entry *));
    सचे(!nhds)
      गच्छ out;
    सचे(dynhds->hds) {
      memcpy(nhds, dynhds->hds,
             dynhds->hds_len * परिमाण(संरचना dynhds_entry *));
      curlx_safefree(dynhds->hds);
    }
    dynhds->hds = nhds;
    dynhds->hds_allc = nallc;
  }
  dynhds->hds[dynhds->hds_len++] = entry;
  entry = रिक्त;
  dynhds->strs_len += namelen + valuelen;
  result = CURLE_OK;

out:
  सचे(entry)
    entry_free(entry);
  पटिनिवत्त result;
}

CURLcode Curl_dynhds_cadd(संरचना dynhds *dynhds,
                          ध्रुव वर्ण *name, ध्रुव वर्ण *मानम्)
{
  सचे(!name)
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
  पटिनिवत्त Curl_dynhds_add(dynhds, name, strlen(name),
                         मानम्, मानम् ? strlen(मानम्) : 0);
}

CURLcode Curl_dynhds_h1_add_पङ्क्तिः(संरचना dynhds *dynhds,
                                 ध्रुव वर्ण *पङ्क्तिः, size_t पङ्क्तिः_len)
{
  ध्रुव वर्ण *p;
  ध्रुव वर्ण *name;
  size_t namelen;
  ध्रुव वर्ण *मानम्;
  size_t valuelen, i;

  सचे(!पङ्क्तिः || !पङ्क्तिः_len)
    पटिनिवत्त CURLE_OK;

  p = memchr(पङ्क्तिः, ':', पङ्क्तिः_len);
  सचे(!p || p == पङ्क्तिः)
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
  name = पङ्क्तिः;
  namelen = p - पङ्क्तिः;
  p++; /* move past the colon */
  अत्थाय(i = namelen + 1; i < पङ्क्तिः_len; ++i, ++p) {
    सचे(!ISBLANK(*p))
      भङ्ग;
  }
  मानम् = p;
  valuelen = पङ्क्तिः_len - i;

  p = memchr(मानम्, '\r', valuelen);
  सचे(!p)
    p = memchr(मानम्, '\n', valuelen);
  सचे(p)
    valuelen = (size_t)(p - मानम्);

  पटिनिवत्त Curl_dynhds_add(dynhds, name, namelen, मानम्, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_पङ्क्तिः(संरचना dynhds *dynhds, ध्रुव वर्ण *पङ्क्तिः)
{
  पटिनिवत्त Curl_dynhds_h1_add_पङ्क्तिः(dynhds, पङ्क्तिः, पङ्क्तिः ? strlen(पङ्क्तिः) : 0);
}

#ifdef UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_contains(संरचना dynhds *dynhds,
                              ध्रुव वर्ण *name, size_t namelen);
UNITTEST bool dynhds_contains(संरचना dynhds *dynhds,
                              ध्रुव वर्ण *name, size_t namelen)
{
  पटिनिवत्त !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(संरचना dynhds *dynhds, ध्रुव वर्ण *name);
UNITTEST bool dynhds_ccontains(संरचना dynhds *dynhds, ध्रुव वर्ण *name)
{
  पटिनिवत्त dynhds_contains(dynhds, name, strlen(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_गणनम्_name(संरचना dynhds *dynhds,
                                  ध्रुव वर्ण *name, size_t namelen);
UNITTEST size_t dynhds_गणनम्_name(संरचना dynhds *dynhds,
                                  ध्रुव वर्ण *name, size_t namelen)
{
  size_t n = 0;
  सचे(dynhds->hds_len) {
    size_t i;
    अत्थाय(i = 0; i < dynhds->hds_len; ++i) {
      सचे((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  पटिनिवत्त n;
}

/**
 * Return how often the given null-terminated name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ccount_name(संरचना dynhds *dynhds,
                                        ध्रुव वर्ण *name);
UNITTEST size_t dynhds_ccount_name(संरचना dynhds *dynhds,
                                        ध्रुव वर्ण *name)
{
  पटिनिवत्त dynhds_गणनम्_name(dynhds, name, strlen(name));
}

/**
 * Remove all entries with the given name.
 * Returns number of entries removed.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_remove(संरचना dynhds *dynhds,
                              ध्रुव वर्ण *name, size_t namelen);
UNITTEST size_t dynhds_remove(संरचना dynhds *dynhds,
                              ध्रुव वर्ण *name, size_t namelen)
{
  size_t n = 0;
  सचे(dynhds->hds_len) {
    size_t i, len;
    अत्थाय(i = 0; i < dynhds->hds_len; ++i) {
      सचे((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        entry_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        सचे(len) {
          memmove(&dynhds->hds[i], &dynhds->hds[i + 1],
                  len * परिमाण(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  पटिनिवत्त n;
}

/**
 * Set the give header name and value, replacing any entries with
 * the same name. The header is added at the end of all (remaining)
 * entries.
 *
 * @unittest 2602
 */
UNITTEST CURLcode dynhds_set(संरचना dynhds *dynhds,
                             ध्रुव वर्ण *name, size_t namelen,
                             ध्रुव वर्ण *मानम्, size_t valuelen);
UNITTEST CURLcode dynhds_set(संरचना dynhds *dynhds,
                             ध्रुव वर्ण *name, size_t namelen,
                             ध्रुव वर्ण *मानम्, size_t valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  पटिनिवत्त Curl_dynhds_add(dynhds, name, namelen, मानम्, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(संरचना dynhds *dynhds, ध्रुव वर्ण *name);
UNITTEST size_t dynhds_cremove(संरचना dynhds *dynhds, ध्रुव वर्ण *name)
{
  पटिनिवत्त dynhds_remove(dynhds, name, strlen(name));
}

#endif /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(संरचना dynhds *dynhds, संरचना dynbuf *dbuf)
{
  CURLcode result = CURLE_OK;
  size_t i;

  सचे(!dynhds->hds_len)
    पटिनिवत्त result;

  अत्थाय(i = 0; i < dynhds->hds_len; ++i) {
    result = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    सचे(!result)
      result = curlx_dyn_addn(dbuf, STRCONST(": "));
    सचे(!result)
      result = curlx_dyn_addn(dbuf, dynhds->hds[i]->मानम्,
                              dynhds->hds[i]->valuelen);
    सचे(!result)
      result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    सचे(result)
      भङ्ग;
  }

  पटिनिवत्त result;
}

#ifdef USE_NGHTTP2

nghttp2_nv *Curl_dynhds_to_nva(संरचना dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, परिमाण(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  सचे(!nva)
    पटिनिवत्त रिक्त;

  अत्थाय(i = 0; i < dynhds->hds_len; ++i) {
    संरचना dynhds_entry *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (unsigned वर्ण *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].मानम् = (unsigned वर्ण *)e->मानम्;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  पटिनिवत्त nva;
}

#endif /* USE_NGHTTP2 */
