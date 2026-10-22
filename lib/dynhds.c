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
                                      целое opts)
{
  struct dynhds_entry *e;
  char *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  если((SIZE_MAX - namelen - valuelen) < (sizeof(*e) + 2))
    вернуть NULL;
  e = curlx_calloc(1, sizeof(*e) + namelen + valuelen + 2);
  если(!e)
    вернуть NULL;
  e->name = p = (char *)e + sizeof(*e);
  memcpy(p, name, namelen);
  e->namelen = namelen;
  e->value = p += namelen + 1; /* leave a \0 at the end of name */
  если(valuelen) {
    memcpy(p, value, valuelen);
    e->valuelen = valuelen;
  }
  если(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  вернуть e;
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
  если(dynhds->hds && dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    для(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

void Curl_dynhds_reset(struct dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  если(dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    для(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
      dynhds->hds[i] = NULL;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_count(struct dynhds *dynhds)
{
  вернуть dynhds->hds_len;
}

void Curl_dynhds_set_opts(struct dynhds *dynhds, целое opts)
{
  dynhds->opts = opts;
}

struct dynhds_entry *Curl_dynhds_getn(struct dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  вернуть (n < dynhds->hds_len) ? dynhds->hds[n] : NULL;
}

struct dynhds_entry *Curl_dynhds_get(struct dynhds *dynhds, const char *name,
                                     size_t namelen)
{
  size_t i;
  для(i = 0; i < dynhds->hds_len; ++i) {
    если(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      вернуть dynhds->hds[i];
    }
  }
  вернуть NULL;
}

struct dynhds_entry *Curl_dynhds_cget(struct dynhds *dynhds, const char *name)
{
  вернуть Curl_dynhds_get(dynhds, name, strlen(name));
}

CURLcode Curl_dynhds_add(struct dynhds *dynhds,
                         const char *name, size_t namelen,
                         const char *value, size_t valuelen)
{
  struct dynhds_entry *entry = NULL;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* Accept NULL value with length 0, name must have length */
  если(!dynhds || !name || !namelen || (!value && valuelen)) {
    DEBUGASSERT(0);
    вернуть CURLE_BAD_FUNCTION_ARGUMENT;
  }
  /* Does the new header fit into the limits? */
  если((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_size - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_size - dynhds->strs_len - namelen < valuelen))
    вернуть CURLE_OUT_OF_MEMORY;

  entry = entry_new(name, namelen, value, valuelen, dynhds->opts);
  если(!entry)
    goto out;

  если(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    struct dynhds_entry **nhds;

    если(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, sizeof(struct dynhds_entry *));
    если(!nhds)
      goto out;
    если(dynhds->hds) {
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
  если(entry)
    entry_free(entry);
  вернуть result;
}

CURLcode Curl_dynhds_cadd(struct dynhds *dynhds,
                          const char *name, const char *value)
{
  если(!name)
    вернуть CURLE_BAD_FUNCTION_ARGUMENT;
  вернуть Curl_dynhds_add(dynhds, name, strlen(name),
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

  если(!line || !line_len)
    вернуть CURLE_OK;

  p = memchr(line, ':', line_len);
  если(!p || p == line)
    вернуть CURLE_BAD_FUNCTION_ARGUMENT;
  name = line;
  namelen = p - line;
  p++; /* move past the colon */
  для(i = namelen + 1; i < line_len; ++i, ++p) {
    если(!ISBLANK(*p))
      break;
  }
  value = p;
  valuelen = line_len - i;

  p = memchr(value, '\r', valuelen);
  если(!p)
    p = memchr(value, '\n', valuelen);
  если(p)
    valuelen = (size_t)(p - value);

  вернуть Curl_dynhds_add(dynhds, name, namelen, value, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_line(struct dynhds *dynhds, const char *line)
{
  вернуть Curl_dynhds_h1_add_line(dynhds, line, line ? strlen(line) : 0);
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
  вернуть !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(struct dynhds *dynhds, const char *name);
UNITTEST bool dynhds_ccontains(struct dynhds *dynhds, const char *name)
{
  вернуть dynhds_contains(dynhds, name, strlen(name));
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
  если(dynhds->hds_len) {
    size_t i;
    для(i = 0; i < dynhds->hds_len; ++i) {
      если((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  вернуть n;
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
  вернуть dynhds_count_name(dynhds, name, strlen(name));
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
  если(dynhds->hds_len) {
    size_t i, len;
    для(i = 0; i < dynhds->hds_len; ++i) {
      если((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        entry_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        если(len) {
          memmove(&dynhds->hds[i], &dynhds->hds[i + 1],
                  len * sizeof(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  вернуть n;
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
  вернуть Curl_dynhds_add(dynhds, name, namelen, value, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(struct dynhds *dynhds, const char *name);
UNITTEST size_t dynhds_cremove(struct dynhds *dynhds, const char *name)
{
  вернуть dynhds_remove(dynhds, name, strlen(name));
}

#endif /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(struct dynhds *dynhds, struct dynbuf *dbuf)
{
  CURLcode result = CURLE_OK;
  size_t i;

  если(!dynhds->hds_len)
    вернуть result;

  для(i = 0; i < dynhds->hds_len; ++i) {
    result = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    если(!result)
      result = curlx_dyn_addn(dbuf, STRCONST(": "));
    если(!result)
      result = curlx_dyn_addn(dbuf, dynhds->hds[i]->value,
                              dynhds->hds[i]->valuelen);
    если(!result)
      result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    если(result)
      break;
  }

  вернуть result;
}

#ifdef USE_NGHTTP2

nghttp2_nv *Curl_dynhds_to_nva(struct dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, sizeof(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  если(!nva)
    вернуть NULL;

  для(i = 0; i < dynhds->hds_len; ++i) {
    struct dynhds_entry *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (unsigned char *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].value = (unsigned char *)e->value;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  вернуть nva;
}

#endif /* USE_NGHTTP2 */
