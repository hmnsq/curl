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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "dynhds.h"
#അന്തര്ഭാവ "strcase.h"

സ്ഥിതം ഘടന dynhds_entry *entry_new(സ്ഥിരം അക്ഷരം *name, size_t namelen,
                                      സ്ഥിരം അക്ഷരം *മൂല്യം, size_t valuelen,
                                      പൂർണ്ണസംഖ്യ opts)
{
  ഘടന dynhds_entry *e;
  അക്ഷരം *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  എങ്കിൽ((SIZE_MAX - namelen - valuelen) < (വലുപ്പം(*e) + 2))
    തിരികെ ശൂന്യ;
  e = curlx_calloc(1, വലുപ്പം(*e) + namelen + valuelen + 2);
  എങ്കിൽ(!e)
    തിരികെ ശൂന്യ;
  e->name = p = (അക്ഷരം *)e + വലുപ്പം(*e);
  memcpy(p, name, namelen);
  e->namelen = namelen;
  e->മൂല്യം = p += namelen + 1; /* leave a \0 at the end of name */
  എങ്കിൽ(valuelen) {
    memcpy(p, മൂല്യം, valuelen);
    e->valuelen = valuelen;
  }
  എങ്കിൽ(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  തിരികെ e;
}

സ്ഥിതം ശൂന്യം entry_free(ഘടന dynhds_entry *e)
{
  curlx_free(e);
}

ശൂന്യം Curl_dynhds_init(ഘടന dynhds *dynhds, size_t max_entries,
                      size_t max_strs_size)
{
  DEBUGASSERT(dynhds);
  DEBUGASSERT(max_strs_size);
  dynhds->hds = ശൂന്യ;
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
  dynhds->max_entries = max_entries;
  dynhds->max_strs_size = max_strs_size;
  dynhds->opts = 0;
}

ശൂന്യം Curl_dynhds_free(ഘടന dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  എങ്കിൽ(dynhds->hds && dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    വേണ്ടി(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

ശൂന്യം Curl_dynhds_reset(ഘടന dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  എങ്കിൽ(dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    വേണ്ടി(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
      dynhds->hds[i] = ശൂന്യ;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_എണ്ണം(ഘടന dynhds *dynhds)
{
  തിരികെ dynhds->hds_len;
}

ശൂന്യം Curl_dynhds_set_opts(ഘടന dynhds *dynhds, പൂർണ്ണസംഖ്യ opts)
{
  dynhds->opts = opts;
}

ഘടന dynhds_entry *Curl_dynhds_getn(ഘടന dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  തിരികെ (n < dynhds->hds_len) ? dynhds->hds[n] : ശൂന്യ;
}

ഘടന dynhds_entry *Curl_dynhds_get(ഘടന dynhds *dynhds, സ്ഥിരം അക്ഷരം *name,
                                     size_t namelen)
{
  size_t i;
  വേണ്ടി(i = 0; i < dynhds->hds_len; ++i) {
    എങ്കിൽ(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      തിരികെ dynhds->hds[i];
    }
  }
  തിരികെ ശൂന്യ;
}

ഘടന dynhds_entry *Curl_dynhds_cget(ഘടന dynhds *dynhds, സ്ഥിരം അക്ഷരം *name)
{
  തിരികെ Curl_dynhds_get(dynhds, name, strlen(name));
}

CURLcode Curl_dynhds_add(ഘടന dynhds *dynhds,
                         സ്ഥിരം അക്ഷരം *name, size_t namelen,
                         സ്ഥിരം അക്ഷരം *മൂല്യം, size_t valuelen)
{
  ഘടന dynhds_entry *entry = ശൂന്യ;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* Accept NULL value with length 0, name must have length */
  എങ്കിൽ(!dynhds || !name || !namelen || (!മൂല്യം && valuelen)) {
    DEBUGASSERT(0);
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  /* Does the new header fit into the limits? */
  എങ്കിൽ((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_size - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_size - dynhds->strs_len - namelen < valuelen))
    തിരികെ CURLE_OUT_OF_MEMORY;

  entry = entry_new(name, namelen, മൂല്യം, valuelen, dynhds->opts);
  എങ്കിൽ(!entry)
    പോകുക out;

  എങ്കിൽ(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    ഘടന dynhds_entry **nhds;

    എങ്കിൽ(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, വലുപ്പം(ഘടന dynhds_entry *));
    എങ്കിൽ(!nhds)
      പോകുക out;
    എങ്കിൽ(dynhds->hds) {
      memcpy(nhds, dynhds->hds,
             dynhds->hds_len * വലുപ്പം(ഘടന dynhds_entry *));
      curlx_safefree(dynhds->hds);
    }
    dynhds->hds = nhds;
    dynhds->hds_allc = nallc;
  }
  dynhds->hds[dynhds->hds_len++] = entry;
  entry = ശൂന്യ;
  dynhds->strs_len += namelen + valuelen;
  result = CURLE_OK;

out:
  എങ്കിൽ(entry)
    entry_free(entry);
  തിരികെ result;
}

CURLcode Curl_dynhds_cadd(ഘടന dynhds *dynhds,
                          സ്ഥിരം അക്ഷരം *name, സ്ഥിരം അക്ഷരം *മൂല്യം)
{
  എങ്കിൽ(!name)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  തിരികെ Curl_dynhds_add(dynhds, name, strlen(name),
                         മൂല്യം, മൂല്യം ? strlen(മൂല്യം) : 0);
}

CURLcode Curl_dynhds_h1_add_വരി(ഘടന dynhds *dynhds,
                                 സ്ഥിരം അക്ഷരം *വരി, size_t വരി_len)
{
  സ്ഥിരം അക്ഷരം *p;
  സ്ഥിരം അക്ഷരം *name;
  size_t namelen;
  സ്ഥിരം അക്ഷരം *മൂല്യം;
  size_t valuelen, i;

  എങ്കിൽ(!വരി || !വരി_len)
    തിരികെ CURLE_OK;

  p = memchr(വരി, ':', വരി_len);
  എങ്കിൽ(!p || p == വരി)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  name = വരി;
  namelen = p - വരി;
  p++; /* move past the colon */
  വേണ്ടി(i = namelen + 1; i < വരി_len; ++i, ++p) {
    എങ്കിൽ(!ISBLANK(*p))
      നിർത്തുക;
  }
  മൂല്യം = p;
  valuelen = വരി_len - i;

  p = memchr(മൂല്യം, '\r', valuelen);
  എങ്കിൽ(!p)
    p = memchr(മൂല്യം, '\n', valuelen);
  എങ്കിൽ(p)
    valuelen = (size_t)(p - മൂല്യം);

  തിരികെ Curl_dynhds_add(dynhds, name, namelen, മൂല്യം, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_വരി(ഘടന dynhds *dynhds, സ്ഥിരം അക്ഷരം *വരി)
{
  തിരികെ Curl_dynhds_h1_add_വരി(dynhds, വരി, വരി ? strlen(വരി) : 0);
}

#ifdef UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_contains(ഘടന dynhds *dynhds,
                              സ്ഥിരം അക്ഷരം *name, size_t namelen);
UNITTEST bool dynhds_contains(ഘടന dynhds *dynhds,
                              സ്ഥിരം അക്ഷരം *name, size_t namelen)
{
  തിരികെ !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(ഘടന dynhds *dynhds, സ്ഥിരം അക്ഷരം *name);
UNITTEST bool dynhds_ccontains(ഘടന dynhds *dynhds, സ്ഥിരം അക്ഷരം *name)
{
  തിരികെ dynhds_contains(dynhds, name, strlen(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_എണ്ണം_name(ഘടന dynhds *dynhds,
                                  സ്ഥിരം അക്ഷരം *name, size_t namelen);
UNITTEST size_t dynhds_എണ്ണം_name(ഘടന dynhds *dynhds,
                                  സ്ഥിരം അക്ഷരം *name, size_t namelen)
{
  size_t n = 0;
  എങ്കിൽ(dynhds->hds_len) {
    size_t i;
    വേണ്ടി(i = 0; i < dynhds->hds_len; ++i) {
      എങ്കിൽ((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  തിരികെ n;
}

/**
 * Return how often the given null-terminated name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ccount_name(ഘടന dynhds *dynhds,
                                        സ്ഥിരം അക്ഷരം *name);
UNITTEST size_t dynhds_ccount_name(ഘടന dynhds *dynhds,
                                        സ്ഥിരം അക്ഷരം *name)
{
  തിരികെ dynhds_എണ്ണം_name(dynhds, name, strlen(name));
}

/**
 * Remove all entries with the given name.
 * Returns number of entries removed.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_remove(ഘടന dynhds *dynhds,
                              സ്ഥിരം അക്ഷരം *name, size_t namelen);
UNITTEST size_t dynhds_remove(ഘടന dynhds *dynhds,
                              സ്ഥിരം അക്ഷരം *name, size_t namelen)
{
  size_t n = 0;
  എങ്കിൽ(dynhds->hds_len) {
    size_t i, len;
    വേണ്ടി(i = 0; i < dynhds->hds_len; ++i) {
      എങ്കിൽ((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        entry_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        എങ്കിൽ(len) {
          memmove(&dynhds->hds[i], &dynhds->hds[i + 1],
                  len * വലുപ്പം(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  തിരികെ n;
}

/**
 * Set the give header name and value, replacing any entries with
 * the same name. The header is added at the end of all (remaining)
 * entries.
 *
 * @unittest 2602
 */
UNITTEST CURLcode dynhds_set(ഘടന dynhds *dynhds,
                             സ്ഥിരം അക്ഷരം *name, size_t namelen,
                             സ്ഥിരം അക്ഷരം *മൂല്യം, size_t valuelen);
UNITTEST CURLcode dynhds_set(ഘടന dynhds *dynhds,
                             സ്ഥിരം അക്ഷരം *name, size_t namelen,
                             സ്ഥിരം അക്ഷരം *മൂല്യം, size_t valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  തിരികെ Curl_dynhds_add(dynhds, name, namelen, മൂല്യം, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(ഘടന dynhds *dynhds, സ്ഥിരം അക്ഷരം *name);
UNITTEST size_t dynhds_cremove(ഘടന dynhds *dynhds, സ്ഥിരം അക്ഷരം *name)
{
  തിരികെ dynhds_remove(dynhds, name, strlen(name));
}

#endif /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(ഘടന dynhds *dynhds, ഘടന dynbuf *dbuf)
{
  CURLcode result = CURLE_OK;
  size_t i;

  എങ്കിൽ(!dynhds->hds_len)
    തിരികെ result;

  വേണ്ടി(i = 0; i < dynhds->hds_len; ++i) {
    result = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    എങ്കിൽ(!result)
      result = curlx_dyn_addn(dbuf, STRCONST(": "));
    എങ്കിൽ(!result)
      result = curlx_dyn_addn(dbuf, dynhds->hds[i]->മൂല്യം,
                              dynhds->hds[i]->valuelen);
    എങ്കിൽ(!result)
      result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    എങ്കിൽ(result)
      നിർത്തുക;
  }

  തിരികെ result;
}

#ifdef USE_NGHTTP2

nghttp2_nv *Curl_dynhds_to_nva(ഘടന dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, വലുപ്പം(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  എങ്കിൽ(!nva)
    തിരികെ ശൂന്യ;

  വേണ്ടി(i = 0; i < dynhds->hds_len; ++i) {
    ഘടന dynhds_entry *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (unsigned അക്ഷരം *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].മൂല്യം = (unsigned അക്ഷരം *)e->മൂല്യം;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  തിരികെ nva;
}

#endif /* USE_NGHTTP2 */
