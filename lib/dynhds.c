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
#شامل "curl_setup.h"

#شامل "dynhds.h"
#شامل "strcase.h"

ایستا ساختار dynhds_entry *entry_new(ثابت نویسه *name, size_t namelen,
                                      ثابت نویسه *مقدار, size_t valuelen,
                                      صحیح opts)
{
  ساختار dynhds_entry *e;
  نویسه *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  اگر((SIZE_MAX - namelen - valuelen) < (اندازه(*e) + 2))
    بازگشت پوچ;
  e = curlx_calloc(1, اندازه(*e) + namelen + valuelen + 2);
  اگر(!e)
    بازگشت پوچ;
  e->name = p = (نویسه *)e + اندازه(*e);
  memcpy(p, name, namelen);
  e->namelen = namelen;
  e->مقدار = p += namelen + 1; /* leave a \0 at the end of name */
  اگر(valuelen) {
    memcpy(p, مقدار, valuelen);
    e->valuelen = valuelen;
  }
  اگر(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  بازگشت e;
}

ایستا تهی entry_free(ساختار dynhds_entry *e)
{
  curlx_free(e);
}

تهی Curl_dynhds_init(ساختار dynhds *dynhds, size_t max_entries,
                      size_t max_strs_size)
{
  DEBUGASSERT(dynhds);
  DEBUGASSERT(max_strs_size);
  dynhds->hds = پوچ;
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
  dynhds->max_entries = max_entries;
  dynhds->max_strs_size = max_strs_size;
  dynhds->opts = 0;
}

تهی Curl_dynhds_free(ساختار dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  اگر(dynhds->hds && dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    برای(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

تهی Curl_dynhds_reset(ساختار dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  اگر(dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    برای(i = 0; i < dynhds->hds_len; ++i) {
      entry_free(dynhds->hds[i]);
      dynhds->hds[i] = پوچ;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_شمار(ساختار dynhds *dynhds)
{
  بازگشت dynhds->hds_len;
}

تهی Curl_dynhds_set_opts(ساختار dynhds *dynhds, صحیح opts)
{
  dynhds->opts = opts;
}

ساختار dynhds_entry *Curl_dynhds_getn(ساختار dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  بازگشت (n < dynhds->hds_len) ? dynhds->hds[n] : پوچ;
}

ساختار dynhds_entry *Curl_dynhds_get(ساختار dynhds *dynhds, ثابت نویسه *name,
                                     size_t namelen)
{
  size_t i;
  برای(i = 0; i < dynhds->hds_len; ++i) {
    اگر(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      بازگشت dynhds->hds[i];
    }
  }
  بازگشت پوچ;
}

ساختار dynhds_entry *Curl_dynhds_cget(ساختار dynhds *dynhds, ثابت نویسه *name)
{
  بازگشت Curl_dynhds_get(dynhds, name, strlen(name));
}

CURLcode Curl_dynhds_add(ساختار dynhds *dynhds,
                         ثابت نویسه *name, size_t namelen,
                         ثابت نویسه *مقدار, size_t valuelen)
{
  ساختار dynhds_entry *entry = پوچ;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* Accept NULL value with length 0, name must have length */
  اگر(!dynhds || !name || !namelen || (!مقدار && valuelen)) {
    DEBUGASSERT(0);
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }
  /* Does the new header fit into the limits? */
  اگر((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_size - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_size - dynhds->strs_len - namelen < valuelen))
    بازگشت CURLE_OUT_OF_MEMORY;

  entry = entry_new(name, namelen, مقدار, valuelen, dynhds->opts);
  اگر(!entry)
    برو out;

  اگر(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    ساختار dynhds_entry **nhds;

    اگر(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, اندازه(ساختار dynhds_entry *));
    اگر(!nhds)
      برو out;
    اگر(dynhds->hds) {
      memcpy(nhds, dynhds->hds,
             dynhds->hds_len * اندازه(ساختار dynhds_entry *));
      curlx_safefree(dynhds->hds);
    }
    dynhds->hds = nhds;
    dynhds->hds_allc = nallc;
  }
  dynhds->hds[dynhds->hds_len++] = entry;
  entry = پوچ;
  dynhds->strs_len += namelen + valuelen;
  result = CURLE_OK;

out:
  اگر(entry)
    entry_free(entry);
  بازگشت result;
}

CURLcode Curl_dynhds_cadd(ساختار dynhds *dynhds,
                          ثابت نویسه *name, ثابت نویسه *مقدار)
{
  اگر(!name)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  بازگشت Curl_dynhds_add(dynhds, name, strlen(name),
                         مقدار, مقدار ? strlen(مقدار) : 0);
}

CURLcode Curl_dynhds_h1_add_سطر(ساختار dynhds *dynhds,
                                 ثابت نویسه *سطر, size_t سطر_len)
{
  ثابت نویسه *p;
  ثابت نویسه *name;
  size_t namelen;
  ثابت نویسه *مقدار;
  size_t valuelen, i;

  اگر(!سطر || !سطر_len)
    بازگشت CURLE_OK;

  p = memchr(سطر, ':', سطر_len);
  اگر(!p || p == سطر)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  name = سطر;
  namelen = p - سطر;
  p++; /* move past the colon */
  برای(i = namelen + 1; i < سطر_len; ++i, ++p) {
    اگر(!ISBLANK(*p))
      بشکن;
  }
  مقدار = p;
  valuelen = سطر_len - i;

  p = memchr(مقدار, '\r', valuelen);
  اگر(!p)
    p = memchr(مقدار, '\n', valuelen);
  اگر(p)
    valuelen = (size_t)(p - مقدار);

  بازگشت Curl_dynhds_add(dynhds, name, namelen, مقدار, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_سطر(ساختار dynhds *dynhds, ثابت نویسه *سطر)
{
  بازگشت Curl_dynhds_h1_add_سطر(dynhds, سطر, سطر ? strlen(سطر) : 0);
}

#ifdef UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_contains(ساختار dynhds *dynhds,
                              ثابت نویسه *name, size_t namelen);
UNITTEST bool dynhds_contains(ساختار dynhds *dynhds,
                              ثابت نویسه *name, size_t namelen)
{
  بازگشت !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(ساختار dynhds *dynhds, ثابت نویسه *name);
UNITTEST bool dynhds_ccontains(ساختار dynhds *dynhds, ثابت نویسه *name)
{
  بازگشت dynhds_contains(dynhds, name, strlen(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_شمار_name(ساختار dynhds *dynhds,
                                  ثابت نویسه *name, size_t namelen);
UNITTEST size_t dynhds_شمار_name(ساختار dynhds *dynhds,
                                  ثابت نویسه *name, size_t namelen)
{
  size_t n = 0;
  اگر(dynhds->hds_len) {
    size_t i;
    برای(i = 0; i < dynhds->hds_len; ++i) {
      اگر((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  بازگشت n;
}

/**
 * Return how often the given null-terminated name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ccount_name(ساختار dynhds *dynhds,
                                        ثابت نویسه *name);
UNITTEST size_t dynhds_ccount_name(ساختار dynhds *dynhds,
                                        ثابت نویسه *name)
{
  بازگشت dynhds_شمار_name(dynhds, name, strlen(name));
}

/**
 * Remove all entries with the given name.
 * Returns number of entries removed.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_remove(ساختار dynhds *dynhds,
                              ثابت نویسه *name, size_t namelen);
UNITTEST size_t dynhds_remove(ساختار dynhds *dynhds,
                              ثابت نویسه *name, size_t namelen)
{
  size_t n = 0;
  اگر(dynhds->hds_len) {
    size_t i, len;
    برای(i = 0; i < dynhds->hds_len; ++i) {
      اگر((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        entry_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        اگر(len) {
          memmove(&dynhds->hds[i], &dynhds->hds[i + 1],
                  len * اندازه(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  بازگشت n;
}

/**
 * Set the give header name and value, replacing any entries with
 * the same name. The header is added at the end of all (remaining)
 * entries.
 *
 * @unittest 2602
 */
UNITTEST CURLcode dynhds_set(ساختار dynhds *dynhds,
                             ثابت نویسه *name, size_t namelen,
                             ثابت نویسه *مقدار, size_t valuelen);
UNITTEST CURLcode dynhds_set(ساختار dynhds *dynhds,
                             ثابت نویسه *name, size_t namelen,
                             ثابت نویسه *مقدار, size_t valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  بازگشت Curl_dynhds_add(dynhds, name, namelen, مقدار, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(ساختار dynhds *dynhds, ثابت نویسه *name);
UNITTEST size_t dynhds_cremove(ساختار dynhds *dynhds, ثابت نویسه *name)
{
  بازگشت dynhds_remove(dynhds, name, strlen(name));
}

#endif /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(ساختار dynhds *dynhds, ساختار dynbuf *dbuf)
{
  CURLcode result = CURLE_OK;
  size_t i;

  اگر(!dynhds->hds_len)
    بازگشت result;

  برای(i = 0; i < dynhds->hds_len; ++i) {
    result = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    اگر(!result)
      result = curlx_dyn_addn(dbuf, STRCONST(": "));
    اگر(!result)
      result = curlx_dyn_addn(dbuf, dynhds->hds[i]->مقدار,
                              dynhds->hds[i]->valuelen);
    اگر(!result)
      result = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    اگر(result)
      بشکن;
  }

  بازگشت result;
}

#ifdef USE_NGHTTP2

nghttp2_nv *Curl_dynhds_to_nva(ساختار dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, اندازه(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  اگر(!nva)
    بازگشت پوچ;

  برای(i = 0; i < dynhds->hds_len; ++i) {
    ساختار dynhds_entry *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (unsigned نویسه *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].مقدار = (unsigned نویسه *)e->مقدار;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  بازگشت nva;
}

#endif /* USE_NGHTTP2 */
