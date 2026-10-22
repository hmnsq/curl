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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "dynhds.h"
#અન્તર્ભાવ "strcase.h"

સ્થિર સંરચના dynhds_entry *entry_new(અચલ અક્ષર *name, size_t namelen,
                                      અચલ અક્ષર *કિંમત, size_t valuelen,
                                      પૂર્ણાંક opts)
{
  સંરચના dynhds_entry *e;
  અક્ષર *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  જો((SIZE_MAX - namelen - valuelen) < (કદ(*e) + 2))
    પાછા ખાલી;
  e = curlx_calloc(1, કદ(*e) + namelen + valuelen + 2);
  જો(!e)
    પાછા ખાલી;
  e->name = p = (અક્ષર *)e + કદ(*e);
  memcpy(p, name, namelen);
  e->namelen = namelen;
  e->કિંમત = p += namelen + 1; /* leave a \0 at the end of name */
  જો(valuelen) {
    memcpy(p, કિંમત, valuelen);
    e->valuelen = valuelen;
  }
  જો(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  પાછા e;
}

સ્થિર શૂન્ય entry_free(સંરચના dynhds_entry *e)
{
  curlx_free(e);
}

શૂન્ય Curl_dynhds_init(સંરચના dynhds *dynhds, size_t max_entries,
                      size_t max_strs_size)
{
  DEBUGASSERT(dynhds);
  DEBUGASSERT(max_strs_size);
  dynhds->hds = ખાલી;
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
  dynhds->max_entries = max_entries;
  dynhds->max_strs_size = max_strs_size;
  dynhds->opts = 0;
}

શૂન્ય Curl_dynhds_free(સંરચના dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  જો(dynhds->hds && dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    માટે(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

શૂન્ય Curl_dynhds_reset(સંરચના dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  જો(dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    માટે(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
      dynhds->hds[i] = ખાલી;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_ગણતરી(સંરચના dynhds *dynhds)
{
  પાછા dynhds->hds_len;
}

શૂન્ય Curl_dynhds_set_opts(સંરચના dynhds *dynhds, પૂર્ણાંક opts)
{
  dynhds->opts = opts;
}

સંરચના dynhds_entry *Curl_dynhds_getn(સંરચના dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  પાછા (n < dynhds->hds_len) ? dynhds->hds[n] : ખાલી;
}

સંરચના dynhds_entry *Curl_dynhds_get(સંરચના dynhds *dynhds, અચલ અક્ષર *name,
                                     size_t namelen)
{
  size_t i;
  માટે(i = 0; i < dynhds->hds_len; ++i) {
    જો(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      પાછા dynhds->hds[i];
    }
  }
  પાછા ખાલી;
}

સંરચના dynhds_entry *Curl_dynhds_cget(સંરચના dynhds *dynhds, અચલ અક્ષર *name)
{
  પાછા Curl_dynhds_get(dynhds, name, strlen(name));
}

CURLcode Curl_dynhds_add(સંરચના dynhds *dynhds,
                         અચલ અક્ષર *name, size_t namelen,
                         અચલ અક્ષર *કિંમત, size_t valuelen)
{
  સંરચના dynhds_entry *entry = ખાલી;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* Accept NULL value with length 0, name must have length */
  જો(!dynhds || !name || !namelen || (!કિંમત && valuelen)) {
    DEBUGASSERT(0);
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }
  /* Does the new header fit into the limits? */
  જો((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_size - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_size - dynhds->strs_len - namelen < valuelen))
    પાછા CURLE_OUT_OF_MEMORY;

  entry = entry_new(name, namelen, કિંમત, valuelen, dynhds->opts);
  જો(!entry)
    જાઓ out;

  જો(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    સંરચના dynhds_entry **nhds;

    જો(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, કદ(સંરચના dynhds_entry *));
    જો(!nhds)
      જાઓ out;
    જો(dynhds->hds) {
      memcpy(nhds, dynhds->hds,
             dynhds->hds_len * કદ(સંરચના dynhds_entry *));
      curlx_safefree(dynhds->hds);
    }
    dynhds->hds = nhds;
    dynhds->hds_allc = nallc;
  }
  dynhds->hds[dynhds->hds_len++] = entry;
  entry = ખાલી;
  dynhds->strs_len += namelen + valuelen;
  result = CURLE_OK;

out:
  જો(entry)
    entry_free(entry);
  પાછા result;
}

CURLcode Curl_dynhds_cadd(સંરચના dynhds *dynhds,
                          અચલ અક્ષર *name, અચલ અક્ષર *કિંમત)
{
  જો(!name)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  પાછા Curl_dynhds_add(dynhds, name, strlen(name),
                         કિંમત, કિંમત ? strlen(કિંમત) : 0);
}

CURLcode Curl_dynhds_h1_add_લીટી(સંરચના dynhds *dynhds,
                                 અચલ અક્ષર *લીટી, size_t લીટી_len)
{
  અચલ અક્ષર *p;
  અચલ અક્ષર *name;
  size_t namelen;
  અચલ અક્ષર *કિંમત;
  size_t valuelen, i;

  જો(!લીટી || !લીટી_len)
    પાછા CURLE_OK;

  p = memchr(લીટી, ':', લીટી_len);
  જો(!p || p == લીટી)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  name = લીટી;
  namelen = p - લીટી;
  p++; /* move past the colon */
  માટે(i = namelen + 1; i < લીટી_len; ++i, ++p) {
    જો(!ISBLANK(*p))
      તોડો;
  }
  કિંમત = p;
  valuelen = લીટી_len - i;

  p = memchr(કિંમત, '\r', valuelen);
  જો(!p)
    p = memchr(કિંમત, '\n', valuelen);
  જો(p)
    valuelen = (size_t)(p - કિંમત);

  પાછા Curl_dynhds_add(dynhds, name, namelen, કિંમત, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_લીટી(સંરચના dynhds *dynhds, અચલ અક્ષર *લીટી)
{
  પાછા Curl_dynhds_h1_add_લીટી(dynhds, લીટી, લીટી ? strlen(લીટી) : 0);
}

#ifdef UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_contains(સંરચના dynhds *dynhds,
                              અચલ અક્ષર *name, size_t namelen);
UNITTEST bool dynhds_contains(સંરચના dynhds *dynhds,
                              અચલ અક્ષર *name, size_t namelen)
{
  પાછા !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(સંરચના dynhds *dynhds, અચલ અક્ષર *name);
UNITTEST bool dynhds_ccontains(સંરચના dynhds *dynhds, અચલ અક્ષર *name)
{
  પાછા dynhds_contains(dynhds, name, strlen(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ગણતરી_name(સંરચના dynhds *dynhds,
                                  અચલ અક્ષર *name, size_t namelen);
UNITTEST size_t dynhds_ગણતરી_name(સંરચના dynhds *dynhds,
                                  અચલ અક્ષર *name, size_t namelen)
{
  size_t n = 0;
  જો(dynhds->hds_len) {
    size_t i;
    માટે(i = 0; i < dynhds->hds_len; ++i) {
      જો((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  પાછા n;
}

/**
 * Return how often the given null-terminated name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ccount_name(સંરચના dynhds *dynhds,
                                        અચલ અક્ષર *name);
UNITTEST size_t dynhds_ccount_name(સંરચના dynhds *dynhds,
                                        અચલ અક્ષર *name)
{
  પાછા dynhds_ગણતરી_name(dynhds, name, strlen(name));
}

/**
 * Remove all entries with the given name.
 * Returns number of entries removed.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_remove(સંરચના dynhds *dynhds,
                              અચલ અક્ષર *name, size_t namelen);
UNITTEST size_t dynhds_remove(સંરચના dynhds *dynhds,
                              અચલ અક્ષર *name, size_t namelen)
{
  size_t n = 0;
  જો(dynhds->hds_len) {
    size_t i, len;
    માટે(i = 0; i < dynhds->hds_len; ++i) {
      જો((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        entry_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        જો(len) {
          memmove(&dynhds->hds[i], &dynhds->hds[i + 1],
                  len * કદ(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  પાછા n;
}

/**
 * Set the give header name and value, replacing any entries with
 * the same name. The header is added at the end of all (remaining)
 * entries.
 *
 * @unittest 2602
 */
UNITTEST CURLcode dynhds_set(સંરચના dynhds *dynhds,
                             અચલ અક્ષર *name, size_t namelen,
                             અચલ અક્ષર *કિંમત, size_t valuelen);
UNITTEST CURLcode dynhds_set(સંરચના dynhds *dynhds,
                             અચલ અક્ષર *name, size_t namelen,
                             અચલ અક્ષર *કિંમત, size_t valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  પાછા Curl_dynhds_add(dynhds, name, namelen, કિંમત, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(સંરચના dynhds *dynhds, અચલ અક્ષર *name);
UNITTEST size_t dynhds_cremove(સંરચના dynhds *dynhds, અચલ અક્ષર *name)
{
  પાછા dynhds_remove(dynhds, name, strlen(name));
}

#endif /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(સંરચના dynhds *dynhds, સંરચના dynbuf *dbuf)
{
  CURLcode result = CURLE_OK;
  size_t i;

  જો(!dynhds->hds_len)
    પાછા result;

  માટે(i = 0; i < dynhds->hds_len; ++i) {
    result = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    જો(!result)
      result = curlx_dyn_addn(dbuf, STRCONST(": "));
    જો(!result)
      result = curlx_dyn_addn(dbuf, dynhds->hds[i]->કિંમત,
                              dynhds->hds[i]->valuelen);
    જો(!result)
      result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    જો(result)
      તોડો;
  }

  પાછા result;
}

#ifdef USE_NGHTTP2

nghttp2_nv *Curl_dynhds_to_nva(સંરચના dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, કદ(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  જો(!nva)
    પાછા ખાલી;

  માટે(i = 0; i < dynhds->hds_len; ++i) {
    સંરચના dynhds_entry *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (unsigned અક્ષર *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].કિંમત = (unsigned અક્ષર *)e->કિંમત;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  પાછા nva;
}

#endif /* USE_NGHTTP2 */
