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
#ضمن "curl_setup.h"

#ضمن "dynhds.h"
#ضمن "strcase.h"

ساكن بنية dynhds_entry *entry_new(ثابت حرف *name, size_t namelen,
                                      ثابت حرف *value, size_t valuelen,
                                      صحيح opts)
{
  بنية dynhds_entry *e;
  حرف *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  𐡄𐡍((SIZE_MAX - namelen - valuelen) < (حجم(*e) + 2))
    أرجع عدم;
  e = curlx_calloc(1, حجم(*e) + namelen + valuelen + 2);
  𐡄𐡍(!e)
    أرجع عدم;
  e->name = p = (حرف *)e + حجم(*e);
  memcpy(p, name, namelen);
  e->namelen = namelen;
  e->value = p += namelen + 1; /* leave a \0 at the end of name */
  𐡄𐡍(valuelen) {
    memcpy(p, value, valuelen);
    e->valuelen = valuelen;
  }
  𐡄𐡍(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  أرجع e;
}

ساكن فراغ entry_free(بنية dynhds_entry *e)
{
  curlx_free(e);
}

فراغ Curl_dynhds_init(بنية dynhds *dynhds, size_t max_entries,
                      size_t max_strs_size)
{
  DEBUGASSERT(dynhds);
  DEBUGASSERT(max_strs_size);
  dynhds->hds = عدم;
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
  dynhds->max_entries = max_entries;
  dynhds->max_strs_size = max_strs_size;
  dynhds->opts = 0;
}

فراغ Curl_dynhds_free(بنية dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  𐡄𐡍(dynhds->hds && dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    لكل(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

فراغ Curl_dynhds_reset(بنية dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  𐡄𐡍(dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    لكل(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
      dynhds->hds[i] = عدم;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_count(بنية dynhds *dynhds)
{
  أرجع dynhds->hds_len;
}

فراغ Curl_dynhds_set_opts(بنية dynhds *dynhds, صحيح opts)
{
  dynhds->opts = opts;
}

بنية dynhds_entry *Curl_dynhds_getn(بنية dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  أرجع (n < dynhds->hds_len) ? dynhds->hds[n] : عدم;
}

بنية dynhds_entry *Curl_dynhds_get(بنية dynhds *dynhds, ثابت حرف *name,
                                     size_t namelen)
{
  size_t i;
  لكل(i = 0; i < dynhds->hds_len; ++i) {
    𐡄𐡍(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      أرجع dynhds->hds[i];
    }
  }
  أرجع عدم;
}

بنية dynhds_entry *Curl_dynhds_cget(بنية dynhds *dynhds, ثابت حرف *name)
{
  أرجع Curl_dynhds_get(dynhds, name, strlen(name));
}

CURLcode Curl_dynhds_add(بنية dynhds *dynhds,
                         ثابت حرف *name, size_t namelen,
                         ثابت حرف *value, size_t valuelen)
{
  بنية dynhds_entry *entry = عدم;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* Accept NULL value with length 0, name must have length */
  𐡄𐡍(!dynhds || !name || !namelen || (!value && valuelen)) {
    DEBUGASSERT(0);
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  }
  /* Does the new header fit into the limits? */
  𐡄𐡍((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_size - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_size - dynhds->strs_len - namelen < valuelen))
    أرجع CURLE_OUT_OF_MEMORY;

  entry = entry_new(name, namelen, value, valuelen, dynhds->opts);
  𐡄𐡍(!entry)
    اذهب out;

  𐡄𐡍(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    بنية dynhds_entry **nhds;

    𐡄𐡍(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, حجم(بنية dynhds_entry *));
    𐡄𐡍(!nhds)
      اذهب out;
    𐡄𐡍(dynhds->hds) {
      memcpy(nhds, dynhds->hds,
             dynhds->hds_len * حجم(بنية dynhds_entry *));
      curlx_safefree(dynhds->hds);
    }
    dynhds->hds = nhds;
    dynhds->hds_allc = nallc;
  }
  dynhds->hds[dynhds->hds_len++] = entry;
  entry = عدم;
  dynhds->strs_len += namelen + valuelen;
  result = CURLE_OK;

out:
  𐡄𐡍(entry)
    entry_free(entry);
  أرجع result;
}

CURLcode Curl_dynhds_cadd(بنية dynhds *dynhds,
                          ثابت حرف *name, ثابت حرف *value)
{
  𐡄𐡍(!name)
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  أرجع Curl_dynhds_add(dynhds, name, strlen(name),
                         value, value ? strlen(value) : 0);
}

CURLcode Curl_dynhds_h1_add_line(بنية dynhds *dynhds,
                                 ثابت حرف *line, size_t line_len)
{
  ثابت حرف *p;
  ثابت حرف *name;
  size_t namelen;
  ثابت حرف *value;
  size_t valuelen, i;

  𐡄𐡍(!line || !line_len)
    أرجع CURLE_OK;

  p = memchr(line, ':', line_len);
  𐡄𐡍(!p || p == line)
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  name = line;
  namelen = p - line;
  p++; /* move past the colon */
  لكل(i = namelen + 1; i < line_len; ++i, ++p) {
    𐡄𐡍(!ISBLANK(*p))
      اكسر;
  }
  value = p;
  valuelen = line_len - i;

  p = memchr(value, '\r', valuelen);
  𐡄𐡍(!p)
    p = memchr(value, '\n', valuelen);
  𐡄𐡍(p)
    valuelen = (size_t)(p - value);

  أرجع Curl_dynhds_add(dynhds, name, namelen, value, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_line(بنية dynhds *dynhds, ثابت حرف *line)
{
  أرجع Curl_dynhds_h1_add_line(dynhds, line, line ? strlen(line) : 0);
}

#ifdef UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_contains(بنية dynhds *dynhds,
                              ثابت حرف *name, size_t namelen);
UNITTEST bool dynhds_contains(بنية dynhds *dynhds,
                              ثابت حرف *name, size_t namelen)
{
  أرجع !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(بنية dynhds *dynhds, ثابت حرف *name);
UNITTEST bool dynhds_ccontains(بنية dynhds *dynhds, ثابت حرف *name)
{
  أرجع dynhds_contains(dynhds, name, strlen(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_count_name(بنية dynhds *dynhds,
                                  ثابت حرف *name, size_t namelen);
UNITTEST size_t dynhds_count_name(بنية dynhds *dynhds,
                                  ثابت حرف *name, size_t namelen)
{
  size_t n = 0;
  𐡄𐡍(dynhds->hds_len) {
    size_t i;
    لكل(i = 0; i < dynhds->hds_len; ++i) {
      𐡄𐡍((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  أرجع n;
}

/**
 * Return how often the given null-terminated name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ccount_name(بنية dynhds *dynhds,
                                        ثابت حرف *name);
UNITTEST size_t dynhds_ccount_name(بنية dynhds *dynhds,
                                        ثابت حرف *name)
{
  أرجع dynhds_count_name(dynhds, name, strlen(name));
}

/**
 * Remove all entries with the given name.
 * Returns number of entries removed.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_remove(بنية dynhds *dynhds,
                              ثابت حرف *name, size_t namelen);
UNITTEST size_t dynhds_remove(بنية dynhds *dynhds,
                              ثابت حرف *name, size_t namelen)
{
  size_t n = 0;
  𐡄𐡍(dynhds->hds_len) {
    size_t i, len;
    لكل(i = 0; i < dynhds->hds_len; ++i) {
      𐡄𐡍((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        entry_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        𐡄𐡍(len) {
          memmove(&dynhds->hds[i], &dynhds->hds[i + 1],
                  len * حجم(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  أرجع n;
}

/**
 * Set the give header name and value, replacing any entries with
 * the same name. The header is added at the end of all (remaining)
 * entries.
 *
 * @unittest 2602
 */
UNITTEST CURLcode dynhds_set(بنية dynhds *dynhds,
                             ثابت حرف *name, size_t namelen,
                             ثابت حرف *value, size_t valuelen);
UNITTEST CURLcode dynhds_set(بنية dynhds *dynhds,
                             ثابت حرف *name, size_t namelen,
                             ثابت حرف *value, size_t valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  أرجع Curl_dynhds_add(dynhds, name, namelen, value, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(بنية dynhds *dynhds, ثابت حرف *name);
UNITTEST size_t dynhds_cremove(بنية dynhds *dynhds, ثابت حرف *name)
{
  أرجع dynhds_remove(dynhds, name, strlen(name));
}

#endif /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(بنية dynhds *dynhds, بنية dynbuf *dbuf)
{
  CURLcode result = CURLE_OK;
  size_t i;

  𐡄𐡍(!dynhds->hds_len)
    أرجع result;

  لكل(i = 0; i < dynhds->hds_len; ++i) {
    result = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    𐡄𐡍(!result)
      result = curlx_dyn_addn(dbuf, STRCONST(": "));
    𐡄𐡍(!result)
      result = curlx_dyn_addn(dbuf, dynhds->hds[i]->value,
                              dynhds->hds[i]->valuelen);
    𐡄𐡍(!result)
      result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    𐡄𐡍(result)
      اكسر;
  }

  أرجع result;
}

#ifdef USE_NGHTTP2

nghttp2_nv *Curl_dynhds_to_nva(بنية dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, حجم(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  𐡄𐡍(!nva)
    أرجع عدم;

  لكل(i = 0; i < dynhds->hds_len; ++i) {
    بنية dynhds_entry *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (unsigned حرف *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].value = (unsigned حرف *)e->value;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  أرجع nva;
}

#endif /* USE_NGHTTP2 */
