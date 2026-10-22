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

स्थिर संरचना dynhds_entry *entry_new(अचर वर्ण *name, माप_प्रकार namelen,
                                      अचर वर्ण *मान, माप_प्रकार valuelen,
                                      पूर्णाङ्क opts)
{
  संरचना dynhds_entry *e;
  वर्ण *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  यदि((SIZE_MAX - namelen - valuelen) < (परिमाण(*e) + 2))
    फर्काऊ रिक्त;
  e = curlx_calloc(1, परिमाण(*e) + namelen + valuelen + 2);
  यदि(!e)
    फर्काऊ रिक्त;
  e->name = p = (वर्ण *)e + परिमाण(*e);
  स_नकल(p, name, namelen);
  e->namelen = namelen;
  e->मान = p += namelen + 1; /* leave a \0 at the end of name */
  यदि(valuelen) {
    स_नकल(p, मान, valuelen);
    e->valuelen = valuelen;
  }
  यदि(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  फर्काऊ e;
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
  यदि(dynhds->hds && dynhds->hds_len) {
    माप_प्रकार i;
    DEBUGASSERT(dynhds->hds);
    लागि(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

शून्य Curl_dynhds_reset(संरचना dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  यदि(dynhds->hds_len) {
    माप_प्रकार i;
    DEBUGASSERT(dynhds->hds);
    लागि(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
      dynhds->hds[i] = रिक्त;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

माप_प्रकार Curl_dynhds_गन्ती(संरचना dynhds *dynhds)
{
  फर्काऊ dynhds->hds_len;
}

शून्य Curl_dynhds_set_opts(संरचना dynhds *dynhds, पूर्णाङ्क opts)
{
  dynhds->opts = opts;
}

संरचना dynhds_entry *Curl_dynhds_getn(संरचना dynhds *dynhds, माप_प्रकार n)
{
  DEBUGASSERT(dynhds);
  फर्काऊ (n < dynhds->hds_len) ? dynhds->hds[n] : रिक्त;
}

संरचना dynhds_entry *Curl_dynhds_get(संरचना dynhds *dynhds, अचर वर्ण *name,
                                     माप_प्रकार namelen)
{
  माप_प्रकार i;
  लागि(i = 0; i < dynhds->hds_len; ++i) {
    यदि(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      फर्काऊ dynhds->hds[i];
    }
  }
  फर्काऊ रिक्त;
}

संरचना dynhds_entry *Curl_dynhds_cget(संरचना dynhds *dynhds, अचर वर्ण *name)
{
  फर्काऊ Curl_dynhds_get(dynhds, name, म_माप(name));
}

CURLcode Curl_dynhds_add(संरचना dynhds *dynhds,
                         अचर वर्ण *name, माप_प्रकार namelen,
                         अचर वर्ण *मान, माप_प्रकार valuelen)
{
  संरचना dynhds_entry *entry = रिक्त;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* Accept NULL value with length 0, name must have length */
  यदि(!dynhds || !name || !namelen || (!मान && valuelen)) {
    DEBUGASSERT(0);
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  /* Does the new header fit into the limits? */
  यदि((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_size - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_size - dynhds->strs_len - namelen < valuelen))
    फर्काऊ CURLE_OUT_OF_MEMORY;

  entry = entry_new(name, namelen, मान, valuelen, dynhds->opts);
  यदि(!entry)
    गच्छ out;

  यदि(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    माप_प्रकार nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    संरचना dynhds_entry **nhds;

    यदि(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, परिमाण(संरचना dynhds_entry *));
    यदि(!nhds)
      गच्छ out;
    यदि(dynhds->hds) {
      स_नकल(nhds, dynhds->hds,
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
  यदि(entry)
    entry_free(entry);
  फर्काऊ result;
}

CURLcode Curl_dynhds_cadd(संरचना dynhds *dynhds,
                          अचर वर्ण *name, अचर वर्ण *मान)
{
  यदि(!name)
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  फर्काऊ Curl_dynhds_add(dynhds, name, म_माप(name),
                         मान, मान ? म_माप(मान) : 0);
}

CURLcode Curl_dynhds_h1_add_पङ्क्ति(संरचना dynhds *dynhds,
                                 अचर वर्ण *पङ्क्ति, माप_प्रकार पङ्क्ति_len)
{
  अचर वर्ण *p;
  अचर वर्ण *name;
  माप_प्रकार namelen;
  अचर वर्ण *मान;
  माप_प्रकार valuelen, i;

  यदि(!पङ्क्ति || !पङ्क्ति_len)
    फर्काऊ CURLE_OK;

  p = स_प्रथम(पङ्क्ति, ':', पङ्क्ति_len);
  यदि(!p || p == पङ्क्ति)
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  name = पङ्क्ति;
  namelen = p - पङ्क्ति;
  p++; /* move past the colon */
  लागि(i = namelen + 1; i < पङ्क्ति_len; ++i, ++p) {
    यदि(!ISBLANK(*p))
      रोक;
  }
  मान = p;
  valuelen = पङ्क्ति_len - i;

  p = स_प्रथम(मान, '\r', valuelen);
  यदि(!p)
    p = स_प्रथम(मान, '\n', valuelen);
  यदि(p)
    valuelen = (माप_प्रकार)(p - मान);

  फर्काऊ Curl_dynhds_add(dynhds, name, namelen, मान, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_पङ्क्ति(संरचना dynhds *dynhds, अचर वर्ण *पङ्क्ति)
{
  फर्काऊ Curl_dynhds_h1_add_पङ्क्ति(dynhds, पङ्क्ति, पङ्क्ति ? म_माप(पङ्क्ति) : 0);
}

#ifdef UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_contains(संरचना dynhds *dynhds,
                              अचर वर्ण *name, माप_प्रकार namelen);
UNITTEST bool dynhds_contains(संरचना dynhds *dynhds,
                              अचर वर्ण *name, माप_प्रकार namelen)
{
  फर्काऊ !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(संरचना dynhds *dynhds, अचर वर्ण *name);
UNITTEST bool dynhds_ccontains(संरचना dynhds *dynhds, अचर वर्ण *name)
{
  फर्काऊ dynhds_contains(dynhds, name, म_माप(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST माप_प्रकार dynhds_गन्ती_name(संरचना dynhds *dynhds,
                                  अचर वर्ण *name, माप_प्रकार namelen);
UNITTEST माप_प्रकार dynhds_गन्ती_name(संरचना dynhds *dynhds,
                                  अचर वर्ण *name, माप_प्रकार namelen)
{
  माप_प्रकार n = 0;
  यदि(dynhds->hds_len) {
    माप_प्रकार i;
    लागि(i = 0; i < dynhds->hds_len; ++i) {
      यदि((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  फर्काऊ n;
}

/**
 * Return how often the given null-terminated name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST माप_प्रकार dynhds_ccount_name(संरचना dynhds *dynhds,
                                        अचर वर्ण *name);
UNITTEST माप_प्रकार dynhds_ccount_name(संरचना dynhds *dynhds,
                                        अचर वर्ण *name)
{
  फर्काऊ dynhds_गन्ती_name(dynhds, name, म_माप(name));
}

/**
 * Remove all entries with the given name.
 * Returns number of entries removed.
 *
 * @unittest 2602
 */
UNITTEST माप_प्रकार dynhds_remove(संरचना dynhds *dynhds,
                              अचर वर्ण *name, माप_प्रकार namelen);
UNITTEST माप_प्रकार dynhds_remove(संरचना dynhds *dynhds,
                              अचर वर्ण *name, माप_प्रकार namelen)
{
  माप_प्रकार n = 0;
  यदि(dynhds->hds_len) {
    माप_प्रकार i, len;
    लागि(i = 0; i < dynhds->hds_len; ++i) {
      यदि((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        entry_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        यदि(len) {
          स_हटाओ(&dynhds->hds[i], &dynhds->hds[i + 1],
                  len * परिमाण(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  फर्काऊ n;
}

/**
 * Set the give header name and value, replacing any entries with
 * the same name. The header is added at the end of all (remaining)
 * entries.
 *
 * @unittest 2602
 */
UNITTEST CURLcode dynhds_set(संरचना dynhds *dynhds,
                             अचर वर्ण *name, माप_प्रकार namelen,
                             अचर वर्ण *मान, माप_प्रकार valuelen);
UNITTEST CURLcode dynhds_set(संरचना dynhds *dynhds,
                             अचर वर्ण *name, माप_प्रकार namelen,
                             अचर वर्ण *मान, माप_प्रकार valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  फर्काऊ Curl_dynhds_add(dynhds, name, namelen, मान, valuelen);
}

/* @unittest 2602 */
UNITTEST माप_प्रकार dynhds_cremove(संरचना dynhds *dynhds, अचर वर्ण *name);
UNITTEST माप_प्रकार dynhds_cremove(संरचना dynhds *dynhds, अचर वर्ण *name)
{
  फर्काऊ dynhds_remove(dynhds, name, म_माप(name));
}

#endif /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(संरचना dynhds *dynhds, संरचना dynbuf *dbuf)
{
  CURLcode result = CURLE_OK;
  माप_प्रकार i;

  यदि(!dynhds->hds_len)
    फर्काऊ result;

  लागि(i = 0; i < dynhds->hds_len; ++i) {
    result = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    यदि(!result)
      result = curlx_dyn_addn(dbuf, STRCONST(": "));
    यदि(!result)
      result = curlx_dyn_addn(dbuf, dynhds->hds[i]->मान,
                              dynhds->hds[i]->valuelen);
    यदि(!result)
      result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    यदि(result)
      रोक;
  }

  फर्काऊ result;
}

#ifdef USE_NGHTTP2

nghttp2_nv *Curl_dynhds_to_nva(संरचना dynhds *dynhds, माप_प्रकार *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, परिमाण(nghttp2_nv) * dynhds->hds_len);
  माप_प्रकार i;

  *pcount = 0;
  यदि(!nva)
    फर्काऊ रिक्त;

  लागि(i = 0; i < dynhds->hds_len; ++i) {
    संरचना dynhds_entry *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (अचिन्हित वर्ण *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].मान = (अचिन्हित वर्ण *)e->मान;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  फर्काऊ nva;
}

#endif /* USE_NGHTTP2 */
