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

स्थिर संरचना dynhds_entry *entry_new(स्थिरांक अक्षर *name, माप_प्रकार namelen,
                                      स्थिरांक अक्षर *मूल्य, माप_प्रकार valuelen,
                                      पूर्णांक opts)
{
  संरचना dynhds_entry *e;
  अक्षर *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  जर((SIZE_MAX - namelen - valuelen) < (आकार(*e) + 2))
    परत रिक्त;
  e = curlx_calloc(1, आकार(*e) + namelen + valuelen + 2);
  जर(!e)
    परत रिक्त;
  e->name = p = (अक्षर *)e + आकार(*e);
  स_नकल(p, name, namelen);
  e->namelen = namelen;
  e->मूल्य = p += namelen + 1; /* leave a \0 at the end of name */
  जर(valuelen) {
    स_नकल(p, मूल्य, valuelen);
    e->valuelen = valuelen;
  }
  जर(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  परत e;
}

स्थिर शून्य entry_free(संरचना dynhds_entry *e)
{
  curlx_free(e);
}

शून्य Curl_dynhds_init(संरचना dynhds *dynhds, माप_प्रकार max_entries,
                      माप_प्रकार max_strs_size)
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
  जर(dynhds->hds && dynhds->hds_len) {
    माप_प्रकार i;
    DEBUGASSERT(dynhds->hds);
    साठी(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

शून्य Curl_dynhds_reset(संरचना dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  जर(dynhds->hds_len) {
    माप_प्रकार i;
    DEBUGASSERT(dynhds->hds);
    साठी(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
      dynhds->hds[i] = रिक्त;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

माप_प्रकार Curl_dynhds_मोजणी(संरचना dynhds *dynhds)
{
  परत dynhds->hds_len;
}

शून्य Curl_dynhds_set_opts(संरचना dynhds *dynhds, पूर्णांक opts)
{
  dynhds->opts = opts;
}

संरचना dynhds_entry *Curl_dynhds_getn(संरचना dynhds *dynhds, माप_प्रकार n)
{
  DEBUGASSERT(dynhds);
  परत (n < dynhds->hds_len) ? dynhds->hds[n] : रिक्त;
}

संरचना dynhds_entry *Curl_dynhds_get(संरचना dynhds *dynhds, स्थिरांक अक्षर *name,
                                     माप_प्रकार namelen)
{
  माप_प्रकार i;
  साठी(i = 0; i < dynhds->hds_len; ++i) {
    जर(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      परत dynhds->hds[i];
    }
  }
  परत रिक्त;
}

संरचना dynhds_entry *Curl_dynhds_cget(संरचना dynhds *dynhds, स्थिरांक अक्षर *name)
{
  परत Curl_dynhds_get(dynhds, name, म_माप(name));
}

CURLcode Curl_dynhds_add(संरचना dynhds *dynhds,
                         स्थिरांक अक्षर *name, माप_प्रकार namelen,
                         स्थिरांक अक्षर *मूल्य, माप_प्रकार valuelen)
{
  संरचना dynhds_entry *entry = रिक्त;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* Accept NULL value with length 0, name must have length */
  जर(!dynhds || !name || !namelen || (!मूल्य && valuelen)) {
    DEBUGASSERT(0);
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  }
  /* Does the new header fit into the limits? */
  जर((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_size - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_size - dynhds->strs_len - namelen < valuelen))
    परत CURLE_OUT_OF_MEMORY;

  entry = entry_new(name, namelen, मूल्य, valuelen, dynhds->opts);
  जर(!entry)
    जा out;

  जर(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    माप_प्रकार nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    संरचना dynhds_entry **nhds;

    जर(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, आकार(संरचना dynhds_entry *));
    जर(!nhds)
      जा out;
    जर(dynhds->hds) {
      स_नकल(nhds, dynhds->hds,
             dynhds->hds_len * आकार(संरचना dynhds_entry *));
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
  जर(entry)
    entry_free(entry);
  परत result;
}

CURLcode Curl_dynhds_cadd(संरचना dynhds *dynhds,
                          स्थिरांक अक्षर *name, स्थिरांक अक्षर *मूल्य)
{
  जर(!name)
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  परत Curl_dynhds_add(dynhds, name, म_माप(name),
                         मूल्य, मूल्य ? म_माप(मूल्य) : 0);
}

CURLcode Curl_dynhds_h1_add_ओळ(संरचना dynhds *dynhds,
                                 स्थिरांक अक्षर *ओळ, माप_प्रकार ओळ_len)
{
  स्थिरांक अक्षर *p;
  स्थिरांक अक्षर *name;
  माप_प्रकार namelen;
  स्थिरांक अक्षर *मूल्य;
  माप_प्रकार valuelen, i;

  जर(!ओळ || !ओळ_len)
    परत CURLE_OK;

  p = स_प्रथम(ओळ, ':', ओळ_len);
  जर(!p || p == ओळ)
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  name = ओळ;
  namelen = p - ओळ;
  p++; /* move past the colon */
  साठी(i = namelen + 1; i < ओळ_len; ++i, ++p) {
    जर(!ISBLANK(*p))
      थांबा;
  }
  मूल्य = p;
  valuelen = ओळ_len - i;

  p = स_प्रथम(मूल्य, '\r', valuelen);
  जर(!p)
    p = स_प्रथम(मूल्य, '\n', valuelen);
  जर(p)
    valuelen = (माप_प्रकार)(p - मूल्य);

  परत Curl_dynhds_add(dynhds, name, namelen, मूल्य, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_ओळ(संरचना dynhds *dynhds, स्थिरांक अक्षर *ओळ)
{
  परत Curl_dynhds_h1_add_ओळ(dynhds, ओळ, ओळ ? म_माप(ओळ) : 0);
}

#ifdef UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_contains(संरचना dynhds *dynhds,
                              स्थिरांक अक्षर *name, माप_प्रकार namelen);
UNITTEST bool dynhds_contains(संरचना dynhds *dynhds,
                              स्थिरांक अक्षर *name, माप_प्रकार namelen)
{
  परत !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(संरचना dynhds *dynhds, स्थिरांक अक्षर *name);
UNITTEST bool dynhds_ccontains(संरचना dynhds *dynhds, स्थिरांक अक्षर *name)
{
  परत dynhds_contains(dynhds, name, म_माप(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST माप_प्रकार dynhds_मोजणी_name(संरचना dynhds *dynhds,
                                  स्थिरांक अक्षर *name, माप_प्रकार namelen);
UNITTEST माप_प्रकार dynhds_मोजणी_name(संरचना dynhds *dynhds,
                                  स्थिरांक अक्षर *name, माप_प्रकार namelen)
{
  माप_प्रकार n = 0;
  जर(dynhds->hds_len) {
    माप_प्रकार i;
    साठी(i = 0; i < dynhds->hds_len; ++i) {
      जर((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  परत n;
}

/**
 * Return how often the given null-terminated name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST माप_प्रकार dynhds_ccount_name(संरचना dynhds *dynhds,
                                        स्थिरांक अक्षर *name);
UNITTEST माप_प्रकार dynhds_ccount_name(संरचना dynhds *dynhds,
                                        स्थिरांक अक्षर *name)
{
  परत dynhds_मोजणी_name(dynhds, name, म_माप(name));
}

/**
 * Remove all entries with the given name.
 * Returns number of entries removed.
 *
 * @unittest 2602
 */
UNITTEST माप_प्रकार dynhds_remove(संरचना dynhds *dynhds,
                              स्थिरांक अक्षर *name, माप_प्रकार namelen);
UNITTEST माप_प्रकार dynhds_remove(संरचना dynhds *dynhds,
                              स्थिरांक अक्षर *name, माप_प्रकार namelen)
{
  माप_प्रकार n = 0;
  जर(dynhds->hds_len) {
    माप_प्रकार i, len;
    साठी(i = 0; i < dynhds->hds_len; ++i) {
      जर((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        entry_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        जर(len) {
          स_हटाओ(&dynhds->hds[i], &dynhds->hds[i + 1],
                  len * आकार(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  परत n;
}

/**
 * Set the give header name and value, replacing any entries with
 * the same name. The header is added at the end of all (remaining)
 * entries.
 *
 * @unittest 2602
 */
UNITTEST CURLcode dynhds_set(संरचना dynhds *dynhds,
                             स्थिरांक अक्षर *name, माप_प्रकार namelen,
                             स्थिरांक अक्षर *मूल्य, माप_प्रकार valuelen);
UNITTEST CURLcode dynhds_set(संरचना dynhds *dynhds,
                             स्थिरांक अक्षर *name, माप_प्रकार namelen,
                             स्थिरांक अक्षर *मूल्य, माप_प्रकार valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  परत Curl_dynhds_add(dynhds, name, namelen, मूल्य, valuelen);
}

/* @unittest 2602 */
UNITTEST माप_प्रकार dynhds_cremove(संरचना dynhds *dynhds, स्थिरांक अक्षर *name);
UNITTEST माप_प्रकार dynhds_cremove(संरचना dynhds *dynhds, स्थिरांक अक्षर *name)
{
  परत dynhds_remove(dynhds, name, म_माप(name));
}

#endif /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(संरचना dynhds *dynhds, संरचना dynbuf *dbuf)
{
  CURLcode result = CURLE_OK;
  माप_प्रकार i;

  जर(!dynhds->hds_len)
    परत result;

  साठी(i = 0; i < dynhds->hds_len; ++i) {
    result = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    जर(!result)
      result = curlx_dyn_addn(dbuf, STRCONST(": "));
    जर(!result)
      result = curlx_dyn_addn(dbuf, dynhds->hds[i]->मूल्य,
                              dynhds->hds[i]->valuelen);
    जर(!result)
      result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    जर(result)
      थांबा;
  }

  परत result;
}

#ifdef USE_NGHTTP2

nghttp2_nv *Curl_dynhds_to_nva(संरचना dynhds *dynhds, माप_प्रकार *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, आकार(nghttp2_nv) * dynhds->hds_len);
  माप_प्रकार i;

  *pcount = 0;
  जर(!nva)
    परत रिक्त;

  साठी(i = 0; i < dynhds->hds_len; ++i) {
    संरचना dynhds_entry *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (अचिन्हित अक्षर *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].मूल्य = (अचिन्हित अक्षर *)e->मूल्य;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  परत nva;
}

#endif /* USE_NGHTTP2 */
