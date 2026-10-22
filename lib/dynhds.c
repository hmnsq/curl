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

static struct dynhds_entry *entry_new(const char *name, size_t namelen,
                                      const char *value, size_t valuelen,
                                      整数 opts)
{
  struct dynhds_entry *e;
  char *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  もし((SIZE_MAX - namelen - valuelen) < (sizeof(*e) + 2))
    戻る NULL;
  e = curlx_calloc(1, sizeof(*e) + namelen + valuelen + 2);
  もし(!e)
    戻る NULL;
  e->name = p = (char *)e + sizeof(*e);
  memcpy(p, name, namelen);
  e->namelen = namelen;
  e->value = p += namelen + 1; /* leave a \0 at the end of name */
  もし(valuelen) {
    memcpy(p, value, valuelen);
    e->valuelen = valuelen;
  }
  もし(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  戻る e;
}

static void entry_free(struct dynhds_entry *e)
{
  curlx_free(e);
}

void Curl_dynhds_init(struct dynhds *dynhds, size_t max_entries,
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

void Curl_dynhds_free(struct dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  もし(dynhds->hds && dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    ために(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

void Curl_dynhds_reset(struct dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  もし(dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    ために(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
      dynhds->hds[i] = NULL;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_count(struct dynhds *dynhds)
{
  戻る dynhds->hds_len;
}

void Curl_dynhds_set_opts(struct dynhds *dynhds, 整数 opts)
{
  dynhds->opts = opts;
}

struct dynhds_entry *Curl_dynhds_getn(struct dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  戻る (n < dynhds->hds_len) ? dynhds->hds[n] : NULL;
}

struct dynhds_entry *Curl_dynhds_get(struct dynhds *dynhds, const char *name,
                                     size_t namelen)
{
  size_t i;
  ために(i = 0; i < dynhds->hds_len; ++i) {
    もし(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      戻る dynhds->hds[i];
    }
  }
  戻る NULL;
}

struct dynhds_entry *Curl_dynhds_cget(struct dynhds *dynhds, const char *name)
{
  戻る Curl_dynhds_get(dynhds, name, strlen(name));
}

CURLcode Curl_dynhds_add(struct dynhds *dynhds,
                         const char *name, size_t namelen,
                         const char *value, size_t valuelen)
{
  struct dynhds_entry *entry = NULL;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* Accept NULL value with length 0, name must have length */
  もし(!dynhds || !name || !namelen || (!value && valuelen)) {
    DEBUGASSERT(0);
    戻る CURLE_BAD_FUNCTION_ARGUMENT;
  }
  /* Does the new header fit into the limits? */
  もし((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_size - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_size - dynhds->strs_len - namelen < valuelen))
    戻る CURLE_OUT_OF_MEMORY;

  entry = entry_new(name, namelen, value, valuelen, dynhds->opts);
  もし(!entry)
    goto out;

  もし(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    struct dynhds_entry **nhds;

    もし(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, sizeof(struct dynhds_entry *));
    もし(!nhds)
      goto out;
    もし(dynhds->hds) {
      memcpy(nhds, dynhds->hds,
             dynhds->hds_len * sizeof(struct dynhds_entry *));
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
  もし(entry)
    entry_free(entry);
  戻る result;
}

CURLcode Curl_dynhds_cadd(struct dynhds *dynhds,
                          const char *name, const char *value)
{
  もし(!name)
    戻る CURLE_BAD_FUNCTION_ARGUMENT;
  戻る Curl_dynhds_add(dynhds, name, strlen(name),
                         value, value ? strlen(value) : 0);
}

CURLcode Curl_dynhds_h1_add_line(struct dynhds *dynhds,
                                 const char *line, size_t line_len)
{
  const char *p;
  const char *name;
  size_t namelen;
  const char *value;
  size_t valuelen, i;

  もし(!line || !line_len)
    戻る CURLE_OK;

  p = memchr(line, ':', line_len);
  もし(!p || p == line)
    戻る CURLE_BAD_FUNCTION_ARGUMENT;
  name = line;
  namelen = p - line;
  p++; /* move past the colon */
  ために(i = namelen + 1; i < line_len; ++i, ++p) {
    もし(!ISBLANK(*p))
      break;
  }
  value = p;
  valuelen = line_len - i;

  p = memchr(value, '\r', valuelen);
  もし(!p)
    p = memchr(value, '\n', valuelen);
  もし(p)
    valuelen = (size_t)(p - value);

  戻る Curl_dynhds_add(dynhds, name, namelen, value, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_line(struct dynhds *dynhds, const char *line)
{
  戻る Curl_dynhds_h1_add_line(dynhds, line, line ? strlen(line) : 0);
}

#ifdef UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_contains(struct dynhds *dynhds,
                              const char *name, size_t namelen);
UNITTEST bool dynhds_contains(struct dynhds *dynhds,
                              const char *name, size_t namelen)
{
  戻る !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(struct dynhds *dynhds, const char *name);
UNITTEST bool dynhds_ccontains(struct dynhds *dynhds, const char *name)
{
  戻る dynhds_contains(dynhds, name, strlen(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_count_name(struct dynhds *dynhds,
                                  const char *name, size_t namelen);
UNITTEST size_t dynhds_count_name(struct dynhds *dynhds,
                                  const char *name, size_t namelen)
{
  size_t n = 0;
  もし(dynhds->hds_len) {
    size_t i;
    ために(i = 0; i < dynhds->hds_len; ++i) {
      もし((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  戻る n;
}

/**
 * Return how often the given null-terminated name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ccount_name(struct dynhds *dynhds,
                                        const char *name);
UNITTEST size_t dynhds_ccount_name(struct dynhds *dynhds,
                                        const char *name)
{
  戻る dynhds_count_name(dynhds, name, strlen(name));
}

/**
 * Remove all entries with the given name.
 * Returns number of entries removed.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_remove(struct dynhds *dynhds,
                              const char *name, size_t namelen);
UNITTEST size_t dynhds_remove(struct dynhds *dynhds,
                              const char *name, size_t namelen)
{
  size_t n = 0;
  もし(dynhds->hds_len) {
    size_t i, len;
    ために(i = 0; i < dynhds->hds_len; ++i) {
      もし((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        entry_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        もし(len) {
          memmove(&dynhds->hds[i], &dynhds->hds[i + 1],
                  len * sizeof(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  戻る n;
}

/**
 * Set the give header name and value, replacing any entries with
 * the same name. The header is added at the end of all (remaining)
 * entries.
 *
 * @unittest 2602
 */
UNITTEST CURLcode dynhds_set(struct dynhds *dynhds,
                             const char *name, size_t namelen,
                             const char *value, size_t valuelen);
UNITTEST CURLcode dynhds_set(struct dynhds *dynhds,
                             const char *name, size_t namelen,
                             const char *value, size_t valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  戻る Curl_dynhds_add(dynhds, name, namelen, value, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(struct dynhds *dynhds, const char *name);
UNITTEST size_t dynhds_cremove(struct dynhds *dynhds, const char *name)
{
  戻る dynhds_remove(dynhds, name, strlen(name));
}

#endif /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(struct dynhds *dynhds, struct dynbuf *dbuf)
{
  CURLcode result = CURLE_OK;
  size_t i;

  もし(!dynhds->hds_len)
    戻る result;

  ために(i = 0; i < dynhds->hds_len; ++i) {
    result = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    もし(!result)
      result = curlx_dyn_addn(dbuf, STRCONST(": "));
    もし(!result)
      result = curlx_dyn_addn(dbuf, dynhds->hds[i]->value,
                              dynhds->hds[i]->valuelen);
    もし(!result)
      result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    もし(result)
      break;
  }

  戻る result;
}

#ifdef USE_NGHTTP2

nghttp2_nv *Curl_dynhds_to_nva(struct dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, sizeof(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  もし(!nva)
    戻る NULL;

  ために(i = 0; i < dynhds->hds_len; ++i) {
    struct dynhds_entry *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (unsigned char *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].value = (unsigned char *)e->value;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  戻る nva;
}

#endif /* USE_NGHTTP2 */
