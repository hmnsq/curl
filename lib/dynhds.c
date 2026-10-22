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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "dynhds.h"
#అన్తర్భావ "strcase.h"

స్థిర నిర్మాణం dynhds_నమోదు *నమోదు_new(స్థిరరాశి అక్షరం *name, size_t namelen,
                                      స్థిరరాశి అక్షరం *విలువ, size_t valuelen,
                                      పూర్ణాంకం opts)
{
  నిర్మాణం dynhds_నమోదు *e;
  అక్షరం *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  ఒకవేళ((SIZE_MAX - namelen - valuelen) < (పరిమాణం(*e) + 2))
    తిరిగి ఖాళీ;
  e = curlx_calloc(1, పరిమాణం(*e) + namelen + valuelen + 2);
  ఒకవేళ(!e)
    తిరిగి ఖాళీ;
  e->name = p = (అక్షరం *)e + పరిమాణం(*e);
  జ్ఞాపకం_నకలు(p, name, namelen);
  e->namelen = namelen;
  e->విలువ = p += namelen + 1; /* leave a \0 at the end of name */
  ఒకవేళ(valuelen) {
    జ్ఞాపకం_నకలు(p, విలువ, valuelen);
    e->valuelen = valuelen;
  }
  ఒకవేళ(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  తిరిగి e;
}

స్థిర శూన్య నమోదు_free(నిర్మాణం dynhds_నమోదు *e)
{
  curlx_free(e);
}

శూన్య Curl_dynhds_init(నిర్మాణం dynhds *dynhds, size_t max_entries,
                      size_t max_strs_కొలత)
{
  DEBUGASSERT(dynhds);
  DEBUGASSERT(max_strs_కొలత);
  dynhds->hds = ఖాళీ;
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
  dynhds->max_entries = max_entries;
  dynhds->max_strs_కొలత = max_strs_కొలత;
  dynhds->opts = 0;
}

శూన్య Curl_dynhds_free(నిర్మాణం dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  ఒకవేళ(dynhds->hds && dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    కొరకు(i = 0; i < dynhds->hds_len; ++i) {
      నమోదు_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

శూన్య Curl_dynhds_reset(నిర్మాణం dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  ఒకవేళ(dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    కొరకు(i = 0; i < dynhds->hds_len; ++i) {
      నమోదు_free(dynhds->hds[i]);
      dynhds->hds[i] = ఖాళీ;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_లెక్క(నిర్మాణం dynhds *dynhds)
{
  తిరిగి dynhds->hds_len;
}

శూన్య Curl_dynhds_సమితి_opts(నిర్మాణం dynhds *dynhds, పూర్ణాంకం opts)
{
  dynhds->opts = opts;
}

నిర్మాణం dynhds_నమోదు *Curl_dynhds_getn(నిర్మాణం dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  తిరిగి (n < dynhds->hds_len) ? dynhds->hds[n] : ఖాళీ;
}

నిర్మాణం dynhds_నమోదు *Curl_dynhds_get(నిర్మాణం dynhds *dynhds, స్థిరరాశి అక్షరం *name,
                                     size_t namelen)
{
  size_t i;
  కొరకు(i = 0; i < dynhds->hds_len; ++i) {
    ఒకవేళ(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      తిరిగి dynhds->hds[i];
    }
  }
  తిరిగి ఖాళీ;
}

నిర్మాణం dynhds_నమోదు *Curl_dynhds_cget(నిర్మాణం dynhds *dynhds, స్థిరరాశి అక్షరం *name)
{
  తిరిగి Curl_dynhds_get(dynhds, name, అక్షరశ్రేణి_పొడవు(name));
}

CURLcode Curl_dynhds_కూడు(నిర్మాణం dynhds *dynhds,
                         స్థిరరాశి అక్షరం *name, size_t namelen,
                         స్థిరరాశి అక్షరం *విలువ, size_t valuelen)
{
  నిర్మాణం dynhds_నమోదు *నమోదు_2 = ఖాళీ;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* Accept NULL value with length 0, name must have length */
  ఒకవేళ(!dynhds || !name || !namelen || (!విలువ && valuelen)) {
    DEBUGASSERT(0);
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }
  /* Does the new header fit into the limits? */
  ఒకవేళ((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_కొలత - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_కొలత - dynhds->strs_len - namelen < valuelen))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  నమోదు_2 = నమోదు_new(name, namelen, విలువ, valuelen, dynhds->opts);
  ఒకవేళ(!నమోదు_2)
    వెళ్ళు బహిర్గతం;

  ఒకవేళ(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    నిర్మాణం dynhds_నమోదు **nhds;

    ఒకవేళ(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, పరిమాణం(నిర్మాణం dynhds_నమోదు *));
    ఒకవేళ(!nhds)
      వెళ్ళు బహిర్గతం;
    ఒకవేళ(dynhds->hds) {
      జ్ఞాపకం_నకలు(nhds, dynhds->hds,
             dynhds->hds_len * పరిమాణం(నిర్మాణం dynhds_నమోదు *));
      curlx_safefree(dynhds->hds);
    }
    dynhds->hds = nhds;
    dynhds->hds_allc = nallc;
  }
  dynhds->hds[dynhds->hds_len++] = నమోదు_2;
  నమోదు_2 = ఖాళీ;
  dynhds->strs_len += namelen + valuelen;
  ఫలితం = CURLE_OK;

బహిర్గతం:
  ఒకవేళ(నమోదు_2)
    నమోదు_free(నమోదు_2);
  తిరిగి ఫలితం;
}

CURLcode Curl_dynhds_cadd(నిర్మాణం dynhds *dynhds,
                          స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *విలువ)
{
  ఒకవేళ(!name)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  తిరిగి Curl_dynhds_కూడు(dynhds, name, అక్షరశ్రేణి_పొడవు(name),
                         విలువ, విలువ ? అక్షరశ్రేణి_పొడవు(విలువ) : 0);
}

CURLcode Curl_dynhds_h1_కూడు_పంక్తి(నిర్మాణం dynhds *dynhds,
                                 స్థిరరాశి అక్షరం *పంక్తి, size_t పంక్తి_len)
{
  స్థిరరాశి అక్షరం *p;
  స్థిరరాశి అక్షరం *name;
  size_t namelen;
  స్థిరరాశి అక్షరం *విలువ;
  size_t valuelen, i;

  ఒకవేళ(!పంక్తి || !పంక్తి_len)
    తిరిగి CURLE_OK;

  p = memchr(పంక్తి, ':', పంక్తి_len);
  ఒకవేళ(!p || p == పంక్తి)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  name = పంక్తి;
  namelen = p - పంక్తి;
  p++; /* move past the colon */
  కొరకు(i = namelen + 1; i < పంక్తి_len; ++i, ++p) {
    ఒకవేళ(!ISBLANK(*p))
      ఆపు;
  }
  విలువ = p;
  valuelen = పంక్తి_len - i;

  p = memchr(విలువ, '\r', valuelen);
  ఒకవేళ(!p)
    p = memchr(విలువ, '\n', valuelen);
  ఒకవేళ(p)
    valuelen = (size_t)(p - విలువ);

  తిరిగి Curl_dynhds_కూడు(dynhds, name, namelen, విలువ, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_పంక్తి(నిర్మాణం dynhds *dynhds, స్థిరరాశి అక్షరం *పంక్తి)
{
  తిరిగి Curl_dynhds_h1_కూడు_పంక్తి(dynhds, పంక్తి, పంక్తి ? అక్షరశ్రేణి_పొడవు(పంక్తి) : 0);
}

#నిర్వచితమైతే UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_కలిగియుంది(నిర్మాణం dynhds *dynhds,
                              స్థిరరాశి అక్షరం *name, size_t namelen);
UNITTEST bool dynhds_కలిగియుంది(నిర్మాణం dynhds *dynhds,
                              స్థిరరాశి అక్షరం *name, size_t namelen)
{
  తిరిగి !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(నిర్మాణం dynhds *dynhds, స్థిరరాశి అక్షరం *name);
UNITTEST bool dynhds_ccontains(నిర్మాణం dynhds *dynhds, స్థిరరాశి అక్షరం *name)
{
  తిరిగి dynhds_కలిగియుంది(dynhds, name, అక్షరశ్రేణి_పొడవు(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_లెక్క_name(నిర్మాణం dynhds *dynhds,
                                  స్థిరరాశి అక్షరం *name, size_t namelen);
UNITTEST size_t dynhds_లెక్క_name(నిర్మాణం dynhds *dynhds,
                                  స్థిరరాశి అక్షరం *name, size_t namelen)
{
  size_t n = 0;
  ఒకవేళ(dynhds->hds_len) {
    size_t i;
    కొరకు(i = 0; i < dynhds->hds_len; ++i) {
      ఒకవేళ((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  తిరిగి n;
}

/**
 * Return how often the given null-terminated name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ccount_name(నిర్మాణం dynhds *dynhds,
                                        స్థిరరాశి అక్షరం *name);
UNITTEST size_t dynhds_ccount_name(నిర్మాణం dynhds *dynhds,
                                        స్థిరరాశి అక్షరం *name)
{
  తిరిగి dynhds_లెక్క_name(dynhds, name, అక్షరశ్రేణి_పొడవు(name));
}

/**
 * Remove all entries with the given name.
 * Returns number of entries removed.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_remove(నిర్మాణం dynhds *dynhds,
                              స్థిరరాశి అక్షరం *name, size_t namelen);
UNITTEST size_t dynhds_remove(నిర్మాణం dynhds *dynhds,
                              స్థిరరాశి అక్షరం *name, size_t namelen)
{
  size_t n = 0;
  ఒకవేళ(dynhds->hds_len) {
    size_t i, len;
    కొరకు(i = 0; i < dynhds->hds_len; ++i) {
      ఒకవేళ((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        నమోదు_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        ఒకవేళ(len) {
          memmove(&dynhds->hds[i], &dynhds->hds[i + 1],
                  len * పరిమాణం(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  తిరిగి n;
}

/**
 * Set the give header name and value, replacing any entries with
 * the same name. The header is added at the end of all (remaining)
 * entries.
 *
 * @unittest 2602
 */
UNITTEST CURLcode dynhds_సమితి(నిర్మాణం dynhds *dynhds,
                             స్థిరరాశి అక్షరం *name, size_t namelen,
                             స్థిరరాశి అక్షరం *విలువ, size_t valuelen);
UNITTEST CURLcode dynhds_సమితి(నిర్మాణం dynhds *dynhds,
                             స్థిరరాశి అక్షరం *name, size_t namelen,
                             స్థిరరాశి అక్షరం *విలువ, size_t valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  తిరిగి Curl_dynhds_కూడు(dynhds, name, namelen, విలువ, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(నిర్మాణం dynhds *dynhds, స్థిరరాశి అక్షరం *name);
UNITTEST size_t dynhds_cremove(నిర్మాణం dynhds *dynhds, స్థిరరాశి అక్షరం *name)
{
  తిరిగి dynhds_remove(dynhds, name, అక్షరశ్రేణి_పొడవు(name));
}

#అయితే_ముగింపు /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(నిర్మాణం dynhds *dynhds, నిర్మాణం dynbuf *dbuf)
{
  CURLcode ఫలితం = CURLE_OK;
  size_t i;

  ఒకవేళ(!dynhds->hds_len)
    తిరిగి ఫలితం;

  కొరకు(i = 0; i < dynhds->hds_len; ++i) {
    ఫలితం = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_addn(dbuf, STRCONST(": "));
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_addn(dbuf, dynhds->hds[i]->విలువ,
                              dynhds->hds[i]->valuelen);
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    ఒకవేళ(ఫలితం)
      ఆపు;
  }

  తిరిగి ఫలితం;
}

#నిర్వచితమైతే USE_NGHTTP2

nghttp2_nv *Curl_dynhds_గమ్యం_nva(నిర్మాణం dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, పరిమాణం(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  ఒకవేళ(!nva)
    తిరిగి ఖాళీ;

  కొరకు(i = 0; i < dynhds->hds_len; ++i) {
    నిర్మాణం dynhds_నమోదు *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (గుర్తులేని అక్షరం *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].విలువ = (గుర్తులేని అక్షరం *)e->విలువ;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  తిరిగి nva;
}

#అయితే_ముగింపు /* USE_NGHTTP2 */
