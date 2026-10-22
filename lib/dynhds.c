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
#समावेश "curl_setup.h"

#समावेश "dynhds.h"
#समावेश "strcase.h"

अटल काष्ठा dynhds_entry *entry_new(स्थिर अक्षर *name, माप_प्रकार namelen,
                                      स्थिर अक्षर *मूल्य, माप_प्रकार valuelen,
                                      पूर्णांक opts)
{
  काष्ठा dynhds_entry *e;
  अक्षर *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  अगर((SIZE_MAX - namelen - valuelen) < (माप(*e) + 2))
    वापस शून्य;
  e = curlx_calloc(1, माप(*e) + namelen + valuelen + 2);
  अगर(!e)
    वापस शून्य;
  e->name = p = (अक्षर *)e + माप(*e);
  स_नकल(p, name, namelen);
  e->namelen = namelen;
  e->मूल्य = p += namelen + 1; /* leave a \0 at the end of name */
  अगर(valuelen) {
    स_नकल(p, मूल्य, valuelen);
    e->valuelen = valuelen;
  }
  अगर(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  वापस e;
}

अटल व्योम entry_free(काष्ठा dynhds_entry *e)
{
  curlx_free(e);
}

व्योम Curl_dynhds_init(काष्ठा dynhds *dynhds, माप_प्रकार max_entries,
                      माप_प्रकार max_strs_आकार)
{
  DEBUGASSERT(dynhds);
  DEBUGASSERT(max_strs_आकार);
  dynhds->hds = शून्य;
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
  dynhds->max_entries = max_entries;
  dynhds->max_strs_आकार = max_strs_आकार;
  dynhds->opts = 0;
}

व्योम Curl_dynhds_free(काष्ठा dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  अगर(dynhds->hds && dynhds->hds_len) {
    माप_प्रकार i;
    DEBUGASSERT(dynhds->hds);
    क्रम(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

व्योम Curl_dynhds_reset(काष्ठा dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  अगर(dynhds->hds_len) {
    माप_प्रकार i;
    DEBUGASSERT(dynhds->hds);
    क्रम(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
      dynhds->hds[i] = शून्य;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

माप_प्रकार Curl_dynhds_गिनती(काष्ठा dynhds *dynhds)
{
  वापस dynhds->hds_len;
}

व्योम Curl_dynhds_set_opts(काष्ठा dynhds *dynhds, पूर्णांक opts)
{
  dynhds->opts = opts;
}

काष्ठा dynhds_entry *Curl_dynhds_getn(काष्ठा dynhds *dynhds, माप_प्रकार n)
{
  DEBUGASSERT(dynhds);
  वापस (n < dynhds->hds_len) ? dynhds->hds[n] : शून्य;
}

काष्ठा dynhds_entry *Curl_dynhds_get(काष्ठा dynhds *dynhds, स्थिर अक्षर *name,
                                     माप_प्रकार namelen)
{
  माप_प्रकार i;
  क्रम(i = 0; i < dynhds->hds_len; ++i) {
    अगर(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      वापस dynhds->hds[i];
    }
  }
  वापस शून्य;
}

काष्ठा dynhds_entry *Curl_dynhds_cget(काष्ठा dynhds *dynhds, स्थिर अक्षर *name)
{
  वापस Curl_dynhds_get(dynhds, name, म_माप(name));
}

CURLcode Curl_dynhds_add(काष्ठा dynhds *dynhds,
                         स्थिर अक्षर *name, माप_प्रकार namelen,
                         स्थिर अक्षर *मूल्य, माप_प्रकार valuelen)
{
  काष्ठा dynhds_entry *entry = शून्य;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;

  /* Accept NULL value with length 0, name must have length */
  अगर(!dynhds || !name || !namelen || (!मूल्य && valuelen)) {
    DEBUGASSERT(0);
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }
  /* Does the new header fit into the limits? */
  अगर((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_आकार - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_आकार - dynhds->strs_len - namelen < valuelen))
    वापस CURLE_OUT_OF_MEMORY;

  entry = entry_new(name, namelen, मूल्य, valuelen, dynhds->opts);
  अगर(!entry)
    जाओ out;

  अगर(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    माप_प्रकार nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    काष्ठा dynhds_entry **nhds;

    अगर(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, माप(काष्ठा dynhds_entry *));
    अगर(!nhds)
      जाओ out;
    अगर(dynhds->hds) {
      स_नकल(nhds, dynhds->hds,
             dynhds->hds_len * माप(काष्ठा dynhds_entry *));
      curlx_safefree(dynhds->hds);
    }
    dynhds->hds = nhds;
    dynhds->hds_allc = nallc;
  }
  dynhds->hds[dynhds->hds_len++] = entry;
  entry = शून्य;
  dynhds->strs_len += namelen + valuelen;
  परिणाम = CURLE_OK;

out:
  अगर(entry)
    entry_free(entry);
  वापस परिणाम;
}

CURLcode Curl_dynhds_cadd(काष्ठा dynhds *dynhds,
                          स्थिर अक्षर *name, स्थिर अक्षर *मूल्य)
{
  अगर(!name)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  वापस Curl_dynhds_add(dynhds, name, म_माप(name),
                         मूल्य, मूल्य ? म_माप(मूल्य) : 0);
}

CURLcode Curl_dynhds_h1_add_पंक्ति(काष्ठा dynhds *dynhds,
                                 स्थिर अक्षर *पंक्ति, माप_प्रकार पंक्ति_len)
{
  स्थिर अक्षर *p;
  स्थिर अक्षर *name;
  माप_प्रकार namelen;
  स्थिर अक्षर *मूल्य;
  माप_प्रकार valuelen, i;

  अगर(!पंक्ति || !पंक्ति_len)
    वापस CURLE_OK;

  p = स_प्रथम(पंक्ति, ':', पंक्ति_len);
  अगर(!p || p == पंक्ति)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  name = पंक्ति;
  namelen = p - पंक्ति;
  p++; /* move past the colon */
  क्रम(i = namelen + 1; i < पंक्ति_len; ++i, ++p) {
    अगर(!ISBLANK(*p))
      अवरोध;
  }
  मूल्य = p;
  valuelen = पंक्ति_len - i;

  p = स_प्रथम(मूल्य, '\r', valuelen);
  अगर(!p)
    p = स_प्रथम(मूल्य, '\n', valuelen);
  अगर(p)
    valuelen = (माप_प्रकार)(p - मूल्य);

  वापस Curl_dynhds_add(dynhds, name, namelen, मूल्य, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_पंक्ति(काष्ठा dynhds *dynhds, स्थिर अक्षर *पंक्ति)
{
  वापस Curl_dynhds_h1_add_पंक्ति(dynhds, पंक्ति, पंक्ति ? म_माप(पंक्ति) : 0);
}

#अगर_घोषित UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_contains(काष्ठा dynhds *dynhds,
                              स्थिर अक्षर *name, माप_प्रकार namelen);
UNITTEST bool dynhds_contains(काष्ठा dynhds *dynhds,
                              स्थिर अक्षर *name, माप_प्रकार namelen)
{
  वापस !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(काष्ठा dynhds *dynhds, स्थिर अक्षर *name);
UNITTEST bool dynhds_ccontains(काष्ठा dynhds *dynhds, स्थिर अक्षर *name)
{
  वापस dynhds_contains(dynhds, name, म_माप(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST माप_प्रकार dynhds_गिनती_name(काष्ठा dynhds *dynhds,
                                  स्थिर अक्षर *name, माप_प्रकार namelen);
UNITTEST माप_प्रकार dynhds_गिनती_name(काष्ठा dynhds *dynhds,
                                  स्थिर अक्षर *name, माप_प्रकार namelen)
{
  माप_प्रकार n = 0;
  अगर(dynhds->hds_len) {
    माप_प्रकार i;
    क्रम(i = 0; i < dynhds->hds_len; ++i) {
      अगर((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  वापस n;
}

/**
 * Return how often the given null-terminated name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST माप_प्रकार dynhds_ccount_name(काष्ठा dynhds *dynhds,
                                        स्थिर अक्षर *name);
UNITTEST माप_प्रकार dynhds_ccount_name(काष्ठा dynhds *dynhds,
                                        स्थिर अक्षर *name)
{
  वापस dynhds_गिनती_name(dynhds, name, म_माप(name));
}

/**
 * Remove all entries with the given name.
 * Returns number of entries removed.
 *
 * @unittest 2602
 */
UNITTEST माप_प्रकार dynhds_remove(काष्ठा dynhds *dynhds,
                              स्थिर अक्षर *name, माप_प्रकार namelen);
UNITTEST माप_प्रकार dynhds_remove(काष्ठा dynhds *dynhds,
                              स्थिर अक्षर *name, माप_प्रकार namelen)
{
  माप_प्रकार n = 0;
  अगर(dynhds->hds_len) {
    माप_प्रकार i, len;
    क्रम(i = 0; i < dynhds->hds_len; ++i) {
      अगर((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        entry_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        अगर(len) {
          स_हटाओ(&dynhds->hds[i], &dynhds->hds[i + 1],
                  len * माप(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  वापस n;
}

/**
 * Set the give header name and value, replacing any entries with
 * the same name. The header is added at the end of all (remaining)
 * entries.
 *
 * @unittest 2602
 */
UNITTEST CURLcode dynhds_set(काष्ठा dynhds *dynhds,
                             स्थिर अक्षर *name, माप_प्रकार namelen,
                             स्थिर अक्षर *मूल्य, माप_प्रकार valuelen);
UNITTEST CURLcode dynhds_set(काष्ठा dynhds *dynhds,
                             स्थिर अक्षर *name, माप_प्रकार namelen,
                             स्थिर अक्षर *मूल्य, माप_प्रकार valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  वापस Curl_dynhds_add(dynhds, name, namelen, मूल्य, valuelen);
}

/* @unittest 2602 */
UNITTEST माप_प्रकार dynhds_cremove(काष्ठा dynhds *dynhds, स्थिर अक्षर *name);
UNITTEST माप_प्रकार dynhds_cremove(काष्ठा dynhds *dynhds, स्थिर अक्षर *name)
{
  वापस dynhds_remove(dynhds, name, म_माप(name));
}

#पूर्ण_अगर /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(काष्ठा dynhds *dynhds, काष्ठा dynbuf *dbuf)
{
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार i;

  अगर(!dynhds->hds_len)
    वापस परिणाम;

  क्रम(i = 0; i < dynhds->hds_len; ++i) {
    परिणाम = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    अगर(!परिणाम)
      परिणाम = curlx_dyn_addn(dbuf, STRCONST(": "));
    अगर(!परिणाम)
      परिणाम = curlx_dyn_addn(dbuf, dynhds->hds[i]->मूल्य,
                              dynhds->hds[i]->valuelen);
    अगर(!परिणाम)
      परिणाम = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    अगर(परिणाम)
      अवरोध;
  }

  वापस परिणाम;
}

#अगर_घोषित USE_NGHTTP2

nghttp2_nv *Curl_dynhds_to_nva(काष्ठा dynhds *dynhds, माप_प्रकार *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, माप(nghttp2_nv) * dynhds->hds_len);
  माप_प्रकार i;

  *pcount = 0;
  अगर(!nva)
    वापस शून्य;

  क्रम(i = 0; i < dynhds->hds_len; ++i) {
    काष्ठा dynhds_entry *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (अचिन्हित अक्षर *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].मूल्य = (अचिन्हित अक्षर *)e->मूल्य;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  वापस nva;
}

#पूर्ण_अगर /* USE_NGHTTP2 */
