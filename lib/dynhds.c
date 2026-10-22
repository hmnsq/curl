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

ساكن بنية dynhds_اندراج *اندراج_new(ثابت حرف *name, size_t namelen,
                                      ثابت حرف *قدر, size_t valuelen,
                                      صحيح opts)
{
  بنية dynhds_اندراج *e;
  حرف *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  إذا((SIZE_MAX - namelen - valuelen) < (حجم(*e) + 2))
    أرجع عدم;
  e = curlx_calloc(1, حجم(*e) + namelen + valuelen + 2);
  إذا(!e)
    أرجع عدم;
  e->name = p = (حرف *)e + حجم(*e);
  حافظہ_نقل(p, name, namelen);
  e->namelen = namelen;
  e->قدر = p += namelen + 1; /* leave a \0 at the end of name */
  إذا(valuelen) {
    حافظہ_نقل(p, قدر, valuelen);
    e->valuelen = valuelen;
  }
  إذا(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  أرجع e;
}

ساكن فراغ اندراج_free(بنية dynhds_اندراج *e)
{
  curlx_free(e);
}

فراغ Curl_dynhds_init(بنية dynhds *dynhds, size_t max_entries,
                      size_t max_strs_ناپ)
{
  DEBUGASSERT(dynhds);
  DEBUGASSERT(max_strs_ناپ);
  dynhds->hds = عدم;
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
  dynhds->max_entries = max_entries;
  dynhds->max_strs_ناپ = max_strs_ناپ;
  dynhds->opts = 0;
}

فراغ Curl_dynhds_free(بنية dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  إذا(dynhds->hds && dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    لكل(i = 0; i < dynhds->hds_len; ++i) {
      اندراج_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

فراغ Curl_dynhds_reset(بنية dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  إذا(dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    لكل(i = 0; i < dynhds->hds_len; ++i) {
      اندراج_free(dynhds->hds[i]);
      dynhds->hds[i] = عدم;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_گنتی(بنية dynhds *dynhds)
{
  أرجع dynhds->hds_len;
}

فراغ Curl_dynhds_سیٹ_opts(بنية dynhds *dynhds, صحيح opts)
{
  dynhds->opts = opts;
}

بنية dynhds_اندراج *Curl_dynhds_getn(بنية dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  أرجع (n < dynhds->hds_len) ? dynhds->hds[n] : عدم;
}

بنية dynhds_اندراج *Curl_dynhds_get(بنية dynhds *dynhds, ثابت حرف *name,
                                     size_t namelen)
{
  size_t i;
  لكل(i = 0; i < dynhds->hds_len; ++i) {
    إذا(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      أرجع dynhds->hds[i];
    }
  }
  أرجع عدم;
}

بنية dynhds_اندراج *Curl_dynhds_cget(بنية dynhds *dynhds, ثابت حرف *name)
{
  أرجع Curl_dynhds_get(dynhds, name, لڑی_لمبائی(name));
}

CURLcode Curl_dynhds_جوڑو(بنية dynhds *dynhds,
                         ثابت حرف *name, size_t namelen,
                         ثابت حرف *قدر, size_t valuelen)
{
  بنية dynhds_اندراج *اندراج = عدم;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;

  /* Accept NULL value with length 0, name must have length */
  إذا(!dynhds || !name || !namelen || (!قدر && valuelen)) {
    DEBUGASSERT(0);
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }
  /* Does the new header fit into the limits? */
  إذا((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_ناپ - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_ناپ - dynhds->strs_len - namelen < valuelen))
    أرجع CURLE_باہرلا_دا_حافظہ;

  اندراج = اندراج_new(name, namelen, قدر, valuelen, dynhds->opts);
  إذا(!اندراج)
    اذهب باہرلا;

  إذا(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    بنية dynhds_اندراج **nhds;

    إذا(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, حجم(بنية dynhds_اندراج *));
    إذا(!nhds)
      اذهب باہرلا;
    إذا(dynhds->hds) {
      حافظہ_نقل(nhds, dynhds->hds,
             dynhds->hds_len * حجم(بنية dynhds_اندراج *));
      curlx_safefree(dynhds->hds);
    }
    dynhds->hds = nhds;
    dynhds->hds_allc = nallc;
  }
  dynhds->hds[dynhds->hds_len++] = اندراج;
  اندراج = عدم;
  dynhds->strs_len += namelen + valuelen;
  نتیجہ = CURLE_OK;

باہرلا:
  إذا(اندراج)
    اندراج_free(اندراج);
  أرجع نتیجہ;
}

CURLcode Curl_dynhds_cadd(بنية dynhds *dynhds,
                          ثابت حرف *name, ثابت حرف *قدر)
{
  إذا(!name)
    أرجع CURLE_BAD_FUNCTION_دلیل;
  أرجع Curl_dynhds_جوڑو(dynhds, name, لڑی_لمبائی(name),
                         قدر, قدر ? لڑی_لمبائی(قدر) : 0);
}

CURLcode Curl_dynhds_h1_جوڑو_سطر(بنية dynhds *dynhds,
                                 ثابت حرف *سطر, size_t سطر_len)
{
  ثابت حرف *p;
  ثابت حرف *name;
  size_t namelen;
  ثابت حرف *قدر;
  size_t valuelen, i;

  إذا(!سطر || !سطر_len)
    أرجع CURLE_OK;

  p = memchr(سطر, ':', سطر_len);
  إذا(!p || p == سطر)
    أرجع CURLE_BAD_FUNCTION_دلیل;
  name = سطر;
  namelen = p - سطر;
  p++; /* move past the colon */
  لكل(i = namelen + 1; i < سطر_len; ++i, ++p) {
    إذا(!ISBLANK(*p))
      اكسر;
  }
  قدر = p;
  valuelen = سطر_len - i;

  p = memchr(قدر, '\r', valuelen);
  إذا(!p)
    p = memchr(قدر, '\n', valuelen);
  إذا(p)
    valuelen = (size_t)(p - قدر);

  أرجع Curl_dynhds_جوڑو(dynhds, name, namelen, قدر, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_سطر(بنية dynhds *dynhds, ثابت حرف *سطر)
{
  أرجع Curl_dynhds_h1_جوڑو_سطر(dynhds, سطر, سطر ? لڑی_لمبائی(سطر) : 0);
}

#جے_تعریف_شدہ UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_شامل_اے(بنية dynhds *dynhds,
                              ثابت حرف *name, size_t namelen);
UNITTEST bool dynhds_شامل_اے(بنية dynhds *dynhds,
                              ثابت حرف *name, size_t namelen)
{
  أرجع !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(بنية dynhds *dynhds, ثابت حرف *name);
UNITTEST bool dynhds_ccontains(بنية dynhds *dynhds, ثابت حرف *name)
{
  أرجع dynhds_شامل_اے(dynhds, name, لڑی_لمبائی(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_گنتی_name(بنية dynhds *dynhds,
                                  ثابت حرف *name, size_t namelen);
UNITTEST size_t dynhds_گنتی_name(بنية dynhds *dynhds,
                                  ثابت حرف *name, size_t namelen)
{
  size_t n = 0;
  إذا(dynhds->hds_len) {
    size_t i;
    لكل(i = 0; i < dynhds->hds_len; ++i) {
      إذا((namelen == dynhds->hds[i]->namelen) &&
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
  أرجع dynhds_گنتی_name(dynhds, name, لڑی_لمبائی(name));
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
  إذا(dynhds->hds_len) {
    size_t i, len;
    لكل(i = 0; i < dynhds->hds_len; ++i) {
      إذا((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        اندراج_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        إذا(len) {
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
UNITTEST CURLcode dynhds_سیٹ(بنية dynhds *dynhds,
                             ثابت حرف *name, size_t namelen,
                             ثابت حرف *قدر, size_t valuelen);
UNITTEST CURLcode dynhds_سیٹ(بنية dynhds *dynhds,
                             ثابت حرف *name, size_t namelen,
                             ثابت حرف *قدر, size_t valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  أرجع Curl_dynhds_جوڑو(dynhds, name, namelen, قدر, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(بنية dynhds *dynhds, ثابت حرف *name);
UNITTEST size_t dynhds_cremove(بنية dynhds *dynhds, ثابت حرف *name)
{
  أرجع dynhds_remove(dynhds, name, لڑی_لمبائی(name));
}

#جے_انت /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(بنية dynhds *dynhds, بنية dynbuf *dbuf)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t i;

  إذا(!dynhds->hds_len)
    أرجع نتیجہ;

  لكل(i = 0; i < dynhds->hds_len; ++i) {
    نتیجہ = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    إذا(!نتیجہ)
      نتیجہ = curlx_dyn_addn(dbuf, STRCONST(": "));
    إذا(!نتیجہ)
      نتیجہ = curlx_dyn_addn(dbuf, dynhds->hds[i]->قدر,
                              dynhds->hds[i]->valuelen);
    إذا(!نتیجہ)
      نتیجہ = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    إذا(نتیجہ)
      اكسر;
  }

  أرجع نتیجہ;
}

#جے_تعریف_شدہ USE_NGHTTP2

nghttp2_nv *Curl_dynhds_منزل_nva(بنية dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, حجم(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  إذا(!nva)
    أرجع عدم;

  لكل(i = 0; i < dynhds->hds_len; ++i) {
    بنية dynhds_اندراج *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (بنانشانی حرف *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].قدر = (بنانشانی حرف *)e->قدر;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  أرجع nva;
}

#جے_انت /* USE_NGHTTP2 */
