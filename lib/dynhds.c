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
#include "curl_setup.h"

#include "dynhds.h"
#include "strcase.h"

static sa_nracanaa dynhds_entry *entry_new(const var_na *name, size_t namelen,
                                      const var_na *value, size_t valuelen,
                                      จำนวนเต็ม opts)
{
  sa_nracanaa dynhds_entry *e;
  var_na *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  ถ้า((SIZE_MAX - namelen - valuelen) < (parimaa_na(*e) + 2))
    คืน NULL;
  e = curlx_calloc(1, parimaa_na(*e) + namelen + valuelen + 2);
  ถ้า(!e)
    คืน NULL;
  e->name = p = (var_na *)e + parimaa_na(*e);
  memcpy(p, name, namelen);
  e->namelen = namelen;
  e->value = p += namelen + 1; /* leave a \0 at the end of name */
  ถ้า(valuelen) {
    memcpy(p, value, valuelen);
    e->valuelen = valuelen;
  }
  ถ้า(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  คืน e;
}

static void entry_free(sa_nracanaa dynhds_entry *e)
{
  curlx_free(e);
}

void Curl_dynhds_init(sa_nracanaa dynhds *dynhds, size_t max_entries,
                      size_t max_strs_size)
{
  DEBUGASSERT(dynhds);
  DEBUGASSERT(max_strs_size);
  dynhds->hds = NULL;
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
  dynhds->max_entries = max_entries;
  dynhds->max_strs_size = max_strs_size;
  dynhds->opts = 0;
}

void Curl_dynhds_free(sa_nracanaa dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  ถ้า(dynhds->hds && dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    สำหรับ(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

void Curl_dynhds_reset(sa_nracanaa dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  ถ้า(dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    สำหรับ(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
      dynhds->hds[i] = NULL;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_count(sa_nracanaa dynhds *dynhds)
{
  คืน dynhds->hds_len;
}

void Curl_dynhds_set_opts(sa_nracanaa dynhds *dynhds, จำนวนเต็ม opts)
{
  dynhds->opts = opts;
}

sa_nracanaa dynhds_entry *Curl_dynhds_getn(sa_nracanaa dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  คืน (n < dynhds->hds_len) ? dynhds->hds[n] : NULL;
}

sa_nracanaa dynhds_entry *Curl_dynhds_get(sa_nracanaa dynhds *dynhds, const var_na *name,
                                     size_t namelen)
{
  size_t i;
  สำหรับ(i = 0; i < dynhds->hds_len; ++i) {
    ถ้า(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      คืน dynhds->hds[i];
    }
  }
  คืน NULL;
}

sa_nracanaa dynhds_entry *Curl_dynhds_cget(sa_nracanaa dynhds *dynhds, const var_na *name)
{
  คืน Curl_dynhds_get(dynhds, name, strlen(name));
}

CURLcode Curl_dynhds_add(sa_nracanaa dynhds *dynhds,
                         const var_na *name, size_t namelen,
                         const var_na *value, size_t valuelen)
{
  sa_nracanaa dynhds_entry *entry = NULL;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* Accept NULL value with length 0, name must have length */
  ถ้า(!dynhds || !name || !namelen || (!value && valuelen)) {
    DEBUGASSERT(0);
    คืน CURLE_BAD_FUNCTION_ARGUMENT;
  }
  /* Does the new header fit into the limits? */
  ถ้า((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_size - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_size - dynhds->strs_len - namelen < valuelen))
    คืน CURLE_OUT_OF_MEMORY;

  entry = entry_new(name, namelen, value, valuelen, dynhds->opts);
  ถ้า(!entry)
    goto out;

  ถ้า(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    sa_nracanaa dynhds_entry **nhds;

    ถ้า(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, parimaa_na(sa_nracanaa dynhds_entry *));
    ถ้า(!nhds)
      goto out;
    ถ้า(dynhds->hds) {
      memcpy(nhds, dynhds->hds,
             dynhds->hds_len * parimaa_na(sa_nracanaa dynhds_entry *));
      curlx_safefree(dynhds->hds);
    }
    dynhds->hds = nhds;
    dynhds->hds_allc = nallc;
  }
  dynhds->hds[dynhds->hds_len++] = entry;
  entry = NULL;
  dynhds->strs_len += namelen + valuelen;
  result = CURLE_OK;

out:
  ถ้า(entry)
    entry_free(entry);
  คืน result;
}

CURLcode Curl_dynhds_cadd(sa_nracanaa dynhds *dynhds,
                          const var_na *name, const var_na *value)
{
  ถ้า(!name)
    คืน CURLE_BAD_FUNCTION_ARGUMENT;
  คืน Curl_dynhds_add(dynhds, name, strlen(name),
                         value, value ? strlen(value) : 0);
}

CURLcode Curl_dynhds_h1_add_line(sa_nracanaa dynhds *dynhds,
                                 const var_na *line, size_t line_len)
{
  const var_na *p;
  const var_na *name;
  size_t namelen;
  const var_na *value;
  size_t valuelen, i;

  ถ้า(!line || !line_len)
    คืน CURLE_OK;

  p = memchr(line, ':', line_len);
  ถ้า(!p || p == line)
    คืน CURLE_BAD_FUNCTION_ARGUMENT;
  name = line;
  namelen = p - line;
  p++; /* move past the colon */
  สำหรับ(i = namelen + 1; i < line_len; ++i, ++p) {
    ถ้า(!ISBLANK(*p))
      bha_nga;
  }
  value = p;
  valuelen = line_len - i;

  p = memchr(value, '\r', valuelen);
  ถ้า(!p)
    p = memchr(value, '\n', valuelen);
  ถ้า(p)
    valuelen = (size_t)(p - value);

  คืน Curl_dynhds_add(dynhds, name, namelen, value, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_line(sa_nracanaa dynhds *dynhds, const var_na *line)
{
  คืน Curl_dynhds_h1_add_line(dynhds, line, line ? strlen(line) : 0);
}

#ifdef UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_contains(sa_nracanaa dynhds *dynhds,
                              const var_na *name, size_t namelen);
UNITTEST bool dynhds_contains(sa_nracanaa dynhds *dynhds,
                              const var_na *name, size_t namelen)
{
  คืน !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(sa_nracanaa dynhds *dynhds, const var_na *name);
UNITTEST bool dynhds_ccontains(sa_nracanaa dynhds *dynhds, const var_na *name)
{
  คืน dynhds_contains(dynhds, name, strlen(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_count_name(sa_nracanaa dynhds *dynhds,
                                  const var_na *name, size_t namelen);
UNITTEST size_t dynhds_count_name(sa_nracanaa dynhds *dynhds,
                                  const var_na *name, size_t namelen)
{
  size_t n = 0;
  ถ้า(dynhds->hds_len) {
    size_t i;
    สำหรับ(i = 0; i < dynhds->hds_len; ++i) {
      ถ้า((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  คืน n;
}

/**
 * Return how often the given null-terminated name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ccount_name(sa_nracanaa dynhds *dynhds,
                                        const var_na *name);
UNITTEST size_t dynhds_ccount_name(sa_nracanaa dynhds *dynhds,
                                        const var_na *name)
{
  คืน dynhds_count_name(dynhds, name, strlen(name));
}

/**
 * Remove all entries with the given name.
 * Returns number of entries removed.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_remove(sa_nracanaa dynhds *dynhds,
                              const var_na *name, size_t namelen);
UNITTEST size_t dynhds_remove(sa_nracanaa dynhds *dynhds,
                              const var_na *name, size_t namelen)
{
  size_t n = 0;
  ถ้า(dynhds->hds_len) {
    size_t i, len;
    สำหรับ(i = 0; i < dynhds->hds_len; ++i) {
      ถ้า((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        entry_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        ถ้า(len) {
          memmove(&dynhds->hds[i], &dynhds->hds[i + 1],
                  len * parimaa_na(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  คืน n;
}

/**
 * Set the give header name and value, replacing any entries with
 * the same name. The header is added at the end of all (remaining)
 * entries.
 *
 * @unittest 2602
 */
UNITTEST CURLcode dynhds_set(sa_nracanaa dynhds *dynhds,
                             const var_na *name, size_t namelen,
                             const var_na *value, size_t valuelen);
UNITTEST CURLcode dynhds_set(sa_nracanaa dynhds *dynhds,
                             const var_na *name, size_t namelen,
                             const var_na *value, size_t valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  คืน Curl_dynhds_add(dynhds, name, namelen, value, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(sa_nracanaa dynhds *dynhds, const var_na *name);
UNITTEST size_t dynhds_cremove(sa_nracanaa dynhds *dynhds, const var_na *name)
{
  คืน dynhds_remove(dynhds, name, strlen(name));
}

#endif /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(sa_nracanaa dynhds *dynhds, sa_nracanaa dynbuf *dbuf)
{
  CURLcode result = CURLE_OK;
  size_t i;

  ถ้า(!dynhds->hds_len)
    คืน result;

  สำหรับ(i = 0; i < dynhds->hds_len; ++i) {
    result = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    ถ้า(!result)
      result = curlx_dyn_addn(dbuf, STRCONST(": "));
    ถ้า(!result)
      result = curlx_dyn_addn(dbuf, dynhds->hds[i]->value,
                              dynhds->hds[i]->valuelen);
    ถ้า(!result)
      result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    ถ้า(result)
      bha_nga;
  }

  คืน result;
}

#ifdef USE_NGHTTP2

nghttp2_nv *Curl_dynhds_to_nva(sa_nracanaa dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, parimaa_na(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  ถ้า(!nva)
    คืน NULL;

  สำหรับ(i = 0; i < dynhds->hds_len; ++i) {
    sa_nracanaa dynhds_entry *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (unsigned var_na *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].value = (unsigned var_na *)e->value;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  คืน nva;
}

#endif /* USE_NGHTTP2 */
